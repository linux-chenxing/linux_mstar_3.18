/*
 * ceva_linkdrv_pci.h
 *
 *  Created on: Aug 14, 2013
 *  Author: Ido Reis <ido.reis@tandemg.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef CEVA_LINKDRV_PCI_H_
#define CEVA_LINKDRV_PCI_H_

#include <linux/cdev.h>
#include "ceva_linkdrv.h"

/*
 * driver name
 */
#define DRIVER_NAME                                             "ceva_linkdrv"

/*
 * By default, the numbering of the minor devices starts at 0
 */
#define CEVA_PCIDEV_DEFAULT_MINOR_START                                      0

/*
 * This ensures no warnings in IDE for unresolved symbol.
 * Actually, this macro is injected by the Makefile
 */
#ifndef CEVA_PCIDEV_NUMBER_OF_DEVICES
#define CEVA_PCIDEV_NUMBER_OF_DEVICES                                        1
#endif

#define PCI_DEVICE_ID_CEVA 0x7011
#define PCI_VENDOR_ID_CEVA 0x10EE

/*
 * number of PCIe bars
 */
#define ACL_PCI_NUM_BARS                                                     4

/*!
 * dma mapped buffer container
 *
 * we used this structure as a container for the mapped dma space
 * its used by the pci driver as container class
 */
struct ceva_pcidev_dma_buf {
	void *cpu_addr;                       /*!< virtual address              */
	size_t size;                          /*!< size of mapped space         */
	dma_addr_t dma_addr;                  /*!< physical address             */
	void *priv_data;                      /*!< owner driver address         */
};

/*!
 * PCIe device driver private structure
 *
 * this class is public to allow access for the logical driver module
 * otherwise, it should be within the c file
 */
struct ceva_pcidev {
	/*
	 * general driver's variables
	 */
	unsigned int count;                   /*!< number of open calls         */
	struct mutex dev_mutex;               /*!< internal driver mutex        */
	struct ceva_linkdrv linkdrv;          /*!< logical ceva driver module   */
	struct cdev cdev;                     /*!< char device handler          */

	/*
	 * PCIe related variables
	 */
	struct pci_dev *pci_dev;              /*!< PCIe device (kernel's)        */
	int pci_gen;                          /*!< PCI generation                */
	int pci_num_lanes;                    /*!< number of PCIe lanes          */
	struct pci_dev *upstream;             /*!< upstream root node            */
	unsigned int msis;                    /*!< number of supported msi       */
	unsigned char hw_irq_id;              /*!< hardware irq number           */
	unsigned char hw_revision;            /*!< PCIe revision                 */
	void * __iomem bar[ACL_PCI_NUM_BARS]; /*!< mem/io bars                   */
	struct ceva_pcidev_dma_buf dma_buf;   /*!< dma container                 */
};

#endif /* CEVA_LINKDRV_PCI_H_ */
