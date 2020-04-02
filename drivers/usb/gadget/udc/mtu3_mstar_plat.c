/*
 * Copyright (C) 2017 MstarSemi Inc.
 *
 * Author: USB team
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

#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/iopoll.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/pinctrl/consumer.h>
#include <linux/platform_device.h>
#include <linux/phy/phy.h> // MMM

#include "mtu3_mstar.h"
#include "mtu3.h"
//#include "mtu3_dr.h"

static void MIU_select_setting_u3dev(uintptr_t U3DEV_base)
{
	printk("[USB] config miu0 start [%llx] end [%llx]\n", USB_MIU_START, USB_MIU_END);
	writew((USB_MIU_START >> 20) & 0xffff, (void*)(U3DEV_base+0x8e*2));
	writew((USB_MIU_START >> (20+16)) & 0xffff, (void*)(U3DEV_base+0x90*2));
	writew((USB_MIU_START >> (20+32)) & 0xffff, (void*)(U3DEV_base+0x92*2));
	writew(USB_MIU_END >> 20, (void*)(U3DEV_base+0x94*2));
	writew(USB_MIU_END >> (20+16), (void*)(U3DEV_base+0x96*2));
	writew(USB_MIU_END >> (20+32), (void*)(U3DEV_base+0x98*2));
}

static void MStar_hsusb_utmi_init(uintptr_t UTMI_base, unsigned flag)
{
	printk("[USB] %s, UTMI %x\n", __func__, (unsigned)UTMI_base);
	if (flag & EHCFLAG_TESTPKG)
	{
		writew(0x2084, (void*)(UTMI_base+0x2*2));
		writew(0x8051, (void*)(UTMI_base+0x20*2));
	}

#if _USB_HS_CUR_DRIVE_DM_ALLWAYS_HIGH_PATCH
	/*
	 * patch for DM always keep high issue
	 * init overwrite register
	 */
	writeb(readb((void*)(UTMI_base+0x0*2)) & (u8)(~BIT3), (void*) (UTMI_base+0x0*2)); //DP_PUEN = 0
	writeb(readb((void*)(UTMI_base+0x0*2)) & (u8)(~BIT4), (void*) (UTMI_base+0x0*2)); //DM_PUEN = 0

	writeb(readb((void*)(UTMI_base+0x0*2)) & (u8)(~BIT5), (void*) (UTMI_base+0x0*2)); //R_PUMODE = 0

	writeb(readb((void*)(UTMI_base+0x0*2)) | BIT6, (void*) (UTMI_base+0x0*2)); //R_DP_PDEN = 1
	writeb(readb((void*)(UTMI_base+0x0*2)) | BIT7, (void*) (UTMI_base+0x0*2)); //R_DM_PDEN = 1

	writeb(readb((void*)(UTMI_base+0x10*2)) | BIT6, (void*) (UTMI_base+0x10*2)); //hs_txser_en_cb = 1
	writeb(readb((void*)(UTMI_base+0x10*2)) & (u8)(~BIT7), (void*) (UTMI_base+0x10*2)); //hs_se0_cb = 0

	/* turn on overwrite mode */
	writeb(readb((void*)(UTMI_base+0x0*2)) | BIT1, (void*) (UTMI_base+0x0*2)); //tern_ov = 1
	/* new HW term overwrite: on */
	writeb(readb((void*)(UTMI_base+0x52*2)) | (BIT5|BIT4|
		BIT3|BIT2|BIT1|BIT0), (void*) (UTMI_base+0x52*2));
#endif

	/* Turn on overwirte mode for D+/D- floating issue when UHC reset
	 * Before UHC reset, R_DP_PDEN = 1, R_DM_PDEN = 1, tern_ov = 1 */
	writeb(readb((void*)(UTMI_base+0x0*2)) | (BIT7|BIT6|BIT1), (void*) (UTMI_base+0x0*2));
	/* new HW term overwrite: on */
	writeb(readb((void*)(UTMI_base+0x52*2)) | (BIT5|BIT4|
		BIT3|BIT2|BIT1|BIT0), (void*) (UTMI_base+0x52*2));

#ifdef ENABLE_DOUBLE_DATARATE_SETTING
	writeb(readb((void*)(UTMI_base+0x0D*2-1)) | BIT0, (void*) (UTMI_base+0x0D*2-1)); // set reg_double_data_rate, To get better jitter performance
#endif

	/* MMM, U3 device not related to USBC IP */
	//writeb(0x0a, (void*)(USBC_base)); // Disable MAC initial suspend, Reset UHC
	//writeb(0x28, (void*)(USBC_base)); // Release UHC reset, enable UHC and OTG XIU function

	if (flag & EHCFLAG_DOUBLE_DATARATE)
	{
		if ((flag & EHCFLAG_DDR_MASK) == EHCFLAG_DDR_x15)
		{
			// Set usb bus = 480MHz x 1.5
			writeb(readb((void*)(UTMI_base+0x20*2)) | 0x76, (void*)(UTMI_base+0x20*2));
		}
		else if ((flag & EHCFLAG_DDR_MASK) == EHCFLAG_DDR_x18)
		{
			// Set usb bus = 480MHz x 1.8
			writeb(readb((void*)(UTMI_base+0x20*2)) | 0x8e, (void*)(UTMI_base+0x20*2));
		}
#if 0
		else if ((flag & EHCFLAG_DDR_MASK) == EHCFLAG_DDR_x20)
		{
			// Set usb bus = 480MHz x2
			writeb(readb((void*)(UTMI_base+0xd*2-1)) | 0x01, (void*)(UTMI_base+0xd*2-1));
		}
#endif
		/* Set slew rate control for overspeed (or 960MHz) */
		writeb(readb((void*)(UTMI_base+0x2c*2)) | BIT0, (void*) (UTMI_base+0x2c*2));
	}

	/* Init UTMI squelch level setting befor CA */
	if(UTMI_DISCON_LEVEL_2A & (BIT3|BIT2|BIT1|BIT0))
	{
		writeb((UTMI_DISCON_LEVEL_2A & (BIT3|BIT2|BIT1|BIT0)), (void*)(UTMI_base+0x2a*2));
		printk("[USB] init squelch level 0x%x\n", readb((void*)(UTMI_base+0x2a*2)));
	}

	writeb(readb((void*)(UTMI_base+0x3c*2)) | BIT0, (void*)(UTMI_base+0x3c*2)); // set CA_START as 1
	mdelay(1);

	writeb(readb((void*)(UTMI_base+0x3c*2)) & (u8)(~BIT0), (void*)(UTMI_base+0x3c*2)); // release CA_START

	while ((readb((void*)(UTMI_base+0x3c*2)) & BIT1) == 0);	// polling bit <1> (CA_END)

	if ((0xFFF0 == (readw((void*)(UTMI_base+0x3C*2)) & 0xFFF0 )) ||
		(0x0000 == (readw((void*)(UTMI_base+0x3C*2)) & 0xFFF0 ))  )
		printk("WARNING: CA Fail !! \n");

	if (flag & EHCFLAG_DPDM_SWAP)
		writeb(readb((void*)(UTMI_base+0x0b*2-1)) | BIT5, (void*)(UTMI_base+0x0b*2-1)); // dp dm swap
#if 0 //defined(CONFIG_USB_MS_OTG) || defined(CONFIG_USB_MS_OTG_MODULE)
	if(flag & EHCFLAG_ENABLE_OTG)
	{
		// let OTG driver to handle the UTMI switch control
	}
	else
#endif
	//writeb((u8)((readb((void*)(USBC_base+0x02*2)) & ~BIT1) | BIT0), (void*)(USBC_base+0x02*2)); // UHC select enable

	//writeb(readb((void*)(UHC_base+0x40*2)) & (u8)(~BIT4), (void*)(UHC_base+0x40*2)); // 0: VBUS On.
	udelay(1); // delay 1us

	//writeb(readb((void*)(UHC_base+0x40*2)) | BIT3, (void*)(UHC_base+0x40*2)); // Active HIGH

	/* Turn on overwirte mode for D+/D- floating issue when UHC reset
	 * After UHC reset, disable overwrite bits */
	writeb(readb((void*)(UTMI_base+0x0*2)) & (u8)(~(BIT7|BIT6|BIT1)), (void*) (UTMI_base+0x0*2));
	/* new HW term overwrite: off */
	writeb(readb((void*)(UTMI_base+0x52*2)) & (u8)(~(BIT5|BIT4|
		BIT3|BIT2|BIT1|BIT0)), (void*) (UTMI_base+0x52*2));

	/* improve the efficiency of USB access MIU when system is busy */
	//writeb(readb((void*)(UHC_base+0x81*2-1)) | (BIT0 | BIT1 | BIT2 | BIT3 | BIT7), (void*)(UHC_base+0x81*2-1));

	writeb((u8)((readb((void*)(UTMI_base+0x06*2)) & ~BIT5) | BIT6), (void*)(UTMI_base+0x06*2)); // reg_tx_force_hs_current_enable

	writeb((u8)((readb((void*)(UTMI_base+0x03*2-1)) & ~BIT4) | (BIT3 | BIT5)), (void*)(UTMI_base+0x03*2-1)); // Disconnect window select

	writeb(readb((void*)(UTMI_base+0x07*2-1)) & (u8)(~BIT1), (void*)(UTMI_base+0x07*2-1)); // Disable improved CDR

#if defined(ENABLE_UTMI_240_AS_120_PHASE_ECO)
	#if defined(UTMI_240_AS_120_PHASE_ECO_INV)
	writeb(readb((void*)(UTMI_base+0x08*2)) & (u8)(~BIT3), (void*)(UTMI_base+0x08*2)); //Set sprcial value for Eiffel USB analog LIB issue
	#else
	/* bit<3> for 240's phase as 120's clock set 1, bit<4> for 240Mhz in mac 0 for faraday 1 for etron */
	writeb(readb((void*)(UTMI_base+0x08*2)) | BIT3, (void*)(UTMI_base+0x08*2));
	#endif
#endif

	writeb(readb((void*)(UTMI_base+0x09*2-1)) | (BIT0 | BIT7), (void*)(UTMI_base+0x09*2-1)); // UTMI RX anti-dead-loc, ISI effect improvement

	if ((flag & EHCFLAG_DOUBLE_DATARATE)==0)
	    writeb(readb((void*)(UTMI_base+0x0b*2-1)) | BIT7, (void*)(UTMI_base+0x0b*2-1)); // TX timing select latch path

	writeb(readb((void*)(UTMI_base+0x15*2-1)) | BIT5, (void*)(UTMI_base+0x15*2-1)); // Chirp signal source select

#if defined(ENABLE_UTMI_55_INTERFACE)
	writeb(readb((void*)(UTMI_base+0x15*2-1)) | BIT6, (void*)(UTMI_base+0x15*2-1)); // change to 55 interface
#endif

	/* new HW chrip design, defualt overwrite to reg_2A */
	writeb(readb((void*)(UTMI_base+0x40*2)) & (u8)(~BIT4), (void*)(UTMI_base+0x40*2));

	/* Init UTMI disconnect level setting */
	writeb(UTMI_DISCON_LEVEL_2A, (void*)(UTMI_base+0x2a*2));

#if defined(ENABLE_NEW_HW_CHRIP_PATCH)
	/* Init chrip detect level setting */
	writeb(UTMI_CHIRP_DCT_LEVEL_42, (void*)(UTMI_base+0x42*2));
	/* enable HW control chrip/disconnect level */
	writeb(readb((void*)(UTMI_base+0x40*2)) & (u8)(~BIT3), (void*)(UTMI_base+0x40*2));
#endif

	/* Init UTMI eye diagram parameter setting */
	writeb(readb((void*)(UTMI_base+0x2c*2)) | UTMI_EYE_SETTING_2C, (void*)(UTMI_base+0x2c*2));
	writeb(readb((void*)(UTMI_base+0x2d*2-1)) | UTMI_EYE_SETTING_2D, (void*)(UTMI_base+0x2d*2-1));
	writeb(readb((void*)(UTMI_base+0x2e*2)) | UTMI_EYE_SETTING_2E, (void*)(UTMI_base+0x2e*2));
	writeb(readb((void*)(UTMI_base+0x2f*2-1)) | UTMI_EYE_SETTING_2F, (void*)(UTMI_base+0x2f*2-1));

#if defined(ENABLE_LS_CROSS_POINT_ECO)
	/* Enable deglitch SE0 (low-speed cross point) */
	writeb(readb((void*)(UTMI_base+LS_CROSS_POINT_ECO_OFFSET)) | LS_CROSS_POINT_ECO_BITSET, (void*)(UTMI_base+LS_CROSS_POINT_ECO_OFFSET));
#endif

#if defined(ENABLE_PWR_NOISE_ECO)
	/* Enable use eof2 to reset state machine (power noise) */
#endif

#if defined(ENABLE_TX_RX_RESET_CLK_GATING_ECO)
	/* Enable hw auto deassert sw reset(tx/rx reset) */
	writeb(readb((void*)(UTMI_base+TX_RX_RESET_CLK_GATING_ECO_OFFSET)) | TX_RX_RESET_CLK_GATING_ECO_BITSET, (void*)(UTMI_base+TX_RX_RESET_CLK_GATING_ECO_OFFSET));
#endif

#if defined(ENABLE_LOSS_SHORT_PACKET_INTR_ECO)
	/* Enable patch for the assertion of interrupt(Lose short packet interrupt) */
#endif

#if defined(ENABLE_BABBLE_ECO)
	/* Enable add patch to Period_EOF1(babble problem) */
#endif

#if defined(ENABLE_MDATA_ECO)
	/* Enable short packet MDATA in Split transaction clears ACT bit (LS dev under a HS hub) */
#endif

#if defined(ENABLE_HS_DM_KEEP_HIGH_ECO)
	/* Change override to hs_txser_en.  Dm always keep high issue */
	writeb(readb((void*)(UTMI_base+0x10*2)) | BIT6, (void*) (UTMI_base+0x10*2));
#endif

#if defined(ENABLE_HS_CONNECTION_FAIL_INTO_VFALL_ECO)
	/* HS connection fail problem (Gate into VFALL state) */
#endif

#if _USB_HS_CUR_DRIVE_DM_ALLWAYS_HIGH_PATCH
	/*
	 * patch for DM always keep high issue
	 * init overwrite register
	 */
	writeb(readb((void*)(UTMI_base+0x0*2)) | BIT6, (void*) (UTMI_base+0x0*2)); //R_DP_PDEN = 1
	writeb(readb((void*)(UTMI_base+0x0*2)) | BIT7, (void*) (UTMI_base+0x0*2)); //R_DM_PDEN = 1

	/* turn on overwrite mode */
	writeb(readb((void*)(UTMI_base+0x0*2)) | BIT1, (void*) (UTMI_base+0x0*2)); //tern_ov = 1
	/* new HW term overwrite: on */
	writeb(readb((void*)(UTMI_base+0x52*2)) | (BIT5|BIT4|
		BIT3|BIT2|BIT1|BIT0), (void*) (UTMI_base+0x52*2));
#endif

#if defined (ENABLE_PV2MI_BRIDGE_ECO)
#endif

#if _USB_ANALOG_RX_SQUELCH_PATCH
	/* squelch level adjust by calibration value */
	{
	unsigned int ca_da_ov, ca_db_ov, ca_tmp;

	ca_tmp = readw((void*)(UTMI_base+0x3c*2));
	ca_da_ov = (((ca_tmp >> 4) & 0x3f) - 5) + 0x40;
	ca_db_ov = (((ca_tmp >> 10) & 0x3f) - 5) + 0x40;
	printk("[%x]-5 -> (ca_da_ov, ca_db_ov)=(%x,%x)\n", ca_tmp, ca_da_ov, ca_db_ov);
	writeb(ca_da_ov ,(void*)(UTMI_base+0x3B*2-1));
	writeb(ca_db_ov ,(void*)(UTMI_base+0x24*2));
	}
#endif

#if _USB_MINI_PV2MI_BURST_SIZE
#endif

#if defined(ENABLE_UHC_PREAMBLE_ECO)
#endif

#if defined(ENABLE_UHC_RUN_BIT_ALWAYS_ON_ECO)
#endif

#if _USB_MIU_WRITE_WAIT_LAST_DONE_Z_PATCH
#endif

#if defined(ENABLE_UHC_EXTRA_HS_SOF_ECO)
#endif

/* Enable HS ISO IN Camera Cornor case ECO function */
#if defined(HS_ISO_IN_ECO_OFFSET)
#else
#endif

#if defined(ENABLE_DISCONNECT_SPEED_REPORT_RESET_ECO)
#endif

#if defined(ENABLE_BABBLE_PCD_ONE_PULSE_TRIGGER_ECO)
	/* Port Change Detect (PCD) is triggered by babble.
	 * Pulse trigger will not hang this condition.
	 */
#endif

#if defined(ENABLE_HC_RESET_FAIL_ECO)
	/* generation of hhc_reset_u */
#endif

#if defined(ENABLE_INT_AFTER_WRITE_DMA_ECO)
	/* DMA interrupt after the write back of qTD */
#endif

#if defined(ENABLE_DISCONNECT_HC_KEEP_RUNNING_ECO)
	/* EHCI keeps running when device is disconnected */
#endif

#if !defined(_EHC_SINGLE_SOF_TO_CHK_DISCONN)
#endif

#if defined(ENABLE_SRAM_CLK_GATING_ECO)
	/* do SRAM clock gating automatically to save power */
#endif

#if defined(MSTAR_EFUSE_RTERM)
	{
		u16 val;
		val = mstar_efuse_rterm();
		if (val & RTERM_VALID_BIT) {
			val = val & 0xF;
			writew((readw((void*)(UTMI_base+0x28*2)) & 0xFE1F) | (val<<5), (void*)(UTMI_base+0x28*2));
			printk("get rterm trim value: 0x%x\n", val);
		}
	}
#endif

	if (flag & EHCFLAG_TESTPKG)
	{
		writew(0x0600, (void*) (UTMI_base+0x14*2));
		writew(0x0078, (void*) (UTMI_base+0x10*2));
		writew(0x0bfe, (void*) (UTMI_base+0x32*2));
	}
}

//#define U3DEV_ENABLE_2ND_CHANNEL
static void MStar_U3phy_MS28_init(uintptr_t U3PHY_D_base, uintptr_t U3PHY_A_base)
{
	printk("[USB] %s, DTOP %x, ATOP %x\n", __func__,
		(unsigned)U3PHY_D_base, (unsigned)U3PHY_A_base);
#ifdef XHCI_SINGLE_PORT_ENABLE_MAC
	writeb(readb((void*)(U3PHY_D_base+0x84*2))| 0x40, (void*)(U3PHY_D_base+0x84*2)); // open XHCI MAC clock
#endif
	//-- 28 hpm mstar only---
	writew(0x0104, (void*) (U3PHY_A_base+0x6*2));  // for Enable 1G clock pass to UTMI //[2] reg_pd_usb3_purt [7:6] reg_gcr_hpd_vsel

	//U3phy initial sequence
	writew(0x0,    (void*) (U3PHY_A_base)); 		 // power on rx atop
	writew(0x0,    (void*) (U3PHY_A_base+0x2*2));	 // power on tx atop
#ifdef U3DEV_ENABLE_2ND_CHANNEL
	writew(0x0,   (void*)(U3PHY_A_base+0x4*2));
#endif
	//writew(0x0910, (void*) (U3PHY_D_base+0x4*2));        // the same as default
	writew(0x0,    (void*) (U3PHY_A_base+0x3A*2));  // overwrite power on rx/tx atop
	writew(0x0160, (void*) (U3PHY_D_base+0x18*2));
	writew(0x0,    (void*) (U3PHY_D_base+0x20*2));	 // power on u3_phy clockgen
	writew(0x0,    (void*) (U3PHY_D_base+0x22*2));	 // power on u3_phy clockgen

	//writew(0x013F, (void*) (U3PHY_D_base+0x4A*2));      // the same as default
	//writew(0x1010, (void*) (U3PHY_D_base+0x4C*2));      // the same as default

	writew(0x0,    (void*) (U3PHY_A_base+0x3A*2));	 // override PD control

#if (ENABLE_XHCI_SSC)
#ifdef XHCI_SSC_TX_SYNTH_SET_C0
	writew(XHCI_SSC_TX_SYNTH_SET_C0, (void*) (U3PHY_D_base+0xC0*2));  //reg_tx_synth_set
	writeb(XHCI_SSC_TX_SYNTH_SET_C2, (void*) (U3PHY_D_base+0xC2*2));  //reg_tx_synth_set
	writew(XHCI_SSC_TX_SYNTH_STEP_C4, (void*) (U3PHY_D_base+0xC4*2));  //reg_tx_synth_step
	writew(XHCI_SSC_TX_SYNTH_SPAN_C6, (void*) (U3PHY_D_base+0xC6*2));  //reg_tx_synth_span
#else
	writew(0x04D0,	(void*) (U3PHY_D_base+0xC6*2));  //reg_tx_synth_span
	writew(0x0003,	(void*) (U3PHY_D_base+0xC4*2));  //reg_tx_synth_step
	writew(0x9375,	(void*) (U3PHY_D_base+0xC0*2));  //reg_tx_synth_set
	writeb(0x18,	(void*) (U3PHY_D_base+0xC2*2));  //reg_tx_synth_set
#endif
#endif

	// -- TX current ---
	//writeb(0x01, 	(void*) (U3PHY_A_base+0x35*2-1));
	writew(0x3932,  (void*) (U3PHY_A_base+0x60*2));
	writew(0x3939,  (void*) (U3PHY_A_base+0x62*2));
	writew(0x3932,  (void*) (U3PHY_A_base+0x64*2));
	writew(0x3939,  (void*) (U3PHY_A_base+0x66*2));
	writew(0x0400,  (void*) (U3PHY_A_base+0x12*2));
	// ---------------

	//-------- New for MS28 ---------
	writeb(0x0,    (void*) (U3PHY_A_base+0xA1*2-1));  //bit[15] EQ override
	writeb(0xF4,   (void*) (U3PHY_D_base+0x12*2));	  //TX lock threshold

	writeb(readb((void*)(U3PHY_A_base+0xF*2-1))&(u8)(~0x4),	(void*)(U3PHY_A_base+0xF*2-1));  // 0xF[10]  Fix AEQ RX-reset issue

#ifdef XHCI_PHY_ENABLE_RX_LOCK
	writeb(readb((void*)(U3PHY_A_base+0x21*2-1)) | 0x80,	(void*)(U3PHY_A_base+0x21*2-1));  // enable rx_lock behavior.
#endif

#ifdef XHCI_PWS_P2
	writeb(readb((void*)(U3PHY_D_base+0x1E*2))| 0x1,	(void*)(U3PHY_D_base+0x1E*2));	//P2 power saving
#endif

	writew(0x80,    (void*) (U3PHY_D_base+0x30*2));	 // RX-detect retry ov
	writeb(readb((void*)(U3PHY_A_base+0xF0*2))| 0x4,	(void*)(U3PHY_A_base+0xF0*2));	// tLinkAround < 400 ns
	writeb(readb((void*)(U3PHY_D_base+0x1E*2)) & ~0x1,	(void*)(U3PHY_D_base+0x1E*2));	// reg_p2_power_saving_en=0
	writeb(readb((void*)(U3PHY_D_base+0x1F*2-1)) | 0x2,	(void*)(U3PHY_D_base+0x1F*2-1));	// reg_p2p3_power_saving_en=1
}

static void MStar_ssusb_phy_init(const struct u3phy_d_addr_base *u3phy_addr,
	unsigned utmi_flag)
{
	uintptr_t U3DEV_base = u3phy_addr->u3devtop_base;

	MStar_hsusb_utmi_init(u3phy_addr->utmi_base, utmi_flag);
	MStar_U3phy_MS28_init(u3phy_addr->u3dtop_base, u3phy_addr->u3atop_base);

	/* Set MIU0/1 space. */
	MIU_select_setting_u3dev(U3DEV_base);

	/* Don't do byte write parser */
	writeb(readb((void*)(U3DEV_base+0x72*2)) & ~BIT1, (void*)(U3DEV_base+0x72*2));

	/* Set DMA read length. */
	writeb(0xe8, (void*)(U3DEV_base+0x64*2));

	/* Set miu read don't wait last_done_z. */
	writeb(readb((void*)(U3DEV_base+0x04*2)) | BIT1, (void*)(U3DEV_base+0x04*2));

	/* Enable clock gating */ // FPGA
	//writeb(readb((void*)(U3DEV_base+0x08*2)) | 0x7, (void*)(U3DEV_base+0x08*2));

	/* SW note U3Phy init done. */
	//writeb(readb((void*)(U3TOP_base+0xEA*2)) | BIT0, (void*)(U3TOP_base+0xEA*2));
}

#ifdef U3DEV_PHY_POWER_SAVING
void ms_UPLL_power_switch(uintptr_t UPLL_PM_base, int on)
{
	if (on) {
		printk("[USB] UPLL on\n");
		writeb(readb((void*) (UPLL_PM_base)) & ~0x32, (void*) (UPLL_PM_base));
	}
	else {
		printk("[USB] UPLL off\n");
		writeb(readb((void*) (UPLL_PM_base)) | 0x32, (void*) (UPLL_PM_base));
	}
}

void ms_USB_XCVR_power_switch(uintptr_t UTMI_PM_base, int on)
{
	if (on) {
		printk("[USB] XCVR on\n");
		#if 0
		writew(readw((void*) (UTMI_PM_base)) & ~0xdf01, (void*) (UTMI_PM_base));
		writeb(readb((void*) (UTMI_PM_base+0x08*2)) & ~0x80, (void*) (UTMI_PM_base+0x08*2));
		writeb(readb((void*) (UTMI_PM_base+0x10*2)) & ~0x40, (void*) (UTMI_PM_base+0x10*2));
		#endif
		MStar_hsusb_utmi_init(UTMI_PM_base, 0); // initial UTMI again
	}
	else {
		printk("[USB] XCVR off\n");
		writew(readw((void*) (UTMI_PM_base)) | 0xdf01, (void*) (UTMI_PM_base));
		writeb(readb((void*) (UTMI_PM_base+0x08*2)) | 0x80, (void*) (UTMI_PM_base+0x08*2));
		writeb(readb((void*) (UTMI_PM_base+0x10*2)) | 0x40, (void*) (UTMI_PM_base+0x10*2));
	}
}

void ms_U3_APHY_power_switch(uintptr_t U3PHY_D_base, uintptr_t U3PHY_A_base, int on)
{
	//printk("[USB] U3 APHY not implemented!!!\n");
	//return;
	if (on) {
		printk("[USB] U3 APHY on\n");
		#if 1
		/* overwrite value */
		writew(0x66ff,	(void*) (U3PHY_A_base+0x00*2));
		writew(0x4eff,	(void*) (U3PHY_A_base+0x02*2));
		writeb(0x00,	(void*) (U3PHY_A_base+0x82*2));
		writeb(0x00,	(void*) (U3PHY_A_base+0x36*2));
		/* switch */
		writew(readw((void*) (U3PHY_A_base+0x3A*2)) & ~0x037f, (void*) (U3PHY_A_base+0x3A*2));
		writeb(readb((void*) (U3PHY_A_base+0x34*2)) & ~0x41, (void*) (U3PHY_A_base+0x34*2));
		writeb(readb((void*) (U3PHY_A_base+0x80*2)) & ~0x01, (void*) (U3PHY_A_base+0x80*2));
		//writeb(readb((void*) (U3PHY_A_base+0x6D*2-1)) & ~0xC0, (void*) (U3PHY_A_base+0x6D*2-1));
		#endif
		MStar_U3phy_MS28_init(U3PHY_D_base, U3PHY_A_base);
	}
	else {
		printk("[USB] U3 APHY off\n");
		/* overwrite value */
		writew(0x66ff,	(void*) (U3PHY_A_base+0x00*2));
		writew(0x4eff,	(void*) (U3PHY_A_base+0x02*2));
		writeb(0x00,	(void*) (U3PHY_A_base+0x82*2));
		writeb(0x00,	(void*) (U3PHY_A_base+0x36*2));
		/* switch */
		writew(readw((void*) (U3PHY_A_base+0x3A*2)) | 0x037f, (void*) (U3PHY_A_base+0x3A*2));
		writeb(readb((void*) (U3PHY_A_base+0x34*2)) | 0x41, (void*) (U3PHY_A_base+0x34*2));
		writeb(readb((void*) (U3PHY_A_base+0x80*2)) | 0x01, (void*) (U3PHY_A_base+0x80*2));
		//writeb(readb((void*) (U3PHY_A_base+0x6D*2-1)) | 0xC0, (void*) (U3PHY_A_base+0x6D*2-1));
	}
}
#endif

#ifdef MSTAR_ENABLE_TYPEC_CALLBACK
typedef	int (*usb_tcp_plug_notifier_fn_t)(bool polarity);
typedef	int (*usb_tcp_unplug_notifier_fn_t)(void);
//typedef	int (*usb_tcp_vbus_notifier_fn_t)(int mv, int ma);

extern usb_tcp_plug_notifier_fn_t usb_tcp_plug_notifier;
extern usb_tcp_unplug_notifier_fn_t usb_tcp_unplug_notifier;
//extern usb_tcp_vbus_notifier_fn_t usb_tcp_vbus_notifier;

#define U3PHY_A_BASE (0xfd000000+0x02300*2)
int usb_plug(bool polarity)
{
	uintptr_t U3PHY_A_base = U3PHY_A_BASE;

	printk("%s %d: polarity=%d\n", __func__, __LINE__, polarity);
	if (polarity)
		writew(0x0,   (void*)(U3PHY_A_base+0x4*2));
	else
		writew(0xfff,   (void*)(U3PHY_A_base+0x4*2));
	return 0;
}

int usb_unplug(void)
{
	printk("%s %d\n", __func__, __LINE__);
	return 0;
}
#endif

/* u2-port0 should be powered on and enabled; */
int ssusb_check_clocks(struct ssusb_mtk *ssusb, u32 ex_clks)
{
	void __iomem *ibase = ssusb->ippc_base;
	u32 value, check_val;
	int ret;

	check_val = ex_clks | SSUSB_SYS125_RST_B_STS | SSUSB_SYSPLL_STABLE |
			SSUSB_REF_RST_B_STS;

	ret = readl_poll_timeout(ibase + U3D_SSUSB_IP_PW_STS1, value,
			(check_val == (value & check_val)), 100, 20000);
	if (ret) {
		dev_err(ssusb->dev, "clks of sts1 are not stable!\n");
		return ret;
	}

	ret = readl_poll_timeout(ibase + U3D_SSUSB_IP_PW_STS2, value,
			(value & SSUSB_U2_MAC_SYS_RST_B_STS), 100, 10000);
	if (ret) {
		dev_err(ssusb->dev, "mac2 clock is not stable\n");
		return ret;
	}

	return 0;
}

static int ssusb_phy_init(struct ssusb_mtk *ssusb)
{
	int i;
	int ret;
	struct u3phy_d_addr_base *u3phy_init_addr;

	u3phy_init_addr = &ssusb->u3phy_mstar_addr;
	MStar_ssusb_phy_init(u3phy_init_addr, 0);

	for (i = 0; i < ssusb->num_phys; i++) {
		ret = phy_init(ssusb->phys[i]);
		if (ret)
			goto exit_phy;
	}
	return 0;

exit_phy:
	for (; i > 0; i--)
		phy_exit(ssusb->phys[i - 1]);

	return ret;
}

static int ssusb_phy_exit(struct ssusb_mtk *ssusb)
{
	int i;

	for (i = 0; i < ssusb->num_phys; i++)
		phy_exit(ssusb->phys[i]);

	return 0;
}

static int ssusb_phy_power_on(struct ssusb_mtk *ssusb)
{
	int i;
	int ret;

	for (i = 0; i < ssusb->num_phys; i++) {
		ret = phy_power_on(ssusb->phys[i]);
		if (ret)
			goto power_off_phy;
	}
	return 0;

power_off_phy:
	for (; i > 0; i--)
		phy_power_off(ssusb->phys[i - 1]);

	return ret;
}

static void ssusb_phy_power_off(struct ssusb_mtk *ssusb)
{
	unsigned int i;

	for (i = 0; i < ssusb->num_phys; i++)
		phy_power_off(ssusb->phys[i]);
}

static int ssusb_common_init(struct ssusb_mtk *ssusb)
{
	int ret = 0;

	ret = regulator_enable(ssusb->vusb33);
	if (ret) {
		dev_err(ssusb->dev, "failed to enable vusb33\n");
		goto vusb33_err;
	}

	ret = clk_prepare_enable(ssusb->sys_clk);
	if (ret) {
		dev_err(ssusb->dev, "failed to enable sys_clk\n");
		goto clk_err;
	}

	ret = ssusb_phy_init(ssusb);
	if (ret) {
		dev_err(ssusb->dev, "failed to init phy\n");
		goto phy_init_err;
	}

	ret = ssusb_phy_power_on(ssusb);
	if (ret) {
		dev_err(ssusb->dev, "failed to power on phy\n");
		goto phy_err;
	}

	return 0;

phy_err:
	ssusb_phy_exit(ssusb);

phy_init_err:
	clk_disable_unprepare(ssusb->sys_clk);

clk_err:
	regulator_disable(ssusb->vusb33);

vusb33_err:

	return ret;
}

static void ssusb_common_exit(struct ssusb_mtk *ssusb)
{
	clk_disable_unprepare(ssusb->sys_clk);
	regulator_disable(ssusb->vusb33);
	ssusb_phy_power_off(ssusb);
	ssusb_phy_exit(ssusb);
}

static void ssusb_ip_sw_reset(struct ssusb_mtk *ssusb)
{
	/* reset whole ip (xhci & u3d) */
	mtu3_setbits(ssusb->ippc_base, U3D_SSUSB_IP_PW_CTRL0, SSUSB_IP_SW_RST);
	udelay(1);
	mtu3_clrbits(ssusb->ippc_base, U3D_SSUSB_IP_PW_CTRL0, SSUSB_IP_SW_RST);
}

static int get_iddig_pinctrl(struct ssusb_mtk *ssusb)
{
	struct otg_switch_mtk *otg_sx = &ssusb->otg_switch;

	otg_sx->id_pinctrl = devm_pinctrl_get(ssusb->dev);
	if (IS_ERR(otg_sx->id_pinctrl)) {
		dev_err(ssusb->dev, "Cannot find id pinctrl!\n");
		return PTR_ERR(otg_sx->id_pinctrl);
	}

	otg_sx->id_float =
		pinctrl_lookup_state(otg_sx->id_pinctrl, "id_float");
	if (IS_ERR(otg_sx->id_float)) {
		dev_err(ssusb->dev, "Cannot find pinctrl id_float!\n");
		return PTR_ERR(otg_sx->id_float);
	}

	otg_sx->id_ground =
		pinctrl_lookup_state(otg_sx->id_pinctrl, "id_ground");
	if (IS_ERR(otg_sx->id_ground)) {
		dev_err(ssusb->dev, "Cannot find pinctrl id_ground!\n");
		return PTR_ERR(otg_sx->id_ground);
	}

	return 0;
}

static int get_ssusb_rscs(struct platform_device *pdev, struct ssusb_mtk *ssusb)
{
	struct device_node *node = pdev->dev.of_node;
	struct otg_switch_mtk *otg_sx = &ssusb->otg_switch;
	struct device *dev = &pdev->dev;
	struct regulator *vbus;
	struct resource *res;
	int i;
	int ret;

	ssusb->num_phys = of_count_phandle_with_args(node,
			"phys", "#phy-cells");
	if (ssusb->num_phys > 0) {
		ssusb->phys = devm_kcalloc(dev, ssusb->num_phys,
					sizeof(*ssusb->phys), GFP_KERNEL);
		if (!ssusb->phys)
			return -ENOMEM;
	} else {
		ssusb->num_phys = 0;
	}

	for (i = 0; i < ssusb->num_phys; i++) {
		//ssusb->phys[i] = devm_of_phy_get_by_index(dev, node, i);
		if (IS_ERR(ssusb->phys[i])) {
			dev_err(dev, "failed to get phy-%d\n", i);
			return PTR_ERR(ssusb->phys[i]);
		}
	}
	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "u3top");
	ssusb->u3phy_mstar_addr.u3devtop_base = (uintptr_t)res->start;
	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "u3dtop");
	ssusb->u3phy_mstar_addr.u3dtop_base = (uintptr_t)res->start;
	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "u3atop");
	ssusb->u3phy_mstar_addr.u3atop_base = (uintptr_t)res->start;
	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "utmi");
	ssusb->u3phy_mstar_addr.utmi_base = (uintptr_t)res->start;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "ippc");
#if defined(ENABLE_MSTAR_MEM_IRQ_REMAP)
	ssusb->ippc_base = (void *)res->start;
#else
	ssusb->ippc_base = devm_ioremap_resource(dev, res);
#endif
	if (IS_ERR(ssusb->ippc_base)) {
		dev_err(dev, "failed to map memory for ippc\n");
		return PTR_ERR(ssusb->ippc_base);
	}

	ssusb->vusb33 = devm_regulator_get(&pdev->dev, "vusb33");
	if (IS_ERR(ssusb->vusb33)) {
		dev_err(dev, "failed to get vusb33\n");
		return PTR_ERR(ssusb->vusb33);
	}

	#if 0 // TODO: sys clock?
	ssusb->sys_clk = devm_clk_get(dev, "sys_ck");
	if (IS_ERR(ssusb->sys_clk)) {
		dev_err(dev, "failed to get sys clock\n");
		return PTR_ERR(ssusb->sys_clk);
	}
	#endif

	ssusb->dr_mode = USB_DR_MODE_PERIPHERAL; // force device mode
	if (ssusb->dr_mode == USB_DR_MODE_UNKNOWN) {
		dev_err(dev, "dr_mode is error\n");
		return -EINVAL;
	}

	if (ssusb->dr_mode == USB_DR_MODE_PERIPHERAL)
		return 0;

	/* if host role is supported */
	//ret = ssusb_wakeup_of_property_parse(ssusb, node);
	//if (ret)
	//	return ret;

	if (ssusb->dr_mode != USB_DR_MODE_OTG)
		return 0;

	/* if dual-role mode is supported */
	vbus = devm_regulator_get(&pdev->dev, "vbus");
	if (IS_ERR(vbus)) {
		dev_err(dev, "failed to get vbus\n");
		return PTR_ERR(vbus);
	}
	otg_sx->vbus = vbus;

	otg_sx->is_u3_drd = of_property_read_bool(node, "mediatek,usb3-drd");
	otg_sx->manual_drd_enabled =
		of_property_read_bool(node, "enable-manual-drd");

	if (of_property_read_bool(node, "extcon")) {
		otg_sx->edev = extcon_get_edev_by_phandle(ssusb->dev, 0);
		if (IS_ERR(otg_sx->edev)) {
			dev_err(ssusb->dev, "couldn't get extcon device\n");
			return -EPROBE_DEFER;
		}
		if (otg_sx->manual_drd_enabled) {
			ret = get_iddig_pinctrl(ssusb);
			if (ret)
				return ret;
		}
	}

	dev_info(dev, "dr_mode: %d, is_u3_dr: %d\n",
		ssusb->dr_mode, otg_sx->is_u3_drd);

	return 0;
}

static int mtu3_probe(struct platform_device *pdev)
{
	//struct device_node *node = pdev->dev.of_node;
	struct device *dev = &pdev->dev;
	struct ssusb_mtk *ssusb;
	int ret = -ENOMEM;

	/* all elements are set to ZERO as default value */
	ssusb = devm_kzalloc(dev, sizeof(*ssusb), GFP_KERNEL);
	if (!ssusb)
		return -ENOMEM;

	ret = dma_set_mask_and_coherent(dev, DMA_BIT_MASK(32));
	if (ret) {
		dev_err(dev, "No suitable DMA config available\n");
		return -ENOTSUPP;
	}

	platform_set_drvdata(pdev, ssusb);
	ssusb->dev = dev;

	ret = get_ssusb_rscs(pdev, ssusb);
	if (ret)
		return ret;

	/* enable power domain */
	pm_runtime_enable(dev);
	pm_runtime_get_sync(dev);
	device_enable_async_suspend(dev);

	ret = ssusb_common_init(ssusb);
	if (ret)
		goto comm_init_err;

	ssusb_ip_sw_reset(ssusb);

	if (IS_ENABLED(CONFIG_USB_MTU3_HOST))
		ssusb->dr_mode = USB_DR_MODE_HOST;
	else if (IS_ENABLED(CONFIG_USB_MTU3_GADGET))
		ssusb->dr_mode = USB_DR_MODE_PERIPHERAL;

	/* default as host */
	ssusb->is_host = !(ssusb->dr_mode == USB_DR_MODE_PERIPHERAL);

	switch (ssusb->dr_mode) {
	case USB_DR_MODE_PERIPHERAL:
		ret = ssusb_gadget_init(ssusb);
		if (ret) {
			dev_err(dev, "failed to initialize gadget\n");
			goto comm_exit;
		}
		break;
	case USB_DR_MODE_HOST:
		//ret = ssusb_host_init(ssusb, node);
		//if (ret) {
		//	dev_err(dev, "failed to initialize host\n");
		//	goto comm_exit;
		//}
		break;
	case USB_DR_MODE_OTG:
		//ret = ssusb_gadget_init(ssusb);
		//if (ret) {
		//	dev_err(dev, "failed to initialize gadget\n");
		//	goto comm_exit;
		//}

		//ret = ssusb_host_init(ssusb, node);
		//if (ret) {
		//	dev_err(dev, "failed to initialize host\n");
		//	goto gadget_exit;
		//}

		//ssusb_otg_switch_init(ssusb);
		break;
	default:
		dev_err(dev, "unsupported mode: %d\n", ssusb->dr_mode);
		ret = -EINVAL;
		goto comm_exit;
	}

	/* registering type-c callback function */
#ifdef MSTAR_ENABLE_TYPEC_CALLBACK
	usb_tcp_plug_notifier = usb_plug;
	usb_tcp_unplug_notifier = usb_unplug;
	//usb_tcp_vbus_notifier = usb_vbus;
#endif

	return 0;

//gadget_exit:
//	ssusb_gadget_exit(ssusb);

comm_exit:
	ssusb_common_exit(ssusb);

comm_init_err:
	pm_runtime_put_sync(dev);
	pm_runtime_disable(dev);

	return ret;
}

static int mtu3_remove(struct platform_device *pdev)
{
	struct ssusb_mtk *ssusb = platform_get_drvdata(pdev);

	switch (ssusb->dr_mode) {
	case USB_DR_MODE_PERIPHERAL:
		ssusb_gadget_exit(ssusb);
		break;
	case USB_DR_MODE_HOST:
		//ssusb_host_exit(ssusb);
		break;
	case USB_DR_MODE_OTG:
		//ssusb_otg_switch_exit(ssusb);
		//ssusb_gadget_exit(ssusb);
		//ssusb_host_exit(ssusb);
		break;
	default:
		return -EINVAL;
	}

	ssusb_common_exit(ssusb);
	pm_runtime_put_sync(&pdev->dev);
	pm_runtime_disable(&pdev->dev);

	return 0;
}

/*
 * when support dual-role mode, we reject suspend when
 * it works as device mode;
 */
static int __maybe_unused mtu3_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct ssusb_mtk *ssusb = platform_get_drvdata(pdev);

	dev_dbg(dev, "%s\n", __func__);

	/* REVISIT: disconnect it for only device mode? */
	if (!ssusb->is_host)
		return 0;

	//ssusb_host_disable(ssusb, true);
	ssusb_phy_power_off(ssusb);
	clk_disable_unprepare(ssusb->sys_clk);
	//ssusb_wakeup_enable(ssusb);

	return 0;
}

static int __maybe_unused mtu3_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct ssusb_mtk *ssusb = platform_get_drvdata(pdev);

	dev_dbg(dev, "%s\n", __func__);

	if (!ssusb->is_host)
		return 0;

	//ssusb_wakeup_disable(ssusb);
	clk_prepare_enable(ssusb->sys_clk);
	ssusb_phy_power_on(ssusb);
	//ssusb_host_enable(ssusb);

	return 0;
}

static const struct dev_pm_ops mtu3_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(mtu3_suspend, mtu3_resume)
};

#define DEV_PM_OPS (IS_ENABLED(CONFIG_PM) ? &mtu3_pm_ops : NULL)

#ifdef CONFIG_OF

static const struct of_device_id mtu3_of_match[] = {
	//{.compatible = "mediatek,mt8173-mtu3",},
	{.compatible = "mtu3-udc",},
	{},
};

MODULE_DEVICE_TABLE(of, mtu3_of_match);

#endif

static struct platform_driver mtu3_driver = {
	.probe = mtu3_probe,
	.remove = mtu3_remove,
	.driver = {
		.name = MTU3_DRIVER_NAME,
		.pm = DEV_PM_OPS,
		.of_match_table = of_match_ptr(mtu3_of_match),
	},
};

static struct resource mtu3_udc_device_resource[] =
{
	[0] = {
		.name = "ippc",
		.start = _MSTAR_U3DEV0_X0_BASE,
		.end   = _MSTAR_U3DEV0_X1_BASE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.name = "mac",
		.start = _MSTAR_U3DEV0_X1_BASE,
		.end   = _MSTAR_U3DEV0_X1_BASE + 0x10000 * 2 - 1,
		.flags = IORESOURCE_MEM,
	},
	[2] = {
		.name = "u3top",
		.start = _MSTAR_U3DEV0_BASE,
		.end   = _MSTAR_U3DEV0_BASE + 0x100 * 2 - 1,
		.flags = IORESOURCE_MEM,
	},
	[3] = {
		.name = "u3dtop",
		.start = _MSTAR_U3PHY_DTOP0_BASE,
		.end   = _MSTAR_U3PHY_DTOP0_BASE + 0x100 * 2 - 1,
		.flags = IORESOURCE_MEM,
	},
	[4] = {
		.name = "u3atop",
		.start = _MSTAR_U3PHY_ATOP0_BASE,
		.end   = _MSTAR_U3PHY_ATOP0_BASE + 0x100 * 2 - 1,
		.flags = IORESOURCE_MEM,
	},
	[5] = {
		.name = "utmi",
		.start = _MSTAR_U3UTMI0_BASE,
		.end   = _MSTAR_U3UTMI0_BASE + 0x80 * 2 - 1,
		.flags = IORESOURCE_MEM,
	},
	[6] = {
		.start = INT_MS_MTU3DEV,
		.end   = INT_MS_MTU3DEV,
		.flags = IORESOURCE_IRQ,
	}
};

#define usb_dma_limit ((phys_addr_t)~0)
static u64 udc_dmamask = usb_dma_limit;
struct platform_device mtu3_udc_device =
{
	.name             = MTU3_DRIVER_NAME,
	.id               = 0x10411,
	.dev =
	{
		.dma_mask		= &udc_dmamask,
		.coherent_dma_mask	= usb_dma_limit, // add for limit DMA range
	},

	.num_resources    = ARRAY_SIZE(mtu3_udc_device_resource),
	.resource         = mtu3_udc_device_resource,
};

static int __init mtu3_udc_init(void)
{
	int err;
	DBG("Entered %s: gadget_name=%s version=%s\n", __FUNCTION__, MTU3_DRIVER_NAME, DRIVER_VERSION);
	printk("MTU3 UDC INIT %s\n", MTU3_VERSION);

	err = platform_device_register(&mtu3_udc_device);
	if (err) {
		pr_err("%s(#%d): platform_device_register failed(%d)\n",
                __func__, __LINE__, err);
		return err;
	}

	err =  platform_driver_register(&mtu3_driver);
	if (err) {
		dev_err(&(mtu3_udc_device.dev), "%s(#%d): platform_driver_register fail(%d)\n",
			__func__, __LINE__, err);
		goto dev_reg_failed;
	}

	return err;

dev_reg_failed:
    platform_device_unregister(&mtu3_udc_device);

    return err;
}

static void __exit mtu3_udc_exit(void)
{
	DBG("Entered %s \n", __FUNCTION__);
	platform_driver_unregister(&mtu3_driver);
}

module_init(mtu3_udc_init);
module_exit(mtu3_udc_exit);

MODULE_AUTHOR("Mstar USB team");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("MediaTek USB3 DRD Controller Driver");
