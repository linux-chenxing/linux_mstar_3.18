/*
 * mtu3-mstar.h - MediaTek Mstar USB3 DRD header
 *
 * Copyright (C) 2016 MediaTek Inc.
 *
 * Author: Chunfeng Yun <chunfeng.yun@mediatek.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __MTU3_MSTAR_H__
#define __MTU3_MSTAR_H__

//#define LOCAL_DEBUG_ON
#ifdef LOCAL_DEBUG_ON
#define DBG(x...) printk(KERN_INFO x)
#else
#define DBG(x...)
#endif

#define MTU3_VERSION "20180222"

//#define MSTAR_ENABLE_U3DEV_U1_U2

//#define U3DEV_PHY_POWER_SAVING

#define MSTAR_ENABLE_TYPEC_CALLBACK

#define ENABLE_MSTAR_MEM_IRQ_REMAP

#define BIT0	(1U<<0)
#define BIT1	(1U<<1)
#define BIT2	(1U<<2)
#define BIT3	(1U<<3)
#define BIT4	(1U<<4)
#define BIT5	(1U<<5)
#define BIT6	(1U<<6)
#define BIT7	(1U<<7)

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

#if defined(CONFIG_ARCH_INFINITY2)
/* MIU setting */
#define USB_MIU_START	(0x00000000ULL)
#define USB_MIU_END	(0xffffffffULL)

#define MIU0_BUS_BASE_ADDR 	MSTAR_MIU0_BUS_BASE
#define MIU1_BUS_BASE_ADDR	MSTAR_MIU1_BUS_BASE

#define MIU0_PHY_BASE_ADDR	(0x00000000ULL)
/* MIU0 4G*/
#define MIU0_SIZE		(0x100000000ULL)

#define MIU1_PHY_BASE_ADDR	(0x100000000ULL)
/* MIU1 0G*/
#define MIU1_SIZE		(0x00000000ULL)

#define MIU_FAIL_ADDR ULONG_MAX

#define IN_MIU_BUS(A, NUM)	\
	(((A)>=MIU##NUM##_BUS_BASE_ADDR)&&((A)<=(MIU##NUM##_BUS_BASE_ADDR+MIU##NUM##_SIZE-1)))

#define IN_MIU_PA(A, NUM)	\
	(((A)>=MIU##NUM##_PHY_BASE_ADDR)&&((A)<=(MIU##NUM##_PHY_BASE_ADDR+MIU##NUM##_SIZE-1)))

#define PA_MIU(A, NUM)	\
	((A)-MIU##NUM##_BUS_BASE_ADDR+MIU##NUM##_PHY_BASE_ADDR)

#define BUS_MIU(A, NUM)	\
	((A)-MIU##NUM##_PHY_BASE_ADDR+MIU##NUM##_BUS_BASE_ADDR)

#define BUS2PA(A)	\
	(IN_MIU_BUS(A, 0) ? PA_MIU(A, 0) : (IN_MIU_BUS(A, 1)? PA_MIU(A, 1): MIU_FAIL_ADDR))

#define PA2BUS(A)	\
	(IN_MIU_PA(A, 0) ? BUS_MIU(A, 0) : (IN_MIU_PA(A, 1)? BUS_MIU(A, 1): MIU_FAIL_ADDR))

#define MSTAR_PIPE_PATCH

#define XHCI_SINGLE_PORT_ENABLE_MAC

#define XHCI_CHIRP_PATCH  1
#define ENABLE_XHCI_SSC 1

#if (ENABLE_XHCI_SSC)
//#define XHCI_SSC_TX_SYNTH_SET_C0			0x9374
#define XHCI_SSC_TX_SYNTH_SET_C0			0x9500 // MMM, I2
#define XHCI_SSC_TX_SYNTH_SET_C2			0x18
#define XHCI_SSC_TX_SYNTH_STEP_C4			0x7002
#define XHCI_SSC_TX_SYNTH_SPAN_C6			0x04D8
#endif

#define INT_MS_MTU3DEV (5+32)

#define MSTAR_PM_BASE	   0xfd000000UL
#define _MSTAR_U3PHY_DTOP0_BASE (MSTAR_PM_BASE+(0x02200*2))
#define _MSTAR_U3PHY_ATOP0_BASE (MSTAR_PM_BASE+(0x02300*2))
#define _MSTAR_U3DEV0_BASE      (MSTAR_PM_BASE+(0x02E00*2))
#define _MSTAR_U3UTMI0_BASE     (MSTAR_PM_BASE+(0x10B00*2))
#define _MSTAR_U3DEV0_X0_BASE   (MSTAR_PM_BASE+(0x90000*2))
#define _MSTAR_U3DEV0_X1_BASE   (MSTAR_PM_BASE+(0xA0000*2))

//------ Hardware ECO enable switch ----------------------------------
//---- 1. cross point
#define ENABLE_LS_CROSS_POINT_ECO
#define LS_CROSS_POINT_ECO_OFFSET	(0x04*2)
#define LS_CROSS_POINT_ECO_BITSET	BIT6

//---- 3. tx/rx reset clock gating cause XIU timeout
#define ENABLE_TX_RX_RESET_CLK_GATING_ECO
#define TX_RX_RESET_CLK_GATING_ECO_OFFSET	(0x04*2)
#define TX_RX_RESET_CLK_GATING_ECO_BITSET	BIT5

//---- 9. change to 55 interface
#define ENABLE_UTMI_55_INTERFACE

//---- 10. 240's phase as 120's clock
/* bit<3> for 240's phase as 120's clock set 1, bit<4> for 240Mhz in mac 0 for faraday 1 for etron */
#define ENABLE_UTMI_240_AS_120_PHASE_ECO

/* UTMI setting definition */
//------ Software patch enable switch --------------------------------
//---- 6. DM always keep high issue
#define _USB_HS_CUR_DRIVE_DM_ALLWAYS_HIGH_PATCH 0

//---- 9. lower squelch level to cover weak cable link
#define _USB_ANALOG_RX_SQUELCH_PATCH 0

//---- 12. enabe PVCI i_miwcplt wait for mi2uh_last_done_z
#define _USB_MIU_WRITE_WAIT_LAST_DONE_Z_PATCH 1
//---- 14. enable new chrip design, disonnect reg_2A, chrip detect reg_42
#define ENABLE_NEW_HW_CHRIP_PATCH

#define _USB_MINI_PV2MI_BURST_SIZE 1

//------ UTMI disconnect level parameters ---------------------------------
// disc: bit[7:4] 0x00: 550mv, 0x20: 575, 0x40: 600, 0x60: 625
// squelch: bit[3:0] 4'b0010 => 100mv
#define UTMI_DISCON_LEVEL_2A	(0x60 | 0x02)
#define UTMI_CHIRP_DCT_LEVEL_42	(0x80)

//------ UTMI eye diagram parameters ---------------------------------
#if 0
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
#elif 0
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

#endif

struct u3phy_d_addr_base {
	uintptr_t __iomem	utmi_base;
	uintptr_t __iomem	u3devtop_base;
	uintptr_t __iomem	u3dtop_base;
	uintptr_t __iomem	u3atop_base;
};

/* the module parameter */
#define DRIVER_DESC "MT USB 3.0 Device Controller Gadget"
#define DRIVER_VERSION "24 Aug 2017"
#define DRIVER_AUTHOR "MStar USB team"

#endif
