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
/// file    Mdrv_adc.c
/// @brief  Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
#define MDRV_ADC_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/wait.h>
#endif

#include "MsCommon.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"

// Internal Definition
#include "drvXC_IOPort.h"
#include "xc_Analog_Reg.h"
#include "xc_hwreg_utility2.h"
#include "apiXC.h"
#include "apiXC_Adc.h"
#include "drvXC_ADC_Internal.h"
#include "mhal_adc.h"
#include "drv_sc_ip.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void MDrv_XC_ADC_set_cvbs_out(E_ADC_CVBSOUT_TYPE e_cvbs_out_type)
{
    Hal_ADC_set_cvbs_out(e_cvbs_out_type);
}

MS_BOOL MDrv_XC_ADC_Is_cvbs_out_enabled(E_DEST_TYPE enOutputType)
{
    if (enOutputType == OUTPUT_CVBS1)
    {
        return Hal_ADC_is_cvbs_out_enabled(OUTPUT_PORT_CVBS1);
    }
    else if (enOutputType == OUTPUT_CVBS2)
    {
        return Hal_ADC_is_cvbs_out_enabled(OUTPUT_PORT_CVBS2);
    }
    else
    {
        return FALSE;
    }

}

MS_BOOL MDrv_XC_ADC_use_hardware_auto_offset(INPUT_SOURCE_TYPE_t eSource)
{
    if ( IsSrcTypeYPbPr(eSource) )
    {
        return ADC_HARDWARE_AUTOOFFSET_YPBPR;
    }
    else if ( IsSrcTypeVga(eSource) )
    {
        return ADC_HARDWARE_AUTOOFFSET_RGB;
    }
    else if ( IsSrcTypeScart(eSource) )
    {
        return ADC_HARDWARE_AUTOOFFSET_SCARTRGB;
    }
    else
    {
        return ENABLE;
    }

}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void MDrv_XC_ADC_offset_setting(XC_AdcGainOffsetSetting *pstADCSetting)
{
    Hal_ADC_offset_setting(pstADCSetting);
}

void MDrv_XC_ADC_gain_setting(XC_AdcGainOffsetSetting *pstADCSetting)
{
    Hal_ADC_gain_setting(pstADCSetting);
}

void MDrv_XC_ADC_Source_Calibrate(ADC_INPUTSOURCE_TYPE enADCInput)
{
    Hal_ADC_Set_Source_Calibration(enADCInput);
}

//******************************************************************************
/// Initial before external calibration
/// @return none
//******************************************************************************
void MApi_XC_ADC_InitExternalCalibration(INPUT_SOURCE_TYPE_t eSource, APIXC_AdcGainOffsetSetting* InitialGainOffset)
{
    ADC_INPUTSOURCE_TYPE eADCSource = ADC_INPUTSOURCE_UNKNOW;

    if ( IsSrcTypeYPbPr(eSource) )
    {
        eADCSource = ADC_INPUTSOURCE_ONLY_YPBPR;
    }
    else if ( IsSrcTypeVga(eSource) )
    {
        eADCSource = ADC_INPUTSOURCE_ONLY_RGB;
    }
    else if ( IsSrcTypeScart(eSource) )
    {
        eADCSource = ADC_INPUTSOURCE_ONLY_SCART;
    }
    else
    {
        // Undefined
    }
    _XC_ENTRY();
    Hal_ADC_InitExternalCalibration(eADCSource,(XC_AdcGainOffsetSetting*)InitialGainOffset);
    _XC_RETURN();
}

//******************************************************************************
/// Initial before internal calibration
/// @return none
//******************************************************************************
void MApi_XC_ADC_InitInternalCalibration(INPUT_SOURCE_TYPE_t eSource, APIXC_AdcGainOffsetSetting* InitialGainOffset)
{
    ADC_INPUTSOURCE_TYPE eADCSource = ADC_INPUTSOURCE_UNKNOW;

    if ( IsSrcTypeYPbPr(eSource) )
    {
        eADCSource = ADC_INPUTSOURCE_ONLY_YPBPR;
    }
    else if ( IsSrcTypeVga(eSource) )
    {
        eADCSource = ADC_INPUTSOURCE_ONLY_RGB;
    }
    else if ( IsSrcTypeScart(eSource) )
    {
        eADCSource = ADC_INPUTSOURCE_ONLY_SCART;
    }
    else
    {
        // Undefined
    }

    _XC_ENTRY();
    Hal_ADC_InitInternalCalibration(eADCSource,(XC_AdcGainOffsetSetting*)InitialGainOffset);
    _XC_RETURN();
}

void MApi_XC_ADC_ExitExternalCalibration(INPUT_SOURCE_TYPE_t eSource, APIXC_AdcGainOffsetSetting* InitialGainOffset)
{

    ADC_INPUTSOURCE_TYPE eADCSource = ADC_INPUTSOURCE_UNKNOW;

    if ( IsSrcTypeYPbPr(eSource) )
    {
        eADCSource = ADC_INPUTSOURCE_ONLY_YPBPR;
    }
    else if ( IsSrcTypeVga(eSource) )
    {
        eADCSource = ADC_INPUTSOURCE_ONLY_RGB;
    }
    else if ( IsSrcTypeScart(eSource) )
    {
        eADCSource = ADC_INPUTSOURCE_ONLY_SCART;
    }
    else
    {
        // Undefined
    }

    Hal_ADC_ExitExternalCalibration(eADCSource,(XC_AdcGainOffsetSetting*)InitialGainOffset);

}

//******************************************************************************
/// Generate ADC clock
/// @param clocktype \b IN Clock type need to generate
//******************************************************************************
void MApi_XC_ADC_GenClock(ADC_Gen_Clock_Type clocktype)
{

    _XC_ENTRY();
    Hal_ADC_clk_gen_setting(clocktype);
    _XC_RETURN();
}

//******************************************************************************
/// Generate Internal voltage in ADC
/// @param InternalVoltage \b IN Voltage type will be generate
//******************************************************************************
void MApi_XC_ADC_SetInternalVoltage(ADC_Internal_Voltage InternalVoltage)
{
    _XC_ENTRY();
    Hal_ADC_dtop_internaldc_setting(InternalVoltage);
    _XC_RETURN();
    MsOS_DelayTask(50);  //** must delay n Vsync
}

//******************************************************************************
/// Get ADC Default gain / offset by referring input structure.
/// @param eSource \b IN The input source
/// @param pstADCSetting \b IN gain / offset need to be set
//******************************************************************************
void MApi_XC_ADC_GetDefaultGainOffset(INPUT_SOURCE_TYPE_t eSource , APIXC_AdcGainOffsetSetting* pstADCSetting)
{
    ADC_INPUTSOURCE_TYPE eADCSource = ADC_INPUTSOURCE_UNKNOW;

    if ( IsSrcTypeYPbPr(eSource) )
    {
        eADCSource = ADC_INPUTSOURCE_ONLY_YPBPR;
    }
    else if ( IsSrcTypeVga(eSource) )
    {
        eADCSource = ADC_INPUTSOURCE_ONLY_RGB;
    }
    else if ( IsSrcTypeScart(eSource) )
    {
        eADCSource = ADC_INPUTSOURCE_ONLY_SCART;
    }
    else
    {
        // Undefined
    }
    _XC_ENTRY();
    Hal_ADC_get_default_gain_offset(eADCSource,(XC_AdcGainOffsetSetting*)pstADCSetting);
    _XC_RETURN();
}


//******************************************************************************
/// get HW default Gain value
/// @param
/// @return HW default Gain value
//******************************************************************************
MS_U16 MApi_XC_ADC_GetCenterGain(void)
{
    return Hal_ADC_get_center_gain();
}


//******************************************************************************
/// get HW default Gain value
/// @param
/// @return HW default Gain value
//******************************************************************************
MS_U16 MApi_XC_ADC_GetCenterOffset(void)
{
    return Hal_ADC_get_center_offset();
}

//******************************************************************************
/// get maximal value of offset
/// @param
/// @return maximal value of offset
//******************************************************************************
MS_U16 MApi_XC_ADC_GetMaximalOffsetValue(void)
{
    MS_U8 u8ADC_offsetbitcnt;
    _XC_ENTRY();
    u8ADC_offsetbitcnt = Hal_ADC_get_offset_bit_cnt();
    _XC_RETURN();
    return  (1 << u8ADC_offsetbitcnt ) - 1;
}

//******************************************************************************
/// get maximal value of gain
/// @param
/// @return maximal value of gain
//******************************************************************************
MS_U16 MApi_XC_ADC_GetMaximalGainValue(void)
{
    MS_U8 u8ADC_gainbitcnt;
    _XC_ENTRY();
    u8ADC_gainbitcnt = Hal_ADC_get_gain_bit_cnt();
    _XC_RETURN();
    return  (1 << u8ADC_gainbitcnt ) - 1;
}

//******************************************************************************
/// Set Gain value
/// @param u8color \b IN The channel type need to be set
/// @param u16value \b IN The value need to be set.
//******************************************************************************
void MApi_XC_ADC_SetGain(MS_U8 u8color, MS_U16 u16value)
{
    _XC_ENTRY();
    switch(u8color)
    {
        case ADC_CHR:
            Hal_ADC_dtop_gain_r_setting(u16value);
            break;
        case ADC_CHG:
            Hal_ADC_dtop_gain_g_setting(u16value);
            break;
        case ADC_CHB:
            Hal_ADC_dtop_gain_b_setting(u16value);
            break;
        case ADC_ALL:
        default:
            Hal_ADC_dtop_gain_r_setting(u16value);
            Hal_ADC_dtop_gain_g_setting(u16value);
            Hal_ADC_dtop_gain_b_setting(u16value);
            break;
    }
    _XC_RETURN();
}

//******************************************************************************
/// Set offset value
/// @param u8color \b IN The channel type need to be set
/// @param u16value \b IN The value need to be set.
//******************************************************************************
void MApi_XC_ADC_SetOffset(MS_U8 u8color, MS_U16 u16value)
{
    _XC_ENTRY();
    switch(u8color)
    {
        case ADC_CHR:
            Hal_ADC_dtop_offset_r_setting(u16value);
            break;
        case ADC_CHG:
            Hal_ADC_dtop_offset_g_setting(u16value);
            break;
        case ADC_CHB:
            Hal_ADC_dtop_offset_b_setting(u16value);
            break;
        case ADC_ALL:
        default:
            Hal_ADC_dtop_offset_r_setting(u16value);
            Hal_ADC_dtop_offset_g_setting(u16value);
            Hal_ADC_dtop_offset_b_setting(u16value);
            break;
    }
    _XC_RETURN();
}

//******************************************************************************
/// Enable auto function (auto gain/offset) for Component or RGB source
/// @param bEnable \b IN Enable or Disable
/// @param bIsYPbPrFlag \b IN Is current source Ypbpr or not
/// @param bIsAutoSWMode \b IN Is auto software?
//******************************************************************************
void MApi_XC_ADC_AutoSetting(MS_BOOL bEnable, MS_BOOL bIsYPbPrFlag, MS_BOOL bIsAutoSWMode)
{
    _XC_ENTRY();
    Hal_ADC_dtop_calibration_target_setting(bIsYPbPrFlag);
    Hal_ADC_dtop_sw_mode_setting(bEnable, bIsAutoSWMode);
    _XC_RETURN();
}

void MDrv_XC_ADC_reset(MS_U16 u16Resete)
{
    Hal_ADC_reset(u16Resete);
}

void MDrv_XC_ADC_Set_Source(ADC_INPUTSOURCE_TYPE enADC_SourceType, E_MUX_INPUTPORT* enInputPortType, MS_U8 u8PortCount)
{
    Hal_ADC_Set_Source(enADC_SourceType, enInputPortType, u8PortCount);
}

/******************************************************************************/
/// Set ADC Sample clock
/// @param u16Value \b IN value need to be set.
/******************************************************************************/
void MApi_XC_ADC_SetPcClock( MS_U16 u16Value )
{
    _XC_ENTRY();
    Hal_ADC_dtop_clk_setting(u16Value);
    _XC_RETURN();
}

void MDrv_XC_ADC_hpolarity_setting( MS_BOOL bHightActive )
{
    Hal_ADC_hpolarity_setting(bHightActive);
}

/******************************************************************************/
/// Set ADC Phase
/// @param u8Value \b IN The adc phase need to be update
/******************************************************************************/
void MApi_XC_ADC_SetPhase( MS_U8 u8Value )
{
    _XC_ENTRY();
    MDrv_XC_wait_input_vsync(1, 50, MAIN_WINDOW);
    Hal_ADC_set_phase(u8Value);
    _XC_RETURN();
}

void MApi_XC_ADC_SetPhaseEx( MS_U16 u16Value )
{
    _XC_ENTRY();
    MDrv_XC_wait_input_vsync(1, 50, MAIN_WINDOW);
    Hal_ADC_set_phaseEx(u16Value);
    _XC_RETURN();
}

/******************************************************************************/
/// Set ADC Phase
/// @param u8Value \b IN The adc phase need to be update
/******************************************************************************/
void MDrv_XC_ADC_SetPhase( MS_U8 u8Value )
{
    _XC_ENTRY();
    Hal_ADC_set_phase(u8Value);
    _XC_RETURN();
}

void MDrv_XC_ADC_SetPhaseEx( MS_U16 u16Value )
{
    _XC_ENTRY();
    Hal_ADC_set_phaseEx(u16Value);
    _XC_RETURN();
}

//******************************************************************************
/// Get current ADC Phase set
/// @return Current Phase
//******************************************************************************
MS_U16 MApi_XC_ADC_GetPhaseRange( void )
{
    return Hal_ADC_get_phase_range();
}

/******************************************************************************/
///Get SOG level range
///@param u32Min \b OUT: min of SOG level
///@param u32Max \b OUT: max of SOG level
///@param u32Recommend_value \b OUT: recommend value
/******************************************************************************/
void MApi_XC_ADC_GetSoGLevelRange(MS_U32 *u32min, MS_U32 *u32max, MS_U32 *u32Recommend_value)
{
    Hal_ADC_get_SoG_LevelRange(u32min, u32max, u32Recommend_value);
}

/******************************************************************************/
///Set SOG level
///@param u32Min \b IN: set SOG level
/******************************************************************************/
void MApi_XC_ADC_SetSoGLevel(MS_U32 u32Value)
{
    Hal_ADC_set_SoG_Level(u32Value);
}

/******************************************************************************/
///Set SOG level
///@param u32Min \b IN: set SOG calibration
/******************************************************************************/
void MApi_XC_ADC_SetSoGCal(void)
{
    Hal_ADC_set_SoG_Calibration();
}

//******************************************************************************
/// Get current ADC Phase set
/// @return Current Phase
//******************************************************************************
MS_U8 MApi_XC_ADC_GetPhase( void )
{
    MS_U8 u8ADC_phase;
    _XC_ENTRY();
    u8ADC_phase = Hal_ADC_get_phase();
    _XC_RETURN();
    return u8ADC_phase;//Hal_ADC_get_phase();
}

MS_U16 MApi_XC_ADC_GetPhaseEx( void )
{
    MS_U16 u16ADC_phase;
    _XC_ENTRY();
    u16ADC_phase = Hal_ADC_get_phaseEx();
    _XC_RETURN();
    return u16ADC_phase;
}

void MDrv_XC_ADC_clamp_placement_setting(MS_U16 u16InputClockMHz)
{
    Hal_ADC_clamp_placement_setting(u16InputClockMHz);
}

//******************************************************************************
/// Set V clamping type
/// @param type \b IN The Vclampling type
//******************************************************************************
void MApi_XC_ADC_SetVClampLevel(ADC_VClamp_Level_Type type)
{
    Hal_XC_ADC_Set_VClamp_level(type);
}


/******************************************************************************/
///This function will set ADC registers for different port
///@param enInputSourceType \b IN: source type
///@param u8InputClock \b IN: pixel clock
/******************************************************************************/
void MDrv_XC_ADC_vco_ctrl (MS_BOOL bIsDVIPort, MS_U16 u16InputClock)
{
    Hal_ADC_set_vco_ctrl(bIsDVIPort, u16InputClock);
}

//******************************************************************************
/// Turn off ADC
//******************************************************************************
void MApi_XC_ADC_PowerOff(void)
{
    Hal_ADC_poweroff();
}

void MDrv_XC_ADC_poweron_source (ADC_INPUTSOURCE_TYPE enADC_SourceType)
{
    Hal_XC_ADC_poweron_source(enADC_SourceType);
}

void MDrv_ADC_SourceSwitch(MS_BOOL bSwitch)
{
   Hal_ADC_SourceSwitch( bSwitch);
}

void MDrv_XC_ADC_Set_Mode(INPUT_SOURCE_TYPE_t enSourceType, MS_U16 u16PixelClockPerSecond, MS_U16 u16HorizontalTotal, MS_U16 u16SamplingRatio)
{
    if (IsSrcTypeVga(enSourceType) || IsSrcTypeYPbPr(enSourceType) || IsSrcTypeScart(enSourceType) )
    {
	    Hal_ADC_set_mode(_MApi_XC_ADC_ConvertSrcToADCSrc(enSourceType), u16PixelClockPerSecond, u16HorizontalTotal, u16SamplingRatio);
    }
}

void MDrv_XC_ADC_BackupAdcReg(void)
{
    Hal_ADC_auto_adc_backup();
}

void MDrv_XC_ADC_RestoreAdcReg(void)
{
    Hal_ADC_auto_adc_restore();
}

MS_BOOL MApi_XC_ADC_IsScartRGB(void)
{
    MS_BOOL bRGB = 0;
    bRGB = Hal_ADC_is_scart_rgb();

    return bRGB;
}

MS_U16 MApi_XC_ADC_GetPcClock(void )
{
    return Hal_ADC_get_clk();
}

void MApi_XC_ADC_SetRGB_PIPE_Delay(MS_U8 u8Value)
{
    Hal_ADC_set_RGB_PIPE_Delay(u8Value);
}

void MApi_XC_ADC_ScartRGB_SOG_ClampDelay(MS_U16 u16Clpdly, MS_U16 u16Caldur)
{

    Hal_ADC_set_ScartRGB_SOG_ClampDelay(u16Clpdly, u16Caldur);
}

void MApi_XC_ADC_Set_YPbPrLooseLPF(MS_BOOL benable)
{
    Hal_ADC_set_YPbPrLooseLPF(benable);
}

void MDrv_XC_ADC_Set_SOGBW(MS_U16 u16value)
{
    Hal_ADC_Set_SOGBW(u16value);
}

void MApi_XC_ADC_SetClampDuration(MS_U16 u16Value)
{
    Hal_ADC_dtop_vClampDuration_setting(u16Value);
}

#undef MDRV_ADC_C
