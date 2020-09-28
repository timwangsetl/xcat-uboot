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

#ifndef __INCmvSysHwConfigh
#define __INCmvSysHwConfigh

#include <config.h>

/*******************************************************************************
 * xCat Boards
 */
#if defined(MV98DX)
#define MV_PRESTERA_SWITCH
#define MV_INCLUDE_PP
#define MV_INCLUDE_DRAGONITE
#undef MV_INCLUDE_SATA
#define PRESTERA_CASCADE_STACK_PORTS
#define PRESTERA_NO_DSA_TAG /* used only in CPU port configuration */
                            /* not used in SDMA configuration 	   */
#endif

/* defines  */
#ifdef MV_INCLUDE_DRAGONITE
#define DRAGONITE_DTCM_BASE 0xF2000000
/*
 * Actually DTCM size is 32K, but minimal hardware resolution is 64K
 */
#define DRAGONITE_DTCM_SIZE _64K
#endif

/* System targers address window definitions (base)                */
#define SDRAM_CS0_BASE  0x00000000      /* Actual base set be auto detection */
#define SDRAM_CS1_BASE  0x10000000      /* Actual base set be auto detection */
#define SDRAM_CS2_BASE  0x20000000      /* Actual base set be auto detection */
#define SDRAM_CS3_BASE  0x30000000      /* Actual base set be auto detection */
/* System targers address window definitions (size) */
#define SDRAM_CS0_SIZE  _256M
#define SDRAM_CS1_SIZE  _256M
#define SDRAM_CS2_SIZE  _256M
#define SDRAM_CS3_SIZE  _256M

#define CONFIG_MARVELL

/*******************************************************************************
 * Soc supporetd Units definitions
 */
#define MV_INCLUDE_PEX
#define MV_INCLUDE_GIG_ETH
#define MV_INCLUDE_CESA
#define MV_INCLUDE_USB
#define MV_INCLUDE_TWSI
#define MV_INCLUDE_UART
#define MV_INCLUDE_SPI
#define MV_INCLUDE_XOR
#define MV_INCLUDE_CLK_PWR_CNTRL
#define MV_INC_BOARD_SPI_FLASH
#define MV_INC_BOARD_NAND_FLASH

/* Disable the DEVICE BAR in the PEX */
#define MV_DISABLE_PEX_DEVICE_BAR

#define PCI0_IF_PTP		0		/* no Bridge on pciIf0 */
#define PCI1_IF_PTP		0		/* no Bridge on pciIf1 */

#define PEX0_MEM_BASE           0x90000000
#define PEX0_IO_BASE            0xf0000000
#define PEX0_MEM_SIZE           _256M
#define PEX0_IO_SIZE            _16M
#define CRYPT_ENG_BASE          0xFB000000
#define CRYPT_ENG_SIZE          _64K

/*******************************************************************************
 * U-Boot Specific
 */
#define MV_INCLUDE_MONT_EXT

#if defined(MV_INCLUDE_MONT_EXT)
#define MV_INCLUDE_MONT_MMU
#define MV_INCLUDE_MONT_MPU
#if defined(MV_INC_BOARD_NOR_FLASH)
#define MV_INCLUDE_MONT_FFS
#endif
#endif

/*******************************************************************************
 * RD boards specifics
 */
#undef MV_INC_BOARD_DDIM

#ifndef MV_BOOTROM
#define MV_STATIC_DRAM_ON_BOARD
#endif

#ifdef MV_INCLUDE_PP
#define PP_DEV0_BASE            0xf4000000
#define PP_DEV1_BASE            PEX0_MEM_BASE + _64M
#define PP_DEV0_SIZE            _64M
#define PP_DEV1_SIZE            _64M
#endif

#define DEVICE_CS0_BASE         0xf9000000
#define DEVICE_CS0_SIZE         _8M

#define DEVICE_SPI_BASE         0xf8000000
#define DEVICE_SPI_SIZE         _16M
#define DEVICE_CS1_BASE         DEVICE_SPI_BASE
#define DEVICE_CS1_SIZE         _16M

#define DEVICE_CS2_BASE         0xf4000000
#define DEVICE_CS2_SIZE         _1M

#define DEVICE_CS3_BASE         BOOTDEV_CS_BASE
#define DEVICE_CS3_SIZE         BOOTDEV_CS_SIZE

#if !defined(MV_BOOTROM) && defined(MV_NAND_BOOT)
#define CONFIG_SYS_NAND_BASE    BOOTDEV_CS_BASE
#define CFG_NAND_BASE           BOOTDEV_CS_BASE
#else
#define CONFIG_SYS_NAND_BASE    DEVICE_CS0_BASE
#define CFG_NAND_BASE           DEVICE_CS0_BASE
#endif
#define CONFIG_SYS_NAND_SIZE   _8M

#define DRAGONITE_DTCM_BASE     0xF2000000
#define DRAGONITE_DTCM_SIZE     _64K

#if defined (MV_INCLUDE_PEX)
#define PCI_IF0_MEM0_BASE       PEX0_MEM_BASE
#define PCI_IF0_MEM0_SIZE       PEX0_MEM_SIZE
#define PCI_IF0_IO_BASE         PEX0_IO_BASE
#define PCI_IF0_IO_SIZE         PEX0_IO_SIZE
#endif

#define MV_CPU_IF_ADDR_WIN_MAP_TBL	{				\
/*     base low      base high     size       	   WinNum     enable */ \
    {{SDRAM_CS0_BASE ,    0,      SDRAM_CS0_SIZE} ,0xFFFFFFFF,DIS}, \
    {{SDRAM_CS1_BASE ,    0,      SDRAM_CS1_SIZE} ,0xFFFFFFFF,DIS}, \
    {{SDRAM_CS2_BASE ,    0,      SDRAM_CS2_SIZE} ,0xFFFFFFFF,DIS}, \
    {{SDRAM_CS3_BASE ,    0,      SDRAM_CS3_SIZE} ,0xFFFFFFFF,DIS}, \
    {{PEX0_MEM_BASE  ,    0,      PEX0_MEM_SIZE } ,0x0       ,EN},	\
    {{PEX0_IO_BASE   ,    0,      PEX0_IO_SIZE  } ,0x2       ,EN},	\
    {{INTER_REGS_BASE,    0,      INTER_REGS_SIZE},0x8       ,EN},	\
    {{DEVICE_CS0_BASE,    0,      DEVICE_CS0_SIZE},0x5	 ,EN},	\
    {{DEVICE_CS1_BASE,    0,      DEVICE_CS1_SIZE},0x3	 ,EN},	\
    {{DEVICE_CS2_BASE,    0,      DEVICE_CS2_SIZE},0x5	 ,EN},	\
    {{DEVICE_CS3_BASE,    0,      DEVICE_CS3_SIZE},0x4       ,EN},	\
    {{CRYPT_ENG_BASE,     0,      CRYPT_ENG_SIZE} ,0x7  	 ,EN},	\
    {{PP_DEV0_BASE,       0,      PP_DEV0_SIZE}   ,0x6  	 ,EN},	\
    {{DRAGONITE_DTCM_BASE,0,      DRAGONITE_DTCM_SIZE}  ,0x1 ,EN},	\
    /* Table terminator */\
    {{TBL_TERM, TBL_TERM, TBL_TERM}, TBL_TERM,TBL_TERM}			\
};

#define PCI_ARBITER_CTRL    /* Use/unuse the Marvell integrated PCI arbiter */
#undef  PCI_ARBITER_BOARD   /* Use/unuse the PCI arbiter on board           */

/* Check macro validity */
#if defined(PCI_ARBITER_CTRL) && defined (PCI_ARBITER_BOARD)
    #error "Please select either integrated PCI arbiter or board arbiter"
#endif

/* We use the following registers to store DRAM interface pre configuration   */
/* auto-detection results													  */
/* IMPORTANT: We are using mask register for that purpose. Before writing     */
/* to units mask register, make sure main maks register is set to disable     */
/* all interrupts.                                                            */
#define DRAM_BUF_REG0   0x30810 /* sdram bank 0 size            */
#define DRAM_BUF_REG1   0x30820 /* sdram config                 */
#define DRAM_BUF_REG2   0x30830 /* sdram mode                   */
#define DRAM_BUF_REG3   0x308c4 /* dunit control low            */
#define DRAM_BUF_REG4   0x60a90 /* sdram address control        */
#define DRAM_BUF_REG5   0x60a94 /* sdram timing control low     */
#define DRAM_BUF_REG6   0x60a98 /* sdram timing control high    */
#define DRAM_BUF_REG7   0x60a9c /* sdram ODT control low        */
#define DRAM_BUF_REG8   0x60b90 /* sdram ODT control high       */
#define DRAM_BUF_REG9   0x60b94 /* sdram Dunit ODT control      */
#define DRAM_BUF_REG10  0x60b98 /* sdram Extended Mode          */
#define DRAM_BUF_REG11  0x60b9c /* sdram Ddr2 Time Low Reg      */
#define DRAM_BUF_REG12  0x60a00 /* sdram Ddr2 Time High Reg     */
#define DRAM_BUF_REG13  0x60a04 /* dunit Ctrl High              */
#define DRAM_BUF_REG14  0x60b00 /* sdram second DIMM exist      */

/************* Ethernet driver configuration ********************/

/*#define ETH_JUMBO_SUPPORT*/
/* HW cache coherency configuration */
#define DMA_RAM_COHER       NO_COHERENCY
#define DRAM_COHERENCY      MV_CACHE_COHER_SW
#define ETHER_DRAM_COHER    DRAM_COHERENCY
#define INTEG_SRAM_COHER    MV_UNCACHED  /* Where integrated SRAM available */

/*******************************************************************************
 * CESA configuration.
 */
#ifdef MV_INCLUDE_CESA
#define MV_CESA_MAX_CHAN               1
/* Use 2K of SRAM */
#define MV_CESA_MAX_BUF_SIZE           1600
#endif /* MV_INCLUDE_CESA */

/*******************************************************************************
 * Network configuration.
 */
#define MV_GBE_PORT0            0
#define MV_GBE_PORT1            1

#define CONFIG_ETH_MULTI_Q

/*
 * Multi-queue support
 */
#ifdef  CONFIG_ETH_MULTI_Q
    #define INCLUDE_MULTI_QUEUE
#else
    #undef  INCLUDE_MULTI_QUEUE
#endif

#define SWITCH_MTU                  1528
/* General macros for reading/writing from/to specified locations */
#define SWITCH_RX_BUF_DEF_SIZE      (SWITCH_MTU + 36)

/*
 * Generic Network Driver (GND) configurations.
 */
#define MV_GND_POLL_COUNT_MAX_DEFAULT     500
#define MV_GND_POLL_TIMOUT_USEC_DEFAULT   10
#define MV_GND_BUFF_ALIGN_DEFAULT         8
#define MV_GND_BUFF_MIN_SIZE_DEFAULT      8

/*
 * Generic Network Driver (GND) configurations.
 */
#define PP_DRV_POLL_COUNT_MAX             50
#define PP_DRV_POLL_TIMOUT_MSEC           10
#define PP_DRV_MAX_BUF_PER_PKT            10
#define PP_DRV_DEF_RX_Q                   MII_DEF_RXQ
#define PP_DRV_DEF_TX_Q                   MII_DEF_TXQ
#define PP_DRV_RX_BUFF_SIZE_DEFAULT       MII_RX_BUFF_SIZE_DEFAULT
#define PP_DRV_MRU_DEFAULT                MII_MRU_DEFAULT
#define PRESTERA_CACHED
#undef  MV_PP_SILENT_EEPROM_SIM

#undef CONFIG_ETH_MULTI_Q

#ifdef CONFIG_ETH_MULTI_Q
    #define MV_NET_NUM_OF_RX_Q                (8)
    #define MV_NET_NUM_OF_TX_Q                (8)
    #define MV_NET_NUM_OF_RX_DESC_PER_Q       (128)
    #define MV_NET_NUM_OF_TX_DESC_PER_Q       (128)
#else
    #define MV_NET_NUM_OF_RX_Q                (1)
    #define MV_NET_NUM_OF_TX_Q                (1)
    #define MV_NET_NUM_OF_RX_DESC_PER_Q       (8)
    #define MV_NET_NUM_OF_TX_DESC_PER_Q       (8)
#endif

#define MV_NET_NUM_OF_RX_DESC_TOTAL (MV_NET_NUM_OF_RX_Q*MV_NET_NUM_OF_RX_DESC_PER_Q)
#define MV_NET_NUM_OF_TX_DESC_TOTAL (MV_NET_NUM_OF_TX_Q*MV_NET_NUM_OF_TX_DESC_PER_Q)

#define NUM_OF_RX_QUEUES                  MV_NET_NUM_OF_RX_Q
#define NUM_OF_TX_QUEUES                  MV_NET_NUM_OF_TX_Q

/* rings length */
#define PRESTERA_RXQ_LEN                  MV_NET_NUM_OF_RX_DESC_PER_Q
#define PRESTERA_TXQ_LEN                  MV_NET_NUM_OF_TX_DESC_PER_Q

#define PP_NUM_OF_RX_DESC_PER_Q           (PRESTERA_RXQ_LEN)
#define PP_NUM_OF_RX_DESC_TOTAL           (PRESTERA_RXQ_LEN * NUM_OF_RX_QUEUES)
#define PP_NUM_OF_TX_DESC_PER_Q           (PRESTERA_TXQ_LEN)
#define PP_NUM_OF_TX_DESC_TOTAL           (PRESTERA_TXQ_LEN * NUM_OF_TX_QUEUES)

/*
 * Default MRU and RGMII-1 buffer size
 * (needed for Scatter-Gather support)
 * (1536 + 2 + 8 + 4) explanation:
 * +2 : 2 bytes prepended by CPU RGMII
 * +8 : extended DSA tag
 * +4 : CRC
 */
#define MII_RX_BUFF_SIZE_DEFAULT   (1536 + 2 + 8 + 4)
#define MII_RX_BUFF_SIZE_MIN       (8    + 2 + 8 + 4)
#define MII_RX_BUFF_SIZE_MAX       (_64KB - _1KB)

#define MII_MRU_DEFAULT            (1536 + 2 + 8 + 4)
#define MII_MRU_MIN                (1518 + 2 + 8 + 4)
#define MII_MRU_MAX                (9700 + 2 + 8 + 4)

/*
 * Scatter-Gather support
 */
#undef  ETH_RX_SCATTER_GATHER
#define ETH_TX_SCATTER_GATHER

#ifdef ETH_RX_SCATTER_GATHER
    #define MII_RX_BUFF_SIZE        (128  + 2 + 8 + 4)
    #define MII_MRU                 9700 /* max size supported by CPU RMGII */
#else
    #define MII_RX_BUFF_SIZE        MII_RX_BUFF_SIZE_DEFAULT
    #define MII_MRU                 MII_MRU_DEFAULT
#endif

/* un-comment if you want to perform tx_done from within the poll function */
/* #define ETH_TX_DONE_ISR */

/* put descriptors in uncached memory */
/* #define ETH_DESCR_UNCACHED */

/* Descriptors location: DRAM/internal-SRAM */
#define ETH_DESCR_IN_SDRAM
#undef  ETH_DESCR_IN_SRAM

#if (ETHER_DRAM_COHER == MV_CACHE_COHER_HW_WB)
#   define ETH_SDRAM_CONFIG_STR      "MV_CACHE_COHER_HW_WB"
#elif (ETHER_DRAM_COHER == MV_CACHE_COHER_HW_WT)
#   define ETH_SDRAM_CONFIG_STR      "MV_CACHE_COHER_HW_WT"
#elif (ETHER_DRAM_COHER == MV_CACHE_COHER_SW)
#   define ETH_SDRAM_CONFIG_STR      "MV_CACHE_COHER_SW"
#elif (ETHER_DRAM_COHER == MV_UNCACHED)
#   define ETH_SDRAM_CONFIG_STR      "MV_UNCACHED"
#else
#   error "Unexpected ETHER_DRAM_COHER value"
#endif /* ETHER_DRAM_COHER */

#define MV_CACHEABLE(address) ((address) | 0x80000000)

#if defined(MV_BOOTSIZE_256K)

#define BOOTDEV_CS_SIZE _256K

#elif defined(MV_BOOTSIZE_512K)

#define BOOTDEV_CS_SIZE _512K

#elif defined(MV_BOOTSIZE_4M)

#define BOOTDEV_CS_SIZE _4M

#elif defined(MV_BOOTSIZE_8M)

#define BOOTDEV_CS_SIZE _8M

#elif defined(MV_BOOTSIZE_16M)

#define BOOTDEV_CS_SIZE _16M

#elif defined(MV_BOOTSIZE_32M)

#define BOOTDEV_CS_SIZE _32M

#elif defined(MV_BOOTSIZE_64M)

#define BOOTDEV_CS_SIZE _64M

#elif defined(MV_mvOsPrintf_BOOT)

#define BOOTDEV_CS_SIZE _512K

#else

#define Error MV_BOOTSIZE undefined

#endif

#define BOOTDEV_CS_BASE	((0xFFFFFFFF - BOOTDEV_CS_SIZE) + 1)

/* Following the pre-configuration registers default values restored after    */
/* auto-detection is done                                                     */
#define DRAM_BUF_REG_DV    	0

#define ETH_DEF_TXQ    		0
#define ETH_DEF_RXQ    		0
#define MII_DEF_TXQ         0
#define MII_DEF_RXQ         0
#define MV_ETH_TX_Q_NUM		    1
#define MV_ETH_RX_Q_NUM		    1
#define MII_RX_Q_NUM            1
#define MII_TX_Q_NUM            1

/* Internal registers: size is defined in Controllerenvironment */
#define INTER_REGS_BASE    0xF1000000

/* DRAM detection stuff */
#define MV_DRAM_AUTO_SIZE

/* Board clock detection */
#define TCLK_AUTO_DETECT   /* Use Tclk auto detection       */
#define SYSCLK_AUTO_DETECT /* Use SysClk auto detection     */
#define PCLCK_AUTO_DETECT  /* Use PClk auto detection       */
#define L2CLK_AUTO_DETECT  /* Use L2 Clk auto detection     */

/*
 * Interrupt stuff
 */

/* PEX interrupt levels */
#define INT_LVL_PEX0_INTA                   65
#define INT_LVL_PEX0_INTB                   66
#define INT_LVL_PEX0_INTC                   67
#define INT_LVL_PEX0_INTD                   68

#define INT_LVL_PCI                         69

/* e.g. PEX1 int C = 76  |  PEX0 int C = 67 */
#define INT_LVL_PEX(pexIf, pin)	    (INT_LVL_PEX0_INTA - 1 + pin)

/*
 * Switch uses GPP interrupt number 49,
 * which is interrupt 17 of group two of GPP interrupts
 */
#define SWITCH_INT_BIT          49 /* should not be used independently */
#define INT_LVL_SWITCH_GPP_INT      (INT_LVL_PCI + SWITCH_INT_BIT)

#define INT_LVL_GBE_PORT_RX(port)   (INT_LVL_GBE0_RX + (4 * (port)))
#define INT_LVL_GBE_PORT_TX(port)   (INT_LVL_GBE0_TX + (4 * (port)))
#define INT_LVL_GBE_PORT_MISC(port) (INT_LVL_GBE0_MISC+ (4 * (port)))

/*
 * Main Interrupt Cause Register bits
 */
#define INT_LVL_HIGHSUM             0	/* Summary of Main Interrupt High Cause register*/
#define INT_LVL_BRIDGE              1	/* Mbus-L to Mbus Bridge interrupt 	*/
#define INT_LVL_DB_IN               2	/* Summary of Inbound Doorbell Cause register  	*/
#define INT_LVL_DB_OUT              3	/* Summary of Outbound Doorbell Cause register 	*/
#define INT_LVL_x4   		    4	/* 4 reserved	*/
#define INT_LVL_XOR0_CHAN0          5	/* Xor 0 Channel0 interrupt */
#define INT_LVL_XOR0_CHAN1          6	/* Xor 0 Channel1 interrupt */
#define INT_LVL_XOR1_CHAN0          7	/* Xor 1 Channel0 interrupt */
#define INT_LVL_XOR1_CHAN1          8	/* Xor 1 Channel1 interrupt */
#define INT_LVL_PEX00_ABCD	    9	/* PCI Express port0.0 INTA/B/C/D assert message interrupt. */
#define INT_LVL_x10   		    10	/* 10 reserved	*/
#define INT_LVL_GBE0_SUM   	    11	/* Gigabit Ethernet Port 0.0 summary	 	*/
#define INT_LVL_GBE0_RX		    12	/* Gigabit Ethernet Port 0.0 Rx summary    	*/
#define INT_LVL_GBE0_TX             13	/* Gigabit Ethernet Port 0.0 Tx summary    	*/
#define INT_LVL_GBE0_MISC           14	/* Gigabit Ethernet Port 0.0 misc. summary 	*/
#define INT_LVL_GBE1_SUM   	    15	/* Gigabit Ethernet Port 0.1 summary	 	*/
#define INT_LVL_GBE1_RX		    16	/* Gigabit Ethernet Port 0.1 Rx summary    	*/
#define INT_LVL_GBE1_TX             17	/* Gigabit Ethernet Port 0.1 Tx summary    	*/
#define INT_LVL_GBE1_MISC           18	/* Gigabit Ethernet Port 0.1 misc. summary 	*/
#define INT_LVL_USB0                19	/* USB0 interrupt   */
#define INT_LVL_x20   		    20	/* 20 reserved	*/
#define INT_LVL_SATA_CTRL  	    21	/* SATA controller interrupt     	*/
#define INT_LVL_CESA	   	    22	/* Security engine completion interrupt	*/
#define INT_LVL_SPI                 23	/* SPI interrupt	*/
#define INT_LVL_AUDIO               24	/* Audio interrupt	*/
#define INT_LVL_x25   		    25	/* 25 reserved	*/
#define INT_LVL_TS0                 26	/* TS0 interrupt	*/
#define INT_LVL_x27   		    27	/* 27 reserved	*/
#define INT_LVL_DSIO	   		28	/* SDIO interrupt	*/
#define INT_LVL_TWSI  			29	/* TWSI interrupt  */
#define INT_LVL_AVB  			30	/* AVB interrupt  */
#define INT_LVL_TDM  			31	/* TDM interrupt  */
#define INT_LVL_x32   		    32	/* 32 reserved	*/
#define INT_LVL_UART0   	    33 	/* UART0 interrupt  */
#define INT_LVL_UART1   	    34 	/* UART1 interrupt  */
#define INT_LVL_GPIOLO0_7	    35	/* GPIO LOW [00:07] Interrupt */
#define INT_LVL_GPIOLO8_15	    36	/* GPIO LOW [O8:15] Interrupt */
#define INT_LVL_GPIOLO16_23     37	/* GPIO LOW [16:23] Interrupt */
#define INT_LVL_GPIOLO24_31     38	/* GPIO LOW [24:31] Interrupt */
#define INT_LVL_GPIOHI0_7	    39	/* GPIO HIGH [00:07] Interrupt */
#define INT_LVL_GPIOHI8_15	    40	/* GPIO HIGH [O8:15] Interrupt */
#define INT_LVL_GPIOHI16_23     41	/* GPIO HIGH [16:23] Interrupt */
#define INT_LVL_XOR0_ERR	    42	/* XOR0 engine error 	 				 */
#define INT_LVL_XOR1_ERR	    43	/* XOR1 engine error 	 				 */
#define INT_LVL_PEX0_ERR	    44	/* PEX0 engine error 	 				 */
#define INT_LVL_x45   		    45	/* 45 reserved	*/
#define INT_LVL_GBE0_ERR	    46	/* GbE port0 error */
#define INT_LVL_GBE1_ERR	    47	/* GbE port1 error */
#define INT_LVL_USB_ERR	        48	/* USB error (address decoding) Summary of errors from all USB ports */
#define INT_LVL_SECURITY_ERR	49  /* Security engine completion error	*/
#define INT_LVL_AUDIO_ERR	   	50	/* Audio error	*/
#define INT_LVL_x51   		    51	/* 51 reserved	*/
#define INT_LVL_x52   		    52	/* 52 reserved	*/
#define INT_LVL_RTC   		    53	/* RTC interrupt */
#define INT_LVL_DRAGONITE       54	/* Dragonite interrupt */
#define INT_LVL_x55             55  /* 55-63 reserved */
#define INT_LVL_XCAT2_SWITCH    INT_LVL_x55 /* Switch MG interrupt */

/*
 * FIQ interrupts support.
 *     Note: These defines enables to route the interrupts of both timers
 *     to chip FIQ. Uncomment to use it.
 */
/* #define MV_SYS_TIMER_THROUGH_GPIO */
#define MV_SYS_TIMER_THROUGH_GPIO_NUM    35

#define MV_PP_HAL_WRITE_LOG_ENABLE
#define MV_DEV_WRITE_LOG_ENABLE
#define MV_DEV_WRITE_LOG_EARLY

#define MV_WRITE_LOG_NUM_OF_INSTANCE      2
#define MV_WRITE_LOG_INSTANCE_LEN         1000
#define MV_WRITE_LOG_LEN_TOTAL            MV_WRITE_LOG_INSTANCE_LEN * MV_WRITE_LOG_NUM_OF_INSTANCE

#endif /* __INCmvSysHwConfigh */


