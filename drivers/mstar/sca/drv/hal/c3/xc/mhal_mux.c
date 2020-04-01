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
// [mhal_mux.c]
// Date: 20090220
// Descriptions: Add a new mux layer for HW setting
//==============================================================================

#define  MHAL_MUX_C


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/wait.h>
#endif

// Common Definition
#include "MsCommon.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"

// Registers
#include "drvXC_IOPort.h"
#include "xc_hwreg_utility2.h"
#include "hwreg_adc_atop.h"
#include "hwreg_adc_dtop.h"
//#include "hwreg_chiptop.h"
#include "hwreg_hdmi.h"
#include "hwreg_pm_sleep.h"//alex_tung

#include "apiXC.h"

#include "drv_sc_ip.h"
#include "mvideo_context.h"
#include "mhal_sc.h"

#include "xc_Analog_Reg.h"

#include "mhal_mux.h"
#include "apiXC_Adc.h"
#include "drvXC_ADC_Internal.h"
#include "mvideo_context.h"

#include "mhal_adc.h"
#include "drvXC_HDMI_if.h"
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Structures
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


/******************************************************************************/
/// MUX
/******************************************************************************/
void Hal_SC_mux_set_dvi_mux(MS_U8 PortId)
{
    //DVI port 0: input A
    //DVI port 1: input B
    //DVI port 2: input D
    //DVI port 3: input C
    if(g_bIsIMMESWITCH)
    {
        if(PortId==HAL_DVI_IP_A) // DVI port 0
        {
            if(g_bIMMESWITCH_DVI_POWERSAVING)
            {
                // DVI slowly update

                #if 0
                // Must turn off when the input port is at off-line status.
                W2BYTEMSK( REG_DVI_DTOP_29_L, 0xE300, HBMASK); // [15]:update Bch slowly; [14:8]: 0x63(100 lines)
                W2BYTEMSK( REG_DVI_DTOP_2A_L, 0xE3E3, HBMASK|LBMASK); // [15:8]: update Rch slowly, [7:0]:update Gch slowly
                #else //Sync from SEC
                // Disable DVI update phase slowly for X5 X6 HDCP test
                W2BYTEMSK( REG_DVI_DTOP_29_L, 0, HBMASK); // [15]:update Bch slowly; [14:8]: 0x63(100 lines)
                W2BYTE( REG_DVI_DTOP_2A_L, 0); // [15:8]: update Rch slowly, [7:0]:update Gch slowly
                #endif
                W2BYTEMSK( REG_DVI_DTOP1_29_L, 0, BIT(15)); // [15]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP1_2A_L, 0, BIT(15)|BIT(7)); // [15]: turn off slowly updated, [7]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP2_29_L, 0, BIT(15)); // [15]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP2_2A_L, 0, BIT(15)|BIT(7)); // [15]: turn off slowly updated, [7]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP3_29_L, 0, BIT(15)); // [15]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP3_2A_L, 0, BIT(15)|BIT(7)); // [15]: turn off slowly updated, [7]: turn off slowly updated
                // DVI power saving mode
                W2BYTEMSK(REG_DVI_PS_00_L, 0, BIT(0)); // turn off DVI power saving mode
                W2BYTEMSK(REG_DVI_PS1_00_L, BIT(0), BIT(0)); // turn on DVI1 power saving mode
                W2BYTEMSK(REG_DVI_PS2_00_L, BIT(0), BIT(0)); // turn on DVI2 power saving mode
                W2BYTEMSK(REG_DVI_PS3_00_L, BIT(0), BIT(0)); // turn on DVI3 power saving mode

                #if 0
                // Turn off Ch2 as DE align out and cycle align delay
                W2BYTEMSK(REG_DVI_PS_01_L, 0, BMASK(9:8)); //[9]: DE cycle align delay, [8]: no select Ch2 DE as DE align out
                #else //Sync from SEC
                W2BYTEMSK(REG_DVI_PS_01_L, BIT(8), BMASK(9:8)); //[9]: DE cycle align delay, [8]: no select Ch2 DE as DE align out
                W2BYTEMSK(REG_DVI_PS_01_L, BMASK(9:8), BMASK(9:8)); //[9]: DE cycle align delay, [8]: no select Ch2 DE as DE align out
                #endif
            }
            // Setup display port
            W2BYTEMSK(REG_DVI_ATOP_6A_L, 0, BMASK(1:0)); // switch display port to port 0
        }
        else if(PortId==HAL_DVI_IP_B) // DVI port 1
        {
            if(g_bIMMESWITCH_DVI_POWERSAVING)
            {
                // DVI slowly update
                #if 0 // Disable DVI update phase slowly for X5 X6 HDCP test
                W2BYTEMSK( REG_DVI_DTOP1_29_L, 0xE300, HBMASK); // [15]:update Bch slowly; [14:8]: 0x63(100 lines)
                W2BYTEMSK( REG_DVI_DTOP1_2A_L, 0xE3E3, HBMASK|LBMASK); // [15:8]: update Rch slowly, [7:0]:update Gch slowly
                #else //Sync from SEC
                W2BYTEMSK( REG_DVI_DTOP1_29_L, 0, HBMASK); // [15]:update Bch slowly; [14:8]: 0x63(100 lines)
                W2BYTE( REG_DVI_DTOP1_2A_L, 0); // [15:8]: update Rch slowly, [7:0]:update Gch slowly
                #endif

                W2BYTEMSK( REG_DVI_DTOP_29_L, 0, BIT(15)); // [15]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP_2A_L, 0, BIT(15)|BIT(7)); // [15]: turn off slowly updated, [7]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP2_29_L, 0, BIT(15)); // [15]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP2_2A_L, 0, BIT(15)|BIT(7)); // [15]: turn off slowly updated, [7]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP3_29_L, 0, BIT(15)); // [15]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP3_2A_L, 0, BIT(15)|BIT(7)); // [15]: turn off slowly updated, [7]: turn off slowly updated
                // DVI power saving mode
                W2BYTEMSK(REG_DVI_PS1_00_L, 0, BIT(0)); // turn off DVI power saving mode
                W2BYTEMSK(REG_DVI_PS_00_L, BIT(0), BIT(0)); // turn on DVI1 power saving mode
                W2BYTEMSK(REG_DVI_PS2_00_L, BIT(0), BIT(0)); // turn on DVI2 power saving mode
                W2BYTEMSK(REG_DVI_PS3_00_L, BIT(0), BIT(0)); // turn on DVI3 power saving mode
                #if 0
                // Turn off Ch2 as DE align out and cycle align delay
                W2BYTEMSK(REG_DVI_PS1_01_L, 0, BMASK(9:8)); //[9]: DE cycle align delay, [8]: no select Ch2 DE as DE align out
                #else //Sync from SEC
                W2BYTEMSK(REG_DVI_PS1_01_L, BIT(8), BMASK(9:8)); //[9]: DE cycle align delay, [8]: no select Ch2 DE as DE align out
                W2BYTEMSK(REG_DVI_PS1_01_L, BMASK(9:8), BMASK(9:8)); //[9]: DE cycle align delay, [8]: no select Ch2 DE as DE align out
                #endif

            }
            // Setup display port
            W2BYTEMSK(REG_DVI_ATOP_6A_L, BIT(0), BMASK(1:0)); // switch display port to port 0
        }
        else if(PortId==HAL_DVI_IP_D) // DVI port 2
        {
            if(g_bIMMESWITCH_DVI_POWERSAVING)
            {
                // DVI slowly update
                #if 0 // Disable DVI update phase slowly for X5 X6 HDCP test
                W2BYTEMSK( REG_DVI_DTOP2_29_L, 0xE300, HBMASK); // [15]:update Bch slowly; [14:8]: 0x63(100 lines)
                W2BYTEMSK( REG_DVI_DTOP2_2A_L, 0xE3E3, HBMASK|LBMASK); // [15:8]: update Rch slowly, [7:0]:update Gch slowly
                #else //Sync from SEC
                W2BYTEMSK( REG_DVI_DTOP2_29_L, 0, HBMASK); // [15]:update Bch slowly; [14:8]: 0x63(100 lines)
                W2BYTE( REG_DVI_DTOP2_2A_L, 0); // [15:8]: update Rch slowly, [7:0]:update Gch slowly
                #endif

                W2BYTEMSK( REG_DVI_DTOP1_29_L, 0, BIT(15)); // [15]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP1_2A_L, 0, BIT(15)|BIT(7)); // [15]: turn off slowly updated, [7]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP_29_L, 0, BIT(15)); // [15]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP_2A_L, 0, BIT(15)|BIT(7)); // [15]: turn off slowly updated, [7]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP3_29_L, 0, BIT(15)); // [15]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP3_2A_L, 0, BIT(15)|BIT(7)); // [15]: turn off slowly updated, [7]: turn off slowly updated
                // DVI power saving mode
                W2BYTEMSK(REG_DVI_PS2_00_L, 0, BIT(0)); // turn off DVI power saving mode
                W2BYTEMSK(REG_DVI_PS1_00_L, BIT(0), BIT(0)); // turn on DVI1 power saving mode
                W2BYTEMSK(REG_DVI_PS_00_L, BIT(0), BIT(0)); // turn on DVI2 power saving mode
                W2BYTEMSK(REG_DVI_PS3_00_L, BIT(0), BIT(0)); // turn on DVI3 power saving mode
                #if 0
                // Turn off Ch2 as DE align out and cycle align delay
                W2BYTEMSK(REG_DVI_PS2_01_L, 0, BMASK(9:8)); //[9]: DE cycle align delay, [8]: no select Ch2 DE as DE align out
                #else //Sync from SEC
                W2BYTEMSK(REG_DVI_PS2_01_L, BIT(8), BMASK(9:8)); //[9]: DE cycle align delay, [8]: no select Ch2 DE as DE align out
                W2BYTEMSK(REG_DVI_PS2_01_L, BMASK(9:8), BMASK(9:8)); //[9]: DE cycle align delay, [8]: no select Ch2 DE as DE align out
                #endif
            }
            // Setup display port
            W2BYTEMSK(REG_DVI_ATOP_6A_L, BIT(1), BMASK(1:0)); // switch display port to port 0
        }
        else if(PortId==HAL_DVI_IP_C) // DVI port 3
        {
            if(g_bIMMESWITCH_DVI_POWERSAVING)
            {
                // DVI slowly update
                #if 0 // Disable DVI update phase slowly for X5 X6 HDCP test
                W2BYTEMSK( REG_DVI_DTOP3_29_L, 0xE300, HBMASK); // [15]:update Bch slowly; [14:8]: 0x63(100 lines)
                W2BYTEMSK( REG_DVI_DTOP3_2A_L, 0xE3E3, HBMASK|LBMASK); // [15:8]: update Rch slowly, [7:0]:update Gch slowly
                #else //Sync from SEC
                W2BYTEMSK( REG_DVI_DTOP3_29_L, 0, HBMASK); // [15]:update Bch slowly; [14:8]: 0x63(100 lines)
                W2BYTE( REG_DVI_DTOP3_2A_L, 0); // [15:8]: update Rch slowly, [7:0]:update Gch slowly
                #endif

                W2BYTEMSK( REG_DVI_DTOP1_29_L, 0, BIT(15)); // [15]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP1_2A_L, 0, BIT(15)|BIT(7)); // [15]: turn off slowly updated, [7]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP2_29_L, 0, BIT(15)); // [15]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP2_2A_L, 0, BIT(15)|BIT(7)); // [15]: turn off slowly updated, [7]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP_29_L, 0, BIT(15)); // [15]: turn off slowly updated
                W2BYTEMSK( REG_DVI_DTOP_2A_L, 0, BIT(15)|BIT(7)); // [15]: turn off slowly updated, [7]: turn off slowly updated
                // DVI power saving mode
                W2BYTEMSK(REG_DVI_PS3_00_L, 0, BIT(0)); // turn off DVI power saving mode
                W2BYTEMSK(REG_DVI_PS1_00_L, BIT(0), BIT(0)); // turn on DVI1 power saving mode
                W2BYTEMSK(REG_DVI_PS2_00_L, BIT(0), BIT(0)); // turn on DVI2 power saving mode
                W2BYTEMSK(REG_DVI_PS_00_L, BIT(0), BIT(0)); // turn on DVI3 power saving mode
                #if 0
                // Turn off Ch2 as DE align out and cycle align delay
                W2BYTEMSK(REG_DVI_PS3_01_L, 0, BMASK(9:8)); //[9]: DE cycle align delay, [8]: no select Ch2 DE as DE align out
                #else //Sync from SEC
                W2BYTEMSK(REG_DVI_PS3_01_L, BIT(8), BMASK(9:8)); //[9]: DE cycle align delay, [8]: no select Ch2 DE as DE align out
                W2BYTEMSK(REG_DVI_PS3_01_L, BMASK(9:8), BMASK(9:8)); //[9]: DE cycle align delay, [8]: no select Ch2 DE as DE align out
                #endif
            }
            // Setup display port
            W2BYTEMSK(REG_DVI_ATOP_6A_L, BMASK(1:0), BMASK(1:0)); // switch display port to port 0
        }
    }
    else
    {
        //power and port control
        switch (PortId)
        {
            case HAL_DVI_IP_A: // DVI0 (port A)
            #ifdef DVI_OFFLINE_CLK_DETECTION
                // DVI Clock power control
                W2BYTEMSK(REG_DVI_ATOP_06_L, 0, 0xFFFF); // enable DVI0 clock power
                W2BYTEMSK(REG_DVI_ATOP1_06_L, 0x7FFE, 0x7FFE); // disable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP2_06_L, 0x7FFE, 0x7FFE); // disable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP_03_L, 0x7FFE, 0x7FFE); // disable DVI0 clock power
                // DVI PLL power control
                W2BYTEMSK(REG_DVI_ATOP_60_L, 0, 0xFFFF); // enable DVI0 PLL power
                W2BYTEMSK(REG_DVI_ATOP1_60_L, 0xFFDF, 0xFFDF); // disable DVI1 PLL power, DVIPLL regulator should be enable
                W2BYTEMSK(REG_DVI_ATOP2_60_L, 0xFFDF, 0xFFDF); // disable DVI2 PLL power
                W2BYTEMSK(REG_DVI_ATOP_69_L, 0xFFDF, 0xFFDF); // disable DVI3 PLL power
            #else
                PM_W2BYTE(REG_PM_SLEEP_4B_L, 0xE0EE, 0xF0FF); // [15:12]: PD_BG, [7:4]: PD_CLKIN, [3:0]: PD_IBGREX
                // DVI Clock power control
                W2BYTE(REG_DVI_ATOP_06_L, 0); // enable DVI0 clock power
                W2BYTEMSK(REG_DVI_ATOP1_06_L, 0xFFFE, 0xFFFE); // disable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP2_06_L, 0xFFFE, 0xFFFE); // disable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP_03_L, 0xFFFE, 0xFFFE); // disable DVI0 clock power
                // DVI PLL power control
                W2BYTE(REG_DVI_ATOP_60_L, 0); // enable DVI0 PLL power
                W2BYTE(REG_DVI_ATOP1_60_L, 0xFFFF); // disable DVI1 PLL power
                W2BYTE(REG_DVI_ATOP2_60_L, 0xFFFF); // disable DVI2 PLL power
                W2BYTE(REG_DVI_ATOP_69_L, 0xFFFF); // disable DVI3 PLL power
            #endif // DVI_OFFLINE_CLK_DETECTION
                MDrv_WriteByteMask(REG_DVI_ATOP_7F_L, 0x00, 0x3F); //power on for Port A DVI DPLPHI/DPLPHQ
                MDrv_WriteByteMask(REG_DVI_ATOP1_74_L, 0x3F, 0x3F); //power off for Port B DVI DPLPHI/DPLPHQ
                MDrv_WriteByteMask(REG_DVI_ATOP_7F_H, 0x3F, 0x3F); //power off for Port C DVI DPLPHI/DPLPHQ
                MDrv_WriteByteMask(REG_DVI_ATOP2_74_L, 0x3F, 0x3F); //power off for Port D DVI DPLPHI/DPLPHQ
                // DVI port enable
                W2BYTEMSK(REG_DVI_ATOP_6A_L, 0, BMASK(1:0)); // switch display port to port 0
                break;
            case HAL_DVI_IP_B: // DVI1 (port B)
            #ifdef DVI_OFFLINE_CLK_DETECTION
                // DVI Clock power control
                W2BYTEMSK(REG_DVI_ATOP_06_L, 0x7F7E, 0x7FFE); // disable DVI0 clock power, bit[7]=0 audio clock
                W2BYTE(REG_DVI_ATOP1_06_L, 0); // enable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP2_06_L, 0x7FFE, 0x7FFE); // disable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP_03_L, 0x7FFE, 0x7FFE); // disable DVI0 clock power
                // DVI PLL power control
                W2BYTEMSK(REG_DVI_ATOP_60_L, 0xFFDF, 0xFFDF); // disable DVI0 PLL power, DVIPLL regulator should be enable
                W2BYTE(REG_DVI_ATOP1_60_L, 0); // enable DVI1 PLL power
                W2BYTEMSK(REG_DVI_ATOP2_60_L, 0xFFDF, 0xFFDF); // disable DVI2 PLL power
                W2BYTEMSK(REG_DVI_ATOP_69_L, 0xFFDF, 0xFFDF); // disable DVI3 PLL power
            #else
                PM_W2BYTE(REG_PM_SLEEP_4B_L, 0xD0DD, 0xF0FF); // [15:12]: PD_BG, [7:4]: PD_CLKIN, [3:0]: PD_IBGREX
                // DVI Clock power control
                W2BYTEMSK(REG_DVI_ATOP_06_L, 0xFF7E, 0xFFFE); // disable DVI0 clock power, bit[7]=0 audio clock
                W2BYTE(REG_DVI_ATOP1_06_L, 0); // enable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP2_06_L, 0xFFFE, 0xFFFE); // disable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP_03_L, 0xFFFE, 0xFFFE); // disable DVI0 clock power
                // DVI PLL power control
                W2BYTE(REG_DVI_ATOP_60_L, 0xFFFF); // disable DVI0 PLL power
                W2BYTE(REG_DVI_ATOP1_60_L, 0); // enable DVI1 PLL power
                W2BYTE(REG_DVI_ATOP2_60_L, 0xFFFF); // disable DVI2 PLL power
                W2BYTE(REG_DVI_ATOP_69_L, 0xFFFF); // disable DVI3 PLL power
            #endif // DVI_OFFLINE_CLK_DETECTION
                MDrv_WriteByteMask(REG_DVI_ATOP_7F_L, 0x3F, 0x3F); //power off for Port A DVI DPLPHI/DPLPHQ
                MDrv_WriteByteMask(REG_DVI_ATOP1_74_L, 0x00, 0x3F); //power on for Port B DVI DPLPHI/DPLPHQ
                MDrv_WriteByteMask(REG_DVI_ATOP_7F_H, 0x3F, 0x3F); //power off for Port C DVI DPLPHI/DPLPHQ
                MDrv_WriteByteMask(REG_DVI_ATOP2_74_L, 0x3F, 0x3F); //power off for Port D DVI DPLPHI/DPLPHQ
                // DVI port enable
                W2BYTEMSK(REG_DVI_ATOP_6A_L, BIT(0), BMASK(1:0)); // switch display port to port 1
                break;
            case HAL_DVI_IP_D: // DVI2 (port D)
            #ifdef DVI_OFFLINE_CLK_DETECTION
                // DVI Clock power control
                W2BYTEMSK(REG_DVI_ATOP_06_L, 0x7F7E, 0x7FFE); // disable DVI0 clock power, bit[7]=0 audio clock
                W2BYTEMSK(REG_DVI_ATOP1_06_L, 0x7FFE, 0x7FFE); // disable DVI1 clock power
                W2BYTE(REG_DVI_ATOP2_06_L, 0); // enable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP_03_L, 0x7FFE, 0x7FFE); // disable DVI0 clock power
                // DVI PLL power control
                W2BYTEMSK(REG_DVI_ATOP_60_L, 0xFFDF, 0xFFDF); // disable DVI0 PLL power, DVIPLL regulator should be enable
                W2BYTEMSK(REG_DVI_ATOP1_60_L, 0xFFDF, 0xFFDF); // disable DVI1 PLL power
                W2BYTE(REG_DVI_ATOP2_60_L, 0); // enable DVI2 PLL power
                W2BYTEMSK(REG_DVI_ATOP_69_L, 0xFFDF, 0xFFDF); // disable DVI3 PLL power
            #else
                PM_W2BYTE(REG_PM_SLEEP_4B_L, 0xB0BB, 0xF0FF); // [15:12]: PD_BG, [7:4]: PD_CLKIN, [3:0]: PD_IBGREX
                // DVI Clock power control
                W2BYTEMSK(REG_DVI_ATOP_06_L, 0xFF7E, 0xFFFE); // disable DVI0 clock power, bit[7]=0 audio clock
                W2BYTEMSK(REG_DVI_ATOP1_06_L, 0xFFFE, 0xFFFE); // disable DVI1 clock power
                W2BYTE(REG_DVI_ATOP2_06_L, 0); // enable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP_03_L, 0xFFFE, 0xFFFE); // disable DVI0 clock power
                // DVI PLL power control
                W2BYTE(REG_DVI_ATOP_60_L, 0xFFFF); // disable DVI0 PLL power
                W2BYTE(REG_DVI_ATOP1_60_L, 0xFFFF); // disable DVI1 PLL power
                W2BYTE(REG_DVI_ATOP2_60_L, 0); // enable DVI2 PLL power
                W2BYTE(REG_DVI_ATOP_69_L, 0xFFFF); // disable DVI3 PLL power
            #endif // DVI_OFFLINE_CLK_DETECTION
                MDrv_WriteByteMask(REG_DVI_ATOP_7F_L, 0x3F, 0x3F); //power off for Port A DVI DPLPHI/DPLPHQ
                MDrv_WriteByteMask(REG_DVI_ATOP1_74_L, 0x3F, 0x3F); //power off for Port B DVI DPLPHI/DPLPHQ
                MDrv_WriteByteMask(REG_DVI_ATOP_7F_H, 0x3F, 0x3F); //power off for Port C DVI DPLPHI/DPLPHQ
                MDrv_WriteByteMask(REG_DVI_ATOP2_74_L, 0x00, 0x3F); //power on for Port D DVI DPLPHI/DPLPHQ
                // DVI port enable
                W2BYTEMSK(REG_DVI_ATOP_6A_L, BIT(1), BMASK(1:0)); // switch display port to port 3
                break;
            case HAL_DVI_IP_C: // DVI3 (port C)
            #ifdef DVI_OFFLINE_CLK_DETECTION
                // DVI Clock power control
                W2BYTEMSK(REG_DVI_ATOP_06_L, 0x7F7E, 0x7FFE); // disable DVI0 clock power, bit[7]=0 audio clock
                W2BYTEMSK(REG_DVI_ATOP1_06_L, 0x7FFE, 0x7FFE); // disable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP2_06_L, 0x7FFE, 0x7FFE); // disable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP_03_L, 0, 0xFFFF); // enable DVI0 clock power
                // DVI PLL power control
                W2BYTEMSK(REG_DVI_ATOP_60_L, 0xFFDF, 0xFFDF); // disable DVI0 PLL power, DVIPLL regulator should be enable
                W2BYTEMSK(REG_DVI_ATOP1_60_L, 0xFFDF, 0xFFDF); // disable DVI1 PLL power
                W2BYTEMSK(REG_DVI_ATOP2_60_L, 0xFFDF, 0xFFDF); // disable DVI2 PLL power
                W2BYTEMSK(REG_DVI_ATOP_69_L, 0, 0xFFFF); // enable DVI3 PLL power
            #else
                PM_W2BYTE(REG_PM_SLEEP_4B_L, 0x7077, 0xF0FF); // [15:12]: PD_BG, [7:4]: PD_CLKIN, [3:0]: PD_IBGREX
                // DVI Clock power control
                W2BYTEMSK(REG_DVI_ATOP_06_L, 0xFF7E, 0xFFFE); // disable DVI0 clock power, bit[7]=0 audio clock
                W2BYTEMSK(REG_DVI_ATOP1_06_L, 0xFFFE, 0xFFFE); // disable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP2_06_L, 0xFFFE, 0xFFFE); // disable DVI1 clock power
                W2BYTE(REG_DVI_ATOP_03_L, 0); // enable DVI0 clock power
                // DVI PLL power control
                W2BYTE(REG_DVI_ATOP_60_L, 0xFFFF); // disable DVI0 PLL power
                W2BYTE(REG_DVI_ATOP1_60_L, 0xFFFF); // disable DVI1 PLL power
                W2BYTE(REG_DVI_ATOP2_60_L, 0xFFFF); // disable DVI2 PLL power
                W2BYTE(REG_DVI_ATOP_69_L, 0); // enable DVI3 PLL power
            #endif // DVI_OFFLINE_CLK_DETECTION
                MDrv_WriteByteMask(REG_DVI_ATOP_7F_L, 0x3F, 0x3F); //power off for Port A DVI DPLPHI/DPLPHQ
                MDrv_WriteByteMask(REG_DVI_ATOP1_74_L, 0x3F, 0x3F); //power off for Port B DVI DPLPHI/DPLPHQ
                MDrv_WriteByteMask(REG_DVI_ATOP_7F_H, 0x00, 0x3F); //power on for Port C DVI DPLPHI/DPLPHQ
                MDrv_WriteByteMask(REG_DVI_ATOP2_74_L, 0x3F, 0x3F); //power off for Port D DVI DPLPHI/DPLPHQ
                // DVI port enable
                W2BYTEMSK(REG_DVI_ATOP_6A_L, BMASK(1:0), BMASK(1:0)); // switch display port to port 2
                break;
            default: // turn off all DVI path
            #ifdef DVI_OFFLINE_CLK_DETECTION
                // DVI Clock power control
                W2BYTEMSK(REG_DVI_ATOP_06_L, 0x7FFE, 0x7FFE); // disable DVI0 clock power
                W2BYTEMSK(REG_DVI_ATOP1_06_L, 0x7FFE, 0x7FFE); // disable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP2_06_L, 0x7FFE, 0x7FFE); // disable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP_03_L, 0x7FFE, 0x7FFE); // enable DVI0 clock power
                // DVI PLL power control
                W2BYTEMSK(REG_DVI_ATOP_60_L, 0xFFDF, 0xFFDF); // disable DVI0 PLL power, DVIPLL regulator should be enable
                W2BYTEMSK(REG_DVI_ATOP1_60_L, 0xFFDF, 0xFFDF); // disable DVI1 PLL power
                W2BYTEMSK(REG_DVI_ATOP2_60_L, 0xFFDF, 0xFFDF); // disable DVI2 PLL power
                W2BYTEMSK(REG_DVI_ATOP_69_L, 0xFFDF, 0xFFDF); // disable DVI3 PLL power
            #else
                PM_W2BYTE(REG_PM_SLEEP_4B_L, 0xF0FF, 0xF0FF); // [15:12]: PD_BG, [7:4]: PD_CLKIN, [3:0]: PD_IBGREX
                // DVI Clock power control
                W2BYTEMSK(REG_DVI_ATOP_06_L, 0xFFFE, 0xFFFE); // disable DVI0 clock power
                W2BYTEMSK(REG_DVI_ATOP1_06_L, 0xFFFE, 0xFFFE); // disable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP2_06_L, 0xFFFE, 0xFFFE); // disable DVI1 clock power
                W2BYTEMSK(REG_DVI_ATOP_03_L, 0xFFFE, 0xFFFE); // enable DVI0 clock power
                // DVI PLL power control
                W2BYTE(REG_DVI_ATOP_60_L, 0xFFFF); // disable DVI0 PLL power
                W2BYTE(REG_DVI_ATOP1_60_L, 0xFFFF); // disable DVI1 PLL power
                W2BYTE(REG_DVI_ATOP2_60_L, 0xFFFF); // disable DVI2 PLL power
                W2BYTE(REG_DVI_ATOP_69_L, 0xFFFF); // disable DVI3 PLL power
            #endif // DVI_OFFLINE_CLK_DETECTION
                MDrv_WriteByteMask(REG_DVI_ATOP_7F_L, 0x3F, 0x3F); //power off for Port A DVI DPLPHI/DPLPHQ
                MDrv_WriteByteMask(REG_DVI_ATOP1_74_L, 0x3F, 0x3F); //power off for Port B DVI DPLPHI/DPLPHQ
                MDrv_WriteByteMask(REG_DVI_ATOP_7F_H, 0x3F, 0x3F); //power off for Port C DVI DPLPHI/DPLPHQ
                MDrv_WriteByteMask(REG_DVI_ATOP2_74_L, 0x3F, 0x3F); //power off for Port D DVI DPLPHI/DPLPHQ
                break;
        }
    }

}

void Hal_SC_mux_set_analog_mux(MS_U8 PortId)
{
    // 0x2501
    //W2BYTEMSK(REG_ADC_ATOP_01_L, PortId & 0x03, 0x03);
    return;
}

void Hal_SC_mux_set_adc_y_mux(MS_U8 PortId)
{
    // ADC VD Ymux
    //W2BYTEMSK(REG_ADC_ATOP_02_L, PortId , BITMASK(3:0) );
    return;
}

void Hal_SC_mux_set_adc_c_mux(MS_U8 PortId)
{
    // ADC VD Cmux
    //W2BYTEMSK(REG_ADC_ATOP_02_L, (PortId<<4), BITMASK(7:4) ); // ADC_VD_CMUX_MASK
    return;
}

void Hal_SC_mux_set_adc_AV_ymux(MS_U8 PortId)
{
    W2BYTEMSK(REG_ADC_ATOP_02_L, PortId , BITMASK(3:0) );
    return;
}

static void _Hal_SC_mux_set_cvbsout1_y_mux(E_INPUT_PORT_TYPE u8Ymux)
{
    MS_U16 u16mux;

    if( u8Ymux >= HAL_INPUTPORT_YMUX_CVBS0 && u8Ymux < (HAL_INPUTPORT_YMUX_CVBS0 + NUMBER_OF_CVBS_PORT) )
    {
        // 0 ~ 7
        u16mux = u8Ymux - HAL_INPUTPORT_YMUX_CVBS0;
        W2BYTEMSK(REG_ADC_ATOP_51_L, u16mux , 0x000F);
    }
    /*
        // DAC port control is obsolated in mux driver. It is controlled in ADC table.
    */
}

static void _Hal_SC_mux_set_cvbsout1_c_mux(E_INPUT_PORT_TYPE u8Cmux)
{
    MS_U16 u16mux;
    if( u8Cmux >= HAL_INPUTPORT_CMUX_CVBS0 && u8Cmux < (HAL_INPUTPORT_CMUX_CVBS0 + NUMBER_OF_CVBS_PORT) )
    {
        u16mux = u8Cmux - HAL_INPUTPORT_CMUX_CVBS0;
        W2BYTEMSK(REG_ADC_ATOP_51_L, u16mux<<4 , 0x00F0);
    }
    /*
        // DAC port control is obsolated in mux driver. It is controlled in ADC table.
    */
}

static void _Hal_SC_mux_set_cvbsout2_y_mux(E_INPUT_PORT_TYPE u8Ymux)
{
    MS_U16 u16mux;

    if( u8Ymux >= HAL_INPUTPORT_YMUX_CVBS0 && u8Ymux < (HAL_INPUTPORT_YMUX_CVBS0 + NUMBER_OF_CVBS_PORT) )
    {
        u16mux = u8Ymux - HAL_INPUTPORT_YMUX_CVBS0;
        W2BYTEMSK(REG_ADC_ATOP_53_L, u16mux , 0x000F);
    }
    /*
        // DAC port control is obsolated in mux driver. It is controlled in ADC table.
    */
}

static void _Hal_SC_mux_set_cvbsout2_c_mux(E_INPUT_PORT_TYPE u8Cmux)
{
    MS_U16 u16mux;
    if( u8Cmux >= HAL_INPUTPORT_CMUX_CVBS0 && u8Cmux < (HAL_INPUTPORT_CMUX_CVBS0 + NUMBER_OF_CVBS_PORT) )
    {
        u16mux = u8Cmux - HAL_INPUTPORT_CMUX_CVBS0;
        W2BYTEMSK(REG_ADC_ATOP_53_L, u16mux<<4 , 0x00F0);
    }
    /*
        // DAC port control is obsolated in mux driver. It is controlled in ADC table.
    */
}

void Hal_SC_mux_set_mainwin_ip_mux( MS_U8 u8Data_Mux, MS_U8 u8Clk_Mux)
{
    //MDrv_WriteByteMask(L_BK_IPMUX(0x01), u8Data_Mux << 4, 0xF0);
    //MDrv_WriteByteMask(REG_CKG_IDCLK2, u8Clk_Mux << 2, CKG_IDCLK2_MASK);
    Hal_SC_set_ip_mux(u8Data_Mux, u8Clk_Mux, HAL_OUTPUTPORT_SCALER_MAIN_WINDOW);
}

MS_BOOL Hal_SC_mux_get_mainwin_ip_mux( MS_U8 *pU8Data_Mux, MS_U8 *pU8Clk_Mux)
{
    MS_BOOL bRet = FALSE;
    if(pU8Data_Mux && pU8Clk_Mux)
    {
        *pU8Data_Mux = (MDrv_ReadRegBit(L_BK_IPMUX(0x01), 0xF0)) >> 4;
        *pU8Clk_Mux = (MDrv_ReadRegBit(REG_CKG_IDCLK2, CKG_IDCLK2_MASK)) >> 2;
        bRet = TRUE;
    }
    else
    {
        bRet = FALSE;
    }
    return bRet;
}

void Hal_SC_set_subwin_ip_mux( MS_U8 u8Data_Mux, MS_U8 u8Clk_Mux)
{
    //MDrv_WriteByteMask(L_BK_IPMUX(0x01), u8Data_Mux, 0x0F);
    //MDrv_WriteByteMask(REG_CKG_IDCLK1, u8Clk_Mux << 2, CKG_IDCLK1_MASK);
    Hal_SC_set_ip_mux(u8Data_Mux, u8Clk_Mux, HAL_OUTPUTPORT_SCALER_SUB_WINDOW1);
}

void Hal_SC_set_ip_mux( MS_U8 u8Data_Mux, MS_U8 u8Clk_Mux, E_OUTPUT_PORT_TYPE eDest)
{
    MS_U32 u32Data_reg, u32CLK_reg, u32PreCLK_reg;
    MS_U8  u8Data_val, u8CLK_val, u8PreCLK_val;
    MS_U8  u8Data_msk, u8CLK_msk, u8PreCLK_msk;
    MS_BOOL bOut = TRUE;


    switch(eDest)
    {
    case HAL_OUTPUTPORT_SCALER_MAIN_WINDOW:
        u32Data_reg   = L_BK_IPMUX(0x01);
        u32CLK_reg    = REG_CKG_IDCLK2;
        u32PreCLK_reg = REG_CKG_PRE_IDCLK2;

        u8CLK_msk    = CKG_IDCLK2_MASK;
        u8PreCLK_msk = CKG_PRE_IDCLK2_MASK;

        u8Data_val = u8Data_Mux << 4;
        u8Data_msk = 0xF0;
        break;

    case HAL_OUTPUTPORT_SCALER_SUB_WINDOW1:
        u32Data_reg   = L_BK_IPMUX(0x01);
        u32CLK_reg    = REG_CKG_IDCLK1;
        u32PreCLK_reg = REG_CKG_PRE_IDCLK1;

        u8CLK_msk    = CKG_IDCLK1_MASK;
        u8PreCLK_msk = CKG_PRE_IDCLK1_MASK;

        u8Data_val = u8Data_Mux;
        u8Data_msk = 0x0F;

        break;

    case HAL_OUTPUTPORT_SCALER1_MAIN_WINDOW:
        u32Data_reg   = L_BK_IPMUX(0x02);
        u32CLK_reg    = REG_CKG_SC1_IDCLK2;
        u32PreCLK_reg = REG_CKG_SC1_PRE_IDCLK2;

        u8CLK_msk    = CKG_SC1_IDCLK2_MASK;
        u8PreCLK_msk = CKG_SC1_PRE_IDCLK2_MASK;

        u8Data_val = u8Data_Mux << 4;
        u8Data_msk = 0xF0;
        break;

    case HAL_OUTPUTPORT_SCALER2_MAIN_WINDOW:
        u32Data_reg   = L_BK_IPMUX(0x03);
        u32CLK_reg    = REG_CKG_SC2_IDCLK2;
        u32PreCLK_reg = REG_CKG_SC2_PRE_IDCLK2;

        u8CLK_msk    = CKG_SC2_IDCLK2_MASK;
        u8PreCLK_msk = CKG_SC2_PRE_IDCLK2_MASK;

        u8Data_val = u8Data_Mux << 4;
        u8Data_msk = 0xF0;
        break;

    case HAL_OUTPUTPORT_SCALER2_SUB_WINDOW1:
        u32Data_reg   = L_BK_IPMUX(0x03);
        u32CLK_reg    = REG_CKG_SC2_IDCLK1;
        u32PreCLK_reg = REG_CKG_SC2_PRE_IDCLK1;

        u8CLK_msk    = CKG_SC2_IDCLK1_MASK;
        u8PreCLK_msk = CKG_SC2_PRE_IDCLK1_MASK;

        u8Data_val = u8Data_Mux;
        u8Data_msk = 0x0F;
        break;

    default:
        u32Data_reg   = L_BK_IPMUX(0x03);
        u32CLK_reg    = 0xFFFFFFFF;
        u32PreCLK_reg = 0xFFFFFFFF;

        u8CLK_msk    = 0xFF;
        u8PreCLK_msk = 0xFF;

        u8Data_val = 0xFF;
        u8Data_msk = 0x0F;

        bOut = FALSE;
        break;
    }

    switch(u8Clk_Mux)
    {
    case SC_MAINWIN_IPMUX_ADC_A:

        if(eDest == HAL_OUTPUTPORT_SCALER_SUB_WINDOW1)
        {
            u8CLK_val    = CKG_IDCLK1_PRE_MUX_OUT;
            u8PreCLK_val = CKG_PRE_IDCLK1_ADC;

        }
        else if(eDest == HAL_OUTPUTPORT_SCALER1_MAIN_WINDOW)
        {
            u8CLK_val    = CKG_SC1_IDCLK2_PRE_MUX_OUT;
            u8PreCLK_val = CKG_SC1_PRE_IDCLK2_ADC;

        }
        else if(eDest == HAL_OUTPUTPORT_SCALER2_MAIN_WINDOW)
        {
            u8CLK_val    = CKG_SC2_IDCLK2_PRE_MUX_OUT;
            u8PreCLK_val = CKG_SC2_PRE_IDCLK2_ADC;
        }
        else if(eDest == HAL_OUTPUTPORT_SCALER2_SUB_WINDOW1)
        {
            u8CLK_val    = CKG_SC2_IDCLK1_PRE_MUX_OUT;
            u8PreCLK_val = CKG_SC2_PRE_IDCLK1_ADC;
        }
        else
        {
            u8CLK_val    = CKG_IDCLK2_PRE_MUX_OUT;
            u8PreCLK_val = CKG_PRE_IDCLK2_ADC;
        }
        break;

    case SC_MAINWIN_IPMUX_VD:
        u8CLK_val    = CKG_IDCLK2_CLK_VD;
        u8PreCLK_val = 0xFF;
        break;

    case SC_MAINWIN_IPMUX_MVOP:
        u8CLK_val    = CKG_IDCLK2_CLK_DC0;
        u8PreCLK_val = 0xFF;
        break;

    case SC_MAINWIN_IPMUX_EXT_VD:

        if(eDest == HAL_OUTPUTPORT_SCALER_SUB_WINDOW1)
        {
			u8CLK_val    = CKG_IDCLK1_PRE_MUX_OUT;
			u8PreCLK_val = CKG_PRE_IDCLK1_EXTDI;

        }
        else if(eDest == HAL_OUTPUTPORT_SCALER1_MAIN_WINDOW)
        {
			u8CLK_val    = CKG_SC1_IDCLK2_PRE_MUX_OUT;
			u8PreCLK_val = CKG_SC1_PRE_IDCLK2_EXTDI;

        }
        else if(eDest == HAL_OUTPUTPORT_SCALER2_MAIN_WINDOW)
        {
			u8CLK_val    = CKG_SC2_IDCLK2_PRE_MUX_OUT;
			u8PreCLK_val = CKG_SC2_PRE_IDCLK2_EXTDI;
        }
        else if(eDest == HAL_OUTPUTPORT_SCALER2_SUB_WINDOW1)
        {
			u8CLK_val    = CKG_SC2_IDCLK1_PRE_MUX_OUT;
			u8PreCLK_val = CKG_SC2_PRE_IDCLK1_EXTDI;
        }
        else
        {
			u8CLK_val    = CKG_IDCLK2_PRE_MUX_OUT;
			u8PreCLK_val = CKG_PRE_IDCLK2_EXTDI;
        }
        break;

    case SC_MAINWIN_IPMUX_CAPTURE_SC0:
        u8CLK_val    = CKG_IDCLK2_ODCLK;
        u8PreCLK_val = 0xFF;
        break;

    case SC_MAINWIN_IPMUX_CAPTURE_SC1:
        u8CLK_val    = CKG_IDCLK2_SC1_ODCLK;
        u8PreCLK_val = 0xFF;
        break;

    case SC_MAINWIN_IPMUX_CAPTURE_SC2:
        u8CLK_val    = CKG_IDCLK2_SC2_ODCLK;
        u8PreCLK_val = 0xFF;
        break;

    case SC_MAINWIN_IPMUX_SENSOR_ICP:
        u8CLK_val    = CKG_IDCLK2_CLK_ICP;
        u8PreCLK_val = 0xFF;
        break;

    case SC_MAINWIN_IPMUX_EXT_VD2:

        if(eDest == HAL_OUTPUTPORT_SCALER_SUB_WINDOW1)
        {
			u8CLK_val    = CKG_IDCLK1_PRE_MUX_OUT;
			u8PreCLK_val = CKG_PRE_IDCLK1_EXTDI2;

        }
        else if(eDest == HAL_OUTPUTPORT_SCALER1_MAIN_WINDOW)
        {
			u8CLK_val    = CKG_SC1_IDCLK2_PRE_MUX_OUT;
			u8PreCLK_val = CKG_SC1_PRE_IDCLK2_EXTDI2;

        }
        else if(eDest == HAL_OUTPUTPORT_SCALER2_MAIN_WINDOW)
        {
			u8CLK_val    = CKG_SC2_IDCLK2_PRE_MUX_OUT;
			u8PreCLK_val = CKG_SC2_PRE_IDCLK2_EXTDI2;
        }
        else if(eDest == HAL_OUTPUTPORT_SCALER2_SUB_WINDOW1)
        {
			u8CLK_val    = CKG_SC2_IDCLK1_PRE_MUX_OUT;
			u8PreCLK_val = CKG_SC2_PRE_IDCLK1_EXTDI2;
        }
        else
        {
			u8CLK_val    = CKG_IDCLK2_PRE_MUX_OUT;
			u8PreCLK_val = CKG_PRE_IDCLK2_EXTDI2;
        }
        break;


    default:
    case SC_MAINWIN_IPMUX_ADC_B:
    case SC_MAINWIN_IPMUX_SC_VOP:
    case SC_MAINWIN_IPMUX_HDMI_DVI:
        u8CLK_val    = CKG_IDCLK2_RESERVED;
        u8PreCLK_val = 0xFF;
        break;
    }

    if(bOut)
    {
        MDrv_WriteByteMask(u32Data_reg, u8Data_val, u8Data_msk);
        MDrv_WriteByteMask(u32CLK_reg, u8CLK_val, u8CLK_msk);

        if(u8PreCLK_val != 0xFF)
        {
            MDrv_WriteByteMask(u32PreCLK_reg, u8PreCLK_val, u8PreCLK_msk);
        }
    }
}

MS_BOOL Hal_SC_get_ip_mux( MS_U8 *pU8Data_Mux, MS_U8 *pU8Clk_Mux, E_OUTPUT_PORT_TYPE eDest)
{
    MS_BOOL bRet = FALSE;

    if(pU8Data_Mux && pU8Clk_Mux)
    {

        switch(eDest)
        {
        case HAL_OUTPUTPORT_SCALER_MAIN_WINDOW:
            *pU8Data_Mux = (MDrv_ReadRegBit(L_BK_IPMUX(0x01), 0xF0)) >> 4;
            *pU8Clk_Mux = (MDrv_ReadRegBit(REG_CKG_IDCLK2, CKG_IDCLK2_MASK)) >> 2;
             bRet = TRUE;
             break;

        case HAL_OUTPUTPORT_SCALER_SUB_WINDOW1:
            *pU8Data_Mux = (MDrv_ReadRegBit(L_BK_IPMUX(0x01), 0x0F));
            *pU8Clk_Mux = (MDrv_ReadRegBit(REG_CKG_IDCLK1, CKG_IDCLK1_MASK)) >> 2;
             bRet = TRUE;
             break;

        case HAL_OUTPUTPORT_SCALER1_MAIN_WINDOW:
            *pU8Data_Mux = (MDrv_ReadRegBit(L_BK_IPMUX(0x02), 0xF0)) >> 4;
            *pU8Clk_Mux = (MDrv_ReadRegBit(REG_CKG_SC1_IDCLK2, CKG_SC1_IDCLK2_MASK)) >> 2;
             bRet = TRUE;
             break;

        case HAL_OUTPUTPORT_SCALER2_MAIN_WINDOW:
            *pU8Data_Mux = (MDrv_ReadRegBit(L_BK_IPMUX(0x03), 0xF0)) >> 4;
            *pU8Clk_Mux = (MDrv_ReadRegBit(REG_CKG_SC2_IDCLK2, CKG_SC2_IDCLK2_MASK)) >> 2;
             bRet = TRUE;
             break;

        case HAL_OUTPUTPORT_SCALER2_SUB_WINDOW1:
            *pU8Data_Mux = (MDrv_ReadRegBit(L_BK_IPMUX(0x03), 0x0F));
            *pU8Clk_Mux = (MDrv_ReadRegBit(REG_CKG_SC2_IDCLK1, CKG_SC2_IDCLK1_MASK)) >> 2;
             bRet = TRUE;
             break;

        default:
            bRet = FALSE;
            break;
        }
    }
    else
    {
        bRet = FALSE;
    }
    return bRet;
}

// Used for backward compatibility
void Hal_SC_set_sync_port_by_dataport(E_MUX_INPUTPORT src_port )
{
    E_MUX_INPUTPORT sync_port;
    switch ( src_port)
    {
        case INPUT_PORT_ANALOG0:
            sync_port = INPUT_PORT_ANALOG0_SYNC;
            break;

        case INPUT_PORT_ANALOG1:
            sync_port = INPUT_PORT_ANALOG1_SYNC;
            break;

        case INPUT_PORT_ANALOG2:
            sync_port = INPUT_PORT_ANALOG2_SYNC;
            break;
        default:
            sync_port = INPUT_PORT_ANALOG0_SYNC;
    }
        // Src -> ( IPmux )
        Hal_ADC_set_mux(sync_port);
}

/******************************************************************************/
///Dispatch Port
///@param src \b IN: source type
///@param dest \b IN: pixel clock
/******************************************************************************/
void hal_ccirin_mode(void)
{
	W2BYTE(REG_HDMI2_00_L, 0x814);
}
void Hal_SC_mux_dispatch(E_MUX_INPUTPORT src_port , E_MUX_OUTPUTPORT dest_port)
{
    MS_U8 _src = (MS_U8) src_port;
    MS_U8 _dest = (MS_U8) dest_port;

    // Hal level in T2 is following MsCommon.h directly.
    // It is unnecessary to add a mapping function.
    E_INPUT_PORT_TYPE src = (E_INPUT_PORT_TYPE) _src;
    E_OUTPUT_PORT_TYPE dest = (E_OUTPUT_PORT_TYPE) _dest;

    // If src is sync port. set adc only.
    if ( src >= HAL_INPUTPORT_ANALOG_SYNC0 && src < (HAL_INPUTPORT_ANALOG_SYNC0 + NUMBER_OF_ANALOG_PORT ) )
    {
        Hal_ADC_set_mux(src_port);
    }

    // Src = Analog 0ort 0 ~ Max number of analog port
    else if ( src >= HAL_INPUTPORT_ANALOG0  && src < (HAL_INPUTPORT_ANALOG0 + NUMBER_OF_ANALOG_PORT ))
    {

        // Src -> ( IPmux )
        Hal_ADC_set_mux(src_port);

        // IPmux -> Dest
        if ( dest == HAL_OUTPUTPORT_SCALER_MAIN_WINDOW ) //SC0 Main
        {
            Hal_SC_mux_set_mainwin_ip_mux(SC_MAINWIN_IPMUX_ADC_A, SC_MAINWIN_IPMUX_ADC_A);
        }
        else if (dest == HAL_OUTPUTPORT_SCALER_SUB_WINDOW1 ) // SC0 Sub
        {
            Hal_SC_set_subwin_ip_mux(SC_SUBWIN_IPMUX_ADC_A,SC_SUBWIN_IPMUX_ADC_A);
        }
        else
        {
            Hal_SC_set_ip_mux(SC_MAINWIN_IPMUX_ADC_A, SC_MAINWIN_IPMUX_ADC_A, dest);
        }

    }

    // Src = Y mux port 0 ~ Max number of Y mux port
    else if (  src >= HAL_INPUTPORT_YMUX_CVBS0 && src < (HAL_INPUTPORT_YMUX_CVBS0 + NUMBER_OF_CVBS_PORT) )
    {
        // IPmux -> Dest
        if ( dest == HAL_OUTPUTPORT_SCALER_MAIN_WINDOW )
        {
            Hal_SC_mux_set_mainwin_ip_mux(SC_MAINWIN_IPMUX_VD, SC_MAINWIN_IPMUX_VD);
            // Src -> ( IPmux )
            Hal_ADC_set_mux(src_port);
        }
        else if (dest == HAL_OUTPUTPORT_SCALER_SUB_WINDOW1 )
        {
            Hal_SC_set_subwin_ip_mux(SC_SUBWIN_IPMUX_VD , SC_SUBWIN_IPMUX_VD);
            // Src -> ( IPmux )
            Hal_ADC_set_mux(src_port);
        }
        else if ( dest == HAL_OUTPUTPORT_CVBS1 )
        {
            _Hal_SC_mux_set_cvbsout1_y_mux(src);
        }
        else if ( dest == HAL_OUTPUTPORT_CVBS2 )
        {
            _Hal_SC_mux_set_cvbsout2_y_mux(src);
        }
        else
        {
            Hal_SC_set_ip_mux(SC_MAINWIN_IPMUX_VD, SC_MAINWIN_IPMUX_VD, dest);
            Hal_ADC_set_mux(src_port);
        }
    }

    // Src = C mux port 0 ~ Max number of C mux port
    else if ( src >= HAL_INPUTPORT_CMUX_CVBS0 && src < (HAL_INPUTPORT_CMUX_CVBS0 + NUMBER_OF_CVBS_PORT) )
    {
        // IPmux -> Dest
        if ( dest == HAL_OUTPUTPORT_SCALER_MAIN_WINDOW )
        {
            // Src -> ( IPmux )
            Hal_ADC_set_mux(src_port);
            ; // Controlled by Ymux
        }
        else if (dest == HAL_OUTPUTPORT_SCALER_SUB_WINDOW1 )
        {
            // Src -> ( IPmux )
            Hal_ADC_set_mux(src_port);
            ; // Controlled by Ymux
        }
        else if ( dest == HAL_OUTPUTPORT_CVBS1 )
        {
            _Hal_SC_mux_set_cvbsout1_c_mux(src);
        }
        else if ( dest == HAL_OUTPUTPORT_CVBS2 )
        {
            _Hal_SC_mux_set_cvbsout2_c_mux(src);
        }
        else
        {
            // Src -> ( IPmux )
            Hal_ADC_set_mux(src_port);
            //Undefined
        }

    }

    // Src = DVI port 0 ~ Max number of DVI port
    else if ( src >= HAL_INPUTPORT_DVI0 && src < (HAL_INPUTPORT_DVI0 + NUMBER_OF_DVI_PORT) )
    {
        // Src -> ( IPmux )
        Hal_SC_mux_set_dvi_mux(src - HAL_INPUTPORT_DVI0); //alex_tung

        // IPmux -> Dest
        if ( dest == HAL_OUTPUTPORT_SCALER_MAIN_WINDOW )
        {
            Hal_SC_mux_set_mainwin_ip_mux(SC_MAINWIN_IPMUX_HDMI_DVI, SC_MAINWIN_IPMUX_HDMI_DVI);
        }
        else if (dest == HAL_OUTPUTPORT_SCALER_SUB_WINDOW1 )
        {
            Hal_SC_set_subwin_ip_mux(SC_SUBWIN_IPMUX_HDMI_DVI , SC_SUBWIN_IPMUX_HDMI_DVI);
        }
        else
        {
            Hal_SC_set_ip_mux(SC_MAINWIN_IPMUX_HDMI_DVI, SC_MAINWIN_IPMUX_HDMI_DVI, dest);
        }
    }

    else if ( src >= HAL_INPUTPORT_MVOP && src < (HAL_INPUTPORT_MVOP + NUMBER_OF_MVOP_PORT) )
    {
        // Src -> ( IPmux )
        // No source mux need to set in T2 (only one MVD).

        // IPmux -> Dest
        if ( dest == HAL_OUTPUTPORT_SCALER_MAIN_WINDOW )
        {
            Hal_SC_mux_set_mainwin_ip_mux(SC_MAINWIN_IPMUX_MVOP, SC_MAINWIN_IPMUX_MVOP);
        }
        else if (dest == HAL_OUTPUTPORT_SCALER_SUB_WINDOW1 )
        {
            Hal_SC_set_subwin_ip_mux(SC_SUBWIN_IPMUX_MVOP , SC_SUBWIN_IPMUX_MVOP);
        }
        else
        {
             Hal_SC_set_ip_mux(SC_MAINWIN_IPMUX_MVOP, SC_MAINWIN_IPMUX_MVOP, dest);
            // Undefeined ( set in ADC table )
        }

    }
    else if ( src >= HAL_INPUTPORT_ICP && src < (HAL_INPUTPORT_ICP + NUMBER_OF_ICP_PORT) )
    {
         Hal_SC_set_ip_mux(SC_MAINWIN_IPMUX_SENSOR_ICP, SC_MAINWIN_IPMUX_SENSOR_ICP, dest);
    }

    else if(src >= HAL_INPUTPORT_BT656_0 && src < (HAL_INPUTPORT_BT656_0 + NUMBER_OF_BT656_PORT) )
    {
        if(src == HAL_INPUTPORT_BT656_0)
        {
			hal_ccirin_mode();
            Hal_SC_set_ip_mux(SC_MAINWIN_IPMUX_EXT_VD, SC_MAINWIN_IPMUX_EXT_VD, dest);
        }
        else
        {
    		hal_ccirin_mode();
            Hal_SC_set_ip_mux(SC_MAINWIN_IPMUX_EXT_VD2, SC_MAINWIN_IPMUX_EXT_VD2, dest);
        }
    }
    else if(src >= HAL_INPUTPORT_SC0_VOP && src < (HAL_INPUTPORT_SC0_VOP + NUMBER_OF_SC_VOP_PORT) )
    {
        if(src == HAL_INPUTPORT_SC0_VOP)
        {
            Hal_SC_set_ip_mux(SC_MAINWIN_IPMUX_CAPTURE_SC0, SC_MAINWIN_IPMUX_CAPTURE_SC0, dest);
            Hal_SC_Set_Capture_Enable(MAIN_WINDOW, TRUE);
            Hal_SC_Set_Cap_Stage(MAIN_WINDOW, 0x10);
        }
        else if(src == HAL_INPUTPORT_SC1_VOP)
        {
            Hal_SC_set_ip_mux(SC_MAINWIN_IPMUX_CAPTURE_SC1, SC_MAINWIN_IPMUX_CAPTURE_SC1, dest);
            Hal_SC_Set_Capture_Enable(SC1_MAIN_WINDOW, TRUE);
            Hal_SC_Set_Cap_Stage(SC1_MAIN_WINDOW, 0x00);
        }
        else
        {
            Hal_SC_set_ip_mux(SC_MAINWIN_IPMUX_CAPTURE_SC2, SC_MAINWIN_IPMUX_CAPTURE_SC2, dest);
            Hal_SC_Set_Capture_Enable(SC2_MAIN_WINDOW, TRUE);
            Hal_SC_Set_Cap_Stage(SC2_MAIN_WINDOW, 0x00);
        }
    }
    else
    {
        if ( dest == HAL_OUTPUTPORT_SCALER_MAIN_WINDOW )
        {
            Hal_SC_mux_set_mainwin_ip_mux(SC_MAINWIN_IPMUX_NONE, SC_MAINWIN_IPMUX_NONE);
        }
        else if (dest == HAL_OUTPUTPORT_SCALER_SUB_WINDOW1 )
        {
            Hal_SC_set_subwin_ip_mux(SC_MAINWIN_IPMUX_NONE , SC_MAINWIN_IPMUX_NONE);
        }
        else
        {
            Hal_SC_set_ip_mux(SC_MAINWIN_IPMUX_NONE, SC_MAINWIN_IPMUX_NONE, dest);
        }
    }
}

void Hal_BT656test_bus( void )
{
#if 0
	MS_U16 check= R2BYTE(REG_HDMI2_03_L);

	printf("[BT565] set test_bus \r\n");
	W2BYTE(REG_CHIPTOP_75, 0x4000);
	W2BYTE(REG_CHIPTOP_77, 0x0025);
	W2BYTE(REG_CHIPTOP_75, 0x4000);
	W2BYTE(REG_CHIPTOP_76, 0x0000);
	W2BYTE(REG_CHIPTOP_75, 0x4000);
		{
			printf("[BT565] orig check %2x \r\n",check);
			}
	W2BYTE(REG_HDMI2_03_L, 0x027C);
		{
			check= R2BYTE(REG_HDMI2_03_L);
			printf("[BT565] after  check %2x \r\n",check);
			}
	printf("[BT565] set test_bus done \r\n");

#endif

}


#undef MHAL_MUX_C
