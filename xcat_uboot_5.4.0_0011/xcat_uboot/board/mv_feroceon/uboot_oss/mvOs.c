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

/* includes */
#include <asm/processor.h>
#include "mvOs.h"
#include "mvSysHwConfig.h"

static MV_U32 read_p15_c0 (void);

/* defines  */
#define ARM_ID_REVISION_OFFS    0
#define ARM_ID_REVISION_MASK    (0xf << ARM_ID_REVISION_OFFS)

#define ARM_ID_PART_NUM_OFFS    4
#define ARM_ID_PART_NUM_MASK    (0xfff << ARM_ID_PART_NUM_OFFS)

#define ARM_ID_ARCH_OFFS    16
#define ARM_ID_ARCH_MASK    (0xf << ARM_ID_ARCH_OFFS)

#define ARM_ID_VAR_OFFS     20
#define ARM_ID_VAR_MASK     (0xf << ARM_ID_VAR_OFFS)

#define ARM_ID_ASCII_OFFS   24
#define ARM_ID_ASCII_MASK   (0xff << ARM_ID_ASCII_OFFS)

MV_VOID arm926_flush_user_cache_range(void *osHandle, void *p, int size);
MV_VOID arm926_dma_inv_range(const void *startP, const void *endP);

/*******************************************************************************
* mvOsCpuVerGet() -
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit CPU Revision
*
*******************************************************************************/
MV_U32 mvOsCpuRevGet(MV_VOID)
{
    return (read_p15_c0 () & ARM_ID_REVISION_MASK) >> ARM_ID_REVISION_OFFS;
}

/*******************************************************************************
* mvOsCpuPartGet() -
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit CPU Part number
*
*******************************************************************************/
MV_U32 mvOsCpuPartGet(MV_VOID)
{
    return (read_p15_c0 () & ARM_ID_PART_NUM_MASK) >> ARM_ID_PART_NUM_OFFS;
}

/*******************************************************************************
* mvOsCpuArchGet() -
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit CPU Architicture number
*
*******************************************************************************/
MV_U32 mvOsCpuArchGet(MV_VOID)
{
    return (read_p15_c0 () & ARM_ID_ARCH_MASK) >> ARM_ID_ARCH_OFFS;
}

/*******************************************************************************
* mvOsCpuVarGet() -
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit CPU Variant number
*
*******************************************************************************/
MV_U32 mvOsCpuVarGet(MV_VOID)
{
    return (read_p15_c0 () & ARM_ID_VAR_MASK) >> ARM_ID_VAR_OFFS;
}

/*******************************************************************************
* mvOsCpuAsciiGet() -
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit CPU Variant number
*
*******************************************************************************/
MV_U32 mvOsCpuAsciiGet(MV_VOID)
{
    return (read_p15_c0 () & ARM_ID_ASCII_MASK) >> ARM_ID_ASCII_OFFS;
}

/*
   static unsigned long read_p15_c0 (void)
 */
/* read co-processor 15, register #0 (ID register) */
static MV_U32 read_p15_c0 (void)
{
    MV_U32 value;

    __asm__ __volatile__ (
        "mrc	p15, 0, %0, c0, c0, 0   @ read control reg\n"
        : "=r" (value)
        :
        : "memory");

    return value;
}

MV_U32 mvOsIoVirtToPhy(void *pDev, void *pVirtAddr)
{
    return (MV_U32)pVirtAddr;
}

MV_U32 mvOsIoPhysToVirt(void *pDev, void *pVirtAddr)
{
    return (MV_U32)pVirtAddr;
}

void * mvOsIoUncachedMalloc(void *osHandle, MV_U32 size, MV_ULONG *pPhyAddr,
                            MV_U32 *memHandle)
{
    *pPhyAddr = (MV_ULONG)malloc (size);
    return (void *)(*pPhyAddr);
}

void mvOsIoUncachedFree(void     *osHandle,
                        MV_U32    size,
                        MV_ULONG  phyAddr,
                        void     *pVirtAddr,
                        MV_U32    memHandle)
{
    free (pVirtAddr);
}

void * mvOsIoCachedMalloc(void *osHandle, MV_U32 size, MV_ULONG *pPhyAddr,
                          MV_U32 *memHandle)
{
    *pPhyAddr = (MV_ULONG)malloc (size);
    return (void *)(*pPhyAddr);
}

void mvOsIoCachedFree(void *osHandle, MV_U32 size, MV_ULONG phyAddr, void *pVirtAddr,
                      MV_U32 memHandle)
{
    free (pVirtAddr);
}

MV_U32 mvOsCacheClear(void *osHandle, void *p, int size)
{
    arm926_flush_user_cache_range (p, p + size, size);
    return (MV_U32)p;
}

MV_U32 mvOsCacheFlush(void *osHandle, void *p, int size)
{
    arm926_flush_user_cache_range (p, p + size, size);
    return (MV_U32)p;
}

MV_U32 mvOsCacheInvalidate(void *osHandle, void *p, int size)
{
    arm926_dma_inv_range (p, p + size);
    return (MV_U32)p;
}

int mvOsRand(void)
{
    return 0;
}

int mvOsStrCmp(const char *str1, const char *str2)
{
    do
    {
        if ((*str1++) != (*str2++))
        {
            return 1;                         /* not equal */
        }
    } while ((*str1 != '\0') && (*str2 != '\0'));

    if (*str1 != *str2)
    {
        return 1;                 /* not equal */
    }
    /* equal */
    return 0;
}

#if defined(REG_DEBUG)
extern int reg_arry[REG_ARRAY_SIZE][2];
extern int reg_arry_index;
void reglog(unsigned int offset, unsigned int data)
{
    reg_arry[reg_arry_index % REG_ARRAY_SIZE][0] = (offset);
    reg_arry[reg_arry_index % REG_ARRAY_SIZE][1] = (data);
    reg_arry_index++;
}
#endif

// extern void arm926_dma_inv_range(const void *start, const void *end);
void mvOsCacheUnmap(void *pDev, const void *virtAddr, MV_U32 size)
{
    arm926_dma_inv_range (virtAddr, (virtAddr + size));
}

/*******************************************************************************
 * mvOsSemCreate
 */
MV_STATUS mvOsSemCreate(MV_8 *name, MV_U32 init, MV_U32 count, MV_U32 *smid)
{
    return MV_OK;
}

/*******************************************************************************
 * mvOsSemDelete
 */
MV_STATUS mvOsSemDelete(MV_U32 smid)
{
    return MV_OK;
}

/*******************************************************************************
 * mvOsSemWait
 */
MV_STATUS mvOsSemWait(MV_U32 smid, MV_U32 time_out)
{
    return MV_OK;
}

/*******************************************************************************
 * mvOsSemSignal
 */
MV_STATUS mvOsSemSignal(MV_U32 smid)
{
    return MV_OK;
}

/*******************************************************************************
 * mvOsTaskCreate
 */
MV_STATUS mvOsTaskCreate(char *         name,
                         unsigned long  prio,
                         unsigned long  stack,
                         unsigned       (*start_addr)(MV_VOID *),
                         MV_VOID *      arglist,
                         unsigned long *tid)
{
    return MV_OK;
}

/*******************************************************************************
 * mvOsTaskIdent
 */
MV_STATUS mvOsTaskIdent(char *name, unsigned long *tid)
{
    return MV_OK;
}

/*******************************************************************************
 * mvOsTaskDelete
 */
MV_STATUS mvOsTaskDelete(unsigned long tid)
{
    return MV_OK;
}

/*******************************************************************************
 * mvOsTaskSuspend
 */
MV_STATUS mvOsTaskSuspend(unsigned long tid)
{
    return MV_OK;
}

/*******************************************************************************
 * mvOsTaskResume
 */
MV_STATUS mvOsTaskResume(unsigned long tid)
{
    return MV_OK;
}

/*******************************************************************************
 * mvOsTaskResume
 */
#ifdef MV_DEV_WRITE_LOG_ENABLE
void MV_REG_WRITE(MV_U32 offset, MV_U32 val)
{
    static void *logHandleP = NULL;
    MV_U32 addr = INTER_REGS_BASE | offset;

    if (logHandleP == NULL)
    {
        logHandleP = mvDevWriteLogCreate();
        if (logHandleP == NULL)
        {
            mvOsPrintf("%s: mvDevWriteLogCreate failed.\n", __func__);
            return;
        }
    }

    if (mvDevWriteLogAddEntry(logHandleP, addr, val) != MV_OK)
    {
        mvOsPrintf("%s: mvWriteLogAddEntry failed.\n", __func__);
        return;
    }

    MV_MEMIO_LE32_WRITE(addr, val);
}
#endif

