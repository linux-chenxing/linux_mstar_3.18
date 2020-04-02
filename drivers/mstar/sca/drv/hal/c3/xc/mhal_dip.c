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
#define  MHAL_DIP_C

#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/kernel.h>
#include <linux/wait.h>
#include <linux/irqreturn.h>
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

#include "drv_sc_ip.h"
#include "mvideo_context.h"
#include "mhal_sc.h"
#include "mhal_menuload.h"
#include "halCHIP.h"
#include "drv_sc_menuload.h"
#include "drv_sc_isr.h"
#include "mhal_dip.h"

void Hal_DIP_set_src_op(MS_BOOL bEn)
{
    SC_W2BYTEMSK(REG_SC2_BK0F_57_L, bEn ? BIT(11) : 0, BIT(11)); // op capture enable
    SC_W2BYTEMSK(REG_SC_BK36_02_L, 0x0000, 0x0300);
}

void Hal_DIP_set_src_ip_sub(MS_BOOL bEn)
{
    SC_W2BYTEMSK(REG_SC2_BK04_2D_L, bEn ? BIT(1) : 0, BIT(1)); // Ip sub capture enable
    SC_W2BYTEMSK(REG_SC2_BK04_2D_L, bEn ? BIT(0) : 0, BIT(0));

    SC_W2BYTEMSK(REG_SC_BK36_02_L, 0x0100, 0x0300);
}

void Hal_DIP_set_src_ip_main(MS_BOOL bEn)
{
    SC_W2BYTEMSK(REG_SC2_BK02_2D_L, bEn ? BIT(1) : 0, BIT(1)); // Ip sub capture enable
    SC_W2BYTEMSK(REG_SC2_BK02_2D_L, bEn ? BIT(0) : 0, BIT(0));

    SC_W2BYTEMSK(REG_SC_BK36_02_L, 0x0200, 0x0300);
}

void Hal_DIP_set_444_to_422(MS_BOOL bEn)
{
    SC_W2BYTEMSK(REG_SC_BK34_30_L, bEn ? BIT(0) : 0, BIT(0));
}

void Hal_DIP_set_src_422(MS_BOOL bEn)
{
    SC_W2BYTEMSK(REG_SC_BK34_30_L, bEn ? BIT(5) : 0, BIT(5));
}

void Hal_DIP_set_clip_window(MS_WINDOW_TYPE stWin)
{
    stWin.x = stWin.x == 0 ? 1 : stWin.x; // the counter is from 1
    stWin.y = stWin.y == 0 ? 1 : stWin.y;
    SC_W2BYTEMSK(REG_SC_BK34_21_L, stWin.x, 0x0FFF);
    SC_W2BYTEMSK(REG_SC_BK34_22_L, stWin.x+stWin.width-1, 0x0FFF);
    SC_W2BYTEMSK(REG_SC_BK34_23_L, stWin.y, 0x0FFF);
    SC_W2BYTEMSK(REG_SC_BK34_24_L, stWin.y+stWin.height-1, 0x0FFF);
}

void Hal_DIP_set_clip_en(MS_BOOL bEn)
{
    SC_W2BYTEMSK(REG_SC_BK34_20_L, bEn ? BIT(0) : 0, BIT(0));
}


void Hal_DIP_set_dyn_clk_gate(MS_BOOL bEn)
{
    MDrv_WriteByteMask(REG_DIP_DYN_CLK_GATE, bEn ? DIP_DYN_CLK_GATE : 0, REG_DIP_DYN_CLK_GATE_MSK);
}

void Hal_DIP_set_pwd_off(MS_BOOL bEn)
{
    SC_W2BYTEMSK(REG_SC_BK36_02_L, bEn ? BIT(10) : 0, BIT(10));
}

void Hal_DIP_set_dip_frame_num(MS_U16 u16Num)
{
    u16Num = u16Num > 1 ? u16Num - 1 : 0;
    SC_W2BYTEMSK(REG_SC_BK36_01_L, u16Num, 0x0007);
}

void Hal_DIP_set_c_base_addr(MS_U32 u32Addr, MS_U32 u32MIU_BUS)
{
    MS_U16 u16AddrLo, u16AddrHi;

    u16AddrLo = (MS_U16)((u32Addr>>u32MIU_BUS) & 0x0000FFFF);
    u16AddrHi = (MS_U16)(((u32Addr>>u32MIU_BUS) & 0xFFFF0000) >> 16);
    SC_W2BYTEMSK(REG_SC_BK36_20_L, u16AddrLo, 0xFFFF);
    SC_W2BYTEMSK(REG_SC_BK36_21_L, u16AddrHi, 0x07FF);
}

void Hal_DIP_set_base_addr(MS_U32 u32Addr, MS_U32 u32MIU_Bus)
{
    MS_U16 u16AddrLo, u16AddrHi;

    u16AddrLo = (MS_U16)((u32Addr>>u32MIU_Bus) & 0x0000FFFF);
    u16AddrHi = (MS_U16)(((u32Addr>>u32MIU_Bus) & 0xFFFF0000) >> 16);
    SC_W2BYTEMSK(REG_SC_BK36_10_L, u16AddrLo, 0xFFFF);
    SC_W2BYTEMSK(REG_SC_BK36_11_L, u16AddrHi, 0x07FF);
}

void Hal_DIP_set_base_offset(MS_U32 u32Offset)
{
    MS_U16 u16OffsetLo, u16OffsetHi;

    u16OffsetLo = (MS_U16)((u32Offset) & 0x0000FFFF);
    u16OffsetHi = (MS_U16)(((u32Offset) & 0xFFFF0000) >> 16);

    SC_W2BYTEMSK(REG_SC_BK36_50_L, u16OffsetLo, 0xFFFF);
    SC_W2BYTEMSK(REG_SC_BK36_51_L, u16OffsetHi, 0x00FF);
}

void Hal_DIP_set_c_base_offset(MS_U32 u32Offset)
{
    MS_U16 u16OffsetLo, u16OffsetHi;

    u16OffsetLo = (MS_U16)((u32Offset) & 0x0000FFFF);
    u16OffsetHi = (MS_U16)(((u32Offset) & 0xFFFF0000) >> 16);

    SC_W2BYTEMSK(REG_SC_BK36_60_L, u16OffsetLo, 0xFFFF);
    SC_W2BYTEMSK(REG_SC_BK36_61_L, u16OffsetHi, 0x00FF);
}


void Hal_DIP_set_src_size(MS_U16 u16Width, MS_U16 u16Height)
{
   SC_W2BYTEMSK(REG_SC_BK36_1F_L, u16Width, 0x0FFF);
   SC_W2BYTEMSK(REG_SC_BK36_2F_L, u16Height, 0x0FFF);
}

void Hal_DIP_set_base_pitch(MS_U16 u16Pitch)
{
    SC_W2BYTEMSK(REG_SC_BK36_3F_L, u16Pitch, 0x0FFF);
}


void Hal_DIP_set_wreq_thrd_max(MS_U8 u8Thrd, MS_U8 u8Max)
{
    SC_W2BYTEMSK(REG_SC_BK36_6F_L, u8Thrd, 0x003F);
    SC_W2BYTEMSK(REG_SC_BK36_6F_L, ((MS_U16)u8Max)<<8, 0x7F00);
}

void Hal_DIP_set_output_fmt(XC_DIP_DST_FMT eFMT)
{
    MS_U16 u16val;
    MS_BOOL bblk_tile_en;

    switch(eFMT)
    {
    default:
    case DIP_DST_FMT_YC422:
        bblk_tile_en = FALSE;
        u16val = 0<<4;
        break;
    case DIP_DST_FMT_RGB565:
        bblk_tile_en = FALSE;
        u16val = 1<<4;
        break;
   case DIP_DST_FMT_ARGB8888:
        bblk_tile_en = FALSE;
        u16val = 2<<4;
        break;
    case DIP_DST_FMT_YC420_MFE:
        bblk_tile_en = TRUE;
        u16val = 3<<4;
        break;

    case DIP_DST_FMT_YC420_MVOP:
        bblk_tile_en = FALSE;
        u16val = 3<<4;
        break;

    }

    SC_W2BYTEMSK(REG_SC_BK36_7E_L, bblk_tile_en ? BIT(15) : 0, BIT(15));
    SC_W2BYTEMSK(REG_SC_BK36_03_L, bblk_tile_en ? BIT(15) : 0, BIT(15));
    SC_W2BYTEMSK(REG_SC_BK36_01_L, u16val, 0x0030);

}

void Hal_DIP_set_422to420_ratio(MS_U16 u16val)
{
    SC_W2BYTEMSK(REG_SC_BK36_01_L, u16val<<8, 0x0F00);
}

void Hal_DIP_set_tile_req_num(MS_U16 u16val)
{
    SC_W2BYTEMSK(REG_SC_BK36_7E_L, u16val, 0x007F);
}

void Hal_DIP_set_wlimit_base_addr(MS_U32 u32Addr, MS_U32 u32MIU_Bus)
{
    MS_U16 u16AddrLo, u16AddrHi;

    u16AddrLo = (MS_U16)((u32Addr>>u32MIU_Bus) & 0x0000FFFF);
    u16AddrHi = (MS_U16)(((u32Addr>>u32MIU_Bus) & 0xFFFF0000) >> 16);

    SC_W2BYTEMSK(REG_SC_BK36_30_L, u16AddrLo, 0xFFFF);
    SC_W2BYTEMSK(REG_SC_BK36_31_L, u16AddrHi, 0x07FF);
}

void Hal_DIP_set_wlimit_c_base_addr(MS_U32 u32Addr, MS_U32 u32MIU_Bus)
{
    MS_U16 u16AddrLo, u16AddrHi;

    u16AddrLo = (MS_U16)((u32Addr>>u32MIU_Bus) & 0x0000FFFF);
    u16AddrHi = (MS_U16)(((u32Addr>>u32MIU_Bus) & 0xFFFF0000) >> 16);
    SC_W2BYTEMSK(REG_SC_BK36_40_L, u16AddrLo, 0xFFFF);
    SC_W2BYTEMSK(REG_SC_BK36_41_L, u16AddrHi, 0x07FF);
}

void Hal_DIP_set_wlimit_en(MS_BOOL bEn)
{
    SC_W2BYTEMSK(REG_SC_BK36_31_L, bEn ? BIT(15) : 0, BIT(15));
}

void Hal_DIP_set_y2r_en(MS_BOOL bEn)
{
    SC_W2BYTEMSK(REG_SC_BK36_7E_L, bEn ? BIT(11) : 0, BIT(11));
}

void Hal_DIP_set_dwin_wt_once(MS_BOOL bEn)
{
    SC_W2BYTEMSK(REG_SC_BK36_03_L, bEn ? BIT(8) : 0, BIT(8));
    SC_W2BYTEMSK(REG_SC_BK36_01_L, BIT(6), BIT(6));
}

void Hal_DIP_set_dwin_wt_once_trig(MS_BOOL bEn)
{
    SC_W2BYTEMSK(REG_SC_BK36_03_L, bEn ? BIT(9) : 0, BIT(9));
}

#undef  MHAL_DIP_C
