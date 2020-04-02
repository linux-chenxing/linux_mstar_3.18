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


#define REG_ROTATE_TRIG_MSK        (0x01<<0)

#define REG_ROTATE_EMI_WLIMIT_MSK  (0x01<<8)
#define REG_ROTATE_EMI_WLIMIT_EN   (0x01<<8)

#define REG_ROTATE_IMI0_WLIMIT_MSK (0x01<<9)
#define REG_ROTATE_IMI0_WLIMIT_EN  (0x01<<9)

#define REG_ROTATE_IMI1_WLIMIT_MSK (0x01<<10)
#define REG_ROTATE_IMI1_WLIMIT_EN  (0x01<<10)

#define REG_ROTATE_SRC_FMT_MSK    (0x0003)
#define REG_ROTATE_SRC_FMT_RGB565 (0x00)
#define REG_ROTATE_SRC_FMT_YUV422 (0x01)
#define REG_ROTATE_SRC_FMT_8888   (0x02)

#define REG_ROTATE_DEGREE_MSK     (0x0030)
#define REG_ROTATE_DEGREE_0       (0x00<<4)
#define REG_ROTATE_DEGREE_90      (0x01<<4)
#define REG_ROTATE_DEGREE_180     (0x02<<4)
#define REG_ROTATE_DEGREE_270     (0x03<<4)

#define ROT_EMI_BYTE_PER_PIXLE    (4) //128bits
#define ROT_IMI_BYTE_PER_PIXLE    (3) //64bits
#define ROT_PITCH_BYTE_PER_PIEXL  (3) //64bits



//==============================================================================
//==============================================================================
#ifdef MHAL_DIP_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE void Hal_ROT_set_size(MS_U16 u16width, MS_U16 u16height);
INTERFACE void Hal_ROT_set_fmt(XC_ROT_SRC_FMT enFmt);
INTERFACE void Hal_ROT_set_degree(XC_ROT_DEGREE enDegree);
INTERFACE void Hal_ROT_set_src_base(MS_U32 u32addr);
INTERFACE void Hal_ROT_set_dest_base(MS_U32 u32addr);
INTERFACE void Hal_ROT_set_src_pitch(MS_U16 u16pitch);
INTERFACE void Hal_ROT_set_dest_pitch(MS_U16 u16pitch);
INTERFACE void Hal_ROT_set_imi_base(MS_U32 u32base_0, MS_U32 u32base_1);
INTERFACE void Hal_ROT_set_trig(MS_BOOL bEn);
INTERFACE void Hal_ROT_set_emi_wirteprotect(MS_BOOL bEn, MS_U32 u32Addr);
INTERFACE void Hal_ROT_set_imi_wirteprotect_base0(MS_BOOL bEn, MS_U32 u32Addr);
INTERFACE void Hal_ROT_set_imi_wirteprotect_base1(MS_BOOL bEn, MS_U32 u32Addr);




#undef INTERFACE
#endif /* MHAL_DIP_H */

