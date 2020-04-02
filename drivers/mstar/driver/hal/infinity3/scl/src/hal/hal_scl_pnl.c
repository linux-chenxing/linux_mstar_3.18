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
#define __HAL_SCL_PNL_C__

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
// Internal Definition
#include "drv_scl_os.h"
#include "hal_utility.h"
#include "hal_scl_reg.h"
#include "hal_scl_util.h"
#include "hal_scl_pnl.h"
#include "hal_scl_pnl_lpll_tbl.h"
#include "drv_scl_os.h"
#include "drv_scl_dbg.h"
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------
#define ENABLE_PNL_UNUSED_FUNCTION 0


//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DCLK_5MHZ           5000000
#define DCLK_10MHZ         10000000
#define DCLK_20MHZ         20000000
#define DCLK_40MHZ         40000000
#define DCLK_80MHZ         80000000

#if defined (SCLOS_TYPE_LINUX_KERNEL)
#define Is_full_pin_TTL()  (Chip_Get_Package_Type()==MS_I3_PACKAGE_BGA_128MB||\
    Chip_Get_Package_Type()==MS_I3_PACKAGE_BGA_256MB||Chip_Get_Package_Type()==MS_I3_PACKAGE_DDR3_1866_128MB\
    ||Chip_Get_Package_Type()== MS_I3_PACKAGE_DDR3_1866_256MB)
#else
#define Is_full_pin_TTL()        1
#endif

#define IsDclkLess5M(Dclk)        ((Dclk) <= DCLK_5MHZ)
#define IsDclk5MTo10M(Dclk)       ((Dclk > DCLK_5MHZ) && (Dclk <= DCLK_10MHZ))
#define IsDclk10MTo20M(Dclk)      ((Dclk > DCLK_10MHZ) && (Dclk <= DCLK_20MHZ))
#define IsDclk20MTo40M(Dclk)      ((Dclk > DCLK_20MHZ) && (Dclk <= DCLK_40MHZ))
#define IsDclk40MTo80M(Dclk)      ((Dclk > DCLK_40MHZ) && (Dclk <= DCLK_80MHZ))
//-------------------------------------------------------------------------------------------------
//  Variable

u32 gu32ttlmode = 1;
u32 gu32JTAGmode;

//-------------------------------------------------------------------------------------------------
//  Functions
//-------------------------------------------------------------------------------------------------
void HalPnlSetRiuBase(u32 u32Riubase)
{
}
void HalPnlSetChiptop(bool bEn)
{
    if(bEn)
    {
        HalUtilityW2BYTEMSK(REG_CHIPTOP_0F_L,0x0040,0x0040);
        //SCL_DBGERR("\n[PNL] ttlmode = ON\n");
    }
    else
    {
        HalUtilityW2BYTEMSK(REG_CHIPTOP_0F_L,0x0000,0x0040);
        //SCL_DBGERR("\n[PNL] ttlmode = OFF\n");
    }
    if(gu32JTAGmode)
    {
        HalUtilityW2BYTEMSK(REG_CHIPTOP_0F_L,0x0002,0x0002);
        //SCL_DBGERR("\n[PNL] JTAGmode = ON\n");
    }
    else
    {
        HalUtilityW2BYTEMSK(REG_CHIPTOP_0F_L,0x0000,0x0002);
        //SCL_DBGERR("\n[PNL] JTAGmode = OFF\n");
    }
}


void HalPnlSetInitY2R(void)
{
    u16 u16Coef[9]={0xcc4,0x950,0x3ffc,0x397e,0x0950,0x3cde,0x3ffe,0x950,0x1024};//hw setting
    HalPnlSetCSCY2REn(1);
    HalPnlSetCSCY2ROffset(0x10,0x80,0x80);
    HalPnlSetCSCY2RCoef(u16Coef);
}
void HalPnlSetVSyncSt(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL2_01_L, u16Val, 0x07FF);
}

void HalPnlSetVSyncEnd(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL2_02_L, u16Val, 0x07FF);
}

void HalPnlSetVfdeSt(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL2_03_L, u16Val, 0x07FF);
}

void HalPnlSetVfdeEnd(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL2_04_L, u16Val, 0x07FF);
}

void HalPnlSetVdeSt(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL2_05_L, u16Val, 0x07FF);
}

void HalPnlSetVdeEnd(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL2_06_L, u16Val, 0x07FF);
}

void HalPnlSetVtt(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL2_07_L, u16Val, 0x07FF);
}

void HalPnlSetHSyncSt(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL2_09_L, u16Val, 0x07FF);
}

void HalPnlSetHSyncEnd(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL2_0A_L, u16Val, 0x07FF);
}

void HalPnlSetHfdeSt(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL2_0B_L, u16Val, 0x07FF);
}

void HalPnlSetHfdeEnd(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL2_0C_L, u16Val, 0x07FF);
}

void HalPnlSetHdeSt(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL2_0D_L, u16Val, 0x07FF);
}

void HalPnlSetHdeEnd(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL2_0E_L, u16Val, 0x07FF);
}

void HalPnlSetHtt(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL2_0F_L, u16Val, 0x07FF);
}

void HalPnlSetFrameColrEn(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL2_10_L, bEn ? BIT8 : 0, BIT8);
}

#if ENABLE_PNL_UNUSED_FUNCTION
void HalPnlSetFrameColor(u32 u32Color)
{
    HalUtilityW4BYTE(REG_SCL2_11_L, u32Color);
}
#endif

u16 HalPnlGetLpllIdx(u64 u64Dclk)
{
    u16 u16Idx = 0;

    if(IsDclkLess5M(u64Dclk))
    {
        u16Idx = E_PNL_SUPPORTED_LPLL_HS_LVDS_CH_5TO5MHZ;
    }
    else if(IsDclk5MTo10M(u64Dclk))
    {
        u16Idx = E_PNL_SUPPORTED_LPLL_HS_LVDS_CH_5TO10MHZ;
    }
    else if(IsDclk10MTo20M(u64Dclk))
    {
        u16Idx = E_PNL_SUPPORTED_LPLL_HS_LVDS_CH_10TO20MHZ;
    }
    else if(IsDclk20MTo40M(u64Dclk))
    {
        u16Idx = E_PNL_SUPPORTED_LPLL_HS_LVDS_CH_20TO40MHZ;
    }
    else if(IsDclk40MTo80M(u64Dclk))
    {
        u16Idx = E_PNL_SUPPORTED_LPLL_HS_LVDS_CH_40TO80MHZ;
    }
    else
    {
        u16Idx = 0xFF;
    }
    return u16Idx;
}

u16 HalPnlGetLpllGain(u16 u16Idx)
{
    return u16LoopGain[u16Idx];
}

u16 HalPnlGetLpllDiv(u16 u16Idx)
{
    return u16LoopDiv[u16Idx];
}

void HalPnlDumpLpllSetting(u16 u16Idx)
{
    u16 u16RegIdx;

    for(u16RegIdx=0; u16RegIdx < LPLL_REG_NUM; u16RegIdx++)
    {

        if(LPLLSettingTBL[u16Idx][u16RegIdx].address == 0xFF)
        {
            DrvSclOsDelayTask(LPLLSettingTBL[u16Idx][u16RegIdx].value);
            continue;
        }

        HalUtilityW2BYTEMSK((REG_SCL_LPLL_BASE | ((u32)LPLLSettingTBL[u16Idx][u16RegIdx].address *2)),
                  LPLLSettingTBL[u16Idx][u16RegIdx].value,
                  LPLLSettingTBL[u16Idx][u16RegIdx].mask);
    }

}

void HalPnlSetLpllSet(u32 u32LpllSet)
{
    u16 u16LpllSet_Lo, u16LpllSet_Hi;
    u16LpllSet_Lo = (u16)(u32LpllSet & 0x0000FFFF);
    u16LpllSet_Hi = (u16)((u32LpllSet & 0x00FF0000) >> 16);
    HalUtilityW2BYTE(REG_SCL_LPLL_48_L, u16LpllSet_Lo);
    HalUtilityW2BYTE(REG_SCL_LPLL_49_L, u16LpllSet_Hi);
}

void HalPnlSetOpenLpllCLK(u8 bLpllClk)
{
    HalUtilityW2BYTEMSK(REG_SCL_CLK_66_L, bLpllClk ? 0x000C : 0x1, 0x000F);
    HalPnlSetChiptop(bLpllClk);
}
// CSC Y2R
void HalPnlSetCSCY2REn(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL2_19_L, bEn ? BIT0 : 0, BIT0);
}

void HalPnlSetCSCY2ROffset(u8 u8Y, u8 u8Cb, u8 u8Cr)
{
    HalUtilityW2BYTEMSK(REG_SCL2_19_L, ((u16)u8Y << 8), 0xFF00);
    HalUtilityW2BYTEMSK(REG_SCL2_1A_L, (((u16)u8Cb << 8)|((u16)u8Cr)), 0xFFFF);
}

void HalPnlSetCSCY2RCoef(u16 *pu16Coef)
{

    u8 u8idx;

    for(u8idx=0; u8idx<9; u8idx++)
    {
        HalUtilityW2BYTEMSK(REG_SCL2_1B_L + (u8idx<<1), pu16Coef[u8idx], 0x3FFF);
    }
}


#undef __HAL_SCL_PNL_C__
