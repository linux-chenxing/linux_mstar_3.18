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
#define _MSDBCCODE_C_


// System
#include "MsDBC_LIB.h"
// Common



//#define DBC_DEBUG
#ifdef DBC_DEBUG
    #define DBC_PRINTF(str,val) printf(str,val)
    #define DBC_PUTSTR(str)     putstr(str)
#else
    #define DBC_PRINTF(str,val)
    #define DBC_PUTSTR(str)
#endif


#define DBC_LIB_VERSION             0x305 // v3.05 add Y/C Gain On Off
                                          // v3.04 Include the definition of debug commands
                                          // v3.03 Include DebugTool Command & add more commands (currentPWM, print message, debug mode)
                                          // v3.02 made struct for parameters & remove the subroutine of backlight control
                                          // v3.01 remove g_bDBCEnable from DBC lib
                                          // v3.00 add g_ucCurrentAvg, g_ucCurrentYGain, g_ucCurrentCGain by Derek & Joseph
                                          // v2.00 add fast mode (like as DLC)
                                          // v1.03 update msGetAverageLuminousValue, remove g_DbcParameters.ucCurrentPWM in DBCInit, update the last calculation bug
                                          // v1.02 update the last calculation bug of g_DbcParameters.ucCurrentPWM
                                          // v1.01 update formula of curve MiddleHigh
                                          // v1.00 new add DBC function
// DBC
XDATA BYTE g_ucAverageLumaValue;
XDATA WORD g_wPrePWM;
XDATA BYTE g_ucCurrentAvg;
XDATA BYTE g_ucCurrentYGain;
XDATA BYTE g_ucCurrentCGain;
XDATA BYTE g_ucDBCPreAvg;
XDATA BYTE g_ucDBCFastLoop; // for pulse ripple
XDATA BOOL g_bDBC_YGainOnOff, g_bDBC_CGainOnOff;

// For Tool Debug
XDATA BYTE g_ucDBC_DebugMode = 0; // [0] DBC On/Off, [7] Debug Mode Enable, [6] Print message

/*
typedef struct
{
    BYTE ucMaxPWM;
    BYTE ucMinPWM;
    BYTE ucMax_Video;
    BYTE ucMid_Video;
    BYTE ucMin_Video;
    BYTE ucCurrentPWM;
    BYTE ucAlpha;
    BYTE ucBackLight_Thres;
    BYTE ucAvgDelta;
    BYTE ucFlickAlpha;
    BYTE ucFastAlphaBlending; // min 17 ~ max 32
    BYTE ucLoop_Dly;
    BYTE ucLoop_Dly_H_Init;
    BYTE ucLoop_Dly_MH_Init;
    BYTE ucLoop_Dly_ML_Init;
    BYTE ucLoop_Dly_L_Init;
    BYTE ucY_Gain_H;
    BYTE ucC_Gain_H;
    BYTE ucY_Gain_M;
    BYTE ucC_Gain_M;
    BYTE ucY_Gain_L;
    BYTE ucC_Gain_L;
    BYTE bYGainCtrl : 1; // 1: enable; 0: disable
    BYTE bCGainCtrl : 1; // 1: enable; 0: disable
}StuDbc_FinetuneParamaters_Lib;
*/
XDATA StuDbc_FinetuneParamaters_Lib g_DbcParameters;

extern XDATA BYTE g_ucTmpAvgN_1; // Avg value (N-1) // from DLC LIB
extern XDATA BYTE g_ucTmpAvgN; // Avg value (current N) // from DLC LIB


//-----------------------------------------------------------------------
// due to patent issues, temporary DISABLE it
//-----------------------------------------------------------------------
#if (ENABLE_DBC )  

#define DBC_DEFLICK_BLEND_FACTOR      32
#define DBC_DEFLICK_PRECISION_SHIFT   64

void msDBCLumiDiffCtrl(void);
void msAdjustYCGain(BYTE Y_Gain,BYTE C_Gain);

WORD msDbcLibVersionCheck(void)
{
    return DBC_LIB_VERSION;
}

void msDBCInit()
{
    g_ucAverageLumaValue = 0;

    g_DbcParameters.ucLoop_Dly = g_DbcParameters.ucLoop_Dly_H_Init;
    g_ucCurrentAvg = g_ucTmpAvgN;
    g_ucDBCPreAvg = g_ucTmpAvgN;

    g_ucDBCFastLoop = (g_DbcParameters.ucFastAlphaBlending/2);
    g_wPrePWM = (WORD)g_DbcParameters.ucCurrentPWM * DBC_DEFLICK_PRECISION_SHIFT;

    // Derek add for DBC Y/C gain adjust 2008/10/12
    g_ucCurrentYGain=g_DbcParameters.ucY_Gain_H;
    g_ucCurrentCGain=g_DbcParameters.ucC_Gain_H;
    msAdjustYCGain(g_ucCurrentYGain,g_ucCurrentCGain);
}

void msDBCLumiDiffCtrl(void)
{
    if (g_ucTmpAvgN_1 >= g_ucTmpAvgN)
    {
        if ((g_ucDBCFastLoop == 0) && ((g_ucTmpAvgN_1 - g_ucTmpAvgN) < g_DbcParameters.ucAvgDelta)) // if (Yavg is less than g_DbcParameters.ucAvgDelta) & (Fast change is finish) may do curve change slowly
        {
            g_DbcParameters.ucFlickAlpha = 1; // (1/32) = (1/DLC_DEFLICK_BLEND_FACTOR)  // it mean the curve change slowly
        }
        else
        {
            DBC_PRINTF("\r\n1H:Avg = %d",g_ucTmpAvgN_1 - g_ucTmpAvgN);
            DBC_PRINTF(" g_ucDlcFlickAlpha = %d\r\n",g_DbcParameters.ucFlickAlpha);

            g_DbcParameters.ucFlickAlpha = g_DbcParameters.ucFastAlphaBlending; // (32/32) = (32/DLC_DEFLICK_BLEND_FACTOR) // it mean the curve change quickly
            if (g_ucDBCFastLoop == 0)
                g_ucDBCFastLoop = (g_DbcParameters.ucFastAlphaBlending/2);
        }
    }
    else // if (g_wHistogramTotal < wTmpAvg)
    {
        if ((g_ucDBCFastLoop == 0) && ((g_ucTmpAvgN - g_ucTmpAvgN_1) < g_DbcParameters.ucAvgDelta)) // if (Yavg is less than g_DbcParameters.ucAvgDelta) & (Fast change is finish) may do curve change slowly
        {
            g_DbcParameters.ucFlickAlpha = 1; // (1/32) = (1/DLC_DEFLICK_BLEND_FACTOR)  // it mean the curve change slowly
        }
        else
        {
            DBC_PRINTF("\r\n2H:Avg = %d",g_ucTmpAvgN - g_ucTmpAvgN_1);
            DBC_PRINTF(" g_ucDlcFlickAlpha = %d\r\n",g_DbcParameters.ucFlickAlpha);

            g_DbcParameters.ucFlickAlpha = g_DbcParameters.ucFastAlphaBlending; // (32/32) = (32/DLC_DEFLICK_BLEND_FACTOR)  // it mean the curve change quickly
            if (g_ucDBCFastLoop == 0)
                g_ucDBCFastLoop = (g_DbcParameters.ucFastAlphaBlending/2);
        }
    }
}

WORD msBackLightHandler(void)
{
    BYTE ucTargetBacklight;
    BYTE ucCurveIndex;
    WORD wBlend, wTemp;

    if (!(g_ucDBC_DebugMode & _BIT0)) // [0] DBC On/Off
        return 0xFFFF; // Do Nothing

    if (g_ucDBC_DebugMode & _BIT7) // [7] Debug Mode Enable
        return 0xFFFF; // Do Nothing

    g_ucCurrentAvg = g_ucTmpAvgN;

    if (g_ucDBC_DebugMode & _BIT6)
        DBC_PRINTF("\r\n[DBC]YAvg = %d",g_ucCurrentAvg);

    msDBCLumiDiffCtrl();

    g_ucCurrentAvg =(WORD)(((DWORD)g_DbcParameters.ucAlpha * g_ucCurrentAvg+(DWORD)g_ucDBCPreAvg * (100-g_DbcParameters.ucAlpha))/100);
    DBC_PRINTF("\r\n[DBC]Avg2 = %d",g_ucCurrentAvg);

    if (g_ucCurrentAvg >= g_DbcParameters.ucMax_Video)
    {
        DBC_PUTSTR("\r\n[DBC] 00");
        ucTargetBacklight = g_DbcParameters.ucMaxPWM;
        ucCurveIndex = 0; // curve High
    }
    else if (g_ucCurrentAvg <= g_DbcParameters.ucMin_Video)
    {
        DBC_PUTSTR("\r\n[DBC] 01");
        ucTargetBacklight = g_DbcParameters.ucMinPWM;
        ucCurveIndex = 3; // curve Low
    }
    else if (g_ucCurrentAvg >= g_DbcParameters.ucMid_Video)
    {
        DBC_PUTSTR("\r\n[DBC] 02");
        ucTargetBacklight = ((WORD)g_ucCurrentAvg - g_DbcParameters.ucMid_Video) * ((WORD)(100 - g_DbcParameters.ucBackLight_Thres) * (g_DbcParameters.ucMaxPWM - g_DbcParameters.ucMinPWM) / 100)
                                / (g_DbcParameters.ucMax_Video - g_DbcParameters.ucMid_Video) + ((WORD)g_DbcParameters.ucBackLight_Thres * (g_DbcParameters.ucMaxPWM - g_DbcParameters.ucMinPWM) / 100) + g_DbcParameters.ucMinPWM;
        ucCurveIndex = 1; // curve MiddleHigh
    }
    else
    {
        DBC_PUTSTR("\r\n[DBC] 03");
        ucTargetBacklight = ((WORD)(g_ucCurrentAvg - g_DbcParameters.ucMin_Video) * ((WORD)g_DbcParameters.ucBackLight_Thres * (g_DbcParameters.ucMaxPWM - g_DbcParameters.ucMinPWM) / 100))
                              / (g_DbcParameters.ucMid_Video - g_DbcParameters.ucMin_Video) + g_DbcParameters.ucMinPWM;
        ucCurveIndex = 2; // curve MiddleLow
    }

    DBC_PRINTF("\r\n[DBC]ucTargetBacklight_orig=%d",ucTargetBacklight);

    g_ucDBCPreAvg = g_ucCurrentAvg;

    DBC_PRINTF("\r\n[DBC]ucTargetBacklight=%d",ucTargetBacklight);
    DBC_PRINTF("\r\n[DBC]g_wPrePWM=%d",g_wPrePWM);

    if(g_ucDBCFastLoop || (g_DbcParameters.ucFlickAlpha >= 16)) // Fast Update
    {
        if (g_ucDBCFastLoop)
            g_ucDBCFastLoop--;

        if (g_DbcParameters.ucCurrentPWM == ucTargetBacklight)
        {
            g_wPrePWM = (WORD)g_DbcParameters.ucCurrentPWM * DBC_DEFLICK_PRECISION_SHIFT;

            if (g_ucDBC_DebugMode & _BIT6)
                DBC_PRINTF("\r\n[DBC]0 PWM = %d",g_DbcParameters.ucCurrentPWM);

            return (WORD)g_DbcParameters.ucCurrentPWM;
        }

        // target value with alpha blending
        wBlend = (WORD)((((DWORD) g_DbcParameters.ucFlickAlpha * ucTargetBacklight * DBC_DEFLICK_PRECISION_SHIFT) +
                     ((DBC_DEFLICK_BLEND_FACTOR - (WORD)g_DbcParameters.ucFlickAlpha) * g_wPrePWM))/DBC_DEFLICK_BLEND_FACTOR);

        DBC_PRINTF("\r\n[DBC]g_wPrePWM=%d",g_wPrePWM);

        if(wBlend > g_wPrePWM)
        {
            wBlend = g_wPrePWM + ((wBlend - g_wPrePWM) / (DBC_DEFLICK_BLEND_FACTOR - g_DbcParameters.ucFlickAlpha + 1));
        }
        else if(wBlend < g_wPrePWM)
        {
            wBlend = g_wPrePWM - ((g_wPrePWM - wBlend) / (DBC_DEFLICK_BLEND_FACTOR - g_DbcParameters.ucFlickAlpha + 1));
        }

        g_wPrePWM = wBlend;

        if (g_DbcParameters.ucCurrentPWM == g_wPrePWM / DBC_DEFLICK_PRECISION_SHIFT)
        {
            if (g_ucDBC_DebugMode & _BIT6)
                DBC_PRINTF("\r\n[DBC]1 PWM = %d",g_DbcParameters.ucCurrentPWM);

            return (WORD)g_DbcParameters.ucCurrentPWM;
        }

        g_DbcParameters.ucCurrentPWM = g_wPrePWM / DBC_DEFLICK_PRECISION_SHIFT;
    }
    else // Slow Update
    {
        if (g_DbcParameters.ucLoop_Dly--)
            return 0xFFFF; // Do Nothing

        switch(ucCurveIndex)
        {
            case 0: // curve High
                g_DbcParameters.ucLoop_Dly = g_DbcParameters.ucLoop_Dly_H_Init;
                break;
            case 1: // curve MiddleHigh
                g_DbcParameters.ucLoop_Dly = g_DbcParameters.ucLoop_Dly_MH_Init;
                break;
            case 2: // curve MiddleLow
                g_DbcParameters.ucLoop_Dly = g_DbcParameters.ucLoop_Dly_ML_Init;
                break;
            case 3: // curve Low
                g_DbcParameters.ucLoop_Dly = g_DbcParameters.ucLoop_Dly_L_Init;
                break;
        }

        if (g_DbcParameters.ucCurrentPWM == ucTargetBacklight)
        {
            g_wPrePWM = (WORD)g_DbcParameters.ucCurrentPWM * DBC_DEFLICK_PRECISION_SHIFT;

            if (g_ucDBC_DebugMode & _BIT6)
                DBC_PRINTF("\r\n[DBC]2 PWM = %d",g_DbcParameters.ucCurrentPWM);

            return (WORD)g_DbcParameters.ucCurrentPWM;
        }

        DBC_PRINTF("\r\n[DBC]ucTargetBacklight=%d",ucTargetBacklight);
        DBC_PRINTF("\r\n[DBC]g_DbcParameters.ucCurrentPWM=%d",g_DbcParameters.ucCurrentPWM);

        if (((g_DbcParameters.ucCurrentPWM+1) <= ucTargetBacklight) && (g_DbcParameters.ucCurrentPWM <= (255-1)))
            g_DbcParameters.ucCurrentPWM+=1;
        else if (((g_DbcParameters.ucCurrentPWM-1) >= ucTargetBacklight) && (g_DbcParameters.ucCurrentPWM >= 1))
            g_DbcParameters.ucCurrentPWM-=1;

        g_wPrePWM = (WORD)g_DbcParameters.ucCurrentPWM * DBC_DEFLICK_PRECISION_SHIFT;
    }

    DBC_PRINTF("\r\n S g_DbcParameters.ucCurrentPWM = %d",g_DbcParameters.ucCurrentPWM);

    wTemp=((WORD)((g_DbcParameters.ucBackLight_Thres) * (g_DbcParameters.ucMaxPWM - g_DbcParameters.ucMinPWM)) / 100) +   g_DbcParameters.ucMinPWM;
    DBC_PRINTF("\r\n[DBC]wTemp=%d",wTemp);

    if(g_DbcParameters.ucCurrentPWM <= g_DbcParameters.ucMinPWM)
    {
        DBC_PUTSTR("\r\n[DBC] 11");
        g_ucCurrentYGain = g_DbcParameters.ucY_Gain_L;
        g_ucCurrentCGain = g_DbcParameters.ucC_Gain_L;
    }
    else if ((g_DbcParameters.ucCurrentPWM>g_DbcParameters.ucMinPWM)&&(g_DbcParameters.ucCurrentPWM<wTemp))
    {
        DBC_PUTSTR("\r\n[DBC] 22");
        g_ucCurrentYGain = g_DbcParameters.ucY_Gain_L-(((WORD)abs((g_DbcParameters.ucY_Gain_L-g_DbcParameters.ucY_Gain_M))) * ((WORD)abs((g_DbcParameters.ucCurrentPWM-g_DbcParameters.ucMinPWM))))/((WORD)abs((wTemp-g_DbcParameters.ucMinPWM))) ;
        g_ucCurrentCGain = g_DbcParameters.ucC_Gain_L-(((WORD)abs((g_DbcParameters.ucC_Gain_L-g_DbcParameters.ucC_Gain_M))) * ((WORD)abs((g_DbcParameters.ucCurrentPWM-g_DbcParameters.ucMinPWM))))/((WORD)abs((wTemp-g_DbcParameters.ucMinPWM))) ;
    }
    else if ((g_DbcParameters.ucCurrentPWM>=wTemp) && (g_DbcParameters.ucCurrentPWM<g_DbcParameters.ucMaxPWM))
    {
        DBC_PUTSTR("\r\n[DBC] 33");
        g_ucCurrentYGain = g_DbcParameters.ucY_Gain_M-(((WORD)abs((g_DbcParameters.ucY_Gain_M-g_DbcParameters.ucY_Gain_H))) * ((WORD)abs((g_DbcParameters.ucCurrentPWM-wTemp))))/((WORD)abs((g_DbcParameters.ucMaxPWM-wTemp))) ;
        g_ucCurrentCGain = g_DbcParameters.ucC_Gain_M-(((WORD)abs((g_DbcParameters.ucC_Gain_M-g_DbcParameters.ucC_Gain_H))) * ((WORD)abs((g_DbcParameters.ucCurrentPWM-wTemp))))/((WORD)abs((g_DbcParameters.ucMaxPWM-wTemp))) ;
    }
    else if (g_DbcParameters.ucCurrentPWM>=g_DbcParameters.ucMaxPWM)
    {
        DBC_PUTSTR("\r\n[DBC] 44");
        g_ucCurrentYGain = g_DbcParameters.ucY_Gain_H;
        g_ucCurrentCGain = g_DbcParameters.ucC_Gain_H;
    }

    msAdjustYCGain(g_ucCurrentYGain,g_ucCurrentCGain);

    if (g_ucDBC_DebugMode & _BIT6)
    {
        DBC_PRINTF("\r\n[DBC]3 PWM = %d",g_DbcParameters.ucCurrentPWM);
        DBC_PRINTF("\r\n[DBC]YGain = %d",g_ucCurrentYGain);
        DBC_PRINTF("\r\n[DBC]CGain = %d",g_ucCurrentCGain);
    }

    return (WORD)g_DbcParameters.ucCurrentPWM;
}

// Derek add for DBC Y/C gain adjust 2008/10/12
void msAdjustYCGain(BYTE Y_Gain,BYTE C_Gain)
{
    msDBC_FunctionEnter();

    if (g_DbcParameters.bYGainCtrl && (REG_ADDR_DBC_Y_GAIN != REG_NULL))
        msWriteByte(REG_ADDR_DBC_Y_GAIN, Y_Gain);
    if (g_DbcParameters.bCGainCtrl && (REG_ADDR_DBC_C_GAIN != REG_NULL))
        msWriteByte(REG_ADDR_DBC_C_GAIN, C_Gain);

    msDBC_FunctionExit();
}

//////////////////////////////////////////////////////////////
// Debug Protocol
//////////////////////////////////////////////////////////////

// Debug Command Paser
#define URCMD_MST_DBC_TEST_READ_INFO    1
#define URCMD_MST_DBC_TEST_WRITE_INFO   2
#define URCMD_MST_DBC_TEST_REINIT       3

void msDBC_ParametersTool(MS_U8 *pCmdBuff, MDrv_DLC_DBC_OnOff_Callback fnDBC_AP_OnOff, MDrv_DLC_DBC_AdjustBackLight_Callback fnDBC_AdjustBacklight, MDrv_DBC_Print_Callback fnPutchar )
{
    switch(*(pCmdBuff+3)) //UART_EXT_CMD
    {
        case URCMD_MST_DBC_TEST_READ_INFO:
            {
                if (*(pCmdBuff+4) == 2) // DBC other
                {
                    if (*(pCmdBuff+5) == 200)
                    {
                        WORD wVer;
                        wVer = msDbcLibVersionCheck();
                        fnPutchar(0xF3);
                        fnPutchar(wVer >> 8);
                        fnPutchar(wVer & 0xFF);
                    }
                    else
                    {
                        fnPutchar(0xF2);
                        if (*(pCmdBuff+5) == 10)
                            fnPutchar(g_DbcParameters.ucMax_Video);
                        else if (*(pCmdBuff+5) == 11)
                            fnPutchar(g_DbcParameters.ucMid_Video);
                        else if (*(pCmdBuff+5) == 12)
                            fnPutchar(g_DbcParameters.ucMin_Video);
                        else if (*(pCmdBuff+5) == 13)
                            fnPutchar(g_DbcParameters.ucMaxPWM);
                        else if (*(pCmdBuff+5) == 14)
                            fnPutchar(g_DbcParameters.ucBackLight_Thres);
                        else if (*(pCmdBuff+5) == 15)
                            fnPutchar(g_DbcParameters.ucMinPWM);
                        else if (*(pCmdBuff+5) == 16)
                            fnPutchar(g_DbcParameters.ucY_Gain_H);
                        else if (*(pCmdBuff+5) == 17)
                            fnPutchar(g_DbcParameters.ucY_Gain_M);
                        else if (*(pCmdBuff+5) == 18)
                            fnPutchar(g_DbcParameters.ucY_Gain_L);
                        else if (*(pCmdBuff+5) == 19)
                            fnPutchar(g_DbcParameters.ucC_Gain_H);
                        else if (*(pCmdBuff+5) == 20)
                            fnPutchar(g_DbcParameters.ucC_Gain_M);
                        else if (*(pCmdBuff+5) == 21)
                            fnPutchar(g_DbcParameters.ucC_Gain_L);
                        else if (*(pCmdBuff+5) == 22)
                            fnPutchar(g_DbcParameters.ucAlpha);
                        else if (*(pCmdBuff+5) == 23)
                            fnPutchar(g_DbcParameters.ucAvgDelta);
                        else if (*(pCmdBuff+5) == 24)
                            fnPutchar(g_DbcParameters.ucFastAlphaBlending);
                        else if (*(pCmdBuff+5) == 25)
                            fnPutchar(g_DbcParameters.ucLoop_Dly_H_Init);
                        else if (*(pCmdBuff+5) == 26)
                            fnPutchar(g_DbcParameters.ucLoop_Dly_MH_Init);
                        else if (*(pCmdBuff+5) == 27)
                            fnPutchar(g_DbcParameters.ucLoop_Dly_ML_Init);
                        else if (*(pCmdBuff+5) == 28)
                            fnPutchar(g_DbcParameters.ucLoop_Dly_L_Init);
                        else if (*(pCmdBuff+5) == 29)
                            fnPutchar(g_ucDBC_DebugMode);
                        else if (*(pCmdBuff+5) == 30)
                            fnPutchar(g_DbcParameters.ucCurrentPWM);
                        else if (*(pCmdBuff+5) == 31)
                            fnPutchar(g_DbcParameters.bYGainCtrl);
                        else if (*(pCmdBuff+5) == 32)
                            fnPutchar(g_DbcParameters.bCGainCtrl);
                        else
                            fnPutchar(0);
                    }
                }
            }
            break;
        case URCMD_MST_DBC_TEST_WRITE_INFO:
            {
                if (*(pCmdBuff+4) == 10)
                    g_DbcParameters.ucMax_Video = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 11)
                    g_DbcParameters.ucMid_Video = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 12)
                    g_DbcParameters.ucMin_Video = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 13)
                    g_DbcParameters.ucMaxPWM= *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 14)
                    g_DbcParameters.ucBackLight_Thres = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 15)
                    g_DbcParameters.ucMinPWM = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 16)
                    g_DbcParameters.ucY_Gain_H = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 17)
                    g_DbcParameters.ucY_Gain_M = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 18)
                    g_DbcParameters.ucY_Gain_L = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 19)
                    g_DbcParameters.ucC_Gain_H = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 20)
                    g_DbcParameters.ucC_Gain_M = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 21)
                    g_DbcParameters.ucC_Gain_L = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 22)
                    g_DbcParameters.ucAlpha = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 23)
                    g_DbcParameters.ucAvgDelta = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 24)
                    g_DbcParameters.ucFastAlphaBlending = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 25)
                    g_DbcParameters.ucLoop_Dly_H_Init = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 26)
                    g_DbcParameters.ucLoop_Dly_MH_Init = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 27)
                    g_DbcParameters.ucLoop_Dly_ML_Init = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 28)
                    g_DbcParameters.ucLoop_Dly_L_Init = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 29)
                {
                    g_ucDBC_DebugMode = *(pCmdBuff+5);
                    fnDBC_AP_OnOff(g_ucDBC_DebugMode && _BIT0);
                }
                else if (*(pCmdBuff+4) == 30)
                {
                    g_DbcParameters.ucCurrentPWM = *(pCmdBuff+5);
                    fnDBC_AdjustBacklight(g_DbcParameters.ucCurrentPWM);
                }
                else if (*(pCmdBuff+4) == 31)
                    g_DbcParameters.bYGainCtrl = *(pCmdBuff+5);
                else if (*(pCmdBuff+4) == 32)
                    g_DbcParameters.bCGainCtrl = *(pCmdBuff+5);
            }
            break;
        case URCMD_MST_DBC_TEST_REINIT:
            break;
    }
}

#endif//ENABLE_DBC
