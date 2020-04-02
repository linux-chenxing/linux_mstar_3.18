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
#define GPIO_UNIT_NUM               226

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

#define PAD_PM_SPI_CZ               0
#define PAD_PM_SPI_CK               1
#define PAD_PM_SPI_DI               2
#define PAD_PM_SPI_DO               3
#define PAD_IRIN                    4
#define PAD_CEC0                    5
#define PAD_AV_LNK                  6
#define PAD_PWM_PM                  7
#define PAD_DDCA_CK                 8
#define PAD_DDCA_DA                 9
#define PAD_GPIO_PM0                10
#define PAD_GPIO_PM1                11
#define PAD_GPIO_PM2                12
#define PAD_GPIO_PM3                13
#define PAD_GPIO_PM4                14
#define PAD_GPIO_PM5                15
#define PAD_GPIO_PM6                16
#define PAD_GPIO_PM7                17
#define PAD_GPIO_PM8                18
#define PAD_GPIO_PM9                19
#define PAD_GPIO_PM10               20
#define PAD_GPIO_PM11               21
#define PAD_GPIO_PM12               22
#define PAD_GPIO_PM13               23
#define PAD_GPIO_PM14               24
#define PAD_GPIO_PM15               25
#define PAD_GPIO_PM16               26
#define PAD_GPIO_PM17               27
#define PAD_GPIO_PM18               28
#define PAD_LED0                    29
#define PAD_LED1                    30
#define PAD_HOTPLUGB                31
#define PAD_HOTPLUGC                32
#define PAD_HOTPLUGD                33
#define PAD_HOTPLUGA_HDMI20_5V      34
#define PAD_HOTPLUGB_HDMI20_5V      35
#define PAD_HOTPLUGC_HDMI20_5V      36
#define PAD_HOTPLUGD_HDMI20_5V      37
#define PAD_DDCDA_CK                38
#define PAD_DDCDA_DA                39
#define PAD_DDCDB_CK                40
#define PAD_DDCDB_DA                41
#define PAD_DDCDC_CK                42
#define PAD_DDCDC_DA                43
#define PAD_DDCDD_CK                44
#define PAD_DDCDD_DA                45
#define PAD_SAR0                    46
#define PAD_SAR1                    47
#define PAD_SAR2                    48
#define PAD_SAR3                    49
#define PAD_SAR4                    50
#define PAD_VID0                    51
#define PAD_VID1                    52
#define PAD_WOL_INT                 53
#define PAD_DDCR_DA                 54
#define PAD_DDCR_CK                 55
#define PAD_GPIO0                   56
#define PAD_GPIO1                   57
#define PAD_GPIO2                   58
#define PAD_GPIO3                   59
#define PAD_GPIO4                   60
#define PAD_GPIO5                   61
#define PAD_GPIO6                   62
#define PAD_GPIO7                   63
#define PAD_GPIO8                   64
#define PAD_GPIO9                   65
#define PAD_GPIO10                  66
#define PAD_GPIO11                  67
#define PAD_GPIO12                  68
#define PAD_GPIO13                  69
#define PAD_GPIO14                  70
#define PAD_GPIO15                  71
#define PAD_GPIO16                  72
#define PAD_GPIO17                  73
#define PAD_GPIO18                  74
#define PAD_GPIO19                  75
#define PAD_GPIO20                  76
#define PAD_GPIO21                  77
#define PAD_GPIO22                  78
#define PAD_GPIO23                  79
#define PAD_GPIO24                  80
#define PAD_GPIO25                  81
#define PAD_GPIO26                  82
#define PAD_GPIO27                  83
#define PAD_GPIO28                  84
#define PAD_GPIO29                  85
#define PAD_GPIO30                  86
#define PAD_GPIO31                  87
#define PAD_GPIO32                  88
#define PAD_GPIO33                  89
#define PAD_GPIO34                  90
#define PAD_GPIO35                  91
#define PAD_GPIO36                  92
#define PAD_GPIO37                  93
#define PAD_I2S_IN_WS               94
#define PAD_I2S_IN_BCK              95
#define PAD_I2S_IN_SD               96
#define PAD_SPDIF_IN                97
#define PAD_SPDIF_OUT               98
#define PAD_I2S_OUT_WS              99
#define PAD_I2S_OUT_MCK             100
#define PAD_I2S_OUT_BCK             101
#define PAD_I2S_OUT_SD              102
#define PAD_I2S_OUT_SD1             103
#define PAD_I2S_OUT_SD2             104
#define PAD_I2S_OUT_SD3             105
#define PAD_VSYNC_LIKE              106
#define PAD_SPI1_CK                 107
#define PAD_SPI1_DI                 108
#define PAD_SPI2_CK                 109
#define PAD_SPI2_DI                 110
#define PAD_DIM0                    111
#define PAD_DIM1                    112
#define PAD_DIM2                    113
#define PAD_DIM3                    114
#define PAD_PCM2_CE_N               115
#define PAD_PCM2_IRQA_N             116
#define PAD_PCM2_WAIT_N             117
#define PAD_PCM2_RESET              118
#define PAD_PCM2_CD_N               119
#define PAD_PCM_D3                  120
#define PAD_PCM_D4                  121
#define PAD_PCM_D5                  122
#define PAD_PCM_D6                  123
#define PAD_PCM_D7                  124
#define PAD_PCM_CE_N                125
#define PAD_PCM_A10                 126
#define PAD_PCM_OE_N                127
#define PAD_PCM_A11                 128
#define PAD_PCM_IORD_N              129
#define PAD_PCM_A9                  130
#define PAD_PCM_IOWR_N              131
#define PAD_PCM_A8                  132
#define PAD_PCM_A13                 133
#define PAD_PCM_A14                 134
#define PAD_PCM_WE_N                135
#define PAD_PCM_IRQA_N              136
#define PAD_PCM_A12                 137
#define PAD_PCM_A7                  138
#define PAD_PCM_A6                  139
#define PAD_PCM_A5                  140
#define PAD_PCM_WAIT_N              141
#define PAD_PCM_A4                  142
#define PAD_PCM_A3                  143
#define PAD_PCM_A2                  144
#define PAD_PCM_REG_N               145
#define PAD_PCM_A1                  146
#define PAD_PCM_A0                  147
#define PAD_PCM_D0                  148
#define PAD_PCM_D1                  149
#define PAD_PCM_D2                  150
#define PAD_PCM_RESET               151
#define PAD_PCM_CD_N                152
#define PAD_PWM0                    153
#define PAD_PWM1                    154
#define PAD_PWM2                    155
#define PAD_PWM3                    156
#define PAD_PWM4                    157
#define PAD_TGPIO0                  158
#define PAD_TGPIO1                  159
#define PAD_TGPIO2                  160
#define PAD_TGPIO3                  161
#define PAD_TS0_D0                  162
#define PAD_TS0_D1                  163
#define PAD_TS0_D2                  164
#define PAD_TS0_D3                  165
#define PAD_TS0_D4                  166
#define PAD_TS0_D5                  167
#define PAD_TS0_D6                  168
#define PAD_TS0_D7                  169
#define PAD_TS0_VLD                 170
#define PAD_TS0_SYNC                171
#define PAD_TS0_CLK                 172
#define PAD_TS1_CLK                 173
#define PAD_TS1_SYNC                174
#define PAD_TS1_VLD                 175
#define PAD_TS1_D7                  176
#define PAD_TS1_D6                  177
#define PAD_TS1_D5                  178
#define PAD_TS1_D4                  179
#define PAD_TS1_D3                  180
#define PAD_TS1_D2                  181
#define PAD_TS1_D1                  182
#define PAD_TS1_D0                  183
#define PAD_EMMC_IO9                184
#define PAD_EMMC_IO12               185
#define PAD_EMMC_IO14               186
#define PAD_EMMC_IO10               187
#define PAD_EMMC_IO16               188
#define PAD_EMMC_IO17               189
#define PAD_EMMC_IO15               190
#define PAD_EMMC_IO11               191
#define PAD_EMMC_IO8                192
#define PAD_EMMC_IO4                193
#define PAD_EMMC_IO3                194
#define PAD_EMMC_IO0                195
#define PAD_TS2_D0                  196
#define PAD_TS2_VLD                 197
#define PAD_TS2_SYNC                198
#define PAD_TS2_CLK                 199
#define PAD_TS2_D1                  200
#define PAD_TS2_D2                  201
#define PAD_TS2_D3                  202
#define PAD_TS2_D4                  203
#define PAD_TS2_D5                  204
#define PAD_TS2_D6                  205
#define PAD_TS2_D7                  206
#define PAD_TS3_D0                  207
#define PAD_TS3_D1                  208
#define PAD_TS3_D2                  209
#define PAD_TS3_D3                  210
#define PAD_TS3_D4                  211
#define PAD_TS3_D5                  212
#define PAD_TS3_D6                  213
#define PAD_TS3_D7                  214
#define PAD_TS3_VLD                 215
#define PAD_TS3_SYNC                216
#define PAD_TS3_CLK                 217
#define PAD_EMMC_IO13               218
#define PAD_EMMC_IO1                219
#define PAD_EMMC_IO2                220
#define PAD_EMMC_IO7                221
#define PAD_EMMC_IO6                222
#define PAD_EMMC_IO5                223
#define PAD_ARC0                    224









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

