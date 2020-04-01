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

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    regHDMITx.h
/// @author MStar Semiconductor,Inc.
/// @brief  HDMITx Register Definition
/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _REG_HDMITX_H_
#define _REG_HDMITX_H_

//#include "MsCommon.h"

//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

#define HDMITX_MISC_HDCP_REG_BASE       (0x151400)
#define HDMITX_REG_BASE                 (0x151500)
#define HDMITX_VIDEO_REG_BASE           (0x151600)
#define HDMITX_AUDIO_REG_BASE           (0x151700)
#define PMBK_PMSLEEP_REG_BASE           (0x000E00)
#define CLKGEN1_REG_BASE                (0x103300)
#define CLKGEN0_REG_BASE                (0x100B00)
#define CHIPTOP_REG_BASE                (0x102600)
#define PM_GPIO_REG_BASE                (0x000F00)

//***** Bank 1026 - PADTOP *****//
#define REG_SYNC_GPIO0                  0x1E

//***** Bank 1033(0x28) - CHIPTOP *****//
#define REG_CKG_HDMITx_CLK_28           0x28
#define REG_I2S_GPIO4                   0x1B

//***** Bank 100B - CLKGNE0 *****//
#define REG_CKG_HDMIT_Tx_CLK_34         0x34
#define REG_CKG_HDMI_PLL_CLK_5F         0x5F

//***** Bank 1514 - HDMITX *****//
#define REG_HDMI_CONFIG1_00             0x00
#define REG_ACT_HDMI_PKTS_CMD_01        0x01
#define REG_PKT_NUL_CFG_02              0x02
#define REG_PKT_GC_CFG_03               0x03
#define REG_PKT_GC12_04                 0x04
#define REG_PKT_ACR_1_05                0x05
#define REG_PKT_ACR_2_06                0x06
#define REG_PKT_ACR_3_07                0x07
#define REG_PKT_ACR_CFG_08              0x08
#define REG_PKT_AVI_1_09                0x09
#define REG_PKT_AVI_2_0A                0x0A
#define REG_PKT_AVI_3_0B                0x0B
#define REG_PKT_AVI_4_0C                0x0C
#define REG_PKT_AVI_5_0D                0x0D
#define REG_PKT_AVI_6_0E                0x0E
#define REG_PKT_AVI_7_0F                0x0F
#define REG_PKT_AVI_CFG_10              0x10
#define REG_PKT_AUD_1_11                0x11
#define REG_PKT_AUD_2_12                0x12
#define REG_PKT_AUD_3_13                0x13
#define REG_PKT_AUD_CFG_14              0x14
#define REG_PKT_SPD_1_15                0x15
#define REG_PKT_SPD_2_16                0x16
#define REG_PKT_SPD_3_17                0x17
#define REG_PKT_SPD_4_18                0x18
#define REG_PKT_SPD_5_19                0x19
#define REG_PKT_SPD_6_1A                0x1A
#define REG_PKT_SPD_7_1B                0x1B
#define REG_PKT_SPD_8_1C                0x1C
#define REG_PKT_SPD_9_1D                0x1D
#define REG_PKT_SPD_10_1E               0x1E
#define REG_PKT_SPD_11_1F               0x1F
#define REG_PKT_SPD_12_20               0x20
#define REG_PKT_SPD_13_21               0x21
#define REG_PKT_SPD_CFG_22              0x22
#define REG_PKT_MPG_1_23                0x23
#define REG_PKT_MPG_2_24                0x24
#define REG_PKT_MPG_3_25                0x25
#define REG_PKT_MPG_CFG_26              0x26
#define REG_PKT_VS_1_27                 0x27
#define REG_PKT_VS_2_28                 0x28
#define REG_PKT_VS_3_29                 0x29
#define REG_PKT_VS_4_2A                 0x2A
#define REG_PKT_VS_5_2B                 0x2B
#define REG_PKT_VS_6_2C                 0x2C
#define REG_PKT_VS_7_2D                 0x2D
#define REG_PKT_VS_8_2E                 0x2E
#define REG_PKT_VS_9_2F                 0x2F
#define REG_PKT_VS_10_30                0x30
#define REG_PKT_VS_11_31                0x31
#define REG_PKT_VS_12_32                0x32
#define REG_PKT_VS_13_33                0x33
#define REG_PKT_VS_14_34                0x34
#define REG_PKT_VS_CFG_35               0x35
#define REG_USER_TYPE_36                0x36
#define REG_USER_HB_37                  0x37
#define REG_PKT_ACP_0_38                0x38
#define REG_PKT_ACP_1_39                0x39
#define REG_PKT_ACP_2_3A                0x3A
#define REG_PKT_ACP_3_3B                0x3B
#define REG_PKT_ACP_4_3C                0x3C
#define REG_PKT_ACP_5_3D                0x3D
#define REG_PKT_ACP_6_3E                0x3E
#define REG_PKT_ACP_7_3F                0x3F
#define REG_PKT_ACP_CFG_40              0x40
#define REG_PKT_ISRC_0_41               0x41
#define REG_PKT_ISRC_1_42               0x42
#define REG_PKT_ISRC_2_43               0x43
#define REG_PKT_ISRC_3_44               0x44
#define REG_PKT_ISRC_4_45               0x45
#define REG_PKT_ISRC_5_46               0x46
#define REG_PKT_ISRC_6_47               0x47
#define REG_PKT_ISRC_7_48               0x48
#define REG_PKT_ISRC_8_49               0x49
#define REG_PKT_ISRC_9_4A               0x4A
#define REG_PKT_ISRC_10_4B              0x4B
#define REG_PKT_ISRC_11_4C              0x4C
#define REG_PKT_ISRC_12_4D              0x4D
#define REG_PKT_ISRC_13_4E              0x4E
#define REG_PKT_ISRC_14_4F              0x4F
#define REG_PKT_ISRC_15_50              0x50
#define REG_PKT_ISRC_CFG_51             0x51
#define REG_TMDS_DE_CNT_52              0x52
#define REG_HPLL_LOCK_CNT_53            0x53
#define REG_PKT_GM_CFG_54               0x54
#define REG_PKT_GM_HB2_55               0x55
#define REG_PKT_GM_1_56                 0x56
#define REG_PKT_GM_3_57                 0x57
#define REG_PKT_GM_5_58                 0x58
#define REG_PKT_GM_7_59                 0x59
#define REG_PKT_GM_9_5A                 0x5A
#define REG_PKT_GM_11_5B                0x5B
#define REG_PKT_GM_13_5C                0x5C
#define REG_PKT_GM_15_5D                0x5D
#define REG_PKT_GM_17_5E                0x5E
#define REG_PKT_GM_19_5F                0x5F

//***** Bank 1516 - HDMITX_Video *****//
#define REG_VE_CONFIG_00                   0x00
#define REG_VE_CONFIG_01                   0x01
#define REG_VE_CONFIG_02                   0x02
#define REG_VE_CONFIG_03                   0x03
#define REG_VE_CONFIG_04                   0x04
#define REG_VE_CONFIG_05                   0x05
#define REG_VE_CONFIG_06                   0x06
#define REG_VE_CONFIG_07                   0x07
#define REG_VE_CONFIG_08                   0x08
#define REG_VE_CONFIG_09                   0x09
#define REG_VE_CONFIG_0A                   0x0A
#define REG_VE_CONFIG_0B                   0x0B
#define REG_VE_CONFIG_0C                   0x0C
#define REG_VE_CONFIG_0D                   0x0D
#define REG_VE_CONFIG_0E                   0x0E
#define REG_VE_CONFIG_0F                   0x0F
#define REG_VE_CONFIG_10                   0x10
#define REG_VE_CONFIG_11                   0x11
#define REG_VE_CONFIG_12                   0x12
#define REG_VE_CONFIG_13                   0x13
#define REG_VE_CONFIG_14                   0x14
#define REG_VE_STATUS_15                   0x15
#define REG_VE_STATUS_16                   0x16
#define REG_VE_CONFIG_17                   0x17
#define REG_VE_CONFIG_18                   0x18
#define REG_VE_CONFIG_20                   0x20
#define REG_VE_CONFIG_21                   0x21
#define REG_VE_CONFIG_22                   0x22
#define REG_VE_CONFIG_23                   0x23
#define REG_VE_CONFIG_24                   0x24
#define REG_VE_CONFIG_25                   0x25
#define REG_VE_CONFIG_26                   0x26
#define REG_VE_CONFIG_27                   0x27
#define REG_VE_CONFIG_28                   0x28
#define REG_VE_CONFIG_29                   0x29

//***** Bank 1517 - HDMITX_Audio *****//
#define REG_AE_CH_STATUS0_00           0x00
#define REG_AE_CH_STATUS1_01           0x01
#define REG_AE_CH_STATUS2_02           0x02
#define REG_AE_CH_STATUS3_03           0x03
#define REG_AE_CH_STATUS4_04           0x04
#define REG_AE_CONFIG_05                   0x05
#define REG_AE_STATUS_06                   0x06
#define REG_AE_STATUS_07                   0x07

//***** Bank 1514(0x00~0x5F) - MISC *****//
#define REG_MISC_CONFIG_00               0x00
#define REG_MISC_CONFIG_01               0x01
#define REG_MISC_CONFIG_02               0x02
#define REG_MISC_CONFIG_03               0x03
#define REG_MISC_CONFIG_04               0x04
#define REG_MISC_CONFIG_05               0x05
#define REG_MISC_CONFIG_06               0x06
#define REG_MISC_CONFIG_07               0x07
#define REG_MISC_CONFIG_08               0x08
#define REG_MISC_CONFIG_09               0x09
#define REG_MISC_STATUS_0A               0x0A
#define REG_MISC_STATUS_0B               0x0B
#define REG_MISC_CONFIG_0C               0x0C
#define REG_MISC_STATUS_0D               0x0D
#define REG_MISC_STATUS_0E               0x0E
#define REG_MISC_STATUS_0F               0x0F
#define REG_MISC_CONFIG_1C               0x1C
#define REG_MISC_CONFIG_1D               0x1D
#define REG_MISC_CONFIG_1E               0x1E
#define REG_MISC_CONFIG_1F               0x1F
#define REG_MISC_CONFIG_2A               0x2A
#define REG_MISC_CONFIG_2B               0x2B
#define REG_MISC_CONFIG_2C               0x2C
#define REG_MISC_CONFIG_2D               0x2D
#define REG_MISC_CONFIG_2E               0x2E
#define REG_MISC_CONFIG_2F               0x2F
#define REG_MISC_CONFIG_33               0x33
#define REG_MISC_CONFIG_34               0x34
#define REG_MISC_CONFIG_36               0x36
#define REG_MISC_CONFIG_38               0x38
#define REG_MISC_CONFIG_40               0x40
#define REG_MISC_CONFIG_41               0x41
#define REG_MISC_CONFIG_45               0x45
#define REG_MISC_CONFIG_48               0x48
#define REG_MISC_CONFIG_4D               0x4D
#define REG_MISC_CONFIG_58               0x58
#define REG_MISC_CONFIG_5C               0x5C

//***** Bank 1514(0x60~0x7F) - HDCP *****//
#define REG_HDCP_TX_RI_60               0x60
#define REG_HDCP_TX_MODE_61             0x61	// Pj[7:0] : 61h[7:0]; Tx_mode[7:0] : 61h[15:8]
#define REG_HDCP_TX_COMMAND_62          0x62
#define REG_HDCP_TX_RI127_63            0x63       // RI[15:0] 127th frame : 63[15:0]
#define REG_HDCP_TX_LN_64               0x64	// Ln[55:0] : 64h[7:0] ~ 67h[7:0]
#define REG_HDCP_TX_LN_SEED_67          0x67	// Ln seed[7:0] : 67h[15:8]
#define REG_HDCP_TX_AN_68               0x68	// An[63:0] : 68[7:0] ~ 6B[15:8]
#define REG_HDCP_TX_MI_6C               0x6C	// Mi[63:0] : 6C[7:0] ~ 6F[15:8]


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

#endif // _REG_HDMITX_H_

