///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2014 - 2015 MStar Semiconductor, Inc.
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

/**
 * @file mali_kbase_mem_block_import.h
 */

#ifndef _KBASE_MEM_BLOCK_IMPORT_H_
#define _KBASE_MEM_BLOCK_IMPORT_H_

#include <linux/types.h>

/**
 * @brief Import a memory block as a dma-buf
 *
 * the base address and size of the memory block must be page-aligned.
 *
 * @param[in] base              the base address of the memory block
 * @param[in] size              the size of the memory block
 * @param[in] is_bus_address    is the address a bus address
 *
 * @return file descriptor on success of -errno on failure
 */
int mem_block_import_dma_buf(phys_addr_t base, u32 size, bool is_bus_address);

#endif /* _KBASE_MEM_BLOCK_IMPORT_H_ */
