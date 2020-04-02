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
/// @file   ipa_bootargs.h
/// @brief  parse ipa pool boot arguments
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _IPA_BOOTARGS_H
#define _IPA_BOOTARGS_H

#define IPA_NAME_LENG  32
#define IPA_CMA_DEFAULT_NUM 16
#define CMA_MEM_ALIGN       (pageblock_nr_pages*PAGE_SIZE)
/* ipa boot args base struct
  *
  * ipa_id:  [heap_type<<6 || heap_id]
  * start: miu offset
  */
#define IPA_BootArgs_Base int miu;  \
    int ipa_id; \
    unsigned long start;    \
    unsigned long size; \
    char name[IPA_NAME_LENG];

/* only for IPA CMA
  * ipa can access this by ipa cma driver
  *
  * now this is designed for vdec xc mfe dip ...
  * driver also use miu kernel protection
  */
struct IPA_CMA_BootArgs_Config {    
    IPA_BootArgs_Base
};

/* only for pa address
  * ipa can access this by pa driver
  *
  * now this is designed for memory not in kernel(mmap.h)
  */
struct PA_BootArgs_Config {     
    IPA_BootArgs_Base
};

struct IPA_STR_POOL_BootArgs_Config {     
    IPA_BootArgs_Base
};

/* ipa cma array */
extern struct IPA_CMA_BootArgs_Config ipa_cma_config[IPA_CMA_DEFAULT_NUM];
extern struct device ipa_cma_device[IPA_CMA_DEFAULT_NUM];
extern int mstar_driver_boot_ipa_cma_num;

/*pa para array*/
extern struct PA_BootArgs_Config pa_config[MIU_NUM];
extern struct device pa_device[MIU_NUM];
extern int mstar_driver_boot_pa_num;

extern struct IPA_STR_POOL_BootArgs_Config ipa_str_pool_config;
extern int mstar_driver_boot_ipa_str_pool_num ;

#endif

