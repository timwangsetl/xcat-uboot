#include <common.h>
#include "hwd_config.h"

#include "private_diag_if.h"
#include "private_test_config.h"


#ifdef  WATCHDOG_HW_TEST

/* Interface functions are resdy for implement in "private_diag_if.c" */
MV_STATUS watchdog_HW_test(void)
{
    printf("\nWATCHDOG HW test: Not implemented yet...\n");
    return MV_FAIL;;

#if UBOOT_MV_WATCHDOG_HW_TEST_DBG
    MV_U8 chk_loop;

    printf("\nWatchdog HW Test:\n");
    diagIfWdInit();   

    printf("\nThe board reset must occurr after %d seconds...\n", WD_4_SEC_INTERVAL_CNS);
    diagIfWdStart();   

    /* diagIfStartTimer(); */
    for(chk_loop = 0; chk_loop < 10; chk_loop++)
    {
        printf ("\r  WdStrobe loop No. %2d ", chk_loop);
        udelay(1000000);
        diagIfWdStrobeDevice(); 
    }

    /* diagIfStopTimer(); */
    diagIfWdStop(); 

    /* Watchdog HW Test - Fail */
    printf("\nWatchdog HW Test - Fail\n");
    return MV_FAIL;

#endif
}


#endif  /* WATCHDOG_HW_TEST */

