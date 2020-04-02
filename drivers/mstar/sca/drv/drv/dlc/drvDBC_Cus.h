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
#ifndef DRV_DBC_CUSH
#define DRV_DBC_CUSH

#ifdef __cplusplus
extern "C"
{
#endif
//-----------------------------------------------------------------------
// due to patent issues, temporary DISABLE it
//-----------------------------------------------------------------------
#define ENABLE_DBC FALSE

//------------------------------------------------------------------------
// DCR library start
//------------------------------------------------------------------------

typedef struct
{
    MS_U8 ucMaxPWM;
    MS_U8 ucMinPWM;
    MS_U8 ucMax_Video;
    MS_U8 ucMid_Video;
    MS_U8 ucMin_Video;
    MS_U8 ucCurrentPWM;
    MS_U8 ucAlpha;
    MS_U8 ucBackLight_Thres;
    MS_U8 ucAvgDelta;
    MS_U8 ucFlickAlpha;
    MS_U8 ucFastAlphaBlending; // min 17 ~ max 32
    MS_U8 ucLoop_Dly;
    MS_U8 ucLoop_Dly_H_Init;
    MS_U8 ucLoop_Dly_MH_Init;
    MS_U8 ucLoop_Dly_ML_Init;
    MS_U8 ucLoop_Dly_L_Init;
    MS_U8 ucY_Gain_H;
    MS_U8 ucC_Gain_H;
    MS_U8 ucY_Gain_M;
    MS_U8 ucC_Gain_M;
    MS_U8 ucY_Gain_L;
    MS_U8 ucC_Gain_L;
    MS_U8 bYGainCtrl : 1; // 1: enable; 0: disable
    MS_U8 bCGainCtrl : 1; // 1: enable; 0: disable
}StuDbc_FinetuneParamaters_Lib;

//------------------------------------------------------------------------
// For Debug Command
//------------------------------------------------------------------------
typedef void (*MDrv_DLC_DBC_OnOff_Callback)          (MS_BOOL/*On/Off*/);
typedef void (*MDrv_DLC_DBC_AdjustBackLight_Callback)(MS_U32 /*PWM_VALUE*/);
typedef void (*MDrv_DBC_Print_Callback)           (MS_U8 /*PWM_VALUE*/);

#if (ENABLE_DBC )
void    msDBCInit(void);
MS_U16  msBackLightHandler(void);
MS_U16  msDbcLibVersionCheck(void);
void    msAdjustYCGain(MS_U8 u8YGain,MS_U8 u8CGain);
void    msDBC_ParametersTool(MS_U8 *pCmdBuff, MDrv_DLC_DBC_OnOff_Callback fnDBC_AP_OnOff, MDrv_DLC_DBC_AdjustBackLight_Callback fnDBC_AdjustBacklight, MDrv_DBC_Print_Callback putchar );
#else
#define msDBCInit()
#define msBackLightHandler()  0
#define  msDbcLibVersionCheck() 0
#define  msAdjustYCGain(x, y)
#define  msDBC_ParametersTool(a, b, c, d)
#endif

//------------------------------------------------------------------------
// Utopia custom functions
//------------------------------------------------------------------------

MS_U8   MDrv_DLC_DBC_GetDebugMode(void);
void    MDrv_DLC_DBC_SetDebugMode(MS_U8 u8DBC_DebugMode);
void    MDrv_DLC_DBC_UpdatePWM(MS_U8 u8PWMvalue);
void    MDrv_DLC_DBC_Init(StuDbc_FinetuneParamaters_Lib DLC_DBC_MFinit);
MS_U16  MDrv_DLC_DBC_Handler(void);
MS_U16  MDrv_DLC_DBC_GetMFVer(void);
MS_BOOL MDrv_DLC_GetDBCInfo(StuDbc_FinetuneParamaters_Lib *pDLCDBCData, MS_U32 u32DBCDataLen);
void    MDrv_DLC_DBC_AdjustYCGain(void);
void    MDrv_DLC_DBC_YCGainInit(MS_U8 u8YGain_M, MS_U8 u8YGain_L, MS_U8 u8CGain_M, MS_U8 u8CGain_L, MS_U8 u8YGain_H, MS_U8 u8CGain_H);

#ifdef __cplusplus
}
#endif

#endif
