//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    drvHDMITx.c
/// @author MStar Semiconductor Inc.
/// @brief  HDMI Tx Driver Interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#define  MDRV_HDMITX_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#else
#include <string.h>
#endif
#include "MsCommon.h"
#include "MsVersion.h"
#include "drvMMIO.h"
#include "regHDMITx.h"
#include "halHDMIUtilTx.h"
#include "halHDMITx.h"
#include "halHDCPTx.h"
#include "drvHDMITx.h"
#ifdef VANCLEEF_MCP
#include "drvHDMITx_vcf.h"
#endif
//#include <bigd.h>

#ifdef CUSTOMER_NDS
#include "apiCEC.h"
#include "drvCEC.h"
#endif // CUSTOMER_NDS

#include "../../api/cec/apiCEC.h"
#include "../cec/drvCEC.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define HDMITX_TASK_STACK_SIZE          4096
#define HDMITX_MUTEX_TIMEOUT            2000        ///< The maximum timeout
#define HDMITX_MONITOR_DELAY            50

#define USE_INTERNAL_HDCP_KEY           1 // Use internal HDCP key
#define USE_AUTO_GEN_AN                 1 // Use auto An generator
#define CheckRx_Timer                   100 // Check Rx HPD and TMDS clock status

/*
 * HDCP CTS 1A-01: NOT JUDGED
 * Refine timing to fix Ri Timer expired issue: Inactivity timer expired.
 */
#define HDCP_1ST_RiCheck_Timer          100 //2600 // Check HDCP Ri value in both of Rx and Tx sides
#define HDCP_RiCheck_Timer              500 //2600 // Check HDCP Ri value in both of Rx and Tx sides

//In Test Instrument, Quantumdata882, it read each DDC step with 30ms sample periods
//While doing SEC Compatibility Test, we find some REPEATER need enlarge sample rate to above 20ms to avoid timeout problem (YAMAHA RX-V2700)
//In SEC Compatibility Test, another repeater face RX Ri freeze after auth done. Enlarge sample rate to 125ms to avoid problem (YAMAHA RX-V467)
#define HDCP_DDC_SAMPLE_PERIOD          125 //20//10 //SEC Compatibility Test (YAMAHA RX-V2700)(YAMAHA RX-V467)

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------
#if 0
typedef	struct	{
    BIGD   p, q, g, x, y;
} dsa_context;
#endif
//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

MDrvHDMITX_PARAMETER_LIST     gHDMITxInfo;
MsHDMITX_INTERRUPT_TYPE gHDMITXIRQ = E_HDMITX_IRQ_12|E_HDMITX_IRQ_10; // HPD and Clock disconnect
MS_U8 gRepeaterFailTime = 0; // for MAX_CASCADE_EXCEEDED and MAX_DEVS_EXCEEDED
MS_U32 gRepeaterStartTime = 0; // for MAX_CASCADE_EXCEEDED and MAX_DEVS_EXCEEDED
MS_U16 gHDCPCheckRiTimer = HDCP_RiCheck_Timer;//2600;
MS_BOOL g_bDisableRegWrite = FALSE;
MS_BOOL g_bDisableTMDSCtrl = FALSE;
MS_BOOL g_bHDMITxTask = TRUE;
MS_BOOL g_bHDCPRxValid = TRUE; //E_HDMITX_HDCP_RX_IS_NOT_VALID //E_HDMITX_HDCP_RX_IS_VALID


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

static void                           *_pHDMITxTaskStack;
static MS_S32                          _s32HDMITxTaskId = -1;
static MS_S32                          _s32HDMITxEventId;
///static MS_S32                          _s32HDMITxMutexId;
static MS_U8                           _u8HDMITx_StackBuffer[HDMITX_TASK_STACK_SIZE];
static MS_S32                       _s32CheckRxTimerId = -1;
static MS_S32                       _s32HDCPRiTimerId = -1;

static MS_U32                       _u32PreAksvWroteTime = 0;
static MS_U32                       _u32Aksv2RoInterval = 150;


MS_U8 HDMITX_EdidHeaderTbl[] =
{
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
};

MS_U8 HDMITX_IEEEHdmiIdTbl[] =
{
    0x03, 0x0C, 0x00,
};

#define HDMITX_DRV_VERSION                  /* Character String for DRV/API version             */  \
    MSIF_TAG,                           /* 'MSIF'                                           */  \
    MSIF_CLASS,                         /* '00'                                             */  \
    MSIF_CUS,                           /* 0x0000                                           */  \
    MSIF_MOD,                           /* 0x0000                                           */  \
    MSIF_CHIP,                                                                                  \
    MSIF_CPU,                                                                                   \
    {'P','Q','_','_'},                  /* IP__                                             */  \
    {'0','0'},                          /* 0.0 ~ Z.Z                                        */  \
    {'0','0'},                          /* 00 ~ 99                                          */  \
    {'0','0','1','1','5','1','5','7'},  /* CL#                                              */  \
    MSIF_OS

/// Debug information
#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
static MSIF_Version _drv_hdmi_tx_version = {{ HDMITX_DRV_VERSION },};
static MS_HDMI_TX_INFO _info =
    {
        0,
    };

#else
static MSIF_Version _drv_hdmi_tx_version = {.DDI = { HDMITX_DRV_VERSION },};
static MS_HDMI_TX_INFO _info =
    {
        // TODO: ADD hdmi tx related info here.
    };
#endif


static MS_HDMI_TX_Status _hdmitx_status = {.bIsInitialized = FALSE, .bIsRunning = FALSE,};

#ifdef MS_DEBUG
static MS_U16 _u16DbgHDMITXSwitch = HDMITX_DBG|HDMITX_DBG_HDCP;
#else
static MS_U16 _u16DbgHDMITXSwitch = 3;
#endif

MS_BOOL bCheckEDID = TRUE; // Check EDID only when HPD from low to high

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

#define DBG_HDMITX(_f)                do{ if(_u16DbgHDMITXSwitch & HDMITX_DBG) (_f); } while(0);
#define DBG_HDCP(_f)                  do{ if(_u16DbgHDMITXSwitch & HDMITX_DBG_HDCP) (_f); } while(0);

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//extern MS_BOOL MDrv_EEPROM_Write(MS_U32 u32Addr, MS_U8 *pu8Buf, MS_U32 u32Size);
MS_BOOL MDrv_EEPROM_Read(MS_U32 u32Addr, MS_U8 *pu8Buf, MS_U32 u32Size)
{
    return TRUE;
}

//------------------------------------------------------------------------------
/// @brief Set event of HDMI ISR
/// @param  None
/// @return None
//------------------------------------------------------------------------------
#ifdef MSOS_TYPE_LINUX_KERNEL
irqreturn_t _HDMITx_Isr(InterruptNum eIntNum, void* dev_id)
{
    MS_U32 irq_status;
    eIntNum = eIntNum;      // prevent compile warning

    //Clear Interrupt Bit
    irq_status = MHal_HDMITx_Int_Status();
    if(irq_status & E_HDMITX_IRQ_12) // TMDS hot-plug
    {
        MHal_HDMITx_Int_Clear(E_HDMITX_IRQ_12);
    }
    else if(irq_status & E_HDMITX_IRQ_10) // Rx disconnection
    {
        MHal_HDMITx_Int_Clear(E_HDMITX_IRQ_10);
    }

    MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_IRQ);
    return IRQ_HANDLED;
}

#ifdef ENABLE_CEC_INT
irqreturn_t _HDMITX_CEC_RxIsr(InterruptNum eIntNum, void* dev_id)
{
    eIntNum = eIntNum;      // prevent compile warning

    MDrv_CEC_RxChkBuf();
    MsOS_EnableInterrupt(E_INT_IRQ_CEC);

    MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_CECRX);
    return IRQ_HANDLED;
}
#endif // ENABLE_CEC_INT

#else

static void _HDMITx_Isr(InterruptNum eIntNum)
{
    MS_U32 irq_status;
    //eIntNum = eIntNum;      // prevent compile warning
    DBG_HDCP(printf("_HDMITx_Isr!!! %d \r\n", eIntNum)); // Fix coverity impact.

    //Clear Interrupt Bit
    irq_status = MHal_HDMITx_Int_Status();
    if(irq_status & E_HDMITX_IRQ_12) // TMDS hot-plug
    {
        MHal_HDMITx_Int_Clear(E_HDMITX_IRQ_12);
    }
    else if(irq_status & E_HDMITX_IRQ_10) // Rx disconnection
    {
        MHal_HDMITx_Int_Clear(E_HDMITX_IRQ_10);
    }
    MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_IRQ);
}

#ifdef ENABLE_CEC_INT
static void _HDMITX_CEC_RxIsr(InterruptNum eIntNum)
{
    //eIntNum = eIntNum;      // prevent compile warning
    DBG_HDCP(printf("_HDMITX_CEC_RxIsr!!! %d \r\n", eIntNum)); // Fix coverity impact.

    MDrv_CEC_RxChkBuf();
    MsOS_EnableInterrupt(E_INT_IRQ_CEC);

    MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_CECRX);
}
#endif // ENABLE_CEC_INT

#endif


//------------------------------------------------------------------------------
/// @brief This routine is to check HDCP sync setting
/// @return None
//------------------------------------------------------------------------------
MS_BOOL _MDrv_HDMITx_HdcpSyncSetting(MsHDMITX_OUTPUT_MODE tmds_mode)
{
    MS_U8 temp, regval = 0; // Fix coverity impact.
    MS_U8 tx_mode = 0;
    MS_BOOL tmp = 0;  // Fix coverity impact.

    //Check Bcaps
    if(MHal_HDMITx_Rx74ReadByte(0x40, &temp) == FALSE)
        return FALSE;

    // EESS/OESS, advance cipher
    if (tmds_mode & BIT1)
    {
        tx_mode |= BIT2;      // EESS

        if (temp & BIT1)
        {
            tx_mode |= BIT3;
            tmp = MHal_HDMITx_Rx74WriteByte(0x15, 0x02);
            tmp = MHal_HDMITx_Rx74ReadByte(0x15, &regval);
            DBG_HDCP(printf("HDMI Advanced Cipher!!! %d \r\n", regval))
        }
    }
    else     //DVI
    {
        if(temp & BIT1)
        {
            tx_mode  |= BIT2;
            tx_mode  |= BIT3;
            tmp = MHal_HDMITx_Rx74WriteByte(0x15,0x02);
            DBG_HDCP(printf("Tx select EESS and enable Advance Cipher!!\r\n"));
        }
    }

    // Repeater mode?
    gHDMITxInfo.hdmitx_HdcpCheckRepeater_flag = FALSE;
    if (temp & BIT6)
    {
        tx_mode |= BIT1;
        gHDMITxInfo.hdmitx_HdcpCheckRepeater_flag = TRUE;
        DBG_HDCP(printf("Rx is a Repeater!!!\r\n"));
    }
    MHal_HDMITX_SetHDCPConfig(tx_mode);
    tmp = TRUE;
    return tmp;
}

//------------------------------------------------------------------------------
/// @brief This routine is to HDCP authentication process
/// @return authentication status
//------------------------------------------------------------------------------
MsHDMITX_HDCP_AUTH_STATUS _MDrv_HDMITx_HdcpAuthExangedAndComputated(MsHDMITX_OUTPUT_MODE tmds_mode)
{
    gHDMITxInfo.hdmitx_HdcpCheckRepeater_flag = FALSE;
    if(_MDrv_HDMITx_HdcpSyncSetting(tmds_mode) == FALSE)
        return E_HDMITX_HDCP_TX_KEY_FAIL;

    //in Test Instrument, Quantumdata882, it read each DDC step with 30ms sample periods
    MsOS_DelayTask(HDCP_DDC_SAMPLE_PERIOD);

    // HDCP CTS 1A-02
    gHDMITxInfo.hdmitx_preRX_status = MHal_HDMITx_GetRXStatus();
    if((MHal_HDMITx_HdcpWriteAn(USE_AUTO_GEN_AN) == FALSE) || (gHDMITxInfo.hdmitx_preRX_status != E_HDMITX_DVIClock_H_HPD_H))
    {
        gHDMITxInfo.HDCP_74_check = FALSE;
        return E_HDMITX_HDCP_TX_KEY_FAIL;
    }
    //in Test Instrument, Quantumdata882, it read each DDC step with 30ms sample periods
    MsOS_DelayTask(HDCP_DDC_SAMPLE_PERIOD);

    // HDCP CTS 1A-02
    gHDMITxInfo.hdmitx_preRX_status = MHal_HDMITx_GetRXStatus();
    if ((MHal_HDMITx_HdcpWriteAksv() == FALSE) || (gHDMITxInfo.hdmitx_preRX_status != E_HDMITX_DVIClock_H_HPD_H))
    {
        gHDMITxInfo.HDCP_74_check = FALSE;
        return E_HDMITX_HDCP_TX_KEY_FAIL;
    }

    //record current time after writing Aksv
    _u32PreAksvWroteTime = MsOS_GetSystemTime();

    // HDCP CTS 1A-02
    gHDMITxInfo.hdmitx_preRX_status = MHal_HDMITx_GetRXStatus();
    if ((MHal_HDMITx_HdcpCheckBksvLn() == FALSE) || (gHDMITxInfo.hdmitx_preRX_status != E_HDMITX_DVIClock_H_HPD_H))
    {
        gHDMITxInfo.hdmitx_preRX_status = MHal_HDMITx_GetRXStatus();
        if ((MHal_HDMITx_HdcpCheckBksvLn() == FALSE) || (gHDMITxInfo.hdmitx_preRX_status != E_HDMITX_DVIClock_H_HPD_H))
        {
        #ifdef CUSTOMER_NDS
            gHDMITxInfo.events |= HDMI_EVENT_CP_KSV_INVALID;
        #endif
            gHDMITxInfo.HDCP_74_check = FALSE;
            return E_HDMITX_HDCP_RX_KEY_FAIL;
        }
    }

    MHal_HDMITx_GET74(0x00, gHDMITxInfo.HDCP_BKSV);
    MHal_HDMITx_GET74(0x10, gHDMITxInfo.HDCP_AKSV);

    // HDCP CTS 1A-02
    gHDMITxInfo.hdmitx_preRX_status = MHal_HDMITx_GetRXStatus();
    if (gHDMITxInfo.hdmitx_preRX_status != E_HDMITX_DVIClock_H_HPD_H)
    {
        gHDMITxInfo.HDCP_74_check = FALSE;
        return E_HDMITX_HDCP_RX_KEY_FAIL;
    }

#if 1 //For SRM, mark at this stage
    if(MDrv_HDMITx_HDCP_CheckRevokedKey())
    {
#ifdef CUSTOMER_NDS
        gHDMITxInfo.events |= HDMI_EVENT_CP_KSV_REVOKED;
#endif // CUSTOMER_NDS
        return E_HDMITX_HDCP_RX_KEY_REVOKED;
    }
#endif
    MHal_HDMITx_HdcpStartCipher();
    gHDMITxInfo.HDCP_74_check = TRUE;
    return E_HDMITX_HDCP_RX_IS_VALID; // completed the exchaned and computed

 }


//------------------------------------------------------------------------------
/// @brief This routine is to HDCP waiting for the active Rx
/// @return authentication status
//------------------------------------------------------------------------------
MsHDMITX_HDCP_AUTH_STATUS _MDrv_HDMITx_HdcpAuthWaitingActiveRx(void)
{
    MHal_HDMITx_HdcpInit();
    if (!MHal_HDMITx_HdcpRxActive())
    {
    #if 0 // for HDCP CTS
    #ifndef CUSTOMER_NDS
        MHal_HDMITx_HdcpSetEncrypt(ENABLE);
    #endif
    #endif
        DBG_HDCP(printf("No HDCP Rx!!!\r\n"));
#ifdef VANCLEEF_MCP
        MDrv_HDMITx_Vancleef_RxLoadBCaps(0x80);
#endif
        return E_HDMITX_HDCP_RX_IS_NOT_VALID; // not active
    }

#ifdef VANCLEEF_MCP
    //VERY IMPORTANT!!
    //Vancleef
    MDrv_HDMITx_Vancleef_RxLoadBCaps(0xC0);

    //KaiserS
    {
        // indirect write
        MHal_HDMITx_Mask_Write(HDCP_REG_BASE, 0x17, BIT(10), BIT(10)); // [10]: HDCP enable for DDC
        MHal_HDMITx_Mask_Write(HDCP_REG_BASE, 0x19, BIT(15)|BIT(14), BIT(15)); // [15]: CPU write enable, [14]: 0: 74 RAM, 1 :HDCP RAM

        // Bcaps = 0x80
        MHal_HDMITx_Mask_Write(HDCP_REG_BASE, 0x17, BMASK(9:0), 0x40); // address
        MHal_HDMITx_Mask_Write(HDCP_REG_BASE, 0x19, BIT(5), BIT(5)); // trigger latch address

        MHal_HDMITx_Mask_Write(HDCP_REG_BASE, 0x18, BMASK(7:0), 0xC0); // data
        MHal_HDMITx_Mask_Write(HDCP_REG_BASE, 0x19, BIT(4), BIT(4)); // trigger latch data

        MsOS_DelayTask(1);
        MHal_HDMITx_Mask_Write(HDCP_REG_BASE, 0x19, BIT(15)|BIT(14), 0); // [15]: CPU write disable, [14]: 0: 74 RAM, 1 :HDCP RAM
    }
#endif
	return E_HDMITX_HDCP_RX_IS_VALID; // active Rx
}


//------------------------------------------------------------------------------
/// @brief This routine is to HDCP check whether repeater is ready or not
/// @return authentication status
//------------------------------------------------------------------------------
MsHDMITX_HDCP_AUTH_STATUS _MDrv_HDMITx_HdcpAuthCheckRepeaterReady(void)
{
    MS_U8 regval;

    if(MHal_HDMITx_Rx74ReadByte(0x40, &regval) == TRUE)
    {
        if(regval & BIT5)
            return  E_HDMITX_HDCP_REPEATER_READY; // ready
    }
    return E_HDMITX_HDCP_REPEATER_NOT_READY; // not ready
}


//------------------------------------------------------------------------------
/// @brief This routine is to HDCP check repeater function
/// @return authentication status
//------------------------------------------------------------------------------
MsHDMITX_HDCP_AUTH_STATUS _MDrv_HDMITx_HdcpCheckRepeater(void)
{
    MS_U16 i, j;
    MS_U8 count;
    MS_U8 testbyte[635], bstatus[2];
    SHA1_DATA   test_dig;
    MS_U8 check_error = 0;
    MS_U16 reg_value;
    MS_U8 tempV[20];
    MS_U32 u32DDCDelay = 0;
#ifdef VANCLEEF_MCP
    MS_U8 ksvlist[80];
    MS_U8 u8Depth = 0;
#endif

    MHal_HDMITx_HdcpSha1Init(&test_dig);

    DBG_HDCP(printf("Repeater is ready!!!\r\n"));

    // read ksv list
#if 0
    if(MHal_HDMITx_Rx74ReadByte(0x41, &count) == FALSE)
    {
        DBG_HDCP(printf("HDCP check repeater failed because I2C\r\n"));
        return E_HDMITX_HDCP_REPEATER_SHA1_FAIL;
    }
    count &= 0x7F;
#else
    if(MHal_HDMITx_Rx74ReadBytes(0x41, 2, bstatus) == FALSE)
    {
        DBG_HDCP(printf("HDCP check repeater failed because I2C\r\n"));
        return E_HDMITX_HDCP_REPEATER_SHA1_FAIL;
    }

#ifdef VANCLEEF_MCP
    //Vancleef 5.4, write Bstatus list to MST3000C
    MS_U8 u8VCbstatus[2] = {0x00, 0x00};
#endif
    if((bstatus[0] & 0x80) || (bstatus[1] & 0x08)) // "MAX_DECS_EXCEEDED" or "MAX_CASCADE_EXCEEDED"
    {
#ifdef VANCLEEF_MCP
        if (bstatus[0] & 0x80)
        {
            u8VCbstatus[0] | BIT7;
        }


        if (bstatus[1] & 0x08)
        {
            u8VCbstatus[1] | BIT3;
        }

        MDrv_HDMITx_Vancleef_RxLoadBStatus(u8VCbstatus);
#endif
        return E_HDMITX_HDCP_REPEATER_SHA1_FAIL;
    }
    count = bstatus[0];
#ifdef VANCLEEF_MCP
    //Vancleef 5.4, write Bstatus to MST3000C
    u8Depth = bstatus[1] & 0x07;

    if((count) <= 16)
    {
       u8VCbstatus[0] = ( (count) & 0x7F);
    }
    else
    {
       u8VCbstatus[0] | BIT7;
    }

    if((u8Depth) < 7)
    {
        u8VCbstatus[1] = ( (u8Depth) & 0xF) | BIT4;
    }
    else
    {

        u8VCbstatus[1] | BIT3;

    }

    MDrv_HDMITx_Vancleef_RxLoadBStatus(u8VCbstatus);
#endif //VANCLEEF_MCP
#endif

    if(count)
    {
        if (count > 16)
        {
            //Store original delay count
            u32DDCDelay = MHal_HDMITx_GetDDCDelayCount();
            MHal_HDMITx_AdjustDDCFreq(100); //speed-up to 100KHz
        }

        if(MHal_HDMITx_Rx74ReadBytes(0x43, 5*count, (MS_U8 *)testbyte) == FALSE)
        {
            DBG_HDCP(printf("HDCP check repeater failed because I2C\r\n"));
            return E_HDMITX_HDCP_REPEATER_SHA1_FAIL;
        }
#ifdef VANCLEEF_MCP
        //Vancleef 5.5, write KSV list to MST3000C
        memcpy(ksvlist, testbyte, 5*count);
#endif
        if (count > 16)
        {
            //Set original delay count
            MHal_HDMITx_SetDDCDelayCount(u32DDCDelay);
        }
    }
    DBG_HDCP(printf("KSV List:(=%d) ", count));
    if(count)
    {
        for (j = 0; j < 5*count; j++)
            DBG_HDCP(printf("0x%x, ", testbyte[j]));
        DBG_HDCP(printf("\r\n"));
        MHal_HDMITx_HdcpSha1AddData(&test_dig, testbyte, 5*count);
    }
    // read Bstatus (2 bytes)
    if(MHal_HDMITx_Rx74ReadBytes(0x41, 2, (MS_U8 *) testbyte) == FALSE)
    {
        DBG_HDCP(printf("HDCP check repeater failed because I2C\r\n"));
        return E_HDMITX_HDCP_REPEATER_SHA1_FAIL;
    }
    MHal_HDMITx_HdcpSha1AddData(&test_dig, testbyte, 2);

    // read M0 (8 byte)
    for (i = 0; i < 4; i++)
    {
        reg_value = MHal_HDMITX_GetM02Bytes(i);
        testbyte[2*i] = (MS_U8)(reg_value & 0x00FF);
        testbyte[2*i+1] = (MS_U8)((reg_value & 0xFF00) >> 8);
       // DBG_HDCP(printf(DBG_HDCP, "0x%x, ", testbyte[i]);
    }

    MHal_HDMITx_HdcpSha1AddData(&test_dig, testbyte, 8);
    MHal_HDMITx_HdcpSha1FinalDigest(&test_dig, testbyte);

    //print out SHA1 encode result
    DBG_HDCP(printf("SHA1 encode result(V)=: "));
    for (i = 0; i < 20; i++)
        DBG_HDCP(printf("0x%x, ", testbyte[i]));
    DBG_HDCP(printf("\r\n"));

    DBG_HDCP(printf("V'=: "));

    /*
     * [HDCP] 1A-01, 1B-01a: DUT kept HDCP encryption even though it did not read V' completely
     */
    if( MHal_HDMITx_Rx74ReadBytes(0x20, 20, tempV) == FALSE)
    {
        DBG_HDCP(printf("HDCP check repeater failed because I2C\r\n"));
        return E_HDMITX_HDCP_REPEATER_SHA1_FAIL;
    }

    for (i = 0; i < 20; i++)
    {
        DBG_HDCP(printf("0x%x, ", tempV[i]));
        if (tempV[i] != testbyte[i])
            check_error = 1;
    }

    DBG_HDCP(printf("\r\n"));

#ifdef VANCLEEF_MCP
    //Vancleef
    if (check_error == 0)
    {
        //Vancleef 5.5, write KSV list to MST3000C
        MDrv_HDMITx_Vancleef_RxWriteKSVFIFO(ksvlist, 5*count);

        //Vancleef 5.6, write V' list to MST3000C
        MDrv_HDMITx_Vancleef_RxWriteV(testbyte);

        //Vancleef 5.7, write "Ready Bit" list to MST3000C
        MDrv_HDMITx_Vancleef_RxWriteRepeaterReady(TRUE);

    }
#endif
    return ((check_error == 0) ? E_HDMITX_HDCP_REPEATER_SHA1_PASS: E_HDMITX_HDCP_REPEATER_SHA1_FAIL);
}

//------------------------------------------------------------------------------
/// @brief This routine is to HDCP check Ri function
/// @return authentication status
//------------------------------------------------------------------------------
MsHDMITX_HDCP_AUTH_STATUS _MDrv_HDMITX_HdcpAuthCheckRi(void)
{
    MS_U16 ptx_ri = 0, prx_ri = 0; // Fix coverity impact.

    /*
     * [HDCP] 1A and 1B: AKSV -> Ro should be large than 100msec
     * to pass Quantumdata 882 HDCP test, we delay 150ms
     *
     * 2013/11/07, in SEC Compatibility test, we meet an Repeater timeout error on PEPEATER YAMAHA RX-V2700
     * patch AKSV -> Ro large than 250ms
     */
    while(MsOS_Timer_DiffTimeFromNow(_u32PreAksvWroteTime) < _u32Aksv2RoInterval);

    if (gHDMITxInfo.hdmitx_fsm_state <= E_HDMITX_FSM_HDCP_AUTH_WAIT_RX)
        return E_HDMITX_HDCP_SYNC_RI_FAIL;

    if (MHal_HDMITx_HdcpCheckRi(&ptx_ri, &prx_ri) == TRUE)
    {
        //DBG_HDCP(printf("\nKey match: tx_ri= 0x%x, rx_ri= 0x%x\n", ptx_ri, prx_ri));
        return E_HDMITX_HDCP_SYNC_RI_PASS;
    }
    else
    {
        DBG_HDCP(printf("Tx_Ri=%x, Rx_Ri=%x\n", ptx_ri, prx_ri));
        return E_HDMITX_HDCP_SYNC_RI_FAIL;
    }
}


//-------------------------------------------------------------------------------------------------
// Check Rx timer callback
// @param  stTimer \b IN: Useless
// @param  u32Data \b IN: Useless
// @return None
//-------------------------------------------------------------------------------------------------
#if 0
static void _MDrv_HDMITX_CheckRx_TimerCallback(MS_U32 stTimer, MS_U32 u32Data)
{
    MsHDMITX_RX_STATUS rx_curstatus;

    // Check HDMI receiver status
    rx_curstatus = MHal_HDMITx_GetRXStatus();
    if(gHDMITxInfo.hdmitx_preRX_status != rx_curstatus)
    {
        DBG_HDMITX(printf("E_HDMITX_FSM_CHECK_HPD:: pre= %d, cur = %d\n", gHDMITxInfo.hdmitx_preRX_status, rx_curstatus));
        if(rx_curstatus == E_HDMITX_DVIClock_H_HPD_H)
        {
            // update EDID when HPD L -> H or DVI clock L -> H with HPD always high
            if(gHDMITxInfo.hdmitx_preRX_status != E_HDMITX_DVIClock_H_HPD_H)
                bCheckEDID = TRUE;
            else
                bCheckEDID = FALSE;
            gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_VALIDATE_EDID;
            gHDMITxInfo.hdmitx_tmds_flag = TRUE;
#ifdef CUSTOMER_NDS
            //gHDMITxInfo.hdmitx_tmds_flag = FALSE;
            //MDrv_HDMITx_SetTMDSOnOff();
            gHDMITxInfo.events |= HDMI_EVENT_HOTPLUGGED;
            MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
#endif
        }
        else
        {
            bCheckEDID = TRUE;
            gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_PENDING;
            gHDMITxInfo.hdmitx_tmds_flag = FALSE;
            gHDMITxInfo.hdmitx_edid_ready = FALSE;
#ifdef CUSTOMER_NDS
            if(rx_curstatus == E_HDMITX_DVIClock_L_HPD_L)
                gHDMITxInfo.events |= HDMI_EVENT_HOTUNPLUG;
            MDrv_HDMITx_HdcpSetEncrypt(FALSE);
#endif
            MDrv_HDMITx_SetTMDSOnOff();
        }
        gHDMITxInfo.hdmitx_preRX_status = rx_curstatus;
        gHDMITxInfo.hdmitx_HdcpStartAuth_flag = FALSE;
        gHDMITxInfo.hdmitx_HdcpAuthDone_flag = FALSE;
        //MDrv_HDMITx_SetTMDSOnOff();

        if(_s32HDCPRiTimerId > 0)
            MsOS_StopTimer(_s32HDCPRiTimerId);
#ifndef CUSTOMER_NDS
        MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
#endif // CUSTOMER_NDS
    }
}

#else

static void _MDrv_HDMITX_CheckRx_TimerCallback(MS_U32 stTimer, MS_U32 u32Data)
{
    MsHDMITX_RX_STATUS rx_curstatus;

    // Check HDMI receiver status
    rx_curstatus = MHal_HDMITx_GetRXStatus();
    if(gHDMITxInfo.hdmitx_preRX_status != rx_curstatus)
    {
    	MHal_HDMITx_HdcpSetEncrypt(DISABLE);
        if(_s32HDCPRiTimerId > 0)
            MsOS_StopTimer(_s32HDCPRiTimerId);
        MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RXTIMER);
    }
}

#endif

//-------------------------------------------------------------------------------------------------
// HDCP Ri timer callback
// @param  stTimer \b IN: Useless
// @param  u32Data \b IN: Useless
// @return None
//-------------------------------------------------------------------------------------------------
#if 0
static void _MDrv_HDMITX_HDCP_Ri_TimerCallback(MS_U32 stTimer, MS_U32 u32Data)
{
    if(gHDMITxInfo.hdmitx_tmds_flag)
    {
        if (_MDrv_HDMITX_HdcpAuthCheckRi() == E_HDMITX_HDCP_SYNC_RI_FAIL)
        {
            gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_WAIT_RX;
            gHDMITxInfo.hdmitx_HDCPAuth_Status = E_HDMITX_HDCP_WAITING_ACTIVE_RX;
            gHDMITxInfo.hdmitx_HdcpStartAuth_flag = TRUE;
            gHDMITxInfo.hdmitx_HdcpAuthDone_flag = FALSE;
            MsOS_StopTimer(_s32HDCPRiTimerId);
        #ifdef CUSTOMER_NDS
            gHDMITxInfo.events |= HDMI_EVENT_CP_LINK_INTEGRITY_FAILED;
        #else
            MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
        #endif // CUSTOMER_NDS
        }
        //DBG_HDMITX(printf("\n_MDrv_HDMITX_HDCP_Ri_TimerCallback(): Time tick= %d ms\n", MsOS_GetSystemTime()));
    }
}

#else

static void _MDrv_HDMITX_HDCP_Ri_TimerCallback(MS_U32 stTimer, MS_U32 u32Data)
{
    if(gHDMITxInfo.hdmitx_tmds_flag)
    {
        MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RITIMER);
        //DBG_HDMITX(printf("\n_MDrv_HDMITX_HDCP_Ri_TimerCallback(): Time tick= %d ms\n", MsOS_GetSystemTime()));
    }
}
#endif

//------------------------------------------------------------------------------
/// @brief This routine set HDMI Tx initial situation.
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_InitVariable(void)
{
    gHDMITxInfo.hdmitx_enable_flag = FALSE;
    gHDMITxInfo.hdmitx_tmds_flag = TRUE;
    gHDMITxInfo.hdmitx_video_flag = TRUE;
    gHDMITxInfo.hdmitx_audio_flag = TRUE;
    gHDMITxInfo.hdmitx_hdcp_flag = FALSE;
    gHDMITxInfo.hdmitx_csc_flag = FALSE;

    gHDMITxInfo.hdmitx_HdcpUseInternalKey_flag = TRUE;
    gHDMITxInfo.hdmitx_HdcpStartAuth_flag = FALSE;
    gHDMITxInfo.hdmitx_HdcpAuthDone_flag = FALSE;
    gHDMITxInfo.hdmitx_HdcpCheckRepeater_flag = FALSE;
    gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_PENDING;
    gHDMITxInfo.hdmitx_fsm_prestate = E_HDMITX_FSM_PENDING;
    gHDMITxInfo.hdmitx_preRX_status = E_HDMITX_DVIClock_L_HPD_L;
    gHDMITxInfo.hdmitx_HDCPAuth_Status = E_HDMITX_HDCP_RESET;
    gHDMITxInfo.hdmitx_unHDCPRx_Control = E_NORMAL_OUTPUT;
    gHDMITxInfo.hdmitx_HDCPRxFail_Control = E_RXFail_NORMAL_OUTPUT;
    gHDMITxInfo.hdmitx_audio_supportAI = FALSE;
    gHDMITxInfo.hdmitx_RB_swap_flag = FALSE;
    gHDMITxInfo.hdmitx_force_mode = FALSE;
    gHDMITxInfo.hdmitx_force_output_color = FALSE;
    gHDMITxInfo.hdmitx_AFD_override_mode = FALSE;
    gHDMITxInfo.hdmitx_edid_ready = FALSE;
    gHDMITxInfo.hdmitx_avmute_flag = FALSE;
    //gHDMITxInfo.hdmitx_CECEnable_flag = FALSE; // CEC

    gHDMITxInfo.output_mode = E_HDMITX_HDMI;
    gHDMITxInfo.force_output_mode = E_HDMITX_HDMI	;
    gHDMITxInfo.output_colordepth_val = E_HDMITX_VIDEO_CD_NoID;//E_HDMITX_VIDEO_CD_24Bits;
    gHDMITxInfo.edid_colordepth_val = E_HDMITX_VIDEO_CD_NoID;//E_HDMITX_VIDEO_CD_24Bits;
    gHDMITxInfo.output_video_prevtiming = E_HDMITX_RES_MAX;
    gHDMITxInfo.output_video_timing = E_HDMITX_RES_720x480p;
    gHDMITxInfo.force_output_color = E_HDMITX_VIDEO_COLOR_RGB444;
    gHDMITxInfo.input_color = E_HDMITX_VIDEO_COLOR_RGB444;
    gHDMITxInfo.output_color = E_HDMITX_VIDEO_COLOR_RGB444;
    gHDMITxInfo.output_aspect_ratio = E_HDMITX_VIDEO_AR_4_3;
    gHDMITxInfo.output_scan_info = E_HDMITX_VIDEO_SI_NoData;
    gHDMITxInfo.output_afd_ratio = E_HDMITX_VIDEO_AFD_SameAsPictureAR;
    gHDMITxInfo.output_activeformat_present = 1;
    gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_48K;
    gHDMITxInfo.vs_pkt_format = E_HDMITX_VIDEO_VS_No_Addition;
    gHDMITxInfo.vs_pkt_3d = E_HDMITx_VIDEO_3D_Not_in_Use;
    gHDMITxInfo.vs_4k2k_vic = E_HDMITx_VIDEO_4k2k_Reserved;
    gHDMITxInfo.edid_2D_50hz_support = FALSE;
    gHDMITxInfo.edid_2D_60hz_support = FALSE;
    gHDMITxInfo.edid_3D_50hz_support = FALSE;
    gHDMITxInfo.edid_3D_60hz_support = FALSE;
    gHDMITxInfo.edid_3D_present = FALSE;
    gHDMITxInfo.edid_HDMI_support = FALSE;

#ifdef OBERON
    gHDMITxInfo.output_audio_channel = E_HDMITX_AUDIO_CH_8;
#else // Uranus, Uranus4
    gHDMITxInfo.output_audio_channel = E_HDMITX_AUDIO_CH_2;
#endif
    gHDMITxInfo.output_audio_type = E_HDMITX_AUDIO_PCM;
    memset(&gHDMITxInfo.edid_phyadr[0], 0, 2*sizeof(MS_U8));
    memset(&gHDMITxInfo.id_manufacturer_name[0], 0, 3*sizeof(MS_U8));
    memset(&gHDMITxInfo.edid_block0, 0, 128*sizeof(MS_U8));
    memset(&gHDMITxInfo.edid_block1, 0, 128*sizeof(MS_U8));
    memset(&gHDMITxInfo.hdcp_srmlist[0], 0, 5116*sizeof(MS_U8));
    memset(&gHDMITxInfo.hdcp_revocationlist[0], 0, 5068*sizeof(MS_U8));
    memset(&gHDMITxInfo.short_video_descriptor[0], 0, 32*sizeof(MS_U8));
    memset(&gHDMITxInfo.short_audio_descriptor[0], 0, 32*sizeof(MS_U8));
    memset(&gHDMITxInfo.data_block_length[0], 0, 8*sizeof(MS_U8));
    memset(&gHDMITxInfo.edid_3D_support_timing[0], 0, 16*sizeof(MsHDMITX_EDID_3D_SUPPORT_TIMING));
    memset(&gHDMITxInfo.HDCP_AKSV[0], 0, 5*sizeof(MS_U8));
    memset(&gHDMITxInfo.HDCP_BKSV[0], 0, 5*sizeof(MS_U8));
    gHDMITxInfo.revocationlist_ready = FALSE;
    gHDMITxInfo.revocation_size = 0;
    gHDMITxInfo.revocation_state = E_CHECK_NOT_READY;
    gHDMITxInfo.HDCP_74_check = FALSE;
#ifdef CUSTOMER_NDS
    gHDMITxInfo.events = 0;
    gHDMITxInfo.hdcp_encryptionStartTime = 0;
    gHDMITxInfo.hdcp_checkPjIntegrity = FALSE;
#endif

    // HDMI Tx Pre-emphasis and Double termination
    memset(&gHDMITxInfo.analog_setting, 0, sizeof(gHDMITxInfo.analog_setting));

    // Set DDC speed to 50KHz as default
    MHal_HDMITx_AdjustDDCFreq(50);

    // HDCP init
    MHal_HDMITx_HdcpDebugEnable(_u16DbgHDMITXSwitch & HDMITX_DBG_HDCP ? TRUE : FALSE);
    if (gHDMITxInfo.hdmitx_HdcpUseInternalKey_flag)
        MHal_HDMITx_HdcpKeyInit(USE_INTERNAL_HDCP_KEY);
}

// HDMI Tx output is DVI / HDMI mode
//------------------------------------------------------------------------------
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_InitSeq(void)
{
    MHal_HDMITx_InitSeq();
    MDrv_HDMITx_InitVariable();
}


// This routine check and set the related format by EDID
MS_BOOL MDrv_HDMITx_EdidChecking(void)
{
    MS_U8 u8EDIDRetryCount = 3;

#ifdef MSOS_TYPE_LINUX_KERNEL
#undef current
#define current hdmitx_current
#endif

    MS_U8 i, j, l, m, value, current, offset, k, block_num, temp_offset;
    MS_U8 edid_3d_multi_present, edid_3d_len;
    MS_U8 descriptor, tag, len, block_data[128];
    MS_U16 edid_3d_str_all, edid_3d_mask;

    if(!bCheckEDID) // Check EDID only when HPD from low to high
        return TRUE;

    gHDMITxInfo.hdmitx_audio_supportAI = FALSE;
    gHDMITxInfo.output_color = E_HDMITX_VIDEO_COLOR_RGB444;
    gHDMITxInfo.edid_colordepth_val = E_HDMITX_VIDEO_CD_NoID;
    gHDMITxInfo.edid_2D_50hz_support = FALSE;
    gHDMITxInfo.edid_2D_60hz_support = FALSE;
    gHDMITxInfo.edid_3D_50hz_support = FALSE;
    gHDMITxInfo.edid_3D_60hz_support = FALSE;
    gHDMITxInfo.edid_3D_present = FALSE;
    gHDMITxInfo.edid_HDMI_support = FALSE;
    j = 0;
    m = 0;
    edid_3d_str_all= 0;
    edid_3d_mask = 0;

    memset(gHDMITxInfo.short_video_descriptor, 0, 32*sizeof(MS_U8));
    memset(gHDMITxInfo.short_audio_descriptor, 0, 32*sizeof(MS_U8));
    memset(&gHDMITxInfo.edid_3D_support_timing[0], 0, 16*sizeof(MsHDMITX_EDID_3D_SUPPORT_TIMING));
    memset(block_data, 0 , 128*sizeof(MS_U8));

    #if 0   /*AWU -- To fix LINT ERROR*/
    gHDMITxInfo.output_mode &= ~BIT1; // default is DVI mode
    #else
    gHDMITxInfo.output_mode = (MsHDMITX_OUTPUT_MODE)((MS_U8)(gHDMITxInfo.output_mode)&(~(MS_U8)BIT1)); // default is DVI mode
    #endif

    //*(volatile MS_U16*)(0xBF803C2C) &=~(BIT6|BIT7); // configure DDC bus

    while(MHal_HDMITx_EdidReadBlock(0, block_data) == FALSE)
    {
        u8EDIDRetryCount--;
        if(u8EDIDRetryCount == 0)
        {
            DBG_HDMITX(printf("EdidChecking, HDMI DDC bus no ACK\n"));
            memset(block_data, 0 , 128*sizeof(MS_U8));
            break;
            //return FALSE;
        }
        MsOS_DelayTask(100);
    }

    #if 1   /*Dump EDID*/
    DBG_HDMITX(printf("\nEDID ---> \n"));
    for (i=0; i<128; i++)
    {
        DBG_HDMITX(printf("%02x ",block_data[i]));
        if ((i%16)==15) DBG_HDMITX(printf("\n"));

    }
    #endif

    for(i=0;i<8;i++)
    {
        if(HDMITX_EdidHeaderTbl[i]!=block_data[i])
        {
            DBG_HDMITX(printf("EdidChecking, No EDID\n"));
            //return FALSE;
        }
    }

    memcpy(gHDMITxInfo.edid_block0, &block_data, 128*sizeof(MS_U8));
    gHDMITxInfo.id_manufacturer_name[0] = (block_data[8]&0x7C)>>2;
    gHDMITxInfo.id_manufacturer_name[1] = ((block_data[8]&0x03)<<3)|((block_data[9]&0xE0)>>5);
    gHDMITxInfo.id_manufacturer_name[2] = block_data[9]&0x1F;

    block_num = block_data[0x7E]; // extension blocks
    //block_num = (block_num > 4) ? 0 : block_num; // restrict block number small than 4 to avoid I2C non-ACK condition.
    if(block_num > 4)
    {
        int j;
        MS_BOOL bEDIDValid = TRUE;
        DBG_HDMITX(printf("EdidChecking, EDID block number is greater than 4!!\n"));
        block_num = 0;

        for(i=1;i<=4;i++)
        {
            u8EDIDRetryCount = 3;
            memset(block_data, 0 , 128*sizeof(MS_U8));
            DBG_HDMITX(printf("EdidChecking, trying block %d\n", i));

            while(MHal_HDMITx_EdidReadBlock(i, block_data) == FALSE)
            {
                u8EDIDRetryCount--;
                if(u8EDIDRetryCount == 0)
                {
                    DBG_HDMITX(printf("EdidChecking, HDMI DDC bus no ACK\n"));
                    memset(block_data, 0 , 128*sizeof(MS_U8));
                    break;
                    //return FALSE;
                }
                MsOS_DelayTask(100);
            }

            if(u8EDIDRetryCount == 0)
            {
                bEDIDValid = FALSE;
                continue;
            }

            if(bEDIDValid)
            {
                block_num++;
            }

            DBG_HDMITX(printf("\nEDID ---> blk num=%d\n", i));
            for (j=0; j<128; j++)
            {
                DBG_HDMITX(printf("%02x ",block_data[j]));
                if ((j%16)==15) DBG_HDMITX(printf("\n"));

            }
        }
    }

    DBG_HDMITX(printf("EdidChecking, blk num=%d\n",block_num));
    for(i=1;i<=block_num;i++)
    {
        DBG_HDMITX(printf("EdidChecking, the cur blk idx=%d\n",i));

	 u8EDIDRetryCount = 3;
        while(MHal_HDMITx_EdidReadBlock(i, block_data) == FALSE)
        {
            u8EDIDRetryCount--;
            if(u8EDIDRetryCount == 0)
            {
                DBG_HDMITX(printf("EdidChecking, HDMI DDC bus no ACK\n"));
                memset(block_data, 0 , 128*sizeof(MS_U8));
                break;
                //return FALSE;
            }
            MsOS_DelayTask(100);
        }

        // Support CEA Timing Extension version 3
        if (block_data[0]==0x02 && block_data[1]<=0x04)
        {
            offset = block_data[0x02];
            DBG_HDMITX(printf("EdidChecking, offset:%x\n",offset));
            if (offset == 0xFF)
                continue;
            if (offset < 0x05)
                continue;
            gHDMITxInfo.edid_HDMI_support = TRUE;

            current = 0x04;

            while(current<offset)
            {
                descriptor = block_data[current];
                tag = ((descriptor&0xE0)>>5);
                len = (descriptor&0x1F);
                gHDMITxInfo.data_block_length[tag] = len;
                DBG_HDMITX(printf("EdidChecking, Tag:%x ",tag));
                DBG_HDMITX(printf("Len:%x \n",len));
                switch (tag)
                {
                    case 1:   // Audio Tag
                        current++;
                        DBG_HDMITX(printf("EdidChecking, Short Audio Descriptor: "));
                        for(k=0;k<len;k++)
                        {
                            gHDMITxInfo.short_audio_descriptor[k] = block_data[current++];
                            //DBG_HDMITX(printf("%x ", gHDMITxInfo.short_audio_descriptor[k]));
                        }
                        //current += (len+1);
                    	break;
                    case 2:   // Video Tag
                    	current++;
                        DBG_HDMITX(printf("EdidChecking, Short Video Descriptor: "));
                        for(k=0; k<len;k++)
                        {
                            gHDMITxInfo.short_video_descriptor[k] = block_data[current++];
                            gHDMITxInfo.edid_3D_support_timing[j].support_timing = gHDMITxInfo.short_video_descriptor[k] & 0x7F;
                            j++;

                            if(((gHDMITxInfo.short_video_descriptor[k] & 0x7F)<=E_HDMITX_VIC_1920x1080p_60_16_9) |
                            ((gHDMITxInfo.short_video_descriptor[k] & 0x7F)==E_HDMITX_VIC_2880x480p_60_4_3) |
                            ((gHDMITxInfo.short_video_descriptor[k] & 0x7F)==E_HDMITX_VIC_2880x480p_60_16_9))
                                gHDMITxInfo.edid_2D_60hz_support = TRUE;
                            else if(((gHDMITxInfo.short_video_descriptor[k] & 0x7F)<=E_HDMITX_VIC_1920x1080p_50_16_9) |
                            ((gHDMITxInfo.short_video_descriptor[k] & 0x7F)==E_HDMITX_VIC_2880x576p_50_4_3) |
                            ((gHDMITxInfo.short_video_descriptor[k] & 0x7F)==E_HDMITX_VIC_2880x576p_50_16_9)|
                            ((gHDMITxInfo.short_video_descriptor[k] & 0x7F)==E_HDMITX_VIC_1920x1080i_50_16_9_1250_total))
                                gHDMITxInfo.edid_2D_50hz_support = TRUE;

                            DBG_HDMITX(printf("%x ", gHDMITxInfo.short_video_descriptor[k]));
                        }
                        DBG_HDMITX(printf("Len:%x \n",len));
                        //current += len;
                        break;
                    case 4:   // Speaker Tag
                    case 5:   // VESA DTC data block Tag
                    case 7:   // Extended Tag
                    default:
                        current += (len+1);
                        break;

                    case 3:  // VSDB tag
                        current++;

                        DBG_HDMITX(printf("EdidChecking, VSDB: "));
                        for(k=0;k<3;k++)
                        {
                            value = block_data[current+k];
                            DBG_HDMITX(printf("%x",value));
                            if(HDMITX_IEEEHdmiIdTbl[k]!=value)
                                break;
                        }
                        DBG_HDMITX(printf("This Device contain the IEEE Registration Identifier of 0x000C03, Support HDMI Mode! \n"));

                        // EDID physical address
                        gHDMITxInfo.edid_phyadr[0] = block_data[current+3];
                        gHDMITxInfo.edid_phyadr[1] = block_data[current+4];

                        gHDMITxInfo.edid_colordepth_val = E_HDMITX_VIDEO_CD_24Bits;
                        //if supported HDMI, then read Support_AI and length
                        if((k>=3) && (len > 5))
                        {
                            value = block_data[current+5]; // Read Support_AI
                            DBG_HDMITX(printf("\nSupport_AI=%x\n",value));

                            if (value & BIT7) // check bit7
                                gHDMITxInfo.hdmitx_audio_supportAI = TRUE;
                            // get deep color information
                            if (value & 0x30) // check bit5 and bit4
                            {
                                if ((value & 0x30) == BIT4)
                                {
                                    gHDMITxInfo.edid_colordepth_val = E_HDMITX_VIDEO_CD_30Bits;
                                    DBG_HDMITX(printf("MDrv_HDMITx_EdidChecking:: CD 10 bits\n"));
                                }
                                else
                                {
                                    gHDMITxInfo.edid_colordepth_val = E_HDMITX_VIDEO_CD_36Bits;
                                    DBG_HDMITX(printf("MDrv_HDMITx_EdidChecking:: CD 12 bits\n"));
                                }
                            }
                            if(len > 7)
                            {
                                value = block_data[current+7]; // [5]: HDMI_Video_present
                                if (value & BIT5)
                                {
                                    if(value & 0xC0 )
                                        temp_offset = 4;
                                    else if(value & 0x40)
                                        temp_offset = 2;
                                    else
                                        temp_offset = 0;
                                    if(len > temp_offset + 9 )
                                    {
                                        value = block_data[current+temp_offset+8]; //[7]: 3D_present
                                        if (value & BIT7)
                                        {
                                            gHDMITxInfo.edid_3D_present = TRUE;
                                            if(gHDMITxInfo.edid_2D_50hz_support )
                                                gHDMITxInfo.edid_3D_50hz_support = TRUE;
                                            if(gHDMITxInfo.edid_2D_60hz_support )
                                                gHDMITxInfo.edid_3D_60hz_support = TRUE;
                                            edid_3d_multi_present = (value & 0x60) >> 5; //[6:5]: 3D_Multi_present
                                            value = block_data[current+temp_offset+9]; //[7:5]: HDMI_VIC_LEN, [4:0]: HDMI_3D_LEN
                                            temp_offset += (value & 0xE0) >> 5;
                                            edid_3d_len = value & 0x1F;
                                            //value = block_data[current+temp_offset+10]; //[7:0]: 3d_structure_all
                                            if(edid_3d_multi_present == 0x01)
                                            {
                                                if(len > temp_offset + 11)
                                                {
                                                    edid_3d_str_all = (block_data[current+temp_offset+10]) << 8;
                                                    edid_3d_str_all = edid_3d_str_all + block_data[current+temp_offset+11];

                                                    for(l=0;l<j;l++)
                                                    {
                                                       gHDMITxInfo.edid_3D_support_timing[l].support_3D_structure = edid_3d_str_all;
                                                    }
                                                }
                                                edid_3d_len = edid_3d_len - 2;
                                            }
                                            else if(edid_3d_multi_present == 0x02)
                                            {
                                                if(len > temp_offset + 13)
                                                {
                                                    edid_3d_str_all = (block_data[current+temp_offset+10]) << 8;
                                                    edid_3d_str_all = edid_3d_str_all + block_data[current+temp_offset+11];
                                                    edid_3d_mask = (block_data[current+temp_offset+12]) << 8;
                                                    edid_3d_mask = edid_3d_mask + block_data[current+temp_offset+13];

                                                    for(l=0;l<j;l++)
                                                    {
                                                        if(edid_3d_mask & (1<<l))
                                                            gHDMITxInfo.edid_3D_support_timing[l].support_3D_structure = edid_3d_str_all;
                                                        else
                                                             gHDMITxInfo.edid_3D_support_timing[l].support_3D_structure = 0;
                                                    }
                                                }
                                                edid_3d_len = edid_3d_len - 4;
                                            }

                                            if(edid_3d_len>1)
                                            {
                                                if(edid_3d_multi_present == 0x01)
                                                {
                                                    if(len > temp_offset + edid_3d_len + 11)
                                                    {
                                                        for(m=0;m<edid_3d_len;m++)
                                                        {
                                                            switch(block_data[current+temp_offset+12+m]&0x0F)
                                                            {
                                                                case 0:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+12+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_FramePacking;
                                                                    break;
                                                                case 1:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+12+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_FieldAlternative;
                                                                    break;
                                                                case 2:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+12+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_LineAlternative;
                                                                    break;
                                                                case 3:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+12+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_SidebySide_FULL;
                                                                    break;
                                                                case 4:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+12+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_L_Dep;
                                                                    break;
                                                                case 5:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+12+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_L_Dep_Graphic_Dep;
                                                                    break;
                                                                case 6:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+12+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_TopandBottom;
                                                                    break;
                                                                case 7:
                                                                    break;
                                                                case 8:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+12+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_SidebySide_Half_horizontal ;
                                                                    m++;
                                                                    break;
                                                                default:
                                                                    m++;
                                                                    break;
                                                            }
                                                        }
                                                    }
                                                }
                                                else if(edid_3d_multi_present == 0x02)
                                                {
                                                    if(len > temp_offset + edid_3d_len + 13)
                                                    {
                                                        for(m=0;m<edid_3d_len;m++)
                                                        {
                                                            switch(block_data[current+temp_offset+14+m]&0x0F)
                                                            {
                                                                case 0:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+14+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_FramePacking;
                                                                    break;
                                                                case 1:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+14+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_FieldAlternative;
                                                                    break;
                                                                case 2:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+14+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_LineAlternative;
                                                                    break;
                                                                case 3:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+14+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_SidebySide_FULL;
                                                                    break;
                                                                case 4:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+14+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_L_Dep;
                                                                    break;
                                                                case 5:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+14+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_L_Dep_Graphic_Dep;
                                                                    break;
                                                                case 6:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+14+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_TopandBottom;
                                                                    break;
                                                                case 7:
                                                                    break;
                                                                case 8:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+14+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_SidebySide_Half_horizontal ;
                                                                    m++;
                                                                    break;
                                                                default:
                                                                    m++;
                                                                    break;
                                                            }
                                                    }
                                                }
                                                else
                                                {
                                                    if(len > temp_offset + edid_3d_len + 9)
                                                    {
                                                        for(m=0;m<edid_3d_len;m++)
                                                        {
                                                            switch(block_data[current+temp_offset+10+m]&0x0F)
                                                            {
                                                                case 0:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+10+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_FramePacking;
                                                                    break;
                                                                case 1:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+10+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_FieldAlternative;
                                                                    break;
                                                                case 2:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+10+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_LineAlternative;
                                                                    break;
                                                                case 3:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+10+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_SidebySide_FULL;
                                                                    break;
                                                                case 4:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+10+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_L_Dep;
                                                                    break;
                                                                case 5:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+10+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_L_Dep_Graphic_Dep;
                                                                    break;
                                                                case 6:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+10+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_TopandBottom;
                                                                    break;
                                                                case 7:
                                                                    break;
                                                                case 8:
                                                                    gHDMITxInfo.edid_3D_support_timing[((block_data[current+temp_offset+10+m]&0xF0)>>4)].support_3D_structure |= E_HDMITX_EDID_3D_SidebySide_Half_horizontal ;
                                                                    m++;
                                                                    break;
                                                                default:
                                                                    m++;
                                                                    break;
                                                            }
                                                        }
                                                    }
                                                }
                                                //2D_VIC_ORDER and 3D_Structure_1;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        if(k>=3)
                        {
                            #if 0   /*AWU -- To fix LINT ERROR*/
                            gHDMITxInfo.output_mode |= BIT1; // set to HDMI mode
                            #else
                            gHDMITxInfo.output_mode = (MsHDMITX_OUTPUT_MODE)((MS_U8)(gHDMITxInfo.output_mode)|((MS_U8)BIT1)); // default is DVI mode
                            #endif

                            switch(block_data[0x03] & 0x30)
                            {
                                case 0x00:
        		               gHDMITxInfo.output_color = E_HDMITX_VIDEO_COLOR_RGB444;
                                    DBG_HDMITX(printf("Rx Not Support YCbCr\n"));
        		               break;
				    #if 1 // Uranus only support YUV -> RGB or RGB/YUV bypass
                                case 0x30:
                                case 0x20:
			               if(gHDMITxInfo.input_color == E_HDMITX_VIDEO_COLOR_YUV444)
			               {
        		                   gHDMITxInfo.output_color = E_HDMITX_VIDEO_COLOR_YUV444;
                                        DBG_HDMITX(printf("Set OutputVideoFormat = TX_OUTPUT_YUV444\n"));
			               }
                                    break;
                                case 0x10:
			               if(gHDMITxInfo.input_color == E_HDMITX_VIDEO_COLOR_YUV444)
			               {
        		                   gHDMITxInfo.output_color = E_HDMITX_VIDEO_COLOR_YUV444;
                                        DBG_HDMITX(printf("Set OutputVideoFormat = TX_OUTPUT_YUV422\n"));
			               }
                                    break;
                                #endif
                            }
                        }
                        current += len;
                        break;
                }
            }
        }
    }

    memcpy(gHDMITxInfo.edid_block1, &block_data, 128*sizeof(MS_U8));

    if(gHDMITxInfo.hdmitx_force_mode) // AP force output mode
    {
        gHDMITxInfo.output_mode = gHDMITxInfo.force_output_mode;
        if((gHDMITxInfo.force_output_mode == E_HDMITX_DVI) || (gHDMITxInfo.force_output_mode == E_HDMITX_DVI_HDCP)) // DVI mode
        {
            gHDMITxInfo.hdmitx_audio_supportAI = FALSE;
            gHDMITxInfo.output_color = E_HDMITX_VIDEO_COLOR_RGB444;
        }
    }

    // AP force output color format
    if((gHDMITxInfo.output_mode == E_HDMITX_HDMI || gHDMITxInfo.output_mode == E_HDMITX_HDMI_HDCP) && (gHDMITxInfo.hdmitx_force_output_color))
    {
        gHDMITxInfo.output_color = gHDMITxInfo.force_output_color;
    }

    // YUV444 -> RGB444
    if( (gHDMITxInfo.input_color != gHDMITxInfo.output_color) && (gHDMITxInfo.output_color == E_HDMITX_VIDEO_COLOR_RGB444) )
        gHDMITxInfo.hdmitx_csc_flag = TRUE;
    else // bypass
        gHDMITxInfo.hdmitx_csc_flag = FALSE;

    gHDMITxInfo.hdmitx_edid_ready = TRUE;

    return TRUE;
#ifdef MSOS_TYPE_LINUX_KERNEL
#undef hdmitx_current
#endif
}

MS_BOOL MDrv_HDMITx_RxBypass_Mode(MsHDMITX_INPUT_FREQ freq, MS_BOOL bflag)
{
    gHDMITxInfo.hdmitx_HdcpStartAuth_flag = FALSE;
    gHDMITxInfo.hdmitx_HdcpAuthDone_flag = FALSE;
#if HDMITX_ISR_ENABLE
    MHal_HDMITx_Int_Disable(gHDMITXIRQ);
    MHal_HDMITx_Int_Clear(gHDMITXIRQ);
#endif
    //MDrv_HDMITx_SetTMDSOnOff();
    //MDrv_HDMITx_PLLOnOff();
    gHDMITxInfo.hdmitx_fsm_prestate = E_HDMITX_FSM_PENDING;
    gHDMITxInfo.hdmitx_preRX_status = E_HDMITX_DVIClock_L_HPD_L;
    if(_s32CheckRxTimerId > 0)
        MsOS_StopTimer(_s32CheckRxTimerId);
    if(_s32HDCPRiTimerId > 0)
        MsOS_StopTimer(_s32HDCPRiTimerId);

    return MHal_HDMITx_RxBypass_Mode(freq, bflag);
}

MS_BOOL MDrv_HDMITx_Disable_RxBypass(void)
{
#if HDMITX_ISR_ENABLE
    MHal_HDMITx_Int_Enable(gHDMITXIRQ);
#endif
    MHal_HDMITx_VideoInit();
    MHal_HDMITx_AudioInit();
    //MDrv_HDMITx_SetTMDSOnOff();
    MDrv_HDMITx_Exhibit();
    return MHal_HDMITx_Disable_RxBypass();
}

/*
 * [HDCP] 1A and 1B: AKSV -> Ro should be large than 100msec
 * to pass Quantumdata 882 HDCP test, we delay 150ms
 *
 * 2013/11/07, in SEC Compatibility test, we meet an Repeater timeout error on PEPEATER YAMAHA RX-V2700
 * patch AKSV -> Ro large than 250ms
 */
void MDrv_HDMITx_SetAksv2R0Interval(MS_U32 u32Interval)
{
    _u32Aksv2RoInterval = u32Interval;
}

void MDrv_HDMITx_Exhibit(void)
{
    if(gHDMITxInfo.output_mode & BIT0) // check HDCP
        gHDMITxInfo.hdmitx_hdcp_flag = TRUE;
    else
        gHDMITxInfo.hdmitx_hdcp_flag = FALSE;

    // The 1st HDCP authentication
    if(!gHDMITxInfo.hdmitx_HdcpAuthDone_flag)
    {
        gHDMITxInfo.output_video_prevtiming = gHDMITxInfo.output_video_timing;
        gHDMITxInfo.hdmitx_preRX_status = E_HDMITX_DVIClock_L_HPD_L;
        // Create timer for timing monitor
        if(_s32CheckRxTimerId < 0)
        {
            _s32CheckRxTimerId = MsOS_CreateTimer( _MDrv_HDMITX_CheckRx_TimerCallback,
                                                  CheckRx_Timer,
                                                  CheckRx_Timer,
                                                  TRUE,
                                                  "Check Rx timer");
            if(_s32CheckRxTimerId < 0)
            {
                DBG_HDMITX(printf("MDrv_HDMITX_Init: Create Check Rx timer fail!\n"));
            }
            DBG_HDMITX(printf("MDrv_HDMITx_Exhibit: Create Check Rx timer success!\n"));
        }
        else
        {
            #ifdef CUSTOMER_NDS
            /*
             * Stop-then-start the timer. Starting a running timer causes
             * a kernel bug, to prevent from this case, we stop the timer,
             * then start it again.
             */
            MsOS_StopTimer(_s32CheckRxTimerId);
            #endif
            MsOS_StartTimer(_s32CheckRxTimerId);
        }
        return;
    }

    // HDCP re-authentication while timing changed or input source changed
    //else if((gHDMITxInfo.output_video_prevtiming != gHDMITxInfo.output_video_timing) && gHDMITxInfo.hdmitx_hdcp_flag)
    else if(gHDMITxInfo.hdmitx_hdcp_flag)
    {
        gHDMITxInfo.hdmitx_HdcpStartAuth_flag = TRUE;
        gHDMITxInfo.hdmitx_HdcpAuthDone_flag = FALSE;
        gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_WAIT_RX;
        gHDMITxInfo.output_video_prevtiming = gHDMITxInfo.output_video_timing;

        if(_s32HDCPRiTimerId > 0)
            MsOS_StopTimer(_s32HDCPRiTimerId);

    #ifndef CUSTOMER_NDS
	 MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
    #endif //CUSTOMER_NDS
	 DBG_HDMITX(printf("MDrv_HDMITx_Exhibit: HDCP reauthentication!!\n"));
    }
    else // turn off HDCP encryption
    {
        if(_s32HDCPRiTimerId > 0)
            MsOS_StopTimer(_s32HDCPRiTimerId);

        MHal_HDMITx_HdcpSetEncrypt(DISABLE);
        DBG_HDMITX(printf("MDrv_HDMITx_Exhibit: Turn off HDCP!!\n"));
    #if 0 // Fix garbage while turn HDCP ON -> OFF
    #ifndef CUSTOMER_NDS
        if(gHDMITxInfo.hdmitx_avmute_flag)
        {
            MDrv_HDMITx_SetAVMUTE(FALSE);
            gHDMITxInfo.hdmitx_avmute_flag = FALSE;
        }
    #endif
    #endif
    }

    MDrv_HDMITx_SetHDMITxMode();
    MDrv_HDMITx_SetColorFormat();
    MDrv_HDMITx_SetVideoOutputMode();
    MDrv_HDMITx_SetVideoOnOff();
    MDrv_HDMITx_SetAudioFrequency();
    MDrv_HDMITx_SetAudioOnOff();
    //MDrv_HDMITx_SetHDCPOnOff();
    //MDrv_HDMITx_SetTMDSOnOff();

    // Fix garbage while turn HDCP ON -> OFF
#ifndef CUSTOMER_NDS
    if( gHDMITxInfo.hdmitx_HdcpAuthDone_flag && (!gHDMITxInfo.hdmitx_hdcp_flag) )
    {
        if(gHDMITxInfo.hdmitx_avmute_flag)
        {
            MDrv_HDMITx_SetAVMUTE(FALSE);
            gHDMITxInfo.hdmitx_avmute_flag = FALSE;
        }
    }
#endif // #ifndef CUSTOMER_NDS
}

void MDrv_HDMITX_Process(MS_U32 u32State)
{
    MS_U32 irq_status;
    MsHDMITX_HDCP_AUTH_STATUS ret;

    if(u32State & E_HDMITX_EVENT_RXTIMER)
    {
        MsHDMITX_RX_STATUS rx_curstatus;

        // Check HDMI receiver status
        rx_curstatus = MHal_HDMITx_GetRXStatus();
        if(gHDMITxInfo.hdmitx_preRX_status != rx_curstatus)
        {
            DBG_HDMITX(printf("E_HDMITX_FSM_CHECK_HPD:: pre= %d, cur = %d\n", gHDMITxInfo.hdmitx_preRX_status, rx_curstatus));
            if(rx_curstatus == E_HDMITX_DVIClock_H_HPD_H)
            {
                // update EDID when HPD L -> H or DVI clock L -> H with HPD always high
                if(gHDMITxInfo.hdmitx_preRX_status != E_HDMITX_DVIClock_H_HPD_H)
                    bCheckEDID = TRUE;
                else
                    bCheckEDID = FALSE;
                gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_VALIDATE_EDID;
                gHDMITxInfo.hdmitx_tmds_flag = TRUE;
    #ifdef CUSTOMER_NDS
                //gHDMITxInfo.hdmitx_tmds_flag = FALSE;
                //MDrv_HDMITx_SetTMDSOnOff();
                gHDMITxInfo.events |= HDMI_EVENT_HOTPLUGGED;
                MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
    #endif
            }
            else
            {
                bCheckEDID = TRUE;
                gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_PENDING;
                gHDMITxInfo.hdmitx_tmds_flag = FALSE;
                gHDMITxInfo.hdmitx_edid_ready = FALSE;
    #ifdef CUSTOMER_NDS
                if(rx_curstatus == E_HDMITX_DVIClock_L_HPD_L)
                    gHDMITxInfo.events |= HDMI_EVENT_HOTUNPLUG;
                //MDrv_HDMITx_HdcpSetEncrypt(DISABLE);
    #endif
	            MHal_HDMITx_HdcpSetEncrypt(DISABLE); // HDCP test
                MDrv_HDMITx_SetTMDSOnOff();
            }
            gHDMITxInfo.hdmitx_preRX_status = rx_curstatus;
            gHDMITxInfo.hdmitx_HdcpStartAuth_flag = FALSE;
            gHDMITxInfo.hdmitx_HdcpAuthDone_flag = FALSE;
            gHDMITxInfo.hdmitx_HDCPAuth_Status = E_HDMITX_HDCP_RESET;
            //MDrv_HDMITx_SetTMDSOnOff();

            if(_s32HDCPRiTimerId > 0)
                MsOS_StopTimer(_s32HDCPRiTimerId);

    #ifndef CUSTOMER_NDS
            MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
    #endif // CUSTOMER_NDS
        }
    // To avoid two events are coming together - HDCP CTS 1A-02
    #ifndef CUSTOMER_NDS
        else
        {
        	if(u32State & E_HDMITX_EVENT_RUN)
        		MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
        }
    #endif // CUSTOMER_NDS
    }

    else if((u32State & E_HDMITX_EVENT_RITIMER)\
            && (gHDMITxInfo.hdmitx_fsm_state == E_HDMITX_FSM_HDCP_AUTH_DONE))
    {
        if(gHDMITxInfo.hdmitx_tmds_flag)
        {
            if (_MDrv_HDMITX_HdcpAuthCheckRi() == E_HDMITX_HDCP_SYNC_RI_FAIL)
            {
                gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_WAIT_RX;
                gHDMITxInfo.hdmitx_HDCPAuth_Status = E_HDMITX_HDCP_SYNC_RI_FAIL;
                gHDMITxInfo.hdmitx_HdcpStartAuth_flag = TRUE;
                gHDMITxInfo.hdmitx_HdcpAuthDone_flag = FALSE;
                MsOS_StopTimer(_s32HDCPRiTimerId);
            #ifdef CUSTOMER_NDS
                gHDMITxInfo.events |= HDMI_EVENT_CP_LINK_INTEGRITY_FAILED;
            #else
                MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
            #endif // CUSTOMER_NDS

                if(gHDMITxInfo.hdmitx_unHDCPRx_Control == E_HDCP_ENCRYPTION)
                {
                    MHal_HDMITx_HdcpSetEncrypt(ENABLE); // for HDCP CTS
                }
                else if(gHDMITxInfo.hdmitx_unHDCPRx_Control == E_BLUE_SCREEN)
                {
                    DBG_HDMITX(printf("\n[6432]E_HDMITX_FSM_HDCP_AUTH_DONE\n"));
                    MHal_HDMITx_HdcpSetEncrypt(DISABLE);
                    gHDMITxInfo.hdmitx_video_flag = FALSE;
                    MDrv_HDMITx_SetVideoOnOff();
                    gHDMITxInfo.hdmitx_audio_flag = FALSE;
                    MDrv_HDMITx_SetAudioOnOff();
                }
                else // normal output
                {
                    if(_s32HDCPRiTimerId > 0)
                    {
                        MsOS_StopTimer(_s32HDCPRiTimerId);
                    }
                    MHal_HDMITx_HdcpSetEncrypt(DISABLE); // for HDCP CTS
                    gHDMITxInfo.hdmitx_video_flag = TRUE;
                    MDrv_HDMITx_SetVideoOnOff();
                }

                MsOS_DelayTask(20);


            }
            //DBG_HDMITX(printf("\n_MDrv_HDMITX_HDCP_Ri_TimerCallback(): Time tick= %d ms\n", MsOS_GetSystemTime()));
        }
    // To avoid two events are coming together - HDCP CTS 1A-02
    #ifndef CUSTOMER_NDS
        else
        {
        	if(u32State & E_HDMITX_EVENT_RUN)
        		MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
        }
    #endif // CUSTOMER_NDS
    }

    else if(u32State & E_HDMITX_EVENT_RUN)
    {
        switch(gHDMITxInfo.hdmitx_fsm_state)
        {
            case E_HDMITX_FSM_PENDING:
                gHDMITxInfo.hdmitx_fsm_prestate = E_HDMITX_FSM_PENDING;
                break;

            case E_HDMITX_FSM_CHECK_HPD:
                gHDMITxInfo.hdmitx_fsm_prestate = E_HDMITX_FSM_CHECK_HPD;
                break;

            case E_HDMITX_FSM_VALIDATE_EDID:
                if (MDrv_HDMITx_EdidChecking()) // EDID is valid
                {
                    DBG_HDMITX(printf("\nMDrv_HDMITX_Process()::EDID OK\n"));

                    //Start to do the first part of authentication, set HDCPRxValid to TRUE;
                    g_bHDCPRxValid = TRUE;

                    if(gHDMITxInfo.hdmitx_hdcp_flag) // HDCP on
                    {
                        gHDMITxInfo.hdmitx_HdcpStartAuth_flag = TRUE;
                        gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_WAIT_RX;
                    }
                    else // HDCP off
                    {
                        gHDMITxInfo.hdmitx_HdcpStartAuth_flag = FALSE;
                        gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_DONE;
                        if(_s32HDCPRiTimerId > 0)
                            MsOS_StopTimer(_s32HDCPRiTimerId);
                    }
                #ifndef CUSTOMER_NDS
                    gHDMITxInfo.hdmitx_tmds_flag = TRUE;
                    MDrv_HDMITx_SetHDMITxMode();
                    MDrv_HDMITx_SetColorFormat();
                    MDrv_HDMITx_SetVideoOutputMode();

                    if((gHDMITxInfo.hdmitx_unHDCPRx_Control == E_NORMAL_OUTPUT)
                        ||(gHDMITxInfo.hdmitx_hdcp_flag == FALSE))
                    {
                        gHDMITxInfo.hdmitx_video_flag = TRUE;
                    }
                    else
                    {
                        MDrv_HDMITx_SetAVMUTE(TRUE);
                        gHDMITxInfo.hdmitx_avmute_flag = TRUE;
                        //Fix Panasonic UITA 2000 HDCP test fail
                        //Video should be show after ENC_EN was detects
                        gHDMITxInfo.hdmitx_video_flag = FALSE;
                        gHDMITxInfo.hdmitx_audio_flag = FALSE;
                    }
                    MDrv_HDMITx_SetVideoOnOff();
                    MDrv_HDMITx_SetAudioFrequency();
                    MDrv_HDMITx_SetAudioOnOff();
                    MDrv_HDMITx_SetTMDSOnOff();
                #endif
                }
                else // EDID is invalid
                {
                    DBG_HDMITX(printf("\nMDrv_HDMITX_Process()::EDID Fail\n"));
                    gHDMITxInfo.hdmitx_HdcpStartAuth_flag = FALSE;
                    gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_CHECK_HPD;
                }
            #ifndef CUSTOMER_NDS
                MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
            #endif // CUSTOMER_NDS
                gHDMITxInfo.hdmitx_fsm_prestate = E_HDMITX_FSM_VALIDATE_EDID;
                break;

            case E_HDMITX_FSM_HDCP_AUTH_WAIT_RX:
                gRepeaterFailTime = 0;
                gRepeaterStartTime = MsOS_GetSystemTime();
#ifdef VANCLEEF_MCP
                //Vancleef 5.7, write "Ready Bit" list to MST3000C
                //reset status if Tx restart HDCP authentication
                MDrv_HDMITx_Vancleef_RxWriteRepeaterReady(FALSE);
#endif
                if(MHal_HDMITx_GetRXStatus() != E_HDMITX_DVIClock_H_HPD_H)
                {
                    gHDMITxInfo.hdmitx_preRX_status = MHal_HDMITx_GetRXStatus();
                    gHDMITxInfo.hdmitx_tmds_flag = FALSE;
                    gHDMITxInfo.hdmitx_fsm_prestate = E_HDMITX_FSM_HDCP_AUTH_WAIT_RX;
                    MDrv_HDMITx_SetTMDSOnOff();
                    gHDMITxInfo.hdmitx_fsm_state =  E_HDMITX_FSM_PENDING;
                    MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
                    break;
                }
                if (gHDMITxInfo.hdmitx_HdcpStartAuth_flag && gHDMITxInfo.hdmitx_tmds_flag)
                {
                    MHal_HDMITx_HdcpSetEncrypt(DISABLE);
                    ret = _MDrv_HDMITx_HdcpAuthWaitingActiveRx();
                    gHDMITxInfo.hdmitx_HDCPAuth_Status = ret;
                #ifndef CUSTOMER_NDS
                    if (ret == E_HDMITX_HDCP_RX_IS_NOT_VALID)
                    {
                        // waiting for the active Rx Fail;
                        g_bHDCPRxValid = FALSE;

                        // CI+ patch
                        if(gHDMITxInfo.hdmitx_unHDCPRx_Control == E_HDCP_ENCRYPTION)
                        {
                            MHal_HDMITx_HdcpSetEncrypt(ENABLE); // for HDCP CTS
                        }
                        else if(gHDMITxInfo.hdmitx_unHDCPRx_Control == E_BLUE_SCREEN)
                        {
                            if(_s32HDCPRiTimerId > 0)
                            {
                                MsOS_StopTimer(_s32HDCPRiTimerId);
                            }
                            gHDMITxInfo.hdmitx_video_flag = FALSE;
                            MDrv_HDMITx_SetVideoOnOff();
                            gHDMITxInfo.hdmitx_audio_flag = FALSE;
                            MDrv_HDMITx_SetAudioOnOff();
                        }
                        else // normal output
                        {
                            if(_s32HDCPRiTimerId > 0)
                            {
                                MsOS_StopTimer(_s32HDCPRiTimerId);
                            }
                            MHal_HDMITx_HdcpSetEncrypt(DISABLE); // for HDCP CTS
                            gHDMITxInfo.hdmitx_video_flag = TRUE;
                            MDrv_HDMITx_SetVideoOnOff();
                        }
                        //MHal_HDMITx_HdcpSetEncrypt(ENABLE); // for HDCP CTS
                        MsOS_DelayTask(100);
                        if (gHDMITxInfo.hdmitx_hdcp_flag == TRUE)
                        {
                            gHDMITxInfo.hdmitx_fsm_state =  E_HDMITX_FSM_HDCP_AUTH_WAIT_RX;
                        }
                        else
                        {
                            gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_VALIDATE_EDID;
                            gHDMITxInfo.hdmitx_HdcpStartAuth_flag = FALSE;
                            gHDMITxInfo.hdmitx_HdcpAuthDone_flag = FALSE;
                        }
                        MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);

                        DBG_HDMITX(printf("_MDrv_HDMITx_HdcpAuthWaitingActiveRx retry!! \n"));
                        break;
                    }
                #endif
                    ret = _MDrv_HDMITx_HdcpAuthExangedAndComputated(gHDMITxInfo.output_mode);

                    gHDMITxInfo.hdmitx_HDCPAuth_Status = ret;
                    if (ret == E_HDMITX_HDCP_RX_IS_VALID)
                    {
                        // waiting for the active Rx OK;
                        g_bHDCPRxValid = TRUE;

                        {
                            MS_U8 u8count;
                            //MsOS_DelayTask(100); // AKSV -> Ro should be large than 100msec
                            for(u8count = 0; u8count<10; u8count++)
                            {
                                // HDCP CTS 1A-02
                                gHDMITxInfo.hdmitx_preRX_status = MHal_HDMITx_GetRXStatus();
                                if (gHDMITxInfo.hdmitx_preRX_status != E_HDMITX_DVIClock_H_HPD_H)
                                {
                                    gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_FAIL;
                                    gHDMITxInfo.hdmitx_fsm_prestate = E_HDMITX_FSM_HDCP_AUTH_WAIT_RX;
                                    MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
                                    break;
                                }
                                MsOS_DelayTask(10);
                            }
                        }
                        gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_CHECK_R0;
                    }
                    else
                    {
                        gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_FAIL;
                    }
                    MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
                }
                gHDMITxInfo.hdmitx_fsm_prestate = E_HDMITX_FSM_HDCP_AUTH_WAIT_RX;
                break;

            case E_HDMITX_FSM_HDCP_AUTH_CHECK_R0:
                ret = _MDrv_HDMITX_HdcpAuthCheckRi();
                gHDMITxInfo.hdmitx_HDCPAuth_Status = ret;
                //HDCP CTS 1B-02
                if(MHal_HDMITx_GetRXStatus() != E_HDMITX_DVIClock_H_HPD_H)
                {
                    gHDMITxInfo.hdmitx_preRX_status = MHal_HDMITx_GetRXStatus();
                    ret = E_HDMITX_HDCP_SYNC_RI_FAIL;
                }
                if (ret == E_HDMITX_HDCP_SYNC_RI_FAIL)
                {
                    gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_FAIL;
                    DBG_HDMITX(printf("Authentication Fail!!!\r\n"));
                }
                else  // R0=R0'
                {
                    if(gHDMITxInfo.hdmitx_HdcpCheckRepeater_flag)
                    {
                        if(_s32HDCPRiTimerId > 0)
                        {
                            MsOS_StopTimer(_s32HDCPRiTimerId);
                        }
                        MHal_HDMITx_HdcpSetEncrypt(DISABLE);
                        gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_CHECK_REPEATER;
                        DBG_HDMITX(printf("R0=R0' then check Repeater ready!!!\r\n"));
                    }
                    else
                    {
#ifdef VANCLEEF_MCP
                        //Vancleef Downstream is not a repeater
                        MS_U8 u8VCbstatus[2] = {0x00, 0x00};

                        //Count
                        u8VCbstatus[0] = 0x02;
                        //Depth
                        u8VCbstatus[1] = 0x01;

                        MDrv_HDMITx_Vancleef_RxLoadBStatus(u8VCbstatus);
#endif
                        gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_DONE;
                        DBG_HDMITX(printf("1st Authentication Pass!!!\r\n"));
                    #ifdef CUSTOMER_NDS
                        /*
                                         * Omit this delay before enabling hardware encryption.
                                         * We need a proper locking mechanism to do both delay
                                         * and set event in time.
                                         */
                        //MsOS_DelayTask(gHDMITxInfo.hdcp_encryptionStartTime);
                    #endif
                    #if 0
                        MHal_HDMITx_HdcpAuthPass();
                    #ifndef CUSTOMER_NDS
                        if(gHDMITxInfo.hdmitx_avmute_flag)
                        {
                            MDrv_HDMITx_SetAVMUTE(FALSE);
                            gHDMITxInfo.hdmitx_avmute_flag = FALSE;
                        }
                    #endif //#ifndef CUSTOMER_NDS
                    #endif
                    }
                }
                MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
                gHDMITxInfo.hdmitx_fsm_prestate = E_HDMITX_FSM_HDCP_AUTH_CHECK_R0;
                break;

            case E_HDMITX_FSM_HDCP_AUTH_CHECK_REPEATER:
                //HDCP CTS 1B-02
                if(MHal_HDMITx_GetRXStatus() != E_HDMITX_DVIClock_H_HPD_H)
                {
                    gHDMITxInfo.hdmitx_preRX_status = MHal_HDMITx_GetRXStatus();
                    gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_FAIL;
                    DBG_HDMITX(printf("Authentication Fail!!!\r\n"));
                    MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
                    gHDMITxInfo.hdmitx_fsm_prestate = E_HDMITX_FSM_HDCP_AUTH_CHECK_REPEATER;
                    break;
                }
                ret = _MDrv_HDMITx_HdcpAuthCheckRepeaterReady(); // check repeater is ready or not .
                gHDMITxInfo.hdmitx_HDCPAuth_Status = ret;
                if (ret == E_HDMITX_HDCP_REPEATER_READY)
                {
                    ret = _MDrv_HDMITx_HdcpCheckRepeater(); // check is sync with repeater or not

                    if (ret == E_HDMITX_HDCP_REPEATER_SHA1_FAIL) // ready but v!=v'
                    {
                        gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_FAIL;
                    }
                    else
                    {
                    #if 0
                        MHal_HDMITx_HdcpAuthPass();
                    #ifndef CUSTOMER_NDS
                        if(gHDMITxInfo.hdmitx_avmute_flag)
                        {
                            MDrv_HDMITx_SetAVMUTE(FALSE);
                            gHDMITxInfo.hdmitx_avmute_flag = FALSE;
                        }
                    #endif //#ifndef CUSTOMER_NDS
                    #endif
                        gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_DONE;
                        DBG_HDMITX(printf("[%s][%d] Repeater Ready!! diff = %lu !!!\r\n", __FUNCTION__, __LINE__, MsOS_Timer_DiffTimeFromNow(gRepeaterStartTime)));
                    }
                }
                else // not ready
                {
                    /*
                                 * HDCP CTS 1B-04: FAIL
                                 * According to VA-1809A test report,
                                 * It seems sometimes VA-1809A cost 7 seconds to rise ready bit, but it is set to 5 seconds in old setting.
                                 * Extend retry time from 5000ms to 10000ms where 10000ms = 20 * 50 * 10ms.
                                 */
                    if(gRepeaterFailTime < 10) //origin=20 updated 2014.04.23, because cycle time became 1060ms per cycle
                    {
                        // counting time per circle
						//DBG_HDMITX(printf("[%s][%d][test] Repeater SystemTime = %lu !!!\r\n", __FUNCTION__, __LINE__, MsOS_GetSystemTime()));
				
                        gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_CHECK_REPEATER;
                        {
                            MS_U8 i;
                            for(i=0;i<50;i++)
                            {
                                gHDMITxInfo.hdmitx_preRX_status = MHal_HDMITx_GetRXStatus();
                                if(gHDMITxInfo.hdmitx_preRX_status != E_HDMITX_DVIClock_H_HPD_H)
                                {

                                	//gHDMITxInfo.hdmitx_tmds_flag = FALSE;
                                	//gHDMITxInfo.hdmitx_fsm_prestate = E_HDMITX_FSM_HDCP_AUTH_WAIT_RX;
                                	//MDrv_HDMITx_SetTMDSOnOff();
                                	gHDMITxInfo.hdmitx_fsm_state =	E_HDMITX_FSM_PENDING;
                                	//MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
                                	break;
                                }
                                MsOS_DelayTask(10);
                            }
                            //MsOS_DelayTask(1000); // wait 10 sec to check whether repeater is ready or not
                        }
                    }
                    else
                    {
                        gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_FAIL;
                        DBG_HDMITX(printf("[%s][%d] Wait Repeater Timeout = %lu !!!\r\n", __FUNCTION__, __LINE__, MsOS_Timer_DiffTimeFromNow(gRepeaterStartTime)));
                    }
                    gRepeaterFailTime++;
                }
                MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
                gHDMITxInfo.hdmitx_fsm_prestate = E_HDMITX_FSM_HDCP_AUTH_CHECK_REPEATER;
                break;

            case E_HDMITX_FSM_HDCP_AUTH_DONE:
                gHDMITxInfo.hdmitx_HdcpAuthDone_flag = TRUE;
                gHDMITxInfo.hdmitx_HdcpStartAuth_flag = FALSE;
                // CI+ patch

                #ifndef CUSTOMER_NDS
                if(gHDMITxInfo.hdmitx_avmute_flag)
                {
                    MDrv_HDMITx_SetAVMUTE(FALSE);
                    gHDMITxInfo.hdmitx_avmute_flag = FALSE;
                }
                #endif //#ifndef CUSTOMER_NDS
                if(gHDMITxInfo.hdmitx_hdcp_flag)
                {
                    MHal_HDMITx_HdcpAuthPass();
#ifdef VANCLEEF_MCP
                    // Vancleef 5.2~5.7, repeater test!!
                    // Case: if downstream is not a repeater device
                    if (gHDMITxInfo.hdmitx_HdcpCheckRepeater_flag == FALSE)
                    {
                        // Move to HDMIRx ISR
                        //_MDrv_HDMITx_Vancleef_HdcpCheckKSVList();
                    }
#endif
                }
                //Fix Panasonic UITA 2000 HDCP test fail
                //Video should be show after ENC_EN was detect
                MsOS_DelayTask(20);

                gHDMITxInfo.hdmitx_video_flag = TRUE;
                MDrv_HDMITx_SetVideoOnOff();
                gHDMITxInfo.hdmitx_audio_flag = TRUE;
                MDrv_HDMITx_SetAudioOnOff();
#ifdef CUSTOMER_NDS
                gHDMITxInfo.hdmitx_HDCPAuth_Status = E_HDMITX_HDCP_AUTH_DONE;
#endif
                if(gHDMITxInfo.hdmitx_tmds_flag == FALSE)
                {
                    gHDMITxInfo.hdmitx_tmds_flag = TRUE;
                    MDrv_HDMITx_SetTMDSOnOff();
                }
                if(gHDMITxInfo.hdmitx_hdcp_flag) // HDCP on
                {
                    #ifdef CUSTOMER_NDS
                    /*
                     * The event must be set _AFTER_ hardware encryption to
                     * ensure that when FOSH client receiving this event,
                     * all works are done. (including
                     * MHal_HDMITx_HdcpAuthPass())
                     */
                    gHDMITxInfo.events |= HDMI_EVENT_CP_AUTHENTICATED;
                    #endif

                    // Create timer for timing monitor
                    /*
                                 * HDCP CTS 1B-01: FAIL, if Encryption is enabled prior to reading R0'
                                 * set HDCP_1ST_RiCheck_Timer as 2000ms
                                 */
                    if(_s32HDCPRiTimerId > 0) // timer is already created
                    {
                        //MsOS_StartTimer(_s32HDCPRiTimerId);
                        MsOS_DeleteTimer(_s32HDCPRiTimerId);
                        _s32HDCPRiTimerId = MsOS_CreateTimer( _MDrv_HDMITX_HDCP_Ri_TimerCallback,
                                                              HDCP_1ST_RiCheck_Timer,
                                                              gHDCPCheckRiTimer,
                                                              TRUE,
                                                              "HDCP Ri Timer");
                        //MsOS_ResetTimer(_s32HDCPRiTimerId, gHDCPCheckRiTimer, gHDCPCheckRiTimer, TRUE);
                        //MsOS_StartTimer(_s32HDCPRiTimerId);
                        DBG_HDMITX(printf("MDrv_HDMITX_Process: restart HDCP Ri timer, %d ms\n", (MS_U16)MsOS_GetSystemTime()));
                    }
                    else // 1st time
                    {
                        _s32HDCPRiTimerId = MsOS_CreateTimer( _MDrv_HDMITX_HDCP_Ri_TimerCallback,
                                                              gHDCPCheckRiTimer,
                                                              gHDCPCheckRiTimer,
                                                              TRUE,
                                                              "HDCP Ri Timer");
                        DBG_HDMITX(printf("MDrv_HDMITX_Process: Create 1st HDCP Ri timer\n"));
                        if(_s32HDCPRiTimerId < 0)
                        {
                            DBG_HDMITX(printf("MDrv_HDMITX_Process: Create HDCP Ri timer fail!\n"));
                        }
                    }
                }
                #if 0
                if (gHDMITxInfo.hdmitx_fsm_prestate != gHDMITxInfo.hdmitx_fsm_state)
                {
                    //MDrv_HDMITx_Exhibit();
                    if( (gHDMITxInfo.hdmitx_fsm_state == E_HDMITX_FSM_HDCP_AUTH_DONE) &&
                        (gHDMITxInfo.hdmitx_enable_flag == TRUE))
                    {
                        //config audio sampling rate

                        //Get Sampling Rate
                        MS_U32 SR;
                        MsHDMITX_AUDIO_FREQUENCY    new_frequncy;
                        MDrv_MAD_AudioSamplingRate(&SR);
                        DBG_HDMITX(printf("MDrv_HDMITX_Process()::Sampling Rate=%d K\n", SR));

                        if (SR<28)
                            new_frequncy = E_HDMITX_AUDIO_FREQ_NO_SIG;
                        else if ( SR< ((32+44)/2) )
                            new_frequncy = E_HDMITX_AUDIO_32K;
                        else if ( SR< ((44+48)/2) )
                            new_frequncy = E_HDMITX_AUDIO_44K;
                        else if ( SR< ((48+88)/2) )
                            new_frequncy = E_HDMITX_AUDIO_48K;
                        else if ( SR< ((88+96)/2) )
                            new_frequncy = E_HDMITX_AUDIO_88K;
                        else if ( SR< ((96+176)/2) )
                            new_frequncy = E_HDMITX_AUDIO_96K;
                        else if ( SR< ((176+192)/2) )
                            new_frequncy = E_HDMITX_AUDIO_176K;
                        else if ( SR< 200 )
                            new_frequncy = E_HDMITX_AUDIO_192K;
                        else
                            new_frequncy = E_HDMITX_AUDIO_FREQ_MAX_NUM;

                        if (( gHDMITxInfo.output_audio_frequncy != new_frequncy) &&
                              (new_frequncy != E_HDMITX_AUDIO_FREQ_MAX_NUM) &&
                              (new_frequncy != E_HDMITX_AUDIO_FREQ_NO_SIG))
                        {
                            gHDMITxInfo.output_audio_frequncy = new_frequncy;
                            MDrv_HDMITx_SetAudioFrequency();
                        }
                    }
                }
                #endif
                gHDMITxInfo.hdmitx_fsm_prestate = E_HDMITX_FSM_HDCP_AUTH_DONE;
                break;

            case E_HDMITX_FSM_HDCP_AUTH_FAIL:

                // Authentication fail
                if(gHDMITxInfo.hdmitx_HDCPRxFail_Control == (MDrvHDMITX_HDCPRXFail_CONTROL) E_HDCP_ENCRYPTION)
                {
                    MHal_HDMITx_HdcpSetEncrypt(ENABLE); // for HDCP CTS
                }
                else if(gHDMITxInfo.hdmitx_HDCPRxFail_Control == (MDrvHDMITX_HDCPRXFail_CONTROL) E_BLUE_SCREEN)
                {
                    gHDMITxInfo.hdmitx_video_flag = FALSE;
                    MDrv_HDMITx_SetVideoOnOff();
                    gHDMITxInfo.hdmitx_audio_flag = FALSE;
                    MDrv_HDMITx_SetAudioOnOff();
                }
                else // normal output
                {
                    if(_s32HDCPRiTimerId > 0)
                    {
                        MsOS_StopTimer(_s32HDCPRiTimerId);
                    }
                    MHal_HDMITx_HdcpSetEncrypt(DISABLE); // for HDCP CTS
                    gHDMITxInfo.hdmitx_video_flag = TRUE;
                    MDrv_HDMITx_SetVideoOnOff();
                }

            #ifndef CUSTOMER_NDS
                //MHal_HDMITx_HdcpSetEncrypt(ENABLE); // for HDCP CTS
					MsOS_DelayTask(100);
            #endif
                if (gHDMITxInfo.hdmitx_hdcp_flag == TRUE)
                {
                    gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_WAIT_RX;
                    gHDMITxInfo.hdmitx_HdcpStartAuth_flag = TRUE;
                gHDMITxInfo.hdmitx_HdcpAuthDone_flag = FALSE;
                }
                else
                {
                    gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_VALIDATE_EDID;
                    gHDMITxInfo.hdmitx_HdcpStartAuth_flag = FALSE;
                    gHDMITxInfo.hdmitx_HdcpAuthDone_flag = FALSE;

                }

                DBG_HDMITX(printf("Authentication Again!!!\r\n"));
            #ifdef CUSTOMER_NDS
                    gHDMITxInfo.events |= HDMI_EVENT_CP_AUTHENTICATION_FAILED;
            #else
                MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
            #endif
                gHDMITxInfo.hdmitx_fsm_prestate = E_HDMITX_FSM_HDCP_AUTH_FAIL;
                break;

            default:
                break;
        }
        DBG_HDMITX(printf("MDrv_HDMITX_Process():: HDCP FSM= %d\n", gHDMITxInfo.hdmitx_fsm_state));
    }

    else if(u32State & E_HDMITX_EVENT_IRQ)
    {
        irq_status = MHal_HDMITx_Int_Status();

        if(irq_status & E_HDMITX_IRQ_12) // TMDS hot-plug
        {
            DBG_HDMITX(printf("MDrv_HDMITX_Process()::E_HDMITX_IRQ_12\n"));
            //gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_CHECK_HPD;
            MHal_HDMITx_Int_Clear(E_HDMITX_IRQ_12);
            //MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
        }
        else if(irq_status & E_HDMITX_IRQ_10) // Rx disconnection
        {
            DBG_HDMITX(printf("MDrv_HDMITX_Process()::E_HDMITX_IRQ_10\n"));
            //gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_CHECK_HPD;
            MHal_HDMITx_Int_Clear(E_HDMITX_IRQ_10);
            //MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
        }
    }

    if(gHDMITxInfo.hdmitx_CECEnable_flag)
    {
    #if ENABLE_CEC_INT
    #ifdef CUSTOMER_NDS
        if(u32State & E_HDMITX_EVENT_CECRX)
        {
            gHDMITxInfo.events |= HDMI_EVENT_CEC_RECEIVED;
        }
    #endif // #ifdef CUSTOMER_NDS
    #endif // #if ENABLE_CEC_INT
    }

}


static int _HDMITx_Task(void *argv)
{
    MS_U32                 u32Events = 0;

    while (g_bHDMITxTask == TRUE)
    {
#if HDMITX_ISR_ENABLE
        // Task wake up
        //MsOS_WaitEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN|E_HDMITX_EVENT_IRQ, &u32Events, E_OR_CLEAR, MSOS_WAIT_FOREVER);
        //Loop polling MAD audio sampling rate and config it for HDMI TX
        MsOS_WaitEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN|E_HDMITX_EVENT_IRQ|E_HDMITX_EVENT_RITIMER|E_HDMITX_EVENT_RXTIMER|E_HDMITX_EVENT_CECRX, &u32Events, E_OR_CLEAR, 5000);

        ///if (MsOS_ObtainMutex(_s32HDMITxMutexId, HDMITX_MUTEX_TIMEOUT) == TRUE)
        if (1)
        {
            //if (u32Events & E_HDMITX_EVENT_IRQ)
            //{
                //MsOS_ClearEvent(_s32HDMITxEventId, E_HDMITX_EVENT_IRQ);
#endif
                MDrv_HDMITX_Process(u32Events);
                if(gHDMITxInfo.hdmitx_CECEnable_flag)
                {
    #if !ENABLE_CEC_INT
        #ifdef CUSTOMER_NDS
            if(MDrv_CEC_RxChkBuf())
                gHDMITxInfo.events |= HDMI_EVENT_CEC_RECEIVED;
        #else
            MDrv_CEC_RxChkBuf();
        #endif // CUSTOMER_NDS
    #endif // !HDMITX_ISR_ENABLE
            }
#if HDMITX_ISR_ENABLE
            //}
            ///MsOS_ReleaseMutex(_s32HDMITxMutexId);
        }
        else
        {
            DBG_HDMITX(printf("%s: obtain mutex failed.\n", __FUNCTION__));
        }

        // Even if not E_DMD_EVENT_ENABLE, the interrupt should be enable.
        MsOS_EnableInterrupt(E_INT_IRQ_HDMI_LEVEL);
#endif

        MsOS_DelayTask(HDMITX_MONITOR_DELAY);
    } // Task loop

    return 0;
}

//------------------------------------------------------------------------------
/// @brief This routine sends HDMI related packets.
/// @param[in] packet_type packet type
/// @param[in] packet_process packet behavior
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_SendPacket(MsHDMITX_PACKET_TYPE packet_type, MS_U8 packet_process)
{
    MHal_HDMITx_SendPacket(packet_type, packet_process, gHDMITxInfo.output_colordepth_val, gHDMITxInfo.output_video_timing,
        gHDMITxInfo.output_color, gHDMITxInfo.output_aspect_ratio, gHDMITxInfo.hdmitx_AFD_override_mode, gHDMITxInfo.output_scan_info,
        gHDMITxInfo.output_afd_ratio, gHDMITxInfo.output_activeformat_present,
        gHDMITxInfo.output_audio_frequncy, gHDMITxInfo.output_audio_channel, gHDMITxInfo.output_audio_type, gHDMITxInfo.vs_pkt_format,
        gHDMITxInfo.vs_pkt_3d, gHDMITxInfo.vs_4k2k_vic
	);
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// @brief This routine initializes HDMI Tx module
/// @return None
/// HDMI driver is in @ref E_TASK_PRI_HIGH level
//------------------------------------------------------------------------------
MS_BOOL MDrv_HDMITx_Init(void)
{
    MS_U32 u32PMRIUBaseAddress = 0, u32PMBankSize = 0;

    _hdmitx_status.bIsInitialized = TRUE;
    _hdmitx_status.bIsRunning     = TRUE;

    if (_s32HDMITxTaskId >= 0)
    {
        DBG_HDMITX(printf("MDrv_HDMITx_Init: already initialized.\n"));
        return TRUE;
    }

    DBG_HDMITX(printf("%s\n", __FUNCTION__));

    if(!MDrv_MMIO_GetBASE( &u32PMRIUBaseAddress, &u32PMBankSize, MS_MODULE_PM ))
    {
        printf("MApi_XC_Init Get PM BASE failure\n");
        return FALSE;
    }
    DBG_HDMITX(printf("Get IOMAP ID:%u Base:%lx!\n", MS_MODULE_PM, u32PMRIUBaseAddress));
    MHal_HDMITx_SetIOMapBase(u32PMRIUBaseAddress, u32PMRIUBaseAddress);

    MDrv_HDMITx_Power_OnOff(TRUE);
#if HDMITX_ISR_ENABLE
    MsOS_DisableInterrupt(E_INT_IRQ_HDMI_LEVEL);
    MsOS_DetachInterrupt(E_INT_IRQ_HDMI_LEVEL);

    MHal_HDMITx_Int_Disable(gHDMITXIRQ);
    MHal_HDMITx_Int_Clear(gHDMITXIRQ);

    if(gHDMITxInfo.hdmitx_CECEnable_flag)
    {
    #if ENABLE_CEC_INT
        MsOS_DisableInterrupt(E_INT_IRQ_CEC);
        MsOS_DetachInterrupt(E_INT_IRQ_CEC);
    #endif
    }

#endif // #if HDMITX_ISR_ENABLE

    MDrv_HDMITx_InitSeq();
///
#if 0
    _s32HDMITxMutexId = MsOS_CreateMutex(E_MSOS_FIFO, "HDMITx_Mutex", MSOS_PROCESS_SHARED);
    if (_s32HDMITxMutexId < 0)
    {
        return FALSE;
    }
#endif

    _s32HDMITxEventId = MsOS_CreateEventGroup("HDMITx_Event");
    if (_s32HDMITxEventId < 0)
    {
        ///MsOS_DeleteMutex(_s32HDMITxMutexId);
        return FALSE;
    }

#if 0
    _pHDMITxTaskStack = MsOS_AllocateMemory(HDMITX_TASK_STACK_SIZE, gs32CachedPoolID);
    if (_pHDMITxTaskStack == NULL)
    {
        MsOS_DeleteEventGroup(_s32HDMITxEventId);
        MsOS_DeleteMutex(_s32HDMITxMutexId);
        return FALSE;
    }
#else
    _pHDMITxTaskStack = _u8HDMITx_StackBuffer;
#endif
    g_bHDMITxTask = TRUE;

     _s32HDMITxTaskId = MsOS_CreateTask(_HDMITx_Task,
                                    (MS_U32)NULL,
                                    E_TASK_PRI_HIGH,
                                    TRUE,
                                    _pHDMITxTaskStack,
                                    HDMITX_TASK_STACK_SIZE,
                                    "HDMITx_Task");
    if (_s32HDMITxTaskId < 0)
    {
#if 0
        MsOS_FreeMemory(_pHDMITxTaskStack, gs32CachedPoolID);
#else
#endif
        MsOS_DeleteEventGroup(_s32HDMITxEventId);
        ///MsOS_DeleteMutex(_s32HDMITxMutexId);
        return FALSE;
    }

#if HDMITX_ISR_ENABLE
    MsOS_AttachInterrupt(E_INT_IRQ_HDMI_LEVEL, _HDMITx_Isr);
    MsOS_EnableInterrupt(E_INT_IRQ_HDMI_LEVEL);                                    // Enable TSP interrupt
    //MDrv_HDMITx_Int_Enable();

    if(gHDMITxInfo.hdmitx_CECEnable_flag)
    {
    #if ENABLE_CEC_INT
        DBG_HDMITX(printf("\nEnable CEC INT!!\r\n"));
        MsOS_AttachInterrupt(E_INT_IRQ_CEC, _HDMITX_CEC_RxIsr);
        MsOS_EnableInterrupt(E_INT_IRQ_CEC);
    #endif
    }

#endif // #if HDMITX_ISR_ENABLE

#if 0
    // Create timer for timing monitor
    _s32CheckRxTimerId = MsOS_CreateTimer( _MDrv_HDMITX_CheckRx_TimerCallback,
                                          5000,//CheckRx_Timer,
                                          CheckRx_Timer,
                                          TRUE,
                                          "Check Rx timer");
    if(_s32CheckRxTimerId < 0)
    {
        DBG_HDMITX(printf("MDrv_HDMITX_Init: Create Check Rx timer fail!\n"));
    }
#endif

//#ifdef CUSTOMER_NDS
    if(gHDMITxInfo.hdmitx_CECEnable_flag)
        MDrv_CEC_Init(12000000UL);
//#endif // CUSTOMER_NDS

#ifdef VANCLEEF_MCP
    MDrv_HDMITx_Vancleef_Init();
    MDrv_HDMITx_Vancleef_RxLoadHDCPKey();
    MDrv_HDMITx_Vancleef_RegisterISR();
#endif

    return TRUE;
}

//------------------------------------------------------------------------------
/// @brief This routine get HDCP key
/// @param[in] useinternalkey: TRUE -> from internal, FALSE -> from external, like SPI flash
/// @param[in] data: data point
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_GetHdcpKey(MS_BOOL useinternalkey, MS_U8 *data)
{
    MHal_HDMITx_GetHdcpKey(useinternalkey, data);
}

//------------------------------------------------------------------------------
/// @brief This routine start/stop HDCP authentication
/// @param[in] bFlag: TRUE -> start authentication, FALSE -> stop authentication
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_HDCP_StartAuth(MS_BOOL bFlag)
{
    if(bFlag)
    {
        gHDMITxInfo.hdmitx_HdcpStartAuth_flag = TRUE;
        gHDMITxInfo.hdmitx_HdcpAuthDone_flag = FALSE;
        gHDMITxInfo.hdmitx_HDCPAuth_Status = E_HDMITX_HDCP_RESET;
        gHDMITxInfo.hdmitx_hdcp_flag = TRUE;
        gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_HDCP_AUTH_WAIT_RX;
        MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
    }
    else
    {
        gHDMITxInfo.hdmitx_HdcpStartAuth_flag = FALSE;
        gHDMITxInfo.hdmitx_HdcpAuthDone_flag = FALSE;
        gHDMITxInfo.hdmitx_HDCPAuth_Status = E_HDMITX_HDCP_RESET;
        gHDMITxInfo.hdmitx_hdcp_flag = FALSE;
        gHDMITxInfo.hdmitx_fsm_state = E_HDMITX_FSM_VALIDATE_EDID;
        MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);

        if(_s32HDCPRiTimerId > 0)
            MsOS_StopTimer(_s32HDCPRiTimerId);
    }
}

//------------------------------------------------------------------------------
/// @brief This routine turn on/off HDMI PLL
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_PLLOnOff(void)
{
    MHal_HDMITx_PLLOnOff(gHDMITxInfo.hdmitx_enable_flag);
}

//------------------------------------------------------------------------------
/// @brief This function clear settings of user defined packet
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_PKT_User_Define_Clear(void)
{
    MHal_HDMITx_PKT_User_Define_Clear();
}

//------------------------------------------------------------------------------
/// @brief This function set user defined hdmi packet
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_PKT_User_Define(MsHDMITX_PACKET_TYPE packet_type, MS_BOOL def_flag,
								 MsHDMITX_PACKET_PROCESS def_process, MS_U8 def_fcnt)
{
    MHal_HDMITx_PKT_User_Define(packet_type, def_flag, def_process, def_fcnt);
    if(_hdmitx_status.bIsInitialized) // fix code dump
    {
        MDrv_HDMITx_SendPacket(packet_type, def_process);
    }
}

//------------------------------------------------------------------------------
/// @brief This function set user defined hdmi packet content
/// @return None
//------------------------------------------------------------------------------
MS_BOOL MDrv_HDMITx_PKT_Content_Define(MsHDMITX_PACKET_TYPE packet_type, MS_U8* data, MS_U8 length)
{
    return MHal_HDMITx_PKT_Content_Define(packet_type, data, length);
}

//------------------------------------------------------------------------------
/// @brief This routine will power on/off HDMITx clock (power saving)
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_Power_OnOff(MS_BOOL bEnable)
{
    MHal_HDMITx_Power_OnOff(bEnable);
}

//------------------------------------------------------------------------------
/// @brief This routine turn on/off HDMI Tx Module
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_TurnOnOff(void)
{
    if (gHDMITxInfo.hdmitx_enable_flag) // turn on
    {
        gHDMITxInfo.hdmitx_HdcpStartAuth_flag = FALSE;
        gHDMITxInfo.hdmitx_HdcpAuthDone_flag = FALSE;
        //MDrv_HDMITx_PLLOnOff();
    #if HDMITX_ISR_ENABLE
        MHal_HDMITx_Int_Enable(gHDMITXIRQ);
    #endif
        MHal_HDMITx_VideoInit();
        MHal_HDMITx_AudioInit();
        //MDrv_HDMITx_SetTMDSOnOff();
        //MDrv_HDMITx_Exhibit();
    }
    else // turn off
    {
        gHDMITxInfo.hdmitx_HdcpStartAuth_flag = FALSE;
        gHDMITxInfo.hdmitx_HdcpAuthDone_flag = FALSE;
    #if HDMITX_ISR_ENABLE
        MHal_HDMITx_Int_Disable(gHDMITXIRQ);
        MHal_HDMITx_Int_Clear(gHDMITXIRQ);
    #endif
        MDrv_HDMITx_SetTMDSOnOff();
        //MDrv_HDMITx_PLLOnOff();
        gHDMITxInfo.hdmitx_fsm_prestate = E_HDMITX_FSM_PENDING;
        gHDMITxInfo.hdmitx_preRX_status = E_HDMITX_DVIClock_L_HPD_L;
        if(_s32CheckRxTimerId > 0)
            MsOS_StopTimer(_s32CheckRxTimerId);
    }
}

//------------------------------------------------------------------------------
/// @brief This routine control HDMI packet generation
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_EnablePacketGen(MS_BOOL bflag)
{
    MHal_HDMITx_EnablePacketGen(bflag);
}

//------------------------------------------------------------------------------
/// @brief This routine config HDMI Tx output mode (DVI / HDMI)
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_SetHDMITxMode(void)
{
    if ( MHal_HDMITx_IsSupportDVIMode() == FALSE)
    {
        /*
         * SW Patch,
         * Kaiser DO NOT support YUV2RGB
         */
        gHDMITxInfo.output_mode = ( gHDMITxInfo.output_mode | BIT1 );
    }
    // HDMI / DVI
    if(gHDMITxInfo.output_mode & BIT1) // HDMI
    {
        gHDMITxInfo.hdmitx_audio_flag = TRUE;
        MHal_HDMITx_SetHDMImode(TRUE, gHDMITxInfo.output_colordepth_val);
        if(gHDMITxInfo.output_colordepth_val) // RX support DC
            MDrv_HDMITx_SendPacket(E_HDMITX_GC_PACKET, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET);
        else
            MDrv_HDMITx_SendPacket(E_HDMITX_GC_PACKET, E_HDMITX_STOP_PACKET);
        MDrv_HDMITx_SendPacket(E_HDMITX_NULL_PACKET, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET);
        MDrv_HDMITx_SendPacket(E_HDMITX_VS_INFOFRAME, E_HDMITX_STOP_PACKET);
        MDrv_HDMITx_SendPacket(E_HDMITX_SPD_INFOFRAME, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET);
        DBG_HDMITX(printf("MDrv_HDMITx_SetHDMITxMode: HDMI mode = %d \n", gHDMITxInfo.output_mode));
    }
    else // DVI
    {
        gHDMITxInfo.hdmitx_audio_flag = FALSE;
        MHal_HDMITx_SetHDMImode(FALSE, gHDMITxInfo.output_colordepth_val);
        DBG_HDMITX(printf("MDrv_HDMITx_SetHDMITxMode: DVI mode = %d \n", gHDMITxInfo.output_mode));
    }
#if 0
    // HDCP
    if(gHDMITxInfo.output_mode & BIT0) // HDCP on
    {
        if(gHDMITxInfo.output_mode) // HDMI EESS
            MDrv_HDMITx_Mask_Write(REG_HDMITX_BANK0, REG_HDCP_TX_MODE_41, 0x0C00, 0x0400);
        else // DVI OESS
            MDrv_HDMITx_Mask_Write(REG_HDMITX_BANK0, REG_HDCP_TX_MODE_41, 0x0C00, 0x0000);
        // HDCP encryption
        MDrv_HDMITx_Mask_Write(REG_HDMITX_BANK0, REG_HDCP_TX_COMMAND_42, 0x0008, 0x0008);
    }
    else // HDCP off
        MDrv_HDMITx_Mask_Write(REG_HDMITX_BANK0, REG_HDCP_TX_COMMAND_42, 0x0008, 0x0000);
#endif
}

//------------------------------------------------------------------------------
/// @brief This routine turn on/off HDMI Tx TMDS signal
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_SetTMDSOnOff(void)
{
    if(!g_bDisableTMDSCtrl)
    {
        MHal_HDMITx_SetTMDSOnOff(gHDMITxInfo.hdmitx_RB_swap_flag, gHDMITxInfo.hdmitx_tmds_flag);
    }
}

//------------------------------------------------------------------------------
/// @brief This routine turn on/off HDMI Tx video output
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_SetVideoOnOff(void)
{
    if(gHDMITxInfo.output_video_timing >= E_HDMITX_RES_1280x720p_50Hz)
        MHal_HDMITx_SetVideoOnOff(gHDMITxInfo.hdmitx_video_flag, gHDMITxInfo.hdmitx_csc_flag, TRUE);
    else
        MHal_HDMITx_SetVideoOnOff(gHDMITxInfo.hdmitx_video_flag, gHDMITxInfo.hdmitx_csc_flag, FALSE);
}

//------------------------------------------------------------------------------
/// @brief This routine sets video color formatt
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_SetColorFormat(void)
{
    MS_BOOL bflag;

    bflag = (gHDMITxInfo.output_color == E_HDMITX_VIDEO_COLOR_YUV422) ? TRUE : FALSE;
	if(gHDMITxInfo.output_video_timing >= E_HDMITX_RES_1280x720p_50Hz)
        MHal_HDMITx_SetColorFormat(gHDMITxInfo.hdmitx_csc_flag, bflag, TRUE);
    else
        MHal_HDMITx_SetColorFormat(gHDMITxInfo.hdmitx_csc_flag, bflag, FALSE);
    // send packet
    MDrv_HDMITx_SendPacket(E_HDMITX_AVI_INFOFRAME, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET);
}

//------------------------------------------------------------------------------
/// @brief This routine sets VS infoframe
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_Set_VS_InfoFrame(void)
{
    MDrv_HDMITx_SendPacket(E_HDMITX_VS_INFOFRAME, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET);
}

//------------------------------------------------------------------------------
/// @brief This routine sets video output mode (color/repetition/regen)
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_SetVideoOutputMode(void)
{
    if (gHDMITxInfo.output_video_timing >= E_HDMITX_RES_MAX)
    {
        printf("[%s][%d] Unsupport HDMI mode 0x%08x\n", __FUNCTION__, __LINE__, gHDMITxInfo.output_video_timing);
        return;
    }

    // 41.6 ms * 128 = 5.324 s, 40 ms * 128 = 5.12 s
    if( (gHDMITxInfo.output_video_timing == E_HDMITX_RES_1920x1080p_24Hz) || (gHDMITxInfo.output_video_timing == E_HDMITX_RES_1920x1080p_25Hz) ) // 24/25 Hz
        gHDCPCheckRiTimer = 5400;
    // 33.3 ms * 128 = 4.26 s
    else if (gHDMITxInfo.output_video_timing == E_HDMITX_RES_1920x1080p_30Hz) // 30 Hz
        gHDCPCheckRiTimer = 4300;
    // 16.6 ms * 128 = 2.124 s, 20 ms * 128 = 2.56 s
    else // 50/60 Hz
        gHDCPCheckRiTimer = HDCP_RiCheck_Timer;//2600;

    MHal_HDMITx_SetVideoOutputMode(gHDMITxInfo.output_video_timing, gHDMITxInfo.hdmitx_csc_flag, gHDMITxInfo.output_colordepth_val, &gHDMITxInfo.analog_setting);
    //MDrv_HDMITx_SendPacket(E_HDMITX_AVI_INFOFRAME, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET);
}

//------------------------------------------------------------------------------
/// @brief This routine sets video aspect ratio
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_SetVideoOutputAsepctRatio(void)
{
    // send packet
    MDrv_HDMITx_SendPacket(E_HDMITX_AVI_INFOFRAME, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET);
}

//------------------------------------------------------------------------------
/// @brief This routine sets video scan info and AFD
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_SetVideoOutputOverscan_AFD(void)
{
    // send packet
    MDrv_HDMITx_SendPacket(E_HDMITX_AVI_INFOFRAME, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET);
}

//------------------------------------------------------------------------------
/// @brief This routine turns On/off Audio module
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_SetAudioOnOff(void)
{
    if(gHDMITxInfo.hdmitx_audio_flag) // turn on
    {
        MHal_HDMITx_SetAudioOnOff(TRUE);
        MDrv_HDMITx_SendPacket(E_HDMITX_ACR_PACKET, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET);
        MDrv_HDMITx_SendPacket(E_HDMITX_AS_PACKET, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET);
        MDrv_HDMITx_SendPacket(E_HDMITX_AUDIO_INFOFRAME, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET);
        MDrv_HDMITx_SendPacket(E_HDMITX_ACP_PACKET, E_HDMITX_STOP_PACKET);
        MDrv_HDMITx_SendPacket(E_HDMITX_ISRC1_PACKET, E_HDMITX_STOP_PACKET);
        MDrv_HDMITx_SendPacket(E_HDMITX_ISRC2_PACKET, E_HDMITX_STOP_PACKET);
    }

    else // turn off
    {
        MHal_HDMITx_SetAudioOnOff(FALSE);
        MDrv_HDMITx_SendPacket(E_HDMITX_ACR_PACKET, E_HDMITX_STOP_PACKET);
        MDrv_HDMITx_SendPacket(E_HDMITX_AS_PACKET, E_HDMITX_STOP_PACKET);
        MDrv_HDMITx_SendPacket(E_HDMITX_AUDIO_INFOFRAME, E_HDMITX_STOP_PACKET);
        MDrv_HDMITx_SendPacket(E_HDMITX_ACP_PACKET, E_HDMITX_STOP_PACKET);
        MDrv_HDMITx_SendPacket(E_HDMITX_ISRC1_PACKET, E_HDMITX_STOP_PACKET);
        MDrv_HDMITx_SendPacket(E_HDMITX_ISRC2_PACKET, E_HDMITX_STOP_PACKET);
    }
}

//------------------------------------------------------------------------------
/// @brief This routine sets audio sampling freq.
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_SetAudioFrequency(void)
{
    if(gHDMITxInfo.hdmitx_audio_flag)
    {
        MHal_HDMITx_SetAudioFrequency(gHDMITxInfo.output_audio_frequncy, gHDMITxInfo.output_audio_channel, gHDMITxInfo.output_audio_type);
        MDrv_HDMITx_SendPacket(E_HDMITX_ACR_PACKET, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET);
        MDrv_HDMITx_SendPacket(E_HDMITX_AUDIO_INFOFRAME, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET);
    }
}

//------------------------------------------------------------------------------
/// @brief This routine sets audio source format.
/// @return None
//------------------------------------------------------------------------------

void MDrv_HDMITx_SetAudioSourceFormat(MsHDMITX_AUDIO_SOURCE_FORMAT fmt)
{
    MHal_HDMITx_SetAudioSourceFormat(fmt);
}

//------------------------------------------------------------------------------
/// @brief This routine Get Audio CTS value
/// @return CTS
//------------------------------------------------------------------------------
MS_U32 MDrv_HDMITx_GetAudioCTS(void)
{
    return MHal_HDMITx_GetAudioCTS();
}

//------------------------------------------------------------------------------
/// @brief This routine Mute Audio FIFO
/// @param[in] bflag: True: mute audio, False: unmute audio
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_MuteAudioFIFO(MS_BOOL bflag)
{
    MHal_HDMITx_MuteAudioFIFO(bflag);
}

//------------------------------------------------------------------------------
/// @brief This routine sets HDMI Tx HDCP encryption On/Off
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_SetHDCPOnOff(void)
{
    MHal_HDMITx_SetHDCPOnOff(gHDMITxInfo.hdmitx_hdcp_flag, ((gHDMITxInfo.output_mode & BIT1) ? TRUE:FALSE));
}

/// Get version (without Mutex protect)
MS_BOOL  MDrv_HDMITx_GetLibVer(const MSIF_Version **ppVersion)
{
    if (!ppVersion)
    {
        return FALSE;
    }

    *ppVersion = &_drv_hdmi_tx_version;
    return TRUE;
}

 MS_BOOL  MDrv_HDMITx_GetInfo(MS_HDMI_TX_INFO *pInfo)
 {
     if (!pInfo)
    {
        return FALSE;
    }

    memcpy((void*)pInfo, (void*)&_info, sizeof(MS_HDMI_TX_INFO));
    return TRUE;
 }

 MS_BOOL  MDrv_HDMITx_GetStatus(MS_HDMI_TX_Status *pStatus)
 {
      if (!pStatus)
    {
        return FALSE;
    }

    memcpy((void*)pStatus, (void*)&_hdmitx_status, sizeof(MS_HDMI_TX_Status ));
    return TRUE;
 }

 ///@brief setdbug level
 ///@u16DbgSwitch  1 means hdmitx only, 2 mean
 MS_BOOL  MDrv_HDMITx_SetDbgLevel(MS_U16 u16DbgSwitch)
 {
    _u16DbgHDMITXSwitch = u16DbgSwitch;
    MHal_HDMITx_HdcpDebugEnable(_u16DbgHDMITXSwitch & HDMITX_DBG_HDCP ? TRUE : FALSE);
    MHal_HDMITx_UtilDebugEnable(_u16DbgHDMITXSwitch & HDMITX_DBG_UTILTX ? TRUE : FALSE);

#ifdef VANCLEEF_MCP
    MDrv_HDMITx_Vancleef_DebugEnable(_u16DbgHDMITXSwitch & HDMITX_DBG_HDCP ? TRUE : FALSE);
#endif
    return TRUE;
 }

void MDrv_HDMITx_SetHPDGpioPin(MS_U8 u8pin)
{
    MHal_HDMITx_SetHPDGpioPin(u8pin);
}

//------------------------------------------------------------------------------
/// @brief This routine turn on/off HDMI Tx output force mode
/// @argument:
///              - bflag: TRUE: force mode, FALSE: auto mode
///              - output_mode: E_HDMITX_DVI: DVI, E_HDMITX_HDMI: HDMI
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_ForceHDMIOutputMode(MS_BOOL bflag, MsHDMITX_OUTPUT_MODE output_mode)
{
    gHDMITxInfo.hdmitx_force_mode = bflag;
    if(bflag)
    {
        gHDMITxInfo.force_output_mode = output_mode;
        gHDMITxInfo.output_mode = gHDMITxInfo.force_output_mode;
        if((gHDMITxInfo.force_output_mode == E_HDMITX_DVI) || (gHDMITxInfo.force_output_mode == E_HDMITX_DVI_HDCP)) // DVI mode
        {
            gHDMITxInfo.hdmitx_audio_supportAI = FALSE;
            gHDMITxInfo.output_color = E_HDMITX_VIDEO_COLOR_RGB444;
        }
    }
}

//------------------------------------------------------------------------------
/// @brief This routine turn on/off HDMI Tx output force color format
/// @argument:
///              - bflag: TRUE: force output color format, FALSE: auto mode
///              - output_mode: HDMITX_VIDEO_COLOR_RGB444: RGB, HDMITX_VIDEO_COLOR_YUV444: YUV
/// @return Ture: Set force output color format successfully
///             FALSE: Fail to set force output color format
//------------------------------------------------------------------------------
MS_BOOL MDrv_HDMITx_ForceHDMIOutputColorFormat(MS_BOOL bflag, MsHDMITX_VIDEO_COLOR_FORMAT output_color)
{
    if(bflag)
    {
        if((output_color != E_HDMITX_VIDEO_COLOR_RGB444) && (gHDMITxInfo.output_mode == E_HDMITX_DVI || gHDMITxInfo.output_mode == E_HDMITX_DVI_HDCP))
        {
            printf("Set force output color format failed!\n");
            return FALSE;
        }

        gHDMITxInfo.hdmitx_force_output_color = bflag;
        gHDMITxInfo.force_output_color = output_color;
        gHDMITxInfo.output_color = gHDMITxInfo.force_output_color;

        if( (gHDMITxInfo.input_color != gHDMITxInfo.output_color) && (gHDMITxInfo.output_color == E_HDMITX_VIDEO_COLOR_RGB444) )
            gHDMITxInfo.hdmitx_csc_flag = TRUE;
        else // bypass
            gHDMITxInfo.hdmitx_csc_flag = FALSE;
    }

    return TRUE;
}

//------------------------------------------------------------------------------
/// @brief This routine set HDMI Tx AVMUTE
/// @argument:
///              - bflag: True=> SET_AVMUTE, FALSE=>CLEAR_AVMUTE
///
/// @return None
//------------------------------------------------------------------------------
void MDrv_HDMITx_SetAVMUTE(MS_BOOL bflag)
{
    if(bflag)
    {
        MDrv_HDMITx_SendPacket(E_HDMITX_GC_PACKET, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET|E_HDMITX_GCP_SET_AVMUTE);
        DBG_HDMITX(printf("\nMDrv_HDMITx_SetAVMUTE \n"));
    }
    else
    {
        MDrv_HDMITx_SendPacket(E_HDMITX_GC_PACKET, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET|E_HDMITX_GCP_CLEAR_AVMUTE);
        MsOS_DelayTask(50);
        if(gHDMITxInfo.output_colordepth_val) // RX support DC
            MDrv_HDMITx_SendPacket(E_HDMITX_GC_PACKET, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET);
        else
            MDrv_HDMITx_SendPacket(E_HDMITX_GC_PACKET, E_HDMITX_STOP_PACKET);
        DBG_HDMITX(printf("\nMDrv_HDMITx_ClearAVMUTE \n"));
    }
}

static MsHDMITX_AVI_VIC _MDrv_HDMIRx_MapVideoTimingToVIC(MsHDMITX_VIDEO_TIMING timing)
{
    switch(timing)
    {
        case E_HDMITX_RES_640x480p:
            return E_HDMITX_VIC_640x480p_60_4_3;

        case E_HDMITX_RES_720x480i:
            return E_HDMITX_VIC_720x480i_60_16_9;

        case E_HDMITX_RES_720x576i:
            return E_HDMITX_VIC_720x576i_50_16_9;

        case E_HDMITX_RES_720x480p:
            return E_HDMITX_VIC_720x480p_60_16_9;

        case E_HDMITX_RES_720x576p:
            return E_HDMITX_VIC_720x576p_50_16_9;

        case E_HDMITX_RES_1280x720p_50Hz:
            return E_HDMITX_VIC_1280x720p_50_16_9;

        case E_HDMITX_RES_1280x720p_60Hz:
            return E_HDMITX_VIC_1280x720p_60_16_9;

        case E_HDMITX_RES_1920x1080i_50Hz:
            return E_HDMITX_VIC_1920x1080i_50_16_9;

        case E_HDMITX_RES_1920x1080i_60Hz:
            return E_HDMITX_VIC_1920x1080i_60_16_9;

        case E_HDMITX_RES_1920x1080p_24Hz:
            return E_HDMITX_VIC_1920x1080p_24_16_9;

        case E_HDMITX_RES_1920x1080p_25Hz:
            return E_HDMITX_VIC_1920x1080p_25_16_9;

        case E_HDMITX_RES_1920x1080p_30Hz:
            return E_HDMITX_VIC_1920x1080p_30_16_9;

        case E_HDMITX_RES_1920x1080p_50Hz:
            return E_HDMITX_VIC_1920x1080p_50_16_9;

        case E_HDMITX_RES_1920x1080p_60Hz:
            return E_HDMITX_VIC_1920x1080p_60_16_9;

        default:
            printf("[HDMITx]: Error video timing: %d\n", timing);
            return E_HDMITX_VIC_NOT_AVAILABLE;
    }
}

MS_BOOL MDrv_HDMITx_GetRx3DStructureFromEDID(MsHDMITX_VIDEO_TIMING timing, MsHDMITX_EDID_3D_STRUCTURE_ALL *p3DStructure)
{
    MS_U8 i;
    MsHDMITX_AVI_VIC vic = E_HDMITX_VIC_NOT_AVAILABLE;
    MsHDMITX_EDID_3D_STRUCTURE_ALL e3DStructureFromEDID = 0; // Default all not support

    vic = _MDrv_HDMIRx_MapVideoTimingToVIC(timing);

    if(gHDMITxInfo.edid_3D_50hz_support)
    {
        switch(vic)
        {
            case E_HDMITX_VIC_1920x1080p_24_16_9:
                e3DStructureFromEDID |= E_HDMITX_EDID_3D_FramePacking;
                e3DStructureFromEDID |= E_HDMITX_EDID_3D_TopandBottom;
                break;
            case E_HDMITX_VIC_1280x720p_50_16_9:
                e3DStructureFromEDID |= E_HDMITX_EDID_3D_FramePacking;
                e3DStructureFromEDID |= E_HDMITX_EDID_3D_TopandBottom;
                break;
            case E_HDMITX_VIC_1920x1080i_50_16_9:
            case E_HDMITX_VIC_1920x1080i_50_16_9_1250_total:
                e3DStructureFromEDID |= E_HDMITX_EDID_3D_SidebySide_Half_horizontal;
                break;
            default:
                break;
        }

    }

    if(gHDMITxInfo.edid_3D_60hz_support)
    {
        switch(vic)
        {
    		case E_HDMITX_VIC_1920x1080p_24_16_9:
    			e3DStructureFromEDID |= E_HDMITX_EDID_3D_FramePacking;
    			e3DStructureFromEDID |= E_HDMITX_EDID_3D_TopandBottom;
    			break;
    		case E_HDMITX_VIC_1280x720p_60_16_9:
    			e3DStructureFromEDID |= E_HDMITX_EDID_3D_FramePacking;
    			e3DStructureFromEDID |= E_HDMITX_EDID_3D_TopandBottom;
    			break;
    		case E_HDMITX_VIC_1920x1080i_60_16_9:
    			e3DStructureFromEDID |= E_HDMITX_EDID_3D_SidebySide_Half_horizontal;
                break;
            default:
                break;
        }
    }

    for(i=0;i<16;i++)
    {
        if(gHDMITxInfo.edid_3D_support_timing[i].support_timing == vic)
        {
            e3DStructureFromEDID |= gHDMITxInfo.edid_3D_support_timing[i].support_3D_structure;
        }
    }

    *p3DStructure = e3DStructureFromEDID;
    return TRUE;
}


// *************  For customer NDS **************//

MS_BOOL MDrv_HDMITx_GetRxStatus(void)
{
    // Check HDMI receiver status
    if (MHal_HDMITx_GetRXStatus() == E_HDMITX_DVIClock_H_HPD_H)
        return TRUE;
    else
        return FALSE;
}


MS_BOOL MDrv_HDMITx_IsHDCPRxValid(void)
{
    return g_bHDCPRxValid;
}

//------------------------------------------------------------------------------
/// HDCP SRM DSA Signature Verification
//------------------------------------------------------------------------------
#if 0
void	_dsa_init(dsa_context *dsa)
{
    dsa->p = bdNew();
    dsa->q = bdNew();
    dsa->g = bdNew();
    dsa->x = bdNew();
    dsa->y = bdNew();
}

void	_dsa_clear(dsa_context *dsa)
{
    bdFree(&dsa->p);
    bdFree(&dsa->q);
    bdFree(&dsa->g);
    bdFree(&dsa->x);
    bdFree(&dsa->y);
}

//
// FUNCTION dsa_verify
//
// m: H(m), hash of the message, in general this is SHA-1
// r: signature
// s: signature
//
// verify method:
// v = ( (g^u1 * y^u2) % p ) % q
// check v = r
//
MS_S8 _dsa_verify(dsa_context *dsa, BIGD m, BIGD r, BIGD s)
{
    BIGD w;
    BIGD u1;
    BIGD u2;
    BIGD v;
    BIGD tmp1, tmp2, tmp3;
    MS_S8	  result;

    w    = bdNew();
    u1   = bdNew();
    u2   = bdNew();
    v    = bdNew();
    tmp1 = bdNew();
    tmp2 = bdNew();
    tmp3 = bdNew();

    bdModInv(w,s,dsa->q);		// w = inv(s) % q
    bdModMult(u1,m,w,dsa->q);		// u1 = m*w % q
    bdModMult(u2,r,w,dsa->q);		// u2 = r*w % q
    bdModExp(tmp1,dsa->g,u1,dsa->p);	// tmp1 = (g^u1) % p
    bdModExp(tmp2,dsa->y,u2,dsa->p);	// tmp2 = (y^u2) % p
    bdModMult(tmp3,tmp1,tmp2,dsa->p);	// tmp3 = (g^u1 % p) * (y^u2 % p)
    bdModulo(v,tmp3,dsa->q);		// v = tmp3 % q
    result = bdCompare(v,r);		// compare (v, r)

    printf("@@SRM- W= ");
    bdPrint(w, BD_PRINT_TRIM | BD_PRINT_NL);
#if 0
    printf("@@SRM- u1= ");
    bdPrint(u1, BD_PRINT_TRIM | BD_PRINT_NL);
    printf("@@SRM- u2= ");
    bdPrint(u2, BD_PRINT_TRIM | BD_PRINT_NL);
    printf("@@SRM- G^u1= ");
    bdPrint(tmp1, BD_PRINT_TRIM | BD_PRINT_NL);
    printf("@@SRM- Y^u2= ");
    bdPrint(tmp2, BD_PRINT_TRIM | BD_PRINT_NL);
    printf("@@SRM- Product= ");
    bdPrint(tmp3, BD_PRINT_TRIM | BD_PRINT_NL);
#endif
    printf("@@SRM- V= ");
    bdPrint(v, BD_PRINT_TRIM | BD_PRINT_NL);

    bdFree(&w);
    bdFree(&u1);
    bdFree(&u2);
    bdFree(&v);
    bdFree(&tmp1);
    bdFree(&tmp2);
    bdFree(&tmp3);

    return result;			// 0: verified, non-zero: failed to verify
}

#endif

//------------------------------------------------------------------------------
/// @brief This routine update revocation list
/// @argument:
///              - *data: data point
///              -  size: amount of revoked key (size 1 = 1*5 bytes, size 2= 2*5 bytes)
/// @return:
//------------------------------------------------------------------------------
void MDrv_HDMITx_HDCP_RevocationKey_List(MS_U8 *data, MS_U16 size)
{
    gHDMITxInfo.revocation_size= size;
    memcpy(&gHDMITxInfo.hdcp_revocationlist[0], data, 5*size);
}

//------------------------------------------------------------------------------
/// @brief This function return revocation check result
/// @argument:
/// @return:
///              - E_CHECK_NOT_READY
///              - E_CHECK_REVOKED
///              - E_CHECL_NOT_REVOKED
//------------------------------------------------------------------------------
MDrvHDMITX_REVOCATION_STATE MDrv_HDMITx_HDCP_RevocationKey_Check(void)
{
    return gHDMITxInfo.revocation_state;
}
//------------------------------------------------------------------------------
/// @brief This routine check whether SRM DSA signauter is valid or not
/// @argument:
///              - *data: data point
///              -   size: size of SRM list(bytes)
/// @return:
///              Ture: valid, FALSE: invalid
//------------------------------------------------------------------------------
MS_BOOL MDrv_HDMITx_HDCP_IsSRMSignatureValid(MS_U8 *data, MS_U32 size)
{
#if 0
    MS_U8 Msg[8]; // message, hex format
    MS_U8 u8rtemp[20], u8stemp[20]; // DSA signature, hex format
    char ms[50];
    MS_U32 u32DSASigIdx;
    unsigned int SHA1[5];
    MS_S8 result;
    BIGD   m, r, s;
    dsa_context dsa;

    // ps, qs, gs is from HDCP specification v1.3
    // msg[] Source message (will be hashed by SHA-1)
    // ps[] (p) Prime Modulus, 1024 bits
    // qs[] (q) Prime Divisor, 160 bits
    // gs[] (g) Generator, 1024 bits
    // ys[] (y) Public key, 1024 bits
    char ps[] = "d3c3f5b2 fd1761b7 018d75f7 9343786b\
                17395b35 5a52c7b8 a1a24fc3 6a7058ff\
                8e7fa164 f500e0dc a0d28482 1d969e4b\
                4f34dc0c ae7c7667 b844c747 d4c6b983\
                e52ba70e 5447cf35 f404a0bc d1974c3a\
                10715509 b3721530 a73f3207 b9982049\
                5c7b9c14 3275733b 028a49fd 96891954\
                2a39951c 46edc211 8c59802b f3287527";
    char qs[] = "ee8af2ce 5e6db56a cd6d14e2 97ef3f4d f9c708e7";
    char gs[] = "92f85d1b 6a4d5213 1ae43e24 45de1ab5\
                02afdeac a9bed731 5d56d766 cd278611\
                8f5db14a bdeca9d2 5162977d a83effa8\
                8eedc6bf eb37e1a9 0e29cd0c a03d799e\
                92dd2945 f778585f f7c83564 2c21ba7f\
                b1a0b6be 81c8a5e3 c8ab69b2 1da54242\
                c98e9b8a ab4a9dc2 51fa7dac 29216fe8\
                b93f185b 2f67405b 69462442 c2ba0bd9";
    char ys[] = "c7060052 6ba0b086 3a80fbe0 a3acff0d\
                4f0d7665 8a1754a8 e7654755 f15ba78d\
                56950e48 654f0bbd e16804de 1b541874\
                db22e14f 031704db 8d5cb2a4 17c4566c\
                27ba973c 43d84e0d a2a70856 fe9ea48d\
                87259038 b16553e6 62435ff7 fd5206e2\
                7bb7ffbd 886c2410 95c8dc8d 66f662cb\
                d88f9df7 e9b3fb83 62a9f7fa 36e53799";

    m = bdNew();
    r = bdNew();
    s = bdNew();
    memcpy(&gHDMITxInfo.hdcp_srmlist[0], data, size);

    u32DSASigIdx = (gHDMITxInfo.hdcp_srmlist[5]<<16) | (gHDMITxInfo.hdcp_srmlist[6]<<8) | gHDMITxInfo.hdcp_srmlist[7];
    if(u32DSASigIdx < 43) // illegal length
        return FALSE;
    else
        u32DSASigIdx -= 43;

    memcpy(&Msg[0], &(gHDMITxInfo.hdcp_srmlist[0]), 8*sizeof(MS_U8)); // Message: The first 64 bits of SRM list
    memcpy(&u8rtemp[0], &(gHDMITxInfo.hdcp_srmlist[8+u32DSASigIdx]), 20*sizeof(MS_U8)); // r: the first 160 bits of DSA signature
    memcpy(&u8stemp[0], &(gHDMITxInfo.hdcp_srmlist[8+20+u32DSASigIdx]), 20*sizeof(MS_U8)); // s: the trailing 160 bits of DSA signature
    MHal_HDMITx_HDCP_SHA1_Transform(SHA1, Msg); // Msg: 64 bits -> 512bits, SHA-1(Msg)
    snprintf(ms, sizeof(ms), "%08x%08x%08x%08x%08x",SHA1[0],SHA1[1],SHA1[2],SHA1[3],SHA1[4]);
    //sprintf(ms,"%08x%08x%08x%08x%08x",SHA1[0],SHA1[1],SHA1[2],SHA1[3],SHA1[4]);

    _dsa_init(&dsa);

    bdConvFromHex(dsa.p, ps);
    bdConvFromHex(dsa.q, qs);
    bdConvFromHex(dsa.g, gs);
    bdConvFromHex(dsa.y, ys);
    bdConvFromHex(m,ms);
    bdConvFromOctets(r, u8rtemp, 20);
    bdConvFromOctets(s, u8stemp, 20);

    printf("@@SRM-SHA-1 Digest= ");
    bdPrint(m, BD_PRINT_TRIM | BD_PRINT_NL);
    printf("@@SRM-R= ");
    bdPrint(r, BD_PRINT_TRIM | BD_PRINT_NL);

    result = _dsa_verify(&dsa, m, r, s);

    printf("@@SRM- Verify result= ");
    result == 0 ? printf("PASS!\r\n") : printf("FAIL!\r\n");

    bdFree(&m);
    bdFree(&r);
    bdFree(&s);

    _dsa_clear(&dsa);

    if(result == 0)
        return TRUE;
    else
        return FALSE;
#endif
    return TRUE;
}

//  *** Sample empty SRM ***
//  0x80 0x00                                             // SRM ID and reserved bit-field
//  0x00 0x01                                             // SRM version 0001
//  0x00                                                     // SRM generator no
//  0x00 0x00 0x2B                                     // VRL length (in bytes) 40+3 bytes.
//  0xD2 0x48 0x9E 0x49 0xD0 0x57 0xAE 0x31 0x5B 0x1A 0xBC 0xE0 0x0E 0x4F 0x6B 0x92    // DCP LLC signature
//  0xA6 0xBA 0x03 0x3B 0x98 0xCC 0xED 0x4A 0x97 0x8F 0x5D 0xD2 0x27 0x29 0x25 0x19
//  0xA5 0xD5 0xF0 0x5D 0x5E 0x56 0x3D 0x0E
MS_BOOL MDrv_HDMITx_HDCP_CheckRevokedKey(void)
{
#if 1
    MS_U16 i;
    MS_U8 j, u8bksv[5];
    //MS_U32 u32keylen;
    // The revoked key from QD882: Bksv=0x23, 0xde, 0x5c, 0x43, 0x93
    MS_U8 u8Revokedksv[5] = {0x23, 0xDE, 0x5C, 0x43, 0x93};

    if(!MHal_HDMITx_HDCP_Get_BKSV(&u8bksv[0]))
    {
        gHDMITxInfo.revocation_state = E_CHECK_NOT_READY;
        return FALSE;
    }
    //if(gHDMITxInfo.hdcp_srmlist[0] != 0x80) // no SRM list
    if(!gHDMITxInfo.revocationlist_ready)
    {
    // HDCP CTS 1A-08
        for(i=0;i<5;i++)
        {
            if(u8bksv[i] != u8Revokedksv[i])
            {
                gHDMITxInfo.revocation_state = E_CHECK_NOT_REVOKED;
                return FALSE;
            }
        }
        DBG_HDCP(printf("MDrv_HDMITx_HDCP_CheckRevokedKey: Revoked Rx key!!\n"));
        gHDMITxInfo.revocation_state = E_CHECK_REVOKED;
        return TRUE;
    }
    else
    {
        // revoked key length
        //u32keylen = (gHDMITxInfo.hdcp_srmlist[5]<<16) | (gHDMITxInfo.hdcp_srmlist[6]<<8) | gHDMITxInfo.hdcp_srmlist[7];
        //u32keylen = (u32keylen >= 43) ? u32keylen-43 : 0; // 3 bytes is VRL length and 40 bytes is DCP LLC signature
        //if((u32keylen == 0) || (u32keylen%5 != 0)) // keylen is 0 or not 5 times
        if(gHDMITxInfo.revocation_size == 0) // keylen is 0 or not 5 times
        {
            gHDMITxInfo.revocation_state = E_CHECK_NOT_REVOKED;
            return FALSE;
        }
        else
        {
            for(i=0 ; i<gHDMITxInfo.revocation_size ; i++)
            {
                for(j=0; j<5; j++)
                {
                    if(gHDMITxInfo.hdcp_revocationlist[5*i+j] != u8bksv[j])
                    break;
                }
                if(j==5) // Bksv is matched the SRM revoked keys
                {
                    gHDMITxInfo.revocation_state = E_CHECK_REVOKED;
                    return TRUE;
                }
            }
        }
    }
#endif
    gHDMITxInfo.revocation_state = E_CHECK_NOT_REVOKED;
    return FALSE;
}

//------------------------------------------------------------------------------
/// @brief This routine return CHIP capability of DVI mode
/// @return TRUE, FALSE
//------------------------------------------------------------------------------
MS_BOOL MDrv_HDMITx_IsSupportDVIMode(void)
{
    return MHal_HDMITx_IsSupportDVIMode();
}

#ifdef CUSTOMER_NDS

void MDrv_HDMITx_EnableRxStatusChecking(MS_BOOL bflag)
{
    if(bflag)
    {
        if(_s32CheckRxTimerId < 0)
        {
            _s32CheckRxTimerId = MsOS_CreateTimer( _MDrv_HDMITX_CheckRx_TimerCallback,
                                                  CheckRx_Timer,
                                                  CheckRx_Timer,
                                                  TRUE,
                                                  "Check Rx timer");
            if(_s32CheckRxTimerId < 0)
            {
                DBG_HDMITX(printf("MDrv_HDMITx_EnableRxStatusChecking: Create Check Rx timer fail!\n"));
            }
            DBG_HDMITX(printf("MDrv_HDMITx_EnableRxStatusChecking: Create Check Rx timer success!\n"));
        }
        else
        {
            #ifdef CUSTOMER_NDS
            /*
             * Stop-then-start the timer. Starting a running timer causes
             * a kernel bug, to prevent from this case, we stop the timer,
             * then start it again.
             */
            MsOS_StopTimer(_s32CheckRxTimerId);
            #endif
            MsOS_StartTimer(_s32CheckRxTimerId);
        }
    }
    else
    {
          if(_s32CheckRxTimerId > 0)
            {
                MsOS_StopTimer(_s32CheckRxTimerId);
                DBG_HDMITX(printf("MDrv_HDMITx_EnableRxStatusChecking: Stop Check Rx timer!\n"));
            }
    }
}

MS_BOOL MDrv_HDMITx_EdidReadBlock(MS_U8 num, MS_U8 *data)
{
    return (MHal_HDMITx_EdidReadBlock(num, data));
}

void MDrv_HDMITx_Set_AVI_InfoFrame(MsHDMITX_AVI_CONTENT_TYPE content_type, MS_U16 *data)
{
    MHal_HDMITx_Set_AVI_InfoFrame(E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET, content_type, data);
}

void MDrv_HDMITx_Set_AVMUTE(MS_BOOL bflag)
{
    if(bflag)
        MDrv_HDMITx_SendPacket(E_HDMITX_GC_PACKET, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET|E_HDMITX_GCP_SET_AVMUTE);
    else
    {
        MDrv_HDMITx_SendPacket(E_HDMITX_GC_PACKET, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET|E_HDMITX_GCP_CLEAR_AVMUTE);
        MsOS_DelayTask(50);
        if(gHDMITxInfo.output_colordepth_val) // RX support DC
            MDrv_HDMITx_SendPacket(E_HDMITX_GC_PACKET, E_HDMITX_SEND_PACKET|E_HDMITX_CYCLIC_PACKET);
        else
            MDrv_HDMITx_SendPacket(E_HDMITX_GC_PACKET, E_HDMITX_STOP_PACKET);
    }
}

MS_BOOL MDrv_HDMITx_HDCP_Get_BCaps(MS_U8 *u8bcaps)
{
    return MHal_HDMITx_HDCP_Get_BCaps(u8bcaps);
}

MS_BOOL MDrv_HDMITx_HDCP_Get_BStatus(MS_U16 *u16bstatus)
{
    return MHal_HDMITx_HDCP_Get_BStatus(u16bstatus);
}

void MDrv_HDMITx_HDCP_Reset_SRM(void)
{
    memset(&gHDMITxInfo.hdcp_srmlist[0], 0, 5116*sizeof(MS_U8));
}

MS_BOOL MDrv_HDMITx_HDCP_Start_Authentication(void)
{
    gHDMITxInfo.hdmitx_HdcpStartAuth_flag = TRUE;
    gHDMITxInfo.hdmitx_HdcpAuthDone_flag = FALSE;
    gHDMITxInfo.hdmitx_HDCPAuth_Status = E_HDMITX_HDCP_RESET;
#ifdef CUSTOMER_NDS
    gHDMITxInfo.hdmitx_hdcp_flag = TRUE;
#endif
    MDrv_HDMITx_FSM_ChangeState(E_HDMITX_FSM_HDCP_AUTH_WAIT_RX);

    return TRUE;
}

MS_BOOL MDrv_HDMITx_HDCP_Stop_Authentication(void)
{
    gHDMITxInfo.hdmitx_HdcpStartAuth_flag = FALSE;
    gHDMITxInfo.hdmitx_HdcpAuthDone_flag = FALSE;
    gHDMITxInfo.hdmitx_HDCPAuth_Status = E_HDMITX_HDCP_RESET;
#ifdef CUSTOMER_NDS
    gHDMITxInfo.hdmitx_hdcp_flag = FALSE;
    if(_s32HDCPRiTimerId > 0) {
        if (MsOS_DeleteTimer(_s32HDCPRiTimerId))
            _s32HDCPRiTimerId = -1;
        else
            printf("delete RiTimer failed\n");
    }
    if(_s32CheckRxTimerId > 0) {
        if(MsOS_DeleteTimer(_s32CheckRxTimerId))
            _s32CheckRxTimerId = -1;
        else
            printf("delete RxTimer failed\n");
    }
#endif

    return TRUE;
}

void MDrv_HDMITx_HdcpSetEncrypt(MS_BOOL bflag)
{
    MHal_HDMITx_HdcpSetEncrypt(bflag);
}

void MDrv_HDMITx_HDCP_Get_HDCP_Status(MS_U8 *status)
{
    *status = HDMI_STATUS_NOT_AUTHENTICATED;
    if(gHDMITxInfo.hdmitx_fsm_state == E_HDMITX_FSM_HDCP_AUTH_FAIL)
        *status = HDMI_STATUS_NOT_AUTHENTICATED;
    else if(gHDMITxInfo.hdmitx_HDCPAuth_Status == E_HDMITX_HDCP_AUTH_DONE)
        *status = HDMI_STATUS_AUTHENTICATED;
    else if((gHDMITxInfo.hdmitx_HDCPAuth_Status == E_HDMITX_HDCP_TX_KEY_FAIL) || (gHDMITxInfo.hdmitx_HDCPAuth_Status == E_HDMITX_HDCP_RX_KEY_FAIL))
        *status = HDMI_STATUS_NOT_AUTHENTICATED_KSV_INVALID;
    else if(gHDMITxInfo.hdmitx_HDCPAuth_Status == E_HDMITX_HDCP_RX_KEY_REVOKED)
        *status = HDMI_STATUS_NOT_AUTHENTICATED_KSV_REVOKED;
    else if(gHDMITxInfo.hdmitx_HDCPAuth_Status == E_HDMITX_HDCP_SYNC_RI_FAIL)
        *status = HDMI_STATUS_NOT_AUTHENTICATED_LINK_INTEGRITY_FAILED;
    else if(gHDMITxInfo.hdmitx_HDCPAuth_Status == E_HDMITX_HDCP_REPEATER_TIMEOUT)
        *status = HDMI_STATUS_NOT_AUTHENTICATED_LINK_INTEGRITY_NO_ACK;
    if(gHDMITxInfo.hdmitx_fsm_state < E_HDMITX_FSM_HDCP_AUTH_WAIT_RX)
        *status = HDMI_STATUS_AUTHENTICATION_DISABLED;
}

void MDrv_HDMITx_HDCP_Configure(HDMIHDCPConfiguration *arg)
{
    gHDMITxInfo.hdcp_encryptionStartTime = arg->encryptionStartTime;
    gHDMITxInfo.hdcp_checkPjIntegrity = arg->checkPjIntegrity;
}

void MDrv_HDMITx_FSM_ChangeState(MDrvHDMITX_FSM_STATE state)
{
    gHDMITxInfo.hdmitx_fsm_state = state;
    MsOS_SetEvent(_s32HDMITxEventId, E_HDMITX_EVENT_RUN);
}
#endif

