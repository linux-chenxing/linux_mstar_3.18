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
#define GPIO_UNIT_NUM               224

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
#define REG_MIPS_BASE               0xFD000000      //Use 8 bit addressing
#elif defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define REG_MIPS_BASE           (mstar_pm_base)
#endif

#define REG_ALL_PAD_IN              (0x101ea1UL)      //set all pads (except SPI) as input
#define REG_LVDS_BASE               (0x103200UL)
#define REG_LVDS_BANK               REG_LVDS_BASE

#define PAD_IRIN                     0
#define PAD_CEC0                     1
#define PAD_PWM_PM                   2
#define PAD_DDCA_CK                  3
#define PAD_DDCA_DA                  4
#define PAD_GPIO_PM0                 5
#define PAD_GPIO_PM1                 6
#define PAD_GPIO_PM2                 7
#define PAD_GPIO_PM3                 8
#define PAD_GPIO_PM4                 9
#define PAD_GPIO_PM5                10
#define PAD_GPIO_PM6                11
#define PAD_GPIO_PM7                12
#define PAD_GPIO_PM8                13
#define PAD_GPIO_PM9                14
#define PAD_GPIO_PM10               15
#define PAD_LED0                    16
#define PAD_LED1                    17
#define PAD_HOTPLUGA                18
#define PAD_HOTPLUGB                19
#define PAD_HOTPLUGA_HDMI20_5V      20
#define PAD_HOTPLUGB_HDMI20_5V      21
#define PAD_HOTPLUGC_HDMI20_5V      22
#define PAD_DDCDA_CK                23
#define PAD_DDCDA_DA                24
#define PAD_DDCDB_CK                25
#define PAD_DDCDB_DA                26
#define PAD_DDCDC_CK                27
#define PAD_DDCDC_DA                28
#define PAD_SAR0                    29
#define PAD_SAR1                    30
#define PAD_SAR2                    31
#define PAD_SAR3                    32
#define PAD_SAR4                    33
#define PAD_SAR5                    34
#define PAD_ARC0                    35
#define PAD_DDCR_DA                 36
#define PAD_DDCR_CK                 37
#define PAD_GPIO0                   38
#define PAD_GPIO1                   39
#define PAD_SPDIF_OUT               40
#define PAD_SPDIF_IN                41
#define PAD_I2S_OUT_SD              42
#define PAD_I2S_OUT_MCK             43
#define PAD_I2S_OUT_WS              44
#define PAD_I2S_OUT_BCK             45
#define PAD_EMMC_IO0                46
#define PAD_EMMC_IO1                47
#define PAD_EMMC_IO2                48
#define PAD_EMMC_IO3                49
#define PAD_EMMC_IO4                50
#define PAD_EMMC_IO5                51
#define PAD_EMMC_IO6                52
#define PAD_EMMC_IO7                53
#define PAD_EMMC_IO8                54
#define PAD_EMMC_IO9                55
#define PAD_EMMC_IO10               56
#define PAD_EMMC_IO11               57
#define PAD_PWM0                    58
#define PAD_PWM1                    59
#define PAD_PWM2                    60
#define PAD_PWM3                    61
#define PAD_PWM4                    62
#define PAD_SD_D0                   63
#define PAD_SD_D1                   64
#define PAD_SD_D2                   65
#define PAD_SD_D3                   66
#define PAD_SD_CLK                  67
#define PAD_SD_CMD                  68
#define PAD_TCON0                   69
#define PAD_TCON1                   70
#define PAD_TCON2                   71
#define PAD_TCON3                   72
#define PAD_TCON4                   73
#define PAD_TCON5                   74
#define PAD_TCON6                   75
#define PAD_TCON7                   76
#define PAD_TGPIO0                  77
#define PAD_TGPIO1                  78
#define PAD_TS0_D0                  79
#define PAD_TS0_D1                  80
#define PAD_TS0_D2                  81
#define PAD_TS0_D3                  82
#define PAD_TS0_D4                  83
#define PAD_TS0_D5                  84
#define PAD_TS0_D6                  85
#define PAD_TS0_D7                  86
#define PAD_TS0_VLD                 87
#define PAD_TS0_SYNC                88
#define PAD_TS0_CLK                 89


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

