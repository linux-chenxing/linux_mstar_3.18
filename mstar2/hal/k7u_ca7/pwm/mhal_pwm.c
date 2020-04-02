//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2012 MStar Semiconductor, Inc.
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
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define _HAL_PWM_C

////////////////////////////////////////////////////////////////////////////////
/// @file mhal_PWM.c
/// @author MStar Semiconductor Inc.
/// @brief Pulse Width Modulation driver
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header Files
////////////////////////////////////////////////////////////////////////////////
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/version.h>
#include <asm/io.h>
#include <asm/uaccess.h>
//#include "MsVersion.h"
#include "mdrv_types.h"
#include "mdrv_pwm.h"/* this is not good idea, just for temp. */
#include "mhal_pwm.h"
#include "mreg_pwm.h"

////////////////////////////////////////////////////////////////////////////////
// Global variable
////////////////////////////////////////////////////////////////////////////////
extern ptrdiff_t mstar_pm_base;

//static U32 _gMIO_MapBase = 0;
//static U32 _gMIO_PM_MapBase = 0;
static U16 _gPWM_Status  = 0;

//static BOOL _gPWM_DBen   = 0;
static BOOL _gPWM_VDBen  = 0;

static void _HAL_PWM_VDBen_SW(PWM_ChNum index, BOOL bSwitch);
////////////////////////////////////////////////////////////////////////////////
// Define & data type
///////////////////////////////////////////////////////////////////////////////
#define WRITE_WORD_MASK(_reg, _val, _mask)  { (*((volatile U16*)(_reg))) = ((*((volatile U16*)(_reg))) & ~(_mask)) | ((U16)(_val) & (_mask)); }

#define HAL_PWM_ReadByte(addr)		     READ_BYTE((REG_PWM_BASE ) + ((addr)<<2))
#define HAL_PWM_Read2Byte(addr)              READ_WORD((REG_PWM_BASE ) + ((addr)<<2))

#define HAL_PWM_WriteByte(addr, val) 	     WRITE_BYTE((REG_PWM_BASE ) + ((addr)<<2), (val))
#define HAL_PWM_Write2Byte(addr, val)        WRITE_WORD((REG_PWM_BASE ) + ((addr)<<2), (val))
#define HAL_PWM_WriteRegBit(addr, val, mask) WRITE_WORD_MASK((REG_PWM_BASE ) + ((addr)<<2), (val), (mask))

#define HAL_TOP_ReadByte(addr)		     READ_BYTE((mstar_pm_base + REG_TOP_BASE) + ((addr)<<2))
#define HAL_TOP_Read2Byte(addr)              READ_WORD((mstar_pm_base + REG_TOP_BASE) + ((addr)<<2))
#define HAL_TOP_WriteRegBit(addr, val, mask) WRITE_WORD_MASK((mstar_pm_base + REG_TOP_BASE) + ((addr)<<2), (val), (mask))

#define HAL_PM_WriteByte(addr, val) 	     WRITE_BYTE((REG_PWM_BASE ) + ((addr)<<2), (val))
#define HAL_PM_Write2Byte(addr, val)        WRITE_WORD((REG_PWM_BASE ) + ((addr)<<2), (val))
#define HAL_PM_WriteRegBit(addr, val, mask) WRITE_WORD_MASK((REG_PWM_BASE ) + ((addr)<<2), (val), (mask))

#define HAL_SUBBANK0    //HAL_PWM_WriteByte(0,0)
#define HAL_SUBBANK1    //HAL_PWM_WriteByte(0,1) /* PWM sub-bank */

////////////////////////////////////////////////////////////////////////////////
// Global Function
////////////////////////////////////////////////////////////////////////////////


static void _HAL_PWM_VDBen_SW(PWM_ChNum index, BOOL bSwitch)
{
	if(_gPWM_VDBen)
	{
		//printk("%s(0x%08X, %x)", __FUNCTION__, (int)index, bSwitch);
		switch(index)
    	        {
        	case E_PWM_CH0:
            	HAL_PWM_WriteRegBit(REG_PWM0_VDBEN_SW,BITS(14:14,bSwitch),BMASK(14:14));
            	break;
        	case E_PWM_CH1:
				HAL_PWM_WriteRegBit(REG_PWM1_VDBEN_SW,BITS(14:14,bSwitch),BMASK(14:14));
            	break;
        	case E_PWM_CH2:
				HAL_PWM_WriteRegBit(REG_PWM2_VDBEN_SW,BITS(14:14,bSwitch),BMASK(14:14));
            	break;
        	case E_PWM_CH3:
				HAL_PWM_WriteRegBit(REG_PWM3_VDBEN_SW,BITS(14:14,bSwitch),BMASK(14:14));
            	break;
        	case E_PWM_CH4:
				HAL_PWM_WriteRegBit(REG_PWM4_VDBEN_SW,BITS(14:14,bSwitch),BMASK(14:14));
            	break;
        	case E_PWM_CH5:
        	case E_PWM_CH6:
        	case E_PWM_CH7:
        	case E_PWM_CH8:
        	case E_PWM_CH9:
				printk("[Utopia] The PWM%d is not support\n", (int)index);
            	UNUSED(bSwitch);
            	break;
			default:
				break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name: HAL_PWM_Init
/// @brief \b Function   \b Description: Initial PWM
/// @param <IN>          \b None :
/// @param <OUT>       \b None :
/// @param <RET>        \b BOOL :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_PWM_Init(void)
{
    BOOL ret = FALSE;
#if 0
    HAL_PWM_WriteRegBit(REG_ALL_PAD_IN, BIT7, 0);
    HAL_PWM_WriteRegBit(REG_PWM_IS_GPIO, BIT0, 0);
    HAL_PWM_WriteRegBit(REG_PWM_IS_GPIO, BIT1, 0);
    HAL_PWM_WriteRegBit(REG_PWM_IS_GPIO, BIT2, 0);
    HAL_PWM_WriteRegBit(REG_PWM_IS_GPIO, BIT3, 0);
#endif
	_gPWM_Status = HAL_TOP_Read2Byte(REG_PWM_MODE);

	if(_gPWM_Status&PAD_PWM0_OUT)
	{
		printk("Init PWM0\n");
	}
	if(_gPWM_Status&PAD_PWM1_OUT)
	{
		printk("Init PWM1\n");
	}
	if(_gPWM_Status&PAD_PWM2_OUT)
	{
		printk("Init PWM2\n");
	}
	if(_gPWM_Status&PAD_PWM3_OUT)
	{
		printk("Init PWM3\n");
	}
	if(_gPWM_Status&PAD_PWM4_OUT)
	{
		printk("Init PWM4\n");
	}
    ret = TRUE;
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_PWM_Oen
/// @brief \b Function  \b Description: Switch PWM PAD as Output or Input
/// @param <IN>         \b U16 : index
/// @param <IN>         \b BOOL : letch, 1 for Input; 0 for Output
/// @param <OUT>      \b None :
/// @param <RET>       \b BOOL :
/// @param <GLOBAL>  \b None :
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_PWM_Oen(PWM_ChNum index, BOOL letch)
{

    //Use the PWM oen in ChipTop Reg first, if it provides for.

    switch(index)
    {
        case E_PWM_CH0:
            HAL_TOP_WriteRegBit(REG_PWM_OEN,BITS(0:0,letch), BMASK(0:0));
            break;
        case E_PWM_CH1:
			HAL_TOP_WriteRegBit(REG_PWM_OEN,BITS(1:1,letch), BMASK(1:1));
            break;
        case E_PWM_CH2:
			HAL_TOP_WriteRegBit(REG_PWM_OEN,BITS(2:2,letch), BMASK(2:2));
            break;
        case E_PWM_CH3:
			HAL_TOP_WriteRegBit(REG_PWM_OEN,BITS(3:3,letch), BMASK(3:3));
            break;
        case E_PWM_CH4:
			HAL_TOP_WriteRegBit(REG_PWM_OEN,BITS(4:4,letch), BMASK(4:4));
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
			printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(letch);
            break;
		default:
			break;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_UnitDiv
/// @brief \b Function   \b Description : Set the Unit_Div of the pwm
/// @param <IN>          \b U16 : the Unit_Div value
/// @param <OUT>       \b None :
/// @param <RET>        \b BOOL : 1 for doen; 0 for not done
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_PWM_UnitDiv(U16 u16DivPWM)
{
    BOOL ret = FALSE;
    /* the PWM clock unit divider is NO USE in T3 */
    UNUSED(u16DivPWM);
    printk("[Utopia] T8 is not support\n");
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_Period
/// @brief \b Function   \b Description : Set the period of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b U32 : the 18-bit Period value
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_Period(PWM_ChNum index, U32 u32PeriodPWM)
{
    U16  Period_L, Period_H;
    Period_L = (U16)u32PeriodPWM;
    Period_H = (U16)(u32PeriodPWM >> 16);
    HAL_SUBBANK1;
    _HAL_PWM_VDBen_SW(index,0);
    /* the Period capability is restricted to ONLY 18-bit */
    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_Write2Byte(REG_PWM0_PERIOD, Period_L);
            HAL_PWM_WriteRegBit(REG_PWM0_PERIOD_EXT,BITS(1:0,Period_H),BMASK(1:0));
            break;
        case E_PWM_CH1:
            HAL_PWM_Write2Byte(REG_PWM1_PERIOD, Period_L);
            HAL_PWM_WriteRegBit(REG_PWM1_PERIOD_EXT,BITS(3:2,Period_H),BMASK(3:2));
            break;
        case E_PWM_CH2:
            HAL_PWM_Write2Byte(REG_PWM2_PERIOD, Period_L);
            HAL_PWM_WriteRegBit(REG_PWM2_PERIOD_EXT,BITS(5:4,Period_H),BMASK(5:4));
            break;
        case E_PWM_CH3:
            HAL_PWM_Write2Byte(REG_PWM3_PERIOD, Period_L);
            HAL_PWM_WriteRegBit(REG_PWM3_PERIOD_EXT,BITS(7:6,Period_H),BMASK(7:6));
            break;
        case E_PWM_CH4:
            HAL_PWM_Write2Byte(REG_PWM4_PERIOD, Period_L);
            HAL_PWM_WriteRegBit(REG_PWM4_PERIOD_EXT,BITS(9:8,Period_H),BMASK(9:8));
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
	    printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(Period_L);
            UNUSED(Period_H);
            break;
		default:
			break;
    }
    _HAL_PWM_VDBen_SW(index,1);
    HAL_SUBBANK0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_DutyCycle
/// @brief \b Function   \b Description : Set the Duty of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b U32 : the 18-bit Duty value
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_DutyCycle(PWM_ChNum index, U32 u32DutyPWM)
{
    U16  Duty_L, Duty_H;

    Duty_L = (U16)u32DutyPWM;
    Duty_H = (U8)(u32DutyPWM >> 16);
    HAL_SUBBANK1;
    _HAL_PWM_VDBen_SW(index,0);
    /* the Duty capability is restricted to ONLY 10-bit */
    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_Write2Byte(REG_PWM0_DUTY, Duty_L);
            HAL_PWM_WriteRegBit(REG_PWM0_DUTY_EXT,BITS(1:0,Duty_H),BMASK(1:0));
            break;
        case E_PWM_CH1:
            HAL_PWM_Write2Byte(REG_PWM1_DUTY, Duty_L);
            HAL_PWM_WriteRegBit(REG_PWM1_DUTY_EXT,BITS(3:2,Duty_H),BMASK(3:2));
            break;
        case E_PWM_CH2:
            HAL_PWM_Write2Byte(REG_PWM2_DUTY, Duty_L);
	    HAL_PWM_WriteRegBit(REG_PWM2_DUTY_EXT,BITS(5:4,Duty_H),BMASK(5:4));
            break;
        case E_PWM_CH3:
            HAL_PWM_Write2Byte(REG_PWM3_DUTY, Duty_L);
            HAL_PWM_WriteRegBit(REG_PWM3_DUTY_EXT,BITS(7:6,Duty_H),BMASK(7:6));
            break;
        case E_PWM_CH4:
            HAL_PWM_Write2Byte(REG_PWM4_DUTY, Duty_L);
            HAL_PWM_WriteRegBit(REG_PWM4_DUTY_EXT,BITS(9:8,Duty_H),BMASK(9:8));
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
			printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(Duty_L);
            UNUSED(Duty_H);
            break;
		default:
			break;
    }
    _HAL_PWM_VDBen_SW(index,1);
    HAL_SUBBANK0;
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_Div
/// @brief \b Function   \b Description : Set the Div of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b U16 : the 16-bit Div value
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_Div(PWM_ChNum index, U16 u16DivPWM)
{
	U8 u8DivPWM[2] = {0};

	u8DivPWM[0] = (U8)(u16DivPWM&0xFF);
	u8DivPWM[1] = (U8)(u16DivPWM >> 8);

    HAL_SUBBANK1;
    /* the Div capability is restricted to ONLY 16-bit */
    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_WriteByte(REG_PWM0_DIV,     u8DivPWM[0]);
            HAL_PWM_WriteByte(REG_PWM0_DIV_EXT, u8DivPWM[1]);
            break;
        case E_PWM_CH1:
            HAL_PWM_WriteByte(REG_PWM1_DIV,     u8DivPWM[0]);
            //HAL_PWM_WriteByte(REG_PWM1_DIV_EXT+1, u8DivPWM[1]);
			HAL_PWM_WriteRegBit(REG_PWM1_DIV_EXT,u16DivPWM,BMASK(15:8));
            break;
        case E_PWM_CH2:
            HAL_PWM_WriteByte(REG_PWM2_DIV,     u8DivPWM[0]);
            HAL_PWM_WriteByte(REG_PWM2_DIV_EXT, u8DivPWM[1]);
            break;
        case E_PWM_CH3:
            HAL_PWM_WriteByte(REG_PWM3_DIV,     u8DivPWM[0]);
            //HAL_PWM_WriteByte(REG_PWM3_DIV_EXT+1, u8DivPWM[1]);
            HAL_PWM_WriteRegBit(REG_PWM3_DIV_EXT,u16DivPWM,BMASK(15:8));
            break;
        case E_PWM_CH4:
            HAL_PWM_WriteByte(REG_PWM4_DIV, 	u8DivPWM[0]);
            HAL_PWM_WriteByte(REG_PWM4_DIV_EXT, u8DivPWM[1]);
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
	    printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(u16DivPWM);
            break;
		default:
			break;
    }
    HAL_SUBBANK0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_Polarity
/// @brief \b Function   \b Description : Set the Polarity of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b BOOL : 1 for enable; 0 for disable
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_Polarity(PWM_ChNum index, BOOL bPolPWM)
{
    HAL_SUBBANK1;
    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_WriteRegBit(REG_PWM0_PORARITY,BITS(8:8,bPolPWM),BMASK(8:8));
            break;
        case E_PWM_CH1:
            HAL_PWM_WriteRegBit(REG_PWM1_PORARITY,BITS(8:8,bPolPWM),BMASK(8:8));
            break;
        case E_PWM_CH2:
            HAL_PWM_WriteRegBit(REG_PWM2_PORARITY,BITS(8:8,bPolPWM),BMASK(8:8));
            break;
        case E_PWM_CH3:
            HAL_PWM_WriteRegBit(REG_PWM3_PORARITY,BITS(8:8,bPolPWM),BMASK(8:8));
            break;
        case E_PWM_CH4:
            HAL_PWM_WriteRegBit(REG_PWM4_PORARITY,BITS(8:8,bPolPWM),BMASK(8:8));
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
	    printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(bPolPWM);
            break;
		default:
			break;
    }
    HAL_SUBBANK0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_VDBen
/// @brief \b Function   \b Description : Set the Vsync Double buffer of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b BOOL : 1 for enable; 0 for disable
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_VDBen(PWM_ChNum index, BOOL bVdbenPWM)
{
    _gPWM_VDBen = bVdbenPWM;

    HAL_SUBBANK1;

    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_WriteRegBit(REG_PWM0_VDBEN,BITS(9:9,bVdbenPWM),BMASK(9:9));
            break;
        case E_PWM_CH1:
            HAL_PWM_WriteRegBit(REG_PWM1_VDBEN,BITS(9:9,bVdbenPWM),BMASK(9:9));
            break;
        case E_PWM_CH2:
            HAL_PWM_WriteRegBit(REG_PWM2_VDBEN,BITS(9:9,bVdbenPWM),BMASK(9:9));
            break;
        case E_PWM_CH3:
            HAL_PWM_WriteRegBit(REG_PWM3_VDBEN,BITS(9:9,bVdbenPWM),BMASK(9:9));
            break;
        case E_PWM_CH4:
            HAL_PWM_WriteRegBit(REG_PWM4_VDBEN,BITS(9:9,bVdbenPWM),BMASK(9:9));
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
	    printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(bVdbenPWM);
            break;
		default:
			break;
    }

    HAL_SUBBANK0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_Vrest
/// @brief \b Function   \b Description : Set the Hsync reset of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b BOOL : 1 for enable; 0 for disable
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_Vrest(PWM_ChNum index, BOOL bRstPWM)
{
    HAL_SUBBANK1;

    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_WriteRegBit(REG_PWM0_RESET_EN,BITS(10:10,bRstPWM),BMASK(10:10));
            break;
        case E_PWM_CH1:
            HAL_PWM_WriteRegBit(REG_PWM1_RESET_EN,BITS(10:10,bRstPWM),BMASK(10:10));
            break;
        case E_PWM_CH2:
            HAL_PWM_WriteRegBit(REG_PWM2_RESET_EN,BITS(10:10,bRstPWM),BMASK(10:10));
            break;
        case E_PWM_CH3:
            HAL_PWM_WriteRegBit(REG_PWM3_RESET_EN,BITS(10:10,bRstPWM),BMASK(10:10));
            break;
        case E_PWM_CH4:
            HAL_PWM_WriteRegBit(REG_PWM4_RESET_EN,BITS(10:10,bRstPWM),BMASK(10:10));
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
	    printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(bRstPWM);
            break;
		default:
			break;
    }

    HAL_SUBBANK0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_DBen
/// @brief \b Function   \b Description : Set the Double buffer of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b BOOL : 1 for enable; 0 for disable
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_DBen(PWM_ChNum index, BOOL bdbenPWM)
{
	//_gPWM_DBen = bdbenPWM;

    HAL_SUBBANK1;

    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_WriteRegBit(REG_PWM0_DBEN,BITS(11:11,bdbenPWM),BMASK(11:11));
            break;
        case E_PWM_CH1:
            HAL_PWM_WriteRegBit(REG_PWM1_DBEN,BITS(11:11,bdbenPWM),BMASK(11:11));
            break;
        case E_PWM_CH2:
            HAL_PWM_WriteRegBit(REG_PWM2_DBEN,BITS(11:11,bdbenPWM),BMASK(11:11));
            break;
        case E_PWM_CH3:
            HAL_PWM_WriteRegBit(REG_PWM3_DBEN,BITS(11:11,bdbenPWM),BMASK(11:11));
            break;
        case E_PWM_CH4:
            HAL_PWM_WriteRegBit(REG_PWM4_DBEN,BITS(11:11,bdbenPWM),BMASK(11:11));
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
	    printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(bdbenPWM);
            break;
		default:
			break;
    }

    HAL_SUBBANK0;
}

void HAL_PWM_IMPULSE_EN(PWM_ChNum index, BOOL bdbenPWM)
{
    HAL_SUBBANK1;

    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_WriteRegBit(REG_PWM0_IMPULSE_EN,BITS(12:12,bdbenPWM),BMASK(12:12));
            break;
        case E_PWM_CH1:
            HAL_PWM_WriteRegBit(REG_PWM1_IMPULSE_EN,BITS(12:12,bdbenPWM),BMASK(12:12));
            break;
        case E_PWM_CH2:
            HAL_PWM_WriteRegBit(REG_PWM2_IMPULSE_EN,BITS(12:12,bdbenPWM),BMASK(12:12));
            break;
        case E_PWM_CH3:
            HAL_PWM_WriteRegBit(REG_PWM3_IMPULSE_EN,BITS(12:12,bdbenPWM),BMASK(12:12));
            break;
        case E_PWM_CH4:
            HAL_PWM_WriteRegBit(REG_PWM4_IMPULSE_EN,BITS(12:12,bdbenPWM),BMASK(12:12));
            break;
        case E_PWM_CH5:
            HAL_PWM_WriteRegBit(REG_PWM5_IMPULSE_EN,BITS(12:12,bdbenPWM),BMASK(12:12));
            break;
        case E_PWM_CH6:
            HAL_PWM_WriteRegBit(REG_PWM6_IMPULSE_EN,BITS(12:12,bdbenPWM),BMASK(12:12));
            break;
        case E_PWM_CH7:
            HAL_PWM_WriteRegBit(REG_PWM7_IMPULSE_EN,BITS(12:12,bdbenPWM),BMASK(12:12));
            break;
        case E_PWM_CH8:
            HAL_PWM_WriteRegBit(REG_PWM8_IMPULSE_EN,BITS(12:12,bdbenPWM),BMASK(12:12));
            break;
        case E_PWM_CH9:
            UNUSED(bdbenPWM);
            break;
		default:
			break;
    }

    HAL_SUBBANK0;
}


void HAL_PWM_ODDEVEN_SYNC(PWM_ChNum index, BOOL bdbenPWM)
{
    HAL_SUBBANK1;

    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_WriteRegBit(REG_PWM0_ODDEVEN_SYNC,BITS(13:13,bdbenPWM),BMASK(13:13));
            break;
        case E_PWM_CH1:
            HAL_PWM_WriteRegBit(REG_PWM1_ODDEVEN_SYNC,BITS(13:13,bdbenPWM),BMASK(13:13));
            break;
        case E_PWM_CH2:
            HAL_PWM_WriteRegBit(REG_PWM2_ODDEVEN_SYNC,BITS(13:13,bdbenPWM),BMASK(13:13));
            break;
        case E_PWM_CH3:
            HAL_PWM_WriteRegBit(REG_PWM3_ODDEVEN_SYNC,BITS(13:13,bdbenPWM),BMASK(13:13));
            break;
        case E_PWM_CH4:
            HAL_PWM_WriteRegBit(REG_PWM4_ODDEVEN_SYNC,BITS(13:13,bdbenPWM),BMASK(13:13));
            break;
        case E_PWM_CH5:
            HAL_PWM_WriteRegBit(REG_PWM5_ODDEVEN_SYNC,BITS(13:13,bdbenPWM),BMASK(13:13));
            break;
        case E_PWM_CH6:
            HAL_PWM_WriteRegBit(REG_PWM6_ODDEVEN_SYNC,BITS(13:13,bdbenPWM),BMASK(13:13));
            break;
        case E_PWM_CH7:
            HAL_PWM_WriteRegBit(REG_PWM7_ODDEVEN_SYNC,BITS(13:13,bdbenPWM),BMASK(13:13));
            break;
        case E_PWM_CH8:
            HAL_PWM_WriteRegBit(REG_PWM8_ODDEVEN_SYNC,BITS(13:13,bdbenPWM),BMASK(13:13));
            break;
        case E_PWM_CH9:
            UNUSED(bdbenPWM);
            break;
		default:
			break;
    }

    HAL_SUBBANK0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_RstMux
/// @brief \b Function   \b Description : Set the Rst Mux of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b BOOL : 1 for Hsync; 0 for Vsync
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_RstMux(PWM_ChNum index, BOOL bMuxPWM)
{
    HAL_SUBBANK1;

    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_WriteRegBit(REG_RST_MUX0,BITS(15:15,bMuxPWM),BMASK(15:15));
            break;
        case E_PWM_CH1:
            HAL_PWM_WriteRegBit(REG_RST_MUX1,BITS(7:7,bMuxPWM),BMASK(7:7));
            break;
        case E_PWM_CH2:
            HAL_PWM_WriteRegBit(REG_RST_MUX2,BITS(15:15,bMuxPWM),BMASK(15:15));
            break;
        case E_PWM_CH3:
            HAL_PWM_WriteRegBit(REG_RST_MUX3,BITS(7:7,bMuxPWM),BMASK(7:7));
            break;
        case E_PWM_CH4:
            HAL_PWM_WriteRegBit(REG_RST_MUX4,BITS(15:15,bMuxPWM),BMASK(15:15));
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
	    printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(bMuxPWM);
            break;
		default:
			break;
    }

    HAL_SUBBANK0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_RstCnt
/// @brief \b Function   \b Description : Set the Rst_Cnt of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b U8 : u8RstCntPWM
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_RstCnt(PWM_ChNum index, U8 u8RstCntPWM)
{
	if( u8RstCntPWM & 0x10 )
	{
		printk("PWM%d Reset Count is too large\n", index);
	}

    HAL_SUBBANK1;
    /* the Hsync reset counter capability is restricted to ONLY 4-bit */
    switch(index)
    {
        case E_PWM_CH0:
	    HAL_PWM_WriteRegBit(REG_HS_RST_CNT0,BITS(11:8,u8RstCntPWM),BMASK(11:8));
            break;
        case E_PWM_CH1:
            HAL_PWM_WriteRegBit(REG_HS_RST_CNT1,BITS(3:0,u8RstCntPWM),BMASK(3:0));
            break;
        case E_PWM_CH2:
            HAL_PWM_WriteRegBit(REG_HS_RST_CNT2,BITS(11:8,u8RstCntPWM),BMASK(11:8));
            break;
        case E_PWM_CH3:
            HAL_PWM_WriteRegBit(REG_HS_RST_CNT3,BITS(3:0,u8RstCntPWM),BMASK(3:0));
            break;
        case E_PWM_CH4:
            HAL_PWM_WriteRegBit(REG_HS_RST_CNT4,BITS(11:8,u8RstCntPWM),BMASK(11:8));
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
	    printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(u8RstCntPWM);
            break;
		default:
			break;
    }

    HAL_SUBBANK0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_BypassUnit
/// @brief \b Function   \b Description : Set the Bypass Unit of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b BOOL : 1 for enable; 0 for disable
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_PWM_BypassUnit(PWM_ChNum index, BOOL bBypassPWM)
{
    HAL_SUBBANK1;

    //T2 ONLY

    switch(index)
    {
        case E_PWM_CH0:
        case E_PWM_CH1:
        case E_PWM_CH2:
        case E_PWM_CH3:
        case E_PWM_CH4:
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
			printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(bBypassPWM);
            break;
		default:
			break;
    }

    HAL_SUBBANK0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM01_CntMode
/// @brief \b Function   \b Description :  Counter mode for PWM0 and PWM1
/// @param <IN>          \b PWM_ChNum : Enum of the Counter mode
/// @param <IN>          \b None :
/// @param <OUT>       \b None :
/// @param <RET>        \b PWM_Result :
/// @param <GLOBAL>   \b None :
/// @note                                                       \n
///     11: PWM1 donate internal divider to PWM0   \n
///     10: PWM0 donate internal divider to PWM1   \n
///     0x: Normal mode                                      \n
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_PWM01_CntMode(PWM_CntMode CntMode)
{
    BOOL ret = FALSE;
    //T2 ONLY
    UNUSED(CntMode);
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM23_CntMode
/// @brief \b Function   \b Description :  Counter mode for PWM2 and PWM3
/// @param <IN>          \b PWM_ChNum : Enum of the Counter mode
/// @param <IN>          \b None :
/// @param <OUT>       \b None :
/// @param <RET>        \b PWM_Result :
/// @param <GLOBAL>   \b None :
/// @note                                                       \n
///     11: PWM3 donate internal divider to PWM2   \n
///     10: PWM2 donate internal divider to PWM3   \n
///     0x: Normal mode                                      \n
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_PWM23_CntMode(PWM_CntMode CntMode)
{
    BOOL ret = FALSE;
    //T2 ONLY
    UNUSED(CntMode);
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM67_CntMode
/// @brief \b Function   \b Description :  Counter mode for PWM6 and PWM7
/// @param <IN>          \b PWM_ChNum : Enum of the Counter mode
/// @param <IN>          \b None :
/// @param <OUT>       \b None :
/// @param <RET>        \b PWM_Result :
/// @param <GLOBAL>   \b None :
/// @note                                                       \n
///     11: PWM7 donate internal divider to PWM6   \n
///     10: PWM6 donate internal divider to PWM7   \n
///     0x: Normal mode                                      \n
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_PWM67_CntMode(PWM_CntMode CntMode)
{
    BOOL ret = FALSE;
    //T2 ONLY
    UNUSED(CntMode);
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function   \b Name : HAL_PWM_Shift
/// @brief \b Function   \b Description : Set the Shift of the specific pwm
/// @param <IN>          \b PWM_ChNum : Enum of the PWM CH
/// @param <IN>          \b U16 : the 18-bit shift value
/// @param <OUT>       \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>   \b None :
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_PWM_Shift(PWM_ChNum index, U32 u32ShiftPWM)
{
    U16 Shift_L, Shift_H;

    Shift_L = (U16)(u32ShiftPWM & 0xFFFF);
    Shift_H = (U16)(u32ShiftPWM >> 16);

    HAL_SUBBANK1;

    switch(index)
    {
        case E_PWM_CH0:
            HAL_PWM_Write2Byte(REG_PWM0_SHIFT_L, Shift_L);
            HAL_PWM_Write2Byte(REG_PWM0_SHIFT_H, Shift_H);
            break;
        case E_PWM_CH1:
            HAL_PWM_Write2Byte(REG_PWM1_SHIFT_L, Shift_L);
            HAL_PWM_Write2Byte(REG_PWM1_SHIFT_H, Shift_H);
            break;
        case E_PWM_CH2:
            HAL_PWM_Write2Byte(REG_PWM2_SHIFT_L, Shift_L);
            HAL_PWM_Write2Byte(REG_PWM2_SHIFT_H, Shift_H);
            break;
        case E_PWM_CH3:
            HAL_PWM_Write2Byte(REG_PWM3_SHIFT_L, Shift_L);
            HAL_PWM_Write2Byte(REG_PWM3_SHIFT_H, Shift_H);
            break;
        case E_PWM_CH4:
            HAL_PWM_Write2Byte(REG_PWM4_SHIFT_L, Shift_L);
            HAL_PWM_Write2Byte(REG_PWM4_SHIFT_H, Shift_H);
            break;
        case E_PWM_CH5:
        case E_PWM_CH6:
        case E_PWM_CH7:
        case E_PWM_CH8:
        case E_PWM_CH9:
			printk("[Utopia] The PWM%d is not support\n", (int)index);
            UNUSED(Shift_L);UNUSED(Shift_H);
            break;
		default:
			break;
    }

    HAL_SUBBANK0;

    return TRUE;
}


void HAL_PWM_Nvsync(PWM_ChNum index, BOOL bNvsPWM)
    {

        HAL_SUBBANK1;

        switch(index)
        {
            case E_PWM_CH0:
                HAL_PWM_WriteRegBit(REG_PWM0_NVS,BITS(0:0,bNvsPWM),BMASK(0:0));
                break;
            case E_PWM_CH1:
                HAL_PWM_WriteRegBit(REG_PWM1_NVS,BITS(1:1,bNvsPWM),BMASK(1:1));
                break;
            case E_PWM_CH2:
                HAL_PWM_WriteRegBit(REG_PWM2_NVS,BITS(2:2,bNvsPWM),BMASK(2:2));
                break;
            case E_PWM_CH3:
                HAL_PWM_WriteRegBit(REG_PWM3_NVS,BITS(3:3,bNvsPWM),BMASK(3:3));
                break;
            case E_PWM_CH4:
                HAL_PWM_WriteRegBit(REG_PWM4_NVS,BITS(4:4,bNvsPWM),BMASK(4:4));
                break;
            case E_PWM_CH5:
            case E_PWM_CH6:
            case E_PWM_CH7:
            case E_PWM_CH8:
            case E_PWM_CH9:
            printk("[Utopia] The PWM%d is not support\n", (int)index);
                UNUSED(bNvsPWM);
                break;
            default:
                break;
        }

        HAL_SUBBANK0;
    }

void HAL_PWM_Align(PWM_ChNum index, BOOL bAliPWM)
    {

        HAL_SUBBANK1;

        switch(index)
        {
            case E_PWM_CH0:
                HAL_PWM_WriteRegBit(REG_PWM0_Align,BITS(0:0,bAliPWM),BMASK(0:0));
                break;
            case E_PWM_CH1:
                HAL_PWM_WriteRegBit(REG_PWM1_Align,BITS(1:1,bAliPWM),BMASK(1:1));
                break;
            case E_PWM_CH2:
                HAL_PWM_WriteRegBit(REG_PWM2_Align,BITS(2:2,bAliPWM),BMASK(2:2));
                break;
            case E_PWM_CH3:
                HAL_PWM_WriteRegBit(REG_PWM3_Align,BITS(3:3,bAliPWM),BMASK(3:3));
                break;
            case E_PWM_CH4:
                HAL_PWM_WriteRegBit(REG_PWM4_Align,BITS(4:4,bAliPWM),BMASK(4:4));
                break;
            case E_PWM_CH5:
            case E_PWM_CH6:
            case E_PWM_CH7:
            case E_PWM_CH8:
            case E_PWM_CH9:
            printk("[Utopia] The PWM%d is not support\n", (int)index);
                UNUSED(bAliPWM);
                break;
            default:
                break;
        }

        HAL_SUBBANK0;
    }

void HAL_PWM_ForceShitfEn(PWM_ChNum index, BOOL bAliPWM)
    {

        HAL_SUBBANK1;

        switch(index)
        {
            case E_PWM_CH0:
                HAL_PWM_WriteRegBit(REG_PWM0_ForceShitf,BITS(6:6,bAliPWM),BMASK(6:6));
                break;
            case E_PWM_CH1:
                HAL_PWM_WriteRegBit(REG_PWM1_ForceShitf,BITS(7:7,bAliPWM),BMASK(7:7));
                break;
            case E_PWM_CH2:
                HAL_PWM_WriteRegBit(REG_PWM2_ForceShitf,BITS(8:8,bAliPWM),BMASK(8:8));
                break;
            case E_PWM_CH3:
                HAL_PWM_WriteRegBit(REG_PWM3_ForceShitf,BITS(9:9,bAliPWM),BMASK(9:9));
                break;
            case E_PWM_CH4:
                HAL_PWM_WriteRegBit(REG_PWM4_ForceShitf,BITS(10:10,bAliPWM),BMASK(10:10));
                break;
            case E_PWM_CH5:
            case E_PWM_CH6:
            case E_PWM_CH7:
            case E_PWM_CH8:
            case E_PWM_CH9:
            printk("[Utopia] The PWM%d is not support\n", (int)index);
                UNUSED(bAliPWM);
                break;
            default:
                break;
        }

        HAL_SUBBANK0;
    }




//---------------------------PM Base--------------------------------//

void HAL_PM_PWM_Enable(void)
{
    HAL_PM_WriteRegBit(REG_PM_PWM0_IS_GPIO,BITS(5:5,0),BMASK(5:5));
    HAL_PM_WriteRegBit(reg_pwm_as_chip_config,BITS(0:0,0),BMASK(0:0));//reg_pwm_pm_is_PWM
}

void HAL_PM_PWM_Period(U16 u16PeriodPWM)
{
    U16  Period;
    Period = u16PeriodPWM;
    HAL_PM_Write2Byte(REG_PM_PWM0_PERIOD, Period);
}

void HAL_PM_PWM_DutyCycle(U16 u16DutyPWM)
{
    U16  Duty;
    Duty =u16DutyPWM;
    HAL_PM_Write2Byte(REG_PM_PWM0_DUTY, Duty);
}

void HAL_PM_PWM_Div(U8 u8DivPWM)
{
    U8 Div;
    Div = u8DivPWM;
    HAL_PM_Write2Byte(REG_PM_PWM0_DIV, Div);
}

void HAL_PM_PWM_Polarity(BOOL bPolPWM)
{
    HAL_PM_WriteRegBit(REG_PM_PWM0_PORARITY,BITS(0:0,bPolPWM),BMASK(0:0));
}

void HAL_PM_PWM_DBen(BOOL bdbenPWM)
{
    HAL_PM_WriteRegBit(REG_PM_PWM0_DBEN,BITS(1:1,bdbenPWM),BMASK(1:1));
}


