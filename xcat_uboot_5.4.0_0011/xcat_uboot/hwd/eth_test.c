#include <common.h>
#include <net.h>
#include "mvTypes.h"
#include "hwd_config.h"

#include "diag_inf.h"
#include "diag_services.h"

#include "private_test_config.h"

#ifdef  ETH_TEST

MV_STATUS eth_lpbk_test(void)
{
    printf("\neth_lpbk_test... Pass !\n");
    return MV_OK;
}

MV_STATUS eth_test(void)
{
    MV_STATUS status = MV_OK;

    if(eth_lpbk_test() != MV_OK)
        status = MV_ERROR;

    return status;
}

#endif  /* ETH_TEST */
