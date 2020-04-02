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

/******************************************************************************
 * SW patch config
 ******************************************************************************/
//***** for Cedric C3 patch*****//
#define DRAM_MORE_THAN_1G_PATCH	//Clippers HW switch miu0/miu1 at 1G, if miu0 > 1G, HW will access wrong place
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
#define RIU_BASE           0xfd200000
#define UTMI_BASE_ADDR     GET_UDC_REG_ADDR(RIU_BASE, 0x3800)
#define USBC_BASE_ADDR     GET_UDC_REG_ADDR(RIU_BASE, 0x13800)
#define OTG0_BASE_ADDR     GET_UDC_REG_ADDR(RIU_BASE, 0x12600)
#define BC_BASE_ADDR	   GET_UDC_REG_ADDR(RIU_BASE, 0x53880)
#endif  /* define _UDC_MSTAR_31023_H */
