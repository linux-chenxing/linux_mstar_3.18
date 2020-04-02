///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016-2017 MStar Semiconductor, Inc.
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
// @file   mhal_graphic_gop.h
// @brief  Graphic Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _MHAL_GRAPHIC_GOP_H
#define _MHAL_GRAPHIC_GOP_H

#if defined(__cplusplus)
extern "C" {
#endif

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

/// GWIN output color domain
typedef enum
{
    DRV_GOPOUT_RGB,     ///< 0: output color RGB
    DRV_GOPOUT_YUV,     ///< 1: output color YUV
} EN_DRV_GOP_OUTPUT_COLOR;

/// Define GOP MIU SEL
typedef enum
{
    /// E_DRV_GOP_SEL_MIU0. gop access miu 0
    E_DRV_GOP_SEL_MIU0    = 0,
    /// E_GOP_SEL_MIU1. gop access miu1
    E_DRV_GOP_SEL_MIU1    = 1,
    /// E_GOP_SEL_MIU2. gop access miu2
    E_DRV_GOP_SEL_MIU2    = 2,
    /// E_GOP_SEL_MIU3. gop access miu3
    E_DRV_GOP_SEL_MIU3    = 3,
} E_DRV_GOP_SEL_TYPE;

typedef enum
{
    E_GOP0 = 0,
    E_GOP1 = 1,
    E_GOP2 = 2,
    E_GOP3 = 3,
    E_GOP4 = 4,
}E_GOP_TYPE;

/// Define GOP destination displayplane type
typedef enum
{
    E_DRV_GOP_DST_IP0       =   0,
    E_DRV_GOP_DST_IP0_SUB   =   1,
    E_DRV_GOP_DST_MIXER2VE  =   2,
    E_DRV_GOP_DST_OP0       =   3,
    E_DRV_GOP_DST_VOP       =   4,
    E_DRV_GOP_DST_IP1       =   5,
    E_DRV_GOP_DST_IP1_SUB   =   6,
    E_DRV_GOP_DST_MIXER2OP  =   7,
    E_DRV_GOP_DST_VOP_SUB   =   8,
    E_DRV_GOP_DST_FRC       =   9,
    E_DRV_GOP_DST_VE        =   10,
    E_DRV_GOP_DST_BYPASS    =   11,
    E_DRV_GOP_DST_OP1       =   12,
    E_DRV_GOP_DST_MIXER2OP1 =  13,
    E_DRV_GOP_DST_DIP       =   14,
    E_DRV_GOP_DST_GOPScaling  = 15,
    E_DRV_GOP_DST_OP_DUAL_RATE   = 16,
    MAX_DRV_GOP_DST_SUPPORT ,
    E_DRV_GOP_DST_INVALID  ,
} EN_DRV_GOP_DST_TYPE;

/// Transparent color format
typedef enum
{
    /// RGB mode transparent color.
    GOPTRANSCLR_FMT0,
   /// YUV mode transparent color.
    GOPTRANSCLR_FMT1,
} EN_DRV_GOP_TRANSCLR_FMT;

typedef enum
{
    /// Color format RGB555 and Blink.
    E_DRV_GOP_COLOR_RGB555_BLINK    =0,
    /// Color format RGB565.
    E_DRV_GOP_COLOR_RGB565          =1,
    /// Color format ARGB4444.
    E_DRV_GOP_COLOR_ARGB4444        =2,
    /// Color format alpha blink.
    E_DRV_GOP_COLOR_2266      =3,
    /// Color format I8 (256-entry palette).
    E_DRV_GOP_COLOR_I8              =4,
    /// Color format ARGB8888.
    E_DRV_GOP_COLOR_ARGB8888        =5,
    /// Color format ARGB1555.
    E_DRV_GOP_COLOR_ARGB1555        =6,
    /// Color format ARGB8888.  - Andriod format
    E_DRV_GOP_COLOR_ABGR8888        =7,
    /// Color format RGB555/YUV422.
     E_DRV_GOP_COLOR_RGB555YUV422    =8,
    /// Color format YUV422.
    E_DRV_GOP_COLOR_YUV422          =9,
    /// Color format ARGB8888.  - Andriod format
    E_DRV_GOP_COLOR_RGBA5551        =10,
    /// Color format ARGB8888.  - Andriod format
    E_DRV_GOP_COLOR_RGBA4444        =11,

    /// Invalid color format.
    E_DRV_GOP_COLOR_INVALID,
} EN_DRV_GOPColorType;

typedef struct
{
    ///panel width.
    MS_U16 u16PanelWidth;
    ///panel height.
    MS_U16 u16PanelHeight;
    ///panel h-start.
    MS_U16 u16PanelHStr;
    ///vsync interrupt flip enable flag.
    MS_BOOL bEnableVsyncIntFlip;
    ///gop frame buffer starting address.
    MS_PHY u32GOPRBAdr;
    ///gop frame buffer length.
    MS_U32 u32GOPRBLen;
    ///gop regdma starting address.
    MS_PHY u32GOPRegdmaAdr;
    ///gop regdma length.
    MS_U32 u32GOPRegdmaLen;
}GOP_InitInfo;

typedef struct
{
    MS_U16 u16HStart;              //!< unit: pix
    MS_U16 u16HEnd;                //!< unit: pix
    MS_U16 u16VStart;              //!< unit: pix
    MS_U16 u16VEnd;                //!< unit: pix
    MS_U32 u16Pitch;               //!< unit: Byte
    MS_U32 u32Addr;                //!< unit: pix
    EN_DRV_GOPColorType clrType;       //!< color format of the buffer
} DRV_GWIN_INFO;

typedef struct
{
    MS_U16 u16Xpos; //!< unit: pix
    MS_U16 u16Ypos; //!< unit: pix
    MS_U16 u16SrcWidth; //!< unit: pix
    MS_U16 u16SrcHeight; //!< unit: pix
    MS_U16 u16DstWidth; //!< unit: pix
    MS_U16 u16DstHeight;//!< unit: pix
}DRV_STRETCHWIN_INFO;
//Define Mux
typedef enum
{
    ///Select gop output to mux0
    E_GOP_MUX0 = 0,
    /// Select gop output to mux1
    E_GOP_MUX1 = 1,
    /// Select gop output to mux2
    E_GOP_MUX2 = 2,
    /// Select gop output to mux3
    E_GOP_MUX3 = 3,
    /// Select gop output to mux4
    E_GOP_MUX4 = 4,
    ///Select gop output to IP0
    E_GOP_IP0_MUX,
    /// Select gop output to IP1
    E_GOP_IP1_MUX,
    /// Select gop output to vop0
    E_GOP_VOP0_MUX,
    /// Select gop output to vop1
    E_GOP_VOP1_MUX,
    /// Select gop output to FRC mux0
    E_GOP_FRC_MUX0,
    /// Select gop output to FRC mux1
    E_GOP_FRC_MUX1,
    /// Select gop output to FRC mux2
    E_GOP_FRC_MUX2,
    /// Select gop output to FRC mux3
    E_GOP_FRC_MUX3,
    /// Select gop output to BYPASS mux
    E_GOP_BYPASS_MUX0,
    /// Select gop output to IP0_SUB
    E_GOP_IP0_SUB_MUX,
    /// Select gop output to VOP0_SUB
    E_GOP_VOP0_SUB_MUX,
    /// Select gop output to OP1
    E_GOP_OP1_MUX,
    /// Select gop output to DIP
    E_GOP_DIP_MUX,
    /// Select gop output to DIP
    E_GOP_GS_MUX,
    /// Select gop output to Mix layer0
    E_GOP_Mix_MUX0,
    /// Select gop output to Mix layer1
    E_GOP_Mix_MUX1,
    /// Select gop output to VE0
    E_GOP_VE0_MUX,
    /// Select gop output to VE1
    E_GOP_VE1_MUX,

    ///Select gop output to dual rate op mux0
    E_GOP_DUALRATE_OP_MUX0,
    ///Select gop output to dual rate op mux1
    E_GOP_DUALRATE_OP_MUX1,
    ///Select gop output to dual rate op mux2
    E_GOP_DUALRATE_OP_MUX2,

    MAX_GOP_MUX_SUPPORT,
    E_GOP_MUX_INVAILD,
}Gop_MuxSel;

typedef struct
{
    // Output timing info
    MS_U16  u16VTotal;            ///< Output vertical total
    MS_U16  u16DEVStart;          ///< Output DE vertical start
    MS_U16  u16DEVEnd;            ///< Output DE Vertical end
    MS_U16  u16VSyncStart;        ///< Output VSync start
    MS_U16  u16VSyncEnd;          ///< Output VSync end
    MS_U16  u16HTotal;            ///< Output horizontal total
    MS_U16  u16DEHStart;          ///< Output DE horizontal start
    MS_U16  u16DEHEnd;            ///< Output DE horizontal end
    MS_U16  u16HSyncWidth;        ///< Output HSync width
    MS_BOOL bIsPanelManualVysncMode; ///< enable manuel V sync control
    MS_BOOL bInterlaceOutput;     ///< enable Scaler Interlace output
}PNL_TimingInfo;

#if defined(__cplusplus)
}
#endif
#endif //_MHAL_GRAPHIC_COP_H
