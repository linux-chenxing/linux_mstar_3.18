
#ifndef MDRV_SC_DISPLAY_H
#define MDRV_SC_DISPLAY_H

typedef struct
{
    MS_U16 u16Htt_out;
    MS_U16 u16Vtt_out;
}XC_DISPLAYOUT_INFO;

MS_U16 MDrv_SC_set_frc(MS_U16 u16InputVFreqX10);
void MDrv_SC_init_lpll(XC_PANEL_INFO* pstPanel_Info /* in */);
void MDrv_SC_set_fpll(MS_BOOL bEnable, MS_BOOL bEnablePhaseLock, MS_U8 u8Lpll_bank);
void MDrv_SC_clear_lpll(void);
void MDrv_SC_monitor_lpll(void);
void MDrv_SC_set_fastlpll(MS_BOOL bEn);
MS_BOOL MDrv_SC_get_fastlpll(void);
MS_BOOL MDrv_Scaler_GetFPLLDoneStatus(void);

void MDrv_SC_set_freerun_window(void);
void MDrv_XC_SetFreeRunTiming (void);
void MDrv_SC_GenerateBlackVideo(MS_BOOL bEnable, SCALER_WIN eWindow);

#ifdef MULTI_SCALER_SUPPORTED

void MDrv_SC_Set_BLSK(MS_BOOL bEnable, SCALER_WIN eWindow);
void MDrv_SC_GenerateBlackVideoForBothWin(MS_BOOL bEnable, SCALER_WIN eWindow);
void MDrv_SC_rep_window(MS_BOOL bEnable,MS_WINDOW_TYPE Win,MS_U8 u8Color, SCALER_WIN eWindow);
void MDrv_SC_SetDisplay_LineBuffer_Mode(MS_BOOL bEnable, SCALER_WIN eWindow);
void MDrv_XC_DisableSubWindow(SCALER_WIN eWindow);

#else
void MDrv_SC_Set_BLSK(MS_BOOL bEnable);
void MDrv_SC_GenerateBlackVideoForBothWin(MS_BOOL bEnable);
void MDrv_SC_rep_window(MS_BOOL bEnable,MS_WINDOW_TYPE Win,MS_U8 u8Color);
void MDrv_SC_SetDisplay_LineBuffer_Mode(MS_BOOL bEnable);
void MDrv_XC_DisableSubWindow(void);

#endif

void MDrv_SC_set_output_dclk(MS_U16 u16Htt, MS_U16 u16Vtt, MS_U16 u16VFreq, SCALER_WIN eWindow);
void MDrv_SC_set_video_dark(MS_BOOL bEnable, MS_U16 u16VideoDarkLevel);
void MDrv_Scaler_set_de_window(XC_PANEL_INFO *pPanel_Info);
void MDrv_SC_set_csc( MS_BOOL bEnable, SCALER_WIN eWindow );
void MDrv_XC_set_dispwin_to_reg(SCALER_WIN eWindow, MS_WINDOW_TYPE *pdspwin);
void MDrv_XC_get_dispwin_from_reg(SCALER_WIN eWindow, MS_WINDOW_TYPE *pdspwin);
MS_U16 MDrv_SC_GetOutputVFreqX100(void);


void MDrv_XC_SetOSD2VEmode(EN_VOP_SEL_OSD_XC2VE_MUX  eVOPSelOSD_MUX);

void MDrv_XC_FPLLCusReset(void);

void MDrv_XC_GetDefaultHVSyncInfo(void);

MS_BOOL MDrv_XC_Get_OPWriteOffEnable(SCALER_WIN eWindow);

#ifdef MULTI_SCALER_SUPPORTED
void MDrv_XC_EnableCLK_for_SUB(MS_BOOL bEnable, SCALER_WIN eWindow);
#else
void MDrv_XC_EnableCLK_for_SUB(MS_BOOL bEnable);
#endif

E_APIXC_ReturnValue MDrv_XC_SetOSDLayer(E_VOP_OSD_LAYER_SEL eVOPOSDLayer, SCALER_WIN eWindow);
E_VOP_OSD_LAYER_SEL MDrv_XC_GetOSDLayer(SCALER_WIN eWindow);
E_APIXC_ReturnValue MDrv_XC_SetVideoAlpha(MS_U8 u8Val, SCALER_WIN eWindow);
E_APIXC_ReturnValue MDrv_XC_GetVideoAlpha(MS_U8 *pu8Val, SCALER_WIN eWindow);

void MDrv_XC_Set_TGen(SCALER_WIN eWindow, XC_TGEN_INFO_t stTGenInfo);
MS_BOOL MDrv_XC_Get_TGen_SCTbl_HV_Start(SCALER_WIN eWindow, MS_U16 *pu16Hstart, MS_U16 *pu16Vstart);
MS_BOOL MDrv_XC_Get_TGen_SCTbl_H_Bacporch(SCALER_WIN eWindow, MS_U16 *pu16H_Backporch);

#undef INTERFACE
#endif /* MDRV_SC_DISPLAY_H */
