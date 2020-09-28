#include <common.h>
#include "hwd_config.h"

#include "private_diag_if.h"                       
#include "private_test_config.h"


#ifdef  SOFTWARE_RST_TEST


MV_STATUS software_RST_test(void)
{
    MV_U8 delay;

    delay = (USER_VISUAL_DELAY/SW_1_SEC_CNS);

    printf("\nSoftware RST Test:\n");
    while(delay)
    {
        printf("\rBoard Reset must occurr after %d seconds...", delay);
        udelay(SW_1_SEC_CNS);
        delay--; 
    }

    diagIfSoftRSTSet();

    /* Watchdog HW Test - Fail */
    printf("\nSoftware RST Test - Fail\n");
    return MV_FAIL;
}


#endif  /* SOFTWARE_RST_TEST */

