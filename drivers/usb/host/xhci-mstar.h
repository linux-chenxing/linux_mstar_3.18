/*
 * xHCI host controller driver
 *
 * Copyright (C) 2013 MStar Inc.
 *
 */

#ifndef _XHCI_MSTAR_H
#define _XHCI_MSTAR_H

#define XHCI_MSTAR_VERSION "20180115"

#include "xhci-mstar-318xx.h"

struct u3phy_addr_base {
	uintptr_t	utmi_base;
	uintptr_t	bc_base;
	uintptr_t	u3top_base;
	uintptr_t	xhci_base;
	uintptr_t	u3dtop_base;
	uintptr_t	u3atop_base;
	uintptr_t	u3dtop1_base; /* XHCI 2 ports */
	uintptr_t	u3atop1_base;
};

#endif	/* _XHCI_MSTAR_H */
