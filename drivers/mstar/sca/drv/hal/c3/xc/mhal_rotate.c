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
// [mhal_sc.c]
// Date: 20081203
// Descriptions: Add a new layer for HW setting
//==============================================================================
#define  MHAL_ROTATE_C

#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/wait.h>
#include <asm/div64.h>
#else
#define do_div(x,y) ((x)/=(y))
#endif

// Common Definition
#include "MsCommon.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"

#include "xc_hwreg_utility2.h"
#include "xc_Analog_Reg.h"
#include "drvXC_IOPort.h"
#include "apiXC.h"

#include "mhal_rotate.h"
#include "hwreg_rotate.h"


void Hal_ROT_set_size(MS_U16 u16width, MS_U16 u16height)
{
    W2BYTEMSK(REG_ROTATE_01_L, u16height, 0x1FFF);
    W2BYTEMSK(REG_ROTATE_02_L, u16width, 0x1FFF);
}


void Hal_ROT_set_fmt(XC_ROT_SRC_FMT enFmt)
{
    MS_U16 u16SrcFm;

    switch(enFmt)
    {
    default:
    case XC_ROT_SRC_FMT_YUV422:
        u16SrcFm = REG_ROTATE_SRC_FMT_YUV422;
        break;
    case XC_ROT_SRC_FMT_RGB565:
        u16SrcFm = REG_ROTATE_SRC_FMT_RGB565;
        break;
    case XC_ROT_SRC_FMT_8888:
        u16SrcFm = REG_ROTATE_SRC_FMT_8888;
        break;
    }

    W2BYTEMSK(REG_ROTATE_03_L, u16SrcFm, REG_ROTATE_SRC_FMT_MSK);
}

void Hal_ROT_set_degree(XC_ROT_DEGREE enDegree)
{
    MS_U16 u16Rot_Degree;

    switch(enDegree)
    {
    default:
    case XC_ROT_DEGREE_90:
        u16Rot_Degree = REG_ROTATE_DEGREE_90;
        break;
    case XC_ROT_DEGREE_180:
        u16Rot_Degree = REG_ROTATE_DEGREE_180;
        break;
    case XC_ROT_DEGREE_270:
        u16Rot_Degree = REG_ROTATE_DEGREE_270;
        break;
    }

    W2BYTEMSK(REG_ROTATE_03_L, u16Rot_Degree, REG_ROTATE_DEGREE_MSK);
}

void Hal_ROT_set_src_base(MS_U32 u32addr)
{
    MS_U16 u16BaseLo, u16BaseHi;

    u16BaseLo = (MS_U16)((u32addr >> ROT_EMI_BYTE_PER_PIXLE) & 0x0000FFFF);
    u16BaseHi = (MS_U16)(((u32addr >> ROT_EMI_BYTE_PER_PIXLE) & 0xFFFF0000) >> 16);
    W2BYTEMSK(REG_ROTATE_04_L, u16BaseLo, 0xFFFF);
    W2BYTEMSK(REG_ROTATE_05_L, u16BaseHi, 0x0FFF);

}


void Hal_ROT_set_dest_base(MS_U32 u32addr)
{
    MS_U16 u16BaseLo, u16BaseHi;

    u16BaseLo = (MS_U16)(( u32addr >> ROT_EMI_BYTE_PER_PIXLE) & 0x0000FFFF);
    u16BaseHi = (MS_U16)((( u32addr >> ROT_EMI_BYTE_PER_PIXLE) & 0xFFFF0000) >> 16);
    W2BYTEMSK(REG_ROTATE_06_L, u16BaseLo, 0xFFFF);
    W2BYTEMSK(REG_ROTATE_07_L, u16BaseHi, 0x0FFF);

}

void Hal_ROT_set_src_pitch(MS_U16 u16pitch)
{
    W2BYTEMSK(REG_ROTATE_08_L, (u16pitch >>ROT_PITCH_BYTE_PER_PIEXL), 0x0FFF);
}

void Hal_ROT_set_dest_pitch(MS_U16 u16pitch)
{
    W2BYTEMSK(REG_ROTATE_09_L, (u16pitch >>ROT_PITCH_BYTE_PER_PIEXL), 0x0FFF);
}


void Hal_ROT_set_imi_base(MS_U32 u32base_0, MS_U32 u32base_1)
{
    MS_U16 u16BaseLo, u16BaseHi;

    u16BaseLo = (MS_U16)((u32base_0 >> ROT_IMI_BYTE_PER_PIXLE) & 0x0000FFFF);
    u16BaseHi = (MS_U16)(((u32base_0 >> ROT_IMI_BYTE_PER_PIXLE) & 0xFFFF0000) >> 16);
    W2BYTEMSK(REG_ROTATE_0A_L, u16BaseLo, 0xFFFF);
    W2BYTEMSK(REG_ROTATE_0B_L, u16BaseHi, 0x0FFF);

    u16BaseLo = (MS_U16)((u32base_1 >> ROT_IMI_BYTE_PER_PIXLE) & 0x0000FFFF);
    u16BaseHi = (MS_U16)(((u32base_1 >> ROT_IMI_BYTE_PER_PIXLE) & 0xFFFF0000) >> 16);
    W2BYTEMSK(REG_ROTATE_0C_L, u16BaseLo, 0xFFFF);
    W2BYTEMSK(REG_ROTATE_0D_L, u16BaseHi, 0x0FFF);

}

void Hal_ROT_set_trig(MS_BOOL bEn)
{
    W2BYTEMSK(REG_ROTATE_00_L, bEn ? 1 : 0, REG_ROTATE_TRIG_MSK);

}

void Hal_ROT_set_emi_wirteprotect(MS_BOOL bEn, MS_U32 u32Addr)
{
    MS_U16 u16BaseLo, u16BaseHi;
    u16BaseLo = (MS_U16)((u32Addr >> ROT_EMI_BYTE_PER_PIXLE) & 0x0000FFFF);
    u16BaseHi = (MS_U16)(((u32Addr >> ROT_EMI_BYTE_PER_PIXLE) & 0xFFFF0000) >> 16);
    if(bEn)
    {
        W2BYTEMSK(REG_ROTATE_10_L, u16BaseLo, 0xFFFF);
        W2BYTEMSK(REG_ROTATE_11_L, u16BaseHi, 0x0FFF);
        W2BYTEMSK(REG_ROTATE_00_L, REG_ROTATE_EMI_WLIMIT_EN, REG_ROTATE_EMI_WLIMIT_MSK);
    }
    else
    {
        W2BYTEMSK(REG_ROTATE_00_L, 0, REG_ROTATE_EMI_WLIMIT_MSK);
        W2BYTEMSK(REG_ROTATE_10_L, u16BaseLo, 0xFFFF);
        W2BYTEMSK(REG_ROTATE_11_L, u16BaseHi, 0x0FFF);
    }
}

void Hal_ROT_set_imi_wirteprotect_base0(MS_BOOL bEn, MS_U32 u32Addr)
{
    MS_U16 u16BaseLo, u16BaseHi;
    u16BaseLo = (MS_U16)((u32Addr >> ROT_IMI_BYTE_PER_PIXLE) & 0x0000FFFF);
    u16BaseHi = (MS_U16)(((u32Addr >> ROT_IMI_BYTE_PER_PIXLE) & 0xFFFF0000) >> 16);
    if(bEn)
    {
        W2BYTEMSK(REG_ROTATE_12_L, u16BaseLo, 0xFFFF);
        W2BYTEMSK(REG_ROTATE_13_L, u16BaseHi, 0x0FFF);
        W2BYTEMSK(REG_ROTATE_00_L, REG_ROTATE_IMI0_WLIMIT_EN, REG_ROTATE_IMI0_WLIMIT_MSK);
    }
    else
    {
        W2BYTEMSK(REG_ROTATE_00_L, 0, REG_ROTATE_IMI0_WLIMIT_MSK);
        W2BYTEMSK(REG_ROTATE_12_L, u16BaseLo, 0xFFFF);
        W2BYTEMSK(REG_ROTATE_13_L, u16BaseHi, 0x0FFF);
    }
}

void Hal_ROT_set_imi_wirteprotect_base1(MS_BOOL bEn, MS_U32 u32Addr)
{
    MS_U16 u16BaseLo, u16BaseHi;
    u16BaseLo = (MS_U16)((u32Addr >> ROT_IMI_BYTE_PER_PIXLE) & 0x0000FFFF);
    u16BaseHi = (MS_U16)(((u32Addr >> ROT_IMI_BYTE_PER_PIXLE) & 0xFFFF0000) >> 16);
    if(bEn)
    {
        W2BYTEMSK(REG_ROTATE_14_L, u16BaseLo, 0xFFFF);
        W2BYTEMSK(REG_ROTATE_15_L, u16BaseHi, 0x0FFF);
        W2BYTEMSK(REG_ROTATE_00_L, REG_ROTATE_IMI1_WLIMIT_EN, REG_ROTATE_IMI1_WLIMIT_MSK);
    }
    else
    {
        W2BYTEMSK(REG_ROTATE_00_L, 0, REG_ROTATE_IMI1_WLIMIT_MSK);
        W2BYTEMSK(REG_ROTATE_14_L, u16BaseLo, 0xFFFF);
        W2BYTEMSK(REG_ROTATE_15_L, u16BaseHi, 0x0FFF);
    }
}

#undef  MHAL_ROTATE_C


