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
#define GPIO0_OEN 0x000f28UL, BIT0
#define GPIO0_OUT 0x000f28UL, BIT1
#define GPIO0_IN  0x000f28UL, BIT2

#define GPIO1_PAD PAD_PM_SPI_CZ
#define GPIO1_OEN 0x000f30UL, BIT0
#define GPIO1_OUT 0x000f30UL, BIT1
#define GPIO1_IN  0x000f30UL, BIT2

#define GPIO2_PAD PAD_PM_SPI_CK
#define GPIO2_OEN 0x000f32UL, BIT0
#define GPIO2_OUT 0x000f32UL, BIT1
#define GPIO2_IN  0x000f32UL, BIT2

#define GPIO3_PAD PAD_PM_SPI_DI
#define GPIO3_OEN 0x000f34UL, BIT0
#define GPIO3_OUT 0x000f34UL, BIT1
#define GPIO3_IN  0x000f34UL, BIT2

#define GPIO4_PAD PAD_PM_SPI_DO
#define GPIO4_OEN 0x000f36UL, BIT0
#define GPIO4_OUT 0x000f36UL, BIT1
#define GPIO4_IN  0x000f36UL, BIT2

#define GPIO5_PAD PAD_PM_GPIO0
#define GPIO5_OEN 0x000f00UL, BIT0
#define GPIO5_OUT 0x000f00UL, BIT1
#define GPIO5_IN  0x000f00UL, BIT2

#define GPIO6_PAD PAD_PM_GPIO1
#define GPIO6_OEN 0x000f02UL, BIT0
#define GPIO6_OUT 0x000f02UL, BIT1
#define GPIO6_IN  0x000f02UL, BIT2

#define GPIO7_PAD PAD_PM_GPIO2
#define GPIO7_OEN 0x000f04UL, BIT0
#define GPIO7_OUT 0x000f04UL, BIT1
#define GPIO7_IN  0x000f04UL, BIT2

#define GPIO8_PAD PAD_PM_GPIO3
#define GPIO8_OEN 0x000f06UL, BIT0
#define GPIO8_OUT 0x000f06UL, BIT1
#define GPIO8_IN  0x000f06UL, BIT2

#define GPIO9_PAD PAD_PM_GPIO4
#define GPIO9_OEN 0x000f08UL, BIT0
#define GPIO9_OUT 0x000f08UL, BIT1
#define GPIO9_IN  0x000f08UL, BIT2

#define GPIO10_PAD PAD_PM_GPIO5
#define GPIO10_OEN 0x000f0a, BIT0
#define GPIO10_OUT 0x000f0a, BIT1
#define GPIO10_IN  0x000f0a, BIT2

#define GPIO11_PAD PAD_PM_GPIO6
#define GPIO11_OEN 0x000f0cUL, BIT0
#define GPIO11_OUT 0x000f0cUL, BIT1
#define GPIO11_IN  0x000f0cUL, BIT2

#define GPIO12_PAD PAD_PM_GPIO7
#define GPIO12_OEN 0x000f0eUL, BIT0
#define GPIO12_OUT 0x000f0eUL, BIT1
#define GPIO12_IN  0x000f0eUL, BIT2

#define GPIO13_PAD PAD_PM_CEC
#define GPIO13_OEN 0x000f2cUL, BIT0
#define GPIO13_OUT 0x000f2cUL, BIT1
#define GPIO13_IN  0x000f2cUL, BIT2

#define GPIO14_PAD PAD_HDMITX_HPD
#define GPIO14_OEN 0x001166UL, BIT3
#define GPIO14_OUT 0x001166UL, BIT2
#define GPIO14_IN  0x001166UL, BIT0

#define GPIO15_PAD PAD_VID0
#define GPIO15_OEN 0x000f90UL, BIT0
#define GPIO15_OUT 0x000f90UL, BIT1
#define GPIO15_IN  0x000f90UL, BIT2

#define GPIO16_PAD PAD_VID1
#define GPIO16_OEN 0x000f92UL, BIT0
#define GPIO16_OUT 0x000f92UL, BIT1
#define GPIO16_IN  0x000f92UL, BIT2

#define GPIO17_PAD PAD_PM_LED0
#define GPIO17_OEN 0x000f94UL, BIT0
#define GPIO17_OUT 0x000f94UL, BIT1
#define GPIO17_IN  0x000f94UL, BIT2

#define GPIO18_PAD PAD_PM_LED1
#define GPIO18_OEN 0x000f96UL, BIT0
#define GPIO18_OUT 0x000f96UL, BIT1
#define GPIO18_IN  0x000f96UL, BIT2

#define GPIO19_PAD PAD_I2S_OUT_BCK
#define GPIO19_OEN 0x102556UL, BIT4
#define GPIO19_OUT 0x102556UL, BIT4
#define GPIO19_IN  0x102556UL, BIT0

#define GPIO20_PAD PAD_I2S_OUT_MCK
#define GPIO20_OEN 0x102558UL, BIT5
#define GPIO20_OUT 0x102558UL, BIT4
#define GPIO20_IN  0x102558UL, BIT0

#define GPIO21_PAD PAD_I2S_OUT_WS
#define GPIO21_OEN 0x10255aUL, BIT5
#define GPIO21_OUT 0x10255aUL, BIT4
#define GPIO21_IN  0x10255aUL, BIT0

#define GPIO22_PAD PAD_I2S_OUT_SD0
#define GPIO22_OEN 0x10255cUL, BIT5
#define GPIO22_OUT 0x10255cUL, BIT4
#define GPIO22_IN  0x10255cUL, BIT0

#define GPIO23_PAD PAD_I2S_OUT_SD1
#define GPIO23_OEN 0x10255eUL, BIT5
#define GPIO23_OUT 0x10255eUL, BIT4
#define GPIO23_IN  0x10255eUL, BIT0

#define GPIO24_PAD PAD_I2S_OUT_SD2
#define GPIO24_OEN 0x102560UL, BIT5
#define GPIO24_OUT 0x102560UL, BIT4
#define GPIO24_IN  0x102560UL, BIT0

#define GPIO25_PAD PAD_I2S_OUT_SD3
#define GPIO25_OEN 0x102562UL, BIT5
#define GPIO25_OUT 0x102562UL, BIT4
#define GPIO25_IN  0x102562UL, BIT0

#define GPIO26_PAD PAD_I2S_OUT_MUTE
#define GPIO26_OEN 0x102564UL, BIT5
#define GPIO26_OUT 0x102564UL, BIT4
#define GPIO26_IN  0x102564UL, BIT0

#define GPIO27_PAD PAD_SD_CLK
#define GPIO27_OEN 0x1025a4UL, BIT5
#define GPIO27_OUT 0x1025a4UL, BIT4
#define GPIO27_IN  0x1025a4UL, BIT0

#define GPIO28_PAD PAD_SD_CMD
#define GPIO28_OEN 0x1025a6UL, BIT5
#define GPIO28_OUT 0x1025a6UL, BIT4
#define GPIO28_IN  0x1025a6UL, BIT0

#define GPIO29_PAD PAD_SD_D0
#define GPIO29_OEN 0x1025a8UL, BIT5
#define GPIO29_OUT 0x1025a8UL, BIT4
#define GPIO29_IN  0x1025a8UL, BIT0

#define GPIO30_PAD PAD_SD_D1
#define GPIO30_OEN 0x1025aaUL, BIT5
#define GPIO30_OUT 0x1025aaUL, BIT4
#define GPIO30_IN  0x1025aaUL, BIT0

#define GPIO31_PAD PAD_SD_D2
#define GPIO31_OEN 0x1025acUL, BIT5
#define GPIO31_OUT 0x1025acUL, BIT4
#define GPIO31_IN  0x1025acUL, BIT0

#define GPIO32_PAD PAD_SD_D3
#define GPIO32_OEN 0x1025aeUL, BIT5
#define GPIO32_OUT 0x1025aeUL, BIT4
#define GPIO32_IN  0x1025aeUL, BIT0

#define GPIO33_PAD PAD_GPIO0
#define GPIO33_OEN 0x1025b0UL, BIT5
#define GPIO33_OUT 0x1025b0UL, BIT4
#define GPIO33_IN  0x1025b0UL, BIT0

#define GPIO34_PAD PAD_GPIO1
#define GPIO34_OEN 0x1025b2UL, BIT5
#define GPIO34_OUT 0x1025b2UL, BIT4
#define GPIO34_IN  0x1025b2UL, BIT0

#define GPIO35_PAD PAD_GPIO2
#define GPIO35_OEN 0x1025b4UL, BIT5
#define GPIO35_OUT 0x1025b4UL, BIT4
#define GPIO35_IN  0x1025b4UL, BIT0

#define GPIO36_PAD PAD_GPIO3
#define GPIO36_OEN 0x1025b6UL, BIT5
#define GPIO36_OUT 0x1025b6UL, BIT4
#define GPIO36_IN  0x1025b6UL, BIT0

#define GPIO37_PAD PAD_GPIO4
#define GPIO37_OEN 0x1025b8UL, BIT5
#define GPIO37_OUT 0x1025b8UL, BIT4
#define GPIO37_IN  0x1025b8UL, BIT0

#define GPIO38_PAD PAD_GPIO5
#define GPIO38_OEN 0x1025baUL, BIT5
#define GPIO38_OUT 0x1025baUL, BIT4
#define GPIO38_IN  0x1025baUL, BIT0

#define GPIO39_PAD PAD_GPIO6
#define GPIO39_OEN 0x1025bcUL, BIT5
#define GPIO39_OUT 0x1025bcUL, BIT4
#define GPIO39_IN  0x1025bcUL, BIT0

#define GPIO40_PAD PAD_GPIO7
#define GPIO40_OEN 0x1025beUL, BIT5
#define GPIO40_OUT 0x1025beUL, BIT4
#define GPIO40_IN  0x1025beUL, BIT0

#define GPIO41_PAD PAD_TS_CLK
#define GPIO41_OEN 0x102524UL, BIT5
#define GPIO41_OUT 0x102524UL, BIT4
#define GPIO41_IN  0x102524UL, BIT0

#define GPIO42_PAD PAD_TS_SYNC
#define GPIO42_OEN 0x102526UL, BIT5
#define GPIO42_OUT 0x102526UL, BIT4
#define GPIO42_IN  0x102526UL, BIT0

#define GPIO43_PAD PAD_TS_VLD
#define GPIO43_OEN 0x102528UL, BIT5
#define GPIO43_OUT 0x102528UL, BIT4
#define GPIO43_IN  0x102528UL, BIT0

#define GPIO44_PAD PAD_TS_D0
#define GPIO44_OEN 0x10252aUL, BIT5
#define GPIO44_OUT 0x10252aUL, BIT4
#define GPIO44_IN  0x10252aUL, BIT0

#define GPIO45_PAD PAD_TS_D1
#define GPIO45_OEN 0x10252cUL, BIT5
#define GPIO45_OUT 0x10252cUL, BIT4
#define GPIO45_IN  0x10252cUL, BIT0

#define GPIO46_PAD PAD_TS_D2
#define GPIO46_OEN 0x10252eUL, BIT5
#define GPIO46_OUT 0x10252eUL, BIT4
#define GPIO46_IN  0x10252eUL, BIT0

#define GPIO47_PAD PAD_TS_D3
#define GPIO47_OEN 0x102530UL, BIT5
#define GPIO47_OUT 0x102530UL, BIT4
#define GPIO47_IN  0x102530UL, BIT0

#define GPIO48_PAD PAD_TS_D4
#define GPIO48_OEN 0x102532UL, BIT5
#define GPIO48_OUT 0x102532UL, BIT4
#define GPIO48_IN  0x102532UL, BIT0

#define GPIO49_PAD PAD_TS_D5
#define GPIO49_OEN 0x102534UL, BIT5
#define GPIO49_OUT 0x102534UL, BIT4
#define GPIO49_IN  0x102534UL, BIT0

#define GPIO50_PAD PAD_TS_D6
#define GPIO50_OEN 0x102536UL, BIT5
#define GPIO50_OUT 0x102536UL, BIT4
#define GPIO50_IN  0x102536UL, BIT0

#define GPIO51_PAD PAD_TS_D7
#define GPIO51_OEN 0x102538UL, BIT5
#define GPIO51_OUT 0x102538UL, BIT4
#define GPIO51_IN  0x102538UL, BIT0

#define GPIO52_PAD PAD_SPDIF_OUT
#define GPIO52_OEN 0x1025a0UL, BIT5
#define GPIO52_OUT 0x1025a0UL, BIT4
#define GPIO52_IN  0x1025a0UL, BIT0

#define GPIO53_PAD PAD_I2CM0_SCL
#define GPIO53_OEN 0x10251cUL, BIT5
#define GPIO53_OUT 0x10251cUL, BIT4
#define GPIO53_IN  0x10251cUL, BIT0

#define GPIO54_PAD PAD_I2CM0_SDA
#define GPIO54_OEN 0x10251eUL, BIT5
#define GPIO54_OUT 0x10251eUL, BIT4
#define GPIO54_IN  0x10251eUL, BIT0

#define GPIO55_PAD PAD_I2CM1_SCL
#define GPIO55_OEN 0x102520UL, BIT5
#define GPIO55_OUT 0x102520UL, BIT4
#define GPIO55_IN  0x102520UL, BIT0

#define GPIO56_PAD PAD_I2CM1_SDA
#define GPIO56_OEN 0x102522UL, BIT5
#define GPIO56_OUT 0x102522UL, BIT4
#define GPIO56_IN  0x102522UL, BIT0

#define GPIO57_PAD PAD_HDMITX_SCL
#define GPIO57_OEN 0x1025daUL, BIT5
#define GPIO57_OUT 0x1025daUL, BIT4
#define GPIO57_IN  0x1025daUL, BIT0

#define GPIO58_PAD PAD_HDMITX_SDA
#define GPIO58_OEN 0x1025dcUL, BIT5
#define GPIO58_OUT 0x1025dcUL, BIT4
#define GPIO58_IN  0x1025dcUL, BIT0

#define GPIO59_PAD PAD_GT0_MDIO
#define GPIO59_OEN 0x10253aUL, BIT5
#define GPIO59_OUT 0x10253aUL, BIT4
#define GPIO59_IN  0x10253aUL, BIT0

#define GPIO60_PAD PAD_GT0_MDC
#define GPIO60_OEN 0x10253cUL, BIT5
#define GPIO60_OUT 0x10253cUL, BIT4
#define GPIO60_IN  0x10253cUL, BIT0

#define GPIO61_PAD PAD_GT0_RX_CLK
#define GPIO61_OEN 0x10253eUL, BIT5
#define GPIO61_OUT 0x10253eUL, BIT4
#define GPIO61_IN  0x10253eUL, BIT0

#define GPIO62_PAD PAD_GT0_RX_CTL
#define GPIO62_OEN 0x102540UL, BIT5
#define GPIO62_OUT 0x102540UL, BIT4
#define GPIO62_IN  0x102540UL, BIT0

#define GPIO63_PAD PAD_GT0_RX_D0
#define GPIO63_OEN 0x102542UL, BIT5
#define GPIO63_OUT 0x102542UL, BIT4
#define GPIO63_IN  0x102542UL, BIT0

#define GPIO64_PAD PAD_GT0_RX_D1
#define GPIO64_OEN 0x102544UL, BIT5
#define GPIO64_OUT 0x102544UL, BIT4
#define GPIO64_IN  0x102544UL, BIT0

#define GPIO65_PAD PAD_GT0_RX_D2
#define GPIO65_OEN 0x102546UL, BIT5
#define GPIO65_OUT 0x102546UL, BIT4
#define GPIO65_IN  0x102546UL, BIT0

#define GPIO66_PAD PAD_GT0_RX_D3
#define GPIO66_OEN 0x102548UL, BIT5
#define GPIO66_OUT 0x102548UL, BIT4
#define GPIO66_IN  0x102548UL, BIT0

#define GPIO67_PAD PAD_GT0_TX_CLK
#define GPIO67_OEN 0x10254aUL, BIT5
#define GPIO67_OUT 0x10254aUL, BIT4
#define GPIO67_IN  0x10254aUL, BIT0

#define GPIO68_PAD PAD_GT0_TX_CTL
#define GPIO68_OEN 0x10254cUL, BIT5
#define GPIO68_OUT 0x10254cUL, BIT4
#define GPIO68_IN  0x10254cUL, BIT0

#define GPIO69_PAD PAD_GT0_TX_D0
#define GPIO69_OEN 0x10254eUL, BIT5
#define GPIO69_OUT 0x10254eUL, BIT4
#define GPIO69_IN  0x10254eUL, BIT0

#define GPIO70_PAD PAD_GT0_TX_D1
#define GPIO70_OEN 0x102550UL, BIT5
#define GPIO70_OUT 0x102550UL, BIT4
#define GPIO70_IN  0x102550UL, BIT0

#define GPIO71_PAD PAD_GT0_TX_D2
#define GPIO71_OEN 0x102552UL, BIT5
#define GPIO71_OUT 0x102552UL, BIT4
#define GPIO71_IN  0x102552UL, BIT0

#define GPIO72_PAD PAD_GT0_TX_D3
#define GPIO72_OEN 0x102554UL, BIT5
#define GPIO72_OUT 0x102554UL, BIT4
#define GPIO72_IN  0x102554UL, BIT0

#define GPIO73_PAD PAD_SAR_GPIO0
#define GPIO73_OEN 0x001423UL, BIT0
#define GPIO73_OUT 0x001424UL, BIT0
#define GPIO73_IN  0x001425UL, BIT0

#define GPIO74_PAD PAD_SAR_GPIO1
#define GPIO74_OEN 0x001423UL, BIT1
#define GPIO74_OUT 0x001424UL, BIT1
#define GPIO74_IN  0x001425UL, BIT1

#define GPIO75_PAD PAD_SAR_GPIO2
#define GPIO75_OEN 0x001423UL, BIT2
#define GPIO75_OUT 0x001424UL, BIT2
#define GPIO75_IN  0x001425UL, BIT2

#define GPIO76_PAD PAD_SAR_GPIO3
#define GPIO76_OEN 0x001423UL, BIT3
#define GPIO76_OUT 0x001424UL, BIT3
#define GPIO76_IN  0x001425UL, BIT3

#define GPIO77_PAD PAD_SAR_GPIO4
#define GPIO77_OEN 0x001423UL, BIT4
#define GPIO77_OUT 0x001424UL, BIT4
#define GPIO77_IN  0x001425UL, BIT4

#define GPIO78_PAD PAD_EMMC_IO12
#define GPIO78_OEN 0x102588UL, BIT5
#define GPIO78_OUT 0x102588UL, BIT4
#define GPIO78_IN  0x102588UL, BIT0

#define GPIO79_PAD PAD_EMMC_IO14
#define GPIO79_OEN 0x10258cUL, BIT5
#define GPIO79_OUT 0x10258cUL, BIT4
#define GPIO79_IN  0x10258cUL, BIT0

#define GPIO80_PAD PAD_EMMC_IO15
#define GPIO80_OEN 0x10258eUL, BIT5
#define GPIO80_OUT 0x10258eUL, BIT4
#define GPIO80_IN  0x10258eUL, BIT0

#define GPIO81_PAD PAD_EMMC_IO16
#define GPIO81_OEN 0x102590UL, BIT5
#define GPIO81_OUT 0x102590UL, BIT4
#define GPIO81_IN  0x102590UL, BIT0

#define GPIO82_PAD PAD_EMMC_IO17
#define GPIO82_OEN 0x102592UL, BIT5
#define GPIO82_OUT 0x102592UL, BIT4
#define GPIO82_IN  0x102592UL, BIT0

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
    __GPIO(80), __GPIO(81), __GPIO(82)

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
