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
#define GPIO_UNIT_NUM               83

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

#define PAD_PM_IRIN                 0
#define PAD_PM_SPI_CZ               1
#define PAD_PM_SPI_CK               2
#define PAD_PM_SPI_DI               3
#define PAD_PM_SPI_DO               4
#define PAD_PM_GPIO0                5
#define PAD_PM_GPIO1                6
#define PAD_PM_GPIO2                7
#define PAD_PM_GPIO3                8
#define PAD_PM_GPIO4                9
#define PAD_PM_GPIO5                10
#define PAD_PM_GPIO6                11
#define PAD_PM_GPIO7                12
#define PAD_PM_CEC                  13
#define PAD_HDMITX_HPD              14
#define PAD_VID0                    15
#define PAD_VID1                    16
#define PAD_PM_LED0                 17
#define PAD_PM_LED1                 18
#define PAD_I2S_OUT_BCK             19
#define PAD_I2S_OUT_MCK             20
#define PAD_I2S_OUT_WS              21
#define PAD_I2S_OUT_SD0             22
#define PAD_I2S_OUT_SD1             23
#define PAD_I2S_OUT_SD2             24
#define PAD_I2S_OUT_SD3             25
#define PAD_I2S_OUT_MUTE            26
#define PAD_SD_CLK                  27
#define PAD_SD_CMD                  28
#define PAD_SD_D0                   29
#define PAD_SD_D1                   30
#define PAD_SD_D2                   31
#define PAD_SD_D3                   32
#define PAD_GPIO0                   33
#define PAD_GPIO1                   34
#define PAD_GPIO2                   35
#define PAD_GPIO3                   36
#define PAD_GPIO4                   37
#define PAD_GPIO5                   38
#define PAD_GPIO6                   39
#define PAD_GPIO7                   40
#define PAD_TS_CLK                  41
#define PAD_TS_SYNC                 42
#define PAD_TS_VLD                  43
#define PAD_TS_D0                   44
#define PAD_TS_D1                   45
#define PAD_TS_D2                   46
#define PAD_TS_D3                   47
#define PAD_TS_D4                   48
#define PAD_TS_D5                   49
#define PAD_TS_D6                   50
#define PAD_TS_D7                   51
#define PAD_SPDIF_OUT               52
#define PAD_I2CM0_SCL               53
#define PAD_I2CM0_SDA               54
#define PAD_I2CM1_SCL               55
#define PAD_I2CM1_SDA               56
#define PAD_HDMITX_SCL              57
#define PAD_HDMITX_SDA              58
#define PAD_GT0_MDIO                59
#define PAD_GT0_MDC                 60
#define PAD_GT0_RX_CLK              61
#define PAD_GT0_RX_CTL              62
#define PAD_GT0_RX_D0               63
#define PAD_GT0_RX_D1               64
#define PAD_GT0_RX_D2               65
#define PAD_GT0_RX_D3               66
#define PAD_GT0_TX_CLK              67
#define PAD_GT0_TX_CTL              68
#define PAD_GT0_TX_D0               69
#define PAD_GT0_TX_D1               70
#define PAD_GT0_TX_D2               71
#define PAD_GT0_TX_D3               72
#define PAD_SAR_GPIO0               73
#define PAD_SAR_GPIO1               74
#define PAD_SAR_GPIO2               75
#define PAD_SAR_GPIO3               76
#define PAD_SAR_GPIO4               77
#define PAD_EMMC_IO12               78
#define PAD_EMMC_IO14               79
#define PAD_EMMC_IO15               80
#define PAD_EMMC_IO16               81
#define PAD_EMMC_IO17               82

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

