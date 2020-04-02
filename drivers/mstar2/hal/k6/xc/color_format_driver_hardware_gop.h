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
/// @file   color_format_driver_hardware_gop.h
/// @brief  MStar CFD Driver for GOP setting
/// @author MStar Semiconductor Inc.
/// @attention
/// <b>(OBSOLETED) <em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_COLOR_FORMAT_DRIVER_HARDWARE_GOP_H
#define _HAL_COLOR_FORMAT_DRIVER_HARDWARE_GOP_H

#ifdef _HAL_COLOR_FORMAT_DRIVER_HARDWARE_C
#define INTERFACE
#else
#define INTERFACE                               extern
#endif

#define GOP_BIT0        (0x0001)
#define GOP_BIT1        (0x0002)
#define GOP_BIT2        (0x0004)
#define GOP_BIT3        (0x0008)
#define GOP_BIT4        (0x0010)
#define GOP_BIT5        (0x0020)
#define GOP_BIT6        (0x0040)
#define GOP_BIT7        (0x0080)
#define GOP_BIT8        (0x0100)
#define GOP_BIT9        (0x0200)
#define GOP_BIT10       (0x0400)
#define GOP_BIT11       (0x0800)
#define GOP_BIT12       (0x1000)
#define GOP_BIT13       (0x2000)
#define GOP_BIT14       (0x4000)
#define GOP_BIT15       (0x8000)

#define GOP_0           (0)
#define GOP_1           (1)
#define GOP_2           (2)
#define GOP_3           (3)
#define GOP_4           (4)
#define GOP_MAX         (5)

#define OP_MUX_0        (0)
#define OP_MUX_1        (1)
#define OP_MUX_2        (2)
#define OP_MUX_3        (3)
#define OP_MUX_4        (4)
#define OP_MUX_MAX      (5)

#define HDR_MUX              OP_MUX_4
#define HDR_MUX_MASK         GOP_OP_MUX4
#define HDR_MUX_SHIFT        GOP_OP_MUX4_SHIFT

#define GOP_DST_OP      (0x2)
#define GOP_MIXER_MUX   (0x6)

#define GOP_OP_PIPEDELAY              (0x4B)
#define GOP_HDR_PIPEDELAY             (0x8A)
#define GOP_NONVSCALE_PIPEDELAY       (10)

//Register define
//GOP BANK
#define GOP_BASE_REG             (0x120200)

#define REG_GOP_CTRL0            (GOP_BASE_REG+0x00*2)
    #define GOP_COLOR                (GOP_BIT10)
    #define GOP_COLOR_SHIFT          (10)

#define REG_GOP_DST              (GOP_BASE_REG+0x01*2)
    #define GOP_DST_MASK             (0xF)

#define REG_GOP_HS_PIPE          (GOP_BASE_REG+0x0F*2)
    #define GOP_HS_PIPE              (0x1FFF)

#define GOP_OPMUX_DB_REG         (GOP_BASE_REG+0x29*2)
    #define OPMUX_DBF_EN             (GOP_BIT15)

#define GOP_VIP_VOP_SEL_REG      (GOP_BASE_REG+0x3C*2)
    #define GOP_VIP_VOP_SEL_MASK     (GOP_BIT10)

#define GOP_MIXER_L_REG          (GOP_BASE_REG+0x4C*2)
    #define GOP_MIXER_BYPASS         (GOP_BIT1)
    #define GOP_MIXER_ALPHA_INV      (GOP_BIT2)
    #define GOP_MIXER_VDE_SEL        (GOP_BIT12)
    #define GOP_MIXER_TRIG           (GOP_BIT14)
    #define GOP_MIXER_FORCE_WRITE    (GOP_BIT15)

#define GOP_MIXER_H_REG          (GOP_BASE_REG+0x4D*2)
    #define GOP_VALID_EN             (GOP_BIT4|GOP_BIT3|GOP_BIT2|GOP_BIT1|GOP_BIT0)
    #define GOP_HDR_BYPASS           (GOP_BIT7)

#define GOP_BAK_FORCE_WRITE_REG          (GOP_BASE_REG+0x50*2)
#define GOP_BAK_WRITE			 (GOP_BIT0)

#define GOP_BAK_SEL_EX_REG       (GOP_BASE_REG+0x7C*2)
    #define GOP4_ACK                 (GOP_BIT2)
    #define GOP_OP0_ACK              (GOP_BIT14)
    #define GOP_OP1_ACK              (GOP_BIT13)
    #define GOP_ACK_H                (GOP4_ACK|GOP_OP0_ACK|GOP_OP1_ACK)

#define GOP_OPMUX_REG            (GOP_BASE_REG+(0x7E*2))
    #define GOP_OP_MUX0_SHIFT        (0)
    #define GOP_OP_MUX0              (GOP_BIT2|GOP_BIT1|GOP_BIT0)
    #define GOP_OP_MUX1_SHIFT        (3)
    #define GOP_OP_MUX1              (GOP_OP_MUX0<<GOP_OP_MUX1_SHIFT)
    #define GOP_OP_MUX2_SHIFT        (6)
    #define GOP_OP_MUX2              (GOP_OP_MUX0<<GOP_OP_MUX2_SHIFT)
    #define GOP_OP_MUX3_SHIFT        (9)
    #define GOP_OP_MUX3              (GOP_OP_MUX0<<GOP_OP_MUX3_SHIFT)
    #define GOP_OP_MUX4_SHIFT        (12)
    #define GOP_OP_MUX4              (GOP_OP_MUX0<<GOP_OP_MUX4_SHIFT)
    #define GOP_OP_MUX_MASK          (GOP_OP_MUX4|GOP_OP_MUX3|GOP_OP_MUX2|GOP_OP_MUX1|GOP_OP_MUX0)

#define GOP_BAK_SEL_REG          (GOP_BASE_REG+0x7F*2)
    #define GOP_FORCE_WRITE          (GOP_BIT9)
    #define GOP0_ACK                 (GOP_BIT12)
    #define GOP1_ACK                 (GOP_BIT13)
    #define GOP2_ACK                 (GOP_BIT14)
    #define GOP3_ACK                 (GOP_BIT15)
    #define GOP_ACK_L                (GOP3_ACK|GOP2_ACK|GOP1_ACK|GOP0_ACK)

//SC BANK
#define GOP_VOP_BASE_REG         (0x131000)

#define GOP_HDE_START_REG        (GOP_VOP_BASE_REG+0x04*2)
    #define GOP_HDE_START_MASK       (0x1FFF)

#define GOP_BLENDING_BASE_REG    (0x132F00)

#define GOP_MUX0_BLENDING_REG    (0x132F00+0x37*2)
    #define GOP_MUX0_OSDB_MASK       (GOP_BIT3)
    #define GOP_MUX1_OSDB_MASK       (GOP_BIT11)

#define GOP_MUX2_BLENDING_REG    (0x132F00+0x38*2)
    #define GOP_MUX2_OSDB_MASK       (GOP_BIT3)
    #define GOP_MUX3_OSDB_MASK       (GOP_BIT11)

#define GOP_MUX4_BLENDING_REG    (0x132F00+0x39*2)
    #define GOP_MUX4_OSDB_MASK       (GOP_BIT3)

#endif //_HAL_COLOR_FORMAT_DRIVER_HARDWARE_GOP_H
