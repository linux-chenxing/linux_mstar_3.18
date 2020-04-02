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
#define __DRV_SCL_PNL_C__


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_scl_os.h"
#include "drv_scl_dbg.h"
#include "hal_scl_util.h"

#include "hal_scl_pnl.h"
#include "drv_scl_pnl.h"
#include "drv_scl_irq_st.h"
#include "drv_scl_irq.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DRV_PNL_DBG(x)
#define DRV_PNL_ERR(x) x
#define ENLPLLCLK   1 //open lpll clk
#define DISLPLLCLK  0 //close lpll clk
#define FACTOR 10*1000000
#define LVDS_MPLL_CLOCK_MHZ 432 //ToDo
#define PNL_CAL_LPLL_DCLK(Vtt,Htt,Freq)   ((Vtt) * (Htt) * (Freq))
#define PNL_CAL_DCLK_FACKTOR(Lpll_Gain)   ((u64)LVDS_MPLL_CLOCK_MHZ * (u64)524288 * (u64)(Lpll_Gain))
#define PNL_CAL_LPLL_SET(DclkFacktor,Dclk_x10,Lpll_Div)      (((DclkFacktor) * (FACTOR)) + (((u64)(Dclk_x10) * (u64)(Lpll_Div)) >> 1))
//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
s32 _PNL_Mutex = -1;
bool gPnlOpen;
u32 gu32Dclk;

extern bool gbclkforcemode;
//-------------------------------------------------------------------------------------------------
//  Functions
//-------------------------------------------------------------------------------------------------
void DrvSclPnlExit(bool bCloseISR)
{
    if(_PNL_Mutex != -1)
    {
        DrvSclOsDeleteMutex(_PNL_Mutex);
        _PNL_Mutex = -1;
    }
    if(bCloseISR)
    {
        DrvSclIrqExit();
    }
}
bool DrvSclPnlInit(DrvSclPnlInitConfig_t *pCfg)
{
    char word[] = {"_PNL_Mutex"};

    if(_PNL_Mutex != -1)
    {
        SCL_DBG(SCL_DBG_LV_PNL(), "[DRVPNL]%s already done\n", __FUNCTION__);
        return TRUE;
    }

    if(DrvSclOsInit() == FALSE)
    {
        SCL_DBG(SCL_DBG_LV_PNL(), "[DRVPNL]%s DrvSclOsInit Fail\n", __FUNCTION__);
        return FALSE;
    }

    _PNL_Mutex = DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO, word, SCLOS_PROCESS_SHARED);

    if (_PNL_Mutex == -1)
    {
        SCL_DBG(SCL_DBG_LV_PNL(), "[DRVPNL]%s create mutex fail\n", __FUNCTION__);
        return FALSE;
    }

    HalPnlSetRiuBase(pCfg->u32RiuBase);
    HalPnlSetInitY2R();
    gPnlOpen = 0;

    return TRUE;
}
bool DrvSclPnlGetPnlOpen(void)
{
    return gPnlOpen ;
}
void DrvSclPnlRelease(void)
{
    if(!gbclkforcemode && gPnlOpen)
    {
        HalPnlSetOpenLpllCLK(DISLPLLCLK);
        gPnlOpen = 0;
    }
}
void DrvSclPnlResume(void)
{
    HalPnlSetInitY2R();
}
u32 DrvSclPnlGetLPLLDclk(void)
{
    return gu32Dclk;
}
bool DrvSclPnlSetTimingConfig(DrvSclPnlTimingConfig_t *pCfg)
{
    SCL_DBG(SCL_DBG_LV_PNL(), "[DRVPNL]%s, V:[%d %d %d %d], H:[%d %d %d %d]\n "
        , __FUNCTION__, pCfg->u16Vsync_St, pCfg->u16Vsync_End, pCfg->u16Vde_St, pCfg->u16Vde_End,
        pCfg->u16Hsync_St, pCfg->u16Hsync_End, pCfg->u16Hde_St, pCfg->u16Hde_End);
    SCL_DBG(SCL_DBG_LV_PNL(), "[DRVPNL]%s, Htt:%d, Vtt:%d, VFreqx10:%d\n "
        ,__FUNCTION__, pCfg->u16Htt, pCfg->u16Vtt, pCfg->u16VFreqx10);


    // LPLL setting
    DrvSclPnlSetLPLLConfig(pCfg);
    //Tgen setting
    HalPnlSetVSyncSt(pCfg->u16Vsync_St);
    HalPnlSetVSyncEnd(pCfg->u16Vsync_End);
    HalPnlSetVfdeSt(pCfg->u16Vfde_St);
    HalPnlSetVfdeEnd(pCfg->u16Vfde_End);
    HalPnlSetVdeSt(pCfg->u16Vde_St);
    HalPnlSetVdeEnd(pCfg->u16Vde_End);
    HalPnlSetVtt(pCfg->u16Vtt);

    HalPnlSetHSyncSt(pCfg->u16Hsync_St);
    HalPnlSetHSyncEnd(pCfg->u16Hsync_End);
    HalPnlSetHfdeSt(pCfg->u16Hfde_St);
    HalPnlSetHfdeEnd(pCfg->u16Hfde_End);
    HalPnlSetHdeSt(pCfg->u16Hde_St);
    HalPnlSetHdeEnd(pCfg->u16Hde_End);
    HalPnlSetHtt(pCfg->u16Htt);
    HalPnlSetFrameColrEn(0);
    HalPnlSetChiptop(1);

    return TRUE;
}

bool DrvSclPnlSetLPLLConfig(DrvSclPnlTimingConfig_t *pCfg)
{
    u16 u16LpllIdx = 0xFF;
    u32 u32Lpll_Gain, u32Lpll_Div, u32Dclk;
    u64 u64Dclk_x10, u64DclkFacktor, u64LpllSet;
    // LPLL setting
    u64Dclk_x10 = PNL_CAL_LPLL_DCLK(pCfg->u16Vtt,pCfg->u16Htt,(pCfg->u16VFreqx10));
    u32Dclk = PNL_CAL_LPLL_DCLK(pCfg->u16Vtt,pCfg->u16Htt,(pCfg->u16VFreqx10/10));
    SCL_DBG(SCL_DBG_LV_PNL(), "[DRVPNL]%sDCLK:%ld\n ", __FUNCTION__, u32Dclk);
    gu32Dclk = u32Dclk;
    u16LpllIdx = HalPnlGetLpllIdx(u32Dclk);
    if(u16LpllIdx == 0xFF)
    {
        DRV_PNL_ERR(sclprintf("[DRVPNL]%s:: LPLL Clk is out of range\n", __FUNCTION__));
        return FALSE;
    }

    u32Lpll_Gain = HalPnlGetLpllGain(u16LpllIdx);
    u32Lpll_Div  = HalPnlGetLpllDiv(u16LpllIdx);

    u64DclkFacktor = PNL_CAL_DCLK_FACKTOR(u32Lpll_Gain);
    u64LpllSet = PNL_CAL_LPLL_SET(u64DclkFacktor,u64Dclk_x10,u32Lpll_Div);

#ifdef SCLOS_TYPE_LINUX_KERNEL
    do_div(u64LpllSet, u64Dclk_x10);
    do_div(u64LpllSet, u32Lpll_Div);
#else
    u64LpllSet = u64LpllSet / u64Dclk_x10;
    u64LpllSet = u64LpllSet / u32Lpll_Div;
#endif

    SCL_DBG(SCL_DBG_LV_PNL(), "[DRVPNL]%s::LpllIdx=%d, Gain:%d, Div:%d, LpllSet=%x\n "
        ,__FUNCTION__, (int)u16LpllIdx, (int)u32Lpll_Gain, (int)u32Lpll_Div, (int)u64LpllSet);

    HalPnlDumpLpllSetting(u16LpllIdx);
    HalPnlSetLpllSet((u32)u64LpllSet);
    if(!gbclkforcemode)
    {
        HalPnlSetOpenLpllCLK(ENLPLLCLK);
        gPnlOpen = 1;
    }

    return TRUE;
}


bool DrvSclPnlSetCscY2RConfig(DrvSclPnlCscY2RConfig_t *pCfg)
{
    HalPnlSetCSCY2REn(pCfg->bEn);
    HalPnlSetCSCY2ROffset(pCfg->u8YOffset, pCfg->u8CbOffset, pCfg->u8CrOffset);
    HalPnlSetCSCY2RCoef(&pCfg->u16Coef[0]);

    return TRUE;
}

#undef __DRV_SCL_PNL_C__
