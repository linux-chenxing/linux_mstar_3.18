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
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
//==============================================================================


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#define _DRV_CEC_C_

#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#else
#include <string.h>
#endif
#include "MsCommon.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "MsTypes.h"

#include "cec_hwreg_utility2.h"
#include "cec_Analog_Reg.h"

#include "apiCEC.h"
#include "drvCEC.h"
#include "drvMMIO.h"
#include "mhal_CEC.h"
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define CEC_DPUTSTR(str)        //printf(str)
#define CEC_DPRINTF(str, x)     //printf(str, x)

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

MS_CEC_INFO_LIST gCECInfo;

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

MS_U32 CEC_RIU_BASE;
InterruptNum gCECIRQ;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------



#if 0
extern void mdrv_dvi_clock_70mhz_swpatch1(INPUT_SOURCE_TYPE_t enInputSourceType); //ToDo
#endif

void _MDrv_CEC_init_riu_base(void)
{
    MS_U32 _XCRIUBaseAddress=0, u32NonPMBankSize = 0;
    MS_U32 _PMRIUBaseAddress=0, u32PMBankSize = 0;

    // get MMIO base
    if(MDrv_MMIO_GetBASE( &_XCRIUBaseAddress, &u32NonPMBankSize, MS_MODULE_XC ) != TRUE)
    {
        MS_CRITICAL_MSG(printf("CEC GetBase failed\n"));
        return;
    }
    else
    {
    }

    if(MDrv_MMIO_GetBASE( &_PMRIUBaseAddress, &u32PMBankSize, MS_MODULE_PM ) != TRUE)
    {
        MS_CRITICAL_MSG(printf("CEC GetBase failed\n"));
        return;
    }
    else
    {
    }

    mhal_CEC_init_riu_base(_XCRIUBaseAddress, _PMRIUBaseAddress);
}

#if !CEC_DEVICE_IS_SOURCE// TV series
static void _MDrv_CEC_RxIsr(InterruptNum eIntNum)
{
    UNUSED(eIntNum);

    // 20081226 - DVI+HDCP snow noise patch - start ...
    //DVICLOCK_PATH = 2;
#if 0
    mdrv_dvi_clock_70mhz_swpatch1(SYS_INPUT_SOURCE_TYPE(MAIN_WINDOW));    // DVI clock less than 70 MHz issue
#endif
    // 20081226 - DVI+HDCP snow noise patch - end ...
    if(ENABLE_CEC_INT)
    {
        MDrv_CEC_RxChkBuf();
    }
    MsOS_EnableInterrupt(gCECIRQ);
}
#endif // TV series

static void _MDrv_CEC_InitVariable(void)
{
    MS_U8 i, j;

    gCECInfo.CecFifoIdxS = 0;
    gCECInfo.CecFifoIdxE = 0;
    gCECInfo.bCecMsgCnt = 0;
    gCECInfo.fCecInitFinish = FALSE;
#if CEC_DEVICE_IS_SOURCE // STB series
    gCECInfo.MyLogicalAddress = E_LA_TUNER1;       // Tuner
    gCECInfo.MyPhysicalAddress[0] = 0x10;      //default (1,0,0,0)
    gCECInfo.MyPhysicalAddress[1] = 0x00;
    gCECInfo.MyDeviceType = E_DEVICE_TYPE_TUNER;    //Tuner device
#else // TV series
    gCECInfo.MyLogicalAddress = E_LA_TV;       //TV
    gCECInfo.MyPhysicalAddress[0] = 0x00;      //default (0,0,0,0) for TV
    gCECInfo.MyPhysicalAddress[1] = 0x00;
    gCECInfo.MyDeviceType = E_DEVICE_TYPE_TV;    //TV device
#endif
    gCECInfo.MyPowerStatus  = E_MSG_PWRSTA_STANDBY2ON;

    gCECInfo.ActiveLogicalAddress = E_LA_TV;
    gCECInfo.ActiveDeviceType = E_DEVICE_TYPE_RESERVED;
    gCECInfo.ActivePowerStatus = E_MSG_PWRSTA_STANDBY;
    gCECInfo.ActivePhysicalAddress[0] = 0x00;
    gCECInfo.ActivePhysicalAddress[1] = 0x00;
    gCECInfo.ActiveDeviceCECVersion = CEC_VERSION_13a;

    for(i=0;i<CEC_FIFO_CNT;i++)
    {
        gCECInfo.CecRxBuf[i].ucLength = 0;
        for(j=0;j<16;j++)
            gCECInfo.CecRxBuf[i].tRxData[j]= 0;
    }
    for(i=0;i<15;i++)
    {
        gCECInfo.CecDevicesExisted[i] = FALSE;
    }
}


static MS_U8 _MDrv_CEC_SendFrame(MS_U8 header, MS_U8 opcode, MS_U8* operand, MS_U8 len)
{
    return (mhal_CEC_SendFrame(header, opcode, operand, len));
}


//******************************************************************************
//
//  [Function Name]:
//      CecInQueueMsgApi
//  [Arguments]:
//      none
//  [Return]:
//      message count in CEC buffer queue
//*******************************************************************************
/*
MS_U8 CecInQueueMsgApi(void)
{
  return bCecMsgCnt;
}
*/



//******************************************************************************
//
//  [Function Name]:
//      CecPingApi
//  [Arguments]:
//      destination address (range:0x01~0x0E)
//  [Return]:
//      CEC_ERROR_CODE
//*******************************************************************************
CEC_ERROR_CODE _MDrv_CEC_PingApi(MsCEC_DEVICELA addr)
{
    MS_U8 res;
    CEC_ERROR_CODE error_code = E_CEC_FEATURE_ABORT;
    MS_U8* Oprend_ptr = NULL;

    if(addr>=0x0f)
    {
        error_code = E_CEC_FEATURE_ABORT;
        return error_code;
    }

    res = _MDrv_CEC_SendFrame( ((addr<<4)&0xF0) |(addr&0x0F), 0x00, Oprend_ptr, 0);
    CEC_DPRINTF("_MDrv_CEC_SendFrame: res = %u\n", res);

    if(res&E_CEC_TX_SUCCESS)
        error_code = E_CEC_TX_SUCCESS;
    else if(res&E_CEC_RF)
        error_code = E_CEC_RF;
    else if(res&E_CEC_LOST_ABT)
        error_code = E_CEC_LOST_ABT;
    else if(res&E_CEC_SYSTEM_BUSY)
        error_code = E_CEC_SYSTEM_BUSY;

    return error_code;
}


void _MDrv_CEC_FindMyAddress(void)
{
    CEC_DPUTSTR("\r\n CEC Find My address \r\n");
#if CEC_DEVICE_IS_SOURCE // STB series
    if(_MDrv_CEC_PingApi(E_LA_TUNER1) == E_CEC_RF)
    {
        gCECInfo.MyLogicalAddress = E_LA_TUNER1;
    }
    else if(_MDrv_CEC_PingApi(E_LA_TUNER2) == E_CEC_RF)
    {
        gCECInfo.MyLogicalAddress = E_LA_TUNER2;
    }
    else if(_MDrv_CEC_PingApi(E_LA_TUNER3) == E_CEC_RF)
    {
        gCECInfo.MyLogicalAddress = E_LA_TUNER3;
    }
    else if(_MDrv_CEC_PingApi(E_LA_TUNER4) == E_CEC_RF)
    {
        gCECInfo.MyLogicalAddress = E_LA_TUNER4;
    }
    else if(_MDrv_CEC_PingApi(E_LA_FREE_USE) == E_CEC_RF)
        gCECInfo.MyLogicalAddress = E_LA_FREE_USE;
    else
        gCECInfo.MyLogicalAddress = E_LA_TUNER1;
#else // TV series
    if(_MDrv_CEC_PingApi(E_LA_TV) == E_CEC_RF)
    {
        gCECInfo.MyLogicalAddress = E_LA_TV;
    }
    else if(_MDrv_CEC_PingApi(E_LA_FREE_USE) == E_CEC_RF)
        gCECInfo.MyLogicalAddress = E_LA_FREE_USE;
    else
        gCECInfo.MyLogicalAddress = E_LA_TV;
#endif // CEC_DEVICE_IS_SOURCE
    mhal_CEC_SetMyAddress((MS_U8)gCECInfo.MyLogicalAddress);

    CEC_DPRINTF("\r\nMy logical address=%x\r\n", gCECInfo.MyLogicalAddress);
}


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//**************************************************************************
//  [Function Name]:
//                  MDrv_CecExit()
//  [Description]
//                  Driver layer: CEC initiail sequence
//  [Arguments]:
//
//  [Return]:
//
//*************************************************************************

void MDrv_CEC_Exit(void)
{
    // Mask CEC interrupt
    mhal_CEC_INTEn(FALSE);

    // Release CEC
    MsOS_DetachInterrupt(gCECIRQ);
}
//**************************************************************************
//  [Function Name]:
//                  MDrv_CecInit()
//  [Description]
//                  Driver layer: CEC set own logical address
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void MDrv_CEC_SetMyLogicalAddress(MsCEC_DEVICELA myLA)
{
    gCECInfo.MyLogicalAddress = myLA ;
    mhal_CEC_SetMyAddress((MS_U8)gCECInfo.MyLogicalAddress);
}

//**************************************************************************
//  [Function Name]:
//                  MDrv_CEC_InitChip()
//  [Description]
//                  Driver layer: CEC initiail chip
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void MDrv_CEC_InitChip(MS_U32 u32XTAL_CLK_Hz)
{
    _MDrv_CEC_InitVariable();

    _MDrv_CEC_init_riu_base();

    mhal_CEC_Init(u32XTAL_CLK_Hz);

#if !CEC_DEVICE_IS_SOURCE // TV series
#if ENABLE_CEC_INT
    MsOS_AttachInterrupt(gCECIRQ, _MDrv_CEC_RxIsr);
    MsOS_EnableInterrupt(gCECIRQ);
#endif
#endif // #if CEC_DEVICE_IS_SOURCE

    gCECInfo.fCecInitFinish = TRUE;
    gCECInfo.MyPowerStatus  = E_MSG_PWRSTA_ON;
    CEC_DPUTSTR("Complete CEC Chip Initiation!!\r\n");
}

//**************************************************************************
//  [Function Name]:
//                  MDrv_CecInit()
//  [Description]
//                  Driver layer: CEC initiail sequence
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void MDrv_CEC_Init(MS_U32 u32XTAL_CLK_Hz)
{
    _MDrv_CEC_InitVariable();

    _MDrv_CEC_init_riu_base();

    mhal_CEC_Init(u32XTAL_CLK_Hz);

#if !CEC_DEVICE_IS_SOURCE // TV series
#if ENABLE_CEC_INT
    MsOS_AttachInterrupt(gCECIRQ, _MDrv_CEC_RxIsr);
    MsOS_EnableInterrupt(gCECIRQ);
#endif
#endif // #if !(defined(CHIP_U3) || defined(CHIP_U4) || defined(CHIP_K1) || defined(CHIP_K2)) // TV series

    _MDrv_CEC_FindMyAddress();       //logical address for this TV.

    gCECInfo.fCecInitFinish = TRUE;
    gCECInfo.MyPowerStatus  = E_MSG_PWRSTA_ON;
    CEC_DPUTSTR("Complete CEC Initiation!!\r\n");
}


//**************************************************************************
//  [Function Name]:
//                  MDrv_CecCheckExistedDevices()
//  [Description]
//                  Driver layer: Use to check the existed CEC devices currently
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void MDrv_CEC_CheckExistDevices(void)
{
    MS_U8 i, res;
    MS_U8* Oprend_ptr = NULL;

    CEC_DPUTSTR("\r\n Existed CEC device \r\n");
    for (i=E_LA_TV; i<E_LA_UNREGISTERED; i++)
    {
#if ENABLE_CUST01_CEC
#if CEC_DEVICE_IS_SOURCE // STB series
        if(!((i==E_LA_TV) ||(i==E_LA_AUDIO_SYS)) )
#else
        if(!((i==E_LA_RECORDER1) || (i==E_LA_TUNER1) || (i==E_LA_PLAYBACK1) || (i==E_LA_AUDIO_SYS) || (i==E_LA_PLAYBACK2) || (i==E_LA_PLYBACK3)))
#endif 
            continue;
#endif // #if ENABLE_CUST01_CEC
        CEC_DPRINTF("CEC CheckDev:%u\n",i);
        // Just request a response.
        res = _MDrv_CEC_SendFrame( ((gCECInfo.MyLogicalAddress<<4)&0xF0) |(i&0x0F), 0x00, Oprend_ptr, 0);
        CEC_DPRINTF("_MDrv_CEC_SendFrame: res = %u\n", res);
        if(res&E_CEC_TX_SUCCESS)
        {
            gCECInfo.CecDevicesExisted[i] = TRUE;
            gCECInfo.ActiveLogicalAddress = (MsCEC_DEVICELA)i;
            CEC_DPRINTF("\r\n DEVICE ID= %d \r\n", i);
        }
        else
        {
            gCECInfo.CecDevicesExisted[i] = FALSE;
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                   MDrv_CecRxChkBuf()
//  [Description]
//                   Driver layer: Use to retrieve CEC message and store into CEC Rx buffer
//  [Arguments]:
//
//  [Return]:
//                   TRUE: While get the new CEC message
//                   FALSE: No new CEC message
//**************************************************************************
MS_BOOL MDrv_CEC_RxChkBuf(void)  //don't place print message in this function
{
    MS_U8 i;
    MS_BOOL ret = FALSE;

    if(mhal_CEC_IsMessageReceived())
    {
        gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].ucLength = mhal_CEC_ReceivedMessageLen();
        if((gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].ucLength>1) && (gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].ucLength<17)) //1: polling message, 17: over size, not needed to handle
        {
            for(i = 0 ; i < gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].ucLength ; i++)
            {
                gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].tRxData[i] = mhal_CEC_GetMessageByte(i);
            }
            if( gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].tRxData[0] != mhal_CEC_HeaderSwap(gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].tRxData[0]) )
            {
                gCECInfo.bCecMsgCnt++;
                gCECInfo.CecFifoIdxE=((++gCECInfo.CecFifoIdxE)>=CEC_FIFO_CNT)? 0 : gCECInfo.CecFifoIdxE;
            }
	        else if( gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].tRxData[0] == 0xFF && gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].tRxData[1] == E_MSG_STANDBY)//12.04 Albert-PT
            {
                gCECInfo.bCecMsgCnt++;
                gCECInfo.CecFifoIdxE=((++gCECInfo.CecFifoIdxE)>=CEC_FIFO_CNT)? 0 : gCECInfo.CecFifoIdxE;
            }
	 }
        mhal_CEC_ClearRxStatus();
        ret = TRUE;
    }
#if ENABLE_CEC_MULTIPLE
    if(mhal_CEC_IsMessageReceived2())
    {
        gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].ucLength = mhal_CEC_ReceivedMessageLen2();
        if((gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].ucLength>1) && (gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].ucLength<17)) //1: polling message, 17: over size, not needed to handle
        {
            for(i = 0 ; i < gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].ucLength ; i++)
            {
                gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].tRxData[i] = mhal_CEC_GetMessageByte2(i);
            }
            if( gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].tRxData[0] != mhal_CEC_HeaderSwap(gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].tRxData[0]) )
            {
                gCECInfo.bCecMsgCnt++;
                gCECInfo.CecFifoIdxE=((++gCECInfo.CecFifoIdxE)>=CEC_FIFO_CNT)? 0 : gCECInfo.CecFifoIdxE;
            }
            else if( gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].tRxData[0] == 0xFF && gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxE].tRxData[1] == E_MSG_STANDBY)
            {
                gCECInfo.bCecMsgCnt++;
                gCECInfo.CecFifoIdxE=((++gCECInfo.CecFifoIdxE)>=CEC_FIFO_CNT)? 0 : gCECInfo.CecFifoIdxE;
            }
    }
        mhal_CEC_ClearRxStatus2();
        ret = TRUE;
    }
#endif
    return ret;
}

//**************************************************************************
//  [Function Name]:
//                   MDrv_CecTxApi()
//  [Description]
//                   Driver layer: Use to send CEC message
//  [Arguments]:
//                   dst_address: destination logical address
//                   msg:            CEC message
//                   operand_ptr: message parameters
//                   len:              parameter length
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MDrv_CEC_TxApi(MsCEC_DEVICELA dst_address, MsCEC_MSGLIST msg, MS_U8* operand_ptr, MS_U8 len)
{
    MS_U8 res, header;
    CEC_ERROR_CODE error_code = E_CEC_FEATURE_ABORT;
#if ENABLE_CEC_MULTIPLE
    header = ((gCECInfo.IniLogicalAddress<<4)&0xF0)|(dst_address&0x0F);
#else
    header = ((gCECInfo.MyLogicalAddress<<4)&0xF0)|(dst_address&0x0F);
#endif  



    res=_MDrv_CEC_SendFrame(header, msg, operand_ptr, len);
    CEC_DPRINTF("_MDrv_CEC_SendFrame: res = 0x[%x]\n", res);

    if(res&E_CEC_TX_SUCCESS)
        error_code = E_CEC_TX_SUCCESS;
    else if(res&E_CEC_RF)
        error_code = E_CEC_RF;
    else if(res&E_CEC_LOST_ABT)
        error_code = E_CEC_LOST_ABT;
    else if(res&E_CEC_SYSTEM_BUSY)
        error_code = E_CEC_SYSTEM_BUSY;

#if ENABLE_CUST01_CEC
    if( (msg==E_MSG_UI_PRESS) && (error_code==E_CEC_TX_SUCCESS) )
    {
        res=_MDrv_CEC_SendFrame(header, E_MSG_UI_RELEASE, operand_ptr, 0);
        CEC_DPRINTF("_MDrv_CEC_SendFrame: res = %u\n", res);

        if(res&E_CEC_TX_SUCCESS)
            error_code = E_CEC_TX_SUCCESS;
        else if(res&E_CEC_RF)
            error_code = E_CEC_RF;
        else if(res&E_CEC_LOST_ABT)
            error_code = E_CEC_LOST_ABT;
        else if(res&E_CEC_SYSTEM_BUSY)
            error_code = E_CEC_SYSTEM_BUSY;
    }
#endif
    return error_code;
}

//**************************************************************************
//  [Function Name]:
//                   MDrv_CecCheckFrame()
//  [Description]
//                   Driver layer: Use to check CEC frame is valid or not
//  [Arguments]:
//                   msg_type:    CEC message type
//                   uclen:           message length, include header and opcode
//  [Return]:
//                   True or False
//**************************************************************************
MS_BOOL MDrv_CEC_CheckFrame(MsCEC_MSG_TRANS_TYPE msg_type, MS_U8 ucLen)
{
    MS_BOOL a, b;

    a = (gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxS].tRxData[0]&0x0F) == 0x0F;    //broadcast case or not (addressed)
    b = (gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxS].ucLength) >= ucLen;          //para length same or bigger than want (spec)

   // CEC_DPRINTF("\r\n CEC Message Length= %d\r\n", gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxS].ucLength);

    if((gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxS].tRxData[0]&0xF0) == 0xF0)     //message from unregistered device
    {
        switch(gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxS].tRxData[1])  //opcode
        {
            case E_MSG_RC_INACTIVE_SOURCE:
            case E_MSG_RC_REQ_ACTIVE_SOURCE:
            case E_MSG_RC_ROUTING_CHANGE:
            case E_MSG_RC_ROUTING_INFO:
            case E_MSG_RC_SET_STREM_PATH:

            case E_MSG_STANDBY:               //=> system standby
            // following command invoke broadcast response
            case E_MSG_SI_REQUEST_PHY_ADDR:      //=> Report_Physical_Address
            case E_MSG_SI_GET_MENU_LANGUAGE:     //=> Set_Menu_Language
            case E_MSG_VS_GIVE_VENDOR_ID:        //=> Device_Vendor_ID
                break;
            default:
                return (FALSE);
                break;
        }
    }


    switch(msg_type)
    {
        case E_TRANS_BROADCAST_MSG:
            return (a&b);
            break;
        case E_TRANS_DIRECT_MSG:
            return ((!a)&b);
            break;
        case E_TRANS_BOTHTYPE_MSG:
            return (b);
            break;
    }
    return (FALSE);
}

void MDrv_CEC_ConfigWakeUp(void)
{
    mhal_CEC_ConfigWakeUp();
}

void MDrv_CEC_Enabled(MS_BOOL bEnableFlag)
{
    if(bEnableFlag)
    {
	    mhal_CEC_ClearRxStatus();
#if ENABLE_CEC_MULTIPLE
        mhal_CEC_ClearRxStatus2();
#endif
        mhal_CEC_INTEn(TRUE);
        mhal_CEC_Enabled(TRUE);
    }
    else
    {
        mhal_CEC_INTEn(FALSE);
        mhal_CEC_Enabled(FALSE);
    }
}

MS_U8 MDrv_CEC_TxStatus(void)
{
    return mhal_CEC_TxStatus();
}

MS_BOOL MDrv_CEC_Device_Is_Tx(void)
{
    return mhal_CEC_Device_Is_Tx();
}

#if ENABLE_CEC_MULTIPLE
void MDrv_CEC_SetMyLogicalAddress2(MsCEC_DEVICELA myLA)
{
    gCECInfo.MyLogicalAddress2 = myLA ;
    mhal_CEC_SetMyAddress2((MS_U8)gCECInfo.MyLogicalAddress2);
}
#endif





