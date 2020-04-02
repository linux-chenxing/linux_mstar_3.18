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
#define PAD_VID0                    50
#define PAD_VID1                    51
#define PAD_WOL_INT                 52
#define PAD_DDCR_DA                 53
#define PAD_DDCR_CK                 54
#define PAD_GPIO0                   55
#define PAD_GPIO1                   56
#define PAD_GPIO2                   57
#define PAD_GPIO3                   58
#define PAD_GPIO4                   59
#define PAD_GPIO5                   60
#define PAD_GPIO6                   61
#define PAD_GPIO7                   62
#define PAD_GPIO8                   63
#define PAD_GPIO9                   64
#define PAD_GPIO10                  65
#define PAD_GPIO11                  66
#define PAD_GPIO12                  67
#define PAD_GPIO13                  68
#define PAD_GPIO14                  69
#define PAD_GPIO15                  70
#define PAD_GPIO16                  71
#define PAD_GPIO17                  72
#define PAD_GPIO18                  73
#define PAD_GPIO19                  74
#define PAD_GPIO20                  75
#define PAD_GPIO21                  76
#define PAD_GPIO22                  77
#define PAD_GPIO23                  78
#define PAD_GPIO24                  79
#define PAD_GPIO25                  80
#define PAD_GPIO26                  81
#define PAD_GPIO27                  82
#define PAD_GPIO28                  83
#define PAD_GPIO29                  84
#define PAD_GPIO30                  85
#define PAD_GPIO31                  86
#define PAD_GPIO32                  87
#define PAD_GPIO33                  88
#define PAD_GPIO34                  89
#define PAD_GPIO35                  90
#define PAD_GPIO36                  91
#define PAD_GPIO37                  92
#define PAD_I2S_IN_WS               93
#define PAD_I2S_IN_BCK              94
#define PAD_I2S_IN_SD               95
#define PAD_SPDIF_IN                96
#define PAD_SPDIF_OUT               97
#define PAD_I2S_OUT_WS              98
#define PAD_I2S_OUT_MCK             99
#define PAD_I2S_OUT_BCK             100
#define PAD_I2S_OUT_SD              101
#define PAD_I2S_OUT_SD1             102
#define PAD_I2S_OUT_SD2             103
#define PAD_I2S_OUT_SD3             104
#define PAD_VSYNC_LIKE              105
#define PAD_SPI1_CK                 106
#define PAD_SPI1_DI                 107
#define PAD_SPI2_CK                 108
#define PAD_SPI2_DI                 109
#define PAD_DIM0                    110
#define PAD_DIM1                    111
#define PAD_DIM2                    112
#define PAD_DIM3                    113
#define PAD_PCM2_CE_N               114
#define PAD_PCM2_IRQA_N             115
#define PAD_PCM2_WAIT_N             116
#define PAD_PCM2_RESET              117
#define PAD_PCM2_CD_N               118
#define PAD_PCM_D3                  119
#define PAD_PCM_D4                  120
#define PAD_PCM_D5                  121
#define PAD_PCM_D6                  122
#define PAD_PCM_D7                  123
#define PAD_PCM_CE_N                124
#define PAD_PCM_A10                 125
#define PAD_PCM_OE_N                126
#define PAD_PCM_A11                 127
#define PAD_PCM_IORD_N              128
#define PAD_PCM_A9                  129
#define PAD_PCM_IOWR_N              130
#define PAD_PCM_A8                  131
#define PAD_PCM_A13                 132
#define PAD_PCM_A14                 133
#define PAD_PCM_WE_N                134
#define PAD_PCM_IRQA_N              135
#define PAD_PCM_A12                 136
#define PAD_PCM_A7                  137
#define PAD_PCM_A6                  138
#define PAD_PCM_A5                  139
#define PAD_PCM_WAIT_N              140
#define PAD_PCM_A4                  141
#define PAD_PCM_A3                  142
#define PAD_PCM_A2                  143
#define PAD_PCM_REG_N               144
#define PAD_PCM_A1                  145
#define PAD_PCM_A0                  146
#define PAD_PCM_D0                  147
#define PAD_PCM_D1                  148
#define PAD_PCM_D2                  149
#define PAD_PCM_RESET               150
#define PAD_PCM_CD_N                151
#define PAD_PWM0                    152
#define PAD_PWM1                    153
#define PAD_PWM2                    154
#define PAD_PWM3                    155
#define PAD_PWM4                    156
#define PAD_TGPIO0                  157
#define PAD_TGPIO1                  158
#define PAD_TGPIO2                  159
#define PAD_TGPIO3                  160
#define PAD_TS0_D0                  161
#define PAD_TS0_D1                  162
#define PAD_TS0_D2                  163
#define PAD_TS0_D3                  164
#define PAD_TS0_D4                  165
#define PAD_TS0_D5                  166
#define PAD_TS0_D6                  167
#define PAD_TS0_D7                  168
#define PAD_TS0_VLD                 169
#define PAD_TS0_SYNC                170
#define PAD_TS0_CLK                 171
#define PAD_TS1_CLK                 172
#define PAD_TS1_SYNC                173
#define PAD_TS1_VLD                 174
#define PAD_TS1_D7                  175
#define PAD_TS1_D6                  176
#define PAD_TS1_D5                  177
#define PAD_TS1_D4                  178
#define PAD_TS1_D3                  179
#define PAD_TS1_D2                  180
#define PAD_TS1_D1                  181
#define PAD_TS1_D0                  182
#define PAD_EMMC_IO9                183
#define PAD_EMMC_IO12               184
#define PAD_EMMC_IO14               185
#define PAD_EMMC_IO10               186
#define PAD_EMMC_IO16               187
#define PAD_EMMC_IO17               188
#define PAD_EMMC_IO15               189
#define PAD_EMMC_IO11               190
#define PAD_EMMC_IO8                191
#define PAD_EMMC_IO4                192
#define PAD_EMMC_IO3                193
#define PAD_EMMC_IO0                194
#define PAD_TS2_D0                  195
#define PAD_TS2_VLD                 196
#define PAD_TS2_SYNC                197
#define PAD_TS2_CLK                 198
#define PAD_TS2_D1                  199
#define PAD_TS2_D2                  200
#define PAD_TS2_D3                  201
#define PAD_TS2_D4                  202
#define PAD_TS2_D5                  203
#define PAD_TS2_D6                  204
#define PAD_TS2_D7                  205
#define PAD_TS3_D0                  206
#define PAD_TS3_D1                  207
#define PAD_TS3_D2                  208
#define PAD_TS3_D3                  209
#define PAD_TS3_D4                  210
#define PAD_TS3_D5                  211
#define PAD_TS3_D6                  212
#define PAD_TS3_D7                  213
#define PAD_TS3_VLD                 214
#define PAD_TS3_SYNC                215
#define PAD_TS3_CLK                 216
#define PAD_EMMC_IO13               217
#define PAD_EMMC_IO1                218
#define PAD_EMMC_IO2                219
#define PAD_EMMC_IO7                220
#define PAD_EMMC_IO6                221
#define PAD_EMMC_IO5                222
#define PAD_ARC0                    223









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

