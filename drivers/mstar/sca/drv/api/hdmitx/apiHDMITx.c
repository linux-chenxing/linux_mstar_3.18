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
/// file   apiHDMITx.c
/// @author MStar Semiconductor,Inc.
/// @brief  HDMI Tx Api
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

#ifndef MSOS_TYPE_LINUX_KERNEL
#include <stdlib.h>
#include <string.h>
#else
#include <linux/time.h>
#endif /* #include MSOS_TYPE_LINUX_KERNEL */

#include "MsCommon.h"
#include "drvHDMITx.h"
#include "apiHDMITx.h"
#ifdef CUSTOMER_NDS
#include "apiHDMITx_NDS.h"
#endif // CUSTOMER_NDS
//#include "drvMAD.h"

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

extern MDrvHDMITX_PARAMETER_LIST     gHDMITxInfo;
extern MS_BOOL     g_bDisableRegWrite;
extern MS_BOOL     g_bDisableTMDSCtrl;
//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------






//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------


//*********************//
//        DVI / HDMI   //
//*********************//

//------------------------------------------------------------------------------
/// @brief This routine initial HDMI Tx Module
/// @return None
//------------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_Init(void)
{
    return(MDrv_HDMITx_Init());
}

//------------------------------------------------------------------------------
/// @brief This routine exits HDMI Tx Module
/// @return None
//------------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_Exit(void)
{
    MDrv_HDMITx_Power_OnOff(FALSE);
    return TRUE;
}

//------------------------------------------------------------------------------
/// @brief This routine turn on/off HDMI Tx Module
/// @param[in] state ON/OFF state
/// @return None
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_TurnOnOff(MS_BOOL state)
{
    gHDMITxInfo.hdmitx_enable_flag = state;
    gHDMITxInfo.hdmitx_tmds_flag = state;
    MDrv_HDMITx_TurnOnOff();
}


//------------------------------------------------------------------------------
/// @brief This routine will set or stop all HDMI packet generation
/// @param[in] bflag  True: Enable packet gen, False : Disable packet gen 
/// @return None
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_EnablePacketGen(MS_BOOL bflag)
{
    MDrv_HDMITx_EnablePacketGen(bflag);
}

//------------------------------------------------------------------------------
/// @brief This routine config HDMI Tx Module output mode (DVI/HDMI)
/// @param[in] mode ON/OFF state
/// @return None
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_SetHDMITxMode(HDMITX_OUTPUT_MODE mode)
{
    switch (mode)
    {
        case HDMITX_DVI:
            gHDMITxInfo.output_mode = E_HDMITX_DVI;
            break;
        case HDMITX_DVI_HDCP:
            gHDMITxInfo.output_mode = E_HDMITX_DVI_HDCP;
            break;
        case HDMITX_HDMI:
            gHDMITxInfo.output_mode = E_HDMITX_HDMI;
            break;
        case HDMITX_HDMI_HDCP:
            gHDMITxInfo.output_mode = E_HDMITX_HDMI_HDCP;
            break;
        default:
            printf("[HDMITx]: Error output mode: %d\n", mode);
            return;
    }

    if(gHDMITxInfo.output_mode & 0x01) // check HDCP
        gHDMITxInfo.hdmitx_hdcp_flag = TRUE;
    else
        gHDMITxInfo.hdmitx_hdcp_flag = FALSE;
    MDrv_HDMITx_SetHDMITxMode();
}

//------------------------------------------------------------------------------
/// @brief This routine config HDMI Tx Module output mode (DVI/HDMI) and deep color depth
/// @param[in] mode ON/OFF state
/// @return None
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_SetHDMITxMode_CD(HDMITX_OUTPUT_MODE mode, HDMITX_VIDEO_COLORDEPTH_VAL val)
{
    switch (mode)
    {
        case HDMITX_DVI:
            gHDMITxInfo.output_mode = E_HDMITX_DVI;
            break;
        case HDMITX_DVI_HDCP:
            gHDMITxInfo.output_mode = E_HDMITX_DVI_HDCP;
            break;
        case HDMITX_HDMI:
            gHDMITxInfo.output_mode = E_HDMITX_HDMI;
            break;
        case HDMITX_HDMI_HDCP:
            gHDMITxInfo.output_mode = E_HDMITX_HDMI_HDCP;
            break;
        default:
            printf("[HDMITx]: Error output mode: %d\n", mode);
            return;
    }

    if(gHDMITxInfo.output_mode & 0x01) // check HDCP
        gHDMITxInfo.hdmitx_hdcp_flag = TRUE;
    else
        gHDMITxInfo.hdmitx_hdcp_flag = FALSE;

    gHDMITxInfo.output_colordepth_val = (MsHDMITX_VIDEO_COLORDEPTH_VAL)val;
    MDrv_HDMITx_SetHDMITxMode();
}

//------------------------------------------------------------------------------
/// @brief This routine config HDMI Tx Module TMDS signal On/Off
/// @param[in] state ON/OFF state
/// @return None
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_SetTMDSOnOff(MS_BOOL state)
{
    gHDMITxInfo.hdmitx_tmds_flag = state;
    MDrv_HDMITx_SetTMDSOnOff();
}

//------------------------------------------------------------------------------
/// @brief This routine disable HDMI Tx TMDS control
/// @param[in] bFlag Disable/Enable
/// @return None
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_DisableTMDSCtrl(MS_BOOL bFlag)
{
    g_bDisableTMDSCtrl = bFlag;
}

//------------------------------------------------------------------------------
/// @brief This routine swap R and B channel color
/// @param[in] state ON/OFF state
/// @return None
//-----------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_SetRBChannelSwap(MS_BOOL state)
{
    gHDMITxInfo.hdmitx_RB_swap_flag = state;
}

//------------------------------------------------------------------------------
/// @brief This routine doing HDMITx exhibition
/// @return None
//-----------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_Exhibit(void)
{
    MDrv_HDMITx_Exhibit();
}

//------------------------------------------------------------------------------
/// @brief This routine getting the connected HDMI Rx status
/// @return Ture: Rx is alive
///             FALSE: Rx is dead
//-----------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_GetRxStatus(void)
{
    return (MDrv_HDMITx_GetRxStatus());
}

//------------------------------------------------------------------------------
/// @brief This routine getting Rx's deep color definition from EDID
/// @return Ture: EDID is ready
///             FALSE: EDID is not ready
//-----------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_GetRxDCInfoFromEDID(HDMITX_VIDEO_COLORDEPTH_VAL *val)
{
    *val = (HDMITX_VIDEO_COLORDEPTH_VAL)gHDMITxInfo.edid_colordepth_val;
    return gHDMITxInfo.hdmitx_edid_ready;
}

//------------------------------------------------------------------------------
/// @brief This routine getting Rx's support video format from EDID
/// @param[in] pu8Buffer pointer of buffer
/// @param[in] u8BufSize buffer size
/// @return Ture: EDID is ready
///             FALSE: EDID is not ready
//-----------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_GetRxVideoFormatFromEDID(MS_U8 *pu8Buffer, MS_U8 u8BufSize)
{
	MS_U8 u8MaxSize = 32;
	if(u8BufSize < u8MaxSize)
	{
		memcpy(pu8Buffer, &gHDMITxInfo.short_video_descriptor, u8BufSize*sizeof(MS_U8));
	}
	else
	{
		memcpy(pu8Buffer, &gHDMITxInfo.short_video_descriptor, u8MaxSize*sizeof(MS_U8));
	}
    return gHDMITxInfo.hdmitx_edid_ready;
}

//------------------------------------------------------------------------------
/// @brief This routine getting Rx's data block length from EDID
/// @param[in] pu8Length pointer of buffer
/// @param[in] u8TagCode
/// @return Ture: EDID is ready
///             FALSE: EDID is not ready
//-----------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_GetDataBlockLengthFromEDID(MS_U8 *pu8Length, MS_U8 u8TagCode)
{
    if(u8TagCode < 8)
    {
	    memcpy(pu8Length, &gHDMITxInfo.data_block_length[u8TagCode], sizeof(MS_U8));
    }
    else
    {
        memcpy(pu8Length, &gHDMITxInfo.data_block_length[0], sizeof(MS_U8));
    }
	return gHDMITxInfo.hdmitx_edid_ready;
}

//------------------------------------------------------------------------------
/// @brief This routine getting Rx's support audio format from EDID
/// @param[in] pu8Buffer pointer of buffer
/// @param[in] u8BufSize buffer size
/// @return Ture: EDID is ready
///             FALSE: EDID is not ready
//-----------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_GetRxAudioFormatFromEDID(MS_U8 *pu8Buffer, MS_U8 u8BufSize)
{
	MS_U8 u8MaxSize = 32;
	if(u8BufSize < u8MaxSize)
	{
		memcpy(pu8Buffer, &gHDMITxInfo.short_audio_descriptor, u8BufSize*sizeof(MS_U8));
	}
	else
	{
		memcpy(pu8Buffer, &gHDMITxInfo.short_audio_descriptor, u8MaxSize*sizeof(MS_U8));
	}
    return gHDMITxInfo.hdmitx_edid_ready;
}


MS_BOOL DLL_PUBLIC MApi_HDMITx_EDID_HDMISupport(MS_BOOL *HDMI_Support)
{
    *HDMI_Support = gHDMITxInfo.edid_HDMI_support; //TRUE for HDMI
    return gHDMITxInfo.hdmitx_edid_ready;
}

//------------------------------------------------------------------------------
/// @brief This routine getting Rx's ID Manu facturer Name from EDID
/// @param[in] pu8Buffer pointer of buffer
/// @return Ture: EDID is ready
///             FALSE: EDID is not ready
//-----------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_GetRxIDManufacturerName(MS_U8 *pu8Buffer)
{
    memcpy(pu8Buffer, &gHDMITxInfo.id_manufacturer_name, 3*sizeof(MS_U8));
    return gHDMITxInfo.hdmitx_edid_ready;
}


//------------------------------------------------------------------------------
/// @brief This routine getting Rx's EDID Data
/// @param[in] pu8Buffer pointer of buffer
/// @param[in] BlockIdx 0: for 0~127, 1: for 128~255
/// @return Ture: EDID is ready
///             FALSE: EDID is not ready
//-----------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_GetEDIDData(MS_U8 *pu8Buffer, MS_BOOL BlockIdx)
{
    if (BlockIdx)
        memcpy(pu8Buffer, &gHDMITxInfo.edid_block1, 128*sizeof(MS_U8));
    else
        memcpy(pu8Buffer, &gHDMITxInfo.edid_block0, 128*sizeof(MS_U8));
    return gHDMITxInfo.hdmitx_edid_ready;
}

//------------------------------------------------------------------------------
/// @brief This routine gets Rx's supported 3D structures of specific timing from EDID
/// @param[in] timing input video timing
/// @param[in] p3DStructure pointer of supported 3D structure
/// @return Ture: EDID is ready
///             FALSE: EDID is not ready
//-----------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_GetRx3DStructureFromEDID(HDMITX_VIDEO_TIMING timing, HDMITX_EDID_3D_STRUCTURE_ALL *p3DStructure)
{
    if(gHDMITxInfo.hdmitx_edid_ready)
        return MDrv_HDMITx_GetRx3DStructureFromEDID((MsHDMITX_VIDEO_TIMING)timing, (MsHDMITX_EDID_3D_STRUCTURE_ALL *)p3DStructure);
    else
        return FALSE;
}

//------------------------------------------------------------------------------
/// @brief This function clear settings of user defined packet
/// @return None
//-----------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_PKT_User_Define_Clear(void)
{
    MDrv_HDMITx_PKT_User_Define_Clear();
}

//------------------------------------------------------------------------------
/// @brief This function set user defined hdmi packet
/// @param[in] packet_type: type of user defined packet
/// @param[in] def_flag: enable/disable user defined packet
/// @param[in] def_process: packet process of user defined packet
/// @param[in] def_fcnt: frame counts of user defined packet (0~31)
/// @return None
//-----------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_PKT_User_Define(HDMITX_PACKET_TYPE packet_type, MS_BOOL def_flag,
	HDMITX_PACKET_PROCESS def_process, MS_U8 def_fcnt)
{
    MDrv_HDMITx_PKT_User_Define((MsHDMITX_PACKET_TYPE)packet_type, def_flag, (MsHDMITX_PACKET_PROCESS)def_process, def_fcnt);
}

//------------------------------------------------------------------------------
/// @brief This function let user define packet's content
/// @param[in] packet_type: type of user defined packet
/// @param[in] data: content of user defined packet
/// @param[in] length: length of packet content
/// @return None
//------------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_PKT_Content_Define(HDMITX_PACKET_TYPE packet_type, MS_U8 *data, MS_U8 length)
{
    return MDrv_HDMITx_PKT_Content_Define((MsHDMITX_PACKET_TYPE)packet_type, data, length);
}


//*********************//
//             Video   //
//*********************//

//------------------------------------------------------------------------------
/// @brief This routine turn on/off HDMI Tx Module video output
/// @param[in] state ON/OFF state
/// @return None
//-----------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_SetVideoOnOff(MS_BOOL state)
{
    gHDMITxInfo.hdmitx_video_flag = state;
    MDrv_HDMITx_SetVideoOnOff();
}

//------------------------------------------------------------------------------
/// @brief This routine config HDMI Tx Module video color format
/// @param[in] in_color input color format
/// @param[in] out_color output color format
/// @return None
//-----------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_SetColorFormat(HDMITX_VIDEO_COLOR_FORMAT in_color, HDMITX_VIDEO_COLOR_FORMAT out_color)
{
    switch (in_color)
    {
        case HDMITX_VIDEO_COLOR_RGB444:
            gHDMITxInfo.input_color = E_HDMITX_VIDEO_COLOR_RGB444;
            break;
        case HDMITX_VIDEO_COLOR_YUV422:
            gHDMITxInfo.input_color = E_HDMITX_VIDEO_COLOR_YUV422;
            break;
        case HDMITX_VIDEO_COLOR_YUV444:
            gHDMITxInfo.input_color = E_HDMITX_VIDEO_COLOR_YUV444;
            break;
        default:
            printf("[HDMITx]: Error video color format(in): %d\n", in_color);
            return;
    }

    switch (out_color)
    {
        case HDMITX_VIDEO_COLOR_RGB444:
            gHDMITxInfo.output_color = E_HDMITX_VIDEO_COLOR_RGB444;
            break;
        case HDMITX_VIDEO_COLOR_YUV422:
            gHDMITxInfo.output_color = E_HDMITX_VIDEO_COLOR_YUV422;
            break;
        case HDMITX_VIDEO_COLOR_YUV444:
            gHDMITxInfo.output_color = E_HDMITX_VIDEO_COLOR_YUV444;
            break;
        default:
            printf("[HDMITx]: Error video color format(out): %d\n", out_color);
            return;
    }

    // YUV444 -> RGB444
    if( (in_color != out_color) && (out_color == (HDMITX_VIDEO_COLOR_FORMAT) E_HDMITX_VIDEO_COLOR_RGB444) )
        gHDMITxInfo.hdmitx_csc_flag = TRUE;
    else
        gHDMITxInfo.hdmitx_csc_flag = FALSE;

    MDrv_HDMITx_SetColorFormat();
}


//------------------------------------------------------------------------------
/// @brief This routine config HDMI Tx vs packet content
/// @param[in] in_color input color format
/// @param[in] out_color output color format
/// @return None
//-----------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_Set_VS_InfoFrame(HDMITX_VIDEO_VS_FORMAT vs_format, HDMITX_VIDEO_3D_STRUCTURE vs_3d, HDMITX_VIDEO_4k2k_VIC vs_vic)
{
    switch (vs_format)
    {
        case HDMITX_VIDEO_VS_4k_2k:
            gHDMITxInfo.vs_pkt_format = E_HDMITX_VIDEO_VS_4k_2k;
			switch (vs_vic)
		    {
		        case HDMITX_VIDEO_4k2k_30Hz:
			        gHDMITxInfo.vs_4k2k_vic = E_HDMITX_VIDEO_4k2k_30Hz;
				    break;
			    case HDMITX_VIDEO_4k2k_25Hz:
				    gHDMITxInfo.vs_4k2k_vic = E_HDMITX_VIDEO_4k2k_25Hz;
				    break;
			    case HDMITX_VIDEO_4k2k_24Hz:
				    gHDMITxInfo.vs_4k2k_vic = E_HDMITX_VIDEO_4k2k_24Hz;
				    break;
			    case HDMITx_VIDEO_4k2k_24Hz_SMPTE:
				    gHDMITxInfo.vs_4k2k_vic = E_HDMITx_VIDEO_4k2k_24Hz_SMPTE;
				    break;
			    case HDMITx_VIDEO_4k2k_Reserved:
				    gHDMITxInfo.vs_4k2k_vic = E_HDMITX_VIDEO_4k2k_30Hz;
				    break;
			    default:
				    gHDMITxInfo.vs_4k2k_vic = E_HDMITx_VIDEO_4k2k_Reserved;
				    break;
		    }
            break;

        case HDMITX_VIDEO_VS_3D:
            gHDMITxInfo.vs_pkt_format = E_HDMITX_VIDEO_VS_3D;
			switch (vs_3d)
	        {
		        case HDMITX_VIDEO_3D_FramePacking: //0000
			    gHDMITxInfo.vs_pkt_3d = E_HDMITX_VIDEO_3D_FramePacking;
				break;
                case HDMITX_VIDEO_3D_FieldAlternative: //0001
                gHDMITxInfo.vs_pkt_3d = E_HDMITX_VIDEO_3D_FieldAlternative;
				break;
                case HDMITX_VIDEO_3D_LineAlternative: //0010
                gHDMITxInfo.vs_pkt_3d = E_HDMITX_VIDEO_3D_LineAlternative;
				break;
				case HDMITX_VIDEO_3D_SidebySide_FULL: //0011
				gHDMITxInfo.vs_pkt_3d = E_HDMITX_VIDEO_3D_SidebySide_FULL;
				break;
				case HDMITX_VIDEO_3D_L_Dep: //0100
				gHDMITxInfo.vs_pkt_3d = E_HDMITX_VIDEO_3D_L_Dep;
				break;
				case HDMITX_VIDEO_3D_L_Dep_Graphic_Dep: //0101
                gHDMITxInfo.vs_pkt_3d = E_HDMITX_VIDEO_3D_L_Dep_Graphic_Dep;
				break;
			    case HDMITX_VIDEO_3D_TopandBottom: //0110
				gHDMITxInfo.vs_pkt_3d = E_HDMITX_VIDEO_3D_TopandBottom;
				break;
			    case HDMITX_VIDEO_3D_SidebySide_Half: //1000
				gHDMITxInfo.vs_pkt_3d = E_HDMITX_VIDEO_3D_SidebySide_Half;
				break;
			    default: //1111
				gHDMITxInfo.vs_pkt_3d = E_HDMITx_VIDEO_3D_Not_in_Use;
				break;
		    }
            break;

        default:
            gHDMITxInfo.vs_pkt_format = E_HDMITX_VIDEO_VS_No_Addition;
            break;
    }

	MDrv_HDMITx_Set_VS_InfoFrame();
}


//------------------------------------------------------------------------------
/// @brief This routine config HDMI Tx Module video output timing
/// @param[in] mode output video timing
/// @return None
//-----------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_SetVideoOutputTiming(HDMITX_VIDEO_TIMING mode)
{
    switch (mode)
    {
        case HDMITX_RES_640x480p:
            gHDMITxInfo.output_video_timing = E_HDMITX_RES_640x480p;
            break;
        case HDMITX_RES_720x480i:
            gHDMITxInfo.output_video_timing = E_HDMITX_RES_720x480i;
            break;
        case HDMITX_RES_720x576i:
            gHDMITxInfo.output_video_timing = E_HDMITX_RES_720x576i;
            break;
        case HDMITX_RES_720x480p:
            gHDMITxInfo.output_video_timing = E_HDMITX_RES_720x480p;
            break;
        case HDMITX_RES_720x576p:
            gHDMITxInfo.output_video_timing = E_HDMITX_RES_720x576p;
            break;
        case HDMITX_RES_1280x720p_50Hz:
            gHDMITxInfo.output_video_timing = E_HDMITX_RES_1280x720p_50Hz;
            break;
        case HDMITX_RES_1280x720p_60Hz:
            gHDMITxInfo.output_video_timing = E_HDMITX_RES_1280x720p_60Hz;
            break;
        case HDMITX_RES_1920x1080i_50Hz:
            gHDMITxInfo.output_video_timing = E_HDMITX_RES_1920x1080i_50Hz;
            break;
        case HDMITX_RES_1920x1080i_60Hz:
            gHDMITxInfo.output_video_timing = E_HDMITX_RES_1920x1080i_60Hz;
            break;
        case HDMITX_RES_1920x1080p_24Hz:
            gHDMITxInfo.output_video_timing = E_HDMITX_RES_1920x1080p_24Hz;
            break;
        case HDMITX_RES_1920x1080p_25Hz:
            gHDMITxInfo.output_video_timing = E_HDMITX_RES_1920x1080p_25Hz;
            break;
        case HDMITX_RES_1920x1080p_30Hz:
            gHDMITxInfo.output_video_timing = E_HDMITX_RES_1920x1080p_30Hz;
            break;
        case HDMITX_RES_1920x1080p_50Hz:
            gHDMITxInfo.output_video_timing = E_HDMITX_RES_1920x1080p_50Hz;
            break;
        case HDMITX_RES_1920x1080p_60Hz:
            gHDMITxInfo.output_video_timing = E_HDMITX_RES_1920x1080p_60Hz;
            break;
        default:
            printf("[HDMITx]: Error video timing: %d\n", mode);
            return;
    }

#ifdef URANUS
    if ((MS_U32)mode > (MS_U32)E_HDMITX_RES_720x576p)
    {
        printf("[%s][%d] Unsupport HDMI mode 0x%08x\n", __FUNCTION__, __LINE__, mode);
        return;
    }
#endif
#ifdef OBERON
    if ((MS_U32)mode >= (MS_U32)E_HDMITX_RES_MAX)
    {
        printf("[%s][%d] Unsupport HDMI mode 0x%08x\n", __FUNCTION__, __LINE__, mode);
        return;
    }
#endif
    MDrv_HDMITx_SetVideoOutputMode();
}

//------------------------------------------------------------------------------
/// @brief This routine config HDMI Tx video output aspect ratio
/// @param[in] out_ar output video aspect ratio
/// @return None
//-----------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_SetVideoOutputAsepctRatio(HDMITX_VIDEO_ASPECT_RATIO out_ar)
{
    switch (out_ar)
    {
        case HDMITX_VIDEO_AR_Reserved:
            gHDMITxInfo.output_aspect_ratio = E_HDMITX_VIDEO_AR_Reserved;
            break;
        case HDMITX_VIDEO_AR_4_3:
            gHDMITxInfo.output_aspect_ratio = E_HDMITX_VIDEO_AR_4_3;
            break;
        case HDMITX_VIDEO_AR_16_9:
            gHDMITxInfo.output_aspect_ratio = E_HDMITX_VIDEO_AR_16_9;
            break;
        default:
            printf("[HDMITx]: Error aspect ratio: %d\n", out_ar);
            return;
    }

    MDrv_HDMITx_SetVideoOutputAsepctRatio();
}


// HDMI Tx video output Overscan and AFD ratio
void DLL_PUBLIC MApi_HDMITx_SetVideoOutputOverscan_AFD(MS_BOOL bflag, HDMITX_VIDEO_SCAN_INFO out_scaninfo, MS_U8 out_afd)
{
    gHDMITxInfo.hdmitx_AFD_override_mode = bflag;
    gHDMITxInfo.output_afd_ratio = (MsHDMITX_VIDEO_AFD_RATIO)out_afd;

    switch (out_scaninfo)
    {
        case HDMITX_VIDEO_SI_NoData:
            gHDMITxInfo.output_scan_info = E_HDMITX_VIDEO_SI_NoData;
            break;
        case HDMITX_VIDEO_SI_Overscanned:
            gHDMITxInfo.output_scan_info = E_HDMITX_VIDEO_SI_Overscanned;
            break;
        case HDMITX_VIDEO_SI_Underscanned:
            gHDMITxInfo.output_scan_info = E_HDMITX_VIDEO_SI_Underscanned;
            break;
        case HDMITX_VIDEO_SI_Reserved:
            gHDMITxInfo.output_scan_info = E_HDMITX_VIDEO_SI_Reserved;
            break;
        default:
            printf("[HDMITx]: Error scan information: %d\n", out_scaninfo);
            return;
    }

    MDrv_HDMITx_SetVideoOutputOverscan_AFD();
}

//*********************//
//             Audio   //
//*********************//

//------------------------------------------------------------------------------
/// @brief This routine turn on/off HDMI Tx Module audio output
/// @param[in] state output video timing
/// @return None
//-----------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_SetAudioOnOff(MS_BOOL state)
{
    gHDMITxInfo.hdmitx_audio_flag = state;
    MDrv_HDMITx_SetAudioOnOff();
}

//------------------------------------------------------------------------------
/// @brief This routine config HDMI Tx Module audio output sampling frequency and channel count
/// @param[in] freq audio frequency
/// @return None
//   For Uranus
//-----------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_SetAudioFrequency(HDMITX_AUDIO_FREQUENCY freq)
{
    switch (freq)
    {
        case HDMITX_AUDIO_FREQ_NO_SIG:
            gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_FREQ_NO_SIG;
            break;
        case HDMITX_AUDIO_32K:
            gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_32K;
            break;
        case HDMITX_AUDIO_44K:
            gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_44K;
            break;
        case HDMITX_AUDIO_48K:
            gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_48K;
            break;
        case HDMITX_AUDIO_88K:
            gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_88K;
            break;
        case HDMITX_AUDIO_96K:
            gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_96K;
            break;
        case HDMITX_AUDIO_176K:
            gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_176K;
            break;
        case HDMITX_AUDIO_192K:
            gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_192K;
            break;
        default:
            printf("[HDMITx]: Error audio frequency: %d\n", freq);
            return;
    }

    MDrv_HDMITx_SetAudioFrequency();
}

void DLL_PUBLIC MApi_HDMITx_SetVideoOutputOverscan_AFD_II(MS_BOOL bflag, HDMITX_VIDEO_SCAN_INFO out_scaninfo, MS_U8 out_afd, MS_U8 A0 )
{
    gHDMITxInfo.hdmitx_AFD_override_mode = bflag;
    gHDMITxInfo.output_afd_ratio = (MsHDMITX_VIDEO_AFD_RATIO)out_afd;
    gHDMITxInfo.output_activeformat_present = A0;

    switch (out_scaninfo)
    {
        case HDMITX_VIDEO_SI_NoData:
            gHDMITxInfo.output_scan_info = E_HDMITX_VIDEO_SI_NoData;
            break;
        case HDMITX_VIDEO_SI_Overscanned:
            gHDMITxInfo.output_scan_info = E_HDMITX_VIDEO_SI_Overscanned;
            break;
        case HDMITX_VIDEO_SI_Underscanned:
            gHDMITxInfo.output_scan_info = E_HDMITX_VIDEO_SI_Underscanned;
            break;
        case HDMITX_VIDEO_SI_Reserved:
            gHDMITxInfo.output_scan_info = E_HDMITX_VIDEO_SI_Reserved;
            break;
        default:
            printf("[HDMITx]: Error scan information: %d\n", out_scaninfo);
            return;
    }

    MDrv_HDMITx_SetVideoOutputOverscan_AFD();
}

//------------------------------------------------------------------------------
/// @brief This routine config HDMI Tx Module audio output: sampling frequency, channel count and coding type
/// @param[in]  freq audio frequency
/// @param[in]  ch audio channel type
/// @param[in]  type audio coding type
/// @return None
//   For Oberon
//-----------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_SetAudioConfiguration(HDMITX_AUDIO_FREQUENCY freq, HDMITX_AUDIO_CHANNEL_COUNT ch, HDMITX_AUDIO_CODING_TYPE type)
{
    switch (freq)
    {
        case HDMITX_AUDIO_FREQ_NO_SIG:
            gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_FREQ_NO_SIG;
            break;
        case HDMITX_AUDIO_32K:
            gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_32K;
            break;
        case HDMITX_AUDIO_44K:
            gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_44K;
            break;
        case HDMITX_AUDIO_48K:
            gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_48K;
            break;
        case HDMITX_AUDIO_88K:
            gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_88K;
            break;
        case HDMITX_AUDIO_96K:
            gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_96K;
            break;
        case HDMITX_AUDIO_176K:
            gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_176K;
            break;
        case HDMITX_AUDIO_192K:
            gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_192K;
            break;
        default:
            printf("[HDMITx]: Error audio frequency: %d\n", freq);
            return;
    }

    switch (ch)
    {
        case HDMITX_AUDIO_CH_2:
            gHDMITxInfo.output_audio_channel = E_HDMITX_AUDIO_CH_2;
            break;
        case HDMITX_AUDIO_CH_8:
            gHDMITxInfo.output_audio_channel = E_HDMITX_AUDIO_CH_8;
            break;
        default:
            printf("[HDMITx]: Error audio channel: %d\n", ch);
            return;
    }

    switch (type)
    {
        case HDMITX_AUDIO_PCM:
            gHDMITxInfo.output_audio_type = E_HDMITX_AUDIO_PCM;
            break;
        case HDMITX_AUDIO_NONPCM:
            gHDMITxInfo.output_audio_type = E_HDMITX_AUDIO_NONPCM;
            break;
        default:
            printf("[HDMITx]: Error audio type: %d\n", type);
            return;
    }
    MDrv_HDMITx_SetAudioFrequency();
}

//------------------------------------------------------------------------------
/// @brief This routine config HDMI Tx audio mode
/// @param[in]  fmt : audio source format select
/// @return None
//-----------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_SetAudioSourceFormat(HDMITX_AUDIO_SOURCE_FORMAT fmt)
{
    MDrv_HDMITx_SetAudioSourceFormat((MsHDMITX_AUDIO_SOURCE_FORMAT) fmt);
}

//------------------------------------------------------------------------------
/// @brief This routine Get Audio CTS value
/// @return CTS
//------------------------------------------------------------------------------
MS_U32 DLL_PUBLIC MApi_HDMITx_GetAudioCTS(void)
{
    return MDrv_HDMITx_GetAudioCTS();
}

//------------------------------------------------------------------------------
/// @brief This routine Mute Audio FIFO
/// @param[in] bflag: True: mute audio, False: unmute audio
/// @return None
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_MuteAudioFIFO(MS_BOOL bflag)
{
    MDrv_HDMITx_MuteAudioFIFO(bflag);
}

#if 0
void MApi_HDMITx_SetAudioFrequencyFromMad(void)
{
    //Get Sampling Rate
    MS_U32 SR;
    MDrv_MAD_AudioSamplingRate(&SR);
    printf("MApi_HDMITx_SetAudioFrequencyFromMad()::Sampling Rate=%d K\n", SR);

    if (SR<28)
        gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_FREQ_NO_SIG;
    else if ( SR< ((32+44)/2) )
        gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_32K;
    else if ( SR< ((44+48)/2) )
        gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_44K;
    else if ( SR< ((48+88)/2) )
        gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_48K;
    else if ( SR< ((88+96)/2) )
        gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_88K;
    else if ( SR< ((96+176)/2) )
        gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_96K;
    else if ( SR< ((176+192)/2) )
        gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_176K;
    else if ( SR< 200 )
        gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_192K;
    else
        gHDMITxInfo.output_audio_frequncy = E_HDMITX_AUDIO_FREQ_MAX_NUM;

    MDrv_HDMITx_SetAudioFrequency();
}
#endif

//*********************//
//             HDCP    //
//*********************//

//------------------------------------------------------------------------------
/// @brief This routine get HDCP key
/// @param[in] useinternalkey: TRUE -> from internal, FALSE -> from external, like SPI flash
/// @param[in] data: data point
/// @return None
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_GetHdcpKey(MS_BOOL useinternalkey, MS_U8 *data)
{
    gHDMITxInfo.hdmitx_HdcpUseInternalKey_flag = useinternalkey;
    MDrv_HDMITx_GetHdcpKey(useinternalkey, data);
}


//------------------------------------------------------------------------------
/// @brief This routine get HDCP bksv
/// @param[in] data: data point
/// @return None
//------------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_GetBksv(MS_U8 *pdata)
{
    memcpy(pdata, &gHDMITxInfo.HDCP_BKSV, 5*sizeof(MS_U8));
	return gHDMITxInfo.HDCP_74_check;
}


//------------------------------------------------------------------------------
/// @brief This routine get HDCP aksv
/// @param[in] data: data point
/// @return None
//------------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_GetAksv(MS_U8 *pdata)
{
    memcpy(pdata, &gHDMITxInfo.HDCP_AKSV, 5*sizeof(MS_U8));
	return gHDMITxInfo.HDCP_74_check;
}


//------------------------------------------------------------------------------
/// @brief This routine turn on/off HDMI Tx Module HDCP encryption
/// @param[in] state ON/OFF state
/// @return None
//-----------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_SetHDCPOnOff(MS_BOOL state)
{
    gHDMITxInfo.hdmitx_hdcp_flag = state;
    MDrv_HDMITx_SetHDCPOnOff();
}

//------------------------------------------------------------------------------
/// @brief This routine set HDMI Tx AVMUTE
/// @argument:
///              - bflag: True=> SET_AVMUTE, FALSE=>CLEAR_AVMUTE
///
/// @return None
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_SetAVMUTE(MS_BOOL bflag)
{
    gHDMITxInfo.hdmitx_avmute_flag = bflag;
    MDrv_HDMITx_SetAVMUTE(bflag);
}

//------------------------------------------------------------------------------
/// @brief This routine get HDMI Tx AVMUTE status
/// @argument:
///
/// @return:
///              - True=> SET_AVMUTE, FALSE=>CLEAR_AVMUTE
//------------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_GetAVMUTEStatus(void)
{
    return gHDMITxInfo.hdmitx_avmute_flag;
}

//------------------------------------------------------------------------------
/// @brief This routine update Revocation key list
/// @argument:
///              - *data: data point
///              -  size: amount of revoked key ( size 1 = 1*5 bytes, size 2 = 2*5 bytes)
/// @return
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_HDCP_RevocationKey_List(MS_U8 *data, MS_U16 size)
{
    return MDrv_HDMITx_HDCP_RevocationKey_List(data, size);
}

//------------------------------------------------------------------------------
/// @brief This routine return revocation key check state
/// @argument:
/// @return: 
//------------------------------------------------------------------------------
HDMITX_REVOCATION_STATE DLL_PUBLIC MApi_HDMITx_HDCP_RevocationKey_Check(void)
{
    return (HDMITX_REVOCATION_STATE)MDrv_HDMITx_HDCP_RevocationKey_Check();
}

//------------------------------------------------------------------------------
/// @brief This routine check whether SRM DSA signauter is valid or not
/// @argument:
///              - *data: data point
///              -   size: size of SRM list(bytes)
/// @return:
///              Ture: valid, FALSE: invalid
//------------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_HDCP_IsSRMSignatureValid(MS_U8 *data, MS_U32 size)
{
    return MDrv_HDMITx_HDCP_IsSRMSignatureValid(data, size);
}


//------------------------------------------------------------------------------
/// @brief This routine get HDCP authentication status
/// @argument:
/// @return:
///                  E_HDCP_DISABLE      = 0, // HDCP disable
///                  E_HDCP_FAIL = 1, // HDCP fail
///                  E_HDCP_PASS = 2, // HDCP pass
//------------------------------------------------------------------------------
HDMITX_HDCP_STATUS DLL_PUBLIC MApi_HDMITx_GetHDCPStatus(void)
{
    HDMITX_HDCP_STATUS res;

    if(!gHDMITxInfo.hdmitx_hdcp_flag)
        res = E_HDCP_DISABLE;
    else
    {
        if(gHDMITxInfo.hdmitx_fsm_state == E_HDMITX_FSM_HDCP_AUTH_DONE)
            res = E_HDCP_PASS;
        else
            res = E_HDCP_FAIL;
    }
    return res;
}

//------------------------------------------------------------------------------
/// @brief This routine will start/stop HDCP authentication 
/// @argument: bFlag : TURE for start Authentication, FALSE for stop Authentication
/// @return:
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_HDCP_StartAuth(MS_BOOL bFlag)
{
    MDrv_HDMITx_HDCP_StartAuth(bFlag);
}

//------------------------------------------------------------------------------
/// @brief This routine get HDCP authentication status
/// @argument:
/// @return:
///                  HDMITX_INT_HDCP_DISABLE  = 0, // HDCP disable
///                  HDMITX_INT_HDCP_FAIL     = 1, // HDCP fail
///                  HDMITX_INT_HDCP_PASS     = 2, // HDCP pass
///                  HDMITX_INT_HDCP_PROCESS  = 3, // HDCP processing
//------------------------------------------------------------------------------
HDMITX_INT_HDCP_STATUS DLL_PUBLIC MApi_HDMITx_GetINTHDCPStatus(void)
{
    HDMITX_INT_HDCP_STATUS res;

    if(!gHDMITxInfo.hdmitx_hdcp_flag)
        res = HDMITX_INT_HDCP_DISABLE;
    else
    {
        if(gHDMITxInfo.hdmitx_fsm_state == E_HDMITX_FSM_HDCP_AUTH_DONE)
            res = HDMITX_INT_HDCP_PASS;
        else if (gHDMITxInfo.hdmitx_fsm_state == E_HDMITX_FSM_HDCP_AUTH_FAIL)
            res = HDMITX_INT_HDCP_FAIL;
        else
            res = HDMITX_INT_HDCP_PROCESS;
    }
    return res;
}

//------------------------------------------------------------------------------
/// @brief This routine set HDMI video output or blank or encryption while connected with unsupport HDCP Rx
/// @argument:
///                 E_UNHDCPRX_NORMAL_OUTPUT      = 0, // still display normally
///                 E_UNHDCPRX_HDCP_ENCRYPTION = 1, // HDCP encryption to show snow screen
///                 E_UNHDCPRX_BLUE_SCREEN = 2, // blue screen
/// @return:
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_UnHDCPRxControl(HDMITX_UNHDCPRX_CONTROL state)
{
    if(state == E_UNHDCPRX_HDCP_ENCRYPTION)
        gHDMITxInfo.hdmitx_unHDCPRx_Control = E_HDCP_ENCRYPTION;
    else if(state == E_UNHDCPRX_BLUE_SCREEN)
        gHDMITxInfo.hdmitx_unHDCPRx_Control = E_BLUE_SCREEN;
    else
        gHDMITxInfo.hdmitx_unHDCPRx_Control = E_NORMAL_OUTPUT;
}


//------------------------------------------------------------------------------
/// @brief This routine set HDMI video output or blank or encryption while HDCP authentication fail
/// @argument:
///                 E_HDCPRXFail_NORMAL_OUTPUT      = 0, // still display normally
///                 E_HDCPRXFail_HDCP_ENCRYPTION = 1, // HDCP encryption to show snow screen
///                 E_HDCPRXFail_BLUE_SCREEN = 2, // blue screen
/// @return:
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_HDCPRxFailControl(HDMITX_HDCPRXFail_CONTROL state)
{
    if(state == E_HDCPRXFail_HDCP_ENCRYPTION)
        gHDMITxInfo.hdmitx_HDCPRxFail_Control = E_RXFail_HDCP_ENCRYPTION;
    else if(state == E_HDCPRXFail_BLUE_SCREEN)
        gHDMITxInfo.hdmitx_HDCPRxFail_Control = E_RXFail_BLUE_SCREEN;
    else
        gHDMITxInfo.hdmitx_HDCPRxFail_Control = E_RXFail_NORMAL_OUTPUT;
}


// Debug
MS_BOOL DLL_PUBLIC MApi_HDMITx_GetLibVer(const MSIF_Version **ppVersion)
{
    return MDrv_HDMITx_GetLibVer(ppVersion);
}
MS_BOOL DLL_PUBLIC MApi_HDMITx_GetInfo(HDMI_TX_INFO *pInfo)
{
    MS_HDMI_TX_INFO Info;

    MDrv_HDMITx_GetInfo(&Info);
    pInfo->Reserved = Info.Reserved;

    return TRUE;
}
MS_BOOL DLL_PUBLIC MApi_HDMITx_GetStatus(HDMI_TX_Status *pStatus)
{
    MS_HDMI_TX_Status Status;

    MDrv_HDMITx_GetStatus(&Status);

    pStatus->bIsInitialized = Status.bIsInitialized;
    pStatus->bIsRunning = Status.bIsRunning;

    return TRUE;
}
/**
* @brief set debug mask
* @param[in] u16DbgSwitch DEBUG MASK,
*   0x01: Debug HDMITX, 0x02: Debug HDCP
*/
MS_BOOL DLL_PUBLIC MApi_HDMITx_SetDbgLevel(MS_U16 u16DbgSwitch)
{
    return MDrv_HDMITx_SetDbgLevel(u16DbgSwitch);
}

void DLL_PUBLIC MApi_HDMITx_SetHPDGpioPin(MS_U8 u8pin)
{
    MDrv_HDMITx_SetHPDGpioPin(u8pin);
}

// Adjust HDMITx analog setting for HDMI test or compliant issue
void DLL_PUBLIC MApi_HDMITx_AnalogTuning(HDMITX_ANALOG_TUNING *pInfo)
{
    memcpy(&gHDMITxInfo.analog_setting, pInfo, sizeof(HDMITX_ANALOG_TUNING));
}

//------------------------------------------------------------------------------
/// @brief This routine turn on/off HDMI Tx output force mode
/// @argument:
///              - bflag: TRUE: force mode, FALSE: auto mode
///              - output_mode: HDMITX_DVI: DVI, HDMITX_HDMI: HDMI
/// @return None
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_ForceHDMIOutputMode(MS_BOOL bflag, HDMITX_OUTPUT_MODE output_mode)
{
    MDrv_HDMITx_ForceHDMIOutputMode(bflag, (MsHDMITX_OUTPUT_MODE)output_mode);
}

//------------------------------------------------------------------------------
/// @brief This routine turn on/off HDMI Tx output force color format
/// @argument:
///              - bflag: TRUE: force output color format, FALSE: auto mode
///              - output_mode: HDMITX_VIDEO_COLOR_RGB444: RGB, HDMITX_VIDEO_COLOR_YUV444: YUV
/// @return Ture: Set force output color format successfully
///             FALSE: Fail to set force output color format
//------------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_ForceHDMIOutputColorFormat(MS_BOOL bflag, HDMITX_VIDEO_COLOR_FORMAT output_color)
{
    return MDrv_HDMITx_ForceHDMIOutputColorFormat(bflag, (MsHDMITX_VIDEO_COLOR_FORMAT)output_color);
}

//------------------------------------------------------------------------------
/// @brief This routine set flag to mask register write for special case \n
///			e.g. MBoot to APP with logo display
/// @argument:
///              - bFlag: TRUE: Mask register write, FALSE: not Mask
/// @return None
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_DisableRegWrite(MS_BOOL bFlag)
{
    g_bDisableRegWrite = bFlag;
}


//*********************//
//             CEC                 //
//*********************//

//------------------------------------------------------------------------------
/// @brief This routine get EDID physical address
/// @argument:
///              - pdata: two bytes of physical address,  ie. 1.0.0.0 => 0x10 0x00
/// @return None
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_GetEDIDPhyAdr(MS_U8 *pdata)
{
    memcpy(pdata, &gHDMITxInfo.edid_phyadr, 2*sizeof(MS_U8));
}

//------------------------------------------------------------------------------
/// @brief This routine turn on/off HDMI Tx CEC
/// @argument:
///              - bflag: TRUE: Enable, FALSE: Disable
/// @return None
//------------------------------------------------------------------------------
void DLL_PUBLIC MApi_HDMITx_SetCECOnOff(MS_BOOL bflag)
{
    gHDMITxInfo.hdmitx_CECEnable_flag = bflag;
}

//------------------------------------------------------------------------------
/// @brief This routine get HDMI Tx CEC On/Off status
/// @argument: None
/// @return: CEC On/Off status
//------------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_GetCECStatus(void)
{
    return gHDMITxInfo.hdmitx_CECEnable_flag;
}

//------------------------------------------------------------------------------
/// @brief This routine get EDID from reciver
/// @argument: None
/// @return: read status
//------------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_EdidChecking(void)
{
    if (MApi_HDMITx_GetRxStatus() == TRUE)
    {
        MDrv_HDMITx_EdidChecking();

        return gHDMITxInfo.hdmitx_edid_ready;
    }

    return FALSE;
}

//*********************//
//      RxBypassMode         //
//*********************//
//------------------------------------------------------------------------------
/// @brief This routine set Rx Bypass Mode
/// @argument: freq: Input Freq; bflag:DDC/CEC/ARC bypass
/// @return: setting status.
//------------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_RxBypass_Mode(HDMITX_INPUT_FREQ freq, MS_BOOL bflag)
{
    return MDrv_HDMITx_RxBypass_Mode((MsHDMITX_INPUT_FREQ) freq, bflag);
}


//------------------------------------------------------------------------------
/// @brief This routine disable Rx Bypass Mode
/// @return: setting status.
//------------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_Disable_RxBypass(void)
{
    return MDrv_HDMITx_Disable_RxBypass();
}

//------------------------------------------------------------------------------
/// @brief This routine to set the time interval from sent aksv to R0.
/// The HDCP Transmitter must not read the R0' value sooner than 100ms after writing Aksv.
/// @return: setting status.
//------------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_SetAksv2R0Interval(MS_U32 u32Interval)
{
    MDrv_HDMITx_SetAksv2R0Interval(u32Interval);

    return TRUE;
}

//------------------------------------------------------------------------------
/// @brief This routine to Get if Rx support HDCP
/// @return: setting status.
//------------------------------------------------------------------------------
MS_BOOL DLL_PUBLIC MApi_HDMITx_IsHDCPRxValid(void)
{
    return MDrv_HDMITx_IsHDCPRxValid();
}

//******************************************************************************
/// API for Query HDMITx Capability
/// @param eCapType \b IN: Capability type
/// @param pRet     \b OUT: return value
/// @param ret_size \b IN: input structure size
/// @return MS_BOOL - Success
//******************************************************************************
MS_BOOL MApi_HDMITx_GetChipCaps(EN_HDMITX_CAPS eCapType, MS_U32* pRet, MS_U32 ret_size)
{

    MS_BOOL bRet = TRUE;
    switch(eCapType)
    {
        case E_HDMITX_CAP_SUPPORT_DVI:
            *pRet = (MS_U32)MDrv_HDMITx_IsSupportDVIMode();
            break;
        default:
            printf("[%s]not support HDMITx capability case: %d\n",__FUNCTION__, eCapType);
            bRet = FALSE;
    }

    return bRet;
}


// *************  For customer NDS **************//
#ifdef CUSTOMER_NDS

//17.7.7.5 Ioctl: Get Events
MS_BOOL DLL_PUBLIC MApi_HDMITx_Get_Events(MS_U32 *events, MS_BOOL bEraseEvent)
{
    *events = gHDMITxInfo.events;

    if(bEraseEvent)
        gHDMITxInfo.events = 0;
    return TRUE;
}

//17.7.7.8 Ioctl: Get Connection Status
MS_BOOL DLL_PUBLIC MApi_HDMITx_Get_Rx_CONNECTION_STATUS(HDMIConnectionStatus *status)
{
    HDMI_TX_Status Status;

    if(MDrv_HDMITx_GetRxStatus())
        status->attached = 1; // attached
    else
        status->attached = 0; // not attached

    MApi_HDMITx_GetStatus(&Status);
    if(Status.bIsRunning == TRUE)
        status->powered = 1; // powered
    else
        status->powered = 0; // not powered

    return TRUE;
}

//17.7.7.9 Ioctl: Get EEDID Block
/* This function returns 0 if success, otherwise returns non-zero */
MS_BOOL DLL_PUBLIC MApi_HDMITx_Get_EEDID_Block(HDMIGetEEDIDBlock *eedid)
{
    if(!MDrv_HDMITx_GetRxStatus()) // RX is not alive
        return FALSE;

    if(MDrv_HDMITx_EdidReadBlock(eedid->blockNumber, eedid->eedid))
        return TRUE;
    else
        return FALSE;
}

//17.7.7.10 Ioctl: Start
MS_BOOL DLL_PUBLIC MApi_HDMITx_Set_HDMI_Protocol(MS_U8 protocol)
{
    if(!MDrv_HDMITx_GetRxStatus()) // RX is not alive
        return FALSE;

    if(protocol == HDMI_PROTOCOL_DVI)
        MApi_HDMITx_SetHDMITxMode(HDMITX_DVI);
    else if(protocol == HDMI_PROTOCOL_HDMI)
        MApi_HDMITx_SetHDMITxMode(HDMITX_HDMI);

    return TRUE;
}

//17.7.7.11 Ioctl: Stop
MS_BOOL DLL_PUBLIC MApi_HDMITx_Stop(void)
{
    if(!MDrv_HDMITx_GetRxStatus()) // RX is not alive
        return FALSE;

    MApi_HDMITx_SetTMDSOnOff(FALSE);

    return TRUE;
}

//17.7.7.14 Ioctl: Set Video Code
// VIC codes: {6, 21, 2, 17, 19, 4, 20, 5, 32, 33, 34, 31, 16} => 480i60,576i50,480p60,576p50,720p50,720p60,1080i50,1080i60,1080p24,1080p25,1080p30,1080p50,1080p60
MS_BOOL DLL_PUBLIC MApi_HDMITx_Set_Video_Code(MS_U8 *videoCode)
{
    HDMITX_VIDEO_TIMING val = HDMITX_RES_720x480i;

    if(!MDrv_HDMITx_GetRxStatus()) // RX is not alive
        return FALSE;

    if(*videoCode == 1)
        val = HDMITX_RES_640x480p;
    else if(*videoCode == 6)
        val = HDMITX_RES_720x480i;
    else if(*videoCode == 21)
        val = HDMITX_RES_720x576i;
    else if(*videoCode == 2)
        val = HDMITX_RES_720x480p;
    else if(*videoCode == 17)
        val = HDMITX_RES_720x576p;
    else if(*videoCode == 19)
        val = HDMITX_RES_1280x720p_50Hz;
    else if(*videoCode == 4)
        val = HDMITX_RES_1280x720p_60Hz;
    else if(*videoCode == 20)
        val = HDMITX_RES_1920x1080i_50Hz;
    else if(*videoCode == 5)
        val = HDMITX_RES_1920x1080i_60Hz;
    else if(*videoCode == 32)
        val = HDMITX_RES_1920x1080p_24Hz;
    else if(*videoCode == 33)
        val = HDMITX_RES_1920x1080p_25Hz;
    else if(*videoCode == 34)
        val = HDMITX_RES_1920x1080p_30Hz;
    else if(*videoCode == 31)
        val = HDMITX_RES_1920x1080p_50Hz;
    else if(*videoCode == 16)
        val = HDMITX_RES_1920x1080p_60Hz;

    MApi_HDMITx_SetVideoOutputTiming(val);

    return TRUE;
}

//17.7.7.15 Ioctl: Set Pixel Format
MS_BOOL DLL_PUBLIC MApi_HDMITx_Set_Pixel_Format(MS_U8 *pixelFormat)
{
    HDMITX_VIDEO_COLOR_FORMAT val;

    if(!MDrv_HDMITx_GetRxStatus()) // RX is not alive
        return FALSE;
//#if 0
    if(*pixelFormat == HDMI_PIXEL_FORMAT_RGB)
        val = HDMITX_VIDEO_COLOR_RGB444;
    else if(*pixelFormat == HDMI_PIXEL_FORMAT_YCbCr422)
        val = HDMITX_VIDEO_COLOR_YUV422;
    else if(*pixelFormat == HDMI_PIXEL_FORMAT_YCbCr444)
        val = HDMITX_VIDEO_COLOR_YUV444;

    MApi_HDMITx_SetColorFormat(HDMITX_VIDEO_COLOR_YUV444, val);
//#else
    MDrv_HDMITx_Set_AVI_InfoFrame(E_HDMITX_AVI_PIXEL_FROMAT, (MS_U16 *)pixelFormat);
//#endif

    return TRUE;
}

//17.7.7.16 Ioctl: Set Aspect Ratio
MS_BOOL DLL_PUBLIC MApi_HDMITx_Set_Aspect_Ratio(HDMIAspectRatio *aspectRatio)
{
    MS_U16 u16reg_val[6];

    if(!MDrv_HDMITx_GetRxStatus()) // RX is not alive
        return FALSE;

    u16reg_val[0] = ((aspectRatio->pictureAspectRatio)|(aspectRatio->activeFormatDescriptor))<<8 | ((aspectRatio->afdValidity) | (aspectRatio->barinfoValidity) | (aspectRatio->scanInfo));
    u16reg_val[1] = (aspectRatio->scalingInfo) << 8;
    if( (aspectRatio->barinfoValidity == HDMI_BARINFO_INVALID) || (aspectRatio->barinfoValidity == HDMI_BARINFO_VERTICAL_VALID))
    {
        u16reg_val[2] = 0;
        u16reg_val[3] = 0;
    }
    else
    {
        u16reg_val[2] = (aspectRatio->topBarEndsLine);
        u16reg_val[3] = (aspectRatio->bottomBarStartsLine);
    }
    if( (aspectRatio->barinfoValidity == HDMI_BARINFO_INVALID) || (aspectRatio->barinfoValidity == HDMI_BARINFO_HORIZONTAL_VALID))
    {
        u16reg_val[4] = 0;
        u16reg_val[5] = 0;
    }
    else
    {
        u16reg_val[4] = (aspectRatio->leftBarEndsPixel);
        u16reg_val[5] = (aspectRatio->rightBarStartsPixel);
    }
    MDrv_HDMITx_Set_AVI_InfoFrame(E_HDMITX_AVI_ASPECT_RATIO, u16reg_val);

    return TRUE;
}

//17.7.7.17 Ioctl: Set Colorimetry
MS_BOOL DLL_PUBLIC MApi_HDMITx_Set_Colorimetry(HDMIColorimetry *colorimetryData)
{
   MS_U16 u16reg_val[2];

    if(!MDrv_HDMITx_GetRxStatus()) // RX is not alive
        return FALSE;

    u16reg_val[0] = (colorimetryData->colorimetry) << 8;
    u16reg_val[1] = colorimetryData->extendedColorimetry;
    MDrv_HDMITx_Set_AVI_InfoFrame(E_HDMITX_AVI_COLORIMETRY, u16reg_val);

    return TRUE;
}


//17.7.7.18 Ioctl: Set AVMUTE
//17.7.7.19 Ioctl: Clear AVMUTE
MS_BOOL DLL_PUBLIC MApi_HDMITx_Set_AVMUTE(MS_BOOL bflag)
{
    if(!MDrv_HDMITx_GetRxStatus()) // RX is not alive
        return FALSE;

    MDrv_HDMITx_Set_AVMUTE(bflag);
    return TRUE;
}

//17.7.7.20 Ioctl: HDCP Start Authentication
MS_BOOL DLL_PUBLIC MApi_HDMITx_HDCP_Start_Authentication(void)
{
    if(!MDrv_HDMITx_GetRxStatus()) // RX is not alive
        return FALSE;

    //MApi_HDMITx_SetHDCPOnOff(TRUE);
    //MDrv_HDMITx_FSM_ChangeState(E_HDMITX_FSM_HDCP_AUTH_WAIT_RX);
    return MDrv_HDMITx_HDCP_Start_Authentication();

    //return 0;
}

//17.7.7.21 Ioctl: HDCP Stop Authentication
MS_BOOL DLL_PUBLIC MApi_HDMITx_HDCP_Stop_Authentication(void)
{
    if(!MDrv_HDMITx_GetRxStatus()) // RX is not alive
        return -1;

    //MApi_HDMITx_SetHDCPOnOff(FALSE);
    return MDrv_HDMITx_HDCP_Stop_Authentication();

    //return 0;
}

//17.7.7.22 Ioctl: HDCP Get Bcaps
/* This function returns 0 if success, otherwise returns non-zero */
MS_BOOL DLL_PUBLIC MApi_HDMITx_HDCP_Get_BCaps(HDMIHDCPBCaps *arg)
{
    if(!MDrv_HDMITx_GetRxStatus()) // RX is not alive
        return FALSE;

    if(!MDrv_HDMITx_HDCP_Get_BCaps(&(arg->bcaps)))
        return FALSE;

    return TRUE;
}

//17.7.7.23 Ioctl: HDCP Get Bstatus
/* This function returns 0 if success, otherwise returns non-zero */
MS_BOOL DLL_PUBLIC MApi_HDMITx_HDCP_Get_BStatus(HDMIHDCPBStatus *arg)
{
    if(!MDrv_HDMITx_GetRxStatus()) // RX is not alive
        return FALSE;

    if(!MDrv_HDMITx_HDCP_Get_BStatus(&(arg->bstatus)))
        return FALSE;

    return TRUE;
}

//17.7.7.24 Ioctl: HDCP Send SRM List
MS_BOOL DLL_PUBLIC MApi_HDMITx_HDCP_Send_SRM(DigitalCPSrm *srm)
{
    if(MApi_HDMITx_HDCP_IsSRMSignatureValid(srm->data, srm->length))
        return TRUE;
    else
        return FALSE;
}

void DLL_PUBLIC MApi_HDMITx_HDCP_Reset_SRM(void)
{
    MDrv_HDMITx_HDCP_Reset_SRM();
}

//17.7.7.25 Ioctl: HDCP Encryption Control
MS_BOOL DLL_PUBLIC MApi_HDMITx_HDCP_Encryption_Enable(MS_U8 control)
{
    if(!MDrv_HDMITx_GetRxStatus()) // RX is not alive
        return FALSE;

    if(control)	// enable encryption
        MDrv_HDMITx_HdcpSetEncrypt(TRUE);
    else // disable encryption
        MDrv_HDMITx_HdcpSetEncrypt(FALSE);

    return TRUE;
}

//17.7.7.26 Ioctl: HDCP Get Status
MS_BOOL DLL_PUBLIC MApi_HDMITx_HDCP_Get_HDCP_Status(MS_U8 *status)
{
    if(!MDrv_HDMITx_GetRxStatus()) // RX is not alive
        return FALSE;

    MDrv_HDMITx_HDCP_Get_HDCP_Status(status);

    return TRUE;
}

//17.7.7.27 Ioctl: HDCP Configure
MS_BOOL DLL_PUBLIC MApi_HDMITx_HDCP_Configure(HDMIHDCPConfiguration *arg)
{
    MDrv_HDMITx_HDCP_Configure(arg);

    return TRUE;
}

#endif //CUSTOMER_NDS

