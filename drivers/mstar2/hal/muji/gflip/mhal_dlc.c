///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2008 - 2009 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mhal_gflip.c
/// @brief  MStar GFLIP DDI HAL LEVEL
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _HAL_DLC_C

//=============================================================================
// Include Files
//=============================================================================

#include "mhal_dlc.h"
//#include <linux/kernel.h>   /* printk() */


//=============================================================================
// Compile options
//=============================================================================

//=============================================================================
// Debug Macros
//=============================================================================

//=============================================================================
// Macros
//=============================================================================

#define HDR_DBG_HAL(x)  //x
#define HDR_DEBUG_MODE   0

#define DLC_CURVE_SIZE 18

static volatile MS_U16 g_u16HdrY2RRatio = 2;
static volatile MS_U16 g_u16Hdr3x3Ratio = 8;

static volatile MS_U8 _u8PreColorPrimaries=0;
static volatile MS_U8 _u8PreTransferCharacteristics=0;

static volatile MS_U8 _u8Colorimetry=0;
static volatile MS_U8 _u8ExtendedColorimetry=0;
static volatile MS_U8 MaxucTmp;
static volatile MS_U16 MaxHistogram32H;
static volatile MS_U16 _u16Count=0;
static volatile MS_U16 u16PreRegs[18]={0};
static volatile MS_BOOL _bSetGenRead=FALSE;

static volatile MS_BOOL _bSetDLCHDRInit=FALSE;

#if defined(CONFIG_ARM64)
static volatile MS_U64 g_u64MstarPmBase = 0;
#endif

//=============================================================================
// Local Variables
//=============================================================================

#if DLC_FUNCTION

static volatile StuDlc_FinetuneParamaters g_DlcParameters;

static volatile WORD g_wLumaHistogram32H[32];

static volatile MS_BOOL g_bSetDLCCurveBoth = 0;//We will set main/sub DLC curve at the same time in 3D side-side ap.
static volatile MS_BOOL g_bSetDlcBleOn = TRUE; //TURE : Enable BLE   ; FALSE : Disnable BLE

static volatile BYTE g_ucHistogramMax, g_ucHistogramMin;
static volatile BYTE g_ucHistogramTotal_H;
static volatile BYTE g_ucHistogramTotal_L;
static volatile BYTE g_ucDlcFlickAlpha;
static volatile BYTE g_ucUpdateCnt;
static volatile BYTE g_ucDlcFastLoop; // for pulse ripple
static volatile BYTE g_ucTmpAvgN_1; // Avg value (N-1)
static volatile BYTE g_ucTmpAvgN; // Avg value (current N)
static volatile BYTE g_ucCurveYAvg[16];
static volatile BYTE g_ucTable[16]; // Final target curve (8 bit)
static volatile BYTE g_pre_ucTable[16]; // Final target curve (8 bit)

static volatile WORD g_wLumiAverageTemp;
static volatile WORD g_uwTable[16]; // Final target curve (10 bit)
static volatile WORD g_uwPre_CurveHistogram[16];
static volatile WORD g_uwPreTable[16]; // New de-Flick
static volatile WORD g_wTmpAvgN_1_x10; // Avg value (N-1) x 10
static volatile WORD g_wTmpAvgN_x10; // Avg value (current N) x 10
static volatile WORD g_wLumiTotalCount;
static volatile WORD g_wDisWinVstart=0, g_wDisWinVend=0 ,g_wDisWinHstart=0, g_wDisWinHend=0 ;
static volatile WORD g_wPIAlpha=0;
static volatile WORD g_wPrePIAlpha=0;
static volatile BYTE g_ucDLCInitBuffer[182];
static volatile DWORD g_uwCurveHistogram[16];

volatile StuDlc_HDRinit g_HDRinitParameters;

HDR_ToneMappingData HDRtmp; //YT ForHDR
WORD HDR_Curve[18]; //YT ForHDR


void msDlcWriteCurve(MS_BOOL bWindow);

void msWriteByte(DWORD u32Reg, BYTE u8Val ) ;
BYTE msReadByte(DWORD u32Reg ) ;
#endif
//=============================================================================
// Global Variables
//=============================================================================

//=============================================================================================
// Local Function
//=============================================================================================


#if DLC_FUNCTION
//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void msWriteByte(DWORD u32Reg, BYTE u8Val )
{
    if(u32Reg%2)
    {
        REG_WH((u32Reg-1), u8Val);
    }
    else
    {
        REG_WL(u32Reg, u8Val);
    }
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
BYTE msReadByte(DWORD u32Reg )
{
    if(u32Reg%2)
    {
        u32Reg = u32Reg-1 ;
        return ((REG_RR(u32Reg) & 0xFF00)>>8);
    }
    else
    {
        return (REG_RR(u32Reg) & 0xFF);
    }
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void MApi_GFLIP_XC_W2BYTE(DWORD u32Reg, WORD u16Val )
{
    REG_W2B(u32Reg, u16Val);
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
WORD MApi_GFLIP_XC_R2BYTE(DWORD u32Reg )
{
    return REG_RR(u32Reg) ;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void MApi_GFLIP_XC_W2BYTEMSK(DWORD u32Reg, WORD u16Val, WORD u16Mask )
{
    WORD u16Data=0 ;
    u16Data = REG_RR(u32Reg);
    u16Data = (u16Data & (0xFFFF-u16Mask))|(u16Val &u16Mask);
    REG_W2B(u32Reg, u16Data);
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
WORD MApi_GFLIP_XC_R2BYTEMSK(DWORD u32Reg, WORD u16Mask )
{
    return (REG_RR(u32Reg) & u16Mask);
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void msDlcSetCurve(MS_U8 *_u8DlcCurveInit)
{
    BYTE ucTmp;

    for(ucTmp=0; ucTmp<16; ucTmp++)
    {
        g_DlcParameters.ucLumaCurve[ucTmp]=_u8DlcCurveInit[ucTmp];
        g_DlcParameters.ucLumaCurve2_a[ucTmp]=_u8DlcCurveInit[ucTmp+16];
        g_DlcParameters.ucLumaCurve2_b[ucTmp]=_u8DlcCurveInit[ucTmp+32];
        g_ucDLCInitBuffer[70+ucTmp]=_u8DlcCurveInit[ucTmp];
        g_ucDLCInitBuffer[86+ucTmp]=_u8DlcCurveInit[ucTmp+16];
        g_ucDLCInitBuffer[102+ucTmp]=_u8DlcCurveInit[ucTmp+32];
    }

    for(ucTmp=0; ucTmp<17; ucTmp++)
    {
        g_DlcParameters.ucDlcHistogramLimitCurve[ucTmp]=_u8DlcCurveInit[ucTmp+48];
        g_ucDLCInitBuffer[37+ucTmp]=_u8DlcCurveInit[ucTmp+48];
    }
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void msDlcSetBleSlopPoint(MS_U16 *_u16BLESlopPoint)
{
    g_DlcParameters.uwDlcBLESlopPoint_1 = _u16BLESlopPoint[0];
    g_DlcParameters.uwDlcBLESlopPoint_2 = _u16BLESlopPoint[1];
    g_DlcParameters.uwDlcBLESlopPoint_3 = _u16BLESlopPoint[2];
    g_DlcParameters.uwDlcBLESlopPoint_4 = _u16BLESlopPoint[3];
    g_DlcParameters.uwDlcBLESlopPoint_5 = _u16BLESlopPoint[4];
    g_DlcParameters.uwDlcDark_BLE_Slop_Min = _u16BLESlopPoint[5];

    g_ucDLCInitBuffer[7]=(BYTE)(g_DlcParameters.uwDlcBLESlopPoint_1 &0xFF);
    g_ucDLCInitBuffer[8]=(BYTE)((g_DlcParameters.uwDlcBLESlopPoint_1>>8) &0xFF);
    g_ucDLCInitBuffer[9]=(BYTE)(g_DlcParameters.uwDlcBLESlopPoint_2 &0xFF);
    g_ucDLCInitBuffer[10]=(BYTE)((g_DlcParameters.uwDlcBLESlopPoint_2>>8) &0xFF);
    g_ucDLCInitBuffer[11]=(BYTE)(g_DlcParameters.uwDlcBLESlopPoint_3 &0xFF);
    g_ucDLCInitBuffer[12]=(BYTE)((g_DlcParameters.uwDlcBLESlopPoint_3>>8) &0xFF);
    g_ucDLCInitBuffer[13]=(BYTE)(g_DlcParameters.uwDlcBLESlopPoint_4 &0xFF);
    g_ucDLCInitBuffer[14]=(BYTE)((g_DlcParameters.uwDlcBLESlopPoint_4>>8) &0xFF);
    g_ucDLCInitBuffer[15]=(BYTE)(g_DlcParameters.uwDlcBLESlopPoint_5 &0xFF);
    g_ucDLCInitBuffer[16]=(BYTE)((g_DlcParameters.uwDlcBLESlopPoint_5>>8) &0xFF);
    g_ucDLCInitBuffer[17]=(BYTE)(g_DlcParameters.uwDlcDark_BLE_Slop_Min &0xFF);
    g_ucDLCInitBuffer[18]=(BYTE)((g_DlcParameters.uwDlcDark_BLE_Slop_Min>>8) &0xFF);

}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
MS_BOOL msDlcInit(MS_U16 *u16DlcInit )
{
    BYTE ucTmp=0;

    g_DlcParameters.ucDlcPureImageMode     = u16DlcInit[10]; // Compare difference of max and min bright
    g_DlcParameters.ucDlcLevelLimit        = u16DlcInit[11]; // n = 0 ~ 4 => Limit n levels => ex. n=2, limit 2 level 0xF7, 0xE7
    g_DlcParameters.ucDlcAvgDelta          = u16DlcInit[12]; // n = 0 ~ 50, default value: 12
    g_DlcParameters.ucDlcAvgDeltaStill     = u16DlcInit[13]; // n = 0 ~ 15 => 0: disable still curve, 1 ~ 15: enable still curve
    g_DlcParameters.ucDlcFastAlphaBlending = u16DlcInit[14]; // min 17 ~ max 32
    g_DlcParameters.ucDlcYAvgThresholdL    = u16DlcInit[15]; // default value: 0
    g_DlcParameters.ucDlcYAvgThresholdH    = u16DlcInit[16]; // default value: 128
    g_DlcParameters.ucDlcBLEPoint          = u16DlcInit[17]; // n = 24 ~ 64, default value: 48
    g_DlcParameters.ucDlcWLEPoint          = u16DlcInit[18]; // n = 24 ~ 64, default value: 48
    g_DlcParameters.bEnableBLE             = u16DlcInit[19]; // 1: enable; 0: disable
    g_DlcParameters.bEnableWLE             = u16DlcInit[20]; // 1: enable; 0: disable
    g_DlcParameters.ucDlcYAvgThresholdM    = u16DlcInit[21];
    g_DlcParameters.ucDlcCurveMode         = u16DlcInit[22];
    g_DlcParameters.ucDlcCurveModeMixAlpha = u16DlcInit[23];
    g_DlcParameters.ucDlcAlgorithmMode     = u16DlcInit[24];
    g_DlcParameters.ucDlcSepPointH         = u16DlcInit[25];
    g_DlcParameters.ucDlcSepPointL         = u16DlcInit[26];
    g_DlcParameters.uwDlcBleStartPointTH   = u16DlcInit[27];
    g_DlcParameters.uwDlcBleEndPointTH     = u16DlcInit[28];
    g_DlcParameters.ucDlcCurveDiff_L_TH    = u16DlcInit[29];
    g_DlcParameters.ucDlcCurveDiff_H_TH    = u16DlcInit[30];
    g_DlcParameters.uwDlcBLESlopPoint_1    = u16DlcInit[31];
    g_DlcParameters.uwDlcBLESlopPoint_2    = u16DlcInit[32];
    g_DlcParameters.uwDlcBLESlopPoint_3    = u16DlcInit[33];
    g_DlcParameters.uwDlcBLESlopPoint_4    = u16DlcInit[34];
    g_DlcParameters.uwDlcBLESlopPoint_5    = u16DlcInit[35];
    g_DlcParameters.uwDlcDark_BLE_Slop_Min = u16DlcInit[36];
    g_DlcParameters.ucDlcCurveDiffCoringTH = u16DlcInit[37];
    g_DlcParameters.ucDlcAlphaBlendingMin  = u16DlcInit[38];
    g_DlcParameters.ucDlcAlphaBlendingMax  = u16DlcInit[39];
    g_DlcParameters.ucDlcFlicker_alpha     = u16DlcInit[40];
    g_DlcParameters.ucDlcYAVG_L_TH         = u16DlcInit[41];
    g_DlcParameters.ucDlcYAVG_H_TH         = u16DlcInit[42];
    g_DlcParameters.ucDlcDiffBase_L        = u16DlcInit[43];
    g_DlcParameters.ucDlcDiffBase_M        = u16DlcInit[44];
    g_DlcParameters.ucDlcDiffBase_H        = u16DlcInit[45];

    g_ucDLCInitBuffer[1]=(BYTE)(g_DlcParameters.ucDlcSepPointH &0xFF);
    g_ucDLCInitBuffer[2]=(BYTE)(g_DlcParameters.ucDlcSepPointL &0xFF);
    g_ucDLCInitBuffer[3]=(BYTE)(g_DlcParameters.uwDlcBleStartPointTH &0xFF);
    g_ucDLCInitBuffer[4]=(BYTE)((g_DlcParameters.uwDlcBleStartPointTH>>8) &0xFF);
    g_ucDLCInitBuffer[5]=(BYTE)(g_DlcParameters.uwDlcBleEndPointTH &0xFF);
    g_ucDLCInitBuffer[6]=(BYTE)((g_DlcParameters.uwDlcBleEndPointTH>>8) &0xFF);
    g_ucDLCInitBuffer[7]=(BYTE)(g_DlcParameters.uwDlcBLESlopPoint_1 &0xFF);
    g_ucDLCInitBuffer[8]=(BYTE)((g_DlcParameters.uwDlcBLESlopPoint_1>>8) &0xFF);
    g_ucDLCInitBuffer[9]=(BYTE)(g_DlcParameters.uwDlcBLESlopPoint_2 &0xFF);
    g_ucDLCInitBuffer[10]=(BYTE)((g_DlcParameters.uwDlcBLESlopPoint_2>>8) &0xFF);
    g_ucDLCInitBuffer[11]=(BYTE)(g_DlcParameters.uwDlcBLESlopPoint_3 &0xFF);
    g_ucDLCInitBuffer[12]=(BYTE)((g_DlcParameters.uwDlcBLESlopPoint_3>>8) &0xFF);
    g_ucDLCInitBuffer[13]=(BYTE)(g_DlcParameters.uwDlcBLESlopPoint_4 &0xFF);
    g_ucDLCInitBuffer[14]=(BYTE)((g_DlcParameters.uwDlcBLESlopPoint_4>>8) &0xFF);
    g_ucDLCInitBuffer[15]=(BYTE)(g_DlcParameters.uwDlcBLESlopPoint_5 &0xFF);
    g_ucDLCInitBuffer[16]=(BYTE)((g_DlcParameters.uwDlcBLESlopPoint_5>>8) &0xFF);
    g_ucDLCInitBuffer[17]=(BYTE)(g_DlcParameters.uwDlcDark_BLE_Slop_Min &0xFF);
    g_ucDLCInitBuffer[18]=(BYTE)((g_DlcParameters.uwDlcDark_BLE_Slop_Min>>8) &0xFF);
    g_ucDLCInitBuffer[19]=g_DlcParameters.ucDlcAlphaBlendingMin;
    g_ucDLCInitBuffer[20]=g_DlcParameters.ucDlcAlphaBlendingMax;
    g_ucDLCInitBuffer[21]=g_DlcParameters.ucDlcFlicker_alpha;
    g_ucDLCInitBuffer[22]=g_DlcParameters.ucDlcYAVG_L_TH;
    g_ucDLCInitBuffer[23]=g_DlcParameters.ucDlcYAVG_H_TH;
    g_ucDLCInitBuffer[24]=g_DlcParameters.ucDlcDiffBase_L;
    g_ucDLCInitBuffer[25]=g_DlcParameters.ucDlcDiffBase_M;
    g_ucDLCInitBuffer[26]=g_DlcParameters.ucDlcDiffBase_H;
    g_ucDLCInitBuffer[27]=g_DlcParameters.ucDlcCurveDiff_L_TH;
    g_ucDLCInitBuffer[28]=g_DlcParameters.ucDlcCurveDiff_H_TH;
    g_ucDLCInitBuffer[29]=g_DlcParameters.ucDlcYAvgThresholdL;
    g_ucDLCInitBuffer[30]=g_DlcParameters.ucDlcYAvgThresholdM;
    g_ucDLCInitBuffer[31]=g_DlcParameters.ucDlcYAvgThresholdH;
    g_ucDLCInitBuffer[32]=g_DlcParameters.ucDlcCurveMode;
    g_ucDLCInitBuffer[33]=g_DlcParameters.ucDlcCurveModeMixAlpha;

    for (ucTmp = 0  ; ucTmp < 32; ucTmp++)
    {
        msWriteByte((REG_SC_BK30_02_L+ucTmp), 0 );
    }

    //Mantis issue of 0232938:Hsense A3 DLC the divisor can't to be equal to zero .
    if((g_DlcParameters.ucDlcYAvgThresholdH-g_DlcParameters.ucDlcYAvgThresholdL)==0)
    {
        DLC_DEBUG("\n Error in  [Kernel DLC][ %s  , %d ]  (ucDlcYAvgThresholdH - ucDlcYAvgThresholdL)=0 !!!\n", __FUNCTION__,__LINE__);
        return FALSE;
    }

    if(( g_DlcParameters.ucDlcYAvgThresholdM - g_DlcParameters.ucDlcYAvgThresholdL)==0)
    {
        DLC_DEBUG("\n Error in  [Kernel DLC][ %s  , %d ]  (ucDlcYAvgThresholdM - ucDlcYAvgThresholdL)=0 !!!\n", __FUNCTION__,__LINE__);
        return FALSE;
    }

    if((g_DlcParameters.ucDlcYAvgThresholdH - g_DlcParameters.ucDlcYAvgThresholdM)==0)
    {
        DLC_DEBUG("\n Error in  [Kernel DLC][ %s  , %d ]  (ucDlcYAvgThresholdH - ucDlcYAvgThresholdM)=0 !!!\n", __FUNCTION__,__LINE__);
        return FALSE;
    }

    if((g_DlcParameters.ucDlcCurveDiff_H_TH - g_DlcParameters.ucDlcCurveDiff_L_TH)==0)
    {
        DLC_DEBUG("\n Error in  [Kernel DLC][ %s  , %d ]  (ucDlcCurveDiff_H_TH - ucDlcCurveDiff_L_TH)=0 !!!\n", __FUNCTION__,__LINE__);
        return FALSE;
    }

    if(g_DlcParameters.ucDlcBLEPoint==0)
    {
        DLC_DEBUG("\n Error in  [Kernel DLC][ %s  , %d ]  ucDlcBLEPoint=0 !!!\n", __FUNCTION__,__LINE__);
        return FALSE;
    }

    if(g_DlcParameters.ucDlcWLEPoint==0)
    {
        DLC_DEBUG("\n Error in  [Kernel DLC][ %s  , %d ]  ucDlcWLEPoint=0 !!!\n", __FUNCTION__,__LINE__);
        return FALSE;
    }

    if(( g_DlcParameters.ucDlcAlphaBlendingMax- g_DlcParameters.ucDlcAlphaBlendingMin)==0)
    {
        DLC_DEBUG("\n Error in  [Kernel DLC][ %s  , %d ]  (ucDlcAlphaBlendingMax - ucDlcAlphaBlendingMin)=0 !!!\n", __FUNCTION__,__LINE__);
        return FALSE;
    }

    return TRUE ;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void msDlcSetBleOnOff(MS_BOOL bSwitch)
{
    g_bSetDlcBleOn= bSwitch ;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
MS_BOOL msDlcGetHistogram32Info(MS_U16 *pu16Histogram)
{
    MS_U8 ucTmp;

    for(ucTmp=0; ucTmp<32; ucTmp++)
    {
        pu16Histogram[ucTmp] = g_wLumaHistogram32H[ucTmp];
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
MS_U8 msGetAverageLuminous(void) // = external msGetAverageLuminousValue()
{
    WORD uwTmpAvr;

    // Normalize
    if (g_wLumiTotalCount!=0)
        uwTmpAvr = (256 * (DWORD)g_wLumiAverageTemp + (g_wLumiTotalCount/2)) / g_wLumiTotalCount;
    else
        uwTmpAvr = 0;

    if (uwTmpAvr >= 255)
        uwTmpAvr = 255;
    return (BYTE)uwTmpAvr;
}

MS_U8 msGetAverageLuminousSub(void)
{
    WORD uwTmpAvr=0, uwLumiTotalCount=0, uwLumiAverageTemp=0 ;

    uwLumiTotalCount = msReadByte(REG_ADDR_HISTOGRAM_SUB_TOTAL_COUNT_H);
    uwLumiTotalCount = (uwLumiTotalCount << 8) + (WORD)msReadByte(REG_ADDR_HISTOGRAM_SUB_TOTAL_COUNT_L);

    uwLumiAverageTemp = msReadByte(REG_ADDR_HISTOGRAM_SUB_TOTAL_SUM_H);
    uwLumiAverageTemp = (uwLumiAverageTemp << 8) + (WORD)msReadByte(REG_ADDR_HISTOGRAM_SUB_TOTAL_SUM_L);

    // Normalize
    if (uwLumiTotalCount!=0)
        uwTmpAvr = (256 * (DWORD)uwLumiAverageTemp + (uwLumiTotalCount/2)) / uwLumiTotalCount;
    else
        uwTmpAvr = 0;

    if (uwTmpAvr >= 255)
        uwTmpAvr = 255;

    return (BYTE)uwTmpAvr;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
WORD msGetAverageLuminous_x10(void)
{
    WORD uwTmpAvr;

    // Normalize
    if (g_wLumiTotalCount!=0)
        uwTmpAvr = (256 * 10 * (DWORD)g_wLumiAverageTemp + (g_wLumiTotalCount/2)) / g_wLumiTotalCount;
    else
        uwTmpAvr = 0;

    if (uwTmpAvr >= (256 * 10 - 1))
        uwTmpAvr = (256 * 10 - 1);

    return uwTmpAvr;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
MS_BOOL msGetHistogramHandler(MS_BOOL bWindow)
{
    BYTE ucTmp;
    WORD wTmpHistogramSum; // for protect histogram overflow
    BYTE ucHistogramOverflowIndex;
    BYTE ucReturn;

    //When DLC algorithm is not from kernel, force return false.
    if( g_DlcParameters.ucDlcAlgorithmMode != XC_DLC_ALGORITHM_KERNEL )
    {
        return FALSE;
    }

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
        MaxHistogram32H=0;
        for (ucTmp = 0, wTmpHistogramSum = 0, ucHistogramOverflowIndex = 0xFF; ucTmp < 32; ucTmp++)
        {
            g_wLumaHistogram32H[ucTmp] = msReadByte(REG_ADDR_HISTOGRAM_DATA_32+(2*ucTmp)+1);
            g_wLumaHistogram32H[ucTmp] <<= 8;
            g_wLumaHistogram32H[ucTmp] |= msReadByte(REG_ADDR_HISTOGRAM_DATA_32+(2*ucTmp));

            //DLC Bank 0x1A . Check for MSTV_Tool debug.
            if((MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK30_01_L, 0xFF))==0x1A)
            {
                g_ucDLCInitBuffer[118+ucTmp*2]=msReadByte(REG_ADDR_HISTOGRAM_DATA_32+(2*ucTmp));
                g_ucDLCInitBuffer[119+ucTmp*2]=msReadByte(REG_ADDR_HISTOGRAM_DATA_32+(2*ucTmp)+1);
            }

            // protect histogram overflow
            if (g_wLumaHistogram32H[ucTmp] >= 65535)
            {
                ucHistogramOverflowIndex = ucTmp;
            }
            g_wLumaHistogram32H[ucTmp] >>= 1; // protect histogram overflow
            wTmpHistogramSum += g_wLumaHistogram32H[ucTmp];

            if (g_wLumaHistogram32H[ucTmp]>MaxHistogram32H) //YT ForHDR
            {
                MaxHistogram32H=g_wLumaHistogram32H[ucTmp];
                MaxucTmp=ucTmp;
            }

        }

        g_wLumiTotalCount = ((WORD)msReadByte(REG_ADDR_HISTOGRAM_TOTAL_COUNT_H) << 8) + msReadByte(REG_ADDR_HISTOGRAM_TOTAL_COUNT_L);

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

        //DLC Bank 0x1A . Check for MSTV_Tool debug.
        if((MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK30_01_L, 0xFF))==0x1A)
        {
            g_ucDLCInitBuffer[34]=g_ucHistogramMin;
            g_ucDLCInitBuffer[35]=g_ucHistogramMax;
            g_ucDLCInitBuffer[36]=g_ucTmpAvgN_1;
        }

        g_wTmpAvgN_1_x10 = msGetAverageLuminous_x10();

        g_wLumiAverageTemp = ((WORD)g_ucHistogramTotal_H<<8) + g_ucHistogramTotal_L;
        g_ucTmpAvgN = msGetAverageLuminous();
        g_wTmpAvgN_x10 = msGetAverageLuminous_x10();

        ucReturn = TRUE;
    }
    else
    {
        ucReturn = FALSE;
    }

    // After read histogram, request HW to do histogram
    // Request HW to do histogram
    msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) & 0xF2);

    g_DlcParameters.ucDlcTimeOut = 150; // 150ms

    msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) | _BIT2 );

    msDlc_FunctionExit();

    return ucReturn;
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
void msDlcHandler(MS_BOOL bWindow)
{
    BYTE ucTmp,Tmp,sepPoint =0,sepPoint0 =0,sepPoint1 =0,sepPoint2 =0,ucYAvgNormalize=0,belnging_factor;
    WORD ucYAvg=0;
    WORD uwHistogramMax, uwHistogramMax2nd,uwHistogramSlopRatioL, uwHistogramSlopRatioH; // 20110406 Ranma add
    DWORD uwHistogramAreaSum[32];//,uwPre_Histogram_ShiftR[32],uwPre_Histogram_ShiftL[32]; // 20110406 Ranma add
    WORD uwHistogramForBleSum=0;
    WORD uwHistogram_BLE_AreaSum[32];
    WORD BLE_StartPoint=0,BLE_EndPoint=0,BLE_StartPoint_Correction=0,BLE_EndPoint_Correction=0;
    WORD BLE_Slop_400,BLE_Slop_480,BLE_Slop_500,BLE_Slop_600,BLE_Slop_800,Dark_BLE_Slop_Min;
    static WORD Pre_BLE_Slop=0x4000;
    static WORD Pre_g_ucDlcFlickAlpha = 0,Pre_TotalYSum = 0,Pre_BLE_StartPoint_Correction=0xFFF;
    static BYTE ucPre_YAvg =0x00;
    static MS_BOOL bPreSubWindowEnable=FALSE;
    static BYTE ucSubWindowChange=0;
    static WORD uwSubChangeDelayCount=0;
    MS_BOOL bSubWindowEnable = FALSE;
    WORD uwBLE_Blend=0,uwBLE_StartPoint_Blend=0,CurveDiff=0,CurveDiff_Coring=0,CurveDiff_base=0,YAvg_base=0;
    BYTE g_ucDlcFlickAlpha_Max=0,g_ucDlcFlickAlpha_Min=0,YAvg_TH_H,YAvg_TH_L,g_ucDlcFlickAlpha_temp,ucDlcFlickAlpha_Diff=0;
    DWORD dwSumBelowAvg=0, dwSumAboveAvg=0,dwHistogramDlcSlop[17],BLE_Slop=0x400,BLE_Slop_tmp=0x400;
    WORD  Delta_YAvg=0,coeff0,coeff1,coeff2;
    DWORD His_ratio_BelowAvg,His_ratio_AboveAvg;
    DWORD g_uwCurveHistogram0[16],g_uwCurveHistogram1[16],g_uwCurveHistogram2[16];
    DWORD BLE_sum=0;
    WORD Pre_YAvg_base,Diff_L,Diff_M,Diff_H,Pre_CurveDiff_base;

    msDlc_FunctionEnter();

    msDlcLumiDiffCtrl();
#if 0 //For customer require. Run DLC algorithm when static field .
    if((g_wLumiAverageTemp-Pre_TotalYSum)==0)
    {
        return;
    }
    else
#endif
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
            DLC_DEBUG("\n Error in [Kernel DLC][ %s  , %d ]  uwHistogramAreaSum[31]=0 !!!\n", __FUNCTION__,__LINE__);
            return;
        }


        // ~~~~~~~~~~~~~~~~~~~~~~~~Pure Image Detect  ~~~~~~~~~~~~~~~~~~~~~~~~
        if( ((g_ucHistogramMax-g_ucHistogramMin) <= 10) || (g_ucHistogramMin >= g_ucHistogramMax) )
        {
            g_wPIAlpha = 256;
        }
        else if ((g_ucHistogramMax-g_ucHistogramMin) >= 26)
        {
            g_wPIAlpha = 0;
        }
        else
        {
            g_wPIAlpha = ((26-(g_ucHistogramMax-g_ucHistogramMin))*256+8)/16;
        }

        if(g_wPIAlpha <= 0)
        {
            g_wPIAlpha = 0;
        }
        else if (g_wPIAlpha >= 256)
        {
            g_wPIAlpha = 256;
        }
        else
        {
            g_wPIAlpha = g_wPIAlpha;
        }
        // ~~~~~~~~~~~~~~~~~~~~~~~~Pure Image Detect End   ~~~~~~~~~~~~~~~~~~~~~~~~

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
            uwHistogram_BLE_AreaSum[ucTmp]=((1024*uwHistogramAreaSum[ucTmp])+uwHistogramAreaSum[31]/2)/uwHistogramAreaSum[31];

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
                    if(uwHistogramAreaSum[ucTmp] >= ((ucTmp+1)*32))
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
                    if(uwHistogramAreaSum[ucTmp] >= ((ucTmp+1)*32))
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
                    if(uwHistogramAreaSum[ucTmp] >= ((ucTmp+1)*32))
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

                     if(uwHistogramAreaSum[ucTmp] >= ((ucTmp+1)*32))
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
            coeff0 = (((3*((sepPoint2*16)-ucYAvgNormalize)*((sepPoint2*16)-ucYAvgNormalize))+64*((sepPoint2*16)-ucYAvgNormalize)+320));
            coeff1 = ((1408-6*((sepPoint2*16)-ucYAvgNormalize)*((sepPoint2*16)-ucYAvgNormalize)));
            coeff2 = 2048-coeff0-coeff1;
        }
        else
        {
            coeff0 = (((3*(ucYAvgNormalize-(sepPoint2*16))*(ucYAvgNormalize-(sepPoint2*16)))-64*(ucYAvgNormalize-(sepPoint2*16))+320));
            coeff1 = ((1408-6*(ucYAvgNormalize-(sepPoint2*16))*(ucYAvgNormalize-(sepPoint2*16))));
            coeff2 = 2048-coeff0-coeff1;
        }

        //Calculate for sepPoint0~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        uwHistogramSlopRatioL = 256;
        uwHistogramSlopRatioH = 256;

        for (ucTmp = 0; ucTmp<= (sepPoint0+1); ucTmp++)
        {
            uwHistogramMax2nd = dwHistogramDlcSlop[ucTmp];

            if (uwHistogramMax2nd < uwHistogramSlopRatioL)
            {
                uwHistogramSlopRatioL = uwHistogramMax2nd;
            }
        }

        for (ucTmp = sepPoint0; ucTmp<=16; ucTmp++)
        {
            uwHistogramMax2nd = dwHistogramDlcSlop[ucTmp];

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
                if(uwHistogramAreaSum[ucTmp]>(ucTmp * 64 + 32))
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
                if(uwHistogramAreaSum[ucTmp]>(ucTmp * 64 + 32))
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
            uwHistogramMax2nd = dwHistogramDlcSlop[ucTmp];

            if (uwHistogramMax2nd < uwHistogramSlopRatioL)
            {
                uwHistogramSlopRatioL = uwHistogramMax2nd;
            }
        }

        for (ucTmp = sepPoint1; ucTmp<=16; ucTmp++)
        {
            uwHistogramMax2nd = dwHistogramDlcSlop[ucTmp];

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
                if(uwHistogramAreaSum[ucTmp]>(ucTmp * 64 + 32))
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
                if(uwHistogramAreaSum[ucTmp]>(ucTmp * 64 + 32))
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
            uwHistogramMax2nd = dwHistogramDlcSlop[ucTmp];

            if (uwHistogramMax2nd < uwHistogramSlopRatioL)
            {
                uwHistogramSlopRatioL = uwHistogramMax2nd;
            }
        }

        for (ucTmp = sepPoint2; ucTmp<=16; ucTmp++)
        {
            uwHistogramMax2nd = dwHistogramDlcSlop[ucTmp];

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
                if(uwHistogramAreaSum[ucTmp]>(ucTmp * 64 + 32))
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
                if(uwHistogramAreaSum[ucTmp]>(ucTmp * 64 + 32))
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

        // Calculate curve by Yavg

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
            DLC_DEBUG("\n Error in  [Kernel DLC][ %s  , %d ]  YAvg_base=0 !!!\n", __FUNCTION__,__LINE__);
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
                g_uwTable[ucTmp] = g_uwCurveHistogram[ucTmp];
            }
        }
        else
        {
            CurveDiff = 0;
            CurveDiff_Coring = 0;
            for (ucTmp=0; ucTmp<16; ucTmp++)
            {
                g_uwTable[ucTmp] = (((WORD)(g_ucCurveYAvg[ucTmp]<<2) * belnging_factor) + ((g_uwCurveHistogram[ucTmp]) * (128 - belnging_factor))) / 128;

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

                if (g_uwCurveHistogram[ucTmp] <= g_DlcParameters.ucDlcYAvgThresholdL*4)
                {
                    CurveDiff_base = Diff_L*4;
                }
                else if (g_uwCurveHistogram[ucTmp] <= g_DlcParameters.ucDlcYAvgThresholdM*4)
                {
                    if(Diff_M >= Diff_L)
                    {
                        CurveDiff_base = Diff_L*4 + (((Diff_M-Diff_L)*(g_uwCurveHistogram[ucTmp]-g_DlcParameters.ucDlcYAvgThresholdL*4))
                                        /(g_DlcParameters.ucDlcYAvgThresholdM- g_DlcParameters.ucDlcYAvgThresholdL));
                    }
                    else
                    {
                        CurveDiff_base = Diff_L*4 - (((Diff_L-Diff_M)*(g_uwCurveHistogram[ucTmp]-g_DlcParameters.ucDlcYAvgThresholdL*4))
                                        /(g_DlcParameters.ucDlcYAvgThresholdM- g_DlcParameters.ucDlcYAvgThresholdL));
                    }
                }
                else if (g_uwCurveHistogram[ucTmp] <= g_DlcParameters.ucDlcYAvgThresholdH*4)
                {
                    if(Diff_H >= Diff_M)
                    {
                        CurveDiff_base = Diff_M*4 + (((Diff_H- Diff_M)*(g_uwCurveHistogram[ucTmp]- g_DlcParameters.ucDlcYAvgThresholdM*4))
                                        /(g_DlcParameters.ucDlcYAvgThresholdH- g_DlcParameters.ucDlcYAvgThresholdM));
                    }
                    else
                    {
                        CurveDiff_base = Diff_M*4 -(((Diff_M- Diff_H)*(g_uwCurveHistogram[ucTmp]- g_DlcParameters.ucDlcYAvgThresholdM*4))
                                        /(g_DlcParameters.ucDlcYAvgThresholdH- g_DlcParameters.ucDlcYAvgThresholdM));
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
                    DLC_DEBUG("\n Error in  [Kernel DLC][ %s  , %d ]  CurveDiff_base=0 !!!\n", __FUNCTION__,__LINE__);
                    return;
                }

                if(ucTmp*16+8 <= ucYAvgNormalize)
                {
                    if (ucTmp == 0)
                    {
                        CurveDiff_Coring = (His_ratio_BelowAvg*(1024-uwHistogram_BLE_AreaSum[1])*abs(g_uwCurveHistogram[ucTmp]-g_uwPre_CurveHistogram[ucTmp]))/(15*CurveDiff_base*128);
                    }
                    else
                    {
                        CurveDiff_Coring = (His_ratio_BelowAvg*(1024-(uwHistogram_BLE_AreaSum[ucTmp*2+1]- uwHistogram_BLE_AreaSum[ucTmp*2-1]))*abs(g_uwCurveHistogram[ucTmp]-g_uwPre_CurveHistogram[ucTmp]))/(15*CurveDiff_base*128);
                    }
                }
                else
                {
                    CurveDiff_Coring = (His_ratio_AboveAvg*(1024-(uwHistogram_BLE_AreaSum[ucTmp*2+1]- uwHistogram_BLE_AreaSum[ucTmp*2-1]))*abs(g_uwCurveHistogram[ucTmp]-g_uwPre_CurveHistogram[ucTmp]))/(15*CurveDiff_base*128);
                }

                CurveDiff_Coring = CurveDiff_Coring/6;

                CurveDiff = CurveDiff + CurveDiff_Coring ;
                g_uwPre_CurveHistogram[ucTmp] = g_uwCurveHistogram[ucTmp];
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

        ucPre_YAvg = ucYAvg;

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

    }

    if( g_DlcParameters.bEnableBLE )
    {
        BYTE uc_cut,uc_lowbound,uc_highbound,i;

        // combine BLE with software
        // BLE - get cut point
        uc_cut = g_ucHistogramMin;
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
        uc_wcut = g_ucHistogramMax;

        uc_whighbound = (msReadByte( REG_ADDR_WLE_UPPER_BOND ) & 0x3F) + 0xC0; // bit offset 0x00~0x3F -> 0xC0~0xFF
        uc_wlowbound = (msReadByte( REG_ADDR_WLE_LOWER_BOND) & 0x3F) + 0xC0; // bit offset 0x00~0x3F -> 0xC0~0xFF

        if ( uc_wcut < uc_wlowbound )
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
#if 0 //For customer require. Run DLC algorithm when static field .
    if((g_wLumiAverageTemp-Pre_TotalYSum)!=0)
#endif
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
        if (g_HDRinitParameters.bHDREnable==ENABLE)  //YT ForHDR
        {
            if(g_HDRinitParameters.DLC_HDRCustomerDlcCurve.bFixHdrCurve == TRUE &&
               g_HDRinitParameters.DLC_HDRCustomerDlcCurve.u16DlcCurveSize == DLC_CURVE_SIZE) //use customer fix DLC curve.
            {
                msHDRFixDlcWriteCurve(bWindow);//HDR Fix DLC
            }
            else
            {
                MS_U16 u16YAvg;   //YT ForHDR
                u16YAvg = msGetAverageLuminous();//YT ForHDR

                HDRtmp.slope = 1582;
                HDRtmp.rolloff = 12288;

#if HDR_DEBUG_MODE
                HDRtmp.smin = MApi_GFLIP_XC_R2BYTE(REG_SC_BK1A_38_L);
                HDRtmp.smax = MApi_GFLIP_XC_R2BYTE(REG_SC_BK1A_39_L);
                HDRtmp.tmin = MApi_GFLIP_XC_R2BYTE(REG_SC_BK1A_3A_L);
                HDRtmp.tmax = MApi_GFLIP_XC_R2BYTE(REG_SC_BK1A_3B_L);
                    HDRtmp.smed =MApi_GFLIP_XC_R2BYTE(REG_SC_BK1A_3C_L);
                    HDRtmp.tmed =MApi_GFLIP_XC_R2BYTE(REG_SC_BK1A_3D_L);
#endif

                //MM and HDMI 2084 format
                if ((g_HDRinitParameters.DLC_HDRMetadataMpegVUI.u8TransferCharacteristics == 16) ||
                    (g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u8EOTF == 2))
                {
                    // 2084 format
                    MDrv_HDR_ToneMappingCurveGen(HDR_Curve,&HDRtmp,bWindow);
                }
                else
                {
                    // Others use original DLC.
                    // ~~~~~~~~~~~~~~~~~~~~~~~~ Sub window open PATCH Start  ~~~~~~~~~~~~~~~~~~~~~~~~
                    bSubWindowEnable = ((msReadByte(REG_ADDR_OP_SW_SUB_ENABLE) & BIT(1)) ==  BIT(1));

                    if(bPreSubWindowEnable != bSubWindowEnable)
                    {
                        ucSubWindowChange = 1;
                        bPreSubWindowEnable = bSubWindowEnable;
                        uwSubChangeDelayCount=0;
                    }

                    if(ucSubWindowChange > 0)
                    {
                        uwSubChangeDelayCount++;
                        if(((g_ucDlcFlickAlpha > 48) &&(uwSubChangeDelayCount>=100))
                           ||((g_ucDlcFlickAlpha > 24) &&(uwSubChangeDelayCount>=200)))
                        {
                            ucSubWindowChange = ucSubWindowChange-1;
                        }

                        if(ucSubWindowChange == 0)
                        {
                            uwSubChangeDelayCount=0;
                        }
                        else
                        {
                            g_ucDlcFlickAlpha = 0;
                        }
                    }
                    // ~~~~~~~~~~~~~~~~~~~~~~~~ Sub window open PATCH End  ~~~~~~~~~~~~~~~~~~~~~~~~

                    // ~~~~~~~~~~~~~~~~~~~~~~~~ PI_Alpha IIR Start  ~~~~~~~~~~~~~~~~~~~~~~~~
                    if ( g_wPIAlpha >= g_wPrePIAlpha)
                    {
                        g_wPIAlpha = ((g_ucDlcFlickAlpha * g_wPIAlpha) +((128-g_ucDlcFlickAlpha)*g_wPrePIAlpha)+127)/128;
                    }
                    else
                    {
                        g_wPIAlpha = ((g_ucDlcFlickAlpha * g_wPIAlpha) +((128-g_ucDlcFlickAlpha)*g_wPrePIAlpha))/128;
                    }

                    g_wPrePIAlpha = g_wPIAlpha;
                    // ~~~~~~~~~~~~~~~~~~~~~~~~ PI_Alpha IIR End  ~~~~~~~~~~~~~~~~~~~~~~~~

                    // Write data to luma curve ...
                    msDlcWriteCurve(bWindow);
                }
            }
        }
        else
        {
           // ~~~~~~~~~~~~~~~~~~~~~~~~ Sub window open PATCH Start  ~~~~~~~~~~~~~~~~~~~~~~~~
           bSubWindowEnable = ((msReadByte(REG_ADDR_OP_SW_SUB_ENABLE) & BIT(1)) ==  BIT(1));

           if(bPreSubWindowEnable != bSubWindowEnable)
           {
               ucSubWindowChange = 1;
               bPreSubWindowEnable = bSubWindowEnable;
               uwSubChangeDelayCount=0;
           }

           if(ucSubWindowChange > 0)
           {
               uwSubChangeDelayCount++;
               if(((g_ucDlcFlickAlpha > 48) &&(uwSubChangeDelayCount>=100))
                   ||((g_ucDlcFlickAlpha > 24) &&(uwSubChangeDelayCount>=200)))
               {
                   ucSubWindowChange = ucSubWindowChange-1;
               }

               if(ucSubWindowChange == 0)
               {
                   uwSubChangeDelayCount=0;
               }
               else
               {
                   g_ucDlcFlickAlpha = 0;
               }
           }
           // ~~~~~~~~~~~~~~~~~~~~~~~~ Sub window open PATCH End  ~~~~~~~~~~~~~~~~~~~~~~~~

           // ~~~~~~~~~~~~~~~~~~~~~~~~ PI_Alpha IIR Start  ~~~~~~~~~~~~~~~~~~~~~~~~
           if ( g_wPIAlpha >= g_wPrePIAlpha)
           {
               g_wPIAlpha = ((g_ucDlcFlickAlpha * g_wPIAlpha) +((128-g_ucDlcFlickAlpha)*g_wPrePIAlpha)+127)/128;
           }
           else
           {
               g_wPIAlpha = ((g_ucDlcFlickAlpha * g_wPIAlpha) +((128-g_ucDlcFlickAlpha)*g_wPrePIAlpha))/128;
           }

           g_wPrePIAlpha = g_wPIAlpha;
           // ~~~~~~~~~~~~~~~~~~~~~~~~ PI_Alpha IIR End  ~~~~~~~~~~~~~~~~~~~~~~~~

           // Write data to luma curve ...
           msDlcWriteCurve(bWindow);
        }

        if(g_bSetDLCCurveBoth)
        {
          //  msDlcWriteCurve(SUB_WINDOW);
        }

        //Dyanmic BLE Function
        //~~~~~~~~~~~~~~~~~Calculate BLE_StartPoint~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if(((ucYAvg*4) + g_DlcParameters.uwDlcBleStartPointTH)>512)
        {
            BLE_StartPoint = ((ucYAvg*4) + g_DlcParameters.uwDlcBleStartPointTH - 512);
        }
        else
        {
            BLE_StartPoint = 0;
        }

        BLE_StartPoint_Correction = (BLE_StartPoint+2)/4;

        if(BLE_StartPoint_Correction >(Pre_BLE_StartPoint_Correction/16))
        {
            if(g_ucDlcFlickAlpha==g_DlcParameters.ucDlcAlphaBlendingMin)
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
            if(g_ucDlcFlickAlpha==g_DlcParameters.ucDlcAlphaBlendingMin)
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
            //Mantis issue of 0264631 :[Konka J3] . Use autotest tool control dc on/off.  DLC divisor can't to be equal to zero .
            if(g_wLumaHistogram32H[0]==0)
            {
                DLC_DEBUG("\n Error in  [Kernel DLC][ %s  , %d ]  g_wLumaHistogram32H[0]=0 !!!\n", __FUNCTION__,__LINE__);
                return;
            }

            BLE_EndPoint = ((g_uwTable[0]*uwHistogramAreaSum[31])/(16*g_wLumaHistogram32H[0]));
        }
        else
        {
            uwHistogramForBleSum = (uwHistogramAreaSum[31]/8) - uwHistogramAreaSum[ucTmp-1];

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
                DLC_DEBUG("\n Error in  [Kernel DLC][ %s  , %d ]  g_wLumaHistogram32H[%d]=0 !!!\n", __FUNCTION__,__LINE__,ucTmp);
                return;
            }

            BLE_sum = BLE_sum + uwHistogramForBleSum *((uwHistogramAreaSum[ucTmp]+uwHistogramAreaSum[ucTmp-1]+1)/2);

            BLE_EndPoint = (BLE_sum*8+(uwHistogramAreaSum[31]/2))/uwHistogramAreaSum[31];
        }

        BLE_EndPoint = (BLE_EndPoint*g_DlcParameters.uwDlcBleEndPointTH+512)/1024;
        BLE_EndPoint_Correction = BLE_EndPoint;


        //~~~~~~~~~output = BLE_EndPoint Correction ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        //~~~~~~~~~Used uwBLE_StartPoint_Blend & BLE_EndPoint_Correction Calculate BLE_Slop ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        if(uwBLE_StartPoint_Blend > 0x7F)
        {
            uwBLE_StartPoint_Blend = 0x7F;
        }

        if( uwBLE_StartPoint_Blend*4 > BLE_EndPoint_Correction)
        {
            BLE_Slop = (1024*uwBLE_StartPoint_Blend*4+((uwBLE_StartPoint_Blend*4 - BLE_EndPoint_Correction)/2))
                    /(uwBLE_StartPoint_Blend*4 - BLE_EndPoint_Correction);
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


        // ~~~~~~~~~~~~~~~~~~~~~~~~ Pure Image Patch  ~~~~~~~~~~~~~~~~~~~~~~~~
        BLE_Slop = (BLE_Slop*(256-g_wPIAlpha)+ 1024*g_wPIAlpha + 128)/256;

        //~~~~~~~~~ output = uwBLE_Blend ¡]with slow approach¡^ ~~~~~~~~~~~~~~~~~~~~~

        if(BLE_Slop >(Pre_BLE_Slop/16))
        {
            if(g_ucDlcFlickAlpha==g_DlcParameters.ucDlcAlphaBlendingMin)
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
            if(g_ucDlcFlickAlpha==g_DlcParameters.ucDlcAlphaBlendingMin)
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
            msWriteByte(L_BK_DLC(0x0D),(msReadByte(L_BK_DLC(0x0D))&0xF8)|(uwBLE_Blend & 0x07));
        }
        else
        {
            msWriteByte(L_BK_DLC(0x10),0x00);
            msWriteByte(H_BK_DLC(0x10),0x80);
            msWriteByte(L_BK_DLC(0x0D),msReadByte(L_BK_DLC(0x0D))&0xF8);
        }

    }
    Pre_TotalYSum = g_wLumiAverageTemp;
    msDlc_FunctionExit();
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void msDlcWriteCurveLSB(MS_BOOL bWindow, BYTE ucIndex, BYTE ucValue)
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

void msDlcWriteCurve(MS_BOOL bWindow)
{
    BYTE ucTmp;
    WORD uwBlend;

#if XC_DLC_SET_DLC_CURVE_BOTH_SAME
    WORD uwTmp;
#endif

    msDlc_FunctionEnter();

    // Write data to luma curve ...
    if( MAIN_WINDOW == bWindow )
    {
        for(ucTmp=0; ucTmp<=0x0f; ucTmp++)
        {
            g_uwTable[ucTmp] = (g_wPIAlpha*(ucTmp*64 + 32)+(256-g_wPIAlpha)*g_uwTable[ucTmp]+128)/256;

            if(g_uwTable[ucTmp]>(g_uwPreTable[ucTmp]/16))
            {
                if(g_ucDlcFlickAlpha==g_DlcParameters.ucDlcAlphaBlendingMin)
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
                if(g_ucDlcFlickAlpha==g_DlcParameters.ucDlcAlphaBlendingMin)
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

            g_uwTable[ucTmp] = uwBlend;
            g_ucTable[ucTmp] = g_uwTable[ucTmp]>>2;

            msWriteByte((REG_ADDR_DLC_DATA_START_MAIN+ucTmp), g_ucTable[ucTmp]);

            //DLC Bank 0x1A . Check for MSTV_Tool debug.
            if((MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK30_01_L, 0xFF))==0x1A)
            {
                g_ucDLCInitBuffer[54+ucTmp]=g_ucTable[ucTmp];
            }

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
            DLC_DEBUG("\n Error in  [Kernel DLC][ %s  , %d ]  (DLC_DEFLICK_BLEND_FACTOR - g_ucDlcFlickAlpha + 1)=0 !!!\n", __FUNCTION__,__LINE__);
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

                g_uwTable[ucTmp] = g_uwPreTable[ucTmp];
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

                g_uwTable[ucTmp] = g_uwPreTable[ucTmp];
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

    g_ucUpdateCnt++;

    msDlc_FunctionExit();
}

 void msHDRFixDlcWriteCurve(MS_BOOL bWindow)//HDR Fix DLC
 {
    // Write data to fix luma curve ...
    int ucTmp=0;
    if( MAIN_WINDOW == bWindow )
    {
        for(ucTmp=0; ucTmp<=0x0f; ucTmp++)
        {
        g_ucTable[ucTmp] = g_HDRinitParameters.DLC_HDRCustomerDlcCurve.pucDlcCurve[ucTmp];
        msWriteByte((REG_ADDR_DLC_DATA_START_MAIN+ucTmp), g_ucTable[ucTmp]);
        msDlcWriteCurveLSB(bWindow, ucTmp, g_uwTable[ucTmp]&0x03);
        // set DLC curve index N0 & 16
        if (ucTmp == 0)
            {
                msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_MAIN, g_HDRinitParameters.DLC_HDRCustomerDlcCurve.pucDlcCurve[16]);
                msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_MAIN + 1, 0x01);

            }
            if (ucTmp == 15)
            {
                msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_MAIN, g_HDRinitParameters.DLC_HDRCustomerDlcCurve.pucDlcCurve[17]);
                msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_MAIN + 1, 0x00);
            }
        }
   }
   else
   {
        for(ucTmp=0; ucTmp<=0x0f; ucTmp++)
        {
        g_ucTable[ucTmp] =g_HDRinitParameters.DLC_HDRCustomerDlcCurve.pucDlcCurve[ucTmp];
        msWriteByte((REG_ADDR_DLC_DATA_START_SUB+ucTmp), g_ucTable[ucTmp]);
        msDlcWriteCurveLSB(bWindow, ucTmp, g_uwTable[ucTmp]&0x03);
        // set DLC curve index N0 & 16
        if (ucTmp == 0)
            {
                msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB, g_HDRinitParameters.DLC_HDRCustomerDlcCurve.pucDlcCurve[16]);
                msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_SUB + 1, 0x01);

            }
            if (ucTmp == 15)
            {
                msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB, g_HDRinitParameters.DLC_HDRCustomerDlcCurve.pucDlcCurve[17]);
                msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_SUB + 1, 0x00);
            }
        }

   }
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
MS_BOOL msIsBlackVideoEnable( MS_BOOL bWindow )
{
    MS_BOOL bReturn;

    if( MAIN_WINDOW == bWindow)
    {
        if(msReadByte(REG_ADDR_VOP_SCREEN_CONTROL) & 0x02)
        {
            bReturn = TRUE;
        }
        else
        {
            bReturn = FALSE;
        }
    }
    else
    {
        if(msReadByte(REG_ADDR_VOP_SCREEN_CONTROL) & 0x20)
        {
            bReturn = TRUE;
        }
        else
        {
            bReturn = FALSE;
        }
    }

    return bReturn;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
MS_BOOL msDLCWriteMSTVToolBufferData( MS_U8 u8IDValue ,  MS_U8 u8DataByte  )
{
    MS_BOOL bReturn = TRUE;
    MS_U8 ucTmp=0;

    g_DlcParameters.ucDlcSepPointH =g_ucDLCInitBuffer[1];
    g_DlcParameters.ucDlcSepPointL =g_ucDLCInitBuffer[2];
    g_DlcParameters.uwDlcBleStartPointTH =(((WORD)g_ucDLCInitBuffer[4]<<8)&0xFF00)|g_ucDLCInitBuffer[3];
    g_DlcParameters.uwDlcBleEndPointTH =(((WORD)g_ucDLCInitBuffer[6]<<8)&0xFF00)|g_ucDLCInitBuffer[5];
    g_DlcParameters.uwDlcBLESlopPoint_1 =(((WORD)g_ucDLCInitBuffer[8]<<8)&0xFF00)|g_ucDLCInitBuffer[7];
    g_DlcParameters.uwDlcBLESlopPoint_2 =(((WORD)g_ucDLCInitBuffer[10]<<8)&0xFF00)|g_ucDLCInitBuffer[9];
    g_DlcParameters.uwDlcBLESlopPoint_3 =(((WORD)g_ucDLCInitBuffer[12]<<8)&0xFF00)|g_ucDLCInitBuffer[11];
    g_DlcParameters.uwDlcBLESlopPoint_4 =(((WORD)g_ucDLCInitBuffer[14]<<8)&0xFF00)|g_ucDLCInitBuffer[13];
    g_DlcParameters.uwDlcBLESlopPoint_5 =(((WORD)g_ucDLCInitBuffer[16]<<8)&0xFF00)|g_ucDLCInitBuffer[15];
    g_DlcParameters.uwDlcDark_BLE_Slop_Min =(((WORD)g_ucDLCInitBuffer[18]<<8)&0xFF00)|g_ucDLCInitBuffer[17];

    if((g_ucDLCInitBuffer[19]<=128)&&(g_ucDLCInitBuffer[19]>=1)) // 1~128
    {
        g_DlcParameters.ucDlcAlphaBlendingMin=g_ucDLCInitBuffer[19];
    }
    else
    {
        DLC_DEBUG("{DLC Tool ERROR]  DlcAlphaBlendingMin= %d   value is error!!!     (1~128) \n",g_ucDLCInitBuffer[19]);
        g_ucDLCInitBuffer[19]=g_DlcParameters.ucDlcAlphaBlendingMin;
        bReturn = FALSE;
    }

    if((g_ucDLCInitBuffer[20]<=128)&&(g_ucDLCInitBuffer[20]>=1)) // 1~128
    {
        g_DlcParameters.ucDlcAlphaBlendingMax=g_ucDLCInitBuffer[20];
    }
    else
    {
        DLC_DEBUG("{DLC Tool ERROR]  DlcAlphaBlendingMax= %d  value is error!!!    (1~128) \n",g_ucDLCInitBuffer[20]);
        g_ucDLCInitBuffer[20]=g_DlcParameters.ucDlcAlphaBlendingMax;
        bReturn = FALSE;
    }

    g_DlcParameters.ucDlcFlicker_alpha=g_ucDLCInitBuffer[21];

    if(g_ucDLCInitBuffer[22]>=48) //  48~255
    {
        g_DlcParameters.ucDlcYAVG_L_TH=g_ucDLCInitBuffer[22];
    }
    else
    {
        DLC_DEBUG("{DLC Tool ERROR]  DlcYAVG_L_TH= %d  value is error!!!    (48~255) \n",g_ucDLCInitBuffer[22]);
        g_ucDLCInitBuffer[22]=g_DlcParameters.ucDlcYAVG_L_TH;
        bReturn = FALSE;
    }

    if(g_ucDLCInitBuffer[23]>=48) //  48~255
    {
        g_DlcParameters.ucDlcYAVG_H_TH=g_ucDLCInitBuffer[23];
    }
    else
    {
        DLC_DEBUG("{DLC Tool ERROR]  DlcYAVG_H_TH= %d  value is error!!!    (48~255) \n",g_ucDLCInitBuffer[23]);
        g_ucDLCInitBuffer[23]=g_DlcParameters.ucDlcYAVG_H_TH;
        bReturn = FALSE;
    }

    g_DlcParameters.ucDlcDiffBase_L=g_ucDLCInitBuffer[24];
    g_DlcParameters.ucDlcDiffBase_M=g_ucDLCInitBuffer[25];
    g_DlcParameters.ucDlcDiffBase_H=g_ucDLCInitBuffer[26];

    if(g_ucDLCInitBuffer[27]>=48) //  48~255
    {
        g_DlcParameters.ucDlcCurveDiff_L_TH=g_ucDLCInitBuffer[27];
    }
    else
    {
        DLC_DEBUG("{DLC Tool ERROR]  DlcCurveDiff_L_TH= %d value is error!!!    (48~255) \n",g_ucDLCInitBuffer[27]);
        g_ucDLCInitBuffer[27]=g_DlcParameters.ucDlcCurveDiff_L_TH;
        bReturn = FALSE;
    }

    if(g_ucDLCInitBuffer[28]>=48) //  48~255
    {
        g_DlcParameters.ucDlcCurveDiff_H_TH=g_ucDLCInitBuffer[28];
    }
    else
    {
        DLC_DEBUG("{DLC Tool ERROR]  DlcCurveDiff_H_TH= %d value is error!!!    (48~255) \n",g_ucDLCInitBuffer[28]);
        g_ucDLCInitBuffer[28]=g_DlcParameters.ucDlcCurveDiff_H_TH;
        bReturn = FALSE;
    }

    g_DlcParameters.ucDlcYAvgThresholdL=g_ucDLCInitBuffer[29];
    g_DlcParameters.ucDlcYAvgThresholdM=g_ucDLCInitBuffer[30];
    g_DlcParameters.ucDlcYAvgThresholdH=g_ucDLCInitBuffer[31];

    if(g_ucDLCInitBuffer[32]<=2) //0~2
    {
        g_DlcParameters.ucDlcCurveMode=g_ucDLCInitBuffer[32];
    }
    else
    {
        DLC_DEBUG("{DLC Tool ERROR]  DlcCurveMode= %d value is error!!!    (0~2) \n",g_ucDLCInitBuffer[32]);
        g_ucDLCInitBuffer[32]=g_DlcParameters.ucDlcCurveMode;
        bReturn = FALSE;
    }

    g_DlcParameters.ucDlcCurveModeMixAlpha=g_ucDLCInitBuffer[33];

    for(ucTmp=0; ucTmp<16; ucTmp++)
    {
        g_DlcParameters.ucLumaCurve[ucTmp]=g_ucDLCInitBuffer[70+ucTmp];
        g_DlcParameters.ucLumaCurve2_a[ucTmp]=g_ucDLCInitBuffer[86+ucTmp];
        g_DlcParameters.ucLumaCurve2_b[ucTmp]=g_ucDLCInitBuffer[102+ucTmp];
    }

    for(ucTmp=0; ucTmp<17; ucTmp++)
    {
        g_DlcParameters.ucDlcHistogramLimitCurve[ucTmp]=g_ucDLCInitBuffer[37+ucTmp];
    }

    return bReturn;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
MS_U32 msDLCRegTrans(MS_U8 u8Value)
{
    MS_U32 u32Ret = 0xFFFF;

    switch(u8Value)
    {
    case 0:
        u32Ret = REG_SC_BK30_06_L;
        break;
    case 1:
        u32Ret = REG_SC_BK30_06_L + 1;
        break;
    case 2:
        u32Ret = REG_SC_BK30_06_L + 3;
        break;
    case 3:
        u32Ret = REG_SC_BK30_06_L + 4;
        break;
    case 4:
        u32Ret = REG_SC_BK30_06_L + 5;
        break;
    case 5:
        u32Ret = REG_SC_BK30_06_L + 7;
        break;
    case 6:
        u32Ret = REG_SC_BK30_06_L + 8;
        break;
    case 7:
        u32Ret = REG_SC_BK30_06_L + 9;
        break;
    case 8:
        u32Ret = REG_SC_BK30_06_L + 10;
        break;
    case 9:
        u32Ret = REG_SC_BK30_06_L + 11;
        break;
    case 10:
        u32Ret = REG_SC_BK30_06_L + 12;
        break;
    case 11:
        u32Ret = REG_SC_BK30_06_L + 14;
        break;
    case 12:
        u32Ret = REG_SC_BK30_06_L + 15;
        break;
    case 13:
        u32Ret = REG_SC_BK30_06_L + 16;
        break;
    case 14:
        u32Ret = REG_SC_BK30_06_L + 17;
        break;
    case 15:
        u32Ret = REG_SC_BK30_06_L + 18;
        break;
    default:
        u32Ret = 0xFFFF;
        //MS_ASSERT(0);
        break;
    }

    return u32Ret;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
MS_BOOL msDLCMSTVToolReadWriteValue( void )
{
    MS_BOOL bReturn=FALSE;
    BYTE u8IDValue=0;
    BYTE u8DataByte=0;
    BYTE ucTmp=0;

    //Check R/C bit
    if ( MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK30_03_L, BIT(0))==BIT(0) )
    {
        u8IDValue=(BYTE)MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK30_02_L, 0xFF);
        u8DataByte=(BYTE)MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK30_04_L, 0xFF);

        if((u8DataByte>16)|(u8IDValue>181)|((u8IDValue+u8DataByte)>182))
        {
            DLC_DEBUG("[DLC tool ERROR] Read   IDValue=%d ,   DataByte=%d   return FALSE     \n",u8IDValue,u8DataByte);
            MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK30_03_L, 0xFE, 0xFF);
            return FALSE;
        }

        //MSTV Tool want to read the data
        if( MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK30_03_L, BIT(8))==BIT(8) )
        {
            for (ucTmp = 0  ; ucTmp < u8DataByte; ucTmp++)
            {
                DLC_DEBUG("[DLC tool Read Data]  g_ucDLCInitBuffer[%d]=0x%x \n",(u8IDValue+ucTmp),g_ucDLCInitBuffer[u8IDValue+ucTmp]);

                msWriteByte( msDLCRegTrans(ucTmp), g_ucDLCInitBuffer[u8IDValue+ucTmp] );
            }
            DLC_DEBUG("[DLC tool Read Data]  Finish     \n");
        }
        else//MSTV Tool want to write the data
        {
            if((u8IDValue>=118)||(( u8IDValue>=34)&&(u8IDValue<=36))||(( u8IDValue>=54)&&(u8IDValue<=69)))
            {
                DLC_DEBUG("[DLC tool ERROR] Read only data    IDValue=%d ,   DataByte=%d   return FALSE     \n",u8IDValue,u8DataByte);
                MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK30_03_L, 0xFE, 0xFF);
                //Read Only Data
                return FALSE;
            }

            for (ucTmp = 0  ; ucTmp < u8DataByte; ucTmp++)
            {
                g_ucDLCInitBuffer[u8IDValue+ucTmp] =msReadByte(msDLCRegTrans(ucTmp)) ;

                DLC_DEBUG("[DLC tool Write Data]   _ucDLCInitBuffer[%d]=%x \n",(u8IDValue+ucTmp),g_ucDLCInitBuffer[u8IDValue+ucTmp]);
            }

            msDLCWriteMSTVToolBufferData(u8IDValue ,u8DataByte);
            DLC_DEBUG("[DLC tool Write Data]  Finish     \n");
        }

        MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK30_03_L, 0, 0xFF);
    }
    bReturn = TRUE;

    return bReturn;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void msDlcSetYGain(MS_U8 u8YGain, MS_BOOL bWindow)
{
    if(MAIN_WINDOW == bWindow)
    {
        msWriteByte(REG_ADDR_DLC_MAIN_Y_GAIN, u8YGain);
    }
    else
    {
        msWriteByte(REG_ADDR_DLC_SUB_Y_GAIN, u8YGain);
    }
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
MS_U8 msDlcGetYGain(MS_BOOL bWindow)
{
    MS_U8 ucYGain=0;

    if(MAIN_WINDOW == bWindow)
    {
        ucYGain = msReadByte(REG_ADDR_DLC_MAIN_Y_GAIN);
    }
    else
    {
        ucYGain = msReadByte(REG_ADDR_DLC_SUB_Y_GAIN);
    }
    return ucYGain;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void msDlcSetCGain(MS_U8 u8CGain, MS_BOOL bWindow)
{
    if(MAIN_WINDOW == bWindow)
    {
        msWriteByte(REG_ADDR_DLC_MAIN_C_GAIN, u8CGain);
    }
    else
    {
        msWriteByte(REG_ADDR_DLC_SUB_C_GAIN, u8CGain);
    }
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
MS_U8 msDlcGetCGain(MS_BOOL bWindow)
{
    MS_U8 ucCGain=0;

    if(MAIN_WINDOW == bWindow)
    {
        ucCGain = msReadByte(REG_ADDR_DLC_MAIN_C_GAIN);
    }
    else
    {
        ucCGain = msReadByte(REG_ADDR_DLC_SUB_C_GAIN);
    }
    return ucCGain;
}


#endif



MS_U32 MDrv_HDR_Pow(MS_U16 data_base, MS_U16 data_exp)
{
    // returns data_base^data_exp
    // data_base : 0.16 (0xFFFF->1.0)
    // data_exp : 4.12  (0x1000->1.0)
    // data_out : 0.16  (0xFFFF->1.0)
    MS_U16 idx, lsb, shift;
    MS_U32 data_tmp, log_res, xlog_res, final_res;

    // input 0.16, output 4.16
    // data  4.16, [(0:63); (32:63)*2; (32:63)*2^2; (32:63)*2^3; (32:63)*2^4; (32:63)*2^5; (32:63)*2^6; (32:63)*2^7; (32:63)*2^8; (32:63)*2^9; (32:64)*2^10]
    static const MS_U32 log_lut[385] = {
      0xC0000,   0xB1721,   0xA65AF,   0x9FDE3,   0x9B43D,   0x97B1D,   0x94C71,   0x924FA,   0x902CB,   0x8E4A4,   0x8C9AB,   0x8B145,   0x89AFF,   0x88681,   0x87388,   0x861DF,
      0x85159,   0x841D4,   0x83332,   0x8255B,   0x81839,   0x80BBC,   0x7FFD3,   0x7F472,   0x7E98D,   0x7DF19,   0x7D50F,   0x7CB66,   0x7C216,   0x7B91A,   0x7B06D,   0x7A808,
      0x79FE7,   0x79806,   0x79062,   0x788F6,   0x781C0,   0x77ABC,   0x773E9,   0x76D42,   0x766C7,   0x76075,   0x75A4A,   0x75443,   0x74E61,   0x748A0,   0x74300,   0x73D7E,
      0x7381A,   0x732D3,   0x72DA7,   0x72895,   0x7239D,   0x71EBC,   0x719F3,   0x71541,   0x710A4,   0x70C1C,   0x707A8,   0x70348,   0x6FEFB,   0x6FABF,   0x6F696,   0x6F27D,
      0x6EE75,   0x6E694,   0x6DEF0,   0x6D784,   0x6D04E,   0x6C94A,   0x6C277,   0x6BBD0,   0x6B555,   0x6AF03,   0x6A8D7,   0x6A2D1,   0x69CEF,   0x6972E,   0x6918E,   0x68C0C,
      0x686A8,   0x68161,   0x67C35,   0x67723,   0x6722B,   0x66D4A,   0x66881,   0x663CF,   0x65F32,   0x65AAA,   0x65636,   0x651D6,   0x64D88,   0x6494D,   0x64524,   0x6410B,
      0x63D03,   0x63522,   0x62D7E,   0x62612,   0x61EDC,   0x617D8,   0x61104,   0x60A5E,   0x603E3,   0x5FD91,   0x5F765,   0x5F15F,   0x5EB7D,   0x5E5BC,   0x5E01B,   0x5DA9A,
      0x5D536,   0x5CFEF,   0x5CAC3,   0x5C5B1,   0x5C0B9,   0x5BBD8,   0x5B70F,   0x5B25D,   0x5ADC0,   0x5A938,   0x5A4C4,   0x5A064,   0x59C16,   0x597DB,   0x593B1,   0x58F99,
      0x58B91,   0x583B0,   0x57C0C,   0x574A0,   0x56D6A,   0x56666,   0x55F92,   0x558EC,   0x55271,   0x54C1F,   0x545F3,   0x53FED,   0x53A0B,   0x5344A,   0x52EA9,   0x52928,
      0x523C4,   0x51E7D,   0x51951,   0x5143F,   0x50F47,   0x50A66,   0x5059D,   0x500EB,   0x4FC4E,   0x4F7C6,   0x4F352,   0x4EEF2,   0x4EAA4,   0x4E669,   0x4E23F,   0x4DE27,
      0x4DA1F,   0x4D23E,   0x4CA9A,   0x4C32E,   0x4BBF8,   0x4B4F4,   0x4AE20,   0x4A77A,   0x4A0FF,   0x49AAC,   0x49481,   0x48E7B,   0x48898,   0x482D8,   0x47D37,   0x477B6,
      0x47252,   0x46D0B,   0x467DF,   0x462CD,   0x45DD4,   0x458F4,   0x4542B,   0x44F79,   0x44ADC,   0x44654,   0x441E0,   0x43D80,   0x43932,   0x434F7,   0x430CD,   0x42CB5,
      0x428AD,   0x420CC,   0x41927,   0x411BC,   0x40A86,   0x40382,   0x3FCAE,   0x3F608,   0x3EF8D,   0x3E93A,   0x3E30F,   0x3DD09,   0x3D726,   0x3D166,   0x3CBC5,   0x3C644,
      0x3C0E0,   0x3BB99,   0x3B66D,   0x3B15B,   0x3AC62,   0x3A782,   0x3A2B9,   0x39E06,   0x3996A,   0x394E2,   0x3906E,   0x38C0E,   0x387C0,   0x38385,   0x37F5B,   0x37B43,
      0x3773A,   0x36F5A,   0x367B5,   0x3604A,   0x35913,   0x35210,   0x34B3C,   0x34496,   0x33E1B,   0x337C8,   0x3319D,   0x32B97,   0x325B4,   0x31FF4,   0x31A53,   0x314D2,
      0x30F6E,   0x30A27,   0x304FB,   0x2FFE9,   0x2FAF0,   0x2F610,   0x2F147,   0x2EC94,   0x2E7F7,   0x2E370,   0x2DEFC,   0x2DA9B,   0x2D64E,   0x2D213,   0x2CDE9,   0x2C9D0,
      0x2C5C8,   0x2BDE8,   0x2B643,   0x2AED8,   0x2A7A1,   0x2A09E,   0x299CA,   0x29324,   0x28CA8,   0x28656,   0x2802B,   0x27A25,   0x27442,   0x26E81,   0x268E1,   0x26360,
      0x25DFC,   0x258B5,   0x25389,   0x24E77,   0x2497E,   0x2449E,   0x23FD5,   0x23B22,   0x23685,   0x231FD,   0x22D8A,   0x22929,   0x224DC,   0x220A1,   0x21C77,   0x2185E,
      0x21456,   0x20C76,   0x204D1,   0x1FD65,   0x1F62F,   0x1EF2C,   0x1E858,   0x1E1B2,   0x1DB36,   0x1D4E4,   0x1CEB9,   0x1C8B3,   0x1C2D0,   0x1BD0F,   0x1B76F,   0x1B1ED,
      0x1AC8A,   0x1A742,   0x1A216,   0x19D05,   0x1980C,   0x1932C,   0x18E63,   0x189B0,   0x18513,   0x1808B,   0x17C18,   0x177B7,   0x1736A,   0x16F2F,   0x16B05,   0x166EC,
      0x162E4,   0x15B04,   0x1535F,   0x14BF3,   0x144BD,   0x13DBA,   0x136E6,   0x1303F,   0x129C4,   0x12372,   0x11D47,   0x11741,   0x1115E,   0x10B9D,   0x105FD,   0x1007B,
       0xFB18,    0xF5D0,    0xF0A4,    0xEB93,    0xE69A,    0xE1BA,    0xDCF1,    0xD83E,    0xD3A1,    0xCF19,    0xCAA5,    0xC645,    0xC1F8,    0xBDBC,    0xB993,    0xB57A,
       0xB172,    0xA991,    0xA1ED,    0x9A81,    0x934B,    0x8C47,    0x8574,    0x7ECD,    0x7852,    0x7200,    0x6BD5,    0x65CF,    0x5FEC,    0x5A2B,    0x548B,    0x4F09,
       0x49A6,    0x445E,    0x3F32,    0x3A20,    0x3528,    0x3048,    0x2B7F,    0x26CC,    0x222F,    0x1DA7,    0x1933,    0x14D3,    0x1086,     0xC4A,     0x821,     0x408,
          0x0
    };
    // input 4.16, output 0.16
    // data  0.16, 2^4*[(0:63); (32:63)*2; (32:63)*2^2; (32:63)*2^3; (32:63)*2^4; (32:63)*2^5; (32:63)*2^6; (32:63)*2^7; (32:63)*2^8; (32:63)*2^9; (32:64)*2^10]
    static const MS_U16 exp_lut[385] = {
       0xFFFF,    0xFFEF,    0xFFDF,    0xFFCF,    0xFFBF,    0xFFAF,    0xFF9F,    0xFF8F,    0xFF7F,    0xFF6F,    0xFF5F,    0xFF4F,    0xFF3F,    0xFF2F,    0xFF1F,    0xFF0F,
       0xFEFF,    0xFEF0,    0xFEE0,    0xFED0,    0xFEC0,    0xFEB0,    0xFEA0,    0xFE90,    0xFE80,    0xFE70,    0xFE60,    0xFE50,    0xFE41,    0xFE31,    0xFE21,    0xFE11,
       0xFE01,    0xFDF1,    0xFDE1,    0xFDD1,    0xFDC2,    0xFDB2,    0xFDA2,    0xFD92,    0xFD82,    0xFD72,    0xFD62,    0xFD53,    0xFD43,    0xFD33,    0xFD23,    0xFD13,
       0xFD03,    0xFCF4,    0xFCE4,    0xFCD4,    0xFCC4,    0xFCB4,    0xFCA5,    0xFC95,    0xFC85,    0xFC75,    0xFC66,    0xFC56,    0xFC46,    0xFC36,    0xFC26,    0xFC17,
       0xFC07,    0xFBE7,    0xFBC8,    0xFBA9,    0xFB89,    0xFB6A,    0xFB4A,    0xFB2B,    0xFB0B,    0xFAEC,    0xFACD,    0xFAAD,    0xFA8E,    0xFA6F,    0xFA4F,    0xFA30,
       0xFA11,    0xF9F2,    0xF9D2,    0xF9B3,    0xF994,    0xF975,    0xF956,    0xF936,    0xF917,    0xF8F8,    0xF8D9,    0xF8BA,    0xF89B,    0xF87C,    0xF85D,    0xF83E,
       0xF81F,    0xF7E1,    0xF7A3,    0xF765,    0xF727,    0xF6E9,    0xF6AC,    0xF66E,    0xF630,    0xF5F3,    0xF5B5,    0xF578,    0xF53B,    0xF4FD,    0xF4C0,    0xF483,
       0xF446,    0xF409,    0xF3CC,    0xF38F,    0xF352,    0xF315,    0xF2D9,    0xF29C,    0xF25F,    0xF223,    0xF1E6,    0xF1AA,    0xF16D,    0xF131,    0xF0F5,    0xF0B9,
       0xF07C,    0xF004,    0xEF8C,    0xEF15,    0xEE9D,    0xEE26,    0xEDAF,    0xED38,    0xECC2,    0xEC4C,    0xEBD6,    0xEB60,    0xEAEA,    0xEA75,    0xEA00,    0xE98B,
       0xE916,    0xE8A2,    0xE82E,    0xE7BA,    0xE746,    0xE6D2,    0xE65F,    0xE5EC,    0xE579,    0xE506,    0xE494,    0xE422,    0xE3B0,    0xE33E,    0xE2CD,    0xE25B,
       0xE1EA,    0xE109,    0xE028,    0xDF49,    0xDE6A,    0xDD8C,    0xDCAF,    0xDBD2,    0xDAF7,    0xDA1C,    0xD943,    0xD86A,    0xD792,    0xD6BB,    0xD5E4,    0xD50F,
       0xD43A,    0xD366,    0xD293,    0xD1C1,    0xD0F0,    0xD01F,    0xCF50,    0xCE81,    0xCDB3,    0xCCE5,    0xCC19,    0xCB4D,    0xCA82,    0xC9B8,    0xC8EF,    0xC826,
       0xC75F,    0xC5D1,    0xC447,    0xC2C0,    0xC13C,    0xBFBB,    0xBE3D,    0xBCC2,    0xBB4A,    0xB9D5,    0xB863,    0xB6F4,    0xB587,    0xB41D,    0xB2B7,    0xB153,
       0xAFF1,    0xAE93,    0xAD37,    0xABDE,    0xAA87,    0xA934,    0xA7E3,    0xA694,    0xA548,    0xA3FF,    0xA2B8,    0xA174,    0xA032,    0x9EF3,    0x9DB7,    0x9C7C,
       0x9B45,    0x98DC,    0x967E,    0x9428,    0x91DC,    0x8F99,    0x8D60,    0x8B2E,    0x8906,    0x86E6,    0x84CF,    0x82C0,    0x80B9,    0x7EBA,    0x7CC3,    0x7AD4,
       0x78EC,    0x770C,    0x7534,    0x7363,    0x7199,    0x6FD6,    0x6E1A,    0x6C65,    0x6AB7,    0x690F,    0x676E,    0x65D4,    0x6440,    0x62B2,    0x612A,    0x5FA8,
       0x5E2D,    0x5B47,    0x5878,    0x55BF,    0x531C,    0x508D,    0x4E13,    0x4BAC,    0x4958,    0x4716,    0x44E6,    0x42C7,    0x40B9,    0x3EBC,    0x3CCE,    0x3AEF,
       0x391F,    0x375D,    0x35A9,    0x3402,    0x3268,    0x30DB,    0x2F5A,    0x2DE5,    0x2C7C,    0x2B1E,    0x29CA,    0x2881,    0x2742,    0x260D,    0x24E1,    0x23BE,
       0x22A5,    0x208C,    0x1E93,    0x1CB9,    0x1AFB,    0x1959,    0x17CF,    0x165E,    0x1503,    0x13BD,    0x128B,    0x116C,    0x105D,     0xF60,     0xE71,     0xD91,
        0xCBF,     0xBF9,     0xB3F,     0xA91,     0x9ED,     0x953,     0x8C2,     0x83B,     0x7BB,     0x743,     0x6D2,     0x669,     0x605,     0x5A8,     0x550,     0x4FE,
        0x4B0,     0x423,     0x3A7,     0x339,     0x2D8,     0x282,     0x237,     0x1F4,     0x1BA,     0x186,     0x158,     0x12F,     0x10C,      0xEC,      0xD1,      0xB8,
         0xA2,      0x8F,      0x7F,      0x70,      0x63,      0x57,      0x4D,      0x44,      0x3C,      0x35,      0x2F,      0x29,      0x24,      0x20,      0x1C,      0x19,
         0x16,      0x11,       0xD,       0xA,       0x8,       0x6,       0x5,       0x4,       0x3,       0x2,       0x2,       0x1,       0x1,       0x1,       0x1,       0x1,
          0x0,       0x0,       0x0,       0x0,       0x0,       0x0,       0x0,       0x0,       0x0,       0x0,       0x0,       0x0,       0x0,       0x0,       0x0,       0x0,
          0x0
    }; // continued data is all zero

    if (data_base == 0)
        return 0;
    if (data_exp == 0)
        return 0xFFFF;

    // log(base) : 0.16 -> 4.16
    data_tmp = data_base;
    for (shift = 0; data_tmp > 63; shift++)
        data_tmp >>= 1;
    idx = (data_base >> shift) + 32*shift;
    lsb = data_base & ((0x1 << shift) - 1);
    if (idx == 383) // to fix!
        lsb = lsb + 1;
    log_res = log_lut[idx] - ((log_lut[idx] - log_lut[idx+1]) * lsb >> shift);

    // x*log(base) : 4.16, x : 4.12
    xlog_res = data_exp * log_res >> 12;

    // exp^(x*log(base)) : 4.16 -> 0.16
    data_tmp = xlog_res;
    for (shift = 0; data_tmp > (63 << 4); shift++)
        data_tmp >>= 1;
    idx = (xlog_res >> (shift+4)) + 32*shift;
    lsb = xlog_res & ((0x1 << (shift+4)) - 1);
    final_res = exp_lut[idx] - ((exp_lut[idx] - exp_lut[idx+1]) * lsb >> (shift+4));

    return final_res;
}

MS_BOOL msHDRInit(MS_U16 *u16HDRInit , MS_U16 u16HDRInitSize)
{
    MS_U16 i = 0, j = 0;
    MS_U16 u16FixDlcSize = 0;

    g_HDRinitParameters.bHDREnable     = u16HDRInit[i++];   // 0
    g_HDRinitParameters.u16HDRFunctionSelect     = u16HDRInit[i++];

    g_HDRinitParameters.DLC_HDRMetadataMpegVUI.u8ColorPrimaries            = u16HDRInit[i++]; // 2
    g_HDRinitParameters.DLC_HDRMetadataMpegVUI.u8TransferCharacteristics   = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRMetadataMpegVUI.u8MatrixCoefficients         = u16HDRInit[i++];

    g_HDRinitParameters.DLC_HDRToneMappingData.u16Smin     = u16HDRInit[i++]; // 5
    g_HDRinitParameters.DLC_HDRToneMappingData.u16Smed     = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRToneMappingData.u16Smax     = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmin     = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmed     = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmax     = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRToneMappingData.u16MidSourceOffset     = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRToneMappingData.u16MidTargetOffset     = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRToneMappingData.u16MidSourceRatio     = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRToneMappingData.u16MidTargetRatio     = u16HDRInit[i++];

    g_HDRinitParameters.DLC_HDRGamutMappingData.u16tRx     = u16HDRInit[i++];    // 15
    g_HDRinitParameters.DLC_HDRGamutMappingData.u16tRy     = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRGamutMappingData.u16tGx     = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRGamutMappingData.u16tGy     = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRGamutMappingData.u16tBx     = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRGamutMappingData.u16tBy     = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRGamutMappingData.u16tWx     = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRGamutMappingData.u16tWy     = u16HDRInit[i++];

    g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u8EOTF = u16HDRInit[i++]; // 23
    g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Rx = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Ry = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Gx = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Gy = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Bx = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16By = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Wx = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Wy = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Lmax = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Lmin = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16MaxCLL = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16MaxFALL = u16HDRInit[i++];

    g_HDRinitParameters.DLC_HDRCustomerDlcCurve.bFixHdrCurve = u16HDRInit[i++];  // 36
    u16FixDlcSize = u16HDRInit[i++];
    if (u16FixDlcSize > 0)
    {
        for ( j = 0 ; j < u16FixDlcSize ; j++ )
        {
            g_HDRinitParameters.DLC_HDRCustomerDlcCurve.pucDlcCurve[j] = u16HDRInit[i++];
        }
    }
    g_HDRinitParameters.DLC_HDRCustomerDlcCurve.u16DlcCurveSize = u16FixDlcSize;

    g_HDRinitParameters.DLC_HDRCustomerColorPrimaries.bCustomerEnable = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRCustomerColorPrimaries.u16sWx = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRCustomerColorPrimaries.u16sWy = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.PixelFormat = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.Colorimetry = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.ExtendedColorimetry = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.RgbQuantizationRange = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.YccQuantizationRange = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.StaticMetadataDescriptorID = u16HDRInit[i++];

    g_HDRinitParameters.DLC_HDRMetadataSeiContentLightLevel.bUsed = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRMetadataSeiContentLightLevel.bContentLightLevelEnabled = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRMetadataSeiContentLightLevel.u16MaxContentLightLevel = u16HDRInit[i++];
    g_HDRinitParameters.DLC_HDRMetadataSeiContentLightLevel.u16MaxPicAverageLightLevel = u16HDRInit[i++];

    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  bHDREnable=%d    \n",g_HDRinitParameters.bHDREnable ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  u16HDRFunctionSelect=%d    \n",g_HDRinitParameters.u16HDRFunctionSelect ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRMetadataMpegVUI.u8ColorPrimaries =%d    \n",g_HDRinitParameters.DLC_HDRMetadataMpegVUI.u8ColorPrimaries ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRMetadataMpegVUI.u8TransferCharacteristics=%d    \n",g_HDRinitParameters.DLC_HDRMetadataMpegVUI.u8TransferCharacteristics ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRMetadataMpegVUI.u8MatrixCoefficients=%d    \n",g_HDRinitParameters.DLC_HDRMetadataMpegVUI.u8MatrixCoefficients ));

    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRToneMappingData.u16Smin=%d    \n",g_HDRinitParameters.DLC_HDRToneMappingData.u16Smin ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRToneMappingData.u16Smed=%d    \n",g_HDRinitParameters.DLC_HDRToneMappingData.u16Smed ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRToneMappingData.u16Smax=%d    \n",g_HDRinitParameters.DLC_HDRToneMappingData.u16Smax ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRToneMappingData.u16Tmin=%d    \n",g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmin ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRToneMappingData.u16Tmed=%d    \n",g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmed ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRToneMappingData.u16Tmax=%d    \n",g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmax ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRToneMappingData.u16MidSourceOffset=%d    \n",g_HDRinitParameters.DLC_HDRToneMappingData.u16MidSourceOffset ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRToneMappingData.u16MidTargetOffset=%d    \n",g_HDRinitParameters.DLC_HDRToneMappingData.u16MidTargetOffset ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRToneMappingData.u16MidSourceRatio=%d    \n",g_HDRinitParameters.DLC_HDRToneMappingData.u16MidSourceRatio ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRToneMappingData.u16MidTargetRatio=%d    \n",g_HDRinitParameters.DLC_HDRToneMappingData.u16MidTargetRatio ));

    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRGamutMappingData.u16tRx=%d    \n",g_HDRinitParameters.DLC_HDRGamutMappingData.u16tRx ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRGamutMappingData.u16tRy=%d    \n",g_HDRinitParameters.DLC_HDRGamutMappingData.u16tRy ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRGamutMappingData.u16tGx=%d    \n",g_HDRinitParameters.DLC_HDRGamutMappingData.u16tGx));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRGamutMappingData.u16tGy=%d    \n",g_HDRinitParameters.DLC_HDRGamutMappingData.u16tGy ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRGamutMappingData.u16tBx=%d    \n",g_HDRinitParameters.DLC_HDRGamutMappingData.u16tBx ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRGamutMappingData.u16tBy=%d    \n",g_HDRinitParameters.DLC_HDRGamutMappingData.u16tBy ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRGamutMappingData.u16tWx=%d    \n",g_HDRinitParameters.DLC_HDRGamutMappingData.u16tWx ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRGamutMappingData.u16tWy=%d    \n",g_HDRinitParameters.DLC_HDRGamutMappingData.u16tWy ));


    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRMetadataHdmiTxInfoFrame.u8EOTF=%d    \n",g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u8EOTF ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRMetadataHdmiTxInfoFrame.u16Rx=%d    \n",g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Rx ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRMetadataHdmiTxInfoFrame.u16Ry=%d    \n",g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Ry ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRMetadataHdmiTxInfoFrame.u16Gx=%d    \n",g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Gx ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRMetadataHdmiTxInfoFrame.u16Gy=%d    \n",g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Gy ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRMetadataHdmiTxInfoFrame.u16Bx=%d    \n",g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Bx ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRMetadataHdmiTxInfoFrame.u16By=%d    \n",g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16By ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRMetadataHdmiTxInfoFrame.u16Wx=%d    \n",g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Wx ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRMetadataHdmiTxInfoFrame.u16Wy=%d    \n",g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Wy ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRMetadataHdmiTxInfoFrame.u16Lmax=%d    \n",g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Lmax) );
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRMetadataHdmiTxInfoFrame.u16Lmin=%d    \n",g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16Lmin ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRMetadataHdmiTxInfoFrame.u16MaxCLL=%d    \n",g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16MaxCLL ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  DLC_HDRMetadataHdmiTxInfoFrame.u16MaxFALL=%d    \n",g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u16MaxFALL  ));

    HDR_DBG_HAL(printk("\n  Kernel  bFixHdrCurve=%d    \n",g_HDRinitParameters.DLC_HDRCustomerDlcCurve.bFixHdrCurve ));
    if (g_HDRinitParameters.DLC_HDRCustomerDlcCurve.bFixHdrCurve == TRUE &&
        g_HDRinitParameters.DLC_HDRCustomerDlcCurve.pucDlcCurve != NULL &&
        g_HDRinitParameters.DLC_HDRCustomerDlcCurve.u16DlcCurveSize > 0)
    {
        HDR_DBG_HAL(printk("\n  Fix Hdr Curve is :  \n"));
        for ( j = 0 ; j < g_HDRinitParameters.DLC_HDRCustomerDlcCurve.u16DlcCurveSize ; j++ )
        {
            HDR_DBG_HAL(DLC_DEBUG("%d   ",g_HDRinitParameters.DLC_HDRCustomerDlcCurve.pucDlcCurve[j]));
        }
        HDR_DBG_HAL(DLC_DEBUG("\n"));
    }

    HDR_DBG_HAL(printk("\n  Kernel  use customer color primaries =%d    \n", g_HDRinitParameters.DLC_HDRCustomerColorPrimaries.bCustomerEnable ));
    if (g_HDRinitParameters.DLC_HDRCustomerColorPrimaries.bCustomerEnable == TRUE)
    {
        HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  customer color primaries Wx =%d    \n",g_HDRinitParameters.DLC_HDRCustomerColorPrimaries.u16sWx));
        HDR_DBG_HAL(DLC_DEBUG("\n  Kernel  customer color primaries Wy =%d    \n",g_HDRinitParameters.DLC_HDRCustomerColorPrimaries.u16sWy));
    }

    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRHdmiTxAviInfoFrame.PixelFormat %x \n",g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.PixelFormat));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRHdmiTxAviInfoFrame.Colorimetry %x \n", g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.Colorimetry ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRHdmiTxAviInfoFrame.ExtendedColorimetry %x \n",g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.ExtendedColorimetry ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRHdmiTxAviInfoFrame.RgbQuantizationRange %x \n",g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.RgbQuantizationRange ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRHdmiTxAviInfoFrame.YccQuantizationRange %x  \n",g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.YccQuantizationRange ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRHdmiTxAviInfoFrame.StaticMetadataDescriptorID %x \n", g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.StaticMetadataDescriptorID ));

#if 1
    HDRtmp.smin = g_HDRinitParameters.DLC_HDRToneMappingData.u16Smin ;
    HDRtmp.smax = g_HDRinitParameters.DLC_HDRToneMappingData.u16Smax;
    HDRtmp.tmax = g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmax;
    HDRtmp.tmin = g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmin;
    HDRtmp.smed = g_HDRinitParameters.DLC_HDRToneMappingData.u16Smed;
    HDRtmp.tmed = g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmed;

    g_u16HdrY2RRatio = 2;
    g_u16Hdr3x3Ratio = 8;

#if HDR_DEBUG_MODE
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK1A_38_L, HDRtmp.smin);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK1A_39_L, HDRtmp.smax);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK1A_3A_L, HDRtmp.tmin);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK1A_3B_L, HDRtmp.tmax);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK1A_3C_L, HDRtmp.smed);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK1A_3D_L, HDRtmp.tmed);
    MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK1A_3E_L, 0,0xFF);
    MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK1A_3E_L, 0,0xFF00);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK1A_3F_L, 0x208);
#endif
#endif
    return TRUE ;
}


MS_U16 MDrv_HDR_Exp(MS_U16 data_base, MS_U16 data_exp)
{
    return MDrv_HDR_Pow(data_base << 6, data_exp);
}

void MDrv_HDR_ToneMappingCurveGen(MS_U16* pCurve18, HDR_ToneMappingData* pData,MS_BOOL bWindow)
{
    MS_U16 smin, smed, smax;
    MS_U16 tmin, tmed, tmax;
    MS_S64 s1, s2, s3;
    MS_S64 t1, t2, t3;
    MS_S64 norm;
    MS_S64 c1, c2, c3;
    MS_S64 term, nume, deno, base;
    MS_U16 dlc_in, dlc_out, i;
    const MS_U16 dlc_pivots[18] = {0, 0x20, 0x60, 0xA0, 0xE0, 0x120, 0x160, 0x1A0, 0x1E0, 0x220, 0x260, 0x2A0, 0x2E0, 0x320, 0x360, 0x3A0, 0x3E0, 0x3FF};

    // anti-dumbness
    smin = pData->smin;
    smed = pData->smed;
    smax = pData->smax;
    tmin = pData->tmin;
    tmed = pData->tmed;
    tmax = pData->tmax;
    if (smax < smin)
    {
        smin = pData->smax;
        smax = pData->smin;
    }
    if (smed*10 < smin*9 + smax)
        smed = (smin*9 + smax) / 10;
    else if (smed*10 > smin + smax*9)
        smed = (smin + smax*9) / 10;
    if (tmax < tmin)
    {
        tmin = pData->tmax;
        tmax = pData->tmin;
    }
    if (tmed*10 < tmin*9 + tmax)
        tmed = (tmin*9 + tmax) / 10;
    else if (tmed*10 > tmin + tmax*9)
        tmed = (tmin + tmax*9) / 10;

    s1 = MDrv_HDR_Exp(smin, pData->slope);
    s2 = MDrv_HDR_Exp(smed, pData->slope);
    s3 = MDrv_HDR_Exp(smax, pData->slope);
    t1 = MDrv_HDR_Exp(tmin, 0x1000000 / pData->rolloff);
    t2 = MDrv_HDR_Exp(tmed, 0x1000000 / pData->rolloff);
    t3 = MDrv_HDR_Exp(tmax, 0x1000000 / pData->rolloff);
    norm = (MS_S64)s3*t3*(s1-s2) + (MS_S64)s2*t2*(s3-s1) + (MS_S64)s1*t1*(s2-s3);
    c1 = (MS_S64)s2*s3*(t2-t3)*t1 + (MS_S64)s1*s3*(t3-t1)*t2 + (MS_S64)s1*s2*(t1-t2)*t3;
    c2 = (MS_S64)t1*(s3*t3 - s2*t2) + (MS_S64)t2*(s1*t1 - s3*t3) + (MS_S64)t3*(s2*t2-s1*t1);
    c3 = (MS_S64)t1*(s3-s2) + (MS_S64)t2*(s1-s3) + (MS_S64)t3*(s2-s1);

    norm >>= 12;

    if (norm != 0)
    {
        c1 = (c1>>16) / norm;
        c2 = c2 / norm;
        c3 = (c3<<16) / norm;
    }
    else
    {
        return;
    }

    c1 = (c1 > 0xFFFF) ? 0xFFFF : (c1 < -0xFFFF) ? -0xFFFF : c1;
    c2 = (c2 > 0xFFFF) ? 0xFFFF : (c2 < -0xFFFF) ? -0xFFFF : c2;
    c3 = (c3 > 0xFFFF) ? 0xFFFF : (c3 < -0xFFFF) ? -0xFFFF : c3;
    // c1, c2, c3 : s4.12
    //printf("%d\n%d\n%d\n%d\n%d\n%d\n%lld\n%lld\n%lld\n", s1, s2, s3, t1, t2, t3, c1, c2, c3);

    for (i = 0; i < 18; i++)
    {
        dlc_in = dlc_pivots[i];

        term = MDrv_HDR_Exp(dlc_in, pData->slope);
        nume = c1 + (c2*term>>16); // s4.12 + s4.12*0.16>>16 = s4.12
        deno = 0x1000 + (c3*term>>16); // s4.12 + s4.12*0.16>>16 = s4.12
        base = (nume<<10) / deno; // (s4.12<<10) / s4.12 = s0.10
        base = (base < 0) ? 0 : (base > 1023) ? 1023 : base;
        dlc_out = MDrv_HDR_Exp((MS_U16)base, pData->rolloff);

        pCurve18[i] = dlc_out >> 6;
    }
    pCurve18[0] = (2*pCurve18[0] > pCurve18[1]) ? 2*pCurve18[0] - pCurve18[1] : (pCurve18[1] - 2*pCurve18[0]) | 0x400;
    pCurve18[17] = 2*pCurve18[17] - pCurve18[16];

_u16Count++;
if(_u16Count>=300)
{
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel [0]=%x ,[1]=%x,[2]=%x,[3]=%x,[4]=%x,[5]=%x,[6]=%x,[7]=%x,[8]=%x   \n",pCurve18[0],pCurve18[1],pCurve18[2],pCurve18[3],pCurve18[4],pCurve18[5],pCurve18[6],pCurve18[7],pCurve18[8]));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel [9]=%x ,[10]=%x,[11]=%x,[12]=%x,[13]=%x,[14]=%x,[15]=%x,[16]=%x,[17]=%x   \n",pCurve18[9],pCurve18[10],pCurve18[11],pCurve18[12],pCurve18[13],pCurve18[14],pCurve18[15],pCurve18[16],pCurve18[17]));
    _u16Count=0;
}


    for (i = 0; i < 18; i++)
    {
        if (i>0 && i<17)
        {
            msWriteByte((REG_ADDR_DLC_DATA_START_MAIN+(i-1)),pCurve18[i]>>2);
            msDlcWriteCurveLSB(0, i, pCurve18[i]&0x03);
        }
        if( MAIN_WINDOW == bWindow )
        {
            // set DLC curve index N0 & 16
            if (i == 0)
            {
                msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_MAIN, pCurve18[0]>>2);
                msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_MAIN + 1, 0x01);
                msDlcWriteCurveLSB(MAIN_WINDOW, 0xFF,pCurve18[0]&0x03);
            }

            if (i== 17)
            {
                msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_MAIN, pCurve18[17]>>2);
                msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_MAIN + 1, 0x00);
                msDlcWriteCurveLSB(MAIN_WINDOW, 16, pCurve18[17]&0x03);
            }
        }
    }
}

MS_S64 MDrv_HDR_MatrixDetGen(MS_U32 a, MS_U32 b, MS_U32 c, MS_U32 d, MS_U32 e, MS_U32 f, MS_U32 g, MS_U32 h, MS_U32 i)
{
    MS_S64 ei_fh = (MS_S64)e*i - (MS_S64)f*h;
    MS_S64 fg_di = (MS_S64)f*g - (MS_S64)d*i;
    MS_S64 dh_eg = (MS_S64)d*h - (MS_S64)e*g;
    MS_S64 det = ei_fh*a + fg_di*b + dh_eg*c;
    return det;
}

void MDrv_HDR_MatrixInvGen(MS_S32 Minv[3][3], MS_S32 M[3][3])
{
    MS_S32 a = M[0][0];
    MS_S32 b = M[0][1];
    MS_S32 c = M[0][2];
    MS_S32 d = M[1][0];
    MS_S32 e = M[1][1];
    MS_S32 f = M[1][2];
    MS_S32 g = M[2][0];
    MS_S32 h = M[2][1];
    MS_S32 i = M[2][2];

    MS_S64 ei_fh = (MS_S64)e*i - (MS_S64)f*h; // a b c a
    MS_S64 fg_di = (MS_S64)f*g - (MS_S64)d*i; // d e f d
    MS_S64 dh_eg = (MS_S64)d*h - (MS_S64)e*g; // g h i g
    MS_S64 hc_ib = (MS_S64)h*c - (MS_S64)i*b; // a b c a
    MS_S64 ia_gc = (MS_S64)i*a - (MS_S64)g*c;
    MS_S64 gb_ha = (MS_S64)g*b - (MS_S64)h*a;
    MS_S64 bf_ce = (MS_S64)b*f - (MS_S64)c*e;
    MS_S64 cd_af = (MS_S64)c*d - (MS_S64)a*f;
    MS_S64 ae_bd = (MS_S64)a*e - (MS_S64)b*d;

    MS_S64 det = a*ei_fh + b*fg_di + c*dh_eg;
    Minv[0][0] = ei_fh * 2500000000LL / det;
    Minv[1][0] = fg_di * 2500000000LL / det;
    Minv[2][0] = dh_eg * 2500000000LL / det;
    Minv[0][1] = hc_ib * 2500000000LL / det;
    Minv[1][1] = ia_gc * 2500000000LL / det;
    Minv[2][1] = gb_ha * 2500000000LL / det;
    Minv[0][2] = bf_ce * 2500000000LL / det;
    Minv[1][2] = cd_af * 2500000000LL / det;
    Minv[2][2] = ae_bd * 2500000000LL / det;
}

void MDrv_HDR_RGB2XYZGen(MS_U32 M[3][3], MS_U16 Rx, MS_U16 Ry, MS_U16 Gx, MS_U16 Gy, MS_U16 Bx, MS_U16 By, MS_U16 Wx, MS_U16 Wy)
{
    // 0 <= (R/G/B)(x/y/z) <= 50000
    MS_U16 Rz = 50000 - Rx - Ry;
    MS_U16 Gz = 50000 - Gx - Gy;
    MS_U16 Bz = 50000 - Bx - By;
    MS_U16 Wz = 50000 - Wx - Wy;

    MS_U32 WX = ((MS_U32)Wx * 50000) / Wy;
    MS_U32 WY = 50000;
    MS_U32 WZ = ((MS_U32)Wz * 50000) / Wy;

    MS_S64 det0 = MDrv_HDR_MatrixDetGen(Rx, Gx, Bx, Ry, Gy, By, Rz, Gz, Bz);
    MS_S64 det1 = MDrv_HDR_MatrixDetGen(WX, Gx, Bx, WY, Gy, By, WZ, Gz, Bz);
    MS_S64 det2 = MDrv_HDR_MatrixDetGen(Rx, WX, Bx, Ry, WY, By, Rz, WZ, Bz);
    MS_S64 det3 = MDrv_HDR_MatrixDetGen(Rx, Gx, WX, Ry, Gy, WY, Rz, Gz, WZ);
    MS_U32 RS = (MS_U32)((MS_U64) det1 * 50000 / det0);
    MS_U32 GS = (MS_U32)((MS_U64) det2 * 50000 / det0);
    MS_U32 BS = (MS_U32)((MS_U64) det3 * 50000 / det0);

    M[0][0] = (MS_U32)((MS_U64) Rx * RS / 50000);
    M[1][0] = (MS_U32)((MS_U64) Ry * RS / 50000);
    M[2][0] = (MS_U32)((MS_U64) Rz * RS / 50000);
    M[0][1] = (MS_U32)((MS_U64) Gx * GS / 50000);
    M[1][1] = (MS_U32)((MS_U64) Gy * GS / 50000);
    M[2][1] = (MS_U32)((MS_U64) Gz * GS / 50000);
    M[0][2] = (MS_U32)((MS_U64) Bx * BS / 50000);
    M[1][2] = (MS_U32)((MS_U64) By * BS / 50000);
    M[2][2] = (MS_U32)((MS_U64) Bz * BS / 50000);
}

void MDrv_HDR_RGB2RGBGen(MS_S32 Mout[3][3], MS_U32 Msrc[3][3], MS_U32 Mdst[3][3])
{
    MS_S32 Minv[3][3], M[3][3];

    M[0][0] = Mdst[0][0];
    M[0][1] = Mdst[0][1];
    M[0][2] = Mdst[0][2];
    M[1][0] = Mdst[1][0];
    M[1][1] = Mdst[1][1];
    M[1][2] = Mdst[1][2];
    M[2][0] = Mdst[2][0];
    M[2][1] = Mdst[2][1];
    M[2][2] = Mdst[2][2];
    MDrv_HDR_MatrixInvGen(Minv, M);

    // Mout = Mdst^-1 * Msrc
    Mout[0][0] = ((MS_S64)Minv[0][0]*Msrc[0][0] + (MS_S64)Minv[0][1]*Msrc[1][0] + (MS_S64)Minv[0][2]*Msrc[2][0]) / 50000;
    Mout[0][1] = ((MS_S64)Minv[0][0]*Msrc[0][1] + (MS_S64)Minv[0][1]*Msrc[1][1] + (MS_S64)Minv[0][2]*Msrc[2][1]) / 50000;
    Mout[0][2] = ((MS_S64)Minv[0][0]*Msrc[0][2] + (MS_S64)Minv[0][1]*Msrc[1][2] + (MS_S64)Minv[0][2]*Msrc[2][2]) / 50000;
    Mout[1][0] = ((MS_S64)Minv[1][0]*Msrc[0][0] + (MS_S64)Minv[1][1]*Msrc[1][0] + (MS_S64)Minv[1][2]*Msrc[2][0]) / 50000;
    Mout[1][1] = ((MS_S64)Minv[1][0]*Msrc[0][1] + (MS_S64)Minv[1][1]*Msrc[1][1] + (MS_S64)Minv[1][2]*Msrc[2][1]) / 50000;
    Mout[1][2] = ((MS_S64)Minv[1][0]*Msrc[0][2] + (MS_S64)Minv[1][1]*Msrc[1][2] + (MS_S64)Minv[1][2]*Msrc[2][2]) / 50000;
    Mout[2][0] = ((MS_S64)Minv[2][0]*Msrc[0][0] + (MS_S64)Minv[2][1]*Msrc[1][0] + (MS_S64)Minv[2][2]*Msrc[2][0]) / 50000;
    Mout[2][1] = ((MS_S64)Minv[2][0]*Msrc[0][1] + (MS_S64)Minv[2][1]*Msrc[1][1] + (MS_S64)Minv[2][2]*Msrc[2][1]) / 50000;
    Mout[2][2] = ((MS_S64)Minv[2][0]*Msrc[0][2] + (MS_S64)Minv[2][1]*Msrc[1][2] + (MS_S64)Minv[2][2]*Msrc[2][2]) / 50000;
}

void MDrv_HDR_GamutMapping3x3Gen(HDR_GamutMappingData* pData)
{
    MS_U32 Msrc[3][3], Mdst[3][3];
    MS_S32 M3x3[3][3];
    MS_U16 u16Regs[9];

    MDrv_HDR_RGB2XYZGen(Msrc, pData->sRx, pData->sRy, pData->sGx, pData->sGy, pData->sBx, pData->sBy, pData->sWx, pData->sWy);
    MDrv_HDR_RGB2XYZGen(Mdst, pData->tRx, pData->tRy, pData->tGx, pData->tGy, pData->tBx, pData->tBy, pData->tWx, pData->tWy);
    MDrv_HDR_RGB2RGBGen(M3x3, Msrc, Mdst);

    u16Regs[0] = (((M3x3[0][0] << 10) + 25000*g_u16Hdr3x3Ratio) / (50000*g_u16Hdr3x3Ratio)) & 0x1FFF;
    u16Regs[1] = (((M3x3[0][1] << 10) + 25000*g_u16Hdr3x3Ratio) / (50000*g_u16Hdr3x3Ratio)) & 0x1FFF;
    u16Regs[2] = (((M3x3[0][2] << 10) + 25000*g_u16Hdr3x3Ratio) / (50000*g_u16Hdr3x3Ratio)) & 0x1FFF;
    u16Regs[3] = (((M3x3[1][0] << 10) + 25000*g_u16Hdr3x3Ratio) / (50000*g_u16Hdr3x3Ratio)) & 0x1FFF;
    u16Regs[4] = (((M3x3[1][1] << 10) + 25000*g_u16Hdr3x3Ratio) / (50000*g_u16Hdr3x3Ratio)) & 0x1FFF;
    u16Regs[5] = (((M3x3[1][2] << 10) + 25000*g_u16Hdr3x3Ratio) / (50000*g_u16Hdr3x3Ratio)) & 0x1FFF;
    u16Regs[6] = (((M3x3[2][0] << 10) + 25000*g_u16Hdr3x3Ratio) / (50000*g_u16Hdr3x3Ratio)) & 0x1FFF;
    u16Regs[7] = (((M3x3[2][1] << 10) + 25000*g_u16Hdr3x3Ratio) / (50000*g_u16Hdr3x3Ratio)) & 0x1FFF;
    u16Regs[8] = (((M3x3[2][2] << 10) + 25000*g_u16Hdr3x3Ratio) / (50000*g_u16Hdr3x3Ratio)) & 0x1FFF;

    MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_02_L, u16Regs[0]);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_03_L, u16Regs[1]);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_04_L, u16Regs[2]);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_05_L, u16Regs[3]);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_06_L, u16Regs[4]);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_07_L, u16Regs[5]);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_08_L, u16Regs[6]);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_09_L, u16Regs[7]);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_0A_L, u16Regs[8]);
}

void MDrv_HDR_Set3x3Gen(MS_BOOL bWrite)
{
    if(TRUE==bWrite)
    {
        if(_bSetGenRead==TRUE)
        {
            // Write 3*3
            MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_01_L, u16PreRegs[8]);
            MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_02_L, u16PreRegs[9]);
            MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_03_L, u16PreRegs[10]);
            MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_04_L, u16PreRegs[11]);
            MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_05_L, u16PreRegs[12]);
            MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_06_L, u16PreRegs[13]);
            MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_07_L, u16PreRegs[14]);
            MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_08_L, u16PreRegs[15]);
            MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_09_L, u16PreRegs[16]);
            MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_0A_L, u16PreRegs[17]);
            _bSetGenRead=FALSE;
        }
    }
    else
    {
        if(_bSetGenRead==FALSE)
        {
            if(FALSE==_bSetDLCHDRInit)
            {
                HDRtmp.smin = g_HDRinitParameters.DLC_HDRToneMappingData.u16Smin ;
                HDRtmp.smax = g_HDRinitParameters.DLC_HDRToneMappingData.u16Smax;
                HDRtmp.tmax = g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmax;
                HDRtmp.tmin = g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmin;
                HDRtmp.smed = g_HDRinitParameters.DLC_HDRToneMappingData.u16Smed;
                HDRtmp.tmed = g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmed;

                g_u16HdrY2RRatio = 2;
                g_u16Hdr3x3Ratio = 8;
#if HDR_DEBUG_MODE
                MApi_GFLIP_XC_W2BYTE(REG_SC_BK1A_38_L, HDRtmp.smin);
                MApi_GFLIP_XC_W2BYTE(REG_SC_BK1A_39_L, HDRtmp.smax);
                MApi_GFLIP_XC_W2BYTE(REG_SC_BK1A_3A_L, HDRtmp.tmin);
                MApi_GFLIP_XC_W2BYTE(REG_SC_BK1A_3B_L, HDRtmp.tmax);
                MApi_GFLIP_XC_W2BYTE(REG_SC_BK1A_3C_L, HDRtmp.smed);
                MApi_GFLIP_XC_W2BYTE(REG_SC_BK1A_3D_L, HDRtmp.tmed);

                MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK1A_3E_L, 0,0xFF);
                MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK1A_3E_L, 0,0xFF00);
                MApi_GFLIP_XC_W2BYTE(REG_SC_BK1A_3F_L, 0x208);
#endif

                _bSetDLCHDRInit=TRUE;
            }

            // Read 3*3
            u16PreRegs[8]=MApi_GFLIP_XC_R2BYTE(REG_SC_BK25_01_L);
            u16PreRegs[9]=MApi_GFLIP_XC_R2BYTE(REG_SC_BK25_02_L);
            u16PreRegs[10]=MApi_GFLIP_XC_R2BYTE(REG_SC_BK25_03_L);
            u16PreRegs[11]=MApi_GFLIP_XC_R2BYTE(REG_SC_BK25_04_L);
            u16PreRegs[12]=MApi_GFLIP_XC_R2BYTE(REG_SC_BK25_05_L);
            u16PreRegs[13]=MApi_GFLIP_XC_R2BYTE(REG_SC_BK25_06_L);
            u16PreRegs[14]=MApi_GFLIP_XC_R2BYTE(REG_SC_BK25_07_L);
            u16PreRegs[15]=MApi_GFLIP_XC_R2BYTE(REG_SC_BK25_08_L);
            u16PreRegs[16]=MApi_GFLIP_XC_R2BYTE(REG_SC_BK25_09_L);
            u16PreRegs[17]=MApi_GFLIP_XC_R2BYTE(REG_SC_BK25_0A_L);

            _bSetGenRead=TRUE;
        }
    }
}

MS_U32 MDrv_HDR_PQ(MS_U16 u16In)
{
    MS_U16 u16Shift, u16Idx, u16Lsb, u16Tmp, u16Exp;
    MS_U32 u32Lut0_out, u32Lut1_out, u32Result;

    const MS_U16 u16PQCurveExpLut[705] =
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
      0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
      0x1, 0x1, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
      0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x3, 0x3, 0x3, 0x3,
      0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
      0x3, 0x3, 0x3, 0x3, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4,
      0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4,
      0x4, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5,
      0x5, 0x5, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6,
      0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7,
      0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x8, 0x8,
      0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x9, 0x9, 0x9, 0x9,
      0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0xA, 0xA, 0xA, 0xA, 0xA,
      0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xB, 0xB, 0xB, 0xB, 0xB,
      0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xC, 0xC, 0xC, 0xC,
      0xC, 0xC, 0xC, 0xC, 0xC, 0xC, 0xC, 0xD, 0xD, 0xD, 0xD, 0xD, 0xD, 0xD, 0xD, 0xD,
      0xE, 0xE, 0xE, 0xE, 0xE, 0xE, 0xE, 0xE, 0xE, 0xE, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
      0xF, 0xF, 0xF, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11,
      0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
      0x13 };

    const MS_U16 u16PQCurveMantLut[705] =
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
      0x2, 0x2, 0x2, 0x2, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x4, 0x4, 0x5, 0x5, 0x5,
      0x5, 0x6, 0x6, 0x6, 0x7, 0x7, 0x7, 0x8, 0x8, 0x8, 0x9, 0x9, 0x9, 0xA, 0xA, 0xA,
      0xB, 0xB, 0xB, 0xC, 0xC, 0xD, 0xD, 0xD, 0xE, 0xE, 0xF, 0xF, 0x10, 0x10, 0x11, 0x11,
      0x11, 0x12, 0x12, 0x13, 0x13, 0x14, 0x14, 0x15, 0x15, 0x16, 0x16, 0x17, 0x17, 0x18, 0x18, 0x19,
      0x19, 0x1A, 0x1B, 0x1B, 0x1C, 0x1C, 0x1D, 0x1D, 0x1E, 0x1F, 0x1F, 0x20, 0x20, 0x21, 0x21, 0x22,
      0x23, 0x23, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x28, 0x28, 0x29, 0x2A, 0x2A, 0x2B, 0x2C, 0x2C,
      0x2D, 0x2E, 0x2E, 0x2F, 0x30, 0x31, 0x31, 0x32, 0x33, 0x33, 0x34, 0x35, 0x36, 0x36, 0x37, 0x38,
      0x39, 0x3A, 0x3C, 0x3D, 0x3F, 0x40, 0x42, 0x44, 0x45, 0x47, 0x48, 0x4A, 0x4C, 0x4E, 0x4F, 0x51,
      0x53, 0x55, 0x56, 0x58, 0x5A, 0x5C, 0x5E, 0x60, 0x62, 0x63, 0x65, 0x67, 0x69, 0x6B, 0x6D, 0x6F,
      0x71, 0x73, 0x75, 0x77, 0x7A, 0x7C, 0x7E, 0x80, 0x82, 0x84, 0x86, 0x89, 0x8B, 0x8D, 0x8F, 0x91,
      0x94, 0x96, 0x98, 0x9B, 0x9D, 0x9F, 0xA2, 0xA4, 0xA7, 0xA9, 0xAB, 0xAE, 0xB0, 0xB3, 0xB5, 0xB8,
      0xBA, 0xBF, 0xC5, 0xCA, 0xCF, 0xD4, 0xDA, 0xDF, 0xE5, 0xEA, 0xF0, 0xF6, 0xFB, 0x101, 0x107, 0x10D,
      0x113, 0x119, 0x11F, 0x126, 0x12C, 0x132, 0x139, 0x13F, 0x146, 0x14C, 0x153, 0x159, 0x160, 0x167, 0x16E, 0x175,
      0x17C, 0x183, 0x18A, 0x191, 0x198, 0x1A0, 0x1A7, 0x1AF, 0x1B6, 0x1BE, 0x1C5, 0x1CD, 0x1D5, 0x1DC, 0x1E4, 0x1EC,
      0x1F4, 0x1FC, 0x204, 0x20D, 0x215, 0x21D, 0x225, 0x22E, 0x236, 0x23F, 0x247, 0x250, 0x259, 0x262, 0x26A, 0x273,
      0x27C, 0x28E, 0x2A1, 0x2B4, 0x2C6, 0x2DA, 0x2ED, 0x301, 0x314, 0x329, 0x33D, 0x352, 0x366, 0x37C, 0x391, 0x3A7,
      0x3BD, 0x3D3, 0x3E9, 0x400, 0x417, 0x42E, 0x445, 0x45D, 0x475, 0x48D, 0x4A6, 0x4BF, 0x4D8, 0x4F1, 0x50A, 0x524,
      0x53E, 0x559, 0x573, 0x58E, 0x5A9, 0x5C4, 0x5E0, 0x5FC, 0x618, 0x635, 0x651, 0x66E, 0x68B, 0x6A9, 0x6C7, 0x6E5,
      0x703, 0x722, 0x741, 0x760, 0x77F, 0x79F, 0x7BF, 0x7DF, 0x800, 0x820, 0x841, 0x863, 0x884, 0x8A6, 0x8C8, 0x8EB,
      0x90E, 0x954, 0x99B, 0x9E4, 0xA2E, 0xA79, 0xAC5, 0xB12, 0xB60, 0xBB0, 0xC00, 0xC52, 0xCA5, 0xCF9, 0xD4F, 0xDA6,
      0xDFD, 0xE56, 0xEB1, 0xF0C, 0xF69, 0xFC7, 0x1026, 0x1086, 0x10E8, 0x114B, 0x11AF, 0x1214, 0x127B, 0x12E3, 0x134C, 0x13B7,
      0x1423, 0x1490, 0x14FE, 0x156E, 0x15DF, 0x1651, 0x16C5, 0x173A, 0x17B1, 0x1828, 0x18A1, 0x191C, 0x1998, 0x1A15, 0x1A93, 0x1B13,
      0x1B95, 0x1C17, 0x1C9B, 0x1D21, 0x1DA8, 0x1E30, 0x1EBA, 0x1F45, 0x1FD2, 0x2060, 0x20EF, 0x2180, 0x2213, 0x22A7, 0x233C, 0x23D3,
      0x246B, 0x25A1, 0x26DC, 0x281D, 0x2965, 0x2AB2, 0x2C06, 0x2D60, 0x2EC1, 0x3028, 0x3195, 0x3309, 0x3483, 0x3604, 0x378C, 0x391A,
      0x3AB0, 0x3C4C, 0x3DEF, 0x3F99, 0x20A5, 0x2181, 0x2260, 0x2344, 0x242A, 0x2515, 0x2603, 0x26F5, 0x27EA, 0x28E3, 0x29E1, 0x2AE1,
      0x2BE6, 0x2CEF, 0x2DFB, 0x2F0C, 0x3020, 0x3139, 0x3256, 0x3376, 0x349B, 0x35C4, 0x36F1, 0x3822, 0x3958, 0x3A91, 0x3BCF, 0x3D12,
      0x3E58, 0x3FA4, 0x207A, 0x2124, 0x21D0, 0x227F, 0x232F, 0x23E3, 0x2498, 0x2550, 0x260A, 0x26C6, 0x2785, 0x2847, 0x290A, 0x29D0,
      0x2A99, 0x2C32, 0x2DD4, 0x2F81, 0x3139, 0x32FA, 0x34C7, 0x369E, 0x3880, 0x3A6D, 0x3C66, 0x3E6A, 0x203D, 0x214A, 0x225E, 0x2378,
      0x2497, 0x25BD, 0x26EA, 0x281C, 0x2955, 0x2A95, 0x2BDB, 0x2D28, 0x2E7C, 0x2FD7, 0x3138, 0x32A1, 0x3411, 0x3588, 0x3707, 0x388D,
      0x3A1B, 0x3BB0, 0x3D4D, 0x3EF2, 0x2050, 0x212A, 0x2209, 0x22EC, 0x23D3, 0x24BE, 0x25AD, 0x26A1, 0x2799, 0x2896, 0x2997, 0x2A9D,
      0x2BA8, 0x2CB7, 0x2DCB, 0x2EE3, 0x3001, 0x3123, 0x324B, 0x3377, 0x34A9, 0x35E0, 0x371C, 0x385D, 0x39A4, 0x3AF0, 0x3C42, 0x3D99,
      0x3EF6, 0x20E0, 0x2251, 0x23CE, 0x2558, 0x26ED, 0x2890, 0x2A3F, 0x2BFC, 0x2DC7, 0x2F9F, 0x3186, 0x337C, 0x3581, 0x3795, 0x39B8,
      0x3BEC, 0x3E30, 0x2043, 0x2176, 0x22B2, 0x23F6, 0x2544, 0x269C, 0x27FC, 0x2967, 0x2ADB, 0x2C5A, 0x2DE3, 0x2F77, 0x3116, 0x32C0,
      0x3475, 0x3635, 0x3802, 0x39DA, 0x3BBF, 0x3DB1, 0x3FAF, 0x20DD, 0x21EA, 0x22FD, 0x2417, 0x2539, 0x2661, 0x2791, 0x28C9, 0x2A08,
      0x2B4F, 0x2C9F, 0x2DF6, 0x2F56, 0x30BE, 0x322F, 0x33A9, 0x352C, 0x36B9, 0x384E, 0x39EE, 0x3B97, 0x3D4A, 0x3F08, 0x2068, 0x2151,
      0x2240, 0x242E, 0x2633, 0x284F, 0x2A85, 0x2CD4, 0x2F3F, 0x31C5, 0x3468, 0x3729, 0x3A09, 0x3D09, 0x2016, 0x21B9, 0x236D, 0x2535,
      0x2710, 0x2900, 0x2B04, 0x2D1E, 0x2F4F, 0x3198, 0x33F9, 0x3673, 0x3908, 0x3BB7, 0x3E83, 0x20B7, 0x223A, 0x23CE, 0x2572, 0x2727,
      0x28ED, 0x2AC6, 0x2CB1, 0x2EB0, 0x30C4, 0x32EC, 0x352A, 0x377F, 0x39EB, 0x3C6F, 0x3F0D, 0x20E2, 0x224C, 0x23C3, 0x2548, 0x26DD,
      0x2881, 0x2A36, 0x2BFB, 0x2DD1, 0x2FB9, 0x31B4, 0x33C2, 0x35E3, 0x381A, 0x3A66, 0x3CC8, 0x3F40, 0x20E9, 0x223D, 0x239E, 0x250D,
      0x2689, 0x29AC, 0x2D0C, 0x30AC, 0x3493, 0x38C4, 0x3D46, 0x210F, 0x23A9, 0x2675, 0x2975, 0x2CAF, 0x3026, 0x33DE, 0x37DD, 0x3C27,
      0x2061, 0x22DA, 0x2582, 0x285B, 0x2B6A, 0x2EB3, 0x323A, 0x3603, 0x3A14, 0x3E71, 0x2190, 0x2414, 0x26C7, 0x29AE, 0x2CCB, 0x3023,
      0x33BA, 0x3796, 0x3BBB, 0x2017, 0x227C, 0x250E, 0x27D1, 0x2AC8, 0x2DF9, 0x3167, 0x3518, 0x390F, 0x3D53, 0x20F5, 0x236D, 0x2616,
      0x28F2, 0x2C06, 0x2F57, 0x32EA, 0x36C2, 0x3AE8, 0x3F60, 0x2219, 0x24B2, 0x277F, 0x2A85, 0x2DC8, 0x314D, 0x351B, 0x3936, 0x3DA6,
      0x2129
    };

    if (u16In == 0)
        return 0;
    if (u16In > 0xFF00)
        u16In = 0xFF00;

    u16Tmp = u16In;
    for (u16Shift = 0; u16Tmp > 127; u16Shift++)
        u16Tmp >>= 1;
    u16Idx = (u16In >> u16Shift) + 64*u16Shift;
    u16Lsb = u16In & ((0x1 << u16Shift) - 1);
    u32Lut0_out = u16PQCurveMantLut[u16Idx];
    u32Lut1_out = u16PQCurveMantLut[u16Idx+1];
    u16Exp = u16PQCurveExpLut[u16Idx];
    if (u16Exp != u16PQCurveExpLut[u16Idx+1])
        u32Lut1_out <<= 1;
    if (u16Shift > u16PQCurveExpLut[u16Idx])
        u32Result = (u32Lut0_out << u16Exp) + ((u32Lut1_out - u32Lut0_out) * u16Lsb >> (u16Shift - u16Exp));
    else
        u32Result = (u32Lut0_out << u16Exp) + ((u32Lut1_out - u32Lut0_out) * u16Lsb << (u16Exp - u16Shift));
    //printk("%x -> %x; %x %x %x %x %x %x\n", u16In, u32Result, u16Idx, u16Lsb, u32Lut0_out, u32Lut1_out, u16Shift, u16Exp);
    return u32Result;
}

// Input max 0xFF00; Output max 0xFFFF0000
MS_U32 MDrv_HDR_DeGamma(MS_U16 u16In, MS_U16 u16Gamma, MS_U16 u16Ginv, MS_U16 u16A, MS_U16 u16B, MS_U16 u16S)
{
    // u16Gamma : 4.12
    // u16GInv  : 1.12
    // u16A : 1.15
    // u16B : 0.16
    // u16S : 6.10
    MS_U32 u32Th, u32Tmp, u32Out;

    u32Th = (MS_U32)u16S * u16B >> 10;
    if (u16In <= u32Th)
        return ((MS_U64)u16In << 10) / u16S << 16;
    u32Tmp = (MS_U32)(u16In + ((u16A - 0x7F80) << 1)) * 0x7FFF / u16A;
    u32Out = MDrv_HDR_Pow(u32Tmp + (u32Tmp>>15), u16Gamma);
    return u32Out << 16;
}

// Input max 0xFF00; Output max 0xFFFF
MS_U32 MDrv_HDR_Bt1886Gamma(MS_U16 u16In, MS_U16 u16Gamma, MS_U16 u16Ginv, MS_U16 u16White, MS_U16 u16Black)
{
    // u16Gamma : 4.12
    // u16GInv  : 1.12
    // u16White : 0.16
    // u16Black : 0.16
    MS_U32 u32Tmp, u32A, u32B, u32Out;

    u32Tmp = MDrv_HDR_Pow(u16White, u16Ginv) - MDrv_HDR_Pow(u16Black, u16Ginv);
    u32A = MDrv_HDR_Pow((MS_U16)u32Tmp, u16Gamma);
    u32Tmp = ((MS_U32)u16Black << 16) / u32A;
    u32B = MDrv_HDR_Pow(u32Tmp, u16Ginv);
    u32Tmp = ((MS_U32)u16In << 16) / u32A;
    u32Out = (u32Tmp < 0xFFFF) ? MDrv_HDR_Pow(u32Tmp, u16Ginv) : (MDrv_HDR_Pow(u32Tmp >> 1, u16Ginv) << 16) / MDrv_HDR_Pow(0x8000, u16Ginv);
    u32Out = (u32Out > u32B) ? u32Out - u32B : 0; // 16b
    return u32Out;
}

// For non-dedicated HDR -> using xvYCC_degamma
void MDrv_HDR_WriteDegammaTable_DeltaMode_600Entries(MS_U32* u32Lut)
{
    MS_U16 u16InputData1, u16InputData2, u16InputData3;
    MS_U32 u32Ref, u32Delta1, u32Delta2;
    MS_U32 u32XVYCC_DGT_DATA;
    MS_U16 u16SRamCode_L, u16SRamCode_H;
    MS_U16 u16WriteCount, u16SpLevel, u16SpAddr[4]={0};
    MS_U16 u16Delay = 0xFFFF;

    // RGB write together and write one time for clear write bit.
    if( MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK2D_60_L, BIT(5)|BIT(6))!= (BIT(5)|BIT(6)))
    {
        MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK2D_60_L, BIT(5)|BIT(6), BIT(5)|BIT(6) );
    }

    u16SpLevel = 0;
    for(u16WriteCount = 0; u16WriteCount < 300; u16WriteCount++)
    {
        u16InputData1 = (u32Lut[u16WriteCount*2]  >>17)*g_u16Hdr3x3Ratio/8;
        u16InputData2 = (u32Lut[u16WriteCount*2+1]>>17)*g_u16Hdr3x3Ratio/8;
        u16InputData3 = (u32Lut[u16WriteCount*2+2]>>17)*g_u16Hdr3x3Ratio/8;

        u32Ref = min(u16InputData1, 0x7FFF); // 15bis
        u32Delta1 = min(u16InputData2 - u16InputData1, 0x7F); // 7 bits
        u32Delta2 = min(u16InputData3 - u16InputData2, 0x7F); // 7 bits

        if ((u32Delta1>>5) > u16SpLevel)
        {
            u16SpAddr[u16SpLevel] = u16WriteCount*2;
            u16SpLevel++;
        }
        else if ((u32Delta2>>5) > u16SpLevel)
        {
            u16SpAddr[u16SpLevel] = u16WriteCount*2+1;
            u16SpLevel++;
        }

        u32Delta1 = u32Delta1 & 0x1F; // 5 bits
        u32Delta2 = u32Delta2 & 0x1F; // 5 bits
        u32XVYCC_DGT_DATA = (u32Ref<<10) + (u32Delta1<<5) + u32Delta2;

        u16SRamCode_L = u32XVYCC_DGT_DATA&0xFFFF;
        u16SRamCode_H = u32XVYCC_DGT_DATA>>16;

        // waiting ready.
        while(MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK2D_60_L, BIT(3))&& (--u16Delay)){;}

        if(u16Delay==0)
        {
            DLC_DEBUG("\n Kernel HDR WriteGamma timeout \n");
        }

        MApi_GFLIP_XC_W2BYTE(REG_SC_BK2D_61_L, u16WriteCount); // address
        MApi_GFLIP_XC_W2BYTE(REG_SC_BK2D_63_L, u16SRamCode_H); //data
        MApi_GFLIP_XC_W2BYTE(REG_SC_BK2D_62_L, u16SRamCode_L); //data
        MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK2D_60_L, BIT(3), BIT(3)); // io_w enable
    }

    //write offset value separate point
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK2D_66_L, u16SpAddr[0]);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK2D_67_L, u16SpAddr[1]);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK2D_68_L, u16SpAddr[2]);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK2D_69_L, u16SpAddr[0]);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK2D_6A_L, u16SpAddr[1]);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK2D_6B_L, u16SpAddr[2]);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK2D_6C_L, u16SpAddr[0]);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK2D_6D_L, u16SpAddr[1]);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK2D_6E_L, u16SpAddr[2]);

    //write offset value
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK2D_6F_L, 0x00E4);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK2D_70_L, 0x00E4);
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK2D_71_L, 0x00E4);
}

// For non-dedicated HDR -> using xvYCC_gamma
void MDrv_HDR_WriteGammaTable_256Entries(void)
{
    MS_U16 u16Data, u16SRamcode, u16Index;
    MS_U16 u16Gamma = 0x2333; // 2.2
    MS_U16 u16White = 0xFF00; // 1
    MS_U16 u16Black = 0x42; // 0.001
    MS_U16 u16Ginv = 0x1000000ul / u16Gamma; // (1<<24) / 4.12 -> 1.12

    for(u16Index = 0; u16Index < 256; u16Index++)
    {
        u16Data = MDrv_HDR_Bt1886Gamma(u16Index<<8, u16Gamma, u16Ginv, u16White, u16Black);
        u16SRamcode = (u16Data - (u16Data>>8)) >> 4;

        // waiting ready.
        while (MApi_GFLIP_XC_R2BYTE(REG_SC_BK25_78_L) & BIT(3));

        MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_79_L, u16Index); // address
        MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_7A_L, u16SRamcode); //data
        MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK25_78_L, 0x60, 0x60); // Select All Channel
        MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK25_78_L, BIT(3), BIT(3)); // io_w enable
    }
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK25_7B_L, u16SRamcode); //Max data
}

void MDrv_HDR_TransferFunctionGen(HDR_TransferFunctionData* pData,MS_U32 *u32LutData)
{
    MS_U16 u16Val, u16NumEntries, u16LutIdx, u16Index;
    MS_U64 u64Nume, u64Denum;

    // u16A : 0.16
    // u16B : 0.16
    // u16S : 6.10
    // u16Gamma : 4.12
    // u16Ginv  : 4.12
    MS_U16 u16Gamma, u16A, u16B, u16S, u16Ginv;

    if(pData->sValid == FALSE)
    {
        HDR_DBG_HAL(DLC_DEBUG("\n    Kernel      MDrv_HDR_TransferFunctionGen    ERROR   u8ColorPrimaries=%d     \n",g_HDRinitParameters.DLC_HDRMetadataMpegVUI.u8ColorPrimaries));
        return;
    }

    // DeGamma
    if (pData->sMode == 0)
    {
        u16Gamma = pData->sData[0];
        u16B = pData->sData[1];
        u16Ginv = 0x1000000ul / u16Gamma; // (1<<24) / 4.12 -> 1.12
        if (u16B == 0)
        {
            u16S = 0;
            u16A = 0x8000; // value = 1.0
        }
        else
        {
            u16S = 0x1200; // value = 4.5
            for (u16Index = 0; u16Index < 8; u16Index++)
            {
                u16A = min(((MS_U64)u16S * u16B) * (u16Gamma - 0x1000) >> 23, 0xFFFF) + 0x8000; // 6.10*0.16*4.12 -> 10.38 -> 1.15
                u16S = min(((MS_U32)u16A << (12-5)) / (u16Gamma * MDrv_HDR_Pow(u16B, 0x1000-u16Ginv) >> 16), 0xFFFF); // 1.15/(4.12*0.16>>16)  = 1.15/4.12
            }
        }

        u16NumEntries = 256;
        for (u16LutIdx = 0; u16LutIdx < u16NumEntries; u16LutIdx++)
        {
            u16Val = ((MS_U32)u16LutIdx << 8);
            u32LutData[u16LutIdx] = MDrv_HDR_DeGamma(u16Val, u16Gamma, u16Ginv, u16A, u16B, u16S);
        }
        // input > 1 case --> to fix
        for ( ; u16LutIdx < 601; u16LutIdx++)
            u32LutData[u16LutIdx] = u32LutData[u16NumEntries-1];
    }
    else if (pData->sMode == 1) // DCI-P3
    {
        // todo
        HDR_DBG_HAL(DLC_DEBUG("\n  Kernel Error!! sMode is 1 \n"));
    }
    else if (pData->sMode == 2) // PQ
    {
        u16NumEntries = 256 * g_u16HdrY2RRatio;
        u64Denum = MDrv_HDR_PQ(pData->tData[0]); // need to be set by register!
        for (u16LutIdx = 0; u16LutIdx < u16NumEntries; u16LutIdx++)
        {
            u16Val = ((MS_U32)u16LutIdx << 8) / g_u16HdrY2RRatio;
            u64Nume = MDrv_HDR_PQ(u16Val);
            u32LutData[u16LutIdx] = min(u64Nume * 0xFFF00000ul / u64Denum, 0xFFF00000ul);
        }
        for ( ; u16LutIdx < 601; u16LutIdx++)
            u32LutData[u16LutIdx] = u32LutData[u16NumEntries-1];
    }
    else// if (mode == 3) // Log
    {
        // todo
        HDR_DBG_HAL(DLC_DEBUG("\n  Kernel Error!! sMode is 3 \n"));
    }
    MDrv_HDR_WriteDegammaTable_DeltaMode_600Entries(u32LutData);

    // Gamma
    MDrv_HDR_WriteGammaTable_256Entries();

}

// Document : JCTVC-P1005_v1_Annex_E_changes_for_Q0084_r1
void MDrv_HDR_ConfigMetadata_MpegVUI( HDR_TransferFunctionData* pTFData, HDR_GamutMappingData* pGMData)
{
    // Color Primaries
    pGMData->sValid = TRUE;
    switch (g_HDRinitParameters.DLC_HDRMetadataMpegVUI.u8ColorPrimaries )
    {
     case 1:
        pGMData->sGx = 15000; //0.30
        pGMData->sGy = 30000; //0.60
        pGMData->sBx =  7500; //0.15
        pGMData->sBy =  3000; //0.06
        pGMData->sRx = 32000; //0.64
        pGMData->sRy = 16500; //0.33
        pGMData->sWx = 15635; //0.3127
        pGMData->sWy = 16450; //0.3290
        break;
     case 4:
        pGMData->sGx = 10500; //0.21
        pGMData->sGy = 35500; //0.71
        pGMData->sBx =  7000; //0.14
        pGMData->sBy =  4000; //0.08
        pGMData->sRx = 33500; //0.67
        pGMData->sRy = 16500; //0.33
        pGMData->sWx = 15500; //0.310
        pGMData->sWy = 15800; //0.316
        break;
     case 5:
        pGMData->sGx = 14500; //0.29
        pGMData->sGy = 30000; //0.60
        pGMData->sBx =  7500; //0.15
        pGMData->sBy =  3000; //0.06
        pGMData->sRx = 32000; //0.64
        pGMData->sRy = 16500; //0.33
        pGMData->sWx = 15635; //0.3127
        pGMData->sWy = 16450; //0.3290
        break;
     case 6:
     case 7:
        pGMData->sGx = 15500; //0.31
        pGMData->sGy = 29750; //0.595
        pGMData->sBx =  7750; //0.155
        pGMData->sBy =  3500; //0.07
        pGMData->sRx = 31500; //0.63
        pGMData->sRy = 17000; //0.34
        pGMData->sWx = 15635; //0.3127
        pGMData->sWy = 16450; //0.3290
        break;
     case 8:
        pGMData->sGx = 12150; //0.243
        pGMData->sGy = 34600; //0.692
        pGMData->sBx =  7250; //0.145
        pGMData->sBy =  2450; //0.049
        pGMData->sRx = 34050; //0.681
        pGMData->sRy = 15950; //0.319
        pGMData->sWx = 15500; //0.310
        pGMData->sWy = 15800; //0.316
        break;
     case 9:
        pGMData->sGx =  8500; //0.17
        pGMData->sGy = 39850; //0.797
        pGMData->sBx =  6550; //0.131
        pGMData->sBy =  2300; //0.046
        pGMData->sRx = 35400; //0.708
        pGMData->sRy = 14600; //0.292
        pGMData->sWx = 15635; //0.3127
        pGMData->sWy = 16450; //0.3290
        break;
     case 10:
        pGMData->sGx =     0; //0.0
        pGMData->sGy = 50000; //1.0
        pGMData->sBx =     0; //0.0
        pGMData->sBy =     0; //0.0
        pGMData->sRx = 50000; //1.0
        pGMData->sRy =     0; //0.0
        pGMData->sWx = 16650; //0.333
        pGMData->sWy = 16650; //0.333
        break;
     case 2: // Unspecified
     default:
        pGMData->sValid = FALSE;
        break;
    }

    if (g_HDRinitParameters.DLC_HDRCustomerColorPrimaries.bCustomerEnable == TRUE)
    {
        pGMData->sWx = g_HDRinitParameters.DLC_HDRCustomerColorPrimaries.u16sWx;
        pGMData->sWy = g_HDRinitParameters.DLC_HDRCustomerColorPrimaries.u16sWy;
    }

    // Transfer Characteristics
    pTFData->sValid = TRUE;

    pTFData->tData[0] = g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmax << 6;

    switch (g_HDRinitParameters.DLC_HDRMetadataMpegVUI.u8TransferCharacteristics)
    {
     case 1: // 709
     case 6: // 601
     case 11: // xvYCC
     case 14: // 2020 10b
     case 15: // 2020 12b
        pTFData->sMode = 0;
        pTFData->sData[0] = 0x8E39>>2; // 1/0.45 = 2.222
        pTFData->sData[1] = 0x49C; // 0.018
        break;
     case 4: // gamma 2.2
        pTFData->sMode = 0;
        pTFData->sData[0] = 0x8CCD>>2; // 2.2
        pTFData->sData[1] = 0; // 0
        break;
     case 5: // gamma 2.8
        pTFData->sMode = 0;
        pTFData->sData[0] = 0xB333>>2; // 2.8
        pTFData->sData[1] = 0; // 0
        break;
     case 7: // 240M
        pTFData->sMode = 0;
        pTFData->sData[0] = 0xB333>>2; // 1/0.45 = 2.222
        pTFData->sData[1] = 0x5D6; // 0.0228
        break;
     case 8: // Linear
        pTFData->sMode = 0;
        pTFData->sData[0] = 0x4000>>2; // 1
        pTFData->sData[1] = 0; // 0
        break;
     case 9: // Log (100:1)
        pTFData->sMode = 3;
        pTFData->sData[0] = 0x2000; // V=1+log10(L)/2
        pTFData->sData[1] = 0x28F; // 0 if L<0.01
        break;
     case 10: // Log (100*sqrt(10) : 1)
        pTFData->sMode = 3;
        pTFData->sData[0] = 0x199A; // V=1+log10(L)/2.5
        pTFData->sData[1] = 0xCF; // 0 if L<sqrt(10)/1000
        break;
     case 12: // 1361 --> actually not supported for negative values
        pTFData->sMode = 0;
        pTFData->sData[0] = 0x8E39>>2; // 1/0.45 = 2.222
        pTFData->sData[1] = 0x49C; // 0.018
        break;
     case 13: // sRGB
        pTFData->sMode = 0;
        pTFData->sData[0] = 0x999A>>2; // 2.4
        pTFData->sData[1] = 0xCD; // 0.0031308
        break;
     case 16: // 2084
        pTFData->sMode = 2;
        pTFData->sData[0] = 0; // 0
        pTFData->sData[1] = 0; // 0
        break;
     case 17: // 428-1
        pTFData->sMode = 1;
        pTFData->sData[0] = 0xA666>>2; // 2.6
        pTFData->sData[1] = 0; // 0
        break;
     case 2: // Unspecified
     default:
        pTFData->sValid = FALSE;
        break;
    }

}
void MDrv_HDR_ConfigMetadata_HdmiTxInfoFrame( HDR_ToneMappingData* pTMData, HDR_TransferFunctionData* pTFData, HDR_GamutMappingData* pGMData)
{
    // Transfer Characteristics
    pTFData->sValid = TRUE;
    switch (g_HDRinitParameters.DLC_HDRMetadataHdmiTxInfoFrame.u8EOTF)
    {
     case 0: // SDR gamma --> use 709
        pTFData->sMode = 0;
        pTFData->sData[0] = 0x8E39>>2; // 1/0.45 = 2.222
        pTFData->sData[1] = 0x49C; // 0.018
        break;
#if 0 // support not yet
     case 1: // HDR gamma --> use x^2.2
        pTFData->sMode = 0;
        pTFData->sData[0] = 0x8CCD>>2; // 2.2
        pTFData->sData[1] = 0; // 0
        break;
#endif
     case 2: // 2084
        pTFData->sMode = 2;
        pTFData->sData[0] = 0; // 0
        pTFData->sData[1] = 0; // 0
        break;
     case 3: // Unspecified
     default:
        pTFData->sValid = FALSE;
        break;
    }

    pTFData->tData[0] = g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmax << 6;
    // Color Primaries  (Note: Don't care about whether the pixel format is RGB or YCC. Just assume input to VOP2 is YCC!! Important!!!!!!!!!)
    pGMData->sValid = TRUE;
    if (g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.Colorimetry == 0 || g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.Colorimetry == 2) // No data or 709
    {
        // 709 color primaries
        pGMData->sGx = 15000; //0.30
        pGMData->sGy = 30000; //0.60
        pGMData->sBx =  7500; //0.15
        pGMData->sBy =  3000; //0.06
        pGMData->sRx = 32000; //0.64
        pGMData->sRy = 16500; //0.33
        pGMData->sWx = 15635; //0.3127
        pGMData->sWy = 16450; //0.3290
    }
    else if (g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.Colorimetry == 1) // 170M (=601)
    {
        // 601 color primaries
        pGMData->sGx = 15500; //0.31
        pGMData->sGy = 29750; //0.595
        pGMData->sBx =  7750; //0.155
        pGMData->sBy =  3500; //0.07
        pGMData->sRx = 31500; //0.63
        pGMData->sRy = 17000; //0.34
        pGMData->sWx = 15635; //0.3127
        pGMData->sWy = 16450; //0.3290
    }
    else if (g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.Colorimetry == 3 && (g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.ExtendedColorimetry == 0 || g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.ExtendedColorimetry == 1 || g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.ExtendedColorimetry == 2)) // xvYCC 709 or xvYCC 601 or sYCC 601
    {
        // 709 color primaries
        pGMData->sGx = 15000; //0.30
        pGMData->sGy = 30000; //0.60
        pGMData->sBx =  7500; //0.15
        pGMData->sBy =  3000; //0.06
        pGMData->sRx = 32000; //0.64
        pGMData->sRy = 16500; //0.33
        pGMData->sWx = 15635; //0.3127
        pGMData->sWy = 16450; //0.3290
    }
    else if (g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.Colorimetry == 3 && (g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.ExtendedColorimetry == 3 || g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.ExtendedColorimetry == 4)) // Adobe YCC 601 or RGB
    {
        // Adobe color primaries
        pGMData->sGx = 10500; //0.21
        pGMData->sGy = 35500; //0.71
        pGMData->sBx =  7500; //0.15
        pGMData->sBy =  3000; //0.06
        pGMData->sRx = 32000; //0.64
        pGMData->sRy = 16500; //0.33
        pGMData->sWx = 15635; //0.3127
        pGMData->sWy = 16450; //0.3290
    }
    else if (g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.Colorimetry == 3 && (g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.ExtendedColorimetry == 5 || g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.ExtendedColorimetry == 6)) // BT2020 CL YCC
    {
        // 2020 color primaries
        pGMData->sGx =  8500; //0.17
        pGMData->sGy = 39850; //0.797
        pGMData->sBx =  6550; //0.131
        pGMData->sBy =  2300; //0.046
        pGMData->sRx = 35400; //0.708
        pGMData->sRy = 14600; //0.292
        pGMData->sWx = 15635; //0.3127
        pGMData->sWy = 16450; //0.3290
    }
    else
    {
        // Please print ERROR here!!
        HDR_DBG_HAL(DLC_DEBUG("\n Please print ERROR here\n"));
    }
}

void MDrv_HDR_ConfigMetadata_HdmiRxDataBlock( HDR_ToneMappingData* pTMData, HDR_TransferFunctionData* pTFData, HDR_Metadata_HdmiRx_DataBlock* stMetadata )
{
    // Transfer Characteristics
    pTFData->tValid = TRUE;
    switch (stMetadata->u8EOTF)
    {
     case 0: // SDR gamma --> use 709
        pTFData->tMode = 0;
        pTFData->tData[0] = 0x8E39>>2; // 1/0.45 = 2.222
        pTFData->tData[1] = 0x49C; // 0.018
        break;
     case 1: // HDR gamma --> use x^2.2
        pTFData->tMode = 0;
        pTFData->tData[0] = 0x8CCD>>2; // 2.2
        pTFData->tData[1] = 0; // 0
        break;
     case 2: // 2084
        pTFData->tMode = 2;
        pTFData->tData[0] = 0; // 0
        pTFData->tData[1] = 0; // 0
        break;
     case 3: // Unspecified
     default:
        pTFData->tValid = FALSE;
        break;
    }

}


// Reference code for Wright
void MDrv_HDR_Main(HDR_TransferFunctionData* stTFData ,MS_U32 *u32LutData )
{

    HDR_GamutMappingData stGMData = {0, 0, 35400, 14600, 8500, 39850, 6550, 2300, 15635, 16450
        , g_HDRinitParameters.DLC_HDRGamutMappingData.u16tRx
        , g_HDRinitParameters.DLC_HDRGamutMappingData.u16tRy
        , g_HDRinitParameters.DLC_HDRGamutMappingData.u16tGx
        , g_HDRinitParameters.DLC_HDRGamutMappingData.u16tGy
        , g_HDRinitParameters.DLC_HDRGamutMappingData.u16tBx
        , g_HDRinitParameters.DLC_HDRGamutMappingData.u16tBy
        , g_HDRinitParameters.DLC_HDRGamutMappingData.u16tWx
        , g_HDRinitParameters.DLC_HDRGamutMappingData.u16tWy};

    //HDR_Metadata_HdmiRx_DataBlock stMetadata = {0, 0, 0, 0};
    if(ENABLE==g_HDRinitParameters.bHDREnable)
    {
        // Read 3*3
        MDrv_HDR_Set3x3Gen(FALSE);

        //HDR ON
        MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK25_01_L, 0x007 , 0x107);


        if(g_HDRinitParameters.u16HDRFunctionSelect & 0x01 )
        {
            MDrv_HDR_ConfigMetadata_MpegVUI(stTFData, &stGMData);
        }


#if HDR_DEBUG_MODE
        if(MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK1A_3F_L, 0xFF)==0)
        {
            MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK1A_3F_L, 0x4 , 0xFF);
            g_u16Hdr3x3Ratio = 8;
        }

        if((MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK1A_3F_L, 0xFF00)>>8)==0)
        {
            MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK1A_3F_L, 0x200 , 0xFF00);
            g_u16HdrY2RRatio = 2;
        }
#endif

        if((g_HDRinitParameters.DLC_HDRMetadataMpegVUI.u8TransferCharacteristics !=_u8PreTransferCharacteristics)
            ||(g_HDRinitParameters.DLC_HDRMetadataMpegVUI.u8ColorPrimaries !=_u8PreColorPrimaries)
#if HDR_DEBUG_MODE
            ||((MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK1A_3F_L, 0xFF00)>>8)!=g_u16HdrY2RRatio)
            ||(MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK1A_3F_L, 0xFF)!=g_u16Hdr3x3Ratio))
#else
            &&(g_HDRinitParameters.u16HDRFunctionSelect & 0x01 ))
#endif

        {
            _u8PreTransferCharacteristics = g_HDRinitParameters.DLC_HDRMetadataMpegVUI.u8TransferCharacteristics;
            _u8PreColorPrimaries = g_HDRinitParameters.DLC_HDRMetadataMpegVUI.u8ColorPrimaries;

#if HDR_DEBUG_MODE
            g_u16Hdr3x3Ratio = MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK1A_3F_L, 0xFF);
            g_u16HdrY2RRatio =(MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK1A_3F_L, 0xFF00)>>8);
#endif

            MDrv_HDR_TransferFunctionGen(stTFData,u32LutData);

            HDR_DBG_HAL(DLC_DEBUG("\n       MDrv_HDR_Main        g_u16Hdr3x3Ratio=%d  ,g_u16HdrY2RRatio=%d \n",g_u16Hdr3x3Ratio,g_u16HdrY2RRatio));
        }

        if(g_HDRinitParameters.u16HDRFunctionSelect & 0x01 )
            MDrv_HDR_GamutMapping3x3Gen(&stGMData);

        // For HDMI HDR
        if(g_HDRinitParameters.u16HDRFunctionSelect & 0x10 )
        {
            MDrv_HDR_ConfigMetadata_HdmiTxInfoFrame( &HDRtmp, stTFData, &stGMData);
            //MDrv_HDR_ConfigMetadata_HdmiRxDataBlock( &HDRtmp, &stTFData, &stMetadata );
            if((_u8Colorimetry != g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.Colorimetry) ||
                (_u8ExtendedColorimetry != g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.ExtendedColorimetry))
            {
                _u8ExtendedColorimetry = g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.ExtendedColorimetry;
                _u8Colorimetry = g_HDRinitParameters.DLC_HDRHdmiTxAviInfoFrame.Colorimetry;
                MDrv_HDR_TransferFunctionGen(stTFData,u32LutData);
            }
            MDrv_HDR_GamutMapping3x3Gen(&stGMData);
        }

    }
    else
    {
        // Write 3*3
        MDrv_HDR_Set3x3Gen(TRUE);

        _u8PreColorPrimaries=0;
        _u8PreTransferCharacteristics=0;
        _u8ExtendedColorimetry = 0;
        _u8Colorimetry = 0;
    }

}

#if defined(CONFIG_ARM64)
void msDlcSetPMBase(MS_U64 u64PMBase)
{
    g_u64MstarPmBase=u64PMBase;
}
#endif

