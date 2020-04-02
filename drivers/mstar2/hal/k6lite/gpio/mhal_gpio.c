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

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "MsCommon.h"
#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <asm/io.h>

#include "mhal_gpio.h"
#include "mhal_gpio_reg.h"

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define _CONCAT( a, b )     a##b
#define CONCAT( a, b )      _CONCAT( a, b )

/*
#define BIT0    BIT(0)
#define BIT1    BIT(1)
#define BIT2    BIT(2)
#define BIT3    BIT(3)
#define BIT4    BIT(4)
#define BIT5    BIT(5)
#define BIT6    BIT(6)
#define BIT7    BIT(7)
*/

// Dummy
#define GPIO999_OEN     0, 0
#define GPIO999_OUT     0, 0
#define GPIO999_IN      0, 0

#define GPIO0_PAD PAD_PM_IRIN
#define GPIO0_OEN 0x000f28, BIT0
#define GPIO0_OUT 0x000f28, BIT1
#define GPIO0_IN  0x000f28, BIT2

#define GPIO1_PAD PAD_PM_SPI_CZ
#define GPIO1_OEN 0x000f30, BIT0
#define GPIO1_OUT 0x000f30, BIT1
#define GPIO1_IN  0x000f30, BIT2

#define GPIO2_PAD PAD_PM_SPI_CK
#define GPIO2_OEN 0x000f32, BIT0
#define GPIO2_OUT 0x000f32, BIT1
#define GPIO2_IN  0x000f32, BIT2

#define GPIO3_PAD PAD_PM_SPI_DI
#define GPIO3_OEN 0x000f34, BIT0
#define GPIO3_OUT 0x000f34, BIT1
#define GPIO3_IN  0x000f34, BIT2

#define GPIO4_PAD PAD_PM_SPI_DO
#define GPIO4_OEN 0x000f36, BIT0
#define GPIO4_OUT 0x000f36, BIT1
#define GPIO4_IN  0x000f36, BIT2

#define GPIO5_PAD PAD_PM_SPI_WPZ
#define GPIO5_OEN 0x000f88, BIT0
#define GPIO5_OUT 0x000f88, BIT1
#define GPIO5_IN  0x000f88, BIT2

#define GPIO6_PAD PAD_PM_SPI_HOLDZ
#define GPIO6_OEN 0x000f8a, BIT0
#define GPIO6_OUT 0x000f8a, BIT1
#define GPIO6_IN  0x000f8a, BIT2

#define GPIO7_PAD PAD_PM_SPI_RSTZ
#define GPIO7_OEN 0x000f8c, BIT0
#define GPIO7_OUT 0x000f8c, BIT1
#define GPIO7_IN  0x000f8c, BIT2

#define GPIO8_PAD PAD_PM_GPIO0
#define GPIO8_OEN 0x000f00, BIT0
#define GPIO8_OUT 0x000f00, BIT1
#define GPIO8_IN  0x000f00, BIT2

#define GPIO9_PAD PAD_PM_GPIO1
#define GPIO9_OEN 0x000f02, BIT0
#define GPIO9_OUT 0x000f02, BIT1
#define GPIO9_IN  0x000f02, BIT2

#define GPIO10_PAD PAD_PM_GPIO2
#define GPIO10_OEN 0x000f04, BIT0
#define GPIO10_OUT 0x000f04, BIT1
#define GPIO10_IN  0x000f04, BIT2

#define GPIO11_PAD PAD_PM_GPIO3
#define GPIO11_OEN 0x000f06, BIT0
#define GPIO11_OUT 0x000f06, BIT1
#define GPIO11_IN  0x000f06, BIT2

#define GPIO12_PAD PAD_PM_GPIO4
#define GPIO12_OEN 0x000f08, BIT0
#define GPIO12_OUT 0x000f08, BIT1
#define GPIO12_IN  0x000f08, BIT2

#define GPIO13_PAD PAD_PM_GPIO5
#define GPIO13_OEN 0x000f0a, BIT0
#define GPIO13_OUT 0x000f0a, BIT1
#define GPIO13_IN  0x000f0a, BIT2

#define GPIO14_PAD PAD_PM_GPIO6
#define GPIO14_OEN 0x000f0c, BIT0
#define GPIO14_OUT 0x000f0c, BIT1
#define GPIO14_IN  0x000f0c, BIT2

#define GPIO15_PAD PAD_PM_GPIO7
#define GPIO15_OEN 0x000f0e, BIT0
#define GPIO15_OUT 0x000f0e, BIT1
#define GPIO15_IN  0x000f0e, BIT2

#define GPIO16_PAD PAD_PM_GPIO8
#define GPIO16_OEN 0x000f10, BIT0
#define GPIO16_OUT 0x000f10, BIT1
#define GPIO16_IN  0x000f10, BIT2

#define GPIO17_PAD PAD_PM_GPIO9
#define GPIO17_OEN 0x000f12, BIT0
#define GPIO17_OUT 0x000f12, BIT1
#define GPIO17_IN  0x000f12, BIT2

#define GPIO18_PAD PAD_PM_GPIO10
#define GPIO18_OEN 0x000f14, BIT0
#define GPIO18_OUT 0x000f14, BIT1
#define GPIO18_IN  0x000f14, BIT2

#define GPIO19_PAD PAD_PM_GPIO11
#define GPIO19_OEN 0x000f16, BIT0
#define GPIO19_OUT 0x000f16, BIT1
#define GPIO19_IN  0x000f16, BIT2

#define GPIO20_PAD PAD_PM_GPIO12
#define GPIO20_OEN 0x000f18, BIT0
#define GPIO20_OUT 0x000f18, BIT1
#define GPIO20_IN  0x000f18, BIT2

#define GPIO21_PAD PAD_PM_GPIO13
#define GPIO21_OEN 0x000f1a, BIT0
#define GPIO21_OUT 0x000f1a, BIT1
#define GPIO21_IN  0x000f1a, BIT2

#define GPIO22_PAD PAD_PM_GPIO14
#define GPIO22_OEN 0x000f1c, BIT0
#define GPIO22_OUT 0x000f1c, BIT1
#define GPIO22_IN  0x000f1c, BIT2

#define GPIO23_PAD PAD_PM_GPIO15
#define GPIO23_OEN 0x000f1e, BIT0
#define GPIO23_OUT 0x000f1e, BIT1
#define GPIO23_IN  0x000f1e, BIT2

#define GPIO24_PAD PAD_PM_CEC
#define GPIO24_OEN 0x000f2c, BIT0
#define GPIO24_OUT 0x000f2c, BIT1
#define GPIO24_IN  0x000f2c, BIT2

#define GPIO25_PAD PAD_HDMITX_HPD
#define GPIO25_OEN 0x001166, BIT3
#define GPIO25_OUT 0x001166, BIT2
#define GPIO25_IN  0x001166, BIT0

#define GPIO26_PAD PAD_PM_SD_CDZ
#define GPIO26_OEN 0x000f8e, BIT0
#define GPIO26_OUT 0x000f8e, BIT1
#define GPIO26_IN  0x000f8e, BIT2

#define GPIO27_PAD PAD_VID0
#define GPIO27_OEN 0x000f90, BIT0
#define GPIO27_OUT 0x000f90, BIT1
#define GPIO27_IN  0x000f90, BIT2

#define GPIO28_PAD PAD_VID1
#define GPIO28_OEN 0x000f92, BIT0
#define GPIO28_OUT 0x000f92, BIT1
#define GPIO28_IN  0x000f92, BIT2

#define GPIO29_PAD PAD_PM_GT0_MDIO
#define GPIO29_OEN 0x000f50, BIT0
#define GPIO29_OUT 0x000f50, BIT1
#define GPIO29_IN  0x000f50, BIT2

#define GPIO30_PAD PAD_PM_GT0_MDC
#define GPIO30_OEN 0x000f4e, BIT0
#define GPIO30_OUT 0x000f4e, BIT1
#define GPIO30_IN  0x000f4e, BIT2

#define GPIO31_PAD PAD_PM_GT0_RX_CLK
#define GPIO31_OEN 0x000f52, BIT0
#define GPIO31_OUT 0x000f52, BIT1
#define GPIO31_IN  0x000f52, BIT2

#define GPIO32_PAD PAD_PM_GT0_RX_CTL
#define GPIO32_OEN 0x000f54, BIT0
#define GPIO32_OUT 0x000f54, BIT1
#define GPIO32_IN  0x000f54, BIT2

#define GPIO33_PAD PAD_PM_GT0_RX_D0
#define GPIO33_OEN 0x000f56, BIT0
#define GPIO33_OUT 0x000f56, BIT1
#define GPIO33_IN  0x000f56, BIT2

#define GPIO34_PAD PAD_PM_GT0_RX_D1
#define GPIO34_OEN 0x000f58, BIT0
#define GPIO34_OUT 0x000f58, BIT1
#define GPIO34_IN  0x000f58, BIT2

#define GPIO35_PAD PAD_PM_GT0_RX_D2
#define GPIO35_OEN 0x000f5a, BIT0
#define GPIO35_OUT 0x000f5a, BIT1
#define GPIO35_IN  0x000f5a, BIT2

#define GPIO36_PAD PAD_PM_GT0_RX_D3
#define GPIO36_OEN 0x000f5c, BIT0
#define GPIO36_OUT 0x000f5c, BIT1
#define GPIO36_IN  0x000f5c, BIT2

#define GPIO37_PAD PAD_PM_GT0_TX_CLK
#define GPIO37_OEN 0x000f5e, BIT0
#define GPIO37_OUT 0x000f5e, BIT1
#define GPIO37_IN  0x000f5e, BIT2

#define GPIO38_PAD PAD_PM_GT0_TX_CTL
#define GPIO38_OEN 0x000f60, BIT0
#define GPIO38_OUT 0x000f60, BIT1
#define GPIO38_IN  0x000f60, BIT2

#define GPIO39_PAD PAD_PM_GT0_TX_D0
#define GPIO39_OEN 0x000f62, BIT0
#define GPIO39_OUT 0x000f62, BIT1
#define GPIO39_IN  0x000f62, BIT2

#define GPIO40_PAD PAD_PM_GT0_TX_D1
#define GPIO40_OEN 0x000f64, BIT0
#define GPIO40_OUT 0x000f64, BIT1
#define GPIO40_IN  0x000f64, BIT2

#define GPIO41_PAD PAD_PM_GT0_TX_D2
#define GPIO41_OEN 0x000f66, BIT0
#define GPIO41_OUT 0x000f66, BIT1
#define GPIO41_IN  0x000f66, BIT2

#define GPIO42_PAD PAD_PM_GT0_TX_D3
#define GPIO42_OEN 0x000f68, BIT0
#define GPIO42_OUT 0x000f68, BIT1
#define GPIO42_IN  0x000f68, BIT2

#define GPIO43_PAD PAD_PM_LED0
#define GPIO43_OEN 0x000f94, BIT0
#define GPIO43_OUT 0x000f94, BIT1
#define GPIO43_IN  0x000f94, BIT2

#define GPIO44_PAD PAD_PM_LED1
#define GPIO44_OEN 0x000f96, BIT0
#define GPIO44_OUT 0x000f96, BIT1
#define GPIO44_IN  0x000f96, BIT2

#define GPIO45_PAD PAD_SAR_GPIO0
#define GPIO45_OEN 0x001423, BIT0
#define GPIO45_OUT 0x001424, BIT0
#define GPIO45_IN  0x001425, BIT0

#define GPIO46_PAD PAD_SAR_GPIO1
#define GPIO46_OEN 0x001423, BIT1
#define GPIO46_OUT 0x001424, BIT1
#define GPIO46_IN  0x001425, BIT1

#define GPIO47_PAD PAD_SAR_GPIO2
#define GPIO47_OEN 0x001423, BIT2
#define GPIO47_OUT 0x001424, BIT2
#define GPIO47_IN  0x001425, BIT2

#define GPIO48_PAD PAD_SAR_GPIO3
#define GPIO48_OEN 0x001423, BIT3
#define GPIO48_OUT 0x001424, BIT3
#define GPIO48_IN  0x001425, BIT3

#define GPIO49_PAD PAD_SAR_GPIO4
#define GPIO49_OEN 0x001423, BIT4
#define GPIO49_OUT 0x001424, BIT4
#define GPIO49_IN  0x001425, BIT4

#define GPIO50_PAD PAD_VPLUG_DET
#define GPIO50_OEN 0x001423, BIT5
#define GPIO50_OUT 0x001424, BIT5
#define GPIO50_IN  0x001425, BIT5

#define GPIO51_PAD PAD_HSYNC_OUT
#define GPIO51_OEN 0x1025ec, BIT5
#define GPIO51_OUT 0x1025ec, BIT4
#define GPIO51_IN  0x1025ec, BIT0

#define GPIO52_PAD PAD_VSYNC_OUT
#define GPIO52_OEN 0x1025ee, BIT5
#define GPIO52_OUT 0x1025ee, BIT4
#define GPIO52_IN  0x1025ee, BIT0

#define GPIO53_PAD PAD_SD_CLK
#define GPIO53_OEN 0x1025a4, BIT5
#define GPIO53_OUT 0x1025a4, BIT4
#define GPIO53_IN  0x1025a4, BIT0

#define GPIO54_PAD PAD_SD_CMD
#define GPIO54_OEN 0x1025a6, BIT5
#define GPIO54_OUT 0x1025a6, BIT4
#define GPIO54_IN  0x1025a6, BIT0

#define GPIO55_PAD PAD_SD_D0
#define GPIO55_OEN 0x1025a8, BIT5
#define GPIO55_OUT 0x1025a8, BIT4
#define GPIO55_IN  0x1025a8, BIT0

#define GPIO56_PAD PAD_SD_D1
#define GPIO56_OEN 0x1025aa, BIT5
#define GPIO56_OUT 0x1025aa, BIT4
#define GPIO56_IN  0x1025aa, BIT0

#define GPIO57_PAD PAD_SD_D2
#define GPIO57_OEN 0x1025ac, BIT5
#define GPIO57_OUT 0x1025ac, BIT4
#define GPIO57_IN  0x1025ac, BIT0

#define GPIO58_PAD PAD_SD_D3
#define GPIO58_OEN 0x1025ae, BIT5
#define GPIO58_OUT 0x1025ae, BIT4
#define GPIO58_IN  0x1025ae, BIT0

#define GPIO59_PAD PAD_GPIO0
#define GPIO59_OEN 0x1025b0, BIT5
#define GPIO59_OUT 0x1025b0, BIT4
#define GPIO59_IN  0x1025b0, BIT0

#define GPIO60_PAD PAD_GPIO1
#define GPIO60_OEN 0x1025b2, BIT5
#define GPIO60_OUT 0x1025b2, BIT4
#define GPIO60_IN  0x1025b2, BIT0

#define GPIO61_PAD PAD_GPIO2
#define GPIO61_OEN 0x1025b4, BIT5
#define GPIO61_OUT 0x1025b4, BIT4
#define GPIO61_IN  0x1025b4, BIT0

#define GPIO62_PAD PAD_GPIO3
#define GPIO62_OEN 0x1025b6, BIT5
#define GPIO62_OUT 0x1025b6, BIT4
#define GPIO62_IN  0x1025b6, BIT0

#define GPIO63_PAD PAD_GPIO4
#define GPIO63_OEN 0x1025b8, BIT5
#define GPIO63_OUT 0x1025b8, BIT4
#define GPIO63_IN  0x1025b8, BIT0

#define GPIO64_PAD PAD_GPIO5
#define GPIO64_OEN 0x1025ba, BIT5
#define GPIO64_OUT 0x1025ba, BIT4
#define GPIO64_IN  0x1025ba, BIT0

#define GPIO65_PAD PAD_GPIO6
#define GPIO65_OEN 0x1025bc, BIT5
#define GPIO65_OUT 0x1025bc, BIT4
#define GPIO65_IN  0x1025bc, BIT0

#define GPIO66_PAD PAD_GPIO7
#define GPIO66_OEN 0x1025be, BIT5
#define GPIO66_OUT 0x1025be, BIT4
#define GPIO66_IN  0x1025be, BIT0

#define GPIO67_PAD PAD_GPIO8
#define GPIO67_OEN 0x1025c0, BIT5
#define GPIO67_OUT 0x1025c0, BIT4
#define GPIO67_IN  0x1025c0, BIT0

#define GPIO68_PAD PAD_GPIO9
#define GPIO68_OEN 0x1025c2, BIT5
#define GPIO68_OUT 0x1025c2, BIT4
#define GPIO68_IN  0x1025c2, BIT0

#define GPIO69_PAD PAD_SM0_CLK
#define GPIO69_OEN 0x102500, BIT5
#define GPIO69_OUT 0x102500, BIT4
#define GPIO69_IN  0x102500, BIT0

#define GPIO70_PAD PAD_SM0_RST
#define GPIO70_OEN 0x102502, BIT5
#define GPIO70_OUT 0x102502, BIT4
#define GPIO70_IN  0x102502, BIT0

#define GPIO71_PAD PAD_SM0_VCC
#define GPIO71_OEN 0x102504, BIT5
#define GPIO71_OUT 0x102504, BIT4
#define GPIO71_IN  0x102504, BIT0

#define GPIO72_PAD PAD_SM0_CD
#define GPIO72_OEN 0x102506, BIT5
#define GPIO72_OUT 0x102506, BIT4
#define GPIO72_IN  0x102506, BIT0

#define GPIO73_PAD PAD_SM0_IO
#define GPIO73_OEN 0x102508, BIT5
#define GPIO73_OUT 0x102508, BIT4
#define GPIO73_IN  0x102508, BIT0

#define GPIO74_PAD PAD_SM0_VSEL
#define GPIO74_OEN 0x10250a, BIT5
#define GPIO74_OUT 0x10250a, BIT4
#define GPIO74_IN  0x10250a, BIT0

#define GPIO75_PAD PAD_SM0_C4
#define GPIO75_OEN 0x10250c, BIT5
#define GPIO75_OUT 0x10250c, BIT4
#define GPIO75_IN  0x10250c, BIT0

#define GPIO76_PAD PAD_SM0_C8
#define GPIO76_OEN 0x10250d, BIT5
#define GPIO76_OUT 0x10250d, BIT4
#define GPIO76_IN  0x10250d, BIT0

#define GPIO77_PAD PAD_CI_RST
#define GPIO77_OEN 0x10269a, BIT5
#define GPIO77_OUT 0x10269a, BIT4
#define GPIO77_IN  0x10269a, BIT0

#define GPIO78_PAD PAD_CI_WAITZ
#define GPIO78_OEN 0x10269c, BIT5
#define GPIO78_OUT 0x10269c, BIT4
#define GPIO78_IN  0x10269c, BIT0

#define GPIO79_PAD PAD_CI_IRQAZ
#define GPIO79_OEN 0x10269e, BIT5
#define GPIO79_OUT 0x10269e, BIT4
#define GPIO79_IN  0x10269e, BIT0

#define GPIO80_PAD PAD_CI_IORDZ
#define GPIO80_OEN 0x1026a0, BIT5
#define GPIO80_OUT 0x1026a0, BIT4
#define GPIO80_IN  0x1026a0, BIT0

#define GPIO81_PAD PAD_CI_REGZ
#define GPIO81_OEN 0x1026a2, BIT5
#define GPIO81_OUT 0x1026a2, BIT4
#define GPIO81_IN  0x1026a2, BIT0

#define GPIO82_PAD PAD_CI_WEZ
#define GPIO82_OEN 0x1026a4, BIT5
#define GPIO82_OUT 0x1026a4, BIT4
#define GPIO82_IN  0x1026a4, BIT0

#define GPIO83_PAD PAD_CI_IOWRZ
#define GPIO83_OEN 0x1026a6, BIT5
#define GPIO83_OUT 0x1026a6, BIT4
#define GPIO83_IN  0x1026a6, BIT0

#define GPIO84_PAD PAD_CI_CEZ
#define GPIO84_OEN 0x1026a8, BIT5
#define GPIO84_OUT 0x1026a8, BIT4
#define GPIO84_IN  0x1026a8, BIT0

#define GPIO85_PAD PAD_CI_OEZ
#define GPIO85_OEN 0x1026aa, BIT5
#define GPIO85_OUT 0x1026aa, BIT4
#define GPIO85_IN  0x1026aa, BIT0

#define GPIO86_PAD PAD_CI_CDZ
#define GPIO86_OEN 0x1026ac, BIT5
#define GPIO86_OUT 0x1026ac, BIT4
#define GPIO86_IN  0x1026ac, BIT0

#define GPIO87_PAD PAD_CI_A0
#define GPIO87_OEN 0x1026ae, BIT5
#define GPIO87_OUT 0x1026ae, BIT4
#define GPIO87_IN  0x1026ae, BIT0

#define GPIO88_PAD PAD_CI_A1
#define GPIO88_OEN 0x1026b0, BIT5
#define GPIO88_OUT 0x1026b0, BIT4
#define GPIO88_IN  0x1026b0, BIT0

#define GPIO89_PAD PAD_CI_A2
#define GPIO89_OEN 0x1026b2, BIT5
#define GPIO89_OUT 0x1026b2, BIT4
#define GPIO89_IN  0x1026b2, BIT0

#define GPIO90_PAD PAD_CI_A3
#define GPIO90_OEN 0x1026b4, BIT5
#define GPIO90_OUT 0x1026b4, BIT4
#define GPIO90_IN  0x1026b4, BIT0

#define GPIO91_PAD PAD_CI_A4
#define GPIO91_OEN 0x1026b6, BIT5
#define GPIO91_OUT 0x1026b6, BIT4
#define GPIO91_IN  0x1026b6, BIT0

#define GPIO92_PAD PAD_CI_A5
#define GPIO92_OEN 0x1026b8, BIT5
#define GPIO92_OUT 0x1026b8, BIT4
#define GPIO92_IN  0x1026b8, BIT0

#define GPIO93_PAD PAD_CI_A6
#define GPIO93_OEN 0x1026ba, BIT5
#define GPIO93_OUT 0x1026ba, BIT4
#define GPIO93_IN  0x1026ba, BIT0

#define GPIO94_PAD PAD_CI_A7
#define GPIO94_OEN 0x1026bc, BIT5
#define GPIO94_OUT 0x1026bc, BIT4
#define GPIO94_IN  0x1026bc, BIT0

#define GPIO95_PAD PAD_CI_A8
#define GPIO95_OEN 0x1026be, BIT5
#define GPIO95_OUT 0x1026be, BIT4
#define GPIO95_IN  0x1026be, BIT0

#define GPIO96_PAD PAD_CI_A9
#define GPIO96_OEN 0x1026c0, BIT5
#define GPIO96_OUT 0x1026c0, BIT4
#define GPIO96_IN  0x1026c0, BIT0

#define GPIO97_PAD PAD_CI_A10
#define GPIO97_OEN 0x1026c2, BIT5
#define GPIO97_OUT 0x1026c2, BIT4
#define GPIO97_IN  0x1026c2, BIT0

#define GPIO98_PAD PAD_CI_A11
#define GPIO98_OEN 0x1026c4, BIT5
#define GPIO98_OUT 0x1026c4, BIT4
#define GPIO98_IN  0x1026c4, BIT0

#define GPIO99_PAD PAD_CI_A12
#define GPIO99_OEN 0x1026c6, BIT5
#define GPIO99_OUT 0x1026c6, BIT4
#define GPIO99_IN  0x1026c6, BIT0

#define GPIO100_PAD PAD_CI_A13
#define GPIO100_OEN 0x1026c8, BIT5
#define GPIO100_OUT 0x1026c8, BIT4
#define GPIO100_IN  0x1026c8, BIT0

#define GPIO101_PAD PAD_CI_A14
#define GPIO101_OEN 0x1026ca, BIT5
#define GPIO101_OUT 0x1026ca, BIT4
#define GPIO101_IN  0x1026ca, BIT0

#define GPIO102_PAD PAD_CI_D0
#define GPIO102_OEN 0x1026cc, BIT5
#define GPIO102_OUT 0x1026cc, BIT4
#define GPIO102_IN  0x1026cc, BIT0

#define GPIO103_PAD PAD_CI_D1
#define GPIO103_OEN 0x1026ce, BIT5
#define GPIO103_OUT 0x1026ce, BIT4
#define GPIO103_IN  0x1026ce, BIT0

#define GPIO104_PAD PAD_CI_D2
#define GPIO104_OEN 0x1026d0, BIT5
#define GPIO104_OUT 0x1026d0, BIT4
#define GPIO104_IN  0x1026d0, BIT0

#define GPIO105_PAD PAD_CI_D3
#define GPIO105_OEN 0x1026d2, BIT5
#define GPIO105_OUT 0x1026d2, BIT4
#define GPIO105_IN  0x1026d2, BIT0

#define GPIO106_PAD PAD_CI_D4
#define GPIO106_OEN 0x1026d4, BIT5
#define GPIO106_OUT 0x1026d4, BIT4
#define GPIO106_IN  0x1026d4, BIT0

#define GPIO107_PAD PAD_CI_D5
#define GPIO107_OEN 0x1026d6, BIT5
#define GPIO107_OUT 0x1026d6, BIT4
#define GPIO107_IN  0x1026d6, BIT0

#define GPIO108_PAD PAD_CI_D6
#define GPIO108_OEN 0x1026d8, BIT5
#define GPIO108_OUT 0x1026d8, BIT4
#define GPIO108_IN  0x1026d8, BIT0

#define GPIO109_PAD PAD_CI_D7
#define GPIO109_OEN 0x1026da, BIT5
#define GPIO109_OUT 0x1026da, BIT4
#define GPIO109_IN  0x1026da, BIT0

#define GPIO110_PAD PAD_TS0_CLK
#define GPIO110_OEN 0x102524, BIT5
#define GPIO110_OUT 0x102524, BIT4
#define GPIO110_IN  0x102524, BIT0

#define GPIO111_PAD PAD_TS0_SYNC
#define GPIO111_OEN 0x102526, BIT5
#define GPIO111_OUT 0x102526, BIT4
#define GPIO111_IN  0x102526, BIT0

#define GPIO112_PAD PAD_TS0_VLD
#define GPIO112_OEN 0x102528, BIT5
#define GPIO112_OUT 0x102528, BIT4
#define GPIO112_IN  0x102528, BIT0

#define GPIO113_PAD PAD_TS0_D0
#define GPIO113_OEN 0x10252a, BIT5
#define GPIO113_OUT 0x10252a, BIT4
#define GPIO113_IN  0x10252a, BIT0

#define GPIO114_PAD PAD_TS0_D1
#define GPIO114_OEN 0x10252c, BIT5
#define GPIO114_OUT 0x10252c, BIT4
#define GPIO114_IN  0x10252c, BIT0

#define GPIO115_PAD PAD_TS0_D2
#define GPIO115_OEN 0x10252e, BIT5
#define GPIO115_OUT 0x10252e, BIT4
#define GPIO115_IN  0x10252e, BIT0

#define GPIO116_PAD PAD_TS0_D3
#define GPIO116_OEN 0x102530, BIT5
#define GPIO116_OUT 0x102530, BIT4
#define GPIO116_IN  0x102530, BIT0

#define GPIO117_PAD PAD_TS0_D4
#define GPIO117_OEN 0x102532, BIT5
#define GPIO117_OUT 0x102532, BIT4
#define GPIO117_IN  0x102532, BIT0

#define GPIO118_PAD PAD_TS0_D5
#define GPIO118_OEN 0x102534, BIT5
#define GPIO118_OUT 0x102534, BIT4
#define GPIO118_IN  0x102534, BIT0

#define GPIO119_PAD PAD_TS0_D6
#define GPIO119_OEN 0x102536, BIT5
#define GPIO119_OUT 0x102536, BIT4
#define GPIO119_IN  0x102536, BIT0

#define GPIO120_PAD PAD_TS0_D7
#define GPIO120_OEN 0x102538, BIT5
#define GPIO120_OUT 0x102538, BIT4
#define GPIO120_IN  0x102538, BIT0

#define GPIO121_PAD PAD_TS1_CLK
#define GPIO121_OEN 0x10263a, BIT5
#define GPIO121_OUT 0x10263a, BIT4
#define GPIO121_IN  0x10263a, BIT0

#define GPIO122_PAD PAD_TS1_SYNC
#define GPIO122_OEN 0x10263c, BIT5
#define GPIO122_OUT 0x10263c, BIT4
#define GPIO122_IN  0x10263c, BIT0

#define GPIO123_PAD PAD_TS1_VLD
#define GPIO123_OEN 0x10263e, BIT5
#define GPIO123_OUT 0x10263e, BIT4
#define GPIO123_IN  0x10263e, BIT0

#define GPIO124_PAD PAD_TS1_D0
#define GPIO124_OEN 0x102640, BIT5
#define GPIO124_OUT 0x102640, BIT4
#define GPIO124_IN  0x102640, BIT0

#define GPIO125_PAD PAD_TS1_D1
#define GPIO125_OEN 0x102642, BIT5
#define GPIO125_OUT 0x102642, BIT4
#define GPIO125_IN  0x102642, BIT0

#define GPIO126_PAD PAD_TS1_D2
#define GPIO126_OEN 0x102644, BIT5
#define GPIO126_OUT 0x102644, BIT4
#define GPIO126_IN  0x102644, BIT0

#define GPIO127_PAD PAD_TS1_D3
#define GPIO127_OEN 0x102646, BIT5
#define GPIO127_OUT 0x102646, BIT4
#define GPIO127_IN  0x102646, BIT0

#define GPIO128_PAD PAD_TS1_D4
#define GPIO128_OEN 0x102648, BIT5
#define GPIO128_OUT 0x102648, BIT4
#define GPIO128_IN  0x102648, BIT0

#define GPIO129_PAD PAD_TS1_D5
#define GPIO129_OEN 0x10264a, BIT5
#define GPIO129_OUT 0x10264a, BIT4
#define GPIO129_IN  0x10264a, BIT0

#define GPIO130_PAD PAD_TS1_D6
#define GPIO130_OEN 0x10264c, BIT5
#define GPIO130_OUT 0x10264c, BIT4
#define GPIO130_IN  0x10264c, BIT0

#define GPIO131_PAD PAD_TS1_D7
#define GPIO131_OEN 0x10264e, BIT5
#define GPIO131_OUT 0x10264e, BIT4
#define GPIO131_IN  0x10264e, BIT0

#define GPIO132_PAD PAD_TS2_CLK
#define GPIO132_OEN 0x102650, BIT5
#define GPIO132_OUT 0x102650, BIT4
#define GPIO132_IN  0x102650, BIT0

#define GPIO133_PAD PAD_TS2_SYNC
#define GPIO133_OEN 0x102652, BIT5
#define GPIO133_OUT 0x102652, BIT4
#define GPIO133_IN  0x102652, BIT0

#define GPIO134_PAD PAD_TS2_VLD
#define GPIO134_OEN 0x102654, BIT5
#define GPIO134_OUT 0x102654, BIT4
#define GPIO134_IN  0x102654, BIT0

#define GPIO135_PAD PAD_TS2_D0
#define GPIO135_OEN 0x102656, BIT5
#define GPIO135_OUT 0x102656, BIT4
#define GPIO135_IN  0x102656, BIT0

#define GPIO136_PAD PAD_TS2_D1
#define GPIO136_OEN 0x102658, BIT5
#define GPIO136_OUT 0x102658, BIT4
#define GPIO136_IN  0x102658, BIT0

#define GPIO137_PAD PAD_TS2_D2
#define GPIO137_OEN 0x10265a, BIT5
#define GPIO137_OUT 0x10265a, BIT4
#define GPIO137_IN  0x10265a, BIT0

#define GPIO138_PAD PAD_TS2_D3
#define GPIO138_OEN 0x10265c, BIT5
#define GPIO138_OUT 0x10265c, BIT4
#define GPIO138_IN  0x10265c, BIT0

#define GPIO139_PAD PAD_TS2_D4
#define GPIO139_OEN 0x10265e, BIT5
#define GPIO139_OUT 0x10265e, BIT4
#define GPIO139_IN  0x10265e, BIT0

#define GPIO140_PAD PAD_TS2_D5
#define GPIO140_OEN 0x102660, BIT5
#define GPIO140_OUT 0x102660, BIT4
#define GPIO140_IN  0x102660, BIT0

#define GPIO141_PAD PAD_TS2_D6
#define GPIO141_OEN 0x102662, BIT5
#define GPIO141_OUT 0x102662, BIT4
#define GPIO141_IN  0x102662, BIT0

#define GPIO142_PAD PAD_TS2_D7
#define GPIO142_OEN 0x102664, BIT5
#define GPIO142_OUT 0x102664, BIT4
#define GPIO142_IN  0x102664, BIT0

#define GPIO143_PAD PAD_SPDIF_OUT
#define GPIO143_OEN 0x1025a0, BIT5
#define GPIO143_OUT 0x1025a0, BIT4
#define GPIO143_IN  0x1025a0, BIT0

#define GPIO144_PAD PAD_I2CM0_SCL
#define GPIO144_OEN 0x10251c, BIT5
#define GPIO144_OUT 0x10251c, BIT4
#define GPIO144_IN  0x10251c, BIT0

#define GPIO145_PAD PAD_I2CM0_SDA
#define GPIO145_OEN 0x10251e, BIT5
#define GPIO145_OUT 0x10251e, BIT4
#define GPIO145_IN  0x10251e, BIT0

#define GPIO146_PAD PAD_I2CM1_SCL
#define GPIO146_OEN 0x102520, BIT5
#define GPIO146_OUT 0x102520, BIT4
#define GPIO146_IN  0x102520, BIT0

#define GPIO147_PAD PAD_I2CM1_SDA
#define GPIO147_OEN 0x102522, BIT5
#define GPIO147_OUT 0x102522, BIT4
#define GPIO147_IN  0x102522, BIT0

#define GPIO148_PAD PAD_HDMITX_SCL
#define GPIO148_OEN 0x1025da, BIT5
#define GPIO148_OUT 0x1025da, BIT4
#define GPIO148_IN  0x1025da, BIT0

#define GPIO149_PAD PAD_HDMITX_SDA
#define GPIO149_OEN 0x1025dc, BIT5
#define GPIO149_OUT 0x1025dc, BIT4
#define GPIO149_IN  0x1025dc, BIT0

#define GPIO150_PAD PAD_EMMC_IO0
#define GPIO150_OEN 0x102570, BIT5
#define GPIO150_OUT 0x102570, BIT4
#define GPIO150_IN  0x102570, BIT0

#define GPIO151_PAD PAD_EMMC_IO1
#define GPIO151_OEN 0x102572, BIT5
#define GPIO151_OUT 0x102572, BIT4
#define GPIO151_IN  0x102572, BIT0

#define GPIO152_PAD PAD_EMMC_IO2
#define GPIO152_OEN 0x102574, BIT5
#define GPIO152_OUT 0x102574, BIT4
#define GPIO152_IN  0x102574, BIT0

#define GPIO153_PAD PAD_EMMC_IO3
#define GPIO153_OEN 0x102576, BIT5
#define GPIO153_OUT 0x102576, BIT4
#define GPIO153_IN  0x102576, BIT0

#define GPIO154_PAD PAD_EMMC_IO4
#define GPIO154_OEN 0x102578, BIT5
#define GPIO154_OUT 0x102578, BIT4
#define GPIO154_IN  0x102578, BIT0

#define GPIO155_PAD PAD_EMMC_IO5
#define GPIO155_OEN 0x10257a, BIT5
#define GPIO155_OUT 0x10257a, BIT4
#define GPIO155_IN  0x10257a, BIT0

#define GPIO156_PAD PAD_EMMC_IO6
#define GPIO156_OEN 0x10257c, BIT5
#define GPIO156_OUT 0x10257c, BIT4
#define GPIO156_IN  0x10257c, BIT0

#define GPIO157_PAD PAD_EMMC_IO7
#define GPIO157_OEN 0x10257e, BIT5
#define GPIO157_OUT 0x10257e, BIT4
#define GPIO157_IN  0x10257e, BIT0

#define GPIO158_PAD PAD_EMMC_IO8
#define GPIO158_OEN 0x102580, BIT5
#define GPIO158_OUT 0x102580, BIT4
#define GPIO158_IN  0x102580, BIT0

#define GPIO159_PAD PAD_EMMC_IO9
#define GPIO159_OEN 0x102582, BIT5
#define GPIO159_OUT 0x102582, BIT4
#define GPIO159_IN  0x102582, BIT0

#define GPIO160_PAD PAD_EMMC_IO10
#define GPIO160_OEN 0x102584, BIT5
#define GPIO160_OUT 0x102584, BIT4
#define GPIO160_IN  0x102584, BIT0

#define GPIO161_PAD PAD_EMMC_IO11
#define GPIO161_OEN 0x102586, BIT5
#define GPIO161_OUT 0x102586, BIT4
#define GPIO161_IN  0x102586, BIT0

#define GPIO162_PAD PAD_EMMC_IO12
#define GPIO162_OEN 0x102588, BIT5
#define GPIO162_OUT 0x102588, BIT4
#define GPIO162_IN  0x102588, BIT0

#define GPIO163_PAD PAD_EMMC_IO13
#define GPIO163_OEN 0x10258a, BIT5
#define GPIO163_OUT 0x10258a, BIT4
#define GPIO163_IN  0x10258a, BIT0

#define GPIO164_PAD PAD_EMMC_IO14
#define GPIO164_OEN 0x10258c, BIT5
#define GPIO164_OUT 0x10258c, BIT4
#define GPIO164_IN  0x10258c, BIT0

#define GPIO165_PAD PAD_EMMC_IO15
#define GPIO165_OEN 0x10258e, BIT5
#define GPIO165_OUT 0x10258e, BIT4
#define GPIO165_IN  0x10258e, BIT0

#define GPIO166_PAD PAD_EMMC_IO16
#define GPIO166_OEN 0x102590, BIT5
#define GPIO166_OUT 0x102590, BIT4
#define GPIO166_IN  0x102590, BIT0

#define GPIO167_PAD PAD_EMMC_IO17
#define GPIO167_OEN 0x102592, BIT5
#define GPIO167_OUT 0x102592, BIT4
#define GPIO167_IN  0x102592, BIT0

#define GPIO168_PAD PAD_DM_P0
#define GPIO168_OEN 0x103a8a, BIT4 
#define GPIO168_OUT 0x103a8a, BIT2 
#define GPIO168_IN  0x103ab1, BIT5

#define GPIO169_PAD PAD_DP_P0
#define GPIO169_OEN 0x103a8a, BIT5
#define GPIO169_OUT 0x103a8a, BIT3
#define GPIO169_IN  0x103ab1, BIT4

#define GPIO170_PAD PAD_DM_P1
#define GPIO170_OEN 0x103a0a, BIT4 
#define GPIO170_OUT 0x103a0a, BIT2
#define GPIO170_IN  0x103a31, BIT5

#define GPIO171_PAD PAD_DP_P1
#define GPIO171_OEN 0x103a0a, BIT5
#define GPIO171_OUT 0x103a0a, BIT3
#define GPIO171_IN  0x103a31, BIT4

#define GPIO172_PAD PAD_DM_P2
#define GPIO172_OEN 0x10380a, BIT4 
#define GPIO172_OUT 0x10380a, BIT2
#define GPIO172_IN  0x103831, BIT5

#define GPIO173_PAD PAD_DP_P2
#define GPIO173_OEN 0x10380a, BIT5
#define GPIO173_OUT 0x10380a, BIT3
#define GPIO173_IN  0x103831, BIT4


#define GPIO174_PAD PADA_TSIO_OUTN_CH0
#define GPIO174_OEN 0x171f02, BIT2
#define GPIO174_OUT 0x171f03, BIT0
#define GPIO174_IN  0x171f1b, BIT0

#define GPIO175_PAD PADA_TSIO_OUTP_CH0
#define GPIO175_OEN 0x171f02, BIT0
#define GPIO175_OUT 0x171f03, BIT2
#define GPIO175_IN  0x171f1b, BIT2

#define GPIO176_PAD PADA_TSIO_OUTN_CH1
#define GPIO176_OEN 0x171f02, BIT3
#define GPIO176_OUT 0x171f03, BIT1
#define GPIO176_IN  0x171f1b, BIT1

#define GPIO177_PAD PADA_TSIO_OUTP_CH1
#define GPIO177_OEN 0x171f02, BIT1
#define GPIO177_OUT 0x171f03, BIT3
#define GPIO177_IN  0x171f1b, BIT3

#define GPIO178_PAD PADA_TSIO_INN
#define GPIO178_OEN 0x171f28, BIT1
#define GPIO178_OUT 0x171f28, BIT0
#define GPIO178_IN  0x171f22, BIT3

#define GPIO179_PAD PADA_TSIO_INP
#define GPIO179_OEN 0x171f28, BIT3
#define GPIO179_OUT 0x171f28, BIT2
#define GPIO179_IN  0x171f22, BIT4


//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static const struct gpio_setting
{
    U32 r_oen;
    U8  m_oen;
    U32 r_out;
    U8  m_out;
    U32 r_in;
    U8  m_in;
} gpio_table[] =
{
#define __GPIO__(_x_)   { CONCAT(CONCAT(GPIO, _x_), _OEN),   \
                          CONCAT(CONCAT(GPIO, _x_), _OUT),   \
                          CONCAT(CONCAT(GPIO, _x_), _IN) }
#define __GPIO(_x_)     __GPIO__(_x_)

//
// !! WARNING !! DO NOT MODIFIY !!!!
//
// These defines order must match following
// 1. the PAD name in GPIO excel
// 2. the perl script to generate the package header file
//
    //__GPIO(999), // 0 is not used

    __GPIO(0), __GPIO(1), __GPIO(2), __GPIO(3), __GPIO(4),
    __GPIO(5), __GPIO(6), __GPIO(7), __GPIO(8), __GPIO(9),
    __GPIO(10), __GPIO(11), __GPIO(12), __GPIO(13), __GPIO(14),
    __GPIO(15), __GPIO(16), __GPIO(17), __GPIO(18), __GPIO(19),
    __GPIO(20), __GPIO(21), __GPIO(22), __GPIO(23), __GPIO(24),
    __GPIO(25), __GPIO(26), __GPIO(27), __GPIO(28), __GPIO(29),
    __GPIO(30), __GPIO(31), __GPIO(32), __GPIO(33), __GPIO(34),
    __GPIO(35), __GPIO(36), __GPIO(37), __GPIO(38), __GPIO(39),
    __GPIO(40), __GPIO(41), __GPIO(42), __GPIO(43), __GPIO(44),
    __GPIO(45), __GPIO(46), __GPIO(47), __GPIO(48), __GPIO(49),
    __GPIO(50), __GPIO(51), __GPIO(52), __GPIO(53), __GPIO(54),
    __GPIO(55), __GPIO(56), __GPIO(57), __GPIO(58), __GPIO(59),
    __GPIO(60), __GPIO(61), __GPIO(62), __GPIO(63), __GPIO(64),
    __GPIO(65), __GPIO(66), __GPIO(67), __GPIO(68), __GPIO(69),
    __GPIO(70), __GPIO(71), __GPIO(72), __GPIO(73), __GPIO(74),
    __GPIO(75), __GPIO(76), __GPIO(77), __GPIO(78), __GPIO(79),
    __GPIO(80), __GPIO(81), __GPIO(82), __GPIO(83), __GPIO(84),
    __GPIO(85), __GPIO(86), __GPIO(87), __GPIO(88), __GPIO(89),
    __GPIO(90), __GPIO(91), __GPIO(92), __GPIO(93), __GPIO(94),
    __GPIO(95), __GPIO(96), __GPIO(97), __GPIO(98), __GPIO(99),
    __GPIO(100), __GPIO(101), __GPIO(102), __GPIO(103), __GPIO(104),
    __GPIO(105), __GPIO(106), __GPIO(107), __GPIO(108), __GPIO(109),
    __GPIO(110), __GPIO(111), __GPIO(112), __GPIO(113), __GPIO(114),
    __GPIO(115), __GPIO(116), __GPIO(117), __GPIO(118), __GPIO(119),
    __GPIO(120), __GPIO(121), __GPIO(122), __GPIO(123), __GPIO(124),
    __GPIO(125), __GPIO(126), __GPIO(127), __GPIO(128), __GPIO(129),
    __GPIO(130), __GPIO(131), __GPIO(132), __GPIO(133), __GPIO(134),
    __GPIO(135), __GPIO(136), __GPIO(137), __GPIO(138), __GPIO(139),
    __GPIO(140), __GPIO(141), __GPIO(142), __GPIO(143), __GPIO(144),
    __GPIO(145), __GPIO(146), __GPIO(147), __GPIO(148), __GPIO(149),
    __GPIO(150), __GPIO(151), __GPIO(152), __GPIO(153), __GPIO(154),
    __GPIO(155), __GPIO(156), __GPIO(157), __GPIO(158), __GPIO(159),
    __GPIO(160), __GPIO(161), __GPIO(162), __GPIO(163), __GPIO(164),
    __GPIO(165), __GPIO(166), __GPIO(167), __GPIO(168), __GPIO(169),
    __GPIO(170), __GPIO(171), __GPIO(172), __GPIO(173), __GPIO(174),
    __GPIO(175), __GPIO(176), __GPIO(177), __GPIO(178), __GPIO(179)
};

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//the functions of this section set to initialize
void MHal_GPIO_Init(void)
{
    MHal_GPIO_REG(REG_ALL_PAD_IN) &= ~BIT7;
}

void MHal_GPIO_WriteRegBit(U32 u32Reg, U8 u8Enable, U8 u8BitMsk)
{
    if(u8Enable)
        MHal_GPIO_REG(u32Reg) |= u8BitMsk;
    else
        MHal_GPIO_REG(u32Reg) &= (~u8BitMsk);
}

U8 MHal_GPIO_ReadRegBit(U32 u32Reg, U8 u8BitMsk)
{
    return ((MHal_GPIO_REG(u32Reg)&u8BitMsk)? 1 : 0);
}

void MHal_GPIO_Pad_Set(U8 u8IndexGPIO)
{
    switch (u8IndexGPIO)
    {
        case PAD_PM_IRIN:
            MHal_GPIO_WriteRegBit(REG_IRIN_00, ENABLE, BIT4);
            break;
        case PAD_PM_SPI_CZ:
            MHal_GPIO_WriteRegBit(REG_SPI_00, ENABLE, BIT1);
            break;
        case PAD_PM_SPI_CK:
        case PAD_PM_SPI_DI:
        case PAD_PM_SPI_DO:
            MHal_GPIO_WriteRegBit(REG_SPI_00, ENABLE, BIT0);
            break;
        case PAD_PM_SPI_WPZ:    
            MHal_GPIO_WriteRegBit(REG_SPI_00, ENABLE, BIT4);
            break;
        case PAD_PM_SPI_HOLDZ:
            MHal_GPIO_WriteRegBit(REG_SPI_00, DISABLE, BIT6|BIT7);
            break;
        case PAD_PM_GPIO0:
            MHal_GPIO_WriteRegBit(REG_SPI_00, ENABLE, BIT2);
            MHal_GPIO_WriteRegBit(REG_SPI_00, DISABLE, BIT6|BIT7);
            MHal_GPIO_WriteRegBit(REG_PMGPIO_02, DISABLE, BIT0|BIT1);
            MHal_GPIO_WriteRegBit(REG_PMGPIO_02, DISABLE, BIT6|BIT7);
            MHal_GPIO_WriteRegBit(REG_PMGPIO_03, DISABLE, BIT2|BIT3);
            MHal_GPIO_WriteRegBit(REG_PMGPIO_03, DISABLE, BIT4|BIT5);
            break;
        case PAD_PM_GPIO1:
            MHal_GPIO_WriteRegBit(REG_SPI_00, DISABLE, BIT6|BIT7);
            MHal_GPIO_WriteRegBit(REG_PMGPIO_02, DISABLE, BIT2|BIT3);
            break;
        case PAD_PM_GPIO3:
            MHal_GPIO_WriteRegBit(REG_PMGPIO_02, DISABLE, BIT6|BIT7);
            MHal_GPIO_WriteRegBit(REG_PMGPIO_03, DISABLE, BIT2|BIT3);
            MHal_GPIO_WriteRegBit(REG_PMGPIO_03, DISABLE, BIT4|BIT5);
            break;
        case PAD_PM_GPIO4:
            MHal_GPIO_WriteRegBit(REG_PMGPIO_00, DISABLE, BIT0|BIT6);
            MHal_GPIO_WriteRegBit(REG_PMGPIO_00, ENABLE, BIT1|BIT2|BIT3|BIT4|BIT5|BIT7);
            MHal_GPIO_WriteRegBit(REG_PMGPIO_01, DISABLE, BIT0|BIT2|BIT6);
            MHal_GPIO_WriteRegBit(REG_PMGPIO_00, ENABLE, BIT1|BIT3|BIT4|BIT5|BIT7);
            break;
        case PAD_PM_GPIO6:
            MHal_GPIO_WriteRegBit(REG_PMGPIO_02, DISABLE, BIT0|BIT1);
            break;
        case PAD_PM_GPIO8:
            MHal_GPIO_WriteRegBit(REG_SPI_00, ENABLE, BIT3);
            break;
        case PAD_PM_GPIO12:
        case PAD_PM_GPIO13:
        case PAD_PM_GPIO14:
        case PAD_PM_GPIO15:
            MHal_GPIO_WriteRegBit(REG_PMGPIO_03, DISABLE, BIT7);
            break;
        case PAD_PM_CEC:
            MHal_GPIO_WriteRegBit(REG_IRIN_00, ENABLE, BIT6);
            break;
        case PAD_HDMITX_HPD:
            MHal_GPIO_WriteRegBit(REG_HPD_00, DISABLE, BIT7);
            break;
        case PAD_VID0:
        case PAD_VID1:    
            MHal_GPIO_WriteRegBit(REG_PMGPIO_02, DISABLE, BIT6|BIT7);
            MHal_GPIO_WriteRegBit(REG_PMGPIO_03, DISABLE, BIT2|BIT3|BIT4|BIT5);
            break;
        case PAD_PM_GT0_MDIO:
        case PAD_PM_GT0_MDC:
        case PAD_PM_GT0_RX_CLK:
        case PAD_PM_GT0_RX_CTL:
        case PAD_PM_GT0_RX_D0:
        case PAD_PM_GT0_RX_D1:
        case PAD_PM_GT0_RX_D2:
        case PAD_PM_GT0_RX_D3:
            MHal_GPIO_WriteRegBit(REG_GT0_00, ENABLE, BIT0);
            break;
        case PAD_PM_GT0_TX_CLK:
        case PAD_PM_GT0_TX_CTL:
        case PAD_PM_GT0_TX_D0:
        case PAD_PM_GT0_TX_D1:
        case PAD_PM_GT0_TX_D2:
        case PAD_PM_GT0_TX_D3:
            MHal_GPIO_WriteRegBit(REG_GT0_00, ENABLE, BIT0);
            MHal_GPIO_WriteRegBit(REG_GT0_01, DISABLE, BIT2);
            break;
        case PAD_PM_LED0:
        case PAD_PM_LED1:
            MHal_GPIO_WriteRegBit(REG_PMGPIO_02, DISABLE, BIT4|BIT5);
            break;
        case PAD_SAR_GPIO0:
            MHal_GPIO_WriteRegBit(REG_SAR_00, DISABLE, BIT0);            
            break;
        case PAD_SAR_GPIO1:
            MHal_GPIO_WriteRegBit(REG_SAR_00, DISABLE, BIT1);            
            break;
        case PAD_SAR_GPIO2:
            MHal_GPIO_WriteRegBit(REG_SAR_00, DISABLE, BIT2);            
            break;
        case PAD_VPLUG_DET:
            MHal_GPIO_WriteRegBit(REG_SAR_00, DISABLE, BIT5);            
            break;
        case PAD_HSYNC_OUT:
        case PAD_VSYNC_OUT:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_02, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_03, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_04, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_05, DISABLE, BIT0);   
            break;
        case PAD_GPIO0:
        case PAD_GPIO1:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_02, DISABLE, BIT6|BIT7);   
            MHal_GPIO_WriteRegBit(REG_GPIO_00, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_GPIO_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_04, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_GPIO_02, DISABLE, BIT0|BIT1|BIT2|BIT3);   
            break;
        case PAD_GPIO2:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_02, DISABLE, BIT6|BIT7);   
            MHal_GPIO_WriteRegBit(REG_GPIO_00, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_GPIO_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_04, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_GPIO_02, DISABLE, BIT0|BIT1|BIT2|BIT3);   
            break;
        case PAD_GPIO3:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_02, DISABLE, BIT6|BIT7);   
            MHal_GPIO_WriteRegBit(REG_GPIO_00, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_GPIO_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_04, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_GPIO_02, DISABLE, BIT2|BIT3|BIT5|BIT6|BIT7);   
            break;
        case PAD_GPIO4:
        case PAD_GPIO5:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_GPIO_03, DISABLE, BIT2|BIT3);   
            MHal_GPIO_WriteRegBit(REG_GPIO_03, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_GPIO_02, DISABLE, BIT5|BIT6);   
            break;
        case PAD_GPIO6:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_GPIO_04, DISABLE, BIT4);   
            MHal_GPIO_WriteRegBit(REG_GPIO_02, DISABLE, BIT4);
            break;
        case PAD_GPIO7:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_GPIO_02, DISABLE, BIT4);
            break;        
        case PAD_GPIO8:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_GPIO_05, DISABLE, BIT4|BIT5);
            MHal_GPIO_WriteRegBit(REG_HSYNC_02, DISABLE, BIT2|BIT3);   
            MHal_GPIO_WriteRegBit(REG_GPIO_02, DISABLE, BIT4);   
            MHal_GPIO_WriteRegBit(REG_GPIO_06, DISABLE, BIT0);   
            break;
        case PAD_GPIO9:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_GPIO_05, DISABLE, BIT0|BIT1);
            MHal_GPIO_WriteRegBit(REG_HSYNC_02, DISABLE, BIT2|BIT3);   
            MHal_GPIO_WriteRegBit(REG_GPIO_02, DISABLE, BIT4);   
            MHal_GPIO_WriteRegBit(REG_GPIO_06, DISABLE, BIT1);   
            break;
        case PAD_GPIO119:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_GPIO_07, DISABLE, BIT0|BIT1);
            MHal_GPIO_WriteRegBit(REG_GPIO_08, DISABLE, BIT6|BIT7);   
            MHal_GPIO_WriteRegBit(REG_GPIO_02, DISABLE, BIT0|BIT1|BIT2|BIT3);   
            break;       
        case PAD_GPIO120:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_GPIO_07, DISABLE, BIT0|BIT1);
            MHal_GPIO_WriteRegBit(REG_GPIO_03, DISABLE, BIT2|BIT3);   
            MHal_GPIO_WriteRegBit(REG_GPIO_08, DISABLE, BIT6|BIT7);   
            MHal_GPIO_WriteRegBit(REG_GPIO_02, DISABLE, BIT0|BIT1|BIT2|BIT3);   
            break;            
        case PAD_GPIO121:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_GPIO_07, DISABLE, BIT0|BIT1);
            MHal_GPIO_WriteRegBit(REG_GPIO_08, DISABLE, BIT6|BIT7);   
            MHal_GPIO_WriteRegBit(REG_GPIO_02, DISABLE, BIT0|BIT1|BIT2|BIT3);   
            break;
        case PAD_GPIO125:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_03, DISABLE, BIT0|BIT1|BIT4|BIT5);
            MHal_GPIO_WriteRegBit(REG_GPIO_00, DISABLE, BIT0|BIT1);   
            break;
        case PAD_GPIO126:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_GPIO_07, DISABLE, BIT0|BIT1);
            MHal_GPIO_WriteRegBit(REG_HSYNC_03, DISABLE, BIT0|BIT1|BIT4|BIT5);
            break;
        case PAD_GPIO140:
        case PAD_GPIO143:
        case PAD_GPIO146:
        case PAD_GPIO147:
        case PAD_GPIO148:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_GPIO_07, DISABLE, BIT4|BIT5|BIT6);
            MHal_GPIO_WriteRegBit(REG_GPIO_01, DISABLE, BIT2|BIT3);
            break;
        case PAD_GPIO144:
        case PAD_GPIO145:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_GPIO_07, DISABLE, BIT4|BIT5|BIT6);
            break;
        case PAD_SPDIF_OUT:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_GPIO_05, DISABLE, BIT4|BIT5);
            MHal_GPIO_WriteRegBit(REG_GPIO_07, DISABLE, BIT3|BIT4);
            break;
        case PAD_I2CM0_SCL:
        case PAD_I2CM0_SDA:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT0|BIT1);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_01, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_02, DISABLE, BIT0|BIT1); 
            MHal_GPIO_WriteRegBit(REG_HSYNC_03, DISABLE, BIT0|BIT1);
            MHal_GPIO_WriteRegBit(REG_GPIO_08, DISABLE, BIT6|BIT7);
            break;
        case PAD_I2CM1_SCL:
        case PAD_I2CM1_SDA:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_02, DISABLE, BIT2|BIT3);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_03, DISABLE, BIT4|BIT5);   
            MHal_GPIO_WriteRegBit(REG_GPIO_08, DISABLE, BIT6|BIT7); 
            break;
        case PAD_HDMITX_SCL:
        case PAD_HDMITX_SDA:
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_HSYNC_02, DISABLE, BIT4|BIT5);
            MHal_GPIO_WriteRegBit(REG_HSYNC_03, DISABLE, BIT4|BIT5);  
            MHal_GPIO_WriteRegBit(REG_GPIO_09, DISABLE, BIT4);
            break;
        case PAD_EMMC_IO0:   
        case PAD_EMMC_IO1: 
        case PAD_EMMC_IO2: 
        case PAD_EMMC_IO3:
        case PAD_EMMC_IO4:
        case PAD_EMMC_IO5:
        case PAD_EMMC_IO6:
        case PAD_EMMC_IO7:
        case PAD_EMMC_IO9:  //PAD_NAND_WEZ
        case PAD_EMMC_IO10: //PAD_NAND_CLE
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_NAND_D00, DISABLE, BIT0|BIT1|BIT2);
            MHal_GPIO_WriteRegBit(REG_NAND_D00, DISABLE, BIT3|BIT4|BIT5);
            break;
        case PAD_EMMC_IO11: //PAD_NAND_WPZ
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_NAND_D00, DISABLE, BIT0|BIT1|BIT2);
            MHal_GPIO_WriteRegBit(REG_NAND_D00, DISABLE, BIT3|BIT4|BIT5);
            MHal_GPIO_WriteRegBit(REG_NAND_D01, DISABLE, BIT5);
            break;
        case PAD_EMMC_IO12: //PAD_NAND_RBZ
        case PAD_EMMC_IO13: //PAD_NAND_CEZ
        case PAD_EMMC_IO14: //PAD_NAND_REZ
        case PAD_EMMC_IO15: //PAD_NAND_ALE
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7);   
            MHal_GPIO_WriteRegBit(REG_NAND_D00, DISABLE, BIT3|BIT4|BIT5);
            MHal_GPIO_WriteRegBit(REG_GPIO_03, DISABLE, BIT6);   
            break;
        case PAD_EMMC_IO16: //PAD_NAND_CEZ1
            MHal_GPIO_WriteRegBit(REG_HSYNC_00, DISABLE, BIT7); 
            MHal_GPIO_WriteRegBit(REG_NAND_D00, DISABLE, BIT6);
            break;
        case PAD_DM_P0:
        case PAD_DP_P0:    
            MHal_GPIO_WriteRegBit(REG_DMDP_P00, ENABLE, BIT6); 
            MHal_GPIO_WriteRegBit(REG_DMDP_P01, ENABLE, BIT0); 
            MHal_GPIO_WriteRegBit(REG_DMDP_P02, DISABLE, BIT4); 
            MHal_GPIO_WriteRegBit(REG_DMDP_P03, ENABLE, BIT0|BIT1|BIT2|BIT3); 
            MHal_GPIO_WriteRegBit(REG_DMDP_P04, ENABLE, BIT2); 
            break;
        case PAD_DM_P1:
        case PAD_DP_P1:
            MHal_GPIO_WriteRegBit(REG_DMDP_P09, ENABLE, BIT6); 
            MHal_GPIO_WriteRegBit(REG_DMDP_P05, ENABLE, BIT0); 
            MHal_GPIO_WriteRegBit(REG_DMDP_P06, DISABLE, BIT4); 
            MHal_GPIO_WriteRegBit(REG_DMDP_P07, ENABLE, BIT0|BIT1|BIT2|BIT3); 
            MHal_GPIO_WriteRegBit(REG_DMDP_P08, ENABLE, BIT2); 
            break;
        default:
            break;
            
    }
}
void MHal_GPIO_Pad_Oen(U8 u8IndexGPIO)
{
    MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_oen) &= (~gpio_table[u8IndexGPIO].m_oen);
}

void MHal_GPIO_Pad_Odn(U8 u8IndexGPIO)
{
    MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_oen) |= gpio_table[u8IndexGPIO].m_oen;
}

U8 MHal_GPIO_Pad_Level(U8 u8IndexGPIO)
{
    return ((MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_in)&gpio_table[u8IndexGPIO].m_in)? 1 : 0);
}

U8 MHal_GPIO_Pad_InOut(U8 u8IndexGPIO)
{
    return ((MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_oen)&gpio_table[u8IndexGPIO].m_oen)? 1 : 0);
}

void MHal_GPIO_Pull_High(U8 u8IndexGPIO)
{
    MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_out) |= gpio_table[u8IndexGPIO].m_out;
}

void MHal_GPIO_Pull_Low(U8 u8IndexGPIO)
{
    MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_out) &= (~gpio_table[u8IndexGPIO].m_out);
}

void MHal_GPIO_Set_High(U8 u8IndexGPIO)
{
    MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_oen) &= (~gpio_table[u8IndexGPIO].m_oen);
    MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_out) |= gpio_table[u8IndexGPIO].m_out;
}

void MHal_GPIO_Set_Low(U8 u8IndexGPIO)
{
    MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_oen) &= (~gpio_table[u8IndexGPIO].m_oen);
    MHal_GPIO_REG(gpio_table[u8IndexGPIO].r_out) &= (~gpio_table[u8IndexGPIO].m_out);
}
