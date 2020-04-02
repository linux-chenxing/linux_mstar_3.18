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
#define __HAL_SCL_IRQ_C__


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#include "drv_scl_os.h"
#include "hal_scl_reg.h"
#include "hal_scl_util.h"
#include "drv_scl_irq_st.h"
#include "hal_scl_irq.h"
#include "drv_scl_dbg.h"
#include "hal_utility.h"
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Variable

u32 SCLIRQ_RIU_BASE = 0;
bool gblockdnr = 0;


//-------------------------------------------------------------------------------------------------
//  Functions
//-------------------------------------------------------------------------------------------------
void HalSclIrqSetRiuBase(u32 u32Riubase)
{

}
void HalSclIrqSetUtility(void)
{
    HalUtilityInit();
}


u64 HalSclIrqGetFlag(DrvSclIrqScTopType_e enType, u64 u64IRQ)
{
    u32 u32Mask;
    u16 u16Mask;
    u64 u64Flag = 0;
    u32 u32Bank;
    switch(enType)
    {
        case E_DRV_SCLIRQ_SCTOP_0:
            u32Bank = REG_SCL0_13_L;
            break;

        case E_DRV_SCLIRQ_SCTOP_1:
            u32Bank = REG_SCL0_51_L;
            break;

        case E_DRV_SCLIRQ_SCTOP_2:
            u32Bank = REG_SCL0_5D_L;
            break;
        default:
            u32Bank = REG_SCL0_13_L;
            break;
    }
    if(u64IRQ & 0x00000000FFFFFFFF)
    {
        u32Mask =  (u32)(u64IRQ & 0xFFFFFFFF);
        u64Flag |= (u64)(HalUtilityR4BYTEDirect(u32Bank) & u32Mask);
    }

    if(u64IRQ & 0x0000FFFF00000000)
    {
        u16Mask =  (u16)((u64IRQ & 0x0000FFFF00000000)>>32);
        u64Flag |= (((u64)(HalUtilityR2BYTEDirect(u32Bank+4) & u16Mask))<<32);

    }

    return u64Flag;
}
u16 HalSclIrqCmdqGetClearIrq(void)
{
	return HalUtilityR2BYTEDirect(REG_SCL0_1E_L);
}


void HalSclIrqSetClear(DrvSclIrqScTopType_e enType, u64 u64IRQ, bool bEn)
{
    u16 u16Mask;
    u32 u32Bank;
    switch(enType)
    {
        case E_DRV_SCLIRQ_SCTOP_0:
            u32Bank = REG_SCL0_10_L;
            break;

        case E_DRV_SCLIRQ_SCTOP_1:
            u32Bank = REG_SCL0_4E_L;
            break;

        case E_DRV_SCLIRQ_SCTOP_2:
            u32Bank = REG_SCL0_5A_L;
            break;
        default:
            u32Bank = REG_SCL0_10_L;
            break;
    }

    if(u64IRQ & 0x0000FFFF)
    {
        u16Mask =  (u16)(u64IRQ & 0x0000FFFF);
        HalUtilityW2BYTEMSKDirect(u32Bank, bEn ? u16Mask : 0, u16Mask);
    }

    if(u64IRQ & 0xFFFF0000)
    {
        u16Mask =  (u16)((u64IRQ & 0xFFFF0000)>>16);
        HalUtilityW2BYTEMSKDirect(u32Bank+2, bEn ? u16Mask : 0, u16Mask);
    }

    if(u64IRQ & 0xFFFF00000000)
    {
        u16Mask =  (u16)((u64IRQ & 0xFFFF00000000)>>32);
        HalUtilityW2BYTEMSKDirect(u32Bank+4, bEn ? u16Mask : 0, u16Mask);
    }
}
u16 HalSclIrqGetRegVal(u32 u32Reg)
{
    return HalUtilityR2BYTEDirect(u32Reg);
}
void HalSclIrqSetReg(u32 u32Reg,u16 u16Val,u16 u16Mask)
{
    HalUtilityW2BYTEMSKDirect(u32Reg,u16Val,u16Mask);
}
void HalSclIrqSetDNRBypass(bool bEn)
{
    static u16 u16open = 0;
    if(bEn)
    {
        u16open = HalUtilityR2BYTEDirect(REG_VIP_MCNR_01_L);
    }
    if(!gblockdnr)
    {
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_ELSE, "[SCLIRQ]DNR bypass:%hhd\n", bEn);
        HalUtilityW2BYTEMSK(REG_VIP_MCNR_01_L, bEn ? 0 : u16open, BIT1);//IOenable
    }
}
void HalSclIrqSetDNRLock(bool bEn)
{
    gblockdnr = bEn;
}

void HalSclIrqSetMask(DrvSclIrqScTopType_e enType, u64 u64IRQ, bool bMask)
{
    u16 u16val;
    u16 u16Mask;
    u32 u32Bank;
    switch(enType)
    {
        case E_DRV_SCLIRQ_SCTOP_0:
            u32Bank = REG_SCL0_08_L;
            break;

        case E_DRV_SCLIRQ_SCTOP_1:
            u32Bank = REG_SCL0_48_L;
            break;

        case E_DRV_SCLIRQ_SCTOP_2:
            u32Bank = REG_SCL0_54_L;
            break;
        default:
            u32Bank = REG_SCL0_08_L;
            break;
    }
    if(u64IRQ & 0x0000FFFF)
    {
        u16Mask =  (u16)(u64IRQ & 0x0000FFFF);
        u16val = bMask ? u16Mask : 0;
        HalUtilityW2BYTEMSK(u32Bank, u16val, u16Mask);
    }

    if(u64IRQ & 0xFFFF0000)
    {
        u16Mask =  (u16)((u64IRQ & 0xFFFF0000)>>16);
        u16val = bMask ? u16Mask : 0;
        HalUtilityW2BYTEMSK(u32Bank+2, u16val, u16Mask);
    }

    if(u64IRQ & 0xFFFF00000000)
    {
        u16Mask =  (u16)((u64IRQ & 0xFFFF00000000)>>32);
        u16val = bMask ? u16Mask : 0;
        HalUtilityW2BYTEMSK(u32Bank+4, u16val, u16Mask);
    }
}
#undef __HAL_SCL_IRQ_C__
