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
/// file    mhal_dlc.c
/// @brief  MStar GFLIP DDI HAL LEVEL
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _HAL_DLC_C

//=============================================================================
// Include Files
//=============================================================================
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include "mdrv_mstypes.h"
#include "color_format_ip.h"
#include "mhal_dlc.h"
#include "mdrv_xc_st.h"
//#include <linux/kernel.h>   /* printk() */
#define FRAME_BUFFER_SIZE 16
#define TEMP_SOL

#define DBG_LIGHT( row, col, val, mask ) MApi_GFLIP_XC_W2BYTEMSK( 0x133080 + (row*4+col)*2 , val, mask )

typedef struct MemIdx
{
    MS_U8 u8IPMIdx;
    MS_U8 u8OPMIdx;
    MS_U8 u8MaxIdx;
    MS_U8 u8RWDiff;
    MS_S8 u8OPDiff;
}MemIdx;

typedef struct HistStat_
{
    MS_U32 u32IpLumaHist[32];   // histogram >> 8 ; 24 bits -> 16 bits
    MS_U32 u32IpLumaTotalSum;   // 8 bits luma sum
    MS_U32 u32IpLumaTotalCount; // pixel count
}HistStat;

typedef struct DlcRegister_
{
    MS_U8 ucLMaxThreshold;
    MS_U8 ucLMinThreshold;
    MS_U8 ucLMaxCorrection;
    MS_U8 ucLMinCorrection;
    MS_U8 ucRMaxThreshold;
    MS_U8 ucRMinThreshold;
    MS_U8 ucRMaxCorrection;
    MS_U8 ucRMinCorrection;
    MS_U8 ucAllowLoseContrast;

    MS_BOOL bHistLimitToFull;
    MS_BOOL bCurveFullToLimit;
    MS_U8 ucIIRRatio;           // value 0~64 (0x40)

    MS_U8 ucWLevel;
    MS_U8 ucBLevel;
}DlcRegister;

typedef struct DlcStatus_
{
    MS_U32 u32IpLumaHistFull[32];
    MS_U32 u32TotalCount;
    MS_U32 u32HistAccuSum[32];
    MS_U32 u32SumBelowAvg;
    MS_U32 u32SumAboveAvg;
    MS_U16 u16LumaAvg; // 10 bits
    MS_U16 u16YLftAvg; // 10 bits
    MS_U16 u16YRtAvg;  // 10 bits
    MS_U16 u16PrevLumaAvg; // 10 bits

    MS_U16 u16LimitCurve[16];
    MS_U16 u16DlcCurve[16];
    MS_U16 u16PrevDlcCurve[16];

    MS_U32 u32LeftCutScore;
    MS_U32 u32RightCutScore;
    MS_U32 u32SlopRatioL;
    MS_U32 u32SlopRatioH;
    MS_U8 ucSingleBinW;
    MS_U8 ucSceneChangeW;
}DlcStatus;

typedef struct DlcCtrl_
{
    volatile HistStat *pHistStat;
    volatile HistStat *pPrevHistStat;
    DlcStatus dlcStat;
    DlcRegister dlcReg;
}DlcCtrl;

//=============================================================================
// Compile options
//=============================================================================

//=============================================================================
// Debug Macros
//=============================================================================

//=============================================================================
// Macros
//=============================================================================

//=============================================================================
// Local Variables
//=============================================================================

//=============================================================================
// Global Variables
//=============================================================================
extern StuDlc_FinetuneParamaters g_DlcParameters;
static volatile HistStat sHistStat[FRAME_BUFFER_SIZE];
static MemIdx sMemIdx;
//=============================================================================
// Local Function
//=============================================================================

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

#define DLC_CURVE_SIZE   18

static volatile MS_U16 g_u16HdrY2RRatio = 2;
static volatile MS_U16 g_u16Hdr3x3Ratio = 8;

static volatile MS_U8 _u8PreColorPrimaries=0;
static volatile MS_U8 _u8PreTransferCharacteristics=0;

static volatile MS_U8 _u8Colorimetry=0;
static volatile MS_U8 _u8ExtendedColorimetry=0;
static volatile MS_U8 MaxucTmp;
static volatile MS_U16 MaxHistogram32H;
static volatile MS_U16 _u16Count=0;
static volatile MS_U16 u16PreRegs[18]= {0};
static volatile MS_BOOL _bSetGenRead=FALSE;

static volatile MS_BOOL _bSetDLCHDRInit=FALSE;

#if defined(CONFIG_ARM64)
static volatile MS_U64 g_u64MstarPmBase = 0;
#endif

extern MS_BOOL KApi_XC_MLoad_Fire(EN_MLOAD_CLIENT_TYPE _client_type, MS_BOOL bImmeidate);
extern BOOL MHal_XC_FireAutoDownload(EN_KDRV_XC_AUTODOWNLOAD_CLIENT enClient);
extern MS_U8 MHal_XC_GetVersion( void );
extern MS_U8 MHal_XC_ResetADL(void);
extern MS_BOOL _bAdlFired;
extern HDR_CONTENT_LIGHT_MATADATA STU_HDR_METADATA;
//extern Ultra_Black_White STU_UltraBW;

extern void Mhal_Cfd_Main_Predefined_Control_Set(MS_U8 u8Value);

extern STU_CFDAPI_MAIN_CONTROL _stu_CFD_Main_Control;


//=============================================================================
// Local Variables
//=============================================================================
MS_U8 LogPrintfOneTimes(void);
MS_U8 u8log;
//MS_U16 u16tmo_lut[512]={0},u16code_in[512] = {0};

#if DLC_FUNCTION

StuDlc_FinetuneParamaters g_DlcParameters;

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

static volatile MS_S32 g_uwDlcTmoCurve[18];
static volatile MS_S32 g_uwDlcPreTmoCurve[18];
static volatile MS_BOOL g_bDlcTmoIIRStatus = false;

volatile StuDlc_HDRinit g_HDRinitParameters;
static MS_U8 u8HistogramErrFlag = 0;
HDR_ToneMappingData HDRtmp; //YT ForHDR
WORD HDR_Curve[18]; //YT ForHDR
MS_U16 u16MaxCLL,u16MaxFALL;
MS_U32 LUT_IndexDeGamma2PQ[512] =
{
0,12,24,35,45,54,63,71,78,85,92,98,104,110,116,121,
126,131,135,140,144,149,153,157,160,164,168,171,175,178,181,185,
188,191,194,197,200,202,205,208,210,213,215,218,220,223,225,227,
230,232,234,236,238,240,242,244,246,248,250,252,254,256,258,260,
261,263,265,267,268,270,272,273,275,276,278,280,281,283,284,286,
287,288,290,291,293,294,295,297,298,300,301,302,303,305,306,307,
308,310,311,312,313,315,316,317,318,319,320,321,323,324,325,326,
327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,
343,344,345,346,347,348,349,349,350,351,352,353,354,355,356,356,
357,358,359,360,361,361,362,363,364,365,365,366,367,368,369,369,
370,371,372,372,373,374,375,375,376,377,378,378,379,380,380,381,
382,383,383,384,385,385,386,387,387,388,389,389,390,391,391,392,
392,393,394,394,395,396,396,397,397,398,399,399,400,401,401,402,
402,403,403,404,405,405,406,406,407,408,408,409,409,410,410,411,
411,412,412,413,414,414,415,415,416,416,417,417,418,418,419,419,
420,420,421,421,422,422,423,423,424,424,425,425,426,426,427,427,
428,428,429,429,430,430,431,431,432,432,432,433,433,434,434,435,
435,436,436,437,437,437,438,438,439,439,440,440,440,441,441,442,
442,443,443,443,444,444,445,445,445,446,446,447,447,448,448,448,
449,449,450,450,450,451,451,451,452,452,453,453,453,454,454,455,
455,455,456,456,456,457,457,458,458,458,459,459,459,460,460,461,
461,461,462,462,462,463,463,463,464,464,464,465,465,465,466,466,
467,467,467,468,468,468,469,469,469,470,470,470,471,471,471,472,
472,472,473,473,473,473,474,474,474,475,475,475,476,476,476,477,
477,477,478,478,478,479,479,479,479,480,480,480,481,481,481,482,
482,482,482,483,483,483,484,484,484,485,485,485,485,486,486,486,
487,487,487,487,488,488,488,489,489,489,489,490,490,490,491,491,
491,491,492,492,492,492,493,493,493,494,494,494,494,495,495,495,
495,496,496,496,496,497,497,497,498,498,498,498,499,499,499,499,
500,500,500,500,501,501,501,501,502,502,502,502,503,503,503,503,
504,504,504,504,505,505,505,505,506,506,506,506,507,507,507,507,
507,508,508,508,508,509,509,509,509,510,510,510,510,511,511,511
};
MS_U16 LUT_CFD_NITS2PQCODE[145] =
{
    0x0002,
    0x0002,0x0003,0x0003,0x0003,0x0004,0x0004,0x0004,0x0004,0x0005,0x0005,0x0005,0x0005,0x0005,0x0006,0x0006,0x0006,0x0006,0x0006,
    0x0008,0x0009,0x000b,0x000c,0x000d,0x000e,0x000f,0x000f,0x0010,0x0011,0x0012,0x0012,0x0013,0x0014,0x0014,0x0015,0x0015,0x0016,
    0x001b,0x001f,0x0022,0x0025,0x0028,0x002b,0x002d,0x002f,0x0031,0x0033,0x0035,0x0037,0x0038,0x003a,0x003c,0x003d,0x003e,0x0040,
    0x004b,0x0055,0x005d,0x0063,0x0069,0x006f,0x0074,0x0078,0x007c,0x0080,0x0084,0x0088,0x008b,0x008e,0x0091,0x0094,0x0097,0x0099,
    0x00b0,0x00c1,0x00ce,0x00da,0x00e5,0x00ee,0x00f6,0x00fe,0x0104,0x010b,0x0111,0x0117,0x011c,0x0121,0x0126,0x012a,0x012e,0x0133,
    0x0154,0x016d,0x0181,0x0192,0x01a0,0x01ad,0x01b8,0x01c2,0x01cc,0x01d4,0x01dc,0x01e4,0x01eb,0x01f1,0x01f7,0x01fd,0x0203,0x0208,
    0x0232,0x0250,0x0268,0x027c,0x028d,0x029c,0x02a9,0x02b4,0x02bf,0x02c8,0x02d1,0x02d9,0x02e1,0x02e8,0x02ef,0x02f5,0x02fb,0x0301,
    0x032e,0x034e,0x0367,0x037c,0x038d,0x039b,0x03a8,0x03b4,0x03be,0x03c8,0x03d1,0x03d9,0x03e0,0x03e7,0x03ee,0x03f4,0x03fa,0x03ff
};

//defined in color_format_ip.c
//extern MS_U16 u16PQCurveExpLut[705];
//extern MS_U16 u16PQCurveMantLut[705];

#if 1
const MS_U16 u16PQCurveExpLut[705] =
{
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
    0x13
};

const MS_U16 u16PQCurveMantLut[705] =
{
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
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
//test input vector for TMO unit convertion
#endif

MS_U16 test_vector1_Dolby00[512] =
{
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00d0, 0x0230, 0x0300, 0x0390, 0x0410,
    0x0480, 0x04f0, 0x0550, 0x05c0, 0x0620, 0x0690, 0x06f0, 0x0750, 0x07b0, 0x0820, 0x0880, 0x08e1, 0x0941, 0x09a1, 0x0a11, 0x0a71,
    0x0ad1, 0x0b31, 0x0ba1, 0x0c01, 0x0c61, 0x0cd1, 0x0d31, 0x0d91, 0x0e01, 0x0e61, 0x0ed1, 0x0f31, 0x0f91, 0x1001, 0x1061, 0x10d1,
    0x1131, 0x11a1, 0x1211, 0x1271, 0x12e1, 0x1341, 0x13b1, 0x1421, 0x1481, 0x14f1, 0x1561, 0x15c1, 0x1631, 0x16a1, 0x1711, 0x1771,
    0x17e1, 0x1851, 0x18c1, 0x1931, 0x1991, 0x1a02, 0x1a72, 0x1ae2, 0x1b52, 0x1bc2, 0x1c32, 0x1ca2, 0x1d02, 0x1d72, 0x1de2, 0x1e52,
    0x1ec2, 0x1f32, 0x1fa2, 0x2012, 0x2082, 0x20f2, 0x2162, 0x21d2, 0x2242, 0x22b2, 0x2332, 0x23a2, 0x2412, 0x2482, 0x24f2, 0x2562,
    0x25d2, 0x2642, 0x26b2, 0x2732, 0x27a2, 0x2812, 0x2882, 0x28f2, 0x2962, 0x29e2, 0x2a52, 0x2ac3, 0x2b33, 0x2ba3, 0x2c13, 0x2c93,
    0x2d03, 0x2d73, 0x2de3, 0x2e63, 0x2ed3, 0x2f43, 0x2fb3, 0x3033, 0x30a3, 0x3113, 0x3183, 0x3203, 0x3273, 0x32e3, 0x3353, 0x33d3,
    0x3443, 0x34b3, 0x3523, 0x35a3, 0x3613, 0x3683, 0x36f3, 0x3773, 0x37e3, 0x3853, 0x38c3, 0x3943, 0x39b3, 0x3a23, 0x3a93, 0x3b13,
    0x3b83, 0x3bf4, 0x3c64, 0x3ce4, 0x3d54, 0x3dc4, 0x3e34, 0x3eb4, 0x3f24, 0x3f94, 0x4004, 0x4084, 0x40f4, 0x4164, 0x41d4, 0x4244,
    0x42c4, 0x4334, 0x43a4, 0x4414, 0x4484, 0x4504, 0x4574, 0x45e4, 0x4654, 0x46c4, 0x4734, 0x47b4, 0x4824, 0x4894, 0x4904, 0x4974,
    0x49e4, 0x4a54, 0x4ac4, 0x4b34, 0x4ba4, 0x4c14, 0x4c84, 0x4d05, 0x4d75, 0x4de5, 0x4e55, 0x4ec5, 0x4f35, 0x4fa5, 0x5005, 0x5075,
    0x50e5, 0x5155, 0x51c5, 0x5235, 0x52a5, 0x5315, 0x5385, 0x53f5, 0x5455, 0x54c5, 0x5535, 0x55a5, 0x5615, 0x5675, 0x56e5, 0x5755,
    0x57c5, 0x5825, 0x5895, 0x5905, 0x5965, 0x59d5, 0x5a45, 0x5aa5, 0x5b15, 0x5b85, 0x5be5, 0x5c55, 0x5cb5, 0x5d25, 0x5d85, 0x5df6,
    0x5e56, 0x5ec6, 0x5f26, 0x5f96, 0x5ff6, 0x6056, 0x60c6, 0x6126, 0x6186, 0x61f6, 0x6256, 0x62b6, 0x6326, 0x6386, 0x63e6, 0x6446,
    0x64a6, 0x6516, 0x6576, 0x65d6, 0x6636, 0x6696, 0x66f6, 0x6756, 0x67b6, 0x6816, 0x6876, 0x68d6, 0x6936, 0x6996, 0x69f6, 0x6a56,
    0x6ab6, 0x6b16, 0x6b66, 0x6bc6, 0x6c26, 0x6c86, 0x6cd6, 0x6d36, 0x6d96, 0x6de6, 0x6e46, 0x6ea6, 0x6ef7, 0x6f57, 0x6fa7, 0x7007,
    0x7057, 0x70b7, 0x7107, 0x7167, 0x71b7, 0x7217, 0x7267, 0x72b7, 0x7317, 0x7367, 0x73b7, 0x7417, 0x7467, 0x74b7, 0x7507, 0x7557,
    0x75b7, 0x7607, 0x7657, 0x76a7, 0x76f7, 0x7747, 0x7797, 0x77e7, 0x7837, 0x7887, 0x78d7, 0x7927, 0x7977, 0x79b7, 0x7a07, 0x7a57,
    0x7aa7, 0x7af7, 0x7b37, 0x7b87, 0x7bd7, 0x7c17, 0x7c67, 0x7cb7, 0x7cf7, 0x7d47, 0x7d87, 0x7dd7, 0x7e17, 0x7e67, 0x7ea7, 0x7ef7,
    0x7f47, 0x7f77, 0x7fc7, 0x8018, 0x8048, 0x8098, 0x80e8, 0x8128, 0x8158, 0x81a8, 0x81f8, 0x8238, 0x8278, 0x82b8, 0x82f8, 0x8338,
    0x8378, 0x83b8, 0x83f8, 0x8438, 0x8478, 0x84b8, 0x84f8, 0x8538, 0x8578, 0x85b8, 0x85e8, 0x8618, 0x8668, 0x86a8, 0x86e8, 0x8718,
    0x8748, 0x8798, 0x87c8, 0x8808, 0x8838, 0x8868, 0x88b8, 0x88e8, 0x8928, 0x8958, 0x8998, 0x89c8, 0x8a08, 0x8a38, 0x8a68, 0x8aa8,
    0x8ad8, 0x8b08, 0x8b48, 0x8b78, 0x8ba8, 0x8be8, 0x8c18, 0x8c48, 0x8c78, 0x8ca8, 0x8cd8, 0x8d18, 0x8d48, 0x8d78, 0x8da8, 0x8dd8,
    0x8e08, 0x8e38, 0x8e68, 0x8e98, 0x8ec8, 0x8ef8, 0x8f28, 0x8f58, 0x8f88, 0x8fb8, 0x8fd8, 0x9008, 0x9038, 0x9068, 0x9098, 0x90b8,
    0x90e8, 0x9119, 0x9149, 0x9169, 0x9199, 0x91c9, 0x91e9, 0x9219, 0x9249, 0x9269, 0x9299, 0x92b9, 0x92e9, 0x9309, 0x9339, 0x9359,
    0x9389, 0x93a9, 0x93d9, 0x93f9, 0x9429, 0x9449, 0x9469, 0x9499, 0x94b9, 0x94d9, 0x9509, 0x9529, 0x9549, 0x9579, 0x9599, 0x95b9,
    0x95d9, 0x9609, 0x9629, 0x9649, 0x9669, 0x9689, 0x96a9, 0x96d9, 0x96f9, 0x9719, 0x9739, 0x9759, 0x9779, 0x9799, 0x97b9, 0x97d9,
    0x97f9, 0x9819, 0x9839, 0x9859, 0x9879, 0x9899, 0x98b9, 0x98d9, 0x98f9, 0x9919, 0x9939, 0x9949, 0x9969, 0x9989, 0x99a9, 0x99c9,
    0x99e9, 0x99f9, 0x9a19, 0x9a39, 0x9a59, 0x9a69, 0x9a89, 0x9aa9, 0x9ac9, 0x9ad9, 0x9af9, 0x9b19, 0x9b29, 0x9b49, 0x9b69, 0x9b79,
    0x9b99, 0x9bb9, 0x9bc9, 0x9be9, 0x9bf9, 0x9c19, 0x9c29, 0x9c49, 0x9c69, 0x9c79, 0x9c99, 0x9ca9, 0x9cc9, 0x9cd9, 0x9cf9, 0x9d09,
    0x9d29, 0x9d39, 0x9d49, 0x9d69, 0x9d79, 0x9d99, 0x9da9, 0x9dc9, 0x9dd9, 0x9de9, 0x9e09, 0x9e19, 0x9e29, 0x9e49, 0x9e59, 0x9e59
};

MS_U16 test_vector2_Dolby17[512] =
{
    0x03e0, 0x03e0, 0x03e0, 0x03f0, 0x0400, 0x0410, 0x0430, 0x0460, 0x0490, 0x04c0, 0x0500, 0x0550, 0x05b0, 0x0600, 0x0660, 0x06d0,
    0x0730, 0x07a0, 0x0810, 0x0880, 0x08f1, 0x0971, 0x09e1, 0x0a61, 0x0ad1, 0x0b51, 0x0bd1, 0x0c51, 0x0cd1, 0x0d41, 0x0dc1, 0x0e41,
    0x0ed1, 0x0f51, 0x0fd1, 0x1051, 0x10d1, 0x1161, 0x11e1, 0x1261, 0x12f1, 0x1371, 0x13f1, 0x1481, 0x1501, 0x1591, 0x1611, 0x16a1,
    0x1731, 0x17b1, 0x1841, 0x18c1, 0x1951, 0x19e2, 0x1a62, 0x1af2, 0x1b82, 0x1c02, 0x1c92, 0x1d22, 0x1da2, 0x1e32, 0x1ec2, 0x1f42,
    0x1fd2, 0x2062, 0x20e2, 0x2172, 0x2202, 0x2292, 0x2312, 0x23a2, 0x2432, 0x24b2, 0x2542, 0x25c2, 0x2652, 0x26e2, 0x2762, 0x27f2,
    0x2872, 0x2902, 0x2982, 0x2a12, 0x2a92, 0x2b23, 0x2ba3, 0x2c33, 0x2cb3, 0x2d33, 0x2dc3, 0x2e43, 0x2ec3, 0x2f43, 0x2fd3, 0x3053,
    0x30d3, 0x3153, 0x31d3, 0x3253, 0x32d3, 0x3353, 0x33d3, 0x3453, 0x34d3, 0x3543, 0x35c3, 0x3643, 0x36b3, 0x3733, 0x37a3, 0x3823,
    0x3893, 0x3913, 0x3983, 0x39f3, 0x3a73, 0x3ae3, 0x3b53, 0x3bc4, 0x3c34, 0x3ca4, 0x3d14, 0x3d84, 0x3df4, 0x3e54, 0x3ec4, 0x3f34,
    0x3f94, 0x4004, 0x4064, 0x40d4, 0x4134, 0x4194, 0x41f4, 0x4264, 0x42c4, 0x4324, 0x4384, 0x43e4, 0x4444, 0x4494, 0x44f4, 0x4554,
    0x45a4, 0x4604, 0x4664, 0x46b4, 0x4704, 0x4764, 0x47b4, 0x4804, 0x4854, 0x48b4, 0x4904, 0x4954, 0x49a4, 0x49e4, 0x4a34, 0x4a84,
    0x4ad4, 0x4b14, 0x4b64, 0x4ba4, 0x4bf4, 0x4c34, 0x4c84, 0x4cd5, 0x4d05, 0x4d55, 0x4da5, 0x4de5, 0x4e25, 0x4e65, 0x4ea5, 0x4ee5,
    0x4f25, 0x4f55, 0x4f85, 0x4fd5, 0x5005, 0x5035, 0x5085, 0x50b5, 0x50f5, 0x5125, 0x5155, 0x5195, 0x51c5, 0x51f5, 0x5225, 0x5255,
    0x5295, 0x52c5, 0x52f5, 0x5325, 0x5355, 0x5375, 0x53a5, 0x53d5, 0x5405, 0x5435, 0x5455, 0x5485, 0x54b5, 0x54d5, 0x5505, 0x5525,
    0x5555, 0x5575, 0x55a5, 0x55c5, 0x55e5, 0x5615, 0x5635, 0x5655, 0x5675, 0x56a5, 0x56c5, 0x56e5, 0x5705, 0x5725, 0x5745, 0x5765,
    0x5785, 0x57a5, 0x57c5, 0x57e5, 0x5805, 0x5825, 0x5835, 0x5855, 0x5875, 0x5895, 0x58b5, 0x58c5, 0x58e5, 0x5905, 0x5915, 0x5935,
    0x5945, 0x5965, 0x5975, 0x5995, 0x59a5, 0x59c5, 0x59d5, 0x59f5, 0x5a05, 0x5a25, 0x5a35, 0x5a45, 0x5a65, 0x5a75, 0x5a85, 0x5aa5,
    0x5ab5, 0x5ac5, 0x5ad5, 0x5af5, 0x5b05, 0x5b15, 0x5b25, 0x5b35, 0x5b45, 0x5b55, 0x5b75, 0x5b85, 0x5b95, 0x5ba5, 0x5bb5, 0x5bc5,
    0x5bd5, 0x5be5, 0x5bf5, 0x5c05, 0x5c15, 0x5c25, 0x5c35, 0x5c45, 0x5c45, 0x5c55, 0x5c65, 0x5c75, 0x5c85, 0x5c95, 0x5ca5, 0x5cb5,
    0x5cb5, 0x5cc5, 0x5cd5, 0x5ce5, 0x5cf5, 0x5cf5, 0x5d05, 0x5d15, 0x5d25, 0x5d25, 0x5d35, 0x5d45, 0x5d45, 0x5d55, 0x5d65, 0x5d65,
    0x5d75, 0x5d85, 0x5d85, 0x5d95, 0x5da5, 0x5da5, 0x5db5, 0x5dc5, 0x5dc5, 0x5dd5, 0x5dd5, 0x5de6, 0x5de6, 0x5df6, 0x5e06, 0x5e06,
    0x5e16, 0x5e16, 0x5e26, 0x5e26, 0x5e36, 0x5e36, 0x5e46, 0x5e46, 0x5e56, 0x5e56, 0x5e66, 0x5e66, 0x5e76, 0x5e76, 0x5e86, 0x5e86,
    0x5e86, 0x5e96, 0x5e96, 0x5ea6, 0x5ea6, 0x5eb6, 0x5eb6, 0x5eb6, 0x5ec6, 0x5ec6, 0x5ed6, 0x5ed6, 0x5ed6, 0x5ee6, 0x5ee6, 0x5ef6,
    0x5ef6, 0x5ef6, 0x5f06, 0x5f06, 0x5f06, 0x5f16, 0x5f16, 0x5f16, 0x5f26, 0x5f26, 0x5f26, 0x5f36, 0x5f36, 0x5f36, 0x5f46, 0x5f46,
    0x5f46, 0x5f56, 0x5f56, 0x5f56, 0x5f66, 0x5f66, 0x5f66, 0x5f66, 0x5f76, 0x5f76, 0x5f76, 0x5f76, 0x5f86, 0x5f86, 0x5f86, 0x5f96,
    0x5f96, 0x5f96, 0x5f96, 0x5fa6, 0x5fa6, 0x5fa6, 0x5fa6, 0x5fb6, 0x5fb6, 0x5fb6, 0x5fb6, 0x5fb6, 0x5fc6, 0x5fc6, 0x5fc6, 0x5fc6,
    0x5fd6, 0x5fd6, 0x5fd6, 0x5fd6, 0x5fd6, 0x5fe6, 0x5fe6, 0x5fe6, 0x5fe6, 0x5fe6, 0x5ff6, 0x5ff6, 0x5ff6, 0x5ff6, 0x5ff6, 0x6006,
    0x6006, 0x6006, 0x6006, 0x6006, 0x6006, 0x6016, 0x6016, 0x6016, 0x6016, 0x6016, 0x6016, 0x6026, 0x6026, 0x6026, 0x6026, 0x6026,
    0x6026, 0x6036, 0x6036, 0x6036, 0x6036, 0x6036, 0x6036, 0x6036, 0x6046, 0x6046, 0x6046, 0x6046, 0x6046, 0x6046, 0x6046, 0x6056,
    0x6056, 0x6056, 0x6056, 0x6056, 0x6056, 0x6056, 0x6066, 0x6066, 0x6066, 0x6066, 0x6066, 0x6066, 0x6066, 0x6066, 0x6066, 0x6076,
    0x6076, 0x6076, 0x6076, 0x6076, 0x6076, 0x6076, 0x6076, 0x6076, 0x6086, 0x6086, 0x6086, 0x6086, 0x6086, 0x6086, 0x6086, 0x6086,
    0x6086, 0x6096, 0x6096, 0x6096, 0x6096, 0x6096, 0x6096, 0x6096, 0x6096, 0x6096, 0x6096, 0x6096, 0x60a6, 0x60a6, 0x60a6, 0x60a6,
    0x60a6, 0x60a6, 0x60a6, 0x60a6, 0x60a6, 0x60a6, 0x60a6, 0x60a6, 0x60b6, 0x60b6, 0x60b6, 0x60b6, 0x60b6, 0x60b6, 0x60b6, 0x60b6
};

MS_U16 test_vector3[512] =
{
    0x0000, 0x00e0, 0x01c1, 0x02a1, 0x0382, 0x0462, 0x0543, 0x0623, 0x0703, 0x07e4, 0x08c4, 0x09a5, 0x0a85, 0x0b66, 0x0c46, 0x0d27,
    0x0e07, 0x0ee0, 0x0fb8, 0x1090, 0x1169, 0x1241, 0x131a, 0x13f2, 0x14cb, 0x15a3, 0x167b, 0x1754, 0x182c, 0x1905, 0x19dd, 0x1ab6,
    0x1b8e, 0x1c66, 0x1d3f, 0x1e17, 0x1ef0, 0x1fc8, 0x20a0, 0x2179, 0x2251, 0x232a, 0x2402, 0x24db, 0x25b3, 0x268b, 0x2764, 0x283c,
    0x2915, 0x29a4, 0x2a2d, 0x2ab5, 0x2b3d, 0x2bc5, 0x2c4e, 0x2cd6, 0x2d5e, 0x2de7, 0x2e6f, 0x2ef7, 0x2f7f, 0x3008, 0x3090, 0x3118,
    0x31a0, 0x3229, 0x32b1, 0x3339, 0x33c1, 0x344a, 0x34d2, 0x355a, 0x35e3, 0x366b, 0x36f3, 0x377b, 0x3804, 0x388c, 0x3914, 0x399c,
    0x3a25, 0x3a92, 0x3afa, 0x3b62, 0x3bca, 0x3c33, 0x3c9b, 0x3d03, 0x3d6b, 0x3dd3, 0x3e3c, 0x3ea4, 0x3f0c, 0x3f74, 0x3fdc, 0x4045,
    0x40ad, 0x4115, 0x417d, 0x41e5, 0x424e, 0x42b6, 0x431e, 0x4386, 0x43ef, 0x4457, 0x44bf, 0x4527, 0x458f, 0x45f8, 0x4660, 0x46c8,
    0x4730, 0x4786, 0x47d6, 0x4826, 0x4876, 0x48c6, 0x4916, 0x4966, 0x49b7, 0x4a07, 0x4a57, 0x4aa7, 0x4af7, 0x4b47, 0x4b98, 0x4be8,
    0x4c38, 0x4c88, 0x4cd8, 0x4d28, 0x4d79, 0x4dc9, 0x4e19, 0x4e69, 0x4eb9, 0x4f09, 0x4f59, 0x4faa, 0x4ffa, 0x504a, 0x509a, 0x50ea,
    0x513a, 0x517f, 0x51bf, 0x51ff, 0x523f, 0x5280, 0x52c0, 0x5300, 0x5340, 0x5380, 0x53c0, 0x5400, 0x5440, 0x5481, 0x54c1, 0x5501,
    0x5541, 0x5581, 0x55c1, 0x5601, 0x5641, 0x5682, 0x56c2, 0x5702, 0x5742, 0x5782, 0x57c2, 0x5802, 0x5842, 0x5882, 0x58c3, 0x5903,
    0x5943, 0x597e, 0x59b6, 0x59ee, 0x5a26, 0x5a5e, 0x5a96, 0x5ace, 0x5b07, 0x5b3f, 0x5b77, 0x5baf, 0x5be7, 0x5c1f, 0x5c57, 0x5c8f,
    0x5cc7, 0x5cff, 0x5d38, 0x5d70, 0x5da8, 0x5de0, 0x5e18, 0x5e50, 0x5e88, 0x5ec0, 0x5ef8, 0x5f31, 0x5f69, 0x5fa1, 0x5fd9, 0x6011,
    0x6049, 0x607c, 0x60ad, 0x60dd, 0x610d, 0x613d, 0x616d, 0x619d, 0x61cd, 0x61fd, 0x622d, 0x625d, 0x628e, 0x62be, 0x62ee, 0x631e,
    0x634e, 0x637e, 0x63ae, 0x63de, 0x640e, 0x643e, 0x646e, 0x649f, 0x64cf, 0x64ff, 0x652f, 0x655f, 0x658f, 0x65bf, 0x65ef, 0x661f,
    0x664f, 0x667f, 0x66b0, 0x66e0, 0x6710, 0x6740, 0x6770, 0x67a0, 0x67d0, 0x6800, 0x6830, 0x6860, 0x6890, 0x68c1, 0x68f1, 0x6921,
    0x6951, 0x6981, 0x69b1, 0x69e1, 0x6a11, 0x6a41, 0x6a71, 0x6aa2, 0x6ad2, 0x6b02, 0x6b32, 0x6b62, 0x6b92, 0x6bc2, 0x6bf2, 0x6c22,
    0x6c52, 0x6c7b, 0x6c9b, 0x6cbb, 0x6cdb, 0x6cfb, 0x6d1b, 0x6d3b, 0x6d5b, 0x6d7b, 0x6d9c, 0x6dbc, 0x6ddc, 0x6dfc, 0x6e1c, 0x6e3c,
    0x6e5c, 0x6e7c, 0x6e9c, 0x6ebc, 0x6edc, 0x6efc, 0x6f1c, 0x6f3c, 0x6f5c, 0x6f7c, 0x6f9d, 0x6fbd, 0x6fdd, 0x6ffd, 0x701d, 0x703d,
    0x705d, 0x707d, 0x709d, 0x70bd, 0x70dd, 0x70fd, 0x711d, 0x713d, 0x715d, 0x717d, 0x719e, 0x71be, 0x71de, 0x71fe, 0x721e, 0x723e,
    0x725e, 0x727e, 0x729e, 0x72be, 0x72de, 0x72fe, 0x731e, 0x733e, 0x735e, 0x737e, 0x739f, 0x73bf, 0x73df, 0x73ff, 0x741f, 0x743f,
    0x745f, 0x747f, 0x749f, 0x74bf, 0x74df, 0x74ff, 0x751f, 0x753f, 0x755f, 0x757f, 0x759f, 0x75c0, 0x75e0, 0x7600, 0x7620, 0x7640,
    0x7660, 0x7680, 0x76a0, 0x76c0, 0x76e0, 0x7700, 0x7720, 0x7740, 0x7760, 0x7780, 0x77a0, 0x77c1, 0x77e1, 0x7801, 0x7821, 0x7841,
    0x7861, 0x7881, 0x78a1, 0x78c1, 0x78e1, 0x7901, 0x7921, 0x7941, 0x7961, 0x7981, 0x79a1, 0x79c2, 0x79e2, 0x7a02, 0x7a22, 0x7a42,
    0x7a62, 0x7a82, 0x7aa2, 0x7ac2, 0x7ae2, 0x7b02, 0x7b22, 0x7b42, 0x7b62, 0x7b82, 0x7ba2, 0x7bc3, 0x7be3, 0x7c03, 0x7c23, 0x7c43,
    0x7c63, 0x7c81, 0x7c99, 0x7cb1, 0x7cc9, 0x7ce1, 0x7cf9, 0x7d11, 0x7d2a, 0x7d42, 0x7d5a, 0x7d72, 0x7d8a, 0x7da2, 0x7dba, 0x7dd2,
    0x7dea, 0x7e02, 0x7e1a, 0x7e32, 0x7e4a, 0x7e62, 0x7e7a, 0x7e92, 0x7eaa, 0x7ec2, 0x7eda, 0x7ef2, 0x7f0a, 0x7f22, 0x7f3b, 0x7f53,
    0x7f6b, 0x7f83, 0x7f9b, 0x7fb3, 0x7fcb, 0x7fe3, 0x7ffb, 0x8013, 0x802b, 0x8043, 0x805b, 0x8073, 0x808b, 0x80a3, 0x80bb, 0x80d3,
    0x80eb, 0x8103, 0x811b, 0x8134, 0x814c, 0x8164, 0x817c, 0x8194, 0x81ac, 0x81c4, 0x81dc, 0x81f4, 0x820c, 0x8224, 0x823c, 0x8254,
    0x826c, 0x8284, 0x829c, 0x82b4, 0x82cc, 0x82e4, 0x82fc, 0x8314, 0x832c, 0x8345, 0x835d, 0x8375, 0x838d, 0x83a5, 0x83bd, 0x83d5,
    0x83ed, 0x8405, 0x841d, 0x8435, 0x844d, 0x8465, 0x847d, 0x8495, 0x84ad, 0x84c5, 0x84dd, 0x84f5, 0x850d, 0x8525, 0x853e, 0x8556,
    0x856e, 0x8586, 0x8598, 0x85aa, 0x85bc, 0x85cf, 0x85e1, 0x85f3, 0x8606, 0x8618, 0x862a, 0x863d, 0x864f, 0x8661, 0x8674, 0x8686
};

MS_U16 test_vector4[512] =
{
    0x0000, 0x00d0, 0x01a1, 0x0271, 0x0342, 0x0412, 0x04e2, 0x05b3, 0x0683, 0x0754, 0x0824, 0x08f4, 0x09c5, 0x0a95, 0x0b66, 0x0c36,
    0x0d06, 0x0ddf, 0x0eb7, 0x0f8f, 0x1068, 0x1140, 0x1219, 0x12f1, 0x13ca, 0x14a2, 0x157a, 0x1653, 0x172b, 0x1804, 0x18dc, 0x19b5,
    0x1a8d, 0x1b65, 0x1c3e, 0x1d16, 0x1def, 0x1ec7, 0x1f9f, 0x2078, 0x2150, 0x2229, 0x2301, 0x23da, 0x24b2, 0x258a, 0x2663, 0x273b,
    0x2814, 0x28b9, 0x295a, 0x29fa, 0x2a9a, 0x2b3a, 0x2bdb, 0x2c7b, 0x2d1b, 0x2dbc, 0x2e5c, 0x2efc, 0x2f9d, 0x303d, 0x30dd, 0x317e,
    0x321e, 0x32be, 0x335f, 0x33ff, 0x349f, 0x353f, 0x35e0, 0x3680, 0x3720, 0x37c1, 0x3861, 0x3901, 0x39a2, 0x3a42, 0x3ae2, 0x3b83,
    0x3c23, 0x3caf, 0x3d37, 0x3dbf, 0x3e48, 0x3ed0, 0x3f58, 0x3fe0, 0x4069, 0x40f1, 0x4179, 0x4202, 0x428a, 0x4312, 0x439a, 0x4423,
    0x44ab, 0x4533, 0x45bb, 0x4644, 0x46cc, 0x4754, 0x47dc, 0x4865, 0x48ed, 0x4975, 0x49fd, 0x4a86, 0x4b0e, 0x4b96, 0x4c1f, 0x4ca7,
    0x4d2f, 0x4dab, 0x4e23, 0x4e9b, 0x4f14, 0x4f8c, 0x5004, 0x507c, 0x50f4, 0x516d, 0x51e5, 0x525d, 0x52d5, 0x534e, 0x53c6, 0x543e,
    0x54b6, 0x552f, 0x55a7, 0x561f, 0x5697, 0x570f, 0x5788, 0x5800, 0x5878, 0x58f0, 0x5969, 0x59e1, 0x5a59, 0x5ad1, 0x5b4a, 0x5bc2,
    0x5c3a, 0x5ca7, 0x5d0f, 0x5d77, 0x5ddf, 0x5e48, 0x5eb0, 0x5f18, 0x5f80, 0x5fe8, 0x6051, 0x60b9, 0x6121, 0x6189, 0x61f1, 0x625a,
    0x62c2, 0x632a, 0x6392, 0x63fa, 0x6463, 0x64cb, 0x6533, 0x659b, 0x6603, 0x666c, 0x66d4, 0x673c, 0x67a4, 0x680c, 0x6875, 0x68dd,
    0x6945, 0x69a3, 0x69fb, 0x6a53, 0x6aab, 0x6b03, 0x6b5c, 0x6bb4, 0x6c0c, 0x6c64, 0x6cbc, 0x6d14, 0x6d6d, 0x6dc5, 0x6e1d, 0x6e75,
    0x6ecd, 0x6f25, 0x6f7e, 0x6fd6, 0x702e, 0x7086, 0x70de, 0x7137, 0x718f, 0x71e7, 0x723f, 0x7297, 0x72ef, 0x7348, 0x73a0, 0x73f8,
    0x7450, 0x74a3, 0x74f4, 0x7544, 0x7594, 0x75e4, 0x7634, 0x7684, 0x76d5, 0x7725, 0x7775, 0x77c5, 0x7815, 0x7865, 0x78b5, 0x7906,
    0x7956, 0x79a6, 0x79f6, 0x7a46, 0x7a96, 0x7ae7, 0x7b37, 0x7b87, 0x7bd7, 0x7c27, 0x7c77, 0x7cc8, 0x7d18, 0x7d68, 0x7db8, 0x7e08,
    0x7e58, 0x7ea4, 0x7eec, 0x7f34, 0x7f7d, 0x7fc5, 0x800d, 0x8055, 0x809d, 0x80e5, 0x812d, 0x8176, 0x81be, 0x8206, 0x824e, 0x8296,
    0x82de, 0x8326, 0x836f, 0x83b7, 0x83ff, 0x8447, 0x848f, 0x84d7, 0x851f, 0x8568, 0x85b0, 0x85f8, 0x8640, 0x8688, 0x86d0, 0x8718,
    0x8761, 0x87a9, 0x87f1, 0x8839, 0x8881, 0x88c9, 0x8911, 0x895a, 0x89a2, 0x89ea, 0x8a32, 0x8a7a, 0x8ac2, 0x8b0a, 0x8b52, 0x8b9b,
    0x8be3, 0x8c2b, 0x8c73, 0x8cbb, 0x8d03, 0x8d4b, 0x8d94, 0x8ddc, 0x8e24, 0x8e6c, 0x8eb4, 0x8efc, 0x8f44, 0x8f8d, 0x8fd5, 0x901d,
    0x9065, 0x90aa, 0x90ea, 0x912a, 0x916a, 0x91aa, 0x91eb, 0x922b, 0x926b, 0x92ab, 0x92eb, 0x932b, 0x936b, 0x93ab, 0x93ec, 0x942c,
    0x946c, 0x94ac, 0x94ec, 0x952c, 0x956c, 0x95ac, 0x95ed, 0x962d, 0x966d, 0x96ad, 0x96ed, 0x972d, 0x976d, 0x97ad, 0x97ee, 0x982e,
    0x986e, 0x98ab, 0x98e3, 0x991b, 0x9953, 0x998c, 0x99c4, 0x99fc, 0x9a34, 0x9a6c, 0x9aa4, 0x9adc, 0x9b14, 0x9b4c, 0x9b85, 0x9bbd,
    0x9bf5, 0x9c2d, 0x9c65, 0x9c9d, 0x9cd5, 0x9d0d, 0x9d45, 0x9d7e, 0x9db6, 0x9dee, 0x9e26, 0x9e5e, 0x9e96, 0x9ece, 0x9f06, 0x9f3e,
    0x9f77, 0x9faf, 0x9fe7, 0xa01f, 0xa057, 0xa08f, 0xa0c7, 0xa0ff, 0xa137, 0xa16f, 0xa1a8, 0xa1e0, 0xa218, 0xa250, 0xa288, 0xa2c0,
    0xa2f8, 0xa330, 0xa368, 0xa3a1, 0xa3d9, 0xa411, 0xa449, 0xa481, 0xa4b9, 0xa4f1, 0xa529, 0xa561, 0xa59a, 0xa5d2, 0xa60a, 0xa642,
    0xa67a, 0xa6b0, 0xa6e0, 0xa711, 0xa741, 0xa771, 0xa7a1, 0xa7d1, 0xa801, 0xa831, 0xa861, 0xa891, 0xa8c1, 0xa8f1, 0xa922, 0xa952,
    0xa982, 0xa9b2, 0xa9e2, 0xaa12, 0xaa42, 0xaa72, 0xaaa2, 0xaad2, 0xab03, 0xab33, 0xab63, 0xab93, 0xabc3, 0xabf3, 0xac23, 0xac53,
    0xac83, 0xacb3, 0xace3, 0xad14, 0xad44, 0xad74, 0xada4, 0xadd4, 0xae04, 0xae34, 0xae64, 0xae94, 0xaec4, 0xaef4, 0xaf25, 0xaf55,
    0xaf85, 0xafb5, 0xafe5, 0xb015, 0xb045, 0xb075, 0xb0a5, 0xb0d5, 0xb105, 0xb136, 0xb166, 0xb196, 0xb1c6, 0xb1f6, 0xb226, 0xb256,
    0xb286, 0xb2b6, 0xb2e6, 0xb317, 0xb347, 0xb377, 0xb3a7, 0xb3d7, 0xb407, 0xb437, 0xb467, 0xb497, 0xb4c7, 0xb4f7, 0xb528, 0xb558,
    0xb588, 0xb5b8, 0xb5e8, 0xb618, 0xb648, 0xb678, 0xb6a8, 0xb6d8, 0xb708, 0xb739, 0xb769, 0xb799, 0xb7c9, 0xb7f9, 0xb829, 0xb859,
    0xb889, 0xb8b9, 0xb8de, 0xb902, 0xb927, 0xb94c, 0xb970, 0xb995, 0xb9ba, 0xb9de, 0xba03, 0xba27, 0xba4c, 0xba71, 0xba95, 0xbaba
};

MS_U16 test_vector5[512] =
{
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0047, 0x00f4, 0x01a1, 0x024e, 0x02fb, 0x03a7, 0x0454, 0x0501, 0x05ae, 0x065a,
    0x0707, 0x07b4, 0x0861, 0x090d, 0x09ba, 0x0a67, 0x0b14, 0x0c6c, 0x0d1c, 0x0dea, 0x0ed6, 0x0fe0, 0x1108, 0x124d, 0x13a7, 0x1500,
    0x165a, 0x17b4, 0x190d, 0x1a67, 0x1bc0, 0x1d1a, 0x1e73, 0x1fcd, 0x2126, 0x2280, 0x23d9, 0x2533, 0x268c, 0x27e6, 0x2940, 0x2a99,
    0x2bf3, 0x2d4c, 0x2ea6, 0x2fff, 0x3159, 0x32b2, 0x335f, 0x340c, 0x34b9, 0x3565, 0x3612, 0x36bf, 0x376c, 0x3818, 0x38c5, 0x3972,
    0x3a1f, 0x3acb, 0x3b78, 0x3c25, 0x3cd2, 0x3d7f, 0x3e2b, 0x3ed8, 0x3f85, 0x4032, 0x40de, 0x418b, 0x4238, 0x42e5, 0x4391, 0x443e,
    0x44eb, 0x4598, 0x4645, 0x46f1, 0x479e, 0x484b, 0x48f8, 0x49a4, 0x4a51, 0x4afe, 0x4bab, 0x4c57, 0x4d04, 0x4db1, 0x4e5e, 0x4f0b,
    0x4fb7, 0x5064, 0x5111, 0x51be, 0x526a, 0x5317, 0x53c4, 0x5471, 0x5519, 0x55bb, 0x5658, 0x56ee, 0x577e, 0x5809, 0x588d, 0x590b,
    0x5983, 0x59f6, 0x5a62, 0x5ac8, 0x5b28, 0x5b83, 0x5bd9, 0x5c2f, 0x5c86, 0x5cdc, 0x5d33, 0x5d89, 0x5ddf, 0x5e36, 0x5e8c, 0x5ee3,
    0x5f39, 0x5f8f, 0x5fe6, 0x603c, 0x6092, 0x60e9, 0x613f, 0x6196, 0x61ec, 0x6242, 0x6299, 0x62ef, 0x6346, 0x639c, 0x63f2, 0x6449,
    0x649f, 0x64f5, 0x654c, 0x65a2, 0x65f9, 0x664f, 0x66a5, 0x66fc, 0x6752, 0x67a9, 0x67ff, 0x6855, 0x68ac, 0x6902, 0x6958, 0x69af,
    0x6a05, 0x6a5c, 0x6ab2, 0x6b08, 0x6b5f, 0x6bb5, 0x6c0c, 0x6c62, 0x6cb8, 0x6d0f, 0x6d65, 0x6dbb, 0x6e12, 0x6e68, 0x6ebf, 0x6f15,
    0x6f6b, 0x6fc2, 0x7018, 0x706f, 0x70c5, 0x711b, 0x7172, 0x71c8, 0x721e, 0x7275, 0x72cb, 0x7322, 0x7378, 0x73ce, 0x7425, 0x747b,
    0x74d1, 0x7528, 0x757e, 0x75d5, 0x762b, 0x7681, 0x76d8, 0x772e, 0x7785, 0x77db, 0x7831, 0x7888, 0x78de, 0x7934, 0x798b, 0x79e1,
    0x7a38, 0x7a8e, 0x7ae4, 0x7b3b, 0x7b91, 0x7be8, 0x7c3e, 0x7c94, 0x7ceb, 0x7d41, 0x7d97, 0x7dee, 0x7e44, 0x7e9b, 0x7ef1, 0x7f47,
    0x7f9e, 0x7ff4, 0x804b, 0x80a1, 0x80f7, 0x814e, 0x81a4, 0x81fa, 0x8251, 0x82a7, 0x82fe, 0x8354, 0x83aa, 0x8401, 0x8457, 0x84ae,
    0x8504, 0x855a, 0x85b1, 0x8607, 0x865d, 0x86b4, 0x870a, 0x8761, 0x87b7, 0x880d, 0x8864, 0x88ba, 0x8911, 0x8967, 0x89bd, 0x8a14,
    0x8a6a, 0x8ac0, 0x8b17, 0x8b6d, 0x8bc4, 0x8c1a, 0x8c70, 0x8cc7, 0x8d1d, 0x8d74, 0x8dca, 0x8e20, 0x8e77, 0x8ecd, 0x8f23, 0x8f7a,
    0x8fd0, 0x9027, 0x907d, 0x90d3, 0x912a, 0x9180, 0x91d7, 0x922d, 0x9283, 0x92da, 0x9330, 0x9386, 0x93dd, 0x9433, 0x948a, 0x94e0,
    0x9536, 0x958d, 0x95e3, 0x963a, 0x9690, 0x96e6, 0x973d, 0x9793, 0x97e9, 0x9840, 0x9896, 0x98ed, 0x9943, 0x9999, 0x99f0, 0x9a46,
    0x9a9d, 0x9af3, 0x9b49, 0x9ba0, 0x9bf6, 0x9c4c, 0x9ca3, 0x9cf9, 0x9d50, 0x9da6, 0x9dfc, 0x9e53, 0x9ea9, 0x9f00, 0x9f56, 0x9fac,
    0xa003, 0xa059, 0xa0af, 0xa106, 0xa15c, 0xa1b3, 0xa209, 0xa25f, 0xa2b6, 0xa30c, 0xa362, 0xa3b9, 0xa40f, 0xa466, 0xa4bc, 0xa512,
    0xa569, 0xa5bf, 0xa616, 0xa66c, 0xa6c2, 0xa719, 0xa76f, 0xa7c5, 0xa81c, 0xa872, 0xa8c9, 0xa91f, 0xa975, 0xa9cc, 0xaa22, 0xaa79,
    0xaacf, 0xab25, 0xab7c, 0xabd2, 0xac28, 0xac7f, 0xacd5, 0xad2c, 0xad82, 0xadd8, 0xae2f, 0xae85, 0xaedc, 0xaf32, 0xaf88, 0xafdf,
    0xb035, 0xb08b, 0xb0e2, 0xb138, 0xb18f, 0xb1e5, 0xb23b, 0xb292, 0xb2e8, 0xb33f, 0xb395, 0xb3eb, 0xb442, 0xb498, 0xb4ee, 0xb545,
    0xb59b, 0xb5f2, 0xb648, 0xb69e, 0xb6f5, 0xb74b, 0xb7a2, 0xb7f8, 0xb84e, 0xb8a5, 0xb8fb, 0xb951, 0xb9a8, 0xb9fe, 0xba55, 0xbaab,
    0xbb01, 0xbb58, 0xbbae, 0xbc05, 0xbc5b, 0xbcb1, 0xbd08, 0xbd5e, 0xbdb4, 0xbe0b, 0xbe61, 0xbeb8, 0xbf0e, 0xbf64, 0xbfbb, 0xc011,
    0xc068, 0xc0be, 0xc114, 0xc16b, 0xc1c1, 0xc217, 0xc26e, 0xc2c4, 0xc31b, 0xc371, 0xc3c7, 0xc41e, 0xc474, 0xc4cb, 0xc521, 0xc577,
    0xc5ce, 0xc624, 0xc67a, 0xc6d1, 0xc727, 0xc77e, 0xc7d4, 0xc82a, 0xc881, 0xc8d7, 0xc92e, 0xc984, 0xc9da, 0xca31, 0xca87, 0xcadd,
    0xcb34, 0xcb8a, 0xcbe1, 0xcc37, 0xcc8d, 0xcce4, 0xcd3a, 0xcd90, 0xcde7, 0xce3d, 0xce94, 0xceea, 0xcf40, 0xcf97, 0xcfed, 0xd044,
    0xd09a, 0xd0f0, 0xd147, 0xd19d, 0xd1f3, 0xd24a, 0xd2a0, 0xd2f7, 0xd34d, 0xd3a3, 0xd3fa, 0xd450, 0xd4a7, 0xd4fd, 0xd553, 0xd5aa,
    0xd600, 0xd656, 0xd6ad, 0xd703, 0xd75a, 0xd7b0, 0xd806, 0xd85d, 0xd8b3, 0xd90a, 0xd960, 0xd9b6, 0xda0d, 0xda63, 0xdab9, 0xdb10,
    0xdb66, 0xdbbd, 0xdc13, 0xdc69, 0xdcc0, 0xdd16, 0xdd6d, 0xddc3, 0xde19, 0xde70, 0xdec6, 0xdf1c, 0xdf73, 0xdfc9, 0xe020, 0xe076
};

//const table
const MS_U16 input_gamma_idx_10k[512] =
{
    0x0000, 0x0011, 0x001d, 0x0029, 0x0033, 0x003e, 0x0048, 0x0052, 0x005d, 0x0067, 0x0071, 0x007b, 0x0085, 0x008f, 0x009a, 0x00a4,
    0x00ae, 0x00b9, 0x00c3, 0x00ce, 0x00d9, 0x00e3, 0x00ee, 0x00f9, 0x0104, 0x010f, 0x011a, 0x0126, 0x0131, 0x013c, 0x0148, 0x0154,
    0x015f, 0x016b, 0x0177, 0x0183, 0x0190, 0x019c, 0x01a8, 0x01b5, 0x01c1, 0x01ce, 0x01db, 0x01e8, 0x01f5, 0x0202, 0x0210, 0x021d,
    0x022b, 0x0238, 0x0246, 0x0254, 0x0262, 0x0270, 0x027e, 0x028d, 0x029b, 0x02aa, 0x02b9, 0x02c8, 0x02d7, 0x02e6, 0x02f5, 0x0304,
    0x0314, 0x0324, 0x0333, 0x0343, 0x0354, 0x0364, 0x0374, 0x0385, 0x0395, 0x03a6, 0x03b7, 0x03c8, 0x03d9, 0x03eb, 0x03fc, 0x040e,
    0x0420, 0x0432, 0x0444, 0x0456, 0x0468, 0x047b, 0x048e, 0x04a0, 0x04b3, 0x04c7, 0x04da, 0x04ee, 0x0501, 0x0515, 0x0529, 0x053d,
    0x0551, 0x0566, 0x057b, 0x058f, 0x05a4, 0x05b9, 0x05cf, 0x05e4, 0x05fa, 0x0610, 0x0626, 0x063c, 0x0652, 0x0669, 0x0680, 0x0697,
    0x06ae, 0x06c5, 0x06dd, 0x06f4, 0x070c, 0x0724, 0x073c, 0x0755, 0x076d, 0x0786, 0x079f, 0x07b8, 0x07d2, 0x07eb, 0x0805, 0x081f,
    0x083a, 0x0854, 0x086f, 0x0889, 0x08a4, 0x08c0, 0x08db, 0x08f7, 0x0913, 0x092f, 0x094b, 0x0968, 0x0985, 0x09a2, 0x09bf, 0x09dd,
    0x09fa, 0x0a18, 0x0a36, 0x0a55, 0x0a73, 0x0a92, 0x0ab1, 0x0ad1, 0x0af0, 0x0b10, 0x0b30, 0x0b51, 0x0b71, 0x0b92, 0x0bb3, 0x0bd4,
    0x0bf6, 0x0c18, 0x0c3a, 0x0c5c, 0x0c7f, 0x0ca2, 0x0cc5, 0x0ce9, 0x0d0c, 0x0d30, 0x0d55, 0x0d79, 0x0d9e, 0x0dc3, 0x0de9, 0x0e0e,
    0x0e34, 0x0e5a, 0x0e81, 0x0ea8, 0x0ecf, 0x0ef6, 0x0f1e, 0x0f46, 0x0f6f, 0x0f97, 0x0fc0, 0x0fe9, 0x1013, 0x103d, 0x1067, 0x1092,
    0x10bd, 0x10e8, 0x1113, 0x113f, 0x116b, 0x1198, 0x11c5, 0x11f2, 0x121f, 0x124d, 0x127c, 0x12aa, 0x12d9, 0x1308, 0x1338, 0x1368,
    0x1398, 0x13c9, 0x13fa, 0x142c, 0x145d, 0x1490, 0x14c2, 0x14f5, 0x1529, 0x155c, 0x1591, 0x15c5, 0x15fa, 0x162f, 0x1665, 0x169b,
    0x16d2, 0x1709, 0x1740, 0x1778, 0x17b0, 0x17e9, 0x1822, 0x185b, 0x1895, 0x18d0, 0x190b, 0x1946, 0x1982, 0x19be, 0x19fa, 0x1a37,
    0x1a75, 0x1ab3, 0x1af1, 0x1b30, 0x1b70, 0x1bb0, 0x1bf0, 0x1c31, 0x1c72, 0x1cb4, 0x1cf6, 0x1d39, 0x1d7d, 0x1dc1, 0x1e05, 0x1e4a,
    0x1e8f, 0x1ed5, 0x1f1c, 0x1f63, 0x1faa, 0x1ff2, 0x203b, 0x2084, 0x20ce, 0x2118, 0x2163, 0x21ae, 0x21fa, 0x2247, 0x2294, 0x22e2,
    0x2330, 0x237f, 0x23cf, 0x241f, 0x246f, 0x24c1, 0x2513, 0x2565, 0x25b9, 0x260c, 0x2661, 0x26b6, 0x270c, 0x2762, 0x27b9, 0x2811,
    0x2869, 0x28c3, 0x291c, 0x2977, 0x29d2, 0x2a2e, 0x2a8a, 0x2ae8, 0x2b46, 0x2ba4, 0x2c04, 0x2c64, 0x2cc5, 0x2d26, 0x2d89, 0x2dec,
    0x2e50, 0x2eb4, 0x2f1a, 0x2f80, 0x2fe7, 0x304f, 0x30b8, 0x3121, 0x318b, 0x31f6, 0x3262, 0x32cf, 0x333c, 0x33ab, 0x341a, 0x348a,
    0x34fb, 0x356d, 0x35df, 0x3653, 0x36c7, 0x373d, 0x37b3, 0x382a, 0x38a3, 0x391c, 0x3996, 0x3a11, 0x3a8c, 0x3b09, 0x3b87, 0x3c06,
    0x3c86, 0x3d07, 0x3d89, 0x3e0b, 0x3e8f, 0x3f14, 0x3f9a, 0x4021, 0x40a9, 0x4132, 0x41bd, 0x4248, 0x42d4, 0x4362, 0x43f0, 0x4480,
    0x4511, 0x45a3, 0x4636, 0x46ca, 0x4760, 0x47f6, 0x488e, 0x4927, 0x49c1, 0x4a5d, 0x4afa, 0x4b98, 0x4c37, 0x4cd7, 0x4d79, 0x4e1c,
    0x4ec0, 0x4f66, 0x500d, 0x50b5, 0x515f, 0x520a, 0x52b6, 0x5364, 0x5413, 0x54c4, 0x5576, 0x5629, 0x56de, 0x5794, 0x584c, 0x5905,
    0x59c0, 0x5a7c, 0x5b3a, 0x5bf9, 0x5cba, 0x5d7d, 0x5e41, 0x5f06, 0x5fcd, 0x6096, 0x6161, 0x622d, 0x62fa, 0x63ca, 0x649b, 0x656e,
    0x6642, 0x6719, 0x67f1, 0x68cb, 0x69a6, 0x6a83, 0x6b63, 0x6c44, 0x6d27, 0x6e0b, 0x6ef2, 0x6fdb, 0x70c5, 0x71b2, 0x72a0, 0x7390,
    0x7483, 0x7577, 0x766d, 0x7766, 0x7860, 0x795d, 0x7a5c, 0x7b5d, 0x7c60, 0x7d65, 0x7e6c, 0x7f76, 0x8081, 0x818f, 0x82a0, 0x83b2,
    0x84c7, 0x85de, 0x86f8, 0x8814, 0x8932, 0x8a53, 0x8b76, 0x8c9c, 0x8dc4, 0x8eef, 0x901c, 0x914c, 0x927f, 0x93b4, 0x94eb, 0x9626,
    0x9763, 0x98a3, 0x99e5, 0x9b2a, 0x9c73, 0x9dbe, 0x9f0b, 0xa05c, 0xa1b0, 0xa306, 0xa460, 0xa5bc, 0xa71c, 0xa87e, 0xa9e4, 0xab4d,
    0xacb9, 0xae28, 0xaf9a, 0xb110, 0xb289, 0xb405, 0xb584, 0xb707, 0xb88d, 0xba17, 0xbba4, 0xbd35, 0xbec9, 0xc061, 0xc1fd, 0xc39c,
    0xc53f, 0xc6e5, 0xc88f, 0xca3e, 0xcbf0, 0xcda5, 0xcf5f, 0xd11d, 0xd2df, 0xd4a5, 0xd66f, 0xd83d, 0xda0f, 0xdbe5, 0xddc0, 0xdf9f,
    0xe182, 0xe36a, 0xe556, 0xe747, 0xe93c, 0xeb36, 0xed34, 0xef37, 0xf13f, 0xf34c, 0xf55d, 0xf774, 0xf98f, 0xfbaf, 0xfdd5, 0xffff
};

//const table
const MS_U16 input_gamma_idx_4k[512] =
{
    0x0000, 0x001a, 0x002d, 0x003e, 0x004e, 0x005e, 0x006e, 0x007d, 0x008c, 0x009c, 0x00ab, 0x00bb, 0x00ca, 0x00d9, 0x00e9, 0x00f9,
    0x0108, 0x0118, 0x0128, 0x0138, 0x0148, 0x0159, 0x0169, 0x017a, 0x018a, 0x019b, 0x01ac, 0x01bd, 0x01cf, 0x01e0, 0x01f1, 0x0203,
    0x0215, 0x0227, 0x0239, 0x024c, 0x025e, 0x0271, 0x0283, 0x0296, 0x02aa, 0x02bd, 0x02d0, 0x02e4, 0x02f8, 0x030c, 0x0320, 0x0334,
    0x0349, 0x035e, 0x0373, 0x0388, 0x039d, 0x03b2, 0x03c8, 0x03de, 0x03f4, 0x040a, 0x0421, 0x0437, 0x044e, 0x0465, 0x047c, 0x0493,
    0x04ab, 0x04c3, 0x04db, 0x04f3, 0x050b, 0x0524, 0x053d, 0x0556, 0x056f, 0x0589, 0x05a2, 0x05bc, 0x05d6, 0x05f1, 0x060b, 0x0626,
    0x0641, 0x065c, 0x0678, 0x0693, 0x06af, 0x06cb, 0x06e8, 0x0704, 0x0721, 0x073e, 0x075c, 0x0779, 0x0797, 0x07b5, 0x07d3, 0x07f2,
    0x0811, 0x0830, 0x084f, 0x086f, 0x088f, 0x08af, 0x08cf, 0x08f0, 0x0910, 0x0932, 0x0953, 0x0975, 0x0997, 0x09b9, 0x09db, 0x09fe,
    0x0a21, 0x0a44, 0x0a68, 0x0a8c, 0x0ab0, 0x0ad5, 0x0af9, 0x0b1e, 0x0b44, 0x0b69, 0x0b8f, 0x0bb6, 0x0bdc, 0x0c03, 0x0c2a, 0x0c52,
    0x0c79, 0x0ca1, 0x0cca, 0x0cf3, 0x0d1c, 0x0d45, 0x0d6f, 0x0d99, 0x0dc3, 0x0dee, 0x0e19, 0x0e44, 0x0e70, 0x0e9c, 0x0ec8, 0x0ef5,
    0x0f22, 0x0f4f, 0x0f7d, 0x0fab, 0x0fda, 0x1008, 0x1038, 0x1067, 0x1097, 0x10c7, 0x10f8, 0x1129, 0x115b, 0x118c, 0x11bf, 0x11f1,
    0x1224, 0x1257, 0x128b, 0x12bf, 0x12f4, 0x1329, 0x135e, 0x1394, 0x13ca, 0x1401, 0x1438, 0x146f, 0x14a7, 0x14df, 0x1518, 0x1551,
    0x158b, 0x15c5, 0x15ff, 0x163a, 0x1676, 0x16b1, 0x16ee, 0x172a, 0x1768, 0x17a5, 0x17e3, 0x1822, 0x1861, 0x18a1, 0x18e1, 0x1921,
    0x1962, 0x19a4, 0x19e6, 0x1a28, 0x1a6b, 0x1aaf, 0x1af3, 0x1b37, 0x1b7c, 0x1bc2, 0x1c08, 0x1c4f, 0x1c96, 0x1cde, 0x1d26, 0x1d6f,
    0x1db8, 0x1e02, 0x1e4c, 0x1e97, 0x1ee3, 0x1f2f, 0x1f7c, 0x1fc9, 0x2017, 0x2066, 0x20b5, 0x2104, 0x2155, 0x21a6, 0x21f7, 0x2249,
    0x229c, 0x22ef, 0x2343, 0x2398, 0x23ed, 0x2443, 0x249a, 0x24f1, 0x2549, 0x25a1, 0x25fb, 0x2654, 0x26af, 0x270a, 0x2766, 0x27c3,
    0x2820, 0x287e, 0x28dd, 0x293c, 0x299c, 0x29fd, 0x2a5f, 0x2ac1, 0x2b25, 0x2b88, 0x2bed, 0x2c52, 0x2cb9, 0x2d1f, 0x2d87, 0x2df0,
    0x2e59, 0x2ec3, 0x2f2e, 0x2f9a, 0x3006, 0x3073, 0x30e2, 0x3151, 0x31c0, 0x3231, 0x32a3, 0x3315, 0x3388, 0x33fc, 0x3471, 0x34e7,
    0x355e, 0x35d6, 0x364e, 0x36c8, 0x3742, 0x37be, 0x383a, 0x38b7, 0x3935, 0x39b5, 0x3a35, 0x3ab6, 0x3b38, 0x3bbb, 0x3c3f, 0x3cc4,
    0x3d4a, 0x3dd1, 0x3e5a, 0x3ee3, 0x3f6d, 0x3ff8, 0x4085, 0x4112, 0x41a1, 0x4230, 0x42c1, 0x4353, 0x43e6, 0x447a, 0x450f, 0x45a5,
    0x463d, 0x46d6, 0x4770, 0x480b, 0x48a7, 0x4944, 0x49e3, 0x4a83, 0x4b24, 0x4bc6, 0x4c6a, 0x4d0e, 0x4db5, 0x4e5c, 0x4f05, 0x4faf,
    0x505a, 0x5107, 0x51b5, 0x5264, 0x5314, 0x53c7, 0x547a, 0x552f, 0x55e5, 0x569d, 0x5756, 0x5810, 0x58cc, 0x598a, 0x5a48, 0x5b09,
    0x5bcb, 0x5c8e, 0x5d53, 0x5e19, 0x5ee1, 0x5fab, 0x6076, 0x6143, 0x6211, 0x62e1, 0x63b3, 0x6486, 0x655b, 0x6631, 0x670a, 0x67e4,
    0x68bf, 0x699d, 0x6a7c, 0x6b5d, 0x6c40, 0x6d24, 0x6e0a, 0x6ef2, 0x6fdc, 0x70c8, 0x71b6, 0x72a5, 0x7397, 0x748a, 0x7580, 0x7677,
    0x7770, 0x786b, 0x7969, 0x7a68, 0x7b69, 0x7c6d, 0x7d72, 0x7e79, 0x7f83, 0x808f, 0x819d, 0x82ad, 0x83bf, 0x84d4, 0x85ea, 0x8703,
    0x881e, 0x893c, 0x8a5c, 0x8b7e, 0x8ca2, 0x8dc9, 0x8ef2, 0x901e, 0x914c, 0x927d, 0x93b0, 0x94e5, 0x961d, 0x9758, 0x9895, 0x99d5,
    0x9b17, 0x9c5c, 0x9da4, 0x9eee, 0xa03b, 0xa18b, 0xa2dd, 0xa433, 0xa58b, 0xa6e6, 0xa844, 0xa9a4, 0xab08, 0xac6f, 0xadd8, 0xaf45,
    0xb0b4, 0xb227, 0xb39d, 0xb515, 0xb691, 0xb810, 0xb993, 0xbb18, 0xbca1, 0xbe2d, 0xbfbc, 0xc14f, 0xc2e5, 0xc47f, 0xc61c, 0xc7bc,
    0xc960, 0xcb08, 0xccb3, 0xce61, 0xd014, 0xd1ca, 0xd384, 0xd541, 0xd702, 0xd8c7, 0xda90, 0xdc5d, 0xde2e, 0xe003, 0xe1db, 0xe3b8,
    0xe599, 0xe77e, 0xe967, 0xeb55, 0xed46, 0xef3c, 0xf136, 0xf335, 0xf538, 0xf740, 0xf94c, 0xfb5c, 0xfd71, 0xff8b, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
};
const MS_U16 BT709toBT1886Gain[512] =
{
300,928,1000,984,949,910,873,839,807,778,757,739,725,713,702,693,
685,678,672,666,661,656,652,648,644,640,637,634,631,628,626,623,
621,619,617,615,613,611,609,608,606,605,603,602,600,599,598,597,
595,594,593,592,591,590,589,588,587,586,585,585,584,583,582,581,
581,580,579,578,578,577,576,576,575,574,574,573,573,572,571,571,
570,570,569,569,568,568,567,567,566,566,565,565,564,564,564,563,
563,562,562,562,561,561,560,560,560,559,559,559,558,558,557,557,
557,556,556,556,555,555,555,555,554,554,554,553,553,553,552,552,
552,552,551,551,551,551,550,550,550,550,549,549,549,549,548,548,
548,548,547,547,547,547,546,546,546,546,546,545,545,545,545,544,
544,544,544,544,543,543,543,543,543,542,542,542,542,542,542,541,
541,541,541,541,540,540,540,540,540,540,539,539,539,539,539,539,
538,538,538,538,538,538,537,537,537,537,537,537,537,536,536,536,
536,536,536,536,535,535,535,535,535,535,535,534,534,534,534,534,
534,534,534,533,533,533,533,533,533,533,532,532,532,532,532,532,
532,532,532,531,531,531,531,531,531,531,531,530,530,530,530,530,
530,530,530,530,529,529,529,529,529,529,529,529,529,529,528,528,
528,528,528,528,528,528,528,528,527,527,527,527,527,527,527,527,
527,527,526,526,526,526,526,526,526,526,526,526,526,525,525,525,
525,525,525,525,525,525,525,525,524,524,524,524,524,524,524,524,
524,524,524,524,523,523,523,523,523,523,523,523,523,523,523,523,
522,522,522,522,522,522,522,522,522,522,522,522,522,521,521,521,
521,521,521,521,521,521,521,521,521,521,521,520,520,520,520,520,
520,520,520,520,520,520,520,520,520,519,519,519,519,519,519,519,
519,519,519,519,519,519,519,519,518,518,518,518,518,518,518,518,
518,518,518,518,518,518,518,518,517,517,517,517,517,517,517,517,
517,517,517,517,517,517,517,517,517,516,516,516,516,516,516,516,
516,516,516,516,516,516,516,516,516,516,515,515,515,515,515,515,
515,515,515,515,515,515,515,515,515,515,515,515,515,514,514,514,
514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,514,
513,513,513,513,513,513,513,513,513,513,513,513,513,513,513,513,
513,513,513,513,512,512,512,512,512,512,512,512,512,512,512,512,
};

MS_U16 MS_Cfd_Luminance_To_PQCode(MS_U16 u16Luminance, MS_U8 u8Function_Mode);
MS_U32 Luminance_To_GammaCode(MS_U16 u16Nits_in, MS_U16 u16maxLuma);
void msDlcWriteCurve(MS_BOOL bWindow);
void msReferTMO(void);
void RefernceTMOSetting(MS_U8 u8Mode);
void msNewTMO(MS_BOOL bWindow);
MS_S8 msNewDlcHandler_WithCorrectionRatio(MS_BOOL bWindow);
MS_U8 msDLC_linearCurve(MS_BOOL bWindow);
MS_BOOL MHalIpHistogramHandler(MS_BOOL bWindow);
void MHalPqDlcHandler(MS_BOOL bWindow);
#define _Driver_IIR
#define STATUS_ERROR (-1)
#define STATUS_SUCCESS 0x00

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
void msWriteByteMask(DWORD u32Reg, BYTE u8Val ,MS_U8 u8Mask)
{
    MS_U8 u8Data=0 ;
    u8Data = msReadByte(u32Reg);
    u8Data = (u8Data & (0xFF -u8Mask))|(u8Val &u8Mask);
    msWriteByte(u32Reg,u8Data);
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
#if 1
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
    //New DLC parameter
    g_DlcParameters.ucLMaxThreshold = u16DlcInit[46];
    g_DlcParameters.ucLMinThreshold = u16DlcInit[47];
    g_DlcParameters.ucLMaxCorrection = u16DlcInit[48];
    g_DlcParameters.ucLMinCorrection = u16DlcInit[49];
    g_DlcParameters.ucRMaxThreshold = u16DlcInit[50];
    g_DlcParameters.ucRMinThreshold = u16DlcInit[51];
    g_DlcParameters.ucRMaxCorrection = u16DlcInit[52];
    g_DlcParameters.ucRMinCorrection = u16DlcInit[53];
    g_DlcParameters.ucAllowLoseContrast = u16DlcInit[54];
    g_DlcParameters.ucKernelDlcAlgorithm = 1; // Use 512 level.
    g_DlcParameters.ucDlcHistogramSource = 1; // DLC histogram from IP.
    g_DlcParameters.u8Dlc_Mode = 1;
    g_DlcParameters.u8Tmo_Mode = 0;

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
    printf("ucLMaxThreshold     :%d\n",g_DlcParameters.ucLMaxThreshold      );
    printf("ucLMinThreshold     :%d\n",g_DlcParameters.ucLMinThreshold      );
    printf("ucLMaxCorrection    :%d\n",g_DlcParameters.ucLMaxCorrection     );
    printf("ucLMinCorrection    :%d\n",g_DlcParameters.ucLMinCorrection     );
    printf("ucRMaxThreshold     :%d\n",g_DlcParameters.ucRMaxThreshold      );
    printf("ucRMinThreshold     :%d\n",g_DlcParameters.ucRMinThreshold      );
    printf("ucRMaxCorrection    :%d\n",g_DlcParameters.ucRMaxCorrection     );
    printf("ucRMinCorrection    :%d\n",g_DlcParameters.ucRMinCorrection     );
    printf("ucAllowLoseContrast :%d\n",g_DlcParameters.ucAllowLoseContrast  );
    printf("ucKernelDlcAlgorithm:%d\n",g_DlcParameters.ucKernelDlcAlgorithm );
    printf("ucDlcHistogramSource:%d\n",g_DlcParameters.ucDlcHistogramSource );
    printf("u8Dlc_Mode          :%d\n",g_DlcParameters.u8Dlc_Mode           );
    printf("u8Tmo_Mode          :%d\n",g_DlcParameters.u8Tmo_Mode           );
    printf("u8Dlc_Mode          :%d\n",g_DlcParameters.u8Dlc_Mode           );

    MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_02_L, 0 );
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_03_L, 0 );
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_04_L, 0 );
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_06_L, 0 );
    msWriteByte(REG_SC_BK30_07_H, 0 );
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_08_L, 0 );
    msWriteByte(REG_SC_BK30_09_H, 0 );
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_0A_L, 0 );
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_0B_L, 0 );
    msWriteByte(REG_SC_BK30_0C_L, 0 );
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_0D_L, 0 );
    MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_0E_L, 0 );
    msWriteByte(REG_SC_BK30_0F_L, 0 );


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
#else
    g_DlcParameters.ucDlcPureImageMode     = 2; // Compare difference of max and min bright
    g_DlcParameters.ucDlcLevelLimit        = 0;    // n = 0 ~ 4 => Limit n levels => ex. n=2, limit 2 level 0xF7, 0xE7
    g_DlcParameters.ucDlcAvgDelta          = 15;   // n = 0 ~ 50, default value: 12
    g_DlcParameters.ucDlcAvgDeltaStill     = 0;    // n = 0 ~ 15 => 0: disable still curve, 1 ~ 15: enable still curve
    g_DlcParameters.ucDlcFastAlphaBlending = 31;   // min 17 ~ max 32
    g_DlcParameters.ucDlcYAvgThresholdL    = 5;    // default value: 0
    g_DlcParameters.ucDlcYAvgThresholdH    = 200;  // default value: 128
    g_DlcParameters.ucDlcBLEPoint          = 48;   // n = 24 ~ 64, default value: 48
    g_DlcParameters.ucDlcWLEPoint          = 48;   // n = 24 ~ 64, default value: 48
    g_DlcParameters.bEnableBLE             = 0;    // 1: enable; 0: disable
    g_DlcParameters.bEnableWLE             = 0;    // 1: enable; 0: disable
    g_DlcParameters.ucDlcYAvgThresholdM    = 70;
    g_DlcParameters.ucDlcCurveMode         = 2;
    g_DlcParameters.ucDlcCurveModeMixAlpha = 72;
    g_DlcParameters.ucDlcAlgorithmMode     = 2;
    g_DlcParameters.ucDlcSepPointH         = 188;
    g_DlcParameters.ucDlcSepPointL         = 80;
    g_DlcParameters.uwDlcBleStartPointTH   = 640;
    g_DlcParameters.uwDlcBleEndPointTH     = 320;
    g_DlcParameters.ucDlcCurveDiff_L_TH    = 56;
    g_DlcParameters.ucDlcCurveDiff_H_TH    = 148;
    g_DlcParameters.uwDlcBLESlopPoint_1    = 1032;
    g_DlcParameters.uwDlcBLESlopPoint_2    = 1172;
    g_DlcParameters.uwDlcBLESlopPoint_3    = 1242;
    g_DlcParameters.uwDlcBLESlopPoint_4    = 1324;
    g_DlcParameters.uwDlcBLESlopPoint_5    = 1396;
    g_DlcParameters.uwDlcDark_BLE_Slop_Min = 1200;
    g_DlcParameters.ucDlcCurveDiffCoringTH = 2;
    g_DlcParameters.ucDlcAlphaBlendingMin  = 1;
    g_DlcParameters.ucDlcAlphaBlendingMax  = 128;
    g_DlcParameters.ucDlcFlicker_alpha     = 96;
    g_DlcParameters.ucDlcYAVG_L_TH         = 56;
    g_DlcParameters.ucDlcYAVG_H_TH         = 136;
    g_DlcParameters.ucDlcDiffBase_L        = 4;
    g_DlcParameters.ucDlcDiffBase_M        = 14;
    g_DlcParameters.ucDlcDiffBase_H        = 20;
    //New DLC parameter
    g_DlcParameters.ucLMaxThreshold        = 128;
    g_DlcParameters.ucLMinThreshold        = 34;
    g_DlcParameters.ucLMaxCorrection       = 196;
    g_DlcParameters.ucLMinCorrection       = 60;
    g_DlcParameters.ucRMaxThreshold        = 100;
    g_DlcParameters.ucRMinThreshold        = 20;
    g_DlcParameters.ucRMaxCorrection       = 60;
    g_DlcParameters.ucRMinCorrection       = 20;
    g_DlcParameters.ucAllowLoseContrast    = 0;
    g_DlcParameters.ucKernelDlcAlgorithm   = 0;
    g_DlcParameters.ucDlcHistogramSource   = 1;
    g_DlcParameters.u8Dlc_Mode = 1;
    g_DlcParameters.u8Tmo_Mode = 0;

#endif


    return TRUE ;
}

void msDlcInitEx(void)
{
    g_DlcParameters.ucKernelDlcAlgorithm = 0; // Use 512 level.
    g_DlcParameters.ucDlcHistogramSource = 0; // DLC histogram from IP.
    g_DlcParameters.u8Dlc_Mode = 1;
    g_DlcParameters.u8Tmo_Mode = 0;
    printf("KERNEL msDlcInitEx ucKernelDlcAlgorithm:%d\n",g_DlcParameters.ucKernelDlcAlgorithm );
    printf("KERNEL msDlcInitEx ucDlcHistogramSource:%d\n",g_DlcParameters.ucDlcHistogramSource );
    printf("KERNEL msDlcInitEx u8Dlc_Mode          :%d\n",g_DlcParameters.u8Dlc_Mode           );
    printf("KERNEL msDlcInitEx u8Tmo_Mode          :%d\n",g_DlcParameters.u8Tmo_Mode           );
}

MS_BOOL msTMOinit(void)
{
    printf("u16SrcMinRatio        :%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SrcMinRatio         );
    printf("u16SrcMedRatio        :%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SrcMedRatio         );
    printf("u16SrcMaxRatio        :%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SrcMaxRatio         );
    printf("u8TgtMinFlag          :%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag           );
    printf("u16TgtMin             :%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMin              );
    printf("u8TgtMaxFlag          :%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMaxFlag           );
    printf("u16TgtMax             :%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMax              );
    printf("u16TgtMed             :%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMed              );
    printf("u16FrontSlopeMin      :%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16FrontSlopeMin       );
    printf("u16FrontSlopeMax      :%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16FrontSlopeMax       );
    printf("u16BackSlopeMin       :%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16BackSlopeMin        );
    printf("u16BackSlopeMax       :%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16BackSlopeMax        );
    printf("u16SceneChangeThrd    :%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SceneChangeThrd     );
    printf("u16SceneChangeRatioMax:%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SceneChangeRatioMax );
    printf("u8IIRRatio            :%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u8IIRRatio             );
    printf("u8TMO_TargetMode      :%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode       );
    printf("u8TMO_Algorithm       :%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_Algorithm        );
    #if 0
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SrcMinRatio         = 10;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SrcMedRatio         = 512;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SrcMaxRatio         = 1019;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag           = 1;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMin              = 500;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMaxFlag           = 0;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMax              = 300;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMed              = 0;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16FrontSlopeMin = 256;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16FrontSlopeMax = 512;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16BackSlopeMin = 128;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16BackSlopeMax = 256;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SceneChangeThrd     = 512;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SceneChangeRatioMax = 1024;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u8IIRRatio             = 31;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode = 0;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_Algorithm = 1;
    #endif
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_targetconfigs_source = 1;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_intoTMOset = 0;

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
MS_BOOL msGetSCHistogramHandler(MS_BOOL bWindow)
{
    BYTE ucTmp;
    WORD wTmpHistogramSum; // for protect histogram overflow
    BYTE ucHistogramOverflowIndex;
    BYTE ucReturn;
    MS_U32 Tg_wLumaHistogram32H[32];
    //MS_U8 u8log = LogPrintfOneTimes();
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
            //enhance to support 20 bits
            Tg_wLumaHistogram32H[ucTmp]=MApi_GFLIP_XC_R2BYTE(REG_ADDR_HISTOGRAM_DATA_32+ (4*ucTmp)+2)&0x000F;
            Tg_wLumaHistogram32H[ucTmp] <<= 16;
            Tg_wLumaHistogram32H[ucTmp] |= MApi_GFLIP_XC_R2BYTE(REG_ADDR_HISTOGRAM_DATA_32+ (4*ucTmp));
            g_wLumaHistogram32H[ucTmp] = (Tg_wLumaHistogram32H[ucTmp])>>4;
            if(1 == u8log)
            {
                printf("ucTmp:%d  g_wLumaHistogram32H:%x\n",ucTmp,g_wLumaHistogram32H[ucTmp]);
            }
            //DLC Bank 0x1A . Check for MSTV_Tool debug.
            if((MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK30_01_L, 0xFF))==0x1A)
            {
                g_ucDLCInitBuffer[118+ucTmp*2]=g_wLumaHistogram32H[ucTmp]&0xFF;
                g_ucDLCInitBuffer[119+ucTmp*2]=(g_wLumaHistogram32H[ucTmp]>>8)&0xFF;
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

        //Get the histogram report sum of pixel number .
        g_wLumiTotalCount = (MApi_GFLIP_XC_R2BYTE(REG_ADDR_HISTOGRAM_TOTAL_COUNT_L)
                             | (MApi_GFLIP_XC_R2BYTE(REG_ADDR_HISTOGRAM_TOTAL_COUNT_L+2)<<16))>>4;

        // protect histogram overflow
        if (ucHistogramOverflowIndex != 0xFF)
        {
            if (wTmpHistogramSum > g_wLumiTotalCount)
                wTmpHistogramSum = g_wLumiTotalCount;
            g_wLumaHistogram32H[ucHistogramOverflowIndex] += (g_wLumiTotalCount - wTmpHistogramSum);
        }

        // Get MIN. & MAX VALUE
        if( MAIN_WINDOW == bWindow )   // Main window
        {
            g_ucHistogramMin = msReadByte(REG_ADDR_MAIN_MIN_VALUE);
            g_ucHistogramMax = msReadByte(REG_ADDR_MAIN_MAX_VALUE);
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

        g_wLumiAverageTemp = ((MApi_GFLIP_XC_R2BYTE( REG_ADDR_HISTOGRAM_TOTAL_SUM_L+2 )<<16) | MApi_GFLIP_XC_R2BYTE( REG_ADDR_HISTOGRAM_TOTAL_SUM_L ))>>4;
        if(1 == u8log)
        {
            printf("g_wLumiTotalCount:%x g_wLumiAverageTemp:%x\n",g_wLumiTotalCount,g_wLumiAverageTemp);
        }
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

MS_BOOL msGetIpHistogramHandler(MS_BOOL bWindow)
{
    BYTE ucTmp;
    WORD wTmpHistogramSum; // for protect histogram overflow
    BYTE ucHistogramOverflowIndex;
    BYTE ucReturn;
    MS_U32 u32YstatisticsFromIP_L[32];
    MS_U32 u32YstatisticsFromIP_M[32];
    MS_U32 u32LumaTotalCount_L,u32LumaTotalCount_M;
    MS_U32 u32HistogramTotal_L,u32HistogramTotal_M;
    //When DLC algorithm is not from kernel, force return false.
    if( g_DlcParameters.ucDlcAlgorithmMode != XC_DLC_ALGORITHM_KERNEL )
    {
        return FALSE;
    }

    msDlc_FunctionEnter();
    //Enable IP histogram HW start
    msWriteByte(REG_ADDR_IP_HISTOGRAM_ENABLE, 0x0F);
    msWriteByte(REG_ADDR_IP_HISTOGRAM_H_END_L, 0x00);
    msWriteByte(REG_ADDR_IP_HISTOGRAM_H_END_H, 0x0F);
    msWriteByte(REG_ADDR_IP_HISTOGRAM_V_END_L, 0x70);
    msWriteByte(REG_ADDR_IP_HISTOGRAM_V_END_H, 0x08);
    //Enable IP histogram HW end
    //printf("IP Histogram\n");
#if 0 //Have double buffer, does not need to wait.
    // ENABLE_FAST_HISTOGRAM_CATCH
    if( !(msReadByte(REG_ADDR_DLC_HANDSHAKE) & _BIT3) )  // Wait until ready
    {
        msDlc_FunctionExit();
        return FALSE;
    }
#endif

    if ((MS_BOOL)g_DlcParameters.ucDlcTimeOut)
    {
        // Get histogram 32 data
        // Total/BW_ref histogram count
        MaxHistogram32H=0;
        for (ucTmp = 0, wTmpHistogramSum = 0, ucHistogramOverflowIndex = 0xFF; ucTmp < 32; ucTmp++ )
        {

            u32YstatisticsFromIP_L[ucTmp] = msReadByte(REG_ADDR_IP_HISTOGRAM_DATA_32+(4*ucTmp+1));
            u32YstatisticsFromIP_L[ucTmp] <<= 8;
            u32YstatisticsFromIP_L[ucTmp] |= msReadByte(REG_ADDR_IP_HISTOGRAM_DATA_32+(4*ucTmp));
            u32YstatisticsFromIP_M[ucTmp] = msReadByte(REG_ADDR_IP_HISTOGRAM_DATA_32+(4*ucTmp+3));
            u32YstatisticsFromIP_M[ucTmp] <<= 8;
            u32YstatisticsFromIP_M[ucTmp] |= msReadByte(REG_ADDR_IP_HISTOGRAM_DATA_32+(4*ucTmp+2));

            u32YstatisticsFromIP_M[ucTmp] = (u32YstatisticsFromIP_M[ucTmp]<<16) +u32YstatisticsFromIP_L[ucTmp];
            //printf("IP u32YstatisticsFromIP_M:%d :%lx (>>8):%d\n",ucTmp,u32YstatisticsFromIP_M[ucTmp],u32YstatisticsFromIP_M[ucTmp]>>8);

            g_wLumaHistogram32H[ucTmp] = u32YstatisticsFromIP_M[ucTmp]>>8;//24bit->16bit
            //DLC Bank 0x1A . Check for MSTV_Tool debug.
            if((MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK30_01_L, 0xFF))==0x1A)
            {
                g_ucDLCInitBuffer[118+ucTmp*2]=msReadByte(REG_ADDR_IP_HISTOGRAM_DATA_32+(2*ucTmp));
                g_ucDLCInitBuffer[119+ucTmp*2]=msReadByte(REG_ADDR_IP_HISTOGRAM_DATA_32+(2*ucTmp)+1);
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

        u32LumaTotalCount_L = msReadByte(REG_ADDR_IP_HISTOGRAM_TOTAL_COUNT_H_0A);
        u32LumaTotalCount_L<<= 8;
        u32LumaTotalCount_L |= msReadByte(REG_ADDR_IP_HISTOGRAM_TOTAL_COUNT_L_0A);
        u32LumaTotalCount_M = msReadByte(REG_ADDR_IP_HISTOGRAM_TOTAL_COUNT_H_0B);
        u32LumaTotalCount_M<<= 8;
        u32LumaTotalCount_M |= msReadByte(REG_ADDR_IP_HISTOGRAM_TOTAL_COUNT_L_0B);
        u32LumaTotalCount_M = (u32LumaTotalCount_M<<16) + u32LumaTotalCount_L;
        //printf("IP u32LumaTotalCount_M:%ld\n",u32LumaTotalCount_M);
        g_wLumiTotalCount = u32LumaTotalCount_M>>8;
        // protect histogram overflow
        if (ucHistogramOverflowIndex != 0xFF)
        {
            if (wTmpHistogramSum > g_wLumiTotalCount)
                wTmpHistogramSum = g_wLumiTotalCount;
            g_wLumaHistogram32H[ucHistogramOverflowIndex] += (g_wLumiTotalCount - wTmpHistogramSum);
        }

        u32HistogramTotal_L = msReadByte(REG_ADDR_IP_HISTOGRAM_TOTAL_SUM_H_0C);
        u32HistogramTotal_L<<= 8;
        u32HistogramTotal_L |= msReadByte(REG_ADDR_IP_HISTOGRAM_TOTAL_SUM_L_0C);
        u32HistogramTotal_M = msReadByte(REG_ADDR_IP_HISTOGRAM_TOTAL_SUM_H_0D);
        u32HistogramTotal_M<<= 8;
        u32HistogramTotal_M |= msReadByte(REG_ADDR_IP_HISTOGRAM_TOTAL_SUM_L_0D);
        u32HistogramTotal_M = (u32HistogramTotal_M<<16) + u32HistogramTotal_L;
        //printf("IP u32HistogramTotal_M:%ld\n",u32HistogramTotal_M);
        // Get MIN. & MAX VALUE
        if( MAIN_WINDOW == bWindow )   // Main window
        {
            g_ucHistogramMin = msReadByte(REG_ADDR_IP_MAIN_MIN_VALUE);
            g_ucHistogramMax = msReadByte(REG_ADDR_IP_MAIN_MAX_VALUE);
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
        g_wLumiAverageTemp = u32HistogramTotal_M>>16;
        g_ucTmpAvgN = msGetAverageLuminous();
        g_wTmpAvgN_x10 = msGetAverageLuminous_x10();
        //printf("IP g_ucTmpAvgN:%d weight:%ld total:%ld\n",g_ucTmpAvgN,g_wLumiAverageTemp,g_wLumiTotalCount);
        ucReturn = TRUE;
    }
    else
    {
        ucReturn = FALSE;
    }

    // After read histogram, request HW to do histogram
    // Request HW to do histogram
    //msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) & 0xF2);

    g_DlcParameters.ucDlcTimeOut = 150; // 150ms

    //msWriteByte(REG_ADDR_DLC_HANDSHAKE, msReadByte(REG_ADDR_DLC_HANDSHAKE) | _BIT2 );

    msDlc_FunctionExit();

    return ucReturn;
}

MS_BOOL msGetHistogramHandler(MS_BOOL bWindow)
{
#if defined(CONFIG_MSTAR_XC_HDR_SUPPORT)
    if (g_DlcParameters.ucDlcHistogramSource == 0)
    {
        return msGetSCHistogramHandler(bWindow);
    }
    else if (g_DlcParameters.ucDlcHistogramSource == 1)
    {
        return msGetIpHistogramHandler(bWindow);//msGetIpHistogramHandler(bWindow);
    }
#else
    return msGetSCHistogramHandler(bWindow);
#endif
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

//void SuperWhtie_forOldDLC(void)

//=========================================================
//
//=========================================================
void msOldDlcHandler(MS_BOOL bWindow)
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
    MS_U16 u16ExpandHistogram[32] = {0};

    msDlc_FunctionEnter();

    msDlcLumiDiffCtrl();
    //SuperWhtie_forOldDLC();
    HistogramExpand(g_wLumaHistogram32H,u16ExpandHistogram);
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
                uwHistogramAreaSum[ucTmp]=u16ExpandHistogram[ucTmp];
            }
            else
            {
                uwHistogramAreaSum[ucTmp] = uwHistogramAreaSum[ucTmp-1] + u16ExpandHistogram[ucTmp];
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
                               +((ucYAvgNormalize-(ucTmp+1)*8)*u16ExpandHistogram[(ucTmp+1)]+4)/8;
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
            if(((g_ucDlcFlickAlpha > 48) &&(uwSubChangeDelayCount>=100)) ||
               ((g_ucDlcFlickAlpha > 24) &&(uwSubChangeDelayCount>=200)))
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
                uwHistogramAreaSum[ucTmp]=u16ExpandHistogram[ucTmp];
            }
            else
            {
                uwHistogramAreaSum[ucTmp] = uwHistogramAreaSum[ucTmp-1] + u16ExpandHistogram[ucTmp];
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
            if(u16ExpandHistogram[0]==0)
            {
                DLC_DEBUG("\n Error in  [Kernel DLC][ %s  , %d ]  g_wLumaHistogram32H[0]=0 !!!\n", __FUNCTION__,__LINE__);
                return;
            }

            BLE_EndPoint = ((g_uwTable[0]*uwHistogramAreaSum[31])/(16*u16ExpandHistogram[0]));
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
                    BLE_sum = u16ExpandHistogram[0]*(g_uwTable[0]+1/2);
                }
                else
                {
                    BLE_sum = BLE_sum + u16ExpandHistogram[Tmp] * ((uwHistogramAreaSum[Tmp]+uwHistogramAreaSum[Tmp-1]+1)/2);
                }
            }

            //Mantis issue of 0232938:Hsense A3 DLC the divisor can't to be equal to zero .
            if(u16ExpandHistogram[ucTmp]==0)
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

        //~~~~~~~~~ output = uwBLE_Blend ?]with slow approach?^ ~~~~~~~~~~~~~~~~~~~~~

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
void msLinearTMO(void)
{
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
     MS_U16 u16tmo_size = 512;
     MS_U16 u16source_max_luma = 10000;
     MS_U16 u16panel_max_luma = 5000;
     //MS_U16 u16panel_max_luma = 10000;

     MS_U16 u16idx = 0;
     MS_U16 u16input = 0, u16input_code = 0, u16linear_in_code = 0;
     MS_U16 u16linear_out_code = 0;
     MS_U16 u16in_thrd = 0, u16out_thrd = 0, u16upper_bound;
     MS_U16 u16output = 0;
     MS_U32 u32norm_in = 0, u32gamma_out = 0;
     //in = 0:8:4088;
     //in = in';
     //in(512) = in(511);
     //tmo_lut = in;
     MS_U32 u32In = 0;
     MS_U32 u32Gamma = 0x7FFF;
     MS_U16 u16A = 0x8000;
     MS_U16 u16B = 0;
     MS_U16 u16S = 0x0100;
     MS_U8 u8BitDepth = 14;
     MS_U8 sMode = 0;

     MS_U16 i = 0;
     MS_U16 *u16curve512 = (MS_U16*)kmalloc(512 * sizeof(MS_U16), GFP_KERNEL);

//printk("\033[1;35m###[Brian][%s][%d]### msLinearTMO!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__);
//printk("msLinearTMO!!!!!!!!!!!!!!!!!\n");


#if  (TMO_TV ==TMO_DETAILS)
    u16panel_max_luma = MApi_GFLIP_XC_R2BYTE(REG_SC_BK4E_27_L);
#else
#endif

     u16linear_in_code = (MS_U16)Luminance_To_GammaCode(u16panel_max_luma, u16source_max_luma);
     u16linear_out_code = 0xFFFC;
     u16in_thrd = min((u16linear_in_code * 0xFF8) / 0xFFFC, 0xFF8);
     u16out_thrd = min((u16linear_out_code * 0xFF8) / 0xFFFC, 0xFF8);
     for (u16idx = 0; u16idx <u16tmo_size; u16idx++)
     {
         u16input = (u16idx * 8);// 4088(0xFF8) as 1;
         u16input_code = u16input;
         //u16code_in[u16idx] = u16input_code;

         u16output = (u16input_code*u16out_thrd) / max(u16in_thrd, 1); //output 0xFF8 as 1
         u16output = min(u16output, 0xFF8);

         //u16tmo_lut[u16idx] = min(((u16output * 4095) / 4088), 4095);
         u16curve512[u16idx] = min(((u16output * 4095) / 4088), 4095);

     }

#if 0
    //printk("debug:u16tmo_lut[320] : %d\n",i,u16curve512[320] );
    //printk("debug:u16tmo_lut[384] : %d\n",i,u16curve512[384] );
    //printk("debug:u16tmo_lut[448] : %d\n",i,u16curve512[448] );
    //printk("debug:u16tmo_lut[511] : %d\n",i,u16curve512[511] );

    for (i = 0; i < 512; i=i+32)
    {
        //if ((i%32) == 0)
        {
            printk("debug:u16tmo_lut[%d] : %d\n",i,u16curve512[i] );
        }
    }

#if 0
    for (i = 0; i < 16; i++)
    {
    //if ((i%16) == 0)
        {
            printk("debug:u16tmo_lut[%d] : %d\n",i,u16tmo_lut[i] );
        }
    }
#endif
#endif

    //writeMdTmo(u16tmo_lut, 0, u16source_max_luma, u16panel_max_luma);
    writeMdTmo(u16curve512, 0, u16source_max_luma, u16panel_max_luma);

    kfree((void *)u16curve512);
#endif
}
void msOldTMO(MS_BOOL bWindow)
{

    // set TMO function parameters
    HDRtmp.slope = 1582;
    HDRtmp.rolloff = 12288;

    if((_u16Count%300)==0)
    {
        //printk("\n  Kernel HDRAdjustMode=%x ; smin=%x , smax=%x ,tmax=%x ,tmin=%x ,smed=%x ,tmed=%x    \n",HDRAdjustMode,HDRtmp.smin,HDRtmp.smax,HDRtmp.tmax,HDRtmp.tmin,HDRtmp.smed,HDRtmp.tmed);
    }
    // 2084 format

    MDrv_HDR_ToneMappingCurveGen_Mod(HDR_Curve,&HDRtmp,bWindow);

    //fire for ac gain lut updation
    KApi_XC_MLoad_Fire(E_CLIENT_MAIN_HDR, TRUE);

}
extern MS_BOOL bTmoFireEnable;

void msDlcHandler(MS_BOOL bWindow)
{
    MS_BOOL bUseDlc = TRUE;
    if(g_DlcParameters.u8Dlc_Mode == 1)
    {
        //msNewDlcHandler_WithCorrectionRatio(bWindow);
        if (g_DlcParameters.ucKernelDlcAlgorithm == 0)
        {
            msOldDlcHandler(bWindow);
        }
        else if (g_DlcParameters.ucKernelDlcAlgorithm == 1)
        {
            //msNewDlcHandler_WithCorrectionRatio(bWindow);
            //MHalPqDlcHandler(bWindow);
        }
    }
    else if(g_DlcParameters.u8Dlc_Mode == 0)
    {
        msDLC_linearCurve(bWindow);
    }
    else
    {
        //manual mode
        msOldDlcHandler(bWindow);
    }

    _u16Count++;
    if(_u16Count>=301)
    {
        _u16Count=0;
    }

}

void msTmoHandler(MS_BOOL bWindow)
{
#if defined(CONFIG_MSTAR_XC_HDR_SUPPORT)
    MS_BOOL bUseDlc = TRUE;
    MS_U8 u8test;
    MS_U16 u16IRQValue = MApi_GFLIP_XC_R2BYTE(REG_SC_BK00_10_L);
    /*
    u8test =  GetDlcFunctionControl();
    if(1 == u8test)
    {
        Color_Format_Driver();
        msWriteByte(REG_SC_BK30_0D_L, 0x00);
        KApi_XC_MLoad_Fire(E_CLIENT_MAIN_HDR, TRUE);
    }
    */
    //u8log = LogPrintfOneTimes();
    if(1 == u8log)
    {
        msTMOinit();
    }
#if 0
    if(MApi_GFLIP_XC_R2BYTE(REG_SC_BK30_01_L) == 0x001C)
    {
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_Algorithm = msReadByte(REG_SC_Ali_BK30_0D_L);
    }
#endif

    //g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_Algorithm = 0;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_targetconfigs_source = 1;

#if  (TMO_STB ==TMO_DETAILS)

#if 0
        if (E_CFIO_MODE_HDR2 == pstu_Control_Param->u8Temp_HDRMode[0]) //PQ uses TMO configures from CFD values
        {
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode = 1;
        }
        else if (E_CFIO_MODE_HDR3 == pstu_Control_Param->u8Temp_HDRMode[0]) //HLG uses TMO configures in TMO
        {
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode = 2;
        }
        else //use TMO default values
        {
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode = 0;
        }
#endif
    //printk("\033[1;35m###[Brian][%s][%d]### u8TMO_TargetMode = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode);

    //printk("u8TMO_TargetMode = %d\n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode);

#if 1
       if (g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode == 2) //HLG
       {
           g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_Algorithm = 3;
           //printk("\033[1;35m###[Brian][%s][%d]### u8TMO_Algorithm = 3 !!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__);
       }
       else //PQ and others
       {
           g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_Algorithm = 0;
            //printk("\033[1;35m###[Brian][%s][%d]### u8TMO_Algorithm = 0 !!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__);
       }
#endif

#else
#endif


    if ((u16IRQValue & BIT(5)) && bTmoFireEnable){
        if(g_DlcParameters.u8Dlc_Mode == 2 || g_DlcParameters.u8Tmo_Mode == 1)
        {
            //msNewTMO(bWindow);
            if (g_HDRinitParameters.DLC_HDRCustomerDlcCurve.bFixHdrCurve == TRUE &&
                g_HDRinitParameters.DLC_HDRCustomerDlcCurve.pucDlcCurve != NULL &&
                g_HDRinitParameters.DLC_HDRCustomerDlcCurve.u16DlcCurveSize == DLC_CURVE_SIZE) //use customer fix DLC curve.
            {
                //bUseDlc = FALSE;
                msHDRFixDlcWriteCurve(bWindow);//HDR Fix DLC
            }
            else
            {
                if (g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_Algorithm == 2)
                {
                    msReferTMO();
                    g_DlcParameters.u8Dlc_Mode = 1;
                }
                else if (g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_Algorithm == 1)
                {
                    msNewTMO(bWindow);
                    g_DlcParameters.u8Dlc_Mode = 0;
                }
                else if(g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_Algorithm == 3)
                {
                    //printk("\033[1;35m###[Brian][%s][%d]### msLinearTMO !!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__);
                    msLinearTMO();
                    g_DlcParameters.u8Dlc_Mode = 1;
                }
                else
                {
                    //printk("\033[1;35m###[Brian][%s][%d]### msOldTMO !!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__);
                    g_DlcParameters.u8Dlc_Mode = 1;
                    msOldTMO(bWindow);

                }
                RefernceTMOSetting(g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_Algorithm);
                #if 0
                //Move below ADL fire to if(stCFD_API_Ctrl.u8AutodownloadFirePermission)
                //Do not binding ADL fire and TMO calc
                if(1 == MHal_XC_FireAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR))
                {
                    //printk("M+D TMO AutoDownload successful!\n");
                }
                else
                {
                    printk("M+D Gamma AutoDownload FAILED!\n");
                }
                _bAdlFired = TRUE;
                #endif
            }
        }
    }

    if (u16IRQValue & BIT(5))
    {
        STU_CFDAPI_HAL_CONTROL stCFD_API_Ctrl;
        Mhal_Cfd_Hal_Control_Get(&stCFD_API_Ctrl);
        if(stCFD_API_Ctrl.u8AutodownloadFirePermission)
        {
            if(1 == MHal_XC_FireAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR))
            {
                //printk("M+D TMO AutoDownload successful!\n");
            }
            else
            {
                printk("M+D Gamma AutoDownload FAILED!\n");
            }
            _bAdlFired = TRUE;

            if(MHal_XC_GetVersion() == 0xFF)
            {
                Mhal_Cfd_Hal_SetAutodownloadFirePermission(0);

                MHal_XC_ResetADL();
            }
        }
    }
#else
    //msOldDlcHandler(bWindow);
#endif
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
            MS_U8 u8Mode = 0;

            if (g_DlcParameters.ucKernelDlcAlgorithm == 0)
            {
                u8Mode = 1;
            }
            else if (g_DlcParameters.ucKernelDlcAlgorithm == 1)   //RD DLC
            {
                u8Mode = 0;
            }

            //u8Mode = (msReadByte(REG_SC_BK30_0F_L)>>1)&0x01;
            if(0 == u8Mode)
            {
                //g_uwTable[ucTmp] = (uwBlend*876/1023) + (16<<2); //Full to Limit Range
                g_uwTable[ucTmp] = uwBlend;
            }
            else
            {
                g_uwTable[ucTmp] = uwBlend;
            }

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
    static const MS_U32 log_lut[385] =
    {
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
    static const MS_U16 exp_lut[385] =
    {
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

MS_BOOL msTMOSet(MS_U16 *u16TMOData , MS_U16 u16TMOSize)
{
    MS_U16 i = 0;

    //printk("\033[1;35m###[Brian][%s][%d]### msTMOSet!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__);

    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SrcMinRatio = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SrcMedRatio = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SrcMaxRatio = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMin = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMed = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMax = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16FrontSlopeMin = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16FrontSlopeMax = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16BackSlopeMin = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16BackSlopeMax = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SceneChangeThrd = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SceneChangeRatioMax = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u8IIRRatio = u16TMOData[i++];

    //u8TMO_TargetMode
    //not assgin this value by UI
    //this value is assigned by CFD
    //g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode = u16TMOData[i++];
    i++;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SDRPanelGain = u16TMOData[i++];

    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smin = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smed = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smax = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmed = u16TMOData[i++];
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax = u16TMOData[i++];

    //use 3 points to 3 points TMO, this value is not changed now
    //g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_Algorithm = 0;

    //0: from ini file
    //1: TMO target configs from CFD
    //g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_targetconfigs_source = 1;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_intoTMOset = 1;

    //printk("\n  Kernel DLC_HDRNewToneMappingData.u16Tmin %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin );
    //printk("\n  Kernel DLC_HDRNewToneMappingData.u16Tmed %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmed );
    //printk("\n  Kernel DLC_HDRNewToneMappingData.u16Tmax %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax );
    //printk("\n  Kernel DLC_HDRNewToneMappingData.u16Smin %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smin );
    //printk("\n  Kernel DLC_HDRNewToneMappingData.u16Smed %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smed );
    //printk("\n  Kernel DLC_HDRNewToneMappingData.u16Smax %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smax );

    //use full range
    //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smin = 150;
    //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smed = 250;
    //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smax = 10000;
    //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin = 10000;
    //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmed = 600;
    //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax = 1500;

    //MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK2E_25_L, 0x0001, 0x0001);
    //MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK4E_21_L, g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smin, 0xFFFF);
    //MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK4E_22_L, g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smed, 0xFFFF);
    //MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK4E_23_L, g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smax, 0xFFFF);
    //MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK4E_25_L, g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin, 0xFFFF);
    //MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK4E_26_L, g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmed, 0xFFFF);
    //MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK4E_27_L, g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax, 0xFFFF);
    //MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK4E_24_L, g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SDRPanelGain, 0xFFFF);

    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRNewToneMappingData.u16SrcMinRatio %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SrcMinRatio ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRNewToneMappingData.u16SrcMedRatio %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SrcMedRatio ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRNewToneMappingData.u16SrcMaxRatio %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SrcMaxRatio ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRNewToneMappingData.u16TgtMin %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMin ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRNewToneMappingData.u16TgtMed %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMed ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRNewToneMappingData.u16TgtMax %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMax ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRNewToneMappingData.u16FrontSlopeMin %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16FrontSlopeMin ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRNewToneMappingData.u16FrontSlopeMax %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16FrontSlopeMax ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRNewToneMappingData.u16BackSlopeMin %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16BackSlopeMin ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRNewToneMappingData.u16BackSlopeMax %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16BackSlopeMax ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRNewToneMappingData.u16SceneChangeThrd %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SceneChangeThrd ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRNewToneMappingData.u16SceneChangeRatioMax %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SceneChangeRatioMax ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRNewToneMappingData.u8IIRRatio %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u8IIRRatio ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRNewToneMappingData.u8TMO_TargetMode %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode ));
    HDR_DBG_HAL(DLC_DEBUG("\n  Kernel DLC_HDRNewToneMappingData.u8TMO_Algorithm %x \n", g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_Algorithm ));

    return TRUE ;
}

MS_U16 MDrv_HDR_Exp(MS_U16 data_base, MS_U16 data_exp,MS_U8 u8Mode)
{
    if(0 == u8Mode)
{
    return MDrv_HDR_Pow(data_base << 6, data_exp);
}
    else if(2 == u8Mode)
   {
        return MDrv_HDR_Pow(data_base , data_exp);
   }
    else
    {
        return MDrv_HDR_Pow(data_base << 4, data_exp);
    }
}
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
MS_U32 MDrv_HDR_Exp2(MS_U16 data_base, MS_U32 data_exp, MS_U8 u8Mode)
{

    if (0 == u8Mode)
    {
        return (pow01_16In32Out(data_base << 6, data_exp));
    }
    else if (1 == u8Mode)
    {
        return (pow01_16In32Out(data_base << 4, data_exp));
    }
    else
    {
        return (pow01_16In32Out(data_base, data_exp));
    }

}
#endif
void MDrv_HDR_ToneMappingCurveGen_Mod(MS_U16* pCurve18, HDR_ToneMappingData* pData, MS_BOOL bWindow)
{
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT

    MS_U16 smin, smed, smax;
    MS_U16 tmin, tmed, tmax;
    MS_S64 s1, s2, s3;
    MS_S64 t1, t2, t3;
    MS_S64 norm;
    MS_S64 c1, c2, c3;
    MS_S64 term, nume, deno, base;
    MS_U16 dlc_in, dlc_out, i;
    const MS_U16 dlc_pivots[18] = {0, 0x20, 0x60, 0xA0, 0xE0, 0x120, 0x160, 0x1A0, 0x1E0, 0x220, 0x260, 0x2A0, 0x2E0, 0x320, 0x360, 0x3A0, 0x3E0, 0x3FF};
    MS_U32 u32nits2Gamma[6] = { 0 };
    MS_U32 u32Temp3, data_out, u32nit[6] = { 0 };
    MS_U16 u16SourceUpper, u16SourceLower, u16TargetUpper, u16TargeLower;
    MS_U16 *u16curve512 = (MS_U16*)kmalloc(512*sizeof(MS_U16),GFP_KERNEL);

    // modify smed/tmed parameter
    MS_U8 u8UpperBound = 80, u8lowerBound = 1;
    MS_U8 u8SourceUpperBound = 80, u8SourcelowerBound = 1;

    ////Gamma 2.2 parameter start////
    MS_U32 u32Gamma = 0x745D, u32GammaInv = 0x23333;
    MS_U16 u16Alpha = 0x8000, u16Beta = 1, u16LowerBoundGain = 0x0100;
    MS_U8 u8BitDepth = 16, sMode = 0;
    ////Gamma 2.2 parameter End///

    MS_U8 u8byte2 = 0;
    MS_U16 u16nit[6] = {0};

#if  (TMO_STB ==TMO_DETAILS)

    //MS_U32
    //MS_U32

#endif

    // anti-dumbness

    //smin = pData->smin;
    //smed = pData->smed = 438;
    //smax = pData->smax =  920;
    //tmin = pData->tmin;//54;
    //tmed = pData->tmed;//485;
    //tmax = pData->tmax;//712;
    //u8Mode = MApi_GFLIP_XC_R2BYTE(REG_SC_BK2E_25_L)&0x01;
    //hard code for curry
    //u8Mode = 1;

    //test code

#if 0
    MS_U8 u8byte0 = 0;
    MS_U8 u8byte1 = 0;
    MS_U8 u8byte2 = 0;
    MS_U8 u8byte3 = 0;

#if RealChip
//dummy in SOC
//if(MApi_GFLIP_XC_R2BYTE(REG_SC_BK30_01_L) == 0x001C)
//{
//u8TestMode = msReadByte(REG_SC_Ali_BK30_0D_H);
//}
//u8byte1 = msReadByte( _PK_H_(0x79, 0x7c));
//u8byte0 = msReadByte( _PK_L_(0x79, 0x7c));
//u8byte3 = msReadByte( _PK_H_(0x79, 0x7d));
u8byte2 = msReadByte( _PK_L_(0x79, 0x7d));

       printk("\n test : u8byte2 = %d\n",u8byte2);
       Mhal_Cfd_Main_Predefined_Control_Set(u8byte2);

       printk("\n test2 : _stu_CFD_Main_Control.u8PredefinedProcess = %d\n",_stu_CFD_Main_Control.u8PredefinedProcess);

//assign
//link to UI
//stu_CfdAPI_Top_Param.pstu_Main_Control->u8TMO_TargetRefer_Mode = u8byte1&0x03;

#endif
#endif

#if 0
    u32nit[0] = MApi_GFLIP_XC_R2BYTE(REG_SC_BK4E_21_L) ;//smin
    u32nit[1] = MApi_GFLIP_XC_R2BYTE(REG_SC_BK4E_22_L) ;//smed
    u32nit[2] = MApi_GFLIP_XC_R2BYTE(REG_SC_BK4E_23_L) ;//smax
    u32nit[3] = MApi_GFLIP_XC_R2BYTE(REG_SC_BK4E_25_L) ;//tmin
    u32nit[4] = MApi_GFLIP_XC_R2BYTE(REG_SC_BK4E_26_L) ;//tmed
    u32nit[5] = MApi_GFLIP_XC_R2BYTE(REG_SC_BK4E_27_L) ;//tmax
#endif

//default values from ini

#if 0
 u8byte2 = msReadByte( _PK_L_(0x79, 0x7d));


    u32nit[0] = 1024;//smin
    u32nit[1] = 3072;//smed
    u32nit[2] = 10000;//smax

if (u8byte2 == 0)
    {
    u32nit[3] = 256;//tmin
    u32nit[4] = 544;//tmed
    u32nit[5] = 768;//tmax
    }
else if (u8byte2 == 1)
    {
    u32nit[3] = 256;//tmin
    u32nit[4] = 576;//tmed
    u32nit[5] = 768;//tmax
    }
else //u8byte2 == 2
    {
    u32nit[3] = 256;//tmin
    u32nit[4] = 640;//tmed
    u32nit[5] = 768;//tmax
    }
#endif

#if 0
u32nit[0] = 1024;//smin
u32nit[1] = 3072;//smed
u32nit[2] = 10000;//smax
u32nit[3] = 256;//tmin
u32nit[4] = 576;//tmed
u32nit[5] = 768;//tmax
#else

//TMO parameters from CFD
if (1 == g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_targetconfigs_source)
{
    u32nit[0] = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smin;//smin
    u32nit[1] = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smed;//smed
    u32nit[2] = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smax;//smax
    u32nit[3] = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin;//tmin
    u32nit[4] = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmed;//tmed
    u32nit[5] = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax;//tmax
}
//TMO parameters from ini
else
{
    if (1 == g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_intoTMOset)
    {
        u32nit[0] = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smin;//smin
        u32nit[1] = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smed;//smed
        u32nit[2] = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smax;//smax
        u32nit[3] = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin;//tmin
        u32nit[4] = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmed;//tmed
        u32nit[5] = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax;//tmax
    }
    else
    {
        u32nit[0] = 100;    //smin
        u32nit[1] = 400;    //smed
        u32nit[2] = 10000;//smax
        u32nit[3] = 100;//tmin
        u32nit[4] = 400;//tmed
        u32nit[5] = 768;//tmax
    }
}
#endif

//constant values
u32nit[2] = 10000;
//u32nit[3] = 100;


// this values is assigned by TMO
//printk("u8TMO_targetconfigs_source: %d \n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_targetconfigs_source);

// this values is assigned by TMO and UI
//printk("u8TMO_intoTMOset: %d \n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_intoTMOset);

// this values is assigned by CFD
//printk("u8TMO_TargetMode: %d \n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode);

//printk("u8TgtMinFlag: %d \n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag);


#if  (TMO_STB ==TMO_DETAILS)

    //for PQ
    if ((1 == g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode) && (1 == g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_targetconfigs_source))
    {

        //u16nit[3] = (((MS_U32)g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin*u8Ratio)>>6);
        //u8TarMinFlag = g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag;
        //u16nit[4] = (((MS_U32)240*u8Ratio)>>6);
        //u16nit[5] = (((MS_U32)g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax*u8Ratio)>>6);

    if (0 == g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag)
    {
            u32nit[3] = 10000;
    }
    else
    {
         //u32nit[3] = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin;
    }

    if (g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax > u32nit[4])
    {
        u32nit[5] =  g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax;
    }

    }
    //HLG
    else if (2  == g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode) //for HLG
    {

#if 1
        u32nit[0] = 0;//smin
        u32nit[1] = 40;//smed
        u32nit[2] = 7040;//smax
        u32nit[3] = 0;//tmin
        u32nit[4] = 53;//tmed
        u32nit[5] = 10000;//tmax
#else
        u32nit[0] = 0;//smin
        u32nit[1] = 576;//smed
        u32nit[2] = 10000;//smax
        u32nit[3] = 0;//tmin
        u32nit[4] = 576;//tmed
        u32nit[5] = 10000;//tmax
#endif
    }

#endif

#if 0
    MS_U8 u8bytea = 0;
    MS_U8 u8byteb = 0;
    MS_U8 u8bytec = 0;
    MS_U8 u8byted = 0;

    u8byteb= msReadByte( _PK_H_(0x79, 0x7c));
    u8bytea = msReadByte( _PK_L_(0x79, 0x7c));
    u8byted = msReadByte( _PK_H_(0x79, 0x7d));
    u8bytec = msReadByte( _PK_L_(0x79, 0x7d));


    u32nit[1] = u8bytea*10;
    u32nit[4] = u32nit[1];

    u32nit[5] = u8byteb*10;

//u32nit[1] = 50;
//u32nit[4] = 50;

//u32nit[0] = ((MS_U32)u8bytea)<<4;
//u32nit[3] = ((MS_U32)u8byteb)<<4;

//u32nit[1] = ((MS_U32)u8bytec)<<4;
//u32nit[4] = ((MS_U32)u8byted)<<4;
//u32nit[5] = 600;
//u32nit[0] = 1;

#endif

if (u32nit[1] == 0){
u32nit[1] =1;
}

if (u32nit[2] == 0){
u32nit[2] =1;
}

if (u32nit[4] == 0){
u32nit[4] =1;
}

if (u32nit[5] == 0){
u32nit[5] =1;
}

//printk("TMO source configs: 0:%d,1:%d,2:%d \n",u32nit[0],u32nit[1],u32nit[2]);
//printk("TMO target configs: 3:%d,4:%d,5:%d \n",u32nit[3],u32nit[4],u32nit[5]);

/*
middle_Smin = 1024;
middle_Smed = 3072;
middle_Smax = 10000;
middle_Tmin = 256;
middle_Tmed = 576;
middle_Tmax = 768;
*/

    // anti-dumbness
    // if the registers are all 0, set linear TMO
    if( (u32nit[0] || u32nit[1] || u32nit[2] || u32nit[3] || u32nit[4]  || u32nit[5]) == 0)
        {
        u32nit[0] = 0x80   ;//smin
        u32nit[1] = 0x400  ;//smed
        u32nit[2] = 0x2710 ;//smax
        u32nit[3] = 0x80   ;//tmin
        u32nit[4] = 0x400  ;//tmed
        u32nit[5] = 0x2710 ;//tmax

        //MApi_GFLIP_XC_W2BYTE( REG_SC_BK4E_21_L, u32nit[0]);
        //MApi_GFLIP_XC_W2BYTE( REG_SC_BK4E_22_L, u32nit[1]);
        //MApi_GFLIP_XC_W2BYTE( REG_SC_BK4E_23_L, u32nit[2]);
        //MApi_GFLIP_XC_W2BYTE( REG_SC_BK4E_25_L, u32nit[3]);
        //MApi_GFLIP_XC_W2BYTE( REG_SC_BK4E_26_L, u32nit[4]);
        //MApi_GFLIP_XC_W2BYTE( REG_SC_BK4E_27_L, u32nit[5]);
        }

    // smin/med/max tmin/med/max in nits (0-10000)
    // make sure smed is between u16SourceUpper and u16SourceLower
    // make sure tmed is between u16TargetUpper and u16TargeLower
    smin = u32nit[0] / 10000;
    smed = u32nit[1];
    smax = u32nit[2];
    tmin = u32nit[3] / 10000;
    tmed = u32nit[4];
    tmax = u32nit[5];



    u16SourceUpper = ((smax - smin)*u8UpperBound) / 100 + smin;
    u16SourceLower = ((smax - smin)*u8lowerBound) / 100 + smin;
    u16TargetUpper = ((tmax - tmin)*u8UpperBound) / 100 + tmin;
    u16TargeLower  = ((tmax - tmin)*u8lowerBound) / 100 + tmin;

        //default values from TV
        //u8TMO_TargetMode
        //0: use driver default values
        //1:
        //2:

        //printk("g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode:%d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode);

    // modify smed by (smax-smin) ratio

#if 1

    if (smed > u16SourceUpper)
    {
        u32nit[1] = u16SourceUpper;
    }
    else if (smed< u16SourceLower)
    {
        u32nit[1] = u16SourceLower;
    }

    // modify tmed by (tmax-tmin) ratio
    if (tmed > u16TargetUpper)
    {
        u32nit[4] = u16TargetUpper;
    }
    else if (tmed < u16TargeLower)
    {
        u32nit[4] = u16TargeLower;
    }

#endif

    //printk("TMO source configs: 0:%d,1:%d,2:%d \n",u32nit[0],u32nit[1],u32nit[2]);
    //printk("TMO target configs: 3:%d,4:%d,5:%d \n",u32nit[3],u32nit[4],u32nit[5]);


    // nits to gamma code
    for (i = 0; i<3; i++)
    {
        if (i == 0)
        {
            //min units = 0.00001nits
            u32Temp3 = (MS_U32)(min((u32nit[i] * 0x10000ul / u32nit[2]), 0x10000ul));
            u32nits2Gamma[i] = (MS_U32)Mhal_CFD_GammaSampling(u32Temp3, u32Gamma, u16Alpha, u16Beta, u16LowerBoundGain, u8BitDepth, sMode) * 0x03E4 / 0xFFFF;

            u32Temp3 = (MS_U32)(min((u32nit[i + 3] * 0x10000ul / u32nit[5]), 0x10000ul));
            u32nits2Gamma[i + 3] = (MS_U32)Mhal_CFD_GammaSampling(u32Temp3, u32Gamma, u16Alpha, u16Beta, u16LowerBoundGain, u8BitDepth, sMode) * 0x03E4 / 0xFFFF;
        }
        else
        {
            u32Temp3 = (MS_U32)(min((u32nit[i] * 0x10000ul / u32nit[2]), 0x10000ul));
            u32nits2Gamma[i] = (MS_U32)Mhal_CFD_GammaSampling(u32Temp3, u32Gamma, u16Alpha, u16Beta, u16LowerBoundGain, u8BitDepth, sMode);// * 0xFFFF / 0xFFFC;

            u32Temp3 = (MS_U32)(min((u32nit[i + 3] * 0x10000ul / u32nit[5]), 0x10000ul));
            u32nits2Gamma[i + 3] = (MS_U32)Mhal_CFD_GammaSampling(u32Temp3, u32Gamma, u16Alpha, u16Beta, u16LowerBoundGain, u8BitDepth, sMode);// * 0xFFFF / 0xFFFC;
        }

    }



    s1 = MDrv_HDR_Exp2(u32nits2Gamma[0], (pData->slope) << 4, 2);
    s2 = MDrv_HDR_Exp2(u32nits2Gamma[1], (pData->slope) << 4, 2);
    s3 = MDrv_HDR_Exp2(u32nits2Gamma[2], (pData->slope) << 4, 2);
    t1 = MDrv_HDR_Exp2(u32nits2Gamma[3], (0x1000000 / pData->rolloff) << 4, 2);
    t2 = MDrv_HDR_Exp2(u32nits2Gamma[4], (0x1000000 / pData->rolloff) << 4, 2);
    t3 = MDrv_HDR_Exp2(u32nits2Gamma[5], (0x1000000 / pData->rolloff) << 4, 2);
    s1 = s1 >> 16;  // 32bit >>16 -> 16bit
    s2 = s2 >> 16;
    s3 = s3 >> 16;
    t1 = t1 >> 16;
    t2 = t2 >> 16;
    t3 = t3 >> 16;

    //printk("gamma: 0:%d,1:%d,2:%d \n",u32nits2Gamma[0],u32nits2Gamma[1],u32nits2Gamma[2]);
    //printk("gamma: 3:%d,4:%d,5:%d \n",u32nits2Gamma[3],u32nits2Gamma[4],u32nits2Gamma[5]);
    //printk("s1:%lld,s2:%lld,s3:%lld \n",s1,s2,s3);
    //printk("t1:%lld,t2:%lld,t3:%lld \n",t1,t2,t3);

    //printk("s1:%d,s2:%d,s3:%d \n",s1,s2,s3);
    //printk("t1:%d,t2:%d,t3:%d \n",t1,t2,t3);

    norm = (MS_S64)s3*t3*(s1 - s2) + (MS_S64)s2*t2*(s3 - s1) + (MS_S64)s1*t1*(s2 - s3);         // 48bit
    c1 = (MS_S64)s2*s3*(t2 - t3)*t1 + (MS_S64)s1*s3*(t3 - t1)*t2 + (MS_S64)s1*s2*(t1 - t2)*t3;  // 64bit
    c2 = (MS_S64)t1*(s3*t3 - s2*t2) + (MS_S64)t2*(s1*t1 - s3*t3) + (MS_S64)t3*(s2*t2 - s1*t1);  // 48bit
    c3 = (MS_S64)t1*(s3 - s2) + (MS_S64)t2*(s1 - s3) + (MS_S64)t3*(s2 - s1);                    // 32bit

    //norm = norm > 0 ? (norm >> 16) : -(abs(norm) >> 16);  // 48bit >>16 -> 32bit
    //c1 = c1 > 0 ? (c1 >> 16) : -(abs(c1) >> 16);          // 64bit >>16 -> 48bit
    //c3 = c3 > 0 ? (c3 << 16) : -(abs(c3) << 16);          // 32bit <<16 -> 48bit

    norm = norm > 0 ? (norm >> 16) : -(-1*(norm) >> 16);  // 48bit >>16 -> 32bit
    c1 = c1 > 0 ? (c1 >> 16) :-(-1*(c1) >> 16);          // 64bit >>16 -> 48bit
    c3 = c3 > 0 ? (c3 << 16) :-(-1*(c3) << 16);          // 32bit <<16 -> 48bit

    if (0 != norm)
    {
        c1 = c1 / norm; // 16bit
        c2 = c2 / norm; // 16bit
        c3 = c3 / norm; // 16bit
    }


if (g_DlcParameters.u8Dlc_Mode == 2) //DLC 18point fixed curve
{
    //printk("===========[%s][%d][DLC TMO]===========\n",__FUNCTION__,__LINE__);
    for (i = 0; i < 18; i++)
    {
        dlc_in = dlc_pivots[i];

        term = MDrv_HDR_Exp(dlc_in, pData->slope,0);
        nume = c1 + (c2*term>>16); // s4.12 + s4.12*0.16>>16 = s4.12
        deno = 0x1000 + (c3*term>>16); // s4.12 + s4.12*0.16>>16 = s4.12
        base = (nume<<10) / deno; // (s4.12<<10) / s4.12 = s0.10
        base = (base < 0) ? 0 : (base > 1023) ? 1023 : base;
        dlc_out = MDrv_HDR_Exp((MS_U16)base, pData->rolloff,0);

        pCurve18[i] = dlc_out >> 6;
    }

    pCurve18[0] = (2*pCurve18[0] > pCurve18[1]) ? 2*pCurve18[0] - pCurve18[1] : (pCurve18[1] - 2*pCurve18[0]) | 0x400;
    pCurve18[17] = 2*pCurve18[17] - pCurve18[16];

    _u16Count++;
    if(_u16Count>=300)
    {
        HDR_DBG_HAL(DLC_DEBUG("\n  Kernel [0]=%x ,[1]=%x,[2]=%x,[3]=%x,[4]=%x,[5]=%x,[6]=%x,[7]=%x,[8]=%x   \n",pCurve18[0],pCurve18[1],pCurve18[2],pCurve18[3],pCurve18[4],pCurve18[5],pCurve18[6],pCurve18[7],pCurve18[8]));
        HDR_DBG_HAL(DLC_DEBUG("\n  Kernel [9]=%x ,[10]=%x,[11]=%x,[12]=%x,[13]=%x,[14]=%x,[15]=%x,[16]=%x,[17]=%x,[18]=%x   \n",pCurve18[9],pCurve18[10],pCurve18[11],pCurve18[12],pCurve18[13],pCurve18[14],pCurve18[15],pCurve18[16],pCurve18[17],pCurve18[18]));
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
else
{
    //printk("c1:%lld,c2:%lld,c3:%lld norm:%lld\n",c1,c2,c3,norm);

    MS_U16 u16target_max_lum_nits = tmax;
    for (i = 0; i < 512; i++)
    {
            dlc_in = i * (0xFFFF) / 511;
            term = MDrv_HDR_Exp2(dlc_in, (pData->slope) << 4, 2);
            nume = c1 + (c2*term >> 32); // s4.16 + s4.16*0.32>>32 = s4.16
            deno = 0x10000 + (c3*term >> 32); // s4.16 + s4.16*0.32>>32 = s4.16
            base = (nume << 16) / deno; // (s4.16<<16) / s4.16 = s0.16
            base = (base < 0) ? 0 : (base > 0xFFFF) ? 0xFFFF : base; // limit to 16bit
            data_out = MDrv_HDR_Exp2((MS_U16)base, (pData->rolloff) << 4, 2);
            data_out = min(data_out, 0xFFFFFFFF - 0x80000);  // limit to 32bit before rounding
            u16curve512[i] = (data_out + 0x80000) >> 20; //12bit

            //if ((i == 16) ||(i == 32))
            //{
            //printk("debug: i : %d , dlc_in : %d,  term: %d, nume:%d, base:%d\n",i,dlc_in, term,nume, base);
            //}

    }

#if 0
    for (i = 0; i < 512; i++)
    {
        if ((i%16) == 0)
        {
            printk("debug:u16curve512[%d] : %d\n",i,u16curve512[i] );
        }
    }

#if 0
    for (i = 0; i < 16; i++)
    {
    //if ((i%16) == 0)
       {
           printk("debug:u16curve512[%d] : %d\n",i,u16curve512[i] );
       }
    }
#endif
#endif

    writeMdTmo(u16curve512, 0, u32nit[2], u32nit[5]);
}
    kfree((void *)u16curve512);

#endif
}

MS_U32 MDrv_HDR_PQ(MS_U16 u16In)
{
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT

    MS_U16 u16Shift, u16Idx, u16Lsb, u16Tmp, u16Exp;
    MS_U32 u32Lut0_out, u32Lut1_out, u32Result;
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
#else
    return 0;
#endif
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
    MS_U16 u16WriteCount, u16SpLevel, u16SpAddr[4]= {0};
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
        while(MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK2D_60_L, BIT(3))&& (--u16Delay))
        {
            ;
        }

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
    MS_U16 u16Val = 255, u16NumEntries=255, u16LutIdx=255, u16Index=255;
    MS_U64 u64Nume=255, u64Denum=255;

    // u16A : 0.16
    // u16B : 0.16
    // u16S : 6.10
    // u16Gamma : 4.12
    // u16Ginv  : 4.12
    MS_U16 u16Gamma = 0x8CCD , u16A = 36012, u16B= 0x49C, u16S= 0x1200, u16Ginv= 0x1000000ul;
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

#if defined(CONFIG_ARM64)
void msDlcSetPMBase(MS_U64 u64PMBase)
{
    g_u64MstarPmBase=u64PMBase;
}
#endif


//////////////GetBiHistogram//////////////////////////
// Generate bi-histogram curve
// Input:
//   dwSumBelowAvg: counts below the average.
//   dwSumAboveAvg: counts above the average.
//   ucYAvgNormalize: The cut position.
//   uwHistogramAreaSum[32]: histogram cumulate sum.
// Output:
//   uwHistogramAreaSum[32]: bi-histogram curve
////////////////////////////////////////////////////////
void GetBiHistogram(const DWORD dwSumBelowAvg, const DWORD dwSumAboveAvg, const BYTE ucYAvgN, DWORD *dwHistogramAreaSum, DWORD *dwDLCCurve)
{
    int ucTmp;
    DWORD scaler;
    DWORD Yseparate = ucYAvgN*1024/255;
    // Bi-Histogram, rescale the histogram equalization to the separate point
    if (dwSumBelowAvg==0 || dwSumAboveAvg==0)
    {
        for (ucTmp = 0; ucTmp < 32; ucTmp++)
        {
            dwHistogramAreaSum[ucTmp] = (ucTmp+1)*8*4;//10 bit
        }
        return;
    }
    for (ucTmp = 0; ucTmp < 32; ucTmp++)
    {
        if (ucYAvgN >= (((WORD)ucTmp+1)*8))  // points at the left side of StepPoint
        {
            scaler = ((DWORD)dwHistogramAreaSum[ucTmp]*256+(dwSumBelowAvg/2))/dwSumBelowAvg;
            dwHistogramAreaSum[ucTmp] = (Yseparate*scaler)/256;
        }
        else
        {
            scaler = (((DWORD)dwHistogramAreaSum[ucTmp]-dwSumBelowAvg)*256+dwSumAboveAvg/2)/dwSumAboveAvg;
            dwHistogramAreaSum[ucTmp] = Yseparate + ((1024-Yseparate)*scaler)/256;
        }
    }

    //down sampling to 16 numbers (store to dwHistogramAreaSum[0~15])
    for (ucTmp=0; ucTmp<16; ucTmp++)
    {
        dwDLCCurve[ucTmp] = dwHistogramAreaSum[ucTmp*2]; //
    }
    // End of Bi-Histogram
}


//////////////Get_DLCInfo_from_Histogram//////////////////////////
// From the histogram, get all information needed latter
// Input:
//   ucYAvgN.
//   g_wLumaHistogram32H_reg: histogram registers
// Output:
//   uwHistogramAreaSum[32]: accumulate result
//   dwSumBelowAvg: the pixel count at the left side of the YAvg
//   dwSumAboveAvg: the pixel count at the right side of the YAvg
//   dwYLftAvg: left side( separated by the global YAVG) of the YAvg
//   dwYRtAvg: right side of the YAvg
////////////////////////////////////////////////////////
int Get_DLCInfo_from_Histogram(/*IN*/const BYTE ucYAvgN, WORD *g_wLumaHistogram32H_reg,/*OUT*/DWORD *dwHistogramAreaSum,
                                     DWORD *dwSumBelowAvg,DWORD *dwSumAboveAvg, DWORD *dwYLftAvg, DWORD *dwYRtAvg)
{
    DWORD YSum, YCount;
    BYTE ucTmp;
    // Calculate left and and right mean separated by the YAvg;
    dwHistogramAreaSum[0]=g_wLumaHistogram32H_reg[0];
    for (ucTmp=1; ucTmp<32; ucTmp++)
    {
        dwHistogramAreaSum[ucTmp] = dwHistogramAreaSum[ucTmp-1] + g_wLumaHistogram32H_reg[ucTmp];
    }

    if(dwHistogramAreaSum[31]==0)
    {
        HDR_DBG_HAL(DLC_DEBUG("\n Error in msDlcHandler(), dwHistogramAreaSum[31] = 0"));
        return STATUS_ERROR;
    }

    YSum = YCount = 0;
    for (ucTmp=0; ucTmp<=ucYAvgN/8; ucTmp++)
    {
        if (ucTmp == ucYAvgN/8)
        {
            YSum += (ucTmp*8+4) * ((g_wLumaHistogram32H_reg[ucTmp]*(ucYAvgN-ucTmp*8)+4)/8);
            YCount+=(g_wLumaHistogram32H_reg[ucTmp]*(ucYAvgN-ucTmp*8)+4)/8;
        }
        else
        {
            YSum += (ucTmp*8+4) * g_wLumaHistogram32H_reg[ucTmp];
            YCount+=g_wLumaHistogram32H_reg[ucTmp];
        }
    }
    *dwSumBelowAvg = YCount;
    if(YCount==0)
    {
        *dwYLftAvg = YSum;
    }
    else
    {
        *dwYLftAvg = YSum/YCount;
    }
    YSum = YCount = 0;
    for (ucTmp=ucYAvgN/8; ucTmp<32; ucTmp++)
    {
        if (ucTmp == ucYAvgN/8)
        {
            YSum += (ucTmp*8+4) * ((g_wLumaHistogram32H_reg[ucTmp]*(8-(ucYAvgN-ucTmp*8))+4)/8);
            YCount+=((g_wLumaHistogram32H_reg[ucTmp]*(8-(ucYAvgN-ucTmp*8))+4)/8);
        }
        else
        {
            YSum += (ucTmp*8+4) * g_wLumaHistogram32H_reg[ucTmp];
            YCount+=g_wLumaHistogram32H_reg[ucTmp];
        }
    }
    *dwSumAboveAvg = YCount;
    if(YCount==0)
    {
        *dwYRtAvg = YSum;
    }
    else
    {
        *dwYRtAvg = YSum/YCount;
    }
    return STATUS_SUCCESS;
}


///////////////////////////////msDlcGetStrengthScore////////////////////////////////////////////
// From the 1st cut position and histogram cumulate sum, calculate 2nd and 3rd cut position.
// If 2nd and 3rd cut diff too much, reduce the strength, otherwise, keep the strength.
// Input:
//    dwHistogramAreaSum[16]: The 16 bins cumulated histogram
//    ucSepPoint: The 1st cut
// Output:
//    *ucBelowScore
//    *ucAboveScore : keep ?? % of curve correction.
/////////////////////////////////////////////////////////////////////////////////////////////////
void msDlcGetStrengthScore(const DWORD *dwDLCCurve, const BYTE ucSepPoint, const DWORD dwYLftAvg, const DWORD dwYRtAvg, WORD *uwBelowScore, WORD *uwAboveScore)
{
    BYTE SecondCut=0, ThirdCut=0;
    WORD uwLetfDiffRatio=0, uwRightDiffRatio=0;

    //To prevent div by 0.
    //ucTmp++ if ((8+ucTmp*16)>=(BYTE)dwYLftAvg) SecondCut=ucTmp
    SecondCut = (MS_U8)(dwYLftAvg+15-8)/16;
    // DiffRatio = Diff/Total *128
    uwLetfDiffRatio = (WORD)(abs((int)dwDLCCurve[SecondCut]-(int)(8+16*SecondCut)*4)*256 / max(dwDLCCurve[ucSepPoint],1));

    if (uwLetfDiffRatio>g_DlcParameters.ucLMaxThreshold) //Diff too much, ucBelowScore small
        *uwBelowScore = g_DlcParameters.ucLMinCorrection;
    else if (uwLetfDiffRatio<g_DlcParameters.ucLMinThreshold) //Diff is small, ucBelowScore big
        *uwBelowScore = g_DlcParameters.ucLMaxCorrection;
    else
        *uwBelowScore = (MS_U16)((g_DlcParameters.ucLMaxThreshold-uwLetfDiffRatio)*g_DlcParameters.ucLMaxCorrection +
                                 (uwLetfDiffRatio-g_DlcParameters.ucLMinThreshold)*g_DlcParameters.ucLMinCorrection  ) /
                        max((int)g_DlcParameters.ucLMaxThreshold-(int)g_DlcParameters.ucLMinThreshold,1);



    //ucTmp++ if ((8+ucTmp*16)>=(BYTE)dwYRtAvg) ThirdCut=ucTmp
    ThirdCut = (MS_U8)(dwYRtAvg+15-8)/16;
    // DiffRatio = Diff/Total *128
    uwRightDiffRatio = (WORD)(abs((int)dwDLCCurve[ThirdCut]-(int)(8+16*ThirdCut)*4)*256 / max(dwDLCCurve[15]-dwDLCCurve[ucSepPoint],1));

    if (uwRightDiffRatio>g_DlcParameters.ucRMaxThreshold) //Diff too much, choice a small correction
        *uwAboveScore = g_DlcParameters.ucRMinCorrection;
    else if (uwRightDiffRatio<g_DlcParameters.ucRMinThreshold) //Diff is small, choice the maximum correction
        *uwAboveScore = g_DlcParameters.ucRMaxCorrection;
    else
        *uwAboveScore = (MS_U16)((g_DlcParameters.ucRMaxThreshold-uwRightDiffRatio)*g_DlcParameters.ucRMaxCorrection +
                                 (uwRightDiffRatio-g_DlcParameters.ucRMinThreshold)*g_DlcParameters.ucRMinCorrection  ) /
                        max((int)g_DlcParameters.ucRMaxThreshold-(int)g_DlcParameters.ucRMinThreshold,1);

}


///////////////////////////////GetLeftRightMinSlop////////////////////////////////////////////
// From the given limit slop, fin the left and right minimum correction.
// Input:
//    ucSepPoint
//    dwDLCCurve
// Output:
//    *uwHistogramSlopRatioL
//    *uwHistogramSlopRatioH
/////////////////////////////////////////////////////////////////////////////////////////////////
void GetLeftRightMinSlop(const BYTE ucSepPoint, DWORD *dwDLCCurve,WORD *uwHistogramSlopRatioL, WORD *uwHistogramSlopRatioH)
{
    BYTE ucTmp;
    DWORD dwHistogramDlcSlop[17];

    //histogram_DLC limit by slope
    for (ucTmp=0; ucTmp<=16; ucTmp++)
    {
        if (ucTmp == 0)
        {
            dwHistogramDlcSlop[ucTmp] = ((dwDLCCurve[ucTmp] - 0) * 256 + 16) / 32;
        }
        else if (ucTmp == 16)
        {
            dwHistogramDlcSlop[ucTmp] = ((1024 - dwDLCCurve[ucTmp-1]) * 256 + 16) / 32;
        }
        else
        {
            // slop = vertical diff / horizontal size 64 -> 8 bit floating point(*256)
            dwHistogramDlcSlop[ucTmp] = ((dwDLCCurve[ucTmp] - dwDLCCurve[ucTmp-1]) * 256 + 32) / 64;
        }
        dwHistogramDlcSlop[ucTmp] = max(dwHistogramDlcSlop[ucTmp],257); // To prevent div by 0
        // calculate all the correction parameter "dwHistogramDlcSlop" if over limit slop
        // limit slop = 7 bit floating point. 0 = 1x, 128 = 2x, maximum 256.
        dwHistogramDlcSlop[ucTmp] = ((g_DlcParameters.ucDlcHistogramLimitCurve[ucTmp]*2*256)+((dwHistogramDlcSlop[ucTmp]-256)/2)) / (dwHistogramDlcSlop[ucTmp]-256); //SlopRatio 8bit 256=1x
        if(dwHistogramDlcSlop[ucTmp] > 0xFFFF)
        {
            dwHistogramDlcSlop[ucTmp] = 0xFFFF;
        }
    }
    *uwHistogramSlopRatioL = 256;
    *uwHistogramSlopRatioH = 256;
    for (ucTmp = 0; ucTmp<= (ucSepPoint); ucTmp++)
    {
        *uwHistogramSlopRatioL = min((MS_U16)dwHistogramDlcSlop[ucTmp],*uwHistogramSlopRatioL);
    }

    for (ucTmp = ucSepPoint; ucTmp<=16; ucTmp++)
    {
        *uwHistogramSlopRatioH = min((MS_U16)dwHistogramDlcSlop[ucTmp],*uwHistogramSlopRatioH);
    }
}

///////////////////////////////LowPassDLCCurve////////////////////////////////////////////
// low pass the dlc curve.
// Input:
//    *dwDLCCurve
// Output:
//    *dwDLCResult
///////////////////////////////////////////////////////////////////////////////////////////
void LowPassDLCCurve(DWORD *dwDLCCurve, DWORD *dwDLCResult)
{
    BYTE ucTmp;
    for (ucTmp = 0; ucTmp < 16; ucTmp++)
    {
        if (ucTmp == 0)
        {
            dwDLCResult[ucTmp] = (0*2 + 5*dwDLCCurve[ucTmp] + dwDLCCurve[ucTmp+1]+4)/8;
        }
        else if (ucTmp == 15)
        {
            dwDLCResult[ucTmp] = (dwDLCCurve[ucTmp-1] + 5*dwDLCCurve[ucTmp] + 2*1024 + 4)/8;
        }
        else
        {
            dwDLCResult[ucTmp] = (dwDLCCurve[ucTmp-1] + 6*dwDLCCurve[ucTmp] + dwDLCCurve[ucTmp+1]+4)/8;
        }
    }
}

///////////////////////////////LimitBiHistogram////////////////////////////////////////////
// Give a bi-histogram curve, generate the final DLC curve
// Input:
//    ucYAvgN: Yavg
//    dwDLCCurve: bi-histogram result
//    dwYLftAvg: left y average
//    dwYRtAvg: right y average
// Output:
//    *dwDLCResult
///////////////////////////////////////////////////////////////////////////////////////////
void LimitBiHistogram(const BYTE ucYAvgN, DWORD *dwDLCCurve, const DWORD dwYLftAvg, const DWORD dwYRtAvg, DWORD *dwDLCResult)
{
    BYTE ucSepPoint;
    BYTE ucTmp;
    WORD uwLeftCutScore = 128;
    WORD uwRightCutScore = 128;
    WORD uwHistogramSlopRatioL, uwHistogramSlopRatioH;

    ucSepPoint = (ucYAvgN-8)/16;

    //Use Left Yavg and Right YAvg to descide the strength of the DLC curve.
    msDlcGetStrengthScore(dwDLCCurve, ucSepPoint, dwYLftAvg, dwYRtAvg, &uwLeftCutScore, &uwRightCutScore);

    // Use Limited Slop, provided by users, to limit the DLC curve.
    GetLeftRightMinSlop(ucSepPoint, dwDLCCurve, &uwHistogramSlopRatioL, &uwHistogramSlopRatioH);

    // Get the final ratio for DLC Curve.
    uwHistogramSlopRatioL = uwLeftCutScore * uwHistogramSlopRatioL/256;
    uwHistogramSlopRatioH = uwRightCutScore * uwHistogramSlopRatioH/256;
    // calcute final target curve for sepPoint

    for (ucTmp = 0; ucTmp < 16; ucTmp++)
    {
        //printf("New DLC Raw curve %d:%d\n",ucTmp,dwDLCCurve[ucTmp]);
        if (ucSepPoint >= (WORD)ucTmp)
        {
            if(dwDLCCurve[ucTmp]>(DWORD)(ucTmp * 64 + 32))
            {
                if (g_DlcParameters.ucAllowLoseContrast)
                    dwDLCCurve[ucTmp] = (ucTmp * 64 + 32) + (((dwDLCCurve[ucTmp] - (ucTmp * 64 + 32)) * uwHistogramSlopRatioL + 128) / 256 );
                else
                    dwDLCCurve[ucTmp] = (ucTmp * 64 + 32);
            }
            else
            {
                dwDLCCurve[ucTmp] = (ucTmp * 64 + 32) - ((((ucTmp * 64 + 32)- dwDLCCurve[ucTmp]) * uwHistogramSlopRatioL + 128) / 256 );
            }
        }
        else
        {
            if(dwDLCCurve[ucTmp]>(DWORD)(ucTmp * 64 + 32))
            {
                dwDLCCurve[ucTmp] = (ucTmp * 64 + 32) + (((dwDLCCurve[ucTmp] - (ucTmp * 64 + 32)) * uwHistogramSlopRatioH + 128) / 256 );
            }
            else
            {
                if (g_DlcParameters.ucAllowLoseContrast)
                    dwDLCCurve[ucTmp] = (ucTmp * 64 + 32) - ((((ucTmp * 64 + 32)- dwDLCCurve[ucTmp]) * uwHistogramSlopRatioH + 128) / 256 );
                else
                    dwDLCCurve[ucTmp] = (ucTmp * 64 + 32);
            }
        }
    }

    LowPassDLCCurve(dwDLCCurve,dwDLCResult);
}


void GetIIR_AlphaBlending(
    WORD uwYAvg,
    //WORD *g_uwTable,
    BYTE ucYAvgNormalize,
    WORD *uwHistogram_BLE_AreaSum,
    DWORD His_ratio_BelowAvg,
    DWORD His_ratio_AboveAvg
    //BYTE *g_ucDlcFlickAlpha_method2
)
{
    BYTE ucTmp;
    WORD Pre_YAvg_base,Diff_L,Diff_M,Diff_H,Pre_CurveDiff_base;
    WORD YAvg_base=0,CurveDiff=0,CurveDiff_Coring=0,CurveDiff_base=0;
    //    StuDlc_FinetuneParamaters g_DlcParameters;
    static BYTE ucPre_YAvg =0x00;
    //    static WORD g_uwPre_CurveHistogram_method2[16];
    static WORD Pre_g_ucDlcFlickAlpha = 0;
    BYTE g_ucDlcFlickAlpha_Max=0,g_ucDlcFlickAlpha_Min=0,YAvg_TH_H,YAvg_TH_L,g_ucDlcFlickAlpha_temp,ucDlcFlickAlpha_Diff=0;
    WORD  Delta_YAvg=0;

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

    if (uwYAvg*4 <= g_DlcParameters.ucDlcYAvgThresholdL*4)
    {
        YAvg_base = Diff_L*4;
    }
    else if (uwYAvg*4 <= g_DlcParameters.ucDlcYAvgThresholdM*4)
    {
        if(Diff_M >= Diff_L)
        {
            YAvg_base = Diff_L*4 + (((Diff_M-Diff_L)*( uwYAvg*4-g_DlcParameters.ucDlcYAvgThresholdL*4))/(g_DlcParameters.ucDlcYAvgThresholdM- g_DlcParameters.ucDlcYAvgThresholdL));
        }
        else
        {
            YAvg_base = Diff_L*4 - (((Diff_L-Diff_M)*( uwYAvg*4-g_DlcParameters.ucDlcYAvgThresholdL*4))/(g_DlcParameters.ucDlcYAvgThresholdM- g_DlcParameters.ucDlcYAvgThresholdL));
        }
    }
    else if (uwYAvg*4 <= g_DlcParameters.ucDlcYAvgThresholdH*4)
    {
        if(Diff_H >= Diff_M)
        {
            YAvg_base = Diff_M*4 + (((Diff_H- Diff_M)*( uwYAvg*4- g_DlcParameters.ucDlcYAvgThresholdM*4))/(g_DlcParameters.ucDlcYAvgThresholdH- g_DlcParameters.ucDlcYAvgThresholdM));
        }
        else
        {
            YAvg_base = Diff_M*4 - (((Diff_M-Diff_H)*(uwYAvg*4-g_DlcParameters.ucDlcYAvgThresholdM*4))/(g_DlcParameters.ucDlcYAvgThresholdH -g_DlcParameters.ucDlcYAvgThresholdM));
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
        //DLC_DEBUG(printf("\n Error in  [New DLC Algorithm][ %s  , %d ]  YAvg_base=0 !!!\n", __FUNCTION__,__LINE__));
        return;
    }

    CurveDiff = 0;
    CurveDiff_Coring = 0;

    for (ucTmp=0; ucTmp<16; ucTmp++)
    {
        // g_uwTable[ucTmp] = (((WORD)(g_ucCurveYAvg[ucTmp]<<2) * belnging_factor) + ((g_uwCurveHistogram[ucTmp]) * (128 - belnging_factor))) / 128;
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
            //DLC_DEBUG(printf("\n Error in  [New DLC Algorithm][ %s  , %d ]  CurveDiff_base=0 !!!\n", __FUNCTION__,__LINE__));
            return;
        }

        if(ucTmp*16+8 <= ucYAvgNormalize)
        {
            if (ucTmp == 0)
            {
                CurveDiff_Coring = (His_ratio_BelowAvg*(1024-uwHistogram_BLE_AreaSum[1])*abs(g_uwTable[ucTmp]-g_uwPre_CurveHistogram[ucTmp]))/(15*CurveDiff_base*128);
            }
            else
            {
                CurveDiff_Coring = (His_ratio_BelowAvg*(1024-(uwHistogram_BLE_AreaSum[ucTmp*2+1]- uwHistogram_BLE_AreaSum[ucTmp*2-1]))*abs(g_uwTable[ucTmp]-g_uwPre_CurveHistogram[ucTmp]))/(15*CurveDiff_base*128);
            }
        }
        else
        {
            CurveDiff_Coring = (His_ratio_AboveAvg*(1024-(uwHistogram_BLE_AreaSum[ucTmp*2+1]- uwHistogram_BLE_AreaSum[ucTmp*2-1]))*abs(g_uwTable[ucTmp]-g_uwPre_CurveHistogram[ucTmp]))/(15*CurveDiff_base*128);
        }

        CurveDiff_Coring = CurveDiff_Coring/6;

        CurveDiff = CurveDiff + CurveDiff_Coring ;
        g_uwPre_CurveHistogram[ucTmp] = g_uwTable[ucTmp];
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

    Delta_YAvg = (96*4*abs(uwYAvg - ucPre_YAvg)+ YAvg_base/2)/YAvg_base;

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

    ucPre_YAvg = uwYAvg;

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

    g_ucDlcFlickAlpha= ( g_DlcParameters.ucDlcFlicker_alpha*g_ucDlcFlickAlpha_Max + (256- g_DlcParameters.ucDlcFlicker_alpha)*g_ucDlcFlickAlpha_Min+128)/256;

    if ((g_ucDlcFlickAlpha < (5*Pre_g_ucDlcFlickAlpha /8)) && (ucDlcFlickAlpha_Diff < (g_DlcParameters.ucDlcAlphaBlendingMax- g_DlcParameters.ucDlcAlphaBlendingMin)/2))
    {
        g_ucDlcFlickAlpha = 2*(ucDlcFlickAlpha_Diff* g_ucDlcFlickAlpha +(( g_DlcParameters.ucDlcAlphaBlendingMax
                               - g_DlcParameters.ucDlcAlphaBlendingMin)/2- ucDlcFlickAlpha_Diff)* (5*Pre_g_ucDlcFlickAlpha /8))
                            /( g_DlcParameters.ucDlcAlphaBlendingMax- g_DlcParameters.ucDlcAlphaBlendingMin);
    }

    Pre_g_ucDlcFlickAlpha = g_ucDlcFlickAlpha;
    ucDlcFlickAlpha_Diff = g_ucDlcFlickAlpha_Max-g_ucDlcFlickAlpha_Min;

}

void SetBlackWhiteCutPoint(MS_U16 ucYAvg)
{
    MS_U8 uc_cut,uc_lowbound,uc_highbound,i;
    BYTE ucTmp,Tmp;
    BYTE uc_wcut,uc_wlowbound,uc_whighbound;
    WORD uwHistogramForBleSum=0;
    DWORD uwHistogramAreaSum[32];
    WORD BLE_StartPoint=0,BLE_EndPoint=0,BLE_StartPoint_Correction=0,BLE_EndPoint_Correction=0;
    WORD BLE_Slop_400,BLE_Slop_480,BLE_Slop_500,BLE_Slop_600,BLE_Slop_800,Dark_BLE_Slop_Min;
    DWORD BLE_sum=0;
    static WORD Pre_BLE_Slop=0x4000;
    static WORD Pre_BLE_StartPoint_Correction=0xFFF;
    WORD uwBLE_Blend=0,uwBLE_StartPoint_Blend=0;
    DWORD BLE_Slop=0x400,BLE_Slop_tmp=0x400;
    if( g_DlcParameters.bEnableBLE )
    {
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
            DLC_DEBUG(DLC_DEBUG("\n Error in  [Kernel DLC][ %s  , %d ]  g_wLumaHistogram32H[0]=0 !!!\n", __FUNCTION__,__LINE__));
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
            DLC_DEBUG(DLC_DEBUG("\n Error in  [Kernel DLC][ %s  , %d ]  g_wLumaHistogram32H[%d]=0 !!!\n", __FUNCTION__,__LINE__,ucTmp));
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

    //~~~~~~~~~ output = uwBLE_Blend ?]with slow approach?^ ~~~~~~~~~~~~~~~~~~~~~

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

void LpfLumaHistogram (MS_U16 *g_wLumaHistogram32H_reg, MS_S32 *lpfLumaHistogram32)
{
    MS_S16 leftIdx;
    MS_S16 curIdx;
    MS_S16 rightIdx;
    MS_S16 ucTmp;
    for (ucTmp = 0; ucTmp < 32; ucTmp++)
    {
        leftIdx  = max (ucTmp - 1, 0);
        curIdx   = ucTmp;
        rightIdx = min (ucTmp + 1, 31);

        lpfLumaHistogram32[ucTmp] = (g_wLumaHistogram32H_reg[leftIdx] + 2*g_wLumaHistogram32H_reg[curIdx] + g_wLumaHistogram32H_reg[rightIdx]) >> 2;
    }
}

void ExpandLumaHistogram (MS_S32 *lpfLumaHistogram32, MS_S32 *expandLumaHistogram256)
{
    MS_S16 ucTmp;
    MS_S16 leftIdx;
    MS_S16 rightIdx;
    for (ucTmp = 0; ucTmp < 4; ucTmp++)
    {
        expandLumaHistogram256[ucTmp] = max(lpfLumaHistogram32[1] - ((lpfLumaHistogram32[1] - lpfLumaHistogram32[0])*(12 - ucTmp)/8), 0);
    }

    for (ucTmp = 4; ucTmp < 252; ucTmp++)
    {
        leftIdx  = (ucTmp - 4)/8;
        rightIdx = leftIdx + 1;
        expandLumaHistogram256[ucTmp] = max((lpfLumaHistogram32[leftIdx] * ((rightIdx*8) - (ucTmp - 4)) + lpfLumaHistogram32[rightIdx] * ((ucTmp - 4) - leftIdx*8)) / 8, 0);
    }

    for (ucTmp = 252; ucTmp < 256; ucTmp++)
    {
        expandLumaHistogram256[ucTmp] = max(lpfLumaHistogram32[30] + ((lpfLumaHistogram32[31] - lpfLumaHistogram32[30])*(ucTmp - 244)/8), 0);
    }

}

MS_S16 FindOutSpecificEntry(MS_U32 *dwHistogramAreaSum, MS_S32 srcThreshold)
{
    MS_S16 srcMinIdx = 0;
    MS_S16 srcMin;
    while (dwHistogramAreaSum[srcMinIdx] < srcThreshold)
    {
        srcMinIdx ++;
    }

    if (srcMinIdx == 0)
    {
        if (dwHistogramAreaSum[0] == 0)
        {
            srcMin = 4;
        }
        else
        {
            srcMin = (srcThreshold*4)/dwHistogramAreaSum[0];    // 256 bin 0~4
        }
    }
    else
    {
        srcMin = ((srcThreshold - dwHistogramAreaSum[srcMinIdx-1])*8 / (dwHistogramAreaSum[srcMinIdx] - dwHistogramAreaSum[srcMinIdx-1])) + 4 + (srcMinIdx-1)*8;
    }

    return srcMin;
}

void CutOffSrcMinMax(MS_S32 *expandLumaHistogram256, MS_S32 *cutOffLumaHistogram256, MS_S16 srcMin, MS_S16 srcMax)
{
    MS_S16 ucTmp;
    for (ucTmp = 0; ucTmp < 256; ucTmp ++)
    {
        if (ucTmp < srcMin || ucTmp > srcMax)
        {
            cutOffLumaHistogram256[ucTmp] = 0;
        }
        else
        {
            cutOffLumaHistogram256[ucTmp] = expandLumaHistogram256[ucTmp];
        }
    }
}

void CurveSlopeLimit(MS_S32 *cutOffLumaHistogram256, MS_S32 *normalizeLumaHistogram256, MS_U16 firstIdx, MS_U16 midIdx, MS_U16 lastIdx, MS_U32 sumOfSignificantInterval)
{
    //MS_U16 frontSlopeMin = 256; //format 2.8
    //MS_U16 frontSlopeMax = 512;
    //MS_U16 backSlopeMin  = 128;
    //MS_U16 backSlopeMax  = 256;

    MS_U16 frontSlopeMin = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16FrontSlopeMin;
    MS_U16 frontSlopeMax = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16FrontSlopeMax;
    MS_U16 backSlopeMin  = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16BackSlopeMin;
    MS_U16 backSlopeMax  = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16BackSlopeMax;

    if (frontSlopeMin == 0 )
    {
        frontSlopeMin = 256;
    }
    if (frontSlopeMax == 0 )
    {
        frontSlopeMax = 512;
    }
    if (backSlopeMin == 0 )
    {
        backSlopeMin = 128;
    }
    if (backSlopeMax == 0 )
    {
        backSlopeMax = 256;
    }
    MS_U16 u16Tmp;
    for (u16Tmp = 0; u16Tmp < 256; u16Tmp++)
    {
        normalizeLumaHistogram256[u16Tmp] = 0;
    }

    for (u16Tmp = firstIdx; u16Tmp <= lastIdx; u16Tmp++)
    {
        normalizeLumaHistogram256 [u16Tmp] = cutOffLumaHistogram256 [u16Tmp]*256*(lastIdx - firstIdx + 1)/(sumOfSignificantInterval);
        if (u16Tmp <= midIdx)
        {
            normalizeLumaHistogram256 [u16Tmp] = min(max(normalizeLumaHistogram256 [u16Tmp],frontSlopeMin), frontSlopeMax);
        }
        else
        {
            normalizeLumaHistogram256 [u16Tmp] = min(max(normalizeLumaHistogram256 [u16Tmp],backSlopeMin), backSlopeMax);
        }
    }
}

MS_U32 SumOfLumaHistogram(MS_S32 *lumaHistogram256, MS_U16 firstIdx, MS_U16 lastIdx)
{
    MS_U16 u16Tmp;
    MS_U32 sumOfSignificantInterval = 0;
    for (u16Tmp = firstIdx; u16Tmp <= lastIdx; u16Tmp++)
    {
        sumOfSignificantInterval += lumaHistogram256 [u16Tmp];
    }

    return sumOfSignificantInterval;
}
void UpSampleTo512(MS_S32 *curveCdfLumaHistogram256, MS_S32 *curveCdfLumaHistogram512)
{
    MS_U16 u16Tmp;
    for(u16Tmp =0; u16Tmp<256; u16Tmp++)
    {
        curveCdfLumaHistogram512[u16Tmp*2] = (curveCdfLumaHistogram256[u16Tmp])<<2; //10bit ->12 bit
        curveCdfLumaHistogram512[u16Tmp*2] = min(max(curveCdfLumaHistogram512[u16Tmp*2],0),4095);
    }
    for(u16Tmp =1; u16Tmp<512; u16Tmp+=2)
    {
        if(511 == u16Tmp)
        {
            curveCdfLumaHistogram512[511] = curveCdfLumaHistogram512[510];
        }
        else
        {
            curveCdfLumaHistogram512[u16Tmp] = (((curveCdfLumaHistogram512[u16Tmp -1])*4 +(curveCdfLumaHistogram512[u16Tmp +1])*4+4)/8);
        }
        curveCdfLumaHistogram512[u16Tmp] = min(max(curveCdfLumaHistogram512[u16Tmp],0),4095);
    }

}
void Subsample18Curve(MS_U16 *curveCdfLumaHistogram, MS_S32 *dwDLCCurve)
{
    MS_U8 u8Tmp;
    MS_U16 u16Tmp;
    MS_U32 cdfIdx;
    MS_S32 curveCdfLumaHistogram256[256];
    for(u16Tmp = 0; u16Tmp<256; u16Tmp++)
    {
        curveCdfLumaHistogram256[u16Tmp] = (curveCdfLumaHistogram[u16Tmp*2])>>6;
    }
    dwDLCCurve[0] = 2*curveCdfLumaHistogram256[0] - curveCdfLumaHistogram256[8];
    for (u8Tmp = 1; u8Tmp <= 16; u8Tmp++)
    {
        cdfIdx = ((u8Tmp - 1) * 16) + 8;
        dwDLCCurve[u8Tmp] = min(max(curveCdfLumaHistogram256[cdfIdx+1], 0 ), 1023);
    }
    dwDLCCurve[17] = curveCdfLumaHistogram256[255] + ((curveCdfLumaHistogram256[255] - curveCdfLumaHistogram256[248])*9/7);
}

void ComputeHistogramCdf(MS_S32 *normalizeLumaHistogram256, MS_S32 *curveCdfLumaHistogram256)
{
    MS_U16 u16Tmp;
    curveCdfLumaHistogram256[0] = normalizeLumaHistogram256[0];
    for (u16Tmp = 1; u16Tmp < 256; u16Tmp++)
    {
        curveCdfLumaHistogram256[u16Tmp] = curveCdfLumaHistogram256[u16Tmp-1] + normalizeLumaHistogram256[u16Tmp];
    }
}

void RemapToTarget(MS_S32 *curveCdfLumaHistogram256, MS_U16 tgtMax, MS_U16 tgtMin, MS_U32 sumOfSignificantInterval)
{
    MS_U16 u16Tmp;
    for (u16Tmp = 0; u16Tmp < 256; u16Tmp++)
    {
        curveCdfLumaHistogram256[u16Tmp] = (curveCdfLumaHistogram256[u16Tmp]*(tgtMax - tgtMin)/(sumOfSignificantInterval)) + tgtMin;
    }
}

void RepeatSlope(MS_S32 *curveCdfLumaHistogram256, MS_S32 *normalizeLumaHistogram256, MS_S16 firstIdx, MS_S16 lastIdx, MS_U32 tgtMaxMinusMin, MS_U32 sumOfSignificantInterval)
{
    MS_S16 s16Tmp;
    // Repeat slope of firstIdx & lastIdx
    for (s16Tmp = firstIdx; s16Tmp >= 0; s16Tmp--)
    {
        curveCdfLumaHistogram256[s16Tmp] = max((MS_S32)(curveCdfLumaHistogram256[s16Tmp] - (((firstIdx - s16Tmp) * normalizeLumaHistogram256[firstIdx])*tgtMaxMinusMin/sumOfSignificantInterval)), 0);
    }
    for (s16Tmp = lastIdx; s16Tmp < 256; s16Tmp++)
    {
        curveCdfLumaHistogram256[s16Tmp] = min(curveCdfLumaHistogram256[s16Tmp] + (((s16Tmp - lastIdx) * normalizeLumaHistogram256[lastIdx])*tgtMaxMinusMin/sumOfSignificantInterval), 1023);
    }
}

void GetTmoCurve( const MS_U32 dwSumBelowAvg, const MS_U32 dwSumAboveAvg, const MS_U16 ucYAvgN, MS_U16 *g_wLumaHistogram32H_reg, MS_U32 *dwHistogramAreaSum/* ,MS_S32 *dwDLCCurve*/
                  ,MS_S32 *curveCdfLumaHistogram512)
{

    MS_U16 u16Tmp;
    MS_S16 s16Tmp;
    MS_S32 lpfLumaHistogram32[32];
    MS_S32 srcMinThreshold, srcMaxThreshold, bisecThreshold, bisecSrc, bisecDst, srcMin, srcMax, firstIdx, lastIdx, midIdx;

    MS_U16 srcMinRatio = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SrcMinRatio;
    MS_U16 srcMedRatio = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SrcMedRatio;
    MS_U16 srcMaxRatio = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SrcMaxRatio;

    MS_U8 tgtMinFlag = g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag;
    MS_U16 tgtMin = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMin;  // 0.05
    MS_U8 tgtMaxFlag = g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMaxFlag;
    MS_U16 tgtMax = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMax; // 300

    MS_U16 tgtMinCode, tgtMaxCode;
    MS_U32 sumOfSignificantInterval = 0;

    //MS_S32 expandLumaHistogram256[256];
    //MS_S32 cutOffLumaHistogram256[256];
    //MS_S32 normalizeLumaHistogram256[256];
    //MS_S32 curveCdfLumaHistogram256[256];

    MS_S32 *expandLumaHistogram256 = (MS_S32*)kmalloc(256*sizeof(MS_S32),GFP_KERNEL);
    MS_S32 *cutOffLumaHistogram256 = (MS_S32*)kmalloc(256*sizeof(MS_S32),GFP_KERNEL);
    MS_S32 *normalizeLumaHistogram256 = (MS_S32*)kmalloc(256*sizeof(MS_S32),GFP_KERNEL);
    MS_S32 *curveCdfLumaHistogram256 = (MS_S32*)kmalloc(256*sizeof(MS_S32),GFP_KERNEL);

    //log = LogPrintfOneTimes();

    // Bi-Histogram, rescale the histogram equalization to the separate point
    if (dwSumBelowAvg==0 || dwSumAboveAvg==0)
    {
        for (u16Tmp = 0; u16Tmp < 32; u16Tmp++)
        {
            dwHistogramAreaSum[u16Tmp] = (u16Tmp+1)*8*4;//10 bit
        }
        return;
    }

    LpfLumaHistogram(g_wLumaHistogram32H_reg, lpfLumaHistogram32);

    ExpandLumaHistogram(lpfLumaHistogram32, expandLumaHistogram256);

    srcMinThreshold = ( dwHistogramAreaSum[31]*srcMinRatio ) >> 10;         // 0.01
    srcMaxThreshold = ( dwHistogramAreaSum[31]*srcMaxRatio ) >> 10;         // 0.99
    bisecThreshold  = (dwHistogramAreaSum[31]*srcMedRatio) >> 10;

    srcMin   = FindOutSpecificEntry(dwHistogramAreaSum, srcMinThreshold);
    srcMax   = FindOutSpecificEntry(dwHistogramAreaSum, srcMaxThreshold);
    bisecSrc = FindOutSpecificEntry(dwHistogramAreaSum, bisecThreshold);

    tgtMinCode = MS_Cfd_Luminance_To_PQCode(tgtMin, tgtMinFlag);
    tgtMaxCode = MS_Cfd_Luminance_To_PQCode(tgtMax, tgtMaxFlag);

    firstIdx = min(srcMin, tgtMinCode*255/1023);
    lastIdx  = max(srcMax, tgtMaxCode*255/1023);
    midIdx   = bisecSrc;

    CutOffSrcMinMax(expandLumaHistogram256, cutOffLumaHistogram256, srcMin, srcMax);

    sumOfSignificantInterval = SumOfLumaHistogram(cutOffLumaHistogram256, firstIdx, lastIdx);
    CurveSlopeLimit(cutOffLumaHistogram256, normalizeLumaHistogram256, firstIdx, midIdx, lastIdx, sumOfSignificantInterval);

    sumOfSignificantInterval = SumOfLumaHistogram(normalizeLumaHistogram256, firstIdx, lastIdx);
    ComputeHistogramCdf(normalizeLumaHistogram256, curveCdfLumaHistogram256);
    RemapToTarget(curveCdfLumaHistogram256, tgtMaxCode, tgtMinCode, sumOfSignificantInterval);
    RepeatSlope(curveCdfLumaHistogram256, normalizeLumaHistogram256, firstIdx, lastIdx, tgtMaxCode - tgtMinCode, sumOfSignificantInterval);
    UpSampleTo512(curveCdfLumaHistogram256,curveCdfLumaHistogram512);//output 12bit
    //Subsample18Curve(curveCdfLumaHistogram256, dwDLCCurve);

    kfree((void *)expandLumaHistogram256);
    kfree((void *)cutOffLumaHistogram256);
    kfree((void *)normalizeLumaHistogram256);
    kfree((void *)curveCdfLumaHistogram256);
}
void SimpleIIRforStatistics(MS_U16* dwDLCTmoCurve, MS_U16* IIR_Results )
{
    MS_U8 u8Tmp;
    MS_U8 u8IIRRatio;

    MS_S32 s32TmoCurveDiff = 0;
    MS_S32 s32SceneChangeThrd = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SceneChangeThrd;
    MS_S32 s32SceneChangeRatioMax = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SceneChangeRatioMax;

    MS_S32 s32SceneChangeRatioMaxQuantize = 32;

    MS_S32 s32SceneChangeRatio;
    MS_S32 s32SceneChangeRatioShift = 5;
    static MS_S32 Tg_uwDlcTmoCurve[32];
    static MS_S32 Tg_uwDlcPreTmoCurve[32];
    static MS_BOOL Tg_bDlcTmoIIRStatus = false;
    u8IIRRatio = g_HDRinitParameters.DLC_HDRNewToneMappingData.u8IIRRatio;

    if(1!= g_DlcParameters.u8Tmo_Mode )
    {
        //printf("Bypass Statistics IIR\n");
        for (u8Tmp = 0; u8Tmp < 32; u8Tmp++)
        {
            IIR_Results[u8Tmp] = dwDLCTmoCurve[u8Tmp];
        }
        return; //Bypass use 18 point Curve to do IIR
    }

    if (u8IIRRatio >= 0x20)
    {
        u8IIRRatio = 0x20;
    }

    if (Tg_bDlcTmoIIRStatus == false)
    {
        for (u8Tmp = 0; u8Tmp < 32; u8Tmp++)
        {
            Tg_uwDlcTmoCurve[u8Tmp] = (MS_S32)dwDLCTmoCurve[u8Tmp];
            Tg_uwDlcPreTmoCurve[u8Tmp] =  (MS_S32)dwDLCTmoCurve[u8Tmp];
        }
        Tg_bDlcTmoIIRStatus = true;
    }
    else
    {
        //scene change
        for (u8Tmp = 0; u8Tmp < 32; u8Tmp++)
        {
            // sum of abs difference
            if ( ((MS_S32)dwDLCTmoCurve[u8Tmp]) >= Tg_uwDlcPreTmoCurve[u8Tmp])
            {
                s32TmoCurveDiff += ( ((MS_S32)dwDLCTmoCurve[u8Tmp]) - Tg_uwDlcPreTmoCurve[u8Tmp]);
            }
            else
            {
                s32TmoCurveDiff += (Tg_uwDlcPreTmoCurve[u8Tmp]  -  ((MS_S32)dwDLCTmoCurve[u8Tmp]));
            }

        }
        s32SceneChangeRatio = min(max(s32TmoCurveDiff -s32SceneChangeThrd, 0 ), s32SceneChangeRatioMax) >> s32SceneChangeRatioShift;
        //s32SceneChangeRatio = ((s32TmoCurveDiff -s32SceneChangeThrd)*s32SceneChangeRatioShift + 32)/64;
        //s32SceneChangeRatio = min(max(s32SceneChangeRatio,0),s32SceneChangeRatioMax);
        u8IIRRatio = (u8IIRRatio * (s32SceneChangeRatioMaxQuantize - s32SceneChangeRatio)) >> 5;

        HDR_DBG_HAL(DLC_DEBUG("\n TonyPeng test in  [Kernel DLC], PreCurve0~5 = [%d, %d, %d, %d, %d]\n", g_uwDlcPreTmoCurve[0], g_uwDlcPreTmoCurve[1], g_uwDlcPreTmoCurve[2], g_uwDlcPreTmoCurve[3], g_uwDlcPreTmoCurve[4]));
        HDR_DBG_HAL(DLC_DEBUG("\n TonyPeng test in  [Kernel DLC], Thrd = %d!!!, SceneChangeRatioMax = %d\n", s32SceneChangeThrd, s32SceneChangeRatioMax));
        HDR_DBG_HAL(DLC_DEBUG("\n TonyPeng test in  [Kernel DLC], Diff = %d!!!, IIRRatio = %d, SceneChangeRatio = %d\n", s32TmoCurveDiff, u8IIRRatio, s32SceneChangeRatio));
        //printf("\n Easter statistics in  [Kernel DLC], Diff = %d!!!, IIRRatio = %d, SceneChangeRatio = %d\n", s32TmoCurveDiff, u8IIRRatio, s32SceneChangeRatio);

        for (u8Tmp = 0; u8Tmp < 32; u8Tmp++)
        {
            Tg_uwDlcTmoCurve[u8Tmp] = (((MS_S32)dwDLCTmoCurve[u8Tmp])*(0x20 - u8IIRRatio) + Tg_uwDlcPreTmoCurve[u8Tmp]*u8IIRRatio + 16) >> 5;

            if (Tg_uwDlcTmoCurve[u8Tmp] == Tg_uwDlcPreTmoCurve[u8Tmp])
            {
                if (((MS_S32)dwDLCTmoCurve[u8Tmp]) > Tg_uwDlcTmoCurve[u8Tmp])
                {
                    Tg_uwDlcTmoCurve[u8Tmp] ++;
                }
                else if (((MS_S32)dwDLCTmoCurve[u8Tmp])< Tg_uwDlcTmoCurve[u8Tmp])
                {
                    Tg_uwDlcTmoCurve[u8Tmp] --;
                }
            }
        }

        for (u8Tmp = 0; u8Tmp < 32; u8Tmp++)
        {
            Tg_uwDlcPreTmoCurve[u8Tmp] = Tg_uwDlcTmoCurve[u8Tmp];
            IIR_Results[u8Tmp] = Tg_uwDlcTmoCurve[u8Tmp];
        }
    }
}
void SimpleTmoIIR(MS_S32* dwDLCTmoCurve)
{
    MS_U8 u8Tmp;
    MS_U8 u8IIRRatio;

    MS_S32 s32TmoCurveDiff = 0;
    MS_S32 s32SceneChangeThrd = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SceneChangeThrd;
    MS_S32 s32SceneChangeRatioMax = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16SceneChangeRatioMax;

    MS_S32 s32SceneChangeRatioMaxQuantize = 32;

    MS_S32 s32SceneChangeRatio;
    MS_S32 s32SceneChangeRatioShift = 5;

    u8IIRRatio = g_HDRinitParameters.DLC_HDRNewToneMappingData.u8IIRRatio;
    if (u8IIRRatio >= 0x20)
    {
        u8IIRRatio = 0x20;
    }

    if (g_bDlcTmoIIRStatus == false)
    {
        for (u8Tmp = 0; u8Tmp < 18; u8Tmp++)
        {
            g_uwDlcTmoCurve[u8Tmp] = dwDLCTmoCurve[u8Tmp];
            g_uwDlcPreTmoCurve[u8Tmp] = dwDLCTmoCurve[u8Tmp];
        }
        g_bDlcTmoIIRStatus = true;
    }
    else
    {
        //scene change
        for (u8Tmp = 0; u8Tmp < 18; u8Tmp++)
        {
            // sum of abs difference
            if (dwDLCTmoCurve[u8Tmp] >= g_uwDlcPreTmoCurve[u8Tmp])
            {
                s32TmoCurveDiff += (dwDLCTmoCurve[u8Tmp]  - g_uwDlcPreTmoCurve[u8Tmp]);
            }
            else
            {
                s32TmoCurveDiff += (g_uwDlcPreTmoCurve[u8Tmp]  - dwDLCTmoCurve[u8Tmp]);
            }

        }

        s32SceneChangeRatio = min(max(s32TmoCurveDiff -s32SceneChangeThrd, 0 ), s32SceneChangeRatioMax) >> s32SceneChangeRatioShift;
        u8IIRRatio = (u8IIRRatio * (s32SceneChangeRatioMaxQuantize - s32SceneChangeRatio)) >> 5;

        HDR_DBG_HAL(DLC_DEBUG("\n TonyPeng test in  [Kernel DLC], PreCurve0~5 = [%d, %d, %d, %d, %d]\n", g_uwDlcPreTmoCurve[0], g_uwDlcPreTmoCurve[1], g_uwDlcPreTmoCurve[2], g_uwDlcPreTmoCurve[3], g_uwDlcPreTmoCurve[4]));
        HDR_DBG_HAL(DLC_DEBUG("\n TonyPeng test in  [Kernel DLC], Thrd = %d!!!, SceneChangeRatioMax = %d\n", s32SceneChangeThrd, s32SceneChangeRatioMax));
        HDR_DBG_HAL(DLC_DEBUG("\n TonyPeng test in  [Kernel DLC], Diff = %d!!!, IIRRatio = %d, SceneChangeRatio = %d\n", s32TmoCurveDiff, u8IIRRatio, s32SceneChangeRatio));
        //printf("\n Easter curve in  [Kernel DLC], Diff = %d!!!, IIRRatio = %d, SceneChangeRatio = %d\n", s32TmoCurveDiff, u8IIRRatio, s32SceneChangeRatio);
        for (u8Tmp = 0; u8Tmp < 18; u8Tmp++)
        {
            g_uwDlcTmoCurve[u8Tmp] = (dwDLCTmoCurve[u8Tmp]*(0x20 - u8IIRRatio) + g_uwDlcPreTmoCurve[u8Tmp]*u8IIRRatio + 16) >> 5;

            if (g_uwDlcTmoCurve[u8Tmp] == g_uwDlcPreTmoCurve[u8Tmp])
            {
                if (dwDLCTmoCurve[u8Tmp] > g_uwDlcTmoCurve[u8Tmp])
                {
                    g_uwDlcTmoCurve[u8Tmp] ++;
                }
                else if (dwDLCTmoCurve[u8Tmp] < g_uwDlcTmoCurve[u8Tmp])
                {
                    g_uwDlcTmoCurve[u8Tmp] --;
                }
            }
        }

        for (u8Tmp = 0; u8Tmp < 18; u8Tmp++)
        {
            g_uwDlcPreTmoCurve[u8Tmp] = g_uwDlcTmoCurve[u8Tmp];
        }
    }
}

void msDlcWriteTmoCurve(MS_BOOL bWindow)
{
    MS_U8 i;
    msDlc_FunctionEnter();
    for (i = 0; i < 18; i++)
    {
        if (i>0 && i<17)
        {
            msWriteByte((REG_ADDR_DLC_DATA_START_MAIN+(i-1)),g_uwDlcTmoCurve[i]>>2);
            msDlcWriteCurveLSB(0, i, g_uwDlcTmoCurve[i]&0x03);
        }
        if( MAIN_WINDOW == bWindow )
        {
            // set DLC curve index N0 & 16
            if (i == 0)
            {
                msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_MAIN, g_uwDlcTmoCurve[0]>>2);
                msWriteByte(REG_ADDR_DLC_DATA_EXTEND_N0_MAIN + 1, 0x01);
                msDlcWriteCurveLSB(MAIN_WINDOW, 0xFF,g_uwDlcTmoCurve[0]&0x03);
            }

            if (i== 17)
            {
                msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_MAIN, g_uwDlcTmoCurve[17]>>2);
                msWriteByte(REG_ADDR_DLC_DATA_EXTEND_16_MAIN + 1, 0x00);
                msDlcWriteCurveLSB(MAIN_WINDOW, 16, g_uwDlcTmoCurve[17]&0x03);
            }
        }
    }

}

MS_U16 MS_Cfd_Luminance_To_PQCode(MS_U16 u16Luminance, MS_U8 u8Function_Mode)
{
    MS_U8 u8Range = 0;
    MS_U16 u16Step = 0;
    MS_U16 u16PQ_Code = 0;
    MS_U16 u16LuminanceOffset =0;
    MS_U8 u8temp = 0;
    MS_U8 u8temp2 = 0;

    MS_U16 u16_hbound = 0;
    MS_U16 u16_lbound = 0;
    MS_U16 u16_diffh = 0;
    MS_U16 u16_diffl = 0;

    //range handle
    if ((u16Luminance>=10000) && (1 == u8Function_Mode))
    {
        u16Luminance = u16Luminance/10000;
        u8Function_Mode = 0;
    }

    if (u16Luminance < 1)
    {
        u8Range = 0;
    }
    else if (u16Luminance < 10)
    {
        u8Range = 1;
        u16Step = 0;
    }
    else if (u16Luminance < 100)
    {
        u8Range = 2;
        u16Step = 5;
    }
    else if (u16Luminance < 1000)
    {
        u8Range = 3;
        u16Step = 50;
    }
    else if (u16Luminance < 10000)
    {
        u8Range = 4;
        u16Step = 500;
    }
    else
    {
        u8Range = 0;
    }


    if  (u8Range == 0)
    {
        if (u16Luminance < 1)
        {
            u16PQ_Code = 0;
        }
        else
        {
            u16PQ_Code = 1023;
        }
    }
    else if (u8Range == 1)
    {

        if (u8Function_Mode == 1)
        {
            u8temp = 0+(u16Luminance-1)*2;
        }
        else
        {
            u8temp = 72+(u16Luminance-1)*2;
        }
        u16PQ_Code = LUT_CFD_NITS2PQCODE[u8temp];

    }
    else // (u8Range > 1)
    {
        u16LuminanceOffset = u16Luminance-(u16Step*2);

        u8temp = (u16LuminanceOffset/u16Step);

        //calculate offset of LUT_CFD_NITS2PQCODE
        if (u8Function_Mode == 1)
        {
            u8temp2 = 0 + 18*(u8Range-1);
        }
        else
        {
            u8temp2 = 72 + 18*(u8Range-1);
        }

        u16_lbound = LUT_CFD_NITS2PQCODE[u8temp2+u8temp];
        u16_hbound = LUT_CFD_NITS2PQCODE[u8temp2+u8temp+1];
        u16_diffl = u16LuminanceOffset-u16Step*u8temp;
        u16_diffh = u16Step-u16_diffl;

        u16PQ_Code = (((MS_U32)u16_hbound)*u16_diffl + ((MS_U32)u16_lbound)*u16_diffh)/u16Step;
    }

    return u16PQ_Code;
}
void Mapi_Cfd_TMO_Unit_PQ2GAMMA(MS_U16 *pu16Result_TMO_PQ2Gamma,
                                MS_U16 u16Length_Result_TMO_PQ2Gamma,
                                const MS_U16 *u16pInput_Gamma_Idx,
                                const MS_U16 *u16pInput_PQArray,
                                MS_U16 u16Length_Input_PQArray,
                                MS_U16 u16target_max_lum_nits)
{
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT

    MS_U16 u16Idx = 0;
    MS_U32 u32Temp = 0;
    MS_U16 u16Input_idx_gamma = 0;
    MS_U16 u16WhileIdx = 0;

    MS_U16 u16x0 = 0;
    MS_U16 u16x2 = 0;
    MS_U16 u16y0 = 0;
    MS_U16 u16y2 = 0;

    MS_U32 u32Temp2 = 0;
    MS_U32 u32Temp3 = 0;
    MS_U32 u32Temp32 = 0;
    MS_U32 u32Temp4 = 0;
    MS_U64 u64Nume = 255;
    MS_U64 u64Denum = 255;
    MS_U16 u16Max_luma_codes = 520;

    MS_U16 u16Gamma = 0x8CCD, u16A = 36012, u16B = 0x49C, u16S = 0x1200, u16Ginv = 0x1000000ul;

    //513 = (2^18-1)/511;
    MS_U16 u16const0 = 513,u16gain = 512;
    MS_U8 u8Mode = 0;
    //for the first entry
    u32Temp = 0;
    pu16Result_TMO_PQ2Gamma[0] = 0;

    u16Max_luma_codes = u16target_max_lum_nits;//MS_Cfd_Luminance_To_PQCode(u16target_max_lum_nits, 0);

    u16Max_luma_codes = ((MS_U32)u16Max_luma_codes*0xff00)/(0x3ff);

    //#if 0
    switch(u8Mode)
    {
        case 0: //gamma 2.2
    u16Gamma = 0x8CCD>>2; // 2.2
    u16B = 0; // 0
    u16Ginv = 0x1000000ul / u16Gamma; // (1<<24) / 4.12 -> 1.12
    u16S = 0;
    u16A = 0x8000; // value = 1.0
        //printk("gamma2.2 ");
        break;
    //#endif
        case 1: //BT709
        u16Gamma = 0x8CCD>>2; // 2.2
        u16B = 0x49C; // 0.018;
        u16Ginv = 0x1000000ul / u16Gamma; // (1<<24) / 4.12 -> 1.12  1125
        u16S = 0x1200; // 44.5
        u16A = 36012; // 1125->1.099
        //printk("BT709 ");
        break;
    //start from second entry to the end
    }

    MS_U32 u32Val, u32Gamma,u32GammaInv,u32Temp5,u32Compen;
    MS_U16 u16Alpha,u16Beta,u16LowerBoundGain;
    MS_U8 u8BitDepth=14,sMode;
    switch (u8Mode)
    {

  // u32Gamma          : 4.16
  // u32GammaInv       : 4.16
  // u16Alpha          : 1.15
  // u16Beta           : 0.16
  // u16LowerBoundGain : 6.10
    case 0: // 2.2
              sMode = 0;
              //stDeGammaData->sData[0] = 0x8CCD>>2; // 2.2
        //stDeGammaData->sData[1] = 0;
  u32Gamma = 0x745D;
  u32GammaInv = 0x23333;
  u16Beta = 0;
  u16Alpha = 0x8000;
  u16LowerBoundGain  = 0x0100;;
  //stDeGammaData->bExtendEn = 0;
  //printk("New Gamma 2.2\n");
  break;
    case 1: // 709
  sMode = 0;
  //stDeGammaData->sData[0] = 0x8E39>>2; // 1/0.45 = 2.222
  //stDeGammaData->sData[1] = 0x49C; // 0.018  u32GammaInv
  u32Gamma = 0x7333;
  u32GammaInv = 0x238E4;
  u16Beta = 0x49C; // 0.018;
  u16Alpha = 36012;
  u16LowerBoundGain  = 0x1200;;
  //stDeGammaData->bExtendEn = 0;
              //printk("New BT709\n");
              break;
    case 2: // gamma 0.1
              sMode = 0;
              //stDeGammaData->sData[0] = 0x8CCD>>2; // 2.2
        //stDeGammaData->sData[1] = 0;
  u32Gamma = 0x1999; //gamma 0.1
  u32GammaInv = 0x9FFF6;
  u16Beta = 0;
  u16Alpha = 0x8000;
  u16LowerBoundGain  = 0x0100;;
  //stDeGammaData->bExtendEn = 0;
  //printk("New Gamma 0.1\n");
  break;
    }
    //Mhal_CFD_GammaSampling(u32Val, pData->u32Gamma, pData->u16Alpha, pData->u16Beta, pData->u16LowerBoundGain , u8BitDepth,pData->sMode);
    //u8Mode = msReadByte(REG_SC_BK30_0F_L);
    for (u16Idx = 0; u16Idx<u16Length_Result_TMO_PQ2Gamma; u16Idx++)
    {
       u16x2 = LUT_IndexDeGamma2PQ[u16Idx];
        //for debug
        //if (u16Idx == 510)
        //{
        //  u16Idx = u16Idx;
        //}
        #if 0
        u32Temp = u32Temp + u16const0;
        u32Temp2 = ((u32Temp+2)>>2);

        if (u32Temp2>0xFFFF)
        {
            u32Temp2 = 0xFFFF;
        }
        u16Input_idx_gamma = u32Temp2&0xffff;

        while(u16pInput_Gamma_Idx[u16WhileIdx] < u16Input_idx_gamma)
        {
            u16WhileIdx = u16WhileIdx + 1;
        }

        u16x0 = u16pInput_Gamma_Idx[u16WhileIdx-1];
        u16x2 = u16pInput_Gamma_Idx[u16WhileIdx];
        u16y0 = u16pInput_PQArray[u16WhileIdx-1];
        u16y2 = u16pInput_PQArray[u16WhileIdx];

        //interpolation for each entries to get PQ'

        u32Temp2 = (u16y2-u16y0)*(u16Input_idx_gamma-u16x0)/(u16x2-u16x0);
        u32Temp2 = u32Temp2 + u16y0;

        //transfer PQ' to gamma by configures
        //PQ2nits
        //nitsGamma
        //set parameters:output max luminance(nits)
        #endif
        //transfer 1 from 0xFFFF to 0xFF00
        //u32Temp2 = u16pInput_PQArray[u16Idx];
        u32Temp2 = u16pInput_PQArray[u16x2];
        u32Temp2 = (u32Temp2*0xFF00/0xFFFF);

        u64Denum = MDrv_HDR_PQ_ali(u16Max_luma_codes);
        u64Nume = MDrv_HDR_PQ_ali(u32Temp2);
        //printk("u64Denum:%llx\n",u64Denum);
        //u32Temp3 = min(u64Nume * 0xFFF00000ul / u64Denum, 0xFFF00000ul);
        //transfer 1 from 0xFFF00000 to 0xFFf0
        u32Temp3 = min((u64Nume * 0xFFF0ul /u64Denum)*(0x10000ul/0xFFF0ul), 0x10000ul);
        //u32Temp3 = min((u64Nume * 0xFFF0ul /u64Denum), 0xFFF0ul);
        //protection for the minimum accuracy of MDrv_HDR_DeGamma_ali()
        if ((u64Nume != 0) && (u32Temp3 == 0))
        {
            u32Temp3 = 1;
        }

        //apply gamma 2.2
        //input 1 = 0xFFF0
        //output 1 = 0xFFE10000
        //transfer 1 from 0xFFFF to 0xFFF0
        //u32Val = (u16Idx<<7);
        //u32Temp4 = MDrv_HDR_Gamma_ali(u32Temp3, u16Gamma, u16Ginv, u16A, u16B, u16S);
        //u32Temp3 = u16Idx <<7;
        u32Temp4 = Mhal_CFD_GammaSampling(u32Temp3, u32Gamma, u16Alpha, u16Beta, u16LowerBoundGain , u8BitDepth,sMode);
        //u32Temp5 = Mhal_CFD_GammaSampling(u32Temp3, 0x1999, 0x8000, 0, 0x0100 , u8BitDepth,sMode); // Compenstate 2.2 to 709 apply gamma 0.1
        //u32Temp5 = MDrv_HDR_Bt1886Gamma(MS_U16 u16In, MS_U16 u16Gamma, MS_U16 u16Ginv, MS_U16 u16White, MS_U16 u16Black);
        //u32Temp4 = MDrv_HDR_Gamma_ali(u32Temp3, u16Gamma, u16Ginv, u16A, u16B, u16S);
           /*
        if(0 == u32Temp5)
        {
            u32Temp5 = 1;
        }
       // u32Compen = ((u32Temp4<<16)/u32Temp5);


        if (u32Compen>0xffff)
        {
            u32Compen = 0xffff;
        }

        if (u32Temp4>0xffff)
        {
            u32Temp4 = 0xffff;
        }
        */
        if(0 == u8Mode)
        {
            u16gain = 512;
        }
        else
        {
            u16gain = BT709toBT1886Gain[u16Idx];
        }
        u32Compen = (u32Temp4*u16gain)/512;
        if (u32Compen>0xffff)
        {
            u32Compen = 0xffff;
        }
        /*
        if(0 ==(u16Idx % 16))
        {
            printk("u16Idx:%d TMO:%d u32Temp5:%d u32Compen:%d\n",u16Idx,u32Temp4,u32Temp5,u32Compen);
        }
        */
        pu16Result_TMO_PQ2Gamma[u16Idx] = u32Compen;
        pu16Result_TMO_PQ2Gamma[u16Idx] = pu16Result_TMO_PQ2Gamma[u16Idx] >>4; //16bit ->12bit TMO input
    }

#endif
}

#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
void msNewTMO(MS_BOOL bWindow)
{
    BYTE ucTmp,ucYAvgN=0;
    DWORD dwHistogramAreaSum[32];
    DWORD dwSumBelowAvg=0, dwSumAboveAvg=0;
    //DWORD dwDLCCurve[16];
    MS_S32 dwDLCTmoCurve[18];
    //DWORD g_uwDLCResult[16];
    DWORD dwYLftAvg;
    DWORD dwYRtAvg;
    MS_U16 u16tmp;
    const MS_U16 *pu16pInput_Gamma_Idx;
    MS_U16 u16Length_Result_TMO_PQ2Gamma = 512;
    MS_U16 u16Length_Input_PQArray = 512;
    MS_U16 u16target_max_lum_nits = g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMax;
    MS_U8 u8Input_Lum_Mode = 1;
    MS_U16 u16IIR_results[32];
    MS_U8 u8Control = 0;
    MS_U16 u16ExpandHistogram[32] = {0};
    //MS_S32 curveCdfLumaHistogram512[512];
    //MS_U16 u16curveCdfLumaHistogram512[512];
    //MS_U16 u16Result_TMO_PQ2Gamma[512];
    MS_S32 *curveCdfLumaHistogram512 = (MS_S32*)kmalloc(512*sizeof(MS_S32),GFP_KERNEL);
    MS_U16 *u16curveCdfLumaHistogram512 = (MS_U16*)kmalloc(512*sizeof(MS_U16),GFP_KERNEL);
    MS_U16 *u16Result_TMO_PQ2Gamma = (MS_U16*)kmalloc(512*sizeof(MS_U16),GFP_KERNEL);
    // Get the separate point "ucYAvgN" at Y average

    HistogramExpand(g_wLumaHistogram32H, u16ExpandHistogram);
    ucYAvgN = msGetAverageLuminous();
    if (ucYAvgN == 16 || ucYAvgN == 235 || 1 == u8HistogramErrFlag) //Criteria should be refered range information 16~235 limit range 0~255 full range
    {
        for (u16tmp = 0; u16tmp < 512; u16tmp++)
        {
            u16Result_TMO_PQ2Gamma[u16tmp] = u16tmp <<3;
        }
        //writeMdTmo(u16Result_TMO_PQ2Gamma,0);
        writeMdTmo(u16Result_TMO_PQ2Gamma,0,4000,100);
     return STATUS_SUCCESS;
    }

    SimpleIIRforStatistics(u16ExpandHistogram,u16IIR_results);
    if (Get_DLCInfo_from_Histogram(ucYAvgN, u16IIR_results, dwHistogramAreaSum, &dwSumBelowAvg, &dwSumAboveAvg, &dwYLftAvg, &dwYRtAvg)!=STATUS_SUCCESS)
    {
        return STATUS_ERROR;
    }

    GetTmoCurve( dwSumBelowAvg, dwSumAboveAvg, ucYAvgN, u16IIR_results, dwHistogramAreaSum/*,dwDLCTmoCurve*/,curveCdfLumaHistogram512);


    for(u16tmp = 0; u16tmp<512; u16tmp++)
    {
        u16curveCdfLumaHistogram512[u16tmp] = curveCdfLumaHistogram512[u16tmp];
        u16curveCdfLumaHistogram512[u16tmp] = (u16curveCdfLumaHistogram512[u16tmp])<<4;//12bit -> 16 bit
    }

    switch(u8Control)
    {
        case 0:
            pu16pInput_Gamma_Idx = input_gamma_idx_10k;
            break;
        case 1:
            pu16pInput_Gamma_Idx = input_gamma_idx_4k;
            break;
        default:
            pu16pInput_Gamma_Idx = input_gamma_idx_4k;
            break;
    }

    if(g_DlcParameters.u8Tmo_Mode == 1)
    {
        Mapi_Cfd_TMO_Unit_PQ2GAMMA(u16Result_TMO_PQ2Gamma,
                                   u16Length_Result_TMO_PQ2Gamma,
                                   pu16pInput_Gamma_Idx,
                                   u16curveCdfLumaHistogram512,
                                   u16Length_Input_PQArray,
                                   u16target_max_lum_nits);//16bit in 12 bit out for TMO
        writeMdTmo(u16Result_TMO_PQ2Gamma,0,4000,100);
    }
    else
    {
        Subsample18Curve(u16curveCdfLumaHistogram512, dwDLCTmoCurve);//16bit in 10bit out for DLC
        SimpleTmoIIR(dwDLCTmoCurve);
        msDlcWriteTmoCurve(bWindow);
    }

    kfree((void *)curveCdfLumaHistogram512);
    kfree((void *)u16curveCdfLumaHistogram512);
    kfree((void *)u16Result_TMO_PQ2Gamma);
}
#endif
/////////////msNewDlcHandler_WithCorrectionRatio/////////////////
// Generate DLC Curve
// Input:
//   ucYAvg: YAvg
//   g_wLumaHistogram32H_reg[32]: 32 bin histogram
// Output:
//   g_ucTable[16]: DLC Curve 0~1023
//////////////////////////////////////////////////////////
void DLC_18PQ2Gamma(MS_U16 *g_ucTable,MS_U16 *u16Result_TMO_PQ2Gamma)
{
    const MS_U16 *pu16pInput_Gamma_Idx;
    MS_U16 u16Length_Result_TMO_PQ2Gamma = 512,i;
    MS_U16 u16Length_Input_PQArray = 512;
    MS_U16 u16target_max_lum_nits = 10000;
    MS_U16 *u16curveCdfLumaHistogram512 = (MS_U16*)kmalloc(512*sizeof(MS_U16),GFP_KERNEL);
    MS_U16 *u16curveCdfLumaHistogram512_16bit = (MS_U16*)kmalloc(512*sizeof(MS_U16),GFP_KERNEL);
    MS_U8 u8Control = 0;
    switch(u8Control)
    {
        case 0:
            pu16pInput_Gamma_Idx = input_gamma_idx_10k;
            break;
        case 1:
            pu16pInput_Gamma_Idx = input_gamma_idx_4k;
            break;
        default:
            pu16pInput_Gamma_Idx = input_gamma_idx_4k;
            break;
    }

    for(i = 0; i< 16;i++)
    {
        if(0 == i)
        {
            u16curveCdfLumaHistogram512[16] = g_ucTable[0];
        }
        else if(15 == i)
        {
            u16curveCdfLumaHistogram512[496] = g_ucTable[15];
        }
        else
        {
            u16curveCdfLumaHistogram512[(i)*32+16] = g_ucTable[i];
        }
        u16curveCdfLumaHistogram512[0] = 0;
        u16curveCdfLumaHistogram512[511] = 1023;
        printk("g_ucTable:%d\n",g_ucTable[i]);
    }
    MS_U8 u8Index = 0,u8Dist = 0;
    for(i= 0;i<512;i++)
    {
        if(i < 16)
        {
            u16curveCdfLumaHistogram512[i] = ((i*u16curveCdfLumaHistogram512[16] + (16 -i)*u16curveCdfLumaHistogram512[0])/16);
        }
        else if(i < 496)
        {

            u8Index = (i -16)/32;
            u8Dist = (i -16)%32;
            u16curveCdfLumaHistogram512[i] = (((u8Dist)*u16curveCdfLumaHistogram512[(u8Index+1)*32+16] + (32 -u8Dist)*u16curveCdfLumaHistogram512[u8Index*32+16])/32);
        }
        else
        {
            u16curveCdfLumaHistogram512[i] = (((i- 496)*u16curveCdfLumaHistogram512[511]  + (16 -(i- 496))*u16curveCdfLumaHistogram512[496])/16);
        }
        u16curveCdfLumaHistogram512_16bit[i] = u16curveCdfLumaHistogram512[i]<<6;
        //printf("u16curveCdfLumaHistogram512[%d]:%d\n",i,u16curveCdfLumaHistogram512_16bit[i]);
    }
    Mapi_Cfd_TMO_Unit_PQ2GAMMA(u16Result_TMO_PQ2Gamma,
                                                         u16Length_Result_TMO_PQ2Gamma,
                                                         pu16pInput_Gamma_Idx,
                                                         u16curveCdfLumaHistogram512_16bit,
                                                         u16Length_Input_PQArray,
                                                         u16target_max_lum_nits);//16bit in 12 bit out for TMO
    for(i = 0; i < 16;i++)
    {
        if(0 == i)
        {
            g_uwTable[0] = (u16Result_TMO_PQ2Gamma[16] >>2)*1023/1024;
        }
        else if(15 == i)
        {
            g_uwTable[15] = (u16Result_TMO_PQ2Gamma[496]>>2)*1023/1024;
        }
        else
        {
            g_uwTable[i] = (u16Result_TMO_PQ2Gamma[i*32 + 16]>>2)*1023/1024;
        }
        printk("g_uwTable:%d\n",g_uwTable[i] );
    }
    kfree((void *)u16curveCdfLumaHistogram512_16bit);
    kfree((void *)u16curveCdfLumaHistogram512);

}
MS_U8 msDLC_linearCurve(MS_BOOL bWindow)
{
    BYTE ucTmp;
    for (ucTmp = 0; ucTmp < 16; ucTmp++)
    {
        g_uwTable[ucTmp] = 32+64*ucTmp;
    }
    msDlcWriteCurve(bWindow);
    return STATUS_SUCCESS;
}
MS_S8 msNewDlcHandler_WithCorrectionRatio(MS_BOOL bWindow)
{
    BYTE ucTmp,ucYAvgN=0;
    DWORD dwHistogramAreaSum[32];
    DWORD dwSumBelowAvg=0, dwSumAboveAvg=0;
#ifdef _Driver_IIR
    WORD uwHistogram_IIR_AreaSum[32];
    DWORD His_ratio_BelowAvg,His_ratio_AboveAvg;
#endif
    DWORD dwDLCCurve[16];
    //MS_S32 dwDLCTmoCurve[18];
    DWORD g_uwDLCResult[16];

    DWORD dwYLftAvg;
    DWORD dwYRtAvg;

    WORD uwYAvg = 0;
    uwYAvg = msGetAverageLuminous();
    MS_U16 u16ExpandHistogram[32] = {0};
    MS_U16 u16DLCcurve[16] = {0};
    MS_U16 u16LimitIdx[16] ={91,146,201,256,311,366,421,476,531,586,641,696,751,806,861,916};
    MS_U16 u16FullIdx[16] = {32,    96, 160,    224,    288,    352,    416,    480,    544,    608,    672,    736,    800,    864,    928,    992};
    MS_U16 u16LimitData[16] ={0};
    static MS_U8 u8WriteFlag = 0;
    //MS_U16 *u16Result_TMO_PQ2Gamma = (MS_U16*)kmalloc(512*sizeof(MS_U16),GFP_KERNEL);
    //g_DlcParameters.ucLMaxThreshold = 128;  //Diff over 50%
    //g_DlcParameters.ucLMinThreshold = 34; //Diff less then 13%
    //g_DlcParameters.ucLMaxCorrection = 196;  //80% correction
    //g_DlcParameters.ucLMinCorrection = 60; //23% correction

    //g_DlcParameters.ucRMaxThreshold = 100;  // Diff over 50%
    //g_DlcParameters.ucRMinThreshold = 20; //Diff less then 13%
    //g_DlcParameters.ucRMaxCorrection = 60;  //80% correction
    // g_DlcParameters.ucRMinCorrection = 20; //23% correction

    //g_DlcParameters.ucAllowLoseContrast = 0;
    //---------------------------------------------------------------------------------------------------------------------------------
#if 0
    if((MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK30_01_L, 0xFFFF)) == 0x1B)
    {
        if(0 == u8WriteFlag)
        {
            msWriteByte(REG_SC_BK30_02_L,g_DlcParameters.ucLMaxThreshold    );
            msWriteByte(REG_SC_BK30_03_L,g_DlcParameters.ucLMinThreshold    );
            msWriteByte(REG_SC_BK30_04_L,g_DlcParameters.ucLMaxCorrection   );
            msWriteByte(REG_SC_BK30_06_L,g_DlcParameters.ucLMinCorrection   );
            msWriteByte(REG_SC_BK30_08_L,g_DlcParameters.ucRMaxThreshold    );
            msWriteByte(REG_SC_BK30_0A_L,g_DlcParameters.ucRMinThreshold    );
            msWriteByte(REG_SC_BK30_0B_L,g_DlcParameters.ucRMaxCorrection   );
            msWriteByte(REG_SC_BK30_0D_L,g_DlcParameters.ucRMinCorrection   );
            msWriteByte(REG_SC_BK30_0E_L,g_DlcParameters.ucAllowLoseContrast);
            u8WriteFlag = 1;
        }
        else
        {
        g_DlcParameters.ucLMaxThreshold = msReadByte(REG_SC_BK30_02_L);
        g_DlcParameters.ucLMinThreshold = msReadByte(REG_SC_BK30_03_L);
        g_DlcParameters.ucLMaxCorrection = msReadByte(REG_SC_BK30_04_L);
        g_DlcParameters.ucLMinCorrection = msReadByte(REG_SC_BK30_06_L);
        g_DlcParameters.ucRMaxThreshold = msReadByte(REG_SC_BK30_08_L);
        g_DlcParameters.ucRMinThreshold = msReadByte(REG_SC_BK30_0A_L);
        g_DlcParameters.ucRMaxCorrection = msReadByte(REG_SC_BK30_0B_L);
        g_DlcParameters.ucRMinCorrection = msReadByte(REG_SC_BK30_0D_L);
        g_DlcParameters.ucAllowLoseContrast = msReadByte(REG_SC_BK30_0E_L);
        }
    }
#endif
    // Get the separate point "ucYAvgN" at Y average
    HistogramExpand(g_wLumaHistogram32H,u16ExpandHistogram);
    ucYAvgN = uwYAvg;
    if (ucYAvgN == 0 || ucYAvgN == 255 || 1 ==u8HistogramErrFlag ) //set DLC as linear curve
    {
        for (ucTmp = 0; ucTmp < 16; ucTmp++)
        {
            g_uwTable[ucTmp] = 32+64*ucTmp;
        }
        return STATUS_SUCCESS;
    }

    if (Get_DLCInfo_from_Histogram(ucYAvgN, u16ExpandHistogram, dwHistogramAreaSum, &dwSumBelowAvg, &dwSumAboveAvg, &dwYLftAvg, &dwYRtAvg)!=STATUS_SUCCESS)
    {
        return STATUS_ERROR;
    }

    //printf("New DLC LYavg:%d Ryavg:%d\n",dwYLftAvg,dwYRtAvg);
#ifdef _Driver_IIR
    // For IIR
    His_ratio_BelowAvg = (4096*dwSumBelowAvg + (dwHistogramAreaSum[31]/2))/dwHistogramAreaSum[31];
    His_ratio_AboveAvg = (4096*dwSumAboveAvg + (dwHistogramAreaSum[31]/2))/dwHistogramAreaSum[31];

    His_ratio_BelowAvg = min(His_ratio_BelowAvg,128);
    His_ratio_AboveAvg = min(His_ratio_AboveAvg,128);

    // For IIR before bi-histogram
    for (ucTmp=0; ucTmp<32; ucTmp++)
    {
        uwHistogram_IIR_AreaSum[ucTmp]=(MS_U16)(((1024*dwHistogramAreaSum[ucTmp])+dwHistogramAreaSum[31]/2)/dwHistogramAreaSum[31]);

        if(uwHistogram_IIR_AreaSum[ucTmp]>1024)
        {
            uwHistogram_IIR_AreaSum[ucTmp]=1024;
        }
    }
#endif

    //Get Bi-Histogram Curve "dwHistogramAreaSum"
    GetBiHistogram(dwSumBelowAvg, dwSumAboveAvg, ucYAvgN, dwHistogramAreaSum, dwDLCCurve);

    // New method get the final DLC curve, g_uwCurveHistogram.
    LimitBiHistogram(ucYAvgN, dwDLCCurve, dwYLftAvg, dwYRtAvg, g_uwDLCResult);

    ///////////// Histogram_DLC add Low_Pass_filter /////////////
#if 0
    for (ucTmp = 0; ucTmp < 16; ucTmp++)
    {
        u16DLCcurve[ucTmp] = g_uwDLCResult[ucTmp]*1023/1024;
        printk("g_uwDLCResult%d:%d\n",ucTmp,g_uwDLCResult[ucTmp]);
        //printf("First Calculation results Index:%d final_Curve:%d  g_uwDLCResult:%d\n",ucTmp,g_uwTable[ucTmp],g_uwDLCResult[ucTmp]);
    }
    DLC_18PQ2Gamma( &u16DLCcurve[0], u16Result_TMO_PQ2Gamma);
#else
    for (ucTmp = 0; ucTmp < 16; ucTmp++)
    {
        u16LimitData[ucTmp] = g_uwDLCResult[ucTmp]*876/1024 +64;
        //g_uwTable[ucTmp] = g_uwDLCResult[ucTmp]*1023/1024;
        //printk("g_uwDLCResult%d:%d\n",ucTmp,g_uwDLCResult[ucTmp]);
        //printf("First Calculation results Index:%d final_Curve:%d  g_uwDLCResult:%d\n",ucTmp,g_uwTable[ucTmp],g_uwDLCResult[ucTmp]);
        if(0 == ucTmp)
        {
            g_uwTable[ucTmp] = 35;
        }
        else if(15 == ucTmp)
        {
            g_uwTable[ucTmp] = 1010;
        }
        else
        {
            g_uwTable[ucTmp] = (((u16FullIdx[ucTmp] -u16LimitIdx[ucTmp-1])*(u16LimitData[ucTmp] - u16LimitData[ucTmp-1]))/(u16LimitIdx[ucTmp] - u16LimitIdx[ucTmp-1])) + u16LimitData[ucTmp-1];
            g_uwTable[ucTmp] = max(min(g_uwTable[ucTmp],940),64);
        }
        //printk("===========[%s][%d][Final:%d][Caculate:%d]===========\n",__FUNCTION__,__LINE__,g_uwTable[ucTmp],g_uwDLCResult[ucTmp]);
    }
#endif

    GetIIR_AlphaBlending(uwYAvg,ucYAvgN,uwHistogram_IIR_AreaSum,His_ratio_BelowAvg,His_ratio_AboveAvg);

    SetBlackWhiteCutPoint(uwYAvg);

    if(g_wLumiTotalCount > 0x0F00)
    {
        msDlcWriteCurve(bWindow);
        //printf("RIC [%s][%d]SET DLC\n",__func__,__LINE__);
    }
    //kfree((void *)u16Result_TMO_PQ2Gamma);
    return STATUS_SUCCESS;
}
void HistogramExpand(MS_U16 *u16LimitSourceIn,MS_U16 *u16FullSouceOut)
{
    //Limit Range Histogram IN ,Output Full Range Histogram
    MS_U8 u8Mode = 0,u8Mode_1,u8Index;
    MS_U8 u8BinZeroCnt = 0;
    MS_U64 u64TotalCount = 0,u64MaxCLL_Cnt = 0,u64MaxCLL = 0;
    MS_U64 u64MaxFALL = 0,u64MaxFALL_Cnt = 0;
    MS_U32 u32CodeToNits[32] = {
        1,5,17,42,84,153,259,417
,649,979,1444,2089,2976,4186,5825,8034,
10997,14959,20242,27275,36625,49047,65549,87481,
116655,155518,207383,276763,369834,495094,664315,893905};//Nits*100
    //u8Mode = msReadByte(REG_SC_BK30_0F_L)& 0x01;
    #if 0
    u8Mode_1 = (MApi_GFLIP_XC_R2BYTEMSK(REG_FSC_BK1A_04_L,0x0080))>>7;
    if(0 == u8Mode_1)
    {
        MApi_GFLIP_XC_W2BYTE(REG_FSC_BK1B_1C_L,0x036C);
        MApi_GFLIP_XC_W2BYTE(REG_FSC_BK1B_1D_L,0x0036);
    }
    else
    {
        MApi_GFLIP_XC_W2BYTE(REG_FSC_BK1B_1C_L,0x0400);
        MApi_GFLIP_XC_W2BYTE(REG_FSC_BK1B_1D_L,0x0000);
    }
    #endif

    if (g_DlcParameters.ucKernelDlcAlgorithm == 0)
    {
        u8Mode = 1;
    }
    else if (g_DlcParameters.ucKernelDlcAlgorithm == 1)   //RD DLC
    {
        u8Mode = 0;
    }

    if(0 == u8Mode)
    {
        u16FullSouceOut[0 ] = u16LimitSourceIn[2 ];
        u16FullSouceOut[1 ] = u16LimitSourceIn[3 ];
        u16FullSouceOut[2 ] = u16LimitSourceIn[4 ];
        u16FullSouceOut[3 ] = u16LimitSourceIn[5 ];

        u16FullSouceOut[5 ] = u16LimitSourceIn[6 ];
        u16FullSouceOut[6 ] = u16LimitSourceIn[7 ];
        u16FullSouceOut[7 ] = u16LimitSourceIn[8 ];
        u16FullSouceOut[8 ] = u16LimitSourceIn[9 ];
        u16FullSouceOut[9 ] = u16LimitSourceIn[10];
        u16FullSouceOut[10] = u16LimitSourceIn[11];
        u16FullSouceOut[11] = u16LimitSourceIn[12];

        u16FullSouceOut[13] = u16LimitSourceIn[13];
        u16FullSouceOut[14] = u16LimitSourceIn[14];
        u16FullSouceOut[15] = u16LimitSourceIn[15];
        u16FullSouceOut[16] = u16LimitSourceIn[16];
        u16FullSouceOut[17] = u16LimitSourceIn[17];
        u16FullSouceOut[18] = u16LimitSourceIn[18];

        u16FullSouceOut[20] = u16LimitSourceIn[19];
        u16FullSouceOut[21] = u16LimitSourceIn[20];
        u16FullSouceOut[22] = u16LimitSourceIn[21];
        u16FullSouceOut[23] = u16LimitSourceIn[22];
        u16FullSouceOut[24] = u16LimitSourceIn[23];
        u16FullSouceOut[25] = u16LimitSourceIn[24];
        u16FullSouceOut[26] = u16LimitSourceIn[25];

        u16FullSouceOut[28] = u16LimitSourceIn[26];
        u16FullSouceOut[29] = u16LimitSourceIn[27];
        u16FullSouceOut[30] = u16LimitSourceIn[28];
        u16FullSouceOut[31] = u16LimitSourceIn[29];

        u16FullSouceOut[4 ] = (u16FullSouceOut[3 ] + u16FullSouceOut[5 ] + 1 )/2;
        u16FullSouceOut[12] = (u16FullSouceOut[11] + u16FullSouceOut[13] + 1 )/2;
        u16FullSouceOut[19] = (u16FullSouceOut[18] + u16FullSouceOut[20] + 1 )/2;
        u16FullSouceOut[27] = (u16FullSouceOut[26] + u16FullSouceOut[28] + 1 )/2;
        //u8Mode = LogPrintfOneTimes();
        for(u8Index = 0; u8Index<32;u8Index++)
        {
            //printk("Bin:%d Histogram:%d\n",u8Index,u16LimitSourceIn[u8Index]);
            if(0 == u16LimitSourceIn[u8Index])
            {
                u8BinZeroCnt++;
            }
            u64TotalCount += u16FullSouceOut[u8Index];
            u64MaxFALL += u32CodeToNits[u8Index]*u16FullSouceOut[u8Index]/100;
        }

        if(u8BinZeroCnt > 30)
        {
            u8HistogramErrFlag = 1;
        }
        else
        {
            u8HistogramErrFlag = 0;
        }
        u64MaxFALL_Cnt = u64TotalCount;
        if(u64MaxFALL_Cnt > 0)
        {
            u64MaxFALL = max(min(u64MaxFALL/u64MaxFALL_Cnt,10000),0);
        }


        for(u8Index = 31; u8Index > 0;u8Index--)
        {
            u64MaxCLL_Cnt += u16FullSouceOut[u8Index];
            u64MaxCLL += u32CodeToNits[u8Index]*u16FullSouceOut[u8Index]/100;
            if(u64MaxCLL_Cnt >  5)
            {
                if(u64MaxCLL_Cnt > 0)
                {
                    u64MaxCLL =  max(min(u64MaxCLL/u64MaxCLL_Cnt,10000),0);
                    break;
                }
            }
        }
        //printk("u32MaxFALL:%d u32MaxCLL:%d,u32MaxFALL_Cnt:%d,u32MaxCLL_Cnt:%d,u32TotalCount:%d\n",u64MaxFALL,u64MaxCLL,u64MaxFALL_Cnt,u64MaxCLL_Cnt,u64TotalCount);
        u16MaxCLL = u64MaxCLL;
        u16MaxFALL = u64MaxFALL;
    }
    else
    {
        for(u8Index = 0; u8Index<32;u8Index++)
        {
            u16FullSouceOut[u8Index] = u16LimitSourceIn[u8Index];
        }
    }

}
void msReferTMO(void)
{
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT

    MS_U16 u16tmo_size = 512;
    MS_U16 u16source_max_luma = 10000;
    MS_U16 u16panel_max_luma = 1000;
    MS_U16 u16linear_luma = 800;
    MS_U16 u16idx = 0;
    MS_U16 u16input = 0,u16input_code = 0,u16linear_in_code = 0;
    MS_U16 u16linear_out_code = 0;
    MS_U16 u16in_thrd = 0,u16out_thrd = 0,u16upper_bound;
    MS_U16 u16output = 0;
    MS_U32 u32norm_in = 0,u32gamma_out = 0;
    //in = 0:8:4088;
    //in = in';
    //in(512) = in(511);
    //tmo_lut = in;
    MS_U32 u32In = 0;
    MS_U32 u32Gamma = 0x7FFF;
    MS_U16 u16A = 0x8000;
    MS_U16 u16B = 0;
    MS_U16 u16S = 0x0100;
    MS_U8 u8BitDepth = 14;
    MS_U8 sMode = 0;

    MS_U16 *u16curve512 = (MS_U16*)kmalloc(512 * sizeof(MS_U16), GFP_KERNEL);

    //FILE *p;
    //p = fopen("RefernceTMO.txt","w");
    u16panel_max_luma = MApi_GFLIP_XC_R2BYTE(REG_SC_BK4E_27_L);
    u16panel_max_luma = max(u16panel_max_luma,u16linear_luma);
    u16linear_in_code = (MS_U16)Luminance_To_GammaCode(u16linear_luma, u16source_max_luma);
    u16linear_out_code = (MS_U16)Luminance_To_GammaCode(u16linear_luma, u16panel_max_luma);
    u16in_thrd = min((u16linear_in_code*0xFF8)/0xFFFC,0xFF8);
    u16out_thrd =min((u16linear_out_code*0xFF8)/0xFFFC,0xFF8);
    for( u16idx= 0;u16idx <u16tmo_size;u16idx++)
    {
        u16input = (u16idx*8);// 4088(0xFF8) as 1;
        u16input_code = u16input;
       // u16code_in[u16idx] = u16input_code;


        u16upper_bound = 0xFF8 - u16out_thrd;

        if (u16input_code <= u16in_thrd )
        {
            u16output = (u16input_code*u16out_thrd)/max(u16in_thrd,1); //output 0xFF8 as 1
        }
        else
        {
            u32norm_in = ((u16input_code- u16in_thrd)*0x10000ul)/max((0xFF8-u16in_thrd),1);
            u32In = min(u32norm_in,0x10000ul);
            //Gammasampling function input 0x10000 as 1 ,output 0xFFFC as 1
            u32gamma_out =Mhal_CFD_GammaSampling( u32In,  u32Gamma,  u16A,  u16B,  u16S,  u8BitDepth,  sMode);//Gamma^(1/2)
            u16output = (u32gamma_out*u16upper_bound)/0xFFFC + u16out_thrd;
        }


        //u16tmo_lut[u16idx] = min(((u16output*4095)/4088),4095);
        u16curve512[u16idx] = min(((u16output*4095)/4088),4095);

        //fprintf(p,"u16tmo_lut[%d]:%x\n",u16idx,u16tmo_lut[u16idx]);
    }
    //writeMdTmo(u16tmo_lut, 0,u16source_max_luma,u16panel_max_luma);
    writeMdTmo(u16curve512, 0,u16source_max_luma,u16panel_max_luma);

kfree((void *)u16curve512);

#endif
}
MS_U32 Luminance_To_GammaCode(MS_U16 u16Nits_in, MS_U16 u16maxLuma)
{
    MS_U32 u32Gamma_code = 0;
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
    MS_U32 u32In = 0;
    MS_U32 u32Gamma = 0x745D;
    MS_U16 u16A = 0x8000;
    MS_U16 u16B = 0;
    MS_U16 u16S = 0x0100;
    MS_U8 u8BitDepth = 14;
    MS_U8 sMode = 0;
    /* Gamma 2.2
    sMode = 0;
    //stDeGammaData->sData[0] = 0x8CCD>>2; // 2.2
    //stDeGammaData->sData[1] = 0;
    u32Gamma = 0x745D;
    u32GammaInv = 0x23333;
    u16Beta = 0;
    u16Alpha = 0x8000;
    u16LowerBoundGain  = 0x0100;;
    */
    u32In = min((u16Nits_in*0x10000ul/max(u16maxLuma,1)),0x10000ul);
    //Gammasampling function input 0x10000 as 1 ,output 0xFFFC as 1
    u32Gamma_code = Mhal_CFD_GammaSampling( u32In,  u32Gamma,  u16A,  u16B,  u16S,  u8BitDepth,  sMode);
#endif
    return u32Gamma_code;
}
void RefernceTMOSetting(MS_U8 u8Mode)
{
    static MS_U8 u8RestoreFlag = 0;
    static MS_U16 u16FSC_Color_Engine = 0,u16TMO_user_alpha = 0;
    if(2 != u8Mode &&0 ==u8RestoreFlag)
    {
        //u16FSC_Color_Engine = MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK18_06_L,0x0400);
        //u16TMO_user_alpha = MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK7A_3A_L,0x001F);
    }

    if(2 == u8Mode && 0 ==u8RestoreFlag)//Refernce TMO setting
    {
        //printk("ReferTMO\n");
        //MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK18_06_L, 0x0400, 0x0400);
        //MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK7A_3A_L, 0x0000, 0x001F);
        u8RestoreFlag = 1;
    }
    else if(2 != u8Mode && 1 == u8RestoreFlag ) //Restore to original setting
    {
        //printk("DefaultTMO\n");
        //MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK18_06_L, u16FSC_Color_Engine, 0x0400);
        //MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK7A_3A_L, u16TMO_user_alpha, 0x001F);
        printk("u16FSC_Color_Engine:%x,u16TMO_user_alpha:%x\n",u16FSC_Color_Engine,u16TMO_user_alpha);
        u8RestoreFlag = 0;
    }
    else
    {
        //donothing
    }

}
MS_U8 LogPrintfOneTimes(void)
{
    return 0;
    /*
    MS_U8 u8Control;
    u8Control = msReadByte(REG_SC_BK30_0F_L);
    if(u8Control & 0x01)
    {

        msWriteByte(REG_SC_BK30_0F_L, u8Control&0xF0);
        return 1;
    }
    else
    {
        return 0;
    }
    */
}
MS_U8 GetDlcFunctionControl(void)
{
    return 0;
    /*
    MS_U8 u8Control,u8LogControl = 0;
    static MS_U8 u8PreLogControl = 0;
    msDlc_FunctionEnter();
    u8Control = msReadByte(REG_SC_BK30_0D_L);

    msDlc_FunctionExit();
    return u8Control & 0x1;
    */
}
void DisableDlcFunctionControl(void)
{
    /*
    msDlc_FunctionEnter();
    msWriteByte(REG_SC_BK30_0D_L, 0x00);
    msDlc_FunctionExit();
    */
}

#if 0

void getIPIdx( void )
{
    sMemIdx.u8IPMIdx = MApi_GFLIP_XC_R2BYTEMSK((0x131200 | 0x3A<< 1), 0x000F);
}

void getOPIdx( void )
{
    sMemIdx.u8OPMIdx = MApi_GFLIP_XC_R2BYTEMSK((0x131200 | (0x3A<<1)), 0x00F0)>>4;
    sMemIdx.u8RWDiff = ( MApi_GFLIP_XC_R2BYTEMSK((0x131200 | (0x06<<1)), 0x8000)>>12 ) + ( MApi_GFLIP_XC_R2BYTEMSK((0x131200 | (0x07<<1)), 0xE000)>>13 );
    sMemIdx.u8MaxIdx = MApi_GFLIP_XC_R2BYTEMSK((0x131200 | (0x19<<1)), 0x001F);
    MS_S8 s8diff = 1;
    sMemIdx.u8OPDiff = (s8diff > 0) ? s8diff : (MS_U8)(s8diff + (MS_S8)sMemIdx.u8MaxIdx); //modify to dummy
}

void DlcInit( DlcCtrl* pDlcCtrl )
{

}

void DlcParseRegister( DlcCtrl* pDlcCtrl )
{
    static MS_BOOL isRegInitialized = false;

    DlcRegister *pDlcReg = &pDlcCtrl->dlcReg;
    pDlcReg->ucIIRRatio = 0x40;
    if((MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK30_01_L, 0xFFFF)) == 0x1B)
    {
        if( isRegInitialized )
        {
            pDlcReg->ucLMaxThreshold = msReadByte(REG_SC_BK30_02_L);
            pDlcReg->ucLMinThreshold = msReadByte(REG_SC_BK30_03_L);
            pDlcReg->ucLMaxCorrection = msReadByte(REG_SC_BK30_04_L);
            pDlcReg->ucLMinCorrection = msReadByte(REG_SC_BK30_06_L);
            pDlcReg->ucRMaxThreshold = msReadByte(REG_SC_BK30_08_L);
            pDlcReg->ucRMinThreshold = msReadByte(REG_SC_BK30_0A_L);
            pDlcReg->ucRMaxCorrection = msReadByte(REG_SC_BK30_0B_L);
            pDlcReg->ucRMinCorrection = msReadByte(REG_SC_BK30_0D_L);
            pDlcReg->ucAllowLoseContrast = msReadByte(REG_SC_BK30_0E_L);
            pDlcReg->ucIIRRatio = msReadByte(REG_SC_BK30_0F_L);
            pDlcReg->ucBLevel =  msReadByte(REG_SC_BK30_07_H);
            pDlcReg->ucWLevel = msReadByte(REG_SC_BK30_09_H);
        }
        else
        {
            isRegInitialized = true;

            msWriteByte( REG_SC_BK30_02_L, pDlcReg->ucLMaxThreshold );
            msWriteByte( REG_SC_BK30_03_L, pDlcReg->ucLMinThreshold );
            msWriteByte( REG_SC_BK30_04_L, pDlcReg->ucLMaxCorrection );
            msWriteByte( REG_SC_BK30_06_L, pDlcReg->ucLMinCorrection );
            msWriteByte( REG_SC_BK30_08_L, pDlcReg->ucRMaxThreshold );
            msWriteByte( REG_SC_BK30_0A_L, pDlcReg->ucRMinThreshold );
            msWriteByte( REG_SC_BK30_0B_L, pDlcReg->ucRMaxCorrection );
            msWriteByte( REG_SC_BK30_0D_L, pDlcReg->ucRMinCorrection );
            msWriteByte( REG_SC_BK30_0E_L, pDlcReg->ucAllowLoseContrast );
            msWriteByte( REG_SC_BK30_0F_L, pDlcReg->ucIIRRatio );
            msWriteByte( REG_SC_BK30_07_H, STU_UltraBW.u8Black_level );
            msWriteByte( REG_SC_BK30_09_H, STU_UltraBW.u8White_level );
        }
    }
    else
    {
        isRegInitialized = false;

        pDlcReg->ucLMaxThreshold = g_DlcParameters.ucLMaxThreshold;
        pDlcReg->ucLMinThreshold = g_DlcParameters.ucLMinThreshold;
        pDlcReg->ucLMaxCorrection = g_DlcParameters.ucLMaxCorrection;
        pDlcReg->ucLMinCorrection = g_DlcParameters.ucLMinCorrection;
        pDlcReg->ucRMaxThreshold = g_DlcParameters.ucRMaxThreshold;
        pDlcReg->ucRMinThreshold = g_DlcParameters.ucRMinThreshold;
        pDlcReg->ucRMaxCorrection = g_DlcParameters.ucRMaxCorrection;
        pDlcReg->ucRMinCorrection = g_DlcParameters.ucRMinCorrection;
        pDlcReg->ucAllowLoseContrast = g_DlcParameters.ucAllowLoseContrast;
        pDlcReg->ucBLevel =  STU_UltraBW.u8Black_level;
        pDlcReg->ucWLevel = STU_UltraBW.u8White_level;
    }

 #ifdef TEMP_SOL
    pDlcReg->bHistLimitToFull = true;
    pDlcReg->bCurveFullToLimit = true;
    //pDlcReg->ucIIRRatio = msReadByte(REG_SC_BK30_0F_L);//0x20;
#endif
}

void DlcExpandHistogram( DlcCtrl* pDlcCtrl )
{
    volatile MS_U32 *pLimitSourceIn = pDlcCtrl->pHistStat->u32IpLumaHist;
    MS_U32 *pFullSouceOut = pDlcCtrl->dlcStat.u32IpLumaHistFull;
    if( pDlcCtrl->dlcReg.bHistLimitToFull )
    {
        pFullSouceOut[0 ] = pLimitSourceIn[2 ];
        pFullSouceOut[1 ] = pLimitSourceIn[3 ];
        pFullSouceOut[2 ] = pLimitSourceIn[4 ];
        pFullSouceOut[3 ] = pLimitSourceIn[5 ];

        pFullSouceOut[5 ] = pLimitSourceIn[6 ];
        pFullSouceOut[6 ] = pLimitSourceIn[7 ];
        pFullSouceOut[7 ] = pLimitSourceIn[8 ];
        pFullSouceOut[8 ] = pLimitSourceIn[9 ];
        pFullSouceOut[9 ] = pLimitSourceIn[10];
        pFullSouceOut[10] = pLimitSourceIn[11];
        pFullSouceOut[11] = pLimitSourceIn[12];

        pFullSouceOut[13] = pLimitSourceIn[13];
        pFullSouceOut[14] = pLimitSourceIn[14];
        pFullSouceOut[15] = pLimitSourceIn[15];
        pFullSouceOut[16] = pLimitSourceIn[16];
        pFullSouceOut[17] = pLimitSourceIn[17];
        pFullSouceOut[18] = pLimitSourceIn[18];

        pFullSouceOut[20] = pLimitSourceIn[19];
        pFullSouceOut[21] = pLimitSourceIn[20];
        pFullSouceOut[22] = pLimitSourceIn[21];
        pFullSouceOut[23] = pLimitSourceIn[22];
        pFullSouceOut[24] = pLimitSourceIn[23];
        pFullSouceOut[25] = pLimitSourceIn[24];
        pFullSouceOut[26] = pLimitSourceIn[25];

        pFullSouceOut[28] = pLimitSourceIn[26];
        pFullSouceOut[29] = pLimitSourceIn[27];
        pFullSouceOut[30] = pLimitSourceIn[28];
        pFullSouceOut[31] = pLimitSourceIn[29];

        pFullSouceOut[4 ] = (pFullSouceOut[3 ] + pFullSouceOut[5 ] + 1 )/2;
        pFullSouceOut[12] = (pFullSouceOut[11] + pFullSouceOut[13] + 1 )/2;
        pFullSouceOut[19] = (pFullSouceOut[18] + pFullSouceOut[20] + 1 )/2;
        pFullSouceOut[27] = (pFullSouceOut[26] + pFullSouceOut[28] + 1 )/2;
    }
    else
    {
        pFullSouceOut = pLimitSourceIn;
    }
}

void DlcBiHistogramInfo( DlcCtrl* pDlcCtrl )
{
    MS_U32 *pLumaHist = pDlcCtrl->dlcStat.u32IpLumaHistFull;
    MS_U32 *pHistAccuSum = pDlcCtrl->dlcStat.u32HistAccuSum;

    MS_U64 YSum, YCount;
    MS_U32 leftBinPix, rightBinPix;
    MS_U8 ucBin, ucAvgBin;

    // 8 bits MSB sum
    MS_U32 avgY = pDlcCtrl->pHistStat->u32IpLumaTotalSum;
    // total pixel
    MS_U32 pixCnt = pDlcCtrl->pHistStat->u32IpLumaTotalCount;

    if( pixCnt != 0 )
        avgY = ( ( (MS_U64)avgY << 2 ) + (pixCnt>>1) ) / pixCnt;
    else
        avgY = 0;

    // limit range signal to full range signal
    if( pDlcCtrl->dlcReg.bHistLimitToFull )
    {
        avgY = min( max( avgY - 64, 0 ) * 1023 / (940-64), 1023 );
    }
    pDlcCtrl->dlcStat.u16PrevLumaAvg = pDlcCtrl->dlcStat.u16LumaAvg;
    pDlcCtrl->dlcStat.u16LumaAvg = avgY;
    pDlcCtrl->dlcStat.u32TotalCount = pixCnt;

    // Calculate left and and right mean separated by the YAvg;
    pHistAccuSum[0] = pLumaHist[0];
    for( ucBin = 1 ; ucBin < 32 ; ++ucBin )
    {
        pHistAccuSum[ucBin] = pHistAccuSum[ucBin-1] + pLumaHist[ucBin];
    }

    if( pHistAccuSum[31] == 0 )
    {
        HDR_DBG_HAL(DLC_DEBUG("\n Error in msDlcHandler(), dwHistogramAreaSum[31] = 0"));
    }

    // left average
    YSum = YCount = 0;
    ucAvgBin = (avgY>>5);
    leftBinPix = (pLumaHist[ucAvgBin] * ( avgY-(ucAvgBin<<5) ) + 16) >> 5;
    rightBinPix = max( pLumaHist[ucAvgBin] - leftBinPix, 0 );
    for( ucBin = 0 ; ucBin <= ucAvgBin ; ++ucBin )
    {
        if( ucBin == ucAvgBin )
        {
            YSum += ( ((MS_U64)ucBin<<5) + 16 ) * leftBinPix;
            YCount += leftBinPix;
        }
        else
        {
            YSum += ( ((MS_U64)ucBin<<5) + 16 ) * pLumaHist[ucBin];
            YCount += pLumaHist[ucBin];
        }
    }
    pDlcCtrl->dlcStat.u32SumBelowAvg = YCount;
    pDlcCtrl->dlcStat.u16YLftAvg = min( YSum / max(YCount,1), avgY );

    // right average
    YSum = YCount = 0;
    for( ucBin = ucAvgBin ; ucBin < 32 ; ++ucBin )
    {
        if( ucBin == ucAvgBin )
        {
            YSum += ( ((MS_U64)ucBin<<5) + 16 ) * rightBinPix;
            YCount += rightBinPix;
        }
        else
        {
            YSum += ( ((MS_U64)ucBin<<5) + 16 ) * pLumaHist[ucBin];
            YCount += pLumaHist[ucBin];
        }
    }
    pDlcCtrl->dlcStat.u32SumAboveAvg = YCount;
    pDlcCtrl->dlcStat.u16YRtAvg = max( YSum / max(YCount,1), avgY );
}

void DlcGetBiHistogram( DlcCtrl* pDlcCtrl )
{
    MS_U8 ucBin;
    MS_U32 scaler;
    MS_U32 lumaAvg = (MS_U32)pDlcCtrl->dlcStat.u16LumaAvg;
    MS_U64 u64SumBelowAvg = (MS_U64)pDlcCtrl->dlcStat.u32SumBelowAvg;
    MS_U64 u64SumAboveAvg = (MS_U64)pDlcCtrl->dlcStat.u32SumAboveAvg;
    MS_U32 *pHistAccuSum = pDlcCtrl->dlcStat.u32HistAccuSum;
    MS_U16 *u16DlcCurve = pDlcCtrl->dlcStat.u16DlcCurve;
    MS_U16 u16DlcCurve32Bin[32];

    // Bi-Histogram, rescale the histogram equalization to the separate point
    for( ucBin = 0 ; ucBin < 32 ; ++ucBin )
    {
        if( ( ((MS_U32)ucBin+1) << 5 ) < lumaAvg )  // points at the left side of StepPoint
        {
            // 8 bits scaling
            scaler = ( ( (MS_U64)pHistAccuSum[ucBin] << 8 ) + (u64SumBelowAvg>>1) ) / u64SumBelowAvg;
            u16DlcCurve32Bin[ucBin] = (lumaAvg*scaler) >> 8;
        }
        else
        {
            scaler = ( ( ( (MS_U64)pHistAccuSum[ucBin]-u64SumBelowAvg ) << 8 ) + (u64SumAboveAvg>>1) ) / u64SumAboveAvg;
            u16DlcCurve32Bin[ucBin] = lumaAvg + ( ((1023-lumaAvg)*scaler) >> 8 );
        }
    }

    //down sampling to 16
    for( ucBin = 0 ; ucBin < 16 ; ++ucBin )
    {
        u16DlcCurve[ucBin] = u16DlcCurve32Bin[ (ucBin<<1) ];
    }
}

void DlcGetStrengthScore( DlcCtrl* pDlcCtrl )
{
    DlcRegister *pDlcReg = &pDlcCtrl->dlcReg;
    MS_U16 *u16DlcCurve = pDlcCtrl->dlcStat.u16DlcCurve;

    MS_U32 u32LeftCutScore = 128, u32RightCutScore = 128;
    MS_U32 u32LetfDiffRatio = 0, u32RightDiffRatio = 0;
    MS_U8 leftBin = 0, rightBin = 0;
    MS_U8 ucSepPoint = pDlcCtrl->dlcStat.u16LumaAvg >> 6;

    leftBin = (MS_U8)( pDlcCtrl->dlcStat.u16YLftAvg >> 6 );
    // DiffRatio = Diff/Total *128
    u32LetfDiffRatio = (MS_U32) abs( (MS_S32)u16DlcCurve[leftBin] - ( ((MS_S32)leftBin<<6) + 32 ) );
    u32LetfDiffRatio = ( u32LetfDiffRatio << 8 ) / max(u16DlcCurve[ucSepPoint],1);

    if( u32LetfDiffRatio > pDlcReg->ucLMaxThreshold ) //Diff too much, ucBelowScore small
        u32LeftCutScore = pDlcReg->ucLMinCorrection;
    else if( u32LetfDiffRatio < pDlcReg->ucLMinThreshold ) //Diff is small, ucBelowScore big
        u32LeftCutScore = pDlcReg->ucLMaxCorrection;
    else
        u32LeftCutScore = ( (pDlcReg->ucLMaxThreshold-u32LetfDiffRatio) * pDlcReg->ucLMaxCorrection +
                                 (u32LetfDiffRatio-pDlcReg->ucLMinThreshold) * pDlcReg->ucLMinCorrection  ) /
                        max((MS_S32)pDlcReg->ucLMaxThreshold-(MS_S32)pDlcReg->ucLMinThreshold, 1);

    rightBin = (MS_U8)( pDlcCtrl->dlcStat.u16YRtAvg >> 6 );
    // DiffRatio = Diff/Total *128
    u32RightDiffRatio = (MS_U32) abs( (MS_S32)u16DlcCurve[rightBin] - ( ((MS_S32)rightBin<<6) + 32 ) );
    u32RightDiffRatio = ( u32RightDiffRatio << 8 ) / max(1023-u16DlcCurve[ucSepPoint],1);

    if( u32RightDiffRatio > pDlcReg->ucRMaxThreshold) //Diff too much, choice a small correction
        u32RightCutScore = pDlcReg->ucRMinCorrection;
    else if (u32RightDiffRatio < pDlcReg->ucRMinThreshold) //Diff is small, choice the maximum correction
        u32RightCutScore = pDlcReg->ucRMaxCorrection;
    else
        u32RightCutScore = ( (pDlcReg->ucRMaxThreshold-u32RightDiffRatio) * pDlcReg->ucRMaxCorrection +
                                 (u32RightDiffRatio-pDlcReg->ucRMinThreshold) * pDlcReg->ucRMinCorrection  ) /
                        max((MS_S32)pDlcReg->ucRMaxThreshold-(MS_S32)pDlcReg->ucRMinThreshold,1);

    pDlcCtrl->dlcStat.u32LeftCutScore = u32LeftCutScore;
    pDlcCtrl->dlcStat.u32RightCutScore = u32RightCutScore;
}

void DlcLeftRightMinSlop( DlcCtrl* pDlcCtrl )
{
    MS_U32 u32Slop[17], u32SlopRatioL = 256, u32SlopRatioH = 256;
    MS_U8 ucBin;
    MS_U16 *u16DlcCurve = pDlcCtrl->dlcStat.u16DlcCurve;
    MS_U8 ucSepPoint = pDlcCtrl->dlcStat.u16LumaAvg >> 6;

    //histogram_DLC limit by slope
    for( ucBin = 0 ; ucBin <= 16 ; ++ucBin )
    {
        if( ucBin == 0 )
        {
            u32Slop[ucBin] = ( ( (MS_U32)u16DlcCurve[ucBin] << 8 ) + 16 ) >> 5;
        }
        else if( ucBin == 16 )
        {
            u32Slop[ucBin] = ( ( (MS_U32)(1024 - u16DlcCurve[ucBin-1]) << 8 ) + 16) >> 5;
        }
        else
        {
            // slop = vertical diff / horizontal size 64 -> 8 bit floating point(*256)
            u32Slop[ucBin] = ( ( (MS_U32)(u16DlcCurve[ucBin] - u16DlcCurve[ucBin-1]) << 8 ) + 32 ) >> 6;
        }
        u32Slop[ucBin] = max(u32Slop[ucBin],257); // To prevent div by 0
        // calculate all the correction parameter "dwHistogramDlcSlop" if over limit slop
        // limit slop = 7 bit floating point. 0 = 1x, 128 = 2x, maximum 256.
        u32Slop[ucBin] = ( ( (MS_U32)g_DlcParameters.ucDlcHistogramLimitCurve[ucBin] << 9 ) + ((u32Slop[ucBin]-256)>>1) )
            / (u32Slop[ucBin]-256); //SlopRatio 8bit 256=1x

        u32Slop[ucBin] = min( u32Slop[ucBin], 0xFFFF );
    }

    for( ucBin = 0 ; ucBin <= (ucSepPoint); ++ucBin )
    {
        u32SlopRatioL = min( u32Slop[ucBin], u32SlopRatioL );
    }

    for( ucBin = ucSepPoint ; ucBin <= 16 ; ++ucBin )
    {
        u32SlopRatioH = min( u32Slop[ucBin], u32SlopRatioH );
    }

    pDlcCtrl->dlcStat.u32SlopRatioL = u32SlopRatioL;
    pDlcCtrl->dlcStat.u32SlopRatioH = u32SlopRatioH;
}

void DlcLimitBiHistogram( DlcCtrl* pDlcCtrl )
{
    MS_U8 ucBin;
    MS_U16 u16LinearY;
    MS_U32 u32HistogramSlopRatioL, u32HistogramSlopRatioH;
    MS_U16 *u16DlcCurve = pDlcCtrl->dlcStat.u16DlcCurve;
    //MS_U8 ucSepPoint = pDlcCtrl->dlcStat.u16LumaAvg >> 6;
    MS_U8 ucAllowLoseContrast = pDlcCtrl->dlcReg.ucAllowLoseContrast;

    //Use Left Yavg and Right YAvg to decide the strength of the DLC curve.
    DlcGetStrengthScore( pDlcCtrl );

    // Use Limited Slop, provided by users, to limit the DLC curve.
    DlcLeftRightMinSlop( pDlcCtrl );

    // Get the final ratio for DLC Curve.
    u32HistogramSlopRatioL = (pDlcCtrl->dlcStat.u32LeftCutScore) * (pDlcCtrl->dlcStat.u32SlopRatioL) >> 8;
    u32HistogramSlopRatioH = (pDlcCtrl->dlcStat.u32RightCutScore) * (pDlcCtrl->dlcStat.u32SlopRatioH) >> 8;

    // calcute final target curve for sepPoint
    for( ucBin = 0 ; ucBin < 16 ; ++ucBin )
    {
        u16LinearY = ( (MS_U16)ucBin << 6 ) + 32;
        //if( ucSepPoint >= ucBin )
        if( u16LinearY < pDlcCtrl->dlcStat.u16LumaAvg )
        {
            if( u16DlcCurve[ucBin] > u16LinearY )
            {
                if( ucAllowLoseContrast )
                {
                    u16DlcCurve[ucBin] = u16LinearY
                        + ( ( (u16DlcCurve[ucBin] - u16LinearY) * u32HistogramSlopRatioL + 128 ) >> 8 );
                }
                else
                    u16DlcCurve[ucBin] = u16LinearY;
            }
            else
            {
                u16DlcCurve[ucBin] = u16LinearY
                    - ( ( (u16LinearY - u16DlcCurve[ucBin]) * u32HistogramSlopRatioL + 128 ) >> 8 );
            }
        }
        else
        {
            if( u16DlcCurve[ucBin] > u16LinearY )
            {
                u16DlcCurve[ucBin] = u16LinearY
                    + ( ( (u16DlcCurve[ucBin] - u16LinearY) * u32HistogramSlopRatioH + 128 ) >> 8 );
            }
            else
            {
                if( ucAllowLoseContrast )
                    u16DlcCurve[ucBin] = u16LinearY
                        - ( ( (u16LinearY- u16DlcCurve[ucBin]) * u32HistogramSlopRatioH + 128 ) >> 8 );
                else
                    u16DlcCurve[ucBin] = u16LinearY;
            }
        }
    }
}

void DlcCurveLowPass( DlcCtrl* pDlcCtrl )
{
    MS_U8 ucBin;
    MS_U16 u16LpfCurve[16];
    MS_U16 *u16DlcCurve = pDlcCtrl->dlcStat.u16DlcCurve;

    for( ucBin = 0 ; ucBin < 16 ; ++ucBin )
    {
        if (ucBin == 0)
        {
            u16LpfCurve[ucBin] = ( /*0*2+*/u16DlcCurve[ucBin] * 5 + u16DlcCurve[ucBin+1] + 4 ) >> 3;
        }
        else if (ucBin == 15)
        {
            u16LpfCurve[ucBin] = ( u16DlcCurve[ucBin-1] + u16DlcCurve[ucBin] * 5 + (1023<<1) + 4) >> 3;
        }
        else
        {
            u16LpfCurve[ucBin] = ( u16DlcCurve[ucBin-1] + u16DlcCurve[ucBin] * 6 + u16DlcCurve[ucBin+1] + 4 ) >> 3;
        }
    }

    memcpy( pDlcCtrl->dlcStat.u16DlcCurve, u16LpfCurve, sizeof(MS_U16)*16 );
}

void DlcSingleBinDetect( DlcCtrl* pDlcCtrl )
{
    MS_U8 ucTmp = 0;
    MS_U32 binMax = 0;
    MS_U32 hist = 0;
    MS_U32 totalCount = pDlcCtrl->dlcStat.u32TotalCount;
    MS_U32 thMin = ( totalCount * 128 ) >> 8;//min th of ratio 0.5
    MS_U32 thMax = ( totalCount * 230 ) >> 8;//max th of ratio 0.9
    MS_U8 SingleBinW = 0;

    for( ucTmp = 0 ; ucTmp < 32 ; ++ucTmp )
    {
        hist = pDlcCtrl->pHistStat->u32IpLumaHist[ucTmp];
        if( hist > binMax )
            binMax = hist;
    }

    if( binMax > thMin )
        SingleBinW = ( (binMax-thMin) << 6 ) / (thMax-thMin);
    else
        SingleBinW = 0;

    pDlcCtrl->dlcStat.ucSingleBinW = SingleBinW;
}

void DlcSceneChangeDetect( DlcCtrl* pDlcCtrl )
{
    MS_U8 ucTmp = 0;
    MS_S32 histCurrent = 0;
    MS_S32 histPrev = 0;
    MS_U32 histDiff = 0;
    MS_S16 dlcCurrent = 0;
    MS_S16 dlcPrev = 0;
    MS_U16 dlcDiff = 0;
    MS_U16 avgCurrent = pDlcCtrl->dlcStat.u16LumaAvg;
    MS_U16 avgPrev = pDlcCtrl->dlcStat.u16PrevLumaAvg;
    MS_U16 avgDiff = 0;
    MS_U16 SceneChangeW = 0;
    MS_U8 weightHistDiff = 16;
    MS_U8 weightDlcDiff  = 16;
    MS_U8 weightAvgDiff  = 16;

    MS_U32 thHistDiffMin = 64;
    MS_U32 HistDiffGain = 64;

    MS_U16 thDlcDiffMin = 16;
    MS_U16 DlcDiffGain = 64;

    MS_U16 thAvgDiffMin = 16;
    MS_U16 AvgDiffGain = 64;

    //sum of hist diff
    for( ucTmp = 0 ; ucTmp < 32 ; ++ucTmp )
    {
        histCurrent = (MS_S32)pDlcCtrl->pHistStat->u32IpLumaHist[ucTmp];
        histPrev = (MS_S32)pDlcCtrl->pPrevHistStat->u32IpLumaHist[ucTmp];
        histDiff += (MS_U32) abs(histCurrent-histPrev);
    }

    //sum of dlc curve diff
    for( ucTmp = 0 ; ucTmp < 16 ; ++ucTmp )
    {
        dlcCurrent = (MS_S16)pDlcCtrl->dlcStat.u16DlcCurve[ucTmp];
        dlcPrev = (MS_S16)pDlcCtrl->dlcStat.u16PrevDlcCurve[ucTmp];
        dlcDiff += (MS_U16)abs(dlcCurrent-dlcPrev);
    }

    // hist diff normalization
    histDiff = ( (MS_U64)histDiff << 8 ) / (pDlcCtrl->dlcStat.u32TotalCount);
    histDiff = histDiff > thHistDiffMin ? histDiff-thHistDiffMin : 0;
    histDiff = min( (histDiff*HistDiffGain) >> 6, 64);

    dlcDiff = dlcDiff >> 4;//dlc diff normalization
    dlcDiff = dlcDiff > (thDlcDiffMin<<2) ? dlcDiff-(thDlcDiffMin<<2) : 0;
    dlcDiff = min((dlcDiff*DlcDiffGain)>>6,64);

    avgDiff = (avgCurrent > avgPrev) ? (avgCurrent - avgPrev) : (avgPrev - avgCurrent);
    avgDiff = avgDiff > (thAvgDiffMin<<2) ? avgDiff-(thAvgDiffMin<<2) : 0;
    avgDiff = min((avgDiff*AvgDiffGain)>>6,64);

    SceneChangeW = max((histDiff*weightHistDiff)>>4,(dlcDiff*weightDlcDiff)>>4);
    SceneChangeW = max(SceneChangeW,(avgDiff*weightAvgDiff)>>4);
    SceneChangeW = max(min(SceneChangeW,64),0);

    pDlcCtrl->dlcStat.ucSceneChangeW = SceneChangeW;

#if 1
    if( SceneChangeW == 64 )
        DBG_LIGHT( 4, 0, 0x7777, 0xFFFF );
    else if( SceneChangeW > 32 )
        DBG_LIGHT( 4, 0, 0x2222, 0xFFFF );
    else
        DBG_LIGHT( 4, 0, 0x0000, 0xFFFF );
#endif
}

void DlcLinearBlending( DlcCtrl* pDlcCtrl )
{
    MS_U8 ucBin;
    MS_U16 *u16DlcCurve = pDlcCtrl->dlcStat.u16DlcCurve;
    MS_U8 ucSingleBinW = 64;//64 - pDlcCtrl->dlcStat.ucSingleBinW;

    for( ucBin = 0 ; ucBin < 16 ; ++ucBin )
    {
        MS_U16 linearY = ( (MS_U16)ucBin << 6 ) + 32;
        if( u16DlcCurve[ucBin] > linearY )
        {
            u16DlcCurve[ucBin] = linearY
                + (MS_U16)( ( (MS_U32)( u16DlcCurve[ucBin] - linearY ) * ucSingleBinW ) >> 6 );
        }
        else
        {
            u16DlcCurve[ucBin] = linearY
                - (MS_U16)( ( (MS_U32)( linearY - u16DlcCurve[ucBin] ) * ucSingleBinW ) >> 6 );
        }
    }
}

void DlcCurveRangeConvert( DlcCtrl* pDlcCtrl )
{
    MS_U32 preGain = 0x400, preOfs = 0;
    MS_U32 postGain = 0x400;
    MS_U8 ucWLevel = pDlcCtrl->dlcReg.ucWLevel;
    MS_U8 ucBLevel = pDlcCtrl->dlcReg.ucBLevel;
    MS_U16 u16WLevel = 0,u16BLevel= 0;

    if(pDlcCtrl->dlcReg.bCurveFullToLimit)
    {
        ucBLevel = min( 0x80, ucBLevel );
        ucWLevel = min( 0x80, ucWLevel );

        if(ucBLevel == 0 && ucWLevel == 0)
        {
            u16BLevel = 64 - ( 64 * ucBLevel >> 7 );
            u16WLevel = 940 + ( 83 * ucWLevel >> 7 );
        }
        else
        {
            u16BLevel = 64 - ( 64 * ucBLevel >> 7 );
            //printk("ucBLevel:%d,u16BLevel:%d %d\n",ucBLevel,u16BLevel,__LINE__);
            u16BLevel = (u16BLevel*876)/1023 +64;

            u16WLevel = 940 + ( 83 * ucWLevel >> 7 );
            //printk("ucWLevel:%d,u16WLevel:%d %d\n",ucWLevel,u16WLevel,__LINE__);
            u16WLevel = (u16WLevel*876)/1023 +64;
        }
        //printk("u16WLevel:%d,u16BLevel:%d\n",u16WLevel,u16BLevel);
        // pre gain -> pre offset
        preGain = ( (MS_U32)(1023 << 11) / (u16WLevel - u16BLevel) + 1 ) >> 1;
        preOfs = ( u16BLevel * preGain + 0x200 ) >> 10;
        preGain = min( preGain, 0x0575 );
        preOfs = min( preOfs, 0x00A2);
        // extend YC to full range before DLC/UVCOMP
        MApi_GFLIP_XC_W2BYTE( 0x141B98, preGain );
        MApi_GFLIP_XC_W2BYTE( 0x141B9A, (0x0100 | preOfs) );

        // pre C
        MApi_GFLIP_XC_W2BYTE( 0x141B9C, 0x0491 );
        MApi_GFLIP_XC_W2BYTE( 0x141B9E, 0x0149 );

        // shrink YC back to limited range after DLC UVCOMP
        MApi_GFLIP_XC_W2BYTE( 0x141B38, 0x036D );
        MApi_GFLIP_XC_W2BYTE( 0x141B3A, 0x004B );

        // post C
        MApi_GFLIP_XC_W2BYTE( 0x141B3C, 0x0381 );
        MApi_GFLIP_XC_W2BYTE( 0x141B3E, 0x0049 );
    }
    else
    {
        // bypass
        MApi_GFLIP_XC_W2BYTE( 0x141B98, 0x0400 );
        MApi_GFLIP_XC_W2BYTE( 0x141B9A, 0x0000 );
        MApi_GFLIP_XC_W2BYTE( 0x141B9C, 0x0400 );
        MApi_GFLIP_XC_W2BYTE( 0x141B9E, 0x0000 );

        MApi_GFLIP_XC_W2BYTE( 0x141B38, 0x0400 );
        MApi_GFLIP_XC_W2BYTE( 0x141B3A, 0x0000 );
        MApi_GFLIP_XC_W2BYTE( 0x141B3C, 0x0400 );
        MApi_GFLIP_XC_W2BYTE( 0x141B3E, 0x0000 );
    }
#if 0
    MS_U16 pOutData[16];
    MS_U16 *pInData = &pDlcCtrl->dlcStat.u16DlcCurve;
    MS_U8 ucTmp = 0;
    MS_U16 u16LimitIdx[16] ={91,146,201,256,311,366,421,476,531,586,641,696,751,806,861,916};
    MS_U16 u16FullIdx[16] = {32,96,160,224,288,352,416,480,544,608,672,736,800,864,928,992};
    MS_U16 TempData[16] = {0};
    if(pDlcCtrl->dlcReg.bCurveFullToLimit)
    {
        for (ucTmp = 0; ucTmp < 16; ucTmp++)
        {
            TempData[ucTmp] = ( ((MS_U32)pInData[ucTmp] * 876 ) >> 10 ) + 64;

            if(0 == ucTmp)
            {
                pOutData[ucTmp] = 35;
            }
            else if(15 == ucTmp)
            {
                pOutData[ucTmp] = 1010;
            }
            else
            {
                pOutData[ucTmp] = (((u16FullIdx[ucTmp] -u16LimitIdx[ucTmp-1])*(TempData[ucTmp] - TempData[ucTmp-1]))/(u16LimitIdx[ucTmp] - u16LimitIdx[ucTmp-1])) + TempData[ucTmp-1];
                pOutData[ucTmp] = max(min(pOutData[ucTmp],940),64);
            }
        }

    }

    memcpy( pDlcCtrl->dlcStat.u16DlcCurve, pOutData, sizeof(MS_U16)*16 );
#endif
}

void DlcCurveIIR( DlcCtrl* pDlcCtrl )
{
    MS_U8 uc8Tmp;
    MS_U8 ucIIRRatio = pDlcCtrl->dlcReg.ucIIRRatio;
    MS_U8 uc8SceneChangeW = pDlcCtrl->dlcStat.ucSceneChangeW;
    MS_U16 uc16DlcCurveBfIIR, uc16DlcCurveAfIIR, uc16PreDlcCurve;

    static MS_BOOL bIIRStatus = false;

    // mistake proofing
    if (ucIIRRatio > 0x40)
    {
        ucIIRRatio = 0x40;
    }

    if( bIIRStatus == false )
    {
        for( uc8Tmp = 0 ; uc8Tmp < 16 ; uc8Tmp++ )
        {
            pDlcCtrl->dlcStat.u16PrevDlcCurve[uc8Tmp] = pDlcCtrl->dlcStat.u16DlcCurve[uc8Tmp];
        }
        bIIRStatus = true;
    }
    else
    {
        ucIIRRatio = (ucIIRRatio * (64-uc8SceneChangeW) + 32) >> 6;
        for( uc8Tmp = 0 ; uc8Tmp < 16 ; uc8Tmp++ )
        {
            uc16DlcCurveBfIIR  = pDlcCtrl->dlcStat.u16DlcCurve[uc8Tmp];
            uc16PreDlcCurve    = pDlcCtrl->dlcStat.u16PrevDlcCurve[uc8Tmp];
            // IIR main process
            uc16DlcCurveAfIIR = (uc16DlcCurveBfIIR*(0x40 - ucIIRRatio) + uc16PreDlcCurve*ucIIRRatio + 32) >> 6;

            // To prevent sticking
            if (uc16DlcCurveAfIIR == uc16PreDlcCurve)
            {
                if (uc16DlcCurveBfIIR > uc16DlcCurveAfIIR)
                {
                    pDlcCtrl->dlcStat.u16DlcCurve[uc8Tmp] = uc16DlcCurveAfIIR + 1;
                }
                else if (uc16DlcCurveBfIIR < uc16DlcCurveAfIIR)
                {
                    pDlcCtrl->dlcStat.u16DlcCurve[uc8Tmp] = uc16DlcCurveAfIIR - 1;
                }
                else
                {
                    pDlcCtrl->dlcStat.u16DlcCurve[uc8Tmp] = uc16DlcCurveAfIIR;
                }
            }
            pDlcCtrl->dlcStat.u16PrevDlcCurve[uc8Tmp] = pDlcCtrl->dlcStat.u16DlcCurve[uc8Tmp];
        }
    }
}

void DlcWriteCurve( DlcCtrl* pDlcCtrl )
{
    MS_U8 ucBin;
    MS_U16 data;
    MS_U64 dataLsb = 0, dataMsk;
    MS_U16 *u16DlcCurve = pDlcCtrl->dlcStat.u16DlcCurve;

    for( ucBin = 0 ; ucBin < 16 ; ++ucBin )
    {
        data = u16DlcCurve[ucBin];
        msWriteByte( (REG_ADDR_DLC_DATA_START_MAIN+ucBin), (data >> 2) & 0xFF );
        dataMsk = ( ( (MS_U64)(data&0x3) ) << ( (ucBin+1) << 1 ) );
        dataLsb |= dataMsk;
    }

    // set DLC curve index N0 & 16
    {
        // X = -32
        msWriteByte( REG_ADDR_DLC_DATA_EXTEND_N0_MAIN, u16DlcCurve[0]>>2 );
        msWriteByte( REG_ADDR_DLC_DATA_EXTEND_N0_MAIN + 1, 0x01 );
        dataLsb |= (u16DlcCurve[0] & 0x3);
    }

    {
        // X = 1056
        data = ( ( 1023 - (MS_U32)u16DlcCurve[15] ) << 6 ) / 31 + 992;
        msWriteByte( REG_ADDR_DLC_DATA_EXTEND_16_MAIN, (data >> 2) & 0xFF );
        msWriteByte( REG_ADDR_DLC_DATA_EXTEND_16_MAIN + 1, 0x01 );
        dataLsb |= ( ( (MS_U64)(data&0x3) ) << 34 );
    }

    for( ucBin = 0 ; ucBin < 5 ; ++ucBin )
    {
        msWriteByte( REG_ADDR_DLC_DATA_LSB_START_MAIN, dataLsb&0xFF );
        dataLsb = dataLsb >> 8;
    }
}

void MHalPqDlcHandler(MS_BOOL bWindow)
{
#ifdef TEMP_SOL
    static DlcCtrl dlcCtrl;
    static MS_BOOL bInitialized = false;
    DlcCtrl* pDlcCtrl = &dlcCtrl;

    MS_U8 u8OPFrameIdx;
    getOPIdx();
    u8OPFrameIdx = (sMemIdx.u8OPMIdx + sMemIdx.u8OPDiff) % (sMemIdx.u8MaxIdx); //to get register
    pDlcCtrl->pHistStat = &sHistStat[u8OPFrameIdx];
    if( !bInitialized )
    {
        pDlcCtrl->pPrevHistStat = pDlcCtrl->pHistStat;
        bInitialized = true;
    }
#endif

    // parsing registers
    DlcParseRegister( pDlcCtrl );

    // expand histogram from limit to full
    DlcExpandHistogram( pDlcCtrl );

    // pre processing for bi-histogram
    DlcBiHistogramInfo( pDlcCtrl );

    //Get Bi-Histogram Curve
    DlcGetBiHistogram( pDlcCtrl );

    // limit histogram
    DlcLimitBiHistogram( pDlcCtrl );

    // DLC curve low pass filtering
    DlcCurveLowPass( pDlcCtrl );

    // single bin detection
    DlcSingleBinDetect( pDlcCtrl );

    // blending curve to linear
    DlcLinearBlending( pDlcCtrl );

    // full range to limit range
    DlcCurveRangeConvert( pDlcCtrl );

    // Scene change
    DlcSceneChangeDetect( pDlcCtrl );

    // IIR : Tony
    DlcCurveIIR( pDlcCtrl );

    // Write curve
    DlcWriteCurve( pDlcCtrl );

#ifdef TEMP_SOL
    pDlcCtrl->pPrevHistStat = pDlcCtrl->pHistStat;
#endif
}

MS_BOOL MHalIpHistogramHandler(MS_BOOL bWindow)
{
    static MS_U8 u8preIPFrameIdx = 0; //to get register
    MS_U8 u8IPFrameIdx = sMemIdx.u8IPMIdx; //to get register
    MS_U8 ucTmp;
    MS_U8 ucReturn;
    MS_U32 u32Ystat;

    getIPIdx();
    u8IPFrameIdx = sMemIdx.u8IPMIdx;
/* Set in CFD
    {
        // TODO : set active window
        msWriteByte(REG_ADDR_IP_HISTOGRAM_ENABLE, 0x0F);
        msWriteByte(REG_ADDR_IP_HISTOGRAM_H_END_L, 0x00);
        msWriteByte(REG_ADDR_IP_HISTOGRAM_H_END_H, 0x0F);
        msWriteByte(REG_ADDR_IP_HISTOGRAM_V_END_L, 0x70);
        msWriteByte(REG_ADDR_IP_HISTOGRAM_V_END_H, 0x08);
    }
*/

    if( (u8preIPFrameIdx != u8IPFrameIdx) && (!bWindow))
    {
        for( ucTmp = 0 ; ucTmp < 32 ; ucTmp++ )
        {
            u32Ystat = ( (MS_U32)MApi_GFLIP_XC_R2BYTE( 0x135C20 + ( (ucTmp<<2) + 2 ) ) << 16 )
                | (MS_U32)MApi_GFLIP_XC_R2BYTE( 0x135C20 + (ucTmp<<2) );

            sHistStat[u8IPFrameIdx].u32IpLumaHist[ucTmp] = min( u32Ystat, 0xFFFFFF ); // 24 bits
        }

        sHistStat[u8IPFrameIdx].u32IpLumaTotalCount = ( (MS_U32)MApi_GFLIP_XC_R2BYTE( 0x135C16 ) << 16 )
                | (MS_U32)MApi_GFLIP_XC_R2BYTE( 0x135C14 );

        sHistStat[u8IPFrameIdx].u32IpLumaTotalSum = ( (MS_U32)MApi_GFLIP_XC_R2BYTE( 0x135C1A ) << 16 )
                | (MS_U32)MApi_GFLIP_XC_R2BYTE( 0x135C18 );

        ucReturn = TRUE;
    }
    else
    {
        ucReturn = FALSE;
    }

    u8preIPFrameIdx = u8IPFrameIdx;
    return ucReturn;
}
#endif
