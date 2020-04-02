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
/******************************************************************************
 Copyright (c) 2008 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: MsDLC.c
 [Date]: 2009-03-20
 [Comment]:
   MST DLC subroutines.
 [Reversion History]:
*******************************************************************************/

#define _MSDLC_C_

#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/wait.h>
#include <linux/irqreturn.h>
#include <asm/div64.h>
#endif

#include "MsDlc_LIB.h"
#include "MsDlc.h"

// Compiler option
#define DLC_LIB_VERSION             0x3299 //v32.61 // fix bugs of u8_L_H_U & u8_L_H_D
                                           //v32.59 // 1. Add guard condition to msDlcOnOff & msDlc_CGC_Init. msDlcOnOff & msDlc_CGC_Init will check DLC On/Off status before execution.
                                           //v32.58 // 1. change register data width from WORD -> DWORD 2. Fixs float point issue.
                                           //v32.57 // fix bug imgae flick while curve change, fix bug for g_DlcParameters.ucDlcAvgDeltaStill & g_DlcParameters.ucDlcAvgDelta
                                           //v32.56 // fix bug for g_DlcParameters.ucDlcAvgDeltaStill
                                           //v32.55 // change the resolution of Yavg for g_DlcParameters.ucDlcAvgDeltaStill
                                           //v32.54 // add C Gain (CGC) On Off
                                           //v32.53 // modify msDlc_AP_ReinitWithoutPara() for debug command
                                           //v32.52 // Include DebugTool Command + move g_u8Yth to CGC parameter groups
                                           //v32.51 // Modify CGC initial flow
                                           //v32.50 // Fix CGC Limit definition
                                           //v32.49 // add Cgain control by Jason.Chen, add protect when AvgMax < Avg Min for pure image
                                           //v32.48 // reserved for Paulo/Lomeo version number.
                                           //v32.47 // fix Yavg threshold problem
                                           //v32.46 // rename parameters, add Yavg threshold, g_DlcParameters.ucDlcYAvgThresholdH & g_DlcParameters.ucDlcYAvgThresholdL
                                           //v32.45 // fix level[14] over flow issue & unstable when LH2(g_DlcParameters.u8_L_H_D) is high
                                           //v32.44 // finetune v32.43
                                           //v32.43 // (test) add new parameter for stop curve cgange when Avg Delta <= g_DlcParameters.ucDlcAvgDeltaStill
                                           //v32.42 // remove Delay1ms() and replace to a Macro msDlc_Delay1ms
                                           //v32.41 // change usage of timeout event, use Delay1ms(n);
                                           //v32.40 // add new parameter for WLE/BLE g_DlcParameters.ucDlcBLEPoint, g_DlcParameters.ucDlcWLEPoint, add new parameter for Avg offset (Compal), add timeout in initial stage
                                           //v32.39 // modify for get stable pixel count "g_wLumiTotalCount"
                                           //v32.38 // fix re-init error in Histogram request bit (bit2)
                                           //v32.37 // seperate code msDlcLumiDiffCtrl()
                                           //v32.36 // add more overflow/underflow protection
                                           //v32.35 // to fix the bug => luma curve (0) is almost  "0x00" for all kind of pattern.
                                           //v32.34 // add compiler option ENABLE_HISTOGRAM_8 and function msDlcSetVarHistogramRange()
                                           //v32.33 // add histogram range of H
                                           //v32.32 // add registers initial table
                                           //v32.31 // add some definition & add function enter and exit
                                           //v32.30 // modify for ATV/DTV are compatible
                                           //v32.23 // change definition of g_wAvgDelta =>  Delta = n * 3 , add function msGetAverageLuminous()
                                           //v32.22 // add g_DlcParameters.ucDlcFastAlphaBlending, update g_wAvgDelta
                                           //v32.21 // add g_wAvgDelta
                                           //v32.20 // add level limit
                                           //v32.19 // modify de-flick function, add parameter for pure image "g_DlcParameters.ucDlcPureImageMode", remove msGetHistogramHandler from DLC handler, finetune the process of pure image
                                           //v32.18 // modify de-flick function (g_ucUpdateCnt & g_ucDlcFlickAlpha)
                                           //v32.17 // fix bug of de-flick function
                                           //v32.16 // modify de-flick function
                                           //v32.15 // modify alpha belnding & add g_DlcParameters.ucDlcFlickAlphaStart after mode change
                                           //v32.14 // Remove garbage (L_L_U...)
                                           //v32.13 // Protect Histogram Overflow
                                           //v32.12 // modify for tool finetune
                                           //v32.11 // modify init static curve & for tool finetune
                                           //v32.10 // Lopez & Raphael series 32-seg DLC & version 10

#define DLC_CGC_LIB_VERSION         0x0104 //v1.04 // add C Gain (CGC) On Off
                                           //v1.03 // move g_u8Yth to CGC parameter groups
                                           //v1.02 // Modify CGC initial flow
                                           //v1.01 // Fix CGC Limit definition
                                           //v1.00 // Initial



#if defined(MSOS_TYPE_CE)

#pragma data_seg(".DLCHal")
    XDATA StuDlc_FinetuneParamaters g_DlcParameters ={0};
    XDATA WORD g_uwPreTable[16]= {0}; // New de-Flick
    XDATA BYTE g_ucHistogramMax = 0;
    XDATA BYTE g_ucHistogramMin = 0;
    XDATA WORD g_wLumiAverageTemp = 0;
    XDATA WORD g_wLumiTotalCount = 0;
    XDATA BYTE g_ucTable[16] ={0};
    XDATA WORD g_wLumaHistogram32H[32] ={0};

#pragma data_seg()
#pragma comment(linker,"/SECTION:.DLCHal,RWS")

#else

XDATA StuDlc_FinetuneParamaters g_DlcParameters;
XDATA WORD g_uwPreTable[16]; // New de-Flick
XDATA BYTE g_ucHistogramMax, g_ucHistogramMin;
XDATA WORD g_wLumiAverageTemp;
XDATA WORD g_wLumiTotalCount;
XDATA BYTE g_ucTable[16];
XDATA WORD g_wLumaHistogram32H[32];

#endif

#define DLC_DEBUG(x)        //(x)

#define DEFAULT_BLE_UPPER_BOND      0x08
#define DEFAULT_BLE_LOWER_BOND      0x04

#define DLC_DEFLICK_PRECISION_SHIFT   64UL
#define DLC_DEFLICK_BLEND_FACTOR      32UL

#define DLC_PURE_IMAGE_DLC_CURVE        0L
#define DLC_PURE_IMAGE_LINEAR_CURVE     1L
#define DLC_PURE_IMAGE_DO_NOTHING       2L

#define XC_DLC_ALGORITHM_OLD       0
#define XC_DLC_ALGORITHM_NEW       1
#define XC_DLC_ALGORITHM_KERNEL    2

#define XC_DLC_SET_DLC_CURVE_BOTH_SAME    0   // 0:Main and Sub use the same DLC curve

#if (ENABLE_HISTOGRAM_8)
XDATA WORD g_wLumaHistogram8H[8];
#endif
XDATA BYTE g_ucHistogramTotal_H;
XDATA BYTE g_ucHistogramTotal_L;
XDATA BYTE g_ucDlcFlickAlpha;
XDATA BYTE g_pre_ucTable[16]; // Final target curve (8 bit)
XDATA WORD g_uwTable[16];     // Final target curve (10 bit)
XDATA BYTE g_ucCurveYAvg[16];
XDATA DWORD g_uwCurveHistogram[16];
XDATA WORD g_uwPre_CurveHistogram[16];
XDATA BYTE g_ucUpdateCnt;
XDATA WORD g_wLumiTotalCount;
XDATA BYTE g_ucTmpAvgN_1; // Avg value (N-1)
XDATA BYTE g_ucTmpAvgN; // Avg value (current N)
XDATA WORD g_wTmpAvgN_1_x10; // Avg value (N-1) x 10
XDATA WORD g_wTmpAvgN_x10; // Avg value (current N) x 10
XDATA BYTE g_ucDlcFastLoop; // for pulse ripple

#define FLICK_ALPHA_START_COUNTER   40

XDATA DWORD g_dwFactory;
XDATA DWORD g_dwFactory_7;
XDATA DWORD g_dwFactory_9;
XDATA DWORD g_dwLstep;
XDATA DWORD g_dwHstep;

XDATA BYTE s_Slope[8] = {0x17,0x27,0x47,0x67,0x97,0xB7,0xD7,0xE7};

static MS_BOOL  g_bSetDlcBleOn = TRUE;

#define LA1 g_DlcParameters.u8_L_L_U
#define LA2 g_DlcParameters.u8_L_L_D
#define LB1 g_DlcParameters.u8_L_L_U
#define LB2 g_DlcParameters.u8_L_L_D
#define LC1 g_DlcParameters.u8_L_L_U
#define LC2 g_DlcParameters.u8_L_L_D
#define LD1 g_DlcParameters.u8_L_L_U
#define LD2 g_DlcParameters.u8_L_L_D
#define LE1 g_DlcParameters.u8_L_H_U
#define LE2 g_DlcParameters.u8_L_H_D
#define LF1 g_DlcParameters.u8_L_H_U
#define LF2 g_DlcParameters.u8_L_H_D
#define LG1 g_DlcParameters.u8_L_H_U
#define LG2 g_DlcParameters.u8_L_H_D
#define LH1 g_DlcParameters.u8_L_H_U
#define LH2 g_DlcParameters.u8_L_H_D

#define SA1 g_DlcParameters.u8_S_L_U
#define SA2 g_DlcParameters.u8_S_L_D
#define SB1 g_DlcParameters.u8_S_L_U
#define SB2 g_DlcParameters.u8_S_L_D
#define SC1 g_DlcParameters.u8_S_L_U
#define SC2 g_DlcParameters.u8_S_L_D
#define SD1 g_DlcParameters.u8_S_L_U
#define SD2 g_DlcParameters.u8_S_L_D
#define SE1 g_DlcParameters.u8_S_H_U
#define SE2 g_DlcParameters.u8_S_H_D
#define SF1 g_DlcParameters.u8_S_H_U
#define SF2 g_DlcParameters.u8_S_H_D
#define SG1 g_DlcParameters.u8_S_H_U
#define SG2 g_DlcParameters.u8_S_H_D
#define SH1 g_DlcParameters.u8_S_H_U
#define SH2 g_DlcParameters.u8_S_H_D

typedef enum
{
    Slope_1,
    Slope_2,
    Slope_4,
    Slope_6,
    Slope_9,
    Slope_11,
    Slope_13,
    Slope_14,
    Slope_Max
} Slop_Type;

#define Lstep           g_dwLstep
#define Hstep           g_dwHstep

//=========================================================

#define TOTAL_PIXEL     ((DWORD)(wVEnd-wVStart)*(wHEnd-wHStart)*7*4/(160*3))  // ((DWORD)(wVEnd-wVStart)*0.04375*4/3*(wHEnd-wHStart))

void msWriteDLCRegsTbl(DlcRegUnitType code *pTable);
void msDlcWriteCurve(BOOL bWindow);
void msDlcWriteCurveLSB(BOOL bWindow, BYTE ucIndex, BYTE ucValue);
void msDlcLumiDiffCtrl(void);
void msDlcSetVarHistogramRange(BYTE *pRangeArray);

/******************************************************************************/
/*                           CGC                                         */
/******************************************************************************/
// Constant
#define Yvar_Th       4                         // Change threshold for DLC luma curve
#define Timer_CountU   3                   // Increasing Chroma Gain change speed
#define Timer_CountD   1                   // Decreasing Chroma Gain change speed
#define CGain_StepU     1                   // Increasing Chroma Gain step
#define CGain_StepD     1                   // Decreasing Chroma Gain step

#define Y01_weight    5                      // Luma Curve Y[0], Y[1] weighting
#define Y23_weight    3                      // Luma Curve Y[2], Y[3] weighting
#define Y45_weight    1                      // Luma Curve Y[4], Y[5] weighting
#define Y67_weight    1                      // Luma Curve Y[6], Y[7] weighting

// Local
XDATA BYTE g_u8Chroma_OriGain = 0x40;
XDATA BYTE g_u8Chroma_target;
XDATA WORD g_u16Ydark, g_u16Ydark_pre;
XDATA BYTE g_u8TCount = 0;
XDATA BYTE g_u8CGC_UpperBond, g_u8CGC_LowerBond;
XDATA BOOL g_bSetDLCCurveBoth = 0;//We will set main/sub DLC curve at the same time in 3D side-side ap.

/******************************************************************************/

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void msDlcInitWithCurve( WORD wHStart, WORD wHEnd, WORD wVStart, WORD wVEnd )
{
    BYTE ucTmp, ucTimeout;

    msDlc_FunctionEnter();

    g_ucDlcFastLoop = (g_DlcParameters.ucDlcFastAlphaBlending/2) * 16;
    if (g_DlcParameters.ucDlcFastAlphaBlending == 32) // 32 / 2 * 16 = 0 overflow
        g_ucDlcFastLoop = 255;

    g_ucHistogramTotal_L = 0;
    g_ucHistogramTotal_H = 0;

    //putstr("\r\nDLC Initilize use HK MCU");

    g_dwFactory = TOTAL_PIXEL/32/2;
    g_dwFactory_7 = (g_dwFactory - (g_dwFactory/8));
    g_dwFactory_9 = (g_dwFactory + (g_dwFactory/8));

    g_dwLstep = g_dwFactory*1/16*12/10;
    g_dwHstep = g_dwFactory*3/8;

    // Registers Init
    msWriteDLCRegsTbl(tDLC_Initialize);

    // Set histogram method/dither
    if (msReadByte(REG_ADDR_DLC_HANDSHAKE) & _BIT2)
    {
        ucTimeout = 60;
        while(!(msReadByte(REG_ADDR_DLC_HANDSHAKE) & _BIT3) && (ucTimeout--))
        {
            msDlc_Delay1ms(1);
        }
    }

    msWriteByte(REG_ADDR_DLC_HANDSHAKE, _BIT5|_BIT4 );

    // Set histogram range - H
    if (REG_ADDR_HISTOGRAM_RANGE_M_HST != REG_NULL)
    {
        // Main Window
        msWriteByte(REG_ADDR_HISTOGRAM_RANGE_M_HST, wHStart/8);
        msWriteByte(REG_ADDR_HISTOGRAM_RANGE_M_HEN, wHEnd/8);
        // MWE/Sub Window
        msWriteByte(REG_ADDR_HISTOGRAM_RANGE_S_HST, wHStart/8);
        msWriteByte(REG_ADDR_HISTOGRAM_RANGE_S_HEN, wHEnd/8);
    }
    // Set histogram range - V
    // Main Window
    msWriteByte(REG_ADDR_HISTOGRAM_RANGE_M_VST, wVStart/8);
    msWriteByte(REG_ADDR_HISTOGRAM_RANGE_M_VEN, wVEnd/8);
    // MWE/Sub Window
    msWriteByte(REG_ADDR_HISTOGRAM_RANGE_S_VST, wVStart/8);
    msWriteByte(REG_ADDR_HISTOGRAM_RANGE_S_VEN, wVEnd/8);

    msWriteByte(REG_ADDR_BLE_UPPER_BOND, DEFAULT_BLE_UPPER_BOND);
    msWriteByte(REG_ADDR_BLE_LOWER_BOND, DEFAULT_BLE_LOWER_BOND);

    // Enable range for Histogram
    msWriteByte(REG_ADDR_HISTOGRAM_RANGE_ENABLE, _BIT7|msReadByte(REG_ADDR_HISTOGRAM_RANGE_ENABLE) ); // Enable MAIN_WINDOW histogram

    // Init Luma Curve
    //putstr("[DLC]: Disable the Burst Write 16 Luma Curve\r\n");
	for(ucTmp=0; ucTmp<=0x0f; ucTmp++)
    {
        g_ucTable[ucTmp] = g_DlcParameters.ucLumaCurve[ucTmp];
        g_uwPreTable[ucTmp] = (WORD)g_DlcParameters.ucLumaCurve[ucTmp] * DLC_DEFLICK_PRECISION_SHIFT; // New de-Flick
	    msWriteByte((REG_ADDR_DLC_DATA_START_MAIN+ucTmp), g_ucTable[ucTmp]);
        msDlcWriteCurveLSB(MAIN_WINDOW, ucTmp, 0);
        msWriteByte((REG_ADDR_DLC_DATA_START_SUB+ucTmp), g_ucTable[ucTmp]);
        msDlcWriteCurveLSB(SUB_WINDOW, ucTmp, 0);
	}
    // init DLC curve index N0 & 16
    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_MAIN, g_ucTable[0]);
    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_MAIN + 1, 0x01);
    msDlcWriteCurveLSB(MAIN_WINDOW, 0xFF, 0);
    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_MAIN, 255-g_ucTable[15]);
    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_MAIN + 1, 0x01);
    msDlcWriteCurveLSB(MAIN_WINDOW, 16, 0);
    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB, g_ucTable[0]);
    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB + 1, 0x01);
    msDlcWriteCurveLSB(SUB_WINDOW, 0xFF, 0);
    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB, 255-g_ucTable[15]);
    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB + 1, 0x01);
    msDlcWriteCurveLSB(SUB_WINDOW, 16, 0);

    // Get Total Pixel Count
    msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) & 0xF0);
    msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) | _BIT2 | _BIT1);
    ucTimeout = 60;
    while(!(msReadByte(REG_ADDR_DLC_HANDSHAKE) & _BIT3) && (ucTimeout--))
    {
        msDlc_Delay1ms(1);
    }

    // patch for get stable pixel count
    msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) & 0xF0);
    msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) | _BIT2 | _BIT1);
    ucTimeout = 60;
    while(!(msReadByte(REG_ADDR_DLC_HANDSHAKE) & _BIT3) && (ucTimeout--))
    {
        msDlc_Delay1ms(1);
    }

    g_wLumiTotalCount = msReadByte(REG_ADDR_HISTOGRAM_TOTAL_COUNT_H);
    g_wLumiTotalCount = (g_wLumiTotalCount << 8) + (WORD)msReadByte(REG_ADDR_HISTOGRAM_TOTAL_COUNT_L);

    g_DlcParameters.ucDlcTimeOut = 0;
    g_DlcParameters.ucDlcFlickAlphaStart = FLICK_ALPHA_START_COUNTER;
    g_DlcParameters.ucDlcSlowEvent = 0;
    g_ucUpdateCnt = 0;

    /*
        ucTmp = 0;
        if (ucTmp)
        {
            msDlcOnOff(0, 0);
            msGetHistogramHandler(0);
            msDlcHandler(0);
            msDlcLibVersionCheck();
            msDlcSetVarHistogramRange(0);
        }
    */

    msDlc_FunctionExit();
}

void msDlc_SetCaptureRange(MS_U16 wHStart,MS_U16 wHEnd, MS_U16 wVStart, MS_U16 wVEnd)
{
    msDlc_FunctionEnter();

	if (REG_ADDR_HISTOGRAM_RANGE_M_HST != REG_NULL)
	{
        // Set histogram range - H
		// Main Window
		msWriteByte(REG_ADDR_HISTOGRAM_RANGE_M_HST, wHStart/8);
		msWriteByte(REG_ADDR_HISTOGRAM_RANGE_M_HEN, wHEnd/8);
		// MWE/Sub Window
		msWriteByte(REG_ADDR_HISTOGRAM_RANGE_S_HST, wHStart/8);
		msWriteByte(REG_ADDR_HISTOGRAM_RANGE_S_HEN, wHEnd/8);
#if 1  // Virtical size need to change by display
		// Set histogram range - V
		// Main Window
		msWriteByte(REG_ADDR_HISTOGRAM_RANGE_M_VST, wVStart/8);
		msWriteByte(REG_ADDR_HISTOGRAM_RANGE_M_VEN, wVEnd/8);
		// MWE/Sub Window
		msWriteByte(REG_ADDR_HISTOGRAM_RANGE_S_VST, wVStart/8);
		msWriteByte(REG_ADDR_HISTOGRAM_RANGE_S_VEN, wVEnd/8);
#else
		wVStart = wVStart;
		wVEnd = wVEnd;
#endif
        msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) & 0xF3);
        msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) | _BIT2 );
#if 0 // Don't wait the histogram acknowledge to save time .
	    // Get Total Pixel Count
	    msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) & 0xF0);
	    msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) | _BIT2 | _BIT1);
	    ucTimeout = 60;
	    while(!(msReadByte(REG_ADDR_DLC_HANDSHAKE) & _BIT3) && (ucTimeout--))
	    {
	        msDlc_Delay1ms(1);
	    }

	    // patch for get stable pixel count
	    msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) & 0xF0);
	    msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) | _BIT2 | _BIT1);
	    ucTimeout = 60;
	    while(!(msReadByte(REG_ADDR_DLC_HANDSHAKE) & _BIT3) && (ucTimeout--))
	    {
	        msDlc_Delay1ms(1);
	    }

	    g_wLumiTotalCount = msReadByte(REG_ADDR_HISTOGRAM_TOTAL_COUNT_H);
	    g_wLumiTotalCount = (g_wLumiTotalCount << 8) + (WORD)msReadByte(REG_ADDR_HISTOGRAM_TOTAL_COUNT_L);
#endif
	}
	msDlc_FunctionExit();
}

void msDlcEnableSetDLCCurveBoth(BOOL bEnable)
{
    g_bSetDLCCurveBoth = bEnable;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void msDlcOnOff(BOOL bSwitch, BOOL bWindow)
{
    BYTE ucTmp;
    BYTE ucCtlBit;

    msDlc_FunctionEnter();

    if( MAIN_WINDOW == bWindow )
        ucCtlBit = _BIT7;
    else // Sub window
        ucCtlBit = _BIT6;

    if( bSwitch ) // Enable
    {
        if((msReadByte(REG_ADDR_DLC_HANDSHAKE) & ucCtlBit) )// Shall not turn DLC on when it is on already.
        {
           msDlc_FunctionExit();
           return;
        }

        // Init Luma Curve
	    for(ucTmp=0; ucTmp<=0x0f; ucTmp++)
        {
            g_ucTable[ucTmp] = g_DlcParameters.ucLumaCurve[ucTmp];
            g_uwPreTable[ucTmp] = (WORD)g_DlcParameters.ucLumaCurve[ucTmp] * DLC_DEFLICK_PRECISION_SHIFT; // New de-Flick
	        msWriteByte((REG_ADDR_DLC_DATA_START_MAIN+ucTmp), g_ucTable[ucTmp]);
            msDlcWriteCurveLSB(MAIN_WINDOW, ucTmp, 0);
	        msWriteByte((REG_ADDR_DLC_DATA_START_SUB+ucTmp), g_ucTable[ucTmp]);
            msDlcWriteCurveLSB(SUB_WINDOW, ucTmp, 0);
	    }

        // init DLC curve index N0 & 16
        msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_MAIN, g_ucTable[0]);
        msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_MAIN + 1, 0x01);
        msDlcWriteCurveLSB(MAIN_WINDOW, 0xFF, 0);
        msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_MAIN, 255-g_ucTable[15]);
        msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_MAIN + 1, 0x01);
        msDlcWriteCurveLSB(MAIN_WINDOW, 16, 0);
        msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB, g_ucTable[0]);
        msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB + 1, 0x01);
        msDlcWriteCurveLSB(SUB_WINDOW, 0xFF, 0);
        msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB, 255-g_ucTable[15]);
        msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB + 1, 0x01);
        msDlcWriteCurveLSB(SUB_WINDOW, 16, 0);

        msWriteBit(REG_ADDR_DLC_HANDSHAKE, TRUE, ucCtlBit);

        // Request HW to do histogram
        msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) & 0xF0);
        if( MAIN_WINDOW == bWindow ) // Main window
        {
            // Enable main window histogram, and handshake request
            if(g_bSetDLCCurveBoth)
            {
                msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) | _BIT2 | _BIT1 | _BIT0);
            }
            else
            {
                msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) | _BIT2 | _BIT1);
            }
        }
        else // Sub window
        {
            // Enable sub window histogram, and handshake request
            msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) | _BIT2 | _BIT0);
        }

        //msWriteBit( REG_ADDR_BLE_WLE_ENABLE, g_DlcParameters.bEnableBLE, _BIT6 );
        //msWriteBit( REG_ADDR_BLE_WLE_ENABLE, g_DlcParameters.bEnableWLE, _BIT5 );

        g_DlcParameters.ucDlcFlickAlphaStart = FLICK_ALPHA_START_COUNTER;
        g_DlcParameters.ucDlcSlowEvent = 0;
    }
    else // Disable
    {
        msWriteBit(REG_ADDR_DLC_HANDSHAKE, FALSE, ucCtlBit);
    }

    msDlc_FunctionExit();
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void msDlcSetBleOnOff(BOOL bSwitch)
{
    g_bSetDlcBleOn= bSwitch ;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
BYTE msGetAverageLuminous(void) // = external msGetAverageLuminousValue()
{
    WORD uwTmpAvr;

    uwTmpAvr = g_wLumiAverageTemp;

    // Normalize
    if (g_wLumiTotalCount!=0)
        uwTmpAvr = (WORD)((256 * (DWORD)g_wLumiAverageTemp + (g_wLumiTotalCount/2)) / g_wLumiTotalCount);
    else
        uwTmpAvr = 0;

    if (uwTmpAvr >= 255)
        uwTmpAvr = 255;

    return (BYTE)uwTmpAvr;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
WORD msGetAverageLuminous_x4(void) // = external msGetAverageLuminousValue()
{
    WORD uwTmpAvr;

    uwTmpAvr = g_wLumiAverageTemp;

    // Normalize
    if (g_wLumiTotalCount!=0)
        uwTmpAvr = (WORD)((1024 * (DWORD)g_wLumiAverageTemp + (g_wLumiTotalCount/2)) / g_wLumiTotalCount);
    else
        uwTmpAvr = 0;

    if (uwTmpAvr >= 1024- 1)
        uwTmpAvr = 1024- 1;

    return uwTmpAvr;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
WORD msGetAverageLuminous_x10(void)
{
    WORD uwTmpAvr;

    uwTmpAvr = g_wLumiAverageTemp;

    // Normalize
    if (g_wLumiTotalCount!=0)
        uwTmpAvr = (WORD)((256 * 10 * (DWORD)g_wLumiAverageTemp + (g_wLumiTotalCount/2)) / g_wLumiTotalCount);
    else
        uwTmpAvr = 0;

    if (uwTmpAvr >= (256 * 10 - 1))
        uwTmpAvr = (256 * 10 - 1);

    return uwTmpAvr;
}
//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
BOOL msGetHistogramHandler(BOOL bWindow)
{
    BYTE ucTmp;
    WORD wTmpHistogramSum; // for protect histogram overflow
    BYTE ucHistogramOverflowIndex;
    BYTE ucReturn;

    msDlc_FunctionEnter();

#if 0 //Have double buffer, does not need to wait.
    // ENABLE_FAST_HISTOGRAM_CATCH
    if( !(msReadByte(REG_ADDR_DLC_HANDSHAKE) & _BIT3) )  // Wait until ready
    {
        msDlc_FunctionExit();
        return FALSE;
    }
#endif

    if (g_DlcParameters.ucDlcTimeOut)
    {
        // Get histogram 32 data
        // Total/BW_ref histogram count
        // putstr("[DLC]: Disable the Burst Read 32-section Histogram\r\n");
        //dwTotalCount = 0;
        for (ucTmp = 0, wTmpHistogramSum = 0, ucHistogramOverflowIndex = 0xFF; ucTmp < 32; ucTmp++)
        {
            g_wLumaHistogram32H[ucTmp] = msReadByte(REG_ADDR_HISTOGRAM_DATA_32+(2*ucTmp)+1);
            g_wLumaHistogram32H[ucTmp] <<= 8;
            g_wLumaHistogram32H[ucTmp] |= msReadByte(REG_ADDR_HISTOGRAM_DATA_32+(2*ucTmp));
            //printf(" %d",g_wLumaHistogram32H[ucTmp]);
            //dwTotalCount += g_wLumaHistogram32H[ucTmp];

             // protect histogram overflow
            if (g_wLumaHistogram32H[ucTmp] >= 65535)
            {
                ucHistogramOverflowIndex = ucTmp;
                //printf("\r\n overflow %d",ucHistogramOverflowIndex);
            }
            g_wLumaHistogram32H[ucTmp] >>= 1; // protect histogram overflow
            wTmpHistogramSum += g_wLumaHistogram32H[ucTmp];
        }

#if (ENABLE_HISTOGRAM_8)
        // Set port address to access histogram
	    //putstr("[DLC]: Disable the Burst Read 8-section Histogram\r\n");
	    for (ucTmp = 0; ucTmp < 8; ucTmp++)
	    {
	        g_wLumaHistogram8H[ucTmp] = msReadByte(REG_ADDR_HISTOGRAM_DATA_8+(2*ucTmp)+1);
            g_wLumaHistogram8H[ucTmp] <<= 8;
            g_wLumaHistogram8H[ucTmp] |= msReadByte(REG_ADDR_HISTOGRAM_DATA_8+(2*ucTmp));
	    }
#endif
        //Get the histogram report sum of pixel number .
        g_wLumiTotalCount = msReadByte(REG_ADDR_HISTOGRAM_TOTAL_COUNT_H);
        g_wLumiTotalCount = (g_wLumiTotalCount << 8) + (WORD)msReadByte(REG_ADDR_HISTOGRAM_TOTAL_COUNT_L);

         // protect histogram overflow
        if (ucHistogramOverflowIndex != 0xFF)
        {
            if (wTmpHistogramSum > g_wLumiTotalCount)
                wTmpHistogramSum = g_wLumiTotalCount;
            g_wLumaHistogram32H[ucHistogramOverflowIndex] += (g_wLumiTotalCount - wTmpHistogramSum);
        }

        g_ucHistogramTotal_L = msReadByte( REG_ADDR_HISTOGRAM_TOTAL_SUM_L );
        g_ucHistogramTotal_H = msReadByte( REG_ADDR_HISTOGRAM_TOTAL_SUM_H );

        // Get MIN. & MAX VALUE
        if( MAIN_WINDOW == bWindow )   // Main window
        {
            g_ucHistogramMin = msReadByte(REG_ADDR_MAIN_MIN_VALUE);
            g_ucHistogramMax = msReadByte(REG_ADDR_MAIN_MAX_VALUE);
        }
        else // Sub window
        {
            g_ucHistogramMin = msReadByte(REG_ADDR_SUB_MIN_VALUE);
            g_ucHistogramMax = msReadByte(REG_ADDR_SUB_MAX_VALUE);
        }

        g_ucTmpAvgN_1 = msGetAverageLuminous();
        g_wTmpAvgN_1_x10 = msGetAverageLuminous_x10();

        g_wLumiAverageTemp = ((WORD)g_ucHistogramTotal_H<<8) + g_ucHistogramTotal_L;
        g_ucTmpAvgN = msGetAverageLuminous();
        g_wTmpAvgN_x10 = msGetAverageLuminous_x10();

        ucReturn = TRUE;
    }
    else
    {
        ucReturn = FALSE;
        //putstr("\r\n!!!!! DLC Time Out !!!!!");
    }

    // After read histogram, request HW to do histogram
    // Request HW to do histogram
    msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) & 0xF3);

    g_DlcParameters.ucDlcTimeOut = 150; // 150ms

    msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) | _BIT2 );

    msDlc_FunctionExit();

    return ucReturn;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void msDlcSetVarHistogramRange(BYTE *pRangeArray)
{
    BYTE ucTmp;
    DWORD wReg;

    wReg = REG_ADDR_HISTOGRAM_8_RANGE_START;
    for (ucTmp=0; ucTmp<7 ; ucTmp++)
    {
        msWriteByte(wReg, *(pRangeArray + ucTmp));
        wReg += 1;
    }
}

//=========================================================
//
//=========================================================
void msDlcLumiDiffCtrl(void)
{
    if (g_DlcParameters.ucDlcFlickAlphaStart)
    {
        g_DlcParameters.ucDlcFlickAlphaStart--;
    }

    if (g_DlcParameters.ucDlcSlowEvent)
    {
        g_DlcParameters.ucDlcSlowEvent--;
        g_ucDlcFlickAlpha = 1; // it mean the curve change slowly
        g_ucDlcFastLoop = 0;
    }
    else
    {
        if (g_wTmpAvgN_1_x10 >= g_wTmpAvgN_x10)
        {
            if ((g_ucDlcFastLoop > 0) || ((g_wTmpAvgN_1_x10 - g_wTmpAvgN_x10 ) >= ((WORD)g_DlcParameters.ucDlcAvgDelta * 10)))
            {
                //printf("\r\n1H:Avg = %d",g_ucTmpAvgN_1 - g_ucTmpAvgN);
                //printf(" g_ucDlcFlickAlpha = %d\r\n",g_ucDlcFlickAlpha);

                g_ucDlcFlickAlpha = g_DlcParameters.ucDlcFastAlphaBlending; // (32/32) = (32/DLC_DEFLICK_BLEND_FACTOR) // it mean the curve change quickly
                if (g_ucDlcFastLoop == 0)
                {
                    g_ucDlcFastLoop = (g_DlcParameters.ucDlcFastAlphaBlending/2) * 16;
                    if (g_DlcParameters.ucDlcFastAlphaBlending == 32) // 32 / 2 * 16 = 0 overflow
                    {
                        g_ucDlcFastLoop = 255;
                    }
                }
            }
            else if ((g_DlcParameters.ucDlcAvgDeltaStill <= ((WORD)g_DlcParameters.ucDlcAvgDelta * 10)) && (g_DlcParameters.ucDlcAvgDeltaStill != 0) && ((g_wTmpAvgN_1_x10 - g_wTmpAvgN_x10) <= g_DlcParameters.ucDlcAvgDeltaStill)) // if (Yavg is less than g_DlcParameters.ucDlcAvgDeltaStill) & (Fast change is finish) does NOT do curve change
            {
                g_ucDlcFlickAlpha = 0; // (0/32) = (1/DLC_DEFLICK_BLEND_FACTOR)  // it mean the curve will be still
            }
            else // if ((g_ucTmpAvgN_1 - g_ucTmpAvgN) < g_DlcParameters.ucDlcAvgDelta) // if (Yavg is less than g_DlcParameters.ucDlcAvgDelta) & (Fast change is finish) may do curve change slowly
            {
                if ( g_DlcParameters.ucDlcFlickAlphaStart == 0) // chagne source and it's stable
                {
                    g_ucDlcFlickAlpha = 1; // (1/32) = (1/DLC_DEFLICK_BLEND_FACTOR)  // it mean the curve change slowly
                }
                else
                {
                    g_ucDlcFlickAlpha = g_DlcParameters.ucDlcFastAlphaBlending; // (32/32) = (32/DLC_DEFLICK_BLEND_FACTOR) // it mean the curve change quickly
                }
            }
        }
        else if (g_wTmpAvgN_1_x10 < g_wTmpAvgN_x10)
        {
             if ((g_ucDlcFastLoop > 0) || (( g_wTmpAvgN_x10 - g_wTmpAvgN_1_x10 ) >= ((WORD)g_DlcParameters.ucDlcAvgDelta * 10)))
            {
                g_ucDlcFlickAlpha = g_DlcParameters.ucDlcFastAlphaBlending; // (32/32) = (32/DLC_DEFLICK_BLEND_FACTOR)  // it mean the curve change quickly
                if (g_ucDlcFastLoop == 0)
                {
                    g_ucDlcFastLoop = (g_DlcParameters.ucDlcFastAlphaBlending/2) * 16;
                    if (g_DlcParameters.ucDlcFastAlphaBlending == 32) // 32 / 2 * 16 = 0 overflow
                    {
                        g_ucDlcFastLoop = 255;
                    }
                }
            }
            else if ((g_DlcParameters.ucDlcAvgDeltaStill <= ((WORD)g_DlcParameters.ucDlcAvgDelta * 10)) && (g_DlcParameters.ucDlcAvgDeltaStill != 0) && ((g_wTmpAvgN_x10 - g_wTmpAvgN_1_x10) <= g_DlcParameters.ucDlcAvgDeltaStill)) // if (Yavg is less than g_DlcParameters.ucDlcAvgDeltaStill) & (Fast change is finish) does NOT do curve change
            {
                g_ucDlcFlickAlpha = 0; // (0/32) = (1/DLC_DEFLICK_BLEND_FACTOR)  // it mean the curve will be still
            }
            else // if ((g_ucTmpAvgN - g_ucTmpAvgN_1) < g_DlcParameters.ucDlcAvgDelta) // if (Yavg is less than g_DlcParameters.ucDlcAvgDelta) & (Fast change is finish) may do curve change slowly
            {
                if (g_DlcParameters.ucDlcFlickAlphaStart == 0) // chagne source and it's stable
                {
                    g_ucDlcFlickAlpha = 1; // (1/32) = (1/DLC_DEFLICK_BLEND_FACTOR)  // it mean the curve change slowly
                }
                else
                {
                    g_ucDlcFlickAlpha = g_DlcParameters.ucDlcFastAlphaBlending; // (32/32) = (32/DLC_DEFLICK_BLEND_FACTOR)  // it mean the curve change quickly
                }
            }
        }
    }
}

//=========================================================
//
//=========================================================
void msDlcHandler(BOOL bWindow)
{
    BYTE ucTmp,Tmp,sepPoint =0,sepPoint0 =0,sepPoint1 =0,sepPoint2 =0,ucYAvgNormalize=0,belnging_factor;
    WORD ucYAvg=0;
    WORD uwHistogramMax, uwHistogramMax2nd,uwHistogramSlopRatioL, uwHistogramSlopRatioH; // 20110406 Ranma add
    DWORD uwHistogramAreaSum[32];//,uwPre_Histogram_ShiftR[32],uwPre_Histogram_ShiftL[32]; // 20110406 Ranma add
    WORD uwHistogram_BLE_AreaSum[32];
    WORD BLE_StartPoint=0,BLE_EndPoint=0,BLE_StartPoint_Correction=0,BLE_EndPoint_Correction=0;
    WORD BLE_Slop_400,BLE_Slop_480,BLE_Slop_500,BLE_Slop_600,BLE_Slop_800,Dark_BLE_Slop_Min;
    static WORD Pre_BLE_Slop=0x400;
    static WORD Pre_g_ucDlcFlickAlpha = 0,Pre_TotalYSum = 0,Pre_BLE_StartPoint_Correction=0xFFF;
    static BYTE ucPre_YAvg =0x00;
    WORD uwBLE_Blend=0,uwBLE_StartPoint_Blend=0,CurveDiff=0,CurveDiff_Coring=0,CurveDiff_base=0,YAvg_base=0;
    BYTE g_ucDlcFlickAlpha_Max=0,g_ucDlcFlickAlpha_Min=0,YAvg_TH_H,YAvg_TH_L,g_ucDlcFlickAlpha_temp,ucDlcFlickAlpha_Diff=0;
    DWORD dwSumBelowAvg=0, dwSumAboveAvg=0,dwHistogramDlcSlop[17],BLE_Slop=0x400,BLE_Slop_tmp=0x400;
    WORD uwArrayComp, uwArraySum, Delta_YAvg=0,coeff0,coeff1,coeff2;
    DWORD His_ratio_BelowAvg,His_ratio_AboveAvg;
    DWORD g_uwCurveHistogram0[16],g_uwCurveHistogram1[16],g_uwCurveHistogram2[16];
    DWORD BLE_sum=0;
    WORD Pre_YAvg_base,Diff_L,Diff_M,Diff_H,Pre_CurveDiff_base;

    //When DLC algorithm is from kernel, force return.
    if(g_DlcParameters.ucDlcAlgorithmMode == XC_DLC_ALGORITHM_KERNEL)
    {
        return ;
    }

    msDlc_FunctionEnter();

    msDlcLumiDiffCtrl();

    if(g_DlcParameters.ucDlcAlgorithmMode == XC_DLC_ALGORITHM_OLD)
    {
        // If difference is too small, don't do dlc
        if( ((g_ucHistogramMax-g_ucHistogramMin) < 0x0A) || (g_ucHistogramMin >= g_ucHistogramMax) )
        {
            if (g_DlcParameters.ucDlcPureImageMode == DLC_PURE_IMAGE_DO_NOTHING)
            {
                msDlc_FunctionExit();
                return;
            }
            else if (g_DlcParameters.ucDlcPureImageMode == DLC_PURE_IMAGE_LINEAR_CURVE)
            {
                BYTE ucLuma = 0x07;
                for( ucTmp = 0; ucTmp < 16; ++ ucTmp )
                {
                    g_ucTable[ucTmp] = ucLuma;
                    ucLuma += 0x10;
                }

                g_ucDlcFlickAlpha = g_DlcParameters.ucDlcFastAlphaBlending; // it mean the curve change quickly

                // Write data to luma curve ...
                msDlcWriteCurve(bWindow);
                if(g_bSetDLCCurveBoth)
                {
                    msDlcWriteCurve(SUB_WINDOW);
                }
                msDlc_FunctionExit();
                return;
            }
            else // g_DlcParameters.ucDlcPureImageMode == DLC_PURE_IMAGE_DLC_CURVE
                ;
        }

        //=========================================================
        // LC[6] - H0 ~ H12
        //=========================================================
        uwArraySum=0;
        for(ucTmp=0;ucTmp<13;ucTmp++)
            uwArraySum+= g_wLumaHistogram32H[ucTmp];
        uwArrayComp = uwArraySum / 13;

        if (uwArrayComp < g_dwFactory_7)
        {
            ucTmp = (BYTE)(((g_dwFactory_7 - uwArrayComp) * SD2 / (Lstep*128))+1);
            if (ucTmp > LD2)
                ucTmp = LD2;
            g_ucTable[6] = 0x67 - ucTmp;
        }
        else if (uwArrayComp > g_dwFactory_9)
        {
            ucTmp = (BYTE)(((uwArrayComp - g_dwFactory_9) * SD1 / (Hstep*128))+1);
            if (ucTmp > LD1)
                ucTmp = LD1;
            g_ucTable[6] = 0x67 + ucTmp;
        }
        else
            g_ucTable[6] = 0x67;

        if ((g_ucTable[6]> s_Slope[Slope_6])&&((g_ucTable[6] - s_Slope[Slope_6])>=2))
            g_ucTable[6] = g_ucTable[6] - 1;
        else if ((s_Slope[Slope_6]> g_ucTable[6])&&((s_Slope[Slope_6] - g_ucTable[6])>=2))
            g_ucTable[6] = g_ucTable[6] + 1;
        else
            g_ucTable[6] = s_Slope[Slope_6];

        //=========================================================
        // LC[4] - H0 ~ H8
        //=========================================================

        g_ucTable[4] = ((g_ucTable[6] - 0x07) * 2 / 3) + 0x07;

        uwArraySum = uwArraySum - (g_wLumaHistogram32H[12] + g_wLumaHistogram32H[11] + g_wLumaHistogram32H[10] + g_wLumaHistogram32H[9]);
        uwArrayComp = uwArraySum/9;

        if (uwArrayComp < g_dwFactory_7)
        {
            ucTmp = (BYTE)(((g_dwFactory_7 - uwArrayComp) * SC2 / (Lstep*128))+1);
            if (ucTmp > LC2)
                ucTmp = LC2;
            g_ucTable[4] = g_ucTable[4] - ucTmp;
        }
        else if (uwArrayComp > g_dwFactory_9)
        {
            ucTmp = (BYTE)(((uwArrayComp - g_dwFactory_9) * SC1 / (Hstep*128))+1);
            if (ucTmp > LC1)
                ucTmp = LC1;
            g_ucTable[4] = g_ucTable[4] + ucTmp;
        }

        if ((g_ucTable[4]> s_Slope[Slope_4])&&((g_ucTable[4]- s_Slope[Slope_4])>=2))
            g_ucTable[4] = g_ucTable[4] - 1;
        else if ((s_Slope[Slope_4]> g_ucTable[4])&&((s_Slope[Slope_4]- g_ucTable[4])>=2))
            g_ucTable[4] = g_ucTable[4] + 1;
        else
            g_ucTable[4] = s_Slope[Slope_4];

        //=========================================================
        // LC[2] - H0 ~ H4
        //=========================================================

        g_ucTable[2] = ((g_ucTable[4] - 0x07) * 1 / 2) + 0x07;
        uwArraySum = uwArraySum - (g_wLumaHistogram32H[8] + g_wLumaHistogram32H[7] + g_wLumaHistogram32H[6] + g_wLumaHistogram32H[5]);
        uwArrayComp = uwArraySum / 5;

        if (uwArrayComp < g_dwFactory_7)
        {
            ucTmp = (BYTE)(((g_dwFactory_7 - uwArrayComp) * SB2 / (Lstep*128))+1);
            if (ucTmp > LB2)
                ucTmp = LB2;
            g_ucTable[2] = g_ucTable[2] - ucTmp;
        }
        else if (uwArrayComp > g_dwFactory_9)
        {
            ucTmp = (BYTE)(((uwArrayComp - g_dwFactory_9) * SB1 / (Hstep*128))+1);
            if (ucTmp > LB1)
                ucTmp = LB1;
            g_ucTable[2] = g_ucTable[2] + ucTmp;
        }

        if ((g_ucTable[2]> s_Slope[Slope_2])&&((g_ucTable[2]- s_Slope[Slope_2])>=2))
            g_ucTable[2] = g_ucTable[2] - 1;
        else if ((s_Slope[Slope_2]> g_ucTable[2])&&((s_Slope[Slope_2]- g_ucTable[2])>=2))
            g_ucTable[2] = g_ucTable[2] + 1;
        else
            g_ucTable[2] = s_Slope[Slope_2];

        //=========================================================
        // LC[1] - H0 ~ H2
        //=========================================================

        g_ucTable[1] = ((g_ucTable[2] - 0x07) * 1 / 2) + 0x07;
        uwArraySum = uwArraySum - (g_wLumaHistogram32H[4] + g_wLumaHistogram32H[3]);
        uwArrayComp = uwArraySum / 3;

        if (uwArrayComp < g_dwFactory_7)
        {
            ucTmp = (BYTE)(((g_dwFactory_7 - uwArrayComp) * SA2 / (Lstep*128))+1);
            if (ucTmp > LA2)
                ucTmp = LA2;

            if (g_ucTable[1] <= ucTmp)
                g_ucTable[1] = 0x00;
            else
                g_ucTable[1] = g_ucTable[1] - ucTmp;
        }
        else if (uwArrayComp > g_dwFactory_9)
        {
            ucTmp = (BYTE)(((uwArrayComp - g_dwFactory_9) * SA1 / (Hstep*128))+1);
            if (ucTmp > LA1)
                ucTmp = LA1;
            g_ucTable[1] = g_ucTable[1] + ucTmp;
        }

        if ((g_ucTable[1]> s_Slope[Slope_1])&&((g_ucTable[1]- s_Slope[Slope_1])>=1))
            g_ucTable[1] = g_ucTable[1] - 1;
        else if ((s_Slope[Slope_1]> g_ucTable[1])&&((s_Slope[Slope_1]- g_ucTable[1])>=2))
            g_ucTable[1] = g_ucTable[1] + 1;
        else
            g_ucTable[1] = s_Slope[Slope_1];

        //=========================================================
        // LC[9] - H19 ~ H31
        //=========================================================

        uwArraySum=0;
        for (ucTmp=19; ucTmp<32; ucTmp++)
            uwArraySum+= g_wLumaHistogram32H[ucTmp];

        uwArrayComp = uwArraySum / 13;

        if (uwArrayComp < g_dwFactory_7)
        {
            ucTmp = (BYTE)(((g_dwFactory_7 - uwArrayComp) * SE1 / (Lstep*128))+1);
            if (ucTmp > LE1)
                ucTmp = LE1;
            g_ucTable[9] = 0x97 + ucTmp;
        }
        else if (uwArrayComp > g_dwFactory_9)
        {
            ucTmp = (BYTE)(((uwArrayComp - g_dwFactory_9) * SE2 / (Hstep*128))+1);
            if (ucTmp > LE2)
                ucTmp = LE2;
            g_ucTable[9] = 0x97 - ucTmp;
        }
        else
            g_ucTable[9] = 0x97;

        //=========================================================
        // LC[11] - H23 ~ H31
        //=========================================================

        g_ucTable[11] = 0xF7 - ((0xF7 - g_ucTable[9]) * 2 / 3);
        uwArraySum = uwArraySum - (g_wLumaHistogram32H[19] + g_wLumaHistogram32H[20] + g_wLumaHistogram32H[21] + g_wLumaHistogram32H[22]);
        uwArrayComp = uwArraySum / 9;

        if (uwArrayComp< g_dwFactory_7)
        {
            ucTmp = (BYTE)(((g_dwFactory_7 - uwArrayComp) * SF1 / (Lstep*128))+1);
            if (ucTmp > LF1)
                ucTmp = LF1;
            g_ucTable[11] = g_ucTable[11] + ucTmp;
        }
        else if (uwArrayComp > g_dwFactory_9)
        {
            ucTmp = (BYTE)(((uwArrayComp - g_dwFactory_9) * SF2 / (Hstep*128))+1);
            if (ucTmp > LF2)
                ucTmp = LF2;
            g_ucTable[11] = g_ucTable[11] - ucTmp;
        }

        //=========================================================
        // LC[13] - H27 ~ H31
        //=========================================================

        g_ucTable[13] = 0xF7 - ((0xF7 - g_ucTable[11]) * 1 / 2);
        uwArraySum = uwArraySum - (g_wLumaHistogram32H[23] + g_wLumaHistogram32H[24] + g_wLumaHistogram32H[25] + g_wLumaHistogram32H[26]);
        uwArrayComp = uwArraySum / 5;

        if (uwArrayComp< g_dwFactory_7)
        {
            ucTmp = (BYTE)(((g_dwFactory_7 - uwArrayComp) * SG1 / (Lstep*128))+1);
            if (ucTmp > LG1)
                ucTmp = LG1;

            if ((0xFF - g_ucTable[13]) <= ucTmp)
                g_ucTable[13] = 0xFF;
            else
                g_ucTable[13] = g_ucTable[13] + ucTmp;
        }
        else if (uwArrayComp > g_dwFactory_9)
        {
            ucTmp = (BYTE)(((uwArrayComp - g_dwFactory_9) * SG2 / (Hstep*128))+1);
            if (ucTmp > LG2)
                ucTmp = LG2;
            g_ucTable[13] = g_ucTable[13] - ucTmp;
        }

        //=========================================================
        // LC[14] - H29 ~ H31
        //=========================================================

        g_ucTable[14] = 0xF7 - ((0xF7 - g_ucTable[13]) * 1 / 2);
        uwArraySum = uwArraySum - (g_wLumaHistogram32H[27] + g_wLumaHistogram32H[28]);
        uwArrayComp = uwArraySum / 3;

        if (uwArrayComp < g_dwFactory_7)
        {
            ucTmp = (BYTE)(((g_dwFactory_7 - uwArrayComp) * SH1 / (Lstep*128))+1);
            if (ucTmp > LH1)
            ucTmp = LH1;

            if ((0xFF - g_ucTable[14]) <= ucTmp)
                g_ucTable[14] = 0xFF;
            else
                g_ucTable[14] = g_ucTable[14] + ucTmp;
        }
        else if (uwArrayComp > g_dwFactory_9)
        {
            ucTmp = (BYTE)(((uwArrayComp - g_dwFactory_9) * SH2 / (Hstep*128))+1);
            if (ucTmp > LH2)
            ucTmp = LH2;
            g_ucTable[14] =g_ucTable[14] - ucTmp;
        }

        s_Slope[Slope_1] = g_ucTable[1];
        s_Slope[Slope_2] = g_ucTable[2];
        s_Slope[Slope_4] = g_ucTable[4];
        s_Slope[Slope_6] = g_ucTable[6];

        g_ucTable[3] = g_ucTable[2] + ((g_ucTable[4] - g_ucTable[2]) * 1 / 2);
        g_ucTable[5] = g_ucTable[4] + ((g_ucTable[6] - g_ucTable[4]) * 1 / 2);
        g_ucTable[7] = g_ucTable[6] + ((g_ucTable[9] - g_ucTable[6]) * 1 / 3);
        g_ucTable[8] = g_ucTable[6] + ((g_ucTable[9] - g_ucTable[6]) * 2 / 3);
        g_ucTable[10] = g_ucTable[9] + ((g_ucTable[11] - g_ucTable[9]) * 1 / 2);
        g_ucTable[12] = g_ucTable[11] + ((g_ucTable[13] - g_ucTable[11]) * 1 / 2);
        g_ucTable[0] = g_ucTable[1]/3; // Joseph 20081014
        g_ucTable[15] = 0xFF - ((0xFF - g_ucTable[14]) / 3);
        g_ucTable[15]= ((g_ucTable[15]) / 2) + (0xf7 / 2);

        // Get Average Value
        ucYAvg = msGetAverageLuminous();

        //printf("\r\n ucYAvg = %d",ucYAvg);
        if (ucYAvg >= g_DlcParameters.ucDlcYAvgThresholdH)
        {
            for (ucTmp=0; ucTmp<16; ucTmp++)
            {
                g_DlcParameters.ucLumaCurve2[ucTmp] = g_DlcParameters.ucLumaCurve2_a[ucTmp];
            }
        }
        else if (ucYAvg <= g_DlcParameters.ucDlcYAvgThresholdL)
        {
            for (ucTmp=0; ucTmp<16; ucTmp++)
            {
                g_DlcParameters.ucLumaCurve2[ucTmp] = g_DlcParameters.ucLumaCurve2_b[ucTmp];
            }
        }
        else // (ucYAvg > g_DlcParameters.ucDlcYAvgThresholdL) && (ucYAvg < g_DlcParameters.ucDlcYAvgThresholdH)
        {
            WORD wYavgW;

            wYavgW = ((WORD)(ucYAvg - g_DlcParameters.ucDlcYAvgThresholdL) * 128) / (g_DlcParameters.ucDlcYAvgThresholdH - g_DlcParameters.ucDlcYAvgThresholdL);

            for (ucTmp=0; ucTmp<16; ucTmp++)
            {
                g_DlcParameters.ucLumaCurve2[ucTmp] = ((WORD)(wYavgW * g_DlcParameters.ucLumaCurve2_a[ucTmp]) + (WORD)((128 - wYavgW) * g_DlcParameters.ucLumaCurve2_b[ucTmp])) / 128;
            }
        }

        if (((WORD)g_ucTable[0] + g_DlcParameters.ucLumaCurve[0] + g_DlcParameters.ucLumaCurve2[0]) >= (0x07 + 0x07))
            g_ucTable[0] = g_ucTable[0]+(g_DlcParameters.ucLumaCurve[0]-0x07)+(g_DlcParameters.ucLumaCurve2[0]-0x07);
        else
            g_ucTable[0] = 0;

        g_ucTable[1] = g_ucTable[1]+(g_DlcParameters.ucLumaCurve[1]-0x17)+(g_DlcParameters.ucLumaCurve2[1]-0x17);
        g_ucTable[2] = g_ucTable[2]+(g_DlcParameters.ucLumaCurve[2]-0x27)+(g_DlcParameters.ucLumaCurve2[2]-0x27);
        g_ucTable[3] = g_ucTable[3]+(g_DlcParameters.ucLumaCurve[3]-0x37)+(g_DlcParameters.ucLumaCurve2[3]-0x37);
        g_ucTable[4] = g_ucTable[4]+(g_DlcParameters.ucLumaCurve[4]-0x47)+(g_DlcParameters.ucLumaCurve2[4]-0x47);
        g_ucTable[5] = g_ucTable[5]+(g_DlcParameters.ucLumaCurve[5]-0x57)+(g_DlcParameters.ucLumaCurve2[5]-0x57);
        g_ucTable[6] = g_ucTable[6]+(g_DlcParameters.ucLumaCurve[6]-0x67)+(g_DlcParameters.ucLumaCurve2[6]-0x67);
        g_ucTable[7] = g_ucTable[7]+(g_DlcParameters.ucLumaCurve[7]-0x77)+(g_DlcParameters.ucLumaCurve2[7]-0x77);
        g_ucTable[8] = g_ucTable[8]+(g_DlcParameters.ucLumaCurve[8]-0x87)+(g_DlcParameters.ucLumaCurve2[8]-0x87);
        g_ucTable[9] = g_ucTable[9]+(g_DlcParameters.ucLumaCurve[9]-0x97)+(g_DlcParameters.ucLumaCurve2[9]-0x97);
        g_ucTable[10] = g_ucTable[10]+(g_DlcParameters.ucLumaCurve[10]-0xa7)+(g_DlcParameters.ucLumaCurve2[10]-0xa7);
        g_ucTable[11] = g_ucTable[11]+(g_DlcParameters.ucLumaCurve[11]-0xb7)+(g_DlcParameters.ucLumaCurve2[11]-0xb7);
        g_ucTable[12] = g_ucTable[12]+(g_DlcParameters.ucLumaCurve[12]-0xc7)+(g_DlcParameters.ucLumaCurve2[12]-0xc7);
        g_ucTable[13] = g_ucTable[13]+(g_DlcParameters.ucLumaCurve[13]-0xd7)+(g_DlcParameters.ucLumaCurve2[13]-0xd7);
        g_ucTable[14] = g_ucTable[14]+(g_DlcParameters.ucLumaCurve[14]-0xe7)+(g_DlcParameters.ucLumaCurve2[14]-0xe7);
        if (((WORD)g_ucTable[15] + g_DlcParameters.ucLumaCurve[15] + g_DlcParameters.ucLumaCurve2[15] - 0xF7 - 0xF7) < 0xFF)
            g_ucTable[15] = g_ucTable[15]+(g_DlcParameters.ucLumaCurve[15]-0xf7)+(g_DlcParameters.ucLumaCurve2[15]-0xf7);
        else
            g_ucTable[15] = 0xFF;

        // Check Over Flow & Under Flow
        if (g_ucTable[15] < 0xD0)
            g_ucTable[15] = 0xFF;
        if (g_ucTable[14] < 0x90)
            g_ucTable[14] = 0xFF;
        if (g_ucTable[13] < 0x70)
            g_ucTable[13] = 0xFF;
        if (g_ucTable[12] < 0x60)
            g_ucTable[12] = 0xFF;
        if (g_ucTable[11] < 0x50)
            g_ucTable[11] = 0xFF;
        if (g_ucTable[2] > 0x90)
            g_ucTable[2] = 0x0;
        if (g_ucTable[1] > 0x60)
            g_ucTable[1] = 0x0;
        if (g_ucTable[0] > 0x30)
            g_ucTable[0] = 0x0;
    }
    else  //New DLC  Algorithm
    {

        if((g_wLumiAverageTemp-Pre_TotalYSum)==0)
        {
            return;
        }
        else
        {
            ucYAvg = msGetAverageLuminous();

            for (ucTmp=0; ucTmp<32; ucTmp++)
            {
                if(ucTmp==0)
                {
                    uwHistogramAreaSum[ucTmp]=g_wLumaHistogram32H[ucTmp];
                }
                else
                {
                    uwHistogramAreaSum[ucTmp] = uwHistogramAreaSum[ucTmp-1] + g_wLumaHistogram32H[ucTmp];
                }
            }

            //Mantis issue of 0232938:Hsense A3 DLC uwHistogramAreaSum[31]=0 will code dump, the divisor can't to be equal to zero .
            if(uwHistogramAreaSum[31]==0)
            {
                DLC_DEBUG(printf("\n Error in [New DLC Algorithm][ %s  , %d ]  uwHistogramAreaSum[31]=0 !!!\n", __FUNCTION__,__LINE__));
                return;
            }

            if(ucYAvg >= g_DlcParameters.ucDlcYAvgThresholdH)
            {
                ucYAvgNormalize=g_DlcParameters.ucDlcSepPointL;
            }
            else if(ucYAvg <= g_DlcParameters.ucDlcYAvgThresholdL)
            {
                ucYAvgNormalize=g_DlcParameters.ucDlcSepPointH;
            }
            else
            {
                ucYAvgNormalize=((ucYAvg-g_DlcParameters.ucDlcYAvgThresholdL)*g_DlcParameters.ucDlcSepPointL
                                +(g_DlcParameters.ucDlcYAvgThresholdH-ucYAvg)*g_DlcParameters.ucDlcSepPointH)
                                /(g_DlcParameters.ucDlcYAvgThresholdH-g_DlcParameters.ucDlcYAvgThresholdL);
            }

            for (ucTmp=0; ucTmp<31; ucTmp++)
            {
                if(((ucTmp+1)*8)<ucYAvgNormalize)
                {
                    dwSumBelowAvg = uwHistogramAreaSum[ucTmp]
                                   +((ucYAvgNormalize-(ucTmp+1)*8)*g_wLumaHistogram32H[(ucTmp+1)]+4)/8;
                }
            }

            dwSumAboveAvg = uwHistogramAreaSum[31] - dwSumBelowAvg ;

            His_ratio_BelowAvg = (4096*dwSumBelowAvg + (uwHistogramAreaSum[31]/2))/uwHistogramAreaSum[31];
            His_ratio_AboveAvg = (4096*dwSumAboveAvg + (uwHistogramAreaSum[31]/2))/uwHistogramAreaSum[31];

            if(His_ratio_BelowAvg > 128)
            {
                His_ratio_BelowAvg = 128;
            }
            if(His_ratio_AboveAvg > 128)
            {
                His_ratio_AboveAvg = 128;
            }

            for (ucTmp=0; ucTmp<32; ucTmp++)
            {
                uwHistogram_BLE_AreaSum[ucTmp]=(WORD)(((1024*uwHistogramAreaSum[ucTmp])+uwHistogramAreaSum[31]/2)/uwHistogramAreaSum[31]);

                if(uwHistogram_BLE_AreaSum[ucTmp]>1024)
                {
                    uwHistogram_BLE_AreaSum[ucTmp]=1024;
                }
            }
            //uwHistogram_BLE_AreaSum[ucTmp] = histogram Add to normalize 1024

            if ((dwSumBelowAvg == 0)&&(dwSumAboveAvg != 0))
            {
                for (ucTmp = 0; ucTmp < 32; ucTmp++)
                {
                    if (ucYAvgNormalize >= (((WORD)ucTmp+1)*8))
                    {
                         uwHistogramAreaSum[ucTmp] = (ucTmp+1)*8*4;
                    }
                    else
                    {
                        uwHistogramAreaSum[ucTmp] = ucYAvgNormalize*4 + ((DWORD)uwHistogramAreaSum[ucTmp]*(1024-ucYAvgNormalize*4)+(dwSumAboveAvg/2))/dwSumAboveAvg;
                        if(uwHistogramAreaSum[ucTmp] >= (DWORD)((ucTmp+1)*32))
                        {
                            uwHistogramAreaSum[ucTmp] = ((ucTmp+1)*32) + (((uwHistogramAreaSum[ucTmp] - ((ucTmp+1)*32))*His_ratio_AboveAvg)/128);
                        }
                        else
                        {
                            uwHistogramAreaSum[ucTmp] = ((ucTmp+1)*32) - (((((ucTmp+1)*32) - uwHistogramAreaSum[ucTmp])*His_ratio_AboveAvg)/128);
                        }
                    }
                }
            }
            else if ((dwSumBelowAvg != 0)&&(dwSumAboveAvg == 0))
            {
                for (ucTmp = 0; ucTmp < 32; ucTmp++)
                {
                    if (ucYAvgNormalize >= (((WORD)ucTmp+1)*8))
                    {
                        uwHistogramAreaSum[ucTmp] = ((DWORD)uwHistogramAreaSum[ucTmp]*ucYAvgNormalize*4+(dwSumBelowAvg/2))/dwSumBelowAvg;
                        if(uwHistogramAreaSum[ucTmp] >= (DWORD)((ucTmp+1)*32))
                        {
                            uwHistogramAreaSum[ucTmp] = ((ucTmp+1)*32) + (((uwHistogramAreaSum[ucTmp] - ((ucTmp+1)*32))*His_ratio_BelowAvg)/128);
                        }
                        else
                        {
                            uwHistogramAreaSum[ucTmp] = ((ucTmp+1)*32) - (((((ucTmp+1)*32) - uwHistogramAreaSum[ucTmp])*His_ratio_BelowAvg)/128);
                        }
                    }
                    else
                    {
                         uwHistogramAreaSum[ucTmp] = (ucTmp+1)*8*4;
                    }
                }
            }
            else if((dwSumBelowAvg == 0)&&(dwSumAboveAvg == 0))
            {
                for (ucTmp = 0; ucTmp < 32; ucTmp++)
                {
                    uwHistogramAreaSum[ucTmp] = (ucTmp+1)*8*4;
                }
            }
            else
            {
                for (ucTmp = 0; ucTmp < 32; ucTmp++)
                {
                    if (ucYAvgNormalize >= (((WORD)ucTmp+1)*8))
                    {
                         uwHistogramAreaSum[ucTmp] = ((DWORD)uwHistogramAreaSum[ucTmp]*ucYAvgNormalize*4+(dwSumBelowAvg/2))/dwSumBelowAvg;
                         if(uwHistogramAreaSum[ucTmp] >= (DWORD)((ucTmp+1)*32))
                         {
                             uwHistogramAreaSum[ucTmp] = ((ucTmp+1)*32) + (((uwHistogramAreaSum[ucTmp] - ((ucTmp+1)*32))*His_ratio_BelowAvg)/128);
                         }
                         else
                         {
                             uwHistogramAreaSum[ucTmp] = ((ucTmp+1)*32) - (((((ucTmp+1)*32) - uwHistogramAreaSum[ucTmp])*His_ratio_BelowAvg)/128);
                         }
                    }
                    else
                    {
                        uwHistogramAreaSum[ucTmp] = ucYAvgNormalize*4 + ((uwHistogramAreaSum[ucTmp]-dwSumBelowAvg)*(1024-ucYAvgNormalize*4)+(dwSumAboveAvg/2))/dwSumAboveAvg;

                        if(uwHistogramAreaSum[ucTmp] >= (DWORD)((ucTmp+1)*32))
                        {
                            uwHistogramAreaSum[ucTmp] = ((ucTmp+1)*32) + (((uwHistogramAreaSum[ucTmp] - ((ucTmp+1)*32))*His_ratio_AboveAvg)/128);
                        }
                        else
                        {
                            uwHistogramAreaSum[ucTmp] = ((ucTmp+1)*32) - (((((ucTmp+1)*32) - uwHistogramAreaSum[ucTmp])*His_ratio_AboveAvg)/128);
                        }
                    }
                }
            }

            //down sampling to 16 numbers (store to uwHistogramAreaSum[0~15])

            for (ucTmp=0; ucTmp<16; ucTmp++)
            {
                uwHistogramAreaSum[ucTmp] = uwHistogramAreaSum[ucTmp*2]; //
            }

            //histogram_DLC limit by slope
            uwHistogramMax = 0;

            for (ucTmp=0; ucTmp<=16; ucTmp++)
            {
                if (ucTmp == 0)
                {
                    dwHistogramDlcSlop[ucTmp] = ((uwHistogramAreaSum[ucTmp] - 0) * 256 + 16) / 64;

                    if (dwHistogramDlcSlop[ucTmp] < 257)
                    {
                        dwHistogramDlcSlop[ucTmp] = 257;
                    }
                }
                else if (ucTmp == 16)
                {
                    dwHistogramDlcSlop[ucTmp] = ((1024 - uwHistogramAreaSum[ucTmp-1]) * 256 + 16) / 64;

                    if (dwHistogramDlcSlop[ucTmp] < 257)
                    {
                        dwHistogramDlcSlop[ucTmp] = 257;
                    }
                }
                else
                {
                    dwHistogramDlcSlop[ucTmp] = ((uwHistogramAreaSum[ucTmp] - uwHistogramAreaSum[ucTmp-1]) * 256 + 32) / 64; //find slop

                    if (dwHistogramDlcSlop[ucTmp] < 257)
                    {
                        dwHistogramDlcSlop[ucTmp] = 257; //(only take the slop > 1)
                    }
                }

                dwHistogramDlcSlop[ucTmp] = ((g_DlcParameters.ucDlcHistogramLimitCurve[ucTmp]*512)+((dwHistogramDlcSlop[ucTmp]-256)/2)) / (dwHistogramDlcSlop[ucTmp]-256) ; //SlopRatio 8bit 256=1x

                if(dwHistogramDlcSlop[ucTmp] > 0xFFFF)
                {
                    dwHistogramDlcSlop[ucTmp] = 0xFFFF;
                }
            }

            sepPoint = (ucYAvgNormalize-8)/16;
            sepPoint0 = sepPoint-1;
            sepPoint1 = sepPoint;
            sepPoint2 = sepPoint+1;

            if(ucYAvgNormalize < (sepPoint2*16))
            {
                //sepPoint0 = sepPoint;
                //sepPoint1 = sepPoint+1;
                //sepPoint2 = sepPoint+2;
                coeff0 = (((3*((sepPoint2*16)-ucYAvgNormalize)*((sepPoint2*16)-ucYAvgNormalize))+64*((sepPoint2*16)-ucYAvgNormalize)+320));
                coeff1 = ((1408-6*((sepPoint2*16)-ucYAvgNormalize)*((sepPoint2*16)-ucYAvgNormalize)));
                coeff2 = 2048-coeff0-coeff1;
            }
            else
            {
                //sepPoint0 = sepPoint-1;
                //sepPoint1 = sepPoint;
                //sepPoint2 = sepPoint+1;
                coeff0 = (((3*(ucYAvgNormalize-(sepPoint2*16))*(ucYAvgNormalize-(sepPoint2*16)))-64*(ucYAvgNormalize-(sepPoint2*16))+320));
                coeff1 = ((1408-6*(ucYAvgNormalize-(sepPoint2*16))*(ucYAvgNormalize-(sepPoint2*16))));
                coeff2 = 2048-coeff0-coeff1;
            }

            //Calculate for sepPoint0~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            uwHistogramSlopRatioL = 256;
            uwHistogramSlopRatioH = 256;

            for (ucTmp = 0; ucTmp<= (sepPoint0+1); ucTmp++)
            {
                uwHistogramMax2nd = (WORD)dwHistogramDlcSlop[ucTmp];

                if (uwHistogramMax2nd < uwHistogramSlopRatioL)
                {
                    uwHistogramSlopRatioL = uwHistogramMax2nd;
                }
            }

            for (ucTmp = sepPoint0; ucTmp<=16; ucTmp++)
            {
                uwHistogramMax2nd = (WORD)dwHistogramDlcSlop[ucTmp];

                if (uwHistogramMax2nd < uwHistogramSlopRatioH)
                {
                    uwHistogramSlopRatioH = uwHistogramMax2nd;
                }
            }

            // calcute final target curve for sepPoint0
            for (ucTmp = 0; ucTmp < 16; ucTmp++)
            {
                if (sepPoint0 >= (WORD)ucTmp)
                {
                    if(uwHistogramAreaSum[ucTmp]>(DWORD)(ucTmp * 64 + 32))
                    {
                        g_uwCurveHistogram0[ucTmp] = (ucTmp * 64 + 32) + (((uwHistogramAreaSum[ucTmp] - (ucTmp * 64 + 32)) * uwHistogramSlopRatioL + 128) / 256 ) ;
                    }
                    else
                    {
                        g_uwCurveHistogram0[ucTmp] = (ucTmp * 64 + 32) - ((((ucTmp * 64 + 32)- uwHistogramAreaSum[ucTmp]) * uwHistogramSlopRatioL + 128) / 256);
                    }
                }
                else
                {
                    if(uwHistogramAreaSum[ucTmp]>(DWORD)(ucTmp * 64 + 32))
                    {
                        g_uwCurveHistogram0[ucTmp] = (ucTmp * 64 + 32) + (((uwHistogramAreaSum[ucTmp] - (ucTmp * 64 + 32)) * uwHistogramSlopRatioH + 128) / 256 ) ;
                    }
                    else
                    {
                        g_uwCurveHistogram0[ucTmp] = (ucTmp * 64 + 32) - ((((ucTmp * 64 + 32)- uwHistogramAreaSum[ucTmp]) * uwHistogramSlopRatioH + 128) / 256);
                    }
                }
            }

            //Calculate for sepPoint1~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            uwHistogramSlopRatioL = 256;
            uwHistogramSlopRatioH = 256;

            for (ucTmp = 0; ucTmp<= (sepPoint1+1); ucTmp++)
            {
                uwHistogramMax2nd = (WORD)dwHistogramDlcSlop[ucTmp];

                if (uwHistogramMax2nd < uwHistogramSlopRatioL)
                {
                    uwHistogramSlopRatioL = uwHistogramMax2nd;
                }
            }

            for (ucTmp = sepPoint1; ucTmp<=16; ucTmp++)
            {
                uwHistogramMax2nd = (WORD)dwHistogramDlcSlop[ucTmp];

                if (uwHistogramMax2nd < uwHistogramSlopRatioH)
                {
                    uwHistogramSlopRatioH = uwHistogramMax2nd;
                }
            }

            // calcute final target curve for sepPoint1
            for (ucTmp = 0; ucTmp < 16; ucTmp++)
            {
                if (sepPoint1 >= (WORD)ucTmp)
                {
                    if(uwHistogramAreaSum[ucTmp]>(DWORD)(ucTmp * 64 + 32))
                    {
                        g_uwCurveHistogram1[ucTmp] = (ucTmp * 64 + 32) + (((uwHistogramAreaSum[ucTmp] - (ucTmp * 64 + 32)) * uwHistogramSlopRatioL + 128) / 256 );
                    }
                    else
                    {
                        g_uwCurveHistogram1[ucTmp] = (ucTmp * 64 + 32) - ((((ucTmp * 64 + 32)- uwHistogramAreaSum[ucTmp]) * uwHistogramSlopRatioL + 128) / 256 );
                    }
                }
                else
                {
                    if(uwHistogramAreaSum[ucTmp]>(DWORD)(ucTmp * 64 + 32))
                    {
                        g_uwCurveHistogram1[ucTmp] = (ucTmp * 64 + 32) + (((uwHistogramAreaSum[ucTmp] - (ucTmp * 64 + 32)) * uwHistogramSlopRatioH + 128) / 256 );
                    }
                    else
                    {
                        g_uwCurveHistogram1[ucTmp] = (ucTmp * 64 + 32) - ((((ucTmp * 64 + 32)- uwHistogramAreaSum[ucTmp]) * uwHistogramSlopRatioH + 128) / 256 );
                    }
                }
            }

            //Calculate for sepPoint2~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            uwHistogramSlopRatioL = 256;
            uwHistogramSlopRatioH = 256;

            for (ucTmp = 0; ucTmp<= (sepPoint2+1); ucTmp++)
            {
                uwHistogramMax2nd = (WORD)dwHistogramDlcSlop[ucTmp];

                if (uwHistogramMax2nd < uwHistogramSlopRatioL)
                {
                    uwHistogramSlopRatioL = uwHistogramMax2nd;
                }
            }

            for (ucTmp = sepPoint2; ucTmp<=16; ucTmp++)
            {
                uwHistogramMax2nd = (WORD)dwHistogramDlcSlop[ucTmp];

                if (uwHistogramMax2nd < uwHistogramSlopRatioH)
                {
                    uwHistogramSlopRatioH = uwHistogramMax2nd;
                }
            }

            // calcute final target curve for sepPoint2
            for (ucTmp = 0; ucTmp < 16; ucTmp++)
            {
                if (sepPoint2 >= (WORD)ucTmp)
                {
                    if(uwHistogramAreaSum[ucTmp]>(DWORD)(ucTmp * 64 + 32))
                    {
                        g_uwCurveHistogram2[ucTmp] = (ucTmp * 64 + 32) + (((uwHistogramAreaSum[ucTmp] - (ucTmp * 64 + 32)) * uwHistogramSlopRatioL + 128) / 256 );
                    }
                    else
                    {
                        g_uwCurveHistogram2[ucTmp] = (ucTmp * 64 + 32) - ((((ucTmp * 64 + 32)- uwHistogramAreaSum[ucTmp]) * uwHistogramSlopRatioL + 128) / 256 );
                    }
                }
                else
                {
                    if(uwHistogramAreaSum[ucTmp]>(DWORD)(ucTmp * 64 + 32))
                    {
                        g_uwCurveHistogram2[ucTmp] = (ucTmp * 64 + 32) + (((uwHistogramAreaSum[ucTmp] - (ucTmp * 64 + 32)) * uwHistogramSlopRatioH + 128) / 256 );
                    }
                    else
                    {
                        g_uwCurveHistogram2[ucTmp] = (ucTmp * 64 + 32) - ((((ucTmp * 64 + 32)- uwHistogramAreaSum[ucTmp]) * uwHistogramSlopRatioH + 128) / 256);
                    }
                }
            }

            for (ucTmp = 0; ucTmp < 16; ucTmp++)
            {
                g_uwCurveHistogram[ucTmp] = (coeff0*g_uwCurveHistogram0[ucTmp]+coeff1*g_uwCurveHistogram1[ucTmp]+coeff2*g_uwCurveHistogram2[ucTmp]+1024)/2048;
            }
            ///////////// Histogram_DLC add Low_Pass_filter /////////////

            for (ucTmp = 0; ucTmp < 16; ucTmp++)
            {
                if (ucTmp == 0)
                {
                    g_uwCurveHistogram[ucTmp] = (5*g_uwCurveHistogram[ucTmp] + g_uwCurveHistogram[ucTmp+1]+4)/8;
                }
                else if (ucTmp == 15)
                {
                    g_uwCurveHistogram[ucTmp] = (g_uwCurveHistogram[ucTmp-1] + 5*g_uwCurveHistogram[ucTmp] + 2048 + 4)/8;
                }
                else
                {
                    g_uwCurveHistogram[ucTmp] = (g_uwCurveHistogram[ucTmp-1] + 6*g_uwCurveHistogram[ucTmp] + g_uwCurveHistogram[ucTmp+1]+4)/8;
                }
            }


#if 0  //Ranma Histogram DLC low pass
            for (ucTmp = 0; ucTmp < 16; ucTmp++)
            {
                uwHistogramAreaSum[ucTmp+16] = g_uwCurveHistogram[ucTmp];

                if (ucTmp == sepPoint0)
                {
                    uwHistogramAreaSum[ucTmp+16] = (g_uwCurveHistogram[ucTmp-1]+g_uwCurveHistogram[ucTmp]*2+g_uwCurveHistogram[ucTmp+1]+2)/4;
                    uwHistogramAreaSum[ucTmp] = coeff0*uwHistogramAreaSum[ucTmp+16]+(2048-coeff0)*uwHistogramAreaSum[ucTmp];
                }
                if (ucTmp == sepPoint1)
                {
                    uwHistogramAreaSum[ucTmp+16] = (g_uwCurveHistogram[ucTmp-2]+g_uwCurveHistogram[ucTmp-1]*2+g_uwCurveHistogram[ucTmp]*2+g_uwCurveHistogram[ucTmp+1]*2+g_uwCurveHistogram[ucTmp+2]+4)/8;
                    uwHistogramAreaSum[ucTmp] = coeff1*uwHistogramAreaSum[ucTmp+16]+(2048-coeff1)*uwHistogramAreaSum[ucTmp];
                }
                if (ucTmp == sepPoint2)
                {
                    uwHistogramAreaSum[ucTmp+16] = (g_uwCurveHistogram[ucTmp-1]+g_uwCurveHistogram[ucTmp]*2+g_uwCurveHistogram[ucTmp+1]+2)/4;
                    uwHistogramAreaSum[ucTmp] = coeff2*uwHistogramAreaSum[ucTmp+16]+(2048-coeff2)*uwHistogramAreaSum[ucTmp];
                }
            }

            LpfBlendAlpha_temp1 = abs(g_uwCurveHistogram[sepPoint0]-g_uwCurveHistogram[sepPoint1]*2+g_uwCurveHistogram[sepPoint2]);
            LpfBlendAlpha_temp2 = abs(g_uwCurveHistogram[sepPoint1]-g_uwCurveHistogram[sepPoint2]*2+g_uwCurveHistogram[sepPoint2+1]);

            if(LpfBlendAlpha_temp1 > LpfBlendAlpha_temp2)
            {
                LpfBlendAlpha = LpfBlendAlpha_temp1;
            }
            else
            {
                LpfBlendAlpha = LpfBlendAlpha_temp2;
            }

            if (LpfBlendAlpha >64)
            {
                LpfBlendAlpha = 64;
            }

            for (ucTmp = 0; ucTmp < 16; ucTmp++)
            {
                g_uwCurveHistogram[ucTmp] = (uwHistogramAreaSum[ucTmp+16]*LpfBlendAlpha + (64-LpfBlendAlpha)*g_uwCurveHistogram[ucTmp]+32)/64;

                if(g_uwCurveHistogram[ucTmp]>1023)
                {
                    g_uwCurveHistogram[ucTmp]=1023;
                }
            }

#endif
            // Calculate curve by Yavg

            //DLC_PRINTF("\r\n ucYAvg = %d",ucYAvg);
            if (ucYAvg <= g_DlcParameters.ucDlcYAvgThresholdL)
            {
                for (ucTmp=0; ucTmp<16; ucTmp++)
                {
                    g_ucCurveYAvg[ucTmp] = g_DlcParameters.ucLumaCurve2_a[ucTmp];
                }
            }
            else if (ucYAvg <= g_DlcParameters.ucDlcYAvgThresholdM)
            {
                for (ucTmp=0; ucTmp<16; ucTmp++)
                {
                    g_ucCurveYAvg[ucTmp] = (((WORD)g_DlcParameters.ucLumaCurve[ucTmp] * (ucYAvg - g_DlcParameters.ucDlcYAvgThresholdL)) / (g_DlcParameters.ucDlcYAvgThresholdM - g_DlcParameters.ucDlcYAvgThresholdL)) +
                                           (g_DlcParameters.ucLumaCurve2_a[ucTmp] - (((WORD)g_DlcParameters.ucLumaCurve2_a[ucTmp] * (ucYAvg - g_DlcParameters.ucDlcYAvgThresholdL)) / ( g_DlcParameters.ucDlcYAvgThresholdM - g_DlcParameters.ucDlcYAvgThresholdL)));
                }
            }
            else if (ucYAvg <= g_DlcParameters.ucDlcYAvgThresholdH)
            {
                for (ucTmp=0; ucTmp<16; ucTmp++)
                {
                    g_ucCurveYAvg[ucTmp] = ((WORD)g_DlcParameters.ucLumaCurve2_b[ucTmp] * (ucYAvg - g_DlcParameters.ucDlcYAvgThresholdM) / (g_DlcParameters.ucDlcYAvgThresholdH - g_DlcParameters.ucDlcYAvgThresholdM)) +
                                           (g_DlcParameters.ucLumaCurve[ucTmp] - ((WORD)g_DlcParameters.ucLumaCurve[ucTmp] * (ucYAvg - g_DlcParameters.ucDlcYAvgThresholdM) / (g_DlcParameters.ucDlcYAvgThresholdH - g_DlcParameters.ucDlcYAvgThresholdM)));
                }
            }
            else // if (ucYAvg > g_DlcParameters.ucDlcYAvgThresholdH)
            {
                for (ucTmp=0; ucTmp<16; ucTmp++)
                {
                    g_ucCurveYAvg[ucTmp] = g_DlcParameters.ucLumaCurve2_b[ucTmp];
                }
            }

            belnging_factor = g_DlcParameters.ucDlcCurveModeMixAlpha;
            //CurveDiff_Coring_TH = g_DlcParameters.ucDlcCurveDiffCoringTH;


            // Decide Target Curve

            Diff_L =g_DlcParameters.ucDlcDiffBase_L;
            Diff_M =g_DlcParameters.ucDlcDiffBase_M;
            Diff_H =g_DlcParameters.ucDlcDiffBase_H;

            if (ucPre_YAvg*4 <= g_DlcParameters.ucDlcYAvgThresholdL*4)
            {
                Pre_YAvg_base = Diff_L*4;
            }
            else if (ucPre_YAvg*4 <= g_DlcParameters.ucDlcYAvgThresholdM*4)
            {
                if(Diff_M >= Diff_L)
                {
                    Pre_YAvg_base = Diff_L*4 + (((Diff_M-Diff_L)*(ucPre_YAvg*4-g_DlcParameters.ucDlcYAvgThresholdL*4))/(g_DlcParameters.ucDlcYAvgThresholdM- g_DlcParameters.ucDlcYAvgThresholdL));
                }
                else
                {
                    Pre_YAvg_base = Diff_L*4 - (((Diff_L-Diff_M)*(ucPre_YAvg*4-g_DlcParameters.ucDlcYAvgThresholdL*4))/(g_DlcParameters.ucDlcYAvgThresholdM- g_DlcParameters.ucDlcYAvgThresholdL));
                }
            }
            else if (ucPre_YAvg*4 <= g_DlcParameters.ucDlcYAvgThresholdH*4)
            {
                if(Diff_H >= Diff_M)
                {
                    Pre_YAvg_base = Diff_M*4 + (((Diff_H- Diff_M)*(ucPre_YAvg*4- g_DlcParameters.ucDlcYAvgThresholdM*4))/(g_DlcParameters.ucDlcYAvgThresholdH- g_DlcParameters.ucDlcYAvgThresholdM));
                }
                else
                {
                    Pre_YAvg_base = Diff_M*4 - (((Diff_M-Diff_H)*(ucPre_YAvg*4-g_DlcParameters.ucDlcYAvgThresholdM*4))/(g_DlcParameters.ucDlcYAvgThresholdH - g_DlcParameters.ucDlcYAvgThresholdM));
                }
            }
            else
            {
                Pre_YAvg_base = Diff_H*4;
            }

            if (ucYAvg*4 <= g_DlcParameters.ucDlcYAvgThresholdL*4)
            {
                YAvg_base = Diff_L*4;
            }
            else if (ucYAvg*4 <= g_DlcParameters.ucDlcYAvgThresholdM*4)
            {
                if(Diff_M >= Diff_L)
                {
                    YAvg_base = Diff_L*4 + (((Diff_M-Diff_L)*( ucYAvg*4-g_DlcParameters.ucDlcYAvgThresholdL*4))/(g_DlcParameters.ucDlcYAvgThresholdM- g_DlcParameters.ucDlcYAvgThresholdL));
                }
                else
                {
                    YAvg_base = Diff_L*4 - (((Diff_L-Diff_M)*( ucYAvg*4-g_DlcParameters.ucDlcYAvgThresholdL*4))/(g_DlcParameters.ucDlcYAvgThresholdM- g_DlcParameters.ucDlcYAvgThresholdL));
                }
            }
            else if (ucYAvg*4 <= g_DlcParameters.ucDlcYAvgThresholdH*4)
            {
                if(Diff_H >= Diff_M)
                {
                    YAvg_base = Diff_M*4 + (((Diff_H- Diff_M)*( ucYAvg*4- g_DlcParameters.ucDlcYAvgThresholdM*4))/(g_DlcParameters.ucDlcYAvgThresholdH- g_DlcParameters.ucDlcYAvgThresholdM));
                }
                else
                {
                    YAvg_base = Diff_M*4 - (((Diff_M-Diff_H)*(ucYAvg*4-g_DlcParameters.ucDlcYAvgThresholdM*4))/(g_DlcParameters.ucDlcYAvgThresholdH -g_DlcParameters.ucDlcYAvgThresholdM));
                }
            }
            else
            {
                YAvg_base = Diff_H*4;
            }

            YAvg_base = (Pre_YAvg_base + YAvg_base +1)/2;

            //Mantis issue of 0232938:Hsense A3 DLC the divisor can't to be equal to zero .
            if(YAvg_base==0)
            {
                DLC_DEBUG(printf("\n Error in  [New DLC Algorithm][ %s  , %d ]  YAvg_base=0 !!!\n", __FUNCTION__,__LINE__));
                return;
            }

            if (g_DlcParameters.ucDlcCurveMode == 0 ) //DLC_CURVE_MODE_BY_YAVG)
            {
                CurveDiff = g_DlcParameters.ucDlcCurveDiff_H_TH;
                for (ucTmp=0; ucTmp<16; ucTmp++)
                {
                    g_uwTable[ucTmp] = (g_ucCurveYAvg[ucTmp]<<2);
                }
            }
            else if (g_DlcParameters.ucDlcCurveMode == 1 ) //DLC_CURVE_MODE_BY_HISTOGRAM)
            {
                CurveDiff = g_DlcParameters.ucDlcCurveDiff_H_TH;
                for (ucTmp=0; ucTmp<16; ucTmp++)
                {
                    g_uwTable[ucTmp] = (WORD)g_uwCurveHistogram[ucTmp];
                }
            }
            else
            {
                CurveDiff = 0;
                CurveDiff_Coring = 0;
                for (ucTmp=0; ucTmp<16; ucTmp++)
                {
                    g_uwTable[ucTmp] = (WORD)((((WORD)(g_ucCurveYAvg[ucTmp]<<2) * belnging_factor) + ((g_uwCurveHistogram[ucTmp]) * (128 - belnging_factor))) / 128);
                    //g_uwTable[ucTmp] = ((g_uwTable[ucTmp]*(1024-Test_pattern_alpha))+((ucTmp*64+32)*Test_pattern_alpha)+512)/1024;

                    if (g_uwPre_CurveHistogram[ucTmp] <= g_DlcParameters.ucDlcYAvgThresholdL*4)
                    {
                        Pre_CurveDiff_base = Diff_L*4;
                    }
                    else if (g_uwPre_CurveHistogram[ucTmp] <= g_DlcParameters.ucDlcYAvgThresholdM*4)
                    {
                        if(Diff_M >= Diff_L)
                        {
                            Pre_CurveDiff_base = Diff_L*4 + (((Diff_M-Diff_L)*(g_uwPre_CurveHistogram[ucTmp]-g_DlcParameters.ucDlcYAvgThresholdL*4))/(g_DlcParameters.ucDlcYAvgThresholdM- g_DlcParameters.ucDlcYAvgThresholdL));
                        }
                        else
                        {
                            Pre_CurveDiff_base = Diff_L*4 - (((Diff_L-Diff_M)*(g_uwPre_CurveHistogram[ucTmp]-g_DlcParameters.ucDlcYAvgThresholdL*4))/(g_DlcParameters.ucDlcYAvgThresholdM- g_DlcParameters.ucDlcYAvgThresholdL));
                        }
                    }
                    else if (g_uwPre_CurveHistogram[ucTmp] <= g_DlcParameters.ucDlcYAvgThresholdH*4)
                    {
                        if(Diff_H >= Diff_M)
                        {
                            Pre_CurveDiff_base = Diff_M*4 + (((Diff_H- Diff_M)*(g_uwPre_CurveHistogram[ucTmp]- g_DlcParameters.ucDlcYAvgThresholdM*4))/(g_DlcParameters.ucDlcYAvgThresholdH- g_DlcParameters.ucDlcYAvgThresholdM));
                        }
                        else
                        {
                            Pre_CurveDiff_base = Diff_M*4 -(((Diff_M- Diff_H)*(g_uwPre_CurveHistogram[ucTmp]- g_DlcParameters.ucDlcYAvgThresholdM*4))/(g_DlcParameters.ucDlcYAvgThresholdH- g_DlcParameters.ucDlcYAvgThresholdM));
                        }
                    }
                    else
                    {
                        Pre_CurveDiff_base = Diff_H*4;
                    }

                    if (g_uwTable[ucTmp] <= g_DlcParameters.ucDlcYAvgThresholdL*4)
                    {
                        CurveDiff_base = Diff_L*4;
                    }
                    else if (g_uwTable[ucTmp] <= g_DlcParameters.ucDlcYAvgThresholdM*4)
                    {
                        if(Diff_M >= Diff_L)
                        {
                            CurveDiff_base = Diff_L*4 + (((Diff_M-Diff_L)*(g_uwTable[ucTmp]-g_DlcParameters.ucDlcYAvgThresholdL*4))/(g_DlcParameters.ucDlcYAvgThresholdM- g_DlcParameters.ucDlcYAvgThresholdL));
                        }
                        else
                        {
                            CurveDiff_base = Diff_L*4 - (((Diff_L-Diff_M)*(g_uwTable[ucTmp]-g_DlcParameters.ucDlcYAvgThresholdL*4))/(g_DlcParameters.ucDlcYAvgThresholdM- g_DlcParameters.ucDlcYAvgThresholdL));
                        }
                    }
                    else if (g_uwTable[ucTmp] <= g_DlcParameters.ucDlcYAvgThresholdH*4)
                    {
                        if(Diff_H >= Diff_M)
                        {
                            CurveDiff_base = Diff_M*4 + (((Diff_H- Diff_M)*(g_uwTable[ucTmp]- g_DlcParameters.ucDlcYAvgThresholdM*4))/(g_DlcParameters.ucDlcYAvgThresholdH- g_DlcParameters.ucDlcYAvgThresholdM));
                        }
                        else
                        {
                            CurveDiff_base = Diff_M*4 -(((Diff_M- Diff_H)*(g_uwTable[ucTmp]- g_DlcParameters.ucDlcYAvgThresholdM*4))/(g_DlcParameters.ucDlcYAvgThresholdH- g_DlcParameters.ucDlcYAvgThresholdM));
                        }
                    }
                    else
                    {
                        CurveDiff_base = Diff_H*4;
                    }

                    CurveDiff_base = (Pre_CurveDiff_base + CurveDiff_base+1)/2;

                    //Mantis issue of 0232938:Hsense A3 DLC the divisor can't to be equal to zero .
                    if(CurveDiff_base==0)
                    {
                        DLC_DEBUG(printf("\n Error in  [New DLC Algorithm][ %s  , %d ]  CurveDiff_base=0 !!!\n", __FUNCTION__,__LINE__));
                        return;
                    }

                    if(ucTmp*16+8 <= ucYAvgNormalize)
                    {
                        if (ucTmp == 0)
                        {
                            CurveDiff_Coring = (WORD)((His_ratio_BelowAvg*(1024-uwHistogram_BLE_AreaSum[1])*abs(g_uwTable[ucTmp]-g_uwPre_CurveHistogram[ucTmp]))/(15*CurveDiff_base*128));
                        }
                        else
                        {
                            CurveDiff_Coring = (WORD)((His_ratio_BelowAvg*(1024-(uwHistogram_BLE_AreaSum[ucTmp*2+1]- uwHistogram_BLE_AreaSum[ucTmp*2-1]))*abs(g_uwTable[ucTmp]-g_uwPre_CurveHistogram[ucTmp]))/(15*CurveDiff_base*128));
                        }
                    }
                    else
                    {
                        CurveDiff_Coring = (WORD)((His_ratio_AboveAvg*(1024-(uwHistogram_BLE_AreaSum[ucTmp*2+1]- uwHistogram_BLE_AreaSum[ucTmp*2-1]))*abs(g_uwTable[ucTmp]-g_uwPre_CurveHistogram[ucTmp]))/(15*CurveDiff_base*128));
                    }

                    CurveDiff_Coring = CurveDiff_Coring/6;

                    CurveDiff = CurveDiff + CurveDiff_Coring ;
                    g_uwPre_CurveHistogram[ucTmp] = g_uwTable[ucTmp];
                }
            }

            if(CurveDiff <= g_DlcParameters.ucDlcCurveDiff_L_TH)
            {
                g_ucDlcFlickAlpha_Max = g_DlcParameters.ucDlcAlphaBlendingMin;
            }
            else if(CurveDiff >= g_DlcParameters.ucDlcCurveDiff_H_TH)
            {
                g_ucDlcFlickAlpha_Max = g_DlcParameters.ucDlcAlphaBlendingMax; ;
            }
            else
            {
                g_ucDlcFlickAlpha_Max = (((g_DlcParameters.ucDlcAlphaBlendingMax-g_DlcParameters.ucDlcAlphaBlendingMin)*(CurveDiff - g_DlcParameters.ucDlcCurveDiff_L_TH))/
                                        (g_DlcParameters.ucDlcCurveDiff_H_TH - g_DlcParameters.ucDlcCurveDiff_L_TH))+g_DlcParameters.ucDlcAlphaBlendingMin;
            }

            Delta_YAvg = (96*4*abs(ucYAvg - ucPre_YAvg)+ YAvg_base/2)/YAvg_base;

            if(Delta_YAvg > 256)
            {
                Delta_YAvg = 256;
            }
            else
            {
                Delta_YAvg = Delta_YAvg;
            }

            YAvg_TH_H = g_DlcParameters.ucDlcYAVG_H_TH;
            YAvg_TH_L = g_DlcParameters.ucDlcYAVG_L_TH;

            if(Delta_YAvg < YAvg_TH_L)
            {
                g_ucDlcFlickAlpha_Min = g_DlcParameters.ucDlcAlphaBlendingMin;
            }
            else if(Delta_YAvg >= YAvg_TH_H)
            {
                g_ucDlcFlickAlpha_Min = g_DlcParameters.ucDlcAlphaBlendingMax;
            }
            else
            {
                g_ucDlcFlickAlpha_Min = ((Delta_YAvg- YAvg_TH_L)*( g_DlcParameters.ucDlcAlphaBlendingMax- g_DlcParameters.ucDlcAlphaBlendingMin)
                                        + ( YAvg_TH_H-YAvg_TH_L)/2)/( YAvg_TH_H-YAvg_TH_L)+g_DlcParameters.ucDlcAlphaBlendingMin;
            }

            ucPre_YAvg = (BYTE)ucYAvg;

            if (g_ucDlcFlickAlpha_Max > g_ucDlcFlickAlpha_Min)
            {
                g_ucDlcFlickAlpha_Max = g_ucDlcFlickAlpha_Max;
                g_ucDlcFlickAlpha_Min = g_ucDlcFlickAlpha_Min;
            }
            else
            {
                g_ucDlcFlickAlpha_temp = g_ucDlcFlickAlpha_Max;
                g_ucDlcFlickAlpha_Max = g_ucDlcFlickAlpha_Min;
                g_ucDlcFlickAlpha_Min = g_ucDlcFlickAlpha_temp;
            }

            g_ucDlcFlickAlpha  = ( g_DlcParameters.ucDlcFlicker_alpha*g_ucDlcFlickAlpha_Max + (256- g_DlcParameters.ucDlcFlicker_alpha)*g_ucDlcFlickAlpha_Min+128)/256;

#if 0
            if(( g_ucDlcFlickAlpha_Max-g_ucDlcFlickAlpha_Min )< 0x38 )
            {
                g_ucDlcFlickAlpha = g_ucDlcFlickAlpha;
            }
            else if(( g_ucDlcFlickAlpha_Max-g_ucDlcFlickAlpha_Min )>0x60)
            {
                g_ucDlcFlickAlpha = g_ucDlcFlickAlpha_Max;
            }
            else
            {
                g_ucDlcFlickAlpha = ((((g_ucDlcFlickAlpha_Max-g_ucDlcFlickAlpha_Min) - 0x38)*g_ucDlcFlickAlpha_Max)
                                    +((0x60 - (g_ucDlcFlickAlpha_Max-g_ucDlcFlickAlpha_Min))*g_ucDlcFlickAlpha))/(0x60-0x38);
            }
#endif
        }
    }

    if( g_DlcParameters.bEnableBLE )
    {
        BYTE uc_cut,uc_lowbound,uc_highbound,i;

        // combine BLE with software
    // BLE - get cut point
        uc_cut = g_ucHistogramMin;
        //uc_lowbound = 0x07;
        //uc_highbound = 0x0f;
        uc_highbound = msReadByte( REG_ADDR_BLE_UPPER_BOND );
        uc_lowbound = msReadByte( REG_ADDR_BLE_LOWER_BOND);

        if (uc_cut < uc_lowbound )
        {
            uc_cut = uc_lowbound;
        }
        else if ( uc_cut > uc_highbound )
        {
            uc_cut = uc_highbound;
        }
        else
        {
            uc_cut = uc_cut;
        }

        // BLE - calculation
        for(i=0; i<8; ++i)
        {
            if (g_ucTable[i] < uc_cut)
            {
                g_ucTable[i] = 0;
            }
            else if (g_ucTable[i] > (uc_cut+g_DlcParameters.ucDlcBLEPoint))
            {
                ;
            }
            else
            {
                g_ucTable[i] = ((WORD)(g_ucTable[i]-uc_cut))*(uc_cut+g_DlcParameters.ucDlcBLEPoint)/g_DlcParameters.ucDlcBLEPoint;
            }
        }
    }


    if( g_DlcParameters.bEnableWLE )
    {
        BYTE i,uc_wcut,uc_wlowbound,uc_whighbound;
        // combine BLE with software
        // WLE - get cut point
        uc_wcut = g_ucHistogramMax;
        //uc_wlowbound = 0xff;
        //uc_whighbound = 0xf0;
        uc_whighbound = (msReadByte( REG_ADDR_WLE_UPPER_BOND ) & 0x3F) + 0xC0; // bit offset 0x00~0x3F -> 0xC0~0xFF
        uc_wlowbound = (msReadByte( REG_ADDR_WLE_LOWER_BOND) & 0x3F) + 0xC0; // bit offset 0x00~0x3F -> 0xC0~0xFF

        if (uc_wcut < uc_wlowbound )
        {
            uc_wcut = uc_wlowbound;
        }
        else if ( uc_wcut > uc_whighbound )
        {
            uc_wcut = uc_whighbound;
        }
        else
        {
            uc_wcut = uc_wcut;
        }

        // WLE - calculation
        for(i=0; i<8; ++i)
        {
            if (g_ucTable[15-i] > uc_wcut)
            {
            g_ucTable[15-i] = 0xFF;
            }
            else if (g_ucTable[15-i] < (uc_wcut-g_DlcParameters.ucDlcWLEPoint))
            {
                ;
            }
            else
            {
                g_ucTable[15-i] = 0xFF - (((WORD)(uc_wcut-g_ucTable[15-i]))*(0xFF-uc_wcut+g_DlcParameters.ucDlcWLEPoint)/g_DlcParameters.ucDlcWLEPoint);
            }
        }
    }

    // Level Limit
    if (g_DlcParameters.ucDlcLevelLimit)
    {
        if ((g_DlcParameters.ucDlcLevelLimit >= 1) && (g_ucTable[15] >= 0xF7))
        {
            g_ucTable[15] = 0xF7;
            if (g_ucTable[14] > 0xF7)
            {
                g_ucTable[14] = 0xF7;
            }

            if (g_ucTable[13] > 0xF7)
            {
                g_ucTable[13] = 0xF7;
            }
        }

        if ((g_DlcParameters.ucDlcLevelLimit >= 2) && (g_ucTable[14] >= 0xE7))
        {
            g_ucTable[14] = 0xE7;
            if (g_ucTable[13] > 0xE7)
            {
                g_ucTable[13] = 0xE7;
            }

            if (g_ucTable[12] > 0xE7)
            {
                g_ucTable[12] = 0xE7;
            }
        }

        if ((g_DlcParameters.ucDlcLevelLimit >= 3) && (g_ucTable[13] >= 0xD7))
        {
            g_ucTable[13] = 0xD7;
            if (g_ucTable[12] > 0xD7)
            {
                g_ucTable[12] = 0xD7;
            }

            if (g_ucTable[11] > 0xD7)
            {
                g_ucTable[11] = 0xD7;
            }
        }

        if ((g_DlcParameters.ucDlcLevelLimit >= 4) && (g_ucTable[12] >= 0xC7))
        {
            g_ucTable[12] = 0xC7;
            if (g_ucTable[11] > 0xC7)
            {
                g_ucTable[11] = 0xC7;
            }

            if (g_ucTable[10] > 0xC7)
            {
                g_ucTable[10] = 0xC7;
            }
        }
    }

    if((g_wLumiAverageTemp-Pre_TotalYSum)!=0)
    {

        if ((g_ucDlcFlickAlpha < (5*Pre_g_ucDlcFlickAlpha /8)) && (ucDlcFlickAlpha_Diff < (g_DlcParameters.ucDlcAlphaBlendingMax- g_DlcParameters.ucDlcAlphaBlendingMin)/2))
        {
            g_ucDlcFlickAlpha = 2*(ucDlcFlickAlpha_Diff* g_ucDlcFlickAlpha +(( g_DlcParameters.ucDlcAlphaBlendingMax
                                  - g_DlcParameters.ucDlcAlphaBlendingMin)/2- ucDlcFlickAlpha_Diff)* (5*Pre_g_ucDlcFlickAlpha /8))
                                  /( g_DlcParameters.ucDlcAlphaBlendingMax- g_DlcParameters.ucDlcAlphaBlendingMin);
        }

        Pre_g_ucDlcFlickAlpha = g_ucDlcFlickAlpha;
        ucDlcFlickAlpha_Diff = g_ucDlcFlickAlpha_Max-g_ucDlcFlickAlpha_Min;

        // Write data to luma curve ...
        msDlcWriteCurve(bWindow);
        if(g_bSetDLCCurveBoth)
        {
            msDlcWriteCurve(SUB_WINDOW);
        }

        if(g_DlcParameters.ucDlcAlgorithmMode == XC_DLC_ALGORITHM_NEW)
        {

            //Dyanmic BLE Function
            //~~~~~~~~~~~~~~~~~Calculate BLE_StartPoint~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

            BLE_StartPoint = ((ucYAvg*4) + g_DlcParameters.uwDlcBleStartPointTH - 512);

            BLE_StartPoint_Correction = (BLE_StartPoint+2)/4;

            if(BLE_StartPoint_Correction >(Pre_BLE_StartPoint_Correction/16))
            {
                if((BLE_StartPoint_Correction <= ((Pre_BLE_StartPoint_Correction/16)+48)) && (g_ucDlcFlickAlpha==g_DlcParameters.ucDlcAlphaBlendingMin))
                {
                    uwBLE_StartPoint_Blend = (WORD)(((16*(DWORD)BLE_StartPoint_Correction)+(63*(DWORD)Pre_BLE_StartPoint_Correction)+63)/64);
                    Pre_BLE_StartPoint_Correction = uwBLE_StartPoint_Blend;
                    uwBLE_StartPoint_Blend = (uwBLE_StartPoint_Blend+8)/16;
                }
                else
                {
                    uwBLE_StartPoint_Blend = (WORD)((((DWORD) g_ucDlcFlickAlpha * (DWORD)BLE_StartPoint_Correction) +
                                             ((128 - (DWORD)g_ucDlcFlickAlpha) * (DWORD)(Pre_BLE_StartPoint_Correction/16))+127)/128);
                    Pre_BLE_StartPoint_Correction = uwBLE_StartPoint_Blend*16;
                }
            }
            else
            {
                if((BLE_StartPoint_Correction >= ((Pre_BLE_StartPoint_Correction/16)-48))&&(g_ucDlcFlickAlpha==g_DlcParameters.ucDlcAlphaBlendingMin))
                {
                    uwBLE_StartPoint_Blend = (WORD)(((16*(DWORD)BLE_StartPoint_Correction)+(63*(DWORD)Pre_BLE_StartPoint_Correction))/64);
                    Pre_BLE_StartPoint_Correction = uwBLE_StartPoint_Blend;
                    uwBLE_StartPoint_Blend = (uwBLE_StartPoint_Blend+8)/16;
                }
                else
                {
                    uwBLE_StartPoint_Blend = (WORD)((((DWORD) g_ucDlcFlickAlpha * (DWORD)BLE_StartPoint_Correction) +
                                             ((128 - (DWORD)g_ucDlcFlickAlpha) * (DWORD)(Pre_BLE_StartPoint_Correction/16)))/128);
                    Pre_BLE_StartPoint_Correction = uwBLE_StartPoint_Blend*16;
                }
            }

            //~~~~~~~~~output = uwBLE_StartPoint_Blend & Pre_BLE_StartPoint_Correction ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

            //~~~~~~~~~~~~~~~~~Calculate  BLE_EndPoint ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

            for (ucTmp=0; ucTmp<32; ucTmp++)
            {
                if(ucTmp==0)
                {
                    uwHistogramAreaSum[ucTmp]=g_wLumaHistogram32H[ucTmp];
                }
                else
                {
                    uwHistogramAreaSum[ucTmp] = uwHistogramAreaSum[ucTmp-1] + g_wLumaHistogram32H[ucTmp];
                }
            }

            ucTmp=0;
            while( uwHistogramAreaSum[ucTmp] < uwHistogramAreaSum[31]/8)
            {
                ucTmp++;
            }

            if(ucTmp==0)
            {
                BLE_EndPoint = (WORD)((g_uwTable[0]*uwHistogramAreaSum[31])/(16*g_wLumaHistogram32H[0]));
            }
            else
            {
                g_wLumaHistogram32H[ucTmp+1] = (WORD)((uwHistogramAreaSum[31]/8) - uwHistogramAreaSum[ucTmp-1]);

                for (Tmp=0; Tmp<30; Tmp++)
                {
                    uwHistogramAreaSum[Tmp] = (g_uwTable[Tmp/2]+g_uwTable[(Tmp+1)/2]+1)/2;
                }

                for (Tmp=0; Tmp<(ucTmp-1); Tmp++)
                {
                    if(Tmp==0)
                    {
                        BLE_sum = g_wLumaHistogram32H[0]*(g_uwTable[0]+1/2);
                    }
                    else
                    {
                        BLE_sum = BLE_sum + g_wLumaHistogram32H[Tmp] * ((uwHistogramAreaSum[Tmp]+uwHistogramAreaSum[Tmp-1]+1)/2);
                    }
                }

                //Mantis issue of 0232938:Hsense A3 DLC the divisor can't to be equal to zero .
                if(g_wLumaHistogram32H[ucTmp]==0)
                {
                    DLC_DEBUG(printf("\n Error in  [New DLC Algorithm][ %s  , %d ]  g_wLumaHistogram32H[%d]=0 !!!\n", __FUNCTION__,__LINE__,ucTmp));
                    return;
                }

                BLE_sum = BLE_sum + g_wLumaHistogram32H[ucTmp+1] * ((((((uwHistogramAreaSum[ucTmp] - uwHistogramAreaSum[ucTmp-1])*g_wLumaHistogram32H[ucTmp+1])/g_wLumaHistogram32H[ucTmp])+1)/2) + uwHistogramAreaSum[ucTmp-1]);

                BLE_EndPoint = (WORD)((BLE_sum*8)/uwHistogramAreaSum[31]);
            }

            BLE_EndPoint = (BLE_EndPoint*g_DlcParameters.uwDlcBleEndPointTH+512)/1024;
            BLE_EndPoint_Correction = (BLE_EndPoint+2)/4;


            //~~~~~~~~~output = BLE_EndPoint Correction ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

            //~~~~~~~~~Used uwBLE_StartPoint_Blend & BLE_EndPoint_Correction Calculate BLE_Slop ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

            if(uwBLE_StartPoint_Blend > 0x7F)
            {
                uwBLE_StartPoint_Blend = 0x7F;
            }

            if( uwBLE_StartPoint_Blend > BLE_EndPoint_Correction)
            {
                BLE_Slop = (1024*uwBLE_StartPoint_Blend+((uwBLE_StartPoint_Blend - BLE_EndPoint_Correction)/2))/(uwBLE_StartPoint_Blend - BLE_EndPoint_Correction);
            }
            else
            {
                BLE_Slop = 0x7FF;
            }

            if(BLE_Slop > 0x7FF)
            {
                BLE_Slop = 0x7FF;
            }

            if(BLE_Slop < 0x400)
            {
                BLE_Slop = 0x400;
            }

            BLE_Slop_400 = g_DlcParameters.uwDlcBLESlopPoint_1;//(msReadByte(L_BK_DLC(0x38)))<<3;
            BLE_Slop_480 = g_DlcParameters.uwDlcBLESlopPoint_2;//(msReadByte(H_BK_DLC(0x38)))<<3;
            BLE_Slop_500 = g_DlcParameters.uwDlcBLESlopPoint_3;//(msReadByte(L_BK_DLC(0x39)))<<3;
            BLE_Slop_600 = g_DlcParameters.uwDlcBLESlopPoint_4;//(msReadByte(H_BK_DLC(0x39)))<<3;
            BLE_Slop_800 = g_DlcParameters.uwDlcBLESlopPoint_5;//(msReadByte(L_BK_DLC(0x3A)))<<3;
            Dark_BLE_Slop_Min = g_DlcParameters.uwDlcDark_BLE_Slop_Min;//(msReadByte(L_BK_DLC(0x3B)))<<3;

            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            if(BLE_Slop <= 0x480)
            {
                BLE_Slop =((BLE_Slop_480-BLE_Slop_400)*(BLE_Slop-0x400)+64)/128+BLE_Slop_400;
            }
            else if ((BLE_Slop > 0x480) && (BLE_Slop <= 0x500))
            {
                BLE_Slop =((BLE_Slop_500-BLE_Slop_480)*(BLE_Slop-0x480)+64)/128+BLE_Slop_480;
            }
            else if ((BLE_Slop > 0x500) && (BLE_Slop <= 0x600))
            {
                BLE_Slop =((BLE_Slop_600-BLE_Slop_500)*(BLE_Slop-0x500)+128)/256+BLE_Slop_500;
            }
            else
            {
                BLE_Slop =((BLE_Slop_800-BLE_Slop_600)*(BLE_Slop-0x600)+256)/512+BLE_Slop_600;
            }

            if (ucYAvg < ((g_DlcParameters.ucDlcYAvgThresholdL+g_DlcParameters.ucDlcYAvgThresholdM)/2))
            {
                BLE_Slop_tmp =(((Dark_BLE_Slop_Min - BLE_Slop_400)*(((g_DlcParameters.ucDlcYAvgThresholdL+g_DlcParameters.ucDlcYAvgThresholdM)/2)-ucYAvg)/
                              ((g_DlcParameters.ucDlcYAvgThresholdL+g_DlcParameters.ucDlcYAvgThresholdM)/2)) + BLE_Slop_400);

                if(BLE_Slop > BLE_Slop_tmp)
                {
                    BLE_Slop = BLE_Slop;
                }
                else
                {
                    BLE_Slop = BLE_Slop_tmp;
                }
            }
            else
            {
                BLE_Slop = BLE_Slop;
            }

            //~~~~~~~~~output = BLE_EndPoint Correction ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

            if(BLE_Slop >(DWORD)(Pre_BLE_Slop/16))
            {
                if((uwBLE_Blend <= ((Pre_BLE_Slop/16)+48))&&(g_ucDlcFlickAlpha==g_DlcParameters.ucDlcAlphaBlendingMin))
                {
                    uwBLE_Blend = (WORD)(((16*(DWORD)BLE_Slop)+(63*(DWORD)Pre_BLE_Slop)+63)/64);
                    Pre_BLE_Slop = uwBLE_Blend;
                    uwBLE_Blend = (uwBLE_Blend+8)/16;
                }
                else
                {
                    uwBLE_Blend = (WORD)((((DWORD) g_ucDlcFlickAlpha * (DWORD)BLE_Slop) +
                                  ((128 - (DWORD)g_ucDlcFlickAlpha) * (DWORD)(Pre_BLE_Slop/16))+127)/128);
                    Pre_BLE_Slop = uwBLE_Blend*16;          //14-bits
                }
            }
            else
            {
                if((uwBLE_Blend >= ((Pre_BLE_Slop/16)-48))&&(g_ucDlcFlickAlpha==g_DlcParameters.ucDlcAlphaBlendingMin))
                {
                    uwBLE_Blend = (WORD)(((16*(DWORD)BLE_Slop)+(63*(DWORD)Pre_BLE_Slop))/64);
                    Pre_BLE_Slop = uwBLE_Blend;
                    uwBLE_Blend = (uwBLE_Blend+8)/16;
                }
                else
                {
                    uwBLE_Blend = (WORD)((((DWORD) g_ucDlcFlickAlpha * (DWORD)BLE_Slop) +
                                  ((128 - (DWORD)g_ucDlcFlickAlpha) * (DWORD)(Pre_BLE_Slop/16)))/128);
                    Pre_BLE_Slop = uwBLE_Blend*16;
                }
            }

            if(g_bSetDlcBleOn)
            {
                msWriteByte(L_BK_DLC(0x10),uwBLE_StartPoint_Blend);
                msWriteByte(H_BK_DLC(0x10),uwBLE_Blend>>3);
                msWriteByte(L_BK_DLC(0x0D),uwBLE_Blend & 0x07);
            }

        }
    }
    Pre_TotalYSum = g_wLumiAverageTemp;
    msDlc_FunctionExit();
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void msDlcWriteCurveLSB(BOOL bWindow, BYTE ucIndex, BYTE ucValue)
{
    DWORD wRegStart;
    BYTE ucData;

    msDlc_FunctionEnter();

	if( MAIN_WINDOW == bWindow )
	    wRegStart = REG_ADDR_DLC_DATA_LSB_START_MAIN;
    else
        wRegStart = REG_ADDR_DLC_DATA_LSB_START_SUB;

    switch(ucIndex)
    {
        case 0:
            wRegStart += 0;
            ucData = (msReadByte(wRegStart) & 0xF3) | (ucValue << 2);
            break;
        case 1:
            wRegStart += 0;
            ucData = (msReadByte(wRegStart) & 0xCF) | (ucValue << 4);
            break;
        case 2:
            wRegStart += 0;
            ucData = (msReadByte(wRegStart) & 0x3F) | (ucValue << 6);
            break;
        case 3:
            wRegStart += 1;
            ucData = (msReadByte(wRegStart) & 0xFC) | (ucValue);
            break;
        case 4:
            wRegStart += 1;
            ucData = (msReadByte(wRegStart) & 0xF3) | (ucValue << 2);
            break;
        case 5:
            wRegStart += 1;
            ucData = (msReadByte(wRegStart) & 0xCF) | (ucValue << 4);
            break;
        case 6:
            wRegStart += 1;
            ucData = (msReadByte(wRegStart) & 0x3F) | (ucValue << 6);
            break;
        case 7:
            wRegStart += 2;
            ucData = (msReadByte(wRegStart) & 0xFC) | (ucValue);
            break;
        case 8:
            wRegStart += 2;
            ucData = (msReadByte(wRegStart) & 0xF3) | (ucValue << 2);
            break;
        case 9:
            wRegStart += 2;
            ucData = (msReadByte(wRegStart) & 0xCF) | (ucValue << 4);
            break;
        case 10:
            wRegStart += 2;
            ucData = (msReadByte(wRegStart) & 0x3F) | (ucValue << 6);
            break;
        case 11:
            wRegStart += 3;
            ucData = (msReadByte(wRegStart) & 0xFC) | (ucValue);
            break;
        case 12:
            wRegStart += 3;
            ucData = (msReadByte(wRegStart) & 0xF3) | (ucValue << 2);
            break;
        case 13:
            wRegStart += 3;
            ucData = (msReadByte(wRegStart) & 0xCF) | (ucValue << 4);
            break;
        case 14:
            wRegStart += 3;
            ucData = (msReadByte(wRegStart) & 0x3F) | (ucValue << 6);
            break;
        case 15:
            wRegStart += 4;
            ucData = (msReadByte(wRegStart) & 0xFC) | (ucValue);
            break;
        case 16: // index 16
            wRegStart += 4;
            ucData = (msReadByte(wRegStart) & 0xF3) | (ucValue << 2);
            break;
        case 0xFF: // index n0
        default:
            wRegStart += 0;
            ucData = (msReadByte(wRegStart) & 0xFC) | (ucValue);
            break;
    }

    msWriteByte(wRegStart, ucData);

    msDlc_FunctionExit();
}

void msDlcWriteCurve(BOOL bWindow)
{
    BYTE ucTmp;
    WORD uwBlend;
    WORD uwTmp;

    msDlc_FunctionEnter();

    // Write data to luma curve ...
    //putstr("[DLC]: Write 16 Luma Curve\r\n");
	//putstr("[DLC]: Disable the Burst Write 16 Luma Curve\r\n");

    if(g_DlcParameters.ucDlcAlgorithmMode == XC_DLC_ALGORITHM_OLD)
    {

	if( MAIN_WINDOW == bWindow )
	{
	    for(ucTmp=0; ucTmp<=0x0f; ucTmp++)
        {
            if(g_ucDlcFastLoop || (g_ucDlcFlickAlpha >= (DLC_DEFLICK_BLEND_FACTOR >> 1)))
            {
                if (g_ucDlcFastLoop)
                    g_ucDlcFastLoop--;

                // target value with alpha blending
                uwBlend = (WORD)((((DWORD) g_ucDlcFlickAlpha * (DWORD)g_ucTable[ucTmp] * DLC_DEFLICK_PRECISION_SHIFT) +
                             ((DLC_DEFLICK_BLEND_FACTOR - (DWORD)g_ucDlcFlickAlpha) * (DWORD)g_uwPreTable[ucTmp]))/DLC_DEFLICK_BLEND_FACTOR);

                if(uwBlend > g_uwPreTable[ucTmp])
                {
                    uwTmp = (uwBlend - g_uwPreTable[ucTmp]) / (DLC_DEFLICK_BLEND_FACTOR - g_ucDlcFlickAlpha + 1);
                    uwBlend = g_uwPreTable[ucTmp] + uwTmp;
                }
                else if(uwBlend < g_uwPreTable[ucTmp])
                {
                    uwTmp = (g_uwPreTable[ucTmp] - uwBlend) / (DLC_DEFLICK_BLEND_FACTOR - g_ucDlcFlickAlpha + 1);
                    uwBlend = g_uwPreTable[ucTmp] - uwTmp;
                }

                g_uwPreTable[ucTmp] = uwBlend;

                //g_ucTable[ucTmp] = g_uwPreTable[ucTmp] / DLC_DEFLICK_PRECISION_SHIFT;
                g_uwTable[ucTmp] = (WORD)(((DWORD)g_uwPreTable[ucTmp]<<2) / DLC_DEFLICK_PRECISION_SHIFT);
                g_ucTable[ucTmp] = g_uwTable[ucTmp]>>2;

                msWriteByte((REG_ADDR_DLC_DATA_START_MAIN+ucTmp), g_ucTable[ucTmp]);
                msDlcWriteCurveLSB(bWindow, ucTmp, g_uwTable[ucTmp]&0x03);

                // set DLC curve index N0 & 16
                if (ucTmp == 0)
                {
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_MAIN, g_uwTable[0]>>2);
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_MAIN + 1, 0x01);
                    msDlcWriteCurveLSB(MAIN_WINDOW, 0xFF, g_uwTable[0]&0x03);
                }

                if (ucTmp == 15)
                {
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_MAIN, 255-(g_uwTable[15]>>2));
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_MAIN + 1, 0x01);
                    msDlcWriteCurveLSB(MAIN_WINDOW, 16, g_uwTable[15]&0x03);
                }
            }
            else if (g_ucDlcFlickAlpha == 0)
            {
                    g_ucTable[ucTmp] = (BYTE)(g_uwPreTable[ucTmp] / DLC_DEFLICK_PRECISION_SHIFT);
            }
            else if ((ucTmp%4) == (g_ucUpdateCnt%4))
            {
                // target value with alpha blending
                uwBlend = (WORD)((((DWORD) g_ucDlcFlickAlpha * (DWORD)g_ucTable[ucTmp] * DLC_DEFLICK_PRECISION_SHIFT) +
                             ((DLC_DEFLICK_BLEND_FACTOR - (DWORD)g_ucDlcFlickAlpha) * (DWORD)g_uwPreTable[ucTmp]))/DLC_DEFLICK_BLEND_FACTOR);

                if(uwBlend > g_uwPreTable[ucTmp])
                {
                    if((uwBlend - g_uwPreTable[ucTmp]) > DLC_DEFLICK_PRECISION_SHIFT)
                        uwBlend = g_uwPreTable[ucTmp] + DLC_DEFLICK_PRECISION_SHIFT;
                }
                else if(uwBlend < g_uwPreTable[ucTmp])
                {
                    if((g_uwPreTable[ucTmp] - uwBlend) > DLC_DEFLICK_PRECISION_SHIFT)
                        uwBlend = g_uwPreTable[ucTmp] - DLC_DEFLICK_PRECISION_SHIFT;
                }

                g_uwPreTable[ucTmp] = uwBlend;

                //g_ucTable[ucTmp] = g_uwPreTable[ucTmp] / DLC_DEFLICK_PRECISION_SHIFT;
                g_uwTable[ucTmp] = (WORD)(((DWORD)g_uwPreTable[ucTmp]<<2) / DLC_DEFLICK_PRECISION_SHIFT);
                g_ucTable[ucTmp] = g_uwTable[ucTmp]>>2;

                msWriteByte((REG_ADDR_DLC_DATA_START_MAIN+ucTmp), g_ucTable[ucTmp]);
                msDlcWriteCurveLSB(bWindow, ucTmp, g_uwTable[ucTmp]&0x03);

                // set DLC curve index N0 & 16
                if (ucTmp == 0)
                {
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_MAIN, g_uwTable[0]>>2);
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_MAIN + 1, 0x01);
                    msDlcWriteCurveLSB(MAIN_WINDOW, 0xFF, g_uwTable[0]&0x03);
                }

                if (ucTmp == 15)
                {
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_MAIN, 255-(g_uwTable[15]>>2));
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_MAIN + 1, 0x01);
                    msDlcWriteCurveLSB(MAIN_WINDOW, 16, g_uwTable[15]&0x03);
                }
            }
	    }
	}
	else
	{
#if XC_DLC_SET_DLC_CURVE_BOTH_SAME
	    for(ucTmp=0; ucTmp<=0x0f; ucTmp++)
        {
            if(g_ucDlcFastLoop || (g_ucDlcFlickAlpha >= (DLC_DEFLICK_BLEND_FACTOR >> 1)))
            {
                if (g_ucDlcFastLoop)
                    g_ucDlcFastLoop--;

                // target value with alpha blending
                uwBlend = (WORD)((((DWORD) g_ucDlcFlickAlpha * (DWORD)g_ucTable[ucTmp] * DLC_DEFLICK_PRECISION_SHIFT) +
                             ((DLC_DEFLICK_BLEND_FACTOR - (DWORD)g_ucDlcFlickAlpha) * (DWORD)g_uwPreTable[ucTmp]))/DLC_DEFLICK_BLEND_FACTOR);

                if(uwBlend > g_uwPreTable[ucTmp])
                {
                    uwTmp = (uwBlend - g_uwPreTable[ucTmp]) / (DLC_DEFLICK_BLEND_FACTOR - g_ucDlcFlickAlpha + 1);
                    uwBlend = g_uwPreTable[ucTmp] + uwTmp;
                }
                else if(uwBlend < g_uwPreTable[ucTmp])
                {
                    uwTmp = (g_uwPreTable[ucTmp] - uwBlend) / (DLC_DEFLICK_BLEND_FACTOR - g_ucDlcFlickAlpha + 1);
                    uwBlend = g_uwPreTable[ucTmp] - uwTmp;
                }

                g_uwPreTable[ucTmp] = uwBlend;

                //g_ucTable[ucTmp] = g_uwPreTable[ucTmp] / DLC_DEFLICK_PRECISION_SHIFT;
                    g_uwTable[ucTmp] = ((DWORD)g_uwPreTable[ucTmp]<<2) / DLC_DEFLICK_PRECISION_SHIFT;
                g_ucTable[ucTmp] = g_uwTable[ucTmp]>>2;

                msWriteByte((REG_ADDR_DLC_DATA_START_SUB+ucTmp), g_ucTable[ucTmp]);
                msDlcWriteCurveLSB(bWindow, ucTmp, g_uwTable[ucTmp]&0x03);

                // set DLC curve index N0 & 16
                if (ucTmp == 0)
                {
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB, g_uwTable[0]>>2);
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB + 1, 0x01);
                    msDlcWriteCurveLSB(SUB_WINDOW, 0xFF, g_uwTable[0]&0x03);
                }

                if (ucTmp == 15)
                {
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB, 255-(g_uwTable[15]>>2));
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB + 1, 0x01);
                    msDlcWriteCurveLSB(SUB_WINDOW, 16, g_uwTable[15]&0x03);
                }
            }
            else if (g_ucDlcFlickAlpha == 0)
            {
                g_ucTable[ucTmp] = g_uwPreTable[ucTmp] / DLC_DEFLICK_PRECISION_SHIFT;
            }
            else if ((ucTmp%4) == (g_ucUpdateCnt%4))
            {
                // target value with alpha blending
                uwBlend = (WORD)((((DWORD) g_ucDlcFlickAlpha * (DWORD)g_ucTable[ucTmp] * DLC_DEFLICK_PRECISION_SHIFT) +
                             ((DLC_DEFLICK_BLEND_FACTOR - (DWORD)g_ucDlcFlickAlpha) * (DWORD)g_uwPreTable[ucTmp]))/DLC_DEFLICK_BLEND_FACTOR);

                if(uwBlend > g_uwPreTable[ucTmp])
                {
                    if((uwBlend - g_uwPreTable[ucTmp]) > DLC_DEFLICK_PRECISION_SHIFT)
                        uwBlend = g_uwPreTable[ucTmp] + DLC_DEFLICK_PRECISION_SHIFT;
                }
                else if(uwBlend < g_uwPreTable[ucTmp])
                {
                    if((g_uwPreTable[ucTmp] - uwBlend) > DLC_DEFLICK_PRECISION_SHIFT)
                        uwBlend = g_uwPreTable[ucTmp] - DLC_DEFLICK_PRECISION_SHIFT;
                }

                g_uwPreTable[ucTmp] = uwBlend;

                //g_ucTable[ucTmp] = g_uwPreTable[ucTmp] / DLC_DEFLICK_PRECISION_SHIFT;
                    g_uwTable[ucTmp] = ((DWORD)g_uwPreTable[ucTmp]<<2) / DLC_DEFLICK_PRECISION_SHIFT;
                    g_ucTable[ucTmp] = g_uwTable[ucTmp]>>2;

                    msWriteByte((REG_ADDR_DLC_DATA_START_SUB+ucTmp), g_ucTable[ucTmp]);
                    msDlcWriteCurveLSB(bWindow, ucTmp, g_uwTable[ucTmp]&0x03);

                    // set DLC curve index N0 & 16
                    if (ucTmp == 0)
                    {
                        msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB, g_uwTable[0]>>2);
                        msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB + 1, 0x01);
                        msDlcWriteCurveLSB(SUB_WINDOW, 0xFF, g_uwTable[0]&0x03);
                    }

                    if (ucTmp == 15)
                    {
                        msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB, 255-(g_uwTable[15]>>2));
                        msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB + 1, 0x01);
                        msDlcWriteCurveLSB(SUB_WINDOW, 16, g_uwTable[15]&0x03);
                    }
                }
            }
#else
            for(ucTmp=0; ucTmp<=0x0f; ucTmp++)
            {
                msWriteByte((REG_ADDR_DLC_DATA_START_SUB+ucTmp), g_ucTable[ucTmp]);
                msDlcWriteCurveLSB(bWindow, ucTmp, g_uwTable[ucTmp]&0x03);

                // set DLC curve index N0 & 16
                if (ucTmp == 0)
                {
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB, g_uwTable[0]>>2);
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB + 1, 0x01);
                    msDlcWriteCurveLSB(SUB_WINDOW, 0xFF, g_uwTable[0]&0x03);
                }

                if (ucTmp == 15)
                {
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB, 255-(g_uwTable[15]>>2));
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB + 1, 0x01);
                    msDlcWriteCurveLSB(SUB_WINDOW, 16, g_uwTable[15]&0x03);
                }
            }
#endif
        }
    }
    else
    {

        if( MAIN_WINDOW == bWindow )
        {
            for(ucTmp=0; ucTmp<=0x0f; ucTmp++)
            {
                if(g_uwTable[ucTmp]>(g_uwPreTable[ucTmp]/16))
                {
                    if((g_uwTable[ucTmp]<=((g_uwPreTable[ucTmp]/16)+48))&&(g_ucDlcFlickAlpha==g_DlcParameters.ucDlcAlphaBlendingMin))
                    {
                        uwBlend = (WORD)(((16*(DWORD)g_uwTable[ucTmp])+(63*(DWORD)g_uwPreTable[ucTmp])+63)/64); //14-bits
                        g_uwPreTable[ucTmp] = uwBlend;                                                              //14-bits
                        uwBlend = (uwBlend+8)/16;                                                                     //10-bits
                    }
                    else
                    {
                        uwBlend = (WORD)((((DWORD) g_ucDlcFlickAlpha * (DWORD)g_uwTable[ucTmp]) +
                                  ((128 - (DWORD)g_ucDlcFlickAlpha) * (DWORD)(g_uwPreTable[ucTmp]/16))+127)/128); //10-bits
                        g_uwPreTable[ucTmp] = uwBlend*16;                                                            //14-bits
                        uwBlend = uwBlend;
                    }
                }
                else
                {
                    if((g_uwTable[ucTmp]>=((g_uwPreTable[ucTmp]/16)-48))&&(g_ucDlcFlickAlpha==g_DlcParameters.ucDlcAlphaBlendingMin))
                    {
                        uwBlend = (WORD)(((16*(DWORD)g_uwTable[ucTmp])+(63*(DWORD)g_uwPreTable[ucTmp]))/64);  //14-bits
                        g_uwPreTable[ucTmp] = uwBlend;                                                            //14-bits
                        uwBlend = (uwBlend+8)/16;                                                                   //10-bits
                    }
                    else
                    {
                        uwBlend = (WORD)((((DWORD) g_ucDlcFlickAlpha * (DWORD)g_uwTable[ucTmp]) +
                                  ((128 - (DWORD)g_ucDlcFlickAlpha) * (DWORD)(g_uwPreTable[ucTmp]/16)))/128); //10-bits
                        g_uwPreTable[ucTmp] = uwBlend*16;                                                        //14-bits
                        uwBlend = uwBlend;
                    }
                }
#if 0
                if (abs(g_uwPreTable[ucTmp]-g_uwTable[ucTmp])<=2)
                {
                    g_uwPreTable[ucTmp] = g_uwPreTable[ucTmp];
                    g_uwTable[ucTmp] = g_uwPreTable[ucTmp];
                }
                else
                {
                    g_uwPreTable[ucTmp] = uwBlend;
                    g_uwTable[ucTmp] = g_uwPreTable[ucTmp];
                }
#endif
                g_uwTable[ucTmp] = uwBlend;
                g_ucTable[ucTmp] = g_uwTable[ucTmp]>>2;

                msWriteByte((REG_ADDR_DLC_DATA_START_MAIN+ucTmp), g_ucTable[ucTmp]);
                msDlcWriteCurveLSB(bWindow, ucTmp, g_uwTable[ucTmp]&0x03);

                // set DLC curve index N0 & 16
                if (ucTmp == 0)
                {
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_MAIN, g_uwTable[0]>>2);
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_MAIN + 1, 0x01);
                    msDlcWriteCurveLSB(MAIN_WINDOW, 0xFF, g_uwTable[0]&0x03);
                }

                if (ucTmp == 15)
                {
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_MAIN, 255-(g_uwTable[15]>>2));
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_MAIN + 1, 0x01);
                    msDlcWriteCurveLSB(MAIN_WINDOW, 16, g_uwTable[15]&0x03);
                }
            }
        }
        else
        {
#if XC_DLC_SET_DLC_CURVE_BOTH_SAME
            //Mantis issue of 0232938:Hsense A3 DLC the divisor can't to be equal to zero .
            if((DLC_DEFLICK_BLEND_FACTOR - g_ucDlcFlickAlpha + 1)==0)
            {
                DLC_DEBUG(printf("\n Error in  [New DLC Algorithm][ %s  , %d ]  (DLC_DEFLICK_BLEND_FACTOR - g_ucDlcFlickAlpha + 1)=0 !!!\n", __FUNCTION__,__LINE__));
                return;
            }

            for(ucTmp=0; ucTmp<=0x0f; ucTmp++)
            {
                if(g_ucDlcFastLoop || (g_ucDlcFlickAlpha >= (DLC_DEFLICK_BLEND_FACTOR >> 1)))
                {
                    if (g_ucDlcFastLoop)
                    {
                        g_ucDlcFastLoop--;
                    }

                    // target value with alpha blending
                    //uwBlend = (WORD)((((DWORD) g_ucDlcFlickAlpha * (DWORD)g_ucTable[ucTmp] * DLC_DEFLICK_PRECISION_SHIFT) +
                    //             ((DLC_DEFLICK_BLEND_FACTOR - (DWORD)g_ucDlcFlickAlpha) * (DWORD)g_uwPreTable[ucTmp]))/DLC_DEFLICK_BLEND_FACTOR);

                    uwBlend = (WORD)((((DWORD) g_ucDlcFlickAlpha * (DWORD)g_ucTable[ucTmp] * 4) +
                              ((32 - (DWORD)g_ucDlcFlickAlpha) * (DWORD)g_uwPreTable[ucTmp])+16)/32);

                    if(uwBlend > g_uwPreTable[ucTmp])
                    {
                        uwTmp = (uwBlend - g_uwPreTable[ucTmp]) / (DLC_DEFLICK_BLEND_FACTOR - g_ucDlcFlickAlpha + 1);
                        uwBlend = g_uwPreTable[ucTmp] + uwTmp;
                    }
                    else if(uwBlend < g_uwPreTable[ucTmp])
                    {
                        uwTmp = (g_uwPreTable[ucTmp] - uwBlend) / (DLC_DEFLICK_BLEND_FACTOR - g_ucDlcFlickAlpha + 1);
                        uwBlend = g_uwPreTable[ucTmp] - uwTmp;
                    }

                    g_uwPreTable[ucTmp] = uwBlend;

                    //g_ucTable[ucTmp] = g_uwPreTable[ucTmp] / DLC_DEFLICK_PRECISION_SHIFT;
                    g_uwTable[ucTmp] = g_uwPreTable[ucTmp];//((DWORD)g_uwPreTable[ucTmp]<<2) / 4;
                    g_ucTable[ucTmp] = g_uwTable[ucTmp]>>2;

                    msWriteByte((REG_ADDR_DLC_DATA_START_SUB+ucTmp), g_ucTable[ucTmp]);
                    msDlcWriteCurveLSB(bWindow, ucTmp, g_uwTable[ucTmp]&0x03);

                    // set DLC curve index N0 & 16
                    if (ucTmp == 0)
                    {
                        msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB, g_uwTable[0]>>2);
                        msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB + 1, 0x01);
                        msDlcWriteCurveLSB(SUB_WINDOW, 0xFF, g_uwTable[0]&0x03);
                    }

                    if (ucTmp == 15)
                    {
                        msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB, 255-(g_uwTable[15]>>2));
                        msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB + 1, 0x01);
                        msDlcWriteCurveLSB(SUB_WINDOW, 16, g_uwTable[15]&0x03);
                    }
                }
                else if (g_ucDlcFlickAlpha == 0)
                {
                    g_ucTable[ucTmp] = g_uwPreTable[ucTmp] / 4;
                }
                else if ((ucTmp%4) == (g_ucUpdateCnt%4))
                {
                    // target value with alpha blending
                    //uwBlend = (WORD)((((DWORD) g_ucDlcFlickAlpha * (DWORD)g_ucTable[ucTmp] * DLC_DEFLICK_PRECISION_SHIFT) +
                    //             ((DLC_DEFLICK_BLEND_FACTOR - (DWORD)g_ucDlcFlickAlpha) * (DWORD)g_uwPreTable[ucTmp]))/DLC_DEFLICK_BLEND_FACTOR);

                    uwBlend = (WORD)((((DWORD) g_ucDlcFlickAlpha * (DWORD)g_ucTable[ucTmp] * 4) +
                              ((32 - (DWORD)g_ucDlcFlickAlpha) * (DWORD)g_uwPreTable[ucTmp])+16)/32);

                    if(uwBlend > g_uwPreTable[ucTmp])
                    {
                        if((uwBlend - g_uwPreTable[ucTmp]) > 4)
                        {
                            uwBlend = g_uwPreTable[ucTmp] + 4;
                        }
                    }
                    else if(uwBlend < g_uwPreTable[ucTmp])
                    {
                        if((g_uwPreTable[ucTmp] - uwBlend) > 4)
                        {
                            uwBlend = g_uwPreTable[ucTmp] - 4;
                        }
                    }

                    g_uwPreTable[ucTmp] = uwBlend;

                    //g_ucTable[ucTmp] = g_uwPreTable[ucTmp] / DLC_DEFLICK_PRECISION_SHIFT;
                    g_uwTable[ucTmp] = g_uwPreTable[ucTmp];//((DWORD)g_uwPreTable[ucTmp]<<2) / 4;
                g_ucTable[ucTmp] = g_uwTable[ucTmp]>>2;

                msWriteByte((REG_ADDR_DLC_DATA_START_SUB+ucTmp), g_ucTable[ucTmp]);
                msDlcWriteCurveLSB(bWindow, ucTmp, g_uwTable[ucTmp]&0x03);

                // set DLC curve index N0 & 16
                if (ucTmp == 0)
                {
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB, g_uwTable[0]>>2);
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB + 1, 0x01);
                    msDlcWriteCurveLSB(SUB_WINDOW, 0xFF, g_uwTable[0]&0x03);
                }

                if (ucTmp == 15)
                {
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB, 255-(g_uwTable[15]>>2));
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB + 1, 0x01);
                    msDlcWriteCurveLSB(SUB_WINDOW, 16, g_uwTable[15]&0x03);
                }
            }
	    }
#else
            for(ucTmp=0; ucTmp<=0x0f; ucTmp++)
            {
                msWriteByte((REG_ADDR_DLC_DATA_START_SUB+ucTmp), g_ucTable[ucTmp]);
                msDlcWriteCurveLSB(bWindow, ucTmp, g_uwTable[ucTmp]&0x03);

                // set DLC curve index N0 & 16
                if (ucTmp == 0)
                {
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB, g_uwTable[0]>>2);
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB + 1, 0x01);
                    msDlcWriteCurveLSB(SUB_WINDOW, 0xFF, g_uwTable[0]&0x03);
                }

                if (ucTmp == 15)
                {
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB, 255-(g_uwTable[15]>>2));
                    msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB + 1, 0x01);
                    msDlcWriteCurveLSB(SUB_WINDOW, 16, g_uwTable[15]&0x03);
                }
            }
#endif
        }
	}

    g_ucUpdateCnt++;

    msDlc_FunctionExit();
}

//////////////////////////////////////////////////////////////
// Program register bytes to MST chip.
//
// Arguments: pTable - pointer of registers table
//////////////////////////////////////////////////////////////
void msWriteDLCRegsTbl(DlcRegUnitType code *pTable)
{
    WORD wReg; // register index

    while (1)
    {
         wReg = pTable->wReg; // get register index
         if (wReg == _END_OF_TBL_) // check end of table
             break;

         msWriteByte(wReg, pTable->ucValue); // write register

         pTable++; // next
    } // while
}

void msDlcSpeedupTrigger(BYTE ucLoop)
{
    g_DlcParameters.ucDlcFlickAlphaStart = ucLoop;
}

/******************************************************************************/
/*                           CGC                                         */
/******************************************************************************/

void msDlc_CGC_Init(void)
{
    msDlc_FunctionEnter();

    if((msReadByte(REG_ADDR_DLC_HANDSHAKE) & _BIT7) )// Shall not Init CGC again while DLC is On.
    {
       msDlc_FunctionExit();
       return;
    }

    g_u8Chroma_OriGain = msReadByte(REG_ADDR_DLC_C_GAIN);
    msDlc_CGC_ReInit();

    msDlc_FunctionExit();
}

void msDlc_CGC_ReInit(void) // For CGC Init & Tool change parameter value dynamicly
{
    if ((0xFF-g_u8Chroma_OriGain)>g_DlcParameters.ucCGCChroma_GainLimitH)
        g_u8CGC_UpperBond = (g_u8Chroma_OriGain+g_DlcParameters.ucCGCChroma_GainLimitH);
    else
        g_u8CGC_UpperBond = 0xFF;

    if (g_DlcParameters.ucCGCChroma_GainLimitL<g_u8Chroma_OriGain)
        g_u8CGC_LowerBond = (g_u8Chroma_OriGain-g_DlcParameters.ucCGCChroma_GainLimitL);
    else
        g_u8CGC_LowerBond = 0x00;

    g_u16Ydark = (Y01_weight*(g_ucTable[0]+g_ucTable[1])+Y23_weight*(g_ucTable[2]+g_ucTable[3]))/(2*(Y01_weight+Y23_weight));
    g_u16Ydark =  g_u16Ydark+((Y45_weight*(g_ucTable[4]+g_ucTable[5])+Y67_weight*(g_ucTable[6]+g_ucTable[7])))/(2*(Y45_weight+Y67_weight));

    if (g_DlcParameters.ucCGCCGain_offset & _BIT7)
        g_u16Ydark = (g_u16Ydark-(g_DlcParameters.ucCGCCGain_offset&0x7F));
    else
        g_u16Ydark = (g_u16Ydark+g_DlcParameters.ucCGCCGain_offset);

    g_u16Ydark_pre = g_u16Ydark;

    //printf("\r\n[1]. g_ucTable[0] = %d \n", g_ucTable[0]);
    //printf("\r\n[2]. g_ucTable[1] = %d \n", g_ucTable[1]);
    //printf("\r\n[3]. g_ucTable[2] = %d \n", g_ucTable[2]);
    //printf("\r\n[4]. g_ucTable[3] = %d \n", g_ucTable[3]);
    //printf("\r\n[5]. g_ucTable[4] = %d \n", g_ucTable[4]);


    //printf("\r\ng_u8CGC_UpperBond = %d \n", g_u8CGC_UpperBond);
    //printf("\r\ng_u8CGC_LowerBond = %d \n", g_u8CGC_LowerBond);

    //printf("\r\ng_u16Ydark(std) = %d \n", g_u16Ydark);
}

void msDlc_CGC_SetGain(BYTE u8CGain)
{
    msDlc_FunctionEnter();

    if (g_DlcParameters.bCGCCGainCtrl && (REG_ADDR_DLC_C_GAIN != REG_NULL))
        msWriteByte(REG_ADDR_DLC_C_GAIN, u8CGain);

    msDlc_FunctionExit();
}

void msDlc_CGC_Handler(void)
{
    BYTE u8TmpVal;

    WORD u16Ytemp, u16Ydiff;

    msDlc_FunctionEnter();

    //printf("\r\n g_u8CGC_UpperBond = %d \n", g_u8CGC_UpperBond);
    //printf("\r\n g_u8CGC_LowerBond = %d \n", g_u8CGC_LowerBond);

    u16Ytemp = (WORD)((DWORD) (Y01_weight*(g_ucTable[0]+g_ucTable[1])+
             Y23_weight*(g_ucTable[2]+g_ucTable[3]))/
            (Y01_weight+Y23_weight));

    u16Ytemp =  (WORD)(u16Ytemp/2+(DWORD)(Y45_weight*(g_ucTable[4]+g_ucTable[5])+
                       Y67_weight*(g_ucTable[6]+g_ucTable[7]))/
                      (2*(Y45_weight+Y67_weight)));

    //printf("\r\nu16Ytemp       = %d", u16Ytemp);
    //printf("\r\ng_u16Ydark_pre = %d", g_u16Ydark_pre);

    if (abs(u16Ytemp-g_u16Ydark_pre)>=Yvar_Th)
    {
        g_u8TCount++;

     	 if ((u16Ytemp>g_u16Ydark)&&(g_u8TCount>=Timer_CountU))
     {
             //printf("\r\ng_u8TCountU       = %d", g_u8TCount);
         g_u8TCount = 0;

             u16Ydiff=(u16Ytemp-g_u16Ydark);

             if (u16Ydiff>g_DlcParameters.ucCGCYth)
             {
     	         g_u8Chroma_target = g_u8Chroma_OriGain+(WORD)((u16Ydiff-g_DlcParameters.ucCGCYth)*g_DlcParameters.ucCGCYCslope)/(0x20);

     	         if (g_u8Chroma_target > g_u8CGC_UpperBond)
     	             g_u8Chroma_target = g_u8CGC_UpperBond;
     	     }
     	     else
     	         g_u8Chroma_target = g_u8Chroma_OriGain;
         }
         else if ((u16Ytemp<=g_u16Ydark)&&(g_u8TCount>=Timer_CountD))
         {
             //printf("\r\ng_u8TCountD       = %d", g_u8TCount);
             g_u8TCount = 0;

             u16Ydiff=(g_u16Ydark-u16Ytemp);

             if (u16Ydiff>g_DlcParameters.ucCGCYth)
             {
     	         g_u8Chroma_target = g_u8Chroma_OriGain-(WORD)((u16Ydiff-g_DlcParameters.ucCGCYth)*g_DlcParameters.ucCGCYCslope)/(0x20);

     	         if (g_u8Chroma_target < g_u8CGC_LowerBond)
     	             g_u8Chroma_target = g_u8CGC_LowerBond;
     	     }
     	     else
     	         g_u8Chroma_target = g_u8Chroma_OriGain;
         }

         //printf("\r\ng_u8Chroma_target = %d", g_u8Chroma_target);

     	 u8TmpVal = msReadByte(REG_ADDR_DLC_C_GAIN);

     	 if (g_u8Chroma_target>u8TmpVal)
     	     msDlc_CGC_SetGain(u8TmpVal+CGain_StepU);
     	 else if (g_u8Chroma_target<u8TmpVal)
     	     msDlc_CGC_SetGain(u8TmpVal-CGain_StepD);
    }

    g_u16Ydark_pre = (WORD)((DWORD)(31*g_u16Ydark_pre+u16Ytemp)/32);

    msDlc_FunctionExit();
}

//////////////////////////////////////////////////////////////
// Debug Protocol
//////////////////////////////////////////////////////////////

// Debug Command Paser
#define URCMD_MST_DLC_TEST_FORCE_PAUSE      0
#define URCMD_MST_DLC_TEST_FORCE_RUN        1
#define URCMD_MST_DLC_TEST_READ             2 // old mode
#define URCMD_MST_DLC_TEST_WRITE_1          3
#define URCMD_MST_DLC_TEST_WRITE_2          4
#define URCMD_MST_DLC_TEST_REINIT           5
#define URCMD_MST_DLC_TEST_READ_BOUNDARY	6
#define URCMD_MST_DLC_TEST_WRITE_BOUNDARY	7
#define URCMD_MST_DLC_TEST_READ_CV1         8
#define URCMD_MST_DLC_TEST_READ_CV2A        9
#define URCMD_MST_DLC_TEST_READ_CV2B        10
#define URCMD_MST_DLC_TEST_WRITE_CV1        11
#define URCMD_MST_DLC_TEST_WRITE_CV2A       12
#define URCMD_MST_DLC_TEST_WRITE_CV2B       13
#define URCMD_MST_DLC_TEST_READ_INFO        14
#define URCMD_MST_DLC_TEST_WRITE_INFO       15
#define URCMD_MST_DLC_TEST_CGC_REINIT       16
#define URCMD_MST_DLC_TEST_READ_Sloplimit   17
#define URCMD_MST_DLC_TEST_WRITE_Sloplimit  18

void msDLC_ParametersTool(BYTE *pCmdBuff)
{
    BYTE ucTmpLen;

    ucTmpLen = *(pCmdBuff+1); //ucTmpLen

    switch(*(pCmdBuff+3)) //UART_EXT_CMD
    {
        case URCMD_MST_DLC_TEST_FORCE_PAUSE:

            break;
        case URCMD_MST_DLC_TEST_FORCE_RUN:

            break;
        case URCMD_MST_DLC_TEST_READ:
            {
                BYTE i;
                for (i=0; i<16; i+=2)
                {
                    putchar(0xF3);
                    putchar(g_DlcParameters.ucLumaCurve[i]);
                    putchar(g_DlcParameters.ucLumaCurve[i+1]);
                    Delay1ms(150);
                }
            }
            break;
        case URCMD_MST_DLC_TEST_WRITE_1:
            {
                BYTE i;
              #if 1//( ENABLE_UART_CHECKSUM )
                ucTmpLen--; // remove checksum byte
              #endif
                for (i=0; i<(ucTmpLen-4); i++)
                    g_DlcParameters.ucLumaCurve[i] = *(pCmdBuff+4 + i);
            }
            break;
        case URCMD_MST_DLC_TEST_WRITE_2:
            {
                BYTE i;
              #if 1//( ENABLE_UART_CHECKSUM )
                ucTmpLen--; // remove checksum byte
              #endif
                for (i=0; i<(ucTmpLen-4); i++)
                    g_DlcParameters.ucLumaCurve[8 + i] = *(pCmdBuff+4 + i);
            }
            break;
        case URCMD_MST_DLC_TEST_READ_CV1:
            {
                putchar(0xF2);
                putchar(g_DlcParameters.ucLumaCurve[*(pCmdBuff+4)]);
            }
            break;
        case URCMD_MST_DLC_TEST_READ_CV2A:
            {
                putchar(0xF2);
                putchar(g_DlcParameters.ucLumaCurve2_a[*(pCmdBuff+4)]);
            }
            break;
        case URCMD_MST_DLC_TEST_READ_CV2B:
            {
                putchar(0xF2);
                putchar(g_DlcParameters.ucLumaCurve2_b[*(pCmdBuff+4)]);
            }
            break;
        case URCMD_MST_DLC_TEST_WRITE_CV1:
            {
                BYTE i;
              #if 1//( ENABLE_UART_CHECKSUM )
                ucTmpLen--; // remove checksum byte
              #endif
                for (i=0; i<(ucTmpLen-4-1); i++)
                    g_DlcParameters.ucLumaCurve[*(pCmdBuff+4) + i] = *(pCmdBuff+5 + i);
            }
            break;
        case URCMD_MST_DLC_TEST_WRITE_CV2A:
            {
                BYTE i;
              #if 1//( ENABLE_UART_CHECKSUM )
                ucTmpLen--; // remove checksum byte
              #endif
                for (i=0; i<(ucTmpLen-4-1); i++)
                    g_DlcParameters.ucLumaCurve2_a[*(pCmdBuff+4) + i] = *(pCmdBuff+5 + i);
            }
            break;
        case URCMD_MST_DLC_TEST_WRITE_CV2B:
            {
                BYTE i;
              #if 1//( ENABLE_UART_CHECKSUM )
                ucTmpLen--; // remove checksum byte
              #endif
                for (i=0; i<(ucTmpLen-4-1); i++)
                    g_DlcParameters.ucLumaCurve2_b[*(pCmdBuff+4) + i] = *(pCmdBuff+5 + i);
            }
            break;
        case URCMD_MST_DLC_TEST_READ_INFO:
            {
                if (*(pCmdBuff+4) == 1) // DLC curve
                {
                    putchar(0xF2);
                    putchar(g_ucTable[*(pCmdBuff+5)]);
                }
                else if (*(pCmdBuff+4) == 2) // DLC other
              #if 1 //(ENABLE_DLC == DLC_32SEG_HIS_MODE)
                {
                    if (*(pCmdBuff+5) < 64)
                    {
                        putchar(0xF2);
                        if (*(pCmdBuff+5)%2)
                            putchar(g_wLumaHistogram32H[*(pCmdBuff+5)/2] & 0xFF);
                        else
                            putchar(g_wLumaHistogram32H[*(pCmdBuff+5)/2] >> 8);
                    }
                    else if (*(pCmdBuff+5) == 200)
                    {
                        WORD wVer;
                        wVer = msDlcLibVersionCheck();
                        putchar(0xF3);
                        putchar(wVer >> 8);
                        putchar(wVer & 0xFF);
                    }
                    else if (*(pCmdBuff+5) == 201)
                    {
                        putchar(0xF3);
                        putchar(g_DlcParameters.uwDlcBleStartPointTH >> 8);
                        putchar(g_DlcParameters.uwDlcBleStartPointTH & 0xFF);
                    }
                    else if (*(pCmdBuff+5) == 202)
                    {
                        putchar(0xF3);
                        putchar(g_DlcParameters.uwDlcBleEndPointTH >> 8);
                        putchar(g_DlcParameters.uwDlcBleEndPointTH & 0xFF);
                    }
                    else if (*(pCmdBuff+5) == 203)
                    {
                        putchar(0xF3);
                        putchar(g_DlcParameters.uwDlcBLESlopPoint_1 >> 8);
                        putchar(g_DlcParameters.uwDlcBLESlopPoint_1 & 0xFF);
                    }
                    else if (*(pCmdBuff+5) == 204)
                    {
                        putchar(0xF3);
                        putchar(g_DlcParameters.uwDlcBLESlopPoint_2 >> 8);
                        putchar(g_DlcParameters.uwDlcBLESlopPoint_2 & 0xFF);
                    }
                    else if (*(pCmdBuff+5) == 205)
                    {
                        putchar(0xF3);
                        putchar(g_DlcParameters.uwDlcBLESlopPoint_3 >> 8);
                        putchar(g_DlcParameters.uwDlcBLESlopPoint_3 & 0xFF);
                    }
                    else if (*(pCmdBuff+5) == 206)
                    {
                        putchar(0xF3);
                        putchar(g_DlcParameters.uwDlcBLESlopPoint_4 >> 8);
                        putchar(g_DlcParameters.uwDlcBLESlopPoint_4 & 0xFF);
                    }
                    else if (*(pCmdBuff+5) == 207)
                    {
                        putchar(0xF3);
                        putchar(g_DlcParameters.uwDlcBLESlopPoint_5 >> 8);
                        putchar(g_DlcParameters.uwDlcBLESlopPoint_5 & 0xFF);
                    }
                    else if (*(pCmdBuff+5) == 208)
                    {
                        putchar(0xF3);
                        putchar(g_DlcParameters.uwDlcDark_BLE_Slop_Min >> 8);
                        putchar(g_DlcParameters.uwDlcDark_BLE_Slop_Min & 0xFF);
                    }
                    else if (*(pCmdBuff+5) >= 64)
                    {
                        putchar(0xF2);
                        if (*(pCmdBuff+5) == 64)
                            putchar(g_wLumiAverageTemp & 0xFF);
                        else if (*(pCmdBuff+5) == 65)
                            putchar(g_wLumiAverageTemp >> 8);
                        else if (*(pCmdBuff+5) == 66)
                            putchar(g_wLumiTotalCount & 0xFF);
                        else if (*(pCmdBuff+5) == 67)
                            putchar(g_wLumiTotalCount >> 8);
                        else if (*(pCmdBuff+5) == 68)
                            putchar(g_DlcParameters.u8_L_L_U);
                        else if (*(pCmdBuff+5) == 69)
                            putchar(g_DlcParameters.u8_L_L_D);
                        else if (*(pCmdBuff+5) == 70)
                            putchar(g_DlcParameters.u8_L_H_U);
                        else if (*(pCmdBuff+5) == 71)
                            putchar(g_DlcParameters.u8_L_H_D);
                        else if (*(pCmdBuff+5) == 72)
                            putchar(g_DlcParameters.u8_S_L_U);
                        else if (*(pCmdBuff+5) == 73)
                            putchar(g_DlcParameters.u8_S_L_D);
                        else if (*(pCmdBuff+5) == 74)
                            putchar(g_DlcParameters.u8_S_H_U);
                        else if (*(pCmdBuff+5) == 75)
                            putchar(g_DlcParameters.u8_S_H_D);
                        else if (*(pCmdBuff+5) == 76)
                            putchar(g_ucHistogramMin);
                        else if (*(pCmdBuff+5) == 77)
                            putchar(g_ucHistogramMax);
                        else if (*(pCmdBuff+5) == 78)
                            putchar(g_DlcParameters.ucDlcPureImageMode);
                        else if (*(pCmdBuff+5) == 79)
                            putchar(g_DlcParameters.ucDlcLevelLimit);
                        else if (*(pCmdBuff+5) == 80)
                            putchar(g_DlcParameters.ucDlcAvgDelta);
                        else if (*(pCmdBuff+5) == 81)
                            putchar(g_DlcParameters.ucDlcFastAlphaBlending);
                        else if (*(pCmdBuff+5) == 82)
                            putchar(g_DlcParameters.bEnableBLE);
                        else if (*(pCmdBuff+5) == 83)
                            putchar(g_DlcParameters.bEnableWLE);
                        else if (*(pCmdBuff+5) == 84)
                            putchar(g_DlcParameters.ucDlcBLEPoint);
                        else if (*(pCmdBuff+5) == 85)
                            putchar(g_DlcParameters.ucDlcWLEPoint);
                        else if (*(pCmdBuff+5) == 86)
                            putchar(g_DlcParameters.ucDlcAvgDeltaStill);
                        else if (*(pCmdBuff+5) == 87)
                            putchar(g_DlcParameters.ucDlcYAvgThresholdL);
                        else if (*(pCmdBuff+5) == 88)
                            putchar(g_DlcParameters.ucDlcYAvgThresholdH);
                        else if (*(pCmdBuff+5) == 89)
                            putchar(g_DlcParameters.ucCGCCGain_offset);
                        else if (*(pCmdBuff+5) == 90)
                            putchar(g_DlcParameters.ucCGCChroma_GainLimitH);
                        else if (*(pCmdBuff+5) == 91)
                            putchar(g_DlcParameters.ucCGCChroma_GainLimitL);
                        else if (*(pCmdBuff+5) == 92)
                            putchar(g_DlcParameters.ucCGCYCslope);
                        else if (*(pCmdBuff+5) == 93)
                            putchar(g_DlcParameters.ucCGCYth);
                        else if (*(pCmdBuff+5) == 94)
                            putchar(g_DlcParameters.bCGCCGainCtrl);
                        else if (*(pCmdBuff+5) == 95)
                            putchar(g_DlcParameters.ucDlcYAvgThresholdM);
                        else if (*(pCmdBuff+5) == 96)
                            putchar(g_DlcParameters.ucDlcCurveMode);
                        else if (*(pCmdBuff+5) == 97)
                            putchar(g_DlcParameters.ucDlcCurveModeMixAlpha);
                        else if (*(pCmdBuff+5) == 98)
                            putchar(g_DlcParameters.ucDlcAlgorithmMode);
                        else if (*(pCmdBuff+5) == 99)
                            putchar(g_DlcParameters.ucDlcSepPointH);
                        else if (*(pCmdBuff+5) == 100)
                            putchar(g_DlcParameters.ucDlcSepPointL);
                        else if (*(pCmdBuff+5) == 101)
                            putchar(g_DlcParameters.ucDlcCurveDiff_L_TH);
                        else if (*(pCmdBuff+5) == 102)
                            putchar(g_DlcParameters.ucDlcCurveDiff_H_TH);
                        else if (*(pCmdBuff+5) == 103)
                            putchar(g_DlcParameters.ucDlcCurveDiffCoringTH);
                        else if (*(pCmdBuff+5) == 104)
                            putchar(g_DlcParameters.ucDlcAlphaBlendingMin);
                        else if (*(pCmdBuff+5) == 105)
                            putchar(g_DlcParameters.ucDlcAlphaBlendingMax);
                        else if (*(pCmdBuff+5) == 106)
                            putchar(g_DlcParameters.ucDlcFlicker_alpha);
                        else if (*(pCmdBuff+5) == 107)
                            putchar(g_DlcParameters.ucDlcYAVG_L_TH);
                        else if (*(pCmdBuff+5) == 108)
                            putchar(g_DlcParameters.ucDlcYAVG_H_TH);
                        else if (*(pCmdBuff+5) == 109)
                            putchar(g_DlcParameters.ucDlcDiffBase_L);
                        else if (*(pCmdBuff+5) == 110)
                            putchar(g_DlcParameters.ucDlcDiffBase_M);
                        else if (*(pCmdBuff+5) == 111)
                            putchar(g_DlcParameters.ucDlcDiffBase_H);
                        else
                            putchar(0);
                    }
                    else
                    {
                        putchar(0xF2);
                        putchar(0);
                    }
                }
              #else // (ENABLE_DLC == DLC_8SEG_HIS_MODE)
                {
                    if (*(pCmdBuff+5) < 16)
                    {
                        putchar(0xF2);
                        if (*(pCmdBuff+5)%2)
                            putchar(g_wLumaHistogram8H[*(pCmdBuff+5)/2] & 0xFF);
                        else
                            putchar(g_wLumaHistogram8H[*(pCmdBuff+5)/2] >> 8);
                    }
                    else if (*(pCmdBuff+5) == 200)
                    {
                        WORD wVer;
                        wVer = msDlcLibVersionCheck();
                        putchar(0xF3);
                        putchar(wVer >> 8);
                        putchar(wVer & 0xFF);
                    }
                    else if (*(pCmdBuff+5) >= 64)
                    {
                        putchar(0xF2);
                        if (*(pCmdBuff+5) == 64)
                            putchar(g_wLumiAverageTemp & 0xFF);
                        else if (*(pCmdBuff+5) == 65)
                            putchar(g_wLumiAverageTemp >> 8);
                        else if (*(pCmdBuff+5) == 66)
                            putchar(g_wLumiTotalCount & 0xFF);
                        else if (*(pCmdBuff+5) == 67)
                            putchar(g_wLumiTotalCount >> 8);
                        else if (*(pCmdBuff+5) == 68)
                            putchar(g_DlcParameters.u8_L_L_U);
                        else if (*(pCmdBuff+5) == 69)
                            putchar(g_DlcParameters.u8_L_L_D);
                        else if (*(pCmdBuff+5) == 70)
                            putchar(g_DlcParameters.u8_L_H_U);
                        else if (*(pCmdBuff+5) == 71)
                            putchar(g_DlcParameters.u8_L_H_D);
                        else if (*(pCmdBuff+5) == 72)
                            putchar(g_DlcParameters.u8_S_L_U);
                        else if (*(pCmdBuff+5) == 73)
                            putchar(g_DlcParameters.u8_S_L_D);
                        else if (*(pCmdBuff+5) == 74)
                            putchar(g_DlcParameters.u8_S_H_U);
                        else if (*(pCmdBuff+5) == 75)
                            putchar(g_DlcParameters.u8_S_H_D);
                        else if (*(pCmdBuff+5) == 76)
                            putchar(g_ucHistogramMin);
                        else if (*(pCmdBuff+5) == 77)
                            putchar(g_ucHistogramMax);
                        else if (*(pCmdBuff+5) == 78)
                            putchar(g_DlcParameters.ucDlcPureImageMode);
                        else if (*(pCmdBuff+5) == 79)
                            putchar(g_DlcParameters.ucDlcLevelLimit);
                        else if (*(pCmdBuff+5) == 80)
                            putchar(g_DlcParameters.ucDlcAvgDelta);
                        else if (*(pCmdBuff+5) == 81)
                            putchar(g_DlcParameters.ucDlcFastAlphaBlending);
                        else if (*(pCmdBuff+5) == 82)
                            putchar(g_DlcParameters.bEnableBLE);
                        else if (*(pCmdBuff+5) == 83)
                            putchar(g_DlcParameters.bEnableWLE);
                        else if (*(pCmdBuff+5) == 84)
                            putchar(g_DlcParameters.ucDlcBLEPoint);
                        else if (*(pCmdBuff+5) == 85)
                            putchar(g_DlcParameters.ucDlcWLEPoint);
                        else if (*(pCmdBuff+5) == 86)
                            putchar(g_DlcParameters.ucDlcAvgDeltaStill);
                        else if (*(pCmdBuff+5) == 87)
                            putchar(g_DlcParameters.ucDlcYAvgThresholdL);
                        else if (*(pCmdBuff+5) == 88)
                            putchar(g_DlcParameters.ucDlcYAvgThresholdH);
                        else if (*(pCmdBuff+5) == 89)
                            putchar(g_DlcParameters.ucCGCCGain_offset);
                        else if (*(pCmdBuff+5) == 90)
                            putchar(g_DlcParameters.ucCGCChroma_GainLimitH);
                        else if (*(pCmdBuff+5) == 91)
                            putchar(g_DlcParameters.ucCGCChroma_GainLimitL);
                        else if (*(pCmdBuff+5) == 92)
                            putchar(g_DlcParameters.ucCGCYCslope);
                        else if (*(pCmdBuff+5) == 93)
                            putchar(g_DlcParameters.ucCGCYth);
                        else if (*(pCmdBuff+5) == 94)
                            putchar(g_DlcParameters.bCGCCGainCtrl);
                        else
                            putchar(0);
                    }
                    else
                    {
                        putchar(0xF2);
                        putchar(0);
                    }
                }
              #endif
            }
            break;
        case URCMD_MST_DLC_TEST_WRITE_INFO:
            {
                if (*(pCmdBuff+4) == 0)
                {
                    g_DlcParameters.u8_L_L_U = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 1)
                {
                    g_DlcParameters.u8_L_L_D = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 2)
                {
                    g_DlcParameters.u8_L_H_U = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 3)
                {
                    g_DlcParameters.u8_L_H_D = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 4)
                {
                    g_DlcParameters.u8_S_L_U = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 5)
                {
                    g_DlcParameters.u8_S_L_D = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 6)
                {
                    g_DlcParameters.u8_S_H_U = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 7)
                {
                    g_DlcParameters.u8_S_H_D = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 8)
                {
                    g_DlcParameters.ucDlcPureImageMode = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 9)
                {
                    g_DlcParameters.ucDlcLevelLimit = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 10)
                {
                    g_DlcParameters.ucDlcAvgDelta = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 11)
                {
                    g_DlcParameters.ucDlcFastAlphaBlending = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 12)
                {
                    g_DlcParameters.bEnableBLE = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 13)
                {
                    g_DlcParameters.bEnableWLE = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 14)
                {
                    g_DlcParameters.ucDlcBLEPoint = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 15)
                {
                    g_DlcParameters.ucDlcWLEPoint = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 16)
                {
                    g_DlcParameters.ucDlcAvgDeltaStill= *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 17)
                {
                    g_DlcParameters.ucDlcYAvgThresholdL = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 18)
                {
                    g_DlcParameters.ucDlcYAvgThresholdH = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 19)
                {
                    g_DlcParameters.ucCGCCGain_offset = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 20)
                {
                    g_DlcParameters.ucCGCChroma_GainLimitH = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 21)
                {
                    g_DlcParameters.ucCGCChroma_GainLimitL = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 22)
                {
                    g_DlcParameters.ucCGCYCslope = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 23)
                {
                    g_DlcParameters.ucCGCYth = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 24)
                {
                    g_DlcParameters.bCGCCGainCtrl = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 25)
                {
                    g_DlcParameters.ucDlcYAvgThresholdM = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 26)
                {
                    g_DlcParameters.ucDlcCurveMode = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 27)
                {
                    g_DlcParameters.ucDlcCurveModeMixAlpha = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 28)
                {
                    g_DlcParameters.ucDlcAlgorithmMode = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 29)
                {
                    g_DlcParameters.ucDlcSepPointH = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 30)
                {
                    g_DlcParameters.ucDlcSepPointL = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 31)
                {
                    g_DlcParameters.uwDlcBleStartPointTH = (WORD)((*(pCmdBuff+6)<<8)+(*(pCmdBuff+5)));
                }
                else if (*(pCmdBuff+4) == 32)
                {
                    g_DlcParameters.uwDlcBleEndPointTH = (WORD)((*(pCmdBuff+6)<<8)+(*(pCmdBuff+5)));
                }
                else if (*(pCmdBuff+4) == 33)
                {
                    g_DlcParameters.ucDlcCurveDiff_L_TH = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 34)
                {
                    g_DlcParameters.ucDlcCurveDiff_H_TH = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 35)
                {
                    g_DlcParameters.uwDlcBLESlopPoint_1 = (WORD)((*(pCmdBuff+6)<<8)+(*(pCmdBuff+5)));
                }
                else if (*(pCmdBuff+4) == 36)
                {
                    g_DlcParameters.uwDlcBLESlopPoint_2 = (WORD)((*(pCmdBuff+6)<<8)+(*(pCmdBuff+5)));
                }
                else if (*(pCmdBuff+4) == 37)
                {
                    g_DlcParameters.uwDlcBLESlopPoint_3 = (WORD)((*(pCmdBuff+6)<<8)+(*(pCmdBuff+5)));
                }
                else if (*(pCmdBuff+4) == 38)
                {
                    g_DlcParameters.uwDlcBLESlopPoint_4 = (WORD)((*(pCmdBuff+6)<<8)+(*(pCmdBuff+5)));
                }
                else if (*(pCmdBuff+4) == 39)
                {
                    g_DlcParameters.uwDlcBLESlopPoint_5 = (WORD)((*(pCmdBuff+6)<<8)+(*(pCmdBuff+5)));
                }
                else if (*(pCmdBuff+4) == 40)
                {
                    g_DlcParameters.uwDlcDark_BLE_Slop_Min = (WORD)((*(pCmdBuff+6)<<8)+(*(pCmdBuff+5)));
                }
                else if (*(pCmdBuff+4) == 41)
                {
                    g_DlcParameters.ucDlcCurveDiffCoringTH = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 42)
                {
                    g_DlcParameters.ucDlcAlphaBlendingMin = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 43)
                {
                    g_DlcParameters.ucDlcAlphaBlendingMax = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 44)
                {
                    g_DlcParameters.ucDlcFlicker_alpha= *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 45)
                {
                    g_DlcParameters.ucDlcYAVG_L_TH = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 46)
                {
                    g_DlcParameters.ucDlcYAVG_H_TH = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 47)
                {
                    g_DlcParameters.ucDlcDiffBase_L= *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 48)
                {
                    g_DlcParameters.ucDlcDiffBase_M = *(pCmdBuff+5);
                }
                else if (*(pCmdBuff+4) == 49)
                {
                    g_DlcParameters.ucDlcDiffBase_H = *(pCmdBuff+5);
                }
                msDlcSpeedupTrigger(16);
            }
            break;
        case URCMD_MST_DLC_TEST_REINIT:
            msDlc_AP_ReinitWithoutPara();
            msDlcOnOff( _ENABLE, MAIN_WINDOW);
            break;
        case URCMD_MST_DLC_TEST_CGC_REINIT:
            msDlc_CGC_ReInit();
            break;
        case URCMD_MST_DLC_TEST_READ_Sloplimit:
            {
                putchar(0xF2);
                putchar(g_DlcParameters.ucDlcHistogramLimitCurve[*(pCmdBuff+4)]);
            }
            break;
        case URCMD_MST_DLC_TEST_WRITE_Sloplimit:
            {
                BYTE i;
              #if 1//( ENABLE_UART_CHECKSUM )
                ucTmpLen--; // remove checksum byte
              #endif
                for (i=0; i<(ucTmpLen-4-1); i++)
                    g_DlcParameters.ucDlcHistogramLimitCurve[*(pCmdBuff+4) + i] = *(pCmdBuff+5 + i);
            }
            break;
    }
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
WORD msDlcLibVersionCheck(void)
{
    return DLC_LIB_VERSION;
}

WORD msDlcCGCLibVersionCheck(void)
{
    return DLC_CGC_LIB_VERSION;
}

#undef _MSDLC_C_
