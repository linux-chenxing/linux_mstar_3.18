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
#define HAL_HVSP_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition

#include "drv_scl_os.h"
#include "hal_utility.h"
#include "hal_scl_reg.h"
#include "hal_scl_util.h"
#include "drv_scl_hvsp_st.h"
#include "drv_scl_irq_st.h"
#include "drv_scl_irq.h"
#include "hal_scl_hvsp.h"
#include "drv_scl_cmdq.h"
#include "drv_scl_dbg.h"
#include "drv_scl_pq_define.h"
#include "Infinity3e_Main.h"             // table config parameter
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HVSP_CMD_TRIG_BUFFER_SIZE 100
#define IsInputSource(Src)             (genIpType == (Src))
#define IsClkIncrease(height,width,rate)    (((height) > 720 || (width) > 1280)&& (rate) < 172000000)
#define IsClkDecrease(height,width,rate)    (((height) <= 720 && (width) <= 1280)&& (rate) >= 172000000)
#define IsRegType(type)               (gstHvspCmdTrigCfg.enType == (type))
#define IsIPFirstSet(ip)             (gbhvspset[(ip)]==0)
#define HAL_HVSP_RATIO(input, output)           ((u32)((u64)((u32)(input) * 1048576) / (u32)(output)))
#define DISABLE_CLK 0x1
#define LOW_CLK 0x4
#define LOCK_FB()                       (gblockfbmg)
#define _SetHVSPType(ID,u8type)                    (gstSclFea[(ID)].u16ModeYCVH |= (u8type))
#define _ReSetHVSPType(ID,u8type)                  (gstSclFea[(ID)].u16ModeYCVH &= ~(u8type))
#define IsCannotUseCmdqToSetReg()      (gbUseCMDQ == 0)

typedef struct
{
    u32 u32Reg;
    u16 u16Msk;
    u16 u16Val;
}HalSclHvspCmdTrigBufferType_t;

typedef struct
{
    HalSclHvspCmdTrigBufferType_t stCmdTrigBuf[HVSP_CMD_TRIG_BUFFER_SIZE];
    u16 u16Idx;
}HalSclHvspCmdTrigBufferConfig_t;

typedef struct
{
    u16 u16ModeYCVH;
    u16 u16DithCoring;
}HalSclHvspScalingFeatureConfig_t;
//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u32 HVSP_RIU_BASE;
/////////////////
/// gstHvspCmdTrigCfg
/// To save set Register type and used in scaling function.
////////////////
DrvSclHvspCmdTrigConfig_t gstHvspCmdTrigCfg;
/////////////////
/// gstHvspCmdTrigBufCfg
/// To save CMD and used in scaling function.
////////////////
HalSclHvspCmdTrigBufferConfig_t gstHvspCmdTrigBufCfg;
/////////////////
/// genIpType
/// To save Input Source Type.
////////////////
DrvSclHvspIpMuxType_e genIpType;
/////////////////
/// gu16height
/// To save resolution and used in dynamic change clk.
////////////////
u16 gu16height[2]={0,0};
/////////////////
/// gu16height
/// To save resolution and used in dynamic change clk.
////////////////
u16 gu16width[2]={0,0};
/////////////////
/// genFilterMode
/// To save scaling type for handle CMDQ can't mask issue.
////////////////
HalSclHvspFilterMode_e genFilterMode;
/////////////////
/// gbhvspset
/// The first time set HVSP scaling config can't use CMDQ(no sync to trig),so need to save this information.
////////////////
bool gbhvspset[E_DRV_SCLHVSP_ID_MAX]={0,0,0};
HalSclHvspScalingFeatureConfig_t gstSclFea[E_DRV_SCLHVSP_ID_MAX];
bool gbUseCMDQ;
bool gbResetDNR;
bool gblockfbmg = 0;
ST_HVSP_SIZE_CONFIG gstHalSrcSize;
bool gbSramCMDQ = 0;

//-------------------------------------------------------------------------------------------------
//  Functions
//-------------------------------------------------------------------------------------------------
void HalSclHvspExit(void)
{
    gblockfbmg = 0;
    genIpType = 0;
    genFilterMode = 0;
    gu16width[0]=0;
    gu16height[0]=0;
    gu16width[1]=0;
    gu16height[1]=0;
    DrvSclOsMemset(&gstHvspCmdTrigCfg, 0,sizeof(DrvSclHvspCmdTrigConfig_t));
    DrvSclOsMemset(&gstHvspCmdTrigBufCfg, 0,sizeof(HalSclHvspCmdTrigBufferConfig_t));
}
//============CLK================================
#if CONFIG_OF
void HalSclHvspSetIdClkOnOff(bool bEn,DrvSclHvspClkConfig_t* stclk)
{
    DrvSclOsClkStruct_t* pstclock = NULL;
    u32 u32Events;
    if(DrvSclIrqGetEachDMAEn()||(!DrvSclIrqGetIsBlankingRegion()))
    {
        SCL_ERR("[IDCLK]%s %d,Wait\n", __FUNCTION__, __LINE__);
        DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(),
            (E_DRV_SCLIRQ_EVENT_ISPFRMEND|E_DRV_SCLIRQ_EVENT_FRMENDSYNC), &u32Events, E_DRV_SCLOS_EVENT_MD_AND, 200); // get status: FRM END
    }
    if(bEn)
    {
        if(IsInputSource(E_DRV_SCLHVSP_IP_MUX_PAT_TGEN))
        {
            HalUtilityW2BYTEMSK(REG_SCL_CLK_61_L,0x0000,0x0F00);//h61
        }

        if(IsInputSource(E_DRV_SCLHVSP_IP_MUX_BT656))
        {
            HalUtilityW2BYTEMSK(REG_BLOCK_24_L,0x0004,0x0005);//idclk
            HalUtilityW2BYTEMSK(REG_CHIPTOP_0F_L,0x0010,0x0010);//ccir mode
            if (DrvSclOsClkGetEnableCount(stclk->idclk)==0)
            {
                if (NULL != (pstclock = DrvSclOsClkGetParentByIndex(stclk->idclk, 1)))
                {
                    DrvSclOsClkSetParent(stclk->idclk, pstclock);
                    DrvSclOsClkPrepareEnable(stclk->idclk);
                    //sclprintf("[idclk]enable count=%d\n", DrvSclOsClkGetEnableCount(stclk->idclk));
                }
                else
                {
                    sclprintf("[idclk]BT656 NULL\n");

                }
            }
        }
        else
        {
            HalUtilityW2BYTEMSK(REG_CHIPTOP_0F_L,0x0000,0x0010);//ccir mode
            HalUtilityW2BYTEMSK(REG_BLOCK_24_L,0x0000,0x0005);//idclk
            if (DrvSclOsClkGetEnableCount(stclk->idclk)==0)
            {
                if (NULL != (pstclock = DrvSclOsClkGetParentByIndex(stclk->idclk, 0)))
                {
                    DrvSclOsClkSetParent(stclk->idclk, pstclock);
                    DrvSclOsClkPrepareEnable(stclk->idclk);
                }
                else
                {
                    SCL_ERR("[HAlHVSP]CLK Tree Not Ready \n");
                }
            }
        }
    }
    else
    {
        if(IsInputSource(E_DRV_SCLHVSP_IP_MUX_PAT_TGEN))
        {
            HalUtilityW2BYTEMSK(REG_SCL_CLK_61_L,0x0100,0x0F00);//h61
        }

        if(IsInputSource(E_DRV_SCLHVSP_IP_MUX_BT656))
        {
            HalUtilityW2BYTEMSK(REG_BLOCK_24_L,0x0005,0x0005);//idclk
            HalUtilityW2BYTEMSK(REG_CHIPTOP_0F_L,0x0000,0x0010);//ccir mode
            if (NULL != (pstclock = DrvSclOsClkGetParentByIndex(stclk->idclk, 0)))
            {
                DrvSclOsClkSetParent(stclk->idclk, pstclock);
            }
            else
            {
                SCL_ERR("[HAlHVSP]CLK Tree Not Ready \n");
            }
            while (DrvSclOsClkGetEnableCount(stclk->idclk))
            {
                DrvSclOsClkDisableUnprepare(stclk->idclk);
            }
        }
        else
        {
            HalUtilityW2BYTEMSK(REG_BLOCK_24_L,0x0001,0x0005);//idclk
            HalUtilityW2BYTEMSK(REG_CHIPTOP_0F_L,0x0000,0x0010);//ccir mode
            if (NULL != (pstclock = DrvSclOsClkGetParentByIndex(stclk->idclk, 0)))
            {
                DrvSclOsClkSetParent(stclk->idclk, pstclock);
            }
            else
            {
                SCL_ERR("[HAlHVSP]CLK Tree Not Ready \n");
            }
            while (DrvSclOsClkGetEnableCount(stclk->idclk))
            {
                DrvSclOsClkDisableUnprepare(stclk->idclk);
            }
        }

    }
}
void HalSclHvspSetClkRate(u8 u8Idx)
{
    if((u8Idx &E_HALSCLHVSP_CLKATTR_BT656))//BT656
    {
        HalUtilityW2BYTEMSK(REG_BLOCK_24_L,0x0004,0x0005);//idclk
        HalUtilityW2BYTEMSK(REG_CHIPTOP_0F_L,0x0010,0x0010);//ccir mode
    }
    else if((u8Idx &E_HALSCLHVSP_CLKATTR_ISP))//ISP
    {
        sclprintf("[hal]ISP xxxxxxxxx\n");
        HalUtilityW2BYTEMSK(REG_CHIPTOP_0F_L,0x0000,0x0010);//ccir mode
        HalUtilityW2BYTEMSK(REG_BLOCK_24_L,0x0000,0x0005);//idclk
    }
    if((u8Idx &E_HALSCLHVSP_CLKATTR_FORCEMODE))
    {
        sclprintf("[hal]FORCEMODE xxxxxxxxx\n");
        HalUtilityW2BYTEMSK(REG_SCL_CLK_61_L,0x0000,0x0F00);//h61
    }
}

void HalSclHvspFclk1(DrvSclHvspClkConfig_t *stclk)
{
#if CLKDynamic
    DrvSclOsClkStruct_t* pstclock = NULL;
    if (DrvSclOsClkGetEnableCount(stclk->fclk1)==0)
    {
    }
    else
    {
        if(IsClkIncrease(gu16height[0],gu16width[0],DrvSclOsClkGetRate(stclk->fclk1)))
        {
            if (NULL != (pstclock = DrvSclOsClkGetParentByIndex(stclk->fclk1, 0)))
            {
                DrvSclOsClkSetParent(stclk->fclk1, pstclock);
            }
            else
            {
                SCL_ERR("[HAlHVSP]CLK Tree Not Ready \n");
            }
    #if OSDinverseBug
            if (NULL != (pstclock = DrvSclOsClkGetParentByIndex(stclk->fclk2, 0)))
            {
                DrvSclOsClkSetParent(stclk->fclk2, pstclock);
            }
            else
            {
                SCL_ERR("[HAlHVSP]CLK Tree Not Ready \n");
            }
    #endif
        }
        else if(IsClkDecrease(gu16height[0],gu16width[0],DrvSclOsClkGetRate(stclk->fclk1)))
        {
            if (NULL != (pstclock = DrvSclOsClkGetParentByIndex(stclk->fclk1, 1)))
            {
                DrvSclOsClkSetParent(stclk->fclk1, pstclock);
            }
            else
            {
                SCL_ERR("[HAlHVSP]CLK Tree Not Ready \n");
            }
    #if OSDinverseBug
            if (NULL != (pstclock = DrvSclOsClkGetParentByIndex(stclk->fclk2, 1)))
            {
                DrvSclOsClkSetParent(stclk->fclk2, pstclock);
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

void HalSclHvspFclk2(DrvSclHvspClkConfig_t *stclk)
{
    #if OSDinverseBug
    #else
    DrvSclOsClkStruct_t* pstclock = NULL;
    if (DrvSclOsClkGetEnableCount(stclk->fclk2)==0)
    {
    }
    else
    {
        if(IsClkIncrease(gu16height[1],gu16width[1],DrvSclOsClkGetRate(stclk->fclk2)))
        {
            if (NULL != (pstclock = DrvSclOsClkGetParentByIndex(stclk->fclk2, 0)))
            {
                DrvSclOsClkSetParent(stclk->fclk2, pstclock);
            }
            else
            {
                SCL_ERR("[HAlHVSP]CLK Tree Not Ready \n");
            }
            //sclprintf("[HVSP]CLK UP\n");
        }
        else if(IsClkDecrease(gu16height[1],gu16width[1],DrvSclOsClkGetRate(stclk->fclk2)))
        {
            if (NULL != (pstclock = DrvSclOsClkGetParentByIndex(stclk->fclk2, 1)))
            {
                DrvSclOsClkSetParent(stclk->fclk2, pstclock);
            }
            else
            {
                SCL_ERR("[HAlHVSP]CLK Tree Not Ready \n");
            }
            //sclprintf("[HVSP]CLK down\n");
        }
    }
#endif
}

#else
void HalSclHvspSetIdClkOnOff(bool bEn,DrvSclHvspClkConfig_t* stclk)
{
    sclprintf("[hal]NO OF\n");
    if(bEn)
    {
        if(IsInputSource(E_DRV_SCLHVSP_IP_MUX_PAT_TGEN))
        {
            HalUtilityW2BYTEMSK(REG_SCL_CLK_61_L,0x0000,0x0F00);//h61
        }

        if(IsInputSource(E_DRV_SCLHVSP_IP_MUX_BT656))
        {
            HalUtilityW2BYTEMSK(REG_BLOCK_24_L,0x0004,0x0005);//idclk
            HalUtilityW2BYTEMSK(REG_CHIPTOP_0F_L,0x0010,0x0010);//ccir mode
            if ((HalUtilityR2BYTEDirect(REG_SCL_CLK_63_L)&DISABLE_CLK))
            {
                HalUtilityW2BYTEMSK(REG_SCL_CLK_63_L,0x0004,0x000F);//h63
            }
        }
        else
        {
            HalUtilityW2BYTEMSK(REG_CHIPTOP_0F_L,0x0000,0x0010);//ccir mode
            HalUtilityW2BYTEMSK(REG_BLOCK_24_L,0x0000,0x0005);//idclk
            if ((HalUtilityR2BYTEDirect(REG_SCL_CLK_63_L)&DISABLE_CLK))
            {
                HalUtilityW2BYTEMSK(REG_SCL_CLK_63_L,0x0000,0x000F);//h63
            }
        }

    }
    else
    {
        if(IsInputSource(E_DRV_SCLHVSP_IP_MUX_PAT_TGEN))
        {
            HalUtilityW2BYTEMSK(REG_SCL_CLK_61_L,0x0100,0x0F00);//h61
        }

        if(IsInputSource(E_DRV_SCLHVSP_IP_MUX_BT656))
        {
            HalUtilityW2BYTEMSK(REG_BLOCK_24_L,0x0005,0x0005);//idclk
            HalUtilityW2BYTEMSK(REG_CHIPTOP_0F_L,0x0000,0x0010);//ccir mode
            HalUtilityW2BYTEMSK(REG_SCL_CLK_63_L,0x0005,0x000F);//h63
        }
        else
        {
            HalUtilityW2BYTEMSK(REG_BLOCK_24_L,0x0001,0x0005);//idclk
            HalUtilityW2BYTEMSK(REG_CHIPTOP_0F_L,0x0000,0x0010);//ccir mode
            HalUtilityW2BYTEMSK(REG_SCL_CLK_63_L,0x0001,0x000F);//h63
        }

    }
}
void HalSclHvspSetClkRate(u8 u8Idx)
{
    if((u8Idx &E_HALSCLHVSP_CLKATTR_BT656))//BT656
    {
        HalUtilityW2BYTEMSK(REG_BLOCK_24_L,0x0004,0x0005);//idclk
        HalUtilityW2BYTEMSK(REG_CHIPTOP_0F_L,0x0010,0x0010);//ccir mode
    }
    else if((u8Idx &E_HALSCLHVSP_CLKATTR_ISP))//ISP
    {
        sclprintf("[hal]ISP xxxxxxxxx\n");
        HalUtilityW2BYTEMSK(REG_CHIPTOP_0F_L,0x0000,0x0010);//ccir mode
        HalUtilityW2BYTEMSK(REG_BLOCK_24_L,0x0000,0x0005);//idclk
    }
    if((u8Idx &E_HALSCLHVSP_CLKATTR_FORCEMODE))
    {
        sclprintf("[hal]FORCEMODE xxxxxxxxx\n");
        HalUtilityW2BYTEMSK(REG_SCL_CLK_61_L,0x0000,0x0F00);//h61
    }
}

void HalSclHvspFclk1(DrvSclHvspClkConfig_t *stclk)
{
    if ((HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L)&DISABLE_CLK))
    {
    }
    else
    {
        if((gu16height[0]>720||gu16width[0]>1280) && (HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L)&LOW_CLK))
        {
            HalUtilityW2BYTEMSK(REG_SCL_CLK_64_L,0x0000,0x000F);//h61
        }
        else if((gu16height[0]<=720 && ,gu16width[0]<=1280)&& !(HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L)&LOW_CLK))
        {
            HalUtilityW2BYTEMSK(REG_SCL_CLK_64_L,LOW_CLK,0x000F);//h61
        }
    }

}

void HalSclHvspFclk2(DrvSclHvspClkConfig_t *stclk)
{
    if ((HalUtilityR2BYTEDirect(REG_SCL_CLK_65_L)&DISABLE_CLK))
    {
    }
    else
    {
        if((gu16height[1]>720||gu16width[1]>1280) && (HalUtilityR2BYTEDirect(REG_SCL_CLK_65_L)&LOW_CLK))
        {
            HalUtilityW2BYTEMSK(REG_SCL_CLK_65_L,0x0000,0x000F);//h61
        }
        else if((gu16height[1]<=720 && ,gu16width[1]<=1280) && !(HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L)&LOW_CLK))
        {
            HalUtilityW2BYTEMSK(REG_SCL_CLK_65_L,LOW_CLK,0x000F);//h61
        }
    }
}


#endif

//===========================================================
void HalSclHvspSetFrameBufferManageLock(bool bEn)
{
    gblockfbmg = bEn;
}
void HalSclHvspSetCmdqTrigCfg(DrvSclHvspCmdTrigConfig_t stCmdTrigCfg)
{
    gbUseCMDQ = DrvSclIrqGetEachDMAEn();
    gstHvspCmdTrigCfg.enType = stCmdTrigCfg.enType;
    gstHvspCmdTrigCfg.u8Fmcnt = stCmdTrigCfg.u8Fmcnt;
}
void HalSclHvspSetResetDNR(bool bEn)
{
    gbResetDNR = bEn;
}
void HalSclHvspGetCmdqTrigCfg(DrvSclHvspCmdTrigConfig_t *pCfg)
{
    DrvSclOsMemcpy(pCfg, &gstHvspCmdTrigCfg, sizeof(DrvSclHvspCmdTrigConfig_t));
}

void HalSclHvspSetCmdqTrigFrameCnt(u16 u16Idx)
{
    gstHvspCmdTrigBufCfg.u16Idx = u16Idx;
}

void HalSclHvspSetCmdqTrigFire(void)
{
    u16 i;
    DrvSclCmdqCmdReg_t stCfg;
    //sclprintf("[HVSP]HalSclHvspSetCmdqTrigFire   gstHvspCmdTrigCfg.enType:%d \n",gstHvspCmdTrigCfg.enType);
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_CMDQ_LDC_SYNC))
    {
        DrvSclCmdqGetModuleMutex(E_DRV_SCLCMDQ_TYPE_IP0,1);
        for(i=0; i< gstHvspCmdTrigBufCfg.u16Idx; i++)
        {
            DrvSclCmdqFillCmd(&stCfg, gstHvspCmdTrigBufCfg.stCmdTrigBuf[i].u32Reg,
                gstHvspCmdTrigBufCfg.stCmdTrigBuf[i].u16Val, gstHvspCmdTrigBufCfg.stCmdTrigBuf[i].u16Msk);
            DrvSclCmdqWriteCmd(E_DRV_SCLCMDQ_TYPE_IP0,stCfg,0);
        }
        if(gbResetDNR)
        {
            DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_BRESETDNR);
            gbResetDNR = 0;
        }
        DrvSclCmdqFire(E_DRV_SCLCMDQ_TYPE_IP0,TRUE);
        DrvSclCmdqGetModuleMutex(E_DRV_SCLCMDQ_TYPE_IP0,0);
    }
    else if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_POLL_LDC_SYNC))
    {
        for(i=0; i< gstHvspCmdTrigBufCfg.u16Idx; i++)
        {
            HalUtilityW2BYTEMSK(gstHvspCmdTrigBufCfg.stCmdTrigBuf[i].u32Reg,
                      gstHvspCmdTrigBufCfg.stCmdTrigBuf[i].u16Val,
                      gstHvspCmdTrigBufCfg.stCmdTrigBuf[i].u16Msk);
        }
    }

}


bool _HalSclHvspWriteCmd(u32 u32Reg,  u16 u16Val, u16 u16Msk)
{
    u16 u16idx = gstHvspCmdTrigBufCfg.u16Idx;
    gstHvspCmdTrigBufCfg.stCmdTrigBuf[u16idx].u32Reg = u32Reg;
    gstHvspCmdTrigBufCfg.stCmdTrigBuf[u16idx].u16Msk = u16Msk;
    gstHvspCmdTrigBufCfg.stCmdTrigBuf[u16idx].u16Val = u16Val;
    //sclprintf("[HVSP]_HalSclHvspWriteCmd   u32Reg:%lx u16Msk:%hx u16Val:%hx \n",u32Reg,u16Msk,u16Val);
    gstHvspCmdTrigBufCfg.u16Idx++;
    if(gstHvspCmdTrigBufCfg.u16Idx >= HVSP_CMD_TRIG_BUFFER_SIZE)
    {
        gstHvspCmdTrigBufCfg.u16Idx = HVSP_CMD_TRIG_BUFFER_SIZE-1;
        SCL_ERR("[HVSP]CMDQ FULL\n");
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

void HalSclHvspSetRiuBase(u32 u32RiuBase)
{
    HVSP_RIU_BASE = u32RiuBase;
}
void HalSclHvspSetInputSrcSize(ST_HVSP_SIZE_CONFIG stSrc)
{
    gstHalSrcSize = stSrc;
}

void HalSclHvspSetReset(void)
{
    gbhvspset[0]= 0;
    gbhvspset[1]= 0;
    gbhvspset[2]= 0;
    gbUseCMDQ = 0;
    gbResetDNR = 0;
    gstSclFea[0].u16ModeYCVH= 0;
    gstSclFea[1].u16ModeYCVH= 0;
    gstSclFea[2].u16ModeYCVH= 0;
    gstSclFea[0].u16DithCoring= 0;
    gstSclFea[1].u16DithCoring= 0;
    gstSclFea[2].u16DithCoring= 0;
    HalSclHvspSetNlmEn(0);
    //HalSclHvspSetHwInputMux(E_DRV_SCLHVSP_IP_MUX_BT656);
    HalUtilityW2BYTEMSK(REG_SCL2_50_L, 0, BIT0);//crop1
    HalUtilityW2BYTEMSK(REG_SCL2_58_L, 0, BIT0);//crop2
    HalSclHvspSetIpmYCMReadEn(0);
    HalSclHvspSetIpmYCMWriteEn(0);
    HalSclHvspSetIpmCiirReadEn(0);
    HalSclHvspSetIpmCiirWriteEn(0);
    HalSclHvspSetLdcPathSel(0);
    HalSclHvspSetLdcBypass(1);
    HalSclHvspSetPrv2CropOnOff(0);
    HalSclHvspPriMaskbEn(E_HAL_SCLHVSP_TRIGGER_OFF);
    HalSclHvspSetDNRReset(1);
    HalSclHvspSetSWReset(1);
}
void HalSclHvspSetClkOFF(void)
{
    HalUtilityW2BYTEMSK(REG_SCL_CLK_64_L,0x0001,0x000F);// ToDo
}
void HalSclHvspSetVipSize(u16 u16Width, u16 u16Height)
{
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(E_DRV_SCLHVSP_ID_1) || IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(REG_VIP_LCE_6E_L, u16Width, 0x0FFF);
        HalUtilityW2BYTEMSK(REG_VIP_LCE_6F_L, u16Height, 0x1FFF);
    }
    else
    {
        _HalSclHvspWriteCmd(REG_VIP_LCE_6E_L, u16Width, 0x0FFF);
        _HalSclHvspWriteCmd(REG_VIP_LCE_6F_L, u16Height, 0x1FFF);
    }
}
void HalSclHvspSetWdrGlobalSize(u16 u16Width, u16 u16Height)
{
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(E_DRV_SCLHVSP_ID_1) || IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(REG_VIP_WDR_35_L, u16Width-1, 0x0FFF);
        HalUtilityW2BYTEMSK(REG_VIP_WDR_36_L, u16Height-1, 0x1FFF);
    }
    else
    {
        _HalSclHvspWriteCmd(REG_VIP_WDR_35_L, u16Width-1, 0x0FFF);
        _HalSclHvspWriteCmd(REG_VIP_WDR_36_L, u16Height-1, 0x1FFF);
    }
}
void HalSclHvspSetWdrLocalSize(u16 u16Width, u16 u16Height)
{
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(E_DRV_SCLHVSP_ID_1) || IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(REG_VIP_WDR_7C_L, u16Width-1, 0x0FFF);
        HalUtilityW2BYTEMSK(REG_VIP_WDR_7D_L, u16Height-1, 0x1FFF);
    }
    else
    {
        _HalSclHvspWriteCmd(REG_VIP_WDR_7C_L, u16Width-1, 0x0FFF);
        _HalSclHvspWriteCmd(REG_VIP_WDR_7D_L, u16Height-1, 0x1FFF);
    }
}
void HalSclHvspSetMXnrSize(u16 u16Width, u16 u16Height)
{
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(E_DRV_SCLHVSP_ID_1) || IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(REG_VIP_SCNR_0A_L, u16Width -1, 0x0FFF);
        HalUtilityW2BYTEMSK(REG_VIP_SCNR_0B_L, u16Height -1, 0x1FFF);
    }
    else
    {
        _HalSclHvspWriteCmd(REG_VIP_SCNR_0A_L, u16Width -1, 0x0FFF);
        _HalSclHvspWriteCmd(REG_VIP_SCNR_0B_L, u16Height -1, 0x1FFF);
    }
}
void HalSclHvspSetUVadjSize(u16 u16Width, u16 u16Height)
{
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(E_DRV_SCLHVSP_ID_1) || IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(REG_VIP_SCNR_3B_L, u16Width-1, 0x0FFF);
        HalUtilityW2BYTEMSK(REG_VIP_SCNR_3C_L, u16Height-1, 0x1FFF);
    }
    else
    {
        _HalSclHvspWriteCmd(REG_VIP_SCNR_3B_L, u16Width-1, 0x0FFF);
        _HalSclHvspWriteCmd(REG_VIP_SCNR_3C_L, u16Height-1, 0x1FFF);
    }
}
void HalSclHvspSetXnrSize(u16 u16Width, u16 u16Height)
{
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(E_DRV_SCLHVSP_ID_1) || IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(REG_VIP_SCNR_5C_L, u16Width-1, 0x0FFF);
        HalUtilityW2BYTEMSK(REG_VIP_SCNR_5D_L, u16Height-1, 0x1FFF);
    }
    else
    {
        _HalSclHvspWriteCmd(REG_VIP_SCNR_5C_L, u16Width-1, 0x0FFF);
        _HalSclHvspWriteCmd(REG_VIP_SCNR_5D_L, u16Height-1, 0x1FFF);
    }
}
void HalSclHvspPriMaskWidth(u16 u16W)
{
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(E_DRV_SCLHVSP_ID_1) || IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(REG_VIP_SCNR_15_L, u16W-1, 0xFFFF);
    }
    else
    {
        _HalSclHvspWriteCmd(REG_VIP_SCNR_15_L, u16W-1, 0x0FFF);
    }
}
void HalSclHvspPriMaskHeight(u16 u16H)
{
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(E_DRV_SCLHVSP_ID_1) || IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(REG_VIP_SCNR_16_L, u16H-1, 0xFFFF);
    }
    else
    {
        _HalSclHvspWriteCmd(REG_VIP_SCNR_16_L, u16H-1, 0x0FFF);
    }
}

void HalSclHvspSetSWReset(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL0_01_L, bEn ? BIT0 : 0, BIT0);
    HalUtilityW2BYTEMSK(REG_SCL0_01_L, 0, BIT0);
}
void HalSclHvspSetDNRReset(bool bEn)
{
    // for issue reset rstz_ctrl_clk_miu_sc_dnr cause to overwrite 0x0
    HalUtilityW2BYTEMSK(REG_SCL_DNR1_24_L, bEn ? BIT6 : 0, BIT6);
    HalUtilityW2BYTEMSK(REG_SCL_DNR1_60_L, bEn ? BIT0 : 0, BIT0);
}

void HalSclHvspSetNlmLineBufferSize(u16 u16Width, u16 u16Height)
{
	  HalUtilityW2BYTEMSK(REG_SCL0_19_L, 0x8000, 0x8000);
      if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(E_DRV_SCLHVSP_ID_1)|| IsCannotUseCmdqToSetReg())
      {
        HalUtilityW2BYTEMSK(REG_SCL0_1A_L, u16Width, 0x0FFF);
        HalUtilityW2BYTEMSK(REG_SCL0_1B_L, u16Height, 0x1FFF);
      }
      else
      {
          _HalSclHvspWriteCmd(REG_SCL0_1A_L, u16Width, 0x0FFF);
          _HalSclHvspWriteCmd(REG_SCL0_1B_L, u16Height, 0x1FFF);
      }
}

void HalSclHvspSetNlmEn(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL0_18_L, bEn ? BIT0 : 0, BIT0);
}
void HalSclHvspSetVpsSRAMEn(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_VIP_PK_10_L, bEn ? BIT7 : 0, BIT7);
}


//-------------------------------------------------------------------------------------------------
// Crop
//-------------------------------------------------------------------------------------------------
void HalSclHvspSetBt656SrcConfig(bool bEn,u16 u16Lineoffset)
{
    if(IsInputSource(E_DRV_SCLHVSP_IP_MUX_BT656) )
    {
        HalUtilityW2BYTEMSK(REG_SCL0_61_L, bEn ? BIT7 : 0, BIT7);
        HalUtilityW2BYTEMSK(REG_SCL0_63_L, u16Lineoffset, 0x07FF);
        HalUtilityW2BYTEMSK(REG_SCL0_62_L, 8, 0x07FF);
    }
}


void HalSclHvspSetCropConfig(DrvSclHvspCropIdType_e enID, HalSclHvspCropInfo_t stCropInfo)
{
    u32 u32reg_idx = enID == E_DRV_SCLHVSP_CROP_ID_1 ? 0x00 : 0x10;

    //Crop01: 0x50
    //Crop02: 0x58
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(E_DRV_SCLHVSP_ID_1)|| IsCannotUseCmdqToSetReg())
    {
        if((stCropInfo.u16In_hsize == stCropInfo.u16Hsize) && (stCropInfo.u16Vsize == stCropInfo.u16In_vsize))
        {
            HalUtilityW2BYTEMSK(REG_SCL2_50_L + u32reg_idx, 0, BIT0);
        }
        else
        {
            HalUtilityW2BYTEMSK(REG_SCL2_50_L + u32reg_idx, stCropInfo.bEn ? BIT0 : 0, BIT0);
        }
        HalUtilityW2BYTEMSK(REG_SCL2_51_L + u32reg_idx, stCropInfo.u16In_hsize, 0x0FFF);
        HalUtilityW2BYTEMSK(REG_SCL2_52_L + u32reg_idx, stCropInfo.u16In_vsize, 0x0FFF);
        HalUtilityW2BYTEMSK(REG_SCL2_53_L + u32reg_idx, stCropInfo.u16Hst, 0x0FFF);
        HalUtilityW2BYTEMSK(REG_SCL2_54_L + u32reg_idx, stCropInfo.u16Hsize, 0x0FFF);
        HalUtilityW2BYTEMSK(REG_SCL2_55_L + u32reg_idx, stCropInfo.u16Vst, 0x0FFF);
        HalUtilityW2BYTEMSK(REG_SCL2_56_L + u32reg_idx, stCropInfo.u16Vsize, 0x0FFF);
    }
    else
    {
        if((stCropInfo.u16In_hsize == stCropInfo.u16Hsize) && (stCropInfo.u16Vsize == stCropInfo.u16In_vsize))
        {
            _HalSclHvspWriteCmd(REG_SCL2_50_L + u32reg_idx, 0, BIT0);
        }
        else
        {
            _HalSclHvspWriteCmd(REG_SCL2_50_L + u32reg_idx, stCropInfo.bEn ? BIT0 : 0, BIT0);
        }
        _HalSclHvspWriteCmd(REG_SCL2_51_L + u32reg_idx, stCropInfo.u16In_hsize, 0x0FFF);
        _HalSclHvspWriteCmd(REG_SCL2_52_L + u32reg_idx, stCropInfo.u16In_vsize, 0x0FFF);
        _HalSclHvspWriteCmd(REG_SCL2_53_L + u32reg_idx, stCropInfo.u16Hst, 0x0FFF);
        _HalSclHvspWriteCmd(REG_SCL2_54_L + u32reg_idx, stCropInfo.u16Hsize, 0x0FFF);
        _HalSclHvspWriteCmd(REG_SCL2_55_L + u32reg_idx, stCropInfo.u16Vst, 0x0FFF);
        _HalSclHvspWriteCmd(REG_SCL2_56_L + u32reg_idx, stCropInfo.u16Vsize, 0x0FFF);
    }
}

//-------------------------------------------------------------------------------------------------
// IP Mux
//-------------------------------------------------------------------------------------------------
void HalSclHvspSetInputMuxType(DrvSclHvspIpMuxType_e enIpType)
{
    genIpType = enIpType;
}

u32 HalSclHvspGetInputSrcMux(void)
{
    return genIpType;
}

void HalSclHvspSetHwInputMux(DrvSclHvspIpMuxType_e enIpType)
{
    HalUtilityW2BYTEMSK(REG_SCL0_60_L, enIpType == E_DRV_SCLHVSP_IP_MUX_BT656 ? 0 : BIT0, BIT0);
}


//-------------------------------------------------------------------------------------------------
// IPM
//-------------------------------------------------------------------------------------------------
void HalSclHvspSetIpmYCMReadEn(bool bEn)
{
    if(!LOCK_FB())
    HalUtilityW2BYTEMSK(REG_SCL_DNR1_01_L, bEn ? BIT0 : 0, BIT0);
}

void HalSclHvspSetIpmYCMWriteEn(bool bEn)
{
    if(!LOCK_FB())
    HalUtilityW2BYTEMSK(REG_SCL_DNR1_01_L, bEn ? BIT1 : 0, BIT1);
}
void HalSclHvspSetIpmCiirReadEn(bool bEn)
{
    if(!LOCK_FB())
    HalUtilityW2BYTEMSK(REG_SCL_DNR1_01_L, bEn ? BIT2 : 0, BIT2);
}

void HalSclHvspSetIpmCiirWriteEn(bool bEn)
{
    if(!LOCK_FB())
    HalUtilityW2BYTEMSK(REG_SCL_DNR1_01_L, bEn ? BIT3 : 0, BIT3);
}

void HalSclHvspSetIpmYCBase(u32 u32Base)
{
    u16 u16Base_Lo, u16Base_Hi;
    u32Base = u32Base>>4;
    u16Base_Lo = u32Base & 0xFFFF;
    u16Base_Hi = (u32Base & 0x07FF0000) >> 16;
    HalUtilityW2BYTE(REG_SCL_DNR1_08_L, u16Base_Lo);
    HalUtilityW2BYTE(REG_SCL_DNR1_09_L, u16Base_Hi);
}
void HalSclHvspSetIpmMotionBase(u32 u32Base)
{
    u16 u16Base_Lo, u16Base_Hi;
    u32Base = u32Base>>4;
    u16Base_Lo = u32Base & 0xFFFF;
    u16Base_Hi = (u32Base & 0x07FF0000) >> 16;
    HalUtilityW2BYTE(REG_SCL_DNR1_10_L, u16Base_Lo);
    HalUtilityW2BYTE(REG_SCL_DNR1_11_L, u16Base_Hi);
}
void HalSclHvspSetIpmCiirBase(u32 u32Base)
{
    u16 u16Base_Lo, u16Base_Hi;
    u32Base = u32Base>>4;
    u16Base_Lo = u32Base & 0xFFFF;
    u16Base_Hi = (u32Base & 0x07FF0000) >> 16;
    HalUtilityW2BYTE(REG_SCL_DNR1_18_L, u16Base_Lo);
    HalUtilityW2BYTE(REG_SCL_DNR1_19_L, u16Base_Hi);
}

#if ENABLE_HVSP_UNUSED_FUNCTION
void HalSclHvspSetIpmReadReq(u8 u8Thrd, u8 u8Limit)
{
    HalUtilityW2BYTE(REG_SCL_DNR1_0A_L, ((((MS_U16)u8Limit)<<8) | ((MS_U16)u8Thrd)));
}

void HalSclHvspSetIpmWriteReq(u8 u8Thrd, u8 u8Limit)
{
    HalUtilityW2BYTE(REG_SCL_DNR1_0B_L, ((((MS_U16)u8Limit)<<8) | ((MS_U16)u8Thrd)));
}
#endif

void HalSclHvspSetIpmvSize(u16 u16Vsize)
{
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE))
    {
        HalUtilityW2BYTEMSK(REG_SCL_DNR1_0D_L, u16Vsize, 0x1FFF);
    }
    else
    {
        _HalSclHvspWriteCmd(REG_SCL_DNR1_0D_L, u16Vsize, 0x1FFF);
    }
    gu16height[0] = u16Vsize;
}

void HalSclHvspSetIpmLineOffset(u16 u16Lineoffset)
{
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE))
    {
        HalUtilityW2BYTEMSK(REG_SCL_DNR1_0E_L, u16Lineoffset, 0x1FFF);
    }
    else
    {
        _HalSclHvspWriteCmd(REG_SCL_DNR1_0E_L, u16Lineoffset, 0x1FFF);
    }
    gu16width[0] = u16Lineoffset;
}

void HalSclHvspSetIpmFetchNum(u16 u16FetchNum)
{
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE))
    {
        HalUtilityW2BYTEMSK(REG_SCL_DNR1_0F_L, u16FetchNum, 0x1FFF);
    }
    else
    {
        _HalSclHvspWriteCmd(REG_SCL_DNR1_0F_L, u16FetchNum, 0x1FFF);
    }
}
void HalSclHvspSetIpmBufferNumber(u8 u8Num)
{
    if(!LOCK_FB())
    HalUtilityW2BYTEMSK(REG_SCL_DNR1_02_L, u8Num , 0x7);
}

//-------------------------------------------------------------------------------------------------
// Scaling
//-------------------------------------------------------------------------------------------------

void HalSclHvspSetScalingVeEn(DrvSclHvspIdType_e enID, bool ben)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_0A_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_0A_L :
                                           REG_SCL_HVSP0_0A_L;

    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(enID)|| IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(u32reg, ben ? BIT8 : 0, BIT8);
    }
    else
    {
        //_HalSclHvspWriteCmd(u32reg, ben ? BIT8 : 0, BIT8);
    }
}
void HalSclHvspSetScalingVeFactor(DrvSclHvspIdType_e enID, u32 u32Ratio)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_09_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_09_L :
                                           REG_SCL_HVSP0_09_L;

    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(enID)|| IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(u32reg, u32Ratio & 0xFFFF, 0xFFFF);
        HalUtilityWBYTEMSK(u32reg+2, (u32Ratio >> 16) & 0x01FF, 0x01FF);
    }
    else
    {
        u16 u16val_L = u32Ratio & 0xFFFF;
        u16 u16val_H = (u32Ratio >> 16) & 0x00FF;
        if(u32Ratio)
        {
            u16val_H=(u16val_H|0x0100);
        }
        _HalSclHvspWriteCmd(u32reg,   u16val_L, 0xFFFF);
        _HalSclHvspWriteCmd(u32reg+2, u16val_H, 0x01FF);
    }
}

void HalSclHvspSetScalingHoEn(DrvSclHvspIdType_e enID, bool ben)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_08_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_08_L :
                                          REG_SCL_HVSP0_08_L;

    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(enID)|| IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(u32reg, ben ? BIT8 : 0, BIT8);
    }
    else
    {
        //_HalSclHvspWriteCmd(u32reg, ben ? BIT8 : 0, BIT8);
    }
}

void HalSclHvspSetScalingHoFacotr(DrvSclHvspIdType_e enID, u32 u32Ratio)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_07_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_07_L :
                                          REG_SCL_HVSP0_07_L;

    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(enID)|| IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(u32reg, u32Ratio & 0xFFFF, 0xFFFF);
        HalUtilityWBYTEMSK(u32reg+2, (u32Ratio >> 16) & 0x01FF, 0x01FF);
    }
    else
    {
        u16 u16val_L = u32Ratio & 0xFFFF;
        u16 u16val_H = (u32Ratio >> 16) & 0x00FF;
        if(u32Ratio)
        {
            u16val_H=(u16val_H|0x0100);
        }
        _HalSclHvspWriteCmd(u32reg,   u16val_L, 0xFFFF);
        _HalSclHvspWriteCmd(u32reg+2, u16val_H, 0x01FF);
    }
}
void HalSclHvspSetModeYHo(DrvSclHvspIdType_e enID, HalSclHvspFilterMode_e enFilterMode)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_0B_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_0B_L :
                                           REG_SCL_HVSP0_0B_L;
    genFilterMode=enFilterMode;
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(enID)|| IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(u32reg, enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_BYPASS ? 0 : BIT0, BIT0);
        HalUtilityW2BYTEMSK(u32reg, (enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_SRAM_0 || enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_SRAM_1) ? BIT6 : 0, BIT6);
        HalUtilityW2BYTEMSK(u32reg, enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_SRAM_1 ? BIT7 : 0, BIT7);
    }
    else
    {
        //_HalSclHvspWriteCmd(u32reg, bBypass ? 0 : BIT0, BIT0);
        if(enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_BYPASS)
        {
            _ReSetHVSPType(enID,BIT0|BIT6|BIT7);
        }
        else if(enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_BILINEAR)
        {
            _SetHVSPType(enID,BIT0);
            _ReSetHVSPType(enID,BIT6|BIT7);
        }
        else
        {
            _SetHVSPType(enID,BIT0|BIT6);
            if(enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_SRAM_1)
            {
                _SetHVSPType(enID,BIT7);
            }
            else
            {
                _ReSetHVSPType(enID,BIT7);
            }
        }
    }
}

void HalSclHvspSetModeYVe(DrvSclHvspIdType_e enID, HalSclHvspFilterMode_e enFilterMode)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_0B_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_0B_L :
                                           REG_SCL_HVSP0_0B_L;

    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(enID)|| IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(u32reg, enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_BYPASS ? 0 : BIT8, BIT8);
        HalUtilityW2BYTEMSK(u32reg, (enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_SRAM_0 || enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_SRAM_1) ? BIT14 : 0, BIT14);
        HalUtilityW2BYTEMSK(u32reg, enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_SRAM_1 ? BIT15 : 0, BIT15);
    }
    else
    {
        if(enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_BYPASS)
        {
            _ReSetHVSPType(enID,BIT15|BIT14|BIT8);
        }
        else if(enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_BILINEAR)
        {
            _SetHVSPType(enID,BIT8);
            _ReSetHVSPType(enID,BIT14|BIT15);
        }
        else
        {
            _SetHVSPType(enID,BIT8|BIT14);
            if(enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_SRAM_1)
            {
                _SetHVSPType(enID,BIT15);
            }
            else
            {
                _ReSetHVSPType(enID,BIT15);
            }
        }
        //_HalSclHvspWriteCmd(u32reg, bBypass ? 0 : BIT8, BIT8);
    }
}

void HalSclHvspSetModeCHo(DrvSclHvspIdType_e enID, HalSclHvspFilterMode_e enFilterMode, HalSclHvspSramSelType_e enSramSel)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_0B_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_0B_L :
                                           REG_SCL_HVSP0_0B_L;

    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(enID)|| IsCannotUseCmdqToSetReg())
    {
        if(enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_BYPASS || enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_BILINEAR)
        {
            HalUtilityW2BYTEMSK(u32reg, enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_BYPASS ? (0<<1) : (1<<1), (BIT3|BIT2|BIT1));
            HalUtilityW2BYTEMSK(u32reg, 0x0000, BIT4); // ram_en
            HalUtilityW2BYTEMSK(u32reg, enSramSel == E_HAL_SCLHVSP_SRAM_SEL_0 ? (0<<5) : (1<<5), BIT5); //ram_sel
        }
        else
        {
            HalUtilityW2BYTEMSK(u32reg, enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_SRAM_0 ? (2<<1) : (3<<1), (BIT3|BIT2|BIT1));
            HalUtilityW2BYTEMSK(u32reg, BIT4, BIT4); // ram_en
            HalUtilityW2BYTEMSK(u32reg, enSramSel == E_HAL_SCLHVSP_SRAM_SEL_0 ? (0<<5) : (1<<5), BIT5); //ram_sel
        }
    }
    else
    {
        if(enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_BYPASS)
        {
            _ReSetHVSPType(enID,BIT3|BIT2|BIT1|BIT4);
            if(enSramSel == E_HAL_SCLHVSP_SRAM_SEL_0)
            {
                _ReSetHVSPType(enID,BIT5);
            }
            else
            {
                _SetHVSPType(enID,BIT5);
            }
        }
        else if(enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_BILINEAR)
        {
            _SetHVSPType(enID,BIT1);
            _ReSetHVSPType(enID,BIT3|BIT2|BIT4);
            if(enSramSel == E_HAL_SCLHVSP_SRAM_SEL_0)
            {
                _ReSetHVSPType(enID,BIT5);
            }
            else
            {
                _SetHVSPType(enID,BIT5);
            }
        }
        else
        {
            _SetHVSPType(enID,BIT4|BIT2);
            _ReSetHVSPType(enID,BIT3);
            if(enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_SRAM_1)
            {
                _SetHVSPType(enID,BIT1);
            }
            else
            {
                _ReSetHVSPType(enID,BIT1);
            }
            if(enSramSel == E_HAL_SCLHVSP_SRAM_SEL_0)
            {
                _ReSetHVSPType(enID,BIT5);
            }
            else
            {
                _SetHVSPType(enID,BIT5);
            }
        }
    }
}

void HalSclHvspSetModeCVe(DrvSclHvspIdType_e enID, HalSclHvspFilterMode_e enFilterMode, HalSclHvspSramSelType_e enSramSel)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_0B_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_0B_L :
                                           REG_SCL_HVSP0_0B_L;
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(enID)|| IsCannotUseCmdqToSetReg())
    {
        if(enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_BYPASS || enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_BILINEAR)
        {
            HalUtilityW2BYTEMSK(u32reg, enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_BYPASS ? (0<<9) : (1<<9), (BIT11|BIT10|BIT9));
            HalUtilityW2BYTEMSK(u32reg, 0x0000, BIT12); // ram_en
            HalUtilityW2BYTEMSK(u32reg, enSramSel == E_HAL_SCLHVSP_SRAM_SEL_0 ? (0<<13) : (1<<13), BIT13); //ram_sel
        }
        else
        {
            HalUtilityW2BYTEMSK(u32reg, enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_SRAM_0 ? (2<<9) : (3<<9), (BIT11|BIT10|BIT9));
            HalUtilityW2BYTEMSK(u32reg, BIT12, BIT12); // ram_en
            HalUtilityW2BYTEMSK(u32reg, enSramSel == E_HAL_SCLHVSP_SRAM_SEL_0 ? (0<<13) : (1<<13), BIT13); //ram_sel
        }
    }
    else
    {
        if(enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_BYPASS)
        {
            _ReSetHVSPType(enID,BIT9|BIT10|BIT11|BIT12);
            if(enSramSel == E_HAL_SCLHVSP_SRAM_SEL_0)
            {
                _ReSetHVSPType(enID,BIT13);
            }
            else
            {
                _SetHVSPType(enID,BIT13);
            }
        }
        else if(enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_BILINEAR)
        {
            _SetHVSPType(enID,BIT9);
            _ReSetHVSPType(enID,BIT10|BIT11|BIT12);
            if(enSramSel == E_HAL_SCLHVSP_SRAM_SEL_0)
            {
                _ReSetHVSPType(enID,BIT13);
            }
            else
            {
                _SetHVSPType(enID,BIT13);
            }
        }
        else
        {
            _SetHVSPType(enID,BIT10|BIT12);
            _ReSetHVSPType(enID,BIT11);
            if(enFilterMode == E_HAL_SCLHVSP_FILTER_MODE_SRAM_1)
            {
                _SetHVSPType(enID,BIT9);
            }
            else
            {
                _ReSetHVSPType(enID,BIT9);
            }
            if(enSramSel == E_HAL_SCLHVSP_SRAM_SEL_0)
            {
                _ReSetHVSPType(enID,BIT13);
            }
            else
            {
                _SetHVSPType(enID,BIT13);
            }
        }
        _HalSclHvspWriteCmd(u32reg,gstSclFea[enID].u16ModeYCVH,0xFFFF);
    }

}

void HalSclHvspSetHspDithEn(DrvSclHvspIdType_e enID, bool bEn)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_0C_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_0C_L :
                                           REG_SCL_HVSP0_0C_L;

    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(enID)|| IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(u32reg, bEn ? (BIT0) : (0), BIT0);
    }
    else
    {
        //_HalSclHvspWriteCmd(u32reg, bEn ? (BIT0) : (0), BIT0);
    }
}


void HalSclHvspSetVspDithEn(DrvSclHvspIdType_e enID, bool bEn)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_0C_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_0C_L :
                                           REG_SCL_HVSP0_0C_L;

    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(enID)|| IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(u32reg, bEn ? (BIT1) : (0), BIT1);
    }
    else
    {
        //_HalSclHvspWriteCmd(u32reg, bEn ? (BIT1) : (0), BIT1);
    }
}

void HalSclHvspSetHspCoringEnC(DrvSclHvspIdType_e enID, bool bEn)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_0C_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_0C_L :
                                           REG_SCL_HVSP0_0C_L;

    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(enID)|| IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(u32reg, bEn ? (BIT8) : (0), BIT8);
    }
    else
    {
        //_HalSclHvspWriteCmd(u32reg, bEn ? (BIT8) : (0), BIT8);
    }
}

void HalSclHvspSetHspCoringEnY(DrvSclHvspIdType_e enID, bool bEn)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_0C_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_0C_L :
                                           REG_SCL_HVSP0_0C_L;

    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(enID)|| IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(u32reg, bEn ? (BIT9) : (0), BIT9);
    }
    else
    {
        //_HalSclHvspWriteCmd(u32reg, bEn ? (BIT11|BIT10|BIT9|BIT8|BIT1|BIT0) : (0), BIT11|BIT10|BIT9|BIT8|BIT1|BIT0);
    }
}

void HalSclHvspSetVspCoringEnC(DrvSclHvspIdType_e enID, bool bEn)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_0C_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_0C_L :
                                           REG_SCL_HVSP0_0C_L;

    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(enID)|| IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(u32reg, bEn ? (BIT10) : (0), BIT10);
    }
    else
    {
        //_HalSclHvspWriteCmd(u32reg, bEn ? (BIT10) : (0), BIT10);
    }
}

void HalSclHvspSetVspCoringEnY(DrvSclHvspIdType_e enID, bool bEn)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_0C_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_0C_L :
                                           REG_SCL_HVSP0_0C_L;

    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(enID)|| IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(u32reg, bEn ? (BIT11) : (0), BIT11);
    }
    else
    {
        _HalSclHvspWriteCmd(u32reg, bEn ? (BIT11|BIT10|BIT9|BIT8|BIT1|BIT0) : (0), BIT11|BIT10|BIT9|BIT8|BIT1|BIT0);
    }
}

void HalSclHvspSetHspCoringThrdC(DrvSclHvspIdType_e enID, u16 u16Thread)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_0D_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_0D_L :
                                           REG_SCL_HVSP0_0D_L;

    HalUtilityW2BYTEMSK(u32reg, u16Thread, 0x00FF);
}

void HalSclHvspSetHspCoringThrdY(DrvSclHvspIdType_e enID, u16 u16Thread)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_0D_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_0D_L :
                                           REG_SCL_HVSP0_0D_L;

    HalUtilityW2BYTEMSK(u32reg, u16Thread<<8, 0xFF00);
}

void HalSclHvspSetVspCoringThrdC(DrvSclHvspIdType_e enID, u16 u16Thread)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_0E_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_0E_L :
                                           REG_SCL_HVSP0_0E_L;

    HalUtilityW2BYTEMSK(u32reg, u16Thread, 0x00FF);
}

void HalSclHvspSetVspCoringThrdY(DrvSclHvspIdType_e enID, u16 u16Thread)
{
    u32 u32reg = enID == E_DRV_SCLHVSP_ID_2 ? REG_SCL_HVSP1_0E_L :
                    enID == E_DRV_SCLHVSP_ID_3 ? REG_SCL_HVSP2_0E_L :
                                           REG_SCL_HVSP0_0E_L;

    HalUtilityW2BYTEMSK(u32reg, u16Thread<<8, 0xFF00);
}
#if ENABLE_HVSP_UNUSED_FUNCTION

void Hal_HVSP_Set_Hvsp_Sram_Coeff(DrvSclHvspIdType_e enID, HalSclHvspFilterSramSelType_e enSramSel, bool bC_SRAM, u8 *pData)
{
    u32 u32reg_41, u32reg_42, u32reg_43;
    u8 u8Ramcode[10];
    u16 u16IdxBase = 0;
    u16 i, j, x;

    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        u32reg_41 = REG_SCL_HVSP1_41_L;
        u32reg_42 = REG_SCL_HVSP1_42_L;
        u32reg_43 = REG_SCL_HVSP1_43_L;
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        u32reg_41 = REG_SCL_HVSP2_41_L;
        u32reg_42 = REG_SCL_HVSP2_42_L;
        u32reg_43 = REG_SCL_HVSP2_43_L;
    }
    else
    {
        u32reg_41 = REG_SCL_HVSP0_41_L;
        u32reg_42 = REG_SCL_HVSP0_42_L;
        u32reg_43 = REG_SCL_HVSP0_43_L;
    }

    u16IdxBase = enSramSel == E_HALSCLHVSP_FILTER_SRAM_SEL_1 ? 0x00 :
                 enSramSel == E_HALSCLHVSP_FILTER_SRAM_SEL_2 ? 0x40 :
                 enSramSel == E_HALSCLHVSP_FILTER_SRAM_SEL_3 ? 0x80 :
                                                          0xC0 ;

    HalUtilityW2BYTEMSK(u32reg_41, bC_SRAM ? BIT1 : BIT0, BIT1|BIT0); // reg_cram_rw_en

    for(i=0; i<64; i++)
    {
        while(HalUtilityR2BYTEDirect(u32reg_41) & BIT8);
        j=i*5;

        HalUtilityW2BYTEMSK(u32reg_42,(i|u16IdxBase), 0x00FF);
        for ( x=0;x<5;x++ )
        {
            u8Ramcode[x] = pData[j+x];
        }

        HalUtilityW2BYTEMSK(u32reg_43+0x00, (((u16)u8Ramcode[1])<<8|(u16)u8Ramcode[0]), 0xFFFF);
        HalUtilityW2BYTEMSK(u32reg_43+0x02, (((u16)u8Ramcode[3])<<8|(u16)u8Ramcode[2]), 0xFFFF);
        HalUtilityW2BYTEMSK(u32reg_43+0x04, ((u16)u8Ramcode[4]), 0x00FF);


        HalUtilityW2BYTEMSK(u32reg_41, BIT8, BIT8);
    }
}
#endif

void HalSclHvspSetHVSPInputSize(DrvSclHvspIdType_e enID, u16 u16Width, u16 u16Height)
{
    u32 u32reg_20, u32reg_21;

    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        u32reg_20 = REG_SCL_HVSP1_20_L;
        u32reg_21 = REG_SCL_HVSP1_21_L;
        gu16height[0] = (gu16height[0]<u16Height) ? u16Height : gu16height[0];
        gu16width[0] = (gu16width[0]<u16Width) ? u16Width : gu16width[0];
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        u32reg_20 = REG_SCL_HVSP2_20_L;
        u32reg_21 = REG_SCL_HVSP2_21_L;
        gu16height[1] = u16Height;
        gu16width[1] = u16Width;
    }
    else
    {
        u32reg_20 = REG_SCL_HVSP0_20_L;
        u32reg_21 = REG_SCL_HVSP0_21_L;
        gu16height[0] =  (gu16height[0]<u16Height) ? u16Height : gu16height[0];
        gu16width[0] =  (gu16width[0]<u16Width) ? u16Width : gu16width[0];
    }

    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(enID)|| IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(u32reg_20, u16Width,  0x1FFF);
        HalUtilityW2BYTEMSK(u32reg_21, u16Height, 0x0FFF);
    }
    else
    {
        _HalSclHvspWriteCmd(u32reg_20, u16Width,  0x1FFF);
        _HalSclHvspWriteCmd(u32reg_21, u16Height, 0x0FFF);
    }
}

void HalSclHvspSetHVSPOutputSize(DrvSclHvspIdType_e enID, u16 u16Width, u16 u16Height)
{
    u32 u32reg_22, u32reg_23;
    u32 u32ratio;
    u16 u16val_L,u16val_H;
    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        u32reg_22 = REG_SCL_HVSP1_22_L;
        u32reg_23 = REG_SCL_HVSP1_23_L;
        gu16height[0] = (gu16height[0]<u16Height) ? u16Height : gu16height[0];;
        gu16width[0] = (gu16width[0]<u16Width) ? u16Width : gu16width[0];
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        u32reg_22 = REG_SCL_HVSP2_22_L;
        u32reg_23 = REG_SCL_HVSP2_23_L;
        gu16height[1] =(gu16height[1]<u16Height) ? u16Height : gu16height[1];
        gu16width[1] = (gu16width[1]<u16Width) ? u16Width : gu16width[1];
    }
    else
    {
        u32reg_22 = REG_SCL_HVSP0_22_L;
        u32reg_23 = REG_SCL_HVSP0_23_L;
        gu16height[0] = (gu16height[0]<u16Height) ? u16Height : gu16height[0];;
        gu16width[0] = (gu16width[0]<u16Width) ? u16Width : gu16width[0];
    }
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(enID)|| IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(u32reg_22, u16Width,  0x1FFF);
        HalUtilityW2BYTEMSK(u32reg_23, u16Height, 0x0FFF);
        gbhvspset[enID]= 1;
        if(enID ==E_DRV_SCLHVSP_ID_1)
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA0_1A_L, u16Width, 0xFFFF);
            HalUtilityW2BYTEMSK(REG_SCL_DMA0_1B_L, u16Height,0xFFFF);
            HalUtilityW2BYTEMSK(REG_SCL_DMA1_4A_L, u16Width, 0xFFFF);
            HalUtilityW2BYTEMSK(REG_SCL_DMA1_4B_L, u16Height,0xFFFF);
            HalUtilityW2BYTEMSK(REG_SCL_DMA0_4A_L, u16Width, 0xFFFF);
            HalUtilityW2BYTEMSK(REG_SCL_DMA0_4B_L, u16Height,0xFFFF);
        }
        else if(enID ==E_DRV_SCLHVSP_ID_2)
        {
            HalUtilityW2BYTEMSK(REG_SCL_DMA0_62_L, u16Width, 0xFFFF);
            HalUtilityW2BYTEMSK(REG_SCL_DMA0_63_L, u16Height,0xFFFF);
            HalUtilityW2BYTEMSK(REG_SCL_DMA1_62_L, u16Width, 0xFFFF);
            HalUtilityW2BYTEMSK(REG_SCL_DMA1_63_L, u16Height,0xFFFF);
        }
    }
    else
    {
        _HalSclHvspWriteCmd(u32reg_22, u16Width,  0x1FFF);
        _HalSclHvspWriteCmd(u32reg_23, u16Height, 0x0FFF);
        if(enID ==E_DRV_SCLHVSP_ID_1)
        {
            if(HalUtilityR2BYTE(REG_SCL_DMA0_1A_L) != u16Width)
                _HalSclHvspWriteCmd(REG_SCL_DMA0_1A_L, u16Width, 0xFFFF);
            if(HalUtilityR2BYTE(REG_SCL_DMA0_1B_L) != u16Height)
                _HalSclHvspWriteCmd(REG_SCL_DMA0_1B_L, u16Height,0xFFFF);
            if(HalUtilityR2BYTE(REG_SCL_DMA1_4A_L) != u16Width)
                _HalSclHvspWriteCmd(REG_SCL_DMA1_4A_L, u16Width, 0xFFFF);
            if(HalUtilityR2BYTE(REG_SCL_DMA1_4B_L) != u16Height)
                _HalSclHvspWriteCmd(REG_SCL_DMA1_4B_L, u16Height,0xFFFF);
            if(HalUtilityR2BYTE(REG_SCL_DMA0_4A_L) != u16Width)
                _HalSclHvspWriteCmd(REG_SCL_DMA0_4A_L, u16Width, 0xFFFF);
            if(HalUtilityR2BYTE(REG_SCL_DMA0_4B_L) != u16Height)
                _HalSclHvspWriteCmd(REG_SCL_DMA0_4B_L, u16Height,0xFFFF);
            if((HalUtilityR2BYTE(REG_SCL_HVSP1_20_L) != u16Width)&&(gbhvspset[E_DRV_SCLHVSP_ID_2]))
            {
                if(HalUtilityR2BYTE(REG_SCL_HVSP1_22_L))
                {
                    u32ratio = HAL_HVSP_RATIO(u16Width,HalUtilityR2BYTE(REG_SCL_HVSP1_22_L));
                    if(u32ratio)
                    {
                        u16val_L = u32ratio & 0xFFFF;
                        u16val_H = (u32ratio >> 16) & 0x00FF;
                        u16val_H = (u16val_H|0x0100);
                        _HalSclHvspWriteCmd(REG_SCL_HVSP1_07_L,   u16val_L, 0xFFFF);
                        _HalSclHvspWriteCmd(REG_SCL_HVSP1_07_L+2, u16val_H, 0x01FF);
                    }
                    _HalSclHvspWriteCmd(REG_SCL_HVSP1_20_L, u16Width,0xFFFF);
                }
            }
            if((HalUtilityR2BYTE(REG_SCL_HVSP1_21_L) != u16Height)&&(gbhvspset[E_DRV_SCLHVSP_ID_2]))
            {
                if(HalUtilityR2BYTE(REG_SCL_HVSP1_23_L))
                {
                    u32ratio = HAL_HVSP_RATIO(u16Height,HalUtilityR2BYTE(REG_SCL_HVSP1_23_L));
                    if(u32ratio)
                    {
                        u16val_L = u32ratio & 0xFFFF;
                        u16val_H = (u32ratio >> 16) & 0x00FF;
                        u16val_H = (u16val_H|0x0100);
                        _HalSclHvspWriteCmd(REG_SCL_HVSP1_09_L,   u16val_L, 0xFFFF);
                        _HalSclHvspWriteCmd(REG_SCL_HVSP1_09_L+2, u16val_H, 0x01FF);
                    }
                    _HalSclHvspWriteCmd(REG_SCL_HVSP1_21_L, u16Height,0xFFFF);
                }
            }
        }
        else if(enID ==E_DRV_SCLHVSP_ID_2)
        {
            if(HalUtilityR2BYTE(REG_SCL_DMA0_62_L) != u16Width)
                _HalSclHvspWriteCmd(REG_SCL_DMA0_62_L, u16Width, 0xFFFF);
            if(HalUtilityR2BYTE(REG_SCL_DMA0_63_L) != u16Height)
                _HalSclHvspWriteCmd(REG_SCL_DMA0_63_L, u16Height,0xFFFF);
            if(HalUtilityR2BYTE(REG_SCL_DMA1_62_L) != u16Width)
                _HalSclHvspWriteCmd(REG_SCL_DMA1_62_L, u16Width, 0xFFFF);
            if(HalUtilityR2BYTE(REG_SCL_DMA1_63_L) != u16Height)
                _HalSclHvspWriteCmd(REG_SCL_DMA1_63_L, u16Height,0xFFFF);
        }
    }
}

// input tgen
void HalSclHvspSetPatTgEn(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL0_40_L, bEn ? (BIT0|BIT1|BIT2|BIT15) : 0, BIT0|BIT1|BIT2|BIT15);
    HalUtilityW2BYTEMSK(REG_SCL0_70_L, bEn ? BIT0 : 0, BIT0);
    HalUtilityW2BYTEMSK(REG_SCL0_44_L, bEn ? BIT0 : 0, BIT0); // h/v
}

void HalSclHvspSetTestPatCfg(void)
{
    HalUtilityW2BYTEMSK(REG_SCL0_42_L, 0x1010, 0xFFFF); // cb h/v width
    HalUtilityW2BYTEMSK(REG_SCL0_43_L, 0x0008, 0xFFFF); // shfit time
    HalUtilityW2BYTEMSK(REG_SCL0_44_L, 0x0C20, 0xFFFF); // h/v
}


void HalSclHvspSetPatTgVsyncSt(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL0_71_L, u16Val, 0x0FFF);
}

void HalSclHvspSetPatTgVsyncEnd(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL0_72_L, u16Val, 0x0FFF);
}

void HalSclHvspSetPatTgVfdeSt(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL0_73_L, u16Val, 0x0FFF);
}

void HalSclHvspSetPatTgVfdeEnd(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL0_74_L, u16Val, 0x0FFF);
}

void HalSclHvspSetPatTgVdeSt(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL0_75_L, u16Val, 0x0FFF);
}

void HalSclHvspSetPatTgVdeEnd(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL0_76_L, u16Val, 0x0FFF);
}

void HalSclHvspSetPatTgVtt(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL0_77_L, u16Val, 0x0FFF);
}

void HalSclHvspSetPatTgHsyncSt(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL0_79_L, u16Val, 0x0FFF);
}

void HalSclHvspSetPatTgHsyncEnd(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL0_7A_L, u16Val, 0x0FFF);
}

void HalSclHvspSetPatTgHfdeSt(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL0_7B_L, u16Val, 0x0FFF);
}

void HalSclHvspSetPatTgHfdeEnd(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL0_7C_L, u16Val, 0x0FFF);
}

void HalSclHvspSetPatTgHdeSt(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL0_7D_L, u16Val, 0x0FFF);
}

void HalSclHvspSetPatTgHdeEnd(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL0_7E_L, u16Val, 0x0FFF);
}

void HalSclHvspSetPatTgHtt(u16 u16Val)
{
    HalUtilityW2BYTEMSK(REG_SCL0_7F_L, u16Val, 0x0FFF);
}


// LDC frame control
//#if ENABLE_HVSP_UNUSED_FUNCTION
void HalSclHvspSetLdcPathSel(bool bEn)
{
    if(!LOCK_FB())
    {
        if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(E_DRV_SCLHVSP_ID_1)|| IsCannotUseCmdqToSetReg())
        {
            HalUtilityW2BYTEMSK(REG_SCL0_03_L, bEn ? BIT12 : 0, BIT12);
            DrvSclOsSetSclFrameDelay(bEn ? 1 : 0);
        }
        else
        {
            _HalSclHvspWriteCmd(REG_SCL0_03_L, bEn ? BIT12 : 0, BIT12);
            DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_SETDELAY);
        }
        SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "LDC:%hhd\n", bEn);
    }
}

bool HalSclHvspGetLdcPathSel(void)
{
    u16 bEn;
    bEn = HalUtilityR2BYTE(REG_SCL0_03_L);
    return (bool)((bEn&BIT12)>>12);
}
u16 HalSclHvspGetShiftMultipleByIspClk(HalSclHvspIspClkType_e enISPClkType)
{
    u16 u16DspH;
    switch(enISPClkType)
    {
        case E_HAL_SCLHVSP_ISPCLK_123M:
            u16DspH = 8;//40%
            break;
        case E_HAL_SCLHVSP_ISPCLK_86M:
            u16DspH = 6;//30%
            break;
        case E_HAL_SCLHVSP_ISPCLK_72M:
            u16DspH = 5; //25 %
            break;
        case E_HAL_SCLHVSP_ISPCLK_54M:
            u16DspH = 4; //20%
            break;
        case E_HAL_SCLHVSP_ISPCLK_144M:
            u16DspH = 9; //45%
            break;
        case E_HAL_SCLHVSP_ISPCLK_OFF:
        default:
           u16DspH =  0;
                break;
    }
    return u16DspH;
}

u16 HalSclHvspGetLimitationByIspClk(HalSclHvspIspClkType_e enISPClkType, u16 u16SrcWidth, u16 u16DspHeight)
{
    u16 u16CropHeightLim;
    if (u16SrcWidth<=1920)//for LDC issue
    {
        switch(enISPClkType)
        {
            case E_HAL_SCLHVSP_ISPCLK_123M:
                if(gstHalSrcSize.u16Height*gstHalSrcSize.u16Width >= 4500000 &&gstHalSrcSize.bSet)
                {
                    u16CropHeightLim = (u16DspHeight/50)*46;//92% 1.1X
                }
                else
                {
                    u16CropHeightLim = (u16DspHeight/20)*15;//75% 1.33X
                }
                break;
            case E_HAL_SCLHVSP_ISPCLK_86M:
                u16CropHeightLim = (u16DspHeight/20)*9;//45%
                break;
            case E_HAL_SCLHVSP_ISPCLK_72M:
                u16CropHeightLim = (u16DspHeight/3); //33 %
                break;
            case E_HAL_SCLHVSP_ISPCLK_54M:
                u16CropHeightLim = u16DspHeight/3; //33%
                break;
            case E_HAL_SCLHVSP_ISPCLK_144M:
                if(gstHalSrcSize.u16Height*gstHalSrcSize.u16Width >= 4500000 &&gstHalSrcSize.bSet)
                {
                    u16CropHeightLim = (u16DspHeight/50)*46;//92% 1.1X
                }
                else
                {
                    u16CropHeightLim = (u16DspHeight/20)*15;//75% 1.33X
                }
                break;
            case E_HAL_SCLHVSP_ISPCLK_OFF:
            default:
               u16CropHeightLim =  u16DspHeight;
                    break;
        }
    }
    else
    {
        if(gstHalSrcSize.u16Height*gstHalSrcSize.u16Width >= 4500000 &&gstHalSrcSize.bSet)
        {
            u16CropHeightLim = (u16DspHeight/50)*46;//92% 1.1X
        }
        else
        {
            u16CropHeightLim = (u16DspHeight/20)*15;//75% 1.33X
        }
    }
    return u16CropHeightLim;
}

HalSclHvspIspClkType_e HalSclHvspGetIspClkType(void)
{
    u16 u16clk;
    HalSclHvspIspClkType_e enISPClkType;
    u16clk = (HalUtilityR2BYTEDirect(REG_SCL_CLK_61_L)&0x1F00)>>8;
    switch(u16clk)
    {
        case E_HAL_SCLHVSP_ISPCLK_123M:
            enISPClkType = E_HAL_SCLHVSP_ISPCLK_123M;
            break;
        case E_HAL_SCLHVSP_ISPCLK_OFF:
            enISPClkType = E_HAL_SCLHVSP_ISPCLK_OFF;
            break;
        case E_HAL_SCLHVSP_ISPCLK_86M:
            enISPClkType = E_HAL_SCLHVSP_ISPCLK_86M;
            break;
        case E_HAL_SCLHVSP_ISPCLK_72M:
            enISPClkType = E_HAL_SCLHVSP_ISPCLK_72M;
            break;
        case E_HAL_SCLHVSP_ISPCLK_54M:
            enISPClkType = E_HAL_SCLHVSP_ISPCLK_54M;
            break;
        case E_HAL_SCLHVSP_ISPCLK_144M:
            enISPClkType = E_HAL_SCLHVSP_ISPCLK_144M;
            break;
        default:
           enISPClkType =  E_HAL_SCLHVSP_ISPCLK_NUM;
                break;
    }
    return enISPClkType;
}

void HalSclHvspSetLdcBypass(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL_LDC_0F_L, bEn ? BIT0 : 0, BIT0);
}
//#endif

void HalSclHvspSetLdcWidth(u16 u16Width)
{
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(E_DRV_SCLHVSP_ID_1)|| IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(REG_SCL_LDC_03_L, u16Width, 0x0FFF);
    }
    else
    {
        _HalSclHvspWriteCmd(REG_SCL_LDC_03_L, u16Width, 0x0FFF);
    }
}

void HalSclHvspSetLdcHeight(u16 u16Height)
{
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(E_DRV_SCLHVSP_ID_1)|| IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(REG_SCL_LDC_04_L, u16Height, 0x0FFF);
    }
    else
    {
        _HalSclHvspWriteCmd(REG_SCL_LDC_04_L, u16Height, 0x0FFF);
    }
}

void HalSclHvspSetLdcBase(u32 u32Base)
{
    HalUtilityW4BYTE(REG_SCL_LDC_05_L, u32Base);
}

void HalSclHvspSetLdcHWrwDiff(u16 u8Val)
{
    HalUtilityW2BYTEMSK(REG_SCL_LDC_11_L, ((u16)u8Val), BIT1|BIT0);
}
void HalSclHvspSetLdcSwIdx(u8 idx)
{
    HalUtilityW2BYTEMSK(REG_SCL_LDC_10_L, ((u16)idx), BIT1|BIT0);
}
void HalSclHvspSetLdcSwMode(u8 bEn)
{
    HalUtilityW2BYTEMSK(REG_SCL_LDC_12_L, ((u16)bEn)? BIT0 :0, BIT0);
}
void HalSclHvspSetLdcBankMode(DrvSclHvspLdcLcBankModeType_e enType)
{
    HalUtilityW2BYTEMSK(REG_SCL_LDC_09_L, ((u16)enType == E_DRV_SCLHVSP_LDCLCBANKMODE_64)? BIT0 :0, BIT0);
}

#if ENABLE_HVSP_UNUSED_FUNCTION
void HalSclHvspSetLdcPitch(u32 u32Pitch)
{
    HalUtilityW4BYTE(REG_SCL_LDC_07_L, u32Pitch);
}


void Hal_HVSP_SetLDCDmap_Base(u32 u32Base)
{
    HalUtilityW4BYTE(REG_SCL_LDC_0A_L, u32Base);
}

void Hal_HVSP_SetLDCDmap_Pitch(u32 u32Pitch)
{
    HalUtilityW4BYTE(REG_SCL_LDC_0C_L, u32Pitch);
}
#endif
void HalSclHvspSetOsdLocate(DrvSclHvspIdType_e enID,DrvSclHvspOsdLocType_e enLoc)
{
    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        HalUtilityW2BYTEMSK(REG_SCL2_64_L, (enLoc==E_DRV_SCLHVSP_OSD_LOC_BEFORE) ? BIT1: 0, BIT1);
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        HalUtilityW2BYTEMSK(REG_SCL2_68_L, (enLoc==E_DRV_SCLHVSP_OSD_LOC_BEFORE) ? BIT1: 0, BIT1);
    }
    else
    {
        HalUtilityW2BYTEMSK(REG_SCL2_60_L, (enLoc==E_DRV_SCLHVSP_OSD_LOC_BEFORE) ? BIT1: 0, BIT1);
    }
}
void HalSclHvspSetOsdOnOff(DrvSclHvspIdType_e enID,bool bEn)
{
    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        HalUtilityW2BYTEMSK(REG_SCL2_64_L, bEn? BIT0: 0, BIT0);
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        HalUtilityW2BYTEMSK(REG_SCL2_68_L, bEn? BIT0: 0, BIT0);
    }
    else
    {
        HalUtilityW2BYTEMSK(REG_SCL2_60_L, bEn? BIT0: 0, BIT0);
    }
}
void HalSclHvspSetOsdBypass(DrvSclHvspIdType_e enID,bool bEn)
{
    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        HalUtilityW2BYTEMSK(REG_SCL2_67_L, bEn? BIT7: 0, BIT7);
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        HalUtilityW2BYTEMSK(REG_SCL2_6B_L, bEn? BIT7: 0, BIT7);
    }
    else
    {
        HalUtilityW2BYTEMSK(REG_SCL2_63_L, bEn? BIT7: 0, BIT7);
    }
}
void HalSclHvspSetOsdBypassWTM(DrvSclHvspIdType_e enID,bool bEn)
{
    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        HalUtilityW2BYTEMSK(REG_SCL2_64_L, bEn? BIT7: 0, BIT7);
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        HalUtilityW2BYTEMSK(REG_SCL2_68_L, bEn? BIT7: 0, BIT7);
    }
    else
    {
        HalUtilityW2BYTEMSK(REG_SCL2_60_L, bEn? BIT7: 0, BIT7);
    }
}
DrvSclHvspOsdLocType_e HalSclHvspGetOsdLocate(DrvSclHvspIdType_e enID)
{
    //for GOP may set same reg, we need to get direct.
    DrvSclHvspOsdLocType_e bEn = 0;
    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        bEn = (HalUtilityR2BYTEDirect(REG_SCL2_64_L)& BIT1)>>1;
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        bEn = (HalUtilityR2BYTEDirect(REG_SCL2_68_L)& BIT1)>>1;
    }
    else
    {
        bEn = (HalUtilityR2BYTEDirect(REG_SCL2_60_L)& BIT1)>>1;
    }
    return bEn;
}
bool HalSclHvspGetOsdOnOff(DrvSclHvspIdType_e enID)
{
    //for GOP may set same reg, we need to get direct.
    bool bEn = 0;
    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        bEn = HalUtilityR2BYTEDirect(REG_SCL2_64_L)& BIT0;
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        bEn = HalUtilityR2BYTEDirect(REG_SCL2_68_L)& BIT0;
    }
    else
    {
        bEn = HalUtilityR2BYTEDirect(REG_SCL2_60_L)& BIT0;
    }
    return bEn;
}
bool HalSclHvspGetOsdBypass(DrvSclHvspIdType_e enID)
{
    //for GOP may set same reg, we need to get direct.
    bool bEn;
    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        bEn = (HalUtilityR2BYTEDirect(REG_SCL2_67_L)& BIT7)>>7;
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        bEn = (HalUtilityR2BYTEDirect(REG_SCL2_6B_L)& BIT7)>>7;
    }
    else
    {
        bEn = (HalUtilityR2BYTEDirect(REG_SCL2_63_L)& BIT7)>>7;
    }
    return bEn;
}
bool HalSclHvspGetOsdBypassWTM(DrvSclHvspIdType_e enID)
{
    //for GOP may set same reg, we need to get direct.
    bool bEn;
    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        bEn = (HalUtilityR2BYTEDirect(REG_SCL2_64_L)& BIT7)>>7;
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        bEn = (HalUtilityR2BYTEDirect(REG_SCL2_68_L)& BIT7)>>7;
    }
    else
    {
        bEn = (HalUtilityR2BYTEDirect(REG_SCL2_60_L)& BIT7)>>7;
    }
    return bEn;
}

void HalSclHvspSetReg(u32 u32Reg, u8 u8Val, u8 u8Mask)
{
    HalUtilityWBYTEMSK(u32Reg, u8Val, u8Mask);
}
void HalSclHvspSet2ByteReg(u32 u32Reg, u16 u16Val, u16 u16Mask)
{
    HalUtilityW2BYTEMSK(u32Reg, u16Val, u16Mask);
}
u16 HalSclHvspGet2ByteReg(u32 u32Reg)
{
    u16 u16Val;
    u16Val = HalUtilityR2BYTEDirect(u32Reg);
    return u16Val;
}

u16 HalSclHvspGetCropX(void)
{
    u16 u16crop1;
    u16crop1 = HalUtilityR2BYTE(REG_SCL2_53_L);
    return u16crop1;
}

u16 HalSclHvspGetCropY(void)
{
    u16 u16crop1;
    u16crop1 = HalUtilityR2BYTE(REG_SCL2_55_L);
    return u16crop1;
}
u16 HalSclHvspGetCrop2Xinfo(void)
{
    u16 u16crop2;
    u16crop2 = HalUtilityR2BYTE(REG_SCL2_5B_L);
    return u16crop2;
}

u16 HalSclHvspGetCrop2Yinfo(void)
{
    u16 u16crop2;
    u16crop2 = HalUtilityR2BYTE(REG_SCL2_5D_L);
    return u16crop2;
}

u16 HalSclHvspGetCrop1WidthCount(void)
{
    u16 u16crop1;
    u16crop1 = HalUtilityR2BYTEDirect(REG_SCL2_70_L);
    return u16crop1;
}
u16 HalSclHvspGetCrop1HeightCount(void)
{
    u16 u16crop1;
    u16crop1 = HalUtilityR2BYTEDirect(REG_SCL2_71_L);
    return u16crop1;
}
u16 HalSclHvspGetCrop2CountH(void)
{
    u16 u16crop1;
    u16crop1 = HalUtilityR2BYTEDirect(REG_SCL2_72_L);
    return u16crop1;
}
u16 HalSclHvspGetCrop2CountV(void)
{
    u16 u16crop1;
    u16crop1 = HalUtilityR2BYTEDirect(REG_SCL2_73_L);
    return u16crop1;
}
u16 HalSclHvspGetCrop1Width(void)
{
    u16 u16crop1;
    u16crop1 = HalUtilityR2BYTE(REG_SCL2_51_L);
    return u16crop1;
}
u16 HalSclHvspGetCrop1Height(void)
{
    u16 u16crop1;
    u16crop1 = HalUtilityR2BYTE(REG_SCL2_52_L);
    return u16crop1;
}
u16 HalSclHvspGetDmaHeight(DrvSclHvspMonitorType_e enMonitorType)
{
    u16 u16def;

    switch(enMonitorType)
    {
        case E_DRV_SCLHVSP_MONITOR_DMA1FRMCHECK:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA0_1B_L);
            break;

        case E_DRV_SCLHVSP_MONITOR_DMA1SNPCHECK:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA0_4B_L);
            break;

        case E_DRV_SCLHVSP_MONITOR_DMA2FRMCHECK:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA0_63_L);
            break;
        case E_DRV_SCLHVSP_MONITOR_DMA3FRMCHECK:
            u16def = HalUtilityR2BYTE(REG_SCL_DMA1_1B_L);
            break;
        default:
            u16def = 0x0;
            break;
    }
    return ((u16def ));
}
u16 HalSclHvspGetDmaHeightCount(DrvSclHvspMonitorType_e enMonitorType)
{
    u16 u16Idx;

    switch(enMonitorType)
    {
        case E_DRV_SCLHVSP_MONITOR_DMA1FRMCHECK:
            u16Idx = HalUtilityR2BYTEDirect(REG_SCL_DMA2_10_L);
            break;

        case E_DRV_SCLHVSP_MONITOR_DMA1SNPCHECK:
            u16Idx = HalUtilityR2BYTEDirect(REG_SCL_DMA2_12_L);
            break;

        case E_DRV_SCLHVSP_MONITOR_DMA2FRMCHECK:
            u16Idx = HalUtilityR2BYTEDirect(REG_SCL_DMA2_13_L);
            break;
        case E_DRV_SCLHVSP_MONITOR_DMA3FRMCHECK:
            u16Idx = HalUtilityR2BYTEDirect(REG_SCL_DMA2_15_L);
            break;
        default:
            u16Idx = 0x0;
            break;
    }
    return ((u16Idx));
}

u16 HalSclHvspGetDmaEn(DrvSclHvspMonitorType_e enMonitorType)
{
    u16 bEn;
    switch(enMonitorType)
    {
        case E_DRV_SCLHVSP_MONITOR_DMA1FRMCHECK:
            bEn = HalUtilityR2BYTE(REG_SCL_DMA0_01_L);
            bEn = (bEn & 0x8000)>>15;
            break;

        case E_DRV_SCLHVSP_MONITOR_DMA1SNPCHECK:
            bEn = HalUtilityR2BYTE(REG_SCL_DMA0_03_L);
            bEn = (bEn & 0x8000)>>15;
            break;

        case E_DRV_SCLHVSP_MONITOR_DMA2FRMCHECK:
            bEn = HalUtilityR2BYTE(REG_SCL_DMA0_04_L);
            bEn = (bEn & 0x8000)>>15;
            break;
        case E_DRV_SCLHVSP_MONITOR_DMA3FRMCHECK:
            bEn = HalUtilityR2BYTE(REG_SCL_DMA1_01_L);
            bEn = (bEn & 0x8000)>>15;
            break;
        default:
            bEn = 0x0;
            break;
    }
    return bEn;
}
u16 HalSclHvspGetHvspInputWidth(DrvSclHvspIdType_e enID)
{
    u16 u16size;
    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        u16size = HalUtilityR2BYTE(REG_SCL_HVSP1_20_L);
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        u16size = HalUtilityR2BYTE(REG_SCL_HVSP2_20_L);
    }
    else
    {
        u16size = HalUtilityR2BYTE(REG_SCL_HVSP0_20_L);
    }
    return u16size;
}

u16 HalSclHvspGetHvspOutputWidth(DrvSclHvspIdType_e enID)
{
    u16 u16size;
    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        u16size = HalUtilityR2BYTE(REG_SCL_HVSP1_22_L);
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        u16size = HalUtilityR2BYTE(REG_SCL_HVSP2_22_L);
    }
    else
    {
        u16size = HalUtilityR2BYTE(REG_SCL_HVSP0_22_L);
    }
    return u16size;
}

u16 HalSclHvspGetCrop2InputWidth(void)
{
    u16 u16size;
    u16size = HalUtilityR2BYTE(REG_SCL2_59_L);
    return u16size;
}

u16 HalSclHvspGetCrop2InputHeight(void)
{
    u16 u16size;
    u16size = HalUtilityR2BYTE(REG_SCL2_5A_L);
    return u16size;
}
u16 HalSclHvspGetCrop2OutputWidth(void)
{
    u16 u16size;
    u16size = HalUtilityR2BYTE(REG_SCL2_5C_L);
    return u16size;
}

u16 HalSclHvspGetCrop2OutputHeight(void)
{
    u16 u16size;
    u16size = HalUtilityR2BYTE(REG_SCL2_5E_L);
    return u16size;
}
u16 HalSclHvspGetCrop1En(void)
{
    u16 u16size;
    u16size = (HalUtilityR2BYTE(REG_SCL2_50_L) & BIT0);
    return u16size;
}
u16 HalSclHvspGetCrop2En(void)
{
    u16 u16size;
    u16size = (HalUtilityR2BYTE(REG_SCL2_58_L) & BIT0);
    return u16size;
}
u16 HalSclHvspGetHvspOutputHeight(DrvSclHvspIdType_e enID)
{
    u16 u16size;
    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        u16size = HalUtilityR2BYTE(REG_SCL_HVSP1_23_L);
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        u16size = HalUtilityR2BYTE(REG_SCL_HVSP2_23_L);
    }
    else
    {
        u16size = HalUtilityR2BYTE(REG_SCL_HVSP0_23_L);
    }
    return u16size;
}
u16 HalSclHvspGetHvspInputHeight(DrvSclHvspIdType_e enID)
{
    u16 u16size;
    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        u16size = HalUtilityR2BYTE(REG_SCL_HVSP1_21_L);
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        u16size = HalUtilityR2BYTE(REG_SCL_HVSP2_21_L);
    }
    else
    {
        u16size = HalUtilityR2BYTE(REG_SCL_HVSP0_21_L);
    }
    return u16size;
}
u16 HalSclHvspGetScalingFunctionStatus(DrvSclHvspIdType_e enID)
{
    u16 u16size;
    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        u16size = (HalUtilityR2BYTE(REG_SCL_HVSP1_08_L) & BIT8)>>8;
        u16size |= (HalUtilityR2BYTE(REG_SCL_HVSP1_0A_L)& BIT8)>>7;
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        u16size = (HalUtilityR2BYTE(REG_SCL_HVSP2_08_L) & BIT8)>>8;
        u16size |= (HalUtilityR2BYTE(REG_SCL_HVSP2_0A_L)& BIT8)>>7;
    }
    else
    {
        u16size = (HalUtilityR2BYTE(REG_SCL_HVSP0_08_L) & BIT8)>>8;
        u16size |= (HalUtilityR2BYTE(REG_SCL_HVSP0_0A_L)& BIT8)>>7;
    }
    return u16size;
}
u16 HalSclHvspGetHvspOutputHeightCount(DrvSclHvspIdType_e enID)
{
    u16 u16size;
    if(enID == E_DRV_SCLHVSP_ID_2)
    {
        u16size = HalUtilityR2BYTEDirect(REG_SCL_HVSP1_25_L);
    }
    else if(enID == E_DRV_SCLHVSP_ID_3)
    {
        u16size = HalUtilityR2BYTEDirect(REG_SCL_HVSP2_26_L);
    }
    else
    {
        u16size = HalUtilityR2BYTEDirect(REG_SCL_HVSP0_26_L);
    }
    return u16size;
}
u32 HalSclHvspGetCmdqStatus(void)
{
    return HalUtilityR2BYTEDirect(0x11208A);
}
void HalSclHvspPriMaskColorY(u16 u16Y)
{
    HalUtilityW2BYTEMSK(REG_VIP_SCNR_12_L, u16Y, 0xFFFF);
}
void HalSclHvspPriMaskColorU(u16 u16U)
{
    HalUtilityW2BYTEMSK(REG_VIP_SCNR_13_L, u16U, 0xFFFF);
}
void HalSclHvspPriMaskColorV(u16 u16V)
{
    HalUtilityW2BYTEMSK(REG_VIP_SCNR_14_L, u16V, 0xFFFF);
}

void HalSclHvspSetPrv2CropOnOff(bool bEn)
{
    if(!LOCK_FB())
    {
        if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(E_DRV_SCLHVSP_ID_1)|| IsCannotUseCmdqToSetReg())
        {
            HalUtilityW2BYTEMSK(REG_SCL_DNR1_01_L, bEn ? BIT5 : 0, BIT5);
        }
        else
        {
            _HalSclHvspWriteCmd(REG_SCL_DNR1_01_L, bEn ? BIT5 : 0, BIT5);
        }
        DrvSclOsSetSclFrameDelay(bEn ? 1 :0);
        if(bEn)
        {
            if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(E_DRV_SCLHVSP_ID_1)|| IsCannotUseCmdqToSetReg())
            {
                HalUtilityW2BYTEMSK(REG_VIP_MCNR_01_L, bEn ? BIT0 : 0, BIT0);
                if(!(HalUtilityR2BYTE(REG_SCL_DNR1_01_L)&BIT0) && bEn)
                {
                    HalUtilityW2BYTEMSK(REG_SCL_DNR1_01_L, BIT0 , BIT0);
                }
            }
            else
            {
                _HalSclHvspWriteCmd(REG_VIP_MCNR_01_L, bEn ? BIT0 : 0, BIT0);
                if(!(HalUtilityR2BYTE(REG_SCL_DNR1_01_L)&BIT0) && bEn)
                {
                    _HalSclHvspWriteCmd(REG_SCL_DNR1_01_L,BIT0, BIT0);
                }
            }
        }
    }

}
bool HalSclHvspGetPrv2CropOnOff(void)
{
    u16 u16Reg;
    u16Reg = HalUtilityR2BYTE(REG_SCL_DNR1_01_L);
    return ((u16Reg &BIT5)>>5);
}

void HalSclHvspPriMaskbEn(bool bEn)
{
    u32 u32Events;
    DrvSclCmdqCmdReg_t stCfg;
    if(IsRegType(E_DRV_SCLHVSP_CMD_TRIG_NONE) || IsIPFirstSet(E_DRV_SCLHVSP_ID_1) || IsCannotUseCmdqToSetReg())
    {
        HalUtilityW2BYTEMSK(REG_VIP_SCNR_10_L, bEn ? BIT0 : 0, BIT0);
    }
    else if(bEn == E_HAL_SCLHVSP_TRIGGER_OFF)
    {
        if(DrvSclIrqGetEachDMAEn()&& VIPSETRULE())
        {
            DrvSclCmdqGetModuleMutex(E_DRV_SCLCMDQ_TYPE_IP0,1);
            DrvSclCmdqFillCmd(&stCfg, REG_VIP_SCNR_10_L, 0, BIT0);
            DrvSclCmdqWriteCmd(E_DRV_SCLCMDQ_TYPE_IP0,stCfg,1);
            DrvSclCmdqFire(E_DRV_SCLCMDQ_TYPE_IP0,TRUE);
            DrvSclCmdqGetModuleMutex(E_DRV_SCLCMDQ_TYPE_IP0,0);
        }
        else
        {
            if(DrvSclIrqGetEachDMAEn()||(!DrvSclIrqGetIsBlankingRegion()))
            {
                DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(),
                    (E_DRV_SCLIRQ_EVENT_FRMENDSYNC), &u32Events, E_DRV_SCLOS_EVENT_MD_AND, 200); // get status: FRM END
            }
            HalUtilityW2BYTEMSK(REG_VIP_SCNR_10_L,0, BIT0);
        }
    }
    else if(bEn == E_HAL_SCLHVSP_TRIGGER_ON)
    {
        if(DrvSclIrqGetEachDMAEn()&& VIPSETRULE())
        {
            DrvSclCmdqGetModuleMutex(E_DRV_SCLCMDQ_TYPE_IP0,1);
            DrvSclCmdqFillCmd(&stCfg, REG_VIP_SCNR_10_L, 0, BIT0);
            DrvSclCmdqWriteCmd(E_DRV_SCLCMDQ_TYPE_IP0,stCfg,1);
            DrvSclCmdqFire(E_DRV_SCLCMDQ_TYPE_IP0,TRUE);
            DrvSclCmdqGetModuleMutex(E_DRV_SCLCMDQ_TYPE_IP0,0);
        }
        else
        {
            if(DrvSclIrqGetEachDMAEn()||(!DrvSclIrqGetIsBlankingRegion()))
            {
                DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(),
                    (E_DRV_SCLIRQ_EVENT_FRMENDSYNC), &u32Events, E_DRV_SCLOS_EVENT_MD_AND, 200); // get status: FRM END
            }
            HalUtilityW2BYTEMSK(REG_VIP_SCNR_10_L,BIT0, BIT0);
        }
    }
    else if(bEn == E_HAL_SCLHVSP_TRIGGER_CMDQ)
    {
        _HalSclHvspWriteCmd(REG_VIP_SCNR_10_L,bEn ? BIT0 : 0, BIT0);
    }
}
void HalSclHvspPriMaskSramEn(bool bEn,u8 u8Idx)
{
    u32 u32Events;
    DrvSclCmdqCmdReg_t stCfg;
    if(DrvSclIrqGetEachDMAEn()&& VIPSETRULE())
    {
        if(bEn)
        {
            DrvSclCmdqFillCmd(&stCfg, REG_VIP_SCNR_11_L, ((u16)(u8Idx<<8)| 0x1 ), 0xFF01);
            DrvSclCmdqWriteCmd(E_DRV_SCLCMDQ_TYPE_IP0,stCfg,1);
        }
        else
        {
            DrvSclCmdqFillCmd(&stCfg, REG_VIP_SCNR_11_L, ((u16)(u8Idx<<8)), 0xFF01);
            DrvSclCmdqWriteCmd(E_DRV_SCLCMDQ_TYPE_IP0,stCfg,1);
        }
    }
    else
    {
        if(DrvSclIrqGetEachDMAEn()||(!DrvSclIrqGetIsBlankingRegion()))
        {
            DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(),
                (E_DRV_SCLIRQ_EVENT_FRMENDSYNC), &u32Events, E_DRV_SCLOS_EVENT_MD_AND, 200); // get status: FRM END
        }
        HalUtilityW2BYTEMSK(REG_VIP_SCNR_11_L, ((u16)(u8Idx<<8)), 0xFF00);
        HalUtilityW2BYTEMSK(REG_VIP_SCNR_11_L, bEn ? BIT0 : 0, BIT0);
    }
}
void HalSclHvspPriMaskFire(void)
{
    if(VIPSETRULE())
    {
        DrvSclCmdqFire(E_DRV_SCLCMDQ_TYPE_IP0,TRUE);
    }
}

void HalSclHvspPriMaskSramOffsetRe(bool bEn)
{
    HalUtilityW2BYTEMSK(REG_VIP_SCNR_11_L, bEn ? BIT1 : 0, BIT1);
}
void HalSclHvspPriMaskSramRdIdx(u8 u8Idx)
{
    HalUtilityW2BYTEMSK(REG_VIP_SCNR_11_L, u8Idx<<5, BIT5|BIT6|BIT7);
}
void HalSclHvspPriMaskSramWriteIdx(u8 u8Idx)
{
    HalUtilityW2BYTEMSK(REG_VIP_SCNR_11_L, (u16)(u8Idx<<8), 0xFF00);
}
void HalSclHvspPriMaskSetSram(u8 u8idx,u16 u16val)
{
    u32 u32Events;
    DrvSclCmdqCmdReg_t stCfg;
    u32 u32reg = (u8idx == 0) ? REG_VIP_SCNR_17_L :
                    (u8idx == 1) ? REG_VIP_SCNR_18_L :
                    (u8idx == 2) ? REG_VIP_SCNR_19_L :
                    (u8idx == 3) ? REG_VIP_SCNR_1A_L :
                    (u8idx == 4) ? REG_VIP_SCNR_1B_L :
                    (u8idx == 5) ? REG_VIP_SCNR_1C_L :
                                   REG_VIP_SCNR_17_L;
    if(DrvSclIrqGetEachDMAEn()&& VIPSETRULE())
    {
        DrvSclCmdqFillCmd(&stCfg, u32reg, u16val, 0xFFFF);
        DrvSclCmdqWriteCmd(E_DRV_SCLCMDQ_TYPE_IP0,stCfg,1);
    }
    else
    {
        if(DrvSclIrqGetEachDMAEn()||(!DrvSclIrqGetIsBlankingRegion()))
        {
            DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(),
                (E_DRV_SCLIRQ_EVENT_FRMENDSYNC), &u32Events, E_DRV_SCLOS_EVENT_MD_AND, 200); // get status: FRM END
        }
        HalUtilityW2BYTEMSK(u32reg, u16val, 0xFFFF);
    }

}
u16 HalSclHvspPriMaskGetSramData(void)
{
    return HalUtilityR2BYTEDirect(REG_VIP_SCNR_1D_L);
}
void HalSclHvspSetWriteRegType(bool bEn)
{
    if(bEn && (VIPSETRULE()==E_DRV_SCLOS_VIPSETRUle_CMDQALL|| VIPSETRULE()==E_DRV_SCLOS_VIPSETRUle_CMDQALLCheck
        ||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLONLYSRAMCheck)))
    {
        DrvSclCmdqGetModuleMutex(E_DRV_SCLCMDQ_TYPE_IP0,1);
        DrvSclCmdqSetForceSkip(1);
        if(DrvSclIrqGetEachDMAEn())
        {
            gbSramCMDQ = 1;
        }
        else
        {
            gbSramCMDQ = 0;
        }
    }
    else if(bEn && VIPSETRULE())
    {
        DrvSclCmdqGetModuleMutex(E_DRV_SCLCMDQ_TYPE_IP0,1);
        DrvSclCmdqSetForceSkip(1);
        if(DrvSclIrqGetEachDMAEn()&& !DrvSclIrqGetIsBlankingRegion())
        {
            gbSramCMDQ = 1;
        }
        else
        {
            gbSramCMDQ = 0;
        }
    }
    else
    {
        gbSramCMDQ = 0;
        DrvSclCmdqSetForceSkip(0);
        DrvSclCmdqGetModuleMutex(E_DRV_SCLCMDQ_TYPE_IP0,0);
    }
}
void HalSclHvspWriteReg(u32 u32Reg,u16 u16Val,u16 u16Mask)
{
    DrvSclCmdqCmdReg_t stCfg;
    if(gbSramCMDQ)
    {
        DrvSclCmdqFillCmd(&stCfg, u32Reg, u16Val, u16Mask);
        DrvSclCmdqWriteCmd(E_DRV_SCLCMDQ_TYPE_IP0, stCfg, 1);
    }
    else
    {
        HalUtilityW2BYTEMSK(u32Reg, u16Val, u16Mask);//sec
    }
}
void HalSclHvspSRAMDumpHSP(u32 u32Sram,u32 u32reg)
{
    u16 u16addr = 0,u16tvalue = 0,u16tcount = 0,u16readdata;
    //clear
    for(u16addr = 64;u16addr<128;u16addr++)
    {
        HalSclHvspWriteReg(u32reg, BIT0, BIT0);//Yenable
        HalSclHvspWriteReg(u32reg+2, u16addr, 0xFF);
        HalSclHvspWriteReg(u32reg+4, 0, 0xFFFF);
        HalSclHvspWriteReg(u32reg+6, 0, 0xFFFF);
        HalSclHvspWriteReg(u32reg+8, 0, 0xFF);
        HalSclHvspWriteReg(u32reg, BIT0|BIT8, BIT0|BIT8);//W pulse
        HalSclHvspWriteReg(u32reg, 0, BIT0);//Yenable

        HalSclHvspWriteReg(u32reg, BIT1, BIT1);//Cenable
        HalSclHvspWriteReg(u32reg+4, 0, 0xFFFF);
        HalSclHvspWriteReg(u32reg+6, 0, 0xFFFF);
        HalSclHvspWriteReg(u32reg+8, 0, 0xFF);
        HalSclHvspWriteReg(u32reg, BIT1|BIT8, BIT1|BIT8);//W pulse
        HalSclHvspWriteReg(u32reg, 0, BIT1);//Cenable
    }
    //SRAM 0 //0~64 entry is V  65~127 is H

    for(u16addr=0;u16addr<(PQ_IP_SRAM1_SIZE_Main/5);u16addr++)
    {
           // 64~127
        HalSclHvspWriteReg(u32reg, BIT0, BIT0);//Yenable
        HalSclHvspWriteReg(u32reg+2, u16addr+64, 0xFF);
        if(u32Sram&0x1)
        {
            u16tvalue = MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount] |
                (MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount+1]<<8);
        }
        else
        {
            u16tvalue = MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount] |
                (MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount+1]<<8);
        }
        SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]VYtval12:%hx\n", u16tvalue);
        HalSclHvspWriteReg(u32reg+4, u16tvalue, 0xFFFF);
        if(u32Sram&0x1)
        {
            u16tvalue = MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount+2] |
                (MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount+3]<<8);
        }
        else
        {
            u16tvalue = MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount+2] |
                (MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount+3]<<8);
        }
        HalSclHvspWriteReg(u32reg+6, u16tvalue, 0xFFFF);
        SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]VYtval34:%hx\n", u16tvalue);
        if(u32Sram&0x1)
        {
            u16tvalue = MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount+4];
        }
        else
        {
            u16tvalue = MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount+4];
        }
        SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]Vtval5:%hx\n", u16tvalue);
        HalSclHvspWriteReg(u32reg+8, u16tvalue, 0xFF);
        HalSclHvspWriteReg(u32reg, BIT0|BIT8|BIT9, BIT0|BIT8|BIT9);//W pulse
        if(!gbSramCMDQ)
        {
            u16readdata = HalUtilityR2BYTEDirect(u32reg+10);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]Vread Yval 12:%hx\n", u16readdata);
            u16readdata = HalUtilityR2BYTEDirect(u32reg+12);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]Vread Yval 34:%hx\n", u16readdata);
            u16readdata = HalUtilityR2BYTEDirect(u32reg+14);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]Vread Yval 5:%hx\n", u16readdata);
        }
        HalSclHvspWriteReg(u32reg, 0, BIT0);//Yenable

        HalSclHvspWriteReg(u32reg, BIT1, BIT1);//Cenable
        u16tvalue = MST_C_SRAM1_Main[0][u16tcount] | (MST_C_SRAM1_Main[0][u16tcount+1]<<8);
        HalSclHvspWriteReg(u32reg+4, u16tvalue, 0xFFFF);
        u16tvalue = MST_C_SRAM1_Main[0][u16tcount+2] | (MST_C_SRAM1_Main[0][u16tcount+3]<<8);
        HalSclHvspWriteReg(u32reg+6, u16tvalue, 0xFFFF);
        u16tvalue = MST_C_SRAM1_Main[0][u16tcount+4];
        HalSclHvspWriteReg(u32reg+8, u16tvalue, 0xFF);
        HalSclHvspWriteReg(u32reg, BIT1|BIT8|BIT9, BIT1|BIT8|BIT9);//W pulse
        if(!gbSramCMDQ)
        {
            u16readdata = HalUtilityR2BYTEDirect(u32reg+10);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]Vread Cval 12:%hx\n", u16readdata);
            u16readdata = HalUtilityR2BYTEDirect(u32reg+12);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]Vread Cval 34:%hx\n", u16readdata);
            u16readdata = HalUtilityR2BYTEDirect(u32reg+14);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]Vread Cval 5:%hx\n", u16readdata);
        }
        HalSclHvspWriteReg(u32reg, 0, BIT1);//Cenable
        u16tcount += 5;
    }
    //SRAM 1 //128~256 entry  link to 0~127 , so do not use
    u16tcount = 0;
}
void HalSclHvspSRAMDumpVSP(u32 u32Sram,u32 u32reg)
{
    u16 u16addr = 0,u16tvalue = 0,u16tcount = 0,u16readdata;
    //clear
    for(u16addr = 0;u16addr<64;u16addr++)
    {
        HalSclHvspWriteReg(u32reg, BIT0, BIT0);//Yenable
        HalSclHvspWriteReg(u32reg+2, u16addr, 0xFF);
        HalSclHvspWriteReg(u32reg+4, 0, 0xFFFF);
        HalSclHvspWriteReg(u32reg+6, 0, 0xFFFF);
        HalSclHvspWriteReg(u32reg+8, 0, 0xFF);
        HalSclHvspWriteReg(u32reg, BIT8|BIT0, BIT8|BIT0);//W pulse
        HalSclHvspWriteReg(u32reg, 0, BIT0);//Yenable

        HalSclHvspWriteReg(u32reg, BIT1, BIT1);//Cenable
        HalSclHvspWriteReg(u32reg+4, 0, 0xFFFF);
        HalSclHvspWriteReg(u32reg+6, 0, 0xFFFF);
        HalSclHvspWriteReg(u32reg+8, 0, 0xFF);
        HalSclHvspWriteReg(u32reg, BIT1|BIT8, BIT1|BIT8);//W pulse
        HalSclHvspWriteReg(u32reg, 0, BIT1);//Cenable
    }
    //SRAM 0 ///0~64 entry is V  65~127 is H

    for(u16addr=0;u16addr<(PQ_IP_SRAM1_SIZE_Main/5);u16addr++)
    {
        HalSclHvspWriteReg(u32reg, BIT0, BIT0);//Yenable
        HalSclHvspWriteReg(u32reg+2, u16addr, 0xFF);
        if(u32Sram&0x1)
        {
            u16tvalue = MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount] |
                (MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount+1]<<8);
        }
        else
        {
            u16tvalue = MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount] |
                (MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount+1]<<8);
        }
        HalSclHvspWriteReg(u32reg+4, u16tvalue, 0xFFFF);
        SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]Ytval12:%hx\n", u16tvalue);
        if(u32Sram&0x1)
        {
            u16tvalue = MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount+2] |
                (MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount+3]<<8);
        }
        else
        {
            u16tvalue = MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount+2] |
                (MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount+3]<<8);
        }
        HalSclHvspWriteReg(u32reg+6, u16tvalue, 0xFFFF);
        SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]Ytval34:%hx\n", u16tvalue);
        if(u32Sram&0x1)
        {
            u16tvalue = MST_SRAM1_Main[((u32Sram&0xF0)>>4)][u16tcount+4];
        }
        else
        {
            u16tvalue = MST_SRAM2_Main[((u32Sram&0xF0)>>4)][u16tcount+4];
        }
        HalSclHvspWriteReg(u32reg+8, u16tvalue, 0xFF);
        SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]Ytval5:%hx\n", u16tvalue);
        HalSclHvspWriteReg(u32reg, BIT0|BIT8|BIT9, BIT0|BIT8|BIT9);//W pulse
        if(!gbSramCMDQ)
        {
            u16readdata = HalUtilityR2BYTEDirect(u32reg+10);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]read Yval 12:%hx\n", u16readdata);
            u16readdata = HalUtilityR2BYTEDirect(u32reg+12);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]read Yval 34:%hx\n", u16readdata);
            u16readdata = HalUtilityR2BYTEDirect(u32reg+14);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]read Yval 5:%hx\n", u16readdata);
        }
        HalSclHvspWriteReg(u32reg, 0, BIT0);//Yenable

        HalSclHvspWriteReg(u32reg, BIT1, BIT1);//Cenable
        u16tvalue = MST_C_SRAM1_Main[0][u16tcount] | (MST_C_SRAM1_Main[0][u16tcount+1]<<8);
        SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]Ctval12:%hx\n", u16tvalue);
        HalSclHvspWriteReg(u32reg+4, u16tvalue, 0xFFFF);
        u16tvalue = MST_C_SRAM1_Main[0][u16tcount+2] | (MST_C_SRAM1_Main[0][u16tcount+3]<<8);
        SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]Ctval34:%hx\n", u16tvalue);
        HalSclHvspWriteReg(u32reg+6, u16tvalue, 0xFFFF);
        u16tvalue = MST_C_SRAM1_Main[0][u16tcount+4];
        SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]Ctval5:%hx\n", u16tvalue);
        HalSclHvspWriteReg(u32reg+8, u16tvalue, 0xFF);
        HalSclHvspWriteReg(u32reg, BIT1|BIT8|BIT9, BIT1|BIT8|BIT9);//W pulse

        if(!gbSramCMDQ)
        {
            u16readdata = HalUtilityR2BYTEDirect(u32reg+10);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]read Cval 12:%hx\n", u16readdata);
            u16readdata = HalUtilityR2BYTEDirect(u32reg+12);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]read Cval 34:%hx\n", u16readdata);
            u16readdata = HalUtilityR2BYTEDirect(u32reg+14);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&EN_DBGMG_HVSPLEVEL_ELSE, "[HVSP]read Cval 5:%hx\n", u16readdata);
        }
        HalSclHvspWriteReg(u32reg, 0, BIT1);//Cenable
        u16tcount += 5;
    }
    //SRAM 1 //128~256 entry  link to 0~127 , so do not use
    u16tcount = 0;
}
bool HalSclHvspSetSRAMDump(HalSclHvspSramDumpType_e endump,u32 u32Sram,u32 u32reg)
{
    if(endump == E_HAL_SCLHVSP_SRAM_DUMP_HVSP_V || endump == E_HAL_SCLHVSP_SRAM_DUMP_HVSP_V_1||
        endump == E_HAL_SCLHVSP_SRAM_DUMP_HVSP_V_2)
    {
        HalSclHvspSRAMDumpVSP(u32Sram,u32reg);
    }
    else if(endump == E_HAL_SCLHVSP_SRAM_DUMP_HVSP_H || endump == E_HAL_SCLHVSP_SRAM_DUMP_HVSP_H_1||
        endump == E_HAL_SCLHVSP_SRAM_DUMP_HVSP_H_2)
    {
        HalSclHvspSRAMDumpHSP(u32Sram,u32reg);
    }
    return 1;
}
bool HalSclHvspSramDump(HalSclHvspSramDumpType_e endump,u32 u32Sram)
{
    u16 u16clkreg;
    bool bRet;
    u32 u32reg = endump == E_HAL_SCLHVSP_SRAM_DUMP_HVSP_V ? REG_SCL_HVSP0_41_L:
                    endump == E_HAL_SCLHVSP_SRAM_DUMP_HVSP_V_1 ? REG_SCL_HVSP1_41_L:
                    endump == E_HAL_SCLHVSP_SRAM_DUMP_HVSP_V_2 ? REG_SCL_HVSP2_41_L:
                    endump == E_HAL_SCLHVSP_SRAM_DUMP_HVSP_H ? REG_SCL_HVSP0_41_L:
                    endump == E_HAL_SCLHVSP_SRAM_DUMP_HVSP_H_1 ? REG_SCL_HVSP1_41_L:
                    endump == E_HAL_SCLHVSP_SRAM_DUMP_HVSP_H_2 ? REG_SCL_HVSP2_41_L:
                        0;
    //clk open
    if(endump == E_HAL_SCLHVSP_SRAM_DUMP_HVSP_V_2 || endump == E_HAL_SCLHVSP_SRAM_DUMP_HVSP_H_2)
    {
        u16clkreg = HalUtilityR2BYTEDirect(REG_SCL_CLK_65_L);
        HalUtilityW2BYTEMSK(REG_SCL_CLK_65_L,0x0,0xFFFF);
    }
    else
    {
        u16clkreg = HalUtilityR2BYTEDirect(REG_SCL_CLK_64_L);
        HalUtilityW2BYTEMSK(REG_SCL_CLK_64_L,0x0,0xFFFF);
        HalSclHvspSetWriteRegType(1);
    }
    bRet = HalSclHvspSetSRAMDump(endump,u32Sram,u32reg);

    //clk close
    if(endump == E_HAL_SCLHVSP_SRAM_DUMP_HVSP_V_2|| endump == E_HAL_SCLHVSP_SRAM_DUMP_HVSP_H_2)
    {
        HalUtilityW2BYTEMSK(REG_SCL_CLK_65_L,u16clkreg,0xFFFF);
    }
    else
    {
        if(gbSramCMDQ)
        {
            DrvSclCmdqFire(E_DRV_SCLCMDQ_TYPE_IP0,TRUE);
        }
        HalSclHvspSetWriteRegType(0);
        HalUtilityW2BYTEMSK(REG_SCL_CLK_64_L,u16clkreg,0xFFFF);
    }
    return bRet;
}

#undef HAL_HVSP_C
