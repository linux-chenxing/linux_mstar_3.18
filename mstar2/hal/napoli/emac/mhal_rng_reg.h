///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2009 - 2010 MStar Semiconductor, Inc.
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    regRNG.h
/// @brief  Random Number Generation Registers Definition
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_RNG_REG_H_
#define _MHAL_RNG_REG_H_

// unit: ms< --> ( HZ / 1000 )
#define InputRNGJiffThreshold    (10 * ( HZ / 1000 ))
#define RIU_MAP                              0xFD200000
#define RIU                                         ((unsigned short volatile *) RIU_MAP)
#define REG_MIPS_BASE              (0x1D00)
#define MIPS_REG(addr)                RIU[(addr<<1)+REG_MIPS_BASE]
#define REG_RNG_OUT                 0x0e

#endif
