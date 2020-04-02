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
#define MHAL_ADC_C
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/kernel.h>
#include <linux/wait.h>
#endif
// Common Definition
#include "MsCommon.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"

// Internal Definition
#include "drvXC_IOPort.h"
#include "xc_Analog_Reg.h"
#include "xc_hwreg_utility2.h"
#include "hwreg_dvi_atop.h"//alex_tung
#include "hwreg_adc_atop.h"
#include "hwreg_adc_dtop.h"
#include "hwreg_adc_dtopb.h"
#include "hwreg_hdmi.h"
#include "apiXC.h"
#include "apiXC_Adc.h"
#include "drvXC_ADC_Internal.h"
#include "hwreg_sc.h"

// include ADC table.
#include "mhal_adctbl.h"
#include "mhal_adctbl.c"
#include "mhal_adc.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

#define MHAL_ADC_DBG(x)  //x

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------
typedef struct
{
    MS_U8  u8L_BkAtop_01;
    MS_U8  u8L_BkAtop_03;
    MS_U8  u8L_BkAtop_0C;
    MS_U8  u8L_BkAtop_2C;
    MS_U8  u8L_BkAtop_1F;
    MS_U8  u8H_BkAtop_2D;
    MS_U8  u8L_BkDtop_06;
    MS_U8  u8H_BkChipTop_1F;
    MS_U8  u8L_BkChipTop_55;
    MS_U8  u8L_BkIpMux_1;
    MS_U16 u16SC_BK1_21;
    MS_U16 u16SC_BK10_19;
    MS_U16 u16L_BkAtop_05;
    MS_U16 u16L_BkAtop_5E;
    MS_U16 u16BkAtop_1C;
    MS_U16 u16BkAtop_04;
    MS_U16 u16BkAtop_05;
    MS_U16 u16BkAtop_06;
    MS_U16 u16BkDtop_01;
    MS_U16 u16BkDtop_02;

    MS_U16 R_UserOffset;
    MS_U16 G_UserOffset;
    MS_U16 B_UserOffset;

    MS_U16 R_BlankLevel;
    MS_U16 G_BlankLevel;
    MS_U16 B_BlankLevel;

    MS_U16 u16SC_BK1_02;
    MS_U16 u16SC_BK1_03;
    MS_U16 u16SC_BK1_04;
    MS_U16 u16SC_BK1_05;
    MS_U16 u16SC_BK1_06;
    MS_U16 u16SC_BK1_07;
    MS_U16 u16SC_BK1_0E;
    MS_U16 u16SC_BK12_01;
    MS_U16 u16SC_BK12_03;
    MS_U16 u16SC_BK12_04;
    MS_U16 u16SC_BK12_0E;
    MS_U16 u16SC_BK12_0F;
    MS_U16 u16SC_BK12_16;
    MS_U16 u16SC_BK12_17;
    MS_U32 u32SC_BK12_10;
    MS_U32 u32SC_BK12_12;
    MS_U32 u32SC_BK12_14;
    MS_U16 u16SC_BK12_07;
    MS_U32 u32SC_BK12_08;
    MS_U32 u32SC_BK12_0A;
    MS_U32 u32SC_BK12_0C;
    MS_U16 u16SC_BK02_04;
    MS_U16 u16SC_BK02_05;
    MS_U16 u16SC_BK02_08;
    MS_U16 u16SC_BK02_09;
    MS_U16 u16SC_BK23_07;
    MS_U16 u16SC_BK23_08;
    MS_U16 u16SC_BK23_09;
    MS_U16 u16SC_BK23_0A;
    MS_U16 u16SC_BK12_1A;
    MS_U16 u16SC_BK12_1B;
    MS_U16 u16SC_BK06_01;
    MS_U16 u16SC_BK06_21;
    MS_U16 u16SC_BK12_44;
    MS_U16 u16SC_BK12_47;
} XC_Adc_BackupSetting;


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static XC_Adc_BackupSetting _stAutoAdcSetting;
static MS_BOOL bIsYPbPrLooseLPF = FALSE;
static MS_BOOL _bSourceSwitched = FALSE;
static MS_BOOL _bEnableHWCalibration = FALSE;
//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
/******************************************************************************/
/// Table dump area
/******************************************************************************/
static void Hal_ADC_LoadTable(TAB_Info* pTab_info)
{
    MS_U32 u32Addr;
    MS_U8 u8Mask;
    MS_U8 u8Value;
    MS_U8 u8DoNotSet;
    MS_U16 i;

    if (pTab_info->pTable == NULL || pTab_info->u8TabRows == 0 || pTab_info->u8TabRows == 1)
        return;

    if (REG_ADDR_SIZE+REG_MASK_SIZE+pTab_info->u8TabIdx*REG_DATA_SIZE >= pTab_info->u8TabCols)
    {
        MHAL_ADC_DBG(printk("Tab_info error\n"));
        return;
    }

    for (i=0; i<pTab_info->u8TabRows-1; i++)
    {
         u32Addr =  (MS_U32)( (pTab_info->pTable[0]<<8) + pTab_info->pTable[1] );

         // DRV_ADC_REG(REG_ADC_DTOPB_FE_L)

         if ( pTab_info->pTable[0] == 0x25 ) // ATOP
            u32Addr = u32Addr | ( REG_ADC_ATOP_BASE & 0xFFFF0000 );
         else if ( pTab_info->pTable[0] == 0x26 ) // DTOP
            u32Addr = u32Addr | ( REG_ADC_DTOP_BASE & 0xFFFF0000 );
         else if ( pTab_info->pTable[0] == 0x12 ) // DTOPB
            u32Addr = u32Addr | ( REG_ADC_DTOPB_BASE & 0xFFFF0000 );

         if(u32Addr == REG_ADC_DTOPB_FE_L)
         {
            // delay only, skip to write next register
            u8Value = pTab_info->pTable[REG_ADDR_SIZE+REG_MASK_SIZE+pTab_info->u8TabIdx*REG_DATA_SIZE + 1];
            MHAL_ADC_DBG(printf("ADC tbl delay (%d) ms \n",u8Value));
            OS_DELAY_TASK(u8Value);
            goto NEXT;
         }

         u8Mask  = pTab_info->pTable[2];
         u8DoNotSet = pTab_info->pTable[REG_ADDR_SIZE+REG_MASK_SIZE+pTab_info->u8TabIdx*REG_DATA_SIZE];
         u8Value = pTab_info->pTable[REG_ADDR_SIZE+REG_MASK_SIZE+pTab_info->u8TabIdx*REG_DATA_SIZE + 1];

         //printf("[AdcTbl02] addr = 0x%06lx, msk = 0x%02x, val= 0x%02x \r\n", u32Addr, u8Mask, u8Value);

         if ( !u8DoNotSet )
         {
             if (u32Addr&0x1)
                W2BYTEMSK((MS_U32)(u32Addr &0xFFFFFE), (MS_U16)u8Value<<8, (MS_U16)u8Mask<<8);
             else
                W2BYTEMSK((MS_U32)u32Addr, (MS_U16)u8Value, (MS_U16)u8Mask);
         }

NEXT:
         pTab_info->pTable+=pTab_info->u8TabCols; // next
    }
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

/******************************************************************************/
///ADC soft reset
/******************************************************************************/
void Hal_ADC_reset(MS_U16 u16Reset)
{
    //[4]: Soft-reset ATOP
    //[3]: Soft-reset PLLB
    //[2]: Soft-reset ADCB
    //[1]: Soft-reset PLLA
    //[0]: Soft-reset ADCA

    // Remove ADC reset after T3
    /*
        W2BYTE(REG_DVI_ATOP_07_L, u16Reset);
        OS_DELAY_TASK(2);
        W2BYTE(REG_DVI_ATOP_07_L, 0x0000);
        */
}

/******************************************************************************/
///This function will set ADC registers for different port
///@param enInputSourceType \b IN: source type
///@param u8InputClock \b IN: pixel clock
/******************************************************************************/
//=========================================================//
// Function : Hal_ADC_ext_clk_en
// Description:
//=========================================================//
void Hal_ADC_ext_clk_en(MS_BOOL benable)
{
    W2BYTEMSK(REG_ADC_ATOP_60_L, (benable ? BIT(7):0), BIT(7));
}

//=========================================================//
// Function : Hal_ADC_hdmi_vco_ctrl
// Description:
//=========================================================//
void Hal_ADC_hdmi_vco_ctrl(MS_U16 u16InputClock)
{
    // Obsolate in T3
}
//=========================================================//
// Function : Hal_ADC_vco_ctrl
// Description:
//=========================================================//
void Hal_ADC_vco_ctrl(MS_U16 u16InputClock)
{
    //Obsolate in T3
}

void Hal_ADC_set_vco_ctrl(MS_BOOL bIsDVIPort, MS_U16 u16InputClock)
{
    //Obsolete
    UNUSED(bIsDVIPort);
    UNUSED(u16InputClock);
}


/******************************************************************************/
///This function sets PLL clock divider ratio
///@param u16Value \b IN: PLL clock divider ratio
/******************************************************************************/
void Hal_ADC_dtop_clk_setting ( MS_U16 u16Value )
{
    // limit set ADC PLL
    if((u16Value > 3) && (u16Value < ADC_MAX_CLK))
    {
        u16Value -= 3; // actual - 3
        W2BYTE(REG_ADC_DTOP_00_L, u16Value);
    }
}

/******************************************************************************/
///This function return phase steps of current chip
///@param u8Value \b IN: phase steps
/******************************************************************************/
MS_U16 Hal_ADC_get_phase_range(void)
{
    // Get phase range according to reg_adc_pll_mod
    // reg_adc_pll_mod == 0 ? 128 :
    // reg_adc_pll_mod == 1 ? 256 :
    // reg_adc_pll_mod == 2 ? 512 : 1024;
	return 128 << (R2BYTEMSK(REG_ADC_ATOP_09_L,BMASK(12:11)) >> 11);
}

MS_U8 Hal_ADC_get_phase(void)
{
    //FIXME: Change API
   return ( (MS_U8) R2BYTEMSK(REG_ADC_ATOP_15_L, BMASK(9:0)));
}

MS_U16 Hal_ADC_get_phaseEx(void)
{
    MS_U16 u16ADCPllMod = R2BYTEMSK(REG_ADC_ATOP_09_L,BMASK(12:11)) >> 11;

   return R2BYTEMSK(REG_ADC_ATOP_15_L,BMASK((6+u16ADCPllMod):0));
}

/******************************************************************************/
///This function sets PLL phase
///@param u8Value \b IN: PLL phase divider ratio
/******************************************************************************/
void Hal_ADC_set_phase( MS_U8 u8Value )
{
    //FIXME: Change API
    W2BYTEMSK(REG_ADC_ATOP_15_L, u8Value+1, BMASK(9:0));
    W2BYTEMSK(REG_ADC_ATOP_15_L, u8Value, BMASK(9:0));
}

void Hal_ADC_set_phaseEx( MS_U16 u16Value )
{
    MS_U16 u16ADCPllMod = R2BYTEMSK(REG_ADC_ATOP_09_L,BMASK(12:11)) >> 11;
    MS_U16 u16Mask = BMASK((6+u16ADCPllMod):0) ;

    W2BYTEMSK(REG_ADC_ATOP_15_L, u16Value+1, u16Mask);
    W2BYTEMSK(REG_ADC_ATOP_15_L, u16Value, u16Mask);
}

/******************************************************************************/
///This function sets ADC offset
///@param *pstADCSetting \b IN: pointer to ADC settings
/******************************************************************************/
void Hal_ADC_offset_setting ( XC_AdcGainOffsetSetting *pstADCSetting  )
{
    //return;     //temp for bring up
    W2BYTEMSK(REG_ADC_DTOP_52_L, ((pstADCSetting->u16RedOffset)), 0x1FFF);
    W2BYTEMSK(REG_ADC_DTOP_57_L, ((pstADCSetting->u16GreenOffset)), 0x1FFF);
    W2BYTEMSK(REG_ADC_DTOP_5C_L, ((pstADCSetting->u16BlueOffset)), 0x1FFF);
}

/******************************************************************************/
///This function sets ADC gain
///@param *pstADCSetting \b IN: pointer to ADC settings
/******************************************************************************/
void Hal_ADC_gain_setting ( XC_AdcGainOffsetSetting *pstADCSetting  )
{
    //return;          //temp for bring up
    W2BYTEMSK(REG_ADC_DTOP_51_L, (pstADCSetting->u16RedGain), 0x3FFF);
    W2BYTEMSK(REG_ADC_DTOP_56_L, (pstADCSetting->u16GreenGain), 0x3FFF);
    W2BYTEMSK(REG_ADC_DTOP_5B_L, (pstADCSetting->u16BlueGain), 0x3FFF);
}

/******************************************************************************/
///This function enable/disable output double buffer
///@param bEnable \b IN:
///- Enable: Turn on ADC double buffer
///- Disable: Turn off ADC double buffer
/******************************************************************************/
void Hal_ADC_doublebuffer_setting(MS_BOOL bEnable)
{
    //Obsolate in T3
}

/******************************************************************************/
///This function recalibrates ADC offset. This function should be called
///after mode changed.
///@param bFlag \b IN:
///- 0: Turn on
///- 1: Turn off
/******************************************************************************/
void Hal_ADC_dtop_calibration_target_setting(MS_BOOL bIsYPbPrFlag)
{
    //FIXME: T3 has new calibration algorithm

}
void Hal_ADC_dtop_sw_mode_setting(MS_BOOL bEnable, MS_BOOL bIsAutoSWMode)
{
#if 0
    TAB_Info Tab_info;
    Tab_info.pTable = (void*)MST_ADCAdcCal_TBL;
    Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_AdcCal_NUMS*REG_DATA_SIZE;
    Tab_info.u8TabRows = sizeof(MST_ADCAdcCal_TBL)/Tab_info.u8TabCols;
    Tab_info.u8TabIdx = 0;

    MHAL_ADC_DBG(printf("ADC Tbl:auto offset\n");)
    Hal_ADC_LoadTable(&Tab_info);
    return;
#endif
}


static void _Hal_ADC_Set_RGBYPbPr_Calibration(void)
{
    TAB_Info Tab_info;

    if (_bEnableHWCalibration == ENABLE)
    {   
        Tab_info.u8TabIdx = ADC_TABLE_AdcCal_Fix_UG;
    }
    else
    {
        Tab_info.u8TabIdx = ADC_TABLE_AdcCal_SW_UG;    
    }

	Tab_info.pTable = (void*)MST_ADCAdcCal_TBL;
	Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_AdcCal_NUMS*REG_DATA_SIZE;
	Tab_info.u8TabRows = sizeof(MST_ADCAdcCal_TBL)/Tab_info.u8TabCols;

    Hal_ADC_LoadTable(&Tab_info);
	
    return;
}


static void _Hal_ADC_Set_AV_Calibration(void)
{
    TAB_Info Tab_info;
    Tab_info.pTable = (void*)MST_ADCAdcCal_AV_TBL;
    Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_AdcCal_AV_NUMS*REG_DATA_SIZE;
    Tab_info.u8TabRows = sizeof(MST_ADCAdcCal_AV_TBL)/Tab_info.u8TabCols;
    Tab_info.u8TabIdx = 0;

    Hal_ADC_LoadTable(&Tab_info);
    return;
}

static void _Hal_ADC_Set_SV_Calibration(void)
{
    TAB_Info Tab_info;
    Tab_info.pTable = (void*)MST_ADCAdcCal_SV_TBL;
    Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_AdcCal_SV_NUMS*REG_DATA_SIZE;
    Tab_info.u8TabRows = sizeof(MST_ADCAdcCal_SV_TBL)/Tab_info.u8TabCols;
    Tab_info.u8TabIdx = 0;

    Hal_ADC_LoadTable(&Tab_info);
    return;
}


static ADC_SOURCE_TYPE _Hal_XC_ADC_InputToSrcType(ADC_INPUTSOURCE_TYPE enADCInput)
{
    ADC_SOURCE_TYPE enADCSrcType;

    switch(enADCInput)
    {
        case ADC_INPUTSOURCE_ONLY_RGB:
            enADCSrcType = ADC_TABLE_SOURCE_RGB;
            break;

        case ADC_INPUTSOURCE_ONLY_YPBPR:
            enADCSrcType = ADC_TABLE_SOURCE_YUV;
            break;

        case ADC_INPUTSOURCE_ONLY_MVOP:
            enADCSrcType = ADC_TABLE_SOURCE_MVOP;
            break;


        case ADC_INPUTSOURCE_ONLY_SVIDEO:
            enADCSrcType = ADC_TABLE_SOURCE_SVIDEO;
            break;

        case ADC_INPUTSOURCE_ONLY_CVBS:
            enADCSrcType = ADC_TABLE_SOURCE_CVBS;
            break;

        case ADC_INPUTSOURCE_ONLY_DVI:
        case ADC_INPUTSOURCE_ONLY_ATV:
        case ADC_INPUTSOURCE_ONLY_SCART:
        default:
            enADCSrcType = ADC_TABLE_SOURCE_NUMS;
            break;
    }

    return enADCSrcType;
}

void Hal_ADC_Set_Source_Calibration(ADC_INPUTSOURCE_TYPE enADC_SourceType)
{
    ADC_SOURCE_TYPE enADCSrcType = _Hal_XC_ADC_InputToSrcType(enADC_SourceType);

    switch (enADCSrcType)
    {
        case ADC_TABLE_SOURCE_RGB:
        case ADC_TABLE_SOURCE_YUV:

        case ADC_TABLE_SOURCE_RGB_MV:

        case ADC_TABLE_SOURCE_YUV_MV:
            _Hal_ADC_Set_RGBYPbPr_Calibration();
            break;
        case ADC_TABLE_SOURCE_CVBS:
        case ADC_TABLE_SOURCE_MVOP_AV:

            _Hal_ADC_Set_AV_Calibration();
            break;
        case ADC_TABLE_SOURCE_SVIDEO:
        case ADC_TABLE_SOURCE_MVOP_SV:

            _Hal_ADC_Set_AV_Calibration();
            _Hal_ADC_Set_SV_Calibration();
            break;


        case ADC_TABLE_SOURCE_RGB_AV:
        case ADC_TABLE_SOURCE_YUV_AV:
            _Hal_ADC_Set_RGBYPbPr_Calibration();
            _Hal_ADC_Set_AV_Calibration();
            break;
        default:
        // ADC_TABLE_SOURCE_ATV:
        // ADC_TABLE_SOURCE_DVI_CVBS:
        // ADC_TABLE_SOURCE_RGB_DVI:
        // ADC_TABLE_SOURCE_YUV_DVI:
        // ADC_TABLE_SOURCE_DVI_SV:
        // ADC_TABLE_SOURCE_SCART:
        // ADC_TABLE_SOURCE_RGB_SC:
        // ADC_TABLE_SOURCE_YUV_SC:
        // ADC_TABLE_SOURCE_MVOP_SC:
        // ADC_TABLE_SOURCE_DVI_SC:
        // ADC_TABLE_SOURCE_DVI
        // ADC_TABLE_SOURCE_MVOP
        // ADC_TABLE_SOURCE_MVOP_DVI
        // ADC_TABLE_SOURCE_NUMS
            break;
    }
}


/******************************************************************************/
///This function sets clamp placement
///@param u8Value \b IN:
/******************************************************************************/
void Hal_ADC_clamp_placement_setting(MS_U16 u16InputClockMHz)
{
    /* Vclamp_dly */
    if(u16InputClockMHz>= 40)  //~= HD resolution likes 1280*720*50/10^6
    {
        W2BYTEMSK(REG_ADC_DTOP_0B_L, 0x38, LBMASK);
    }
    else
    {
        W2BYTEMSK(REG_ADC_DTOP_0B_L, 0x08, LBMASK);
    }
}

void Hal_XC_ADC_Set_VClamp_level(ADC_VClamp_Level_Type type)
{
    //FIXME: T3 has new calibration algorithm
    return;
}

/******************************************************************************/
///This function sets input HSync polarity
///@param u8Value \b IN:
/******************************************************************************/
void Hal_ADC_hpolarity_setting(MS_BOOL bHightActive)
{
    W2BYTEMSK(REG_ADC_DTOP_07_L, (bHightActive ? BIT(7):0), BIT(7));
}

/******************************************************************************/
///This function power off ADC
/******************************************************************************/
void Hal_ADC_poweroff(void)
{
    //Obsolate in T3
}
//----------------------------------------------------------------------
//  RGB Gain setting
//----------------------------------------------------------------------
void Hal_ADC_dtop_gain_r_setting(MS_U16 u16value)
{
    W2BYTEMSK(REG_ADC_DTOP_51_L, u16value, 0x3FFF);
}
void Hal_ADC_dtop_gain_g_setting(MS_U16 u16value)
{
    W2BYTEMSK(REG_ADC_DTOP_56_L, u16value, 0x3FFF);
}
void Hal_ADC_dtop_gain_b_setting(MS_U16 u16value)
{
    W2BYTEMSK(REG_ADC_DTOP_5B_L, u16value, 0x3FFF);
}
//----------------------------------------------------------------------
//  RGB Offset setting
//----------------------------------------------------------------------

void Hal_ADC_dtop_offset_r_setting(MS_U16 u16value)
{
    W2BYTEMSK(REG_ADC_DTOP_52_L, u16value, 0x1FFF);
}
void Hal_ADC_dtop_offset_g_setting(MS_U16 u16value)
{
    W2BYTEMSK(REG_ADC_DTOP_57_L, u16value, 0x1FFF);
}
void Hal_ADC_dtop_offset_b_setting(MS_U16 u16value)
{
    W2BYTEMSK(REG_ADC_DTOP_5C_L, u16value, 0x1FFF);
}

//----------------------------------------------------------------------
// Internal calibration related.
//----------------------------------------------------------------------
void Hal_ADC_dtop_internaldc_setting(ADC_Internal_Voltage InternalVoltage)
{
    MS_U16 u16regvalue;
#if 0 // Ref
    switch ( InternalVoltage )
    {

        case E_ADC_Internal_0_1V: //!! Mapping to T3 0.55V
             u16regvalue = 0xB00;
        break;

        case E_ADC_Internal_0_6V: //!! Mapping to T3 1.05V
             u16regvalue = 0xF00;
        break;

        case E_ADC_Internal_0V:  // We do not need to tune offset in T3 ( using iclamping )
        case E_ADC_Internal_None:
        default:
            W2BYTEMSK(REG_ADC_DTOPB_04_L, 0x00 , BMASK(12:10) );
            //Ref VCal disable
            W2BYTEMSK(REG_ADC_DTOPB_05_L, 0x00 , BIT(12) );
            return;
        break;
    }

    W2BYTEMSK(REG_ADC_DTOPB_04_L, BMASK(12:10) , BMASK(12:10) );
    //Ref VCal enable
    W2BYTEMSK(REG_ADC_DTOPB_05_L, BIT(12) , BIT(12) );

    //Ref Voltage - ADCA VCAL force value
    W2BYTEMSK(REG_ADC_DTOPB_05_L, u16regvalue , BMASK(11:8) );
#else // LDO

    switch ( InternalVoltage )
    {

        case E_ADC_Internal_0_1V: //!! Mapping to T3 0.55V
             u16regvalue = 0x2;
        	break;

        case E_ADC_Internal_0_6V: //!! Mapping to T3 1.05V
             u16regvalue = 0x3;
        	break;

        case E_ADC_Internal_0V:  // We do not need to tune offset in T3 ( using iclamping )
        case E_ADC_Internal_None:
        default:
            W2BYTEMSK(REG_ADC_ATOP_5A_L, 0x00 , BIT(0) );
            // LDO VCal disable
            W2BYTEMSK(REG_ADC_DTOPB_05_L, 0x00 , BMASK(3:0) );

            W2BYTEMSK(REG_ADC_DTOPB_04_L, 0x00 , BMASK(12:10) );

            W2BYTEMSK(REG_ADC_DTOPB_05_L, 0x00 , BMASK(12:8) );
            return;
        	//break;
    }

    W2BYTEMSK(REG_ADC_DTOPB_04_L, BMASK(12:10) , BMASK(12:10) );

    W2BYTEMSK(REG_ADC_DTOPB_05_L, BIT(8) | BIT(12) , BIT(8) | BIT(12) );

    W2BYTEMSK(REG_ADC_ATOP_5A_L, BIT(0) , BIT(0) );

    // LDO VCal enable
    W2BYTEMSK(REG_ADC_DTOPB_05_L, BMASK(3:2) , BMASK(3:2) );

    //Ref Voltage - ADCA VCAL force value
    W2BYTEMSK(REG_ADC_DTOPB_05_L, u16regvalue , BMASK(1:0) );

#endif
}
static MS_U8 Hal_ADC_SearchFreqSetTableIndex(MS_U16 u16OriginalPixClk)
{
    MS_U8  u8ClkIndex;
    for(u8ClkIndex=0; u8ClkIndex<sizeof(MST_ADC_FreqRange_TBL)/sizeof(ADC_FREQ_RANGE); u8ClkIndex++)
    {
        if((u16OriginalPixClk < MST_ADC_FreqRange_TBL[u8ClkIndex].FreqHLimit) &&
            (u16OriginalPixClk >= MST_ADC_FreqRange_TBL[u8ClkIndex].FreqLLimit))
            break;
    }
    return u8ClkIndex;
}

static void Hal_ADC_SetADCBwFilter(ADC_INPUTSOURCE_TYPE enADCInput,MS_U16 u16PixelClk)
{
    TAB_Info Tab_info;
    if ( enADCInput & ADC_INPUTSOURCE_ONLY_YPBPR ) // YPbPr
    {
        Tab_info.pTable = (void*)MST_ADCSetModeYUV_TBL;
        Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_SetMode_NUMS*REG_DATA_SIZE;
        Tab_info.u8TabRows = sizeof(MST_ADCSetModeYUV_TBL)/Tab_info.u8TabCols;

    }
    else // RGB
    {
        Tab_info.pTable = (void*)MST_ADCSetModeRGB_TBL;
        Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_SetMode_NUMS*REG_DATA_SIZE;
        Tab_info.u8TabRows = sizeof(MST_ADCSetModeRGB_TBL)/Tab_info.u8TabCols;
    }

    Tab_info.u8TabIdx = Hal_ADC_SearchFreqSetTableIndex(u16PixelClk);

    MHAL_ADC_DBG(printf("ADC Tbl:BwFilter PixelClk %d, TabIdx %d\n", u16PixelClk, Tab_info.u8TabIdx);)
    Hal_ADC_LoadTable(&Tab_info);

    if (bIsYPbPrLooseLPF && (enADCInput & ADC_INPUTSOURCE_ONLY_YPBPR))
    {
        Tab_info.pTable = (void*)MST_ADCSetModeYUV_Y_TBL;
        Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_SetMode_NUMS*REG_DATA_SIZE;
        Tab_info.u8TabRows = sizeof(MST_ADCSetModeYUV_Y_TBL)/Tab_info.u8TabCols;

        MHAL_ADC_DBG(printf("ADC Tbl: Enable YPbPr Loose LPF\n");)
        Hal_ADC_LoadTable(&Tab_info);
    }

}

void Hal_ADC_ExitExternalCalibration(ADC_INPUTSOURCE_TYPE eADC_Source,XC_AdcGainOffsetSetting* InitialGainOffset)
{
    if ( eADC_Source == ADC_INPUTSOURCE_ONLY_RGB)
    {

    }
	else if ( eADC_Source == ADC_INPUTSOURCE_ONLY_YPBPR)
	{


	}
    else if (eADC_Source == ADC_INPUTSOURCE_ONLY_SCART )
    {
		W2BYTEMSK(REG_ADC_ATOP_42_L, BIT(5) , BIT(5));
    }
    else
    {
        // Undefined.
    }

}

void Hal_ADC_InitExternalCalibration(ADC_INPUTSOURCE_TYPE eADC_Source,XC_AdcGainOffsetSetting* InitialGainOffset)
{
    if ( eADC_Source == ADC_INPUTSOURCE_ONLY_RGB )
    {
        // Initial gain for Ypbpr
        InitialGainOffset->u16BlueGain = InitialGainOffset->u16RedGain =
        InitialGainOffset->u16GreenGain = 0x1000;

        // Initial offset for Ypbpr
        InitialGainOffset->u16GreenOffset = InitialGainOffset->u16BlueOffset =
        InitialGainOffset->u16RedOffset = 0x00;
    }
    else if ( eADC_Source == ADC_INPUTSOURCE_ONLY_YPBPR )
    {
        // Initial gain for Ypbpr
        InitialGainOffset->u16BlueGain = InitialGainOffset->u16RedGain =
        InitialGainOffset->u16GreenGain = 0x1000;

        Hal_ADC_dtop_gain_r_setting(0x1000);
        Hal_ADC_dtop_gain_g_setting(0x1000);
        Hal_ADC_dtop_gain_b_setting(0x1000);

        // Initial offset for Ypbpr
        InitialGainOffset->u16GreenOffset = 0x100;
        InitialGainOffset->u16BlueOffset = InitialGainOffset->u16RedOffset = 0x800;

		#if 0
        // Follow flow is used to set memory format as 444 10 bit mode.
        // Calibration in YUV 444 10 bit mode
        MDrv_WriteByte( BK_SELECT_00, REG_BANK_SCMI);
        MDrv_WriteByteMask(L_BK_SCMI(0x01), BIT(5), BIT(4) | BIT(5));

        // Linear mode enable (for capture memory data)
        MDrv_WriteRegBit(L_BK_SCMI(0x03),  BIT(4) | BIT(5) , BIT(4) | BIT(5) );

        // Disable user memory format
        MDrv_WriteByteMask(H_BK_SCMI(0x02), 0x00, BIT(0)|BIT(1));

        // Turn off 444 -> 422
        MDrv_WriteByte( BK_SELECT_00, REG_BANK_HVSP);
        MDrv_WriteByteMask(L_BK_HVSP(0x0C), 0x00, BIT(7) );

        // Turn off NR (NR only runs on 422 mode)
        MDrv_WriteByte( BK_SELECT_00, REG_BANK_DNR);
        MDrv_WriteByteMask(L_BK_DNR(0x21), 0x00, BIT(0) );

        // Turn off 444 -> 422 filter
        MDrv_WriteByte( BK_SELECT_00, REG_BANK_IP2F2);
        MDrv_WriteByte(L_BK_IP2F2(0x0A), 0x00 );

        #endif

    }
    else if (eADC_Source == ADC_INPUTSOURCE_ONLY_SCART )
    {
        // Initial gain for Scart RGB
        InitialGainOffset->u16BlueGain = InitialGainOffset->u16RedGain =
        InitialGainOffset->u16GreenGain = 0x1000;

        // Initial offset for Scart RGB
        InitialGainOffset->u16GreenOffset =
        InitialGainOffset->u16BlueOffset = InitialGainOffset->u16RedOffset = 0x0000;

    	// Not YUV source. Clamping to 0v
    	Hal_ADC_dtop_calibration_target_setting(FALSE);
    	Hal_ADC_dtop_sw_mode_setting(TRUE, FALSE);
    	W2BYTEMSK(REG_ADC_ATOP_42_L, 0x00 , BIT(5));
    }
    else
    {
        // Undefined.
    }
    Hal_ADC_gain_setting(InitialGainOffset);
    Hal_ADC_offset_setting(InitialGainOffset);

}

void Hal_ADC_InitInternalCalibration(ADC_INPUTSOURCE_TYPE eADC_Source,XC_AdcGainOffsetSetting* InitialGainOffset)
{

    if ( eADC_Source == ADC_INPUTSOURCE_ONLY_RGB )
    {
        // Initial gain for Ypbpr
        InitialGainOffset->u16BlueGain = InitialGainOffset->u16RedGain =
        InitialGainOffset->u16GreenGain = 0x1000;

        // Initial offset for VGA
        InitialGainOffset->u16GreenOffset = InitialGainOffset->u16BlueOffset =
        InitialGainOffset->u16RedOffset = 0x00;
    }
    else if ( eADC_Source == ADC_INPUTSOURCE_ONLY_YPBPR )
    {
        // Initial gain for Ypbpr
        InitialGainOffset->u16BlueGain = InitialGainOffset->u16RedGain =
        InitialGainOffset->u16GreenGain = 0x1000;

        // Initial offset for Ypbpr
        InitialGainOffset->u16GreenOffset = 0x100;
        InitialGainOffset->u16BlueOffset = InitialGainOffset->u16RedOffset = 0x800;
    }
    else if(eADC_Source == ADC_INPUTSOURCE_ONLY_SCART )
    {
        // Initial gain for Scart RGB
        InitialGainOffset->u16BlueGain = InitialGainOffset->u16RedGain =
        InitialGainOffset->u16GreenGain = 0x1000;

        // Initial offset for Scart RGB
        InitialGainOffset->u16GreenOffset =
        InitialGainOffset->u16BlueOffset = InitialGainOffset->u16RedOffset = 0x0000;
    }
    else
    {
        // Undefined.
    }

    //MS_U8 u8Bank;
    //u8Bank = MDrv_ReadByte(BK_SELECT_00);
    // Calibration in YUV or RGB 444 10 bit mode
    //MDrv_WriteByte( BK_SELECT_00, REG_BANK_SCMI);
    //MDrv_WriteByteMask(L_BK_SCMI(0x01), BIT(5), BIT(4) | BIT(5));
    //MDrv_WriteRegBit(L_BK_SCMI(0x03), FALSE, BIT(4));
    //MDrv_WriteRegBit(H_BK_SCMI(0x03), 0x0, (BIT(4)|BIT(5)));//disable mirror

    SC_W2BYTEMSK(REG_SC_BK12_01_L, BIT(5), BIT(4) | BIT(5));
    //SC_W2BYTEMSK(REG_SC_BK12_03_L, FALSE, BIT(4));

    // P mode
    //MDrv_WriteByteMask(H_BK_SCMI(0x01), 0x01, BIT(0)|BIT(1)|BIT(2));
    //MDrv_WriteByteMask(H_BK_SCMI(0x01), 0x00, BIT(4)|BIT(5)|BIT(6));
    //MDrv_WriteRegBit(L_BK_SCMI(0x04), TRUE, BIT(1));
    SC_W2BYTEMSK(REG_SC_BK12_01_L, 0x0100, BIT(8)|BIT(9)|BIT(10));
    SC_W2BYTEMSK(REG_SC_BK12_01_L, 0x0000, BIT(12)|BIT(13)|BIT(14));
    SC_W2BYTEMSK(REG_SC_BK12_04_L, 0x02, BIT(1));

    // Disable user memory format
    //MDrv_WriteByteMask(H_BK_SCMI(0x02), 0x00, BIT(0)|BIT(1));
    SC_W2BYTEMSK(REG_SC_BK12_02_L, 0x0000, BIT(8)|BIT(9));

    //MDrv_WriteByte( BK_SELECT_00, REG_BANK_IP1F2 );
    //MDrv_WriteByte(L_BK_IP1F2(0x0E), 0x11); // enable auto gain function
    SC_W2BYTE(REG_SC_BK01_0E_L, 0x0011); // enable auto gain function

    MDrv_Write2Byte(L_BK_ADC_ATOP(0x00) ,0x0001 );

    // Disable NR
    SC_W2BYTEMSK(REG_SC_BK06_21_L, 0, BIT(1)|BIT(0));
    SC_W2BYTEMSK(REG_SC_BK06_01_L, 0, BIT(1)|BIT(0));

    //framebuffer number
    //SC_W2BYTEMSK(REG_SC_BK12_04_L, 0, BIT(6)|BIT(7));
//    SC_W2BYTEMSK(REG_SC_BK12_07_L, 0, BIT(13));
	// Two frame mode
    //SC_W2BYTEMSK(REG_SC_BK12_19_L, 0x02,  BIT(3)|BIT(2)|BIT(1)|BIT(0));
    //SC_W2BYTEMSK(REG_SC_BK12_44_L, 0, BIT(6)|BIT(7));
    //SC_W2BYTEMSK(REG_SC_BK12_47_L, 0, BIT(13));

    // init power
    MDrv_Write2Byte(L_BK_ADC_ATOP(0x04) ,0xF800 );
    MDrv_Write2Byte(L_BK_ADC_ATOP(0x05) ,0x0003 );
    MDrv_Write2Byte(L_BK_ADC_ATOP(0x06) ,0xFB00 );

    // divider number of the post divider
    MDrv_WriteByteMask(H_BK_ADC_ATOP(0x09), 0x00 ,0x18 );

    // Set offset
    Hal_ADC_dtop_offset_r_setting(0x800);
    Hal_ADC_dtop_offset_g_setting(0x800);
    Hal_ADC_dtop_offset_b_setting(0x800);

    // Set black level
    W2BYTEMSK(REG_ADC_DTOP_50_L, 0x800, 0x0FFF);
    W2BYTEMSK(REG_ADC_DTOP_55_L, 0x800, 0x0FFF);
    W2BYTEMSK(REG_ADC_DTOP_5A_L, 0x800, 0x0FFF);

    //Set ADC input LPF to low bandwidth.
    W2BYTE(REG_ADC_ATOP_34_L,0xFFFF);
    W2BYTE(REG_ADC_ATOP_35_L,0x00FF);

    W2BYTEMSK( L_BK_IPMUX(0x01) , 0xF0, 0xF0 );  //select pattern generator source
    if(eADC_Source == ADC_INPUTSOURCE_ONLY_SCART )
    {
        MDrv_Write2Byte(L_BK_ADC_ATOP(0x03) ,0x0000 );
        MDrv_Write2Byte(L_BK_ADC_ATOP(0x5E) ,0x0200);
        MDrv_WriteByte(L_BK_CHIPTOP(0x55),0x00);
    }
    //MDrv_WriteByte(H_BK_ADC_ATOP(0x1C), 0xF8);   // Turn on SOG input low bandwidth filter

    //MDrv_WriteByte(BK_SELECT_00, u8Bank);

}

void Hal_ADC_clk_gen_setting(ADC_Gen_Clock_Type clocktype)
{

    W2BYTEMSK(REG_ADC_ATOP_01_L, 0x0f, 0x0f );

    W2BYTEMSK(REG_ADC_ATOP_1C_L, BIT(5), BIT(5) );  /// turn off ADC a SoG comparator
    W2BYTEMSK(REG_ADC_ATOP_1F_L, BIT(5), BIT(5) );  /// turn off ADC a SoG comparator

    W2BYTEMSK(REG_ADC_ATOP_0C_L, 0x00, 0x07 );
    switch(clocktype)
    {
    case E_ADC_Gen_480P_Clk:
    default:

        W2BYTEMSK(REG_ADC_DTOP_09_L, 0x1400,0x1F00);

        W2BYTE(REG_ADC_DTOP_01_L, 0x6656);
        W2BYTE(REG_ADC_DTOP_02_L, 0x0066);

        break;
    case E_ADC_Gen_720P_Clk:
        W2BYTEMSK(REG_ADC_DTOP_09_L, 0x0A00,0x1F00);

        W2BYTE(REG_ADC_DTOP_01_L, 0x8B2E);
        W2BYTE(REG_ADC_DTOP_02_L, 0x00A2);

        break;
    case E_ADC_Gen_1080P_Clk:
        W2BYTEMSK(REG_ADC_DTOP_09_L, 0x0100,0x1F00);

        W2BYTE(REG_ADC_DTOP_01_L, 0x4517);
        W2BYTE(REG_ADC_DTOP_02_L, 0x00D1);
        break;
    }

    W2BYTEMSK(REG_ADC_DTOP_06_L, 0x80, 0x80);

}



/******************************************************************************/
/// Power
/******************************************************************************/
// This will become table dumping function.
void Hal_ADC_init(MS_U16 u16XTAL_CLK,MS_BOOL IsShareGrd,  MS_U16 eScartIDPortSelection)
{
    TAB_Info Tab_info;
    MS_U16 ScartIDPort = 0;
    //MS_U8 u8MPLL_LOOP_2nd_DIVIDER;

    Tab_info.pTable = (void*)MST_ADCINIT_TBL;
    Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_INIT_NUMS*REG_DATA_SIZE;
    Tab_info.u8TabRows = sizeof(MST_ADCINIT_TBL)/Tab_info.u8TabCols;
    Tab_info.u8TabIdx = 0;

    MHAL_ADC_DBG(printf("ADC Tbl:init\n");)
    Hal_ADC_LoadTable(&Tab_info);

    // Set share ground bit
    W2BYTEMSK(REG_ADC_ATOP_32_L, IsShareGrd?BIT(9):0 , BIT(9) );

    // Set Scart ID port selection
    if (eScartIDPortSelection & E_XC_SCARTID_TO_HSYNC0)
        ScartIDPort |= BIT(12);
    if (eScartIDPortSelection & E_XC_SCARTID_TO_HSYNC1)
        ScartIDPort |= BIT(13);
    if (eScartIDPortSelection & E_XC_SCARTID_TO_HSYNC2)
        ScartIDPort |= BIT(14);
    if (eScartIDPortSelection & E_XC_SCARTID_TO_SOG2)
        ScartIDPort |= BIT(15);

    //E_XC_SCARTID_TO_SOG0 and E_XC_SCARTID_TO_SOG1 are undefined.
    W2BYTEMSK(REG_ADC_ATOP_32_L, ScartIDPort , BIT(12)|BIT(13)|BIT(14)|BIT(15) );
    // Disable bIsYPbPrLooseLPF
    bIsYPbPrLooseLPF = FALSE;
}


static void Hal_ADC_SetSourceAndPowerOn(ADC_SOURCE_TYPE inputsrc_type)
{
    TAB_Info Tab_info;
    Tab_info.pTable = (void*)MST_ADCSOURCE_TBL;
    Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_SOURCE_NUMS*REG_DATA_SIZE;
    Tab_info.u8TabRows = sizeof(MST_ADCSOURCE_TBL)/Tab_info.u8TabCols;
    Tab_info.u8TabIdx = inputsrc_type;

    MHAL_ADC_DBG(printf("ADC Tbl:set src %d \n", inputsrc_type);)
    Hal_ADC_LoadTable(&Tab_info);
}

static void Hal_ADC_SetMux(ADC_MUX_TYPE ipmux_type)
{
    TAB_Info Tab_info;
    Tab_info.pTable = (void*)MST_ADCMUX_TBL;
    Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_MUX_NUMS*REG_DATA_SIZE;
    Tab_info.u8TabRows = sizeof(MST_ADCMUX_TBL)/Tab_info.u8TabCols;
    Tab_info.u8TabIdx = ipmux_type;

    MHAL_ADC_DBG(printf("ADC Tbl:set mux %d \n",ipmux_type);)
    Hal_ADC_LoadTable(&Tab_info);

}

#if 0
static void Hal_ADC_SetCVBSO_Ch1(void)
{

}

static void Hal_ADC_SetCVBSO_Ch2(void)
{

}
#endif


void Hal_XC_ADC_poweron_source(ADC_INPUTSOURCE_TYPE enADC_SourceType)
{
    // Obsolete in T3. Power control is merged into 'Hal_ADC_Set_Source'
}

ADC_MUX_TYPE Hal_ADC_port_mapping(E_MUX_INPUTPORT enInputPortType)
{
    ADC_MUX_TYPE stADCIPMux;

    switch(enInputPortType)
    {
    case INPUT_PORT_ANALOG0:
        stADCIPMux = ADC_TABLE_MUX_RGB0_Data;
        break;
    case INPUT_PORT_ANALOG0_SYNC:
        stADCIPMux = ADC_TABLE_MUX_RGB0_Sync;
        break;
    case INPUT_PORT_YMUX_CVBS0:
        stADCIPMux = ADC_TABLE_MUX_CVBSY0;
        break;
    case INPUT_PORT_YMUX_CVBS1:
        stADCIPMux = ADC_TABLE_MUX_CVBSY1;
        break;
    case INPUT_PORT_YMUX_CVBS2:
        stADCIPMux = ADC_TABLE_MUX_CVBSY2;
        break;
    case INPUT_PORT_YMUX_G0:
        stADCIPMux = ADC_TABLE_MUX_G0;
        break;
    case INPUT_PORT_CMUX_CVBS0:
        stADCIPMux = ADC_TABLE_MUX_CVBSC0;
        break;
    case INPUT_PORT_CMUX_CVBS1:
        stADCIPMux = ADC_TABLE_MUX_CVBSC1;
        break;
    case INPUT_PORT_CMUX_CVBS2:
        stADCIPMux = ADC_TABLE_MUX_CVBSC2;
        break;
    case INPUT_PORT_CMUX_R0:
        stADCIPMux = ADC_TABLE_MUX_R0;
        break;
    case INPUT_PORT_ANALOG1:        // C3 doesn't support
    case INPUT_PORT_ANALOG2:
    case INPUT_PORT_ANALOG1_SYNC:
    case INPUT_PORT_ANALOG2_SYNC:
    case INPUT_PORT_YMUX_CVBS3:
    case INPUT_PORT_YMUX_CVBS4:
    case INPUT_PORT_YMUX_CVBS5:
    case INPUT_PORT_YMUX_CVBS6:
    case INPUT_PORT_YMUX_CVBS7:
    case INPUT_PORT_YMUX_G1:
    case INPUT_PORT_YMUX_G2:
    case INPUT_PORT_CMUX_CVBS3:
    case INPUT_PORT_CMUX_CVBS4:
    case INPUT_PORT_CMUX_CVBS5:
    case INPUT_PORT_CMUX_CVBS6:
    case INPUT_PORT_CMUX_CVBS7:
    case INPUT_PORT_CMUX_R1:
    case INPUT_PORT_CMUX_R2:
    case INPUT_PORT_DVI0:
    case INPUT_PORT_DVI1:
    case INPUT_PORT_DVI2:
    case INPUT_PORT_DVI3:
    case INPUT_PORT_MVOP:
    case INPUT_PORT_SC0_VOP:
    case INPUT_PORT_SC1_VOP:
    case INPUT_PORT_SC2_VOP:
    default:
        printf("Hal_ADC_port_mapping: unknown type %u\n",enInputPortType);
        stADCIPMux = ADC_TABLE_MUX_NUMS;
        break;
    }

    return stADCIPMux;
}



void Hal_ADC_set_mux(E_MUX_INPUTPORT port)
{
    Hal_ADC_SetMux(Hal_ADC_port_mapping(port));
}

static void _Hal_ADC_set_freerun(MS_BOOL bEnable)
{
    TAB_Info Tab_info;

    if (bEnable)
    {
        Tab_info.pTable = (void*)MST_ADCFreeRunEn_TBL;
        Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_FreeRunEn_NUMS*REG_DATA_SIZE;
        Tab_info.u8TabRows = sizeof(MST_ADCFreeRunEn_TBL)/Tab_info.u8TabCols;
        Tab_info.u8TabIdx = 0;
    }
    else
    {
        Tab_info.pTable = (void*)MST_ADCFreeRunDis_TBL;
        Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_FreeRunDis_NUMS*REG_DATA_SIZE;
        Tab_info.u8TabRows = sizeof(MST_ADCFreeRunDis_TBL)/Tab_info.u8TabCols;
        Tab_info.u8TabIdx = 0;
    }

    Hal_ADC_LoadTable(&Tab_info);
}

static void _Hal_ADC_porst(MS_BOOL bEnable)
{
    TAB_Info Tab_info;

    if (bEnable)
    {
        Tab_info.pTable = (void*)MST_ADCPorstEn_TBL;
        Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_PorstEn_NUMS*REG_DATA_SIZE;
        Tab_info.u8TabRows = sizeof(MST_ADCPorstEn_TBL)/Tab_info.u8TabCols;
        Tab_info.u8TabIdx = 0;
    }
    else
    {
        Tab_info.pTable = (void*)MST_ADCPorstDis_TBL;
        Tab_info.u8TabCols = REG_ADDR_SIZE+REG_MASK_SIZE+ADC_TABLE_PorstDis_NUMS*REG_DATA_SIZE;
        Tab_info.u8TabRows = sizeof(MST_ADCPorstDis_TBL)/Tab_info.u8TabCols;
        Tab_info.u8TabIdx = 0;
    }

    Hal_ADC_LoadTable(&Tab_info);
}

void MDrv_XC_ADC_Set_Freerun(MS_BOOL bEnable)
{

    _Hal_ADC_set_freerun(bEnable);

    if (bEnable)
    {
        _Hal_ADC_porst(TRUE);
        MsOS_DelayTaskUs(10);
        _Hal_ADC_porst(FALSE);
    }
}

void Hal_ADC_SourceSwitch(MS_BOOL bSwitch)
{
    _bSourceSwitched = bSwitch;
}

void Hal_ADC_set_mode(ADC_INPUTSOURCE_TYPE enADCInput, MS_U16 u16PixelClockPerSecond, MS_U16 u16HorizontalTotal, MS_U16 u16SamplingRatio)
{
    MS_U16 u16PixClk, u16HTotal, u16TimeOutCnt = 32;
    static ADC_INPUTSOURCE_TYPE enLastSourceType = ADC_INPUTSOURCE_UNKNOW;
    static MS_U16 u16LastHtotal = 0;
    static MS_U16 u16LastPixelClk = 0;
    static MS_U16 u16LastSamplingRatio = 1;

    if((enLastSourceType != enADCInput) ||
       (u16LastHtotal != u16HorizontalTotal) ||
       (u16LastPixelClk != u16PixelClockPerSecond) ||
       (u16LastSamplingRatio != u16SamplingRatio) ||
       _bSourceSwitched)
    {
        // update new setting
        enLastSourceType = enADCInput;
        u16LastHtotal = u16HorizontalTotal;
        u16LastPixelClk = u16PixelClockPerSecond;
        u16LastSamplingRatio = u16SamplingRatio;

        _bSourceSwitched = 0; //Must clear this flag once the init has been done one time, or the screen will be blinking.
    }
    else
    {
        u16PixClk = u16PixelClockPerSecond * u16SamplingRatio;
        u16HTotal = u16HorizontalTotal * u16SamplingRatio;

        // If same timing, dump register only. Do not reset again.
        Hal_ADC_SetADCBwFilter(enADCInput,u16PixClk);

        // Adc set mode already complete if reach here.
        _Hal_ADC_set_freerun(FALSE);

        ///////////////////////////
        // Protection
        do
        {
            while (R2BYTEMSK(REG_ADC_ATOP_0F_L, BIT(12)))
            {
                _Hal_ADC_set_freerun(TRUE);

                _Hal_ADC_porst(TRUE);
                Hal_ADC_SetADCBwFilter(enADCInput,u16PixClk);
                _Hal_ADC_porst(FALSE);

                MsOS_DelayTask(5);
                Hal_ADC_dtop_clk_setting (u16HTotal);
                _Hal_ADC_set_freerun(FALSE);
    			if(u16TimeOutCnt == 0)
                    break;
                else
                    --u16TimeOutCnt;
            }
    		if(u16TimeOutCnt == 0)
    			break;
    		else
    			--u16TimeOutCnt;

            MsOS_DelayTask(2);
        } while(!R2BYTEMSK(REG_ADC_DTOP_05_L, (BIT(7) | BIT(6)) ));

        return;
    }

    // There is no table for Scart
    if (enADCInput == ADC_INPUTSOURCE_ONLY_SCART)
    {
        Hal_ADC_Set_Source_Calibration(enADCInput);
        return;
    }

    u16PixClk = u16PixelClockPerSecond * u16SamplingRatio;
    u16HTotal = u16HorizontalTotal * u16SamplingRatio;

    _Hal_ADC_set_freerun(TRUE);
    _Hal_ADC_porst(TRUE);
    Hal_ADC_SetADCBwFilter(enADCInput,u16PixClk);
    _Hal_ADC_porst(FALSE);
    MsOS_DelayTask(5);
    Hal_ADC_dtop_clk_setting (u16HTotal);
    // disable freerun after set ATOP_09[12:11], BK_ATOP_0C[2:0], DTOP_00[12:0]
    _Hal_ADC_set_freerun(FALSE);

    do
    {
        while (R2BYTEMSK(REG_ADC_ATOP_0F_L, BIT(12)))
        {
            _Hal_ADC_set_freerun(TRUE);
            _Hal_ADC_porst(TRUE);
            Hal_ADC_SetADCBwFilter(enADCInput,u16PixClk);
            _Hal_ADC_porst(FALSE);
            MsOS_DelayTask(5);
            Hal_ADC_dtop_clk_setting (u16HTotal);
            _Hal_ADC_set_freerun(FALSE);
            if(u16TimeOutCnt == 0)
                break;
            else
                --u16TimeOutCnt;
        }
        if(u16TimeOutCnt == 0)
            break;
        else
            --u16TimeOutCnt;
        MsOS_DelayTask(2);
    } while(!R2BYTEMSK(REG_ADC_DTOP_05_L, (BIT(7) | BIT(6)) ));

    Hal_ADC_Set_Source_Calibration(enADCInput);

    //Hal_ADC_clamp_duration_setting( u16HorizontalTotal/50 );
    W2BYTEMSK(REG_ADC_DTOP_08_L, (u16HorizontalTotal/50)<<8, HBMASK);
}

void Hal_ADC_set_cvbs_out(E_ADC_CVBSOUT_TYPE e_cvbs_out_type)
{
#if 0
    // e_cvbs_out_type must larger then ADC_CVBSOUT_DISABLE_1 and less or equal then ADC_CVBSOUT_VIF_VIF_1
    if ( e_cvbs_out_type <= ADC_CVBSOUT_VIF_VIF_1 )
    {
        if ( e_cvbs_out_type == ADC_CVBSOUT_DISABLE_1 )
        {
            Hal_ADC_SetCVBSO_Ch1();

            // Set mux to a undefined mux for forcing send out black level.
            MDrv_WriteByteMask(REG_ADC_ATOP_51_L, 0xE0 , 0xF0 ); // Change mux to null mux.

            // Make sure Y mux is turn off ( Keep C mux enabled )
            MDrv_WriteByteMask(REG_ADC_ATOP_51_H, BIT(2) , BIT(2) | BIT(3)); // Turn on C mux

            // DISABLE DACA2 (Disable clock of DAC out ch1)
            MDrv_WriteRegBit(REG_CKG_DACA2, ENABLE, CKG_DACA2_GATED);

            // Waiting stable
            MsOS_DelayTask(30);

            // Turn off CVBS out power
            MDrv_WriteByteMask(REG_ADC_ATOP_50_L, BIT(0) , BIT(0) );
        }
        else
        {
            // Turn on CVBS out power
            MDrv_WriteByteMask(REG_ADC_ATOP_50_L, 0x00  , BIT(0) );

            // DISABLE DACA2 (Disable clock of DAC out ch1)
            MDrv_WriteRegBit(REG_CKG_DACA2, DISABLE, CKG_DACA2_GATED);

            // Reset C mux
            MDrv_WriteByteMask(REG_ADC_ATOP_51_L, 0x00 , 0xF0 );

            Hal_ADC_SetCVBSO_Ch1();
        }

    }
    else
    {
        if ( e_cvbs_out_type == ADC_CVBSOUT_DISABLE_2 )
        {
            Hal_ADC_SetCVBSO_Ch2();

            // Set mux to a undefined mux for forcing send out black level.
            MDrv_WriteByteMask(REG_ADC_ATOP_53_L, 0xE0 , 0xF0 ); // Change mux to null mux.

            // Make sure Y mux is turn off ( Keep C mux enabled )
            MDrv_WriteByteMask(REG_ADC_ATOP_53_H, BIT(2) , BIT(2) | BIT(3)); // Turn on C mux

            // DISABLE DACB2 (Disable clock of DAC out ch2)
            MDrv_WriteRegBit(REG_CKG_DACB2, ENABLE, CKG_DACB2_GATED);

            // Waiting stable
            MsOS_DelayTask(30);

            // Turn off CVBS out power
            MDrv_WriteByteMask(REG_ADC_ATOP_52_L, BIT(0) , BIT(0) );

        }
        else
        {
            // Turn on CVBS out power
            MDrv_WriteByteMask(REG_ADC_ATOP_52_L, 0x00 , BIT(0) );

            // DISABLE DACB2 (Disable clock of DAC out ch2)
            MDrv_WriteRegBit(REG_CKG_DACB2, DISABLE, CKG_DACB2_GATED);
            // Reset C mux
            MDrv_WriteByteMask(REG_ADC_ATOP_53_L, 0x00 , 0xF0 );

            Hal_ADC_SetCVBSO_Ch2();
        }
    }

    if (e_cvbs_out_type == ADC_CVBSOUT_VIF_VE_1 || e_cvbs_out_type == ADC_CVBSOUT_VIF_VIF_1 )
    {
        // Select Mux to DAC, otherwise setting from Mux driver.
        MDrv_WriteByte(REG_ADC_ATOP_51_L,0x0F);
    }
    else if ( e_cvbs_out_type == ADC_CVBSOUT_VIF_VE_2 || e_cvbs_out_type == ADC_CVBSOUT_VIF_VIF_2 )
    {
        // Select Mux to DAC, otherwise setting from Mux driver.
        MDrv_WriteByte(REG_ADC_ATOP_53_L,0x0F);
    }
#endif
}

MS_BOOL Hal_ADC_is_cvbs_out_enabled(E_MUX_OUTPUTPORT outputPort)
{
#if 0
    if (outputPort == OUTPUT_PORT_CVBS1)
    {
        if ( MDrv_ReadByte(REG_ADC_ATOP_50_L ) & BIT(0) )
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
    else if (outputPort == OUTPUT_PORT_CVBS2)
    {
        if ( MDrv_ReadByte(REG_ADC_ATOP_52_L ) & BIT(0) )
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }

    }
    else
    {
        return FALSE;
    }
#else
    return FALSE;
#endif
}

void Hal_ADC_Set_Source(ADC_INPUTSOURCE_TYPE enADC_SourceType, E_MUX_INPUTPORT* enInputPortType, MS_U8 u8PortCount)
{
    ADC_SOURCE_TYPE stADCSrc;

    UNUSED(enInputPortType);
    UNUSED(u8PortCount);

    switch(enADC_SourceType)
    {
    case ADC_INPUTSOURCE_ONLY_RGB:
        stADCSrc = ADC_TABLE_SOURCE_RGB;
        break;

    case ADC_INPUTSOURCE_ONLY_YPBPR:
        stADCSrc = ADC_TABLE_SOURCE_YUV;
        break;

    case ADC_INPUTSOURCE_ONLY_MVOP:
        stADCSrc = ADC_TABLE_SOURCE_MVOP;
        break;


    case ADC_INPUTSOURCE_ONLY_SVIDEO:
        stADCSrc = ADC_TABLE_SOURCE_SVIDEO;
        break;

    case ADC_INPUTSOURCE_ONLY_CVBS:
        stADCSrc = ADC_TABLE_SOURCE_CVBS;
        break;

    case ADC_INPUTSOURCE_MULTI_RGB_MVOP:
        stADCSrc = ADC_TABLE_SOURCE_RGB_MV;
        break;


    case ADC_INPUTSOURCE_MULTI_RGB_ATV:
    case ADC_INPUTSOURCE_MULTI_RGB_CVBS:
        stADCSrc = ADC_TABLE_SOURCE_RGB_AV;
        break;

    case ADC_INPUTSOURCE_MULTI_YPBPR_MVOP:
        stADCSrc = ADC_TABLE_SOURCE_YUV_MV;
        break;



    case ADC_INPUTSOURCE_MULTI_YPBPR_ATV:
    case ADC_INPUTSOURCE_MULTI_YPBPR_CVBS:
        stADCSrc = ADC_TABLE_SOURCE_YUV_AV;
        break;


    case ADC_INPUTSOURCE_MULTI_MVOP_SVIDEO:
        stADCSrc = ADC_TABLE_SOURCE_MVOP_SV;
        break;

    case ADC_INPUTSOURCE_MULTI_MVOP_CVBS:
		stADCSrc = ADC_TABLE_SOURCE_MVOP_AV;
		break;

    case ADC_INPUTSOURCE_ONLY_DVI:
    case ADC_INPUTSOURCE_ONLY_ATV:
    case ADC_INPUTSOURCE_ONLY_SCART:
    case ADC_INPUTSOURCE_MULTI_RGB_DVI:
    case ADC_INPUTSOURCE_MULTI_RGB_SCART:
    case ADC_INPUTSOURCE_MULTI_YPBPR_DVI:
    case ADC_INPUTSOURCE_MULTI_YPBPR_SCART:
    case ADC_INPUTSOURCE_MULTI_MVOP_SCART:
    case ADC_INPUTSOURCE_MULTI_DVI_SCART:
    case ADC_INPUTSOURCE_MULTI_DVI_SVIDEO:
    case ADC_INPUTSOURCE_MULTI_DVI_ATV:
    case ADC_INPUTSOURCE_MULTI_DVI_CVBS:
    case ADC_INPUTSOURCE_MULTI_YPBPR_SVIDEO:
    case ADC_INPUTSOURCE_MULTI_RGB_SVIDEO:
    case ADC_INPUTSOURCE_MULTI_MVOP_DVI:
    default:
        printf("Unknown type =%u\n",enADC_SourceType);
        stADCSrc = ADC_TABLE_SOURCE_NUMS;
        break;
    }

    if(stADCSrc != ADC_TABLE_SOURCE_NUMS)
    {
        Hal_ADC_SetSourceAndPowerOn(stADCSrc);
    }

    MsOS_DelayTask(2);

}

void Hal_ADC_get_default_gain_offset(ADC_INPUTSOURCE_TYPE adc_src,XC_AdcGainOffsetSetting* ADCSetting)
{
    switch(adc_src)
    {
        case ADC_INPUTSOURCE_ONLY_RGB:      // RGB source
        default:
            ADCSetting->u16RedGain = ADCSetting->u16GreenGain = ADCSetting->u16BlueGain = 0x1000;
            ADCSetting->u16RedOffset = ADCSetting->u16GreenOffset = ADCSetting->u16BlueOffset = 0x0000;
            break;

        case ADC_INPUTSOURCE_ONLY_YPBPR:
            ADCSetting->u16RedGain = 0x1212;
            ADCSetting->u16GreenGain =  0x11aa;
            ADCSetting->u16BlueGain = 0x1212;
            ADCSetting->u16RedOffset = 0x800;   // 128
            ADCSetting->u16GreenOffset = 0x100; // 16
            ADCSetting->u16BlueOffset = 0x800;  // 128
            break;

        case ADC_INPUTSOURCE_ONLY_SCART:
            ADCSetting->u16RedGain = 0x1000;
            ADCSetting->u16GreenGain =  0x1000;
            ADCSetting->u16BlueGain = 0x1000;
            ADCSetting->u16RedOffset = 0x100;   // 16
            ADCSetting->u16GreenOffset = 0x100; // 16
            ADCSetting->u16BlueOffset = 0x100;  // 16
            break;
    }

}

MS_U16 Hal_ADC_get_center_gain(void)
{
    return 0x1000;
}

MS_U16 Hal_ADC_get_center_offset(void)
{
    return 0x0800;
}

MS_U8 Hal_ADC_get_offset_bit_cnt(void)
{
    return 13;
}
MS_U8 Hal_ADC_get_gain_bit_cnt(void)
{
    return 14;
}

void Hal_ADC_auto_adc_backup(void)
{
    //MS_U8 u8Bank;
    //FIXME: T3 need rewrite this function.
    //u8Bank = MDrv_ReadByte(BK_SELECT_00);

    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);
    _stAutoAdcSetting.u8L_BkAtop_01    = MDrv_ReadByte(L_BK_ADC_ATOP(0x01) );
    _stAutoAdcSetting.u8L_BkAtop_0C    = MDrv_ReadByte(L_BK_ADC_ATOP(0x0C) );
    _stAutoAdcSetting.u8L_BkAtop_2C    = MDrv_ReadByte(L_BK_ADC_ATOP(0x2C) );
    _stAutoAdcSetting.u8L_BkAtop_1F    = MDrv_ReadByte(L_BK_ADC_ATOP(0x1F) );
    _stAutoAdcSetting.u8H_BkAtop_2D    = MDrv_ReadByte(H_BK_ADC_ATOP(0x2D) );
    _stAutoAdcSetting.u8L_BkAtop_03    = MDrv_ReadByte(L_BK_ADC_ATOP(0x03) );
    _stAutoAdcSetting.u16L_BkAtop_05   = MDrv_Read2Byte(L_BK_ADC_ATOP(0x05) );
    _stAutoAdcSetting.u16L_BkAtop_5E   = MDrv_Read2Byte(L_BK_ADC_ATOP(0x5E) );

    _stAutoAdcSetting.u8H_BkChipTop_1F = MDrv_ReadByte(H_BK_CHIPTOP(0x1f));
    _stAutoAdcSetting.u8L_BkChipTop_55 = MDrv_ReadByte(L_BK_CHIPTOP(0x55));
    _stAutoAdcSetting.u8L_BkIpMux_1    = MDrv_ReadByte(L_BK_IPMUX(0x01) );
    //_stAutoAdcSetting.u8L_SC_BK1_21    = MDrv_ReadByte(L_BK_IP1F2(0x21) );
    _stAutoAdcSetting.u16SC_BK1_21    =  SC_R2BYTE(REG_SC_BK01_21_L);

    _stAutoAdcSetting.R_UserOffset =    MDrv_Read2Byte(REG_ADC_DTOP_52_L) & 0x1FFF;
    _stAutoAdcSetting.G_UserOffset =    MDrv_Read2Byte(REG_ADC_DTOP_57_L) & 0x1FFF;
    _stAutoAdcSetting.B_UserOffset =    MDrv_Read2Byte(REG_ADC_DTOP_5C_L) & 0x1FFF;

    _stAutoAdcSetting.R_BlankLevel =    MDrv_Read2Byte(REG_ADC_DTOP_50_L) & 0x0FFF;
    _stAutoAdcSetting.G_BlankLevel =    MDrv_Read2Byte(REG_ADC_DTOP_55_L) & 0x0FFF;
    _stAutoAdcSetting.B_BlankLevel =    MDrv_Read2Byte(REG_ADC_DTOP_5A_L) & 0x0FFF;

    _stAutoAdcSetting.u16BkAtop_1C     = MDrv_Read2Byte(L_BK_ADC_ATOP(0x1C) );

    // backup power setting
    _stAutoAdcSetting.u16BkAtop_04     = MDrv_Read2Byte(L_BK_ADC_ATOP(0x04) );
    _stAutoAdcSetting.u16BkAtop_05     = MDrv_Read2Byte(L_BK_ADC_ATOP(0x05) );
    _stAutoAdcSetting.u16BkAtop_06     = MDrv_Read2Byte(L_BK_ADC_ATOP(0x06) );

    _stAutoAdcSetting.u16BkDtop_01     = MDrv_Read2Byte(L_BK_ADC_DTOP(0x01) );
    _stAutoAdcSetting.u16BkDtop_02     = MDrv_Read2Byte(L_BK_ADC_DTOP(0x02) );
    _stAutoAdcSetting.u8L_BkDtop_06    = MDrv_ReadByte(L_BK_ADC_DTOP(0x06) );
    _stAutoAdcSetting.u16SC_BK1_02     = SC_R2BYTE(REG_SC_BK01_02_L);
    _stAutoAdcSetting.u16SC_BK1_03     = SC_R2BYTE(REG_SC_BK01_03_L);
    _stAutoAdcSetting.u16SC_BK1_04     = SC_R2BYTE(REG_SC_BK01_04_L);
    _stAutoAdcSetting.u16SC_BK1_05     = SC_R2BYTE(REG_SC_BK01_05_L);
    _stAutoAdcSetting.u16SC_BK1_06     = SC_R2BYTE(REG_SC_BK01_06_L);
    _stAutoAdcSetting.u16SC_BK1_07     = SC_R2BYTE(REG_SC_BK01_07_L);
    _stAutoAdcSetting.u16SC_BK1_0E     = SC_R2BYTE(REG_SC_BK01_0E_L);

    _stAutoAdcSetting.u16SC_BK12_01    = SC_R2BYTE(REG_SC_BK12_01_L);
    _stAutoAdcSetting.u16SC_BK12_03    = SC_R2BYTE(REG_SC_BK12_03_L);
    _stAutoAdcSetting.u16SC_BK12_04    = SC_R2BYTE(REG_SC_BK12_04_L);
    _stAutoAdcSetting.u16SC_BK12_0E    = SC_R2BYTE(REG_SC_BK12_0E_L);
    _stAutoAdcSetting.u16SC_BK12_0F    = SC_R2BYTE(REG_SC_BK12_0F_L);
    _stAutoAdcSetting.u16SC_BK12_16    = SC_R2BYTE(REG_SC_BK12_16_L);
    _stAutoAdcSetting.u16SC_BK12_17    = SC_R2BYTE(REG_SC_BK12_17_L);

    //scaling
    _stAutoAdcSetting.u16SC_BK02_04    = SC_R2BYTE(REG_SC_BK02_04_L);
    _stAutoAdcSetting.u16SC_BK02_05    = SC_R2BYTE(REG_SC_BK02_05_L);
    _stAutoAdcSetting.u16SC_BK02_08    = SC_R2BYTE(REG_SC_BK02_08_L);
    _stAutoAdcSetting.u16SC_BK02_09    = SC_R2BYTE(REG_SC_BK02_09_L);
    _stAutoAdcSetting.u16SC_BK23_07    = SC_R2BYTE(REG_SC_BK23_07_L);
    _stAutoAdcSetting.u16SC_BK23_08    = SC_R2BYTE(REG_SC_BK23_08_L);
    _stAutoAdcSetting.u16SC_BK23_09    = SC_R2BYTE(REG_SC_BK23_09_L);
    _stAutoAdcSetting.u16SC_BK23_0A    = SC_R2BYTE(REG_SC_BK23_0A_L);

    //DNR base
    _stAutoAdcSetting.u32SC_BK12_08    = SC_R4BYTE(REG_SC_BK12_08_L);
    _stAutoAdcSetting.u32SC_BK12_0A    = SC_R4BYTE(REG_SC_BK12_0A_L);
    _stAutoAdcSetting.u32SC_BK12_0C    = SC_R4BYTE(REG_SC_BK12_0C_L);
    //OPM Base
    _stAutoAdcSetting.u32SC_BK12_10    = SC_R4BYTE(REG_SC_BK12_10_L);
    _stAutoAdcSetting.u32SC_BK12_12    = SC_R4BYTE(REG_SC_BK12_12_L);
    _stAutoAdcSetting.u32SC_BK12_14    = 0;//SC_R4BYTE(REG_SC_BK12_14_L);

    _stAutoAdcSetting.u16SC_BK06_01    = SC_R2BYTE(REG_SC_BK06_01_L);
    _stAutoAdcSetting.u16SC_BK06_21    = SC_R2BYTE(REG_SC_BK06_21_L);
    _stAutoAdcSetting.u16SC_BK12_07    = SC_R2BYTE(REG_SC_BK12_07_L);
    _stAutoAdcSetting.u16SC_BK12_44    = SC_R2BYTE(REG_SC_BK12_44_L);
    _stAutoAdcSetting.u16SC_BK12_47    = SC_R2BYTE(REG_SC_BK12_47_L);
    _stAutoAdcSetting.u16SC_BK12_1A     = SC_R2BYTE(REG_SC_BK12_1A_L);
    _stAutoAdcSetting.u16SC_BK12_1B     = SC_R2BYTE(REG_SC_BK12_1B_L);

    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_VOP);
    _stAutoAdcSetting.u16SC_BK10_19   = SC_R2BYTE(REG_SC_BK10_19_L);

    MDrv_WriteByteMask(L_BK_ADC_ATOP(0x5c), 0x30,0x30);  //ldo

    //MDrv_WriteByte(BK_SELECT_00, u8Bank);
}

void Hal_ADC_auto_adc_restore(void)
{
    //MS_U8 u8Bank;

   // u8Bank = MDrv_ReadByte(BK_SELECT_00);
   // MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);
    MDrv_WriteByte(L_BK_ADC_ATOP(0x01), _stAutoAdcSetting.u8L_BkAtop_01);
    MDrv_WriteByte(L_BK_ADC_ATOP(0x0C), _stAutoAdcSetting.u8L_BkAtop_0C);
    MDrv_WriteByte(L_BK_ADC_ATOP(0x2C), _stAutoAdcSetting.u8L_BkAtop_2C);
    MDrv_WriteByte(L_BK_ADC_ATOP(0x1F), _stAutoAdcSetting.u8L_BkAtop_1F);
    MDrv_WriteByte(H_BK_ADC_ATOP(0x2D), _stAutoAdcSetting.u8H_BkAtop_2D);
    MDrv_WriteByte(L_BK_ADC_ATOP(0x03), _stAutoAdcSetting.u8L_BkAtop_03);
    MDrv_Write2Byte(L_BK_ADC_ATOP(0x05), _stAutoAdcSetting.u16L_BkAtop_05);
    MDrv_Write2Byte(L_BK_ADC_ATOP(0x5E), _stAutoAdcSetting.u16L_BkAtop_5E);

    // ADC clock gen
    MDrv_Write2Byte(L_BK_ADC_DTOP(0x01), _stAutoAdcSetting.u16BkDtop_01);
    MDrv_Write2Byte(L_BK_ADC_DTOP(0x02), _stAutoAdcSetting.u16BkDtop_02);
    MDrv_WriteByte(L_BK_ADC_DTOP(0x06), _stAutoAdcSetting.u8L_BkDtop_06);

    // Rstore Offset
    W2BYTEMSK(REG_ADC_DTOP_52_L, _stAutoAdcSetting.R_UserOffset, 0x1FFF);
    W2BYTEMSK(REG_ADC_DTOP_57_L, _stAutoAdcSetting.G_UserOffset, 0x1FFF);
    W2BYTEMSK(REG_ADC_DTOP_5C_L, _stAutoAdcSetting.B_UserOffset, 0x1FFF);

    // Restore black level
    W2BYTEMSK(REG_ADC_DTOP_50_L, _stAutoAdcSetting.R_BlankLevel, 0x0FFF);
    W2BYTEMSK(REG_ADC_DTOP_55_L, _stAutoAdcSetting.G_BlankLevel, 0x0FFF);
    W2BYTEMSK(REG_ADC_DTOP_5A_L, _stAutoAdcSetting.B_BlankLevel, 0x0FFF);

    MDrv_WriteByte(H_BK_CHIPTOP(0x1f), _stAutoAdcSetting.u8H_BkChipTop_1F);
    MDrv_WriteByte(L_BK_CHIPTOP(0x55), _stAutoAdcSetting.u8L_BkChipTop_55);
    MDrv_WriteByte(L_BK_IPMUX(0x01), _stAutoAdcSetting.u8L_BkIpMux_1);
    //MDrv_WriteByte(L_BK_IP1F2(0x21), _stAutoAdcSetting.u8L_SC_BK1_21);
    SC_W2BYTE(REG_SC_BK01_21_L, _stAutoAdcSetting.u16SC_BK1_21);

    MDrv_Write2Byte(L_BK_ADC_ATOP(0x1C), _stAutoAdcSetting.u16BkAtop_1C);

    // Restore Power
    MDrv_Write2Byte(L_BK_ADC_ATOP(0x04), _stAutoAdcSetting.u16BkAtop_04);
    MDrv_Write2Byte(L_BK_ADC_ATOP(0x05), _stAutoAdcSetting.u16BkAtop_05);
    MDrv_Write2Byte(L_BK_ADC_ATOP(0x06), _stAutoAdcSetting.u16BkAtop_06);

    SC_W2BYTE(REG_SC_BK01_02_L, _stAutoAdcSetting.u16SC_BK1_02);
    SC_W2BYTE(REG_SC_BK01_03_L, _stAutoAdcSetting.u16SC_BK1_03);
    SC_W2BYTE(REG_SC_BK01_04_L, _stAutoAdcSetting.u16SC_BK1_04);
    SC_W2BYTE(REG_SC_BK01_05_L, _stAutoAdcSetting.u16SC_BK1_05);
    SC_W2BYTE(REG_SC_BK01_06_L, _stAutoAdcSetting.u16SC_BK1_06);
    SC_W2BYTE(REG_SC_BK01_07_L, _stAutoAdcSetting.u16SC_BK1_07);
    SC_W2BYTE(REG_SC_BK01_0E_L, _stAutoAdcSetting.u16SC_BK1_0E);
    /*
    MDrv_Write2Byte(L_BK_IP1F2(0x02), _stAutoAdcSetting.u16SC_BK1_02);
    MDrv_Write2Byte(L_BK_IP1F2(0x03), _stAutoAdcSetting.u16SC_BK1_03);
    MDrv_Write2Byte(L_BK_IP1F2(0x04), _stAutoAdcSetting.u16SC_BK1_04);
    MDrv_Write2Byte(L_BK_IP1F2(0x05), _stAutoAdcSetting.u16SC_BK1_05);
    MDrv_Write2Byte(L_BK_IP1F2(0x06), _stAutoAdcSetting.u16SC_BK1_06);
    MDrv_Write2Byte(L_BK_IP1F2(0x07), _stAutoAdcSetting.u16SC_BK1_07);
    MDrv_Write2Byte(L_BK_IP1F2(0x0E), _stAutoAdcSetting.u16SC_BK1_0E);
    */
    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_SCMI);
    SC_W2BYTE(REG_SC_BK12_01_L, _stAutoAdcSetting.u16SC_BK12_01);
    SC_W2BYTE(REG_SC_BK12_03_L, _stAutoAdcSetting.u16SC_BK12_03);
    SC_W2BYTE(REG_SC_BK12_04_L, _stAutoAdcSetting.u16SC_BK12_04);
    SC_W2BYTE(REG_SC_BK12_0E_L, _stAutoAdcSetting.u16SC_BK12_0E);
    SC_W2BYTE(REG_SC_BK12_0F_L, _stAutoAdcSetting.u16SC_BK12_0F);
    SC_W2BYTE(REG_SC_BK12_16_L, _stAutoAdcSetting.u16SC_BK12_16);
    SC_W2BYTE(REG_SC_BK12_17_L, _stAutoAdcSetting.u16SC_BK12_17);

    //scaling
    SC_W2BYTE(REG_SC_BK02_04_L, _stAutoAdcSetting.u16SC_BK02_04);
    SC_W2BYTE(REG_SC_BK02_05_L, _stAutoAdcSetting.u16SC_BK02_05);
    SC_W2BYTE(REG_SC_BK02_08_L, _stAutoAdcSetting.u16SC_BK02_08);
    SC_W2BYTE(REG_SC_BK02_09_L, _stAutoAdcSetting.u16SC_BK02_09);
    SC_W2BYTE(REG_SC_BK23_07_L, _stAutoAdcSetting.u16SC_BK23_07);
    SC_W2BYTE(REG_SC_BK23_08_L, _stAutoAdcSetting.u16SC_BK23_08);
    SC_W2BYTE(REG_SC_BK23_09_L, _stAutoAdcSetting.u16SC_BK23_09);
    SC_W2BYTE(REG_SC_BK23_0A_L, _stAutoAdcSetting.u16SC_BK23_0A);

    //DNR,OPM Base
    SC_W4BYTE(REG_SC_BK12_08_L, _stAutoAdcSetting.u32SC_BK12_08);
    SC_W4BYTE(REG_SC_BK12_0A_L, _stAutoAdcSetting.u32SC_BK12_0A);
    SC_W4BYTE(REG_SC_BK12_0C_L, _stAutoAdcSetting.u32SC_BK12_0C);
    SC_W4BYTE(REG_SC_BK12_10_L, _stAutoAdcSetting.u32SC_BK12_10);
    SC_W4BYTE(REG_SC_BK12_12_L, _stAutoAdcSetting.u32SC_BK12_12);
//    SC_W4BYTE(REG_SC_BK12_14_L, _stAutoAdcSetting.u32SC_BK12_14);
    SC_W2BYTE(REG_SC_BK12_1A_L, _stAutoAdcSetting.u16SC_BK12_1A);
    SC_W2BYTE(REG_SC_BK12_1B_L, _stAutoAdcSetting.u16SC_BK12_1B);

    SC_W2BYTE(REG_SC_BK06_01_L, _stAutoAdcSetting.u16SC_BK06_01);
    SC_W2BYTE(REG_SC_BK06_21_L, _stAutoAdcSetting.u16SC_BK06_21);
    SC_W2BYTE(REG_SC_BK12_07_L, _stAutoAdcSetting.u16SC_BK12_07);
    SC_W2BYTE(REG_SC_BK12_44_L, _stAutoAdcSetting.u16SC_BK12_44);
    SC_W2BYTE(REG_SC_BK12_47_L, _stAutoAdcSetting.u16SC_BK12_47);
/*
    MDrv_Write2Byte(L_BK_SCMI(0x01), _stAutoAdcSetting.u16SC_BK12_01);
    MDrv_Write2Byte(L_BK_SCMI(0x03), _stAutoAdcSetting.u16SC_BK12_03);
    MDrv_Write2Byte(L_BK_SCMI(0x04), _stAutoAdcSetting.u16SC_BK12_04);
    MDrv_Write2Byte(L_BK_SCMI(0x0E), _stAutoAdcSetting.u16SC_BK12_0E);
    MDrv_Write2Byte(L_BK_SCMI(0x0F), _stAutoAdcSetting.u16SC_BK12_0F);
    MDrv_Write2Byte(L_BK_SCMI(0x16), _stAutoAdcSetting.u16SC_BK12_16);
    MDrv_Write2Byte(L_BK_SCMI(0x17), _stAutoAdcSetting.u16SC_BK12_17);
*/
    MDrv_WriteByteMask(L_BK_ADC_ATOP(0x5c), 0x00,0x70);

    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_VOP);
    //MDrv_WriteByte(L_BK_VOP(0x19),_stAutoAdcSetting.u8L_SC_BK10_19  );
    SC_W2BYTE(REG_SC_BK10_19_L, _stAutoAdcSetting.u16SC_BK10_19);

    //MDrv_WriteByte(BK_SELECT_00, u8Bank);
}

MS_BOOL Hal_ADC_is_scart_rgb(void)
{
    MS_BOOL bRGB;
    MS_U8 u8Flag;

    u8Flag = MDrv_ReadByte(H_BK_ADC_ATOP(0x45));

    if((u8Flag & 0x50) == 0x10)
        bRGB = TRUE;
    else
        bRGB = FALSE;

    MDrv_WriteByteMask(H_BK_ADC_ATOP(0x45), 0x20, 0x20);

    return bRGB;
}

MS_U16 Hal_ADC_get_clk (void)
{
    //u16Value -= 3; // actual - 3
 //ADC PLL divider ratio (htotal-3), (write sequence LSB -> MSB)
    return (R2BYTEMSK(REG_ADC_DTOP_00_L, 0xFFFF)+3);//(REG_ADC_DTOP_00_L, u16Value);
}

MS_BOOL Hal_ADC_set_SoG_Calibration(void)
{
    W2BYTEMSK(REG_ADC_ATOP_39_L, 0, BIT(13) );
    W2BYTEMSK(REG_ADC_ATOP_3C_L, 0, BIT(13) );

    OS_DELAY_TASK(1);

    W2BYTEMSK(REG_ADC_ATOP_39_L, BIT(13), BIT(13) );
    W2BYTEMSK(REG_ADC_ATOP_3C_L, BIT(13), BIT(13) );

    return TRUE;
}
/******************************************************************************/
///This function return SOG level range
///@param u32Min \b OUT: min of SOG level
///@param u32Max \b OUT: max of SOG level
///@param u32Recommend_value \b OUT: recommend value
/******************************************************************************/
void Hal_ADC_get_SoG_LevelRange(MS_U32 *u32Min, MS_U32 *u32Max, MS_U32 *u32Recommend_value)
{
    *u32Min = 0;
    *u32Max = 255;
    *u32Recommend_value = 0x68;
}

/******************************************************************************/
///This function Set SOG Level
///@param u32Value \b IN: set SOG value
/******************************************************************************/
void Hal_ADC_set_SoG_Level(MS_U32 u32Value)
{
    W2BYTEMSK(REG_ADC_ATOP_3B_L, (MS_U16)u32Value,LBMASK);
}

/******************************************************************************/
///select RGB input pipe delay, this reg will decide the H start of SCART RGB
///@param u32Value \b IN: set PIPE Delay value
/******************************************************************************/
void Hal_ADC_set_RGB_PIPE_Delay(MS_U8 u8Value)
{
    W2BYTEMSK(REG_ADC_ATOP_43_L, (u8Value<<8), 0x7F00);
}

/******************************************************************************/
///This function set Scart RGB Sync on Green clamp delay.
///@param u16Value \b IN: set clamp delay value
/******************************************************************************/
void Hal_ADC_set_ScartRGB_SOG_ClampDelay(MS_U16 u16Clpdly, MS_U16 u16Caldur)
{
    W2BYTEMSK(REG_ADC_DTOP_17_L, u16Clpdly, 0x0FFF);
    W2BYTEMSK(REG_ADC_DTOP_18_L, u16Caldur, 0x00FF);
}

/******************************************************************************/
///This function set YPbPr Loose LPF.
///@param benable \b IN: enable or disable
/******************************************************************************/
void Hal_ADC_set_YPbPrLooseLPF(MS_BOOL benable)
{
    bIsYPbPrLooseLPF = benable;
}

/******************************************************************************/
///This function set SOG BW
///@param u16value \b IN: value of SOG BW
/******************************************************************************/
void Hal_ADC_Set_SOGBW(MS_U16 u16value)
{
    W2BYTEMSK(REG_ADC_ATOP_39_L, u16value, 0x1F);
}

/******************************************************************************/
///Set negative clamping duration..
///@param u16Value \b IN: set clamp delay value
/******************************************************************************/
void Hal_ADC_dtop_iClampDuration_setting(MS_U16 u16value)
{
    W2BYTEMSK(REG_ADC_DTOP_18_L, u16value, LBMASK);
}

/******************************************************************************/
///Set postive clamping duration..
///@param u16Value \b IN: set clamp delay value
/******************************************************************************/
void Hal_ADC_dtop_vClampDuration_setting(MS_U16 u16value)
{
    W2BYTEMSK(REG_ADC_DTOP_08_L, u16value, LBMASK);
}

/******************************************************************************/
///This function set HW mode calibration
///@param benable \b IN: enable or disable
/******************************************************************************/
void Hal_ADC_enable_HWCalibration(MS_BOOL benable)
{
    _bEnableHWCalibration = benable;
}

/******************************************************************************/
///This function return R or Pr fixed gain
///@param eADC_Source   \b IN: source type
///@param u16value      \b OUT: fixed gain value
/******************************************************************************/
void Hal_ADC_get_fixed_gain_r(ADC_INPUTSOURCE_TYPE eADC_Source, MS_U16 *u16value)
{
    if (ADC_INPUTSOURCE_ONLY_RGB == eADC_Source ||
        ADC_INPUTSOURCE_ONLY_SCART == eADC_Source)
    {
        *u16value = 0x16A5;
    }
    else if (ADC_INPUTSOURCE_ONLY_YPBPR == eADC_Source)
    {
        *u16value = 0x13E4; //as PbPr  ((240 - 16) * 4) / (255 * 4)
    }
    else
    {
         // Undefined.
    }
}

/******************************************************************************/
///This function return G or Y fixed gain
///@param eADC_Source   \b IN: source type
///@param u16value      \b OUT: fixed gain value
/******************************************************************************/
void Hal_ADC_get_fixed_gain_g(ADC_INPUTSOURCE_TYPE eADC_Source, MS_U16 *u16value)
{
    if (ADC_INPUTSOURCE_ONLY_RGB == eADC_Source ||
        ADC_INPUTSOURCE_ONLY_SCART == eADC_Source)
    {
        *u16value = 0x16A5;
    }
    else if (ADC_INPUTSOURCE_ONLY_YPBPR == eADC_Source)
    {
        *u16value = 0x1372;  // as Y  ((235 - 16) * 4) / (255 * 4)
    }
    else
    {
         // Undefined.
    }
}

/******************************************************************************/
///This function return B or Pb fixed gain
///@param eADC_Source   \b IN: source type
///@param u16value      \b OUT: fixed gain value
/******************************************************************************/
void Hal_ADC_get_fixed_gain_b(ADC_INPUTSOURCE_TYPE eADC_Source, MS_U16 *u16value)
{
    if (ADC_INPUTSOURCE_ONLY_RGB == eADC_Source ||
        ADC_INPUTSOURCE_ONLY_SCART == eADC_Source)
    {
        *u16value = 0x16A5;
    }
    else if (ADC_INPUTSOURCE_ONLY_YPBPR == eADC_Source)
    {
        *u16value = 0x13E4; //as PbPr  ((240 - 16) * 4) / (255 * 4)
    }
    else
    {
         // Undefined.
    }
}

#undef MHAL_ADC_C

