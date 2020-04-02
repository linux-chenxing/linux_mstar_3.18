////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (!!¡ÓMStar Confidential Information!!L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
// [mhal_adc.h]
// Date: 20081203
// Descriptions: Add a new layer for HW setting
//==============================================================================
#ifndef MHAL_ADC_H
#define MHAL_ADC_H


//#include "hwreg_utility.h"
#include "MsCommon.h"

///////////////////////////////////////////////
// Enable Hardware auto offset
#define ADC_HARDWARE_AUTOOFFSET_RGB   		ENABLE
#define ADC_HARDWARE_AUTOOFFSET_YPBPR 		ENABLE
#define ADC_HARDWARE_AUTOOFFSET_SCARTRGB 	ENABLE

#define ADC_SOG_FILTER_THRSHLD      40

#define ADC_BANDWIDTH_CLK_THRSHLD0    6 // 8/0.75
#define ADC_BANDWIDTH_CLK_THRSHLD1    14 // 8/0.75
#define ADC_BANDWIDTH_CLK_THRSHLD2    15 // 8/0.75
#define ADC_BANDWIDTH_CLK_THRSHLD3    16 // 8/0.75
#define ADC_BANDWIDTH_CLK_THRSHLD4    17 // 8/0.75
#define ADC_BANDWIDTH_CLK_THRSHLD5    18 // 8/0.75
#define ADC_BANDWIDTH_CLK_THRSHLD6    20 // 8/0.75
#define ADC_BANDWIDTH_CLK_THRSHLD7    21 // 8/0.75
#define ADC_BANDWIDTH_CLK_THRSHLD8    23 // 8/0.75
#define ADC_BANDWIDTH_CLK_THRSHLD9    25 // 20/0.75
#define ADC_BANDWIDTH_CLK_THRSHLD10    30 // 50/0.75
#define ADC_BANDWIDTH_CLK_THRSHLD11    60 // 100/0.75
#define ADC_BANDWIDTH_CLK_THRSHLD12    120 // 200/0.75
#define ADC_BANDWIDTH_CLK_THRSHLD13    150 // 300/0.75
#define ADC_BANDWIDTH_CLK_THRSHLD14    190 // 400/0.75
#define ADC_BANDWIDTH_CLK_THRSHLD15    260 // 500/0.75



/******************************************************************************/
/*                   Function Prototypes                     */
/******************************************************************************/

void Hal_ADC_set_cvbs_out(E_ADC_CVBSOUT_TYPE e_cvbs_out_type);
MS_BOOL Hal_ADC_is_cvbs_out_enabled(E_MUX_OUTPUTPORT outputPort);
void Hal_ADC_clk_gen_setting(ADC_Gen_Clock_Type clocktype);
void Hal_ADC_set_mux(E_MUX_INPUTPORT port);


void Hal_ADC_gainoffset_reset(void);
void Hal_ADC_offset_setting(XC_AdcGainOffsetSetting *pstADCSetting);
void Hal_ADC_gain_setting(XC_AdcGainOffsetSetting *pstADCSetting);
void Hal_ADC_ExitExternalCalibration(ADC_INPUTSOURCE_TYPE eADC_Source,XC_AdcGainOffsetSetting* InitialGainOffset);
void Hal_ADC_InitExternalCalibration(ADC_INPUTSOURCE_TYPE eADC_Source,XC_AdcGainOffsetSetting* InitialGainOffset);
void Hal_ADC_InitInternalCalibration(ADC_INPUTSOURCE_TYPE eADC_Source,XC_AdcGainOffsetSetting* InitialGainOffset);
void Hal_ADC_ext_clk_en(MS_BOOL benable);
void Hal_ADC_hdmi_vco_ctrl(MS_U16 u16InputClock);
void Hal_ADC_vco_ctrl(MS_U16 u16InputClock);
void Hal_XC_ADC_Set_VClamp_level(ADC_VClamp_Level_Type type);
void Hal_ADC_set_vco_ctrl(MS_BOOL bIsDVIPort, MS_U16 u16InputClock);


void Hal_ADC_sog_filter_en(MS_BOOL bEnable);
void Hal_ADC_reset(MS_U16 u16Reset);

void Hal_ADC_dtop_internaldc_setting(ADC_Internal_Voltage InternalVoltage);

//  RGB Gain setting
void Hal_ADC_dtop_gain_r_setting(MS_U16 u16value);
void Hal_ADC_dtop_gain_g_setting(MS_U16 u16value);
void Hal_ADC_dtop_gain_b_setting(MS_U16 u16value);

//  RGB Offset setting
void Hal_ADC_dtop_offset_r_setting(MS_U16 u16value);
void Hal_ADC_dtop_offset_g_setting(MS_U16 u16value);
void Hal_ADC_dtop_offset_b_setting(MS_U16 u16value);

void Hal_ADC_get_default_gain_offset(ADC_INPUTSOURCE_TYPE adc_src,XC_AdcGainOffsetSetting* ADCSetting);
MS_U16 Hal_ADC_get_center_gain(void);
MS_U16 Hal_ADC_get_center_offset(void);
MS_U8 Hal_ADC_get_offset_bit_cnt(void);
MS_U8 Hal_ADC_get_gain_bit_cnt(void);

void Hal_ADC_dtop_calibration_target_setting(MS_BOOL bIsYPbPrFlag);
void Hal_ADC_dtop_sw_mode_setting(MS_BOOL bEnable, MS_BOOL bIsAutoSWMode);

void Hal_ADC_reset(MS_U16 u16Reset);
void Hal_ADC_Set_Source(ADC_INPUTSOURCE_TYPE enADC_SourceType, E_MUX_INPUTPORT* enInputPortType, MS_U8 u8PortCount);
void Hal_ADC_dtop_clk_setting ( MS_U16 u16Value );
void Hal_ADC_hpolarity_setting(MS_BOOL bHightActive);
void Hal_ADC_set_phase(MS_U8 u8Value);
void Hal_ADC_set_phaseEx(MS_U16 u16Value);
MS_U16 Hal_ADC_get_phase_range(void);
MS_U8 Hal_ADC_get_phase(void);
MS_U16 Hal_ADC_get_phaseEx(void);
void Hal_ADC_Set_Source_Calibration(ADC_INPUTSOURCE_TYPE enADC_SourceType);
void Hal_ADC_clamp_placement_setting(MS_U16 u16InputClockMHz);
void Hal_ADC_dvi_setting(void);
void Hal_ADC_poweroff(void);
void Hal_XC_ADC_poweron_source(ADC_INPUTSOURCE_TYPE enADC_SourceType);

void Hal_ADC_set_mode(ADC_INPUTSOURCE_TYPE enADCInput, MS_U16 u16PixelClockPerSecond, MS_U16 u16HorizontalTotal, MS_U16 u16SamplingRatio);
void Hal_ADC_SourceSwitch(MS_BOOL bSwitch);

void Hal_ADC_auto_adc_backup(void);
void Hal_ADC_auto_adc_restore(void);

MS_BOOL Hal_ADC_is_scart_rgb(void);
MS_U16 Hal_ADC_get_clk (void);
void Hal_ADC_get_SoG_LevelRange(MS_U32 *u32Min, MS_U32 *u32Max, MS_U32 *u32Recommend_value);
void Hal_ADC_set_SoG_Level(MS_U32 u32Value);
MS_BOOL Hal_ADC_set_SoG_Calibration(void);

void Hal_ADC_set_RGB_PIPE_Delay(MS_U8 u8Value);
void Hal_ADC_set_ScartRGB_SOG_ClampDelay(MS_U16 u16Clpdly, MS_U16 u16Caldur);
void Hal_ADC_set_YPbPrLooseLPF(MS_BOOL benable);
void Hal_ADC_Set_SOGBW(MS_U16 u16value);

void Hal_ADC_dtop_iClampDuration_setting(MS_U16 u16value);
void Hal_ADC_dtop_vClampDuration_setting(MS_U16 u16value);

#define HW_CALIBRATION_SUPPORTED TRUE
void Hal_ADC_enable_HWCalibration(MS_BOOL benable);
void Hal_ADC_get_fixed_gain_r(ADC_INPUTSOURCE_TYPE eADC_Source, MS_U16 *u16value);
void Hal_ADC_get_fixed_gain_g(ADC_INPUTSOURCE_TYPE eADC_Source, MS_U16 *u16value);
void Hal_ADC_get_fixed_gain_b(ADC_INPUTSOURCE_TYPE eADC_Source, MS_U16 *u16value);

#endif // MHAL_ADC_H

