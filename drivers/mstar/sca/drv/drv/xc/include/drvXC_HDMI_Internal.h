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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   drvXC_HDMI_Internal.h
/// @brief  Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRVXC_HDMI_INTERNAL_H_
#define _DRVXC_HDMI_INTERNAL_H_

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------

#include "drvXC_HDMI_if.h"

#ifdef _DRV_HDMI_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
//HDMI
INTERFACE MS_U16 MDrv_HDMI_packet_info(void);
INTERFACE MS_U8 MDrv_HDMI_avi_infoframe_info(MS_U8 u8byte);
INTERFACE void MDrv_HDMI_packet_ctrl(MS_U16 u16pkt);
INTERFACE MS_U16 MDrv_HDMI_pll_ctrl1(HDMI_PLL_CTRL_t pllctrl, MS_BOOL bread, MS_U16 u16value);
INTERFACE MS_U16 MDrv_HDMI_GetHDE(void);


//HDCP

//DVI
INTERFACE MS_BOOL   MDrv_HDMI_dvi_clock_detect(E_MUX_INPUTPORT enInputPortType);

//**Not use**
INTERFACE void MDrv_HDMI_hpd_en(MS_BOOL benable);
INTERFACE MS_BOOL MDrv_HDCP_isexist( void );

INTERFACE MS_U8 MDrv_HDMI_audio_cp_data_info(MS_U8 u8byte);
INTERFACE MS_U16 MDrv_HDMI_pll_ctrl2(HDMI_PLL_CTRL2_t pllctrl, MS_BOOL bread, MS_U16 u16value);
INTERFACE void  MDrv_DVI_IMMESWITCH_PS_SW_Path(void);


#undef INTERFACE
#endif // _DRV_ADC_INTERNAL_H_

