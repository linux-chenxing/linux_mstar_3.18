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

#define GPIO0_PAD PAD_PM_SPI_CZ
#define GPIO0_OEN 0x0f2eUL, BIT0
#define GPIO0_OUT 0x0f2eUL, BIT1
#define GPIO0_IN  0x0f2eUL, BIT2

#define GPIO1_PAD PAD_PM_SPI_CK
#define GPIO1_OEN 0x0f30UL, BIT0
#define GPIO1_OUT 0x0f30UL, BIT1
#define GPIO1_IN  0x0f30UL, BIT2

#define GPIO2_PAD PAD_PM_SPI_DI
#define GPIO2_OEN 0x0f32UL, BIT0
#define GPIO2_OUT 0x0f32UL, BIT1
#define GPIO2_IN  0x0f32UL, BIT2

#define GPIO3_PAD PAD_PM_SPI_DO
#define GPIO3_OEN 0x0f34UL, BIT0
#define GPIO3_OUT 0x0f34UL, BIT1
#define GPIO3_IN  0x0f34UL, BIT2

#define GPIO4_PAD PAD_IRIN
#define GPIO4_OEN 0x0f26UL, BIT0
#define GPIO4_OUT 0x0f26UL, BIT1
#define GPIO4_IN  0x0f26UL, BIT2

#define GPIO5_PAD PAD_CEC0
#define GPIO5_OEN 0x0f2aUL, BIT0
#define GPIO5_OUT 0x0f2aUL, BIT1
#define GPIO5_IN  0x0f2aUL, BIT2

#define GPIO6_PAD PAD_PWM_PM
#define GPIO6_OEN 0x0f28UL, BIT0
#define GPIO6_OUT 0x0f28UL, BIT1
#define GPIO6_IN  0x0f28UL, BIT2

#define GPIO7_PAD PAD_DDCA_CK
#define GPIO7_OEN 0x0494UL, BIT1
#define GPIO7_OUT 0x0494UL, BIT2
#define GPIO7_IN  0x0494UL, BIT0

#define GPIO8_PAD PAD_DDCA_DA
#define GPIO8_OEN 0x0494UL, BIT5
#define GPIO8_OUT 0x0494UL, BIT6
#define GPIO8_IN  0x0494UL, BIT4

#define GPIO9_PAD PAD_GPIO_PM0
#define GPIO9_OEN 0x0f00UL, BIT0
#define GPIO9_OUT 0x0f00UL, BIT1
#define GPIO9_IN  0x0f00UL, BIT2

#define GPIO10_PAD PAD_GPIO_PM1
#define GPIO10_OEN 0x0f02UL, BIT0
#define GPIO10_OUT 0x0f02UL, BIT1
#define GPIO10_IN  0x0f02UL, BIT2

#define GPIO11_PAD PAD_GPIO_PM2
#define GPIO11_OEN 0x0f04UL, BIT0
#define GPIO11_OUT 0x0f04UL, BIT1
#define GPIO11_IN  0x0f04UL, BIT2

#define GPIO12_PAD PAD_GPIO_PM3
#define GPIO12_OEN 0x0f06UL, BIT0
#define GPIO12_OUT 0x0f06UL, BIT1
#define GPIO12_IN  0x0f06UL, BIT2

#define GPIO13_PAD PAD_GPIO_PM4
#define GPIO13_OEN 0x0f08UL, BIT0
#define GPIO13_OUT 0x0f08UL, BIT1
#define GPIO13_IN  0x0f08UL, BIT2

#define GPIO14_PAD PAD_GPIO_PM5
#define GPIO14_OEN 0x0f0aUL, BIT0
#define GPIO14_OUT 0x0f0aUL, BIT1
#define GPIO14_IN  0x0f0aUL, BIT2

#define GPIO15_PAD PAD_GPIO_PM6
#define GPIO15_OEN 0x0f0cUL, BIT0
#define GPIO15_OUT 0x0f0cUL, BIT1
#define GPIO15_IN  0x0f0cUL, BIT2

#define GPIO16_PAD PAD_GPIO_PM9
#define GPIO16_OEN 0x0f12UL, BIT0
#define GPIO16_OUT 0x0f12UL, BIT1
#define GPIO16_IN  0x0f12UL, BIT2

#define GPIO17_PAD PAD_GPIO_PM10
#define GPIO17_OEN 0x0f14UL, BIT0
#define GPIO17_OUT 0x0f14UL, BIT1
#define GPIO17_IN  0x0f14UL, BIT2

#define GPIO18_PAD PAD_GPIO_PM11
#define GPIO18_OEN 0x0f16UL, BIT0
#define GPIO18_OUT 0x0f16UL, BIT1
#define GPIO18_IN  0x0f16UL, BIT2

#define GPIO19_PAD PAD_GPIO_PM12
#define GPIO19_OEN 0x0f18UL, BIT0
#define GPIO19_OUT 0x0f18UL, BIT1
#define GPIO19_IN  0x0f18UL, BIT2

#define GPIO20_PAD PAD_GPIO_PM13
#define GPIO20_OEN 0x0f1aUL, BIT0
#define GPIO20_OUT 0x0f1aUL, BIT1
#define GPIO20_IN  0x0f1aUL, BIT2

#define GPIO21_PAD PAD_GPIO_PM14
#define GPIO21_OEN 0x0f1cUL, BIT0
#define GPIO21_OUT 0x0f1cUL, BIT1
#define GPIO21_IN  0x0f1cUL, BIT2

#define GPIO22_PAD PAD_GPIO_PM15
#define GPIO22_OEN 0x0f1eUL, BIT0
#define GPIO22_OUT 0x0f1eUL, BIT1
#define GPIO22_IN  0x0f1eUL, BIT2

#define GPIO23_PAD PAD_GPIO_PM16
#define GPIO23_OEN 0x0f20UL, BIT0
#define GPIO23_OUT 0x0f20UL, BIT1
#define GPIO23_IN  0x0f20UL, BIT2

#define GPIO24_PAD PAD_LED0
#define GPIO24_OEN 0x2e80UL, BIT1
#define GPIO24_OUT 0x2e80UL, BIT0
#define GPIO24_IN  0x2e80UL, BIT2

#define GPIO25_PAD PAD_LED1
#define GPIO25_OEN 0x2e81UL, BIT1
#define GPIO25_OUT 0x2e81UL, BIT0
#define GPIO25_IN  0x2e81UL, BIT2

#define GPIO26_PAD PAD_HOTPLUGB
#define GPIO26_OEN 0x0e4eUL, BIT1
#define GPIO26_OUT 0x0e4eUL, BIT5
#define GPIO26_IN  0x0e4fUL, BIT1

#define GPIO27_PAD PAD_HOTPLUGC
#define GPIO27_OEN 0x0e4eUL, BIT2
#define GPIO27_OUT 0x0e4eUL, BIT6
#define GPIO27_IN  0x0e4fUL, BIT2

#define GPIO28_PAD PAD_HOTPLUGD
#define GPIO28_OEN 0x0e4eUL, BIT3
#define GPIO28_OUT 0x0e4eUL, BIT7
#define GPIO28_IN  0x0e4fUL, BIT3

#define GPIO29_PAD PAD_HOTPLUGA_HDMI20_5V
#define GPIO29_OEN 0x010218UL, BIT5
#define GPIO29_OUT 0x010218UL, BIT4
#define GPIO29_IN  0x010218UL, BIT6

#define GPIO30_PAD PAD_HOTPLUGB_HDMI20_5V
#define GPIO30_OEN 0x010318UL, BIT5
#define GPIO30_OUT 0x010318UL, BIT4
#define GPIO30_IN  0x010318UL, BIT6

#define GPIO31_PAD PAD_HOTPLUGC_HDMI20_5V
#define GPIO31_OEN 0x010418UL, BIT5
#define GPIO31_OUT 0x010418UL, BIT4
#define GPIO31_IN  0x010418UL, BIT6

#define GPIO32_PAD PAD_HOTPLUGD_HDMI20_5V
#define GPIO32_OEN 0x010518UL, BIT5
#define GPIO32_OUT 0x010518UL, BIT4
#define GPIO32_IN  0x010518UL, BIT6

#define GPIO33_PAD PAD_DDCDA_CK
#define GPIO33_OEN 0x0496UL, BIT1
#define GPIO33_OUT 0x0496UL, BIT2
#define GPIO33_IN  0x0496UL, BIT0

#define GPIO34_PAD PAD_DDCDA_DA
#define GPIO34_OEN 0x0496UL, BIT5
#define GPIO34_OUT 0x0496UL, BIT6
#define GPIO34_IN  0x0496UL, BIT4

#define GPIO35_PAD PAD_DDCDB_CK
#define GPIO35_OEN 0x0497UL, BIT1
#define GPIO35_OUT 0x0497UL, BIT2
#define GPIO35_IN  0x0497UL, BIT0

#define GPIO36_PAD PAD_DDCDB_DA
#define GPIO36_OEN 0x0497UL, BIT5
#define GPIO36_OUT 0x0497UL, BIT6
#define GPIO36_IN  0x0497UL, BIT4

#define GPIO37_PAD PAD_DDCDC_CK
#define GPIO37_OEN 0x0498UL, BIT1
#define GPIO37_OUT 0x0498UL, BIT2
#define GPIO37_IN  0x0498UL, BIT0

#define GPIO38_PAD PAD_DDCDC_DA
#define GPIO38_OEN 0x0498UL, BIT5
#define GPIO38_OUT 0x0498UL, BIT6
#define GPIO38_IN  0x0498UL, BIT4

#define GPIO39_PAD PAD_DDCDD_CK
#define GPIO39_OEN 0x0499UL, BIT1
#define GPIO39_OUT 0x0499UL, BIT2
#define GPIO39_IN  0x0499UL, BIT0

#define GPIO40_PAD PAD_DDCDD_DA
#define GPIO40_OEN 0x0499UL, BIT5
#define GPIO40_OUT 0x0499UL, BIT6
#define GPIO40_IN  0x0499UL, BIT4

#define GPIO41_PAD PAD_SAR0
#define GPIO41_OEN 0x1423UL, BIT0
#define GPIO41_OUT 0x1424UL, BIT0
#define GPIO41_IN  0x1425UL, BIT0

#define GPIO42_PAD PAD_SAR1
#define GPIO42_OEN 0x1423UL, BIT1
#define GPIO42_OUT 0x1424UL, BIT1
#define GPIO42_IN  0x1425UL, BIT1

#define GPIO43_PAD PAD_SAR2
#define GPIO43_OEN 0x1423UL, BIT2
#define GPIO43_OUT 0x1424UL, BIT2
#define GPIO43_IN  0x1425UL, BIT2

#define GPIO44_PAD PAD_SAR3
#define GPIO44_OEN 0x1423UL, BIT3
#define GPIO44_OUT 0x1424UL, BIT3
#define GPIO44_IN  0x1425UL, BIT3

#define GPIO45_PAD PAD_SAR4
#define GPIO45_OEN 0x1423UL, BIT4
#define GPIO45_OUT 0x1424UL, BIT4
#define GPIO45_IN  0x1425UL, BIT4

#define GPIO46_PAD PAD_VPLGUIN
#define GPIO46_OEN 0x1423UL, BIT5
#define GPIO46_OUT 0x1424UL, BIT5
#define GPIO46_IN  0x1425UL, BIT5

#define GPIO47_PAD PAD_VID0
#define GPIO47_OEN 0x2e84UL, BIT1
#define GPIO47_OUT 0x2e84UL, BIT0
#define GPIO47_IN  0x2e84UL, BIT2

#define GPIO48_PAD PAD_VID1
#define GPIO48_OEN 0x2e85UL, BIT1
#define GPIO48_OUT 0x2e85UL, BIT0
#define GPIO48_IN  0x2e85UL, BIT2

#define GPIO49_PAD PAD_VID2
#define GPIO49_OEN 0x0f22UL, BIT0
#define GPIO49_OUT 0x0f22UL, BIT1
#define GPIO49_IN  0x0f22UL, BIT2

#define GPIO50_PAD PAD_VID3
#define GPIO50_OEN 0x0f24UL, BIT0
#define GPIO50_OUT 0x0f24UL, BIT1
#define GPIO50_IN  0x0f24UL, BIT2

#define GPIO51_PAD PAD_VID4
#define GPIO51_OEN 0x0f0eUL, BIT0
#define GPIO51_OUT 0x0f0eUL, BIT1
#define GPIO51_IN  0x0f0eUL, BIT2

#define GPIO52_PAD PAD_VID5
#define GPIO52_OEN 0x0f10UL, BIT0
#define GPIO52_OUT 0x0f10UL, BIT1
#define GPIO52_IN  0x0f10UL, BIT2

#define GPIO53_PAD PAD_WOL_INT_OUT
#define GPIO53_OEN 0x2e82UL, BIT1
#define GPIO53_OUT 0x2e82UL, BIT0
#define GPIO53_IN  0x2e82UL, BIT2

#define GPIO54_PAD PAD_DDCR_DA
#define GPIO54_OEN 0x102b86UL, BIT1
#define GPIO54_OUT 0x102b86UL, BIT0
#define GPIO54_IN  0x102b86UL, BIT2

#define GPIO55_PAD PAD_DDCR_CK
#define GPIO55_OEN 0x102b87UL, BIT1
#define GPIO55_OUT 0x102b87UL, BIT0
#define GPIO55_IN  0x102b87UL, BIT2

#define GPIO56_PAD PAD_GPIO0
#define GPIO56_OEN 0x102b00UL, BIT1
#define GPIO56_OUT 0x102b00UL, BIT0
#define GPIO56_IN  0x102b00UL, BIT2

#define GPIO57_PAD PAD_GPIO1
#define GPIO57_OEN 0x102b01UL, BIT1
#define GPIO57_OUT 0x102b01UL, BIT0
#define GPIO57_IN  0x102b01UL, BIT2

#define GPIO58_PAD PAD_GPIO2
#define GPIO58_OEN 0x102b02UL, BIT1
#define GPIO58_OUT 0x102b02UL, BIT0
#define GPIO58_IN  0x102b02UL, BIT2

#define GPIO59_PAD PAD_GPIO3
#define GPIO59_OEN 0x102b03UL, BIT1
#define GPIO59_OUT 0x102b03UL, BIT0
#define GPIO59_IN  0x102b03UL, BIT2

#define GPIO60_PAD PAD_GPIO4
#define GPIO60_OEN 0x102b04UL, BIT1
#define GPIO60_OUT 0x102b04UL, BIT0
#define GPIO60_IN  0x102b04UL, BIT2

#define GPIO61_PAD PAD_GPIO5
#define GPIO61_OEN 0x102b05UL, BIT1
#define GPIO61_OUT 0x102b05UL, BIT0
#define GPIO61_IN  0x102b05UL, BIT2

#define GPIO62_PAD PAD_GPIO6
#define GPIO62_OEN 0x102b06UL, BIT1
#define GPIO62_OUT 0x102b06UL, BIT0
#define GPIO62_IN  0x102b06UL, BIT2

#define GPIO63_PAD PAD_GPIO7
#define GPIO63_OEN 0x102b07UL, BIT1
#define GPIO63_OUT 0x102b07UL, BIT0
#define GPIO63_IN  0x102b07UL, BIT2

#define GPIO64_PAD PAD_GPIO8
#define GPIO64_OEN 0x102b08UL, BIT1
#define GPIO64_OUT 0x102b08UL, BIT0
#define GPIO64_IN  0x102b08UL, BIT2

#define GPIO65_PAD PAD_GPIO16
#define GPIO65_OEN 0x102b10UL, BIT1
#define GPIO65_OUT 0x102b10UL, BIT0
#define GPIO65_IN  0x102b10UL, BIT2

#define GPIO66_PAD PAD_GPIO17
#define GPIO66_OEN 0x102b11UL, BIT1
#define GPIO66_OUT 0x102b11UL, BIT0
#define GPIO66_IN  0x102b11UL, BIT2

#define GPIO67_PAD PAD_GPIO18
#define GPIO67_OEN 0x102b12UL, BIT1
#define GPIO67_OUT 0x102b12UL, BIT0
#define GPIO67_IN  0x102b12UL, BIT2

#define GPIO68_PAD PAD_GPIO19
#define GPIO68_OEN 0x102b13UL, BIT1
#define GPIO68_OUT 0x102b13UL, BIT0
#define GPIO68_IN  0x102b13UL, BIT2

#define GPIO69_PAD PAD_GPIO20
#define GPIO69_OEN 0x102b14UL, BIT1
#define GPIO69_OUT 0x102b14UL, BIT0
#define GPIO69_IN  0x102b14UL, BIT2

#define GPIO70_PAD PAD_GPIO21
#define GPIO70_OEN 0x102b15UL, BIT1
#define GPIO70_OUT 0x102b15UL, BIT0
#define GPIO70_IN  0x102b15UL, BIT2

#define GPIO71_PAD PAD_GPIO22
#define GPIO71_OEN 0x102b16UL, BIT1
#define GPIO71_OUT 0x102b16UL, BIT0
#define GPIO71_IN  0x102b16UL, BIT2

#define GPIO72_PAD PAD_GPIO23
#define GPIO72_OEN 0x102b17UL, BIT1
#define GPIO72_OUT 0x102b17UL, BIT0
#define GPIO72_IN  0x102b17UL, BIT2

#define GPIO73_PAD PAD_GPIO24
#define GPIO73_OEN 0x102b18UL, BIT1
#define GPIO73_OUT 0x102b18UL, BIT0
#define GPIO73_IN  0x102b18UL, BIT2

#define GPIO74_PAD PAD_GPIO25
#define GPIO74_OEN 0x102b19UL, BIT1
#define GPIO74_OUT 0x102b19UL, BIT0
#define GPIO74_IN  0x102b19UL, BIT2

#define GPIO75_PAD PAD_GPIO26
#define GPIO75_OEN 0x102b1aUL, BIT1
#define GPIO75_OUT 0x102b1aUL, BIT0
#define GPIO75_IN  0x102b1aUL, BIT2

#define GPIO76_PAD PAD_GPIO27
#define GPIO76_OEN 0x102b1bUL, BIT1
#define GPIO76_OUT 0x102b1bUL, BIT0
#define GPIO76_IN  0x102b1bUL, BIT2

#define GPIO77_PAD PAD_GPIO28
#define GPIO77_OEN 0x102ba0UL, BIT1
#define GPIO77_OUT 0x102ba0UL, BIT0
#define GPIO77_IN  0x102ba0UL, BIT2

#define GPIO78_PAD PAD_GPIO29
#define GPIO78_OEN 0x102ba1UL, BIT1
#define GPIO78_OUT 0x102ba1UL, BIT0
#define GPIO78_IN  0x102ba1UL, BIT2

#define GPIO79_PAD PAD_GPIO30
#define GPIO79_OEN 0x102ba2UL, BIT1
#define GPIO79_OUT 0x102ba2UL, BIT0
#define GPIO79_IN  0x102ba2UL, BIT2

#define GPIO80_PAD PAD_GPIO31
#define GPIO80_OEN 0x102ba3UL, BIT1
#define GPIO80_OUT 0x102ba3UL, BIT0
#define GPIO80_IN  0x102ba3UL, BIT2

#define GPIO81_PAD PAD_GPIO32
#define GPIO81_OEN 0x102ba4UL, BIT1
#define GPIO81_OUT 0x102ba4UL, BIT0
#define GPIO81_IN  0x102ba4UL, BIT2

#define GPIO82_PAD PAD_GPIO33
#define GPIO82_OEN 0x102ba5UL, BIT1
#define GPIO82_OUT 0x102ba5UL, BIT0
#define GPIO82_IN  0x102ba5UL, BIT2

#define GPIO83_PAD PAD_GPIO34
#define GPIO83_OEN 0x102ba6UL, BIT1
#define GPIO83_OUT 0x102ba6UL, BIT0
#define GPIO83_IN  0x102ba6UL, BIT2

#define GPIO84_PAD PAD_GPIO35
#define GPIO84_OEN 0x102ba7UL, BIT1
#define GPIO84_OUT 0x102ba7UL, BIT0
#define GPIO84_IN  0x102ba7UL, BIT2

#define GPIO85_PAD PAD_GPIO36
#define GPIO85_OEN 0x102ba8UL, BIT1
#define GPIO85_OUT 0x102ba8UL, BIT0
#define GPIO85_IN  0x102ba8UL, BIT2

#define GPIO86_PAD PAD_GPIO37
#define GPIO86_OEN 0x102ba9UL, BIT1
#define GPIO86_OUT 0x102ba9UL, BIT0
#define GPIO86_IN  0x102ba9UL, BIT2

#define GPIO87_PAD PAD_I2S_IN_WS
#define GPIO87_OEN 0x102b36UL, BIT1
#define GPIO87_OUT 0x102b36UL, BIT0
#define GPIO87_IN  0x102b36UL, BIT2

#define GPIO88_PAD PAD_I2S_IN_BCK
#define GPIO88_OEN 0x102b37UL, BIT1
#define GPIO88_OUT 0x102b37UL, BIT0
#define GPIO88_IN  0x102b37UL, BIT2

#define GPIO89_PAD PAD_I2S_IN_SD
#define GPIO89_OEN 0x102b38UL, BIT1
#define GPIO89_OUT 0x102b38UL, BIT0
#define GPIO89_IN  0x102b38UL, BIT2

#define GPIO90_PAD PAD_SPDIF_IN
#define GPIO90_OEN 0x102b39UL, BIT1
#define GPIO90_OUT 0x102b39UL, BIT0
#define GPIO90_IN  0x102b39UL, BIT2

#define GPIO91_PAD PAD_SPDIF_OUT
#define GPIO91_OEN 0x102b3aUL, BIT1
#define GPIO91_OUT 0x102b3aUL, BIT0
#define GPIO91_IN  0x102b3aUL, BIT2

#define GPIO92_PAD PAD_I2S_OUT_WS
#define GPIO92_OEN 0x102b3bUL, BIT1
#define GPIO92_OUT 0x102b3bUL, BIT0
#define GPIO92_IN  0x102b3bUL, BIT2

#define GPIO93_PAD PAD_I2S_OUT_MCK
#define GPIO93_OEN 0x102b3cUL, BIT1
#define GPIO93_OUT 0x102b3cUL, BIT0
#define GPIO93_IN  0x102b3cUL, BIT2

#define GPIO94_PAD PAD_I2S_OUT_BCK
#define GPIO94_OEN 0x102b3dUL, BIT1
#define GPIO94_OUT 0x102b3dUL, BIT0
#define GPIO94_IN  0x102b3dUL, BIT2

#define GPIO95_PAD PAD_I2S_OUT_SD
#define GPIO95_OEN 0x102b3eUL, BIT1
#define GPIO95_OUT 0x102b3eUL, BIT0
#define GPIO95_IN  0x102b3eUL, BIT2

#define GPIO96_PAD PAD_I2S_OUT_SD1
#define GPIO96_OEN 0x102b3fUL, BIT1
#define GPIO96_OUT 0x102b3fUL, BIT0
#define GPIO96_IN  0x102b3fUL, BIT2

#define GPIO97_PAD PAD_I2S_OUT_SD2
#define GPIO97_OEN 0x102b40UL, BIT1
#define GPIO97_OUT 0x102b40UL, BIT0
#define GPIO97_IN  0x102b40UL, BIT2

#define GPIO98_PAD PAD_I2S_OUT_SD3
#define GPIO98_OEN 0x102b41UL, BIT1
#define GPIO98_OUT 0x102b41UL, BIT0
#define GPIO98_IN  0x102b41UL, BIT2

#define GPIO99_PAD PAD_VSYNC_LIKE
#define GPIO99_OEN 0x102bb0UL, BIT1
#define GPIO99_OUT 0x102bb0UL, BIT0
#define GPIO99_IN  0x102bb0UL, BIT2

#define GPIO100_PAD PAD_SPI1_CK
#define GPIO100_OEN 0x102bb1UL, BIT1
#define GPIO100_OUT 0x102bb1UL, BIT0
#define GPIO100_IN  0x102bb1UL, BIT2

#define GPIO101_PAD PAD_SPI1_DI
#define GPIO101_OEN 0x102bb2UL, BIT1
#define GPIO101_OUT 0x102bb2UL, BIT0
#define GPIO101_IN  0x102bb2UL, BIT2

#define GPIO102_PAD PAD_SPI2_CK
#define GPIO102_OEN 0x102bb3UL, BIT1
#define GPIO102_OUT 0x102bb3UL, BIT0
#define GPIO102_IN  0x102bb3UL, BIT2

#define GPIO103_PAD PAD_SPI2_DI
#define GPIO103_OEN 0x102bb4UL, BIT1
#define GPIO103_OUT 0x102bb4UL, BIT0
#define GPIO103_IN  0x102bb4UL, BIT2

#define GPIO104_PAD PAD_DIM0
#define GPIO104_OEN 0x102beeUL, BIT1
#define GPIO104_OUT 0x102beeUL, BIT0
#define GPIO104_IN  0x102beeUL, BIT2

#define GPIO105_PAD PAD_DIM1
#define GPIO105_OEN 0x102befUL, BIT1
#define GPIO105_OUT 0x102befUL, BIT0
#define GPIO105_IN  0x102befUL, BIT2

#define GPIO106_PAD PAD_DIM2
#define GPIO106_OEN 0x102bf0UL, BIT1
#define GPIO106_OUT 0x102bf0UL, BIT0
#define GPIO106_IN  0x102bf0UL, BIT2

#define GPIO107_PAD PAD_DIM3
#define GPIO107_OEN 0x102bf1UL, BIT1
#define GPIO107_OUT 0x102bf1UL, BIT0
#define GPIO107_IN  0x102bf1UL, BIT2

#define GPIO108_PAD PAD_PCM2_CE_N
#define GPIO108_OEN 0x102b63UL, BIT1
#define GPIO108_OUT 0x102b63UL, BIT0
#define GPIO108_IN  0x102b63UL, BIT2

#define GPIO109_PAD PAD_PCM2_IRQA_N
#define GPIO109_OEN 0x102b64UL, BIT1
#define GPIO109_OUT 0x102b64UL, BIT0
#define GPIO109_IN  0x102b64UL, BIT2

#define GPIO110_PAD PAD_PCM2_WAIT_N
#define GPIO110_OEN 0x102b65UL, BIT1
#define GPIO110_OUT 0x102b65UL, BIT0
#define GPIO110_IN  0x102b65UL, BIT2

#define GPIO111_PAD PAD_PCM2_RESET
#define GPIO111_OEN 0x102b66UL, BIT1
#define GPIO111_OUT 0x102b66UL, BIT0
#define GPIO111_IN  0x102b66UL, BIT2

#define GPIO112_PAD PAD_PCM2_CD_N
#define GPIO112_OEN 0x102b67UL, BIT1
#define GPIO112_OUT 0x102b67UL, BIT0
#define GPIO112_IN  0x102b67UL, BIT2

#define GPIO113_PAD PAD_PCM_D3
#define GPIO113_OEN 0x102b42UL, BIT1
#define GPIO113_OUT 0x102b42UL, BIT0
#define GPIO113_IN  0x102b42UL, BIT2

#define GPIO114_PAD PAD_PCM_D4
#define GPIO114_OEN 0x102b43UL, BIT1
#define GPIO114_OUT 0x102b43UL, BIT0
#define GPIO114_IN  0x102b43UL, BIT2

#define GPIO115_PAD PAD_PCM_D5
#define GPIO115_OEN 0x102b44UL, BIT1
#define GPIO115_OUT 0x102b44UL, BIT0
#define GPIO115_IN  0x102b44UL, BIT2

#define GPIO116_PAD PAD_PCM_D6
#define GPIO116_OEN 0x102b45UL, BIT1
#define GPIO116_OUT 0x102b45UL, BIT0
#define GPIO116_IN  0x102b45UL, BIT2

#define GPIO117_PAD PAD_PCM_D7
#define GPIO117_OEN 0x102b46UL, BIT1
#define GPIO117_OUT 0x102b46UL, BIT0
#define GPIO117_IN  0x102b46UL, BIT2

#define GPIO118_PAD PAD_PCM_CE_N
#define GPIO118_OEN 0x102b47UL, BIT1
#define GPIO118_OUT 0x102b47UL, BIT0
#define GPIO118_IN  0x102b47UL, BIT2

#define GPIO119_PAD PAD_PCM_A10
#define GPIO119_OEN 0x102b48UL, BIT1
#define GPIO119_OUT 0x102b48UL, BIT0
#define GPIO119_IN  0x102b48UL, BIT2

#define GPIO120_PAD PAD_PCM_OE_N
#define GPIO120_OEN 0x102b49UL, BIT1
#define GPIO120_OUT 0x102b49UL, BIT0
#define GPIO120_IN  0x102b49UL, BIT2

#define GPIO121_PAD PAD_PCM_A11
#define GPIO121_OEN 0x102b4aUL, BIT1
#define GPIO121_OUT 0x102b4aUL, BIT0
#define GPIO121_IN  0x102b4aUL, BIT2

#define GPIO122_PAD PAD_PCM_IORD_N
#define GPIO122_OEN 0x102b4bUL, BIT1
#define GPIO122_OUT 0x102b4bUL, BIT0
#define GPIO122_IN  0x102b4bUL, BIT2

#define GPIO123_PAD PAD_PCM_A9
#define GPIO123_OEN 0x102b4cUL, BIT1
#define GPIO123_OUT 0x102b4cUL, BIT0
#define GPIO123_IN  0x102b4cUL, BIT2

#define GPIO124_PAD PAD_PCM_IOWR_N
#define GPIO124_OEN 0x102b4dUL, BIT1
#define GPIO124_OUT 0x102b4dUL, BIT0
#define GPIO124_IN  0x102b4dUL, BIT2

#define GPIO125_PAD PAD_PCM_A8
#define GPIO125_OEN 0x102b4eUL, BIT1
#define GPIO125_OUT 0x102b4eUL, BIT0
#define GPIO125_IN  0x102b4eUL, BIT2

#define GPIO126_PAD PAD_PCM_A13
#define GPIO126_OEN 0x102b4fUL, BIT1
#define GPIO126_OUT 0x102b4fUL, BIT0
#define GPIO126_IN  0x102b4fUL, BIT2

#define GPIO127_PAD PAD_PCM_A14
#define GPIO127_OEN 0x102b50UL, BIT1
#define GPIO127_OUT 0x102b50UL, BIT0
#define GPIO127_IN  0x102b50UL, BIT2

#define GPIO128_PAD PAD_PCM_WE_N
#define GPIO128_OEN 0x102b51UL, BIT1
#define GPIO128_OUT 0x102b51UL, BIT0
#define GPIO128_IN  0x102b51UL, BIT2

#define GPIO129_PAD PAD_PCM_IRQA_N
#define GPIO129_OEN 0x102b52UL, BIT1
#define GPIO129_OUT 0x102b52UL, BIT0
#define GPIO129_IN  0x102b52UL, BIT2

#define GPIO130_PAD PAD_PCM_A12
#define GPIO130_OEN 0x102b53UL, BIT1
#define GPIO130_OUT 0x102b53UL, BIT0
#define GPIO130_IN  0x102b53UL, BIT2

#define GPIO131_PAD PAD_PCM_A7
#define GPIO131_OEN 0x102b54UL, BIT1
#define GPIO131_OUT 0x102b54UL, BIT0
#define GPIO131_IN  0x102b54UL, BIT2

#define GPIO132_PAD PAD_PCM_A6
#define GPIO132_OEN 0x102b55UL, BIT1
#define GPIO132_OUT 0x102b55UL, BIT0
#define GPIO132_IN  0x102b55UL, BIT2

#define GPIO133_PAD PAD_PCM_A5
#define GPIO133_OEN 0x102b56UL, BIT1
#define GPIO133_OUT 0x102b56UL, BIT0
#define GPIO133_IN  0x102b56UL, BIT2

#define GPIO134_PAD PAD_PCM_WAIT_N
#define GPIO134_OEN 0x102b57UL, BIT1
#define GPIO134_OUT 0x102b57UL, BIT0
#define GPIO134_IN  0x102b57UL, BIT2

#define GPIO135_PAD PAD_PCM_A4
#define GPIO135_OEN 0x102b58UL, BIT1
#define GPIO135_OUT 0x102b58UL, BIT0
#define GPIO135_IN  0x102b58UL, BIT2

#define GPIO136_PAD PAD_PCM_A3
#define GPIO136_OEN 0x102b59UL, BIT1
#define GPIO136_OUT 0x102b59UL, BIT0
#define GPIO136_IN  0x102b59UL, BIT2

#define GPIO137_PAD PAD_PCM_A2
#define GPIO137_OEN 0x102b5aUL, BIT1
#define GPIO137_OUT 0x102b5aUL, BIT0
#define GPIO137_IN  0x102b5aUL, BIT2

#define GPIO138_PAD PAD_PCM_REG_N
#define GPIO138_OEN 0x102b5bUL, BIT1
#define GPIO138_OUT 0x102b5bUL, BIT0
#define GPIO138_IN  0x102b5bUL, BIT2

#define GPIO139_PAD PAD_PCM_A1
#define GPIO139_OEN 0x102b5cUL, BIT1
#define GPIO139_OUT 0x102b5cUL, BIT0
#define GPIO139_IN  0x102b5cUL, BIT2

#define GPIO140_PAD PAD_PCM_A0
#define GPIO140_OEN 0x102b5dUL, BIT1
#define GPIO140_OUT 0x102b5dUL, BIT0
#define GPIO140_IN  0x102b5dUL, BIT2

#define GPIO141_PAD PAD_PCM_D0
#define GPIO141_OEN 0x102b5eUL, BIT1
#define GPIO141_OUT 0x102b5eUL, BIT0
#define GPIO141_IN  0x102b5eUL, BIT2

#define GPIO142_PAD PAD_PCM_D1
#define GPIO142_OEN 0x102b5fUL, BIT1
#define GPIO142_OUT 0x102b5fUL, BIT0
#define GPIO142_IN  0x102b5fUL, BIT2

#define GPIO143_PAD PAD_PCM_D2
#define GPIO143_OEN 0x102b60UL, BIT1
#define GPIO143_OUT 0x102b60UL, BIT0
#define GPIO143_IN  0x102b60UL, BIT2

#define GPIO144_PAD PAD_PCM_RESET
#define GPIO144_OEN 0x102b61UL, BIT1
#define GPIO144_OUT 0x102b61UL, BIT0
#define GPIO144_IN  0x102b61UL, BIT2

#define GPIO145_PAD PAD_PCM_CD_N
#define GPIO145_OEN 0x102b62UL, BIT1
#define GPIO145_OUT 0x102b62UL, BIT0
#define GPIO145_IN  0x102b62UL, BIT2

#define GPIO146_PAD PAD_PWM0
#define GPIO146_OEN 0x102b88UL, BIT1
#define GPIO146_OUT 0x102b88UL, BIT0
#define GPIO146_IN  0x102b88UL, BIT2

#define GPIO147_PAD PAD_PWM1
#define GPIO147_OEN 0x102b89UL, BIT1
#define GPIO147_OUT 0x102b89UL, BIT0
#define GPIO147_IN  0x102b89UL, BIT2

#define GPIO148_PAD PAD_PWM2
#define GPIO148_OEN 0x102b8aUL, BIT1
#define GPIO148_OUT 0x102b8aUL, BIT0
#define GPIO148_IN  0x102b8aUL, BIT2

#define GPIO149_PAD PAD_PWM3
#define GPIO149_OEN 0x102b8bUL, BIT1
#define GPIO149_OUT 0x102b8bUL, BIT0
#define GPIO149_IN  0x102b8bUL, BIT2

#define GPIO150_PAD PAD_PWM4
#define GPIO150_OEN 0x102b8cUL, BIT1
#define GPIO150_OUT 0x102b8cUL, BIT0
#define GPIO150_IN  0x102b8cUL, BIT2

#define GPIO151_PAD PAD_TGPIO0
#define GPIO151_OEN 0x102b8dUL, BIT1
#define GPIO151_OUT 0x102b8dUL, BIT0
#define GPIO151_IN  0x102b8dUL, BIT2

#define GPIO152_PAD PAD_TGPIO1
#define GPIO152_OEN 0x102b8eUL, BIT1
#define GPIO152_OUT 0x102b8eUL, BIT0
#define GPIO152_IN  0x102b8eUL, BIT2

#define GPIO153_PAD PAD_TGPIO2
#define GPIO153_OEN 0x102b8fUL, BIT1
#define GPIO153_OUT 0x102b8fUL, BIT0
#define GPIO153_IN  0x102b8fUL, BIT2

#define GPIO154_PAD PAD_TGPIO3
#define GPIO154_OEN 0x102b90UL, BIT1
#define GPIO154_OUT 0x102b90UL, BIT0
#define GPIO154_IN  0x102b90UL, BIT2

#define GPIO155_PAD PAD_TS0_D0
#define GPIO155_OEN 0x102b1cUL, BIT1
#define GPIO155_OUT 0x102b1cUL, BIT0
#define GPIO155_IN  0x102b1cUL, BIT2

#define GPIO156_PAD PAD_TS0_D1
#define GPIO156_OEN 0x102b1dUL, BIT1
#define GPIO156_OUT 0x102b1dUL, BIT0
#define GPIO156_IN  0x102b1dUL, BIT2

#define GPIO157_PAD PAD_TS0_D2
#define GPIO157_OEN 0x102b1eUL, BIT1
#define GPIO157_OUT 0x102b1eUL, BIT0
#define GPIO157_IN  0x102b1eUL, BIT2

#define GPIO158_PAD PAD_TS0_D3
#define GPIO158_OEN 0x102b1fUL, BIT1
#define GPIO158_OUT 0x102b1fUL, BIT0
#define GPIO158_IN  0x102b1fUL, BIT2

#define GPIO159_PAD PAD_TS0_D4
#define GPIO159_OEN 0x102b20UL, BIT1
#define GPIO159_OUT 0x102b20UL, BIT0
#define GPIO159_IN  0x102b20UL, BIT2

#define GPIO160_PAD PAD_TS0_D5
#define GPIO160_OEN 0x102b21UL, BIT1
#define GPIO160_OUT 0x102b21UL, BIT0
#define GPIO160_IN  0x102b21UL, BIT2

#define GPIO161_PAD PAD_TS0_D6
#define GPIO161_OEN 0x102b22UL, BIT1
#define GPIO161_OUT 0x102b22UL, BIT0
#define GPIO161_IN  0x102b22UL, BIT2

#define GPIO162_PAD PAD_TS0_D7
#define GPIO162_OEN 0x102b23UL, BIT1
#define GPIO162_OUT 0x102b23UL, BIT0
#define GPIO162_IN  0x102b23UL, BIT2

#define GPIO163_PAD PAD_TS0_VLD
#define GPIO163_OEN 0x102b24UL, BIT1
#define GPIO163_OUT 0x102b24UL, BIT0
#define GPIO163_IN  0x102b24UL, BIT2

#define GPIO164_PAD PAD_TS0_SYNC
#define GPIO164_OEN 0x102b25UL, BIT1
#define GPIO164_OUT 0x102b25UL, BIT0
#define GPIO164_IN  0x102b25UL, BIT2

#define GPIO165_PAD PAD_TS0_CLK
#define GPIO165_OEN 0x102b26UL, BIT1
#define GPIO165_OUT 0x102b26UL, BIT0
#define GPIO165_IN  0x102b26UL, BIT2

#define GPIO166_PAD PAD_TS1_CLK
#define GPIO166_OEN 0x102b27UL, BIT1
#define GPIO166_OUT 0x102b27UL, BIT0
#define GPIO166_IN  0x102b27UL, BIT2

#define GPIO167_PAD PAD_TS1_SYNC
#define GPIO167_OEN 0x102b28UL, BIT1
#define GPIO167_OUT 0x102b28UL, BIT0
#define GPIO167_IN  0x102b28UL, BIT2

#define GPIO168_PAD PAD_TS1_VLD
#define GPIO168_OEN 0x102b29UL, BIT1
#define GPIO168_OUT 0x102b29UL, BIT0
#define GPIO168_IN  0x102b29UL, BIT2

#define GPIO169_PAD PAD_TS1_D7
#define GPIO169_OEN 0x102b2aUL, BIT1
#define GPIO169_OUT 0x102b2aUL, BIT0
#define GPIO169_IN  0x102b2aUL, BIT2

#define GPIO170_PAD PAD_TS1_D6
#define GPIO170_OEN 0x102b2bUL, BIT1
#define GPIO170_OUT 0x102b2bUL, BIT0
#define GPIO170_IN  0x102b2bUL, BIT2

#define GPIO171_PAD PAD_TS1_D5
#define GPIO171_OEN 0x102b2cUL, BIT1
#define GPIO171_OUT 0x102b2cUL, BIT0
#define GPIO171_IN  0x102b2cUL, BIT2

#define GPIO172_PAD PAD_TS1_D4
#define GPIO172_OEN 0x102b2dUL, BIT1
#define GPIO172_OUT 0x102b2dUL, BIT0
#define GPIO172_IN  0x102b2dUL, BIT2

#define GPIO173_PAD PAD_TS1_D3
#define GPIO173_OEN 0x102b2eUL, BIT1
#define GPIO173_OUT 0x102b2eUL, BIT0
#define GPIO173_IN  0x102b2eUL, BIT2

#define GPIO174_PAD PAD_TS1_D2
#define GPIO174_OEN 0x102b2fUL, BIT1
#define GPIO174_OUT 0x102b2fUL, BIT0
#define GPIO174_IN  0x102b2fUL, BIT2

#define GPIO175_PAD PAD_TS1_D1
#define GPIO175_OEN 0x102b30UL, BIT1
#define GPIO175_OUT 0x102b30UL, BIT0
#define GPIO175_IN  0x102b30UL, BIT2

#define GPIO176_PAD PAD_TS1_D0
#define GPIO176_OEN 0x102b31UL, BIT1
#define GPIO176_OUT 0x102b31UL, BIT0
#define GPIO176_IN  0x102b31UL, BIT2

#define GPIO177_PAD PAD_EMMC_IO9
#define GPIO177_OEN 0x102b78UL, BIT1
#define GPIO177_OUT 0x102b78UL, BIT0
#define GPIO177_IN  0x102b78UL, BIT2

#define GPIO178_PAD PAD_EMMC_IO12
#define GPIO178_OEN 0x102b79UL, BIT1
#define GPIO178_OUT 0x102b79UL, BIT0
#define GPIO178_IN  0x102b79UL, BIT2

#define GPIO179_PAD PAD_EMMC_IO14
#define GPIO179_OEN 0x102b7aUL, BIT1
#define GPIO179_OUT 0x102b7aUL, BIT0
#define GPIO179_IN  0x102b7aUL, BIT2

#define GPIO180_PAD PAD_EMMC_IO10
#define GPIO180_OEN 0x102b7bUL, BIT1
#define GPIO180_OUT 0x102b7bUL, BIT0
#define GPIO180_IN  0x102b7bUL, BIT2

#define GPIO181_PAD PAD_EMMC_IO16
#define GPIO181_OEN 0x102b7cUL, BIT1
#define GPIO181_OUT 0x102b7cUL, BIT0
#define GPIO181_IN  0x102b7cUL, BIT2

#define GPIO182_PAD PAD_EMMC_IO17
#define GPIO182_OEN 0x102b7dUL, BIT1
#define GPIO182_OUT 0x102b7dUL, BIT0
#define GPIO182_IN  0x102b7dUL, BIT2

#define GPIO183_PAD PAD_EMMC_IO15
#define GPIO183_OEN 0x102b7eUL, BIT1
#define GPIO183_OUT 0x102b7eUL, BIT0
#define GPIO183_IN  0x102b7eUL, BIT2

#define GPIO184_PAD PAD_EMMC_IO11
#define GPIO184_OEN 0x102b7fUL, BIT1
#define GPIO184_OUT 0x102b7fUL, BIT0
#define GPIO184_IN  0x102b7fUL, BIT2

#define GPIO185_PAD PAD_EMMC_IO8
#define GPIO185_OEN 0x102b80UL, BIT1
#define GPIO185_OUT 0x102b80UL, BIT0
#define GPIO185_IN  0x102b80UL, BIT2

#define GPIO186_PAD PAD_EMMC_IO4
#define GPIO186_OEN 0x102bacUL, BIT1
#define GPIO186_OUT 0x102bacUL, BIT0
#define GPIO186_IN  0x102bacUL, BIT2

#define GPIO187_PAD PAD_EMMC_IO3
#define GPIO187_OEN 0x102badUL, BIT1
#define GPIO187_OUT 0x102badUL, BIT0
#define GPIO187_IN  0x102badUL, BIT2

#define GPIO188_PAD PAD_EMMC_IO0
#define GPIO188_OEN 0x102baeUL, BIT1
#define GPIO188_OUT 0x102baeUL, BIT0
#define GPIO188_IN  0x102baeUL, BIT2

#define GPIO189_PAD PAD_TS2_D0
#define GPIO189_OEN 0x102b32UL, BIT1
#define GPIO189_OUT 0x102b32UL, BIT0
#define GPIO189_IN  0x102b32UL, BIT2

#define GPIO190_PAD PAD_TS2_VLD
#define GPIO190_OEN 0x102b33UL, BIT1
#define GPIO190_OUT 0x102b33UL, BIT0
#define GPIO190_IN  0x102b33UL, BIT2

#define GPIO191_PAD PAD_TS2_SYNC
#define GPIO191_OEN 0x102b34UL, BIT1
#define GPIO191_OUT 0x102b34UL, BIT0
#define GPIO191_IN  0x102b34UL, BIT2

#define GPIO192_PAD PAD_TS2_CLK
#define GPIO192_OEN 0x102b35UL, BIT1
#define GPIO192_OUT 0x102b35UL, BIT0
#define GPIO192_IN  0x102b35UL, BIT2

#define GPIO193_PAD PAD_TS2_D1
#define GPIO193_OEN 0x102bf2UL, BIT1
#define GPIO193_OUT 0x102bf2UL, BIT0
#define GPIO193_IN  0x102bf2UL, BIT2

#define GPIO194_PAD PAD_TS2_D2
#define GPIO194_OEN 0x102bf3UL, BIT1
#define GPIO194_OUT 0x102bf3UL, BIT0
#define GPIO194_IN  0x102bf3UL, BIT2

#define GPIO195_PAD PAD_TS2_D3
#define GPIO195_OEN 0x102bf4UL, BIT1
#define GPIO195_OUT 0x102bf4UL, BIT0
#define GPIO195_IN  0x102bf4UL, BIT2

#define GPIO196_PAD PAD_TS2_D4
#define GPIO196_OEN 0x102bf5UL, BIT1
#define GPIO196_OUT 0x102bf5UL, BIT0
#define GPIO196_IN  0x102bf5UL, BIT2

#define GPIO197_PAD PAD_TS2_D5
#define GPIO197_OEN 0x102bf6UL, BIT1
#define GPIO197_OUT 0x102bf6UL, BIT0
#define GPIO197_IN  0x102bf6UL, BIT2

#define GPIO198_PAD PAD_TS2_D6
#define GPIO198_OEN 0x102bf7UL, BIT1
#define GPIO198_OUT 0x102bf7UL, BIT0
#define GPIO198_IN  0x102bf7UL, BIT2

#define GPIO199_PAD PAD_TS2_D7
#define GPIO199_OEN 0x102bf8UL, BIT1
#define GPIO199_OUT 0x102bf8UL, BIT0
#define GPIO199_IN  0x102bf8UL, BIT2

#define GPIO200_PAD PAD_TS3_D0
#define GPIO200_OEN 0x110300UL, BIT1
#define GPIO200_OUT 0x110300UL, BIT0
#define GPIO200_IN  0x110300UL, BIT2

#define GPIO201_PAD PAD_TS3_D1
#define GPIO201_OEN 0x110301UL, BIT1
#define GPIO201_OUT 0x110301UL, BIT0
#define GPIO201_IN  0x110301UL, BIT2

#define GPIO202_PAD PAD_TS3_D2
#define GPIO202_OEN 0x110302UL, BIT1
#define GPIO202_OUT 0x110302UL, BIT0
#define GPIO202_IN  0x110302UL, BIT2

#define GPIO203_PAD PAD_TS3_D3
#define GPIO203_OEN 0x110303UL, BIT1
#define GPIO203_OUT 0x110303UL, BIT0
#define GPIO203_IN  0x110303UL, BIT2

#define GPIO204_PAD PAD_TS3_D4
#define GPIO204_OEN 0x110304UL, BIT1
#define GPIO204_OUT 0x110304UL, BIT0
#define GPIO204_IN  0x110304UL, BIT2

#define GPIO205_PAD PAD_TS3_D5
#define GPIO205_OEN 0x110305UL, BIT1
#define GPIO205_OUT 0x110305UL, BIT0
#define GPIO205_IN  0x110305UL, BIT2

#define GPIO206_PAD PAD_TS3_D6
#define GPIO206_OEN 0x110306UL, BIT1
#define GPIO206_OUT 0x110306UL, BIT0
#define GPIO206_IN  0x110306UL, BIT2

#define GPIO207_PAD PAD_TS3_D7
#define GPIO207_OEN 0x110307UL, BIT1
#define GPIO207_OUT 0x110307UL, BIT0
#define GPIO207_IN  0x110307UL, BIT2

#define GPIO208_PAD PAD_TS3_VLD
#define GPIO208_OEN 0x110308UL, BIT1
#define GPIO208_OUT 0x110308UL, BIT0
#define GPIO208_IN  0x110308UL, BIT2

#define GPIO209_PAD PAD_TS3_SYNC
#define GPIO209_OEN 0x110309UL, BIT1
#define GPIO209_OUT 0x110309UL, BIT0
#define GPIO209_IN  0x110309UL, BIT2

#define GPIO210_PAD PAD_TS3_CLK
#define GPIO210_OEN 0x11030aUL, BIT1
#define GPIO210_OUT 0x11030aUL, BIT0
#define GPIO210_IN  0x11030aUL, BIT2

#define GPIO211_PAD PAD_EMMC_IO13
#define GPIO211_OEN 0x102b81UL, BIT1
#define GPIO211_OUT 0x102b81UL, BIT0
#define GPIO211_IN  0x102b81UL, BIT2

#define GPIO212_PAD PAD_EMMC_IO1
#define GPIO212_OEN 0x102b82UL, BIT1
#define GPIO212_OUT 0x102b82UL, BIT0
#define GPIO212_IN  0x102b82UL, BIT2

#define GPIO213_PAD PAD_EMMC_IO2
#define GPIO213_OEN 0x102b83UL, BIT1
#define GPIO213_OUT 0x102b83UL, BIT0
#define GPIO213_IN  0x102b83UL, BIT2

#define GPIO214_PAD PAD_EMMC_IO7
#define GPIO214_OEN 0x102b84UL, BIT1
#define GPIO214_OUT 0x102b84UL, BIT0
#define GPIO214_IN  0x102b84UL, BIT2

#define GPIO215_PAD PAD_EMMC_IO6
#define GPIO215_OEN 0x102b85UL, BIT1
#define GPIO215_OUT 0x102b85UL, BIT0
#define GPIO215_IN  0x102b85UL, BIT2

#define GPIO216_PAD PAD_EMMC_IO5
#define GPIO216_OEN 0x102b76UL, BIT1
#define GPIO216_OUT 0x102b76UL, BIT0
#define GPIO216_IN  0x102b76UL, BIT2

#define GPIO217_PAD PAD_ARC0
#define GPIO217_OEN 0x110320UL, BIT1
#define GPIO217_OUT 0x110320UL, BIT0
#define GPIO217_IN  0x110320UL, BIT2

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
    __GPIO(175), __GPIO(176), __GPIO(177), __GPIO(178), __GPIO(179),
    __GPIO(180), __GPIO(181), __GPIO(182), __GPIO(183), __GPIO(184),
    __GPIO(185), __GPIO(186), __GPIO(187), __GPIO(188), __GPIO(189),
    __GPIO(190), __GPIO(191), __GPIO(192), __GPIO(193), __GPIO(194),
    __GPIO(195), __GPIO(196), __GPIO(197), __GPIO(198), __GPIO(199),
    __GPIO(200), __GPIO(201), __GPIO(202), __GPIO(203), __GPIO(204),
    __GPIO(205), __GPIO(206), __GPIO(207), __GPIO(208), __GPIO(209),
    __GPIO(210), __GPIO(211), __GPIO(212), __GPIO(213), __GPIO(214),
    __GPIO(215), __GPIO(216), __GPIO(217)

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
