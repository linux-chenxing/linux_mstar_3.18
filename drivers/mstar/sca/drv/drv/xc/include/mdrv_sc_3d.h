////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2010 MStar Semiconductor, Inc.
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
/// file    Mdrv_sc_3d.h
/// @brief  Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_SC_3D_H_
#define _MDRV_SC_3D_H_
#undef INTERFACE
#ifdef DRV_SC_3D_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

// 3d internal internal interface
INTERFACE MS_BOOL MDrv_SC_3D_IsOutputVfreq48hz(SCALER_WIN eWindow, MS_U16 u16InputVFreq);
INTERFACE MS_BOOL MDrv_SC_3D_AdjustPreVerSrcForFramepacking(SCALER_WIN eWindow);
INTERFACE MS_BOOL MDrv_SC_3D_IsIPMFetchNoLimitCase(SCALER_WIN eWindow);
INTERFACE void MDrv_SC_3D_Variable_Init(void);
INTERFACE void MDrv_SC_3D_Adjust_FillLBMode(SCALER_WIN eWindow);
INTERFACE void MDrv_SC_3D_Adjust_PreVLength(INPUT_SOURCE_TYPE_t enInputSourceType,
                                            XC_InternalStatus *pSrcInfo,
                                            SCALER_WIN eWindow);
INTERFACE void MDrv_SC_3D_Adjust_PreHorDstSize(INPUT_SOURCE_TYPE_t enInputSourceType,
                                               XC_InternalStatus *pSrcInfo,
                                               SCALER_WIN eWindow);
INTERFACE void MDrv_SC_3D_Adjust_SubLineBufferOffset(INPUT_SOURCE_TYPE_t enInputSourceType,
                                                     XC_InternalStatus *pSrcInfo,
                                                     SCALER_WIN eWindow);
INTERFACE MS_U16 MDrv_SC_3D_Adjust_PreVerDstSize(INPUT_SOURCE_TYPE_t enInputSourceType,
                                                 XC_InternalStatus *pSrcInfo,
                                                 MS_U16 u16ScaleDst,
                                                 SCALER_WIN eWindow);
INTERFACE MS_U16 MDrv_SC_3D_Adjust_PostHorDstSize(XC_InternalStatus *pSrcInfo,
                                                  MS_U16 u16ScaleDst,
                                                  SCALER_WIN eWindow);
INTERFACE MS_U16 MDrv_SC_3D_Adjust_PostHorSrcSize(XC_InternalStatus *pSrcInfo,
                                                  MS_U16 u16ScaleSrc,
                                                  SCALER_WIN eWindow);
INTERFACE MS_U16 MDrv_SC_3D_Adjust_PostVerSrcSize(XC_InternalStatus *pSrcInfo,
                                                  MS_U16 u16ScaleSrc,
                                                  SCALER_WIN eWindow);
INTERFACE void MDrv_SC_3D_Adjust_PostVLength(XC_InternalStatus *pSrcInfo,
                                             SCALER_WIN eWindow);
INTERFACE XC_FRAME_STORE_NUMBER MDrv_SC_3D_Adjust_FBNum(XC_FRAME_STORE_NUMBER u8FBNumType,
                                                 SCALER_WIN eWindow);
INTERFACE void MDrv_SC_3D_Adjust_DNRBase(MS_U32 *pu32DNRBase0,
                                         MS_U32 *pu32DNRBase1,
                                         SCALER_WIN eWindow);
INTERFACE void MDrv_SC_3D_Adjust_OPMBase(MS_U32 *pu32OPMBase0,
                                         MS_U32 *pu32OPMBase1,
                                         MS_U32 *pu32OPMBase2,
                                         MS_U32 u32Offset,
                                         SCALER_WIN eWindow);
INTERFACE void MDrv_SC_3D_Adjust_FetchOffset(XC_InternalStatus *pSrcInfo,
                                             SCALER_WIN eWindow);
INTERFACE MS_BOOL MDrv_SC_3D_FormatValidation(XC_InternalStatus *pSrcInfo,
                                              SCALER_WIN eWindow);
INTERFACE void MDrv_SC_3D_SetWriteMemory(SCALER_WIN eWindow);
INTERFACE MS_BOOL MDrv_SC_3D_Is3DProcessByPIP(SCALER_WIN eWindow);
INTERFACE void MDrv_SC_3D_Adjust_MirrorDNROffset(XC_InternalStatus *pSrcInfo,
                                                 MS_U16 *pu16V_SizeAfterPreScaling,
                                                 MS_U16 *pu16DNROffset,
                                                 MS_U16 *pu16Ratio,
                                                 SCALER_WIN eWindow);
INTERFACE void MDrv_SC_3D_AdjustMirrorCrop(XC_InternalStatus *pSrcInfo,SCALER_WIN eWindow);
INTERFACE void MDrv_SC_3D_Adjust_DNROffset(MS_U16 *pu16DNROffsetTemp,MS_U16 *pu16DNROffset, SCALER_WIN eWindow);
INTERFACE void MDrv_SC_3D_Adjust_CropWinWidth(MS_U16 *pu16CropWinWidth,SCALER_WIN eWindow);
INTERFACE void MDrv_SC_3D_GetFP_Info(XC_InternalStatus *pSrcInfo,
                                     MS_U16 *pu16VactVideo,
                                     MS_U16 *pu16VactSpace,
                                     MS_BOOL bPreVscalingEna);
INTERFACE void MDrv_SC_3D_Disable_Crop(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow);
INTERFACE MS_BOOL MDrv_XC_3D_LoadReg(E_XC_3D_INPUT_MODE eInputMode, E_XC_3D_OUTPUT_MODE eOutputMode, XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow);
INTERFACE MS_BOOL MDrv_SC_3D_Is2Dto3DCase(E_XC_3D_INPUT_MODE e3dInputMode,
                                          E_XC_3D_OUTPUT_MODE e3dOutputMode);
INTERFACE MS_BOOL MDrv_SC_3D_IsUseSameBuffer(void);
INTERFACE void MDrv_SC_3D_CloneMainSettings(XC_InternalStatus *pSrcInfo,
                                            SCALER_WIN eWindow);
INTERFACE MS_BOOL MDrv_SC_3D_Is_LR_Sbs2Line(void);
INTERFACE void MDrv_SC_3D_AdjustHShift(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow);
INTERFACE void MDrv_SC_3D_Adjust_BytesPer2Pixel(MS_U8 *u8BytesPer2PixelTemp,SCALER_WIN eWindow);
INTERFACE MS_BOOL MDrv_SC_3D_Is2TapModeSupportedFormat(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow);
INTERFACE MS_BOOL MDrv_SC_3D_Set2TapMode(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow);
INTERFACE MS_BOOL MDrv_SC_3D_PostPQSetting(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow);
#endif

