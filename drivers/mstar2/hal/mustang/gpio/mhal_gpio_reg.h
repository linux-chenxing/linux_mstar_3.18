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
#define GPIO_UNIT_NUM               257

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define REG_MIPS_BASE               0xFD000000      //Use 8 bit addressing

#define REG_ALL_PAD_IN              (0x101ea1)      //set all pads (except SPI) as input
#define REG_LVDS_BASE               (0x103200)
#define REG_LVDS_BANK               REG_LVDS_BASE

#define PAD_PM_SPI_CZ                               0
#define PAD_PM_SPI_CK                               1
#define PAD_PM_SPI_DI                               2
#define PAD_PM_SPI_DO                               3
#define PAD_PM_SPI_HOLDN                            4
#define PAD_PM_SPI_WPN                              5
#define PAD_VID0                                    6
#define PAD_VID1                                    7
#define PAD_IRIN                                    8
#define PAD_CEC                                     9
#define PAD_GPIO_PM0                                10
#define PAD_GPIO_PM1                                11
#define PAD_GPIO_PM2                                12
#define PAD_GPIO_PM3                                13
#define PAD_GPIO_PM4                                14
#define PAD_GPIO_PM5                                15
#define PAD_GPIO_PM6                                16
#define PAD_GPIO_PM7                                17
#define PAD_GPIO_PM8                                18
#define PAD_GPIO_PM9                                19
#define PAD_GPIO_PM10                               20
#define PAD_GPIO_PM11                               21
#define PAD_GPIO_PM12                               22
#define PAD_GPIO_PM13                               23
#define PAD_GPIO_PM14                               24
#define PAD_GPIO_PM15                               25
#define PAD_HOTPLUGA                                26
#define PAD_HOTPLUGB                                27
#define PAD_HOTPLUGC                                28
#define PAD_HOTPLUGD                                29
#define PAD_DDCDA_CK                                30
#define PAD_DDCDA_DA                                31
#define PAD_DDCDB_CK                                32
#define PAD_DDCDB_DA                                33
#define PAD_DDCDC_CK                                34
#define PAD_DDCDC_DA                                35
#define PAD_DDCDD_CK                                36
#define PAD_DDCDD_DA                                37
#define PAD_SAR0                                    38
#define PAD_SAR1                                    39
#define PAD_SAR2                                    40
#define PAD_SAR3                                    41
#define PAD_SAR4                                    42
#define PAD_PWM_PM                                  43
#define PAD_DDCA_CK                                 44
#define PAD_DDCA_DA                                 45
#define PAD_WOL_INT_OUT                             46
#define PAD_LED0                                    47
#define PAD_LED1                                    48
#define PAD_GPIO0                                   49
#define PAD_GPIO1                                   50
#define PAD_GPIO2                                   51
#define PAD_GPIO3                                   52
#define PAD_GPIO4                                   53
#define PAD_GPIO5                                   54
#define PAD_GPIO6                                   55
#define PAD_GPIO7                                   56
#define PAD_GPIO8                                   57
#define PAD_GPIO9                                   58
#define PAD_GPIO10                                  59
#define PAD_GPIO11                                  60
#define PAD_GPIO12                                  61
#define PAD_GPIO13                                  62
#define PAD_GPIO14                                  63
#define PAD_GPIO15                                  64
#define PAD_GPIO16                                  65
#define PAD_GPIO17                                  66
#define PAD_GPIO18                                  67
#define PAD_GPIO19                                  68
#define PAD_GPIO20                                  69
#define PAD_GPIO21                                  70
#define PAD_GPIO22                                  71
#define PAD_GPIO23                                  72
#define PAD_GPIO24                                  73
#define PAD_GPIO25                                  74
#define PAD_GPIO26                                  75
#define PAD_GPIO27                                  76
#define PAD_UART_RX2                                77
#define PAD_UART_TX2                                78
#define PAD_PWM0                                    79
#define PAD_PWM1                                    80
#define PAD_PWM2                                    81
#define PAD_PWM3                                    82
#define PAD_PWM4                                    83
#define PAD_DDCR_DA                                 84
#define PAD_DDCR_CK                                 85
#define PAD_TGPIO0                                  86
#define PAD_TGPIO1                                  87
#define PAD_TGPIO2                                  88
#define PAD_TGPIO3                                  89
#define PAD_TS0_D0                                  90
#define PAD_TS0_D1                                  91
#define PAD_TS0_D2                                  92
#define PAD_TS0_D3                                  93
#define PAD_TS0_D4                                  94
#define PAD_TS0_D5                                  95
#define PAD_TS0_D6                                  96
#define PAD_TS0_D7                                  97
#define PAD_TS0_VLD                                 98
#define PAD_TS0_SYNC                                99
#define PAD_TS0_CLK                                 100
#define PAD_TS1_D0                                  101
#define PAD_TS1_D1                                  102
#define PAD_TS1_D2                                  103
#define PAD_TS1_D3                                  104
#define PAD_TS1_D4                                  105
#define PAD_TS1_D5                                  106
#define PAD_TS1_D6                                  107
#define PAD_TS1_D7                                  108
#define PAD_TS1_VLD                                 109
#define PAD_TS1_SYNC                                110
#define PAD_TS1_CLK                                 111
#define PAD_PCM_A4                                  112
#define PAD_PCM_WAIT_N                              113
#define PAD_PCM_A5                                  114
#define PAD_PCM_A6                                  115
#define PAD_PCM_A7                                  116
#define PAD_PCM_A12                                 117
#define PAD_PCM_IRQA_N                              118
#define PAD_PCM_A14                                 119
#define PAD_PCM_A13                                 120
#define PAD_PCM_A8                                  121
#define PAD_PCM_IOWR_N                              122
#define PAD_PCM_A9                                  123
#define PAD_PCM_IORD_N                              124
#define PAD_PCM_A11                                 125
#define PAD_PCM_OE_N                                126
#define PAD_PCM_A10                                 127
#define PAD_PCM_CE_N                                128
#define PAD_PCM_D7                                  129
#define PAD_PCM_D6                                  130
#define PAD_PCM_D5                                  131
#define PAD_PCM_D4                                  132
#define PAD_PCM_D3                                  133
#define PAD_PCM_A3                                  134
#define PAD_PCM_A2                                  135
#define PAD_PCM_REG_N                               136
#define PAD_PCM_A1                                  137
#define PAD_PCM_A0                                  138
#define PAD_PCM_D0                                  139
#define PAD_PCM_D1                                  140
#define PAD_PCM_D2                                  141
#define PAD_PCM_RESET                               142
#define PAD_PCM_CD_N                                143
#define PAD_PCM2_CE_N                               144
#define PAD_PCM2_IRQA_N                             145
#define PAD_PCM2_WAIT_N                             146
#define PAD_PCM2_RESET                              147
#define PAD_PCM2_CD_N                               148
#define PAD_EMMC_IO0                                149
#define PAD_EMMC_IO1                                150
#define PAD_EMMC_IO2                                151
#define PAD_EMMC_IO3                                152
#define PAD_EMMC_IO4                                153
#define PAD_EMMC_IO5                                154
#define PAD_EMMC_IO6                                155
#define PAD_EMMC_IO7                                156
#define PAD_EMMC_IO8                                157
#define PAD_EMMC_IO9                                158
#define PAD_EMMC_IO10                               159
#define PAD_EMMC_IO11                               160
#define PAD_EMMC_IO12                               161
#define PAD_EMMC_IO13                               162
#define PAD_EMMC_IO14                               163
#define PAD_EMMC_IO15                               164
#define PAD_EMMC_IO16                               165
#define PAD_EMMC_IO17                               166
#define PAD_VSYNC_LIKE                              167
#define PAD_I2S_IN_WS                               168
#define PAD_I2S_IN_BCK                              169
#define PAD_I2S_IN_SD                               170
#define PAD_SPDIF_IN                                171
#define PAD_SPDIF_OUT                               172
#define PAD_I2S_OUT_MCK                             173
#define PAD_I2S_OUT_WS                              174
#define PAD_I2S_OUT_BCK                             175
#define PAD_I2S_OUT_SD                              176
#define PAD_I2S_OUT_SD1                             177
#define PAD_I2S_OUT_SD2                             178
#define PAD_I2S_OUT_SD3                             179
#define PAD_B_ODD0                                  180
#define PAD_B_ODD1                                  181
#define PAD_B_ODD2                                  182
#define PAD_B_ODD3                                  183
#define PAD_B_ODD4                                  184
#define PAD_B_ODD5                                  185
#define PAD_B_ODD6                                  186
#define PAD_B_ODD7                                  187
#define PAD_G_ODD0                                  188
#define PAD_G_ODD1                                  189
#define PAD_G_ODD2                                  190
#define PAD_G_ODD3                                  191
#define PAD_G_ODD4                                  192
#define PAD_G_ODD5                                  193
#define PAD_G_ODD6                                  194
#define PAD_G_ODD7                                  195
#define PAD_R_ODD0                                  196
#define PAD_R_ODD1                                  197
#define PAD_R_ODD2                                  198
#define PAD_R_ODD3                                  199
#define PAD_R_ODD4                                  200
#define PAD_R_ODD5                                  201
#define PAD_R_ODD6                                  202
#define PAD_R_ODD7                                  203
#define PAD_LCK                                     204
#define PAD_LDE                                     205
#define PAD_LHSYNC                                  206
#define PAD_LVSYNC                                  207
#define PAD_PCM_WE_N                                208
#define PAD_SPI1_CK                                 209
#define PAD_SPI1_DI                                 210
#define PAD_SPI2_CK                                 211
#define PAD_SPI2_DI                                 212
#define PAD_TCON0                                   213
#define PAD_TCON1                                   214
#define PAD_TCON2                                   215
#define PAD_TCON3                                   216
#define PAD_TCON4                                   217
#define PAD_TCON5                                   218
#define PAD_TCON6                                   219
#define PAD_TCON7                                   220
#define PAD_TS2_D0                                  221
#define PAD_TS2_D1                                  222
#define PAD_TS2_D2                                  223
#define PAD_TS2_D3                                  224
#define PAD_TS2_D4                                  225
#define PAD_TS2_D5                                  226
#define PAD_TS2_D6                                  227
#define PAD_TS2_D7                                  228
#define PAD_TS2_CLK                                 229
#define PAD_TS2_SYNC                                230
#define PAD_TS2_VLD                                 231
#define PAD_LINEIN_L1                               232
#define PAD_LINEIN_R1                               233
#define PAD_LINEIN_L2                               234
#define PAD_LINEIN_R2                               235
#define PAD_LINEIN_L3                               236
#define PAD_LINEIN_R3                               237
#define PAD_LINEIN_L4                               238
#define PAD_LINEIN_R4                               239
#define PAD_LINEIN_L5                               240
#define PAD_LINEIN_R5                               241
#define PADA_HSYNC0                                 242
#define PADA_VSYNC0                                 243
#define PADA_HSYNC1                                 244
#define PADA_VSYNC1                                 245
#define PADA_HSYNC2                                 246
#define PADA_VSYNC2                                 247
#define PADA_RIN0P                                  248
#define PADA_RIN1P                                  249
#define PADA_GIN0P                                  250
#define PADA_GIN1P                                  251
#define PADA_BIN0P                                  252
#define PADA_BIN1P                                  253
#define PADA_GIN0M                                  254
#define PADA_GIN1M                                  255
#define PAD_ARC0                                    256

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

