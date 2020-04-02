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
// [mhal_mux.h]
// Date: 20090220
// Descriptions: Add a new mux layer for HW setting
//==============================================================================
#ifndef MHAL_MUX_H
#define MHAL_MUX_H

///////////////////////////////
//  Mux Hardware Option
///////////////////////////////
// Please set this #define after Mux tree is ready
#define MUX_TREE_HEIGHT 4

#define NUMBER_OF_ANALOG_PORT 3
#define NUMBER_OF_CVBS_PORT 8
#define NUMBER_OF_DVI_PORT 3
#define NUMBER_OF_MVOP_PORT 2
#define NUMBER_OF_CVBS_OUT_PORT 2
#define NUMBER_OF_YPBPR_OUT_PORT 0
#define PIP_SUPPORTED (MAX_WINDOW_NUM > 1)
#define NUMBER_OF_SCALER_OP_PORT 1

/*
    Input ports.
    It is interface between Hal and Driver level.
*/

typedef enum
{
    HAL_INPUTPORT_NONE = INPUT_PORT_NONE_PORT,

    HAL_INPUTPORT_ANALOG0 = INPUT_PORT_ANALOG0,
    HAL_INPUTPORT_ANALOG1,
    HAL_INPUTPORT_ANALOG2,

    HAL_INPUTPORT_YMUX_CVBS0 = INPUT_PORT_YMUX_CVBS0,
    HAL_INPUTPORT_YMUX_CVBS1,
    HAL_INPUTPORT_YMUX_CVBS2,
    HAL_INPUTPORT_YMUX_CVBS3,
    HAL_INPUTPORT_YMUX_CVBS4,
    HAL_INPUTPORT_YMUX_CVBS5,
    HAL_INPUTPORT_YMUX_CVBS6,
    HAL_INPUTPORT_YMUX_CVBS7,

    HAL_INPUTPORT_CMUX_CVBS0 = INPUT_PORT_CMUX_CVBS0,
    HAL_INPUTPORT_CMUX_CVBS1,
    HAL_INPUTPORT_CMUX_CVBS2,
    HAL_INPUTPORT_CMUX_CVBS3,
    HAL_INPUTPORT_CMUX_CVBS4,
    HAL_INPUTPORT_CMUX_CVBS5,
    HAL_INPUTPORT_CMUX_CVBS6,
    HAL_INPUTPORT_CMUX_CVBS7,

    HAL_INPUTPORT_DVI0 = INPUT_PORT_DVI0,
    HAL_INPUTPORT_DVI1,
    HAL_INPUTPORT_DVI2,

    HAL_INPUTPORT_MVOP = INPUT_PORT_MVOP,
    HAL_INPUTPORT_MVOP2,

    HAL_INPUTPORT_SCALER_OP = INPUT_PORT_SCALER_OP,
} E_INPUT_PORT_TYPE;

// Output ports.

typedef enum
{
    HAL_OUTPUTPORT_NONE_PORT = OUTPUT_PORT_NONE_PORT,

    HAL_OUTPUTPORT_SCALER_MAIN_WINDOW = OUTPUT_PORT_SCALER_MAIN_WINDOW,

    HAL_OUTPUTPORT_SCALER_SUB_WINDOW1 = OUTPUT_PORT_SCALER_SUB_WINDOW1,

    HAL_OUTPUTPORT_CVBS1 = OUTPUT_PORT_CVBS1,
    HAL_OUTPUTPORT_CVBS2 ,

    HAL_OUTPUTPORT_DIP_WINDOW = OUTPUT_PORT_DWIN,

}E_OUTPUT_PORT_TYPE;

typedef enum
{
    SC_MAINWIN_IPMUX_HDMI_DVI       = 1,            ///< DVI
    SC_MAINWIN_IPMUX_OP1            = 2,            ///< SC0 OP12SC1
    SC_MAINWIN_IPMUX_MVOP           = 3,            ///< MPEG/DC0
    SC_MAINWIN_IPMUX_IP_MAIN        = 4,            ///< IP MAIN
    SC_MAINWIN_IPMUX_HDR            = 6,            ///< HDR output
    SC_MAINWIN_IPMUX_MVOP2          = 8,            ///< MPEG/DC1
    SC_MAINWIN_IPMUX_SC1_CAPTURE    = 10,           ///< SC1 Capture
    SC_MAINWIN_IPMUX_H2V2           = 11,           ///< H2V2 output
} SC_MAINWIN_IPMUX_TYPE;

typedef enum
{
    SC_SUBWIN_IPMUX_HDMI_DVI        = 1,            ///< DVI
    SC_SUBWIN_IPMUX_OP1             = 2,            ///< SC0 OP12SC1
    SC_SUBWIN_IPMUX_MVOP            = 3,            ///< MPEG/DC0
    SC_SUBWIN_IPMUX_IP_MAIN         = 4,            ///< IP MAIN
    SC_SUBWIN_IPMUX_HDR             = 6,            ///< HDR output
    SC_SUBWIN_IPMUX_MVOP2           = 8,            ///< MPEG/DC1
    SC_SUBWIN_IPMUX_SC1_CAPTURE     = 10,           ///< SC1 Capture
    SC_SUBWIN_IPMUX_H2V2            = 11,           ///< H2V2 output
    SC_SUBWIN_IPMUX_ADC_B           = 15,           ///< ADC B, not using in Kano, only for fixing build error
} SC_SUBWIN_IPMUX_TYPE;

typedef enum
{
    SC_HDR_IPMUX_HDMI_DVI        = 1,            ///< DVI
    SC_HDR_IPMUX_OP1             = 2,            ///< SC0 OP12SC1
    SC_HDR_IPMUX_MVOP            = 3,            ///< MPEG/DC0
    SC_HDR_IPMUX_IP_MAIN         = 4,            ///< IP MAIN
    SC_HDR_IPMUX_HDR             = 6,            ///< HDR output
    SC_HDR_IPMUX_MVOP2           = 8,            ///< MPEG/DC1
    SC_HDR_IPMUX_SC1_CAPTURE     = 10,           ///< SC1 Capture
    SC_HDR_IPMUX_H2V2            = 11,           ///< H2V2 output
    SC_HDR_IPMUX_ADC_B           = 15,           ///< ADC B, not using in Kano, only for fixing build error
} SC_HDR_IPMUX_TYPE;

typedef enum
{
    SC_OFFLINE_IPMUX_ADC_A          = 0,            ///< ADC A
    SC_OFFLINE_IPMUX_HDMI_DVI       = 1,            ///< DVI
    SC_OFFLINE_IPMUX_VD             = 2,            ///< VD
    SC_OFFLINE_IPMUX_MVOP           = 3,            ///< MPEG/DC0
    SC_OFFLINE_IPMUX_IP_MAIN        = 4,            ///< IP MAIN
    SC_OFFLINE_IPMUX_EXT_VD         = 5,            ///< External VD
    SC_OFFLINE_IPMUX_ADC_B          = 6,            ///< ADC B
    SC_OFFLINE_IPMUX_CAPTURE        = 7,            ///< Capture
    SC_OFFLINE_IPMUX_MVOP2          = 8,            ///< MPEG/DC1
    SC_OFFLINE_IPMUX_IP_SUB         = 9,            ///< IP SUB
    SC_OFFLINE_IPMUX_SC1_CAPTURE    = 10,           ///< SC1 Capture
    SC_OFFLINE_IPMUX_SC_VOP         = 11,           ///< Scaler VOP output
    SC_OFFLINE_IPMUX_SC1_VOP        = 12,           ///< SC1 VOP output
} SC_OFFLINE_IPMUX_TYPE;

typedef enum
{
    SC_DWIN_IPMUX_HDMI_DVI          = 1,            ///< DVI
    SC_DWIN_IPMUX_OP1               = 7,            ///< SC0 OP12SC1
    SC_DWIN_IPMUX_MVOP              = 3,            ///< MPEG/DC0
    SC_DWIN_IPMUX_IP_MAIN           = 4,            ///< IP MAIN
    SC_DWIN_IPMUX_HDR               = 6,            ///< HDR output
    SC_DWIN_IPMUX_MVOP2             = 8,            ///< MPEG/DC1
    SC_DWIN_IPMUX_SC1_CAPTURE       = 10,           ///< SC1 Capture
    SC_DWIN_IPMUX_H2V2              = 11,           ///< H2V2 output
    SC_DWIN_IPMUX_IP_SUB     		= 9,            ///< IP SUB/VE
} SC_DWIN_IPMUX_TYPE;

typedef enum
{
    SC_CLK_ADC_A      = 0,            ///< ADC A
    SC_CLK_HDMI_DVI   = 1,            ///< DVI
    SC_CLK_VD         = 2,            ///< VD
    SC_CLK_DC0        = 3,            ///< MPEG/DC0
    SC_CLK_ADC_B      = 4,            ///< ADC_B

    SC_CLK_ODCLK      = 7,            ///< ODLCK
    SC_CLK_DC1        = 8,            ///< MPEG/DC1
    SC_CLK_ADC_C      = 9,            ///< ADC
    SC_CLK_ODCLK_B    = 10,           ///<ODCLK
    SC_CLK_MHL        = 11,           ///<MHL
} SC_IDCLK_TYPE;

typedef enum
{
    SC1_MAINWIN_IPMUX_HDMI_DVI      = 1,            ///< DVI
    SC1_MAINWIN_IPMUX_OP1           = 2,            ///< SC0 OP12SC1
    SC1_MAINWIN_IPMUX_MVOP          = 3,            ///< MPEG/DC0
    SC1_MAINWIN_IPMUX_IP_MAIN       = 4,            ///< IP MAIN
    SC1_MAINWIN_IPMUX_HDR           = 6,            ///< HDR output
    SC1_MAINWIN_IPMUX_MVOP2         = 8,            ///< MPEG/DC1
    SC1_MAINWIN_IPMUX_SC1_CAPTURE   = 10,           ///< SC1 Capture
    SC1_MAINWIN_IPMUX_H2V2          = 11,           ///< H2V2 output
} SC1_MAINWIN_IPMUX_TYPE;

typedef enum
{
    SC1_CLK_PRE_H2V2  = 0,            ///< Pre H2V2
    SC1_CLK_HDMI_DVI  = 1,            ///< DVI
    SC1_CLK_ODCLK     = 2,            ///< ODCLK
    SC1_CLK_DC0       = 3,            ///< MPEG/DC0
    SC1_CLK_DC1       = 8,            ///< MPEG/DC1
} SC1_IDCLK_TYPE;

typedef enum
{
    SC2_MAINWIN_IPMUX_ADC_A      = 0,            ///< ADC A
    SC2_MAINWIN_IPMUX_HDMI_DVI   = 1,            ///< DVI
    SC2_MAINWIN_IPMUX_VD         = 2,            ///< VD
    SC2_MAINWIN_IPMUX_MVOP       = 3,            ///< MPEG/DC0

    SC2_MAINWIN_IPMUX_CAPTURE    = 7,            ///< Capture
    SC2_MAINWIN_IPMUX_MVOP2      = 8,            ///< MPEG/DC1

    SC2_MAINWIN_IPMUX_SPT4K_CH0  = 11,           ///< 4K spliter ch0

    SC2_MAINWIN_IPMUX_MHL        = 13,           ///< MHL
    SC2_MAINWIN_IPMUX_SPT4K_CH1  = 14,           ///< 4K spliter ch1
} SC2_MAINWIN_IPMUX_TYPE;

typedef enum
{
    SC2_CLK_ADC_A      = 0,            ///< ADC A
    SC2_CLK_HDMI_DVI   = 1,            ///< DVI
    SC2_CLK_VD         = 2,            ///< VD
    SC2_CLK_DC0        = 3,            ///< MPEG/DC0
    SC2_CLK_ADC_B      = 4,            ///< ADC_B

    SC2_CLK_ODCLK      = 7,            ///< ODLCK
    SC2_CLK_DC1        = 8,            ///< MPEG/DC1
    SC2_CLK_ADC_C      = 9,            ///< ADC
    SC2_CLK_ODCLK_B    = 10,           ///<ODCLK
    SC2_CLK_MHL        = 13,           ///<MHL
} SC2_IDCLK_TYPE;

/*
void Hal_SC_mux_dispatch(void *pInstance, E_MUX_INPUTPORT src , E_MUX_OUTPUTPORT dest);
void Hal_SC_mux_set_dvi_mux(void *pInstance, MS_U8 PortId, E_OUTPUT_PORT_TYPE enDstPort);
void Hal_SC_mux_set_adc_y_mux(void *pInstance, MS_U8 PortId);
void Hal_SC_mux_set_adc_c_mux(void *pInstance, MS_U8 PortId);
#define Hal_SC_set_sync_port_by_dataport(args...)
MS_BOOL Hal_SC_mux_get_mainwin_ip_mux( void *pInstance, MS_U8 *pU8Data_Mux, MS_U8 *pU8Clk_Mux);
void Hal_SC_set_subwin_ip_mux( void *pInstance, MS_U8 u8Data_Mux, MS_U8 u8Clk_Mux);
void Hal_SC_mux_set_mainwin_ip_mux( void *pInstance, MS_U8 u8Data_Mux, MS_U8 u8Clk_Mux);

void Hal_SC_mux_set_dipwin_ip_mux(void *pInstance, MS_U8 u8Data_Mux, MS_U8 u8Clk_Mux);
void Hal_SC_EnableCLK_for_DIP(void *pInstance, MS_BOOL bEnable);
*/


#endif // MHAL_MUX_H
