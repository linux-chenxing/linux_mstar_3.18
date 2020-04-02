/*
 * ceva_linkdrv_xm6.h
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

#ifndef CEVA_LINKDRV_XM6_H_
#define CEVA_LINKDRV_XM6_H_

#include <linux/cdev.h>
#include "ceva_linkdrv.h"
#include "hal_ceva.h"

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
struct xm6_dev_dma_buf {
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
struct xm6_dev_data {
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
    struct platform_device *plat_dev;     /*!< PCIe device (kernel's)        */
    int pci_gen;                          /*!< PCI generation                */
    int pci_num_lanes;                    /*!< number of PCIe lanes          */
    struct pci_dev *upstream;             /*!< upstream root node            */
    unsigned int msis;                    /*!< number of supported msi       */
    unsigned char hw_irq_id;              /*!< hardware irq number           */
    phys_addr_t hw_addr_xm6;
    phys_addr_t hw_addr_sys;
    phys_addr_t hw_addr_axi2miu0;
    phys_addr_t hw_addr_axi2miu1;
    phys_addr_t hw_addr_axi2miu2;
    phys_addr_t hw_addr_axi2miu3;
    ceva_hal_handle ceva_hal;
    u8 dummy[2048];

    phys_addr_t dsp_code_phys;
    u8         *dsp_code_virt;

    struct xm6_dev_dma_buf dma_buf;      /*!< dma container                 */
};

#endif /* CEVA_LINKDRV_XM6_H_ */
