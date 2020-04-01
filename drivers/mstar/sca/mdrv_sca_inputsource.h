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
#ifndef MDRV_SCA_INPUTSOURCE_H
#define MDRV_SCA_INPUTSOURCE_H

//------Input Source Mux--------------------------------------------------------

#define INPUT_AV_VIDEO_COUNT            1
#define INPUT_SV_VIDEO_COUNT            1
#define INPUT_YPBPR_VIDEO_COUNT         1
#define INPUT_SCART_VIDEO_COUNT         0
#define INPUT_HDMI_VIDEO_COUNT          0
#define ENABLE_SCART_VIDEO              0


#define INPUT_VGA_MUX               INPUT_PORT_ANALOG0
#define INPUT_VGA_SYNC_MUX          INPUT_PORT_ANALOG0_SYNC
#define INPUT_YPBPR_MUX             INPUT_PORT_ANALOG0
#define INPUT_AV_YMUX				INPUT_PORT_YMUX_CVBS0
#define INPUT_AV2_YMUX              INPUT_PORT_YMUX_CVBS1
#define INPUT_AV3_YMUX              INPUT_PORT_YMUX_CVBS2
#define INPUT_SV_YMUX               INPUT_PORT_YMUX_CVBS2
#define INPUT_SV_CMUX               INPUT_PORT_CMUX_CVBS1

#define INPUT_TV_YMUX               INPUT_PORT_YMUX_CVBS3 //INPUT_PORT_YMUX_CVBS2
#define INPUT_SV2_YMUX              INPUT_PORT_NONE_PORT
#define INPUT_SV2_CMUX              INPUT_PORT_NONE_PORT
#define INPUT_SCART_YMUX            INPUT_PORT_YMUX_CVBS3
#define INPUT_SCART_RGBMUX          INPUT_PORT_ANALOG2
#define INPUT_SCART_FB_MUX          SCART_FB1
#define INPUT_SCART2_YMUX           INPUT_PORT_NONE_PORT
#define INPUT_SCART2_RGBMUX         INPUT_PORT_NONE_PORT
#define INPUT_SCART2_FB_MUX         SCART_FB2
//------Input Source Mux--------------------------------------------------------

#define DTV_CROP_ALIGN_X            8
#define DTV_CROP_ALIGN_Y            2
#define MEDIA_CROP_ALIGN_X          8
#define MEDIA_CROP_ALIGN_Y          2
#define DIGITAL_CROP_ALIGN_X        1
#define DIGITAL_CROP_ALIGN_Y        2
#define ANALOG_CROP_ALIGN_X         1
#define ANALOG_CROP_ALIGN_Y         2

#define DEFAULT_SCREEN_UNMUTE_TIME  (300)

//------------------------------------------------------------------------------
#define ENABLE_PWS 0
//------------------------------------------------------------------------------
//  struct & enum
//------------------------------------------------------------------------------

typedef enum
{
    UI_INPUT_SOURCE_RGB,
    UI_INPUT_SOURCE_TV,
    UI_INPUT_SOURCE_ATV,

    // AV <-> CVBS
#if (INPUT_AV_VIDEO_COUNT >= 1)
    UI_INPUT_SOURCE_AV,
#endif
#if (INPUT_AV_VIDEO_COUNT >= 2)
    UI_INPUT_SOURCE_AV2,
#endif
#if (INPUT_AV_VIDEO_COUNT >= 3)
    UI_INPUT_SOURCE_AV3,
#endif

    // S-video
#if (INPUT_SV_VIDEO_COUNT >= 1)
    UI_INPUT_SOURCE_SVIDEO,
#endif
#if (INPUT_SV_VIDEO_COUNT >= 2)
    UI_INPUT_SOURCE_SVIDEO2,
#endif
#if (INPUT_SV_VIDEO_COUNT >= 3)
    UI_INPUT_SOURCE_SVIDEO3,
#endif

    // Component <-> YPBPR
#if (INPUT_YPBPR_VIDEO_COUNT >= 1)
    UI_INPUT_SOURCE_COMPONENT,
#endif
#if (INPUT_YPBPR_VIDEO_COUNT >= 2)
    UI_INPUT_SOURCE_COMPONENT2,
#endif
#if (INPUT_YPBPR_VIDEO_COUNT >= 3)
    UI_INPUT_SOURCE_COMPONENT3,
#endif

    // DTV
    UI_INPUT_SOURCE_DTV,
    UI_INPUT_SOURCE_NUM,
    UI_INPUT_SOURCE_NONE = UI_INPUT_SOURCE_NUM,
}E_UI_INPUT_SOURCE;


typedef enum
{
    E_VE_MUTE_INIT  = 0x00,
    E_VE_MUTE_GEN   = 0x01,        // mute VE for general purpose
    E_VE_MUTE_BLOCK = 0x02,        // mute VE for block
    E_VE_MUTE_RATING = 0x04,       // mute VE for rating
    E_VE_MUTE_CI_PLUS = 0x08,      // mute VE for CI+
    E_VE_MUTE_INVALID = 0xFF,      // Invalid VE mute state for init
}E_VE_MUTE_STATUS;


typedef struct
{
    MS_BOOL bInterlace;
    MS_U16  u16InputVFreq;
    MS_U16  u16InputVTotal;
}MS_TIMING_CONFIG;


typedef struct
{
    U16 u16H_CapStart;          ///< Capture window H start
    U16 u16V_CapStart;          ///< Capture window V start

    U8 u8HCrop_Left;    // H Crop Left
    U8 u8HCrop_Right;    // H crop Right
    U8 u8VCrop_Up;      // V Crop Up
    U8 u8VCrop_Down;      // V Crop Down
} MS_VIDEO_Window_Info;


typedef enum
{
    MS_USER_DISPLAY_YPBPR      = 0,
    MS_USER_DISPLAY_CVBS_NTSC  = 1,
    MS_USER_DISPLAY_CVBS_PAL   = 2,
    MS_USER_DISPLAY_VGA        = 3,
    MS_USER_DISPLAY_NUM,
}MS_USER_DISPLAY_TYPE;

typedef struct
{
    MS_BOOL bEn;
    MS_U16 u16H_Overscan;
    MS_U16 u16V_Overscan;
    MS_S16 s16H_Offset;
    MS_S16 s16V_Offset;
}MS_USER_DISPLAY_CONFIG;

typedef struct
{
    MS_U16  u16x;
    MS_U16  u16y;
    MS_U16  u16Width;
    MS_U16  u16Height;
    MS_U16  u16Vtotal;
    MS_U16  u16VFreq;
    MS_BOOL bInterlace;
}MS_INPUT_TIMING_CONFIG;

//------------------------------------------------------------------------------
//  Export Functions & Variable
//------------------------------------------------------------------------------

#ifdef INTERFACE
#undef INTERFACE
#endif

#ifdef MDRV_SCA_INPUTSOURCE_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE void MApi_InputSource_SwitchSource ( INPUT_SOURCE_TYPE_t enUIInputSourceType, SCALER_WIN eWindow );
INTERFACE MS_BOOL MApi_Scaler_SetWindow(MS_WINDOW_TYPE *ptSrcWin,  MS_WINDOW_TYPE *ptCropWin, MS_WINDOW_TYPE *ptDstWin, INPUT_SOURCE_TYPE_t enInputSourceType,SCALER_WIN eWindow);
INTERFACE void MApi_Scaler_SetTiming(INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow, MS_TIMING_CONFIG *pstTimingCfg);
INTERFACE void MApi_Set_Window_And_Timing(INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow);
INTERFACE void MApi_InputSource_InputPort_Mapping(INPUT_SOURCE_TYPE_t src_ids , E_MUX_INPUTPORT* port_ids , MS_U8* u8port_count );
INTERFACE void MApi_SetSourceType(INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow);
INTERFACE void MApi_SetColor( INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow );
INTERFACE void MApi_SetCVBSMute( MS_BOOL bEn , E_VE_MUTE_STATUS eMuteStatus , INPUT_SOURCE_TYPE_t enInputPortType,E_DEST_TYPE OutputPortType);
INTERFACE MS_BOOL MApi_SetVE(INPUT_SOURCE_TYPE_t src,E_DEST_TYPE enOutputType, MS_VE_VIDEOSYS enVideoSys, MS_VE_Set_Mode_Type SetModeType);
INTERFACE void MApi_Set_MVOP(void);
INTERFACE void MApi_VE_SourceSwitch(INPUT_SOURCE_TYPE_t src, E_DEST_TYPE enOutputType);
INTERFACE void MApi_Set_Window_OnOFF(MS_BOOL bMainEn, MS_BOOL bSubEn);
INTERFACE void MApi_Set_SC1_LoadPQ(MS_U8 u8value);
INTERFACE void MApi_InputSource_Set_User_Display_Config(SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputPortType, MS_U16 u16H_Overscan, MS_U16 u16V_Overscan, MS_S16 s16H_Offset, MS_S16 s16V_Offset, MS_BOOL bCVBS_NTSC, MS_BOOL bEn);
INTERFACE void MApi_Scaler_set_DisplayMute_Color(XC_FREERUN_COLOR enColor, SCALER_WIN eWindow);
INTERFACE XC_FREERUN_COLOR MApi_Scaler_Get_DisplayMute_ColorType(SCALER_WIN eWindow);
INTERFACE void MApi_InputSource_SetCameraTiming(MS_INPUT_TIMING_CONFIG stTiming);
INTERFACE void MApi_InputSrouce_GetCameraTiming(MS_INPUT_TIMING_CONFIG *pTiming);

#undef INTERFACE
#endif
