#include <common.h>
#include "private_test_config.h"
#include "hwd_config.h"
#include "test_functions.h" 


#ifdef  ALL_TESTS
#define DRAM_TEST
#define COMMANDS_TEST

void main(void)
{
    all_tests();

}

MV_STATUS all_tests(void)
{
    /*! CPU tests */
    #ifdef  DRAM_TEST 
    dram_test();       
    #endif  /* DRAM_TEST */
    #ifdef  FLASH_TEST
    flash_test();
    #endif  /* FLASH_TEST */
    #ifdef  NVRAM_TEST  
    nvram_test();
    #endif  /* NVRAM_TEST */
    #ifdef  TOD_TEST
    tod_test();
    #endif  /* TOD_TEST */
    #ifdef  LED_TEST
    led_test();
    #endif  /* LED_TEST */
    #ifdef  DISPLAY_TEST
    display_test();
    #endif  /* DISPLAY_TEST */
    #ifdef  PCI_TEST
    pci_test();
    #endif  /* PCI_TEST */
    #ifdef  SMI_TEST
    /* smi_test();             <--     */
    #endif  /* SMI_TEST */
    #ifdef  I2C_TEST
    /* i2c_test();             <--     */
    #endif  /* I2C_TEST */
    /*! PP_SMI tests */
    #ifdef  PP_SMI_TEST
    pp_smi_test();
    #endif  /* PP_SMI_TEST */
    #ifdef  TIMER_TEST
    timer_test();
    #endif  /* TIMER_TEST */
    #ifdef  INTERRUPT_TEST
    interrupt_test();
    #endif  /* INTERRUPT_TEST */

    #ifdef  SOFTWARE_RST_TEST
    /* software_RST_test(); */
    #endif  /* SOFTWARE_RST_TEST */

    #ifdef  WATCHDOG_HW_TEST
    /* watchdog_HW_test(); */
    #endif  /* WATCHDOG_HW_TEST */

    /*! PLD tests */
    #ifdef  PLD_TYPE_TEST
    pld_type_test();
    #endif  /* PLD_TYPE_TEST */
    #ifdef  PLD_REGISTER_TEST
    pld_register_test();
    #endif  /* PLD_REGISTER_TEST */
    #ifdef  PLD_SPECIAL_FEATURES_TEST
    pld_special_features_test();
    #endif  /* PLD_SPECIAL_FEATURES_TEST */

    /*! ETH_SMI tests */
    #ifdef  ETH_TEST
    /* eth_test();             <--     */
    #endif  /* ETH_TEST */
    
    /*! COMMANDS tests */
    #ifdef  COMMANDS_TEST 
    commands_test();          
    #endif  /* COMMANDS_TEST */

    return MV_OK;
}

#endif  /* ALL_TESTS */

