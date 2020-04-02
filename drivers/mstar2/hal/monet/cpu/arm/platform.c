///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2012 MStar Semiconductor, Inc.
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

//#include <linux/config.h>
#include <generated/autoconf.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/resource.h>
#include <linux/platform_device.h>

#include <mach/irqs.h>
#include "chip_int.h"

static struct resource Mstar_usb_ehci_resources[] = 
{
	[2] = 
	{
		.start		= E_IRQ_UHC,
		.end		= E_IRQ_UHC,
		.flags		= IORESOURCE_IRQ,
	},
};

//add for 2st EHCI
static struct resource Second_Mstar_usb_ehci_resources[] = 
{
	[2] = 
	{
		.start		= E_IRQEXPL_UHC1,
		.end		= E_IRQEXPL_UHC1,
		.flags		= IORESOURCE_IRQ,
	},
};

static struct resource Third_Mstar_usb_ehci_resources[] = 
{
	[2] = 
	{
		.start		= E_IRQEXPL_UHC2P2,
		.end		= E_IRQEXPL_UHC2P2,
		.flags		= IORESOURCE_IRQ,
	},
};

/* The dmamask must be set for EHCI to work */
static u64 ehci_dmamask = ~(u32)0;

static struct platform_device Mstar_usb_ehci_device = 
{
	.name           = "Mstar-ehci-1",
	.id             = 0,
	.dev = 
	{
		.dma_mask		= &ehci_dmamask,
		.coherent_dma_mask	= 0xffffffff, //add for limit DMA range
	},
	.num_resources	= ARRAY_SIZE(Mstar_usb_ehci_resources),
	.resource	= Mstar_usb_ehci_resources,
};

//tony add for 2st EHCI
static struct platform_device Second_Mstar_usb_ehci_device = 
{
	.name		= "Mstar-ehci-2",
	.id		= 1,
	.dev = 
	{
		.dma_mask		= &ehci_dmamask,
		.coherent_dma_mask	= 0xffffffff,    //add for limit DMA range
	},
	.num_resources	= ARRAY_SIZE(Second_Mstar_usb_ehci_resources),
	.resource	= Second_Mstar_usb_ehci_resources,
};

static struct platform_device Third_Mstar_usb_ehci_device = 
{
	.name		= "Mstar-ehci-3",
	.id		= 2,
	.dev = 
	{
		.dma_mask		= &ehci_dmamask,
		.coherent_dma_mask	= 0xffffffff,    //add for limit DMA range
	},
	.num_resources	= ARRAY_SIZE(Third_Mstar_usb_ehci_resources),
	.resource	= Third_Mstar_usb_ehci_resources,
};

static struct platform_device *Mstar_platform_devices[] = {
	&Mstar_usb_ehci_device,
	&Second_Mstar_usb_ehci_device,
	&Third_Mstar_usb_ehci_device,
};

int Mstar_ehc_platform_init(void)
{
	return platform_add_devices(Mstar_platform_devices, ARRAY_SIZE(Mstar_platform_devices));
}
