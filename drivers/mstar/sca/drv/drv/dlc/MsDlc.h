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
#ifndef __MSDLC_H
#define __MSDLC_H

#ifdef _MSDLC_C_
  #define _MSDLCDEC_
#else
  #define _MSDLCDEC_ extern
#endif
#if ENABLE_DLC
//------------------------------------------------------------------------
// Input:
//      wWidth: Output image Width
//      wHeight: Output iamge Height
//------------------------------------------------------------------------
void msDlcInit(WORD wWidth, WORD wHeight);
void msDlcInitWithCurve( WORD wHStart, WORD wHEnd, WORD wVStart, WORD wVEnd );


//------------------------------------------------------------------------
// Put this function at main loop
// This function will use 33ms for 5xxx/6xxx version chip
// This function will use 22ms for 5xxxA/6xxxA version chip
//------------------------------------------------------------------------
void msDlcHandler(BOOL bWindow);
BOOL msGetHistogramHandler(BOOL bWindow);
//void msDlcStaticCurveOnOff(BOOL bEnable);

void msDlcEnableSetDLCCurveBoth(BOOL bEnable);

//------------------------------------------------------------------------
// Input:
//      bWindow: 0 main window, 1 sub window
//      bSwitch: 0 is OFF, 1 is ON
//------------------------------------------------------------------------
void msDlcOnOff(BOOL bSwitch, BOOL bWindow);
void msDlcSpeedupTrigger(BYTE ucLoop);
void msDlcSetBleOnOff(BOOL bSwitch);

//------------------------------------------------------------------------
// CGC
//------------------------------------------------------------------------
void msDlc_CGC_Init(void);
void msDlc_CGC_ReInit(void);
void msDlc_CGC_SetGain(BYTE u8CGain);
void msDlc_CGC_Handler(void);

//------------------------------------------------------------------------
// For Debug Command
//------------------------------------------------------------------------
void msDLC_ParametersTool(BYTE *pCmdBuff);
//------------------------------------------------------------------------
// LIB version check
//------------------------------------------------------------------------
WORD msDlcLibVersionCheck(void);
WORD msDlcCGCLibVersionCheck(void);
//------------------------------------------------------------------------
//------------------------------------------------------------------------

typedef struct
{
    // Default luma curve
    BYTE ucLumaCurve[16];
    BYTE ucLumaCurve2_a[16];
    BYTE ucLumaCurve2_b[16];
    BYTE ucLumaCurve2[16];

    BYTE u8_L_L_U; // default value: 10
    BYTE u8_L_L_D; // default value: 10
    BYTE u8_L_H_U; // default value: 10
    BYTE u8_L_H_D; // default value: 10
    BYTE u8_S_L_U; // default value: 128 (0x80)
    BYTE u8_S_L_D; // default value: 128 (0x80)
    BYTE u8_S_H_U; // default value: 128 (0x80)
    BYTE u8_S_H_D; // default value: 128 (0x80)
    BYTE ucCGCCGain_offset; // -31 ~ 31 (bit7 = minus, ex. 0x88 => -8)
    BYTE ucCGCChroma_GainLimitH; // 0x00~0x6F
    BYTE ucCGCChroma_GainLimitL; // 0x00~0x10
    BYTE ucCGCYCslope; // 0x01~0x20
    BYTE ucCGCYth; // 0x01
    BYTE ucDlcPureImageMode; // Compare difference of max and min bright
    BYTE ucDlcLevelLimit; // n = 0 ~ 4 => Limit n levels => ex. n=2, limit 2 level 0xF7, 0xE7
    BYTE ucDlcAvgDelta; // n = 0 ~ 50, default value: 12
    BYTE ucDlcAvgDeltaStill; // n = 0 ~ 15 => 0: disable still curve, 1 ~ 15 => 0.1 ~ 1.5 enable still curve
    BYTE ucDlcFastAlphaBlending; // min 17 ~ max 32
    BYTE ucDlcSlowEvent; // some event is triggered, DLC must do slowly // for PIP On/Off, msMultiPic.c
    BYTE ucDlcTimeOut; // for IsrApp.c
    BYTE ucDlcFlickAlphaStart; // for force to do fast DLC in a while
    BYTE ucDlcYAvgThresholdH; // default value: 128
    BYTE ucDlcYAvgThresholdL; // default value: 0
    BYTE ucDlcBLEPoint; // n = 24 ~ 64, default value: 48
    BYTE ucDlcWLEPoint; // n = 24 ~ 64, default value: 48
    BYTE bCGCCGainCtrl : 1; // 1: enable; 0: disable
    BYTE bEnableBLE : 1; // 1: enable; 0: disable
    BYTE bEnableWLE : 1; // 1: enable; 0: disable
    BYTE ucDlcYAvgThresholdM;
    BYTE ucDlcCurveMode;
    BYTE ucDlcCurveModeMixAlpha;
    BYTE ucDlcAlgorithmMode;
    BYTE ucDlcHistogramLimitCurve[17];
    BYTE ucDlcSepPointH;
    BYTE ucDlcSepPointL;
    WORD uwDlcBleStartPointTH;
    WORD uwDlcBleEndPointTH;
    BYTE ucDlcCurveDiff_L_TH;
    BYTE ucDlcCurveDiff_H_TH;
    WORD uwDlcBLESlopPoint_1;
    WORD uwDlcBLESlopPoint_2;
    WORD uwDlcBLESlopPoint_3;
    WORD uwDlcBLESlopPoint_4;
    WORD uwDlcBLESlopPoint_5;
    WORD uwDlcDark_BLE_Slop_Min;
    BYTE ucDlcCurveDiffCoringTH;
    BYTE ucDlcAlphaBlendingMin;
    BYTE ucDlcAlphaBlendingMax;
    BYTE ucDlcFlicker_alpha;
    BYTE ucDlcYAVG_L_TH;
    BYTE ucDlcYAVG_H_TH;
    BYTE ucDlcDiffBase_L;
    BYTE ucDlcDiffBase_M;
    BYTE ucDlcDiffBase_H;
}StuDlc_FinetuneParamaters;

#if defined(MSOS_TYPE_CE)

 #if defined(WITHIN_MHAL_DLC_DLL)
    extern XDATA StuDlc_FinetuneParamaters g_DlcParameters;
    extern XDATA BYTE g_ucHistogramMax, g_ucHistogramMin;
    extern XDATA WORD g_wLumiAverageTemp;
    extern XDATA BYTE g_ucTable[16];
    extern XDATA WORD g_wLumaHistogram32H[32];

 #else
    __declspec(dllimport)  XDATA StuDlc_FinetuneParamaters g_DlcParameters;
    __declspec(dllimport)  XDATA BYTE g_ucHistogramMax, g_ucHistogramMin;
    __declspec(dllimport)  XDATA WORD g_wLumiAverageTemp;
    __declspec(dllimport)  XDATA WORD g_wLumiTotalCount;
    __declspec(dllimport)  XDATA BYTE g_ucTable[16];
    __declspec(dllimport)  XDATA WORD g_wLumaHistogram32H[32];

 #endif
#else

extern XDATA StuDlc_FinetuneParamaters g_DlcParameters;
extern XDATA BYTE g_ucHistogramMax, g_ucHistogramMin;
extern XDATA WORD g_wLumiAverageTemp;
extern XDATA WORD g_wLumiTotalCount;
extern XDATA BYTE g_ucTable[16];
extern XDATA WORD g_wLumaHistogram32H[32];

#endif

#define DLC_PURE_IMAGE_DLC_CURVE        0L
#define DLC_PURE_IMAGE_LINEAR_CURVE     1L
#define DLC_PURE_IMAGE_DO_NOTHING       2L


extern XDATA char* g_pInitLumaCurve;

#define DLC_EVENT_SLOW_ENABLE   40

#if (ENABLE_DLC == DLC_8SEG_HIS_MODE)
extern XDATA WORD g_wLumaHistogram8H[8];
#endif

void msDlc_SetCaptureRange(MS_U16 wHStart,MS_U16 wHEnd, MS_U16 wVStart, MS_U16 wVEnd);



#endif
#endif
