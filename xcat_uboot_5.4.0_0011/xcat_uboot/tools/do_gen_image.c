#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

#define _HOST_COMPILER
#include "bootstrap_def.h"

#ifndef O_BINARY                /* should be define'd on __WIN32__ */
#define O_BINARY        0
#endif

typedef enum 
{
	REG_FILE_OK,
	REG_FILE_END,
	REG_FILE_FORMAT_ERROR
}REG_FILE_STATUS;


#define MAX_DRAM_REGS_NUM	52
#define NAND_ALIGNMENT		256

//#define DEBUG

#if defined(DEBUG)
#define DB(x) x
#else
#define DB(x)
#endif

#define TEMP_IMAGE_NAME "temp-gen.bin"

char gen_dest_addr[11];
char gen_exec_addr[11];

char nand_page_size[8];
char nand_ecc_mode[4];

REG_FILE_STATUS read_board_reg_list(FILE *f_dram, RegFileHdr_t *regHdr, unsigned int *dram_reg);
int update_out_file(int f_out, char* buf_in, int buf_size);
int get_file_size(int f_out);
int open_file_rw(char* fname_out,int is_out_file_exist,int* f_out);
int open_file_read(char* fname_in, int* f_in);
int map_file(int f_in, char** buf_in);

int set_generic_boot_image(char *buf_in, int buf_size, BHR_t* hdr, ImageInfo_t* imageInfo,
			   char *fname_image, char *fname_dram, char* fname_out)
{
	RegFileHdr_t regHdr, tmpHeader;
	Info_t info;
	unsigned int dram_reg[(512>>2)];
	int f_out, f_image;
	char *buf_image = NULL;
	FILE*   f_dram;
	REG_FILE_STATUS reg_status = REG_FILE_OK;
	int bytesToAlign=0,i;
	char tmpAlign[3];
	int new_argc, chsum32;

	/* set boot device */
	BOOT_DEV boot_dev = (BOOT_DEV)hdr->blockID;
	
	int is_out_file_exist = (int)hdr->rsvd3;
	
	/* Align size to 4 byte*/
	if (buf_size & 0x3)
	{
		bytesToAlign = (4 - (buf_size & 0x3));
		DB(printf("buf_size = 0x%x alignment of %d bytes\n", buf_size, bytesToAlign));
		memset(tmpAlign, 0, 3);
	}

	/* update bin file with images info */
	memset(&info, 0, sizeof(Info_t));
	//info.regFileInfo.regFileOffset = buf_size + bytesToAlign + sizeof(Info_t) + sizeof(BHR_t);
	info.regFileInfo.regFileOffset = buf_size + bytesToAlign;

	/* map main image file */
	open_file_read(fname_image, &f_image);
	if(map_file(f_image, &buf_image) == -1)
	{
		fprintf(stderr,"unable to map file '%s'\n",fname_image);
		exit(1);
	}

	info.imageInfo.imageOffset = 0xBAD0ADD0; /* Will be calculated afterwards */
	info.imageInfo.imageSize   = get_file_size(f_image) + 4/* include checksum*/; 
	info.imageInfo.imageDest   = imageInfo->imageDest;
	info.imageInfo.imageExec   = imageInfo->imageExec;

	DB(printf("Info: regFileOffset: Dec: %d Hex: 0x%X\n",info.regFileInfo.regFileOffset,info.regFileInfo.regFileOffset));
	DB(printf("Info: imageOffset: 0x%X\n",info.imageInfo.imageOffset));

	if(open_file_rw(TEMP_IMAGE_NAME,is_out_file_exist,&f_out) == -1)
	{
		exit(1);
	}

	if (update_out_file(f_out, (char*)&info, sizeof(Info_t))!=0) return -1;

	/* update bin file with generic image */
	if (update_out_file(f_out, buf_in, buf_size)!=0) return -1;

	/* update bin file with alignment */
	if (update_out_file(f_out, tmpAlign, bytesToAlign)!=0) return -1;
	
	if (fname_dram) 
	{
		f_dram = fopen(fname_dram, "r");
		
		if (f_dram == NULL)
		{
			fprintf(stderr,"File '%s' not found \n",fname_dram);
			exit(1);
		}

		do
		{
			memset(&tmpHeader, 0 ,sizeof(RegFileHdr_t));
			reg_status = read_board_reg_list(f_dram, &tmpHeader, dram_reg);
			if(reg_status==REG_FILE_END)
			{
				unsigned int endStrap = 0xFFFFFFFF;
				if (update_out_file(f_out, (char*)&endStrap, sizeof(unsigned int))!=0) return -1;

				DB(printf("EOF\n"));
				break;
			}
			else if(reg_status!=REG_FILE_OK)
			{
				return -1;
			}

			memcpy(&regHdr, &tmpHeader, sizeof(RegFileHdr_t));
			
			/* set checksum */
			regHdr.checkSum = checksum8((u32)&tmpHeader, sizeof(RegFileHdr_t) ,0);

			/* update bin file with board's header */
			if (update_out_file(f_out, (char*)&regHdr, sizeof(RegFileHdr_t))!=0) return -1;

			/* update bin file with board's register list */
			if (update_out_file(f_out, (char*)dram_reg, (regHdr.dramRegsNum)*8)!=0) return -1;
		} while(reg_status == REG_FILE_OK);

		close_file(f_dram);
		close_file(f_out);

		/* now we update info header in bin file with image offset */
		/* for this we need to re-open the file */
		f_out = open(TEMP_IMAGE_NAME,O_RDWR|O_BINARY);
		if (f_out == -1)
		{
			fprintf(stderr,"File '%s' not found \n",fname_out);
			exit(1);
		}

		bytesToAlign = 0;

		/* set image offset */
		info.imageInfo.imageOffset = get_file_size(f_out) + 
					     sizeof(struct BHR_t) + 4 /*4 bytes of checksum*/;
		if(boot_dev==BOOT_NAND) /* TODO - WA for NAND because we add extended header */
		{
			info.imageInfo.imageOffset += (512-sizeof(struct BHR_t));
		
			/* align image to 256 bytes */
			if (info.imageInfo.imageOffset & (NAND_ALIGNMENT-1))
			{
				DB(printf("Nand image not alligned to 256 bytes: %d\n",info.imageInfo.imageOffset));
				bytesToAlign = (NAND_ALIGNMENT - (info.imageInfo.imageOffset & (NAND_ALIGNMENT-1)));
				DB(printf("buf_size = %d alignment of %d bytes\n", info.imageInfo.imageOffset, bytesToAlign));
				memset(tmpAlign, 0, 3);
				info.imageInfo.imageOffset += bytesToAlign;
			}
		}

		DB(printf("image offset is: %d\n",info.imageInfo.imageOffset));

		if (update_out_file(f_out, (char*)&info, sizeof(Info_t))!=0) return -1;

		close_file(f_out);

		/* call main so it will add the header to the image that contains 
  		   the generic image & register file .
		   Before that, we change the 'type' to SPI ot NAND and update argv/argc*/
		new_argc = reset_argv(hdr);
		DB(printf("new_argc: %d\n",new_argc));

		char** argv = (char**)hdr->rsvd1;

		#if defined(DEBUG)
		for(i=0; i<new_argc; i++)
		{
			printf("%s\n",argv[i]);
		}
		#endif
		main(new_argc, argv);

		f_out = open(fname_out,O_RDWR|O_BINARY|O_APPEND);
		/* add alignment to bin file */
		while(bytesToAlign > 0)
		{
			DB(printf("%d more bytes to align\n",bytesToAlign));
			
			if (update_out_file(f_out, tmpAlign, (bytesToAlign > 4) ? 4 : bytesToAlign)!=0) return -1;
			bytesToAlign -= 4;
		}

		/* add main image (without checksum) to bin file*/
		if (update_out_file(f_out, buf_image, info.imageInfo.imageSize-4)!=0) return -1;

		DB(printf("Image size (include checksum) : %d\n",info.imageInfo.imageSize));
		
		/* calculate checksum and update bin file */
		chsum32 = checksum32((u32)buf_image, info.imageInfo.imageSize-4, 0);
		DB(printf("check sum: 0x%x\n",chsum32));
		if (update_out_file(f_out, (char*)&chsum32, 4)!=0) return -1;

		
		if (buf_image) 
			munmap((void*)buf_image, get_file_size(f_image));	

		close_file(f_image);
		close_file(f_out);
	}

	return 0;			
}

REG_FILE_STATUS read_board_reg_list(FILE *f_dram, RegFileHdr_t *regHdr, unsigned int *dram_reg)
{
	int i=0, status;

	unsigned int deviceID;

	/* first, scan the device ID */
	status = fscanf(f_dram,"%x\n",&deviceID);
	if (status==EOF)
	{
		fprintf(stderr,"Bad register file format for board 0x%X\n",deviceID);
		fprintf(stderr,"File most end with 0xFFFFFFFF\n");
		return REG_FILE_FORMAT_ERROR;
	}
	else if (deviceID == 0xFFFFFFFF)
	{
		return REG_FILE_END;
	}
	else if (deviceID == 0)
	{
		fprintf(stderr,"Bad register file format for board 0x%X\n",deviceID);
		fprintf(stderr,"Device ID cannot be 0\n");
		return REG_FILE_FORMAT_ERROR;
	}

	do {
		if (i/2 > MAX_DRAM_REGS_NUM)
		{
			fprintf(stderr,"Bad register file format for board 0x%X\n",deviceID);
			fprintf(stderr,"Too many registers. Max allowed registers number is %d\n",MAX_DRAM_REGS_NUM);
			return REG_FILE_FORMAT_ERROR;

		}

		if (status = fscanf(f_dram,"%x\n",&dram_reg[i++])==EOF)
		{
			fprintf(stderr,"Bad register file format for board 0x%X\n",deviceID);
			fprintf(stderr,"board registers list doesn't end with 0\n");
			return REG_FILE_FORMAT_ERROR;
		}
		//DEBUG(printf("%d: 0x%X\n",i,dram_reg[i-1]));
		
	} while (dram_reg[i-1] != 0 || (i%2==0) /* register data can be 0*/);

	/* At this point we finished reading this board registers and reached
 	   0x0. We read one more int since it's also 0x0 */
	status = fscanf(f_dram,"%x\n",&dram_reg[i++]);
	if(dram_reg[i-1] != 0)
	{
		fprintf(stderr,"Bad register file format for board 0x%X\n",deviceID);
		fprintf(stderr,"board registers list doesn't end with 0\n");
	}


	regHdr->deviceId = deviceID;
	regHdr->dramRegsNum = i/2 - 1; /* last register is 0x0 (sector end) */
	regHdr->nextSecOffs = (regHdr->dramRegsNum)*8 + sizeof(RegFileHdr_t);

	DB(printf("regHdr->deviceId: 0x%X\nregHdr->dramRegsNum: %d\nregHdr->nextSecOffs: 0x%X\n",
			regHdr->deviceId,regHdr->dramRegsNum,regHdr->nextSecOffs));

	return (status==EOF) ? REG_FILE_END : REG_FILE_OK;
}

int update_out_file(int f_out, char* buf_in, int buf_size)
{
	int size_written=0, err;

	/* copy buffer to output image */
	size_written += write(f_out, buf_in, buf_size);

	if (size_written != buf_size)
	{
		DB(printf("size_written!=buf_size\nsize_written: %d, buf_size:%d\n",size_written,buf_size));
		fprintf(stderr,"Error writing to output file \n");
		return -1;
	}

	return 0;
}

int get_file_size(int f_out)
{
	struct stat fs_stat;
	int err;

	/* get the size of the input image */
	err = fstat(f_out, &fs_stat);

	if (0 != err)
	{
		close(f_out);
		fprintf(stderr,"fstat failed for file. Error: err=%d\n",err);
		exit(1);
	}

	return fs_stat.st_size;
}

int open_file_rw(char* fname_out,int is_out_file_exist, int* f_out)
{
	/* open the output image */
	if (is_out_file_exist == 0)
	{
        	*f_out = open(fname_out, O_RDWR|O_TRUNC|O_CREAT|O_BINARY, 0666);
	}
	else
	{
        	*f_out = open(fname_out, O_RDWR|O_BINARY);
		if (*f_out == -1)
	        	*f_out = open(fname_out, O_RDWR | O_TRUNC | O_CREAT | O_BINARY,
			              S_IRWXU | S_IRWXG | S_IRWXO);
	}
	
	if (*f_out == -1)
	{
		fprintf(stderr,"Error openning %s file \n",fname_out);
		return -1;
    	}
	
	return 1;
}

int open_file_read(char* fname_in, int* f_in)
{
	/* open input image */
	*f_in = open(fname_in,O_RDONLY|O_BINARY);
	if (*f_in == -1)
	{
		fprintf(stderr,"File '%s' not found \n",fname_in);
		exit(0);
	}
}

int close_file(int f_desc)
{
	if (f_desc != -1) 
		close(f_desc);
}

/* map input image */
int map_file(int f_in, char** buf_in)
{
	*buf_in = mmap(0, get_file_size(f_in), PROT_READ, MAP_SHARED, f_in, 0);
	if (!(*buf_in))
	{
		fprintf(stderr,"Error mapping file \n");
		return -1;
	}

	return 1;
}



int reset_argv(BHR_t* hdr)
{
	int i,j;

	char** argv = (char**)hdr->rsvd1;
	int argc    = (int)hdr->rsvd2;
	int new_argc = argc;

	/* set boot device */
	BOOT_DEV boot_dev = (BOOT_DEV)hdr->blockID;
	DB(printf("boot_dev: %d\n",boot_dev));

	/* set generic image dest/exec */
	int gen_dest = hdr->destinationAddr;
	int gen_exec = hdr->executionAddr;

	for(i=0; i<argc; i++)
	{
		switch (*argv[i]) 
		{
		case 'T': /* image type */
			//DEBUG(printf("found flash type\n"));
			argv[i] = "-T";
			if (boot_dev==BOOT_SPI)
				argv[i+1] = "flash";
			else
				argv[i+1] = "nand";
			break;
		case 'B': /* boot device - should be removed */
			//DEBUG(printf("found boot device\n"));
			argv[i] = "";
			argv[i+1] = "";
			break;
		case 'D': /* image destination type */
			//DEBUG(printf("found destination\n"));
			argv[i] = "-D";
			argv[i+1] = gen_dest_addr;
			sprintf(argv[i+1],"0x%x",gen_dest);
			break;
		case 'E': /* image execution */
			//DEBUG(printf("found execution\n"));
			argv[i] = "-E";
			argv[i+1] = gen_exec_addr;
			sprintf(argv[i+1],"0x%x",gen_exec);
			break;
		case 'R': /* register - should be removed */
			//DEBUG(printf("found registers - removing\n"));
			if (boot_dev==BOOT_SPI)
			{
				argv[i] = "";
				argv[i+1] = "";
			}
			else
			{
				argv[i] = "-R";
				argv[i+1] = "dramregs_db98dx4122_h320.txt";
			}
			break;
		case 'G': /* generic - should be removed/replaced */
			//DEBUG(printf("found generic destination - removing\n"));
			argv[i] = "";
			argv[i+1] = "";
			break;
		case 'F': /* generic - should be removed/replaced */
			//DEBUG(printf("found generic execution - removing\n"));
			argv[i] = "";
			argv[i+1] = "";
			break;
		case 'C': /* NAND ECC mode */
			DB(printf("found NAND ECC mode\n"));
			if (boot_dev==BOOT_SPI)
			{
				argv[i] = "";
				argv[i+1] = "";
			}
			else
			{
				argv[i] = "-C";
				argv[i+1] = nand_ecc_mode;
				sprintf(argv[i+1],"%d",hdr->nandEccMode);
			}
			break;
		case 'P': /* NAND page size */
			DB(printf("found NAND page size\n"));
			if (boot_dev==BOOT_SPI)
			{
				argv[i] = "";
				argv[i+1] = "";
			}
			else
			{
				argv[i] = "-P";
				argv[i+1] = nand_page_size;
				sprintf(argv[i+1],"%d",hdr->nandPageSize);
			}
			break;
		default:
			//printf("%s\n",argv[i]);
			break;
		}
	}

	/* the lst 3 parameters are file names
  	   we need to remove the image name */
	argv[argc - 3] = "";
	argv[argc - 2] = TEMP_IMAGE_NAME;

	/* remove empty cells */
	for(i=0; i<argc; i++)
	{
		if(argv[i]=="")
		{
			new_argc--;
			j=i+1;
			for(j=i+1; j<argc; j++)
			{
				if(argv[j]!="")
				{
					new_argc++;
					argv[i] = argv[j];
					argv[j] = "";
					break;
				}
			}	
		}
	}

	return new_argc;
}

