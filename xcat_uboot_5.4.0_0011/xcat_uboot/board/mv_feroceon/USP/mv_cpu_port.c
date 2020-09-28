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
#include <net.h>
#include <malloc.h>

#if defined (MV_INCLUDE_GIG_ETH)
#include "sys/mvSysGbe.h"
#include "mvOs.h"
#include "mvSysHwConfig.h"
#include "eth/mvEth.h"
#include "eth/gbe/mvEthGbe.h"
#include "eth-phy/mvEthPhy.h"
#include "ethswitch/mvSwitch.h"
#include "mvBoardEnvLib.h"

#include "pss/pssBspApis.h"
#include "prestera/common/mvSysConf.h"

/*
#define MV_DEBUG
*/
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

#define PKT_SIZE_CHANGE_EN

/****************************************************** 
 * driver internal definitions --                     *
 ******************************************************/ 
/* use only tx-queue0 and rx-queue0 */
#define EGIGA_DEF_TXQ 0
#define EGIGA_DEF_RXQ 0

/* rx buffer size */ 
#define ETH_HLEN       14
#define WRAP           (2 + ETH_HLEN + 4 + 32)  /* 2(HW hdr) 14(MAC hdr) 4(CRC) 32(extra for cache prefetch)*/
#define MTU            1500
#define RX_BUFFER_SIZE (MTU + WRAP)

/* rings length */
#define EGIGA_TXQ_LEN   20
#define EGIGA_RXQ_LEN   20

#ifdef UBOOT_TEST
#define RX_BUFFER_DEFAULT_SIZE  0x608
#endif


typedef struct _egigaPriv
{
	int port;
	MV_VOID *halPriv;
	MV_U32 rxqCount;
	MV_U32 txqCount;
	MV_BOOL devInit;
} egigaPriv; 

/* export eth drvr handle */
extern void*           glbMiiPortHandle;

MV_U8 rxBuffPool[RX_BUFFER_DEFAULT_SIZE*20];

/****************************************************** 
 * functions prototype --                             *
 ******************************************************/
static int mvEgigaCpuPortLoad( int port, char *name, char *enet_addr );

static int mvEgigaCpuPortInit( struct eth_device *dev, bd_t *p );
static int mvEgigaCpuPortHalt( struct eth_device *dev );
static int mvEgigaCpuPortTx( struct eth_device *dev, volatile MV_VOID *packet, int len );
static int mvEgigaCpuPortRx( struct eth_device *dev );

static MV_PKT_INFO* mvEgigaRxFill(MV_VOID);



/*********************************************************** 
 * mv_eth_initialize --                                    *
 *   main driver initialization. loading the interfaces.   *
 ***********************************************************/
int mv_egiga_cpu_port_initialize( bd_t *bis ) 
{

	MV_8 *enet_addr;
	MV_8 enetvar[9];

	MV_U32 reqBufNum = 20;
	MV_U32 qPercentage[] = {100,0,0,0,0,0,0,0};

	DB( printf("%s: Start\n",__FUNCTION__) );

	enet_addr = getenv( "eth1addr" );
	
	if(!enet_addr || strlen(enet_addr)!=17) /*MAC address len (include ':' is 17 chars)*/
	{
		printf( "\nError: (Egiga1) MAC Address is not set properly(eth1addr)\n");
		return -1;
	}

	/******		Switch required initializations 	******/
	if(setCpuAsVLANMember(PRESTERA_DEF_DEV, PRESTERA_DEF_VLAN/*VLAN Num*/)!=MV_OK)
	{
		printf( "\nError: (Egiga1) Unable to set CPU as VLAN member (for device %d)\n",PRESTERA_DEF_DEV);
		return -1;
	}

	if(setCPUAddressInMACTAble(PRESTERA_DEF_DEV/* devNum */,(MV_U8*)enet_addr/* macAddr */,
				   PRESTERA_DEF_VLAN/* vid */)!=MV_OK)
	{
		printf( "\nError: (Egiga1) Unable to teach CPU MAC address\n");
		return -1;
	}

	/* Config switch to to CPU Port mode
 	 * This is only switch configuration
 	 * CPU configuration should be done separately */
	mvSwitchCpuPortConfig(PRESTERA_DEF_DEV /*devNum*/);

	/******		Controller required initializations 	******/
	bspEthInit(EGIGA_CPU_PORT);

	if (bspEthPortRxInit(RX_BUFFER_DEFAULT_SIZE*reqBufNum,
			 rxBuffPool,
			 RX_BUFFER_DEFAULT_SIZE,
			 reqBufNum,
			 0/*headerOffset*/,
			 1/*rxQNum*/,
			 &qPercentage)!=MV_OK)
	{
		printf("bspEthPortRxInit failed\n");
		return 1;
	}

	if (bspEthPortTxInit(20)!=MV_OK)
	{
		printf("bspEthPortTxInit failed\n");
		return 1;
	}

	if (bspEthPortTxModeSet(bspEthTxMode_synch_E)!=MV_OK)
	{
		printf("bspEthPortTxModeSet failed\n");
		return 1;
	}

	if (bspEthPortEnable()!=MV_OK)
	{
		printf("bspEthPortEnable failed\n");
		return 1;
	}

	sprintf( enetvar, EGIGA_CPU_PORT ? "eth%daddr" : "ethaddr", EGIGA_CPU_PORT );
	enet_addr = getenv( enetvar );

	/******		U-Boot interface initialization 	******/
	if (mvEgigaCpuPortLoad( EGIGA_CPU_PORT, "cpu_port", enet_addr )!=0)
	{
		printf("mvEgigaCpuPortLoad failed\n");
		return 1;
	}

	return 0;
}


/*********************************************************** 
 * mvEgigaCpuPortLoad --                                          *
 *   load a network interface into uboot network core.     *
 *   initialize sw structures e.g. private, rings, etc.    *
 ***********************************************************/
static int mvEgigaCpuPortLoad( int port, char *name, char *enet_addr ) 
{
	struct eth_device *dev = NULL;
	egigaPriv *priv = NULL;
	ETH_PORT_CTRL dummy_port_handle;

	DB( printf( "%s: %s load - ", __FUNCTION__, name ) );

	dev = malloc( sizeof(struct eth_device) );
	priv = malloc( sizeof(egigaPriv) );

	if( !dev ) {
		DB( printf( "%s: %s falied to alloc eth_device (error)\n", __FUNCTION__, name ) );
		goto error;
	}

	if( !priv ) {
		DB( printf( "%s: %s falied to alloc egiga_priv (error)\n", __FUNCTION__, name ) );
		goto error;
	}

	memset( priv, 0, sizeof(egigaPriv) );

	/* init device methods */
	memcpy( dev->name, name, NAMESIZE );
	mvMacStrToHex(enet_addr, (MV_8 *)(dev->enetaddr));

	/* set MAC addres even if port was not used yet. */
	dummy_port_handle.portNo = port;
	mvEthMacAddrSet( &dummy_port_handle, dev->enetaddr, EGIGA_DEF_RXQ);

	dev->init = (void *)mvEgigaCpuPortInit;
	dev->halt = (void *)mvEgigaCpuPortHalt;
	dev->send = (void *)mvEgigaCpuPortTx;
	dev->recv = (void *)mvEgigaCpuPortRx;
	dev->priv = priv;
	dev->iobase = 0;
	priv->port = port;

	/* register the interface */
	eth_register(dev);


	DB( printf( "%s: %s load ok\n", __FUNCTION__, name ) );
	return 0;

	error:
	printf( "%s: %s load failed\n", __FUNCTION__, name );
	if( priv ) free( dev->priv );
	if( dev ) free( dev );
	return -1;
}


unsigned int egiga_cpu_port_init=0;

static int mvEgigaCpuPortInit( struct eth_device *dev, bd_t *p )
{
	egigaPriv *priv = dev->priv;

	/*priv->halPriv = glbMiiPortHandle;*/

	DB( printf( "%s: %s init - ", __FUNCTION__, dev->name ) );

	/* egiga not ready */
	DB( printf ("mvBoardPhyAddrGet()=0x%x , priv->port =0x%x\n",mvBoardPhyAddrGet(priv->port),priv->port) );

	/* If speed is not auto then link is force */
	if (BOARD_MAC_SPEED_AUTO == mvBoardMacSpeedGet(priv->port))
	{
		/* Check Link status on phy */
		if( mvEthPhyCheckLink( mvBoardPhyAddrGet(priv->port) ) == MV_FALSE ) {
			printf( "%s no link\n", dev->name );
			return 0;
		}
		else DB( printf( "link up\n" ) );
	}

	egiga_cpu_port_init = 1;

	/* set new addr in hw */
/*
	if( mvEthMacAddrSet( priv->halPriv, dev->enetaddr, EGIGA_DEF_RXQ) != MV_OK ) {
		printf("%s: ethSetMacAddr failed\n", dev->name );
		goto error;
	}
*/
	priv->devInit = MV_TRUE;

#ifdef MV_DEBUG
	ethRegs(priv->port);
	ethPortRegs(priv->port);
	ethPortStatus(priv->port);

	ethPortQueues(priv->port, EGIGA_DEF_RXQ, -1/*EGIGA_DEF_TXQ*/, 0);
#endif

	DB( printf( "%s: %s complete ok\n", __FUNCTION__, dev->name ) );
	return 1;

	error:
	if( priv->devInit )
		mvEgigaCpuPortHalt( dev );
	printf( "%s: %s failed\n", __FUNCTION__, dev->name );
	return 0;
}

static int mvEgigaCpuPortHalt( struct eth_device *dev )
{

	egigaPriv *priv = dev->priv;

	DB( printf( "%s: %s halt - ", __FUNCTION__, dev->name ) );
	if( priv->devInit == MV_TRUE ) {
		priv->devInit = MV_FALSE;
	}

	/* Clear Cause registers (must come before mvEthPortFinish) */
    	MV_REG_WRITE(ETH_INTR_CAUSE_REG(priv->port),0);
    	MV_REG_WRITE(ETH_INTR_CAUSE_EXT_REG(priv->port),0);

	egiga_cpu_port_init = 0;

	DB( printf( "%s: %s complete\n", __FUNCTION__, dev->name ) );
	return 0;
}

static int mvEgigaCpuPortTx( struct eth_device *dev, volatile void *buf, int len )
{
	egigaPriv *priv = dev->priv;
	MV_STATUS status;

	DB( printf( "mvEgigaCpuPortTx start\n" ) );
	/* if no link exist */
	if(!egiga_cpu_port_init) return 0;

#if (defined(MV_PRESTERA_SWITCH) && !defined(PRESTERA_NO_DSA_TAG))
	MV_U8*      segmentList[3];
	MV_U32      segmentLen[3];
	MV_U8	    dsaBuf[8];

	/* Only switch port 0 is used since we don't scan on which
	 * switch port we have link */
	/* TODO - set DSA tag for all ports */
	mvSwitchGetDsaTag(0/*devNum*/,0/*In-Band port*/,dsaBuf);

	/* the packet is devided into 3 fragments in order to add
 	 * the DSA tag without the need of copying the packet to
 	 * a new buffer */ 
	segmentList[0] = buf;
	segmentLen[0] = MAC_SA_AND_DA_SIZE;
	segmentList[1] = dsaBuf;
	segmentLen[1] = EXTEND_DSA_TAG_SIZE;
	segmentList[2] = buf + MAC_SA_AND_DA_SIZE;		
	segmentLen[2] = len - MAC_SA_AND_DA_SIZE;

	#ifdef PKT_SIZE_CHANGE_EN
	/* for testing different packet sizes */
	char* env = getenv("pkt_size");
	if(env)
	{
		int temp = simple_strtoul(env, NULL, 10);
		int old_size = segmentLen[0] + segmentLen[1] + segmentLen[2];
		temp = temp - MAC_SA_AND_DA_SIZE - EXTEND_DSA_TAG_SIZE;
		if(temp > segmentLen[2])
		{
			printf("Total Size: %d (0x%x) old\n",old_size,old_size);
			segmentLen[2] = temp;
			printf("segmentLen[2]: %d\n",segmentLen[2]);
			temp =  segmentLen[0] + segmentLen[1] + segmentLen[2];
			printf("Total Size: %d (0x%x) new\n",temp,temp);
		}
	}
	#endif /* ifdef PKT_SIZE_CHANGE_EN*/
	
	status = bspEthPortTx(segmentList,segmentLen,3/*numOfSegments*/, 0 /* txQ */);
#else
	status = bspEthPortTx(&buf,&len,1/*numOfSegments*/, 0 /* txQ */);
    DB(printf("%s: bspEthPortTx returned %d.\n", __func__, status));
#endif /* if (defined(MV_PRESTERA_SWITCH) && !defined(PRESTERA_NO_DSA_TAG)) */
	

	if( status != MV_OK ) {
		if( status == MV_NO_RESOURCE )
			DB( printf( "ring is full (error)\n" ) );
		else if( status == MV_ERROR )
			printf( "%s: error: bspEthPortTx returned %d.\n", __func__, status );
		else
			printf( "unrecognize status (error) ethPortSend\n" );
		goto error;
	} 
	else DB( printf( "ok\n" ) );

	priv->txqCount++;

	DB( printf( "%s: %s complete ok\n", __FUNCTION__, dev->name ) );
	return 0;

	error:
	printf( "%s: %s failed\n", __FUNCTION__, dev->name );
	return 1;
}


static int mvEgigaCpuPortRx( struct eth_device *dev )
{
	egigaPriv*  priv = dev->priv;
    	MV_PKT_INFO *pktInfo;
	MV_STATUS   status;

	DB( printf( "%s start\n",__FUNCTION__ ) );
	/* if no link exist */
	if(!egiga_cpu_port_init) return 0;

	while( 1 ) {

		/* get rx packet from hal */
		pktInfo = bspEthPortRx(EGIGA_DEF_RXQ);

		if( pktInfo != NULL ) {
			DB( printf( "good rx\n" ) );
			priv->rxqCount--;

			/* check rx error status */
			if( pktInfo->status & (ETH_ERROR_SUMMARY_MASK) ) {
				MV_U32 err = pktInfo->status & ETH_RX_ERROR_CODE_MASK;
				/*DB( printf( "bad rx status %08x, ", (MV_U32)pktInfo->cmdSts ) );*/
				if( err == ETH_RX_RESOURCE_ERROR )
					DB( printf( "(resource error)" ) );
				else if( err == ETH_RX_MAX_FRAME_LEN_ERROR )
					DB( printf( "(max frame length error)" ) );
				else if( err == ETH_RX_OVERRUN_ERROR )
					DB( printf( "(overrun error)" ) );
				else if( err == ETH_RX_CRC_ERROR )
					DB( printf( "(crc error)" ) );
				else {
					DB( printf( "(unknown error)" ) );
					goto error;
				}
				DB( printf( "\n" ) );
			}
			else {

				DB( printf( "%s: %s calling NetRecieve ", __FUNCTION__, dev->name) );
				DB( printf( "%s: calling NetRecieve pkInfo = 0x%x\n", __FUNCTION__, pktInfo) );
				DB( printf( "%s: calling NetRecieve osInfo = 0x%x\n", __FUNCTION__, pktInfo->osInfo) );
				DB( printf( "%s: calling NetRecieve pktSize = 0x%x\n", __FUNCTION__, pktInfo->pFrags->dataSize) );

#if (defined(MV_PRESTERA_SWITCH) && !defined(PRESTERA_NO_DSA_TAG))
				if (priv->port==1)
				{
					/* remove DSA tag*/
					memcpy(&pktInfo->pFrags->bufVirtPtr[14],
					       &pktInfo->pFrags->bufVirtPtr[6], EXTEND_DSA_TAG_SIZE);
					memcpy(&pktInfo->pFrags->bufVirtPtr[EXTEND_DSA_TAG_SIZE],
					       &pktInfo->pFrags->bufVirtPtr[0], 6);
					pktInfo->pFrags->dataSize;
					/* good rx - push the packet up (skip on two first empty bytes) 
					 * and gap that was caused by removing DSA TAG */
					NetReceive( ((MV_U8 *)pktInfo->osInfo) + 2 + EXTEND_DSA_TAG_SIZE, 
						    (int)pktInfo->pFrags->dataSize - EXTEND_DSA_TAG_SIZE);
				}
				else
#endif /* if (defined(MV_PRESTERA_SWITCH) && !defined(PRESTERA_NO_DSA_TAG)) */

				/* good rx - push the packet up (skip on two first empty bytes) */
				NetReceive( ((MV_U8 *)pktInfo->osInfo) + 2, (int)pktInfo->pFrags->dataSize);
			}

			DB( printf( "%s: %s refill rx buffer - ", __FUNCTION__, dev->name) );

			/* give the buffer back to hal (re-init the buffer address) */
			status = bspEthRxPacketFreeNonSc(&pktInfo->pFrags->bufVirtPtr,
			                                 1, EGIGA_DEF_RXQ);
	
			if( status == MV_OK ) {
				priv->rxqCount++;
			}
			else {
				printf( "error\n" );
				goto error;
			}

		} else {
			/* no more rx packets ready */
			DB( printf( "no more work\n" ) );
			break;
		}
	}

	DB( printf( "%s: %s complete ok\n", __FUNCTION__, dev->name ) );
	return 0;

	error:
	DB( printf( "%s: %s failed\n", __FUNCTION__, dev->name ) );
	return 1;
}

int do_eth_counter_show ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	MV_U8	port;

	if (argc != 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 0;
	}

	/* Get port number*/
	port = simple_strtoul(argv[1], NULL, 10);
	if(port >= MV_ETH_MAX_PORTS)
	{
		printf("Invalid port number: %d\n",port);
	}
	
	ethPortCounters(port);
	
	return 1;
}

U_BOOT_CMD(
	ethCounterShow,     2,      1,       do_eth_counter_show,
	"ethCounterShow - Displays eth port MIB counterLoads\n",
	"port_number\n" 
	"	- Displays eth port MIB counterLoads\n"
);


#endif /* #if defined (MV_INCLUDE_GIG_ETH) */
