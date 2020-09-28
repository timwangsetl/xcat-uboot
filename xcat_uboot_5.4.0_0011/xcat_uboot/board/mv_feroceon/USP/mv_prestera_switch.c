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

#include <common.h>
#include <command.h>
#include <net.h>
#include <malloc.h>

#include "mvOs.h"
#include "mii.h"
#include "mv_prestera_switch.h"
#include "mvNetDrvCommon.h"
#include "mvPresteraPriv.h"
#include "mvDebug.h"
#include "mvBoardEnvLib.h"
#include "mvPresteraEEPROM.h"

/*
 #define MV_DEBUG
 */
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

/*
 * Struct needed to init Generic Network Driver (GND).
 */
static MV_SWITCH_GEN_INIT G_swGenInit;
static MV_BOOL G_mvSwitchDrtIsInited = MV_FALSE;

MV_BOOL standalone_network_device = MV_TRUE;

/*
 * Switch driver control structure.
 */
typedef struct _mvSwitchDrvCtrl
{
    GEN_SYNC_POOL                      *rxPktInfoPoolP;
    GEN_SYNC_POOL                      *txPktInfoPoolP;

    MV_GND_HW_FUNCS                    *hwP;
} MV_SWITCH_DRV_CTRL;

MV_SWITCH_DRV_CTRL  G_mvSwitchDrvCtrl;
MV_SWITCH_DRV_CTRL *G_mvSwitchDrvCtrlP = &G_mvSwitchDrvCtrl;

/*******************************************************************************
 * mvSwitchDrvIsInited
 */
MV_BOOL mvSwitchDrvIsInited(void)
{
    return G_mvSwitchDrtIsInited;
}

/*******************************************************************************
 * mvSwitchEnvMacGet
 */
extern MV_U8 G_switchDefaultMacAddr[MV_MAC_ADDR_SIZE];

MV_STATUS mvSwitchEnvMacGet(MV_U8 *ethAddr)
{
    MV_8 *enet_addr;

    /*
     * U-Boot allocates eth addresses in 'eth%daddr' format.
     */
    enet_addr = getenv ("eth1addr");

    /* Check MAC format. */
    if (!enet_addr || strlen (enet_addr) != 17)
    {
        /* Use default MAC address. */
        //mvOsPrintf("MAC address 'eth1addr' is undefined, using default.\n");
        mvOsMemmove(ethAddr, G_switchDefaultMacAddr, MV_MAC_ADDR_SIZE);
        return MV_OK;
    }

    if (mvMacStrToHex(enet_addr /* string */, ethAddr /* HEX */) != MV_OK)
    {
        mvOsPrintf ("%s: mvMacStrToHex failed.\n", __func__);
        return MV_FAIL;
    }

    return MV_OK;
}

/*******************************************************************************
 * mvSwitchUserTxDone
 *     May only be used in driver DSR mode.
 */
MV_STATUS mvSwitchUserTxDone(MV_GND_PKT_INFO *pktInfoP)
{
    MV_SWITCH_DRV_CTRL *drvCtrlP = G_mvSwitchDrvCtrlP;

    if (genSyncPoolPut(drvCtrlP->txPktInfoPoolP, pktInfoP) != MV_OK)
    {
        mvOsPrintf("%s: genSyncPoolPut failed.\n", __func__);
        return MV_FAIL;
    }

    return MV_OK;
}

/*******************************************************************************
 * mvSwitchInitSwitchGen
 */
MV_STATUS mvSwitchInitSwitchGen(MV_BOOL isMiiMode)
{
    MV_SW_GEN_RX_HOOKS    rxHooks;
    MV_SW_GEN_TX_HOOKS    txHooks;
    MV_U32                hookId;
    MV_SWITCH_GEN_HW_INIT genHwInit;

    if (switchDrvGenIsInited() == MV_TRUE)
    {
        return MV_OK;
    }

    /*
     * Init and configure lower layer (including hardware).
     */
    if (mvSwitchEnvMacGet(genHwInit.ethAddr) != MV_OK)
    {
        mvOsPrintf("%s: mvSwitchEnvMacGet failed.\n", __func__);
        return MV_FAIL;
    }

    /* In U-Boot switch port are initialized in the driver init. */
    genHwInit.initPortsF = NULL;

    genHwInit.cfgCascadePorts = switchGenCfgCascadePorts;
    G_swGenInit.genHwInitP    = &genHwInit;

    G_swGenInit.isMiiMode       = isMiiMode;
    G_swGenInit.gndInitP        = NULL; /* use GND defaults */
    G_swGenInit.isTxSyncMode    = MV_TRUE;
    G_swGenInit.gbeDefaultIndex = mvChipFeaturesGet()->miiGbeIndex;

    if (switchDrvGenInit(&G_swGenInit) != MV_OK)
    {
        mvOsPrintf("%s: switchDrvGenInit failed.\n", __func__);
        return MV_FAIL;
    }

    /*
     * Register RX_READY callbacks.
     */
    if (switchDrvGenHookSetCalcFwdRxPktHookId(
         (MV_SWITCH_GEN_HOOK_CALC_PKT_ID)mvSwitchGetPortNumFromExtDsa) != MV_OK)
    {
        mvOsPrintf("%s: switchDrvGenHookSetCalcFwdRxPktHookId failed.\n",
                   __func__);
        return MV_FAIL;
    }

    if (switchGenHooksInitRx(MV_PP_NUM_OF_PORTS) != MV_OK)
    {
        mvOsPrintf("%s: switchGenHooksInitRx failed.\n", __func__);
        return MV_FAIL;
    }

    rxHooks.fwdRxReadyPktHookF =
            (MV_SWITCH_GEN_HOOK_FWD_RX_PKT)switchFwdRxPktToOs;
    rxHooks.hdrAltBeforeFwdRxPktHookF =
            (MV_SWITCH_GEN_HOOK_HDR_ALT_RX)mvSwitchExtractExtDsa;
    rxHooks.hdrAltBeforeFwdRxFreePktHookF =
            (MV_SWITCH_GEN_HOOK_HDR_ALT_RX_FREE)mvSwitchInjectExtDsa;

    for (hookId = 0; hookId < MV_PP_NUM_OF_PORTS; hookId++)
    {
        if (switchGenHooksFillRx(&rxHooks, hookId) != MV_OK)
        {
            mvOsPrintf("%s: switchGenHooksFillRx failed.\n", __func__);
            return MV_FAIL;
        }
    }

    /*
     * Register TX_DONE callbacks.
     */
    if (switchGenHooksInitTx(MV_NET_OWN_TOTAL_NUM) != MV_OK)
    {
        mvOsPrintf("%s: switchGenHooksInitTx failed.\n", __func__);
        return MV_FAIL;
    }

    txHooks.hdrAltBeforeFwdTxDonePktHookF =
            (MV_SWITCH_GEN_HOOK_HDR_ALT_TX)mvSwitchRemoveExtDsa;
    txHooks.hdrAltBeforeTxF =
            (MV_SWITCH_GEN_HOOK_HDR_ALT_TX)mvSwitchInjectExtDsa;
    txHooks.txDonePktFreeF  =
            (MV_SWITCH_GEN_HOOK_DRV_OS_PKT_FREE)mvSwitchUserTxDone;

    if (switchGenHooksFillTx(&txHooks,
                             MV_NET_OWN_NET_DRV_RX_REMOVE_TX_ADD_DSA) != MV_OK)
    {
        mvOsPrintf("%s: switchGenHooksFillTx failed.\n", __func__);
        return MV_FAIL;
    }

    return MV_OK;
}

/*******************************************************************************
 * mvSwitchDevReg
 */
MV_STATUS mvSwitchDevReg(MV_BOOL isMiiMode)
{
    struct eth_device *dev = NULL;

    dev = mvOsCalloc (1, sizeof(struct eth_device));
    if (dev == NULL)
    {
        mvOsPrintf ("%s: alloc failed.\n", __func__);
        return MV_FAIL;
    }

    dev->init   = (void *) mvSwitchInit;
    dev->halt   = (void *) mvSwitchHalt;
    dev->send   = (void *) mvSwitchTx;
    dev->recv   = (void *) mvSwitchRx;
    dev->iobase = 0;

    if (mvSwitchEnvMacGet(dev->enetaddr) != MV_OK)
    {
        mvOsPrintf("%s: mvSwitchEnvMacGet failed.\n", __func__);
        return MV_FAIL;
    }

    if (isMiiMode == MV_TRUE)
    {
        mvOsMemmove(dev->name, "ppmii", NAMESIZE);
    }
    else
    {
        mvOsMemmove(dev->name, "ppsdma", NAMESIZE);
    }

    if (eth_register (dev))
    {
        mvOsPrintf ("%s: eth_register() failed.\n", __func__);
        return MV_FAIL;
    }

    return MV_OK;
}

/*******************************************************************************
 * mvSwitchLoad
 */
MV_STATUS mvSwitchLoad(void)
{
    MV_BOOL             isMiiMode;
    char               *ethPrime;

    ethPrime = getenv ("ethprime");
    if (msOsStrncmp(ethPrime, "ppmii", strlen("ppmii")) == 0)
    {
        isMiiMode = MV_TRUE;
    }
    else
    {
        /* PP-SDMA interface is the default. */
        isMiiMode = MV_FALSE;
    }

    standalone_network_device = MV_TRUE;

    /*
     * Register device in u-boot eth layer.
     */
    if (mvSwitchDevReg(isMiiMode) != MV_OK)
    {
        mvOsPrintf("%s: mvSwitchDevReg.\n", __func__);
        return MV_FAIL;
    }

    return MV_OK;
}

/*******************************************************************************
 * mvSwitchInit
 */
int mvSwitchInit(struct eth_device *dev, bd_t* bis)
{
    MV_SWITCH_DRV_CTRL *drvCtrlP = G_mvSwitchDrvCtrlP;
    MV_U8 ethAddr[MV_MAC_ADDR_SIZE];
    MV_U32              totalBuffNum;
    MV_U32              maxFragsInPkt;
    MV_U32              linkUpTrial;
    MV_BOOL             isMiiMode;
    GEN_SYNC_POOL      *poolP;
    char               *ethAct;
    char               *switchEmulateEepromStr;
    MV_BOOL             switchEmulateEepromFlag = MV_TRUE;

    static MV_BOOL funcAlreadyRun = MV_FALSE;
    static MV_BOOL waitOnceForLinkUp = MV_FALSE;

    /* xCat2 boards have PP-EEPROM currently. */
    static MV_BOOL isPpPortsInited = MV_FALSE;
    /* Initializing all switch port (PP-EEPROM simulation). */

    if (isPpPortsInited == MV_FALSE)
    {
        MV_SWITCH_GEN_HOOK_HW_INIT_PORTS fP;

        switchEmulateEepromStr = getenv ("switchUseSwEepromEmulation");
        if (switchEmulateEepromStr &&
            msOsStrncmp(switchEmulateEepromStr, "yes", strlen("yes")) == 0)
        {
            mvOsPrintf("Env variable 'switchUseSwEepromEmulation' set to 'yes'!\n");
            switchEmulateEepromFlag = MV_TRUE;
        }
        else
        {
            mvOsPrintf("Env variable 'switchUseSwEepromEmulation' set to 'no'!\n");
            switchEmulateEepromFlag = MV_FALSE;
        }

        if (switchEmulateEepromFlag == MV_TRUE)
        {
            fP = mvBoardSwitchPortsInitFuncGet();

            mvOsPrintf("Initializing switch ports...");

            if (fP != NULL)
            {
                if (fP() != MV_OK)
                {
                    mvOsPrintf("PP-EEPROM simulation failed.\n");
                    return 1; /* failure */
                }
                mvOsPrintf("Done.\n");
            }
            else
            {
                mvOsPrintf("PP-EEPROM simulation failed.\n");
                /* return 1; */ /* failure */
            }
        }
        isPpPortsInited = MV_TRUE;
    }

    if (waitOnceForLinkUp == MV_FALSE)
    {
        mvOsDelay(1000);
        waitOnceForLinkUp = MV_TRUE;

        /* To support auto-choosing of interface with active link. */
        for (linkUpTrial = 0; linkUpTrial < 200; linkUpTrial++)
        {
            if (mvSwitchIsAnyLinkUp() == MV_TRUE)
            {
                MV_U32 portWithLinkUp = mvSwitchFirstPortLinkUpGet();
                if (portWithLinkUp < mvSwitchGetPortsNum())
                {
                    mvOsPrintf("Port %d has link up.\n", portWithLinkUp);
                    break;
                }
                else
                {
                    mvOsPrintf("ERROR Port %d has link up.\n", portWithLinkUp);
                    return 1; /* failure */
                }
            }
            else
            {
                mvOsDelay(10);
            }
        }

        if (linkUpTrial == 200)
        {
            mvOsPrintf("Link is down on all switch ports.\n");
            return 1; /* failure */
        }
    }

    if (funcAlreadyRun == MV_TRUE)
    {
        return 0;
    }

    G_mvSwitchDrtIsInited = MV_TRUE;
    funcAlreadyRun = MV_TRUE;

    ethAct = getenv ("ethact");
    if (msOsStrncmp(ethAct, "ppsdma", strlen("ppsdma")) == 0)
    {
        isMiiMode = MV_FALSE;
    }
    else
    {
        /* PP-MII interface is the default. */
        isMiiMode = MV_TRUE;
    }

    /*
     * Create pool to hold pktInfos when upper layer sends to driver only buffs.
     */
    totalBuffNum = PP_NUM_OF_TX_DESC_TOTAL;
    maxFragsInPkt = 1;
    poolP = mvGndTxPktInfoPoolCreate(totalBuffNum, maxFragsInPkt);
    if (poolP == NULL)
    {
        mvOsPrintf("%s: mvGndTxPktInfoPoolCreate failed.\n", __func__);
        return MV_FAIL;
    }
    drvCtrlP->txPktInfoPoolP = poolP;

    /*
     * Init SwitchGen layer.
     */
    if (mvSwitchInitSwitchGen(isMiiMode) != MV_OK)
    {
        mvOsPrintf("%s: mvSwitchInitSwitchGen.\n", __func__);
        return MV_FAIL;
    }

    if (switchGenStart() != MV_OK)
    {
        mvOsPrintf("%s: switchGenStart failed.\n", __func__);
        return -1;
    }

    if (isMiiMode == MV_TRUE)
    {
        if (mvSwitchEnvMacGet(ethAddr) != MV_OK)
        {
            mvOsPrintf("%s: mvSwitchEnvMacGet failed.\n", __func__);
            return -1;
        }

        if (miiMacAddrSet(ethAddr, 0 /* Queue */) != MV_OK)
        {
            mvOsPrintf("%s: miiMacAddrSet failed.\n", __func__);
            return -1;
        }
    }

    return 0;
}

/*******************************************************************************
 * mvSwitchHalt
 */
int mvSwitchHalt(struct eth_device *dev)
{
    return 0;
}

/*******************************************************************************
 * mvSwitchTx
 */
int mvSwitchTx(struct eth_device *dev, volatile void *buffP, int len)
{
    MV_SWITCH_DRV_CTRL *drvCtrlP = G_mvSwitchDrvCtrlP;

    if (mvSwitchDrvIsInited() == MV_FALSE)
    {
        mvOsPrintf("%s: Switch driver is not initialized.\n", __func__);
        return -1;
    }

    if (switchGenWrapBuffSend((MV_U8 *)buffP,
                              len,
                              MII_DEF_TXQ,
                              drvCtrlP->txPktInfoPoolP,
                              MV_NET_OWN_NET_DRV_RX_REMOVE_TX_ADD_DSA) != MV_OK)
    {
        mvOsPrintf("%s: switchGenWrapBuffSend failed.\n", __func__);
        return -1;
    }

    return 0;
}

/*******************************************************************************
 * switchEndRxPathLock
 */
MV_VOID switchEndRxPathLock(void)
{
    /* In Operating System semaphore should be taken here (or task lock). */
}

/*******************************************************************************
 * switchEndRxPathUnlock
 */
MV_VOID switchEndRxPathUnlock(void)
{
    /* In Operating System semaphore should be taken here (or task lock). */
}

/*******************************************************************************
 * switchEndTxPathLock
 */
MV_VOID switchEndTxPathLock(void)
{
    /* In Operating System semaphore should be taken here (or task lock). */
}

/*******************************************************************************
 * switchEndTxPathUnlock
 */
MV_VOID switchEndTxPathUnlock(void)
{
    /* In Operating System semaphore should be taken here (or task lock). */
}

/*******************************************************************************
 * switchDrvOsPktFree
 */
MV_STATUS switchDrvOsPktFree(MV_VOID *osPkt)
{
    return MV_OK;
}

/*******************************************************************************
 * switchRxReadyIsrCb
 */
MV_VOID switchRxReadyIsrCb(void)
{
    if (switchGenRxJob() != MV_OK)
    {
        while (1)
        {
            mvDebugPrint("switchRxReadyIsrCb: switchGenRxJob failed.\n");
        }
    }
}

/*******************************************************************************
 * switchTxDoneIsrCb
 */
MV_VOID switchTxDoneIsrCb(void)
{
    if (switchGenTxJob() != MV_OK)
    {
        while (1)
        {
            mvDebugPrint("switchTxDoneIsrCb: switchGenTxJob failed.\n");
        }
    }
}

/*******************************************************************************
 * switchDrvIntInitMiiRx
 */
MV_STATUS switchDrvIntInitMiiRx(void (*rxReadyIsrF)(void *))
{
    return MV_OK;
}

/*******************************************************************************
 * switchDrvIntInitMiiTx
 */
MV_STATUS switchDrvIntInitMiiTx(void (*txDoneIsrF) (void *))
{
    return MV_OK;
}

/*******************************************************************************
 * switchDrvIntInitSdmaRx
 */
MV_STATUS switchDrvIntInitSdmaRx(void (*rxReadyIsrF)(void *), MV_U32 dev)
{
    return MV_OK;
}

/*******************************************************************************
 * switchDrvIntInitSdmaTx
 */
MV_STATUS switchDrvIntInitSdmaTx(void (*txDoneIsrF) (void *), MV_U32 dev)
{
    return MV_OK;
}

/*******************************************************************************
 * switchDrvIntEnableMiiRx
 */
MV_STATUS switchDrvIntEnableMiiRx(void)
{
    return MV_OK;
}

/*******************************************************************************
 * switchDrvIntEnableMiiTx
 */
MV_STATUS switchDrvIntEnableMiiTx(void)
{
    return MV_OK;
}

/*******************************************************************************
 * switchDrvIntEnableSdmaRx
 */
MV_STATUS switchDrvIntEnableSdmaRx(MV_U32 dev)
{
    return MV_OK;
}

/*******************************************************************************
 * switchDrvIntEnableSdmaTx
 */
MV_STATUS switchDrvIntEnableSdmaTx(MV_U32 dev)
{
    return MV_OK;
}

/*******************************************************************************
 * switchFwdRxPktToOs
 */
MV_STATUS switchFwdRxPktToOs(MV_GND_PKT_INFO *pktInfoP, MV_U32 rxQ)
{
    MV_GND_BUF_INFO *fragsP;

    fragsP = pktInfoP->pFrags;
    ETH_PKT_CACHE_INV (fragsP->bufVirtPtr, fragsP->dataSize);

    /* U-Boot can serve one-buffer-packet only. */
    NetReceive(fragsP->bufVirtPtr, fragsP->dataSize);

    return MV_OK;
}

/*******************************************************************************
 * mvSwitchRx
 */
int mvSwitchRx(struct eth_device *dev)
{
    MV_GND_PKT_INFO *pktInfoP;
    MV_GND_BUF_INFO *fragsP;

    if (mvSwitchDrvIsInited() == MV_FALSE)
    {
        mvOsPrintf("%s: Switch driver is not initialized.\n", __func__);
        return -1;
    }

    pktInfoP = switchGenGetRxPkt(MII_DEF_RXQ);
    if (pktInfoP == NULL)
    {
        DB(mvOsPrintf("%s: switchGenGetRxPkt has no pkts.\n", __func__));
        return 0;
    }

    fragsP = pktInfoP->pFrags;
    ETH_PKT_CACHE_INV (fragsP->bufVirtPtr, fragsP->dataSize);

    /* U-Boot can serve one-buffer-packet only. */
    NetReceive(fragsP->bufVirtPtr, fragsP->dataSize);

    if (switchGenFreeRxPkt(pktInfoP, 0 /* rxQ */) != MV_OK)
    {
        mvOsPrintf("%s: switchGenFreeRxPkt failed.\n", __func__);
    }

    return 0;
}

