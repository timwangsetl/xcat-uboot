/*******************************************************************************
 Copyright (C) Marvell International Ltd. and its affiliates

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

*******************************************************************************/

#include "mv_eth.h"
#include "mvSysHwConfig.h"
#include "mvTypes.h"
#include "mvCtrlEnvLib.h"
#include "mvSysGbe.h"
#include "mv_egiga.h"
#include "mv_prestera_switch.h"
#include "mii.h"

#include "common.h"

/*******************************************************************************
 * mv_eth_init
 */
int mv_eth_init(void)
{
    MV_CHIP_FEATURES *featuresP = mvChipFeaturesGet();

    mvEthForceInitAllPorts();

    {
        if (mvEgigaLoad (0 /* featuresP->oobGbeIndex */) != MV_OK)
        {
            mvOsPrintf ("%s: mvEgigaLoad() failed.\n", __func__);
            return MV_FAIL;
        }
    }

#if defined(MV_INCLUDE_PP)
    if (mvSwitchLoad () != MV_OK)
    {
        mvOsPrintf ("%s: mvSwitchLoad() failed.\n", __func__);
        return MV_FAIL;
    }
#endif

    return 0;
}

