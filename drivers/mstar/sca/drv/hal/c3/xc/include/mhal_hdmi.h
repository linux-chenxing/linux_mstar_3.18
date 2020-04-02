////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (!¡±MStar Confidential Information!¡L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
// [mhal_hdmi.h]
// Date: 20081203
// Descriptions: Add a new layer for HW setting
//==============================================================================
#ifndef MHAL_HDMI_H       
#define MHAL_HDMI_H

#include "hwreg_ddc.h"
#include "hwreg_adc_atop.h"
#include "hwreg_adc_dtop.h"
#include "hwreg_hdcp.h"
#include "hwreg_hdmi.h"
#include "hwreg_sc.h"
#include "hwreg_sc1.h"
#include "hwreg_sc2.h"

#include "drvXC_HDMI_if.h"
#include "apiXC.h"



//==============================================================================
//==============================================================================
/* DDC SRAM SEL (After T3) */
#define     DDC_RAM_SRAM_DVI    0
#define     DDC_RAM_SRAM_ADC	1
#define     DDC_RAM_SRAM_DVI0   0
#define     DDC_RAM_SRAM_DVI1   1
#define     DDC_RAM_SRAM_DVI2   2
#define     DDC_RAM_SRAM_DVI3   3

//==============================================================================
//==============================================================================

#define     IRQ_DVI_CK_CHG            BIT(0)      //#[0]
#define     IRQ_HDMI_MODE_CHG    BIT(1)    //#[1]
#define     IRQ_SCART_ID0_CHG      BIT(2)     //#[2]
#define     IRQ_SCART_ID1_CHG      BIT(3)     //#[3]
#define     IRQ_SAR_DET_UPD         BIT(4)        //#[4]
#define     IRQ_RESERVE                 (BIT(7)|BIT(6)|BIT(5))                 //#[5:7]
#define     IRQ_ALL_BIT                   (BIT(4)|BIT(3)|BIT(2)|BIT(1)|BIT(0))

//==============================================================================
//==============================================================================
#ifdef MHAL_HDMI_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE MS_U16 Hal_HDMI_Func_Caps(void);
////--------------> for HDCP
//INTERFACE void Hal_HDCP_clearflag( void );
INTERFACE void Hal_HDCP_ddc_en(E_MUX_INPUTPORT enInputPortType, MS_BOOL bEnable);
INTERFACE void Hal_HDCP_Vsync_end_en( MS_BOOL bEnalbe );

////--------------> for HDMI
INTERFACE void Hal_HDMI_pullhpd( MS_BOOL bHighLow, E_MUX_INPUTPORT enInputPortType, MS_BOOL bInverse);
INTERFACE void Hal_HDMI_init( void );
INTERFACE void Hal_HDMI_exit(E_MUX_INPUTPORT enInputPortType);

INTERFACE void Hal_HDMI_Set_EQ(E_MUX_INPUTPORT enInputPortType, MS_HDMI_EQ enEq, MS_U8 u8EQValue);

INTERFACE MS_U16 Hal_HDMI_gcontrol_info(HDMI_GControl_INFO_t gcontrol);
INTERFACE MS_U16 Hal_HDMI_pll_ctrl1(HDMI_PLL_CTRL_t pllctrl, MS_BOOL bread, MS_U16 u16value);
INTERFACE MS_U16 Hal_HDMI_pll_ctrl2(HDMI_PLL_CTRL2_t pllctrl, MS_BOOL bread, MS_U16 u16value);
INTERFACE MS_U16 Hal_HDMI_GetHDE(void);

////--------------> for HDMI Audio
INTERFACE void Hal_HDMI_audio_output(MS_BOOL bEnable);

////--------------> for DVI
INTERFACE MS_BOOL Hal_DVI_clk_lessthan70mhz_usingportc( MS_BOOL bIsPortC);
INTERFACE MS_U16 Hal_DVI_ChannelPhaseStatus( MS_U8 u8Channel);
INTERFACE MS_BOOL Hal_DVI_clk_stable_usingportc(MS_BOOL bIsPortC);
INTERFACE MS_BOOL Hal_DVI_clklose_det(E_MUX_INPUTPORT enInputPortType);
INTERFACE void Hal_DVI_IMMESWITCH_PS_SW_Path(void);
INTERFACE void Hal_DVI_sw_reset(MS_U16 u16Reset);
INTERFACE MS_U16 Hal_DVI_irq_info(E_MUX_INPUTPORT enInputPortType, MS_U16 irqbit);
INTERFACE void Hal_DVI_irq_mask(E_MUX_INPUTPORT enInputPortType, MS_U16 irqbit);
INTERFACE void Hal_DVI_irq_forcemode(E_MUX_INPUTPORT enInputPortType, MS_U16 irqbit);
INTERFACE void Hal_DVI_irq_clear(E_MUX_INPUTPORT enInputPortType, MS_U16 irqbit);
INTERFACE void Hal_DVI_ClkPullLow(MS_BOOL bPullLow, E_MUX_INPUTPORT enInputPortType);
INTERFACE void Hal_DVI_SwitchSrc(E_MUX_INPUTPORT enInputPortType);

INTERFACE MS_U8 Hal_HDMI_err_status_update(MS_U8 u8value, MS_BOOL bread);
INTERFACE void Hal_HDMI_pkt_reset(E_MUX_INPUTPORT enInputPortType, HDMI_REST_t breset);
INTERFACE void Hal_HDMI_hpd_en(MS_BOOL benable);
INTERFACE void Hal_HDCP_initproductionkey( MS_U8 * pu8HdcpKeyData );
INTERFACE void Hal_HDCP_clearflag(E_MUX_INPUTPORT enInputPortType);
INTERFACE void Hal_HDMI_audio_output( MS_BOOL bEnable );
INTERFACE MS_U16 Hal_HDMI_packet_info(void);
INTERFACE MS_U8 Hal_HDMI_audio_content_protection_info(void);
INTERFACE MS_U8 Hal_HDMI_avi_infoframe_info(MS_U8 u8byte);
INTERFACE MS_BOOL Hal_HDMI_get_packet_value(MS_HDMI_PACKET_STATE_t u8state, MS_U8 u8byte_idx, MS_U8 *pu8Value);

INTERFACE MS_U8 Hal_HDMI_audio_channel_status(MS_U8 u8byte);
INTERFACE MS_U16 Hal_HDCP_getstatus(E_MUX_INPUTPORT enInputPortType);
INTERFACE void Hal_DVI_adc_adjust( MS_BOOL bClockLessThan70MHz);

INTERFACE void Hal_HDMI_packet_ctrl(MS_U16 u16pkt);
INTERFACE MS_U8 Hal_HDMI_audio_payload_info(MS_U8 u8byte);

// DDC
INTERFACE void HAL_HDMI_DDCRam_SelectPort(E_XC_DDCRAM_PROG_TYPE eDDCRamType);
INTERFACE void HAL_HDMI_DDCRAM_SetPhyAddr(XC_DDCRAM_PROG_INFO *pstDDCRam_Info);


// HDMI 1.4 new feature:
INTERFACE E_HDMI_ADDITIONAL_VIDEO_FORMAT Hal_HDMI_Check_Additional_Format(void);
INTERFACE E_XC_3D_INPUT_MODE Hal_HDMI_Get_3D_Structure(void);
INTERFACE E_HDMI_3D_EXT_DATA_T Hal_HDMI_Get_3D_Ext_Data(void);
INTERFACE void Hal_HDMI_Get_3D_Meta_Field(sHDMI_3D_META_FIELD *pdata);
INTERFACE MS_U8 Hal_HDMI_Get_VIC_Code(void);
INTERFACE E_HDMI_VIC_4Kx2K_CODE Hal_HDMI_Get_4Kx2K_VIC_Code(void);
INTERFACE void Hal_HDMI_Set_YUV422to444_Bypass(MS_BOOL btrue);
INTERFACE MS_BOOL Hal_HDMI_Is_Input_Large_166MHz(void);
INTERFACE void Hal_HDMI_AVG_ScaleringDown(MS_BOOL btrue);
INTERFACE void Hal_HDMI_ARC_PINControl(E_MUX_INPUTPORT enInputPortType, MS_BOOL bEnable, MS_BOOL bDrivingHigh);

MS_BOOL Hal_DVI_GetPowerSavingStatus(E_MUX_INPUTPORT enInputPortType);
MS_BOOL Hal_DVI_GetDEStableStatus(E_MUX_INPUTPORT enInputPortType);
void Hal_DVI_EnhanceImmeswitch(E_MUX_INPUTPORT enInputPortType, MS_BOOL bflag);
void Hal_DVI_ForceAllPortsEnterPS(void);

#undef INTERFACE
#endif /* MHAL_HDMI_H */


