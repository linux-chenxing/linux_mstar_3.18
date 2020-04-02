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
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
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

////////////////////////////////////////////////////////////////////////////////////////////////////
// file   hal_scl_cmdq.c
// @brief  CMDQ HAL
// @author MStar Semiconductor,Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "drv_scl_os.h"
#include "hal_utility.h"
#include "hal_scl_reg.h"
#include "hal_scl_util.h"
#include "drv_scl_os.h"

#include "drv_scl_cmdq.h"
#include "hal_scl_cmdq.h"

#define  CMDQDBG(x)



//--------------------------------------------------------------------------------------------------
//  Driver Compiler Option
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//  Macro of bit operations
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Inline Function
//--------------------------------------------------------------------------------------------------

void HalSclCmdqInitRiuBase(u32 u32PM_riu_base)
{
}

//---------------------------------------------------------------------------
///set the element of _CMDQCtrl
///  .CMDQ_Enable
///  .CMDQ_Length_ReadMode
///  .CMDQ_Mask_Setting
//---------------------------------------------------------------------------
u32 HalSclCmdqGetRegFromIPID(DrvSclCmdqIpType_e enIPType, u8 u8addr)
{
    u32 Reg = 0;
    switch(enIPType)
    {
        case E_DRV_SCLCMDQ_TYPE_IP0:
            Reg = REG_CMDQ_BASE;
            break;

        case E_DRV_SCLCMDQ_TYPE_IP1:
            Reg = REG_CMDQ1_BASE;
            break;
        case E_DRV_SCLCMDQ_TYPE_IP2:
            Reg = REG_CMDQ2_BASE;
            break;
        default:
            Reg = REG_CMDQ_BASE;
            break;
    }
    Reg += (u8addr*2);
    return Reg;

}
u16 HalSclCmdqGetFinalIrq(DrvSclCmdqIpType_e enIPType,u16 u16Mask)
{
    u16 u16val;
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x44);
    u16val = R2BYTEMSK(Reg, u16Mask);;
    return u16val;
}
u16 HalSclCmdqGetRawIrq(DrvSclCmdqIpType_e enIPType,u16 u16Mask)
{
    u16 u16val;
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x45);
    u16val = R2BYTEMSK(Reg, u16Mask);;
    return u16val;
}

void HalSclCmdqEnable(DrvSclCmdqIpType_e enIPType,bool bEn)
{
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x1);
    if(bEn)
    {
        HalUtilityW2BYTEMSKDirect(Reg, 0x01, 0x01);

    }
    else
    {
        HalUtilityW2BYTEMSKDirect(Reg, 0x00, 0x01);
    }

}
void HalSclCmdqSetBaseEn(DrvSclCmdqIpType_e enIPType, bool bEn)
{
    u32 Reg = 0;
    u32 Reg1 = 0;
    u32 Reg2 = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x1);
    Reg1 = HalSclCmdqGetRegFromIPID(enIPType ,0x11);
    Reg2 = HalSclCmdqGetRegFromIPID(enIPType ,0x20);
    if(bEn)
    {
      HalUtilityW2BYTEMSK(Reg, 0x01, 0x01);
      HalUtilityW2BYTEMSK(Reg1, 0x190d, 0x1FFF);
      HalUtilityW2BYTEMSK(Reg2, 0x0080, 0x0080);
    }
    else
    {

      HalUtilityW2BYTEMSK(Reg1, 0x00, 0x10FF);
      HalUtilityW2BYTEMSK(Reg2, 0x00, 0x0080);
    }

}


//---------------------------------------------------------------------------
///set the element of _CMDQCtrl
///  .CMDQ_En_Clk_Miu
//---------------------------------------------------------------------------
void HalSclCmdqReset(DrvSclCmdqIpType_e enIPType, bool bEn)
{
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x31);
    if(bEn)
    {
      HalUtilityW2BYTEMSKDirect(Reg, 0x00, 0x01);
    }
    else
    {
      HalUtilityW2BYTEMSKDirect(Reg, 0x01, 0x01);
    }
}
void HalSclCmdqSetTrigAlways(DrvSclCmdqIpType_e enIPType, bool bEn)
{
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x2);
    if(bEn)
        HalUtilityW2BYTEMSK(Reg, 0x00, 0x01);
    else
        HalUtilityW2BYTEMSK(Reg, 0x01, 0x01);
}
bool HalSclCmdqSetDMAMode(DrvSclCmdqIpType_e enIPType,int ModeSel)
{
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x2);
    if (ModeSel==E_DRV_SCLCMDQ_DIRECT_MODE)
    {
        HalUtilityW2BYTEMSK(Reg, 0x00, (BIT1|BIT2));
        return TRUE;
    }
    else if(ModeSel==E_DRV_SCLCMDQ_INCREMENT_MODE)
    {
        HalUtilityW2BYTEMSK(Reg, BIT1, (BIT1|BIT2));
        return TRUE;
    }
    else
    {
        HalUtilityW2BYTEMSK(Reg, BIT2, (BIT1|BIT2));
        return TRUE;
    }
}

void HalSclCmdqSetStartPointer(DrvSclCmdqIpType_e enIPType,u32 StartAddr)
{
    u32 temp;
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x4);
    temp=(u32)(StartAddr);
    temp=temp/16;
    HalUtilityW4BYTE(Reg,temp);
}

void HalSclCmdqSetEndPointer(DrvSclCmdqIpType_e enIPType,u32 EndAddr)
{
    u32 temp;
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x6);
    temp=(u32)(EndAddr);
    temp=temp/16;
    HalUtilityW4BYTE(Reg,temp);
}

void HalSclCmdqSetOffsetPointer(DrvSclCmdqIpType_e enIPType,u32 OffsetAddr)
{
    u32 temp;
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x8);
    temp=(u32)(OffsetAddr);
    temp=temp>>4;
    CMDQDBG(sclprintf("End_Pointer _bits:%lx\n", temp));
    HalUtilityW4BYTEDirect(Reg,temp);
}

void HalSclCmdqSetTimer(DrvSclCmdqIpType_e enIPType,u32 time)
{
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x4A);
    HalUtilityW2BYTEMSK(Reg, time, 0xFFFF);
}

void HalSclCmdqSetRatio(DrvSclCmdqIpType_e enIPType,u32 Ratio)
{
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x4B);
    HalUtilityW2BYTEMSK(Reg, Ratio, 0x00FF);
}
void HalSclCmdqSetWaittrig(DrvSclCmdqIpType_e enIPType,u16 trig)
{
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x22);
    HalUtilityW2BYTEMSK(Reg, trig, 0x00FF);
}
void HalSclCmdqSetSkipPollWhenWaitTimeout(DrvSclCmdqIpType_e enIPType,bool ben)
{
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x29);
    if(ben)
        HalUtilityW2BYTEMSKDirect(Reg, 0x0080, 0x0080);
    else
        HalUtilityW2BYTEMSKDirect(Reg, 0x0000, 0x0080);
}
void HalSclCmdqSetTimeoutAmount(DrvSclCmdqIpType_e enIPType)
{
    u32 Reg = 0;
    u32 Reg1 = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x28);
    Reg1 = HalSclCmdqGetRegFromIPID(enIPType ,0x29);
    HalUtilityW2BYTEMSK(Reg, (u16)(CMDQ_BASE_AMOUNT&0xFFFF), 0xFFFF);
    HalUtilityW2BYTEMSK(Reg1, (u16)((CMDQ_BASE_AMOUNT>>16) &0xF), 0x000F);
    HalUtilityW2BYTEMSK(Reg1, (u16)(CMDQ_TIMEOUT_AMOUNT <<8), 0xFF00);

}
void HalSclCmdqClearIrqByFlag(DrvSclCmdqIpType_e enIPType,u16 u16IRQ)
{
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x48);
    HalUtilityW2BYTEMSKDirect(Reg, u16IRQ, 0xFFFF);
    HalUtilityW2BYTEMSKDirect(Reg, 0x0, 0xFFFF);
}
void HalSclCmdqResetSoftInterrupt(DrvSclCmdqIpType_e enIPType)
{
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x48);
    HalUtilityW2BYTEMSKDirect(Reg, 0xFFFF, 0xFFFF);
    HalUtilityW2BYTEMSKDirect(Reg, 0x0, 0xFFFF);
}

void HalSclCmdqSetIsrMsk(DrvSclCmdqIpType_e enIPType,u16 u16Msk)
{
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x47);
    HalUtilityW2BYTEMSK(Reg, u16Msk, 0xFFFF);
}

void HalSclCmdqWriteRegDirect(u32 u32Addr,u16 u16Data)
{
    HalUtilityW2BYTEDirect(u32Addr, u16Data);
}
void HalSclCmdqWriteRegMaskDirect(u32 u32Addr,u16 u16Data,u16 u16Mask)
{
    HalUtilityW2BYTEMSKDirect(u32Addr,u16Data,u16Mask);

}
void HalSclCmdqWriteRegMaskBuffer(u32 u32Addr,u16 u16Data,u16 u16Mask)
{
    if(u32Addr%2)  //Hbyte
    {
        HalUtilityWBYTEMSKBuffer(u32Addr,(u8)u16Data,(u8)u16Mask);
    }
    else
    {
        HalUtilityW2BYTEMSKBuffer(u32Addr,u16Data,u16Mask);
    }

}
u16 HalSclCmdqRead2ByteReg(u32 u32Addr)
{
    u16 u16val;
    u16val = HalUtilityR2BYTE(u32Addr);
    return u16val;
}
u16 HalSclCmdqRead2ByteMaskDirect(u32 u32Addr,u16 u16Mask)
{
    u16 u16val;
    u16val = HalUtilityR2BYTEMaskDirect(u32Addr, u16Mask);
    return u16val;
}
u16 HalSclCmdqGetIspCnt(void)
{
    u16 isp_cnt;
    u32 reg;
    reg=0x1302f4;
    isp_cnt=HalUtilityR2BYTEDirect(reg);
    return isp_cnt;
}
bool HalSclCmdqSetBufferCmdCnt(u32 u32Addr,u16 u16BufferCnt)
{
    bool bNewBufferCnt;
    bNewBufferCnt = HalUtilitySetCmdqBufferIdx(u16BufferCnt,u32Addr);
    return bNewBufferCnt;
}
bool HalSclCmdqGetBufferCmdCnt(u16 *u16BufferCnt, u32 u32Addr)
{
    bool bNewBufferCnt;
    bNewBufferCnt = HalUtilityGetCmdqBufferIdx(u16BufferCnt,u32Addr);
    return bNewBufferCnt;
}
//---------------------------------------------------------------------------
///Trigger for update start pointer and end pointer
//---------------------------------------------------------------------------
void HalSclCmdqStart(DrvSclCmdqIpType_e enIPType,bool bEn)
{
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x3);
    if (bEn)
    {
        HalUtilityW2BYTEMSKDirect(Reg, 0x0002, 0x0002);
    }

}
u32 HalSclCmdqGetMIUReadAddr(DrvSclCmdqIpType_e enIPType)
{
    u32 reg_value=0;
    u32 Reg = 0;
    u32 Reg1 = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x3);
    Reg1 = HalSclCmdqGetRegFromIPID(enIPType ,0xA);
    HalUtilityW2BYTEMSKDirect(Reg,0x0100 , 0x0100);
    reg_value=HalUtilityR4BYTEDirect(Reg1);
    return reg_value;
}

//---------------------------------------------------------------------------
///Set Previous Dummy Register bit to be 1(which means this CAF is already write to DRAM)
//---------------------------------------------------------------------------
u32 HalSclCmdqErrorCommand(DrvSclCmdqIpType_e enIPType,u32 select_bit)
{
    u32 reg_value=0;
    u32 Reg = 0;
    u32 Reg1 = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x43);
    Reg1 = HalSclCmdqGetRegFromIPID(enIPType ,0x40);
    HalUtilityW2BYTEMSKDirect(Reg,select_bit , 0x0007);
    reg_value= HalUtilityR2BYTEDirect(Reg1);
    return reg_value;

}

void HalSclCmdqSetmiusel(DrvSclCmdqIpType_e enIPType, u8 u8MIUSel)
{
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x10);
    if (u8MIUSel == 0)
    {
        HalUtilityW2BYTEMSK(Reg, 0x0000, 0x0002);
    }
    else
    {
        HalUtilityW2BYTEMSK(Reg, 0x0002, 0x0002);
    }
}
#if CMDQ_NONUSE_FUNCTION
/*
u32 Hal_CMDQ_Read_Start_Pointer(void)
{
    u32 reg_value=0;
    reg_value=HalUtilityR4BYTEDirect(REG_CMDQ_04_L);
    return reg_value;
}

u32 Hal_CMDQ_Read_End_Pointer(void)
{
    u32 reg_value=0;
    reg_value=HalUtilityR4BYTEDirect(REG_CMDQ_06_L);
    return reg_value;
}
*/
void HalSclCmdqResetStartPointerBit(DrvSclCmdqIpType_e enIPType)
{
    //for inclement mode
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x3);
    HalUtilityW2BYTEMSK(Reg, 0x0004, 0x0004);
}
u16 HalSclCmdqRetStatus(DrvSclCmdqIpType_e enIPType)
{
    u32 Reg = 0;
    Reg = HalSclCmdqGetRegFromIPID(enIPType ,0x1);
    return ((HalUtilityR2BYTE(Reg) & 0x0001));
}
bool HalSclCmdqGetLdcTirg(void)
{
    u16 trig;
    bool ret;
    trig=(HalUtilityR2BYTEDirect(REG_SCL0_1F_L)&0x0800)>>11;
    ret= trig == 0x1 ? 0 :1;
    return ret;
}

#endif
