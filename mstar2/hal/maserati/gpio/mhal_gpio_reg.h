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
#define GPIO_UNIT_NUM               218

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

#define PAD_PM_SPI_CZ	0
#define PAD_PM_SPI_CK	1
#define PAD_PM_SPI_DI	2
#define PAD_PM_SPI_DO	3
#define PAD_IRIN	4
#define PAD_CEC0	5
#define PAD_PWM_PM	6
#define PAD_DDCA_CK	7
#define PAD_DDCA_DA	8
#define PAD_GPIO_PM0	9
#define PAD_GPIO_PM1	10
#define PAD_GPIO_PM2	11
#define PAD_GPIO_PM3	12
#define PAD_GPIO_PM4	13
#define PAD_GPIO_PM5	14
#define PAD_GPIO_PM6	15
#define PAD_GPIO_PM9	16
#define PAD_GPIO_PM10	17
#define PAD_GPIO_PM11	18
#define PAD_GPIO_PM12	19
#define PAD_GPIO_PM13	20
#define PAD_GPIO_PM14	21
#define PAD_GPIO_PM15	22
#define PAD_GPIO_PM16	23
#define PAD_LED0	24
#define PAD_LED1	25
#define PAD_HOTPLUGB	26
#define PAD_HOTPLUGC	27
#define PAD_HOTPLUGD	28
#define PAD_HOTPLUGA_HDMI20_5V	29
#define PAD_HOTPLUGB_HDMI20_5V	30
#define PAD_HOTPLUGC_HDMI20_5V	31
#define PAD_HOTPLUGD_HDMI20_5V	32
#define PAD_DDCDA_CK	33
#define PAD_DDCDA_DA	34
#define PAD_DDCDB_CK	35
#define PAD_DDCDB_DA	36
#define PAD_DDCDC_CK	37
#define PAD_DDCDC_DA	38
#define PAD_DDCDD_CK	39
#define PAD_DDCDD_DA	40
#define PAD_SAR0	41
#define PAD_SAR1	42
#define PAD_SAR2	43
#define PAD_SAR3	44
#define PAD_SAR4	45
#define PAD_VPLGUIN	46
#define PAD_VID0	47
#define PAD_VID1	48
#define PAD_VID2	49
#define PAD_VID3	50
#define PAD_VID4	51
#define PAD_VID5	52
#define PAD_WOL_INT_OUT	53
#define PAD_DDCR_DA	54
#define PAD_DDCR_CK	55
#define PAD_GPIO0	56
#define PAD_GPIO1	57
#define PAD_GPIO2	58
#define PAD_GPIO3	59
#define PAD_GPIO4	60
#define PAD_GPIO5	61
#define PAD_GPIO6	62
#define PAD_GPIO7	63
#define PAD_GPIO8	64
#define PAD_GPIO16	65
#define PAD_GPIO17	66
#define PAD_GPIO18	67
#define PAD_GPIO19	68
#define PAD_GPIO20	69
#define PAD_GPIO21	70
#define PAD_GPIO22	71
#define PAD_GPIO23	72
#define PAD_GPIO24	73
#define PAD_GPIO25	74
#define PAD_GPIO26	75
#define PAD_GPIO27	76
#define PAD_GPIO28	77
#define PAD_GPIO29	78
#define PAD_GPIO30	79
#define PAD_GPIO31	80
#define PAD_GPIO32	81
#define PAD_GPIO33	82
#define PAD_GPIO34	83
#define PAD_GPIO35	84
#define PAD_GPIO36	85
#define PAD_GPIO37	86
#define PAD_I2S_IN_WS	87
#define PAD_I2S_IN_BCK	88
#define PAD_I2S_IN_SD	89
#define PAD_SPDIF_IN	90
#define PAD_SPDIF_OUT	91
#define PAD_I2S_OUT_WS	92
#define PAD_I2S_OUT_MCK	93
#define PAD_I2S_OUT_BCK	94
#define PAD_I2S_OUT_SD	95
#define PAD_I2S_OUT_SD1	96
#define PAD_I2S_OUT_SD2	97
#define PAD_I2S_OUT_SD3	98
#define PAD_VSYNC_LIKE	99
#define PAD_SPI1_CK	100
#define PAD_SPI1_DI	101
#define PAD_SPI2_CK	102
#define PAD_SPI2_DI	103
#define PAD_DIM0	104
#define PAD_DIM1	105
#define PAD_DIM2	106
#define PAD_DIM3	107
#define PAD_PCM2_CE_N	108
#define PAD_PCM2_IRQA_N	109
#define PAD_PCM2_WAIT_N	110
#define PAD_PCM2_RESET	111
#define PAD_PCM2_CD_N	112
#define PAD_PCM_D3	113
#define PAD_PCM_D4	114
#define PAD_PCM_D5	115
#define PAD_PCM_D6	116
#define PAD_PCM_D7	117
#define PAD_PCM_CE_N	118
#define PAD_PCM_A10	119
#define PAD_PCM_OE_N	120
#define PAD_PCM_A11	121
#define PAD_PCM_IORD_N	122
#define PAD_PCM_A9	123
#define PAD_PCM_IOWR_N	124
#define PAD_PCM_A8	125
#define PAD_PCM_A13	126
#define PAD_PCM_A14	127
#define PAD_PCM_WE_N	128
#define PAD_PCM_IRQA_N	129
#define PAD_PCM_A12	130
#define PAD_PCM_A7	131
#define PAD_PCM_A6	132
#define PAD_PCM_A5	133
#define PAD_PCM_WAIT_N	134
#define PAD_PCM_A4	135
#define PAD_PCM_A3	136
#define PAD_PCM_A2	137
#define PAD_PCM_REG_N	138
#define PAD_PCM_A1	139
#define PAD_PCM_A0	140
#define PAD_PCM_D0	141
#define PAD_PCM_D1	142
#define PAD_PCM_D2	143
#define PAD_PCM_RESET	144
#define PAD_PCM_CD_N	145
#define PAD_PWM0	146
#define PAD_PWM1	147
#define PAD_PWM2	148
#define PAD_PWM3	149
#define PAD_PWM4	150
#define PAD_TGPIO0	151
#define PAD_TGPIO1	152
#define PAD_TGPIO2	153
#define PAD_TGPIO3	154
#define PAD_TS0_D0	155
#define PAD_TS0_D1	156
#define PAD_TS0_D2	157
#define PAD_TS0_D3	158
#define PAD_TS0_D4	159
#define PAD_TS0_D5	160
#define PAD_TS0_D6	161
#define PAD_TS0_D7	162
#define PAD_TS0_VLD	163
#define PAD_TS0_SYNC	164
#define PAD_TS0_CLK	165
#define PAD_TS1_CLK	166
#define PAD_TS1_SYNC	167
#define PAD_TS1_VLD	168
#define PAD_TS1_D7	169
#define PAD_TS1_D6	170
#define PAD_TS1_D5	171
#define PAD_TS1_D4	172
#define PAD_TS1_D3	173
#define PAD_TS1_D2	174
#define PAD_TS1_D1	175
#define PAD_TS1_D0	176
#define PAD_EMMC_IO9	177
#define PAD_EMMC_IO12	178
#define PAD_EMMC_IO14	179
#define PAD_EMMC_IO10	180
#define PAD_EMMC_IO16	181
#define PAD_EMMC_IO17	182
#define PAD_EMMC_IO15	183
#define PAD_EMMC_IO11	184
#define PAD_EMMC_IO8	185
#define PAD_EMMC_IO4	186
#define PAD_EMMC_IO3	187
#define PAD_EMMC_IO0	188
#define PAD_TS2_D0	189
#define PAD_TS2_VLD	190
#define PAD_TS2_SYNC	191
#define PAD_TS2_CLK	192
#define PAD_TS2_D1	193
#define PAD_TS2_D2	194
#define PAD_TS2_D3	195
#define PAD_TS2_D4	196
#define PAD_TS2_D5	197
#define PAD_TS2_D6	198
#define PAD_TS2_D7	199
#define PAD_TS3_D0	200
#define PAD_TS3_D1	201
#define PAD_TS3_D2	202
#define PAD_TS3_D3	203
#define PAD_TS3_D4	204
#define PAD_TS3_D5	205
#define PAD_TS3_D6	206
#define PAD_TS3_D7	207
#define PAD_TS3_VLD	208
#define PAD_TS3_SYNC	209
#define PAD_TS3_CLK	210
#define PAD_EMMC_IO13	211
#define PAD_EMMC_IO1	212
#define PAD_EMMC_IO2	213
#define PAD_EMMC_IO7	214
#define PAD_EMMC_IO6	215
#define PAD_EMMC_IO5	216
#define PAD_ARC0	217










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

