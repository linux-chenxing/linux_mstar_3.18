/*------------------------------------------------------------------------------
	Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
------------------------------------------------------------------------------*/
#ifndef __ARCH_ARM_ASM_IRQS_H
#define __ARCH_ARM_ASM_IRQS_H
/*-----------------------------------------------------------------------------
    Include Files
------------------------------------------------------------------------------*/
//#include "platform.h"

/*------------------------------------------------------------------------------
    Constant
-------------------------------------------------------------------------------*/

/* Max number of Interrupts */
#ifdef CONFIG_MSTAR_PCIE2
#define ORG_NR_IRQS	384
#define NR_IRQS		(ORG_NR_IRQS+4+12)		/* PCIE legacy INTA INTB INTC INTD, MSI x 12 irq */
#else
#define NR_IRQS		384
#endif
#endif // __ARCH_ARM_ASM_IRQS_H

