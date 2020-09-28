/*  File Name:  hwd_config.h  */

#ifndef __INChwd_configh
#define __INChwd_configh

#include "mvCommon.h"
#include "mvTypes.h"


extern MV_U32 _start;
extern MV_U32 _end;

#define ERROR(X)    (BIT31 | X)
#define IS_ERROR(X) ((X&BIT31)>0)

#define KBD_BREAK() \
if (ctrlc()) \
{ \
	putc ('\n'); \
	return ERROR(MV_TERMINATE); \
}

#define START_PRG ((MV_U32)(&_start))
#define END_PRG   ((MV_U32)(&_end))

#define MV_MEMIO_WRITE(addr, data)	(*(volatile BUS_WIDTH*)(addr)) = ((BUS_WIDTH)(data))
#define MV_MEMIO_READ(addr) (*(volatile BUS_WIDTH*)(addr))

#define DRAM_ADDRESSBUS_PATTERN      0x55
#define DRAM_ADDRESSBUS_ANTIPATTERN  0xAA

#define FLASH_ADDRESSBUS_PATTERN      0x55
#define FLASH_ADDRESSBUS_ANTIPATTERN  0x01
		 
#endif /*__INChwd_configh*/
