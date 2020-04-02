///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2014 MStar Semiconductor, Inc.
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

#ifndef _MSTAR_PCIE_TYPE1CS_HEADER
#define _MSTAR_PCIE_TYPE1CS_HEADER

/*
 * ============================================================================
 * INCLUDE HEADERS
 * ============================================================================
 */
#include "mstar_pcie_addrmap.h"

/*
 * ============================================================================
 * LOCAL VARIABLES DEFINITIONS
 * ============================================================================
 */

/*
 * ============================================================================
 * CONSTANT DEFINITIONS
 * ============================================================================
 */
 
/****  PCIE TYPE1 CONFIGURATION SPACE REGISTERS  ****/ 
#define REG_PCIE_VENDOR_DEVICE_ID      (0x0000*4 + (REG_TYPE1_CS_BASE))		/* 16 BITS */
    #define REG_PCIE_VENDORID_MASK                0x0000FFFFU 
    #define REG_PCIE_DEVICEID_MASK                0xFFFF0000U
#define REG_PCIE_COMMAND_STATUS        (0x0001*4 + (REG_TYPE1_CS_BASE))		/* 16 BITS */
    #define REG_PCIE_COMMAND_MASK                 0x0000FFFFU 
    #define REG_PCIE_STATUS_MASK                  0xFFFF0000U
#define REG_PCIE_REVISIONID_CLASSCODE  (0x0002*4 + (REG_TYPE1_CS_BASE))
    #define REG_PCIE_REVISIONID_MASK              0x000000FFU 
    #define REG_PCIE_CLASSCODE_MASK               0xFFFFFF00U
#define REG_CLS_PLT_HEADERTYPE_BIST    (0x0003*4 + (REG_TYPE1_CS_BASE))
    #define REG_CACHE_LINE_SIZE_MASK              0x000000FFU 
    #define REG_PRIMARY_LATENCY_TIMER_MASK        0x0000FF00U
    #define REG_HEADER_TYPE_MASK                  0x00FF0000U 
    #define REG_BIST_MASK                         0xFF000000U
#define REG_PCIE_BAR0                  (0x0004*4 + (REG_TYPE1_CS_BASE))	
#define REG_PCIE_BAR1                  (0x0005*4 + (REG_TYPE1_CS_BASE))	
#define REG_PBN_SBN_SBN_SLT            (0x0006*4 + (REG_TYPE1_CS_BASE))
    #define REG_PRIMARY_BUS_NUMBER_MASK           0x000000FFU 
    #define REG_SECONDARY_BUS_NUMBER_MASK         0x0000FF00U
    #define REG_SUBORDINATE_BUS_NUMBER_MASK       0x00FF0000U 
    #define REG_SECONDARY_LATENCY_TIMER_MASK      0xFF000000U
#define REG_IOBASE_IOLIMIT_2NDSTATUS   (0x0007*4 + (REG_TYPE1_CS_BASE))
    #define REG_IO_BASE_MASK                      0x000000FFU 
    #define REG_IO_LIMIT_MASK                     0x0000FF00U
    #define REG_SECONDARY_STATUS_MASK             0xFFFF0000U 
#define REG_MEMBASE_MEMLIMIT           (0x0008*4 + (REG_TYPE1_CS_BASE))   
    #define REG_MEMORY_BASE_MASK                  0x0000FFFFU 
    #define REG_MEMORY_LIMIT_MASK                 0xFFFF0000U
#define REG_PREMEMBASE_PREMEMLIMIT     (0x0009*4 + (REG_TYPE1_CS_BASE))   
    #define REG_PREFETCH_MEMORY_BASE_MASK         0x0000FFFFU 
    #define REG_PREFETCH_MEMORY_LIMIT_MASK        0xFFFF0000U
#define REG_PREFETCHABLE_BASE_UPPER    (0x000A*4 + (REG_TYPE1_CS_BASE))	
#define REG_PREFETCHABLE_LIMIT_UPPER   (0x000B*4 + (REG_TYPE1_CS_BASE))	
#define REG_IOBASEUPPER_IOLIMITUPPER   (0x000C*4 + (REG_TYPE1_CS_BASE))	
    #define REG_IO_BASE_UPPER_MASK                0x0000FFFFU 
    #define REG_IO_LIMIT_UPPER_MASK               0xFFFF0000U
#define REG_PCIE_CAPABILITY_POINT      (0x000D*4 + (REG_TYPE1_CS_BASE))	
    #define REG_CAPABILITY_POINT_MASK             0x000000FFU 
#define REG_EXPANSION_ROM_BASE_ADDRESS (0x000E*4 + (REG_TYPE1_CS_BASE))	
#define REG_INTERRUPT_BRIDGECONTROL    (0x000F*4 + (REG_TYPE1_CS_BASE))	
    #define REG_INTERRUPT_LINE_MASK               0x000000FFU 
    #define REG_INTERRUPT_PIN_MASK                0x0000FF00U
    #define REG_BRIDGE_CONTROL_MASK               0xFFFF0000U 
		
#endif