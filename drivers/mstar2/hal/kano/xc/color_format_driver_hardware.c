///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2008 - 2009 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    color_format_driver.c
/// @brief  MStar XC Driver DDI HAL Level
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_COLOR_FORMAT_DRIVER_HARDWARE_C
#define _HAL_COLOR_FORMAT_DRIVER_HARDWARE_C
#endif

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include "mdrv_mstypes.h"
#include "color_format_input.h"
#include "color_format_driver.h"
#include "color_format_driver_hardware.h"
#include "color_format_ip.h"
#include "mhal_dlc.h"
#include "mhal_xc.h"

#if (1 == CFD_debug)
#else
#define printf
#endif

#define function1(var1,var2) var1 = (var1&MaskForMode_HB) | (var2&MaskForMode_LB);

extern MS_U16 MS_Cfd_TMO_parameter_Check_Update(StuDlc_HDRinit *pg_HDRinitParameters);


MS_U32 Maserati22Gamma[513] = {
0x0, 0x3, 0x4, 0x5, 0x5, 0x6, 0x6, 0x7, 0x7, 0x7, 0x8, 0x8, 0x8, 0x9, 0x9, 0x9,
0xa, 0xa, 0xa, 0xa, 0xb, 0xb, 0xb, 0xb, 0xc, 0xc, 0xc, 0xc, 0xc, 0xd, 0xd, 0xd,
0xd, 0xe, 0xe, 0xe, 0xf, 0xf, 0xf, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x12, 0x12,
0x12, 0x13, 0x13, 0x14, 0x14, 0x15, 0x15, 0x15, 0x16, 0x16, 0x17, 0x17, 0x17, 0x18, 0x18, 0x19,
0x19, 0x1a, 0x1a, 0x1b, 0x1c, 0x1c, 0x1d, 0x1d, 0x1e, 0x1e, 0x1f, 0x20, 0x20, 0x21, 0x21, 0x22,
0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2b, 0x2c, 0x2d, 0x2d, 0x2e,
0x2f, 0x30, 0x31, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
0x40, 0x42, 0x44, 0x45, 0x47, 0x48, 0x4a, 0x4c, 0x4d, 0x4e, 0x50, 0x51, 0x53, 0x54, 0x55, 0x56,
0x58, 0x5a, 0x5d, 0x5f, 0x61, 0x63, 0x65, 0x67, 0x69, 0x6b, 0x6d, 0x6f, 0x71, 0x73, 0x75, 0x77,
0x78, 0x7c, 0x7f, 0x82, 0x85, 0x88, 0x8b, 0x8e, 0x91, 0x93, 0x96, 0x99, 0x9b, 0x9e, 0xa0, 0xa2,
0xa5, 0xa9, 0xae, 0xb2, 0xb6, 0xba, 0xbe, 0xc2, 0xc6, 0xca, 0xcd, 0xd1, 0xd4, 0xd8, 0xdb, 0xdf,
0xe2, 0xe8, 0xee, 0xf4, 0xfa, 0xff, 0x105, 0x10a, 0x10f, 0x115, 0x11a, 0x11e, 0x123, 0x128, 0x12c, 0x131,
0x135, 0x13e, 0x146, 0x14f, 0x156, 0x15e, 0x166, 0x16d, 0x174, 0x17b, 0x182, 0x188, 0x18f, 0x195, 0x19c, 0x1a2,
0x1a8, 0x1b4, 0x1bf, 0x1ca, 0x1d5, 0x1e0, 0x1ea, 0x1f4, 0x1fe, 0x207, 0x211, 0x21a, 0x223, 0x22c, 0x234, 0x23d,
0x245, 0x255, 0x265, 0x274, 0x283, 0x291, 0x2a0, 0x2ad, 0x2bb, 0x2c8, 0x2d4, 0x2e1, 0x2ed, 0x2f9, 0x305, 0x311,
0x31c, 0x332, 0x348, 0x35d, 0x371, 0x385, 0x398, 0x3ab, 0x3bd, 0x3cf, 0x3e1, 0x3f2, 0x403, 0x413, 0x424, 0x433,
0x443, 0x462, 0x47f, 0x49c, 0x4b8, 0x4d3, 0x4ed, 0x507, 0x520, 0x538, 0x551, 0x568, 0x57f, 0x596, 0x5ac, 0x5c2,
0x5d7, 0x601, 0x629, 0x651, 0x677, 0x69c, 0x6c0, 0x6e3, 0x706, 0x727, 0x748, 0x769, 0x788, 0x7a7, 0x7c6, 0x7e4,
0x801, 0x83a, 0x872, 0x8a7, 0x8dc, 0x90e, 0x940, 0x970, 0x9a0, 0x9ce, 0x9fb, 0xa27, 0xa52, 0xa7d, 0xaa7, 0xacf,
0xaf8, 0xb46, 0xb92, 0xbdc, 0xc23, 0xc69, 0xcad, 0xcef, 0xd30, 0xd6f, 0xdad, 0xdea, 0xe25, 0xe5f, 0xe98, 0xed0,
0xf08, 0xf73, 0xfdb, 0x1040, 0x10a2, 0x1102, 0x115f, 0x11ba, 0x1212, 0x1269, 0x12be, 0x1311, 0x1362, 0x13b2, 0x1400, 0x144d,
0x1499, 0x152c, 0x15ba, 0x1645, 0x16cb, 0x174e, 0x17ce, 0x184a, 0x18c4, 0x193a, 0x19ae, 0x1a20, 0x1a90, 0x1afd, 0x1b68, 0x1bd2,
0x1c39, 0x1d03, 0x1dc7, 0x1e84, 0x1f3d, 0x1ff0, 0x209f, 0x2149, 0x21ef, 0x2292, 0x2331, 0x23cd, 0x2466, 0x24fc, 0x258f, 0x261f,
0x26ad, 0x27c2, 0x28ce, 0x29d1, 0x2ace, 0x2bc4, 0x2cb3, 0x2d9d, 0x2e81, 0x2f60, 0x303a, 0x3110, 0x31e1, 0x32ae, 0x3378, 0x343e,
0x3500, 0x367b, 0x37ea, 0x394e, 0x3aa8, 0x3bf9, 0x3d41, 0x3e81, 0x3fba, 0x40eb, 0x4216, 0x433b, 0x445a, 0x4573, 0x4687, 0x4796,
0x48a1, 0x4aa8, 0x4c9f, 0x4e87, 0x5062, 0x522f, 0x53f1, 0x55a7, 0x5754, 0x58f6, 0x5a90, 0x5c21, 0x5daa, 0x5f2c, 0x60a6, 0x621a,
0x6387, 0x64ee, 0x664f, 0x67aa, 0x6900, 0x6a51, 0x6b9d, 0x6ce4, 0x6e27, 0x6f65, 0x709f, 0x71d5, 0x7307, 0x7435, 0x7560, 0x7687,
0x77ab, 0x78cc, 0x79e9, 0x7b03, 0x7c1a, 0x7d2e, 0x7e40, 0x7f4f, 0x805b, 0x8164, 0x826b, 0x836f, 0x8471, 0x8571, 0x866f, 0x876a,
0x8863, 0x8a4f, 0x8c32, 0x8e0e, 0x8fe3, 0x91b1, 0x9377, 0x9538, 0x96f2, 0x98a6, 0x9a55, 0x9bfd, 0x9da1, 0x9f3f, 0xa0d8, 0xa26d,
0xa3fd, 0xa588, 0xa70f, 0xa892, 0xaa10, 0xab8b, 0xad02, 0xae75, 0xafe4, 0xb150, 0xb2b8, 0xb41d, 0xb57e, 0xb6dd, 0xb838, 0xb990,
0xbae5, 0xbd87, 0xc01e, 0xc2ab, 0xc52d, 0xc7a5, 0xca15, 0xcc7b, 0xced9, 0xd12f, 0xd37d, 0xd5c3, 0xd802, 0xda39, 0xdc6a, 0xde95,
0xe0b8, 0xe2d6, 0xe4ee, 0xe700, 0xe90c, 0xeb13, 0xed14, 0xef11, 0xf108, 0xf2fa, 0xf4e8, 0xf6d1, 0xf8b6, 0xfa96, 0xfc72, 0xfe49,
0x1001d, };

#if RealChip

extern MS_U8 u8HLGUseLUT_Flag;
extern StuDlc_HDRinit g_HDRinitParameters;
extern StuDlc_FinetuneParamaters g_DlcParameters;

extern MS_U8 u8Mhal_xc_TMO_mode;

#endif

//switch handshake/DE mode in HW
extern void PathSwitch(MS_BOOL bEnable);


void MS_Cfd_Kano_HDRIP_Debug(STU_CFDAPI_Kano_TMOIP *pstu_TMO_Param, STU_CFDAPI_Kano_HDRIP *pstu_HDRIP_Param)
{
    printf("u8TMO_curve_enable_Mode                 :0x%02x\n",pstu_TMO_Param->u8HDR_TMO_curve_enable_Mode);
    printf("u8TMO_curve_Mode                        :0x%02x\n",pstu_TMO_Param->u8HDR_TMO_curve_Mode );
    printf("u8HDR_UVC_Mode                          :0x%02x\n",pstu_TMO_Param->u8HDR_UVC_Mode );
    printf("\n");
    printf("u8HDR_IP_enable_Mode                    :0x%02x\n",pstu_HDRIP_Param->u8HDR_IP_enable_Mode);
    printf("u8HDR_IP_PATH_Mode                      :0x%02x\n",pstu_HDRIP_Param->u8HDR_IP_PATH_Mode);
    printf("u8HDR_RGB3D_enable_Mode                 :0x%02x\n",pstu_HDRIP_Param->u8HDR_RGB3D_enable_Mode);
    printf("u8HDR_RGB3D_PATH_Mode                   :0x%02x\n",pstu_HDRIP_Param->u8HDR_RGB3D_PATH_Mode);
    printf("u8HDR_RGB3D_Mode                        :0x%02x\n",pstu_HDRIP_Param->u8HDR_RGB3D_Mode);
    printf("\n");
    printf("u8HDR_YCGAINOFFSET_In_Mode              :0x%02x\n",pstu_HDRIP_Param->u8HDR_YCGAINOFFSET_In_Mode);
    printf("u8HDR_YCGAINOFFSET_Out_Mode             :0x%02x\n",pstu_HDRIP_Param->u8HDR_YCGAINOFFSET_Out_Mode);
    printf("\n");
    printf("u8HDR_InputCSC_Mode                     :0x%02x\n",pstu_HDRIP_Param->u8HDR_InputCSC_Mode);
    printf("u8HDR_InputCSC_Ratio1                   :0x%02x\n",pstu_HDRIP_Param->u8HDR_InputCSC_Ratio1);
    printf("u8HDR_InputCSC_Manual_Vars_en           :0x%02x\n",pstu_HDRIP_Param->u8HDR_InputCSC_Manual_Vars_en);
    printf("u8HDR_InputCSC_MC                       :0x%02x\n",pstu_HDRIP_Param->u8HDR_InputCSC_MC);
    printf("\n");
    printf("u8HDR_RGBOffset_Mode                    :0x%02x\n",pstu_HDRIP_Param->u8HDR_RGBOffset_Mode);
    printf("u8HDR_RGBClip_Mode                      :0x%02x\n",pstu_HDRIP_Param->u8HDR_RGBClip_Mode);
    printf("u8HDR_LinearRGBBypass_Mode              :0x%02x\n",pstu_HDRIP_Param->u8HDR_LinearRGBBypass_Mode);
    printf("\n");
    printf("u8HDR_Degamma_enable_Mode               :0x%02x\n",pstu_HDRIP_Param->u8HDR_Degamma_enable_Mode);
    printf("u8HDR_Degamma_Dither_Mode               :0x%02x\n",pstu_HDRIP_Param->u8HDR_Degamma_Dither_Mode);
    printf("u8HDR_Degamma_SRAM_Mode                 :0x%02x\n",pstu_HDRIP_Param->u8HDR_Degamma_SRAM_Mode);
    printf("\n");
    printf("u8HDR_Degamma_Ratio1                    :0x%02x\n",pstu_HDRIP_Param->u8HDR_Degamma_Ratio1);
    printf("u16HDR_Degamma_Ratio2                   :0x%04x\n",pstu_HDRIP_Param->u16HDR_Degamma_Ratio2);
    printf("u8HDR_DeGamma_Manual_Vars_en            :0x%02x\n",pstu_HDRIP_Param->u8HDR_DeGamma_Manual_Vars_en);
    printf("u8HDR_Degamma_TR                        :0x%02x\n",pstu_HDRIP_Param->u8HDR_Degamma_TR);
    printf("u8HDR_Degamma_Lut_En                    :0x%02x\n",pstu_HDRIP_Param->u8HDR_Degamma_Lut_En);
    printf("pu32HDR_Degamma_Lut_Address             :0x%08x\n",pstu_HDRIP_Param->pu32HDR_Degamma_Lut_Address);
    printf("u16HDR_Degamma_Lut_Length               :0x%04x\n",pstu_HDRIP_Param->u16HDR_Degamma_Lut_Length);
    printf("u8DHDR_Degamma_Max_Lum_En               :0x%02x\n",pstu_HDRIP_Param->u8HDR_Degamma_Max_Lum_En);
    printf("u16HDR_Degamma_Max_Lum                  :0x%04x\n",pstu_HDRIP_Param->u16HDR_Degamma_Max_Lum);
    printf("\n");
    printf("u8HDR_3x3_enable_Mode                   :0x%02x\n",pstu_HDRIP_Param->u8HDR_3x3_enable_Mode);
    printf("u8HDR_3x3_Mode                          :0x%02x\n",pstu_HDRIP_Param->u8HDR_3x3_Mode);
    printf("u16HDR_3x3_Ratio2                       :0x%02x\n",pstu_HDRIP_Param->u16HDR_3x3_Ratio2);
    printf("u8HDR_3x3_Manual_Vars_en                :0x%02x\n",pstu_HDRIP_Param->u8HDR_3x3_Manual_Vars_en);
    printf("u8HDR_3x3_InputCP                       :0x%02x\n",pstu_HDRIP_Param->u8HDR_3x3_InputCP);
    printf("u8HDR_3x3_OutputCP                      :0x%02x\n",pstu_HDRIP_Param->u8HDR_3x3_OutputCP);
    printf("\n");
    printf("u8HDR_Compress_settings_Mode            :0x%02x\n",pstu_HDRIP_Param->u8HDR_Compress_settings_Mode);
    printf("u8HDR_Compress_dither_Mode              :0x%02x\n",pstu_HDRIP_Param->u8HDR_Compress_dither_Mode);
    printf("u8HDR_3x3Clip_Mode                      :0x%02x\n",pstu_HDRIP_Param->u8HDR_3x3Clip_Mode);
    printf("\n");
    printf("u8HDR_Gamma_enable_Mode                 :0x%02x\n",pstu_HDRIP_Param->u8HDR_Gamma_enable_Mode);
    printf("u8HDR_Gamma_Dither_Mode                 :0x%02x\n",pstu_HDRIP_Param->u8HDR_Gamma_Dither_Mode);
    printf("u8HDR_Gamma_maxdata_Mode                :0x%02x\n",pstu_HDRIP_Param->u8HDR_Gamma_maxdata_Mode);
    printf("u8HDR_Gamma_SRAM_Mode                   :0x%02x\n",pstu_HDRIP_Param->u8HDR_Gamma_SRAM_Mode);
    printf("\n");
    printf("u8HDR_Gamma_Manual_Vars_en              :0x%02x\n",pstu_HDRIP_Param->u8HDR_Gamma_Manual_Vars_en);
    printf("u8HDR_Gamma_TR                          :0x%02x\n",pstu_HDRIP_Param->u8HDR_Gamma_TR);
    printf("u8HDR_Gamma_Lut_En                      :0x%02x\n",pstu_HDRIP_Param->u8HDR_Gamma_Lut_En);
    printf("pu32HDR_Gamma_Lut_Address               :0x%08x\n",pstu_HDRIP_Param->pu32HDR_Gamma_Lut_Address);
    printf("u16HDR_Gamma_Lut_Length                 :0x%04x\n",pstu_HDRIP_Param->u16HDR_Gamma_Lut_Length);
    printf("\n");
    printf("u8HDR_OutputCSC_Mode                    :0x%02x\n",pstu_HDRIP_Param->u8HDR_OutCSC_Mode);
    printf("u8HDR_OutputCSC_Ratio1                  :0x%02x\n",pstu_HDRIP_Param->u8HDR_OutCSC_Ratio1);
    printf("u8HDR_OutputCSC_Manual_Vars_en          :0x%02x\n",pstu_HDRIP_Param->u8HDR_OutCSC_Manual_Vars_en);
    printf("u8HDR_OutputCSC_MC                      :0x%02x\n",pstu_HDRIP_Param->u8HDR_OutCSC_MC);
    printf("\n");
    printf("\n");
}

void MS_Cfd_Kano_SDRIP_Debug(STU_CFDAPI_Kano_DLCIP *pstu_DLC_Param, STU_CFDAPI_Kano_SDRIP *pstu_SDRIP_Param)
{
    printf("\n");
    printf("\n");
    printf("u8SDR_IP2_CSC_Mode                       :0x%02x\n",pstu_SDRIP_Param->u8SDR_IP2_CSC_Mode);
    printf("u8SDR_IP2_CSC_Ratio1                     :0x%02x\n",pstu_SDRIP_Param->u8SDR_IP2_CSC_Ratio1);
    printf("u8SDR_IP2_CSC_Manual_Vars_en             :0x%02x\n",pstu_SDRIP_Param->u8SDR_IP2_CSC_Manual_Vars_en);
    printf("u8SDR_IP2_CSC_MC                         :0x%02x\n",pstu_SDRIP_Param->u8SDR_IP2_CSC_MC);
    printf("\n");
    printf("u8SDR_VIP_CM_Mode                        :0x%02x\n",pstu_SDRIP_Param->u8SDR_VIP_CM_Mode);
    printf("u8SDR_VIP_CM_Ratio1                      :0x%02x\n",pstu_SDRIP_Param->u8SDR_VIP_CM_Ratio1);
    printf("u8SDR_VIP_CM_Manual_Vars_en              :0x%02x\n",pstu_SDRIP_Param->u8SDR_VIP_CM_Manual_Vars_en);
    printf("u8SDR_VIP_CM_MC                          :0x%02x\n",pstu_SDRIP_Param->u8SDR_VIP_CM_MC);
    printf("\n");
    printf("u8SDR_Conv420_CM_Mode                    :0x%02x\n",pstu_SDRIP_Param->u8SDR_Conv420_CM_Mode);
    printf("u8SDR_Conv420_CM_Ratio1                  :0x%02x\n",pstu_SDRIP_Param->u8SDR_Conv420_CM_Ratio1);
    printf("u8SDR_Conv420_CM_Manual_Vars_en          :0x%02x\n",pstu_SDRIP_Param->u8SDR_Conv420_CM_Manual_Vars_en);
    printf("u8SDR_Conv420_CM_MC                      :0x%02x\n",pstu_SDRIP_Param->u8SDR_Conv420_CM_MC);
    printf("\n");
    printf("\n");
}

//start of Kano IP write register function =========================================================
MS_U8 Kano_HDRIP_DeGamma_enable(MS_BOOL bMode, MS_U8 u8IpType)
{

    write2ByteMask(0x102F00, 0xFFFF, 0x0040,E_IP);

    if(E_MAIN_WINDOW == u8IpType)
    {
        //write2ByteMask(0x102F00, 0xFFFF, 0x0040 ,E_IP);

        if(E_OFF == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x01),0x0001,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x01),0x0001,0x0001,E_IP);
        }
        else
            return 0;
    }
#if 0
    else if(E_SUB_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x11),0x0001,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x11),0x0001,0x0001,E_IP);
        }
        else
            return 0;
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}

MS_U8 Kano_HDRIP_Gamma_enable(MS_BOOL bMode, MS_U8 u8IpType)
{

    write2ByteMask(0x102F00, 0xFFFF, 0x0040,E_IP);

    if(E_MAIN_WINDOW == u8IpType)
    {
    if(E_OFF == bMode)
    {
        write2ByteMask(_PK_L_kano(0x40,0x01),0x0004,0x0000,E_IP);
    }
    else if(E_ON == bMode)
    {
        write2ByteMask(_PK_L_kano(0x40,0x01),0x0004,0x0004,E_IP);
    }
    else
        return 0;
    }
#if 0
    else if(E_SUB_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x01),0x0004,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x01),0x0004,0x0004,E_IP);
        }
        else
            return 0;
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}

MS_U8 Kano_HDRIP_HDRIP_enable(MS_BOOL bMode, MS_U8 u8IpType)
{

    write2ByteMask(0x102F00, 0xFFFF, 0x0040,E_IP);

    if(E_MAIN_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x7F),0x0001,0x0000,E_IP);
         PathSwitch(bMode);
         //printf("\033[1;35m###[Brian][%s][%d]### off !!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__);
         //msWriteByteMask(0x102F00,0x40,0xFF);
         //msWriteByteMask(0x102FFE,0x00,0x01);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x7F),0x0001,0x0001,E_IP);
         PathSwitch(bMode);
         //printf("\033[1;35m###[Brian][%s][%d]### on!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__);
         //msWriteByteMask(0x102F00,0x40,0xFF);
         //msWriteByteMask(0x102FFE,0x01,0x01);

            //msWriteByteMask(0x102F00,0x40,0xFF);
         //msWriteByteMask(0x1340FE,0x01,0x01);

         //printf("\033[1;35m###[Brian][%s][%d]### on3!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__);
        }
        else
            return 0;
    }
#if 0
    else if(E_SUB_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x7F),0x0001,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x7F),0x0001,0x0001,E_IP);
        }
        else
            return 0;
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}

MS_U8 Kano_HDRIP_HDRIP_path(MS_BOOL bMode, MS_U8 u8IpType)
{
    //write2ByteMask(0x102F00, 0xFFFF, 0x0040 ,E_IP);

    if(E_MAIN_WINDOW == u8IpType)
    {
    if(E_OFF == bMode)
    {
        write2ByteMask(_PK_L_kano(0x40,0x7F),0x0004,0x0000,E_IP);
    }
    else if(E_ON == bMode)
    {
        write2ByteMask(_PK_L_kano(0x40,0x7F),0x0004,0x0000,E_IP);
    }
    else
        return 0;
    }
#if 0
    else if(E_SUB_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x7F),0x0001,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x7F),0x0001,0x0001,E_IP);
        }
        else
            return 0;
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}

//reg_rgb_3d_lut_main_en
//reg_rgb_3d_lut_sub_en
MS_U8 Kano_HDRIP_RGB3D_enable(MS_BOOL bMode, MS_U8 u8IpType)
{

    write2ByteMask(0x102F00, 0xFFFF, 0x0040,E_IP);

    if(E_MAIN_WINDOW == u8IpType)
    {
    if(E_OFF == bMode)
    {
        write2ByteMask(_PK_L_kano(0x40,0x54),0x0001,0x0000,E_IP);
    }
    else if(E_ON == bMode)
    {
        write2ByteMask(_PK_L_kano(0x40,0x54),0x0001,0x0001,E_IP);
    }
    else
        return 0;
    }
#if 1
    else if(E_SUB_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x54),0x0002,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x54),0x0002,0x0002,E_IP);
        }
        else
            return 0;
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}

//reg_rgb_3d_lut_stage_sel
MS_U8 Kano_HDRIP_RGB3D_path(MS_BOOL bMode, MS_U8 u8IpType)
{
    //write2ByteMask(0x102F00, 0xFFFF, 0x0040,E_IP);

    if(E_MAIN_WINDOW == u8IpType)
    {
    if(E_OFF == bMode)
    {
        write2ByteMask(_PK_L_kano(0x40,0x54),0x0300,0x0300,E_IP);
    }
#if 0
    else if(E_ON == bMode)
    {
        write2ByteMask(_PK_L_kano(0x40,0x54),0x0001,0x0001,E_IP);
    }
#endif
    else
        return 0;
    }
#if 0
    else if(E_SUB_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x54),0x0002,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x54),0x0002,0x0002,E_IP);
        }
        else
            return 0;
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}

//handle RGB 3D LUT
//write table
MS_U8 Kano_HDRIP_RGB3D_settings(MS_BOOL bMode, MS_U8 u8IpType)
{

    if (0 != bMode)
    {
        return 0;
    }

    //from SW
    //SetOSDLUT(1);

    return 1;
}

MS_U8 Kano_HDRIP_YCGAINOFFSET_In(MS_BOOL bMode, MS_U8 u8IpType)
{
    write2ByteMask(0x102F00, 0xFFFF, 0x0042,E_IP);

    if(E_MAIN_WINDOW == u8IpType)
    {
        switch (bMode)
        {
            case 0: //off
                write2ByteMask(_PK_L_kano(0x42,0x18),0x07FF,0x0400,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x19),0x01FF,0x0000,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1A),0x07FF,0x0400,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1B),0x01FF,0x0000,E_IP);
            break;

            case 1: //limit_to_full
                write2ByteMask(_PK_L_kano(0x42,0x18),0x07FF,0x04AC,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x19),0x01FF,0x014B,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1A),0x07FF,0x0492,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1B),0x01FF,0x0149,E_IP);
            break;

            case 2: //limit_to_full_Conly
                write2ByteMask(_PK_L_kano(0x42,0x18),0x07FF,0x0400,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x19),0x01FF,0x0000,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1A),0x07FF,0x0492,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1B),0x01FF,0x0149,E_IP);
            break;

            case 3: //full_to_limit
                write2ByteMask(_PK_L_kano(0x42,0x18),0x07FF,0x036D,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x19),0x01FF,0x0040,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1A),0x07FF,0x0380,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1B),0x01FF,0x0040,E_IP);
            break;

            default:
                write2ByteMask(_PK_L_kano(0x42,0x18),0x07FF,0x0400,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x19),0x01FF,0x0000,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1A),0x07FF,0x0400,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1B),0x01FF,0x0000,E_IP);
            break;
        }
    }
    else
    {
        return 0;
    }

    return 1;
}

MS_U8 Kano_HDRIP_YCGAINOFFSET_Out(MS_BOOL bMode, MS_U8 u8IpType)
{
    write2ByteMask(0x102F00, 0xFFFF, 0x0042,E_IP);

    if(E_MAIN_WINDOW == u8IpType)
    {
        switch (bMode)
        {
            case 0: //off
                write2ByteMask(_PK_L_kano(0x42,0x1C),0x07FF,0x0400,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1D),0x01FF,0x0000,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1E),0x07FF,0x0400,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1F),0x01FF,0x0000,E_IP);
            break;

            case 1: //full_to_limit
                write2ByteMask(_PK_L_kano(0x42,0x1C),0x07FF,0x036D,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1D),0x01FF,0x004B,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1E),0x07FF,0x0380,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1F),0x01FF,0x0049,E_IP);
            break;

            case 2: //limit_to_full
                write2ByteMask(_PK_L_kano(0x42,0x1C),0x07FF,0x04AC,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1D),0x01FF,0x014B,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1E),0x07FF,0x0492,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1F),0x01FF,0x0149,E_IP);
            break;

            default:
                write2ByteMask(_PK_L_kano(0x42,0x1C),0x07FF,0x0400,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1D),0x01FF,0x0000,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1E),0x07FF,0x0400,E_IP);
                write2ByteMask(_PK_L_kano(0x42,0x1F),0x01FF,0x0000,E_IP);
            break;
        }
    }
    else
    {
        return 0;
    }

    return 1;
}


MS_U8 Kano_HDRIP_rgbOffset(MS_BOOL bMode, MS_U8 u8IpType)
{

    //write2ByteMask(0x102F00, 0xFFFF, 0x0040,E_IP);

    if(E_MAIN_WINDOW == u8IpType) //main window
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x30),0x0300,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x31),0x07FF,0x0400,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x32),0x07FF,0x0400,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x33),0x07FF,0x0400,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x30),0x0300,0x0200,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x31),0x07FF,0x0400,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x32),0x07FF,0x0400,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x33),0x07FF,0x0400,E_IP);
        }
        else
            return 0;
    }
#if 0
    else if(E_SUB_WINDOW == u8IpType) //sub window
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK0F_57_L,0x0010,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_5E_L,0x1FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_5F_L,0x0FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_60_L,0x1FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_61_L,0x0FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_62_L,0x1FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_63_L,0x0FFF,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK0F_57_L,0x0010,0x0010,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_5E_L,0x1FFF,0x1FFF,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_5F_L,0x0FFF,0x03FF,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_60_L,0x1FFF,0x1FFF,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_61_L,0x0FFF,0x03FF,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_62_L,0x1FFF,0x1FFF,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_63_L,0x0FFF,0x03FF,E_IP);
        }
        else
            return 0;
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}


MS_U8 Kano_HDRIP_rgbClip(MS_BOOL bMode, MS_U8 u8IpType)
{

    //write2ByteMask(0x102F00, 0xFFFF, 0x0040,E_IP);

    if(E_MAIN_WINDOW == u8IpType) //main window
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x30),0x0001,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x34),0x1FFF,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x35),0x0FFF,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x36),0x1FFF,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x37),0x0FFF,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x38),0x1FFF,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x39),0x0FFF,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x30),0x0001,0x0001,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x34),0x1FFF,0x1FFF,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x35),0x0FFF,0x0FFF,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x36),0x1FFF,0x1FFF,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x37),0x0FFF,0x0FFF,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x38),0x1FFF,0x1FFF,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x39),0x0FFF,0x0FFF,E_IP);
        }
        else
            return 0;
    }
#if 0
    else if(E_SUB_WINDOW == u8IpType) //sub window
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK0F_57_L,0x0010,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_5E_L,0x1FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_5F_L,0x0FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_60_L,0x1FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_61_L,0x0FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_62_L,0x1FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_63_L,0x0FFF,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK0F_57_L,0x0010,0x0010,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_5E_L,0x1FFF,0x1FFF,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_5F_L,0x0FFF,0x03FF,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_60_L,0x1FFF,0x1FFF,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_61_L,0x0FFF,0x03FF,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_62_L,0x1FFF,0x1FFF,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_63_L,0x0FFF,0x03FF,E_IP);
        }
        else
            return 0;
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}

MS_U8 Kano_HDRIP_DeGammaDither(MS_BOOL bMode, MS_U8 u8IpType){

    write2ByteMask(0x102F00, 0xFFFF, 0x0040,E_IP);

    if(E_MAIN_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x01),0x1000,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x01),0x1000,0x1000,E_IP);
        }
        else
        {
        return 0;
        }
    }
#if 0
    else if(E_SUB_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x01),0x1000,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x01),0x1000,0x1000,E_IP);
        }
        else
        {
            return 0;
        }
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}

MS_U8 Kano_HDRIP_RgbComp(MS_BOOL bMode, MS_U8 u8IpType)
{

    write2ByteMask(0x102F00, 0xFFFF, 0x0040,E_IP);

    if(E_MAIN_WINDOW == u8IpType)
    {
    if(E_OFF == bMode)
    {
        write2ByteMask(_PK_L_kano(0x40,0x01),0x8200,0x0000,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x42),0xFF7F,0x8000,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x43),0xFF7F,0x8000,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x40),0xFF7F,0x8000,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x41),0xFF7F,0x8000,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x44),0xFF7F,0x8000,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x45),0xFF7F,0x8000,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x48),0xF3FF,0x0000,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x4A),0x0F07,0x0000,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x4B),0x1FFF,0x0000,E_IP);
    }
    else
        {
            return 0;
        }
    }
#if 0
    else if(E_SUB_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK25_11_L,0x8200,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_6E_L,0xFF7F,0x8000,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_6F_L,0xFF7F,0x8000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_42_L,0xFF7F,0x8000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_43_L,0xFF7F,0x8000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_46_L,0xFF7F,0x8000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_47_L,0xFF7F,0x8000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_49_L,0xF3FF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_4C_L,0x0F07,0x8000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_4D_L,0x1FFF,0x8000,E_IP);
        }
        else
        {
        return 0;
        }
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}

MS_U8 Kano_HDRIP_RgbCompDither(MS_BOOL bMode, MS_U8 u8IpType)
{
    write2ByteMask(0x102F00, 0xFFFF, 0x0040,E_IP);

    if(E_MAIN_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x01),0x0400,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x01),0x0400,0x0400,E_IP);
        }
        else
            return 0;
    }
#if 0
    else if(E_SUB_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK25_11_L,0x0400,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK25_11_L,0x0400,0x0400,E_IP);
        }
        else
            return 0;
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}

MS_U8 Kano_HDRIP_linearRgbClip(MS_BOOL bMode, MS_U8 u8IpType)
{
    write2ByteMask(0x102F00, 0xFFFF, 0x0040,E_IP);

    if(E_MAIN_WINDOW == u8IpType)
    {
    if(E_OFF == bMode)
    {
        write2ByteMask(_PK_L_kano(0x40,0x01),0x0008,0x0000,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x0B),0x0FFF,0x0000,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x0C),0x0FFF,0x0000,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x0D),0x0FFF,0x0000,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x0E),0x0FFF,0x0000,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x0F),0x0FFF,0x0000,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x10),0x0FFF,0x0000,E_IP);
    }
    else if(E_ON == bMode)
    {
        write2ByteMask(_PK_L_kano(0x40,0x01),0x0008,0x0008,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x0B),0x0FFF,0x0FFF,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x0C),0x0FFF,0x0FFF,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x0D),0x0FFF,0x0FFF,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x0E),0x0FFF,0x0FFF,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x0F),0x0FFF,0x0FFF,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x10),0x0FFF,0x0FFF,E_IP);
    }
    else
        return 0;
    }
#if 0
    else if(E_SUB_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK25_11_L,0x0008,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_1B_L,0x0FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_1C_L,0x0FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_1D_L,0x0FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_1E_L,0x0FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_1F_L,0x0FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_20_L,0x0FFF,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK25_11_L,0x0008,0x0008,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_1B_L,0x0FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_1C_L,0x0FFF,0x0FFF,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_1D_L,0x0FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_1E_L,0x0FFF,0x0FFF,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_1F_L,0x0FFF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_20_L,0x0FFF,0x0FFF,E_IP);
        }
        else
            return 0;
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}

MS_U8 Kano_HDRIP_linearRgb3x3_enable(MS_BOOL bMode, MS_U8 u8IpType)
{

    write2ByteMask(0x102F00, 0xFFFF, 0x0040,E_IP);

    if(E_MAIN_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x01),0x0002,0x0000,E_IP);

        }
        else if(E_ON == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x01),0x0002,0x0002,E_IP);

        }
        else
            return 0;
    }
#if 0
    else if(E_SUB_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK25_11_L,0x0002,0x0000,E_IP);

        }
        else if(E_ON == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK25_11_L,0x0002,0x0002,E_IP);

        }
        else
            return 0;
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}

MS_U8 Kano_HDRIP_GammaDither(MS_BOOL bMode, MS_U8 u8IpType)
{

    write2ByteMask(0x102F00, 0xFFFF, 0x0040,E_IP);

    if(E_MAIN_WINDOW == u8IpType)
    {
    if(E_OFF == bMode)
    {
        write2ByteMask(_PK_L_kano(0x40,0x01),0x0800,0x0000,E_IP);
    }
    else if(E_ON == bMode)
    {
        write2ByteMask(_PK_L_kano(0x40,0x01),0x0800,0x0800,E_IP);
    }
    else
        return 0;
    }
#if 0
    else if(E_SUB_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK25_11_L,0x0800,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK25_11_L,0x0800,0x0800,E_IP);
        }
        else
            return 0;
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}

MS_U8 Kano_HDRIP_linearRgbBypass(MS_BOOL bMode, MS_U8 u8IpType)
{

    write2ByteMask(0x102F00, 0xFFFF, 0x0040,E_IP);

    if(E_MAIN_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x01),0x0180,0x0080,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(_PK_L_kano(0x40,0x01),0x0180,0x0180,E_IP);
        }
        else
            return 0;
    }
#if 0
    else if(E_SUB_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK25_11_L,0x0180,0x0080,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK25_11_L,0x0180,0x0180,E_IP);
        }
        else
            return 0;
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}



MS_U8 Kano_TMOIP_curve_hardcodetemp(MS_BOOL bEnable)
{

    if (0 == bEnable)
    {
        return 1;
    }

    write2ByteMask(0x102F00, 0xFFFF, 0x0042, E_IP);

    //patch on
    //TMO curve for ylimit to yfull
    //curve from dolby_pq_kano_IBCDemoTSBTV_with_TMOpatch_100nits2.xlsx
    if (CFD_KANO_PATCH0 == 1)
    {
        write2ByteMask(_PK_L_kano(0x42,0x76),0x81FF,0x8110,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x77),0x01FF,0x0088,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x30),0xFFFF,0x1210,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x31),0xFFFF,0x402E,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x32),0xFFFF,0x574D,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x33),0xFFFF,0x6660,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x34),0xFFFF,0x726D,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x35),0xFFFF,0x7B76,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x36),0xFFFF,0x827F,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x37),0xFFFF,0x8886,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x78),0xFFFF,0x0000,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x79),0xFFFF,0x0000,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x7A),0x000F,0x0000,E_IP);
    }
    else  //dolby_pq_kano_IBCDemoTSBTV_with_TMOpatch_100nits2_nopatch.xlsx
    {
        write2ByteMask(_PK_L_kano(0x42,0x76),0x81FF,0x010E,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x77),0x01FF,0x0087,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x30),0xFFFF,0x290E,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x31),0xFFFF,0x473A,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x32),0xFFFF,0x5951,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x33),0xFFFF,0x6660,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x34),0xFFFF,0x706C,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x35),0xFFFF,0x7874,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x36),0xFFFF,0x7F7C,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x37),0xFFFF,0x8582,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x78),0xFFFF,0x0000,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x79),0xFFFF,0x0000,E_IP);
        write2ByteMask(_PK_L_kano(0x42,0x7A),0x000F,0x0000,E_IP);
    }

    return 1;
}

MS_U8 Kano_TMOIP_UVC_write(MS_BOOL bMode, MS_U8 u8IpType)
{

    //write2ByteMask(0x102F00, 0xFFFF, 0x0042);

    if(E_MAIN_WINDOW == u8IpType)
    {
        switch (bMode)
        {
            case 0: //off
            write2ByteMask(_PK_L_kano(0x42,0x08),0x0300,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x64),0x0FFF,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x65),0x0FFF,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x11),0x00FF,0x0000,E_IP); //0x00FF
            write2ByteMask(_PK_L_kano(0x42,0x12),0xFFFF,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x13),0x1F1F,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x14),0x001F,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x49),0xFFFF,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x4A),0xFF3F,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x4B),0x0FFF,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x09),0x00BF,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x0A),0x03FF,0x0000,E_IP);
            break;

            case 1: //PQ1
            write2ByteMask(_PK_L_kano(0x42,0x08),0x0300,0x0100,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x64),0x0FFF,0x0200,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x65),0x0FFF,0x0080,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x11),0x00FF,0x005E,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x12),0xFFFF,0xE030,E_IP);//
            write2ByteMask(_PK_L_kano(0x42,0x13),0x1F1F,0x0802,E_IP);//
            write2ByteMask(_PK_L_kano(0x42,0x14),0x001F,0x0008,E_IP);//
            write2ByteMask(_PK_L_kano(0x42,0x49),0xFFFF,0x0020,E_IP);//
            write2ByteMask(_PK_L_kano(0x42,0x4A),0xFF3F,0x8404,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x4B),0x0FFF,0x0040,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x09),0x00BF,0x0020,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x0A),0x03FF,0x0200,E_IP);
            break;

            case 2: //PQ2
            write2ByteMask(_PK_L_kano(0x42,0x08),0x0300,0x0100,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x64),0x0FFF,0x0200,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x65),0x0FFF,0x0080,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x11),0x00FF,0x0052,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x12),0xFFFF,0xE030,E_IP);//
            write2ByteMask(_PK_L_kano(0x42,0x13),0x1F1F,0x0802,E_IP);//
            write2ByteMask(_PK_L_kano(0x42,0x14),0x001F,0x0008,E_IP);//
            write2ByteMask(_PK_L_kano(0x42,0x49),0xFFFF,0x0020,E_IP);//
            write2ByteMask(_PK_L_kano(0x42,0x4A),0xFF3F,0x8404,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x4B),0x0FFF,0x0040,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x09),0x00BF,0x00A0,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x0A),0x03FF,0x03FF,E_IP);
            break;

            default:
            write2ByteMask(_PK_L_kano(0x42,0x08),0x0300,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x64),0x0FFF,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x65),0x0FFF,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x11),0x00FF,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x12),0xFFFF,0x0000,E_IP);//
            write2ByteMask(_PK_L_kano(0x42,0x13),0x1F1F,0x0000,E_IP);//
            write2ByteMask(_PK_L_kano(0x42,0x14),0x001F,0x0000,E_IP);//
            write2ByteMask(_PK_L_kano(0x42,0x49),0xFFFF,0x0000,E_IP);//
            write2ByteMask(_PK_L_kano(0x42,0x4A),0xFF3F,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x4B),0x0FFF,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x09),0x00BF,0x0000,E_IP);
            write2ByteMask(_PK_L_kano(0x42,0x0A),0x03FF,0x0000,E_IP);
            break;
        }

    }
#if 0
    else if(E_SUB_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK25_11_L,0x8600,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_6E_L,0xFF7F,0x8000,E_IP);
            write2ByteMask(REG_SC_Ali_BK0F_6F_L,0xFF7F,0x8000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_42_L,0xFF7F,0x8000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_43_L,0xFF7F,0x8000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_46_L,0xFF7F,0x8000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_47_L,0xFF7F,0x8000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_49_L,0xF3FF,0x0000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_4C_L,0x0F07,0x8000,E_IP);
            write2ByteMask(REG_SC_Ali_BK25_4D_L,0x1FFF,0x8000,E_IP);
        }
        else
        {
        return 0;
        }
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}

MS_U8 Kano_TMOIP_TMO_enable_write(MS_BOOL bMode, MS_U8 u8IpType)
{

    //for the histogram report part
    //write2ByteMask(0x102F00, 0xFFFF, 0x0041);
    //write2ByteMask(_PK_L_kano(0x41,0x04),0x0202,0x0202);
    //write2ByteMask(_PK_L_kano(0x41,0x08),0x0001,0x0001);

    //write2ByteMask(0x102F00, 0xFFFF, 0x0042);
    if(E_MAIN_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(_PK_L_kano(0x42,0x04),0x0080,0x0000,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(_PK_L_kano(0x42,0x04),0x0080,0x0080,E_IP);
        }
        else
            return 0;
    }
#if 0
    else if(E_SUB_WINDOW == u8IpType)
    {
        if(E_OFF == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK25_11_L,0x0180,0x0080,E_IP);
        }
        else if(E_ON == bMode)
        {
            write2ByteMask(REG_SC_Ali_BK25_11_L,0x0180,0x0180,E_IP);
        }
        else
            return 0;
    }
#endif
    else
    {
        return 0;
    }

    return 1;
}


//end of Kano IP write register function =========================================================

MS_U16 MS_Cfd_Kano_CheckModes(MS_U8 *temp, MS_U8 mode_upbound)
{
    //E_CFD_MC_ERR
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8temp = 0;

    u8temp = (*temp)&MaskForMode_LB;

    if (u8temp >= mode_upbound)
    {
            u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_OVERRANGE;
            (*temp) = ((*temp)&MaskForMode_HB);
            HDR_DBG_HAL_CFD(printk("\n  out of range [%s]\n",__func__));
    }

    return u16_check_status;
}

//initialize the variables besides the modes for every controlled modules
void Mapi_Cfd_Kano_TMOIP_Param_Init(STU_CFDAPI_Kano_TMOIP *pstu_Kano_TMOIP_Param)
{
    //pstu_Kano_TMOIP_Param->u8HDR_TMO_curve_Mode = 0;

    //for TMO algorithm ,  from user and driver
    //0: not set
    //1: set
    pstu_Kano_TMOIP_Param->u8HDR_TMO_param_SetbyDriver = 0;
    pstu_Kano_TMOIP_Param->u16HDR_TMO_Tmin = 3000;
    pstu_Kano_TMOIP_Param->u16HDR_TMO_Tmed = 120;
    pstu_Kano_TMOIP_Param->u16HDR_TMO_Tmax = 600;

}

//initialize the variables besides the modes for every controlled modules
void Mapi_Cfd_Kano_HDRIP_Param_Init(STU_CFDAPI_Kano_HDRIP *pstu_Kano_HDRIP_Param)
{
    pstu_Kano_HDRIP_Param->u8HDR_InputCSC_Ratio1 = 0x40;
    pstu_Kano_HDRIP_Param->u8HDR_InputCSC_Manual_Vars_en = 0;
    pstu_Kano_HDRIP_Param->u8HDR_InputCSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;

    pstu_Kano_HDRIP_Param->u8HDR_Degamma_Ratio1 = 0x40;//0x40 = 1 Q2.6
    pstu_Kano_HDRIP_Param->u16HDR_Degamma_Ratio2 = 0x40;//0x40 = 1 Q2.6

    pstu_Kano_HDRIP_Param->u8HDR_DeGamma_Manual_Vars_en = 0;
    pstu_Kano_HDRIP_Param->u8HDR_Degamma_TR = E_CFD_CFIO_TR_BT709;
    pstu_Kano_HDRIP_Param->u8HDR_Degamma_Lut_En = 0;
    pstu_Kano_HDRIP_Param->pu32HDR_Degamma_Lut_Address = NULL;
    pstu_Kano_HDRIP_Param->u16HDR_Degamma_Lut_Length = 600;
    pstu_Kano_HDRIP_Param->u8HDR_Degamma_Max_Lum_En = 0;
    pstu_Kano_HDRIP_Param->u16HDR_Degamma_Max_Lum = 100;

    pstu_Kano_HDRIP_Param->u16HDR_3x3_Ratio2 = 0x40;//0x40 = 1 Q2.6
    pstu_Kano_HDRIP_Param->u8HDR_3x3_Manual_Vars_en = 0;
    pstu_Kano_HDRIP_Param->u8HDR_3x3_InputCP = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    pstu_Kano_HDRIP_Param->u8HDR_3x3_OutputCP = E_CFD_CFIO_CP_BT709_SRGB_SYCC;

    pstu_Kano_HDRIP_Param->u8HDR_Gamma_Manual_Vars_en = 0;
    pstu_Kano_HDRIP_Param->u8HDR_Gamma_TR = E_CFD_CFIO_TR_BT709;
    pstu_Kano_HDRIP_Param->u8HDR_Gamma_Lut_En = 0;
    pstu_Kano_HDRIP_Param->pu32HDR_Gamma_Lut_Address = NULL;
    pstu_Kano_HDRIP_Param->u16HDR_Gamma_Lut_Length = 256;

    pstu_Kano_HDRIP_Param->u8HDR_OutCSC_Ratio1 = 0x40;
    pstu_Kano_HDRIP_Param->u8HDR_OutCSC_Manual_Vars_en = 0;
    pstu_Kano_HDRIP_Param->u8HDR_OutCSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;

}

//initialize the variables besides the modes for every controlled modules
void Mapi_Cfd_Kano_SDRIP_Param_Init(STU_CFDAPI_Kano_SDRIP *pstu_Kano_SDRIP_Param)
{

    pstu_Kano_SDRIP_Param->u8SDR_IP2_CSC_Ratio1 = 0x40;
    pstu_Kano_SDRIP_Param->u8SDR_IP2_CSC_Manual_Vars_en = 0;
    pstu_Kano_SDRIP_Param->u8SDR_IP2_CSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;

    pstu_Kano_SDRIP_Param->u8SDR_VIP_CM_Ratio1 = 0x40;
    pstu_Kano_SDRIP_Param->u8SDR_VIP_CM_Manual_Vars_en = 0;
    pstu_Kano_SDRIP_Param->u8SDR_VIP_CM_MC = E_CFD_CFIO_MC_BT709_XVYCC709;

    pstu_Kano_SDRIP_Param->u8SDR_Conv420_CM_Ratio1 = 0x40;
    pstu_Kano_SDRIP_Param->u8SDR_Conv420_CM_Manual_Vars_en = 0;
    pstu_Kano_SDRIP_Param->u8SDR_Conv420_CM_MC = E_CFD_CFIO_MC_BT709_XVYCC709;

#if (Kano_Control_HDMITX_CSC == 1)
    pstu_Kano_SDRIP_Param->u8SDR_HDMITX_CSC_Ratio1 = 0x40;
    pstu_Kano_SDRIP_Param->u8SDR_HDMITX_CSC_Manual_Vars_en = 0;
    pstu_Kano_SDRIP_Param->u8SDR_HDMITX_CSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
#endif
}

MS_U16 Mapi_Cfd_Kano_HDRIP_Param_Check(STU_CFDAPI_Kano_HDRIP *pstu_Kano_HDRIP_Param)
{
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
    //MS_U16 u16Temp;

    MS_U8 u8temp = 0;

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_IP_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_IP_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_IP_enable_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_IP_enable_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_IP_PATH_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_IP_PATH_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_IP_PATH_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_IP_PATH_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_RGB3D_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_RGB3D_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_RGB3D_enable_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_RGB3D_enable_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_RGB3D_PATH_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x01)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_RGB3D_PATH_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_RGB3D_PATH_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_RGB3D_PATH_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_RGB3D_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x01)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_RGB3D_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_RGB3D_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_RGB3D_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_YCGAINOFFSET_In_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x04)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_YCGAINOFFSET_In_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_YCGAINOFFSET_In_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_YCGAINOFFSET_In_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_YCGAINOFFSET_Out_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x03)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_RGB3D_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_YCGAINOFFSET_Out_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_YCGAINOFFSET_Out_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_InputCSC_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x0D)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_InputCSC_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_InputCSC_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_InputCSC_Mode,0);
    }

    //2x
    if (pstu_Kano_HDRIP_Param->u8HDR_InputCSC_Ratio1 > 0x80)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_InputCSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_InputCSC_Ratio1 = 0x80;
    }


    //1x
    if (pstu_Kano_HDRIP_Param->u8HDR_InputCSC_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_InputCSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_InputCSC_Ratio1 = 0x40;
    }

    if (pstu_Kano_HDRIP_Param->u8HDR_InputCSC_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_InputCSC_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_InputCSC_Manual_Vars_en = 0x00;
    }

    if (pstu_Kano_HDRIP_Param->u8HDR_InputCSC_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_InputCSC_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_InputCSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_RGBClip_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_RGBClip_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_RGBClip_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_RGBClip_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_LinearRGBBypass_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_LinearRGBBypass_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_LinearRGBBypass_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_LinearRGBBypass_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_Degamma_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Degamma_enable_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_Degamma_enable_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_Degamma_Dither_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_Dither_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Degamma_Dither_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_Degamma_Dither_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_Degamma_SRAM_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8Degamma_SRAM_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Degamma_SRAM_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_Degamma_SRAM_Mode,0);
    }

    //2x
    if (pstu_Kano_HDRIP_Param->u8HDR_Degamma_Ratio1 > 0x80)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Degamma_Ratio1 = 0x80;
    }

    //1x
    if (pstu_Kano_HDRIP_Param->u8HDR_Degamma_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Degamma_Ratio1 = 0x40;
    }

    //8x
    if (pstu_Kano_HDRIP_Param->u16HDR_Degamma_Ratio2 > 0x200)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_Degamma_Ratio2 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u16HDR_Degamma_Ratio2 = 0x200;
    }

    //1x
    if (pstu_Kano_HDRIP_Param->u16HDR_Degamma_Ratio2 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_Degamma_Ratio2 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u16HDR_Degamma_Ratio2 = 0x40;
    }

    if (pstu_Kano_HDRIP_Param->u8HDR_DeGamma_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_DeGamma_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_DeGamma_Manual_Vars_en = 0x00;
    }

    if (pstu_Kano_HDRIP_Param->u8HDR_Degamma_TR >= E_CFD_CFIO_TR_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_TR is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Degamma_TR = E_CFD_CFIO_TR_BT709;
    }

    if (pstu_Kano_HDRIP_Param->u8HDR_Degamma_Lut_En > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_Lut_En is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Degamma_Lut_En = 0x00;
    }

    if (pstu_Kano_HDRIP_Param->u16HDR_Degamma_Lut_Length > 600)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_Degamma_Lut_Length is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u16HDR_Degamma_Lut_Length = 600;
    }

    if (pstu_Kano_HDRIP_Param->u16HDR_Degamma_Lut_Length < 256)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_Degamma_Lut_Length is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u16HDR_Degamma_Lut_Length = 256;
    }

    if (pstu_Kano_HDRIP_Param->u8HDR_Degamma_Max_Lum_En > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_Max_Lum_En is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Degamma_Max_Lum_En = 0x00;
    }

    if (pstu_Kano_HDRIP_Param->u16HDR_Degamma_Max_Lum > 0xff00)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_Degamma_Max_Lum is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u16HDR_Degamma_Max_Lum = 0xff00;
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_3x3_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3x3_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_3x3_enable_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_3x3_enable_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_3x3_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x03)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3x3_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_3x3_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_3x3_Mode,0);
    }

    //8x
    if (pstu_Kano_HDRIP_Param->u16HDR_3x3_Ratio2 > 0x200)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_3x3_Ratio2 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u16HDR_3x3_Ratio2 = 0x200;
    }

    //1x
    if (pstu_Kano_HDRIP_Param->u16HDR_3x3_Ratio2 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_3x3_Ratio2 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u16HDR_3x3_Ratio2 = 0x40;
    }

    if (pstu_Kano_HDRIP_Param->u8HDR_3x3_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3x3_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_3x3_Manual_Vars_en = 0x00;
    }

    //if (pstu_Kano_HDRIP_Param->u8HDR_3x3_InputCP >= E_CFD_CFIO_CP_RESERVED_START)
    if (MS_Cfd_checkColorprimaryIsUndefined(&(pstu_Kano_HDRIP_Param->u8HDR_3x3_InputCP)))
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3x3_InputCP is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_3x3_InputCP = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    }

    //if (pstu_Kano_HDRIP_Param->u8HDR_3x3_OutputCP >= E_CFD_CFIO_CP_RESERVED_START)
    if (MS_Cfd_checkColorprimaryIsUndefined(&(pstu_Kano_HDRIP_Param->u8HDR_3x3_OutputCP)))
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3x3_OutputCP is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_3x3_OutputCP = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_Compress_settings_Mode);
    if (u8temp != 0)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Compress_settings_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Compress_settings_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_Compress_settings_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_Compress_dither_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Compress_dither_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Compress_dither_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_Compress_dither_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_3x3Clip_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3x3Clip_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_3x3Clip_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_3x3Clip_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_Gamma_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Gamma_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Gamma_enable_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_Gamma_enable_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_Gamma_Dither_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Gamma_Dither_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Gamma_Dither_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_Gamma_Dither_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_Gamma_maxdata_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Gamma_maxdata_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Gamma_maxdata_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_Gamma_maxdata_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_Gamma_SRAM_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Gamma_SRAM_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Gamma_SRAM_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_Gamma_SRAM_Mode,0);
    }

    if (pstu_Kano_HDRIP_Param->u8HDR_Gamma_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Gamma_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Gamma_Manual_Vars_en = 0x00;
    }

    if (pstu_Kano_HDRIP_Param->u8HDR_Gamma_TR >= E_CFD_CFIO_TR_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Gamma_TR is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Gamma_TR = E_CFD_CFIO_TR_BT709;
    }

    if (pstu_Kano_HDRIP_Param->u8HDR_Gamma_Lut_En > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Gamma_Lut_En is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_Gamma_Lut_En = 0x00;
    }

    if (pstu_Kano_HDRIP_Param->u16HDR_Gamma_Lut_Length != 256)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_Gamma_Lut_Length is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u16HDR_Gamma_Lut_Length = 256;
    }

    u8temp = FunctionMode(pstu_Kano_HDRIP_Param->u8HDR_OutCSC_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x0D)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_OutCSC_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_OutCSC_Mode = function1(pstu_Kano_HDRIP_Param->u8HDR_OutCSC_Mode,0);
    }

    //1x
    if (pstu_Kano_HDRIP_Param->u8HDR_OutCSC_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_OutCSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_OutCSC_Ratio1 = 0x40;
    }

    //1x
    if (pstu_Kano_HDRIP_Param->u8HDR_OutCSC_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_OutCSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_OutCSC_Ratio1 = 0x40;
    }

    if (pstu_Kano_HDRIP_Param->u8HDR_OutCSC_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_OutCSC_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_OutCSC_Manual_Vars_en = 0x00;
    }

    if (pstu_Kano_HDRIP_Param->u8HDR_OutCSC_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_OutCSC_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Kano_HDRIP_Param->u8HDR_OutCSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }

    return u16_check_status;
}

MS_U16 Mapi_Cfd_Kano_SDRIP_Param_Check(STU_CFDAPI_Kano_SDRIP *pstu_Kano_SDRIP_Param)
{
    MS_U8 u8_check_status = TRUE;

    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
    //MS_U16 u16Temp;

    MS_U8 u8temp = 0;

    u8temp = FunctionMode(pstu_Kano_SDRIP_Param->u8SDR_IP2_CSC_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x0D)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_IP2_CSC_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_IP2_CSC_Mode = function1(pstu_Kano_SDRIP_Param->u8SDR_IP2_CSC_Mode,0);
    }

    //1x
    if (pstu_Kano_SDRIP_Param->u8SDR_IP2_CSC_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8IP2_CSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_IP2_CSC_Ratio1 = 0x40;
    }

    //1x
    if (pstu_Kano_SDRIP_Param->u8SDR_IP2_CSC_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8IP2_CSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_IP2_CSC_Ratio1 = 0x40;
    }

    if (pstu_Kano_SDRIP_Param->u8SDR_IP2_CSC_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_IP2_CSC_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_IP2_CSC_Manual_Vars_en = 0x00;
    }

    if (pstu_Kano_SDRIP_Param->u8SDR_IP2_CSC_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8SDR_IP2_CSC_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_IP2_CSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }


    u8temp = FunctionMode(pstu_Kano_SDRIP_Param->u8SDR_VIP_CM_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x0D)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_VIP_CM_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_VIP_CM_Mode = function1(pstu_Kano_SDRIP_Param->u8SDR_VIP_CM_Mode,0);
    }

    //not supported cases
    if ((u8temp == E_CFD_IP_CSC_RFULL_TO_RLIMIT) ||
        (u8temp == E_CFD_IP_CSC_RLIMIT_TO_RFULL) ||
        (u8temp == E_CFD_IP_CSC_RLIMIT_TO_YFULL) ||
        (u8temp == E_CFD_IP_CSC_RLIMIT_TO_YLIMIT) ||
        (u8temp == E_CFD_IP_CSC_YFULL_TO_RLIMIT) ||
        (u8temp == E_CFD_IP_CSC_YLIMIT_TO_RLIMIT))
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_VIP_CM_Mode %d is not supported now \n",u8temp));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_MODE_SDR_NOTSUPPORTED;

        switch(u8temp)
        {
            case E_CFD_IP_CSC_RFULL_TO_RLIMIT:
            u8temp = E_CFD_IP_CSC_OFF;
            break;

            case E_CFD_IP_CSC_RLIMIT_TO_RFULL:
            u8temp = E_CFD_IP_CSC_OFF;
            break;

            case E_CFD_IP_CSC_RLIMIT_TO_YFULL:
            u8temp = E_CFD_IP_CSC_RFULL_TO_YFULL;
            break;

            case E_CFD_IP_CSC_RLIMIT_TO_YLIMIT:
            u8temp = E_CFD_IP_CSC_RFULL_TO_YLIMIT;
            break;

            case E_CFD_IP_CSC_YFULL_TO_RLIMIT:
            u8temp = E_CFD_IP_CSC_YFULL_TO_RFULL;
            break;

            case E_CFD_IP_CSC_YLIMIT_TO_RLIMIT:
            u8temp = E_CFD_IP_CSC_YLIMIT_TO_RFULL;
            break;
        }

        pstu_Kano_SDRIP_Param->u8SDR_VIP_CM_Mode = function1(pstu_Kano_SDRIP_Param->u8SDR_VIP_CM_Mode,u8temp);
    }

    //1x
    if (pstu_Kano_SDRIP_Param->u8SDR_VIP_CM_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_VIP_CM_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_VIP_CM_Ratio1 = 0x40;
    }

    //1x
    if (pstu_Kano_SDRIP_Param->u8SDR_VIP_CM_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_VIP_CM_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_VIP_CM_Ratio1 = 0x40;
    }

    if (pstu_Kano_SDRIP_Param->u8SDR_VIP_CM_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_VIP_CM_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_VIP_CM_Manual_Vars_en = 0x00;
    }

    if (pstu_Kano_SDRIP_Param->u8SDR_VIP_CM_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8SDR_VIP_CM_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_VIP_CM_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }

    u8temp = FunctionMode(pstu_Kano_SDRIP_Param->u8SDR_Conv420_CM_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x0D)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_Conv420_CM_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_Conv420_CM_Mode = function1(pstu_Kano_SDRIP_Param->u8SDR_Conv420_CM_Mode,0);
    }

    //not supported cases
    if ((u8temp == E_CFD_IP_CSC_RFULL_TO_RLIMIT) ||
        (u8temp == E_CFD_IP_CSC_RLIMIT_TO_RFULL) ||
        (u8temp == E_CFD_IP_CSC_RLIMIT_TO_YFULL) ||
        (u8temp == E_CFD_IP_CSC_RLIMIT_TO_YLIMIT) ||
        (u8temp == E_CFD_IP_CSC_YFULL_TO_RLIMIT) ||
        (u8temp == E_CFD_IP_CSC_YLIMIT_TO_RLIMIT))
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_Conv420_CM_Mode %d is not supported now \n",u8temp));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_MODE_SDR_NOTSUPPORTED;

        switch(u8temp)
        {
            case E_CFD_IP_CSC_RFULL_TO_RLIMIT:
            u8temp = E_CFD_IP_CSC_OFF;
            break;

            case E_CFD_IP_CSC_RLIMIT_TO_RFULL:
            u8temp = E_CFD_IP_CSC_OFF;
            break;

            case E_CFD_IP_CSC_RLIMIT_TO_YFULL:
            u8temp = E_CFD_IP_CSC_RFULL_TO_YFULL;
            break;

            case E_CFD_IP_CSC_RLIMIT_TO_YLIMIT:
            u8temp = E_CFD_IP_CSC_RFULL_TO_YLIMIT;
            break;

            case E_CFD_IP_CSC_YFULL_TO_RLIMIT:
            u8temp = E_CFD_IP_CSC_YFULL_TO_RFULL;
            break;

            case E_CFD_IP_CSC_YLIMIT_TO_RLIMIT:
            u8temp = E_CFD_IP_CSC_YLIMIT_TO_RFULL;
            break;
        }

        pstu_Kano_SDRIP_Param->u8SDR_Conv420_CM_Mode = function1(pstu_Kano_SDRIP_Param->u8SDR_Conv420_CM_Mode,u8temp);
    }

    //1x
    if (pstu_Kano_SDRIP_Param->u8SDR_Conv420_CM_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_Conv420_CM_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_Conv420_CM_Ratio1 = 0x40;
    }

    //1x
    if (pstu_Kano_SDRIP_Param->u8SDR_Conv420_CM_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_Conv420_CM_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_Conv420_CM_Ratio1 = 0x40;
    }

    if (pstu_Kano_SDRIP_Param->u8SDR_Conv420_CM_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_Conv420_CM_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_Conv420_CM_Manual_Vars_en = 0x00;
    }

    if (pstu_Kano_SDRIP_Param->u8SDR_Conv420_CM_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8SDR_Conv420_CM_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_Conv420_CM_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }

#if (Kano_Control_HDMITX_CSC == 1)

    u8temp = FunctionMode(pstu_Kano_SDRIP_Param->u8SDR_HDMITX_CSC_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x0D)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_HDMITX_CSC_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_HDMITX_CSC_Mode = function1(pstu_Kano_SDRIP_Param->u8SDR_HDMITX_CSC_Mode,0);
    }

    //1x
    if (pstu_Kano_SDRIP_Param->u8SDR_HDMITX_CSC_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_HDMITX_CSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_HDMITX_CSC_Ratio1 = 0x40;
    }

    //1x
    if (pstu_Kano_SDRIP_Param->u8SDR_HDMITX_CSC_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_HDMITX_CSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_HDMITX_CSC_Ratio1 = 0x40;
    }

    if (pstu_Kano_SDRIP_Param->u8SDR_HDMITX_CSC_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8SDR_HDMITX_CSC_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_HDMITX_CSC_Manual_Vars_en = 0x00;
    }

    if (pstu_Kano_SDRIP_Param->u8SDR_HDMITX_CSC_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8SDR_HDMITX_CSC_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Kano_SDRIP_Param->u8SDR_HDMITX_CSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }

#endif

    return u16_check_status;
}

MS_U16 Mapi_Cfd_Kano_TMOIP_Param_Check(STU_CFDAPI_Kano_TMOIP *pstu_Kano_TMOIP_Param)
{
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    //MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
    //MS_U16 u16Temp;

    MS_U8 u8temp = 0;
    MS_U16 u16temp = 0;

    u8temp = FunctionMode(pstu_Kano_TMOIP_Param->u8HDR_TMO_curve_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n TMO u8HDR_TMO_curve_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Kano_TMOIP_Param->u8HDR_TMO_curve_enable_Mode = function1(pstu_Kano_TMOIP_Param->u8HDR_TMO_curve_enable_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_TMOIP_Param->u8HDR_TMO_curve_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x05)
    {
        HDR_DBG_HAL_CFD(printk("\n  TMO u8HDR_TMO_curve_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Kano_TMOIP_Param->u8HDR_TMO_curve_Mode = function1(pstu_Kano_TMOIP_Param->u8HDR_TMO_curve_Mode,0);
    }

    u8temp = FunctionMode(pstu_Kano_TMOIP_Param->u8HDR_UVC_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x03)
    {
        HDR_DBG_HAL_CFD(printk("\n  TMO u8HDR_UVC_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Kano_TMOIP_Param->u8HDR_UVC_Mode = function1(pstu_Kano_TMOIP_Param->u8HDR_UVC_Mode,0);
    }

    u8temp = pstu_Kano_TMOIP_Param->u8HDR_TMO_param_SetbyDriver;
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  TMO u8HDR_TMO_param_SetbyDriver is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Kano_TMOIP_Param->u8HDR_TMO_param_SetbyDriver = 0;
    }

    u16temp = pstu_Kano_TMOIP_Param->u16HDR_TMO_Tmin;
    if (u16temp >= 10001)
    {
        HDR_DBG_HAL_CFD(printk("\n  TMO u16HDR_TMO_Tmin is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Kano_TMOIP_Param->u16HDR_TMO_Tmin = 10000;
    }

    u16temp = pstu_Kano_TMOIP_Param->u16HDR_TMO_Tmed;
    if (u16temp >= 10001)
    {
        HDR_DBG_HAL_CFD(printk("\n  TMO u16HDR_TMO_Tmed is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Kano_TMOIP_Param->u16HDR_TMO_Tmed = 128;
    }

    u16temp = pstu_Kano_TMOIP_Param->u16HDR_TMO_Tmax;
    if (u16temp >= 10001)
    {
        HDR_DBG_HAL_CFD(printk("\n  TMO u16HDR_TMO_Tmax is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Kano_TMOIP_Param->u16HDR_TMO_Tmax = 10000;
    }

    if (u16temp < 100)
    {
        HDR_DBG_HAL_CFD(printk("\n  TMO u16HDR_TMO_Tmax is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Kano_TMOIP_Param->u16HDR_TMO_Tmax = 100;
    }

    //error correction
    if (pstu_Kano_TMOIP_Param->u16HDR_TMO_Tmed >=  pstu_Kano_TMOIP_Param->u16HDR_TMO_Tmax)
    {

      while (pstu_Kano_TMOIP_Param->u16HDR_TMO_Tmed >=  pstu_Kano_TMOIP_Param->u16HDR_TMO_Tmax)
      {
        pstu_Kano_TMOIP_Param->u16HDR_TMO_Tmed = pstu_Kano_TMOIP_Param->u16HDR_TMO_Tmed>>1;
      }

      u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
    }


    return u16_check_status;
}


//Due to Kano's ygain_offset issue
MS_U16 MS_Cfd_KanoPatch_TMO_For_LimitIn(MS_U8 *TMO_curve, MS_U8 *TMO_curve_patch)
{
    MS_U16 status = 0;

    return status;
}

MS_U8 MS_Cfd_Kano_HDR_TMO_Curve_Mode_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    MS_U8 u8_mode = 0;

    //Linear = 0
    //TMO_alg() = 1
    //Manual = 2 , not write registers
    //fixed curve = 3


    if(0x80 == (pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_Mode&0x80))
    {

        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if (1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
            {
                u8_mode = 1;
            }
            else
            {
                u8_mode = 0;
            }
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_Mode), 0x05);

        u8_mode = pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_Mode;
    }

    return u8_mode;
}


MS_U8 MS_Cfd_Kano_HDR_TMO_curve_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    //0: OFF
    //1: ON

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_enable_Mode&0x80))
    {

        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            if (1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_enable_Mode), 0x02);

        u8_mode = pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Kano_HDR_UVC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    MS_U8 u8_mode = 0;

    //0: off
    //1: PQ1
    //2: PQ2

    //for all cases
    u8_mode = 0;

    if(0x80 == (pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_UVC_Mode&0x80))
    {

        //openHDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            if (1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
            {
                u8_mode = 2;
            }
            else
            {
                u8_mode = 0;
            }
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            if (1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
            {
                u8_mode = 2;
            }
            else
            {
                u8_mode = 0;
            }
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_UVC_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_UVC_Mode), 0x03);
        u8_mode = pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_UVC_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Kano_HDR_Path_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    MS_U8 u8_mode = 0;

    //do decision
    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_IP_enable_Mode&0x80))
    {
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            if (((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && (0x04 == (CFD_HDR_IP_CAPABILITY&0x04))) ||
            ((1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) && (0x02 == (CFD_HDR_IP_CAPABILITY&0x02))) ||
            ((1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) && (0x08 == (CFD_HDR_IP_CAPABILITY&0x08))) ||
            ((1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag) && (0x01 == (CFD_HDR_IP_CAPABILITY&0x01))))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
            else if (((E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) ||
                     (E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) ||
                     (E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format)) &&
                     (0x10 == (CFD_HDR_IP_CAPABILITY&0x10)))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
            else if (1 == pstu_Control_Param->u8DoForceEnterHDRIP_Flag)
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
            }
            else
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
            }
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_IP_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    //assign by user
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_IP_enable_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_IP_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Kano_HDR_IP_PATH_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    MS_U8 u8_mode = 0;
    //only one case
    //0: case 0

    //do decision
    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_IP_PATH_Mode&0x80))
    {
        u8_mode = 0; //before Y2R

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_IP_PATH_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    //assign by user
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_IP_PATH_Mode), 0x01);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_IP_PATH_Mode;
    }

    return u8_mode;
}


//force 0
//not use this HW now
MS_U8 MS_Cfd_Kano_HDR_RGB3D_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    MS_U8 u8_mode = 0;
    //only one case
    //0: case 0

    //do decision
    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_enable_Mode&0x80))
    {
        //HDR bypass
        //if ((E_CFIO_MODE_HDR2 == pstu_Control_Param->u8Temp_HDRMode[0]) && (E_CFIO_MODE_HDR2 == pstu_Control_Param->u8Temp_HDRMode[1]))
        if ((pstu_Control_Param->u8Temp_HDRMode[1] == pstu_Control_Param->u8Temp_HDRMode[0]) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            //u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
            u8_mode = 0;
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    //assign by user
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_enable_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_enable_Mode;
    }

    return u8_mode;

}

MS_U8 MS_Cfd_Kano_HDR_RGB3D_path_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    MS_U8 u8_mode = 0;
    //only one case
    //0: after OSD

    //do decision
    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_PATH_Mode&0x80))
    {
        u8_mode = 0; //after OSD

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_PATH_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    //assign by user
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_PATH_Mode), 0x01);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_PATH_Mode;
    }

    return u8_mode;

}

MS_U8 MS_Cfd_Kano_HDR_RGB3D_Mode_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    MS_U8 u8_mode = 0;
    //only one case
    //0: case SDR2HDR

    //do decision
    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_Mode&0x80))
    {
        u8_mode = 0; //SDR2HDR

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    //assign by user
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_Mode), 0x01);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_Mode;
    }

    return u8_mode;

}

MS_U8 MS_Cfd_Kano_HDR_YCGAIN_OFFSET_In_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    MS_U8 u8_mode = 0;

    //0: OFF
    //1: limit to full
    //2: limit to full Conly
    //3: full to limit

    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_YCGAINOFFSET_In_Mode&0x80))
    {
        //for YUV limit , TMO
        if ((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && ( E_CFD_MC_FORMAT_RGB != pstu_Control_Param->u8Temp_DataFormat[0]) && ( E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[0]))
        {
            if (CFD_KANO_PATCH0 == 1)
            u8_mode = 2;
            else
            u8_mode = 1;
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_YCGAINOFFSET_In_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_YCGAINOFFSET_In_Mode), 0x04);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_YCGAINOFFSET_In_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Kano_HDR_YCGAIN_OFFSET_Out_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{

    MS_U8 u8_mode = 0;

    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_YCGAINOFFSET_Out_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_YCGAINOFFSET_Out_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        return pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_YCGAINOFFSET_Out_Mode;

        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_YCGAINOFFSET_Out_Mode), 0x01);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_YCGAINOFFSET_Out_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Kano_HDR_InputCSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    MS_U8 u8_mode = 0;

    //do decision
    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if (((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && (0x04 == (CFD_HDR_IP_CAPABILITY&0x04))) ||
            ((1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) && (0x02 == (CFD_HDR_IP_CAPABILITY&0x02))) ||
            ((1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) && (0x08 == (CFD_HDR_IP_CAPABILITY&0x08))) ||
            ((1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag) && (0x01 == (CFD_HDR_IP_CAPABILITY&0x01)))||
            ((E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            ((E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            ((E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            (1 == pstu_Control_Param->u8DoForceEnterHDRIP_Flag))
            {

                if (E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[0])
                {
                    if (0 == pstu_Control_Param->u8Input_IsRGBBypass)
                    {
                        //
                        if(E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[0])
                        {
                            u8_mode = E_CFD_IP_CSC_RLIMIT_TO_RFULL;
                        }
                        else
                        {
                            u8_mode = E_CFD_IP_CSC_OFF;
                        }
        }
                    else // u8Input_IsRGBBypass = 1
                    {
                        u8_mode = E_CFD_IP_CSC_OFF; //keep RGB
                    }
                }
                else if ( E_CFD_MC_FORMAT_RGB != pstu_Control_Param->u8Temp_DataFormat[0])
                {
                    if (1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
        {
            u8_mode = E_CFD_IP_CSC_YFULL_TO_RFULL;
                    }
            else
                    {
                        if(E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[0])
                        {
            u8_mode = E_CFD_IP_CSC_YLIMIT_TO_RFULL;
        }
                        else if(E_CFD_CFIO_RANGE_FULL== pstu_Control_Param->u8Temp_IsFullRange[0])
                        {
                            u8_mode = E_CFD_IP_CSC_YFULL_TO_RFULL;
                        }
                        else
                        {
                            u8_mode = E_CFD_IP_CSC_YFULL_TO_RFULL;
                        }
                    }
                }
            }
            else
            {
                u8_mode = E_CFD_IP_CSC_OFF;
            }
        }
        else
        {
            u8_mode = E_CFD_IP_CSC_OFF;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    //assign by user
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_Mode), 0x0D);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_Mode;
    }

    return u8_mode;
}

//based on pstu_Control_Param
MS_U8 MS_Cfd_Kano_HDR_RGBoffset_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{

    MS_U8 u8_mode = 0;

    //0: OFF
    //1: dither

    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGBOffset_Mode&0x80))
    {
        u8_mode = 1;

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGBOffset_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGBOffset_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGBOffset_Mode;
    }

    return u8_mode;
}

//based on pstu_Control_Param
MS_U8 MS_Cfd_Kano_HDR_RGBclip_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{

    MS_U8 u8_mode = 0;

    //0: OFF (clip in 0~1)
    //1: ON  (clip in -1~1)
    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGBClip_Mode&0x80))
    {
        if (( E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format ) || ( E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format ) || ( E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format ) )
        {
            u8_mode = 1;
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = 1;
        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGBClip_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGBClip_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGBClip_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Kano_HDR_LinearRGBBypass_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{

    //0: off
    //1: on

    MS_U8 u8_mode = 0;

    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_LinearRGBBypass_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_LinearRGBBypass_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_LinearRGBBypass_Mode), 0x0D);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_LinearRGBBypass_Mode;
    }

    return u8_mode;
}



MS_U8 MS_Cfd_Kano_HDR_Degamma_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{

    //0:off
    //1:on

    MS_U8 u8_mode = 0;

    //do decision
    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_enable_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if (((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && (0x04 == (CFD_HDR_IP_CAPABILITY&0x04))) ||
            ((1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) && (0x02 == (CFD_HDR_IP_CAPABILITY&0x02))) ||
            ((1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) && (0x08 == (CFD_HDR_IP_CAPABILITY&0x08))) ||
            ((1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag) && (0x01 == (CFD_HDR_IP_CAPABILITY&0x01)))||
            ((E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            ((E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            ((E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))))
            {
                u8_mode = 1;
        }
            else
        {
                //include pstu_Control_Param->u8DoFull2LimitInHDRIP_Flag = 1
                u8_mode = 0;
            }
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    //assign by user
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_enable_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_enable_Mode;
    }

    return u8_mode;

}

MS_U8 MS_Cfd_Kano_HDR_Degamma_dither_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{

    MS_U8 u8_mode = 0;
    //only one case
    //0: off
    //1: on

    //do decision
    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_Dither_Mode&0x80))
    {
        u8_mode = 1;

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_Dither_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    //assign by user
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_Dither_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_Dither_Mode;
    }

    return u8_mode;

}

MS_U8 MS_Cfd_Kano_HDR_Degamma_SRAM_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    MS_U8 u8_mode = 0;
    return u8_mode;
}

MS_U8 MS_Cfd_Kano_HDR_3x3_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{

    MS_U8 u8_mode = 0;

    //0: off
    //1: on

    //do decision
    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_enable_Mode&0x80))
    {
        //HDR to SDR
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if ((1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) && (0x02 == (CFD_HDR_IP_CAPABILITY&0x02)))
            {
                u8_mode = 1;
        }
            else if ((1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) && (0x08 == (CFD_HDR_IP_CAPABILITY&0x08)))
        {
                u8_mode = 1;
            }
            else
            {
                u8_mode = 0;
            }
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    //assign by user
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_enable_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_enable_Mode;
    }

    return u8_mode;

}

MS_U8 MS_Cfd_Kano_HDR_3x3_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    MS_U8 u8_mode = 0;

    //0:3x3(CP1=CP2)
    //1:3x3(CP1,CP2)
    //2:3x3(2020CLtoNCL)

    //do decision
    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_Mode&0x80))
    {
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if ((1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) && (0x02 == (CFD_HDR_IP_CAPABILITY&0x02)))
        {
            u8_mode = 1;
        }
            else if ((1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) && (0x08 == (CFD_HDR_IP_CAPABILITY&0x08)))
        {
            u8_mode = 2;
        }
        else
        {
            u8_mode = 0;
        }
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    //assign by user
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Kano_HDR_Compress_settings_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    MS_U8 u8_mode = 0;

    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Compress_settings_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Compress_settings_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Compress_settings_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Compress_settings_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Kano_HDR_Compress_dither_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    MS_U8 u8_mode = 0;

    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Compress_dither_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_ON();

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Compress_dither_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Compress_dither_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Compress_dither_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Kano_HDR_3x3Clip_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    MS_U8 u8_mode = 0;

    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3Clip_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3Clip_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3Clip_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3Clip_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Kano_HDR_Gamma_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    MS_U8 u8_mode = 0;

    //0 : on
    //1 : off

    //do decision
    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_enable_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if (((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && (0x04 == (CFD_HDR_IP_CAPABILITY&0x04))) ||
            ((1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) && (0x02 == (CFD_HDR_IP_CAPABILITY&0x02))) ||
            ((1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) && (0x08 == (CFD_HDR_IP_CAPABILITY&0x08))) ||
            ((1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag) && (0x01 == (CFD_HDR_IP_CAPABILITY&0x01)))||
            ((E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            ((E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            ((E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))))
            {
                u8_mode = 1;
        }
            else
        {
                //include pstu_Control_Param->u8DoFull2LimitInHDRIP_Flag = 1
                u8_mode = 0;
            }
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    //assign by user
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_enable_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Kano_HDR_Gamma_dither_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{

    MS_U8 u8_mode = 0;

    //0: off
    //1: on

    //do decision
    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_Dither_Mode&0x80))
    {
        u8_mode = 1;

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_Dither_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    //assign by user
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_Dither_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_Dither_Mode;
    }

    return u8_mode;

}

MS_U8 MS_Cfd_Kano_HDR_Gamma_maxdata_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    MS_U8 u8_mode = 0;

    //0: off
    //1: on

    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_maxdata_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_ON();

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_maxdata_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_maxdata_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_maxdata_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Kano_HDR_Gamma_SRAM_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    MS_U8 u8_mode = 0; //?
    return u8_mode;
}

MS_U8 MS_Cfd_Kano_HDR_outCSC_Mode_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
    MS_U8 u8_mode = 0;

    //do decision
    if (0x80 == (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_OutCSC_Mode&0x80))
    {

        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if (((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && (0x04 == (CFD_HDR_IP_CAPABILITY&0x04))) ||
            ((1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) && (0x02 == (CFD_HDR_IP_CAPABILITY&0x02))) ||
            ((1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) && (0x08 == (CFD_HDR_IP_CAPABILITY&0x08))) ||
            ((1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag) && (0x01 == (CFD_HDR_IP_CAPABILITY&0x01)))||
            ((E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            ((E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            ((E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format) && (0x10 == (CFD_HDR_IP_CAPABILITY&0x10))) ||
            (1 == pstu_Control_Param->u8DoForceEnterHDRIP_Flag))
            {
                if (0 == pstu_Control_Param->u8Input_IsRGBBypass)
        {
            u8_mode = E_CFD_IP_CSC_RFULL_TO_YFULL;
        }
                else
        {
                    u8_mode = E_CFD_IP_CSC_OFF;
                }
            }
            else
            {
                u8_mode = E_CFD_IP_CSC_OFF;
            }
        }
        else
        {
            u8_mode = E_CFD_IP_CSC_OFF;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_OutCSC_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    //assign by user
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_OutCSC_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_OutCSC_Mode;
    }

    return u8_mode;
}


MS_U8 MS_Cfd_Kano_SDR_IP2_CSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
    MS_U8 u8_mode = 0;

    //MS_U8 u8curr_DataFormat = pstu_Control_Param->u8Input_DataFormat;
    //MS_U8 u8curr_IsFullRange = pstu_Control_Param->u8Input_IsFullRange;
    //MS_U8 u8curr_IsRGBBypass = pstu_Control_Param->u8Input_IsRGBBypass;
    //MS_U8 u8curr_DoPathFullRange_Flag = pstu_Control_Param->u8DoPathFullRange_Flag;

    MS_U8 u8curr_DataFormat = pstu_Control_Param->u8Temp_DataFormat[1];
    MS_U8 u8curr_IsFullRange = pstu_Control_Param->u8Temp_IsFullRange[1];
    MS_U8 u8curr_IsRGBBypass = pstu_Control_Param->u8Input_IsRGBBypass;
    MS_U8 u8curr_DoPathFullRange_Flag = pstu_Control_Param->u8DoPathFullRange_Flag;

#if 0
    if(E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format ||E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format)
    {
        u8_mode = E_CFD_IP_CSC_OFF;
    }
#endif

    if (E_CFD_MC_FORMAT_RGB == u8curr_DataFormat)
    {
        if(E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange && 0 == u8curr_IsRGBBypass)
        {
            if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
            u8_mode = E_CFD_IP_CSC_RLIMIT_TO_YFULL;
            else
            u8_mode = E_CFD_IP_CSC_RLIMIT_TO_YLIMIT;
        }
        else if(E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange && 1 == u8curr_IsRGBBypass)
        {
            if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
            u8_mode = E_CFD_IP_CSC_RLIMIT_TO_RFULL;
            else
            u8_mode = E_CFD_IP_CSC_OFF;
        }
        else if(E_CFD_CFIO_RANGE_FULL== u8curr_IsFullRange && 0 == u8curr_IsRGBBypass)
        {
            if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
            u8_mode = E_CFD_IP_CSC_RFULL_TO_YFULL;
            else
            u8_mode = E_CFD_IP_CSC_RFULL_TO_YLIMIT;
        }
        else if(E_CFD_CFIO_RANGE_FULL== u8curr_IsFullRange && 1 == u8curr_IsRGBBypass)
        {
            if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
            u8_mode = E_CFD_IP_CSC_OFF;
            else
            u8_mode = E_CFD_IP_CSC_RFULL_TO_RLIMIT;
        }
        else
        {
            u8_mode = E_CFD_IP_CSC_OFF;
        }
    }
    else if ( E_CFD_MC_FORMAT_RGB != u8curr_DataFormat)
    {
            if(E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange && 0 == u8curr_IsRGBBypass)
            {
                if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
                u8_mode = E_CFD_IP_CSC_YLIMIT_TO_YFULL;
                else
                u8_mode = E_CFD_IP_CSC_OFF;
            }
            else if(E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange && 1 == u8curr_IsRGBBypass)
            {
                if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
                u8_mode = E_CFD_IP_CSC_YLIMIT_TO_YFULL;
                else
                u8_mode = E_CFD_IP_CSC_OFF;
            }
            else if(E_CFD_CFIO_RANGE_FULL== u8curr_IsFullRange && 0 == u8curr_IsRGBBypass)
            {
                if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
                u8_mode = E_CFD_IP_CSC_OFF;
                else
                u8_mode = E_CFD_IP_CSC_YFULL_TO_YLIMIT;
            }
            else if(E_CFD_CFIO_RANGE_FULL== u8curr_IsFullRange && 1 == u8curr_IsRGBBypass)
            {
                if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
                u8_mode = E_CFD_IP_CSC_OFF;
                else
                u8_mode = E_CFD_IP_CSC_YFULL_TO_YLIMIT;
            }
            else
            {
                u8_mode = E_CFD_IP_CSC_OFF;
            }
    }
    else
    {
        u8_mode = E_CFD_IP_CSC_OFF;
    }

    if(0x80 == (pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_Mode&0x80))
    {
        u8_mode = (pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_Mode), 0x0D);

        u8_mode = pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Kano_SDR_VIP_CM_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{

    MS_U8 u8_mode = 0;

    MS_U8 u8curr_DataFormat = pstu_Control_Param->u8Output_DataFormat;
    MS_U8 u8curr_IsFullRange = pstu_Control_Param->u8Output_IsFullRange;
    MS_U8 u8curr_IsRGBBypass = pstu_Control_Param->u8Input_IsRGBBypass;
    MS_U8 u8curr_DoPathFullRange_Flag = pstu_Control_Param->u8DoPathFullRange_Flag;

    if(E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange && (pstu_Control_Param->u8Output_Format <= E_CFD_CFIO_RESERVED_START && pstu_Control_Param->u8Output_Format >= E_CFD_CFIO_XVYCC_601))
    {
        printf("Error code =%d!!! This ouput format can't support limit range output [ %s  , %d]\n",E_CFD_MC_ERR_WRONGOUTPUTSOURCE, __FUNCTION__,__LINE__);
        //return E_CFD_MC_ERR_WRONGOUTPUTSOURCE;
        u8_mode = E_CFD_IP_CSC_OFF;
    }
    //else if ((E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[1]) &&  (1== pstu_Control_Param->u8Input_IsRGBBypass))
    //{
    //  u8_mode = E_CFD_IP_CSC_OFF;
    //}
    //any input & RGB limit out
    else if (E_CFD_MC_FORMAT_RGB == u8curr_DataFormat && E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange)
    {
        if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
        u8_mode = E_CFD_IP_CSC_YFULL_TO_RLIMIT;
        else
        u8_mode = E_CFD_IP_CSC_YLIMIT_TO_RLIMIT;
    }
    //RGB full out
    else if (E_CFD_MC_FORMAT_RGB == u8curr_DataFormat && E_CFD_CFIO_RANGE_FULL == u8curr_IsFullRange)
    {
        if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
        u8_mode = E_CFD_IP_CSC_YFULL_TO_RFULL;
        else
        u8_mode = E_CFD_IP_CSC_YLIMIT_TO_RFULL;
    }
    //YUV limit
    else if (E_CFD_MC_FORMAT_RGB != u8curr_DataFormat && E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange)
    {
        if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
        u8_mode = E_CFD_IP_CSC_YFULL_TO_YLIMIT;
        else
        u8_mode = E_CFD_IP_CSC_OFF;
    }
    //YUV full
    else if (E_CFD_MC_FORMAT_RGB != u8curr_DataFormat && E_CFD_CFIO_RANGE_FULL == u8curr_IsFullRange)
    {
        if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
        u8_mode = E_CFD_IP_CSC_OFF;
        else
        u8_mode = E_CFD_IP_CSC_YLIMIT_TO_YFULL;
    }
    else
    {
        u8_mode = E_CFD_IP_CSC_OFF;
    }

    if(0x80 == (pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_VIP_CM_Mode&0x80))
    {
        u8_mode = (pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_VIP_CM_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_VIP_CM_Mode), 0x0D);

        u8_mode = pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_VIP_CM_Mode;

    }

    return u8_mode;
}

MS_U8 MS_Cfd_Kano_SDR_Conv420_CSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
    MS_U8 u8_mode = 0;

    MS_U8 u8curr_DataFormat = pstu_Control_Param->u8Output_DataFormat;
    MS_U8 u8curr_IsFullRange = pstu_Control_Param->u8Output_IsFullRange;
    MS_U8 u8curr_IsRGBBypass = pstu_Control_Param->u8Input_IsRGBBypass;
    MS_U8 u8curr_DoPathFullRange_Flag = pstu_Control_Param->u8DoPathFullRange_Flag;

    if(E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange && (pstu_Control_Param->u8Output_Format <= E_CFD_CFIO_RESERVED_START && pstu_Control_Param->u8Output_Format >= E_CFD_CFIO_XVYCC_601))
    {
        printf("Error code =%d!!! This ouput format can't support limit range output [ %s  , %d]\n",E_CFD_MC_ERR_WRONGOUTPUTSOURCE, __FUNCTION__,__LINE__);
        //return E_CFD_MC_ERR_WRONGOUTPUTSOURCE;
        u8_mode = E_CFD_IP_CSC_OFF;
    }
    //else if ((E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[1]) &&  (1== pstu_Control_Param->u8Input_IsRGBBypass))
    //{
    //  u8_mode = E_CFD_IP_CSC_OFF;
    //}
    //any input & RGB limit out
    else if (E_CFD_MC_FORMAT_RGB == u8curr_DataFormat && E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange)
    {
        if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
        u8_mode = E_CFD_IP_CSC_YFULL_TO_RLIMIT;
        else
        u8_mode = E_CFD_IP_CSC_YLIMIT_TO_RLIMIT;
    }
    //RGB full out
    else if (E_CFD_MC_FORMAT_RGB == u8curr_DataFormat && E_CFD_CFIO_RANGE_FULL == u8curr_IsFullRange)
    {
        if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
        u8_mode = E_CFD_IP_CSC_YFULL_TO_RFULL;
        else
        u8_mode = E_CFD_IP_CSC_YLIMIT_TO_RFULL;
    }
    //YUV limit
    else if (E_CFD_MC_FORMAT_RGB != u8curr_DataFormat && E_CFD_CFIO_RANGE_LIMIT == u8curr_IsFullRange)
    {
        if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
        u8_mode = E_CFD_IP_CSC_YFULL_TO_YLIMIT;
        else
        u8_mode = E_CFD_IP_CSC_OFF;
    }
    //YUV full
    else if (E_CFD_MC_FORMAT_RGB != u8curr_DataFormat && E_CFD_CFIO_RANGE_FULL == u8curr_IsFullRange)
    {
        if (E_CFD_CFIO_RANGE_FULL == u8curr_DoPathFullRange_Flag)
        u8_mode = E_CFD_IP_CSC_OFF;
        else
        u8_mode = E_CFD_IP_CSC_YLIMIT_TO_YFULL;
    }
    else
    {
        u8_mode = E_CFD_IP_CSC_OFF;
    }

    if(0x80 == (pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_Mode&0x80))
    {
        u8_mode = (pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Kano_CheckModes(&(pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_Mode), 0x0D);

        u8_mode = pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_Mode;

    }

    return u8_mode;
}

MS_U8 MS_Cfd_Kano_SDR_HDMIRX_CSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
    return 0;
}

MS_U16 MS_Cfd_Kano_InterfaceControl(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Contol_Param,
    STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Param,
    STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param,
    STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param
    )
{

    //E_CFD_MC_ERR
    MS_U16 u16_check_status = TRUE;

#if 0
    //copy structure
    memcpy(&(pstu_HDRIP_Param->stu_Param),pstu_Contol_Param,sizeof(STU_CFD_MS_ALG_COLOR_FORMAT));
    memcpy(&(pstu_SDRIP_Param->stu_Param),pstu_Contol_Param,sizeof(STU_CFD_MS_ALG_COLOR_FORMAT));



    //YUVflag
    MS_U8 u8Input_YUV_Flag = 0;
    MS_U8 u8Output_YUV_Flag = 0;
    //0: RGB
    //1: YUV

    //for Kano
    //the output of pstu_HDRIP_Input is YUV full and colorimetry = output_format

    //update output of pstu_HDRIP_Input
    //update input of pstu_SDRIP_Input

    pstu_HDRIP_Param->stu_Param.u8Output_Format =



    //RGB in (full & limit)
    if (pstu_Contol_input->u8Input_DataFormat == E_CFD_MC_FORMAT_RGB) //RGB
    {
        //assign *pstu_TMO_Input
        //STU_CFD_MS_ALG_INTERFACE_TMO
        //pstu_TMO_Input->u8Controls = 0;

        //STU_CFD_MS_ALG_INTERFACE_HDRIP
        //pstu_HDRIP_Input->u8Controls = 0;

        //assign *pstu_SDRIP_Input
        //STU_CFD_MS_ALG_INTERFACE_SDRIP
        //pstu_SDRIP_Input->u8Controls = 1;

        pstu_SDRIP_Input->stu_Param.u8Input_Format      = pstu_Contol_input->u8Input_Format;
        pstu_SDRIP_Input->stu_Param.u8Input_IsFullRange = pstu_Contol_input->u8Input_IsFullRange;
        pstu_SDRIP_Input->stu_Param.u8InputColorPriamries = pstu_Contol_input->u8InputColorPriamries;
        pstu_SDRIP_Input->stu_Param.u8InputTransferCharacterstics = pstu_Contol_input->u8InputTransferCharacterstics;
        pstu_SDRIP_Input->stu_Param.u8InputMatrixCoefficients = pstu_Contol_input->u8InputMatrixCoefficients;

        //With Kano's design, no gamut mapping in SDR IP
        //force gamut keep the same
        pstu_SDRIP_Input->stu_Param.u8Output_Format     = pstu_Contol_input->u8Output_Format;
        pstu_SDRIP_Input->stu_Param.u8Output_IsFullRange = pstu_Contol_input->u8Output_IsFullRange;
        pstu_SDRIP_Input->stu_Param.u8OutputColorPriamries = pstu_Contol_input->u8OutputColorPriamries;
        pstu_SDRIP_Input->stu_Param.u8OutputTransferCharacterstics = pstu_Contol_input->u8OutputTransferCharacterstics;
        pstu_SDRIP_Input->stu_Param.u8OutputMatrixCoefficients = pstu_Contol_input->u8OutputMatrixCoefficients;

        if (1 == pstu_Contol_input->u8DoGamutMapping_Flag)
        {
            //u16_check_status = E_CFD_MC_ERR_PARTIALWRONG;
        }

        pstu_SDRIP_Input->stu_Param.u8DoGamutMapping_Flag = 0;
    }
    else //if (0 == pstu_Contol_input->u8InputISRGB) //YUV in (full & limit)
    {
        //assign *pstu_TMO_Input
        if (1 == pstu_Contol_input->u8DoTMO_Flag)
        {
            pstu_TMO_Input->u8Controls = 1;
        }
        else
        {
            pstu_TMO_Input->u8Controls = 0;
        }

        //?

        //assign *pstu_HDRIP_Input
        if ((1 == pstu_Contol_input->u8DoGamutMapping_Flag)||(1 == pstu_Contol_input->u8DoTMO_Flag))
        {
            pstu_HDRIP_Input->u8Controls = 1;

            pstu_HDRIP_Input->stu_Param.u8Input_Format      = pstu_Contol_input->u8Input_Format;
            pstu_HDRIP_Input->stu_Param.u8Input_IsFullRange = pstu_Contol_input->u8Input_IsFullRange;
            pstu_HDRIP_Input->stu_Param.u8InputColorPriamries = pstu_Contol_input->u8InputColorPriamries;
            pstu_HDRIP_Input->stu_Param.u8InputTransferCharacterstics = pstu_Contol_input->u8InputTransferCharacterstics;
            pstu_HDRIP_Input->stu_Param.u8InputMatrixCoefficients = pstu_Contol_input->u8InputMatrixCoefficients;

            //With Kano's design, no gamut mapping in SDR IP
            pstu_HDRIP_Input->stu_Param.u8Output_Format     = pstu_Contol_input->u8Output_Format;
            pstu_HDRIP_Input->stu_Param.u8Output_IsFullRange = pstu_Contol_input->u8Output_IsFullRange;
            pstu_HDRIP_Input->stu_Param.u8OutputColorPriamries = pstu_Contol_input->u8OutputColorPriamries;
            pstu_HDRIP_Input->stu_Param.u8OutputTransferCharacterstics = pstu_Contol_input->u8OutputTransferCharacterstics;
            pstu_HDRIP_Input->stu_Param.u8OutputMatrixCoefficients = pstu_Contol_input->u8OutputMatrixCoefficients;

        }
        else
        {
            pstu_HDRIP_Input->u8Controls = 0;
        }

        //assign *pstu_SDRIP_Input
        pstu_SDRIP_Input->u8Controls = 1;

        if (1 == pstu_HDRIP_Input->u8Controls)
        {
            pstu_SDRIP_Input->stu_Param.u8Input_Format      = pstu_Contol_input->u8Output_Format;
            pstu_SDRIP_Input->stu_Param.u8Input_IsFullRange = pstu_Contol_input->u8Output_IsFullRange;
            pstu_SDRIP_Input->stu_Param.u8InputColorPriamries = pstu_Contol_input->u8OutputColorPriamries;
            pstu_SDRIP_Input->stu_Param.u8InputTransferCharacterstics = pstu_Contol_input->u8OutputTransferCharacterstics;
            pstu_SDRIP_Input->stu_Param.u8InputMatrixCoefficients = pstu_Contol_input->u8OutputMatrixCoefficients;

            pstu_SDRIP_Input->stu_Param.u8Output_Format     = pstu_Contol_input->u8Output_Format;
            pstu_SDRIP_Input->stu_Param.u8Output_IsFullRange = pstu_Contol_input->u8Output_IsFullRange;
            pstu_SDRIP_Input->stu_Param.u8OutputColorPriamries = pstu_Contol_input->u8OutputColorPriamries;
            pstu_SDRIP_Input->stu_Param.u8OutputTransferCharacterstics = pstu_Contol_input->u8OutputTransferCharacterstics;
            pstu_SDRIP_Input->stu_Param.u8OutputMatrixCoefficients = pstu_Contol_input->u8OutputMatrixCoefficients;
        }
        else
        {
            pstu_SDRIP_Input->stu_Param.u8Input_Format      = pstu_Contol_input->u8Input_Format;
            pstu_SDRIP_Input->stu_Param.u8Input_IsFullRange = pstu_Contol_input->u8Input_IsFullRange;
            pstu_SDRIP_Input->stu_Param.u8InputColorPriamries = pstu_Contol_input->u8InputColorPriamries;
            pstu_SDRIP_Input->stu_Param.u8InputTransferCharacterstics = pstu_Contol_input->u8InputTransferCharacterstics;
            pstu_SDRIP_Input->stu_Param.u8InputMatrixCoefficients = pstu_Contol_input->u8InputMatrixCoefficients;

            pstu_SDRIP_Input->stu_Param.u8Output_Format     = pstu_Contol_input->u8Output_Format;
            pstu_SDRIP_Input->stu_Param.u8Output_IsFullRange = pstu_Contol_input->u8Output_IsFullRange;
            pstu_SDRIP_Input->stu_Param.u8OutputColorPriamries = pstu_Contol_input->u8OutputColorPriamries;
            pstu_SDRIP_Input->stu_Param.u8OutputTransferCharacterstics = pstu_Contol_input->u8OutputTransferCharacterstics;
            pstu_SDRIP_Input->stu_Param.u8OutputMatrixCoefficients = pstu_Contol_input->u8OutputMatrixCoefficients;

        }

    }

    //-------------------------------------------------------------------------------------
#endif
    return u16_check_status;
}



//
//@Param
//input :
//STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param
//STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input

//Output :
//temp[0] : for max
//temp[1] : for med
//temp[2] : for min
//temp[3] : updating flag for max/min
//need to update driver global variables or not

//temp[4] : updating flag for med
//need to update driver global variables or not


void MS_Cfd_TMO_parameter_update_source(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param ,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input, MS_U16 *u16temp)
{
    MS_U8 u8FindFlag;

    //0: match done
    //1: continue to match
    u8FindFlag = 0;

    //for source Max/Min

    //check auto/force
    if (0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserModeEn)
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode = 0;
    }

    //check this first
    if ((0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatus) && (0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode))
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatus = 0;
        u8FindFlag = 1;
    }
    else
    {
        if (0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode)
        {
            pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode = 1;
        }
        u8FindFlag = 0;
    }

    if ((1 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode) && (0 == u8FindFlag))
    {
        if (1 == pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_param_SetbyDriver)
        {
             pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatus = 1;
             u8FindFlag = 1;
        }
        else
        {
             pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode = 2;
             u8FindFlag = 0;
        }
    }

    if ((2 <= pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode) && (0 == u8FindFlag))
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatus = 2;
        u8FindFlag = 1;
    }

    //for source Med
    pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode = 1;
    u8FindFlag = 0;

    if ((1 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode) && (0 == u8FindFlag))
    {
        if (1 == pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_param_SetbyDriver)
        {
            pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatusM = 1;
            u8FindFlag = 1;
        }
        else
        {
            pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode = 2;
            u8FindFlag = 0;
        }
    }

    if ((2 <= pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceUserMode) && (0 == u8FindFlag))
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatusM = 2;
        u8FindFlag = 1;
    }

    if ((0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatus) || (2 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatus))
    {
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax;
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;

        u16temp[0] = pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax;
        u16temp[2] = pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMin;
        u16temp[3] = 1;

    }
    else if (1 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatus)
    {
        //no need to update
        //pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax = pstu_TMO_Input->stu_Kano_TMOIP_Param.u16HDR_TMO_Tmax;
        u16temp[3] = 0;
    }

    if (2 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_SourceStatusM)
    {
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmed      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;
        u16temp[1] = pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMed;
        u16temp[4] = 1;
    }
    else
    {
        u16temp[4] = 0;
    }
    //no need to update for 1
}


void MS_Cfd_TMO_parameter_update_target(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param ,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input, MS_U16 *u16temp)
{
    MS_U8 u8FindFlag;

    //0: match done
    //1: continue to match
    u8FindFlag = 0;

    //for source Max/Min

    //check auto/force
    if (0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserModeEn)
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode = 0;
    }

    //check this first
    if ((0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatus) && (0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode))
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatus = 0;
        u8FindFlag = 1;
    }
    else
    {
        if (0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode)
        {
            pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode = 1;
        }
        u8FindFlag = 0;
    }

    if ((1 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode) && (0 == u8FindFlag))
    {
        if (1 == pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_param_SetbyDriver)
        {
             pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatus = 1;
             u8FindFlag = 1;
        }
        else
        {
             pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode = 2;
             u8FindFlag = 0;
        }
    }

    if ((2 <= pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode) && (0 == u8FindFlag))
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatus = 2;
        u8FindFlag = 1;
    }

    //for Target Med
    pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode = 1;
    u8FindFlag = 0;

    if ((1 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode) && (0 == u8FindFlag))
    {
        if (1 == pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_param_SetbyDriver)
        {
            pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatusM = 1;
            u8FindFlag = 1;
        }
        else
        {
            pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode = 2;
            u8FindFlag = 0;
        }
    }

    if ((2 <= pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetUserMode) && (0 == u8FindFlag))
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatusM = 2;
        u8FindFlag = 1;
    }

    if ((0 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatus) || (2 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatus))
    {
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax;
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;

        u16temp[0] = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax;
        if ((pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin >= 1) && (pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMinFlag==0))
        {
            u16temp[2] = 10000;
        }
        else
        {
            u16temp[2] = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;
        }
        u16temp[3] = 1;

    }
    else if (1 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatus)
    {
        //no need to update
#if (NowHW == Kano)
        pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax = pstu_TMO_Input->stu_Kano_TMOIP_Param.u16HDR_TMO_Tmax;
#endif
        u16temp[3] = 0;
    }

    if (2 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetStatusM)
    {
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmed      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;
        u16temp[1] = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMed;
        u16temp[4] = 1;
    }
    else
    {
        u16temp[4] = 0;
    }
    //no need to update for 1
}

#if (0 == RealChip)
    StuDlc_HDRinit g_HDRinitParameters;
#endif

MS_U16 MS_Cfd_Kano_TMO_Control(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    //E_CFD_MC_ERR
    MS_U8 u8_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_ReturnStatus = 0;
    MS_BOOL bFuncEn;
    MS_BOOL bRegWriteEn;

    MS_U8 u8Mode;

    MS_U16 u16temp[5] = {0};

    u8Mode = 0;

    //ST_HDR_UVC_Kano stUVC;
    //memset(&stUVC,0x00,sizeof(ST_HDR_UVC_Kano));

    //if pstu_TMO_Param->u8TMO_TargetMode = 1;

    if (0 == pstu_Control_Param->u8Input_HDRIPMode)
    {
        function1(pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_UVC_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
    }
    else
    {
        pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_enable_Mode = MS_Cfd_Kano_HDR_TMO_curve_enable_Decision(pstu_Control_Param,pstu_TMO_Input);
        pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_Mode = MS_Cfd_Kano_HDR_TMO_Curve_Mode_Decision(pstu_Control_Param,pstu_TMO_Input);
        pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_UVC_Mode = MS_Cfd_Kano_HDR_UVC_Decision(pstu_Control_Param,pstu_TMO_Input);
    }
    //produce TMO curves...

#if 0 //there is no autodolwnload for the TMO HW in kano
#if ((CFD_SW_VERSION >= 0x00000014) && (CFD_SW_RETURN_ST_VERSION >= 1))

    if (0x00 != FunctionMode(pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_enable_Mode))
    {
        pstu_Control_Param->u8VideoADFlag = 1;
    }

#endif
#endif

    u16_check_status = Mapi_Cfd_Kano_TMOIP_Param_Check(&(pstu_TMO_Input->stu_Kano_TMOIP_Param));

//only in C model
//define a fake TMO structure
#if (0 == RealChip)

    memset(&g_HDRinitParameters,0x00,sizeof(StuDlc_HDRinit));
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smin = 60;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smed = 120;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smax = 4000;

    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin = 3000;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmed = 120;
    g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax = 600;

#endif

#if RealChip
    //g_DlcParameters.u8Dlc_Mode = pstu_TMO_Input->stu_Manhattan_TMOIP_Param.u8HDR_TMO_curve_Mode;

    if (1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
    {
        //printf("1 TMO control TgtMax:%d\n",pstu_TMO_Input->stu_CFD_TMO_Param.u16TgtMax);
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMinFlag;
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMaxFlag = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMaxFlag;
    }

    //if (1 == g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode)
    {
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMin = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;  // 0.05
        //g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMax = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax; // 300
        //g_HDRinitParameters.DLC_HDRToneMappingData.u16Smin =  pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMin;
        //g_HDRinitParameters.DLC_HDRToneMappingData.u16Smax = pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax;
        //printf("\033[1;35m###[Brian][%s][%d]### old u16TgtMax = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmax);
        //printf("\033[1;35m###[Brian][%s][%d]### old u16TgtMin = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmin);

        //backward compatibility
        if (2 > pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetRefer_Mode)
        {
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax;
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMinFlag;
        }
        else if (2 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetRefer_Mode)
        {
            MS_Cfd_TMO_parameter_update_source(pstu_Control_Param ,pstu_TMO_Input, u16temp);

            if (1 == u16temp[3])
            {
                g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smax = u16temp[0];
                g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smin = u16temp[2];
            }

            if (1 == u16temp[4])
            {
                g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smed = u16temp[1];
            }

            //for target
            MS_Cfd_TMO_parameter_update_target(pstu_Control_Param ,pstu_TMO_Input, u16temp);

            g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag = 1;
            if (1 == u16temp[3])
            {
                g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax = u16temp[0];
                g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin = u16temp[2];
            }

            if (1 == u16temp[4])
            {
                g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmed = u16temp[1];
            }

#if (1 == RealChip)
            u16_ReturnStatus = MS_Cfd_TMO_parameter_Check_Update(&g_HDRinitParameters);
#endif 

            //update for Kano
#if (NowHW == Kano)

            //when the check fails, set the default value to pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax
            if ((u16_ReturnStatus&0x02) != 0x00)
            {
                pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax = 600;
            }

#endif

        } //else if (2 == pstu_Control_Param->stu_CFD_TMO_Param.u8TMO_TargetRefer_Mode)
        else
        {
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax;
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin      = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMinFlag;
        }

        //pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_param_SetbyDriver
        //printf("\033[1;35m###[Brian][%s][%d]### u8HDR_TMO_param_SetbyDriver = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_param_SetbyDriver);
        //printf("\033[1;35m###[Brian][%s][%d]### u16_ReturnStatus = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,u16_ReturnStatus);
        //printf("\033[1;35m###[Brian][%s][%d]### u16Tmax = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax);
        //printf("\033[1;35m###[Brian][%s][%d]### u16Tmed = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmed);
        //printf("\033[1;35m###[Brian][%s][%d]### u16Tmin = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin);

        //printf("\033[1;35m###[Brian][%s][%d]### u16Smax = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smax);
        //printf("\033[1;35m###[Brian][%s][%d]### u16Smed = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smed);
        //printf("\033[1;35m###[Brian][%s][%d]### u16Smin = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smin);

        if (E_CFIO_MODE_HDR2 == pstu_Control_Param->u8Temp_HDRMode[0]) //PQ uses TMO configures from CFD values
        {
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode = 1;
        }
        else if (E_CFIO_MODE_HDR3 == pstu_Control_Param->u8Temp_HDRMode[0]) //HLG uses TMO configures in TMO
        {
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode = 2;
        }
        else //use TMO default values
        {
            g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode = 0;
        }

        //printf("\033[1;35m###[Brian][%s][%d]### new u16Tmax = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax);
        //printf("\033[1;35m###[Brian][%s][%d]### new u16Tmin = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin);
        //printf("\033[1;35m###[Brian][%s][%d]### pstu_Control_Param->u8Temp_HDRMode[0] = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,pstu_Control_Param->u8Temp_HDRMode[0]);
        //printf("\033[1;35m###[Brian][%s][%d]### g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode] = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode);
        //g_HDRinitParameters.DLC_HDRToneMappingData.u16Smed = pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMed;
        //g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmed = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMed;
    }
#endif


    u8Mode = FunctionMode(pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_Mode);
#if RealChip
    g_DlcParameters.u8Tmo_Mode = u8Mode;
    u8Mhal_xc_TMO_mode = u8Mode;
    //printf("\033[1;35m###[Brian][%s][%d]### g_DlcParameters.u8Tmo_Mode = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,g_DlcParameters.u8Tmo_Mode);
    //printf("\033[1;35m###[Brian][%s][%d]### u8Mhal_xc_TMO_mode = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,u8Mhal_xc_TMO_mode);
#endif


    //bFuncEn =     FunctionMode(pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_enable_Mode);
    //bRegWriteEn = WriteRegsiterFlag(pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_setting_Mode);
    //u8Mode =      FunctionMode(pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_setting_Mode);
    u8Mode = FunctionMode(pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_TMO_curve_enable_Mode);
    u8_check_status = Kano_TMOIP_TMO_enable_write(u8Mode,E_MAIN_WINDOW);

    //u8HDR_UVC_Mode
    u8Mode = FunctionMode(pstu_TMO_Input->stu_Kano_TMOIP_Param.u8HDR_UVC_Mode);
    u8_check_status = Kano_TMOIP_UVC_write(u8Mode,E_MAIN_WINDOW);

    if(0 == u8_check_status)
    {
        printf("Error code =%d!!!u8HDR_LinearRGBBypass_Mode Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    ///*/

    return u16_check_status;
}

void MS_CFD_HDRIP_EOTF_Ext_function(ST_DEGAMMA_PARAMETERS_EXTENSION_IN* pt_degamma_extension_in, STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{

    MS_U16 u16temp;

//for PQ clamp function
#if (1 == DePQClamp_EN)

    pt_degamma_extension_in->dePQclamp_en = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.st_degamma_extension.dePQclamp_en;
    u16temp = MS_Cfd_Luminance_To_PQCode(pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax, 0);
    //printf("\033[1;35m###[Brian][%s][%d]### u16SourceMax = %d!!!!!\033[0m\n",__FUNCTION__,__LINE__,pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax);
    pt_degamma_extension_in->dePQclamp_value = ((MS_U32)u16temp*0xff00)/(0x3ff);

#else

    //give maximum values
    pt_degamma_extension_in->dePQclamp_en    = 0x00;
    pt_degamma_extension_in->dePQclamp_value = 0xff00;

#endif

    //new process for HDRx to HDRy
    if (0 != pstu_Control_Param->u8DoHDRXtoHDRY_Flag)
    {
        //HLG to HDR PQ
        if (( E_CFIO_MODE_HDR3 == pstu_Control_Param->u8Temp_HDRMode[0]) && ( E_CFIO_MODE_HDR2 == pstu_Control_Param->u8Temp_HDRMode[1]))
        {
            pt_degamma_extension_in->u8eotf_nor_en = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.st_degamma_extension.u8deHLG_eotf_nor_en;

#if 0
    #if (1 == RealChip)
            MS_U8 u8byte0, u8byte1;
            u8byte0 = msReadByte( _PK_L_(0x79, 0x7c));
            pt_degamma_extension_in->u16deHLG_eotf_nor_lum_value = ((MS_U16)u8byte0)*100;
    #endif
#else
            pt_degamma_extension_in->u16eotf_nor_lum_value = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.st_degamma_extension.u16deHLG_eotf_nor_lum_value;
#endif

        }
        //HDR PQ to HDR HLG is not ready
    }
    else if (0 != pstu_Control_Param->u8DoSDRtoHDR_Flag) //new process for SDR2HDR
    {
        //SDR to HDR PQ
        if (( E_CFIO_MODE_SDR == pstu_Control_Param->u8Temp_HDRMode[0]) && ( E_CFIO_MODE_HDR2 == pstu_Control_Param->u8Temp_HDRMode[1]))
        {
            pt_degamma_extension_in->u8eotf_nor_en = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.st_degamma_extension.u8SDR2PQ_eotf_nor_en;
#if 0
    #if (1 == RealChip)
            MS_U8 u8byte0, u8byte1;
            u8byte0 = msReadByte( _PK_L_(0x79, 0x7c));
            pt_degamma_extension_in->u16eotf_nor_lum_value = ((MS_U16)u8byte0)*100;
    #endif
#else
            pt_degamma_extension_in->u16eotf_nor_lum_value = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.st_degamma_extension.u16SDR2PQ_eotf_nor_lum_value;
#endif


        }
        //HDR PQ to HDR HLG is not ready
    }
    else
    {
        pt_degamma_extension_in->u8eotf_nor_en = 0;
        pt_degamma_extension_in->u16eotf_nor_lum_value = 0;
    }

    #if 0
            printf("\033[1;35m###[Brian][%s][%d]### dePQclamp_en = %d!!!!!\033[0m\n",__FUNCTION__,__LINE__,pt_degamma_extension_in->dePQclamp_en);
            printf("\033[1;35m###[Brian][%s][%d]### dePQclamp_value = %d!!!!!\033[0m\n",__FUNCTION__,__LINE__,pt_degamma_extension_in->dePQclamp_value);
            printf("\033[1;35m###[Brian][%s][%d]### u8eotf_nor_en = %d!!!!!\033[0m\n",__FUNCTION__,__LINE__,pt_degamma_extension_in->u8eotf_nor_en);
            printf("\033[1;35m###[Brian][%s][%d]### u16eotf_nor_lum_value = %d!!!!!\033[0m\n",__FUNCTION__,__LINE__,pt_degamma_extension_in->u16eotf_nor_lum_value);
    #endif

}

MS_U16 MS_Cfd_Kano_HDRIP_WriteRegister(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    MS_U8 u8CurrentIP_Vars = 0,u8CurrentIP_Vars2 = 0;
    MS_U16 u16Max_luma_codes = 0;
    MS_U8 u8GammutMode = 0;
    MS_U8 u8IPindex = 0,u8IPmode = 0;
    MS_BOOL bFuncEn,bRegWriteEn;
    MS_U8 u8Mode;
    MS_U8 u8Input_format;
    MS_U8 u8IPtype;
    MS_U16 u16temp;

#if RealChip
    //u8IPindex = msReadByte(REG_SC_BK30_0E_H);
    //u8IPmode = msReadByte(REG_SC_BK30_0E_L);
#endif

    MS_U8 u8_check_status = 0;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;

    //for HDR InputCSC
    //use user-defined paramter from api, not from decision tree
    if (1 == pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_MC;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempMatrixCoefficients[0];
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_MC = pstu_Control_Param->u8TempMatrixCoefficients[0];
    }

//#if (CFD_KANO_Force_Inside_MC709 == 1)
#if (0)
    if (E_CFD_CFIO_MC_BT2020CL != pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_MC)
    {
        u8CurrentIP_Vars = 1;
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_MC = 1;
    }
#endif

    //for test only =======================================================================================
    //pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_Mode = E_CFD_IP_CSC_YFULL_TO_RFULL;
    //pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_Mode = E_CFD_IP_CSC_RLIMIT_TO_RFULL;
    //u8CurrentIP_Vars = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    //pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_Ratio1 = 0x40;
    //=====================================================================================================

    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_Mode);

    //u8Mode = E_CFD_IP_CSC_RLIMIT_TO_RFULL;

    u8IPtype = E_CSC_KANO_HDR_INPUT;

    //pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_Ratio1 = 0x80; //2x

    u8_check_status = CSC(u8Mode,u8CurrentIP_Vars,pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_Ratio1,3,1,u8IPtype);

    if (u8_check_status == 0)
    {
        printf("Error code =%d!!! HDR InputCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }

    //for HDR degamma
    if (1 == pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_DeGamma_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_TR;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempTransferCharacterstics[0];
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_TR = pstu_Control_Param->u8TempTransferCharacterstics[0];
    }


    //max luma codes
    //MS_U16 u16Max_luma_codes;
    //u16Max_luma_codes = MS_Cfd_Luminance_To_PQCode(pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax, pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMaxFlag);

    u16Max_luma_codes = MS_Cfd_Luminance_To_PQCode(pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax, 0);

    //use constant value now
    //u16Max_luma_codes = MS_Cfd_Luminance_To_PQCode(600, 0);
    //pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_Ratio1 = 0x80;

    //MS_U32 u32Max_luma_codes;
    u16Max_luma_codes = ((MS_U32)u16Max_luma_codes*0xff00)/(0x3ff);
    if (u16Max_luma_codes > 0xff00)
    {
        u16Max_luma_codes = 0xff00;
    }
    pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u16HDR_Degamma_Max_Lum = u16Max_luma_codes;

    //for test only =======================================================================================
    //u8CurrentIP_Vars = E_CFD_CFIO_TR_SMPTE2084;
    //pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_Ratio1 = 0x80;
    //pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u16HDR_Degamma_Ratio2 = 0x40;
    //pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_Lut_En = 0x00;
    //=====================================================================================================
    //degamma function should modify
    u8IPtype = E_DEGAMMA_KANO_NORMAL_HDRIP;
    u8Input_format = pstu_Control_Param->u8Temp_Format[0];
    //turn off first
    Kano_HDRIP_DeGamma_enable(0,E_MAIN_WINDOW);
    //#if 0

    //param
    //HDR u8HDR_IP_enable_Mode start//
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_IP_enable_Mode);
    Kano_HDRIP_HDRIP_enable(u8Mode,E_MAIN_WINDOW);
    //HDR u8HDR_IP_enable_Mode end//

    ST_DEGAMMA_PARAMETERS_EXTENSION_IN st_degamma_extension_in;
    ST_DEGAMMA_PARAMETERS_EXTENSION_IN* pt_degamma_extension_in = &st_degamma_extension_in;

    memset(pt_degamma_extension_in, 0x00, sizeof(ST_DEGAMMA_PARAMETERS_EXTENSION_IN));

    MS_CFD_HDRIP_EOTF_Ext_function(pt_degamma_extension_in, pstu_Control_Param, pstu_HDRIP_Param);

#if (1 == RealChip)
    u8HLGUseLUT_Flag = 0;
#endif

    u8_check_status = deGamma(u8CurrentIP_Vars,u16Max_luma_codes,
                    pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_Ratio1,
                    pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u16HDR_Degamma_Ratio2,
                    pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_Lut_En,
                    pstu_HDRIP_Param->stu_Kano_HDRIP_Param.pu32HDR_Degamma_Lut_Address,
                    pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u16HDR_Degamma_Lut_Length,
                    u8IPtype,u8Input_format,2,pt_degamma_extension_in);
    //#endif
    if(0 == u8_check_status)
    {
        printf("Error code =%d!!! HDR Degamma Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }

    //3x3
    if (1 == pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_InputCP;
        u8CurrentIP_Vars2 = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_OutputCP;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempColorPriamries[0];
        u8CurrentIP_Vars2 = pstu_Control_Param->u8TempColorPriamries[1];
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_InputCP = pstu_Control_Param->u8TempColorPriamries[0];
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_OutputCP = pstu_Control_Param->u8TempColorPriamries[1];
    }


    if (E_CFD_CFIO_CP_PANEL == u8CurrentIP_Vars2)
    {
        u8GammutMode = 0;
    }
    else
    {
        u8GammutMode = 1;
    }

    //only for test
    //u8CurrentIP_Vars = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    //u8CurrentIP_Vars2 = E_CFD_CFIO_CP_BT2020;

    //E_GAMUT_MAPPING_KANO_HDRIP
    u8IPtype = E_GAMUT_MAPPING_KANO_HDRIP;
    u8Input_format = pstu_Control_Param->u8Temp_Format[0];
    u8_check_status = gamutMapping(u8CurrentIP_Vars, u8GammutMode,
                           //&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.pstu_Panel_Param->stu_Cfd_Panel_ColorMetry) ,
                           pstu_Control_Param->pstu_Panel_Param_Colorimetry,
                           u8CurrentIP_Vars2,u8Input_format,
                           pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u16HDR_3x3_Ratio2,u8IPtype,1);

    if(0 == u8_check_status)
    {
        printf("Error code =%d!!!GamutMapping Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }

    //u8_check_status = gamutMapping(u8CurrentIP_Vars, u8GammutMode, &(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.pstu_Panel_Param->stu_Cfd_Panel_ColorMetry) , u8CurrentIP_Vars2, pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_Ratio,1);

    //for HDR gamma
    if (1 == pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_TR;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempTransferCharacterstics[1];
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_TR = pstu_Control_Param->u8TempTransferCharacterstics[1];
    }

    u16Max_luma_codes = 0xff00;

    //if (u8CurrentIP_Vars == 16)
    //{
    //  u8CurrentIP_Vars = 6;
    //}

    u8CurrentIP_Vars = 1;

    //E_GAMMA_KANO_NORMAL_HDRIP
    u8IPtype = E_GAMMA_KANO_NORMAL_HDRIP;
    u8Input_format = pstu_Control_Param->u8Temp_Format[1];

    //need to modify
    Kano_HDRIP_Gamma_enable(0,E_MAIN_WINDOW);
    u8_check_status = gamma(u8CurrentIP_Vars,u16Max_luma_codes,0,0,
                    pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_Lut_En,
                    pstu_HDRIP_Param->stu_Kano_HDRIP_Param.pu32HDR_Gamma_Lut_Address,
                    pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u16HDR_Gamma_Lut_Length,
                    u8IPtype,u8Input_format,2);

    if(0 == u8_check_status)
    {
        printf("Error code =%d!!!Gamma Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }

#if 0
        //printf("\033[1;35m###[Brian][%s][%d]### In gamma !!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__);

        write2ByteMask(0x102F00, 0xFFFF, 0x0040,E_IP);
        write2ByteMask(_PK_L_kano(0x40,0x79), 0x00FF, 0x0000,E_IP);
        //burst write
        //write2ByteMask(_PK_L_kano(0x40,0x78), 0xFFFF, 0x00E0,E_IP);

        //normal write
        //write2ByteMask(_PK_L_kano(0x40,0x78), 0xFFFF, 0x0060,E_IP);

        MS_U16 u16Index =0;
        MS_U16 u16Byte =0;

        //aa++;

        for( u16Index = 0; u16Index <256; u16Index++)
        //for( u16Index = 0; u16Index <8; u16Index++)
        {

         //address
            write2ByteMask(_PK_L_kano(0x40,0x79), 0x00FF, u16Index,E_IP);
         write2ByteMask(_PK_L_kano(0x40,0x78), 0x0060, 0x0040,E_IP);
            write2ByteMask(_PK_L_kano(0x40,0x78),0x0010, 0x0010,E_IP); // read enable

            u16Byte = MApi_GFLIP_XC_R2BYTE(0x1340f4);
         //if (aa == 1)
         {
         printf("%3d : out  %04x\n",u16Index, u16Byte);
         }

        }

        write2ByteMask(_PK_L_kano(0x40,0x78), 0xFFFF, 0x0000,E_IP);

#endif


    //for OutputCSC
    //MS_U8 outputCSC(MS_U8 mode,MS_U8 matrixCoefficents,MS_U8 ratio)
    if (1 == pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_OutCSC_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_OutCSC_MC;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempMatrixCoefficients[1];
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_OutCSC_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
    }

#if 0
#if (CFD_KANO_Force_Inside_MC709 == 1)
    u8CurrentIP_Vars = 1;
    pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_OutCSC_MC = 1;
#endif
#endif

    //for test only =======================================================================================
    //pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_Mode = E_CFD_IP_CSC_RFULL_TO_YFULL;
    //pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_Mode = E_CFD_IP_CSC_RLIMIT_TO_RFULL;
    //u8CurrentIP_Vars = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    //pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_Ratio = 0x40;
    //=====================================================================================================

    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_OutCSC_Mode);

    u8IPtype = E_CSC_KANO_HDR_OUTPUT;

    //u8CurrentIP_Vars = E_CFD_CFIO_MC_BT2020NCL;

    u8_check_status = CSC(u8Mode,u8CurrentIP_Vars,pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_OutCSC_Ratio1,3,1,u8IPtype);

    if(0 == u8_check_status)
    {
        printf("Error code =%d!!!OutCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }

    //Multi param
    //u8HDR_YCGAINOFFSET_In_Mode
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_YCGAINOFFSET_In_Mode);
    Kano_HDRIP_YCGAINOFFSET_In(u8Mode,E_MAIN_WINDOW);

    //Multi param
    //u8HDR_YCGAINOFFSET_Out_Mode
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_YCGAINOFFSET_Out_Mode);
    Kano_HDRIP_YCGAINOFFSET_Out(u8Mode,E_MAIN_WINDOW);

    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGBOffset_Mode);
    u8_check_status = Kano_HDRIP_rgbOffset(u8Mode,E_MAIN_WINDOW);

    //Multi param
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGBClip_Mode);
    u8_check_status = Kano_HDRIP_rgbClip(u8Mode,E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printf("Error code =%d!!!HDR u8HDR_RGBClip_Mode Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }

    //1 param
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_enable_Mode);
    u8_check_status = Kano_HDRIP_DeGamma_enable(u8Mode,E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printf("Error code =%d!!!u8HDR_Degamma_enable_Mode Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }

    //1 param
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_Dither_Mode);
    u8_check_status = Kano_HDRIP_DeGammaDither(u8Mode,E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printf("Error code =%d!!!u8HDR_Degamma_Dither_Mode Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }

    //Multi param
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Compress_settings_Mode);
    u8_check_status = Kano_HDRIP_RgbComp(u8Mode,E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printf("Error code =%d!!!u8HDR_Compress_settings_Mode Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }

    //1 param
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Compress_dither_Mode);
    u8_check_status = Kano_HDRIP_RgbCompDither(u8Mode,E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printf("Error code =%d!!!u8HDR_Compress_dither_Mode Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }

    //Multi param
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3Clip_Mode);
    u8_check_status = Kano_HDRIP_linearRgbClip(u8Mode,E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printf("Error code =%d!!!u8HDR_3x3Clip_Mode Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }

    //1 param
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_enable_Mode);
    u8_check_status = Kano_HDRIP_linearRgb3x3_enable(u8Mode,E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printf("Error code =%d!!!u8HDR_3x3_enable_Mode Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }

    //1 param
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_enable_Mode);
    u8_check_status = Kano_HDRIP_Gamma_enable(u8Mode,E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printf("Error code =%d!!!u8HDR_Gamma_enable_Mode Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }

    //1 param
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_Dither_Mode);
    u8_check_status = Kano_HDRIP_GammaDither(u8Mode,E_MAIN_WINDOW);
        if(0 == u8_check_status)
    {
        printf("Error code =%d!!!u8HDR_Gamma_Dither_Mode Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }

    //multi param
    //need to add :u8HDR_Gamma_maxdata_Mode
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_LinearRGBBypass_Mode);
    u8_check_status = Kano_HDRIP_linearRgbBypass(u8Mode,E_MAIN_WINDOW);

    if(0 == u8_check_status)
    {
        printf("Error code =%d!!!u8HDR_LinearRGBBypass_Mode Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }

    //1param
    //HDR u8HDR_IP_PATH_Mode start//
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_IP_PATH_Mode);
    Kano_HDRIP_HDRIP_path(u8Mode,E_MAIN_WINDOW);
    //HDR u8HDR_IP_PATH_Mode end//

     //1 param
    //HDR u8HDR_IP_enable_Mode start//
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_IP_enable_Mode);
    Kano_HDRIP_HDRIP_enable(u8Mode,E_MAIN_WINDOW);
    //HDR u8HDR_IP_enable_Mode end//

    //1 param
    //HDR u8HDR_RGB3D_PATH_Mode start//
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_PATH_Mode);
    Kano_HDRIP_RGB3D_path(u8Mode,E_MAIN_WINDOW);
    //HDR u8HDR_RGB3D_PATH_Mode end//

    //write Kano 3D LUT
    //parameters : u8HDR_RGB3D_Mode
    //hard code
    //some settings needs to write
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_Mode);
    //Kano_HDRIP_RGB3D_settings(u8Mode,E_MAIN_WINDOW);

    //note : enable function should be assigned after configure function
    //1 param
    //HDR u8HDR_RGB3D_enable_Mode start//
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_enable_Mode);
    Kano_HDRIP_RGB3D_enable(u8Mode,E_MAIN_WINDOW);
    //HDR u8HDR_RGB3D_enable_Mode end//



    return u16_check_status;
}


MS_U16 MS_Cfd_Kano_HDRIP_WriteRegister_DebugMode(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;

    return u16_check_status;
}


MS_U16 MS_Cfd_Kano_HDRIP_Control(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    //E_CFD_MC_ERR
    MS_U8 u8_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_place = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status_tmp = E_CFD_MC_ERR_NOERR;
    MS_U8  u8ERR_Happens_Flag = 0;
    MS_U8 u8CurrentIP_Vars = 0;
    MS_U8 u8CurrentIP_Vars2 = 0;
    MS_U8 u8temp = 0;
    MS_U8 u8GammutMode = 0;
    MS_U8 u8IPindex = 0xFF,u8IPmode = 0;

    //not test mode
    if (0 == pstu_Control_Param->u8Input_HDRIPMode)
    {
        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_IP_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());

        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_IP_PATH_Mode = MS_Cfd_Kano_HDR_IP_PATH_Decision(pstu_Control_Param,pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_enable_Mode = MS_Cfd_Kano_HDR_RGB3D_enable_Decision(pstu_Control_Param,pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_PATH_Mode = MS_Cfd_Kano_HDR_RGB3D_path_Decision(pstu_Control_Param,pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_Mode = MS_Cfd_Kano_HDR_RGB3D_Mode_Decision(pstu_Control_Param,pstu_HDRIP_Param);

        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_YCGAINOFFSET_In_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_YCGAINOFFSET_Out_Mode,MS_Cfd_OnlyEnable_Decision_OFF());

        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_Mode,MS_Cfd_OnlyEnable_Decision_OFF());

        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_LinearRGBBypass_Mode,MS_Cfd_OnlyEnable_Decision_ON());
        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGBOffset_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGBClip_Mode,MS_Cfd_OnlyEnable_Decision_OFF());

        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_Dither_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_SRAM_Mode,0);

        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_Mode,MS_Cfd_OnlyEnable_Decision_OFF());

        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Compress_settings_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Compress_dither_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3Clip_Mode,MS_Cfd_OnlyEnable_Decision_OFF());

        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_maxdata_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_Dither_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_SRAM_Mode,0);
        function1(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_OutCSC_Mode,MS_Cfd_OnlyEnable_Decision_OFF());

    }
    //normal mode
    else
    {
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_IP_enable_Mode = MS_Cfd_Kano_HDR_Path_enable_Decision(pstu_Control_Param,pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_IP_PATH_Mode = MS_Cfd_Kano_HDR_IP_PATH_Decision(pstu_Control_Param,pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_enable_Mode = MS_Cfd_Kano_HDR_RGB3D_enable_Decision(pstu_Control_Param,pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_PATH_Mode = MS_Cfd_Kano_HDR_RGB3D_path_Decision(pstu_Control_Param,pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGB3D_Mode = MS_Cfd_Kano_HDR_RGB3D_Mode_Decision(pstu_Control_Param,pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_YCGAINOFFSET_In_Mode = MS_Cfd_Kano_HDR_YCGAIN_OFFSET_In_Decision(pstu_Control_Param,pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_YCGAINOFFSET_Out_Mode = MS_Cfd_Kano_HDR_YCGAIN_OFFSET_Out_Decision(pstu_Control_Param,pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_InputCSC_Mode          = MS_Cfd_Kano_HDR_InputCSC_Decision(pstu_Control_Param,pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_LinearRGBBypass_Mode = MS_Cfd_Kano_HDR_LinearRGBBypass_Decision(pstu_Control_Param,pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGBOffset_Mode         = MS_Cfd_Kano_HDR_RGBoffset_Decision(pstu_Control_Param,pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_RGBClip_Mode           = MS_Cfd_Kano_HDR_RGBclip_Decision(pstu_Control_Param,pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_enable_Mode    = MS_Cfd_Kano_HDR_Degamma_enable_Decision(pstu_Control_Param,pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_Dither_Mode    = MS_Cfd_Kano_HDR_Degamma_dither_Decision(pstu_Control_Param,pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_SRAM_Mode      = MS_Cfd_Kano_HDR_Degamma_enable_Decision(pstu_Control_Param,pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_enable_Mode = MS_Cfd_Kano_HDR_3x3_enable_Decision(pstu_Control_Param,pstu_HDRIP_Param); //
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3_Mode = MS_Cfd_Kano_HDR_3x3_Decision(pstu_Control_Param,pstu_HDRIP_Param); //

        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Compress_settings_Mode = MS_Cfd_Kano_HDR_Compress_settings_Decision(pstu_Control_Param,pstu_HDRIP_Param); //
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Compress_dither_Mode = MS_Cfd_Kano_HDR_Compress_dither_Decision(pstu_Control_Param,pstu_HDRIP_Param); //
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_3x3Clip_Mode = MS_Cfd_Kano_HDR_3x3Clip_Decision(pstu_Control_Param,pstu_HDRIP_Param); //

        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_enable_Mode = MS_Cfd_Kano_HDR_Gamma_enable_Decision(pstu_Control_Param,pstu_HDRIP_Param); //
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_maxdata_Mode = MS_Cfd_Kano_HDR_Gamma_maxdata_Decision(pstu_Control_Param,pstu_HDRIP_Param); //
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_Dither_Mode = MS_Cfd_Kano_HDR_Gamma_dither_Decision(pstu_Control_Param,pstu_HDRIP_Param); //
        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_SRAM_Mode        = MS_Cfd_Kano_HDR_Gamma_enable_Decision(pstu_Control_Param,pstu_HDRIP_Param); //

        pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_OutCSC_Mode = MS_Cfd_Kano_HDR_outCSC_Mode_Decision(pstu_Control_Param,pstu_HDRIP_Param); //
    }

#if ((CFD_SW_VERSION >= 0x00000014) && (CFD_SW_RETURN_ST_VERSION >= 1))

    if (0x00 != FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Degamma_enable_Mode))
    {
        pstu_Control_Param->u8VideoADFlag = 1;
    }

    if (0x00 != FunctionMode(pstu_HDRIP_Param->stu_Kano_HDRIP_Param.u8HDR_Gamma_enable_Mode))
    {
        pstu_Control_Param->u8VideoADFlag = 1;
    }

#endif


    u16_check_status_tmp = Mapi_Cfd_Kano_HDRIP_Param_Check(&(pstu_HDRIP_Param->stu_Kano_HDRIP_Param));

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }


    // Input to Physical Layer by Ali Start
    if(u8IPindex ==0xFF)
    {
        u16_check_status_tmp = MS_Cfd_Kano_HDRIP_WriteRegister(pstu_Control_Param,pstu_HDRIP_Param,pstu_TMO_Input);
    }
    else
    {
        u16_check_status_tmp = MS_Cfd_Kano_HDRIP_WriteRegister_DebugMode(pstu_Control_Param,pstu_HDRIP_Param,pstu_TMO_Input);
    }
    // Input to Physical Layer by Ali End

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

    return u16_check_status;
}

MS_U16 MS_Cfd_Kano_SDRIP_WriteRegister(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{

    MS_U8 u8CurrentIP_Vars = 0,u8CurrentIP_Vars2 = 0;
    MS_U16 u16Max_luma_codes = 0;
    MS_U8 u8GammutMode = 0;
    MS_U8 u8IPindex = 0,u8IPmode = 0;
#if RealChip
    u8IPindex = msReadByte(REG_SC_BK30_0E_H);
    u8IPmode = msReadByte(REG_SC_BK30_0E_L);
#endif
    MS_U8 u8_check_status = 0;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;


    if (1 == pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_MC;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempMatrixCoefficients[1];
        pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
    }

#if 0 //(CFD_KANO_Force_Inside_MC709 == 1)
    u8CurrentIP_Vars = 1;
    pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_MC = 1;
#endif

    u8_check_status = CSC((pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_Mode&MaskForMode_LB),u8CurrentIP_Vars,pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_Ratio1,3,1,E_CSC_MANHATTAN_INPUT);

    if(0 == u8_check_status)
    {
        printf("Error code =%d!!! InputCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    //inputCSC End

    //VIP off
    //pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_VIP_CM_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
    //write2ByteMask(0x102F00, 0xFFFF, 0x002F);
    //write2ByteMask(_PK_L_kano(0x2F,0x70), 0x0001, 0x00);
    //default is off

#if 1 //not control now
    //outputCSC start
    if (1 == pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_VIP_CM_Manual_Vars_en)
    {
        u8CurrentIP_Vars2 = pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_VIP_CM_MC;
    }
    else
    {
        u8CurrentIP_Vars2 = pstu_Control_Param->u8TempMatrixCoefficients[1];
        pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_VIP_CM_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
    }

#if (CFD_KANO_Force_Inside_MC709 == 1)
    u8CurrentIP_Vars2 = 1;
    pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_VIP_CM_MC = 1;
#endif

    u8_check_status = CSC((pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_VIP_CM_Mode&MaskForMode_LB),u8CurrentIP_Vars2,pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_VIP_CM_Ratio1,3,1,E_CSC_KANO_CONV420_CM);

    if(0 == u8_check_status)
    {
        printf("Error code =%d!!! OutCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    //outputCSC end
#endif



#if 0 //not control now
    //outputCSC start
    if (1 == pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_Manual_Vars_en)
    {
        u8CurrentIP_Vars2 = pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_MC;
    }
    else
    {
        u8CurrentIP_Vars2 = pstu_Control_Param->u8TempMatrixCoefficients[1];
        pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
    }

#if (CFD_KANO_Force_Inside_MC709 == 1)
    u8CurrentIP_Vars = 2;
    pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_MC = 1;
#endif

    u8_check_status = CSC((pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_Mode&MaskForMode_LB),u8CurrentIP_Vars2,pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_Ratio1,3,1,E_CSC_KANO_CONV420_CM);

    if(0 == u8_check_status)
    {
        printf("Error code =%d!!! OutCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    //outputCSC end
#endif

    return u16_check_status;
}

MS_U16 MS_Cfd_Kano_SDRIP_WriteRegister_Sub(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    return 0;
}

MS_U16 MS_Cfd_Kano_SDRIP_WriteRegister_DebugMode(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input,MS_U8 u8MainSubMode)
{

    MS_U8 u8CurrentIP_Vars = 0,u8CurrentIP_Vars2 = 0;
    MS_U16 u16Max_luma_codes = 0;
    MS_U8 u8GammutMode = 0;
    MS_U8 u8IPindex = 0,u8IPmode = 0;
#if RealChip
    u8IPindex = msReadByte(REG_SC_BK30_0E_H);
    u8IPmode = msReadByte(REG_SC_BK30_0E_L);
#endif
    MS_U8 u8_check_status = 0;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;

    //VIP off
    pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_VIP_CM_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];

    if(u8IPindex == 1)
    {
        //inputCSC start

        if (1 == pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_Manual_Vars_en)
        {
            u8CurrentIP_Vars = pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_MC;
        }
        else
        {
            u8CurrentIP_Vars = pstu_Control_Param->u8TempMatrixCoefficients[1];
            pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
        }

#if (CFD_KANO_Force_Inside_MC709 == 1)
    u8CurrentIP_Vars = 1;
    pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_MC = 1;
#endif


        //if(u8IPmode == 0xFF)
        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_Mode = u8IPmode;
        }

        if(0 == u8MainSubMode)
        {
            u8_check_status = CSC((pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_Mode&MaskForMode_LB),u8CurrentIP_Vars,pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_Ratio1,3,1,E_CSC_MANHATTAN_INPUT);
        }
        else
        {
            u8_check_status = CSC((pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_Mode&MaskForMode_LB),u8CurrentIP_Vars,pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_Ratio1,3,1,E_CSC_KANO_IP2_OUTPUT_SUB);
        }

        if(0 == u8_check_status)
        {
            printf("Error code =%d!!! InputCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
        //inputCSC End
    }
    else if(u8IPindex == 2)
    {
        //outputCSC start
        if (1 == pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_Manual_Vars_en)
        {
            u8CurrentIP_Vars2 = pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_MC;
        }
        else
        {
            u8CurrentIP_Vars2 = pstu_Control_Param->u8TempMatrixCoefficients[1];
            pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
        }

        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_Mode = u8IPmode;
        }

        //no sub for this IP
        //if(0 == u8MainSubMode)
        {
            u8_check_status = CSC((pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_Mode&MaskForMode_LB),u8CurrentIP_Vars2,pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_Ratio1,3,1,E_CSC_MANHATTAN_OUTPUT);
        }
        //else
        //{
        //   u8_check_status = CSC((pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_Mode&MaskForMode_LB),u8CurrentIP_Vars2,pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_Ratio1,3,1,E_CSC_MASERATI_OUTPUT_SUB);
        //}

        if(0 == u8_check_status)
        {
            printf("Error code =%d!!! OutCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
        //outputCSC end
    }

    return u16_check_status;
}

MS_U16 MS_Cfd_Kano_SDRIP_Control(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param, STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input,MS_U8 u8MainSubMode)
{
    //E_CFD_MC_ERR
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status_tmp = E_CFD_MC_ERR_NOERR;
    MS_U8  u8ERR_Happens_Flag = 0;
    MS_U8 u8IPindex = 0xFF,u8IPmode = 0;

    pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_Mode = MS_Cfd_Kano_SDR_IP2_CSC_Decision(pstu_Control_Param,pstu_SDRIP_Param); //
    pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_VIP_CM_Mode = MS_Cfd_Kano_SDR_VIP_CM_Decision(pstu_Control_Param,pstu_SDRIP_Param); //

    //not control now
    //pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_Conv420_CM_Mode = MS_Cfd_Kano_SDR_Conv420_CSC_Decision(pstu_Control_Param,pstu_SDRIP_Param); //

    //quick bypass control
    //only not control Output CSC
    if ((0 == pstu_Control_Param->u8Input_SDRIPMode) || (2 == pstu_Control_Param->u8Input_SDRIPMode))
    {
        //the final CSC
        if (0 == pstu_Control_Param->u8Input_SDRIPMode)
        {
            function1(pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_VIP_CM_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        }

        function1(pstu_SDRIP_Param->stu_Kano_SDRIP_Param.u8SDR_IP2_CSC_Mode,0x00);
    }

    u16_check_status_tmp = Mapi_Cfd_Kano_SDRIP_Param_Check(&(pstu_SDRIP_Param->stu_Kano_SDRIP_Param));

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

#if RealChip
    u8IPindex = msReadByte(REG_SC_BK30_0E_H);
    u8IPmode = msReadByte(REG_SC_BK30_0E_L);
#endif

    // Input to Physical Layer by Ali Start
#if 1
    if(u8IPindex ==0xFF)
    {
        if(0 == u8MainSubMode)
        {
            u16_check_status_tmp = MS_Cfd_Kano_SDRIP_WriteRegister(pstu_Control_Param,pstu_SDRIP_Param,pstu_TMO_Input);
        }
        else
        {
            u16_check_status_tmp = MS_Cfd_Kano_SDRIP_WriteRegister_Sub(pstu_Control_Param,pstu_SDRIP_Param,pstu_TMO_Input);
        }
    }
    else
    {
        u16_check_status_tmp = MS_Cfd_Kano_SDRIP_WriteRegister_DebugMode(pstu_Control_Param,pstu_SDRIP_Param,pstu_TMO_Input,u8MainSubMode);
    }
    // Input to Physical Layer by Ali End
#endif

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

    //not test mode
    return u16_check_status;
}

//#endif



