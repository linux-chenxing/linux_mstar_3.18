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

// [mhal_ip.h]

// Date: 20081203

// Descriptions: Add a new layer for HW setting

//==============================================================================

#ifndef MHAL_IP_H

#define MHAL_IP_H



#include "hwreg_sc.h"



//==============================================================================

//==============================================================================



#define MST_H_PERIOD_MASK           0x3FFF

#define MST_V_TOTAL_MASK            0x0FFF



//==============================================================================

//==============================================================================

#ifdef MHAL_IP_C

#define INTERFACE

#else

#define INTERFACE extern

#endif



//for BK01_0x03[4:3]

#define LEADING_EDGE    0

#define TAILING_EDGE    1



//for BK01_0x03[5]

#define DELAY_QUARTER_HSYNC 0

#define NO_DELAY            1



//for BK01_0x24[5]

#define PC_SOURCE           0

#define COMPONENT_VIDEO     1



//for BK01_0x26[2]

#define NORMAL_MODE     0

#define GLITCH_REMOVAL  1





INTERFACE MS_U16 Hal_SC_ip_de_hstart_info(SCALER_WIN eWindow);

INTERFACE MS_U16 Hal_SC_ip_de_hend_info(SCALER_WIN eWindow);



INTERFACE MS_U16 Hal_SC_ip_de_vstart_info(SCALER_WIN eWindow);

INTERFACE MS_U16 Hal_SC_ip_de_vend_info(SCALER_WIN eWindow);



INTERFACE MS_U8 Hal_SC_ip_get_sync_flag(SCALER_WIN eWindow);


#ifdef MULTI_SCALER_SUPPORTED
	INTERFACE MS_U8 Hal_SC_ip_get_input_vsync_polarity(SCALER_WIN eWindow);
#else
	INTERFACE MS_U8 Hal_SC_ip_get_input_vsync_polarity(void);
#endif


INTERFACE MS_U8 Hal_SC_ip_get_input_vsync_value(SCALER_WIN eWindow);

INTERFACE MS_U8 Hal_SC_ip_get_output_vsync_value(SCALER_WIN eWindow);



INTERFACE MS_U8 Hal_SC_ip_get_sync_detect_status(SCALER_WIN eWindow);



INTERFACE MS_U8 Hal_SC_ip_get_interlace_status (SCALER_WIN eWindow);

INTERFACE void Hal_SC_ip_set_ms_filter(MS_BOOL bEnable, MS_U16 u16FilterRange, SCALER_WIN eWindow );

INTERFACE void Hal_SC_ip_set_coast_window(MS_U8 u8Start, MS_U8 u8End, SCALER_WIN eWindow);



INTERFACE void Hal_SC_ip_sog_detect(void);





INTERFACE void Hal_SC_IPMux_Gen_SpecificTiming( XC_Internal_TimingType timingtype );  //scaler 480p timing setting

INTERFACE MS_U16  Hal_SC_ip_get_h_position(void);

INTERFACE MS_U16  Hal_SC_ip_get_v_position(void);

INTERFACE void Hal_SC_ip_get_capture_window(MS_WINDOW_TYPE* capture_win, SCALER_WIN eWindow);

INTERFACE void Hal_SC_ip_set_capture_v_start(MS_U16 u16Vstart , SCALER_WIN eWindow);

INTERFACE void Hal_SC_ip_set_capture_h_start(MS_U16 u16Hstart , SCALER_WIN eWindow);

INTERFACE void Hal_SC_ip_set_capture_v_size(MS_U16 u16Vsize , SCALER_WIN eWindow);

INTERFACE void Hal_SC_ip_set_capture_h_size(MS_U16 u16Hsize , SCALER_WIN eWindow);

INTERFACE void Hal_SC_ip_set_fir_down_sample_divider( MS_U8 u8Enable, MS_U16 u16OverSampleCount,SCALER_WIN eWindow);

INTERFACE void Hal_SC_ip_set_IPAutoNoSignal(MS_BOOL bEnable, SCALER_WIN eWindow );

INTERFACE MS_BOOL Hal_SC_ip_get_IPAutoNoSignal(SCALER_WIN eWindow);

INTERFACE void Hal_SC_ip_set_auto_gain_function( MS_U8 u8Enable, SCALER_WIN eWindow);

INTERFACE MS_BOOL Hal_SC_ip_is_auto_gain_result_ready(SCALER_WIN eWindow);

INTERFACE MS_U8 Hal_SC_ip_auto_gain_min_value_status(SCALER_WIN eWindow);

INTERFACE MS_U8 Hal_SC_ip_auto_gain_max_value_status(SCALER_WIN eWindow);

INTERFACE MS_BOOL Hal_SC_ip_is_auto_phase_result_ready(SCALER_WIN eWindow);

INTERFACE MS_U32 Hal_SC_ip_get_auto_phase_value(SCALER_WIN eWindow);

INTERFACE MS_BOOL Hal_SC_ip_is_auto_position_result_ready(SCALER_WIN eWindow);

INTERFACE void Hal_SC_ip_set_auto_position_function( MS_U8 u8Enable, SCALER_WIN eWindow);

INTERFACE void Hal_SC_ip_set_valid_data_threshold( MS_U8 u8Threshold, SCALER_WIN eWindow );

INTERFACE MS_U16 Hal_SC_ip_get_auto_position_v_start( SCALER_WIN eWindow );

INTERFACE MS_U16 Hal_SC_ip_get_auto_position_h_start( SCALER_WIN eWindow );

INTERFACE MS_U16 Hal_SC_ip_get_auto_position_v_end( SCALER_WIN eWindow );

INTERFACE MS_U16 Hal_SC_ip_get_auto_position_h_end( SCALER_WIN eWindow );

INTERFACE void Hal_SC_ip_set_auto_range_window(MS_U16 u16Vstart, MS_U16 u16Hstart, MS_U16 u16Vsize , MS_U16 u16Hsize, SCALER_WIN eWindow);

INTERFACE void Hal_SC_ip_set_auto_range_function( MS_U8 u8Enable, SCALER_WIN eWindow );

INTERFACE void Hal_SC_ip_set_reg_usr_vspolmd(MS_U8 u8Enable, SCALER_WIN eWindow );

INTERFACE void Hal_SC_ip_init_reg_in_timingchange(SCALER_WIN eWindow);



INTERFACE MS_BOOL HAL_SC_set_GOP_Enable(MS_U8 MaxGOP ,MS_U8 UseNum, MS_U8 u8MuxNum, MS_BOOL bEnable);

INTERFACE void HAL_SC_ip_sel_for_gop(MS_U8 u8MuxNum ,MS_XC_IPSEL_GOP ipSelGop);

INTERFACE void HAL_SC_SetVOPNBL(void);



void Hal_SC_ip_software_reset(MS_U8 u8Reset, SCALER_WIN eWindow);

void Hal_SC_ip_set_input_source(MS_U8 u8InputSrcSel, MS_U8 u8SyncSel, MS_U8 u8VideoSel, MS_U8 u8isYPbPr, SCALER_WIN eWindow);

MS_BOOL Hal_SC_Check_IP_Gen_Timing(void);

void hal_ip_set_input_10bit(MS_BOOL bInput10Bit, SCALER_WIN eWindow);

void Hal_SC_ip_set_image_wrap(MS_BOOL bHEnable, MS_BOOL bVEnable, SCALER_WIN eWindow);

void Hal_SC_ip_set_input_sync_reference_edge(MS_BOOL bHRef, MS_BOOL bVRef, SCALER_WIN eWindow);

void Hal_SC_ip_set_input_vsync_delay(MS_BOOL bDelay, SCALER_WIN eWindow);

void Hal_SC_ip_set_de_only_mode(MS_BOOL bEnable, SCALER_WIN eWindow);

void Hal_SC_ip_set_coast_input(MS_BOOL bInputSel, SCALER_WIN eWindow);

void Hal_SC_ip_set_DE_Mode_Glitch(MS_U8 u8Setting , SCALER_WIN eWindow );

void Hal_SC_ip_set_input_sync_sample_mode(MS_BOOL bMode, SCALER_WIN eWindow);

void Hal_SC_ip_set_post_glitch_removal(MS_BOOL bEnble, MS_U8 u8Range, SCALER_WIN eWindow);



MS_U8 HAL_SC_ip_get_ipmux(void);

void HAL_SC_ip_set_ipmux(MS_U8 u8Val);

MS_U8 HAL_SC_ip_get_ipclk(void);

void HAL_SC_ip_set_ipclk(MS_U8 u8Val);

MS_U8 HAL_SC_ip_get_capturesource(void);

void HAL_SC_ip_set_capturesource(MS_U8 u8Val);



void Hal_SC_set_skp_fd(MS_BOOL bEnable);

MS_U8 Hal_SC_ip_get_user_def_interlace_status(void);

void Hal_SC_ip_set_user_def_interlace_status(MS_BOOL bEnable, MS_BOOL bIsInterlace);

MS_U8 HAL_SC_ip_get_capture_method(void);

MS_BOOL HAL_SC_ip_get_capture_format(void);

void HAL_SC_ip_set_capture_format(MS_BOOL bIsRGB);





MS_BOOL HAL_SC_ip_get_h_predown_info(void);

MS_BOOL HAL_SC_ip_get_v_predown_info(void);

void HAL_SC_ip_set_h_predown_info(MS_BOOL bEnable);

void HAL_SC_ip_set_v_predown_info(MS_BOOL bEnable);



MS_BOOL HAL_SC_ip_get_444to422_filter_mod(void);

void HAL_SC_ip_set_444to422_filter_mod(MS_BOOL bEnable);



MS_U8 HAL_SC_get_memory_fmt(void);

MS_U8 HAL_SC_get_IPM_memory_fmt(void);

void HAL_SC_set_IPM_memory_fmt(MS_U8 u8Value);

MS_U8 HAL_SC_get_OPM_memory_fmt(void);

void HAL_SC_set_OPM_memory_fmt(MS_U8 u8Value);

void HAL_SC_set_capture_image2ip(MS_BOOL bEnable);

MS_U8 HAL_SC_get_VOP_mux(void);

void HAL_SC_set_VOP_mux(MS_U8 u8Val);

void Hal_SC_Set_Cap_Stage(SCALER_WIN eWindow, MS_U8 u8val);

MS_U8 HAL_SC_get_memory_bit_fmt(SCALER_WIN eWindow);

void HAL_SC_set_memory_bit_fmt(MS_BOOL bEnable_8bit, MS_BOOL bEnable_10bit);

void HAL_SC_set_IPM_capture_start(MS_BOOL bEnable);

MS_U32 HAL_SC_get_IRQ_mask_0_31(void);

void HAL_SC_set_IRQ_mask_0_31(MS_U32 u32Val);



void Hal_SC_ip_set_handshaking_md(MS_U8 u8MD, SCALER_WIN eWindow);

void Hal_SC_ip_Set_TestPattern(MS_U8 u8Enable, MS_U16 u6Pattern_type, SCALER_WIN eWindow);

void HAL_SC_ip_3DMainSub_IPSync(void);

void HAL_SC_ip_detect_mode(MS_BOOL bEnable);

MS_U8 Hal_SC_ip_get_h_v_sync_active(SCALER_WIN eWindow);



#undef INTERFACE

#endif /* MHAL_IP_H */



