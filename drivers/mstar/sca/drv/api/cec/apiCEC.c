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
/// file    msAPI_CEC.c
/// @author MStar Semiconductor Inc.
/// @brief  HDMI CEC API Interface
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

#define _APICEC_C_

#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#else
#include <string.h>
#endif
#include "MsCommon.h"
#include "MsVersion.h"
#include "MsOS.h"
#include "MsTypes.h"

#include "apiCEC.h"
#include "drvCEC.h"

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define CEC_DPUTSTR(str)        //MS_DEBUG_MSG(do( if(_u16CEC_DBG_SWITCH == TRUE ) printf(str);) while(0);)
#define CEC_DPRINTF(str, x)     //MS_DEBUG_MSG(do {if (_u16CEC_DBG_SWITCH == TRUE ) printf(str, x);} while(0);)


//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

extern MS_CEC_INFO_LIST gCECInfo;


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
static MSIF_Version _api_cec_version = {
    { CEC_API_VERSION },
};

#else
static MSIF_Version _api_cec_version = {
    .DDI = { CEC_API_VERSION },
};
#endif

static MS_CEC_Status _cec_status = {.bIsInitialized = FALSE, .bIsRunning = FALSE,};
static MS_U16 _u16CEC_DBG_SWITCH = FALSE;
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CECNextDevice()
//  [Description]
//                   Set logical address to next active device
//  [Arguments]:
//
//  [Return]:
//                   Nect CEC active device's logical address
//**************************************************************************
MsCEC_DEVICELA MApi_CEC_NextDevice(void)
{
    MS_U8 i;

    MDrv_CEC_CheckExistDevices();
    for (i=gCECInfo.ActiveLogicalAddress+1; i<gCECInfo.ActiveLogicalAddress+15 && (i < E_LA_UNREGISTERED) ; i++)
    {
      #if 0
        if(gCECInfo.CecDevicesExisted[i%16] && (i!=E_LA_TV) && (i!=E_LA_UNREGISTERED) && (i!=E_LA_FREE_USE))
      #else // modifed it by coverity _555
        if(gCECInfo.CecDevicesExisted[i] && (i!=E_LA_TV) && (i!=E_LA_FREE_USE))
      #endif
        {
            return (MsCEC_DEVICELA)i;
        }
    }
    return gCECInfo.ActiveLogicalAddress;
}


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------


MS_BOOL MApi_CEC_GetLibVer(const MSIF_Version **ppVersion)              ///< Get version
{
    if (!ppVersion)
    {
        return FALSE;
    }

    *ppVersion = &_api_cec_version;
    return TRUE;
}

MS_BOOL  MApi_DDC2BI_GetInfo(MS_CEC_INFO_LIST *pInfo)
{
     if (!pInfo)
    {
        return FALSE;
    }

    memcpy((void*)pInfo, (void*)&gCECInfo, sizeof(MS_CEC_INFO_LIST));
    return TRUE;

}

MS_BOOL  MApi_DDC2BI_GetStatus(MS_CEC_Status *pStatus)
{
    if (!pStatus)
    {
        return FALSE;
    }

    memcpy((void*)pStatus, (void*)&_cec_status, sizeof(MS_CEC_Status ));
    return TRUE;
}

MS_BOOL  MApi_DDC2BI_SetDbgLevel(MS_U16 u16DbgSwitch)
{
    _u16CEC_DBG_SWITCH = u16DbgSwitch;
    //_u16CEC_DBG_SWITCH = _u16CEC_DBG_SWITCH;
    return TRUE;
}

void MApi_CEC_Exit(void)
{
    MDrv_CEC_Exit();
}
//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecInitChip()
//  [Description]
//                  CEC chip relatived part initial
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void MApi_CEC_InitChip(MS_CEC_INIT_INFO *pCECInfo)
{
    MDrv_CEC_InitChip(pCECInfo->u32XTAL_CLK_Hz);
}

//**************************************************************************
//  [Function Name]:
//                   MApi_CEC_SetMyLogicalAddress()
//  [Description]
//                  CEC set own logical address
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************

void MApi_CEC_SetMyLogicalAddress(MsCEC_DEVICELA myLA)
{
    MDrv_CEC_SetMyLogicalAddress(myLA);
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecInit()
//  [Description]
//                  CEC initial sequence
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void MApi_CEC_Init(MS_CEC_INIT_INFO *pCECInfo)
{

    MsCEC_DEVICELA i;
#if !ENABLE_SW_CEC_WAKEUP
    MS_U8 u8PowerOnMode = 0;
#endif
#if ENABLE_CUST01_CEC
    MS_BOOL bCECIsTx = MDrv_CEC_Device_Is_Tx();
#endif


    MDrv_CEC_Init(pCECInfo->u32XTAL_CLK_Hz);
    MDrv_CEC_CheckExistDevices();

    // report power status to the active CEC devices
    for (i=E_LA_TV; i<E_LA_UNREGISTERED; i++)
    {
#if ENABLE_CUST01_CEC
    if(bCECIsTx)
    {
        if(!((i==E_LA_TV) ||(i==E_LA_AUDIO_SYS)) )
            continue;
    }
	else
    {
        if(!((i==E_LA_RECORDER1) || (i==E_LA_TUNER1) || (i==E_LA_PLAYBACK1) || (i==E_LA_AUDIO_SYS) || (i==E_LA_PLAYBACK2) || (i==E_LA_PLYBACK3)))
            continue;
    }
#endif // #if ENABLE_CUST01_CEC

        if(gCECInfo.CecDevicesExisted[i])
        {
            MApi_CEC_Msg_ReportPowerStatus(i); // report TV power status
            //MApi_CEC_Msg_ReqPowerStatus(i); // request existed devices power status
        }
    }
    //some dvd player may send the active source too early,
    //so we add here another request active source to get the proper cmd from player
    //to get the correct the HDMI port to switch to.
#if !ENABLE_SW_CEC_WAKEUP
    mdrv_pm_ctrl_read(CTRL_READ_CMD, CP_READ_POWERON_MODE, 1, &u8PowerOnMode);
    if(u8PowerOnMode == M_POWER_ON_BY_HDMI_CEC)
    {
        CEC_DPUTSTR("cec wake up, then req active source\n");
        MApi_CEC_Msg_ReqActiveSource();
    }
#endif
}


//**************************************************************************
//  [Function Name]:
//                  MsAPI_CecRxChkBuf()
//  [Description]
//                  Use to store CEC received messages
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void MApi_CEC_ChkRxBuf(void)
{
    MDrv_CEC_RxChkBuf();
}


//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecTxSendMsg()
//  [Description]
//                   Use to send any CEC message by user defined
//  [Arguments]:
//                   dst_address: destination logical address
//                   msg:            CEC message
//                   cmd:            message parameters
//                   len:              parameter length
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_TxSendMsg(MsCEC_DEVICELA dst_address, MsCEC_MSGLIST msg, MS_U8 *cmd, MS_U8 len)
{
    CEC_ERROR_CODE res;
    res = MDrv_CEC_TxApi(dst_address, msg, cmd, len);

    return res;
}


//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecTxSendPollingMsg()
//  [Description]
//                   Use to send any CEC polling msg by user defined
//  [Arguments]:
//                   dst_address: destination logical address
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MsAPI_CecTxSendPollingMsg(MsCEC_DEVICELA dst_address)
{
    CEC_ERROR_CODE res;
    res = MDrv_CEC_TxApi(dst_address, (MsCEC_MSGLIST)0x00, NULL, 0x00);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_ActiveSource()
//  [Description]
//                   Used by a new source to indicate that it has started to transmit a stream or
//                   used in response to a <Request Active Source>
//  [Message property]
//                   Address:  Broadcast
//                   Initiator:  All sources
//                   Follower:  TV, CEC Switches
//  [Arguments]:
//
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_ActiveSource(void)
{
    CEC_ERROR_CODE res;
    MS_U8 txbuf[2];

    txbuf[0] = gCECInfo.MyPhysicalAddress[0];
    txbuf[1] = gCECInfo.MyPhysicalAddress[1];

    res = MDrv_CEC_TxApi(E_LA_BROADCAST, E_MSG_ACTIVE_SOURCE, txbuf, 2);

    return res;
}


//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_RoutingChange()
//  [Description]
//                   Used by a CEC switch when it's manually switched to inform akk other devices.
//  [Message property]
//                   Address:  Broadcast
//                   Initiator:  CEC switches and TV(with 2 or more HDMI inputs)
//                   Follower:  CEC switches
//  [Arguments]:
//                   orig_address: The original physicall address
//                   new_address: The new physical address
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_RoutingChange(MS_U8* orig_address, MS_U8* new_address)
{
    CEC_ERROR_CODE res;
    MS_U8 txbuf[4];

    txbuf[0] = orig_address[0];
    txbuf[1] = orig_address[1];
    txbuf[2] = new_address[0];
    txbuf[3] = new_address[1];

    res = MDrv_CEC_TxApi(E_LA_BROADCAST, E_MSG_RC_ROUTING_CHANGE, txbuf, 4);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_ReqActiveSource()
//  [Description]
//                   Used by a new device to discover the status of the system.
//  [Message property]
//                   Address:  Broadcast
//                   Initiator:
//                   Follower:  All, except for CEC Switches and devices which cannot become a source.
//  [Arguments]:
//
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_ReqActiveSource(void)
{
    CEC_ERROR_CODE res;

    res = MDrv_CEC_TxApi(E_LA_BROADCAST, E_MSG_RC_REQ_ACTIVE_SOURCE, (MS_U8*)E_MSG_FEATURE_ABORT, 0);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_SetStreamPath()
//  [Description]
//                   Used by the TV to request a streaming path from the specified physical address.
//  [Message property]
//                   Address:  Broadcast
//                   Initiator:
//                   Follower:  CEC Switches
//  [Arguments]:
//                   new_address: The new physical address
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_SetStreamPath(MS_U8* new_address)
{
    CEC_ERROR_CODE res;
    MS_U8 txbuf[2];

    txbuf[0] = new_address[0];
    txbuf[1] = new_address[1];

    res = MDrv_CEC_TxApi(E_LA_BROADCAST, E_MSG_RC_SET_STREM_PATH, txbuf, 2);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_Standby()
//  [Description]
//                   Use to switch one or all devices into standby mode.
//                   One device: it's specific logical address, like "E_LA_RECORDER1"
//                   All device:   Use broadcast logical address, "E_LA_BROADCAST"
//  [Message property]
//                   Address:  Directly / Broadcast
//                   Initiator:  TV(Broadcast Address)
//                   Follower:  All
//  [Arguments]:
//                   dst_address: destination logical address
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_Standby(MsCEC_DEVICELA dst_address)
{
    CEC_ERROR_CODE res;
    res = MDrv_CEC_TxApi(dst_address, E_MSG_STANDBY, (MS_U8*)E_MSG_FEATURE_ABORT, 0);

    gCECInfo.MyPowerStatus = E_MSG_PWRSTA_STANDBY;
    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_RecordOff()
//  [Description]
//                   Requests a device to stop a recording.
//  [Message property]
//                   Address:  Directly
//                   Initiator:  Device Initiating a recording
//                   Follower:  Recording Device if implementing <Record On>
//  [Arguments]:
//                   dst_address: destination logical address
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_RecordOff(MsCEC_DEVICELA dst_address)
{
    CEC_ERROR_CODE res;
    res = MDrv_CEC_TxApi(dst_address, E_MSG_OTR_RECORD_OFF, (MS_U8*)E_MSG_FEATURE_ABORT, 0);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_RecordOn()
//  [Description]
//                   Attempt to record the specified source.
//  [Message property]
//                   Address:  Directly
//                   Initiator:
//                   Follower:
//  [Arguments]:
//                   dst_address: destination logical address
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MsAPI_CEC_Msg_RecordOn(MsCEC_DEVICELA dst_address, MsCEC_MSG_RECORD_SOURCE_TYPE cmd)
{
    CEC_ERROR_CODE res;
    res = MDrv_CEC_TxApi(dst_address, E_MSG_OTR_RECORD_ON, (MS_U8*)&cmd, 1);

    return res;
}


//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_ReportCECVersion()
//  [Description]
//                   Used to indicate the supported CEC version, in response to a <Get CEC Version>
//  [Message property]
//                   Address:  Directly
//                   Initiator:  All
//                   Follower:  All
//  [Arguments]:
//                   dst_address: destination logical address
//                   version: CEC version
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_ReportCECVersion(MsCEC_DEVICELA dst_address, MS_U8 version)
{
    CEC_ERROR_CODE res;

    res = MDrv_CEC_TxApi(dst_address, E_MSG_SI_CEC_VERSION, &version, 1);

    return res;
}


//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_ReqCECVersion()
//  [Description]
//                   Used by a device to enquire which version of CEC the target supports
//  [Message property]
//                   Address:  Directly
//                   Initiator:  All
//                   Follower:  All
//  [Arguments]:
//                   dst_address: destination logical address
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_ReqCECVersion(MsCEC_DEVICELA dst_address)
{
    CEC_ERROR_CODE res;

    res = MDrv_CEC_TxApi(dst_address, E_MSG_SI_GET_CEC_VERSION, (MS_U8*)E_MSG_FEATURE_ABORT, 0);

    return res;
}


//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_ReportPhycalAddress()
//  [Description]
//                   Use to inform all other devices of the mapping between physical and logical address of the initiator
//  [Message property]
//                   Address:  Broadcast
//                   Initiator:  All
//                   Follower:  TV
//  [Arguments]:
//
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_ReportPhycalAddress(void)
{
    CEC_ERROR_CODE res;
    MS_U8 txbuf[3];

    txbuf[0] = gCECInfo.MyPhysicalAddress[0];
    txbuf[1] = gCECInfo.MyPhysicalAddress[1];
    txbuf[2] = gCECInfo.MyDeviceType;

    res = MDrv_CEC_TxApi(E_LA_BROADCAST, E_MSG_SI_REPORT_PHY_ADDR, txbuf, 3);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_ReqPhycalAddress()
//  [Description]
//                   A request to a device to return its physical address
//  [Message property]
//                   Address:  Directly
//                   Initiator:
//                   Follower:  All, except for CEC Switches using logical address 15
//  [Arguments]:
//                   dst_address: destination logical address
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_ReqPhycalAddress(MsCEC_DEVICELA dst_address)
{
    CEC_ERROR_CODE res;
    res = MDrv_CEC_TxApi(dst_address, E_MSG_SI_REQUEST_PHY_ADDR, (MS_U8*)E_MSG_FEATURE_ABORT, 0);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_DeckControl()
//  [Description]
//                   Use to control a device's media functions
//  [Message property]
//                   Address:  Directly
//                   Initiator:
//                   Follower:
//  [Arguments]:
//                   dst_address: destination logical address
//                   cmd:      deck control mode
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_DeckControl(MsCEC_DEVICELA dst_address, MsCEC_MSG_DC_DECK_CTRL_PARM cmd)
{
    CEC_ERROR_CODE res;
    res = MDrv_CEC_TxApi(dst_address, E_MSG_DC_DECK_CTRL, (MS_U8*)&cmd, 1);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_DeckStatus()
//  [Description]
//                   Use to provide a device's status when received "Give Deck Status"
//  [Message property]
//                   Address:  Directly
//                   Initiator:
//                   Follower:
//  [Arguments]:
//                   dst_address: destination logical address
//                   cmd:      deck info mode
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_DecStatus(MsCEC_DEVICELA dst_address, MsCEC_MSG_DC_DECK_STATUS_PARM cmd)
{
    CEC_ERROR_CODE res;
    res = MDrv_CEC_TxApi(dst_address, E_MSG_DC_DECK_STATUS, (MS_U8*)&cmd, 1);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_GiveDeckStatus()
//  [Description]
//                   Use to request device's status
//  [Message property]
//                   Address:  Directly
//                   Initiator:
//                   Follower:
//  [Arguments]:
//                   dst_address: destination logical address
//                   cmd:      status request mode
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_MSg_GiveDeckStatus(MsCEC_DEVICELA dst_address, MsCEC_MSG_STATUS_REQUEST_PARM cmd)
{
    CEC_ERROR_CODE res;
    res = MDrv_CEC_TxApi(dst_address, E_MSG_DC_GIVE_DECK_STATUS, (MS_U8*)&cmd, 1);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_DCPlay()
//  [Description]
//                   Use to control the playback behaviour of a source device
//  [Message property]
//                   Address:  Directly
//                   Initiator:
//                   Follower:
//  [Arguments]:
//                   dst_address: destination logical address
//                   cmd:      play mode
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_MSg_DCPlay(MsCEC_DEVICELA dst_address, MsCEC_MSG_DC_PLAY_PARM cmd)
{
    CEC_ERROR_CODE res;
    res = MDrv_CEC_TxApi(dst_address, E_MSG_DC_PLAY, (MS_U8*)&cmd, 1);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_ReqMenuStatus()
//  [Description]
//                   Request from TV for a device to show/remove a menu or to query status
//  [Message property]
//                   Address:  Directly
//                   Initiator:
//                   Follower:
//  [Arguments]:
//                   dst_address: destination logical address
//                   cmd: menu request type
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_ReqMenuStatus(MsCEC_DEVICELA dst_address, MsCEC_MSG_MENU_REQ_TYPE cmd)
{
    CEC_ERROR_CODE res;

    res = MDrv_CEC_TxApi(dst_address, E_MSG_DMC_MENU_REQUEST, (MS_U8*)&cmd, 1);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_UserCtrlPressed()
//  [Description]
//                   Use to indicated that user pressed which control button
//  [Message property]
//                   Address:  Directly
//                   Initiator:
//                   Follower:
//  [Arguments]:
//                   dst_address: destination logical address
//                   cmd: UI command
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_UserCtrlPressed(MS_BOOL bUserCtrlEn, MsCEC_DEVICELA dst_address, MsCEC_MSG_USER_CTRL_PARM cmd)
{
    CEC_ERROR_CODE res;

    if(bUserCtrlEn) // menu status is activated
        res = MDrv_CEC_TxApi(dst_address, E_MSG_UI_PRESS, (MS_U8*)&cmd, 1);
    else // if menu status ia deactivated, send request to active device
        res = MApi_CEC_Msg_ReqMenuStatus(dst_address, E_MSG_MENU_REQ_ACTIVATED);
    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_UserCtrlReleased()
//  [Description]
//                   Use to indicated that user released which control button
//  [Message property]
//                   Address:  Directly
//                   Initiator:
//                   Follower:
//  [Arguments]:
//                   dst_address: destination logical address
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_UserCtrlReleased(MsCEC_DEVICELA dst_address)
{
    CEC_ERROR_CODE res;
    res = MDrv_CEC_TxApi(dst_address, E_MSG_UI_RELEASE, (MS_U8*)E_MSG_FEATURE_ABORT, 0);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MApi_CEC_Msg_GiveAudioStatus()
//  [Description]
//                   Use to request an Amplifier to send its volume and mute status
//  [Message property]
//                   Address:  Directly
//                   Initiator:  All (except CEC switches)
//                   Follower:
//  [Arguments]:
//                   dst_address: destination logical address
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_GiveAudioStatus(MsCEC_DEVICELA dst_address)
{
    CEC_ERROR_CODE res;
    res = MDrv_CEC_TxApi(dst_address, E_MSG_SAC_GIVE_AUDIO_STATUS, (MS_U8*)E_MSG_FEATURE_ABORT, 0);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_ReportPowerStatus()
//  [Description]
//                   Use to inform a requesting device of the current power status
//  [Message property]
//                   Address:  Directly
//                   Initiator:  All (except CEC switches)
//                   Follower:
//  [Arguments]:
//                   dst_address: destination logical address
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_ReportPowerStatus(MsCEC_DEVICELA dst_address)
{
    CEC_ERROR_CODE res;

    CEC_DPRINTF("*CEC*:Report My PowerSts: 0x%lx\n", (MS_U32) &(gCECInfo.MyPowerStatus));
    res = MDrv_CEC_TxApi(dst_address, E_MSG_PS_REPORT_POWER_STATUS, (MS_U8*)&(gCECInfo.MyPowerStatus), 1);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_ReqPowerStatus()
//  [Description]
//                   Use to determine the current power status of a target device
//  [Message property]
//                   Address:  Directly
//                   Initiator:
//                   Follower:  All (except CEC switches)
//  [Arguments]:
//                   dst_address: destination logical address
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_ReqPowerStatus(MsCEC_DEVICELA dst_address)
{
    CEC_ERROR_CODE res;

    res = MDrv_CEC_TxApi(dst_address, E_MSG_PS_GIVE_POWER_STATUS, (MS_U8*)E_MSG_FEATURE_ABORT, 0);
    CEC_DPRINTF("*CEC*:Req PowerSts: 0x%x\n", (MS_U16)res);
    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_FeatureAbort()
//  [Description]
//                   Use to indicate that device doesn't support message type
//  [Message property]
//                   Address:  Directly
//                   Initiator:  Generate if a message is not supported
//                   Follower:  All
//  [Arguments]:
//                   dst_address: destination logical address
//                   msg: the requested message type
//                   cmd: unsupport reason
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_FeatureAbort(MsCEC_DEVICELA dst_address, MsCEC_MSGLIST msg, MsCEC_MSG_ABORT_REASON cmd)
{
    CEC_ERROR_CODE res;
    MS_U8 txbuf[2];

    txbuf[0] = msg;
    txbuf[1] = cmd;
    res = MDrv_CEC_TxApi(dst_address, E_MSG_FEATURE_ABORT, txbuf, 2);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_Abort()
//  [Description]
//                   This message is reserved for testing purposes.
//  [Message property]
//                   Address:  Directly
//                   Initiator:
//                   Follower:  All except for CEC switches
//  [Arguments]:
//                   dst_address: destination logical address
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_Abort(MsCEC_DEVICELA dst_address)
{
    CEC_ERROR_CODE res;

    res = MDrv_CEC_TxApi(dst_address, E_MSG_ABORT_MESSAGE, (MS_U8*)E_MSG_FEATURE_ABORT, 0);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MApi_CEC_Msg_SendMenuLanguage()
//  [Description]
//                   Used by a CEC switch when it's manually switched language to inform akk other devices.
//  [Message property]
//                   Address:  Broadcast
//                   Initiator:  CEC switches and TV(with 2 or more HDMI inputs)
//                   Follower:  CEC switches
//  [Arguments]:
//                   MenuLanguageCode: iso639-2 Language Code, 3 bytes
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MApi_CEC_Msg_SendMenuLanguage(MS_U8* pu8MenuLanguageCode)
{
    CEC_ERROR_CODE res;
    MS_U8 txbuf[3];

    txbuf[0] = pu8MenuLanguageCode[0];
    txbuf[1] = pu8MenuLanguageCode[1];
    txbuf[2] = pu8MenuLanguageCode[2];

    res = MDrv_CEC_TxApi(E_LA_BROADCAST, E_MSG_SI_SET_MENU_LANGUAGE, txbuf, 3);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_ReqARCInitiation()
//  [Description]
//                   Used by an ARC TX device to request an ARC RX device to active
//                   the ARC functionility in ARC TX device
//  [Message property]
//                   Address:  Directly
//                   Initiator:
//                   Follower:  ARC RX device
//  [Arguments]:
//                   dst_address: destination logical address
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MsAPI_CecMsg_ReqARCInitiation(MsCEC_DEVICELA dst_address)
{
    CEC_ERROR_CODE res;

    res = MDrv_CEC_TxApi(dst_address, E_MSG_ARC_REQUEST_ARC_INITATION, (MS_U8*)E_MSG_FEATURE_ABORT, 0);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_ReqARCTermination()
//  [Description]
//                   Used by an ARC TX device to request an ARC RX device to deactive
//                   the ARC functionility in ARC TX device
//  [Message property]
//                   Address:  Directly
//                   Initiator:
//                   Follower:  ARC RX device
//  [Arguments]:
//                   dst_address: destination logical address
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MsAPI_CecMsg_ReqARCTermination(MsCEC_DEVICELA dst_address)
{
    CEC_ERROR_CODE res;

    res = MDrv_CEC_TxApi(dst_address, E_MSG_ARC_REQUEST_ARC_TERMINATION, (MS_U8*)E_MSG_FEATURE_ABORT, 0);

    return res;
}

//**************************************************************************
//  [Function Name]:
//                   MsAPI_CecMsg_AudioModeReq()
//  [Description]
//                   A device implementing System Audio Control and which has volume control
//                   RC buttons request to use System Audio Mode to the Amplifier
//  [Message property]
//                   Address:  Directly
//                   Initiator:
//                   Follower:
//  [Arguments]:
//                   dst_address: destination logical address
//                   amp_switch: 1: unmute amplifier. 0: mute amplifier
//                   my_address: source to be used is the device specified at this address
//  [Return]:
//                   error_code: return status
//**************************************************************************
CEC_ERROR_CODE MsAPI_CecMsg_AudioModeReq(MsCEC_DEVICELA dst_address, MS_BOOL amp_switch, MS_U8* my_address )
{
    CEC_ERROR_CODE res;
    MS_U8 txbuf[2];

    if(amp_switch)
    {
        txbuf[0] = my_address[0];
        txbuf[1] = my_address[1];
        res = MDrv_CEC_TxApi(dst_address, E_MSG_SAC_SYSTEM_AUDIO_MODE_REQUEST, txbuf, 2);
    }
    else
        res = MDrv_CEC_TxApi(dst_address, E_MSG_SAC_SYSTEM_AUDIO_MODE_REQUEST, (MS_U8*)E_MSG_FEATURE_ABORT, 0);

    return res;
}

MS_U8 MApi_CEC_Get_Header(void)
{
    return gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxS].tRxData[0];
}

MS_U8 MApi_CEC_Get_OpCode(void)
{
    return gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxS].tRxData[1];
}

MS_U8 MApi_CEC_Get_Para(MS_U8 u8Idx)
{
    if(u8Idx+2 > 15)
        return gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxS].tRxData[0];
    else
        return gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxS].tRxData[u8Idx+2];
}

MS_U8 MApi_CEC_GetCmdLen(void)
{
    return  gCECInfo.CecRxBuf[gCECInfo.CecFifoIdxS].ucLength;
}

MS_BOOL MApi_CEC_IsRxBufEmpty(void)
{
    if(gCECInfo.bCecMsgCnt == 0 )
        return TRUE;
    else
        return FALSE;
}

void MApi_CEC_SetActiveLogicalAddress(MsCEC_DEVICELA Addr)
{
    gCECInfo.ActiveLogicalAddress = Addr;
}

MsCEC_DEVICELA MApi_CEC_GetActiveLogicalAddress(void)
{
    return gCECInfo.ActiveLogicalAddress;
}

MsCEC_MSG_POWER_STATUS_PARM MApi_CEC_GetPowerStatus(void)
{
    return gCECInfo.MyPowerStatus;
}

MS_U8 MApi_CEC_GetFifoIdx(void)
{
    return gCECInfo.CecFifoIdxS;
}

void MApi_CEC_SetFifoIdx(MS_U8 u8Idx)
{
    gCECInfo.CecFifoIdxS = u8Idx;
}

void MApi_CEC_SetActivePowerStatus(MsCEC_MSG_POWER_STATUS_PARM Status)
{
    gCECInfo.ActivePowerStatus = Status;
}

MsCEC_MSG_POWER_STATUS_PARM MApi_CEC_GetActivePowerStatus(void)
{
    return gCECInfo.ActivePowerStatus;
}

void MApi_CEC_SetActivePhysicalAddress(MS_U8 u8Para1, MS_U8 u8Para2)
{
    gCECInfo.ActivePhysicalAddress[0] = u8Para1;
    gCECInfo.ActivePhysicalAddress[1] = u8Para2;
}

void MApi_CEC_SetActiveDeviceCECVersion(MS_U8 u8Ver)
{
    gCECInfo.ActiveDeviceCECVersion = u8Ver;
}

void MApi_CEC_SetActiveDeviceType(MsCEC_DEVICE_TYPE Type)
{
    gCECInfo.ActiveDeviceType = Type;
}

MS_U8 MApi_CEC_GetMsgCnt(void)
{
    return gCECInfo.bCecMsgCnt;
}

void MApi_CEC_SetMsgCnt(MS_U8 u8Cnt)
{
    gCECInfo.bCecMsgCnt = u8Cnt;
}

MS_U8 MApi_CEC_GetRxData(MS_U8 u8Fifoidx, MS_U8 u8Idx)
{
    return gCECInfo.CecRxBuf[u8Fifoidx].tRxData[u8Idx];
}


MS_BOOL MApi_CEC_CheckFrame(MsCEC_MSG_TRANS_TYPE msg_type, MS_U8 ucLen)
{
    return MDrv_CEC_CheckFrame(msg_type, ucLen);
}

void MApi_CEC_CheckExistDevices(void)
{
    MDrv_CEC_CheckExistDevices();
}

void MApi_CEC_SetMyPhysicalAddress(MS_U8 *pdata)
{
    gCECInfo.MyPhysicalAddress[0] = pdata[0];
    gCECInfo.MyPhysicalAddress[1] = pdata[1];
}

#if ENABLE_CEC_MULTIPLE
void MApi_CEC_SetMyLogicalAddress2(MsCEC_DEVICELA myLA)
{
    MDrv_CEC_SetMyLogicalAddress2(myLA);
}

CEC_ERROR_CODE MApi_CEC_Msg_ReportPhycalAddress2(void)
{
    CEC_ERROR_CODE res;
    MS_U8 txbuf[3];

    txbuf[0] = gCECInfo.MyPhysicalAddress2[0];
    txbuf[1] = gCECInfo.MyPhysicalAddress2[1];
    txbuf[2] = gCECInfo.MyDeviceType2;

    res = MDrv_CEC_TxApi(E_LA_BROADCAST, E_MSG_SI_REPORT_PHY_ADDR, txbuf, 3);

    return res;
}

void MApi_CEC_SetMyPhysicalAddress2(MS_U8 *pdata)
{
    gCECInfo.MyPhysicalAddress2[0] = pdata[0];
    gCECInfo.MyPhysicalAddress2[1] = pdata[1];
}

void MApi_CEC_SetInitiator(MsCEC_DEVICELA IniLA)

{
    gCECInfo.IniLogicalAddress = IniLA ;
}


#endif
/***************************************************************************************/
/// config cec wake up
/***************************************************************************************/
void MApi_CEC_ConfigWakeUp(void)
{
    MDrv_CEC_ConfigWakeUp();
}

void MApi_CEC_Enabled(MS_BOOL bEnableFlag)
{
    MDrv_CEC_Enabled(bEnableFlag);
}

MS_U8 MApi_CEC_GetTxStatus(void)
{
    return MDrv_CEC_TxStatus();
}

