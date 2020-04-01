////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (!!¡ÓMStar Confidential Information!!L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
/// @file  mhal_tvencoder.c
/// @brief TV encoder Module
/// @author MStar Semiconductor Inc.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _MHALTVENCODER_C
#define _MHALTVENCODER_C

////////////////////////////////////////////////////////////////////////////////
// Include List
////////////////////////////////////////////////////////////////////////////////
#include "ve_Analog_Reg.h"
#include "ve_hwreg_utility2.h"

#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "drvTVEncoder.h"
#include "mhal_tvencoder_tbl.h"
#include "mhal_tvencoder.h"

MS_U32 _VE_RIU_BASE;

// Put this function here because hwreg_utility2 only for hal.
void Hal_VE_init_riu_base(MS_U32 u32riu_base)
{
    _VE_RIU_BASE = u32riu_base;
}

void MsWriteVEReg(MS_U16 u16addr, MS_U16 u16Data)
{
    MDrv_Write2Byte(u16addr, u16Data);
}

void Hal_VE_WriteRegTbl ( const MS_U8 *pRegTable )
{
    MS_U32 u32Index; // register index
    MS_U16 u16Dummy;

    u16Dummy = 2000;
    do
    {
        u32Index = ((pRegTable[0] << 16) + (pRegTable[1] << 8) + pRegTable[2]);
        if (u32Index == 0xFFFFFF) // check end of table
            break;

        u32Index &= 0xFFFFFF;

        MDrv_WriteByte( u32Index, pRegTable[3] );
        pRegTable += 4;

    } while (--u16Dummy > 0);
}

/******************************************************************************/
/*                     VE Function                                            */
/* ****************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_set_color_convert()
/// @brief \b Function \b Description : Set the color convert for different
///                                     video system and DAC type
///
/// @param <IN>        \b VideoSys : The output video system
/// @param <IN>        \b u8DACType : The output DAC type
/// @param <OUT>       \b None :
/// @param <RET>       \b None :
/// @param <GLOBAL>    \b None :
////////////////////////////////////////////////////////////////////////////////
void Hal_VE_set_color_convert(MS_VE_VIDEOSYS videosys, MS_U8 u8dactype)
{

    if(videosys == MS_VE_NTSC || videosys == MS_VE_NTSC_J || videosys == MS_VE_NTSC_443)
    { // NTSC
        switch(u8dactype)
        {
        case VE_OUT_CVBS_YCC:
            break;

        case VE_OUT_CVBS_YCbCr:
            MDrv_WriteByte(L_BK_VE_ENC(0x43), 0x5A);
            MDrv_WriteByte(L_BK_VE_ENC(0x46), 0x80);
            break;

        case VE_OUT_CVBS_RGB:
            MDrv_WriteByte(L_BK_VE_ENC(0x41), 0x95);
            MDrv_WriteByte(L_BK_VE_ENC(0x42), 0x4B);
            MDrv_WriteByte(L_BK_VE_ENC(0x43), 0x90);
            MDrv_WriteByte(L_BK_VE_ENC(0x44), 0x49);
            MDrv_WriteByte(L_BK_VE_ENC(0x45), 0x32);
            MDrv_Write2Byte(L_BK_VE_ENC(0x46), 0x102);
            break;
        }
    }
    else
    { // PAL
        switch(u8dactype)
        {
        case VE_OUT_CVBS_YCC:
            break;

        case VE_OUT_CVBS_YCbCr:
            MDrv_WriteByte(L_BK_VE_ENC(0x43), 0x55);
            MDrv_WriteByte(L_BK_VE_ENC(0x46), 0x78);
            break;

        case VE_OUT_CVBS_RGB:
            MDrv_WriteByte(L_BK_VE_ENC(0x41), 0x8E);
            MDrv_WriteByte(L_BK_VE_ENC(0x42), 0x4B);
            MDrv_WriteByte(L_BK_VE_ENC(0x43), 0x88);
            MDrv_WriteByte(L_BK_VE_ENC(0x44), 0x45);
            MDrv_WriteByte(L_BK_VE_ENC(0x45), 0x2F);
            MDrv_WriteByte(L_BK_VE_ENC(0x46), 0xF3);
            break;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_SelMIU(MS_BOOL)
/// @brief \b Function \b Description : select VE read/write memory in MIU0 or MIU1
///
/// @param <IN>        \b bMIU1 : TRUE: use MIU1, FALSE: use MIU0
/// @param <OUT>       \b None :
/// @param <RET>       \b None :
/// @param <GLOBAL>    \b None :
////////////////////////////////////////////////////////////////////////////////
#define VE_MIU_GROUP1          (REG_MIU_BASE+0xF3)
void Hal_VE_SelMIU(MS_BOOL bMIU1)
{

}
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_SetMemAddr()
/// @brief \b Function \b Description : set VE read/write memory address
///
/// @param <IN>        \b pu32RDAddress : Pointer of VE Memory Read address
/// @param <IN>        \b pu32WRAddress : Pointer of VE Memory Write address
/// @param <OUT>       \b None :
/// @param <RET>       \b None :
/// @param <GLOBAL>    \b None :
////////////////////////////////////////////////////////////////////////////////
void Hal_VE_SetMemAddr(MS_U32 *pu32RDAddress, MS_U32 *pu32WRAddress)
{

}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_init()
/// @brief \b Function \b Description : Initiate VE
///
/// @param <IN>        \b u32MIUAddress : Memory address of VE
/// @param <OUT>       \b None :
/// @param <RET>       \b None :
/// @param <GLOBAL>    \b None :
////////////////////////////////////////////////////////////////////////////////
void Hal_VE_init(void)
{
    //MDrv_WriteByte(H_BK_CHIPTOP(0x19), 0x80);
#if 0
    MDrv_WriteByteMask(L_BK_CLKGEN0(0x24), 0x00, 0x0f);   // clock of ve
    MDrv_WriteByteMask(H_BK_CLKGEN0(0x24), 0x08, 0x0f);   // clock of vedac
    MDrv_WriteByteMask(H_BK_CLKGEN0(0x33), 0x20, 0x20);   // RET_CKG_HDGEN_SRC_SEL [13]
    MDrv_WriteByteMask(H_BK_CLKGEN0(0x5F), 0x0C, 0x1F);   // REG_CKG_SC_HDMIPLL
    MDrv_WriteByteMask(L_BK_CLKGEN0(0x37), 0x00, 0x1F);   // REG_CKG_DAC1
    MDrv_WriteByteMask(L_BK_VE_ENC(0x03),0x00,0x10);


    MDrv_WriteByte(L_BK_VE_ENC(0x07), 0x00);
#endif
    MDrv_WriteByteMask(L_BK_CLKGEN0(0x5F), 0x00, 0x30);
    MDrv_WriteByteMask(H_BK_VE_ENC(0x3F), 0x08, 0x08);
    MDrv_WriteByteMask(L_BK_IDAC(0x00), 0x11, 0xFF);
    MDrv_WriteByteMask(L_BK_IDAC(0x01), 0x01, 0x01);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_Get_Output_Video_Std()
/// @brief \b Function \b Description : Get the output video standard of video
///                                     encoder
///
/// @param <IN>        \b None :
/// @param <OUT>       \b None :
/// @param <RET>       \b VideoSystem : the video standard
/// @param <GLOBAL>    \b None :
////////////////////////////////////////////////////////////////////////////////
MS_VE_VIDEOSYS Hal_VE_Get_Output_Video_Std(void)
{
    if(MDrv_Read2Byte(L_BK_VE_ENC(0x06)) & BIT(0))
    {
        //n
        return MS_VE_PAL;
    }
    else
    {
        //p
        return MS_VE_NTSC;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_set_output_video_std()
/// @brief \b Function \b Description : Set the output video standard of video
///                                     encoder
///
/// @param <IN>        \b VideoSystem : the video standard
/// @param <OUT>       \b None :
/// @param <RET>       \b MS_BOOL : TRUE : supported and success,
///                                 FALSE: unsupported or unsuccess
/// @param <GLOBAL>    \b None :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL Hal_VE_set_output_video_std(MS_VE_VIDEOSYS VideoSystem)
{
    MS_BOOL bRet;
    PMS_VE_Out_VideoSYS pVideoSysTbl = NULL;

    if(VideoSystem >= MS_VE_VIDEOSYS_NUM)
    {
        bRet = FALSE;
    }
    else
    {
        bRet = TRUE;
        if(VideoSystem == MS_VE_PAL)
            pVideoSysTbl = &VE_OUT_VIDEOSTD_TBL[MS_VE_PAL];
        else
            pVideoSysTbl = &VE_OUT_VIDEOSTD_TBL[VideoSystem];

    }

    if(bRet)
    {
        //return FALSE if chip does not support selected output type
        if(pVideoSysTbl->pVE_TBL == NULL)
        {
            return FALSE;
        }

        Hal_VE_WriteRegTbl(pVideoSysTbl->pVE_TBL);
        Hal_VE_WriteRegTbl(pVideoSysTbl->pVE_Coef_TBL);
        Hal_VE_WriteRegTbl(pVideoSysTbl->pVBI_TBL);

        MDrv_WriteRegBit(L_BK_VE_ENC(0x03), pVideoSysTbl->bvtotal_525, BIT(3)); // vtotal
        MDrv_WriteRegBit(L_BK_VE_ENC(0x06), pVideoSysTbl->bPALSwitch, BIT(0));  // Palswitch
    }

    return bRet;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_set_wss_data()
/// @brief \b Function \b Description : Set the WSS data of video encoder
///
/// @param <IN>        \b bEn       : Enable, Disable
/// @param <IN>        \b u8WSSData : The WSS data
/// @param <OUT>       \b None :
/// @param <RET>       \b None :
/// @param <GLOBAL>    \b None :
////////////////////////////////////////////////////////////////////////////////
void Hal_VE_set_wss_data(MS_BOOL ben, MS_U16 u16wssdata)
{
    if(ben)
    {
        MDrv_Write2Byte(L_BK_VE_ENC(0x3B), (u16wssdata & 0x3FFF));

        MDrv_Write2Byte(L_BK_VE_ENC(0x56), 0x0017);
        MDrv_Write2Byte(L_BK_VE_ENC(0x57), 0x0017);
        MDrv_Write2Byte(L_BK_VE_ENC(0x6C), 0x0000);
        MDrv_Write2Byte(L_BK_VE_ENC(0x6D), 0x0000);

        MDrv_WriteByteMask(L_BK_VE_ENC(0x2E), BIT(7)|BIT(2), BIT(7)|BIT(2));
    }
    else
    {
        MDrv_WriteByte(L_BK_VE_ENC(0x2E), 0x00);
    }
}

MS_U16 Hal_VE_get_wss_data(void)
{
  return (MDrv_Read2Byte(L_BK_VE_ENC(0x3B)) & 0x3FFF);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_set_fix_color_out()
/// @brief \b Function \b Description : Set VE fix color out
///
/// @param <IN>        \b pInfo : the information of fix color out
/// @param <OUT>       \b None :
/// @param <RET>       \b None :
/// @param <GLOBAL>    \b None :
////////////////////////////////////////////////////////////////////////////////
void Hal_VE_set_fix_color_out(MS_BOOL ben, MS_U8 u8color_y, MS_U8 u8color_cb, MS_U8 u8color_cr)
{
}

#if 0
//------------------------------------------------------------------------------
/// Adjust contrast
/// @param  -u8Constrast \b IN: adjusting value for contrast, range:0~FFh
/// @return MS_MTSTATUS
//------------------------------------------------------------------------------

void MDrv_VE_Adjust_Contrast(MS_U8 u8Constrast)
{
    MDrv_WriteByte(L_BK_VE_ENC(0x04), u8Constrast);
}


//------------------------------------------------------------------------------
/// Adjust Hue
/// @param  -u8Hue \b IN: adjusting value for Hue, range:0~FFh
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_Adjust_Hue(MS_U8 u8Hue)
{
    MDrv_WriteByte(H_BK_VE_ENC(0x05), u8Hue);
}


//------------------------------------------------------------------------------
/// Adjust saturation
///
/// @param  -u8Saturation \b IN: adjusting value for saturation, range:0~FFh
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_Adjust_Saturation(MS_U8 u8USaturation, MS_U8 u8VSaturation)
{
    MDrv_WriteByte(L_BK_VE_ENC(0x2A), u8USaturation);
    MDrv_WriteByte(H_BK_VE_ENC(0x2A), u8VSaturation);
}


//------------------------------------------------------------------------------
/// Adjust brightness
///
/// @param  -u8brightness \b IN: adjusting value for brightness, range:0~FFh
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_Adjust_Brightness(MS_U8 u8brightness)
{
    MDrv_WriteByte(H_BK_VE_ENC(0x04), u8brightness);
}


//------------------------------------------------------------------------------
/// VE power trun on
///
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_PowerOn(void)
{
    MDrv_Power_Set_HwClock(E_HWCLK_VEIN_ONOFF, POWER_ON);
    MDrv_Power_Set_HwClock(E_HWCLK_VE_ONOFF, POWER_ON);
    MDrv_Power_Set_HwClock(E_HWCLK_VEDAC_ONOFF, POWER_ON);
    MDrv_WriteByteMask(L_BK_DAC(0x00), 0x8F, 0x8F);
}


//------------------------------------------------------------------------------
/// VE power trun off
///
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_PowerOff(void)
{
    MDrv_Power_Set_HwClock(E_HWCLK_VEIN_ONOFF, POWER_DOWN);
    MDrv_Power_Set_HwClock(E_HWCLK_VE_ONOFF, POWER_DOWN);
    MDrv_Power_Set_HwClock(E_HWCLK_VEDAC_ONOFF, POWER_DOWN);
    MDrv_WriteByteMask(L_BK_DAC(0x00), 0x00, 0x8F);
}
#endif


void Hal_VE_set_capture_window(MS_U16 u16hstart, MS_U16 u16hend, MS_U16 u16vstart, MS_U16 u16vend)
{
}

void Hal_VE_set_hsync_inverse(MS_BOOL bEn)
{
}

void Hal_VE_set_cvbs_buffer_out(MS_U16 u16chnl, MS_U16 u16mux, MS_U16 u16clamp, MS_U16 u16test)
{
    UNUSED(u16chnl);
    UNUSED(u16mux);
    UNUSED(u16clamp);
    UNUSED(u16test);
// The register below has been changed from what is was, and it may cause component output no signal(ADC39[13])
// So comment it temporarily
/*
    MS_U8 u8Adc38L = u16chnl & 0xFF;
    MS_U8 u8Adc39L = u16mux & 0xFF;
    MS_U8 u8Adc39H = (u16mux >> 8);
    MS_U8 u8Adc3AL = u16clamp & 0xFF;
    MS_U8 u8Adc3BH = u16test & 0xFF;


    MDrv_WriteByte(L_BK_ADC_ATOP(0x38), u8Adc38L);
    MDrv_WriteByte(L_BK_ADC_ATOP(0x39), u8Adc39L);
    MDrv_WriteByte(H_BK_ADC_ATOP(0x39), u8Adc39H);
    MDrv_WriteByte(L_BK_ADC_ATOP(0x3A), u8Adc3AL);
    MDrv_WriteByte(H_BK_ADC_ATOP(0x3B), u8Adc3BH);*/
}

void Hal_VE_set_frc(MS_BOOL bfulldrop, MS_U32 u32fullnum, MS_U32 u32emptynum, MS_BOOL binterlace, MS_BOOL b3FrameMode)
{

}

void Hal_VE_set_field_size(MS_U16 u16FieldSize)
{
}

void Hal_VE_set_field_inverse(MS_BOOL ben)
{

}

void Hal_VE_set_ccir656_out_pal(MS_BOOL bpal)
{
}

void Hal_VE_sofeware_reset(MS_BOOL ben)
{

}

void Hal_VE_set_vbi(MS_BOOL ben)
{

}

void Hal_VE_set_reg_load(MS_BOOL ben)
{
}

void Hal_VE_set_ctrl(MS_U16 u16enval)
{
}

void Hal_VE_set_h_scaling(MS_BOOL ben, MS_U16 u16ratio)
{

}

void Hal_VE_set_h_upscaling(MS_BOOL ben, MS_U32 u32ratio)
{
    // not supported
}

void Hal_VE_set_v_scaling(MS_BOOL ben, MS_U16 u16ratio)
{
}

void Hal_VE_set_v_upscaling(MS_BOOL ben, MS_U32 u32ratio)
{
    // not supported
}

void Hal_VE_set_v_scaling_Traditional(MS_BOOL ben, MS_U16 u16ratio)
{
    u16ratio -=7;//experience value to complement the receiver's overscan
    Hal_VE_set_v_scaling(ben, u16ratio);
}

void Hal_VE_set_out_sel(MS_U8 u8out_sel)
{
    MDrv_WriteByteMask(L_BK_VE_ENC(0x40), u8out_sel, BIT(1)|BIT(0));

}

void Hal_VE_set_source_sync_inv(MS_U8 u8val, MS_U8 u8Mask)
{
}

void Hal_VE_set_sog(MS_BOOL ben)
{
}


void Hal_VE_set_inputsource(PMS_VE_InputSrc_Info pInputSrcInfo)
{
    MS_U8 u8Clk_Mux = 0, u8Data_Mux = 0;
    switch(pInputSrcInfo->eInputSrcType)
    {
        case MS_VE_SRC_DTV:
            #if (ENABLE_VE_SUBTITLE)
            u8Data_Mux = VE_IPMUX_SC_SUBIP;
            #else
            u8Data_Mux = VE_IPMUX_MVOP;
            #endif
            u8Clk_Mux = CKG_VE_IN_CLK_MPEG0;

            break;

        case MS_VE_SRC_MAIN:
            u8Data_Mux = VE_IPMUX_SC_IP1;
            u8Clk_Mux  = CKG_VE_IN_1;
            break;

        case MS_VE_SRC_SUB:
            u8Data_Mux = VE_IPMUX_SC_SUBIP;
            u8Clk_Mux  = CKG_VE_IN_CLK_MPEG0;
            break;

        case MS_VE_SRC_DTV_FROM_MVOP:
            u8Data_Mux = VE_IPMUX_MVOP;
            u8Clk_Mux = CKG_VE_IN_CLK_MPEG0;
            break;

        case MS_VE_SRC_DSUB:
        #if (ENABLE_VE_SUBTITLE)
            u8Data_Mux = VE_IPMUX_SC_SUBIP;
            u8Clk_Mux  = CKG_VE_IN_CLK_ADC;
        #else
            u8Data_Mux = VE_IPMUX_ADC_A;
            u8Clk_Mux  = CKG_VE_IN_CLK_ADC;
        #endif
            break;

        case MS_VE_SRC_COMP:
        #if (ENABLE_VE_SUBTITLE)
            u8Data_Mux = VE_IPMUX_SC_SUBIP;
            u8Clk_Mux  = CKG_VE_IN_CLK_ADC;
        #else
            u8Data_Mux = VE_IPMUX_ADC_A;
            u8Clk_Mux  = CKG_VE_IN_CLK_ADC;
        #endif
            break;

        case MS_VE_SRC_ATV:
        case MS_VE_SRC_CVBS0:
        case MS_VE_SRC_CVBS1:
        case MS_VE_SRC_CVBS2:
        case MS_VE_SRC_CVBS3:
        case MS_VE_SRC_SVIDEO:
            u8Data_Mux = VE_IPMUX_VD;
            u8Clk_Mux  = CKG_VE_IN_CLK_VD;
            break;

        case MS_VE_SRC_HDMI_A:
        case MS_VE_SRC_HDMI_B:
        case MS_VE_SRC_HDMI_C:
        #if (ENABLE_VE_SUBTITLE)
            u8Clk_Mux  = CKG_VE_IN_1;
            u8Data_Mux = VE_IPMUX_SC_IP1;
        #else
            u8Clk_Mux  = CKG_VE_IN_CLK_DVI;
            u8Data_Mux = VE_IPMUX_HDMI_DVI;
        #endif
            break;

        case MS_VE_SRC_SCALER:
            u8Clk_Mux  = CKG_VE_IN_0_;
            u8Data_Mux = VE_IPMUX_CAPTURE;
            Hal_VE_set_rgb_in(ENABLE); // enable RGB in
            break;
        default:
            u8Clk_Mux  = 0;
            u8Data_Mux = 0;
            break;
    }
    if(pInputSrcInfo->eInputSrcType == MS_VE_SRC_SUB)
    {
        switch(pInputSrcInfo->eInputSrcOfMixedSrc)
        {
            case MS_VE_SRC_DTV:
                u8Clk_Mux = CKG_VE_IN_CLK_MPEG0;
                break;
            case MS_VE_SRC_DTV_FROM_MVOP:
                u8Clk_Mux = CKG_VE_IN_CLK_MPEG0;
                break;
            case MS_VE_SRC_DSUB:
                u8Clk_Mux  = CKG_VE_IN_CLK_ADC;
                break;
            case MS_VE_SRC_COMP:
                u8Clk_Mux  = CKG_VE_IN_CLK_ADC;
                break;
            case MS_VE_SRC_ATV:
            case MS_VE_SRC_CVBS0:
            case MS_VE_SRC_CVBS1:
            case MS_VE_SRC_CVBS2:
            case MS_VE_SRC_CVBS3:
            case MS_VE_SRC_SVIDEO:
                u8Data_Mux = VE_IPMUX_VD;
                u8Clk_Mux  = CKG_VE_IN_CLK_VD;
                break;
            case MS_VE_SRC_HDMI_A:
            case MS_VE_SRC_HDMI_B:
            case MS_VE_SRC_HDMI_C:
                u8Clk_Mux  = CKG_VE_IN_CLK_DVI;
                break;
            case MS_VE_SRC_SCALER:
                u8Clk_Mux  = CKG_VE_IN_0_;
                break;
            default:
                break;
        }
    }
    else if(pInputSrcInfo->eInputSrcType == MS_VE_SRC_MAIN)
    {
        switch(pInputSrcInfo->eInputSrcOfMixedSrc)
        {
            case MS_VE_SRC_ATV:
            case MS_VE_SRC_CVBS0:
            case MS_VE_SRC_CVBS1:
            case MS_VE_SRC_CVBS2:
            case MS_VE_SRC_CVBS3:
            case MS_VE_SRC_SVIDEO:
                u8Data_Mux = VE_IPMUX_VD;
                u8Clk_Mux  = CKG_VE_IN_CLK_VD;
                break;
            default:
                break;
        }
    }
    if(u8Data_Mux == VE_IPMUX_VD)
    {
        //Special for VD sources
    }
    Hal_VE_set_mux(u8Clk_Mux, u8Data_Mux);
}

void Hal_VE_set_mux(MS_U8 u8clk, MS_U8 u8data)
{
    MDrv_WriteByte(L_BK_IPMUX(0x02), u8data); // input select
    //MDrv_WriteByte(REG_CKG_VE_IN, u8clk); // idclk
    MDrv_WriteByte(L_BK_CLKGEN0(0x25), u8clk);  // idclk
}


void Hal_VE_set_rgb_in(MS_BOOL ben)
{
}

void Hal_VE_set_ccir656_in(MS_BOOL ben)
{
}

void Hal_VE_set_source_interlace(MS_BOOL ben)
{
}

void Hal_VE_set_clk_on_off(MS_BOOL ben)
{
    MDrv_WriteByteMask(L_BK_CLKGEN0(0x25), !ben, 0x01);   // clock of vein Ena/disable
    MDrv_WriteByteMask(L_BK_CLKGEN0(0x24), !ben, 0x01);   // clock of ve Ena/disable
    MDrv_WriteByteMask(H_BK_CLKGEN0(0x24), !ben, 0x01);   // clock of vedac Ena/disable
}

void Hal_VE_set_ve_on_off(MS_BOOL ben)
{
}

void Hal_VE_set_blackscreen(MS_BOOL ben)
{
    if(ben)
    {
        MDrv_WriteByteMask(L_BK_VE_ENC(0x03), 0x80, 0x80);
    }
    else
    {
        MDrv_WriteByteMask(L_BK_VE_ENC(0x03), 0x00, 0x80);
    }
}

MS_BOOL Hal_VE_is_blackscreen_enabled(void)
{
    MS_BOOL bRet;
    if( MDrv_ReadByte(L_BK_VE_ENC(0x03)) & 0x80)
    {
        bRet = TRUE;
    }
    else
    {
        bRet = FALSE;
    }
    return bRet;
}

//------------------------------------------------------------------------------
/// VE restart TVE to read data from TT buffer
/// @return none
//------------------------------------------------------------------------------
void Hal_VE_EnableTtx(MS_BOOL bEnable)
{

}

//------------------------------------------------------------------------------
/// VE set TT buffer address
/// @return none
//------------------------------------------------------------------------------
void Hal_VE_Set_ttx_Buffer(MS_U32 u32StartAddr, MS_U32 u32Size)
{
    MS_U32 u32VETTX_Addr,u32TTXdatasize;
    u32VETTX_Addr = u32StartAddr >> 4;
    u32TTXdatasize = (u32Size/16)-1;
    /*Drv_TVEncoder_Write(BK_VE_SRC(REG_TVE_VBI_ADDR_L), (MS_U16)u32StartAddr);
    Drv_TVEncoder_Write(BK_VE_SRC(REG_TVE_VBI_ADDR_H), u32StartAddr>>16);
    Drv_TVEncoder_Write(BK_VE_SRC(REG_TVE_VBI_MEMSIZE_L), (MS_U16)u32Size);
    Drv_TVEncoder_Write(BK_VE_SRC(REG_TVE_VBI_MEMSIZE_H), u32Size>>16);*/
    //Drv_TVEncoder_WriteBit(BK_VE_SRC(0x17), TRUE, BIT1); //stop mode
}

//------------------------------------------------------------------------------
/// VE clear TT buffer read done status
/// @return none
//------------------------------------------------------------------------------
void Hal_VE_Clear_ttxReadDoneStatus(void)
{
}

//------------------------------------------------------------------------------
/// VE TT buffer read done status
/// @return TRUE/FALSE
//------------------------------------------------------------------------------
MS_BOOL Hal_VE_Get_ttxReadDoneStatus(void)
{
    return FALSE;
}

//------------------------------------------------------------------------------
/// VE Set VBI TT active line per field
/// @return none
//------------------------------------------------------------------------------
void Hal_VE_Set_VbiTtxActiveLines(MS_U8 u8LinePerField)
{
    MS_U8 i;

    MDrv_Write2Byte(L_BK_VE_ENC(0x3c),0xffff);
    //MDrv_Write2Byte(L_BK_VE_ENC(0x3d),0xffff);
    for(i=u8LinePerField;i<16;i++)
    {
        MDrv_Write2Byte(L_BK_VE_ENC(0x3c),MDrv_Read2Byte(L_BK_VE_ENC(0x3c))<<1);
    }

    if(u8LinePerField>16)
    {
        MDrv_Write2Byte(L_BK_VE_ENC(0x3d), 0x01);
    }
    else
    {
        MDrv_Write2Byte(L_BK_VE_ENC(0x3d), 0x00);
    }

}

//------------------------------------------------------------------------------
/// VE Set VBI ttx active line
/// @param <IN>\b odd_start: odd page start line
/// @param <IN>\b odd_end: odd page end line
/// @param <IN>\b even_start: even page start line
/// @param <IN>\b even_end: even page end line
/// @return none
//------------------------------------------------------------------------------
void Hal_VE_SetVbiTtxRange(MS_U16 odd_start, MS_U16 odd_end,
                         MS_U16 even_start, MS_U16 enen_end)
{
    // not implemented
}

//------------------------------------------------------------------------------
/// VE Set VBI TT active line by the given bitmap
/// @param <IN>\b u32Bitmap: a bitmap that defines whick physical lines the teletext lines are to be inserted.
/// @return none
//------------------------------------------------------------------------------
void Hal_VE_Set_VbiTtxActiveLinesBitmap(MS_U32 u32Bitmap)
{
    // not implemented
}

//------------------------------------------------------------------------------
/// VE Set VE display window offset
/// @return none
//------------------------------------------------------------------------------
void Hal_VE_Set_winodw_offset(MS_U32 u32offsetaddr)
{
    Hal_VE_SetMemAddr(NULL, &u32offsetaddr);
}

//------------------------------------------------------------------------------
/// VE Set VE output with OSD
/// @return none
//------------------------------------------------------------------------------
void Hal_VE_Set_OSD(MS_BOOL bEnable)
{
}

//------------------------------------------------------------------------------
/// VE Set VBI CC
/// @return none
//------------------------------------------------------------------------------
void Hal_VE_EnableCcSw(MS_BOOL bEnable)
{
    if (bEnable)
    {
        MDrv_Write2Byte(L_BK_VE_ENC(0x2E), (MDrv_Read2Byte(L_BK_VE_ENC(0x2E))|0x0081));
    }
    else
    {
        MDrv_Write2Byte(L_BK_VE_ENC(0x2E), (MDrv_Read2Byte(L_BK_VE_ENC(0x2E))&0xfffe));
    }
}

//------------------------------------------------------------------------------
/// VE Set VBI CC active line
/// @return none
//------------------------------------------------------------------------------
void Hal_VE_SetCcRange(MS_U16 odd_start, MS_U16 odd_end, MS_U16 even_start, MS_U16 enen_end)
{
    MDrv_Write2Byte(L_BK_VE_ENC(0x4E), odd_start);
    MDrv_Write2Byte(L_BK_VE_ENC(0x4F), odd_end);
    MDrv_Write2Byte(L_BK_VE_ENC(0x50), even_start);
    MDrv_Write2Byte(L_BK_VE_ENC(0x51), enen_end);
}

//------------------------------------------------------------------------------
/// VE Set VBI CC data
/// @return none
//------------------------------------------------------------------------------
void Hal_VE_SendCcData(MS_BOOL bIsOdd, MS_U16 data)    // true: odd, false: even
{
    if (bIsOdd)
    {
        MDrv_Write2Byte(L_BK_VE_ENC(0x2B), data);
    }
    else
    {
        MDrv_Write2Byte(L_BK_VE_ENC(0x2C), data);
    }
}

//------------------------------------------------------------------------------
/// VE Enable Test pattern
/// @return none
//------------------------------------------------------------------------------
void Hal_VE_Set_TestPattern(MS_BOOL ben)
{
}

//------------------------------------------------------------------------------
/// VE write register
/// @return none
//------------------------------------------------------------------------------
void Hal_VE_W2BYTE_MSK(MS_U32 u32Reg, MS_U16 u16Val, MS_U16 u16Mask)
{
   W2BYTEMSK(u32Reg, u16Val, u16Mask);
}

//------------------------------------------------------------------------------
/// VE write register
/// @return MS_U16 register value
//------------------------------------------------------------------------------
MS_U16 Hal_VE_R2BYTE_MSK(MS_U32 u32Reg, MS_U16 u16Mask)
{
    return R2BYTEMSK(u32Reg, u16Mask);
}

void Hal_VE_DumpTable(MS_U8 *pVeTable, MS_U8 u8TableType)
{
    //do nothing on this chip
}

VE_Result Hal_VE_SetMV(MS_BOOL bEnable, MS_VE_MV_TYPE eMvType)
{
    //do nothing on this chip
    return E_VE_NOT_SUPPORT;
}

//for Hal_VE_DisableRegWrite_GetCaps
#define DISABLE_REG_WRITE_CAP  FALSE;

MS_BOOL Hal_VE_DisableRegWrite_GetCaps(void)
{
    return DISABLE_REG_WRITE_CAP;
}

// show internal color bar
// TRUE: enable color bar
// FALSE: disable color bar
void Hal_VE_ShowColorBar(MS_BOOL bEnable)
{
    // not implemented
}

VE_Result Hal_VE_AdjustPositionBase(MS_S32 s32WAddrAdjustment, MS_S32 s32RAddrAdjustment)
{
    //do nothing on this chip
    return E_VE_NOT_SUPPORT;
}

VE_Result HAL_VE_SetFrameLock(MS_U32 u32IDclk, MS_U32 u32ODclk, MS_U32 u32InitPll, MS_BOOL bEnable)
{
    //do nothing on this chip
    return E_VE_NOT_SUPPORT;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_GetCaps()
/// @brief \b Function \b Description : return chip capability
///
/// @param <IN>        \b pointer to MS_VE_Cap structure
////////////////////////////////////////////////////////////////////////////////
VE_Result Hal_VE_GetCaps(MS_VE_Cap *cap)
{
    cap->bSupport_UpScale = FALSE;
    cap->bSupport_CropMode = FALSE;

    return E_VE_OK;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_Get_DRAM_Format()
/// @brief \b Function \b Description : return DRAM format
///
/// @param <IN>        \b none
/// @return one of MS_VE_DRAM_FORMET
////////////////////////////////////////////////////////////////////////////////
MS_VE_DRAM_FORMAT Hal_VE_Get_DRAM_Format(void)
{
    // not supported
    return MS_VE_DRAM_FORMAT_Y8C8;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_Set_DRAM_Format()
/// @brief \b Function \b Description : set DRAM format
///
/// @param <IN>        \b one of MS_VE_DRAM_FORMAT
/// @return none
////////////////////////////////////////////////////////////////////////////////
void Hal_VE_Set_DRAM_Format(MS_VE_DRAM_FORMAT fmt)
{
    // not supported
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_Get_Pack_Num_Per_Line()
/// @brief \b Function \b Description : return DRAM Pack number per line
///
/// @param <IN>        \b none
/// @return pack num per line
////////////////////////////////////////////////////////////////////////////////
MS_U8 Hal_VE_Get_Pack_Num_Per_Line(void)
{
    // not supported
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_Set_Pack_Num_Per_Line()
/// @brief \b Function \b Description : Set Pack number per line
///
/// @param <IN>        \b none
/// @return one of MS_VE_DRAM_FORMET
////////////////////////////////////////////////////////////////////////////////
void Hal_VE_Set_Pack_Num_Per_Line(MS_U8 PackNumPerLine)
{
    // not supported
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_Get_Field_Line_Number()
/// @brief \b Function \b Description : return field line number
///
/// @param <IN>        \b none
/// @return field line number
////////////////////////////////////////////////////////////////////////////////
MS_U16 Hal_VE_Get_Field_Line_Number(void)
{
    // not supported
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_Get_Field_Line_Number()
/// @brief \b Function \b Description : return field line number
///
/// @param <IN>        \b none
/// @return field line number
////////////////////////////////////////////////////////////////////////////////
void Hal_VE_Set_Field_Line_Number(MS_U16 FieldLineNumber)
{
    // not supported
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_Set_Crop_OSD_Offest()
/// @brief \b Function \b Description : setup crop for osd offset
///
/// @param <IN>        \b hstart
/// @param <IN>        \b vstart
/// @return none
////////////////////////////////////////////////////////////////////////////////
void Hal_VE_Set_Crop_OSD_Offset(MS_U16 hstart, MS_U16 vstart)
{
    // not supported
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_Set_VScale_Output_Line_Number()
/// @brief \b Function \b Description : Set VE vertical output line number in scale mode
///
/// @param <IN>        \b line
/// @return none
////////////////////////////////////////////////////////////////////////////////
void Hal_VE_Set_VScale_Output_Line_Number(MS_U16 line)
{
    // not supported
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_Set_HScale_Output_Line_Number()
/// @brief \b Function \b Description : Set VE Horizontal output line number in scale mode
///
/// @param <IN>        \b line
/// @return none
////////////////////////////////////////////////////////////////////////////////
void Hal_VE_Set_HScale_Output_Line_Number(MS_U16 line)
{
    // not supported
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: Hal_VE_Adjust_FrameStart()
/// @brief \b Function \b Description : the frame start is used to adjust output video
///                                     (in pixel)
///
/// @param <IN>        \b pixel_offset
/// @return VE_Result
////////////////////////////////////////////////////////////////////////////////
VE_Result Hal_VE_Adjust_FrameStart(MS_S16 pixel_offset)
{
    // not supported
    return E_VE_NOT_SUPPORT;
}

#endif
