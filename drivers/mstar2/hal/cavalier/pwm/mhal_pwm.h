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
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_PWM_H_
#define _HAL_PWM_H_

#include "mdrv_types.h"

////////////////////////////////////////////////////////////////////////////////
/// @file HALPWM.h
/// @author MStar Semiconductor Inc.
/// @brief Pulse Width Modulation hal
////////////////////////////////////////////////////////////////////////////////

#define PWM_Num     5       /* Max. 6 */

////////////////////////////////////////////////////////////////////////////////
// Macro for utility
////////////////////////////////////////////////////////////////////////////////
#define MST_MACRO_START     do {
#define MST_MACRO_END       } while (0)

#define UNUSED( var )       ((void)(var))

////////////////////////////////////////////////////////////////////////////////
// Macro for bitwise
////////////////////////////////////////////////////////////////////////////////
#define _BITMASK(loc_msb, loc_lsb) ((1U << (loc_msb)) - (1U << (loc_lsb)) + (1U << (loc_msb)))
#define BITMASK(x) _BITMASK(1?x, 0?x)
#define BITFLAG(loc) (1U << (loc))

#define SETBIT(REG, BIT)   ((REG) |= (1UL << (BIT)))
#define CLRBIT(REG, BIT)   ((REG) &= ~(1UL << (BIT)))
#define GETBIT(REG, BIT)   (((REG) >> (BIT)) & 0x01UL)
#define COMPLEMENT(a)      (~(a))
#define BITS(_bits_, _val_)         ((BIT(((1)?_bits_)+1)-BIT(((0)?_bits_))) & (_val_<<((0)?_bits_)))
#define BMASK(_bits_)               (BIT(((1)?_bits_)+1)-BIT(((0)?_bits_)))
#define READ_WORD(_reg)             (*(volatile U16*)(_reg))
#define WRITE_BYTE(_reg, _val)      { (*((volatile U8*)(_reg))) = (U8)(_val); }
#define WRITE_WORD(_reg, _val)      { (*((volatile U16*)(_reg))) = (U16)(_val); }

////////////////////////////////////////////////////////////////////////////////////////
// Extern function
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_PWM_Init(void);
BOOL HAL_PWM_Oen(PWM_ChNum index, BOOL letch);
BOOL HAL_PWM_UnitDiv(U16 u16DivPWM);
void HAL_PWM_Period(PWM_ChNum index, U32 u32PeriodPWM);
void HAL_PWM_DutyCycle(PWM_ChNum index, U32 u32DutyPWM);
void HAL_PWM_Div(PWM_ChNum index, U16 u16DivPWM);
void HAL_PWM_Polarity(PWM_ChNum index, BOOL bPolPWM);
void HAL_PWM_VDBen(PWM_ChNum index, BOOL bVdbenPWM);
void HAL_PWM_Vrest(PWM_ChNum index, BOOL bRstPWM);
void HAL_PWM_DBen(PWM_ChNum index, BOOL bdbenPWM);
void HAL_PWM_RstMux(PWM_ChNum index, BOOL bMuxPWM);
void HAL_PWM_RstCnt(PWM_ChNum index, U8 u8RstCntPWM);
void HAL_PWM_BypassUnit(PWM_ChNum index, BOOL bBypassPWM);
BOOL HAL_PWM01_CntMode(PWM_CntMode CntMode);
BOOL HAL_PWM23_CntMode(PWM_CntMode CntMode);
BOOL HAL_PWM67_CntMode(PWM_CntMode CntMode);
BOOL HAL_PWM_Shift(PWM_ChNum index, U32 u32ShiftPWM);
void HAL_PWM_IMPULSE_EN(PWM_ChNum index, BOOL bdbenPWM);
void HAL_PWM_ODDEVEN_SYNC(PWM_ChNum index, BOOL bdbenPWM);
void HAL_PWM_Nvsync(PWM_ChNum index, BOOL bNvsPWM);
void HAL_PWM_Align(PWM_ChNum index, BOOL bAliPWM);

void HAL_PM_PWM_Enable(void);
void HAL_PM_PWM_Period(U16 u16PeriodPWM);
void HAL_PM_PWM_DutyCycle(U16 u16DutyPWM);
void HAL_PM_PWM_Div(U8 u8DivPWM);
void HAL_PM_PWM_Polarity(BOOL bPolPWM);
void HAL_PM_PWM_DBen(BOOL bdbenPWM);

void HAL_PWM_ForceShitfEn(PWM_ChNum index, BOOL bAliPWM);

#endif // _HAL_PWM_H_

