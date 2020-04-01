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
/// file  apiXC_Adc.c
/// brief  Scaler API layer Interface
/// author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _API_XC_ADC_C_
#define _API_XC_ADC_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/wait.h>
#elif defined(MSOS_TYPE_CE)
#include <windows.h>
#include <ceddk.h>
#endif

// Common
#include "MsCommon.h"
#include "mhal_xc_chip_config.h"
#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "mvideo_context.h"
#include "apiXC_Adc.h"
#include "xc_hwreg_utility2.h"

// Driver
#include "drvXC_ADC_Internal.h"

// Internal Definition

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define TRACE_ADC_INPUT(x) //x
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

//******************************************************************************
/// Enable and Disable CVBS out. This will setup ADC part for CVBS out (monitor out).
/// @param bEnable \b IN Enable CVBS Out
/// @param enInputPortType \b IN The source need to monitor out.
/// @param isDigitalSource \b IN Tell the driver current source is digital source or not. (ex. If internal VIF used in ATV, it will be digital source)
//******************************************************************************
void MApi_XC_ADC_SetCVBSOut(MS_BOOL bEnable, E_DEST_TYPE enOutputType , INPUT_SOURCE_TYPE_t enInputPortType, MS_BOOL isDigitalSource)
{
    E_ADC_CVBSOUT_TYPE enCVBSoutType = ADC_CVBSOUT_DISABLE_1;
/*
    printf("====>CVBS out is %s", (bEnable) ?("ENABLE"):("DISABLE"));
    printf(" and Output is from =%s(%x), eSourceToVE=%x, enInputPortType=%u\n", (enOutputType==20)?("CVBS_OUT1"):
                                                    (enOutputType==21)?("CVBS_OUT2"):("Others"), enOutputType, gSrcInfo[MAIN_WINDOW].Status2.eSourceToVE, enInputPortType);
*/
    if ( bEnable )
    {
        if ( enOutputType == OUTPUT_CVBS1)
        {
            if (IsSrcTypeDTV(enInputPortType) ||
                (gSrcInfo[MAIN_WINDOW].Status2.eSourceToVE != E_XC_NONE))
            {
                //IP/DI/OP2 to VE case, need goes with VIF_VE path
                enCVBSoutType = ADC_CVBSOUT_VIF_VE_1;
            }
            else if ( IsSrcTypeATV(enInputPortType) && isDigitalSource )
            {
                enCVBSoutType = ADC_CVBSOUT_VIF_VIF_1;  // VIF source.
            }
            else if ( IsSrcTypeSV(enInputPortType) )
            {
                enCVBSoutType = ADC_CVBSOUT_SV_ON_1;
            }
            else
            {
                enCVBSoutType = ADC_CVBSOUT_CVBS_ON_1;
            }
        }
        else if ( enOutputType == OUTPUT_CVBS2)
        {
            if (IsSrcTypeDTV(enInputPortType) ||
                (gSrcInfo[MAIN_WINDOW].Status2.eSourceToVE != E_XC_NONE))
            {
                //IP/DI/OP2 to VE case, need goes with VIF_VE path
                enCVBSoutType = ADC_CVBSOUT_VIF_VE_2;
            }
            else if ( IsSrcTypeATV(enInputPortType) && isDigitalSource )
            {
                enCVBSoutType = ADC_CVBSOUT_VIF_VIF_2;  // VIF source.
            }
            else if ( IsSrcTypeSV(enInputPortType) )
            {
                enCVBSoutType = ADC_CVBSOUT_SV_ON_2;
            }
            else
            {
                enCVBSoutType = ADC_CVBSOUT_CVBS_ON_2;
            }
        }
    }
    else
    {
        if ( enOutputType == OUTPUT_CVBS1)
        {
            enCVBSoutType = ADC_CVBSOUT_DISABLE_1;
        }
        else if ( enOutputType == OUTPUT_CVBS2)
        {
            enCVBSoutType = ADC_CVBSOUT_DISABLE_2;
        }
    }

    _XC_ENTRY();
    MDrv_XC_ADC_set_cvbs_out( enCVBSoutType );
    _XC_RETURN();

}

MS_BOOL MApi_XC_ADC_IsCVBSOutEnabled(E_DEST_TYPE enOutputType)
{
    if (enOutputType == OUTPUT_CVBS1 || enOutputType == OUTPUT_CVBS2)
    {
        return MDrv_XC_ADC_Is_cvbs_out_enabled(enOutputType);
    }
    else
        return FALSE;
}

//******************************************************************************
/// Convert input source type to ADC input source type
/// @param enInputSource \b IN The source array.
//******************************************************************************
ADC_INPUTSOURCE_TYPE _MApi_XC_ADC_ConvertSrcToADCSrc(INPUT_SOURCE_TYPE_t enInputSourceType)
{
    if ( IsSrcTypeVga(enInputSourceType) )
    {
        return ADC_INPUTSOURCE_ONLY_RGB;
    }
    else if ( IsSrcTypeYPbPr(enInputSourceType) )
    {
        return ADC_INPUTSOURCE_ONLY_YPBPR;
    }
    else if ( IsSrcTypeATV(enInputSourceType) )
    {
        return ADC_INPUTSOURCE_ONLY_ATV;
    }
    else if ( IsSrcTypeSV(enInputSourceType) )
    {
        return ADC_INPUTSOURCE_ONLY_SVIDEO;
    }
    else if ( IsSrcTypeAV(enInputSourceType) )
    {
        return ADC_INPUTSOURCE_ONLY_CVBS;
    }
    else if ( IsSrcTypeScart(enInputSourceType) )
    {
        return ADC_INPUTSOURCE_ONLY_SCART;
    }
    else if ( IsSrcTypeHDMI(enInputSourceType) )
    {
        return ADC_INPUTSOURCE_ONLY_DVI;
    }
    else if ( IsSrcTypeStorage(enInputSourceType) || IsSrcTypeDTV(enInputSourceType))
    {
        return ADC_INPUTSOURCE_ONLY_MVOP;
    }
    else
    {
        return ADC_INPUTSOURCE_UNKNOW;
    }
}

//******************************************************************************
/// Set ADC calibration for each input source
/// @param enInputSource \b IN The source array.
//******************************************************************************
void MApi_XC_ADC_Source_Calibrate(INPUT_SOURCE_TYPE_t enInputSourceType)
{
    MDrv_XC_ADC_Source_Calibrate(_MApi_XC_ADC_ConvertSrcToADCSrc(enInputSourceType));
}

//******************************************************************************
/// Set ADC for specific input source
/// @param enInputSource \b IN The source array. It is possible there are multi-active sources at the same time (For ex. RGB source with ATV monitor out).
/// @param u8InputSourceCount \b IN The number of active source.
//******************************************************************************
typedef enum {
	ADC_INPUT_OP_NONE = 0x01,
	ADC_INPUT_OP_ADD,
	ADC_INPUT_OP_DEL,
}ADC_INPUT_OPTYPE;

static MS_S8 IDX_FROM_DIGIT[0x10] = { -1,   0,  1, -1,
                                       2,  -1, -1, -1,
                                       3,  -1, -1, -1,
                                      -1,  -1, -1, -1, };

static MS_U8 g_u8InputSourceCounter[ADC_INPUTSOURCE_ONLY_TOTAL];

static MS_U8 Get_Bit_From_BYTE(MS_U8 u8Bits)
{
	MS_S8 s8Idx;

	if ( -1 != (s8Idx = IDX_FROM_DIGIT[(0xF&u8Bits)]) )
		return s8Idx;

	if ( -1 != (s8Idx = IDX_FROM_DIGIT[(u8Bits>>4)] ));
		return (4+s8Idx);

	return 0xFF;
}


static void finalize_adc_source_input(ADC_INPUTSOURCE_TYPE * peAdcPresent, ADC_INPUTSOURCE_TYPE * efinAdcInputs, ADC_INPUT_OPTYPE eAdcOptype)
{
	MS_U8 idx;

	idx = Get_Bit_From_BYTE(*peAdcPresent);

	if ( !peAdcPresent || !efinAdcInputs)
		return;

	if ( ADC_INPUT_OP_NONE == eAdcOptype )
		return;

	if ( ADC_INPUTSOURCE_ONLY_TOTAL <= idx )
		return;

	if ( ADC_INPUT_OP_ADD == eAdcOptype )
	{
		if ( 0xff > g_u8InputSourceCounter[idx] )
			g_u8InputSourceCounter[idx] = g_u8InputSourceCounter[idx] + 1;

		if ( (*peAdcPresent)!=((*efinAdcInputs)&(*peAdcPresent)) )
		{
			(*efinAdcInputs) |= (*peAdcPresent);
		}
	}
	else if ( ADC_INPUT_OP_DEL == eAdcOptype )
	{
		if ( 0 < g_u8InputSourceCounter[idx] )
			g_u8InputSourceCounter[idx] = g_u8InputSourceCounter[idx] - 1;

		if ( 0!=((*efinAdcInputs)&(*peAdcPresent)) && 0==g_u8InputSourceCounter[idx] )
		{
			(*efinAdcInputs) &= ~(*peAdcPresent);
		}
	}

}

static void _MApi_XC_ADC_SetInputSource( INPUT_SOURCE_TYPE_t*  enInputSource, MS_U8 u8InputSourceCount, ADC_INPUT_OPTYPE eAdcOptype)
{
    ADC_INPUTSOURCE_TYPE adc_source_index[2] = {ADC_INPUTSOURCE_UNKNOW, ADC_INPUTSOURCE_UNKNOW};
	E_MUX_INPUTPORT enPorts[3] = {INPUT_PORT_NONE_PORT, INPUT_PORT_NONE_PORT, INPUT_PORT_NONE_PORT};
    MS_U8 u8Port_count = 0;
    MS_BOOL isMultipleSource = ( u8InputSourceCount > 1 );
    MS_U8 i;

	static ADC_INPUTSOURCE_TYPE epreviousInputs =  ADC_INPUTSOURCE_UNKNOW;
	ADC_INPUTSOURCE_TYPE current_input_source   =  ADC_INPUTSOURCE_UNKNOW;

    TRACE_ADC_INPUT(
        for(i=0; i<u8InputSourceCount; i++)
        {
            printf("(src %d)enInputSource(%s), u8InputSourceCount(%d)\n",
                    i,
                    enInputSource[i] == INPUT_SOURCE_VGA?"VGA" :
                    enInputSource[i] == INPUT_SOURCE_TV? "TV"  :
                    enInputSource[i] == INPUT_SOURCE_YPBPR? "YPBPR"  :
                    enInputSource[i] == INPUT_SOURCE_YPBPR2? "YPBPR2"  :
                    enInputSource[i] == INPUT_SOURCE_YPBPR3? "YPBPR3"  :
                    "Others Input",
                    u8InputSourceCount);
        }
    );


	current_input_source = epreviousInputs;

    for(i=0;i <u8InputSourceCount; i++)
    {
        adc_source_index[i] = _MApi_XC_ADC_ConvertSrcToADCSrc(enInputSource[i]);

		finalize_adc_source_input(&adc_source_index[i], &current_input_source, eAdcOptype);
    }


    TRACE_ADC_INPUT(printf("===> %s\n", isMultipleSource?"Multi Input":"Single Input"));


	#if ENABLE_ADC_INPUTSOURCE_COUNTER_MODE

	printf("[%s:%04d] %d, %d, %d, %d, %d, %d, %d, %d  \r\n", TEXT(__FUNCTION__),
		                                                        __LINE__,
		                                                        g_u8InputSourceCounter[0],
		                                                        g_u8InputSourceCounter[1],
		                                                        g_u8InputSourceCounter[2],
		                                                        g_u8InputSourceCounter[3],
																g_u8InputSourceCounter[4],
																g_u8InputSourceCounter[5],
																g_u8InputSourceCounter[6],
																g_u8InputSourceCounter[7]  );



	printf("[%s:%04d] adc_src = 0x%02x, 0x%02x, prev = 0x%02x, curr = 0x%02x !!!\r\n", TEXT(__FUNCTION__),
				                                                           			   __LINE__,
								                                                       adc_source_index[0],
								                                                       adc_source_index[1],
								                                                       epreviousInputs,
								                                                       current_input_source );

	//if ( current_input_source!=epreviousInputs && ADC_INPUT_OP_ADD==eAdcOptype ) // for mantis 0296003 //
	if ( current_input_source!=epreviousInputs )
	{
		MApi_XC_Mux_GetPort( enInputSource[0] , enPorts , &u8Port_count );
		MDrv_XC_ADC_poweron_source(current_input_source);
		MDrv_XC_ADC_Set_Source(current_input_source, enPorts, u8Port_count);

		epreviousInputs = current_input_source;
	}

	#else

	if(isMultipleSource)
    {
        // set Power for main/sub
        if(adc_source_index[0] != ADC_INPUTSOURCE_UNKNOW && adc_source_index[1] != ADC_INPUTSOURCE_UNKNOW)
        {
            MDrv_XC_ADC_poweron_source((ADC_INPUTSOURCE_TYPE)(adc_source_index[0]|adc_source_index[1]));
        }

        // set mux for main
        if(adc_source_index[0] != ADC_INPUTSOURCE_UNKNOW)
        {
            MApi_XC_Mux_GetPort( enInputSource[0] , enPorts , &u8Port_count );
            MDrv_XC_ADC_Set_Source((ADC_INPUTSOURCE_TYPE)(adc_source_index[0] | adc_source_index[1]), enPorts, u8Port_count);
        }
        else if(adc_source_index[1] != ADC_INPUTSOURCE_UNKNOW)
        {
            MApi_XC_Mux_GetPort( enInputSource[1] , enPorts , &u8Port_count );
            MDrv_XC_ADC_Set_Source((ADC_INPUTSOURCE_TYPE)(adc_source_index[0] | adc_source_index[1]), enPorts, u8Port_count);
        }
    }
    else
    {
        // set Mux & Power
        if(adc_source_index[0] != ADC_INPUTSOURCE_UNKNOW)
        {
            MApi_XC_Mux_GetPort( enInputSource[0] , enPorts , &u8Port_count );
            MDrv_XC_ADC_poweron_source(adc_source_index[0]);
            MDrv_XC_ADC_Set_Source(adc_source_index[0], enPorts, u8Port_count);
        }
    }

	#endif
}

#if ENABLE_ADC_INPUTSOURCE_COUNTER_MODE

void MApi_XC_ADC_AddInputSource(INPUT_SOURCE_TYPE_t*  enInputSource, MS_U8 u8InputSourceCount)
{
	_MApi_XC_ADC_SetInputSource(enInputSource, u8InputSourceCount, ADC_INPUT_OP_ADD);
}
void MApi_XC_ADC_DeleteInputSource(INPUT_SOURCE_TYPE_t*  enInputSource, MS_U8 u8InputSourceCount)
{
	_MApi_XC_ADC_SetInputSource(enInputSource, u8InputSourceCount, ADC_INPUT_OP_DEL);
}
#else

void MApi_XC_ADC_SetInputSource( INPUT_SOURCE_TYPE_t*  enInputSource, MS_U8 u8InputSourceCount)
{
	_MApi_XC_ADC_SetInputSource(enInputSource, u8InputSourceCount, ADC_INPUT_OP_NONE);
}
#endif


//******************************************************************************
/// Set ADC gain / offset by referring input structure.
/// @param pstADCSetting \b IN gain / offset need to be set
//******************************************************************************
void MApi_XC_ADC_AdjustGainOffset(APIXC_AdcGainOffsetSetting* pstADCSetting)
{
    if(pstADCSetting == NULL)
    {
        return;
    }
    _XC_ENTRY();
    MDrv_XC_ADC_offset_setting((XC_AdcGainOffsetSetting *)pstADCSetting);
    MDrv_XC_ADC_gain_setting((XC_AdcGainOffsetSetting *)pstADCSetting);
    _XC_RETURN();
}

//******************************************************************************
/// Backup the setting for internal auto-calibration
/// @param
//******************************************************************************
void MApi_XC_ADC_BackupInternalAutoReg(void)
{
    _XC_ENTRY();
    MDrv_XC_ADC_BackupAdcReg();
    _XC_RETURN();
}

//******************************************************************************
/// Restore the setting for internal auto-calibration
/// @param
//******************************************************************************
void MApi_XC_ADC_RestoreInternalAdcReg(void)
{
    _XC_ENTRY();
    MDrv_XC_ADC_RestoreAdcReg();
    _XC_RETURN();
}

void MApi_XC_ADC_Set_SOGBW(MS_U16 u16value)
{
    _XC_ENTRY();
    MDrv_XC_ADC_Set_SOGBW(u16value);
    _XC_RETURN();
}

#undef _API_XC_ADC_C_
#endif  // _API_XC_ADC_C_
