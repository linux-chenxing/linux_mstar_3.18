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

#define  MHAL_HDMITX_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#include "MsCommon.h"

// Internal Definition
#include "regHDMITx.h"
#include "halHDMIUtilTx.h"
#include "halHDMITx.h"
// External Definition
//#include "../../drv/gpio/drvGPIO.h"


//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define     IS_STOP_PKT(_X_)            ( (_X_ & E_HDMITX_STOP_PACKET) ? 1 : 0 )
#define     IS_CYCLIC_PKT(_X_)          ( (_X_ & E_HDMITX_CYCLIC_PACKET) ? 1 : 0 )

// HDMI packet cyclic frame count
#define HDMITX_PACKET_NULL_FCNT         0           ///< 0 ~ 31
#define HDMITX_PACKET_ACR_FCNT          0           ///< 0 ~ 15
#define HDMITX_PACKET_GC_FCNT           0           ///< 0 ~ 1
#define HDMITX_PACKET_ACP_FCNT          15          ///< 0 ~ 31
#define HDMITX_PACKET_ISRC_FCNT         15          ///< 0 ~ 31

#define HDMITX_PACKET_VS_FCNT           0          ///< 0 ~ 31
#define HDMITX_PACKET_AVI_FCNT          0           ///< 0 ~ 31
#define HDMITX_PACKET_SPD_FCNT          15          ///< 0 ~ 31
#define HDMITX_PACKET_AUD_FCNT          0           ///< 0 ~ 31

#define HDMITX_PACKET_SPD_SDI           1   // Digital STB

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//*********************//
//             Video   //
//*********************//

typedef enum
{
    E_HDMITX_VIDEO_INTERLACE_MODE     = 0,
    E_HDMITX_VIDEO_PROGRESSIVE_MODE = 1,
} MDrv_HDMITx_VIDEO_MODE;

typedef enum
{
    E_HDMITX_VIDEO_POLARITY_HIGH  = 0,
    E_HDMITX_VIDEO_POLARITY_LOW = 1,
} MDrv_HDMITx_VIDEO_POLARITY;

typedef struct
{
    MDrv_HDMITx_VIDEO_MODE          i_p_mode;         // interlace / progressive mode
    MDrv_HDMITx_VIDEO_POLARITY   h_polarity;        // Hsync polarity
    MDrv_HDMITx_VIDEO_POLARITY   v_polarity;        // Vsync polarity
    MS_U16                                            vs_width;          // Vsync pulse width
    MS_U16                                            vs_bporch;        // Vsync back-porch
    MS_U16                                            vde_width;        // Vde active width
    MS_U16                                            vs_delayline;    // Vsync line delay
    MS_U16                                            vs_delaypixel;  // Vsync pixel delay
    MS_U16                                            hs_width;          // Hsync pulse width
    MS_U16                                            hs_bporch;        // Hsync back-porch
    MS_U16                                            hde_width;        // Hde active width
    MS_U16                                            hs_delay;          // Hsync delay
    MS_U16                                            vtotal;               // Vsync total
    MS_U16                                            htotal;	         // Hsync total
} MDrv_HDMITx_VIDEO_MODE_INFO_TYPE;

//*********************//
//             Packet  //
//*********************//

typedef enum
{
    E_HDMITX_ACT_GCP_CMD    = 0,
    E_HDMITX_ACT_ACR_CMD    = 1,
    E_HDMITX_ACT_AVI_CMD    = 2,
    E_HDMITX_ACT_AUD_CMD    = 3,
    E_HDMITX_ACT_SPD_CMD    = 4,
    E_HDMITX_ACT_MPG_CMD    = 5,
    E_HDMITX_ACT_VSP_CMD    = 6,
    E_HDMITX_ACT_NUL_CMD    = 7,
    E_HDMITX_ACT_ACP_CMD    = 8,
    E_HDMITX_ACT_ISRC_CMD    = 9,
    E_HDMITX_ACT_GCP_DC_CMD    = 10, // GCP with non-zero CD
    E_HDMITX_ACT_GMP_CMD    = 11, // Gamut Metadata packet
} MDrvHDMITX_PKTS_ACT_CMD;

typedef struct PKT
{
    MS_BOOL User_Define;
    MsHDMITX_PACKET_PROCESS Define_Process;
    MS_U8 Define_FCnt;
}PKT_Behavior;
//*********************//
//             Audio   //
//*********************//
typedef struct
{
    MS_U8     CH_Status3;
    MS_U32   NcodeValue;
} MDrv_HDMITx_AUDIO_FREQ_TYPE;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

// User defined packet behavior
PKT_Behavior NULL_PACKET = {FALSE, E_HDMITX_STOP_PACKET, 0};
PKT_Behavior ACR_PACKET = {FALSE, E_HDMITX_STOP_PACKET, 0};
PKT_Behavior AS_PACKET = {FALSE, E_HDMITX_STOP_PACKET, 0};
PKT_Behavior GC_PACKET = {FALSE, E_HDMITX_STOP_PACKET, 0};
PKT_Behavior ACP_PACKET = {FALSE, E_HDMITX_STOP_PACKET, 0};
PKT_Behavior ISRC1_PACKET = {FALSE, E_HDMITX_STOP_PACKET, 0};
PKT_Behavior ISRC2_PACKET = {FALSE, E_HDMITX_STOP_PACKET, 0};
PKT_Behavior DSD_PACKET = {FALSE, E_HDMITX_STOP_PACKET, 0};
PKT_Behavior HBR_PACKET = {FALSE, E_HDMITX_STOP_PACKET, 0};
PKT_Behavior GM_PACKET = {FALSE, E_HDMITX_STOP_PACKET, 0};

PKT_Behavior VS_INFORAME = {FALSE, E_HDMITX_STOP_PACKET, 0};
PKT_Behavior AVI_INFORAME = {FALSE, E_HDMITX_STOP_PACKET, 0};
PKT_Behavior SPD_INFORAME = {FALSE, E_HDMITX_STOP_PACKET, 0};
PKT_Behavior AUDIO_INFORAME = {FALSE, E_HDMITX_STOP_PACKET, 0};
PKT_Behavior MPEG_INFORAME = {FALSE, E_HDMITX_STOP_PACKET, 0};
//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

//*********************//
//             Video   //
//*********************//

// It should be mapped with MsHDMITX_VIDEO_TIMING structure in drvHDMITx.h
MDrv_HDMITx_VIDEO_MODE_INFO_TYPE HDMITxVideoModeTbl[E_HDMITX_RES_MAX]=
{
    {E_HDMITX_VIDEO_PROGRESSIVE_MODE, E_HDMITX_VIDEO_POLARITY_LOW, E_HDMITX_VIDEO_POLARITY_LOW, 0x0002, 0x0021, 0x01E0, 0x000A, 0, 0x0060, 0x0030, 0x0280, 0x0000, 0x020D, 0x0320},
    {E_HDMITX_VIDEO_INTERLACE_MODE, E_HDMITX_VIDEO_POLARITY_LOW, E_HDMITX_VIDEO_POLARITY_LOW, 0x0003, 0x000F, 0x00F0, 0x0004, 0, 0x007C, 0x0072, 0x05A0, 0x0000, 0x0106, 0x06B4},
    {E_HDMITX_VIDEO_INTERLACE_MODE, E_HDMITX_VIDEO_POLARITY_LOW, E_HDMITX_VIDEO_POLARITY_LOW, 0x0003, 0x0013, 0x0120, 0x0002, 0, 0x007E, 0x008A, 0x05A0, 0x0000, 0x0138, 0x06C0},
    {E_HDMITX_VIDEO_PROGRESSIVE_MODE, E_HDMITX_VIDEO_POLARITY_LOW, E_HDMITX_VIDEO_POLARITY_LOW, 0x0006, 0x001E, 0x01E0, 0x0009, 0, 0x003E, 0x003C, 0x02D0, 0x0000, 0x020D, 0x035A},
    {E_HDMITX_VIDEO_PROGRESSIVE_MODE, E_HDMITX_VIDEO_POLARITY_LOW, E_HDMITX_VIDEO_POLARITY_LOW, 0x0005, 0x0027, 0x0240, 0x0005, 0, 0x0040, 0x0044, 0x02D0, 0x0000, 0x0271, 0x0360},
    {E_HDMITX_VIDEO_PROGRESSIVE_MODE, E_HDMITX_VIDEO_POLARITY_HIGH, E_HDMITX_VIDEO_POLARITY_HIGH, 0x0005, 0x0014, 0x02D0, 0x0005, 0, 0x0028, 0x00DC, 0x0500, 0x0000, 0x02EE, 0x07BC},
    {E_HDMITX_VIDEO_PROGRESSIVE_MODE, E_HDMITX_VIDEO_POLARITY_HIGH, E_HDMITX_VIDEO_POLARITY_HIGH, 0x0005, 0x0014, 0x02D0, 0x0005, 0, 0x0028, 0x00DC, 0x0500, 0x0000, 0x02EE, 0x0672},
    {E_HDMITX_VIDEO_INTERLACE_MODE, E_HDMITX_VIDEO_POLARITY_HIGH, E_HDMITX_VIDEO_POLARITY_HIGH, 0x0005, 0x000F, 0x021C, 0x0002, 0, 0x002C, 0x0094, 0x0780, 0x0000, 0x0232, 0x0A50},
    {E_HDMITX_VIDEO_INTERLACE_MODE, E_HDMITX_VIDEO_POLARITY_HIGH, E_HDMITX_VIDEO_POLARITY_HIGH, 0x0005, 0x000F, 0x021C, 0x0002, 0, 0x002C, 0x0094, 0x0780, 0x0000, 0x0232, 0x0898},
    {E_HDMITX_VIDEO_PROGRESSIVE_MODE, E_HDMITX_VIDEO_POLARITY_HIGH, E_HDMITX_VIDEO_POLARITY_HIGH, 0x0005, 0x0024, 0x0438, 0x0004, 0, 0x002C, 0x0094, 0x0780, 0x0000, 0x0465, 0x0ABE},
    {E_HDMITX_VIDEO_PROGRESSIVE_MODE, E_HDMITX_VIDEO_POLARITY_HIGH, E_HDMITX_VIDEO_POLARITY_HIGH, 0x0005, 0x0024, 0x0438, 0x0004, 0, 0x002C, 0x0094, 0x0780, 0x0000, 0x0465, 0x0A50},
    {E_HDMITX_VIDEO_PROGRESSIVE_MODE, E_HDMITX_VIDEO_POLARITY_HIGH, E_HDMITX_VIDEO_POLARITY_HIGH, 0x0005, 0x0024, 0x0438, 0x0004, 0, 0x002C, 0x0094, 0x0780, 0x0000, 0x0465, 0x0898},
    {E_HDMITX_VIDEO_PROGRESSIVE_MODE, E_HDMITX_VIDEO_POLARITY_HIGH, E_HDMITX_VIDEO_POLARITY_HIGH, 0x0005, 0x0024, 0x0438, 0x0004, 0, 0x002C, 0x0094, 0x0780, 0x0000, 0x0465, 0x0A50},
    {E_HDMITX_VIDEO_PROGRESSIVE_MODE, E_HDMITX_VIDEO_POLARITY_HIGH, E_HDMITX_VIDEO_POLARITY_HIGH, 0x0005, 0x0024, 0x0438, 0x0004, 0, 0x002C, 0x0094, 0x0780, 0x0000, 0x0465, 0x0898},
};

//*********************//
//             Audio   //
//*********************//

MSTHDMITX_REG_TYPE HDMITxAudioOnTbl[] =
{
    {HDMITX_AUDIO_REG_BASE, REG_AE_CONFIG_05, 0x1087, 0x1086}, //[12]: CH status swap[7:0], [7]: enable audio FIFO, [2]:enable CTS Gen, [1]: automatically block start, [0]: audio FIFO not flush
};

MSTHDMITX_REG_TYPE HDMITxAudioOffTbl[] =
{
    {HDMITX_AUDIO_REG_BASE, REG_AE_CONFIG_05, 0x1087, 0x1003}, //[12]: CH status swap[7:0], [7]: disable audio FIFO, [2]:disable CTS Gen, [1]: automatically block start, [0]: audio FIFO flush
};

MSTHDMITX_REG_TYPE HDMITxAudioInitTbl[] =
{
    {HDMITX_AUDIO_REG_BASE, REG_AE_CONFIG_05, 0x1087, 0x1003}, //[12]: CH status swap[7:0], [7]: disable audio FIFO, [2]:disable CTS Gen, [1]: automatically block start, [0]: audio FIFO flush
};

MDrv_HDMITx_AUDIO_FREQ_TYPE TxAudioFreqTbl[E_HDMITX_AUDIO_FREQ_MAX_NUM] =
{
    {0x02, 0x001800},  // No signal, set to 48 KHz
    {0x03, 0x001000},  // 0 ~(32)~ 38 KHz,  4096
    {0x00, 0x001880},  // 38 ~(44.1)~ 46 KHz, 6272
    {0x02, 0x001800},  // 46 ~(48)~ 60 KHz, 6144
    {0x08, 0x003100}, //  60 ~(88.2)~ 92 KHz, 12544
    {0x0a, 0x003000}, //  92 ~(96)~ 140 KHz, 12288
    {0x0c, 0x006200}, //  140 ~(176.4)~ 180 KHz, 25088
    {0x0e, 0x006000},  //  180 ~(192)~ ~~ KHz, 24576
};

//*********************//
//             Packet  //
//*********************//

MS_U8 HDMITX_AviCmrTbl[E_HDMITX_RES_MAX] =
{
    0x48, 0x48, 0x48, 0x48, 0x48, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8            // SDTV C=01(601),M=00(no data) ,R=1000(same)
};

MS_U8 HDMITX_AviVicTbl[E_HDMITX_RES_MAX] =
{
    1, 6, 21, 2, 17, 19, 4, 20, 5, 32, 33, 34, 31, 16           // SDTV 480i60,576i50,480p60,576p50,720p50,720p60,1080i50,1080i60,1080p24,1080p25,1080p30, 1080p50, 1080p60
};

MS_U8 HDMITX_VendorName[8] =
{
    "MStar   "
};

MS_U8 HDMITX_ProductName[16] =
{
    "HDMI Tx Demo",
};

MS_U8 _gHPDGpioPin = 0;

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

#ifdef MS_DEBUG
#define DBG_HDMITX(_f)                  (_f)
#else
#define DBG_HDMITX(_f)
#endif

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

// HPD: GPIO_PM[11] -> external interrupt[11], register 0x000E00[14]
// DVI disconnet: must power down clock termination resistor: TM_REG[0] = 1, TM_REG[16:15] = 00, TM_REG[35:34] = 00.
// Interrupt helper functoins
//------------------------------------------------------------------------------
/// @brief Disable interrupt
/// @param[in] u32Int interrupter value
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_Int_Disable(MS_U32 u32Int)
{
    //MS_U16 u16reg_val = 0;

    MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_0C, (MS_U16)u32Int, (MS_U16)u32Int);
    // [9]: mask FIQ, [8]: mask IRQ
    MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_STATUS_0D, (MS_U16)(u32Int>>16), (MS_U16)(u32Int>>16) );

#if 0 // K1 HPD pin doesn't have interrupt function
    if(_gHPDGpioPin != 0xC4) // HPD != I2S_GPIO4
    {
        u16reg_val = (_gHPDGpioPin > 10) ? (1<<(_gHPDGpioPin+4)) : (1<<_gHPDGpioPin);
        if(u32Int & E_HDMITX_IRQ_12) // HPD IRQ is move to PM_Sleep bank
        {
            MHal_HDMITxPM_Mask_Write(PMBK_PMSLEEP_REG_BASE, 0x00, u16reg_val, u16reg_val); // GPIO_PM mask
        }
    }
#endif
}

//------------------------------------------------------------------------------
/// @brief Enable interrupt
/// @param[in] u32Int interrupter value
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_Int_Enable(MS_U32 u32Int)
{
    //MS_U16 u16reg_val = 0;

    MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_0C, 0xFFFF, ~u32Int);
    // [9]: mask FIQ, [8]: unmask IRQ
    MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_STATUS_0D, 0xFFFF, (~u32Int)>>16 );

#if 0 // K1 HPD pin doesn't have interrupt function
    if(_gHPDGpioPin != 0xC4) // HPD != I2S_GPIO4
    {
        u16reg_val = (_gHPDGpioPin > 10) ? (1<<(_gHPDGpioPin+4)) : (1<<_gHPDGpioPin);
        if(u32Int & E_HDMITX_IRQ_12)
        {
            MHal_HDMITxPM_Mask_Write(PMBK_PMSLEEP_REG_BASE, 0x0F, 1<<_gHPDGpioPin, 1<<_gHPDGpioPin); // GPIO_PM output disable
            MHal_HDMITxPM_Mask_Write(PMBK_PMSLEEP_REG_BASE, 0x00, u16reg_val, 0); // GPIO_PM non-mask
        }
    }
#endif
}

//------------------------------------------------------------------------------
/// @brief MHal_HDMITx_Int_Clear
/// @param[in] u32Int interrupter value
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_Int_Clear(MS_U32 u32Int)
{
    //MS_U16 u16reg_val = 0;

    MHal_HDMITx_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_STATUS_0E, u32Int);
    MHal_HDMITx_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_STATUS_0F, (u32Int>>16));

#if 0 // K1 HPD pin doesn't have interrupt function
    if(_gHPDGpioPin != 0xC4) // HPD != I2S_GPIO4
    {
        u16reg_val = (_gHPDGpioPin > 10) ? (1<<(_gHPDGpioPin+4)) : (1<<_gHPDGpioPin);
        if(u32Int & E_HDMITX_IRQ_12)
        {
            MHal_HDMITxPM_Mask_Write(PMBK_PMSLEEP_REG_BASE, 0x04, u16reg_val, u16reg_val);
            MHal_HDMITxPM_Mask_Write(PMBK_PMSLEEP_REG_BASE, 0x04, u16reg_val, 0);
        }
    }
#endif
}

//------------------------------------------------------------------------------
/// @brief MHal_HDMITx_Int_Status
/// @param[in] u32Int interrupter value
/// @return None
//------------------------------------------------------------------------------
MS_U32 MHal_HDMITx_Int_Status(void)
{
    //MS_U16 u16reg_val = 0;
    MS_U32 reg_value=0;

    reg_value |= MHal_HDMITx_Read(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_STATUS_0E);
    reg_value |= (MHal_HDMITx_Read(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_STATUS_0F)<<16);

#if 0 // K1 HPD pin doesn't have interrupt function
    if(_gHPDGpioPin != 0xC4) // HPD = I2S_GPIO4
    {
        u16reg_val = (_gHPDGpioPin > 10) ? (1<<(_gHPDGpioPin+4)) : (1<<_gHPDGpioPin);
        reg_value = ((MHal_HDMITxPM_Read(PMBK_PMSLEEP_REG_BASE, 0x0A) & u16reg_val) ? (reg_value|E_HDMITX_IRQ_12):(reg_value&(~E_HDMITX_IRQ_12)));
    }
#endif
    return reg_value;
}


//------------------------------------------------------------------------------
/// @brief This routine is to get HDMI receiver DVI clock and HPD status.
/// @return MsHDMITX_RX_STATUS
//------------------------------------------------------------------------------
MsHDMITX_RX_STATUS MHal_HDMITx_GetRXStatus(void)
{
    MS_BOOL dviclock_s, hpd_s = FALSE;
    MsHDMITX_RX_STATUS state;

    dviclock_s = MHal_HDMITx_Read(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_STATUS_0A) & BIT5 ? FALSE : TRUE;

#if 0 //ToDo Ryan
    if(_gHPDGpioPin == 108) // HPD = GPIO108, PAD_HSYNC_OUT
    {
        //MHal_HDMITx_Mask_Write(CHIPTOP_REG_BASE, REG_SYNC_GPIO0, BIT5, BIT5); // configure GPIO as input
        hpd_s = MHal_HDMITx_Read(CHIPTOP_REG_BASE, REG_SYNC_GPIO0) & BIT0 ? TRUE : FALSE;
    }
    else
    {
        //configure GPIO as GPIO index + 1
        hpd_s = mdrv_gpio_get_level(_gHPDGpioPin);
    }
#else
    if(MHal_HDMITxPM_Read(PM_GPIO_REG_BASE, _gHPDGpioPin) & 0x0010)
    {
        hpd_s = TRUE;
    }
    else
    {
        hpd_s = FALSE;
    }
#endif

    if((dviclock_s == FALSE) && (hpd_s == FALSE))
        state = E_HDMITX_DVIClock_L_HPD_L;
    else if((dviclock_s == FALSE) && (hpd_s == TRUE))
        state = E_HDMITX_DVIClock_L_HPD_H;
    else if((dviclock_s == TRUE) && (hpd_s == FALSE))
        state = E_HDMITX_DVIClock_H_HPD_L;
    else
        state = E_HDMITX_DVIClock_H_HPD_H;

    return state;
}


//------------------------------------------------------------------------------
/// @brief MHal_HDMITX_SetHDCPConfig
/// @param[in] u32Int HDCP mode
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITX_SetHDCPConfig(MS_U8 HDCP_mode)
{
    MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_HDCP_TX_MODE_61, 0x0E00, HDCP_mode<<8);
}


//------------------------------------------------------------------------------
/// @brief MHal_HDMITX_GetM02Bytes
/// @param[in] u16Int index
/// @return M0 2 bytes
//------------------------------------------------------------------------------
MS_U16 MHal_HDMITX_GetM02Bytes(MS_U16 idx)
{
    return(MHal_HDMITx_Read(HDMITX_MISC_HDCP_REG_BASE, REG_HDCP_TX_MI_6C+idx));
}


//------------------------------------------------------------------------------
/// @brief MHal_HDMITx_InitSeq
/// @param[in] None
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_InitSeq(void)
{
    MHal_HDMITx_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2A, 0x000F); // [3:0]: power down TMDS TX

    //MHal_HDMITx_Write(CLKGEN1_REG_BASE, REG_CKG_HDMITx_CLK_28, 0); // enable clk_hdmi_tx_p
    MHal_HDMITx_Mask_Write(CLKGEN0_REG_BASE, REG_CKG_HDMIT_Tx_CLK_34, 0x000C, 0x0000); // enable HDMITx CLK
    MHal_HDMITx_Mask_Write(CLKGEN0_REG_BASE, REG_CKG_HDMI_PLL_CLK_5F, 0x1C00, 0x0000); // enable HDMI PLL
    MHal_HDMITx_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_1E, 0xFFFF); // clkgen enable
    MHal_HDMITx_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_45, 0x0000); // power on Synth PLL
    MHal_HDMITx_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_33, 0x0000); // power on Tx PLL
    MHal_HDMITx_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2B, 0x0003); // TM_REG[0] = 1, power down clock termination, TM_REG[1], power down data termination
    MHal_HDMITx_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2C, 0x0000); // power on TM ATOP on TM_reg

    // Software reset tmds_atop
    MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_00, BIT7, BIT7);
    MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_00, BIT7, 0);

    MHal_HDMITx_Write(HDMITX_REG_BASE, REG_HPLL_LOCK_CNT_53, 0x0300); // HPLL lock counter
    MHal_HDMITx_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_58, 0x0003); // [1]: enable TM 4-ch new FIFO mode , [0]: select new mode for TM 4-ch FIFO
    MHal_HDMITx_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_41, 0x1010); // [12]: TEST_CLK_EN for synth pll, [4]: ENLCKDCT for synth pll
    MHal_HDMITx_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_34, 0x1010); // [12]: TEST_CLK_EN for tpll, [4]: ENLCKDCT for tpll
}

//------------------------------------------------------------------------------
/// @brief This routine is the initialization for Video module.
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_VideoInit(void)
{
    // DVI PLL exit power down: [8]: PD_BG, [7]: DVIPLL PD, [2]: Enable 2X pixel clock
    MHal_HDMITx_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_33, 0x0000);

    //[3:0]: disable TMDS DINEN async
    MHal_HDMITx_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2E, 0x0000);

    // VE FIFO enable
    MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_01, BIT4, BIT4);

    //MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_12, 0x000F, BIT3); // [3]: manual mode of pixel-repetition enable
}

//------------------------------------------------------------------------------
/// @brief This routine is the initialization for Audio module.
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_AudioInit(void)
{
    int num;

    num = sizeof(HDMITxAudioInitTbl) / sizeof(MSTHDMITX_REG_TYPE);
    MHal_HDMITx_RegsTbl_Write(HDMITxAudioInitTbl, num);
}


//------------------------------------------------------------------------------
/// @brief This routine turn on/off HDMI PLL
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_PLLOnOff(MS_BOOL bflag)
{
    if (bflag)
    {
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_33, BIT7, 0);
    }
    else
    {
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_33, BIT7, BIT7);
    }
}


void MHal_HDMITx_PKT_User_Define_Clear(void)
{
     NULL_PACKET.User_Define = FALSE;
     NULL_PACKET.Define_Process = E_HDMITX_STOP_PACKET;
     NULL_PACKET.Define_FCnt = 0;

	 ACR_PACKET = NULL_PACKET;
	 AS_PACKET = NULL_PACKET;
	 GC_PACKET = NULL_PACKET;
	 ACP_PACKET = NULL_PACKET;
	 ISRC1_PACKET = NULL_PACKET;
	 ISRC2_PACKET = NULL_PACKET;
	 DSD_PACKET = NULL_PACKET;
	 HBR_PACKET = NULL_PACKET;
	 GM_PACKET = NULL_PACKET;

	 VS_INFORAME = NULL_PACKET;
	 AVI_INFORAME = NULL_PACKET;
	 SPD_INFORAME = NULL_PACKET;
	 AUDIO_INFORAME = NULL_PACKET;
	 MPEG_INFORAME = NULL_PACKET;

}

void MHal_HDMITx_PKT_User_Define(MsHDMITX_PACKET_TYPE packet_type, MS_BOOL def_flag,
        MsHDMITX_PACKET_PROCESS def_process, MS_U8 def_fcnt)
{
    switch(packet_type)
	{
        case E_HDMITX_NULL_PACKET:
			NULL_PACKET.User_Define = def_flag;
			NULL_PACKET.Define_Process = def_process;
			NULL_PACKET.Define_FCnt = def_fcnt;
		break;
        case E_HDMITX_ACR_PACKET:
			ACR_PACKET.User_Define = def_flag;
			ACR_PACKET.Define_Process = def_process;
			ACR_PACKET.Define_FCnt = def_fcnt;
		break;
	    case E_HDMITX_AS_PACKET:
		    AS_PACKET.User_Define = def_flag;
		    AS_PACKET.Define_Process = def_process;
		    AS_PACKET.Define_FCnt = def_fcnt;
		break;
	    case E_HDMITX_GC_PACKET:
		    GC_PACKET.User_Define = def_flag;
		    GC_PACKET.Define_Process = def_process;
		    GC_PACKET.Define_FCnt = def_fcnt;
		break;
	    case E_HDMITX_ACP_PACKET:
	 	    ACP_PACKET.User_Define = def_flag;
		    ACP_PACKET.Define_Process = def_process;
		    ACP_PACKET.Define_FCnt = def_fcnt;
		break;
	    case E_HDMITX_ISRC1_PACKET:
		    ISRC1_PACKET.User_Define = def_flag;
		    ISRC1_PACKET.Define_Process = def_process;
		    ISRC1_PACKET.Define_FCnt = def_fcnt;
		break;
	    case E_HDMITX_ISRC2_PACKET:
		    ISRC2_PACKET.User_Define = def_flag;
		    ISRC2_PACKET.Define_Process = def_process;
		    ISRC2_PACKET.Define_FCnt = def_fcnt;
		break;
	    case E_HDMITX_DSD_PACKET:
		    DSD_PACKET.User_Define = def_flag;
		    DSD_PACKET.Define_Process = def_process;
		    DSD_PACKET.Define_FCnt = def_fcnt;
		break;
	    case E_HDMITX_HBR_PACKET:
		    HBR_PACKET.User_Define = def_flag;
		    HBR_PACKET.Define_Process = def_process;
		    HBR_PACKET.Define_FCnt = def_fcnt;
		break;
	    case E_HDMITX_GM_PACKET:
	 	    GM_PACKET.User_Define = def_flag;
		    GM_PACKET.Define_Process = def_process;
		    GM_PACKET.Define_FCnt = def_fcnt;
		break;
	    case E_HDMITX_VS_INFOFRAME:
		    VS_INFORAME.User_Define = def_flag;
		    VS_INFORAME.Define_Process = def_process;
		    VS_INFORAME.Define_FCnt = def_fcnt;
		break;
	    case E_HDMITX_AVI_INFOFRAME:
		    AVI_INFORAME.User_Define = def_flag;
		    AVI_INFORAME.Define_Process = def_process;
		    AVI_INFORAME.Define_FCnt = def_fcnt;
		break;
	    case E_HDMITX_SPD_INFOFRAME:
		    SPD_INFORAME.User_Define = def_flag;
		    SPD_INFORAME.Define_Process = def_process;
		    SPD_INFORAME.Define_FCnt = def_fcnt;
		break;
	    case E_HDMITX_AUDIO_INFOFRAME:
		    AUDIO_INFORAME.User_Define = def_flag;
		    AUDIO_INFORAME.Define_Process = def_process;
		    AUDIO_INFORAME.Define_FCnt = def_fcnt;
		break;
	    case E_HDMITX_MPEG_INFOFRAME:
		    MPEG_INFORAME.User_Define = def_flag;
		    MPEG_INFORAME.Define_Process = def_process;
		    MPEG_INFORAME.Define_FCnt = def_fcnt;
		break;
	    default:
		break;
    }
}

MS_BOOL MHal_HDMITx_PKT_Content_Define(MsHDMITX_PACKET_TYPE packet_type, MS_U8* data, MS_U8 length)
{
    MS_BOOL bRet = TRUE;
    MS_U8 i, j, *ptr;
	ptr = data;

    switch(packet_type)
	{
	    case E_HDMITX_VS_INFOFRAME:
            for(i=0;i<length;i++)
            {
                j = i>>1;

                if((REG_PKT_VS_1_27+j > REG_PKT_VS_14_34) || ((REG_PKT_VS_1_27+j == REG_PKT_VS_14_34) && (i % 2 == 1)))
                {
                    //Packet over size, last VS packet PB register is REG_PKT_VS_14_34[7:0]
                    DBG_HDMITX(printf("VS packet over size, length = %d \n", length));
                    break;
                }

                if((i%2)==0)
			        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_VS_1_27+j, 0x00FF, *(ptr+i));
				else
			        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_VS_1_27+j, 0xFF00, (*(ptr+i))<<8);
            }
		break;
	    case E_HDMITX_SPD_INFOFRAME:
            for(i=0;i<length;i++)
            {
                j = i>>1;

                if((REG_PKT_SPD_1_15+j > REG_PKT_SPD_13_21) || ((REG_PKT_SPD_1_15+j == REG_PKT_SPD_13_21) && (i % 2 == 1)))
                {
                    //Packet over size, last SPD packet PB register is REG_PKT_SPD_13_21[7:0]
                    DBG_HDMITX(printf("VS packet over size, length = %d \n", length));
                    break;
                }

                if((i%2)==0)
			        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_SPD_1_15+j, 0x00FF, *(ptr+i));
				else
			        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_SPD_1_15+j, 0xFF00, (*(ptr+i))<<8);
            }
		break;
	    default:
		    i = 0;
			j = 0;
            bRet = FALSE;
            DBG_HDMITX(printf("Not implemented, packet type = %u\n", packet_type));
	    break;
    }

    return bRet;
}

//------------------------------------------------------------------------------
/// @brief This routine turn on/off HDMI Tx TMDS signal
/// @param[in] bRB_Swap: R/B swap; bTMDS: TMDS flag
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_SetTMDSOnOff(MS_BOOL bRB_Swap, MS_BOOL bTMDS)
{
    if(bRB_Swap) // R/B channel swap
        MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_00, BIT10, BIT10);
    else
        MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_00, BIT10, 0);

    if(bTMDS) // power on TMDS channel and clock
    {
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2A, 0x000F, 0); // [3:0]: TDMS power on
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2E, 0x000F, 0x000F); // enable TMDS DINEN async
    }
    else // power down TMDS channel and clock
    {
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2E, 0x000F, 0x0000); // disable TMDS DINEN async
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2A, 0x000F, 0x000F); // [3:0]: TDMS power down
    }
}


//------------------------------------------------------------------------------
/// @brief This routine turn on/off HDMI Tx video output
/// @param[in] bVideo: Video flag; bCSC: CSC flag, b709format = BT.709-5
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_SetVideoOnOff(MS_BOOL bVideo, MS_BOOL bCSC, MS_BOOL b709format)
{
    if(bVideo)
    {
        if(bCSC) // YUV -> RGB
            MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_01, BIT8, BIT8);
        else // bypass
            MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_01, BIT8, 0);
        // disable test pattern
        MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_01, BIT5, 0x0000);
    }
    else
    {
        if(bCSC) // YUV -> RGB
            MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_01, BIT8, BIT8);
        else // bypass
            MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_01, BIT8, 0);
        DBG_HDMITX(printf("MDrv_HDMITx_SetVideoOnOff: csc flag= %d \n", bCSC));
        // enable test pattern
        MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_11, 0x003F, 0x0003); // whole-blue
        MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_01, BIT5, BIT5);
    }
}

//------------------------------------------------------------------------------
/// @brief This routine sets video color formatt
/// @param[in] bCSC: CSC flag, YUV422 12 bit, b709format = BT.709-5
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_SetColorFormat(MS_BOOL bCSC, MS_BOOL bHdmi422b12, MS_BOOL b709format)
{
    if(bCSC) // YUV -> RGB
        MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_01, BIT8, BIT8);
    else // bypass
        MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_01, BIT8, 0);

    // YUV422 12 bits output
    MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_18, BIT0, bHdmi422b12);
}


//------------------------------------------------------------------------------
/// @brief This routine will set or stop all HDMI packet generation
/// @param[in] bflag  True: Enable packet gen, False : Disable packet gen 
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_EnablePacketGen(MS_BOOL bflag)
{
    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_HDMI_CONFIG1_00, BIT2, bflag?BIT2:0);
}

//------------------------------------------------------------------------------
/// @brief This routine sets HDMI/DVI mode
/// @param[in] bflag
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_SetHDMImode(MS_BOOL bflag, MsHDMITX_VIDEO_COLORDEPTH_VAL cd_val)
{
    MS_U8 reg_val1;//, reg_val2;

    DBG_HDMITX(printf("[%s][%d]HDMI mode = %d, Color Depth = %d \n", __FUNCTION__, __LINE__, bflag, bflag));

    if(bflag) // HDMI mode
    {
        switch(cd_val)
        {
            case E_HDMITX_VIDEO_CD_NoID:
            case E_HDMITX_VIDEO_CD_24Bits:
            default:
                reg_val1 = 0x00;
                //reg_val2 = 0;
                break;
            case E_HDMITX_VIDEO_CD_30Bits:
                reg_val1 = 0x40;
                //reg_val2 = BIT0;
                break;
            case E_HDMITX_VIDEO_CD_36Bits:
                reg_val1 = 0x80;
                //reg_val2 = BIT1;
                break;
        }
        //MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_17, 0x0007, reg_val2); // [2:0]: DVI output data bus
        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_HDMI_CONFIG1_00, 0x00FF, reg_val1|BIT2); // [7:6]: DC_mode, [2]: packet enable, [0]: HDMI/DVI
    }
    else // DVI
    {
        //MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_17, 0x0007, 0); // [2:0]: DVI output data bus
        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_HDMI_CONFIG1_00, 0x00FF, BIT0); // [7:6]: DC_mode, [2]: packet enable, [0]: HDMI/DVI
    }
}


//------------------------------------------------------------------------------
/// @brief This routine sets audio on/off
/// @param[in] bflag
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_SetAudioOnOff(MS_BOOL bflag)
{
    MS_U8 num;

    if(bflag) // audio on
    {
        num = sizeof(HDMITxAudioOnTbl)/sizeof(MSTHDMITX_REG_TYPE);
        MHal_HDMITx_RegsTbl_Write(HDMITxAudioOnTbl, num);
    }
    else // audio off
    {
        num = sizeof(HDMITxAudioOffTbl)/sizeof(MSTHDMITX_REG_TYPE);
        MHal_HDMITx_RegsTbl_Write(HDMITxAudioOffTbl, num);
    }
}

//------------------------------------------------------------------------------
/// @brief This routine sets audio sampling freq.
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_SetAudioFrequency(MsHDMITX_AUDIO_FREQUENCY afidx,
        MsHDMITX_AUDIO_CHANNEL_COUNT achidx, MsHDMITX_AUDIO_CODING_TYPE actidx
)
{
    // HDMI audio channel setting
    if(achidx == E_HDMITX_AUDIO_CH_2) // 2 channels
    {
        MHal_HDMITx_Mask_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CONFIG_05, BIT4|BIT5|BIT6, BIT5|BIT6); //[6:5]: audio FIFO depth ch1234, [4]=1'b0: 2 channels
    }
    else // 8 channels
    {
        MHal_HDMITx_Mask_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CONFIG_05, BIT4, BIT4); //[4]=1'b1: 8 channels
    }
    // Audio channel status
    MHal_HDMITx_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CH_STATUS0_00, ((actidx == E_HDMITX_AUDIO_PCM) ? 0 : BIT1) ); // [1]: PCM / non-PCM
    MHal_HDMITx_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CH_STATUS1_01, 0);
    MHal_HDMITx_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CH_STATUS2_02, (achidx<<4)); // [7:4]: audio channel count
    MHal_HDMITx_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CH_STATUS3_03, TxAudioFreqTbl[afidx].CH_Status3); // [3:0]: audio sampling frequncy
    MHal_HDMITx_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CH_STATUS4_04, 0);
    // ACR N code
    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_ACR_2_06, 0x0F00, (TxAudioFreqTbl[afidx].NcodeValue & 0xFF0000) >> 8);
    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_ACR_3_07, 0xFFFF, (TxAudioFreqTbl[afidx].NcodeValue & 0x00FFFF));
}

//------------------------------------------------------------------------------
/// @brief This routine sets audio source format.
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_SetAudioSourceFormat(MsHDMITX_AUDIO_SOURCE_FORMAT fmt)
{
    MHal_HDMITx_Mask_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CONFIG_05, BIT9|BIT8, fmt << 8);
}

//------------------------------------------------------------------------------
/// @brief This routine Get Audio CTS value
/// @return CTS
//------------------------------------------------------------------------------
MS_U32 MHal_HDMITx_GetAudioCTS(void)
{
    MS_U32 ret;

    ret = ((MHal_HDMITx_Read(HDMITX_REG_BASE, REG_PKT_ACR_2_06) & 0x000F) << 16) | MHal_HDMITx_Read(HDMITX_REG_BASE, REG_PKT_ACR_1_05);
    return ret;
}

//------------------------------------------------------------------------------
/// @brief This routine Mute Audio FIFO
/// @param[in] bflag: True: mute audio, False: unmute audio
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_MuteAudioFIFO(MS_BOOL bflag)
{
    MHal_HDMITx_Mask_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CONFIG_05, 0x0001, (MS_U16)bflag);
}

//------------------------------------------------------------------------------
/// @brief This routine sets HDMI Tx HDCP encryption On/Off
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_SetHDCPOnOff(MS_BOOL hdcp_flag, MS_BOOL hdmi_flag)
{
    if(hdcp_flag) // HDCP on
    {
        if(hdmi_flag) // HDMI EESS
            MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_HDCP_TX_MODE_61, 0x0C00, 0x0400);
        else // DVI OESS
            MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_HDCP_TX_MODE_61, 0x0C00, 0x0000);
        // HDCP encryption
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_HDCP_TX_COMMAND_62, 0x0008, 0x0008);
    }
    else // HDCP off
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_HDCP_TX_COMMAND_62, 0x0008, 0x0000);
}

//------------------------------------------------------------------------------
/// @brief This routine calculate check sum of infoframes.
/// @param[in] packet_type packet type
/// @return checksum
//------------------------------------------------------------------------------
MS_U8 MHal_HDMITx_InfoFrameCheckSum(MsHDMITX_PACKET_TYPE packet_type)
{
    MS_U8 i, j, pkt_checksum, ver, tmp_value;
    MS_U16 reg_value;

    pkt_checksum = 0;//add for compiler warning

    switch (packet_type)
    {
        case E_HDMITX_VS_INFOFRAME:
            pkt_checksum = (MS_U8)(0 - E_HDMITX_VS_INFOFRAME - 0x01 - 0x1B);
            for (i = 0; i < 14; i++)
            {
                reg_value = MHal_HDMITx_Read(HDMITX_REG_BASE, REG_PKT_VS_1_27+i);;
                for(j=0; j<2; j++)
                {
                    tmp_value = (MS_U8)( j==0 ? (reg_value & 0x00FF) : ((reg_value & 0xFF00)>>8) );
                    if ( (i == 13) && (j == 1) ) // reserved
                        pkt_checksum -= 0x00;
                    else
                        pkt_checksum -= tmp_value;
                }
            }
            //MHal_HDMITx_Mask_Write(REG_HDMITX_BANK1, REG_PKT_VS_CFG_35, 0xFF00, (MS_U16)checksum<<8);
            break;

        case E_HDMITX_AVI_INFOFRAME:
            ver = (MHal_HDMITx_Read(HDMITX_REG_BASE, REG_PKT_AVI_1_09) & 0x0080) ? 2 : 1;
            pkt_checksum = 0 - E_HDMITX_AVI_INFOFRAME - ver - 0x0D;
            for (i = 0; i < 7; i++)
            {
                reg_value = MHal_HDMITx_Read(HDMITX_REG_BASE, REG_PKT_AVI_1_09+i);
                for(j=0; j<2; j++)
                {
                    tmp_value = (MS_U8)( j==0 ? (reg_value & 0x00FF) : ((reg_value & 0xFF00)>>8) );
                    if ( (i == 0) && (j == 0) )
                    {
                        // Aaron - Uranus U01 bug fixed : check sum error if version is 2
                        //pkt_checksum = pkt_checksum - (tmp_value & 0x7F) - ( (ver==2) ? 0x80 : 0x00 );
                        //U03
                        pkt_checksum -= (tmp_value & 0x7F);
                    }
                    else if ( (i == 1) && (j == 0) ) // SC[1:0]
                        pkt_checksum -= ((ver == 2) ? tmp_value : (tmp_value & 0x03));
                    else if ( (i == 1) && (j == 1) ) // VIC[6:0]
                        pkt_checksum -= ((ver == 2) ? (tmp_value & 0x7F) : 0x00);
                    else if ( (i == 2) && (j == 0) ) // PR[3:0]
                        pkt_checksum -= (tmp_value & 0x0F);
                    else if ( (i == 2) && (j == 1) ) // reserved
                        pkt_checksum -= 0x00;
                    else
                        pkt_checksum -= tmp_value;
                }
            }
            //MHal_HDMITx_Mask_Write(REG_HDMITX_BANK1, REG_PKT_AVI_CFG_10, 0xFF00, (MS_U16)checksum<<8);
            break;

        case E_HDMITX_SPD_INFOFRAME:
            pkt_checksum = (MS_U8)(0 - E_HDMITX_SPD_INFOFRAME - 0x01 - 0x19);
            for (i = 0; i < 13; i++)
            {
                reg_value = MHal_HDMITx_Read(HDMITX_REG_BASE, REG_PKT_SPD_1_15+i);
                for(j=0; j<2; j++)
                {
                    tmp_value = (MS_U8)( j==0 ? (reg_value & 0x00FF) : ((reg_value & 0xFF00)>>8) );
                    if ( (i == 12) && (j == 0) )
                        pkt_checksum -= tmp_value;
                    else if ( (i == 12) && (j == 1) ) // reserved
                        pkt_checksum -= 0x00;
                    else
                        pkt_checksum -= (tmp_value & 0x7F);
                }
            }
            //MHal_HDMITx_Mask_Write(REG_HDMITX_BANK1, REG_PKT_SPD_CFG_22, 0xFF00, (MS_U16)checksum<<8);
            break;

        case E_HDMITX_AUDIO_INFOFRAME:
            pkt_checksum = (MS_U8)(0 - E_HDMITX_AUDIO_INFOFRAME - 0x01 - 0x0A);
            for (i = 0; i < 3; i++)
            {
                reg_value = MHal_HDMITx_Read(HDMITX_REG_BASE, REG_PKT_AUD_1_11+i);
                for(j=0; j<2; j++)
                {
                    tmp_value = (MS_U8)( j==0 ? (reg_value & 0x00FF) : ((reg_value & 0xFF00)>>8) );
                    if ( (i == 0) && (j == 0) )
                        pkt_checksum -= (tmp_value & 0xF7);
                    else if ( (i == 0) && (j == 1) )
                        pkt_checksum -= (tmp_value & 0x1F);
                    else if ( (i == 2) && (j == 0) )
                        pkt_checksum -= (tmp_value & 0xFB);
                    else if ( (i == 2) && (j == 1) ) // reserved
                        pkt_checksum -= 0x00;
                    else
                        pkt_checksum -= tmp_value;
                }
            }
            //MHal_HDMITx_Mask_Write(REG_HDMITX_BANK1, REG_PKT_AUD_CFG_14, 0xFF00, (MS_U16)checksum<<8);
            break;

        case E_HDMITX_MPEG_INFOFRAME: // unsupport in Uranus
            break;

        default:
            break;
    }
    return pkt_checksum;
}


//------------------------------------------------------------------------------
/// @brief This routine sends HDMI related packets.
/// @param[in] packet_type packet type
/// @param[in] packet_process packet behavior
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_SendPacket(MsHDMITX_PACKET_TYPE packet_type, MS_U8 packet_process,
        MsHDMITX_VIDEO_COLORDEPTH_VAL vcdidx, MsHDMITX_VIDEO_TIMING vtidx, MsHDMITX_VIDEO_COLOR_FORMAT vcidx,
        MsHDMITX_VIDEO_ASPECT_RATIO varidx, MS_BOOL bafd_override, MsHDMITX_VIDEO_SCAN_INFO vsidx, MsHDMITX_VIDEO_AFD_RATIO vafdidx, MS_U8 a0,
        MsHDMITX_AUDIO_FREQUENCY afidx, MsHDMITX_AUDIO_CHANNEL_COUNT achidx, MsHDMITX_AUDIO_CODING_TYPE actidx, MsHDMITX_VIDEO_VS_FORMAT vsfmt,
        MsHDMITX_VIDEO_3D_STRUCTURE vs3d, MsHDMITX_VIDEO_4k2k_VIC vsvic
)
{
    MS_U16 i, tmp_value=0;

    switch (packet_type)
    {
        case E_HDMITX_NULL_PACKET:
			if(NULL_PACKET.User_Define)
			{
                if (IS_STOP_PKT(NULL_PACKET.Define_Process))   // Stop sending packet
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_NUL_CFG_02, 0x0005, 0x0000);
                else // Enable sending packet
                {
                    // cyclic packet
                    if (IS_CYCLIC_PKT(NULL_PACKET.Define_Process))
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_NUL_CFG_02, 0x00FF, ((NULL_PACKET.Define_FCnt<<3) |0x0005) );
                    // single packet
                    else
                    {
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_NUL_CFG_02, 0x00FF, 0x0001);
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_NUL_CMD);
                    }
                }
			}
			else
			{
                if (IS_STOP_PKT(packet_process))   // Stop sending packet
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_NUL_CFG_02, 0x0005, 0x0000);
                else // Enable sending packet
                {
                    // cyclic packet
                    if (IS_CYCLIC_PKT(packet_process))
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_NUL_CFG_02, 0x00FF, ((HDMITX_PACKET_NULL_FCNT<<3) |0x0005) );
                    // single packet
                    else
                    {
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_NUL_CFG_02, 0x00FF, 0x0001);
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_NUL_CMD);
                    }
                }
			}
            break;

        case E_HDMITX_ACR_PACKET:
            if(ACR_PACKET.User_Define)
            {
                if (IS_STOP_PKT(ACR_PACKET.Define_Process))   // Stop sending packet
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_ACR_CFG_08, 0x000F, 0x0008);
                else                    // Enable sending packet
                {
                    //MHal_HDMITx_SetAudioFrequency();
                    // cyclic packet
                    if (IS_CYCLIC_PKT(ACR_PACKET.Define_Process))
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_ACR_CFG_08, 0x00FF, ((ACR_PACKET.Define_FCnt<<4) |0x05));
                    // single packet
                    else
                    {
						MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_ACR_CFG_08, 0x00FF, 0x0009);
						MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_ACR_CMD);
                    }
                }
            }
			else
            {
                if (IS_STOP_PKT(packet_process))   // Stop sending packet
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_ACR_CFG_08, 0x000F, 0x0008);
                else                    // Enable sending packet
                {
                    //MHal_HDMITx_SetAudioFrequency();
                    // cyclic packet
                    if (IS_CYCLIC_PKT(packet_process))
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_ACR_CFG_08, 0x00FF, ((HDMITX_PACKET_ACR_FCNT<<4) |0x05));
                    // single packet
                    else
                    {
                       MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_ACR_CFG_08, 0x00FF, 0x0009);
                       MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_ACR_CMD);
                    }
                }
            }
            break;

        case E_HDMITX_AS_PACKET:
			if(AS_PACKET.User_Define)
            {
                if (IS_STOP_PKT(AS_PACKET.Define_Process))   // Stop sending packet
                    MHal_HDMITx_Mask_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CONFIG_05, BIT7|BIT0, BIT0); // [7]: disable audio FIFO, [0]:audio FIFO flush
                else                    // Enable sending packet
                    MHal_HDMITx_Mask_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CONFIG_05, BIT7|BIT0, BIT7); // [7]: enable audio FIFO, [0]:audio FIFO not flush
			}
			else
            {
                if (IS_STOP_PKT(packet_process))   // Stop sending packet
                    MHal_HDMITx_Mask_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CONFIG_05, BIT7|BIT0, BIT0); // [7]: disable audio FIFO, [0]:audio FIFO flush
                else                    // Enable sending packet
                    MHal_HDMITx_Mask_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CONFIG_05, BIT7|BIT0, BIT7); // [7]: enable audio FIFO, [0]:audio FIFO not flush
            }
			break;

        case E_HDMITX_GC_PACKET:
            if(GC_PACKET.User_Define)
            {
                if (IS_STOP_PKT(GC_PACKET.Define_Process))   // Stop sending packet
                {
                    tmp_value = 0x00 | ((GC_PACKET.Define_Process & 0x03) << 1);
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_GC_CFG_03, 0x000F, tmp_value);
                }
                else
                {
                    // Deep color information
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_GC12_04, 0x010F, vcdidx); // [8]: default phase = 0, [3:0]: Color depth
                    tmp_value = (GC_PACKET.Define_Process & 0x03) << 1;
                    // cyclic packet
                    if (IS_CYCLIC_PKT(GC_PACKET.Define_Process))
                    {
                        tmp_value |= 0x29;
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_GC_CFG_03, 0x007F, ((GC_PACKET.Define_FCnt<<4) |tmp_value)); // [6]: 0, DC and non-DC info send together
                    }
                    // single packet
                    else
                    {
                        tmp_value |= 0x21;
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_GC_CFG_03, 0x007F, tmp_value); // [6]: 0, DC and non-DC info send together
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_GCP_CMD);
                    }
                }
            }
			else
			{
                if (IS_STOP_PKT(packet_process))   // Stop sending packet
                {
                    tmp_value = 0x00 | ((packet_process & 0x03) << 1);
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_GC_CFG_03, 0x000F, tmp_value);
                }
                else
                {
                    // Deep color information
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_GC12_04, 0x010F, vcdidx); // [8]: default phase = 0, [3:0]: Color depth
                    tmp_value = (packet_process & 0x03) << 1;
                    // cyclic packet
                    if (IS_CYCLIC_PKT(packet_process))
                    {
                        tmp_value |= 0x29;
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_GC_CFG_03, 0x007F, ((HDMITX_PACKET_GC_FCNT<<4) |tmp_value)); // [6]: 0, DC and non-DC info send together
                    }
                    // single packet
                    else
                    {
                        tmp_value |= 0x21;
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_GC_CFG_03, 0x007F, tmp_value); // [6]: 0, DC and non-DC info send together
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_GCP_CMD);
                    }
                }
            }
            break;

        case E_HDMITX_ACP_PACKET:
            if(ACP_PACKET.User_Define)
            {
                if (IS_STOP_PKT(ACP_PACKET.Define_Process))
                {
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_ACP_CFG_40, 0x0005, 0x0000);  // Stop ACP packet
                }
                else
                {
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_ACP_0_38, 0xFFFF, 0x0000);  // ACP type is 00
                    tmp_value = 0x00;  // set Generic Audio
                    // cyclic packet
                    if (IS_CYCLIC_PKT(ACP_PACKET.Define_Process))
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_ACP_CFG_40, ( (tmp_value<<8) | (ACP_PACKET.Define_FCnt<<3) | 0x0005));
                    // single packet
                    else
                    {
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_ACP_CFG_40, (tmp_value<<8) | 0x0001);
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_ACP_CMD);
                    }
                }
            }
            else
            {
                if (IS_STOP_PKT(packet_process))
                {
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_ACP_CFG_40, 0x0005, 0x0000);  // Stop ACP packet
                }
                else
                {
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_ACP_0_38, 0xFFFF, 0x0000);  // ACP type is 00
                    tmp_value = 0x00;  // set Generic Audio
                    // cyclic packet
                    if (IS_CYCLIC_PKT(packet_process))
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_ACP_CFG_40, ( (tmp_value<<8) | (HDMITX_PACKET_ACP_FCNT<<3) | 0x0005));
                    // single packet
                    else
                    {
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_ACP_CFG_40, (tmp_value<<8) | 0x0001);
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_ACP_CMD);
                    }
                }
            }
            break;

        case E_HDMITX_ISRC1_PACKET:
            if(ISRC1_PACKET.User_Define)
            {
                if (IS_STOP_PKT(ISRC1_PACKET.Define_Process))  // Stop sending packet
                {
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_ISRC_CFG_51, 0x0005, 0x0000);  // Stop ISRC1 packet
                }
                else
                {
                    tmp_value = 0x00;  // ISRC_CONT = 0
                    // cyclic packet
                    if (IS_CYCLIC_PKT(ISRC1_PACKET.Define_Process))
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_ISRC_CFG_51, ( (tmp_value<<8) | (ISRC1_PACKET.Define_FCnt<<3) | 0x0005) );  // Send ISRC1 packet
                    // single packet
                    else
                    {
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_ISRC_CFG_51, (tmp_value<<8) | 0x0001 );
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_ISRC_CMD);
                    }
                }
            }
            else
            {
                if (IS_STOP_PKT(packet_process))  // Stop sending packet
                {
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_ISRC_CFG_51, 0x0005, 0x0000);  // Stop ISRC1 packet
                }
                else
                {
                    tmp_value = 0x00;  // ISRC_CONT = 0
                    // cyclic packet
                    if (IS_CYCLIC_PKT(packet_process))
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_ISRC_CFG_51, ( (tmp_value<<8) | (HDMITX_PACKET_ISRC_FCNT<<3) | 0x0005) );  // Send ISRC1 packet
                    // single packet
                    else
                    {
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_ISRC_CFG_51, (tmp_value<<8) | 0x0001 );
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_ISRC_CMD);
                    }
                }
            }
            break;

        case E_HDMITX_ISRC2_PACKET:
            if(ISRC2_PACKET.User_Define)
            {
                if (MHal_HDMITx_Read(HDMITX_REG_BASE, REG_PKT_ISRC_CFG_51) & 0x8000)
                {
                    if (IS_STOP_PKT(ISRC2_PACKET.Define_Process))  // Stop sending packet
                    {
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_ISRC_CFG_51, 0x0005, 0x0000);  // Stop ISRC1 packet
                    }
                    else
                    {
                        tmp_value = 0x80;  // ISRC_CONT = 1, ISRC1 & ISRC2
                        // cyclic packet
                        if (IS_CYCLIC_PKT(ISRC2_PACKET.Define_Process))
                            MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_ISRC_CFG_51, ( (tmp_value<<8) | (ISRC2_PACKET.Define_FCnt<<3) | 0x0005) );  // Send ISRC1 & ISRC2 packet
                        // single packet
                        else
                        {
                            MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_ISRC_CFG_51, (tmp_value<<8) | 0x0001 );
                            MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_ISRC_CMD);
                        }
                    }
                }
            }
            else
            {
                if (MHal_HDMITx_Read(HDMITX_REG_BASE, REG_PKT_ISRC_CFG_51) & 0x8000)
                {
                    if (IS_STOP_PKT(packet_process))  // Stop sending packet
                    {
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_ISRC_CFG_51, 0x0005, 0x0000);  // Stop ISRC1 packet
                    }
                    else
                    {
                        tmp_value = 0x80;  // ISRC_CONT = 1, ISRC1 & ISRC2
                        // cyclic packet
                        if (IS_CYCLIC_PKT(packet_process))
                            MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_ISRC_CFG_51, ( (tmp_value<<8) | (HDMITX_PACKET_ISRC_FCNT<<3) | 0x0005) );  // Send ISRC1 & ISRC2 packet
                        // single packet
                        else
                        {
                            MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_ISRC_CFG_51, (tmp_value<<8) | 0x0001 );
                            MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_ISRC_CMD);
                        }
                    }
                }
            }
            break;

        case E_HDMITX_DSD_PACKET: // unsupport in Uranus
            break;

        case E_HDMITX_GM_PACKET: // TBD
            break;

        case E_HDMITX_VS_INFOFRAME:
            if(VS_INFORAME.User_Define)
            {
                if (IS_STOP_PKT(VS_INFORAME.Define_Process))  // Stop sending packet
                {
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_VS_CFG_35, 0x0005, 0x0000);  // Stop VS packet
                }
                else
                {
                    //MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_VS_1_27, 0xFFFF, 0x0C03);  // IEEE HDMI ID
                    tmp_value = MHal_HDMITx_InfoFrameCheckSum(packet_type);  // Checksum
                    // cyclic packet
                    if (IS_CYCLIC_PKT(VS_INFORAME.Define_Process))
                            MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_VS_CFG_35, ( (tmp_value<<8) | (VS_INFORAME.Define_FCnt<<3) | 0x0005));  // send VS packet
                    // single packet
                    else
                    {
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_VS_CFG_35, (tmp_value<<8) | 0x0001);
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_VSP_CMD);
                    }
                }
            }
            else
            {
                if (IS_STOP_PKT(packet_process))  // Stop sending packet
                {
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_VS_CFG_35, 0x0005, 0x0000);  // Stop VS packet
                }
                else
                {
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_VS_1_27, 0xFFFF, 0x0C03);  // IEEE HDMI ID

                    if(vsfmt == E_HDMITX_VIDEO_VS_3D)
                    {
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_VS_2_28, 0xE000, vsfmt<<13);  // video format
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_VS_3_29, 0x00FF, vs3d<<4);  // 3D structure
					}
                    else if(vsfmt == E_HDMITX_VIDEO_VS_4k_2k)
                    {
						MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_VS_2_28, 0xE000, vsfmt<<13);  // video format
						MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_VS_3_29, 0x00FF, vsvic);	// 4k2k vic
					}
					else
                    {
						MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_VS_2_28, 0xE000, 0);  // video format
						MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_VS_3_29, 0x00FF, 0);
					}

                    tmp_value = MHal_HDMITx_InfoFrameCheckSum(packet_type);  // Checksum
                    // cyclic packet
                    if (IS_CYCLIC_PKT(packet_process))
                            MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_VS_CFG_35, ( (tmp_value<<8) | (HDMITX_PACKET_VS_FCNT<<3) | 0x0005));  // send VS packet
                    // single packet
                    else
                    {
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_VS_CFG_35, (tmp_value<<8) | 0x0001);
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_VSP_CMD);
                    }
                }
            }
            break;

        case E_HDMITX_AVI_INFOFRAME:
            if(AVI_INFORAME.User_Define)
			{
                if (IS_STOP_PKT(AVI_INFORAME.Define_Process))   // Stop sending packet
                {
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_CFG_10, 0x0005, 0x0000);  // Stop AVI packet
                }
                else
                {
                    // Version 2, RGB444/YCbCr444/YCbCr422
                    tmp_value = 0x80|BIT4|(vcidx<< 5);

                    // C:Colorimetry (NONE/ITU601/ITU709)
                    // M:Picture Aspect Ratio (NONE/4:3/16:9)
                    if((vtidx >= E_HDMITX_RES_720x480i) && (vtidx <= E_HDMITX_RES_720x576p)) // only 480ip/576ip could have 4:3/16:9 option
                        tmp_value |= ((HDMITX_AviCmrTbl[vtidx] | (varidx<<4)) << 8);
                    else // HD timing is always 16:9
                        tmp_value |= (HDMITX_AviCmrTbl[vtidx] << 8);

                    if(bafd_override) // AFD override mode
                    {
                        tmp_value = (tmp_value & 0xF0EC) |(a0<<4) | vsidx | ((vafdidx&0x0F)<<8);
                    }

                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_1_09, 0xFFFF, tmp_value);

                    // VIC:Video Id Code
                    if(((vtidx >= E_HDMITX_RES_720x480i) && (vtidx <= E_HDMITX_RES_720x576p)) && varidx == E_HDMITX_VIDEO_AR_16_9) // VIC code should be +1 if aspect ratio is 16:9
                        tmp_value = ((HDMITX_AviVicTbl[vtidx]+1) << 8);
                    else
                        tmp_value = (HDMITX_AviVicTbl[vtidx] << 8);
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_2_0A, 0x7F00, tmp_value);

                    // Check repetition
                    if ((HDMITxVideoModeTbl[vtidx].i_p_mode == E_HDMITX_VIDEO_INTERLACE_MODE) && (vtidx<=E_HDMITX_RES_720x576i))
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_3_0B, 0x000F, 0x0001);
                    else
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_3_0B, 0x000F, 0x0000);

                    tmp_value = MHal_HDMITx_InfoFrameCheckSum(packet_type);  // Checksum
                    // cyclic packet
                    if (IS_CYCLIC_PKT(AVI_INFORAME.Define_Process))
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_AVI_CFG_10, ( (tmp_value<<8) | (AVI_INFORAME.Define_FCnt<<3) | 0x0005));  // send AVI packet
                    // single packet
                    else
                    {
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_AVI_CFG_10, (tmp_value<<8) | 0x0001);
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_AVI_CMD);
                    }
                }
            }
			else
			{
                if (IS_STOP_PKT(packet_process))   // Stop sending packet
                {
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_CFG_10, 0x0005, 0x0000);  // Stop AVI packet
                }
                else
                {
                    // Version 2, RGB444/YCbCr444/YCbCr422
                    tmp_value = 0x80|BIT4|(vcidx<< 5);

                    // C:Colorimetry (NONE/ITU601/ITU709)
                    // M:Picture Aspect Ratio (NONE/4:3/16:9)
                    if((vtidx >= E_HDMITX_RES_720x480i) && (vtidx <= E_HDMITX_RES_720x576p)) // only 480ip/576ip could have 4:3/16:9 option
                        tmp_value |= ((HDMITX_AviCmrTbl[vtidx] | (varidx<<4)) << 8);
                    else // HD timing is always 16:9
                        tmp_value |= (HDMITX_AviCmrTbl[vtidx] << 8);

                    if(bafd_override) // AFD override mode
                    {
                        tmp_value = (tmp_value & 0xF0EC) |(a0<<4) | vsidx | ((vafdidx&0x0F)<<8);
                    }

                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_1_09, 0xFFFF, tmp_value);

                    // VIC:Video Id Code
                    if(((vtidx >= E_HDMITX_RES_720x480i) && (vtidx <= E_HDMITX_RES_720x576p)) && varidx == E_HDMITX_VIDEO_AR_16_9) // VIC code should be +1 if aspect ratio is 16:9
                        tmp_value = ((HDMITX_AviVicTbl[vtidx]+1) << 8);
                    else
                        tmp_value = (HDMITX_AviVicTbl[vtidx] << 8);
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_2_0A, 0x7F00, tmp_value);

                    // Check repetition
                    if ((HDMITxVideoModeTbl[vtidx].i_p_mode == E_HDMITX_VIDEO_INTERLACE_MODE) && (vtidx<=E_HDMITX_RES_720x576i))
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_3_0B, 0x000F, 0x0001);
                    else
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_3_0B, 0x000F, 0x0000);

                    tmp_value = MHal_HDMITx_InfoFrameCheckSum(packet_type);  // Checksum
                    // cyclic packet
                    if (IS_CYCLIC_PKT(packet_process))
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_AVI_CFG_10, ( (tmp_value<<8) | (HDMITX_PACKET_AVI_FCNT<<3) | 0x0005));  // send AVI packet
                    // single packet
                    else
                    {
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_AVI_CFG_10, (tmp_value<<8) | 0x0001);
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_AVI_CMD);
                    }
                }
			}
            break;

        case E_HDMITX_SPD_INFOFRAME:
			if(SPD_INFORAME.User_Define)
			{
                if (IS_STOP_PKT(SPD_INFORAME.Define_Process))   // Stop sending packet
                {
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_SPD_CFG_22, 0x0005, 0x0000);  // Stop SPD packet
                }
                else
                {
                    /*
                    for (i = 0; i < 13; i++)
                    {
                        if (i < 4)  // vendor name
                        {
                            tmp_value = (HDMITX_VendorName[2*i+1]<<8) | HDMITX_VendorName[2*i];
                            MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_SPD_1_15+i, 0x7F7F, tmp_value);
                        }
                        else if ( (i>=4) && (i<12) )  // product description
                        {
                            tmp_value = (HDMITX_ProductName[2*(i-4)+1]<<8) | HDMITX_ProductName[2*(i-4)];
                            MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_SPD_5_19+(i-4), 0x7F7F, tmp_value);
                        }
                        else  // source device information
                        {
                            MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_SPD_13_21, 0x00FF, HDMITX_PACKET_SPD_SDI);
                        }
                    }
					*/
                    tmp_value = MHal_HDMITx_InfoFrameCheckSum(packet_type);  // Checksum
                    // cyclic packet
                    if (IS_CYCLIC_PKT(SPD_INFORAME.Define_Process))
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_SPD_CFG_22, ( (tmp_value<<8) | (SPD_INFORAME.Define_FCnt<<3) | 0x0005));  // send SPD packet
                    // single packet
                    else
                    {
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_SPD_CFG_22, (tmp_value<<8) | 0x0001);
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_SPD_CMD);
                    }
                }
			}
			else
			{
                if (IS_STOP_PKT(packet_process))   // Stop sending packet
                {
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_SPD_CFG_22, 0x0005, 0x0000);  // Stop SPD packet
                }
                else
                {
                    for (i = 0; i < 13; i++)
                    {
                        if (i < 4)  // vendor name
                        {
                            tmp_value = (HDMITX_VendorName[2*i+1]<<8) | HDMITX_VendorName[2*i];
                            MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_SPD_1_15+i, 0x7F7F, tmp_value);
                        }
                        else if ( (i>=4) && (i<12) )  // product description
                        {
                            tmp_value = (HDMITX_ProductName[2*(i-4)+1]<<8) | HDMITX_ProductName[2*(i-4)];
                            MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_SPD_5_19+(i-4), 0x7F7F, tmp_value);
                        }
                        else  // source device information
                        {
                            MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_SPD_13_21, 0x00FF, HDMITX_PACKET_SPD_SDI);
                        }
                    }
                    tmp_value = MHal_HDMITx_InfoFrameCheckSum(packet_type);  // Checksum
                    // cyclic packet
                    if (IS_CYCLIC_PKT(packet_process))
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_SPD_CFG_22, ( (tmp_value<<8) | (HDMITX_PACKET_SPD_FCNT<<3) | 0x0005));  // send SPD packet
                    // single packet
                    else
                    {
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_SPD_CFG_22, (tmp_value<<8) | 0x0001);
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_SPD_CMD);
                    }
                }
			}
            break;

        case E_HDMITX_AUDIO_INFOFRAME:
            if(AUDIO_INFORAME.User_Define)
			{
                if (IS_STOP_PKT(AUDIO_INFORAME.Define_Process))   // Stop sending packet
                {
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AUD_CFG_14, 0x0005, 0x0000);  // Stop audio packet
                }
                else
                {
                    // Modified for HDMI CTS test -
                    //     - Audio Coding Type (CT3~CT0) is 0x0 then continue else then FAIL
                    //     - Sampling Frequency (SF2~ SF0) is zero then continue else then FAIL.
                    //     - Sample Size (SS1~ SS0) is zero then continue else then FAIL.
                    //tmp_value = (gHDMITxInfo.output_audio_frequncy << 10) | 0x11;  // audio sampling frequency, PCM and 2 channel.
                    //MHal_HDMITx_Mask_Write(REG_HDMITX_BANK1, REG_PKT_AUD_1_11, 0x1FFF, tmp_value);

                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AUD_3_13, 0x0001, 0x0000); //Fix LFEP defalut value in Kappa.
                    if(achidx == E_HDMITX_AUDIO_CH_2) // 2 channels
                    {
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_AUD_1_11, 0x01); // 2 channels
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AUD_2_12, 0xFF00, 0); // Channel allocation
                    }
                    else // 8 channels
                    {
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_AUD_1_11, 0x07); // 8 channels
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AUD_2_12, 0xFF00, 0x1F00); // Channel allocation
                    }
                    tmp_value = MHal_HDMITx_InfoFrameCheckSum(packet_type);  // Checksum
                    // cyclic packet
                    if (IS_CYCLIC_PKT(AUDIO_INFORAME.Define_Process))
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_AUD_CFG_14, ( (tmp_value<<8) | (AUDIO_INFORAME.Define_FCnt<<3) | 0x0005));  // send audio packet
                    // single packet
                    else
                    {
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_AUD_CFG_14, (tmp_value<<8) | 0x0001);
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_AUD_CMD);
                    }
                }
			}
			else
            {
                if (IS_STOP_PKT(packet_process))   // Stop sending packet
                {
                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AUD_CFG_14, 0x0005, 0x0000);  // Stop audio packet
                }
                else
                {
                    // Modified for HDMI CTS test -
                    //     - Audio Coding Type (CT3~CT0) is 0x0 then continue else then FAIL
                    //     - Sampling Frequency (SF2~ SF0) is zero then continue else then FAIL.
                    //     - Sample Size (SS1~ SS0) is zero then continue else then FAIL.
                    //tmp_value = (gHDMITxInfo.output_audio_frequncy << 10) | 0x11;  // audio sampling frequency, PCM and 2 channel.
                    //MHal_HDMITx_Mask_Write(REG_HDMITX_BANK1, REG_PKT_AUD_1_11, 0x1FFF, tmp_value);

                    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AUD_3_13, 0x0001, 0x0000); //Fix LFEP defalut value in Kappa.
                    if(achidx == E_HDMITX_AUDIO_CH_2) // 2 channels
                    {
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_AUD_1_11, 0x01); // 2 channels
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AUD_2_12, 0xFF00, 0); // Channel allocation
                    }
                    else // 8 channels
                    {
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_AUD_1_11, 0x07); // 8 channels
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AUD_2_12, 0xFF00, 0x1F00); // Channel allocation
                    }
                    tmp_value = MHal_HDMITx_InfoFrameCheckSum(packet_type);  // Checksum
                    // cyclic packet
                    if (IS_CYCLIC_PKT(packet_process))
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_AUD_CFG_14, ( (tmp_value<<8) | (HDMITX_PACKET_AUD_FCNT<<3) | 0x0005));  // send audio packet
                    // single packet
                    else
                    {
                        MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_AUD_CFG_14, (tmp_value<<8) | 0x0001);
                        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_AUD_CMD);
                    }
                }
            }
            // Channel status
            {
                MHal_HDMITx_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CH_STATUS0_00, ((actidx == E_HDMITX_AUDIO_PCM) ? 0 : BIT1) ); // [1]: PCM / non-PCM
                MHal_HDMITx_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CH_STATUS1_01, 0);
                // Audio sampling frequency
                // 1           1        0        0        32 kHz
                // 0           0        0        0        44.1 kHz
                // 0           0        0        1        88.2 kHz
                // 0           0        1        1        176.4 kHz
                // 0           1        0        0        48 kHz
                // 0           1        0        1        96 kHz
                // 0           1        1        1        192 kHz
                // 1           0        0        1        768 kHz
                MHal_HDMITx_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CH_STATUS2_02, (achidx<<4)); //[7:4]: audio channel count
                MHal_HDMITx_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CH_STATUS3_03, TxAudioFreqTbl[afidx].CH_Status3); //[3:0]: audio sampling frequncy
                MHal_HDMITx_Write(HDMITX_AUDIO_REG_BASE, REG_AE_CH_STATUS4_04, 0);
            }
            break;

        case E_HDMITX_MPEG_INFOFRAME: // unsupport in Uranus
            break;
        default:
            break;
    }
}

//------------------------------------------------------------------------------
/// @brief This routine sets video output mode (color/repetition/regen)
/// @param[in] idx: gHDMITxInfo.output_video_timing
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_SetVideoOutputMode(MsHDMITX_VIDEO_TIMING idx, MS_BOOL bflag, MsHDMITX_VIDEO_COLORDEPTH_VAL cd_val, MsHDMITX_ANALOG_TUNING *pInfo)
{
    MS_U16 reg_value=0;

    // Deep color FIFO reset
    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_HDMI_CONFIG1_00, BIT10, BIT10);
    MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_HDMI_CONFIG1_00, BIT10, 0);
    MsOS_DelayTask(10);


    // Interlace mode
    if(HDMITxVideoModeTbl[idx].i_p_mode == E_HDMITX_VIDEO_INTERLACE_MODE)
    {
    #if 0 // already set by HD gen
        // HDMI pixel repetition rate
        if(gHDMITxInfo.output_video_timing<=E_HDMITX_RES_720x576i) // 480i/576i
            (*((volatile MS_U16*)(0xBF805E00 + ((0x20)<<2)))) |= BIT11; //SDTV interlace mode
        else // 1080i
            (*((volatile MS_U16*)(0xBF805E00 + ((0x20)<<2)))) &= ~BIT11;
    #endif
        //MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_12, 0x000F, 0); // [3]: manual mode of pixel-repetition disable, IC would stuff automatically by AVI setting
        // [8]=1, BT656 timing; [0]=1, interlace mode
        MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_00, 0x017F, 0x004F);
    }
    else
    {
        // [8]=0, BT656 timing; [0]=0, progressive mode
        MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_00, 0x017F, 0x004E);
        // /2 input video clock at interlaced mode
        //MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_1C, 0x0001, 0x0000);
        // HDMI pixel repetition rate
        //MHal_HDMITx_Mask_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_12, 0x000F, 0x0000);
    #if 0 // already set by HD gen
        (*((volatile MS_U16*)(0xBF805E00 + ((0x20)<<2)))) &= ~BIT11;
    #endif
    }

#if 0 // Aaron - HDMI PLL setting is already set by scaler side
#ifdef OBERON
    // Output divider setting for Oberon
    // Analog Interface    | Register Address | 1080i(74.25M) | 480p(27M) | 480pX2(54M) | 480pX3(81M)
    // TM_INOUT_DIV[1:0]   | h2708 [7:6]      | 2'b11         | 2'b11     | 2'b11       | 2'b11
    // TM_INPUT_RATIO[3:0] | h2706[3:0]       | 4'h2          | 4'h0      | 4'h0        | 4'h0
    // TM_LOOP_DIV[1:0]    | h270f[6:5]       | 2'b00         | 2'b01     | 2'b10       | 2'b01
    // TM_DIV_D[3:0]       | h2710[3:0]       | 4'hb          | 4'h0      | 4'h0        | 4'h3
    // TM_REG[24]          | h270d[0]         | 1'b0          | 1'b1      | 1'b0        | 1'b0
    // TM_ICTRL[2:0]       | h2706[6:4]       | 3'b011        | 3'b010    | 3'b010      | 3'b100

    if(gHDMITxInfo.output_video_timing <= E_HDMITX_RES_720x576p) // 480i/576i/480p/576p
    {
        MHal_HDMITx_Mask_Write(REG_HDMITX_BANK0, REG_MISC_CONFIG_04, 0x00C0, 0x00C0);
        MHal_HDMITx_Mask_Write(REG_HDMITX_BANK0, REG_MISC_CONFIG_03, 0x007F, 0x0020);
        MHal_HDMITx_Mask_Write(REG_HDMITX_BANK0, REG_MISC_CONFIG_07, 0x6000, 0x2000);
        MHal_HDMITx_Mask_Write(REG_HDMITX_BANK0, REG_MISC_CONFIG_08, 0x000F, 0x0000);
        MHal_HDMITx_Mask_Write(REG_HDMITX_BANK0, REG_MISC_CONFIG_06, 0x0100, 0x0100);
    }
    else // 720p/1080i
    {
        MHal_HDMITx_Mask_Write(REG_HDMITX_BANK0, REG_MISC_CONFIG_04, 0x00C0, 0x00C0);
        MHal_HDMITx_Mask_Write(REG_HDMITX_BANK0, REG_MISC_CONFIG_03, 0x007F, 0x0032);
        MHal_HDMITx_Mask_Write(REG_HDMITX_BANK0, REG_MISC_CONFIG_07, 0x6000, 0x0000);
        MHal_HDMITx_Mask_Write(REG_HDMITX_BANK0, REG_MISC_CONFIG_08, 0x000F, 0x000B);
        MHal_HDMITx_Mask_Write(REG_HDMITX_BANK0, REG_MISC_CONFIG_06, 0x0100, 0x0000);
    }
#endif // #ifdef OBERON
#endif

    // Hsync/Vsync polarity and CSC
    reg_value = MHal_HDMITx_Read(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_01) & 0xFEF8; // Aaron 20090112
    reg_value = reg_value | (HDMITxVideoModeTbl[idx].v_polarity<<2)
		| (HDMITxVideoModeTbl[idx].h_polarity<<1) | BIT0 | BIT15;
    //if(bflag) // bypass
    //    reg_value |= BIT8;
    MHal_HDMITx_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_01, reg_value);
    reg_value = MHal_HDMITx_Read(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_17) & 0xDBFF;
    reg_value = reg_value | BIT10;
	MHal_HDMITx_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_17, reg_value);

#if 0 // already set by HD gen
#ifdef OBERON
    // U02 bug fix - Timing generator setting for HDMI
    if(HDMITxVideoModeTbl[gHDMITxInfo.output_video_timing].i_p_mode == E_HDMITX_VIDEO_INTERLACE_MODE)
    {
        if(gHDMITxInfo.output_video_timing == E_HDMITX_RES_720x480i)
        {
            (*((volatile MS_U16*)(0xBF805E00 + ((0x1E)<<2)))) = 0x02F0; // HDMI Hfp
            (*((volatile MS_U16*)(0xBF805E00 + ((0x23)<<2)))) = 0x2000; // HDMI filed delay
        }
        else if (gHDMITxInfo.output_video_timing == E_HDMITX_RES_720x576i)
        {
            (*((volatile MS_U16*)(0xBF805E00 + ((0x1E)<<2)))) = 0x02E8; // HDMI Hfp
            (*((volatile MS_U16*)(0xBF805E00 + ((0x23)<<2)))) = 0x2000; // HDMI filed delay
        }
        else if(gHDMITxInfo.output_video_timing == E_HDMITX_RES_1920x1080i_50Hz)
        {
            (*((volatile MS_U16*)(0xBF805E00 + ((0x1E)<<2)))) = 0x099D; // HDMI Hfp
            (*((volatile MS_U16*)(0xBF805E00 + ((0x23)<<2)))) = 0x8000; // HDMI filed delay
        }
        else if (gHDMITxInfo.output_video_timing == E_HDMITX_RES_1920x1080i_60Hz)
        {
            (*((volatile MS_U16*)(0xBF805E00 + ((0x1E)<<2)))) = 0x07E4; // HDMI Hfp
            (*((volatile MS_U16*)(0xBF805E00 + ((0x23)<<2)))) = 0x8000; // HDMI filed delay
        }
    }
    else
    {
        (*((volatile MS_U16*)(0xBF805E00 + ((0x1E)<<2)))) = 0;
        (*((volatile MS_U16*)(0xBF805E00 + ((0x23)<<2)))) = 0;
    }
#endif
#endif

#if 0
    // DVI PLL divider 1/2/4/8
    if(idx <= E_HDMITX_RES_720x576p) // <50MHz
    {
        if(cd_val >= E_HDMITX_VIDEO_CD_30Bits)
            MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_38, BIT3|BIT2, BIT3); // /2'd2
        else
            MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_38, BIT3|BIT2, BIT3|BIT2); // /2'd3
        //MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_33, BIT6|BIT5|BIT4, BIT5|BIT4); // 3'd3
    }
    else if((idx > E_HDMITX_RES_720x576p) && (idx <= E_HDMITX_RES_1920x1080p_30Hz)) // >50MHz < 100MHz
    {
        if(cd_val >= E_HDMITX_VIDEO_CD_48Bits)
            MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_38, BIT3|BIT2, 0); // /2'd0
        else
            MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_38, BIT3|BIT2, BIT2); // 2'd1
        //MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_33, BIT6|BIT5|BIT4, BIT5|BIT4); //3'd3
    }
    else // >100MHz
    {
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_38, BIT3|BIT2, 0); // 2'd0
        //MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_33, BIT6|BIT5|BIT4, BIT5|BIT4); // 3'd3
    }

    // Double termination - TMDS clock > 165MHZ should be enable data double termination in 4 Layer board
    // Double termination - TMDS clock >= 1080p@8bits should be enable data double termination in 2 Layer board
    if((idx >= E_HDMITX_RES_1920x1080p_50Hz) && (cd_val >= E_HDMITX_VIDEO_CD_30Bits))
    {
        if(cd_val == E_HDMITX_VIDEO_CD_36Bits)
        {
            MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2B, 0xFFFF, 0x0C81);
            MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2E, 0xFFFF, 0x0F9F);
            MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2C, 0xFFFF, 0x0001);
            MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2F, 0xFFFF, 0x0070);
        }
        else
        {
            MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2B, 0xFFFF, 0x0C01);
            MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2E, 0xFFFF, 0x0F2F);
            MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2C, 0xFFFF, 0x0007);
            MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2F, 0xFFFF, 0x0000);
        }
        //printf("1080p and deep color mode \r\n");
    }
    else
    {
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2B, 0xFFFF, 0x0C01);
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2F, 0xFFFF, 0x0000);
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2E, 0xFFFF, 0x0F9F);
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2C, 0xFFFF, 0x0007);
        //printf("not 1080p and deep color mode \r\n");
    }
    MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_33, 0xFFFF, 0x0020);
#endif

#if 0
    // pre-emphasis mode control
    if(pInfo->tm_pren2)
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2D, BIT5, BIT5);
    else
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2D, BIT5, 0);
    if(pInfo->tm_ten)
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2B, BIT1, 0); // power on data termination
    else
        MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2B, BIT1, BIT1); // power on data termination
    MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2B, BIT13|BIT12|BIT11, pInfo->tm_txcurrent << 11); // [13:11]: tx current control
    MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2E, 0x0FF0, pInfo->tm_pren <<8 | pInfo->tm_precon << 4); // [11:8]: pre-emphasis enable, [7:4]: Precon
    MHal_HDMITx_Mask_Write(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_2F, 0x00FF, pInfo->tm_ten << 4 | pInfo->tm_tenpre); // [7:4]: double ternination enable, [3:0]: pre-emphasis of double term enable
#endif

    // Timing regeneration
    MHal_HDMITx_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_02, HDMITxVideoModeTbl[idx].vs_width);
    MHal_HDMITx_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_03, HDMITxVideoModeTbl[idx].vs_bporch);
    MHal_HDMITx_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_04, HDMITxVideoModeTbl[idx].vde_width);
    MHal_HDMITx_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_05, HDMITxVideoModeTbl[idx].vs_delayline);
    MHal_HDMITx_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_06, HDMITxVideoModeTbl[idx].vs_delaypixel);
    MHal_HDMITx_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_07, HDMITxVideoModeTbl[idx].hs_width);
    MHal_HDMITx_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_08, HDMITxVideoModeTbl[idx].hs_bporch);
    MHal_HDMITx_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_09, HDMITxVideoModeTbl[idx].hde_width);
    MHal_HDMITx_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_0A, HDMITxVideoModeTbl[idx].hs_delay);
    MHal_HDMITx_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_13, HDMITxVideoModeTbl[idx].vtotal);
    MHal_HDMITx_Write(HDMITX_VIDEO_REG_BASE, REG_VE_CONFIG_14, HDMITxVideoModeTbl[idx].htotal);
}

//------------------------------------------------------------------------------
/// @brief This routine will power on or off HDMITx clock (power saving)
/// @param[in] bEnable: TRUE/FALSE
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_Power_OnOff(MS_BOOL bEnable)
{
    if (bEnable)
    {
        //MHal_HDMITx_Mask_Write(CLKGEN1_REG_BASE, REG_CKG_HDMITx_CLK_28, BIT0, 0);
        MHal_HDMITx_Mask_Write(CLKGEN0_REG_BASE, REG_CKG_HDMIT_Tx_CLK_34, 0x0001, 0x0000); // enable HDMITx CLK
        MHal_HDMITx_Mask_Write(CLKGEN0_REG_BASE, REG_CKG_HDMI_PLL_CLK_5F, 0x0100, 0x0000); // enable HDMI PLL
    }
    else
    {
        //MHal_HDMITx_Mask_Write(CLKGEN1_REG_BASE, REG_CKG_HDMITx_CLK_28, BIT0, BIT0);
        MHal_HDMITx_Mask_Write(CLKGEN0_REG_BASE, REG_CKG_HDMIT_Tx_CLK_34, 0x0001, 0x0001); // enable HDMITx CLK
        MHal_HDMITx_Mask_Write(CLKGEN0_REG_BASE, REG_CKG_HDMI_PLL_CLK_5F, 0x0100, 0x0100); // enable HDMI PLL

    }
}


MS_BOOL MHal_HDMITx_RxBypass_Mode(MsHDMITX_INPUT_FREQ freq, MS_BOOL bflag)
{
	return FALSE;
}


MS_BOOL MHal_HDMITx_Disable_RxBypass(void)
{
    return FALSE;
}

//------------------------------------------------------------------------------
/// @brief This routine will set GPIO pin for HPD
/// @param[in] u8pin: GPIO0 ~ 12
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_SetHPDGpioPin(MS_U8 u8pin)
{
    _gHPDGpioPin = u8pin;
}

//------------------------------------------------------------------------------
/// @brief This routine return CHIP capability of DVI mode
/// @return TRUE, FALSE
//------------------------------------------------------------------------------
MS_BOOL MHal_HDMITx_IsSupportDVIMode(void)
{
    MS_BOOL bSupport = TRUE;

    return bSupport;
}

// *************  For customer NDS **************//

void MHal_HDMITx_Set_AVI_InfoFrame(MsHDMITX_PACKET_PROCESS packet_process, MsHDMITX_AVI_CONTENT_TYPE content_type, MS_U16 *data)
{
    MS_U16 tmp_value=0;

    if (IS_STOP_PKT(packet_process))   // Stop sending packet
    {
        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_CFG_10, 0x0005, 0x0000);  // Stop AVI packet
    }
    else
    {
        MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_1_09, 0x0080, 0x80); // EIA version 2
        switch(content_type)
        {
            case E_HDMITX_AVI_PIXEL_FROMAT:
                MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_1_09, 0x0060, *data);
		break;
            case E_HDMITX_AVI_ASPECT_RATIO:
                MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_1_09, 0x3F1F, *data);
                MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_2_0A, 0x0003, *(data+1));
                MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_4_0C, 0x0003, *(data+2));
                MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_5_0D, 0x0003, *(data+3));
                MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_6_0E, 0x0003, *(data+4));
                MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_7_0F, 0x0003, *(data+5));
                break;
            case E_HDMITX_AVI_COLORIMETRY:
                MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_1_09, 0xC000, *data);
                MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_PKT_AVI_2_0A, 0x0030, *(data+1));
                break;
            default:
                break;
        }

        tmp_value = MHal_HDMITx_InfoFrameCheckSum(E_HDMITX_AVI_INFOFRAME);  // Checksum
        // cyclic packet
        if (IS_CYCLIC_PKT(packet_process))
            MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_AVI_CFG_10, ( (tmp_value<<8) | (HDMITX_PACKET_AVI_FCNT<<3) | 0x0005));  // send AVI packet
        // single packet
        else
        {
            MHal_HDMITx_Write(HDMITX_REG_BASE, REG_PKT_AVI_CFG_10, (tmp_value<<8) | 0x0001);
            MHal_HDMITx_Mask_Write(HDMITX_REG_BASE, REG_ACT_HDMI_PKTS_CMD_01, 0x03FF, 1<<E_HDMITX_ACT_AVI_CMD);
        }
    }
}
