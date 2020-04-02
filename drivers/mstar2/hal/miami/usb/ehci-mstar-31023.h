/*
 * eHCI host controller driver
 *
 * Copyright (C) 2012 MStar Inc.
 *
 *            Warning
 * This file is only for Mstar ARM
 * core chips later than Amber3.
 * ex. Amber3, Eagle, Agate, Edison,
 *     Eiffel
 *     Amethyst (A7p) 2012/12/21
 *     Kaiser (K3) 2013/01/08
 *     Kaiserin (K2) 2013/05/15
 *     Einstein 2013/6/14
 *     Kaisers (K3S)
 *     Kenya 2013/08/19
 *     Keltic 2013/12/27
 *     Keres 2014/04/25
 *     Monaco 2014/05/22
 *
 * Date: Aug 2012
 */

#ifndef _EHCI_MSTAR_31023_H
#define _EHCI_MSTAR_31023_H

//------ General constant definition ---------------------------------
#define DISABLE	0
#define ENABLE	1

#define BIT0	0x01
#define BIT1	0x02
#define BIT2	0x04
#define BIT3	0x08
#define BIT4	0x10
#define BIT5	0x20
#define BIT6	0x40
#define BIT7	0x80
//--------------------------------------------------------------------

//------ Additional port enable (default: 2 ports) -------------------
#define ENABLE_THIRD_EHC
#define ENABLE_FOURTH_EHC
#define ENABLE_FIFTH_EHC

//--------------------------------------------------------------------


//------ Battery charger -------------------- -------------------
#if defined(CONFIG_MSTAR_EDISON)
//#define ENABLE_BATTERY_CHARGE
#endif

#if defined(CONFIG_MSTAR_EDISON)
//#define ENABLE_APPLE_CHARGER_SUPPORT
#endif

#if defined(CONFIG_MSTAR_AGATE)   || \
	defined(CONFIG_MSTAR_EDISON)  || \
	defined(CONFIG_MSTAR_EMERALD) || \
	defined(CONFIG_MSTAR_AMETHYST)|| \
	defined(CONFIG_MSTAR_EAGLE) || \
	defined(CONFIG_MSTAR_KAISERIN) || \
	defined(CONFIG_MSTAR_KRONUS)
	/* list for ICs before Edison
	 * Disable default setting.
	 */
	#define USB_NO_BC_FUNCTION
#endif

#if defined(CONFIG_MSTAR_EDISON)
#define USB_BATTERY_CHARGE_SETTING_1
#endif
//--------------------------------------------------------------------


//------ RIU base addr and bus-addr to phy-addr translation ----------
#if defined(CONFIG_ARM)
	#include <mstar/mstar_chip.h>

	#define _MSTAR_USB_BASEADR 0xfd200000

	#define MIU0_BUS_BASE_ADDR 	MSTAR_MIU0_BUS_BASE
	#define MIU1_BUS_BASE_ADDR	MSTAR_MIU1_BUS_BASE
#if defined(MSTAR_MIU2_BUS_BASE)
	#define MIU2_BUS_BASE_ADDR	MSTAR_MIU2_BUS_BASE
#endif

	#if defined(CONFIG_MSTAR_EAGLE)
		#define MIU0_PHY_BASE_ADDR	((unsigned long)0x00000000)
		/* MIU0 512M*/
		#define MIU0_SIZE		((unsigned long)0x20000000)

		#define MIU1_PHY_BASE_ADDR	((unsigned long)0x20000000)
		/* MIU1 512M*/
		#define MIU1_SIZE		((unsigned long)0x20000000)
	#elif defined(CONFIG_MSTAR_AMBER3) || \
		defined(CONFIG_MSTAR_AGATE) || \
		defined(CONFIG_MSTAR_EDISON)
		#define MIU0_PHY_BASE_ADDR	((unsigned long)0x00000000)
		/* MIU0 1.5G*/
		#define MIU0_SIZE		((unsigned long)0x60000000)

		#define MIU1_PHY_BASE_ADDR	((unsigned long)0x80000000)
		/* MIU1 1G*/
		#define MIU1_SIZE		((unsigned long)0x40000000)
	#elif defined(CONFIG_MSTAR_NIKE) || \
		defined(CONFIG_MSTAR_MADISON)
		#define MIU0_PHY_BASE_ADDR	((unsigned long)0x00000000)
		/* MIU0 1G*/
		#define MIU0_SIZE		((unsigned long)0x40000000)

		#define MIU1_PHY_BASE_ADDR	((unsigned long)0x40000000)
		/* MIU1 1G*/
		#define MIU1_SIZE		((unsigned long)0x40000000)
	#elif defined(CONFIG_MSTAR_MONACO)
		#define MIU0_PHY_BASE_ADDR	((unsigned long)0x00000000)
		/* MIU0 2G*/
		#define MIU0_SIZE		((unsigned long)0x80000000)

		#define MIU1_PHY_BASE_ADDR	((unsigned long)0x80000000)
		/* MIU1 1G*/
		#define MIU1_SIZE		((unsigned long)0x40000000)

		#define MIU2_PHY_BASE_ADDR	((unsigned long)0xC0000000)
		/* MIU2 512M*/
		#define MIU2_SIZE		((unsigned long)0x20000000)

		#define ENABLE_USB_NEW_MIU_SLE	1
		#define USB_MIU_SEL0	0x70	
		#define USB_MIU_SEL1	0xB8
		#define USB_MIU_SEL2	0xDC
		#define USB_MIU_SEL3	0xFF
	#else
		#define MIU0_PHY_BASE_ADDR	((unsigned long)0x00000000)
		/* MIU0 2G*/
		#define MIU0_SIZE		((unsigned long)0x80000000)

		#define MIU1_PHY_BASE_ADDR	((unsigned long)0x80000000)
		/* MIU1 1G*/
		#define MIU1_SIZE		((unsigned long)0x40000000)
	#endif

	#if defined(CONFIG_MSTAR_MONACO)
		#define MIU_FAIL_ADDR 0xFFFFFFFF

		#define IN_MIU_BUS(A, NUM)	\
			(((A)>=MIU##NUM##_BUS_BASE_ADDR)&&((A)<=(MIU##NUM##_BUS_BASE_ADDR+MIU##NUM##_SIZE-1)))

		#define IN_MIU_PA(A, NUM)	\
			(((A)>=MIU##NUM##_PHY_BASE_ADDR)&&((A)<=(MIU##NUM##_PHY_BASE_ADDR+MIU##NUM##_SIZE-1)))

		#define PA_MIU(A, NUM)	\
			((A)-MIU##NUM##_BUS_BASE_ADDR+MIU##NUM##_PHY_BASE_ADDR)

		#define BUS_MIU(A, NUM)	\
			((A)-MIU##NUM##_PHY_BASE_ADDR+MIU##NUM##_BUS_BASE_ADDR)

		#if defined(MIU2_BUS_BASE_ADDR)
			#define BUS2PA(A)	\
				(IN_MIU_BUS(A, 0) ? PA_MIU(A, 0) : (IN_MIU_BUS(A, 1)? PA_MIU(A, 1): (IN_MIU_BUS(A, 2)? PA_MIU(A, 2) : MIU_FAIL_ADDR)))

			#define PA2BUS(A)	\
				(IN_MIU_PA(A, 0) ? BUS_MIU(A, 0) : (IN_MIU_PA(A, 1)? BUS_MIU(A, 1): (IN_MIU_PA(A, 2)? BUS_MIU(A, 2) : MIU_FAIL_ADDR)))
		#else
			#define BUS2PA(A)	\
				(IN_MIU_BUS(A, 0) ? PA_MIU(A, 0) : (IN_MIU_BUS(A, 1)? PA_MIU(A, 1): MIU_FAIL_ADDR))

			#define PA2BUS(A)	\
				(IN_MIU_PA(A, 0) ? BUS_MIU(A, 0) : (IN_MIU_PA(A, 1)? BUS_MIU(A, 1): MIU_FAIL_ADDR))
		#endif
	#else
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
	#endif
#elif defined(CONFIG_MIPS)
	#define _MSTAR_USB_BASEADR 0xbf200000
	
	#define MIU0_KSEG2_BASE_ADDR 	0x40000000 //MSTAR_MIU0_BUS_BASE
	#define MIU1_BUS_BASE_ADDR	0xA0000000 //MSTAR_MIU1_BUS_BASE
	#define MIU0_LOW_NONCACHE_SIZE	0x10000000

	// general MIPS physical memory map setting
	#define MIU0_PHY_BASE_ADDR	((unsigned long)0x00000000)
	/* MIU0 1.5G*/
	#define MIU0_SIZE		((unsigned long)0x60000000)

	#define MIU1_PHY_BASE_ADDR	((unsigned long)0x80000000)
	/* MIU1 1G*/
	#define MIU1_SIZE		((unsigned long)0x40000000)

	#define _BUS2PA(A)	\
		((A)<(MIU0_KSEG2_BASE_ADDR+MIU0_SIZE)?	\
			(((A)<MIU0_KSEG2_BASE_ADDR)? (A) :\
			((A)-MIU0_KSEG2_BASE_ADDR+MIU0_PHY_BASE_ADDR)):	\
			((((A)>= MIU1_BUS_BASE_ADDR)&&((A)<MIU1_BUS_BASE_ADDR+MIU1_SIZE))?	\
				((A)-MIU1_BUS_BASE_ADDR+MIU1_PHY_BASE_ADDR): \
				(0xFFFFFFFF)))

	//#define DEBUG_BUS_PA_PATCH
	#ifdef DEBUG_BUS_PA_PATCH
		extern unsigned int BUS2PA(unsigned int A);
	#else
		#define BUS2PA(A) _BUS2PA(A)
	#endif

	/* only non-cache map is considered */
	#define PA2BUS(A)	\
		((((A)>=MIU0_PHY_BASE_ADDR)&&((A)<(MIU0_PHY_BASE_ADDR+MIU0_SIZE)))?	\
			(((A)<MIU0_LOW_NONCACHE_SIZE)? (A) :	\
			((A)-MIU0_PHY_BASE_ADDR+MIU0_KSEG2_BASE_ADDR)):	\
			((((A)>= MIU1_PHY_BASE_ADDR)&&((A)<MIU1_PHY_BASE_ADDR+MIU1_SIZE))?	\
				((A)-MIU1_PHY_BASE_ADDR+MIU1_BUS_BASE_ADDR): \
				(0xFFFFFFFF)))
#else
#error chip setting
#endif

#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
#define BUS_PA_PATCH
#endif
//--------------------------------------------------------------------


//------ UTMI, USBC and UHC base address -----------------------------
//---- Port0
#define _MSTAR_UTMI0_BASE	(_MSTAR_USB_BASEADR+(0x3A80*2))
#define _MSTAR_UHC0_BASE	(_MSTAR_USB_BASEADR+(0x2400*2))
#define _MSTAR_USBC0_BASE	(_MSTAR_USB_BASEADR+(0x0700*2))
#define _MSTAR_BC0_BASE		(_MSTAR_USB_BASEADR+(0x23600*2))

//---- Port1
#define _MSTAR_UTMI1_BASE	(_MSTAR_USB_BASEADR+(0x3A00*2))
#define _MSTAR_UHC1_BASE	(_MSTAR_USB_BASEADR+(0x0D00*2))
#define _MSTAR_USBC1_BASE	(_MSTAR_USB_BASEADR+(0x0780*2))
#define _MSTAR_BC1_BASE		(_MSTAR_USB_BASEADR+(0x23620*2))
	
//---- Port2
#define _MSTAR_UTMI2_BASE	(_MSTAR_USB_BASEADR+(0x3900*2))
#define _MSTAR_UHC2_BASE	(_MSTAR_USB_BASEADR+(0x13900*2))
#define _MSTAR_USBC2_BASE	(_MSTAR_USB_BASEADR+(0x13800*2))
#define _MSTAR_BC2_BASE		(_MSTAR_USB_BASEADR+(0x23640*2))

//---- Port3
#define _MSTAR_UTMI3_BASE	(_MSTAR_USB_BASEADR+(0x22300*2))
#define _MSTAR_UHC3_BASE	(_MSTAR_USB_BASEADR+(0x53000*2))
#define _MSTAR_USBC3_BASE	(_MSTAR_USB_BASEADR+(0x53200*2))
#define _MSTAR_BC3_BASE		(_MSTAR_USB_BASEADR+(0x23680*2))
#define _MSTAR_EHC3_COMP_PORT (_MSTAR_USB_BASEADR+(0x90000*2)+0x440)

//---- Port4
#define _MSTAR_UTMI4_BASE	(_MSTAR_USB_BASEADR+(0x22380*2))
#define _MSTAR_UHC4_BASE	(_MSTAR_USB_BASEADR+(0x53100*2))
#define _MSTAR_USBC4_BASE	(_MSTAR_USB_BASEADR+(0x53280*2))
#define _MSTAR_BC4_BASE		(_MSTAR_USB_BASEADR+(0x236A0*2))
#define _MSTAR_EHC4_COMP_PORT (_MSTAR_USB_BASEADR+(0x90000*2)+0x450)

//--------------------------------------------------------------------
//------ Hardware ECO enable switch ----------------------------------
//---- 1. cross point
// every chip must enable it manually
#define ENABLE_LS_CROSS_POINT_ECO	
#define LS_CROSS_POINT_ECO_OFFSET	(0x04*2)
#define LS_CROSS_POINT_ECO_BITSET	BIT6

//---- 2. power noise
// every chip must enable it manually
#define ENABLE_PWR_NOISE_ECO

//---- 3. tx/rx reset clock gating cause XIU timeout
// every chip must enable it manually
#define ENABLE_TX_RX_RESET_CLK_GATING_ECO
#define TX_RX_RESET_CLK_GATING_ECO_OFFSET	(0x04*2)
#define TX_RX_RESET_CLK_GATING_ECO_BITSET	BIT5

//---- 4. short packet lose interrupt without IOC
//#define ENABLE_LOSS_SHORT_PACKET_INTR_ECO
//#define LOSS_SHORT_PACKET_INTR_ECO_OPOR
//#define LOSS_SHORT_PACKET_INTR_ECO_OFFSET	(0x04*2)
//#define LOSS_SHORT_PACKET_INTR_ECO_BITSET	BIT7

//---- 5. babble avoidance
//#define ENABLE_BABBLE_ECO

//---- 6. lose packet in MDATA condition
//#define ENABLE_MDATA_ECO
//#define MDATA_ECO_OFFSET	(0x0F*2-1)
//#define MDATA_ECO_BITSET	BIT4

//---- 7. change override to hs_txser_en condition (DM always keep high issue)
//#define ENABLE_HS_DM_KEEP_HIGH_ECO

//---- 8. fix pv2mi bridge mis-behavior
// every chip must enable it manually
#define ENABLE_PV2MI_BRIDGE_ECO

//---- 9. change to 55 interface
#define ENABLE_UTMI_55_INTERFACE

//---- 10. 240's phase as 120's clock
/* bit<3> for 240's phase as 120's clock set 1, bit<4> for 240Mhz in mac 0 for faraday 1 for etron */
#define ENABLE_UTMI_240_AS_120_PHASE_ECO

//---- 11. double date rate (480MHz)
#if 0 // no chip should set it
#define ENABLE_DOUBLE_DATARATE_SETTING
#endif

//---- 12. UPLL setting, normally it should be done in sboot
#if 0 // no chip should set it
#define ENABLE_UPLL_SETTING
#endif

//---- 13. chip top performance tuning
//#define ENABLE_CHIPTOP_PERFORMANCE_SETTING

//---- 14. HS connection fail problem (Gate into VFALL state)
#define ENABLE_HS_CONNECTION_FAIL_INTO_VFALL_ECO

//---- 15. Enable UHC Preamble ECO function
#define ENABLE_UHC_PREAMBLE_ECO


//---- 16. Don't close RUN bit when device disconnect
#define ENABLE_UHC_RUN_BIT_ALWAYS_ON_ECO

//--------------------------------------------------------------------


//------ Software patch enable switch --------------------------------
//---- 1. flush MIU pipe
// every chip must apply it
#define _USB_T3_WBTIMEOUT_PATCH 1

//---- 2. data structure (qtd ,...) must be 128-byte aligment
// every chip must apply it
#define _USB_128_ALIGMENT 1

//---- 3. tx/rx reset clock gating cause XIU timeout
#define _USB_XIU_TIMEOUT_PATCH 0

//---- 4. short packet lose interrupt without IOC
#define _USB_SHORT_PACKET_LOSE_INT_PATCH 0

//---- 5. QH blocking in MDATA condition, split zero-size data
// every chip must apply it
#define _USB_SPLIT_MDATA_BLOCKING_PATCH 1

//---- 6. DM always keep high issue
// if without ECO solution, use SW patch.
#define _USB_HS_CUR_DRIVE_DM_ALLWAYS_HIGH_PATCH 0

//---- 7. clear port eanble when device disconnect while bus reset
// every chip must apply it, so far
#define _USB_CLEAR_PORT_ENABLE_AFTER_FAIL_RESET_PATCH 1

//---- 8. mstar host only supports "Throttle Mode" in split translation
// every chip must apply it, so far
#define _USB_TURN_ON_TT_THROTTLE_MODE_PATCH 1

//---- 9. lower squelch level to cover weak cable link
#define _USB_ANALOG_RX_SQUELCH_PATCH 0

//---- 10. high speed reset chirp patch
//#define _USB_HS_CHIRP_PATCH 1

//---- 11. friendly customer patch
#define _USB_FRIENDLY_CUSTOMER_PATCH 1

//--------------------------------------------------------------------

//------ Reduce EOF1 to 12us for performance imporvement -------------
/* Enlarge EOP1 from 12us to 16us for babble prvention under hub case.
 * However, we keep the "old config name". 20130121
 */
#define ENABLE_12US_EOF1

//--------------------------------------------------------------------
#define _UTMI_PWR_SAV_MODE_ENABLE 0

//---- Setting PV2MI bridge read/write burst size to minimum
#define _USB_MINI_PV2MI_BURST_SIZE 1

//--------------------------------------------------------------------
#define _USB_UTMI_DPDM_SWAP_P0 0
#define _USB_UTMI_DPDM_SWAP_P1 0

//------ Titania3_series_start_ehc flag ------------------------------
// Use low word as flag
#define EHCFLAG_NONE			0x0
#define EHCFLAG_DPDM_SWAP		0x1
#define EHCFLAG_TESTPKG			0x2
#define EHCFLAG_DOUBLE_DATARATE		0x4
#define EHCFLAG_ENABLE_OTG		0x8

// Use high word as data
#define EHCFLAG_DDR_MASK		0xF0000000
#define EHCFLAG_DDR_x15			0x10000000 //480MHz x1.5
#define EHCFLAG_DDR_x18			0x20000000 //480MHz x1.8
//--------------------------------------------------------------------

//------ UTMI disconnect level parameters ---------------------------------
// 0x00: 550mv, 0x20: 575, 0x40: 600, 0x60: 625
#if defined(CONFIG_MSTAR_EINSTEIN) ||\
	defined(CONFIG_MSTAR_NAPOLI)
	// for 28 nm
	#define UTMI_DISCON_LEVEL_2A	(0x60)
#else
	#define UTMI_DISCON_LEVEL_2A	(0x0)
#endif
	
//------ UTMI eye diagram parameters ---------------------------------
#if defined(CONFIG_MSTAR_AMBER3) ||\
	defined(CONFIG_MSTAR_EAGLE) ||\
	defined(CONFIG_MSTAR_AMETHYST)
	// for 40nm
	#define UTMI_EYE_SETTING_2C	(0x98)
	#define UTMI_EYE_SETTING_2D	(0x02)
	#define UTMI_EYE_SETTING_2E	(0x10)
	#define UTMI_EYE_SETTING_2F	(0x01)
#elif 0
	// for 40nm after Agate, use 55nm setting7
	#define UTMI_EYE_SETTING_2C	(0x90)
	#define UTMI_EYE_SETTING_2D	(0x03)
	#define UTMI_EYE_SETTING_2E	(0x30)
	#define UTMI_EYE_SETTING_2F	(0x81)
#elif 0
	// for 40nm after Agate, use 55nm setting6
	#define UTMI_EYE_SETTING_2C	(0x10)
	#define UTMI_EYE_SETTING_2D	(0x03)
	#define UTMI_EYE_SETTING_2E	(0x30)
	#define UTMI_EYE_SETTING_2F	(0x81)
#elif 0
	// for 40nm after Agate, use 55nm setting5
	#define UTMI_EYE_SETTING_2C	(0x90)
	#define UTMI_EYE_SETTING_2D	(0x02)
	#define UTMI_EYE_SETTING_2E	(0x30)
	#define UTMI_EYE_SETTING_2F	(0x81)
#elif 0
	// for 40nm after Agate, use 55nm setting4
	#define UTMI_EYE_SETTING_2C	(0x90)
	#define UTMI_EYE_SETTING_2D	(0x03)
	#define UTMI_EYE_SETTING_2E	(0x00)
	#define UTMI_EYE_SETTING_2F	(0x81)
#elif 0
	// for 40nm after Agate, use 55nm setting3
	#define UTMI_EYE_SETTING_2C	(0x10)
	#define UTMI_EYE_SETTING_2D	(0x03)
	#define UTMI_EYE_SETTING_2E	(0x00)
	#define UTMI_EYE_SETTING_2F	(0x81)
#elif defined(CONFIG_MSTAR_EDISON) || \
	defined(CONFIG_MSTAR_KAISER) || \
	defined(CONFIG_MSTAR_EIFFEL) || \
	defined(CONFIG_MSTAR_KAISERS)
	// for 40nm after Agate, use 55nm setting2
	#define UTMI_EYE_SETTING_2C	(0x90)
	#define UTMI_EYE_SETTING_2D	(0x02)
	#define UTMI_EYE_SETTING_2E	(0x00)
	#define UTMI_EYE_SETTING_2F	(0x81)
#else
	// for 40nm after Agate, use 55nm setting1, the default
	#define UTMI_EYE_SETTING_2C	(0x10)
	#define UTMI_EYE_SETTING_2D	(0x02)
	#define UTMI_EYE_SETTING_2E	(0x00)
	#define UTMI_EYE_SETTING_2F	(0x81)
#endif

#endif	/* _EHCI_MSTAR_31023_H */
