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

#include <common.h>
#include <command.h>
#include <net.h>
#include <malloc.h>
#include "mvSysHwConfig.h"
#include "mv_egiga.h"

#include "sys/mvSysGbe.h"
#include "mvOs.h"
#include "mvSysHwConfig.h"
#include "eth/mvEth.h"
#include "mvPrestera.h"
#include "eth/gbe/mvEthGbe.h"
#include "eth-phy/mvEthPhy.h"
#include "mvBoardEnvLib.h"

/* #define MV_DEBUG */
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

/*
 * Driver internal definitions.
 */
/* use only tx-queue0 and rx-queue0 */
#define EGIGA_DEF_TXQ 0
#define EGIGA_DEF_RXQ 0

/* rx buffer size */
#define ETH_HLEN       14

/* 2(HW hdr) 14(MAC hdr) 4(CRC) 32(extra for cache prefetch) */
#define WRAP           (2 + ETH_HLEN + 4 + 32)

#define MTU            1500
#define RX_BUFFER_SIZE (MTU + WRAP)

/* rings length */
#define EGIGA_TXQ_LEN   20
#define EGIGA_RXQ_LEN   20

#define PHY_MODEL_OFF   4
#define PHY_MODEL_MASK  0x3F

#define PHY_E1111_MODEL 0x0C
#define PHY_E1116_MODEL 0x24

extern MV_BOARD_INFO *boardInfoTbl[];
#define BOARD_INFO(boardId) boardInfoTbl[boardId - BOARD_ID_BASE]

typedef struct _egigaPriv
{
    int port;
    MV_VOID *halPriv;
    MV_U32   rxqCount;
    MV_U32   txqCount;
    MV_BOOL  devInit;
} egigaPriv;

unsigned int egiga_init = 0;

extern unsigned int _G_xCatIsFEBoardType;
extern unsigned int _G_xCatRevision;

/*******************************************************************************
 * mvEgigaEnvMacGet
 */
MV_STATUS mvEgigaEnvMacGet(MV_U8 *ethAddr)
{
    MV_8 *enet_addr;

    enet_addr = getenv ("ethaddr");

    /* Check MAC format. */
    if (!enet_addr || strlen (enet_addr) != 17)
    {
        /* MAC address len (including ':') is 17 chars. */
        mvOsPrintf ("%s: Wrong MAC (%s).\n", __func__, enet_addr);
        return MV_FAIL;
    }

    if (mvMacStrToHex(enet_addr /* string */, ethAddr /* HEX */) != MV_OK)
    {
        mvOsPrintf ("%s: mvMacStrToHex failed.\n", __func__);
        return MV_FAIL;
    }

    return MV_OK;
}

/*******************************************************************************
 * mvEgigaLoad
 */
int mvEgigaLoad(int port)
{
    MV_U8 macAddr[6];

    mvEthForceInitPort (port);

    if (mvEgigaEnvMacGet(macAddr) != MV_OK)
    {
        mvOsPrintf("%s: mvEgigaEnvMacGet failed.\n", __func__);
        return MV_FAIL;
    }

    if (mvEthMacAddrSetSimple (port, macAddr, 0 /* Queue */) != MV_OK)
    {
        mvOsPrintf ("%s: mvEthMacAddrSetSimple failed.\n", __func__);
        return MV_FAIL;
    }

    /*
     * Register device in u-boot eth layer.
     */
    if (mvEgigaDevReg(port) != MV_OK)
    {
        mvOsPrintf("%s: mvEgigaDevReg.\n", __func__);
        return MV_FAIL;
    }

    return 0;
}

/*******************************************************************************
 * mvEgigaDevReg
 */
MV_STATUS mvEgigaDevReg(int port)
{
    struct eth_device *dev = NULL;
    egigaPriv *priv = NULL;

    priv = mvOsCalloc (1, sizeof(egigaPriv));
    if (priv == NULL)
    {
        mvOsPrintf ("%s: alloc failed.\n", __func__);
        return MV_FAIL;
    }

    dev = mvOsCalloc (1, sizeof(struct eth_device));
    if (dev == NULL)
    {
        mvOsPrintf ("%s: alloc failed.\n", __func__);
        return MV_FAIL;
    }

    dev->init   = (void *) mvEgigaInit;
    dev->halt   = (void *) mvEgigaHalt;
    dev->send   = (void *) mvEgigaTx;
    dev->recv   = (void *) mvEgigaRx;
    dev->priv   = priv;
    dev->iobase = 0;
    priv->port  = port;

    if (mvEgigaEnvMacGet(dev->enetaddr) != MV_OK)
    {
        mvOsPrintf("%s: mvEgigaEnvMacGet failed.\n", __func__);
        return MV_FAIL;
    }

    sprintf (dev->name, "egiga%d", port);

    if (eth_register (dev))
    {
        mvOsPrintf ("%s: eth_register() failed.\n", __func__);
        return MV_FAIL;
    }

    return MV_OK;
}

/*******************************************************************************
 * mvEgigaRxFill
 */
MV_PKT_INFO * mvEgigaRxFill(MV_VOID)
{
    MV_BUF_INFO *pBufInfo;
    MV_PKT_INFO *pPktInfo;
    MV_U8 *buf = (MV_U8 *) memalign (32, RX_BUFFER_SIZE);

    if (!buf)
    {
        DB (printf ("failed to alloc buffer\n"));
        return NULL;
    }

    if (((MV_U32) buf) & 0xf)
    {
        printf ("un-align rx buffer %x\n", (MV_U32) buf);
    }

    pPktInfo = malloc (sizeof(MV_PKT_INFO));
    if (pPktInfo == NULL)
    {
        printf ("Error: cannot allocate memory for pktInfo\n");
        free (buf);
        return NULL;
    }

    pBufInfo = malloc (sizeof(MV_BUF_INFO));
    if (pBufInfo == NULL)
    {
        printf ("Error: cannot allocate memory for bufInfo\n");
        free (buf);
        free (pPktInfo);
        return NULL;
    }
    pBufInfo->bufPhysAddr = mvOsIoVirtToPhy (NULL, buf);
    pBufInfo->bufVirtPtr = buf;
    pBufInfo->bufSize = RX_BUFFER_SIZE;
    pBufInfo->dataSize = 0;
    pPktInfo->osInfo = (MV_ULONG) buf;
    pPktInfo->pFrags = pBufInfo;
    pPktInfo->pktSize = RX_BUFFER_SIZE; /* how much to invalidate */
    pPktInfo->numFrags = 1;
    pPktInfo->status = 0;
    pPktInfo->srcIdx = -1;
    return pPktInfo;
}

/*******************************************************************************
 * mvEgigaInit
 */
int mvEgigaInit(struct eth_device *dev, bd_t *p)
{
    egigaPriv *priv = dev->priv;
    MV_ETH_PORT_INIT halInitStruct;
    MV_PKT_INFO *pktInfo;
    MV_STATUS status;
    int i;

    DB (printf ("%s: %s init - ", __FUNCTION__, dev->name));

    /* egiga not ready */
    DB (printf
            ("mvBoardPhyAddrGet()=0x%x , priv->port =0x%x\n",
            mvBoardPhyAddrGet (priv->port), priv->port));

    /* If speed is not auto then link is force */
    if (BOARD_MAC_SPEED_AUTO == mvBoardMacSpeedGet (priv->port))
    {
        /* To support auto-choosing of interface with active link. */
        /* Check Link status on phy */
        if (mvEthPhyCheckLink (mvBoardPhyAddrGet (priv->port)) == MV_FALSE)
        {
            printf ("%s no link\n", dev->name);
            return 1; /* failure */
        }
        else
        {
            DB (printf ("link up\n"));
        }
    }

    /* Use OOB WA. */
    if (mvPpChipIsXCat2() == MV_TRUE && mvChipXCat2IsBGA() == MV_TRUE)
    {
        if (mvBoardChipRevGet() == 0)
        {
            mvPpFtdllWaXcat2();
        }
        else if (mvBoardChipRevGet() >= 1)
        {
            mvPpWriteReg(PP_DEV0, 0x000000B0, 0x00000481);
        }
    }

    egiga_init = 1;

    /* init the hal -- create internal port control structure and descriptor rings,
      */
    /* open address decode windows, disable rx and tx operations. mask interrupts.
       */
    halInitStruct.maxRxPktSize = RX_BUFFER_SIZE;
    halInitStruct.mru = RX_BUFFER_SIZE;
    halInitStruct.rxDefQ = EGIGA_DEF_RXQ;

    halInitStruct.txDescrNum[0] = EGIGA_TXQ_LEN;
    halInitStruct.rxDescrNum[0] = EGIGA_RXQ_LEN;
    halInitStruct.osHandle = NULL;

    /* mvOsPrintf("%s: calling to mvEthPortInit(port=%d).\n", __func__, priv->port); */
    priv->halPriv = mvEthPortInit (priv->port, &halInitStruct);

    if (!priv->halPriv)
    {
        DB (printf ("falied to init eth port (error)\n"));
        goto error;
    }

    /* set new addr in hw */
    if (mvEthMacAddrSet (priv->halPriv, dev->enetaddr, EGIGA_DEF_RXQ) !=
        MV_OK)
    {
        printf ("%s: ethSetMacAddr failed\n", dev->name);
        goto error;
    }

    priv->devInit = MV_TRUE;

    /* fill rx ring with buffers */
    for (i = 0; i < EGIGA_RXQ_LEN; i++)
    {
        pktInfo = mvEgigaRxFill ();
        if (pktInfo == NULL)
        {
            goto error;
        }

        /* give the buffer to hal */
        status =
            mvEthPortRxDone (priv->halPriv, EGIGA_DEF_RXQ, pktInfo);
        if (status == MV_OK)
        {
            priv->rxqCount++;
        }
        else if (status == MV_FULL)
        {
            /* the ring is full */
            priv->rxqCount++;
            DB (printf ("ring full\n"));
            break;
        }
        else
        {
            printf ("error\n");
            goto error;
        }
    }

#ifdef MV_DEBUG
    ethPortQueues (priv->port, EGIGA_DEF_RXQ, EGIGA_DEF_TXQ, 1);

    printf ("%s : after calling ethPortQueues\n", __FUNCTION__);
#endif

    /* start the hal - rx/tx activity */
    /* Check if link is up for 2 Sec */
    for (i = 1; i < 100; i++)
    {
        /* mvOsPrintf("%s: calling to mvEthPortEnable.\n", __func__); */
        status = mvEthPortEnable (priv->halPriv);
        if (status == MV_OK)
        {
            break;
        }
        mvOsDelay (20);
    }

    if (status != MV_OK)
    {
        printf ("%s: %s mvEthPortEnable failed (error)\n",
                __FUNCTION__, dev->name);
        goto error;
    }
#ifdef MV_DEBUG
    ethRegs (priv->port);
    ethPortRegs (priv->port);
    ethPortStatus (priv->port);
    ethPortQueues (priv->port, EGIGA_DEF_RXQ, -1 /*EGIGA_DEF_TXQ */,
                   0);
#endif

    DB (printf ("%s: %s complete ok\n", __FUNCTION__, dev->name));
    return 0;

error:
    if (priv->devInit)
    {
        mvEgigaHalt (dev);
    }
    printf ("%s: %s failed\n", __FUNCTION__, dev->name);
    return 1;
}

/*******************************************************************************
 * mvEgigaHalt
 */
int mvEgigaHalt(struct eth_device *dev)
{
    egigaPriv *priv = dev->priv;
    MV_PKT_INFO *pktInfo;

    DB (printf ("%s: %s halt - ", __FUNCTION__, dev->name));
    if (priv->devInit == MV_TRUE)
    {
        /* stop the port activity, mask all interrupts */
        if (mvEthPortDisable (priv->halPriv) != MV_OK)
        {
            printf ("mvEthPortDisable failed (error)\n");
        }

        /* free the buffs in the rx ring */
        while ((pktInfo =
                    mvEthPortForceRx (priv->halPriv,
                                      EGIGA_DEF_RXQ)) != NULL)
        {
            priv->rxqCount--;
            if (pktInfo->osInfo)
            {
                free ((void *) pktInfo->osInfo);
            }
            else
            {
                printf
                    ("mvEthPortForceRx failed (error)\n");
            }
            if (pktInfo->pFrags)
            {
                free ((void *) pktInfo->pFrags);
            }
            else
            {
                printf
                    ("mvEthPortForceRx failed (error)\n");
            }
            free ((void *) pktInfo);
        }

        /* Clear Cause registers (must come before mvEthPortFinish) */
        MV_REG_WRITE (ETH_INTR_CAUSE_REG
                          (((ETH_PORT_CTRL *) (priv->halPriv))->portNo),
                      0);
        MV_REG_WRITE (ETH_INTR_CAUSE_EXT_REG
                          (((ETH_PORT_CTRL *) (priv->halPriv))->portNo),
                      0);

        /* Clear Cause registers */
        MV_REG_WRITE (ETH_INTR_CAUSE_REG
                          (((ETH_PORT_CTRL *) (priv->halPriv))->portNo),
                      0);
        MV_REG_WRITE (ETH_INTR_CAUSE_EXT_REG
                          (((ETH_PORT_CTRL *) (priv->halPriv))->portNo),
                      0);

        mvEthPortFinish (priv->halPriv);
        priv->devInit = MV_FALSE;
    }
    egiga_init = 0;

    DB (printf ("%s: %s complete\n", __FUNCTION__, dev->name));
    return 0;
}

/*******************************************************************************
 * mvEgigaTx
 */
int mvEgigaTx(struct eth_device *dev, volatile void *buf, int len)
{
    egigaPriv *priv = dev->priv;
    MV_BUF_INFO bufInfo;
    MV_PKT_INFO pktInfo;
    MV_PKT_INFO *pPktInfo;
    MV_STATUS status;

    DB (printf ("mvEgigaTx start\n"));
    /* if no link exist */
    if (!egiga_init)
    {
        return 0;
    }

    pktInfo.osInfo = (MV_ULONG) 0x44CAFE44;
    pktInfo.pktSize = len;
    pktInfo.pFrags = &bufInfo;
    pktInfo.status = 0;
    pktInfo.numFrags = 1;
    bufInfo.bufVirtPtr = (MV_U8 *) buf;
    bufInfo.bufPhysAddr = mvOsIoVirtToPhy (NULL, (void *)buf);
    bufInfo.dataSize = len;

    /* send the packet */
    status = mvEthPortTx (priv->halPriv, EGIGA_DEF_TXQ, &pktInfo);

    if (status != MV_OK)
    {
        if (status == MV_NO_RESOURCE)
        {
            DB (printf ("ring is full (error)\n"));
        }
        else if (status == MV_ERROR)
        {
            printf ("error\n");
        }
        else
        {
            printf ("unrecognize status (error) ethPortSend\n");
        }
        goto error;
    }
    else
    {
        DB (printf ("ok\n"));
    }

    priv->txqCount++;

    /* release the transmitted packet(s) */
    while (1)
    {
        DB (printf ("%s: %s tx-done - ", __FUNCTION__, dev->name));

        /* get a packet */
        pPktInfo = mvEthPortTxDone (priv->halPriv, EGIGA_DEF_TXQ);

        if (pPktInfo != NULL)
        {
            priv->txqCount--;

            /* validate skb */
            if ((pPktInfo != &pktInfo)
                || (pPktInfo->osInfo != 0x44CAFE44))
            {
                printf ("error\n");
                goto error;
            }

            /* handle tx error */
            if (pPktInfo->status & (ETH_ERROR_SUMMARY_BIT))
            {
                printf ("bad status (error)\n");
                goto error;
            }
            DB (printf ("ok\n"));
            break;
        }
        else
        {
            DB (printf ("NULL pPktInfo\n"));
        }
    }

    DB (printf ("%s: %s complete ok\n", __FUNCTION__, dev->name));
    return 0;

error:
    printf ("%s: %s failed\n", __FUNCTION__, dev->name);
    return 1;
}

/*******************************************************************************
 * mvEgigaRx
 */
int mvEgigaRx(struct eth_device *dev)
{
    egigaPriv *priv = dev->priv;
    MV_PKT_INFO *pktInfo;
    MV_STATUS status;

    /* if no link exist */
    if (!egiga_init)
    {
        return 0;
    }

    while (1)
    {
        /* get rx packet from hal */
        pktInfo = mvEthPortRx (priv->halPriv, EGIGA_DEF_RXQ);

        if (pktInfo != NULL)
        {
            priv->rxqCount--;

            /* check rx error status */
            if (pktInfo->status & (ETH_ERROR_SUMMARY_MASK))
            {
                MV_U32 err =
                    pktInfo->
                    status & ETH_RX_ERROR_CODE_MASK;
                if (err == ETH_RX_RESOURCE_ERROR)
                {
                    DB (printf ("(resource error)"));
                }
                else if (err == ETH_RX_MAX_FRAME_LEN_ERROR)
                {
                    DB (printf
                            ("(max frame length error)"));
                }
                else if (err == ETH_RX_OVERRUN_ERROR)
                {
                    DB (printf ("(overrun error)"));
                }
                else if (err == ETH_RX_CRC_ERROR)
                {
                    DB (printf ("(crc error)"));
                }
                else
                {
                    DB (printf ("(unknown error)"));
                    goto error;
                }
                DB (printf ("\n"));
            }
            else
            {
                /* good rx - push the packet up (skip on two first empty bytes) */
                ETH_PKT_CACHE_INV ((void *)pktInfo->osInfo, pktInfo->pFrags->dataSize);
                NetReceive (((MV_U8 *) pktInfo->osInfo) + 2,
                            (int) pktInfo->pFrags->dataSize);
            }

            /* give the buffer back to hal (re-init the buffer address) */
            pktInfo->pktSize = RX_BUFFER_SIZE;  /* how much to invalidate */

            status = mvEthPortRxDone (priv->halPriv, EGIGA_DEF_RXQ, pktInfo);
            if (status == MV_OK)
            {
                priv->rxqCount++;
            }
            else if (status == MV_FULL)
            {
                /* this buffer made the ring full */
                priv->rxqCount++;
                DB (printf ("ring full\n"));
                break;
            }
            else
            {
                printf ("error\n");
                goto error;
            }
        }
        else
        {
            /* no more rx packets ready */
            break;
        }
    }

    return 0;

error:
    return 1;
}

/*******************************************************************************
 * mvBoardEgigaPhySwitchInit
 */
MV_VOID mvBoardEgigaPhySwitchInit(MV_VOID)
{
    MV_U32 boardId;
    MV_U32 ethPortNum = 0;
    MV_U16 reg;

    if ((DB_88F6281A_BP_ID == mvBoardIdGet ()) ||
        (DB_88F6192A_BP_ID == mvBoardIdGet ()))
    {
        if (!(mvBoardIsPortInGmii ()))
        {
            mvEthE1116PhyBasicInit (0);
            mvEthE1116PhyBasicInit (1);
        }
    }
    else if ((DB_88F6190A_BP_ID == mvBoardIdGet ()) ||
             (DB_88F6180A_BP_ID == mvBoardIdGet ()) ||
             (RD_88F6192A_ID == mvBoardIdGet ()) ||
             (RD_88F6190A_ID == mvBoardIdGet ()) ||
             (RD_88F6281A_PCAC_ID == mvBoardIdGet ()))
    {
        mvEthE1116PhyBasicInit (0);
    }
    else if (DB_98DX4122_ID == mvBoardIdGet ()
             || RD_98DX3121_ID == mvBoardIdGet ())
    {
        boardId = mvBoardIdGet ();

        if (!((boardId >= BOARD_ID_BASE)
              && (boardId < MV_MAX_BOARD_ID)))
        {
            return;
        }

        /* first we determine PHY address (by reading valid PHY ID)
         * xCat possible PHY addresses are 0x8 for DB and 0xB for RD
         */
        mvEthPhyRegRead (0x8, 2, &reg);
        if (reg == 0x141)
        {
            BOARD_INFO (boardId)->
            pBoardMacInfo[ethPortNum].boardEthSmiAddr =
                0x8;
        }
        else
        {
            BOARD_INFO (boardId)->
            pBoardMacInfo[ethPortNum].boardEthSmiAddr =
                0xB;
        }

        /* now we determine the PHY ID (model number) and config it */
        mvEthPhyRegRead (mvBoardPhyAddrGet (ethPortNum), 3, &reg);
        reg = (reg >> PHY_MODEL_OFF) & PHY_MODEL_MASK;

        if (reg == PHY_E1111_MODEL)
        {
            mvEthE1111PhyBasicInit (0);
        }
        else if (reg == PHY_E1116_MODEL)
        {
            mvEthE1116PhyBasicInit (0);
        }
        else
        {
            return;
        }
    }
}

