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
#define GPIO_UNIT_NUM               198

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
#define PAD_IRIN                                    4
#define PAD_CEC0                                    5
#define PAD_AV_LNK                                  6
#define PAD_PWM_PM                                  7
#define PAD_DDCA_CK                                 8
#define PAD_DDCA_DA                                 9
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
#define PAD_LED0                                    26
#define PAD_LED1                                    27
#define PAD_HOTPLUGA                                28
#define PAD_HOTPLUGB                                29
#define PAD_HOTPLUGC                                30
#define PAD_HOTPLUGD                                31
#define PAD_HOTPLUGA_HDMI20_5V                      32
#define PAD_HOTPLUGB_HDMI20_5V                      33
#define PAD_HOTPLUGC_HDMI20_5V                      34
#define PAD_HOTPLUGD_HDMI20_5V                      35
#define PAD_DDCDA_CK                                36
#define PAD_DDCDA_DA                                37
#define PAD_DDCDB_CK                                38
#define PAD_DDCDB_DA                                39
#define PAD_DDCDC_CK                                40
#define PAD_DDCDC_DA                                41
#define PAD_DDCDD_CK                                42
#define PAD_DDCDD_DA                                43
#define PAD_SAR0                                    44
#define PAD_SAR1                                    45
#define PAD_SAR2                                    46
#define PAD_SAR3                                    47
#define PAD_SAR4                                    48
#define PAD_VID0                                    49
#define PAD_VID1                                    50
#define PAD_VID2                                    51
#define PAD_VID3                                    52
#define PAD_WOL_INT_OUT                             53
#define PAD_ARC0                                    54
#define PAD_DDCR_DA                                 55
#define PAD_DDCR_CK                                 56
#define PAD_GPIO0                                   57
#define PAD_GPIO1                                   58
#define PAD_GPIO2                                   59
#define PAD_GPIO3                                   60
#define PAD_GPIO4                                   61
#define PAD_GPIO5                                   62
#define PAD_GPIO6                                   63
#define PAD_GPIO7                                   64
#define PAD_GPIO8                                   65
#define PAD_GPIO9                                   66
#define PAD_GPIO10                                  67
#define PAD_GPIO11                                  68
#define PAD_GPIO12                                  69
#define PAD_GPIO13                                  70
#define PAD_GPIO14                                  71
#define PAD_GPIO15                                  72
#define PAD_GPIO16                                  73
#define PAD_GPIO17                                  74
#define PAD_GPIO18                                  75
#define PAD_GPIO19                                  76
#define PAD_GPIO20                                  77
#define PAD_GPIO21                                  78
#define PAD_GPIO22                                  79
#define PAD_GPIO23                                  80
#define PAD_GPIO24                                  81
#define PAD_GPIO25                                  82
#define PAD_GPIO26                                  83
#define PAD_GPIO27                                  84
#define PAD_GPIO28                                  85
#define PAD_GPIO29                                  86
#define PAD_GPIO30                                  87
#define PAD_GPIO31                                  88
#define PAD_GPIO32                                  89
#define PAD_GPIO33                                  90
#define PAD_GPIO34                                  91
#define PAD_GPIO35                                  92
#define PAD_GPIO36                                  93
#define PAD_GPIO37                                  94
#define PAD_I2S_IN_WS                               95
#define PAD_I2S_IN_BCK                              96
#define PAD_I2S_IN_SD                               97
#define PAD_SPDIF_IN                                98
#define PAD_SPDIF_OUT                               99
#define PAD_I2S_OUT_WS                              100
#define PAD_I2S_OUT_MCK                             101
#define PAD_I2S_OUT_BCK                             102
#define PAD_I2S_OUT_SD                              103
#define PAD_I2S_OUT_SD1                             104
#define PAD_I2S_OUT_SD2                             105
#define PAD_I2S_OUT_SD3                             106
#define PAD_EMMC_IO9                                107
#define PAD_EMMC_IO12                               108
#define PAD_EMMC_IO14                               109
#define PAD_EMMC_IO10                               110
#define PAD_EMMC_IO16                               111
#define PAD_EMMC_IO17                               112
#define PAD_EMMC_IO15                               113
#define PAD_EMMC_IO11                               114
#define PAD_EMMC_IO8                                115
#define PAD_EMMC_IO13                               116
#define PAD_EMMC_IO1                                117
#define PAD_EMMC_IO2                                118
#define PAD_EMMC_IO7                                119
#define PAD_EMMC_IO6                                120
#define PAD_EMMC_IO5                                121
#define PAD_PCM2_CE_N                               122
#define PAD_PCM2_IRQA_N                             123
#define PAD_PCM2_WAIT_N                             124
#define PAD_PCM2_RESET                              125
#define PAD_PCM2_CD_N                               126
#define PAD_PCM_D3                                  127
#define PAD_PCM_D4                                  128
#define PAD_PCM_D5                                  129
#define PAD_PCM_D6                                  130
#define PAD_PCM_D7                                  131
#define PAD_PCM_CE_N                                132
#define PAD_PCM_A10                                 133
#define PAD_PCM_OE_N                                134
#define PAD_PCM_A11                                 135
#define PAD_PCM_IORD_N                              136
#define PAD_PCM_A9                                  137
#define PAD_PCM_IOWR_N                              138
#define PAD_PCM_A8                                  139
#define PAD_PCM_A13                                 140
#define PAD_PCM_A14                                 141
#define PAD_PCM_WE_N                                142
#define PAD_PCM_IRQA_N                              143
#define PAD_PCM_A12                                 144
#define PAD_PCM_A7                                  145
#define PAD_PCM_A6                                  146
#define PAD_PCM_A5                                  147
#define PAD_PCM_WAIT_N                              148
#define PAD_PCM_A4                                  149
#define PAD_PCM_A3                                  150
#define PAD_PCM_A2                                  151
#define PAD_PCM_REG_N                               152
#define PAD_PCM_A1                                  153
#define PAD_PCM_A0                                  154
#define PAD_PCM_D0                                  155
#define PAD_PCM_D1                                  156
#define PAD_PCM_D2                                  157
#define PAD_PCM_RESET                               158
#define PAD_PCM_CD_N                                159
#define PAD_PWM0                                    160
#define PAD_PWM1                                    161
#define PAD_PWM2                                    162
#define PAD_PWM3                                    163
#define PAD_PWM4                                    164
#define PAD_EMMC_IO4                                165
#define PAD_EMMC_IO3                                166
#define PAD_EMMC_IO0                                167
#define PAD_TGPIO0                                  168
#define PAD_TGPIO1                                  169
#define PAD_TGPIO2                                  170
#define PAD_TGPIO3                                  171
#define PAD_TS0_D0                                  172
#define PAD_TS0_D1                                  173
#define PAD_TS0_D2                                  174
#define PAD_TS0_D3                                  175
#define PAD_TS0_D4                                  176
#define PAD_TS0_D5                                  177
#define PAD_TS0_D6                                  178
#define PAD_TS0_D7                                  179
#define PAD_TS0_VLD                                 180
#define PAD_TS0_SYNC                                181
#define PAD_TS0_CLK                                 182
#define PAD_TS1_CLK                                 183
#define PAD_TS1_SYNC                                184
#define PAD_TS1_VLD                                 185
#define PAD_TS1_D7                                  186
#define PAD_TS1_D6                                  187
#define PAD_TS1_D5                                  188
#define PAD_TS1_D4                                  189
#define PAD_TS1_D3                                  190
#define PAD_TS1_D2                                  191
#define PAD_TS1_D1                                  192
#define PAD_TS1_D0                                  193
#define PAD_TS2_D0                                  194
#define PAD_TS2_VLD                                 195
#define PAD_TS2_SYNC                                196
#define PAD_TS2_CLK                                 197

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

