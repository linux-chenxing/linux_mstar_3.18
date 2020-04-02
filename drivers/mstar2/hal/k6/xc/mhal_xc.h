///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2008 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_XC_H_
#define _MHAL_XC_H_

#include "mdrv_xc_st.h"
#include "color_format_input.h"
#include "dolby_hdr_mem.h"

#ifdef _HAL_GFLIP_C
#define INTERFACE
#else
#define INTERFACE                               extern
#endif

#define SUPPORT_TCH
#define TCH_DEV 1
#define SUPPORT_SEAMLESS_HDR
//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

#define XC_FILM_DRIVER_VER               2

#define XC_FUNCTION                      1

#define XC_FRC_R2_SW_TOGGLE              1

#define IS_DOLBY_HDR(eWindow) ((_stCfdHdr[eWindow].u8HdrType & E_KDRV_XC_CFD_HDR_TYPE_DOLBY) == E_KDRV_XC_CFD_HDR_TYPE_DOLBY)
#define IS_OPEN_HDR(eWindow) ((_stCfdHdr[eWindow].u8HdrType & E_KDRV_XC_CFD_HDR_TYPE_OPEN) == E_KDRV_XC_CFD_HDR_TYPE_OPEN)
#define IS_HLG_HDR(eWindow) ((_stCfdHdr[eWindow].u8HdrType & E_KDRV_XC_CFD_HDR_TYPE_HLG) == E_KDRV_XC_CFD_HDR_TYPE_HLG)
#define IS_TC_HDR(eWindow) ((_stCfdHdr[eWindow].u8HdrType & E_KDRV_XC_CFD_HDR_TYPE_TCH) == E_KDRV_XC_CFD_HDR_TYPE_TCH)
//#define IS_HDMI_OPEN(eWindow) (((_stCfdHdr[eWindow].u8HdrType == E_KDRV_XC_CFD_HDR_TYPE_OPEN) && (_enInputSourceType == E_KDRV_XC_INPUT_SOURCE_HDMI)) ? 1 : 0)
//#define IS_OTT_OPEN(eWindow) (((_stCfdHdr[eWindow].u8HdrType == E_KDRV_XC_CFD_HDR_TYPE_OPEN) && (_enInputSourceType == E_KDRV_XC_INPUT_SOURCE_OTT)) ? 1 : 0)

#define IS_HDMI_DOLBY (((_enHDRType == E_KDRV_XC_HDR_DOLBY) && (_enInputSourceType == E_KDRV_XC_INPUT_SOURCE_HDMI)) ? 1 : 0)
#define IS_OTT_DOLBY (((_enHDRType == E_KDRV_XC_HDR_DOLBY) && (_enInputSourceType == E_KDRV_XC_INPUT_SOURCE_OTT)) ? 1 : 0)
#define IS_HDMI_OPEN (((_enHDRType == E_KDRV_XC_HDR_OPEN) && (_enInputSourceType == E_KDRV_XC_INPUT_SOURCE_HDMI)) ? 1 : 0)
#define IS_OTT_OPEN (((_enHDRType == E_KDRV_XC_HDR_OPEN) && (_enInputSourceType == E_KDRV_XC_INPUT_SOURCE_OTT)) ? 1 : 0)


#if XC_FUNCTION
#define BYTE                                    MS_U8
#define WORD                                    MS_U16
#define DWORD                                   MS_U32
#define printf                                  printk
#define DLC_DEBUG(x)                            //(x)


#define REG_ADDR_VOP_SCREEN_CONTROL             L_BK_VOP(0x19)

#define REG_ADDR_OP_SW_SUB_ENABLE               L_BK_OP(0x10)

#define PRJ_MCNR

#define REG_LPLL_11_L                           (0x103100 | 0x11<< 1)
#define REG_LPLL_13_L                           (0x103100 | 0x13<< 1)

#define REG_SC_BK00_10_L                        (0x130000 | 0x10<< 1)
#define REG_SC_BK00_11_L                        (0x130000 | 0x11<< 1)
#define REG_SC_BK00_12_L                        (0x130000 | 0x12<< 1)
#define REG_SC_BK00_13_L                        (0x130000 | 0x13<< 1)
#define REG_SC_BK00_14_L                        (0x130000 | 0x14<< 1)
#define REG_SC_BK00_15_L                        (0x130000 | 0x15<< 1)

#define REG_SC_VSYNC_IRQ                        REG_SC_BK00_10_L
#define REG_SC_CLEAR_VSYNC_IRQ                  REG_SC_BK00_12_L

#define REG_SC_BK01_40_L                        (0x130100 | 0x40<< 1)

#define REG_SC_BK01_42_L                        (0x130100 | 0x42<< 1)

#define REG_SC_BK01_61_L                        (0x130100 | 0x61<< 1)
#define REG_SC_BK01_61_H                        (0x130100 | ((0x61<< 1)+1))

#define REG_SC_BK02_10_L                        (0x130200 | (0x10<<1))
#define REG_SC_BK02_2B_L                        (0x130200 | (0x2B<< 1))
#define REG_SC_BK02_2D_L                        (0x130200 | (0x2D<< 1))
#define REG_SC_BK02_2E_L                        (0x130200 | (0x2E<< 1))
#define REG_SC_BK02_2F_L                        (0x130200 | (0x2F<< 1))
#define REG_SC_BK02_30_L                        (0x130200 | (0x30<< 1))
#define REG_SC_BK02_31_L                        (0x130200 | (0x31<< 1))
#define REG_SC_BK02_32_L                        (0x130200 | (0x32<< 1))
#define REG_SC_BK02_33_L                        (0x130200 | (0x33<< 1))
#define REG_SC_BK02_34_L                        (0x130200 | (0x34<< 1))
#define REG_SC_BK02_35_L                        (0x130200 | (0x35<< 1))

#define REG_SC_BK10_19_L                        (0x131000 | 0x19<< 1)

#define REG_SC_BK12_01_L                        (0x131200 | 0x01<< 1)

#define REG_SC_BK0A_02_L                        (0x130A00 | 0x02<< 1)
#define REG_SC_BK0A_05_L                        (0x130A00 | 0x05<< 1)
#define REG_SC_BK0A_06_L                        (0x130A00 | 0x06<< 1)
#define REG_SC_BK0A_07_L                        (0x130A00 | 0x07<< 1)
#define REG_SC_BK0A_0A_L                        (0x130A00 | 0x0A<< 1)
#define REG_SC_BK0A_0B_L                        (0x130A00 | 0x0B<< 1)
#define REG_SC_BK0A_0C_L                        (0x130A00 | 0x0C<< 1)
#define REG_SC_BK0A_0D_L                        (0x130A00 | 0x0D<< 1)
#define REG_SC_BK0A_0F_L                        (0x130A00 | 0x0F<< 1)

#define REG_SC_BK0A_10_L                        (0x130A00 | 0x10<< 1)
#define REG_SC_BK0A_15_L                        (0x130A00 | 0x15<< 1)
#define REG_SC_BK0A_16_L                        (0x130A00 | 0x16<< 1)
#define REG_SC_BK0A_17_L                        (0x130A00 | 0x17<< 1)
#define REG_SC_BK0A_1E_L                        (0x130A00 | 0x1E<< 1)
#define REG_SC_BK0A_1F_L                        (0x130A00 | 0x1F<< 1)

#define REG_SC_BK0A_20_L                        (0x130A00 | 0x20<< 1)
#define REG_SC_BK0A_21_L                        (0x130A00 | 0x21<< 1)
#define REG_SC_BK0A_23_L                        (0x130A00 | 0x23<< 1)
#define REG_SC_BK0A_24_L                        (0x130A00 | 0x24<< 1)
#define REG_SC_BK0A_25_L                        (0x130A00 | 0x25<< 1)
#define REG_SC_BK0A_41_L                        (0x130A00 | 0x41<< 1)
#define REG_SC_BK0A_4A_L                        (0x130A00 | 0x4A<< 1)
#define REG_SC_BK0A_4B_L                        (0x130A00 | 0x4B<< 1)

#define REG_SC_BK0A_57_L                        (0x130A00 | 0x57<< 1)
#define REG_SC_BK0A_5C_L                        (0x130A00 | 0x5C<< 1)
#define REG_SC_BK0A_5E_L                        (0x130A00 | 0x5E<< 1)

#define REG_SC_BK12_03_L                        (0x131200 | 0x03<< 1)
#define REG_SC_BK12_0F_L                        (0x131200 | 0x0F<< 1)
#define REG_SC_BK12_30_L                        (0x131200 | 0x30<< 1)
#define REG_SC_BK12_33_L                        (0x131200 | 0x33<< 1)
#define REG_SC_BK12_3A_L                        (0x131200 | 0x3A<< 1)
#define REG_SC_BK22_2A_L                        (0x132200 | 0x2A<< 1)
#define REG_SC_BK22_7A_L                        (0x132200 | 0x7A<< 1)
#define REG_SC_BK22_7C_L                        (0x132200 | 0x7C<< 1)
#define REG_SC_BK22_7E_L                        (0x132200 | 0x7E<< 1)

#define REG_SC_BK2A_02_L                        (0x132A00 | 0x02<< 1)
#define REG_SC_BK2A_29_L                        (0x132A00 | 0x29<< 1)
#define REG_SC_BK2A_2A_L                        (0x132A00 | 0x2A<< 1)
#define REG_SC_BK2A_2F_L                        (0x132A00 | 0x2F<< 1)
#define REG_SC_BK2A_7C_L                        (0x132A00 | 0x7C<< 1)
#define REG_SC_BK2A_7D_L                        (0x132A00 | 0x7D<< 1)
#define REG_SC_BK2A_7E_L                        (0x132A00 | 0x7E<< 1)

#define REG_SC_BK2F_20_L                        (0x132F00 | 0x20<< 1)

#define REG_SC_BK30_01_L                        (0x133000 | (0x01<< 1))
#define REG_SC_BK30_02_L                        (0x133000 | (0x02<< 1))
#define REG_SC_BK30_02_H                        (0x133000 | ((0x02<< 1)+1))
#define REG_SC_BK30_03_L                        (0x133000 | (0x03<< 1))
#define REG_SC_BK30_04_L                        (0x133000 | (0x04<< 1))

#define REG_SC_BK30_06_L                        (0x133000 | (0x06<< 1))
#define REG_SC_BK30_06_H                        (0x133000 | ((0x06<< 1)+1))
#define REG_SC_BK30_07_L                        (0x133000 | (0x07<< 1))
#define REG_SC_BK30_07_H                        (0x133000 | ((0x07<< 1)+1))
#define REG_SC_BK30_08_L                        (0x133000 | (0x08<< 1))
#define REG_SC_BK30_08_H                        (0x133000 | ((0x08<< 1)+1))
#define REG_SC_BK30_09_L                        (0x133000 | (0x09<< 1))
#define REG_SC_BK30_09_H                        (0x133000 | ((0x09<< 1)+1))
#define REG_SC_BK30_0A_L                        (0x133000 | (0x0A<< 1))
#define REG_SC_BK30_0A_H                        (0x133000 | ((0x0A<< 1)+1))
#define REG_SC_BK30_0B_L                        (0x133000 | (0x0B<< 1))
#define REG_SC_BK30_0B_H                        (0x133000 | ((0x0B<< 1)+1))
#define REG_SC_BK30_0C_L                        (0x133000 | (0x0C<< 1))
#define REG_SC_BK30_0C_H                        (0x133000 | ((0x0C<< 1)+1))
#define REG_SC_BK30_0D_L                        (0x133000 | (0x0D<< 1))
#define REG_SC_BK30_0D_H                        (0x133000 | ((0x0D<< 1)+1))
#define REG_SC_BK30_0E_L                        (0x133000 | (0x0E<< 1))
#define REG_SC_BK30_0E_H                        (0x133000 | ((0x0E<< 1)+1))
#define REG_SC_BK30_0F_L                        (0x133000 | (0x0F<< 1))
#define REG_SC_BK30_0F_H                        (0x133000 | ((0x0F<< 1)+1))

#define REG_SC_BK42_05_L                        (0x134200 | 0x05<< 1)
#define REG_SC_BK42_08_L                        (0x134200 | 0x08<< 1)
#define REG_SC_BK42_09_L                        (0x134200 | 0x09<< 1)
#define REG_SC_BK42_10_L                        (0x134200 | 0x10<< 1)
#define REG_SC_BK42_11_L                        (0x134200 | 0x11<< 1)
#define REG_SC_BK42_19_L                        (0x134200 | 0x19<< 1)
#define REG_SC_BK42_50_L                        (0x134200 | 0x50<< 1)
#define REG_SC_BK42_52_L                        (0x134200 | 0x52<< 1)
#define REG_SC_BK42_53_L                        (0x134200 | 0x53<< 1)
#define REG_SC_BK42_54_L                        (0x134200 | 0x54<< 1)
#define REG_SC_BK42_55_L                        (0x134200 | 0x55<< 1)

#define REG_SC_BK62_06_L                        (0x136200 | 0x06<< 1)
#define REG_SC_BK62_0E_L                        (0x136200 | 0x0E<< 1)
#define REG_SC_BK62_08_L                        (0x136200 | 0x08<< 1)
#define REG_SC_BK63_61_L                        (0x136300 | 0x61<< 1)

#define REG_SC_BK67_01_L                        (0x136700 | 0x01<< 1)
#define REG_SC_BK67_01_H                        (0x136700 | ((0x01<< 1)+1))
#define REG_SC_BK67_02_L                        (0x136700 | 0x02<< 1)
#define REG_SC_BK67_02_H                        (0x136700 | ((0x02<< 1)+1))
#define REG_SC_BK67_03_L                        (0x136700 | 0x03<< 1)
#define REG_SC_BK67_03_H                        (0x136700 | ((0x03<< 1)+1))
#define REG_SC_BK67_04_L                        (0x136700 | 0x04<< 1)
#define REG_SC_BK67_04_H                        (0x136700 | ((0x04<< 1)+1))
#define REG_SC_BK67_05_L                        (0x136700 | 0x05<< 1)
#define REG_SC_BK67_05_H                        (0x136700 | ((0x05<< 1)+1))
#define REG_SC_BK67_06_L                        (0x136700 | 0x06<< 1)
#define REG_SC_BK67_06_H                        (0x136700 | ((0x06<< 1)+1))
#define REG_SC_BK67_07_L                        (0x136700 | 0x07<< 1)
#define REG_SC_BK67_07_H                        (0x136700 | ((0x07<< 1)+1))
#define REG_SC_BK67_11_L                        (0x136700 | 0x11<< 1)
#define REG_SC_BK67_11_H                        (0x136700 | ((0x11<< 1)+1))
#define REG_SC_BK67_12_L                        (0x136700 | 0x12<< 1)
#define REG_SC_BK67_12_H                        (0x136700 | ((0x12<< 1)+1))
#define REG_SC_BK67_13_L                        (0x136700 | 0x13<< 1)
#define REG_SC_BK67_13_H                        (0x136700 | ((0x13<< 1)+1))
#define REG_SC_BK67_16_L                        (0x136700 | 0x16<< 1)
#define REG_SC_BK67_16_H                        (0x136700 | ((0x16<< 1)+1))
#define REG_SC_BK67_17_L                        (0x136700 | 0x17<< 1)
#define REG_SC_BK67_17_H                        (0x136700 | ((0x17<< 1)+1))

#define REG_SC_BK67_1A_L                        (0x136700 | 0x1A<< 1)
#define REG_SC_BK67_1A_H                        (0x136700 | ((0x1A<< 1)+1))

#define REG_SC_BK67_1D_L                        (0x136700 | 0x1D<< 1)
#define REG_SC_BK67_1D_H                        (0x136700 | ((0x1D<< 1)+1))

#define REG_SC_BK67_20_L                        (0x136700 | 0x20<< 1)
#define REG_SC_BK67_20_H                        (0x136700 | ((0x20<< 1)+1))

#define REG_SC_BK67_21_L                        (0x136700 | 0x21<< 1)
#define REG_SC_BK67_21_H                        (0x136700 | ((0x21<< 1)+1))
#define REG_SC_BK67_22_L                        (0x136700 | 0x22<< 1)
#define REG_SC_BK67_22_H                        (0x136700 | ((0x22<< 1)+1))
#define REG_SC_BK67_23_L                        (0x136700 | 0x23<< 1)
#define REG_SC_BK67_23_H                        (0x136700 | ((0x23<< 1)+1))
#define REG_SC_BK67_24_L                        (0x136700 | 0x24<< 1)
#define REG_SC_BK67_24_H                        (0x136700 | ((0x24<< 1)+1))
#define REG_SC_BK67_28_L                        (0x136700 | 0x28<< 1)
#define REG_SC_BK67_28_H                        (0x136700 | ((0x28<< 1)+1))
#define REG_SC_BK67_29_L                        (0x136700 | 0x29<< 1)
#define REG_SC_BK67_29_H                        (0x136700 | ((0x29<< 1)+1))
#define REG_SC_BK67_2A_L                        (0x136700 | 0x2A<< 1)
#define REG_SC_BK67_2A_H                        (0x136700 | ((0x2A<< 1)+1))
#define REG_SC_BK67_2B_L                        (0x136700 | 0x2B<< 1)
#define REG_SC_BK67_2B_H                        (0x136700 | ((0x2B<< 1)+1))
#define REG_SC_BK67_2C_L                        (0x136700 | 0x2C<< 1)
#define REG_SC_BK67_2C_H                        (0x136700 | ((0x2C<< 1)+1))
#define REG_SC_BK67_32_L                        (0x136700 | 0x32<< 1)
#define REG_SC_BK67_32_H                        (0x136700 | ((0x32<< 1)+1))
#define REG_SC_BK67_33_L                        (0x136700 | 0x33<< 1)
#define REG_SC_BK67_33_H                        (0x136700 | ((0x33<< 1)+1))
#define REG_SC_BK67_34_L                        (0x136700 | 0x34<< 1)
#define REG_SC_BK67_34_H                        (0x136700 | ((0x34<< 1)+1))
#define REG_SC_BK67_77_L                        (0x136700 | 0x77<< 1)
#define REG_SC_BK67_77_H                        (0x136700 | ((0x77<< 1)+1))

#define REG_SC_BK79_02_L                        (0x137900 | 0x02<< 1)
#define REG_SC_BK79_02_H                        (0x137900 | ((0x02<< 1)+1))
#define REG_SC_BK79_03_L                        (0x137900 | 0x03<< 1)
#define REG_SC_BK79_03_H                        (0x137900 | ((0x03<< 1)+1))
#define REG_SC_BK79_04_L                        (0x137900 | 0x04<< 1)
#define REG_SC_BK79_04_H                        (0x137900 | ((0x04<< 1)+1))
#define REG_SC_BK79_06_L                        (0x137900 | 0x06<< 1)
#define REG_SC_BK79_06_H                        (0x137900 | ((0x06<< 1)+1))
#define REG_SC_BK79_07_L                        (0x137900 | 0x07<< 1)
#define REG_SC_BK79_07_H                        (0x137900 | ((0x07<< 1)+1))
#define REG_SC_BK79_08_L                        (0x137900 | 0x08<< 1)
#define REG_SC_BK79_08_H                        (0x137900 | ((0x08<< 1)+1))
#define REG_SC_BK79_09_L                        (0x137900 | 0x09<< 1)
#define REG_SC_BK79_09_H                        (0x137900 | ((0x09<< 1)+1))
#define REG_SC_BK79_10_L                        (0x137900 | 0x10<< 1)
#define REG_SC_BK79_10_H                        (0x137900 | ((0x10<< 1)+1))
#define REG_SC_BK79_11_L                        (0x137900 | 0x11<< 1)
#define REG_SC_BK79_11_H                        (0x137900 | ((0x11<< 1)+1))
#define REG_SC_BK79_12_L                        (0x137900 | 0x12<< 1)
#define REG_SC_BK79_12_H                        (0x137900 | ((0x12<< 1)+1))
#define REG_SC_BK79_13_L                        (0x137900 | 0x13<< 1)
#define REG_SC_BK79_13_H                        (0x137900 | ((0x13<< 1)+1))
#define REG_SC_BK79_14_L                        (0x137900 | 0x14<< 1)
#define REG_SC_BK79_14_H                        (0x137900 | ((0x14<< 1)+1))
#define REG_SC_BK79_18_L                        (0x137900 | 0x18<< 1)
#define REG_SC_BK79_18_H                        (0x137900 | ((0x18<< 1)+1))
#define REG_SC_BK79_19_L                        (0x137900 | 0x19<< 1)
#define REG_SC_BK79_19_H                        (0x137900 | ((0x19<< 1)+1))
#define REG_SC_BK79_1A_L                        (0x137900 | 0x1A<< 1)
#define REG_SC_BK79_1A_H                        (0x137900 | ((0x1A<< 1)+1))
#define REG_SC_BK79_1B_L                        (0x137900 | 0x1B<< 1)
#define REG_SC_BK79_1B_H                        (0x137900 | ((0x1B<< 1)+1))
#define REG_SC_BK79_1C_L                        (0x137900 | 0x1C<< 1)
#define REG_SC_BK79_1C_H                        (0x137900 | ((0x1C<< 1)+1))
#define REG_SC_BK79_1D_L                        (0x137900 | 0x1D<< 1)
#define REG_SC_BK79_1D_H                        (0x137900 | ((0x1D<< 1)+1))
#define REG_SC_BK79_1E_L                        (0x137900 | 0x1E<< 1)
#define REG_SC_BK79_1E_H                        (0x137900 | ((0x1E<< 1)+1))
#define REG_SC_BK79_1F_L                        (0x137900 | 0x1F<< 1)
#define REG_SC_BK79_1F_H                        (0x137900 | ((0x1F<< 1)+1))
#define REG_SC_BK79_7C_L                        (0x137900 | 0x7C<< 1)
#define REG_SC_BK79_7C_H                        (0x137900 | ((0x7C<< 1)+1))
#define REG_SC_BK79_7D_L                        (0x137900 | 0x7D<< 1)
#define REG_SC_BK79_7D_H                        (0x137900 | ((0x7D<< 1)+1))
#define REG_SC_BK79_7E_L                        (0x137900 | 0x7E<< 1)
#define REG_SC_BK79_7E_H                        (0x137900 | ((0x7E<< 1)+1))
#define REG_SC_BK79_7F_L                        (0x137900 | 0x7F<< 1)
#define REG_SC_BK79_7F_H                        (0x137900 | ((0x7F<< 1)+1))

#define REG_SC_BK7A_01_L                        (0x137A00 | 0x01<< 1)
#define REG_SC_BK7A_01_H                        (0x137A00 | ((0x01<< 1)+1))
#define REG_SC_BK7A_02_L                        (0x137A00 | 0x02<< 1)
#define REG_SC_BK7A_02_H                        (0x137A00 | ((0x02<< 1)+1))
#define REG_SC_BK7A_42_L                        (0x137A00 | 0x42<< 1)
#define REG_SC_BK7A_42_H                        (0x137A00 | ((0x42<< 1)+1))
#define REG_SC_BK7A_54_L                        (0x137A00 | 0x54<< 1)
#define REG_SC_BK7A_54_H                        (0x137A00 | ((0x54<< 1)+1))
#define REG_SC_BK7A_55_L                        (0x137A00 | 0x55<< 1)
#define REG_SC_BK7A_55_H                        (0x137A00 | ((0x55<< 1)+1))
#define REG_SC_BK7A_59_L                        (0x137A00 | 0x59<< 1)
#define REG_SC_BK7A_59_H                        (0x137A00 | ((0x59<< 1)+1))

#define REG_SC_BK7A_70_L                        (0x137A00 | 0x70<< 1)
#define REG_SC_BK7A_70_H                        (0x137A00 | ((0x70<< 1)+1))
#define REG_SC_BK7A_71_L                        (0x137A00 | 0x71<< 1)
#define REG_SC_BK7A_71_H                        (0x137A00 | ((0x71<< 1)+1))
#define REG_SC_BK7A_72_L                        (0x137A00 | 0x72<< 1)
#define REG_SC_BK7A_72_H                        (0x137A00 | ((0x72<< 1)+1))

#define REG_HDMI_DUAL_0_BASE                    0x173000UL
#define REG_HDMI_DUAL_0_30_L                    (REG_HDMI_DUAL_0_BASE + 0x60)
#define REG_HDMI_DUAL_0_31_L                    (REG_HDMI_DUAL_0_BASE + 0x62)
#define REG_HDMI_DUAL_0_32_L                    (REG_HDMI_DUAL_0_BASE + 0x64)
#define REG_HDMI_DUAL_0_33_L                    (REG_HDMI_DUAL_0_BASE + 0x66)
#define REG_HDMI_DUAL_0_34_L                    (REG_HDMI_DUAL_0_BASE + 0x68)
#define REG_HDMI_DUAL_0_35_L                    (REG_HDMI_DUAL_0_BASE + 0x6A)
#define REG_HDMI_DUAL_0_36_L                    (REG_HDMI_DUAL_0_BASE + 0x6C)
#define REG_HDMI_DUAL_0_37_L                    (REG_HDMI_DUAL_0_BASE + 0x6E)
#define REG_HDMI_DUAL_0_38_L                    (REG_HDMI_DUAL_0_BASE + 0x70)
#define REG_HDMI_DUAL_0_39_L                    (REG_HDMI_DUAL_0_BASE + 0x72)
#define REG_HDMI_DUAL_0_3A_L                    (REG_HDMI_DUAL_0_BASE + 0x74)
#define REG_HDMI_DUAL_0_3B_L                    (REG_HDMI_DUAL_0_BASE + 0x76)
#define REG_HDMI_DUAL_0_3C_L                    (REG_HDMI_DUAL_0_BASE + 0x78)
#define REG_HDMI_DUAL_0_3D_L                    (REG_HDMI_DUAL_0_BASE + 0x7A)

#define DOLBY_VSIF_LEN                          0x18
#define DOLBY_VSIF_PB_VALUE                     0x0

#define _BIT0                                   (0x0001)
#define _BIT1                                   (0x0002)
#define _BIT2                                   (0x0004)
#define _BIT3                                   (0x0008)
#define _BIT4                                   (0x0010)
#define _BIT5                                   (0x0020)
#define _BIT6                                   (0x0040)
#define _BIT7                                   (0x0080)

#define L_BK_VOP(_x_)                           (REG_SCALER_BASE | (REG_SC_BK_VOP << 8) | (_x_ << 1))
#define L_BK_DLC(_x_)                           (REG_SCALER_BASE | (REG_SC_BK_DLC << 8) | (_x_ << 1))
#define H_BK_DLC(_x_)                           (REG_SCALER_BASE | (REG_SC_BK_DLC << 8) | ((_x_ << 1)+1))
#define L_BK_OP(_x_)                            (REG_SCALER_BASE | (REG_SC_BK_OP << 8) | (_x_ << 1))

#define REG_SCALER_BASE                         0x130000
#define REG_SC_BK_VOP                           0x10
#define REG_SC_BK_DLC                           0x1A
#define REG_SC_BK_OP                            0x20

#if defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define REG_ADDR(addr)                          (*((volatile unsigned short int*)(mstar_pm_base + (addr << 1))))
#else
#define REG_ADDR(addr)                          (*((volatile unsigned short int*)(0xFD000000 + (addr << 1))))
#endif

#if XC_FRC_R2_SW_TOGGLE
#define REG_FRC_CPUINT_BASE                     (REG_INT_FRC_BASE + (0x640UL<<1))
#define FRC_CPU_INT_REG(address)                (*((volatile MS_U16 *)(REG_FRC_CPUINT_BASE + ((address)<<2) )))
#define REG_FRCINT_HKCPUFIRE                    0x0000UL //hst0 to hst1
#define INT_HKCPU_FRCR2_INPUT_SYNC              BIT1
#define INT_HKCPU_FRCR2_OUTPUT_SYNC             BIT2
#endif

// read 2 byte
#define REG_RR(_reg_)                           ({REG_ADDR(_reg_);})

// write low byte
#define REG_WL(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) = (REG_ADDR(_reg_) & 0xFF00) | ((_val_) & 0x00FF); }while(0)

#define REG_WLMSK(_reg_, _val_, mask)    \
                    do{ REG_ADDR(_reg_) = (REG_ADDR(_reg_) & (0xFFFF - mask)) | (((_val_) & (mask)) & 0x00FF); }while(0)

// write high byte
#define REG_WH(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) = (REG_ADDR(_reg_) & 0x00FF) | ((_val_) << 8); }while(0)

#define REG_WHMSK(_reg_, _val_, mask)    \
        do{ REG_ADDR(_reg_) = (REG_ADDR(_reg_) & (0xFFFF - (mask << 8))) | (((_val_) & (mask)) << 8); }while(0)

// write 2 byte
#define REG_W2B(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) =(_val_) ; }while(0)

#define MAIN_WINDOW                             0
#define SUB_WINDOW                              1
#define msDlc_FunctionExit()
#define msDlc_FunctionEnter()

#define DLC_DEFLICK_BLEND_FACTOR                32UL
#define XC_DLC_ALGORITHM_KERNEL                 2

#define XC_DLC_SET_DLC_CURVE_BOTH_SAME          0   // 0:Main and Sub use the same DLC curve
#endif
// MIU Word (Bytes)
#define BYTE_PER_WORD                           (32)  // MIU 128: 16Byte/W, MIU 256: 32Byte/W
#define XC_HDR_DOLBY_PACKET_LENGTH              (128) //128 byte
#define XC_HDR_DOLBY_PACKET_HEADER              (3)
#define XC_HDR_DOLBY_PACKET_TAIL                (4)
#define XC_HDR_DOLBY_METADATA_LENGTH_BIT        (2) // 2 front bytes of dolby packet body is metadata length
#define XC_HDR_HW_SUPPORT_MAX_DOLBY_PACKET      (2)

#define SUPPORT_KERNEL_DS
#define PATCH_FOR_DS_STRUCT_NOT_SYNC
#define SUPPORT_KERNEL_MLOAD
#define SUPPORT_XC_DLC
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_KDRV_XC_HDR_PATH_NONE,
    E_KDRV_XC_HDR_PATH_DOLBY_HDMI,
    E_KDRV_XC_HDR_PATH_DOLBY_OTT_SINGLE,
    E_KDRV_XC_HDR_PATH_DOLBY_OTT_DUAL,
    E_KDRV_XC_HDR_PATH_OPEN_HDMI,
    E_KDRV_XC_HDR_PATH_OPEN_OTT,
    E_KDRV_XC_HDR_PATH_ORIGIN,
    E_KDRV_XC_HDR_PATH_DOLBY_OTT_NONE,  // this is a transition status between dolby OTT and non dolby OTT
    E_KDRV_XC_HDR_PATH_NON_HDR_HDMI,  // This is used when non HDR HDMI input source.
    E_KDRV_XC_HDR_PATH_MAX,
} EN_KDRV_XC_HDR_PATH;

// Communcation format of metadata with VDEC and XC driver.
// (Shared memory format):
// 32
// | Version | Current_Index | DM_Length | DM_Addr | DM_MIU_NO | Composer_Length | Composer_Addr | Composer_MIU_NO | Comp_En |Reserved_Area |
//    1byte        1byte         4byte      4byte     1byte         4byte              4byte            1byte         1byte       11byte
//

typedef struct _STU_CFDAPI_CFDCall_MAIN_CONTROL
{

    //0: off
    //1: on
    MS_U8 u8seamless_en;

    //0: auto
    //1: based on the value of u8seamless_colorspace;

    MS_U8 u8seamless_colorspace_mode;

    //assign by E_CFD_SL_CS
    MS_U8 u8seamless_colorspace;

    //0: auto
    //1: based on the value of u8seamless_colorspace;
    MS_U8 u8seamless_hdrmode_mode;

    //agssign by
    MS_U8 u8seamless_hdrmode;

} STU_CFDAPI_CFDCall_MAIN_CONTROL;


typedef struct
{
    MS_U8 u8CurrentIndex;       /// current metadata index, which is the same as DS index if DS is turned on.
    MS_U32 u32DmLength;         /// display management metadata length
    MS_U32 u32DmAddr;           /// where display management metadata lies
    MS_U8 u8DmMiuNo;            /// dm miu no
    MS_U32 u32ComposerLength;   /// Composer data length
    MS_U32 u32ComposerAddr;     /// where composer data lies
    MS_U8 u8ComposerMiuNo;      /// composer miu no
    MS_BOOL bEnableComposer;    /// composer enable
} ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT;

typedef struct
{
    MS_U8 u8CurrentIndex;
    MS_U8 u8InputFormat;
    MS_U8 u8InputDataFormat;
    MS_BOOL u8Video_Full_Range_Flag;

    MS_BOOL bVUIValid;
    MS_U8 u8Colour_primaries;
    MS_U8 u8Transfer_Characteristics;
    MS_U8 u8Matrix_Coeffs;

    MS_BOOL bSEIValid;
    MS_U16 u16Display_Primaries_x[3];
    MS_U16 u16Display_Primaries_y[3];
    MS_U16 u16White_point_x;
    MS_U16 u16White_point_y;
    MS_U32 u32Master_Panel_Max_Luminance;
    MS_U32 u32Master_Panel_Min_Luminance;

    MS_BOOL bContentLightLevelEnabled;
    MS_U16  u16maxContentLightLevel;
    MS_U16  u16maxPicAverageLightLevel;
} ST_KDRV_XC_HDR_CFD_MEMORY_FORMAT;

typedef struct
{
    MS_U8 u8Version;
    union
    {
        ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT stHDRMemFormatDolby;
        ST_KDRV_XC_HDR_CFD_MEMORY_FORMAT  stHDRMemFormatCFD;
    } HDRMemFormat;

} ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info;

typedef struct
{
    STU_CFDAPI_Curry_DLCIP stCurryDLCParam;
} ST_KDRV_XC_CFD_MS_ALG_INTERFACE_DLC;

typedef struct
{
    MS_U8 u8Controls;
    // E_CFD_MC_MODE
    // 0 : bypass
    // 1 : normal
    // 2 : test
    STU_CFDAPI_Curry_TMOIP    stCurryTMOParam;
} ST_KDRV_XC_CFD_MS_ALG_INTERFACE_TMO;

typedef struct
{
    STU_CFDAPI_Curry_HDRIP  stCurryHDRIPParam;
} ST_KDRV_XC_CFD_MS_ALG_INTERFACE_HDRIP;

typedef struct
{
    MS_U8 u8Controls;
    // E_CFD_MC_MODE
    // 0 : bypass
    // 1 : normal
    // 2 : test
    STU_CFDAPI_Curry_SDRIP  stCurrySDRIPParam;
} ST_KDRV_XC_CFD_MS_ALG_INTERFACE_SDRIP;

typedef struct
{
    //Main sub control mode
    MS_U8 u8HWMainSubMode;
    //0: current control is for SC0 (Main)
    //1: current control is for SC1 (Sub)

    //2-255 is reversed

    ST_KDRV_XC_CFD_MS_ALG_INTERFACE_DLC stDLCInput;
    ST_KDRV_XC_CFD_MS_ALG_INTERFACE_TMO stTMOInput;
    ST_KDRV_XC_CFD_MS_ALG_INTERFACE_HDRIP stHDRIPInput;
    ST_KDRV_XC_CFD_MS_ALG_INTERFACE_SDRIP stSDRIPInput;
} ST_KDRV_XC_CFD_HW_IPS;


/// CFD color format enum.
typedef enum
{
    /// RGB not specified
    E_KDRV_XC_CFD_FORMAT_RGB_NOT_SPECIFIED = 0x00,
    /// RGB BT.601_625
    E_KDRV_XC_CFD_FORMAT_RGB_BT601_625 = 0x01,
    /// RGB BT.601_525
    E_KDRV_XC_CFD_FORMAT_RGB_BT601_525 = 0x02,
    /// RGB BT.709
    E_KDRV_XC_CFD_FORMAT_RGB_BT709 = 0x03,
    /// RGB BT.2020
    E_KDRV_XC_CFD_FORMAT_RGB_BT2020 = 0x04,
    /// sRGB
    E_KDRV_XC_CFD_FORMAT_SRGB = 0x05,
    /// Adobe RGB
    E_KDRV_XC_CFD_FORMAT_ADOBE_RGB = 0x06,
    /// YUV net specified
    E_KDRV_XC_CFD_FORMAT_YUV_NOT_SPECIFIED = 0x07,
    /// YUV BT.601_625
    E_KDRV_XC_CFD_FORMAT_YUV_BT601_625 = 0x08,
    /// YUV BT.601_525
    E_KDRV_XC_CFD_FORMAT_YUV_BT601_525 = 0x09,
    /// YUV BT.709
    E_KDRV_XC_CFD_FORMAT_YUV_BT709 = 0x0a,
    /// YUV BT.2020 NCL
    E_KDRV_XC_CFD_FORMAT_YUV_BT2020NCL = 0x0b,
    /// YUV BT.2020 CL
    E_KDRV_XC_CFD_FORMAT_YUV_BT2020CL = 0x0c,
    /// xvYCC 601
    E_KDRV_XC_CFD_FORMAT_XVYCC_601 = 0x0d,
    /// xvYCC 709
    E_KDRV_XC_CFD_FORMAT_XVYCC_709 = 0x0e,
    /// sYCC 601
    E_KDRV_XC_CFD_FORMAT_SYCC_601 = 0x0f,
    /// Adobe YCC 601
    E_KDRV_XC_CFD_FORMAT_ADOBE_YCC601 = 0x10,
    /// HDR
    E_KDRV_XC_CFD_FORMAT_DOLBY_HDR_TEMP = 0x11,
    /// Reserved
    E_KDRV_XC_CFD_FORMAT_RESERVED_START,
    /// MAX
    E_KDRV_XC_CFD_FORMAT_MAX
} EN_KDRV_XC_CFD_COLOR_FORMAT;

/// CFD Color data format enum.
typedef enum
{
    /// RGB
    E_KDRV_XC_CFD_DATA_FORMAT_RGB = 0,
    /// YUV422
    E_KDRV_XC_CFD_DATA_FORMAT_YUV422 = 1,
    /// YUV444
    E_KDRV_XC_CFD_DATA_FORMAT_YUV444 = 2,
    /// YUV420
    E_KDRV_XC_CFD_DATA_FORMAT_YUV420 = 3,
    /// MAX
    E_KDRV_XC_CFD_DATA_FORMAT_MAX
} EN_KDRV_XC_CFD_COLOR_DATA_FORMAT;

/// HDR type
typedef enum
{
    /// None HDR
    E_KDRV_XC_CFD_HDR_TYPE_NONE = 0x0000,
    /// Dolby HDR
    E_KDRV_XC_CFD_HDR_TYPE_DOLBY = 0x0001,
    /// Open HDR (HDR10)
    E_KDRV_XC_CFD_HDR_TYPE_OPEN = 0x0002,
    /// TCH
    E_KDRV_XC_CFD_HDR_TYPE_TCH = 0x0004,
    /// HLG
    E_KDRV_XC_CFD_HDR_TYPE_HLG = 0x0008,
    /// Max
    E_KDRV_XC_CFD_HDR_TYPE_MAX
} EN_KDRV_XC_CFD_HDR_TYPE;

/// Update type
typedef enum
{
    /// Update all
    E_KDRV_XC_CFD_UPDATE_TYPE_ALL,
    /// Update OSD only
    E_KDRV_XC_CFD_UPDATE_TYPE_OSD_ONLY,
    /// Max
    E_KDRV_XC_CFD_UPDATE_TYPE_MAX
} EN_KDRV_XC_CFD_UPDATE_TYPE;


typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Window (main or sub window)
    MS_U8 u8Win;
    /// Input source
    MS_U8 u8InputSource;
} ST_KDRV_XC_CFD_INIT;

typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Window (main or sub window)
    MS_U8 u8Win;

    /// Full range
    MS_BOOL bIsFullRange;

    /// AVI infoframe
    /// Pixel Format
    MS_U8 u8PixelFormat;
    /// Color imetry
    MS_U8 u8Colorimetry;
    /// Extended Color imetry
    MS_U8 u8ExtendedColorimetry;
    /// Rgb Quantization Range
    MS_U8 u8RgbQuantizationRange;
    /// Ycc Quantization Range
    MS_U8 u8YccQuantizationRange;

    /// HDR infoframe
    /// HDR infoframe valid
    MS_BOOL bHDRInfoFrameValid;
    /// EOTF (/// 0:SDR gamma, 1:HDR gamma, 2:SMPTE ST2084, 3:Future EOTF, 4-7:Reserved)
    MS_U8 u8EOTF;
    /// Static metadata ID (0: Static Metadata Type 1, 1-7:Reserved for future use)
    MS_U8 u8SMDID;
    /// Display primaries x
    MS_U16 u16Display_Primaries_x[3];
    /// Display primaries y
    MS_U16 u16Display_Primaries_y[3];
    /// White point x
    MS_U16 u16White_point_x;
    /// White point y
    MS_U16 u16White_point_y;
    /// Panel max luminance
    MS_U16 u16MasterPanelMaxLuminance;
    /// Panel min luminance
    MS_U16 u16MasterPanelMinLuminance;
    /// Max content light level
    MS_U16 u16MaxContentLightLevel;
    /// Max frame average light level
    MS_U16 u16MaxFrameAvgLightLevel;

} ST_KDRV_XC_CFD_HDMI;

typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Window (main or sub window)
    MS_U8 u8Win;

    /// Color format, reference EN_KDRV_XC_CFD_COLOR_FORMAT.
    MS_U8 u8ColorFormat;
    /// Color data format, reference EN_KDRV_XC_CFD_COLOR_DATA_FORMAT.
    MS_U8 u8ColorDataFormat;
    /// Full range
    MS_BOOL bIsFullRange;
    /// Color primaries
    MS_U8 u8ColorPrimaries;
    /// Transfer characteristics
    MS_U8 u8TransferCharacteristics;
    /// Matrix coefficients
    MS_U8 u8MatrixCoefficients;

} ST_KDRV_XC_CFD_ANALOG;

typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Color format, reference EN_KDRV_XC_CFD_COLOR_FORMAT.
    MS_U8 u8ColorFormat;
    /// Color data format, reference EN_KDRV_XC_CFD_COLOR_DATA_FORMAT.
    MS_U8 u8ColorDataFormat;
    /// Full range
    MS_BOOL bIsFullRange;

    /// Display primaries x, data *0.00002 0xC350 = 1
    MS_U16 u16Display_Primaries_x[3];
    /// Display primaries y, data *0.00002 0xC350 = 1
    MS_U16 u16Display_Primaries_y[3];
    /// White point x, data *0.00002 0xC350 = 1
    MS_U16 u16White_point_x;
    /// White point y, data *0.00002 0xC350 = 1
    MS_U16 u16White_point_y;
    /// Max luminance, data * 1 nits
    MS_U16 u16MaxLuminance;
    /// Med luminance, data * 1 nits
    MS_U16 u16MedLuminance;
    /// Min luminance, data * 0.0001 nits
    MS_U16 u16MinLuminance;
    /// Linear RGB
    MS_BOOL bLinearRgb;
    /// Customer color primaries
    MS_BOOL bCustomerColorPrimaries;
    /// Source white x
    MS_U16 u16SourceWx;
    /// Source white y
    MS_U16 u16SourceWy;
} ST_KDRV_XC_CFD_PANEL;

typedef struct
{
    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_HDMI_EDID_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_HDMI_EDID_PARSER)

    MS_U8 u8HDMISinkHDRDataBlockValid;
    //assign by E_CFD_VALIDORNOT
    //0 :Not valid
    //1 :valid

    MS_U8 u8HDMISinkEOTF;
    //byte 3 in HDR static Metadata Data block

    MS_U8 u8HDMISinkSM;
    //byte 4 in HDR static Metadata Data block

    MS_U8 u8HDMISinkDesiredContentMaxLuminance;           //need a LUT to transfer
    MS_U8 u8HDMISinkDesiredContentMaxFrameAvgLuminance; //need a LUT to transfer
    MS_U8 u8HDMISinkDesiredContentMinLuminance;           //need a LUT to transfer
    //byte 5 ~ 7 in HDR static Metadata Data block

    MS_U8 u8HDMISinkHDRDataBlockLength;
    //byte 1[4:0] in HDR static Metadata Data block

    //order R->G->B
    //MS_U16 u16display_primaries_x[3];                       //data *1/1024 0x03FF = 0.999
    //MS_U16 u16display_primaries_y[3];                       //data *1/1024 0x03FF = 0.999
    //MS_U16 u16white_point_x;                                //data *1/1024 0x03FF = 0.999
    //MS_U16 u16white_point_y;                                //data *1/1024 0x03FF = 0.999
    /// Display primaries x, data *0.00002 0xC350 = 1
    MS_U16 u16Display_Primaries_x[3];
    /// Display primaries y, data *0.00002 0xC350 = 1
    MS_U16 u16Display_Primaries_y[3];
    /// White point x, data *0.00002 0xC350 = 1
    MS_U16 u16White_point_x;
    /// White point y, data *0.00002 0xC350 = 1
    MS_U16 u16White_point_y;
    //address 0x19h to 22h in base EDID

    MS_U8 u8HDMISinkEDIDBaseBlockVersion;                //for debug
    //address 0x12h in EDID base block

    MS_U8 u8HDMISinkEDIDBaseBlockReversion;              //for debug
    //address 0x13h in EDID base block

    MS_U8 u8HDMISinkEDIDCEABlockReversion;               //for debug
    //address 0x01h in EDID CEA block

    //table 59 Video Capability Data Block (VCDB)
    //0:VCDB is not avaliable
    //1:VCDB is avaliable
    MS_U8 u8HDMISinkVCDBValid;

    MS_U8 u8HDMISinkSupportYUVFormat;
    //bit 0:Support_YUV444
    //bit 1:Support_YUV422
    //bit 2:Support_YUV420

    //QY in Byte#3 in table 59 Video Capability Data Block (VCDB)
    //bit 3:RGB_quantization_range

    //QS in Byte#3 in table 59 Video Capability Data Block (VCDB)
    //bit 4:Y_quantization_range 0:no data(due to CE or IT video) ; 1:selectable


    MS_U8 u8HDMISinkExtendedColorspace;
    //byte 3 of Colorimetry Data Block
    //bit 0:xvYCC601
    //bit 1:xvYCC709
    //bit 2:sYCC601
    //bit 3:Adobeycc601
    //bit 4:Adobergb
    //bit 5:BT2020 cl
    //bit 6:BT2020 ncl
    //bit 7:BT2020 RGB

    MS_U8 u8HDMISinkEDIDValid;

} ST_KDRV_XC_CFD_EDID;

typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Window (main or sub window)
    MS_U8 u8Win;
    /// HDR type, reference EN_KDRV_XC_CFD_HDR_TYPE
    MS_U8 u8HdrType;
} ST_KDRV_XC_CFD_HDR;

typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Hue
    MS_U16 u16Hue;
    /// Saturation
    MS_U16 u16Saturation;
    /// Contrast
    MS_U16 u16Contrast;

    //0:off
    //1:on
    //default on , not in the document
    MS_U8  u8OSDUIEn;

    //Mode 0: update matrix by OSD and color format driver
    //Mode 1: only update matrix by OSD controls
    //for mode1 : the configures of matrix keep the same as the values by calling CFD last time
    MS_U8  u8OSDUIMode;
    ///
    MS_BOOL bHueValid;
    MS_BOOL bSaturationValid;
    MS_BOOL bContrastValid;

    MS_BOOL bBacklightValid;
    MS_U32 u32MinBacklight;
    MS_U32 u32MaxBacklight;
    MS_U32 u32Backlight;

    MS_U8 u8Win;
    MS_BOOL bColorRangeValid;
    MS_U8 u8ColorRange; // 0: Auto, 1: Full Range (0~255), 2: Limit Range (16~235)

    MS_BOOL bUltraBlackLevelValid;
    MS_U8 u8UltraBlackLevel;
    MS_BOOL bUltraWhiteLevelValid;
    MS_U8 u8UltraWhiteLevel;

    MS_BOOL bSkipPictureSettingValid;
    MS_BOOL bSkipPictureSetting;

    MS_BOOL bColorCorrectionValid;
    MS_S16 s16ColorCorrectionMatrix[32];

    MS_BOOL bYVUtoRGBMatrixValid;
    MS_S16 s16YVUtoRGBMatrix[32];

    MS_U16 u16R;
    MS_U16 u16G;
    MS_U16 u16B;

    MS_BOOL bRValid;
    MS_BOOL bGValid;
    MS_BOOL bBValid;

    //0:auto depends on STB rule
    //1:always do HDR2SDR for HDR input
    //2:always not do HDR2SDR for HDR input
    MS_U8  u8HDR_UI_H2SMode;
} ST_KDRV_XC_CFD_OSD;

typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Window (main or sub window)
    MS_U8 u8Win;
    /// Linear RGB enable
    MS_BOOL bEnable;
} ST_KDRV_XC_CFD_LINEAR_RGB;

typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Window (main or sub window)
    MS_U8 u8Win;
    /// Input source
    MS_U8 u8InputSource;
    /// Update type, reference EN_KDRV_XC_CFD_UPDATE_TYPE
    MS_U8 u8UpdateType;
    /// RGB bypass
    MS_BOOL bIsRgbBypass;
    /// HD mode
    MS_BOOL bIsHdMode;
} ST_KDRV_XC_CFD_FIRE;

typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// customer dlc curve
    MS_BOOL bUseCustomerDlcCurve;
} ST_KDRV_XC_CFD_DLC;

typedef struct
{
    /// Structure version
    MS_U32 u32Version;
    /// Structure length
    MS_U16 u16Length;

    /// Window (main or sub window)
    MS_U8 u8Win;
    /// HDR type (0: SDR, 1: Dolby HDR, 2: Open HDR)
    MS_U8 u8VideoHdrType;
    /// HDR running
    MS_BOOL bIsHdrRunning;
    /// Is full range
    MS_BOOL bIsFullRange;
    /// Ultra black & white active
    MS_BOOL bUltraBlackWhiteActive;
} ST_KDRV_XC_CFD_STATUS;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
#define MHal_XC_SetHDR_DMARequestOFF(args...)
#define MHal_XC_SetDMPQBypass(args...)
#define MHal_XC_SetDMAPath(args...)
#define MHal_XC_Init(args...)
INTERFACE void MHal_XC_WriteByte(DWORD u32Reg, BYTE u8Val);
INTERFACE void MHal_XC_WriteByteMask(DWORD u32Reg, BYTE u8Val, WORD u16Mask);
INTERFACE BYTE MHal_XC_ReadByte(DWORD u32Reg);
INTERFACE void MHal_XC_W2BYTE(DWORD u32Reg, WORD u16Val );
INTERFACE WORD MHal_XC_R2BYTE(DWORD u32Reg );
INTERFACE void MHal_XC_W2BYTEMSK(DWORD u32Reg, WORD u16Val, WORD u16Mask );
INTERFACE WORD MHal_XC_R2BYTEMSK(DWORD u32Reg, WORD u16Mask );
INTERFACE BOOL MHal_XC_IsBlackVideoEnable( MS_BOOL bWindow );
INTERFACE void MHal_XC_FRCR2SoftwareToggle( void );
INTERFACE BOOL MHal_XC_GetHdmiMetadata(MS_U8 *pu8Metadata, MS_U16 *pu16MetadataLength);
INTERFACE BOOL MHal_XC_GetMiuOffset(MS_U32 u32MiuNo, MS_U32 *pu32Offset);
INTERFACE BOOL MHal_XC_SetInputSourceType(EN_KDRV_XC_INPUT_SOURCE_TYPE enInputSourceType);
INTERFACE BOOL MHal_XC_SetHDRType(EN_KDRV_XC_HDR_TYPE enHDRType);
INTERFACE BOOL MHal_XC_UpdatePath(EN_KDRV_XC_HDR_PATH enPath);
INTERFACE BOOL MHal_XC_EnableEL(MS_BOOL bEnable);
INTERFACE BOOL MHal_XC_EnableAutoSeamless(MS_BOOL bEnable);
INTERFACE BOOL MHal_XC_DisableHDMI422To444(void);
INTERFACE BOOL MHal_XC_SetHDRWindow(MS_U16 u16Width, MS_U16 u16Height);
INTERFACE BOOL MHal_XC_Set3DLutInfo(MS_U8* pu8Data, MS_U32 u32Size);
INTERFACE BOOL MHal_XC_SetDolbyMetaData(MS_U8* pu8Data, MS_U32 u32Size);
INTERFACE BOOL MHal_XC_SetDolbyCompData(MS_U8* pu8Data, MS_U32 u32Size);
INTERFACE BOOL MHal_XC_EnableHDR(MS_BOOL bEnableHDR);
INTERFACE BOOL MHal_XC_MuteHDR(void);
INTERFACE BOOL MHal_XC_SupportDolbyHDR(void);
INTERFACE BOOL MHal_XC_SupportTCH(void);
INTERFACE BOOL MHal_XC_ConfigAutoDownload(EN_KDRV_XC_AUTODOWNLOAD_CLIENT enClient, MS_BOOL bEnable,
        EN_KDRV_XC_AUTODOWNLOAD_MODE enMode, MS_PHY phyBaseAddr, MS_U32 u32Size, MS_U32 u32MiuNo);
INTERFACE BOOL MHal_XC_WriteAutoDownload(EN_KDRV_XC_AUTODOWNLOAD_CLIENT enClient, MS_U8* pu8Data, MS_U32 u32Size, void* pParam);
INTERFACE BOOL MHal_XC_FireAutoDownload(EN_KDRV_XC_AUTODOWNLOAD_CLIENT enClient);
INTERFACE BOOL MHal_XC_ConfigHDRAutoDownloadStoredInfo(MS_PHY phyLutBaseAddr, MS_U8 *pu8VirtLutBaseAddr, MS_U32 u32Size);
INTERFACE BOOL MHal_XC_StoreHDRAutoDownload(MS_U8* pu8Data, MS_U32 u32Size, void* pParam);
INTERFACE BOOL MHal_XC_WriteStoredHDRAutoDownload(MS_U8 *pu8LutData, MS_U32 u32Size, MS_U8 u8Index,
    MS_PHY *pphyFireAdlAddr, MS_U32 *pu32Depth);
INTERFACE MS_U32 MHal_XC_GetHDRAutoDownloadStoredSize(void);
INTERFACE BOOL MHal_XC_GetAutoDownloadCaps(EN_KDRV_XC_AUTODOWNLOAD_CLIENT enClient, MS_BOOL *pbSupported);
INTERFACE BOOL MHal_XC_SetColorFormat(MS_BOOL bHDMI422);
INTERFACE MS_U16 MHal_XC_CFD_GetInitParam(ST_KDRV_XC_CFD_INIT *pstCfdInit);
INTERFACE MS_U16 MHal_XC_CFD_GetHdmiParam(ST_KDRV_XC_CFD_HDMI *pstCfdHdmi);
INTERFACE MS_S32 MHal_XC_CFD_DepositeMMParam(ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info *pstFormatInfo, MS_U8 u8Win);
INTERFACE MS_S32 MHal_XC_CFD_WithdrawMMParam(ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info *pstFormatInfo, MS_U8 u8Win);
INTERFACE MS_U16 MHal_XC_CFD_SetMainCtrl_MMinput(STU_CFDAPI_MM_PARSER *pstMMParam, ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info *pstFormatInfo);
INTERFACE BOOL MHal_XC_EnableHDRCLK(MS_BOOL bEnable, MS_BOOL bImmediate);
INTERFACE E_CFD_MC_FORMAT MHal_XC_HDMI_Color_Data_Format(MS_U8 u8PixelFormat);
INTERFACE BOOL MHal_XC_CFDControl(ST_KDRV_XC_CFD_CONTROL_INFO *pstKdrvCFDCtrlInfo);
INTERFACE BOOL MHal_XC_InitHDR(void);
INTERFACE BOOL MHal_XC_ExitHDR(void);
INTERFACE MS_U32 MHal_XC_GetRegsetCnt(void);
INTERFACE void MHal_XC_PrepareDolbyInfo(void);
INTERFACE void MHal_XC_Suspend(void);
INTERFACE void MHal_XC_Resume(void);
INTERFACE void Color_Format_Driver(void);
INTERFACE MS_BOOL MHal_XC_CheckMuteStatusByRegister(MS_U8 u8Window);
INTERFACE BOOL MHal_XC_IsBlackVideoEnable( MS_BOOL bWindow );
INTERFACE void MHal_XC_FRCR2SoftwareToggle( void );
INTERFACE BOOL MHal_XC_IsCRCPass(MS_U8 u8PkgIdx);
INTERFACE MS_BOOL MHal_XC_VSIF_Dolby_Status(void);
INTERFACE BOOL MHAL_XC_IsCFDInitFinished();
INTERFACE MS_U8 MHal_XC_GetVersion( void );
INTERFACE MS_U8 MHal_XC_ResetADL( void );
#endif // _MHAL_TEMP_H_
