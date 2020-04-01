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
// @file   mdrv_fb_st.h
// @brief  FrameBuffer Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *  @file mdrv_fb_st.h
 *  @brief FrameBuffer Driver IOCTL parameter interface
 */

 /**
 * \ingroup fb_group
 * @{
 */
#ifndef _MDRV_FB_ST_H
#define _MDRV_FB_ST_H

//=============================================================================
// enum
//=============================================================================
/**
* Used to show current support color format of gop in FB_GOP_SUPINF_CONFIG
*/
typedef enum
{
    COLORFMT_ARGB8888 = 0,
    COLORFMT_RGB565   = 1,
    COLORFMT_YUV422   = 2,
    COLORFMT_I8PALETTE= 3,
    COLORFMT_MAX      = 4,
}FB_GOP_COLORFORMAT_TYPE;

/**
* Used to setup the mode of alpha blending for gop in FB_GOP_ALPHA_CONFIG
*/
typedef enum
{
    PIXEL_ALPHA = 0,
    CONST_ALPHA = 1,
}FB_GOP_ALPHA_TYPE;

/**
* Used to setup the buffer number for gop in FB_GOP_RESOLUTION_STRETCH_H_CONFIG
*/
typedef enum
{
    FB_SINGLE_BUFFER = 1,
    FB_DOUBLE_BUFFER = 2,
}FB_GOP_BUFFER_NUM;

/**
* Used to setup the stretch H ratio for gop in FB_GOP_RESOLUTION_STRETCH_H_CONFIG
*/
typedef enum
{
    FB_STRETCH_H_RATIO_1 = 1,
    FB_STRETCH_H_RATIO_2 = 2,
    FB_STRETCH_H_RATIO_4 = 4,
    FB_STRETCH_H_RATIO_8 = 8,
}FB_GOP_STRETCH_H_RATIO;

//=============================================================================
// struct
//=============================================================================



//=============================================================================
// struct for IOCTL_FB_GETFBSUPPORTINF
/**
* Used to get framebuffer support information
*/
typedef struct
{

    unsigned char bKeyAlpha;             ///< whether support colorkey
    unsigned char bConstAlpha;           ///< whether support constant alpha
    unsigned char bPixelAlpha;           ///< whether support pixel alpha
    unsigned char bColFmt[COLORFMT_MAX]; ///< support which color format
    unsigned long u32MaxWidth;           ///< the max pixels per line
    unsigned long u32MaxHeight;          ///< the max lines
}FB_GOP_SUPINF_CONFIG;

// struct for FB_GOP_GWIN_CONFIG
/**
* Used to set or get gwin parameters
*/
typedef struct
{
    unsigned short u18HStart; ///< gwin horizontal starting coordinate
    unsigned short u18HEnd;   ///< gwin horizontal ending coordinate
    unsigned short u18VStart; ///< gwin vertical starting coordinate
    unsigned short u18VEnd;   ///< gwin vertical ending coordinate
}FB_GOP_GWIN_CONFIG;

// struct for FB_GOP_ALPHA_CONFIG
/**
* Used to set or get gop alpha blending settings
*/
typedef struct
{
    unsigned char bEn;             ///< alpha blending enable or disable
    FB_GOP_ALPHA_TYPE enAlphaType; ///< set alpha type: pixel alpha or constant alpha
    unsigned char u8Alpha;         ///< constant alpha value, availble for setting constant alpha
}FB_GOP_ALPHA_CONFIG;

// struct for FB_GOP_COLORKEY_CONFIG
/**
* Used to set or get gop colorkey settings
*/
typedef struct
{
    unsigned char bEn; ///< colorkey enable or disable
    unsigned char u8R; ///< parameter for red color in colorkey
    unsigned char u8G; ///< parameter for green color in colorkey
    unsigned char u8B; ///< parameter for blue color in colorkey
}FB_GOP_COLORKEY_CONFIG;

// struct for FB_GOP_PaletteEntry
/**
* Used to set palette parameters
*/
typedef union
{
    /// RGBA8888
    struct
    {
        unsigned char u8B; ///< parameter for blue color in palette
        unsigned char u8G; ///< parameter for green color in palette
        unsigned char u8R; ///< parameter for red color in palette
        unsigned char u8A; ///< parameter for alpha in palette
    } BGRA;
} FB_GOP_PaletteEntry;

// struct for FB_GOP_RESOLUTION_STRETCH_H_CONFIG
/**
* Used to set or get gop colorkey settings
*/
typedef struct
{
    unsigned long u32Width;                  ///< the width for gop resolution
    unsigned long u32Height;                 ///< the height for gop resolution
    unsigned long u32DisplayWidth;           ///< the width for display resolution, =u32Width*enStretchH_Ratio
    unsigned long u32DisplayHeight;          ///< the height for display resolution,=u32Height
    FB_GOP_BUFFER_NUM enBufferNum;           ///< buffer number
    FB_GOP_STRETCH_H_RATIO enStretchH_Ratio; ///< stretch H ratio
}FB_GOP_RESOLUTION_STRETCH_H_CONFIG;

//=============================================================================

//=============================================================================

#define GOP_PALETTE_ENTRY_NUM 256

//// {B, G, R, Alpha}
static FB_GOP_PaletteEntry DefaultPaletteEntry[GOP_PALETTE_ENTRY_NUM] =
{
    {{  0,  0,  0,  0}}, {{  0,  0,128,  0}}, {{  0,128,  0,  0}}, {{  0,128,128,  0}},
    {{128,  0,  0,  0}}, {{128,  0,128,  0}}, {{128,128,  0,  0}}, {{192,192,192,  0}},
    {{192,220,192,  0}}, {{240,202,166,  0}}, {{  0, 32, 64,  0}}, {{  0, 32, 96,  0}},
    {{  0, 32,128,  0}}, {{  0, 32,160,  0}}, {{  0, 32,192,  0}}, {{  0, 32,224,  0}},
    {{  0, 64,  0,  0}}, {{  0, 64, 32,  0}}, {{  0, 64, 64,  0}}, {{  0, 64, 96,  0}},
    {{  0, 64,128,  0}}, {{  0, 64,160,  0}}, {{  0, 64,192,  0}}, {{  0, 64,224,  0}},
    {{  0, 96,  0,  0}}, {{  0, 96, 32,  0}}, {{  0, 96, 64,  0}}, {{  0, 96, 96,  0}},
    {{  0, 96,128,  0}}, {{  0, 96,160,  0}}, {{  0, 96,192,  0}}, {{  0, 96,224,  0}},
    {{  0,128,  0,  0}}, {{  0,128, 32,  0}}, {{  0,128, 64,  0}}, {{  0,128, 96,  0}},
    {{  0,128,128,  0}}, {{  0,128,160,  0}}, {{  0,128,192,  0}}, {{  0,128,224,  0}},
    {{  0,160,  0,  0}}, {{  0,160, 32,  0}}, {{  0,160, 64,  0}}, {{  0,160, 96,  0}},
    {{  0,160,128,  0}}, {{  0,160,160,  0}}, {{  0,160,192,  0}}, {{  0,160,224,  0}},
    {{  0,192,  0,  0}}, {{  0,192, 32,  0}}, {{  0,192, 64,  0}}, {{  0,192, 96,  0}},
    {{  0,192,128,  0}}, {{  0,192,160,  0}}, {{  0,192,192,  0}}, {{  0,192,224,  0}},
    {{  0,224,  0,  0}}, {{  0,224, 32,  0}}, {{  0,224, 64,  0}}, {{  0,224, 96,  0}},
    {{  0,224,128,  0}}, {{  0,224,160,  0}}, {{  0,224,192,  0}}, {{  0,224,224,  0}},
    {{ 64,  0,  0,  0}}, {{ 64,  0, 32,  0}}, {{ 64,  0, 64,  0}}, {{ 64,  0, 96,  0}},
    {{ 64,  0,128,  0}}, {{ 64,  0,160,  0}}, {{ 64,  0,192,  0}}, {{ 64,  0,224,  0}},
    {{ 64, 32,  0,  0}}, {{ 64, 32, 32,  0}}, {{ 64, 32, 64,  0}}, {{ 64, 32, 96,  0}},
    {{ 64, 32,128,  0}}, {{ 64, 32,160,  0}}, {{ 64, 32,192,  0}}, {{ 64, 32,224,  0}},
    {{ 64, 64,  0,  0}}, {{ 64, 64, 32,  0}}, {{ 64, 64, 64,  0}}, {{ 64, 64, 96,  0}},
    {{ 64, 64,128,  0}}, {{ 64, 64,160,  0}}, {{ 64, 64,192,  0}}, {{ 64, 64,224,  0}},
    {{ 64, 96,  0,  0}}, {{ 64, 96, 32,  0}}, {{ 64, 96, 64,  0}}, {{ 64, 96, 96,  0}},
    {{ 64, 96,128,  0}}, {{ 64, 96,160,  0}}, {{ 64, 96,192,  0}}, {{ 64, 96,224,  0}},
    {{ 64,128,  0,  0}}, {{ 64,128, 32,  0}}, {{ 64,128, 64,  0}}, {{ 64,128, 96,  0}},
    {{ 64,128,128,  0}}, {{ 64,128,160,  0}}, {{ 64,128,192,  0}}, {{ 64,128,224,  0}},
    {{ 64,160,  0,  0}}, {{ 64,160, 32,  0}}, {{ 64,160, 64,  0}}, {{ 64,160, 96,  0}},
    {{ 64,160,128,  0}}, {{ 64,160,160,  0}}, {{ 64,160,192,  0}}, {{ 64,160,224,  0}},
    {{ 64,192,  0,  0}}, {{ 64,192, 32,  0}}, {{ 64,192, 64,  0}}, {{ 64,192, 96,  0}},
    {{ 64,192,128,  0}}, {{ 64,192,160,  0}}, {{ 64,192,192,  0}}, {{ 64,192,224,  0}},
    {{ 64,224,  0,  0}}, {{ 64,224, 32,  0}}, {{ 64,224, 64,  0}}, {{ 64,224, 96,  0}},
    {{ 64,224,128,  0}}, {{ 64,224,160,  0}}, {{ 64,224,192,  0}}, {{ 64,224,224,  0}},
    {{128,  0,  0,  0}}, {{128,  0, 32,  0}}, {{128,  0, 64,  0}}, {{128,  0, 96,  0}},
    {{128,  0,128,  0}}, {{128,  0,160,  0}}, {{128,  0,192,  0}}, {{128,  0,224,  0}},
    {{128, 32,  0,  0}}, {{128, 32, 32,  0}}, {{128, 32, 64,  0}}, {{128, 32, 96,  0}},
    {{128, 32,128,  0}}, {{128, 32,160,  0}}, {{128, 32,192,  0}}, {{128, 32,224,  0}},
    {{128, 64,  0,  0}}, {{128, 64, 32,  0}}, {{128, 64, 64,  0}}, {{128, 64, 96,  0}},
    {{128, 64,128,  0}}, {{128, 64,160,  0}}, {{128, 64,192,  0}}, {{128, 64,224,  0}},
    {{128, 96,  0,  0}}, {{128, 96, 32,  0}}, {{128, 96, 64,  0}}, {{128, 96, 96,  0}},
    {{128, 96,128,  0}}, {{128, 96,160,  0}}, {{128, 96,192,  0}}, {{128, 96,224,  0}},
    {{128,128,  0,  0}}, {{128,128, 32,  0}}, {{128,128, 64,  0}}, {{128,128, 96,  0}},
    {{128,128,128,  0}}, {{128,128,160,  0}}, {{128,128,192,  0}}, {{128,128,224,  0}},
    {{128,160,  0,  0}}, {{128,160, 32,  0}}, {{128,160, 64,  0}}, {{128,160, 96,  0}},
    {{128,160,128,  0}}, {{128,160,160,  0}}, {{128,160,192,  0}}, {{128,160,224,  0}},
    {{128,192,  0,  0}}, {{128,192, 32,  0}}, {{128,192, 64,  0}}, {{128,192, 96,  0}},
    {{128,192,128,  0}}, {{128,192,160,  0}}, {{128,192,192,  0}}, {{128,192,224,  0}},
    {{128,224,  0,  0}}, {{128,224, 32,  0}}, {{128,224, 64,  0}}, {{128,224, 96,  0}},
    {{128,224,128,  0}}, {{128,224,160,  0}}, {{128,224,192,  0}}, {{128,224,224,  0}},
    {{192,  0,  0,  0}}, {{192,  0, 32,  0}}, {{192,  0, 64,  0}}, {{192,  0, 96,  0}},
    {{192,  0,128,  0}}, {{192,  0,160,  0}}, {{192,  0,192,  0}}, {{192,  0,224,  0}},
    {{192, 32,  0,  0}}, {{192, 32, 32,  0}}, {{192, 32, 64,  0}}, {{192, 32, 96,  0}},
    {{192, 32,128,  0}}, {{192, 32,160,  0}}, {{192, 32,192,  0}}, {{192, 32,224,  0}},
    {{192, 64,  0,  0}}, {{192, 64, 32,  0}}, {{192, 64, 64,  0}}, {{192, 64, 96,  0}},
    {{192, 64,128,  0}}, {{192, 64,160,  0}}, {{192, 64,192,  0}}, {{192, 64,224,  0}},
    {{192, 96,  0,  0}}, {{192, 96, 32,  0}}, {{192, 96, 64,  0}}, {{192, 96, 96,  0}},
    {{192, 96,128,  0}}, {{192, 96,160,  0}}, {{192, 96,192,  0}}, {{192, 96,224,  0}},
    {{192,128,  0,  0}}, {{192,128, 32,  0}}, {{192,128, 64,  0}}, {{192,128, 96,  0}},
    {{192,128,128,  0}}, {{192,128,160,  0}}, {{192,128,192,  0}}, {{192,128,224,  0}},
    {{192,160,  0,  0}}, {{192,160, 32,  0}}, {{192,160, 64,  0}}, {{192,160, 96,  0}},
    {{192,160,128,  0}}, {{192,160,160,  0}}, {{192,160,192,  0}}, {{192,160,224,  0}},
    {{192,192,  0,  0}}, {{192,192, 32,  0}}, {{192,192, 64,  0}}, {{192,192, 96,  0}},
    {{192,192,128,  0}}, {{192,192,160,  0}}, {{240,251,255,  0}}, {{164,160,160,  0}},
    {{128,128,128,  0}}, {{  0,  0,255,  0}}, {{  0,255,  0,  0}}, {{  0,255,255,  0}},
    {{255,  0,  0,  0}}, {{255,  0,255,  0}}, {{255,255,  0,  0}}, {{255,255,255,  0}}
 };
#endif //MDRV_FB_ST_H


/** @} */ // end of fb_group
