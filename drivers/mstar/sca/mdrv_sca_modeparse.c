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
#define MDRV_SCA_MODEPARSE_C

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>

#include "ms_platform.h"

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
#include "apiPNL.h"
#include "apiXC_Ace.h"
#include "drvTVEncoder.h"
#include "apiXC_Dlc.h"
#include "drvMVOP.h"
#include "drvMMIO.h"
#include "mdrv_sca_st.h"
#include "mdrv_sca.h"
#include "mdrv_sca_modeparse.h"
#include "mdrv_sca_inputsource.h"
//drver header files

//=============================================================================
// Local Defines
//=============================================================================

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#define PCMSG_DBG(x) //
//-------------------------------------------------------------------------------------------------

#define ENABLE_3D_PROCESS   0

#define PCMSG(x)            //x
#define PC_SelfAuto_MSG(x)  //x

// PCMSG_SEL definition //
#define PCMSG_SEL_DEBUG             FALSE
#define PCMSG_SEL_RETAIL            FALSE
#define PCMSG_SEL                   PCMSG_SEL_DEBUG

#define CALMSG_SEL					TRUE

// TODO:
#define ENABLE_USER_CALLBACK        (1)


// Range of RGB OOR
#if ENABLE_VGA_EIA_TIMING
#define MIN_HFREQ_OF_RGB            140  // 150
#else
#define MIN_HFREQ_OF_RGB            270    // 150
#endif
#define MAX_HFREQ_OF_RGB            950//840

#if ENABLE_VGA_EIA_TIMING
#define MIN_VFREQ_OF_RGB            240
#else
#define MIN_VFREQ_OF_RGB            470
#endif

#if ENABLE_RGB_SUPPORT_85HZ
#define MAX_VFREQ_OF_RGB            870
#else
#define MAX_VFREQ_OF_RGB            770
#endif

#define MIN_VTOTAL_OF_RGB           415
#define MAX_VTOTAL_OF_RGB           1550 // 1250
// Range of DVI OOR
#define MIN_HFREQ_OF_DVI            140    // same with HDMI
#define MAX_HFREQ_OF_DVI            950//840    // same with RGB
#define MIN_VFREQ_OF_DVI            230    // same with HDMI
#define MAX_VFREQ_OF_DVI            770    // same with RGB
#define MIN_VTOTAL_OF_DVI           250    // same with HDMI
#define MAX_WIDTH_OF_DVI            1930
#if ENABLE_3D_PROCESS          //for mode like 1280x1470(720x2+30), 1920x2205(1080x2+45)
#define MAX_VTOTAL_OF_DVI           2300//1250// same with HDMI
#define MAX_HEIGHT_OF_DVI           2800
#else
#define MAX_VTOTAL_OF_DVI           1300//1250// same with HDMI
#define MAX_HEIGHT_OF_DVI           1210
#endif

// Range of YPbPr OOR
#define MIN_HFREQ_OF_YPBPR          140
#define MIN_VFREQ_OF_YPBPR          230
#if ENABLE_3D_PROCESS          //for mode like 1280x1470(720x2+30), 1920x2205(1080x2+45)
#define MAX_HFREQ_OF_YPBPR          950
#define MIN_VTOTAL_OF_YPBPR         200
#define MAX_VTOTAL_OF_YPBPR         2300
#define MAX_VFREQ_OF_YPBPR          2700 //620 change for support 75Hz
#else
#define MAX_HFREQ_OF_YPBPR          700
#define MIN_VTOTAL_OF_YPBPR         250
#define MAX_VTOTAL_OF_YPBPR         1300
#define MAX_VFREQ_OF_YPBPR          770 //620 change for support 75Hz
#endif

//------------------------------------------------------------------------------
//  Global Variables
//------------------------------------------------------------------------------
MS_PCADC_MODESETTING_TYPE g_PcadcModeSetting[MAX_WINDOW];

//------------------------------------------------------------------------------
//  Local Variables
//------------------------------------------------------------------------------
static PCMode_Info              PCMODE_INFO[MAX_WINDOW] ;
static XC_PCMONITOR_STATUS      previous_status[MAX_WINDOW];
static MS_U8                    Enable_SelfAuto[MAX_WINDOW];

static MS_MODEPARSE_INITDATA    _tModeParseInitData  = {0};
static MS_PCADC_MODETABLE_TYPE* _pstTimingModeDB     = NULL;
static U8                       _u8NumOfModeDBItem   = 0;
static MS_BOOL 					bSrcIsHD[MAX_WINDOW];     // Daten FixMe, MAX_WINDOW will be removed and can be obtained from MApi_XC_GetInfo()

static MS_GENSETTING stGenSetting =
{
 {
    { //ADC_SET_VGA
      {
        0x1000, //R Gain
        0x1000, //G Gain
        0x1000, //B Gain
        0x0800, //R Offset
        0x0100, //G Offset
        0x0800, //B Offset
      },
      0,
    },
    { //ADC_SET_YPBPR_SD
      {
        0x1000, //R Gain
        0x1000, //G Gain
        0x1000, //B Gain
        0x0800, //R Offset
        0x0100, //G Offset
        0x0800, //B Offset
      },
      0,
    },

    { //ADC_SET_YPBPR_HD
      {
        0x1000, //R Gain
        0x1000, //G Gain
        0x1000, //B Gain
        0x0800, //R Offset
        0x0100, //G Offset
        0x0800, //B Offset
      },
      0,
    },
 },
};

static MS_PCADC_MODESETTING_TYPE  gstUserModeSetting[MD_STD_MODE_MAX_INDEX];

static MS_PCADC_MODETABLE_TYPE  astStandardModeTable[MD_STD_MODE_MAX_INDEX] =
{
    // 640x350 70Hz (IBM VGA)   --> 0
    {//MD_640x350_70    925FS-31    //0
    RES_640X350, // resolution index, use 640x400 resolution
    315, 700,     // HFreq, VFreq
    141, 59,      // HStart, VStart
    800, 449,     // HTotal, VTotal
    5,           // VTotalTorance
    0x26,         // ADC phase
    MD_FLAG_POR_HPVN,
    },

#if ENABLE_RGB_SUPPORT_85HZ
    // 640x350 85Hz (VESA)--->
    {
    RES_640X350, // resolution index
    379, 851,     // HFreq, VFreq
    160, 63,      // HStart, VStart
    832, 445,     // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,         // ADC phase
    MD_FLAG_POR_HPVN,
    },
#endif // #if ENABLE_RGB_SUPPORT_85HZ

    // 640x350 60Hz ()  --> 1
    {
    RES_640X350, // resolution index, use 640x400 resolution
    314, 592,     // HFreq, VFreq
    80, 3,  // HStart, VStart
    800, 369,     // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,         // ADC phase
    MD_FLAG_POR_HNVP,
    },

    // 720x400 70Hz (VGA)   --> 2
    {//MD_720x400_70    925FS-40    2
    RES_720X400, // resolution index
    315, 700,       // HFreq, VFreq
    158, 34,         // HStart, VStart
    //155, 33,         // HStart, VStart
    900, 449,       // HTotal, VTotal
    5,           // VTotalTorance
    0x2B,      // ADC phase
    MD_FLAG_POR_HNVP | MD_FLAG_POR_HNVN | MD_FLAG_POR_HPVP | MD_FLAG_POR_HPVN,
    },

#if ENABLE_RGB_SUPPORT_85HZ
    // 640x400 85Hz (VESA)--->
    {
    RES_640X400, // resolution index
    379, 851,     // HFreq, VFreq
    160, 44,      // HStart, VStart
    832, 445,     // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,         // ADC phase
    MD_FLAG_POR_HNVP,
    },
#endif //#if ENABLE_RGB_SUPPORT_85HZ

    // 640x400 70Hz (IBM VGA)   -->3
    {
    RES_640X400, // resolution index
    315, 700,     // HFreq, VFreq
    156, 33,       // HStart, VStart
    800, 449,     // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,      // ADC phase
    MD_FLAG_POR_HNVP,
    },

#if ENABLE_RGB_SUPPORT_85HZ
    // 720x400 85Hz (VESA)--->
    {
    RES_720X400, // resolution index
    379, 850,     // HFreq, VFreq
    180, 45,      // HStart, VStart
    936, 446,     // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,         // ADC phase
    MD_FLAG_POR_HNVP,
    },
#endif //#if ENABLE_RGB_SUPPORT_85HZ

    {//MD_640x480_60    925FS-35    //--->4
    RES_640X480, // resolution index
    315, 599,     // HFreq, VFreq
    141, 32,      // HStart, VStart
    800, 525,     // HTotal, VTotal
    5,           // VTotalTorance
    0x25,         // ADC phase
    MD_FLAG_POR_HNVN | MD_FLAG_HDTV_BIT,
    },

    {//MD_640x480_66    925FS-36    //--->5
    RES_640X480, // resolution index
    350, 667,     // HFreq, VFreq
    156, 38,      // HStart, VStart
    864, 525,     // HTotal, VTotal
    5,           // VTotalTorance
    0x3C,         // ADC phase
    MD_FLAG_POR_HNVN,
    },

    {//MD_640x480_72    925FS-37   //--->6
    RES_640X480, // resolution index
    379, 728,     // HFreq, VFreq
    164, 27,      // HStart, VStart
    832, 520,     // HTotal, VTotal
    5,           // VTotalTorance
    0x3A,         // ADC phase
    MD_FLAG_POR_HNVN,
    },

    // 640x480 75Hz (VESA)  --> 7
    {//MD_640x480_75    925FS-38    7
    RES_640X480, // resolution index
    375, 750,     // HFreq, VFreq
    180, 15,      // HStart, VStart
    840, 500,     // HTotal, VTotal
    5,           // VTotalTorance
    0x39,         // ADC phase
    MD_FLAG_POR_HNVN,
    },

#if ENABLE_RGB_SUPPORT_85HZ
    // 640x480 85Hz (VESA)  -->
    {
    RES_640X480, // resolution index
    433, 850,     // HFreq, VFreq
    136, 28,      // HStart, VStart
    832, 509,     // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,         // ADC phase
    MD_FLAG_POR_HNVN,
    },
#endif // #if ENABLE_RGB_SUPPORT_85HZ

    // 800x600 56Hz (VESA)  --> 8
    {//MD_800x600_56    925FS-44    8
        //              925FS-45(À¯»ç)
    RES_800X600, // resolution index
    352, 562,     // HFreq, VFreq
    198, 21,      // HStart, VStart
    1024, 625,   // HTotal, VTotal
    5,           // VTotalTorance
    0x04,         // ADC phase
    MD_FLAG_POR_HPVP,
    },

    // 800x600 60Hz (VESA)  --> 9
    {//MD_800x600_60    925FS-46    9
    RES_800X600, // resolution index
    379, 603,     // HFreq, VFreq
    213, 22,      // HStart, VStart
    1056, 628,   // HTotal, VTotal
    5,           // VTotalTorance
    0x30,         // ADC phase
    MD_FLAG_POR_HPVP,
    },

    // 800x600 72Hz (VESA)  --> 10
    {//MD_800x600_72    925FS-47    10
    RES_800X600, // resolution index
    481, 722,     // HFreq, VFreq
    182, 22,      // HStart, VStart
    1040, 666,   // HTotal, VTotal
    5,           // VTotalTorance
    0x06,         // ADC phase
    MD_FLAG_POR_HPVP,
    },

    // 800x600 75Hz (VESA)  --> 11
    {//MD_800x600_75    925FS-48    11
    RES_800X600, // resolution index
    469, 750,     // HFreq, VFreq
    237, 20,      // HStart, VStart
    1056, 625,   // HTotal, VTotal
    5,           // VTotalTorance
    0x2A,         // ADC phase
    MD_FLAG_POR_HPVP,
    },

#if ENABLE_RGB_SUPPORT_85HZ
    // 800x600 85Hz (VESA)  -->
    {
    RES_800X600, // resolution index
    537, 851,     // HFreq, VFreq
    216, 30,      // HStart, VStart
    1048, 631,   // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,         // ADC phase
    MD_FLAG_POR_HPVP,
    },
#endif //#if ENABLE_RGB_SUPPORT_85HZ

    // 832x624 75Hz (MAC)  --> 12
    {//MD_832x624_75    925FS-53    12
    RES_832X624, // resolution index
    497, 746,     // HFreq, VFreq
    283, 36,      // HStart, VStart
    1152, 667,   // HTotal, VTotal
    5,           // VTotalTorance
    0x2C,         // ADC phase
    MD_FLAG_POR_HNVN,//MD_FLAG_POR_HPVP,
    },

    // 848x480 59Hz ()  --> 13
    {//MD_848x480_59    925FS-55    13
    RES_848X480, // resolution index
    298, 596,     // HFreq, VFreq
    180, 11,      // HStart, VStart
    1056, 500,    // HTotal, VTotal
    5,           // VTotalTorance
    0x27,         // ADC phase
    MD_FLAG_POR_HNVP, //flags
    },

#if ENABLE_RGB_SUPPORT_85HZ
    // 1024x768 43i (VESA)  -->
    {
    RES_1024X768, // resolution index
    355, 868,      // HFreq, VFreq
    232, 24,       // HStart, VStart
    1264, 817,    // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_INTERLACE, // flags
    },
#endif // #if ENABLE_RGB_SUPPORT_85HZ

    // 1024x768 60Hz (VESA)  --> 14
    {//MD_1024x768_60   925FS_60    14
    RES_1024X768, // resolution index
    484, 600,      // HFreq, VFreq
    292, 29,      // HStart, VStart
    1344, 806,    // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_POR_HNVN,
    },

    // 1024x768 70Hz (VESA)  --> 15
    {//MD_1024x768_70   925FS_62    15
    RES_1024X768, // resolution index
    565, 700,      // HFreq, VFreq
    275, 28,       // HStart, VStart
    1328, 806,    // HTotal, VTotal
    5,           // VTotalTorance
    0x00,          // ADC phase
    MD_FLAG_POR_HNVN,
    },

    // 1024x768 75Hz (VESA)  --> 16
    {//MD_1024x768_75   925FS_65    16
    RES_1024X768, // resolution index
    600, 750,      // HFreq, VFreq
    269, 27,       // HStart, VStart
    1312, 800,    // HTotal, VTotal
    5,           // VTotalTorance
    0x29,          // ADC phase
    MD_FLAG_POR_HPVP,
    },

#if ENABLE_RGB_SUPPORT_85HZ
    // 1024x768 85Hz (VESA)  -->
    {
    RES_1024X768, // resolution index
    687, 850,      // HFreq, VFreq
    304, 39,       // HStart, VStart
    1376, 808,    // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_POR_HPVP,
    },
#endif // #if ENABLE_RGB_SUPPORT_85HZ

  // 1152x864 60Hz (VESA)  --> 17
    {//MD_1152x864_60   925FS_71    17
    RES_1152X864,  // resolution index
    543, 600,      // HFreq, VFreq
    //540, 600,      // HFreq, VFreq
    285, 36,       // HStart, VStart
    1472, 905,    // HTotal, VTotal
    //1480, 900,    // HTotal, VTotal
    5,             // VTotalTorance
    0x33,          // ADC phase
    MD_FLAG_POR_HPVP,
    },

    // 1152x864 70Hz (VESA)  --> 18
    {//18
    RES_1152X864, // resolution index
    639, 702,      // HFreq, VFreq
    288, 49,       // HStart, VStart
    1472, 912,    // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_POR_HPVP,
    },

    // 1152x864 75Hz (VESA)  --> 19
    {//MD_1152x864_75   925FS_72    19
    RES_1152X864, // resolution index
    675, 750,      // HFreq, VFreq
    381, 31,       // HStart, VStart
    1600, 900,    // HTotal, VTotal
    5,           // VTotalTorance
    0x11,          // ADC phase
    MD_FLAG_POR_HPVP,
    },

#if ENABLE_RGB_SUPPORT_85HZ
    // 1152x864 85Hz (VESA)  -->
    {
    RES_1152X864, // resolution index
    771, 850,      // HFreq, VFreq
    360, 42,       // HStart, VStart
    1552, 910,    // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_POR_HNVP,
    },
#endif // #if ENABLE_RGB_SUPPORT_85HZ

    // 1152x870 75Hz (MAC)  --> 20
    {//MD_1152x870_75   925FS_74    20
    RES_1152X870, // resolution index
    689, 751,      // HFreq, VFreq
    265, 38,       // HStart, VStart
    1456, 916,    // HTotal, VTotal
    5,           // VTotalTorance
    0x1B,          // ADC phase
    MD_FLAG_POR_HNVN,
    },

    // 1280x768 60Hz (VESA-GTF)  --> 21
    {// MSPG1025D_85                 21
    RES_1280X768, // resolution index
    477, 598,      // HFreq, VFreq
    314, 19,       // HStart, VStart
    1664, 798,    // HTotal, VTotal
    5,           // VTotalTorance
    0x26,          // ADC phase
    MD_FLAG_POR_HNVP,
    },

    // 1280x768 75Hz (VESA-GTF)  --> 22
    {//MSPG1025D_89                  22
    RES_1280X768, // resolution index
    602, 748,      // HFreq, VFreq
    329, 26,       // HStart, VStart
    1696, 805,    // HTotal, VTotal
    5,           // VTotalTorance
    0x13,          // ADC phase
    MD_FLAG_POR_HNVP,
    },

    // 1280x960 60Hz (VESA)  --> 23
    {// MSPG1025D-94             23
    RES_1280X960, // resolution index
    600, 600,      // HFreq, VFreq
    421, 35,       // HStart, VStart
    1800, 1000,   // HTotal, VTotal
    5,           // VTotalTorance
    0x11,          // ADC phase
    MD_FLAG_POR_HPVP,
    },

    // 1280x960 75Hz (VESA)  --> 24
    {// MSPG1025D-95             24
    RES_1280X960, // resolution index
    750, 750,      // HFreq, VFreq
    381, 35,       // HStart, VStart
    1728, 1002,   // HTotal, VTotal
    5,           // VTotalTorance
    0x11,          // ADC phase
    MD_FLAG_POR_HPVP,
    },

#if ENABLE_RGB_SUPPORT_85HZ
    // 1280x960 85Hz (VESA)  -->
    {
    RES_1280X960, // resolution index
    859, 850,      // HFreq, VFreq
    384, 50,       // HStart, VStart
    1728, 1011,   // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_POR_HPVP,
    },
#endif // #if ENABLE_RGB_SUPPORT_85HZ

    // 1280x1024 60Hz (VESA)  --> 25
    {// MSPG1025D-99              25
    RES_1280X1024, // resolution index
    639, 600,       // HFreq, VFreq
    357, 37,        // HStart, VStart
    1688, 1066, // HTotal, VTotal
    5,           // VTotalTorance
    0x22,           // ADC phase
    MD_FLAG_POR_HPVP,
    },

    // 1280x1024 59Hz (VESA)  --> 26
    {// MSPG1025D-98              26
    RES_1280X1024, // resolution index
    633, 599,      // HFreq, VFreq
    345, 28,       // HStart, VStart
    1712, 1063,    // HTotal, VTotal
    5,           // VTotalTorance
    0x02,          // ADC phase
    MD_FLAG_POR_HNVN,
    },

    // 1280x1024 72Hz (VESA)  --> 27
    {// MSPG1025D-101             27
    RES_1280X1024, // resolution index
    760, 720,       // HFreq, VFreq
    375, 54,        // HStart, VStart
    1712, 1064, // HTotal, VTotal
    5,           // VTotalTorance
    0x3A,           // ADC phase
    MD_FLAG_POR_HPVP,
    },

    // 1280x1024 75Hz (VESA)  --> 28
    {// MSPG1025D-102             28
    RES_1280X1024, // resolution index
    800, 750,       // HFreq, VFreq
    389, 37,        // HStart, VStart
    1688, 1066, // HTotal, VTotal
    5,           // VTotalTorance
    0x02,           // ADC phase
    MD_FLAG_POR_HPVP,
    },

    // 1280x1024 76Hz (VESA)  --> 29
    {// MSPG1025D-103             29
    RES_1280X1024, // resolution index
    811, 760,       // HFreq, VFreq
    343, 31,        // HStart, VStart
    1664, 1066, // HTotal, VTotal
    5,           // VTotalTorance
    0x3A,           // ADC phase
    MD_FLAG_POR_HNVN,
    },

#if ENABLE_RGB_SUPPORT_85HZ
    // 1280x1024 85Hz (VESA)  -->
    {
    RES_1280X1024, // resolution index
    911, 850,       // HFreq, VFreq
    384, 47,        // HStart, VStart
    1728, 1072, // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,           // ADC phase
    MD_FLAG_POR_HPVP,
    },
#endif //#if ENABLE_RGB_SUPPORT_85HZ

    // 1600x1200 59Hz (R.B)  --> 30
    {// MSPG1025D-117            30
    RES_1600X1200, // resolution index
    741, 599,       // HFreq, VFreq
    146, 20,        // HStart, VStart
    1760, 1236, // HTotal, VTotal
    5,           // VTotalTorance
    0x11,           // ADC phase
    MD_FLAG_POR_HPVN,
    },

    // 1600x1200 60Hz (VESA)  --> 31
    {// MSPG1025D-120             31
    RES_1600X1200, // resolution index
    750, 600,       // HFreq, VFreq
    492, 45,        // HStart, VStart
    2160, 1250, // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,           // ADC phase
    MD_FLAG_POR_HPVP,
    },

    // 1600x1200 65Hz (VESA)  --> 32
    {//32
    RES_1600X1200, // resolution index
    813, 650,       // HFreq, VFreq
    496, 49,        // HStart, VStart
    2160, 1250, // HTotal, VTotal
    5,           // VTotalTorance
    0x39,           // ADC phase
    MD_FLAG_POR_HPVP,
    },

    // 1600x1200 70Hz (VESA)  --> 33
    {//33
    RES_1600X1200, // resolution index
    875, 700,       // HFreq, VFreq
    496, 49,        // HStart, VStart
    2160, 1250, // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,           // ADC phase
    MD_FLAG_POR_HPVP,
    },

    // 1600x1200 75Hz (VESA)  --> 34
    {// MSPG1025D-121             34
    RES_1600X1200, // resolution index
    938, 750,       // HFreq, VFreq
    493, 45,        // HStart, VStart
    2160, 1250, // HTotal, VTotal
    5,           // VTotalTorance
    0x11,           // ADC phase
    MD_FLAG_POR_HPVP,
    },

    // 1920x1200 59Hz (R.B)  --> 35
    {// 35
    RES_1920X1200,  // resolution index
    740, 599,       // HFreq, VFreq
    110, 30,        // HStart, VStart
    2080, 1235,     // HTotal, VTotal
    5,              // VTotalTorance
    0x3f,           // ADC phase
    MD_FLAG_POR_HPVN,
    },

    // 1920x1200 60Hz ()  --> 36
    {// MSPG1025D-128         36
    RES_1920X1200, // resolution index
    745, 599,       // HFreq, VFreq
    500, 40,        // HStart, VStart
    2592, 1245, // HTotal, VTotal
    5,           // VTotalTorance
    0x31,           // ADC phase
    MD_FLAG_POR_HNVN,
    },

    // 1920x1200 65Hz ()  --> 37
    {//37
    RES_1920X1200, // resolution index
    812, 650,       // HFreq, VFreq
    496, 49,        // HStart, VStart
    2592, 1248, // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,           // ADC phase
    MD_FLAG_POR_HNVP,
    },

    // 1920x1200 75Hz ()  --> 38
    {//38
    RES_1920X1200, // resolution index
    940, 749,       // HFreq, VFreq
    344, 49,        // HStart, VStart
    2608, 1255, // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,           // ADC phase
    MD_FLAG_POR_HNVP,
    },

#if 0
    // 1400x1050 59Hz (R.B)  --> 48
    {//39
    RES_1400X1050, // resolution index
    647, 599,      // HFreq, VFreq
    110, 26,       // HStart, VStart
    1560, 1080, // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_POR_HPVN,// | MD_FLAG_POR_HNVP,
    },
#endif
    // 1400x1050 59Hz (R.B)  --> 39
    {//39
    RES_1400X1050, // resolution index
    647, 599,      // HFreq, VFreq
    232, 32,       // HStart, VStart
    1560, 1080, // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_POR_HPVN | MD_FLAG_POR_HNVP,
    },

#if 0
    // 1400x1050 60Hz ()  --> 49
    {//40
    RES_1400X1050, // resolution index
    653, 599,      // HFreq, VFreq
    332, 35,       // HStart, VStart
    1864, 1089, // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_POR_HNVP,
    },
#endif
    // 1400x1050 60Hz ()  --> 40
    {//40
    RES_1400X1050, // resolution index
    653, 599,      // HFreq, VFreq
    232, 32,       // HStart, VStart
    1864, 1088, // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_POR_HNVP,
    },

    // 1400x1050 75Hz ()  --> 41
    {// MSPG1025D-111         41
    RES_1400X1050, // resolution index
    823, 748,      // HFreq, VFreq
    382, 41,       // HStart, VStart
    1896, 1099, // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_POR_HNVN,
    },

    // 1440x900 60Hz (RB)  --> 42
    {// MSPG1025D-113        42
    RES_1440X900, // resolution index
    554, 599,    //559, 598,      // HFreq, VFreq
    110, 23,       // HStart, VStart
    1600, 926,  //1904, 934,    // HTotal, VTotal
    5,           // VTotalTorance
    0x0E,          // ADC phase
    MD_FLAG_POR_HPVN, //MD_FLAG_POR_HNVP,
    },

    // 1440x900 75Hz ()  --> 43
    {//43
    RES_1440X900, // resolution index
    706, 749,      // HFreq, VFreq
    248, 33,       // HStart, VStart
    1936, 942,    // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_POR_HNVP,
    },
#if 0
    // 1680x1050 59Hz (R.B)  --> 53
    {//44
    RES_1680X1050, // resolution index
    647, 599,      // HFreq, VFreq
    112, 25,       // HStart, VStart
    1840, 1080, // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_POR_HPVN | MD_FLAG_POR_HNVP,
    },

#endif

    // 1680x1050 59Hz (R.B)  --> 44
    {//44
    RES_1680X1050, // resolution index
    646, 598,      // HFreq, VFreq
    108, 19,       // HStart, VStart    //may_20070614 adjust default position
    //280, 30,       // HStart, VStart
    1840, 1080, // HTotal, VTotal
    5,           // VTotalTorance
    32,          // ADC phase            //may_20070614 adjust default position
    //0x3f,          // ADC phase
    MD_FLAG_POR_HPVN | MD_FLAG_POR_HNVP,
    },
#if 0
    // 1680x1050 60Hz ()  --> 54
    {//45
    RES_1680X1050, // resolution index
    653, 599,      // HFreq, VFreq
    280, 30,       // HStart, VStart
    2240, 1089, // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_POR_HNVP,
    },
#endif

    // 1680x1050 60Hz ()  --> 45
    {//45
    RES_1680X1050, // resolution index
    652, 599,      // HFreq, VFreq
    450, 28,       // HStart, VStart    //may_20070614 adjust default position
    //280, 30,       // HStart, VStart
    2240, 1089, // HTotal, VTotal
    5,           // VTotalTorance
    6,          // ADC phase            //may_20070614 adjust default position
    //0x3f,          // ADC phase
    MD_FLAG_POR_HNVP,
    },

    // 1680x1050 75Hz ()  --> 46
    {//46
    RES_1680X1050, // resolution index
    823, 748,      // HFreq, VFreq
    296, 40,       // HStart, VStart
    2272, 1099, // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_POR_HNVN,
    },

    // 720x480I 60Hz (YPbPr)  --> 47  Start of YPbPr
    {//47
    RES_720X480, // resolution index
    156, 600,     // HFreq, VFreq
    54, 24,      // HStart, VStart
    858, 525,     // HTotal, VTotal
    5,           // VTotalTorance
    0x3C,         // ADC phase
    MD_FLAG_YPBPR_BIT | MD_FLAG_INTERLACE, // flags
    },

    // 720x480P 60Hz (YPbPr)  --> 48
    {//48
    RES_720X480,  // resolution index
    315, 600,     // HFreq, VFreq
    57, 28,       // HStart, VStart
    858, 525,     // HTotal, VTotal
    5,            // VTotalTorance
    0x2E,         // ADC phase
    MD_FLAG_YPBPR_BIT, // flags
    },

    // 720x576I 50Hz (YPbPr)  --> 49
    {//49
    RES_720X576,  // resolution index
    156, 500,     // HFreq, VFreq
    67, 32,       // HStart, VStart
    864, 625,     // HTotal, VTotal
    5,            // VTotalTorance
    0x3B,         // ADC phase
    MD_FLAG_YPBPR_BIT | MD_FLAG_INTERLACE, // flags
    },

    // 720x576P 50Hz (YPbPr)  --> 50
    {//50
    RES_720X576, // resolution index
    312, 500,     // HFreq, VFreq
    66, 35,     // HStart, VStart
    864, 625,     // HTotal, VTotal
    5,           // VTotalTorance
    0x3B,         // ADC phase
    MD_FLAG_YPBPR_BIT, // flags
    },

    // 1280x720P 50Hz (YPbPr)  --> 51
    {//51
    RES_1280X720, // resolution index
    375, 500,      // HFreq, VFreq
    0x102, 0x10,       // HStart, VStart
    //0x10B, 0x11,       // HStart, VStart
    1980, 750,    // HTotal, VTotal
    5,           // VTotalTorance
    0x05,        // ADC phase
    MD_FLAG_YPBPR_BIT,// | MD_FLAG_HDTV_BIT, // flags
    },

    // 1280x720P 60Hz (YPbPr)  --> 52
    {//52
    RES_1280X720, // resolution index
    450, 600,      // HFreq, VFreq
    0x101, 0x10,       // HStart, VStart
    1650, 750,    // HTotal, VTotal
    5,           // VTotalTorance
    0x05,        // ADC phase
    MD_FLAG_YPBPR_BIT,// | MD_FLAG_HDTV_BIT , // flags
    },

    // 1920x1080i 50Hz (YPbPr)  --> 53
    {//53
    RES_1920X1080, // resolution index
    281, 500,       // HFreq, VFreq
    0xbd, 0x17,     // HStart, VStart
    //0xC7, 0x1B,     // HStart, VStart
    2640, 1125, // HTotal, VTotal (patch)
    5,           // VTotalTorance
    0x06,         // ADC phase
    MD_FLAG_YPBPR_BIT  | MD_FLAG_INTERLACE, // flags
    },

    // 1920x1080i 60Hz (YPbPr)  --> 54
    {//54
    RES_1920X1080, // resolution index
    337, 600,       // HFreq, VFreq
    0xbd, 0x16,     // HStart, VStart
    //0xC7, 0x1B,     // HStart, VStart
    2200, 1125, // HTotal, VTotal (patch)
    5,           // VTotalTorance
    0x05,         // ADC phase
    MD_FLAG_YPBPR_BIT  | MD_FLAG_INTERLACE, // flags
    },

#if (SUPPORT_EURO_HDTV)
    // 1920x1080i 50Hz (YPbPr)  --> 55
    {//MODE_NAME("1080i50_Euro")
     RES_1920X1080, // resolution index
     313, 500,      // HFreq, VFreq
     304, 152,      // HStart, VStart
     2376, 1250,    // HTotal, VTotal
     5,           // VTotalTorance
     0x1f,          // ADC phase
     MD_FLAG_YPBPR_BIT  | MD_FLAG_INTERLACE | MD_FLAG_EURO_HDTV_BIT, // flags
    },
#endif

    // 1920x1080P 24Hz (YPbPr)  -->56
    {//55
    RES_1920X1080P, // resolution index
    270, 240,       // HFreq, VFreq
    0xbc, 0x21,     // HStart, VStart
    2750, 1125, // HTotal, VTotal (patch) 135Hz
    5,           // VTotalTorance
    0x21,           // ADC phase
    MD_FLAG_YPBPR_BIT , // flags
    },

    // 1920x1080P 25Hz (YPbPr)  -->57
    {//56
    RES_1920X1080P, // resolution index
    281, 250,       // HFreq, VFreq
    0xbc, 0x21,     // HStart, VStart
    2640, 1125, // HTotal, VTotal (patch) 135Hz
    5,           // VTotalTorance
    0x21,           // ADC phase
    MD_FLAG_YPBPR_BIT , // flags
    },

    // 1920x1080P 30Hz (YPbPr)  -->58
    {//56
    RES_1920X1080P, // resolution index
    337, 300,       // HFreq, VFreq
    0xbc, 0x20,     // HStart, VStart
    //0xD0, 0x20,     // HStart, VStart
    2200, 1125, // HTotal, VTotal (patch)  135
    5,           // VTotalTorance
    0x21,           // ADC phase
    MD_FLAG_YPBPR_BIT , // flags
    },

    // 1920x1080P 50Hz (YPbPr)  --> 59
    {//57
    RES_1920X1080P, // resolution index
    562, 500,       // HFreq, VFreq
    0xbc, 0x20,     // HStart, VStart
    2640, 1125, // HTotal, VTotal (patch) 148MHz
    5,           // VTotalTorance
    0x17,           // ADC phase
    MD_FLAG_YPBPR_BIT , // flags
    },

    // 1920x1080P 60Hz (YPbPr)  --> 60
    {//58
    RES_1920X1080P, // resolution index
    675, 600,       // HFreq, VFreq
    0xbc, 0x20,     // HStart, VStart
    2200, 1125, // HTotal, VTotal (patch)  148MHz
    5,           // VTotalTorance
    0x12,           // ADC phase
    MD_FLAG_YPBPR_BIT , // flags
    },

    // 720x480I 60Hz (YPbPr)  --> 61  End of YPbPr
    {//59, patch for DVD 480i -> Sherwood -> component output -> OSD unsupport mode
    RES_720X480, // resolution index
    156, 300,     // HFreq, VFreq
    0x3b, 0x1a,   // HStart, VStart    //may_070401_CompPosAdj
    //0x41, 0x1E,   // HStart, VStart
    858, 525,     // HTotal, VTotal
    5,           // VTotalTorance
    0x1f,         // ADC phase
    MD_FLAG_YPBPR_BIT, // flags
    },

    // 1152x870 60Hz    --> 62
    {//59
    RES_1152X870,   // resolution index
    541, 599,       // HFreq, VFreq
    304, 25,        // HStart, VStart
    1520, 903,  // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,           // ADC phase
    MD_FLAG_POR_HNVP,
    },

    // 1280x800 60Hz    --> 63
    {//60
    RES_1280X800,   // resolution index
    495, 598,       // HFreq, VFreq
    323, 20,        // HStart, VStart
    1680, 828,  // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,           // ADC phase
    MD_FLAG_POR_HNVP,
    },

    // 1280x800 75Hz    --> 64
    {//61
    RES_1280X800,   // resolution index
    625, 749,       // HFreq, VFreq
    335, 27,        // HStart, VStart
    1712, 835,  // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,           // ADC phase
    MD_FLAG_POR_HNVP,
    },

    // 1600x1024 60Hz   --> 65
    {// MSPG1025D-114       62
    RES_1600X1024,  // resolution index
    621, 597,       // HFreq, VFreq
    //637, 601,       // HFreq, VFreq
    154, 20,        // HStart, VStart
    1808, 1040,  // HTotal, VTotal
    //2144, 1060,  // HTotal, VTotal
    5,           // VTotalTorance
    0x38,           // ADC phase
    MD_FLAG_POR_HNVN,
    //MD_FLAG_POR_HNVP,
    },

    // 1600x1024 75Hz   --> 66
    {//63
    RES_1600X1024,  // resolution index
    802, 750,       // HFreq, VFreq
    154, 20,        // HStart, VStart
    2176, 1069,  // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,           // ADC phase
    MD_FLAG_POR_HNVP,
    },

    // 1600x900 60Hz ()  --> 67
    {//64
    RES_1600X900, // resolution index
    557, 598,      // HFreq, VFreq
    232, 25,       // HStart, VStart
    2128, 932,    // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_POR_HNVP,
    },

    // 1600x900 75Hz ()  --> 68
    {//65
    RES_1600X900, // resolution index
    702, 747,      // HFreq, VFreq
    248, 33,       // HStart, VStart
    2160, 940,    // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,          // ADC phase
    MD_FLAG_POR_HNVP,
    },

    // 1280x720 75Hz ()  --> 69
    {// MSPG1025D-81         66
    RES_1280X720, // resolution index
    564, 750,     // HFreq, VFreq
    329, 26,      // HStart, VStart
    1696, 752,   // HTotal, VTotal
    5,           // VTotalTorance
    0x2d,         // ADC phase
    MD_FLAG_POR_HNVP,
    },

    // 640x480 57Hz ()  --> 70
    {//67
    RES_640X480, // resolution index
    300, 571,     // HFreq, VFreq
    138, 31,      // HStart, VStart
    800, 525,     // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,         // ADC phase
    MD_FLAG_POR_HNVN,
    },

    // 1360x768 60Hz ()  --> 71
    {// MSPG1025D_109        68
    RES_1360X768, // resolution index
    477, 600,     // HFreq, VFreq
    365, 17,      // HStart, VStart
    1792, 795,    // HTotal, VTotal
    5,           // VTotalTorance
    0x22,         // ADC phase
    MD_FLAG_POR_HPVP,
    },

    // 1600x1200 51Hz ()  --> 72
    {//69
    RES_1600X1200, // resolution index
    648, 518,       // HFreq, VFreq
    496, 46,        // HStart, VStart
    2160, 1250, // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,           // ADC phase
    MD_FLAG_POR_HPVP, //flags
    },

    // 848x480 60Hz ()  --> 73
    {//70
    RES_848X480, // resolution index
    310, 600,     // HFreq, VFreq
    139, 31,      // HStart, VStart
    1088, 517,    // HTotal, VTotal
    5,           // VTotalTorance
    0x12,         // ADC phase
    MD_FLAG_POR_HPVP, //flags
    },

    // 1280x768 50Hz (925FS 83)  --> 74
    {//MD_1280x768_50   //71
    RES_1280X768, // resolution index
    395, 500,      // HFreq, VFreq
    307, 14,       // HStart, VStart
    1648, 791,    // HTotal, VTotal
    5,           // VTotalTorance
    0x13,          // ADC phase
    MD_FLAG_POR_HPVP,
    },

    // 1366x768 60Hz ()  --> 75
    {//MD_1366x768_60   925FS_106, MSPG1025D_115   72
    RES_1366X768, // resolution index
    491, 600,     // HFreq, VFreq
    346, 32,      // HStart, VStart
    1780, 817,    // HTotal, VTotal
    5,           // VTotalTorance
    0x2f,         // ADC phase
    MD_FLAG_POR_HNVN,
    },

    // 720x576 60Hz ()  --> 76
    {//73    //Radeon 9200
    RES_720X576, // resolution index
    357, 598,     // HFreq, VFreq
    0xA3, 0x0F,   // HStart, VStart
    912, 597,     // HTotal, VTotal
    5,           // VTotalTorance
    0x1f,         // ADC phase
    MD_FLAG_POR_HNVP,
    },

    // 864x648 60Hz ()  --> 77
    {//74    //Radeon X850 XT Platinum Ed
    RES_864X648, // resolution index
    403, 600,     // HFreq, VFreq
    212, 17,      // HStart, VStart
    1120, 671,   // HTotal, VTotal
    5,           // VTotalTorance
    0x3f,         // ADC phase
    MD_FLAG_POR_HNVP,
    },

    // 1920x1080 60Hz ()  --> 78
    {//75        //925FS_126
    RES_1920X1080, // resolution index
    667, 600,       // HFreq, VFreq
    109, 21,     // HStart, VStart
    2080, 1111, // HTotal, VTotal (patch)  148MHz
    5,           // VTotalTorance
    0x12,           // ADC phase
    MD_FLAG_POR_HPVN, // flags
    },

    // 720x480 60Hz ()  --> 79
    {//76        //925FS_126
    RES_720X480, // resolution index
    351, 598,     // HFreq, VFreq
    0xA3, 0x0F,   // HStart, VStart
    891, 587,     // HTotal, VTotal
    5,           // VTotalTorance
    0x1f,         // ADC phase
    MD_FLAG_POR_HNVN,
    },

    // 1440x900 60Hz () --> 80
    {//77
    RES_1440X900, // resolution index
    559, 598,    //559, 598,      // HFreq, VFreq
    381, 24,       // HStart, VStart
    1904, 934,  //1904, 934,    // HTotal, VTotal
    5,           // VTotalTorance
    0x0E,          // ADC phase
    MD_FLAG_POR_HNVP, //MD_FLAG_POR_HNVP,
    },

    // 1280x768 60Hz RB (VESA)--> 81
    {// 78
    RES_1280X768, // resolution index
    474, 600,      // HFreq, VFreq
    110, 19,       // HStart, VStart
    1440, 790,    // HTotal, VTotal
    5,           // VTotalTorance
    0x26,          // ADC phase
    MD_FLAG_POR_HPVN,
    },

    // 1280x720P 25Hz (YPbPr)  --> 82
    {//79
    RES_1280X720, // resolution index
    184, 250,      // HFreq, VFreq
    0xC1, 5,       // HStart, VStart
    1600, 734,    // HTotal, VTotal
    5,           // VTotalTorance
    0x05,        // ADC phase
    MD_FLAG_YPBPR_BIT  , // flags
    },

    // 1280x720P 30Hz (YPbPr)  --> 83
    {//80
    RES_1280X720, // resolution index
    221, 300,      // HFreq, VFreq
    0xC2, 5,       // HStart, VStart
    1600, 736,    // HTotal, VTotal
    5,           // VTotalTorance
    0x05,        // ADC phase
    MD_FLAG_YPBPR_BIT , // flags
    },

    // 1920X1080 60Hz --> 84
    {//81        //925FS_126
       RES_1920X1080, // resolution index
       671, 600,       // HFreq, VFreq
       528, 37,     // HStart, VStart
       2576, 1120, // HTotal, VTotal (patch)  148MHz
       5,           // VTotalTorance
       0x12,           // ADC phase
       MD_FLAG_POR_HNVP, // flags
   },

   // 1920X1080 50Hz --> 85
   {//82
       RES_1920X1080, // resolution index
       556, 500,       // HFreq, VFreq
       512, 31,     // HStart, VStart
       2544, 1114, // HTotal, VTotal (patch) 148MHz
       5,           // VTotalTorance
       0x17,           // ADC phase
       MD_FLAG_POR_HNVP, // flags
   },
};

//=============================================================================
// Local Function Prototypes
//=============================================================================

//------------------------------------------------------------------------------
//  Internal function
//------------------------------------------------------------------------------
static void _PCMode_Init(void)
{
    static MS_BOOL _bInitOnce = FALSE;
    MS_U8  u8Idx;

    if (!_bInitOnce)
    {
        for(u8Idx=0; u8Idx<MAX_WINDOW; ++u8Idx)
        {
            previous_status[u8Idx] = E_XC_PCMONITOR_STABLE_NOSYNC;
            Enable_SelfAuto[u8Idx] = DISABLE;
            MApi_PCMode_Reset((SCALER_WIN)u8Idx);
        }

        _bInitOnce = TRUE;
    }

    //![default: startdard mode table db]
    _pstTimingModeDB   = &astStandardModeTable[0];
    _u8NumOfModeDBItem = MD_STD_MODE_MAX_INDEX;

}

//------------------------------------------------------------------------
void MApi_XC_Sys_SetSrcIsHD(MS_BOOL bIsHd, SCALER_WIN eWindow)
{
    bSrcIsHD[eWindow] = bIsHd;
}

//------------------------------------------------------------------------

MS_BOOL MApi_XC_Sys_IsSrcHD(SCALER_WIN eWindow)
{
    return bSrcIsHD[eWindow];
}


//--------------------------------------------------------------------------

static void  _MApi_CheckSupportTiming(SCALER_WIN eWindow)
{
   MS_U8 u8SyncStatus;
   MS_U16 u16InputVFreq;// u16HSize, u16VSize,
   if (IsHDMIInUse())
    {
         u8SyncStatus = PCMODE_INFO[eWindow].sModeParseInputInfo.u8SyncStatus;
         u16InputVFreq = PCMODE_INFO[eWindow].sModeParseInputInfo.u16VFreqx10;
         //u16HSize = PCMODE_INFO[eWindow].sModeParseInputInfo.sDVI_HDMI_DE_Info.width;
         //u16VSize = PCMODE_INFO[eWindow].sModeParseInputInfo.sDVI_HDMI_DE_Info.height;
         if (XC_MODEPARSE_SUPPORT_MODE == PCMODE_INFO[eWindow].eModeParseResult)
         {
            if ((u8SyncStatus & XC_MD_INTERLACE_BIT)&&(u16InputVFreq >=620) )
            {
                PCMODE_INFO[eWindow].eModeParseResult = XC_MODEPARSE_UNSUPPORT_MODE;
                PCMSG(printf(" !! > 60Hz \n"));
            }
        }
    }
}

//--------------------------------------------------------------------------

static MS_BOOL _MApi_PCMode_Check_OOR(SCALER_WIN eWindow, XC_MODEPARSE_INPUT_INFO *psInputInfo)
{
    MS_U16 u16MinHfreq, u16MaxHfreq;
    MS_U16 u16MinVfreq, u16MaxVfreq;
    MS_U16 u16MinVtotal, u16MaxVtotal;

    if(IsSrcTypeVga(psInputInfo->eSrcType))
    {
        u16MinHfreq    = MIN_HFREQ_OF_RGB;
        u16MaxHfreq    = MAX_HFREQ_OF_RGB;
        u16MinVfreq    = MIN_VFREQ_OF_RGB;
        u16MaxVfreq    = MAX_VFREQ_OF_RGB;
        u16MinVtotal   = MIN_VTOTAL_OF_RGB;
        u16MaxVtotal   = MAX_VTOTAL_OF_RGB;
    }
#if (INPUT_HDMI_VIDEO_COUNT > 0)
    else if( MApi_XC_HDMI_GetHdmiType(SYS_INPUT_SOURCE_TYPE(eWindow))==E_HDMI_STATUS_DVI )
    {
        u16MinHfreq    = MIN_HFREQ_OF_DVI;
        u16MaxHfreq    = MAX_HFREQ_OF_DVI;
        u16MinVfreq    = MIN_VFREQ_OF_DVI;
        u16MaxVfreq    = MAX_VFREQ_OF_DVI;
        u16MinVtotal   = MIN_VTOTAL_OF_DVI;
        u16MaxVtotal   = MAX_VTOTAL_OF_DVI;
    }
#endif
    else
    {
        u16MinHfreq    = MIN_HFREQ_OF_YPBPR;
        u16MaxHfreq    = MAX_HFREQ_OF_YPBPR;
        u16MinVfreq    = MIN_VFREQ_OF_YPBPR;
        u16MaxVfreq    = MAX_VFREQ_OF_YPBPR;
        u16MinVtotal   = MIN_VTOTAL_OF_YPBPR;
        u16MaxVtotal   = MAX_VTOTAL_OF_YPBPR;
    }
#ifdef TEST_3D_FUNCTION_EN
    return FALSE;

#else
     PCMSG(printf("OOR u16HFreqx10 %d\n", psInputInfo->u16HFreqx10));
     PCMSG(printf("OOR u16VFreqx10 %d\n", psInputInfo->u16VFreqx10));
     PCMSG(printf("OOR u16Vtotal %d\n", psInputInfo->u16Vtotal));

    if( (psInputInfo->u16HFreqx10 > u16MaxHfreq) || (psInputInfo->u16HFreqx10 < u16MinHfreq) ||
        (psInputInfo->u16VFreqx10 > u16MaxVfreq) || (psInputInfo->u16VFreqx10 < u16MinVfreq) ||
        (psInputInfo->u16Vtotal > u16MaxVtotal) || (psInputInfo->u16Vtotal < u16MinVtotal)
#if (INPUT_HDMI_VIDEO_COUNT > 0)
        ||((IsSrcTypeHDMI(psInputInfo->eSrcType)&&(psInputInfo->sDVI_HDMI_DE_Info.width > MAX_WIDTH_OF_DVI)) || (psInputInfo->sDVI_HDMI_DE_Info.height > MAX_HEIGHT_OF_DVI))
#endif
        )
    {

        return TRUE;
    }

    return FALSE;
#endif
}


//--------------------------------------------------------------------------
static XC_MODEPARSE_RESULT _MApi_PCMode_ModeParse(INPUT_SOURCE_TYPE_t src,SCALER_WIN eWindow)
{
    MS_U8 u8Index = 0;

    // to match mode, we need these information
    // H/V freq., Vtt
    // Sync polarity
    // Interlaced or Progrssive
    PCMODE_INFO[eWindow].sModeParseInputInfo.eSrcType = src;
    PCMODE_INFO[eWindow].sModeParseInputInfo.u16HFreqx10 = MApi_XC_PCMonitor_Get_HFreqx10(eWindow);
    PCMODE_INFO[eWindow].sModeParseInputInfo.u16VFreqx10 = MApi_XC_PCMonitor_Get_VFreqx10(eWindow);
    PCMODE_INFO[eWindow].sModeParseInputInfo.u8SyncStatus = MApi_XC_PCMonitor_GetSyncStatus(eWindow);
    PCMODE_INFO[eWindow].sModeParseInputInfo.u16Vtotal = MApi_XC_PCMonitor_Get_Vtotal(eWindow);

    PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "_MApi_PCMode_ModeParse [IN] HFrq: %d, VFrq: %d, Vtt: %d\r\n",
                                                PCMODE_INFO[eWindow].sModeParseInputInfo.u16HFreqx10,
                                                PCMODE_INFO[eWindow].sModeParseInputInfo.u16VFreqx10,
                                                PCMODE_INFO[eWindow].sModeParseInputInfo.u16Vtotal));

    PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "_MApi_PCMode_ModeParse [DB0]HFrq: %d, VFrq: %d, Vtt: %d\r\n",
                                                 _pstTimingModeDB[0].u16HorizontalFrequency,
                                                 _pstTimingModeDB[0].u16VerticalFrequency,
                                                 _pstTimingModeDB[0].u16VerticalTotal));
    if ( IsSrcTypeHDMI(src) )
    {
        if ( MApi_XC_GetHdmiSyncMode() == HDMI_SYNC_DE)
        {
            MApi_XC_PCMonitor_Get_Dvi_Hdmi_De_Info(eWindow,&(PCMODE_INFO[eWindow].sModeParseInputInfo.sDVI_HDMI_DE_Info));
        }
        else
        {
            MApi_XC_GetDEWidthHeightInDEByPassMode(&PCMODE_INFO[eWindow].sModeParseInputInfo.sDVI_HDMI_DE_Info.width,&PCMODE_INFO[eWindow].sModeParseInputInfo.sDVI_HDMI_DE_Info.height, eWindow );
            //printf("daniel_test DEWidthHeightInDEByPassMode: W=%u,H=%u\n", PCMODE_INFO[eWindow].sModeParseInputInfo.sDVI_HDMI_DE_Info.width, PCMODE_INFO[eWindow].sModeParseInputInfo.sDVI_HDMI_DE_Info.height);
        }
    }


    // Pre-Check the signal Range
    if(_MApi_PCMode_Check_OOR(eWindow, &(PCMODE_INFO[eWindow].sModeParseInputInfo)))
    {
        PCMODE_INFO[eWindow].eModeParseResult = XC_MODEPARSE_OUT_OF_RANGE;
    }
    else
    {
        PCMODE_INFO[eWindow].eModeParseResult = MApi_XC_ModeParse_MatchMode( _pstTimingModeDB,
                                                                              _u8NumOfModeDBItem,
                                                                             &(PCMODE_INFO[eWindow].sModeParseInputInfo) );

        u8Index = PCMODE_INFO[eWindow].sModeParseInputInfo.u8ModeIndex;


        PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "_MApi_PCMode_ModeParse Error: %d, ModeIdx: %d\r\n", PCMODE_INFO[eWindow].eModeParseResult, u8Index) );



        _MApi_CheckSupportTiming(eWindow);
        if (IsSrcTypeVga(src))//VGA EIA timing
        {
            //MApi_XC_SetDbgLevel(XC_DBGLEVEL_AUTOGEOMETRY);
            if (XC_MODEPARSE_SUPPORT_MODE == PCMODE_INFO[eWindow].eModeParseResult)
            {
                u8Index = PCMODE_INFO[eWindow].sModeParseInputInfo.u8ModeIndex;
                if ((MD_720x480_60I == u8Index )||( MD_720x576_50I == u8Index))
                {
                     PCMODE_INFO[eWindow].eModeParseResult=XC_MODEPARSE_UNSUPPORT_MODE;
                }
            }
        }
    }

    return PCMODE_INFO[eWindow].eModeParseResult;
}

//--------------------------------------------------------------------------
MS_BOOL MApi_GetAutoColor_From_PQ(SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputSourceType, APIXC_AdcGainOffsetSetting *pAutoColor)
{
    MS_BOOL bRet = TRUE;
    MS_U16 u16Buf[E_PQ_PIC_AUTOCOLOR_NUM];
    PQ_WIN PQWin = eWindow == MAIN_WINDOW     ? PQ_MAIN_WINDOW : // for PQ maintain easier, always use sc0 main setting
                   eWindow == SUB_WINDOW      ? PQ_MAIN_WINDOW :
                   eWindow == SC2_MAIN_WINDOW ? PQ_MAIN_WINDOW :
                   eWindow == SC2_SUB_WINDOW  ? PQ_MAIN_WINDOW :
                                                PQ_MAIN_WINDOW;


    PQ_INPUT_SOURCE_TYPE enPQInput = IsSrcTypeYPbPr(enInputSourceType) ? PQ_INPUT_SOURCE_YPBPR : PQ_INPUT_SOURCE_VGA;
    MS_PQ_Mode_Info stPQModeInfo;
    MS_U8 u8CurrentSyncStatus = MApi_XC_PCMonitor_GetSyncStatus(eWindow);

    SCA_DBG(SCA_DBG_LV_COLOR, "AutoColor: S:%d, Dest:%d, Gain[%04x %04x %04x],  offset[%04x %04x %04x] \n)",
                    enInputSourceType, eWindow,
                    pAutoColor->u16RedGain, pAutoColor->u16GreenGain, pAutoColor->u16BlueGain,
                    pAutoColor->u16RedOffset, pAutoColor->u16GreenOffset, pAutoColor->u16BlueOffset);

    stPQModeInfo.bFBL = FALSE;
    stPQModeInfo.bInterlace = u8CurrentSyncStatus & XC_MD_INTERLACE_BIT ? TRUE : FALSE;
    stPQModeInfo.u16input_hsize = MApi_PCMode_Get_HResolution( eWindow ,TRUE);
    stPQModeInfo.u16input_vsize = MApi_PCMode_Get_VResolution( eWindow ,TRUE);
    stPQModeInfo.u16input_vfreq = MApi_XC_PCMonitor_Get_VFreqx10(eWindow);

    if(MDrv_PQ_Get_Picture(PQWin, enPQInput, &stPQModeInfo, E_PQ_PICTURE_AUTO_COLOR, (void *)u16Buf))
    {
        SCA_DBG(SCA_DBG_LV_COLOR, "AutoColor: S:%d, Dest:%d, Gain(%04x %04x %04x),  offset(%04x %04x %42x) \n",
            enInputSourceType, eWindow,
            u16Buf[E_PQ_PIC_AUTOCOLOR_GAIN_R], u16Buf[E_PQ_PIC_AUTOCOLOR_GAIN_G], u16Buf[E_PQ_PIC_AUTOCOLOR_GAIN_B],
            u16Buf[E_PQ_PIC_AUTOCOLOR_OFFSET_R], u16Buf[E_PQ_PIC_AUTOCOLOR_OFFSET_G], u16Buf[E_PQ_PIC_AUTOCOLOR_OFFSET_B]);

        pAutoColor->u16RedGain   = u16Buf[E_PQ_PIC_AUTOCOLOR_GAIN_R];
        pAutoColor->u16GreenGain = u16Buf[E_PQ_PIC_AUTOCOLOR_GAIN_G];
        pAutoColor->u16BlueGain  = u16Buf[E_PQ_PIC_AUTOCOLOR_GAIN_B];

        pAutoColor->u16RedOffset   = u16Buf[E_PQ_PIC_AUTOCOLOR_OFFSET_R];
        pAutoColor->u16GreenOffset = u16Buf[E_PQ_PIC_AUTOCOLOR_OFFSET_G];
        pAutoColor->u16BlueOffset  = u16Buf[E_PQ_PIC_AUTOCOLOR_OFFSET_B];

    }
    else
    {
        bRet = FALSE;
    }
    return bRet;

}


static MS_BOOL _MApi_PCMode_SetMode (SCALER_WIN eWindow)
{

    MS_BOOL  bDisplay = 1;
	MS_U8 u8_ADC_Idx;

    //printf(">> _MApi_PCMode_SetMode \n");

#if 0 //TODO: disable HDMI-related functions, JoshChiang--



    MApi_XC_HDMI_CheckModeChanged(TRUE, eWindow);

    if (MApi_XC_HDMI_GetHdmiType(SYS_INPUT_SOURCE_TYPE(eWindow)) == E_HDMI_STATUS_DVI)
    {
        //reset HDMI status
        MDrv_HDMI_pkt_reset(REST_HDMI_STATUS);
    }

    if (IsSrcTypeHDMI(SYS_INPUT_SOURCE_TYPE(eWindow)))
    {
        MApi_XC_HDMI_CheckPacketInfo();
        g_HdmiPollingStatus.u8ColorFormat = MApi_XC_HDMI_GetPacketInfo()->enPKT_Status.PacketColorFormat;

        #if ENABLE_3D_PROCESS

        #if 1
        if(MApi_PCMode_Get_VResolution(eWindow,FALSE) >= 1400)
        {
            printf("3D: framepacking is detected[%u]\n", MApi_PCMode_Get_VResolution(eWindow,FALSE));
            g_HdmiInput3DFormatStatus = E_XC_3D_INPUT_FRAME_PACKING;
        }
        #endif

        #if 1
        //Test with Samsung BD-C6900(1080P 3D Only!),sometimes package can not meet timing change.
        if(g_HdmiInput3DFormatStatus == E_XC_3D_INPUT_FRAME_PACKING)
        {
             printf("-----------------------frame packing\n");
            //MD_1280X1470_50P, //59
            //MD_1280X1470_60P, //60
            //MD_1920X2205_24P, //61
            if(MApi_PCMode_Get_VResolution(eWindow,FALSE) < 1400)
            {
                printf("<<Correct_VResolution[%d]>>\n", MApi_PCMode_Get_VResolution(eWindow,FALSE));
                g_HdmiInput3DFormatStatus = E_XC_3D_INPUT_MODE_NONE;
            }
        }
        #endif
        //g_HdmiInput3DFormatStatus = E_XC_3D_INPUT_FRAME_PACKING;
        if((eWindow == MAIN_WINDOW)&&(g_HdmiInput3DFormatStatus != E_XC_3D_INPUT_MODE_NONE))
        {
            g_HdmiInput3DFormat = g_HdmiInput3DFormatStatus;
            MApi_Scaler_Set_3DMode(MAPP_Scaler_Map3DFormatTo3DUserMode(g_HdmiInput3DFormatStatus));
        }

        #endif  // # if ENABLE_3D_PROCESS
    }

    if ( (IsSrcTypeHDMI(SYS_INPUT_SOURCE_TYPE(eWindow))) &&
          (MApi_XC_HDMI_GetHdmiType(SYS_INPUT_SOURCE_TYPE(eWindow)) == E_HDMI_STATUS_HDMI) )
    {
    /*
        if (MApi_XC_HDMI_GetPacketInfo()->enPKT_Value.PKT_GC_VALUE)
        {
            if (MApi_XC_HDMI_GetPacketInfo()->enPKT_Status.AVMuteStatus)
            {
                bDisplay = 0;
                g_HdmiPollingStatus.bMuteHDMIVideo = 1;
            }
            else
            {
                bDisplay = 1;
                g_HdmiPollingStatus.bMuteHDMIVideo = 0;
            }
        }
    */
     //  if (MApi_XC_HDMI_GetPacketInfo()->enPKT_Status.ChecksumErrOrBCHParityErr)
       if(!MApi_XC_HDMI_CheckSumAndBCH(SYS_INPUT_SOURCE_TYPE(eWindow)))
        {
            bDisplay = 0;
           // g_HdmiPollingStatus.bMuteHDMIVideo = 1;
        }
    }
#endif //TODO: disable HDMI-related functions, JoshChiang--

    if (MApi_XC_PCMonitor_InvalidTimingDetect(TRUE, eWindow))
    {
        bDisplay = 0;
        PCMSG(printf("Invalid Timing Detect \n"));
    }

    if ( !bDisplay )
    {
        //MS_BOOL bEnable = FALSE;

        MApi_XC_PCMonitor_Restart(eWindow);
        //MApi_XC_Mux_TriggerDestOnOffEvent(SYS_INPUT_SOURCE_TYPE(eWindow), &bEnable); Don't need it in CE
        PCMSG(printf("SET MODE FAIL \n"));

        return FALSE;
    }

    // Set all destination device connected with current source
    //MApi_XC_Mux_TriggerPathSyncEvent(SYS_INPUT_SOURCE_TYPE(eWindow), NULL);  Don't need it in CE

#if 1 //TODO: disable ADC & AutoOffset functions, JoshChiang--

    if (IsSrcTypeVga(SYS_INPUT_SOURCE_TYPE(eWindow)) || IsSrcTypeYPbPr(SYS_INPUT_SOURCE_TYPE(eWindow)))
    {
        /////////////////////////////////
        /// Setup user gain / offset
        /////////////////////////////////
        if ( IsSrcTypeVga(SYS_INPUT_SOURCE_TYPE(eWindow)) )
        {
            u8_ADC_Idx = ADC_SET_VGA;
        }
        else
        {
            // YPbPr
            if( MApi_XC_Sys_IsSrcHD(eWindow) )
            {
                // HD
        #if (ENABLE_ADC_BY_SOURCE ==1)
            #if(INPUT_YPBPR_VIDEO_COUNT==2)
                if(SYS_INPUT_SOURCE_TYPE(eWindow)==INPUT_SOURCE_YPBPR2)
                {
                    u8_ADC_Idx = ADC_SET_YPBPR2_HD;
                }
                else
            #endif
            #if(INPUT_YPBPR_VIDEO_COUNT==3)
                if(SYS_INPUT_SOURCE_TYPE(eWindow)==INPUT_SOURCE_YPBPR2)
                {
                    u8_ADC_Idx = ADC_SET_YPBPR3_HD;
                }
                else
            #endif
        #endif
                {
                    u8_ADC_Idx = ADC_SET_YPBPR_HD;
                }
            }
            else
            {
                // SD
        #if (ENABLE_ADC_BY_SOURCE ==1)
            #if(INPUT_YPBPR_VIDEO_COUNT==2)
                if(SYS_INPUT_SOURCE_TYPE(eWindow)==INPUT_SOURCE_YPBPR2)
                {
                    u8_ADC_Idx = ADC_SET_YPBPR2_SD;
                }
                else
            #endif
            #if(INPUT_YPBPR_VIDEO_COUNT==3)
                if(SYS_INPUT_SOURCE_TYPE(eWindow)==INPUT_SOURCE_YPBPR2)
                {
                    u8_ADC_Idx = ADC_SET_YPBPR3_SD;
                }
                else
            #endif
        #endif
                {
                    u8_ADC_Idx = ADC_SET_YPBPR_SD;
                }
            }
        }

#if PQ_ENABLE_COLOR
        if(stGenSetting.g_AdcSetting[u8_ADC_Idx].u8AdcCalOK == 0)
        {
            MApi_GetAutoColor_From_PQ(eWindow, SYS_INPUT_SOURCE_TYPE(eWindow), &(stGenSetting.g_AdcSetting[u8_ADC_Idx].stAdcGainOffsetSetting));
        }
#endif

        MApi_XC_ADC_AdjustGainOffset( &(stGenSetting.g_AdcSetting[u8_ADC_Idx].stAdcGainOffsetSetting) );

		//RETAILMSG(TRUE, (L"[Set_Mode] Adc_Idx: %d, GAIN_G: 0x%X, GAIN_B: 0x%X, GAIN_R: 0x%X \n", u8_ADC_Idx,
		//					stGenSetting.g_AdcSetting[u8_ADC_Idx].stAdcGainOffsetSetting.u16GreenGain,
		//					stGenSetting.g_AdcSetting[u8_ADC_Idx].stAdcGainOffsetSetting.u16BlueGain,
		//					stGenSetting.g_AdcSetting[u8_ADC_Idx].stAdcGainOffsetSetting.u16RedGain   ));
	}

    // Setup enrivement parameter by mode
    if( IsSrcTypeYPbPr(SYS_INPUT_SOURCE_TYPE(eWindow)) )
    {
        MApi_XC_Auto_AutoOffset( ENABLE, TRUE ); // Trigger ADC to do auto-offset
        //msAPI_Timer_Delayms(100);
    }
    else
    {
        MApi_XC_Auto_AutoOffset( DISABLE, FALSE );
    }
#endif  //TODO: disable ADC & AutoOffset functions, JoshChiang--

    return TRUE;
}

//--------------------------------------------------------------------------

static void _MApi_PCMode_SelfAuto(SCALER_WIN eWindow)
{
    XC_Auto_Signal_Info_Ex active;
    XC_Auto_Signal_Info_Ex standard;
    MS_PCADC_MODETABLE_TYPE sModeTbl;

    if(MApi_IsSrcHasSignal(eWindow))
    {
        if(( SYS_INPUT_SOURCE_TYPE(eWindow) == INPUT_SOURCE_VGA ) && (g_PcadcModeSetting[eWindow].u8AutoSign == 0))
        {
            printf("starting Geometyr Calibration \n");
            // Current signal info
            active.u16HorizontalStart = g_PcadcModeSetting[eWindow].u16HorizontalStart;
            active.u16VerticalStart   = g_PcadcModeSetting[eWindow].u16VerticalStart;
            active.u16HorizontalTotal = g_PcadcModeSetting[eWindow].u16HorizontalTotal;
            active.u16Phase = g_PcadcModeSetting[eWindow].u16Phase;
            active.u16Version = XC_AUTO_SIGNAL_INFO_VERSION;

            PC_SelfAuto_MSG(printf("MApi_PCMode_SelfAuto \n"));;

            MApi_PCMode_GetModeTbl(g_PcadcModeSetting[eWindow].u8ModeIndex, &sModeTbl);
            standard.u16HorizontalStart = sModeTbl.u16HorizontalStart;
            standard.u16VerticalStart = sModeTbl.u16VerticalStart;
            standard.u16HorizontalTotal = sModeTbl.u16HorizontalTotal;
            standard.u16HResolution = MApi_PCMode_Get_HResolution(eWindow,TRUE);
            standard.u16Phase = sModeTbl.u8AdcPhase;
            standard.u16Version = XC_AUTO_SIGNAL_INFO_VERSION;

            //ZUI: MApi_UiMenu_Show_Warning_Message_Dialog(MENU_AUTO_ADJUSTING_MSG);
            PC_SelfAuto_MSG(printf("Start Geometry\n"));
            // Geometry will update current active signal.
            if (MApi_XC_Auto_Geometry_Ex(E_XC_AUTO_TUNE_BASIC, &active, &standard, eWindow)) // auto-tune
            {
                // update default H/V start after E_XC_AUTO_TUNE_POSITION ( Do we need these two statements?)
                g_PcadcModeSetting[eWindow].u16DefaultHStart = active.u16HorizontalStart;
                g_PcadcModeSetting[eWindow].u16DefaultVStart = active.u16VerticalStart;
                g_PcadcModeSetting[eWindow].u16DefaultHTotal = active.u16HorizontalTotal;
                g_PcadcModeSetting[eWindow].u16HorizontalStart = active.u16HorizontalStart;
                g_PcadcModeSetting[eWindow].u16VerticalStart = active.u16VerticalStart;
                g_PcadcModeSetting[eWindow].u16HorizontalTotal = active.u16HorizontalTotal;
                g_PcadcModeSetting[eWindow].u16Phase = active.u16Phase;

                g_PcadcModeSetting[eWindow].u8AutoSign = TRUE;   // Auto config OK
                PC_SelfAuto_MSG(printf("\n AutoTune OK\n"));
            }
            else
            {
                g_PcadcModeSetting[eWindow].u8AutoSign = FALSE;  // Auto config NG
                //Mode change During AutoConfiguration
                PC_SelfAuto_MSG(printf("AutoTune FAIL\n"));
            }
            PC_SelfAuto_MSG(printf("Mode Index = %d \n",g_PcadcModeSetting[eWindow].u8ModeIndex));
            PC_SelfAuto_MSG(printf("Mode HStart = %d \n",g_PcadcModeSetting[eWindow].u16HorizontalStart));
            PC_SelfAuto_MSG(printf("Mode VStart = %d \n",g_PcadcModeSetting[eWindow].u16VerticalStart));
            PC_SelfAuto_MSG(printf("Mode Htotal = %d \n",g_PcadcModeSetting[eWindow].u16HorizontalTotal));
            PC_SelfAuto_MSG(printf("Mode Phase =  %d \n",g_PcadcModeSetting[eWindow].u16Phase));
            PC_SelfAuto_MSG(printf("Mode H Size = %d \n",MApi_PCMode_Get_HResolution(eWindow,TRUE)));
            PC_SelfAuto_MSG(printf("Mode V Size = %d \n",MApi_PCMode_Get_VResolution(eWindow,TRUE)));
            PC_SelfAuto_MSG(printf("Mode u8AutoSign = %d\n",g_PcadcModeSetting[eWindow].u8AutoSign));
        }
    }
    else
    {
        PC_SelfAuto_MSG(printf("[PC]no signal\n"));
    }
}

//--------------------------------------------------------------------------

#if 1

static XC_MODEPARSE_RESULT _MApi_PCMode_StausHandler(INPUT_SOURCE_TYPE_t src, XC_PCMONITOR_STATUS current_status , SCALER_WIN eWindow)
{
    U8 u8TurnOffDestination   = FALSE;
    XC_MODEPARSE_RESULT  eModeParseResult = XC_MODEPARSE_NOT_PARSED;

    PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "PCMode status: %d -> %d \n",previous_status[eWindow], current_status));

    // Stable -> unstable or no sync
    if (  previous_status[eWindow] == E_XC_PCMONITOR_STABLE_SYNC &&
               previous_status[eWindow] != current_status ) // unstable or no sync
    {
        // Disable all destination as soon as possiable.
        PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "\n PCMode: stable -> unstable or no sync"));


        if ( current_status == E_XC_PCMONITOR_UNSTABLE )
            PCMODE_INFO[eWindow].eCurrentState = E_PCMODE_UNSTABLE;
        else
            PCMODE_INFO[eWindow].eCurrentState = E_PCMODE_NOSYNC;

        // MApi_XC_Mux_TriggerDestOnOffEvent(src, &u8TurnOffDestination); Don't need it in CE

        if (_tModeParseInitData.pfSignalStableToUnstable)
        {
            _tModeParseInitData.pfSignalStableToUnstable(0);
        }
        else
        {
            PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "MApi_PCMode_ModeParse pfSignalStableToUnstable callback not defined!\n"));
        }
    }

    // unstable or no sync -> stable
    else if ( current_status == E_XC_PCMONITOR_STABLE_SYNC &&
         previous_status[eWindow] != current_status)
    {
        PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "PCMode: unstable or no sync -> stable \n"));

        // Mode parse after timing stable.
        eModeParseResult = _MApi_PCMode_ModeParse(src,eWindow);

        if(PCMODE_INFO[eWindow].eModeParseResult == XC_MODEPARSE_SUPPORT_MODE)
        {
            PCMODE_INFO[eWindow].eCurrentState = E_PCMODE_STABLE_SUPPORT_MODE;

            if(_MApi_PCMode_SetMode(eWindow))
            {
                PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "PCMode: Set Mode ok \n"));

                if(!MApi_PCMode_LoadUserModeSetting(eWindow, MApi_PCMode_Get_Mode_Idx(eWindow)))
                {
                    MApi_PCMode_LoadDefualtTable(eWindow, MApi_PCMode_Get_Mode_Idx(eWindow)); // Load Default table
                }

                // un-mute video
                u8TurnOffDestination = ENABLE;
                // MApi_XC_Mux_TriggerDestOnOffEvent(src,&u8TurnOffDestination);Don't need it in CE

                if (_tModeParseInitData.pfSignalUnstableToStable_TimingSupported_ModeSet)
                {
                    _tModeParseInitData.pfSignalUnstableToStable_TimingSupported_ModeSet(0);
                }
                else
                {
                    PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "MApi_PCMode_ModeParse pfSignalUnstableToStable_TimingSupported_ModeSet callback not defined!\n"));
                }

            }
            else
            {
                // set mode failed (timing changed), so disable all destination.
                PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "PCMode: Setmode failed \n"));

                // MApi_XC_Mux_TriggerDestOnOffEvent(src,&u8TurnOffDestination); Don't need it in CE

                if (_tModeParseInitData.pfSignalUnstableToStable_TimingSupported_ModeUnset)
                {
                    _tModeParseInitData.pfSignalUnstableToStable_TimingSupported_ModeUnset(0);
                }
                else
                {
                    PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "MApi_PCMode_ModeParse pfSignalUnstableToStable_TimingSupported_ModeUnset callback not defined!\n"));
                }

            }
        }
        else
        {
            PCMODE_INFO[eWindow].eCurrentState = E_PCMODE_STABLE_UN_SUPPORT_MODE;

            // un-support, so disable all destination.
            MApi_XC_Mux_TriggerDestOnOffEvent(src, &u8TurnOffDestination);

            if (_tModeParseInitData.pfSignalUnstableToStable_TimingUnsupported)
            {
                _tModeParseInitData.pfSignalUnstableToStable_TimingUnsupported(0);
            }
            else
            {
                PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "MApi_PCMode_ModeParse pfSignalUnstableToStable_TimingUnsupported callback not defined!\n"));
            }
        }

    }
    // Unstable -> no snyc
    else if ( previous_status[eWindow] == E_XC_PCMONITOR_UNSTABLE && current_status == E_XC_PCMONITOR_STABLE_NOSYNC)
    {
        PCMODE_INFO[eWindow].eCurrentState = E_PCMODE_NOSYNC;

        if (_tModeParseInitData.pfSignalUnstableToNoSync)
        {
            _tModeParseInitData.pfSignalUnstableToNoSync(0);
        }
        else
        {
            PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "MApi_PCMode_ModeParse pfSignalUnstableToNoSync callback not defined!\n"));
        }
    }
    // no sync -> unstable
    else if (previous_status[eWindow] == E_XC_PCMONITOR_STABLE_NOSYNC && current_status == E_XC_PCMONITOR_UNSTABLE)
    {
        PCMODE_INFO[eWindow].eCurrentState = E_PCMODE_UNSTABLE;

        if (_tModeParseInitData.pfSignalNoSyncToUnstable)
        {
            _tModeParseInitData.pfSignalNoSyncToUnstable(0);
        }
        else
        {
            PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "MApi_PCMode_ModeParse pfSignalNoSyncToUnstable callback not defined!\n"));
        }

    }

    if ( previous_status[eWindow] != current_status )
        previous_status[eWindow] = current_status;


    return eModeParseResult;
}

#else

static XC_MODEPARSE_RESULT _MApi_PCMode_StausHandler(INPUT_SOURCE_TYPE_t src, XC_PCMONITOR_STATUS current_status , SCALER_WIN eWindow)
{
    U8 u8TurnOffDestination   = FALSE;
    XC_MODEPARSE_RESULT  eModeParseResult = XC_MODEPARSE_NOT_PARSED;

    // Stable -> unstable or no sync
    if (  previous_status[eWindow] == E_XC_PCMONITOR_STABLE_SYNC &&
               previous_status[eWindow] != current_status ) // unstable or no sync
    {
        // Disable all destination as soon as possiable.
        PCMSG(printf("\n PCMode: stable -> unstable or no sync"));


        if ( current_status == E_XC_PCMONITOR_UNSTABLE )
            PCMODE_INFO[eWindow].eCurrentState = E_PCMODE_UNSTABLE;
        else
            PCMODE_INFO[eWindow].eCurrentState = E_PCMODE_NOSYNC;

        MApi_XC_Mux_TriggerDestOnOffEvent(src,&u8TurnOffDestination);

        #if (ENABLE_USER_CALLBACK)

        if (_tModeParseInitData.pfSignalStableToUnstable)
        {
            _tModeParseInitData.pfSignalStableToUnstable(0);
        }
        else
        {
            RETAILMSG(PCMSG_SEL, (TEXT("MApi_PCMode_ModeParse pfSignalStableToUnstable callback not defined!\n")));
        }

        #else // #if (ENABLE_USER_CALLBACK)

        if (IsSrcTypeHDMI(SYS_INPUT_SOURCE_TYPE(eWindow)))
        {
            #if ENABLE_3D_PROCESS
            if((eWindow == MAIN_WINDOW)&&(g_HdmiInput3DFormat != E_XC_3D_INPUT_MODE_NONE))
            {
                g_HdmiInput3DFormat = E_XC_3D_INPUT_MODE_NONE;
                g_HdmiInput3DFormatStatus = E_XC_3D_INPUT_MODE_NONE;
                previous_status[SUB_WINDOW] = E_XC_PCMONITOR_STABLE_NOSYNC;
                if(IsSrcTypeHDMI(src))
                {
                    MDrv_HDMI_pkt_reset(REST_HDMI_STATUS);
                }
                MApi_Scaler_Set_3DMode(MAPP_Scaler_Map3DFormatTo3DUserMode(E_XC_3D_INPUT_MODE_NONE));
            }
            #endif
        }
        #if (ENABLE_PIP)
        if( (IsPIPEnable()==FALSE) || UI_IS_AUDIO_SOURCE_IN(eWindow) )
        #endif
        {
//            MApi_AUDIO_HDMI_SetDownSample(0);
               _MDrv_HDMI_audio_downsample();
               msAPI_AUD_AdjustAudioFactor(E_ADJUST_AUDIOMUTE, E_AUDIO_BYSYNC_MUTEON, E_AUDIOMUTESOURCE_ACTIVESOURCE);
        }
        MApi_DB_LoadDefaultTable(eWindow, 0);


        // if any OSD Menu is on, disable it.
        if ((MApi_ZUI_GetActiveOSD() != E_OSD_EMPTY)&&(MApi_ZUI_GetActiveOSD() != E_OSD_FACTORY_MENU))
       {
           // MApi_ZUI_ACT_ShutdownOSD();
            MApi_ZUI_ACT_ExecuteWndAction(EN_EXE_CLOSE_CURRENT_OSD);
       }


        #endif  // #if (ENABLE_USER_CALLBACK)

    }

    // unstable or no sync -> stable
    else if ( current_status == E_XC_PCMONITOR_STABLE_SYNC &&
         previous_status[eWindow] != current_status)
    {
        PCMSG(printf("\n PCMode: unstable or no sync -> stable"));

        #if 0 //TODO: JoshChiang--
        #if (INPUT_HDMI_VIDEO_COUNT > 0)
        if(IsSrcTypeHDMI(src))
        {
            // check if HDMI or DVI
            MApi_XC_HDMI_CheckModeChanged(TRUE, eWindow);

            msApiPrintfHDMI_STATUSMode(MApi_XC_HDMI_GetHdmiType(SYS_INPUT_SOURCE_TYPE(eWindow)));
        }
        #endif
        #endif

        // Mode parse after timing stable.
        eModeParseResult = _MApi_PCMode_ModeParse(src,eWindow);

        if(PCMODE_INFO[eWindow].eModeParseResult == XC_MODEPARSE_SUPPORT_MODE)
        {
            PCMODE_INFO[eWindow].eCurrentState = E_PCMODE_STABLE_SUPPORT_MODE;

            #if 0 //TODO: JoshChiang--
            // load mode setting
            MApi_PCMode_LoadModeData(eWindow, PCMODE_INFO[eWindow].sModeParseInputInfo.u8ModeIndex, (MS_BOOL)( (PCMODE_INFO[eWindow].sModeParseInputInfo.u8SyncStatus & XC_MD_USER_MODE_BIT) >0));
            #endif

            if(_MApi_PCMode_SetMode(eWindow))
            {
                PCMSG(printf("\n PCMode: Set Mode ok\n"));

                // un-mute video
                u8TurnOffDestination = ENABLE;
                MApi_XC_Mux_TriggerDestOnOffEvent(src,&u8TurnOffDestination);

                #if (ENABLE_USER_CALLBACK)

                if (_tModeParseInitData.pfSignalUnstableToStable_TimingSupported_ModeSet)
                {
                    _tModeParseInitData.pfSignalUnstableToStable_TimingSupported_ModeSet(0);
                }
                else
                {
                    RETAILMSG(PCMSG_SEL, (TEXT("MApi_PCMode_ModeParse pfSignalUnstableToStable_TimingSupported_ModeSet callback not defined!\n")));
                }


                #else // #if (ENABLE_USER_CALLBACK)

                MApi_AnalogInputs_Force2MonitorWindows(eWindow);

                // switch audio between HDMI/DVI
                #if defined (DVB_SYSTEM)
                if(IsHDMIInUse())
                {
                    if (MApi_XC_HDMI_GetHdmiType(SYS_INPUT_SOURCE_TYPE(eWindow)) == E_HDMI_STATUS_HDMI)
                    {
                        #if (ENABLE_PIP)
                        if( (IsPIPEnable()==FALSE) || UI_IS_AUDIO_SOURCE_IN(eWindow) )
                        #endif
                        {
                            msAPI_AUD_AdjustAudioFactor(E_ADJUST_AUDIOMUTE, E_AUDIO_BYSYNC_MUTEON, E_AUDIOMUTESOURCE_ACTIVESOURCE);
                            MAPP_InputSource_SwitchHDMI_DVI(E_HDMI_STATUS_HDMI);
                        }
                    }
                    else if(MApi_XC_HDMI_GetHdmiType(SYS_INPUT_SOURCE_TYPE(eWindow)) == E_HDMI_STATUS_DVI)
                    {
                        #if (ENABLE_PIP)
                        if( (IsPIPEnable()==FALSE) || UI_IS_AUDIO_SOURCE_IN(eWindow) )
                        #endif
                        {
                            MAPP_InputSource_SwitchHDMI_DVI(E_HDMI_STATUS_DVI);
                        }
                    }
                }
                #endif
                // un-mute video
                u8TurnOffDestination = ENABLE;
                MApi_XC_Mux_TriggerDestOnOffEvent(src,&u8TurnOffDestination);

                // un-mute audio
                #if (ENABLE_PIP)
                if( (IsPIPEnable()==FALSE) || UI_IS_AUDIO_SOURCE_IN(eWindow) )
                #endif
                {
                    msAPI_AUD_AdjustAudioFactor(E_ADJUST_AUDIOMUTE, E_AUDIO_BYSYNC_MUTEOFF, E_AUDIOMUTESOURCE_ACTIVESOURCE);
                }
                //MApi_PCMode_SelfAuto();
                if(IsSrcTypeVga(SYS_INPUT_SOURCE_TYPE(eWindow))&&(g_PcadcModeSetting[eWindow].u8AutoSign == 0))
                {
                     PCMSG(printf("Auto Adjust!!!\n"););
                    if(MApi_PCMode_Enable_SelfAuto(ENABLE, eWindow))
                    {
                        #if (DTV_SYSTEM_SEL == DTV_SYSTEM_ATSC)
                        if((MApi_ZUI_GetActiveOSD() != E_OSD_FACTORY_MENU)
                        #if ENABLE_INPUT_LOCK
                         &&(MApi_UiMenuFunc_CheckInputLock ()==FALSE)
                        #endif
                         )
                        {
                            MApi_ZUI_ACT_StartupOSD(E_OSD_MESSAGE_BOX);
                            MApi_ZUI_ACT_ExecuteWndAction(EN_EXE_SHOW_AUTO_ADJUSTING_MSGBOX);
                            MApi_ZUI_API_SetTimer(HWND_MSGBOX_BG_PARENTAL_BG, 0, 5000);
                            MApi_TopStateMachine_SetTopState(STATE_TOP_ANALOGINPUTS);
                            MApi_UiMenu_Main_ResetState();
                        }
                        #else
                        {
                            MApi_ZUI_ACT_StartupOSD(E_OSD_MESSAGE_BOX);
                            MApi_ZUI_ACT_ExecuteWndAction(EN_EXE_SHOW_AUTO_ADJUSTING_MSGBOX);
                        }
                        #endif

                    }
                }

                if(MApi_ZUI_GetActiveOSD()==E_OSD_CHANNEL_INFO)
                {
                   MApi_ZUI_ACT_ExecuteWndAction(EN_EXE_SHOW_SOURCE_BANNER);
                }
                else if(MApi_ZUI_GetActiveOSD()==E_OSD_EMPTY)
                {
                   MApi_ZUI_ACT_StartupOSD(E_OSD_CHANNEL_INFO);
                   MApi_ZUI_ACT_ExecuteWndAction(EN_EXE_SHOW_SOURCE_BANNER);
                }

                #endif  // #if (ENABLE_USER_CALLBACK)
            }
            else
            {
                // set mode failed (timing changed), so disable all destination.
                PCMSG(printf("\n PCMode: Setmode failed"));

                MApi_XC_Mux_TriggerDestOnOffEvent(src,&u8TurnOffDestination);

                #if (ENABLE_USER_CALLBACK)

                if (_tModeParseInitData.pfSignalUnstableToStable_TimingSupported_ModeUnset)
                {
                    _tModeParseInitData.pfSignalUnstableToStable_TimingSupported_ModeUnset(0);
                }
                else
                {
                    RETAILMSG(PCMSG_SEL, (TEXT("MApi_PCMode_ModeParse pfSignalUnstableToStable_TimingSupported_ModeUnset callback not defined!\n")));
                }

                #else // #if (ENABLE_USER_CALLBACK)

                #if (ENABLE_PIP)
                if( (IsPIPEnable()==FALSE) || UI_IS_AUDIO_SOURCE_IN(eWindow) )
                #endif
                {
                    msAPI_AUD_AdjustAudioFactor(E_ADJUST_AUDIOMUTE, E_AUDIO_BYSYNC_MUTEON, E_AUDIOMUTESOURCE_ACTIVESOURCE);
                }
                MApi_DB_LoadDefaultTable(eWindow, 0);

                #endif // #if (ENABLE_USER_CALLBACK)
            }
        }
        else
        {
            PCMODE_INFO[eWindow].eCurrentState = E_PCMODE_STABLE_UN_SUPPORT_MODE;

            // un-support, so disable all destination.
            MApi_XC_Mux_TriggerDestOnOffEvent(src,&u8TurnOffDestination);

            #if (ENABLE_USER_CALLBACK)

            if (_tModeParseInitData.pfSignalUnstableToStable_TimingUnsupported)
            {
                _tModeParseInitData.pfSignalUnstableToStable_TimingUnsupported(0);
            }
            else
            {
                RETAILMSG(PCMSG_SEL, (TEXT("MApi_PCMode_ModeParse pfSignalUnstableToStable_TimingUnsupported callback not defined!\n")));
            }

            #else   // #if (ENABLE_USER_CALLBACK)

            #if (ENABLE_PIP)
            if( (IsPIPEnable()==FALSE) || UI_IS_AUDIO_SOURCE_IN(eWindow) )
            #endif
            {
                msAPI_AUD_AdjustAudioFactor(E_ADJUST_AUDIOMUTE, E_AUDIO_BYSYNC_MUTEON, E_AUDIOMUTESOURCE_ACTIVESOURCE);
            }
            MApi_DB_LoadDefaultTable(eWindow, 0);


            #endif // #if (ENABLE_USER_CALLBACK)

        }

    }

    // Unstable -> no snyc
    else if ( previous_status[eWindow] == E_XC_PCMONITOR_UNSTABLE && current_status == E_XC_PCMONITOR_STABLE_NOSYNC)
    {
        PCMODE_INFO[eWindow].eCurrentState = E_PCMODE_NOSYNC;

        #if (ENABLE_USER_CALLBACK)

        if (_tModeParseInitData.pfSignalUnstableToNoSync)
        {
            _tModeParseInitData.pfSignalUnstableToNoSync(0);
        }
        else
        {
            RETAILMSG(PCMSG_SEL, (TEXT("MApi_PCMode_ModeParse pfSignalUnstableToNoSync callback not defined!\n")));
        }

        #else

        msAPI_AUD_AdjustAudioFactor(E_ADJUST_AUDIOMUTE, E_AUDIO_BYSYNC_MUTEON, E_AUDIOMUTESOURCE_ACTIVESOURCE);


        #endif  // #if (ENABLE_USER_CALLBACK)
    }

    // no sync -> unstable
    else if (previous_status[eWindow] == E_XC_PCMONITOR_STABLE_NOSYNC && current_status == E_XC_PCMONITOR_UNSTABLE)
    {
        PCMODE_INFO[eWindow].eCurrentState = E_PCMODE_UNSTABLE;

        #if (ENABLE_USER_CALLBACK)
        if (_tModeParseInitData.pfSignalNoSyncToUnstable)
        {
            _tModeParseInitData.pfSignalNoSyncToUnstable(0);
        }
        else
        {
            RETAILMSG(PCMSG_SEL, (TEXT("MApi_PCMode_ModeParse pfSignalNoSyncToUnstable callback not defined!\n")));
        }
        #else

        msAPI_AUD_AdjustAudioFactor(E_ADJUST_AUDIOMUTE, E_AUDIO_BYSYNC_MUTEON, E_AUDIOMUTESOURCE_ACTIVESOURCE);

        #endif  // #if (ENABLE_USER_CALLBACK)
    }

    if ( previous_status[eWindow] != current_status )
        previous_status[eWindow] = current_status;


    return eModeParseResult;
}
#endif


//------------------------------------------------------------------------------
//  Export function
//------------------------------------------------------------------------------
MS_U16 MApi_PCMode_Get_HResolution(SCALER_WIN eWindow,MS_BOOL IsYpbprOrVga)
{
    if( (PCMODE_INFO[eWindow].eCurrentState == E_PCMODE_UNSTABLE) || (PCMODE_INFO[eWindow].eCurrentState == E_PCMODE_NOSYNC))
    {
        return 0;
    }
    else
    {
        if(IsYpbprOrVga)
        {
            // VGA/YPbPr
            return (PCMODE_INFO[eWindow].sModeParseInputInfo.u16ResWidth);
        }
        else
        {
            return PCMODE_INFO[eWindow].sModeParseInputInfo.sDVI_HDMI_DE_Info.width;
        }
    }
}

//--------------------------------------------------------------------------


MS_U16 MApi_PCMode_Get_VResolution(SCALER_WIN eWindow,MS_BOOL IsYpbprOrVga)
{
    if((PCMODE_INFO[eWindow].eCurrentState == E_PCMODE_UNSTABLE) || (PCMODE_INFO[eWindow].eCurrentState == E_PCMODE_NOSYNC))
    {
        return 0;
    }
    else
    {
        if(IsYpbprOrVga)
        {
            // VGA/YPbPr
            return PCMODE_INFO[eWindow].sModeParseInputInfo.u16ResHeight;
        }
        else
        {
            return PCMODE_INFO[eWindow].sModeParseInputInfo.sDVI_HDMI_DE_Info.height;
        }
    }
}


//--------------------------------------------------------------------------

void MApi_PCMode_Init(const MS_MODEPARSE_INITDATA* ptModeParseInitData)
{
    _PCMode_Init();

    if (ptModeParseInitData)
    {
        memcpy(&_tModeParseInitData, ptModeParseInitData, sizeof(MS_MODEPARSE_INITDATA));
    }
    else
    {
        memset(&_tModeParseInitData, 0, sizeof(MS_MODEPARSE_INITDATA));
    }

    PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "MApi_PCMode_Init:done!\n"));
}

//--------------------------------------------------------------------------

void MApi_PCMode_SetTimingModeDB(MS_PCADC_MODETABLE_TYPE* a_pstInputModeDB, MS_U8 a_u8NumOfDBItem)
{
    if ( !a_pstInputModeDB || 0==a_u8NumOfDBItem)
        return;

    _pstTimingModeDB   = a_pstInputModeDB;
    _u8NumOfModeDBItem = a_u8NumOfDBItem;
}


void MApi_PCMode_Init_UserModeSetting(void)
{
    MS_U32 i;

    for(i=0;i<MD_STD_MODE_MAX_INDEX;i++)
    {
        gstUserModeSetting[i].u16DefaultHStart = astStandardModeTable[i].u16HorizontalStart;
        gstUserModeSetting[i].u16DefaultHTotal = astStandardModeTable[i].u16HorizontalTotal;
        gstUserModeSetting[i].u16DefaultVStart = astStandardModeTable[i].u16VerticalStart;
        gstUserModeSetting[i].u16HorizontalStart = astStandardModeTable[i].u16HorizontalStart;
        gstUserModeSetting[i].u16HorizontalTotal = astStandardModeTable[i].u16HorizontalTotal;
        gstUserModeSetting[i].u16Phase = astStandardModeTable[i].u8AdcPhase;
        gstUserModeSetting[i].u16VerticalStart = astStandardModeTable[i].u16VerticalStart;
        gstUserModeSetting[i].u8AutoSign = 0;
        gstUserModeSetting[i].u8ModeIndex = i;
        gstUserModeSetting[i].u8Resolution = 0;
        gstUserModeSetting[i].u8SyncStatus = astStandardModeTable[i].u8StatusFlag;
    }
}

void MApi_PCMode_SetUserModeSetting(MS_U8 u8ModeIdx, MS_PCADC_MODESETTING_TYPE stModeSetting)
{
    MS_U8 u8SyncSatus = gstUserModeSetting[u8ModeIdx].u8SyncStatus;

    memcpy(&gstUserModeSetting[u8ModeIdx], &stModeSetting, sizeof(MS_PCADC_MODESETTING_TYPE));
    gstUserModeSetting[u8ModeIdx].u8SyncStatus = u8SyncSatus;

    SCA_DBG(SCA_DBG_LV_CALI,"SetUserMode: ModeIdx:%d, Htt:%d, Phase:%d, Hstar:%d, VStart:%d \n",
        gstUserModeSetting[u8ModeIdx].u8ModeIndex,
        gstUserModeSetting[u8ModeIdx].u16HorizontalTotal,
        gstUserModeSetting[u8ModeIdx].u16Phase,
        gstUserModeSetting[u8ModeIdx].u16HorizontalStart,
        gstUserModeSetting[u8ModeIdx].u16VerticalStart);
}

//--------------------------------------------------------------------------

XC_MODEPARSE_RESULT MApi_PC_MainWin_Handler(INPUT_SOURCE_TYPE_t src, SCALER_WIN eWindow)
{
    XC_MODEPARSE_RESULT  eModeParseResult = XC_MODEPARSE_NOT_PARSED;

    eModeParseResult = _MApi_PCMode_StausHandler(src, MApi_XC_PCMonitor(src, eWindow), eWindow);

    //MApi_PCMode_RunSelfAuto(eWindow);

    return eModeParseResult;
}

//--------------------------------------------------------------------------
void MApi_PCMode_RunSelfAuto(SCALER_WIN eWindow)
{
   if(IsSrcTypeVga(SYS_INPUT_SOURCE_TYPE(eWindow))&&(Enable_SelfAuto[eWindow] == ENABLE))
    {
        //U8 u8ModeIndex;

        Enable_SelfAuto[eWindow] = DISABLE;
        g_PcadcModeSetting[eWindow].u8AutoSign = 0;
        _MApi_PCMode_SelfAuto(eWindow);

        #if 0 // TODO: JoshChiang--, 20120208
        if(g_u8PcUserModeRamIndex[eWindow] == 0xFF)
            u8ModeIndex = g_PcadcModeSetting[eWindow].u8ModeIndex; //standard
        else
            u8ModeIndex = g_u8PcUserModeRamIndex[eWindow]; //user

        MApi_DB_SaveModeSetting(eWindow, u8ModeIndex);
        #endif
    }
    else
    {
        printf("Not Slef Auto \n");
    }
}

//--------------------------------------------------------------------------

MS_BOOL MApi_PCMode_Enable_SelfAuto(MS_BOOL EnableAuto, SCALER_WIN eWindow)
{
    if(!MApi_XC_PCMonitor_SyncLoss(eWindow))
    {
        Enable_SelfAuto[eWindow] = EnableAuto;
        return TRUE;
    }
    Enable_SelfAuto[eWindow] = DISABLE;
    return FALSE;
}


//--------------------------------------------------------------------------

void MApi_PCMode_Reset( SCALER_WIN eWindow)
{
    PCMODE_INFO[eWindow].eCurrentState = E_PCMODE_NOSYNC;
    PCMODE_INFO[eWindow].eModeParseResult = XC_MODEPARSE_NOT_PARSED;
    memset(&(PCMODE_INFO[eWindow].sModeParseInputInfo), 0, sizeof(XC_MODEPARSE_INPUT_INFO));
}

//--------------------------------------------------------------------------

PCMODE_SyncStatus MApi_PCMode_GetCurrentState(SCALER_WIN eWindow)
{
    return PCMODE_INFO[eWindow].eCurrentState;
}

//--------------------------------------------------------------------------

MS_U8 MApi_PCMode_Get_Mode_Idx(SCALER_WIN eWindow)
{

    if((PCMODE_INFO[eWindow].eCurrentState == E_PCMODE_UNSTABLE) || (PCMODE_INFO[eWindow].eCurrentState == E_PCMODE_NOSYNC))
    {
        return 0;
    }
    else
    {
        return PCMODE_INFO[eWindow].sModeParseInputInfo.u8ModeIndex;
    }
}

//--------------------------------------------------------------------------

void MApi_PCMode_GetModeTbl(MS_U8 u8ModeIdx, MS_PCADC_MODETABLE_TYPE *sModeTbl)
{
    memcpy(sModeTbl, &(astStandardModeTable[u8ModeIdx]), sizeof(MS_PCADC_MODETABLE_TYPE));
}

//--------------------------------------------------------------------------
MS_BOOL MApi_PCMode_LoadUserModeSetting(SCALER_WIN eWindow, MS_U8 u8ModeIndex)
{
    MS_BOOL bRet;

    if(gstUserModeSetting[u8ModeIndex].u8AutoSign == 1 &&
       gstUserModeSetting[u8ModeIndex].u8ModeIndex == u8ModeIndex)
    {
        memcpy(&g_PcadcModeSetting[eWindow], &gstUserModeSetting[u8ModeIndex], sizeof(MS_PCADC_MODESETTING_TYPE));
        bRet = TRUE;

        SCA_DBG(SCA_DBG_LV_CALI,"User Mode: \n");
        SCA_DBG(SCA_DBG_LV_CALI,"ModeIdx:%d, Htotal:%d, Hstar:%d, Vstart:%d, Phase:%d \n",
            g_PcadcModeSetting[eWindow].u8ModeIndex,
            g_PcadcModeSetting[eWindow].u16HorizontalTotal,
            g_PcadcModeSetting[eWindow].u16HorizontalStart,
            g_PcadcModeSetting[eWindow].u16VerticalStart,
            g_PcadcModeSetting[eWindow].u16Phase);
    }
    else
    {
        SCA_DBG(SCA_DBG_LV_CALI, "UserMode not setting \n");
        bRet = FALSE;
    }
    return bRet;
}



MS_BOOL MApi_PCMode_LoadDefualtTable(SCALER_WIN eWindow, MS_U8 u8ModeIdx)
{
    MS_PCADC_MODETABLE_TYPE sModeTbl;

    MApi_PCMode_GetModeTbl(u8ModeIdx, &sModeTbl);

    g_PcadcModeSetting[eWindow].u8ModeIndex = u8ModeIdx;
    g_PcadcModeSetting[eWindow].u16HorizontalStart = sModeTbl.u16HorizontalStart;
    g_PcadcModeSetting[eWindow].u16VerticalStart = sModeTbl.u16VerticalStart;
    g_PcadcModeSetting[eWindow].u16HorizontalTotal = sModeTbl.u16HorizontalTotal;
    g_PcadcModeSetting[eWindow].u16Phase = sModeTbl.u8AdcPhase;
    g_PcadcModeSetting[eWindow].u16DefaultHStart = g_PcadcModeSetting[eWindow].u16HorizontalStart;
    g_PcadcModeSetting[eWindow].u16DefaultVStart = g_PcadcModeSetting[eWindow].u16VerticalStart;
    g_PcadcModeSetting[eWindow].u16DefaultHTotal = g_PcadcModeSetting[eWindow].u16HorizontalTotal;
    g_PcadcModeSetting[eWindow].u8SyncStatus = sModeTbl.u8StatusFlag;
    g_PcadcModeSetting[eWindow].u8AutoSign = 0;

    PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "ModeInfo: Idx:%d, Hstart:%d, Vstart:%d, Htt=%d \n",
                           g_PcadcModeSetting[eWindow].u8ModeIndex,
                           g_PcadcModeSetting[eWindow].u16HorizontalStart,
                           g_PcadcModeSetting[eWindow].u16VerticalStart,
                           g_PcadcModeSetting[eWindow].u16HorizontalTotal));
    return TRUE;
}

MS_PCADC_MODESETTING_TYPE MApi_PCMode_Get_ModeInfo(SCALER_WIN eWindow)
{
    return g_PcadcModeSetting[eWindow];
}

//--------------------------------------------------------------------------

void MApi_Scaler_MainWindowOnOffEventHandler(INPUT_SOURCE_TYPE_t src, void* para)
{
    U8 u8Enable = *((U8*)para);

    PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "MApi_Scaler_MainWindowOnOffEventHandler: invoke!\n"));


    // Turn on Main window
    if ( u8Enable )
    {
        #if 0 // TODO: JoshChiang--, 20120208
        //if(MApi_ZUI_GetActiveOSD() == E_OSD_MAIN_MENU)
        {
            MApi_XC_Sys_PQ_ReduceBW_ForOSD(MAIN_WINDOW, MApi_GOP_GWIN_IsEnabled());
        }
        MApi_Picture_Setting_SetColor(src, MAIN_WINDOW);
        #endif
    }
    else
    {
        MApi_XC_Mux_OnOffPeriodicHandler(src, DISABLE);
    }

#if ENABLE_3D_PROCESS
    if((E_XC_3D_OUTPUT_LINE_ALTERNATIVE == MApi_XC_Get_3D_Output_Mode())
        || (E_XC_3D_OUTPUT_TOP_BOTTOM == MApi_XC_Get_3D_Output_Mode())
        || (E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF == MApi_XC_Get_3D_Output_Mode()))
    {
        if(MApi_XC_Get_3D_HW_Version() < 2)
        {
            MApi_XC_DisableInputSource(!u8Enable, SUB_WINDOW);
        }
    }
    MDrv_PQ_3DCloneforPIP(TRUE);
#endif

    #if 1 // TODO: JoshChiang--, 20120208

        MApi_SetBlueScreen( !u8Enable, E_XC_FREE_RUN_COLOR_BLACK, DEFAULT_SCREEN_UNMUTE_TIME, MAIN_WINDOW);

    #else

    if(MApi_VChip_GetCurVChipBlockStatus())
        msAPI_Scaler_SetBlueScreen( ENABLE, E_XC_FREE_RUN_COLOR_BLACK, DEFAULT_SCREEN_UNMUTE_TIME, MAIN_WINDOW);
    else
    {
        msAPI_Scaler_SetBlueScreen( !u8Enable , E_XC_FREE_RUN_COLOR_BLACK, DEFAULT_SCREEN_UNMUTE_TIME, MAIN_WINDOW);
    }

    #endif
}

//--------------------------------------------------------------------------

void MApi_Scaler_MainWindowSyncEventHandler(INPUT_SOURCE_TYPE_t src, void* para)
{
    #if 0 // TODO: JoshChiang--, 20120208
    UNUSED(src); UNUSED(para);

    stSystemInfo[MAIN_WINDOW].enAspectRatio = stGenSetting.g_VideoSetting.VideoScreen;//MApp_Scaler_GetAspectRatio(stGenSetting.g_VideoSetting.VideoScreen);
    _MApp_Scaler_CheckHDMode();
    #endif

    PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "MApi_Scaler_MainWindowSyncEventHandler: invoke!\n"));

#if 0 // ToDo Ryan
    MApi_Scaler_SetWindow(NULL, NULL, NULL,
        src,
        MAIN_WINDOW);

    // panelTiming is determined by main input source
    MApi_Scaler_SetTiming(src, MAIN_WINDOW, NULL);
#endif

    MApi_XC_Mux_OnOffPeriodicHandler(src, ENABLE);

}


//*************************************************************************
//Function name:     MApp_YPbPr_Setting_Auto
//Passing parameter: none
//Return parameter:  none
//Description:       Auto adjust video for YPbpr mode
//*************************************************************************
MS_BOOL MApp_YPbPr_Setting_Auto(XC_Auto_CalibrationType type,SCALER_WIN eWindow, MS_ADC_SETTING *pstMsAdcSetting)
{
    MS_BOOL bResult = FALSE;
    XC_Auto_CalibrationType calibration_type = type;
    E_ADC_SET_INDEX eAdcSetIdx, eAdcSetIdxToCopy;

	if (!pstMsAdcSetting)
		return bResult;

    if((type == E_XC_EXTERNAL_CALIBRATION) && (MApi_IsSrcHasSignal(eWindow) != TRUE))
    {
        // external calibration without input signal
        calibration_type = E_XC_INTERNAL_CALIBRATION;
    }

    if (calibration_type == E_XC_EXTERNAL_CALIBRATION )
    {
        // seperate SD/HD setting
        if(MApi_XC_Sys_IsSrcHD(eWindow))
        {
            eAdcSetIdx = ADC_SET_YPBPR_HD;              // calibration HD resolution
        }
        else
        {
            eAdcSetIdx = ADC_SET_YPBPR_SD;
        }
    }
    else // Internal calibration
    {
        eAdcSetIdx = ADC_SET_YPBPR_SD;
        eAdcSetIdxToCopy = ADC_SET_YPBPR_HD;        // if internal calibration, copy SD setting to HD
    }

    bResult = MApi_XC_Auto_GainOffset(calibration_type, E_XC_AUTO_TUNE_YUV_COLOR,
        &(pstMsAdcSetting->stAdcGainOffsetSetting), eWindow);

    // do auto calibration
    if(bResult)
    {
        // auto gain/offset calibration successed
        pstMsAdcSetting->u8AdcCalOK = 0xAA;
    }
    else
    {
        pstMsAdcSetting->u8AdcCalOK = 0x00;
    }

    PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "u16RedGain %x \n",    pstMsAdcSetting->stAdcGainOffsetSetting.u16RedGain));
    PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "u16GreenGain %x \n",  pstMsAdcSetting->stAdcGainOffsetSetting.u16GreenGain));
    PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "u16BlueGain %x \n",   pstMsAdcSetting->stAdcGainOffsetSetting.u16BlueGain));
    PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "u16RedOffset %x \n",  pstMsAdcSetting->stAdcGainOffsetSetting.u16RedOffset));
    PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "u16GreenOffset %x \n",pstMsAdcSetting->stAdcGainOffsetSetting.u16GreenOffset));
    PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "u16BlueOffset %x \n", pstMsAdcSetting->stAdcGainOffsetSetting.u16BlueOffset));

    //MApp_SaveADCSetting(eAdcSetIdx);

    if(calibration_type == E_XC_INTERNAL_CALIBRATION)
    {
		PCMSG_DBG(SCA_DBG(SCA_DBG_LV_MODE_PARSE, "[AUTO_CALIB] INTERNAL_CALIBRATION not supported !!! \n"));

		bResult = FALSE;

        //memcpy(&stGenSetting.g_AdcSetting[eAdcSetIdxToCopy].stAdcGainOffsetSetting,
        //       &stGenSetting.g_AdcSetting[eAdcSetIdx].stAdcGainOffsetSetting,
        //       sizeof(APIXC_AdcGainOffsetSetting));

        //stGenSetting.g_AdcSetting[eAdcSetIdxToCopy].u8AdcCalOK = stGenSetting.g_AdcSetting[eAdcSetIdx].u8AdcCalOK;
        //MApp_SaveADCSetting(eAdcSetIdxToCopy);
    }

	if (bResult)
	{
		memcpy( &stGenSetting.g_AdcSetting[eAdcSetIdx], pstMsAdcSetting, sizeof(MS_ADC_SETTING) );
	}

    return bResult;
}

//*************************************************************************
//Function name:     MApp_RGB_Setting_Auto
//Passing parameter: none
//Return parameter:  none
//Description:       Auto adjust video for VGA mode
//*************************************************************************
MS_BOOL MApp_RGB_Setting_Auto(XC_Auto_CalibrationType type,SCALER_WIN eWindow, MS_ADC_SETTING *pstMsAdcSetting)
{
    MS_BOOL bResult = FALSE;
    XC_Auto_CalibrationType calibration_type = type;

    if((type == E_XC_EXTERNAL_CALIBRATION) && (MApi_IsSrcHasSignal(eWindow) != TRUE))
    {
        // external calibration without input signal
        calibration_type = E_XC_INTERNAL_CALIBRATION;
    }

    bResult =  MApi_XC_Auto_GainOffset(calibration_type, E_XC_AUTO_TUNE_RGB_COLOR,
        &(pstMsAdcSetting->stAdcGainOffsetSetting), eWindow);

    // do auto calibration
    if(bResult)
    {
        // auto gain/offset calibration successed
        pstMsAdcSetting->u8AdcCalOK = 0xAA;
    }
    else
    {
        pstMsAdcSetting->u8AdcCalOK = 0x00;
    }


    ////MApp_SaveADCSetting(ADC_SET_VGA);

    return bResult;
}


//*************************************************************************
//Function name:    MApp_IsSrcHasSignal
//Passing parameter: eWindow - MAIN_WINDOW, SUB_WINDOW
//Return parameter:
//Description:
//*************************************************************************
MS_BOOL MApi_IsSrcHasSignal(SCALER_WIN eWindow)
{
    if ( IsSrcTypeAnalog(SYS_INPUT_SOURCE_TYPE(eWindow)) || IsSrcTypeHDMI(SYS_INPUT_SOURCE_TYPE(eWindow)) )
    {
        //VGA should not shut down if it is unsupport signal
        if( IsSrcTypeVga(SYS_INPUT_SOURCE_TYPE(eWindow)) && (MApi_PCMode_GetCurrentState(eWindow) == E_PCMODE_STABLE_UN_SUPPORT_MODE))
        {
            return TRUE;
        }

        if(MApi_PCMode_GetCurrentState(eWindow) != E_PCMODE_STABLE_SUPPORT_MODE) //(g_bInputTimingChange||g_bUnsupportMode||(msAPI_Scaler_SyncLossFlag()))
        {
            #if defined(SCA_ENABLE_DLC)
            MApi_XC_DLC_CGC_Reset();
            //#if ENABLE_DBC
            //    MApi_XC_DLC_DBC_Reset();
            //#endif
            #endif
            return FALSE;
        }
     }
 #if 0	//!! focus on YPbPr/VGA/AV input, 20120726. !!//

    else if ( IsSrcTypeDTV(SYS_INPUT_SOURCE_TYPE(eWindow)) )
    {

    #if 0
        MMSAPI_VID_STATUS vidStatus;
        return (msAPI_VID_GetVidInfo(&vidStatus));
    #else
        if( FE_LOCK == MApp_GetSignalLockStatus() )
            return TRUE;
        else
        {
            #if defined(SCA_ENABLE_DLC)
            MApi_XC_DLC_CGC_Reset();
            #if ENABLE_DBC
                MApi_XC_DLC_DBC_Reset();
            #endif
            #endif
            return FALSE;
        }
    #endif
    }
    else if ( IsSrcTypeDigitalVD(SYS_INPUT_SOURCE_TYPE(eWindow)))
    {
        if (!msAPI_AVD_IsSyncLocked()||(mvideo_vd_get_videosystem() == SIG_NONE))
        {
            #if defined(SCA_ENABLE_DLC)
            MApi_XC_DLC_CGC_Reset();
            #if ENABLE_DBC
                MApi_XC_DLC_DBC_Reset();
            #endif
            #endif
            return FALSE;
        }
    }
    else if (IsSrcTypeStorage(SYS_INPUT_SOURCE_TYPE(eWindow)))
    {
    }
    else
    {
        #if defined(SCA_ENABLE_DLC)
        MApi_XC_DLC_CGC_Reset();
        #if ENABLE_DBC
                MApi_XC_DLC_DBC_Reset();
            #endif
        #endif
        return FALSE;
    }
#endif

    return TRUE;
}

//------------------------------------------------------------------------
MS_BOOL MApp_SetADCSetting(E_ADC_SET_INDEX eAdcIndex, const MS_ADC_SETTING * pstMsAdcSetting)
{
	if (!pstMsAdcSetting)
		return FALSE;

	memcpy( &stGenSetting.g_AdcSetting[eAdcIndex], pstMsAdcSetting, sizeof(MS_ADC_SETTING));

	return TRUE;
}

//------------------------------------------------------------------------
void MApp_Scaler_CheckHDMode(SCALER_WIN eWindow)
{
	MS_U16 u16H_CapSize=0, u16V_CapSize=0;

	if( IsSrcTypeDigitalVD(SYS_INPUT_SOURCE_TYPE(eWindow)) )
	{
		MApi_XC_Sys_SetSrcIsHD( FALSE, eWindow );
	}
	else
	{

#if (INPUT_HDMI_VIDEO_COUNT > 0)
		if(IsSrcTypeHDMI(SYS_INPUT_SOURCE_TYPE(eWindow)))
		{
			u16H_CapSize = MApi_PCMode_Get_HResolution(eWindow,FALSE);
			u16V_CapSize = MApi_PCMode_Get_VResolution(eWindow,FALSE);
		}
		else
#endif
		if(IsSrcTypeVga(SYS_INPUT_SOURCE_TYPE(eWindow)) || IsSrcTypeYPbPr(SYS_INPUT_SOURCE_TYPE(eWindow)))
		{
			u16H_CapSize = MApi_PCMode_Get_HResolution( eWindow, TRUE); // standard display width
			u16V_CapSize = MApi_PCMode_Get_VResolution( eWindow, TRUE); // standard display height
		}
		else if( IsSrcTypeDTV(SYS_INPUT_SOURCE_TYPE(eWindow)) )
		{
			u16H_CapSize = MDrv_MVOP_GetHSize();
			u16V_CapSize = MDrv_MVOP_GetVSize();
		}
		else if( IsSrcTypeCamera(SYS_INPUT_SOURCE_TYPE(eWindow)) )
		{
            MS_INPUT_TIMING_CONFIG stCameraTiming;

            MApi_InputSrouce_GetCameraTiming(&stCameraTiming);
            u16H_CapSize = stCameraTiming.u16Width;
            u16V_CapSize = stCameraTiming.u16Height;
		}
        else if(IsSrcTypeSC_VOP(SYS_INPUT_SOURCE_TYPE(eWindow)))
        {
            SCALER_WIN eWin = SYS_INPUT_SOURCE_TYPE(eWindow) == INPUT_SOURCE_SC0_VOP ? MAIN_WINDOW :
                              SYS_INPUT_SOURCE_TYPE(eWindow) == INPUT_SOURCE_SC1_VOP ? SC1_MAIN_WINDOW :
                                                                                       SC2_MAIN_WINDOW;
            XC_OUPUT_CFG stOutputCfg;

            MApi_XC_Get_OuputCfg(&stOutputCfg, eWin);

			u16H_CapSize = stOutputCfg.u16Height;
			u16V_CapSize = stOutputCfg.u16Width;
        }



		// Setup HD flag
		if(u16H_CapSize >= 1280 && u16V_CapSize >= 720)
		{
			MApi_XC_Sys_SetSrcIsHD( TRUE, eWindow );
        	#if ( ENABLE_OD )
			MDrv_FrontOverDriver();
        	#endif
		}
		else
		{
			MApi_XC_Sys_SetSrcIsHD( FALSE, eWindow );
        	#if ( ENABLE_OD )
			MDrv_BackOverDriver();
        	#endif
		}
	}
}


