/*
 * pci_acl.h
 *
 *  Created on: Nov 28, 2013
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

#ifndef PCI_ACL_H_
#define PCI_ACL_H_

/*
 * device interrupts definitions
 */
#define CEVA_IRQ_DATA_BASE_ADDR                          0x0003FF00

#define CEVA_IRQ_00_DATA_ADDR                            0x0003FF00
#define CEVA_IRQ_01_DATA_ADDR                            0x0003FF08
#define CEVA_IRQ_02_DATA_ADDR                            0x0003FF10
#define CEVA_IRQ_03_DATA_ADDR                            0x0003FF18
#define CEVA_IRQ_04_DATA_ADDR                            0x0003FF20
#define CEVA_IRQ_05_DATA_ADDR                            0x0003FF28
#define CEVA_IRQ_06_DATA_ADDR                            0x0003FF30
#define CEVA_IRQ_07_DATA_ADDR                            0x0003FF38
#define CEVA_IRQ_08_DATA_ADDR                            0x0003FF40
#define CEVA_IRQ_09_DATA_ADDR                            0x0003FF48
#define CEVA_IRQ_10_DATA_ADDR                            0x0003FF50
#define CEVA_IRQ_11_DATA_ADDR                            0x0003FF58
#define CEVA_IRQ_12_DATA_ADDR                            0x0003FF60
#define CEVA_IRQ_13_DATA_ADDR                            0x0003FF68
#define CEVA_IRQ_14_DATA_ADDR                            0x0003FF70
#define CEVA_IRQ_15_DATA_ADDR                            0x0003FF78
// helper macro
#define GET_IRQ_DATA_ADDR(n)       (CEVA_IRQ_DATA_BASE_ADDR+(n<<3))

#define CEVA_IRQ_STATUS_ADDR                             0x0003FFF0

/*
 * address to put host's dma address
 */
#define CEVA_PCIE_CRA_PAGE_SIZE                          0x01000000
#define CAVA_PCIE_CRA_ADDR_BASE                          0x0000020C
#define GET_PCIE_CRA_ADDR(page) (CAVA_PCIE_CRA_ADDR_BASE)

/*
 * host interrupts definitions (for generation)
 */
#define CEVA_HOST_INTERRUPT_FIFO                         0x00040000

#endif /* PCI_ACL_H_ */
