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

#ifndef _REG_PCMCIA_H_
#define _REG_PCMCIA_H_
#include "chip_setup.h"
//-----------------------------------------------------------------------------
//  Hardware Capability
//-----------------------------------------------------------------------------
// Base address should be initial.
#define PCMCIA_RIU_MAP RIU_VIRT_BASE  //obtain in init

//-----------------------------------------------------------------------------
//  Macro and Define
//-----------------------------------------------------------------------------
//hardware spec
#define REG_PCMCIA_PCM_MEM_IO_CMD           0x00
#define REG_PCMCIA_ADDR0                    0x02
#define REG_PCMCIA_ADDR1                    0x03
#define REG_PCMCIA_WRITE_DATA               0x04
#define REG_PCMCIA_FIRE_READ_DATA_CLEAR     0x06
#define REG_PCMCIA_READ_DATA                0x08
#define REG_PCMCIA_READ_DATA_DONE_BUS_IDLE  0x09
#define REG_PCMCIA_INT_MASK_CLEAR           0x0A
#define REG_PCMCIA_INT_MASK_CLEAR1          0x0B
#define REG_PCMCIA_STAT_INT_RAW_INT         0x0E
#define REG_PCMCIA_STAT_INT_RAW_INT1        0x0F
#define REG_PCMCIA_MODULE_VCC_OOB           0x10

#define PCMCIA_ATTRIBMEMORY_READ            0x03
#define PCMCIA_ATTRIBMEMORY_WRITE           0x04
#define PCMCIA_IO_READ                      0x05
#define PCMCIA_IO_WRITE                     0x06

#define REG_PCMCIA_BASE_ADR                 0x103440
#define PCMCIA_BASE_ADDRESS	                ( PCMCIA_RIU_MAP + ( REG_PCMCIA_BASE_ADR * 2 ) )

#if defined ( MCU_AEON )
#define PCMCIA_RIU_DEFAULT_BASE_ADDR        0xA0000000;
#elif defined ( MCU_MIPS )
#define PCMCIA_RIU_DEFAULT_BASE_ADDR        0xBF200000;
#endif

#define PCM_OOB_BIT_MASK                    0x03
#define PCM_OOB_BIT_SHFT                    6

#define PCM_OOB_CYCLE_EXTEND                0x3
                                            // 00:th(CE)=4T (extend 3 active cycle)
                                            // 01:th(CE)=3T (extend 2 active cycle) 
                                            // 10:th(CE)=2T (extend 1 active cycle)
                                            // 11:th(CE)=1T 

//-----------------------------------------------------------------------------
//  Type and Structure
//-----------------------------------------------------------------------------

#endif // _REG_PCMCIA_H_
