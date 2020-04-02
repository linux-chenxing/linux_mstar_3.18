/*
 * xHCI host controller driver
 *
 * Copyright (C) 2013 MStar Inc.
 *
 */

#ifndef _XHCI_MSTAR_30813_H
#define _XHCI_MSTAR_30813_H

#include <ehci-mstar-30813.h>

// ----- Don't modify it !----------
#if defined(CONFIG_ARM) 
#define XHCI_PA_PATCH   1
#else
#define XHCI_PA_PATCH   0
#endif
#define XHCI_FLUSHPIPE_PATCH  1
//------------------------------

#define XHCI_CHIRP_PATCH  1
#define ENABLE_XHCI_SSC   1
#define XHCI_TX_SWING_PATCH  1

//------ for test -----------------
//#define XHCI_CURRENT_SHARE_PATCH 0   //Only for USB3; will cause USB2 chirp handshake fail. 
#define XHCI_ENABLE_DEQ  0
#define XHCI_ENABLE_TESTBUS  0
//--------------------------------

//Inter packet delay setting for all chips
#define XHCI_IPACKET_DELAY_PATCH

#define XHCI_DISABLE_COMPLIANCE
#define XHCI_DISABLE_TESTMODE
#define XHCI_SSDISABLED_PATCH
#define XHCI_HC_RESET_PATCH

#define MSTAR_LOST_SLOT_PATCH 0
#define XHCI_TX_ERR_EVENT_PATCH

#define XHCI_ENABLE_PPC

//--------  Setting option  -----------

//Enable it after Agate
#define XHCI_ENABLE_240MHZ

#define XHCI_ENABLE_LASTDOWNZ

//--------------------------------


// --------- ECO option ---------
#define XHCI_ENABLE_LOOPBACK_ECO
#define LOOPBACK_ECO_OFFSET		0x20*2
#define LOOPBACK_ECO_BIT		BIT4|BIT5

//--------------------------------


//--------  U3 PHY IP  -----------
#define XHCI_PHY_MT28
#define XHCI_PHY_EFUSE
#define XHCI_2PORTS
#define XHCI_COMPANION
//--------------------------------

#if defined(CONFIG_ARM)
#define _MSTAR_PM_BASE         0xFD000000
#else
#define _MSTAR_PM_BASE         0xBF000000
#endif

#define _MSTAR_U3PHY_P0_ATOP_M0_BASE (_MSTAR_USB_BASEADR+(0x22100*2))
#define _MSTAR_U3PHY_P0_ATOP_M1_BASE (_MSTAR_USB_BASEADR+(0x22200*2))
#define _MSTAR_U3PHY_P0_DTOP_M0_BASE (_MSTAR_USB_BASEADR+(0x11C00*2))
#define _MSTAR_U3PHY_P0_DTOP_M1_BASE (_MSTAR_USB_BASEADR+(0x11D00*2))
#define _MSTAR_U3PHY_P1_ATOP_M0_BASE (_MSTAR_USB_BASEADR+(0x53500*2))
#define _MSTAR_U3PHY_P1_ATOP_M1_BASE (_MSTAR_USB_BASEADR+(0x53600*2))
#define _MSTAR_U3PHY_P1_DTOP_M0_BASE (_MSTAR_USB_BASEADR+(0x53300*2))
#define _MSTAR_U3PHY_P1_DTOP_M1_BASE (_MSTAR_USB_BASEADR+(0x53400*2))

#define _MSTAR_U3UTMI_BASE     0
#define _MSTAR_U3TOP_BASE      (_MSTAR_USB_BASEADR+(0x22500*2))
#define _MSTAR_XHCI_BASE       (_MSTAR_USB_BASEADR+(0x90000*2))
#define _MSTAR_U3BC_BASE       0


#define XHC_HSPORT_OFFSET	0x420
#define XHC_SSPORT_OFFSET	0x430

#if defined(XHCI_PHY_EFUSE)
	#define _MSTAR_EFUSE_BASE	(_MSTAR_PM_BASE+(0x2000*2))
	#define XHC_EFUSE_OFFSET	0x26	//bank5E
	#define XHC_EFUSE_FSM		1		//use FSM1
#endif

#endif	/* _XHCI_MSTAR_30813_H */

