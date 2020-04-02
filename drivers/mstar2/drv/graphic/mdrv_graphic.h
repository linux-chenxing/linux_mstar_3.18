////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   mdrv_graphic.h
/// @brief  MStar graphic Interface header file
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_GRAPHIC_H
#define _MDRV_GRAPHIC_H
#include "mhal_graphic_gop.h"

#ifdef _MDRV_GRAPHIC_H
#define INTERFACE
#else
#define INTERFACE extern
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#include "uapi/mstarFb.h"

//=============================================================================
// Function
//=============================================================================

INTERFACE MS_BOOL _MDrv_GRAPHIC_Init(MS_U32 u32GopIdx);
INTERFACE void _MDrv_GRAPHIC_SetHMirror(MS_U8 u8GOP,MS_BOOL bEnable);
INTERFACE void _MDrv_GRAPHIC_SetVMirror(MS_U8 u8GOP,MS_BOOL bEnable);
INTERFACE void _MDrv_GRAPHIC_OutputColor_EX(MS_U8 u8GOP,EN_DRV_GOP_OUTPUT_COLOR type);
INTERFACE void _MDrv_GRAPHIC_MIUSel(MS_U8 u8GOP, E_DRV_GOP_SEL_TYPE MiuSel);
INTERFACE void _MDrv_GRAPHIC_SetGOPDst(MS_U8 u8GOP, EN_DRV_GOP_DST_TYPE eDsttype, MS_BOOL bOnlyCheck);
INTERFACE void _MDrv_GRAPHIC_EnableTransClr_EX(MS_U8 u8GOP,EN_DRV_GOP_TRANSCLR_FMT eFmt, MS_BOOL bEnable);
INTERFACE void _MDrv_GRAPHIC_SetBlending(MS_U8 u8GOP, MS_U8 u8win, MS_BOOL bEnable, MS_U8 u8coef);
INTERFACE void _MDrv_GOP_SetGwinInfo(MS_U8 u8GOP, MS_U8 u8win,DRV_GWIN_INFO WinInfo);
INTERFACE void _MDrv_GRAPHIC_SetStretchWin(MS_U8 u8GOP,MS_U16 u16x, MS_U16 u16y, MS_U16 u16width, MS_U16 u16height);
INTERFACE void _MDrv_GRAPHIC_SetHScale(MS_U8 u8GOP,MS_BOOL bEnable, MS_U16 u16src, MS_U16 u16dst);
INTERFACE void _MDrv_GRAPHIC_SetVScale(MS_U8 u8GOP,MS_BOOL bEnable, MS_U16 u16src, MS_U16 u16dst);
INTERFACE void _MDrv_GRAPHIC_GWIN_Enable(MS_U8 u8GOP, MS_U8 u8win,MS_BOOL bEnable);
INTERFACE void _MDrv_GRAPHIC_UpdateReg(MS_U8 u8Gop);
INTERFACE void _MDrv_GRAPHIC_SetForceWrite(MS_BOOL bEnable);
INTERFACE MS_U16 _MDrv_GRAPHIC_GetBpp(MI_FB_ColorFmt_e eColorFmt);
INTERFACE void _MDrv_Graphic_SetTransClr_8888(MS_U8 u8Gop, MS_U32 clr, MS_U32 mask);
INTERFACE MS_BOOL _MDrv_Graphic_IsSupportMultiAlpha(MS_U8 u8Gop);
INTERFACE void _MDrv_Graphic_EnableMultiAlpha(MS_U8 u8Gop, MS_BOOL bEnable);
INTERFACE void _MDrv_Graphic_SetAlpha0(MS_U8 u8Gop, MS_U8 u8Alpha0);
INTERFACE void _MDrv_Graphic_SetAlpha1(MS_U8 u8Gop, MS_U8 u8Alpha1);
INTERFACE void _MDrv_Graphic_GetGwinNewAlphaModeEnable(MS_U8 u8Gop, MS_U8 u8win, MS_BOOL* pEnable);
INTERFACE void _MDrv_Graphic_SetNewAlphaMode(MS_U8 u8Gop, MS_U8 u8win, MS_BOOL bEnable);
INTERFACE void _MDrv_Graphic_SetGopMux(MS_U8 u8Gop, Gop_MuxSel eGopMux);
INTERFACE void _MDrv_Graphic_SetGopHstart(MS_U8 u8Gop, MS_U16 hstart, EN_DRV_GOP_DST_TYPE eDsttype, MS_BOOL bInitHSPD);
INTERFACE void _MDrv_Graphic_InitHSPDByGop(MS_U8 u8Gop);
#if defined(__cplusplus)
}
#endif

#undef INTERFACE

#endif //_MDRV_GRAPHIC_H

