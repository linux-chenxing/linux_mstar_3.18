//------------------------------------------------------------------------------
//
// Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
//
//------------------------------------------------------------------------------
// FILE
//      ms_otg.h
//
// DESCRIPTION
//
// HISTORY
//
//------------------------------------------------------------------------------
#ifndef _UDC_MSTAR_31023_H
#define _UDC_MSTAR_31023_H

#include <mstar/mstar_chip.h>

#if defined(CONFIG_ARM64)
	extern ptrdiff_t mstar_pm_base;
	#define MSTAR_PM_BASE		(mstar_pm_base)
	#define _MSTAR_USB_BASEADR	(mstar_pm_base + 0x00200000ULL)
#else
	#define MSTAR_PM_BASE	   0xfd000000UL
	#define _MSTAR_USB_BASEADR 0xfd200000UL
#endif


#define MIU0_BUS_BASE_ADDR 	MSTAR_MIU0_BUS_BASE
#define MIU1_BUS_BASE_ADDR	MSTAR_MIU1_BUS_BASE
#if defined(MSTAR_MIU2_BUS_BASE)
#define MIU2_BUS_BASE_ADDR	MSTAR_MIU2_BUS_BASE
#endif


#define MIU0_PHY_BASE_ADDR	((unsigned long)0x00000000)
/* MIU0 2G*/
#define MIU0_SIZE		((unsigned long)0x80000000)

#define MIU1_PHY_BASE_ADDR	((unsigned long)0x80000000)
/* MIU1 1G*/
#define MIU1_SIZE		((unsigned long)0x40000000)



#define BUS2PA(A)	\
	((((A)>=MIU0_BUS_BASE_ADDR)&&((A)<(MIU0_BUS_BASE_ADDR+MIU0_SIZE)))?	\
		((A)-MIU0_BUS_BASE_ADDR+MIU0_PHY_BASE_ADDR):	\
		((((A)>= MIU1_BUS_BASE_ADDR)&&((A)<MIU1_BUS_BASE_ADDR+MIU1_SIZE))?	\
			((A)-MIU1_BUS_BASE_ADDR+MIU1_PHY_BASE_ADDR):	\
			(0xFFFFFFFF)))

#define PA2BUS(A)	\
	((((A)>=MIU0_PHY_BASE_ADDR)&&((A)<(MIU0_PHY_BASE_ADDR+MIU0_SIZE)))?	\
		((A)-MIU0_PHY_BASE_ADDR+MIU0_BUS_BASE_ADDR):	\
		((((A)>= MIU1_PHY_BASE_ADDR)&&((A)<MIU1_PHY_BASE_ADDR+MIU1_SIZE))?	\
			((A)-MIU1_PHY_BASE_ADDR+MIU1_BUS_BASE_ADDR):	\
			(0xFFFFFFFF)))


/******************************************************************************
 * SW patch config
 ******************************************************************************/
//***** for Cedric C3 patch*****//
//#define DRAM_MORE_THAN_1G_PATCH	//Clippers HW switch miu0/miu1 at 1G, if miu0 > 1G, HW will access wrong place
/******************************************************************************
 * HW eco config
 ******************************************************************************/
#define AHB_WDATA_CHANGES_WITHOUT_HREADY_ECO
#define SHORTPKT_STATUS_CLEAR_ECO
#define	MONKEY_TEST_ECO	//avoid device can not enter high speed mode
#define ENABLE_TX_RX_RESET_ECO
#define DMA_RXTX_INT_LAST_DONE_ECO
#define XIU_ACCESS_FAILED_WITH_ECO //(when clk_mcu<120MHz)

//---- change to 55 interface
#define ENABLE_UTMI_55_INTERFACE
/******************************************************************************
 * HW config
 ******************************************************************************/
#define MAX_USB_DMA_CHANNEL  2
#define MAX_EP_NUMBER	4

static const char ep0name [] = "ep0";
static const char *const ep_name[] = {
    ep0name,                                /* everyone has ep0 */
	"ep1in-bulk", "ep2out-bulk", "ep3in-int",
};
#define MSB250X_ENDPOINTS ARRAY_SIZE(ep_name)

//---USB device switch pad to CID enable
#define CID_ENABLE

//------RIU, UTMI, USBC and OTG base address -----------------------------
/*---port2 support device---*/
#if defined(CONFIG_ARM64)
	extern ptrdiff_t mstar_pm_base;
	#define MSTAR_PM_BASE		(mstar_pm_base)
	#define RIU_BASE	(mstar_pm_base + 0x00200000ULL)
#else
	#define RIU_BASE           0xfd200000UL
#endif


#define UTMI_BASE_ADDR     (RIU_BASE+(0x3a80*2))
#define USBC_BASE_ADDR     (RIU_BASE+(0x0700*2))
#define OTG0_BASE_ADDR     (RIU_BASE+(0x11700*2))
#define BC_BASE_ADDR	   	 (RIU_BASE+(0x13700*2))
#endif  /* define _UDC_MSTAR_31023_H */
