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
#define MHAL_IP_C


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/wait.h>
#endif

// Common Definition
#include "MsCommon.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"

// Internal Definition
#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "mvideo_context.h"
#include "xc_Analog_Reg.h"
#include "mhal_ip.h"
#include "apiXC_Adc.h"
#include "drvXC_ADC_Internal.h"
#include "mhal_adc.h"
#include "drv_sc_ip.h"
#include "xc_hwreg_utility2.h"
#include "hwreg_sc.h"
#include "hwreg_sc1.h"
#include "hwreg_sc2.h"
#include "hwreg_ipmux.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define DRVSCIP_DBG(x)  //x


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
///software reset for scaler
///@param u16Sel \b IN
///- software reset VDFE: BIT(15)
///- software reset VDCOMBF: BIT(14)
///- software reset EMCU: BIT(13)
///- software reset GMC: BIT(12)
///- software reset REG: BIT(7)
///- software reset ADC: BIT(6)
///- software reset DIGITAL: BIT(5)
///- software reset SCALER: BIT(4)
///- software reset DISPLAY: BIT(3)
///- software reset OSD: BIT(1)
///- software reset ALL: BIT(0)
///@param u8DelayTime \b IN:
///- software reset delay time
/******************************************************************************/
void Hal_SC_ip_software_reset(MS_U8 u8Reset, SCALER_WIN eWindow)
{
/*  if ((u8Reset & RST_SCALER_ALL) || (u8Reset & RST_IP_ALL) || (u8Reset & RST_IP_F2))
    {
        XBYTE[0x2F00]=0x6;
        XBYTE[0x2F21]|=0x80;
        XBYTE[0x2F00]=0x0;
        MsOS_DelayTask(1);
    }*/

    MS_U8  u8CLK1Mux = eWindow == SUB_WINDOW ? MDrv_ReadByte(REG_CKG_IDCLK1) :   //Sub window
                                               MDrv_ReadByte(REG_CKG_SC2_IDCLK1);

    MS_U8  u8CLK2Mux = eWindow == MAIN_WINDOW     ? MDrv_ReadByte(REG_CKG_IDCLK2) : //Main window
                       eWindow == SC1_MAIN_WINDOW ? MDrv_ReadByte(REG_CKG_SC1_IDCLK2) :
                                                    MDrv_ReadByte(REG_CKG_SC2_IDCLK2);

    if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
    {
        SC_W2BYTEMSK( REG_SC_BK00_02_L, u8Reset, 0xFF );
    }
    else if(eWindow == SC2_MAIN_WINDOW || eWindow == SC2_SUB_WINDOW)
    {
        SC_W2BYTEMSK( REG_SC2_BK00_02_L, u8Reset, 0xFF );
    }
    else
    {
        SC_W2BYTEMSK( REG_SC1_BK00_02_L, u8Reset, 0xFF );
    }

    MsOS_DelayTask(1);
    if(u8Reset == REST_IP_F1) //Sub window
    {
        if(eWindow == SUB_WINDOW)
        {
            MDrv_WriteByteMask(REG_CKG_IDCLK1, CKG_IDCLK1_XTAL, CKG_IDCLK1_MASK); // Sub window reset to XTAL when ADC input no signal
            MDrv_WriteByteMask(REG_CKG_IDCLK1, u8CLK1Mux, CKG_IDCLK1_MASK); // Sub window reset to XTAL when ADC input no signal
        }
        else
        {
            MDrv_WriteByteMask(REG_CKG_SC2_IDCLK1, CKG_SC2_IDCLK1_XTAL, CKG_SC2_IDCLK1_MASK); // Sub window reset to XTAL when ADC input no signal
            MDrv_WriteByteMask(REG_CKG_SC2_IDCLK1, u8CLK1Mux, CKG_SC2_IDCLK1_MASK); // Sub window reset to XTAL when ADC input no signal
        }
    }
    else if(u8Reset == REST_IP_F2) //Main window
    {
        if(eWindow == MAIN_WINDOW)
        {
            MDrv_WriteByteMask(REG_CKG_IDCLK2, CKG_IDCLK2_XTAL, CKG_IDCLK2_MASK); // Main window reset to XTAL when ADC input no signal
            MDrv_WriteByteMask(REG_CKG_IDCLK2, u8CLK2Mux, CKG_IDCLK2_MASK); // Main window reset to XTAL when ADC input no signal
        }
        else if(eWindow == SC1_MAIN_WINDOW)
        {
            MDrv_WriteByteMask(REG_CKG_SC1_IDCLK2, CKG_SC1_IDCLK2_XTAL, CKG_SC1_IDCLK2_MASK); // Main window reset to XTAL when ADC input no signal
            MDrv_WriteByteMask(REG_CKG_SC1_IDCLK2, u8CLK2Mux, CKG_SC1_IDCLK2_MASK); // Main window reset to XTAL when ADC input no signal
        }
        else
        {
            MDrv_WriteByteMask(REG_CKG_SC2_IDCLK2, CKG_SC2_IDCLK2_XTAL, CKG_SC2_IDCLK2_MASK); // Main window reset to XTAL when ADC input no signal
            MDrv_WriteByteMask(REG_CKG_SC2_IDCLK2, u8CLK2Mux, CKG_SC2_IDCLK2_MASK); // Main window reset to XTAL when ADC input no signal
        }
    }
    else if(u8Reset == REST_IP_ALL) //Main and sub windows
    {

        if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
        {
            MDrv_WriteByteMask(REG_CKG_IDCLK1, CKG_IDCLK1_XTAL, CKG_IDCLK1_MASK); // Sub window reset to XTAL when ADC input no signal
            MDrv_WriteByteMask(REG_CKG_IDCLK1, u8CLK1Mux, CKG_IDCLK1_MASK); // Sub window reset to XTAL when ADC input no signal
            MDrv_WriteByteMask(REG_CKG_IDCLK2, CKG_IDCLK2_XTAL, CKG_IDCLK2_MASK); // Main window reset to XTAL when ADC input no signal
            MDrv_WriteByteMask(REG_CKG_IDCLK2, u8CLK2Mux, CKG_IDCLK2_MASK); // Main window reset to XTAL when ADC input no signal
        }
        else if(eWindow == SC2_MAIN_WINDOW || eWindow == SC2_SUB_WINDOW)
        {
            MDrv_WriteByteMask(REG_CKG_SC2_IDCLK1, CKG_SC2_IDCLK1_XTAL, CKG_SC2_IDCLK1_MASK); // Sub window reset to XTAL when ADC input no signal
            MDrv_WriteByteMask(REG_CKG_SC2_IDCLK1, u8CLK1Mux, CKG_SC2_IDCLK1_MASK); // Sub window reset to XTAL when ADC input no signal
            MDrv_WriteByteMask(REG_CKG_SC2_IDCLK2, CKG_SC2_IDCLK2_XTAL, CKG_SC2_IDCLK2_MASK); // Main window reset to XTAL when ADC input no signal
            MDrv_WriteByteMask(REG_CKG_SC2_IDCLK2, u8CLK2Mux, CKG_SC2_IDCLK2_MASK); // Main window reset to XTAL when ADC input no signal
        }
        else
        {
            MDrv_WriteByteMask(REG_CKG_SC1_IDCLK2, CKG_SC1_IDCLK2_XTAL, CKG_SC1_IDCLK2_MASK); // Main window reset to XTAL when ADC input no signal
            MDrv_WriteByteMask(REG_CKG_SC1_IDCLK2, u8CLK2Mux, CKG_SC1_IDCLK2_MASK); // Main window reset to XTAL when ADC input no signal
        }
    }

    if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
    {
        SC_W2BYTEMSK( REG_SC_BK00_02_L, 0x00, 0xFF );
    }
    else if(eWindow == SC2_MAIN_WINDOW || eWindow == SC2_SUB_WINDOW)
    {
        SC_W2BYTEMSK( REG_SC2_BK00_02_L, 0x00, 0xFF );
    }
    else
    {
        SC_W2BYTEMSK( REG_SC1_BK00_02_L, 0x00, 0xFF );
    }

    MsOS_DelayTask(1);

/*    if ((u8Reset & RST_SCALER_ALL) || (u8Reset & RST_IP_ALL) || (u8Reset & RST_IP_F2))
    {
        XBYTE[0x2F00]=0x6;
        XBYTE[0x2F21]&=~(0x80);
        XBYTE[0x2F00]=0x0;
    }*/
}

MS_U8 Hal_SC_ip_get_sync_flag(SCALER_WIN eWindow)
{
    MS_U8 u8SyncFlag;
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_1E_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_1E_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_1E_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_1E_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_1E_L :
                                          REG_SC_DUMMY;

    u8SyncFlag = SC_R2BYTE(u32Reg) & 0x00FF;

    return u8SyncFlag;
}

MS_U8 Hal_SC_ip_get_sync_detect_status(SCALER_WIN eWindow)
{
    MS_U8 u8DetectStatus;
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_1E_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_1E_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_1E_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_1E_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_1E_L :
                                          REG_SC_DUMMY;
    /* mode detect status */
    u8DetectStatus = (SC_R2BYTE(u32Reg) & 0xFF00)>>8;

    return u8DetectStatus;
}

MS_U8 Hal_SC_ip_get_input_vsync_polarity(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_1E_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_1E_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_1E_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_1E_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_1E_L :
                                          REG_SC_DUMMY;

    return (SC_R2BYTE(u32Reg) & BIT(8)) ? 1:0;
}

MS_U8 Hal_SC_ip_get_input_vsync_value(SCALER_WIN eWindow)
{
    return (Hal_SC_ip_get_sync_flag(eWindow) & BIT(2)) ? 1:0;
}

MS_U8 Hal_SC_ip_get_output_vsync_value(SCALER_WIN eWindow)
{
    return (Hal_SC_ip_get_sync_flag(eWindow) & BIT(0)) ? 1 : 0;
}

void Hal_SC_ip_set_reg_usr_vspolmd(MS_U8 u8Enable, SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_21_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_21_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_21_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_21_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_21_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg, u8Enable ? BIT(12) : 0, BIT(12));
}

void Hal_SC_ip_init_reg_in_timingchange(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_21_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_21_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_21_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_21_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_21_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg,0x0000,0x01FF);

    // restore coast window to default settings
    Hal_SC_ip_set_coast_window( 0x0C, 0x0C, eWindow );
}

/******************************************************************************/
///This function will return Horizontal period value
///@return
///- MS_U16 Horizontal Period
/******************************************************************************/
MS_U16 Hal_SC_ip_get_horizontalPeriod(SCALER_WIN eWindow)
{
    MS_U16 u16HorizontalPeriod;
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_20_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_20_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_20_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_20_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_20_L :
                                          REG_SC_DUMMY;

    u16HorizontalPeriod = ( SC_R2BYTE( u32Reg) & MST_H_PERIOD_MASK );

    return u16HorizontalPeriod;
}


/******************************************************************************/
///This function will return Vertical total value
///@return
///- MS_U16 Vertical total
/******************************************************************************/
MS_U16 Hal_SC_ip_get_verticaltotal(SCALER_WIN eWindow)
{
    MS_U16 u16VerticalTotal;
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_1F_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_1F_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_1F_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_1F_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_1F_L :
                                          REG_SC_DUMMY;

    u16VerticalTotal = ( SC_R2BYTE( u32Reg) & MST_V_TOTAL_MASK );

    return u16VerticalTotal;
}
#if 0
/******************************************************************************/
/// Get the horizontal period of auto postion
/// @return the horizontal period of auto postion
/******************************************************************************/
MS_U16 MDrv_Scaler_GetHorizontalDEStart(void)
{
    MS_U8 u8Bank;
    MS_U16 u16HorizontalDE;

    u8Bank = MDrv_ReadByte ( BK_SELECT_00 );
    MDrv_WriteByte ( BK_SELECT_00, REG_BANK_IP1F2 );

    u16HorizontalDE = MDrv_Read2Byte ( L_BK_IP1F2 ( 0x13 ) );

    MDrv_WriteByte ( BK_SELECT_00, u8Bank );

    return u16HorizontalDE;
}


/******************************************************************************/
/// Get the horizontal period of auto postion
/// @return the horizontal period of auto postion
/******************************************************************************/
MS_U16 MDrv_Scaler_GetHorizontalDE(void)
{
    MS_U8 u8Bank;
    MS_U16 u16HorizontalDE;

    u8Bank = MDrv_ReadByte ( BK_SELECT_00 );
    MDrv_WriteByte ( BK_SELECT_00, REG_BANK_IP1F2 );

    u16HorizontalDE = MDrv_Read2Byte ( L_BK_IP1F2 ( 0x15 ) ) -
                      MDrv_Read2Byte ( L_BK_IP1F2 ( 0x13 ) ) + 1;

    MDrv_WriteByte ( BK_SELECT_00, u8Bank );

    return u16HorizontalDE;
}


/******************************************************************************/
/// Get the vertical period of auto postion
/// @return the vertical period of auto postion
/******************************************************************************/
MS_U16 MDrv_Scaler_GetVerticalDEStart(void)
{
    MS_U8 u8Bank;
    MS_U16 u16VerticalDE;

    u8Bank = MDrv_ReadByte ( BK_SELECT_00 );
    MDrv_WriteByte ( BK_SELECT_00, REG_BANK_IP1F2 );

    u16VerticalDE = MDrv_Read2Byte ( L_BK_IP1F2 ( 0x12 ) );

    MDrv_WriteByte ( BK_SELECT_00, u8Bank );

    return u16VerticalDE;
}

/******************************************************************************/
/// Get the vertical period of auto postion
/// @return the vertical period of auto postion
/******************************************************************************/
MS_U16 MDrv_Scaler_GetVerticalDEEnd(void)
{
    MS_U8 u8Bank;
    MS_U16 u16VerticalDE;

    u8Bank = MDrv_ReadByte ( BK_SELECT_00 );
    MDrv_WriteByte ( BK_SELECT_00, REG_BANK_IP1F2 );

    u16VerticalDE = MDrv_Read2Byte ( L_BK_IP1F2 ( 0x14 ) );

    MDrv_WriteByte ( BK_SELECT_00, u8Bank );

    return u16VerticalDE;
}


/******************************************************************************/
/// Get the vertical period of auto postion
/// @return the vertical period of auto postion
/******************************************************************************/
MS_U16 MDrv_Scaler_GetVerticalDE(void)
{
    MS_U8 u8Bank;
    MS_U16 u16VerticalDE;

    u8Bank = MDrv_ReadByte ( BK_SELECT_00 );
    MDrv_WriteByte ( BK_SELECT_00, REG_BANK_IP1F2 );

    u16VerticalDE = MDrv_Read2Byte ( L_BK_IP1F2 ( 0x14 ) ) -
                    MDrv_Read2Byte ( L_BK_IP1F2 ( 0x12 ) ) + 1;

    // SC_PATCH_02 start ===============================
    // 2008-07-24.Daniel: Patch Interlace mode
    if( Hal_SC_ip_get_interlace_status() )
    {
        if( u16VerticalDE&1 )
            u16VerticalDE += 1;
    }
    // SC_PATCH_02 end =================================

    MDrv_WriteByte ( BK_SELECT_00, u8Bank );

    return u16VerticalDE;
}
#endif
//=========================================================//
// Function : Hal_SC_ip_de_hstart_info
// Description:
//=========================================================//
MS_U16 Hal_SC_ip_de_hstart_info(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_13_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_13_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_13_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_13_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_13_L :
                                          REG_SC_DUMMY;

    return SC_R2BYTEMSK(u32Reg, 0x0FFF);
}
//=========================================================//
// Function : Hal_SC_ip_de_hend_info
// Description:
//=========================================================//
MS_U16 Hal_SC_ip_de_hend_info(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_15_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_15_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_15_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_15_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_15_L :
                                          REG_SC_DUMMY;

    return SC_R2BYTEMSK(u32Reg, 0x0FFF);
}

//=========================================================//
// Function : Hal_SC_ip_de_vstart_info
// Description:
//=========================================================//
MS_U16 Hal_SC_ip_de_vstart_info(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_12_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_12_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_12_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_12_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_12_L :
                                          REG_SC_DUMMY;

    return SC_R2BYTEMSK(u32Reg, 0x0FFF);
}
//=========================================================//
// Function : Hal_SC_ip_de_vend_info
// Description:
//=========================================================//
MS_U16 Hal_SC_ip_de_vend_info(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_14_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_14_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_14_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_14_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_14_L :
                                          REG_SC_DUMMY;

    return SC_R2BYTEMSK(u32Reg, 0x0FFF);
}

/******************************************************************************/
///Set H position for PC mode
///@param u16Position \b IN
///- H position
/******************************************************************************/
void Hal_SC_ip_set_pc_h_position ( MS_U16 u16Position, SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_05_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_05_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_05_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_05_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_05_L :
                                          REG_SC_DUMMY;

    SC_W2BYTE(u32Reg, u16Position) ;
}


/******************************************************************************/
///Set V position for PC mode
///@param u16Position \b IN
///- V position
/******************************************************************************/
void Hal_SC_ip_set_pc_v_position ( MS_U16 u16Position, SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_04_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_04_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_04_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_04_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_04_L :
                                          REG_SC_DUMMY;

    u16Position = (u16Position==0)?1:u16Position;

    SC_W2BYTE(u32Reg, u16Position) ;
}

/******************************************************************************/
///Set Macrovision filter
///@param bEnable \b IN
///- ENABLE Filter
/******************************************************************************/
void Hal_SC_ip_set_ms_filter(MS_BOOL bEnable, MS_U16 u16FilterRange, SCALER_WIN eWindow )
{
    //Macrovision filter is not used right now
    MS_U32 u32Reg;
    UNUSED(u16FilterRange);

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_35_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_35_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_35_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_35_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_35_L :
                                          REG_SC_DUMMY;


    SC_W2BYTEMSK( u32Reg, bEnable ? BIT(12) : 0x00, BIT(12) | BIT(13) );
}

/******************************************************************************/
///Set coast window information.
///@param u8Start \b IN
///- Coast start from n HSYNC leading edge
///@param u8End \n IN
///- Coast end at n HSYNC leading edge
/******************************************************************************/
void Hal_SC_ip_set_coast_window( MS_U8 u8Start, MS_U8 u8End, SCALER_WIN eWindow )
{
    MS_U32 u32Reg_Start, u32Reg_End;

    u32Reg_Start = eWindow == MAIN_WINDOW     ? REG_SC_BK01_24_L :
                   eWindow == SUB_WINDOW      ? REG_SC_BK03_24_L :
                   eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_24_L :
                   eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_24_L :
                   eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_24_L :
                                                REG_SC_DUMMY;

    u32Reg_End = eWindow == MAIN_WINDOW     ? REG_SC_BK01_25_L :
                 eWindow == SUB_WINDOW      ? REG_SC_BK03_25_L :
                 eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_25_L :
                 eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_25_L :
                 eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_25_L :
                                                REG_SC_DUMMY;

    SC_W2BYTEMSK( u32Reg_Start, u8Start<<8, 0xFF00 );
    SC_W2BYTEMSK( u32Reg_End, u8End, 0x00FF );
}


void Hal_SC_ip_sog_detect(void)
{
   // MS_U8 u8Bank;
    MS_U8 u8SOGState;
    //u8Bank = MDrv_ReadByte(BK_SELECT_00);
    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);
    //u8SOGState = MDrv_ReadByte(L_BK_IP1F2(0x02));
    u8SOGState = SC_R2BYTE(L_BK_IP1F2(0x02))& 0x00FF;
    // check Separated -> Composite -> SOG -> Separated
    if(u8SOGState & BIT(4))
    {
        // in detecting SOG, turn SOG detect off and set back to auto-detect
        u8SOGState &= ~0x70;    // set BIT[6..4] to 0 to turn off SOG detect
        u8SOGState |= 0x00;     // switch to detect separated H/V Sync
    }
    else if((u8SOGState & 0x60) == 0x40)
    {
        // detecting separated H/V sync, switch to detect composite sync
        u8SOGState &= ~0x70;
        u8SOGState |= 0x20;     // switch to detect composite sync
    }
    else if((u8SOGState & 0x60) == 0x20)
    {
        // detecting composite sync, switch to detect SOG
        u8SOGState |= 0x70;     // set BIT[6..4] to 0x7 to turn SOG detect on
    }
    else
    {
        // detecting separated H/V sync, switch to detect composite sync
        u8SOGState &= ~0x70;
        u8SOGState |= 0x40;     // switch to detect composite sync
    }
    //MDrv_WriteByte(L_BK_IP1F2(0x02), u8SOGState);
    SC_W2BYTEMSK(REG_SC_BK01_02_L,u8SOGState,0x00ff);
    Hal_SC_ip_software_reset(REST_IP_F2, MAIN_WINDOW);
    Hal_ADC_reset(REST_ADC);
    // adjust ADC bandwidth
    if((u8SOGState & 0x70) == 0x70)
    {
        // SOG
        //MDrv_WriteRegBit(L_BK_IP1F2(0x03), DISABLE, BIT(5));  // Delay 1/4 input HSYNC
        SC_W2BYTEMSK(REG_SC_BK01_03_L, 0 ,BIT(5));
        MDrv_WriteRegBit(L_BK_ADC_ATOP(0x1C), DISABLE, BIT(5));
        MDrv_WriteByte(H_BK_ADC_ATOP(0x1C), 0x38);          // Turn on SOG input low bandwidth filter
        MDrv_WriteRegBit(L_BK_ADC_DTOP(0x07), ENABLE, BIT(6));   // ADC PLL lock source is SOG
    }
    else
    {
        // Non SOG
        //MDrv_WriteRegBit(L_BK_IP1F2(0x03), DISABLE, BIT(5));  // No delay input HSYNC
        SC_W2BYTEMSK(REG_SC_BK01_03_L, 0 ,BIT(5));
        MDrv_WriteRegBit(L_BK_ADC_ATOP(0x1C), ENABLE, BIT(5));
        MDrv_WriteByte(H_BK_ADC_ATOP(0x1C), 0x30);           // Turn off SOG input low bandwidth filter
        MDrv_WriteRegBit(L_BK_ADC_DTOP(0x07), DISABLE, BIT(6));   // ADC PLL lock source is HSync
    }
    //MDrv_WriteByte(BK_SELECT_00, u8Bank);
}

/******************************************************************************/
///get interlace detecting result
///@return MS_U8
///- 1:interlace
///- 0:no interlace
/******************************************************************************/
MS_U8 Hal_SC_ip_get_interlace_status ( SCALER_WIN eWindow )
{
    MS_BOOL bInterlace;
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_1E_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_1E_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_1E_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_1E_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_1E_L :
                                          REG_SC_DUMMY;
    bInterlace = 0;

    if ( SC_R2BYTE(u32Reg) & BIT(11) )
    {
        bInterlace = 1;
    }

    return bInterlace;
}

void Hal_SC_IPMux_Gen_SpecificTiming( XC_Internal_TimingType timingtype )
{

    switch (timingtype)
    {
        case E_XC_480P:
        case E_XC_480I:
            W2BYTE(REG_IPMUX_12_L, 0x02D0);  //HDE
            W2BYTEMSK(REG_IPMUX_13_L, 010, LBMASK);   //H front porch
            W2BYTEMSK(REG_IPMUX_14_L, 0x3E, LBMASK) ;   //H sync pulse width
            W2BYTE(REG_IPMUX_15_L, 0x05F0);  //Htotal     0x035A
            W2BYTE(REG_IPMUX_16_L, 0x01E0) ;  //VDE
            W2BYTEMSK(REG_IPMUX_17_L, 0x06, LBMASK) ;    //V front porch
            W2BYTEMSK(REG_IPMUX_18_L, 0x09, LBMASK) ;    //V sync pulse width
            W2BYTE(REG_IPMUX_19_L, 0x020d) ;  //Vtotal
            break;

        case E_XC_576P:
        case E_XC_576I:
            W2BYTE(REG_IPMUX_12_L, 0x02D0);  //HDE
            W2BYTEMSK(REG_IPMUX_13_L, 010, LBMASK);   //H front porch
            W2BYTEMSK(REG_IPMUX_14_L, 0x3E, LBMASK) ;   //H sync pulse width
            W2BYTE(REG_IPMUX_15_L, 0x035A);  //Htotal
            W2BYTE(REG_IPMUX_16_L, 0x0240) ;  //VDE
            W2BYTEMSK(REG_IPMUX_17_L, 0x06, LBMASK) ;    //V front porch
            W2BYTEMSK(REG_IPMUX_18_L, 0x09, LBMASK) ;    //V sync pulse width
            W2BYTE(REG_IPMUX_19_L, 0x0271) ;  //Vtotal
            break;

        case E_XC_720P:
        case E_XC_720I:
            W2BYTE(REG_IPMUX_12_L, 0x0500);  //HDE
            W2BYTEMSK(REG_IPMUX_13_L, 0x6E, LBMASK);   //H front porch
            W2BYTEMSK(REG_IPMUX_14_L, 0x28, LBMASK) ;   //H sync pulse width
            W2BYTE(REG_IPMUX_15_L, 0x0690);  //Htotal
            W2BYTE(REG_IPMUX_16_L, 0x02D0) ;  //VDE
            W2BYTEMSK(REG_IPMUX_17_L, 0x05, LBMASK) ;    //V front porch
            W2BYTEMSK(REG_IPMUX_18_L, 0x05, LBMASK) ;    //V sync pulse width
            W2BYTE(REG_IPMUX_19_L, 0x02EE) ;  //Vtotal
            break;

        case E_XC_1080P:
        case E_XC_1080I:

            W2BYTE(REG_IPMUX_12_L, 0x0780);  //HDE
            W2BYTEMSK(REG_IPMUX_13_L, 0x58, LBMASK);   //H front porch
            W2BYTEMSK(REG_IPMUX_14_L, 0x2C, LBMASK) ;   //H sync pulse width
            W2BYTE(REG_IPMUX_15_L, 0x0898);  //Htotal
            W2BYTE(REG_IPMUX_16_L, 0x0438) ;  //VDE
            W2BYTEMSK(REG_IPMUX_17_L, 0x02, LBMASK) ;    //V front porch
            W2BYTEMSK(REG_IPMUX_18_L, 0x05, LBMASK) ;    //V sync pulse width
            W2BYTE(REG_IPMUX_19_L, 0x0465) ;  //Vtotal

            break;
        case E_XC_OFF:
        default:
            break;
    }

    if((timingtype == E_XC_480I) || (timingtype == E_XC_576I) || (timingtype == E_XC_720I) || (timingtype == E_XC_1080I))
    {
        W2BYTEMSK(REG_IPMUX_10_L, BIT(1), BIT(1));
    }
    else
    {
        W2BYTEMSK(REG_IPMUX_10_L, 0x00, BIT(1));
    }
    if ( timingtype == E_XC_OFF)
    {
        // Turn off timing gen
        W2BYTEMSK(REG_IPMUX_10_L, 0x00, BIT(0));
    }
    else
    {
        W2BYTEMSK(REG_IPMUX_10_L, BIT(15), BIT(15));
        W2BYTEMSK(REG_IPMUX_10_L, BIT(0), BIT(0));
    }


}

MS_BOOL Hal_SC_Check_IP_Gen_Timing()
{
    return (MDrv_ReadByte(REG_IPMUX_10_L) & 0x01);
}

void Hal_SC_ip_set_input_source( MS_U8 u8InputSrcSel, MS_U8 u8SyncSel, MS_U8 u8VideoSel, MS_U8 u8isYPbPr, SCALER_WIN eWindow )
{
    MS_U16 u16RegVal;
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_02_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_02_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_02_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_02_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_02_L :
                                          REG_SC_DUMMY;


    u16RegVal = (u8InputSrcSel&0x07) |
                ((u8SyncSel&0x07)  << 4) |
                ((u8VideoSel&0x03) << 8) |
                ((u8isYPbPr&0x01)  << 10);

    SC_W2BYTEMSK( u32Reg, u16RegVal,  0x0777);
}


void hal_ip_set_input_10bit( MS_BOOL bInput10Bit, SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_03_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_03_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_03_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_03_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_03_L :
                                          REG_SC_DUMMY;


    if(bInput10Bit)
    {
        SC_W2BYTEMSK( u32Reg, BIT(11), BIT(11));
    }
    else //Input is 8bit
    {
        //when BIT(11)=0 --> BIT(10)=1, 8bit.5;  BIT(10)=0, 8bit.0
        SC_W2BYTEMSK( u32Reg, BIT(10), (BIT(11)|BIT(10)) );
    }
}


void Hal_SC_ip_set_image_wrap( MS_BOOL bHEnable, MS_BOOL bVEnable, SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_03_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_03_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_03_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_03_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_03_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK( u32Reg, ((bVEnable<<1)|bHEnable), (BIT(1)|BIT(0)) );
}


void Hal_SC_ip_set_input_sync_reference_edge( MS_BOOL bHRef, MS_BOOL bVRef, SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_03_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_03_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_03_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_03_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_03_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK( u32Reg, ((bHRef<<4)|(bVRef<<3)), (BIT(4)|BIT(3)) );
}


void Hal_SC_ip_set_input_vsync_delay( MS_BOOL bDelay, SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_03_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_03_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_03_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_03_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_03_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK( u32Reg, bDelay<<5, BIT(5) );
}


void Hal_SC_ip_set_de_only_mode( MS_BOOL bEnable, SCALER_WIN eWindow )
{
    //Turn on BIT(7) to let H/V Start being programable.
    if( eWindow == MAIN_WINDOW )
    {
        SC_W2BYTEMSK( REG_SC_BK01_03_L, (BIT(7)|(bEnable<<6)), (BIT(7)|BIT(6)) );
        MDrv_WriteRegBit(REG_DE_ONLY_F2, bEnable, DE_ONLY_F2_MASK);
    }
    else
    {
        SC_W2BYTEMSK( REG_SC_BK03_03_L, (BIT(7)|(bEnable<<6)), (BIT(7)|BIT(6)) );
        MDrv_WriteRegBit(REG_DE_ONLY_F1, bEnable, DE_ONLY_F1_MASK);
    }
}

void Hal_SC_ip_set_coast_input( MS_BOOL bInputSel, SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_24_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_24_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_24_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_24_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_24_L :
                                          REG_SC_DUMMY;

    //BIT(0): Coast Polarity to PAD  --TODO
    SC_W2BYTEMSK( u32Reg, ((bInputSel<<5)|BIT(0)), 0xFF );
 }

void Hal_SC_ip_set_DE_Mode_Glitch(MS_U8 u8Setting , SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_27_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_27_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_27_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_27_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_27_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK( u32Reg, u8Setting, 0xFF );
}

void Hal_SC_ip_set_input_sync_sample_mode( MS_BOOL bMode, SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_26_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_26_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_26_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_26_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_26_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK( u32Reg, (bMode<<2), BIT(2) );
}

void Hal_SC_ip_set_de_bypass_mode( MS_BOOL bEnable, SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_2F_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_2F_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_2F_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_2F_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_2F_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK( u32Reg, (bEnable<<15), BIT(15) );
}


MS_U16 Hal_SC_get_hsd_pixel_count( SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK02_3E_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK04_3E_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK02_3E_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK02_3E_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK04_3E_L :
                                          REG_SC_DUMMY;

    return SC_R2BYTEMSK( u32Reg, BITMASK(11 : 0) );
}

MS_U16 Hal_SC_get_vsd_line_count( SCALER_WIN eWindow )
{
    MS_U16 u16Count;
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK02_3F_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK04_3F_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK02_3F_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK02_3F_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK04_3F_L :
                                          REG_SC_DUMMY;

    u16Count = SC_R2BYTEMSK( u32Reg, BITMASK(10 : 0) );

    if(MDrv_XC_PCMonitor_Get_Vtotal(eWindow) > DOUBLEHD_1080X2P_VSIZE)
    {
        u16Count |= 0x800; //Complement for HW bits missing
    }
    return u16Count;
}

void Hal_SC_ip_set_post_glitch_removal( MS_BOOL bEnble, MS_U8 u8Range, SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_26_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_26_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_26_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_26_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_26_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK( u32Reg, ((bEnble<<7)|((u8Range&0x07)<<4)), 0xF0 );
}

//////////////////////////////////
// Capture window
void Hal_SC_ip_get_capture_window(MS_WINDOW_TYPE* capture_win,SCALER_WIN eWindow)
{
    MS_U32 u32Reg_X;
    MS_U32 u32Reg_Y;
    MS_U32 u32Reg_H;
    MS_U32 u32Reg_W;

    u32Reg_X = eWindow == MAIN_WINDOW     ? REG_SC_BK01_04_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK03_04_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_04_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_04_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_04_L :
                                            REG_SC_DUMMY;

    u32Reg_Y = eWindow == MAIN_WINDOW     ? REG_SC_BK01_05_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK03_05_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_05_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_05_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_05_L :
                                            REG_SC_DUMMY;

    u32Reg_H = eWindow == MAIN_WINDOW     ? REG_SC_BK01_06_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK03_06_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_06_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_06_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_06_L :
                                            REG_SC_DUMMY;

    u32Reg_W = eWindow == MAIN_WINDOW     ? REG_SC_BK01_07_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK03_07_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_07_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_07_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_07_L :
                                            REG_SC_DUMMY;

    capture_win->y =  SC_R2BYTE( u32Reg_X ) ;  // Vstart
    capture_win->x =  SC_R2BYTE( u32Reg_Y ) ;  // Hstart
    capture_win->height=  SC_R2BYTE( u32Reg_H ) ;  // Vsize
    capture_win->width=  SC_R2BYTE( u32Reg_W ) ;  // Hsize
}

#if 0
static void Hal_SC_ip_set_capture_window(MS_U32 u32Reg, MS_U16 u16Value , SCALER_WIN eWindow)
{
    if( eWindow == MAIN_WINDOW )
    {
        // There are 11bits for each setting of capture window
        SC_W2BYTEMSK( u32Reg , u16Value , 0x07FF );
    }
    else
    {
        MS_ASSERT(0);
    }
}
#endif

void Hal_SC_ip_set_capture_v_start(MS_U16 u16Vstart , SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_04_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_04_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_04_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_04_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_04_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg, u16Vstart, 0x7FF);
}

void Hal_SC_ip_set_capture_h_start(MS_U16 u16Hstart , SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_05_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_05_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_05_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_05_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_05_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg, u16Hstart, 0x7FF);
}

void Hal_SC_ip_set_capture_v_size(MS_U16 u16Vsize , SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_06_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_06_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_06_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_06_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_06_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg, u16Vsize, 0x1FFF);
}

void Hal_SC_ip_set_capture_h_size(MS_U16 u16Hsize , SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_07_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_07_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_07_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_07_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_07_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg, u16Hsize, 0x1FFF);
}

void Hal_SC_ip_set_fir_down_sample_divider( MS_U8 u8Enable, MS_U16 u16OverSampleCount,SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_0D_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_0D_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_0D_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_0D_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_0D_L :
                                          REG_SC_DUMMY;
    SC_W2BYTEMSK( u32Reg, (u8Enable?( BIT(7) | (u16OverSampleCount & 0xF ) ):0) , (BIT(7) | BIT(0) |BIT(1) | BIT(2) |BIT(3)) );
}

void Hal_SC_ip_set_IPAutoNoSignal(MS_BOOL bEnable, SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_1D_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_1D_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_1D_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_1D_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_1D_L :
                                          REG_SC_DUMMY;

    //printf("[comp];bEnable=%u,eWindow=%u.\n",bEnable,eWindow);
    SC_W2BYTEMSK(u32Reg, bEnable? BIT(12):0, BIT(12));
}

MS_BOOL Hal_SC_ip_get_IPAutoNoSignal( SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_1D_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_1D_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_1D_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_1D_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_1D_L :
                                          REG_SC_DUMMY;

    //printf("[comp];eWindow=%u.\n",eWindow);
    return ( SC_R2BYTEMSK(u32Reg, BIT(12) ) )? TRUE:FALSE;
}

//////////////////////////////////
// Auto gain
void Hal_SC_ip_set_auto_gain_function( MS_U8 u8Enable, SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_0E_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_0E_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_0E_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_0E_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_0E_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK( u32Reg, (u8Enable?(BIT(0) | BIT(4)):0) , (BIT(0) | BIT(4)) );
}

MS_BOOL Hal_SC_ip_is_auto_gain_result_ready(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_0E_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_0E_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_0E_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_0E_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_0E_L :
                                          REG_SC_DUMMY;

    return ( SC_R2BYTEMSK(u32Reg, BIT(1) ) )? TRUE:FALSE;
}

// bit 0  : B min
// bit 1  : G min
// bit 2  : R min
MS_U8 Hal_SC_ip_auto_gain_min_value_status(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_0E_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_0E_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_0E_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_0E_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_0E_L :
                                          REG_SC_DUMMY;

    return ( (SC_R2BYTEMSK(u32Reg, ( BIT(10) | BIT(9) | BIT(8) ) ) ) >> 8 );
}

// Make sure
// bit 0  : B max
// bit 1  : G max
// bit 2  : R max
MS_U8 Hal_SC_ip_auto_gain_max_value_status(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_0E_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_0E_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_0E_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_0E_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_0E_L :
                                          REG_SC_DUMMY;

    return ( (SC_R2BYTEMSK(u32Reg, ( BIT(5) | BIT(6) | BIT(7) ) ) ) >> 5 );
}

//////////////////////////////////
// Auto phase
MS_U32 Hal_SC_ip_get_auto_phase_value(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_1A_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_1A_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_1A_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_1A_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_1A_L :
                                          REG_SC_DUMMY;
    return ( SC_R4BYTE(u32Reg) );
}

MS_BOOL Hal_SC_ip_is_auto_phase_result_ready(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_19_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_19_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_19_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_19_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_19_L :
                                          REG_SC_DUMMY;

    return ( SC_R2BYTEMSK(u32Reg, BIT(1) ) )? TRUE:FALSE;
}

//////////////////////////////////
// Auto position
MS_BOOL Hal_SC_ip_is_auto_position_result_ready(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_10_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_10_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_10_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_10_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_10_L :
                                          REG_SC_DUMMY;

    return ( SC_R2BYTEMSK(u32Reg, BIT(1) ) )? TRUE:FALSE;
}
void Hal_SC_ip_set_auto_position_function( MS_U8 u8Enable, SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_10_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_10_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_10_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_10_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_10_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK( u32Reg, (u8Enable?BIT(0):0) , BIT(0) );
}

// Threshold is from 2'b0000 ~ 2'b1111
void Hal_SC_ip_set_valid_data_threshold( MS_U8 u8Threshold, SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_10_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_10_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_10_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_10_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_10_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK( u32Reg, (u8Threshold & 0x0F) << 12 , 0xF000 );
}

#if 0
static MS_U16 Hal_SC_ip_get_auto_position(MS_U32 u32Reg, SCALER_WIN eWindow)
{
    if( eWindow == MAIN_WINDOW )
    {
        return ( SC_R2BYTE(u32Reg) & 0xFFF );
    }
    else
    {
        MS_ASSERT(0);
        return 0;
    }
}
#endif

MS_U16 Hal_SC_ip_get_auto_position_v_start( SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_12_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_12_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_12_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_12_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_12_L :
                                          REG_SC_DUMMY;

    return (SC_R2BYTE(u32Reg) & 0xFFF);
}

MS_U16 Hal_SC_ip_get_auto_position_h_start( SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_13_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_13_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_13_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_13_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_13_L :
                                          REG_SC_DUMMY;

    return (SC_R2BYTE(u32Reg) & 0xFFF);
}

MS_U16 Hal_SC_ip_get_auto_position_v_end( SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_14_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_14_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_14_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_14_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_14_L :
                                          REG_SC_DUMMY;

    return (SC_R2BYTE(u32Reg) & 0xFFF);
}

MS_U16 Hal_SC_ip_get_auto_position_h_end( SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_15_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_15_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_15_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_15_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_15_L :
                                          REG_SC_DUMMY;

    return (SC_R2BYTE(u32Reg) & 0xFFF);
}

//////////////////////////////////
// Auto range
void Hal_SC_ip_set_auto_range_function( MS_U8 u8Enable, SCALER_WIN eWindow )
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_29_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_29_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_29_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_29_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_29_L :
                                          REG_SC_DUMMY;

    #if 0
    if (u8Enable)
    {
        // Debug mode.
        SC_W2BYTEMSK( REG_SC_BK01_60_L, 0xFF00 , 0xFF00); // Bit(15) is enable.
    }
    else
    {
        // Debug mode.
        SC_W2BYTEMSK( REG_SC_BK01_60_L, 0x0000 , 0xFF00); // Bit(15) is enable.
    }
    #endif

    SC_W2BYTEMSK( u32Reg, (u8Enable?BIT(8):0) , BIT(8) );

}

void Hal_SC_ip_set_auto_range_window(MS_U16 u16Vstart, MS_U16 u16Hstart, MS_U16 u16Vsize , MS_U16 u16Hsize, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_X;
    MS_U32 u32Reg_Y;
    MS_U32 u32Reg_H;
    MS_U32 u32Reg_W;

    u32Reg_X = eWindow == MAIN_WINDOW     ? REG_SC_BK01_2A_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK03_2A_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_2A_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_2A_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_2A_L :
                                            REG_SC_DUMMY;

    u32Reg_Y = eWindow == MAIN_WINDOW     ? REG_SC_BK01_2B_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK03_2B_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_2B_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_2B_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_2B_L :
                                            REG_SC_DUMMY;

    u32Reg_H = eWindow == MAIN_WINDOW     ? REG_SC_BK01_2C_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK03_2C_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_2C_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_2C_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_2C_L :
                                            REG_SC_DUMMY;

    u32Reg_W = eWindow == MAIN_WINDOW     ? REG_SC_BK01_2D_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK03_2D_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_2D_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_2D_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_2D_L :
                                            REG_SC_DUMMY;

    SC_W2BYTE(u32Reg_X, u16Vstart  );  //vstart
    SC_W2BYTE(u32Reg_Y, u16Hstart  );  //hstart
    SC_W2BYTE(u32Reg_H, u16Vsize  );  //vsize
    SC_W2BYTE(u32Reg_W, u16Hsize  );  //hsize

}

MS_U8 HAL_SC_ip_get_ipmux(void)
{
    return (MS_U8)MDrv_ReadByte(REG_IPMUX_01_L)&0xF0;
}
void HAL_SC_ip_set_ipmux(MS_U8 u8Val)
{
    MDrv_WriteByteMask(REG_IPMUX_01_L, u8Val,0x00F0);
}

MS_U8 HAL_SC_ip_get_ipclk(void)
{
    return (MS_U8)MDrv_ReadByte(0x1E3F)&0x3F;
}

void HAL_SC_ip_set_ipclk(MS_U8 u8Val)
{
    MDrv_WriteByteMask(0x1E3F, u8Val, 0x3F); // 1E1F[13:8]
}

MS_U8 HAL_SC_ip_get_capturesource(void)
{
    return (MS_U8)SC_R2BYTEMSK(REG_SC_BK01_02_L, 0x0007);
}

void HAL_SC_ip_set_capturesource(MS_U8 u8Val)
{
    SC_W2BYTEMSK(REG_SC_BK01_02_L, (MS_U16)u8Val, 0x0007);
}

MS_U8 Hal_SC_ip_get_user_def_interlace_status(void)
{
    return (MS_U8)SC_R2BYTEMSK(REG_SC_BK01_21_L, 0x0003);
}
void Hal_SC_ip_set_user_def_interlace_status(MS_BOOL bEnable, MS_BOOL bIsInterlace)
{
    SC_W2BYTEMSK(REG_SC_BK01_21_L, (bEnable?BIT(0):0),BIT(0));
    SC_W2BYTEMSK(REG_SC_BK01_21_L, (bIsInterlace?BIT(1):0),BIT(1));
}

MS_U8 HAL_SC_ip_get_capture_method(void)
{
    return (MS_U8)SC_R2BYTEMSK(REG_SC_BK01_03_L, 0x00FF);
}

MS_BOOL HAL_SC_ip_get_capture_format(void)
{
    MS_BOOL bIsRGBfmt;
    if(SC_R2BYTEMSK(REG_SC_BK01_02_L, 0x0008))
        bIsRGBfmt = TRUE;
    else
        bIsRGBfmt = FALSE;

    return bIsRGBfmt;
}

void HAL_SC_ip_set_capture_format(MS_BOOL bIsRGB)
{
    SC_W2BYTEMSK(REG_SC_BK01_02_L, (bIsRGB?BIT(3):0), 0x0008);
}

MS_BOOL HAL_SC_ip_get_h_predown_info(void)
{
    if(SC_R2BYTEMSK(REG_SC_BK02_05_L, 0x8000))
        return TRUE;
    else
        return FALSE;
}
MS_BOOL HAL_SC_ip_get_v_predown_info(void)
{
    if(SC_R2BYTEMSK(REG_SC_BK02_09_L, 0x8000))
        return TRUE;
    else
        return FALSE;
}
void HAL_SC_ip_set_h_predown_info(MS_BOOL bEnable)
{
    SC_W2BYTEMSK(REG_SC_BK02_05_L, (bEnable?BIT(15):0), BIT(15));
}
void HAL_SC_ip_set_v_predown_info(MS_BOOL bEnable)
{
    SC_W2BYTEMSK(REG_SC_BK02_09_L, (bEnable?BIT(15):0),BIT(15));
}

MS_BOOL HAL_SC_ip_get_444to422_filter_mod(void)
{
    if(SC_R2BYTEMSK(REG_SC_BK02_0A_L, BIT(7)))
        return TRUE;
    else
        return FALSE;
}
void HAL_SC_ip_set_444to422_filter_mod(MS_BOOL bEnable)
{
    SC_W2BYTEMSK(REG_SC_BK02_0A_L, (bEnable?BIT(7):0), BIT(7));
}

//F2 memory data format, BK12_01[10:8]
MS_U8 HAL_SC_get_memory_fmt(void)
{
    return (MS_U8)(SC_R2BYTEMSK(REG_SC_BK12_01_L, 0x0700)>>8);
}

//F2 IP memory data format, BK12_2[3:0]
MS_U8 HAL_SC_get_IPM_memory_fmt(void)
{
    return (MS_U8)(SC_R2BYTEMSK(REG_SC_BK12_02_L, 0x000F));
}
void HAL_SC_set_IPM_memory_fmt(MS_U8 u8Value)
{
    SC_W2BYTEMSK(REG_SC_BK12_02_L, (((MS_U16)u8Value)|BIT(8)),0x010F);
}

//F2 OP memory data format, BK12_2[7:4]
MS_U8 HAL_SC_get_OPM_memory_fmt(void)
{
    return (MS_U8)(SC_R2BYTEMSK(REG_SC_BK12_02_L, 0x00F0));
}
void HAL_SC_set_OPM_memory_fmt(MS_U8 u8Value)
{
    SC_W2BYTEMSK(REG_SC_BK12_02_L, (((MS_U16)u8Value)<<4|(BIT(9))),0x02F0);
}

//capture image to IP enable
void HAL_SC_set_capture_image2ip(MS_BOOL bEnable)
{
    SC_W2BYTEMSK(REG_SC_BK0F_57_L, (bEnable ? BIT(11):0), BIT(11));
}
MS_U8 HAL_SC_get_VOP_mux(void)
{
    return (MS_U8)SC_R2BYTEMSK(REG_SC_BK0F_46_L, 0x00F0);
}

void HAL_SC_set_VOP_mux(MS_U8 u8Val)
{
    SC_W2BYTEMSK(REG_SC_BK0F_46_L, (MS_U16)u8Val, 0x00F0);
}

void Hal_SC_Set_Cap_Stage(SCALER_WIN eWindow, MS_U8 u8val)
{
    MS_U32 u32Reg;
    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK0F_46_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK0F_46_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK0F_46_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK0F_46_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK0F_46_L :
                                          REG_SC_DUMMY;
    SC_W2BYTEMSK(u32Reg, (MS_U16)u8val, 0x0030);
}


MS_U8 HAL_SC_get_memory_bit_fmt(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK12_01_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK12_41_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK12_01_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK12_01_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK12_41_L :
                                          REG_SC_DUMMY;
    return (MS_U8)SC_R2BYTEMSK(u32Reg, BIT(5)|BIT(4));
}

//F2 RGB/YUV 444 8-bits format
//F2 RGB/YUV 444 10-bits format
//(bEnable_8bit : bEnable_10bit)
//(0:0)YCbCr422, (1:0)RGB/YCbCr444_8bit, (0:1)RGB/YCbCr444_10bit
void HAL_SC_set_memory_bit(MS_BOOL bEnable_8bit, MS_BOOL bEnable_10bit)
{
    SC_W2BYTEMSK(REG_SC_BK12_01_L, (MS_U16)((bEnable_10bit<<5)|bEnable_8bit<<4), (BIT(5)|BIT(4)));
}

void HAL_SC_set_IPM_capture_start(MS_BOOL bEnable)
{
    SC_W2BYTEMSK(REG_SC_BK12_02_L, (MS_U16)(bEnable?BIT(15):0), BIT(15));
}

MS_U32 HAL_SC_get_IRQ_mask_0_31(void)
{
    return (MS_U32)SC_R4BYTE(REG_SC_BK00_14_L);
}

void HAL_SC_set_IRQ_mask_0_31(MS_U32 u32Val)
{
    SC_W4BYTE(REG_SC_BK00_14_L, u32Val);
}
MS_BOOL HAL_SC_set_GOP_Enable(MS_U8 MaxGOP ,MS_U8 UseNum, MS_U8 u8MuxNum, MS_BOOL bEnable)
{
    /* GOP OP Path enable to SC Setting
        T3: GOP OP Path blending with SC sequence
        mux0-->mux2-->mux3
    */
    MS_U16 u16regval;

    if(UseNum > MaxGOP)
        return FALSE;
    u16regval = (MS_U16)SC_R2BYTE(REG_SC_BK00_0C_L);

    if(UseNum == (u8MuxNum & (BIT(0)|BIT(1))))  // Enable Mux0 to XC
    {
        SC_W2BYTEMSK(REG_SC_BK00_0C_L, bEnable?(u16regval|BIT(15)):(u16regval & (~ BIT(15))), BIT(15));
    }
    else if(UseNum == ((u8MuxNum & (BIT(4)|BIT(5)))>>4))  // Enable Mux2 to XC
    {
        SC_W2BYTEMSK(REG_SC_BK00_0C_L, bEnable?(u16regval|BIT(14)):(u16regval & (~ BIT(14))), BIT(14));
    }
    else if(UseNum == ((u8MuxNum & (BIT(6)|BIT(7)))>>6))  // Enable Mux3 to XC
    {
        SC_W2BYTEMSK(REG_SC_BK00_0C_L, bEnable?(u16regval|BIT(13)):(u16regval & (~ BIT(13))), BIT(13));
    }
    else
    {
        return FALSE;
    }
    return TRUE;
}


/********************************************************************************/
/// Set GOP destination (OP/IP) setting to scaler
/// @param ipSelGop \b IN \copydoc MS_XC_IPSEL_GOP
/********************************************************************************/
void HAL_SC_ip_sel_for_gop(MS_U8 u8MuxNum ,MS_XC_IPSEL_GOP ipSelGop)
{
    switch(ipSelGop)
    {
        default:
        case MS_IP0_SEL_GOP0:
            SC_W2BYTEMSK(REG_SC_BK00_0A_L, 0xA0, 0xA0);
            if(0 == (u8MuxNum & 0x0003))
            {
                SC_W2BYTEMSK(REG_SC_BK00_0A_L, BIT(12), BIT(12)); // mux 0
            }
            else
            {
                SC_W2BYTEMSK(REG_SC_BK00_0A_L, BIT(13), BIT(13)); // mux 1
            }
            break;
        case MS_IP0_SEL_GOP1:
            SC_W2BYTEMSK(REG_SC_BK00_0A_L, 0xA0, 0xA0);
            if(1 == (u8MuxNum & 0x0003))
            {
                SC_W2BYTEMSK(REG_SC_BK00_0A_L, BIT(12), BIT(12)); // mux 0
            }
            else
                SC_W2BYTEMSK(REG_SC_BK00_0A_L, BIT(13), BIT(13)); // mux 1
            break;
        case MS_IP0_SEL_GOP2:
            SC_W2BYTEMSK(REG_SC_BK00_0A_L, 0xA0, 0xA0);
            if(2 == (u8MuxNum & 0x0003))
            {
                SC_W2BYTEMSK(REG_SC_BK00_0A_L, BIT(12), BIT(12)); // mux 0
            }
            else
            {
                SC_W2BYTEMSK(REG_SC_BK00_0A_L, BIT(13), BIT(13)); // mux 1
            }
            break;
        case MS_NIP_SEL_GOP0:
            if(0==(u8MuxNum &0x0003))
            {
                SC_W2BYTEMSK(REG_SC_BK00_0A_L, 0, BIT(12)); // mux 0
            }
            else
            {
                SC_W2BYTEMSK(REG_SC_BK00_0A_L, 0, BIT(13)); // mux 1
            }
            break;
        case MS_NIP_SEL_GOP1:
            if(1==(u8MuxNum &0x0003))
            {
                SC_W2BYTEMSK(REG_SC_BK00_0A_L, 0, BIT(12)); // mux 0
            }
            else
            {
                SC_W2BYTEMSK(REG_SC_BK00_0A_L, 0, BIT(13)); // mux 1
            }
            break;
        case MS_NIP_SEL_GOP2:
            if(2==(u8MuxNum &0x0003))
            {
                SC_W2BYTEMSK(REG_SC_BK00_0A_L, 0, BIT(12)); // mux 0
            }
            else
            {
                SC_W2BYTEMSK(REG_SC_BK00_0A_L, 0, BIT(13)); // mux 1
            }
            break;
        case MS_MVOP_SEL:
            SC_W2BYTEMSK(REG_SC_BK00_0A_L, 0xA0, 0xA0);
            SC_W2BYTEMSK(REG_SC_BK00_0A_L, BIT(12), BIT(12)|BIT(13)); // mux 0
            break;
    }
}


/******************************************************************************/
/// Set Scaler VOP New blending level
/******************************************************************************/
void HAL_SC_SetVOPNBL(void)
{
    SC_W2BYTEMSK(REG_SC_BK00_46_L, 0x20, 0x20);
}

/******************************************************************************/
/// Set handshaking mode
/******************************************************************************/
void Hal_SC_ip_set_handshaking_md(MS_U8 u8MD, SCALER_WIN eWindow)
{
    MS_U32 u32Reg10;

    u32Reg10 = eWindow == MAIN_WINDOW     ? REG_SC_BK02_10_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK04_10_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK02_10_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK02_10_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK04_10_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg10, u8MD ? BIT(2)|BIT(0) : 0, BIT(2)|BIT(0));
}

//////////////////////////////////
// Set IP1 Test Pattern
void Hal_SC_ip_Set_TestPattern(MS_U8 u8Enable, MS_U16 u6Pattern_type, SCALER_WIN eWindow)
{
    MS_U32 u32Reg40, u32Reg42;

    u32Reg40 = eWindow == MAIN_WINDOW     ? REG_SC_BK01_40_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK03_40_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_40_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_40_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_40_L :
                                          REG_SC_DUMMY;

    u32Reg42 = eWindow == MAIN_WINDOW     ? REG_SC_BK01_42_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK03_42_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_42_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_42_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_42_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg40, u8Enable, 0xFF);
    SC_W2BYTE(u32Reg42, u6Pattern_type);
}

void HAL_SC_ip_3DMainSub_IPSync()
{
    MS_U16 value ;

    //copy bk1 to bk3 and then reset fclk
    value = SC_R2BYTE(REG_SC_BK01_08_L);
    SC_W2BYTE(REG_SC_BK03_08_L, value);

    value = SC_R2BYTE(REG_SC_BK01_09_L);
    SC_W2BYTE(REG_SC_BK03_09_L, value);

    value = SC_R2BYTE(REG_SC_BK01_21_L);
    SC_W2BYTE(REG_SC_BK03_21_L, value);

    value = SC_R2BYTE(REG_SC_BK01_24_L);
    SC_W2BYTE(REG_SC_BK03_24_L, value);

    value = SC_R2BYTE(REG_SC_BK01_25_L);
    SC_W2BYTE(REG_SC_BK03_25_L, value);

    value = SC_R2BYTE(REG_SC_BK01_26_L);
    SC_W2BYTE(REG_SC_BK03_26_L, value);

    value = SC_R2BYTE(REG_SC_BK01_2F_L);
    SC_W2BYTE(REG_SC_BK03_2F_L, value);

    SC_W2BYTEMSK(REG_SC_BK00_02_L, 1 <<6, BIT(6));
    MsOS_DelayTask(100);
    SC_W2BYTEMSK(REG_SC_BK00_02_L, 0x0,     BIT(6));
}

void HAL_SC_ip_detect_mode(MS_BOOL bEnable)
{
    //MS_U8 u8Bank;

    //u8Bank = MDrv_ReadByte(BK_SELECT_00);

    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);

    if(bEnable)
    {
        //MDrv_WriteByteMask(L_BK_IP1F2(0x29), BIT(5), BIT(5)|BIT(4));
        SC_W2BYTEMSK(REG_SC_BK01_29_L,BIT(5), BIT(5)|BIT(4));
    }
    else
    {
        //MDrv_WriteByteMask(L_BK_IP1F2(0x29), 0, BIT(5)|BIT(4));
        SC_W2BYTEMSK(REG_SC_BK01_29_L, 0 , BIT(5)|BIT(4));
    }

    //MDrv_WriteByte(BK_SELECT_00, u8Bank);
}

/******************************************************************************/
/// get the h/v sync active
/******************************************************************************/
MS_U8 Hal_SC_ip_get_h_v_sync_active(SCALER_WIN eWindow)
{
    MS_U8 u8val;

    SC_W2BYTEMSK(REG_SC_BK00_33_L, BIT(8), BIT(8));

    u8val = (SC_R2BYTE(REG_SC_BK01_1E_L) & 0xC000) >> 8;

    //SC_W2BYTEMSK(REG_SC_BK00_33_L, 0, BIT(8));

    return u8val;
}

void Hal_SC_set_skp_fd(MS_BOOL bEnable)
{
}

