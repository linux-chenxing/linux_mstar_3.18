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
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
// [mhal_sc.h]
// Date: 20081203
// Descriptions: Add a new layer for HW setting
//==============================================================================
#ifndef MHAL_PIP_H
#define MHAL_PIP_H

#ifdef MULTI_SCALER_SUPPORTED
void Hal_SC_Sub_SetDisplayWindow_burst(MS_WINDOW_TYPE *pstDispWin, SCALER_WIN eWindow);
void Hal_SC_Sub_SetDisplayWindow(MS_WINDOW_TYPE *pstDispWin, SCALER_WIN eWindow);
void Hal_SC_subwindow_disable_burst(SCALER_WIN eWindow);
void Hal_SC_subwindow_disable(SCALER_WIN eWindow);
MS_BOOL Hal_SC_Is_subwindow_enable(SCALER_WIN eWindow);
void Hal_SC_set_Zorder_main_first(MS_BOOL bMainFirst,SCALER_WIN eWindow);
void Hal_SC_set_Zorder_main_first_burst(MS_BOOL bMainFirst,SCALER_WIN eWindow);
void Hal_SC_set_Zextra_y_half(MS_BOOL bInterlace,SCALER_WIN eWindow);
void Hal_SC_set_Zextra_y_half_burst(MS_BOOL bInterlace,SCALER_WIN eWindow);
#else
void Hal_SC_Sub_SetDisplayWindow_burst(MS_WINDOW_TYPE *pstDispWin);
void Hal_SC_Sub_SetDisplayWindow(MS_WINDOW_TYPE *pstDispWin);
void Hal_SC_subwindow_disable_burst(void);
void Hal_SC_subwindow_disable(void);
MS_BOOL Hal_SC_Is_subwindow_enable(void);
void Hal_SC_set_Zorder_main_first(MS_BOOL bMainFirst);
void Hal_SC_set_Zorder_main_first_burst(MS_BOOL bMainFirst);
void Hal_SC_set_Zextra_y_half(MS_BOOL bInterlace);
void Hal_SC_set_Zextra_y_half_burst(MS_BOOL bInterlace);
#endif

void Hal_SC_set_border_format( MS_U8 u8Left, MS_U8 u8Right, MS_U8 u8Up, MS_U8 u8Down, MS_U8 u8color, SCALER_WIN eWindow );
void Hal_SC_border_enable(MS_BOOL bEnable, SCALER_WIN eWindow);
void Hal_SC_SetPIP_FRC(MS_BOOL enable, MS_U8 u8IgonreLinesNum);

#endif /* MHAL_SC_H */
