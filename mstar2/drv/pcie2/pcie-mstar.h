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


#ifndef _PCIE_MSTAR_H
#define _PCIE_MSTAR_H

#define PCIE_MSTAR_VERSION "20170418"

#define PCIE_DEBUG		0
/*#define PCIE_DEBUG_IRQ*/				/* turn on this will slow down interrupt response, on your own risk */
/*#define PCIE_DEBUG_RC_TOP */			/* printk RC top register setting */


#define PATCH_BAR					/* the BAR address reflects pcie2axi window setting, RC will not fit... needs to patch */
#define PATCH_SINGLE_DEVFN			/* pci kernel will scan all dev fn combination, this rc will response to some dev fn setting, needs to fix it to single dev fn to prevent kernel confuse with the other random setting */
/*#define PATCH_WED_XIU_BUS_COLLISION*/ /* no xiu access within interrupt handler */
#define LEGACY_INDIRECT_MMIO_MODE 	/* legacy support of K7 outbound indirect mmio mode (MT7615 only) */
#define ENABLE_L3_CACHE

/* these 2 options exclude to each other!!! */
#define PATCH_RC_BUSNUM_00			/* RC bus number always 00 */
/*#define PATCH_RC_BUSNUM_POWER_00 */	/* RC number keep 0 before assigned(type1 cfg 18h) */

/*#define PCIE_TEST_MMIO_REGISTERS*/
#define PCIE_ENABLE_5G_LINK
/*#define PCIE_DUMP_REGISTERS */

/*#define ECO01_ON*/

#include <pcie-mstar-3.10.40.h>

#ifdef LEGACY_INDIRECT_MMIO_MODE
#define INDIRECT_FLAG		0x80000000
#endif

#endif	/* _PCIE_MSTAR_H */
