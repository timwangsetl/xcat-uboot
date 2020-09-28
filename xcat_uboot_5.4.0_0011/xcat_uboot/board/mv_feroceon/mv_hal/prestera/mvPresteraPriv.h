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

#ifndef __INCmvPresteraPrivh
#define __INCmvPresteraPrivh

#include "mvTypes.h"

/******************************************************************************
 * Prestera Unit Registers
 */
#define PRESTERA_SDMA_CFG_REG                       0x2800
#define PRESTERA_SDMA_RX_CURR_DESC_REG(rxQ)         (0x260C + rxQ * 4)
#define PRESTERA_SDMA_TX_CURR_DESC_REG(txQ)         (0x26C0 + txQ * 4)
#define PRESTERA_SDMA_RX_STATUS_REG                 0x281C
#define PRESTERA_SDMA_RX_PKTS_REG                   0x2820
#define PRESTERA_SDMA_RX_BYTES_REG                  0x2840

/*
 * CPU Port Configuration Register and MIB Counters
 */
#define PRESTERA_CPU_PORT_GLOBAL_CFG_REG            0xA0

#define PRESTERA_PORT_STATUS_REG                    0x0A800010

#define CASCADE_AND_HEADER_CONFIG_REG               0x0F000004
#define PREPEND_TWO_BYTES_HEADER_BIT                BIT_28
#define CPU_PORT_DSA_EN_BIT                         BIT_31

#define SWITCH_RX_SDMA_INT_BIT                      20
#define SWITCH_RX_SDMA_INT_MASK                 (0x1 << SWITCH_RX_SDMA_INT_BIT)

#define SWITCH_TX_SDMA_INT_BIT                      19
#define SWITCH_TX_SDMA_INT_MASK                 (0x1 << SWITCH_TX_SDMA_INT_BIT)

/******************************************************************************
 * Interrupts defines.
 */
#define SWITCH_GLOBAL_CAUSE_REG                     0x30
#define SWITCH_GLOBAL_MASK_REG                      0x34

#define SWITCH_SDMA_RX_CAUSE_REG                    0x280c
#define SWITCH_SDMA_RX_MASK_REG                     0x2814

#define SWITCH_SDMA_TX_CAUSE_REG                    0x2810
#define SWITCH_SDMA_TX_MASK_REG                     0x2818

#define SWITCH_RX_BUFFER_QUEUE_0_BIT                2
#define SWITCH_RX_QUEUE_MASK            (0xFF << SWITCH_RX_BUFFER_QUEUE_0_BIT)

#define SWITCH_TX_BUFFER_QUEUE_0_BIT                1
#define SWITCH_TX_QUEUE_MASK            (0xFF << SWITCH_TX_BUFFER_QUEUE_0_BIT)

/******************************************************************************
 * Window 0 control
 */
#define PRESTERA_ADDR_DECODE_WIN_BASE               0x30C
#define PRESTERA_ADDR_DECODE_WIN_SIZE               0x310
#define PRESTERA_ADDR_DECODE_WIN_ENABLE             0x34C

/******************************************************************************
 * CPU Port Registers
 */
#define PRESTERA_CPU_PORT_GLOBAL_CFG_REG            0xA0
#define PRESTERA_CPU_PORT_MAC_CTRL_REG(num) (0xA80FC00 + ((num==3) ? 0x48 : 4*num))
#define PP_CPU_PORT_MAC_CTRL_REG (0xA80FC00)

/*******************************************************************************
 * Tri-speed Port Registers (regular switch ports)
 */
#define PP_PORT_MAC_CTRL_REG0(port)         (0x0A800000 + port * 0x400)

/******************************************************************************
 * MIB Counters register definitions
 */
#define PRESTERA_MIB_REG_BASE(port)                 (0x04010000 +           \
                                                    ((port / 6) * 0x800000) + \
                                                    ((port % 6) * 0x80))

#define PRESTERA_MIB_GOOD_OCTETS_RECEIVED_LOW       0x0
#define PRESTERA_MIB_GOOD_OCTETS_RECEIVED_HIGH      0x4
#define PRESTERA_MIB_BAD_OCTETS_RECEIVED            0x8
#define PRESTERA_MIB_TX_FIFO_UNDERRUN_AND_CRC       0xc
#define PRESTERA_MIB_GOOD_FRAMES_RECEIVED           0x10
#define PRESTERA_MIB_SENT_DEFERRED                  0x14
#define PRESTERA_MIB_BROADCAST_FRAMES_RECEIVED      0x18
#define PRESTERA_MIB_MULTICAST_FRAMES_RECEIVED      0x1c
#define PRESTERA_MIB_FRAMES_64_OCTETS               0x20
#define PRESTERA_MIB_FRAMES_65_TO_127_OCTETS        0x24
#define PRESTERA_MIB_FRAMES_128_TO_255_OCTETS       0x28
#define PRESTERA_MIB_FRAMES_256_TO_511_OCTETS       0x2c
#define PRESTERA_MIB_FRAMES_512_TO_1023_OCTETS      0x30
#define PRESTERA_MIB_FRAMES_1024_TO_MAX_OCTETS      0x34
#define PRESTERA_MIB_GOOD_OCTETS_SENT_LOW           0x38
#define PRESTERA_MIB_GOOD_OCTETS_SENT_HIGH          0x3c
#define PRESTERA_MIB_GOOD_FRAMES_SENT               0x40
#define PRESTERA_MIB_EXCESSIVE_COLLISION            0x44
#define PRESTERA_MIB_MULTICAST_FRAMES_SENT          0x48
#define PRESTERA_MIB_BROADCAST_FRAMES_SENT          0x4c
#define PRESTERA_MIB_SENT_MULTIPLE                  0x50
#define PRESTERA_MIB_FC_SENT                        0x54
#define PRESTERA_MIB_GOOD_FC_RECEIVED               0x58
#define PRESTERA_MIB_RECEIVED_FIFO_OVERRUN          0x5c
#define PRESTERA_MIB_UNDERSIZE_RECEIVED             0x60
#define PRESTERA_MIB_FRAGMENTS_RECEIVED             0x64
#define PRESTERA_MIB_OVERSIZE_RECEIVED              0x68
#define PRESTERA_MIB_JABBER_RECEIVED                0x6c
#define PRESTERA_MIB_RX_ERROR_FRAME_RECEIVED        0x70

/*******************************************************************************
 * MG - Address Decoding registers
 */
#define PRESTERA_AD_BASE_ADDR(index)                (0x30C + (index*8))
#define PRESTERA_AD_SIZE(index)                     (0x310 + (index*8))
#define PRESTERA_AD_BARE                            0x34C

#define PRESTERA_AD_BARE_BITS_SET(value, index)    value &= ~(1<<index)
#define PRESTERA_AD_BASE_ADDR_BITS_SET(base, attr, target) ((base & 0xFFFF0000) | (attr << 8) | target)
#define PRESTERA_AD_DRAM_TARGET_ID 0

/*******************************************************************************
 * BITs of Ethernet Port Status reg (PSR)
 */
#define PRESTERA_LINK_UP_BIT                     1
#define PRESTERA_LINK_UP_MASK                    (1<<PRESTERA_LINK_UP_BIT)

#define PRESTERA_FULL_DUPLEX_BIT                 2
#define PRESTERA_FULL_DUPLEX_MASK                (1<<PRESTERA_FULL_DUPLEX_BIT)

#define PRESTERA_ENABLE_RCV_FLOW_CTRL_BIT        3
#define PRESTERA_ENABLE_RCV_FLOW_CTRL_MASK       (1<<PRESTERA_ENABLE_RCV_FLOW_CTRL_BIT)

#define PRESTERA_GMII_SPEED_1000_BIT             4
#define PRESTERA_GMII_SPEED_1000_MASK            (1<<PRESTERA_GMII_SPEED_1000_BIT)

#define PRESTERA_MII_SPEED_100_BIT               5
#define PRESTERA_MII_SPEED_100_MASK              (1<<PRESTERA_MII_SPEED_100_BIT)

#define PRESTERA_TX_IN_PROGRESS_BIT              7
#define PRESTERA_TX_IN_PROGRESS_MASK             (1<<PRESTERA_TX_IN_PROGRESS_BIT)

#define PRESTERA_TX_FIFO_EMPTY_BIT               10
#define PRESTERA_TX_FIFO_EMPTY_MASK              (1<<PRESTERA_TX_FIFO_EMPTY_BIT)

/* read bit # offset from 32 bits data. Return 0 or 1 */
#define U32_GET_BIT(data,offset) (((data) & (1 << (offset))) >> (offset))

/* write 1 to bit # "offset" in 32 bits data */
#define U32_SET_BIT(data,offset) ((data) |= (1 << (offset)))

/* write 0 to bit # "offset" in 32 bits data */
#define U32_UNSET_BIT(data,offset) ((data) &= (~(1 << (offset))))

#define MV_HW_MAC_LOW32(macAddr)                \
           (MV_U32)((macAddr)->arEther[5] |          \
                ((macAddr)->arEther[4] << 8) |    \
                ((macAddr)->arEther[3] << 16) |   \
                ((macAddr)->arEther[2] << 24))

#define MV_HW_MAC_HIGH16(macAddr)           \
        (MV_U32)((macAddr)->arEther[1] | ((macAddr)->arEther[0] << 8))

#define MV_HW_MAC_LOW16(macAddr)            \
        (MV_U32)((macAddr)->arEther[5] | ((macAddr)->arEther[4] << 8))

#define MV_HW_MAC_HIGH32(macAddr)               \
                (MV_U32)((macAddr)->arEther[3] |          \
                ((macAddr)->arEther[2] << 8) |    \
                ((macAddr)->arEther[1] << 16) |   \
                ((macAddr)->arEther[0] << 24))

#define MV_SYNC

/*******************************************************************************
 * Internal usage environment parameters
 */

MV_U32 mvSwitchFromCpuMultiTargetDsaTagWord0(void);
MV_U32 mvSwitchFromCpuMultiTargetDsaTagWord1(void);
MV_U32 calcFromCpuDsaTagWord0(MV_U8 devNum, MV_U32 portNum);
MV_U32 calcFromCpuDsaTagWord1(MV_U32 portNum);

MV_STATUS mvReceivePacket(MV_U8                 devNum,
                          MV_U8                 rxQ,
                          STRUCT_SW_RX_DESC    *swRxDesc,
                          MV_U32                descNum,
                          MV_PKT_INFO          *pktInfo);

MV_VOID createMacAddr(MV_ETHERADDR  *dest, MV_U8 *source);

MV_VOID mvStrToMac(MV_8 *source, MV_8 *dest);
MV_U32 mvStrToHex(MV_8 ch);

#endif /* __INCmvPresteraPrivh */

