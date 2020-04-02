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
#ifdef CONFIG_USB_MS_OTG
#include <linux/usb/ms_usb.h>
#endif

#define ENABLE_THIRD_EHC

#if  defined(CONFIG_USB_XHCI_HCD) || defined(CONFIG_USB_XHCI_HCD_MODULE) 
#define ENABLE_XHC
#endif

#define UTMI_BASE_ADDRESS_START         (0xFD000000 + (0x103A80 << 1))  //0xFD207500
#define UTMI_BASE_ADDRESS_END           (0xFD000000 + (0x103AFE << 1))  //0xFD2075FC
#define USB_HOST20_ADDRESS_START        (0xFD000000 + (0x102400 << 1))  //0xFD204800
#define USB_HOST20_ADDRESS_END          (0xFD000000 + (0x1024FE << 1))  //0xFD2049FC

#define SECOND_UTMI_BASE_ADDRESS_START  (0xFD000000 + (0x103A00 << 1))  //0xFD207400
#define SECOND_UTMI_BASE_ADDRESS_END    (0xFD000000 + (0x103A7E << 1))  //0xFD2074FC
#define SECOND_USB_HOST20_ADDRESS_START (0xFD000000 + (0x100D00 << 1))  //0xFD201A00
#define SECOND_USB_HOST20_ADDRESS_END   (0xFD000000 + (0x100DFE << 1))  //0xFD201BFC

#define THIRD_UTMI_BASE_ADDRESS_START   (0xFD000000 + (0x103900 << 1))  //0xFD207200
#define THIRD_UTMI_BASE_ADDRESS_END     (0xFD000000 + (0x10397E << 1))  //0xFD2072FC
#define THIRD_USB_HOST20_ADDRESS_START  (0xFD000000 + (0x113900 << 1))  //0xFD227200
#define THIRD_USB_HOST20_ADDRESS_END    (0xFD000000 + (0x1139FE << 1))  //0xFD2273FC

#define XHCI_ADDRESS_START           0xFD320000
#define XHCI_ADDRESS_END             0xFD327fff
#define U3PHY_ADDRESS_START          0xFD244200
#define U3PHY_ADDRESS_END            0xFD244BFF
/* Clippers UHC irq rotated 2->0, 0->1, 1->2 */
#define EHC_PORT0_IRQ				 E_IRQEXPL_UHC2
#define EHC_PORT1_IRQ				 E_IRQ_UHC
#define EHC_PORT2_IRQ				 E_IRQEXPL_UHC1
static struct resource Mstar_usb_ehci_resources[] = 
{
	[0] = 
{
		.start		= UTMI_BASE_ADDRESS_START,
		.end		= UTMI_BASE_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[1] = 
{
		.start		= USB_HOST20_ADDRESS_START,
		.end		= USB_HOST20_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[2] =
 {
		.start		= EHC_PORT0_IRQ,
		.end		= EHC_PORT0_IRQ,
		.flags		= IORESOURCE_IRQ,
	},
};

//tony add for 2st EHCI
static struct resource Second_Mstar_usb_ehci_resources[] = 
{
	[0] = 
{
		.start		= SECOND_UTMI_BASE_ADDRESS_START,
		.end		= SECOND_UTMI_BASE_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
        },
	[1] = 
{
		.start		= SECOND_USB_HOST20_ADDRESS_START,
		.end		= SECOND_USB_HOST20_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[2] = 
{
		.start		= EHC_PORT1_IRQ,
		.end		= EHC_PORT1_IRQ,
		.flags		= IORESOURCE_IRQ,
	},
};

#ifdef ENABLE_THIRD_EHC
static struct resource Third_Mstar_usb_ehci_resources[] = 
{
	[0] = 
{
		.start		= THIRD_UTMI_BASE_ADDRESS_START,
		.end		= THIRD_UTMI_BASE_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[1] = 
{
		.start		= THIRD_USB_HOST20_ADDRESS_START,
		.end		= THIRD_USB_HOST20_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
     [2] =
    {
        .start      = EHC_PORT2_IRQ,
        .end        = EHC_PORT2_IRQ,
		.flags		= IORESOURCE_IRQ,
	},
};
#endif

/* The dmamask must be set for EHCI to work */
static u64 ehci_dmamask = ~(u32)0;

static struct platform_device Mstar_usb_ehci_device = 
{
	.name           = "Mstar-ehci-1",
	.id             = 0,
	.dev = 
{
		.dma_mask		= &ehci_dmamask,
		.coherent_dma_mask	= 0xffffffff, //tony add for limit DMA range
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
		.coherent_dma_mask	= 0xffffffff,    //tony add for limit DMA range
	},
	.num_resources	= ARRAY_SIZE(Second_Mstar_usb_ehci_resources),
	.resource	= Second_Mstar_usb_ehci_resources,
};
#ifdef ENABLE_THIRD_EHC
static struct platform_device Third_Mstar_usb_ehci_device = 
{
	.name		= "Mstar-ehci-3",
	.id		= 2,
	.dev = 
{
		.dma_mask		= &ehci_dmamask,
		.coherent_dma_mask	= 0xffffffff,    //tony add for limit DMA range
	},
	.num_resources	= ARRAY_SIZE(Third_Mstar_usb_ehci_resources),
	.resource	= Third_Mstar_usb_ehci_resources,
};
#endif

#ifdef ENABLE_XHC
#define XHC_PORT0_IRQ	E_IRQ_UHC30
//-----------------------------------------
//   xHCI platform device
//-----------------------------------------
static struct resource Mstar_usb_xhci_resources[] = {
	[0] = {
		.start		= U3PHY_ADDRESS_START,
		.end		= U3PHY_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[1] = {
		.start		= XHCI_ADDRESS_START,
		.end		= XHCI_ADDRESS_END,
		.flags		= IORESOURCE_MEM,
	},
	[2] = {
		.start		= XHC_PORT0_IRQ,
		.end		= XHC_PORT0_IRQ,
		.flags		= IORESOURCE_IRQ,
	},
};

static u64 xhci_dmamask = 0xffffffff;

static struct platform_device Mstar_usb_xhci_device = {
	.name		= "Mstar-xhci-1",
	.id		= 0,
	.dev = {
		.dma_mask		= &xhci_dmamask,
		.coherent_dma_mask	= 0xffffffff, 
	},
	.num_resources	= ARRAY_SIZE(Mstar_usb_xhci_resources),
	.resource	= Mstar_usb_xhci_resources,

};
//---------------------------------------------------------------
#endif

static struct platform_device *Mstar_platform_devices[] = {
	&Mstar_usb_ehci_device,
	&Second_Mstar_usb_ehci_device,
#ifdef ENABLE_THIRD_EHC
	&Third_Mstar_usb_ehci_device,
#endif
#ifdef ENABLE_XHC
    &Mstar_usb_xhci_device,    
#endif
};

int Mstar_ehc_platform_init(void)
{
	return platform_add_devices(Mstar_platform_devices, ARRAY_SIZE(Mstar_platform_devices));
}
#if defined(CONFIG_USB_MSB250X)
#define INT_MS_OTG	E_IRQ_OTG
#define UDC_ADDRESS_START	(0xFD000000 + (0x151000 << 1))
#define UDC_ADDRESS_END		(0xFD000000 + (0x1511FE << 1))
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
#define OTG_USBC_ADDRESS_START	 (0xFD000000 + (0x113800 << 1))
#define OTG_USBC_ADDRESS_END	 (0xFD000000 + (0x11387E << 1))
#define OTG_MOTG_ADDRESS_START	 (0xFD000000 + (0x151000 << 1))
#define OTG_MOTG_ADDRESS_END	 (0xFD000000 + (0x1511FE << 1))
#define OTG_HOST20_ADDRESS_START	THIRD_USB_HOST20_ADDRESS_START
#define OTG_HOST20_ADDRESS_END		THIRD_USB_HOST20_ADDRESS_END
#define OTG_UTMI_ADDRESS_START	THIRD_UTMI_BASE_ADDRESS_START
#define OTG_UTMI_ADDRESS_END	THIRD_UTMI_BASE_ADDRESS_END
#define USB_IRQ_OTG	E_IRQEXPL_USB2
#define OTG_IRQ_UHC EHC_PORT2_IRQ


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
int ms_set_vbus(unsigned int on)
{
#if 0
	printk("[OTG] ms_set_vbus %d\n", on);

	*((u16 volatile*)(MSTAR_CEDRIC_RIU_BASE+(0x1A13<<1))) = *((u16 volatile*)(MSTAR_CEDRIC_RIU_BASE+(0x1A13<<1))) & (u16)0xfff7;
	*((u16 volatile*)(MSTAR_CEDRIC_RIU_BASE+(0x1A15<<1))) = *((u16 volatile*)(MSTAR_CEDRIC_RIU_BASE+(0x1A15<<1))) & (u16)0xfff7;
	*((u16 volatile*)(MSTAR_CEDRIC_RIU_BASE+(0x2B0C<<1))) = *((u16 volatile*)(MSTAR_CEDRIC_RIU_BASE+(0x2B0C<<1))) & (u16)0xfffd;

	if (on)
		*((u16 volatile*)(MSTAR_CEDRIC_RIU_BASE+(0x2B0C<<1))) = *((u16 volatile*)(MSTAR_CEDRIC_RIU_BASE+(0x2B0C<<1))) | (u16)0x0001;
	else
		*((u16 volatile*)(MSTAR_CEDRIC_RIU_BASE+(0x2B0C<<1))) = *((u16 volatile*)(MSTAR_CEDRIC_RIU_BASE+(0x2B0C<<1))) & (u16)0xfffe;
#endif	
	return on;
}

struct ms_usb_platform_data ms_otg_platform_data =
{
	.mode = 0,
	.set_vbus = ms_set_vbus,
};

int Mstar_otg_platform_init(void)
{
	Mstar_otg_device.dev.platform_data = &ms_otg_platform_data;
	return platform_add_devices(Mstar_otg_devices, ARRAY_SIZE(Mstar_otg_devices));
}
#endif

