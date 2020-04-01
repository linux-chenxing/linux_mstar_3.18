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
#ifndef MDRV_SCA_COLOR_H
#define MDRV_SCA_COLOR_H

//------------------------------------------------------------------------------
// Default Contrast & Saturation & Hue & Sharpness Value
//------------------------------------------------------------------------------
// Contrast
#define ACE_Contrast_DefaultValue_HDTV          128
#define ACE_Contrast_DefaultValue_SDTV          128
#define ACE_Contrast_DefaultValue_ATV           128
#define ACE_Contrast_DefaultValue_AV            128
#define ACE_Contrast_DefaultValue_SV            128
#define ACE_Contrast_DefaultValue_YPBPR_HD      128
#define ACE_Contrast_DefaultValue_YPBPR_SD      128
#define ACE_Contrast_DefaultValue_HDMI_HD       128
#define ACE_Contrast_DefaultValue_HDMI_SD       128
#define ACE_Contrast_DefaultValue_VGA_DVI       128

// Saturation
#define ACE_Saturation_DefaultValue_HDTV        128
#define ACE_Saturation_DefaultValue_SDTV        128
#define ACE_Saturation_DefaultValue_ATV         128
#define ACE_Saturation_DefaultValue_AV          128
#define ACE_Saturation_DefaultValue_SV          128
#define ACE_Saturation_DefaultValue_YPBPR_HD    128
#define ACE_Saturation_DefaultValue_YPBPR_SD    128
#define ACE_Saturation_DefaultValue_HDMI_HD     128
#define ACE_Saturation_DefaultValue_HDMI_SD     128
#define ACE_Saturation_DefaultValue_VGA_DVI     128

// Sharpness
#define ACE_Sharpness_DefaultValue_HDTV         31
#define ACE_Sharpness_DefaultValue_SDTV         23
#define ACE_Sharpness_DefaultValue_ATV          31
#define ACE_Sharpness_DefaultValue_AV           44
#define ACE_Sharpness_DefaultValue_SV           44
#define ACE_Sharpness_DefaultValue_YPBPR_HD     31
#define ACE_Sharpness_DefaultValue_YPBPR_SD     31
#define ACE_Sharpness_DefaultValue_HDMI_HD      32
#define ACE_Sharpness_DefaultValue_HDMI_SD      32
#define ACE_Sharpness_DefaultValue_VGA_DVI      32

// Hue
#define ACE_Hue_DefaultValue_HDTV               50
#define ACE_Hue_DefaultValue_SDTV               50
#define ACE_Hue_DefaultValue_ATV                50
#define ACE_Hue_DefaultValue_AV                 50
#define ACE_Hue_DefaultValue_SV                 50
#define ACE_Hue_DefaultValue_YPBPR              50
#define ACE_Hue_DefaultValue_HDMI               50
#define ACE_Hue_DefaultValue_VGA_DVI            50


//------------------------------------------------------------------------------
// Color Temp
#define INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R      0x80
#define INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G      0x80
#define INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B      0x80

#define INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R    0x80
#define INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G    0x80
#define INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B    0x80

#define INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R      0x80
#define INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G      0x80
#define INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B      0x80

#define INIT_VIDEO_COLOR_BRIGHTNESS_USER_R      0x80
#define INIT_VIDEO_COLOR_BRIGHTNESS_USER_G      0x80
#define INIT_VIDEO_COLOR_BRIGHTNESS_USER_B      0x80


#define INIT_VIDEO_COLOR_TEMP_COOL_R            128//126//111//0x80
#define INIT_VIDEO_COLOR_TEMP_COOL_G            128//128//106//0x80
#define INIT_VIDEO_COLOR_TEMP_COOL_B            128//110//128//0x80

#define INIT_VIDEO_COLOR_TEMP_NORMAL_R          128//118//0x80
#define INIT_VIDEO_COLOR_TEMP_NORMAL_G          128//109//0x80
#define INIT_VIDEO_COLOR_TEMP_NORMAL_B          128//128//0x80

#define INIT_VIDEO_COLOR_TEMP_WARM_R            0x80
#define INIT_VIDEO_COLOR_TEMP_WARM_G            0x80
#define INIT_VIDEO_COLOR_TEMP_WARM_B            0x80

#define INIT_VIDEO_COLOR_TEMP_USER_R            0x80
#define INIT_VIDEO_COLOR_TEMP_USER_G            0x80
#define INIT_VIDEO_COLOR_TEMP_USER_B            0x80

#define INIT_VIDEO_COLOR_TEMP_USER_R_SCALE      0x80 //must refer INIT_VIDEO_COLOR_TEMP_USER_R, for example: 0x80 -> 0x32
#define INIT_VIDEO_COLOR_TEMP_USER_G_SCALE      0x80 //must refer INIT_VIDEO_COLOR_TEMP_USER_G, for example: 0x80 -> 0x32
#define INIT_VIDEO_COLOR_TEMP_USER_B_SCALE      0x80 //must refer INIT_VIDEO_COLOR_TEMP_USER_B, for example: 0x80 -> 0x32

////////////////////////////////////////////////////////////////////////////////
// Adjust Color
////////////////////////////////////////////////////////////////////////////////
#define MAX_VIDEO_CONTRAST      0xA0
#define MIN_VIDEO_CONTRAST      0x00
#define MAX_PC_CONTRAST         0xA0
#define MIN_PC_CONTRAST         0x50

#define MAX_VIDEO_BRIHTNESS     0xFF
#define MIN_VIDEO_BRIHTNESS     0x00

#define MAX_PC_BRIGHTNESS       0xFE
#define MIN_PC_BRIGHTNESS       0x12

#define MAX_VIDEO_SATURATION    0xFF
#define MIN_VIDEO_SATURATION    0

#define MAX_VIDEO_HUE           100
#define MIN_VIDEO_HUE           0

#define MAX_VIDEO_SHARPNESS     0x3F
#define MIN_VIDEO_SHARPNESS     0

#define MAX_PC_SHARPNESS        0x3F
#define MIN_PC_SHARPNESS        0

#define MAX_VIDEO_BLACKLEVEL    0x2F
#define MIN_VIDEO_BLACKLEVEL    0x00

#define INIT_SUB_BRIGHTNESS_DATA    128
#define INIT_SUB_CONTRAST_DATA      128

#define SUB_BRIGHTNESS_MIN      (SUB_BRIGHTNESS_BYPASS - 50)
#define SUB_BRIGHTNESS_BYPASS   128
#define SUB_BRIGHTNESS_MAX      (SUB_BRIGHTNESS_BYPASS + 50)

#define SUB_CONTRAST_MIN        (SUB_CONTRAST_BYPASS - 50)
#define SUB_CONTRAST_BYPASS     128
#define SUB_CONTRAST_MAX        (SUB_CONTRAST_BYPASS + 50)

#define BRIGHTNESS_BYPASS       128


//------------------------------------------------------------------------------
//          DLC
//------------------------------------------------------------------------------
#define DLC_PURE_IMAGE_DLC_CURVE            0
#define DLC_PURE_IMAGE_LINEAR_CURVE         1
#define DLC_PURE_IMAGE_DO_NOTHING           2

#define DLC_C_GAIN_CONTROL                  1

#define DLC_ALGORITHM_OLD                   0
#define DLC_ALGORITHM_NEW                   1

#define DLC_PARAMETER_ALGORITHM_MODE       DLC_ALGORITHM_NEW //DLC_ALGORITHM_OLD//DLC_ALGORITHM_NEW

#if DLC_PARAMETER_ALGORITHM_MODE   //for NEW DLC ALGORITHM
#define DLC_PARAMETER_L_L_U                3 // 12
#define DLC_PARAMETER_L_L_D                0 // 5
#define DLC_PARAMETER_L_H_U                3 // 4
#define DLC_PARAMETER_L_H_D                3
#define DLC_PARAMETER_S_L_U                128
#define DLC_PARAMETER_S_L_D                128
#define DLC_PARAMETER_S_H_U                128
#define DLC_PARAMETER_S_H_D                127 // 192
#define DLC_PARAMETER_PURE_IMAGE_MODE      E_XC_DLC_PURE_IMAGE_DO_NOTHING//E_XC_DLC_PURE_IMAGE_LINEAR_CURVE//E_XC_DLC_PURE_IMAGE_DO_NOTHING
#define DLC_PARAMETER_LEVEL_LIMIT          0 // 1
#define DLC_PARAMETER_AVG_DELTA            15 // 12
#define DLC_PARAMETER_AVG_DELTA_STILL      0
#define DLC_PARAMETER_FAST_ALPHA_BLENDING  31 //24
#define DLC_PARAMETER_Y_AVG_THRESHOLD_L    5 //32  // 0
#define DLC_PARAMETER_Y_AVG_THRESHOLD_H    200 //155//180 // 180 // 128
#define DLC_PARAMETER_BLE_POINT            48
#define DLC_PARAMETER_WLE_POINT            48
#define DLC_PARAMETER_ENABLE_BLE           0
#define DLC_PARAMETER_ENABLE_WLE           0

#define DLC_CGC_ENABLE                      0
#define DLC_CGC_CGAIN_OFFSET                0 // 0x05 // 0x08
#define DLC_CGC_CGAIN_LIMIT_H               0x00 // 0x38
#define DLC_CGC_CGAIN_LIMIT_L               0x00 //0x08
#define DLC_CGC_YC_SLOPE                    0x01 // 0x04 // 0x0C
#define DLC_CGC_Y_TH                        0x08 // 0x03 // 0x01

#else  //for OLD DLC ALGORITHM
#define DLC_PARAMETER_L_L_U                4; // default value: 10
#define DLC_PARAMETER_L_L_D                0; // default value: 10
#define DLC_PARAMETER_L_H_U                0; // default value: 10
#define DLC_PARAMETER_L_H_D                0; // default value: 10
#define DLC_PARAMETER_S_L_U                64; // default value: 128 (0x80)
#define DLC_PARAMETER_S_L_D                12; // default value: 128 (0x80)
#define DLC_PARAMETER_S_H_U                128; // default value: 128 (0x80)
#define DLC_PARAMETER_S_H_D                192; // default value: 128 (0x80)
#define DLC_PARAMETER_PURE_IMAGE_MODE      DLC_PURE_IMAGE_LINEAR_CURVE; // Compare difference of max and min bright
#define DLC_PARAMETER_LEVEL_LIMIT          3; // n = 0 ~ 4 => Limit n levels => ex. n=2, limit 2 level 0xF7, 0xE7
#define DLC_PARAMETER_AVG_DELTA            0; // n = 0 ~ 50, default value: 12
#define DLC_PARAMETER_AVG_DELTA_STILL      0; // n = 0 ~ 15 => 0: disable still curve, 1 ~ 15: enable still curve
#define DLC_PARAMETER_FAST_ALPHA_BLENDING  31; // min 17 ~ max 32
#define DLC_PARAMETER_Y_AVG_THRESHOLD_L    12; // default value: 0
#define DLC_PARAMETER_Y_AVG_THRESHOLD_H    210; // default value: 128
#define DLC_PARAMETER_BLE_POINT            48; // n = 24 ~ 64, default value: 48
#define DLC_PARAMETER_WLE_POINT            48; // n = 24 ~ 64, default value: 48
#define DLC_PARAMETER_ENABLE_BLE           0; // 1: enable; 0: disable
#define DLC_PARAMETER_ENABLE_WLE           0; // 1: enable; 0: disable

#define DLC_CGC_ENABLE                     0; // 1: enable; 0: disable
#define DLC_CGC_CGAIN_OFFSET               0x08; // -31 ~ 31 (bit7 = minus, ex. 0x88 => -8)
#define DLC_CGC_CGAIN_LIMIT_H              0x38; // 0x00~0x6F
#define DLC_CGC_CGAIN_LIMIT_L              0x00; // 0x00~0x10
#define DLC_CGC_YC_SLOPE                   0x0C; // 0x01~0x20
#define DLC_CGC_Y_TH                       0x01; // 0x01~0x0A

#endif

#define DLC_PARAMETER_CURVE_MODE           E_XC_DLC_CURVE_MODE_BY_MIX//E_XC_DLC_PURE_IMAGE_DO_NOTHING
#define DLC_PARAMETER_Y_AVG_THRESHOLD_M    70 //99//64 // 180 // 128
#define DLC_PARAMETER_CURVE_MIXALPHA       72 //55//38 // 180 // 128

#define DLC_PARAMETER_SepPoint_H           188
#define DLC_PARAMETER_SepPoint_L           60
#define DLC_PARAMETER_BLEStartPointTH      520 //768
#define DLC_PARAMETER_BLEEndPointTH        320 //420
#define DLC_PARAMETER_DLC_CurveDiff_L_TH   56
#define DLC_PARAMETER_DLC_CurveDiff_H_TH   148
#define DLC_PARAMETER_BLE_Slop_Point1      1032 //1032
#define DLC_PARAMETER_BLE_Slop_Point2      1172 //1196
#define DLC_PARAMETER_BLE_Slop_Point3      1242 //1312
#define DLC_PARAMETER_BLE_Slop_Point4      1324 //1460
#define DLC_PARAMETER_BLE_Slop_Point5      1396 //1630
#define DLC_PARAMETER_BLE_Dark_Slop_Min    1200
#define DLC_PARAMETER_DLC_CurveDiff_Coring_TH 2
#define DLC_PARAMETER_DLC_FAST_ALPHA_BLENDING_MIN 1
#define DLC_PARAMETER_DLC_FAST_ALPHA_BLENDING_MAX 128
#define DLC_PARAMETER_DLC_FlICKER_ALPHA    96
#define DLC_PARAMETER_DLC_YAVG_L_TH        56
#define DLC_PARAMETER_DLC_YAVG_H_TH        136

#define DLC_PARAMETER_DLC_DiffBase_L        4
#define DLC_PARAMETER_DLC_DiffBase_M        14
#define DLC_PARAMETER_DLC_DiffBase_H        20

//------------------------------------------------------------------------------
//  struct & enum
//------------------------------------------------------------------------------
typedef struct
{
    U8 ucX;
    U8 ucY;
}T_MS_NONLINEAR_POINT;

typedef struct
{
    T_MS_NONLINEAR_POINT stPoint0;
    T_MS_NONLINEAR_POINT stPoint1;
    T_MS_NONLINEAR_POINT stPoint2;
    T_MS_NONLINEAR_POINT stPoint3;
    //T_MS_NONLINEAR_POINT stPoint4;
}T_MS_NONLINEAR_CURVE;

typedef enum{
    BRIGHTNESS_R,
    BRIGHTNESS_G,
    BRIGHTNESS_B
}EN_BRIGHTNESS_TYPE;


//////////////////////////////////////////////////////////////////////////////////////////
//  Nonlinear Setting structure type
//////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    PICTURE_CONTRAST = 0,
    PICTURE_BRIGHTNESS,
    PICTURE_SATURATION,
    PICTURE_SHARPNESS,
    PICTURE_HUE,
    PICTURE_NUM
} EN_MS_PICTURE_TYPE;


typedef struct
{
    U8 u8OSD_0;
    U8 u8OSD_25;
    U8 u8OSD_50;
    U8 u8OSD_75;
    U8 u8OSD_100;
} T_MS_USER_NONLINEAR_CURVE, *P_MS_USER_NONLINEAR_CURVE;


typedef struct
{
    T_MS_USER_NONLINEAR_CURVE DTVPictureCurve[PICTURE_NUM];
    T_MS_USER_NONLINEAR_CURVE AVPictureCurve[PICTURE_NUM];
    T_MS_USER_NONLINEAR_CURVE YPbPrPictureCurve[PICTURE_NUM];
    T_MS_USER_NONLINEAR_CURVE PCPictureCurve[PICTURE_NUM];
    T_MS_USER_NONLINEAR_CURVE BT656PictureCurve[PICTURE_NUM];
}MS_NONLINEAR_CURVE_SETTING;

//------------------------------------------------------------------------------
/// Color temperture
typedef enum
{
    COLOR_TEMP_COOL = 0,
    COLOR_TEMP_NORMAL,
    COLOR_TEMP_WARM,
    COLOR_TEMP_USER,
    COLOR_TEMP_NUM,
}MS_COLOR_TEMP_TYPE;

#define MS_COLOR_TEMP_COUNT COLOR_TEMP_NUM

typedef struct
{
    U8 cRedOffset;   ///< red offset
    U8 cGreenOffset; ///< green offset
    U8 cBlueOffset;  ///< blue offset

    U8 cRedColor;   ///< red color
    U8 cGreenColor; ///< green color
    U8 cBlueColor;  ///< blue color

    U8 cRedScaleValue;   ///< scale 100 value of red color
    U8 cGreenScaleValue; ///< scale 100 value of green color
    U8 cBlueScaleValue;  ///< scale 100 value of blue color
} T_MS_COLOR_TEMP;

typedef struct
{
    T_MS_COLOR_TEMP DTVColorTempTbl[MS_COLOR_TEMP_COUNT];
    T_MS_COLOR_TEMP AVColorTempTbl[MS_COLOR_TEMP_COUNT];
    T_MS_COLOR_TEMP YPbPrColorTempTbl[MS_COLOR_TEMP_COUNT];
    T_MS_COLOR_TEMP PCColorTempTbl[MS_COLOR_TEMP_COUNT];
    T_MS_COLOR_TEMP BT656ColorTempTbl[MS_COLOR_TEMP_COUNT];
} T_MS_WHITEBALANCE;
//------------------------------------------------------------------------------
// Sub Color
typedef enum
{
    SUB_COLOR_CONTRAST,
    SUB_COLOR_BRIGHTNESS,
}MS_SUB_COLOR_TYPE;

typedef struct
{
    U8 u8SubBrightness;            // brightness
    U8 u8SubContrast;              // contrast
} T_MS_SUB_COLOR;

//------------------------------------------------------------------------------
typedef struct
{
    U8 u8Brightness;
    U8 u8Contrast;
    U8 u8Saturation;
    U8 u8Hue;
    U8 u8Sharpness;
    MS_COLOR_TEMP_TYPE enColorTemp;
    T_MS_SUB_COLOR stSubColor;
}MS_PICTURE_COLOR_SETTING;

typedef struct
{
    MS_PICTURE_COLOR_SETTING stDTVPicture;
    MS_PICTURE_COLOR_SETTING stAVPicture;
    MS_PICTURE_COLOR_SETTING stYPbPrPicture;
    MS_PICTURE_COLOR_SETTING stPCPicture;
    MS_PICTURE_COLOR_SETTING stBT656Picture;
}MS_PICTURE_CONFIG;
//------------------------------------------------------------------------------
typedef enum
{
    MS_COLOR_YUV_TO_RGB,
    MS_COLOR_RGB_TO_RGB,
    MS_COLOR_YUV_TO_YUV,
    MS_COLOR_RGB_TO_YUV,
}MS_COLOR_TRANS_TYPE;

//------------------------------------------------------------------------------
//  Export Functions & Variable
//------------------------------------------------------------------------------

#ifdef MDRV_SCA_COLOR_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE U8 MApi_PictureContrastN100toReallyValue ( SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputSourceType, U8 u8value );
INTERFACE U8 MApi_PictureBrightnessN100toReallyValue ( SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputSourceType, U8 u8value );
INTERFACE U8 MApi_PictureHueN100toReallyValue ( SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputSourceType, U8 u8value );
INTERFACE U8 MApi_PictureSaturationN100toReallyValue ( SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputSourceType, U8 u8value );
INTERFACE U8 MApi_PictureSharpnessN100toReallyValue ( SCALER_WIN eWindow,INPUT_SOURCE_TYPE_t enInputSourceType, U8 u8value );
INTERFACE U8 MApi_GetPictureSetting(INPUT_SOURCE_TYPE_t enInputSourceType, EN_MS_PICTURE_TYPE ePictureType, SCALER_WIN eWindow);
INTERFACE T_MS_COLOR_TEMP  MApi_GetColorTempTBL(INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow);
INTERFACE U8 MApi_GetSubColor(INPUT_SOURCE_TYPE_t enInputSourceType, MS_SUB_COLOR_TYPE enType, SCALER_WIN eWindow);
INTERFACE U8 MApi_ACE_transferRGB_Bri(U8 u8Brightness, U8 u8Brightnesstype);
INTERFACE U8 MApi_ACE_transfer_Bri(U8 u8Brightness, U8 u8Brightnesstype);
INTERFACE U8 MApi_FactoryAdjBrightness(U8 u8Brightness, U8 u8SubBrightness);
INTERFACE U8 MApi_FactoryContrast(U8 u8Contrast, U8 u8SubContrast);
INTERFACE void MApi_SetPictureSetting(INPUT_SOURCE_TYPE_t enInputSourceType, EN_MS_PICTURE_TYPE ePictureType, U8 u8val, SCALER_WIN eWindow);
INTERFACE void MApi_SetColorTemp(INPUT_SOURCE_TYPE_t enInputSourceType, MS_COLOR_TEMP_TYPE enColorTempType, SCALER_WIN eWindow);
INTERFACE void MApi_SetSubColor(INPUT_SOURCE_TYPE_t enInputSourceType, MS_SUB_COLOR_TYPE enType, U8 u8val, SCALER_WIN eWindow);
INTERFACE MS_COLOR_TEMP_TYPE MApi_GetColorTemp(INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow);
INTERFACE MS_U8 mApi_DLC_GetLumaCurve(MS_U8 u8Index);
INTERFACE MS_U8 mApi_DLC_GetLumaCurve2_a(MS_U8 u8Index);
INTERFACE MS_U8 mApi_DLC_GetLumaCurve2_b(MS_U8 u8Index);
INTERFACE MS_U8 mApi_DLC_GetDlcHistogramLimitCurve(MS_U8 u8Index);
INTERFACE MS_BOOL MApi_DLC_Get_From_PQ(SCALER_WIN eWindow);
INTERFACE MS_BOOL MApi_GetColorTempTBL_From_PQ(SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputSourceType, T_MS_COLOR_TEMP *pColorTemp);

#undef INTERFACE
#endif
