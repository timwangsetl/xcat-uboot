/*  File Name:  private_test_config.h  */

#ifndef __INCprivate_test_configh
#define __INCprivate_test_configh


#include "mvTypes.h"
#include "mvCommon.h"
#include "private_diag_if.h"

/* #include "../board/88f5181/Soc/ctrlEnv/MV_88FXX81/mvCtrlEnvSpec.h" */
#include "../board/mv_feroceon/mv_kw/kw_family/ctrlEnv/mvCtrlEnvSpec.h"
/* #include "mvPciRegs.h" */
#include "../board/mv_feroceon/mv_hal/pci/mvPciRegs.h"

/**************************/
/* ! Functiond that not implemented yet:
*  
*  display_test()
*  nvram_test()
*  tod_test()
*  interrupt_test()
*  led_test()
*  timer_test()
*  watchdog_HW_test
*  pld_register_test()
*  pld_special_features_test()
*  pld_type_test()
*  
*
* **************************/


/*****************************************************************************
*   
*           SOFTWARE TESTS DEFINITIONS
*
*****************************************************************************/

/* ! CPU tests */
#define  DRAM_TEST
    #undef  FLASH_TEST
    #undef  PCI_TEST
#undef  SMI_TEST
#undef  I2C_TEST
    #undef   NVRAM_TEST
    #undef   TOD_TEST
    #undef   LED_TEST
    #undef   DISPLAY_TEST
    #undef   TIMER_TEST
    #undef   INTERRUPT_TEST
    #undef   WATCHDOG_HW_TEST
    #undef  SOFTWARE_RST_TEST

/* ! PP_SMI test */
    #undef   PP_SMI_TEST

/* ! PP_PCI test */
    #undef  PP_PCI_TEST

/* ! PLD tests */
    #undef   PLD_TEST
    #ifdef  PLD_TEST
        #define  PLD_TYPE_TEST
        #define  PLD_REGISTER_TEST
        #define  PLD_SPECIAL_FEATURES_TEST
    #endif  /* PLD_TEST */

/* ! ETHERNET tests */
#define ETH_TEST

/*! COMMANDS tests */
#define  COMMANDS_TEST 

/* ! Complete test */
#define  ALL_TESTS       


/*****************************************************************************
*   
*           HARDWARE TEST PARMETERS
*
*****************************************************************************/


#ifndef DIAG_IF_SDRAM_BASE
#define DIAG_IF_SDRAM_BASE          CFG_SDRAM_BASE
#endif

#ifndef DIAG_IF_INTER_REGS_BASE
#define DIAG_IF_INTER_REGS_BASE     INTER_REGS_BASE
#endif


/* define FLASH Data Bus Width */
#define FLASH_BUS_WIDTH MV_U16

/* define SDRAM Data Bus Width, Marker and Pattern */
#define BUS_WIDTH MV_U32
#define MARKER    0x12345678
#define PATTERN   0xA5A5A5A5


/* DRAM TEST PARAMS */
#define BASE_ADDRESS        0           /* dram base address */                 /* MV_U32  */
/* The first size offset should be such, so the first address after
   code & OS data so we don't overlap with it during tests.  */
#define FIRST_SIZE_OFFSET   0x03000000  /* offset from base_address */          /* MV_U32  */
#define DEV_AMOUNT          1           /* dram amount of devices */            /* MV_U8   */
#define DRAM_DATA_BUS_WIDTH 32          /* dram bus width */                    /* MV_U8   */
#define MEM_SIZE            0           /* real SDRAM size */                   /* MV_U32  */
                                        /* if 0 - apply size detect algorithm */   /* mvBspDiagBooleanParamFNC* */

/* FLASH TEST PARAMS */
/* Params for falash_params struct: should be assigned as MV_U32 type */
#define FLASH_BASE_ADDR     0xFF000000  /* device base address */   /* FLASH_CS_BASE */
#define PAGE_SIZE           _128K       /* device page size */
#define OVERALL_SIZE        _16M        /* device overall size */
#define TESTED_AREA_SIZE    _16M        /* device tested size */
#define DATA_BUS_WIDTH      16          /* device bus width */
#define SERIAL_DEV_AMOUNT   1           /* amount of devices in serial configuration */
#define RESERVED_OFFSETS_0  0           /* offsets to not be deleted (boots area) */
#define RESERVED_OFFSETS_1  0
#define RESERVED_OFFSETS_2  0
#define RESERVED_SIZES_0    0           /* The size of each reserved area */
#define RESERVED_SIZES_1    0
#define RESERVED_SIZES_2    0

#define MV_FLASH_EMPTY_32BIT    0xFFFFFFFF


/* PCI TEST PARAMS */
#define DIAG_MAX_PCI_BUSSES         32
#define DIAG_MAX_PCI_DEVICES        32
#define DIAG_PCI_READ_WRITE_REG     0


/* SMI TEST PARAMS */
#define DIAG_MAX_SMI_DEVICES        32
#define DIAG_SMI_READ_WRITE_REG     0


/* PP_SMI TEST PARAMS */
#undef  SX_ONLY_DIRECT
#undef  SX_ONLY_INDIRECT
    #define DX_ONLY
#undef  DX_SX_DIRECT
#undef  DX_SX_INDIRECT

#define DX_SMI_ADDR         0x17
#define SX_SMI_ADDR_FIRST   0
#define SX_SMI_ADDR_LAST    0


/* I2C TEST PARAMS */

#define DIAG_I2C_NUM_OF_DEVICES    7 

/* I2C MUX (Philips PCA9544A) Address and SFP's  (I2C id) */
/* All the SFP's are muxed under I2C_DEV_0, so they have BUS_ID's
*/
#define DIAG_I2C_SFP_MUX_PCA9544A_ADDR      0xE0
#define DIAG_I2C_SFP1_INDEX      0x01 /* Ry@ 05302007 used for bus_id */
#define DIAG_I2C_SFP2_INDEX      0x02
#define DIAG_I2C_SFP3_INDEX      0x03
#define DIAG_I2C_SFP4_INDEX      0x04

/* RTC - real time clock EEPROM - MAXIM DS1339 (I2C id) */
#define DIAG_I2C_RTC_EEPROM_DS1339_ADDR     0xD0
 
/* Tempreture Sensor & Fan Control - LM96000 (I2C id) */
#define DIAG_I2C_TEMP_SENSE_LM96000_ADDR    0x5C

/* 16 GPIO's Controller Extender - PCA9555 (I2C id) */
#define DIAG_I2C_GPIO_EXTENDER_PCA9555_ADDR 0x40

/* PoE Controller Extender - PowerDsine PD63000 with
   two PD64012 PoE Managers (I2C id) */
#define DIAG_I2C_POE_CONTR_PD63000_ADDR     0x48



/* WATCHDOG HW TEST PARAMS */
#define WD_BASE_ADDR_CNS        (INTER_REGS_BASE + 0x20108)

#define WD_MASK_REG_CNS                \
                        ((volatile MV_U32 *)(INTER_REGS_BASE + 0x20108))
#define SW_RST_EN_REG_CNS              \
                        ((volatile MV_U32 *)(INTER_REGS_BASE + 0x2010C))
#define CPU_TIMERS_CONTROL_REG_CNS     \
                        ((volatile MV_U32 *)(INTER_REGS_BASE + 0x20300))
#define CPU_WD_TIMER_RELOAD_REG_CNS    \
                        ((volatile MV_U32 *)(INTER_REGS_BASE + 0x20320))
#define CPU_WD_TIMER_REG_CNS           \
                        ((volatile MV_U32 *)(INTER_REGS_BASE + 0x20324))
/* #define WD_CLEAR_REG_ADDR_CNS       \ 
                        ((volatile MV_U32 *)(WD_BASE_ADDR_CNS + ?????)) */

#define WD_EN_BIT_CNS               ((MV_U32)0x1)
#define WD_CLEAR_BIT_CNS            0x0             /* ((MV_U32)~0x1)  */
#define WD_DEFAULT_MASK_CNS         ((MV_U32)0x2)
#define CPU_WD_TIMER_EN_CNS         ((MV_U32)0x10)

#define WD_1_SEC_INTERVAL_CNS       ((MV_U32)0x1)
#define WD_2_SEC_INTERVAL_CNS       ((MV_U32)0x2)
#define WD_3_SEC_INTERVAL_CNS       ((MV_U32)0x3)
#define WD_4_SEC_INTERVAL_CNS       ((MV_U32)0x4)
  


/* SW RESET TEST PARAMS */
/* #define SW_RST_MASK_BIT_CNS         ((MV_U32)0x4) */
/* #define SW_RST_EN_BIT_CNS           ((MV_U32)0x1) */

#define CPLD_BYTE_2      0x02       /* Software Reset Register 1 - at 0xE8000002 */
#define CPLD_BYTE_3      0x03       /* Software Reset Register 2 - at 0xE8000003 */

#define SW_1_SEC_CNS                1000000
#define USER_VISUAL_DELAY           (4 * SW_1_SEC_CNS)
#define USER_PING_DELAY             (11 * SW_1_SEC_CNS)



/* ! PP_PCI test */

#define GET_PCI_DEVICE_NUM(pp_num)      pp_num==1 ? 6 : 7
#define MAX_PP_PCI_DEVS     2



#endif  /* __INCprivate_test_configh */

