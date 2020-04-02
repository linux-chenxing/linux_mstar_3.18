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

//Enable it after Edison
#define XHCI_ENABLE_LASTDOWNZ

//Enable it for Kaiser
#define XHCI_ENABLE_DPDM_SWAP
//--------------------------------


// --------- ECO option ---------
#define XHCI_ENABLE_LOOPBACK_ECO
#define LOOPBACK_ECO_OFFSET		0xFF*2-1
#define LOOPBACK_ECO_BIT		BIT5|BIT6

//--------------------------------


//--------  U3 PHY IP  -----------

//--------------------------------

#if defined(CONFIG_ARM)
#define _MSTAR_PM_BASE         0xFD000000
#else
#define _MSTAR_PM_BASE         0xBF000000
#endif

#define _MSTAR_U3PHY_DTOP_BASE (_MSTAR_USB_BASEADR+(0x22C00*2))
#define _MSTAR_U3PHY_ATOP_BASE (_MSTAR_USB_BASEADR+(0x22D00*2))
#define _MSTAR_U3UTMI_BASE     (_MSTAR_USB_BASEADR+(0x22B00*2))
#define _MSTAR_U3TOP_BASE      (_MSTAR_USB_BASEADR+(0x22000*2))
#define _MSTAR_XHCI_BASE       (_MSTAR_USB_BASEADR+(0x90000*2))
#define _MSTAR_U3BC_BASE       (_MSTAR_USB_BASEADR+(0x22FC0*2))


#define XHC_HSPORT_OFFSET	0x420
#define XHC_SSPORT_OFFSET	0x430

#endif	/* _XHCI_MSTAR_30813_H */

