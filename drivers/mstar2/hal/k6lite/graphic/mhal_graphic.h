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
// @file   mhal_graphic.h
// @brief  Graphic Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _MHAL_GRAPHIC_H
#define _MHAL_GRAPHIC_H
#include "mhal_graphic_gop.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define MAX_GOP_SUPPORT                          5
#define MAX_GOP0_GWIN                           2
#define MAX_GOP1_GWIN                           2UL
#define MAX_GOP2_GWIN                           1UL
#define MAX_GOP3_GWIN                           1UL
#define MAX_GOP4_GWIN                           1UL
#define MAX_GOP5_GWIN                           0UL

#define GOP0_Gwin0Id                            0UL
#define GOP0_Gwin1Id                            1UL
#define GOP1_Gwin0Id                            2UL
#define GOP1_Gwin1Id                            3UL
#define GOP2_Gwin0Id                            4UL
#define GOP3_Gwin0Id                            5UL
#define GOP4_Gwin0Id                            6UL
#define GOP5_Gwin0Id                            0xF0

#define GOP0_GwinIdBase                         GOP0_Gwin0Id
#define GOP1_GwinIdBase                         MAX_GOP0_GWIN
#define GOP2_GwinIdBase                         MAX_GOP0_GWIN + MAX_GOP1_GWIN
#define GOP3_GwinIdBase                         MAX_GOP0_GWIN + MAX_GOP1_GWIN + MAX_GOP2_GWIN
#define GOP4_GwinIdBase                         MAX_GOP0_GWIN + MAX_GOP1_GWIN + MAX_GOP2_GWIN + MAX_GOP3_GWIN
#define GOP5_GwinIdBase                         MAX_GOP0_GWIN + MAX_GOP1_GWIN + MAX_GOP2_GWIN + MAX_GOP3_GWIN + MAX_GOP4_GWIN

#define GOP_BIT0    0x01
#define GOP_BIT1    0x02
#define GOP_BIT2    0x04
#define GOP_BIT3    0x08
#define GOP_BIT4    0x10
#define GOP_BIT5    0x20
#define GOP_BIT6    0x40
#define GOP_BIT7    0x80
#define GOP_BIT8    0x0100
#define GOP_BIT9    0x0200
#define GOP_BIT10   0x0400
#define GOP_BIT11   0x0800
#define GOP_BIT12   0x1000
#define GOP_BIT13   0x2000
#define GOP_BIT14   0x4000
#define GOP_BIT15   0x8000

#define GOP_REG_WORD_MASK                           0xffff
#define GOP_REG_HW_MASK                             0xff00
#define GOP_REG_LW_MASK                             0x00ff

#define GOP_FIFO_BURST_ALL                          (GOP_BIT8|GOP_BIT9|GOP_BIT10|GOP_BIT11|GOP_BIT12)

#define GOP_FIFO_BURST_MASK                         (GOP_BIT8|GOP_BIT9|GOP_BIT10|GOP_BIT11|GOP_BIT12)
#define GOP_FIFO_THRESHOLD                          0xF0

#define GOP_YUV_TRANSPARENT_ENABLE              GOP_BIT5
#define GOP_RGB_TRANSPARENT_ENABLE              GOP_BIT11

#define GOP_WordUnit                                32
#define GOP_STRETCH_WIDTH_UNIT                2
#define SCALING_MULITPLIER                      0x1000

#define MAX_GOP_MUX                             5UL

//#define _fb_phy_addr  0x38F00000
#define _fb_phy_addr  0xE000000
#define PANEL_WIDTH         1920//3840
#define PANEL_HEIGHT        1080//2160


//-------------------------------------------------------------------------------------------------
//  Chip Configuration used by XC
//-------------------------------------------------------------------------------------------------
#define MAX_XC_DEVICE_NUM       (2)
#define MAX_XC_DEVICE0_OFFSET   (0UL)
#define MAX_XC_DEVICE1_OFFSET   (128UL)

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

void _HAL_GOP_Write16Reg(MS_U32 u32addr, MS_U16 u16val, MS_U16 mask);
void _HAL_GOP_Read16Reg(MS_U32 u32addr, MS_U16* pu16ret);
void _HAL_GOP_Write32Reg(MS_U32 u32addr, MS_U32 u32val);
void _HAL_GOP_Init(MS_U8 u8GOP);
void _HAL_GOP_GWIN_SetDstPlane(MS_U8 u8GOP, EN_DRV_GOP_DST_TYPE eDsttype, MS_BOOL bOnlyCheck);
void _HAL_GOP_SetBlending(MS_U8 u8GOP, MS_U8 u8win, MS_BOOL bEnable, MS_U8 u8coef);
MS_U8 _HAL_GOP_GetBnkOfstByGop(MS_U8 gop, MS_U32 *pBnkOfst);
MS_BOOL _HAL_GOP_IsSupportMultiAlpha(MS_U8 u8Gop);
MS_BOOL _HAL_GOP_GetDstPlane(MS_U8 u8GOP, EN_DRV_GOP_DST_TYPE *pGopDst);
MS_BOOL _HAL_GOP_EnableVENewAlphaMode(MS_U8 veMuxNum, MS_BOOL bEnable);
void _HAL_GOP_SetMux(MS_U8 u8Gop, Gop_MuxSel eGopMux);
void _HAL_GOP_GetMux(MS_U8* pU8Gop, Gop_MuxSel eGopMux);
void _HAL_GOP_GetPDByDst(MS_U8 u8Gop, EN_DRV_GOP_DST_TYPE gopDst, MS_U16* u16StrwinStr);
void _HAL_GOP_AdjustHSPD(MS_U8 u8Gop, MS_U16 u16StrwinStr, EN_DRV_GOP_DST_TYPE gopDst);
void _HAL_GOP_SetHstart(MS_U8 u8Gop, MS_U16 u16PanelHStr);
void _HAL_GOP_InitMux(void);
void _HAL_GOP_GetHstarByTiming(MS_U16 timingWidht, MS_U16 timingHeight, MS_U16*  hstart);
#endif
