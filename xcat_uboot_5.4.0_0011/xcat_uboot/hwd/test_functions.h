/*  File Name:  test_functions.h  */

#ifndef __INCtest_functionsh
#define __INCtest_functionsh

#include "mvTypes.h"
#include "diag_inf.h"
#include "diag_services.h"


/*! CPU tests */
extern void diagIfGetMarvellBoardName(char *boardName, char *boardRev);
extern void diagIfDramInit(void);
extern MV_STATUS dram_test(void);
extern MV_STATUS mvBspDramAddrBusTest(void);
extern MV_STATUS mvBspDramDataBusTest(void);
extern MV_STATUS mvBspDramSizeTest(void);
extern MV_STATUS mvBspDramMemoryDeviceTest(void);
extern MV_STATUS mvBspDramReadByOffset(void);
extern MV_STATUS mvBspDramWriteByOffset(void);
extern MV_STATUS mvBspDramFreeMemoryReadTest(void);
extern MV_STATUS mvBspDramFreeMemoryWriteTest(void);

extern void diagIfFlashInit(void);  
extern MV_STATUS flash_test(void);
extern MV_STATUS mvBspFlashAddrBusTest(void);
extern MV_STATUS mvBspFlashDataBusTest(void);
extern MV_STATUS mvBspFlashMemoryDeviceTest(void);
extern MV_STATUS mvBspFlashEraseByOffsetTest(void);
extern MV_STATUS mvBspFlashReadByOffsetTest(void);
extern MV_STATUS mvBspFlashWriteByOffsetTest(void);

extern MV_STATUS nvram_test(void);
extern MV_STATUS tod_test(void);
extern MV_STATUS led_test(void);
extern MV_STATUS display_test(void);

extern MV_STATUS pci_test(void);
extern MV_STATUS pci_scan_test(void);
extern MV_STATUS pci_config_read_test(void);
extern MV_STATUS pci_config_write_test(void);

extern MV_STATUS smi_test(void);
extern MV_STATUS smi_scan_test(void);
extern MV_STATUS smi_read_test(void);
extern MV_STATUS smi_write_test(void);

extern MV_STATUS i2c_test(void);
extern MV_STATUS mvI2cScanBus(void);
extern MV_STATUS mvI2cScanTest(void);
extern MV_STATUS mvI2cReadTest(void); 
extern MV_STATUS mvI2cWriteTest(void);

extern MV_STATUS timer_test(void);
extern MV_STATUS interrupt_test(void);
extern MV_STATUS software_RST_test(void);
extern MV_STATUS watchdog_HW_test(void);

/* ! PP_SMI tests */
extern MV_STATUS pp_smi_test(void);
extern MV_STATUS pp_smi_scan_test(void);
extern MV_STATUS pp_smi_read_test(void);
extern MV_STATUS pp_smi_write_test(void);

/* ! PP_PCI tests */
extern MV_STATUS pp_pci_express_read_test(void);
extern MV_STATUS pp_pci_custom_read_test(void);
extern MV_STATUS pp_pci_write_reg(void);

/*! PLD tests */
extern MV_STATUS pld_type_test(void);
extern MV_STATUS pld_register_test(void);
extern MV_STATUS pld_special_features_test(void);

/*! ETH tests */
extern MV_STATUS eth_test(void);
extern MV_STATUS eth_lpbk_test(void);

/*! COMMANDS tests */
extern MV_STATUS commands_test(void);
extern MV_STATUS commands_test_until_fail(void);
extern MV_STATUS commands_memory_test(void);
extern MV_STATUS commands_memory_test_until_fail(void);
extern MV_STATUS commands_general_test(void);
extern MV_STATUS commands_general_test_until_fail(void);

extern MV_STATUS mvMemoryBaseTest(void);
extern MV_STATUS mvMemoryCopyTest(void);
extern MV_STATUS mvMemoryCompareTest(void);
extern MV_STATUS mvMemoryCompareValueTest(void);
#if UBOOT_COMMANDS_MEM_TEST_DEBUG
extern MV_STATUS mvMemoryCpumapTest(void);
#endif
extern MV_STATUS mvMemoryCrc32Test(void);
extern MV_STATUS mvMemoryDramregsTest(void);
extern MV_STATUS mvMemoryFindValTest(void);
extern MV_STATUS mvMemoryDisplayTest(void);
extern MV_STATUS mvMemoryWriteTest(void);
extern MV_STATUS mvMemoryPhyReadTest(void);
extern MV_STATUS mvMemoryPhyWriteTest(void);

/*======== PCI / PEX commands  =============*/
extern MV_STATUS mvMemoryMeTest(void);
extern MV_STATUS mvMemoryMpTest(void);
extern MV_STATUS mvMemoryPciTest(void);

/*======== I2C commands  ===================*/
extern MV_STATUS mvMemoryIcrc32Test(void);
extern MV_STATUS mvMemoryImdTest(void);
extern MV_STATUS mvMemoryImwTest(void);
extern MV_STATUS mvMemoryIprobeTest(void);

/*======== FLASH commands ===================*/
extern MV_STATUS mvMemoryEraseFlashTest(void);
extern MV_STATUS mvMemoryFlinfoFlashTest(void);
extern MV_STATUS mvMemoryProtectTest(void);
extern volatile MV_STATUS mvMemorySFlashTest(void);

extern MV_STATUS mvGeneralDclkTest(void);
extern MV_STATUS mvGeneralEchoTest(void);
extern MV_STATUS mvGeneralPingTest(void);
extern MV_STATUS mvGeneralSeTest(void);
extern MV_STATUS mvGeneralSgTest(void);
extern MV_STATUS mvGeneralSpTest(void);
extern MV_STATUS mvGeneralVersionTest(void);
extern MV_STATUS mvGeneralTestsmiTest(void);

/*======== SWITCH commands ==================*/
extern MV_STATUS mvGeneralPhyrTest(void);
extern MV_STATUS mvGeneralPhywTest(void);
extern MV_STATUS mvGeneralSmdTest(void);
extern MV_STATUS mvGeneralSmwTest(void);
extern MV_STATUS mvGeneralSwitchMibReadTest(void);

extern MV_STATUS mvGeneralDummyTest(void);
extern MV_STATUS mvMemoryDummyTest(void);

/*! Complete test */
extern MV_STATUS all_tests(void);
void main(void);

#endif /*__INCtest_functionsh*/
