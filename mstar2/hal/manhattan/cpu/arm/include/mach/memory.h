/*
 *  arch/arm/mach-vexpress/include/mach/memory.h
 *
 *  Copyright (C) 2003 ARM Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * The macros below define a section size of 256MB and a non-linear virtual to
 * physical mapping:
 *
 * 512MB @ 0x40000000 -> PAGE_OFFSET
 * 256MB @ 0xA0000000 -> PAGE_OFFSET + 0x20000000
 */

#ifndef __ASM_ARCH_MEMORY_H
#define __ASM_ARCH_MEMORY_H

#define PHYS_OFFSET		UL(CONFIG_MEMORY_START_ADDRESS)

#ifdef CONFIG_SPARSEMEM

#ifndef __ASSEMBLY__
#ifdef CONFIG_ARM_LPAE
extern phys_addr_t lx_mem2_addr;
extern phys_addr_t lx_mem3_addr;
#else
extern unsigned long lx_mem2_addr;
extern unsigned long lx_mem3_addr;
#endif
extern unsigned long lx_mem_size;
extern unsigned long lx_mem2_size;
extern unsigned long lx_mem3_size;
extern unsigned long miu01_adj;
#endif

#ifdef CONFIG_ARM_LPAE
	#define MAX_PHYSMEM_BITS        36
#else
	#define MAX_PHYSMEM_BITS        32
#endif
#define SECTION_SIZE_BITS       29
#define INVALID_PHY_ADDR        UL(-1)

#ifdef CONFIG_CMA
#define _PMD_SIZE_    (1UL << 21)
#define REM_TO_PMD_SIZE(x) ((x)&(_PMD_SIZE_-1))
#define ALIGN_UPTO_PMD_SIZE(x) (((x)+_PMD_SIZE_-1)&(~(_PMD_SIZE_-1)))
#define VIRT_HOLE(x, y) (ALIGN_UPTO_PMD_SIZE(REM_TO_PMD_SIZE(x) + REM_TO_PMD_SIZE(y)) - ((REM_TO_PMD_SIZE(x) + REM_TO_PMD_SIZE(y))))

/* page offset of LX_MEM */
#define PAGE_OFFSET0    \
        (PHYS_OFFSET==INVALID_PHY_ADDR ? (PAGE_OFFSET) : (PAGE_OFFSET + VIRT_HOLE(PHYS_OFFSET, PAGE_OFFSET)))         

/* page offset of LX_MEM2 */
#define PAGE_OFFSET1    \
        (lx_mem2_addr==INVALID_PHY_ADDR ? (PAGE_OFFSET0 + lx_mem_size) :  \
        (PAGE_OFFSET0 + lx_mem_size + VIRT_HOLE(lx_mem2_addr, PAGE_OFFSET0 + lx_mem_size)))      

/* page offset of LX_MEM3*/
#define PAGE_OFFSET2    \
        (lx_mem3_addr==INVALID_PHY_ADDR ? (PAGE_OFFSET1 + lx_mem2_size) :  \
        (PAGE_OFFSET1 + lx_mem2_size + VIRT_HOLE(lx_mem3_addr, PAGE_OFFSET1 + lx_mem2_size)))

#else
/* page offset of LX_MEM */
#define PAGE_OFFSET0    PAGE_OFFSET

/* page offset of LX_MEM2 */
#define PAGE_OFFSET1    (PAGE_OFFSET + lx_mem_size)

/* page offset of LX_MEM3*/
#define PAGE_OFFSET2    (PAGE_OFFSET1 + lx_mem2_size)
#endif

#define __phys_to_virt(phys)                                            \
        ((phys) >= lx_mem2_addr ? ( (phys) >= lx_mem3_addr ?( lx_mem3_addr > ( lx_mem2_addr + lx_mem2_size)?(phys) - lx_mem3_addr + PAGE_OFFSET2 : (phys) - lx_mem2_addr + PAGE_OFFSET1 ): ((phys) - lx_mem2_addr + PAGE_OFFSET1) ):    \
         (phys) - PHYS_OFFSET + PAGE_OFFSET0)

#define __virt_to_phys(virt)                                            \
         ((virt) >= PAGE_OFFSET1 ?  ( (virt) >= PAGE_OFFSET2? ((virt) - PAGE_OFFSET2 + lx_mem3_addr) : ((virt) - PAGE_OFFSET1 + lx_mem2_addr) ): \
          (virt) - PAGE_OFFSET0 + PHYS_OFFSET )

#endif
#endif
