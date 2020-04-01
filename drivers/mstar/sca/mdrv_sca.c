///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
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
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// @file   mdrv_sca_io.c
// @brief  SCA KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Include Files
//=============================================================================
#define MDRV_SCA_C

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>

#include "ms_platform.h"

#include "dev_info.h"

#include "MsTypes.h"
#include "MsDevice.h"
#include "MsCommon.h"
#include "drvXC_IOPort.h"
#include "drvPQ.h"
#include "drvPQ_Define.h"
#include "apiXC.h"
#include "apiXC_PCMonitor.h"
#include "apiXC_ModeParse.h"
#include "apiXC_Adc.h"
#include "apiXC_Auto.h"
#include "apiXC_Ace.h"
#include "apiPNL.h"
#include "drvTVEncoder.h"
#include "apiXC_Dlc.h"
#include "drvMVOP.h"
#include "apiHDMITx.h"

#include "drvMMIO.h"
#include "mdrv_sca_st.h"
#include "mdrv_sca.h"
#include "mdrv_sca_color.h"
#include "mdrv_sca_modeparse.h"
#include "mdrv_sca_inputsource.h"
#include "mdrv_sca_avd.h"
#include "mdrv_sca_pnlcfg.h"
#include "mdrv_sca_hdmitx.h"
//drver header files

//=============================================================================
// Local Defines
//=============================================================================

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------

#define SCA_DNR_BUF         "MMAP_SCA_DNR_BUF"
#define SCA_DNR_SUB_BUF     "MMAP_SCA_DNR_SUB_BUF"
#define SCA_SC1_DNR_BUF     "MMAP_SCA_SC1_DNR_BUF"
#define SCA_SC2_DNR_BUF     "MMAP_SCA_SC2_DNR_BUF"
#define SCA_SC2_DNR_SUB_BUF "MMAP_SCA_SC2_DNR_SUB_BUF"

#define SCA_MLOAD_BUF   "MMAP_SCA_MLOAD_BUF"
#define VE_BUF          "MMAP_VE_BUF"

//------------------------------DRAM type---------------------------------------
#define SDR                     0x10
#define DDR_I                   0x11
#define DDR_II                  0x12
//------------------------------DRAM Bus---------------------------------------
#define DRAM_BUS_16             0
#define DRAM_BUS_32             1
//------------------------------DDRPLL Freq--------------------------------------
#define DDRLLL_FREQ_166         166
#define DDRLLL_FREQ_200         200
#define DDRLLL_FREQ_333         333
#define DDRLLL_FREQ_380         380
#define DDRLLL_FREQ_400         400
#define DDRLLL_FREQ_450         450

#define DRAM_TYPE               DDR_II
#define DRAM_BUS                DRAM_BUS_16
#define DDRPLL_FREQ             DDRLLL_FREQ_400
//=============================================================================
// Local Function Prototypes
//=============================================================================

//------------------------------------------------------------------------------
static const MS_U8  astNoSignalColor[] =
{
    0x00, // E_XC_FREE_RUN_COLOR_BLACK
    0xFF, // E_XC_FREE_RUN_COLOR_WHITE
    0x03, // E_XC_FREE_RUN_COLOR_BLUE
    0xE0, // E_XC_FREE_RUN_COLOR_RED
    0x1C, // E_XC_FREE_RUN_COLOR_GREEN
};

static const MS_U8  astNoSignalColorYUV[] =
{
    0x82, // E_XC_FREE_RUN_COLOR_BLACK
    0x9E, // E_XC_FREE_RUN_COLOR_WHITE
    0x83, // E_XC_FREE_RUN_COLOR_BLUE
    0xC0, // E_XC_FREE_RUN_COLOR_RED
    0x00, // E_XC_FREE_RUN_COLOR_GREEN
};

MS_S16  S16DefaultRGB[3][3] =
{
    { 1096, -43, -28 }, // R  1.0694, -0.0424, -0.0270
    { - 21,   1063, -18 }, // G -0.0204,  1.0376, -0.0172
    { - 1,   34,   991 }  // B -0.0009,  0.0330,  0.9679
};

MS_S16  S16ByPassDefaultRGB[3][3] =
{
    { 1024,    0,    0 }, // R  1.0694, -0.0424, -0.0270
    {    0, 1024,    0 }, // G -0.0204,  1.0376, -0.0172
    {    0,    0, 1024 }  // B -0.0009,  0.0330,  0.9679
};


#if PQ_ENABLE_COLOR
MS_U16 S16SC0_ColorCorrectMatrix[32];
MS_U16 S16SC1_ColorCorrectMatrix[32];
MS_U16 S16SC2_ColorCorrectMatrix[32];

MS_U8 tSC0_NormalGammaR[386];
MS_U8 tSC0_NormalGammaG[386];
MS_U8 tSC0_NormalGammaB[386];
MS_U8 *tSC0_AllGammaTab[3] =
{
    tSC0_NormalGammaR,
    tSC0_NormalGammaG,
    tSC0_NormalGammaB,
};

MS_U8 tSC1_NormalGammaR[386];
MS_U8 tSC1_NormalGammaG[386];
MS_U8 tSC1_NormalGammaB[386];
MS_U8 *tSC1_AllGammaTab[3] =
{
    tSC1_NormalGammaR,
    tSC1_NormalGammaG,
    tSC1_NormalGammaB,
};

#else

MS_S16  S16DefaultColorCorrectionMatrix[32] =
    {
     0x0400, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000,
     0x0400,-0x02E6, 0x0288,-0x05BB, 0x07A4,-0x062C, 0x06F3,-0x073C,
    -0x0024, 0x01BF, 0x07EF,-0x0116, 0x01EE, 0x052C,-0x03BB, 0x00B1,
    -0x0831, 0x0100,-0x0000, 0x0000,-0x0000, 0x0000, 0x0000, 0x0000,

    };

//DAC output bypass color matrix
MS_S16 S16ByPassColorCorrectionMatrix[32] =
{
   0x0400, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000,
   0x0400,-0x02E6, 0x0288,-0x05BB, 0x07A4,-0x062C, 0x06F3,-0x073C,
  -0x0024, 0x01BF, 0x07EF,-0x0116, 0x01EE, 0x052C,-0x03BB, 0x00B1,
  -0x0831, 0x0100,-0x0000, 0x0000,-0x0000, 0x0000, 0x0000, 0x0000,
};


MS_U8 tNormalGammaR[] =
{
    0x00,0x00,0x01,0x00,0x02,0x03,0x00,0x04,0x05,
    0x00,0x06,0x07,0x00,0x08,0x09,0x00,0x0A,0x0B,
    0x00,0x0C,0x0D,0x00,0x0E,0x0F,0x00,0x10,0x11,
    0x00,0x12,0x13,0x00,0x14,0x15,0x00,0x16,0x17,
    0x00,0x18,0x19,0x00,0x1A,0x1B,0x00,0x1C,0x1D,
    0x00,0x1E,0x1F,0x00,0x20,0x21,0x00,0x22,0x23,
    0x00,0x24,0x25,0x00,0x26,0x27,0x00,0x28,0x29,
    0x00,0x2A,0x2B,0x00,0x2C,0x2D,0x00,0x2E,0x2F,
    0x00,0x30,0x31,0x00,0x32,0x33,0x00,0x34,0x35,
    0x00,0x36,0x37,0x00,0x38,0x39,0x00,0x3A,0x3B,
    0x00,0x3C,0x3D,0x00,0x3E,0x3F,0x00,0x40,0x41,
    0x00,0x42,0x43,0x00,0x44,0x45,0x00,0x46,0x47,
    0x00,0x48,0x49,0x00,0x4A,0x4B,0x00,0x4C,0x4D,
    0x00,0x4E,0x4F,0x00,0x50,0x51,0x00,0x52,0x53,
    0x00,0x54,0x55,0x00,0x56,0x57,0x00,0x58,0x59,
    0x00,0x5A,0x5B,0x00,0x5C,0x5D,0x00,0x5E,0x5F,
    0x00,0x60,0x61,0x00,0x62,0x63,0x00,0x64,0x65,
    0x00,0x66,0x67,0x00,0x68,0x69,0x00,0x6A,0x6B,
    0x00,0x6C,0x6D,0x00,0x6E,0x6F,0x00,0x70,0x71,
    0x00,0x72,0x73,0x00,0x74,0x75,0x00,0x76,0x77,
    0x00,0x78,0x79,0x00,0x7A,0x7B,0x00,0x7C,0x7D,
    0x00,0x7E,0x7F,0x00,0x80,0x81,0x00,0x82,0x83,
    0x00,0x84,0x85,0x00,0x86,0x87,0x00,0x88,0x89,
    0x00,0x8A,0x8B,0x00,0x8C,0x8D,0x00,0x8E,0x8F,
    0x00,0x90,0x91,0x00,0x92,0x93,0x00,0x94,0x95,
    0x00,0x96,0x97,0x00,0x98,0x99,0x00,0x9A,0x9B,
    0x00,0x9C,0x9D,0x00,0x9E,0x9F,0x00,0xA0,0xA1,
    0x00,0xA2,0xA3,0x00,0xA4,0xA5,0x00,0xA6,0xA7,
    0x00,0xA8,0xA9,0x00,0xAA,0xAB,0x00,0xAC,0xAD,
    0x00,0xAE,0xAF,0x00,0xB0,0xB1,0x00,0xB2,0xB3,
    0x00,0xB4,0xB5,0x00,0xB6,0xB7,0x00,0xB8,0xB9,
    0x00,0xBA,0xBB,0x00,0xBC,0xBD,0x00,0xBE,0xBF,
    0x00,0xC0,0xC1,0x00,0xC2,0xC3,0x00,0xC4,0xC5,
    0x00,0xC6,0xC7,0x00,0xC8,0xC9,0x00,0xCA,0xCB,
    0x00,0xCC,0xCD,0x00,0xCE,0xCF,0x00,0xD0,0xD1,
    0x00,0xD2,0xD3,0x00,0xD4,0xD5,0x00,0xD6,0xD7,
    0x00,0xD8,0xD9,0x00,0xDA,0xDB,0x00,0xDC,0xDD,
    0x00,0xDE,0xDF,0x00,0xE0,0xE1,0x00,0xE2,0xE3,
    0x00,0xE4,0xE5,0x00,0xE6,0xE7,0x00,0xE8,0xE9,
    0x00,0xEA,0xEB,0x00,0xEC,0xED,0x00,0xEE,0xEF,
    0x00,0xF0,0xF1,0x00,0xF2,0xF3,0x00,0xF4,0xF5,
    0x00,0xF6,0xF7,0x00,0xF8,0xF9,0x00,0xFA,0xFB,
    0x00,0xFC,0xFD,0x00,0xFE,0xFF,0x00,0xFF
};

MS_U8 tNormalGammaG[] =
{
    0x00,0x00,0x01,0x00,0x02,0x03,0x00,0x04,0x05,
    0x00,0x06,0x07,0x00,0x08,0x09,0x00,0x0A,0x0B,
    0x00,0x0C,0x0D,0x00,0x0E,0x0F,0x00,0x10,0x11,
    0x00,0x12,0x13,0x00,0x14,0x15,0x00,0x16,0x17,
    0x00,0x18,0x19,0x00,0x1A,0x1B,0x00,0x1C,0x1D,
    0x00,0x1E,0x1F,0x00,0x20,0x21,0x00,0x22,0x23,
    0x00,0x24,0x25,0x00,0x26,0x27,0x00,0x28,0x29,
    0x00,0x2A,0x2B,0x00,0x2C,0x2D,0x00,0x2E,0x2F,
    0x00,0x30,0x31,0x00,0x32,0x33,0x00,0x34,0x35,
    0x00,0x36,0x37,0x00,0x38,0x39,0x00,0x3A,0x3B,
    0x00,0x3C,0x3D,0x00,0x3E,0x3F,0x00,0x40,0x41,
    0x00,0x42,0x43,0x00,0x44,0x45,0x00,0x46,0x47,
    0x00,0x48,0x49,0x00,0x4A,0x4B,0x00,0x4C,0x4D,
    0x00,0x4E,0x4F,0x00,0x50,0x51,0x00,0x52,0x53,
    0x00,0x54,0x55,0x00,0x56,0x57,0x00,0x58,0x59,
    0x00,0x5A,0x5B,0x00,0x5C,0x5D,0x00,0x5E,0x5F,
    0x00,0x60,0x61,0x00,0x62,0x63,0x00,0x64,0x65,
    0x00,0x66,0x67,0x00,0x68,0x69,0x00,0x6A,0x6B,
    0x00,0x6C,0x6D,0x00,0x6E,0x6F,0x00,0x70,0x71,
    0x00,0x72,0x73,0x00,0x74,0x75,0x00,0x76,0x77,
    0x00,0x78,0x79,0x00,0x7A,0x7B,0x00,0x7C,0x7D,
    0x00,0x7E,0x7F,0x00,0x80,0x81,0x00,0x82,0x83,
    0x00,0x84,0x85,0x00,0x86,0x87,0x00,0x88,0x89,
    0x00,0x8A,0x8B,0x00,0x8C,0x8D,0x00,0x8E,0x8F,
    0x00,0x90,0x91,0x00,0x92,0x93,0x00,0x94,0x95,
    0x00,0x96,0x97,0x00,0x98,0x99,0x00,0x9A,0x9B,
    0x00,0x9C,0x9D,0x00,0x9E,0x9F,0x00,0xA0,0xA1,
    0x00,0xA2,0xA3,0x00,0xA4,0xA5,0x00,0xA6,0xA7,
    0x00,0xA8,0xA9,0x00,0xAA,0xAB,0x00,0xAC,0xAD,
    0x00,0xAE,0xAF,0x00,0xB0,0xB1,0x00,0xB2,0xB3,
    0x00,0xB4,0xB5,0x00,0xB6,0xB7,0x00,0xB8,0xB9,
    0x00,0xBA,0xBB,0x00,0xBC,0xBD,0x00,0xBE,0xBF,
    0x00,0xC0,0xC1,0x00,0xC2,0xC3,0x00,0xC4,0xC5,
    0x00,0xC6,0xC7,0x00,0xC8,0xC9,0x00,0xCA,0xCB,
    0x00,0xCC,0xCD,0x00,0xCE,0xCF,0x00,0xD0,0xD1,
    0x00,0xD2,0xD3,0x00,0xD4,0xD5,0x00,0xD6,0xD7,
    0x00,0xD8,0xD9,0x00,0xDA,0xDB,0x00,0xDC,0xDD,
    0x00,0xDE,0xDF,0x00,0xE0,0xE1,0x00,0xE2,0xE3,
    0x00,0xE4,0xE5,0x00,0xE6,0xE7,0x00,0xE8,0xE9,
    0x00,0xEA,0xEB,0x00,0xEC,0xED,0x00,0xEE,0xEF,
    0x00,0xF0,0xF1,0x00,0xF2,0xF3,0x00,0xF4,0xF5,
    0x00,0xF6,0xF7,0x00,0xF8,0xF9,0x00,0xFA,0xFB,
    0x00,0xFC,0xFD,0x00,0xFE,0xFF,0x00,0xFF
};


MS_U8 tNormalGammaB[] =
{
    0x00,0x00,0x01,0x00,0x02,0x03,0x00,0x04,0x05,
    0x00,0x06,0x07,0x00,0x08,0x09,0x00,0x0A,0x0B,
    0x00,0x0C,0x0D,0x00,0x0E,0x0F,0x00,0x10,0x11,
    0x00,0x12,0x13,0x00,0x14,0x15,0x00,0x16,0x17,
    0x00,0x18,0x19,0x00,0x1A,0x1B,0x00,0x1C,0x1D,
    0x00,0x1E,0x1F,0x00,0x20,0x21,0x00,0x22,0x23,
    0x00,0x24,0x25,0x00,0x26,0x27,0x00,0x28,0x29,
    0x00,0x2A,0x2B,0x00,0x2C,0x2D,0x00,0x2E,0x2F,
    0x00,0x30,0x31,0x00,0x32,0x33,0x00,0x34,0x35,
    0x00,0x36,0x37,0x00,0x38,0x39,0x00,0x3A,0x3B,
    0x00,0x3C,0x3D,0x00,0x3E,0x3F,0x00,0x40,0x41,
    0x00,0x42,0x43,0x00,0x44,0x45,0x00,0x46,0x47,
    0x00,0x48,0x49,0x00,0x4A,0x4B,0x00,0x4C,0x4D,
    0x00,0x4E,0x4F,0x00,0x50,0x51,0x00,0x52,0x53,
    0x00,0x54,0x55,0x00,0x56,0x57,0x00,0x58,0x59,
    0x00,0x5A,0x5B,0x00,0x5C,0x5D,0x00,0x5E,0x5F,
    0x00,0x60,0x61,0x00,0x62,0x63,0x00,0x64,0x65,
    0x00,0x66,0x67,0x00,0x68,0x69,0x00,0x6A,0x6B,
    0x00,0x6C,0x6D,0x00,0x6E,0x6F,0x00,0x70,0x71,
    0x00,0x72,0x73,0x00,0x74,0x75,0x00,0x76,0x77,
    0x00,0x78,0x79,0x00,0x7A,0x7B,0x00,0x7C,0x7D,
    0x00,0x7E,0x7F,0x00,0x80,0x81,0x00,0x82,0x83,
    0x00,0x84,0x85,0x00,0x86,0x87,0x00,0x88,0x89,
    0x00,0x8A,0x8B,0x00,0x8C,0x8D,0x00,0x8E,0x8F,
    0x00,0x90,0x91,0x00,0x92,0x93,0x00,0x94,0x95,
    0x00,0x96,0x97,0x00,0x98,0x99,0x00,0x9A,0x9B,
    0x00,0x9C,0x9D,0x00,0x9E,0x9F,0x00,0xA0,0xA1,
    0x00,0xA2,0xA3,0x00,0xA4,0xA5,0x00,0xA6,0xA7,
    0x00,0xA8,0xA9,0x00,0xAA,0xAB,0x00,0xAC,0xAD,
    0x00,0xAE,0xAF,0x00,0xB0,0xB1,0x00,0xB2,0xB3,
    0x00,0xB4,0xB5,0x00,0xB6,0xB7,0x00,0xB8,0xB9,
    0x00,0xBA,0xBB,0x00,0xBC,0xBD,0x00,0xBE,0xBF,
    0x00,0xC0,0xC1,0x00,0xC2,0xC3,0x00,0xC4,0xC5,
    0x00,0xC6,0xC7,0x00,0xC8,0xC9,0x00,0xCA,0xCB,
    0x00,0xCC,0xCD,0x00,0xCE,0xCF,0x00,0xD0,0xD1,
    0x00,0xD2,0xD3,0x00,0xD4,0xD5,0x00,0xD6,0xD7,
    0x00,0xD8,0xD9,0x00,0xDA,0xDB,0x00,0xDC,0xDD,
    0x00,0xDE,0xDF,0x00,0xE0,0xE1,0x00,0xE2,0xE3,
    0x00,0xE4,0xE5,0x00,0xE6,0xE7,0x00,0xE8,0xE9,
    0x00,0xEA,0xEB,0x00,0xEC,0xED,0x00,0xEE,0xEF,
    0x00,0xF0,0xF1,0x00,0xF2,0xF3,0x00,0xF4,0xF5,
    0x00,0xF6,0xF7,0x00,0xF8,0xF9,0x00,0xFA,0xFB,
    0x00,0xFC,0xFD,0x00,0xFE,0xFF,0x00,0xFF
};

MS_U8 *tAllGammaTab[3] =
{
    // COLOR_TEMP_NORMAL/COLOR_TEMP_USER
    tNormalGammaR,
    tNormalGammaG,
    tNormalGammaB,
};
#endif

static MS_BOOL g_bXCSysInitReady = FALSE;
extern DEVINFO_PANEL_TYPE ms_devinfo_panel_type(void);
extern DEVINFO_BOARD_TYPE ms_devinfo_board_type(void);
extern DEVINFO_RTK_FLAG ms_devinfo_rtk_flag(void);

extern SCA_PQ_BIN_DATA stPQBinData[SCA_PQ_BIN_ID_NUM];

MS_SYS_INFO stSystemInfo[MAX_WINDOW];
static E_SCREEN_MUTE_STATUS s_eScreenMuteStatus[MAX_WINDOW] = {E_SCREEN_MUTE_INIT,E_SCREEN_MUTE_INIT};
static PanelType* pstPaneltype = NULL;
MS_BOOL bInitPNL = FALSE;
MS_DLC_MONITOR_CONFIG g_stDLCMonitorCfg = {0};

//------------------------------------------------------------------------------
//  Internal function
//------------------------------------------------------------------------------

static EN_SYS_INPUT_SOURCE_TYPE  _enSysInputSource = EN_SYS_INPUT_SOURCE_TYPE_NONE;


void msAPI_InputSrcType_SetType(EN_SYS_INPUT_SOURCE_TYPE enInpSrcType)
{
    _enSysInputSource |= enInpSrcType;
}

EN_SYS_INPUT_SOURCE_TYPE msAPI_InputSrcType_GetType(void)
{
    return _enSysInputSource;
}

void msAPI_InputSrcType_ClrType(EN_SYS_INPUT_SOURCE_TYPE enClrType)
{
    _enSysInputSource &= (EN_SYS_INPUT_SOURCE_TYPE)~enClrType;
}

MS_BOOL msAPI_InputSrcType_InUsedType(EN_SYS_INPUT_SOURCE_TYPE enInpSrcType)
{
    if ((_enSysInputSource & enInpSrcType) == 0)
        return FALSE;
    else
        return TRUE;
}


static void SaveProjPaneltype(const PanelType* a_pstSrcPaneltype)
{
	pstPaneltype = (PanelType*)a_pstSrcPaneltype;
    SCA_DBG(SCA_DBG_LV_INIT_XC, "[%s:%04d] W= %d, H = %d, Hst = %d, Vst = %d, Ltype = %d !!! \n",
            __FUNCTION__, __LINE__,
            a_pstSrcPaneltype->m_wPanelWidth,
    		a_pstSrcPaneltype->m_wPanelHeight,
            a_pstSrcPaneltype->m_wPanelHStart,
            a_pstSrcPaneltype->m_wPanelVStart,
            a_pstSrcPaneltype->m_ePanelLinkType);
}


static E_XC_PNL_LPLL_TYPE GetPnlLplltype(APIPNL_LINK_TYPE eSrcLinktype)
{
	E_XC_PNL_LPLL_TYPE ePnlLplltype;

	switch (eSrcLinktype)
	{
		case LINK_EXT:
			ePnlLplltype = E_XC_PNL_LPLL_EXT;
			break;

		case LINK_LVDS:
			ePnlLplltype = E_XC_PNL_LPLL_LVDS;
			break;

		case LINK_TTL:
		default:
			ePnlLplltype = E_XC_PNL_LPLL_TTL;
			break;
	}

	return ePnlLplltype;
}

PanelType* MApi_GetProjPaneltype(void)
{
	return pstPaneltype;
}


void MApi_XC_Sys_Init_Pnl(void)
{
    MSIF_Version pnlVersion;
    MS_U8        tag[4] = MSIF_TAG;
    MS_U8        type[2] = MSIF_CLASS;
    MS_U8        name[4] = MSIF_PNL_LIB_CODE;
    MS_U8        version[2] = MSIF_PNL_LIBVER;
    MS_U8        build[2]  = MSIF_PNL_BUILDNUM;
    MS_U8       change[8] = MSIF_PNL_CHANGELIST;
	PanelType   *pstProjPanelType = NULL;

    MS_U32 u32ModuleBaseAddr =0, u32ModuleBaseSize =0;
    MDrv_MMIO_GetBASE( &u32ModuleBaseAddr, &u32ModuleBaseSize, MS_MODULE_PM);


    memcpy(pnlVersion.MS_DDI.tag, tag, 4);
    memcpy(pnlVersion.MS_DDI.type, type, 2);
    pnlVersion.MS_DDI.customer = MSIF_CUS;
    pnlVersion.MS_DDI.model = MSIF_MOD;
    pnlVersion.MS_DDI.chip = MSIF_CHIP;
    pnlVersion.MS_DDI.cpu = MSIF_CPU;
    memcpy(pnlVersion.MS_DDI.name, name, 4);
    memcpy(pnlVersion.MS_DDI.version, version, 2);
    memcpy(pnlVersion.MS_DDI.build, build, 2);
    memcpy(pnlVersion.MS_DDI.change, change, 8);
    pnlVersion.MS_DDI.os = MSIF_OS;
    if(DEVINFO_PANEL_HSD_070P_FW3==ms_devinfo_panel_type())
    {
        pstProjPanelType = tPanelHSD_070P_FW3_MST154A;
    }
    else if(DEVINFO_PANEL_HSD_062I_DW1 == ms_devinfo_panel_type())
    {
        pstProjPanelType = tPanelHSD_062I_DW1_MST786_SZDEMO;
    }
    else if(DEVINFO_PANEL_HSD_070I_DW1 == ms_devinfo_panel_type())
    {
        pstProjPanelType = tPanelHSD_070I_DW1_MST786_SZDEMO;
    }
    else if(DEVINFO_PANEL_HSD_070I_DW2 == ms_devinfo_panel_type())
    {
        pstProjPanelType = tPanelHSD_070I_DW2_MST786_SZDEMO;
    }
	else if(DEVINFO_PANEL_HSD_LVDS_800480 == ms_devinfo_panel_type())
	{
        pstProjPanelType = tPanelHSD_070P_LVDS_800480;
	}
	else if(DEVINFO_PANEL_N070ICG_LD1 == ms_devinfo_panel_type())
	{
		pstProjPanelType = tPanelN070ICG_LD1_MST154A;
	}
	else if(DEVINFO_PANEL_HSD_062I_DW2 == ms_devinfo_panel_type())
	{
		pstProjPanelType = tPanelHSD_062I_DW2__MST786_SZDEMO;
	}
	else if(DEVINFO_PANEL_HSD_070I_MODE2 == ms_devinfo_panel_type())
	{
		pstProjPanelType = tPanelHSD_070I_DW1__MST786_MODE2;
	}
	else
    {
        pstProjPanelType = tPanelHSD_070I_DW1_MST154A_D02A;
    }

    SaveProjPaneltype(pstProjPanelType);

    if(pstProjPanelType->m_ePanelLinkType == LINK_TTL)
    {
        MApi_BD_LVDS_Output_Type(4);
    }
    else
    {
        MApi_BD_LVDS_Output_Type(1);
    }

    MApi_PNL_Init_Ex(pstProjPanelType, pnlVersion);


    MApi_Mod_Calibration_Setting(0x00, 0x00);
    g_IPanel.Enable(TRUE);

    #define REG_GPIO1_16_L ((0x101A00+(0x16*2))*2)
    #define REG_GPIO0_55_L ((0x102B00+(0x55*2))*2)
    #define REG_GPIO0_56_L ((0x102B00+(0x56*2))*2)

#ifdef __BOOT_PNL__
    #define REG_GPIO0_04_L ((0x102B00+(0x04*2))*2)
    OUTREG16((u32ModuleBaseAddr+REG_GPIO0_04_L), 0x500);
#endif

    if(DEVINFO_BD_MST786_SZDEMO == ms_devinfo_board_type())
    {
	    OUTREG16((u32ModuleBaseAddr+REG_GPIO1_16_L), 0x002C);//[5]ttl_bg_mode [3]tck_mode [2]ttlhv_mode
	    OUTREG16((u32ModuleBaseAddr+REG_GPIO0_55_L), 0x0005);//[2:0]Tcon_gpio_4. REset, [10:8]tcon_gpio_5 L/R control
	    OUTREG16((u32ModuleBaseAddr+REG_GPIO0_56_L), 0x0505);
    }
	else if(DEVINFO_BD_MST786_CUSTOM30== ms_devinfo_board_type())
    {
	    OUTREG16((u32ModuleBaseAddr+REG_GPIO1_16_L), 0x002C);//[5]ttl_bg_mode [3]tck_mode [2]ttlhv_mode
	    OUTREG16((u32ModuleBaseAddr+REG_GPIO0_55_L), 0x0005);//[2:0]Tcon_gpio_4. REset, [10:8]tcon_gpio_5 L/R control
	    OUTREG16((u32ModuleBaseAddr+REG_GPIO0_56_L), 0x0505);
    }
	else
    {
		OUTREG16((u32ModuleBaseAddr+REG_GPIO1_16_L), 0x002C);//[5]ttl_bg_mode [3]tck_mode [2]ttlhv_mode
		OUTREG16((u32ModuleBaseAddr+REG_GPIO0_55_L), 0x0101);//[2:0]Tcon_gpio_4. REset, [10:8]tcon_gpio_5 L/R control
    }


	#define REG_MOD_BK00_75_L ((0x103200+(0x75*2))*2)

	if ( DEVINFO_PANEL_N070ICG_LD1 == ms_devinfo_panel_type() )
	{
		// enable internal resistor
		OUTREG16((u32ModuleBaseAddr + REG_MOD_BK00_75_L), 0x000F);
	}



    // setting switch GOP Mux
    {
    #define REG_SC_BK00_06_L ((0x130000+(0x06*2))*2)
    #define REG_SC_BK10_23_L ((0x131000+(0x23*2))*2)
    #define REG_SC_BK10_19_L ((0x131000+(0x19*2))*2)

    #define REG_SC01_BK00_06_L ((0x138000+(0x06*2))*2)
    #define REG_SC01_BK10_23_L ((0x139000+(0x23*2))*2)
    #define REG_SC01_BK10_19_L ((0x139000+(0x19*2))*2)


    OUTREG16((u32ModuleBaseAddr + REG_SC_BK00_06_L), 0xF008);
    OUTREG16((u32ModuleBaseAddr + REG_SC_BK10_23_L), 0x0080);
    OUTREG16((u32ModuleBaseAddr + REG_SC_BK10_19_L), 0x0000);

#if 1//defined(CONFIG_MS_CLONE_SCREEN)
    OUTREG16((u32ModuleBaseAddr + REG_SC01_BK00_06_L), 0xB008);
    OUTREG16((u32ModuleBaseAddr + REG_SC01_BK10_23_L), 0x0080);
    OUTREG16((u32ModuleBaseAddr + REG_SC01_BK10_19_L), 0x0000);
#endif

    }
}

void MApi_XC_Get_ColorCorrectMatix_From_PQ(SCALER_WIN eWindow, MS_S16 *pBuf)
{
    MS_S16 s16Matrix[32];
    PQ_WIN PQWin = eWindow == MAIN_WINDOW     ? PQ_MAIN_WINDOW :
                   eWindow == SUB_WINDOW      ? PQ_MAIN_WINDOW :
                   eWindow == SC2_MAIN_WINDOW ? PQ_SC2_MAIN_WINDOW :
                   eWindow == SC2_SUB_WINDOW  ? PQ_SC2_MAIN_WINDOW :
                                                PQ_SC1_MAIN_WINDOW;

    if(MDrv_PQ_Get_Picture(PQWin, PQ_INPUT_SOURCE_NONE, NULL, E_PQ_PICTURE_3x3_MATRIX, (void *)s16Matrix))
    {
        memcpy(pBuf, s16Matrix, sizeof(MS_S16)*32);
    }
}


static void _MApi_XC_Sys_Init_ACE(void)
{
    XC_ACE_InitData sXC_ACE_InitData;

    memset(&sXC_ACE_InitData, 0, sizeof(XC_ACE_InitData));

    // Init ACE
    sXC_ACE_InitData.eWindow = MAIN_WINDOW;

#if PQ_ENABLE_COLOR
    MApi_XC_Get_ColorCorrectMatix_From_PQ(MAIN_WINDOW,  (MS_S16 *)S16SC0_ColorCorrectMatrix);
    sXC_ACE_InitData.S16ColorCorrectionMatrix = (MS_S16*) S16SC0_ColorCorrectMatrix;
#else
    sXC_ACE_InitData.S16ColorCorrectionMatrix = (MS_S16*) S16DefaultColorCorrectionMatrix;
#endif
    sXC_ACE_InitData.S16RGB = (MS_S16*) S16DefaultRGB;
    sXC_ACE_InitData.u16MWEHstart = (MApi_GetProjPaneltype()->m_wPanelHStart);
    sXC_ACE_InitData.u16MWEVstart = PNL_VSTART ;//(MApi_GetProjPaneltype()->m_wPanelVStart);
    sXC_ACE_InitData.u16MWEWidth  = (MApi_GetProjPaneltype()->m_wPanelWidth);
    sXC_ACE_InitData.u16MWEHeight = (MApi_GetProjPaneltype()->m_wPanelHeight);

    //Init H/V info for first MWE function select
    sXC_ACE_InitData.u16MWE_Disp_Hstart = 0;
    sXC_ACE_InitData.u16MWE_Disp_Vstart = 0;
    sXC_ACE_InitData.u16MWE_Disp_Width  = (MApi_GetProjPaneltype()->m_wPanelWidth);
    sXC_ACE_InitData.u16MWE_Disp_Height = (MApi_GetProjPaneltype()->m_wPanelHeight);

    sXC_ACE_InitData.bMWE_Enable = TRUE;

    if(MApi_XC_ACE_Init(&sXC_ACE_InitData, sizeof(XC_ACE_InitData)) == FALSE)
    {
        SCA_DBG(SCA_DBG_LV_0, " ACE_Init failed because of InitData wrong, please update header file and compile again\n");
    }

    MApi_XC_Set_OutputColor(MAIN_WINDOW, E_XC_OUTPUT_COLOR_RGB);
    MApi_XC_Set_OutputColor(SUB_WINDOW, E_XC_OUTPUT_COLOR_RGB);


    MApi_XC_Set_OutputColor(SC1_MAIN_WINDOW, E_XC_OUTPUT_COLOR_YUV);
    MApi_XC_ACE_SetPCYUV2RGB(SC1_MAIN_WINDOW, FALSE);

#if PQ_ENABLE_COLOR
    MApi_XC_Get_ColorCorrectMatix_From_PQ(SC1_MAIN_WINDOW,  (MS_S16 *)S16SC1_ColorCorrectMatrix);
    MApi_XC_ACE_ColorCorrectionTable(SC1_MAIN_WINDOW, (MS_S16 *)S16SC1_ColorCorrectMatrix);
#else
    MApi_XC_ACE_ColorCorrectionTable(SC1_MAIN_WINDOW, (MS_S16 *)S16ByPassColorCorrectionMatrix);
#endif

    MApi_XC_ACE_PCsRGBTable(SC1_MAIN_WINDOW, (MS_S16 *)S16ByPassDefaultRGB);
    MApi_XC_ACE_SetColorCorrectionTable(SC1_MAIN_WINDOW);
    MApi_XC_ACE_SetPCsRGBTable(SC1_MAIN_WINDOW);

    // SC2
    MApi_XC_Set_OutputColor(SC2_MAIN_WINDOW, E_XC_OUTPUT_COLOR_YUV);
    MApi_XC_ACE_SetPCYUV2RGB(SC2_MAIN_WINDOW, FALSE);
#if PQ_ENABLE_COLOR
    MApi_XC_Get_ColorCorrectMatix_From_PQ(SC2_MAIN_WINDOW,  (MS_S16 *)S16SC2_ColorCorrectMatrix);
    MApi_XC_ACE_ColorCorrectionTable(SC2_MAIN_WINDOW, (MS_S16 *)S16SC2_ColorCorrectMatrix);
#else
    MApi_XC_ACE_ColorCorrectionTable(SC2_MAIN_WINDOW, (MS_S16 *)S16ByPassColorCorrectionMatrix);
#endif
    MApi_XC_ACE_PCsRGBTable(SC2_MAIN_WINDOW, (MS_S16 *)S16ByPassDefaultRGB);
    MApi_XC_ACE_SetColorCorrectionTable(SC2_MAIN_WINDOW);
    MApi_XC_ACE_SetPCsRGBTable(SC2_MAIN_WINDOW);

}

static MS_U16 _MApi_XC_Sys_Calc_Freq(MS_U16 u16Htt, MS_U16 u16Vtt, MS_U16 u16DCLK)
{
    MS_U16 u16Freq;
    MS_U32 u32Freq;

    u32Freq = (MS_U32)u16DCLK * (MS_U32)10000000 / ((MS_U32)u16Htt * (MS_U32)u16Vtt);

    if(u32Freq > 1100)
    {
        u16Freq = 1200;
    }
    else if(u32Freq > 900)
    {
        u16Freq = 1000;
    }
    else if(u32Freq > 550)
    {
        u16Freq = 600;
    }
    else if(u32Freq > 350)
    {
        u16Freq = 500;
    }
    else
    {
        u16Freq = 600;
    }

    return u16Freq;

}

static void _MApi_XC_Sys_Init_XC(XC_INITDATA *pstXC_InitData, XC_INIT_INFO stInitInfo)
{
    XC_INITMISC sXC_Init_Misc;

    // reset to zero
    memset(&sXC_Init_Misc, 0, sizeof(XC_INITMISC));

    // Init XC
    // Check library version. Do not modify this statement please.
    pstXC_InitData->u32XC_version = XC_INITDATA_VERSION;

    pstXC_InitData->u32XTAL_Clock = MST_XTAL_CLOCK_HZ;

    if(stInitInfo.u32SC0_Main_Phy == 0 || stInitInfo.u32SC0_Main_Size == 0)
    {
        SCA_DBG(SCA_DBG_LV_INIT_XC, "SC0 Main Phy is 0 \n");
        return;
    }

    pstXC_InitData->u32Main_FB_Size = stInitInfo.u32SC0_Main_Phy;
    pstXC_InitData->u32Main_FB_Start_Addr = stInitInfo.u32SC0_Main_Size;

    // Init DNR Address in Main & Sub channel. Keep the same. If project support FB PIP mode, set Sub DNR Address in AP layer (eg. mapp_init).
    pstXC_InitData->u32Sub_FB_Size = pstXC_InitData->u32Main_FB_Size;
    pstXC_InitData->u32Sub_FB_Start_Addr = pstXC_InitData->u32Main_FB_Start_Addr;

    // Chip related.
    pstXC_InitData->bIsShareGround = SHARE_GND;

    // Board related
    pstXC_InitData->eScartIDPort_Sel = SCART_ID_SEL | SCART2_ID_SEL ;

    pstXC_InitData->bCEC_Use_Interrupt = FALSE;

    pstXC_InitData->bEnableIPAutoCoast = ENABLE_IP_AUTO_COAST;

    memset(&pstXC_InitData->bMirror, 0, sizeof(MS_BOOL)*MAX_WINDOW);

    // panel info
    pstXC_InitData->stPanelInfo.u16HStart = (MApi_GetProjPaneltype()->m_wPanelHStart);
    pstXC_InitData->stPanelInfo.u16VStart = PNL_VSTART ;//(MApi_GetProjPaneltype()->m_wPanelVStart);
    pstXC_InitData->stPanelInfo.u16Width  = (MApi_GetProjPaneltype()->m_wPanelWidth);
    pstXC_InitData->stPanelInfo.u16Height = (MApi_GetProjPaneltype()->m_wPanelHeight);
    pstXC_InitData->stPanelInfo.u16HTotal = (MApi_GetProjPaneltype()->m_wPanelHTotal);
    pstXC_InitData->stPanelInfo.u16VTotal = (MApi_GetProjPaneltype()->m_wPanelVTotal);

    pstXC_InitData->stPanelInfo.u16DefaultVFreq =  _MApi_XC_Sys_Calc_Freq( (MApi_GetProjPaneltype()->m_wPanelHTotal),
                                                                           (MApi_GetProjPaneltype()->m_wPanelVTotal),
                                                                           (MApi_GetProjPaneltype()->m_dwPanelDCLK));

    pstXC_InitData->stPanelInfo.u8LPLL_Mode = (MApi_GetProjPaneltype()->m_bPanelDoubleClk) ? 1:0; // 0: single clock mode, 1: double clock mode
    pstXC_InitData->stPanelInfo.enPnl_Out_Timing_Mode = (MApi_GetProjPaneltype()->m_ucOutTimingMode);

    pstXC_InitData->stPanelInfo.u16DefaultHTotal = (MApi_GetProjPaneltype()->m_wPanelHTotal);
    pstXC_InitData->stPanelInfo.u16DefaultVTotal = (MApi_GetProjPaneltype()->m_wPanelVTotal);
    pstXC_InitData->stPanelInfo.u32MinSET = (MApi_GetProjPaneltype()->m_dwPanelMinSET);
    pstXC_InitData->stPanelInfo.u32MaxSET = (MApi_GetProjPaneltype()->m_dwPanelMaxSET);
    pstXC_InitData->stPanelInfo.eLPLL_Type = GetPnlLplltype( (MApi_GetProjPaneltype()->m_ePanelLinkType) );;
    pstXC_InitData->bDLC_Histogram_From_VBlank = FALSE;

	//!!! since XC may be init at RTK, to avoid double init. !!!//
	pstXC_InitData->pCbfPreTestInputSourceLocked = NULL; ///InputSourceLockedTestCbf;


    if (  MApi_XC_GetCapability(E_XC_SUPPORT_IMMESWITCH)  )
    {
        sXC_Init_Misc.u32MISC_A |= E_XC_INIT_MISC_A_IMMESWITCH;
    }

    if ( MApi_XC_GetCapability(E_XC_SUPPORT_FRC_INSIDE) && (ENABLE_FRC) )
    {
        sXC_Init_Misc.u32MISC_A |= E_XC_INIT_MISC_A_FRC_INSIDE;
    }


    if(MApi_XC_Init(pstXC_InitData, sizeof(XC_INITDATA)) == FALSE)
    {
        SCA_DBG(SCA_DBG_LV_0, "XC_Init failed because of InitData wrong, please update header file and compile again \n");
    }

    MApi_XC_SetHdmiSyncMode(HDMI_SYNC_HV);

    if(MApi_XC_Init_MISC(&sXC_Init_Misc, sizeof(XC_INITMISC)) == FALSE)
    {
        SCA_DBG(SCA_DBG_LV_0, "XC Init MISC failed because of InitData wrong, please update header file and compile again \n");
    }

    if(stInitInfo.u32MLoad_Phy == 0 || stInitInfo.u32MLoad_Size == 0)
    {
        SCA_DBG(SCA_DBG_LV_0, "[SCA] MLoad is 0 \n");
        return;
    }

    // Init MLoad
#if defined(MSOS_TYPE_LINUX_KERNEL)
    MApi_XC_MLoad_Init(stInitInfo.u32MLoad_Phy, stInitInfo.u32MLoad_Vir, stInitInfo.u32MLoad_Size);
#else
    MApi_XC_MLoad_Init(stInitInfo.u32MLoad_Phy, stInitInfo.u32MLoad_Size);
#endif
    MApi_XC_MLoad_Enable(TRUE);

    //
    MApi_XC_SetFrameBufferAddress(stInitInfo.u32SC0_Main_Phy, stInitInfo.u32SC0_Main_Size, MAIN_WINDOW);

    if(stInitInfo.u32SC0_Sub_Phy && stInitInfo.u32SC0_Sub_Size)
    {
        MApi_XC_SetFrameBufferAddress(stInitInfo.u32SC0_Sub_Phy, stInitInfo.u32SC0_Sub_Size, SUB_WINDOW);
    }

    if(stInitInfo.u32SC1_Main_Phy && stInitInfo.u32SC1_Main_Size)
    {
        MApi_XC_SetFrameBufferAddress(stInitInfo.u32SC1_Main_Phy, stInitInfo.u32SC1_Main_Size, SC1_MAIN_WINDOW);
    }

    if(stInitInfo.u32SC2_Main_Phy && stInitInfo.u32SC2_Main_Size)
    {
        MApi_XC_SetFrameBufferAddress(stInitInfo.u32SC2_Main_Phy, stInitInfo.u32SC2_Main_Size, SC2_MAIN_WINDOW);
    }

    if(stInitInfo.u32SC2_Sub_Phy && stInitInfo.u32SC2_Sub_Size)
    {
        MApi_XC_SetFrameBufferAddress(stInitInfo.u32SC2_Sub_Phy, stInitInfo.u32SC2_Sub_Size, SC2_SUB_WINDOW);
    }

    MApi_PCMode_Init_UserModeSetting();

    //MApi_XC_SetDbgLevel(XC_DBGLEVEL_SETWINDOW|XC_DGBLEVEL_CROPCALC);
}

static void _MApi_XC_Sys_Init_PQ_CallBack(void)
{
    PQ_Function_Info function_Info;

    memset( &function_Info , 0 , sizeof(PQ_Function_Info) );

    function_Info.pq_deside_srctype = MDrv_PQ_DesideSrcType;
    function_Info.pq_disable_filmmode = MDrv_PQ_DisableFilmMode;
    function_Info.pq_load_scalingtable = MDrv_PQ_LoadScalingTable;
    function_Info.pq_set_420upsampling = MDrv_PQ_Set420upsampling;
    function_Info.pq_set_csc = MDrv_PQ_SetCSC;
    function_Info.pq_set_memformat = MDrv_PQ_SetMemFormat;
    function_Info.pq_set_modeInfo = MDrv_PQ_Set_ModeInfo;
    function_Info.pq_get_memyuvfmt = MDrv_PQ_Get_MemYUVFmt;
    function_Info.pq_ioctl = MDrv_PQ_IOCTL;


    MApi_XC_PQ_LoadFunction(&function_Info , sizeof(PQ_Function_Info));
    SCA_DBG(SCA_DBG_LV_INIT_XC, "[1]Set PQ Load function...\n");

}

MS_U8 _Trans_PQ_BinID(SCA_PQ_BIN_ID_TYPE enPQIDType)
{
    MS_U8 u8ret;

    switch(enPQIDType)
    {
    case SCA_PQ_BIN_ID_STD_MAIN:
        u8ret = PQ_BIN_STD_MAIN;
        break;
    case SCA_PQ_BIN_ID_STD_SUB:
        u8ret = PQ_BIN_STD_SUB;
        break;
    case SCA_PQ_BIN_ID_STD_SC1_MAIN:
        u8ret = PQ_BIN_STD_SC1_MAIN;
        break;
    case SCA_PQ_BIN_ID_STD_SC2_MAIN:
        u8ret = PQ_BIN_STD_SC2_MAIN;
        break;
    case SCA_PQ_BIN_ID_STD_SC2_SUB:
        u8ret = PQ_BIN_STD_SC2_SUB;
        break;
    case SCA_PQ_BIN_ID_EXT_MAIN:
        u8ret = PQ_BIN_EXT_MAIN;
        break;
    case SCA_PQ_BIN_ID_EXT_SUB:
        u8ret = PQ_BIN_EXT_SUB;
        break;
    case SCA_PQ_BIN_ID_EXT_SC1_MAIN:
        u8ret = PQ_BIN_EXT_SC1_MAIN;
        break;
    case SCA_PQ_BIN_ID_EXT_SC2_MAIN:
        u8ret = PQ_BIN_EXT_SC2_MAIN;
        break;
    case SCA_PQ_BIN_ID_EXT_SC2_SUB:
        u8ret = PQ_BIN_EXT_SC2_SUB;
        break;
    default:
        u8ret = MAX_PQ_BIN_NUM;
        break;
    }
    return u8ret;
}

void MApi_XC_Init_PQ_Bin(MS_PQ_Init_Info *pPQInitInfo)
{
    int i;
    MS_U8 u8PQBinNum = 0;
    for(i=0;i<SCA_PQ_BIN_ID_NUM; i++)
    {
        if(stPQBinData[i].enPQBinIDType != SCA_PQ_BIN_ID_NONE &&
           stPQBinData[i].pBuff && stPQBinData[i].u32Len)
        {
            pPQInitInfo->stPQBinInfo[u8PQBinNum].u8PQID = _Trans_PQ_BinID(stPQBinData[i].enPQBinIDType);
            pPQInitInfo->stPQBinInfo[u8PQBinNum].pPQBin_AddrVirt = (void *)stPQBinData[i].pBuff;
            pPQInitInfo->stPQBinInfo[u8PQBinNum].PQBin_PhyAddr = (MS_PHYADDR)stPQBinData[i].pBuff;
            pPQInitInfo->stPQBinInfo[u8PQBinNum].PQ_Bin_BufSize = stPQBinData[i].u32Len;

		   /*
            SCA_DBG(SCA_DBG_LV_PQ_BIN,"[PQ_BIN] ID:%d, Size:%d PhyAddr:%x \n",
                (int)pPQInitInfo->stPQBinInfo[u8PQBinNum].u8PQID,
                (int)pPQInitInfo->stPQBinInfo[u8PQBinNum].PQ_Bin_BufSize,
                (int)pPQInitInfo->stPQBinInfo[u8PQBinNum].PQBin_PhyAddr);
                */

            u8PQBinNum++;
        }
    }

    SCA_DBG(SCA_DBG_LV_PQ_BIN,"[PQ_BIN] BinNum:%d\n", u8PQBinNum);
    pPQInitInfo->u8PQBinCnt = u8PQBinNum;
    pPQInitInfo->u8PQTextBinCnt = 0;
}

void MApi_XC_Sys_Init_PQ(MS_U16 u16PNL_Width, MS_U16 u16PNL_Vtt)
{
    MS_PQ_Init_Info  sXC_PQ_InitData;

    memset(&sXC_PQ_InitData, 0, sizeof(MS_PQ_Init_Info));

    // Init PQ related
#if (DRAM_TYPE == DDR_II)
     sXC_PQ_InitData.bDDR2 = TRUE;
#else
     sXC_PQ_InitData.bDDR2 = FALSE;
#endif

    sXC_PQ_InitData.u32DDRFreq = DDRPLL_FREQ;

#if (DRAM_BUS == DRAM_BUS_16)
        sXC_PQ_InitData.u8BusWidth = 16;
#elif (DRAM_BUS == DRAM_BUS_32)
        sXC_PQ_InitData.u8BusWidth = 32;
#else
    #error "DRAM_TYPE: BUS_WIDTH NOT DEFINE"
#endif

    if(u16PNL_Width == 1920)
    {
        sXC_PQ_InitData.u16PnlWidth = 1920;
    }
    else
    {
        sXC_PQ_InitData.u16PnlWidth = 1366;
    }

    sXC_PQ_InitData.u16OSD_hsize =  u16PNL_Width;
    sXC_PQ_InitData.u16Pnl_vtotal = u16PNL_Vtt;

#if (ENABLE_PQ_BIN)
    MApi_XC_Init_PQ_Bin(&sXC_PQ_InitData);
#else
    sXC_PQ_InitData.u8PQBinCnt = 0;
    sXC_PQ_InitData.u8PQTextBinCnt = 0;
#endif


    MDrv_PQ_Init(&sXC_PQ_InitData);

    SCA_DBG(SCA_DBG_LV_INIT_XC, "[N] PQ init. finished!\n");
}

#if defined(SCA_ENABLE_DLC)
void MApi_XC_Sys_Init_DLC(void)
{
    // Init ACE
    MS_U8 i;
    XC_DLC_init sXC_DLC_InitData;

    memset(&sXC_DLC_InitData, 0, sizeof(XC_DLC_init));

#if PQ_ENABLE_COLOR
    MApi_DLC_Get_From_PQ(MAIN_WINDOW);
#endif

    for (i=0; i<16; ++i)
    {
        sXC_DLC_InitData.DLC_MFinit_Ex.ucLumaCurve[i]     = mApi_DLC_GetLumaCurve(i);
        sXC_DLC_InitData.DLC_MFinit_Ex.ucLumaCurve2_a[i]  = mApi_DLC_GetLumaCurve2_a(i);
        sXC_DLC_InitData.DLC_MFinit_Ex.ucLumaCurve2_b[i]  = mApi_DLC_GetLumaCurve2_b(i);
    }

    for (i=0; i<DLC_HISTOGRAM_LIMIT_CURVE_ARRARY_NUM; ++i)
    {
        sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcHistogramLimitCurve[i] = mApi_DLC_GetDlcHistogramLimitCurve(i);
    }
    sXC_DLC_InitData.DLC_MFinit_Ex.u32DLC_MFinit_Ex_Version= DLC_MFINIT_EX_VERSION;
    sXC_DLC_InitData.DLC_MFinit_Ex.u8_L_L_U               = DLC_PARAMETER_L_L_U; // default value: 10
    sXC_DLC_InitData.DLC_MFinit_Ex.u8_L_L_D               = DLC_PARAMETER_L_L_D; // default value: 10
    sXC_DLC_InitData.DLC_MFinit_Ex.u8_L_H_U               = DLC_PARAMETER_L_H_U; // default value: 10
    sXC_DLC_InitData.DLC_MFinit_Ex.u8_L_H_D               = DLC_PARAMETER_L_H_D; // default value: 10
    sXC_DLC_InitData.DLC_MFinit_Ex.u8_S_L_U               = DLC_PARAMETER_S_L_U; // default value: 128 (0x80)
    sXC_DLC_InitData.DLC_MFinit_Ex.u8_S_L_D               = DLC_PARAMETER_S_L_D; // default value: 128 (0x80)
    sXC_DLC_InitData.DLC_MFinit_Ex.u8_S_H_U               = DLC_PARAMETER_S_H_U; // default value: 128 (0x80)
    sXC_DLC_InitData.DLC_MFinit_Ex.u8_S_H_D               = DLC_PARAMETER_S_H_D; // default value: 128 (0x80)

    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcPureImageMode     = DLC_PARAMETER_PURE_IMAGE_MODE; // Compare difference of max and min bright
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcLevelLimit        = DLC_PARAMETER_LEVEL_LIMIT; // n = 0 ~ 4 => Limit n levels => ex. n=2, limit 2 level 0xF7, 0xE7
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcAvgDelta          = DLC_PARAMETER_AVG_DELTA; // n = 0 ~ 50, default value: 12
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcAvgDeltaStill     = DLC_PARAMETER_AVG_DELTA_STILL; // n = 0 ~ 15 => 0: disable still curve, 1 ~ 15: enable still curve
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcFastAlphaBlending = DLC_PARAMETER_FAST_ALPHA_BLENDING; // min 17 ~ max 32
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcYAvgThresholdL    = DLC_PARAMETER_Y_AVG_THRESHOLD_L; // default value: 0
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcYAvgThresholdH    = DLC_PARAMETER_Y_AVG_THRESHOLD_H; // default value: 128
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcBLEPoint          = DLC_PARAMETER_BLE_POINT; // n = 24 ~ 64, default value: 48
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcWLEPoint          = DLC_PARAMETER_WLE_POINT; // n = 24 ~ 64, default value: 48
    sXC_DLC_InitData.DLC_MFinit_Ex.bEnableBLE             = DLC_PARAMETER_ENABLE_BLE; // 1: enable; 0: disable
    sXC_DLC_InitData.DLC_MFinit_Ex.bEnableWLE             = DLC_PARAMETER_ENABLE_WLE; // 1: enable; 0: disable

    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcYAvgThresholdM    = DLC_PARAMETER_Y_AVG_THRESHOLD_M;
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcCurveMode         = DLC_PARAMETER_CURVE_MODE;
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcCurveModeMixAlpha = DLC_PARAMETER_CURVE_MIXALPHA;
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcAlgorithmMode     = DLC_PARAMETER_ALGORITHM_MODE;
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcSepPointH         = DLC_PARAMETER_SepPoint_H;
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcSepPointL         = DLC_PARAMETER_SepPoint_L;
    sXC_DLC_InitData.DLC_MFinit_Ex.uwDlcBleStartPointTH   = DLC_PARAMETER_BLEStartPointTH;
    sXC_DLC_InitData.DLC_MFinit_Ex.uwDlcBleEndPointTH     = DLC_PARAMETER_BLEEndPointTH;
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcCurveDiff_L_TH    = DLC_PARAMETER_DLC_CurveDiff_L_TH;
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcCurveDiff_H_TH    = DLC_PARAMETER_DLC_CurveDiff_H_TH;
    sXC_DLC_InitData.DLC_MFinit_Ex.uwDlcBLESlopPoint_1    = DLC_PARAMETER_BLE_Slop_Point1;
    sXC_DLC_InitData.DLC_MFinit_Ex.uwDlcBLESlopPoint_2    = DLC_PARAMETER_BLE_Slop_Point2;
    sXC_DLC_InitData.DLC_MFinit_Ex.uwDlcBLESlopPoint_3    = DLC_PARAMETER_BLE_Slop_Point3;
    sXC_DLC_InitData.DLC_MFinit_Ex.uwDlcBLESlopPoint_4    = DLC_PARAMETER_BLE_Slop_Point4;
    sXC_DLC_InitData.DLC_MFinit_Ex.uwDlcBLESlopPoint_5    = DLC_PARAMETER_BLE_Slop_Point5;
    sXC_DLC_InitData.DLC_MFinit_Ex.uwDlcDark_BLE_Slop_Min = DLC_PARAMETER_BLE_Dark_Slop_Min;
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcCurveDiffCoringTH = DLC_PARAMETER_DLC_CurveDiff_Coring_TH;
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcAlphaBlendingMin  = DLC_PARAMETER_DLC_FAST_ALPHA_BLENDING_MIN;
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcAlphaBlendingMax  = DLC_PARAMETER_DLC_FAST_ALPHA_BLENDING_MAX;

    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcFlicker_alpha     = DLC_PARAMETER_DLC_FlICKER_ALPHA;
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcYAVG_L_TH         = DLC_PARAMETER_DLC_YAVG_L_TH;
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcYAVG_H_TH         = DLC_PARAMETER_DLC_YAVG_H_TH;

    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcDiffBase_L             = DLC_PARAMETER_DLC_DiffBase_L;
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcDiffBase_M             = DLC_PARAMETER_DLC_DiffBase_M;
    sXC_DLC_InitData.DLC_MFinit_Ex.ucDlcDiffBase_H             = DLC_PARAMETER_DLC_DiffBase_H;

    sXC_DLC_InitData.u16CurveHStart                    = (MApi_GetProjPaneltype()->m_wPanelWidth)/8;
    sXC_DLC_InitData.u16CurveHEnd                      = (MApi_GetProjPaneltype()->m_wPanelWidth)*7/8;
    sXC_DLC_InitData.u16CurveVStart                    = (MApi_GetProjPaneltype()->m_wPanelHeight)/8;
    sXC_DLC_InitData.u16CurveVEnd                      = (MApi_GetProjPaneltype()->m_wPanelHeight)*7/8;

    // CGC
    sXC_DLC_InitData.DLC_MFinit_Ex.ucCGCCGain_offset      = DLC_CGC_CGAIN_OFFSET;
    sXC_DLC_InitData.DLC_MFinit_Ex.ucCGCYCslope           = DLC_CGC_YC_SLOPE; // 0x01~0x20
#if ( DLC_C_GAIN_CONTROL )
    sXC_DLC_InitData.DLC_MFinit_Ex.ucCGCChroma_GainLimitH = DLC_CGC_CGAIN_LIMIT_H; // 0x00~0x6F
    sXC_DLC_InitData.DLC_MFinit_Ex.ucCGCChroma_GainLimitL = DLC_CGC_CGAIN_LIMIT_L; // 0x00~0x10
#else
    sXC_DLC_InitData.DLC_MFinit_Ex.ucCGCChroma_GainLimitH = 0;
    sXC_DLC_InitData.DLC_MFinit_Ex.ucCGCChroma_GainLimitL = 0;
#endif

    sXC_DLC_InitData.DLC_MFinit_Ex.ucCGCYth               = DLC_CGC_Y_TH;
    sXC_DLC_InitData.DLC_MFinit_Ex.bCGCCGainCtrl          = DLC_CGC_ENABLE; //Dixon, DISABLE CGC

    if(MApi_XC_DLC_Init_Ex(&sXC_DLC_InitData, sizeof(XC_DLC_init)) == FALSE)
    {
        SCA_DBG(SCA_DBG_LV_0, "DLC_Init failed because of InitData wrong, please update header file and compile again \n");
    }
    SCA_DBG(SCA_DBG_LV_0, "[SCA] DLC init. finished! \n");
}
#endif


void MApi_XC_GetGamma_From_PQ(SCALER_WIN eWindow, MS_U8 *pTabR, MS_U8 *pTabG, MS_U8 *pTabB)
{
#if PQ_ENABLE_COLOR
    #define PQ_GAMMA_SIZE 386
    MS_U8 gtPQGamma[PQ_GAMMA_SIZE];
    PQ_WIN PQWin = eWindow == MAIN_WINDOW     ? PQ_MAIN_WINDOW :
                   eWindow == SUB_WINDOW      ? PQ_MAIN_WINDOW :
                   eWindow == SC2_MAIN_WINDOW ? PQ_SC2_MAIN_WINDOW :
                   eWindow == SC2_SUB_WINDOW  ? PQ_SC2_MAIN_WINDOW :
                                                PQ_SC1_MAIN_WINDOW;

    if(MDrv_PQ_Get_Picture(PQWin, PQ_INPUT_SOURCE_NONE, NULL, E_PQ_PICTURE_GAMMA_TBL_R, (void *)gtPQGamma))
    {
        memcpy(pTabR, gtPQGamma, sizeof(MS_U8)*PQ_GAMMA_SIZE);
    }


    if(MDrv_PQ_Get_Picture(PQWin, PQ_INPUT_SOURCE_NONE, NULL, E_PQ_PICTURE_GAMMA_TBL_G, (void *)gtPQGamma))
    {
        memcpy(pTabG, gtPQGamma, sizeof(MS_U8)*PQ_GAMMA_SIZE);
    }

    if(MDrv_PQ_Get_Picture(PQWin, PQ_INPUT_SOURCE_NONE, NULL, E_PQ_PICTURE_GAMMA_TBL_B, (void *)gtPQGamma))
    {
        memcpy(pTabB, gtPQGamma, sizeof(MS_U8)*PQ_GAMMA_SIZE);
    }
#endif
}


void _MApi_XC_Init_HDMITx(void)
{
    XC_HDMITX_INITDATA stHDMI_InitData;
    XC_HDMITX_BOARD_INFO stHDMITxInfo =
    {
        HDMITX_ANALOG_TUNING_SD,
        HDMITX_ANALOG_TUNING_HD,
        HDMITX_ANALOG_TUNING_DEEP_HD,
    };

    stHDMI_InitData.bDisableHDMI = TRUE;
    stHDMI_InitData.eHdmitxOutputMode = E_XC_HDMITX_OUTPUT_HDMI;
    stHDMI_InitData.eHdmitxCDType = E_XC_HDMITX_CD_8BITS;
    stHDMI_InitData.u16HdmitxHpdPin = 0x09; //PM_GPIO 09
    stHDMI_InitData.eHdmitxOutputTiming = E_XC_HDMITX_480_60I;
    memcpy(&stHDMI_InitData.stHdmitxBoardInfo, &stHDMITxInfo, sizeof(XC_HDMITX_BOARD_INFO));

    MApi_Init_HDMITx(stHDMI_InitData);
}

void _MApi_XC_Init_Pnl_SSC_Current(void)
{
    MS_U16 u16CurrDrv, u16Tmp;
    #define REG_GPIO1_19_L (0x101A00+(0x19*2))
    #define REG_GPIO1_1A_L (0x101A00+(0x1A*2))

    // SSC
    MApi_PNL_SetSSC_Fmodulation(400);
    MApi_PNL_SetSSC_Rdeviation(600);
    MApi_PNL_SetSSC_En(TRUE);

    //Current
    u16Tmp = (MApi_GetProjPaneltype()->m_ucPanelDECurrent) & 0x03;

    u16CurrDrv = (u16Tmp << 0) |
                 (u16Tmp << 2) |
                 (u16Tmp << 4) |
                 (u16Tmp << 6);

    u16CurrDrv =  (u16CurrDrv << 8) | u16CurrDrv;

    MApi_XC_Write2ByteMask(REG_GPIO1_19_L, u16CurrDrv, 0xFF00);
    MApi_XC_Write2ByteMask(REG_GPIO1_1A_L, u16CurrDrv, 0xFFFF);

}
///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------
void MApi_XC_Sys_Init(XC_INIT_INFO stInitInfo)
{
    XC_INITDATA stXCInit_Data;

	if (g_bXCSysInitReady)
	{
	    SCA_DBG(SCA_DBG_LV_0, "MApi_XC_Sys_Init: already init \n");
	    return;
    }

    if(!bInitPNL)
    {
        SCA_DBG(SCA_DBG_LV_INIT_XC, "MApi_XC_Sys_Init: PNL Init \n");
        MApi_XC_Sys_Init_Pnl();
    }


    memset (&stXCInit_Data, 0, sizeof(XC_INITDATA));
    _MApi_XC_Sys_Init_XC(&stXCInit_Data, stInitInfo);

    _MApi_XC_Init_Pnl_SSC_Current();


    _MApi_XC_Sys_Init_PQ_CallBack();

    MApi_XC_Sys_Init_PQ(stXCInit_Data.stPanelInfo.u16Width, stXCInit_Data.stPanelInfo.u16VTotal);


    // Init Gamma table
    SCA_DBG(SCA_DBG_LV_INIT_XC, "[SCA] Init Gamma \n");
#if PQ_ENABLE_COLOR
    MApi_XC_GetGamma_From_PQ(MAIN_WINDOW, tSC0_NormalGammaR, tSC0_NormalGammaG, tSC0_NormalGammaB);
    MApi_PNL_SetGammaTbl(E_APIPNL_GAMMA_12BIT, tSC0_AllGammaTab, E_APIPNL_GAMMA_8BIT_MAPPING);
#else
    MApi_PNL_SetGammaTbl(E_APIPNL_GAMMA_12BIT, tAllGammaTab, E_APIPNL_GAMMA_8BIT_MAPPING);
#endif


#if defined(SCA_ENABLE_DLC)
    MApi_XC_Sys_Init_DLC();
    MApi_XC_DLC_CGC_ResetCGain();

    MApi_XC_DLC_CGC_CheckCGainInPQCom();
#endif

    _MApi_XC_Sys_Init_ACE();

    MApi_XC_ModeParse_Init();

    // Init timing monitor
    MApi_XC_PCMonitor_Init(MApi_XC_GetInfo()->u8MaxWindowNum);

    MApi_XC_Mux_Init(MApi_InputSource_InputPort_Mapping);

    _MApi_XC_Init_HDMITx();

    if(((VE_DrvInfo *)MDrv_VE_GetInfo())->stCaps.bVESupported)
    {
        XC_TGEN_INFO_t stTGenInfo;

        MDrv_VE_Init(0);

        MApi_SetBlueScreen(ENABLE, E_XC_FREE_RUN_COLOR_BLACK, 300, SC1_MAIN_WINDOW);

        MDrv_VE_SetOutputVideoStd(MS_VE_NTSC);
        stTGenInfo.enTimeType = E_XC_TGEN_VE_480_I_60;
        MApi_XC_Set_TGen(stTGenInfo, SC1_MAIN_WINDOW);

        MApi_XC_SetFrameColor(0x800080, SC1_MAIN_WINDOW);
        MApi_XC_SetFrameColor_En(TRUE, SC1_MAIN_WINDOW);

    }

    //FPLL FSM
    MApi_SC_Enable_FPLL_FSM(FALSE);

    MDrv_BW_LoadInitTable();

#if ENABLE_2_GOP_BLENDING_PATCH
    MApi_XC_SetMemoryWriteRequest(FALSE, MAIN_WINDOW);
    MApi_XC_DisableInputSource(FALSE, MAIN_WINDOW);
    MApi_XC_SetMemoryWriteRequest(FALSE, SC1_MAIN_WINDOW);
    MApi_XC_DisableInputSource(FALSE, SC1_MAIN_WINDOW);

    MApi_XC_SetMemoryWriteRequest(FALSE, SC2_MAIN_WINDOW);
    MApi_XC_DisableInputSource(FALSE, SC2_MAIN_WINDOW);
    MApi_XC_SetMemoryWriteRequest(FALSE, SC2_SUB_WINDOW);
    MApi_XC_DisableInputSource(FALSE, SC2_SUB_WINDOW);
    MApi_XC_SetMemoryWriteRequest(FALSE, SUB_WINDOW);
    MApi_XC_DisableInputSource(FALSE, SUB_WINDOW);
#endif

    MApi_XC_GenerateBlackVideo( ENABLE, MAIN_WINDOW );
    MApi_XC_GenerateBlackVideo( ENABLE, SUB_WINDOW );
    MApi_XC_GenerateBlackVideo( ENABLE, SC1_MAIN_WINDOW );
    MApi_XC_GenerateBlackVideo( ENABLE, SC2_MAIN_WINDOW );
    MApi_XC_GenerateBlackVideo( ENABLE, SC2_SUB_WINDOW );

    SCA_DBG(SCA_DBG_LV_INIT_XC, "[SCA] Init MVOP \n");
    MDrv_MVOP_Init();
    SCA_DBG(SCA_DBG_LV_INIT_XC, "[SCA] XC_Sys_Int Finish \n");

	g_bXCSysInitReady = TRUE;

}

MS_BOOL MApi_XC_GetSysInitFlag(void)
{
	return g_bXCSysInitReady;
}

void MApi_XC_ClearSysInitFlag(void)
{
	g_bXCSysInitReady = FALSE;
}

MS_BOOL IsRTKCvbsInputLocked(void)
{
    #define RTK_CVBS_IN_MASK 0x02

	MS_BOOL bRet = FALSE;

	if ( DEVINFO_RTK_FLAG_1 == ms_devinfo_rtk_flag() && (GetMailBoxStatus() & RTK_CVBS_IN_MASK) )
	{
		bRet = TRUE;
	}

	return bRet;
}

MS_BOOL IsRTKCarBacking(void)
{
    #define RTK_CAR_BACK_MASK 0x04

	MS_BOOL bRet = FALSE;

    //0 for car back, 1 for forward
	if ( DEVINFO_RTK_FLAG_1 == ms_devinfo_rtk_flag() && !(GetMailBoxStatus() & RTK_CAR_BACK_MASK) )
	{
		bRet = TRUE;
	}

	return bRet;
}

void MApi_SetBlueScreen( MS_BOOL bEnable, MS_U8 u8Color, MS_U16 u16ScreenUnMuteTime, SCALER_WIN eWindow)
{
    //UNUSED(eWindow);

    if (bEnable )
    {
        MApi_SetScreenMute(E_SCREEN_MUTE_FREERUN, ENABLE, 0, eWindow);

        u8Color = (u8Color >= E_XC_FREE_RUN_COLOR_MAX) ? E_XC_FREE_RUN_COLOR_BLACK : u8Color;
        //stSystemInfo[eWindow].enFreeRunColor = (XC_FREERUN_COLOR)u8Color;

        if(eWindow == MAIN_WINDOW)
        {
            MApi_XC_SetFreeRunTiming();
        }

        if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
            MApi_XC_SetDispWindowColor(astNoSignalColor[u8Color], eWindow);
        else
            MApi_XC_SetDispWindowColor(astNoSignalColorYUV[u8Color], eWindow);

#if ENABLE_2_GOP_BLENDING_PATCH
        MApi_XC_SetMemoryWriteRequest(FALSE, eWindow);
#else
        MApi_XC_DisableInputSource(TRUE, eWindow);
#endif


        // turn off NR for disable memory write (for MM case)
        //MApi_XC_Sys_PQ_SetNR( E_XC_PQ_3D_NR_OFF, eWindow );

        stSystemInfo[eWindow].u8PanelPowerStatus |= PANEL_POWER_BLUESCREEN;
    }
    else
    {
        if(stSystemInfo[eWindow].u8PanelPowerStatus & PANEL_POWER_BLUESCREEN)
        {
            MSG(SCA_DBG(SCA_DBG_LV_0, "Disable BlueScreen(%u)\n", MsOS_GetSystemTime()));

#if ENABLE_2_GOP_BLENDING_PATCH
            MApi_XC_SetMemoryWriteRequest(TRUE, eWindow);
#else
            MApi_XC_DisableInputSource(FALSE, eWindow);
#endif

            if(IsSrcTypeDTV(stSystemInfo[eWindow].enInputSourceType))
            {
                MApi_XC_set_FD_Mask(FALSE, eWindow); //pathc no sync due to FD Mask
            }

            msleep(75); // dealy to avoid garbage screen

            if(IsSrcTypeDTV(stSystemInfo[eWindow].enInputSourceType))
            {
                MApi_XC_set_FD_Mask(TRUE, eWindow); //pathc no sync due to FD Mask
            }

            MApi_SetScreenMute(E_SCREEN_MUTE_TEMPORARY, ENABLE, u16ScreenUnMuteTime, eWindow);
            MApi_SetScreenMute(E_SCREEN_MUTE_FREERUN, DISABLE, 0, eWindow);

            stSystemInfo[eWindow].u8PanelPowerStatus &= ~PANEL_POWER_BLUESCREEN;
        }
        else
        {
            MSG(SCA_DBG(SCA_DBG_LV_0, "Disable BlueScreen return\n"));
        }
    }

}

void MApi_SetScreenMute(E_SCREEN_MUTE_STATUS eScreenMute, MS_BOOL bMuteEnable, MS_U16 u161ms , SCALER_WIN eWindow)
{
    //MS_BOOL bMLoadEn = MApi_XC_MLoad_GetStatus() == E_MLOAD_ENABLED ? TRUE : FALSE;
    SCA_DBG(SCA_DBG_LV_SCREEN_MUTE, "ewin:%d, Mute:En:%d, Cur:%x, State:%x \n", eWindow, bMuteEnable, eScreenMute, s_eScreenMuteStatus[eWindow]);

    //MApi_XC_MLoad_Enable(FALSE);
    if( bMuteEnable )
    {
        if(eScreenMute == E_SCREEN_MUTE_TEMPORARY)
        {
            MSG(SCA_DBG(SCA_DBG_LV_0, "E_SCREEN_MUTE_TEMPORARY Before %u\r\n",(MS_U16)u161ms));
            //u161ms = MAX( u161ms, msAPI_Timer_GetDownTimer0() );
            MSG(SCA_DBG(SCA_DBG_LV_0, "E_SCREEN_MUTE_TEMPORARY After %u\r\n",(MS_U16)u161ms));
            //msAPI_Timer_SetDownTimer0(u161ms);
        }

        if((s_eScreenMuteStatus[eWindow]& eScreenMute) && bMuteEnable)
        {
            MSG(SCA_DBG(SCA_DBG_LV_0, "msAPI_Scaler_SetScreenMute(%x, %d) return, orig status %x\r\n",
                    eScreenMute, bMuteEnable, s_eScreenMuteStatus[eWindow]));
            return;
        }

        s_eScreenMuteStatus[eWindow] |= eScreenMute;

        {
            if( MApi_XC_IsBlackVideoEnable(eWindow) == FALSE )
            {
                MApi_XC_GenerateBlackVideo( ENABLE, eWindow );
                //MSG(RETAILMSG(UTOPIA_FAIL, (TEXT("MApi_XC_GenerateBlackVideo Enable %bu\r\n"), (MS_U8)eScreenMute)));
            }
        }
    }
    else
    {
        E_SCREEN_MUTE_STATUS eScreenMuteTmp;
        //s_eScreenMuteStatus &= ~eScreenMute;

        eScreenMuteTmp = s_eScreenMuteStatus[eWindow];
        s_eScreenMuteStatus[eWindow] = (E_SCREEN_MUTE_STATUS)((MS_U8)s_eScreenMuteStatus[eWindow] & ~((MS_U8)eScreenMute));

        if (eScreenMuteTmp != s_eScreenMuteStatus[eWindow])
        {
            if( s_eScreenMuteStatus[eWindow] == E_SCREEN_MUTE_INIT  )
            {
                {
                    MApi_XC_GenerateBlackVideo( DISABLE, eWindow );

                }
            }
        }
    }
    //MApi_XC_MLoad_Enable(bMLoadEn);
    //MSG(RETAILMSG(UTOPIA_FAIL, (TEXT("s_eScreenMuteStatus %bu\n"), (MS_U8)s_eScreenMuteStatus[eWindow])));
}

#if 0
//-------------------------------------------------------------------------------
// Debug...
//-------------------------------------------------------------------------------
extern BOOL SCA_Connect(PSCA_CONNECT_CONFIG pConfig);
extern BOOL SCA_DisConnect(PSCA_CONNECT_CONFIG pConfig);
extern BOOL SCA_Set_Timing_Window(PSCA_SET_TIMING_WINDOW_CONFIG pConfig);
extern BOOL SCA_Set_Display_Mute(PSCA_DISPLAY_MUTE_CONFIG pConfig, E_SCREEN_MUTE_STATUS enMuteStatus);
extern BOOL SCA_YPBPR_VGA_Mode_Monitor_Parse(PSCA_MODE_MONITOR_PARSE_IN_CONFIG pInConfig, PSCA_MODE_MONITOR_PARSE_OUT_CONFIG pOutConfig);
extern BOOL SCA_AVD_Connect(AVD_CONNECT_CONFIG *pCfg);
extern BOOL SCA_AVD_Check_VideoStd(AVD_CHECK_VIDEOSTD_CONFIG *pCfg);
extern BOOL SCA_Change_Window(PSCA_CHANGE_WINDOW_CONFIG pConfig);


void Test_AVD(void)
{
    MS_U32 u32CurTime;
    SCA_SRC_TYPE enCurSrc = SCA_SRC_CVBS;
    SCA_DEST_TYPE enCurDest = SCA_DEST_MAIN;

	SCA_CONNECT_CONFIG stConnectCfg;
    AVD_CONNECT_CONFIG stAVDConnectCfg;
    SCA_DISPLAY_MUTE_CONFIG stDisplayMuteCfg;
    SCA_SET_TIMING_WINDOW_CONFIG stWin_Timing_Cng;
    AVD_CHECK_VIDEOSTD_CONFIG stAVD_VideoStd;


    stDisplayMuteCfg.bEn = TRUE;
    stDisplayMuteCfg.enDestType = enCurDest;
    SCA_Set_Display_Mute(&stDisplayMuteCfg, E_SCREEN_MUTE_BLOCK);

	// 1) input source switch
	memset(&stConnectCfg, 0, sizeof(SCA_CONNECT_CONFIG));
	stConnectCfg.enSrcType = enCurSrc;
    stConnectCfg.enDestType = enCurDest;
    SCA_Connect(&stConnectCfg);

    //2 AVD Connect
    stAVDConnectCfg.enSrcType = (SCA_SRC_SVIDEO==enCurSrc)?
                                 AVD_SRC_SVIDEO:
                                 AVD_SRC_CVBS;
    SCA_AVD_Connect(&stAVDConnectCfg);

	u32CurTime = MsOS_GetSystemTime();
    while(1)
    {
        SCA_AVD_Check_VideoStd(&stAVD_VideoStd);

        if(stAVD_VideoStd.enVideoStandard != AVD_VIDEOSTANDARD_NOTSTANDARD)
        {
            //_tprintf(_T("The Video is stable: standard is %d !\n"), stAVD_VideoStd.enVideoStandard );
            break;
        }

        mdelay(25);
        if( MsOS_GetSystemTime() - u32CurTime >  0x1000)
            break;
    }

    memset(&stWin_Timing_Cng, 0, sizeof(SCA_SET_TIMING_WINDOW_CONFIG));

    stWin_Timing_Cng.enSrcType = enCurSrc;
    stWin_Timing_Cng.enDestType = enCurDest;

    stWin_Timing_Cng.stWinConfig.bCropWin = FALSE;
    stWin_Timing_Cng.stWinConfig.bDispWin = FALSE;

    SCA_Set_Timing_Window(&stWin_Timing_Cng);

    stDisplayMuteCfg.bEn = FALSE;
    SCA_Set_Display_Mute(&stDisplayMuteCfg, E_SCREEN_MUTE_BLOCK);

}

void Test_Analog(void)
{
    MS_U32 u32CurTime;
    SCA_SRC_TYPE enCurSrc = SCA_SRC_YPBPR;
    SCA_DEST_TYPE enCurDest = SCA_DEST_MAIN;
    SCA_CONNECT_CONFIG stConnectCfg;
    SCA_DISPLAY_MUTE_CONFIG stDisplayMuteCfg;
	SCA_MODE_MONITOR_PARSE_IN_CONFIG  stModeParseInCfg;
    SCA_MODE_MONITOR_PARSE_OUT_CONFIG stModeParseOutCfg;
    SCA_SET_TIMING_WINDOW_CONFIG stWin_Timing_Cng;
    static SCA_MODE_MOINITOR_STATUS ePreviousStatus = SCA_MODE_MONITOR_NOSYNC;

    //0 turn off display
    stDisplayMuteCfg.bEn = TRUE;
    stDisplayMuteCfg.enDestType = enCurDest;
    SCA_Set_Display_Mute(&stDisplayMuteCfg, E_SCREEN_MUTE_BLOCK);

	memset(&stConnectCfg, 0, sizeof(SCA_CONNECT_CONFIG));
	stConnectCfg.enSrcType = enCurSrc;
    stConnectCfg.enDestType = enCurDest;
    SCA_Connect(&stConnectCfg);

	// 2) mode parse
	stModeParseInCfg.enSrcType = enCurSrc;
	stModeParseInCfg.enDestType = enCurDest;

	u32CurTime = MsOS_GetSystemTime();

	while(1)
	{

        SCA_YPBPR_VGA_Mode_Monitor_Parse(&stModeParseInCfg, &stModeParseOutCfg);

        if(stModeParseOutCfg.eModeMonitorStatus == SCA_MODE_MONITOR_NOSYNC ||
           stModeParseOutCfg.eModeMonitorStatus == SCA_MODE_MONITOR_UNSTABLE)
        {
            stDisplayMuteCfg.bEn = TRUE;
            SCA_Set_Display_Mute(&stDisplayMuteCfg, E_SCREEN_MUTE_BLOCK);

        }
        else if( stModeParseOutCfg.eModeMonitorStatus == SCA_MODE_MONITOR_STABLE_SUPPORT_MODE && ePreviousStatus != stModeParseOutCfg.eModeMonitorStatus)
        {
            memset(&stWin_Timing_Cng, 0, sizeof(SCA_SET_TIMING_WINDOW_CONFIG));

            stWin_Timing_Cng.enSrcType = enCurSrc;
            stWin_Timing_Cng.enDestType = enCurDest;

            stWin_Timing_Cng.stWinConfig.bCropWin = FALSE;
            stWin_Timing_Cng.stWinConfig.bDispWin = FALSE;

            SCA_Set_Timing_Window(&stWin_Timing_Cng);

            stDisplayMuteCfg.bEn = FALSE;
            SCA_Set_Display_Mute(&stDisplayMuteCfg, E_SCREEN_MUTE_BLOCK);

        }
        else
        {

        }
        ePreviousStatus = stModeParseOutCfg.eModeMonitorStatus;

        mdelay(25);
        if( MsOS_GetSystemTime() - u32CurTime >  0x1000)
            break;

	}
}




void Test_input(void)
{
    INPUT_SOURCE_TYPE_t enInputSourceType = INPUT_SOURCE_YPBPR;
    SCALER_WIN eWindow = MAIN_WINDOW;
    //MS_MODEPARSE_INITDATA stModeParseInitData;
    MS_U32 u32CurTime;
    //MS_VE_Set_Mode_Type stVEMode;
    //XC_ApiStatus stXCStatus;

    // 1. input source switch
    MApi_InputSource_SwitchSource(enInputSourceType, eWindow);

    if(enInputSourceType == INPUT_SOURCE_YPBPR || enInputSourceType == INPUT_SOURCE_VGA)
    {
        // 2. Mode table init
        //memset(&stModeParseInitData, 0 , sizeof(MS_MODEPARSE_INITDATA));
        MApi_PCMode_Init(NULL);

        u32CurTime = MsOS_GetSystemTime();

        // 3. Mode Parsing
        while(MsOS_GetSystemTime() - u32CurTime < 1500)
        {
            XC_MODEPARSE_RESULT stModeParseResult;
            PCMODE_SyncStatus   stPCMonitorStatus;
            SCA_MODE_MOINITOR_STATUS eModeMonitorStatus;
            stModeParseResult = MApi_PC_MainWin_Handler(enInputSourceType, eWindow);

            stPCMonitorStatus = MApi_PCMode_GetCurrentState(eWindow);

            eModeMonitorStatus = stPCMonitorStatus == E_PCMODE_NOSYNC                 ? SCA_MODE_MONITOR_NOSYNC :
                                 stPCMonitorStatus == E_PCMODE_STABLE_SUPPORT_MODE    ? SCA_MODE_MONITOR_STABLE_SUPPORT_MODE :
                                 stPCMonitorStatus == E_PCMODE_STABLE_UN_SUPPORT_MODE ? SCA_MODE_MONITOR_STABLE_UN_SUPPORT_MODE :
                                                                                                    SCA_MODE_MONITOR_UNSTABLE;

            if(stPCMonitorStatus == E_PCMODE_STABLE_SUPPORT_MODE &&
                stModeParseResult == XC_MODEPARSE_SUPPORT_MODE)
            {
                SCA_DBG(SCA_DBG_LV_0, " ----------Mode Stable \n");
                break;
            }

            mdelay(20);
        }

        // 4. Load Defualt value.
        SCA_DBG(SCA_DBG_LV_0, "Mode Parsing Finish !!!!!!!!!!!!!! \n");
        MApi_PCMode_LoadDefualtTable(eWindow, MApi_PCMode_Get_Mode_Idx(eWindow));

    }
    else if(enInputSourceType == INPUT_SOURCE_DTV)
    {
        SCA_DBG(SCA_DBG_LV_0, "MApi_Set_MVOP \n");
        MApi_Set_MVOP();
    }

    // 5. set window & timing
    MApi_Set_Window_And_Timing(enInputSourceType, eWindow);

    // 6. Load PQ setting.
    MApi_SetColor(enInputSourceType, eWindow);

    MApi_SetBlueScreen(DISABLE, 3, 0, eWindow);

    MApi_SetScreenMute(E_SCREEN_MUTE_TEMPORARY, DISABLE, 0, eWindow);

#if 0
    //VE output
    if(MApi_XC_GetStatus(&stXCStatus, eWindow) == FALSE)
    {
        MSG(RETAILMSG(TRUE,TEXT("MApi_XC_GetStatus failed because of InitData wrong, please update header file and compile again\n")));
    }

    memset(&stVEMode, 0, sizeof(MS_VE_Set_Mode_Type));

    stVEMode.bHDuplicate = stXCStatus.bHDuplicate;
    stVEMode.bSrcInterlace = stXCStatus.bInterlace;
    stVEMode.u16InputVFreq = stXCStatus.u16InputVFreq;

    stVEMode.u16H_CapSize     = stXCStatus.stCapWin.width;
    stVEMode.u16V_CapSize     = stXCStatus.stCapWin.height;
    stVEMode.u16H_CapStart    = stXCStatus.stCapWin.x;
    stVEMode.u16V_CapStart    = stXCStatus.stCapWin.y;
    stVEMode.u16H_SC_CapSize  = stXCStatus.stCapWin.width;
    stVEMode.u16V_SC_CapSize  = stXCStatus.stCapWin.height;
    stVEMode.u16H_SC_CapStart = stXCStatus.stCapWin.x;
    stVEMode.u16V_SC_CapStart = stXCStatus.stCapWin.y;

    MApi_VE_SourceSwitch(enInputSourceType, OUTPUT_CVBS1);

    MApi_SetVE(enInputSourceType, OUTPUT_CVBS1, MS_VE_NTSC, stVEMode);
#endif

    MApi_XC_Write2ByteMask(0x101FFE, 0x0201, 0xFFFF);
    MApi_XC_Write2ByteMask(0x101F00, 0x0000, 0x0001);
}

void ChangeDispWin(SCA_SRC_TYPE enSrcType, SCA_DEST_TYPE enDestType, SCA_WINDOW_TYPE stDispWin, BOOL bImmediate)
{
    SCA_CHANGE_WINDOW_CONFIG stChangeWin;

    memset(&stChangeWin, 0, sizeof(SCA_CHANGE_WINDOW_CONFIG));
    stChangeWin.enSrcType = enSrcType;
    stChangeWin.enDestType = enDestType;
    stChangeWin.stWinConfig.bDispWin = TRUE;
    stChangeWin.stWinConfig.stDispWin.x = stDispWin.x;
    stChangeWin.stWinConfig.stDispWin.y = stDispWin.y;
    stChangeWin.stWinConfig.stDispWin.width = stDispWin.width;
    stChangeWin.stWinConfig.stDispWin.height= stDispWin.height;
    stChangeWin.bImmediateUpdate = bImmediate;
    SCA_Change_Window(&stChangeWin);

}


void Test_AutoDetect(SCA_SRC_TYPE enCurSrc, SCA_DEST_TYPE enCurDest)
{
    SCA_CONNECT_CONFIG stConnectCfg;
    SCA_WINDOW_TYPE stWinCfg;
    AVD_CONNECT_CONFIG stAVDConnectCfg;

    MApi_XC_Write2ByteMask(0x101F00+(0x7F*2), 0x201, 0xFFFF);
    MApi_XC_Write2ByteMask(0x101F00+(0x00*2), 0x000, 0x0001);

    stWinCfg.x = 0;
    stWinCfg.y = 0;
    stWinCfg.width = 800;
    stWinCfg.height = 480;
    ChangeDispWin(enCurSrc, enCurDest, stWinCfg, FALSE);

    SCA_DBG(SCA_DBG_LV_0, "+++ AutoDetect: S:%d D:%d \n", enCurSrc, enCurDest);


    if(enCurSrc == SCA_SRC_CVBS || enCurSrc == SCA_SRC_CVBS2 || enCurSrc == SCA_SRC_CVBS3 || enCurSrc ==SCA_SRC_SVIDEO)
    {
        stAVDConnectCfg.enSrcType = (SCA_SRC_SVIDEO==enCurSrc)?
                                     AVD_SRC_SVIDEO:
                                     AVD_SRC_CVBS;

        SCA_AVD_Connect(&stAVDConnectCfg);

    }

	memset(&stConnectCfg, 0, sizeof(SCA_CONNECT_CONFIG));

	stConnectCfg.bAutoDetect = TRUE;
	stConnectCfg.enSrcType = enCurSrc;
    stConnectCfg.enDestType = enCurDest;
    SCA_Connect(&stConnectCfg);



#if 0
	stConnectCfg.enSrcType = SCA_SRC_YPBPR;
    stConnectCfg.enDestType = SCA_DEST_MAIN_1;
    SCA_Connect(&stConnectCfg);


    mdelay(3000);


	stConnectCfg.enSrcType = SCA_SRC_CVBS;
    stConnectCfg.enDestType = SCA_DEST_MAIN;
    SCA_Connect(&stConnectCfg);

    mdelay(3000);

	stConnectCfg.enSrcType = SCA_SRC_YPBPR;
    stConnectCfg.enDestType = SCA_DEST_MAIN_1;
    SCA_DisConnect(&stConnectCfg);

    mdelay(30);
	stConnectCfg.enSrcType = enCurSrc;
    stConnectCfg.enDestType = enCurDest;
    SCA_DisConnect(&stConnectCfg);
#endif
}


#endif
