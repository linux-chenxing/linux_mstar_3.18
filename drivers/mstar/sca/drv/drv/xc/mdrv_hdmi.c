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
/// file    Mdrv_hdmi.c
/// @brief  Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
#define  MDRV_HDMI_C
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/wait.h>
#else
#include <string.h>
#endif

#include "MsCommon.h"
#include "MsVersion.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"

// Internal Definition
#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "mvideo_context.h"
#include "drvXC_HDMI_Internal.h"
#include "mhal_hdmi.h"
#include "apiXC_Adc.h"
#include "drvXC_ADC_Internal.h"
#include "mhal_adc.h"
#include "xc_hwreg_utility2.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------
#define msg_hdmi(x) //x
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
/// Debug information
static MSIF_Version _drv_hdmi_version =
{
#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL))
	{ HDMI_DRV_VERSION },
#else
	.DDI = { HDMI_DRV_VERSION },
#endif
};
static MS_HDMI_INFO _info = {0};
static MS_HDMI_Status _status =
{
#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) )
	FALSE, FALSE,
#else
	.bIsInitialized = FALSE, .bIsRunning = FALSE,
#endif
};

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static void MDrv_HDMI_WriteEDID(E_XC_DDCRAM_PROG_TYPE eDDCRamType, MS_U8 *u8EDID, MS_U16 u16EDIDSize);
//-------------------------------------------------------------------------------------------------
/// report which patch function hdmi needs
/// @return @ref MS_U16
//-------------------------------------------------------------------------------------------------
MS_U16 MDrv_HDMI_Func_Caps(void)
{
    return Hal_HDMI_Func_Caps();
}
//-------------------------------------------------------------------------------------------------
/// Check the HDCP is exist or not
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_HDCP_isexist( void )
{
    MS_HDCP_STATUS_INFO_t* enStatus_info;
    enStatus_info = MDrv_HDCP_GetStatus();

    if(enStatus_info->b_St_Reserve_1 && enStatus_info->b_St_ENC_En)
    {
        msg_hdmi(printf("HDCP is exit"));
        return TRUE;
    }
    else
    {
        msg_hdmi(printf("No HDCP"));
        return FALSE;
    }
}
//-------------------------------------------------------------------------------------------------
/// DDC clock on/off control
/// @param  bEnable      \b IN: ON/OFF control
//-------------------------------------------------------------------------------------------------
void MDrv_HDCP_Enable(MS_BOOL bEnable)
{
#if defined(CHIP_A2) || defined(CHIP_T12) || defined(CHIP_T8) || defined(CHIP_T9) || defined(CHIP_J2) || defined(CHIP_T11) || defined(CHIP_MARIA10) || defined(CHIP_T13) || defined(CHIP_A1) || defined(CHIP_A6) || defined(CHIP_A7) || defined(CHIP_A5) || defined(CHIP_A3) || defined(CHIP_AMETHYST) || defined(CHIP_EAGLE) || defined(CHIP_AGATE)|| defined(CHIP_C3)
    Hal_HDCP_ddc_en(MApi_XC_Mux_GetHDMIPort(gSrcInfo[0].enInputSourceType), bEnable);
#else
    Hal_HDCP_ddc_en(bEnable);
#endif
}

//-------------------------------------------------------------------------------------------------
/// Check the ending of the Vsync
/// @param  bStable      \b IN: Stable or not
//-------------------------------------------------------------------------------------------------
void MDrv_HDCP_Vsync_end_en(MS_BOOL bStable)
{
    static MS_BOOL _bHDCP_Vsync_done = FALSE;
    static MS_BOOL _bHDCP_Vsync_count = FALSE;
    static MS_U32 u32VsyncTimer;

    if(bStable)
    {
        if(!_bHDCP_Vsync_count)
        {
            u32VsyncTimer = MsOS_GetSystemTime();
            _bHDCP_Vsync_count = TRUE;
        }
        if ((_bHDCP_Vsync_done) && ((MsOS_GetSystemTime() - u32VsyncTimer) > 4500) )
        {
            Hal_HDCP_Vsync_end_en(TRUE);
            _bHDCP_Vsync_done = FALSE;
        }
    }
    else
    {
        if (!_bHDCP_Vsync_done)
        {
            Hal_HDCP_Vsync_end_en(FALSE);
            _bHDCP_Vsync_done = TRUE;
            _bHDCP_Vsync_count = FALSE;
        }
    }
}

//-------------------------------------------------------------------------------------------------
/// Exit the HDMI and turn off the related setting of HDMI
//-------------------------------------------------------------------------------------------------
void MDrv_HDMI_Exit( void )
{
#if defined(CHIP_A2) || defined(CHIP_T12) || defined(CHIP_T8) || defined(CHIP_T9) || defined(CHIP_J2) || defined(CHIP_T11) || defined(CHIP_MARIA10) || defined(CHIP_T13) || defined(CHIP_A1) || defined(CHIP_A6) || defined(CHIP_A7) || defined(CHIP_A5) || defined(CHIP_A3) || defined(CHIP_AMETHYST) || defined(CHIP_EAGLE) || defined(CHIP_AGATE) || defined(CHIP_C3)
    Hal_HDMI_exit(MApi_XC_Mux_GetHDMIPort(gSrcInfo[0].enInputSourceType));
#else
    Hal_HDMI_exit();
#endif
#if 0
    if(g_bIMMESWITCH_DVI_POWERSAVING)
        memset(u8PSDVIStableCount, 0, 4*sizeof(MS_U8));
#endif
}

//-------------------------------------------------------------------------------------------------
/// Init the setting for HDMI
//-------------------------------------------------------------------------------------------------
void MDrv_HDMI_init( void )
{
    _status.bIsInitialized = TRUE;
    _status.bIsRunning     = TRUE;

    if (SUPPORT_IMMESWITCH)
    {
        g_bIsIMMESWITCH = (MS_BOOL)((g_XC_Init_Misc.u32MISC_A & E_XC_INIT_MISC_A_IMMESWITCH));
        g_bIMMESWITCH_DVI_POWERSAVING = (MS_BOOL)((g_XC_Init_Misc.u32MISC_A & E_XC_INIT_MISC_A_IMMESWITCH_DVI_POWERSAVING) >> 1);
    }
    else
    {
        g_bIsIMMESWITCH = FALSE;
        g_bIMMESWITCH_DVI_POWERSAVING = FALSE;
    }
    if (SUPPORT_DVI_AUTO_EQ)
    {
        g_bDVI_AUTO_EQ = (MS_BOOL)((g_XC_Init_Misc.u32MISC_A & E_XC_INIT_MISC_A_DVI_AUTO_EQ) >> 2);
    }
    else
    {
        g_bDVI_AUTO_EQ = FALSE;
    }

    Hal_HDMI_init();

#if 0
    if(g_bIMMESWITCH_DVI_POWERSAVING)
        memset(u8PSDVIStableCount, 0, 4*sizeof(MS_U8));
#endif
}

//-------------------------------------------------------------------------------------------------
/// Set the equalizer volumn for HDMI
/// @param  enEq      \b IN: ON/OFF the equlizer
/// @param  u8EQValue      \b IN: The volumn of equalizer
//-------------------------------------------------------------------------------------------------
void MDrv_HDMI_Set_EQ(MS_HDMI_EQ enEq, MS_U8 u8EQValue)
{
#if defined(CHIP_A2) || defined(CHIP_T12) || defined(CHIP_T8) || defined(CHIP_T9) || defined(CHIP_J2) || defined(CHIP_T11) || defined(CHIP_MARIA10) || defined(CHIP_T13) || defined(CHIP_A1) || defined(CHIP_A6) || defined(CHIP_A7) || defined(CHIP_A5) || defined(CHIP_A3) || defined(CHIP_AMETHYST) || defined(CHIP_EAGLE) || defined(CHIP_AGATE) || defined(CHIP_C3)
    INPUT_SOURCE_TYPE_t src;

    for(src = INPUT_SOURCE_HDMI; src < INPUT_SOURCE_HDMI_MAX; src++)
    {
        Hal_HDMI_Set_EQ(MApi_XC_Mux_GetHDMIPort(src), enEq, u8EQValue);
    }

#else
    Hal_HDMI_Set_EQ(enEq, u8EQValue);
#endif
}

//-------------------------------------------------------------------------------------------------
/// Control the Hot Plug Detection
/// in HIGH Level, the voltage level is 2.4 ~ 5.3V
/// in LOW Level,  the voltage level is 0 ~ 0.4V
/// Enable hotplug GPIO[0] output and set output value
/// @param  bHighLow                \b IN: High/Low control
/// @param  enInputPortType      \b IN: Input source selection
/// @param  bInverse                 \b IN: Inverse or not
//-------------------------------------------------------------------------------------------------
void MDrv_HDMI_pullhpd( MS_BOOL bHighLow, E_MUX_INPUTPORT enInputPortType, MS_BOOL bInverse)
{
    msg_hdmi(printf("src hpd = %x \n",enInputPortType));
    // Fix me. Do not use HDMI_PORT_ALL and HDMI_PORT_A.
    // use E_MUX_INPUTPORT.

    Hal_HDMI_pullhpd(bHighLow, enInputPortType, bInverse);
}

//-------------------------------------------------------------------------------------------------
/// HDMI G control information
/// @param  gcontrol                \b IN: G control setting
//-------------------------------------------------------------------------------------------------
MS_U16 MDrv_HDMI_GC_Info(HDMI_GControl_INFO_t gcontrol)
{
    return (Hal_HDMI_gcontrol_info(gcontrol));
}

//-------------------------------------------------------------------------------------------------
/// HDMI PLL control setting 1
/// @param  pllctrl                \b IN: PLL control
/// @param  bread                \b IN: Read/Write (1/0)
/// @param  u16value           \b IN: Write value
/// @return @ref MS_U16
//-------------------------------------------------------------------------------------------------
MS_U16 MDrv_HDMI_pll_ctrl1(HDMI_PLL_CTRL_t pllctrl, MS_BOOL bread, MS_U16 u16value)
{
    return (Hal_HDMI_pll_ctrl1(pllctrl, bread, u16value));
}
//-------------------------------------------------------------------------------------------------
/// HDMI PLL control setting 2
/// @param  pllctrl                \b IN: PLL control
/// @param  bread                \b IN: Read/Write (1/0)
/// @param  u16value           \b IN: Write value
/// @return @ref MS_U16
//-------------------------------------------------------------------------------------------------
MS_U16 MDrv_HDMI_pll_ctrl2(HDMI_PLL_CTRL2_t pllctrl, MS_BOOL bread, MS_U16 u16value)
{
    return (Hal_HDMI_pll_ctrl2(pllctrl, bread, u16value));
}

////////////////////////


/******************************************************************************/
/// for DVI
/******************************************************************************/
//-------------------------------------------------------------------------------------------------
/// Check the clock is less 70MHz or not with DVI source
/// @param  enInputPortType                \b IN: Input source
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_HDMI_SetForClockLessThan70Mhz( E_MUX_INPUTPORT enInputPortType )
{
    switch(enInputPortType)
    {
        default:
        case INPUT_PORT_DVI0:
        case INPUT_PORT_DVI1:
        case INPUT_PORT_DVI3:
            return Hal_DVI_clk_lessthan70mhz_usingportc(FALSE);
        case INPUT_PORT_DVI2:
            return Hal_DVI_clk_lessthan70mhz_usingportc(TRUE);
    }
}

void MDrv_DVI_SoftwareReset( MS_U16 u16Reset )
{
    Hal_DVI_sw_reset(u16Reset);

}

//-------------------------------------------------------------------------------------------------
/// Detect the the clock is lose or not from ADC DTOP
/// @param  enInputPortType                \b IN: Input source
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_HDMI_dvi_clock_detect(E_MUX_INPUTPORT enInputPortType)
{
    MS_BOOL bIsLoss;

#if defined(CHIP_T12) || defined(CHIP_T8) || defined(CHIP_T9) || defined(CHIP_J2) || defined(CHIP_T11) || defined(CHIP_MARIA10) || defined(CHIP_T13) || defined(CHIP_A1) || defined(CHIP_A2) || defined(CHIP_A6) || defined(CHIP_A7) || defined(CHIP_A5) || defined(CHIP_A3) || defined(CHIP_AMETHYST) || defined(CHIP_EAGLE) || defined(CHIP_C3)
    bIsLoss = Hal_DVI_clklose_det(enInputPortType);
#else
    if (enInputPortType == INPUT_PORT_DVI2)
    {
        bIsLoss = Hal_DVI_clklose_det(TRUE);
    }
    else
    {
        bIsLoss = Hal_DVI_clklose_det(FALSE);
    }
#endif

    return bIsLoss;
}

static MS_U8 _ResetDone;

//-------------------------------------------------------------------------------------------------
/// Reset the DVI setting
//-------------------------------------------------------------------------------------------------
void mdrv_dvi_reset(void)
{
#if defined(CHIP_A2) || defined(CHIP_T12) || defined(CHIP_T8) || defined(CHIP_T9) || defined(CHIP_J2) || defined(CHIP_T11) || defined(CHIP_MARIA10) || defined(CHIP_T13) || defined(CHIP_A1) || defined(CHIP_A6) || defined(CHIP_A7) || defined(CHIP_A5) || defined(CHIP_A3) || defined(CHIP_AMETHYST) || defined(CHIP_EAGLE) || defined(CHIP_AGATE)|| defined(CHIP_C3)
    if(Hal_DVI_irq_info(MApi_XC_Mux_GetHDMIPort(gSrcInfo[0].enInputSourceType), IRQ_DVI_CK_CHG) && (!_ResetDone))
    {
        MS_U16 u8_IRQ_STATUS;

        u8_IRQ_STATUS = Hal_DVI_irq_info(MApi_XC_Mux_GetHDMIPort(gSrcInfo[0].enInputSourceType), IRQ_DVI_CK_CHG);
        Hal_DVI_irq_clear(MApi_XC_Mux_GetHDMIPort(gSrcInfo[0].enInputSourceType), u8_IRQ_STATUS);
        Hal_DVI_irq_clear(MApi_XC_Mux_GetHDMIPort(gSrcInfo[0].enInputSourceType), ((MS_U16)~IRQ_ALL_BIT));

        if (u8_IRQ_STATUS == IRQ_DVI_CK_CHG)
        {
            // not do DVI software reset at here; Avoid the snow image

            _ResetDone = 1;
        }
    }
#else
    if(Hal_DVI_irq_info(IRQ_DVI_CK_CHG) && (!_ResetDone))
    {
        MS_U16 u8_IRQ_STATUS;

        u8_IRQ_STATUS = Hal_DVI_irq_info(IRQ_DVI_CK_CHG);
        Hal_DVI_irq_clear(u8_IRQ_STATUS);
        Hal_DVI_irq_clear(~IRQ_ALL_BIT);

        if (u8_IRQ_STATUS == IRQ_DVI_CK_CHG)
        {
            // not do DVI software reset at here; Avoid the snow image

            _ResetDone = 1;
        }
    }
#endif
}
#ifdef DVI_NEW_Patch
// 20081226 - DVI+HDCP snow noise patch - start ...
//-------------------------------------------------------------------------------------------------
/// avoid the snow image while AC on/off and source change for DVI 70Mhz issue, setting 1
/// @param  enInputPortType                \b IN: Input source
//-------------------------------------------------------------------------------------------------
void mdrv_dvi_clock_70mhz_swpatch1(INPUT_SOURCE_TYPE_t enInputSourceType)
{
    MS_BOOL bClockLess70MHz;
    E_MUX_INPUTPORT enMuxPort =MApi_XC_Mux_GetHDMIPort(enInputSourceType);

//    if(MDrv_ReadByte( H( REG_HDMI_INT_STATUS) ) & BIT5)
    if(R2BYTEMSK(REG_HDMI_61_L, BIT13))
    {
        #if ( WATCH_DOG == ENABLE )
            MDrv_Sys_ClearWatchDog();
        #endif

        switch(enMuxPort)
        {
        default:
        case INPUT_PORT_DVI0:
        case INPUT_PORT_DVI1:
        case INPUT_PORT_DVI3:
//            if(!(MDrv_ReadByte(H_BK_ADC_DTOP(0x57)) & BIT3)) // DVI clock is stable
            if(!Hal_DVI_clk_stable_usingportc(NO)) // DVI clock is stable
            {
                //printf("\r\nAaron - HDCP status = %bx\r\n", MDrv_ReadByte(L_BK_ADC_DTOP(0x79)));
//                if(!(R2BYTEMSK(0x2C33, BIT5))) // Video clock is unstable
                if(!(R2BYTEMSK(0x2C32, BIT13))) // Video clock is unstable
                {
                    g_DVI70MhzUnstableCounter++;
                    if(g_DVI70MhzUnstableCounter >= 2)
                    {
                        g_DVI70MhzUnstableCounter = 0;
                        //bClockLess70MHz = (MDrv_ReadByte(H_BK_ADC_DTOP(0x57)) & BIT2) > 0 ? 1 : 0;
                        bClockLess70MHz = Hal_DVI_clk_lessthan70mhz_usingportc(NO); //Not Port C
                        //printf("\r\nAaron - CLK_CNT = %bx, AF= %bx\r\n", MDrv_ReadByte(L_BK_ADC_DTOP(0x57)), MDrv_ReadByte(H_BK_ADC_DTOP(0x57)));
                        //printf("\r\nAaron - VCO= %bx\r\n", MDrv_ReadByte(L_BK_ADC_ATOP(0x68)));

                        if (g_bDVILess70MHz != bClockLess70MHz)
                        {
                            //printf("\r\nAaron - SWP1  CL70_old = %d, CL70_new = %d\r\n", g_bDVILess70MHz, bClockLess70MHz);
                            g_bDVILess70MHz = bClockLess70MHz;
                            MDrv_HDMI_dvi_adjust(bClockLess70MHz);

                        }
                    }
                }
                g_DVI70MhzPollingCounter = 20; // 20 msec
            }
            break;

        case INPUT_PORT_DVI2:

//            if(!(MDrv_ReadByte(H_BK_ADC_DTOP(0x58)) & BIT3)) // DVI clock is stable
            if(!Hal_DVI_clk_stable_usingportc(YES)) // DVI clock is stable
            {
//                if(!(R2BYTEMSK(0x2C33, BIT5))) // Video clock is unstable
                if(!(R2BYTEMSK(0x2C32, BIT13))) // Video clock is unstable
                {
                    g_DVI70MhzUnstableCounter++;
                    if(g_DVI70MhzUnstableCounter >= 2)
                    {
                        g_DVI70MhzUnstableCounter = 0;

                        //bClockLess70MHz = (MDrv_ReadByte(H_BK_ADC_DTOP(0x58)) & BIT2) > 0 ? 1 : 0;
                        bClockLess70MHz = Hal_DVI_clk_lessthan70mhz_usingportc(YES); //Is Port C
                        //printf("\r\nAaron - CLK_CNT = %bx, AF= %bx\r\n", MDrv_ReadByte(L_BK_ADC_DTOP(0x58)), MDrv_ReadByte(H_BK_ADC_DTOP(0x58)));
                        //printf("\r\nAaron - VCO= %bx\r\n", MDrv_ReadByte(L_BK_ADC_ATOP(0x68)));

                        if (g_bDVILess70MHz != bClockLess70MHz)
                        {
                            //printf("\r\nAaron - CL70_old = %d, CL70_new = %d\r\n", g_bDVILess70MHz, bClockLess70MHz);
                            g_bDVILess70MHz = bClockLess70MHz;
                            MDrv_HDMI_dvi_adjust(bClockLess70MHz);

                        }
                    }
                }
                g_DVI70MhzPollingCounter = 20; // 20 msec
            }
            break;
        }

        // clear interrupt status
		W2BYTEMSK(REG_HDMI_63_L, BIT13, BIT13);  //REG_HDMI_INT_CLEAR
        W2BYTEMSK(REG_HDMI_63_L,     0, BIT13);  //REG_HDMI_INT_CLEAR
        //printf("\r\nAaron- DVICLOCK_PATH = %d\r\n", DVICLOCK_PATH);
    }
}

//-------------------------------------------------------------------------------------------------
/// avoid the snow image while AC on/off and source change for DVI 70Mhz issue, setting 2
/// @param  enInputPortType                \b IN: Input source
//-------------------------------------------------------------------------------------------------
void mdrv_dvi_clock_70mhz_swpatch2(INPUT_SOURCE_TYPE_t enInputSourceType)
{
    MS_BOOL bClockLess70MHz;
    static MS_U16 g_DVITMDSClock = 0;
    MS_U16 u16DVITMDSClock, u16DVIClockDiff;
    E_MUX_INPUTPORT enMuxPort =MApi_XC_Mux_GetHDMIPort(enInputSourceType);

//    if(R2BYTEMSK(0x2C33, BIT5)) // Video clock is stable
    if(R2BYTEMSK(0x2C32, BIT13)) // Video clock is stable
    {
        g_DVI70MhzUnstableCounter = 0;
        g_DVI70MhzPollingCounter = 0;

        #if ( WATCH_DOG == ENABLE )
            //msAPI_Timer_ResetWDT();
            MDrv_Sys_ClearWatchDog();
        #endif

        switch(enMuxPort)
        {
        default:
        case INPUT_PORT_DVI0:
        case INPUT_PORT_DVI1:
        case INPUT_PORT_DVI3:
            if(Hal_DVI_clk_stable_usingportc(NO)) // input clock is not really stable
            {
                //printf("\r\nAaron - False alarm, SWP2 TMDS Clock= %d\r\n", MDrv_Read2Byte(__REG_HDMI(0x11)) & 0x1FFF);
                return;
            }
            break;

        case INPUT_PORT_DVI2:
            if(Hal_DVI_clk_stable_usingportc(YES)) // input clock is not really stable
            {
                //printf("\r\nAaron - False alarm, SWP2 TMDS Clock= %d\r\n", MDrv_Read2Byte(__REG_HDMI(0x11)) & 0x1FFF);
                return;
            }
            break;
        }

//        u16DVITMDSClock = MDrv_Read2Byte(__REG_HDMI(0x11)) & 0x1FFF;
        u16DVITMDSClock = R2BYTE(REG_HDMI_11_L) & 0x1FFF;
        u16DVIClockDiff = (g_DVITMDSClock >= u16DVITMDSClock) ? (g_DVITMDSClock - u16DVITMDSClock) : (u16DVITMDSClock - g_DVITMDSClock);
        if(u16DVIClockDiff > 10) // tolerance = 1MHz
        {
            //printf("\r\nAaron - SWP2 Clock_Old = %d <-> Clock_New = %d \r\n", g_DVITMDSClock, u16DVITMDSClock);
            g_DVITMDSClock = u16DVITMDSClock;
            g_DVI70MhzPollingCounter = 20; // 20 msec
            return;
        }
//        bClockLess70MHz = (MDrv_ReadByte(H(__REG_HDMI(0x11))) & 0x60) > 0 ? 0 : 1;
        bClockLess70MHz = (R2BYTEMSK(REG_HDMI_11_L, HBMASK) & 0x60) > 0 ? 0 : 1;
     //printf("\r\nAaron - HDCP status = %bx, INT timer= %d\r\n", MDrv_ReadByte(L_BK_ADC_DTOP(0x79)), MDrv_ReadByte(H(__REG_HDMI(0x09))));
     //printf("\r\nAaron - SWP2 CL70_old = %d, CL70_new = %d\r\n", g_bDVILess70MHz, bClockLess70MHz);
     //printf("\r\nAaron - SWP2 TMDS Clock = %d\r\n", MDrv_Read2Byte(__REG_HDMI(0x11)) & 0x1FFF );
        if (g_bDVILess70MHz != bClockLess70MHz)
        {
            g_bDVILess70MHz = bClockLess70MHz;
            MDrv_HDMI_dvi_adjust(bClockLess70MHz);
        }
    }
}
// 20081226 - DVI+HDCP snow noise patch - end ...
#endif
//-------------------------------------------------------------------------------------------------
/// Get/Set error status
/// @param  u8value                \b IN: Write value
/// @param  bread                   \b IN: Read/Write (1/0)
/// @return @ref MS_U8
//-------------------------------------------------------------------------------------------------
MS_U8 MDrv_HDMI_err_status_update(MS_U8 u8value, MS_BOOL bread)
{
   return Hal_HDMI_err_status_update( u8value,  bread);
}

//-------------------------------------------------------------------------------------------------
/// HDMI packet reset
/// @param  breset                \b IN: Enabl reset or not
//-------------------------------------------------------------------------------------------------
void MDrv_HDMI_pkt_reset( HDMI_REST_t breset )
{
#if defined(CHIP_A2) || defined(CHIP_T12) || defined(CHIP_T8) || defined(CHIP_T9) || defined(CHIP_J2) || defined(CHIP_T11) || defined(CHIP_T13) || defined(CHIP_A1) || defined(CHIP_A6) || defined(CHIP_A7) || defined(CHIP_A5) || defined(CHIP_A3) || defined(CHIP_AMETHYST) || defined(CHIP_EAGLE) || defined(CHIP_AGATE)|| defined(CHIP_C3)
    Hal_HDMI_pkt_reset(MApi_XC_Mux_GetHDMIPort(gSrcInfo[0].enInputSourceType), breset);
#else
    Hal_HDMI_pkt_reset(breset);
#endif
}

//-------------------------------------------------------------------------------------------------
/// Enable Hot plug Detection out enable
/// @param  benable                \b IN: Enabl output or not
//-------------------------------------------------------------------------------------------------
void MDrv_HDMI_hpd_en(MS_BOOL benable)
{
    Hal_HDMI_hpd_en(benable);
}

//-------------------------------------------------------------------------------------------------
/// Init the HDCP production key
/// @param  pu8HdcpKeyData                \b IN: HDCP key
//-------------------------------------------------------------------------------------------------
void MDrv_HDCP_initproductionkey( MS_U8 * pu8HdcpKeyData )
{
    Hal_HDCP_initproductionkey(pu8HdcpKeyData);
}

//-------------------------------------------------------------------------------------------------
/// Clear the HDCP Flag
//-------------------------------------------------------------------------------------------------
void MDrv_HDCP_ClearStatus( void )
{
#if defined(CHIP_A2) || defined(CHIP_T12) || defined(CHIP_T8) || defined(CHIP_T9) || defined(CHIP_J2) || defined(CHIP_T11) || defined(CHIP_MARIA10) || defined(CHIP_T13) || defined(CHIP_A1) || defined(CHIP_A6) || defined(CHIP_A7) || defined(CHIP_A5) || defined(CHIP_A3) || defined(CHIP_AMETHYST) || defined(CHIP_EAGLE) || defined(CHIP_AGATE)|| defined(CHIP_C3)
    Hal_HDCP_clearflag(MApi_XC_Mux_GetHDMIPort(gSrcInfo[0].enInputSourceType));
#else
    Hal_HDCP_clearflag();
#endif
}
//-------------------------------------------------------------------------------------------------
/// Enable audio output or not
/// @param  bEnable                \b IN: Enable or not
//-------------------------------------------------------------------------------------------------
void MDrv_HDMI_audio_output( MS_BOOL bEnable )
{
    Hal_HDMI_audio_output(bEnable);
}
//-------------------------------------------------------------------------------------------------
/// Get the HDMI packet info
/// @return @ref MS_U16
//-------------------------------------------------------------------------------------------------
MS_U16 MDrv_HDMI_packet_info(void)
{
    return Hal_HDMI_packet_info();
}
//-------------------------------------------------------------------------------------------------
/// audio content protection info
/// @return @ref MS_U8
//-------------------------------------------------------------------------------------------------
MS_U8 MDrv_HDMI_audio_cp_hdr_info(void)
{
    return Hal_HDMI_audio_content_protection_info();
}
//-------------------------------------------------------------------------------------------------
/// Get the AVI infoframe information
/// @param  u8byte                \b IN: byte info selection
/// @return @ref MS_U8
//-------------------------------------------------------------------------------------------------
MS_U8 MDrv_HDMI_avi_infoframe_info(MS_U8 u8byte)
{
    return Hal_HDMI_avi_infoframe_info(u8byte);
}

//-------------------------------------------------------------------------------------------------
/// Get manufactor ID information(0x47~0x53)
/// @param  u8byte *               \b IN: byte info selection
//-------------------------------------------------------------------------------------------------
void MDrv_HDMI_Get_MID_info(MS_U8 * pu8ManufactorData)
{
	MS_U8 i=0;
//    pkt_value.PKT_SPD_VALUE = u16tmp[0] & BIT(2);
    Hal_HDMI_packet_ctrl(0x04); //set SPD

	for(i=0; i<25; i++)
	{
		(pu8ManufactorData[i])=Hal_HDMI_avi_infoframe_info(i+2*(47-40)+1);
	}
}

//-------------------------------------------------------------------------------------------------
/// Get the HDMI packet value
/// @param  u8state                \b IN: MS_HDMI_PACKET_STATE_t
/// @param  u8byte_idx                \b IN: Byte index
/// @param  *pValue                \b IN: packet value
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_HDMI_get_packet_value(MS_HDMI_PACKET_STATE_t u8state, MS_U8 u8byte_idx, MS_U8 *pValue)
{
    return Hal_HDMI_get_packet_value( u8state, u8byte_idx, pValue);
}



//-------------------------------------------------------------------------------------------------
/// Get the audio channel status
/// @param  u8byte                \b IN: byte info selection
/// @return @ref MS_U8
//-------------------------------------------------------------------------------------------------
MS_U8 MDrv_HDMI_audio_channel_status(MS_U8 u8byte)
{
    return Hal_HDMI_audio_channel_status(u8byte);
}

MS_HDCP_STATUS_INFO_t enStatus_info;
//-------------------------------------------------------------------------------------------------
/// Get the HDCP status
/// @return @ref MS_HDCP_STATUS_INFO_t
//-------------------------------------------------------------------------------------------------
MS_HDCP_STATUS_INFO_t* MDrv_HDCP_GetStatus(void )
{
#if defined(CHIP_A2) || defined(CHIP_T12) || defined(CHIP_T8) || defined(CHIP_T9) || defined(CHIP_J2) || defined(CHIP_T11) || defined(CHIP_MARIA10) || defined(CHIP_T13) || defined(CHIP_A1) || defined(CHIP_A6) || defined(CHIP_A7) || defined(CHIP_A5) || defined(CHIP_A3) || defined(CHIP_AMETHYST) || defined(CHIP_EAGLE) || defined(CHIP_AGATE)|| defined(CHIP_C3)
	MS_U16 u16Status = Hal_HDCP_getstatus(MApi_XC_Mux_GetHDMIPort(gSrcInfo[0].enInputSourceType));
#else
    MS_U16 u16Status = Hal_HDCP_getstatus();
#endif

    enStatus_info.b_St_HDMI_Mode = u16Status & BIT(0);
    enStatus_info.b_St_Reserve_1 = u16Status & BIT(1);
    enStatus_info.b_St_HDCP_Ver = u16Status & BIT(2);
    enStatus_info.b_St_AVMute = u16Status & BIT(3);
    enStatus_info.b_St_ENC_En = u16Status & BIT(4);
    enStatus_info.b_St_AKsv_Rcv = u16Status & BIT(5);
    enStatus_info.b_St_ENC_Dis = u16Status & BIT(6);
    enStatus_info.b_St_Reserve_7 = u16Status & BIT(7);

    return &enStatus_info;
}
//-------------------------------------------------------------------------------------------------
/// Returne Phase status
/// @param  bClockLessThan70MHz                \b IN: u8Channel
//-------------------------------------------------------------------------------------------------
MS_U16 MDrv_DVI_ChannelPhaseStatus( MS_DVI_CHANNEL_TYPE u8Channel)
{
    return Hal_DVI_ChannelPhaseStatus(u8Channel);
}

//-------------------------------------------------------------------------------------------------
/// Adjust the ADC for DVI by the condition is less than 70Mhz or not
/// @param  bClockLessThan70MHz                \b IN: Is Less 70Mhz or not
//-------------------------------------------------------------------------------------------------
void MDrv_HDMI_dvi_adjust( MS_BOOL bClockLessThan70MHz)
{
    Hal_DVI_adc_adjust(bClockLessThan70MHz);
}
//-------------------------------------------------------------------------------------------------
/// Packet control
/// @param  u16pkt                \b IN: Packet info
//-------------------------------------------------------------------------------------------------
void MDrv_HDMI_packet_ctrl(MS_U16 u16pkt)
{
    Hal_HDMI_packet_ctrl(u16pkt);
}
//-------------------------------------------------------------------------------------------------
/// audio content payload info
/// @param  u8byte                \b IN: Byte number
/// @return @ref MS_U8
//-------------------------------------------------------------------------------------------------
MS_U8 MDrv_HDMI_audio_cp_data_info(MS_U8 u8byte)
{
    return Hal_HDMI_audio_payload_info(u8byte);
}

MS_HDMI_PACKET_VALUE_t pkt_value;
//-------------------------------------------------------------------------------------------------
/// Get the HDMI packet
/// @param  bEnable                \b IN: Enable receive or not
/// @return @ref MS_HDMI_PACKET_VALUE_t
//-------------------------------------------------------------------------------------------------
MS_HDMI_PACKET_VALUE_t MDrv_HDMI_Packet_Received(MS_BOOL bEnable)
{
    MS_U16 u16tmp[1];

    if(bEnable)
    {
        u16tmp[0] = MDrv_HDMI_packet_info();
        //Need to write again to clear the packet flag
        MDrv_HDMI_packet_ctrl(u16tmp[0]);
    }
    else
    {
        u16tmp[0] = 0;
    }
    pkt_value.PKT_MPEG_VALUE = u16tmp[0] & BIT(0);
    pkt_value.PKT_AUI_VALUE = u16tmp[0] & BIT(1);
    pkt_value.PKT_SPD_VALUE = u16tmp[0] & BIT(2);
    pkt_value.PKT_AVI_VALUE = u16tmp[0] & BIT(3);
    pkt_value.PKT_GC_VALUE = u16tmp[0] & BIT(4);
    pkt_value.PKT_ASAMPLE_VALUE = u16tmp[0] & BIT(5);
    pkt_value.PKT_ACR_VALUE = u16tmp[0] & BIT(6);
    pkt_value.PKT_VS_VALUE = u16tmp[0] & BIT(7);
    pkt_value.PKT_NULL_VALUE = u16tmp[0] & BIT(8);
    pkt_value.PKT_ISRC2_VALUE = u16tmp[0] & BIT(9);
    pkt_value.PKT_ISRC1_VALUE = u16tmp[0] & BIT(10);
    pkt_value.PKT_ACP_VALUE = u16tmp[0] & BIT(11);
    pkt_value.PKT_ONEBIT_AUD_VALUE = u16tmp[0] & BIT(12);

    return pkt_value;
}
//-------------------------------------------------------------------------------------------------
/// Get the HDMI color format
/// @return @ref MS_HDMI_COLOR_FORMAT
//-------------------------------------------------------------------------------------------------
MS_HDMI_COLOR_FORMAT MDrv_HDMI_Get_ColorFormat(void)
{
    MS_HDMI_COLOR_FORMAT pkt_ColorFormat=MS_HDMI_COLOR_UNKNOWN;
    switch (  MDrv_HDMI_avi_infoframe_info(_BYTE_1)& 0x60 )
    {
        case 0x00:
            pkt_ColorFormat = MS_HDMI_COLOR_RGB;
            break;
        case 0x40:
            pkt_ColorFormat = MS_HDMI_COLOR_YUV_444;
            break;
        case 0x20:
            pkt_ColorFormat = MS_HDMI_COLOR_YUV_422;
            break;
        default:
            break;
    }
    return pkt_ColorFormat;
}
//-------------------------------------------------------------------------------------------------
/// Get the HDMI aspect ratio info
/// @param  bEn_PAR                \b IN: Enable to get the Picture Aspect Ratio
/// @param  bEn_AFAR              \b IN: Enable to get the Active Format Aspect Ratio
/// @return @ref MS_HDMI_AR_TYPE
//-------------------------------------------------------------------------------------------------
MS_HDMI_AR_TYPE MDrv_HDMI_Get_AspectRatio(MS_BOOL bEn_PAR, MS_BOOL bEn_AFAR)
{
    MS_HDMI_AR_TYPE enPar_type;
    MS_U8 u8AR = MDrv_HDMI_avi_infoframe_info(_BYTE_2) & HDMI_AR_REG_MASK;

    // Picture Aspect Ratio - PAR
    if(bEn_PAR)
    {
        switch(u8AR & HDMI_AR_MASK)
        {
            case 0x00:     // IF0[13..12] 00
                enPar_type = HDMI_Pic_AR_NODATA;
                break;
            case 0x10:     // IF0[13..12] 01, 4:3
                enPar_type = HDMI_Pic_AR_4x3;
                break;
            case 0x20:     // IF0[13..12] 10, 16:9
                enPar_type = HDMI_Pic_AR_16x9;
                break;
            case 0x30:     // IF0[13..12] 11, reserved
            default:
                enPar_type = HDMI_Pic_AR_RSV;
                break;
        }
    }
    else
    {
        enPar_type = HDMI_Pic_AR_RSV;
    }

    return enPar_type;
}
//-------------------------------------------------------------------------------------------------
/// Get the HDMI aspect ratio info
/// @param  bEn_AFAR              \b IN: Enable to get the Active Format Aspect Ratio
/// @return @ref MS_HDMI_AR_TYPE
//-------------------------------------------------------------------------------------------------
MS_HDMI_AR_TYPE MDrv_HDMI_Get_ActiveFormat_AspectRatio(MS_BOOL bEn_AFAR)
{
    MS_HDMI_AR_TYPE enAfar_types;
    MS_U8 u8AR = MDrv_HDMI_avi_infoframe_info(_BYTE_2) & HDMI_AR_REG_MASK;

    // Active Format Aspect Ratio - AFAR
    if(bEn_AFAR)
    {
        switch(u8AR & HDMI_AFAR_MASK)
        {
            case 0x02:    // IF0[11..8] 0010, box 16:9 (top)
                enAfar_types = HDMI_AF_AR_16x9_Top;
                break;
            case 0x03:    // IF0[11..8] 0011, box 14:9 (top)
                enAfar_types = HDMI_AF_AR_14x9_Top;
                break;
            case 0x04:    // IF0[11..8] 0100, box >16:9 (centre)
                enAfar_types = HDMI_AF_AR_GT_16x9;
                break;
            case 0x08:    // IF0[11..8] 1000, same as picture
                enAfar_types = HDMI_AF_AR_SAME;
                break;
            case 0x09:    // IF0[11..8] 1001, 4:3 Center
                enAfar_types = HDMI_AF_AR_4x3_C;
                break;
            case 0x0A:    // IF0[11..8] 1010, 16:9 Center
                enAfar_types = HDMI_AF_AR_16x9_C;
                break;
            case 0x0B:    // IF0[11..8] 1011, 14:9 Center
                enAfar_types = HDMI_AF_AR_14x9_C;
                break;
            case 0x0D:    // IF0[11..8] 1101, 4:3 with shoot and protect 14:9 centre
                enAfar_types = HDMI_AF_AR_4x3_with_14x9_C;
                break;
            case 0x0E:    // IF0[11..8] 1110, 16:9 with shoot and protect 14:9 centre
                enAfar_types = HDMI_AF_AR_16x9_with_14x9_C;
                break;
            case 0x0F:    // IF0[11..8] 1111, 16:9 with shoot and protect 4:3 centre.
                enAfar_types = HDMI_AF_AR_16x9_with_4x3_C;
                break;

            default:
                enAfar_types = HDMI_AF_AR_SAME;
                break;
        }
    }
    else
    {
        enAfar_types = HDMI_AF_AR_SAME;
    }

    return enAfar_types;
}

//-------------------------------------------------------------------------------------------------
/// Get the Pixel repetition from player
/// @return @ref MS_HDMI_COLOR_FORMAT
//-------------------------------------------------------------------------------------------------
MS_U8 MDrv_HDMI_Get_Pixel_Repetition(void)
{
    return MDrv_HDMI_avi_infoframe_info(_BYTE_5)& 0x0F;

}
//-------------------------------------------------------------------------------------------------
/// Pull DVI Clock to low
/// @param  bPullLow                          \b IN: Set DVI clock to low
/// @param  enInputPortType              \b IN: Input source
//-------------------------------------------------------------------------------------------------
void MDrv_DVI_ClkPullLow(MS_BOOL bPullLow, E_MUX_INPUTPORT enInputPortType)
{
    Hal_DVI_ClkPullLow(bPullLow, enInputPortType);
}
//-------------------------------------------------------------------------------------------------
/// Switch DVI source
/// @param  enInputPortType              \b IN: Input source
//-------------------------------------------------------------------------------------------------
void MDrv_DVI_SwitchSrc(E_MUX_INPUTPORT enInputPortType)
{
    Hal_DVI_SwitchSrc(enInputPortType);
}

void MDrv_DVI_IMMESWITCH_PS_SW_Path(void)
{
    if(g_bIsIMMESWITCH) // Only apply in imme switch case
    {
        Hal_DVI_IMMESWITCH_PS_SW_Path();
    }
}

/// lib version
HDMI_Ret_Value MDrv_HDMI_GetLibVer(const MSIF_Version **ppVersion)
{
    if (!ppVersion)
    {
        return E_HDMI_RET_FAIL;
    }

    *ppVersion = &_drv_hdmi_version;
    return E_HDMI_RET_OK;
}

// lib information
HDMI_Ret_Value  MDrv_HDMI_GetInfo(MS_HDMI_INFO *pInfo)
{
    memcpy((void*)pInfo, (void*)&_info, sizeof(MS_HDMI_INFO));
    return E_HDMI_RET_OK;
}

// curretn status
HDMI_Ret_Value  MDrv_HDMI_GetStatus(MS_HDMI_Status *pStatus)
{
    memcpy((void*)pStatus, (void*)&_status, sizeof(MS_HDMI_Status));
    return E_HDMI_RET_OK;
}


// DDC
static void MDrv_HDMI_WriteEDID(E_XC_DDCRAM_PROG_TYPE eDDCRamType, MS_U8 *u8EDID, MS_U16 u8EDIDSize)
{
    MS_U16 i;

    HAL_HDMI_DDCRam_SelectPort(eDDCRamType);

    for( i = 0; i < u8EDIDSize; i++)
    {
        if(eDDCRamType == E_XC_PROG_VGA_EDID)
        {
            DDC_WRITE_MASK(REG_DDC_A_ACCESS,REG_DDC_A_ACCESS_BITS(0),REG_DDC_A_ACCESS_MASK);
            DDC_WRITE_MASK(REG_DDC_A_WRITEADDR,REG_DDC_A_WRITEADDR_BITS(i),REG_DDC_A_WRITEADDR_MASK);
            DDC_WRITE_MASK(REG_DDC_A_WRITEDATA,REG_DDC_A_WRITEDATA_BITS(*(u8EDID + i)),REG_DDC_A_WRITEDATA_MASK);
            DDC_WRITE_MASK(REG_DDC_A_WRITEPULSE,REG_DDC_A_WRITEPULSE_BITS(1),REG_DDC_A_WRITEPULSE_MASK);
        }
        else
        {
            DDC_WRITE_MASK(REG_DDC_D_ACCESS,REG_DDC_D_ACCESS_BITS(0),REG_DDC_D_ACCESS_MASK);
            DDC_WRITE_MASK(REG_DDC_D_WRITEADDR,REG_DDC_D_WRITEADDR_BITS(i),REG_DDC_D_WRITEADDR_MASK);
            DDC_WRITE_MASK(REG_DDC_D_WRITEDATA,REG_DDC_D_WRITEDATA_BITS(*(u8EDID + i)),REG_DDC_D_WRITEDATA_MASK);
            DDC_WRITE_MASK(REG_DDC_D_WRITEPULSE,REG_DDC_D_WRITEPULSE_BITS(1),REG_DDC_D_WRITEPULSE_MASK);
        }
    }
	switch(eDDCRamType)
    {
        case E_XC_PROG_DVI0_EDID:
            DDC_WRITE_MASK(REG_DDC_DVI0_WP,BITS(13:13,1),BMASK(13:13)); // Write Protect 0 ddc
            break;
        case E_XC_PROG_DVI1_EDID:
            DDC_WRITE_MASK(REG_DDC_DVI1_WP,BITS(13:13,1),BMASK(13:13)); // Write Protect 1 ddc
            break;
        case E_XC_PROG_DVI2_EDID:
            DDC_WRITE_MASK(REG_DDC_DVI2_WP,BITS(5:5,1),BMASK(5:5));     // Write Protect 2 ddc
            break;
        case E_XC_PROG_DVI3_EDID:
            DDC_WRITE_MASK(REG_DDC_DVI3_WP,BITS(5:5,1),BMASK(5:5));     //Write Protect  3 ddc
            break;
        case E_XC_PROG_VGA_EDID:
            DDC_WRITE_MASK(REG_DDC_ADC0_WP,BITS(13:13,1),BMASK(13:13)); // Write Protect analog ddc
            break;
        default:
            MS_ASSERT(0);
		}
}

static void MDrv_HDMI_DDCRAM_Enable(E_XC_DDCRAM_PROG_TYPE eDDCRamType)
{
    switch(eDDCRamType)
    {
        case E_XC_PROG_DVI0_EDID:
            DDC_WRITE_MASK(REG_DDC_DVI0_EN,BITS(15:15,1),BMASK(15:15)); // enable digital 0 ddc
            break;
        case E_XC_PROG_DVI1_EDID:
            DDC_WRITE_MASK(REG_DDC_DVI1_EN,BITS(15:15,1),BMASK(15:15)); // enable digital 1 ddc
            break;
        case E_XC_PROG_DVI2_EDID:
            DDC_WRITE_MASK(REG_DDC_DVI2_EN,BITS(7:7,1),BMASK(7:7));     // enable digital 2 ddc
            break;
        case E_XC_PROG_DVI3_EDID:
            DDC_WRITE_MASK(REG_DDC_DVI3_EN,BITS(7:7,1),BMASK(7:7));     // enable digital 3 ddc
            break;
        case E_XC_PROG_VGA_EDID:
            DDC_WRITE_MASK(REG_DDC_ADC0_EN,BITS(15:15,1),BMASK(15:15)); // enable analog ddc
            break;
        default:
            MS_ASSERT(0);
    }
}

void MDrv_HDMI_PROG_DDCRAM(XC_DDCRAM_PROG_INFO *pstDDCRam_Info, MS_U32 u32SizeOfInfo)
{
    (u32SizeOfInfo)=(u32SizeOfInfo); //this Var is temp for UNUSE

    MDrv_HDMI_DDCRAM_Enable(pstDDCRam_Info->eDDCProgType);

    MDrv_HDMI_WriteEDID(pstDDCRam_Info->eDDCProgType, pstDDCRam_Info->EDID, pstDDCRam_Info->u16EDIDSize);

    HAL_HDMI_DDCRAM_SetPhyAddr(pstDDCRam_Info);
}

/** According to the previous experience, some specific source devices would
send strange DVI / HDMI signal in slow frequency(like 480p) let us in trouble.
So it's better to increase DVI updated phase count to 100 lines in low
resolution to overcome that situation, originally DVI updated phase count is 1.
So the patch would check input DVI frequency first, if small than 50MHz, DVI
updated phase count would increase to 100 lines. Otherwise, DVI updated phase
count would be recovered to 1 line. */
void MDrv_HDMI_SetUpdatePhaseLineCount(INPUT_SOURCE_TYPE_t enInputSourceType)
{
    if( MApi_XC_Mux_GetHDMIPort(enInputSourceType) == INPUT_PORT_DVI0 || MApi_XC_Mux_GetHDMIPort(enInputSourceType) == INPUT_PORT_DVI1 ||
        MApi_XC_Mux_GetHDMIPort(enInputSourceType) == INPUT_PORT_DVI3 )
    {
        if( R2BYTEMSK(REG_DVI_ATOP_61_L, BMASK(9:8)) == 2<<8 ||
            R2BYTEMSK(REG_DVI_ATOP_61_L, BMASK(9:8)) == 3<<8 ) //<50Mhz
        {
			W2BYTEMSK(REG_DVI_DTOP_29_L, 0x0000, 0xFF00);
			W2BYTE(REG_DVI_DTOP_2A_L, 0x0000);
//            printf("Low resolution\n");

        }
        else
      {
            W2BYTEMSK(REG_DVI_DTOP_29_L, 0, 0xFF00);
			W2BYTE(REG_DVI_DTOP_2A_L, 0);
//            printf("High resolution\n");
        }

     }
     else if(MApi_XC_Mux_GetHDMIPort(enInputSourceType) == INPUT_PORT_DVI2)
     {
        if( R2BYTEMSK(REG_DVI_ATOP_6A_L, BMASK(9:8)) == 2<<8 ||
            R2BYTEMSK(REG_DVI_ATOP_6A_L, BMASK(9:8)) == 3<<8 ) //<50Mhz
        {
            W2BYTEMSK(REG_DVI_DTOP_29_L, 0x0000, 0xFF00);
            W2BYTE(REG_DVI_DTOP_2A_L, 0x0000);
//            printf("Low resolution 2\n");
        }
        else
        {
            W2BYTEMSK(REG_DVI_DTOP_29_L, 0, 0xFF00);
            W2BYTE(REG_DVI_DTOP_2A_L, 0);
//            printf("High resolution 2\n");
        }

     }

}

void MDrv_HDMI_Get_PollingStatus(HDMI_POLLING_STATUS_t **pStatus)
{
    *pStatus = &g_HdmiPollingStatus;
}

MS_BOOL MDrv_HDMI_IsHDMI_Mode(void)
{
    return g_HdmiPollingStatus.bIsHDMIMode;
}

MS_U16 MDrv_HDMI_GetHDE(void)
{
    return Hal_HDMI_GetHDE();
}
#if 0
void MDrv_HDMI_ENABLE_IMMESWITCH( MS_BOOL bEnImmeswitch,MS_BOOL bEnPowSaving )
{
    if(SUPPORT_IMMESWITCH)
    {
        g_bIsIMMESWITCH = bEnImmeswitch;
        g_bIMMESWITCH_DVI_POWERSAVING = bEnPowSaving;
    }
    else
    {
        g_bIsIMMESWITCH = FALSE;
        g_bIMMESWITCH_DVI_POWERSAVING = FALSE;
    }
}
#endif

#if 0
void MDrv_HDMI_ImmeswitchPS_Handler(void)
{
    E_MUX_INPUTPORT port;
    if(g_bIMMESWITCH_DVI_POWERSAVING)
    {
        for(port = INPUT_PORT_DVI0; port <=INPUT_PORT_DVI3; port++)
        {
            //if(!Hal_DVI_GetPowerSavingStatus(port)) // not need to check if power saving mode is not enable
            //continue;

            if(Hal_DVI_GetDEStableStatus(port)) // stable
            {
                if(u8PSDVIStableCount[port-INPUT_PORT_DVI0] < 255)
                	u8PSDVIStableCount[port-INPUT_PORT_DVI0] += 1;
            }
            else // unstable
            {
                if(u8PSDVIStableCount[port-INPUT_PORT_DVI0] > 0)
                {
                    Hal_DVI_EnhanceImmeswitch(port, FALSE);
                    msg_hdmi(printf("$$$ MDrv_HDMI_ImmeswitchPS_Handler:  port %d is unstable\n", port-INPUT_PORT_DVI0));
                }
                u8PSDVIStableCount[port-INPUT_PORT_DVI0] = 0;
            }

            if(u8PSDVIStableCount[port-INPUT_PORT_DVI0] >= 3) // enable Immeswithc enhancement only when continous DE stable
            {
                if(u8PSDVIStableCount[port-INPUT_PORT_DVI0] == 3)
                {
                    Hal_DVI_EnhanceImmeswitch(port, TRUE);
                    msg_hdmi(printf("$$$ MDrv_HDMI_ImmeswitchPS_Handler:  port %d is stable\n", port-INPUT_PORT_DVI0));
                }
            }
        }
    }
}

void MDrv_DVI_ForceAllPortsEnterPS(void)
{
    if(g_bIMMESWITCH_DVI_POWERSAVING)
    {
        msg_hdmi(printf("$$$ MDrv_DVI_ForceAllPortsEnterPS \n"));
        Hal_DVI_ForceAllPortsEnterPS();
    }
}
#endif

#if defined(CHIP_T12) || defined(CHIP_A2) || defined(CHIP_T8) || defined(CHIP_T9) || defined(CHIP_J2) || defined(CHIP_T11) || defined(CHIP_T13) || defined(CHIP_A1) || defined(CHIP_A6) || defined(CHIP_A7) || defined(CHIP_A5) || defined(CHIP_A3) || defined(CHIP_AMETHYST) || defined(CHIP_EAGLE)// HDMI 1.4 new feature
void MDrv_HDMI_3D_4Kx2K_Process(void)
{
    E_HDMI_ADDITIONAL_VIDEO_FORMAT val;

    if(pkt_value.PKT_VS_VALUE) // VSP packet should be received first
    {
        val = Hal_HDMI_Check_Additional_Format();
        if(Hal_HDMI_Is_Input_Large_166MHz() && ((val == E_HDMI_3D_FORMAT)|(val == E_HDMI_4Kx2K_FORMAT)))
        {
            Hal_HDMI_AVG_ScaleringDown(TRUE);
            msg_hdmi(printf("~~~ HDMI 3D or 4Kx2K scaling down= %x\n", val));
        }
        else
        {
            Hal_HDMI_AVG_ScaleringDown(FALSE);
        }

        if(Hal_HDMI_Check_Additional_Format() == E_HDMI_3D_FORMAT)
        {
            Hal_HDMI_Set_YUV422to444_Bypass(FALSE);
            msg_hdmi(printf("~~~ HDMI 3D 422 to 444 Enable\n"));
        }
        else
        {
            Hal_HDMI_Set_YUV422to444_Bypass(TRUE);
            msg_hdmi(printf("~~~ HDMI 422 to 444 Bypass\n"));
        }

    }
}

E_HDMI_ADDITIONAL_VIDEO_FORMAT MDrv_HDMI_Check_Additional_Format(void)
{
    if(pkt_value.PKT_VS_VALUE) // VSP packet should be received first
        return Hal_HDMI_Check_Additional_Format();
    else
        return E_HDMI_NA;
}

E_XC_3D_INPUT_MODE MDrv_HDMI_Get_3D_Structure(void)
{
    if(pkt_value.PKT_VS_VALUE) // VSP packet should be received first
        return Hal_HDMI_Get_3D_Structure();
    else
        return E_XC_3D_INPUT_MODE_NONE;
}

E_HDMI_3D_EXT_DATA_T MDrv_HDMI_Get_3D_Ext_Data(void)
{
    if(pkt_value.PKT_VS_VALUE) // VSP packet should be received first
        return Hal_HDMI_Get_3D_Ext_Data();
    else
        return E_3D_EXT_DATA_MODE_MAX;
}

void MDrv_HDMI_Get_3D_Meta_Field(sHDMI_3D_META_FIELD *pdata)
{
    if(pkt_value.PKT_VS_VALUE) // VSP packet should be received first
        Hal_HDMI_Get_3D_Meta_Field(pdata);
    else
        memset(pdata, 0, sizeof(sHDMI_3D_META_FIELD));
}

MS_U8 MDrv_HDMI_Get_VIC_Code(void)
{
    if(pkt_value.PKT_AVI_VALUE) // AVI packet should be received first
        return Hal_HDMI_Get_VIC_Code();
    else
        return 0;
}

E_HDMI_VIC_4Kx2K_CODE MDrv_HDMI_Get_4Kx2K_VIC_Code(void)
{
    if(pkt_value.PKT_VS_VALUE) // VSP packet should be received first
        return Hal_HDMI_Get_4Kx2K_VIC_Code();
    else
        return E_VIC_NA;
}

// HDMI ARC pin control
//     - enInputPortType: INPUT_PORT_DVI0 / INPUT_PORT_DVI1 / INPUT_PORT_DVI2 / INPUT_PORT_DVI3
//     - bEnable: ARC enable or disable
//     - bDrivingHigh: ARC driving current high or low, suggest driving current should be set to high when ARC is enable
void MDrv_HDMI_ARC_PINControl(E_MUX_INPUTPORT enInputPortType, MS_BOOL bEnable, MS_BOOL bDrivingHigh)
{
    Hal_HDMI_ARC_PINControl(enInputPortType, bEnable, bDrivingHigh);
}

/******************************************************************************/
///DVI soft reset
/******************************************************************************/
//[6]: soft-reset hdmi
//[5]: soft-reset hdcp
//[4]: soft-reset dvi
void Hal_DVI_Software_Reset(E_MUX_INPUTPORT enInputPortType, MS_U16 u16Reset)
{
    MS_U32 u32RegAdr;

    switch(enInputPortType)
    {
        case INPUT_PORT_DVI0://A
            u32RegAdr = REG_DVI_ATOP_07_L;
            break;
        case INPUT_PORT_DVI1://B
            u32RegAdr = REG_DVI_ATOP1_07_L;
            break;
        case INPUT_PORT_DVI3://D
            u32RegAdr = REG_DVI_ATOP2_07_L;
            break;
        case INPUT_PORT_DVI2://C
            u32RegAdr = REG_DVI_ATOP_04_L;
            break;
        default:
            printf("enInputPortType=0x%X error at %s %u\n", enInputPortType, __FUNCTION__, __LINE__);
            return;
            break;
    }
    //printf("daniel_test u32RegAdr=0x%lX\n", u32RegAdr );

    W2BYTE(u32RegAdr, u16Reset);
    OS_DELAY_TASK(1); // delay 1 usec if possible
    /*{ // For system delay
        MS_U32 t;
        t = MsOS_GetSystemTime();
        t = MsOS_GetSystemTime();
    }*/
    W2BYTE(u32RegAdr, 0x0000);

}

//[6]: soft-reset hdmi
//[5]: soft-reset hdcp
//[4]: soft-reset dvi
void MDrv_DVI_Software_Reset(E_MUX_INPUTPORT enInputPortType, MS_U16 u16Reset)
{
    Hal_DVI_Software_Reset(enInputPortType, u16Reset);
}

#endif
