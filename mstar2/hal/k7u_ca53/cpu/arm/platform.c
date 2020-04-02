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
#include <linux/dma-mapping.h>

#include <mach/irqs.h>
#include "chip_int.h"

#if  defined(CONFIG_USB_XHCI_HCD) || defined(CONFIG_USB_XHCI_HCD_MODULE)
#define ENABLE_XHC
#endif
#define usb_dma_limit ((phys_addr_t)~0)

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
		.start		= E_IRQHYPH_USB30_HS_UHC,
		.end		= E_IRQHYPH_USB30_HS_UHC,
		.flags		= IORESOURCE_IRQ,
	},
};

/* The dmamask must be set for EHCI to work */
//static u64 ehci_dmamask = ~(u32)0;
static u64 ehci_dmamask = usb_dma_limit;

static struct platform_device Mstar_usb_ehci_device =
{
	.name           = "Mstar-ehci-1",
	.id             = 0,
	.dev =
	{
		.dma_mask		= &ehci_dmamask,
		.coherent_dma_mask	= usb_dma_limit, //add for limit DMA range
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
		.coherent_dma_mask	= usb_dma_limit,    //add for limit DMA range
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
		.coherent_dma_mask	= usb_dma_limit,    //add for limit DMA range
	},
	.num_resources	= ARRAY_SIZE(Third_Mstar_usb_ehci_resources),
	.resource	= Third_Mstar_usb_ehci_resources,
};

#ifdef ENABLE_XHC
//-----------------------------------------
//   xHCI platform device
//-----------------------------------------
static struct resource Mstar_usb_xhci_resources[] = {
	[2] = {
		.start		= E_IRQHYPH_USB30_SS,
		.end		= E_IRQHYPH_USB30_SS,
		.flags		= IORESOURCE_IRQ,
	},
};

//static u64 xhci_dmamask = 0xffffffff;
static u64 xhci_dmamask = usb_dma_limit;

static struct platform_device Mstar_usb_xhci_device = {
	.name		= "Mstar-xhci-1",
	.id		= 0,
	.dev = {
		.dma_mask		= &xhci_dmamask,
		.coherent_dma_mask	= usb_dma_limit,
	},
	.num_resources	= ARRAY_SIZE(Mstar_usb_xhci_resources),
	.resource	= Mstar_usb_xhci_resources,

};
//---------------------------------------------------------------
#endif

static struct platform_device *Mstar_platform_devices[] = {
	&Mstar_usb_ehci_device,
	&Second_Mstar_usb_ehci_device,
	&Third_Mstar_usb_ehci_device,
#ifdef ENABLE_XHC
	&Mstar_usb_xhci_device,
#endif
};

int Mstar_ehc_platform_init(void)
{
	return platform_add_devices(Mstar_platform_devices, ARRAY_SIZE(Mstar_platform_devices));
}

#if defined(CONFIG_USB_MSB250X)
#define INT_MS_OTG	E_IRQEXPL_OTG
#define UDC_ADDRESS_START	(0xFD000000 + (0x111700 << 1))
#define UDC_ADDRESS_END		(0xFD000000 + (0x1117FE << 1))
static struct resource ms_udc_device_resource[] =
{
	[0] = {
		.start = UDC_ADDRESS_START,
		.end   = UDC_ADDRESS_END,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = INT_MS_OTG,
		.end   = INT_MS_OTG,
		.name  = "udc-int",
		.flags = IORESOURCE_IRQ,
	}
};

struct platform_device ms_udc_device =
{
	.name             = "msb250x_udc",
	.id               = -1,
	.num_resources    = ARRAY_SIZE(ms_udc_device_resource),
	.resource         = ms_udc_device_resource,
};


static struct platform_device *Mstar_udc_devices[] = {
	&ms_udc_device,
};

int Mstar_udc_platform_init(void)
{
	return platform_add_devices(Mstar_udc_devices, ARRAY_SIZE(Mstar_udc_devices));
}

#endif

#if defined(CONFIG_USB_MS_OTG)
#define OTG_USBC_ADDRESS_START	 (0xFD000000 + (0x100700 << 1))
#define OTG_USBC_ADDRESS_END	 (0xFD000000 + (0x10077E << 1))
#define OTG_MOTG_ADDRESS_START	 (0xFD000000 + (0x111700 << 1))
#define OTG_MOTG_ADDRESS_END	 (0xFD000000 + (0x1117FE << 1))
#define OTG_HOST20_ADDRESS_START	(0xFD000000 + (0x102400 << 1))
#define OTG_HOST20_ADDRESS_END		(0xFD000000 + (0x1024FE << 1))
#define OTG_UTMI_ADDRESS_START	(0xFD000000 + (0x103A80 << 1))
#define OTG_UTMI_ADDRESS_END	(0xFD000000 + (0x103B7E << 1))
#define USB_IRQ_OTG	E_IRQ_USB
#define OTG_IRQ_UHC E_IRQ_UHC


static struct resource ms_otg_device_resource[] =
{
	[0] = {
		.start = OTG_USBC_ADDRESS_START,
		.end   = OTG_USBC_ADDRESS_END,
		.name  = "usbc-base",
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = OTG_HOST20_ADDRESS_START,
		.end   = OTG_HOST20_ADDRESS_END,
		.name  = "uhc-base",
		.flags = IORESOURCE_MEM,
	},
	[2] = {
		.start = OTG_MOTG_ADDRESS_START,
		.end   = OTG_MOTG_ADDRESS_END,
		.name  = "motg-base",
		.flags = IORESOURCE_MEM,
	},
	[3] = {
		.start = USB_IRQ_OTG,
		.end   = USB_IRQ_OTG,
		.name  = "usb-int",
		.flags = IORESOURCE_IRQ,
	},
	[4] = {
		.start = OTG_UTMI_ADDRESS_START,
		.end   = OTG_UTMI_ADDRESS_END,
		.name  = "utmi-base",
		.flags = IORESOURCE_MEM,
	},
	[5] = {
		.start = OTG_IRQ_UHC,
		.end   = OTG_IRQ_UHC,
		.name  = "uhc-int",
		.flags = IORESOURCE_IRQ,
	}
};

struct platform_device Mstar_otg_device =
{
	.name             = "mstar-otg",
	.id               = -1,
	.num_resources    = ARRAY_SIZE(ms_otg_device_resource),
	.resource         = ms_otg_device_resource,
};

static struct platform_device *Mstar_otg_devices[] = {
	&Mstar_otg_device,
};

int Mstar_otg_platform_init(void)
{
	return platform_add_devices(Mstar_otg_devices, ARRAY_SIZE(Mstar_otg_devices));
}
#endif
