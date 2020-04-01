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
//==============================================================================
// [mhal_sc.h]
// Date: 20081203
// Descriptions: Add a new layer for HW setting
//==============================================================================
#ifndef MHAL_SC_H
#define MHAL_SC_H

#include "hwreg_sc.h"
#include "hwreg_sc1.h"
#include "hwreg_sc2.h"
#include "hwreg_mod.h"

//==============================================================================
// Scaling Ratio Macro

// H_PreScalingDownRatio() was refined to reduce the calculation error.
// Use round up (x+y/2)/y might reduce the scaling ratio and induce right vertical garbage line.
// So use un-conditional add by 1 (x+y)/y.
#define H_PreScalingDownRatio(Input, Output)                ( (((MS_U32)(Output)) * 1048576ul)/ (Input) + 1 )
#define H_PreScalingDownRatioAdv(Input, Output)             ( (((MS_U32)(Input)-1) * 1048576ul)/ ((Output)-1) ) //Advance scaling
#define H_PreScalingDownRatioAdv_No_Minus1(Input, Output)   ( (((MS_U32)(Input)) * 1048576ul)/ ((Output)) )     //Advance scaling without minus 1
#define V_PreScalingDownRatio(Input, Output)                ( (((MS_U32)(Output)) * 1048576ul)/ (Input) + 1 ) // CB mode
#define V_PreScalingDownRatioBilinear(Input, Output)        ( (((MS_U32)(Input)-1) * 1048576ul)/ ((Output) -1)) // Bilinear

#define H_PostScalingRatio(Input, Output)        ( ((MS_U32)(Input)) * 1048576ul / (Output) + 1 )
//#define H_PostScalingRatioAdv(Input, Output)     ( ((MS_U32)(Input)-1) * 1048576ul / ((Output)-1) + 1 )
#define V_PostScalingRatio(Input, Output)        ( ((MS_U32)(Input)-1) * 1048576ul / ((Output)-1) + 1 )

// In MDrv_SC_3D_Adjust_PreHorDstSize(),
// do a rough check after all, for 2 frame case.
#define ENABLE_2_FRAME_SIZE_PROTECTION  TRUE

typedef struct
{
    MS_U32 u32IPMBase0;
    MS_U32 u32IPMBase1;
    MS_U32 u32IPMBase2;
    MS_U16 u16IPMOffset;
    MS_U16 u16IPMFetch;
    MS_U16 u16VLength;
    MS_U32 u32WriteLimitBase;

    MS_BOOL bLinearAddrMode;
    MS_BOOL bYCSeparate;

    MS_BOOL bMemFormat422;
    MS_BOOL bInterlace;
    MS_U8 u8BitPerPixel;
    MS_U8 u8FBNum;
} SC_FRAMEBUF_INFO_t;

typedef struct
{
    MS_U16 u16MiuG0Mask;
    MS_U16 u16MiuG1Mask;
    MS_U16 u16MiuG2Mask;
    MS_U16 u16MiuG3Mask;
} SC_MIUMASK_t;

typedef enum
{
    E_XC_FPLL_DIR_UNKNOWN,
    E_XC_FPLL_DIR_UP,
    E_XC_FPLL_DIR_DOWN,
} MS_XC_FPLL_DIRECTION;

typedef enum
{
    E_XC_FPLL_RES_WAITING,
    E_XC_FPLL_RES_TIMEOUT,
    E_XC_FPLL_RES_FINISHED,
} MS_XC_FPLL_RESULT;

typedef struct
{
    MS_U8  u8Debounce;
    MS_U16 u16PhaseDiff;
    MS_XC_FPLL_DIRECTION eFpllDir;
    MS_XC_FPLL_RESULT eFpllResult;
} MS_XC_GET_FPLL_PHASEDIFF;

typedef enum
{
    HDMI_OUT_480_I,
    HDMI_OUT_576_I,
    HDMI_OUT_480_P,
    HDMI_OUT_720_P,
    HDMI_OUT_1080_I,
    HDMI_OUT_1080_P,
    HDMI_OUT_NONE,
}MS_HDMIT_OUTCLK_SRC_TYPE;
//==============================================================================
//==============================================================================
#ifdef MHAL_SC_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE void Hal_SC_setfield(MS_U16 reg_1D, MS_U16 reg_21, MS_U16 reg_23, SCALER_WIN eWindow);

INTERFACE void Hal_SC_set_ficlk(MS_BOOL bPreDown, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_shiftline(MS_U8 u8Val, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_422_cbcr_swap(MS_BOOL bEnable, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_pre_align_pixel(MS_BOOL bEnable, MS_U16 pixels, SCALER_WIN eWindow);
INTERFACE void Hal_XC_Set_FreeFRCMD(MS_BOOL bEnable);
#ifdef MSOS_TYPE_CE
#define Hal_SC_force3fb(...)
#define Hal_SC_force4fb(...)
#define Hal_SC_force8fb(...)
#else
#define Hal_SC_force3fb(args...)
#define Hal_SC_force4fb(args...)
#define Hal_SC_force8fb(args...)
#endif
INTERFACE XC_FRAME_STORE_NUMBER Hal_SC_GetFrameStoreMode(SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_csc(MS_BOOL bEnable, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_wr_bank_mapping(MS_U8 u8val, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_delayline(MS_U8 u8DelayLines, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_write_limit(MS_U32 u32WritelimitAddrBase, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_opm_write_limit(MS_BOOL bEnable, MS_BOOL bFlag, MS_U32 u32OPWlimitAddr, SCALER_WIN eWindow);
INTERFACE void Hal_SC_sw_db(P_SC_SWDB_INFO pDBreg, SCALER_WIN eWindow);
INTERFACE void Hal_SC_sw_db_burst(P_SC_SWDB_INFO pDBreg, SCALER_WIN eWindow);
INTERFACE E_APIXC_ReturnValue Hal_SC_support_source_to_ve(MS_U16* OutputCapability);
INTERFACE E_APIXC_ReturnValue Hal_SC_set_output_capture_enable(MS_BOOL bEnable,E_XC_SOURCE_TO_VE eSourceToVE);
INTERFACE E_APIXC_ReturnValue Hal_SC_get_Htt_Vtt(MS_U16 *u16Htt, MS_U16 *u16Vtt, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_de_window(XC_PANEL_INFO *pPanel_Info, SCALER_WIN eWindow);
INTERFACE void Hal_SC_get_disp_de_window(MS_WINDOW_TYPE *pWin, SCALER_WIN eWindow);
INTERFACE MS_U32 Hal_SC_Get_DNRBase0(SCALER_WIN eWindow);
INTERFACE MS_U32 Hal_SC_Get_DNRBase1(SCALER_WIN eWindow);
INTERFACE MS_U32 Hal_SC_Get_OPMBase0(SCALER_WIN eWindow);
INTERFACE MS_U32 Hal_SC_Get_OPMBase1(SCALER_WIN eWindow);
INTERFACE MS_U32 Hal_SC_Get_OPMBase2(SCALER_WIN eWindow);
INTERFACE MS_U8  Hal_SC_Get_LBOffset(SCALER_WIN eWindow);
INTERFACE void   Hal_SC_Set_LBOffset(MS_U8 u8LBOffset, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_DNRBase0(MS_U32 u32DNRBase0, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_DNRBase1(MS_U32 u32DNRBase1, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_DNRBase2(MS_U32 u32DNRBase2, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_OPMBase0(MS_U32 u32OPMBase0, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_OPMBase1(MS_U32 u32OPMBase1, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_OPMBase2(MS_U32 u32OPMBase2, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_memoryaddress( MS_U32 u32DNRBase0, MS_U32 u32DNRBase1, MS_U32 u32DNRBase2, MS_U32 u32OPMBase0, MS_U32 u32OPMBase1, MS_U32 u32OPMBase2, SCALER_WIN eWindow);
INTERFACE void Hal_SC_Enable_MiuMask(void);
INTERFACE void Hal_SC_Disable_MiuMask(void);
INTERFACE void Hal_SC_set_miusel(MS_U8 u8MIUSel);

INTERFACE void Hal_SC_set_blsk(MS_BOOL bEn, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_blsk_burst(MS_BOOL bEn, SCALER_WIN eWindow);

INTERFACE void Hal_SC_set_main_black_screen_burst(MS_BOOL bEn, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_main_sub_black_screen_burst(MS_BOOL bEn, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_sub_blue_screen_burst(MS_BOOL bEn, MS_BOOL bBlue, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_main_black_screen(MS_BOOL bEn, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_main_sub_black_screen(MS_BOOL bEn, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_sub_blue_screen(MS_BOOL bEn, MS_BOOL bBlue, SCALER_WIN eWindow);
INTERFACE E_APIXC_ReturnValue Hal_SC_memory_write_request(MS_BOOL bEnable, SCALER_WIN eWindow);
INTERFACE E_APIXC_ReturnValue Hal_SC_memory_read_request(MS_BOOL bEnable, SCALER_WIN eWindow);
INTERFACE void Hal_SC_disable_inputsource_burst(MS_BOOL bDisable, SCALER_WIN eWindow);
INTERFACE void Hal_SC_disable_inputsource(MS_BOOL bDisable, SCALER_WIN eWindow);
INTERFACE MS_U16 Hal_SC_Is_InputSource_Disable(SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_nosignal_color(MS_U8 u8Color,SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_fbl(MS_BOOL bEn, SCALER_WIN eWindow);
INTERFACE MS_BOOL Hal_SC_get_fbl(SCALER_WIN eWindow);
INTERFACE void HAL_SC_Set_FB_Num(SCALER_WIN eWindow, XC_FRAME_STORE_NUMBER eBestFBNum, MS_BOOL bInterlace);
INTERFACE void Hal_SC_set_freezeimg_burst(MS_BOOL bEn, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_freezeimg(MS_BOOL bEn, SCALER_WIN eWindow);
INTERFACE MS_BOOL Hal_SC_get_freezeimg(SCALER_WIN eWindow);
INTERFACE void Hal_SC_ip_Init_for_internal_timing(XC_Internal_TimingType timingtype);
INTERFACE MS_U16 Hal_SC_rgb_average_info(MS_U16 u16mask, SCALER_WIN eWindow);
INTERFACE void Hal_SC_autogain_enable(MS_BOOL bEnable, SCALER_WIN eWindow);
INTERFACE MS_BOOL Hal_SC_autogain_status(SCALER_WIN eWindow);
INTERFACE void Hal_SC_init_riu_base(MS_U32 u32riu_base, MS_U32 u32PMriu_base);
INTERFACE void Hal_SC_set_mirror(MS_BOOL bEnable, SCALER_WIN eWindow);
INTERFACE MS_U16 Hal_SC_GetAVDStatus(void);
INTERFACE void Hal_SC_set_rep_window(MS_BOOL bEnable,MS_U16 x,MS_U16 y,MS_U16 w,MS_U16 h,MS_U8 u8Color, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_disp_window(SCALER_WIN eWindow, MS_WINDOW_TYPE *pdspwin);
INTERFACE void Hal_SC_get_disp_window(SCALER_WIN eWindow, MS_WINDOW_TYPE *pdspwin);
INTERFACE void Hal_SC_set_Fclk(EN_SET_FCLK_CASE enCase);
INTERFACE void Hal_SC_get_framebuf_Info(SC_FRAMEBUF_INFO_t *pFrameBufInfo);
INTERFACE void Hal_SC_set_framebuf_Info(SC_FRAMEBUF_INFO_t pFrameBufInfo);
INTERFACE MS_U8 HAL_SC_get_dnr_setting(SCALER_WIN eWindow);
INTERFACE void HAL_SC_enable_dnr(MS_U8 u8Val, SCALER_WIN eWindow);
INTERFACE void HAL_SC_enable_field_avg_y(MS_BOOL bEnable, SCALER_WIN eWindow);
INTERFACE void HAL_SC_enable_field_avg_c(MS_BOOL bEnable, SCALER_WIN eWindow);
INTERFACE MS_U8 Hal_SC_get_cs_det_cnt(SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_linearmem_mode(MS_BOOL bEnable, SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_cs_det_cnt(MS_U8 u8val, SCALER_WIN eWindow);
INTERFACE MS_U8 Hal_SC_get_plus_width(SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_opm_fetch(SCALER_WIN eWindow, MS_U16 u16OPMFetch);
INTERFACE MS_U16 Hal_SC_get_opm_fetch(SCALER_WIN eWindow);

INTERFACE void HAL_SC_VOP_Set_Contrast_En(MS_BOOL bEenable, SCALER_WIN eWindow);
INTERFACE void HAL_SC_VOP_Set_Contrast_Value(MS_XC_VOP_CHANNEL_t eVop_Channel, MS_U16 u16Val, SCALER_WIN eWindow);
INTERFACE void HAL_SC_VOP_Set_Brightness_En(MS_BOOL bEenable, SCALER_WIN eWindow);
INTERFACE void HAL_SC_VOP_Set_Brightness_Value(MS_XC_VOP_CHANNEL_t eVop_Channel, MS_U16 u16Val, SCALER_WIN eWindow);

INTERFACE MS_BOOL HAL_SC_Set_DynamicScaling(MS_U32 u32MemBaseAddr, MS_U8 u8MIU_Select, MS_U8 u8IdxDepth, MS_BOOL bOP_On, MS_BOOL bIPS_On, MS_BOOL bIPM_On);
INTERFACE MS_BOOL HAL_SC_Get_DynamicScaling_Status(void);
INTERFACE void HAL_SC_Enable_VInitFactor(MS_BOOL bEnable,SCALER_WIN eWindow);
INTERFACE void HAL_SC_Set_VInitFactorOne(MS_U32 u32Value,SCALER_WIN eWindow);
INTERFACE void HAL_SC_Set_VInitFactorTwo(MS_U32 u32Value,SCALER_WIN eWindow);
INTERFACE void HAL_SC_Set_vsd_input_line_count(MS_BOOL bEnable,MS_BOOL bUserMode,MS_U32 u32UserLineCount,SCALER_WIN eWindow);
INTERFACE void HAL_SC_Set_vsd_output_line_count(MS_BOOL bEnable,MS_U32 u32LineCount,SCALER_WIN eWindow);
INTERFACE void HAL_SC_EnableFPLL(void);
INTERFACE MS_BOOL HAL_SC_WaitFPLLDone(void);
INTERFACE MS_U16 HAL_SC_GetOutputVFreqX100(MS_U32 u32XTAL_Clock);
INTERFACE MS_BOOL Hal_SC_get_csc(SCALER_WIN eWindow);

//Add for T3
INTERFACE void HAL_SC_FilmMode_Patch1(void);

INTERFACE void HAL_SC_SetSubLineBufferOffset(MS_U16 u16Linebuffer, SCALER_WIN eWindow);
INTERFACE void HAL_SC_SetDisplay_LineBuffer_Mode(MS_BOOL bEnable, SCALER_WIN eWindow);
INTERFACE void HAL_SC_FillLineBuffer(MS_BOOL bEnable, SCALER_WIN eWindow);
INTERFACE void Hal_SC_enable_window(MS_BOOL bEn,SCALER_WIN eWindow);
INTERFACE void Hal_SC_enable_window_burst(MS_BOOL bEn,SCALER_WIN eWindow);
INTERFACE void Hal_SC_set_trigger_signal(void);
INTERFACE void Hal_SC_Set_extra_fetch_line(MS_U8 u8val, SCALER_WIN eWindow);
INTERFACE void Hal_SC_Set_extra_adv_line(MS_U8 u8val, SCALER_WIN eWindow);
INTERFACE void Hal_SC_SetOPWriteOff(MS_BOOL bEna);
INTERFACE MS_BOOL Hal_SC_GetOPWriteOff(void);
INTERFACE MS_BOOL Hal_SC_get_pixel_rgb(XC_Get_Pixel_RGB *pData);

// FPLL
void HAL_SC_Set_FPLL_Limit(MS_U32 *u32PllSet, MS_U32 u32LowBound, MS_U32 u32UpBound, MS_BOOL _bInFPLLDbgMode, MS_U32 _U32LimitD5D6D7);

//Add for U4
INTERFACE void Hal_SC_Set_OSD2VE(EN_VOP_SEL_OSD_XC2VE_MUX  eVOPSelOSD_MUX);
//Add for T4/Janus
INTERFACE MS_BOOL Hal_SC_IsOPMFetchPatch_Enable(void);
INTERFACE MS_BOOL Hal_SC_Check_HNonLinearScaling(void);

//Add for T8 H3D reg handle
INTERFACE void Hal_XC_H3D_Enable(MS_BOOL bEn);
INTERFACE void Hal_XC_H3D_Input3DType(E_XC_3D_INPUT_MODE e3DType);
INTERFACE void Hal_XC_H3D_Breakline_Enable(MS_BOOL bEn);
INTERFACE void Hal_XC_H3D_HDE(MS_U16 u16Hde);
INTERFACE void Hal_XC_H3D_VDE_F0(MS_U16 u16Vde);
INTERFACE void Hal_XC_H3D_VDE_F2(MS_U16 u16Vde);
INTERFACE void Hal_XC_H3D_HBLANK(MS_U16 u16Hblank);
INTERFACE void Hal_XC_H3D_INIT_VBLANK(MS_U8 u8Vblank);
INTERFACE void Hal_XC_H3D_VBLANK0(MS_U8 u8Vblank);
INTERFACE void Hal_XC_H3D_VBLANK1(MS_U8 u8Vblank);
INTERFACE void Hal_XC_H3D_VBLANK2(MS_U8 u8Vblank);
INTERFACE void Hal_XC_H3D_VSYNC_WIDTH(MS_U8 u8Width);
INTERFACE void Hal_XC_H3D_VSYNC_POSITION(MS_U16 u16Position);
INTERFACE void Hal_XC_H3D_SELECT_REGEN_TIMING(MS_BOOL bEn);
INTERFACE void Hal_XC_H3D_LR_Toggle_Enable(MS_BOOL bEn);
INTERFACE void HAL_XC_H3D_OPM_SBYS_PIP_Enable(MS_BOOL bEn);

INTERFACE MS_BOOL Hal_SC_Detect_RequestFBL_Mode(void);
INTERFACE void Hal_XC_SetFrameColor(MS_U32 u32aRGB, SCALER_WIN eWindow);
INTERFACE void Hal_XC_SetFrameColor_En(MS_BOOL bEn, SCALER_WIN eWindow);
INTERFACE MS_BOOL Hal_SC_is_extra_req_en(MS_U16 *pu16MainHStart, MS_U16 *pu16MainHEnd, MS_U16 *pu16SubHStart,  MS_U16 *pu16SubHEnd, SCALER_WIN eWindow);
INTERFACE MS_U8 Hal_SC_getVSyncWidth(SCALER_WIN eWindow);
INTERFACE void Hal_SC_enable_cursor_report(MS_BOOL bEn);

//Add for K1, just a dummy function at this chip
INTERFACE E_APIXC_ReturnValue Hal_SC_Set_OSDLayer(E_VOP_OSD_LAYER_SEL  eVOPOSDLayer, SCALER_WIN eWindow);
INTERFACE E_VOP_OSD_LAYER_SEL Hal_SC_Get_OSDLayer(SCALER_WIN eWindow);
INTERFACE E_APIXC_ReturnValue Hal_SC_Set_VideoAlpha(MS_U8 u8Val, SCALER_WIN eWindow);
INTERFACE E_APIXC_ReturnValue Hal_SC_Get_VideoAlpha(MS_U8 *pu8Val, SCALER_WIN eWindow);
INTERFACE void Hal_XC_ClearScalingFactorForInternalCalib( void );
INTERFACE MS_BOOL Hal_SC_IsHW2Dto3DPatch_Enable(void);

INTERFACE MS_U8 MHal_SC_cal_usedgain_to_reggain(MS_U32 u16UsedGain_x32);
INTERFACE E_APIXC_ReturnValue Hal_SC_OP2VOPDESel(E_OP2VOP_DE_SEL eVopDESel);
INTERFACE MS_BOOL Hal_XC_SVOutput_GetCaps(void);
INTERFACE void Hal_SC_set_Mclk(MS_BOOL bEn, SCALER_WIN eWindow);
INTERFACE void Hal_SC1_set_ODCLK(MS_U8 u8val);
INTERFACE void Hal_SC2_set_ODCLK(MS_U32 u32SynthCtl, MS_U8 u8Synth);
INTERFACE void Hal_SC1_set_ODCLK_src(MS_BOOL bHDMI_Out, MS_HDMIT_OUTCLK_SRC_TYPE enHDMI_Out_Type);


#ifdef MSOS_TYPE_CE
#define _MHal_SC_Flock_Caculate_LPLLSet(...)
#define _MHal_SC_Set_LPLL_Limit(...)
#define _MHal_SC_Flock_Set_IGainPGain(...)
#define _MHal_SC_Flock_Set_LPLL_Enable(...)

#define MHal_CLKGEN_FRC_Bypass_Enable(...)
#else
#define _MHal_SC_Flock_Caculate_LPLLSet(args...)
#define _MHal_SC_Set_LPLL_Limit(args...)
#define _MHal_SC_Flock_Set_IGainPGain(args...)
#define _MHal_SC_Flock_Set_LPLL_Enable(args...)

#define MHal_CLKGEN_FRC_Bypass_Enable(args...)
#endif

INTERFACE E_APIXC_ReturnValue Hal_XC_IsForcePrescaling(const XC_InternalStatus *pSrcInfo, MS_BOOL *pbForceV, MS_BOOL *pbForceH, SCALER_WIN eWindow);
INTERFACE MS_U32 MHal_SC_Get_LpllSet_Factor(MS_U8 u8LPLL_Mode);
INTERFACE void    Hal_SC_set_FICLK(SCALER_WIN eWindow, MS_BOOL bMainPreVSD, MS_BOOL bSubPreVSD);
INTERFACE MS_U16  Hal_SC_Get_Ouput_HSync_End(SCALER_WIN eWindow);
INTERFACE MS_U16  Hal_SC_Get_Ouput_VSync_End(SCALER_WIN eWindow);
INTERFACE MS_BOOL Hal_SC_Get_Ouput_Interlace(SCALER_WIN eWindow);
INTERFACE void    Hal_SC_Set_Capture_Enable(SCALER_WIN eWindow, MS_BOOL bEn);


#undef INTERFACE
#endif /* MHAL_SC_H */

