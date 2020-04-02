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
// Copyright (c) 2006-20012 MStar Semiconductor, Inc.
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
// file   halCMDQ.h
// @brief  CMDQ HAL
// @author MStar Semiconductor,Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __HAL_SCL_CMDQ_H__
#define __HAL_SCL_CMDQ_H__



// Trigger Bus inverse mask
#define ISP2CMDQ_FRAME_DONE         0xFFFE  // 0
#define ISP2CMDQ_VS_R               0xFFFD  // 1
#define SC2CMDQ_LV_TRIG             0xFFFB  // 2
#define HAL_SCLCMDQ_DMA_FINISHED    0xFFF7  // 3


#define HAL_SCLCMDQ_CMD_LEN             8                     // 8byte =64bit
#define HAL_SCLCMDQ_MEM_BASE_UNIT       16                    // MIU 128bit
#define HAL_SCLCMDQ_CMD_ALIGN           2                     // (cmd/membase)
#define HAL_SCLCMDQ_END_CMD             0x100802010000FFFE    // nonuse
#define HAL_SCLCMDQ_NULL_CMD            0x000000000000FFFF    // null cmd
#define HAL_SCLCMDQ_NULLTEST_CMD        0x00000000FFFFFFFF    // null cmd
#define HAL_SCLCMDQ_WRITE_CMD           0x1000000000000000    // write base head
#define HAL_SCLCMDQ_ALLNULL_CMD         0x0000000000000000    // clean
#define HAL_SCLCMDQ_WAIT_CMD            0x2000000000000000    // wait base head
#define HAL_SCLCMDQ_POLL_EQ_CMD         0x3000000000000000    // polleq base head
#define HAL_SCLCMDQ_POLL_NEQ_CMD        0xB000000000000000    // pollneq base head
#define HAL_SCLCMDQ_CRASH_15_0_BIT      0x0000   //0x43 bit:3 command[15:0]
#define HAL_SCLCMDQ_CRASH_31_16_BIT     0x0001   //0x43 bit:3 command[31:16]
#define HAL_SCLCMDQ_CRASH_55_32_BIT     0x0002   //0x43 bit:3 command[55:32]
#define HAL_SCLCMDQ_CRASH_63_56_BIT     0x0003   //0x43 bit:3 command[63:56]

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//  Macro of bit operations
//--------------------------------------------------------------------------------------------------

////////////////////////////////////////////////
// HAL
////////////////////////////////////////////////

void HalSclCmdqSetBaseEn(DrvSclCmdqIpType_e enIPType,bool bEn);
void HalSclCmdqReset(DrvSclCmdqIpType_e enIPType,bool bEn);
void HalSclCmdqSetTrigAlways(DrvSclCmdqIpType_e enIPType,bool bEn);
bool HalSclCmdqSetDMAMode(DrvSclCmdqIpType_e enIPType,int ModeSel);
void HalSclCmdqSetStartPointer(DrvSclCmdqIpType_e enIPType,u32 StartAddr);
void HalSclCmdqSetEndPointer(DrvSclCmdqIpType_e enIPType,u32 EndAddr);
void HalSclCmdqSetOffsetPointer(DrvSclCmdqIpType_e enIPType,u32 OffsetAddr);
void HalSclCmdqSetTimer(DrvSclCmdqIpType_e enIPType,u32 time);
void HalSclCmdqSetRatio(DrvSclCmdqIpType_e enIPType,u32 Ratio);
void HalSclCmdqStart(DrvSclCmdqIpType_e enIPType,bool bEn);
u32  HalSclCmdqErrorCommand(DrvSclCmdqIpType_e enIPType,u32 select_bit);
void HalSclCmdqSetmiusel(DrvSclCmdqIpType_e enIPType, u8 u8MIUSel);
void HalSclCmdqInitRiuBase(u32 u32PM_riu_base);
u16 HalSclCmdqGetFinalIrq(DrvSclCmdqIpType_e enIPType ,u16 u16Mask);
u16 HalSclCmdqGetRawIrq(DrvSclCmdqIpType_e enIPType ,u16 u16Mask);
void HalSclCmdqEnable(DrvSclCmdqIpType_e enIPType,bool bEn);
u32  HalSclCmdqGetMIUReadAddr(DrvSclCmdqIpType_e enIPType);
void HalSclCmdqSetWaittrig(DrvSclCmdqIpType_e enIPType,u16 trig);
void HalSclCmdqSetSkipPollWhenWaitTimeout(DrvSclCmdqIpType_e enIPType,bool ben);
void HalSclCmdqSetTimeoutAmount(DrvSclCmdqIpType_e enIPType);
void HalSclCmdqWriteRegMaskBuffer(u32 u32Addr,u16 u16Data,u16 u16Mask);
void HalSclCmdqWriteRegDirect(u32 u32Addr,u16 u16Data);
void HalSclCmdqWriteRegMaskDirect(u32 u32Addr,u16 u16Data,u16 u16Mask);
u16  HalSclCmdqRead2ByteReg(u32 u32Addr);
u16  HalSclCmdqRead2ByteMaskDirect(u32 u32Addr,u16 u16Mask);
u16  HalSclCmdqGetIspCnt(void);
bool HalSclCmdqGetBufferCmdCnt(u16 *u16BufferCnt, u32 u32Addr);
bool HalSclCmdqSetBufferCmdCnt(u32 u32Addr,u16 u16BufferCnt);
void HalSclCmdqResetSoftInterrupt(DrvSclCmdqIpType_e enIPType);
void HalSclCmdqClearIrqByFlag(DrvSclCmdqIpType_e enIPType,u16 u16IRQ);
void HalSclCmdqResetStartPointerBit(DrvSclCmdqIpType_e enIPType);
u16  HalSclCmdqRetStatus(DrvSclCmdqIpType_e enIPType);
bool HalSclCmdqGetLdcTirg(void);
void HalSclCmdqSetIsrMsk(DrvSclCmdqIpType_e enIPType,u16 u16Msk);


#endif // #ifndef _CMDQ_REG_MCU_H_
