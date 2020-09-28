/*  File Name:  menu.h  */

#ifndef __INCmenuh
#define __INCmenuh


#include <common.h>
#include "mvTypes.h"
#include "diag_inf.h"
#include "test_functions.h" 

#include "private_test_config.h"


struct MenuItem
{
    int           id;
    int           parentid;
    char          *name;
    char          *usage;
    int           (*cmd)(void);
    char          *help;
};

typedef struct MenuItem MENUITEM;

#define ARRAY_SIZE(__arr) sizeof(__arr)/sizeof(__arr[0])

#define DIAG_PROMPT  "Marvell diagnostic>> "

#define BEGIN_MENU_MAP() MENUITEM   HWD_MENU [] = {
#define MENU_MAP(__parent, __name, __usage, __cmd, __help) \
    {__LINE__, __parent, __name, __usage, __cmd, __help},
#define END_MENU_MAP() };
                                               
BEGIN_MENU_MAP()

#ifdef DRAM_TEST
    MENU_MAP(-1, "* DRAM test", "", NULL, "")
        MENU_MAP(__LINE__-1, "DRAM overall test"           , "", dram_test, "")
        MENU_MAP(__LINE__-2, "Dram Read By Offset test"    , "", mvBspDramReadByOffset, "")
        MENU_MAP(__LINE__-3, "Dram Write By Offset test"   , "", mvBspDramWriteByOffset, "")
        MENU_MAP(__LINE__-4, "Dram Free Memory Read test"  , "", mvBspDramFreeMemoryReadTest, "")
        MENU_MAP(__LINE__-5, "Dram Free Memory Write test" , "", mvBspDramFreeMemoryWriteTest, "")
#endif  /*  */

#ifdef  FLASH_TEST
    MENU_MAP(-1, "* FLASH test", "", NULL, "")
        MENU_MAP(__LINE__-1, "FLASH overall test"          , "", flash_test, "")
        MENU_MAP(__LINE__-2, "Flash Addr. Bus test"        , "", mvBspFlashAddrBusTest, "")
        MENU_MAP(__LINE__-3, "Flash Data Bus test"         , "", mvBspFlashDataBusTest, "")
        MENU_MAP(__LINE__-4, "Flash Memory Device test"    , "", mvBspFlashMemoryDeviceTest, "")
        MENU_MAP(__LINE__-5, "Flash Erase By Offset test"  , "", mvBspFlashEraseByOffsetTest , "")
        MENU_MAP(__LINE__-6, "Flash Read By Offset test"   , "", mvBspFlashReadByOffsetTest, "")
        MENU_MAP(__LINE__-7, "Flash Write By Offset test"  , "", mvBspFlashWriteByOffsetTest, "")
#endif  /*  */

#ifdef  PCI_TEST
    MENU_MAP(-1, "* PCI test", "", NULL, "")
        MENU_MAP(__LINE__-1, "PCI Scan test"        , "", pci_test             , "")
        MENU_MAP(__LINE__-2, "PCI Config Read test" , "", pci_config_read_test , "")
        MENU_MAP(__LINE__-3, "PCI Config Write test", "", pci_config_write_test, "")
#endif  /*  */

#ifdef  SMI_TEST
    MENU_MAP(-1, "* SMI test", "", NULL, "")
        MENU_MAP(__LINE__-1, "SMI Scan test" , "", smi_scan_test , "")
        MENU_MAP(__LINE__-2, "SMI Read test" , "", smi_read_test , "")
        MENU_MAP(__LINE__-3, "SMI Write test", "", smi_write_test, "")
#endif  /*  */

#ifdef  I2C_TEST
    MENU_MAP(-1, "* I2C test", "", NULL, "")
        MENU_MAP(__LINE__-1, "I2C Scan Bus"  , "", mvI2cScanBus  , "")
        MENU_MAP(__LINE__-2, "I2C Scan test" , "", mvI2cScanTest , "")
        MENU_MAP(__LINE__-3, "I2C Read test" , "", mvI2cReadTest , "")
        MENU_MAP(__LINE__-4, "I2C Write test", "", mvI2cWriteTest, "")
#endif  /*  */

/*  Changed here  */

#ifdef  PP_SMI_TEST
    MENU_MAP(-1, "** PP_SMI test", "", NULL, "")
        MENU_MAP(__LINE__-1, "PP_SMI Scan test" , "", pp_smi_scan_test , "")
        MENU_MAP(__LINE__-2, "PP_SMI Read test" , "", pp_smi_read_test , "")
        MENU_MAP(__LINE__-3, "PP_SMI Write test", "", pp_smi_write_test, "")
#endif  /*  */

#ifdef  PP_PCI_TEST
    MENU_MAP(-1, "** PP_PCI test", "", NULL, "")
        MENU_MAP(__LINE__-1, "PP_PCI Express Read test" , "", pp_pci_express_read_test , "")
        MENU_MAP(__LINE__-2, "PP_PCI Custom  Read test" , "", pp_pci_custom_read_test   , "")
        MENU_MAP(__LINE__-3, "PP_PCI Write Reg. test"   , "", pp_pci_write_reg, "")
#endif  /*  */

#ifdef  PLD_TEST
    MENU_MAP(-1, "** PLD tests      - N.A.", "", NULL, "")
        MENU_MAP(__LINE__-1, "PLD Device Type test      - N.A." , "",  pld_type_test, "")
        MENU_MAP(__LINE__-2, "PLD Device Register test  - N.A." , "",  pld_register_test, "")
        MENU_MAP(__LINE__-3, "PLD Special Features test - N.A." , "",  pld_special_features_test, "")
#endif  /*  */

#ifdef  ETH_TEST
    MENU_MAP(-1, "* ETH test", "", NULL, "")
        MENU_MAP(__LINE__-1, "ETH loopback test" , "", eth_lpbk_test , "")
#endif  /*  */

#ifdef COMMANDS_TEST
    MENU_MAP(-1, "* COMMANDS overall tests", "", NULL, "")
        MENU_MAP(__LINE__-1, "COMMANDS overall tests"  , " - Run all tests", commands_test, "")
        MENU_MAP(__LINE__-2, "COMMANDS overall tests until fail"  , " - Run all tests until fail", commands_test_until_fail, "")
        MENU_MAP(__LINE__-3, "COMMANDS overall memory tests"  , " - Run all memory tests", commands_memory_test, "")
        MENU_MAP(__LINE__-4, "COMMANDS overall memory tests until fail"  , " - Run all memory tests until fail", commands_memory_test_until_fail, "")
        MENU_MAP(__LINE__-5, "COMMANDS overall general tests"  , " - Run all general tests", commands_general_test, "")
        MENU_MAP(__LINE__-6, "COMMANDS overall general tests until fail"  , " - Run all general tests until fail", commands_general_test_until_fail, "")		

    MENU_MAP(-1, "* COMMANDS Memory tests", "", NULL, "")
		MENU_MAP(__LINE__-1, "COMMANDS Memory test - base          (set address offset for mem cmds)" , "", mvMemoryBaseTest, "")	 
        MENU_MAP(__LINE__-2, "COMMANDS Memory test - cmp           (memory compare)"                  , "", mvMemoryCompareTest, "") 
        MENU_MAP(__LINE__-3, "COMMANDS Memory test - cmpm          (memory compare value)"            , "", mvMemoryCompareValueTest, "")
		MENU_MAP(__LINE__-4, "COMMANDS Memory test - cp            (memory copy)"                     , "", mvMemoryCopyTest, "")
        MENU_MAP(__LINE__-5, "COMMANDS Memory test - crc32         (checksum calculation)"            , "", mvMemoryCrc32Test, "")
		MENU_MAP(__LINE__-6, "COMMANDS Memory test - dramregs      (show dramregs)"                   , "", mvMemoryDramregsTest, "")
        MENU_MAP(__LINE__-7, "COMMANDS Memory test - fi            (find value in memory)"            , "", mvMemoryFindValTest, "")		
        MENU_MAP(__LINE__-8, "COMMANDS Memory test - md            (memory display)"                  , "", mvMemoryDisplayTest, "")		
        MENU_MAP(__LINE__-9, "COMMANDS Memory test - mw            (memory write)"                    , "", mvMemoryWriteTest, "")
        MENU_MAP(__LINE__-10,"COMMANDS Memory test - phyRead       (read phy register)"               , "", mvMemoryPhyReadTest, "")		
        MENU_MAP(__LINE__-11,"COMMANDS Memory test - phyWrite      (write phy register)"              , "", mvMemoryPhyWriteTest, "")		
        MENU_MAP(__LINE__-12,""                                                                       , "", mvMemoryDummyTest, "")
        MENU_MAP(__LINE__-13,"###########################################"                            , "", mvMemoryDummyTest, "")
        MENU_MAP(__LINE__-14,"======== PCI / PEX commands  =============="                            , "", mvMemoryDummyTest, "")	
        MENU_MAP(__LINE__-15,"COMMANDS Memory test - me            (PCI master enable)"               , "", mvMemoryMeTest, "")		
        MENU_MAP(__LINE__-16,"COMMANDS Memory test - mp            (map PCI BAR)"                     , "", mvMemoryMpTest, "")
        MENU_MAP(__LINE__-17,"COMMANDS Memory test - pci           (list PCI configuration space)"    , "", mvMemoryPciTest, "")
        MENU_MAP(__LINE__-18,"COMMANDS Memory test - pciePhyRead   (read PCI-E phy register)"         , "", mvMemoryDummyTest, "")		
        MENU_MAP(__LINE__-19,"COMMANDS Memory test - pciePhyWrite  (write PCI-E phy register)"        , "", mvMemoryDummyTest, "")		
        MENU_MAP(__LINE__-20, ""                                                                      , "", mvMemoryDummyTest, "")		
        MENU_MAP(__LINE__-21,"###########################################"                            , "", mvMemoryDummyTest, "")
        MENU_MAP(__LINE__-22,"======== I2C commands   ==================="                            , "", mvMemoryDummyTest, "")	
        MENU_MAP(__LINE__-23,"COMMANDS Memory test - icrc32        (compute CRC32 checksum)"          , "", mvMemoryIcrc32Test, "")		
        MENU_MAP(__LINE__-24,"COMMANDS Memory test - imd           (i2c memory display)"              , "", mvMemoryImdTest, "")
        MENU_MAP(__LINE__-25,"COMMANDS Memory test - imw           (i2c memory write (fill))"         , "", mvMemoryImwTest, "")		
        MENU_MAP(__LINE__-26,"COMMANDS Memory test - iprobe        (find I2C chip addresses)"         , "", mvMemoryIprobeTest, "")
        MENU_MAP(__LINE__-27,""                                                                       , "", mvMemoryDummyTest, "")		
        MENU_MAP(__LINE__-28,"###########################################"                            , "", mvMemoryDummyTest, "")
        MENU_MAP(__LINE__-29,"======== FLASH commands ==================="                            , "", mvMemoryDummyTest, "")		
        MENU_MAP(__LINE__-30,"COMMANDS Memory test - erase         (erase FLASH memory)"              , "", mvMemoryEraseFlashTest, "")
        MENU_MAP(__LINE__-31,"COMMANDS Memory test - flinfo        (print info for FLASH)"            , "", mvMemoryFlinfoFlashTest, "")		
        MENU_MAP(__LINE__-32,"COMMANDS Memory test - protect       (Ena/Dis FLASH write protect)"     , "", mvMemoryProtectTest, "")
        MENU_MAP(__LINE__-33,"COMMANDS Memory test - sflash        (read/write or erase SPI FLASH)"   , "", mvMemorySFlashTest, "")		
    
    MENU_MAP(-1, "* COMMANDS General tests", "", NULL, "")
        MENU_MAP(__LINE__-1, "COMMANDS General test - dclk         (display the MV device CLKs)"      , "", mvGeneralDclkTest, "")
        MENU_MAP(__LINE__-2, "COMMANDS General test - echo         (echo args to console)"            , "", mvGeneralEchoTest, "")
        MENU_MAP(__LINE__-3, "COMMANDS General test - ping         (send ICMP ECHO_REQUEST)"          , "", mvGeneralPingTest, "")
		MENU_MAP(__LINE__-4, "COMMANDS General test - se           (PCI slave enable)"                , "", mvGeneralSeTest, "")
		MENU_MAP(__LINE__-5, "COMMANDS General test - sg           (scanning the PHY's status)"       , "", mvGeneralSgTest, "")
		MENU_MAP(__LINE__-6, "COMMANDS General test - sp           (scan PCI bus)"                    , "", mvGeneralSpTest, "")
		MENU_MAP(__LINE__-7, "COMMANDS General test - version      (print U-boot version)"            , "", mvGeneralVersionTest, "")
		MENU_MAP(__LINE__-8, "COMMANDS General test - test_smi     (test the SMI interface)"          , "", mvGeneralTestsmiTest, "")
		MENU_MAP(__LINE__-9, ""                                                                       , "", mvGeneralDummyTest, "")		
        MENU_MAP(__LINE__-10,"###########################################"                            , "", mvGeneralDummyTest, "")
        MENU_MAP(__LINE__-11,"======== SWITCH commands =================="                            , "", mvGeneralDummyTest, "")		
        MENU_MAP(__LINE__-12,"COMMANDS Memory test - phyr          (read phy value)"                  , "", mvGeneralPhyrTest, "")
        MENU_MAP(__LINE__-13,"COMMANDS Memory test - phyw          (write phy value)"                 , "", mvGeneralPhywTest, "")		
		MENU_MAP(__LINE__-14,"COMMANDS Memory test - smd           (display switch internal register)", "", mvGeneralSmdTest, "")
		MENU_MAP(__LINE__-15,"COMMANDS Memory test - smw           (modify switch internal register)" , "", mvGeneralSmwTest, "")
        MENU_MAP(__LINE__-16,"COMMANDS Memory test - switchMibRead (read switch MIB counters)"        , "", mvGeneralSwitchMibReadTest, "")		
#endif  /*  */

/*  End of Change.  */

	/* MENU_MAP(__LINE__-5, "COMMANDS Memory test - cpumap   (display CPU memory mapping)"      , "", mvMemoryCpumapTest, "") */

#ifdef  NVRAM_TEST
    MENU_MAP(-1, "* NVRAM test      - N.A.", "", nvram_test, "")
#endif  /*  */
#ifdef  TOD_TEST
    MENU_MAP(-1, "* TOD test        - N.A.", "", tod_test, "")
#endif  /*  */
#ifdef  LED_TEST
    MENU_MAP(-1, "* LED test        - N.A.", "", led_test, "")
#endif  /*  */
#ifdef  DISPLAY_TEST
    MENU_MAP(-1, "* Display test    - N.A.", "", display_test, "")
#endif  /*  */
#ifdef  TIMER_TEST
    MENU_MAP(-1, "* TIMER test      - N.A.", "", timer_test, "")
#endif  /*  */
#ifdef  INTERRUPT_TEST
    MENU_MAP(-1, "* INTERRUPT test  - N.A.", "", interrupt_test, "")
#endif  /*  */
#ifdef  WATCHDOG_HW_TEST
    MENU_MAP(-1, "* WATCHDOG HW test- N.A.", "", watchdog_HW_test, "")
#endif  /*  */  
#ifdef  SOFTWARE_RST_TEST
    MENU_MAP(-1, "* SOFTWARE RESET test", "", software_RST_test, "")
#endif  /*  */

#ifdef  ALL_TESTS
    MENU_MAP(-1, "** COMPLETE test", "", all_tests, "")
#endif  /*  */

END_MENU_MAP()





#endif /*__INCmenuh*/
