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

#ifndef DRV_SC_SCALING_H

#define DRV_SC_SCALING_H





#ifdef DRV_SC_SCALING_C

#define INTERFACE

#else

#define INTERFACE extern

#endif



typedef enum

{

    DRV_MIRROR_NORMAL,

    DRV_MIRROR_H_ONLY,

    DRV_MIRROR_V_ONLY,

    DRV_MIRROR_HV,

    DRV_MIRROR_MAX,

} DrvMirrorMode_t;




#ifdef MULTI_SCALER_SUPPORTED
extern void MDrv_SC_set_fbl(MS_BOOL bEn, SCALER_WIN eWindow);
extern MS_BOOL MDrv_SC_get_fbl(SCALER_WIN eWindow);
#else
extern void MDrv_SC_set_fbl(MS_BOOL bEn);  //TODO
extern MS_BOOL MDrv_SC_get_fbl(void);  //TODO
#endif

#define ENABLE_DYNAMIC_SUB_LB_OFFSET 	(1)


INTERFACE void MDrv_SC_SetPQHSDFlag(MS_BOOL bEnable, SCALER_WIN eWindow);

INTERFACE MS_BOOL MDrv_SC_GetPQHSDFlag(SCALER_WIN eWindow);

INTERFACE void MDrv_sc_set_capture_window(SCALER_WIN eWindow);

INTERFACE void MDrv_SC_set_display_window(SCALER_WIN eWindow);

INTERFACE void MDrv_SC_set_std_display_window(SCALER_WIN eWindow);

INTERFACE void MDrv_SC_set_crop_window(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow);

INTERFACE void MDrv_SC_set_prescaling_ratio(INPUT_SOURCE_TYPE_t enInputSourceType, XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow);

INTERFACE void MDrv_SC_set_postscaling_ratio(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow);

INTERFACE void MDrv_SC_set_fetch_number_limit(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow);

INTERFACE void MDrv_SC_set_write_limit(MS_U32 u32WritelimitAddrBase, SCALER_WIN eWindow);

INTERFACE void MDrv_SC_set_opm_write_limit(MS_BOOL bEnable, MS_BOOL bFlag, MS_U32 u32OPWlimitAddr, SCALER_WIN eWindow);

INTERFACE void MDrv_SC_SetSkipWaitVsync( MS_BOOL eWindow,MS_BOOL bIsSkipWaitVsyn);

INTERFACE MS_BOOL MDrv_SC_GetSkipWaitVsync( MS_BOOL bScalerWin);

INTERFACE void MDrv_SC_sw_db( P_SC_SWDB_INFO pDBreg, SCALER_WIN eWindow );

INTERFACE void MDrv_SC_set_shift_line(MS_BOOL bFBL, MS_DEINTERLACE_MODE eDeInterlaceMode, SCALER_WIN eWindow);

INTERFACE void MDrv_SC_set_mirror(MS_BOOL bEnable, SCALER_WIN eWindow);

INTERFACE void MDrv_SC_set_mirror2(DrvMirrorMode_t eMirrorMode, SCALER_WIN eWindow);

INTERFACE void MDrv_XC_init_fbn_win(SCALER_WIN eWindow);



INTERFACE MS_BOOL MDrv_XC_Set_DynamicScaling(MS_U32 u32MemBaseAddr, MS_U8 u8MIU_Select, MS_U8 u8IdxDepth, MS_BOOL bOP_On, MS_BOOL bIPS_On, MS_BOOL bIPM_On);



INTERFACE void MDrv_XC_FilLineBuffer(MS_BOOL bEnable, SCALER_WIN eWindow);

INTERFACE void MDrv_XC_SetFrameBufferAddress(MS_PHYADDR u32FBAddress, MS_PHYADDR u32FBSize, SCALER_WIN eWindow);

INTERFACE void MDrv_XC_SetFrameBufferAddressSilently(MS_PHYADDR u32FBAddress, MS_PHYADDR u32FBSize, SCALER_WIN eWindow);

INTERFACE MS_U8 MDrv_SC_Get_FrameStoreNum_Factor(XC_FRAME_STORE_NUMBER eFrameStoreNumber, MS_BOOL bLinearMode);



INTERFACE MS_BOOL MDrv_XC_GetHSizeChangeManuallyFlag(SCALER_WIN eWindow);



INTERFACE MS_PHYADDR MDrv_XC_GetIPMBase(MS_U8 num, SCALER_WIN eWindow);

INTERFACE MS_U32 MDrv_XC_GetDNRBufSize(SCALER_WIN eWindow);

INTERFACE void MDrv_XC_SetDNRBufSize(MS_U32 u32DNRBufSize, SCALER_WIN eWindow);

INTERFACE void MDrv_SC_Set_pre_align_pixel(MS_BOOL bEnable, MS_U16 pixels, SCALER_WIN eWindow);

INTERFACE void MDrv_SC_Set_FB_Num(SCALER_WIN eWindow, XC_FRAME_STORE_NUMBER enFBType, MS_BOOL bInterlace);

INTERFACE void MDrv_SC_ClearScalingFactorForInternalCalib(void);

INTERFACE void MDrv_SC_Set_wr_bank_mapping(MS_U8 u8val, SCALER_WIN eWindow);

INTERFACE void MDrv_SC_Enable_PreScaling(MS_BOOL bHSDEnable, MS_BOOL bVSDEnable, SCALER_WIN eWindow);

INTERFACE void MDrv_SC_SetFullRangeCapture(SCALER_WIN eWindow);

INTERFACE void MDrv_XC_ClearSizeOfLB(SCALER_WIN eWindow);


#undef INTERFACE



#endif /* DRV_SC_SCALING_H */



