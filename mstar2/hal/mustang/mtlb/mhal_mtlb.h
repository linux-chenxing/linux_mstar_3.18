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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   Mhal_mtlb.h
/// @author MStar Semiconductor Inc.
/// @brief  MTLB Driver Interface
///////////////////////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// Linux Mhal_mtlb.h define start
// -----------------------------------------------------------------------------
#ifndef __DRV_MTLB__
#define __DRV_MTLB__

#include "mdrv_mtlb.h"

//-------------------------------------------------------------------------------------------------
//  Define Enable or Compiler Switches
//-------------------------------------------------------------------------------------------------
#define MTLB_ERR(fmt, args...)   printk(KERN_ERR "mtlb hal error %s:%d" fmt,__FUNCTION__,__LINE__,## args)

//--------------------------------------------------------------------------------------------------
//  Constant definition
//--------------------------------------------------------------------------------------------------
#define TLB_Entry_BYTE_SIZE             4           /* tlb entry size = 4 bytes */
#define TLB_MAP_PAGE_SIZE               0x1000      /* tlb map per page size = 0x1000 bytes */

#define TLB_TAG_BIT_SHIFT               26
#define TLB_MIU_BIT_SHIFT               19

#define MTLB_RIU_REG_BASE                   0xFD000000
#define MTLB_RIU_BANK                       0x1628

/*8 bits address*/
#define REG_TLB_MODE                        0x00
    #define GE_TLB_FLUSH                        0x0008
#define REG_TLB_ENABLE                      0x02
    #define GE_TLB_ENABLE                        0x0001
#define REG_TLB_TAG                         0x20

#define TLB_GE_TAG_VALUE                          0x28         // (MTLB) GE Tag Value


void MHal_MTLB_Get_HWInfo(mtlb_hardware_info * hwinfo);
int MHal_MTLB_Get_HWStatus(mtlb_hardware_status * hwstatus);
int MHal_MTLB_TableSize(void);
int MHal_MTLB_Init(void);
int MHal_MTLB_Reserve_Memory(void);
int MHal_MTLB_Get_ZeroPage(unsigned long * u32miu0pa, unsigned long * u32miu1pa, unsigned long * u32miu2pa);
int MHal_MTLB_Enable(mtlb_tlbclient_enable * tlb_client_enable);
void MHal_MTLB_Mapping_Start(unsigned long *flag);
void MHal_MTLB_Mapping_End(unsigned long * flag, u8 u8miu, unsigned long start, unsigned long size);
int MHal_MTLB_Mapping(u8 u8miu, u32 u32va, u32 u32pa, bool paInc, u32 u32size);
int MHal_MTLB_Mapping_Address_Support(unsigned long * u32pa, unsigned long nents, unsigned char * miu);
int MHal_MTLB_Dump(u32 *u32va, u32 *u32pa);
#endif
// -----------------------------------------------------------------------------
// Linux Mhal_mtlb.h End
// -----------------------------------------------------------------------------


