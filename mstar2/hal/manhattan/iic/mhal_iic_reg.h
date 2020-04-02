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

#ifndef _REG_IIC_H_
#define _REG_IIC_H_


//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------
//#define IIC_UNIT_NUM               2

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define MHal_IIC_DELAY()            { udelay(1000); udelay(1000); udelay(1000); udelay(1000); udelay(1000); }//delay 5ms

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
#define REG_IIC_BASE                0xFD223600 // 0xBF200000 + (0x8D80*4) //The 4th port
#elif defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define REG_IIC_BASE           (mstar_pm_base + 0x00223600UL)
#endif

#define REG_IIC_CTRL                0x00UL
#define REG_IIC_CLK_SEL             0x01UL
#define REG_IIC_WDATA               0x02UL
#define REG_IIC_RDATA               0x03UL
#define REG_IIC_STATUS              0x04UL                                // reset, clear and status

#define MHal_IIC_REG(addr)          (*(volatile U32*)(REG_IIC_BASE + ((addr)<<2)))

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
#define REG_CHIP_BASE               0xFD203C00
#elif defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define REG_CHIP_BASE           (mstar_pm_base + 0x00203C00UL)
#endif

#define REG_IIC_ALLPADIN            0x50UL
#define REG_IIC_MODE                0x57UL
#define REG_DDCR_GPIO_SEL           0x70UL
#define MHal_CHIP_REG(addr)         (*(volatile U32*)(REG_CHIP_BASE + ((addr)<<2)))


//the definitions of GPIO reg set to initialize
#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
#define REG_ARM_BASE                0xFD000000//Use 8 bit addressing
#elif defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define REG_CHIP_BASE           (mstar_pm_base)
#endif

//#define REG_ALL_PAD_IN            ((0x0f50<<1) )   //set all pads (except SPI) as input
#define REG_ALL_PAD_IN              (0x101ea1UL)   //set all pads (except SPI) as input

//the definitions of GPIO reg set to make output
#define PAD_DDCR_CK                 173
#define REG_PAD_DDCR_CK_SET         (0x101eaeUL)
#define REG_PAD_DDCR_CK_OEN         (0x102b87UL)
#define REG_PAD_DDCR_CK_IN          (0x102b87UL)
#define REG_PAD_DDCR_CK_OUT         (0x102b87UL)
#define PAD_DDCR_DA                 172
#define REG_PAD_DDCR_DA_SET         (0x101eaeUL)
#define REG_PAD_DDCR_DA_OEN         (0x102b86UL)
#define REG_PAD_DDCR_DA_IN          (0x102b86UL)
#define REG_PAD_DDCR_DA_OUT         (0x102b86UL)

#define PAD_TGPIO2 181
#define REG_PAD_TGPIO2_SET
#define REG_PAD_TGPIO2_OEN (0x102b8fUL)
#define REG_PAD_TGPIO2_IN (0x102b8fUL)
#define REG_PAD_TGPIO2_OUT (0x102b8fUL)
#define PAD_TGPIO3 182
#define REG_PAD_TGPIO3_SET
#define REG_PAD_TGPIO3_OEN (0x102b90UL)
#define REG_PAD_TGPIO3_IN (0x102b90UL)
#define REG_PAD_TGPIO3_OUT (0x102b90UL)

#define PAD_I2S_OUT_SD1 101
#define REG_PAD_I2S_OUT_SD1_SET
#define REG_PAD_I2S_OUT_SD1_OEN (0x102b3fUL)
#define REG_PAD_I2S_OUT_SD1_IN (0x102b3fUL)
#define REG_PAD_I2S_OUT_SD1_OUT (0x102b3fUL)
#define PAD_SPDIF_IN 95
#define REG_PAD_SPDIF_IN_SET
#define REG_PAD_SPDIF_IN_OEN (0x102b39UL)
#define REG_PAD_SPDIF_IN_IN (0x102b39UL)
#define REG_PAD_SPDIF_IN_OUT (0x102b39UL)

#define PAD_I2S_IN_WS 92
#define REG_PAD_I2S_IN_WS_SET
#define REG_PAD_I2S_IN_WS_OEN (0x102b36UL)
#define REG_PAD_I2S_IN_WS_IN (0x102b36UL)
#define REG_PAD_I2S_IN_WS_OUT (0x102b36UL)
#define PAD_I2S_IN_BCK 93
#define REG_PAD_I2S_IN_BCK_SET
#define REG_PAD_I2S_IN_BCK_OEN (0x102b37UL)
#define REG_PAD_I2S_IN_BCK_IN (0x102b37UL)
#define REG_PAD_I2S_IN_BCK_OUT (0x102b37UL)

#define PAD_I2S_OUT_SD3 103
#define REG_PAD_I2S_OUT_SD3_SET
#define REG_PAD_I2S_OUT_SD3_OEN (0x102b41UL)
#define REG_PAD_I2S_OUT_SD3_IN (0x102b41UL)
#define REG_PAD_I2S_OUT_SD3_OUT (0x102b41UL)
#define PAD_I2S_OUT_SD2 102
#define REG_PAD_I2S_OUT_SD2_SET
#define REG_PAD_I2S_OUT_SD2_OEN (0x102b40UL)
#define REG_PAD_I2S_OUT_SD2_IN (0x102b40UL)
#define REG_PAD_I2S_OUT_SD2_OUT (0x102b40UL)

#define PAD_GPIO_PM9 9
#define REG_PAD_GPIO_PM9_SET
#define REG_PAD_GPIO_PM9_OEN (0x0f12UL)
#define REG_PAD_GPIO_PM9_IN (0x0f12UL)
#define REG_PAD_GPIO_PM9_OUT (0x0f12UL)
#define PAD_GPIO_PM8 8
#define REG_PAD_GPIO_PM8_SET
#define REG_PAD_GPIO_PM8_OEN (0x0f10UL)
#define REG_PAD_GPIO_PM8_IN (0x0f10UL)
#define REG_PAD_GPIO_PM8_OUT (0x0f10UL)

#define MHal_GPIO_REG(addr)             (*(volatile U8*)(REG_ARM_BASE + (((addr) & ~1)<<1) + (addr & 1)))
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

typedef struct
{
    U32 SclOenReg;
    U8  SclOenBit;

    U32 SclOutReg;
    U8  SclOutBit;

    U32 SclInReg;
    U8  SclInBit;

    U32 SdaOenReg;
    U8  SdaOenBit;

    U32 SdaOutReg;
    U8  SdaOutBit;

    U32 SdaInReg;
    U8  SdaInBit;

    U8  DefDelay;
}IIC_Bus_t;

#endif // _REG_IIC_H_

