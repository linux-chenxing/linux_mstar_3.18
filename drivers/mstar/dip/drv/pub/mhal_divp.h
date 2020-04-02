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
#include "mhal_divp_datatype.h"

//------------------------------------------------------------------------------
/// @brief Init DIVP Hardware
/// @param[in] eDevId: DIVP device ID.
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_Init(MHAL_DIVP_DeviceId_e eDevId);

//------------------------------------------------------------------------------
/// @brief deInit DIVP Hardware
/// @param[in] eDevId: DIVP device ID.
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_DeInit(MHAL_DIVP_DeviceId_e eDevId);

//------------------------------------------------------------------------------
/// @brief get capbility of DIVP Hardware
/// @param[in] eDevId: DIVP device ID.
/// @param[out] pu32Caps: return DIVP capbility.
/// DIVP_Dev_NOT_EXIST = 0x0,
/// DIVP_Dev_EXIST =     0x1,
/// DIVP_Dev_420TILE =   0x2,
/// DIVP_Dev_SCALING_DOWN =   0x4,
/// DIVP_Dev_MIRROR =    0x8,
/// DIVP_Dev_DIPR    =   0x10,
/// DIVP_Dev_R2Y     =   0x20,
/// DIVP_Dev_OP1_CAPTURE    = 0x40,
/// DIVP_Dev_OP1_CAPTURE_V1 = 0x40,//not support OP1 capture when traveling main source with PIP mode
/// DIVP_Dev_OP1_CAPTURE_V2 = 0x80,//support OP1 capture in all case
/// DIVP_Dev_SCALING_UP = 0x100,
/// DIVP_Dev_OSD_BLENDING = 0x200,
/// DIVP_Dev_OC_CAPTURE = 0x400,
/// DIVP_Dev_SC2_NON_SCALING = 0x800,
/// DIVP_Dev_SECURE = 0x1000,
/// DIVP_Dev_3DDI = 0x2000,
/// DIVP_Dev_MED_FIELD = 0x4000,
/// DIVP_Dev_MED_FRAME = 0x8000,
/// DIVP_Dev_MFDEC = 0x10000,
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_GetChipCapbility(MHAL_DIVP_DeviceId_e eDevId, MS_U32* pu32Caps);

//------------------------------------------------------------------------------
/// @brief Open Video handler
/// @param[in] eDevId: DIVP device ID.
/// @param[in] pstAlloc: function pointer to allocate memory.
/// @param[in] pfFree: function pointer to free memory.
/// @param[in] u16MaxWidth: max width channel supportrd.
/// @param[in] u16MaxHeight: max height channel supportrd.
/// @param[out] pCtx: DIVP channel contex in hal and down layer.
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_CreateInstance(MHAL_DIVP_DeviceId_e eDevId, MS_U16 u16MaxWidth, MS_U16 u16MaxHeight, PfnAlloc pfAlloc, PfnFree pfFree, MS_U8 u8ChannelId, void** ppCtx);

//------------------------------------------------------------------------------
/// @brief destroy DIVP channel inistance
/// @param[in] pCtx: DIVP channel contex in hal and down layer.
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_DestroyInstance(void* pCtx);

//------------------------------------------------------------------------------
/// @attention: Disable TNR, 3D DI and crop when capture a picture.
/// @brief capture a picture from display VOP/OP2.
/// @param[in] pstCaptureInfo: config information of the picture captured by DIVP.
/// @param[in] pstCmdQInfo: a group of function pointer for controlling HW register. These functions
///    will deside write/read regster with RIU or Command queue.
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_CaptureTiming(void* pCtx, MHAL_DIVP_CaptureInfo_t* pstCaptureInfo, MHAL_CMDQ_CmdqInterface_t* pstCmdQInfo);

//------------------------------------------------------------------------------
/// @brief DIVP deal with a frame and than write to dram. The frame will be used by VPE...
/// @param[in] pCtx: DIVP channel contex in hal and down layer.
/// @param[in] pstDivpInputInfo: config information of the input frame.
/// @param[in] pstDivpOutputInfo: config information of the output frame.
/// @param[in] pstCmdQInfo: a group of function pointer for controlling HW register. These functions
///    will deside write/read regster with RIU or Command queue.
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_ProcessDramData(void* pCtx, MHAL_DIVP_InputInfo_t* pstDivpInputInfo,
                                 MHAL_DIVP_OutPutInfo_t* pstDivpOutputInfo, MHAL_CMDQ_CmdqInterface_t* pstCmdQInfo);


//------------------------------------------------------------------------------
/// @brief set attribute of DIVP channel.
/// @param[in] pCtx: DIVP channel contex in hal and down layer.
/// @param[in] eAttrType: DIVP channel attribute type.
/// @param[in] pAttrParam: config information of DIVP channel.
///    if(E_MHAL_DIVP_ATTR_TNR == eAttrType) ==> pAttr is MHAL_DIVP_TnrLevel_e*.
///    if(E_MHAL_DIVP_ATTR_DI == eAttrType) ==> pAttr is MHAL_DIVP_DiType_e*.
///    if(E_MHAL_DIVP_ATTR_ROTATE == eAttrType) ==> pAttr is MHAL_DIVP_Rotate_e*.
///    if(E_MHAL_DIVP_ATTR_CROP == eAttrType) ==> pAttr is MHAL_DIVP_Window_t*.
///    if(E_MHAL_DIVP_ATTR_MIRROR == eAttrType) ==> pAttr is MHAL_DIVP_Mirror_t*.
/// @param[in] pstCmdQInfo: a group of function pointer for controlling HW register. These functions
///    will deside write/read regster with RIU or Command queue.
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_SetAttr(void* pCtx, MHAL_DIVP_AttrType_e eAttrType, const void* pAttrParam,
                         MHAL_CMDQ_CmdqInterface_t* pstCmdQInfo);

//------------------------------------------------------------------------------
/// @brief DIVP deal with a frame and than write to dram. The frame will be used by DISP.
/// @param[in] bEnable: control DIVP module's ISR of frame done. TURE: accept interrupt,
///    FALSE:ignore frame done ISR.
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_EnableFrameDoneIsr(MS_BOOL bEnable);

//------------------------------------------------------------------------------
/// @brief reset frame done interrupt..
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_CleanFrameDoneIsr(void);

//------------------------------------------------------------------------------
/// @brief wait cmdq frame done..
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_WaitCmdQFrameDone(MHAL_CMDQ_CmdqInterface_t* pstCmdQInfo, MHAL_CMDQ_EventId_e eEventId);

//------------------------------------------------------------------------------
/// @brief get interrupt ID.
/// @param[out] pu32IrqNum: interrupt ID,
/// @return MHAL_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_DIVP_GetIrqNum(MS_U32 *pu32IrqNum);
