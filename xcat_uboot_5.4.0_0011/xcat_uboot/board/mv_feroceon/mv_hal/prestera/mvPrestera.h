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

#ifndef __INCmvPresterah
#define __INCmvPresterah

#include "mvTypes.h"
#include "mvEth.h"
#include "mvSysHwConfig.h"
#include "mvGenPool.h"

/*
 * Global Configuration Registers
 */

/*
 * dev_id[15:10] bits of DeviceID register of Prestera (0x4C)
 * determine the chip type (xCat or xCat2).
 * dev_id[15:10] == 0x37 stands for xCat
 * dev_id[15:10] == 0x39 stands for xCat2
 */
#define MV_PP_CHIP_TYPE_MASK                        0x000FC000
#define MV_PP_CHIP_TYPE_OFFSET                      14
#define MV_PP_CHIP_TYPE_XCAT                        0x37
#define MV_PP_CHIP_TYPE_XCAT2                       0x39

#define PRESTERA_ADDR_COMPLETION_REG                0x0
#define PRESTERA_FTDLL_REG                          0xC
#define PRESTERA_DEV_CFG_REG                        0x28
#define PRESTERA_SAR_REG                            0x2C
#define PRESTERA_LAST_READ_TIMESTAMP_REG            0x40
#define PRESTERA_DEV_ID_REG                         0x4C
#define PRESTERA_VENDOR_ID_REG                      0x50
#define PRESTERA_EXT_GLOBAL_CFG_REG                 0x5C

#define PRESTERA_DEV_CONFIG_REG                     0x28
#define PRESTERA_SAMPLE_AT_RESET_REG                0x2C
#define PRESTERA_GLOBAL_CTRL_REG                    0x58

#define PRESTERA_DRAGONITE_CPU_CTRL_REG             0x88
#define PRESTERA_EXT_GLOBAL_CFG_2_REG               0x8C
#define PRESTERA_ANALOG_CFG_REG                     0x9C

#define MV_VENDOR_ID 0x11AB

typedef struct _mvPpDevCfg
{
    MV_U32          prependCfgReg;
    MV_U32          prependCfgBit;
    MV_U32          dsaEnableCfgReg;
    MV_U32          dsaEnableBit;
} MV_PP_DEV_CFG;

typedef struct _mvPpHalCtrl
{
    void           *ppLogHandleP;
    GEN_POOL       *txPktInfoPoolP;
    MV_PP_DEV_CFG   devCfg;
} MV_PP_HAL_CTRL;

#define PP_CPU_PORT_NUM     (63) /* = 0x3F */

#define PP_ALL_Q_MASK       (0xFF)

/* Calculate the field mask for a given offset & length */
/* e.g.: BIT_MASK(8,2) = 0xFFFFFCFF                     */
#define FIELD_MASK_NOT(offset,len)                      \
        (~(BIT_MASK((len)) << (offset)))

/* Calculate the field mask for a given offset & length */
/* e.g.: BIT_MASK(8,2) = 0x00000300                     */
#define FIELD_MASK(offset,len)                      \
        ( (BIT_MASK((len)) << (offset)) )

/* Returns the info located at the specified offset & length in data.   */
#define U32_GET_FIELD(data,offset,length)           \
        (((data) >> (offset)) & ((1 << (length)) - 1))

/* Sets the field located at the specified offset & length in data.     */
#define U32_SET_FIELD(data,offset,length,val)           \
   (data) = (((data) & FIELD_MASK_NOT((offset),(length))) | ((val) <<(offset)))

#define MAC_BYTES 6
#define DSA_TAG_SIZE        (4)
#define EXTEND_DSA_TAG_SIZE (8)

/* Max number of PPs in system  */
#define MAX_PP_DEVICES              (128)

#define MV_PP_NUM_OF_PORTS                  (48)
#define MV_PP_NUM_OF_PORTS_PER_DEV          (24)

#define SYS_CONF_MAX_DEV    128

/* Maximum interrupts number in system per PP   */
#define     PP_INTERRUPT_MAX_NUM        MV_NUM_OF_INT_CAUSE

/* Number of inerrupt queues                    */
#define     NUM_OF_INT_QUEUES           8

/* The interrupt queue handling policy for all  */
/* queues.                                      */
#define     INT_QUEUES_POLICY           MV_STRICT_PRIO
#define     INT_QUEUES_WEIGHT           10

#define     INT_NOT_EXIST   0x0

#define TX_STACK_SIZE                       0x2000

#define CRC_SIZE                            4
#define MAC_SA_AND_DA_SIZE                  12

#define MRVL_TAG_PORT_BIT                   19
#define MRVL_TAG_DEV_BIT                    24
#define MRVL_TAG_EXT_PORT_BIT               10

#define MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN   64
#define MIN_PCKT_SIZE                       64

/* Size of block to aligned to. Must be power of 2, 2 is minimum */
#define ALIGN_ADDR_CHUNK                    64
/* This macro used to get alighnment address from allocated */
#define ALIGN_ADR_FROM_MEM_BLOCK(adr,align) \
                   (( ((MV_U32)( ((MV_U8*)(adr)) + (ALIGN_ADDR_CHUNK) - 1)) & \
                   ((0xFFFFFFFF - (ALIGN_ADDR_CHUNK) + 1) )) + (align))

#define TX_DESC_ALIGN       (16)  /* In bytes */
#define TX_SHORT_BUFF_SIZE  (8)   /* Bytes    */
#define TX_HEADER_SIZE      (16)  /* Bytes    */
#define TX_DESC_SIZE        (16)  /* In Bytes */

/* Resets the Tx descriptor's word1 & word2.                        */
#define TX_DESC_RESET(txDesc)                                           \
            (txDesc)->word1 = 0x0;                                      \
            (txDesc)->word2 = 0x0

/* Copy a tx descriptor from one struct to another      */
#define TX_DESC_COPY(dstDesc,srcDesc)                                   \
            (dstDesc)->buffPointer      = (srcDesc)->buffPointer;       \
            (dstDesc)->word2            = (srcDesc)->word2;             \
            (dstDesc)->word1            = (srcDesc)->word1;

/* Get / Set the Own bit field of the tx descriptor.    */
#define TX_DESC_GET_OWN_BIT(txDesc)                         \
            ( (hwByteSwap((txDesc)->word1) >> 31) & 0x1)
#define TX_DESC_SET_OWN_BIT(txDesc,val)                     \
            (U32_SET_FIELD((txDesc)->word1,31,1,(val & 1)))

/* Get / Set the AM bit field of the tx descriptor.    */
#define TX_DESC_GET_AM_BIT(txDesc)                         \
            ( (hwByteSwap((txDesc)->word1) >> 30) & 0x1 )
#define TX_DESC_SET_AM_BIT(txDesc,val)                     \
            (U32_SET_FIELD((txDesc)->word1,30,1,(val & 1)))

/* Get / Set the Int bit field of the tx descriptor.    */
#define TX_DESC_GET_INT_BIT(txDesc)                          \
        ( (hwByteSwap( (txDesc)->word1 ) >> 23) & 0x1)
#define TX_DESC_SET_INT_BIT(txDesc,val)                      \
            (U32_SET_FIELD((txDesc)->word1,23,1,(val & 1)))

/* Get / Set the First bit field of the tx descriptor.  */
#define TX_DESC_GET_FIRST_BIT(txDesc)                       \
            ((hwByteSwap((txDesc)->word1) >> 21) & 0x1)
#define TX_DESC_SET_FIRST_BIT(txDesc,val)                   \
            (U32_SET_FIELD((txDesc)->word1,21,1,(val & 1)))

/* Get / Set the Last bit field of the tx descriptor.   */
#define TX_DESC_GET_LAST_BIT(txDesc)                        \
            ((hwByteSwap((txDesc)->word1)>> 20) & 0x1)
#define TX_DESC_SET_LAST_BIT(txDesc,val)                    \
            (U32_SET_FIELD((txDesc)->word1,20,1,(val & 1)))

/* Get / Set the recalc crc bit field of the tx descriptor.   */
#define TX_DESC_GET_RECALC_CRC_BIT(txDesc)                        \
            ((hwByteSwap((txDesc)->word1)>> 12) & 0x1)
#define TX_DESC_SET_RECALC_CRC_BIT(txDesc,val)                    \
            (U32_SET_FIELD((txDesc)->word1,12,1,(val & 1)))

/* Get / Set the Byte Count field in the tx descriptor.       */
#define TX_DESC_GET_BYTE_CNT_FIELD(txDesc)                       \
        (U32_GET_FIELD(hwByteSwap((txDesc)->word2),16,14) )
#define TX_DESC_SET_BYTE_CNT(txDesc,val)                         \
            (U32_SET_FIELD((txDesc)->word2,16,14,(val & 0x3fff)))

/*******************************************************************************
 * RX Related Definitions
 */

#define RX_DESC_ALIGN       (16)  /* In bytes */
#define RX_BUFF_ALIGN       (8)   /* In bytes */
#define RX_BUFF_SIZE_MULT   (8)   /* In bytes */

#define RX_MAX_PACKET_DESC  (1000)    /* Max number of descriptors    */
                                    /* per Rx packet.               */

/* Resets the Rx descriptor's word1 & word2.                        */
#define RX_DESC_RESET(rxDesc) ((rxDesc)->word1 = hwByteSwap(0xA0000000))

/* Returns true if Rx descriptor is owned by CPU */
#define IS_RX_DESC_CPU_OWNED(rxDesc) ((((rxDesc)->word1) & BIT_31) == 0)

/* Returns true if Rx descriptor is owned by DMA */
#define IS_RX_DESC_DMA_OWNED(rxDesc) ((((rxDesc)->word1) & BIT_31) != 0)

/* Returns true if Tx descriptor is owned by CPU */
#define IS_TX_DESC_CPU_OWNED(txDesc) ((((txDesc)->word1) & BIT_31) == 0)

/* Returns true if Tx descriptor is owned by DMA */
#define IS_TX_DESC_DMA_OWNED(txDesc) ((((txDesc)->word1) & BIT_31) != 0)

/* Returns the Own bit field of the rx descriptor.           */
#define RX_DESC_GET_OWN_BIT(rxDesc) (((rxDesc)->word1) >> 31)

/* Sets the Own bit field of the rx descriptor.           */
#define RX_DESC_SET_OWN_BIT(rxDesc,val)   \
         (rxDesc)->word1 = hwByteSwap( (rxDesc)->word1); \
         U32_SET_FIELD( (rxDesc)->word1,31,1,(val & 1));      \
         (rxDesc)->word1 = hwByteSwap((rxDesc)->word1)

/* Returns the First bit field of the rx descriptor.                */
#define RX_DESC_GET_FIRST_BIT(rxDesc) ((((rxDesc)->word1) >> 27) & 0x1)

/* Returns the Last bit field of the rx descriptor.                 */
#define RX_DESC_GET_LAST_BIT(rxDesc) ((((rxDesc)->word1) >> 26) & 0x1)

/* Returns true if last and fist bits are set */
#define IS_RX_DESC_LAST_AND_FIST_SET(rxDesc)                                   \
        ((((rxDesc)->word1) & (BIT_26 | BIT_27)) == (BIT_26 | BIT_27))

/* Returns the Resource error bit field of the rx descriptor.       */
#define RX_DESC_GET_REC_ERR_BIT(rxDesc) ((((rxDesc)->word1) >> 28) & 0x1)

/* Returns the bus error bit field of the rx descriptor.       */
#define RX_DESC_GET_BUS_ERR_BIT(rxDesc) ((((rxDesc)->word1) >> 30) & 0x1)

/* Returns the enable interrupt bit field of the rx descriptor.       */
#define RX_DESC_GET_EI_BIT(rxDesc) ((((rxDesc)->word1) >> 29) & 0x1)

/* Sets the packet encapsulation field of the rx descriptor.       */
#define RX_DESC_SET_EI_BIT(rxDesc,val)                     \
         (rxDesc)->word1 = hwByteSwap( (rxDesc)->word1);   \
         U32_SET_FIELD( (rxDesc)->word1,29,1,(val & 1));      \
         (rxDesc)->word1 = hwByteSwap((rxDesc)->word1)

/* Return the valid crc bit from the second word of an Rx desc.     */
#define RX_DESC_GET_VALID_CRC_BIT(rxDesc)   \
            ((MV_BOOL)((((rxDesc)->word2) >> 30) & 0x1))

/* Return the buffer size field from the second word of an Rx desc. */
/* Make sure to set the lower 3 bits to 0.                          */
#define RX_DESC_GET_BUFF_SIZE_FIELD(rxDesc)             \
            (((hwByteSwap((rxDesc)->word2) >> 3) & 0x7FF) << 3)
#define RX_DESC_GET_BUFF_SIZE_FIELD_NO_SWAP(rxDesc)             \
            ((((rxDesc)->word2 >> 3) & 0x7FF) << 3)

#define RX_DESC_SET_BUFF_SIZE_FIELD(rxDesc,val)         \
            (rxDesc)->word2 = hwByteSwap((rxDesc)->word2);  \
            U32_SET_FIELD((rxDesc)->word2,3,11,(val & 0x7ff) );      \
            (rxDesc)->word2 = hwByteSwap((rxDesc)->word2)

/* Return the byte count field from the second word of an Rx desc.  */
/* Make sure to set the lower 3 bits to 0.                          */
#define RX_DESC_GET_BYTE_COUNT_FIELD(rxDesc)        \
            ((((rxDesc)->word2) >> 16) & 0x3FFF)

/****************** Marvell tag ***********************************************/
/* Return the Tag command field from the first word of a DSA          */
#define DSA_GET_TAG_CMD_FIELD(hwDsa)  (((hwDsa).word0 >> 30) & 0x3)

/* Return the SrcTagged/TrgTagged field from the first word of a DSA          */
#define DSA_GET_SRC_TRG_TAGGED_FIELD(hwDsa) (((hwDsa).word0 >> 29) & 0x1)

/* Return the SrcDev/TrgDev field from the first word of a DSA          */
#define RX_DSA_GET_SRC_TRG_DEV_FIELD(hwDsa) ((((hwDsa).word0) >> 24) & 0x1F)

/* Return the SrcPort/TrgPort field from the first and seconds words of a DSA */
#define DSA_GET_SRC_TRG_PORT_FIELD(hwDsa)             \
    ((((((hwDsa).word1) >> 10) & 0x3) << 5) | ((((hwDsa).word0) >> 19) & 0x1F))

/* Return the UP field from the first word of a DSA          */
#define DSA_GET_UP_FIELD(hwDsa)  ((((hwDsa).word0) >> 13) & 0x7)

/* Return the CPU code field from the first word of a DSA          */
#define RX_DSA_GET_CPU_CODE_FIELD(hwDsa)             \
      ((((((hwDsa).word0) >> 16) & 0x7) << 1) | ((((hwDsa).word0) >> 12) & 0x1))

/* Return the VID field from the first word of a DSA          */
#define DSA_GET_VID_FIELD(hwDsa)  (((hwDsa).word0) & 0xFFF)

/* Return the Extend field from the second word of a DSA          */
#define RX_DSA_GET_EXTEND_FIELD(hwDsa)  ((((hwDsa).word1) >> 31) & 0x1)

/* Return the CFI field from the second word of a DSA          */
#define RX_DSA_GET_CFI_FIELD(hwDsa)  ((((hwDsa).word1) >> 30) & 0x1)

/* Return the Truncated field from the second word of a DSA          */
#define RX_DSA_GET_TRUNCATED_FIELD(hwDsa)  ((((hwDsa).word1) >> 26) & 0x1)

/* Return the Packet original byte count field from the second word of a DSA  */
#define RX_DSA_GET_PCKT_ORIG_BYTE_COUNT_FIELD(hwDsa)  \
            ((((hwDsa).word1) >> 12) & 0x3FFF)

/* Return the SrcTrg field from the second word of a DSA  */
#define RX_DSA_GET_SRC_TRG_FIELD(hwDsa)  ((((hwDsa).word1) >> 8) & 0x1)

/* Return the Long CPU code field from the second word of a DSA  */
#define RX_DSA_GET_LONG_CPU_CODE_FIELD(hwDsa)  (((hwDsa).word1) & 0xFF)


/* Set the Tag command field from the first word of a DSA          */
#define DSA_SET_TAG_CMD_FIELD(hwDsa, val)  \
    (U32_SET_FIELD((hwDsa).word0,30,1,(val & 1)))

/* Set the VID field from the first word of a DSA          */
#define DSA_SET_VID_FIELD(hwDsa, val)  \
    (U32_SET_FIELD((hwDsa).word0,0,12,(val & 0xfff)))

/* Set the SrcTagged/TrgTagged field from the first word of a DSA          */
#define DSA_SET_SRC_TRG_TAGGED_FIELD(hwDsa, val)   \
    (U32_SET_FIELD((hwDsa).word0,29,1,(val & 1)))

/* Set the SrcDev/TrgDev field from the first word of a DSA          */
#define RX_DSA_SET_SRC_TRG_DEV_FIELD(hwDsa, val) \
    (U32_SET_FIELD((hwDsa).word0,24,5,(val & 0x1f)));

/* Set the SrcPort/TrgPort field from the first and seconds words of a DSA */
#define DSA_SET_SRC_TRG_PORT_FIELD(hwDsa, val)        \
    (U32_SET_FIELD((hwDsa).word0,19,5,(val & 0x1f)));          \
    (U32_SET_FIELD((hwDsa).word1,10,3,(val >> 5)))

/* Set the UP field from the first word of a DSA          */
#define DSA_SET_UP_FIELD(hwDsa, val)  \
     (U32_SET_FIELD((hwDsa).word0,13,3,(val & 7)))

/* Set the DropOnSource field from Rx DSA          */
#define RX_DSA_SET_DROP_ON_SRC_FIELD(hwDsa, val)   \
    (U32_SET_FIELD((hwDsa).word1,29,1,(val & 1)))
/* Return the DropOnSource field from the DSA  */
#define RX_DSA_GET_DROP_ON_SRC_FIELD(hwDsa)  ((((hwDsa).word1) >> 29) & 0x1)

/* Set the PacketIsLooped field from Rx DSA          */
#define RX_DSA_SET_PKT_IS_LOOPED_SRC_FIELD(hwDsa, val)   \
    (U32_SET_FIELD((hwDsa).word1,28,1,(val & 1)))
/* Return the PacketIsLooped field from the DSA  */
#define RX_DSA_GET_PKT_IS_LOOPED_FIELD(hwDsa)  ((((hwDsa).word1) >> 28) & 0x1)

/* Set the OrigIsTrunk field from Rx DSA          */
#define RX_DSA_SET_ORIG_IS_TRUNK_SRC_FIELD(hwDsa, val)   \
    (U32_SET_FIELD((hwDsa).word1,27,1,(val & 1)))
/* Return the OrigIsTrunk field from the DSA  */
#define RX_DSA_GET_ORIG_IS_TRUNK_FIELD(hwDsa)  ((((hwDsa).word1) >> 27) & 0x1)

/************************ From CPU DSA tag fields *****************************/

/* Return the VIDX code field from the first & second word of a DSA          */
#define TX_DSA_GET_VIDX_FIELD(hwDsa)             \
   ((((((hwDsa).word1) >> 12) & 0x3) << 10) | ((((hwDsa).word0) >> 19) & 0x3FF))

/* Set the VIDX field from the first & second word of a DSA          */
#define TX_DSA_SET_VIDX_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word0,19,10,(val & 0x3ff))); \
      (U32_SET_FIELD((hwDsa).word1,12,2,(val >> 10)))

/* Set the USE_VIDX field from the first word of a DSA          */
#define TX_DSA_SET_USE_VIDX_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word0,18,1,(val & 1)))

/* Set the TC field from the first & second word of a DSA          */
#define TX_DSA_SET_TC_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word0,17,1,(val & 1))); \
      (U32_SET_FIELD((hwDsa).word1,14,1,((val >> 1) & 1))); \
      (U32_SET_FIELD((hwDsa).word1,27,1,((val >> 2) & 1)))

/* Set the CFI field from the first word of a DSA          */
#define TX_DSA_SET_CFI_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word0,16,1,(val & 1)))

/* Set the Extend field from the first word of a DSA          */
#define TX_DSA_SET_EXTEND0_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word0,12,1,(val & 1)))

/* Set the Extend field from the second word of a DSA          */
#define TX_DSA_SET_EXTEND1_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word1,31,1,(val & 1)))

/* Set the EgressFilterEn field from the second word of a DSA          */
#define TX_DSA_SET_EGRESS_FLTR_EN_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word1,30,1,(val & 1)))

/* Set the Cascade Control field from the second word of a DSA          */
#define TX_DSA_SET_CASCADE_CNTRL_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word1,29,1,(val & 1)))

/* Set the EgressFilterReg field from the second word of a DSA          */
#define TX_DSA_SET_EGRESS_FLTR_REG_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word1,28,1,(val & 1)))

/* Set the PacketIsLooped field from the second word of a DSA          */
#define TX_DSA_SET_PKT_IS_LOOPED_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word1,25,1,(val & 1)))

/* Set the DropOnSource field from the second word of a DSA          */
#define TX_DSA_SET_DROP_ON_SRC_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word1,26,1,(val & 1)))

    /* Set the Src ID field from the second word of a DSA          */
#define TX_DSA_SET_SRC_ID_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word1,20,5,(val & 0x1f)))

/* Set SrcDev field from the first word of a DSA          */
#define TX_DSA_SET_SRC_DEV_FIELD(hwDsa)     \
    (U32_SET_FIELD((hwDsa).word1,15,5,(val & 0x1f)));
/* Set TrgDev field from the first word of a DSA          */
#define TX_DSA_SET_TRG_DEV_FIELD(hwDsa)     \
    (U32_SET_FIELD((hwDsa).word0,24,5,(val & 0x1f)));

/* Set the ExcludeIsTrunk field from the second word of a DSA          */
#define TX_DSA_SET_EXCL_IS_TRUNK_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word1,11,1,(val & 1)))

/* Set the MirrotTo All CPUs field from the second word of a DSA          */
#define TX_DSA_SET_MIRROR_TO_ALL_CPU_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word1,10,1,(val & 1)))

/* Set the ExcludedTrunk field from the second word of a DSA          */
#define TX_DSA_SET_EXCL_TRUNK_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word1,0,7,(val & 0x7f)))

/* Set the ExcludedDev field from the second word of a DSA          */
#define TX_DSA_SET_EXCL_DEV_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word1,0,5,(val & 0x1f)));

/* Set the ExcludedPort field from the second word of a DSA          */
#define TX_DSA_SET_EXCL_PORT_FIELD(hwDsa,val)             \
      (U32_SET_FIELD((hwDsa).word1,5,6,(val & 0x3f)))

/*
 * Typedef: struct STRUCT_TX_DESC
 *
 * Description: Includes the PP Tx descriptor fields, to be used for handling
 *              packet transmits.
 *
 * Fields:
 *      word1           - First word of the Tx Descriptor.
 *      word2           - Second word of the Tx Descriptor.
 *      buffPointer     - The physical data-buffer address.
 *      virtBuffPointer - The virtual data-buffer address.
 *      nextDescPointer - The physical address of the next Tx descriptor.
 *
 */
typedef struct _txDesc
{
    volatile MV_U32         word1;
    volatile MV_U32         word2;

    volatile MV_U32         buffPointer;
    volatile MV_U32         nextDescPointer;
}STRUCT_TX_DESC;

/*
 * typedef: struct STRUCT_SW_TC_DESC
 *
 * Description: Sw management Tx descirptor.
 *
 * Fields:
 *      txDesc          - Points to the Tx descriptor representing this Sw Tx
 *                        desc.
 *      swNextDesc      - A pointer to the next descriptor in the linked-list.
 *      userData        - A data to be stored in mvBuf on packet transmit
 *                        request, and returned to user on txEnd.
 *
 */
typedef struct _swTxDesc
{
    STRUCT_TX_DESC     *txDesc;
    MV_U8               txBufferAligment;
    struct _swTxDesc   *swNextDesc;
    volatile MV_U32     virtBuffPointer;
    MV_VOID            *userData;
} STRUCT_SW_TX_DESC;

/*
 * Typedef: struct TX_DESC_LIST
 *
 * Description: The control block of a single Tx descriptors list.
 *
 * Fields:
 *
 *      next2Free       - Points to the descriptor from which the next
 *                        transmitted buffer should be freed, When receiving
 *                        a TxEnd interrupt.
 *      next2Feed       - Points to the descriptor to which the next transmitted
 *                        packet should be attached.
 *                        (This actually points to the first desc. of the packet
 *                        in case of a multi desc. packet).
 *      freeDescNum     - Number of free descriptors in list.
 *      txListSem       - Semaphore for mutual exclusion on the access to the Tx
 *                        descriptors list.
 *      freeCoreBuff    - Whether to free the tx core buffer,  this is true
 *                        whenever a packet is trasfered to the core with the
 *                        bufCopy on (parameter of coreGetTxPacket()).
 */
typedef struct
{
    STRUCT_SW_TX_DESC  *next2Free;
    STRUCT_SW_TX_DESC  *next2Feed;
    MV_U32              freeDescNum;
    MV_BOOL             freeCoreBuff;
    MV_U32              sizeOfList;
}TX_DESC_LIST;

/*
 * typedef: struct NET_CACHE_DMA_BLOCKS_INFO
 *
 * Description:
 *
 * Fields:
 *    devNum          - The device number to assign to this PP.
 *    txDescBlock     - Pointer to a block of host memory to be used
 *                       for allocating Tx packet descriptor structures.
 *    txDescBlockSize - The raw size in bytes of txDescBlock memory.
 *    rxDescBlock     - Pointer to a block memory to be used for
 *                       allocating Rx description structures.
 *    rxDescBlockSize - The raw size in byte of rxDescBlock.
 *    rxBufInfo       - Rx buffers allocation information.
 *    auDescBlock     - The block of memory used for the Address Update Queue.
 *                       The packet processor writes AU messages to this queue.
 *    auDescBlockSize - Size of auDescBlock (in Bytes).
 *
 *
 */
typedef struct
{
    MV_U8   devNum;
    MV_U32  *txDescBlock;
    MV_U32  *txDescBlockPhy;
    MV_U32  txDescBlockSize;
    MV_U32  txDescMemHandle;

    MV_U32  *rxDescBlock;
    MV_U32  *rxDescBlockPhy;
    MV_U32  rxDescBlockSize;
    MV_U32  rxDescMemHandle;

    MV_U32  *rxBufBlock;
    MV_U32  *rxBufBlockPhy;
    MV_U32  rxBufBlockSize;
    MV_U32  rxBufMemHandle;

    MV_U8   *auDescBlock;
    MV_U8   *auDescBlockSecond;
    MV_U32  *auDescBlockPhy;
    MV_U32  *auDescBlockSecondPhy;
    MV_U32  auDescBlockSize;
    MV_U32  auDescMemHandle;
    MV_U32  auDescMemSecondHandle;

    MV_U8   *fuDescBlock;
    MV_U8   *fuDescBlockSecond;
    MV_U32  *fuDescBlockPhy;
    MV_U32  *fuDescBlockSecondPhy;
    MV_U32  fuDescBlockSize;
    MV_U32  fuDescMemHandle;
    MV_U32  fuDescMemSecondHandle;

} NET_CACHE_DMA_BLOCKS_INFO;

/*
 * Typedef: struct STRUCT_RX_DESC
 *
 * Description: Includes the PP Rx descriptor fields, to be used for handling
 *              recieved packets.
 *
 * Fields:
 *      word1           - First word of the Rx Descriptor.
 *      word2           - Second word of the Rx Descriptor.
 *      buffPointer     - The physical data-buffer address.
 *      nextDescPointer - The physical address of the next Rx descriptor.
 *
 */
typedef struct _rxDesc
{
    volatile MV_U32         word1;
    volatile MV_U32         word2;

    volatile MV_U32         buffPointer;
    volatile MV_U32         nextDescPointer;

}STRUCT_RX_DESC;

/*
 * typedef: struct STRUCT_SW_TC_DESC
 *
 * Description: Sw management Tx descirptor.
 *
 * Fields:
 *      rxDesc          - Points to the Rx descriptor representing this Sw Rx
 *                        desc.
 *      swNextDesc      - A pointer to the next descriptor in the linked-list.
 *      buffVirtPointer - The virtual data-buffer address
 *      shadowRxDesc    - A shadow struct to hold the real descriptor data
 *                        after byte swapping to save non-cachable memory
 *                        access.
 *
 */
typedef struct _swRxDesc
{
    STRUCT_RX_DESC      *rxDesc;
    struct _swRxDesc    *swNextDesc;
    volatile MV_U32     buffVirtPointer;

    STRUCT_RX_DESC      shadowRxDesc;
} STRUCT_SW_RX_DESC;


/*
 * Typedef: struct RX_DESC_LIST
 *
 * Description: The control block of a single Rx descriptors list.
 *
 * Fields:
 *
 *      next2Return     - Points to the descriptor to which the next returned
 *                        buffer should be attached.
 *      next2Receive    - Points to the descriptor from which the next packet
 *                        will be fetched when an Rx interrupt is received.
 *                        (This actually points to the first desc. of the packet
 *                        in case of a multi desc. packet).
 *      rxFreeDescNum   - Number of free descriptors in list.
 *      rxListSem       - Semaphore for mutual exclusion on the access to the rx
 *                        descriptors list.
 *      headerOffset    - Number of reserved bytes before each buffer, to be
 *                        kept for application and internal use.
 *      forbidQEn       - When set to MV_TRUE enabling the Rx SDMA on buffer
 *                        release is forbidden.
 *
 */
typedef struct
{
    STRUCT_SW_RX_DESC  *next2Return;
    STRUCT_SW_RX_DESC  *next2Receive;
    MV_U32              rxFreeDescNum;
    MV_U32              rxResource;
    MV_BOOL             forbidQEn;
} RX_DESC_LIST;

/*
 * typedef: struct AU_DESC_CTRL
 *
 * Description: Address Update descriptors block control struct.
 *
 * Fields:
 *      blockAddr   - Address of the descs. block (Virtual Address)
 *      blockSize   - Size (in descs.) of the descs. block.
 *      currDescIdx - Index of the next desc. to handle.
 *      auCtrlSem   - Semaphore for mutual exclusion.
 *
 */
typedef struct
{
    MV_U32  blockAddr;
    MV_U32  blockAddrSecond;
    MV_U32  blockSize;
    MV_U32  currDescIdx;
} AU_DESC_CTRL;

/************* AUQ structures ********************************************/

/*
 * Typedef: struct AU_MESSAGE
 *
 * Description: Includes fields definitions of the Address Update messages sent
 *              to the CPU.
 *
 * Fields:
 */
typedef struct
{
    MV_U32  word0;
    MV_U32  word1;
    MV_U32  word2;
    MV_U32  word3;

}STRUCT_AU_DESC;

/*
 * Typedef: struct SDMA_INTERRUPT_CTRL
 *
 * Description: Includes all needed definitions for interrupts handling in core
 *              level. (Rx, Tx, Address Updates,...).
 *
 * Fields:
 *      rxDescList  - A list of Rx_Descriptor_List control structs, one for each
 *                    Rx queue.
 *      txDescList  - A list of Tx_Descriptor_List control structs, one for each
 *                    Tx queue.
 *      auDescCtrl  - Control block of the AU desc.
 *
 */
typedef struct
{
    RX_DESC_LIST    rxDescList[NUM_OF_RX_QUEUES];
    TX_DESC_LIST    txDescList[NUM_OF_TX_QUEUES];
    AU_DESC_CTRL    auDescCtrl;
    AU_DESC_CTRL    fuDescCtrl;

} SDMA_INTERRUPT_CTRL;

/*******************************************************************************
 * External usage environment parameters
 */
extern NET_CACHE_DMA_BLOCKS_INFO   netCacheDmaBlocks[SYS_CONF_MAX_DEV];
extern SDMA_INTERRUPT_CTRL         G_sdmaInterCtrl[SYS_CONF_MAX_DEV];

/*
 * typedef: enum MV_DESC_OWNERSHIP
 *
 * Description:
 *      Descriptor ownership values
 *
 * Fields:
 *      MV_OWNERSHIP_CPU - CPU ownership
 *      MV_OWNERSHIP_DMA - DMA ownership
 *
 * Comment:
 */
typedef enum
{
    MV_OWNERSHIP_CPU = 0,
    MV_OWNERSHIP_DMA = 1

}MV_DESC_OWNERSHIP;

/*
 * typedef: struct MV_RX_DESC
 *
 * Description:
 *      Contains the RX descriptor parameters
 *
 * Fields:
 *      os              - ownership bit
 *      busError        - bus error occured.
 *      enableInterrupt - enable interrupt
 *      resourceError   - Resource error
 *      first           - First buffer of the packet.
 *      last            - Last buffer of the packet.
 *      srcDev          - The device the packet was recieved on.
 *      srcPort         - The port the packet was recieved on.
 *      cpuCode         - CPU code.
 *      validCrc        - Valid or invalid CRC.
 *      byteCount       - Total byts count of the packet.
 *      bufferSize      - Current buffer size.
 *
 * Comment:
 */
typedef struct
{
    MV_DESC_OWNERSHIP os;
    MV_BOOL           busError;
    MV_BOOL           enableInterrupt;
    MV_BOOL           resourceError;
    MV_BOOL           first;
    MV_BOOL           last;
    MV_BOOL           validCrc;
    MV_U32            byteCount;
    MV_U32            bufferSize;

}MV_RX_DESC;


/*
 * typedef: struct MV_RX_DESC_DATA
 *
 * Description:
 *      Contains the over all data in the RX descriptor
 *
 * Fields:
 *      rxDesc       - RX descriptor
 *      intDev       - The device the interrupt was received on.
 *
 * Comment:
 */
typedef struct
{
    MV_RX_DESC     rxDesc;
    MV_U8          intDev;
    MV_U32         word1;
    MV_U32         word2;

}MV_RX_DESC_DATA;


/*
 * typedef: struct MV_TX_DESC
 *
 * Description:
 *      Contains the TX descriptor parameters
 *
 * Fields:
 *      os               - ownership bit
 *      autoMode         - Auto mode.
 *      enableInterrupt  - enable interrupt
 *      first            - First buffer of the packet.
 *      last             - Last buffer of the packet.
 *      recalcCrc        - Recalculate crc.
 *      byteCount        - Current buffer size.
 * Comment:
 */
typedef struct
{
    MV_DESC_OWNERSHIP os;
    MV_BOOL           autoMode;
    MV_BOOL           enableInterrupt;
    MV_BOOL           first;
    MV_BOOL           last;
    MV_BOOL           recalcCrc;
    MV_U32            byteCount;

}MV_TX_DESC;

/*
 * typedef: enum MV_TX_COOKIE_TYPE
 *
 * Description:
 *      Type of cookie placed in the descriptor
 *
 * Fields:
 *      COOKIE_MV_BUFF            - The cookie contains a pointer to the mvBuf.
 *      COOKIE_MV_BUFF_RX_2_TX    - The cookie contains a pointer to the
 *                                  mvBuf and the packet was sent because of
 *                                  rx2Tx mode.
 *      COOKIE_NO_MV_BUFF         - The packet was sent directly from the core.
 *                                  This means there is no mvBuf pointer.
 *      COOKIE_NO_MV_BUFF_RX_2_TX - The packet was sent directly from the core.
 *                                  this means there is no mvBuff.the packet was
 *                                  sent because of rx2Tx mode
 * Comment:
 */
typedef enum
{
    COOKIE_MV_BUFF             = 0,
    COOKIE_MV_BUFF_RX_2_TX     = 1,
    COOKIE_NO_MV_BUFF          = 2,
    COOKIE_NO_MV_BUFF_RX_2_TX  = 3

}MV_TX_COOKIE_TYPE;


/*
 * typedef: struct MV_TX_COOKIE
 *
 * Description:
 *
 * Fields:
 *      cookieType - cookieType
 *      mvBuf      - Pointer to the mvBuf. This parameter is valid only in
 *                   packet types COOKIE_MV_BUFF and COOKIE_MV_BUFF_RX_2_TX.
 *      tc         - The Rx queue in which the packet was received befor the.
 *                   rx 2 tx.
 *      port       - The port in which the packet was received befor the rx 2
 *                   tx.
 *      bufOwDevNum - device number of buffer owner (for Rx 2 Tx support on
 *                    Mini SV platform)
 *
 * Comment:
 */
typedef struct
{
    MV_TX_COOKIE_TYPE cookieType;
    MV_U8             **mvBuf;
    MV_U32            numOfBuf;
    MV_U8             tc;
    MV_U8             bufOwDevNum;

}MV_TX_COOKIE;

/*
 * typedef: struct MV_NET_RX_2_TX_PARAMS
 *
 * Description:
 *      Rx To TX parameters.
 *
 * Fields:
 *      valid    - Entry valid
 *      start    - MV_TRUE - enable RX to TX.
 *      outPort  - Out port.
 *      outTc    - Out queue.
 *      packetsCnt  - Number of received packets.
 *
 * Comment:
 */
typedef struct
{
    MV_BOOL  valid;
    MV_BOOL  start;
    MV_U8    outPort;
    MV_U8    outDev;
    MV_U8    outTc;
    MV_U32   packetsCnt;

}MV_NET_RX_2_TX_PARAMS;

typedef struct
{
    MV_U8 arEther[6];
} MV_ETHERADDR;

typedef struct
{
    MV_U8 arIP[4];
} MV_IPADDR;

typedef struct
{
    MV_U8 addr[16];
} MV_IPV6ADDR;

/********* Typedefs ***********************************************************/
/*
 * typedef: enum MV_NET_DIRECTION
 *
 * Description: transfer direction: RX or TX
 *
 * Enumerations:
 *       MV_NET_RX  - incomming packets.
 *       MV_NET_TX  - outgoing packets.
 */
typedef enum
{
    MV_NET_RX=0,
    MV_NET_TX
} MV_NET_DIRECTION;

/*
 * Typedef: MV_RX_CPU_CODE
 *
 * Description: Defines the different CPU codes that indicate the reason for
 *              sending a packet to the CPU.
 *
 * Fields:
 *
 *  BPDU TRAP               - BPDU packet trapped to the host CPU.
 *  FDB ENTEY TRAP/MIRROR   - Packet trapped or mirrored to the host CPU due
 *                            to FDB Entry command set to Trap or Mirror on
 *                            the packet's MAC SA or MAC DA.
 *  MAC RANGE TRAP          - 98DX240 Code for MAC Range Trapping.
 *  ARP BROADCAST TRAP/MIRROR - ARP Broadcast packet trapped or mirrored to the
 *                            host CPU.
 *  IPv4 IGMP TRAP/MIRROR   - IPv4 IGMP packet trapped or mirrored to the host
 *                            CPU.
 *  MAC ADDR INTERVENTION   - 98DX240 Code for FDB command set to intervention.
 *  LEARN DISABLE UNKNOWN   - Packet with unknown Source Address received on a
 *  SOURCE MAC ADDR TRAP      learning disabled port with learning disable
 *                            command set to 'Trap packets with unknown Source
 *                            Address'.
 *  MAC RANGE TRAP          - 98DX240 Code for MAC Range Mirroring
 *  LEARN DISABLE UNKNOWN   - Packet with unknown Source Address received on a
 *  SOURCE MAC ADDR MIRROR    learning disabled port with learning disable
 *                            command set to 'Mirror packets with unknown Source
 *                            Address'.
 *  BRIDGED UNKNOWN UNICAST - 98DX240 Code: Packet with unknown Unicast
 *  TRAP/MIRROR               Destination Address assigned to a VLAN with
 *                            Unknown Unicast Command set to Trap or Mirror.
 *  BRIDGED UNREGISTERED    - 8DX240 Code: packet with unknown Destination
 *  MULTICAST TRAP/MIRROR     Address assigned to a VLAN with Unknown Unicast
 *                            Command set to Trap or Mirror
 *  IEEE RESERVED MULTICAST - Packet with MAC DA in the IEEE Reserved Multicast
 *  ADDR TRAP/MIRROR          range trapped or mirrored to the host CPU.
 *  IPv6 ICMP TRAP/MIRROR   - IPv6 ICMP packet trapped or mirrored to the host.
 *  IPv4/IPv6 LINK-LOCAL    - IPv4 or IPv6 packets with DIP in the Link Local
 *  MULTICAST DIP TRAP/MIRROR Range trapped or mirrored to the host CPU.
 *  IPv4 RIPv1 MIRROR       - IPv4 RIPv1 packet mirrored to the host CPU.
 *  IPv6 NEIGHBOR SOLICITATION - IPv6 neighbor solicitation packet trapped or
 *  TRAP/MIRROR                  mirrored to the host CPU.
 *  IPv4 BROADCAST TRAP/MIRROR - IPv4 Broadcast packet assigned to a VLAN with
 *                               IPv4 Broadcast Command set to Trap or Mirror.
 *  NON IPv4 BROADCAST      - Non IPv4 Broadcast packet assigned to a VLAN with
 *  TRAP/MIRROR               non IPv4 Broadcast Command set to Trap or Mirror.
 *  CISCO CONTROL MULTICAST - Multicast packet with a MAC DA in the CISCO AUI
 *  MAC ADDR TRAP/MIRROR      range trapped or mirrored to the host CPU.
 *  BRIDGED NON IPv4/IPv6   - Non IPv4/IPv6 Unregistered Multicast packet
 *  UNREGISTERED              assigned to a VLAN with non IPv4/IPv6 Unregistered
 *  MULTICAST TRAP/MIRROR     Multicast Command set to Trap or Mirror.
 *  BRIDGED IPv4 UNREGISTERED - IPv4 Unregistered Multicast packet assigned to
 *  MULTICAST TRAP/MIRROR       a VLAN with IPv4 Unregistered Multicast Command
 *                              set to Trap or Mirror.
 *  BRIDGED IPv6 UNREGISTERED - IPv6 Unregistered Multicast packet assigned to
 *  MULTICAST TRAP/MIRROR       a VLAN with IPv6 Unregistered Multicast Command
 *                              set to Trap or Mirror.
 *  ROUTED PACKET FORWARD   - Packet forwarded to the host CPU by the Router.
 *  BRIDGED PACKET FORWARD  - Packet forwarded to the host CPU by one of the
 *                            following engines in the device:
 *                           . Redirected by the PCL engine to the CPU port.
 *                           . MAC DA is associated with the CPU port.
 *                           . Private VLAN edge target set to CPU port.
 *  INGRESS MIRRORED TO     - Ingress mirrored packets to the analyzer port,
 *  ANALYAER                  when the ingress analyzer port number is set to
 *                            the CPU port number.
 *  EGRESS MIRRORED TO      - Egress mirrored packets to the analyzer port,
 *  ANALYZER                  when the egress analyzer port number is set to the
 *                            CPU port number.
 *  MAIL FROM NEIGHBOR CPU  - A packet sent to the local host CPU as a mail from
 *                            the neighboring CPU.
 *  CPU TO CPU              - A packet sent to the local host CPU, from one of
 *                            the other host CPUs in the system.
 *  EGRESS SAMPLED          - Packet mirrored to the host CPU by the egress STC
 *                            mechanism.
 *  INGRESS SAMPLED         - Packet mirrored to the host CPU by the ingress STC
 *                            mechanism.
 *  INVALID USER DEFINED    - Packet trapped/mirrored to the host CPU by the
 *  SELECTED BYTES ON PCL     Policy Engine due to the following:
 *  KEY TRAP/MIRROR             User-defined bytes in the key could not be
 *                              parsed due to packet header length or its format.
 */
typedef enum
{
    RESERVED0                                       = 0,
    RESERVED1                                       = 1,
    BPDU_TRAP                                       = 2,
    FDB_ENTRY_TRAP_MIRROR                           = 3,
    MAC_RANGE_TRAP                                  = 4,
    ARP_BROADCAST_TRAP_MIRROR                       = 5,
    IPv4_IGMP_TRAP_MIRROR                           = 6,
    MAC_ADDR_INTERVENTION                           = 7,
    LEARN_DISABLE_UNKNOWN_SOURCE_MAC_ADDR_TRAP      = 8,
    MAC_RANGE_MIRROR                                = 9,
    LEARN_DISABLE_UNKNOWN_SOURCE_MAC_ADDR_MIRROR    = 10,
    BRIDGED_UNKNOWN_UNICAST_TRAP_MIRROR             = 11,
    BRIDGED_UNREGISTERED_MULTICAST_TRAP_MIRROR      = 12,
    IEEE_RESERVED_MULTICAST_ADDR_TRAP_MIRROR        = 13,
    IPv6_ICMP_TRAP_MIRROR                           = 14,
    RESERVED15,
    IPv4_IPv6_LINK_LOCAL_MULTICAST_DIP_TRAP_MIRROR  = 16,
    IPv4_RIPv1_MIRROR                               = 17,
    IPv6_NEIGHBOR_SOLICITATION_TRAP_MIRROR          = 18,
    IPv4_BROADCAST_TRAP_MIRROR                      = 19,
    NON_IPv4_BROADCAST_TRAP_MIRROR                  = 20,
    CISCO_CONTROL_MULTICAST_MAC_ADDR_TRAP_MIRROR    = 21,
    BRIDGED_NON_IPv4_IPv6_UNREGISTERED_MULTICAST_TRAP_MIRROR   = 22,
    BRIDGED_IPv4_UNREGISTERED_MULTICAST_TRAP_MIRROR = 23,
    BRIDGED_IPv6_UNREGISTERED_MULTICAST_TRAP_MIRROR = 24,
    UNKNOWN_UNICAST_TRAP_TO_CPU                     = 25,
    IEEE_RES_MC_ADDR_TRAP_MIRROR1                   = 26,
    IEEE_RES_MC_ADDR_TRAP_MIRROR2                   = 27,
    IEEE_RES_MC_ADDR_TRAP_MIRROR3                   = 28,
    IPV4_6_LINK_LOCAL_MC_ADDR_TRAP_MIRROR1          = 29,
    IPV4_6_LINK_LOCAL_MC_ADDR_TRAP_MIRROR2          = 30,
    IPV4_6_LINK_LOCAL_MC_ADDR_TRAP_MIRROR3          = 31,
    UDP_BC_TRAP_MIRROR_0                            = 32,
    UDP_BC_TRAP_MIRROR_1                            = 33,
    UDP_BC_TRAP_MIRROR_2                            = 34,
    UDP_BC_TRAP_MIRROR_3                            = 35,
    SEC_AUTO_LEARN_UNK_SRC_TRAP                     = 36,
    ROUTED_PACKET_FORWARD                           = 64,
    BRIDGED_PACKET_FORWARD                          = 65,
    INGRESS_MIRRORED_TO_ANALYZER                    = 66,
    EGRESS_MIRRORED_TO_ANALYZER                     = 67,
    MAIL_FROM_NEIGHBOR_CPU                          = 68,
    CPU_TO_CPU                                      = 69,
    EGRESS_SAMPLED                                  = 70,
    INGRESS_SAMPLED                                 = 71,
    INVALID_USER_DEF_SELECTED_BYTES_ON_PCL_KEY_TRAP_MIRROR = 74,
    IPV4_TT_HEADER_ERROR                            = 75,
    IPV4_TT_OPTION_FRAG_ERROR                       = 76,
    IPV4_TT_UNSUP_GRE_ERROR                         = 77,
    RESERVED_78                                     = 78,
    RESERVED_79                                     = 79,
    RESERVED_80                                     = 80,
    RESERVED_81                                     = 81,
    RESERVED_82                                     = 82,
    RESERVED_83                                     = 83,
    RESERVED_84                                     = 84,
    RESERVED_85                                     = 85,
    RESERVED_86                                     = 86,
    RESERVED_87                                     = 87,
    RESERVED_88                                     = 88,
    RESERVED_89                                     = 89,
    RESERVED_90                                     = 90,
    RESERVED_91                                     = 91,
    RESERVED_92                                     = 92,
    RESERVED_93                                     = 93,
    RESERVED_94                                     = 94,
    RESERVED_95                                     = 95,
    RESERVED_96                                     = 96,
    RESERVED_97                                     = 97,
    RESERVED_98                                     = 98,
    RESERVED_99                                     = 99,
    RESERVED_100                                    = 100,
    RESERVED_101                                    = 101,
    RESERVED_102                                    = 102,
    RESERVED_103                                    = 103,
    RESERVED_104                                    = 104,
    RESERVED_105                                    = 105,
    RESERVED_106                                    = 106,
    RESERVED_107                                    = 107,
    RESERVED_108                                    = 108,
    RESERVED_109                                    = 109,
    RESERVED_110                                    = 110,
    RESERVED_111                                    = 111,
    RESERVED_112                                    = 112,
    RESERVED_113                                    = 113,
    RESERVED_114                                    = 114,
    RESERVED_115                                    = 115,
    RESERVED_116                                    = 116,
    RESERVED_117                                    = 117,
    RESERVED_118                                    = 118,
    RESERVED_119                                    = 119,
    RESERVED_120                                    = 120,
    RESERVED_121                                    = 121,
    RESERVED_122                                    = 122,
    RESERVED_123                                    = 123,
    RESERVED_124                                    = 124,
    RESERVED_125                                    = 125,
    RESERVED_126                                    = 126,
    RESERVED_127                                    = 127,
    RESERVED_128                                    = 128,
    RESERVED_129                                    = 129,
    RESERVED_130                                    = 130,
    RESERVED_131                                    = 131,
    RESERVED_132                                    = 132,
    IPV4_TTL_EXCEEDED                               = 133,
    IPV4_6_MTU_EXCEEDED                             = 134,
    IPV6_HOPLIMIT_EXCEEDED                          = 135,
    IPV4_6_ILLEGAL_ADDR_ERROR                       = 136,
    IPV4_HEADER_ERROR                               = 137,
    IPV4_6_DIP_DA_MISMATCH                          = 138,
    IPV6_HEADER_ERROR                               = 139,
    IPV4_6_UC_SIP_SA_MISMATCH                       = 140,
    IPV4_OPTIONS                                    = 141,
    IPV6_NON_HBH_OPTION                             = 142,
    IPV6_HBH_OPTIONS                                = 143,
    RESERVED_144                                    = 144,
    RESERVED_145                                    = 145,
    RESERVED_146                                    = 146,
    RESERVED_147                                    = 147,
    RESERVED_148                                    = 148,
    RESERVED_149                                    = 149,
    RESERVED_150                                    = 150,
    RESERVED_151                                    = 151,
    RESERVED_152                                    = 152,
    RESERVED_153                                    = 153,
    RESERVED_154                                    = 154,
    RESERVED_155                                    = 155,
    RESERVED_156                                    = 156,
    RESERVED_157                                    = 157,
    RESERVED_158                                    = 158,
    IPV6_SCOPE                                      = 159,
    IPV4_UC_ROUTE_ENTRY_0                           = 160,
    IPV4_UC_ROUTE_ENTRY_1                           = 161,
    IPV4_UC_ROUTE_ENTRY_2                           = 162,
    IPV4_UC_ROUTE_ENTRY_3                           = 163,
    IPV4_MC_ROUTE_ENTRY_0                           = 164,
    IPV4_MC_ROUTE_ENTRY_1                           = 165,
    IPV4_MC_ROUTE_ENTRY_2                           = 166,
    IPV4_MC_ROUTE_ENTRY_3                           = 167,
    IPV6_UC_ROUTE_ENTRY_0                           = 168,
    IPV6_UC_ROUTE_ENTRY_1                           = 169,
    IPV6_UC_ROUTE_ENTRY_2                           = 170,
    IPV6_UC_ROUTE_ENTRY_3                           = 171,
    IPV6_MC_ROUTE_ENTRY_0                           = 172,
    IPV6_MC_ROUTE_ENTRY_1                           = 173,
    IPV6_MC_ROUTE_ENTRY_2                           = 174,
    IPV6_MC_ROUTE_ENTRY_3                           = 175,
    IPV4_6_UC_RPF_FAIL                              = 176,
    IPV4_6_MC_ROUTE_RPF_FAIL                        = 177,
    IPV4_6_MC_MLL_RPF_FAIL                          = 178,
    ARP_BC_TO_ME                                    = 179,
    IPV4_UC_ICMP_REDIRECT                           = 180,
    IPV6_UC_ICMP_REDIRECT                           = 181,
    RESERVED_182                                    = 182,
    RESERVED_183                                    = 183,
    RESERVED_184                                    = 184,
    RESERVED_185                                    = 185,
    RESERVED_186                                    = 186,
    RESERVED_187                                    = 187,
    ARP_REPLY_TO_ME                                 = 188,
    CPU_TO_ALL_CPUS                                 = 189,
    TCP_SYN_TO_CPU                                  = 190,
    PACKET_TO_VIRTUAL_ROUTER_PORT                   = 191,
    USER_DEFINED_192                                = 192,
    USER_DEFINED_193                                = 193,
    USER_DEFINED_194                                = 194,
    USER_DEFINED_195                                = 195,
    USER_DEFINED_196                                = 196,
    USER_DEFINED_197                                = 197,
    USER_DEFINED_198                                = 198,
    USER_DEFINED_199                                = 199,
    USER_DEFINED_200                                = 200,
    USER_DEFINED_201                                = 201,
    USER_DEFINED_202                                = 202,
    USER_DEFINED_203                                = 203,
    USER_DEFINED_204                                = 204,
    USER_DEFINED_205                                = 205,
    USER_DEFINED_206                                = 206,
    USER_DEFINED_207                                = 207,
    USER_DEFINED_208                                = 208,
    USER_DEFINED_209                                = 209,
    USER_DEFINED_210                                = 210,
    USER_DEFINED_211                                = 211,
    USER_DEFINED_212                                = 212,
    USER_DEFINED_213                                = 213,
    USER_DEFINED_214                                = 214,
    USER_DEFINED_215                                = 215,
    USER_DEFINED_216                                = 216,
    USER_DEFINED_217                                = 217,
    USER_DEFINED_218                                = 218,
    USER_DEFINED_219                                = 219,
    USER_DEFINED_220                                = 220,
    USER_DEFINED_221                                = 221,
    USER_DEFINED_222                                = 222,
    USER_DEFINED_223                                = 223,
    USER_DEFINED_224                                = 224,
    USER_DEFINED_225                                = 225,
    USER_DEFINED_226                                = 226,
    USER_DEFINED_227                                = 227,
    USER_DEFINED_228                                = 228,
    USER_DEFINED_229                                = 229,
    USER_DEFINED_230                                = 230,
    USER_DEFINED_231                                = 231,
    USER_DEFINED_232                                = 232,
    USER_DEFINED_233                                = 233,
    USER_DEFINED_234                                = 234,
    USER_DEFINED_235                                = 235,
    USER_DEFINED_236                                = 236,
    USER_DEFINED_237                                = 237,
    USER_DEFINED_238                                = 238,
    USER_DEFINED_239                                = 239,
    USER_DEFINED_240                                = 240,
    USER_DEFINED_241                                = 241,
    USER_DEFINED_242                                = 242,
    USER_DEFINED_243                                = 243,
    USER_DEFINED_244                                = 244,
    USER_DEFINED_245                                = 245,
    USER_DEFINED_246                                = 246,
    USER_DEFINED_247                                = 247,
    USER_DEFINED_248                                = 248,
    USER_DEFINED_249                                = 249,
    USER_DEFINED_250                                = 250,
    USER_DEFINED_251                                = 251,
    USER_DEFINED_252                                = 252,
    USER_DEFINED_253                                = 253,
    USER_DEFINED_254                                = 254,
    USER_DEFINED_255                                = 255,

    NUM_OF_SWITCH_CPU_CODES

} MV_RX_CPU_CODE;

typedef enum
{
    DSA_TO_CPU,
    DSA_FROM_CPU
} MV_DSA_TYPE;

typedef enum
{
    INGRESS_PIPE,
    EGRESS_PIPE
} MV_PIPE_TYPE;

/*
 * typedef: struct MV_MTAG_TO_CPU
 *
 * Description:
 *          Marvell (DSA) tag to CPU structure
 * Fields:
 *      TagCommand   - 0 - To CPU,
 *                     1 - From CPU
 *      SrcTagged    - 0 - Packet was received from a network port untagged.
 *                     1 - Packet was received from a network port tagged.
 *      SrcDev       - number of the Source Device on which the packet was
 *                     received.
 *      SrcPort      - number of the Source Port on which the packet was
 *                     received.
 *      CPUCode      - must be set to 0xF to indicate an Extended DSA tag.
 *      UP           - The 802.1p User Priority field assigned to the packet.
 *      VID          - The VID assigned to the packet.
 *      Extend       - This must be set to 0.
 *      CFI          - When SrcTagged = 1, this is the VLAN Tag CFI bit with
 *                          which the packet was received from the network port.
 *                     When SrcTagged = 0, the packet was received untagged and
 *                          the CFI bit is assigned by the device to '0'.
 *      Truncated    - Packet sent to CPU is truncated.
 *      flowId      - The packet Flow ID.
 *      SrcTrg       - indicates the type of data forwarded to the CPU.
 *                     0 - The packet was forwarded to the CPU by the Ingress
 *                         pipe and this tag contains the packets source info.
 *                     1 - The packet was forwarded to the CPU by the Egress
 *                         pipe and this tag contains the pckt destination info.
 *      LongCPUCode  - 8 bits CPU code
 */
typedef struct
{
    struct
    {
        MV_DSA_TYPE     tagCmd ;
        MV_BOOL         srcTagged;
        MV_U8           srcDev;
        MV_U8           srcPort;
        MV_U8           cpuCode;
        MV_U8           up;
        MV_U16          vid;
        MV_BOOL         extend;
        MV_U8           cfi;
        MV_BOOL         packetIsLooped;
        MV_BOOL         dropOnSource;
        MV_BOOL         isTrunk;
        MV_BOOL         truncated;
        MV_PIPE_TYPE    srcTrg;
        MV_U8           longCpuCode;
        MV_U16          pktOrigBc;
    } swDsa;
    struct
    {
        MV_U32 word0;
        MV_U32 word1;
    } hwDsa;
} MV_RX_DSA;

/*
 * typedef: struct MV_MTAG_FROM_CPU
 *
 * Description:
 *          Marvell (DSA) tag from CPU structure
 * Fields:
 *      TagCommand   - 0 - To CPU,
 *                     1 - From CPU
 *      TrgTagged    - 0 - Packet is sent via network port untagged.
 *                     1 - Packet is sent via network port tagged.
 *      mllPtr       - mll pointer
 *      TrgDev       - Target Device to which the packet is forwarded.
 *      TrgPort      - Target Port to which the packet is forwarded.
 *      VIDx         - vidx according to which the packet is to be forwarded.
 *      Use_VIDx     - 0 - Packet from the CPU is a Unicast packet forwarded to
 *                         a target specified in this tag.
 *                     1 - Packet from the CPU is a Multicast packet forwarded
 *                         to the VLAN-ID and Multicast group index in this tag.
 *      UP           - The 802.1p User Priority field assigned to the packet.
 *      TC           - The packet Traffic Class.
 *      Extend0      - Extended bit in word 0 must be set to 1.
 *      CFI          - When TrgTagged = 1, this is the VLAN Tag CFI bit with
 *                          which the packet was received from the network port.
 *                     When TrgTagged = 0, the packet was received untagged and
 *                          the CFI bit is assigned by the device to '0'.
 *      VID          - VID assigned to the packet.
 *      Extend1      - Extended bit in word 1 must be set to 0.
 *      EgressFltrEn - Egress filtering enable.
 *      CascadeCntr  - This bit indicates which TC is assigned to the packet
 *                     when it is forwarded over a cascading/stacking port.
 *                     0 - If the packet is queued on a port that is enabled for
 *                         Data QoS mapping (typically a cascade port), the
 *                         packet is queued according to the data traffic {TC,
 *                         DP} mapping table, which maps the DSA tag TC and DP
 *                         to a cascade port TC and DP. On a port that is not
 *                         enabled for Data QoS mapping (a non-cascade ports),
 *                         the packet is queued according to the DSA tag info.
 *                      1 - If the packet is queued on a port that is enabled
 *                          for Control QoS mapping (typically a cascade port),
 *                          the packet is queued according to the configured
 *                          Control TC and DP. On a port that is not enabled for
 *                          Control QoS mapping (non-cascade ports), the packet
 *                          is queued according to the DSA tag TC and DP.
 *      EgressFltrReg- Indicates that the packet is Egress filtered as a
 *                     registered packet.
 *      DP           - Packet's drop precedence.
 *      Src_ID       - Packet's Source ID.
 *      SrcDev       - Packet's Source Device Number.
 *      ExcIsTrunk   - A Trunk is excluded from the Multicast group.
 *      ExcTrunk     - The excluded trunk number.
 *      ExcPort      - Together with ExcludedDev, specifies the port that is
 *                     excluded from the Multicast group.
 *      ExcDev       - Together with ExcludedPort, specifies the port that is
 *                     excluded from the Multicast group.
 *      MailBoxToNeighborCPU - Mail box to Neighbor CPU, for CPU to CPU mail box
 *                             communication.
 */
typedef struct
{
    struct
    {
        MV_DSA_TYPE     tagCmd;
        MV_BOOL         trgTagged;
        MV_BOOL         useVidx;
        MV_U8           trgDev;
        MV_U8           trgPort;
        MV_U16          vidx;
        MV_U8           up;
        MV_U8           tc;
        MV_U16          vid;
        MV_BOOL         extend0;
        MV_BOOL         extend1;
        MV_U8           cfi;
        MV_BOOL         egressFltrEn;
        MV_BOOL         cascadeCntr;
        MV_BOOL         egressFltrReg;
        MV_BOOL         dropOnSrc;
        MV_BOOL         pktIsLooped;
        MV_U8           srcId;
        MV_U8           srcDev;
        MV_BOOL         excludeIsTrunk;
        MV_U8           excludeTrunk;
        MV_U8           excludePort;
        MV_U8           excludeDev;
        MV_BOOL         mirrorToAllCpus;
        MV_BOOL         mailBoxToNeigCpu;
    } swDsa;
    struct
    {
        MV_U32 word0;
        MV_U32 word1;
    } hwDsa;

} MV_TX_DSA;

/*******************************************************************************
* coreSetTxDesc
*
* DESCRIPTION:
*       Set the TX descriptor parameters
*
* INPUTS:
*       devNum     - Device number.
*       queue      - Queue number.
*       descNum    - Descriptor number. The first descriptor is number 1.
*       txDesc     - TX descriptor parameters.
*       bufferPtr  - Buffer pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       MV_OK        - Operation succeede
*       MV_BAD_PARAM - Illegal parameter in function called
*
* COMMENTS:
*
*
*******************************************************************************/
MV_STATUS coreSetTxDesc(MV_U8         devNum,
                        MV_U8         queue,
                        MV_U32        descNum,
                        MV_TX_DESC   *txDesc,
                        MV_U8        *bufferP);

/*******************************************************************************
* mvDbRxDmaSetMode
*
* DESCRIPTION:
*       Enable/Disable Rx on specified TX DMA Queue
*
* INPUTS:
*        devNum    - device number
*        queue     - tx queue
*        enable    - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_OK   - on success
*       MV_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command - netQueueEnable
*
*******************************************************************************/
MV_STATUS mvDbRxDmaSetMode(MV_U8    devNum,
                           MV_U8    rxQueue,
                           MV_BOOL  enable);

/*******************************************************************************
* mvDbTxDmaSetMode
*
* DESCRIPTION:
*       Enable/Disable Tx on specified TX DMA Queue
*
* INPUTS:
*        devNum    - device number
*        queue     - tx queue
*        enable    - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_OK   - on success
*       MV_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command - netQueueEnable
*
*******************************************************************************/
MV_STATUS mvDbTxDmaSetMode(MV_U8    devNum,
                           MV_U8    txQueue,
                           MV_BOOL  enable);

/*******************************************************************************
* coreTxSetAm
*
* DESCRIPTION:
*       Set all the descriptors for a specific TC to AM
*
* INPUTS:
*       devNum - Device number.
*       queue  - Queue number.
*       am     - MV_TRUE - Auto mode
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       MV_OK        - Operation succeede
*       MV_BAD_PARAM - Illegal parameter in function called
*
* COMMENTS:
*
*
*******************************************************************************/
MV_STATUS coreTxSetAm(MV_U8    devNum,
                      MV_U8    queue,
                      MV_BOOL  am);

/*
 * Typedef: enum MV_TX_CMD
 *
 * Description: Enumeration of Transmit command Types
 *
 * Enumerations:
 *      TX_BY_VLAN  - Send packets using "netSendPktByVid" function and
 *                    defines to use spesific parameters of this function.
 *      TX_BY_LPORT - Send packets using "netSendPktByLport" function and
 *                    defines to use spesific parameters of this function.
 */
typedef enum
{
    TX_BY_VLAN = 0,
    TX_BY_LPORT
} MV_TX_CMD;

/*
 * Typedef: struct MV_PKT_DESC
 *
 * Description: Packet Description structure
 *
 *      entryID         - Entry ID number for Delete/Get/ChangeAllign operations
 *      appendCrc       - (MV_TRUE) Add Crc to the transmitted packet,
 *                        (MV_FALSE) Leave the packet as is.
 *      tagged          - does the packet already include tag.
 *      pcktsNum        - Number of packets to send.
 *      gap             - The time is calculated in multiples of 64 clock cycles
 *                        Valid values are 0 (no delay between packets to
 *                        CPU), through 0x3FFE (1,048,544 Clk cycles).
 *                        0x3FFF - has same effect as 0.
 *      waitTime        - The wait time before moving to the next entry.
 *      pcktData        - Array of pointers to packet binary data.
 *      pcktDataLen     - Array of lengths of pcktData pointers.
 *      pcktDataAllign  - Alignments of buffers for pcktData pointers.
 *      numSentPackets  - Number of sent packets.
 *      cmdType         - Defines type of transmition (VLAN, If, LPort).
 *      cmdParams       - Defines fields for transmition type (VLAN, If, LPort)
 *
 */
typedef struct
{
    MV_U32          entryId;
    MV_BOOL         appendCrc;
    MV_U32          pcktsNum;
    MV_U32          gap;
    MV_U32          waitTime;
    MV_U8          *pcktData[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN];
    MV_U8          *pcktPhyData[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN];
    MV_U32          pcktDataLen[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN];
    MV_U8           pcktDataAlign[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN];
    MV_U32          numSentPackets;
    MV_TX_DSA       mrvlTag;
    MV_TX_CMD       cmdType;
} MV_PKT_DESC;

#define MACTBL_MESSAGE_SIZE 4

/******************************************************************************/
/*
 * typedef: enum MAC_TBL_UPDATE_MSG_TYPE
 *
 * Description:
 *      Update message type
 *
 * Fields:
 *      UPDATE_MSG_NA -
 *      UPDATE_MSG_QA -
 *      UPDATE_MSG_QR -
 *      UPDATE_MSG_AA -
 *      UPDATE_MSG_TA -
 *
 * Comment:
 */
typedef enum
{
    UPDATE_MSG_NA = 0,
    UPDATE_MSG_QA = 1,
    UPDATE_MSG_QR = 2,
    UPDATE_MSG_AA = 3,
    UPDATE_MSG_TA = 4,
    UPDATE_MSG_FU = 5,

    UPDATE_MSG_LAST

} MAC_TBL_UPDATE_MSG_TYPE;

/******************************************************************************/
/*
 * typedef: enum MAC_TBL_UPDATE_ENTRY_TYPE
 *
 * Description:
 *      Update message entry type
 *
 * Fields:
 *      UPDATE_MSG_ENTRY_MAC  -
 *      UPDATE_MSG_ENTRY_IPV4 -
 *      UPDATE_MSG_ENTRY_IPV6 -
 *
 * Comment:
 */
typedef enum
{
    UPDATE_MSG_ENTRY_MAC = 0,
    UPDATE_MSG_ENTRY_IPV4 = 1,
    UPDATE_MSG_ENTRY_IPV6 = 2,
    UPDATE_MSG_ENTRY_LAST

} MAC_TBL_UPDATE_ENTRY_TYPE;

/*
 * Typedef struct: MAC_TBL_HW_ENTRY
 *
 * Description:
 *      Define HW entry in the MAC TABLE
 *
 * Fields:
 *
 *   macAddr     -
 *   vid         -
 *   trunk       -
 *   aging       -
 *   skip        -
 *   valid       -
 *   rxSniff     -
 *   saCmd       -
 *   daCmd       -
 *   forceL3Cos  -
 *   multiple    -
 *   isStatic    -
 *   daPrio      -
 *   saPrio      -
 *   vidx        -
 *   portTrnk    -
 *
 */
typedef struct
{
    MV_ETHERADDR                    macAddr;
    MV_IPADDR                       dipAddr;
    MV_IPADDR                       sipAddr;
    MV_U32                          vidx;
    MV_U32                          portNum;
    MV_U32                          trunkNum;
    MV_BOOL                         trunk;
    MV_BOOL                         multiple;
    MV_BOOL                         spUnknown;
    MV_BOOL                         age;
    MV_BOOL                         skip;
    MV_U32                          vid;
    MV_BOOL                         mirrorToAnalyzer;
    MV_U32                          saCmd;
    MV_U32                          daCmd;
    MAC_TBL_UPDATE_ENTRY_TYPE       entryType;
    MV_BOOL                         isStatic;
    MV_U32                          daQosProfileIndex;
    MV_U32                          saQosProfileIndex;
    MV_U32                          ownDevNum;
    MV_U32                          srcId;
    MV_BOOL                         valid;
    MV_U32                          saSecurityLevel;
    MV_U32                          daSecurityLevel;
    MV_BOOL                         appSpecCpuCodeEn;
    MV_U32                          userDefined;
    MV_IPV6ADDR                     ipv6Sip;
    MV_IPV6ADDR                     ipv6Dip;

}MAC_TBL_HW_ENTRY;


/*
 * typedef: struct MAC_TBL_UPDATE_MSG
 *
 * Description:
 *      Defines the update message format.
 *
 * Fields:
 *      msgType      - message type.
 *      entryFound   - Valid Only for query reply message (QR) from the
 *                     98DX240/160 to the Host CPU and for Index query reply
 *                     message (QI) from the 98DX240/160 to the Host CPU and
 *                     MV_TRUE  - Entry was found.
 *                     MV_FALSE - Entry was not found.
 *      ownDevNum    - The 98DX240/160 Device number.
 *      macAddrIndex - If entryFound = MV_TRUE, this fields contains the entry
 *                      Index in the FDB.
 *      macEntryData - Other mac entry data.
 *
 * Comment:
 */
typedef struct
{
    MV_BOOL                     entryFound;
    MV_U32                      macAddrOffset;
    MV_U32                      macAddrIndex;
    MAC_TBL_UPDATE_MSG_TYPE     msgType;
    MV_U32                      msgId;
    MV_BOOL                     chainTooLong;
    MV_BOOL                     daRoute;
    MV_BOOL                     searchType;
    MAC_TBL_HW_ENTRY            macEntryData;

}MAC_TBL_UPDATE_MSG;

/************* Functions Prototype ********************************************/


/*******************************************************************************
* mvBrgTeachNewAddress
*
* DESCRIPTION:
*       Building the message for learning the cheetah new address on controlled
*       learning mode.
*
* INPUTS:
*       devNum      - Device number
*       updateMsg   - an update message
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
MV_STATUS mvBrgTeachNewAddress(MV_U8                    devNum,
                               MAC_TBL_UPDATE_MSG      *updateMsg);

/*******************************************************************************
* setCPUAddressInMACTAble
*
* DESCRIPTION:
*
* INPUTS:
*       devId           - Device Id
*       macAddr		- MAC address
*	vid		- VLAN ID
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK            - on success.
*       CMD_AGENT_ERROR   - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
MV_STATUS setCPUAddressInMACTAble(MV_U8    devNum,
                                  MV_U8   *macAddr,
                                  MV_U32   vid);

/*******************************************************************************
* mvSwitchTxStart
*
* DESCRIPTION:
*       Starts transmition of packets
*
* INPUTS:
*       packetDesc - pointer to packet desciptor struct
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MV_OK   - on success
*       MV_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command   - cmdTxStart
*
*******************************************************************************/
MV_STATUS mvSwitchTxStart(MV_PKT_DESC *packetDesc);

MV_STATUS mvSwitchBuildPacket(MV_U8         devNum,
                              MV_U32        portNum,
                              MV_U32        entryId,
                              MV_U8         appendCrc,
                              MV_U32        pktsNum,
                              MV_U8         gap,
                              MV_U8        *pktData,
                              MV_U32        pktSize,
                              MV_PKT_DESC  *netTxPacketDesc);

MV_STATUS mvSwitchInjectExtDsa   (MV_GND_PKT_INFO     *pktInfoP);
MV_STATUS mvSwitchExtractExtDsa  (MV_GND_PKT_INFO *pktInfoP);
MV_STATUS mvSwitchRememberExtDsa (MV_GND_PKT_INFO *pktInfoP);
MV_STATUS mvSwitchRemoveExtDsa   (MV_GND_PKT_INFO     *pktInfoP);
MV_U32    mvSwitchGetPortNumFromExtDsa(MV_GND_PKT_INFO *pktInfoP);
MV_STATUS mvSwitchBuildPacketZeroCopy(MV_U8         devNum,
                                      MV_U32        portNum,
                                      MV_U32        entryId,
                                      MV_U8         appendCrc,
                                      MV_U32        pktsNum,
                                      MV_U8         gap,
                                      MV_U8        *pktData,
                                      MV_U32        pktSize,
                                      MV_PKT_DESC  *pktDesc);

/*******************************************************************************
* mvSwitchDoTx
*
* DESCRIPTION:
*       This function receives packet buffers & parameters and prepares them
*       for the transmit operation, and
*       enqueues the prepared descriptors to the PP's tx queues.
*
* INPUTS:
*       devNum      - Device number
*       txQ     - TX Q ID
*       recalcCrc   - Whether to calculate CRC.
*       buffList    - The packet data buffers list.
*       phyBuffList - The packet physical data buffers list.
*       buffLen     - A list of the buffers len in buffList.
*       numOfBufs   - Length of buffList.
*       getTxSem    - Whether to get the Tx semaphore or not.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_OK on success, or
*       MV_NO_RESOURCE if there is not enough desc. for enqueuing the packet, or
*       MV_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
MV_STATUS mvSwitchDoTx(MV_U8     devNum,
                       MV_U8     txQ,
                       MV_BOOL   recalcCrc,
                       MV_U8    *buffList[],
                       MV_U8    *phyBuffList[],
                       MV_U32   *buffLen,
                       MV_U32    numOfBufs);

/*******************************************************************************
* mvSwitchIsTxDone
*
* DESCRIPTION:
*       Tests if the packet transmission is finished.
*
* INPUTS:
*       devNum      - The device number from which the packet was sent.
*       txQ     - The Tx queue index from which the packet was sent.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_TRUE if TX operation is finished.
*       MV_FALSE otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MV_BOOL mvSwitchIsTxDone(MV_U8 devNum, MV_U8 txQ);

/*******************************************************************************
* mvSwitchTxDone
*
* DESCRIPTION:
*       This function resets TX descriptor and updates the
*       TX descriptors linked list.
*
* INPUTS:
*       devNum      - The device number from which the packet was sent.
*       txQ     - The Tx queue index from which the packet was sent.
*       descNum     - Number of descriptors (buffers) this packet occupies.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_OK if successful, or
*       MV_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MV_STATUS mvSwitchTxDone(MV_U8 devNum, MV_U8 txQ, MV_U32 descNum);

/*******************************************************************************
* mvFreeRxBuf
*
* DESCRIPTION:
*       Frees a list of buffers, that where previously passed to the upper layer
*       in an Rx event.
*
* INPUTS:
*       rxBuffList  - List of Rx buffers to be freed.
*       buffListLen - Length of rxBufList.
*       devNum      - The device number throw which these buffers where
*                     received.
*       rxQueue     - The Rx queue number throw which these buffers where
*                     received.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_OK on success, or
*       MV_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MV_STATUS mvFreeRxBuf(MV_U8  *virtBuffP,
                      MV_U32  physBuffP,
                      MV_U8   devNum,
                      MV_U8   rxQueue);

/*******************************************************************************
* mvInitSdmaNetIfDev
*
* DESCRIPTION:
*       Initialize the network interface structures, Rx descriptors & buffers
*       and Tx descriptors (For a single device).
*
* INPUTS:
*       devNum  - The device to initialize.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_OK   - on success,
*       MV_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MV_STATUS mvInitSdmaNetIfDev(MV_U8       devNum,
                             MV_U32      auDescNum,
                             MV_U32      txDescNum,
                             MV_U32      rxDescNum,
                             MV_U32      rxBufSize);

/*******************************************************************************
* mvFreeSdmaNetIfDev
*
* DESCRIPTION:
*       Free the network interface structures, Rx descriptors & buffers
*       and Tx descriptors (For a single device).
*
* INPUTS:
*       devNum  - The device to initialize.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_OK   - on success,
*       MV_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MV_STATUS mvFreeSdmaNetIfDev(MV_U8 devNum);

/*******************************************************************************
* mvPpSetSdmaCurrTxDesc
*
* DESCRIPTION:
*       Configure/set Transmit SDMA Current Descriptor pointer.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_TRUE     - success.
*       MV_FALSE    - failure.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MV_STATUS mvPpSetSdmaCurrTxDesc(MV_U8 dev, MV_U32 txQ, MV_U32 txDescP);

/*******************************************************************************
* mvPpInitRxDescRing
*
* DESCRIPTION:
*       Initialized RX Descriptor Ring.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_TRUE     - if xCat2 package type is BGA.
*       MV_FALSE    - if xCat2 package type is QFP.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MV_STATUS mvPpInitRxDescRing(MV_U8    devNum,
                             MV_U8   *descBlockP,
                             MV_U32   descBlockSize,
                             MV_U32   numOfDescs);

/*******************************************************************************
* mvPpInitTxDescRing
*
* DESCRIPTION:
*       Initialized TX Descriptor Ring.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_TRUE     - success.
*       MV_FALSE    - failure.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MV_STATUS mvPpInitTxDescRing(MV_U8    dev,
                             MV_U8   *descBlockP,
                             MV_U32   descBlockSize,
                             MV_U32   numOfDescs);

/*******************************************************************************
* mvNetEnableRxProcess
*
* DESCRIPTION:
*   Enable the RX process.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       MV_OK        - Operation succeede
*
* COMMENTS:
*
*
*******************************************************************************/
MV_STATUS mvNetEnableRxProcess(MV_VOID);

/*******************************************************************************
* mvNetGetEnableRxProcess
*
* DESCRIPTION:
*   Get the status of Enable the RX process.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       MV_TRUE  - Enable RX process
*       MV_FALSE - Disable RX process
*
* RETURNS :
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
MV_BOOL mvNetGetEnableRxProcess(MV_VOID);

#define hwByteSwap(x)                   MV_32BIT_LE_FAST(x)
#define hwIfGetReg                      mvSwitchReadReg
#define hwIfSetReg                      mvSwitchWriteReg
#define hwIfSetMaskReg                  mvSwitchReadModWriteReg

#ifdef PRESTERA_CACHED
static INLINE void PP_DESCR_FLUSH_INV(void *osHandle, void *descP)
{
    mvOsCacheLineFlushInv(osHandle, (MV_U32)(descP));
}

static INLINE void PP_DESCR_INV(void *osHandle, void *descP)
{
    mvOsCacheLineInv(osHandle, (MV_U32)(descP));
}

static INLINE void PP_PKT_CACHE_FLUSH(void *buffP, MV_U32 size)
{
    mvOsCacheClear(NULL, buffP, size);
}

static INLINE void PP_CACHE_INV(void *buffP, MV_U32 size)
{
    mvOsCacheInvalidate(NULL, buffP, size);
}
#else
    #define PP_DESCR_FLUSH_INV(pPortCtrl, pDescr)
    #define PP_DESCR_INV(pPortCtrl, pDescr)
    #define PP_PKT_CACHE_FLUSH(pAddr, size)
    #define PP_CACHE_INV(pAddr, size)
#endif

/*
 * Typedef: struct STRUCT_VLAN_ENTRY
 *
 * Description:
 *    This structure contains VLAN enrty data (4 words).
 *
 * Fields:
 *    VLTData   - array of that contains VLAN entry data
 */
typedef struct
{
    volatile MV_U32 VLTData[4];
} STRUCT_VLAN_ENTRY;

void *mvPpMalloc(MV_U32     descSize,
                 MV_U32    *pPhysAddr,
                 MV_U32    *memHandle);

void  mvPpFree  (MV_U32     size,
                 MV_U32    *phyAddr,
                 void      *pVirtAddr,
                 MV_U32    *memHandle);

MV_STATUS mvSwitchInitWA(MV_VOID);

#define DEFAULT_REGION	2

#define PP_DEV_PORT_NUM		24 /* number of ports per device */
#define PRESTERA_DEFAULT_DEV		0
#define PRESTERA_SECOND_DEV         1

#define PP_DEV0              0
#define PP_DEV1              1
#define MV_PP_DEV0              0
#define MV_PP_DEV1              1
#define PP_DEVS_NUM          2

#if defined (MV_XCAT_INTERPOSER)
#define MV_PP_DEV_BASE_TBL    { \
    MV_SWITCH_PEX_BASE,\
    MV_SWITCH_XBAR_BASE \
}
#else
#define MV_PP_DEV_BASE_TBL    { \
    PP_DEV0_BASE,\
    PP_DEV1_BASE \
}
#endif

/************ Macros **********************************************************/

#define PORT_TO_DEV(port)			port/PP_DEV_PORT_NUM
#define PORT_TO_DEV_PORT(port)			port%PP_DEV_PORT_NUM

/************ Declarations **********************************************************/
MV_STATUS mvPpWriteLogActivate(void);
MV_STATUS mvPpWriteLogPause(void);
MV_VOID   mvPpInitDevCfgInfo(void);
MV_STATUS mvPpHalInitHw(void);
MV_STATUS mvPpHalInit(void);
MV_VOID   mvPpWriteReg(MV_U8 devNum, MV_U32 regAddr, MV_U32 value);
MV_STATUS mvSwitchWriteReg(MV_U8 devNum, MV_U32 regAddr, MV_U32 value);
MV_STATUS mvSwitchReadReg (MV_U8 devNum, MV_U32 regAddr, MV_U32 *pValue);
MV_U32    mvPpReadReg(MV_U8 devNum, MV_U32 regAddr);
MV_U32    mvPpReadRegDev0(MV_U32 regAddr);
MV_STATUS mvSwitchReadModWriteReg(MV_U8 devNum, MV_U32 regAddr, MV_U32 mask, MV_U32 value);
MV_STATUS mvSwitchBitSet(MV_U8 devNum, MV_U32 regAddr, MV_U32 bit);
MV_VOID   mvPpBitSet(MV_U8 devNum, MV_U32 regAddr, MV_U32 bitMask);
MV_STATUS mvSwitchBitReset(MV_U8 devNum, MV_U32 regAddr, MV_U32 bit);
MV_VOID   mvPpBitReset(MV_U8 devNum, MV_U32 regAddr, MV_U32 bit);

MV_U8     mvPpDevNumGet(void);
MV_U8     mvSwitchGetDevicesNum(void);
MV_STATUS mvPpTwoBytePrependDisable(void);
MV_U32    mvSwitchGetPortsNum(void);
MV_U32    mvPpNetPortsNumGet(void);
MV_STATUS mvSwitchCpuPortConfig(MV_U8 devNum);
MV_STATUS mvSwitchOpenMemWinAsKirkwood(MV_U8 devNum);
MV_STATUS mvPpSwitchInit(MV_U32 dev);
MV_STATUS mvPpSetCpuPortToCascadeMode(MV_U32 dev);
MV_STATUS mvSwitchMruSetCpuPort(MV_U32 mruBytes);
MV_STATUS mvSwitchMruSet(MV_U32 dev, MV_U32 port, MV_U32 mruBytes);
MV_STATUS mvSwitchMruSetAllPorts(MV_U32 mruBytes);
void mvSwitchMruPrintAllPorts(void);
MV_BOOL   mvPpIsTwoBytesPrepended(MV_U32 dev);
void mvSwitchEgressFilterCfg(MV_VOID);
void mvSwitchVidxCfg(MV_VOID);
void mvPpCascadePortCfg(MV_VOID);
void mvSwitchRxIntEnable(MV_U32 dev);
void mvSwitchTxIntEnable(MV_U32 dev);
void mvSwitchRxIntDisable(MV_U32 dev);
void mvSwitchTxIntDisable(MV_U32 dev);
MV_VOID mvPpRxReadyIntMask(MV_U32 rxQBitMask);
MV_VOID mvPpTxDoneIntMask(MV_U32 txQBitMask);
MV_VOID mvPpRxReadyIntUnmask(MV_U32 rxQBitMask);
MV_VOID mvPpTxDoneIntUnmask(MV_U32 txQBitMask);
MV_VOID mvPpRxReadyIntAck(MV_U32 ackBitMask);
MV_VOID mvPpTxDoneIntAck(MV_U32 ackBitMask);
MV_U32 mvPpRxReadyQGet(MV_VOID);
MV_U32 mvPpTxDoneQGet(MV_VOID);
MV_STATUS mvPpRxDone(MV_PKT_INFO *pktInfoP, MV_U32 rxQ);
MV_PKT_INFO *mvPpPortRx(MV_U32 rxQ);
MV_PKT_INFO *mvPpPortTxDone(MV_U32 txQ);
MV_STATUS mvPpPortTx(MV_PKT_INFO *pktInfoP, MV_U32 txQ);
MV_BOOL mvPpIsTxDone(MV_U32 txQ);
MV_U32 mvPpRxResourceGet(MV_U32 rxQ);
MV_STATUS mvPpEthInit(MV_ETH_INIT *ethInitP);
MV_STATUS mvPpPortEnable(MV_VOID);
MV_STATUS mvPpMacAddrSet(MV_U8 *macAddr, MV_U32 queue);
void      mvPpValidMacEntryPrint(void);
MV_STATUS setCpuAsVLANMember(int devNum, int vlanNum);
void mvPresteraReadPortMibCounters(int port);
MV_BOOL mvSwitchCheckPortStatus(int devNum, int portNum);
MV_BOOL mvSwitchIsAnyLinkUp(void);
MV_U32  mvSwitchFirstPortLinkUpGet(void);
MV_VOID mvEFuseConfig(MV_VOID);
MV_BOOL mvPpChipIsXCat2(void);
MV_BOOL mvPpChipIsXCat(void);
MV_BOOL mvPpChipIsXCat2Simple(void);
MV_U32  mvPpGetDevId(void);
MV_U32  mvPpGetChipRev(void);
MV_U32  mvPpGetXcat2ChipRev(void);
MV_BOOL mvPpIsChipFE(void);
MV_BOOL mvPpIsChipGE(void);
void    mvPpFtdllWaXcat2(void);
void    mvPpMaskAllInts(void);
MV_BOOL mvPpIsRxReadyIntPending(void);
MV_BOOL mvPpIsTxDoneIntPending(void);

#endif /* __INCmvPresterah */

