//<MStar Software>
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

#ifndef _REG_PWM_H_
#define _REG_PWM_H_

////////////////////////////////////////////////////////////////////////////////
// Header Files
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Define & and data type
////////////////////////////////////////////////////////////////////////////////
#define REG_ALL_PAD_IN              (0x50) //bit 15;set all pads (except SPI) as input
#define REG_PWM_OEN                 (0x03) //bit 0~4

#define REG_PWM_MODE				(0x64)
#define PAD_PWM0_OUT                (BIT2)
#define PAD_PWM1_OUT                (BIT6)
#define PAD_PWM2_OUT                (BIT7)
#define PAD_PWM3_OUT                (BIT12)
#define PAD_PWM4_OUT                (BIT13)

#define REG_PM_BASE                 (0x001C00UL)
#define REG_TOP_BASE                (0x203C00UL)


#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
#define REG_PWM_BASE                0xFD27E800 // 0xBF200000 + (0x8D80*4) //The 4th port
#elif defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define REG_PWM_BASE           (mstar_pm_base + 0x0027E800UL)
#endif

#define REG_PWM0_PERIOD             (0x02)  //bit0~15
#define REG_PWM1_PERIOD             (0x05)  //bit0~15
#define REG_PWM2_PERIOD             (0x08)  //bit0~15
#define REG_PWM3_PERIOD             (0x0B)  //bit0~15
#define REG_PWM4_PERIOD             (0x0E)  //bit0~15
//#define REG_PWM5_PERIOD             (0x11)  //bit0~15
//#define REG_PWM6_PERIOD             (0x40)  //bit0~15
//#define REG_PWM7_PERIOD             (0x43)  //bit0~15
//#define REG_PWM8_PERIOD             (0x46)  //bit0~15

#define REG_PWM0_DUTY               (0x03)  //bit0~15
#define REG_PWM1_DUTY               (0x06)  //bit0~15
#define REG_PWM2_DUTY               (0x09)  //bit0~15
#define REG_PWM3_DUTY               (0x0C)  //bit0~15
#define REG_PWM4_DUTY               (0x0F)  //bit0~15
//#define REG_PWM5_DUTY               (0x12)  //bit0~15
//#define REG_PWM6_DUTY               (0x41)  //bit0~15
//#define REG_PWM7_DUTY               (0x44)  //bit0~15
//#define REG_PWM8_DUTY               (0x47)  //bit0~15

#define REG_PWM0_DIV                (0x04)  //bit0~7
#define REG_PWM1_DIV                (0x07)  //bit0~7
#define REG_PWM2_DIV                (0x0A)  //bit0~7
#define REG_PWM3_DIV                (0x0D)  //bit0~7
#define REG_PWM4_DIV                (0x10)  //bit0~7
//#define REG_PWM5_DIV                (0x13)  //bit0~7
//#define REG_PWM6_DIV                (0x42)  //bit0~7
//#define REG_PWM7_DIV                (0x45)  //bit0~7
//#define REG_PWM8_DIV                (0x48)  //bit0~7

#define REG_PWM0_PORARITY           (0x04)  //bit8
#define REG_PWM1_PORARITY           (0x07)  //bit8
#define REG_PWM2_PORARITY           (0x0A)  //bit8
#define REG_PWM3_PORARITY           (0x0D)  //bit8
#define REG_PWM4_PORARITY           (0x10)  //bit8
//#define REG_PWM5_PORARITY           (0x13)  //bit8
//#define REG_PWM6_PORARITY           (0x42)  //bit8
//#define REG_PWM7_PORARITY           (0x45)  //bit8
//#define REG_PWM8_PORARITY           (0x48)  //bit8

#define REG_PWM0_VDBEN              (0x04)  //bit9
#define REG_PWM1_VDBEN              (0x07)  //bit9
#define REG_PWM2_VDBEN              (0x0A)  //bit9
#define REG_PWM3_VDBEN              (0x0D)  //bit9
#define REG_PWM4_VDBEN              (0x10)  //bit9
//#define REG_PWM5_VDBEN              (0x13)  //bit9
//#define REG_PWM6_VDBEN              (0x42)  //bit9
//#define REG_PWM7_VDBEN              (0x45)  //bit9
//#define REG_PWM8_VDBEN              (0x48)  //bit9

#define REG_PWM0_RESET_EN           (0x04)  //bit10
#define REG_PWM1_RESET_EN           (0x07)  //bit10
#define REG_PWM2_RESET_EN           (0x0A)  //bit10
#define REG_PWM3_RESET_EN           (0x0D)  //bit10
#define REG_PWM4_RESET_EN           (0x10)  //bit10
//#define REG_PWM5_RESET_EN           (0x13)  //bit10
//#define REG_PWM6_RESET_EN           (0x42)  //bit10
//#define REG_PWM7_RESET_EN           (0x45)  //bit10
//#define REG_PWM8_RESET_EN           (0x48)  //bit10

#define REG_PWM0_DBEN               (0x04)  //bit11
#define REG_PWM1_DBEN               (0x07)  //bit11
#define REG_PWM2_DBEN               (0x0A)  //bit11
#define REG_PWM3_DBEN               (0x0D)  //bit11
#define REG_PWM4_DBEN               (0x10)  //bit11
//#define REG_PWM5_DBEN               (0x13)  //bit11
//#define REG_PWM6_DBEN               (0x42)  //bit11
//#define REG_PWM7_DBEN               (0x45)  //bit11
//#define REG_PWM8_DBEN               (0x48)  //bit11

#define REG_PWM0_IMPULSE_EN         (0x04)  //bit12
#define REG_PWM1_IMPULSE_EN         (0x07)  //bit12
#define REG_PWM2_IMPULSE_EN         (0x0A)  //bit12
#define REG_PWM3_IMPULSE_EN         (0x0D)  //bit12
#define REG_PWM4_IMPULSE_EN         (0x10)  //bit12
#define REG_PWM5_IMPULSE_EN         (0x13)  //bit12
#define REG_PWM6_IMPULSE_EN         (0x42)  //bit12
#define REG_PWM7_IMPULSE_EN         (0x45)  //bit12
#define REG_PWM8_IMPULSE_EN         (0x48)  //bit12

#define REG_PWM0_ODDEVEN_SYNC       (0x04)  //bit13
#define REG_PWM1_ODDEVEN_SYNC       (0x07)  //bit13
#define REG_PWM2_ODDEVEN_SYNC       (0x0A)  //bit13
#define REG_PWM3_ODDEVEN_SYNC       (0x0D)  //bit13
#define REG_PWM4_ODDEVEN_SYNC       (0x10)  //bit13
#define REG_PWM5_ODDEVEN_SYNC       (0x13)  //bit13
#define REG_PWM6_ODDEVEN_SYNC       (0x42)  //bit13
#define REG_PWM7_ODDEVEN_SYNC       (0x45)  //bit13
#define REG_PWM8_ODDEVEN_SYNC       (0x48)  //bit13

// Add from T8
#define REG_PWM0_VDBEN_SW           (0x04)  //bit14
#define REG_PWM1_VDBEN_SW           (0x07)  //bit14
#define REG_PWM2_VDBEN_SW           (0x0A)  //bit14
#define REG_PWM3_VDBEN_SW           (0x0D)  //bit14
#define REG_PWM4_VDBEN_SW           (0x10)  //bit14

/* If chiptop provides the related reg, please use them at chiptop reg. */
//#define REG_PWM0_OEN                (0x04)  //bit15
//#define REG_PWM1_OEN                (0x07)  //bit15
//#define REG_PWM2_OEN                (0x0a)  //bit15
//#define REG_PWM3_OEN                (0x0d)  //bit15
//#define REG_PWM4_OEN                (0x10)  //bit15
//#define REG_PWM5_OEN                (0x13)  //bit15
//#define REG_PWM6_OEN                (0x42)  //bit15
//#define REG_PWM7_OEN                (0x45)  //bit15
//#define REG_PWM8_OEN                (0x48)  //bit15

#define REG_RST_MUX0                (0x14)  //bit15
#define REG_RST_MUX1                (0x14)  //bit7
#define REG_RST_MUX2                (0x15)  //bit15
#define REG_RST_MUX3                (0x15)  //bit7
#define REG_RST_MUX4                (0x16)  //bit15
//#define REG_RST_MUX5                (0x16)  //bit7
//#define REG_RST_MUX6                (0x49)  //bit15
//#define REG_RST_MUX7                (0x49)  //bit7
//#define REG_RST_MUX8                (0x4A)  //bit15

#define REG_HS_RST_CNT0             (0x14)  //bit8~11
#define REG_HS_RST_CNT1             (0x14)  //bit0~3
#define REG_HS_RST_CNT2             (0x15)  //bit8~11
#define REG_HS_RST_CNT3             (0x15)  //bit0~3
#define REG_HS_RST_CNT4             (0x16)  //bit8~11
//#define REG_HS_RST_CNT5             (0x16)  //bit0~3
//#define REG_HS_RST_CNT6             (0x49)  //bit8~11
//#define REG_HS_RST_CNT7             (0x49)  //bit0~3
//#define REG_HS_RST_CNT8             (0x4A)  //bit8~11

#define REG_PWM0_PERIOD_EXT         (0x20)  //bit0~1
#define REG_PWM1_PERIOD_EXT         (0x20)  //bit2~3
#define REG_PWM2_PERIOD_EXT         (0x20)  //bit4~5
#define REG_PWM3_PERIOD_EXT         (0x20)  //bit6~7
#define REG_PWM4_PERIOD_EXT         (0x20)  //bit8~9
//#define REG_PWM5_PERIOD_EXT         (0x20)  //bit10~11
//#define REG_PWM6_PERIOD_EXT         (0x4B)  //bit0~1
//#define REG_PWM7_PERIOD_EXT         (0x4B)  //bit2~3
//#define REG_PWM8_PERIOD_EXT         (0x4B)  //bit4~5

#define REG_PWM0_DUTY_EXT           (0x21)  //bit0~1
#define REG_PWM1_DUTY_EXT           (0x21)  //bit2~3
#define REG_PWM2_DUTY_EXT           (0x21)  //bit4~5
#define REG_PWM3_DUTY_EXT           (0x21)  //bit6~7
#define REG_PWM4_DUTY_EXT           (0x21)  //bit8~9
//#define REG_PWM5_DUTY_EXT           (0x21)  //bit10~11
//#define REG_PWM6_DUTY_EXT           (0x4B)  //bit8~9
//#define REG_PWM7_DUTY_EXT           (0x4B)  //bit10~11
//#define REG_PWM8_DUTY_EXT           (0x4B)  //bit12~13

#define REG_PWM0_DIV_EXT            (0x22)  //bit0~7
#define REG_PWM1_DIV_EXT            (0x22)  //bit8~15
#define REG_PWM2_DIV_EXT            (0x23)  //bit0~7
#define REG_PWM3_DIV_EXT            (0x23)  //bit8~15
#define REG_PWM4_DIV_EXT            (0x24)  //bit0~7
//#define REG_PWM5_DIV_EXT            (0x24)  //bit8~15
//#define REG_PWM6_DIV_EXT            (0x4C)  //bit0~7
//#define REG_PWM7_DIV_EXT            (0x4C)  //bit8~15
//#define REG_PWM8_DIV_EXT            (0x4D)  //bit0~7

#define REG_PWM0_SHIFT_L            (0x28)  //bit0~15
#define REG_PWM0_SHIFT_H            (0x29)  //bit0~1
#define REG_PWM1_SHIFT_L            (0x2A)  //bit0~15
#define REG_PWM1_SHIFT_H            (0x2B)  //bit0~1
#define REG_PWM2_SHIFT_L            (0x2C)  //bit0~15
#define REG_PWM2_SHIFT_H            (0x2D)  //bit0~1
#define REG_PWM3_SHIFT_L            (0x2E)  //bit0~15
#define REG_PWM3_SHIFT_H            (0x2F)  //bit0~1
#define REG_PWM4_SHIFT_L            (0x30)  //bit0~15
#define REG_PWM4_SHIFT_H            (0x31)  //bit0~1
//#define REG_PWM5_SHIFT_L            (0x32)  //bit0~15
//#define REG_PWM5_SHIFT_H            (0x33)  //bit0~1
//#define REG_PWM6_SHIFT_L            (0x4E)  //bit0~15
//#define REG_PWM6_SHIFT_H            (0x4F)  //bit0~1
//#define REG_PWM7_SHIFT_L            (0x50)  //bit0~15
//#define REG_PWM7_SHIFT_H            (0x51)  //bit0~1
//#define REG_PWM8_SHIFT_L            (0x52)  //bit0~15
//#define REG_PWM8_SHIFT_H            (0x53)  //bit0~1

#define REG_PWM0_NVS            (0x34)  //bit0
#define REG_PWM1_NVS            (0x34)  //bit1
#define REG_PWM2_NVS            (0x34)  //bit2
#define REG_PWM3_NVS            (0x34)  //bit3
#define REG_PWM4_NVS            (0x34)  //bit4

#define REG_PWM0_Align          (0x35)  //bit0
#define REG_PWM1_Align          (0x35)  //bit1
#define REG_PWM2_Align          (0x35)  //bit2
#define REG_PWM3_Align          (0x35)  //bit3
#define REG_PWM4_Align          (0x35)  //bit4

#define REG_PWM0_ForceShitf     (0x35)  //bit6
#define REG_PWM1_ForceShitf     (0x35)  //bit7
#define REG_PWM2_ForceShitf     (0x35)  //bit8
#define REG_PWM3_ForceShitf     (0x35)  //bit9
#define REG_PWM4_ForceShitf     (0x35)  //bit10




#define reg_pwm_as_chip_config      (0x70)  //bit0
#define REG_PM_PWM0_IS_GPIO         (0x1C)  //bit5
#define REG_PM_PWM0_PERIOD          (0x6A)  //bit0~15
#define REG_PM_PWM0_DUTY            (0x69)  //bit0~15
#define REG_PM_PWM0_DIV             (0x68)  //bit0~7
#define REG_PM_PWM0_PORARITY        (0x6B)  //bit0
#define REG_PM_PWM0_DBEN            (0x6B)  //bit1


//For Debug Use
#define REG_PWM_DUMMY2              (0x36)  //bit0~15 Default:0x0000
#define REG_PWM_DUMMY3              (0x37)  //bit0~15 Default:0xFFFF

#endif // _REG_PWM_H_

