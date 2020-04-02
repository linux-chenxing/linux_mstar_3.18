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
#include "mhal_xc.h"
#include "color_format_input.h"
#include "color_format_driver.h"
#include "color_format_driver_hardware.h"
#include "color_format_ip.h"
#include "mhal_dlc.h"

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
MS_U32 adobeRgbMod[257] = {
        0x0, 0xc, 0x19, 0x26, 0x32, 0x3e, 0x4b, 0x57, 0x64, 0x70, 0x7c, 0x89, 0x95, 0xa2, 0xaf, 0xbb,
        0xc7, 0xd4, 0xe0, 0xed, 0xf9, 0x10a, 0x135, 0x14f, 0x174, 0x195, 0x1b6, 0x1d6, 0x1f9, 0x222, 0x24c, 0x278,
        0x2a5, 0x2d5, 0x306, 0x339, 0x36e, 0x3a4, 0x3dd, 0x417, 0x453, 0x490, 0x4d0, 0x511, 0x555, 0x59a, 0x5e1, 0x62a,
        0x675, 0x6c1, 0x710, 0x760, 0x7b3, 0x807, 0x85d, 0x8b5, 0x90f, 0x96c, 0x9ca, 0xa2a, 0xa8c, 0xaef, 0xb55, 0xbbd,
        0xc27, 0xc93, 0xd01, 0xd71, 0xde3, 0xe57, 0xecd, 0xf45, 0xfbf, 0x103b, 0x10b9, 0x113a, 0x11bc, 0x1240, 0x12c7, 0x1350,
        0x13da, 0x1467, 0x14f6, 0x1587, 0x161a, 0x16af, 0x1746, 0x17e0, 0x187b, 0x1919, 0x19b9, 0x1a5b, 0x1aff, 0x1ba5, 0x1c4e, 0x1cf8,
        0x1da5, 0x1e54, 0x1f05, 0x1fb8, 0x206e, 0x2126, 0x21df, 0x229c, 0x235a, 0x241a, 0x24dd, 0x25a2, 0x2669, 0x2732, 0x27fe, 0x28cc,
        0x299c, 0x2a6e, 0x2b43, 0x2c19, 0x2cf2, 0x2dce, 0x2eab, 0x2f8b, 0x306d, 0x3151, 0x3238, 0x3321, 0x340c, 0x34f9, 0x35e9, 0x36db,
        0x37d0, 0x38c6, 0x39bf, 0x3aba, 0x3bb8, 0x3cb8, 0x3dba, 0x3ebe, 0x3fc5, 0x40ce, 0x41da, 0x42e8, 0x43f8, 0x450a, 0x461f, 0x4736,
        0x4850, 0x496c, 0x4a8a, 0x4bab, 0x4cce, 0x4df3, 0x4f1b, 0x5045, 0x5171, 0x52a0, 0x53d1, 0x5505, 0x563b, 0x5774, 0x58ae, 0x59ec,
        0x5b2b, 0x5c6d, 0x5db2, 0x5ef8, 0x6042, 0x618d, 0x62db, 0x642c, 0x657f, 0x66d4, 0x682c, 0x6986, 0x6ae3, 0x6c42, 0x6da3, 0x6f07,
        0x706e, 0x71d7, 0x7342, 0x74b0, 0x7620, 0x7793, 0x7908, 0x7a7f, 0x7bf9, 0x7d76, 0x7ef5, 0x8077, 0x81fb, 0x8381, 0x850a, 0x8696,
        0x8823, 0x89b4, 0x8b47, 0x8cdc, 0x8e74, 0x900f, 0x91ac, 0x934b, 0x94ed, 0x9692, 0x9839, 0x99e2, 0x9b8e, 0x9d3d, 0x9eee, 0xa0a1,
        0xa258, 0xa410, 0xa5cb, 0xa789, 0xa949, 0xab0c, 0xacd2, 0xae9a, 0xb064, 0xb231, 0xb401, 0xb5d3, 0xb7a8, 0xb97f, 0xbb59, 0xbd35,
        0xbf14, 0xc0f6, 0xc2da, 0xc4c0, 0xc6aa, 0xc896, 0xca84, 0xcc75, 0xce69, 0xd05f, 0xd258, 0xd453, 0xd651, 0xd852, 0xda55, 0xdc5a,
        0xde63, 0xe06e, 0xe27b, 0xe48c, 0xe69e, 0xe8b4, 0xeacc, 0xece7, 0xef04, 0xf124, 0xf346, 0xf56b, 0xf793, 0xf9be, 0xfbeb, 0xfe1a,
        0x1004d, };
extern StuDlc_HDRinit g_HDRinitParameters;
extern StuDlc_FinetuneParamaters g_DlcParameters;

//#if NowHW == Maserati
//#include "Color_map_driver_lib_Maserati.h"
//#endif

MS_U16 MS_Cfd_Maserati_CheckModes(MS_U8 *temp, MS_U8 mode_upbound)
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

void MS_Cfd_Maserati_HDRIP_Debug(STU_CFDAPI_Maserati_TMOIP *pstu_Maserati_TMO_Param, STU_CFDAPI_Maserati_HDRIP *pstu_Maserati_HDRIP_Param)
{
    printk("u8TMO_curve_enable_Mode                 :0x%02x\n", pstu_Maserati_TMO_Param->u8HDR_TMO_curve_enable_Mode);
    printk("u8TMO_curve_Mode                        :0x%02x\n", pstu_Maserati_TMO_Param->u8HDR_TMO_curve_Mode );
    printk("u8TMO_curve_setting_Mode                :0x%02x\n", pstu_Maserati_TMO_Param->u8HDR_TMO_curve_setting_Mode );
    printk("u8HDR_UVC_setting_Mode                  :0x%02x\n", pstu_Maserati_TMO_Param->u8HDR_UVC_setting_Mode );
    printk("\n");
    printk("u8HDR_enable_Mode                       :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_enable_Mode);
    printk("u8HDR_Composer_Mode                     :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_Composer_Mode);
    printk("u8HDR_Module1_enable_Mode               :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_Module1_enable_Mode);
    printk("\n");
    printk("u8HDR_InputCSC_Mode                     :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_Mode);
    printk("u8HDR_InputCSC_Ratio1                   :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_Ratio1);
    printk("u8HDR_InputCSC_Manual_Vars_en           :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_Manual_Vars_en);
    printk("u8HDR_InputCSC_MC                       :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_MC);
    printk("\n");
    printk("u8HDR_Degamma_SRAM_Mode                 :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_Degamma_SRAM_Mode);
    printk("u8HDR_Degamma_Ratio1                    :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_Degamma_Ratio1);
    printk("u16HDR_Degamma_Ratio2                   :0x%04x\n",pstu_Maserati_HDRIP_Param->u16HDR_Degamma_Ratio2);
    printk("u8HDR_DeGamma_Manual_Vars_en            :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_DeGamma_Manual_Vars_en);
    printk("u8HDR_Degamma_TR                        :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_Degamma_TR);
    printk("u8HDR_Degamma_Lut_En                    :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_Degamma_Lut_En);
    printk("pu32HDR_Degamma_Lut_Address             :0x%08x\n",pstu_Maserati_HDRIP_Param->pu32HDR_Degamma_Lut_Address);
    printk("u16HDR_Degamma_Lut_Length               :0x%04x\n",pstu_Maserati_HDRIP_Param->u16HDR_Degamma_Lut_Length);
    printk("u8DHDR_Degamma_Max_Lum_En               :0x%02x\n",pstu_Maserati_HDRIP_Param->u8DHDR_Degamma_Max_Lum_En);
    printk("u16HDR_Degamma_Max_Lum                  :0x%04x\n",pstu_Maserati_HDRIP_Param->u16HDR_Degamma_Max_Lum);
    printk("\n");
    printk("u8HDR_3x3_Mode                          :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_3x3_Mode);
    printk("u16HDR_3x3_Ratio2                       :0x%02x\n",pstu_Maserati_HDRIP_Param->u16HDR_3x3_Ratio2);
    printk("u8HDR_3x3_Manual_Vars_en                :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_3x3_Manual_Vars_en);
    printk("u8HDR_3x3_InputCP                       :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_3x3_InputCP);
    printk("u8HDR_3x3_OutputCP                      :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_3x3_OutputCP);
    printk("\n");
    printk("u8HDR_Gamma_SRAM_Mode                   :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_Gamma_SRAM_Mode);
    printk("u8HDR_Gamma_Manual_Vars_en              :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_Gamma_Manual_Vars_en);
    printk("u8HDR_Gamma_TR                          :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_Gamma_TR);
    printk("u8HDR_Gamma_Lut_En                      :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_Gamma_Lut_En);
    printk("pu32HDR_Gamma_Lut_Address               :0x%08x\n",pstu_Maserati_HDRIP_Param->pu32HDR_Gamma_Lut_Address);
    printk("u16HDR_Gamma_Lut_Length                 :0x%04x\n",pstu_Maserati_HDRIP_Param->u16HDR_Gamma_Lut_Length);
    printk("\n");
    printk("u8HDR_OutputCSC_Mode                    :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_Mode);
    printk("u8HDR_OutputCSC_Ratio1                  :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_Ratio1);
    printk("u8HDR_OutputCSC_Manual_Vars_en          :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_Manual_Vars_en);
    printk("u8HDR_OutputCSC_MC                      :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_MC);
    printk("\n");
    printk("u8HDR_Yoffset_Mode                      :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_Yoffset_Mode);
    printk("\n");
    printk("u8HDR_NLM_enable_Mode                   :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_NLM_enable_Mode);
    printk("u8HDR_NLM_setting_Mode                  :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_NLM_setting_Mode);
    printk("u8HDR_ACGain_enable_Mode                :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_ACGain_enable_Mode);
    printk("u8HDR_ACGain_setting_Mode               :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_ACGain_setting_Mode);
    printk("u8HDR_ACE_enable_Mode                   :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_ACE_enable_Mode);
    printk("u8HDR_ACE_setting_Mode                  :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_ACE_setting_Mode);
    printk("u8HDR_Dither1_setting_Mode              :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_Dither1_setting_Mode);
    printk("\n");
    printk("u8HDR_3DLUT_enable_Mode                 :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_3DLUT_enable_Mode);
    printk("u8HDR_3DLUT_SRAM_Mode                   :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_3DLUT_SRAM_Mode);
    printk("u8HDR_3DLUT_setting_Mode                :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_3DLUT_setting_Mode);
    printk("u8HDR_444to422_enable_Mode              :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_444to422_enable_Mode);
    printk("u8HDR_Dither2_enable_Mode               :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_Dither2_enable_Mode);
    printk("u8HDR_Dither2_setting_Mode              :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDR_Dither2_setting_Mode);
    printk("\n");
    printk("u8HDRIP_Patch                           :0x%02x\n",pstu_Maserati_HDRIP_Param->u8HDRIP_Patch);
    printk("\n");
}

void MS_Cfd_Maserati_SDRIP_Debug(STU_CFDAPI_Maserati_DLCIP *pstu_Maserati_DLC_Param, STU_CFDAPI_Maserati_SDRIP *pstu_Maserati_SDRIP_Param)
{

    printk("u8DLC_curve_Mode                        :0x%02x\n", pstu_Maserati_DLC_Param->u8DLC_curve_Mode         );
    printk("u8DLC_curve_enable_Mode                 :0x%02x\n", pstu_Maserati_DLC_Param->u8DLC_curve_enable_Mode  );
    printk("\n");
    printk("u8IP2_CSC_Mode                          :0x%02x\n",pstu_Maserati_SDRIP_Param->u8IP2_CSC_Mode                    );
    printk("u8IP2_CSC_Ratio1                        :0x%02x\n",pstu_Maserati_SDRIP_Param->u8IP2_CSC_Ratio1                  );
    printk("u8IP2_CSC_Manual_Vars_en                :0x%02x\n",pstu_Maserati_SDRIP_Param->u8IP2_CSC_Manual_Vars_en          );
    printk("u8IP2_CSC_MC                            :0x%02x\n",pstu_Maserati_SDRIP_Param->u8IP2_CSC_MC                      );
    printk("\n");
    printk("u8UFSC_YCOffset_Gain_Mode               :0x%02x\n",pstu_Maserati_SDRIP_Param->u8UFSC_YCOffset_Gain_Mode         );
    printk("\n");
    printk("u8VIP_CSC_Mode                          :0x%02x\n",pstu_Maserati_SDRIP_Param->u8VIP_CSC_Mode          );
    printk("u8VIP_PreYoffset_Mode                   :0x%02x\n",pstu_Maserati_SDRIP_Param->u8VIP_PreYoffset_Mode          );
    printk("u8VIP_PreYgain_Mode                     :0x%02x\n",pstu_Maserati_SDRIP_Param->u8VIP_PreYgain_Mode          );
    printk("u8VIP_PreYgain_dither_Mode              :0x%02x\n",pstu_Maserati_SDRIP_Param->u8VIP_PreYgain_dither_Mode          );
    printk("u8VIP_PostYoffset_Mode                  :0x%02x\n",pstu_Maserati_SDRIP_Param->u8VIP_PostYoffset_Mode          );
    printk("u8VIP_PostYgain_Mode                    :0x%02x\n",pstu_Maserati_SDRIP_Param->u8VIP_PostYgain_Mode          );
    printk("u8VIP_PostYoffset2_Mode                 :0x%02x\n",pstu_Maserati_SDRIP_Param->u8VIP_PostYoffset2_Mode          );
    printk("\n");
    printk("u8VOP_3x3_Mode                          :0x%02x\n",pstu_Maserati_SDRIP_Param->u8VOP_3x3_Mode                    );
    printk("u8VOP_3x3_Ratio1                        :0x%02x\n",pstu_Maserati_SDRIP_Param->u8VOP_3x3_Ratio1                  );
    printk("u8VOP_3x3_Manual_Vars_en                :0x%02x\n",pstu_Maserati_SDRIP_Param->u8VOP_3x3_Manual_Vars_en          );
    printk("u8VOP_3x3_MC                            :0x%02x\n",pstu_Maserati_SDRIP_Param->u8VOP_3x3_MC                      );

    printk("u8VOP_3x3RGBClip_Mode                   :0x%02x\n",pstu_Maserati_SDRIP_Param->u8VOP_3x3RGBClip_Mode             );
    printk("u8LinearRGBBypass_Mode                  :0x%02x\n",pstu_Maserati_SDRIP_Param->u8LinearRGBBypass_Mode                  );

    printk("u8Degamma_enable_Mode                   :0x%02x\n",pstu_Maserati_SDRIP_Param->u8Degamma_enable_Mode             );
    printk("u8Degamma_Dither_Mode                   :0x%02x\n",pstu_Maserati_SDRIP_Param->u8Degamma_Dither_Mode             );
    printk("u8Degamma_SRAM_Mode                     :0x%02x\n",pstu_Maserati_SDRIP_Param->u8Degamma_SRAM_Mode                    );
    printk("u8Degamma_Ratio1                        :0x%02x\n",pstu_Maserati_SDRIP_Param->u8Degamma_Ratio1                  );
    printk("u16Degamma_Ratio2                       :0x%04x\n",pstu_Maserati_SDRIP_Param->u16Degamma_Ratio2                 );

    printk("u8DeGamma_Manual_Vars_en                :0x%02x\n",pstu_Maserati_SDRIP_Param->u8DeGamma_Manual_Vars_en          );
    printk("u8Degamma_TR                            :0x%02x\n",pstu_Maserati_SDRIP_Param->u8Degamma_TR                      );

    printk("u8Degamma_Lut_En                        :0x%02x\n",pstu_Maserati_SDRIP_Param->u8Degamma_Lut_En                  );
    printk("pu32Degamma_Lut_Address                 :0x%08x\n",pstu_Maserati_SDRIP_Param->pu32Degamma_Lut_Address             );
    printk("u16Degamma_Lut_Length                   :0x%04x\n",pstu_Maserati_SDRIP_Param->u16Degamma_Lut_Length             );

    printk("u8Degamma_Max_Lum_En                    :0x%02x\n",pstu_Maserati_SDRIP_Param->u8Degamma_Max_Lum_En                 );
    printk("u16Degamma_Max_Lum                      :0x%04x\n",pstu_Maserati_SDRIP_Param->u16Degamma_Max_Lum                 );

    printk("u83x3_enable_Mode                       :0x%02x\n",pstu_Maserati_SDRIP_Param->u83x3_enable_Mode                        );
    printk("u83x3_Mode                              :0x%02x\n",pstu_Maserati_SDRIP_Param->u83x3_Mode                        );
    printk("u163x3_Ratio2                           :0x%02x\n",pstu_Maserati_SDRIP_Param->u163x3_Ratio2                      );
    printk("u83x3_Manual_Vars_en                    :0x%02x\n",pstu_Maserati_SDRIP_Param->u83x3_Manual_Vars_en              );
    printk("u83x3_InputCP                           :0x%02x\n",pstu_Maserati_SDRIP_Param->u83x3_InputCP                     );
    printk("u83x3_OutputCP                          :0x%02x\n",pstu_Maserati_SDRIP_Param->u83x3_OutputCP                    );

    printk("u8Compress_settings_Mode                :0x%02x\n",pstu_Maserati_SDRIP_Param->u8Compress_settings_Mode          );
    printk("u8Compress_dither_Mode                  :0x%02x\n",pstu_Maserati_SDRIP_Param->u8Compress_dither_Mode            );
    printk("u83x3Clip_Mode                          :0x%02x\n",pstu_Maserati_SDRIP_Param->u83x3Clip_Mode                    );

    printk("u8Gamma_enable_Mode                     :0x%02x\n",pstu_Maserati_SDRIP_Param->u8Gamma_enable_Mode               );
    printk("u8Gamma_Dither_Mode                     :0x%02x\n",pstu_Maserati_SDRIP_Param->u8Gamma_Dither_Mode               );
    printk("u8Gamma_maxdata_Mode                    :0x%02x\n",pstu_Maserati_SDRIP_Param->u8Gamma_maxdata_Mode              );
    printk("u8Gamma_SRAM_Mode                       :0x%02x\n",pstu_Maserati_SDRIP_Param->u8Gamma_SRAM_Mode                      );

    printk("u8Gamma_Mode_Vars_en                    :0x%02x\n",pstu_Maserati_SDRIP_Param->u8Gamma_Mode_Vars_en       );
    printk("u8Gamma_TR                              :0x%02x\n",pstu_Maserati_SDRIP_Param->u8Gamma_TR                        );
    printk("u8Gamma_Lut_En                          :0x%02x\n",pstu_Maserati_SDRIP_Param->u8Gamma_Lut_En                    );
    printk("pu32Gamma_Lut_Address                   :0x%08x\n",pstu_Maserati_SDRIP_Param->pu32Gamma_Lut_Address             );
    printk("u16Gamma_Lut_Length                     :0x%04x\n",pstu_Maserati_SDRIP_Param->u16Gamma_Lut_Length               );

    printk("u8YHSL_R2Y_Mode                     :0x%02x\n",pstu_Maserati_SDRIP_Param->u8YHSL_R2Y_Mode               );
    printk("\n");
}

#if 0
void Restore_input_bank_and_log_Maserati(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param)
{

#if RealChip
    msDlc_FunctionEnter();
    if(1 == GetDlcFunctionControl())
    {
        //msWriteByte(REG_SC_Ali_BK30_01_L,pstApiControlParamInit->u8Process_Mode );//0x1     ;     //0:off 1:on - normal mode 2:on - test mode
        //pstControlParamInit->u8HW_Structure                          = ;//E_CFD_HWS_STB_TYPE1 ;     //assign by E_CFD_MC_HW_STRUCTURE
        //pstControlParamInit->u8HW_PatchEn                            = ;//0x1     ;     //0: patch off, 1:patch on
        //msWriteByte(REG_SC_Ali_BK30_01_H,pstApiControlParamInit->u8Input_Source   );// ;     //assign by E_CFD_MC_SOURCE
        //msWriteByte(REG_SC_Ali_BK30_02_L,pstApiControlParamInit->u8Input_AnalogIdx   );   //assign by E_CFD_INPUT_ANALOG_FORMAT
        //msWriteByte(REG_SC_Ali_BK30_02_H,pstApiControlParamInit->u8Input_Format);  //assign by E_CFD_CFIO
        //msWriteByte(REG_SC_Ali_BK30_03_L,pstApiControlParamInit->u8Input_DataFormat );//assign by E_CFD_MC_FORMAT
        //msWriteByte(REG_SC_Ali_BK30_03_H,pstApiControlParamInit->u8Input_IsFullRange);//assign by E_CFD_CFIO_RANGE
        //msWriteByte(REG_SC_Ali_BK30_04_L,pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_HDRMode);//assign by E_CFIO_HDR_STATUS
        msWriteByte(REG_SC_Ali_BK30_04_H,pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_IsRGBBypass);//0:no R2Y for RGB in;1:R2Y for RGB in
        msWriteByte(REG_SC_Ali_BK30_05_L,pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_Source);//assign by E_CFD_MC_SOURCE
        msWriteByte(REG_SC_Ali_BK30_05_H,pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_Format); //assign by E_CFD_CFIO
        msWriteByte(REG_SC_Ali_BK30_06_L,pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_DataFormat); //assign by E_CFD_MC_FORMAT
        msWriteByte(REG_SC_Ali_BK30_06_H,pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_IsFullRange);//assign by E_CFD_CFIO_RANGE
        msWriteByte(REG_SC_Ali_BK30_07_H,pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_HDRMode);//assign by E_CFIO_HDR_STATUS
        //pstApiControlParamInit->u8Input_Format_HDMI_CE_Flag             = ; //1: CE 0 : IT
        //pstApiControlParamInit->u8Output_Format_HDMI_CE_Flag            = ; //1: CE 0 : IT
        //pstApiControlParamInit->u8HDMIOutput_GammutMapping_En           = ;  //1: ON 0: OFF
        //pstApiControlParamInit->u8HDMIOutput_GammutMapping_MethodMode           = ;  //0: extension 1:compression
        /*
        msWriteByte(REG_SC_Ali_BK30_08_L,pstApiControlParamInit->u8MMInput_ColorimetryHandle_Mode);      //1: ON 0: OFF
        msWriteByte(REG_SC_Ali_BK30_08_H,pstApiControlParamInit->u8PanelOutput_GammutMapping_Mode);  //1:depend on output_source infor ON 0: use default values
        msWriteByte(REG_SC_Ali_BK30_09_H,pstApiControlParamInit->u8TMO_TargetRefer_Mode);   //for TMO, unit : 1 nits
        msWriteByte(REG_SC_Ali_BK30_0A_H,((pstApiControlParamInit->u16Target_Max_Luminance>>8)&0xFF));
        msWriteByte(REG_SC_Ali_BK30_0A_L,((pstApiControlParamInit->u16Target_Max_Luminance)&0xFF)); //for TMO, unit : 0.0001 nits
        msWriteByte(REG_SC_Ali_BK30_0B_H,((pstApiControlParamInit->u16Target_Min_Luminance>>8)&0xFF));
        msWriteByte(REG_SC_Ali_BK30_0B_L,(pstApiControlParamInit->u16Target_Min_Luminance&0xFF));
        */
        //msWriteByte(REG_SC_Ali_BK30_08_L,(pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode
        //                                                          |(pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_SDRIPMode<<4)
        //                                                          |(pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_HDRIPMode<<6)));      //1: ON 0: OFF
        //msWriteByte(REG_SC_Ali_BK30_08_H,pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Manual_Vars_en);  //1:depend on output_source infor ON 0: use default values
        //msWriteByte(REG_SC_Ali_BK30_09_H,pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_MC);   //for TMO, unit : 1 nits
        msWriteByte(REG_SC_Ali_BK30_08_H,pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_DeGamma_Manual_Vars_en);
        msWriteByte(REG_SC_Ali_BK30_09_H,pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_Degamma_TR); //for TMO, unit : 0.0001 nits
        msWriteByte(REG_SC_Ali_BK30_0B_H,pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_Gamma_Manual_Vars_en);
        msWriteByte(REG_SC_Ali_BK30_0B_L,pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_Gamma_TR);
        msWriteByte(REG_SC_Ali_BK30_0A_H,((pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax>>8)&0xFF));
        msWriteByte(REG_SC_Ali_BK30_0A_L,((pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax)&0xFF)); //for TMO, unit : 0.0001 nits
        u8IP2_CSC_Ratio1   =  pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Manhattan_SDRIP_Param.u8IP2_CSC_Ratio1;
        u8VOP_3x3_Ratio1   =  pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Manhattan_SDRIP_Param.u8VOP_3x3_Ratio1;
        u8Degamma_Ratio1   =  pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Manhattan_SDRIP_Param.u8Degamma_Ratio1;
        u16Degamma_Ratio2  = pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Manhattan_SDRIP_Param.u16Degamma_Ratio2;
        u163x3_Ratio2      =     pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Manhattan_SDRIP_Param.u163x3_Ratio2;
    }

    msDlc_FunctionExit();
#endif

    //printk("u8Process_Mode                        :%d REG_SC_Ali_BK30_01_L \n", pstApiControlParamInit->u8Process_Mode                     );
    printk("u8Input_Source                        :%d REG_SC_Ali_BK30_01_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_Source                     );
    //printk("u8Input_AnalogIdx                     :%d REG_SC_Ali_BK30_02_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_AnalogIdx                  );
    printk("u8Input_Format                        :%d REG_SC_Ali_BK30_02_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_Format                     );
    printk("u8Input_DataFormat                    :%d REG_SC_Ali_BK30_03_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_DataFormat                 );
    printk("u8Input_IsFullRange                   :%d REG_SC_Ali_BK30_03_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_IsFullRange                );
    printk("u8Input_HDRMode                       :%d REG_SC_Ali_BK30_04_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_HDRMode                    );
    printk("u8Input_IsRGBBypass                   :%d REG_SC_Ali_BK30_04_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_IsRGBBypass                );
    printk("u8Output_Source                       :%d REG_SC_Ali_BK30_05_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_Source                    );
    printk("u8Output_Format                       :%d REG_SC_Ali_BK30_05_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_Format                    );
    printk("u8Output_DataFormat                   :%d REG_SC_Ali_BK30_06_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_DataFormat                );
    printk("u8Output_IsFullRange                  :%d REG_SC_Ali_BK30_06_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_IsFullRange               );
    printk("u8Output_HDRMode                      :%d REG_SC_Ali_BK30_07_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_HDRMode                   );
    printk("u8HDR_enable_Mode            :%d REG_SC_Ali_BK30_08_L Bit[0]  \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode             );
    printk("u8Input_SDRIPMode            :%d REG_SC_Ali_BK30_08_L Bit[5:4]\n",pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_SDRIPMode                     );
    printk("u8Input_HDRIPMode            :%d REG_SC_Ali_BK30_08_L Bit[7:6]\n",pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_HDRIPMode                     );
    //printk("u8HDR_InputCSC_Manual_Vars_en:%d REG_SC_Ali_BK30_08_H         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Manual_Vars_en );
    //printk("u8HDR_InputCSC_MC            :%d REG_SC_Ali_BK30_09_H         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_MC             );
    printk("u8HDR_DeGamma_Manual_Vars_en :%d REG_SC_Ali_BK30_08_H         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_DeGamma_Manual_Vars_en  );
    printk("u8HDR_Degamma_TR             :%d REG_SC_Ali_BK30_09_H         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_Degamma_TR              );
    printk("u8HDR_Gamma_Manual_Vars_en   :%d REG_SC_Ali_BK30_0B_H         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_Gamma_Manual_Vars_en    );
    printk("u8HDR_Gamma_TR               :%d REG_SC_Ali_BK30_0B_L         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_Gamma_TR                );
}
#endif


void Mapi_Cfd_Maserati_HDRIP_Param_Init(STU_CFDAPI_Maserati_HDRIP *pstu_Maserati_HDRIP_Param)
{

    //pstu_Maserati_HDRIP_Param->u8HDR_enable_Mode = 0xC0;

    //Composer
    //pstu_Maserati_HDRIP_Param->u8HDR_Composer_Mode = 0xC0;

    //B01
    //pstu_Maserati_HDRIP_Param->u8HDR_Module1_enable_Mode = 0xC0;

    //B01-02
    //pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_Mode;
    pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_Ratio1 = 0x40;
    pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_Manual_Vars_en = 0;
    pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;

    //B01-03
    //pstu_Maserati_HDRIP_Param->u8HDR_Degamma_SRAM_Mode = 0;
    pstu_Maserati_HDRIP_Param->u8HDR_Degamma_Ratio1 = 0x40;//0x40 = 1 Q2.6
    pstu_Maserati_HDRIP_Param->u16HDR_Degamma_Ratio2 = 0x40;//0x40 = 1 Q2.6
    pstu_Maserati_HDRIP_Param->u8HDR_DeGamma_Manual_Vars_en = 0;
    pstu_Maserati_HDRIP_Param->u8HDR_Degamma_TR = E_CFD_CFIO_TR_BT709;
    pstu_Maserati_HDRIP_Param->u8HDR_Degamma_Lut_En = 0;
    pstu_Maserati_HDRIP_Param->pu32HDR_Degamma_Lut_Address = NULL;
    pstu_Maserati_HDRIP_Param->u16HDR_Degamma_Lut_Length = 0x200;
    pstu_Maserati_HDRIP_Param->u8DHDR_Degamma_Max_Lum_En = 0;
    pstu_Maserati_HDRIP_Param->u16HDR_Degamma_Max_Lum = 100;

    //B01-04
    //pstu_Maserati_HDRIP_Param->u8HDR_3x3_Mode;
    pstu_Maserati_HDRIP_Param->u16HDR_3x3_Ratio2= 0x40;//0x40 = 1 Q2.6
    pstu_Maserati_HDRIP_Param->u8HDR_3x3_Manual_Vars_en = 0;
    pstu_Maserati_HDRIP_Param->u8HDR_3x3_InputCP = E_CFD_CFIO_TR_BT709;
    pstu_Maserati_HDRIP_Param->u8HDR_3x3_OutputCP = E_CFD_CFIO_TR_BT709;

    //B01-05
    //pstu_Maserati_HDRIP_Param->u8HDR_Gamma_SRAM_Mode = 0;
    pstu_Maserati_HDRIP_Param->u8HDR_Gamma_Manual_Vars_en = 0;
    pstu_Maserati_HDRIP_Param->u8HDR_Gamma_TR = E_CFD_CFIO_TR_BT709;
    pstu_Maserati_HDRIP_Param->u8HDR_Gamma_Lut_En = 0;
    pstu_Maserati_HDRIP_Param->pu32HDR_Gamma_Lut_Address = NULL;
    pstu_Maserati_HDRIP_Param->u16HDR_Gamma_Lut_Length = 0x200;

    //B01-06
    //pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_Mode;
    pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_Ratio1 = 0x40;
    pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_Manual_Vars_en = 0;
    pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;

    //MaxRGB for B02
    pstu_Maserati_HDRIP_Param->u8HDR_MAXRGB_CSC_Mode = 0xC7;
    pstu_Maserati_HDRIP_Param->u8HDR_MAXRGB_Ratio1 = 0x40;
    pstu_Maserati_HDRIP_Param->u8HDR_MAXRGB_Manual_Vars_en = 0;
    pstu_Maserati_HDRIP_Param->u8HDR_MAXRGB_MC = E_CFD_CFIO_MC_BT709_XVYCC709;

}

void Mapi_Cfd_Maserati_SDRIP_Param_Init(STU_CFDAPI_Maserati_SDRIP *pstu_Maserati_SDRIP_Param)
{
    //pstu_Maserati_SDRIP_Param->u8IP2_CSC_Mode = 0x80;
    pstu_Maserati_SDRIP_Param->u8IP2_CSC_Ratio1 = 0x40;
    pstu_Maserati_SDRIP_Param->u8IP2_CSC_Manual_Vars_en = 0;
    pstu_Maserati_SDRIP_Param->u8IP2_CSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;

    //pstu_Maserati_SDRIP_Param->u8VOP_3x3_Mode = 0x80;
    pstu_Maserati_SDRIP_Param->u8VOP_3x3_Ratio1 = 0x40;//0x40 = 1 Q2.6
    pstu_Maserati_SDRIP_Param->u8VOP_3x3_Manual_Vars_en = 0;
    pstu_Maserati_SDRIP_Param->u8VOP_3x3_MC = E_CFD_CFIO_MC_BT709_XVYCC709;

    //pstu_Maserati_SDRIP_Param->u8VOP_3x3RGBClip_Mode = 0x80;

    //pstu_Maserati_SDRIP_Param->u8LinearRGB_Mode = 0x80;

    //pstu_Maserati_SDRIP_Param->u8Degamma_enable_Mode = 0x80;
    //pstu_Maserati_SDRIP_Param->u8Degamma_Dither_Mode = 0x80;
    //pstu_Maserati_SDRIP_Param->u8Degamma_SRAM_Mode = 0x80;
    pstu_Maserati_SDRIP_Param->u8Degamma_Ratio1 = 0x40;//0x40 = 1 Q2.6
    pstu_Maserati_SDRIP_Param->u16Degamma_Ratio2 = 0x40;//0x40 = 1 Q2.6

    pstu_Maserati_SDRIP_Param->u8DeGamma_Manual_Vars_en = 0;
    pstu_Maserati_SDRIP_Param->u8Degamma_TR = E_CFD_CFIO_TR_BT709;
    pstu_Maserati_SDRIP_Param->u8Degamma_Lut_En = 0;
    pstu_Maserati_SDRIP_Param->pu32Degamma_Lut_Address = NULL;
    pstu_Maserati_SDRIP_Param->u16Degamma_Lut_Length = 256;

    pstu_Maserati_SDRIP_Param->u8Degamma_Max_Lum_En = 0;
    pstu_Maserati_SDRIP_Param->u16Degamma_Max_Lum = 100;

    //pstu_Maserati_SDRIP_Param->u83x3_enable_Mode = 0x80;
    //pstu_Maserati_SDRIP_Param->u83x3_Mode = 0x80;
    pstu_Maserati_SDRIP_Param->u163x3_Ratio2 = 0x40;//0x40 = 1 Q2.6
    pstu_Maserati_SDRIP_Param->u83x3_Manual_Vars_en = 0;
    pstu_Maserati_SDRIP_Param->u83x3_InputCP = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    pstu_Maserati_SDRIP_Param->u83x3_OutputCP = E_CFD_CFIO_CP_BT709_SRGB_SYCC;

    //pstu_Maserati_SDRIP_Param->u83x3Clip_Mode = 0x80;

    //pstu_Maserati_SDRIP_Param->u8Compress_settings_Mode = 0x80;
    //pstu_Maserati_SDRIP_Param->u8Compress_dither_Mode = 0x80;

    //pstu_Maserati_SDRIP_Param->u8Gamma_enable_Mode = 0x80;
    //pstu_Maserati_SDRIP_Param->u8Gamma_Dither_Mode = 0x80;
    //pstu_Maserati_SDRIP_Param->u8Gamma_maxdata_Mode = 0x80;
    //pstu_Maserati_SDRIP_Param->u8Gamma_SRAM_Mode = 0x80;

    pstu_Maserati_SDRIP_Param->u8Gamma_Mode_Vars_en = 0;
    pstu_Maserati_SDRIP_Param->u8Gamma_TR = E_CFD_CFIO_TR_BT709;
    pstu_Maserati_SDRIP_Param->u8Gamma_Lut_En = 0;
    pstu_Maserati_SDRIP_Param->pu32Gamma_Lut_Address = NULL;
    pstu_Maserati_SDRIP_Param->u16Gamma_Lut_Length = 256;
}

void Mapi_Cfd_Maserati_TMOIP_Param_Init(STU_CFDAPI_Maserati_TMOIP *pstu_Maserati_TMOIP_Param)
{
    //do nothing
}

void Mapi_Cfd_Maserati_DLCIP_Param_Init(STU_CFDAPI_Maserati_DLCIP *pstu_Maserati_DLCIP_Param)
{
    //do nothing
}

MS_U16 Mapi_Cfd_Maserati_DLCIP_Param_Check(STU_CFDAPI_Maserati_DLCIP *pstu_Maserati_DLCIP_Param)
{
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
    //MS_U16 u16Temp;

    MS_U8 u8temp = 0;

    u8temp = FunctionMode(pstu_Maserati_DLCIP_Param->u8DLC_curve_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  DLC u8DLC_curve_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_DLC_OVERRANGE;
        pstu_Maserati_DLCIP_Param->u8DLC_curve_enable_Mode = function1(pstu_Maserati_DLCIP_Param->u8DLC_curve_enable_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_DLCIP_Param->u8DLC_curve_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x04)
    {
        HDR_DBG_HAL_CFD(printk("\n  DLC u8DLC_curve_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_DLC_OVERRANGE;
        pstu_Maserati_DLCIP_Param->u8DLC_curve_enable_Mode = function1(pstu_Maserati_DLCIP_Param->u8DLC_curve_enable_Mode,0);
    }

#if 0
    u8temp = FunctionMode(pstu_Maserati_DLCIP_Param->u8UVC_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  DLC u8UVC_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_DLC_OVERRANGE;
        pstu_Maserati_DLCIP_Param->u8UVC_enable_Mode = function1(pstu_Maserati_DLCIP_Param->u8UVC_enable_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_DLCIP_Param->u8UVC_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x03)
    {
        HDR_DBG_HAL_CFD(printk("\n  DLC u8UVC_setting_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_DLC_OVERRANGE;
        pstu_Maserati_DLCIP_Param->u8UVC_setting_Mode = function1(pstu_Maserati_DLCIP_Param->u8UVC_setting_Mode,0);
    }
#endif
    u16_check_status = E_CFD_MC_ERR_NOERR;//Easter_test
    return u16_check_status;
}


MS_U16 Mapi_Cfd_Maserati_TMOIP_Param_Check(STU_CFDAPI_Maserati_TMOIP *pstu_Maserati_TMOIP_Param)
{
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
    //MS_U16 u16Temp;

    MS_U8 u8temp = 0;

    u8temp = FunctionMode(pstu_Maserati_TMOIP_Param->u8HDR_TMO_curve_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n TMO u8HDR_TMO_curve_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Maserati_TMOIP_Param->u8HDR_TMO_curve_enable_Mode = function1(pstu_Maserati_TMOIP_Param->u8HDR_TMO_curve_enable_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_TMOIP_Param->u8HDR_TMO_curve_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x04)
    {
        HDR_DBG_HAL_CFD(printk("\n  TMO u8HDR_TMO_curve_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Maserati_TMOIP_Param->u8HDR_TMO_curve_Mode = function1(pstu_Maserati_TMOIP_Param->u8HDR_TMO_curve_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_TMOIP_Param->u8HDR_TMO_curve_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x03)
    {
        HDR_DBG_HAL_CFD(printk("\n  TMO u8HDR_TMO_curve_setting_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Maserati_TMOIP_Param->u8HDR_TMO_curve_setting_Mode = function1(pstu_Maserati_TMOIP_Param->u8HDR_TMO_curve_setting_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_TMOIP_Param->u8HDR_UVC_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x04)
    {
        HDR_DBG_HAL_CFD(printk("\n  TMO u8HDR_UVC_setting_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE;
        pstu_Maserati_TMOIP_Param->u8HDR_UVC_setting_Mode = function1(pstu_Maserati_TMOIP_Param->u8HDR_UVC_setting_Mode,0);
    }

    u16_check_status = E_CFD_MC_ERR_NOERR;//Easter_test
    return u16_check_status;
}

MS_U16 Mapi_Cfd_Maserati_SDRIP_Param_Check(STU_CFDAPI_Maserati_SDRIP *pstu_Maserati_SDRIP_Param)
{

    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
    //MS_U16 u16Temp;

    MS_U8 u8temp = 0;

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8IP2_CSC_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x0D)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8IP2_CSC_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8IP2_CSC_Mode = function1(pstu_Maserati_SDRIP_Param->u8IP2_CSC_Mode,0);
    }

    //1x
    if (pstu_Maserati_SDRIP_Param->u8IP2_CSC_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8IP2_CSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8IP2_CSC_Ratio1 = 0x40;
    }

    //1x
    if (pstu_Maserati_SDRIP_Param->u8IP2_CSC_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8IP2_CSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8IP2_CSC_Ratio1 = 0x40;
    }

    if (pstu_Maserati_SDRIP_Param->u8IP2_CSC_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8IP2_CSC_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8IP2_CSC_Manual_Vars_en = 0x00;
    }

    if (pstu_Maserati_SDRIP_Param->u8IP2_CSC_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8IP2_CSC_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8IP2_CSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8UFSC_YCOffset_Gain_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8UFSC_YCOffset_Gain_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8UFSC_YCOffset_Gain_Mode = function1(pstu_Maserati_SDRIP_Param->u8UFSC_YCOffset_Gain_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8VIP_CSC_Mode);
    if (u8temp != 0)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8VIP_CSC_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8VIP_CSC_Mode = function1(pstu_Maserati_SDRIP_Param->u8VIP_CSC_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8VIP_PreYoffset_Mode);
    if (u8temp != 0)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8VIP_PreYoffset_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8VIP_PreYoffset_Mode = function1(pstu_Maserati_SDRIP_Param->u8VIP_PreYoffset_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8VIP_PreYgain_Mode);
    if (u8temp != 0)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8VIP_PreYgain_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8VIP_PreYgain_Mode = function1(pstu_Maserati_SDRIP_Param->u8VIP_PreYgain_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8VIP_PreYgain_dither_Mode);
    if (u8temp != 0)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8VIP_PreYgain_dither_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8VIP_PreYgain_dither_Mode = function1(pstu_Maserati_SDRIP_Param->u8VIP_PreYgain_dither_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8VIP_PostYoffset_Mode);
    if (u8temp != 0)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8VIP_PostYoffset_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8VIP_PostYoffset_Mode = function1(pstu_Maserati_SDRIP_Param->u8VIP_PostYoffset_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8VIP_PostYgain_Mode);
    if (u8temp != 0)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8VIP_PostYgain_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8VIP_PostYgain_Mode = function1(pstu_Maserati_SDRIP_Param->u8VIP_PostYgain_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8VIP_PostYoffset2_Mode);
    if (u8temp != 0)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8VIP_PostYoffset2_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8VIP_PostYoffset2_Mode = function1(pstu_Maserati_SDRIP_Param->u8VIP_PostYoffset2_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8VOP_3x3_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x0D)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8VOP_3x3_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8VOP_3x3_Mode = function1(pstu_Maserati_SDRIP_Param->u8VOP_3x3_Mode,0);
    }

    //2x
    if (pstu_Maserati_SDRIP_Param->u8VOP_3x3_Ratio1 > 0x80)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8VOP_3x3_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8VOP_3x3_Ratio1 = 0x80;
    }

    //1x
    if (pstu_Maserati_SDRIP_Param->u8VOP_3x3_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8VOP_3x3_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8VOP_3x3_Ratio1 = 0x40;
    }

    if (pstu_Maserati_SDRIP_Param->u8VOP_3x3_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8VOP_3x3_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8VOP_3x3_Manual_Vars_en = 0x00;
    }

    if (pstu_Maserati_SDRIP_Param->u8VOP_3x3_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8VOP_3x3_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8VOP_3x3_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8VOP_3x3RGBClip_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8VOP_3x3RGBClip_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8VOP_3x3RGBClip_Mode = function1(pstu_Maserati_SDRIP_Param->u8VOP_3x3RGBClip_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8LinearRGBBypass_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8LinearRGBBypass_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8LinearRGBBypass_Mode = function1(pstu_Maserati_SDRIP_Param->u8LinearRGBBypass_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8Degamma_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Degamma_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Degamma_enable_Mode = function1(pstu_Maserati_SDRIP_Param->u8Degamma_enable_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8Degamma_Dither_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Degamma_Dither_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Degamma_Dither_Mode = function1(pstu_Maserati_SDRIP_Param->u8Degamma_Dither_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8Degamma_SRAM_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Degamma_SRAM_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Degamma_SRAM_Mode = function1(pstu_Maserati_SDRIP_Param->u8Degamma_SRAM_Mode,0);
    }

    //2x
    if (pstu_Maserati_SDRIP_Param->u8Degamma_Ratio1 > 0x80)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Degamma_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Degamma_Ratio1 = 0x80;
    }

    //1x
    if (pstu_Maserati_SDRIP_Param->u8Degamma_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Degamma_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Degamma_Ratio1 = 0x40;
    }

    //8x
    if (pstu_Maserati_SDRIP_Param->u16Degamma_Ratio2 > 0x200)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u16Degamma_Ratio2 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u16Degamma_Ratio2 = 0x200;
    }

    //1x
    if (pstu_Maserati_SDRIP_Param->u16Degamma_Ratio2 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u16Degamma_Ratio2 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u16Degamma_Ratio2 = 0x40;
    }

    if (pstu_Maserati_SDRIP_Param->u8DeGamma_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8DeGamma_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8DeGamma_Manual_Vars_en = 0x00;
    }

    if (pstu_Maserati_SDRIP_Param->u8Degamma_TR >= E_CFD_CFIO_TR_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Degamma_TR is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Degamma_TR = E_CFD_CFIO_TR_BT709;
    }

    if (pstu_Maserati_SDRIP_Param->u8Degamma_Lut_En > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Degamma_Lut_En is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Degamma_Lut_En = 0x00;
    }

    if (pstu_Maserati_SDRIP_Param->u16Degamma_Lut_Length > 600)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u16Degamma_Lut_Length is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u16Degamma_Lut_Length = 600;
    }

    if (pstu_Maserati_SDRIP_Param->u16Degamma_Lut_Length < 256)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u16Degamma_Lut_Length is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u16Degamma_Lut_Length = 256;
    }

    if (pstu_Maserati_SDRIP_Param->u8Degamma_Max_Lum_En > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Degamma_Max_Lum_En is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Degamma_Max_Lum_En = 0x00;
    }

    if (pstu_Maserati_SDRIP_Param->u16Degamma_Max_Lum > 0xff00)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u16Degamma_Max_Lum is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u16Degamma_Max_Lum = 0xff00;
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u83x3_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u83x3_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u83x3_enable_Mode = function1(pstu_Maserati_SDRIP_Param->u83x3_enable_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u83x3_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x03)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u83x3_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u83x3_Mode = function1(pstu_Maserati_SDRIP_Param->u83x3_Mode,0);
    }

    //8x
    if (pstu_Maserati_SDRIP_Param->u163x3_Ratio2 > 0x200)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u163x3_Ratio2 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u163x3_Ratio2 = 0x200;
    }

    //1x
    if (pstu_Maserati_SDRIP_Param->u163x3_Ratio2 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u163x3_Ratio2 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u163x3_Ratio2 = 0x40;
    }

    if (pstu_Maserati_SDRIP_Param->u83x3_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u83x3_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u83x3_Manual_Vars_en = 0x00;
    }

    if (pstu_Maserati_SDRIP_Param->u83x3_InputCP >= E_CFD_CFIO_CP_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u83x3_InputCP is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u83x3_InputCP = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    }

    if (pstu_Maserati_SDRIP_Param->u83x3_OutputCP >= E_CFD_CFIO_CP_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u83x3_OutputCP is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u83x3_OutputCP = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8Compress_settings_Mode);
    if (u8temp != 0)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Compress_settings_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Compress_settings_Mode = function1(pstu_Maserati_SDRIP_Param->u8Compress_settings_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8Compress_dither_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Compress_dither_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Compress_dither_Mode = function1(pstu_Maserati_SDRIP_Param->u8Compress_dither_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u83x3Clip_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u83x3Clip_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u83x3Clip_Mode = function1(pstu_Maserati_SDRIP_Param->u83x3Clip_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8Gamma_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Gamma_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Gamma_enable_Mode = function1(pstu_Maserati_SDRIP_Param->u8Gamma_enable_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8Gamma_Dither_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Gamma_Dither_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Gamma_Dither_Mode = function1(pstu_Maserati_SDRIP_Param->u8Gamma_Dither_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8Gamma_maxdata_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Gamma_maxdata_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Gamma_maxdata_Mode = function1(pstu_Maserati_SDRIP_Param->u8Gamma_maxdata_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8Gamma_SRAM_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Gamma_SRAM_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Gamma_SRAM_Mode = function1(pstu_Maserati_SDRIP_Param->u8Gamma_SRAM_Mode,0);
    }

    if (pstu_Maserati_SDRIP_Param->u8Gamma_Mode_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Gamma_Mode_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Gamma_Mode_Vars_en = 0x00;
    }

    if (pstu_Maserati_SDRIP_Param->u8Gamma_TR >= E_CFD_CFIO_TR_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Gamma_TR is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Gamma_TR = E_CFD_CFIO_TR_BT709;
    }

    if (pstu_Maserati_SDRIP_Param->u8Gamma_Lut_En > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8Gamma_Lut_En is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8Gamma_Lut_En = 0x00;
    }

    if (pstu_Maserati_SDRIP_Param->u16Gamma_Lut_Length != 256)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u16Gamma_Lut_Length is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u16Gamma_Lut_Length = 256;
    }
    u8temp = FunctionMode(pstu_Maserati_SDRIP_Param->u8YHSL_R2Y_Mode);
    if (u8temp > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  SDRIP u8YHSL_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE;
        pstu_Maserati_SDRIP_Param->u8YHSL_R2Y_Mode = 0x00;
    }
    u16_check_status = E_CFD_MC_ERR_NOERR;//Easter_test
    return u16_check_status;
}

MS_U16 Mapi_Cfd_Maserati_HDRIP_Param_Check(STU_CFDAPI_Maserati_HDRIP *pstu_Maserati_HDRIP_Param)
{

    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
//MS_U16 u16Temp;

    MS_U8 u8temp = 0;

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_enable_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_enable_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_Composer_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Composer_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_Composer_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_Composer_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_Module1_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Module1_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_Module1_enable_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_Module1_enable_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x0E)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_InputCSC_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_Mode,0);
    }

//1x
    if (pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_InputCSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_Ratio1 = 0x40;
    }

//1x
    if (pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_InputCSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_Ratio1 = 0x40;
    }

    if (pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_InputCSC_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_Manual_Vars_en = 0x00;
    }

    if (pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_InputCSC_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_InputCSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_Degamma_SRAM_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x03)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_SRAM_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_Degamma_SRAM_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_Degamma_SRAM_Mode,0);
    }

//2x
    if (pstu_Maserati_HDRIP_Param->u8HDR_Degamma_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_Degamma_Ratio1 = 0x40;
    }

//1x
    if (pstu_Maserati_HDRIP_Param->u8HDR_Degamma_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_Degamma_Ratio1 = 0x40;
    }

//2x
    if (pstu_Maserati_HDRIP_Param->u16HDR_Degamma_Ratio2 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_Degamma_Ratio2 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u16HDR_Degamma_Ratio2 = 0x40;
    }

//1x
    if (pstu_Maserati_HDRIP_Param->u16HDR_Degamma_Ratio2 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_Degamma_Ratio2 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u16HDR_Degamma_Ratio2 = 0x40;
    }

    if (pstu_Maserati_HDRIP_Param->u8HDR_DeGamma_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_DeGamma_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_DeGamma_Manual_Vars_en = 0x00;
    }

    if (pstu_Maserati_HDRIP_Param->u8HDR_Degamma_TR >= E_CFD_CFIO_TR_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_TR is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_Degamma_TR = E_CFD_CFIO_TR_BT709;
    }

    if (pstu_Maserati_HDRIP_Param->u8HDR_Degamma_Lut_En > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Degamma_Lut_En is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_Degamma_Lut_En = 0x00;
    }

    if (pstu_Maserati_HDRIP_Param->u16HDR_Degamma_Lut_Length != 512)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_Degamma_Lut_Length is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u16HDR_Degamma_Lut_Length = 512;
    }

    if (pstu_Maserati_HDRIP_Param->u8DHDR_Degamma_Max_Lum_En != 0)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8DHDR_Degamma_Max_Lum_En is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8DHDR_Degamma_Max_Lum_En = 0x00;
    }

    if (pstu_Maserati_HDRIP_Param->u16HDR_Degamma_Max_Lum > 0xff00)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_Degamma_Max_Lum is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u16HDR_Degamma_Max_Lum = 0xff00;
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_3x3_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x04)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3x3_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_3x3_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_3x3_Mode,0);
    }

//1x
    if (pstu_Maserati_HDRIP_Param->u16HDR_3x3_Ratio2 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_3x3_Ratio2 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u16HDR_3x3_Ratio2 = 0x40;
    }

//1x
    if (pstu_Maserati_HDRIP_Param->u16HDR_3x3_Ratio2 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_3x3_Ratio2 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u16HDR_3x3_Ratio2 = 0x40;
    }

    if (pstu_Maserati_HDRIP_Param->u8HDR_3x3_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3x3_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_3x3_Manual_Vars_en = 0x00;
    }

    if (pstu_Maserati_HDRIP_Param->u8HDR_3x3_InputCP >= E_CFD_CFIO_CP_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3x3_InputCP is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_3x3_InputCP = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    }

    if (pstu_Maserati_HDRIP_Param->u8HDR_3x3_OutputCP >= E_CFD_CFIO_CP_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3x3_OutputCP is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_3x3_OutputCP = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_Gamma_SRAM_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x03)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Gamma_SRAM_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_Gamma_SRAM_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_Gamma_SRAM_Mode,0);
    }

    if (pstu_Maserati_HDRIP_Param->u8HDR_Gamma_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Gamma_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_Gamma_Manual_Vars_en = 0x00;
    }

    if (pstu_Maserati_HDRIP_Param->u8HDR_Gamma_TR >= E_CFD_CFIO_TR_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Gamma_TR is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_Gamma_TR = E_CFD_CFIO_TR_BT709;
    }

    if (pstu_Maserati_HDRIP_Param->u8HDR_Gamma_Lut_En > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Gamma_Lut_En is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_Gamma_Lut_En = 0x00;
    }

    if (pstu_Maserati_HDRIP_Param->u16HDR_Gamma_Lut_Length != 512)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u16HDR_Gamma_Lut_Length is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u16HDR_Gamma_Lut_Length = 512;
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x0E)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_OutputCSC_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_Mode,0);
    }

//1x
    if (pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_OutputCSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_Ratio1 = 0x40;
    }

//1x
    if (pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_OutputCSC_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_Ratio1 = 0x40;
    }

    if (pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_OutputCSC_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_Manual_Vars_en = 0x00;
    }

    if (pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_OutputCSC_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_OutputCSC_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_Yoffset_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Yoffset_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_Yoffset_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_Yoffset_Mode,0);
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_MAXRGB_CSC_Mode);
    if (u8temp != E_CFD_IP_CSC_YFULL_TO_RFULL)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_MAXRGB_CSC_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_MAXRGB_CSC_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_MAXRGB_CSC_Mode,E_CFD_IP_CSC_YFULL_TO_RFULL);
    }

//1x
    if (pstu_Maserati_HDRIP_Param->u8HDR_MAXRGB_Ratio1 > 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_MAXRGB_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_MAXRGB_Ratio1 = 0x40;
    }

//1x
    if (pstu_Maserati_HDRIP_Param->u8HDR_MAXRGB_Ratio1 < 0x40)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_MAXRGB_Ratio1 is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_MAXRGB_Ratio1 = 0x40;
    }

    if (pstu_Maserati_HDRIP_Param->u8HDR_MAXRGB_Manual_Vars_en > 2)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_MAXRGB_Manual_Vars_en is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_MAXRGB_Manual_Vars_en = 0x00;
    }

    if (pstu_Maserati_HDRIP_Param->u8HDR_MAXRGB_MC >= E_CFD_CFIO_MC_RESERVED_START)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_MAXRGB_MC is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_MAXRGB_MC = E_CFD_CFIO_MC_BT709_XVYCC709;
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_NLM_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_NLM_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_NLM_enable_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_NLM_enable_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_NLM_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x03)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_NLM_setting_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_NLM_setting_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_NLM_setting_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_ACGain_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_ACGain_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_ACGain_enable_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_ACGain_enable_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_ACGain_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_ACGain_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_ACGain_setting_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_ACGain_setting_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_ACE_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_ACE_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_ACE_enable_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_ACE_enable_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_ACE_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_ACE_setting_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_ACE_setting_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_ACE_setting_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_Dither1_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Dither1_setting_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_Dither1_setting_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_Dither1_setting_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_3DLUT_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3DLUT_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_3DLUT_enable_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_3DLUT_enable_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_3DLUT_SRAM_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3DLUT_SRAM_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_3DLUT_SRAM_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_3DLUT_SRAM_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_3DLUT_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x03)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_3DLUT_setting_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_3DLUT_setting_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_3DLUT_setting_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_444to422_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_444to422_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_444to422_enable_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_444to422_enable_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_Dither2_enable_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Dither2_enable_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_Dither2_enable_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_Dither2_enable_Mode,0x00);
    }

    u8temp = FunctionMode(pstu_Maserati_HDRIP_Param->u8HDR_Dither2_setting_Mode);
    if (u8temp >= E_CFD_RESERVED_AT0x02)
    {
        HDR_DBG_HAL_CFD(printk("\n  HDRIP u8HDR_Dither2_setting_Mode is not correct \n"));
        u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE;
        pstu_Maserati_HDRIP_Param->u8HDR_Dither2_setting_Mode = function1(pstu_Maserati_HDRIP_Param->u8HDR_Dither2_setting_Mode,0x00);
    }
    u16_check_status = E_CFD_MC_ERR_NOERR;//Easter_test
    return u16_check_status;
}
MS_U8 MS_Cfd_Maserati_YHSL_R2Y_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
// 0: off
// 1: on
    MS_U8 u8_mode = 0;
    if( (E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Input_DataFormat)
        && ( E_CFD_INPUT_SOURCE_HDMI == pstu_Control_Param->u8Input_Source)
        && ( E_CFIO_MODE_HDR1 !=pstu_Control_Param->u8Input_HDRMode))
    {
        u8_mode = 1;
    }
    else
    {
        u8_mode = 0;
    }

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8YHSL_R2Y_Mode&0x80))
    {
        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8YHSL_R2Y_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8YHSL_R2Y_Mode), 0x03);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8YHSL_R2Y_Mode;
    }

    return u8_mode;
}
MS_U8 MS_Cfd_Maserati_VIP_CSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_CSC_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_CSC_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_CSC_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_CSC_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_VIP_PreYoffset_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PreYoffset_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PreYoffset_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PreYoffset_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PreYoffset_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_VIP_PreYgain_dither_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PreYgain_dither_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PreYgain_dither_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PreYgain_dither_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PreYgain_dither_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_VIP_PreYgain_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PreYgain_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PreYgain_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PreYgain_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PreYgain_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_VIP_PostYoffset_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PostYoffset_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PostYoffset_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PostYoffset_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PostYoffset_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_VIP_PostYgain_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PostYgain_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PostYgain_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PostYgain_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PostYgain_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_VIP_PostYoffset2_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PostYoffset2_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PostYoffset2_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PostYoffset2_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PostYoffset2_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_UFSC_YCOffsetGain_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8UFSC_YCOffset_Gain_Mode&0x80))
    {
        if (E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[1])
        {
            u8_mode = 0; //off
        }
        else
        {
            //u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8Temp_IsFullRange[1])
                u8_mode = 0; //off
            else
                u8_mode = 1; //limit to full
        }
        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8UFSC_YCOffset_Gain_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8UFSC_YCOffset_Gain_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8UFSC_YCOffset_Gain_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_InputCSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
    MS_U8 u8_mode = 0;

#if 0
    if(E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format ||E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format)
    {
        u8_mode = E_CFD_IP_CSC_OFF;
    }
#endif

    if (E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Input_DataFormat)
    {
        if(E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Input_IsFullRange && 0 == pstu_Control_Param->u8Input_IsRGBBypass)
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = E_CFD_IP_CSC_RLIMIT_TO_YFULL;
            else
                u8_mode = E_CFD_IP_CSC_RLIMIT_TO_YLIMIT;
        }
        else if(E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Input_IsFullRange && 1 == pstu_Control_Param->u8Input_IsRGBBypass)
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
            {
                #if HW_peaking_bug
                u8_mode = E_CFD_IP_CSC_RLIMIT_TO_RFULL;
                #else
                u8_mode = E_CFD_IP_CSC_OFF;
                #endif
            }
            else
                u8_mode = E_CFD_IP_CSC_OFF;
        }
        else if(E_CFD_CFIO_RANGE_FULL== pstu_Control_Param->u8Input_IsFullRange && 0 == pstu_Control_Param->u8Input_IsRGBBypass)
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = E_CFD_IP_CSC_RFULL_TO_YFULL;
            else
                u8_mode = E_CFD_IP_CSC_RFULL_TO_YLIMIT;
        }
        else if(E_CFD_CFIO_RANGE_FULL== pstu_Control_Param->u8Input_IsFullRange && 1 == pstu_Control_Param->u8Input_IsRGBBypass)
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = E_CFD_IP_CSC_OFF;
            else
            {
                #if HW_peaking_bug
                u8_mode = E_CFD_IP_CSC_RFULL_TO_RLIMIT;
                #else
                u8_mode = E_CFD_IP_CSC_OFF;
                #endif
            }
        }
        else
        {
            u8_mode = E_CFD_IP_CSC_OFF;
        }
    }
    else if ( E_CFD_MC_FORMAT_RGB != pstu_Control_Param->u8Input_DataFormat)
    {
        if(E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Input_IsFullRange && 0 == pstu_Control_Param->u8Input_IsRGBBypass)
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = E_CFD_IP_CSC_YLIMIT_TO_YFULL;
            else
                u8_mode = E_CFD_IP_CSC_OFF;
        }
        else if(E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Input_IsFullRange && 1 == pstu_Control_Param->u8Input_IsRGBBypass)
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
            {
                #if HW_peaking_bug
                u8_mode = E_CFD_IP_CSC_YLIMIT_TO_YFULL;
                #else
                u8_mode = E_CFD_IP_CSC_OFF;
                #endif
            }
            else
                u8_mode = E_CFD_IP_CSC_OFF;
        }
        else if(E_CFD_CFIO_RANGE_FULL== pstu_Control_Param->u8Input_IsFullRange && 0 == pstu_Control_Param->u8Input_IsRGBBypass)
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = E_CFD_IP_CSC_OFF;
            else
                u8_mode = E_CFD_IP_CSC_YFULL_TO_YLIMIT;
        }
        else if(E_CFD_CFIO_RANGE_FULL== pstu_Control_Param->u8Input_IsFullRange && 1 == pstu_Control_Param->u8Input_IsRGBBypass)
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = E_CFD_IP_CSC_OFF;
            else
            {
                #if HW_peaking_bug
                u8_mode = E_CFD_IP_CSC_YFULL_TO_YLIMIT;
                #else
                u8_mode = E_CFD_IP_CSC_OFF;
                #endif
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

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode&0x80))
    {
        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode), 0x0D);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_OutputCSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
    MS_U8 u8_mode = 0;
    if(E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Output_IsFullRange && (pstu_Control_Param->u8Output_Format <= E_CFD_CFIO_RESERVED_START && pstu_Control_Param->u8Output_Format >= E_CFD_CFIO_XVYCC_601))
    {
        printk("Error code =%d!!! This ouput format can't support limit range output [ %s  , %d]\n",E_CFD_MC_ERR_WRONGOUTPUTSOURCE, __FUNCTION__,__LINE__);
        //return E_CFD_MC_ERR_WRONGOUTPUTSOURCE;
        u8_mode = E_CFD_IP_CSC_OFF;
    }
#if !HW_peaking_bug
    else if ((E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[1]) &&  (1== pstu_Control_Param->u8Input_IsRGBBypass))
    {
        if(E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[1])
        {
            u8_mode = E_CFD_IP_CSC_RLIMIT_TO_RFULL;
        }
        else
        {
            u8_mode = E_CFD_IP_CSC_OFF;
        }
    }
#endif
//any input & RGB limit out
    else if (E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Output_DataFormat && E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Output_IsFullRange)
    {
        //RGB limit to RGB limit
        if ((E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[1]) &&  (E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[1]))
        {
            u8_mode = E_CFD_IP_CSC_OFF;
        }
        //RGB full to RGB limit
        else if ((E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[1]) &&  (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8Temp_IsFullRange[1]))
        {
            u8_mode = E_CFD_IP_CSC_RFULL_TO_RLIMIT;
        }
        //YUV limit to RGB limit
        else if ((E_CFD_MC_FORMAT_RGB != pstu_Control_Param->u8Temp_DataFormat[1]) &&  (E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[1]))
        {
            u8_mode = E_CFD_IP_CSC_YLIMIT_TO_RLIMIT;
        }
        //YUV full to RGB limit
        else if ((E_CFD_MC_FORMAT_RGB != pstu_Control_Param->u8Temp_DataFormat[1]) &&  (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8Temp_IsFullRange[1]))
        {
            u8_mode = E_CFD_IP_CSC_YFULL_TO_RLIMIT;
        }
    }
//RGB full out
    else if (E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Output_DataFormat && E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8Output_IsFullRange)
    {
        if((E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Temp_Format[1]) || (E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Temp_Format[1]))
        {
            u8_mode = E_CFD_IP_CSC_YLIMIT_TO_RFULL;
        }
        //RGB limit to RGB full
        else if ((E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[1]) && (E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[1]))
        {
            u8_mode = E_CFD_IP_CSC_RLIMIT_TO_RFULL;
        }
        //RGB full to RGB full
        else if ((E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[1]) &&  (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8Temp_IsFullRange[1]))
        {
            u8_mode = E_CFD_IP_CSC_OFF;
        }
        //YUV limit to RGB full
        else if ((E_CFD_MC_FORMAT_RGB != pstu_Control_Param->u8Temp_DataFormat[1]) &&  (E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[1]))
        {
            u8_mode = E_CFD_IP_CSC_YLIMIT_TO_RFULL;
        }
        //YUV full to RGB full
        else if ((E_CFD_MC_FORMAT_RGB != pstu_Control_Param->u8Temp_DataFormat[1]) &&  (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8Temp_IsFullRange[1]))
        {
            u8_mode = E_CFD_IP_CSC_YFULL_TO_RFULL;
        }
    }
//YUV limit
    else if (E_CFD_MC_FORMAT_RGB != pstu_Control_Param->u8Output_DataFormat && E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Output_IsFullRange)
    {
        //RGB limit tO YUV limit
        if ((E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[1]) && (E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[1]))
        {
            u8_mode = E_CFD_IP_CSC_RLIMIT_TO_YLIMIT;
        }
        //RGB full to YUV limit
        else if ((E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[1]) &&  (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8Temp_IsFullRange[1]))
        {
            u8_mode = E_CFD_IP_CSC_RFULL_TO_YLIMIT;
        }
        //YUV limit to YUV limit
        else if ((E_CFD_MC_FORMAT_RGB != pstu_Control_Param->u8Temp_DataFormat[1]) &&  (E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[1]))
        {
            u8_mode = E_CFD_IP_CSC_OFF;
        }
        //YUV full to YUV limit
        else if ((E_CFD_MC_FORMAT_RGB != pstu_Control_Param->u8Temp_DataFormat[1]) &&  (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8Temp_IsFullRange[1]))
        {
            u8_mode = E_CFD_IP_CSC_YFULL_TO_YLIMIT;
        }
    }
//YUV full
    else if (E_CFD_MC_FORMAT_RGB != pstu_Control_Param->u8Output_DataFormat && E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8Output_IsFullRange)
    {
        if((E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Temp_Format[1]) || (E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Temp_Format[1]))
        {
            u8_mode = E_CFD_IP_CSC_OFF;
        }
        //RGB limit tO YUV full
        else if ((E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[1]) && (E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[1]))
        {
            u8_mode = E_CFD_IP_CSC_RLIMIT_TO_YFULL;
        }
        //RGB full to YUV full
        else if ((E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[1]) &&  (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8Temp_IsFullRange[1]))
        {
            u8_mode = E_CFD_IP_CSC_RFULL_TO_YFULL;
        }
        //YUV limit to YUV full
        else if ((E_CFD_MC_FORMAT_RGB != pstu_Control_Param->u8Temp_DataFormat[1]) &&  (E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[1]))
        {
            u8_mode = E_CFD_IP_CSC_YLIMIT_TO_YFULL;
        }
        //YUV full to YUV full
        else if ((E_CFD_MC_FORMAT_RGB != pstu_Control_Param->u8Temp_DataFormat[1]) &&  (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8Temp_IsFullRange[1]))
        {
            u8_mode = E_CFD_IP_CSC_OFF;
        }
    }
    else
    {
        u8_mode = E_CFD_IP_CSC_OFF;
    }

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Mode&0x80))
    {
        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Mode), 0x0D);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Mode;

    }

    return u8_mode;

}

MS_U8 MS_Cfd_Maserati_OutputCSC_RGBClip_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3RGBClip_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_ON();

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3RGBClip_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3RGBClip_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3RGBClip_Mode;
    }

    return u8_mode;

}

MS_U8 MS_Cfd_Maserati_LinearRGB_Bypass_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8LinearRGBBypass_Mode&0x80))
    {
        //main
        if (0 == pstu_Control_Param->u8HW_MainSub_Mode)
        {
            if ((1 == pstu_Control_Param->u8DoTMO_Flag) || (1 == pstu_Control_Param->u8DoGamutMapping_Flag) || (1 == pstu_Control_Param->u8DoBT2020CLP_Flag))
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
            }
            else
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
            }
        }
        else //sub is always bypass
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8LinearRGBBypass_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8LinearRGBBypass_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8LinearRGBBypass_Mode;
    }

    return u8_mode;
}
MS_U8 MS_Cfd_Maserati_LinearRGB_DeGamma_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_enable_Mode&0x80))
    {
        if ((1 == pstu_Control_Param->u8DoTMO_Flag) || (1 == pstu_Control_Param->u8DoGamutMapping_Flag) || (1 == pstu_Control_Param->u8DoBT2020CLP_Flag))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_enable_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_LinearRGB_DeGamma_Dither_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_Dither_Mode&0x80))
    {
        if ((1 == pstu_Control_Param->u8DoTMO_Flag) || (1 == pstu_Control_Param->u8DoGamutMapping_Flag) || (1 == pstu_Control_Param->u8DoBT2020CLP_Flag))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_Dither_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_Dither_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_Dither_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_LinearRGB_DeGamma_SRAM_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: linear
//1: DegammabyTR

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_SRAM_Mode&0x80))
    {
        if ((1 == pstu_Control_Param->u8DoTMO_Flag) || (1 == pstu_Control_Param->u8DoGamutMapping_Flag) || (1 == pstu_Control_Param->u8DoBT2020CLP_Flag))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_SRAM_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_SRAM_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_SRAM_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_LinearRGB_3x3_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF
//1: ON

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_enable_Mode&0x80))
    {
        if ((1 == pstu_Control_Param->u8DoTMO_Flag) || (1 == pstu_Control_Param->u8DoGamutMapping_Flag) || (1 == pstu_Control_Param->u8DoBT2020CLP_Flag))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_enable_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_LinearRGB_3x3_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF
//1: ON

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_Mode&0x80))
    {
        if ((1 == pstu_Control_Param->u8DoTMO_Flag) || (1 == pstu_Control_Param->u8DoGamutMapping_Flag))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        else if (1 == pstu_Control_Param->u8DoBT2020CLP_Flag)
        {
            u8_mode = 2;
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_LinearRGB_Com_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_settings_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_settings_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_settings_Mode), 0x01);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_settings_Mode;
    }

    return u8_mode;

}
MS_U8 MS_Cfd_Maserati_LinearRGB_Com_Dither_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_dither_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_ON();

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_dither_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_dither_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_dither_Mode;
    }

    return u8_mode;

}

MS_U8 MS_Cfd_Maserati_LinearRGB_3x3Clip_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
    //0: OFF
//1: ON

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3Clip_Mode&0x80))
    {
        if ((1 == pstu_Control_Param->u8DoTMO_Flag) || (1 == pstu_Control_Param->u8DoGamutMapping_Flag) || (1 == pstu_Control_Param->u8DoBT2020CLP_Flag))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3Clip_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3Clip_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3Clip_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_LinearRGB_Gamma_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF
//1: ON

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_enable_Mode&0x80))
    {
        if ((1 == pstu_Control_Param->u8DoTMO_Flag) || (1 == pstu_Control_Param->u8DoGamutMapping_Flag) || (1 == pstu_Control_Param->u8DoBT2020CLP_Flag))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_enable_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_LinearRGB_Gamma_SRAM_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: Linear
//1: GammabyTR

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_SRAM_Mode&0x80))
    {
        if ((1 == pstu_Control_Param->u8DoTMO_Flag) || (1 == pstu_Control_Param->u8DoGamutMapping_Flag) || (1 == pstu_Control_Param->u8DoBT2020CLP_Flag))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_SRAM_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_SRAM_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_SRAM_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_LinearRGB_Gamma_Dither_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: OFF
//1: ON

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_Dither_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_ON();

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_Dither_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_Dither_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_Dither_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_LinearRGB_Gamma_maxdata_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param)
{
//0: PQ0_OFF
//1: PQ1_ON

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_maxdata_Mode&0x80))
    {
        u8_mode = MS_Cfd_OnlyEnable_Decision_ON();

        u8_mode = (pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_maxdata_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_maxdata_Mode), 0x02);

        u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_maxdata_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_DLC_Curve_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_DLC *pstu_DLC_Input)
{

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_DLC_Input->stu_Maserati_DLC_Param.u8DLC_curve_enable_Mode&0x80))
    {
        if (( E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[1]) && ( 1 == pstu_Control_Param->u8Input_IsRGBBypass))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }

        u8_mode = (pstu_DLC_Input->stu_Maserati_DLC_Param.u8DLC_curve_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_DLC_Input->stu_Maserati_DLC_Param.u8DLC_curve_enable_Mode), 0x02);

        u8_mode = pstu_DLC_Input->stu_Maserati_DLC_Param.u8DLC_curve_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_DLC_Curve_Mode_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_DLC *pstu_DLC_Input)
{
    MS_U8 u8_mode = 0;

// 0: linear
// 1: By DLC
// 2: by TMO curve function
// 3: Manual Mode

//MS_U8  u8DLC_curve_Mode;
    //MS_U8  u8DLC_curve_Manual_Mode;
    pstu_Control_Param->u8DoDLC_Flag = 0;

    if(0x80 == (pstu_DLC_Input->stu_Maserati_DLC_Param.u8DLC_curve_Mode&0x80))
    {
        //if(1 == pstu_DLC_Input->stu_Maserati_DLC_Param.u8DLC_curve_Manual_Mode)
        //{
        //   u8_mode = 3;
        //}
        //else
        if (( E_CFD_MC_FORMAT_RGB == pstu_Control_Param->u8Temp_DataFormat[1]) && ( 1 == pstu_Control_Param->u8Input_IsRGBBypass))
        {
            u8_mode = 0;
        }
        else
        {
            if(1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
            {
                u8_mode = 0;
                //g_DlcParameters.ucDlcHistogramSource = 1;
            }
            else if( 1 == pstu_Control_Param->u8DoTMO_Flag)
            {
                u8_mode = 2;
                //g_DlcParameters.ucDlcHistogramSource = 1;
            }
            else
            {
                u8_mode = 1;
                pstu_Control_Param->u8DoDLC_Flag = 1;
                //g_DlcParameters.ucDlcHistogramSource = 1;
            }
        }

        u8_mode = (pstu_DLC_Input->stu_Maserati_DLC_Param.u8DLC_curve_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_DLC_Input->stu_Maserati_DLC_Param.u8DLC_curve_Mode), 0x04);

        u8_mode = pstu_DLC_Input->stu_Maserati_DLC_Param.u8DLC_curve_Mode;

        if (1 == u8_mode)
        {
            pstu_Control_Param->u8DoDLC_Flag = 1;
        }
    }

    return u8_mode;
}

//IPs in HDR IP
MS_U8 MS_Cfd_Maserati_HDR_TMO_Curve_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
//0: OFF
//1: ON

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_enable_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        //else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR2 == pstu_Control_Param->u8Temp_HDRMode[0]))
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
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

        u8_mode = (pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_enable_Mode), 0x02);

        u8_mode = pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_enable_Mode;
    }

    return u8_mode;
}


MS_U8 MS_Cfd_Maserati_HDR_TMO_Curve_Mode_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    MS_U8 u8_mode = 0;

// 0: linear
// 1: By TMO algorithm function
// 2: by Dolby driver function
// 3: Manual Mode

//MS_U8  u8TMO_curve_Mode;
    //MS_U8  u8TMO_curve_Manual_Mode;
    pstu_Control_Param->u8DoDLCInHDRIP_Flag = 0;

    if(0x80 == (pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 2;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
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

        u8_mode = (pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_Mode), 0x04);

        u8_mode = pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_UVC_setting_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    MS_U8 u8_mode = 0;

    // 0: PQ0_bypass
    // 1: PQ1_Open_mode
    // 2: PQ2_openHDR_bypass
    // 3: Dolby_mode

    if(0x80 == (pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_UVC_setting_Mode&0x80))
    {
        //Dolby HDR
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 3;
        }
        //openHDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            if (1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
            {
                u8_mode = 1;
            }
            else
            {
                if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                    u8_mode = 0;
                else
                    u8_mode = 2;
            }
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            if (1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
            {
                u8_mode = 1;
            }
            else
            {
                if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                    u8_mode = 0;
                else
                    u8_mode = 2;
            }
        }
        else
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = 0;
            else
                u8_mode = 2;
        }

        u8_mode = (pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_UVC_setting_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_UVC_setting_Mode), 0x03);
        u8_mode = pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_UVC_setting_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_TMO_Curve_setting_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    MS_U8 u8_mode = 0;

// 0: PQ0
// 1: Open_mode
// 2: Dolby_mode

    if(0x80 == (pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_setting_Mode&0x80))
    {

        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 2;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
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

        u8_mode = (pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_setting_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_setting_Mode), 0x03);
        u8_mode = pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_setting_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode&0x80))
    {

        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            if ((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag))
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
            }
            else if ((E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) ||
                     (E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) ||
                     (E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format))
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

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_Composer_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: OFF
//1: dolby_driver()
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Composer_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 1;
        }
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 0;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            u8_mode = 0;
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Composer_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Composer_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Composer_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_Module1_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Module1_enable_Mode&0x80))
    {

        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            if ((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag))
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
            }
            else if ((E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) ||
                     (E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) ||
                     (E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format))
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

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Module1_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Module1_enable_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Module1_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_InputCSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: OFF
//1~12 : see definition in E_CFD_IP_CSC_PROCESS
//13 : dolby_driver()

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 13;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                 ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if ((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag)||
                (E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) ||
                (E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) ||
                (E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format) ||
                (1 == pstu_Control_Param->u8DoForceEnterHDRIP_Flag)
               )
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
                        //RFULL to RFULL = OFF
                        else if(E_CFD_CFIO_RANGE_FULL== pstu_Control_Param->u8Temp_IsFullRange[0])
                        {
                            u8_mode = E_CFD_IP_CSC_OFF;
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
                    if(E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[0])
                    {
                        if((MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))
                            ||(E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format)
                            ||(E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format)
                            ||(E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format))
                        {
                        u8_mode = E_CFD_IP_CSC_YLIMIT_TO_RFULL;
                    }
                        else
                        {
                            u8_mode = E_CFD_IP_CSC_OFF;
                        }
                    }
                    else if(E_CFD_CFIO_RANGE_FULL== pstu_Control_Param->u8Temp_IsFullRange[0])
                    {
                        u8_mode = E_CFD_IP_CSC_YFULL_TO_RFULL;
                    }
                    else
                    {
                        u8_mode = E_CFD_IP_CSC_OFF;
                    }
#if !HW_peaking_bug
                    if(1 == pstu_Control_Param->u8Input_IsRGBBypass)
                    {
                        u8_mode = E_CFD_IP_CSC_OFF;
                    }
#endif
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

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Mode), 0x0e);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_DeGamma_SRAM_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: degamma(linear)
//1: degamma(TR)
//2 : dolby_driver()

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_SRAM_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 2;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                 ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if ((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag)||
                (E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) ||
                (E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) ||
                (E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format))
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
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_SRAM_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_SRAM_Mode), 0x03);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_SRAM_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_3x3_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0:3x3(CP1=CP2)
//1:3x3(CP1,CP2)
//2:3x3(2020CLtoNCL)
//3:dolby_driver()
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3x3_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 3;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                 ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if (1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag)
            {
                u8_mode = 1;
            }
            else if (1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag)
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

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3x3_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3x3_Mode), 0x04);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3x3_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_Gamma_SRAM_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0:Gamma(linear)
//1:Gamma(TR)
//2:dolby_driver()

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_SRAM_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 2;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                 ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if ((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag)||
                (E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) ||
                (E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) ||
                (E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format))
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
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_SRAM_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_SRAM_Mode), 0x03);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_SRAM_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_OutputCSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{

//0: OFF
//1~12 : see definition in E_CFD_IP_CSC_PROCESS
//13 : dolby_driver()
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 13;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                 ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if ((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoGamutMappingInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoBT2020CLPInHDRIP_Flag) ||
                (1 == pstu_Control_Param->u8DoDLCInHDRIP_Flag)||
                (E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format) ||
                (E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format) ||
                (E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format) ||
                (1 == pstu_Control_Param->u8DoForceEnterHDRIP_Flag))
            {
                if (0 == pstu_Control_Param->u8Input_IsRGBBypass)
                {
                    if(E_CFD_INPUT_SOURCE_HDMI == pstu_Control_Param->u8Input_Source && E_CFIO_MODE_SDR == pstu_Control_Param->u8Input_HDRMode)
                    {
                            if( E_CFD_MC_FORMAT_RGB != pstu_Control_Param->u8Temp_DataFormat[0] && E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[0])
                            {
                                if((E_CFD_CFIO_XVYCC_601 == pstu_Control_Param->u8Input_Format)
                                    ||(E_CFD_CFIO_XVYCC_709 == pstu_Control_Param->u8Input_Format)
                                    ||(E_CFD_CFIO_SYCC601 == pstu_Control_Param->u8Input_Format))
                                {
                                    u8_mode = E_CFD_IP_CSC_RFULL_TO_YLIMIT;
                                }
                                else
                                {
                                    if(E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[1])
                                    {
                                        u8_mode = E_CFD_IP_CSC_OFF;
                                    }
                                    else
                                    {
                                        u8_mode = E_CFD_IP_CSC_YLIMIT_TO_YFULL;
                                    }
                                }
                            }
                            else
                            {
                                if(E_CFD_CFIO_RANGE_LIMIT == pstu_Control_Param->u8Temp_IsFullRange[1])
                                {
                                    u8_mode = E_CFD_IP_CSC_RFULL_TO_YLIMIT;
                                }
                                else
                                {
                                    u8_mode = E_CFD_IP_CSC_RFULL_TO_YFULL;
                                }
                            }
                    }
                    else
                    {
                        u8_mode = E_CFD_IP_CSC_RFULL_TO_YFULL;
                    }
                }
                else
                {
                    if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8Temp_IsFullRange[0])
                    {
                            #if HW_peaking_bug
                        u8_mode = E_CFD_IP_CSC_RFULL_TO_RLIMIT;
                            #else
                            u8_mode = E_CFD_IP_CSC_OFF;
                            #endif
                    }
                    else
                    {
                        u8_mode = E_CFD_IP_CSC_OFF;
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

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_Mode), 0x0e);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_Yoffset_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: PQ0_bypass
//1: dolby_driver()
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Yoffset_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 1;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                 ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            u8_mode = 0;
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Yoffset_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Yoffset_Mode), 0x02);
        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Yoffset_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_MAXRGB_CSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: PQ0_bypass
//1: dolby_driver()
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_MAXRGB_CSC_Mode&0x80))
    {
        u8_mode = E_CFD_IP_CSC_YFULL_TO_RFULL;
        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_MAXRGB_CSC_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_MAXRGB_CSC_Mode&MaskForMode_LB;

        if (u8_mode!=E_CFD_IP_CSC_YFULL_TO_RFULL)
        {
            u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_MAXRGB_CSC_Mode&MaskForMode_HB)|(E_CFD_IP_CSC_YFULL_TO_RFULL&MaskForMode_LB);
            pstu_Control_Param->u16_check_status = E_CFD_MC_ERR_HW_IPS_PARAM_OVERRANGE;
        }

        //pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Yoffset_Mode), 0x02);
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_NLM_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: OFF
//1: ON

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_enable_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            if (1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
            {
                //u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
                u8_mode = MS_Cfd_OnlyEnable_Decision_ON();//temporally off ,wait for AC gain driver ready 2016/03/02
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

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_enable_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_NLM_setting_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: PQ0
//1: PQ1
//2: PQ2

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_setting_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 0;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
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

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_setting_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_setting_Mode), 0x03);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_setting_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_ACgain_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: OFF
//1: ON

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_enable_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
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

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_enable_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_ACgain_setting_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: PQ0_bypass
//1: ACgain_alg()

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_setting_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 0;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
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

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_setting_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_setting_Mode), 0x03);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_setting_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_ACE_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: OFF
//1: ON

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_enable_Mode&0x80))
    {
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }
        else
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        }

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_enable_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_ACE_setting_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0:PQ0_bypass
//1:dolby_driver()

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_setting_Mode&0x80))
    {

        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 1;
        }
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 0;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            u8_mode = 0;
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_setting_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_setting_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_setting_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_Dither1_setting_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{

//0: PQ0_bypass
//1: Dolby_mode

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither1_setting_Mode&0x80))
    {
        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 1;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                 ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            u8_mode = 0;
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither1_setting_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither1_setting_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither1_setting_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_3DLUT_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_enable_Mode&0x80))
    {
        //Dolby HDR in
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
        }
        //Open HDR in
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
            else
            {
                if ((0 == pstu_Control_Param->u8Input_IsRGBBypass))
                {
                    u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
                }
                else
                {
                    u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
                }
            }
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
            {
                u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
            }
            else
            {
                if ((0 == pstu_Control_Param->u8Input_IsRGBBypass))
                {
                    if(E_CFD_INPUT_SOURCE_HDMI == pstu_Control_Param->u8Input_Source && E_CFIO_MODE_SDR == pstu_Control_Param->u8Input_HDRMode)
                    {
                        u8_mode = MS_Cfd_OnlyEnable_Decision_OFF
                                  ();;
                    }
                    else
                    {
                        u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
                    }
                }
                else
                {
                    u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
                }
            }
        }
        else
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
            else
            {
                if ((0 == pstu_Control_Param->u8Input_IsRGBBypass))
                {
                    u8_mode = MS_Cfd_OnlyEnable_Decision_ON();
                }
                else
                {
                    u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
                }
            }
        }

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_enable_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_3DLUT_SRAM_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: dolby_driver()

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_SRAM_Mode&0x80))
    {

        if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
            u8_mode = MS_Cfd_OnlyEnable_Decision_OFF();
        else
            u8_mode = 1; //full to limit

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_SRAM_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_SRAM_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_SRAM_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_3DLUT_setting_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: PQ0_bypass
//1: Dolby_mode
    //2: full to limit

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_setting_Mode&0x80))
    {

        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 1;
        }
        //
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = 0;
            else
                u8_mode = 2; //full to limit
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        else if ((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0]))
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = 0;
            else
                u8_mode = 2; //full to limit
        }
        else
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = 0;
            else
                u8_mode = 2; //full to limit
        }

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_setting_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_setting_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_setting_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_444to422_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_444to422_enable_Mode&0x80))
    {

        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 1;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                 ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            u8_mode = 0;
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_444to422_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_444to422_enable_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_444to422_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_Dither2_enable_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: OFF
//1: ON
    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_enable_Mode&0x80))
    {

        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 1;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                 ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = 0; //off
            else
                u8_mode = 0; //on
        }
        else
        {
            if (E_CFD_CFIO_RANGE_FULL == pstu_Control_Param->u8DoPathFullRange_Flag)
                u8_mode = 0; //off
            else
                u8_mode = 0; //on
        }

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_enable_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_enable_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_enable_Mode;
    }

    return u8_mode;
}

MS_U8 MS_Cfd_Maserati_HDR_Dither2_setting_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param)
{
//0: PQ0_bypass
//1: Dolby_mode

    MS_U8 u8_mode = 0;

    if(0x80 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_setting_Mode&0x80))
    {

        //u8Input_HDRIPMode = 1, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        //u8Input_HDRIPMode = 3, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR1
        if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])) ||
            ((3 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Temp_HDRMode[0])))
        {
            u8_mode = 1;
        }
        //u8Input_HDRIPMode = 1  u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2 or SDR
        //u8Input_HDRIPMode = 2, u8Temp_HDRMode[0] = E_CFIO_MODE_HDR2
        else if (((1 == pstu_Control_Param->u8Input_HDRIPMode) && (E_CFIO_MODE_HDR1 != pstu_Control_Param->u8Temp_HDRMode[0])) ||
                 ((2 == pstu_Control_Param->u8Input_HDRIPMode) && (MS_Cfd_CheckOpenHDR(pstu_Control_Param->u8Temp_HDRMode[0]))))
        {
            u8_mode = 0;
        }
        else
        {
            u8_mode = 0;
        }

        u8_mode = (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_setting_Mode&MaskForMode_HB)|(u8_mode&MaskForMode_LB);
    }
    else
    {
        pstu_Control_Param->u16_check_status = MS_Cfd_Maserati_CheckModes(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_setting_Mode), 0x02);

        u8_mode = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_setting_Mode;
    }

    return u8_mode;
}

MS_U16 MS_Cfd_Maserati_DLC_Control(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_DLC *pstu_DLC_Input,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
//E_CFD_MC_ERR
    MS_U8 u8_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;

//need a function to check if the mode is out of range and handle exception
//u8_check_status = MS_Cfd_Maserati_DLC_Control_CheckModes(pstu_Control_Param,pstu_DLC_Input);

//DLC_Decision_Tree_Start
    pstu_DLC_Input->stu_Maserati_DLC_Param.u8DLC_curve_enable_Mode = MS_Cfd_Maserati_DLC_Curve_enable_Decision(pstu_Control_Param,pstu_DLC_Input);
    pstu_DLC_Input->stu_Maserati_DLC_Param.u8DLC_curve_Mode = MS_Cfd_Maserati_DLC_Curve_Mode_Decision(pstu_Control_Param,pstu_DLC_Input);

//pstu_DLC_Input->stu_Maserati_DLC_Param.u8UVC_enable_Mode = MS_Cfd_Maserati_UVC_enable_Decision(pstu_Control_Param,pstu_DLC_Input);
//pstu_DLC_Input->stu_Maserati_DLC_Param.u8UVC_setting_Mode = MS_Cfd_Maserati_UVC_setting_Decision(pstu_Control_Param,pstu_DLC_Input);

    if ((0 == pstu_Control_Param->u8Input_SDRIPMode) || (2 == pstu_Control_Param->u8Input_SDRIPMode))
    {
        function1(pstu_DLC_Input->stu_Maserati_DLC_Param.u8DLC_curve_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_DLC_Input->stu_Maserati_DLC_Param.u8DLC_curve_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        //function1(pstu_DLC_Input->stu_Maserati_DLC_Param.u8UVC_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        //function1(pstu_DLC_Input->stu_Maserati_DLC_Param.u8UVC_setting_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        pstu_Control_Param->u8DoDLC_Flag = 0;
    }

//DLC_Decision_Tree_End
// Input_to_Physical_Layer_by_Tony_start

    u16_check_status = Mapi_Cfd_Maserati_DLCIP_Param_Check(&(pstu_DLC_Input->stu_Maserati_DLC_Param));

#if RealChip
    g_DlcParameters.u8Dlc_Mode = FunctionMode(pstu_DLC_Input->stu_Maserati_DLC_Param.u8DLC_curve_Mode);

    if (1 == pstu_Control_Param->u8DoTMO_Flag)
    {
        //printk("1 TMO control TgtMax:%d\n",pstu_TMO_Input->stu_CFD_TMO_Param.u16TgtMax);
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMinFlag;
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMaxFlag = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMaxFlag;
    }

    if (1 == g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode)
    {
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMin = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;  // 0.05
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMax = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax; // 300
        g_HDRinitParameters.DLC_HDRToneMappingData.u16Smin =  pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMin;
        g_HDRinitParameters.DLC_HDRToneMappingData.u16Smax = pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax;
        g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmax = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax;
        g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmin = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;
        g_HDRinitParameters.DLC_HDRToneMappingData.u16Smed = pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMed;
        g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmed = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMed;
    }
#endif

// Input_to_Physical_Layer_by_Tony_start
//produce TMO curves...
    SetIPhistogramWindow();
    return u16_check_status;
}



MS_U16 MS_Cfd_Maserati_TMO_Control(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
//E_CFD_MC_ERR
    MS_U8 u8_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_BOOL bFuncEn,bRegWriteEn;
    MS_U8 u8Mode;
    ST_HDR_UVC stUVC;
    ST_HDR_TMO_SETTINGS stTMO;
    memset(&stUVC,0,sizeof(ST_HDR_UVC));
    memset(&stTMO,0,sizeof(ST_HDR_TMO_SETTINGS));
    //TMO_Decision_Tree_Start

    //bypass mode
    if (0 == pstu_Control_Param->u8Input_HDRIPMode)
    {
        function1(pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_setting_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_UVC_setting_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
    }
    //normal mode
    else
    {
        pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_enable_Mode = MS_Cfd_Maserati_HDR_TMO_Curve_enable_Decision(pstu_Control_Param,pstu_TMO_Input);
        pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_Mode = MS_Cfd_Maserati_HDR_TMO_Curve_Mode_Decision(pstu_Control_Param,pstu_TMO_Input);
        pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_setting_Mode = MS_Cfd_Maserati_HDR_TMO_Curve_setting_Decision(pstu_Control_Param,pstu_TMO_Input);
        pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_UVC_setting_Mode = MS_Cfd_Maserati_HDR_UVC_setting_Decision(pstu_Control_Param,pstu_TMO_Input);
    }

    u16_check_status = Mapi_Cfd_Maserati_TMOIP_Param_Check(&(pstu_TMO_Input->stu_Maserati_TMO_Param));

    // Input_to_Physical_Layer_by_Tony_start
#if RealChip
    //g_DlcParameters.u8Dlc_Mode = pstu_TMO_Input->stu_Manhattan_TMOIP_Param.u8HDR_TMO_curve_Mode;
    if (1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag)
    {
        //printk("1 TMO control TgtMax:%d\n",pstu_TMO_Input->stu_CFD_TMO_Param.u16TgtMax);
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMinFlag;
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMaxFlag = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMaxFlag;
    }

    if (1 == g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode)
    {
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMin = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;  // 0.05
        g_HDRinitParameters.DLC_HDRNewToneMappingData.u16TgtMax = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax; // 300
        g_HDRinitParameters.DLC_HDRToneMappingData.u16Smin =  pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMin;
        g_HDRinitParameters.DLC_HDRToneMappingData.u16Smax = pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax;
        g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmax = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax;
        g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmin = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMin;
        g_HDRinitParameters.DLC_HDRToneMappingData.u16Smed = pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMed;
        g_HDRinitParameters.DLC_HDRToneMappingData.u16Tmed = pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMed;
    }
#endif
    // Input_to_Physical_Layer_by_Tony_start
    //produce TMO curves...
    //B02 TMO start//

    u8Mode = FunctionMode(pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_Mode);
#if RealChip
    g_DlcParameters.u8Tmo_Mode = u8Mode;
#endif
    if(2 != u8Mode)
    {
        bFuncEn = FunctionMode(pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_enable_Mode);
        bRegWriteEn = WriteRegsiterFlag(pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_setting_Mode);
        u8Mode = FunctionMode(pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_TMO_curve_setting_Mode);
        HdrTmoSettings(         bFuncEn,bRegWriteEn,u8Mode, &stTMO);
    }
    else
    {
        //u8_check_status = DolbyDriver();
    }
    //B02 TMO End//

    //B04 UVC Start//
    bFuncEn = FunctionMode(pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_UVC_setting_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_UVC_setting_Mode);
    u8Mode = FunctionMode(pstu_TMO_Input->stu_Maserati_TMO_Param.u8HDR_UVC_setting_Mode);
    if(3 != u8Mode)
    {
        HdrUvcWrite2Register(   bFuncEn,bRegWriteEn,u8Mode, &stUVC);
    }
    else
    {
        //u8_check_status = DolbyDriver();
    }
    //B04 UVC Edn//
    return u16_check_status ;
}
MS_U16 MS_Cfd_Maserati_HDRIP_WriteRegister_DebugMode(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    MS_U8 u8CurrentIP_Vars,u8CurrentIP_Vars2,u8GammutMode;
    MS_U16 u16Max_luma_codes;
    MS_U8 u8_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_BOOL bFuncEn,bRegWriteEn;
    MS_U8 u8Mode;
    MS_U8 u8IPindex = 0,u8IPmode = 0;
    MS_U16 u16temp;

    ST_HDR_B107 stYoffset;
    ST_HDR_NLM stNLM;
    ST_HDR_AC_GAIN stAcGain;
    ST_HDR_ACE stACE;
    ST_HDR_UVC stUVC;
    ST_HDR_DITHER1 stDither1;
    ST_HDR_3DLUT_SETTINGS st3DLIT;
    ST_HDR_DITHER2 stDither;
    memset(&stYoffset, 0, sizeof(ST_HDR_B107));
    memset(&stNLM, 0, sizeof(ST_HDR_NLM));
    memset(&stAcGain, 0, sizeof(ST_HDR_AC_GAIN));
    memset(&stACE, 0, sizeof(ST_HDR_ACE));
    memset(&stUVC, 0, sizeof(ST_HDR_UVC));
    memset(&stDither1, 0, sizeof(ST_HDR_DITHER1));
    memset(&st3DLIT, 0, sizeof(ST_HDR_3DLUT_SETTINGS));
    memset(&stDither, 0, sizeof(ST_HDR_DITHER2));
#ifdef CFD_debug
    u8IPindex = msReadByte(REG_SC_BK30_0E_H);
    u8IPmode = msReadByte(REG_SC_BK30_0E_L);
#endif
//HDR enable start//
    if(u8IPindex == 17)
    {
        if(u8IPmode == 0x80)
        {
            bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode);
            bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode);
            u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }
        HdrEnable(bFuncEn,bRegWriteEn,u8Mode);
    }
//HDR enable end//

//Composer Mode start//
    pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Composer_Mode = MS_Cfd_Maserati_HDR_Composer_Decision(pstu_Control_Param, pstu_HDRIP_Param);
//Composer Mode End//

//B01 Enable start//
    if(u8IPindex == 18)
    {
        if(0x80 == (u8IPmode &0x80))
        {
            bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Module1_enable_Mode);
            bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Module1_enable_Mode);
            u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Module1_enable_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }
        HdrB01En(               bFuncEn,bRegWriteEn,u8Mode);
    }
//B01 Enable End//

//B01-01 Start //
    //MS_U8 HdrCupB0101(            bFuncEn,bRegWriteEn,u8Mode, ST_HDR_CUP* Struct);
//B01-01 End //

//B01-02 InCSC Start//
    if(u8IPindex == 19)
    {
        if (1 == pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Manual_Vars_en)
        {
            u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_MC;
        }
        else
        {
            u8CurrentIP_Vars = pstu_Control_Param->u8TempMatrixCoefficients[0];
            pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_MC = pstu_Control_Param->u8TempMatrixCoefficients[0];
        }

        if(0x80 == (u8IPmode &0x80))
        {

        }
        else
        {
            pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Mode = u8IPmode;
        }

        u8_check_status = CSC((pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Mode&MaskForMode_LB),u8CurrentIP_Vars,pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Ratio1,3,1,E_CSC_MASERTATI_HDR_INPUT);
        if(0 == u8_check_status)
        {
            //printk("Error code =%d!!! B01-02 InputCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    }
//B01-02  InCSC End//

//B01-03 Degamma Start//
    if(u8IPindex == 20)
    {
        if(1 == pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_DeGamma_Manual_Vars_en)
        {
            u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_TR;
        }
        else
        {
            u8CurrentIP_Vars = pstu_Control_Param->u8TempTransferCharacterstics[0];
            pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_TR = pstu_Control_Param->u8TempTransferCharacterstics[0];
        }

        if(0x80 == (u8IPmode &0x80))
        {

        }
        else
        {
            u8CurrentIP_Vars = u8IPmode;
        }
        u16Max_luma_codes = MS_Cfd_Luminance_To_PQCode(pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax, pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMaxFlag);

        u16Max_luma_codes = ((MS_U32)u16Max_luma_codes*0xff00)/(0x3ff);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u16HDR_Degamma_Max_Lum = u16Max_luma_codes;

        ST_DEGAMMA_PARAMETERS_EXTENSION st_degamma_extension;
        ST_DEGAMMA_PARAMETERS_EXTENSION* pt_degamma_extension = &st_degamma_extension;
        pt_degamma_extension->dePQclamp_en = DePQClamp_EN;

        #if (1 == DePQClamp_EN)
        u16temp = MS_Cfd_Luminance_To_PQCode(pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax, 0);
        pt_degamma_extension->dePQclamp_value = ((MS_U32)u16temp*0xff00)/(0x3ff);
        #else
        //give maximum values
        pt_degamma_extension->dePQclamp_value = 0xff00;
        #endif

        u8_check_status = deGamma(u8CurrentIP_Vars, u16Max_luma_codes, pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_Ratio1
                                  ,pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u16HDR_Degamma_Ratio2
                                  , pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_Lut_En
                                  , pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.pu32HDR_Degamma_Lut_Address
                                  , pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u16HDR_Degamma_Lut_Length
                                  , 2,pstu_Control_Param->u8Temp_Format[0] , 1, pt_degamma_extension);
        if(0 == u8_check_status)
        {
            printk("Error code =%d!!!B01-03 Degamma Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    }
//B01-03 Degamma End//

//B01-04 HDR 3x3 Start//
    if(u8IPindex == 21)
    {
        if (1 == pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3x3_Manual_Vars_en)
        {
            u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3x3_InputCP;
            u8CurrentIP_Vars2 = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3x3_OutputCP;
        }
        else
        {
            u8CurrentIP_Vars = pstu_Control_Param->u8TempColorPriamries[0];
            u8CurrentIP_Vars2 =  pstu_Control_Param->u8TempColorPriamries[1];

            pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3x3_InputCP = pstu_Control_Param->u8TempColorPriamries[0];
            pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3x3_OutputCP = pstu_Control_Param->u8TempColorPriamries[1];
        }

        /*#if 0
        if (E_CFD_CFIO_CP_PANEL == u8CurrentIP_Vars2)
        {
            u8GammutMode = 0;
        }
        else
        {
        u8GammutMode = 1;
        }
        #endif */

//for HDR
        u8GammutMode = 1;
        if(0x80 == (u8IPmode &0x80))
        {
        }
        else
        {
            u8CurrentIP_Vars2 = u8IPmode;
        }
        u8_check_status = gamutMapping(u8CurrentIP_Vars, u8GammutMode,
                                       //&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.pstu_Panel_Param->stu_Cfd_Panel_ColorMetry)
                                       pstu_Control_Param->pstu_Panel_Param_Colorimetry,
                                       u8CurrentIP_Vars2,pstu_Control_Param->u8Temp_Format[0],
                                       pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u16HDR_3x3_Ratio2,1,1);

        if(0 == u8_check_status)
        {
            printk("Error code =%d!!!B01 -04 GamutMapping Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    }
//B01-04 HDR 3x3 End//

//B01-05 Gamma Start//
    if(u8IPindex == 22)
    {
        if (1 == pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_Manual_Vars_en)
        {
            u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_TR;
        }
        else
        {
            u8CurrentIP_Vars = pstu_Control_Param->u8TempTransferCharacterstics[1];
            pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_TR = pstu_Control_Param->u8TempTransferCharacterstics[1];
        }

        u16Max_luma_codes = 0xff00;

//if (u8CurrentIP_Vars == 16)
//{
// u8CurrentIP_Vars = 6;
//}
        if(0x80 == (u8IPmode &0x80))
        {
        }
        else
        {
            u8CurrentIP_Vars = u8IPmode;
        }
        u8_check_status = gamma(u8CurrentIP_Vars,u16Max_luma_codes,0,0,
                                pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_Lut_En,
                                pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.pu32HDR_Gamma_Lut_Address,
                                pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u16HDR_Gamma_Lut_Length,
                                1,pstu_Control_Param->u8Temp_Format[1],1);


        if(0 == u8_check_status)
        {
            printk("Error code =%d!!!B01-05 Gamma Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    }
//B01-05 Gamma End//

//B01-06 OutCSC start//
    if(u8IPindex == 23)
    {
        if (1 == pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_Manual_Vars_en)
        {
            u8CurrentIP_Vars2 = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_MC;
        }
        else
        {
            u8CurrentIP_Vars2 = pstu_Control_Param->u8TempMatrixCoefficients[1];
            pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
        }

        if(0x80 == (u8IPmode &0x80))
        {
        }
        else
        {
            pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_Mode = u8IPmode;
        }
        u8_check_status = CSC((pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_Mode&MaskForMode_LB)
                              ,u8CurrentIP_Vars2,pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_Ratio1
                              ,3,1,E_CSC_MASERTATI_HDR_OUTPUT);
        if(0 == u8_check_status)
        {
            printk("Error code =%d!!! B01-06 OutCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    }
//B01-06 OutCSC End//

//B01-07 Yoffset Start//
    if(u8IPindex == 24)
    {
        if(0x80 == (u8IPmode &0x80))
        {
            bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Yoffset_Mode);
            bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Yoffset_Mode);
            u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Yoffset_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }

        if(0 == u8Mode)
        {
            b107Write2Register(     bFuncEn,bRegWriteEn,u8Mode, &stYoffset);
        }
        else
        {
            //u8_check_status = DolbyDriver();
        }

        if(0 == u8_check_status)
        {
            printk("Error code =%d!!! B01-07 Yoffset Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    }
//B01-07 Yoffset End//

//M IP NLM start//
    if(u8IPindex == 25)
    {
        if(0x80 == (u8IPmode &0x80))
        {
            bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_enable_Mode);
            bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_setting_Mode);
            u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_setting_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }
        NlmWrite2Register(      bFuncEn,bRegWriteEn,u8Mode, &stNLM);
    }
//M IP NLM End//

//M IP AC Start //
    if(u8IPindex == 26)
    {
        if(0x80 == (u8IPmode &0x80))
        {
            bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_enable_Mode);
            bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_setting_Mode);
            u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_setting_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }

        if(0 == u8Mode)
        {
            HdrAcGainWrite2Register(bFuncEn,bRegWriteEn,u8Mode, &stAcGain);
        }
        else
        {
            //u16_check_status = ACgain_algo();
        }
    }

//M IP AC End //

//B03 ACE Start //
    if(u8IPindex == 27)
    {
        if(0x80 == (u8IPmode &0x80))
        {
            bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_enable_Mode);
            bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_setting_Mode);
            u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_setting_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }

        if(0 == u8Mode)
        {
            HdrAceWrite2Register(   bFuncEn,bRegWriteEn,u8Mode, &stACE);
        }
        else
        {
            //u16_check_status = DolbyDriver();
        }
    }


//B03 ACE End //

//B05-01 Dither Start//
    if(u8IPindex == 28)
    {
        if(0x80 == (u8IPmode &0x80))
        {
            bFuncEn = FunctionMode(0x01);
            bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither1_setting_Mode);
            u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither1_setting_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }
        if(0 == u8Mode)
        {
            hdrDither1_b501(        bFuncEn,bRegWriteEn,u8Mode, &stDither1);
        }
        else
        {
            //u16_check_status = DolbyDriver();
        }
    }

//B05-01 Dither End//

//B05-02 3D Lut Start//
    if(u8IPindex == 29)
    {
        if(0x80 == (u8IPmode &0x80))
        {
            bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_enable_Mode);
            bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_setting_Mode);
            u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_setting_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }
        hdr3dLutSettings_b502(  bFuncEn,bRegWriteEn,u8Mode, &st3DLIT);

        u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_SRAM_Mode);
        if(0 == u8Mode)
        {
            //u16_check_status = DolbyDriver();
        }
    }
//B05-02 3D Lut End//

//B06-01 444to422 start //
    if(u8IPindex == 30)
    {
        if(0x80 == (u8IPmode &0x80))
        {
            bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_444to422_enable_Mode);
            bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_444to422_enable_Mode);
            u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_444to422_enable_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }
        Hdr444to442_b601(       bFuncEn,bRegWriteEn,u8Mode);
    }
//B06-01 444to422 End //

//B06-02 Dither2 Start //
    if(u8IPindex == 31)
    {
        if(0x80 == (u8IPmode &0x80))
        {
            bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_enable_Mode);
            bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_setting_Mode);
            u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_setting_Mode);
        }
        else
        {
            bFuncEn = FunctionMode(u8IPmode);
            bRegWriteEn = WriteRegsiterFlag(u8IPmode);
            u8Mode = FunctionMode(u8IPmode);
        }
        hdrDither2_b602(       bFuncEn,bRegWriteEn,u8Mode, &stDither);
    }
//B06-02 Dither2 End //
//mD3dLutWrite2Register(u16Full2limit3dLut);
    return u16_check_status ;
}

MS_U16 MS_Cfd_Maserati_HDRIP_WriteRegister(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    MS_U8 u8CurrentIP_Vars,u8CurrentIP_Vars2,u8GammutMode;
    MS_U16 u16Max_luma_codes;
    MS_U8 u8_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_BOOL bFuncEn,bRegWriteEn;
    MS_U8 u8Mode;
    MS_U16 u16temp;
    ST_HDR_B107 stYoffset;
    ST_HDR_NLM stNLM;
    ST_HDR_AC_GAIN stAcGain;
    ST_HDR_ACE stACE;
    ST_HDR_UVC stUVC;
    ST_HDR_DITHER1 stDither1;
    ST_HDR_3DLUT_SETTINGS st3DLIT;
    ST_HDR_DITHER2 stDither;
    memset(&stYoffset, 0, sizeof(ST_HDR_B107));
    memset(&stNLM, 0, sizeof(ST_HDR_NLM));
    memset(&stAcGain, 0, sizeof(ST_HDR_AC_GAIN));
    memset(&stACE, 0, sizeof(ST_HDR_ACE));
    memset(&stUVC, 0, sizeof(ST_HDR_UVC));
    memset(&stDither1, 0, sizeof(ST_HDR_DITHER1));
    memset(&st3DLIT, 0, sizeof(ST_HDR_3DLUT_SETTINGS));
    memset(&stDither, 0, sizeof(ST_HDR_DITHER2));

//HDR enable start//
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode);
    HdrEnable(bFuncEn,bRegWriteEn,u8Mode);
//HDR enable end//

//Composer Mode start//
    pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Composer_Mode = MS_Cfd_Maserati_HDR_Composer_Decision(pstu_Control_Param, pstu_HDRIP_Param);
//Composer Mode End//

//B01 Enable start//
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Module1_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Module1_enable_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Module1_enable_Mode);
    HdrB01En(               bFuncEn,bRegWriteEn,u8Mode);
//B01 Enable End//

//B01-01 Start //
    //MS_U8 HdrCupB0101(            bFuncEn,bRegWriteEn,u8Mode, ST_HDR_CUP* Struct);
//B01-01 End //

//B01-02 InCSC Start//
    if (1 == pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_MC;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempMatrixCoefficients[0];
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_MC = pstu_Control_Param->u8TempMatrixCoefficients[0];
    }

    u8_check_status = CSC((pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Mode&MaskForMode_LB),u8CurrentIP_Vars,pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Ratio1,3,0,E_CSC_MASERTATI_HDR_INPUT);
    if(0 == u8_check_status)
    {
        //printk("Error code =%d!!! B01-02 InputCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
//B01-02  InCSC End//

//B01-03 Degamma Start//
    if(1 == pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_DeGamma_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_TR;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempTransferCharacterstics[0];
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_TR = pstu_Control_Param->u8TempTransferCharacterstics[0];
    }
    if(MApi_GFLIP_XC_R2BYTE(REG_SC_BK30_01_L) == 0x001C)
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax = MApi_GFLIP_XC_R2BYTE(REG_SC_Ali_BK30_0E_L);
    }
    else
    {
        pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax = 10000;
    }
    u16Max_luma_codes = MS_Cfd_Luminance_To_PQCode(pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax, pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMaxFlag);
    u16Max_luma_codes = ((MS_U32)u16Max_luma_codes*0xff00)/(0x3ff);
    pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u16HDR_Degamma_Max_Lum = u16Max_luma_codes;

    ST_DEGAMMA_PARAMETERS_EXTENSION st_degamma_extension;
    ST_DEGAMMA_PARAMETERS_EXTENSION* pt_degamma_extension = &st_degamma_extension;
    pt_degamma_extension->dePQclamp_en = DePQClamp_EN;

#if (1 == DePQClamp_EN)
    u16temp = MS_Cfd_Luminance_To_PQCode(pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax, 0);
    pt_degamma_extension->dePQclamp_value = ((MS_U32)u16temp*0xff00)/(0x3ff);
#else
    //give maximum values
    pt_degamma_extension->dePQclamp_value = 0xff00;
#endif

    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_SRAM_Mode);
    if(u8Mode == 0)//degamma = Linear
    {
        u8CurrentIP_Vars = 8;
        u8_check_status = deGamma(u8CurrentIP_Vars, u16Max_luma_codes, pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_Ratio1
                                  ,pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u16HDR_Degamma_Ratio2
                                  , pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_Lut_En
                                  , pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.pu32HDR_Degamma_Lut_Address
                                  , pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u16HDR_Degamma_Lut_Length
                                  , 2,pstu_Control_Param->u8Temp_Format[0] , 0, pt_degamma_extension);
    }
    else if(u8Mode == 1) //degamma = u8TempTransferCharacterstics
    {
        u8CurrentIP_Vars = u8CurrentIP_Vars;
        u8_check_status = deGamma(u8CurrentIP_Vars, u16Max_luma_codes, pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_Ratio1
                                  ,pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u16HDR_Degamma_Ratio2
                                  , pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_Lut_En
                                  , pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.pu32HDR_Degamma_Lut_Address
                                  , pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u16HDR_Degamma_Lut_Length
                                  , 2,pstu_Control_Param->u8Temp_Format[0] , 0, pt_degamma_extension);
    }
    else //dolby driver
    {
        //dolby_driver();
    }


    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!B01-03 Degamma Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
//B01-03 Degamma End//

//B01-04 HDR 3x3 Start//
    if (1 == pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3x3_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3x3_InputCP;
        u8CurrentIP_Vars2 = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3x3_OutputCP;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempColorPriamries[0];
        u8CurrentIP_Vars2 =  pstu_Control_Param->u8TempColorPriamries[1];

        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3x3_InputCP = pstu_Control_Param->u8TempColorPriamries[0];
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3x3_OutputCP = pstu_Control_Param->u8TempColorPriamries[1];
    }

#if 0
    if (E_CFD_CFIO_CP_PANEL == u8CurrentIP_Vars2)
    {
        u8GammutMode = 0;
    }
    else
    {
        u8GammutMode = 1;
    }
#endif

//for HDR
    u8GammutMode = 1;

    u8_check_status = gamutMapping(u8CurrentIP_Vars, u8GammutMode,
                                   //&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.pstu_Panel_Param->stu_Cfd_Panel_ColorMetry)
                                   pstu_Control_Param->pstu_Panel_Param_Colorimetry,
                                   u8CurrentIP_Vars2,pstu_Control_Param->u8Temp_Format[0],
                                   pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u16HDR_3x3_Ratio2,1,0);

    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!B01 -04 GamutMapping Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
//B01-04 HDR 3x3 End//

//B01-05 Gamma Start//
    if (1 == pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_TR;
    }
    else
    {
        if((1 == pstu_Control_Param->u8DoTMOInHDRIP_Flag) && (0x01 == (pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDRIP_Patch&0x01)))
        {
            //gamma_patch force to gamma 2.2
            u8CurrentIP_Vars = E_CFD_CFIO_TR_GAMMA2P2;
            pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_TR = E_CFD_CFIO_TR_GAMMA2P2;
            pstu_Control_Param->u8TempTransferCharacterstics[1] = E_CFD_CFIO_TR_GAMMA2P2;
            pstu_Control_Param->u8OutputTransferCharacterstics = E_CFD_CFIO_TR_GAMMA2P2;
        }
        else
        {
            u8CurrentIP_Vars = pstu_Control_Param->u8TempTransferCharacterstics[1];
            pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_TR = pstu_Control_Param->u8TempTransferCharacterstics[1];
        }
    }

    u16Max_luma_codes = 0xff00;

//if (u8CurrentIP_Vars == 16)
//{
// u8CurrentIP_Vars = 6;
//}
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_SRAM_Mode);
    if(u8Mode == 0)//gamma = linear
    {
        u8CurrentIP_Vars = 8;
        u8_check_status = gamma(u8CurrentIP_Vars,u16Max_luma_codes,0,0,
                                pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_Lut_En,
                                pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.pu32HDR_Gamma_Lut_Address,
                                pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u16HDR_Gamma_Lut_Length,
                                1,pstu_Control_Param->u8Temp_Format[1],0);

    }
    else if(u8Mode == 1)//gamma = u8TempTransferCharacterstics
    {
        u8CurrentIP_Vars = u8CurrentIP_Vars;
        if(E_CFD_CFIO_TR_GAMMA2P2 == u8CurrentIP_Vars)
        {
            pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_Lut_En = 1;
            pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.pu32HDR_Gamma_Lut_Address = Maserati22Gamma;
            pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u16HDR_Gamma_Lut_Length = 513;
            //printk("LutAdd:%d :%d\n",pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.pu32HDR_Gamma_Lut_Address,&Maserati22Gamma);
        }
        else
        {
            pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_Lut_En = 0;
            pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.pu32HDR_Gamma_Lut_Address = NULL;
            pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u16HDR_Gamma_Lut_Length = 513;
        }
        u8_check_status = gamma(u8CurrentIP_Vars,u16Max_luma_codes,0,0,
                                pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_Lut_En,
                                pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.pu32HDR_Gamma_Lut_Address,
                                pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u16HDR_Gamma_Lut_Length,
                                1,pstu_Control_Param->u8Temp_Format[1],0);
    }
    else //dolby driver
    {
        //dolby_driver();
    }


    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!B01-05 Gamma Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
//B01-05 Gamma End//

//B01-06 OutCSC start//
    if (1 == pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_Manual_Vars_en)
    {
        u8CurrentIP_Vars2 = pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_MC;
    }
    else
    {
        u8CurrentIP_Vars2 = pstu_Control_Param->u8TempMatrixCoefficients[1];
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
    }

    u8_check_status = CSC((pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_Mode&MaskForMode_LB)
                          ,u8CurrentIP_Vars2,pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_Ratio1
                          ,3,0,E_CSC_MASERTATI_HDR_OUTPUT);
    if(0 == u8_check_status)
    {
        printk("Error code =%d!!! B01-06 OutCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    //Max RGB CSC Start//
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_MAXRGB_CSC_Mode);

    u8_check_status = CSC(u8Mode,u8CurrentIP_Vars2,0x40,3,0,E_CSC_MASERATI_HDR_MAXRGB);
    //Max RGB CSC End//
    if(0 == u8_check_status)
    {
        printk("Error code =%d!!! Max RGB CSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
//B01-06 OutCSC End//

//B01-07 Yoffset Start//
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Yoffset_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Yoffset_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Yoffset_Mode);
    if(0 == u8Mode)
    {
        b107Write2Register(     bFuncEn,bRegWriteEn,u8Mode, &stYoffset);
    }
    else
    {
        //u8_check_status = DolbyDriver();
    }
    if(0 == u8_check_status)
    {
        printk("Error code =%d!!! B01-07 Yoffset Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
//B01-07 Yoffset End//

//M IP NLM start//
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_setting_Mode);
    NlmWrite2Register(      bFuncEn,bRegWriteEn,u8Mode, &stNLM);
//M IP NLM End//

//M IP AC Start //

    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_setting_Mode);
    if(0 == u8Mode)
    {
        HdrAcGainWrite2Register(bFuncEn,bRegWriteEn,u8Mode, &stAcGain);
    }
    else
    {
        //u16_check_status = ACgain_algo();
    }

//M IP AC End //

//B03 ACE Start //

    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_setting_Mode);

    if(0 == u8Mode)
    {
        HdrAceWrite2Register(   bFuncEn,bRegWriteEn,u8Mode, &stACE);
    }
    else
    {
        //u16_check_status = DolbyDriver();
    }


//B03 ACE End //

//B05-01 Dither Start//
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither1_setting_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither1_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither1_setting_Mode);
    if(0 == u8Mode)
    {
        hdrDither1_b501(        bFuncEn,bRegWriteEn,u8Mode, &stDither1);
    }
    else
    {
        //u16_check_status = DolbyDriver();
    }

//B05-01 Dither End//

//B05-02 3D Lut Start//
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_setting_Mode);
    hdr3dLutSettings_b502(  bFuncEn,bRegWriteEn,u8Mode, &st3DLIT);

    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_SRAM_Mode);
    if(0 == u8Mode)
    {
        //u16_check_status = DolbyDriver();
    }
//B05-02 3D Lut End//

//B06-01 444to422 start //
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_444to422_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_444to422_enable_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_444to422_enable_Mode);
    Hdr444to442_b601(       bFuncEn,bRegWriteEn,u8Mode);
//B06-01 444to422 End //

//B06-02 Dither2 Start //
    bFuncEn = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_enable_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_setting_Mode);
    u8Mode = FunctionMode(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_setting_Mode);
    hdrDither2_b602(       bFuncEn,bRegWriteEn,u8Mode, &stDither);
//B06-02 Dither2 End //
    //mD3dLutWrite2Register(u16Full2limit3dLut);

    return u16_check_status ;
}

MS_U16 MS_Cfd_Maserati_HDRIP_Control(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param, STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
//E_CFD_MC_ERR
    MS_U8 u8_check_status = 0;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status_tmp = E_CFD_MC_ERR_NOERR;
    MS_U8  u8ERR_Happens_Flag = 0;
    MS_U8 u8IPindex = 0xFF,u8IPmode = 0;
#ifdef CFD_debug
    u8IPindex = msReadByte(REG_SC_BK30_0E_H);
    u8IPmode = msReadByte(REG_SC_BK30_0E_L);
#endif
//4 cases of u8Input_HDRIPMode
//if ()
//bypass mode
    if (0 == pstu_Control_Param->u8Input_HDRIPMode)
    {
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Composer_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Module1_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());

        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Mode,E_CFD_IP_CSC_OFF);
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_SRAM_Mode,0);
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3x3_Mode,0);
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_SRAM_Mode,0);
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_Mode,E_CFD_IP_CSC_OFF);
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Yoffset_Mode,0);

        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_MAXRGB_CSC_Mode,E_CFD_IP_CSC_YFULL_TO_RFULL);

        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_setting_Mode,0);
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_setting_Mode,0);
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_setting_Mode,0);

        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither1_setting_Mode,0);
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_enable_Mode,0);
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_SRAM_Mode,0);
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_setting_Mode,0);

        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_444to422_enable_Mode,0);
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_enable_Mode,0);
        function1(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_setting_Mode,0);
    }
//normal mode
    else
    {
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode = MS_Cfd_Maserati_HDR_enable_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Composer_Mode = MS_Cfd_Maserati_HDR_Composer_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Module1_enable_Mode = MS_Cfd_Maserati_HDR_Module1_enable_Decision(pstu_Control_Param, pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Mode = MS_Cfd_Maserati_HDR_InputCSC_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Degamma_SRAM_Mode = MS_Cfd_Maserati_HDR_DeGamma_SRAM_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3x3_Mode = MS_Cfd_Maserati_HDR_3x3_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Gamma_SRAM_Mode = MS_Cfd_Maserati_HDR_Gamma_SRAM_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_OutputCSC_Mode = MS_Cfd_Maserati_HDR_OutputCSC_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Yoffset_Mode = MS_Cfd_Maserati_HDR_Yoffset_Decision(pstu_Control_Param, pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_MAXRGB_CSC_Mode = MS_Cfd_Maserati_HDR_MAXRGB_CSC_Decision(pstu_Control_Param, pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_enable_Mode = MS_Cfd_Maserati_HDR_NLM_enable_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_NLM_setting_Mode = MS_Cfd_Maserati_HDR_NLM_setting_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_enable_Mode = MS_Cfd_Maserati_HDR_ACgain_enable_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACGain_setting_Mode = MS_Cfd_Maserati_HDR_ACgain_setting_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_enable_Mode = MS_Cfd_Maserati_HDR_ACE_enable_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_ACE_setting_Mode = MS_Cfd_Maserati_HDR_ACE_setting_Decision(pstu_Control_Param, pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither1_setting_Mode = MS_Cfd_Maserati_HDR_Dither1_setting_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_enable_Mode = MS_Cfd_Maserati_HDR_3DLUT_enable_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_SRAM_Mode = MS_Cfd_Maserati_HDR_3DLUT_SRAM_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_3DLUT_setting_Mode = MS_Cfd_Maserati_HDR_3DLUT_setting_Decision(pstu_Control_Param, pstu_HDRIP_Param);

        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_444to422_enable_Mode = MS_Cfd_Maserati_HDR_444to422_enable_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_enable_Mode = MS_Cfd_Maserati_HDR_Dither2_enable_Decision(pstu_Control_Param, pstu_HDRIP_Param);
        pstu_HDRIP_Param->stu_Maserati_HDRIP_Param.u8HDR_Dither2_setting_Mode = MS_Cfd_Maserati_HDR_Dither2_setting_Decision(pstu_Control_Param, pstu_HDRIP_Param);
    }

    u16_check_status_tmp = Mapi_Cfd_Maserati_HDRIP_Param_Check(&(pstu_HDRIP_Param->stu_Maserati_HDRIP_Param));

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }
// Input to Physical Layer by Ali Start
    if(u8IPindex ==0xFF)
    {
        u16_check_status_tmp = MS_Cfd_Maserati_HDRIP_WriteRegister(pstu_Control_Param,pstu_HDRIP_Param,pstu_TMO_Input);
    }
    else
    {
        u16_check_status_tmp = MS_Cfd_Maserati_HDRIP_WriteRegister_DebugMode(pstu_Control_Param,pstu_HDRIP_Param,pstu_TMO_Input);
    }
// Input to Physical Layer by Ali End

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

    return u16_check_status;
}

MS_U16 MS_Cfd_Maserati_SDRIP_WriteRegister_DebugMode(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input,MS_U8 u8MainSubMode)
{
    MS_U8 u8CurrentIP_Vars = 0,u8CurrentIP_Vars2 = 0;
    MS_U16 u16Max_luma_codes = 0;
    MS_U8 u8GammutMode = 0;
    MS_U8 u8IPindex = 0,u8IPmode = 0;
    MS_U16 u16temp;
#ifdef CFD_debug
    u8IPindex = msReadByte(REG_SC_BK30_0E_H);
    u8IPmode = msReadByte(REG_SC_BK30_0E_L);
#endif
    MS_U8 u8_check_status = 0;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;


    if(u8IPindex == 1)
    {
        //inputCSC start

        if (1 == pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Manual_Vars_en)
        {
            u8CurrentIP_Vars = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_MC;
        }
        else
        {
            u8CurrentIP_Vars = pstu_Control_Param->u8InputMatrixCoefficients;
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_MC = pstu_Control_Param->u8InputMatrixCoefficients;
        }

        //if(u8IPmode == 0xFF)
        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode = u8IPmode;
        }
        if(0 == u8MainSubMode)
        {
            u8_check_status = CSC((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode&MaskForMode_LB),u8CurrentIP_Vars,pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Ratio1,3,1,E_CSC_MANHATTAN_INPUT);
        }
        else
        {
            u8_check_status = CSC((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode&MaskForMode_LB),u8CurrentIP_Vars,pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Ratio1,3,1,E_CSC_MASERATI_INPUT_SUB);
        }

        if(0 == u8_check_status)
        {
//            printk("Error code =%d!!! InputCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
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
        if (1 == pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Manual_Vars_en)
        {
            u8CurrentIP_Vars2 = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_MC;
        }
        else
        {
            u8CurrentIP_Vars2 = pstu_Control_Param->u8TempMatrixCoefficients[1];
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
        }

        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Mode = u8IPmode;
        }
        if(0 == u8MainSubMode)
        {
            u8_check_status = CSC((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Mode&MaskForMode_LB),u8CurrentIP_Vars2,pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Ratio1,3,1,E_CSC_MANHATTAN_OUTPUT);
        }
        else
        {
            u8_check_status = CSC((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Mode&MaskForMode_LB),u8CurrentIP_Vars2,pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Ratio1,3,1,E_CSC_MASERATI_OUTPUT_SUB);
        }
        if(0 == u8_check_status)
        {
            printk("Error code =%d!!! OutCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
        //outputCSC end
    }
    else if (u8IPindex == 3)
    {
        //Degamma Start
        if (1 == pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8DeGamma_Manual_Vars_en)
        {
            u8CurrentIP_Vars = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_TR;
        }
        else
        {
            u8CurrentIP_Vars = pstu_Control_Param->u8TempTransferCharacterstics[1];
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_TR = u8CurrentIP_Vars = pstu_Control_Param->u8TempTransferCharacterstics[1];
        }

        //max luma codes
        u16Max_luma_codes = MS_Cfd_Luminance_To_PQCode(pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax, pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMaxFlag);

        u16Max_luma_codes = ((MS_U32)u16Max_luma_codes*0xff00)/(0x3ff);
        pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u16Degamma_Max_Lum = u16Max_luma_codes;

        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            u8CurrentIP_Vars = u8IPmode;
        }

        //degamma function should modify
        linearRgbDeGamma(0,E_MAIN_WINDOW);
        ST_DEGAMMA_PARAMETERS_EXTENSION st_degamma_extension;
        ST_DEGAMMA_PARAMETERS_EXTENSION* pt_degamma_extension = &st_degamma_extension;
        pt_degamma_extension->dePQclamp_en = DePQClamp_EN;

#if (1 == DePQClamp_EN)
        u16temp = MS_Cfd_Luminance_To_PQCode(pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax, 0);
        pt_degamma_extension->dePQclamp_value = ((MS_U32)u16temp*0xff00)/(0x3ff);
#else
        //give maximum values
        pt_degamma_extension->dePQclamp_value = 0xff00;
#endif
        u8_check_status = deGamma(u8CurrentIP_Vars,u16Max_luma_codes,
                                  pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_Ratio1,
                                  pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u16Degamma_Ratio2,
                                  pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_Lut_En,
                                  pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.pu32Degamma_Lut_Address,
                                  pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u16Degamma_Lut_Length,
                                  1,pstu_Control_Param->u8Temp_Format[1],0, pt_degamma_extension);
        if(0 == u8_check_status)
        {
            printk("Error code =%d!!!Degamma Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
        //Degamma End
    }
    else if(u8IPindex == 4)
    {
        //R2R3x3 start
        if (1 == pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_Manual_Vars_en)
        {
            u8CurrentIP_Vars = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_InputCP;
            u8CurrentIP_Vars2 = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_OutputCP;
        }
        else
        {
            u8CurrentIP_Vars = pstu_Control_Param->u8TempColorPriamries[1];
            u8CurrentIP_Vars2 = pstu_Control_Param->u8OutputColorPriamries;
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_InputCP = pstu_Control_Param->u8TempColorPriamries[1];
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_OutputCP = u8CurrentIP_Vars2 = pstu_Control_Param->u8OutputColorPriamries;
        }

        if (E_CFD_CFIO_CP_PANEL == u8CurrentIP_Vars2)
        {
            u8GammutMode = 0;
        }
        else
        {
            u8GammutMode = 1;
        }

        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            u8CurrentIP_Vars = u8IPmode;
        }

        u8_check_status = gamutMapping(u8CurrentIP_Vars, u8GammutMode,
                                       //&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.pstu_Panel_Param->stu_Cfd_Panel_ColorMetry) ,
                                       pstu_Control_Param->pstu_Panel_Param_Colorimetry,
                                       u8CurrentIP_Vars2,pstu_Control_Param->u8Temp_Format[1],
                                       pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u163x3_Ratio2,E_GAMUT_MAPPING_VOP2,1);


        if(0 == u8_check_status)
        {
            printk("Error code =%d!!!GamutMapping Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
        //R2R3x3 End
    }
    else if (u8IPindex == 5)
    {
        //Gamma Start
        if (1 == pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_Mode_Vars_en)
        {
            u8CurrentIP_Vars = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_TR;
        }
        else
        {
            u8CurrentIP_Vars = pstu_Control_Param->u8OutputTransferCharacterstics;
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_TR = pstu_Control_Param->u8OutputTransferCharacterstics;
        }

        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            u8CurrentIP_Vars = u8IPmode;
        }

        u16Max_luma_codes = 0xff00;
        linearRgbGamma(0,E_MAIN_WINDOW);
        u8_check_status = gamma(u8CurrentIP_Vars,u16Max_luma_codes,0,0,
                                pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_Lut_En,
                                pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.pu32Gamma_Lut_Address,
                                pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u16Gamma_Lut_Length,
                                0,pstu_Control_Param->u8Output_Format,0);

        if(0 == u8_check_status)
        {
            printk("Error code =%d!!!Gamma Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
        //Gamma End
    }
    else if (u8IPindex == 6)
    {
        //OnOff Start

        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3RGBClip_Mode = u8IPmode;
        }

        u8_check_status = rgbClip((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3RGBClip_Mode&MaskForMode_LB),E_MAIN_WINDOW);
        if(0 == u8_check_status)
        {
            printk("Error code =%d!!!LinearRGB Clip Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    }
    else if (u8IPindex == 7)
    {

        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_enable_Mode = u8IPmode;
        }


        u8_check_status = linearRgbDeGamma((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_enable_Mode&MaskForMode_LB),E_MAIN_WINDOW);
        if(0 == u8_check_status)
        {
            printk("Error code =%d!!!Degamma Enable Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    }
    else if (u8IPindex == 8)
    {

        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_Dither_Mode = u8IPmode;
        }


        u8_check_status = linearRgbDeGammaDither((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_Dither_Mode&MaskForMode_LB),E_MAIN_WINDOW);
        if(0 == u8_check_status)
        {
            printk("Error code =%d!!!Degamma Dither Enbale Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    }
    else if (u8IPindex == 9)
    {

        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_settings_Mode = u8IPmode;
        }

        u8_check_status = linearRgbComp((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_settings_Mode&MaskForMode_LB),E_MAIN_WINDOW);
        if(0 == u8_check_status)
        {
            printk("Error code =%d!!!LinearRGB Compress Enbale Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    }
    else if (u8IPindex == 10)
    {

        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_dither_Mode = u8IPmode;
        }

        u8_check_status = linearRgbCompDither((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_dither_Mode&MaskForMode_LB),E_MAIN_WINDOW);
        if(0 == u8_check_status)
        {
            printk("Error code =%d!!!LinearRGB Compress Dither Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    }
    else if (u8IPindex == 11)
    {

        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3Clip_Mode = u8IPmode;
        }


        u8_check_status = linearRgbClip((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3Clip_Mode&MaskForMode_LB),E_MAIN_WINDOW);
        if(0 == u8_check_status)
        {
            printk("Error code =%d!!!LinearRGB Clip Enbale Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    }
    else if (u8IPindex == 12)
    {

        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_enable_Mode = u8IPmode;
        }

        u8_check_status = linearRgb3x3((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_enable_Mode&MaskForMode_LB),E_MAIN_WINDOW);
        if(0 == u8_check_status)
        {
            printk("Error code =%d!!!LinearRGB 3x3 Enbale Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    }
    else if(u8IPindex == 13)
    {
        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_enable_Mode = u8IPmode;
        }

        u8_check_status = linearRgbGamma((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_enable_Mode&MaskForMode_LB),E_MAIN_WINDOW);
        if(0 == u8_check_status)
        {
            printk("Error code =%d!!!LinearRGB Gamma Enable Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    }
    else if (u8IPindex == 14)
    {
        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_Dither_Mode = u8IPmode;
        }

        u8_check_status = linearRgbGammaDither((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_Dither_Mode&MaskForMode_LB),E_MAIN_WINDOW);
        if(0 == u8_check_status)
        {
            printk("Error code =%d!!!LinearRGB Gamma Dither Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    }
    else if (u8IPindex == 15)
    {
        if(0x80 == (u8IPmode&0x80))
        {

        }
        else
        {
            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8LinearRGBBypass_Mode = u8IPmode;
        }
        if(0 == u8MainSubMode)
        {
            u8_check_status = linearRgbBypass((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8LinearRGBBypass_Mode&MaskForMode_LB),E_MAIN_WINDOW);
        }
        else
        {
            u8_check_status = linearRgbBypass((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8LinearRGBBypass_Mode&MaskForMode_LB),E_SUB_WINDOW);
        }
        if(0 == u8_check_status)
        {
            printk("Error code =%d!!!LinearRGB ByPass Enbale Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
            u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
        }
        else
        {
            u16_check_status = E_CFD_MC_ERR_NOERR;
        }
    }
    else if (u8IPindex == 16)
    {
        colorMapVipDisable();
    }
    return u16_check_status;
}

MS_U16 MS_Cfd_Maserati_SDRIP_WriteRegister(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    MS_U8 u8CurrentIP_Vars = 0,u8CurrentIP_Vars2 = 0;
    MS_U16 u16Max_luma_codes = 0;
    MS_U8 u8GammutMode = 0;
    MS_U8 u8IPindex = 0,u8IPmode = 0;
    MS_U16 u16temp;
#ifdef CFD_debug
    u8IPindex = msReadByte(REG_SC_BK30_0E_H);
    u8IPmode = msReadByte(REG_SC_BK30_0E_L);
#endif
    MS_U8 u8_check_status = 0;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_BOOL bFuncEn,bRegWriteEn;
    MS_U8 u8Mode;

    if (1 == pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_MC;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8InputMatrixCoefficients;
        pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_MC = pstu_Control_Param->u8InputMatrixCoefficients;
    }

    u8_check_status = CSC((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode&MaskForMode_LB),u8CurrentIP_Vars,pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Ratio1,3,0,E_CSC_MANHATTAN_INPUT);

    if(0 == u8_check_status)
    {
//        printk("Error code =%d!!! InputCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    //inputCSC End
    //outputCSC start
    if (1 == pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Manual_Vars_en)
    {
        u8CurrentIP_Vars2 = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_MC;
    }
    else
    {
        u8CurrentIP_Vars2 = pstu_Control_Param->u8TempMatrixCoefficients[1];
        pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
    }

    u8_check_status = CSC((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Mode&MaskForMode_LB),u8CurrentIP_Vars2,pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Ratio1,3,0,E_CSC_MANHATTAN_OUTPUT);

    if(0 == u8_check_status)
    {
        printk("Error code =%d!!! OutCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    //outputCSC end
    //Degamma Start
    if (1 == pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8DeGamma_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_TR;
    }
    else
    {
        if(E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Input_HDRMode)
        {
            u8CurrentIP_Vars = E_CFD_CFIO_TR_BT709;
        }
        else
        {
            u8CurrentIP_Vars = pstu_Control_Param->u8TempTransferCharacterstics[1];
        }
        pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_TR = u8CurrentIP_Vars;
    }
    //max luma codes
    u16Max_luma_codes = MS_Cfd_Luminance_To_PQCode(pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax, pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMaxFlag);
    //printk("TMO input u16TgtMaxFlag:%d u16TgtMaxFlag:%d u16Max_luma_codes:%d\n",pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMaxFlag,pstu_Control_Param->stu_CFD_TMO_Param.u16TgtMax,u16Max_luma_codes);
    u16Max_luma_codes = ((MS_U32)u16Max_luma_codes*0xff00)/(0x3ff);
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u16Degamma_Max_Lum = u16Max_luma_codes;


    if(E_CFD_CFIO_TR_GAMMA2P2 == u8CurrentIP_Vars)
    {
        pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_Lut_En = 1;
        pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.pu32Degamma_Lut_Address = adobeRgbMod;
        pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u16Degamma_Lut_Length = 257;

    }
    ST_DEGAMMA_PARAMETERS_EXTENSION st_degamma_extension;
    ST_DEGAMMA_PARAMETERS_EXTENSION* pt_degamma_extension = &st_degamma_extension;

    //this patch is not for SDR IP
    pt_degamma_extension->dePQclamp_en = 0;

#if (1 == DePQClamp_EN)

    u16temp = MS_Cfd_Luminance_To_PQCode(pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax, 0);
    pt_degamma_extension->dePQclamp_value = ((MS_U32)u16temp*0xff00)/(0x3ff);

#else

    //give maximum values
    pt_degamma_extension->dePQclamp_value = 0xff00;

#endif

    //degamma function should modify
    linearRgbDeGamma(0,E_MAIN_WINDOW);
    u8_check_status = deGamma(u8CurrentIP_Vars,u16Max_luma_codes,
                              pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_Ratio1,
                              pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u16Degamma_Ratio2,
                              pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_Lut_En,
                              pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.pu32Degamma_Lut_Address,
                              pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u16Degamma_Lut_Length,
                              1,pstu_Control_Param->u8Temp_Format[1],0, pt_degamma_extension);

    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!Degamma Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    //Degamma End
    //R2R3x3 start
    if (1 == pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_InputCP;
        u8CurrentIP_Vars2 = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_OutputCP;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8TempColorPriamries[1];
        u8CurrentIP_Vars2 = pstu_Control_Param->u8OutputColorPriamries;
        pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_InputCP = pstu_Control_Param->u8TempColorPriamries[1];
        pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_OutputCP = u8CurrentIP_Vars2 = pstu_Control_Param->u8OutputColorPriamries;
    }

    if (E_CFD_CFIO_CP_PANEL == u8CurrentIP_Vars2)
    {
        u8GammutMode = 0;
    }
    else
    {
        u8GammutMode = 1;
    }

    u8_check_status = gamutMapping(u8CurrentIP_Vars, u8GammutMode,
                                   //&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.pstu_Panel_Param->stu_Cfd_Panel_ColorMetry) ,
                                   pstu_Control_Param->pstu_Panel_Param_Colorimetry,
                                   u8CurrentIP_Vars2,pstu_Control_Param->u8Temp_Format[1],
                                   pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u163x3_Ratio2,E_GAMUT_MAPPING_VOP2,0);

    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!GamutMapping Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    //R2R3x3 End
    //Gamma Start
    if (1 == pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_Mode_Vars_en)
    {
        u8CurrentIP_Vars = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_TR;
    }
    else
    {
        if(E_CFIO_MODE_HDR1 == pstu_Control_Param->u8Input_HDRMode)
        {
            u8CurrentIP_Vars = E_CFD_CFIO_TR_BT709;
        }
        else
        {
            u8CurrentIP_Vars = pstu_Control_Param->u8OutputTransferCharacterstics;
        }
        pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_TR = u8CurrentIP_Vars;
    }

    u16Max_luma_codes = 0xff00;

    if (u8CurrentIP_Vars == 16)
    {
        u8CurrentIP_Vars = 6;
    }
    linearRgbGamma(0,E_MAIN_WINDOW);
    u8_check_status = gamma(u8CurrentIP_Vars,u16Max_luma_codes,0,0,
                            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_Lut_En,
                            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.pu32Gamma_Lut_Address,
                            pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u16Gamma_Lut_Length,
                            0,pstu_Control_Param->u8Output_Format,0);


    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!Gamma Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    //Gamma End
    //OnOff Start

    u8_check_status = rgbClip((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3RGBClip_Mode&MaskForMode_LB),E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!LinearRGB Clip Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    u8_check_status = linearRgbDeGamma((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_enable_Mode&MaskForMode_LB),E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!Degamma Enable Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    u8_check_status = linearRgbDeGammaDither((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_Dither_Mode&MaskForMode_LB),E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!Degamma Dither Enbale Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    u8_check_status = linearRgbComp((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_settings_Mode&MaskForMode_LB),E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!LinearRGB Compress Enbale Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    u8_check_status = linearRgbCompDither((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_dither_Mode&MaskForMode_LB),E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!LinearRGB Compress Dither Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    u8_check_status = linearRgbClip((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3Clip_Mode&MaskForMode_LB),E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!LinearRGB Clip Enbale Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    u8_check_status = linearRgb3x3((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_enable_Mode&MaskForMode_LB),E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!LinearRGB 3x3 Enbale Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    u8_check_status = linearRgbGamma((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_enable_Mode&MaskForMode_LB),E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!LinearRGB Gamma Enbale Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    u8_check_status = linearRgbGammaDither((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_Dither_Mode&MaskForMode_LB),E_MAIN_WINDOW);
    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!LinearRGB Gamma Dither Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }

    u8_check_status = linearRgbBypass((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8LinearRGBBypass_Mode&MaskForMode_LB),E_MAIN_WINDOW);

    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!LinearRGB ByPass Enbale Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    colorMapVipDisable();

    bFuncEn = FunctionMode(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8YHSL_R2Y_Mode);
    bRegWriteEn = WriteRegsiterFlag(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8YHSL_R2Y_Mode);
    u8Mode = FunctionMode(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8YHSL_R2Y_Mode);
    u8_check_status = YSHL_R2Y_Enable(               bFuncEn,bRegWriteEn,u8Mode);

    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!YHSL R2Y Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        //u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    return u16_check_status ;
}
MS_U16 MS_Cfd_Maserati_SDRIP_WriteRegister_Sub(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input)
{
    MS_U8 u8CurrentIP_Vars = 0,u8CurrentIP_Vars2 = 0;
    MS_U16 u16Max_luma_codes = 0;
    MS_U8 u8GammutMode = 0;
    MS_U8 u8IPindex = 0,u8IPmode = 0;
#ifdef CFD_debug
    u8IPindex = msReadByte(REG_SC_BK30_0E_H);
    u8IPmode = msReadByte(REG_SC_BK30_0E_L);
#endif
    MS_U8 u8_check_status = 0;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;


    if (1 == pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Manual_Vars_en)
    {
        u8CurrentIP_Vars = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_MC;
    }
    else
    {
        u8CurrentIP_Vars = pstu_Control_Param->u8InputMatrixCoefficients;
        pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_MC = pstu_Control_Param->u8InputMatrixCoefficients;
    }

    u8_check_status = CSC((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode&MaskForMode_LB),u8CurrentIP_Vars,pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Ratio1,3,0,E_CSC_MASERATI_INPUT_SUB);

    if(0 == u8_check_status)
    {
//        printk("Error code =%d!!! InputCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    //inputCSC End
    //outputCSC start
    if (1 == pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Manual_Vars_en)
    {
        u8CurrentIP_Vars2 = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_MC;
    }
    else
    {
        u8CurrentIP_Vars2 = pstu_Control_Param->u8TempMatrixCoefficients[1];
        pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_MC = pstu_Control_Param->u8TempMatrixCoefficients[1];
    }

    u8_check_status = CSC((pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Mode&MaskForMode_LB),u8CurrentIP_Vars2,pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Ratio1,3,0,E_CSC_MASERATI_OUTPUT_SUB);

    if(0 == u8_check_status)
    {
        printk("Error code =%d!!! OutCSC Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        u16_check_status = E_CFD_MC_ERR_NOERR;
    }
    //outputCSC end

    u8_check_status = linearRgbBypass(1,E_SUB_WINDOW);

    if(0 == u8_check_status)
    {
        printk("Error code =%d!!!LinearRGB ByPass Enbale Wrong Parameter [ %s  , %d]\n",E_CFD_MC_ERR_HW_IP_PARAMETERS, __FUNCTION__,__LINE__);
        u16_check_status = E_CFD_MC_ERR_HW_IP_PARAMETERS;
    }
    else
    {
        u16_check_status = E_CFD_MC_ERR_NOERR;
    }

    return u16_check_status ;
}

MS_U16 MS_Cfd_Maserati_SDRIP_Control(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input,MS_U8 u8MainSubMode)
{

    //E_CFD_MC_ERR
    MS_U8 u8_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status_tmp = E_CFD_MC_ERR_NOERR;
    MS_U8  u8ERR_Happens_Flag = 0;
    MS_U8 u8CurrentIP_Vars = 0,u8CurrentIP_Vars2 = 0;
    MS_U16 u16Max_luma_codes = 0;
    MS_U8 u8GammutMode = 0;
    MS_U8 u8IPindex = 0xFF,u8IPmode = 0;

// Decision Tree_Start
//has done in Mapi_Cfd_ColorFormatDriver_Control(STU_CFDAPI_TOP_CONTROL *pstu_Cfd_api_top)
    //pstu_SDRIP_Param->stu_Manhattan_SDRIP_Param.u8IP2_CSC_Mode = MS_Cfd_Manhattan_InputCSC_Decision(pstu_Control_Param,pstu_SDRIP_Param);

//VIP
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_CSC_Mode = MS_Cfd_Maserati_VIP_CSC_Decision(pstu_Control_Param,pstu_SDRIP_Param);
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PreYoffset_Mode = MS_Cfd_Maserati_VIP_PreYoffset_Decision(pstu_Control_Param,pstu_SDRIP_Param);
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PreYgain_Mode = MS_Cfd_Maserati_VIP_PreYgain_dither_Decision(pstu_Control_Param,pstu_SDRIP_Param);
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PreYgain_dither_Mode = MS_Cfd_Maserati_VIP_PreYgain_Decision(pstu_Control_Param,pstu_SDRIP_Param);
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PostYoffset_Mode = MS_Cfd_Maserati_VIP_PostYoffset_Decision(pstu_Control_Param,pstu_SDRIP_Param);
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PostYgain_Mode = MS_Cfd_Maserati_VIP_PostYgain_Decision(pstu_Control_Param,pstu_SDRIP_Param);
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VIP_PostYoffset2_Mode = MS_Cfd_Maserati_VIP_PostYoffset2_Decision(pstu_Control_Param,pstu_SDRIP_Param);

//UFSC
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8UFSC_YCOffset_Gain_Mode = MS_Cfd_Maserati_UFSC_YCOffsetGain_Decision(pstu_Control_Param,pstu_SDRIP_Param);

//VOP
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Mode = MS_Cfd_Maserati_OutputCSC_Decision(pstu_Control_Param,pstu_SDRIP_Param);
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3RGBClip_Mode= MS_Cfd_Maserati_OutputCSC_RGBClip_Decision(pstu_Control_Param, pstu_SDRIP_Param);

    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8LinearRGBBypass_Mode = MS_Cfd_Maserati_LinearRGB_Bypass_Decision(pstu_Control_Param,pstu_SDRIP_Param);

    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_enable_Mode = MS_Cfd_Maserati_LinearRGB_DeGamma_enable_Decision(pstu_Control_Param, pstu_SDRIP_Param);
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_Dither_Mode = MS_Cfd_Maserati_LinearRGB_DeGamma_Dither_Decision(pstu_Control_Param,pstu_SDRIP_Param);
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_SRAM_Mode = MS_Cfd_Maserati_LinearRGB_DeGamma_SRAM_Decision(pstu_Control_Param,pstu_SDRIP_Param);

    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_enable_Mode = MS_Cfd_Maserati_LinearRGB_3x3_enable_Decision(pstu_Control_Param, pstu_SDRIP_Param);
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_Mode = MS_Cfd_Maserati_LinearRGB_3x3_Decision(pstu_Control_Param, pstu_SDRIP_Param);

    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_settings_Mode = MS_Cfd_Maserati_LinearRGB_Com_Decision(pstu_Control_Param, pstu_SDRIP_Param);
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_dither_Mode = MS_Cfd_Maserati_LinearRGB_Com_Dither_Decision(pstu_Control_Param, pstu_SDRIP_Param);
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3Clip_Mode = MS_Cfd_Maserati_LinearRGB_3x3Clip_Decision(pstu_Control_Param, pstu_SDRIP_Param);

    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_enable_Mode = MS_Cfd_Maserati_LinearRGB_Gamma_enable_Decision(pstu_Control_Param, pstu_SDRIP_Param);
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_Dither_Mode = MS_Cfd_Maserati_LinearRGB_Gamma_Dither_Decision(pstu_Control_Param,pstu_SDRIP_Param);
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_maxdata_Mode = MS_Cfd_Maserati_LinearRGB_Gamma_maxdata_Decision(pstu_Control_Param,pstu_SDRIP_Param);
    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_SRAM_Mode = MS_Cfd_Maserati_LinearRGB_Gamma_SRAM_Decision(pstu_Control_Param,pstu_SDRIP_Param);


    pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8YHSL_R2Y_Mode =MS_Cfd_Maserati_YHSL_R2Y_Decision(pstu_Control_Param,pstu_SDRIP_Param);
//quick bypass control
//only not control Output CSC
    if ((0 == pstu_Control_Param->u8Input_SDRIPMode) || (2 == pstu_Control_Param->u8Input_SDRIPMode))
    {
        if (0 == pstu_Control_Param->u8Input_SDRIPMode)
        {
            function1(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        }
        function1(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8UFSC_YCOffset_Gain_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8VOP_3x3RGBClip_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8LinearRGBBypass_Mode,MS_Cfd_OnlyEnable_Decision_ON());

        function1(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Degamma_Dither_Mode,MS_Cfd_OnlyEnable_Decision_OFF());

        function1(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3_Mode,MS_Cfd_OnlyEnable_Decision_OFF());

        function1(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_settings_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Compress_dither_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u83x3Clip_Mode,MS_Cfd_OnlyEnable_Decision_OFF());

        function1(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_enable_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
        function1(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8Gamma_Dither_Mode,MS_Cfd_OnlyEnable_Decision_OFF());

        function1(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8YHSL_R2Y_Mode,MS_Cfd_OnlyEnable_Decision_OFF());
    }

    u16_check_status_tmp = Mapi_Cfd_Maserati_SDRIP_Param_Check(&(pstu_SDRIP_Param->stu_Maserati_SDRIP_Param));

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }
// Decision Tree_End

#ifdef CFD_debug
    u8IPindex = msReadByte(REG_SC_BK30_0E_H);
    u8IPmode = msReadByte(REG_SC_BK30_0E_L);
#endif

// Input to Physical Layer by Ali Start
    if(u8IPindex ==0xFF)
    {
        if(0 == u8MainSubMode)
        {
            u16_check_status_tmp = MS_Cfd_Maserati_SDRIP_WriteRegister(pstu_Control_Param,pstu_SDRIP_Param,pstu_TMO_Input);
        }
        else
        {
            u16_check_status_tmp = MS_Cfd_Maserati_SDRIP_WriteRegister_Sub(pstu_Control_Param,pstu_SDRIP_Param,pstu_TMO_Input);
        }
    }
    else
    {
        u16_check_status_tmp = MS_Cfd_Maserati_SDRIP_WriteRegister_DebugMode(pstu_Control_Param,pstu_SDRIP_Param,pstu_TMO_Input,u8MainSubMode);
    }
// Input to Physical Layer by Ali End

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

    //not test mode
    return u16_check_status ;
}

MS_U16 MS_Cfd_Maserati_SetGM_TMO_Flags(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam)
{

//E_CFD_MC_ERR
//MS_U8 u8_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;

//for HDRP IP
//MS_U8 u8DoTMOInHDRIP_Flag;
//MS_U8 u8DoGamutMappingInHDRIP_Flag;
//MS_U8 u8DoDLCInHDRIP_Flag;

//not Dolby input case
    if (E_CFIO_MODE_HDR1 != pstu_ControlParam->u8Temp_HDRMode[0])
    {
#if 0
        if ((E_CFD_CFIO_YUV_BT2020_CL == pstu_ControlParam->u8Temp_Format[0]) && (E_CFD_CFIO_YUV_BT2020_CL != pstu_ControlParam->u8Temp_Format[1]))
        {
            pstu_ControlParam->u8DoBT2020CLPInHDRIP_Flag = 1;
        }
        else
        {
            pstu_ControlParam->u8DoBT2020CLPInHDRIP_Flag = 0;
        }
#endif
        //current design is not supported this process
        pstu_ControlParam->u8DoBT2020CLPInHDRIP_Flag = 0;


        if (pstu_ControlParam->u8Temp_GamutOrderIdx[1] != pstu_ControlParam->u8Temp_GamutOrderIdx[0] )
        {
            pstu_ControlParam->u8DoGamutMappingInHDRIP_Flag = 1;
        }
        else
        {
            pstu_ControlParam->u8DoGamutMappingInHDRIP_Flag = 0;
        }

        if ((E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[0] ) && (E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[1] ))
        {
            pstu_ControlParam->u8DoTMOInHDRIP_Flag = 0;
        }
        //HDR to SDR
        else if ((E_CFIO_MODE_SDR != pstu_ControlParam->u8Temp_HDRMode[0] ) && (E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[1] ))
        {
            pstu_ControlParam->u8DoTMOInHDRIP_Flag = 1;
        }
        //SDR to HDR (not ready now)
        else if ((E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[0] ) && (E_CFIO_MODE_SDR != pstu_ControlParam->u8Temp_HDRMode[1] ))
        {
            pstu_ControlParam->u8DoTMOInHDRIP_Flag = 0;
            u16_check_status = E_CFD_MC_ERR_HW_IPS_SDR2HDR_NOTSUPPORTED;
        }
        //HDR to HDR
        else if ((E_CFIO_MODE_SDR != pstu_ControlParam->u8Temp_HDRMode[0] ) && (E_CFIO_MODE_SDR != pstu_ControlParam->u8Temp_HDRMode[1] ))
        {
            if (pstu_ControlParam->u8Temp_HDRMode[0] == pstu_ControlParam->u8Temp_HDRMode[1])//HDR to HDR, the same HDR
            {
                pstu_ControlParam->u8DoTMOInHDRIP_Flag = 0;
            }
            else////HDR to HDR, different HDR, not support now
            {
                pstu_ControlParam->u8DoTMOInHDRIP_Flag = 0;
                u16_check_status = E_CFD_MC_ERR_HW_IPS_HDRXtoHDRY_NOTSUPPORTED;
            }
        }
        else
        {
            pstu_ControlParam->u8DoTMOInHDRIP_Flag = 0;
        }
    }
    else //Dobly HDR in, force TMO
    {
        //do nothing now , control by Dolby metadata
    }

//for SDR IP
//MS_U8 u8DoTMO_Flag;
//MS_U8 u8DoGamutMapping_Flag;
//MS_U8 u8DoDLC_Flag;

    if ((E_CFD_CFIO_YUV_BT2020_CL == pstu_ControlParam->u8Temp_Format[1]) &&
        (E_CFD_MC_FORMAT_RGB != pstu_ControlParam->u8Temp_DataFormat[1]) &&
        (E_CFD_CFIO_YUV_BT2020_CL != pstu_ControlParam->u8Output_Format) &&
        (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_Format))
    {
        pstu_ControlParam->u8DoBT2020CLP_Flag = 1;
    }
    else
    {
        pstu_ControlParam->u8DoBT2020CLP_Flag = 0;
    }

    if ((pstu_ControlParam->u8Output_GamutOrderIdx != pstu_ControlParam->u8Temp_GamutOrderIdx[1]) && (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_Format))
    {
        pstu_ControlParam->u8DoGamutMapping_Flag = 1;
    }
    else
    {
        pstu_ControlParam->u8DoGamutMapping_Flag = 0;
    }

//SDR to SDR
    if ((E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[1] ) && (E_CFIO_MODE_SDR == pstu_ControlParam->u8Output_HDRMode ))
    {
        pstu_ControlParam->u8DoTMO_Flag = 0;
    }

//HDR to SDR
    else if ((E_CFIO_MODE_SDR != pstu_ControlParam->u8Temp_HDRMode[1] ) && (E_CFIO_MODE_SDR == pstu_ControlParam->u8Output_HDRMode ) && (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_Format))
    {
        pstu_ControlParam->u8DoTMO_Flag = 1;
    }

//SDR to HDR (not ready now)
    else if ((E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[1] ) && (E_CFIO_MODE_SDR != pstu_ControlParam->u8Output_HDRMode ))
    {
        pstu_ControlParam->u8DoTMO_Flag = 0;
        u16_check_status = E_CFD_MC_ERR_HW_IPS_SDR2HDR_NOTSUPPORTED;
    }
    else if ((E_CFIO_MODE_SDR != pstu_ControlParam->u8Temp_HDRMode[1] ) && (E_CFIO_MODE_SDR != pstu_ControlParam->u8Output_HDRMode ))
    {
        if (pstu_ControlParam->u8Temp_HDRMode[1] == pstu_ControlParam->u8Output_HDRMode)//HDR to HDR, the same HDR
        {
            pstu_ControlParam->u8DoTMO_Flag = 0;
        }
        else////HDR to HDR, different HDR, not support now
        {
            pstu_ControlParam->u8DoTMO_Flag = 0;
            u16_check_status = E_CFD_MC_ERR_HW_IPS_HDRXtoHDRY_NOTSUPPORTED;
        }
    }
    else
    {
        pstu_ControlParam->u8DoTMO_Flag = 0;
        //u16_check_status = E_CFD_MC_ERR_WRONGTMOSET;
        //return u16_check_status;
    }

#if 0
    if ((1 == pstu_ControlParam->u8DoTMO_Flag) && (1 == pstu_ControlParam->u8DoTMOInHDRIP_Flag))
    {
        u16_check_status = E_CFD_MC_ERR_WRONGTMOSET;
    }
#endif


    if (E_CFD_MC_ERR_NOERR != u16_check_status)
    {
        HDR_DBG_HAL_CFD(printk("\n Error:sothing wrong in set TMO GM flags function \n"));
        HDR_DBG_HAL_CFD(printk("\n Error code = %04x !!!, error is in [ %s  , %d] \n",u16_check_status, __FUNCTION__,__LINE__));
    }

    return u16_check_status;
}
MS_U16 MS_Cfd_Maserati_ColorSampling_Control(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param)
{
    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_BOOL bFuncEn,bRegWriteEn;
    MS_U8 u8Mode;
    ST_HDR_CUP CupStruct;
    memset(&CupStruct, 0, sizeof(ST_HDR_CUP));
    if(1 == pstu_Control_Param->u8Input_HDRIPMode)
    {
        if(E_CFD_INPUT_SOURCE_HDMI == pstu_Control_Param->u8Input_Source)
        {
        
#if defined HW_NO_HDR1
            u8Mode = 1;
#else
            if(E_CFD_MC_FORMAT_YUV422 == pstu_Control_Param->u8Input_DataFormat && TRUE == pstu_Control_Param->u16DolbySupportStatus)
            {
                u8Mode = 0;
            }
            else
            {
                u8Mode = 1;
            }
#endif

        }
        else //mm case
        {
            u8Mode = 1;
        }
        HdrCupB0101(1,1,u8Mode, &CupStruct);
    }
    return u16_check_status;
}
