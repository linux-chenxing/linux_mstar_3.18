/*

 * Copyright (c) 2010 - 2014 MStar Semiconductor, Inc.
 This program is free software.
 You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
 either version 2 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program;
 if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/


#ifndef _PCIE_MSTAR_31040_H
#define _PCIE_MSTAR_31040_H

#include "chip_int.h"

#if defined(CONFIG_ARM) || defined(CONFIG_ARM64)
	#include <mstar/mstar_chip.h>

	#if defined(CONFIG_ARM64)
		extern ptrdiff_t mstar_pm_base;
		#define MSTAR_PM_BASE		(mstar_pm_base)
	#else
		#define MSTAR_PM_BASE      0xfd000000UL
	#endif

	#define MSTAR_PCIE_REG_BASE		MSTAR_PM_BASE

#else
#error chip setting
#endif

#define ENABLE_PCIE_PORT0			/* PORT0 is always on*/
/*#define ENABLE_PCIE_PORT1*/		/* PORT1 can be disabled if no hw exist. */

#define MIU0_BUS_BASE_ADDR	MSTAR_MIU0_BUS_BASE
#define MIU1_BUS_BASE_ADDR	MSTAR_MIU1_BUS_BASE
#define MIU2_BUS_BASE_ADDR	0xC0000000ULL

#define MIU0_BUS_LENGTH		(1024*1024*1024)		/* assume 1G length */
#define MIU1_BUS_LENGTH		(1024*1024*1024)		/* assume 1G length */
#define MIU2_BUS_LENGTH		(1024*1024*1024)		/* assume 1G length */

#ifdef ENABLE_PCIE_PORT0
/* PCIE port 0 */
#define MSTAR_PCIE_PORT0_MAC0		(0x1F0000*2)
#define MSTAR_PCIE_PORT0_BASE		(MSTAR_PCIE_REG_BASE + MSTAR_PCIE_PORT0_MAC0)
/* RC TOP */
#define MSTAR_PCIE_PORT0_RC0		(0x140700*2)
#define MSTAR_PCIE_PORT0_RIU0_BASE	(MSTAR_PCIE_REG_BASE + MSTAR_PCIE_PORT0_RC0)

#define MSTAR_PCIE_PORT0_IRQ		E_IRQHYPH_PCIE_RC

#define MSTAR_PCIE_PORT0_NAME		"Mstar-pcie-0"			/* mstar_pcie_port.name */
#define MSTAR_PCIE_PORT0_LANE		1						/* mstar_pcie_port.lane */

#define MSTAR_PCIE_PORT0_INTA		ORG_NR_IRQS
#define MSTAR_PCIE_PORT0_INTB		(MSTAR_PCIE_PORT0_INTA + 1)
#define MSTAR_PCIE_PORT0_INTC		(MSTAR_PCIE_PORT0_INTA + 2)
#define MSTAR_PCIE_PORT0_INTD		(MSTAR_PCIE_PORT0_INTA + 3)

#define MSTAR_PCIE_PORT0_IOMEM_BASE		0xE0000000UL
#define MSTAR_PCIE_PORT0_IOMEM_LEN		0x08000000UL		/* 128M */

#define MSTAR_PCIE_MSI_BASE         (MSTAR_PCIE_PORT0_INTA+8)
#endif


/* PCIE port 1 */
#ifdef ENABLE_PCIE_PORT1

#define MSTAR_PCIE_PORT1_MAC1		(0x1D0000*2)
#define MSTAR_PCIE_PORT1_BASE		(MSTAR_PCIE_REG_BASE + MSTAR_PCIE_PORT1_MAC1)
/* RC TOP */
#define MSTAR_PCIE_PORT1_RC1		(0x140800*2)
#define MSTAR_PCIE_PORT1_RIU1_BASE	(MSTAR_PCIE_REG_BASE + MSTAR_PCIE_PORT1_RC1)

#define MSTAR_PCIE_PORT1_IRQ		E_IRQHYPH_PCIE_RC2

#define MSTAR_PCIE_PORT1_NAME		"Mstar-pcie-1"			/* mstar_pcie_port.name */
#define MSTAR_PCIE_PORT1_LANE		1						/* mstar_pcie_port.lane */

#define MSTAR_PCIE_PORT1_INTA		(MSTAR_PCIE_PORT0_INTD + 1)
#define MSTAR_PCIE_PORT1_INTB		(MSTAR_PCIE_PORT0_INTD + 2)
#define MSTAR_PCIE_PORT1_INTC		(MSTAR_PCIE_PORT0_INTD + 3)
#define MSTAR_PCIE_PORT1_INTD		(MSTAR_PCIE_PORT0_INTD + 4)

#define MSTAR_PCIE_PORT1_IOMEM_BASE		0xE8000000UL
#define MSTAR_PCIE_PORT1_IOMEM_LEN		0x08000000UL		/* 128M */
#endif

#ifdef PATCH_WED_XIU_BUS_COLLISION
void pcie_set_no_xiu_access(u32 PortIdx, u32 val);
#endif

#endif	/* _PCIE_MSTAR_H */
