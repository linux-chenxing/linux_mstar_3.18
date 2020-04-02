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
#ifndef MDRV_SCA_MODEPARSE_H
#define MDRV_SCA_MODEPARSE_H


#define ENABLE_RGB_SUPPORT_85HZ     0
#define SUPPORT_EURO_HDTV           0
#define ENABLE_VGA_EIA_TIMING       0
#define ENABLE_ADC_BY_SOURCE        0
#define ENABLE_OD                   0
//------------------------------------------------------------------------------
//  struct & enum
//------------------------------------------------------------------------------
typedef enum
{
    E_PCMODE_NOSYNC = 0,                ///< Input timing stable, no input sync detected
    E_PCMODE_STABLE_SUPPORT_MODE,       ///< Input timing stable, has stable input sync and support this timing
    E_PCMODE_STABLE_UN_SUPPORT_MODE,    ///< Input timing stable, has stable input sync but this timing is not supported
    E_PCMODE_UNSTABLE,                  ///< Timing change, has to wait InfoFrame if HDMI input
} PCMODE_SyncStatus;

typedef struct
{
    PCMODE_SyncStatus eCurrentState;  ///< timing monitor state
    // Mode parse result
    XC_MODEPARSE_RESULT eModeParseResult;           ///< the result after mode parse
    XC_MODEPARSE_INPUT_INFO sModeParseInputInfo;    ///< prepare the input info for mode parse
} PCMode_Info;



typedef enum
{
    MD_640x350_70,  // 0
#if ENABLE_RGB_SUPPORT_85HZ
    MD_640x350_85,
#endif //#if ENABLE_RGB_SUPPORT_85HZ
    MD_640x350_60,  // 1
    MD_720x400_70,  // 2
#if ENABLE_RGB_SUPPORT_85HZ
    MD_640x400_85,
#endif // #if ENABLE_RGB_SUPPORT_85HZ
    MD_640x400_70,    // 3
#if ENABLE_RGB_SUPPORT_85HZ
    MD_720x400_85,
#endif // #if ENABLE_RGB_SUPPORT_85HZ
    MD_640x480_60,    // 4
    MD_640x480_66,    // 5
    MD_640x480_72,    // 6
    MD_640x480_75,    // 7
#if ENABLE_RGB_SUPPORT_85HZ
    MD_640x480_85,
#endif // #if ENABLE_RGB_SUPPORT_85HZ
    MD_800x600_56,    // 8
    MD_800x600_60,    // 9
    MD_800x600_72,    //10
    MD_800x600_75,  // 11
#if ENABLE_RGB_SUPPORT_85HZ
    MD_800x600_85,
#endif // #if ENABLE_RGB_SUPPORT_85HZ
    MD_832x624_75,  // 12
    MD_848x480_59,    // 13
#if ENABLE_RGB_SUPPORT_85HZ
    MD_1024x768_43I,
#endif // #if ENABLE_RGB_SUPPORT_85HZ
    MD_1024x768_60, // 14
    MD_1024x768_70, // 15
    MD_1024x768_75, // 16
#if ENABLE_RGB_SUPPORT_85HZ
    MD_1024x768_85,
#endif //#if ENABLE_RGB_SUPPORT_85HZ
    MD_1152x864_60, // 17
    MD_1152x864_70, // 18
    MD_1152x864_75, // 19
#if ENABLE_RGB_SUPPORT_85HZ
    MD_1152x864_85,
#endif // #if ENABLE_RGB_SUPPORT_85HZ
    MD_1152x870_75, // 20
    MD_1280x768_60, // 21
    MD_1280x768_75, // 22
    MD_1280x960_60, // 23
    MD_1280x960_75, // 24
#if ENABLE_RGB_SUPPORT_85HZ
    MD_1280x960_85,
#endif // #if ENABLE_RGB_SUPPORT_85HZ
    MD_1280x1024_60, // 25
    MD_1280x1024_59, // 26
    MD_1280x1024_72, // 27
    MD_1280x1024_75, // 28
    MD_1280x1024_76, // 29
#if ENABLE_RGB_SUPPORT_85HZ
    MD_1280x1024_85,
#endif // #if ENABLE_RGB_SUPPORT_85HZ
    MD_1600x1200_59, // 30
    MD_1600x1200_60, // 31
    MD_1600x1200_65, // 32
    MD_1600x1200_70, // 33
    MD_1600x1200_75, // 34
    MD_1920x1200_59, // 35
    MD_1920x1200_60, // 36
    MD_1920x1200_65, // 37
    MD_1920x1200_75, // 38
    MD_1400x1050_59, // 39
    MD_1400x1050_60, // 40
    MD_1400x1050_75, // 41
    MD_1440x900_60_RB,  // 42
    MD_1440x900_75,  // 43
    MD_1680x1050_59, // 44
    MD_1680x1050_60, // 45
    MD_1680x1050_75, // 46


    /*++ Component Mode ++*/
    MD_720x480_60I,  // 47
    MD_720x480_60P,  // 48
    MD_720x576_50I,  // 49
    MD_720x576_50P,  // 50
    MD_1280x720_50P, // 51
    MD_1280x720_60P, // 52
    MD_1920x1080_50I,// 53
    MD_1920x1080_60I,// 54
#if (SUPPORT_EURO_HDTV)
    MD_1920x1080_50I_EURO,
#endif
    MD_1920x1080_24P, // 55
    MD_1920x1080_25P,//56
    MD_1920x1080_30P, // 56
    MD_1920x1080_50P, // 57
    MD_1920x1080_60P, // 58
    MD_720x480_60I_P, // patch for DVD 480i -> Sherwood -> component output -> OSD unsupport mode
    /*-- Component Mode --*/
    MD_1152x870_60,   // 59
    MD_1280x800_60,      // 60
    MD_1280x800_75,      // 61
    MD_1600x1024_60,  // 62
    MD_1600x1024_75,  // 63
    MD_1600x900_60,      // 64
    MD_1600x900_75,      // 65
    MD_1280x720_75,      // 66
    MD_640x480_57,    // 67
    MD_1360x768_60,      // 68
    MD_1600x1200_51,  // 69
    MD_848x480_60,    // 70
    MD_1280x768_50,      // 71
    MD_1366x768_60,   // 72
    MD_720X576_60,    // 73
    MD_864X648_60,    // 74
    MD_1920X1080_60,  // 75
    MD_720X480_60,    // 76
    MD_1440x900_60,  // 77
    MD_1280x768_60_RB,  // 78
    MD_1280x720_25P, // 79
    MD_1280x720_30P, // 80
    MD_1920X1080_60S,  // 79
    MD_1920X1080_50VESA,  // 80
    MD_STD_MODE_MAX_INDEX, // 81
    YPBPR_MD_START = MD_720x480_60I,
    YPBPR_MD_END = MD_1920x1080_60P,
    //PC_MD_END = YPBPR_MD_START - 1
}EN_MODE_TYPE;

/// PC ADC Mode setting type
typedef struct
{
    U8 u8ModeIndex;             ///< mode index

    // program
    U16 u16HorizontalStart;     ///< horizontal start
    U16 u16VerticalStart;       ///< vertical start
    U16 u16HorizontalTotal;     ///< ADC horizontal total
    U16 u16Phase;                 ///< ADC phase
    U8 u8AutoSign;              ///< Already after auto tuned or not

    // from mode table
    U8 u8Resolution;
    U16 u16DefaultHStart;       ///< default horizontal start
    U16 u16DefaultVStart;       ///< default Vertical start
    U16 u16DefaultHTotal;       ///< default horizontal total
    U8 u8SyncStatus;            ///< sync status
}MS_PCADC_MODESETTING_TYPE;


typedef void (*MS_MODEPARSE_CBF)(MS_S32);

typedef struct {
    MS_MODEPARSE_CBF         pfSignalStableToUnstable;
    MS_MODEPARSE_CBF         pfSignalUnstableToStable_TimingUnsupported;
    MS_MODEPARSE_CBF         pfSignalUnstableToStable_TimingSupported_ModeSet;
    MS_MODEPARSE_CBF         pfSignalUnstableToStable_TimingSupported_ModeUnset;
    MS_MODEPARSE_CBF         pfSignalUnstableToNoSync;
    MS_MODEPARSE_CBF         pfSignalNoSyncToUnstable;
}MS_MODEPARSE_INITDATA;

// !! add for external calibratoin, 20120726. !!//
// ADC setting index
typedef enum _AdcSetIndexType
{
    ADC_SET_VGA,
    ADC_SET_YPBPR_SD,
    ADC_SET_YPBPR_HD,
#if 0 ////(ENABLE_ADC_BY_SOURCE ==1)
    #if(INPUT_YPBPR_VIDEO_COUNT>=2)
    ADC_SET_YPBPR2_SD,
    ADC_SET_YPBPR2_HD,
    #endif
    #if(INPUT_YPBPR_VIDEO_COUNT>=3)
    ADC_SET_YPBPR3_SD,
    ADC_SET_YPBPR3_HD,
    #endif
    ADC_SET_SCART_RGB,
#endif
    ADC_SET_NUMS
}E_ADC_SET_INDEX;


/// XC - ADC setting
typedef struct
{
    APIXC_AdcGainOffsetSetting stAdcGainOffsetSetting;
    U8 u8AdcCalOK;
} MS_ADC_SETTING;


typedef struct
{
    MS_ADC_SETTING g_AdcSetting[ADC_SET_NUMS];
} MS_GENSETTING;


//------------------------------------------------------------------------------
//  Export Functions
//------------------------------------------------------------------------------
#ifdef INTERFACE
#undef INTERFACE
#endif

#ifdef MDRV_SCA_MODEPARSE_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE MS_PCADC_MODESETTING_TYPE g_PcadcModeSetting[MAX_WINDOW];

//------------------------------------------------------------------------------
INTERFACE MS_U16 MApi_PCMode_Get_HResolution(SCALER_WIN eWindow,MS_BOOL IsYpbprOrVga);
INTERFACE MS_U16 MApi_PCMode_Get_VResolution(SCALER_WIN eWindow,MS_BOOL IsYpbprOrVga);

INTERFACE void MApi_PCMode_Init( const MS_MODEPARSE_INITDATA* ptModeParseInitData);
INTERFACE void MApi_PCMode_Reset( SCALER_WIN eWindow);
INTERFACE PCMODE_SyncStatus MApi_PCMode_GetCurrentState(SCALER_WIN eWindow);
INTERFACE MS_U8 MApi_PCMode_Get_Mode_Idx(SCALER_WIN eWindow);
INTERFACE XC_MODEPARSE_RESULT MApi_PC_MainWin_Handler(INPUT_SOURCE_TYPE_t src, SCALER_WIN eWindow);
INTERFACE void MApi_PCMode_RunSelfAuto(SCALER_WIN eWindow);
INTERFACE MS_BOOL MApi_PCMode_Enable_SelfAuto(MS_BOOL EnableAuto, SCALER_WIN eWindow);
INTERFACE void MApi_PCMode_SetTimingModeDB(MS_PCADC_MODETABLE_TYPE* pstInputModeDB, MS_U8 u8NumOfDBItem);

INTERFACE void MApi_Scaler_MainWindowOnOffEventHandler(INPUT_SOURCE_TYPE_t src, void* para);
INTERFACE void MApi_Scaler_MainWindowSyncEventHandler(INPUT_SOURCE_TYPE_t src, void* para);

INTERFACE MS_BOOL MApi_PCMode_LoadDefualtTable(SCALER_WIN eWindow, MS_U8 u8ModeIdx);
INTERFACE void MApi_PCMode_GetModeTbl(MS_U8 u8ModeIdx, MS_PCADC_MODETABLE_TYPE *sModeTbl);
INTERFACE MS_PCADC_MODESETTING_TYPE MApi_PCMode_Get_ModeInfo(SCALER_WIN eWindow);

INTERFACE MS_BOOL MApp_YPbPr_Setting_Auto(XC_Auto_CalibrationType type,SCALER_WIN eWindow, MS_ADC_SETTING *pstMsAdcSetting);
INTERFACE MS_BOOL MApp_RGB_Setting_Auto(XC_Auto_CalibrationType type,SCALER_WIN eWindow, MS_ADC_SETTING *pstMsAdcSetting);
INTERFACE void    MApp_Scaler_CheckHDMode(SCALER_WIN eWindow);
INTERFACE MS_BOOL MApi_IsSrcHasSignal(SCALER_WIN eWindow);
INTERFACE MS_BOOL MApp_SetADCSetting(E_ADC_SET_INDEX eAdcIndex, const MS_ADC_SETTING *pstMsAdcSetting);
INTERFACE MS_BOOL MApi_XC_Sys_IsSrcHD(SCALER_WIN eWindow);
INTERFACE void    MApi_PCMode_SetUserModeSetting(MS_U8 u8ModeIdx, MS_PCADC_MODESETTING_TYPE stModeSetting);
INTERFACE void    MApi_PCMode_Init_UserModeSetting(void);
INTERFACE MS_BOOL MApi_PCMode_LoadUserModeSetting(SCALER_WIN eWindow, MS_U8 u8ModeIndex);

#undef INTERFACE

#endif

