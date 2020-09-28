/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.

*******************************************************************************/

#include <common.h>
#include <command.h>

#include <nand.h>

#include "sflash/mvSFlash.h"
#include "mvCtrlEnvRegs.h"
#include "mvCommon.h"
#include "mvOs.h"
#include "eth-phy/mvEthPhy.h"
#include "mvBoardEnvLib.h"
#include "mvPrestera.h"
#include "gpp/mvGpp.h"

extern MV_U32 mvCpuIfTargetWinBaseLowGet(MV_TARGET target);
extern void mvMD5(unsigned char const *buf, unsigned len, unsigned char* digest);
extern void sflashInfoCopy(flash_info_t *flash_info, MV_SFLASH_INFO *pFlash);
extern int do_flerase (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_protect (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_mem_cmp (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_mem_cp ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_dhcp (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_ping (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_tftpb (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

extern flash_info_t flash_info[];	/* info for FLASH chips */
extern ulong NetBootFileXferSize;	/* The actual transferred size of the bootfile (in bytes) */

static MV_SFLASH_INFO sflash;

/*
#define MV_DEBUG
*/
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

#define FLASH_BUG

#define TFTP_ADDR	"0x800000"
#define TFTP_ADDR_CMP	"0x1000000"

/* Look up command in command table */
#define FIND_CMD(cmdtp, name) \
{ \
		if ((cmdtp = find_cmd(name)) == NULL) { \
		printf ("Unknown command '%s' - try 'help'\n", name); \
		return 1; \
	} \
} \

#define CHECK_CMD_STATUS(origFunc) \
{ \
        int mvStatus = origFunc; \
        if (0 != mvStatus) \
        { \
        printf("Test Failed !!!\n"); \
            return mvStatus; \
        } \
}

#define CHECK_CTRL_C(op) \
{ \
	if (ctrlc()) { \
		op; \
		putc ('\n'); \
		printf("Interrupted by user\n"); \
		return 1; \
	} \
}

#define DRAM_SIZE		_64M
#define DRAM_PROTECT		_16M
#define DRAM_LIMIT		DRAM_BASE + DRAM_SIZE
#define FLASH_BANK		1
#define U_BOOT_PROTECT		_1M

#define SECTOR_ID(addr)		(addr - sflash.baseAddr)/sflash.sectorSize

MV_U32 random(void)
{
	int i,rand;
	MV_U32 random[16];
	unsigned char digest[16];

	MV_REG_BIT_SET(0x1478, BIT7);
	for(i=0; i < 16;i++)
		random[i] = MV_REG_READ(0x1470);

	/* Run MD5 over the ftdll buffer */
	mvMD5((unsigned char*)random, 64, (MV_U8*)&digest);
	memcpy(&rand,digest,4);
	return   rand;
}

MV_STATUS eraseBySector(unsigned long startAddr, unsigned long endAddr)
{
	cmd_tbl_t *tmpcmdtp;
	MV_8 strErase[15];

	printf("erase %d:%d-%d\n",FLASH_BANK,
					(MV_32)SECTOR_ID(startAddr),
					(MV_32)SECTOR_ID(endAddr));

	sprintf((char*)strErase,"%d:%d-%d",
			FLASH_BANK, (MV_U32)SECTOR_ID(startAddr),
			(MV_U32)SECTOR_ID(endAddr));
	char* argv[] = {"erase", strErase};
	FIND_CMD(tmpcmdtp,"erase");
	CHECK_CMD_STATUS( do_flerase (tmpcmdtp, 0, 4, argv) );

	return MV_OK;
}

MV_STATUS eraseByAddress
(
	unsigned long startAddr,
	unsigned long size
)
{
	cmd_tbl_t *tmpcmdtp;
	MV_U32 alignedAddr,alignedSize;
	MV_8 strStartAddr[11],strSize[11];

	alignedAddr = startAddr -   (startAddr % sflash.sectorSize);
	alignedSize = size 	+   (startAddr % sflash.sectorSize);
	DB( printf("Start Address: 0x%08X Size: 0x%04X\n",
					startAddr,size) );
	DB( printf("Aligned Address: 0x%08X Aligned Size: 0x%04X\n",
					alignedAddr,alignedSize) );
	printf("erase 0x%08X +0x%04X",alignedAddr,alignedSize);

	sprintf((char*)strStartAddr,"0x%x",alignedAddr);
	sprintf((char*)strSize,"+0x%x",alignedSize);

	DB( printf("\nDebug: Start Address: %s\tSize %s\n",strStartAddr,strSize) );
	char* argv[] = {"erase", strStartAddr,strSize};

	FIND_CMD(tmpcmdtp,"erase");
	CHECK_CMD_STATUS( do_flerase (tmpcmdtp, 0, 3, argv) );

	return MV_OK;
}

void initFlashInfo(void)
{
        sflash.baseAddr         = mvCpuIfTargetWinBaseLowGet(SPI_CS);
        sflash.sectorNumber     = flash_info[FLASH_BANK-1].sector_count;
        sflash.sectorSize       = flash_info[FLASH_BANK-1].size/flash_info[FLASH_BANK-1].sector_count;

	DB( printf("Base: 0x%08X\nSectors: 0x%08X\nSector Size: 0x%08X\nFlashSize: 0x%08X\n",
		sflash.baseAddr,sflash.sectorNumber,sflash.sectorSize,sflash.sectorNumber*sflash.sectorSize) );
}

int protectFlash(void)
{
	cmd_tbl_t *tmpcmdtp;
	char* argv[] = {"protect", "on","all"};

	printf("protect on all\n");

	FIND_CMD(tmpcmdtp,"protect");
	CHECK_CMD_STATUS( do_protect (tmpcmdtp, 0, 3, argv) );

	return 0;
}

int do_flash_test(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned long size,flashAddr,dramAddr;
	cmd_tbl_t *tmpcmdtp;

	initFlashInfo();
	MV_U32 FLASH_SIZE =  sflash.sectorNumber*sflash.sectorSize;
	MV_U32 FLASH_LIMIT=  sflash.baseAddr + FLASH_SIZE;

	printf("protect off 0x%08X +0x%08X\n",sflash.baseAddr+U_BOOT_PROTECT,
							  FLASH_SIZE-U_BOOT_PROTECT);
	{
	MV_8 startAddr[11],strSize[11];
	sprintf((char*)startAddr,"0x%x",sflash.baseAddr+U_BOOT_PROTECT);
	sprintf((char*)strSize,"+0x%x",FLASH_SIZE-U_BOOT_PROTECT);
	char* argv[] = {"protect", "off",startAddr,strSize};
	FIND_CMD(tmpcmdtp,"protect");
	CHECK_CMD_STATUS( do_protect (tmpcmdtp, 0, 4, argv) );
	}

	for (;;)
	{
		CHECK_CTRL_C(protectFlash())

		do
		{
			int x = random();
			flashAddr = sflash.baseAddr + U_BOOT_PROTECT +
						(x % (FLASH_SIZE-U_BOOT_PROTECT));
		} while(flashAddr >= FLASH_LIMIT
			#if defined(FLASH_BUG)
			/* Bug in flash driver:
			   odd addresses cannot be copies */
				|| (flashAddr % 2)
			#endif
			);

		do
		{
			size   = random() % (FLASH_SIZE-U_BOOT_PROTECT);

		} while((flashAddr + size) > FLASH_LIMIT);

		do
		{
			dramAddr   = DRAM_BASE + DRAM_PROTECT + (random() % DRAM_SIZE);
		} while((dramAddr + size) > DRAM_LIMIT
			#if defined(FLASH_BUG)
			/* Bug in flash driver:
			   odd addresses cannot be copies */
				|| (dramAddr % 2)
			#endif
			);

		if (random() % 2)
		{
			CHECK_CMD_STATUS( eraseBySector(flashAddr,flashAddr+size) );
		}
		else
		{
			CHECK_CMD_STATUS( eraseByAddress(flashAddr,size) );
		}

		CHECK_CTRL_C(protectFlash())

		MV_8 srcAddr[11],destAddr[11],strSize[11];
		sprintf((char*)srcAddr,"0x%X",(MV_U32)dramAddr);
		sprintf((char*)destAddr,"0x%X",(MV_U32)flashAddr);
		sprintf((char*)strSize,"0x%X",(MV_U32)size);

		printf("cp.b  0x%08X 0x%08X 0x%04X\n",
			   (MV_U32)dramAddr, (MV_U32)flashAddr, (MV_U32)size);
		{
		DB( printf("cp.b params: Src: %s\tDest: %s\tSize: %s\n", srcAddr, destAddr, strSize) );
		char* argv[] = {"cp.b", srcAddr, destAddr, strSize};
		FIND_CMD(tmpcmdtp,"cp");
		CHECK_CMD_STATUS( do_mem_cp (tmpcmdtp, 0, 4, argv) );
		}

		CHECK_CTRL_C(protectFlash())

		printf("cmp.b 0x%08X 0x%08X 0x%04X\n",
			   (MV_U32)dramAddr, (MV_U32)flashAddr, (MV_U32)size);
		{
		DB( printf("cmp.b params: Src: %s\tDest: %s\tSize: %s\n", srcAddr, destAddr, strSize) );
		char* argv[] = {"cmp.b", srcAddr, destAddr, strSize};
		FIND_CMD(tmpcmdtp,"cmp");
		CHECK_CMD_STATUS( do_mem_cmp (tmpcmdtp, 0, 4, argv) );
		}

		printf("\n");
	}

	return 1;
}

U_BOOT_CMD(
	test_flash,      1,     1,      do_flash_test,
	"test_flash	- test flash interfaces.\n",
	"\n"
);

/* #if defined(MV_NAND_BOOT) */
#if defined(MV_NAND)
void nand_init(void);

extern int G_nand_found_flag;

int do_nand_test(void)
{
	nand_info_t *nand = &nand_info[0];
	nand_erase_options_t er_opts;
	nand_write_options_t wr_opts;
	nand_read_options_t rd_opts;
	u_char readBuff[0x40000];
	u_char* dataBuff;
	unsigned long testOffset, testMaxSize;
	unsigned long size, startAddr;
	int i, ret, regVal;

	/*
	 * Init NAND flash before the test.
	 */
	printf("\nStarting the NAND flash test.\n");
	MV_REG_WRITE(0x10000, DB_98DX4122_MPP0_7);
	MV_REG_WRITE(0x10008, DB_98DX4122_MPP16_23);
	puts ("NAND init:");
	nand_init();
	printf("\n");

	if (G_nand_found_flag == 0) /* NAND init failed */
	{
		if (mvPpChipIsXCat2() == MV_FALSE)
		{
			printf("For xCat board: recheck JP11 settings.\n");
		}
		return 0;
	}
	/* G_nand_found_flag = 0; */ /* prepare for the next test run */

	testOffset = 0xC0000;
	testMaxSize = 0x40000;
	size = testMaxSize;
	startAddr = 0;

//	dataBuff = (u_char*)malloc(size);
//	if (dataBuff == NULL){ printf("\n Malloc Fail 1"); return 0;}
	dataBuff = (u_char*)malloc(0x40000);
	if (readBuff == NULL){ printf("\n Malloc Fail 2"); return 0;}

	for(i = 0; i < size; i++)
	{
		dataBuff[i] = (i*23) % 256;
	}

	printf("Erase 0x%X - 0x%X ... ",testOffset, testMaxSize);
	memset(&er_opts, 0, sizeof(er_opts));
	er_opts.offset = testOffset;
	er_opts.length = testMaxSize;
	er_opts.quiet  = 1;
	ret = nand_erase_opts(nand, &er_opts);
	printf("status 0x%X", ret);

	printf("\nCopy to Nand Flash 0x%X - 0x%X... ", testOffset+startAddr, size);
	memset(&wr_opts, 0, sizeof(wr_opts));
	wr_opts.buffer	= (u_char*) dataBuff;
	wr_opts.offset	= testOffset+startAddr;
	wr_opts.length	= size;
	/* opts.forcejffs2 = 1; */
	wr_opts.pad	= 1;
	wr_opts.blockalign = 1;
	wr_opts.quiet      = 1;
	ret = nand_write_opts(nand, &wr_opts);
	printf("status 0x%X", ret);

	size = 0x40000;
	printf("\nRead Nand Flash... ");

for( startAddr = 0; startAddr+size <= testMaxSize; startAddr+=size)
{
	rd_opts.buffer	= (u_char*) readBuff;
	rd_opts.offset	= testOffset+startAddr;
	rd_opts.length	= size;
	rd_opts.readoob	= 0;
	rd_opts.quiet   = 1;
	ret = nand_read_opts(nand, &rd_opts);

	ret = 0;
	for(i = 0; i < size; i++)
	{
		if( dataBuff [i] != readBuff[i] )
		{
			printf("\nFailed at 0x%X, expected 0x%X, read 0x%X",
				testOffset+startAddr+i, dataBuff[i], readBuff[i]);
			ret++;
		}

//		printf("\Ok at 0x%X, expected 0x%X, read 0x%X",
//				testOffset+i, &((u_char*)dataBuff)[i], &((u_char*)readBuff)[i]);
		if( ret > 10 )
		{
			printf("\nFailed\n");
			break;
		}
	}
	printf("~");
}

	free((void*)dataBuff);
	printf("\nPassed !!\n");

	regVal = MV_REG_READ(0x10000);
	regVal &= ~0xFFFF;
	regVal |=  0x2222;
	MV_REG_WRITE(0xf1010000, regVal);

	if (mvPpChipIsXCat2() == MV_FALSE)
	{
		printf("For xCat board GE-Rev 1.0: set JP11 to [1,2] after the test.\n");
		printf("For xCat board FE-Rev 1.0: set JP13 to [1,2] after the test.\n");
	}

	return 1;
}

U_BOOT_CMD(
	test_nand,      1,     1,      do_nand_test,
	"test_nand	- test NAND flash interfaces.\n",
		"\n"
	"For xCat board GE-Rev 1.0: set JP11 to [2,3] before the test.\n"
	"For xCat board FE-Rev 1.0: set JP13 to [2,3] before the test.\n"
	"Thus NAND flash is made Selected by the Chip Select.\n"
	"For xCat board: return JP11/JP13 to [1,2] after the test is finished.\n"
);

#endif /* defined(MV_NAND_BOOT) */

#define CRYPT_SRAM_BASE		CRYPT_ENG_BASE
#define CRYPT_SRAM_SIZE		_2K
#define CRYPT_SRAM_LIMIT	CRYPT_SRAM_BASE + CRYPT_SRAM_SIZE

int do_sram_test(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned long size,sramAddr,dramAddr;
	cmd_tbl_t *tmpcmdtp;

	for (;;)
	{
		CHECK_CTRL_C()

		do
		{
			sramAddr = CRYPT_SRAM_BASE + (random() % CRYPT_SRAM_SIZE);
		} while(sramAddr >= CRYPT_SRAM_LIMIT);

		do
		{
			size   = random() % CRYPT_SRAM_SIZE;
		} while(((sramAddr + size) > CRYPT_SRAM_LIMIT) || (size==0));

		do
		{
			dramAddr   = DRAM_BASE + (random() % DRAM_SIZE);
		} while((dramAddr + size) > DRAM_LIMIT);

		MV_8 srcAddr[11],destAddr[11],strSize[11];
		sprintf((char*)srcAddr,"0x%x", (MV_U32)dramAddr);
		sprintf((char*)destAddr,"0x%x", (MV_U32)sramAddr);
		sprintf((char*)strSize,"0x%x", (MV_U32)size);

		printf("cp.b  0x%08X 0x%08X 0x%04X\n",
			   (MV_U32)dramAddr, (MV_U32)sramAddr, (MV_U32)size);
		{
		DB( printf("cp.b params: Src: %s\tDest: %s\tSize: %s\n", srcAddr, destAddr, strSize) );
		char* argv[] = {"cp.b", srcAddr, destAddr, strSize};
		FIND_CMD(tmpcmdtp,"cp");
		CHECK_CMD_STATUS( do_mem_cp (tmpcmdtp, 0, 4, argv) );
		}

		printf("cmp.b 0x%08X 0x%08X 0x%04X\n",
			   (MV_U32)dramAddr, (MV_U32)sramAddr, (MV_U32)size);
		{
		DB( printf("cmp.b params: Src: %s\tDest: %s\tSize: %s\n", srcAddr, destAddr, strSize) );
		char* argv[] = {"cmp.b", srcAddr, destAddr, strSize};
		FIND_CMD(tmpcmdtp,"cmp");
		CHECK_CMD_STATUS( do_mem_cmp (tmpcmdtp, 0, 4, argv) );
		}

		printf("\n");
	}

	return 1;
}

U_BOOT_CMD(
	test_sram,      1,     1,      do_sram_test,
	"test_sram	- test sram interfaces.\n",
		"\n"
);

int do_cpu_pp_conn_test(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	cmd_tbl_t *tmpcmdtp = NULL;
	char *gatewayIp = NULL, *serverIp = NULL, *imageName = NULL;
	MV_U32	vendirId, checkTftp;
	MV_U32 devNum = PP_DEV0;
	MV_U32 rand;
	MV_8 strSize[11];

	switch (argc) {
	case 1:
		checkTftp = 0;
		break;
	case 2:
		checkTftp = 0;
		devNum = (MV_U32)argv[1];
		break;
	case 3:
		checkTftp = 1;
		serverIp = argv[1];
		imageName = argv[2];
		printf("Setting server ip to %s\n", serverIp);
		setenv("serverip", serverIp);
		break;
	case 4:
		checkTftp = 1;
		devNum = (MV_U32)argv[1];
		serverIp = argv[2];
		imageName = argv[3];
		printf("Setting server ip to %s\n", serverIp);
		setenv("serverip", serverIp);
		break;
	default: printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	printf("Checking device %d\n", devNum);
	mvSwitchReadReg(devNum, 0x50, &vendirId);
	if(vendirId != MV_VENDOR_ID)
	{
		printf("Unrecognized vendor ID\n");
		goto error;
	}

	printf("dhcp\n");
	{
	char* argv[] = {"dhcp"};
	FIND_CMD(tmpcmdtp,"dhcp");
	CHECK_CMD_STATUS( do_dhcp (tmpcmdtp, 0, 1, argv) );
	}

	if(checkTftp)
	{
		printf("Setting serverip to %s\n", serverIp);
		setenv("serverip", serverIp);
	}

	for (;;)
	{
		CHECK_CTRL_C();

		rand = random() * checkTftp;

		if (rand % 3 == 0)
		{
			if ((gatewayIp = getenv("gatewayip")) == NULL) {
				printf("Unable tp read gatewayip environment variable\n");
				goto error;
			}

			printf("ping %s\n",gatewayIp);
			char* argv[] = {"ping",gatewayIp};
			FIND_CMD(tmpcmdtp,"ping");
			CHECK_CMD_STATUS( do_ping (tmpcmdtp, 0, 2, argv) );
		}
		else if (rand % 3 == 1)
		{
			printf("ping %s\n",serverIp);
			char* argv[] = {"ping",serverIp};
			FIND_CMD(tmpcmdtp,"ping");
			CHECK_CMD_STATUS( do_ping (tmpcmdtp, 0, 2, argv) );
		}
		else
		{
			{
			printf("tftp %s %s\n",TFTP_ADDR, imageName);
			char* argv[] = {"tftp",TFTP_ADDR, imageName};
			FIND_CMD(tmpcmdtp,"tftp");
			CHECK_CMD_STATUS( do_tftpb (tmpcmdtp, 0, 3, argv) );
			}

			{
			printf("tftp %s %s\n",TFTP_ADDR_CMP, imageName);
			char* argv[] = {"tftp",TFTP_ADDR_CMP, imageName};
			FIND_CMD(tmpcmdtp,"tftp");
			CHECK_CMD_STATUS( do_tftpb (tmpcmdtp, 0, 3, argv) );
			}


			sprintf((char*)strSize,"0x%X", (MV_U32)NetBootFileXferSize);

			printf("cmp.b %s %s 0x%04X\n",
				   TFTP_ADDR, TFTP_ADDR_CMP, (MV_U32)NetBootFileXferSize);
			{
			DB( printf("cmp.b params: Src: %s\tDest: %s\tSize: %s\n",
					   TFTP_ADDR, TFTP_ADDR_CMP, strSize) );
			char* argv[] = {"cmp.b", TFTP_ADDR, TFTP_ADDR_CMP, strSize};
			FIND_CMD(tmpcmdtp,"cmp");
			CHECK_CMD_STATUS( do_mem_cmp (tmpcmdtp, 0, 4, argv) );
			}
		}

		printf("\n");
	}

	return 1;
error:
	printf("Test Failed !!!\n");
	return 0;
}

U_BOOT_CMD(
	test_cpu_pp_conn,      4,     1,      do_cpu_pp_conn_test,
	"test_cpu_pp	- test CPU and PP connectivity.\n",
	"\n"
);

#define PHY_VENDOR_ID 0x0141

int do_smi_test(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	MV_U32 ethPortNum;
	MV_U16 vendorId = 0;

	switch (argc) {
	case 1:
		ethPortNum = 0;
		break;
	case 2:
		ethPortNum = simple_strtoul( argv[1], NULL, 10 );
		break;
	default: printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	printf("Checking for port %d\n", (int)ethPortNum);
	mvEthPhyRegRead(mvBoardPhyAddrGet(ethPortNum), 2, &vendorId);
	if(vendorId != PHY_VENDOR_ID)
	{
		printf("PHY Vendor ID: 0x%04X\n",(MV_U32)vendorId);
		printf("Expected PHY Vendor ID: 0x%04X\n", PHY_VENDOR_ID);
		goto error;
	}

	printf("Test Passed\n");
	return 1;
 error:
    printf("Test Failed !!!\n");
    return 0;
}

U_BOOT_CMD(
	test_smi,      2,     1,      do_smi_test,
	"test_smi	- test smi interfaces.\n",
		"\n"
);

int do_wait_for_interrupt ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned long old,temp;

	//mvCtrlPwrSaveOn();

	/* Disable int */
	__asm__ __volatile__("mrs %0, cpsr\n"
				 "orr %1, %0, #0xc0\n"
				 "msr cpsr_c, %1"
				 : "=r" (old), "=r" (temp)
				 :
				 : "memory");

	printf("Interrupt Disabled (CPSR 0x%x)\n", (MV_U32)temp);

	/* Wait for int */
	__asm__ __volatile__("mcr    p15, 0, r0, c7, c0, 4");

	printf("Interrupt Enabled (CPSR 0x%x)\n", (MV_U32)old);

	/* Enabled int */
	__asm__ __volatile__("msr cpsr_c, %0"
				 : "=r" (old)
				 :
				 : "memory");
	return 1;
}

U_BOOT_CMD(
	waitForInterrupt,     1,      1,       do_wait_for_interrupt,
	"waitForInterrupt - Sets the CPU in \"Wait for Interrupt\" mode (Power saving mode)\n",
	" \n"
	"	- Sets the CPU in \"Wait for Interrupt\" mode (Power saving mode)\n"
);

int do_endless_loop ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned long temp;

	/* Enable I Cache if disabled */
	asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (temp));
	if ((temp & BIT12) == 0)
	{
		printf("Enabling I Cache\n");
		temp |= BIT12;
		asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (temp));
	}

    /* Perfom endless loop. After the first time it will be loaded
     * into I Cache
         */

	printf("Starting endless loop\n");
	while(1)
	{
		if(tstc())
		{
			break;
		}
	}

	printf("Done.\n");
	return 0;
}

U_BOOT_CMD(
	endlessLoop,     1,      1,       do_endless_loop,
	"endlessLoop - Performs endles loop code from I Cache\n",
	" \n"
	"	- Performs endles loop code from I Cache\n"
);

/*******************************************************************************
 * ppFtdllWaXcat2 - shell command
 */
void mvPpFtdllWaXcat2(void);

int do_FtdllWaXcat2 (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    mvPpFtdllWaXcat2();
    return 0;
}

U_BOOT_CMD(
	ppFtdllWaXcat2, 1, 1, do_FtdllWaXcat2,
	"ppFtdllWaXcat2.\n",
	"ppFtdllWaXcat2.\n"
);

/*******************************************************************************
 * Configure xCat in sleed board to enable ping from DiscoSmp and xCat.
 */

MV_STATUS mvPpEeprom_sleed_board(void);

int do_mvPpEeprom_sleed_board(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    mvPpEeprom_DB_XCat2A1_48FE_4GP();
    return 0;
}

U_BOOT_CMD(
	ledinit, 1, 1, do_mvPpEeprom_sleed_board,
	"ppSleedBoardNetworkConfig.\n",
	"ppSleedBoardNetworkConfig.\n"
);

/*******************************************************************************
 * Get number of PP chips on the board - shell command
 */

MV_U8 mvSwitchGetDevicesNum(void);
MV_U8 mvSwitchGetDevicesNumDBG(void);

int do_mvPpGetDevicesNum (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    MV_U32 numOfPpDevs = mvSwitchGetDevicesNum();

    mvOsPrintf("The number of Prestera devices is %d.\n", numOfPpDevs);

    /* mvSwitchGetDevicesNumDBG(); */
    return 0;
}

U_BOOT_CMD(
	ppGetDevicesNum, 1, 1, do_mvPpGetDevicesNum,
	"ppGetDevicesNum.\n",
	"ppGetDevicesNum.\n"
);

/*******************************************************************************
 * ppReadMib - shell command
 */
void mvPresteraReadPortMibCounters(int port);

int do_mvPresteraReadPortMibCounters (cmd_tbl_t *cmdtp, int flag,
                                     int argc, char *argv[])
{
    MV_U32 port;

    switch (argc) {
    case 1:
            port = 0;
            break;
    case 2:
            port = simple_strtoul( argv[1], NULL, 10 );
            break;
    default: printf ("Usage:\n%s\n", cmdtp->usage);
            return -1;
    }

    mvPresteraReadPortMibCounters(port);

    return 0;
}

U_BOOT_CMD(
    ppReadMib, 2, 1, do_mvPresteraReadPortMibCounters,
    "ppReadMib - read MIB counter for specific Switch port.\n",
    "Read MIB counter for specific Switch port.\n"
);

/*******************************************************************************
 * ppReadMib - shell command
 */
void mvErrataPrint(void);

int do_mvErrataPrint(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    mvErrataPrint();
    return 0;
}

U_BOOT_CMD(
    socErrataPrint, 2, 1, do_mvErrataPrint,
    "socErrataPrint.\n",
    "socErrataPrint.\n"
);

/*******************************************************************************
 * ppWriteLogPrint - shell command
 */
void mvPpWriteLogPrint(void);

int do_ppWriteLogPrint (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    mvPpWriteLogPrint();
    return 0;
}

U_BOOT_CMD(
    ppWriteLogPrint, 1, 1, do_ppWriteLogPrint,
    "Read MIB counter for specific Switch port (1).\n",
    "Read MIB counter for specific Switch port (2).\n"
);

void mvDevWriteLogPrint(void);

int do_socWriteLogPrint (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    mvDevWriteLogPrint();
    return 0;
}

U_BOOT_CMD(
    socWriteLogPrint, 1, 1, do_socWriteLogPrint,
    "ppReadMib - read MIB counter for specific Switch port.\n",
    "Read MIB counter for specific Switch port.\n"
);

MV_STATUS setCpuAsVLANMember(int devNum, int vlanNum);

int do_ppSetCpuAsVLANMember (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    MV_U32 vlan;

    switch (argc) {
    case 1:
            vlan = 1; /* default VLAN */
            break;
    case 2:
            vlan = simple_strtoul( argv[1], NULL, 10 );
            break;
    default: printf ("Usage:\n%s\n", cmdtp->usage);
            return -1;
    }

    mvOsPrintf("Adding CPU Port of PP dev %d to VLAN %d.\n", PP_DEV0, vlan);
    if (setCpuAsVLANMember(PP_DEV0, vlan) != MV_OK)
    {
        mvOsPrintf("%s: setCpuAsVLANMember failed.\n", __func__);
        return MV_FAIL;
    }
    return 0;
}

U_BOOT_CMD(
	ppSetCpuAsVLANMember, 2, 1, do_ppSetCpuAsVLANMember,
	"ppSetCpuAsVLANMember - add CPU Port of dev 0 to some VLAN.\n",
	"Add CPU Port of dev 0 to some VLAN.\n"
);

void swPrintConfig(int dev);

int do_ppPrintConfig (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    swPrintConfig(0);
    return 0;
}

U_BOOT_CMD(
	ppPrintConfig, 1, 1, do_ppPrintConfig,
	"ppSetCpuAsVLANMember - add CPU Port of dev 0 to some VLAN.\n",
	"Add CPU Port of dev 0 to some VLAN.\n"
);

void swPrintSdmaCounters(int dev);

int do_ppPrintSdmaCounters (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    swPrintSdmaCounters(0);
    return 0;
}

U_BOOT_CMD(
	ppPrintSdmaCounters, 1, 1, do_ppPrintSdmaCounters,
	"ppSetCpuAsVLANMember - add CPU Port of dev 0 to some VLAN.\n",
	"Add CPU Port of dev 0 to some VLAN.\n"
);

void swPrintCounters(int dev);

int do_ppPrintCounters (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    swPrintCounters(0);
    return 0;
}

U_BOOT_CMD(
	ppPrintCounters, 1, 1, do_ppPrintCounters,
	"ppSetCpuAsVLANMember - add CPU Port of dev 0 to some VLAN.\n",
	"Add CPU Port of dev 0 to some VLAN.\n"
);

void mvPpEnableSwitchWriteReg(void);

int do_ppEnableSwitchWriteReg (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    mvPpEnableSwitchWriteReg();
    return 0;
}

U_BOOT_CMD(
	ppEnableSwitchWriteReg, 1, 1, do_ppEnableSwitchWriteReg,
	"ppSetCpuAsVLANMember - add CPU Port of dev 0 to some VLAN.\n",
	"Add CPU Port of dev 0 to some VLAN.\n"
);

void mvPpDisableSwitchWriteReg(void);

int do_ppDisableSwitchWriteReg (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    mvPpDisableSwitchWriteReg();
    return 0;
}

U_BOOT_CMD(
	ppDisableSwitchWriteReg, 1, 1, do_ppDisableSwitchWriteReg,
	"ppSetCpuAsVLANMember - add CPU Port of dev 0 to some VLAN.\n",
	"Add CPU Port of dev 0 to some VLAN.\n"
);

void mvChipFeaturesPrint(void);

int do_socChipFeaturesPrint (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    mvChipFeaturesPrint();
    return 0;
}

U_BOOT_CMD(
    socChipFeaturesPrint, 1, 1, do_socChipFeaturesPrint,
    "",
    ""
);

void mvGenUtRunAll(void);

int do_mvGenUtRunAll (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    mvGenUtRunAll();
    return 0;
}

U_BOOT_CMD(
    mvGenUtRunAll, 1, 1, do_mvGenUtRunAll,
    "",
    ""
);

void mvCpuMaxCurrentTest(void);

int do_mvCpuMaxCurrentTest (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    mvCpuMaxCurrentTest();
    return 0;
}

U_BOOT_CMD(
	test_cpuMaxCurrent, 1, 1, do_mvCpuMaxCurrentTest,
	"Run code that consumes maximal current by CPU.\n",
	".\n"
);

int mvSwitchInit(int, int);

int do_ppSwitchDrvInit (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    mvSwitchInit(0, 0);
    return 0;
}

U_BOOT_CMD(
	ppSwitchDrvInit, 1, 1, do_ppSwitchDrvInit,
	"Init switch driver (without enable).\n",
	".\n"
);

void mvPpValidMacEntryPrint(void);

int do_mvPpValidMacEntryPrint (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    mvPpValidMacEntryPrint();
    return 0;
}

U_BOOT_CMD(
	ppValidMacEntryPrint, 1, 1, do_mvPpValidMacEntryPrint,
	"Print all valid mac entries.\n",
	".\n"
);


int input_num(char *input, int force_hex, int size, MV_U8 *value)
{
    MV_U32 val = 0x00ULL;
    int num = 10, out = 0;
    char *name = input;

    if(*(name+1) == 'x')
    {
        num = 16;
		name += 2;  
    }else if(1 == force_hex)
        num = 16;
    else
        num = 10;

    for(;*name != '\0';name++)
	{
	    if(1 == out)
	        break;
	        
		switch(*name)
		{
			case '0'...'9':
				val = val*num + (*name - '0');
				break;
				
			case 'a'...'f':
			    if(10 == num)
			    {   
			        out = 1; 
                    break;
			    }
			        
				val = val*num + (*name - 'a') + 10ULL;
				break;
				
			case 'A'...'F':	
			    if(10 == num)		    
			    {   
			        out = 1; 
                    break;
			    }
			        
				val = val*num + (*name - 'A') + 10ULL;
				break;
				
			default:
			    out = 1;
			    break;
		}
	}    

    DEBUG("[%s:%d] val:0x%08x%08x\n", __FUNCTION__, __LINE__, (uint32)(val >> 32), (uint32)val);
    	
	if(1 == size)
	    * ((MV_U8 *)value) = (MV_U8) val;
	else if(2 == size)
	    * ((MV_U16 *)value) = (MV_U16) val;
	else if(4 == size)
	    * ((MV_U32 *)value) = (MV_U32) val;
	else
	{
	    printf("Unknown inout size, pls check\n");
	    return -1;
	}    
	    
    return 0;
}

#define REG_MV_GPP_DATA_OUT			0x10100
#define REG_MV_GPP_DATA_OUT_EN		0x10104
#define REG_MV_GPP_BLINK_EN			0x10108
#define REG_MV_GPP_DATA_IN_POL    	0x1010C
#define REG_MV_GPP_DATA_IN   		0x10110
#define REG_MV_GPP_INT_CAUSE		0x10114
#define REG_MV_GPP_INT_MASK			0x10118
#define REG_MV_GPP_INT_LVL			0x1011c

#define REG_MVH_GPP_DATA_OUT	    0x10140
#define REG_MVH_GPP_DATA_OUT_EN		0x10144
#define REG_MVH_GPP_BLINK_EN	    0x10148
#define REG_MVH_GPP_DATA_IN_POL    	0x1014C
#define REG_MVH_GPP_DATA_IN   		0x10150
#define REG_MVH_GPP_INT_CAUSE		0x10154
#define REG_MVH_GPP_INT_MASK	    0x10158
#define REG_MVH_GPP_INT_LVL			0x1015c

int do_gpio_ctrl (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    MV_U32 addr, val;
    
    if((argc != 3) && (argc != 4)) 
    {    
        mvOsPrintf("%s: gpio read address/gpio set address val\n", __func__);
        return -1;
    } else if ((argc == 3) && (*argv[1] != 'r'))  
    {    
        mvOsPrintf("%s: gpio read address\n", __func__);
        return -1;
    } else if ((argc == 4) && (*argv[1] != 'w')) 
    {    
        mvOsPrintf("%s: gpio set address val\n", __func__);
        return -1;
    }  
    
    if(*argv[1] == 'r')
    {
        if(*argv[2] == 'a')
        {
            mvOsPrintf("[%s:%d] REG_MV_GPP_DATA_OUT: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, REG_MV_GPP_DATA_OUT, MV_REG_READ(GPP_DATA_OUT_REG(0)));
            mvOsPrintf("[%s:%d] REG_MV_GPP_DATA_OUT_EN: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, REG_MV_GPP_DATA_OUT_EN, MV_REG_READ(GPP_DATA_OUT_EN_REG(0)));
            mvOsPrintf("[%s:%d] REG_MV_GPP_BLINK_EN: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, REG_MV_GPP_BLINK_EN, MV_REG_READ(GPP_BLINK_EN_REG(0)));
            mvOsPrintf("[%s:%d] REG_MV_GPP_DATA_IN_POL: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, REG_MV_GPP_DATA_IN_POL, MV_REG_READ(GPP_DATA_IN_POL_REG(0)));
            mvOsPrintf("[%s:%d] REG_MV_GPP_DATA_IN: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, REG_MV_GPP_DATA_IN, MV_REG_READ(GPP_DATA_IN_REG(0)));
            mvOsPrintf("[%s:%d] REG_MV_GPP_INT_CAUSE: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, REG_MV_GPP_INT_CAUSE, MV_REG_READ(GPP_INT_CAUSE_REG(0)));
            mvOsPrintf("[%s:%d] REG_MV_GPP_INT_MASK: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, REG_MV_GPP_INT_MASK, MV_REG_READ(GPP_INT_MASK_REG(0)));
            mvOsPrintf("[%s:%d] REG_MV_GPP_INT_LVL: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, REG_MV_GPP_INT_LVL, MV_REG_READ(GPP_INT_LVL_REG(0)));

            mvOsPrintf("[%s:%d] High REG_MVH_GPP_DATA_OUT: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, REG_MVH_GPP_DATA_OUT, MV_REG_READ(GPP_DATA_OUT_REG(1)));
            mvOsPrintf("[%s:%d] High REG_MVH_GPP_DATA_OUT_EN: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, REG_MVH_GPP_DATA_OUT_EN, MV_REG_READ(GPP_DATA_OUT_EN_REG(1)));
            mvOsPrintf("[%s:%d] High REG_MVH_GPP_BLINK_EN: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, REG_MVH_GPP_BLINK_EN, MV_REG_READ(GPP_BLINK_EN_REG(1)));
            mvOsPrintf("[%s:%d] High REG_MVH_GPP_DATA_IN_POL: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, REG_MVH_GPP_DATA_IN_POL, MV_REG_READ(GPP_DATA_IN_POL_REG(1)));
            mvOsPrintf("[%s:%d] High REG_MVH_GPP_DATA_IN: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, REG_MVH_GPP_DATA_IN, MV_REG_READ(GPP_DATA_IN_REG(1)));
            mvOsPrintf("[%s:%d] High REG_MVH_GPP_INT_CAUSE: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, REG_MVH_GPP_INT_CAUSE, MV_REG_READ(GPP_INT_CAUSE_REG(1)));
            mvOsPrintf("[%s:%d] High REG_MVH_GPP_INT_MASK: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, REG_MVH_GPP_INT_MASK, MV_REG_READ(GPP_INT_MASK_REG(1)));
            mvOsPrintf("[%s:%d] High REG_MVH_GPP_INT_LVL: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, REG_MVH_GPP_INT_LVL, MV_REG_READ(GPP_INT_LVL_REG(1)));
        }else
        {
            input_num(argv[2], 1, 4, &addr);    
            mvOsPrintf("[%s:%d] read register: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, addr, MV_REG_READ(addr));
        }        
    }    
    else     
    {    
        input_num(argv[2], 1, 4, &addr);   
        input_num(argv[3], 1, 4, &val);  
        MV_REG_WRITE(addr, val);  
        mvOsPrintf("[%s:%d] write register: 0x%08x data: 0x%08x\n", __FUNCTION__, __LINE__, addr, val);
    }
        
    return 0;
}

U_BOOT_CMD(
	gpio, 4, 1, do_gpio_ctrl,
	"set/get all gpio control register entries.\n",
	".\n"
);

