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
#ifndef __DRV_MOD_ST_H__
#define __DRV_MOD_ST_H__

//------------------------------------------------------------------------------
// Structure
//------------------------------------------------------------------------------
typedef enum
{
    MOD_CTRL0_LVDS_TI       = BIT2,
    MOD_CTRL0_PDP_10BIT     = BIT3,
    MOD_CTRL0_LVDS_PLASMA   = BIT4,
    MOD_CTRL0_CH_POLARITY   = BIT5,
    MOD_CTRL0_CH_SWAP       = BIT6,
} MOD_CTRL0_e;

typedef enum
{
    MOD_CTRLA_ABS_SWITCH    = BIT0,
    MOD_CTRLA_DUAL_MODE     = BIT1,
    MOD_CTRLA_DE_INVERT     = BIT2,
    MOD_CTRLA_VS_INVERT     = BIT3,
    MOD_CTRLA_CLK_INVERT    = BIT4,
} MOD_CTRLA_e;


#endif // __DRV_MOD_ST_H__ 

