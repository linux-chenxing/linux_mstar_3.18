////////////////////////////////////////////////////////////////////////////////
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
/// @file  drvACE.h
/// @brief ACE library interface header file
/// @author MStar Semiconductor Inc.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef DRV_ACE_H
#define DRV_ACE_H


#ifdef __cplusplus
extern "C"
{
#endif
/******************************************************************************/
/*                   Export variables                                                                                     */
/******************************************************************************/
typedef enum
{
    ACE_INFO_TYPE_VERSION,
    ACE_INFO_TYPE_CONTRAST,
    ACE_INFO_TYPE_R_GAIN,
    ACE_INFO_TYPE_G_GAIN,
    ACE_INFO_TYPE_B_GAIN,
    ACE_INFO_TYPE_SATURATION,
    ACE_INFO_TYPE_HUE,
    ACE_INFO_TYPE_COLOR_CORRECTION_XY_R,
    ACE_INFO_TYPE_COLOR_CORRECTION_XY_G,
    ACE_INFO_TYPE_COLOR_CORRECTION_XY_B,
    ACE_INFO_TYPE_COLOR_CORRECTION_OFFSET_R,
    ACE_INFO_TYPE_COLOR_CORRECTION_OFFSET_G,
    ACE_INFO_TYPE_COLOR_CORRECTION_OFFSET_B,

    ACE_INFO_TYPE_COMPILER_VERSION,
    ACE_INFO_TYPE_YUV_TO_RGB_MATRIX_SEL,
    ACE_INFO_TYPE_DUMMY,
} EnuACEInfoType;

typedef enum
{
    ACE_MATRIX_ID__YUV_TO_RGB,
    ACE_MATRIX_ID__COLOR_CORRECTION,
    ACE_MATRIX_ID__SRGB
}EnuAceMatrixId;

typedef enum
{
    ACE_YUV_TO_RGB_MATRIX_SDTV, // Use Std CSCM for SDTV
    ACE_YUV_TO_RGB_MATRIX_HDTV, // Use Std CSCM for HDTV
    ACE_YUV_TO_RGB_MATRIX_USER, // Use user define CSCM, specified by tUserYVUtoRGBMatrix
} E_ACE_YUVTORGBInfoType;

/// MWE border color
typedef enum
{
    MWE_BORDER_COLOR_BLACK,
    MWE_BORDER_COLOR_WHITE,
    MWE_BORDER_COLOR_BLUE,
    MWE_BORDER_COLOR_RED,
    MWE_BORDER_COLOR_GREEN,
    MWE_BORDER_COLOR_MAX,
}EN_MWE_BORDER_COLOR;

typedef struct {
    MS_U32 Xpos;
    MS_U32 Ypos;
    MS_U32 Xsize;
    MS_U32 Ysize;
}MWE_RECT;

typedef struct _PanelSizeType
{
    MS_U16 u16Pnl_Hstart;
    MS_U16 u16Pnl_Vstart;
    MS_U16 u16Pnl_Width;
    MS_U16 u16Pnl_Height;
}PanelSizeType;

typedef enum MWEType
{
    MWE_OFF,
    MWE_H_SPLIT,
    MWE_MOVE,
    MWE_ZOOM,
    MWE_NUMS,
}MWEType;

MS_U16  msACE_GetVersion(void);
void    InitACEVar(void);
void    msACEPCForceYUVtoRGB( MS_BOOL bScalerWin, MS_BOOL bEnable );
void    msAdjustPCContrast( MS_BOOL bScalerWin, MS_U8 u8Contrast );
void    msAdjustPCContrast_G( MS_BOOL bScalerWin, MS_U8 ucContrast );
void    msAdjustPCRGB( MS_BOOL bScalerWin, MS_U8 u8Red, MS_U8 u8Green, MS_U8 u8Blue );
void    msAdjustVideoContrast( MS_BOOL bScalerWin, MS_U8 u8Contrast );
void    msAdjustVideoContrast_G( MS_BOOL bScalerWin, MS_U8 ucContrast );
void    msAdjustVideoRGB( MS_BOOL bScalerWin, MS_U8 u8Red, MS_U8 u8Green, MS_U8 u8Blue);
void    msACESelectYUVtoRGBMatrix( MS_BOOL bScalerWin, MS_U8 u8Matrix, MS_S16 *psUserYUVtoRGBMatrix );
void    msACESetColorCorrectionTable( MS_BOOL bScalerWin, MS_S16 *psColorCorrectionTable );
void    msACESetPCsRGBTable( MS_BOOL bScalerWin, MS_S16 *psPCsRGBTable );
void    msAdjustVideoSaturation( MS_BOOL bScalerWin, MS_U8 u8Saturation );
void    msAdjustVideoHue( MS_BOOL bScalerWin, MS_U8 u8Hue );
MS_U16  msACE_GetInfo( MS_BOOL bWindow, EnuACEInfoType eACEInfoType );
void    msSetVideoColorMatrix( MS_BOOL bScalerWin );
void    msSetPCColorMatrix( MS_BOOL bScalerWin );
void    msACE_SetColorMatrixControl( MS_BOOL bScalerWin, MS_BOOL bEnable );
void    msACE_SetRBChannelRange( MS_BOOL bScalerWin, MS_BOOL bRange );

/******************************************************************************/
/*                   Function Prototypes                                      */
/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_GetVersion()
/// @brief \b Function \b Description:  Get version of ACE library
/// @param <IN>        \b None
/// @param <OUT>       \b None
/// @param <RET>       \b Version
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
MS_U16 MDrv_ACE_GetVersion(void);

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_Init()
/// @brief \b Function \b Description:  Initialize ACE
/// @param <IN>        \b None
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_Init(  MS_BOOL bScalerWin, MS_S16* psColorCorrectionTable, MS_S16* psPCsRGBTable );

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_PCForceYUVtoRGB()
/// @brief \b Function \b Description:  Force transfer YUV to RGB
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b bEnable
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_PCForceYUVtoRGB( MS_BOOL bScalerWin, MS_BOOL bEnable );

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_PicSetContrast()
/// @brief \b Function \b Description:  Adjust Contrast
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b u8Contrast
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_PicSetContrast( MS_BOOL bScalerWin, MS_BOOL bUseYUVSpace, MS_U8 u8Contrast );


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_PicSetContrast()
/// @brief \b Function \b Description:  Adjust Contrast only on G channel
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b u8Contrast
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_PicSetContrast_G( MS_BOOL bScalerWin, MS_BOOL bUseYUVSpace, MS_U8 u8Contrast );

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_PicSetRGB()
/// @brief \b Function \b Description:  Adjust Video RGB
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b u8Red
/// @param <IN>        \b u8Green
/// @param <IN>        \b u8Blue
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_PicSetRGB( MS_BOOL bScalerWin, MS_BOOL bUseYUVSpace, MS_U8 u8Red, MS_U8 u8Green, MS_U8 u8Blue );

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_PicSetPostRGBGain()
/// @brief \b Function \b Description:  Adjust Video Post RGB Gain
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b u8RedGain
/// @param <IN>        \b u8GreenGain
/// @param <IN>        \b u8BlueGain
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_PicSetPostRGBGain(MS_BOOL bScalerWin, MS_U16 u8RedGain, MS_U16 u8GreenGain, MS_U16 u8BlueGain);

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_GetInfo()
/// @brief \b Function \b Description:  Get ACE Info
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b ucInfoType
/// @param <OUT>       \b None
/// @param <RET>       \b wReturnValue
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
MS_U8 MDrv_ACE_GetPostRGBGain(MS_BOOL bScalerWin, MS_U16 u16ACEinfo);

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_PicSetPostRGBOffset()
/// @brief \b Function \b Description:  Adjust Video Post RGB Offset
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b u16RedOffset
/// @param <IN>        \b u16GreenOffset
/// @param <IN>        \b u16BlueOffset
/// @param <IN>        \b u8Step
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_PicSetPostRGBOffset(MS_BOOL bScalerWin, MS_U16 u16RedOffset, MS_U16 u16GreenOffset, MS_U16 u16BlueOffset, MS_U8 u8Step);
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_SelectYUVtoRGBMatrix()
/// @brief \b Function \b Description:  Select YUV to RGB Matrix
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b ucMatrix
/// @param <IN>        \b psUserYUVtoRGBMatrix
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_SelectYUVtoRGBMatrix( MS_BOOL bScalerWin, MS_U8 ucMatrix, MS_S16* psUserYUVtoRGBMatrix );

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_SetColorCorrectionTable()
/// @brief \b Function \b Description:  Set Color Correction Table
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b psColorCorrectionTable
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_SetColorCorrectionTable( MS_BOOL bScalerWin);

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_SetPCsRGBTable()
/// @brief \b Function \b Description:  Set PCs RGB Table
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b psPCsRGBTable
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_SetPCsRGBTable( MS_BOOL bScalerWin);

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_AdjustVideoSaturation()
/// @brief \b Function \b Description:  Adjust Video Saturation
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b u8Saturation
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_SetSaturation( MS_BOOL bScalerWin, MS_U8 u8Saturation );

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_SetHue()
/// @brief \b Function \b Description:  Adjust Video Hue
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b u8Hue
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_SetHue( MS_BOOL bScalerWin, MS_U8 u8Hue );

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_GetInfo()
/// @brief \b Function \b Description:  Get ACE Info
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b ucInfoType
/// @param <OUT>       \b None
/// @param <RET>       \b wReturnValue
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
MS_U16 MDrv_ACE_GetACEInfo( MS_BOOL bWindow, MS_U16 u16ACEinfo );


void MDrv_ACE_DMS( MS_BOOL bScalerWin, MS_BOOL bisATV );
void MDrv_ACE_GetColorMatrix( MS_BOOL bScalerWin, MS_U16* pu16Matrix );
void MDrv_ACE_SetColorMatrix( MS_BOOL bScalerWin, MS_U16* pu16Matrix );
void MDrv_ACE_PatchDTGColorChecker( MS_U8 u8Mode);
void MDrv_ACE_SetFleshTone( MS_BOOL bScalerWin, MS_BOOL bEn, MS_U8 u8FlashTone); ///< set flash tone
void MDrv_ACE_SetBlackAdjust( MS_BOOL bScalerWin, MS_U8 u8BlackAdjust);

void MDrv_ACE_init_riu_base( MS_U32 u32riu_base );
void MDrv_ACE_SetSharpness( MS_BOOL bScalerWin, MS_U8 u8Sharpness );
void MDrv_ACE_SetBrightness ( MS_BOOL bScalerWin, MS_U8 u8RedBrightness, MS_U8 u8GreenBrightness, MS_U8 u8BlueBrightness );
void MDrv_ACE_SetBrightnessPrecise(MS_BOOL bScalerWin, MS_U16 u16RedBrightness, MS_U16 u16GreenBrightness, MS_U16 u16BlueBrightness);
MS_BOOL MDrv_ACE_IsPostRGBGain(MS_BOOL bWindow);
MS_BOOL MDrv_ACE_IsPostRGBOffset(MS_BOOL bWindow);
MS_BOOL MDrv_XC_ACE_Set_IHC_SRAM(MS_U8 *pBuf, MS_U16 u16ByteSize);
MS_BOOL MDrv_XC_ACE_Set_ICC_SRAM(MS_U16 *pBuf, MS_U16 u16ByteSize);

//------------------------------
// MWE related
//------------------------------
void MDrv_XC_ACE_MWEinit(MS_U16 u16MWEHstart, MS_U16 u16MWEVstart, MS_U16 u16MWEWidth, MS_U16 u16MWEHeight);
void MDrv_XC_ACE_MWESetWin(MWE_RECT *pRect);
void MDrv_XC_ACE_MWESetBorder( MS_BOOL bScalerWin, MS_BOOL ben, MS_U8 u8color, MS_U16 u16_l, MS_U16 u16_r, MS_U16 u16_t, MS_U16 u16_d);
void MDrv_XC_ACE_MWEEnable(MS_BOOL bEnable, MS_BOOL bLoadFromTable);
void MDrv_XC_ACE_MWECloneVisualEffect(void);
void MDrv_ACE_3DClonePQMap(MS_BOOL bHWeaveOut, MS_BOOL bVWeaveOut);
void MDrv_XC_ACE_MWESetRegRow(MS_U32 u32Addr, MS_U8 u8Value, MS_U8 u8Mask);

//------------------------------
// DynamicnNR related
//------------------------------

MS_U8 MDrv_XC_ACE_DNR_GetMotion(void);
MS_U8 MDrv_ACE_DNR_GetCoringThreshold(MS_BOOL bScalerWin);
MS_U8 MDrv_ACE_DNR_GetSharpnessAdjust(MS_BOOL bScalerWin);
MS_U8 MDrv_ACE_DNR_GetGuassin_SNR_Threshold(MS_BOOL bScalerWin);

void MDrv_ACE_DNR_SetCoringThreshold(MS_BOOL bScalerWin, MS_U16 u16val);
void MDrv_ACE_DNR_SetSharpnessAdjust(MS_BOOL bScalerWin, MS_U16 u16val);
void MDrv_ACE_DNR_SetNM_V(MS_BOOL bScalerWin, MS_U16 u16val);
void MDrv_ACE_DNR_SetGNR_0(MS_BOOL bScalerWin, MS_U16 u16val);
void MDrv_ACE_DNR_SetGNR_1(MS_BOOL bScalerWin, MS_U16 u16val);
void MDrv_ACE_DNR_SetCP(MS_BOOL bScalerWin, MS_U16 u16val);
void MDrv_ACE_DNR_SetDP(MS_BOOL bScalerWin, MS_U16 u16val);
void MDrv_ACE_DNR_SetNM_H_0(MS_BOOL bScalerWin, MS_U16 u16val);
void MDrv_ACE_DNR_SetNM_H_1(MS_BOOL bScalerWin, MS_U16 u16val);
void MDrv_ACE_DNR_SetGray_Ground_Gain(MS_U16 u16val);
void MDrv_ACE_DNR_SetGray_Ground_En(MS_BOOL bScalerWin, MS_U16 u16val);
void MDrv_ACE_DNR_SetSC_Coring(MS_BOOL bScalerWin, MS_U16 u16val);
void MDrv_ACE_DNR_SetSpikeNR_0(MS_U16 u16val);
void MDrv_ACE_DNR_SetSpikeNR_1(MS_U16 u16val);
void MDrv_ACE_DNR_SetSNR_NM(MS_U16 u16val);
void MDrv_ACE_DNR_SetBank_Coring(MS_BOOL bScalerWin, MS_U16 u16val);
void MDrv_ACE_DNR_SetGuassin_SNR_Threshold(MS_BOOL bScalerWin, MS_U16 u16val);
void MDrv_ACE_DNR_SetNRTable(MS_U8 *pTbl);
void MDrv_ACE_SetColorMatrixControl( MS_BOOL bScalerWin, MS_BOOL bEnable );
void MDrv_ACE_SetRBChannelRange(MS_BOOL bScalerWin, MS_BOOL bRange);
void MDrv_ACE_WriteColorMatrixBurst( MS_BOOL bWindow, MS_U16* psMatrix );
MS_BOOL MDrv_ACE_IsSupportMLoad( MS_BOOL bWindow );

void MDrv_ACE_ColorCorrectionTable( MS_BOOL bScalerWin, MS_S16 *psColorCorrectionTable );
void MDrv_ACE_PCsRGBTable( MS_BOOL bScalerWin, MS_S16 *psPCsRGBTable);
//OS dependent.

void MDrv_ACE_SetSkipWaitVsync( MS_BOOL eWindow,MS_BOOL bIsSkipWaitVsyn);
MS_BOOL MDrv_ACE_GetSkipWaitVsync( MS_BOOL bScalerWin);



#ifdef __cplusplus
}
#endif
#endif // DRV_ACE_H

