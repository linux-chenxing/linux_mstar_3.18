//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
#define __DRV_SCL_MGWIN_C__


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "ms_platform.h"
#include "ms_types.h"
#include "cam_os_wrapper.h"
#include "drv_scl_mgwin_dbg.h"
#include "hal_scl_mgwin_util.h"

#include "drv_scl_mgwin_verchk.h"
#include "drv_scl_mgwin.h"
#include "hal_scl_mgwin.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define CHECK_ALIGN(value, align)  (value & (align -1))


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_DRV_SCLMGWIN_MUX_CMD_ONOFF,
    E_DRV_SCLMGWIN_MUX_CMD_PATH,
}DrvSclMgwinMuxCmdType_e;

typedef struct
{
    DrvSclMgwinMemFormat_e enMemFormat;
    DrvSclMgwinWindowConfig_t stWin;
    u32 u32BaseAddr;
    u32 u32Pitch;
    bool bEn;
}DrvSclMgwinSubWinContain_t;


typedef struct
{
    DrvSclMgwinWinIdType_e enWinId;
    DrvSclMgwinWindowConfig_t stFrameWin;
    DrvSclMgwinFramePath_e enPath;
    bool bFrameOnOff;
    DrvSclMgwinSubWinContain_t stSubWinContain[DRV_SCLMGWIN_SUBWIN_NUM];
}DrvSclMgwinContain_t;

typedef struct
{
    bool bUsed;
    DrvSclMgwinContain_t stContain[DRV_SCLMGWIN_NUM];
}DrvSclMgwinCtxConfig_t;

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
DrvSclMgwinCtxConfig_t _gstSclMgwinCtxCfg[DRV_SCLMGWIN_INSTANT_MAX];
bool bDrvSclMgwinInit = FALSE;
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
bool _DrvSclMgwinSetMuxConfig(DrvSclMgwinWinIdType_e enWinId, DrvSclMgwinCtxConfig_t *pSclMgwinCtxCfg, DrvSclMgwinMuxCmdType_e enCmd)
{
    DrvSclMgwinFramePath_e enMgwin1Path, enMgwin2Path;
    bool bMgwin1En, bMgwin2En;
    bool bRet = TRUE;

    enMgwin1Path = pSclMgwinCtxCfg->stContain[E_DRV_SCLMGWIN_WIN_ID_1].enPath;
    enMgwin2Path = pSclMgwinCtxCfg->stContain[E_DRV_SCLMGWIN_WIN_ID_2].enPath;

    bMgwin1En = pSclMgwinCtxCfg->stContain[E_DRV_SCLMGWIN_WIN_ID_1].bFrameOnOff;
    bMgwin2En = pSclMgwinCtxCfg->stContain[E_DRV_SCLMGWIN_WIN_ID_2].bFrameOnOff;

    if(enCmd == E_DRV_SCLMGWIN_MUX_CMD_ONOFF && pSclMgwinCtxCfg->stContain[enWinId].bFrameOnOff == FALSE)
    {
        HalSclMgwinSetFrameOnOff(enWinId, FALSE);
    }
    else
    {
        if((enMgwin1Path == E_DRV_SCLMGWIN_PATH_SC1 && enMgwin2Path != enMgwin1Path) ||
           (enMgwin1Path == E_DRV_SCLMGWIN_PATH_SC2 && enMgwin2Path != enMgwin1Path) )
        {

            SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN() & EN_DBGMG_SCLMGWINLEVEL_FRAME,
                "[MGWIN_%d], En=1, Path(%d %d), Sc1ForSc0(0, 0), LayOff:(1, 1)\n", enWinId, enMgwin1Path, enMgwin2Path);

            HalSclMgwinSetFrameTrigSel(enWinId, pSclMgwinCtxCfg->stContain[enWinId].enPath);
            HalSclMgwinSetFrameOnOff(enWinId, TRUE);

            HalSclMgwinSetFrameSc1ForSc0En(E_DRV_SCLMGWIN_WIN_ID_1, FALSE);
            HalSclMgwinSetFrameLayOff(E_DRV_SCLMGWIN_WIN_ID_1, TRUE);

            HalSclMgwinSetFrameSc1ForSc0En(E_DRV_SCLMGWIN_WIN_ID_2, FALSE);
            HalSclMgwinSetFrameLayOff(E_DRV_SCLMGWIN_WIN_ID_2, TRUE);
        }
        else if(enMgwin1Path == E_DRV_SCLMGWIN_PATH_SC1 && enMgwin2Path == E_DRV_SCLMGWIN_PATH_SC1)
        {
            SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN() & EN_DBGMG_SCLMGWINLEVEL_FRAME,
                "[MGWIN_%d], En(1, 0) Path(%d %d), Sc1ForSc0(1, 0), LayOff:(0, 0)\n", enWinId, enMgwin1Path, enMgwin2Path);

            HalSclMgwinSetFrameTrigSel(enWinId, pSclMgwinCtxCfg->stContain[enWinId].enPath);

            HalSclMgwinSetFrameSc1ForSc0En(E_DRV_SCLMGWIN_WIN_ID_1, TRUE);
            HalSclMgwinSetFrameLayOff(E_DRV_SCLMGWIN_WIN_ID_1, FALSE);
            HalSclMgwinSetFrameOnOff(E_DRV_SCLMGWIN_WIN_ID_1, TRUE);

            HalSclMgwinSetFrameSc1ForSc0En(E_DRV_SCLMGWIN_WIN_ID_2, FALSE);
            HalSclMgwinSetFrameLayOff(E_DRV_SCLMGWIN_WIN_ID_2, FALSE);
            HalSclMgwinSetFrameOnOff(E_DRV_SCLMGWIN_WIN_ID_2, FALSE);
        }
        else if(enMgwin1Path == E_DRV_SCLMGWIN_PATH_NUM || enMgwin2Path == E_DRV_SCLMGWIN_PATH_NUM)
        {
            SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN() & EN_DBGMG_SCLMGWINLEVEL_FRAME, "[MGWIN_%d], Path:(%d, %d)\n",
                enWinId,  enMgwin1Path, enMgwin2Path);

        }
        else
        {
            SCL_ERR("[SCLMGWIN] %s %d, Path is  illegal, Mgwin1=%d, Mgwin2=%d\n", __FUNCTION__, __LINE__, enMgwin1Path, enMgwin2Path);
            bRet = FALSE;
        }
    }

    return bRet;
}



bool _DrvSclMgwinCheckSubWinOverlay(DrvSclMgwinContain_t *pMgwinContain)
{
    u8 i, j;
    u16 u16Xstart1, u16Ystart1, u16Xend1, u16Yend1;
    u16 u16Xstart2, u16Ystart2, u16Xend2, u16Yend2;
    bool bOvelay = 0;

    for(i=0; i<DRV_SCLMGWIN_SUBWIN_NUM; i++)
    {
        if(pMgwinContain->stSubWinContain[i].bEn == 0)
        {
            continue;
        }

        u16Xstart1 = pMgwinContain->stSubWinContain[i].stWin.u16X;
        u16Xend1   = pMgwinContain->stSubWinContain[i].stWin.u16X + pMgwinContain->stSubWinContain[i].stWin.u16Width - 1;
        u16Ystart1 = pMgwinContain->stSubWinContain[i].stWin.u16Y;
        u16Yend1  =  pMgwinContain->stSubWinContain[i].stWin.u16Y + pMgwinContain->stSubWinContain[i].stWin.u16Height - 1;

        for(j=i+1; j<DRV_SCLMGWIN_SUBWIN_NUM; j++)
        {
            if(pMgwinContain->stSubWinContain[j].bEn == 0)
            {
                continue;
            }

            u16Xstart2 = pMgwinContain->stSubWinContain[j].stWin.u16X;
            u16Xend2   = pMgwinContain->stSubWinContain[j].stWin.u16X + pMgwinContain->stSubWinContain[j].stWin.u16Width - 1;
            u16Ystart2 = pMgwinContain->stSubWinContain[j].stWin.u16Y;
            u16Yend2  =  pMgwinContain->stSubWinContain[j].stWin.u16Y + pMgwinContain->stSubWinContain[j].stWin.u16Height - 1;

            bOvelay = ( u16Xstart1 <= u16Xend2 && u16Xstart2 <= u16Xend1 && u16Ystart1 >= u16Yend2 && u16Yend2 >= u16Yend1) ?  1 : 0;

            if(bOvelay)
            {
                SCL_ERR("MGwin_%d, SubWin(%d,%d): (%d %d %d %d) (%d %d %d %d)\n",
                    pMgwinContain->enWinId, i, j,
                    u16Xstart1, u16Xend1, u16Ystart1, u16Yend1,
                    u16Xstart2, u16Xend2, u16Ystart2, u16Yend2);
                break;
            }

        }

    }

    return (bOvelay);
}



//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
bool DrvSclMgwinInit(void)
{
    u16 i, j;

    if( bDrvSclMgwinInit == TRUE)
    {
        SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN(), "[SCLMGWIN] %s %d, Already Init\n", __FUNCTION__, __LINE__);
        return TRUE;
    }

    memset(_gstSclMgwinCtxCfg, 0, sizeof(DrvSclMgwinCtxConfig_t)*DRV_SCLMGWIN_INSTANT_MAX);
    for(i=0; i<DRV_SCLMGWIN_INSTANT_MAX; i++)
    {
        for(j=0; j<DRV_SCLMGWIN_NUM; j++)
        {
            _gstSclMgwinCtxCfg[i].stContain[j].enPath = E_DRV_SCLMGWIN_PATH_NUM;
        }
    }

    HalSclMgwinInit(E_DRV_SCLMGWIN_WIN_ID_1);
    HalSclMgwinInit(E_DRV_SCLMGWIN_WIN_ID_2);

    bDrvSclMgwinInit = TRUE;

    return TRUE;
}


void *DrvSclMgwinCtxAllocate(void)
{
    s16 i;
    s16 s16Idx = -1;

    for(i=0; i<DRV_SCLMGWIN_INSTANT_MAX; i++)
    {
        if(_gstSclMgwinCtxCfg[i].bUsed == FALSE)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        return NULL;
    }
    else
    {
        return (void *)&_gstSclMgwinCtxCfg[s16Idx];
    }
}

bool DrvSclMgwinCtxFree(void *pCtx)
{
    DrvSclMgwinCtxConfig_t *pSclMgwinCtx;

    pSclMgwinCtx = (DrvSclMgwinCtxConfig_t *)pCtx;

    if(pSclMgwinCtx)
    {
        u16 i;
        memset(pSclMgwinCtx, 0, sizeof(DrvSclMgwinCtxConfig_t));

        for(i=0; i<DRV_SCLMGWIN_NUM; i++)
        {
            pSclMgwinCtx->stContain[i].enPath = E_DRV_SCLMGWIN_PATH_NUM;
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool DrvSclMgwinSetFrameConfig(void *pCtx, DrvSclMgwinFrameConfig_t *pstFrameCfg)
{
    DrvSclMgwinCtxConfig_t *pSclMgwinCtxCfg = (DrvSclMgwinCtxConfig_t *)pCtx;
    DrvSclMgwinContain_t *pMgwinContain =NULL;

    if(pSclMgwinCtxCfg == NULL || pstFrameCfg == NULL)
    {
        SCL_ERR("[SCLMGWIN] %s %d, parame is NULL\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN() & EN_DBGMG_SCLMGWINLEVEL_FRAME, "[MGWIN_%d], Frame Size(%d, %d, %d, %d)\n",
        pstFrameCfg->enWinId,
        pstFrameCfg->stWin.u16X, pstFrameCfg->stWin.u16Y, pstFrameCfg->stWin.u16Width, pstFrameCfg->stWin.u16Height);

    //Ctx Data saving
    pMgwinContain = &pSclMgwinCtxCfg->stContain[pstFrameCfg->enWinId];
    pMgwinContain->stFrameWin.u16X      = pstFrameCfg->stWin.u16X;
    pMgwinContain->stFrameWin.u16Y      = pstFrameCfg->stWin.u16Y;
    pMgwinContain->stFrameWin.u16Width  = pstFrameCfg->stWin.u16Width;
    pMgwinContain->stFrameWin.u16Height = pstFrameCfg->stWin.u16Height;

    HalSclMgwinSetFrameSize(pstFrameCfg->enWinId, pstFrameCfg->stWin.u16Width, pstFrameCfg->stWin.u16Height);

    return TRUE;
}

bool DrvSclMgwinSetFrameOnOffConfig(void *pCtx, DrvSclMgwinFrameOnOffConfig_t *pstFrameOnOffCfg)
{
    DrvSclMgwinCtxConfig_t *pSclMgwinCtxCfg = (DrvSclMgwinCtxConfig_t *)pCtx;
    DrvSclMgwinContain_t *pMgwinContain =NULL;

    if(pSclMgwinCtxCfg == NULL || pstFrameOnOffCfg == NULL)
    {
        SCL_ERR("[SCLMGWIN] %s %d, parame is NULL\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN() & EN_DBGMG_SCLMGWINLEVEL_FRAME, "[MGWIN_%d], Frame bEn:%d\n",
        pstFrameOnOffCfg->enWinId,  pstFrameOnOffCfg->bEn);

    //Ctx Data saving
    pMgwinContain = &pSclMgwinCtxCfg->stContain[pstFrameOnOffCfg->enWinId];
    pMgwinContain->bFrameOnOff = pstFrameOnOffCfg->bEn;

    _DrvSclMgwinSetMuxConfig(pstFrameOnOffCfg->enWinId, pSclMgwinCtxCfg, E_DRV_SCLMGWIN_MUX_CMD_ONOFF);

    return TRUE;
}

bool DrvSclMgwinSetFramePathConfig(void *pCtx, DrvSclMgwinFramePathConfig_t *pstFramePathCfg)
{
    DrvSclMgwinCtxConfig_t *pSclMgwinCtxCfg = (DrvSclMgwinCtxConfig_t *)pCtx;
    DrvSclMgwinContain_t *pMgwinContain =NULL;

    if(pSclMgwinCtxCfg == NULL || pstFramePathCfg == NULL)
    {
        SCL_ERR("[SCLMGWIN] %s %d, parame is NULL\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    //Ctx Data saving
    pMgwinContain = &pSclMgwinCtxCfg->stContain[pstFramePathCfg->enWinId];
    pMgwinContain->enPath = pstFramePathCfg->enPathType;

    return _DrvSclMgwinSetMuxConfig(pstFramePathCfg->enWinId, pSclMgwinCtxCfg, E_DRV_SCLMGWIN_MUX_CMD_PATH);
}


bool DrvSclMgwinSetSubWinConfig(void *pCtx, DrvSclMgwinSubWinConfig_t *pstSubWinCfg)
{
    DrvSclMgwinCtxConfig_t *pSclMgwinCtxCfg = (DrvSclMgwinCtxConfig_t *)pCtx;
    DrvSclMgwinSubWinContain_t *pMgwinSubWinContain;

    if(pSclMgwinCtxCfg == NULL || pstSubWinCfg == NULL)
    {
        SCL_ERR("[SCLMGWIN] %s %d, parame is NULL\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN() & EN_DBGMG_SCLMGWINLEVEL_SUBWIN, "[MGWIN_%d_%d], SubWin(%d, %d, %d, %d)\n",
        pstSubWinCfg->enWinId, pstSubWinCfg->enSubWinId,
        pstSubWinCfg->stWin.u16X, pstSubWinCfg->stWin.u16Y, pstSubWinCfg->stWin.u16Width, pstSubWinCfg->stWin.u16Height);


    if(CHECK_ALIGN(pstSubWinCfg->stWin.u16X, 2) || CHECK_ALIGN(pstSubWinCfg->stWin.u16Y, 2) ||
       CHECK_ALIGN(pstSubWinCfg->stWin.u16Width, 2) || CHECK_ALIGN(pstSubWinCfg->stWin.u16Height, 2))
    {
        SCL_ERR("[SCLMGWIN] %s %d, [MGWIN_%d_%d] Error for H/V must 2 alignment (%d %d %d %d)\n",
            __FUNCTION__, __LINE__,
            pstSubWinCfg->enWinId, pstSubWinCfg->enSubWinId,
            pstSubWinCfg->stWin.u16X, pstSubWinCfg->stWin.u16Y,
            pstSubWinCfg->stWin.u16Width, pstSubWinCfg->stWin.u16Height);
        return FALSE;
    }


    //Ctx Data saving
    pMgwinSubWinContain = &pSclMgwinCtxCfg->stContain[pstSubWinCfg->enWinId].stSubWinContain[pstSubWinCfg->enSubWinId];
    pMgwinSubWinContain->stWin.u16X     = pstSubWinCfg->stWin.u16X;
    pMgwinSubWinContain->stWin.u16Y     = pstSubWinCfg->stWin.u16Y;
    pMgwinSubWinContain->stWin.u16Width = pstSubWinCfg->stWin.u16Width;
    pMgwinSubWinContain->stWin.u16Height = pstSubWinCfg->stWin.u16Height;


    HalSclMgwinSetSubWinSize(pstSubWinCfg->enWinId,
                             pstSubWinCfg->enSubWinId,
                             pstSubWinCfg->stWin.u16X,
                             pstSubWinCfg->stWin.u16X + pstSubWinCfg->stWin.u16Width -1,
                             pstSubWinCfg->stWin.u16Y,
                             pstSubWinCfg->stWin.u16Y + pstSubWinCfg->stWin.u16Height -1);


    return TRUE;
}

bool DrvSclMgwinSetSubWinOnOffConfig(void *pCtx, DrvSclMgwinSubWinOnOffConfig_t *pstSubWinOnOffCfg)
{
    DrvSclMgwinCtxConfig_t *pSclMgwinCtxCfg = (DrvSclMgwinCtxConfig_t *)pCtx;
    DrvSclMgwinSubWinContain_t *pMgwinSubWinContain;

    if(pSclMgwinCtxCfg == NULL || pstSubWinOnOffCfg == NULL)
    {
        SCL_ERR("[SCLMGWIN] %s %d, parame is NULL\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN() & EN_DBGMG_SCLMGWINLEVEL_SUBWIN, "[MGWIN_%d_%d], bEn=%d\n",
        pstSubWinOnOffCfg->enWinId, pstSubWinOnOffCfg->enSubWinId,
        pstSubWinOnOffCfg->bEnable);

    //Ctx Data saving
    pMgwinSubWinContain = &pSclMgwinCtxCfg->stContain[pstSubWinOnOffCfg->enWinId].stSubWinContain[pstSubWinOnOffCfg->enSubWinId];
    pMgwinSubWinContain->bEn = pstSubWinOnOffCfg->bEnable;

    HalSclMgwinSetSubWinOnOff(pstSubWinOnOffCfg->enWinId, pstSubWinOnOffCfg->enSubWinId, pMgwinSubWinContain->bEn);
    return TRUE;
}

bool DrvSclMgwinSetSubWinFlipConfig(void *pCtx, DrvSclMgwinSubWinFlipConfig_t *pSubWinFlipCfg)
{
    DrvSclMgwinCtxConfig_t *pSclMgwinCtxCfg = (DrvSclMgwinCtxConfig_t *)pCtx;
    DrvSclMgwinSubWinContain_t *pMgwinSubWinContain;

    if(pSclMgwinCtxCfg == NULL || pSubWinFlipCfg == NULL)
    {
        SCL_ERR("[SCLMGWIN] %s %d, parame is NULL\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    if(CHECK_ALIGN(pSubWinFlipCfg->u32Pitch, 16))
    {
        SCL_ERR("[SCLMGWIN] %s %d, [Mgwin_%d_%d], Pitch must be 16 alighment\n",
            __FUNCTION__, __LINE__, pSubWinFlipCfg->enWinId, pSubWinFlipCfg->enSubWinId);

        return FALSE;
    }

    SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN() & EN_DBGMG_SCLMGWINLEVEL_SUBWIN, "[MGWIN_%d_%d], Mem=%d, Base=%x, Pitch=%x\n",
        pSubWinFlipCfg->enWinId, pSubWinFlipCfg->enSubWinId,
        pSubWinFlipCfg->enMemFmt,
        pSubWinFlipCfg->u32BaseAddr, pSubWinFlipCfg->u32Pitch);

    //Ctx Data saving
    pMgwinSubWinContain = &pSclMgwinCtxCfg->stContain[pSubWinFlipCfg->enWinId].stSubWinContain[pSubWinFlipCfg->enSubWinId];
    pMgwinSubWinContain->u32BaseAddr = pSubWinFlipCfg->u32BaseAddr;
    pMgwinSubWinContain->enMemFormat = pSubWinFlipCfg->enMemFmt;
    pMgwinSubWinContain->u32Pitch    = pSubWinFlipCfg->u32Pitch;
    pMgwinSubWinContain->u32BaseAddr = pSubWinFlipCfg->u32BaseAddr;

    HalSclMgwinSetSubWinMemFmt(pSubWinFlipCfg->enWinId, pSubWinFlipCfg->enSubWinId, pSubWinFlipCfg->enMemFmt, FALSE);
    HalSclMgwinSetSubWinBaseAddr(pSubWinFlipCfg->enWinId, pSubWinFlipCfg->enSubWinId, pSubWinFlipCfg->u32BaseAddr);
    HalSclMgwinSetSubWinPitch(pSubWinFlipCfg->enWinId, pSubWinFlipCfg->enSubWinId, pSubWinFlipCfg->u32Pitch);
    return TRUE;
}

bool DrvSclMgwinSetDbfConfig(void *pCtx, DrvSclMgwinDbfConfig_t *pstDbfCfg)
{
    bool bRet = TRUE;
    DrvSclMgwinCtxConfig_t *pSclMgwinCtxCfg = (DrvSclMgwinCtxConfig_t *)pCtx;
    if(pSclMgwinCtxCfg == NULL || pstDbfCfg == NULL)
    {
        SCL_ERR("[SCLMGWIN] %s %d, parame is NULL\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    if(_DrvSclMgwinCheckSubWinOverlay( &pSclMgwinCtxCfg->stContain[pstDbfCfg->enWinId]))
    {
        SCL_ERR("[SCLMGWIN] SubWin Overlay \n");
        bRet = FALSE;
    }
    else
    {
        if(pstDbfCfg->enWinId == E_DRV_SCLMGWIN_WIN_ID_1 || pstDbfCfg->enWinId == E_DRV_SCLMGWIN_WIN_ID_2)
        {
            HalSclMgwinSetFrameDbfSwTrig(pstDbfCfg->enWinId);
            bRet = TRUE;
        }
        else
        {
            SCL_ERR("[SCLMGWIN] %s %d, Frame Id is not correctL\n", __FUNCTION__, __LINE__);
            bRet = FALSE;
        }
    }

    return bRet;
}

bool DrvSclMgwinGetDbfConfig(void *pCtx, DrvSclMgwinDbfConfig_t *pstDbfCfg)
{
    bool bRet = TRUE;
    DrvSclMgwinCtxConfig_t *pSclMgwinCtxCfg = (DrvSclMgwinCtxConfig_t *)pCtx;
    if(pSclMgwinCtxCfg == NULL || pstDbfCfg == NULL)
    {
        SCL_ERR("[SCLMGWIN] %s %d, parame is NULL\n", __FUNCTION__, __LINE__);
        return FALSE;
    }


    if(pstDbfCfg->enWinId == E_DRV_SCLMGWIN_WIN_ID_1 || pstDbfCfg->enWinId == E_DRV_SCLMGWIN_WIN_ID_2)
    {
        pstDbfCfg->bTrigger = HalSclMgwinGetFrameDbfSwTrig(pstDbfCfg->enWinId) == 0 ? TRUE : FALSE;
        bRet = TRUE;
    }
    else
    {
        SCL_ERR("[SCLMGWIN] %s %d, Frame Id is not correctL\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

bool DrvSclMgwinSetRegister(u32 u32Addr, u16 u16Val, u16 u16Msk)
{
    HalSclMgwinSetRegister(u32Addr, u16Val, u16Msk);
    return TRUE;
}


