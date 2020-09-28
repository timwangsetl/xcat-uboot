/*******************************************************************************
   Copyright (C) Marvell International Ltd. and its affiliates

   This software file (the "File") is owned and distributed by Marvell
   International Ltd. and/or its affiliates ("Marvell") under the following
   alternative licensing terms.  Once you have made an election to distribute the
   File under one of the following license alternatives, please (i) delete this
   introductory statement regarding license alternatives, (ii) delete the two
   license alternatives that you have not elected to use and (iii) preserve the
   Marvell copyright notice above.

********************************************************************************
   Marvell Commercial License Option

   If you received this File from Marvell and you have entered into a commercial
   license agreement (a "Commercial License") with Marvell, the File is licensed
   to you under the terms of the applicable Commercial License.

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
********************************************************************************
   Marvell BSD License Option

   If you received this File from Marvell, you may opt to use, redistribute and/or
   modify this File under the following licensing terms.
   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

*   Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer.

*   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

*   Neither the name of Marvell nor the names of its contributors may be
        used to endorse or promote products derived from this software without
        specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef MV_OS_H
#define MV_OS_H

/*************/
/* Includes  */
/*************/

#include "mvTypes.h"
#include "mvCommon.h"
#include <malloc.h>
#include <common.h>
#include "mvSysHwConfig.h"
#include "mvCtrlEnvSpec.h"
#include "mvDevWriteLog.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Flush CPU pipe */
#define CPU_PIPE_FLUSH

#define INLINE             inline

#define msOsStrncmp        strncmp
#define mvOsPrintf         printf
#define mvOsSPrintf        sprintf
#define mvOsOutput         printf
#define mvOsMalloc         malloc
#define mvOsCalloc         calloc
#define mvOsMemset         memset
#define mvOsFree           free
#define mvOsMemcpy         memcpy
#define mvOsMemmove        memmove
#define mvOsDelay(ms)      udelay (ms * 1000)
#define mvOsSleep(us)      mvOsDelay (us)
#define mvOsTaskLock()
#define mvOsTaskUnlock()
#define mvOsIntLock()      0
#define mvOsIntUnlock(key)
#define mvOsUDelay(x)      udelay (x)
#define strtol             simple_strtoul

/*************/
/* Constants */
/*************/

#define MV_OS_WAIT_FOREVER              0

/*************/
/* Datatypes */
/*************/

#define CPU_PHY_MEM(x)          (MV_U32)x
#define CPU_MEMIO_CACHED_ADDR(x)        (void *)x
#define CPU_MEMIO_UNCACHED_ADDR(x)  (void *)x

/* CPU architecture dependent 32, 16, 8 bit read/write IO addresses */
#define MV_MEMIO32_WRITE(addr, data)    \
    ((*((volatile unsigned int *)(addr))) = ((unsigned int)(data)))

#define MV_MEMIO32_READ(addr)           \
    ((*((volatile unsigned int *)(addr))))

#define MV_MEMIO16_WRITE(addr, data)    \
    ((*((volatile unsigned short *)(addr))) = ((unsigned short)(data)))

#define MV_MEMIO16_READ(addr)           \
    ((*((volatile unsigned short *)(addr))))

#define MV_MEMIO8_WRITE(addr, data)     \
    ((*((volatile unsigned char *)(addr))) = ((unsigned char)(data)))

#define MV_MEMIO8_READ(addr)            \
    ((*((volatile unsigned char *)(addr))))

/* No Fast Swap implementation (in assembler) for ARM */
#define MV_32BIT_LE_FAST(val)            MV_32BIT_LE (val)
#define MV_16BIT_LE_FAST(val)            MV_16BIT_LE (val)
#define MV_32BIT_BE_FAST(val)            MV_32BIT_BE (val)
#define MV_16BIT_BE_FAST(val)            MV_16BIT_BE (val)

/* 32 and 16 bit read/write in big/little endian mode */

/* 16bit write in little endian mode */
#define MV_MEMIO_LE16_WRITE(addr, data) \
    MV_MEMIO16_WRITE (addr, MV_16BIT_LE_FAST (data))

/* 16bit read in little endian mode */
static __inline MV_U16 MV_MEMIO_LE16_READ(MV_U32 addr)
{
    MV_U16 data;

    data = (MV_U16)MV_MEMIO16_READ (addr);

    return (MV_U16)MV_16BIT_LE_FAST (data);
}

/* 32bit write in little endian mode */
#define MV_MEMIO_LE32_WRITE(addr, data) \
    MV_MEMIO32_WRITE (addr, MV_32BIT_LE_FAST (data))

/* 32bit read in little endian mode */
static __inline MV_U32 MV_MEMIO_LE32_READ(MV_U32 addr)
{
    MV_U32 data;

    data = (MV_U32)MV_MEMIO32_READ (addr);

    return (MV_U32)MV_32BIT_LE_FAST (data);
}

/******************************************************************************
* This debug function enable the write of each register that u-boot access to
* to an array in the DRAM, the function record only MV_REG_WRITE access.
* The function could not be operate when booting from flash.
* In order to print the array we use the printreg command.
******************************************************************************/
#if defined(REG_DEBUG)
#define REG_ARRAY_SIZE 4096
extern int reg_arry[REG_ARRAY_SIZE][2];
extern int reg_arry_index;

void reglog(unsigned int offset, unsigned int data);
#endif

/* Marvell controller register read/write macros */
#define MV_REG_VALUE(offset)          \
    (MV_MEMIO32_READ ((INTER_REGS_BASE | (offset))))

#define MV_REG_READ(offset)             \
    (MV_MEMIO_LE32_READ (INTER_REGS_BASE | (offset)))

#ifdef MV_DEV_WRITE_LOG_ENABLE
void MV_REG_WRITE(MV_U32 offset, MV_U32 val);
#else
#define MV_REG_WRITE(offset, val) \
        MV_MEMIO_LE32_WRITE((INTER_REGS_BASE | (offset)), (val));
#endif

#if defined(REG_DEBUG)
#define MV_REG_WORD_WRITE(offset, val)  \
    MV_MEMIO_LE16_WRITE ((INTER_REGS_BASE | (offset)), (val)); \
    reglog ((INTER_REGS_BASE | (offset)), (val));
#else
#define MV_REG_WORD_WRITE(offset, val)  \
    MV_MEMIO_LE16_WRITE ((INTER_REGS_BASE | (offset)), (val))
#endif

#define MV_REG_WORD_READ(offset)        \
    (MV_MEMIO16_READ ((INTER_REGS_BASE | (offset))))

#define MV_REG_BYTE_READ(offset)        \
    (MV_MEMIO8_READ ((INTER_REGS_BASE | (offset))))

#if defined(REG_DEBUG)
#define MV_REG_BYTE_WRITE(offset, val)  \
    MV_MEMIO8_WRITE ((INTER_REGS_BASE | (offset)), (val)); \
    reglog ((INTER_REGS_BASE | (offset)), (val));
#else
#define MV_REG_BYTE_WRITE(offset, val)  \
    MV_MEMIO8_WRITE ((INTER_REGS_BASE | (offset)), (val))
#endif

#if defined(REG_DEBUG)
#define MV_REG_BIT_SET(offset, bitMask)                 \
    (MV_MEMIO32_WRITE ((INTER_REGS_BASE | (offset)), \
                       (MV_MEMIO32_READ (INTER_REGS_BASE | (offset)) | \
                        MV_32BIT_LE_FAST (bitMask)))); \
    reglog ((INTER_REGS_BASE | (offset)), \
            (MV_MEMIO32_READ (INTER_REGS_BASE | (offset))));
#else
#define MV_REG_BIT_SET(offset, bitMask)                 \
    (MV_MEMIO32_WRITE ((INTER_REGS_BASE | (offset)), \
                       (MV_MEMIO32_READ (INTER_REGS_BASE | (offset)) | \
                        MV_32BIT_LE_FAST (bitMask))))
#endif

#if defined(REG_DEBUG)
#define MV_REG_BIT_RESET(offset, bitMask)                \
    (MV_MEMIO32_WRITE ((INTER_REGS_BASE | (offset)), \
                       (MV_MEMIO32_READ (INTER_REGS_BASE | (offset)) & \
                        MV_32BIT_LE_FAST (~bitMask)))); \
    reglog ((INTER_REGS_BASE | (offset)), \
            (MV_MEMIO32_READ (INTER_REGS_BASE | (offset))));
#else
#define MV_REG_BIT_RESET(offset, bitMask)                \
    (MV_MEMIO32_WRITE ((INTER_REGS_BASE | (offset)), \
                       (MV_MEMIO32_READ (INTER_REGS_BASE | (offset)) & \
                        MV_32BIT_LE_FAST (~bitMask))))
#endif

/* Flash APIs */
#define MV_FL_8_READ            MV_MEMIO8_READ
#define MV_FL_16_READ           MV_MEMIO_LE16_READ
#define MV_FL_32_READ           MV_MEMIO_LE32_READ
#define MV_FL_8_DATA_READ       MV_MEMIO8_READ
#define MV_FL_16_DATA_READ      MV_MEMIO16_READ
#define MV_FL_32_DATA_READ      MV_MEMIO32_READ
#define MV_FL_8_WRITE           MV_MEMIO8_WRITE
#define MV_FL_16_WRITE          MV_MEMIO_LE16_WRITE
#define MV_FL_32_WRITE          MV_MEMIO_LE32_WRITE
#define MV_FL_8_DATA_WRITE      MV_MEMIO8_WRITE
#define MV_FL_16_DATA_WRITE     MV_MEMIO16_WRITE
#define MV_FL_32_DATA_WRITE     MV_MEMIO32_WRITE

/* CPU cache information */
#define CPU_I_CACHE_LINE_SIZE   32    /* 2do: replace 32 with linux core macro */
#define CPU_D_CACHE_LINE_SIZE   32    /* 2do: replace 32 with linux core macro */

#define mvOsCacheLineFlushInv(handle, addr)              \
    {                                                                   \
        __asm volatile ("mcr p15, 0, %0, c7, c14, 1" : : "r" (addr));   \
        __asm volatile ("mcr p15, 1, %0, c15, c10, 1" : : "r" (addr));  \
        __asm volatile ("mcr p15, 0, %0, c7, c10, 4" : : "r" (addr));   \
    }

#define mvOsCacheLineInv(handle, addr)                   \
    {                                                                   \
        __asm volatile ("mcr p15, 0, %0, c7, c6, 1" : : "r" (addr));    \
        __asm volatile ("mcr p15, 1, %0, c15, c11, 1" : : "r" (addr));  \
    }

#if defined(MV_BRIDGE_SYNC_REORDER)
extern MV_U32 *mvUncachedParam;

static INLINE void mvOsBridgeReorderWA(void)
{
    /* sync write reordering in the bridge */
    volatile MV_U32 val = 0;

    val = mvUncachedParam[0];
}
#endif

static INLINE void mvOsBCopy(MV_U8 *srcAddr, MV_U8 *dstAddr, int byteCount)

{
    while (byteCount != 0)

    {
        *dstAddr = *srcAddr;

        dstAddr++;

        srcAddr++;

        byteCount--;
    }
}

/* ARM architecture APIs */
MV_U32  mvOsCpuRevGet (MV_VOID);
MV_U32  mvOsCpuPartGet (MV_VOID);
MV_U32  mvOsCpuArchGet (MV_VOID);
MV_U32  mvOsCpuVarGet (MV_VOID);
MV_U32  mvOsCpuAsciiGet (MV_VOID);
MV_U32 mvOsIoVirtToPhy(void *osHandle, void *pVirtAddr);
MV_U32 mvOsIoPhysToVirt(void *pDev, void *pVirtAddr);

void * mvOsIoUncachedMalloc(void *osHandle, MV_U32 size, MV_ULONG *pPhyAddr,
                            MV_U32 *memHandle);
void mvOsIoUncachedFree(void *osHandle, MV_U32 size, MV_ULONG phyAddr,
                        void *pVirtAddr,
                        MV_U32 memHandle);
void * mvOsIoCachedMalloc(void *osHandle, MV_U32 size, MV_ULONG *pPhyAddr,
                          MV_U32 *memHandle);
void mvOsIoCachedFree(void *osHandle, MV_U32 size, MV_ULONG phyAddr, void *pVirtAddr,
                      MV_U32 memHandle);
MV_U32 mvOsCacheClear(void *osHandle, void *p, int size);
MV_U32 mvOsCacheFlush(void *osHandle, void *p, int size);
MV_U32 mvOsCacheInvalidate(void *osHandle, void *p, int size);
int mvOsRand(void);
int mvOsStrCmp(const char *str1, const char *str2);

#ifdef __cplusplus
}
#endif

#define WAIT_FOREVER 1
#define semGive
#define semTake
#define SEM_ID int

#define MV_Rx_FUNCPTR int *
#define MV_Tx_COMPLETE_FUNCPTR int *

void mvOsCacheUnmap(void *pDev, const void *virtAddr, MV_U32 size);
#define BIT_MASK(bitNum)         (1 << (bitNum))

#define CHK_STS_VOID(origFunc) \
    { \
        MV_STATUS mvStatus = origFunc; \
        if (MV_OK != mvStatus) \
        { \
            return; \
        } \
    }

#define CHECK_STATUS(origFunc) \
    { \
        MV_STATUS mvStatus = origFunc; \
        if (MV_OK != mvStatus) \
        { \
            return mvStatus; \
        } \
    }

#define CHK_STS(origFunc) \
{ \
    MV_STATUS mvStatus = origFunc; \
    if (MV_OK != mvStatus) \
    { \
        return mvStatus; \
    } \
}

#define CHK_STAT_VX(origFunc) \
{ \
    STATUS mvStatus = origFunc; \
    if (OK != mvStatus) \
    { \
        return mvStatus; \
    } \
}

#define CHK_STAT_VX_VOID(origFunc) \
{ \
    STATUS mvStatus = origFunc; \
    if (OK != mvStatus) \
    { \
        return; \
    } \
}

#define CHECK_STATUS_EXIT(origFunc, exitFunc) \
    { \
        mvStatus = origFunc;    \
        if (MV_OK != mvStatus)  \
        {                       \
            mvBreakOnFail ();    \
            exitFunc;           \
            return mvStatus;    \
        }                       \
    }

#define CHECK_IF_LIB_INIT(vlnIsInit) \
    if (MV_FALSE == vlnIsInit) \
    { \
        return MV_NOT_INITIALIZED; \
    }

#define CHECK_INPUT_PARAM(p1) \
    if (NULL == p1) \
    { \
        return MV_BAD_PARAM; \
    }

MV_STATUS mvOsSemCreate(MV_8 *name, MV_U32 init, MV_U32 count, MV_U32 *smid);
MV_STATUS mvOsSemDelete(MV_U32 smid);
MV_STATUS mvOsSemWait(MV_U32 smid, MV_U32 time_out);
MV_STATUS mvOsSemSignal(MV_U32 smid);

MV_STATUS mvOsTaskCreate(char *         name,
                         unsigned long  prio,
                         unsigned long  stack,
                         unsigned       (*start_addr)(MV_VOID *),
                         MV_VOID *      arglist,
                         unsigned long *tid);
MV_STATUS mvOsTaskIdent(char *name, unsigned long *tid);
MV_STATUS mvOsTaskDelete(unsigned long tid);
MV_STATUS mvOsTaskSuspend(unsigned long tid);
MV_STATUS mvOsTaskResume(unsigned long tid);

#endif /* MV_OS_H */

