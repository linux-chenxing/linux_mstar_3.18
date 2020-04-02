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
#define GPIO_UNIT_NUM               187

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
#define  PAD_HDMIRX_HPD             26
#define  PAD_RX_DDCDA_CK            27
#define  PAD_RX_DDCDA_DA            28
#define  PAD_PM_SD_CDZ              29
#define  PAD_HOTPLUGA_HDMI20_5V     30
#define  PAD_VID0                   31
#define  PAD_VID1                   32
#define  PAD_PM_LED0                33
#define  PAD_PM_LED1                34
#define  PAD_SAR_GPIO0              35
#define  PAD_SAR_GPIO1              36
#define  PAD_SAR_GPIO2              37
#define  PAD_SAR_GPIO3              38
#define  PAD_SAR_GPIO4              39
#define  PAD_HSYNC_OUT              40
#define  PAD_VSYNC_OUT              41
#define  PAD_I2S_OUT_BCK            42
#define  PAD_I2S_OUT_MCK            43
#define  PAD_I2S_OUT_WS             44
#define  PAD_I2S_OUT_SD0            45
#define  PAD_I2S_OUT_SD1            46
#define  PAD_I2S_OUT_SD2            47
#define  PAD_I2S_OUT_SD3            48
#define  PAD_I2S_OUT_MUTE           49
#define  PAD_SD_CLK                 50
#define  PAD_SD_CMD                 51
#define  PAD_SD_D0                  52
#define  PAD_SD_D1                  53
#define  PAD_SD_D2                  54
#define  PAD_SD_D3                  55
#define  PAD_GPIO0                  56
#define  PAD_GPIO1                  57
#define  PAD_GPIO2                  58
#define  PAD_GPIO3                  59
#define  PAD_GPIO4                  60
#define  PAD_GPIO5                  61
#define  PAD_GPIO6                  62
#define  PAD_GPIO7                  63
#define  PAD_GPIO8                  64
#define  PAD_GPIO9                  65
#define  PAD_SM0_CLK                66
#define  PAD_SM0_RST                67
#define  PAD_SM0_VCC                68
#define  PAD_SM0_CD                 69
#define  PAD_SM0_IO                 70
#define  PAD_SM0_VSEL               71
#define  PAD_SM0_C4                 72
#define  PAD_SM0_C8                 73
#define  PAD_CI_RST                 74
#define  PAD_CI_WAITZ               75
#define  PAD_CI_IRQAZ               76
#define  PAD_CI_IORDZ               77
#define  PAD_CI_REGZ                78
#define  PAD_CI_WEZ                 79
#define  PAD_CI_IOWRZ               80
#define  PAD_CI_CEZ                 81
#define  PAD_CI_OEZ                 82
#define  PAD_CI_CDZ                 83
#define  PAD_CI_A0                  84
#define  PAD_CI_A1                  85
#define  PAD_CI_A2                  86
#define  PAD_CI_A3                  87
#define  PAD_CI_A4                  88
#define  PAD_CI_A5                  89
#define  PAD_CI_A6                  90
#define  PAD_CI_A7                  91
#define  PAD_CI_A8                  92
#define  PAD_CI_A9                  93
#define  PAD_CI_A10                 94
#define  PAD_CI_A11                 95
#define  PAD_CI_A12                 96
#define  PAD_CI_A13                 97
#define  PAD_CI_A14                 98
#define  PAD_CI_D0                  99
#define  PAD_CI_D1                  100
#define  PAD_CI_D2                  101
#define  PAD_CI_D3                  102
#define  PAD_CI_D4                  103
#define  PAD_CI_D5                  104
#define  PAD_CI_D6                  105
#define  PAD_CI_D7                  106
#define  PAD_TS0_CLK                107
#define  PAD_TS0_SYNC               108
#define  PAD_TS0_VLD                109
#define  PAD_TS0_D0                 110
#define  PAD_TS0_D1                 111
#define  PAD_TS0_D2                 112
#define  PAD_TS0_D3                 113
#define  PAD_TS0_D4                 114
#define  PAD_TS0_D5                 115
#define  PAD_TS0_D6                 116
#define  PAD_TS0_D7                 117
#define  PAD_TS1_CLK                118
#define  PAD_TS1_SYNC               119
#define  PAD_TS1_VLD                120
#define  PAD_TS1_D0                 121
#define  PAD_TS1_D1                 122
#define  PAD_TS1_D2                 123
#define  PAD_TS1_D3                 124
#define  PAD_TS1_D4                 125
#define  PAD_TS1_D5                 126
#define  PAD_TS1_D6                 127
#define  PAD_TS1_D7                 128
#define  PAD_TS2_CLK                129
#define  PAD_TS2_SYNC               130
#define  PAD_TS2_VLD                131
#define  PAD_TS2_D0                 132
#define  PAD_TS2_D1                 133
#define  PAD_TS2_D2                 134
#define  PAD_TS2_D3                 135
#define  PAD_TS2_D4                 136
#define  PAD_TS2_D5                 137
#define  PAD_TS2_D6                 138
#define  PAD_TS2_D7                 139
#define  PAD_SPDIF_OUT              140
#define  PAD_I2CM0_SCL              141
#define  PAD_I2CM0_SDA              142
#define  PAD_I2CM1_SCL              143
#define  PAD_I2CM1_SDA              144
#define  PAD_HDMITX_SCL             145
#define  PAD_HDMITX_SDA             146
#define  PAD_HDMITX_ARC             147
#define  PAD_HDMIRX_ARC             148
#define  PAD_EMMC_IO0               149
#define  PAD_EMMC_IO1               150
#define  PAD_EMMC_IO2               151
#define  PAD_EMMC_IO3               152
#define  PAD_EMMC_IO4               153
#define  PAD_EMMC_IO5               154
#define  PAD_EMMC_IO6               155
#define  PAD_EMMC_IO7               156
#define  PAD_EMMC_IO8               157
#define  PAD_EMMC_IO9               158
#define  PAD_EMMC_IO10              159
#define  PAD_EMMC_IO11              160
#define  PAD_EMMC_IO12              161
#define  PAD_EMMC_IO13              162
#define  PAD_EMMC_IO14              163
#define  PAD_EMMC_IO15              164
#define  PAD_EMMC_IO16              165
#define  PAD_EMMC_IO17              166
#define  PAD_SDIO_IO0               167
#define  PAD_SDIO_IO1               168
#define  PAD_SDIO_IO2               169
#define  PAD_SDIO_IO3               170
#define  PAD_SDIO_IO4               171
#define  PAD_SDIO_IO5               172
#define  PAD_GT0_MDIO               173
#define  PAD_GT0_MDC                174
#define  PAD_GT0_RX_CLK             175
#define  PAD_GT0_RX_CTL             176
#define  PAD_GT0_RX_D0              177
#define  PAD_GT0_RX_D1              178
#define  PAD_GT0_RX_D2              179
#define  PAD_GT0_RX_D3              180
#define  PAD_GT0_TX_CLK             181
#define  PAD_GT0_TX_CTL             182
#define  PAD_GT0_TX_D0              183
#define  PAD_GT0_TX_D1              184
#define  PAD_GT0_TX_D2              185
#define  PAD_GT0_TX_D3              186


////8 bit define
#define REG_GPIO_PM_LOCK_L          0x0e24
#define REG_GPIO_PM_LOCK_H          0x0e25
#define REG_IRIN_CFG_GPIO           0x0e38
#define REG_MISC_CFG_GPIO_L         0x0e50
#define REG_MISC_CFG_GPIO_H         0x0e51
#define REG_SPI_CFG_GPIO            0x0e6a
#define REG_UART_CFG_GPIO           0x0e6b
#define REG_HDMI_HPD_BYPASS         0x0e4f
#define REG_SAR_CFG_GPIO            0x1422
#define REG_TS_MODE_CFG_GPIO        0x101e04
#define REG_MSPI_MODE_CFG_GPIO      0x101e05
#define REG_UART_MODE_CFG_GPIO_L    0x101e06
#define REG_UART_MODE_CFG_GPIO_H    0x101e07
#define REG_SPDIF_MODE_CFG_GPIO     0x101e0a
#define REG_GT_MODE_CFG_GPIO        0x101e0c
#define REG_PWM_MODE_CFG_GPIO       0x101e0e
#define REG_EMMC_NAND_CFG_GPIO      0x101e10
#define REG_I2C_MODE_CFG_GPIO       0x101e12
#define REG_FUART_MODE_CFG_GPIO     0x101e13
#define REG_HDMI_MODE_CFG_GPIO      0x101e17
#define REG_TEST_MODE_CFG_GPIO      0x101e24
#define REG_USB30_MODE0_CFG_GPIO    0x101e26
#define REG_USB30_MODE1_CFG_GPIO    0x101e27


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

