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
#define HAL_SCLDMA_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#include "drv_scl_os.h"
#include "hal_utility.h"
// Internal Definition
#include "hal_scl_reg.h"
#include "hal_scl_util.h"
#include "drv_scl_dma_st.h"
#include "hal_scl_dma.h"
#include "drv_scl_dbg.h"
#include "drv_scl_irq_st.h"
#include "drv_scl_irq.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HAL_SCLDMA_ERR(x)  x
#define DISABLE_CLK 0x1
#define LOW_CLK 0x4
#define IsClkIncrease(height,width,rate)    (((height) > 720 || (width) > 1280)&& (rate) < 172000000)
#define IsClkDecrease(height,width,rate)    (((height) <= 720 && (width) <= 1280)&& (rate) >= 172000000)
//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u32 SCLDMA_RIU_BASE = 0;
u16 u16gheight[2]  = {1080,720};
u16 u16gwidth[2]  = {1920,1280};

//-------------------------------------------------------------------------------------------------
//  Functions
//-------------------------------------------------------------------------------------------------
void HalSclDmaSetRiuBase(u32 u32riubase)
{
    SCLDMA_RIU_BASE = u32riubase;
}

void HalSclDmaSetSwReset(void)
{
    HalUtilityW2BYTEMSK(REG_SCL0_01_L,0x1E,0x1E);
    HalUtilityW2BYTEMSK(REG_SCL0_01_L,0,0x1E);
}
void _HalSclDmaSetRegLen(DrvSclDmaClientType_e enSCLDMA_ID, u8 u8value)
{
    if(enSCLDMA_ID == E_DRV_SCLDMA_1_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_19_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_1_SNP_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_49_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_1_IMI_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_49_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_2_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_61_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_2_FRM2_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_61_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_2_IMI_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_79_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_3_FRM_R)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_31_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_3_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_19_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_4_FRM_R)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_31_L, ((u16)u8value<<8), 0xFF00);
    }
    else
    {
        HAL_SCLDMA_ERR(sclprintf("%s %d, wrong enSCLDMA_ID: %d\n", __FUNCTION__, __LINE__, enSCLDMA_ID));
    }
}
void _HalSclDmaSetRegTh(DrvSclDmaClientType_e enSCLDMA_ID, u8 u8value)
{
    if(enSCLDMA_ID == E_DRV_SCLDMA_1_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_19_L, ((u16)u8value), 0x00FF);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_1_SNP_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_49_L, ((u16)u8value), 0x00FF);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_1_IMI_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_49_L, ((u16)u8value), 0x00FF);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_2_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_61_L, ((u16)u8value), 0x00FF);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_2_FRM2_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_61_L, ((u16)u8value), 0x00FF);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_2_IMI_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_79_L, ((u16)u8value), 0x00FF);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_3_FRM_R)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_31_L, ((u16)u8value), 0x00FF);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_3_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_19_L, ((u16)u8value), 0x00FF);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_4_FRM_R)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_31_L, ((u16)u8value), 0x00FF);
    }
    else
    {
        HAL_SCLDMA_ERR(sclprintf("%s %d, wrong enSCLDMA_ID: %d\n", __FUNCTION__, __LINE__, enSCLDMA_ID));
    }
}
void _HalSclDmaSetPriThd(DrvSclDmaClientType_e enSCLDMA_ID, u8 u8value)
{
    if(enSCLDMA_ID == E_DRV_SCLDMA_1_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_18_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_1_SNP_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_48_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_1_IMI_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_48_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_2_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_60_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_2_FRM2_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_60_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_2_IMI_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_78_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_3_FRM_R)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_30_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_3_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_18_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_4_FRM_R)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_30_L, ((u16)u8value<<8), 0xFF00);
    }
    else
    {
        HAL_SCLDMA_ERR(sclprintf("%s %d, wrong enSCLDMA_ID: %d\n", __FUNCTION__, __LINE__, enSCLDMA_ID));
    }
}
void _HalSclDmaHkForceAuto(void)
{
    // sc22sc2_hk_force_auto, sc2out_hk_force_auto
    HalUtilityW2BYTEMSK(REG_SCL0_03_L, BIT11|BIT10|BIT9|BIT8|BIT2|BIT1|BIT0, BIT11|BIT10|BIT9|BIT8|BIT2|BIT1|BIT0);
}
void _HalSclDmaSc3DbHwAuto(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL0_06_L, bEn ? BIT4 :0, BIT4);
}
void _HalSclDmaDbHwAuto(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL0_06_L, bEn ? BIT3 :0, BIT3);
}
void _HalSclDamHkHwAutoMd(bool bDbmode)
{
    HalUtilityW2BYTEMSK(REG_SCL0_06_L, bDbmode ? BIT6 : 0, BIT6);
}
void _HalSclDmaHkHwAuto(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL0_06_L, bEn ? BIT5 :0, BIT5);
}
void HalSclDmaHWInit(void)
{
    _HalSclDmaHkForceAuto();
    //_HalSclDmaHkHwAuto(1);
    _HalSclDmaDbHwAuto(1);
    HalSclDmaSetMCMByDMAClient(E_DRV_SCLDMA_1_SNP_W,0);
    _HalSclDmaSetPriThd(E_DRV_SCLDMA_3_FRM_R,0x08);
    _HalSclDmaSetRegLen(E_DRV_SCLDMA_3_FRM_R,0x10);
    _HalSclDmaSetRegTh(E_DRV_SCLDMA_3_FRM_R,0x12);
    _HalSclDmaSetRegLen(E_DRV_SCLDMA_3_FRM_W,0x10);
    _HalSclDmaSetRegTh(E_DRV_SCLDMA_3_FRM_W,0x12);
    _HalSclDmaSetRegLen(E_DRV_SCLDMA_1_FRM_W,0x10);
    _HalSclDmaSetRegTh(E_DRV_SCLDMA_1_FRM_W,0x12);
    _HalSclDmaSetRegLen(E_DRV_SCLDMA_1_SNP_W,0x10);
    _HalSclDmaSetRegTh(E_DRV_SCLDMA_1_SNP_W,0x12);
    _HalSclDmaSetRegLen(E_DRV_SCLDMA_2_FRM_W,0x10);
    _HalSclDmaSetRegTh(E_DRV_SCLDMA_2_FRM_W,0x12);
    _HalSclDmaSetRegLen(E_DRV_SCLDMA_2_FRM2_W,0x10);
    _HalSclDmaSetRegTh(E_DRV_SCLDMA_2_FRM2_W,0x12);
    HalSclDmaSetSwReset();

}
#if CONFIG_OF
void HalSclDmaCLKInit(bool bEn,DrvSclDmaClkConfig_t *stclk)
{
    DrvSclOsClkStruct_t* stclock = NULL;
    u16 regclk;
    u32 u32Events;
    if(DrvSclIrqGetEachDMAEn()|| (!DrvSclIrqGetIsBlankingRegion()))
    {
        HAL_SCLDMA_ERR(sclprintf("[FCLK]%s %d,Wait\n", __FUNCTION__, __LINE__));
        DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(),
            (E_DRV_SCLIRQ_EVENT_ISPFRMEND|E_DRV_SCLIRQ_EVENT_FRMENDSYNC), &u32Events, E_DRV_SCLOS_EVENT_MD_AND, 200); // get status: FRM END
    }
    if(bEn)
    {
        if (DrvSclOsClkGetEnableCount(stclk->fclk1) == 0)
        {
#if CLKDynamic
            if(IsClkIncrease(u16gheight[0],u16gwidth[0],0))
#else
            if(1)
#endif
            {
                if (NULL != (stclock = DrvSclOsClkGetParentByIndex(stclk->fclk1, 0)))
                {
                    DrvSclOsClkSetParent(stclk->fclk1, stclock);
                    DrvSclOsClkPrepareEnable(stclk->fclk1);
                    regclk = HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L);
                    //sclprintf("[SCLDMA]flag:%ld index:%hx\n",DrvSclOsClkGetRate(stclk->fclk1),regclk);
                }
                else
                {
                    SCL_ERR("[HAlSCLDMA]CLK Tree Not Ready \n");
                }
        #if OSDinverseBug
                if (NULL != (stclock = DrvSclOsClkGetParentByIndex(stclk->fclk2, 0)))
                {
                    DrvSclOsClkSetParent(stclk->fclk2, stclock);
                    DrvSclOsClkPrepareEnable(stclk->fclk2);
                }
                else
                {
                    SCL_ERR("[HAlHVSP]CLK Tree Not Ready \n");
                }
        #endif
            }
            else
            {
                if (NULL != (stclock = DrvSclOsClkGetParentByIndex(stclk->fclk1, 1)))
                {
                    DrvSclOsClkSetParent(stclk->fclk1, stclock);
                    DrvSclOsClkPrepareEnable(stclk->fclk1);
                    regclk = HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L);
                    //sclprintf("[SCLDMA]flag:%ld index:%hx\n",DrvSclOsClkGetRate(stclk->fclk1),regclk);
                }
                else
                {
                    SCL_ERR("[HAlSCLDMA]CLK Tree Not Ready \n");
                }
        #if OSDinverseBug
                if (NULL != (stclock = DrvSclOsClkGetParentByIndex(stclk->fclk2, 1)))
                {
                    DrvSclOsClkSetParent(stclk->fclk2, stclock);
                    DrvSclOsClkPrepareEnable(stclk->fclk2);
                }
                else
                {
                    SCL_ERR("[HAlHVSP]CLK Tree Not Ready \n");
                }
        #endif
            }
        }
#if CLKDynamic
        else
        {
            if(IsClkIncrease(u16gheight[0],u16gwidth[0], DrvSclOsClkGetRate(stclk->fclk1)))
            {
                if (NULL != (stclock = DrvSclOsClkGetParentByIndex(stclk->fclk1, 0)))
                {
                    DrvSclOsClkSetParent(stclk->fclk1, stclock);
                    regclk = HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L);
                    //sclprintf("[SCLDMA]flag:%ld index:%hx\n",DrvSclOsClkGetRate(stclk->fclk1),regclk);
                }
                else
                {
                    SCL_ERR("[HAlSCLDMA]CLK Tree Not Ready \n");
                }
            #if OSDinverseBug
                if (NULL != (stclock = DrvSclOsClkGetParentByIndex(stclk->fclk2, 0)))
                {
                    DrvSclOsClkSetParent(stclk->fclk2, stclock);
                }
                else
                {
                    SCL_ERR("[HAlHVSP]CLK Tree Not Ready \n");
                }
            #endif
            }
            else if(IsClkDecrease(u16gheight[0],u16gwidth[0], DrvSclOsClkGetRate(stclk->fclk1)))
            {
                if (NULL != (stclock = DrvSclOsClkGetParentByIndex(stclk->fclk1, 1)))
                {
                    DrvSclOsClkSetParent(stclk->fclk1, stclock);
                    regclk = HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L);
                    //sclprintf("[SCLDMA]flag:%ld index:%hx\n",DrvSclOsClkGetRate(stclk->fclk1),regclk);
                }
                else
                {
                    SCL_ERR("[HAlSCLDMA]CLK Tree Not Ready \n");
                }
            #if OSDinverseBug
                if (NULL != (stclock = DrvSclOsClkGetParentByIndex(stclk->fclk2, 1)))
                {
                    DrvSclOsClkSetParent(stclk->fclk2, stclock);
                }
                else
                {
                    SCL_ERR("[HAlHVSP]CLK Tree Not Ready \n");
                }
            #endif
            }
        }
#endif
    }
    else
    {
        if (NULL != (stclock = DrvSclOsClkGetParentByIndex(stclk->fclk1, 0)))
        {
            DrvSclOsClkSetParent(stclk->fclk1, stclock);
        }
        else
        {
            SCL_ERR("[HAlSCLDMA]CLK Tree Not Ready \n");
        }
#if OSDinverseBug
        if (NULL != (stclock = DrvSclOsClkGetParentByIndex(stclk->fclk2, 0)))
        {
            DrvSclOsClkSetParent(stclk->fclk2, stclock);
        }
        else
        {
            SCL_ERR("[HAlHVSP]CLK Tree Not Ready \n");
        }
#endif
        while (DrvSclOsClkGetEnableCount(stclk->fclk1))
        {
            DrvSclOsClkDisableUnprepare(stclk->fclk1);
        }
    #if OSDinverseBug
        while (DrvSclOsClkGetEnableCount(stclk->fclk2))
        {
            DrvSclOsClkDisableUnprepare(stclk->fclk2);
        }
    #endif
    }
    regclk=regclk;
}

void HalSclDmaSC3CLKInit(bool bEn,DrvSclDmaClkConfig_t *stclk)
{
#if OSDinverseBug
#else
    DrvSclOsClkStruct_t* stclock = NULL;
    if(bEn)
    {
        if (DrvSclOsClkGetEnableCount(stclk->fclk2) == 0)
        {
            if(IsClkIncrease(u16gheight[1],u16gwidth[1],0))
            {
                if (NULL != (stclock = DrvSclOsClkGetParentByIndex(stclk->fclk2, 0)))
                {
                    DrvSclOsClkSetParent(stclk->fclk2, stclock);
                    DrvSclOsClkPrepareEnable(stclk->fclk2);
                }
                else
                {
                    SCL_ERR("[HAlSCLDMA]CLK Tree Not Ready \n");
                }
            }
            else
            {
                if (NULL != (stclock = DrvSclOsClkGetParentByIndex(stclk->fclk2, 1)))
                {
                    DrvSclOsClkSetParent(stclk->fclk2, stclock);
                    DrvSclOsClkPrepareEnable(stclk->fclk2);
                }
                else
                {
                    SCL_ERR("[HAlSCLDMA]CLK Tree Not Ready \n");
                }
            }
        }
        else
        {
            if(IsClkIncrease(u16gheight[1],u16gwidth[1], DrvSclOsClkGetRate(stclk->fclk2)))
            {
                if (NULL != (stclock = DrvSclOsClkGetParentByIndex(stclk->fclk2, 0)))
                {
                    DrvSclOsClkSetParent(stclk->fclk2, stclock);
                }
                else
                {
                    SCL_ERR("[HAlSCLDMA]CLK Tree Not Ready \n");
                }
            }
            else if(IsClkDecrease(u16gheight[1],u16gwidth[1], DrvSclOsClkGetRate(stclk->fclk2)))
            {
                if (NULL != (stclock = DrvSclOsClkGetParentByIndex(stclk->fclk2, 1)))
                {
                    DrvSclOsClkSetParent(stclk->fclk2, stclock);
                }
                else
                {
                    SCL_ERR("[HAlSCLDMA]CLK Tree Not Ready \n");
                }
            }
        }

    }
    else
    {
        if (NULL != (stclock = DrvSclOsClkGetParentByIndex(stclk->fclk2, 0)))
        {
            DrvSclOsClkSetParent(stclk->fclk2, stclock);
        }
        else
        {
            SCL_ERR("[HAlSCLDMA]CLK Tree Not Ready \n");
        }
        while (DrvSclOsClkGetEnableCount(stclk->fclk2))
        {
            DrvSclOsClkDisableUnprepare(stclk->fclk2);
        }
    }
#endif
}
void HalSclDmaODCLKInit(bool bEn,DrvSclDmaClkConfig_t *stclk)
{
    DrvSclOsClkStruct_t* pstclock = NULL;
    if(bEn)
    {
        if(DrvSclOsClkGetEnableCount(stclk->odclk)==0)
        {
            if (NULL != (pstclock = DrvSclOsClkGetParentByIndex(stclk->odclk, 3)))
            {
                DrvSclOsClkSetParent(stclk->odclk, pstclock);
                DrvSclOsClkPrepareEnable(stclk->odclk);
            }
            else
            {
                SCL_ERR("[HAlSCLDMA]CLK Tree Not Ready \n");
            }
        }
        else
        {
            sclprintf("[odclk]CAN'T/Already OPEN\n");
        }

    }
    else
    {
        if (NULL != (pstclock = DrvSclOsClkGetParentByIndex(stclk->odclk, 0)))
        {
            DrvSclOsClkSetParent(stclk->odclk, pstclock);
        }
        else
        {
            SCL_ERR("[HAlSCLDMA]CLK Tree Not Ready \n");
        }
        while (DrvSclOsClkGetEnableCount(stclk->odclk))
        {
            DrvSclOsClkDisableUnprepare(stclk->odclk);
        }
    }
}

#else
void HalSclDmaCLKInit(bool bEn,DrvSclDmaClkConfig_t *stclk)
{
    u16 regclk=0;
    sclprintf("[hal]NO OF\n");
    if(bEn)
    {
        if ((HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L)&DISABLE_CLK))
        {
#if CLKDynamic
            if(IsClkIncrease(u16gheight[0],u16gwidth[0],0))
#else
            if(1)
#endif
            {
                HalUtilityW2BYTEMSK(REG_SCL_CLK_64_L,0x0000,0x000F);//h61
                regclk = HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L);
#if OSDinverseBug
                W2BYTEMSK(REG_SCL_CLK_65_L,0x0000,0x000F);//h61
                regclk = R2BYTE(REG_SCL_CLK_65_L);
#endif
            }
            else
            {
                HalUtilityW2BYTEMSK(REG_SCL_CLK_64_L,LOW_CLK,0x000F);//h61
                regclk = HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L);
#if OSDinverseBug
                W2BYTEMSK(REG_SCL_CLK_65_L,LOW_CLK,0x000F);//h61
                regclk = R2BYTE(REG_SCL_CLK_65_L);
#endif
            }
        }
#if CLKDynamic
        else
        {
            if((u16gheight[0] > 720 || u16gwidth[0] > 1280)&& (HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L)&LOW_CLK))
            {
                HalUtilityW2BYTEMSK(REG_SCL_CLK_64_L,0x0000,0x000F);//h61
                regclk = HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L);
#if OSDinverseBug
                W2BYTEMSK(REG_SCL_CLK_65_L,0x0000,0x000F);//h61
                regclk = R2BYTE(REG_SCL_CLK_65_L);

#endif
            }
            else if((u16gheight[0] <= 720 && u16gwidth[0] <= 1280)&& !(HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L)&LOW_CLK))
            {
                HalUtilityW2BYTEMSK(REG_SCL_CLK_64_L,LOW_CLK,0x000F);//h61
                regclk = HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L);
#if OSDinverseBug
                W2BYTEMSK(REG_SCL_CLK_65_L,LOW_CLK,0x000F);//h61
                regclk = R2BYTE(REG_SCL_CLK_64_L);
#endif
            }
        }
#endif
    }
    else
    {
        regclk=0;
        HalUtilityW2BYTEMSK(REG_SCL_CLK_64_L,0x0,0x000F);//h61
#if OSDinverseBug
        W2BYTEMSK(REG_SCL_CLK_65_L,0x0,0x000F);//h61
#endif
    }
    regclk=regclk;
}

void HalSclDmaSC3CLKInit(bool bEn,DrvSclDmaClkConfig_t *stclk)
{
#if OSDinverseBug
#else
    u16 regclk=0;
    if(bEn)
    {
        if ((HalUtilityR2BYTEDirect(REG_SCL_CLK_65_L)&DISABLE_CLK))
        {
            if(IsClkIncrease(u16gheight[1],u16gwidth[1],0))
            {
                HalUtilityW2BYTEMSK(REG_SCL_CLK_65_L,0x0000,0x000F);//h61
                regclk = HalUtilityR2BYTEDirect(REG_SCL_CLK_65_L);
            }
            else
            {
                HalUtilityW2BYTEMSK(REG_SCL_CLK_65_L,LOW_CLK,0x000F);//h61
                regclk = HalUtilityR2BYTEDirect(REG_SCL_CLK_65_L);
            }
        }
        else
        {
            if((u16gheight[1] > 720 || u16gwidth[1] > 1280) && (HalUtilityR2BYTEDirect(REG_SCL_CLK_65_L)&LOW_CLK))
            {
                HalUtilityW2BYTEMSK(REG_SCL_CLK_65_L,0x0000,0x000F);//h61
                regclk = HalUtilityR2BYTEDirect(REG_SCL_CLK_65_L);
            }
            else if((u16gheight[1] <= 720 && u16gwidth[1] <= 1280) && !(HalUtilityR2BYTEDirect(REG_SCL_CLK_65_L)&LOW_CLK))
            {
                HalUtilityW2BYTEMSK(REG_SCL_CLK_65_L,LOW_CLK,0x000F);//h61
                regclk = HalUtilityR2BYTEDirect(REG_SCL_CLK_65_L);
            }
        }

    }
    else
    {
       regclk=0;
        HalUtilityW2BYTEMSK(REG_SCL_CLK_65_L,0x0,0x000F);//h61
    }
    regclk=regclk;
#endif
}
void HalSclDmaODCLKInit(bool bEn,DrvSclDmaClkConfig_t *stclk)
{
    //struct clk* pstclock = NULL;
    if(bEn)
    {
        if((HalUtilityR2BYTEDirect(REG_SCL_CLK_66_L)&DISABLE_CLK))
        {
            HalUtilityW2BYTEMSK(REG_SCL_CLK_66_L,0x000C,0x000F);//h66
        }
        else
        {
            sclprintf("[odclk]CAN'T/Already OPEN\n");
        }

    }
    else
    {
        HalUtilityW2BYTEMSK(REG_SCL_CLK_66_L,0x000D,0x000F);//h66
    }
}

#endif

void HalSclDmaSetSC2HandshakeForce(DrvSclDmaRwModeType_e enRWMode, bool bEn)
{
    if(enRWMode == E_DRV_SCLDMA_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_02_L, bEn ? 0 : BIT0, BIT0);
    }
    else if(enRWMode == E_DRV_SCLDMA_IMI_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_02_L, bEn ? 0 : BIT1, BIT1);
    }
    else if(enRWMode == E_DRV_SCLDMA_FRM2_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_02_L, bEn ? 0 : BIT2, BIT2);
    }
}

void HalSclDmaSetSC1HandshakeForce(DrvSclDmaRwModeType_e enRWMode, bool bEn)
{
    if(enRWMode == E_DRV_SCLDMA_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_02_L, bEn ? 0 : BIT9, BIT9);
    }
    else if(enRWMode == E_DRV_SCLDMA_SNP_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_02_L, bEn ? 0 : BIT10, BIT10);
    }
    else if(enRWMode == E_DRV_SCLDMA_IMI_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_02_L, bEn ? 0 : BIT11, BIT11);
    }

    //HAL_SCLDMA_ERR(sclprintf("[HalSCLDMA]HK_Force:%hx\n", R2BYTE(REG_SCL0_02_L)));
}

void HalSclDmaSetSC1ToSC2HandshakeForce(bool bEn)
{
    if((HalUtilityR2BYTE(REG_SCL0_02_L)&0x7) == 0x7 && bEn==0)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_02_L, BIT8, BIT8);
    }
    else if((HalUtilityR2BYTE(REG_SCL0_02_L)&0xF) != 0x7 && bEn)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_02_L, 0, BIT8);
    }
}
void HalSclDmaSetMCMByDMAClient(DrvSclDmaClientType_e enSCLDMA_ID, u8 u8value)
{
    if(enSCLDMA_ID == E_DRV_SCLDMA_1_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_MCM_06_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_1_SNP_W)
    {
        HalUtilityW2BYTEMSK(REG_MCM_07_L, u8value, 0xFF);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_1_IMI_W)
    {
        HalUtilityW2BYTEMSK(REG_MCM_10_L, u8value, 0xFF);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_2_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_MCM_08_L, u8value, 0xFF);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_2_IMI_W)
    {
        HalUtilityW2BYTEMSK(REG_MCM_10_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_3_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_MCM_08_L, ((u16)u8value<<8), 0xFF00);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_4_FRM_R)
    {
        HalUtilityW2BYTEMSK(REG_MCM_07_L, ((u16)u8value<<8), 0xFF00);
    }
    else
    {
        HAL_SCLDMA_ERR(sclprintf("%s %d, wrong enSCLDMA_ID: %d\n", __FUNCTION__, __LINE__, enSCLDMA_ID));
    }
}
void HalSclDmaSetCheckFrmEndSignal(DrvSclDmaClientType_e enSCLDMA_ID, bool bEn)
{
    if(enSCLDMA_ID == E_DRV_SCLDMA_1_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA2_50_L, bEn ? BIT11 : 0, BIT11);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_1_SNP_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA2_50_L, bEn ? BIT12 : 0, BIT12);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_1_IMI_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA2_50_L, bEn ? BIT13 : 0, BIT13);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_2_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA2_50_L, bEn ? BIT10 : 0, BIT10);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_2_FRM2_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA2_50_L, bEn ? BIT14 : 0, BIT14);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_2_IMI_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA2_50_L, bEn ? BIT9 : 0, BIT9);
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_3_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA2_50_L, bEn ? BIT8 : 0, BIT8);
    }
    else
    {
        HAL_SCLDMA_ERR(sclprintf("%s %d, wrong enSCLDMA_ID: %d\n", __FUNCTION__, __LINE__, enSCLDMA_ID));
    }
}

void HalSclDmaSetSC1DMAEn(DrvSclDmaRwModeType_e enRWMode, bool bEn)
{
    if(enRWMode == E_DRV_SCLDMA_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_01_L, bEn ? BIT15 : 0, BIT15);
    }
    else if(enRWMode == E_DRV_SCLDMA_SNP_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_03_L, bEn ? BIT15 : 0, BIT15);
    }
    else if(enRWMode == E_DRV_SCLDMA_IMI_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_03_L, bEn ? BIT15 : 0, BIT15);
    }
    else
    {
        HAL_SCLDMA_ERR(sclprintf("%s %d, wrong RWmode: %d\n", __FUNCTION__, __LINE__, enRWMode));
    }
    //HAL_SCLDMA_ERR(sclprintf("[HalSCLDMA]DMA1_En:%hx\n", R2BYTE(REG_SCL_DMA0_01_L)));

}
void Hal_SCLDMA_GetHVSPResolutionForEnsure(u16 u16height)
{
    u16 u16Srcheight,u16Dspheight,u16FBheight;
    u16 u16Srcwidth,u16Dspwidth,u16FBwidth;
    u16Srcheight = HalUtilityR2BYTE(REG_SCL_HVSP0_21_L);
    u16Dspheight = HalUtilityR2BYTE(REG_SCL_HVSP0_23_L);
    u16FBheight  = HalUtilityR2BYTE(REG_SCL_DNR1_0D_L);
    u16Srcwidth = HalUtilityR2BYTE(REG_SCL_HVSP0_20_L);
    u16Dspwidth = HalUtilityR2BYTE(REG_SCL_HVSP0_22_L);
    u16FBwidth  = HalUtilityR2BYTE(REG_SCL_DNR1_0E_L);
    if(u16Srcheight>0)
    {
        u16gheight[0] = (u16Srcheight>u16Dspheight) ? u16Srcheight : u16Dspheight ;
        u16gheight[0] = (u16FBheight>u16gheight[0])? u16FBheight: u16gheight[0];
        u16gwidth[0] = (u16Srcwidth>u16Dspwidth) ? u16Srcwidth : u16Dspwidth ;
        u16gwidth[0] = (u16FBwidth>u16gwidth[0])? u16FBwidth: u16gwidth[0];
    }
    u16Srcheight = HalUtilityR2BYTE(REG_SCL_HVSP2_21_L);
    u16Dspheight = HalUtilityR2BYTE(REG_SCL_HVSP2_23_L);
    u16Srcwidth = HalUtilityR2BYTE(REG_SCL_HVSP2_20_L);
    u16Dspwidth = HalUtilityR2BYTE(REG_SCL_HVSP2_22_L);
    if(u16Srcheight>0)
    {
        u16gheight[1] = (u16Srcheight>u16Dspheight) ? u16Srcheight : u16Dspheight ;
        u16gwidth[1] = (u16Srcwidth>u16Dspwidth) ? u16Srcwidth : u16Dspwidth ;
    }

}
u16 Hal_GetHVSPOutputHSize(DrvSclDmaIdType_e enSCLDMA_ID)
{
    u16 u16Width;
    u16Width = (enSCLDMA_ID == E_DRV_SCLDMA_ID_1_W) ? HalUtilityR2BYTE(REG_SCL_HVSP0_22_L) :
               (enSCLDMA_ID == E_DRV_SCLDMA_ID_2_W) ? HalUtilityR2BYTE(REG_SCL_HVSP1_22_L) :
               (enSCLDMA_ID == E_DRV_SCLDMA_ID_3_W) ? HalUtilityR2BYTE(REG_SCL_HVSP2_22_L) :
               (enSCLDMA_ID == E_DRV_SCLDMA_ID_3_R) ? HalUtilityR2BYTE(REG_SCL_HVSP2_20_L) :
                0;
    return u16Width;
}
u16 Hal_GetHVSPOutputVSize(DrvSclDmaIdType_e enSCLDMA_ID)
{
    u16 u16Height;
    u16Height = (enSCLDMA_ID == E_DRV_SCLDMA_ID_1_W) ? HalUtilityR2BYTE(REG_SCL_HVSP0_23_L) :
                (enSCLDMA_ID == E_DRV_SCLDMA_ID_2_W) ? HalUtilityR2BYTE(REG_SCL_HVSP1_23_L) :
                (enSCLDMA_ID == E_DRV_SCLDMA_ID_3_W) ? HalUtilityR2BYTE(REG_SCL_HVSP2_23_L) :
                (enSCLDMA_ID == E_DRV_SCLDMA_ID_3_R) ? HalUtilityR2BYTE(REG_SCL_HVSP2_21_L) :
                0;
    return u16Height;
}
void HalSclDmaSetSC1DMAConfig(DrvSclDmaRwConfig_t stSCLDMACfg)
{
    u32 u32yoffset;
    u32 u32coffset;
    u8 u8420md;
    u8 u8DataMd;
    if(stSCLDMACfg.enRWMode == E_DRV_SCLDMA_FRM_W)
    {
        if(stSCLDMACfg.bvFlag.btsBase_0)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_08_L, stSCLDMACfg.u32Base_Y[0]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_10_L, stSCLDMACfg.u32Base_C[0]>>3);
            HalUtilityW4BYTE(REG_SCL3_38_L, stSCLDMACfg.u32Base_V[0]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_1)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_0A_L, stSCLDMACfg.u32Base_Y[1]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_12_L, stSCLDMACfg.u32Base_C[1]>>3);
            HalUtilityW4BYTE(REG_SCL3_3A_L, stSCLDMACfg.u32Base_V[1]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_2)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_0C_L, stSCLDMACfg.u32Base_Y[2]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_14_L, stSCLDMACfg.u32Base_C[2]>>3);
            HalUtilityW4BYTE(REG_SCL3_3C_L, stSCLDMACfg.u32Base_V[2]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_3)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_0E_L, stSCLDMACfg.u32Base_Y[3]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_16_L, stSCLDMACfg.u32Base_C[3]>>3);
            HalUtilityW4BYTE(REG_SCL3_3E_L, stSCLDMACfg.u32Base_V[3]>>3);
        }
        if(stSCLDMACfg.enBuffMode == E_DRV_SCLDMA_BUF_MD_RING)
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA0_18_L, stSCLDMACfg.u8MaxIdx<<3, BIT4|BIT3);
        }
        else
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA0_18_L, 0, BIT4|BIT3);
        }
        Hal_SCLDMA_GetHVSPResolutionForEnsure(stSCLDMACfg.u16Height);
        u8420md = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV420 ? BIT3 :
                stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT3 :
                                                                0;
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_1E_L, u8420md, BIT3);   // w_422to420_md[3]
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_1E_L, BIT2, BIT2);
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_01_L, stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV422 ? BIT14 : 0, BIT14); // w_422_pack[14]
        u8DataMd = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT0 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YCSep422 ? BIT1 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep422 ? BIT2 :
                                                                    0;
        HalUtilityW2BYTEMSK(REG_SCL3_20_L, u8DataMd, BIT0|BIT1|BIT2); // w_422_pack[14]
    }
    else if(stSCLDMACfg.enRWMode == E_DRV_SCLDMA_SNP_W)
    {
        if(stSCLDMACfg.bvFlag.btsBase_0)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_38_L, stSCLDMACfg.u32Base_Y[0]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_40_L, stSCLDMACfg.u32Base_C[0]>>3);
            HalUtilityW4BYTE(REG_SCL3_40_L, stSCLDMACfg.u32Base_V[0]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_1)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_3A_L, stSCLDMACfg.u32Base_Y[1]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_42_L, stSCLDMACfg.u32Base_C[1]>>3);
            HalUtilityW4BYTE(REG_SCL3_42_L, stSCLDMACfg.u32Base_V[1]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_2)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_3C_L, stSCLDMACfg.u32Base_Y[2]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_44_L, stSCLDMACfg.u32Base_C[2]>>3);
            HalUtilityW4BYTE(REG_SCL3_44_L, stSCLDMACfg.u32Base_V[2]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_3)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_3E_L, stSCLDMACfg.u32Base_Y[3]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_46_L, stSCLDMACfg.u32Base_C[3]>>3);
            HalUtilityW4BYTE(REG_SCL3_46_L, stSCLDMACfg.u32Base_V[3]>>3);
        }
        if(stSCLDMACfg.enBuffMode == E_DRV_SCLDMA_BUF_MD_RING)
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA0_48_L, stSCLDMACfg.u8MaxIdx<<3, BIT4|BIT3);
        }
        else
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA0_48_L, 0, BIT4|BIT3);
        }
        u8420md = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV420 ? BIT3 :
                stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT3 :
                                                                0;
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_4E_L, u8420md, BIT3);   // w_422to420_md[3]
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_4E_L, BIT2, BIT2);
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_03_L, stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV422 ? BIT14 : 0, BIT14); // w_422_pack[14]
        u8DataMd = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT0 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YCSep422 ? BIT1 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep422 ? BIT2 :
                                                                    0;
        HalUtilityW2BYTEMSK(REG_SCL3_22_L, u8DataMd, BIT0|BIT1|BIT2); // w_422_pack[14]
    }
    else if(stSCLDMACfg.enRWMode == E_DRV_SCLDMA_IMI_W)
    {
#if ENABLE_RING_DB
        u32yoffset = 0x20;
        u32coffset = 0x20;
#else
        if((stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV420)||(stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420))
        {
            u32yoffset=(stSCLDMACfg.u16Width*16)/8-1;
            u32coffset=(stSCLDMACfg.u16Width*8)/8-1;
        }
        else
        {
            u32yoffset=(stSCLDMACfg.u16Width*16)/8-2;
            u32coffset=(stSCLDMACfg.u16Width*16)/8-2;
        }
#endif
        if(stSCLDMACfg.bvFlag.btsBase_0)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_38_L, stSCLDMACfg.u32Base_Y[0]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_40_L, stSCLDMACfg.u32Base_C[0]>>3);
            HalUtilityW4BYTE(REG_SCL3_68_L, stSCLDMACfg.u32Base_V[0]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_1)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_3A_L, stSCLDMACfg.u32Base_Y[1]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_42_L, stSCLDMACfg.u32Base_C[1]>>3);
            HalUtilityW4BYTE(REG_SCL3_6A_L, stSCLDMACfg.u32Base_V[1]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_2)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_3C_L, stSCLDMACfg.u32Base_Y[2]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_44_L, stSCLDMACfg.u32Base_C[2]>>3);
            HalUtilityW4BYTE(REG_SCL3_6C_L, stSCLDMACfg.u32Base_V[2]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_3)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_3E_L, stSCLDMACfg.u32Base_Y[3]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_46_L, stSCLDMACfg.u32Base_C[3]>>3);
            HalUtilityW4BYTE(REG_SCL3_6E_L, stSCLDMACfg.u32Base_V[3]>>3);
        }
        if(stSCLDMACfg.enBuffMode == E_DRV_SCLDMA_BUF_MD_RING)
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA1_48_L, stSCLDMACfg.u8MaxIdx<<3, BIT4|BIT3);
        }
        else
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA1_48_L, 0, BIT4|BIT3);
        }
        u8420md = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV420 ? BIT3 :
                stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT3 :
                                                                0;
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_4E_L, u8420md, BIT3);   // w_422to420_md[3]
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_4E_L, BIT2, BIT2);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_03_L, stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV422 ? BIT14 : 0, BIT14); // w_422_pack[14]
        u8DataMd = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT0 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YCSep422 ? BIT1 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep422 ? BIT2 :
                                                                    0;
        HalUtilityW2BYTEMSK(REG_SCL3_2C_L, u8DataMd, BIT0|BIT1|BIT2); // w_422_pack[14]
        HalUtilityW4BYTE(REG_SCL_DMA2_40_L, u32yoffset); // imi offset y
        HalUtilityW4BYTE(REG_SCL_DMA2_42_L, u32coffset); // imi offset c
        HalUtilityW2BYTEMSK(REG_SCL0_64_L,0, BIT15); // sc1 open
    }
    else
    {
        HAL_SCLDMA_ERR(sclprintf("%s %d, wrong RWmode: %d\n",  __FUNCTION__, __LINE__, stSCLDMACfg.enRWMode));
    }
    if(Hal_GetHVSPOutputHSize(E_DRV_SCLDMA_ID_1_W)==stSCLDMACfg.u16Width ||(Hal_GetHVSPOutputHSize(E_DRV_SCLDMA_ID_1_W) == 0))
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_1A_L, stSCLDMACfg.u16Width, 0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_4A_L, stSCLDMACfg.u16Width, 0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_4A_L, stSCLDMACfg.u16Width, 0xFFFF);
    }
    else
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_1A_L, Hal_GetHVSPOutputHSize(E_DRV_SCLDMA_ID_1_W), 0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_4A_L, Hal_GetHVSPOutputHSize(E_DRV_SCLDMA_ID_1_W), 0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_4A_L, Hal_GetHVSPOutputHSize(E_DRV_SCLDMA_ID_1_W), 0xFFFF);
    }
    if(Hal_GetHVSPOutputVSize(E_DRV_SCLDMA_ID_1_W) == stSCLDMACfg.u16Height ||(Hal_GetHVSPOutputVSize(E_DRV_SCLDMA_ID_1_W) == 0))
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_1B_L, stSCLDMACfg.u16Height,0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_4B_L, stSCLDMACfg.u16Height,0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_4B_L, stSCLDMACfg.u16Height,0xFFFF);
    }
    else
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_1B_L, Hal_GetHVSPOutputVSize(E_DRV_SCLDMA_ID_1_W), 0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_4B_L, Hal_GetHVSPOutputVSize(E_DRV_SCLDMA_ID_1_W), 0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_4B_L, Hal_GetHVSPOutputVSize(E_DRV_SCLDMA_ID_1_W), 0xFFFF);
    }

}
void HalSclDmaSetIMIClientReset(void)
{
    u32 u32yoffset;
    u32 u32coffset;
    u32yoffset = 0x0;
    u32coffset = 0x0;
    HalUtilityW4BYTE(REG_SCL_DMA1_38_L, 0);
    HalUtilityW4BYTE(REG_SCL_DMA1_40_L, 0);
    HalUtilityW4BYTE(REG_SCL_DMA1_3A_L, 0);
    HalUtilityW4BYTE(REG_SCL_DMA1_42_L, 0);
    HalUtilityW4BYTE(REG_SCL_DMA1_3C_L, 0);
    HalUtilityW4BYTE(REG_SCL_DMA1_44_L, 0);
    HalUtilityW4BYTE(REG_SCL_DMA1_3E_L, 0);
    HalUtilityW4BYTE(REG_SCL_DMA1_46_L, 0);

    HalUtilityW2BYTEMSK(REG_SCL_DMA1_48_L, 0, BIT4|BIT3);
    HalUtilityW2BYTEMSK(REG_SCL_DMA1_4A_L, 0, 0xFFFF);
    HalUtilityW2BYTEMSK(REG_SCL_DMA1_4B_L, 0, 0xFFFF);
    HalUtilityW2BYTEMSK(REG_SCL_DMA1_4E_L, 0, BIT3);   // w_422to420_md[3]
    HalUtilityW2BYTEMSK(REG_SCL_DMA1_4E_L, 0, BIT2);
    HalUtilityW2BYTEMSK(REG_SCL_DMA1_03_L, 0, BIT14); // w_422_pack[14]
    HalUtilityW4BYTE(REG_SCL_DMA2_40_L, u32yoffset); // imi offset y
    HalUtilityW4BYTE(REG_SCL_DMA2_42_L, u32coffset); // imi offset c
    HalUtilityW2BYTEMSK(REG_SCL0_64_L,0, BIT15); // sc1 open

}
void HalSclDmaSetDMAOutputBufferAddr
    (DrvSclDmaClientType_e enClientType,u32 u32YBufferAddr,u32 u32CBufferAddr,u32 u32VBufferAddr)
{
    switch(enClientType)
    {
        case E_DRV_SCLDMA_1_FRM_W:
        HalUtilityW4BYTE(REG_SCL_DMA0_08_L, u32YBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL_DMA0_10_L, u32CBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL3_38_L, u32CBufferAddr>>3);
            break;

        case E_DRV_SCLDMA_1_IMI_W:
        HalUtilityW4BYTE(REG_SCL_DMA1_38_L, u32YBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL_DMA1_40_L, u32CBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL3_68_L, u32CBufferAddr>>3);
        break;

        case E_DRV_SCLDMA_1_SNP_W:
        HalUtilityW4BYTE(REG_SCL_DMA0_38_L, u32YBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL_DMA0_40_L, u32CBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL3_40_L, u32CBufferAddr>>3);
        break;

        case E_DRV_SCLDMA_2_FRM_W:
        HalUtilityW4BYTE(REG_SCL_DMA0_50_L, u32YBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL_DMA0_58_L, u32CBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL3_48_L, u32CBufferAddr>>3);
        break;

        case E_DRV_SCLDMA_2_FRM2_W:
        HalUtilityW4BYTE(REG_SCL_DMA1_50_L, u32YBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL_DMA1_58_L, u32CBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL3_70_L, u32CBufferAddr>>3);
        break;

        case E_DRV_SCLDMA_2_IMI_W:
        HalUtilityW4BYTE(REG_SCL_DMA1_38_L, u32YBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL_DMA1_40_L, u32CBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL3_50_L, u32CBufferAddr>>3);
        break;

        case E_DRV_SCLDMA_3_FRM_R:
        HalUtilityW4BYTE(REG_SCL_DMA1_20_L, u32YBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL_DMA1_28_L, u32CBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL3_60_L, u32CBufferAddr>>3);
        break;

        case E_DRV_SCLDMA_3_FRM_W:
        HalUtilityW4BYTE(REG_SCL_DMA1_08_L, u32YBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL_DMA1_10_L, u32CBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL3_58_L, u32CBufferAddr>>3);
        break;

        case E_DRV_SCLDMA_4_FRM_R:
        HalUtilityW4BYTE(REG_SCL_DMA0_20_L, u32YBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL_DMA0_28_L, u32CBufferAddr>>3);
        HalUtilityW4BYTE(REG_SCL3_30_L, u32CBufferAddr>>3);
        break;
    default:
        break;
    }

}

void HalSclDmaSetSC2DMAEn(DrvSclDmaRwModeType_e enRWMode, bool bEn)
{
    if(enRWMode == E_DRV_SCLDMA_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_04_L, bEn ? BIT15 : 0, BIT15);
    }
    else if(enRWMode == E_DRV_SCLDMA_FRM2_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_04_L, bEn ? BIT15 : 0, BIT15);
    }
    else if(enRWMode == E_DRV_SCLDMA_IMI_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_03_L, bEn ? BIT15 : 0, BIT15);
    }
    else
    {
        HAL_SCLDMA_ERR(sclprintf("%s %d, wrong RWmode: %d\n",  __FUNCTION__, __LINE__, enRWMode));
    }

}

void HalSclDmaSetSC2DMAConfig(DrvSclDmaRwConfig_t stSCLDMACfg)
{
    u32 u32yoffset;
    u32 u32coffset;
    u8 u8420md;
    u8 u8DataMd;
    if(stSCLDMACfg.enRWMode == E_DRV_SCLDMA_FRM_W)
    {
        if(stSCLDMACfg.bvFlag.btsBase_0)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_50_L, stSCLDMACfg.u32Base_Y[0]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_58_L, stSCLDMACfg.u32Base_C[0]>>3);
            HalUtilityW4BYTE(REG_SCL3_48_L, stSCLDMACfg.u32Base_V[0]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_1)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_52_L, stSCLDMACfg.u32Base_Y[1]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_5A_L, stSCLDMACfg.u32Base_C[1]>>3);
            HalUtilityW4BYTE(REG_SCL3_4A_L, stSCLDMACfg.u32Base_V[1]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_2)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_54_L, stSCLDMACfg.u32Base_Y[2]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_5C_L, stSCLDMACfg.u32Base_C[2]>>3);
            HalUtilityW4BYTE(REG_SCL3_4C_L, stSCLDMACfg.u32Base_V[2]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_3)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_56_L, stSCLDMACfg.u32Base_Y[3]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_5E_L, stSCLDMACfg.u32Base_C[3]>>3);
            HalUtilityW4BYTE(REG_SCL3_4E_L, stSCLDMACfg.u32Base_V[3]>>3);
        }
        if(stSCLDMACfg.enBuffMode == E_DRV_SCLDMA_BUF_MD_RING)
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA0_60_L, stSCLDMACfg.u8MaxIdx<<3, BIT4|BIT3);
        }
        else
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA0_60_L, 0, BIT4|BIT3);
        }
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_66_L, BIT2, BIT2);
        u8420md = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV420 ? BIT3 :
                stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT3 :
                                                                0;
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_66_L, u8420md, BIT3);
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_04_L, stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV422 ? BIT14 : 0, BIT14);
        u8DataMd = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT0 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YCSep422 ? BIT1 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep422 ? BIT2 :
                                                                    0;
        HalUtilityW2BYTEMSK(REG_SCL3_24_L, u8DataMd, BIT0|BIT1|BIT2); // w_422_pack[14]
    }
    else if(stSCLDMACfg.enRWMode == E_DRV_SCLDMA_FRM2_W)
    {
        if(stSCLDMACfg.bvFlag.btsBase_0)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_50_L, stSCLDMACfg.u32Base_Y[0]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_58_L, stSCLDMACfg.u32Base_C[0]>>3);
            HalUtilityW4BYTE(REG_SCL3_70_L, stSCLDMACfg.u32Base_V[0]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_1)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_52_L, stSCLDMACfg.u32Base_Y[1]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_5A_L, stSCLDMACfg.u32Base_C[1]>>3);
            HalUtilityW4BYTE(REG_SCL3_72_L, stSCLDMACfg.u32Base_V[1]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_2)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_54_L, stSCLDMACfg.u32Base_Y[2]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_5C_L, stSCLDMACfg.u32Base_C[2]>>3);
            HalUtilityW4BYTE(REG_SCL3_74_L, stSCLDMACfg.u32Base_V[2]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_3)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_56_L, stSCLDMACfg.u32Base_Y[3]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_5E_L, stSCLDMACfg.u32Base_C[3]>>3);
            HalUtilityW4BYTE(REG_SCL3_76_L, stSCLDMACfg.u32Base_V[3]>>3);
        }
        if(stSCLDMACfg.enBuffMode == E_DRV_SCLDMA_BUF_MD_RING)
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA1_60_L, stSCLDMACfg.u8MaxIdx<<3, BIT4|BIT3);
        }
        else
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA1_60_L, 0, BIT4|BIT3);
        }
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_66_L, BIT2, BIT2);
        u8420md = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV420 ? BIT3 :
                stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT3 :
                                                                0;
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_66_L, u8420md, BIT3);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_04_L, stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV422 ? BIT14 : 0, BIT14);
        u8DataMd = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT0 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YCSep422 ? BIT1 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep422 ? BIT2 :
                                                                    0;
        HalUtilityW2BYTEMSK(REG_SCL3_2E_L, u8DataMd, BIT0|BIT1|BIT2); // w_422_pack[14]
    }
    else if(stSCLDMACfg.enRWMode == E_DRV_SCLDMA_IMI_W)
    {

        if((stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV420)||(stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420))
        {
            u32yoffset=(stSCLDMACfg.u16Width*16)/8-1;
            u32coffset=(stSCLDMACfg.u16Width*8)/8-1;
        }
        else
        {
            u32yoffset=(stSCLDMACfg.u16Width*16)/8-2;
            u32coffset=(stSCLDMACfg.u16Width*16)/8-2;
        }
        if(stSCLDMACfg.bvFlag.btsBase_0)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_68_L, stSCLDMACfg.u32Base_Y[0]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_70_L, stSCLDMACfg.u32Base_C[0]>>3);
            HalUtilityW4BYTE(REG_SCL3_50_L, stSCLDMACfg.u32Base_V[0]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_1)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_6A_L, stSCLDMACfg.u32Base_Y[1]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_72_L, stSCLDMACfg.u32Base_C[1]>>3);
            HalUtilityW4BYTE(REG_SCL3_52_L, stSCLDMACfg.u32Base_V[1]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_2)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_6C_L, stSCLDMACfg.u32Base_Y[2]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_74_L, stSCLDMACfg.u32Base_C[2]>>3);
            HalUtilityW4BYTE(REG_SCL3_54_L, stSCLDMACfg.u32Base_V[2]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_3)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_6E_L, stSCLDMACfg.u32Base_Y[3]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_76_L, stSCLDMACfg.u32Base_C[3]>>3);
            HalUtilityW4BYTE(REG_SCL3_56_L, stSCLDMACfg.u32Base_V[3]>>3);
        }
        if(stSCLDMACfg.enBuffMode == E_DRV_SCLDMA_BUF_MD_RING)
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA0_78_L, stSCLDMACfg.u8MaxIdx<<3, BIT4|BIT3);
        }
        else
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA0_78_L, 0, BIT4|BIT3);
        }
        u8420md = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV420 ? BIT3 :
                stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT3 :
                                                                0;
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_7E_L, u8420md, BIT3);   // w_422to420_md[3]
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_7E_L, BIT2, BIT2);
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_05_L, stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV422 ? BIT14 : 0, BIT14); // w_422_pack[14]
        u8DataMd = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT0 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YCSep422 ? BIT1 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep422 ? BIT2 :
                                                                    0;
        HalUtilityW2BYTEMSK(REG_SCL3_26_L, u8DataMd, BIT0|BIT1|BIT2); // w_422_pack[14]
        HalUtilityW4BYTE(REG_SCL_DMA2_44_L, u32yoffset); // imi offset y
        HalUtilityW4BYTE(REG_SCL_DMA2_46_L, u32coffset); // imi offset c
        HalUtilityW2BYTEMSK(REG_SCL0_64_L,BIT15, BIT15); //sc2 open
    }
    else
    {
        HAL_SCLDMA_ERR(sclprintf("%s %d, wrong RWmode: %d\n",  __FUNCTION__, __LINE__, stSCLDMACfg.enRWMode));
    }
    if(Hal_GetHVSPOutputHSize(E_DRV_SCLDMA_ID_2_W)==stSCLDMACfg.u16Width ||(Hal_GetHVSPOutputHSize(E_DRV_SCLDMA_ID_2_W) == 0))
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_62_L, stSCLDMACfg.u16Width, 0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_62_L, stSCLDMACfg.u16Width, 0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_7A_L, stSCLDMACfg.u16Width, 0xFFFF);
    }
    else
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_62_L, Hal_GetHVSPOutputHSize(E_DRV_SCLDMA_ID_2_W), 0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_62_L, Hal_GetHVSPOutputHSize(E_DRV_SCLDMA_ID_2_W), 0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_7A_L, Hal_GetHVSPOutputHSize(E_DRV_SCLDMA_ID_2_W), 0xFFFF);
    }
    if(Hal_GetHVSPOutputVSize(E_DRV_SCLDMA_ID_2_W) == stSCLDMACfg.u16Height ||(Hal_GetHVSPOutputVSize(E_DRV_SCLDMA_ID_2_W) == 0))
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_63_L, stSCLDMACfg.u16Height,0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_63_L, stSCLDMACfg.u16Height,0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_7B_L, stSCLDMACfg.u16Height,0xFFFF);
    }
    else
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_63_L, Hal_GetHVSPOutputVSize(E_DRV_SCLDMA_ID_2_W), 0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_63_L, Hal_GetHVSPOutputVSize(E_DRV_SCLDMA_ID_2_W), 0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_7B_L, Hal_GetHVSPOutputVSize(E_DRV_SCLDMA_ID_2_W), 0xFFFF);
    }
}


void HalSclDmaSetSC3DMAEn(DrvSclDmaRwModeType_e enRWMode, bool bEn)
{
    if(enRWMode == E_DRV_SCLDMA_FRM_R)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_02_L, bEn ? BIT15 : 0, BIT15);
    }
    else if(enRWMode == E_DRV_SCLDMA_FRM_W)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_01_L, bEn ? BIT15 : 0, BIT15);
    }
    else
    {
        HAL_SCLDMA_ERR(sclprintf("%s %d, wrong RWmode: %d\n",  __FUNCTION__, __LINE__, enRWMode));
    }

}

void HalSclDmaSetSC3DMAConfig(DrvSclDmaRwConfig_t stSCLDMACfg)
{
    u32 u32yoffset;
    u32 u32coffset;
    u8 u8420md;
    u8 u8DataMd;
    if(stSCLDMACfg.enRWMode == E_DRV_SCLDMA_FRM_R)
    {
        if(stSCLDMACfg.bvFlag.btsBase_0)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_20_L, stSCLDMACfg.u32Base_Y[0]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_28_L, stSCLDMACfg.u32Base_C[0]>>3);
            HalUtilityW4BYTE(REG_SCL3_60_L, stSCLDMACfg.u32Base_V[0]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_1)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_22_L, stSCLDMACfg.u32Base_Y[1]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_2A_L, stSCLDMACfg.u32Base_C[1]>>3);
            HalUtilityW4BYTE(REG_SCL3_62_L, stSCLDMACfg.u32Base_V[1]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_2)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_24_L, stSCLDMACfg.u32Base_Y[2]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_2C_L, stSCLDMACfg.u32Base_C[2]>>3);
            HalUtilityW4BYTE(REG_SCL3_64_L, stSCLDMACfg.u32Base_V[2]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_3)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_26_L, stSCLDMACfg.u32Base_Y[3]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_2E_L, stSCLDMACfg.u32Base_C[3]>>3);
            HalUtilityW4BYTE(REG_SCL3_66_L, stSCLDMACfg.u32Base_V[3]>>3);
        }
        if(stSCLDMACfg.enBuffMode == E_DRV_SCLDMA_BUF_MD_RING)
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA1_30_L, stSCLDMACfg.u8MaxIdx<<3, BIT4|BIT3);
        }
        else
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA1_30_L, 0, BIT4|BIT3);
        }
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_32_L, stSCLDMACfg.u16Width, 0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_33_L, stSCLDMACfg.u16Height,0xFFFF);
        u8420md = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV422? BIT7 : 0;
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_36_L, u8420md, BIT7);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_36_L, 0x06, 0x0F); //422to444_md[1:0], 420to422_md[2], 420to422_md_avg[3]
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_02_L, stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV422 ? BIT14 : 0, BIT14);
        u8DataMd = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT0 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YCSep422 ? BIT1 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep422 ? BIT2 :
                                                                    0;
        HalUtilityW2BYTEMSK(REG_SCL3_2A_L, u8DataMd, BIT0|BIT1|BIT2); // w_422_pack[14]
    }
    else if(stSCLDMACfg.enRWMode == E_DRV_SCLDMA_FRM_W)
    {
        if(stSCLDMACfg.bvFlag.btsBase_0)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_08_L, stSCLDMACfg.u32Base_Y[0]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_10_L, stSCLDMACfg.u32Base_C[0]>>3);
            HalUtilityW4BYTE(REG_SCL3_58_L, stSCLDMACfg.u32Base_V[0]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_1)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_0A_L, stSCLDMACfg.u32Base_Y[1]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_12_L, stSCLDMACfg.u32Base_C[1]>>3);
            HalUtilityW4BYTE(REG_SCL3_5A_L, stSCLDMACfg.u32Base_V[1]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_2)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_0C_L, stSCLDMACfg.u32Base_Y[2]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_14_L, stSCLDMACfg.u32Base_C[2]>>3);
            HalUtilityW4BYTE(REG_SCL3_5C_L, stSCLDMACfg.u32Base_V[2]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_3)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_0E_L, stSCLDMACfg.u32Base_Y[3]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_16_L, stSCLDMACfg.u32Base_C[3]>>3);
            HalUtilityW4BYTE(REG_SCL3_5E_L, stSCLDMACfg.u32Base_V[3]>>3);
        }
        if(stSCLDMACfg.enBuffMode == E_DRV_SCLDMA_BUF_MD_RING)
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA1_18_L, stSCLDMACfg.u8MaxIdx<<3, BIT4|BIT3);
        }
        else
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA1_18_L, 0, BIT4|BIT3);
        }
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_1A_L, stSCLDMACfg.u16Width, 0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_1B_L, stSCLDMACfg.u16Height,0xFFFF);
        u8420md = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV420 ? BIT3 :
                stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT3 :
                                                                0;
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_1E_L, u8420md, BIT3);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_1E_L, BIT2, BIT2);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_01_L, stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV422 ? BIT14 : 0, BIT14);
        u8DataMd = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT0 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YCSep422 ? BIT1 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep422 ? BIT2 :
                                                                    0;
        HalUtilityW2BYTEMSK(REG_SCL3_28_L, u8DataMd, BIT0|BIT1|BIT2); // w_422_pack[14]
        Hal_SCLDMA_GetHVSPResolutionForEnsure(stSCLDMACfg.u16Height);

    }
    else if(stSCLDMACfg.enRWMode == E_DRV_SCLDMA_IMI_R)
    {
        if((stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV420)||(stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420))
        {
            u32yoffset=(stSCLDMACfg.u16Width*16)/8-1;
            u32coffset=(stSCLDMACfg.u16Width*8)/8-1;
        }
        else
        {
            u32yoffset=(stSCLDMACfg.u16Width*16)/8-2;
            u32coffset=(stSCLDMACfg.u16Width*16)/8-2;
        }
        if(stSCLDMACfg.bvFlag.btsBase_0)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_20_L, stSCLDMACfg.u32Base_Y[0]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_28_L, stSCLDMACfg.u32Base_C[0]>>3);
            HalUtilityW4BYTE(REG_SCL3_60_L, stSCLDMACfg.u32Base_V[0]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_1)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_22_L, stSCLDMACfg.u32Base_Y[1]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_2A_L, stSCLDMACfg.u32Base_C[1]>>3);
            HalUtilityW4BYTE(REG_SCL3_62_L, stSCLDMACfg.u32Base_V[1]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_2)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_24_L, stSCLDMACfg.u32Base_Y[2]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_2C_L, stSCLDMACfg.u32Base_C[2]>>3);
            HalUtilityW4BYTE(REG_SCL3_64_L, stSCLDMACfg.u32Base_V[1]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_3)
        {
            HalUtilityW4BYTE(REG_SCL_DMA1_26_L, stSCLDMACfg.u32Base_Y[3]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA1_2E_L, stSCLDMACfg.u32Base_C[3]>>3);
            HalUtilityW4BYTE(REG_SCL3_66_L, stSCLDMACfg.u32Base_V[1]>>3);
        }

        HalUtilityW2BYTEMSK(REG_SCL_DMA1_30_L, stSCLDMACfg.u8MaxIdx<<3, BIT4|BIT3);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_32_L, stSCLDMACfg.u16Width, 0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_33_L, stSCLDMACfg.u16Height,0xFFFF);

        HalUtilityW2BYTEMSK(REG_SCL_DMA1_36_L, stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV422 ? BIT7 : 0, BIT7);
        u8DataMd = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT0 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YCSep422 ? BIT1 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep422 ? BIT2 :
                                                                    0;
        HalUtilityW2BYTEMSK(REG_SCL3_2A_L, u8DataMd, BIT0|BIT1|BIT2); // w_422_pack[14]
        HalUtilityW2BYTEMSK(REG_SCL_DMA1_36_L, 0x06, 0x0F); //422to444_md[1:0], 420to422_md[2], 420to422_md_avg[3]
        HalUtilityW4BYTE(REG_SCL_DMA2_48_L, u32yoffset); // imi offset y
        HalUtilityW4BYTE(REG_SCL_DMA2_4A_L, u32coffset); // imi offset c
        HalUtilityW2BYTEMSK(REG_SCL_DMA2_02_L, 0x01,0x01); // SC3 mode
    }
    else
    {
        HAL_SCLDMA_ERR(sclprintf("%s %d, wrong RWmode: %d\n",  __FUNCTION__, __LINE__, stSCLDMACfg.enRWMode));
    }

}

void HalSclDmaSetDisplayDMAEn(DrvSclDmaRwModeType_e enRWMode, bool bEn)
{
    if(enRWMode == E_DRV_SCLDMA_DBG_R)
    {
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_02_L, bEn ? BIT15 : 0, BIT15);
    }
    else
    {
        HAL_SCLDMA_ERR(sclprintf("%s %d, wrong RWmode: %d\n",  __FUNCTION__, __LINE__, enRWMode));
    }

}


void HalSclDmaSetDisplayDMAConfig(DrvSclDmaRwConfig_t stSCLDMACfg)
{
    u8 u8420md;
    u8 u8DataMd;
    if(stSCLDMACfg.enRWMode == E_DRV_SCLDMA_DBG_R)
    {
        if(stSCLDMACfg.bvFlag.btsBase_0)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_20_L, stSCLDMACfg.u32Base_Y[0]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_28_L, stSCLDMACfg.u32Base_C[0]>>3);
            HalUtilityW4BYTE(REG_SCL3_30_L, stSCLDMACfg.u32Base_V[0]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_1)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_22_L, stSCLDMACfg.u32Base_Y[1]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_2A_L, stSCLDMACfg.u32Base_C[1]>>3);
            HalUtilityW4BYTE(REG_SCL3_32_L, stSCLDMACfg.u32Base_V[1]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_2)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_24_L, stSCLDMACfg.u32Base_Y[2]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_2C_L, stSCLDMACfg.u32Base_C[2]>>3);
            HalUtilityW4BYTE(REG_SCL3_34_L, stSCLDMACfg.u32Base_V[2]>>3);
        }
        if(stSCLDMACfg.bvFlag.btsBase_3)
        {
            HalUtilityW4BYTE(REG_SCL_DMA0_26_L, stSCLDMACfg.u32Base_Y[3]>>3);
            HalUtilityW4BYTE(REG_SCL_DMA0_2E_L, stSCLDMACfg.u32Base_C[3]>>3);
            HalUtilityW4BYTE(REG_SCL3_36_L, stSCLDMACfg.u32Base_V[3]>>3);
        }
        if(stSCLDMACfg.enBuffMode == E_DRV_SCLDMA_BUF_MD_RING)
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA0_30_L, stSCLDMACfg.u8MaxIdx<<3, BIT4|BIT3);
        }
        else
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA0_30_L, 0, BIT4|BIT3);
        }
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_32_L, stSCLDMACfg.u16Width, 0xFFFF);
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_33_L, stSCLDMACfg.u16Height,0xFFFF);
        u8420md = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV422? BIT7 : 0;
         HalUtilityW2BYTEMSK(REG_SCL_DMA0_36_L, u8420md, BIT7);
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_36_L, 0x06, 0x0F); //422to444_md[1:0], 420to422_md[2], 420to422_md_avg[3]
        HalUtilityW2BYTEMSK(REG_SCL_DMA0_02_L, stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUV422 ? BIT14 : 0, BIT14);
        u8DataMd = stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 ? BIT0 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YCSep422 ? BIT1 :
                    stSCLDMACfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep422 ? BIT2 :
                                                                    0;
        HalUtilityW2BYTEMSK(REG_SCL3_1E_L, u8DataMd, BIT0|BIT1|BIT2); // w_422_pack[14]
    }
    else
    {
        HAL_SCLDMA_ERR(sclprintf("%s %d, wrong RWmode: %d\n",  __FUNCTION__, __LINE__, stSCLDMACfg.enRWMode));
    }
}


//SCLDMA Trig
void HalSclDmaSetVSyncRegenMode(DrvSclDmaVsIdType_e enID, DrvSclDmaVsTrigModeType_e enTrigMd)
{
    u16 u16val, u16Mask;
    u32 u32Reg;
    if(enID == E_DRV_SCLDMA_VS_ID_SC)
    {
        u16val = enTrigMd == E_DRV_SCLDMA_VS_TRIG_MODE_HW_IN_VSYNC ? 0x0000 :
                 enTrigMd == E_DRV_SCLDMA_VS_TRIG_MODE_SWTRIGGER       ? 0x0001 :
                                                                 0x0002;
        u16Mask = BIT1|BIT0;
        u32Reg = REG_SCL0_26_L;

    }
    else if(enID == E_DRV_SCLDMA_VS_ID_AFF)
    {

        u16val = enTrigMd == E_DRV_SCLDMA_VS_TRIG_MODE_HW_IN_VSYNC ? 0x0000 :
                 enTrigMd == E_DRV_SCLDMA_VS_TRIG_MODE_HW_DELAY    ? 0x0100 :
                 enTrigMd == E_DRV_SCLDMA_VS_TRIG_MODE_SWTRIGGER       ? 0x0200 :
                                                                 0x0300;

        u16Mask = BIT9|BIT8;
        u32Reg = REG_SCL0_26_L;

    }
    else if(enID == E_DRV_SCLDMA_VS_ID_LDC)
    {
        u16val = enTrigMd == E_DRV_SCLDMA_VS_TRIG_MODE_HW_IN_VSYNC ? 0x0000 :
                 enTrigMd == E_DRV_SCLDMA_VS_TRIG_MODE_HW_DELAY    ? 0x0001 :
                 enTrigMd == E_DRV_SCLDMA_VS_TRIG_MODE_SWTRIGGER       ? 0x0002 :
                                                                 0x0003 ;

        u16Mask = BIT1|BIT0;
        u32Reg = REG_SCL0_20_L;

    }
    else if(enID == E_DRV_SCLDMA_VS_ID_SC3)
    {
        u16val = enTrigMd == E_DRV_SCLDMA_VS_TRIG_MODE_HW_IN_VSYNC ? 0x0000 :
                 enTrigMd == E_DRV_SCLDMA_VS_TRIG_MODE_HW_DELAY    ? 0x0001 :
                 enTrigMd == E_DRV_SCLDMA_VS_TRIG_MODE_SWTRIGGER       ? 0x0002 :
                                                                 0x0003 ;

        u16Mask = BIT1|BIT0;
        u32Reg = REG_SCL0_22_L;

    }
    else if(enID == E_DRV_SCLDMA_VS_ID_DISP)
    {
        u16val = enTrigMd == E_DRV_SCLDMA_VS_TRIG_MODE_HW_IN_VSYNC ? 0x0000 :
                 enTrigMd == E_DRV_SCLDMA_VS_TRIG_MODE_HW_DELAY    ? 0x0001 :
                 enTrigMd == E_DRV_SCLDMA_VS_TRIG_MODE_SWTRIGGER       ? 0x0002 :
                                                                 0x0003 ;

        u16Mask = BIT1|BIT0;
        u32Reg = REG_SCL0_24_L;
    }
    else
    {
        return;
    }


    HalUtilityW2BYTEMSK(u32Reg, u16val, u16Mask);
}

void HalSclDmaSetRegenVSyncVariableWidthEn(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL0_2D_L, bEn ? BIT7 : 0, BIT7);

}

void HalSclDmaSetRegenVSyncRefEdgeMode(DrvSclDmaVsIdType_e enID, DrvSclDmaRegenVsRefModeType_e enRefMd)
{
    u8 u16val = enRefMd == E_DRV_SCLDMA_REF_VS_REF_MODE_RASING ? 1 : 0;

    if(enID == E_DRV_SCLDMA_VS_ID_SC)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_2D_L, u16val, BIT0);
    }
    else if(enID == E_DRV_SCLDMA_VS_ID_AFF)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_2D_L, u16val<<1, BIT1);
    }
    else if(enID == E_DRV_SCLDMA_VS_ID_LDC)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_2D_L, u16val<<2, BIT2);
    }
    else if(enID == E_DRV_SCLDMA_VS_ID_SC3)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_2D_L, u16val<<3, BIT3);
    }
    else if(enID == E_DRV_SCLDMA_VS_ID_DISP)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_2D_L, u16val<<4, BIT4);
    }
    else
    {
    }
}

void HalSclDmaSetRegenVSyncWidth(DrvSclDmaVsIdType_e enID, u16 u16Vs_Width)
{
    if(enID == E_DRV_SCLDMA_VS_ID_SC)
    {
        HalUtilityW2BYTE(REG_SCL0_28_L, u16Vs_Width);
    }
    else if(enID == E_DRV_SCLDMA_VS_ID_AFF)
    {
        HalUtilityW2BYTE(REG_SCL0_29_L, u16Vs_Width);
    }
    else if(enID == E_DRV_SCLDMA_VS_ID_LDC)
    {
        HalUtilityW2BYTE(REG_SCL0_2A_L, u16Vs_Width);
    }
    else if(enID == E_DRV_SCLDMA_VS_ID_SC3)
    {
        HalUtilityW2BYTE(REG_SCL0_2B_L, u16Vs_Width);
    }
    else if(enID == E_DRV_SCLDMA_VS_ID_DISP)
    {
        HalUtilityW2BYTE(REG_SCL0_2C_L, u16Vs_Width);
    }
    else
    {
    }
}


void HalSclDmaSetRegenVSyncStartPoint(DrvSclDmaVsIdType_e enID, u16 u16Vs_St)
{

    if(enID == E_DRV_SCLDMA_VS_ID_SC)
    {
    }
    else if(enID == E_DRV_SCLDMA_VS_ID_AFF)
    {
        HalUtilityW2BYTE(REG_SCL0_27_L, u16Vs_St);
    }
    else if(enID == E_DRV_SCLDMA_VS_ID_LDC)
    {
        HalUtilityW2BYTE(REG_SCL0_21_L, u16Vs_St);
    }
    else if(enID == E_DRV_SCLDMA_VS_ID_SC3)
    {
        HalUtilityW2BYTE(REG_SCL0_23_L, u16Vs_St);
    }
    else if(enID == E_DRV_SCLDMA_VS_ID_DISP)
    {
        HalUtilityW2BYTE(REG_SCL0_25_L, u16Vs_St);
    }
    else
    {
    }
}

void HalSclDmaTrigRegenVSync(DrvSclDmaVsIdType_e enID, bool bEn)
{
    if(enID == E_DRV_SCLDMA_VS_ID_SC)
    {
    }
    else if(enID == E_DRV_SCLDMA_VS_ID_AFF)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_26_L, bEn ? BIT10: 0, BIT10);
    }
    else if(enID == E_DRV_SCLDMA_VS_ID_LDC)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_20_L, bEn ? BIT2: 0, BIT2);
    }
    else if(enID == E_DRV_SCLDMA_VS_ID_SC3)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_22_L, bEn ? BIT2: 0, BIT2);
    }
    else if(enID == E_DRV_SCLDMA_VS_ID_DISP)
    {
        HalUtilityW2BYTEMSK(REG_SCL0_24_L, bEn ? BIT2: 0, BIT2);
    }
    else
    {
    }
}

void HalSclDmaSetDMAEnableDoubleBuffer(bool bEn,DrvSclDmaIdType_e enSCLDMA_ID)
{
    switch(enSCLDMA_ID)
    {
        case E_DRV_SCLDMA_ID_1_W:
        case E_DRV_SCLDMA_ID_2_W:
        HalUtilityW2BYTEMSK(REG_SCL_DMA2_50_L, bEn ? BIT4 : 0, BIT4);
            break;

        case E_DRV_SCLDMA_ID_3_W:
        case E_DRV_SCLDMA_ID_3_R:
        HalUtilityW2BYTEMSK(REG_SCL_DMA2_50_L, bEn ? BIT5 : 0, BIT5);
            break;

        case E_DRV_SCLDMA_ID_PNL_R:
        HalUtilityW2BYTEMSK(REG_SCL_DMA2_50_L, bEn ? BIT6 : 0, BIT6);
            break;

        default:
        HalUtilityW2BYTEMSK(REG_SCL_DMA2_50_L, bEn ? BIT7 : 0, BIT7);
            break;

    }
}
void HalSclDmaSetHandshakeDoubleBuffer(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL_DMA2_50_L, bEn ? BIT0 : 0, BIT0);

}
u8 HalSclDmaGetIspFrameCountReg(void)
{
    u8 u8Value = (u8)((HalUtilityR2BYTEDirect(REG_ISP_7A_L)>>8)&0x7F);
    //u8Value = (u8Value==0) ? 0x7F : (u8Value - 1);
    return u8Value;
}
u16 HalSclDmaGetDMAOutputCount(DrvSclDmaClientType_e enClientType)
{
    u16 u16Idx=0;
    u16 u16def=0;

    u16Idx=u16Idx;
    u16def=u16def;

    switch(enClientType)
    {
        case E_DRV_SCLDMA_1_FRM_W:
            u16Idx = HalUtilityR2BYTEDirect(REG_SCL_DMA2_10_L);
            u16def = HalUtilityR2BYTEDirect(REG_SCL_DMA0_1B_L);
            break;

        case E_DRV_SCLDMA_1_IMI_W:
            u16Idx = HalUtilityR2BYTEDirect(REG_SCL_DMA2_11_L);
            u16def = HalUtilityR2BYTEDirect(REG_SCL_DMA1_4B_L);
            break;

        case E_DRV_SCLDMA_1_SNP_W:
            u16Idx = HalUtilityR2BYTEDirect(REG_SCL_DMA2_12_L);
            u16def = HalUtilityR2BYTEDirect(REG_SCL_DMA0_4B_L);
            break;

        case E_DRV_SCLDMA_2_FRM_W:
            u16Idx = HalUtilityR2BYTEDirect(REG_SCL_DMA2_13_L);
            u16def = HalUtilityR2BYTEDirect(REG_SCL_DMA0_63_L);
            break;

        case E_DRV_SCLDMA_2_FRM2_W:
            u16Idx = HalUtilityR2BYTEDirect(REG_SCL_DMA2_18_L);
            u16def = HalUtilityR2BYTEDirect(REG_SCL_DMA1_63_L);
            break;

        case E_DRV_SCLDMA_2_IMI_W:
            u16Idx = HalUtilityR2BYTEDirect(REG_SCL_DMA2_14_L);
            u16def = HalUtilityR2BYTEDirect(REG_SCL_DMA0_7B_L);
            break;

        case E_DRV_SCLDMA_3_FRM_R:
            u16Idx = HalUtilityR2BYTEDirect(REG_SCL_DMA2_16_L);
            u16def = HalUtilityR2BYTEDirect(REG_SCL_DMA1_33_L);
            break;

        case E_DRV_SCLDMA_3_FRM_W:
            u16Idx = HalUtilityR2BYTEDirect(REG_SCL_DMA2_15_L);
            u16def = HalUtilityR2BYTEDirect(REG_SCL_DMA1_1B_L);
            break;

        case E_DRV_SCLDMA_4_FRM_R:
            u16Idx = HalUtilityR2BYTEDirect(REG_SCL_DMA2_17_L);
            u16def = HalUtilityR2BYTEDirect(REG_SCL_DMA0_33_L);
            break;
        default:
            u16Idx = 0x0;
            u16def = 0x0;
            break;
    }
    return ((u16Idx));

}
u16 HalSclDmaGetOutputHsize(DrvSclDmaClientType_e enClientType)
{
    u16 u16def=0;

    switch(enClientType)
    {
        case E_DRV_SCLDMA_1_FRM_W:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA0_1A_L);
            break;

        case E_DRV_SCLDMA_1_IMI_W:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA1_4A_L);
            break;

        case E_DRV_SCLDMA_1_SNP_W:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA0_4A_L);
            break;

        case E_DRV_SCLDMA_2_FRM_W:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA0_62_L);
            break;

        case E_DRV_SCLDMA_2_FRM2_W:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA1_62_L);
            break;

        case E_DRV_SCLDMA_2_IMI_W:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA0_7A_L);
            break;

        case E_DRV_SCLDMA_3_FRM_R:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA1_32_L);
            break;

        case E_DRV_SCLDMA_3_FRM_W:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA1_1A_L);
            break;

        case E_DRV_SCLDMA_4_FRM_R:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA0_32_L);
            break;
        default:
            u16def = 0x0;
            break;
    }
    return ((u16def));

}
u16 HalSclDmaGetOutputVsize(DrvSclDmaClientType_e enClientType)
{
    u16 u16def;

    switch(enClientType)
    {
        case E_DRV_SCLDMA_1_FRM_W:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA0_1B_L);
            break;

        case E_DRV_SCLDMA_1_IMI_W:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA1_4B_L);
            break;

        case E_DRV_SCLDMA_1_SNP_W:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA0_4B_L);
            break;

        case E_DRV_SCLDMA_2_FRM_W:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA0_63_L);
            break;

        case E_DRV_SCLDMA_2_FRM2_W:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA1_63_L);
            break;

        case E_DRV_SCLDMA_2_IMI_W:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA0_7B_L);
            break;

        case E_DRV_SCLDMA_3_FRM_R:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA1_33_L);
            break;

        case E_DRV_SCLDMA_3_FRM_W:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA1_1B_L);
            break;

        case E_DRV_SCLDMA_4_FRM_R:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA0_33_L);
            break;
        default:
            u16def = 0x0;
            break;
    }
    return ((u16def));

}
u16 HalSclDmaGetRWIdx(DrvSclDmaClientType_e enClient)
{
    u16 u16Idx;
    switch(enClient)
    {
        case E_DRV_SCLDMA_1_FRM_W:
            u16Idx = HalUtilityR2BYTEMaskDirect(REG_SCL_DMA2_03_L, (BIT1|BIT0));
            break;

        case E_DRV_SCLDMA_1_IMI_W:
            u16Idx = (HalUtilityR2BYTEMaskDirect(REG_SCL_DMA2_03_L, (BIT3|BIT2)))>>2;
            break;

        case E_DRV_SCLDMA_1_SNP_W:
            u16Idx = (HalUtilityR2BYTEMaskDirect(REG_SCL_DMA2_03_L, (BIT5|BIT4)))>>4;
            break;

        case E_DRV_SCLDMA_2_FRM_W:
            u16Idx = (HalUtilityR2BYTEMaskDirect(REG_SCL_DMA2_03_L, (BIT7|BIT6)))>>6;
            break;

        case E_DRV_SCLDMA_2_FRM2_W:
            u16Idx = (HalUtilityR2BYTEMaskDirect(REG_SCL_DMA2_03_L, (BIT15|BIT14)))>>14;
            break;

        case E_DRV_SCLDMA_2_IMI_W:
            u16Idx = (HalUtilityR2BYTEMaskDirect(REG_SCL_DMA2_03_L, (BIT9|BIT8)))>>8;
            break;

        case E_DRV_SCLDMA_3_FRM_R:
            u16Idx = (HalUtilityR2BYTEMaskDirect(REG_SCL_DMA2_03_L, (BIT11|BIT10)))>>10;
            break;

        case E_DRV_SCLDMA_3_FRM_W:
            u16Idx = (HalUtilityR2BYTEMaskDirect(REG_SCL_DMA2_03_L, (BIT13|BIT12)))>>12;
            break;

        case E_DRV_SCLDMA_4_FRM_R:
            u16Idx = (HalUtilityR2BYTEMaskDirect(REG_SCL_DMA2_03_L, (BIT15|BIT14)))>>14;
            break;
        default:
            u16Idx = 0xFF;
            break;
    }
    return u16Idx;
}

#undef HAL_SCLDMA_C
