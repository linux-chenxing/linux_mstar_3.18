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
// (!¡±MStar Confidential Information!¡L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
#ifndef __DRV_SCL_HVSP_H__
#define __DRV_SCL_HVSP_H__


//-------------------------------------------------------------------------------------------------
//  Defines & Structure
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------
#ifndef DRV_SCL_HVSP_C
#define INTERFACE extern
#else
#define INTERFACE
#endif
#define HVSP_irq                    0
INTERFACE void DrvSclHvspSetMemoryAllocateReady(bool bEn);
INTERFACE bool DrvSclHvspInit(DrvSclHvspInitConfig_t *pInitCfg);
INTERFACE void DrvSclHvspPriMaskBufferReset(void);
INTERFACE void DrvSclHvspExit(u8 bCloseISR);
INTERFACE void DrvSclHvspPriMaskBufferReset(void);
INTERFACE void DrvSclHvspSetBufferNum(u8 u8Num);
INTERFACE bool DrvSclHvspGetBufferNum(void);
INTERFACE bool DrvSclHvspSetIPMConfig(DrvSclHvspIpmConfig_t stCfg);
INTERFACE void DrvSclHvspSetFbManageConfig(DrvSclHvspSetFbManageConfig_t stCfg);
INTERFACE DrvSclHvspScalingConfig_t DrvSclHvspSetScaling(DrvSclHvspIdType_e enHVSP_ID, DrvSclHvspScalingConfig_t stCfg, DrvSclHvspClkConfig_t* stclk);
INTERFACE bool DrvSclHvspSetInputMux(DrvSclHvspIpMuxType_e enIP,DrvSclHvspClkConfig_t* stclk);
INTERFACE u32 DrvSclHvspGetInputSrcMux(void);
INTERFACE bool DrvSclHvspSetRegisterForce(u32 u32Reg, u8 u8Val, u8 u8Msk);
INTERFACE void DrvSclHvspSetSCIQHSRAM(DrvSclHvspIdType_e enHVSP_ID);
INTERFACE void DrvSclHvspSetSCIQVSRAM(DrvSclHvspIdType_e enHVSP_ID);
INTERFACE bool DrvSclHvspSetLDCFrameBuffer_Config(DrvSclHvspLdcFrameBufferConfig_t stLDCCfg);
INTERFACE bool DrvSclHvspSetPatTgen(bool bEn, DrvSclHvspPatTgenConfig_t *pCfg);
INTERFACE bool DrvSclHvspGetSCLInform(DrvSclHvspIdType_e enID,DrvSclHvspScInformConfig_t *stInformCfg);
INTERFACE void DrvSclHvspSclIq(DrvSclHvspIdType_e enID,DrvSclHvspIqType_e enIQ);
INTERFACE void DrvSclHvspSetPrv2CropOnOff(u8 bEn);
INTERFACE DrvSclHvspScIntsType_t DrvSclHvspGetSclInts(void);
INTERFACE void DrvSclHvspSetOsdConfig(DrvSclHvspIdType_e enID, DrvSclHvspOsdConfig_t stOSdCfg);
INTERFACE void DrvSclHvspSetPriMaskConfig(DrvSclHvspPriMaskConfig_t stCfg);
INTERFACE u16 DrvSclHvspPriMaskGetSram(u8 u8idx, u8 u8idy);
INTERFACE void DrvSclHvspSetPriMaskTrigger(DrvSclHvspPriMaskType_e enType);
INTERFACE void DrvSclHvspIdclkRelease(DrvSclHvspClkConfig_t* stclk);
INTERFACE u8 DrvSclHvspCheckInputVsync(void);
INTERFACE void DrvSclHvspGetCrop12Inform(DrvSclHvspInputInformConfig_t *stInformCfg);
INTERFACE bool DrvSclHvspGetFrameBufferAttribute(DrvSclHvspIdType_e enID,DrvSclHvspIpmConfig_t *stInformCfg);
INTERFACE bool DrvSclHvspGetHvspAttribute(DrvSclHvspIdType_e enID,DrvSclHvspInformConfig_t *stInformCfg);
INTERFACE void DrvSclHvspGetOsdAttribute(DrvSclHvspIdType_e enID,DrvSclHvspOsdConfig_t *stOsdCfg);
INTERFACE bool DrvSclHvspSuspend(DrvSclHvspSuspendResumeConfig_t *pCfg);
INTERFACE bool DrvSclHvspResume(DrvSclHvspSuspendResumeConfig_t *pCfg);
INTERFACE void DrvSclHvspRelease(DrvSclHvspIdType_e HVSP_IP);
INTERFACE void DrvSclHvspOpen(DrvSclHvspIdType_e HVSP_IP);
INTERFACE bool DrvSclHvspSetCmdqTrigType(DrvSclHvspCmdTrigConfig_t stCmdTrigCfg);
INTERFACE DrvSclHvspCmdTrigConfig_t DrvSclHvspSetCmdqTrigTypeByRIU(void);
INTERFACE void DrvSclHvspSetClkForcemode(u8 bEn);
INTERFACE bool DrvSclHvspGetClkForcemode(void);
INTERFACE void DrvSclHvspSetClkRate(u8 u8Idx);
INTERFACE u32 DrvSclHvspCmdqStatusReport(void);
INTERFACE void DrvSclHvspSetLdcONOFF(bool bEn);
INTERFACE void * DrvSclHvspGetWaitQueueHead(void);
INTERFACE bool DrvSclHvspGetCmdqDoneStatus(DrvSclHvspPollIdType_e enPollId);
INTERFACE void DrvSclHvspSetCropWindowSize(void);
INTERFACE void DrvSclHvspSetInputSrcSize(u16 u16Height,u16 u16Width);

#undef INTERFACE

#endif
