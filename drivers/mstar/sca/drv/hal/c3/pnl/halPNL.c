#ifndef _HAL_PNL_C_
#define _HAL_PNL_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/kernel.h>
#include "ms_platform.h"
#endif

#include "MsCommon.h"
#include "MsTypes.h"
#include "drvPNL.h"
#include "halPNL.h"
#include "pnl_hwreg_utility2.h"
#include "MsOS.h"
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define UNUSED(x)       (x=x)
#if 1

#ifdef MSOS_TYPE_CE
#define HAL_PNL_DBG(_dbgSwitch_, _fmt, _args, ...)      { if((_dbgSwitch_ & _u16PnlDbgSwitch) != 0) printf("PNL:"_fmt, ##_args); }
#else
#define HAL_PNL_DBG(_dbgSwitch_, _fmt, _args...)      { if((_dbgSwitch_ & _u16PnlDbgSwitch) != 0) printf("PNL:"_fmt, ##_args); }
#endif

#define HAL_MOD_CAL_DBG(x)    //x
#else
#define HAL_PNL_DBG(_dbgSwitch_, _fmt, _args...)      { }
#endif

#define DAC_LPLL_ICTRL     0x0002
#define LVDS_LPLL_ICTRL    0x0001

//Get MOD calibration time
#define MOD_CAL_TIMER   FALSE

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static MS_U16 _u16PnlDbgSwitch = 0;

// Output swing = 150mV + Code * 10mV.
// Designer recommand value. 0x15 = 21 = 250mv
static MS_U16  _u16PnlDefault_SwingLevel = 250;
static PNL_OUTPUT_MODE   _eDrvPnlInitOptions = E_PNL_OUTPUT_CLK_DATA;
MS_U8 _u8MOD_CALI_TARGET = 0x01;     ///< MOD_REG(0x7D),[10:9]00: 250mV ,01: 350mV ,10:300mV ,11:200mV
MS_S8 _usMOD_CALI_OFFSET = 0;        ///< MOD_REG(0x7D),[5:0]+ _usMOD_CALI_OFFSET
MS_U8 _u8MOD_CALI_VALUE  = 0x15;     /// Final value

MS_U8 _u8MOD_LVDS_Pair_Shift = 0;       ///< 0:default setting, LVDS pair Shift
MS_U8 _u8MOD_LVDS_Pair_Type = 0;        ///< 0:default setting, LVDS data differential pair
//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Function
//-------------------------------------------------------------------------------------------------
MS_BOOL MHal_PNL_SetDbgLevel(MS_U16 u16DbgSwitch)       ///< Set debug level
{
    _u16PnlDbgSwitch = u16DbgSwitch;
    UNUSED(_u16PnlDbgSwitch);
    return TRUE;
}

/**
*   @brief: Power On MOD. but not mutex protected
*
*/
MS_U8 MHal_MOD_PowerOn(MS_BOOL bEn, MS_U8 u8LPLL_Type,MS_U8 DualModeType, MS_U16 u16OutputCFG0_7, MS_U16 u16OutputCFG8_15, MS_U16 u16OutputCFG16_21)
{
    if( bEn )
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_37_L, 0x00, BIT(8));

        //analog MOD power down. 1: power down, 0: power up
        // For Mod2 no output signel
        ///////////////////////////////////////////////////

        //2. Power on MOD (current and regulator)
        MOD_W2BYTEMSK(REG_MOD_BK00_78_L, 0x00 , BIT(0) | BIT(7) );

        // 3. 4. 5.
        MHal_Output_LVDS_Pair_Setting(DualModeType, u16OutputCFG0_7, u16OutputCFG8_15, u16OutputCFG16_21);


        //enable ib, enable ck
        MOD_W2BYTEMSK(REG_MOD_BK00_77_L, (BIT(1) | BIT(0)), (BIT(1) | BIT(0)));

        // clock gen of dot-mini
        if(u8LPLL_Type == E_PNL_TYPE_MINILVDS)
        {
            MOD_W2BYTE(REG_MOD_BK00_20_L, 0x4400);
        }
        else
        {
            MOD_W2BYTE(REG_MOD_BK00_20_L, 0x0000);
        }
    }
    else
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_37_L, BIT(8), BIT(8));
        if(u8LPLL_Type !=E_PNL_TYPE_MINILVDS)
        {
            MOD_W2BYTEMSK(REG_MOD_BK00_78_L, 0x00, BIT(0));                              //analog MOD power down. 1: power down, 0: power up
        }

        MOD_W2BYTEMSK(REG_MOD_BK00_77_L, 0, (BIT(1) | BIT(0) ));                           //enable ib, enable ck

        // clock gen of dot-mini
        MOD_W2BYTE(REG_MOD_BK00_20_L, 0x1100);
    }
    return 1;
}

/**
*   @brief: Setup the PVDD power 1:2.5V, 0:3.3V
*
*/
void MHal_MOD_PVDD_Power_Setting(MS_BOOL bIs2p5)
{
    MOD_W2BYTEMSK(REG_MOD_BK00_37_L, ((bIs2p5)? BIT(6):0), BIT(6));    //MOD PVDD=1: 2.5,PVDD=0: 3.3
}

/**
*   @brief: Get current platform is DAC out or not
*
*/
MS_BOOL MHal_PNL_GetDACOut()
{
    MS_BOOL bRet = FALSE;
    return   bRet;
}

void MHal_PNL_TCON_Init(void)
{

}

void MHal_Shift_LVDS_Pair(MS_U8 Type)
{
    if(Type == 1)
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_42_L, (BIT(7) | BIT(6)), (BIT(7) | BIT(6))); // shift_lvds_pair, set LVDS Mode3
    }
    else
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_42_L, 0x0000, (BIT(7) | BIT(6)));
    }

}

void MHal_Output_LVDS_Pair_Setting(MS_U8 Type, MS_U16 u16OutputCFG0_7, MS_U16 u16OutputCFG8_15, MS_U16 u16OutputCFG16_21)
{
    // Disable OP
    MOD_W2BYTEMSK(REG_MOD_BK00_78_L, BIT(1) , BIT(1) );

    if(Type == LVDS_DUAL_OUTPUT_SPECIAL )
    {
        MOD_W2BYTE(REG_MOD_BK00_6D_L, 0x0555);
        MOD_W2BYTE(REG_MOD_BK00_6E_L, 0x1554);
        MOD_W2BYTEMSK(REG_MOD_BK00_6F_L, 0x0000, 0x000F);
    }
    else if(Type == LVDS_SINGLE_OUTPUT_A)
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_6D_L, 0x5550, 0xFFF0);
        MOD_W2BYTE(REG_MOD_BK00_6E_L, 0x0555);
        MOD_W2BYTEMSK(REG_MOD_BK00_6F_L, 0x0000, 0x000F);
    }
    else if( Type == LVDS_SINGLE_OUTPUT_B)
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_6D_L, 0x5550, 0xFFF0);
        MOD_W2BYTE(REG_MOD_BK00_6E_L, 0x0555);
        MOD_W2BYTEMSK(REG_MOD_BK00_6F_L, 0x0000, 0x000F);
    }
    else if( Type == LVDS_OUTPUT_User)
    {
        MOD_W2BYTE(REG_MOD_BK00_6D_L, u16OutputCFG0_7);
        MOD_W2BYTE(REG_MOD_BK00_6E_L, u16OutputCFG8_15);
        MOD_W2BYTEMSK(REG_MOD_BK00_6F_L, u16OutputCFG16_21, 0x000F);
    }
    else
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_6D_L, 0x5550, 0xFFF0);
        MOD_W2BYTE(REG_MOD_BK00_6E_L, 0x0555);
        MOD_W2BYTEMSK(REG_MOD_BK00_6F_L, 0x0000, 0x000F);
    }

    MsOS_DelayTask(2);

    // Enable OP
    MOD_W2BYTEMSK(REG_MOD_BK00_78_L, 0x00 , BIT(1) );

}

void MHal_PQ_Clock_Gen_For_Gamma(void)
{
    W2BYTEMSK(REG_CLKGEN0_52_L, 0x00, 0x07);
}

void MHal_VOP_SetGammaMappingMode(MS_U8 u8Mapping)
{

    if(u8Mapping & GAMMA_MAPPING)
    {
        SC_W2BYTEMSK(REG_SC_BK10_74_L, (u8Mapping & GAMMA_10BIT_MAPPING)? BIT(4):0, BIT(4));
    }
    else
    {
        PNL_ASSERT(0, "Invalid eSupportGammaMapMode [%d] Passed to [%s], please make sure the u8Mapping[%d] is valid\n.",
                       u8Mapping, __FUNCTION__, u8Mapping);
    }
}

MS_BOOL Hal_VOP_Is_GammaMappingMode_enable(void)
{
    return SC_R2BYTEMSK(REG_SC_BK10_74_L, BIT(15));
}

// After A5, 8 bit mode only support burst write!!!
MS_BOOL Hal_VOP_Is_GammaSupportSignalWrite(DRVPNL_GAMMA_MAPPEING_MODE u8Mapping)
{
    if( u8Mapping == E_DRVPNL_GAMMA_10BIT_MAPPING )
        return TRUE;
    else
        return FALSE;
}


void hal_PNL_WriteGamma12Bit(MS_U8 u8Channel, MS_BOOL bBurstWrite, MS_U16 u16Addr, MS_U16 u16GammaValue)
{
    MS_U16 u16Delay = 0xFFFF;

    PNL_DBG(PNL_DBGLEVEL_GAMMA, "Write [ch %d][addr 0x%x]: 0x%x \n", u8Channel, u16Addr, u16GammaValue);

    if (!bBurstWrite )
    {
        while (SC_R2BYTEMSK(REG_SC_BK10_6D_L, 0xE0) && (--u16Delay));          // Check whether the Write chanel is ready

        PNL_ASSERT(u16Delay > 0, "%s\n", "WriteGamma timeout");

        SC_W2BYTEMSK(REG_SC_BK10_6C_L, u16Addr, 0x3FF);                          // set address port
        SC_W2BYTEMSK((REG_SC_BK10_6E_L + u8Channel *2), u16GammaValue, 0xFFF);      // Set channel data

        // kick off write
        switch(u8Channel)
        {
            case 0:  // Red
                SC_W2BYTEMSK(REG_SC_BK10_6D_L, BIT(5), BIT(5));
                break;

            case 1:  // Green
                SC_W2BYTEMSK(REG_SC_BK10_6D_L, BIT(6), BIT(6));
                break;

            case 2:  // Blue
                SC_W2BYTEMSK(REG_SC_BK10_6D_L, BIT(7), BIT(7));
                break;
        }

        while (SC_R2BYTEMSK(REG_SC_BK10_6D_L, 0xE0) && (--u16Delay));          // Check whether the Write chanel is ready
    }
    else
    {
        SC_W2BYTEMSK(REG_SC_BK10_6E_L, u16GammaValue, 0xFFF);
    }

    PNL_ASSERT(u16Delay > 0, "%s\n", "WriteGamma timeout");

}


void hal_PNL_SetMaxGammaValue( MS_U8 u8Channel, MS_U16 u16MaxGammaValue)
{
    PNL_DBG(PNL_DBGLEVEL_GAMMA, "Max gamma of %d is 0x%x\n", u8Channel, u16MaxGammaValue);

    SC_W2BYTEMSK((REG_SC_BK10_7A_L + 4 * u8Channel), u16MaxGammaValue, 0xFFF);           // max. base 0
    SC_W2BYTEMSK((REG_SC_BK10_7B_L + 4 * u8Channel), u16MaxGammaValue, 0xFFF);           // max. base 1
}



static void _Hal_PNL_WriteGamma12Bit(MS_U8 u8Channel, MS_U16 u16Addr, MS_U16 u16GammaValue)
{
    MS_U16 u16Delay = 0xFFFF;

    PNL_DBG(PNL_DBGLEVEL_GAMMA, "Write [ch %d][addr 0x%x]: 0x%x \n", u8Channel, u16Addr, u16GammaValue);

    while (SC_R2BYTEMSK(REG_SC_BK10_6D_L, 0xE0) && (--u16Delay));          // Check whether the Write chanel is ready
    PNL_ASSERT(u16Delay > 0, "%s\n", "WriteGamma timeout");

    SC_W2BYTEMSK(REG_SC_BK10_6C_L, u16Addr, 0x3FF);                          // set address port
    SC_W2BYTEMSK((REG_SC_BK10_6E_L + u8Channel *2), u16GammaValue, 0xFFFF);      // Set channel data

    // kick off write
    switch(u8Channel)
    {
        case 0:  // Red
            SC_W2BYTEMSK(REG_SC_BK10_6D_L, BIT(5), BIT(5));
            break;

        case 1:  // Green
            SC_W2BYTEMSK(REG_SC_BK10_6D_L, BIT(6), BIT(6));
            break;

        case 2:  // Blue
            SC_W2BYTEMSK(REG_SC_BK10_6D_L, BIT(7), BIT(7));
            break;
    }

    while (SC_R2BYTEMSK(REG_SC_BK10_6D_L, 0xE0) && (--u16Delay));          // Check whether the Write chanel is ready
    PNL_ASSERT(u16Delay > 0, "%s\r\n", "WriteGamma timeout");

}

#if 0
static void _Hal_PNL_WriteGamma12Bit_SC1(MS_U8 u8Channel, MS_U16 u16Addr, MS_U16 u16GammaValue)
{
    MS_U16 u16Delay = 0xFFFF;

    PNL_DBG(PNL_DBGLEVEL_GAMMA, "Write [ch %d][addr 0x%x]: 0x%x \n", u8Channel, u16Addr, u16GammaValue);

    while (SC_R2BYTEMSK(REG_SC1_BK10_6D_L, 0xE0) && (--u16Delay));          // Check whether the Write chanel is ready
    PNL_ASSERT(u16Delay > 0, "%s\n", "WriteGamma timeout");

    SC_W2BYTEMSK(REG_SC1_BK10_6C_L, u16Addr, 0x3FF);                          // set address port
    SC_W2BYTEMSK((REG_SC1_BK10_6E_L + u8Channel *2), u16GammaValue, 0xFFFF);      // Set channel data

    // kick off write
    switch(u8Channel)
    {
        case 0:  // Red
            SC_W2BYTEMSK(REG_SC1_BK10_6D_L, BIT(5), BIT(5));
            break;

        case 1:  // Green
            SC_W2BYTEMSK(REG_SC1_BK10_6D_L, BIT(6), BIT(6));
            break;

        case 2:  // Blue
            SC_W2BYTEMSK(REG_SC1_BK10_6D_L, BIT(7), BIT(7));
            break;
    }

    while (SC_R2BYTEMSK(REG_SC1_BK10_6D_L, 0xE0) && (--u16Delay));          // Check whether the Write chanel is ready
    PNL_ASSERT(u16Delay > 0, "%s\r\n", "WriteGamma timeout");

}
#endif

static MS_U16 _Hal_PNL_GetGamma12Data(MS_U8 *pu8GammaTable, MS_U16 u16Index)
{
    MS_U16 u16Data, u16DataOffset;

    if(u16Index == 256)
        return 0xFFFF;

    u16DataOffset = u16Index / 2 *3;

    if(u16Index & 1)
    {
        u16Data = (((MS_U16)pu8GammaTable[u16DataOffset+2]) << 4) + (((MS_U16)pu8GammaTable[u16DataOffset]) >> 4);
    }
    else
    {
        u16Data = (((MS_U16)pu8GammaTable[u16DataOffset+1]) << 4) + (((MS_U16)pu8GammaTable[u16DataOffset]) & 0x0F);
    }

    return u16Data;
}

/////////////////////////////////////////////////////////////////////////////
// Gamma format (12 bit LUT)
//      0, 1, 2, 3, ..., NumOfLevel, totally N Sets of tNormalGammaR/G/B[],
//      1 set uses 2 bytes of memory.
//
// [T2 and before ] N = 256
// [T3]             N = 256 or 1024
// ______________________________________________________________________________
//  Byte | 0         1           2               n-1        n
//    [G1|G0]       [G0]       [G1] . ...... .  [Gmax]    [Gmax]
//    3:0  3:0      11:4       11:4              3:0       11:4
//

void Hal_PNL_Set12BitGammaPerChannel( MS_U8 u8Channel, MS_U8 * u8Tab, DRVPNL_GAMMA_MAPPEING_MODE GammaMapMode )
{
    MS_U16 u16Addr            = 0;
    MS_U16 u16GammaValue      = 0;
    MS_U16 u16MaxGammaValue   = 0;
    MS_U16 u16NumOfLevel = GammaMapMode == E_DRVPNL_GAMMA_8BIT_MAPPING ? 128 : 128;
    MS_U16 u16InputData1, u16InputData2, u16OutRef;
    MS_U8  u8OutDelta, u8OutData1, u8OutData2;

    // write gamma table per one channel
    //printf("Gamma u8Channel:%04x\r\n",u8Channel );


    for(u16Addr=0; u16Addr < u16NumOfLevel; u16Addr++)
    {
        u16InputData1 = _Hal_PNL_GetGamma12Data(u8Tab, u16Addr*2);
        u16InputData2 = _Hal_PNL_GetGamma12Data(u8Tab, u16Addr*2+2);

        u16OutRef = u16InputData1 / 4;

        if(u16Addr == 127)
            u8OutDelta = (_Hal_PNL_GetGamma12Data(u8Tab, 255) - u16InputData1 + 1) / 2;
        else
            u8OutDelta = (u16InputData2 - u16InputData1 + 2) / 4;

        u8OutData1 = (MS_U8)((u16OutRef & 0x07) << 5) + u8OutDelta;
        u8OutData2 = (MS_U8)(u16OutRef >> 3);

        u16GammaValue =  (((MS_U16)u8OutData2) <<  8) | ((MS_U16)u8OutData1);

        if(u16MaxGammaValue < u16GammaValue)
        {
            u16MaxGammaValue = u16GammaValue;
        }

        //printf("Input1:%04x, Input2:%04x Gamma: %04x \r\n", u16InputData1, u16InputData2, u16GammaValue);
        _Hal_PNL_WriteGamma12Bit(u8Channel, u16Addr, u16GammaValue);
    }

    hal_PNL_SetMaxGammaValue(u8Channel, u16MaxGammaValue);
}

void MHal_PNL_Init_LPLL(PNL_TYPE eLPLL_Type,PNL_MODE eLPLL_Mode,MS_U64 ldHz)
{
    if((eLPLL_Type == E_PNL_TYPE_LVDS)||(eLPLL_Type == E_PNL_TYPE_HS_LVDS)||(eLPLL_Type == E_PNL_TYPE_HF_LVDS))
    {

        W2BYTEMSK(L_BK_LPLL(0x03), 0,     BIT(5));  //[5] reg_lpll_pd
        W2BYTEMSK(L_BK_LPLL(0x03), 0x12,  0x1F);    //[4:2]|[1:0] reg_lpll_icp_ictrl|reg_lpll_ibias_ictrl
        W2BYTEMSK(L_BK_LPLL(0x01), 0x203, 0xF03);   //[11:8]|[1:0] reg_lpll_loop_div_first | reg_lpll_loop_div_second

		W2BYTEMSK(L_BK_LPLL(0x2E), 0, 0x0F);        //[3]|[2:0] reg_lpll_en_fifo|reg_lpll_fifo_div
		W2BYTEMSK(L_BK_LPLL(0x03), 0, BIT(9));      //[9] reg_lpll1_en_skew_div

		W2BYTEMSK(L_BK_LPLL(0x35), 0x300,  0x300);  //[9:8] reg_lpll_pd_phdac

        W2BYTEMSK(L_BK_LPLL(0x2E), BIT(5), BIT(5)); //[5] reg_lpll_vco_sel
        W2BYTEMSK(L_BK_LPLL(0x37), 0x00,   0x02);   //[1] reg_lpll1_test[17]

		W2BYTEMSK(L_BK_LPLL(0x33), 0x20, 0x3F);     //[5]|[4:2]|[1:0] reg_lpll2_pd|reg_lpll2_ibias_ictrl|reg_lpll2_icp_ictrl
        W2BYTEMSK(L_BK_LPLL(0x35), 0,    BIT(5));   //[5] reg_lpll_2ndpll_clk_sel

		W2BYTEMSK(L_BK_LPLL(0x30), 0,      0xFF00); //[15:8] reg_lpll2_input_div_second
        W2BYTEMSK(L_BK_LPLL(0x31), 0x0000, 0xFF03); //[15:8]|[1:0] reg_lpll2_loop_div_second|reg_lpll2_loop_div_first
        W2BYTEMSK(L_BK_LPLL(0x32), 0x00,   0x03);   //[1:0] reg_lpll2_output_div_first

		W2BYTEMSK(L_BK_LPLL(0x3F), 0, BIT(15));     //[15] reg_lpll_en_hdmi

        if(eLPLL_Type == E_PNL_TYPE_LVDS)
        {
        	if ( E_PNL_MODE_SINGLE == eLPLL_Mode )
        	{
	            if( ldHz >750000000 )
	            {
	                W2BYTEMSK(L_BK_LPLL(0x02), 0x1700, 0x3F00);  //[13:12]|[11:8] reg_lpll_scaler_div_first|reg_lpll_scaler_div_second
	                W2BYTEMSK(L_BK_LPLL(0x35), 0x0, 0x7000);    //[14:12] reg_lpll_skew_div
	            }
	            else if( ldHz <750000000 )
	            {
	                W2BYTEMSK(L_BK_LPLL(0x02), 0x2700, 0x3F00);  //[13:12]|[11:8] reg_lpll_scaler_div_first|reg_lpll_scaler_div_second
	                W2BYTEMSK(L_BK_LPLL(0x35), 0x1000, 0x7000);    //[14:12] reg_lpll_skew_div
	            }
	            else
	            {
	                W2BYTEMSK(L_BK_LPLL(0x02), 0x3700, 0x3F00);  //[13:12]|[11:8] reg_lpll_scaler_div_first|reg_lpll_scaler_div_second
	                W2BYTEMSK(L_BK_LPLL(0x35), 0x2000, 0x7000);    //[14:12] reg_lpll_skew_div
	            }
        	}
			else
			{
				printk(" [LPLL] !!!!! C3: only support *single* LVDS !!!!! \r\n");
			}
        }
        else if((eLPLL_Type == E_PNL_TYPE_HS_LVDS))
        {
            if(ldHz >750000000)
            {
                W2BYTEMSK(L_BK_LPLL(0x02), 0x1700, 0x3F00);  //[13:12]|[11:8] reg_lpll_scaler_div_first|reg_lpll_scaler_div_second
                W2BYTEMSK(L_BK_LPLL(0x35), 0x00<<12, 0x7000);    //[14:12] reg_lpll_skew_div
            }
            else
            {
                W2BYTEMSK(L_BK_LPLL(0x02), 0x2700, 0x3F00);  //[13:12]|[11:8] reg_lpll_scaler_div_first|reg_lpll_scaler_div_second
                W2BYTEMSK(L_BK_LPLL(0x35), 0x01<<12, 0x7000);    //[14:12] reg_lpll_skew_div
            }
        }
        else
        {
            //Add by different app
        }
    }
    else if ( E_PNL_TYPE_TTL == eLPLL_Type || E_PNL_TYPE_TTL_TCON == eLPLL_Type)
    {
        //W2BYTEMSK(L_BK_LPLL(0x03), 0,     BIT(5));   //[5] reg_lpll_pd
        W2BYTEMSK(L_BK_LPLL(0x03), 0x0013, 0x023F);  //[4:2]|[1:0] reg_lpll_icp_ictrl|reg_lpll_ibias_ictrl
        W2BYTEMSK(L_BK_LPLL(0x01), 0x0203, 0x0F03);  //[11:8]|[1:0] reg_lpll_loop_div_first | reg_lpll_loop_div_second

        W2BYTEMSK(L_BK_LPLL(0x02), 0x2700, 0x3F00);  //[5] reg_lpll2_pd, power down
        W2BYTEMSK(L_BK_LPLL(0x35), 0x0300, 0x7320);       //[5] reg_lpll_2ndpll_clk_sel

        W2BYTEMSK(L_BK_LPLL(0x33), 0x0020, 0x003F);  //reg_lpll2
        W2BYTEMSK(L_BK_LPLL(0x30), 0x0000, 0xFF00);  // reg_lpll2_input_div_first | reg_lpll2_input_div_second
        W2BYTEMSK(L_BK_LPLL(0x31), 0x0000, 0xFF03);  // reg_lpll2_loop_div_first | reg_lpll2_loop_div_second
        W2BYTEMSK(L_BK_LPLL(0x32), 0x0000, 0x0003);  // reg_lpll2_out_div_first

        W2BYTEMSK(L_BK_LPLL(0x2E), 0x0020, 0x002F);
        W2BYTEMSK(L_BK_LPLL(0x37), 0x0002, 0x0002);
    }
    else
    {
        W2BYTEMSK(L_BK_LPLL(0x03), 0, BIT(5));   //[5] reg_lpll_pd
        W2BYTEMSK(L_BK_LPLL(0x03), 0x0D, 0x1F);  //[4:2]|[1:0] reg_lpll_icp_ictrl|reg_lpll_ibias_ictrl
        W2BYTEMSK(L_BK_LPLL(0x01), 0x203, 0xF03);  //[11:8]|[1:0] reg_lpll_loop_div_first | reg_lpll_loop_div_second

        W2BYTEMSK(L_BK_LPLL(0x33), BIT(5), BIT(5));  //[5] reg_lpll2_pd, power down
        W2BYTEMSK(L_BK_LPLL(0x35), 0, BIT(5));       //[5] reg_lpll_2ndpll_clk_sel

/// need to check
        W2BYTEMSK(L_BK_LPLL(0x33), 0x2E, 0x2E);  //reg_lpll2
        W2BYTEMSK(L_BK_LPLL(0x30), 0x00, 0xFF03);  // reg_lpll2_input_div_first | reg_lpll2_input_div_second
        W2BYTEMSK(L_BK_LPLL(0x31), 0x00, 0xFF03);  // reg_lpll2_loop_div_first | reg_lpll2_loop_div_second
        W2BYTEMSK(L_BK_LPLL(0x32), 0x00, 0x03);  // reg_lpll2_out_div_first
    }

	if( eLPLL_Type == E_PNL_TYPE_TTL)
		MHal_MOD_PVDD_Power_Setting(FALSE);
	else
		MHal_MOD_PVDD_Power_Setting(TRUE);
}

MS_U8 MHal_PNL_Get_Loop_DIV( MS_U8 u8LPLL_Mode, MS_U8 eLPLL_Type, MS_U32 ldHz)
{
    MS_U8   u8loop_div = 0;

    switch(eLPLL_Type)
    {
        case E_PNL_LPLL_EPI34_6P:
        case E_PNL_LPLL_EPI34_8P:
        {
            u8loop_div = 17;
        }
        break;

        case E_PNL_LPLL_EPI28_6P:
        {
            if(ldHz > 1100000000)
            {
                u8loop_div = 7;
            }
            else if((ldHz<=1100000000)&&(ldHz>=800000000))
            {
                u8loop_div = 17;
            }
            else
            {
                u8loop_div = 14;
            }
        }
        break;

        case E_PNL_TYPE_LVDS:
        {
            if(u8LPLL_Mode == E_PNL_MODE_SINGLE)
            {
                u8loop_div = 28;
            }
            else
            {
                if(ldHz > 750000000)
                {
                    u8loop_div = 14;
                }
                else
                {
                    u8loop_div = 28;
                }
            }
        }
        break;

        case E_PNL_LPLL_EPI28_8P:
        case E_PNL_TYPE_TTL:
        //case E_PNL_TYPE_LVDS:
        case E_PNL_TYPE_RSDS:
        case E_PNL_TYPE_MFC:
        case E_PNL_TYPE_PDPLVDS:
        case E_PNL_TYPE_HF_LVDS:
        case E_PNL_TYPE_HS_LVDS:
        default:
        {
            if(ldHz > 750000000)
            {
                u8loop_div = 14;
            }
            else
            {
                u8loop_div = 28;
            }
        }
        break;
    }
    u8loop_div *= 2;
    return u8loop_div;
}

MS_U16 MHal_PNL_Get_LPLL_LoopGain(MS_U8 eLPLL_Type, MS_U32 ldHz)
{
    MS_U16     u16loop_gain = 0;

    switch(eLPLL_Type)
    {
        case E_PNL_TYPE_MINILVDS:
        case E_PNL_TYPE_ANALOG_MINILVDS:
        case E_PNL_LPLL_MINILVDS_6P_1L:
            u16loop_gain = LPLL_LOOPGAIN;
        break;

        case E_PNL_TYPE_DIGITAL_MINILVDS:  //E_PNL_TYPE_MINILVDS_6P_2L
            u16loop_gain = 24;
        break;

        case E_PNL_LPLL_MINILVDS_4P_2L:
        case E_PNL_LPLL_MINILVDS_4P_1L:
            u16loop_gain = LPLL_LOOPGAIN;
        break;

        case E_PNL_LPLL_MINILVDS_3P_2L:
        case E_PNL_LPLL_MINILVDS_3P_1L:
            u16loop_gain = LPLL_LOOPGAIN;
        break;

        case E_PNL_TYPE_TTL:
        case E_PNL_TYPE_LVDS:
        case E_PNL_TYPE_RSDS:
        case E_PNL_TYPE_MFC:
        case E_PNL_TYPE_DAC_I:
        case E_PNL_TYPE_DAC_P:
        case E_PNL_TYPE_PDPLVDS:
        default:
            u16loop_gain = LPLL_LOOPGAIN;
        break;
    }

    return u16loop_gain;
}

#define SKIP_TIMING_CHANGE_CAP  TRUE
MS_BOOL Hal_PNL_SkipTimingChange_GetCaps(void)
{
    #if (SKIP_TIMING_CHANGE_CAP)
        return TRUE;
    #else
        return FALSE;
    #endif
}

void MHal_PNL_HWLVDSReservedtoLRFlag(PNL_DrvHW_LVDSResInfo lvdsresinfo)
{
    if (lvdsresinfo.bEnable)
    {
        if (lvdsresinfo.u16channel & BIT(0))  // Channel A
        {
            if (lvdsresinfo.u32pair & BIT(3))  // pair 3
            {
                MOD_W2BYTEMSK(REG_MOD_BK00_5A_L, BIT(15), BIT(15));
            }
            if (lvdsresinfo.u32pair & BIT(4))  // pair 4
            {
                MOD_W2BYTEMSK(REG_MOD_BK00_5A_L, BIT(14), BIT(14));
            }
        }
        if (lvdsresinfo.u16channel & BIT(1))  // Channel B
        {
            if (lvdsresinfo.u32pair & BIT(3))  // pair 3
            {
                MOD_W2BYTEMSK(REG_MOD_BK00_5A_L, BIT(13), BIT(13));
            }
            if (lvdsresinfo.u32pair & BIT(4))  // pair 4
            {
                MOD_W2BYTEMSK(REG_MOD_BK00_5A_L, BIT(12), BIT(12));
            }
        }
    }
    else
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_5A_L, 0x0000, (BIT(15) | BIT(14) | BIT(13) | BIT(12)));
    }
}

////////////////////////////////////////////////////////////////////////
// Turn OD function
////////////////////////////////////////////////////////////////////////
void MHal_PNL_OverDriver_Init(MS_U32 u32OD_MSB_Addr, MS_U32 u32OD_MSB_limit, MS_U32 u32OD_LSB_Addr, MS_U32 u32OD_LSB_limit)
{
    SC_W2BYTE(REG_SC_BK16_15_L, (MS_U16)(u32OD_MSB_Addr & 0xFFFF)); // OD MSB request base address
    SC_W2BYTEMSK(REG_SC_BK16_16_L, (MS_U16)((u32OD_MSB_Addr >> 16) & 0x00FF), 0x00FF); // OD MSB request base address
    SC_W2BYTEMSK(REG_SC_BK16_60_L, (MS_U16)((u32OD_MSB_Addr >> 24) & 0x0003), 0x0003); // OD MSB request base address
    SC_W2BYTE(REG_SC_BK16_17_L, (MS_U16)(u32OD_MSB_limit & 0xFFFF)); // OD MSB request address limit
    SC_W2BYTEMSK(REG_SC_BK16_18_L, (MS_U16)((u32OD_MSB_limit >> 16) & 0x00FF), 0x00FF); // OD MSB request address limit
    SC_W2BYTEMSK(REG_SC_BK16_60_L, (MS_U16)((u32OD_MSB_limit >> 24) & 0x0003)<<2, 0x000C); // OD MSB request address limit
    SC_W2BYTE(REG_SC_BK16_39_L, (MS_U16)(u32OD_LSB_limit & 0xFFFF)); // OD frame buffer write address limit
    SC_W2BYTEMSK(REG_SC_BK16_3A_L, (MS_U16)((u32OD_LSB_limit >> 16) & 0x00FF), 0x00FF); // OD frame buffer write address limit
    SC_W2BYTE(REG_SC_BK16_3B_L, (MS_U16)(u32OD_LSB_limit & 0xFFFF)); // OD frame buffer read address limit
    SC_W2BYTEMSK(REG_SC_BK16_3C_L, (MS_U16)((u32OD_LSB_limit >> 16) & 0x00FF), 0x00FF); // OD frame buffer read address limit
    SC_W2BYTE(REG_SC_BK16_4F_L, (MS_U16)(u32OD_LSB_Addr & 0xFFFF)); // OD LSB request base address
    SC_W2BYTEMSK(REG_SC_BK16_50_L, (MS_U16)((u32OD_LSB_Addr >> 16) & 0x00FF), 0x00FF); // OD LSB request base address
    SC_W2BYTEMSK(REG_SC_BK16_50_L, (MS_U16)((u32OD_LSB_limit & 0x00FF) << 8), 0xFF00); // OD LSB request limit address
    SC_W2BYTE(REG_SC_BK16_51_L, (MS_U16)((u32OD_LSB_limit >> 8) & 0xFFFF)); // OD LSB request limit address

    SC_W2BYTE(REG_SC_BK16_1A_L, 0x4020); // OD request rFIFO limit threshold, priority threshold
    SC_W2BYTE(REG_SC_BK16_1C_L, 0x4020); // OD request wFIFO limit threshold, priority threshold
    SC_W2BYTEMSK(REG_SC_BK16_12_L,0x0C,0x00FF); //OD active threshold
}

void MHal_PNL_OverDriver_Enable(MS_BOOL bEnable)
{
    // OD mode
    // OD used user weight to output blending directly
    // OD Enable
    if (bEnable)
    {
        SC_W2BYTEMSK(REG_SC_BK16_10_L, 0x2D, 0x2F);
    }
    else
    {
        SC_W2BYTEMSK(REG_SC_BK16_10_L, 0x2C, 0x2F);
    }
}

void MHal_PNL_OverDriver_TBL(MS_U8 u8ODTbl[1056])
{
    MS_U16 i;
    MS_U8 u8target;
    MS_BOOL bEnable;

    bEnable = SC_R2BYTEMSK(REG_SC_BK16_10_L, BIT(0));
    SC_W2BYTEMSK(REG_SC_BK16_10_L, 0x00, BIT(0)); // OD enable
    SC_W2BYTEMSK(REG_SC_BK16_01_L, 0x0E, 0x0E); // OD table SRAM enable, RGB channel

    u8target= u8ODTbl[9];
    for (i=0; i<272; i++)
    {
        SC_W2BYTEMSK(REG_SC_BK16_03_L, (i == 9)?u8target:(u8target ^ u8ODTbl[i]), 0x00FF);
        SC_W2BYTEMSK(REG_SC_BK16_02_L, (i|0x8000), 0x81FF);
        while(SC_R2BYTEMSK(REG_SC_BK16_02_L, BIT(15)));
    }

    u8target= u8ODTbl[(272+19)];
    for (i=0; i<272; i++)
    {
        SC_W2BYTEMSK(REG_SC_BK16_06_L, (i == 19)?u8target:(u8target ^ u8ODTbl[(272+i)]), 0x00FF);
        SC_W2BYTEMSK(REG_SC_BK16_05_L, (i|0x8000), 0x81FF);
        while(SC_R2BYTEMSK(REG_SC_BK16_05_L, BIT(15)));
    }

    u8target= u8ODTbl[(272*2+29)];
    for (i=0; i<256; i++)
    {
        SC_W2BYTEMSK(REG_SC_BK16_09_L, (i == 29)?u8target:(u8target ^ u8ODTbl[(272*2+i)]), 0x00FF);
        SC_W2BYTEMSK(REG_SC_BK16_08_L, (i|0x8000), 0x81FF);
        while(SC_R2BYTEMSK(REG_SC_BK16_08_L, BIT(15)));
    }

    u8target= u8ODTbl[(272*2+256+39)];
    for (i=0; i<256; i++)
    {
        SC_W2BYTEMSK(REG_SC_BK16_0C_L, (i == 39)?u8target:(u8target ^ u8ODTbl[(272*2+256+i)]), 0x00FF);
        SC_W2BYTEMSK(REG_SC_BK16_0B_L, (i|0x8000), 0x81FF);
        while(SC_R2BYTEMSK(REG_SC_BK16_0D_L, BIT(15)));
    }

    SC_W2BYTEMSK(REG_SC_BK16_01_L, 0x00, 0x0E); // OD table SRAM enable, RGB channel
    SC_W2BYTEMSK(REG_SC_BK16_10_L, bEnable, BIT(0)); // OD enable
}

MS_U16 _MHal_PNL_MOD_Swing_Refactor_AfterCAL(MS_U16 u16Swing_Level)
{
    MS_U8 u8ibcal = 0x00;
    MS_U16 u16AfterCal_value = 0;
    MS_U16 u16Cus_value = 0;


    // =========
    // GCR_CAL_LEVEL[1:0] : REG_MOD_BK00_7D_L =>
    // 2'b00 250mV ' GCR_ICON_CHx[5:0]=2'h15 (decimal 21)
    // 2'b01 350mV ' GCR_ICON_CHx[5:0]=2'h1F (decimal 31)
    // 2'b10 300mV ' GCR_ICON_CHx[5:0]=2'h1A (decimal 26)
    // 2'b11 200mV ' GCR_ICON_CHx[5:0]=2'h10 (decimal 16)
    // =========
    switch(_u8MOD_CALI_TARGET)
    {
        default:
        case 0x00:
            u8ibcal = 0x15;
        break;
        case 0x01:
            u8ibcal = 0x1F;
        break;
        case 0x10:
            u8ibcal = 0x1A;
        break;
        case 0x11:
            u8ibcal = 0x10;
        break;
    }
    u16Cus_value = (u16Swing_Level) * (_u8MOD_CALI_VALUE + 4)/(u8ibcal + 4);
    u16AfterCal_value = (u16Cus_value-40)/10+2;

    HAL_MOD_CAL_DBG(printf("\r\n--Swing value after refactor = %d\n", u16AfterCal_value));

    return u16AfterCal_value;
}

MS_BOOL MHal_PNL_MOD_Control_Out_Swing(MS_U16 u16Swing_Level)
{
    MS_BOOL bStatus = FALSE;

    MS_U16 u16ValidSwing = 0;

    if(u16Swing_Level>600)
        u16Swing_Level=600;
    if(u16Swing_Level<40)
        u16Swing_Level=40;

    u16ValidSwing = _MHal_PNL_MOD_Swing_Refactor_AfterCAL(u16Swing_Level);

    // Separate mode.
    MOD_W2BYTEMSK(REG_MOD_BK00_29_L,
        ( (u16ValidSwing << 4 )|(u16ValidSwing << 10 ) ) , 0xFFF0);

    MOD_W2BYTE(REG_MOD_BK00_2A_L,
        ((u16ValidSwing  )|(u16ValidSwing << 6 ) |(u16ValidSwing << 12 ) ));

    MOD_W2BYTE(REG_MOD_BK00_2B_L,
        ( (u16ValidSwing >> 4  )|(u16ValidSwing << 2 ) |(u16ValidSwing << 8 ) | (u16ValidSwing << 14 )));

    MOD_W2BYTE(REG_MOD_BK00_2C_L,
        ( (u16ValidSwing >> 2  )|(u16ValidSwing << 4 ) |(u16ValidSwing << 10 ) ) );

    MOD_W2BYTE(REG_MOD_BK00_2D_L,
        ( (u16ValidSwing  )|(u16ValidSwing << 6 ) |(u16ValidSwing << 12 ) ));

    MOD_W2BYTE(REG_MOD_BK00_2E_L,
        ( (u16ValidSwing >> 4 )|(u16ValidSwing << 2 ) |(u16ValidSwing << 8 ) |(u16ValidSwing << 14 ) ) );

    MOD_W2BYTE(REG_MOD_BK00_2F_L,
        ( (u16ValidSwing >> 2  )|(u16ValidSwing << 4 ) |(u16ValidSwing << 10 ) ) );

    MOD_W2BYTE(REG_MOD_BK00_30_L,
        ( (u16ValidSwing  )|(u16ValidSwing << 6 ) |(u16ValidSwing << 12 ) ));

    MOD_W2BYTEMSK(REG_MOD_BK00_31_L, (u16ValidSwing >> 4 ) ,0x0003);

    bStatus = TRUE;

    return bStatus;
}

////////////////////////////////////////////////////////////////////////
// Turn Pre-Emphasis Current function
////////////////////////////////////////////////////////////////////////
MS_BOOL MHal_PNL_MOD_Control_Out_PE_Current (MS_U16 u16Current_Level)
{
    MS_BOOL bStatus = FALSE;
    MS_U16 u16ValidCurrent = u16Current_Level & 0x07;

    MOD_W2BYTEMSK(REG_MOD_BK00_23_L,
        ( (u16ValidCurrent  ) |(u16ValidCurrent << 3 )|(u16ValidCurrent << 6 )
        |(u16ValidCurrent << 9 ) |(u16ValidCurrent << 12 )) , 0x7FFF);

    MOD_W2BYTEMSK(REG_MOD_BK00_24_L,
        ( (u16ValidCurrent  ) |(u16ValidCurrent << 3 )|(u16ValidCurrent << 6 )
        |(u16ValidCurrent << 9 ) |(u16ValidCurrent << 12 )) , 0x7FFF);

    MOD_W2BYTEMSK(REG_MOD_BK00_25_L,
        ( (u16ValidCurrent  ) |(u16ValidCurrent << 3 )|(u16ValidCurrent << 6 )
        |(u16ValidCurrent << 9 ) |(u16ValidCurrent << 12 )) , 0x7FFF);

    MOD_W2BYTEMSK(REG_MOD_BK00_26_L,
        ( (u16ValidCurrent  ) |(u16ValidCurrent << 3 )|(u16ValidCurrent << 6 )
        |(u16ValidCurrent << 9 ) |(u16ValidCurrent << 12 )) , 0x7FFF);

    MOD_W2BYTEMSK(REG_MOD_BK00_29_L, u16ValidCurrent  ,0x0007);

    bStatus = TRUE;

    return bStatus;
}

////////////////////////////////////////////////////////////////////////
// 1.Turn TTL low-power mode function
// 2.Turn internal termination function
// 3.Turn DRIVER BIAS OP function
////////////////////////////////////////////////////////////////////////
MS_BOOL MHal_PNL_MOD_Control_Out_TTL_Resistor_OP (MS_BOOL bEnble)
{
    MS_BOOL bStatus = FALSE;
    if(bEnble)
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_36_L, 0xFFFF, 0xFFFF); //Enable TTL low-power mode
        MOD_W2BYTEMSK(REG_MOD_BK00_37_L, 0x001E, 0x001E);

        MOD_W2BYTEMSK(REG_MOD_BK00_75_L, 0xFFFF, 0xFFFF); //GCR_EN_RINT (internal termination open)
        MOD_W2BYTEMSK(REG_MOD_BK00_76_L, 0x003F, 0x003F);

        MOD_W2BYTEMSK(REG_MOD_BK00_79_L, 0xFFFF, 0xFFFF); //Disable DRIVER BIAS OP
        MOD_W2BYTEMSK(REG_MOD_BK00_7A_L, 0x003F, 0x003F);
    }
    else
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_36_L, 0x0000, 0xFFFF); //Disable TTL low-power mode
        MOD_W2BYTEMSK(REG_MOD_BK00_37_L, 0x0000, 0x001E);

        MOD_W2BYTEMSK(REG_MOD_BK00_75_L, 0x0000, 0xFFFF); //GCR_EN_RINT (internal termination close)
        MOD_W2BYTEMSK(REG_MOD_BK00_76_L, 0x0000, 0x003F);

        MOD_W2BYTEMSK(REG_MOD_BK00_79_L, 0x0000, 0xFFFF); //Enable DRIVER BIAS OP
        MOD_W2BYTEMSK(REG_MOD_BK00_7A_L, 0x0000, 0x003F);
    }

    bStatus = TRUE;
    return bStatus;
}

void MHal_PNL_PreInit(PNL_OUTPUT_MODE eParam)
{
    _eDrvPnlInitOptions = eParam;
}

PNL_OUTPUT_MODE MHal_PNL_Get_Output_MODE(void)
{
    PNL_OUTPUT_MODE eParam = _eDrvPnlInitOptions;

    return eParam;
}

void MHal_PNL_SetOutputType(PNL_OUTPUT_MODE eOutputMode, PNL_TYPE eLPLL_Type)
{
    MS_U16 u16ValidSwing2 = 0;
    if( eLPLL_Type == E_PNL_TYPE_TTL)
    {
        // select pair output to be TTL
        MOD_W2BYTE(REG_MOD_BK00_6D_L, 0x0000);
        MOD_W2BYTE(REG_MOD_BK00_6E_L, 0x0000);
        MOD_W2BYTEMSK(REG_MOD_BK00_6F_L, 0x0000, 0xEFFF);//0x6F

        MOD_W2BYTEMSK(REG_MOD_BK00_42_L, 0x0000, (BIT(7) | BIT(6))); // shift_lvds_pair

        // other TTL setting
        MOD_W2BYTE(REG_MOD_BK00_45_L, 0x0000);     // TTL output enable

        MOD_W2BYTE(REG_MOD_BK00_46_L, 0x0000);
        MOD_W2BYTE(REG_MOD_BK00_47_L, 0x0000);
        MOD_W2BYTEMSK(REG_MOD_BK00_7E_L, 0x0000, 0xE000);

        MOD_W2BYTEMSK(REG_MOD_BK00_44_L, 0x3FF, 0x3FF);       // TTL skew

        // GPO gating
        MOD_W2BYTEMSK(REG_MOD_BK00_4A_L, BIT(8), BIT(8));         // GPO gating

        //W2BYTE(REG_GPIO1_16_L, 0x002C);//[5]ttl_bg_mode [3]tck_mode [2]ttlhv_mode
        //W2BYTE(REG_GPIO0_55_L, 0x0101);//tcon_gpio to enable panel

		// Close differential clock for power saving
		MOD_W2BYTE(REG_MOD_BK00_77_L, 0x0000);		// close differential clock
		MOD_W2BYTE(REG_MOD_BK00_78_L, 0x0000);		// close differential power
    }
    else
    {
        switch(eOutputMode)
        {
            case E_PNL_OUTPUT_NO_OUTPUT:
                // if MOD_45[5:0] = 0x3F && XC_MOD_EXT_DATA_EN_L = 0x0,
                // then if XC_MOD_OUTPUT_CONF_L = 0x0 ---> output TTL as tri-state
                MOD_W2BYTEMSK(REG_MOD_BK00_6D_L, 0x0000, 0xF000);
                MOD_W2BYTE(REG_MOD_BK00_6E_L, 0x0000);
                MOD_W2BYTEMSK(REG_MOD_BK00_6F_L, 0x0000, 0x000F);

                //----------------------------------
                // Purpose: Set the output to be the GPO, and let it's level to Low
                // 1. External Enable, Pair 0~5
                // 2. GPIO Enable, pair 0~5
                // 3. GPIO Output data : All low, pair 0~5
                // 4. GPIO OEZ: output piar 0~5
                //----------------------------------

                //1.External Enable, Pair 0~5
                MOD_W2BYTEMSK(REG_MOD_BK00_46_L, 0x0FFF, 0x0FFF);
                //2.GPIO Enable, pair 0~5
                MOD_W2BYTEMSK(REG_MOD_BK00_4D_L, 0x0FFF, 0x0FFF);
                //3.GPIO Output data : All low, pair 0~5
                MOD_W2BYTEMSK(REG_MOD_BK00_4F_L, 0x0000, 0x0FFF);
                //4.GPIO OEZ: output piar 0~5
                MOD_W2BYTEMSK(REG_MOD_BK00_51_L, 0x0000, 0x0FFF);

                //1.External Enable, Pair 6~15
                MOD_W2BYTEMSK(REG_MOD_BK00_46_L, 0xF000, 0xF000);
                MOD_W2BYTE(REG_MOD_BK00_47_L, 0xFFFF);
                //2.GPIO Enable, pair 6~15
                MOD_W2BYTEMSK(REG_MOD_BK00_4D_L, 0xF000, 0xF000);
                MOD_W2BYTE(REG_MOD_BK00_4E_L, 0xFFFF);
                //3.GPIO Output data : All low, pair 6~15
                MOD_W2BYTEMSK(REG_MOD_BK00_4F_L, 0x0000, 0xF000);
                MOD_W2BYTE(REG_MOD_BK00_50_L, 0x0000);
                //4.GPIO OEZ: output piar 6~15
                MOD_W2BYTEMSK(REG_MOD_BK00_51_L, 0x0000, 0xF000);
                MOD_W2BYTE(REG_MOD_BK00_52_L, 0x0000);

                //1234.External Enable, Pair 16~17
                MOD_W2BYTE(REG_MOD_BK00_7E_L, 0xFF00);

                //1.External Enable, Pair 18~20, 2.GPIO Enable, pair 18~20
                MOD_W2BYTEMSK(REG_MOD_BK00_7C_L, 0x3F3F, 0x3F3F);
                //3.GPIO Output data : All low, pair 18~20
                MOD_W2BYTEMSK(REG_MOD_BK00_7A_L, 0x0000, 0x3F00);
                //4.GPIO OEZ: output piar 18~20
                MOD_W2BYTEMSK(REG_MOD_BK00_7F_L, 0x0000, 0xFC00);
                break;

            case E_PNL_OUTPUT_CLK_ONLY:
                MOD_W2BYTEMSK(REG_MOD_BK00_6D_L, 0, 0xF000);
                MOD_W2BYTE(REG_MOD_BK00_6E_L, 0x4004);
                MOD_W2BYTEMSK(REG_MOD_BK00_6F_L, 0, 0x000F);
                break;

            case E_PNL_OUTPUT_DATA_ONLY:
            case E_PNL_OUTPUT_CLK_DATA:
            default:

                MOD_W2BYTEMSK(REG_MOD_BK00_4D_L, 0x0000, 0xF000);
                MOD_W2BYTE(REG_MOD_BK00_4E_L, 0x0000);
                //1. set GCR_PVDD_2P5=1・b1;           MOD PVDD power:    1: 2.5V
                MOD_W2BYTEMSK(REG_MOD_BK00_37_L, BIT(6), BIT(6));
                //2. set PD_IB_MOD=1・b0;
                MOD_W2BYTEMSK(REG_MOD_BK00_78_L, 0x00, BIT(0));
                //  save ch6 init value
                u16ValidSwing2 = (MOD_R2BYTEMSK(REG_MOD_BK00_2B_L, 0x3F00)>>8);
                //3. set Desired Pairs: GCR_ICON[5:0]=6・h3f (current all open);
                MHal_PNL_MOD_Control_Out_Swing(0x3F);
                //4. set Desired Pairs: GCR_PE_ADJ[2:0]=3・h7 (pre-emphasis current all open )
                MHal_PNL_MOD_Control_Out_PE_Current (0x07);
                //5. Enable low-power modeinternal termination Open, Enable OP
                MHal_PNL_MOD_Control_Out_TTL_Resistor_OP (1);

                MsOS_DelayTask(1);

                //6. Enable low-power modeinternal termination Open, Enable OP
                MHal_Output_LVDS_Pair_Setting(_u8MOD_LVDS_Pair_Type, _stPnlInitData.u16OutputCFG0_7, _stPnlInitData.u16OutputCFG8_15, _stPnlInitData.u16OutputCFG16_21);
                MHal_Shift_LVDS_Pair(_u8MOD_LVDS_Pair_Shift);

                //7. set Desired Pairs: GCR_PE_ADJ[2:0]=3・h0 (pre-emphasis current all Close)
                MHal_PNL_MOD_Control_Out_PE_Current (0x00);
                //8. set Desired Pairs: GCR_ICON[5:0]    (current all init);
                MHal_PNL_MOD_Control_Out_Swing(u16ValidSwing2);
                //9. Disable low-power modeinternal termination Close, Disable OP
                MHal_PNL_MOD_Control_Out_TTL_Resistor_OP (0);

                // other TTL setting
                MOD_W2BYTE(REG_MOD_BK00_45_L, 0x003F);     // LVDS output enable, [5:4] Output enable: PANEL_LVDS/ PANEL_miniLVDS/ PANEL_RSDS

                MOD_W2BYTEMSK(REG_MOD_BK00_46_L, 0x0000, 0xF000);
                MOD_W2BYTE(REG_MOD_BK00_47_L, 0x0000);
                MOD_W2BYTEMSK(REG_MOD_BK00_7E_L, 0x0000, 0x000F);

                MOD_W2BYTEMSK(REG_MOD_BK00_44_L, 0x000, 0x3FF);    // TTL skew

                // GPO gating
                MOD_W2BYTEMSK(REG_MOD_BK00_4A_L, 0x0, BIT(8));     // GPO gating

                break;
        }
    }
}

void Mhal_PNL_Flock_LPLLSet(MS_U32 ldHz)
{
    UNUSED(ldHz);
}


void MHal_PNL_MISC_Control(MS_U32 u32PNL_MISC)
{
    if(u32PNL_MISC & E_DRVPNL_MISC_MFC_ENABLE)
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_42_L, BIT(7), BIT(7));       // shift LVDS pair
    }
}

void MHal_PNL_Init_XC_Clk(PNL_InitData *pstPanelInitData)
{
    // setup output dot clock
    W2BYTEMSK(REG_CKG_ODCLK, CKG_ODCLK_CLK_LPLL, CKG_ODCLK_MASK);      // select source tobe LPLL clock
    W2BYTEMSK(REG_CKG_ODCLK, DISABLE, CKG_ODCLK_INVERT);               // clock not invert
    W2BYTEMSK(REG_CKG_ODCLK, DISABLE, CKG_ODCLK_GATED);                // enable clock

    W2BYTEMSK(REG_CKG_BT656, CKG_BT656_CLK_LPLL, CKG_BT656_MASK);      // select source tobe LPLL clock
    W2BYTEMSK(REG_CKG_BT656, DISABLE, CKG_ODCLK_INVERT);               // clock not invert
    W2BYTEMSK(REG_CKG_BT656, DISABLE, CKG_ODCLK_GATED);                // enable clock
    W2BYTE(REG_CLKGEN0_57_L, 0x0000); //[15:12]ckg_bt656 [3:0]ckg_fifo
    W2BYTE(REG_CLKGEN0_58_L, 0x0000); //[3:0]ckg_tx_mod
    W2BYTE(REG_CLKGEN0_59_L, 0x0000); //[3:0]ckg_lpll_syn
    W2BYTE(REG_CLKGEN0_5E_L, 0x0000); //[11:8]ckg_tx_mod [3:0]ckg_osd2mod
}

void MHal_PNL_Init_MOD(PNL_InitData *pstPanelInitData)
{

    //-------------------------------------------------------------------------
    // Set MOD registers
    //-------------------------------------------------------------------------
    MOD_W2BYTEMSK(REG_MOD_BK00_40_L, pstPanelInitData->u16MOD_CTRL0, LBMASK);

    //    GPIO is controlled in drvPadConf.c
    //    MDrv_Write2Byte(L_BK_MOD(0x46), 0x0000);    //EXT GPO disable
    //    MDrv_Write2Byte(L_BK_MOD(0x47), 0x0000);    //EXT GPO disable
    MOD_W2BYTE(REG_MOD_BK00_49_L, pstPanelInitData->u16MOD_CTRL9); //{L_BK_MOD(0x49), 0x00}, // [7,6] : output formate selction 10: 8bit, 01: 6bit :other 10bit, bit shift
    MOD_W2BYTE(REG_MOD_BK00_4A_L, pstPanelInitData->u16MOD_CTRLA);
    MOD_W2BYTE(REG_MOD_BK00_4B_L,  pstPanelInitData->u8MOD_CTRLB);  //[1:0]ti_bitmode 10:8bit  11:6bit  0x:10bit

    if ( SUPPORT_SYNC_FOR_DUAL_MODE )
    {
        // Set 1 only when PNL is dual mode
        MOD_W2BYTEMSK(REG_MOD_BK00_44_L, (pstPanelInitData->eLPLL_Mode << 12) , BIT(12));
    }

    //dual port lvds _start_//
    // output configure for 26 pair output 00: TTL, 01: LVDS/RSDS/mini-LVDS data differential pair, 10: mini-LVDS clock output, 11: RSDS clock output
    MOD_W2BYTE(REG_MOD_BK00_6F_L, 0x0000);    // output configure for 26 pair output 00: TTL, 01: LVDS/RSDS/mini-LVDS data differential pair, 10: mini-LVDS clock output, 11: RSDS clock output
    MOD_W2BYTEMSK(REG_MOD_BK00_77_L, 0x1F, 0x1F);    //[4]ck_pd[3]ck_pc[2]ck_pb[1]ck_pa[0]en_ck   // original is MDrv_WriteByteMask(L_BK_MOD(0x77), 0x0F, BITMASK(7:2));
    //dual port lvds _end_//

    //MOD_W2BYTEMSK(REG_MOD_BK00_78_L, (_u8PnlDiffSwingLevel << 1), 0xFE);       //differential output swing level
    //if(!MHal_PNL_MOD_Control_Out_Swing(_u8PnlDiffSwingLevel))
    //    printf(">>Swing Level setting error!!\n");
    if(pstPanelInitData->eLPLL_Type != E_PNL_TYPE_MINILVDS)
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_7D_L, 0x1F, LBMASK);       //[6]disable power down bit and [5:0]enable all channel
    }
    MOD_W2BYTE(REG_MOD_BK00_73_L, pstPanelInitData->u16LVDSTxSwapValue);


    // TODO: move from MDrv_Scaler_Init(), need to double check!
    MOD_W2BYTEMSK(REG_MOD_BK00_53_L, BIT(0), BIT(0));


    //--------------------------------------------------------------
    //Depend On Bitmode to set Dither
    //--------------------------------------------------------------


    // always enable noise dither and disable TAILCUT
    SC_W2BYTEMSK(REG_SC_BK24_3F_L, ((pstPanelInitData->u8PanelNoiseDith ? XC_PAFRC_DITH_NOISEDITH_EN : (1 - XC_PAFRC_DITH_NOISEDITH_EN)) <<3) , BIT(3));
    SC_W2BYTEMSK(REG_SC_BK24_3F_L, XC_PAFRC_DITH_TAILCUT_DISABLE, BIT(4));

    switch(pstPanelInitData->u8MOD_CTRLB & 0x03)//[1:0]ti_bitmode b'10:8bit  11:6bit  0x:10bit
    {
        case HAL_TI_6BIT_MODE:
            SC_W2BYTEMSK(REG_SC_BK24_3F_L, BIT(0), BIT(0));
            SC_W2BYTEMSK(REG_SC_BK24_3F_L, BIT(2), BIT(2));
            break;

        case HAL_TI_8BIT_MODE:
            SC_W2BYTEMSK(REG_SC_BK24_3F_L, BIT(0), BIT(0));
            SC_W2BYTEMSK(REG_SC_BK24_3F_L, 0x00, BIT(2));
            break;

        case HAL_TI_10BIT_MODE:
        default:
            SC_W2BYTEMSK(REG_SC_BK24_3F_L, 0x00, BIT(0));
            SC_W2BYTEMSK(REG_SC_BK24_3F_L, 0x00, BIT(2));
            break;
    }


    //-----depend on bitmode to set Dither------------------------------
    MHal_PNL_SetOutputType(_eDrvPnlInitOptions, pstPanelInitData->eLPLL_Type);     // TTL to Ursa

    MHal_PNL_MISC_Control(pstPanelInitData->u32PNL_MISC);

}

void MHal_PNL_DumpMODReg(MS_U32 u32Addr, MS_U16 u16Value, MS_BOOL bHiByte, MS_U16 u16Mask)
{
    if (bHiByte)
    {
        MOD_W2BYTEMSK(u32Addr, (u16Value << 8), (u16Mask << 8));
    }
    else
    {
        MOD_W2BYTEMSK(u32Addr, u16Value, u16Mask);
    }
}

void MHal_MOD_Calibration_Init(MS_U8 U8MOD_CALI_TARGET, MS_S8 S8MOD_CALI_OFFSET)
{
    _u8MOD_CALI_TARGET = U8MOD_CALI_TARGET;
    _usMOD_CALI_OFFSET = S8MOD_CALI_OFFSET;
}

void MHal_BD_LVDS_Output_Type(MS_U16 Type)
{
    if(Type == LVDS_DUAL_OUTPUT_SPECIAL )
    {
        _u8MOD_LVDS_Pair_Shift = LVDS_DUAL_OUTPUT_SPECIAL;
        _u8MOD_LVDS_Pair_Type = 1;
    }
    else
    {
        _u8MOD_LVDS_Pair_Type  = (MS_U8)Type;
    }
}

MS_BOOL msModCalDDAOUT(void)
{
   // W2BYTEMSK(BK_MOD(0x7D), ENABLE, 8:8);
   // MsOS_DelayTask(10);  //10ms
    return (MS_BOOL)((MOD_R2BYTEMSK(REG_MOD_BK00_7D_L, BIT(8))) >> 8);
}

MS_U8 msModCurrentCalibration(void)
{
#if MOD_CAL_TIMER
    MS_U32 delay_start_time;
    delay_start_time=MsOS_GetSystemTime();
#endif

#if (!ENABLE_Auto_ModCurrentCalibration)
    return 0x60;
#else
    MS_U8 u8cur_ibcal=0;
    MS_U8 u8cnt;
    MS_BOOL bdecflag=0,bincflag=0;
    MS_U8 u8steady=0;
    MS_U16 u16reg_3280,u16reg_3282,u16reg_328a;
    MS_U16 u16reg_32da,u16reg_32dc,u16reg_32de;
    MS_U8 u8preibcal=0;

    u16reg_3280 = MOD_R2BYTEMSK(REG_MOD_BK00_40_L, LBMASK);
    u16reg_3282 = MOD_R2BYTEMSK(REG_MOD_BK00_41_L, LBMASK);
    u16reg_328a = MOD_R2BYTEMSK(REG_MOD_BK00_45_L, LBMASK);

    u16reg_32da = MOD_R2BYTE(REG_MOD_BK00_6D_L);
    u16reg_32dc = MOD_R2BYTE(REG_MOD_BK00_6E_L);
    u16reg_32de = MOD_R2BYTE(REG_MOD_BK00_6F_L);

// =========
// GCR_CAL_LEVEL[1:0] : REG_MOD_BK00_7D_L =>
// 2'b00 250mV ' GCR_ICON_CHx[5:0]=2'h15 (decimal 21)
// 2'b01 350mV ' GCR_ICON_CHx[5:0]=2'h1F (decimal 31)
// 2'b10 300mV ' GCR_ICON_CHx[5:0]=2'h1A (decimal 26)
// 2'b11 200mV ' GCR_ICON_CHx[5:0]=2'h10 (decimal 16)
// =========

    switch(_u8MOD_CALI_TARGET)
    {
        default:
        case 0x00:
            u8cur_ibcal = 0x15;
        break;
        case 0x01:
            u8cur_ibcal = 0x1F;
        break;
        case 0x10:
            u8cur_ibcal = 0x1A;
        break;
        case 0x11:
            u8cur_ibcal = 0x10;
        break;
    }

    MOD_W2BYTEMSK(REG_MOD_BK00_2A_L, (u8cur_ibcal&0x3F), 0x003F); // ch10, calibration initialized value

    //Set output config to be test clock output mode
    MOD_W2BYTE(REG_MOD_BK00_6D_L, 0xFFFF);
    MOD_W2BYTE(REG_MOD_BK00_6E_L, 0xFFFF);
    MOD_W2BYTE(REG_MOD_BK00_6F_L, 0xFFFF);

    // White Pattern for Calibration
    MOD_W2BYTEMSK(REG_MOD_BK00_32_L, BIT(15), BIT(15)); // Enable test enable of digi seri
    MOD_W2BYTEMSK(REG_MOD_BK00_41_L, 0x00, 0xFF);       // Set analog testpix output to low

    // Set Calibration target
    MOD_W2BYTEMSK(REG_MOD_BK00_7D_L, 0, BIT(3)|BIT(2));  // Select calibration source pair, 01: channel 10
    MOD_W2BYTEMSK(REG_MOD_BK00_7D_L, _u8MOD_CALI_TARGET, BIT(1)|BIT(0));    // Select calibration target voltage, 00: 200mv, 01:350mv, 10: 250mv, 11: 150mv
    MOD_W2BYTEMSK(REG_MOD_BK00_7D_L, BIT(7), BIT(7));         // Enable calibration function

    HAL_MOD_CAL_DBG(printf("\r\n [%s](1)Init value:u8cur_ibcal = %d\n", __FUNCTION__, u8cur_ibcal));

    for (u8cnt=0;u8cnt<40;u8cnt++)
    {
        if (msModCalDDAOUT())
        {
            u8cur_ibcal--;
            bdecflag = 1;
        }
        else
        {
            u8cur_ibcal++;
            bincflag = 1;
        }

        HAL_MOD_CAL_DBG(printf("\r\n (2)u8cur_ibcal = %d", u8cur_ibcal));

        MOD_W2BYTEMSK(REG_MOD_BK00_2A_L,(u8cur_ibcal&0x3F), 0x003F);

        if (bdecflag&&bincflag)
        {
            if (abs(u8preibcal-u8cur_ibcal)<2)
            {
                u8steady++;
            }
            else
            {
                u8steady =0;
            }

            u8preibcal = u8cur_ibcal;

            bdecflag = 0;
            bincflag = 0;
        }

        if (u8steady==3)
            break;

    }

    MOD_W2BYTEMSK(REG_MOD_BK00_7D_L, 0x00, BIT(7));  // Disable calibration function
    MOD_W2BYTEMSK(REG_MOD_BK00_32_L, 0x00, BIT(15)); // Disable test enable of digi seri

    MOD_W2BYTEMSK(REG_MOD_BK00_40_L, u16reg_3280, LBMASK);
    MOD_W2BYTEMSK(REG_MOD_BK00_41_L, u16reg_3282, LBMASK);
    MOD_W2BYTEMSK(REG_MOD_BK00_45_L, u16reg_328a, LBMASK);

    MOD_W2BYTE(REG_MOD_BK00_6D_L, u16reg_32da);
    MOD_W2BYTE(REG_MOD_BK00_6E_L, u16reg_32dc);
    MOD_W2BYTE(REG_MOD_BK00_6F_L, u16reg_32de);

    // If the calibration is failed, used the default value
    if ((u8steady==3)&&(u8cnt<=40))
    {
        HAL_MOD_CAL_DBG(printf("\r\n----- Calibration ok.=> u8cur_ibcal = %d\n", u8cur_ibcal));
    }
    else
    {

        switch(_u8MOD_CALI_TARGET)
        {
            default:
            case 0x00:
                u8cur_ibcal = 0x15;//msReadByte(REG_32FA); //default 6'b100000
            break;
            case 0x01:
                u8cur_ibcal = 0x1F;
            break;
            case 0x10:
                u8cur_ibcal = 0x1A;
            break;
            case 0x11:
                u8cur_ibcal = 0x10;
            break;
        }
        HAL_MOD_CAL_DBG(printf("\r\n----- Calibration failed.=> u8cur_ibcal = %d\n", u8cur_ibcal));
    }

    //Limit the u8cur_ibcal range
    //u8cur_ibcal [5:0] => 0~63
    if (_usMOD_CALI_OFFSET > 0)
    {
        u8cur_ibcal += (MS_U8)abs(_usMOD_CALI_OFFSET);

        if(u8cur_ibcal >= 63)
        {
            u8cur_ibcal = 63;
        }
    }
    else if (_usMOD_CALI_OFFSET < 0)
    {
        if(u8cur_ibcal < abs(_usMOD_CALI_OFFSET))
        {
            u8cur_ibcal = 0;
        }
        else
        {
            u8cur_ibcal -= (MS_U8)abs(_usMOD_CALI_OFFSET);
        }

    }

    // Store the final value
    HAL_MOD_CAL_DBG(printf("\r\n (3)Store value = %d\n", u8cur_ibcal));

    _u8MOD_CALI_VALUE = (u8cur_ibcal & 0x3F);

    // copy the valur of ch6 to all channel( 0~20)
    MOD_W2BYTEMSK(REG_MOD_BK00_29_L,((MS_U16)(u8cur_ibcal&0x3F))<<4, 0x3F0);   // ch0
    MOD_W2BYTEMSK(REG_MOD_BK00_29_L,((MS_U16)(u8cur_ibcal&0x3F))<<10, 0xFC00); // ch1

    MOD_W2BYTEMSK(REG_MOD_BK00_2A_L,((MS_U16)(u8cur_ibcal&0x3F)), 0x3F);       // ch2
    MOD_W2BYTEMSK(REG_MOD_BK00_2A_L,((MS_U16)(u8cur_ibcal&0x3F))<<6, 0xFC0);   // ch3
    MOD_W2BYTEMSK(REG_MOD_BK00_2A_L,((MS_U16)(u8cur_ibcal&0x3F))<<12, 0xF000); // ch4
    MOD_W2BYTEMSK(REG_MOD_BK00_2B_L,((MS_U16)(u8cur_ibcal&0x3F))>>4, 0x03);
    MOD_W2BYTEMSK(REG_MOD_BK00_2B_L,((MS_U16)(u8cur_ibcal&0x3F))<<2, 0xFC);    // ch5
    MOD_W2BYTEMSK(REG_MOD_BK00_2B_L,((MS_U16)(u8cur_ibcal&0x3F))<<8, 0x3F00);  // ch6
    MOD_W2BYTEMSK(REG_MOD_BK00_2B_L,((MS_U16)(u8cur_ibcal&0x3F))<<14, 0xC000);   // ch7
    MOD_W2BYTEMSK(REG_MOD_BK00_2C_L,((MS_U16)(u8cur_ibcal&0x3F))>>2, 0x0F);
    MOD_W2BYTEMSK(REG_MOD_BK00_2C_L,((MS_U16)(u8cur_ibcal&0x3F))<<4, 0x3F0);    // ch8
    MOD_W2BYTEMSK(REG_MOD_BK00_2C_L,((MS_U16)(u8cur_ibcal&0x3F))<<10, 0xFC00);    // ch9

    MOD_W2BYTEMSK(REG_MOD_BK00_2D_L,((MS_U16)(u8cur_ibcal&0x3F)), 0x3F);       // ch10
    MOD_W2BYTEMSK(REG_MOD_BK00_2D_L,((MS_U16)(u8cur_ibcal&0x3F))<<6, 0xFC0);   // ch11
    MOD_W2BYTEMSK(REG_MOD_BK00_2D_L,((MS_U16)(u8cur_ibcal&0x3F))<<12, 0xF000); // ch12
    MOD_W2BYTEMSK(REG_MOD_BK00_2E_L,((MS_U16)(u8cur_ibcal&0x3F))>>4, 0x03);
    MOD_W2BYTEMSK(REG_MOD_BK00_2E_L,((MS_U16)(u8cur_ibcal&0x3F))<<2, 0xFC);    // ch13
    MOD_W2BYTEMSK(REG_MOD_BK00_2E_L,((MS_U16)(u8cur_ibcal&0x3F))<<8, 0x3F00);  // ch14
    MOD_W2BYTEMSK(REG_MOD_BK00_2E_L,((MS_U16)(u8cur_ibcal&0x3F))<<14, 0xC000);   // ch15
    MOD_W2BYTEMSK(REG_MOD_BK00_2F_L,((MS_U16)(u8cur_ibcal&0x3F))>>2, 0x0F);
    MOD_W2BYTEMSK(REG_MOD_BK00_2F_L,((MS_U16)(u8cur_ibcal&0x3F))<<4, 0x3F0);    // ch16
    MOD_W2BYTEMSK(REG_MOD_BK00_2F_L,((MS_U16)(u8cur_ibcal&0x3F))<<10, 0xFC00);    // ch17

    MOD_W2BYTEMSK(REG_MOD_BK00_30_L,((MS_U16)(u8cur_ibcal&0x3F)), 0x3F);       // ch18
    MOD_W2BYTEMSK(REG_MOD_BK00_30_L,((MS_U16)(u8cur_ibcal&0x3F))<<6, 0xFC0);   // ch19
    MOD_W2BYTEMSK(REG_MOD_BK00_30_L,((MS_U16)(u8cur_ibcal&0x3F))<<12, 0xF000); // ch20
    MOD_W2BYTEMSK(REG_MOD_BK00_31_L,((MS_U16)(u8cur_ibcal&0x3F))>>4, 0x03);

#if MOD_CAL_TIMER
    printf("[%s] takes %ld ms\n", __FUNCTION__, (MsOS_GetSystemTime()-delay_start_time));
#endif
    return (MS_U8)MOD_R2BYTEMSK(REG_MOD_BK00_2D_L, 0x003F);
#endif
}

PNL_Result MHal_PNL_MOD_Calibration(void)
{
    MS_U8 u8Cab;
    MS_U8 u8BackUSBPwrStatus;

    MOD_W2BYTEMSK(REG_MOD_BK00_7D_L, ((_u8MOD_CALI_TARGET&0x03) << 9), (BIT(10) | BIT(9)));

    u8BackUSBPwrStatus = R2BYTEMSK(L_BK_UTMI1(0x04), BIT(7));

    W2BYTEMSK(L_BK_UTMI1(0x04), 0x00, BIT(7));

    u8Cab = msModCurrentCalibration();

    W2BYTEMSK(L_BK_UTMI1(0x04), u8BackUSBPwrStatus, BIT(7));

    if(_stPnlInitData.eLPLL_Type !=E_PNL_TYPE_MINILVDS)
        MOD_W2BYTEMSK(REG_MOD_BK00_7D_L, u8Cab, LBMASK);

    return E_PNL_OK;

}

static void MHal_PNL_PowerDownLPLL(MS_BOOL bEnable)
{
    if(bEnable)
    {
        W2BYTEMSK(L_BK_LPLL(0x03), BIT(5), BIT(5));
    }
    else
    {
        W2BYTEMSK(L_BK_LPLL(0x03), FALSE, BIT(5));
    }
}

PNL_Result MHal_PNL_En(MS_BOOL bPanelOn, MS_BOOL bCalEn)
{
    MS_U8 u8Cab;
    MS_U8 u8BackUSBPwrStatus;


    if(bPanelOn)
    {
        // The order is PanelVCC -> delay pnlGetOnTiming1() -> VOP -> MOD
        // VOP
        SC_W2BYTEMSK(REG_SC_BK10_46_L, 0x4000, HBMASK);

        // mod power on
        MHal_MOD_PowerOn(ENABLE, _stPnlInitData.eLPLL_Type,_u8MOD_LVDS_Pair_Type, _stPnlInitData.u16OutputCFG0_7, _stPnlInitData.u16OutputCFG8_15, _stPnlInitData.u16OutputCFG16_21);

        // turn on LPLL
        MHal_PNL_PowerDownLPLL(FALSE);

        if(bCalEn)
        {

            u8BackUSBPwrStatus = R2BYTEMSK(L_BK_UTMI1(0x04), BIT(7));

            W2BYTEMSK(L_BK_UTMI1(0x04), 0x00, BIT(7));

            u8Cab = msModCurrentCalibration();

            W2BYTEMSK(L_BK_UTMI1(0x04), u8BackUSBPwrStatus, BIT(7));

        }
        if(!MHal_PNL_MOD_Control_Out_Swing(_u16PnlDefault_SwingLevel))
            printf(">>Swing Level setting error!!\n");
    }
    else
    {
        // The order is LPLL -> MOD -> VOP -> delay for MOD power off -> turn off VCC
        // LPLL
        MHal_PNL_PowerDownLPLL(TRUE);

        MHal_MOD_PowerOn(DISABLE, _stPnlInitData.eLPLL_Type,_u8MOD_LVDS_Pair_Type, _stPnlInitData.u16OutputCFG0_7, _stPnlInitData.u16OutputCFG8_15, _stPnlInitData.u16OutputCFG16_21);
        // VOP
        if(_stPnlInitData.eLPLL_Type == E_PNL_TYPE_LVDS ||
            _stPnlInitData.eLPLL_Type == E_PNL_TYPE_DAC_I ||
            _stPnlInitData.eLPLL_Type == E_PNL_TYPE_DAC_P)//(bIsLVDS)
        {
            SC_W2BYTEMSK(REG_SC_BK10_46_L, 0xFF, LBMASK);
        }
        else
        {
            SC_W2BYTEMSK(REG_SC_BK10_46_L, 0x00, 0xFF);
        }
    }

    return E_PNL_OK;
}

void MHal_PNL_SetOutputPattern(MS_BOOL bEnable, MS_U16 u16Red , MS_U16 u16Green, MS_U16 u16Blue)
{
    if (bEnable)
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_02_L, u16Red , 0x03FF);
        MOD_W2BYTEMSK(REG_MOD_BK00_03_L, u16Green , 0x03FF);
        MOD_W2BYTEMSK(REG_MOD_BK00_04_L, u16Blue , 0x03FF);
        MsOS_DelayTask(10);
        MOD_W2BYTEMSK(REG_MOD_BK00_01_L, BIT(15) , BIT(15));
    }
    else
    {
        MOD_W2BYTEMSK(REG_MOD_BK00_01_L, DISABLE , BIT(15));
    }

}

void MHal_PNL_Switch_LPLL_SubBank(MS_U16 u16Bank)
{
    UNUSED(u16Bank);
}


//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
//MS_U32 gu32PnlRiuBaseAddr = 0;
//MS_U32 gu32PMRiuBaseAddr = 0;

#endif

