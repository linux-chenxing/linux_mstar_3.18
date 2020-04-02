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

#ifndef _HAL_HDMITX_H_
#define _HAL_HDMITX_H_


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

typedef enum
{
    E_HDMITX_DVIClock_L_HPD_L     = 0,
    E_HDMITX_DVIClock_L_HPD_H     = 1,
    E_HDMITX_DVIClock_H_HPD_L     = 2,
    E_HDMITX_DVIClock_H_HPD_H     = 3,
} MsHDMITX_RX_STATUS;

typedef enum
{
    E_HDMITX_SEND_PACKET        = 0x00,   // send packet
    E_HDMITX_CYCLIC_PACKET      = 0x04,   // cyclic packet by frame count
    E_HDMITX_STOP_PACKET        = 0x80,   // stop packet
} MsHDMITX_PACKET_PROCESS;

typedef enum
{
    E_HDMITX_NULL_PACKET        = 0x00,
    E_HDMITX_ACR_PACKET         = 0x01,
    E_HDMITX_AS_PACKET          = 0x02,
    E_HDMITX_GC_PACKET          = 0x03,
    E_HDMITX_ACP_PACKET         = 0x04,
    E_HDMITX_ISRC1_PACKET       = 0x05,
    E_HDMITX_ISRC2_PACKET       = 0x06,
    E_HDMITX_DSD_PACKET         = 0x07,
    E_HDMITX_HBR_PACKET         = 0x09,
    E_HDMITX_GM_PACKET         = 0x0A,

    E_HDMITX_VS_INFOFRAME       = 0x81,
    E_HDMITX_AVI_INFOFRAME      = 0x82,
    E_HDMITX_SPD_INFOFRAME      = 0x83,
    E_HDMITX_AUDIO_INFOFRAME    = 0x84,
    E_HDMITX_MPEG_INFOFRAME     = 0x85,
} MsHDMITX_PACKET_TYPE;

typedef enum
{
    E_HDMITX_GCP_NO_CMD         = 0,
    E_HDMITX_GCP_SET_AVMUTE     = 1,
    E_HDMITX_GCP_CLEAR_AVMUTE   = 2,
    E_HDMITX_GCP_NA             = 3,
} MsHDMITX_PACKET_GCP_STATUS;

//*********************//
//             Video   //
//*********************//

typedef enum
{
    E_HDMITX_VIDEO_CD_NoID     = 0,
    E_HDMITX_VIDEO_CD_24Bits     = 4,
    E_HDMITX_VIDEO_CD_30Bits     = 5,
    E_HDMITX_VIDEO_CD_36Bits     = 6,
    E_HDMITX_VIDEO_CD_48Bits     = 7,
} MsHDMITX_VIDEO_COLORDEPTH_VAL;

typedef enum
{
    E_HDMITX_VIDEO_COLOR_RGB444     = 0,
    E_HDMITX_VIDEO_COLOR_YUV422     = 1,
    E_HDMITX_VIDEO_COLOR_YUV444     = 2,
} MsHDMITX_VIDEO_COLOR_FORMAT;

typedef enum
{
    E_HDMITX_RES_640x480p       = 0,
    E_HDMITX_RES_720x480i       = 1,
    E_HDMITX_RES_720x576i       = 2,
    E_HDMITX_RES_720x480p       = 3,
    E_HDMITX_RES_720x576p       = 4,
    E_HDMITX_RES_1280x720p_50Hz   = 5,
    E_HDMITX_RES_1280x720p_60Hz   = 6,
    E_HDMITX_RES_1920x1080i_50Hz  = 7,
    E_HDMITX_RES_1920x1080i_60Hz  = 8,
    E_HDMITX_RES_1920x1080p_24Hz  = 9,
    E_HDMITX_RES_1920x1080p_25Hz  = 10,
    E_HDMITX_RES_1920x1080p_30Hz  = 11,
    E_HDMITX_RES_1920x1080p_50Hz  = 12,
    E_HDMITX_RES_1920x1080p_60Hz  = 13,
    E_HDMITX_RES_MAX              = 14,
} MsHDMITX_VIDEO_TIMING;

typedef enum
{
    E_HDMITX_VIC_NOT_AVAILABLE       = 0,
    E_HDMITX_VIC_640x480p_60_4_3	 = 1,
    E_HDMITX_VIC_720x480p_60_4_3	 = 2,
    E_HDMITX_VIC_720x480p_60_16_9	 = 3,
    E_HDMITX_VIC_1280x720p_60_16_9	 = 4,
    E_HDMITX_VIC_1920x1080i_60_16_9	 = 5,
    E_HDMITX_VIC_720x480i_60_4_3	 = 6,
    E_HDMITX_VIC_720x480i_60_16_9    = 7,
    E_HDMITX_VIC_720x240p_60_4_3	 = 8,
    E_HDMITX_VIC_720x240p_60_16_9	 = 9,
    E_HDMITX_VIC_2880x480i_60_4_3	 = 10,
    E_HDMITX_VIC_2880x480i_60_16_9	 = 11,
    E_HDMITX_VIC_2880x240p_60_4_3	 = 12,
    E_HDMITX_VIC_2880x240p_60_16_9	 = 13,
    E_HDMITX_VIC_1440x480p_60_4_3    = 14,
    E_HDMITX_VIC_1440x480p_60_16_9	 = 15,
    E_HDMITX_VIC_1920x1080p_60_16_9	 = 16,
    E_HDMITX_VIC_720x576p_50_4_3	 = 17,
    E_HDMITX_VIC_720x576p_50_16_9	 = 18,
    E_HDMITX_VIC_1280x720p_50_16_9	 = 19,
    E_HDMITX_VIC_1920x1080i_50_16_9	 = 20,
    E_HDMITX_VIC_720x576i_50_4_3     = 21,
    E_HDMITX_VIC_720x576i_50_16_9	 = 22,
    E_HDMITX_VIC_720x288p_50_4_3	 = 23,
    E_HDMITX_VIC_720x288p_50_16_9	 = 24,
    E_HDMITX_VIC_2880x576i_50_4_3	 = 25,
    E_HDMITX_VIC_2880x576i_50_16_9	 = 26,
    E_HDMITX_VIC_2880x288p_50_4_3	 = 27,
    E_HDMITX_VIC_2880x288p_50_16_9   = 28,
    E_HDMITX_VIC_1440x576p_50_4_3	 = 29,
    E_HDMITX_VIC_1440x576p_50_16_9	 = 30,
    E_HDMITX_VIC_1920x1080p_50_16_9	 = 31,
    E_HDMITX_VIC_1920x1080p_24_16_9	 = 32,
    E_HDMITX_VIC_1920x1080p_25_16_9	 = 33,
    E_HDMITX_VIC_1920x1080p_30_16_9	 = 34,
    E_HDMITX_VIC_2880x480p_60_4_3    = 35,
    E_HDMITX_VIC_2880x480p_60_16_9	 = 36,
    E_HDMITX_VIC_2880x576p_50_4_3	 = 37,
    E_HDMITX_VIC_2880x576p_50_16_9	 = 38,
    E_HDMITX_VIC_1920x1080i_50_16_9_1250_total	 = 39,
    E_HDMITX_VIC_1920x1080i_100_16_9 = 40,
    E_HDMITX_VIC_1280x720p_100_16_9	 = 41,
    E_HDMITX_VIC_720x576p_100_4_3    = 42,
    E_HDMITX_VIC_720x576p_100_16_9	 = 43,
    E_HDMITX_VIC_720x576i_100_4_3	 = 44,
    E_HDMITX_VIC_720x576i_100_16_9	 = 45,
    E_HDMITX_VIC_1920x1080i_120_16_9 = 46,
    E_HDMITX_VIC_1280x720p_120_16_9	 = 47,
    E_HDMITX_VIC_720x480p_120_4_3	 = 48,
    E_HDMITX_VIC_720x480p_120_16_9   = 49,
    E_HDMITX_VIC_720x480i_120_4_3	 = 50,
    E_HDMITX_VIC_720x480i_120_16_9	 = 51,
    E_HDMITX_VIC_720x576p_200_4_3	 = 52,
    E_HDMITX_VIC_720x576p_200_16_9	 = 53,
    E_HDMITX_VIC_720x576i_200_4_3	 = 54,
    E_HDMITX_VIC_720x576i_200_16_9	 = 55,
    E_HDMITX_VIC_720x480p_240_4_3    = 56,
    E_HDMITX_VIC_720x480p_240_16_9	 = 57,
    E_HDMITX_VIC_720x480i_240_4_3	 = 58,
    E_HDMITX_VIC_720x480i_240_16_9	 = 59,
    E_HDMITX_VIC_1280x720p_24_16_9	 = 60,
    E_HDMITX_VIC_1280x720p_25_16_9	 = 61,
    E_HDMITX_VIC_1280x720p_30_16_9	 = 62,
    E_HDMITX_VIC_1920x1080p_120_16_9 = 63,
    E_HDMITX_VIC_1920x1080p_100_16_9 = 64,
} MsHDMITX_AVI_VIC;

typedef enum
{
    E_HDMITX_VIDEO_AR_Reserved   = 0,
    E_HDMITX_VIDEO_AR_4_3     = 1,
    E_HDMITX_VIDEO_AR_16_9   = 2,
} MsHDMITX_VIDEO_ASPECT_RATIO;

typedef enum
{
    E_HDMITX_VIDEO_SI_NoData    = 0,
    E_HDMITX_VIDEO_SI_Overscanned         = 1,
    E_HDMITX_VIDEO_SI_Underscanned        = 2,
    E_HDMITX_VIDEO_SI_Reserved    = 3,
} MsHDMITX_VIDEO_SCAN_INFO;


typedef enum
{
    E_HDMITX_VIDEO_AFD_SameAsPictureAR    = 8, // 1000
    E_HDMITX_VIDEO_AFD_4_3_Center         = 9, // 1001
    E_HDMITX_VIDEO_AFD_16_9_Center        = 10, // 1010
    E_HDMITX_VIDEO_AFD_14_9_Center        = 11, // 1011
    E_HDMITx_VIDEO_AFD_Others = 15, // 0000~ 0111, 1100 ~ 1111
} MsHDMITX_VIDEO_AFD_RATIO;


typedef enum
{
    E_HDMITX_VIDEO_VS_No_Addition    = 0, // 000
    E_HDMITX_VIDEO_VS_4k_2k          = 1, // 001
    E_HDMITX_VIDEO_VS_3D             = 2, // 010
    E_HDMITx_VIDEO_VS_Reserved       = 7, // 011~ 111
} MsHDMITX_VIDEO_VS_FORMAT;


typedef enum
{
    E_HDMITX_VIDEO_3D_FramePacking     = 0, // 0000
    E_HDMITX_VIDEO_3D_FieldAlternative = 1, // 0001
    E_HDMITX_VIDEO_3D_LineAlternative  = 2, // 0010
    E_HDMITX_VIDEO_3D_SidebySide_FULL  = 3, // 0011
    E_HDMITX_VIDEO_3D_L_Dep            = 4, // 0100
    E_HDMITX_VIDEO_3D_L_Dep_Graphic_Dep= 5, // 0101
    E_HDMITX_VIDEO_3D_TopandBottom     = 6, // 0110
    E_HDMITX_VIDEO_3D_SidebySide_Half  = 8, // 1000
    E_HDMITx_VIDEO_3D_Not_in_Use       = 15, // 1111
} MsHDMITX_VIDEO_3D_STRUCTURE;

typedef enum
{
    E_HDMITX_EDID_3D_FramePacking               = 1, // 3D_STRUCTURE_ALL_0
    E_HDMITX_EDID_3D_FieldAlternative           = 2, // 3D_STRUCTURE_ALL_1
    E_HDMITX_EDID_3D_LineAlternative            = 4, // 3D_STRUCTURE_ALL_2
    E_HDMITX_EDID_3D_SidebySide_FULL            = 8, // 3D_STRUCTURE_ALL_3
    E_HDMITX_EDID_3D_L_Dep                      = 16, // 3D_STRUCTURE_ALL_4
    E_HDMITX_EDID_3D_L_Dep_Graphic_Dep          = 32, // 3D_STRUCTURE_ALL_5
    E_HDMITX_EDID_3D_TopandBottom               = 64, // 3D_STRUCTURE_ALL_6
    E_HDMITX_EDID_3D_SidebySide_Half_horizontal = 256, // 3D_STRUCTURE_ALL_8
    E_HDMITX_EDID_3D_SidebySide_Half_quincunx   = 32768, // 3D_STRUCTURE_ALL_15
} MsHDMITX_EDID_3D_STRUCTURE_ALL;



typedef enum
{
    E_HDMITx_VIDEO_4k2k_Reserved    = 0, // 0x00
    E_HDMITX_VIDEO_4k2k_30Hz        = 1, // 0x01
    E_HDMITX_VIDEO_4k2k_25Hz        = 2, // 0x02
    E_HDMITX_VIDEO_4k2k_24Hz        = 3, // 0x03
    E_HDMITx_VIDEO_4k2k_24Hz_SMPTE  = 4, // 0x04
} MsHDMITX_VIDEO_4k2k_VIC;


//*********************//
//             Audio   //
//*********************//

typedef enum
{
    E_HDMITX_AUDIO_FREQ_NO_SIG  = 0,
    E_HDMITX_AUDIO_32K          = 1,
    E_HDMITX_AUDIO_44K          = 2,
    E_HDMITX_AUDIO_48K          = 3,
    E_HDMITX_AUDIO_88K          = 4,
    E_HDMITX_AUDIO_96K          = 5,
    E_HDMITX_AUDIO_176K         = 6,
    E_HDMITX_AUDIO_192K         = 7,
    E_HDMITX_AUDIO_FREQ_MAX_NUM = 8,
} MsHDMITX_AUDIO_FREQUENCY;

typedef enum
{
    E_HDMITX_AUDIO_FORMAT_PCM   = 0,
    E_HDMITX_AUDIO_FORMAT_DSD   = 1,
    E_HDMITX_AUDIO_FORMAT_HBR   = 2,
    E_HDMITX_AUDIO_FORMAT_NA    = 3,
} MsHDMITX_AUDIO_SOURCE_FORMAT;

typedef enum
{
    E_HDMITX_AUDIO_CH_2  = 2, // 2 channels
    E_HDMITX_AUDIO_CH_4  = 4, // 4 channels
    E_HDMITX_AUDIO_CH_6  = 6, // 6 channels
    E_HDMITX_AUDIO_CH_8  = 8, // 8 channels
} MsHDMITX_AUDIO_CHANNEL_COUNT;

typedef enum
{
    E_HDMITX_AUDIO_PCM  = 0, // PCM
    E_HDMITX_AUDIO_NONPCM  = 1, // non-PCM
} MsHDMITX_AUDIO_CODING_TYPE;

typedef enum
{
    E_HDMITX_IRQ_00             = 0x000001,   // IRQ event 0x00
    E_HDMITX_IRQ_01             = 0x000002,   // IRQ event 0x01
    E_HDMITX_IRQ_02             = 0x000004,   // IRQ event 0x02
    E_HDMITX_IRQ_03             = 0x000008,   // IRQ event 0x03
    E_HDMITX_IRQ_04             = 0x000010,   // IRQ event 0x04
    E_HDMITX_IRQ_05             = 0x000020,   // HDCP 128th Ri
    E_HDMITX_IRQ_06             = 0x000040,   // HDCP 127th Ri
    E_HDMITX_IRQ_07             = 0x000080,   // HDCP Pj
    E_HDMITX_IRQ_08             = 0x000100,   // TMDS timing error
    E_HDMITX_IRQ_09             = 0x000200,   // IRQ event 0x09
    E_HDMITX_IRQ_0A             = 0x000400,  // Video engine image size change
    E_HDMITX_IRQ_0B             = 0x000800,  // Audio FIFO Overflow
    E_HDMITX_IRQ_0C             = 0x001000,  // TMDS_DE_CNT_DIFF, defined by U4
    E_HDMITX_IRQ_0D             = 0x002000,  // IRQ event 0x0D
    E_HDMITX_IRQ_0E             = 0x004000,  // IRQ event 0x0E
    E_HDMITX_IRQ_0F             = 0x008000,  // IRQ event 0x0F
    E_HDMITX_IRQ_10             = 0x010000,  // TMDS detect RX disconnection (Rx sense)
    E_HDMITX_IRQ_11             = 0x020000,  // TMDS detect Rx disconnection in PM
    E_HDMITX_IRQ_12             = 0x040000,  // HPD(real function is in PM_Sleep)
    E_HDMITX_IRQ_13             = 0x080000,  // IRQ event 0x13
    E_HDMITX_IRQ_14             = 0x100000,  // IRQ event 0x14
    E_HDMITX_IRQ_15             = 0x200000,  // IRQ event 0x15
    E_HDMITX_IRQ_16             = 0x400000,  // IRQ event 0x16
    E_HDMITX_IRQ_17             = 0x800000,  // IRQ event 0x17
    E_HDMITX_IRQ_ALL            = 0xFFFFFF, // IRQ all
}MsHDMITX_INTERRUPT_TYPE;

typedef enum
{
    E_HDMITX_INPUT_LESS_60MHZ  =0,
    E_HDMITX_INPUT_60_to_160MHZ  =1,
    E_HDMITX_INPUT_OVER_160MHZ  =2,
} MsHDMITX_INPUT_FREQ;

typedef struct
{
    // HDMI Tx Current, Pre-emphasis and Double termination
    MS_U8    tm_txcurrent; // TX current control(U4: 0x11302B[13:12], K1: 0x11302B[13:11])
    MS_U8    tm_pren2; // pre-emphasis mode control, 0x11302D[5]
    MS_U8    tm_precon; // TM_PRECON, 0x11302E[7:4]
    MS_U8    tm_pren; // pre-emphasis enable, 0x11302E[11:8]
    MS_U8    tm_tenpre; // Double termination pre-emphasis enable, 0x11302F[3:0]
    MS_U8    tm_ten; // Double termination enable, 0x11302F[7:4]
} MsHDMITX_ANALOG_TUNING;

typedef struct
{
    MsHDMITX_AVI_VIC support_timing;
    MsHDMITX_EDID_3D_STRUCTURE_ALL support_3D_structure;
} MsHDMITX_EDID_3D_SUPPORT_TIMING;


// *************  For customer NDS **************//
typedef enum
{
    E_HDMITX_AVI_PIXEL_FROMAT = 0, // Y[1:0]
    E_HDMITX_AVI_ASPECT_RATIO  = 1, // AR, AFD, TBEL, BBSL, LBEP, RBSP, BIValid, AFDValid, ScanInfo, ScalingInfo
    E_HDMITX_AVI_COLORIMETRY  = 2 // Colormetry and extended colorimetry
} MsHDMITX_AVI_CONTENT_TYPE;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
#ifdef MHAL_HDMITX_C
#define INTERFACEE
#else
#define INTERFACEE extern
#endif

INTERFACEE void MHal_HDMITx_Int_Disable(MS_U32 u32Int);
INTERFACEE void MHal_HDMITx_Int_Enable(MS_U32 u32Int);
INTERFACEE void MHal_HDMITx_Int_Clear(MS_U32 u32Int);
INTERFACEE MS_U32 MHal_HDMITx_Int_Status(void);
INTERFACEE MsHDMITX_RX_STATUS MHal_HDMITx_GetRXStatus(void);
INTERFACEE void MHal_HDMITX_SetHDCPConfig(MS_U8 HDCP_mode);
INTERFACEE MS_U16 MHal_HDMITX_GetM02Bytes(MS_U16 idx);

INTERFACEE void MHal_HDMITx_InitSeq(void);
INTERFACEE void MHal_HDMITx_VideoInit(void);
INTERFACEE void MHal_HDMITx_AudioInit(void);
INTERFACEE void MHal_HDMITx_PLLOnOff(MS_BOOL bflag);
INTERFACEE void MHal_HDMITx_PKT_User_Define_Clear(void);
INTERFACEE void MHal_HDMITx_PKT_User_Define(MsHDMITX_PACKET_TYPE packet_type, MS_BOOL def_flag,
        MsHDMITX_PACKET_PROCESS def_process, MS_U8 def_fcnt);
INTERFACEE MS_BOOL MHal_HDMITx_PKT_Content_Define(MsHDMITX_PACKET_TYPE packet_type, MS_U8* data, MS_U8 length);
INTERFACEE void MHal_HDMITx_SetTMDSOnOff(MS_BOOL bRB_Swap, MS_BOOL bTMDS);
INTERFACEE void MHal_HDMITx_SetVideoOnOff(MS_BOOL bVideo, MS_BOOL bCSC, MS_BOOL b709format);
INTERFACEE void MHal_HDMITx_SetColorFormat(MS_BOOL bCSC, MS_BOOL bHdmi422b12, MS_BOOL b709format);
INTERFACEE MS_U8 MHal_HDMITx_InfoFrameCheckSum(MsHDMITX_PACKET_TYPE packet_type);
INTERFACEE void MHal_HDMITx_EnablePacketGen(MS_BOOL bflag);
INTERFACEE void MHal_HDMITx_SetHDMImode(MS_BOOL bflag, MsHDMITX_VIDEO_COLORDEPTH_VAL cd_val);
INTERFACEE void MHal_HDMITx_SetAudioOnOff(MS_BOOL bflag);
INTERFACEE void MHal_HDMITx_SetAudioFrequency(MsHDMITX_AUDIO_FREQUENCY afidx,
        MsHDMITX_AUDIO_CHANNEL_COUNT achidx, MsHDMITX_AUDIO_CODING_TYPE actidx
);
INTERFACEE void MHal_HDMITx_SetAudioSourceFormat(MsHDMITX_AUDIO_SOURCE_FORMAT fmt);
INTERFACEE MS_U32 MHal_HDMITx_GetAudioCTS(void);
INTERFACEE void MHal_HDMITx_MuteAudioFIFO(MS_BOOL bflag);
INTERFACEE void MHal_HDMITx_SetHDCPOnOff(MS_BOOL hdcp_flag, MS_BOOL hdmi_flag);
INTERFACEE void MHal_HDMITx_SendPacket(MsHDMITX_PACKET_TYPE packet_type, MS_U8 packet_process,
        MsHDMITX_VIDEO_COLORDEPTH_VAL vcdidx, MsHDMITX_VIDEO_TIMING vtidx, MsHDMITX_VIDEO_COLOR_FORMAT vcidx,
        MsHDMITX_VIDEO_ASPECT_RATIO varidx, MS_BOOL bafd_override, MsHDMITX_VIDEO_SCAN_INFO vsidx, MsHDMITX_VIDEO_AFD_RATIO vafdidx, MS_U8 a0,
        MsHDMITX_AUDIO_FREQUENCY afidx, MsHDMITX_AUDIO_CHANNEL_COUNT achidx, MsHDMITX_AUDIO_CODING_TYPE actidx, MsHDMITX_VIDEO_VS_FORMAT vsfmt,
        MsHDMITX_VIDEO_3D_STRUCTURE vs3d, MsHDMITX_VIDEO_4k2k_VIC vsvic
);
INTERFACEE void MHal_HDMITx_SetVideoOutputMode(MsHDMITX_VIDEO_TIMING idx, MS_BOOL bflag, MsHDMITX_VIDEO_COLORDEPTH_VAL cd_val, MsHDMITX_ANALOG_TUNING *pInfo);
INTERFACEE void MHal_HDMITx_Power_OnOff(MS_BOOL bEnable);
INTERFACEE void MHal_HDMITx_SetHPDGpioPin(MS_U8 u8pin);
INTERFACEE MS_BOOL MHal_HDMITx_RxBypass_Mode(MsHDMITX_INPUT_FREQ freq, MS_BOOL bflag);
INTERFACEE MS_BOOL MHal_HDMITx_Disable_RxBypass(void);
INTERFACEE MS_BOOL MHal_HDMITx_IsSupportDVIMode(void);

// *************  For customer NDS **************//
INTERFACEE void MHal_HDMITx_Set_AVI_InfoFrame(MsHDMITX_PACKET_PROCESS packet_process, MsHDMITX_AVI_CONTENT_TYPE content_type, MS_U16 *data);

#endif // _HAL_HDMITX_H_

