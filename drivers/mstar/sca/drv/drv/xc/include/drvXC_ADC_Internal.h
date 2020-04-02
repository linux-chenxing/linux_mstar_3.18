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
/// @file   drvTEMP.h
/// @brief  TEMP Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_ADC_INTERNAL_H_
#define _DRV_ADC_INTERNAL_H_
#include "mvideo_context.h"

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
/// ADC setting
typedef struct
{
    MS_U16 u16RedGain;      ///< ADC red gain
    MS_U16 u16GreenGain;    ///< ADC green gain
    MS_U16 u16BlueGain;     ///< ADC blue gain
    MS_U16 u16RedOffset;    ///< ADC red offset
    MS_U16 u16GreenOffset;  ///< ADC green offset
    MS_U16 u16BlueOffset;   ///< ADC blue offset
} XC_AdcGainOffsetSetting;


typedef enum
{
    En_ADC_A    =  0x01,
    En_ADC_B    =  0x02,
    En_DVI      =  0x04,
    En_VD       =  0x08,
    En_VD_YC    =  0x10,
    En_FB_RGB   =  0x20,
    En_ADC_AMUX =  0x40,
    EN_ADC_FB   =  0x80,
    Mask_VD_En  =  En_VD|En_VD_YC|En_FB_RGB,
} E_ADC_FUNC_ENABLE;

/*
    ADC multiple inputsource enum.
    Common interface for Hal and Api
 */

typedef enum
{
    // Unknow
    ADC_INPUTSOURCE_UNKNOW = 0x00,

    // Single source
    ADC_INPUTSOURCE_ONLY_RGB    = 0x01,
    ADC_INPUTSOURCE_ONLY_YPBPR  = 0x02,
    ADC_INPUTSOURCE_ONLY_MVOP   = 0x04,
    ADC_INPUTSOURCE_ONLY_DVI    = 0x08,
    ADC_INPUTSOURCE_ONLY_ATV    = 0x10,
    ADC_INPUTSOURCE_ONLY_SVIDEO = 0x20,
    ADC_INPUTSOURCE_ONLY_SCART  = 0x40,
    ADC_INPUTSOURCE_ONLY_CVBS   = 0x80,

    ADC_INPUTSOURCE_ONLY_TOTAL  = 8,

    ADC_INPUTSOURCE_MULTI_RGB_MVOP   = (ADC_INPUTSOURCE_ONLY_RGB | ADC_INPUTSOURCE_ONLY_MVOP),
    ADC_INPUTSOURCE_MULTI_RGB_DVI    = (ADC_INPUTSOURCE_ONLY_RGB | ADC_INPUTSOURCE_ONLY_DVI),
    ADC_INPUTSOURCE_MULTI_RGB_ATV    = (ADC_INPUTSOURCE_ONLY_RGB | ADC_INPUTSOURCE_ONLY_ATV),
    ADC_INPUTSOURCE_MULTI_RGB_SVIDEO = (ADC_INPUTSOURCE_ONLY_RGB | ADC_INPUTSOURCE_ONLY_SVIDEO),
    ADC_INPUTSOURCE_MULTI_RGB_SCART  = (ADC_INPUTSOURCE_ONLY_RGB | ADC_INPUTSOURCE_ONLY_SCART),
    ADC_INPUTSOURCE_MULTI_RGB_CVBS   = (ADC_INPUTSOURCE_ONLY_RGB | ADC_INPUTSOURCE_ONLY_CVBS),

    ADC_INPUTSOURCE_MULTI_YPBPR_MVOP   = (ADC_INPUTSOURCE_ONLY_YPBPR | ADC_INPUTSOURCE_ONLY_MVOP),
    ADC_INPUTSOURCE_MULTI_YPBPR_DVI    = (ADC_INPUTSOURCE_ONLY_YPBPR | ADC_INPUTSOURCE_ONLY_DVI),
    ADC_INPUTSOURCE_MULTI_YPBPR_ATV    = (ADC_INPUTSOURCE_ONLY_YPBPR | ADC_INPUTSOURCE_ONLY_ATV),
    ADC_INPUTSOURCE_MULTI_YPBPR_SVIDEO = (ADC_INPUTSOURCE_ONLY_YPBPR | ADC_INPUTSOURCE_ONLY_SVIDEO),
    ADC_INPUTSOURCE_MULTI_YPBPR_SCART  = (ADC_INPUTSOURCE_ONLY_YPBPR | ADC_INPUTSOURCE_ONLY_SCART),
    ADC_INPUTSOURCE_MULTI_YPBPR_CVBS   = (ADC_INPUTSOURCE_ONLY_YPBPR | ADC_INPUTSOURCE_ONLY_CVBS),

    ADC_INPUTSOURCE_MULTI_MVOP_DVI    = (ADC_INPUTSOURCE_ONLY_MVOP | ADC_INPUTSOURCE_ONLY_DVI),
    ADC_INPUTSOURCE_MULTI_MVOP_SVIDEO = (ADC_INPUTSOURCE_ONLY_MVOP | ADC_INPUTSOURCE_ONLY_SVIDEO),
    ADC_INPUTSOURCE_MULTI_MVOP_SCART  = (ADC_INPUTSOURCE_ONLY_MVOP | ADC_INPUTSOURCE_ONLY_SCART),
    ADC_INPUTSOURCE_MULTI_MVOP_CVBS   = (ADC_INPUTSOURCE_ONLY_MVOP | ADC_INPUTSOURCE_ONLY_CVBS),

    ADC_INPUTSOURCE_MULTI_DVI_ATV    = (ADC_INPUTSOURCE_ONLY_DVI | ADC_INPUTSOURCE_ONLY_ATV),
    ADC_INPUTSOURCE_MULTI_DVI_SVIDEO = (ADC_INPUTSOURCE_ONLY_DVI | ADC_INPUTSOURCE_ONLY_SVIDEO),
    ADC_INPUTSOURCE_MULTI_DVI_SCART  = (ADC_INPUTSOURCE_ONLY_DVI | ADC_INPUTSOURCE_ONLY_SCART),
    ADC_INPUTSOURCE_MULTI_DVI_CVBS   = (ADC_INPUTSOURCE_ONLY_DVI | ADC_INPUTSOURCE_ONLY_CVBS),
}ADC_INPUTSOURCE_TYPE;

/*
    ADC CVBS out enum.
    Common interface for Hal and Api
 */
typedef enum
{
    // ch1 out, source select
    ADC_CVBSOUT_DISABLE_1,
    ADC_CVBSOUT_SV_ON_1,
    ADC_CVBSOUT_SV_OFF_1,
    ADC_CVBSOUT_CVBS_ON_1,
    ADC_CVBSOUT_CVBS_OFF_1,
    ADC_CVBSOUT_VIF_VE_1,
    ADC_CVBSOUT_VIF_VIF_1,

    // ch2 out, source select
    ADC_CVBSOUT_DISABLE_2,
    ADC_CVBSOUT_SV_ON_2,
    ADC_CVBSOUT_SV_OFF_2,
    ADC_CVBSOUT_CVBS_ON_2,
    ADC_CVBSOUT_CVBS_OFF_2,
    ADC_CVBSOUT_VIF_VE_2,
    ADC_CVBSOUT_VIF_VIF_2,
}E_ADC_CVBSOUT_TYPE;


typedef enum
{
    MS_ADC_A_POWER_ON,
    MS_ADC_B_POWER_ON,
    MS_VDB_CVBS_POWER_ON,
    MS_VDB_SV_POWER_ON,
    MS_VDB_FBLANK_POWER_ON,
    MS_VDA_ATV_POWER_ON,
    MS_VDA_CVBS_POWER_ON,
    MS_VDA_SV_POWER_ON,
    MS_VDA_FBLANK_POWER_ON,
    MS_DVI_POWER_ON,
    MS_ADC_VD_BLEND_POWER_ON,
    MS_ADC_POWER_ALL_OFF,
    MS_VDB_DTV_POWER_ON,
} MS_ADC_POWER_ON_TYPE;

typedef struct
{
    MS_U8 *pTable;
    MS_U8 u8TabCols;
    MS_U8 u8TabRows;
    MS_U8 u8TabIdx;
} TAB_Info;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

void Hal_ADC_init(MS_U16 u16XTAL_CLK,MS_BOOL IsShareGrd,  MS_U16 eScartIDPortSelection);
#define MDrv_ADC_init  Hal_ADC_init

void MDrv_XC_ADC_hpolarity_setting(MS_BOOL bHightActive);
void MDrv_XC_ADC_Set_Freerun(MS_BOOL bEnable);
void MDrv_XC_ADC_Set_Mode(INPUT_SOURCE_TYPE_t enSourceType, MS_U16 u16PixelClockPerSecond, MS_U16 u16HorizontalTotal, MS_U16 u16SamplingRatio);
void MDrv_ADC_SourceSwitch(MS_BOOL bSwitch);
// reset
void MDrv_XC_ADC_reset(MS_U16 u16Reset);
void MDrv_XC_ADC_clamp_placement_setting(MS_U16 u16InputClockMHz);
void MDrv_XC_ADC_vco_ctrl (MS_BOOL bIsDVIPort, MS_U16 u16InputClock);

void MDrv_XC_ADC_offset_setting(XC_AdcGainOffsetSetting *pstADCSetting);
void MDrv_XC_ADC_gain_setting(XC_AdcGainOffsetSetting *pstADCSetting);
// Main functions for ADC.
void MDrv_XC_ADC_poweron_source(ADC_INPUTSOURCE_TYPE enADC_SourceType);
// ADC cvbs out
void MDrv_XC_ADC_set_cvbs_out(E_ADC_CVBSOUT_TYPE e_cvbs_out_type);
MS_BOOL MDrv_XC_ADC_Is_cvbs_out_enabled(E_DEST_TYPE enOutputType);
// src & mode setting
void MDrv_XC_ADC_Set_Source(ADC_INPUTSOURCE_TYPE enADC_SourceType, E_MUX_INPUTPORT* enInputPortType, MS_U8 u8PortCount);
// backup setting before auto-adc
void MDrv_XC_ADC_BackupAdcReg(void);
// restore setting after auto-adc
void MDrv_XC_ADC_RestoreAdcReg(void);
// set SOG BW
void MDrv_XC_ADC_Set_SOGBW(MS_U16 u16value);

ADC_INPUTSOURCE_TYPE _MApi_XC_ADC_ConvertSrcToADCSrc(INPUT_SOURCE_TYPE_t enInputSourceType);
void MApi_XC_Mux_GetPort(INPUT_SOURCE_TYPE_t src, E_MUX_INPUTPORT* port_ids , MS_U8* u8port_count );
void MApi_XC_InitIPForInternalTiming(XC_Internal_TimingType timingtype);
void MApi_XC_AutoGainEnable(MS_U8 u8Enable , SCALER_WIN eWindow);
MS_BOOL MApi_XC_IsAutoGainResultReady(SCALER_WIN eWindow);
MS_U32 MApi_XC_GetPhaseValue(SCALER_WIN eWindow);

void    MApi_XC_ADC_Source_Calibrate(INPUT_SOURCE_TYPE_t enInputSourceType);

#define ENABLE_ADC_INPUTSOURCE_COUNTER_MODE		(0)

#if ENABLE_ADC_INPUTSOURCE_COUNTER_MODE
void 	MApi_XC_ADC_AddInputSource(INPUT_SOURCE_TYPE_t*  enInputSource, MS_U8 u8InputSourceCount);
void 	MApi_XC_ADC_DeleteInputSource(INPUT_SOURCE_TYPE_t*  enInputSource, MS_U8 u8InputSourceCount);
#else
void    MApi_XC_ADC_SetInputSource( INPUT_SOURCE_TYPE_t*  enInputSource, MS_U8 u8InputSourceCount);
#endif

// ADC calibration
void MDrv_XC_ADC_Source_Calibrate(ADC_INPUTSOURCE_TYPE enADCInput);
void MApi_XC_ADC_InitExternalCalibration(INPUT_SOURCE_TYPE_t eADC_Source,APIXC_AdcGainOffsetSetting* InitialGainOffset);
void MApi_XC_ADC_InitInternalCalibration(INPUT_SOURCE_TYPE_t eSource, APIXC_AdcGainOffsetSetting* InitialGainOffset);
void MApi_XC_ADC_ExitExternalCalibration(INPUT_SOURCE_TYPE_t eSource, APIXC_AdcGainOffsetSetting* InitialGainOffset);
void MApi_XC_ADC_GenClock(ADC_Gen_Clock_Type clocktype);
void MApi_XC_ADC_SetInternalVoltage(ADC_Internal_Voltage InternalVoltage);
MS_BOOL MDrv_XC_ADC_use_hardware_auto_offset(INPUT_SOURCE_TYPE_t eSource);
void MApi_XC_ADC_AutoSetting(MS_BOOL bEnable, MS_BOOL bIsYPbPrFlag, MS_BOOL bIsAutoSWMode);
void MApi_XC_ADC_SetVClampLevel(ADC_VClamp_Level_Type type);
void MDrv_XC_ADC_SetPhase( MS_U8 u8Value );
void MDrv_XC_ADC_SetPhaseEx( MS_U16 u16Value );

void MApi_XC_ADC_BackupInternalAutoReg(void);
void MApi_XC_ADC_RestoreInternalAdcReg(void);
void MDrv_XC_ADC_SetMode(INPUT_SOURCE_TYPE_t enSourceType, XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow);
void MApi_XC_ADC_SwitchSource(MS_BOOL bUpdateStatus);


#endif // _DRV_ADC_INTERNAL_H_

