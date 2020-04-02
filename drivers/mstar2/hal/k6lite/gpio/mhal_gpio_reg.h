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

#define  PAD_PM_IRIN                0
#define  PAD_PM_SPI_CZ              1
#define  PAD_PM_SPI_CK              2
#define  PAD_PM_SPI_DI              3
#define  PAD_PM_SPI_DO              4
#define  PAD_PM_SPI_WPZ             5
#define  PAD_PM_SPI_HOLDZ           6
#define  PAD_PM_SPI_RSTZ            7
#define  PAD_PM_GPIO0               8
#define  PAD_PM_GPIO1               9
#define  PAD_PM_GPIO2               10
#define  PAD_PM_GPIO3               11
#define  PAD_PM_GPIO4               12
#define  PAD_PM_GPIO5               13
#define  PAD_PM_GPIO6               14
#define  PAD_PM_GPIO7               15
#define  PAD_PM_GPIO8               16
#define  PAD_PM_GPIO9               17
#define  PAD_PM_GPIO10              18
#define  PAD_PM_GPIO11              19
#define  PAD_PM_GPIO12              20
#define  PAD_PM_GPIO13              21
#define  PAD_PM_GPIO14              22
#define  PAD_PM_GPIO15              23
#define  PAD_PM_CEC                 24
#define  PAD_HDMITX_HPD             25
#define  PAD_PM_SD_CDZ              26
#define  PAD_VID0                   27
#define  PAD_VID1                   28
#define  PAD_PM_GT0_MDIO            29
#define  PAD_PM_GT0_MDC             30
#define  PAD_PM_GT0_RX_CLK          31
#define  PAD_PM_GT0_RX_CTL          32
#define  PAD_PM_GT0_RX_D0           33
#define  PAD_PM_GT0_RX_D1           34
#define  PAD_PM_GT0_RX_D2           35
#define  PAD_PM_GT0_RX_D3           36
#define  PAD_PM_GT0_TX_CLK          37
#define  PAD_PM_GT0_TX_CTL          38
#define  PAD_PM_GT0_TX_D0           39
#define  PAD_PM_GT0_TX_D1           40
#define  PAD_PM_GT0_TX_D2           41
#define  PAD_PM_GT0_TX_D3           42
#define  PAD_PM_LED0                43
#define  PAD_PM_LED1                44
#define  PAD_SAR_GPIO0              45
#define  PAD_SAR_GPIO1              46
#define  PAD_SAR_GPIO2              47
#define  PAD_SAR_GPIO3              48
#define  PAD_SAR_GPIO4              49
#define  PAD_VPLUG_DET              50
#define  PAD_HSYNC_OUT              51
#define  PAD_VSYNC_OUT              52
#define  PAD_SD_CLK                 53
#define  PAD_SD_CMD                 54
#define  PAD_SD_D0                  55
#define  PAD_SD_D1                  56
#define  PAD_SD_D2                  57
#define  PAD_SD_D3                  58
#define  PAD_GPIO0                  59
#define  PAD_GPIO1                  60
#define  PAD_GPIO2                  61
#define  PAD_GPIO3                  62
#define  PAD_GPIO4                  63
#define  PAD_GPIO5                  64
#define  PAD_GPIO6                  65
#define  PAD_GPIO7                  66
#define  PAD_GPIO8                  67
#define  PAD_GPIO9                  68
#define  PAD_SM0_CLK                69
#define  PAD_SM0_RST                70
#define  PAD_SM0_VCC                71
#define  PAD_SM0_CD                 72
#define  PAD_SM0_IO                 73
#define  PAD_SM0_VSEL               74
#define  PAD_SM0_C4                 75
#define  PAD_SM0_C8                 76
#define  PAD_CI_RST                 77
#define  PAD_CI_WAITZ               78
#define  PAD_CI_IRQAZ               79
#define  PAD_CI_IORDZ               80
#define  PAD_CI_REGZ                81
#define  PAD_CI_WEZ                 82
#define  PAD_CI_IOWRZ               83
#define  PAD_CI_CEZ                 84
#define  PAD_CI_OEZ                 85
#define  PAD_CI_CDZ                 86
#define  PAD_CI_A0                  87
#define  PAD_CI_A1                  88
#define  PAD_CI_A2                  89
#define  PAD_CI_A3                  90
#define  PAD_CI_A4                  91
#define  PAD_CI_A5                  92
#define  PAD_CI_A6                  93
#define  PAD_CI_A7                  94
#define  PAD_CI_A8                  95
#define  PAD_CI_A9                  96
#define  PAD_CI_A10                 97
#define  PAD_CI_A11                 98
#define  PAD_CI_A12                 99
#define  PAD_CI_A13                 100
#define  PAD_CI_A14                 101
#define  PAD_CI_D0                  102
#define  PAD_CI_D1                  103
#define  PAD_CI_D2                  104
#define  PAD_CI_D3                  105
#define  PAD_CI_D4                  106
#define  PAD_CI_D5                  107
#define  PAD_CI_D6                  108
#define  PAD_CI_D7                  109
#define  PAD_GPIO119                110
#define  PAD_TS0_SYNC               111
#define  PAD_GPIO120                112
#define  PAD_GPIO121                113
#define  PAD_TS0_D1                 114
#define  PAD_TS0_D2                 115
#define  PAD_TS0_D3                 116
#define  PAD_GPIO125                117
#define  PAD_GPIO126                118
#define  PAD_TS0_D6                 119
#define  PAD_TS0_D7                 120
#define  PAD_TS1_CLK                121
#define  PAD_TS1_SYNC               122
#define  PAD_TS1_VLD                123
#define  PAD_TS1_D0                 124
#define  PAD_TS1_D1                 125
#define  PAD_TS1_D2                 126
#define  PAD_TS1_D3                 127
#define  PAD_TS1_D4                 128
#define  PAD_TS1_D5                 129
#define  PAD_TS1_D6                 130
#define  PAD_TS1_D7                 131
#define  PAD_GPIO140                132
#define  PAD_TS2_SYNC               133
#define  PAD_TS2_VLD                134                
#define  PAD_GPIO143                135
#define  PAD_GPIO144                136
#define  PAD_GPIO145                137
#define  PAD_GPIO146                138
#define  PAD_GPIO147                139
#define  PAD_GPIO148                140
#define  PAD_TS2_D6                 141
#define  PAD_TS2_D7                 142
#define  PAD_SPDIF_OUT              143
#define  PAD_I2CM0_SCL              144
#define  PAD_I2CM0_SDA              145
#define  PAD_I2CM1_SCL              146
#define  PAD_I2CM1_SDA              147
#define  PAD_HDMITX_SCL             148
#define  PAD_HDMITX_SDA             149
#define  PAD_EMMC_IO0               150
#define  PAD_EMMC_IO1               151
#define  PAD_EMMC_IO2               152
#define  PAD_EMMC_IO3               153
#define  PAD_EMMC_IO4               154
#define  PAD_EMMC_IO5               155
#define  PAD_EMMC_IO6               156
#define  PAD_EMMC_IO7               157
#define  PAD_EMMC_IO8               158
#define  PAD_EMMC_IO9               159
#define  PAD_EMMC_IO10              160
#define  PAD_EMMC_IO11              161           
#define  PAD_EMMC_IO12              162
#define  PAD_EMMC_IO13              163
#define  PAD_EMMC_IO14              164
#define  PAD_EMMC_IO15              165
#define  PAD_EMMC_IO16              166
#define  PAD_EMMC_IO17              167
#define  PAD_DM_P0                  168
#define  PAD_DP_P0                  169
#define  PAD_DM_P1                  170
#define  PAD_DP_P1                  171
#define  PAD_DM_P2                  172
#define  PAD_DP_P2                  173
#define  PADA_TSIO_OUTN_CH0         174
#define  PADA_TSIO_OUTP_CH0         175
#define  PADA_TSIO_OUTN_CH1         176
#define  PADA_TSIO_OUTP_CH1         177
#define  PADA_TSIO_INN              178
#define  PADA_TSIO_INP              179



////8 bit define
#define REG_IRIN_00    0x0e38
#define REG_SPI_00     0x0e6a
#define REG_PMGPIO_00  0x0e24
#define REG_PMGPIO_01  0x0e25
#define REG_PMGPIO_02  0x0e50
#define REG_PMGPIO_03  0x0e51
#define REG_HPD_00     0x0e4f
#define REG_GT0_00     0x0e39
#define REG_GT0_01     0x33b4
#define REG_SAR_00     0x1422
#define REG_HSYNC_00   0x101ea1
#define REG_HSYNC_01   0x101e24
#define REG_HSYNC_02   0x101e12
#define REG_HSYNC_03   0x101e06
#define REG_HSYNC_04   0x101e0e
#define REG_HSYNC_05   0x101e8e
#define REG_GPIO_00    0x101e07
#define REG_GPIO_01    0x101e09
#define REG_GPIO_02    0x101e0b
#define REG_GPIO_03    0x101e13
#define REG_GPIO_04    0x101e4b
#define REG_GPIO_05    0x101e0a
#define REG_GPIO_06    0x101e0d
#define REG_GPIO_07    0x101e04
#define REG_GPIO_08    0x101e08
#define REG_GPIO_09    0x101e17
#define REG_NAND_D00   0x101e10
#define REG_NAND_D01   0x101e6c
#define REG_DMDP_P00   0x103abf
#define REG_DMDP_P01   0x103a80
#define REG_DMDP_P02   0x103a81
#define REG_DMDP_P03   0x103a88
#define REG_DMDP_P04   0x103a89
#define REG_DMDP_P05   0x103a00
#define REG_DMDP_P06   0x103a01
#define REG_DMDP_P07   0x103a08
#define REG_DMDP_P08   0x103a09
#define REG_DMDP_P09   0x103a3f



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

