///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2008 - 2009 MStar Semiconductor, Inc.
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

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   mhal_dlc.h
/// @brief  MStar DLC Driver DDI HAL Level
/// @author MStar Semiconductor Inc.
/// @attention
/// <b>(OBSOLETED) <em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_DLC_H
#define _HAL_DLC_H

#ifdef _HAL_DLC_C
#define INTERFACE
#else
#define INTERFACE                               extern
#endif

//=============================================================================
// Includs
//=============================================================================

//=============================================================================
// Defines & Macros
//=============================================================================
#define printf                                  //printk
#define DLC_FUNCTION                            1


//=============================================================================
// Type and Structure Declaration
//=============================================================================

#ifdef CONFIG_64BIT
            /// data type unsigned int, data length 8 byte
#define MS_U64 unsigned long
            /// data type signed int, data length 8 byte
#define MS_S64 signed long
#else
            /// data type unsigned int, data length 8 byte
#define MS_U64 unsigned long long
            /// data type signed int, data length 8 byte
#define MS_S64 signed long long
#endif


    /// data type unsigned char, data length 1 byte
#define MS_U8 unsigned char                                             // 1 byte
            /// data type unsigned short, data length 2 byte
#define MS_U16 unsigned short                                           // 2 bytes
            /// data type unsigned int, data length 4 byte
#define MS_U32 unsigned int                                             // 4 bytes
                                  // 8 bytes
            /// data type signed char, data length 1 byte
#define MS_S8 signed char                                               // 1 byte
            /// data type signed short, data length 2 byte
#define MS_S16 signed short                                             // 2 bytes
            /// data type signed int, data length 4 byte
#define MS_S32 signed int                                               // 4 bytes
                                     // 8 bytes
            /// data type float, data length 4 byte
#define MS_FLOAT float                                                  // 4 bytes
            /// data type pointer content
#define MS_VIRT size_t                                                  // 8 bytes
            /// data type hardware physical address
#define MS_PHYADDR size_t                                               // 8 bytes
            /// data type 64bit physical address
#define MS_PHY MS_U64                                                   // 8 bytes
            /// data type size_t
#define MS_SIZE size_t                                                  // 8 bytes

/// print type  MPRI_PHY
#define                             MPRI_PHY                            "%x"
/// print type  MPRI_VIRT
#define                             MPRI_VIRT                           "%tx"


/// data type null pointer
#ifdef NULL
#undef NULL
#endif
#define NULL                        0

#ifdef CONFIG_MP_PURE_SN_32BIT
#define MS_PHY64 unsigned int                                         // 32bit physical address
#else
#define MS_PHY64 unsigned long long                                   // 64bit physical address
#endif




//-------------------------------------------------------------------------------------------------
//  Software Data Type
//-------------------------------------------------------------------------------------------------

/// definition for MS_BOOL
#define MS_BOOL unsigned char
/// definition for VOID
#define VOID void
/// definition for FILEID
#define FILEID MS_S32


#if !defined(TRUE) && !defined(FALSE)
/// definition for TRUE
#define TRUE                        1
/// definition for FALSE
#define FALSE                       0
#endif

#ifndef true
/// definition for true
#define true                        1
/// definition for false
#define false                       0


#if !defined(TRUE) && !defined(FALSE)
/// definition for TRUE
#define TRUE                        1
/// definition for FALSE
#define FALSE                       0
#endif
#endif



#if !defined(ENABLE) && !defined(DISABLE)
/// definition for ENABLE
#define ENABLE                      1
/// definition for DISABLE
#define DISABLE                     0
#endif




#if DLC_FUNCTION
#define BYTE                                    MS_U8
#define WORD                                    MS_U16
#define DWORD                                   MS_U32

//#define MS_DLC_DEBUG


#ifdef MS_DLC_DEBUG
#ifndef DLC_DEBUG
#define DLC_DEBUG(  _fmt, _args...)    printk(_fmt, ##_args);
#endif
#else
#ifndef DLC_DEBUG
#define DLC_DEBUG(  _fmt, _args...)
#endif
#endif

#ifdef CONFIG_64BIT
#define abs(x) ({                       \
        long ret;                   \
        if (sizeof(x) == sizeof(long)) {        \
            long __x = (x);             \
            ret = (__x < 0) ? -__x : __x;       \
        } else {                    \
            int __x = (x);              \
            ret = (__x < 0) ? -__x : __x;       \
        }                       \
        ret;                        \
    })
#else
#define abs(x) ({                       \
        long long ret;                  \
        if (sizeof(x) == sizeof(long long)) {       \
            long long __x = (x);                \
            ret = (__x < 0) ? -__x : __x;       \
        } else {                    \
            int __x = (x);              \
            ret = (__x < 0) ? -__x : __x;       \
        }                       \
        ret;                        \
    })
#endif
#ifndef min
#define min(x, y) ({                \
            typeof(x) _min1 = (x);          \
            typeof(y) _min2 = (y);          \
            (void) (&_min1 == &_min2);      \
            _min1 < _min2 ? _min1 : _min2; })
#endif
#define max(x, y) ({                \
            typeof(x) _max1 = (x);          \
            typeof(y) _max2 = (y);          \
            (void) (&_max1 == &_max2);      \
            _max1 > _max2 ? _max1 : _max2; })

#define REG_ADDR_DLC_HANDSHAKE                  L_BK_DLC(0x04)
#define REG_ADDR_DLC_HISTOGRAM_MODE             H_BK_DLC(0x04)
#define REG_ADDR_HISTOGRAM_TOTAL_SUM_L          L_BK_DLC(0x06)
#define REG_ADDR_HISTOGRAM_TOTAL_SUM_H          H_BK_DLC(0x06)
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_L        L_BK_DLC(0x07)
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_H        H_BK_DLC(0x07)
#define REG_ADDR_HISTOGRAM_RANGE_ENABLE         L_BK_DLC(0x08)
#define REG_ADDR_BLE_UPPER_BOND                 L_BK_DLC(0x09)
#define REG_ADDR_BLE_LOWER_BOND                 H_BK_DLC(0x09)
#define REG_ADDR_WLE_UPPER_BOND                 L_BK_DLC(0x0A)
#define REG_ADDR_WLE_LOWER_BOND                 H_BK_DLC(0x0A)
#define REG_ADDR_MAIN_MAX_VALUE                 L_BK_DLC(0x0B)
#define REG_ADDR_MAIN_MIN_VALUE                 H_BK_DLC(0x0B)
#define REG_ADDR_SUB_MAX_VALUE                  L_BK_DLC(0x0C)
#define REG_ADDR_SUB_MIN_VALUE                  H_BK_DLC(0x0C)
#define REG_ADDR_MAIN_BLACK_SLOP_LSB_L          L_BK_DLC(0x0D)
#define REG_ADDR_MAIN_BLACK_SLOP_L              L_BK_DLC(0x10)
#define REG_ADDR_MAIN_BLACK_SLOP_H              H_BK_DLC(0x10)
#define REG_ADDR_DLC_MAIN_Y_GAIN                L_BK_DLC(0x14)
#define REG_ADDR_DLC_MAIN_C_GAIN                H_BK_DLC(0x14)
#define REG_ADDR_DLC_SUB_Y_GAIN                 L_BK_DLC(0x15)
#define REG_ADDR_DLC_SUB_C_GAIN                 H_BK_DLC(0x15)
#define REG_ADDR_HISTOGRAM_DATA_8               L_BK_DLC(0x28)
#define REG_ADDR_DLC_DATA_START_MAIN            L_BK_DLC(0x30)
#define REG_ADDR_DLC_DATA_START_SUB             L_BK_DLC(0x38)
#define REG_ADDR_HISTOGRAM_DATA_32              L_BK_DLC(0x40)
#define REG_ADDR_HISTOGRAM_SUB_TOTAL_COUNT_L    L_BK_DLC(0x72)
#define REG_ADDR_HISTOGRAM_SUB_TOTAL_COUNT_H    H_BK_DLC(0x72)
#define REG_ADDR_HISTOGRAM_SUB_TOTAL_SUM_L      L_BK_DLC(0x73)
#define REG_ADDR_HISTOGRAM_SUB_TOTAL_SUM_H      H_BK_DLC(0x73)
#define REG_ADDR_DLC_DATA_EXTEND_N0_MAIN        L_BK_DLC(0x76)
#define REG_ADDR_DLC_DATA_EXTEND_16_MAIN        L_BK_DLC(0x77)
#define REG_ADDR_DLC_DATA_LSB_START_MAIN        L_BK_DLC(0x78)
#define REG_ADDR_DLC_DATA_LSB_START_SUB         L_BK_DLC(0x7B)
#define REG_ADDR_DLC_DATA_EXTEND_N0_SUB         L_BK_DLC(0x7E)
#define REG_ADDR_DLC_DATA_EXTEND_16_SUB         L_BK_DLC(0x7F)

#define REG_ADDR_VOP_SCREEN_CONTROL             L_BK_VOP(0x19)

#define REG_ADDR_OP_SW_SUB_ENABLE               L_BK_OP(0x10)

#define REG_ADDR_IP_HISTOGRAM_DATA_32              L_BK_IP_DLC(0x10)
#define REG_ADDR_IP_HISTOGRAM_TOTAL_SUM_L_0C          L_BK_IP_DLC(0x0C)
#define REG_ADDR_IP_HISTOGRAM_TOTAL_SUM_H_0C          H_BK_IP_DLC(0x0C)
#define REG_ADDR_IP_HISTOGRAM_TOTAL_SUM_L_0D          L_BK_IP_DLC(0x0D)
#define REG_ADDR_IP_HISTOGRAM_TOTAL_SUM_H_0D          H_BK_IP_DLC(0x0D)
#define REG_ADDR_IP_HISTOGRAM_TOTAL_COUNT_L_0A        L_BK_IP_DLC(0x0A)
#define REG_ADDR_IP_HISTOGRAM_TOTAL_COUNT_H_0A        H_BK_IP_DLC(0x0A)
#define REG_ADDR_IP_HISTOGRAM_TOTAL_COUNT_L_0B        L_BK_IP_DLC(0x0B)
#define REG_ADDR_IP_HISTOGRAM_TOTAL_COUNT_H_0B        H_BK_IP_DLC(0x0B)
#define REG_ADDR_IP_MAIN_MAX_VALUE                 L_BK_IP_DLC(0x08)
#define REG_ADDR_IP_MAIN_MIN_VALUE                 L_BK_IP_DLC(0x09)
#define REG_ADDR_IP_HISTOGRAM_ENABLE                 L_BK_IP_DLC(0x01)
#define REG_ADDR_IP_HISTOGRAM_H_END_L          L_BK_IP_DLC(0x05)
#define REG_ADDR_IP_HISTOGRAM_H_END_H          H_BK_IP_DLC(0x05)
#define REG_ADDR_IP_HISTOGRAM_V_END_L          L_BK_IP_DLC(0x07)
#define REG_ADDR_IP_HISTOGRAM_V_END_H          H_BK_IP_DLC(0x07)

#define PRJ_MCNR

#define REG_LPLL_11_L                           (0x103100 | 0x11<< 1)
#define REG_LPLL_13_L                           (0x103100 | 0x13<< 1)
#ifndef REG_SC_BK00_11_L
#define REG_SC_BK00_11_L                        (0x130000 | (0x11<< 1))
#endif
#define REG_SC_BK00_13_L                        (0x130000 | 0x13<< 1)
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

#define REG_SC_BK10_26_L                        (0x131000 | 0x26<< 1)
#define REG_SC_BK10_27_L                        (0x131000 | 0x27<< 1)
#define REG_SC_BK10_28_L                        (0x131000 | 0x28<< 1)
#define REG_SC_BK10_29_L                        (0x131000 | 0x29<< 1)
#define REG_SC_BK10_2A_L                        (0x131000 | 0x2A<< 1)
#define REG_SC_BK10_2B_L                        (0x131000 | 0x2B<< 1)
#define REG_SC_BK10_2C_L                        (0x131000 | 0x2C<< 1)
#define REG_SC_BK10_2D_L                        (0x131000 | 0x2D<< 1)
#define REG_SC_BK10_2E_L                        (0x131000 | 0x2E<< 1)

#define REG_SC_BK12_03_L                        (0x131200 | 0x03<< 1)
#define REG_SC_BK12_0F_L                        (0x131200 | 0x0F<< 1)
#define REG_SC_BK12_19_L                        (0x131200 | 0x19<< 1)
#define REG_SC_BK12_30_L                        (0x131200 | 0x30<< 1)
#define REG_SC_BK12_33_L                        (0x131200 | 0x33<< 1)
#define REG_SC_BK12_3A_L                        (0x131200 | 0x3A<< 1)
#define REG_SC_BK22_2A_L                        (0x132200 | 0x2A<< 1)
#define REG_SC_BK22_7A_L                        (0x132200 | 0x7A<< 1)
#define REG_SC_BK22_7C_L                        (0x132200 | 0x7C<< 1)

#define REG_SC_BK1A_38_L                        (0x131A00 | 0x38<< 1)
#define REG_SC_BK1A_39_L                        (0x131A00 | 0x39<< 1)
#define REG_SC_BK1A_3A_L                        (0x131A00 | 0x3A<< 1)
#define REG_SC_BK1A_3B_L                        (0x131A00 | 0x3B<< 1)
#define REG_SC_BK1A_3C_L                        (0x131A00 | 0x3C<< 1)
#define REG_SC_BK1A_3D_L                        (0x131A00 | 0x3D<< 1)
#define REG_SC_BK1A_3E_L                        (0x131A00 | 0x3E<< 1)
#define REG_SC_BK1A_3F_L                        (0x131A00 | 0x3F<< 1)

#define REG_SC_BK25_01_L                        (0x132500 | 0x01<< 1)
#define REG_SC_BK25_02_L                        (0x132500 | 0x02<< 1)
#define REG_SC_BK25_03_L                        (0x132500 | 0x03<< 1)
#define REG_SC_BK25_04_L                        (0x132500 | 0x04<< 1)
#define REG_SC_BK25_05_L                        (0x132500 | 0x05<< 1)
#define REG_SC_BK25_06_L                        (0x132500 | 0x06<< 1)
#define REG_SC_BK25_07_L                        (0x132500 | 0x07<< 1)
#define REG_SC_BK25_08_L                        (0x132500 | 0x08<< 1)
#define REG_SC_BK25_09_L                        (0x132500 | 0x09<< 1)
#define REG_SC_BK25_0A_L                        (0x132500 | 0x0A<< 1)
#define REG_SC_BK25_78_L                        (0x132500 | 0x78<< 1)
#define REG_SC_BK25_79_L                        (0x132500 | 0x79<< 1)
#define REG_SC_BK25_7A_L                        (0x132500 | 0x7A<< 1)
#define REG_SC_BK25_7B_L                        (0x132500 | 0x7B<< 1)
#define REG_SC_BK25_7D_L                        (0x132500 | 0x7D<< 1)


#define REG_SC_BK2A_02_L                        (0x132A00 | 0x02<< 1)
#define REG_SC_BK2A_29_L                        (0x132A00 | 0x29<< 1)
#define REG_SC_BK2A_2A_L                        (0x132A00 | 0x2A<< 1)
#define REG_SC_BK2A_2F_L                        (0x132A00 | 0x2F<< 1)
#define REG_SC_BK2A_7C_L                        (0x132A00 | 0x7C<< 1)
#define REG_SC_BK2A_7D_L                        (0x132A00 | 0x7D<< 1)
#define REG_SC_BK2A_7E_L                        (0x132A00 | 0x7E<< 1)

#define REG_SC_BK2D_60_L                        (0x132D00 | 0x60<< 1)
#define REG_SC_BK2D_61_L                        (0x132D00 | 0x61<< 1)
#define REG_SC_BK2D_62_L                        (0x132D00 | 0x62<< 1)
#define REG_SC_BK2D_63_L                        (0x132D00 | 0x63<< 1)
#define REG_SC_BK2D_64_L                        (0x132D00 | 0x64<< 1)
#define REG_SC_BK2D_65_L                        (0x132D00 | 0x65<< 1)
#define REG_SC_BK2D_66_L                        (0x132D00 | 0x66<< 1)
#define REG_SC_BK2D_67_L                        (0x132D00 | 0x67<< 1)
#define REG_SC_BK2D_68_L                        (0x132D00 | 0x68<< 1)
#define REG_SC_BK2D_69_L                        (0x132D00 | 0x69<< 1)
#define REG_SC_BK2D_6A_L                        (0x132D00 | 0x6A<< 1)
#define REG_SC_BK2D_6B_L                        (0x132D00 | 0x6B<< 1)
#define REG_SC_BK2D_6C_L                        (0x132D00 | 0x6C<< 1)
#define REG_SC_BK2D_6D_L                        (0x132D00 | 0x6D<< 1)
#define REG_SC_BK2D_6E_L                        (0x132D00 | 0x6E<< 1)
#define REG_SC_BK2D_6F_L                        (0x132D00 | 0x6F<< 1)
#define REG_SC_BK2D_70_L                        (0x132D00 | 0x70<< 1)
#define REG_SC_BK2D_71_L                        (0x132D00 | 0x71<< 1)

#define REG_SC_BK30_01_L                        (0x133000 | (0x01<< 1))
#define REG_SC_BK30_02_L                        (0x133000 | (0x02<< 1))
#define REG_SC_BK30_02_H                        (0x133000 | ((0x02<< 1)+1))
#define REG_SC_BK30_03_L                        (0x133000 | (0x03<< 1))
#define REG_SC_BK30_04_L                        (0x133000 | (0x04<< 1))
#define REG_SC_BK30_05_L                        (0x133000 | 0x05<< 1)

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

#define REG_SC_BK30_10_L                        (0x133000 | (0x10<< 1))
#define REG_SC_BK30_10_H                        (0x133000 | ((0x10<< 1)+1))

#define REG_SC_BK62_06_L                        (0x136200 | 0x06<< 1)
#define REG_SC_BK62_0E_L                        (0x136200 | 0x0E<< 1)
#define REG_SC_BK62_08_L                        (0x136200 | 0x08<< 1)
#define REG_SC_BK63_61_L                        (0x136300 | 0x61<< 1)

#define REG_SC_BK4E_21_L                        (0x134E00 | 0x21<< 1)
#define REG_SC_BK4E_22_L                        (0x134E00 | 0x22<< 1)
#define REG_SC_BK4E_23_L                        (0x134E00 | 0x23<< 1)
#define REG_SC_BK4E_24_L                        (0x134E00 | 0x24<< 1)
#define REG_SC_BK4E_25_L                        (0x134E00 | 0x25<< 1)
#define REG_SC_BK4E_26_L                        (0x134E00 | 0x26<< 1)
#define REG_SC_BK4E_27_L                        (0x134E00 | 0x27<< 1)
#define REG_SC_BK4E_28_L                        (0x134E00 | 0x28<< 1)


#define REG_SC_BK2E_22_L                        (0x132E00 | 0x22<< 1)
#define REG_SC_BK2E_23_L                        (0x132E00 | 0x23<< 1)
#define REG_SC_BK2E_24_L                        (0x132E00 | 0x24<< 1)
#define REG_SC_BK2E_25_L                        (0x132E00 | 0x25<< 1)
#define REG_SC_BK2E_26_L                        (0x132E00 | 0x26<< 1)
#define REG_SC_BK2E_27_L                        (0x132E00 | 0x27<< 1)
#define REG_SC_BK2E_28_L                        (0x132E00 | 0x28<< 1)

#define REG_SC_BK02_70_L                        (0x130200 | (0x70<< 1))
#define REG_SC_BK02_71_L                        (0x130200 | (0x71<< 1))
#define REG_SC_BK02_72_L                        (0x130200 | (0x72<< 1))
#define REG_SC_BK02_73_L                        (0x130200 | (0x73<< 1))
#define REG_SC_BK02_74_L                        (0x130200 | (0x74<< 1))
#define REG_SC_BK02_75_L                        (0x130200 | (0x75<< 1))
#define REG_SC_BK02_76_L                        (0x130200 | (0x76<< 1))
#define REG_SC_BK02_77_L                        (0x130200 | (0x77<< 1))
#define REG_SC_BK02_78_L                        (0x130200 | (0x78<< 1))
#define REG_SC_BK02_79_L                        (0x130200 | (0x79<< 1))
#define REG_SC_BK02_7F_L                        (0x130200 | (0x7F<< 1))

#define REG_SC_BK22_0D_L                        (0x132200 | (0x0D<< 1))
#define REG_SC_BK22_0E_L                        (0x132200 | (0x0E<< 1))
#define REG_SC_BK18_06_L                        (0x131800 | (0x06<< 1))
#define REG_SC_BK18_06_H                        (0x131800 | ((0x06<< 1)+1))
#define REG_SC_BK7A_3A_L                        (0x137A00 | (0x3A<< 1))

#define REG_FSC_BK19_10_L                        (0x141900 | (0x10<< 1))
#define REG_FSC_BK19_13_L                        (0x141900 | (0x13<< 1))

#define REG_FSC_BK1A_04_L                        (0x141A00 | (0x04<< 1))
#define REG_FSC_BK1A_0B_L                        (0x141A00 | (0x0B<< 1))

#define REG_FSC_BK1B_1C_L                        (0x141B00 | (0x1C<< 1))
#define REG_FSC_BK1B_1D_L                        (0x141B00 | (0x1D<< 1))

#define REG_FSC_BK30_01_L                        (0x143000 | (0x01<< 1))
#define REG_FSC_BK30_02_L                        (0x143000 | (0x02<< 1))
#define REG_FSC_BK30_03_L                        (0x143000 | (0x03<< 1))
#define REG_FSC_BK30_04_L                        (0x143000 | (0x04<< 1))
#define REG_FSC_BK30_05_L                        (0x143000 | (0x05<< 1))
#define REG_FSC_BK30_06_L                        (0x143000 | (0x06<< 1))
#define REG_FSC_BK30_07_L                        (0x143000 | (0x07<< 1))
#define REG_FSC_BK30_08_L                        (0x143000 | (0x08<< 1))
#define REG_FSC_BK30_09_L                        (0x143000 | (0x09<< 1))
#define REG_FSC_BK30_0A_L                        (0x143000 | (0x0A<< 1))
#define REG_FSC_BK30_0B_L                        (0x143000 | (0x0B<< 1))
#define REG_FSC_BK30_0C_L                        (0x143000 | (0x0C<< 1))
#define REG_FSC_BK30_0D_L                        (0x143000 | (0x0D<< 1))
#define REG_FSC_BK30_0E_L                        (0x143000 | (0x0E<< 1))
#define REG_FSC_BK30_0F_L                        (0x143000 | (0x0F<< 1))
#define REG_FSC_BK30_24_L                        (0x143000 | (0x24<< 1))
#define REG_FSC_BK30_25_L                        (0x143000 | (0x25<< 1))
#define REG_FSC_BK30_26_L                        (0x143000 | (0x26<< 1))
#define REG_FSC_BK30_1D_L                        (0x143000 | (0x1D<< 1))

#define REG_FSC_BK55_30_L                        (0x145500 | (0x30<< 1))
#define REG_FSC_BK55_31_L                        (0x145500 | (0x31<< 1))
#define REG_FSC_BK55_32_L                        (0x145500 | (0x32<< 1))
#define REG_FSC_BK55_33_L                        (0x145500 | (0x33<< 1))
#define REG_FSC_BK55_34_L                        (0x145500 | (0x34<< 1))
#define REG_FSC_BK55_35_L                        (0x145500 | (0x35<< 1))
#define REG_FSC_BK55_36_L                        (0x145500 | (0x36<< 1))
#define REG_FSC_BK55_37_L                        (0x145500 | (0x37<< 1))
#define REG_FSC_BK55_38_L                        (0x145500 | (0x38<< 1))
#define REG_FSC_BK55_39_L                        (0x145500 | (0x39<< 1))
#define REG_FSC_BK55_3A_L                        (0x145500 | (0x3A<< 1))
#define REG_FSC_BK55_3B_L                        (0x145500 | (0x3B<< 1))
#define REG_FSC_BK55_3C_L                        (0x145500 | (0x3C<< 1))
#define REG_FSC_BK55_3D_L                        (0x145500 | (0x3D<< 1))
#define REG_FSC_BK55_3E_L                        (0x145500 | (0x3E<< 1))
#define REG_FSC_BK55_3F_L                        (0x145500 | (0x3F<< 1))
#define REG_FSC_BK55_40_L                        (0x145500 | (0x40<< 1))
#define REG_FSC_BK55_41_L                        (0x145500 | (0x41<< 1))
#define REG_FSC_BK55_42_L                        (0x145500 | (0x42<< 1))
#define REG_FSC_BK55_43_L                        (0x145500 | (0x43<< 1))
#define REG_FSC_BK55_44_L                        (0x145500 | (0x44<< 1))
#define REG_FSC_BK55_45_L                        (0x145500 | (0x45<< 1))
#define REG_FSC_BK55_46_L                        (0x145500 | (0x46<< 1))
#define REG_FSC_BK55_47_L                        (0x145500 | (0x47<< 1))
#define REG_FSC_BK55_48_L                        (0x145500 | (0x48<< 1))
#define REG_FSC_BK55_49_L                        (0x145500 | (0x49<< 1))
#define REG_FSC_BK55_4A_L                        (0x145500 | (0x4A<< 1))
#define REG_FSC_BK55_4B_L                        (0x145500 | (0x4B<< 1))
#define REG_FSC_BK55_4C_L                        (0x145500 | (0x4C<< 1))
#define REG_FSC_BK55_4D_L                        (0x145500 | (0x4D<< 1))
#define REG_FSC_BK55_4E_L                        (0x145500 | (0x4E<< 1))
#define REG_FSC_BK55_4F_L                        (0x145500 | (0x4F<< 1))
#define REG_FSC_BK55_50_L                        (0x145500 | (0x50<< 1))
#define REG_FSC_BK55_51_L                        (0x145500 | (0x51<< 1))
#define REG_FSC_BK55_52_L                        (0x145500 | (0x52<< 1))
#define REG_FSC_BK55_53_L                        (0x145500 | (0x53<< 1))
#define REG_FSC_BK55_54_L                        (0x145500 | (0x54<< 1))
#define REG_FSC_BK55_55_L                        (0x145500 | (0x55<< 1))
#define REG_FSC_BK55_56_L                        (0x145500 | (0x56<< 1))
#define REG_FSC_BK55_57_L                        (0x145500 | (0x57<< 1))
#define REG_FSC_BK55_58_L                        (0x145500 | (0x58<< 1))
#define REG_FSC_BK55_59_L                        (0x145500 | (0x59<< 1))
#define REG_FSC_BK55_5A_L                        (0x145500 | (0x5A<< 1))
#define REG_FSC_BK55_5B_L                        (0x145500 | (0x5B<< 1))
#define REG_FSC_BK55_5C_L                        (0x145500 | (0x5C<< 1))
#define REG_FSC_BK55_5D_L                        (0x145500 | (0x5D<< 1))
#define REG_FSC_BK55_5E_L                        (0x145500 | (0x5E<< 1))
#define REG_FSC_BK55_5F_L                        (0x145500 | (0x5F<< 1))
#define REG_FSC_BK55_60_L                        (0x145500 | (0x60<< 1))
#define REG_FSC_BK55_61_L                        (0x145500 | (0x61<< 1))
#define REG_FSC_BK55_62_L                        (0x145500 | (0x62<< 1))
#define REG_FSC_BK55_63_L                        (0x145500 | (0x63<< 1))
#define REG_FSC_BK55_64_L                        (0x145500 | (0x64<< 1))
#define REG_FSC_BK55_65_L                        (0x145500 | (0x65<< 1))
#define REG_FSC_BK55_66_L                        (0x145500 | (0x66<< 1))
#define REG_FSC_BK55_67_L                        (0x145500 | (0x67<< 1))
#define REG_FSC_BK55_68_L                        (0x145500 | (0x68<< 1))
#define REG_FSC_BK55_69_L                        (0x145500 | (0x69<< 1))
#define REG_FSC_BK55_6A_L                        (0x145500 | (0x6A<< 1))
#define REG_FSC_BK55_6B_L                        (0x145500 | (0x6B<< 1))
#define REG_FSC_BK55_6C_L                        (0x145500 | (0x6C<< 1))
#define REG_FSC_BK55_6D_L                        (0x145500 | (0x6D<< 1))
#define REG_FSC_BK55_6E_L                        (0x145500 | (0x6E<< 1))
#define REG_FSC_BK55_6F_L                        (0x145500 | (0x6F<< 1))

#define REG_FSC_BK5F_3F_L                        (0x145F00 | (0x3F<< 1))

#define _PK_L_(bank, addr)   (((MS_U16)(bank) << 8) | (MS_U16)((addr)*2)|REG_SCALER_BASE)
#define _PK_H_(bank, addr)   (((MS_U16)(bank) << 8) | (MS_U16)((addr)*2+1)|REG_SCALER_BASE)

#define REG_SC_BK5F_50_L      _PK_L_(0x5F, 0x50)
#define REG_SC_BK5F_50_H      _PK_H_(0x5F, 0x50)
#define REG_SC_BK5F_51_L      _PK_L_(0x5F, 0x51)
#define REG_SC_BK5F_51_H      _PK_H_(0x5F, 0x51)
#define REG_SC_BK5F_52_L      _PK_L_(0x5F, 0x52)
#define REG_SC_BK5F_52_H      _PK_H_(0x5F, 0x52)
#define REG_SC_BK5F_53_L      _PK_L_(0x5F, 0x53)
#define REG_SC_BK5F_53_H      _PK_H_(0x5F, 0x53)
#define REG_SC_BK5F_54_L      _PK_L_(0x5F, 0x54)
#define REG_SC_BK5F_54_H      _PK_H_(0x5F, 0x54)
#define REG_SC_BK5F_55_L      _PK_L_(0x5F, 0x55)
#define REG_SC_BK5F_55_H      _PK_H_(0x5F, 0x55)
#define REG_SC_BK5F_56_L      _PK_L_(0x5F, 0x56)
#define REG_SC_BK5F_56_H      _PK_H_(0x5F, 0x56)
#define REG_SC_BK5F_57_L      _PK_L_(0x5F, 0x57)
#define REG_SC_BK5F_57_H      _PK_H_(0x5F, 0x57)
#define REG_SC_BK5F_58_L      _PK_L_(0x5F, 0x58)
#define REG_SC_BK5F_58_H      _PK_H_(0x5F, 0x58)
#define REG_SC_BK5F_59_L      _PK_L_(0x5F, 0x59)
#define REG_SC_BK5F_59_H      _PK_H_(0x5F, 0x59)
#define REG_SC_BK5F_5A_L      _PK_L_(0x5F, 0x5A)
#define REG_SC_BK5F_5A_H      _PK_H_(0x5F, 0x5A)
#define REG_SC_BK5F_5B_L      _PK_L_(0x5F, 0x5B)
#define REG_SC_BK5F_5B_H      _PK_H_(0x5F, 0x5B)
#define REG_SC_BK5F_5C_L      _PK_L_(0x5F, 0x5C)
#define REG_SC_BK5F_5C_H      _PK_H_(0x5F, 0x5C)
#define REG_SC_BK5F_5D_L      _PK_L_(0x5F, 0x5D)
#define REG_SC_BK5F_5D_H      _PK_H_(0x5F, 0x5D)
#define REG_SC_BK5F_5E_L      _PK_L_(0x5F, 0x5E)
#define REG_SC_BK5F_5E_H      _PK_H_(0x5F, 0x5E)
#define REG_SC_BK5F_5F_L      _PK_L_(0x5F, 0x5F)
#define REG_SC_BK5F_5F_H      _PK_H_(0x5F, 0x5F)
#define REG_SC_BK5F_60_L      _PK_L_(0x5F, 0x60)
#define REG_SC_BK5F_60_H      _PK_H_(0x5F, 0x60)
#define REG_SC_BK5F_61_L      _PK_L_(0x5F, 0x61)
#define REG_SC_BK5F_61_H      _PK_H_(0x5F, 0x61)
#define REG_SC_BK5F_62_L      _PK_L_(0x5F, 0x62)
#define REG_SC_BK5F_62_H      _PK_H_(0x5F, 0x62)
#define REG_SC_BK5F_63_L      _PK_L_(0x5F, 0x63)
#define REG_SC_BK5F_63_H      _PK_H_(0x5F, 0x63)
#define REG_SC_BK5F_64_L      _PK_L_(0x5F, 0x64)
#define REG_SC_BK5F_64_H      _PK_H_(0x5F, 0x64)
#define REG_SC_BK5F_65_L      _PK_L_(0x5F, 0x65)
#define REG_SC_BK5F_65_H      _PK_H_(0x5F, 0x65)
#define REG_SC_BK5F_66_L      _PK_L_(0x5F, 0x66)
#define REG_SC_BK5F_66_H      _PK_H_(0x5F, 0x66)
#define REG_SC_BK5F_67_L      _PK_L_(0x5F, 0x67)
#define REG_SC_BK5F_67_H      _PK_H_(0x5F, 0x67)
#define REG_SC_BK5F_68_L      _PK_L_(0x5F, 0x68)
#define REG_SC_BK5F_68_H      _PK_H_(0x5F, 0x68)
#define REG_SC_BK5F_69_L      _PK_L_(0x5F, 0x69)
#define REG_SC_BK5F_69_H      _PK_H_(0x5F, 0x69)
#define REG_SC_BK5F_6A_L      _PK_L_(0x5F, 0x6A)
#define REG_SC_BK5F_6A_H      _PK_H_(0x5F, 0x6A)
#define REG_SC_BK5F_6B_L      _PK_L_(0x5F, 0x6B)
#define REG_SC_BK5F_6B_H      _PK_H_(0x5F, 0x6B)
#define REG_SC_BK5F_6C_L      _PK_L_(0x5F, 0x6C)
#define REG_SC_BK5F_6C_H      _PK_H_(0x5F, 0x6C)
#define REG_SC_BK5F_6D_L      _PK_L_(0x5F, 0x6D)
#define REG_SC_BK5F_6D_H      _PK_H_(0x5F, 0x6D)
#define REG_SC_BK5F_6E_L      _PK_L_(0x5F, 0x6E)
#define REG_SC_BK5F_6E_H      _PK_H_(0x5F, 0x6E)
#define REG_SC_BK5F_6F_L      _PK_L_(0x5F, 0x6F)
#define REG_SC_BK5F_6F_H      _PK_H_(0x5F, 0x6F)
#define REG_SC_BK5F_70_L      _PK_L_(0x5F, 0x70)
#define REG_SC_BK5F_70_H      _PK_H_(0x5F, 0x70)
#define REG_SC_BK5F_71_L      _PK_L_(0x5F, 0x71)
#define REG_SC_BK5F_71_H      _PK_H_(0x5F, 0x71)
#define REG_SC_BK5F_72_L      _PK_L_(0x5F, 0x72)
#define REG_SC_BK5F_72_H      _PK_H_(0x5F, 0x72)
#define REG_SC_BK5F_73_L      _PK_L_(0x5F, 0x73)
#define REG_SC_BK5F_73_H      _PK_H_(0x5F, 0x73)
#define REG_SC_BK5F_74_L      _PK_L_(0x5F, 0x74)
#define REG_SC_BK5F_74_H      _PK_H_(0x5F, 0x74)
#define REG_SC_BK5F_75_L      _PK_L_(0x5F, 0x75)
#define REG_SC_BK5F_75_H      _PK_H_(0x5F, 0x75)
#define REG_SC_BK5F_76_L      _PK_L_(0x5F, 0x76)
#define REG_SC_BK5F_76_H      _PK_H_(0x5F, 0x76)
#define REG_SC_BK5F_77_L      _PK_L_(0x5F, 0x77)
#define REG_SC_BK5F_77_H      _PK_H_(0x5F, 0x77)
#define REG_SC_BK5F_78_L      _PK_L_(0x5F, 0x78)
#define REG_SC_BK5F_78_H      _PK_H_(0x5F, 0x78)
#define REG_SC_BK5F_79_L      _PK_L_(0x5F, 0x79)
#define REG_SC_BK5F_79_H      _PK_H_(0x5F, 0x79)
#define REG_SC_BK5F_7A_L      _PK_L_(0x5F, 0x7A)
#define REG_SC_BK5F_7A_H      _PK_H_(0x5F, 0x7A)
#define REG_SC_BK5F_7B_L      _PK_L_(0x5F, 0x7B)
#define REG_SC_BK5F_7B_H      _PK_H_(0x5F, 0x7B)
#define REG_SC_BK5F_7C_L      _PK_L_(0x5F, 0x7C)
#define REG_SC_BK5F_7C_H      _PK_H_(0x5F, 0x7C)
#define REG_SC_BK5F_7D_L      _PK_L_(0x5F, 0x7D)
#define REG_SC_BK5F_7D_H      _PK_H_(0x5F, 0x7D)
#define REG_SC_BK5F_7E_L      _PK_L_(0x5F, 0x7E)
#define REG_SC_BK5F_7E_H      _PK_H_(0x5F, 0x7E)
#define REG_SC_BK5F_7F_L      _PK_L_(0x5F, 0x7F)
#define REG_SC_BK5F_7F_H      _PK_H_(0x5F, 0x7F)


#define _BIT0                                   (0x0001)
#define _BIT1                                   (0x0002)
#define _BIT2                                   (0x0004)
#define _BIT3                                   (0x0008)
#define _BIT4                                   (0x0010)
#define _BIT5                                   (0x0020)
#define _BIT6                                   (0x0040)
#define _BIT7                                   (0x0080)

#define BIT(x) (1UL << (x))

#define L_BK_VOP(_x_)                           (REG_SCALER_BASE | (REG_SC_BK_VOP << 8) | (_x_ << 1))
#define L_BK_DLC(_x_)                           (REG_SCALER_BASE | (REG_SC_BK_DLC << 8) | (_x_ << 1))
#define H_BK_DLC(_x_)                           (REG_SCALER_BASE | (REG_SC_BK_DLC << 8) | ((_x_ << 1)+1))
#define L_BK_OP(_x_)                            (REG_SCALER_BASE | (REG_SC_BK_OP << 8) | (_x_ << 1))

#define L_BK_IP_DLC(_x_)                           (0x130000 | (REG_IP_BK_DLC << 8) | (_x_ << 1))
#define H_BK_IP_DLC(_x_)                           (0x130000 | (REG_IP_BK_DLC << 8) | ((_x_ << 1)+1))

#define REG_SCALER_BASE                         0x130000
#define REG_SC_BK_VOP                           0x10
#define REG_SC_BK_DLC                           0x1A
#define REG_SC_BK_OP                            0x20

#define REG_IP_BK_DLC                           0x5C


#if defined(CONFIG_ARM64)
extern volatile MS_U64 g_u64MstarPmBase ;
#define REG_ADDR(addr)                          (*((volatile unsigned short int*)(g_u64MstarPmBase + (addr << 1))))
#else
#define REG_ADDR(addr)                          (*((volatile unsigned short int*)(0xFD000000 + (addr << 1))))
#endif

// read 2 byte
#define REG_RR(_reg_)                           ({REG_ADDR(_reg_);})

// write low byte
#define REG_WL(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) = (REG_ADDR(_reg_) & 0xFF00) | ((_val_) & 0x00FF); }while(0)

// write high byte
#define REG_WH(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) = (REG_ADDR(_reg_)  & 0x00FF) | ((_val_) << 8); }while(0)

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

//=============================================================================
// HAL Driver Function
//=============================================================================

INTERFACE void      MApi_GFLIP_XC_W2BYTE(DWORD u32Reg, WORD u16Val );
INTERFACE WORD      MApi_GFLIP_XC_R2BYTE(DWORD u32Reg );
INTERFACE void      MApi_GFLIP_XC_W2BYTEMSK(DWORD u32Reg, WORD u16Val, WORD u16Mask );
INTERFACE WORD      MApi_GFLIP_XC_R2BYTEMSK(DWORD u32Reg, WORD u16Mask );
INTERFACE void msWriteByte(DWORD u32Reg, BYTE u8Val ) ;
INTERFACE BYTE msReadByte(DWORD u32Reg ) ;
INTERFACE void msWriteByteMask(DWORD u32Reg, BYTE u8Val ,MS_U8 u8Mask);
//------------------------------------------------------------------------
// HDR Begin
//------------------------------------------------------------------------

///HDR Code////
typedef struct
{
    MS_U16 smin; // 0.10
    MS_U16 smed; // 0.10
    MS_U16 smax; // 0.10
    MS_U16 tmin; // 0.10
    MS_U16 tmed; // 0.10
    MS_U16 tmax; // 0.10
    MS_U16 slope; // 4.12
    MS_U16 rolloff; // 4.12
} HDR_ToneMappingData;

typedef struct
{
    MS_BOOL sValid; // source data valid
    MS_BOOL tValid; // target data valid
    MS_U16 sRx; // source Rx
    MS_U16 sRy; // source Ry
    MS_U16 sGx; // source Gx
    MS_U16 sGy; // source Gy
    MS_U16 sBx; // source Bx
    MS_U16 sBy; // source By
    MS_U16 sWx; // source Wx
    MS_U16 sWy; // source Wy
    MS_U16 tRx; // target Rx
    MS_U16 tRy; // target Ry
    MS_U16 tGx; // target Gx
    MS_U16 tGy; // target Gy
    MS_U16 tBx; // target Bx
    MS_U16 tBy; // target By
    MS_U16 tWx; // target Wx
    MS_U16 tWy; // target Wy
} HDR_GamutMappingData;

typedef struct
{
    MS_BOOL sValid; // source data valid
    MS_BOOL tValid; // target data valid
    MS_U8 sMode; // 0:gamma, 1:DCI-P3, 2:PQ, 3:Log
    //U16 sG; // 2.14
    //U16 sB; // 0.16
    MS_U16 sData[4]; //[0]:sG [1]:sB
    MS_U8 tMode; // 0:gamma, 1:DCI-P3, 2:PQ, 3:Log
    MS_U16 tData[4];
} HDR_TransferFunctionData;

// Data format follows HDR_Metadata_MpegSEI_MasteringColorVolume
typedef struct
{
    MS_U16     display_primaries_x[3]; // x = data*0.00002    [709] {15000, 7500, 32000}
    MS_U16     display_primaries_y[3]; // y = data*0.00002    [709] {30000, 3000, 16500}
    MS_U16     white_point_x; // x = data*0.00002    [709] 15635
    MS_U16     white_point_y; // y = data*0.00002    [709] 16450
    MS_U32     max_display_mastering_luminance; // data*0.0001 nits    [600nits] 6000000
    MS_U32     min_display_mastering_luminance; // data*0.0001 nits    [0.3nits] 3000
} HDR_Metadata_FromPanel;

// Document : w14700 SEI - D.2.15 : Tone mapping information SEI message syntax
typedef struct
{
    MS_U8      tone_map_id;
    MS_BOOL    tone_map_cancel_flag;
    MS_BOOL    tone_map_persistence_flag;
    MS_U8      coded_data_bit_depth;
    MS_U8      target_bit_depth;
    MS_U8      tone_map_model_id;
    MS_U32     min_value;
    MS_U32     max_value;
    MS_U32     sigmoid_midpoint;
    MS_U32     sigmoid_width;
    MS_U16*    start_of_coded_interval; // array size = (1 << target_bit_depth)
    MS_U16     num_pivots;
    MS_U16*    coded_pivot_value;  // array size = num_pivots
    MS_U16*    target_pivot_value; // array size = num_pivots
    MS_U8      camera_iso_speed_idc;
    MS_U32     camera_iso_speed_value;
    MS_U8      exposure_index_idc;
    MS_U32     exposure_index_value;
    MS_BOOL    exposure_compensation_value_sign_flag;
    MS_U16     exposure_compensation_value_numerator;
    MS_U16     exposure_compensation_value_denom_idc;
    MS_U32     ref_screen_luminance_white;
    MS_U32     extended_range_white_level;
    MS_U16     nominal_black_level_code_value;
    MS_U16     nominal_white_level_code_value;
    MS_U16     extended_white_level_code_value;
} HDR_Metadata_MpegSEI_ToneMapping;

// Document : w14700 SEI - D.2.27 : Mastering display colour volume SEI message syntax
typedef struct
{
    MS_U16     display_primaries_x[3]; // x = data*0.00002
    MS_U16     display_primaries_y[3]; // y = data*0.00002
    MS_U16     white_point_x; // x = data*0.00002
    MS_U16     white_point_y; // y = data*0.00002
    MS_U32     max_display_mastering_luminance; // data*0.0001 nits
    MS_U32     min_display_mastering_luminance; // data*0.0001 nits
} HDR_Metadata_MpegSEI_MasteringColorVolume;

// Document : w14700 SEI - D.2.31 : Knee function information SEI message syntax
typedef struct
{
    MS_U8      knee_function_id;
    MS_BOOL    knee_function_cancel_flag;
    MS_BOOL    knee_function_persistence_flag;
    MS_U32     input_d_range;
    MS_U32     input_disp_luminance;
    MS_U32     output_d_range;
    MS_U32     output_disp_luminance;
    MS_U16     num_knee_points_minus1; // num_knee_points_minus1 shall be in the range of 0 to 998, inclusive.
    MS_U16*    input_knee_point; // array size = num_knee_points_minus1+1
    MS_U16*    output_knee_point; // array size = num_knee_points_minus1+1
} HDR_Metadata_MpegSEI_KneeFunction;

// Document : CEA-861.3_V16BallotDraft
typedef struct
{
    MS_U8      u8EOTF; // 0:SDR gamma, 1:HDR gamma, 2:SMPTE ST2084, 3:Future EOTF, 4-7:Reserved
    MS_U8      u8Lmax; // desired content max luminance data (8 bits)
    MS_U8      u8Lavg; // desired content max frame-average luminance data (8 bits)
    MS_U8      u8Lmin; // desired content min luminance data (8 bits)
} HDR_Metadata_HdmiRx_DataBlock;

INTERFACE void MDrv_HDR_ToneMappingCurveGen_Mod(MS_U16* pCurve18, HDR_ToneMappingData* pData,MS_BOOL bWindow);
INTERFACE MS_BOOL msTMOSet(MS_U16 *u16TMOData , MS_U16 u16TMOSize);
INTERFACE void msHDRFixDlcWriteCurve(MS_BOOL bWindow);

#if defined(CONFIG_ARM64)
void    msDlcSetPMBase(MS_U64 u64PMBase);
#endif
//------------------------------------------------------------------------
// HDR End
//------------------------------------------------------------------------



#if DLC_FUNCTION

void    msDlcHandler(MS_BOOL bWindow);
MS_BOOL    msGetHistogramHandler(MS_BOOL bWindow);
void    msDlcWriteCurve(MS_BOOL bWindow);
void    msDlcWriteCurveLSB(MS_BOOL bWindow, BYTE ucIndex, BYTE ucValue);
void    msDlcLumiDiffCtrl(void);
void    msDlcSetCurve(MS_U8 *_u8DlcCurveInit);
void    msDlcSetBleSlopPoint(MS_U16 *_u16BLESlopPoint);
MS_BOOL    msDlcInit(MS_U16 *u16DlcInit );
void    msDlcInitEx(void);
void    msDlcSetBleOnOff(MS_BOOL bSwitch);
MS_BOOL    msDlcGetHistogram32Info(MS_U16 *pu16Histogram);
MS_U8   msGetAverageLuminous(void);
MS_U8   msGetAverageLuminousSub(void);
MS_BOOL    msIsBlackVideoEnable( MS_BOOL bWindow );
MS_BOOL    msDLCMSTVToolReadWriteValue( void );
void    msDlcSetYGain(MS_U8 u8YGain, MS_BOOL bWindow);
MS_U8   msDlcGetYGain(MS_BOOL bWindow);
void    msDlcSetCGain(MS_U8 u8CGain, MS_BOOL bWindow);
MS_U8   msDlcGetCGain(MS_BOOL bWindow);
MS_U8 GetDlcFunctionControl(void);
void DisableDlcFunctionControl(void);
MS_BOOL msTMOinit(void);
void Color_Format_Driver(void);
void HistogramExpand(MS_U16 * u16LimitSourceIn, MS_U16 * u16FullSouceOut);
void Mapi_Cfd_TMO_Unit_PQ2GAMMA(MS_U16 *pu16Result_TMO_PQ2Gamma,
                                MS_U16 u16Length_Result_TMO_PQ2Gamma,
                                const MS_U16 *u16pInput_Gamma_Idx,
                                const MS_U16 *u16pInput_PQArray,
                                MS_U16 u16Length_Input_PQArray,
                                MS_U16 u16target_max_lum_nits);
void msTmoHandler(MS_BOOL bWindow);
void msWriteTmoCurve(MS_BOOL bWindow);
typedef struct
{
    // Default luma curve
    BYTE ucLumaCurve[16];
    BYTE ucLumaCurve2_a[16];
    BYTE ucLumaCurve2_b[16];

    BYTE ucDlcPureImageMode; // Compare difference of max and min bright
    BYTE ucDlcLevelLimit; // n = 0 ~ 4 => Limit n levels => ex. n=2, limit 2 level 0xF7, 0xE7
    BYTE ucDlcAvgDelta; // n = 0 ~ 50, default value: 12
    BYTE ucDlcAvgDeltaStill; // n = 0 ~ 15 => 0: disable still curve, 1 ~ 15 => 0.1 ~ 1.5 enable still curve
    BYTE ucDlcFastAlphaBlending; // min 17 ~ max 32
    BYTE ucDlcSlowEvent; // some event is triggered, DLC must do slowly // for PIP On/Off, msMultiPic.c
    BYTE ucDlcTimeOut; // for IsrApp.c
    BYTE ucDlcFlickAlphaStart; // for force to do fast DLC in a while
    BYTE ucDlcYAvgThresholdH; // default value: 128
    BYTE ucDlcYAvgThresholdL; // default value: 0
    BYTE ucDlcBLEPoint; // n = 24 ~ 64, default value: 48
    BYTE ucDlcWLEPoint; // n = 24 ~ 64, default value: 48
    BYTE bCGCCGainCtrl : 1; // 1: enable; 0: disable
    BYTE bEnableBLE : 1; // 1: enable; 0: disable
    BYTE bEnableWLE : 1; // 1: enable; 0: disable
    BYTE ucDlcYAvgThresholdM;
    BYTE ucDlcCurveMode;
    BYTE ucDlcCurveModeMixAlpha;
    BYTE ucDlcAlgorithmMode;
    BYTE ucDlcHistogramLimitCurve[17];
    BYTE ucDlcSepPointH;
    BYTE ucDlcSepPointL;
    WORD uwDlcBleStartPointTH;
    WORD uwDlcBleEndPointTH;
    BYTE ucDlcCurveDiff_L_TH;
    BYTE ucDlcCurveDiff_H_TH;
    WORD uwDlcBLESlopPoint_1;
    WORD uwDlcBLESlopPoint_2;
    WORD uwDlcBLESlopPoint_3;
    WORD uwDlcBLESlopPoint_4;
    WORD uwDlcBLESlopPoint_5;
    WORD uwDlcDark_BLE_Slop_Min;
    BYTE ucDlcCurveDiffCoringTH;
    BYTE ucDlcAlphaBlendingMin;
    BYTE ucDlcAlphaBlendingMax;
    BYTE ucDlcFlicker_alpha;
    BYTE ucDlcYAVG_L_TH;
    BYTE ucDlcYAVG_H_TH;
    BYTE ucDlcDiffBase_L;
    BYTE ucDlcDiffBase_M;
    BYTE ucDlcDiffBase_H;
    // NEW DLC
    BYTE ucLMaxThreshold;
    BYTE ucLMinThreshold;
    BYTE ucLMaxCorrection;
    BYTE ucLMinCorrection;
    BYTE ucRMaxThreshold;
    BYTE ucRMinThreshold;
    BYTE ucRMaxCorrection;
    BYTE ucRMinCorrection;
    BYTE ucAllowLoseContrast;
    BYTE ucKernelDlcAlgorithm;
    BYTE ucDlcHistogramSource;
    MS_U8 u8Dlc_Mode;
    MS_U8 u8Tmo_Mode;
}StuDlc_FinetuneParamaters;

#endif

typedef struct __attribute__((packed))
{
    MS_U8      u8ColorPrimaries;
    MS_U8      u8TransferCharacteristics;
    MS_U8      u8MatrixCoefficients;
} StuDlc_HDRMetadataMpegVUI;

// Data format follows HDR_Metadata_MpegSEI_MasteringColorVolume
typedef struct __attribute__((packed))
{
    MS_U16     display_primaries_x[3]; // x = data*0.00002    [709] {15000, 7500, 32000}
    MS_U16     display_primaries_y[3]; // y = data*0.00002    [709] {30000, 3000, 16500}
    MS_U16     white_point_x; // x = data*0.00002    [709] 15635
    MS_U16     white_point_y; // y = data*0.00002    [709] 16450
    MS_U32     max_display_mastering_luminance; // data*0.0001 nits    [600nits] 6000000
    MS_U32     min_display_mastering_luminance; // data*0.0001 nits    [0.3nits] 3000
} StuDlc_HDRMetadataMpegSEIMasteringColorVolume;

///HDR Code////
typedef struct __attribute__((packed))
{
    MS_U16 u16Smin; // 0.10
    MS_U16 u16Smed; // 0.10
    MS_U16 u16Smax; // 0.10
    MS_U16 u16Tmin; // 0.10
    MS_U16 u16Tmed; // 0.10
    MS_U16 u16Tmax; // 0.10
    MS_U16 u16MidSourceOffset;
    MS_U16 u16MidTargetOffset;
    MS_U16 u16MidSourceRatio;
    MS_U16 u16MidTargetRatio;
} StuDlc_HDRToneMappingData;

typedef struct __attribute__((packed))
{
    MS_U16 u16tRx; // target Rx
    MS_U16 u16tRy; // target Ry
    MS_U16 u16tGx; // target Gx
    MS_U16 u16tGy; // target Gy
    MS_U16 u16tBx; // target Bx
    MS_U16 u16tBy; // target By
    MS_U16 u16tWx; // target Wx
    MS_U16 u16tWy; // target Wy
} StuDlc_HDRGamutMappingData;

// Document : CEA-861.3_V16BallotDraft
typedef struct __attribute__((packed))
{
    MS_U8      u8EOTF; // 0:SDR gamma, 1:HDR gamma, 2:SMPTE ST2084, 3:Future EOTF, 4-7:Reserved
    MS_U16     u16Rx; // display primaries Rx
    MS_U16     u16Ry; // display primaries Ry
    MS_U16     u16Gx; // display primaries Gx
    MS_U16     u16Gy; // display primaries Gy
    MS_U16     u16Bx; // display primaries Bx
    MS_U16     u16By; // display primaries By
    MS_U16     u16Wx; // display primaries Wx
    MS_U16     u16Wy; // display primaries Wy
    MS_U16     u16Lmax; // max display mastering luminance
    MS_U16     u16Lmin; // min display mastering luminance
    MS_U16     u16MaxCLL; // maximum content light level
    MS_U16     u16MaxFALL; // maximum frame-average light level
} StuDlc_HDRMetadataHdmiTxInfoFrame;

// HDR use customer DLC curve.
typedef struct __attribute__((packed))
{
    MS_BOOL bFixHdrCurve;
    MS_U16 u16DlcCurveSize;
    MS_U8 *pucDlcCurve;
} StuDLC_HDRCustomerDlcCurve;

// HDR use customer color primaries.
typedef struct __attribute__((packed))
{
    MS_BOOL bCustomerEnable;
    MS_U16 u16sWx;
    MS_U16 u16sWy;
} StuDlc_HDRCustomerColorPrimaries;

typedef struct __attribute__((packed))
{
    MS_U8 PixelFormat;              // Pixel Format
    MS_U8 Colorimetry;              // Color imetry
    MS_U8 ExtendedColorimetry;      // Extended Color imetry
    MS_U8 RgbQuantizationRange;     // Rgb Quantization Range
    MS_U8 YccQuantizationRange;     // Ycc Quantization Range
    MS_U8 StaticMetadataDescriptorID; //Static Metadata Descriptor ID
} StuDlc_HDRHdmiTxAviInfoFrame;

// HDR new tone mapping parameters.
typedef struct __attribute__((packed))
{
    // TMO
    MS_U16 u16SrcMinRatio;			//default 10
    MS_U16 u16SrcMedRatio;                   //default 512
    MS_U16 u16SrcMaxRatio;			//default 990

    MS_U8 u8TgtMinFlag;                         //default 1
    MS_U16 u16TgtMin;					//default 500
    MS_U8 u8TgtMaxFlag;                        //default 0
    MS_U16 u16TgtMax;				//default 300
    MS_U16 u16TgtMed;

    MS_U16 u16FrontSlopeMin;			//default 256
    MS_U16 u16FrontSlopeMax;			//default 512
    MS_U16 u16BackSlopeMin;			//default 128
    MS_U16 u16BackSlopeMax;			//default 256

    MS_U16 u16SceneChangeThrd;		//default 1024
    MS_U16 u16SceneChangeRatioMax;	//default 1024

    MS_U8 u8IIRRatio;					//default 31
    MS_U8 u8TMO_TargetMode;      // default 0. 0 : keeps the value in initial function  1 : from output source
    MS_U8 u8TMO_Algorithm;                // default 0.  0: 18 level TMO algorithm, 1: 512 level TMO algorithm.
    MS_U16 u16SDRPanelGain;        //

    MS_U16 u16Smin;
    MS_U16 u16Smed;
    MS_U16 u16Smax;
    MS_U16 u16Tmin;
    MS_U16 u16Tmed;
    MS_U16 u16Tmax;

} StuDlc_HDRNewToneMapping;

/*!
 *  Initial  HDR   Settings
 */
typedef struct __attribute__((packed))
{
    /// HDR Enable
    MS_BOOL bHDREnable;
    /// HDR Function Select
    MS_U16 u16HDRFunctionSelect;
    /// HDR Metadata Mpeg VUI
    StuDlc_HDRMetadataMpegVUI DLC_HDRMetadataMpegVUI;
    //HDR Tone Mapping Data
    StuDlc_HDRToneMappingData DLC_HDRToneMappingData;
    //HDR Gamut Mapping Data
    StuDlc_HDRGamutMappingData DLC_HDRGamutMappingData;
    //HDR Metadata Hdmi Tx Info Frame
    StuDlc_HDRMetadataHdmiTxInfoFrame DLC_HDRMetadataHdmiTxInfoFrame;
    // Customer DLC Curve
    StuDLC_HDRCustomerDlcCurve DLC_HDRCustomerDlcCurve;
    // Customer color primarie.
    StuDlc_HDRCustomerColorPrimaries DLC_HDRCustomerColorPrimaries;
    //HDR Hdmi Tx Avi Info Frame
    StuDlc_HDRHdmiTxAviInfoFrame DLC_HDRHdmiTxAviInfoFrame;
    // HDR metadata MPEG SEI mastering color volume
    StuDlc_HDRMetadataMpegSEIMasteringColorVolume DLC_HDRMetadataMpegSEIMasteringColorVolume;
    // New tone mapping parameters.
    StuDlc_HDRNewToneMapping DLC_HDRNewToneMappingData;
} StuDlc_HDRinit;

#undef INTERFACE
#endif //_HAL_DLC_H

