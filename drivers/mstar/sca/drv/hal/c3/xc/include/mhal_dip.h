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
#ifndef MHAL_DIP_H
#define MHAL_DIP_H

#include "hwreg_sc.h"
#include "hwreg_sc1.h"
#include "hwreg_sc2.h"
#include "hwreg_mod.h"

#define DIP_MIU_BUS  (4) // 128bits
#define DIP_MIU_BUS_BYTE  (1<<DIP_MIU_BUS)

#define DIP_MIU_BUS_I64 (3)
#define DIP_MIU_BUS_I64_BYTE (1<<DIP_MIU_BUS_I64) // 64bits

#define MVOP_420_H_Align(x)  (((x)+0x0F)&~0x0F) // 16 align
#define MVOP_420_V_Align(y)  (((y)+0x1F)&~0x1F) // 32 align
#define MFE_420_H_Align(x)   (((x)+0x07)&~0x07) //  8 align
#define MFE_420_V_Align(y)   (((y)+0x07)&~0x07) //  8 align

//==============================================================================
//==============================================================================
#ifdef MHAL_DIP_C
#define INTERFACE
#else
#define INTERFACE extern
#endif


INTERFACE void Hal_DIP_set_src_op(MS_BOOL bEn);
INTERFACE void Hal_DIP_set_src_ip_sub(MS_BOOL bEn);
INTERFACE void Hal_DIP_set_src_ip_main(MS_BOOL bEn);
INTERFACE void Hal_DIP_set_444_to_422(MS_BOOL bEn);
INTERFACE void Hal_DIP_set_src_422(MS_BOOL bEn);
INTERFACE void Hal_DIP_set_clip_window(MS_WINDOW_TYPE stWin);
INTERFACE void Hal_DIP_set_clip_en(MS_BOOL bEn);
INTERFACE void Hal_DIP_set_pwd_off(MS_BOOL bEn);
INTERFACE void Hal_DIP_set_dip_frame_num(MS_U16 u16Num);
INTERFACE void Hal_DIP_set_base_addr(MS_U32 u32Addr, MS_U32 u32MIU_Bus);
INTERFACE void Hal_DIP_set_c_base_addr(MS_U32 u32Addr, MS_U32 u32MIU_Bus);
INTERFACE void Hal_DIP_set_base_offset(MS_U32 u32Offset);
INTERFACE void Hal_DIP_set_c_base_offset(MS_U32 u32Offset);

INTERFACE void Hal_DIP_set_src_size(MS_U16 u16Width, MS_U16 u16Height);
INTERFACE void Hal_DIP_set_base_pitch(MS_U16 u16Pitch);
INTERFACE void Hal_DIP_set_wreq_thrd_max(MS_U8 u8Thrd, MS_U8 u8Max);
INTERFACE void Hal_DIP_set_output_fmt(XC_DIP_DST_FMT eFMT);
INTERFACE void Hal_DIP_set_422to420_ratio(MS_U16 u16val);
INTERFACE void Hal_DIP_set_tile_req_num(MS_U16 u16val);
INTERFACE void Hal_DIP_set_wlimit_base_addr(MS_U32 u32Addr, MS_U32 u32MIU_Bus);
INTERFACE void Hal_DIP_set_wlimit_c_base_addr(MS_U32 u32Addr, MS_U32 u32MIU_Bus);
INTERFACE void Hal_DIP_set_wlimit_en(MS_BOOL bEn);
INTERFACE void Hal_DIP_set_y2r_en(MS_BOOL bEn);
INTERFACE void Hal_DIP_set_dyn_clk_gate(MS_BOOL bEn);
INTERFACE void Hal_DIP_set_dwin_wt_once(MS_BOOL bEn);
INTERFACE void Hal_DIP_set_dwin_wt_once_trig(MS_BOOL bEn);

#undef INTERFACE
#endif /* MHAL_DIP_H */

