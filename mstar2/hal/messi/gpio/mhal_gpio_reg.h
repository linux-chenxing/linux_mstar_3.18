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

#define PAD_IRIN            1
#define PAD_CEC0            2
#define PAD_PWM_PM          3
#define PAD_DDCA_CK         4
#define PAD_DDCA_DA         5	
#define PAD_GPIO_PM0        6
#define PAD_GPIO_PM1        7
#define PAD_GPIO_PM2        8
#define PAD_GPIO_PM3        9
#define PAD_GPIO_PM4        10
#define PAD_GPIO_PM5        11
#define PAD_GPIO_PM6        12
#define PAD_GPIO_PM7        13
#define PAD_GPIO_PM8        14
#define PAD_GPIO_PM9        15
#define PAD_GPIO_PM10       16
#define PAD_LED0            17
#define PAD_LED1            18
#define PAD_HOTPLUGA        19
#define PAD_HOTPLUGB        20
#define PAD_HOTPLUGC        21
#define PAD_DDCDA_CK        22
#define PAD_DDCDA_DA        23
#define PAD_DDCDB_CK        24
#define PAD_DDCDB_DA        25
#define PAD_DDCDC_CK        26
#define PAD_DDCDC_DA        27
#define PAD_SAR0            28
#define PAD_SAR1            29
#define PAD_SAR2            30
#define PAD_SAR3            31
#define PAD_SAR4            32
#define PAD_SAR5            33
#define PAD_ARC0            34
#define PAD_DDCR_DA         35
#define PAD_DDCR_CK         36
#define PAD_GPIO0           37
#define PAD_GPIO1           38
#define PAD_GPIO2           39
#define PAD_GPIO3           40
#define PAD_GPIO4           41
#define PAD_GPIO5           42
#define PAD_SPDIF_OUT       43
#define PAD_SPDIF_IN        44
#define PAD_I2S_OUT_SD      45
#define PAD_I2S_OUT_MCK     46
#define PAD_I2S_OUT_WS      47
#define PAD_I2S_OUT_BCK     48
#define PAD_PWM0            49
#define PAD_PWM1            50
#define PAD_PWM2            51
#define PAD_PWM3            52
#define PAD_PWM4            53
#define PAD_TGPIO0          54
#define PAD_TGPIO1          55
#define PAD_TS0_D0          56
#define PAD_TS0_D1          57
#define PAD_TS0_D2          58
#define PAD_TS0_D3          59
#define PAD_TS0_D4          60
#define PAD_TS0_D5          61
#define PAD_TS0_D6          62
#define PAD_TS0_D7          63
#define PAD_TS0_VLD         64
#define PAD_TS0_SYNC        65
#define PAD_TS0_CLK         66
#define PAD_TS1_CLK         67
#define PAD_TS1_SYNC        68
#define PAD_TS1_VLD         69
#define PAD_TS1_D7          70
#define PAD_TS1_D6          71
#define PAD_TS1_D5          72
#define PAD_TS1_D4          73
#define PAD_TS1_D3          74
#define PAD_TS1_D2          75
#define PAD_TS1_D1          76
#define PAD_TS1_D0          77
#define PAD_PCM_D4          78
#define PAD_PCM_D5          79
#define PAD_PCM_D6          80
#define PAD_PCM_D7          81
#define PAD_PCM_A10         82
#define PAD_PCM_OE_N        83
#define PAD_PCM_A11         84
#define PAD_PCM_A13         85
#define PAD_PCM_A14         86
#define PAD_PCM_A3          87
#define PAD_PCM_A2          88
#define PAD_PCM_A4          89
#define PAD_PCM_A7          90
#define PAD_PCM_A6          91
#define PAD_PCM_A5          92
#define PAD_PCM_A12         93
#define PAD_PCM_IORD_N      94
#define PAD_PCM_A9          95
#define PAD_PCM_A1          96
#define PAD_PCM_A0          97
#define PAD_PCM_D0          98
#define PAD_PCM_D1          99
#define PAD_PCM_D2          100
#define PAD_PCM_D3          101
#define PAD_PCM_A8          102
#define PAD_PCM_CE_N        103
#define PAD_PCM_RESET       104
#define PAD_PCM_CD_N        105
#define PAD_PCM_WAIT_N      106
#define PAD_PCM_IRQA_N      107
#define PAD_PCM_IOWR_N      108
#define PAD_PCM_REG_N       109
#define PAD_PCM_WE_N        110


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

