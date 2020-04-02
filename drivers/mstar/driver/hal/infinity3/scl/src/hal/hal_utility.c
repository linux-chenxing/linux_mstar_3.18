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
#define HAL_SCLIRQ_C


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#include "drv_scl_os.h"

// Internal Definition
#include "hal_scl_reg.h"
#include "hal_utility.h"
#include "hal_scl_util.h"
#include "drv_scl_dbg.h"
#include "drv_scl_cmdq.h"
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DrvUtilityMutexLock()            DrvSclOsObtainMutex(_UTILITY_Mutex,SCLOS_WAIT_FOREVER)
#define DrvUtilityMutexUNLock()          DrvSclOsReleaseMutex(_UTILITY_Mutex)


//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u32 UTILITY_RIU_BASE = 0;
u32 UTILITY_RIU_BASE_Vir = 0;
s32 _UTILITY_Mutex = -1;
void *gpvRegBuffer = NULL;
void *gpvDefaultRegBuffer = NULL;
bool bFirstTimeTrig = 1;
//-------------------------------------------------------------------------------------------------
//  Functions
//-------------------------------------------------------------------------------------------------
u32 _HalUtilitySwitchBankToType(u32 u32Bank)
{
    EN_REG_CONFIG_TYPE enType;
    switch(u32Bank)
    {
        case REG_SCL_HVSP0_BASE:
            enType = EN_REG_SCL_HVSP0_BASE;
            break;
        case REG_SCL_HVSP1_BASE:
            enType = EN_REG_SCL_HVSP1_BASE;
            break;
        case REG_SCL_HVSP2_BASE:
            enType = EN_REG_SCL_HVSP2_BASE;
            break;
        case REG_SCL_DMA0_BASE:
            enType = EN_REG_SCL_DMA0_BASE;
            break;
        case REG_SCL_DMA1_BASE:
            enType = EN_REG_SCL_DMA1_BASE;
            break;
        case REG_SCL_DMA2_BASE:
            enType = EN_REG_SCL_DMA2_BASE;
            break;
        case REG_SCL_DNR0_BASE:
            enType = EN_REG_SCL_DNR0_BASE;
            break;
        case REG_SCL_NLM0_BASE:
            enType = EN_REG_SCL_NLM0_BASE;
            break;
        case REG_SCL0_BASE:
            enType = EN_REG_SCL0_BASE;
            break;
        case REG_SCL1_BASE:
            enType = EN_REG_SCL1_BASE;
            break;
        case REG_SCL2_BASE:
            enType = EN_REG_SCL2_BASE;
            break;
        case REG_VIP_ACE3_BASE:
            enType = EN_REG_VIP_ACE3_BASE;
            break;
        case REG_VIP_ACE_BASE:
            enType = EN_REG_VIP_ACE_BASE;
            break;
        case REG_VIP_PK_BASE:
            enType = EN_REG_VIP_PK_BASE;
            break;
        case REG_VIP_DLC_BASE:
            enType = EN_REG_VIP_DLC_BASE;
            break;
        case REG_VIP_MWE_BASE:
            enType = EN_REG_VIP_MWE_BASE;
            break;
        case REG_VIP_ACE2_BASE:
            enType = EN_REG_VIP_ACE2_BASE;
            break;
        case REG_VIP_LCE_BASE:
            enType = EN_REG_VIP_LCE_BASE;
            break;
        case REG_SCL_DNR1_BASE:
            enType = EN_REG_SCL_DNR1_BASE;
            break;
        case REG_SCL_DNR2_BASE:
            enType = EN_REG_SCL_DNR2_BASE;
            break;
        case REG_SCL_LDC_BASE:
            enType = EN_REG_SCL_LDC_BASE;
            break;
        case REG_SCL3_BASE:
            enType = EN_REG_SCL3_BASE;
            break;
        case REG_VIP_SCNR_BASE:
            enType = EN_REG_VIP_SCNR_BASE;
            break;
        case REG_VIP_WDR_BASE:
            enType = EN_REG_VIP_WDR_BASE;
            break;
        case REG_VIP_YEE_BASE:
            enType = EN_REG_VIP_YEE_BASE;
            break;
        case REG_VIP_MCNR_BASE:
            enType = EN_REG_VIP_MCNR_BASE;
            break;
        case REG_CMDQ_BASE:
            enType = EN_REG_CMDQ0_BASE;
            break;
        case REG_CMDQ1_BASE:
            enType = EN_REG_CMDQ1_BASE;
            break;
        case REG_CMDQ2_BASE:
            enType = EN_REG_CMDQ2_BASE;
            break;
        case REG_ISP_BASE:
            enType = EN_REG_ISP_BASE;
            break;
        case REG_SCL_LPLL_BASE:
            enType = EN_REG_SCL_LPLL_BASE;
            break;
        case REG_SCL_CLK_BASE:
            enType = EN_REG_SCL_CLK_BASE;
            break;
        case REG_BLOCK_BASE:
            enType = EN_REG_BLOCK_BASE;
            break;
        case REG_CHIPTOP_BASE:
            enType = EN_REG_CHIPTOP_BASE;
            break;
        default:
            enType = EN_REG_NUM_CONFIG;
            break;
    }
    return enType;
}
u32 _HalUtilitySwitchTypeToBank(EN_REG_CONFIG_TYPE enType)
{
    u32 u32bankaddr;
    switch(enType)
    {
        case EN_REG_SCL_HVSP0_BASE:
            u32bankaddr = REG_SCL_HVSP0_BASE;
            break;
        case EN_REG_SCL_HVSP1_BASE:
            u32bankaddr = REG_SCL_HVSP1_BASE;
            break;
        case EN_REG_SCL_HVSP2_BASE:
            u32bankaddr = REG_SCL_HVSP2_BASE;
            break;
        case EN_REG_SCL_DMA0_BASE:
            u32bankaddr = REG_SCL_DMA0_BASE;
            break;
        case EN_REG_SCL_DMA1_BASE:
            u32bankaddr = REG_SCL_DMA1_BASE;
            break;
        case EN_REG_SCL_DMA2_BASE:
            u32bankaddr = REG_SCL_DMA2_BASE;
            break;
        case EN_REG_SCL_DNR0_BASE:
            u32bankaddr = REG_SCL_DNR0_BASE;
            break;
        case EN_REG_SCL_NLM0_BASE:
            u32bankaddr = REG_SCL_NLM0_BASE;
            break;
        case EN_REG_SCL0_BASE:
            u32bankaddr = REG_SCL0_BASE;
            break;
        case EN_REG_SCL1_BASE:
            u32bankaddr = REG_SCL1_BASE;
            break;
        case EN_REG_SCL2_BASE:
            u32bankaddr = REG_SCL2_BASE;
            break;
        case EN_REG_VIP_ACE3_BASE:
            u32bankaddr = REG_VIP_ACE3_BASE;
            break;
        case EN_REG_VIP_ACE_BASE:
            u32bankaddr = REG_VIP_ACE_BASE;
            break;
        case EN_REG_VIP_PK_BASE:
            u32bankaddr = REG_VIP_PK_BASE;
            break;
        case EN_REG_VIP_DLC_BASE:
            u32bankaddr = REG_VIP_DLC_BASE;
            break;
        case EN_REG_VIP_MWE_BASE:
            u32bankaddr = REG_VIP_MWE_BASE;
            break;
        case EN_REG_VIP_ACE2_BASE:
            u32bankaddr = REG_VIP_ACE2_BASE;
            break;
        case EN_REG_VIP_LCE_BASE:
            u32bankaddr = REG_VIP_LCE_BASE;
            break;
        case EN_REG_SCL_DNR1_BASE:
            u32bankaddr = REG_SCL_DNR1_BASE;
            break;
        case EN_REG_SCL_DNR2_BASE:
            u32bankaddr = REG_SCL_DNR2_BASE;
            break;
        case EN_REG_SCL_LDC_BASE:
            u32bankaddr = REG_SCL_LDC_BASE;
            break;
        case EN_REG_SCL3_BASE:
            u32bankaddr = REG_SCL3_BASE;
            break;
        case EN_REG_VIP_SCNR_BASE:
            u32bankaddr = REG_VIP_SCNR_BASE;
            break;
        case EN_REG_VIP_WDR_BASE:
            u32bankaddr = REG_VIP_WDR_BASE;
            break;
        case EN_REG_VIP_YEE_BASE:
            u32bankaddr = REG_VIP_YEE_BASE;
            break;
        case EN_REG_VIP_MCNR_BASE:
            u32bankaddr = REG_VIP_MCNR_BASE;
            break;
        case EN_REG_CMDQ0_BASE:
            u32bankaddr = REG_CMDQ_BASE;
            break;
        case EN_REG_CMDQ1_BASE:
            u32bankaddr = REG_CMDQ1_BASE;
            break;
        case EN_REG_CMDQ2_BASE:
            u32bankaddr = REG_CMDQ2_BASE;
            break;
        default:
            u32bankaddr = 0;
            break;
    }
    return u32bankaddr;
}
void _HalUtilityReadBufferToDebug(EN_REG_CONFIG_TYPE enType)
{
    u16 *p16buffer = NULL;
    u16 u16idx;
    if(gpvRegBuffer)
    {
        SCL_ERR("enType:%d\n",enType);
        // data
        // 4byte (2byte val+2byte 0)
        // 1bank
        // 128 addr 512byte
        p16buffer = (u16 *)(gpvRegBuffer +(BANKSIZE*enType));
        for(u16idx=0;u16idx<(BANKSIZE/4);u16idx++)
        {
            SCL_ERR("addr:%hx val:%hx\n",u16idx,*(p16buffer+u16idx*2));
        }
    }
}
void _HalUtilityReadBankCpyToBuffer(EN_REG_CONFIG_TYPE enType)
{
    u32 u32Bank;
    void *pvBankaddr;
    if(gpvRegBuffer)
    {
        u32Bank = _HalUtilitySwitchTypeToBank(enType);
        pvBankaddr = RIU_GET_ADDR(u32Bank);
        DrvUtilityMutexLock();
        DrvSclOsMemcpy((void *)(gpvRegBuffer +(BANKSIZE*enType)),(void *)pvBankaddr,BANKSIZE);
        DrvUtilityMutexUNLock();
        SCL_DBG(SCL_DBG_LV_UTILITY()&0x2, "[SCLUTILITY]%s(%d)::%d:%lx, @(%lx)\n",
            __FUNCTION__, __LINE__,enType,u32Bank,(u32)pvBankaddr);
    }
}
void _HalUtilityInitRegBuffer(void)
{
    u16 u16idx;
    for(u16idx = 0;u16idx<EN_REG_NUM_CONFIG;u16idx++)
    {
        _HalUtilityReadBankCpyToBuffer(u16idx);
    }
}
bool _HalUtilitySetCmdqBufferIdx(u16 u16Val,u32 u32Reg)
{
    EN_REG_CONFIG_TYPE enType;
    u32 u32Bank;
    u16 u16Addr;
    u16 *pvBankaddr;
    u32Bank = (u32Reg&0xFFFF00);
    u16Addr = (u16)(u32Reg&0xFE); //only low byte
    enType = _HalUtilitySwitchBankToType(u32Bank);
    if(enType< EN_REG_NUM_CONFIG &&gpvRegBuffer)
    {
        pvBankaddr = (u16 *)(gpvRegBuffer + (BANKSIZE*enType) + (u16Addr<<1) +2);
        *pvBankaddr = (u16Val|FLAG_CMDQ_IDXEXIST);
        SCL_DBG(SCL_DBG_LV_UTILITY(), "[SCLUTILITY]CMDQ 2ByteReg:%lx pvBankaddr:%lx val:%hx\n",
            u32Reg,(u32)pvBankaddr,*pvBankaddr);
        return 1;
    }
    else
    {
        return 0;
    }
}
bool _HalUtilityGetCmdqBufferIdx(u16 *u16Val,u32 u32Reg)
{
    EN_REG_CONFIG_TYPE enType;
    u32 u32Bank;
    u16 u16Addr;
    u16 *pvBankaddr;
    u32Bank = (u32Reg&0xFFFF00);
    u16Addr = (u16)(u32Reg&0xFE); //only low byte
    enType = _HalUtilitySwitchBankToType(u32Bank);
    if(enType< EN_REG_NUM_CONFIG &&gpvRegBuffer)
    {
        pvBankaddr = (u16 *)(gpvRegBuffer + (BANKSIZE*enType) + (u16Addr<<1) +2);
        *u16Val = ((*pvBankaddr)& (~FLAG_CMDQ_IDXEXIST));
        SCL_DBG(SCL_DBG_LV_UTILITY(), "[SCLUTILITY]CMDQ 2ByteReg:%lx pvBankaddr:%lx val:%hx\n",
            u32Reg,(u32)pvBankaddr,*pvBankaddr);
        if((*pvBankaddr) & FLAG_CMDQ_IDXEXIST)
        {
            SCL_DBG(SCL_DBG_LV_UTILITY(), "[SCLUTILITY]CMDQ 2ByteReg:%lx val:%hx already exist CMDQ\n",
                u32Reg,(u16)*u16Val);
            return 0;
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_UTILITY(), "[SCLUTILITY]CMDQ 2ByteReg:%lx val:%hx not ready exist CMDQ\n",
                u32Reg,(u16)*u16Val);
            return 1;
        }
    }
    else
    {
        return 1;
    }
}
u16 _HalUtilityWrite2ByteRegBuffer(u32 u32Reg,u16 u16Val,u16 u16Mask)
{
    EN_REG_CONFIG_TYPE enType;
    u32 u32Bank;
    u16 u16Addr;
    u16 *pvBankaddr;
    u32Bank = (u32Reg&0xFFFF00);
    u16Addr = (u16)(u32Reg&0xFF);
    enType = _HalUtilitySwitchBankToType(u32Bank);
    if(enType< EN_REG_NUM_CONFIG &&gpvRegBuffer)
    {
        pvBankaddr = (u16 *)(gpvRegBuffer + (BANKSIZE*enType) + (u16Addr<<1));
        SCL_DBG(SCL_DBG_LV_UTILITY()&0x2, "[SCLUTILITY]2ByteReg:%lx val:%hx mask:%hx pvBankaddr:%lx val:%hx\n",
            u32Reg,u16Val,u16Mask,(u32)pvBankaddr,*pvBankaddr);
        //DrvUtilityMutexLock();
        *pvBankaddr = (u16)((u16Val&u16Mask)|(*pvBankaddr&(~u16Mask)));
        //DrvUtilityMutexUNLock();
        return *(pvBankaddr);
    }
    else
    {
        return 0;
    }
}
u8 _HalUtilityWriteByteRegBuffer(u32 u32Reg,u8 u8Val,u8 u8Mask)
{
    EN_REG_CONFIG_TYPE enType;
    u32 u32Bank;
    u16 u16Addr;
    u8 *pvBankaddr;
    u32Bank = (u32Reg&0xFFFF00);
    u16Addr = (u16)(u32Reg&0xFF);
    enType = _HalUtilitySwitchBankToType(u32Bank);
    if(enType< EN_REG_NUM_CONFIG  &&gpvRegBuffer)
    {
        //(((u32Reg)<<1) - ((u32Reg) & 1))
        pvBankaddr = (u8 *)(gpvRegBuffer + (BANKSIZE*enType) + ((u16Addr<<1)-((u16Addr) & 1)));
        SCL_DBG(SCL_DBG_LV_UTILITY()&0x2, "[SCLUTILITY]ByteReg:%lx val:%hhx mask:%hhx pvBankaddr:%lx val:%hx\n",
            u32Reg,u8Val,u8Mask,(u32)pvBankaddr,(u8)*pvBankaddr);
        //DrvUtilityMutexLock();
        *pvBankaddr = (u8)((u8Val&u8Mask)|(*pvBankaddr&(~u8Mask)));
        SCL_DBG(SCL_DBG_LV_UTILITY()&0x2, "[SCLUTILITY]ByteRegval:%hhx\n",
            (u8)*pvBankaddr);
        //DrvUtilityMutexUNLock();
        return (u8)*(pvBankaddr);
    }
    else
    {
        return 0;
    }
}
u16 _HalUtilityReadRegBuffer(u32 u32Reg)
{
    EN_REG_CONFIG_TYPE enType;
    u32 u32Bank;
    u16 u16Addr;
    u16 *pvBankaddr = NULL;
    if(gpvRegBuffer)
    {
        u32Bank = (u32Reg&0xFFFF00);
        u16Addr = (u16)(u32Reg&0xFF);
        enType = _HalUtilitySwitchBankToType(u32Bank);
        //DrvUtilityMutexLock();
        if(enType<EN_REG_NUM_CONFIG)
        {
            pvBankaddr = (u16 *)(gpvRegBuffer + (BANKSIZE*enType) + (u16Addr<<1));
            //DrvUtilityMutexUNLock();
            return *(pvBankaddr);
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}
u32 HalUtilityR4BYTEDirect(u32 u32Reg)
{
    u32 u32RetVal;
    u32RetVal = R4BYTE(u32Reg);
    return u32RetVal;
}
u16 HalUtilityR2BYTEDirect(u32 u32Reg)
{
    u16 u16RetVal;
    u16RetVal = R2BYTE(u32Reg);
    return u16RetVal;
}
u16 HalUtilityR2BYTEMaskDirect(u32 u32Reg,u16 u16Mask)
{
    u16 u16RetVal;
    u16RetVal = R2BYTEMSK(u32Reg,u16Mask);
    return u16RetVal;
}
bool HalUtilityGetCmdqBufferIdx(u16 *u16BufferCnt,u32 u32Reg)
{
    bool bRet;
    bRet = _HalUtilityGetCmdqBufferIdx(u16BufferCnt,u32Reg);
    return bRet;
}
bool HalUtilitySetCmdqBufferIdx(u16 u16BufferCnt,u32 u32Reg)
{
    bool bRet;
    bRet = _HalUtilitySetCmdqBufferIdx(u16BufferCnt,u32Reg);
    return bRet;
}

#if USE_Utility
void HalUtilitySetFirstTimeTrig(bool bSet)
{
    bFirstTimeTrig = bSet;
}
void HalUtilityCmdqFire(void)
{
    if(bFirstTimeTrig)
    {
        DrvSclCmdqFireFirstTime();
    }
    DrvSclCmdqFireDirectBufferRingMode(E_DRV_SCLCMDQ_TYPE_IP0,TRUE);

}
u16 HalUtilityR2BYTE(u32 u32Reg)
{
    u16 u16RetVal;
    u16 u16ChkVal;
    u16RetVal = _HalUtilityReadRegBuffer(u32Reg);
    u16ChkVal = HalUtilityR2BYTEDirect(u32Reg);
    if(u16ChkVal != u16RetVal)
    {
        SCL_DBG(SCL_DBG_LV_UTILITY()&0x2, "[SCLUTILITY]u32Reg:%lx chkval:%hx RetVal:%hx \n",
            u32Reg,u16ChkVal,u16RetVal);
    }
    return u16RetVal;
}
void HalUtilityW2BYTECmdq(u32 u32Reg,u16 u16Val)
{
    if(_HalUtilityWrite2ByteRegBuffer(u32Reg, u16Val, 0xFFFF))
    {
        DrvSclCmdqWrite(E_DRV_SCLCMDQ_TYPE_IP0, u32Reg, u16Val, 0xFFFF);
    }
    else
    {
        W2BYTE(u32Reg,u16Val);
    }
}
void HalUtilityW2BYTEDirect(u32 u32Reg,u16 u16Val)
{
    _HalUtilityWrite2ByteRegBuffer(u32Reg, u16Val, 0xFFFF);
    W2BYTE(u32Reg,u16Val);
}
void HalUtilityW2BYTE(u32 u32Reg,u16 u16Val)
{
    if((DrvSclOsGetAccessRegMode() ==  E_DRV_SCLOS_AccessReg_CMDQ))
    {
        HalUtilityW2BYTECmdq(u32Reg,u16Val);
    }
    else
    {
        HalUtilityW2BYTEDirect(u32Reg,u16Val);
    }
}
void HalUtilityW4BYTECmdq(u32 u32Reg,u32 u32Val)
{
    if(_HalUtilityWrite2ByteRegBuffer(u32Reg, (u16)(u32Val&0xFFFF), 0xFFFF))
    {
        _HalUtilityWrite2ByteRegBuffer(u32Reg+2, (u16)((u32Val&0xFFFF0000)>>16), 0xFFFF);
        DrvSclCmdqWrite(E_DRV_SCLCMDQ_TYPE_IP0, u32Reg, (u16)(u32Val&0xFFFF), 0xFFFF);
        DrvSclCmdqWrite(E_DRV_SCLCMDQ_TYPE_IP0, u32Reg+2, (u16)((u32Val&0xFFFF0000)>>16), 0xFFFF);
    }
    else
    {
        W4BYTE(u32Reg,u32Val);
    }
}
void HalUtilityW4BYTEDirect(u32 u32Reg,u32 u32Val)
{
    _HalUtilityWrite2ByteRegBuffer(u32Reg, (u16)(u32Val&0xFFFF), 0xFFFF);
    _HalUtilityWrite2ByteRegBuffer(u32Reg+2, (u16)((u32Val&0xFFFF0000)>>16), 0xFFFF);
    W4BYTE(u32Reg,u32Val);
}
void HalUtilityW4BYTE(u32 u32Reg,u32 u32Val)
{
    if((DrvSclOsGetAccessRegMode() ==  E_DRV_SCLOS_AccessReg_CMDQ))
    {
        HalUtilityW4BYTECmdq(u32Reg,u32Val);
    }
    else
    {
        HalUtilityW4BYTEDirect(u32Reg,u32Val);
    }
}
void HalUtilityW2BYTEMSKCmdq(u32 u32Reg,u16 u16Val,u16 u16Mask)
{
    if(_HalUtilityWrite2ByteRegBuffer(u32Reg, u16Val, u16Mask))
    {
        DrvSclCmdqWrite(E_DRV_SCLCMDQ_TYPE_IP0, u32Reg, u16Val, u16Mask);
    }
    else
    {
        W2BYTEMSK(u32Reg,u16Val,u16Mask);
    }
}
void HalUtilityW2BYTEMSKDirect(u32 u32Reg,u16 u16Val,u16 u16Mask)
{
    _HalUtilityWrite2ByteRegBuffer(u32Reg, u16Val, u16Mask);
    W2BYTEMSK(u32Reg,u16Val,u16Mask);
}
void HalUtilityW2BYTEMSK(u32 u32Reg,u16 u16Val,u16 u16Mask)
{
    if((DrvSclOsGetAccessRegMode() ==  E_DRV_SCLOS_AccessReg_CMDQ))
    {
        HalUtilityW2BYTEMSKCmdq(u32Reg,u16Val,u16Mask);
    }
    else
    {
        HalUtilityW2BYTEMSKDirect(u32Reg,u16Val,u16Mask);
    }
}
void HalUtilityW2BYTEMSKBuffer(u32 u32Reg,u16 u16Val,u16 u16Mask)
{
    u16 u16RetVal;
    u16RetVal = _HalUtilityWrite2ByteRegBuffer(u32Reg, u16Val, u16Mask);
    u16RetVal = u16RetVal;
}
void HalUtilityWBYTEMSKCmdq(u32 u32Reg,u16 u16Val,u16 u16Mask)
{
    if(_HalUtilityWriteByteRegBuffer(u32Reg, (u8)u16Val, (u8)u16Mask))
    {
        DrvSclCmdqWrite(E_DRV_SCLCMDQ_TYPE_IP0, u32Reg, u16Val, u16Mask);
    }
    else
    {
        WBYTEMSK(u32Reg,u16Val,u16Mask);
    }
}
void HalUtilityWBYTEMSKDirect(u32 u32Reg,u16 u16Val,u16 u16Mask)
{
    _HalUtilityWriteByteRegBuffer(u32Reg, (u8)u16Val, (u8)u16Mask);
    WBYTEMSK(u32Reg,u16Val,u16Mask);
}
void HalUtilityWBYTEMSK(u32 u32Reg,u16 u16Val,u16 u16Mask)
{
    if((DrvSclOsGetAccessRegMode() ==  E_DRV_SCLOS_AccessReg_CMDQ))
    {
        HalUtilityWBYTEMSKCmdq(u32Reg,u16Val,u16Mask);
    }
    else
    {
        HalUtilityWBYTEMSKDirect(u32Reg,u16Val,u16Mask);
    }
}
void HalUtilityWBYTEMSKBuffer(u32 u32Reg,u8 u8Val,u8 u8Mask)
{
    u8 u8RetVal;
    u8RetVal = _HalUtilityWriteByteRegBuffer(u32Reg, (u8)u8Val, (u8)u8Mask);
    u8RetVal = u8RetVal;
}
#else
u16 HalUtilityR2BYTE(u32 u32Reg)
{
    u16 u16RetVal;
    u16RetVal = HalUtilityR2BYTEDirect(u32Reg);
    return u16RetVal;
}
void HalUtilityW2BYTE(u32 u32Reg,u16 u16Val)
{
    W2BYTE(u32Reg,u16Val);
}
void HalUtilityW4BYTE(u32 u32Reg,u32 u32Val)
{
    W4BYTE(u32Reg,u32Val);
}
void HalUtilityW2BYTEMSK(u32 u32Reg,u16 u16Val,u16 u16Mask)
{
    W2BYTEMSK(u32Reg,u16Val,u16Mask);
}
void HalUtilityW2BYTEMSKBuffer(u32 u32Reg,u16 u16Val,u16 u16Mask)
{
}
void HalUtilityWBYTEMSK(u32 u32Reg,u16 u16Val,u16 u16Mask)
{
    WBYTEMSK(u32Reg,u16Val,u16Mask);
}
void HalUtilityWBYTEMSKBuffer(u32 u32Reg,u8 u8Val,u8 u8Mask)
{
}
#endif
bool HalUtilityCtxRegBufferInit(void *pvRegBuffer)
{
    void *pvOriBuffer;
    pvOriBuffer = gpvRegBuffer;
    if(pvRegBuffer)
    {
        gpvRegBuffer = pvRegBuffer;
        _HalUtilityInitRegBuffer();
        gpvRegBuffer = pvOriBuffer;
    }
    return 1;
}
bool HalUtilityInit(void)
{
    //1.create mutex
    //2.allocate buffer 1bank=256byte x EN_REG_NUM_CONFIG
    //3.read all address and keep in buffer
    char mutx_word[] = {"_UTILITY_Mutex"};
    UTILITY_RIU_BASE_Vir = VIR_RIUBASE;
    // 1.
    if(_UTILITY_Mutex != -1)
    {
        SCL_DBG(SCL_DBG_LV_UTILITY(), "[SCLUTILITY]%s(%d)::Already Done\n", __FUNCTION__, __LINE__);
        return TRUE;
    }
    if(DrvSclOsInit() == FALSE)
    {
        SCL_ERR("[SCLUTILITY]%s(%d)::MsOS_Init Fail\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
    _UTILITY_Mutex = DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO, mutx_word, SCLOS_PROCESS_SHARED);

    //2.
    #if USE_Utility
    gpvDefaultRegBuffer = DrvSclOsVirMemalloc(BANKSIZE*EN_REG_NUM_CONFIG);
    if(!gpvDefaultRegBuffer)
    {
        SCL_ERR("[SCLUTILITY]%s(%d)::Allocate Fail\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
    else
    {
        gpvRegBuffer = gpvDefaultRegBuffer;
        SCL_ERR("[SCLUTILITY]%s(%d)::Allocate Success!\n", __FUNCTION__, __LINE__);
    }

    //3.
    _HalUtilityInitRegBuffer();
    #endif
    return TRUE;
}

#undef HAL_SCLIRQ_C
