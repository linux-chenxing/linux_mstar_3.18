///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2007 MStar Semiconductor, Inc.
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

#ifndef _REG_GPIO_H_
#define _REG_GPIO_H_

//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------
#define GPIO_UNIT_NUM               82

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define REG_MIPS_BASE               0xFD000000      //Use 8 bit addressing

#define REG_ALL_PAD_IN              (0x101ea1)      //set all pads (except SPI) as input
#define REG_LVDS_BASE               (0x103200)
#define REG_LVDS_BANK               REG_LVDS_BASE

#define PAD_GPIO_PM0               0
#define PAD_GPIO_PM1               1
#define PAD_GPIO_PM2               2
#define PAD_GPIO_PM3               3
#define PAD_GPIO_PM4                    4
#define PAD_GPIO_PM5                     5
#define PAD_PM_SPI_CZ                  6
#define PAD_GPIO_PM7                  7
#define PAD_GPIO_PM8                 8
#define PAD_GPIO_PM9                 9
#define PAD_PM_SPI_CK                10
#define PAD_PM_SPI_DO                11
#define PAD_GPIO_PM6                12
#define PAD_PM_SPI_DI                13
#define PAD_VID1                14
#define PAD_VID0                15
#define PAD_GPIO0                16
#define PAD_GPIO1                17
#define PAD_GPIO2                18
#define PAD_GPIO3                19
#define PAD_GPIO4               20
#define PAD_GPIO5               21
#define PAD_GPIO6                  22
#define PAD_GPIO7                23
#define PAD_SPI2_CK                24
#define PAD_SPI2_DI                25
#define PAD_SPI1_CK                26
#define PAD_SPI1_DI                27
#define PAD_I2CM_CK                28
#define PAD_I2CM_DA                29
#define PAD_SM_VCC                30
#define PAD_SM_CD                31
#define PAD_SM_VSEL                32
#define PAD_SM_IO                33
#define PAD_SM_RST                34
#define PAD_SM_CLK                    35
#define PAD_TS_D7                    36
#define PAD_TS_D6                    37
#define PAD_TS_D5                    38
#define PAD_TS_D4                    39
#define PAD_TS_D3                    40
#define PAD_TS_D2                    41
#define PAD_TS_D1                 42
#define PAD_TS_D0                 43
#define PAD_TS_SYNC                   44
#define PAD_TS_VLD                   45
#define PAD_TS_CLK                   46
#define PAD_EJ_TRST_N                   47
#define PAD_EJ_TDO                   48
#define PAD_EJ_TCK                   49
#define PAD_EJ_TDI                   50
#define PAD_EJ_DINT                   51
#define PAD_EJ_RSTZ                   52
#define PAD_EJ_TMS                   53
#define PAD_I2S_IN_WS                  54
#define PAD_I2S_IN_SD                  55
#define PAD_I2S_IN_BCK                  56
#define PAD_I2S_OUT_MCK                  57
#define PAD_I2S_OUT_WS                  58
#define PAD_I2S_OUT_MUTE                  59
#define PAD_I2S_OUT_SD                  60
#define PAD_I2S_OUT_SD1                  61
#define PAD_I2S_OUT_SD2                  62
#define PAD_I2S_OUT_SD3                  63
#define PAD_I2S_OUT_BCK                  64
#define PAD_SPDIF_OUT                  65
#define PAD_SPDIF_IN                  66
#define PAD_LED0                  67
#define PAD_LED1                  68
#define PAD_DDCCK                  69
#define PAD_DDC_DA                  70
#define PAD_SD_D3                  71
#define PAD_SD_D2                  72
#define PAD_SD_D1               73
#define PAD_SD_D0              74
#define PAD_SD_CMD               75
#define PAD_SD_CLK                76
#define PAD_SAR0               77
#define PAD_SAR1              78
#define PAD_SAR2             79
#define PAD_SAR3             80
#define PAD_SAR4              81

#define GPIO_OEN                    0   //set o to nake output
#define GPIO_ODN                    1

#define IN_HIGH                     1   //input high
#define IN_LOW                      0   //input low

#define OUT_HIGH                    1   //output high
#define OUT_LOW                     0   //output low

#define MHal_GPIO_REG(addr)         (*(volatile U8*)(REG_MIPS_BASE + (((addr) & ~1) << 1) + (addr & 1)))

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

#endif // _REG_GPIO_H_

