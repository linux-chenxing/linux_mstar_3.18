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

#ifndef _HAL_COLOR_FORMAT_DRIVER_C
#define _HAL_COLOR_FORMAT_DRIVER_C
#endif

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include "mdrv_mstypes.h"
#include "mhal_xc.h"
#include "color_format_input.h"
#include "color_format_driver.h"
#include "color_format_ip.h"
#include "mhal_dlc.h"

extern StuDlc_HDRinit g_HDRinitParameters;
extern StuDlc_FinetuneParamaters g_DlcParameters;
extern HDR_CONTENT_LIGHT_MATADATA STU_HDR_METADATA;
#define CFD_Current_MAX_AnalogIdx 66

volatile MS_U8 u8IP2_CSC_Ratio1;
volatile MS_U8 u8VOP_3x3_Ratio1;
volatile MS_U8 u8Degamma_Ratio1;
volatile MS_U16 u16Degamma_Ratio2;
volatile MS_U16 u163x3_Ratio2;

#define TEST_CODING_MMIN 0

//0:nothing before HDR IP
//1:IP2 CSC before HDR IP
//#define HW_PIPE_MODE 1


static MS_BOOL bCFDrun = FALSE;
MS_U8 LUTU8_CFD_ANALOGIN_COLORFORMAT[CFD_Current_MAX_AnalogIdx] =
{
    9,9,8,8,8,8,8,9,
    9,8,8,8,8,8,9,9,
    8,8,8,8,8,9,9,8,
    8,8,8,8,9,9,8,8,
    8,8,8,9,9,8,8,8,
    8,8,2,1,8,8,8,8,
    8,8,8,10,10,10,10,10, //48-55
    5,5,5,5,5,5,5,5,   //56-63
    5,5                   //64-65
};

MS_U8 LUTU8_CFD_ANALOGIN_MC[CFD_Current_MAX_AnalogIdx] =
{
    6,6,5,5,5,5,5,6,
    6,5,5,5,5,5,6,6,
    5,5,5,5,5,6,6,5,
    5,5,5,5,6,6,5,5,
    5,5,5,6,6,5,5,5,
    5,5,6,5,5,5,5,5,
    5,5,5,1,1,1,1,1,      //48-55
    5,5,5,5,5,5,5,5,   //56-63
    5,5                   //64-65
};

//LUT for calculation of HDMI EDID u8HDMISink_Desired_Content_Max_Luminance/u8HDMISink_Desired_Content_Max_Frame_Avg_Luminance
MS_U16 LUT_CFD_CV1[256] =
{
    0x0032, 0x0033, 0x0034, 0x0035, 0x0037, 0x0038, 0x0039, 0x003a, 0x003b, 0x003d, 0x003e, 0x003f, 0x0041, 0x0042, 0x0044, 0x0045,
    0x0047, 0x0048, 0x004a, 0x004b, 0x004d, 0x004f, 0x0051, 0x0052, 0x0054, 0x0056, 0x0058, 0x005a, 0x005c, 0x005e, 0x0060, 0x0062,
    0x0064, 0x0066, 0x0068, 0x006b, 0x006d, 0x006f, 0x0072, 0x0074, 0x0077, 0x007a, 0x007c, 0x007f, 0x0082, 0x0085, 0x0087, 0x008a,
    0x008d, 0x0091, 0x0094, 0x0097, 0x009a, 0x009e, 0x00a1, 0x00a5, 0x00a8, 0x00ac, 0x00b0, 0x00b3, 0x00b7, 0x00bb, 0x00c0, 0x00c4,
    0x00c8, 0x00cc, 0x00d1, 0x00d5, 0x00da, 0x00df, 0x00e4, 0x00e9, 0x00ee, 0x00f3, 0x00f8, 0x00fe, 0x0103, 0x0109, 0x010f, 0x0115,
    0x011b, 0x0121, 0x0127, 0x012e, 0x0134, 0x013b, 0x0142, 0x0149, 0x0150, 0x0158, 0x015f, 0x0167, 0x016f, 0x0177, 0x017f, 0x0187,
    0x0190, 0x0199, 0x01a2, 0x01ab, 0x01b4, 0x01be, 0x01c8, 0x01d1, 0x01dc, 0x01e6, 0x01f1, 0x01fc, 0x0207, 0x0212, 0x021e, 0x022a,
    0x0236, 0x0242, 0x024f, 0x025c, 0x0269, 0x0276, 0x0284, 0x0292, 0x02a1, 0x02af, 0x02bf, 0x02ce, 0x02de, 0x02ee, 0x02fe, 0x030f,
    0x0320, 0x0332, 0x0343, 0x0356, 0x0368, 0x037c, 0x038f, 0x03a3, 0x03b7, 0x03cc, 0x03e1, 0x03f7, 0x040d, 0x0424, 0x043b, 0x0453,
    0x046b, 0x0484, 0x049d, 0x04b7, 0x04d2, 0x04ed, 0x0508, 0x0525, 0x0541, 0x055f, 0x057d, 0x059c, 0x05bb, 0x05db, 0x05fc, 0x061e,
    0x0640, 0x0663, 0x0687, 0x06ab, 0x06d1, 0x06f7, 0x071e, 0x0746, 0x076f, 0x0798, 0x07c3, 0x07ee, 0x081b, 0x0848, 0x0877, 0x08a6,
    0x08d7, 0x0908, 0x093b, 0x096f, 0x09a4, 0x09da, 0x0a11, 0x0a49, 0x0a83, 0x0abe, 0x0afa, 0x0b38, 0x0b76, 0x0bb7, 0x0bf8, 0x0c3b,
    0x0c80, 0x0cc6, 0x0d0e, 0x0d57, 0x0da2, 0x0dee, 0x0e3c, 0x0e8c, 0x0edd, 0x0f31, 0x0f86, 0x0fdd, 0x1036, 0x1091, 0x10ee, 0x114d,
    0x11ad, 0x1211, 0x1276, 0x12dd, 0x1347, 0x13b3, 0x1422, 0x1492, 0x1506, 0x157c, 0x15f4, 0x166f, 0x16ed, 0x176d, 0x17f1, 0x1877,
    0x1900, 0x198c, 0x1a1b, 0x1aae, 0x1b43, 0x1bdc, 0x1c78, 0x1d18, 0x1dbb, 0x1e62, 0x1f0c, 0x1fba, 0x206c, 0x2122, 0x21db, 0x2299,
    0x235b, 0x2421, 0x24ec, 0x25bb, 0x268e, 0x2766, 0x2843, 0x2925, 0x2a0b, 0x2af7, 0x2be8, 0x2cde, 0x2dda, 0x2edb, 0x2fe1, 0x30ee
};

//LUT for calculation of HDMI EDID u8HDMISink_Desired_Content_Min_Luminance
MS_U16 LUT_CFD_CV2[256] =
{
    0x0000, 0x0001, 0x0003, 0x0006, 0x000a, 0x0010, 0x0017, 0x0020, 0x0029, 0x0034, 0x0041, 0x004e, 0x005d, 0x006d, 0x007e, 0x0091,
    0x00a5, 0x00ba, 0x00d1, 0x00e9, 0x0102, 0x011c, 0x0138, 0x0155, 0x0174, 0x0193, 0x01b4, 0x01d6, 0x01fa, 0x021e, 0x0245, 0x026c,
    0x0295, 0x02be, 0x02ea, 0x0316, 0x0344, 0x0373, 0x03a3, 0x03d5, 0x0408, 0x043c, 0x0472, 0x04a9, 0x04e1, 0x051a, 0x0555, 0x0591,
    0x05ce, 0x060d, 0x064d, 0x068e, 0x06d0, 0x0714, 0x0759, 0x079f, 0x07e7, 0x0830, 0x087a, 0x08c5, 0x0912, 0x0960, 0x09af, 0x0a00,
    0x0a52, 0x0aa5, 0x0afa, 0x0b50, 0x0ba7, 0x0bff, 0x0c59, 0x0cb4, 0x0d10, 0x0d6d, 0x0dcc, 0x0e2c, 0x0e8e, 0x0ef0, 0x0f54, 0x0fba,
    0x1020, 0x1088, 0x10f1, 0x115c, 0x11c7, 0x1234, 0x12a3, 0x1312, 0x1383, 0x13f5, 0x1469, 0x14dd, 0x1554, 0x15cb, 0x1643, 0x16bd,
    0x1739, 0x17b5, 0x1833, 0x18b2, 0x1932, 0x19b4, 0x1a37, 0x1abb, 0x1b41, 0x1bc7, 0x1c50, 0x1cd9, 0x1d64, 0x1df0, 0x1e7d, 0x1f0b,
    0x1f9b, 0x202c, 0x20bf, 0x2153, 0x21e8, 0x227e, 0x2315, 0x23ae, 0x2448, 0x24e4, 0x2581, 0x261f, 0x26be, 0x275f, 0x2800, 0x28a4,
    0x2948, 0x29ee, 0x2a95, 0x2b3d, 0x2be7, 0x2c92, 0x2d3e, 0x2dec, 0x2e9a, 0x2f4b, 0x2ffc, 0x30af, 0x3163, 0x3218, 0x32ce, 0x3386,
    0x343f, 0x34fa, 0x35b5, 0x3672, 0x3731, 0x37f0, 0x38b1, 0x3973, 0x3a37, 0x3afb, 0x3bc2, 0x3c89, 0x3d51, 0x3e1b, 0x3ee7, 0x3fb3,
    0x4081, 0x4150, 0x4220, 0x42f2, 0x43c5, 0x4499, 0x456e, 0x4645, 0x471d, 0x47f7, 0x48d1, 0x49ad, 0x4a8b, 0x4b69, 0x4c49, 0x4d2a,
    0x4e0c, 0x4ef0, 0x4fd5, 0x50bb, 0x51a3, 0x528c, 0x5376, 0x5461, 0x554e, 0x563c, 0x572b, 0x581c, 0x590e, 0x5a01, 0x5af6, 0x5beb,
    0x5ce2, 0x5ddb, 0x5ed4, 0x5fcf, 0x60cb, 0x61c9, 0x62c8, 0x63c8, 0x64c9, 0x65cc, 0x66d0, 0x67d5, 0x68dc, 0x69e3, 0x6aec, 0x6bf7,
    0x6d03, 0x6e10, 0x6f1e, 0x702d, 0x713e, 0x7250, 0x7364, 0x7478, 0x758e, 0x76a6, 0x77be, 0x78d8, 0x79f3, 0x7b10, 0x7c2e, 0x7d4d,
    0x7e6d, 0x7f8f, 0x80b2, 0x81d6, 0x82fb, 0x8422, 0x854a, 0x8673, 0x879e, 0x88ca, 0x89f7, 0x8b26, 0x8c56, 0x8d87, 0x8eb9, 0x8fed,
    0x9122, 0x9258, 0x9390, 0x94c8, 0x9602, 0x973e, 0x987b, 0x99b9, 0x9af8, 0x9c38, 0x9d7a, 0x9ebe, 0xa002, 0xa148, 0xa28f, 0xa3d7
};

//Mapping color gamut of MM VUI to E_CFD_CFIO_GAMUTORDER_IDX
//E_CFD_CFIO_CP
//idx is from u8InputColorPriamries

MS_U8 u8_gamut_idx_order[13] =
{
    E_CFD_CFIO_GT_BT709,
    E_CFD_CFIO_GT_BT709, //E_CFD_CFIO_GT_XVYCC
    E_CFD_CFIO_GT_BT709,
    E_CFD_CFIO_GT_BT709,
    E_CFD_CFIO_GT_BT4706,
    E_CFD_CFIO_GT_BT601_625,
    E_CFD_CFIO_GT_BT601_525,
    E_CFD_CFIO_GT_BT601_525,
    E_CFD_CFIO_GT_GF,
    E_CFD_CFIO_GT_BT2020,
    E_CFD_CFIO_GT_XYZ,
    E_CFD_CFIO_GT_ADOBERGB,
    E_CFD_CFIO_GT_PANEL_reservedstart
};

#if 0
typedef enum
{
    E_CFD_CFIO_CP_RESERVED0    = 0x0, //means RGB, but no specific colorspace
    E_CFD_CFIO_CP_BT709_SRGB_SYCC  = 0x1,
    E_CFD_CFIO_CP_UNSPECIFIED   = 0x2,
    E_CFD_CFIO_CP_RESERVED3    = 0x3,
    E_CFD_CFIO_CP_BT470_6    = 0x4,
    E_CFD_CFIO_CP_BT601625             = 0x5,
    E_CFD_CFIO_CP_BT601525_SMPTE170M = 0x6,
    E_CFD_CFIO_CP_SMPTE240M    = 0x7,
    E_CFD_CFIO_CP_GENERIC_FILM   = 0x8,
    E_CFD_CFIO_CP_BT2020    = 0x9,
    E_CFD_CFIO_CP_CIEXYZ    = 0xA,
    E_CFD_CFIO_CP_ADOBERGB    = 0xB,
    E_CFD_CFIO_CP_PANEL        ,
    E_CFD_CFIO_CP_UNDEFINED

} E_CFD_CFIO_CP;
#endif

/*
void Mapi_Cfd_TMO_Param_Init(STU_CFDAPI_TMO_CONTROL *pstu_TMO_Param)
{

 //percentage
 //pstu_TMO_Param->u16SrcMinRatio = 10;        //source minimum, 0.10 format
 //pstu_TMO_Param->u16SrcMedRatio = 512;        //source median (bisection point), 0.10 format
 //pstu_TMO_Param->u16SrcMaxRatio = 990;        //source maximum, 0.10 format

 //target_min_code = 47
 //target_max_code = 634
 //pstu_TMO_Param->u16TgtMin = 500;  //target minimum in nits, 1~10000
 //pstu_TMO_Param->u16TgtMax = 300;        //target maximum in nits, 1~10000
 //pstu_TMO_Param->u16TgtMinFlag = 1;  //flag of target minimum
           //0: the unit of u16Luminance is 1 nits
           //1: the unit of u16Luminance is 0.0001 nits

 //pstu_TMO_Param->u16TgtMaxFlag = 0;      //flag of target maximum
           //0: the unit of u16Luminance is 1 nits
           //1: the unit of u16Luminance is 0.0001 nits

 //pstu_TMO_Param->u16FrontSlopeMin = 256;      //slope minimum of curve that index smaller than bisection point, 2.8 format
 //pstu_TMO_Param->u16FrontSlopeMax = 512;      //slope maximum of curve that index smaller than bisection point, 2.8 format
 //pstu_TMO_Param->u16BackSlopeMin = 128;       //slope minimum of curve that index larger than bisection point, 2.8 format
 //pstu_TMO_Param->u16BackSlopeMax = 256;       //slope maximum of curve that index larger than bisection point, 2.8 format

 //pstu_TMO_Param->u16SceneChangeThrd = 1024;         //scene change threshold,
 //pstu_TMO_Param->u16SceneChangeRatioMax = 1024;     //scene change ratio maximum

 //pstu_TMO_Param->u8IIRRatio = 31;     //IIR ratio, maximum is 0x20

 //0 : keeps the value in initial function
 //1 : from output source, such as Panel information
 //pstu_TMO_Param->u8TMO_TargetMode = 1;
 //pstu_TMO_Param->u8TMO_TargetMode = 1;

}
*/


//for MS_U8 MS_Cfd_InputMM_SetColorMetry()
MS_U8 u8Const_LUT_MMInfor_HDMIDefinition[9][5] =
{
    {5,6,5,E_CFD_CFIO_RGB_BT601_625,E_CFD_CFIO_YUV_BT601_625}, //0:BT601_625 RGB/YUV
    {6,6,6,E_CFD_CFIO_RGB_BT601_525,E_CFD_CFIO_YUV_BT601_525}, //1:BT601_525 RGB/YUV
    {1,1,1,E_CFD_CFIO_RGB_BT709,E_CFD_CFIO_YUV_BT709},   //2:BT601_709 RGB/YUV
    {9,14,9,E_CFD_CFIO_RGB_BT2020,E_CFD_CFIO_YUV_BT2020_NCL}, //3:BT601_2020ncl RGB/YUV
    {9,15,10,E_CFD_CFIO_RGB_BT2020,E_CFD_CFIO_YUV_BT2020_CL}, //4:BT601_2020cl  RGB/YUV
    {1,13,5,E_CFD_CFIO_SRGB,E_CFD_CFIO_SYCC601},    //5:sRGB/sYCC601 RGB/YUV
    {11,18,5,E_CFD_CFIO_ADOBE_RGB,E_CFD_CFIO_ADOBE_YCC601},  //6:AdobeRGB/AdobeYCC601 RGB/YUV
    {1,11,5,E_CFD_CFIO_RGB_NOTSPECIFIED,E_CFD_CFIO_XVYCC_601},   //7:xvYCC601 only YUV
    {1,11,1,E_CFD_CFIO_RGB_NOTSPECIFIED,E_CFD_CFIO_XVYCC_709},   //8:xvYCC709 only YUV
};
//check if this HDR is openHDR or not
MS_U8 MS_Cfd_CheckOpenHDR(MS_U8 u8HDR_mode)
{
    MS_U8 u8Status = 0;

    if ((E_CFIO_MODE_HDR2 == u8HDR_mode)||(E_CFIO_MODE_HDR3 == u8HDR_mode))
    {
        u8Status = 1;
    }
    else
    {
        u8Status = 0;
    }

    return u8Status;
}

//assign current TR based on HDR mode
MS_U8 MS_Cfd_GetTR_FromHDRMode(MS_U8 u8HDR_mode, MS_U8 u8_tr)
{
    MS_U8 u8Curr_TR = 0;

    u8Curr_TR = u8_tr;

    if (E_CFIO_MODE_HDR2 == u8HDR_mode)
    {
        u8Curr_TR = E_CFD_CFIO_TR_SMPTE2084;
    }
    else if (E_CFIO_MODE_HDR3 == u8HDR_mode)
    {
        u8Curr_TR = E_CFD_CFIO_TR_HLG;
    }
    else if (E_CFIO_MODE_HDR1 == u8HDR_mode)
    {
        u8Curr_TR = E_CFD_CFIO_TR_UNSPECIFIED;
    }
    //
    //else
    //keep the same

    return u8Curr_TR;
}

//table[i][0] = Color primaries
//table[i][1] = Transfer characteristics
//table[i][2] = Matrix coefficient
//table[i][3] = RGB
//table[i][4] = YUV

//specify which color space is supported by HDMI sink ++++++++++++++++++++++++++++++++++++++++

//for current design of Kano
//only support 4 main gamuts

//MS_U8 u8_gamut_array[4][3] =
//{
// {0,1,E_CFD_CFIO_GT_BT601_525},
// {0,2,E_CFD_CFIO_GT_BT709},
// {0,2,E_CFD_CFIO_GT_ADOBERGB},
// {0,2,E_CFD_CFIO_GT_BT2020}
//};

//[x][0] -> 0:not supported by EDID, 1:supported by EDID
//[x][1] -> 0:only RGB, 1:only YUV 2:both
//[x][2] -> colormetry supported by this HW
void MS_Cfd_SetOutputGamut_HDMISink(STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParamInit,STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam,STU_CFDAPI_MAIN_CONTROL *pstApiControlParamInit)
{
//from 0 ~ 8
    /*
    typedef enum
    {
     E_CFD_CFIO_GT_BT601_525  = 0x0, //used colorspace has min gamut
             //SMPTE 170M, SMPTE 240M

     E_CFD_CFIO_GT_BT709   = 0x1, //sRGB
     E_CFD_CFIO_GT_BT601_625  = 0x2,
     E_CFD_CFIO_GT_ADOBERGB   = 0x3, //AdobeRGB/YUV
     E_CFD_CFIO_GT_BT4706     = 0x4, //BT470_6
     E_CFD_CFIO_GT_GF      = 0x5, //Generic film
     E_CFD_CFIO_GT_XVYCC      = 0x6, //sYCC601,xvYCC601,xvYCC709
     E_CFD_CFIO_GT_BT2020  = 0x7, //BT2020
     E_CFD_CFIO_GT_XYZ      = 0x8, //CIE XYZ

     E_CFD_CFIO_GT_PANEL      = 0x9  //Panel or user-defined gamut

    }E_CFD_CFIO_GAMUTORDER_IDX;
    */

    MS_U8 u8_gamut_array[4][3] =
    {
        {0,1,E_CFD_CFIO_GT_BT601_525},
        {1,2,E_CFD_CFIO_GT_BT709},
        {0,2,E_CFD_CFIO_GT_ADOBERGB},
        {0,2,E_CFD_CFIO_GT_BT2020}
    };

    MS_U8 temp = 0;
    MS_U8 temp_lb = 0;
    MS_U8 temp_hb = 0;
    MS_U8 temp_hb_valid = 0;
    MS_U8 temp_lb_valid = 0;
    MS_U8 u8Match_Flag = 0;

    //RGB is supported by Sink in default
    //Sink support YUV
    if ((pstHDMI_EDIDParamInit->u8HDMISink_Support_YUVFormat&0x07)!=0)
    {
        u8_gamut_array[0][0] = 1;
    }

    //Even sink support xvYCC , but Kano not support
    //E_CFD_CFIO_GT_ADOBERGB
    if (((pstHDMI_EDIDParamInit->u8HDMISink_Extended_Colorspace>>4)&0x01)==0x01)
    {
        u8_gamut_array[2][0] = 1;
    }

    //E_CFD_CFIO_GT_BT2020
    if (((pstHDMI_EDIDParamInit->u8HDMISink_Extended_Colorspace>>5)&0x07)!=0x00)
    {
        u8_gamut_array[3][0] = 1;
    }

    //specify which color space is supported by HDMI sink ----------------------------------------

    //stu_ControlParam.u8Temp_GamutOrderIdx[1] = 100;
    //for (MS_U8 temp1 = 0;temp1<=8;temp1++)
    {
        //pstu_ControlParam->u8Output_DataFormat = E_CFD_MC_FORMAT_RGB;
        //stu_ControlParam.u8Temp_GamutOrderIdx[1] = temp1;
        //pstu_ControlParam->u8Temp_GamutOrderIdx[1] = 8;

        temp_hb_valid = 0;
        temp_lb_valid = 0;

        for (temp = 0; temp<=3; temp++)
        {

            u8Match_Flag = 0;


            if (1 == u8_gamut_array[temp][0]) //this case is avaliable from the information from EDID
            {
                //RGB
                if ((E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_DataFormat) && ((0 == u8_gamut_array[temp][1] ) || (2 == u8_gamut_array[temp][1] )))
                {
                    u8Match_Flag = 1;
                }
                //YUV
                else if ((E_CFD_MC_FORMAT_RGB != pstu_ControlParam->u8Output_DataFormat) && ((1 == u8_gamut_array[temp][1] ) || (2 == u8_gamut_array[temp][1] )))
                {
                    u8Match_Flag = 1;
                }

                if ((1 == u8_gamut_array[temp][0]) && (pstu_ControlParam->u8Temp_GamutOrderIdx[1]>=u8_gamut_array[temp][2]) && (1 == u8Match_Flag))
                {
                    temp_lb = u8_gamut_array[temp][2];
                    temp_lb_valid = 1;
                }

                if ((1 == u8_gamut_array[temp][0]) && (pstu_ControlParam->u8Temp_GamutOrderIdx[1]<=u8_gamut_array[temp][2]) && (1 == u8Match_Flag))
                {
                    temp_hb = u8_gamut_array[temp][2];
                    temp_hb_valid = 1;
                    break;
                }
            }
        }

        if ((1 == temp_lb_valid) && (1 == temp_hb_valid))
        {
            if (temp_lb == temp_hb)
            {
                pstu_ControlParam->u8Output_GamutOrderIdx = temp_hb;
            }
            else
            {
                //gamut extension
                if (0 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_MethodMode)
                {
                    pstu_ControlParam->u8Output_GamutOrderIdx = temp_hb;
                }
                else //gamut compression
                {
                    pstu_ControlParam->u8Output_GamutOrderIdx = temp_lb;
                }
            }
        }
        else if ((1 == temp_lb_valid) && (0 == temp_hb_valid))
        {
            pstu_ControlParam->u8Output_GamutOrderIdx = temp_lb;
        }
        else if ((1 == temp_hb_valid) && (0 == temp_lb_valid))
        {
            pstu_ControlParam->u8Output_GamutOrderIdx = temp_hb;
        }
        else //force 709
        {
            pstu_ControlParam->u8Output_GamutOrderIdx = E_CFD_CFIO_GT_BT709;
        }

    } //for test - for (MS_U8 temp1 = 0;temp1<=8;temp1++)

}

MS_U8 MS_Cfd_SetFormat_OnlyR2Y(MS_U8 u8input_format)
{
    MS_U8 u8temp = 0;


    switch(u8input_format)
    {
        case E_CFD_CFIO_RGB_NOTSPECIFIED:
            u8temp = E_CFD_CFIO_YUV_NOTSPECIFIED;
            break;
        case E_CFD_CFIO_RGB_BT601_625:
            u8temp = E_CFD_CFIO_YUV_BT601_625;
            break;
        case E_CFD_CFIO_RGB_BT601_525:
            u8temp = E_CFD_CFIO_YUV_BT601_525;
            break;
        case E_CFD_CFIO_RGB_BT709:
            u8temp = E_CFD_CFIO_YUV_BT709;
            break;
        case E_CFD_CFIO_RGB_BT2020:
            u8temp = E_CFD_CFIO_YUV_BT2020_NCL;
            break;
        case E_CFD_CFIO_SRGB:
            u8temp = E_CFD_CFIO_SYCC601;
            break;
        case E_CFD_CFIO_ADOBE_RGB:
            u8temp = E_CFD_CFIO_ADOBE_YCC601;
            break;
        default:
            u8temp = E_CFD_CFIO_YUV_NOTSPECIFIED;
            break;
    }

    return u8temp;
}

void MS_Cfd_inside_control_Debug(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam)
{
    printf("u8Input_Format                         :0x%02x\n", pstu_ControlParam->u8Input_Format         );
    printf("u8Temp_Format[0]                       :0x%02x\n", pstu_ControlParam->u8Temp_Format[0]         );
    printf("u8Temp_Format[1]                       :0x%02x\n", pstu_ControlParam->u8Temp_Format[1]         );
    printf("u8Output_Format                        :0x%02x\n", pstu_ControlParam->u8Output_Format         );
    printf("\n");

    printf("u8Input_SDRIPMode                      :0x%02x\n", pstu_ControlParam->u8Input_SDRIPMode         );
    printf("u8Input_HDRIPMode                      :0x%02x\n", pstu_ControlParam->u8Input_HDRIPMode         );
    printf("\n");

    printf("u8Input_DataFormat                     :0x%02x\n", pstu_ControlParam->u8Input_DataFormat         );
    printf("u8Input_IsFullRange                    :0x%02x\n", pstu_ControlParam->u8Input_IsFullRange         );
    printf("u8Input_HDRMode                        :0x%02x\n", pstu_ControlParam->u8Input_HDRMode         );
    printf("u8Input_GamutOrderIdx                   :0x%02x\n", pstu_ControlParam->u8Input_GamutOrderIdx         );
    printf("u8InputColorPriamries                   :0x%02x\n", pstu_ControlParam->u8InputColorPriamries         );
    printf("u8InputTransferCharacterstics          :0x%02x\n", pstu_ControlParam->u8InputTransferCharacterstics         );
    printf("u8InputMatrixCoefficients              :0x%02x\n", pstu_ControlParam->u8InputMatrixCoefficients         );
    printf("\n");

    printf("u8Temp_DataFormat[0]                   :0x%02x\n", pstu_ControlParam->u8Temp_DataFormat[0]        );
    printf("u8Temp_IsFullRange[0]                   :0x%02x\n", pstu_ControlParam->u8Temp_IsFullRange[0]          );
    printf("u8Temp_HDRMode[0]                      :0x%02x\n", pstu_ControlParam->u8Temp_HDRMode[0]          );
    printf("u8Temp_GamutOrderIdx[0]                :0x%02x\n", pstu_ControlParam->u8Temp_GamutOrderIdx[0]         );
    printf("u8TempColorPriamries[0]                :0x%02x\n", pstu_ControlParam->u8TempColorPriamries[0]         );
    printf("u8TempTransferCharacterstics[0]        :0x%02x\n", pstu_ControlParam->u8TempTransferCharacterstics[0]         );
    printf("u8TempMatrixCoefficients[0]            :0x%02x\n", pstu_ControlParam->u8TempMatrixCoefficients[0]         );
    printf("\n");

    printf("u8Temp_DataFormat[1]                   :0x%02x\n", pstu_ControlParam->u8Temp_DataFormat[1]        );
    printf("u8Temp_IsFullRange[1]                   :0x%02x\n", pstu_ControlParam->u8Temp_IsFullRange[1]          );
    printf("u8Temp_HDRMode[1]                      :0x%02x\n", pstu_ControlParam->u8Temp_HDRMode[1]          );
    printf("u8Temp_GamutOrderIdx[1]                :0x%02x\n", pstu_ControlParam->u8Temp_GamutOrderIdx[1]         );
    printf("u8TempColorPriamries[1]                :0x%02x\n", pstu_ControlParam->u8TempColorPriamries[1]         );
    printf("u8TempTransferCharacterstics[1]        :0x%02x\n", pstu_ControlParam->u8TempTransferCharacterstics[1]         );
    printf("u8TempMatrixCoefficients[1]            :0x%02x\n", pstu_ControlParam->u8TempMatrixCoefficients[1]         );
    printf("\n");

    printf("u8Output_DataFormat                    :0x%02x\n", pstu_ControlParam->u8Output_DataFormat         );
    printf("u8Output_IsFullRange                   :0x%02x\n", pstu_ControlParam->u8Output_IsFullRange         );
    printf("u8Output_HDRMode                       :0x%02x\n", pstu_ControlParam->u8Output_HDRMode         );
    printf("u8Output_GamutOrderIdx                 :0x%02x\n", pstu_ControlParam->u8Output_GamutOrderIdx         );
    printf("u8OutputColorPriamries                 :0x%02x\n", pstu_ControlParam->u8OutputColorPriamries         );
    printf("u8OutputTransferCharacterstics         :0x%02x\n", pstu_ControlParam->u8OutputTransferCharacterstics         );
    printf("u8OutputMatrixCoefficients             :0x%02x\n", pstu_ControlParam->u8OutputMatrixCoefficients         );
    printf("\n");
}

void MS_Cfd_top_control_Debug(STU_CFDAPI_MAIN_CONTROL *pstApiControlParamInit)
{
    printf("\n");
//printf("u8HW_Structure                          :0x%02x\n",pstApiControlParamInit->u8HW_Structure  );
    printf("u8Input_Source                          :0x%02x\n",pstApiControlParamInit->u8Input_Source  );
    printf("u8Output_Source                         :0x%02x\n",pstApiControlParamInit->u8Output_Source );
//printf("u8Input_AnalogIdx                       :0x%02x\n",pstApiControlParamInit->u8Input_AnalogIdx );
    printf("\n");
}

//check the relation of input and temp_format parameters which control the I/0 of HDR IP
//return 0 when the current paramters is not supported by current design
//return 0 when the result is not ok, and CFD force u8Input_HDRIPMode to 0

#if 0
if ((0 == pstApiControlParamInit->u8Input_HDRIPMode) || (3 == pstApiControlParamInit->u8Input_HDRIPMode))
{
    stu_ControlParam.u8Temp_Format[1] = stu_ControlParam.u8Temp_Format[0];
    stu_ControlParam.u8Temp_DataFormat[1] = stu_ControlParam.u8Temp_DataFormat[0];
    stu_ControlParam.u8Temp_IsFullRange[1] = stu_ControlParam.u8Temp_IsFullRange[0];
    stu_ControlParam.u8Temp_HDRMode[1] = stu_ControlParam.u8Temp_HDRMode[0];
}
else
{
    stu_ControlParam.u8Temp_Format[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format;
    stu_ControlParam.u8Temp_DataFormat[1] = pstApiControlParamInit->u8Temp_DataFormat;
    stu_ControlParam.u8Temp_IsFullRange[1] = pstApiControlParamInit->u8Temp_IsFullRange;
    stu_ControlParam.u8Temp_HDRMode[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode;
}
#endif

MS_U16 MS_Cfd_Maserati_CheckHDRIPProcessIsOk(STU_CFDAPI_MAIN_CONTROL *pstApiControlParamInit, STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam)
{
    MS_U16 u16Check_status = E_CFD_MC_ERR_NOERR;

//for Maserati M+D IP
//can not do
//0.BT2020CL to NCL
//1.GM for xvYCC601,709,sYCC601
//2.wrong TMO settings

    if ((1 == pstu_ControlParam->u8Input_HDRIPMode) || (2 == pstu_ControlParam->u8Input_HDRIPMode))
    {
        //case 0:YUV BT2020CL to NCL
        if ((E_CFD_CFIO_YUV_BT2020_CL == pstu_ControlParam->u8Temp_Format[0]) && (E_CFD_CFIO_YUV_BT2020_CL != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_BT2020CLtoNCL_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printk("\nForce:Current HDR IP can not support BT2020CL to NCL,CFD will force u8Input_HDRIPMode = 0\n"));
        }

        //case 1:xvYCC601,709,sYCC601 to other gamut
        if (((E_CFD_CFIO_XVYCC_601 == pstu_ControlParam->u8Temp_Format[0]) || (E_CFD_CFIO_XVYCC_709 == pstu_ControlParam->u8Temp_Format[0])||(E_CFD_CFIO_SYCC601 == pstu_ControlParam->u8Temp_Format[0]))
            && (pstu_ControlParam->u8Temp_Format[0] != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_GMforXVYCC_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printk("\nForce:Current HDR IP is not suggested to do gamut mapping for the current input colro space"));
            HDR_DBG_HAL_CFD(printk("\nCFD will force u8Input_HDRIPMode = 0\n"));
        }

        //case 2: wrong TMO : SDR to HDR
        if ((E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[0]) && (E_CFIO_MODE_SDR != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_SDR2HDR_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printk("\nForce:Current HDR IP can not support SDR to HDR\n"));
        }

        if ((E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Temp_HDRMode[0]) && (E_CFIO_MODE_HDR1 == pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_HDRXtoHDRY_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printk("\nForce:Current HDR IP can not support HDR2 to HDR1\n"));
        }

    }

    if (E_CFD_MC_ERR_NOERR != u16Check_status)
    {
        HDR_DBG_HAL_CFD(printk("\nError:HDR IP cannot support this process, check IO of HDR IP\n"));
        HDR_DBG_HAL_CFD(printk("Error code = %04x !!!, error is in [ %s  , %d]\n",u16Check_status, __FUNCTION__,__LINE__));
    }

    return u16Check_status;
}

MS_U16 MS_Cfd_Maserati_CheckSDRIPProcessIsOk(STU_CFDAPI_MAIN_CONTROL *pstApiControlParamInit, STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam)
{
    MS_U16 u16Check_status = E_CFD_MC_ERR_NOERR;

//for Maserati SDR IP
//can not do
//2.wrong TMO settings

//if (1 == pstu_ControlParam->u8Input_SDRIPMode)
    {

        if ((( E_CFD_MC_FORMAT_RGB != pstu_ControlParam->u8Output_DataFormat) && (E_CFD_MS_SC0_MAIN == pstu_ControlParam->u8HW_MainSub_Mode)) ||
            (E_CFD_MS_SC1_SUB == pstu_ControlParam->u8HW_MainSub_Mode))
        {
            if ((E_CFD_MC_FORMAT_RGB != pstu_ControlParam->u8Temp_DataFormat[1]) && (E_CFD_CFIO_YUV_BT2020_CL == pstu_ControlParam->u8Temp_Format[1]) && (E_CFD_CFIO_YUV_BT2020_CL != pstu_ControlParam->u8Output_Format))
            {

                HDR_DBG_HAL_CFD(printk("\n Error: Current SDR IP group can not support YUV out and Do BT2020CL handle at the same time\n"));
                u16Check_status = E_CFD_MC_ERR_HW_IPS_BT2020CLtoNCL_NOTSUPPORTED;

                HDR_DBG_HAL_CFD(printk("\n Force: output format is the same as input format of SDR IP\n"));
                pstu_ControlParam->u8DoSDRIP_ForceNOBT2020CL_Flag = 1;

                pstu_ControlParam->u8Output_Format = E_CFD_CFIO_YUV_BT2020_CL;
                pstu_ControlParam->u8OutputColorPriamries = pstu_ControlParam->u8TempColorPriamries[1];
                pstu_ControlParam->u8OutputTransferCharacterstics = pstu_ControlParam->u8TempTransferCharacterstics[1];
                pstu_ControlParam->u8OutputMatrixCoefficients = pstu_ControlParam->u8TempMatrixCoefficients[1];
                pstu_ControlParam->u8Output_GamutOrderIdx = pstu_ControlParam->u8Temp_GamutOrderIdx[1];
            }

            if (pstu_ControlParam->u8Output_GamutOrderIdx != pstu_ControlParam->u8Temp_GamutOrderIdx[1])
            {

                HDR_DBG_HAL_CFD(printk("\n Force:Current SDR IP can not support YUV out and Do GM at the same time\n"));
                u16Check_status = E_CFD_MC_ERR_HW_IPS_GM_NOTSUPPORTED;

                HDR_DBG_HAL_CFD(printk("\n Force: output format is the same as input format of SDR IP\n"));
                pstu_ControlParam->u8DoSDRIP_ForceNOGM_Flag = 1;

                pstu_ControlParam->u8Output_Format = pstu_ControlParam->u8Temp_Format[1];
                pstu_ControlParam->u8OutputColorPriamries = pstu_ControlParam->u8TempColorPriamries[1];
                pstu_ControlParam->u8OutputTransferCharacterstics = pstu_ControlParam->u8TempTransferCharacterstics[1];
                pstu_ControlParam->u8OutputMatrixCoefficients = pstu_ControlParam->u8TempMatrixCoefficients[1];
                pstu_ControlParam->u8Output_GamutOrderIdx = pstu_ControlParam->u8Temp_GamutOrderIdx[1];
            }

            if ((pstu_ControlParam->u8Temp_HDRMode[1] != pstApiControlParamInit->u8Output_HDRMode) && ( E_CFIO_MODE_SDR == pstApiControlParamInit->u8Output_HDRMode))
            {

                HDR_DBG_HAL_CFD(printk("\n Force:Current SDR IP can not support YUV out and Do TMO\n"));
                u16Check_status = E_CFD_MC_ERR_HW_IPS_TMO_NOTSUPPORTED;

                HDR_DBG_HAL_CFD(printk("\n Force: output format HDR mode is the same as input HDR mode of SDR IP\n"));
                pstu_ControlParam->u8DoSDRIP_ForceNOTMO_Flag = 1;

                pstApiControlParamInit->u8Output_HDRMode = pstu_ControlParam->u8Temp_HDRMode[1];
            }
        }

        //case 2: wrong TMO : SDR to HDR
        if ((E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[1]) && (E_CFIO_MODE_SDR != pstApiControlParamInit->u8Output_HDRMode))
        {

            HDR_DBG_HAL_CFD(printk("\nForce:Current SDR IP can not support SDR to HDR\n"));
            u16Check_status = E_CFD_MC_ERR_HW_IPS_SDR2HDR_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printk("\n Force: output format HDR mode is the same as input HDR mode of SDR IP\n"));
            pstu_ControlParam->u8DoSDRIP_ForceNOTMO_Flag = 1;

            pstApiControlParamInit->u8Output_HDRMode = pstu_ControlParam->u8Temp_HDRMode[1];
        }

        if ((E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Temp_HDRMode[1]) && (E_CFIO_MODE_HDR1 == pstApiControlParamInit->u8Output_HDRMode))
        {

            HDR_DBG_HAL_CFD(printk("\nForce:Current SDR IP can not support HDR2 to HDR1\n"));
            u16Check_status = E_CFD_MC_ERR_HW_IPS_HDRXtoHDRY_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printk("\n Force: output format HDR mode is the same as input HDR mode of SDR IP\n"));
            pstu_ControlParam->u8DoSDRIP_ForceNOTMO_Flag = 1;

            pstApiControlParamInit->u8Output_HDRMode = pstu_ControlParam->u8Temp_HDRMode[1];
        }

    }

    if (E_CFD_MC_ERR_NOERR != u16Check_status)
    {
        HDR_DBG_HAL_CFD(printk("\nError:SDR IP cannot support this process, check IO of SDR IP\n"));
        HDR_DBG_HAL_CFD(printk("Error code = %04x !!!, error is in [ %s  , %d]\n",u16Check_status, __FUNCTION__,__LINE__));
    }

    return u16Check_status;
}

void MS_Cfd_WriteBack_Control(STU_CFDAPI_TOP_CONTROL *pstu_Cfd_api_top,STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam)
{
    MS_U8 u8CFDversion = 0x02;
    pstu_Cfd_api_top->pstu_Main_Control->u8Input_Format = pstu_ControlParam->u8Input_Format;
    pstu_Cfd_api_top->pstu_Main_Control->u8Input_DataFormat = pstu_ControlParam->u8Input_DataFormat;
    pstu_Cfd_api_top->pstu_Main_Control->u8Input_IsFullRange = pstu_ControlParam->u8Input_IsFullRange;
    pstu_Cfd_api_top->pstu_Main_Control->u8Input_HDRMode = pstu_ControlParam->u8Input_HDRMode;
    pstu_Cfd_api_top->pstu_Main_Control->stu_Middle_Format[0].u8Mid_Format = pstu_ControlParam->u8Temp_Format[1];
    pstu_Cfd_api_top->pstu_Main_Control->stu_Middle_Format[0].u8Mid_DataFormat = pstu_ControlParam->u8Temp_DataFormat[1];
    pstu_Cfd_api_top->pstu_Main_Control->stu_Middle_Format[0].u8Mid_IsFullRange = pstu_ControlParam->u8Temp_IsFullRange[1];
    pstu_Cfd_api_top->pstu_Main_Control->stu_Middle_Format[0].u8Mid_HDRMode = pstu_ControlParam->u8Temp_HDRMode[1];
    if(MApi_GFLIP_XC_R2BYTE(REG_SC_BK30_01_L) == 0x001C)
    {
        MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_02_L, ((pstu_ControlParam->u8Input_Format<<8)+pstu_ControlParam->u8Input_DataFormat));
        MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_03_L, ((pstu_ControlParam->u8Input_SDRIPMode<<8)+pstu_ControlParam->u8Input_HDRMode));
        MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_04_L, ((pstu_ControlParam->u8InputColorPriamries<<8)+pstu_ControlParam->u8InputTransferCharacterstics));
        MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_06_L, ((pstu_ControlParam->u8InputMatrixCoefficients<<8)+pstu_ControlParam->u8Input_IsFullRange));
        MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_08_L, ((pstu_ControlParam->u8Input_IsRGBBypass<<8)+u8CFDversion));

    }
}

MS_U8 MS_Cfd_InputHDMI_SetColorMetry(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam,
    STU_CFDAPI_HDMI_INFOFRAME_PARSER *pstu_api_HDMI_InfoFrame_Param,
    STU_CFDAPI_MAIN_CONTROL *pstu_Main_Control
)
{
    MS_U8 u8Check_Status = 0;
    MS_U8 u8idx = 0;
//MS_U8 u8temp = 0;

//set

//stu_ControlParam.u8InputFormat
//stu_ControlParam.u8Input_DataFormat
//stu_ControlParam.u8InputIsFullRange
//stu_ControlParam.u8InputColorPriamries
//stu_ControlParam.u8InputTransferCharacterstics
//stu_ControlParam.u8InputMatrixCoefficients

//pstu_ControlParam->u8Input_DataFormat = pstu_Main_Control->u8Input_DataFormat;
//pstu_ControlParam->u8Input_Format  = pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace;

//force to a specific color for reserved case
    if ((E_CFD_CFIO_RGB_NOTSPECIFIED == pstu_ControlParam->u8Input_Format)||
        (E_CFD_CFIO_YUV_NOTSPECIFIED == pstu_ControlParam->u8Input_Format)||
        (E_CFD_CFIO_RESERVED_START <= pstu_ControlParam->u8Input_Format))
    {
        u8Check_Status = 1;

        if (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Input_DataFormat) //RGB
        {
            pstu_ControlParam->u8Input_Format = E_CFD_CFIO_SRGB;
        }
        else
        {
            pstu_ControlParam->u8Input_Format = E_CFD_CFIO_YUV_BT709;
        }
    }

//assign u8InputIsFullRange
//u8HDMISource_Support_Format = {Q1 Q0},{YQ1 YQ0},{Y2 Y1 Y0}
#if 0
    if (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Input_DataFormat) //RGB
    {
        u8temp = (pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format&0x60)>>5;

        if (0 == u8temp) //default depends on Video format
        {
            if (1 == pstu_Main_Control->u8Input_Format_HDMI_CE_Flag) //limited
            {
                pstu_ControlParam->u8Input_IsFullRange = 0;
            }
            else
            {
                pstu_ControlParam->u8Input_IsFullRange = 1;
            }
        }
        else if (1 == u8temp) //limited range
        {
            pstu_ControlParam->u8Input_IsFullRange = 0;
        }
        else if (2 == u8temp) //full range
        {
            pstu_ControlParam->u8Input_IsFullRange = 1;
        }
        else //reversed
        {
            pstu_ControlParam->u8Input_IsFullRange = 1;
            u8Check_Status = 1;
        }
    }
    else
    {
        u8temp = (pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format&0x18)>>3;

        if (0 == u8temp)
        {
            pstu_ControlParam->u8Input_IsFullRange = 0;
        }
        else if (1 == u8temp)
        {
            pstu_ControlParam->u8Input_IsFullRange = 1;
        }
        else
        {
            pstu_ControlParam->u8Input_IsFullRange = 0;
            u8Check_Status = 1;
        }

    }
#endif

//
//assign
//stu_ControlParam.u8InputColorPriamries
//stu_ControlParam.u8InputTransferCharacterstics
//stu_ControlParam.u8InputMatrixCoefficients

    for (u8idx =0; u8idx<=9 ; u8idx++)
    {
        if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3] == pstu_ControlParam->u8Input_Format)||
            (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4] == pstu_ControlParam->u8Input_Format))
        {
            break;
        }
    }

    if (u8idx > 8)
    {
        u8idx = 8;
    }

    pstu_ControlParam->u8InputColorPriamries = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0];
    pstu_ControlParam->u8InputTransferCharacterstics = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][1];
    pstu_ControlParam->u8InputMatrixCoefficients = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][2];

#if 0
    if (E_CFD_VALID == pstu_api_HDMI_InfoFrame_Param->u8HDMISource_HDR_InfoFrame_Valid)
    {
        if (E_CFD_HDMI_EOTF_SMPTE2084 == pstu_api_HDMI_InfoFrame_Param->u8HDMISource_EOTF)
        {
            pstu_ControlParam->u8InputTransferCharacterstics = E_CFD_CFIO_TR_SMPTE2084;
        }
    }
#endif

#if 0
    if (E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Input_HDRMode)
    {
        pstu_ControlParam->u8InputTransferCharacterstics = E_CFD_CFIO_TR_SMPTE2084;
    }
    else if (E_CFIO_MODE_HDR1 == pstu_ControlParam->u8Input_HDRMode)
    {
        pstu_ControlParam->u8InputTransferCharacterstics = E_CFD_CFIO_TR_UNSPECIFIED;
    }
#else
    pstu_ControlParam->u8InputTransferCharacterstics = MS_Cfd_GetTR_FromHDRMode(pstu_ControlParam->u8Input_HDRMode,pstu_ControlParam->u8InputTransferCharacterstics);
#endif

//0:current HDMI input is one case of HDMI colormetry
//1:current HDMI input is not one case of HDMI colormetry,
    return u8Check_Status;
}


MS_U8 MS_Cfd_InputHDMI_SetColorMetry_lite(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam,
    STU_CFDAPI_MAIN_CONTROL *pstu_Main_Control
)
{
    MS_U8 u8Check_Status = 0;
    MS_U8 u8idx = 0;
//MS_U8 u8temp = 0;

//set

//stu_ControlParam.u8InputFormat
//stu_ControlParam.u8Input_DataFormat
//stu_ControlParam.u8InputIsFullRange
//stu_ControlParam.u8InputColorPriamries
//stu_ControlParam.u8InputTransferCharacterstics
//stu_ControlParam.u8InputMatrixCoefficients

//pstu_ControlParam->u8Input_DataFormat = pstu_Main_Control->u8Input_DataFormat;
//pstu_ControlParam->u8Input_Format  = pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace;

//force to a specific color for reserved case
    if ((E_CFD_CFIO_RGB_NOTSPECIFIED == pstu_ControlParam->u8Input_Format)||
        (E_CFD_CFIO_YUV_NOTSPECIFIED == pstu_ControlParam->u8Input_Format)||
        (E_CFD_CFIO_RESERVED_START <= pstu_ControlParam->u8Input_Format))
    {
        u8Check_Status = 1;

        if (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Input_DataFormat) //RGB
        {
            pstu_ControlParam->u8Input_Format = E_CFD_CFIO_SRGB;
        }
        else
        {
            pstu_ControlParam->u8Input_Format = E_CFD_CFIO_YUV_BT709;
        }

        HDR_DBG_HAL_CFD(printk("warning forceto709 happens in [ %s  , %d]\n", __FUNCTION__,__LINE__));
    }

//assign u8InputIsFullRange
//u8HDMISource_Support_Format = {Q1 Q0},{YQ1 YQ0},{Y2 Y1 Y0}

//
//assign
//stu_ControlParam.u8InputColorPriamries
//stu_ControlParam.u8InputTransferCharacterstics
//stu_ControlParam.u8InputMatrixCoefficients

    for (u8idx =0; u8idx<=9 ; u8idx++)
    {
        if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3] == pstu_ControlParam->u8Input_Format)||
            (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4] == pstu_ControlParam->u8Input_Format))
        {
            break;
        }
    }

    if (u8idx > 8)
    {
        u8idx = 8;
    }

    pstu_ControlParam->u8InputColorPriamries = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0];
    pstu_ControlParam->u8InputTransferCharacterstics = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][1];
    pstu_ControlParam->u8InputMatrixCoefficients = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][2];

#if 0
    if (E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Input_HDRMode)
    {
        pstu_ControlParam->u8InputTransferCharacterstics = E_CFD_CFIO_TR_SMPTE2084;
    }
    else if (E_CFIO_MODE_HDR1 == pstu_ControlParam->u8Input_HDRMode)
    {
        pstu_ControlParam->u8InputTransferCharacterstics = E_CFD_CFIO_TR_UNSPECIFIED;
    }
#else
    pstu_ControlParam->u8InputTransferCharacterstics = MS_Cfd_GetTR_FromHDRMode(pstu_ControlParam->u8Input_HDRMode,pstu_ControlParam->u8InputTransferCharacterstics);
#endif

//0:current HDMI input is one case of HDMI colormetry
//1:current HDMI input is not one case of HDMI colormetry,
    return u8Check_Status;
}

MS_U8 MS_Cfd_InputHDMI_SetColorMetry_lite_out(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam,
    STU_CFDAPI_MAIN_CONTROL *pstu_Main_Control
)
{
    MS_U8 u8Check_Status = 0;
    MS_U8 u8idx = 0;

//force to a specific color for reserved case
    if ((E_CFD_CFIO_RGB_NOTSPECIFIED == pstu_ControlParam->u8Output_Format)||
        (E_CFD_CFIO_YUV_NOTSPECIFIED == pstu_ControlParam->u8Output_Format)||
        (E_CFD_CFIO_RESERVED_START <= pstu_ControlParam->u8Output_Format))
    {
        u8Check_Status = 1;

        if (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_DataFormat) //RGB
        {
            pstu_ControlParam->u8Output_Format = E_CFD_CFIO_SRGB;
        }
        else
        {
            pstu_ControlParam->u8Output_Format = E_CFD_CFIO_YUV_BT709;
        }

        HDR_DBG_HAL_CFD(printk("warning forceto709 happens in [ %s  , %d]\n", __FUNCTION__,__LINE__));
    }

//assign
//stu_ControlParam.u8InputColorPriamries
//stu_ControlParam.u8InputTransferCharacterstics
//stu_ControlParam.u8InputMatrixCoefficients

    for (u8idx =0; u8idx<=9 ; u8idx++)
    {
        if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3] == pstu_ControlParam->u8Output_Format)||
            (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4] == pstu_ControlParam->u8Output_Format))
        {
            break;
        }
    }

    if (u8idx > 8)
    {
        u8idx = 8;
    }

    pstu_ControlParam->u8OutputColorPriamries = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0];
    pstu_ControlParam->u8OutputTransferCharacterstics = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][1];
    pstu_ControlParam->u8OutputMatrixCoefficients = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][2];

#if 0
    if (E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Output_HDRMode)
    {
        pstu_ControlParam->u8OutputTransferCharacterstics = E_CFD_CFIO_TR_SMPTE2084;
    }
    else if (E_CFIO_MODE_HDR1 == pstu_ControlParam->u8Output_HDRMode)
    {
        pstu_ControlParam->u8OutputTransferCharacterstics = E_CFD_CFIO_TR_UNSPECIFIED;
    }
#else
    pstu_ControlParam->u8OutputTransferCharacterstics = MS_Cfd_GetTR_FromHDRMode(pstu_ControlParam->u8Output_HDRMode, pstu_ControlParam->u8OutputTransferCharacterstics);
#endif

//0:current HDMI input is one case of HDMI colormetry
//1:current HDMI input is not one case of HDMI colormetry,
    return u8Check_Status;
}

MS_U16 MS_Cfd_Maserati_Dolby_Control(STU_CFDAPI_DOLBY_CONTROL *pstu_Dolby_Param)
{
// todo
    return 0;
}

MS_U16 MS_Cfd_SetGM_TMO_Flags(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam)
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
            if ((pstu_ControlParam->u8Temp_GamutOrderIdx[0] == 1) && (pstu_ControlParam->u8Temp_GamutOrderIdx[1] == 6))
            {
                pstu_ControlParam->u8DoGamutMappingInHDRIP_Flag = 0;
            }
            else
            {
                                pstu_ControlParam->u8DoGamutMappingInHDRIP_Flag = 1;
                        }
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

        //force image enter HDRIP when input_source = HDMI
        if (E_CFD_INPUT_SOURCE_HDMI == pstu_ControlParam->u8Input_Source)
        {
            pstu_ControlParam->u8DoForceEnterHDRIP_Flag = 1;
        }
        else
        {
            pstu_ControlParam->u8DoForceEnterHDRIP_Flag = 0;
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
        (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_DataFormat))
    {
        pstu_ControlParam->u8DoBT2020CLP_Flag = 1;
    }
    else
    {
        pstu_ControlParam->u8DoBT2020CLP_Flag = 0;
    }

    if ((pstu_ControlParam->u8Output_GamutOrderIdx != pstu_ControlParam->u8Temp_GamutOrderIdx[1]) && (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_DataFormat))
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
    else if ((E_CFIO_MODE_SDR != pstu_ControlParam->u8Temp_HDRMode[1] ) && (E_CFIO_MODE_SDR == pstu_ControlParam->u8Output_HDRMode ) && (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_DataFormat))
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

#if NowHW == Maserati
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
        //u8IP2_CSC_Ratio1   =  pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Manhattan_SDRIP_Param.u8IP2_CSC_Ratio1;
        //u8VOP_3x3_Ratio1   =  pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Manhattan_SDRIP_Param.u8VOP_3x3_Ratio1;
        //u8Degamma_Ratio1   =  pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Manhattan_SDRIP_Param.u8Degamma_Ratio1;
        //u16Degamma_Ratio2  = pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Manhattan_SDRIP_Param.u16Degamma_Ratio2;
        //u163x3_Ratio2      =     pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Manhattan_SDRIP_Param.u163x3_Ratio2;
    }

    msDlc_FunctionExit();
#endif

    //printf("u8Process_Mode                        :%d REG_SC_Ali_BK30_01_L \n", pstApiControlParamInit->u8Process_Mode                     );
    printf("u8Input_Source                        :%d REG_SC_Ali_BK30_01_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_Source                     );
    //printf("u8Input_AnalogIdx                     :%d REG_SC_Ali_BK30_02_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_AnalogIdx                  );
    printf("u8Input_Format                        :%d REG_SC_Ali_BK30_02_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_Format                     );
    printf("u8Input_DataFormat                    :%d REG_SC_Ali_BK30_03_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_DataFormat                 );
    printf("u8Input_IsFullRange                   :%d REG_SC_Ali_BK30_03_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_IsFullRange                );
    printf("u8Input_HDRMode                       :%d REG_SC_Ali_BK30_04_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_HDRMode                    );
    printf("u8Input_IsRGBBypass                   :%d REG_SC_Ali_BK30_04_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_IsRGBBypass                );
    printf("u8Output_Source                       :%d REG_SC_Ali_BK30_05_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_Source                    );
    printf("u8Output_Format                       :%d REG_SC_Ali_BK30_05_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_Format                    );
    printf("u8Output_DataFormat                   :%d REG_SC_Ali_BK30_06_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_DataFormat                );
    printf("u8Output_IsFullRange                  :%d REG_SC_Ali_BK30_06_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_IsFullRange               );
    printf("u8Output_HDRMode                      :%d REG_SC_Ali_BK30_07_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_HDRMode                   );
    printf("u8HDR_enable_Mode            :%d REG_SC_Ali_BK30_08_L Bit[0]  \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_enable_Mode             );
    printf("u8Input_SDRIPMode            :%d REG_SC_Ali_BK30_08_L Bit[5:4]\n",pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_SDRIPMode                     );
    printf("u8Input_HDRIPMode            :%d REG_SC_Ali_BK30_08_L Bit[7:6]\n",pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_HDRIPMode                     );
    //printf("u8HDR_InputCSC_Manual_Vars_en:%d REG_SC_Ali_BK30_08_H         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_Manual_Vars_en );
    //printf("u8HDR_InputCSC_MC            :%d REG_SC_Ali_BK30_09_H         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_InputCSC_MC             );
    printf("u8HDR_DeGamma_Manual_Vars_en :%d REG_SC_Ali_BK30_08_H         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_DeGamma_Manual_Vars_en  );
    printf("u8HDR_Degamma_TR             :%d REG_SC_Ali_BK30_09_H         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_Degamma_TR              );
    printf("u8HDR_Gamma_Manual_Vars_en   :%d REG_SC_Ali_BK30_0B_H         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_Gamma_Manual_Vars_en    );
    printf("u8HDR_Gamma_TR               :%d REG_SC_Ali_BK30_0B_L         \n",pstu_CfdAPI_Top_Param->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param.u8HDR_Gamma_TR                );
}
#endif

MS_U16 Mapi_Cfd_inter_Main_Control_PreConstraints(STU_CFDAPI_MAIN_CONTROL *pstControlParamInit )
{

    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
    MS_U16 u16Temp;
//RGB
    if (E_CFD_MC_FORMAT_RGB == pstControlParamInit->u8Input_DataFormat)
    {
        if (1 == pstControlParamInit->u8Input_IsRGBBypass)
        {
            if (E_CFIO_MODE_SDR == pstControlParamInit->u8Input_HDRMode)
            {
                if ( E_CFD_INPUT_SOURCE_HDMI != pstControlParamInit->u8Input_Source)
                {
                    pstControlParamInit->u8Input_HDRIPMode = 0;
                }
                else //modify for Dolby HDMI parsing
                {
                    pstControlParamInit->u8Input_HDRIPMode = 1;
                }

                pstControlParamInit->u8Input_SDRIPMode = 1;
                pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_RGB;
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;
#if HW_peaking_bug
                if( HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }
#else
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = pstControlParamInit->u8Input_IsFullRange;
#endif
            }
            //else if (E_CFIO_MODE_HDR2 == pstControlParamInit->u8Input_HDRMode)
            else if (MS_Cfd_CheckOpenHDR(pstControlParamInit->u8Input_HDRMode))
            {
                pstControlParamInit->u8Input_HDRIPMode = 1;
                pstControlParamInit->u8Input_SDRIPMode = 1;
                pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_RGB;
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;
#if HW_peaking_bug
                if( HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }
#else
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = pstControlParamInit->u8Input_IsFullRange;
#endif
            }
        }
        else
        {
            if (E_CFIO_MODE_SDR == pstControlParamInit->u8Input_HDRMode)
            {
                if ( E_CFD_INPUT_SOURCE_HDMI != pstControlParamInit->u8Input_Source)
                {
                    pstControlParamInit->u8Input_HDRIPMode = 0;
                }
                else //modify for Dolby HDMI parsing
                {
                    pstControlParamInit->u8Input_HDRIPMode = 1;
                }
                pstControlParamInit->u8Input_SDRIPMode = 1;
                //pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                pstControlParamInit->stu_Middle_Format->u8Mid_Format = MS_Cfd_SetFormat_OnlyR2Y(pstControlParamInit->u8Input_Format);
                //format RGB to YUV
                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;

                if( HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }
            }
            //else if (E_CFIO_MODE_HDR2 == pstControlParamInit->u8Input_HDRMode)
            else if (MS_Cfd_CheckOpenHDR(pstControlParamInit->u8Input_HDRMode))
            {
                pstControlParamInit->u8Input_HDRIPMode = 1;
                pstControlParamInit->u8Input_SDRIPMode = 1;
                //pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                pstControlParamInit->stu_Middle_Format->u8Mid_Format = MS_Cfd_SetFormat_OnlyR2Y(pstControlParamInit->u8Input_Format);
                //format RGB to YUV
                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;

                if( HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }
            }
        }
    }
    else // YUV
    {

        if (E_CFIO_MODE_SDR == pstControlParamInit->u8Input_HDRMode)
        {
            if ( E_CFD_INPUT_SOURCE_HDMI != pstControlParamInit->u8Input_Source)
            {
                pstControlParamInit->u8Input_HDRIPMode = 0;
            }
            else //modify for Dolby HDMI parsing
            {
                pstControlParamInit->u8Input_HDRIPMode = 1;
            }
            pstControlParamInit->u8Input_SDRIPMode = 1;
            pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
            pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;
            pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;

            if( HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
            {
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
            }
            else
            {
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
            }


#if !HW_peaking_bug
            if(1 == pstControlParamInit->u8Input_IsRGBBypass)
            {
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = pstControlParamInit->u8Input_IsFullRange;
            }
#endif
        }
        //else if (E_CFIO_MODE_HDR2 == pstControlParamInit->u8Input_HDRMode)
        else if (MS_Cfd_CheckOpenHDR(pstControlParamInit->u8Input_HDRMode))
        {
            pstControlParamInit->u8Input_HDRIPMode = 1;
            pstControlParamInit->u8Input_SDRIPMode = 1;
            pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
            //pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;
            pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;
            pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;


            if( HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
            {
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
            }
            else
            {
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
            }
#if !HW_peaking_bug
            if(1 == pstControlParamInit->u8Input_IsRGBBypass)
            {
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = pstControlParamInit->u8Input_IsFullRange;
            }
#endif
        }

    }
    if( (E_CFD_CFIO_XVYCC_601 == pstControlParamInit->u8Input_Format) ||
        (E_CFD_CFIO_XVYCC_709 == pstControlParamInit->u8Input_Format) ||
        (E_CFD_CFIO_SYCC601 == pstControlParamInit->u8Input_Format)
      )
    {
        pstControlParamInit->u8Input_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
        pstControlParamInit->u8Input_HDRIPMode = 1;
    }
    return 1;
}

MS_U8 MS_Cfd_SearchAndMappingHDMIcase_OnlyRGB(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam,
    STU_CFDAPI_MM_PARSER *pstu_MMParam,
    STU_CFDAPI_MAIN_CONTROL *pstu_Main_Control,
    MS_U8 *u8candidateIdx)
{
    MS_U8 u8Check_Status = 1;
    MS_U8 u8idx = 0;
    MS_U8 u8Okflag = 0;

    for (u8idx = 0; u8idx<= 8; u8idx++)
    {
        //three features are the same
        //not HDR case
        if((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0] == pstu_ControlParam->u8InputColorPriamries)&&
           (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][2] == pstu_ControlParam->u8InputMatrixCoefficients)&&
           (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][1] == pstu_ControlParam->u8InputTransferCharacterstics))
        {
            u8Okflag = 1;
        }
        //HDR case
        else if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0] == pstu_ControlParam->u8InputColorPriamries)&&
                 (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][2] == pstu_ControlParam->u8InputMatrixCoefficients)&&
                 (MS_Cfd_CheckOpenHDR(pstu_ControlParam->u8Input_HDRMode)))
                 //((E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Input_HDRMode)))
        {
            u8Okflag = 1;
        }
        //not HDR case, RGB
        else if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0] == pstu_ControlParam->u8InputColorPriamries)&&
                 (E_CFD_CFIO_MC_IDENTITY == pstu_ControlParam->u8InputMatrixCoefficients)&&
                 (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][1] == pstu_ControlParam->u8InputTransferCharacterstics))
        {
            u8Okflag = 2;
        }
        //HDR case, RGB
        else if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0] == pstu_ControlParam->u8InputColorPriamries)&&
                 (E_CFD_CFIO_MC_IDENTITY == pstu_ControlParam->u8InputMatrixCoefficients)&&
                 (MS_Cfd_CheckOpenHDR(pstu_ControlParam->u8Input_HDRMode)))
                 //(E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Input_HDRMode))
        {
            u8Okflag = 2;
        }
        else
        {
            u8Okflag = 0;
        }

        if (2 == u8Okflag)
        {
            if (pstu_ControlParam->u8Input_DataFormat == E_CFD_MC_FORMAT_RGB) //RGB
            {
                if ((0 <= u8idx)&&(6 >= u8idx))
                {
                    pstu_ControlParam->u8InputMatrixCoefficients = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][2];
                    u8Check_Status = 0;
                    *u8candidateIdx = u8idx;
                    break;
                }
            }

        }
        else
        {
            u8Check_Status = 1;
        }
    }
    return u8Check_Status;
}
MS_U16 MS_Cfd_SourceMax_Clip_MM(STU_CFDAPI_MM_PARSER *pstu_MM_Param)
{
    MS_U16 u16ClipMax;

    //both valid
    if((E_CFD_VALID == pstu_MM_Param->u8Mastering_Display_Infor_Valid) && (E_CFD_VALID == pstu_MM_Param->u8MM_HDR_ContentLightMetaData_Valid))
    {
        if(pstu_MM_Param->u16Max_content_light_level < DePQClampMin)
        {
            u16ClipMax = pstu_MM_Param->u32Master_Panel_Max_Luminance/10000;
        }
        else
        {
            u16ClipMax = min(pstu_MM_Param->u32Master_Panel_Max_Luminance/10000,pstu_MM_Param->u16Max_content_light_level);
        }
    }
    else if((E_CFD_VALID == pstu_MM_Param->u8Mastering_Display_Infor_Valid) && (E_CFD_VALID != pstu_MM_Param->u8MM_HDR_ContentLightMetaData_Valid))
    {
        u16ClipMax = pstu_MM_Param->u32Master_Panel_Max_Luminance/10000;
    }
    else if((E_CFD_VALID != pstu_MM_Param->u8Mastering_Display_Infor_Valid) && (E_CFD_VALID == pstu_MM_Param->u8MM_HDR_ContentLightMetaData_Valid))
    {
        u16ClipMax = pstu_MM_Param->u16Max_content_light_level;
    }
    else
    {
        u16ClipMax = DePQClampMin;
    }
#if DePQreferMetadata_EN
    u16ClipMax = max(u16ClipMax,DePQClampMin);
#else
    u16ClipMax = DePQClampMin;
#endif

    return u16ClipMax;
}

MS_U16 MS_Cfd_SourceMax_Clip_HDMI(STU_CFDAPI_HDMI_INFOFRAME_PARSER *pstu_api_HDMI_InfoFrame_Param)
{
    MS_U16 u16ClipMax;

    //HDR infoFrame valid
    if(E_CFD_VALID == pstu_api_HDMI_InfoFrame_Param->u8Mastering_Display_Infor_Valid && 0 ==  pstu_api_HDMI_InfoFrame_Param->u8HDMISource_SMD_ID)
    {
        if(pstu_api_HDMI_InfoFrame_Param->u16Max_Content_Light_Level < DePQClampMin)
        {
            u16ClipMax = pstu_api_HDMI_InfoFrame_Param->u16Master_Panel_Max_Luminance;
        }
        else
        {
            u16ClipMax = min(pstu_api_HDMI_InfoFrame_Param->u16Master_Panel_Max_Luminance,pstu_api_HDMI_InfoFrame_Param->u16Max_Content_Light_Level);
        }
    }
    else if (E_CFD_VALID == pstu_api_HDMI_InfoFrame_Param->u8Mastering_Display_Infor_Valid && 0 !=  pstu_api_HDMI_InfoFrame_Param->u8HDMISource_SMD_ID)
    {
        u16ClipMax = pstu_api_HDMI_InfoFrame_Param->u16Master_Panel_Max_Luminance;
    }
    else if (E_CFD_VALID != pstu_api_HDMI_InfoFrame_Param->u8Mastering_Display_Infor_Valid && 0 ==  pstu_api_HDMI_InfoFrame_Param->u8HDMISource_SMD_ID)
    {
        u16ClipMax = pstu_api_HDMI_InfoFrame_Param->u16Max_Content_Light_Level;
    }
    else
    {
        u16ClipMax = DePQClampMin;
    }
#if DePQreferMetadata_EN
    u16ClipMax = max(u16ClipMax,DePQClampMin);
#else
    u16ClipMax = DePQClampMin;
#endif

    return u16ClipMax;
}
MS_U16 Mapi_Cfd_ColorFormatDriver_Control(STU_CFDAPI_TOP_CONTROL *pstu_Cfd_api_top)
{
//E_CFD_MC_ERR
    MS_U8 u8_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8ERR_Happens_Flag = 0;
    MS_U8 u8Match_Flag = 0;
    MS_U8 u8Check_status2 = 0;

    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status_tmp = E_CFD_MC_ERR_NOERR;

    MS_U8 u8idx = 0;

    MS_U16 U16Temp0 = 0;
    MS_U16 U16Temp1 = 0;
    MS_U8 u8MainSubMode = 0;

//interior stucture
    STU_CFD_MS_ALG_COLOR_FORMAT stu_ControlParam;
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam;
    pstu_ControlParam = &stu_ControlParam;
    memset(&stu_ControlParam, 0, sizeof(STU_CFD_MS_ALG_COLOR_FORMAT));
    MS_IN_Cfd_TMO_Control_Param_Init(&(stu_ControlParam.stu_CFD_TMO_Param));
    pstu_ControlParam->u8HW_MainSub_Mode = pstu_Cfd_api_top->pstu_HW_IP_Param->u8HW_MainSub_Mode;
    pstu_ControlParam->u16DolbySupportStatus = pstu_Cfd_api_top->pstu_Main_Control->u16DolbySupportStatus;
    pstu_ControlParam->u8DoPathFullRange_Flag = HW_PIPE_RANGE;
    pstu_ControlParam->u8Input_Source = pstu_Cfd_api_top->pstu_Main_Control->u8Input_Source;


//interface to TMO/HDR/SDR
    STU_CFD_MS_ALG_INTERFACE_DLC stu_DLC_Input;
    STU_CFD_MS_ALG_INTERFACE_TMO stu_TMO_Input;
    STU_CFD_MS_ALG_INTERFACE_HDRIP stu_HDRIP_Input;
    STU_CFD_MS_ALG_INTERFACE_SDRIP stu_SDRIP_Input;
    memset(&stu_DLC_Input, 0, sizeof(STU_CFD_MS_ALG_INTERFACE_DLC));
    memset(&stu_TMO_Input, 0, sizeof(STU_CFD_MS_ALG_INTERFACE_TMO));
    memset(&stu_HDRIP_Input, 0, sizeof(STU_CFD_MS_ALG_INTERFACE_HDRIP));
    memset(&stu_SDRIP_Input, 0, sizeof(STU_CFD_MS_ALG_INTERFACE_SDRIP));

//memcpy(&stu_HDRIP_Input.stu_Kano_HDRIP_Param,pstu_Cfd_api_top->pstu_Kano_HDRIP_Param,sizeof(STU_CFDAPI_Kano_HDRIP));
//memcpy(&stu_SDRIP_Input.stu_Kano_SDRIP_Param,pstu_Cfd_api_top->pstu_Kano_SDRIP_Param,sizeof(STU_CFDAPI_Kano_SDRIP));
//memcpy(&stu_TMO_Input.stu_Kano_TMOIP_Param,pstu_Cfd_api_top->pstu_Kano_TMO_Param,sizeof(STU_CFDAPI_Kano_TMOIP));

//memcpy(&stu_HDRIP_Input.stu_Manhattan_HDRIP_Param,pstu_Cfd_api_top->pstu_Manhattan_HDRIP_Param,sizeof(STU_CFDAPI_Manhattan_HDRIP));
//memcpy(&stu_SDRIP_Input.stu_Manhattan_SDRIP_Param,pstu_Cfd_api_top->pstu_Manhattan_SDRIP_Param,sizeof(STU_CFDAPI_Manhattan_SDRIP));
//memcpy(&stu_TMO_Input.stu_Manhattan_TMOIP_Param,pstu_Cfd_api_top->pstu_Manhattan_TMO_Param,sizeof(STU_CFDAPI_Manhattan_TMOIP));

    memcpy(&stu_HDRIP_Input,pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_HDRIP_Input,sizeof(STU_CFD_MS_ALG_INTERFACE_HDRIP));
    memcpy(&stu_SDRIP_Input,pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_SDRIP_Input,sizeof(STU_CFD_MS_ALG_INTERFACE_SDRIP));
    memcpy(&stu_TMO_Input,pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_TMO_Input,sizeof(STU_CFD_MS_ALG_INTERFACE_TMO));
    memcpy(&stu_DLC_Input,pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_DLC_Input,sizeof(STU_CFD_MS_ALG_INTERFACE_DLC));

//Maserati M+D IP patch 0
#if NowHW == Maserati
#if HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT
    stu_HDRIP_Input.stu_Maserati_HDRIP_Param.u8HDRIP_Patch = 0;
    stu_HDRIP_Input.stu_Maserati_HDRIP_Param.u8HDRIP_Patch = stu_HDRIP_Input.stu_Maserati_HDRIP_Param.u8HDRIP_Patch | 0x01;
#endif
#endif
//

//API structure group
    STU_CFDAPI_MAIN_CONTROL *pstApiControlParamInit;
    STU_CFDAPI_MM_PARSER *pstu_MM_Param;
    STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParamInit;
    STU_CFDAPI_HDMI_INFOFRAME_PARSER *pstu_api_HDMI_InfoFrame_Param;
//STU_CFDAPI_HDR_FORMAT   *pstu_HDR_Format_Param;
    STU_CFDAPI_PANEL_FORMAT *pstu_Panel_Param;
    STU_CFDAPI_OSD_CONTROL  *pstu_OSD_Param;

//STU_CFDAPI_TMO_CONTROL *pstu_TMO_Param;
    pstApiControlParamInit = pstu_Cfd_api_top->pstu_Main_Control;
    pstu_MM_Param     = pstu_Cfd_api_top->pstu_MM_Param;
    pstHDMI_EDIDParamInit  = pstu_Cfd_api_top->pstu_HDMI_EDID_Param;
    pstu_api_HDMI_InfoFrame_Param = pstu_Cfd_api_top->pstu_HDMI_InfoFrame_Param;
    pstu_Panel_Param    = pstu_Cfd_api_top->pstu_Panel_Param;
    pstu_OSD_Param         = pstu_Cfd_api_top->pstu_OSD_Param;
//pstu_TMO_Param     = pstu_Cfd_api_top->pstu_TMO_Param;
    stu_ControlParam.pstu_Panel_Param_Colorimetry = &(pstu_Cfd_api_top->pstu_Panel_Param->stu_Cfd_Panel_ColorMetry);

//force some values when not Dolby
    if (( E_CFD_CFIO_DOLBY_HDR_TEMP != pstApiControlParamInit->u8Input_Format) || ( E_CFIO_MODE_HDR1 != pstApiControlParamInit->u8Input_HDRMode))
    {
        pstApiControlParamInit->stu_Middle_Format[0].u8Mid_IsFullRange = HW_PIPE_RANGE;
    }

#if NowHW == Maserati
//stu_SDRIP_Input.stu_Maserati_SDRIP_Param.pstu_Panel_Param = pstu_Cfd_api_top->pstu_Panel_Param;
//stu_HDRIP_Input.stu_Maserati_HDRIP_Param.pstu_Panel_Param = pstu_Cfd_api_top->pstu_Panel_Param;
#endif

    u8MainSubMode = pstu_Cfd_api_top->pstu_HW_IP_Param->u8HW_MainSub_Mode;
    if(TRUE == bCFDrun )
    {
        //pstu_OSD_Param->u16Hue = MApi_GFLIP_XC_R2BYTE(REG_SC_BK30_01_L);
        //pstu_OSD_Param->u16Saturation = MApi_GFLIP_XC_R2BYTE(REG_SC_BK30_02_L);
        //pstu_OSD_Param->u16Contrast = MApi_GFLIP_XC_R2BYTE(REG_SC_BK30_03_L);
        if(1 == pstu_OSD_Param->u8OSD_UI_Mode)
        {
            MS_Cfd_OSD_Control(pstu_OSD_Param,u8MainSubMode);
            return;
        }
    }

//input control check  function ================================================================
    u16_check_status_tmp = Mapi_Cfd_inter_Main_Control_Param_Check(pstApiControlParamInit);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

//==============================================================================================
#if NowHW == Maserati
#ifdef HE_PIPE_PRECONSTRAINTS
    //if (( E_CFD_CFIO_DOLBY_HDR_TEMP != pstApiControlParamInit->u8Input_Format) || ( E_CFIO_MODE_HDR1 != pstApiControlParamInit->u8Input_HDRMode))
    if(E_CFIO_MODE_HDR1 != pstApiControlParamInit->u8Input_HDRMode)
    {
        u16_check_status_tmp = Mapi_Cfd_inter_Main_Control_PreConstraints(pstApiControlParamInit);
        stu_ControlParam.u8DoPreConstraints_Flag = 1;
    }
    else
    {

        pstApiControlParamInit->u8Input_HDRIPMode = 0;
        pstApiControlParamInit->u8Input_SDRIPMode = 1;
        pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format = E_CFD_CFIO_YUV_BT709;
        pstApiControlParamInit->stu_Middle_Format[0].u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;
        pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode = E_CFIO_MODE_SDR;
        pstApiControlParamInit->stu_Middle_Format[0].u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
    }
#endif
#endif

//turn off this function
#if 0
    if (0 == pstApiControlParamInit->u8Process_Mode)
    {

        u16_check_status = E_CFD_MC_ERR_PROCESSOFF;

        return u16_check_status;
    }
#endif

    u16_check_status_tmp = Mapi_Cfd_inter_TOP_Param_CrossCheck(pstu_Cfd_api_top);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }
//assign input ========================================================================
    stu_ControlParam.u8Input_Format = pstApiControlParamInit->u8Input_Format;
    stu_ControlParam.u8Input_DataFormat = pstApiControlParamInit->u8Input_DataFormat;
    stu_ControlParam.u8Input_IsFullRange = pstApiControlParamInit->u8Input_IsFullRange;
    stu_ControlParam.u8Input_HDRMode = pstApiControlParamInit->u8Input_HDRMode;
    stu_ControlParam.u8Input_IsRGBBypass = pstApiControlParamInit->u8Input_IsRGBBypass;
    stu_ControlParam.u8Input_SDRIPMode = pstApiControlParamInit->u8Input_SDRIPMode;
    stu_ControlParam.u8Input_HDRIPMode = pstApiControlParamInit->u8Input_HDRIPMode;
//if (E_CFD_MC_SOURCE_MM == pstApiControlParamInit->u8Input_Source) //TV, STB
    if ((E_CFD_INPUT_SOURCE_STORAGE == pstApiControlParamInit->u8Input_Source)||
        (E_CFD_INPUT_SOURCE_DTV == pstApiControlParamInit->u8Input_Source)||
        (E_CFD_INPUT_SOURCE_JPEG == pstApiControlParamInit->u8Input_Source))
    {
        //printk("\033[31m [%s] MM/DTV source\033[m\n", __func__);
        //check for MM input
        u16_check_status_tmp = Mapi_Cfd_inter_MM_Param_Check(pstu_MM_Param);

        if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
        {
            u16_check_status = u16_check_status_tmp;
            u8ERR_Happens_Flag = 1;
        }

        //force values
        //not allowed to controlled by user at this moment
        //pstApiControlParamInit->u8MMInput_ColorimetryHandle_Mode = 1;

        if ((1 == pstApiControlParamInit->u8MMInput_ColorimetryHandle_Mode) || (3 == pstApiControlParamInit->u8MMInput_ColorimetryHandle_Mode))
        {
            stu_ControlParam.u8DoMMIn_ForceHDMI_Flag = 1;
        }
        else
        {
            stu_ControlParam.u8DoMMIn_ForceHDMI_Flag = 0;
        }

        //not used this information in Dolby case
        if (E_CFD_CFIO_DOLBY_HDR_TEMP == stu_ControlParam.u8Input_Format)
        {
            stu_ControlParam.u8InputColorPriamries = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
            stu_ControlParam.u8InputTransferCharacterstics = E_CFD_CFIO_TR_BT709;
            stu_ControlParam.u8InputMatrixCoefficients = E_CFD_CFIO_MC_BT709_XVYCC709;
        }
        else
        {
            if ((1 == pstApiControlParamInit->u8MMInput_ColorimetryHandle_Mode) || (3 == pstApiControlParamInit->u8MMInput_ColorimetryHandle_Mode))
            {
                //set u8InputFormat a specific colormetry with E_CFD_CFIO definition
                //force other no HDMI colormetry to BT709 RGB/YUV

                //set stu_ControlParam.u8InputFormat
                //stu_ControlParam.u8InputColorPriamries,stu_ControlParam.u8InputTransferCharacterstics,stu_ControlParam.u8InputMatrixCoefficients
                U16Temp0 = MS_Cfd_InputMM_SetColorMetry(&stu_ControlParam,pstu_MM_Param,pstApiControlParamInit,pstApiControlParamInit->u8MMInput_ColorimetryHandle_Mode);
                stu_ControlParam.u8DoMMIn_Force709_Flag = 0;

                if ((1 == pstApiControlParamInit->u8MMInput_ColorimetryHandle_Mode) && (1 == (U16Temp0&0x0001)))
                {
                    stu_ControlParam.u8DoMMIn_Force709_Flag = 1;
                }

                if (0x0010 == (U16Temp0&0x0010))
                {
                    u16_check_status_tmp = E_CFD_MC_ERR_MM_Handle_Undefined_Case;
                }

                if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
                {
                    u16_check_status = u16_check_status_tmp;
                    u8ERR_Happens_Flag = 1;
                }

#if 0
                if (1 == stu_ControlParam.u8DoMMIn_Force709_Flag)
                {
                    HDR_DBG_HAL_CFD(printk("\n  Force:input_format is forced to BT709 RGB or YUV due to not mapping to a HDMI case\n"));
                }
#endif
            }
            else if (2 == pstApiControlParamInit->u8MMInput_ColorimetryHandle_Mode)
            {
                //force MMinput to SDR YUV limit 709
                stu_ControlParam.u8DoMMIn_Force709_Flag = 1;
                MS_Cfd_ForceMMInputToRec709(&stu_ControlParam,pstu_MM_Param->u8Transfer_Characteristics);
                //HDR_DBG_HAL_CFD(printk("\n  Force:input_format is forced to BT709 RGB or YUV due to u8ColorDescription_Valid = 0 \n"));
            }
            else
            {
                //do nothing
                //stu_ControlParam.u8InputColorPriamries = pstu_MM_Param->u8Colour_primaries;
                //stu_ControlParam.u8InputTransferCharacterstics = pstu_MM_Param->u8Transfer_Characteristics;
                //stu_ControlParam.u8InputMatrixCoefficients = pstu_MM_Param->u8Matrix_Coeffs;
                //handle undefined case of color primary
                if((E_CFD_CFIO_CP_RESERVED0 == pstu_MM_Param->u8Colour_primaries) ||
                   (E_CFD_CFIO_CP_UNSPECIFIED == pstu_MM_Param->u8Colour_primaries) ||
                   (E_CFD_CFIO_CP_RESERVED3 == pstu_MM_Param->u8Colour_primaries) ||
                   (E_CFD_CFIO_CP_RESERVED_START <= pstu_MM_Param->u8Colour_primaries))
                {
                    pstu_ControlParam->u8InputColorPriamries = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
                    u16_check_status_tmp = E_CFD_MC_ERR_MM_Handle_Undefined_Case;
                }
                else
                {
                    pstu_ControlParam->u8InputColorPriamries = pstu_MM_Param->u8Colour_primaries;

                }

                //handle undefined case of Matrix coeff
                if((E_CFD_CFIO_MC_UNSPECIFIED == pstu_MM_Param->u8Matrix_Coeffs) ||
                   (E_CFD_CFIO_MC_RESERVED == pstu_MM_Param->u8Matrix_Coeffs) ||
                   (E_CFD_CFIO_MC_RESERVED_START <= pstu_MM_Param->u8Matrix_Coeffs))
                {
                    pstu_ControlParam->u8InputMatrixCoefficients = E_CFD_CFIO_MC_BT709_XVYCC709;
                    u16_check_status_tmp = E_CFD_MC_ERR_MM_Handle_Undefined_Case;
                }
                else
                {
                    pstu_ControlParam->u8InputMatrixCoefficients = pstu_MM_Param->u8Matrix_Coeffs;
                }

                //handle undefined case of transfer characteristic
                if((E_CFD_CFIO_TR_RESERVED0 == pstu_MM_Param->u8Transfer_Characteristics) ||
                   (E_CFD_CFIO_TR_UNSPECIFIED == pstu_MM_Param->u8Transfer_Characteristics) ||
                   (E_CFD_CFIO_TR_RESERVED3 == pstu_MM_Param->u8Transfer_Characteristics) ||
                   (E_CFD_CFIO_TR_RESERVED_START <= pstu_MM_Param->u8Transfer_Characteristics))
                {
                    pstu_ControlParam->u8InputTransferCharacterstics = E_CFD_CFIO_TR_BT709;
                    u16_check_status_tmp = E_CFD_MC_ERR_MM_Handle_Undefined_Case;
                }
                else
                {
                    pstu_ControlParam->u8InputTransferCharacterstics = pstu_MM_Param->u8Transfer_Characteristics;
                }

                if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
                {
                    u16_check_status = u16_check_status_tmp;
                    u8ERR_Happens_Flag = 1;
                }

                //need to add force for not defined and not specified case
                //should handle RGB case
                //change the value of MC from 0 to the value from format
                MS_Cfd_SearchAndMappingHDMIcase_OnlyRGB(pstu_ControlParam,pstu_MM_Param,pstApiControlParamInit,&u8idx);
            }
        }

        //set
        //if ((1 == stu_ControlParam.u8InputColorPriamries) && (1 == pstMMParamInit->u8Video_Full_Range_Flag))
        if ((E_CFD_CFIO_XVYCC_601 == stu_ControlParam.u8Input_Format) ||
            (E_CFD_CFIO_XVYCC_709 == stu_ControlParam.u8Input_Format) ||
            (E_CFD_CFIO_SYCC601 == stu_ControlParam.u8Input_Format))
        {
            stu_ControlParam.u8Input_GamutOrderIdx = E_CFD_CFIO_GT_XVYCC;
        }
        else
        {
            stu_ControlParam.u8Input_GamutOrderIdx = u8_gamut_idx_order[stu_ControlParam.u8InputColorPriamries];
        }

        //Source luminance
        if ((1 == pstApiControlParamInit->u8TMO_TargetRefer_Mode) && (E_CFD_VALID == pstu_MM_Param->u8Mastering_Display_Infor_Valid))
        {
            //stu_TMO_Input.stu_CFD_TMO_Param.u16SourceMax = MS_Cfd_Luminance_To_PQCode(pstu_MM_Param->u32Master_Panel_Max_Luminance/10000,0);
            //stu_TMO_Input.stu_CFD_TMO_Param.u16SourceMin = MS_Cfd_Luminance_To_PQCode(pstu_MM_Param->u32Master_Panel_Min_Luminance,1);
            #if (0 == DePQClamp_EN)
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = pstu_MM_Param->u32Master_Panel_Max_Luminance/10000;
            #else
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = MS_Cfd_SourceMax_Clip_MM(pstu_MM_Param);
            #endif
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMin = pstu_MM_Param->u32Master_Panel_Min_Luminance;

            STU_HDR_METADATA.u16Maxluminance_clip = MS_Cfd_SourceMax_Clip_MM(pstu_MM_Param);
        }
        else
        {
            //stu_TMO_Input.stu_CFD_TMO_Param.u16SourceMax = MS_Cfd_Luminance_To_PQCode(pstApiControlParamInit->u16Source_Max_Luminance,0);
            //stu_TMO_Input.stu_CFD_TMO_Param.u16SourceMin = MS_Cfd_Luminance_To_PQCode(pstApiControlParamInit->u16Source_Min_Luminance,1);
            #if (0 == DePQClamp_EN)
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = pstApiControlParamInit->u16Source_Max_Luminance;
            #else
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = DePQClampMin;
            #endif
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMin = pstApiControlParamInit->u16Source_Min_Luminance;

            STU_HDR_METADATA.u16Maxluminance_clip = DePQClampMin;
        }
        pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMed = pstApiControlParamInit->u16Source_Med_Luminance;

    }
    else if  (E_CFD_INPUT_SOURCE_HDMI == pstApiControlParamInit->u8Input_Source) //TV, STB
    {
        //printk("\033[31m [%s] HDMI source\033[m\n", __func__);
        //find u8InputFormat,u8InputIsFullRange,u8InputGamutIdx from outside information
        //u8InputColorPriamries,u8InputTransferCharacterstics,u8InputMatrixCoefficients

        //check input
        u16_check_status_tmp = Mapi_Cfd_inter_HDMI_InfoFrame_Param_Check(pstu_api_HDMI_InfoFrame_Param);

        if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
        {
            u16_check_status = u16_check_status_tmp;
            u8ERR_Happens_Flag = 1;
        }

        //not used this information in Dolby case
        if (E_CFD_CFIO_DOLBY_HDR_TEMP == stu_ControlParam.u8Input_Format)
        {
            stu_ControlParam.u8InputColorPriamries = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
            stu_ControlParam.u8InputTransferCharacterstics = E_CFD_CFIO_TR_BT709;
            stu_ControlParam.u8InputMatrixCoefficients = E_CFD_CFIO_MC_BT709_XVYCC709;
        }
        else
        {
#if 0
            if ( E_CFD_NOT_VALID == pstu_api_HDMI_InfoFrame_Param->u8HDMISource_AVIInfoFrame_Valid )
            {
                if (0 == u8ERR_Happens_Flag)
                {
                    u16_check_status = E_CFD_MC_ERR_HDMIINPUT;
                    u8ERR_Happens_Flag = 1;
                }

                //force MMinput to SDR YUV limit 709
                MS_Cfd_ForceMMInputToRec709(&stu_ControlParam);

                //return u16_check_status;
            }
            else
#endif
            {
                stu_ControlParam.u8DoHDMIIn_Force709_Flag = MS_Cfd_InputHDMI_SetColorMetry(&stu_ControlParam,pstu_api_HDMI_InfoFrame_Param,pstApiControlParamInit);
            }
        }

        if ((E_CFD_CFIO_XVYCC_601 == stu_ControlParam.u8Input_Format) ||
            (E_CFD_CFIO_XVYCC_709 == stu_ControlParam.u8Input_Format) ||
            (E_CFD_CFIO_SYCC601 == stu_ControlParam.u8Input_Format))
        {
            stu_ControlParam.u8Input_GamutOrderIdx = E_CFD_CFIO_GT_XVYCC;
        }
        else
        {
            stu_ControlParam.u8Input_GamutOrderIdx = u8_gamut_idx_order[stu_ControlParam.u8InputColorPriamries];
        }


#if 1
        //Source luminance
        if ((1 == pstApiControlParamInit->u8TMO_TargetRefer_Mode) && (E_CFD_VALID == pstu_api_HDMI_InfoFrame_Param->u8HDMISource_HDR_InfoFrame_Valid))
        {
            //stu_TMO_Input.stu_CFD_TMO_Param.u16Smax = MS_Cfd_Luminance_To_PQCode(pstu_api_HDMI_InfoFrame_Param->u16Master_Panel_Max_Luminance,0);
            //stu_TMO_Input.stu_CFD_TMO_Param.u16Smin = MS_Cfd_Luminance_To_PQCode(pstu_api_HDMI_InfoFrame_Param->u16Master_Panel_Min_Luminance,1);
            //lack of stu_ControlParam.stu_CFD_TMO_Param.u16Smed
            #if (0 == DePQClamp_EN)
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = pstu_api_HDMI_InfoFrame_Param->u16Master_Panel_Max_Luminance;
            #else
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = MS_Cfd_SourceMax_Clip_HDMI(pstu_api_HDMI_InfoFrame_Param);
            #endif
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMin = pstu_api_HDMI_InfoFrame_Param->u16Master_Panel_Min_Luminance;

            STU_HDR_METADATA.u16Maxluminance_clip = MS_Cfd_SourceMax_Clip_HDMI(pstu_api_HDMI_InfoFrame_Param);
        }
        else
        {
            #if (0 == DePQClamp_EN)
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = pstApiControlParamInit->u16Source_Max_Luminance;
            #else
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = DePQClampMin;
            #endif
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMin = pstApiControlParamInit->u16Source_Min_Luminance;

            STU_HDR_METADATA.u16Maxluminance_clip = DePQClampMin;
        }

        pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMed = pstApiControlParamInit->u16Source_Med_Luminance;
#endif

    }
//else if  (E_CFD_MC_SOURCE_ANALOG == pstApiControlParamInit->u8Input_Source)
    else if  ((E_CFD_INPUT_SOURCE_VGA == pstApiControlParamInit->u8Input_Source) ||
              (E_CFD_INPUT_SOURCE_TV == pstApiControlParamInit->u8Input_Source) ||
              (E_CFD_INPUT_SOURCE_CVBS == pstApiControlParamInit->u8Input_Source) ||
              (E_CFD_INPUT_SOURCE_SVIDEO == pstApiControlParamInit->u8Input_Source) ||
              (E_CFD_INPUT_SOURCE_YPBPR == pstApiControlParamInit->u8Input_Source) ||
              (E_CFD_INPUT_SOURCE_SCART == pstApiControlParamInit->u8Input_Source) ||
              (E_CFD_INPUT_SOURCE_DVI == pstApiControlParamInit->u8Input_Source) ||
              (E_CFD_INPUT_SOURCE_KTV == pstApiControlParamInit->u8Input_Source) ||
              (E_CFD_INPUT_SOURCE_RX == pstApiControlParamInit->u8Input_Source))

    {
        printk("\033[31m [%s] Analog source\033[m\n", __func__);
        //u16_check_status = MS_Cfd_Input_Analog_SetConfigures(pstApiControlParamInit,&stu_ControlParam);

        stu_ControlParam.u8InputColorPriamries = pstApiControlParamInit->u8Input_ext_Colour_primaries;
        stu_ControlParam.u8InputTransferCharacterstics = pstApiControlParamInit->u8Input_ext_Transfer_Characteristics;
        stu_ControlParam.u8InputMatrixCoefficients = pstApiControlParamInit->u8Input_ext_Matrix_Coeffs;

        //need to modify
        if ((E_CFD_CFIO_XVYCC_601 == pstu_ControlParam->u8Input_Format) ||
            (E_CFD_CFIO_XVYCC_709 == pstu_ControlParam->u8Input_Format) ||
            (E_CFD_CFIO_SYCC601 == pstu_ControlParam->u8Input_Format))
        {
            pstu_ControlParam->u8Input_GamutOrderIdx = E_CFD_CFIO_GT_XVYCC;
        }
        else
        {
            pstu_ControlParam->u8Input_GamutOrderIdx = u8_gamut_idx_order[stu_ControlParam.u8InputColorPriamries];
        }

    }
    else //reserved
    {
        //treat as HDMI case  in
        //the details of format only depends on input_format
        stu_ControlParam.u8DoOtherIn_Force709_Flag = MS_Cfd_InputHDMI_SetColorMetry_lite(&stu_ControlParam,pstApiControlParamInit);

        if ((E_CFD_CFIO_XVYCC_601 == stu_ControlParam.u8Input_Format) ||
            (E_CFD_CFIO_XVYCC_709 == stu_ControlParam.u8Input_Format) ||
            (E_CFD_CFIO_SYCC601 == stu_ControlParam.u8Input_Format))
        {
            stu_ControlParam.u8Input_GamutOrderIdx = E_CFD_CFIO_GT_XVYCC;
        }
        else
        {
            stu_ControlParam.u8Input_GamutOrderIdx = u8_gamut_idx_order[stu_ControlParam.u8InputColorPriamries];
        }

    } //end of if (E_CFD_MC_SOURCE_MM == pstControlParamInit->u8Input_Source) //TV, STB

////assign temp_format[0] ==========================================================================================
//this is special case for Masearti
#if (HW_PIPE_MODE == 0)

//temp_format[0] = input_format
    stu_ControlParam.u8Temp_Format[0] = stu_ControlParam.u8Input_Format;
    stu_ControlParam.u8Temp_DataFormat[0] = stu_ControlParam.u8Input_DataFormat;
    stu_ControlParam.u8Temp_IsFullRange[0] = stu_ControlParam.u8Input_IsFullRange;
    stu_ControlParam.u8Temp_HDRMode[0] = stu_ControlParam.u8Input_HDRMode;

    stu_ControlParam.u8TempColorPriamries[0] = stu_ControlParam.u8InputColorPriamries;
    stu_ControlParam.u8TempTransferCharacterstics[0] = stu_ControlParam.u8InputTransferCharacterstics;
    stu_ControlParam.u8TempMatrixCoefficients[0] = stu_ControlParam.u8InputMatrixCoefficients;

    stu_ControlParam.u8Temp_GamutOrderIdx[0] = stu_ControlParam.u8Input_GamutOrderIdx;

#elif (HW_PIPE_MODE == 1)

//u8IP2CSC_Mode
    stu_ControlParam.u8Temp_HDRMode[0] = stu_ControlParam.u8Input_HDRMode;
    stu_ControlParam.u8Temp_GamutOrderIdx[0] = stu_ControlParam.u8Input_GamutOrderIdx;
    stu_ControlParam.u8Temp_Format[0] = stu_ControlParam.u8Input_Format;
    stu_ControlParam.u8Temp_DataFormat[0] = stu_ControlParam.u8Input_DataFormat;
    stu_ControlParam.u8Temp_IsFullRange[0] = stu_ControlParam.u8Input_IsFullRange;
//IP2 CSC decision tree should be here
    if (E_CFD_INPUT_SOURCE_HDMI != stu_ControlParam.u8Input_Source || 1 == u8MainSubMode)
    {
    stu_SDRIP_Input.stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode = MS_Cfd_Maserati_InputCSC_Decision(&stu_ControlParam,&stu_SDRIP_Input);
    }
    else //when HDMI , force OFF
    {
        if(0x80 == (stu_SDRIP_Input.stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode&0x80))
        {
            stu_SDRIP_Input.stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode = (stu_SDRIP_Input.stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode&MaskForMode_HB)|(E_CFD_IP_CSC_OFF&MaskForMode_LB);
        }
        else
        {
            stu_ControlParam.u16_check_status = MS_Cfd_Maserati_CheckModes(&(stu_SDRIP_Input.stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode), 0x01);
            //u8_mode = pstu_SDRIP_Param->stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode;
        }
    }

    if ((0 == stu_ControlParam.u8Input_SDRIPMode) || (2 == stu_ControlParam.u8Input_SDRIPMode))
    {
        //stu_SDRIP_Input.stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode = E_CFD_IP_CSC_OFF;
        function1(stu_SDRIP_Input.stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode,E_CFD_IP_CSC_OFF);
    }



    switch((stu_SDRIP_Input.stu_Maserati_SDRIP_Param.u8IP2_CSC_Mode&MaskForMode_LB))
    {
        case E_CFD_IP_CSC_OFF:
            //no process
            break;

        case E_CFD_IP_CSC_RFULL_TO_RLIMIT:
            //no process
            stu_ControlParam.u8Temp_IsFullRange[0] = E_CFD_CFIO_RANGE_LIMIT;
            break;

        case E_CFD_IP_CSC_RFULL_TO_YFULL:
            stu_ControlParam.u8Temp_Format[0] = MS_Cfd_SetFormat_OnlyR2Y(stu_ControlParam.u8Input_Format);
            stu_ControlParam.u8Temp_DataFormat[0] = E_CFD_MC_FORMAT_YUV444;
            //stu_ControlParam.u8Temp_IsFullRange[0] = E_CFD_CFIO_RANGE_FULL;
            break;

        case E_CFD_IP_CSC_RFULL_TO_YLIMIT:
            stu_ControlParam.u8Temp_Format[0] = MS_Cfd_SetFormat_OnlyR2Y(stu_ControlParam.u8Input_Format);
            stu_ControlParam.u8Temp_DataFormat[0] = E_CFD_MC_FORMAT_YUV444;
            stu_ControlParam.u8Temp_IsFullRange[0] = E_CFD_CFIO_RANGE_LIMIT;
            break;

        case E_CFD_IP_CSC_RLIMIT_TO_RFULL: //RGBbypass only
            //stu_ControlParam.u8Temp_DataFormat[0] = E_CFD_MC_FORMAT_YUV444;
            stu_ControlParam.u8Temp_IsFullRange[0] = E_CFD_CFIO_RANGE_FULL;
            break;

        case E_CFD_IP_CSC_RLIMIT_TO_YFULL:
            stu_ControlParam.u8Temp_Format[0] = MS_Cfd_SetFormat_OnlyR2Y(stu_ControlParam.u8Input_Format);
            stu_ControlParam.u8Temp_DataFormat[0] = E_CFD_MC_FORMAT_YUV444;
            stu_ControlParam.u8Temp_IsFullRange[0] = E_CFD_CFIO_RANGE_FULL;
            break;

        case E_CFD_IP_CSC_RLIMIT_TO_YLIMIT:
            stu_ControlParam.u8Temp_Format[0] = MS_Cfd_SetFormat_OnlyR2Y(stu_ControlParam.u8Input_Format);
            stu_ControlParam.u8Temp_DataFormat[0] = E_CFD_MC_FORMAT_YUV444;
            stu_ControlParam.u8Temp_IsFullRange[0] = E_CFD_CFIO_RANGE_LIMIT;
            break;

        case E_CFD_IP_CSC_YFULL_TO_YLIMIT:
            //stu_ControlParam.u8Temp_DataFormat[0] = E_CFD_MC_FORMAT_YUV444;
            stu_ControlParam.u8Temp_IsFullRange[0] = E_CFD_CFIO_RANGE_LIMIT;
            break;

        case E_CFD_IP_CSC_YLIMIT_TO_YFULL:
            //stu_ControlParam.u8Temp_DataFormat[0] = E_CFD_MC_FORMAT_YUV444;
            stu_ControlParam.u8Temp_IsFullRange[0] = E_CFD_CFIO_RANGE_FULL;
            break;

        default:
            //no process
            break;
    }

//select cp, tr , mc from u8Temp_Format[0]
//MM cases
    if ((E_CFD_INPUT_SOURCE_STORAGE == pstApiControlParamInit->u8Input_Source)||
        (E_CFD_INPUT_SOURCE_DTV == pstApiControlParamInit->u8Input_Source)||
        (E_CFD_INPUT_SOURCE_JPEG == pstApiControlParamInit->u8Input_Source))
    {
        if ((E_CFD_CFIO_RGB_NOTSPECIFIED == pstu_ControlParam->u8Temp_Format[0])||
            (E_CFD_CFIO_YUV_NOTSPECIFIED == pstu_ControlParam->u8Temp_Format[0]))
        {
            stu_ControlParam.u8DoOtherIn_Force709_Flag = 0;
            pstu_ControlParam->u8TempColorPriamries[0] = pstu_ControlParam->u8InputColorPriamries;
            pstu_ControlParam->u8TempTransferCharacterstics[0] = pstu_ControlParam->u8InputTransferCharacterstics;
            pstu_ControlParam->u8TempMatrixCoefficients[0] = pstu_ControlParam->u8InputMatrixCoefficients;
        }
        else
        {
            stu_ControlParam.u8DoOtherIn_Force709_Flag = MS_Cfd_InputFormat_SetColorMetry(&stu_ControlParam,0);
        }
    }
    else
    {
        stu_ControlParam.u8DoOtherIn_Force709_Flag = MS_Cfd_InputFormat_SetColorMetry(&stu_ControlParam,0);
    }

#endif
//========================================================================================
#if NowHW == Maserati
//constraints for Maserati M+D IP =========================================================
    u16_check_status_tmp = MS_Cfd_Maserati_CheckHDRIPProcessIsOk(pstApiControlParamInit,&stu_ControlParam);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

    if (u16_check_status_tmp != E_CFD_MC_ERR_NOERR)
    {
        pstApiControlParamInit->u8Input_HDRIPMode = 0;
        stu_ControlParam.u8Input_HDRIPMode = 0;

        stu_ControlParam.u8DoHDRIP_Forcebypass_Flag = 1;
    }
//=========================================================================================
#endif

    if (0 == pstApiControlParamInit->u8Input_HDRIPMode)
    {
        if(E_CFD_INPUT_SOURCE_HDMI == pstApiControlParamInit->u8Input_Source && E_CFIO_MODE_HDR1 == pstApiControlParamInit->u8Input_HDRMode)
        {
            stu_ControlParam.u8Temp_Format[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format;
            stu_ControlParam.u8Temp_DataFormat[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_DataFormat;
            stu_ControlParam.u8Temp_IsFullRange[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_IsFullRange;
            stu_ControlParam.u8Temp_HDRMode[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode;
        }
        else
        {
            stu_ControlParam.u8Temp_Format[1] = stu_ControlParam.u8Temp_Format[0];
            stu_ControlParam.u8Temp_DataFormat[1] = stu_ControlParam.u8Temp_DataFormat[0];
            stu_ControlParam.u8Temp_IsFullRange[1] = stu_ControlParam.u8Temp_IsFullRange[0];
            stu_ControlParam.u8Temp_HDRMode[1] = stu_ControlParam.u8Temp_HDRMode[0];
        }
    }
    else
    {
        stu_ControlParam.u8Temp_Format[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format;
        stu_ControlParam.u8Temp_DataFormat[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_DataFormat;
        stu_ControlParam.u8Temp_IsFullRange[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_IsFullRange;
        stu_ControlParam.u8Temp_HDRMode[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode;

        if ((E_CFD_INPUT_SOURCE_STORAGE == pstApiControlParamInit->u8Input_Source)||
            (E_CFD_INPUT_SOURCE_DTV == pstApiControlParamInit->u8Input_Source)||
            (E_CFD_INPUT_SOURCE_JPEG == pstApiControlParamInit->u8Input_Source))
        {
            if ((E_CFD_CFIO_RGB_NOTSPECIFIED != stu_ControlParam.u8Temp_Format[0])&&(E_CFD_CFIO_YUV_NOTSPECIFIED != stu_ControlParam.u8Temp_Format[0]))
            {
                if ((E_CFD_CFIO_RGB_NOTSPECIFIED == pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format)||(E_CFD_CFIO_YUV_NOTSPECIFIED == pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format))
                {
                    stu_ControlParam.u8Temp_Format[1] = stu_ControlParam.u8Temp_Format[0];
                }
            }
        }

    }
//select cp, tr , mc from u8Temp_Format[1]
    if (( E_CFD_CFIO_DOLBY_HDR_TEMP == stu_ControlParam.u8Temp_Format[1]) && (1 == pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format_Mode))
    {
        stu_ControlParam.u8TempColorPriamries[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Colour_primaries;
        stu_ControlParam.u8TempTransferCharacterstics[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Transfer_Characteristics;
        stu_ControlParam.u8TempMatrixCoefficients[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Matrix_Coeffs;
    }
    else
    {
#if 1
        if ((E_CFD_INPUT_SOURCE_STORAGE == pstApiControlParamInit->u8Input_Source)||
            (E_CFD_INPUT_SOURCE_DTV == pstApiControlParamInit->u8Input_Source)||
            (E_CFD_INPUT_SOURCE_JPEG == pstApiControlParamInit->u8Input_Source))
        {
            if ((E_CFD_CFIO_RGB_NOTSPECIFIED == pstu_ControlParam->u8Temp_Format[1])||
                (E_CFD_CFIO_YUV_NOTSPECIFIED == pstu_ControlParam->u8Temp_Format[1]))
            {
                stu_ControlParam.u8DoOtherIn_Force709_Flag = 0;
                pstu_ControlParam->u8TempColorPriamries[1] = pstu_ControlParam->u8TempColorPriamries[0];
                pstu_ControlParam->u8TempTransferCharacterstics[1] = pstu_ControlParam->u8TempTransferCharacterstics[0];
                pstu_ControlParam->u8TempMatrixCoefficients[1] = pstu_ControlParam->u8TempMatrixCoefficients[0];
            }
            else
            {
                stu_ControlParam.u8DoOtherIn_Force709_Flag = MS_Cfd_InputFormat_SetColorMetry(&stu_ControlParam,1);
            }
        }
        else
        {
#endif
            stu_ControlParam.u8DoOtherIn_Force709_Flag = MS_Cfd_InputFormat_SetColorMetry(&stu_ControlParam,1);
#if 1
        }
#endif
    }

//Maserati M+D IP patch 0
#if NowHW == Maserati

//if (0x01 == (stu_HDRIP_Input.stu_Maserati_HDRIP_Param.u8HDRIP_Patch&0x01))
//{
// stu_ControlParam.u8TempTransferCharacterstics[1] = E_CFD_CFIO_TR_GAMMA2P2;
//}

#endif
//

    if (0 == pstApiControlParamInit->u8Input_HDRIPMode)
    {
        stu_ControlParam.u8Temp_GamutOrderIdx[1] = stu_ControlParam.u8Temp_GamutOrderIdx[0];
    }
    else
    {
        if ((E_CFD_CFIO_XVYCC_601 == stu_ControlParam.u8Temp_Format[1]) ||
            (E_CFD_CFIO_XVYCC_709 == stu_ControlParam.u8Temp_Format[1]) ||
            (E_CFD_CFIO_SYCC601 == stu_ControlParam.u8Temp_Format[1]))
        {
            stu_ControlParam.u8Temp_GamutOrderIdx[1] = E_CFD_CFIO_GT_XVYCC;
        }
        else
        {
            stu_ControlParam.u8Temp_GamutOrderIdx[1] = u8_gamut_idx_order[stu_ControlParam.u8TempColorPriamries[1]];
        }
    }

//purpose +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//find u8OutputFormat,u8OutputIsFullRange, u8OutputGamutIdx
//u8OutputColorPriamries,u8OutputTransferCharacterstics,u8OutputMatrixCoefficients

//constraints for Maserati SDR IP =========================================================
#if 0
    u16_check_status = MS_Cfd_Maserati_CheckSDRIPProcessIsOk(pstApiControlParamInit,&stu_ControlParam);

    if (u16_check_status == 0)
    {
        pstApiControlParamInit->u8Output_HDRMode = stu_ControlParam.u8Temp_HDRMode[1];

        stu_ControlParam.u8DoSDRIP_ForceNOTMO_Flag = 1;
    }
#endif
//=========================================================================================
//assign output
    if (0 == pstApiControlParamInit->u8Input_SDRIPMode)
    {
        stu_ControlParam.u8Output_Format = stu_ControlParam.u8Temp_Format[1];
        stu_ControlParam.u8Output_DataFormat = stu_ControlParam.u8Temp_DataFormat[1];
        stu_ControlParam.u8Output_IsFullRange = stu_ControlParam.u8Temp_IsFullRange[1];
        stu_ControlParam.u8Output_HDRMode = stu_ControlParam.u8Temp_HDRMode[1];
    }
    else
    {
        stu_ControlParam.u8Output_Format = pstApiControlParamInit->u8Output_Format;
        stu_ControlParam.u8Output_DataFormat = pstApiControlParamInit->u8Output_DataFormat;
        stu_ControlParam.u8Output_IsFullRange = pstApiControlParamInit->u8Output_IsFullRange;
        stu_ControlParam.u8Output_HDRMode = pstApiControlParamInit->u8Output_HDRMode;
    }

    if (E_CFD_OUTPUT_SOURCE_HDMI == pstApiControlParamInit->u8Output_Source) //STB
    {

        //check input
        u16_check_status_tmp = Mapi_Cfd_inter_HDMI_EDID_Param_Check(pstHDMI_EDIDParamInit);

        if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
        {
            u16_check_status = u16_check_status_tmp;
            u8ERR_Happens_Flag = 1;
        }

        //QS & QY in EDID in Video Capability Data Block(VCDB)
        MS_U8 u8Output_range_flag = 0;

        if (1 == pstHDMI_EDIDParamInit->u8HDMISink_EDID_Valid )
        {
#if 0
            stu_ControlParam.u8Output_DataFormat = pstApiControlParamInit->u8Output_DataFormat;
            //assign u8OutputIsFullRange +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=

            //bit 3:RGB_quantization_range
            //bit 4:Y_quantization_range 0:no data(due to CE or IT video) ; 1:selectable
            if (E_CFD_MC_FORMAT_RGB == pstApiControlParamInit->u8Output_DataFormat) // RGB
            {
                u8Output_range_flag = ((pstHDMI_EDIDParamInit->u8HDMISink_Support_YUVFormat>>3)&0x01);
            }
            else // YUV
            {
                u8Output_range_flag = ((pstHDMI_EDIDParamInit->u8HDMISink_Support_YUVFormat>>4)&0x01);
            }


            if (1 == u8Output_range_flag) //Via AVI
            {
                //force limit range for this case
                //0:limit 1:full
                stu_ControlParam.u8Output_IsFullRange = 0;
            }
            else //refer to CE/IT video format
            {
                //CE
                if (1 == pstApiControlParamInit->u8Output_Format_HDMI_CE_Flag)
                {
                    stu_ControlParam.u8Output_IsFullRange = 0;
                }
                else
                {
                    stu_ControlParam.u8Output_IsFullRange = 1;
                }
            }
#endif
            //assign u8OutputIsFullRange -----------------------------------------------------------------
            if (1 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_Mode)
            {

                //set u8Output_GamutOrderIdx by EDID or user controls
                MS_Cfd_SetOutputGamut_HDMISink(pstHDMI_EDIDParamInit, &stu_ControlParam, pstApiControlParamInit);

                //set by u8Output_GamutOrderIdx
                MS_Cfd_SetOutputColorParam_HDMISink(&stu_ControlParam);

            }
            else
            {
                //set by user-defined u8Output_Format & u8Output_HDRMode

                for (u8idx =0; u8idx<=9 ; u8idx++)
                {
                    if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3] == stu_ControlParam.u8Output_Format)||
                        (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4] == stu_ControlParam.u8Output_Format))
                    {
                        break;
                    }
                }

                if (u8idx > 8)
                {
                    u8idx = 8;
                }

                stu_ControlParam.u8OutputColorPriamries = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0];
                stu_ControlParam.u8OutputTransferCharacterstics = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][1];
                stu_ControlParam.u8OutputMatrixCoefficients = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][2];

#if 0
                if (E_CFIO_MODE_HDR2 == stu_ControlParam.u8Output_HDRMode)
                {
                    stu_ControlParam.u8OutputTransferCharacterstics = E_CFD_CFIO_TR_SMPTE2084;
                }
                else if (E_CFIO_MODE_HDR1 == stu_ControlParam.u8Output_HDRMode)
                {
                    stu_ControlParam.u8OutputTransferCharacterstics = E_CFD_CFIO_TR_DOLBYMETA;
                }
#else
                stu_ControlParam.u8OutputTransferCharacterstics = MS_Cfd_GetTR_FromHDRMode(stu_ControlParam.u8Output_HDRMode, stu_ControlParam.u8OutputTransferCharacterstics);
#endif

                if ((E_CFD_CFIO_XVYCC_601 == stu_ControlParam.u8Output_Format) ||
                    (E_CFD_CFIO_XVYCC_709 == stu_ControlParam.u8Output_Format) ||
                    (E_CFD_CFIO_SYCC601 == stu_ControlParam.u8Output_Format))
                {
                    stu_ControlParam.u8Output_GamutOrderIdx = E_CFD_CFIO_GT_XVYCC;
                }
                else
                {
                    stu_ControlParam.u8Output_GamutOrderIdx = u8_gamut_idx_order[stu_ControlParam.u8OutputColorPriamries];
                }
            }
            // if (1 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_En)

            //set
            //stu_ControlParam->u8InputFormat
            //stu_ControlParam->u8OutputColorPriamries
            //stu_ControlParam->u8OutputTransferCharacterstics
            //stu_ControlParam->u8OutputMatrixCoefficients


            //Target luminance
            //check HDR block in EDID exist?
            //if pstu_TMO_Param->u8TMO_TargetMode = 1;
            //if (1 == stu_TMO_Input.stu_CFD_TMO_Param.u8TMO_TargetMode)
            {
                if ((1 == pstApiControlParamInit->u8TMO_TargetRefer_Mode) && (E_CFD_VALID == pstHDMI_EDIDParamInit->u8HDMISink_HDRData_Block_Valid))
                {
                    //due to CEA-861.3
                    U16Temp0 = LUT_CFD_CV1[pstHDMI_EDIDParamInit->u8HDMISink_Desired_Content_Max_Luminance];
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMax = U16Temp0;

                    //Transfer pqcode inside TMO
                    //stu_TMO_Input.stu_CFD_TMO_Param.u16TgtMax = MS_Cfd_Luminance_To_PQCode(U16Temp0,0);

                    U16Temp0 = LUT_CFD_CV1[pstHDMI_EDIDParamInit->u8HDMISink_Desired_Content_Max_Frame_Avg_Luminance];
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMed = U16Temp0;
                    //Transfer pqcode inside TMO
                    //stu_TMO_Input.stu_CFD_TMO_Param.u16Tmed = MS_Cfd_Luminance_To_PQCode(U16Temp0,0);

                    U16Temp1 = (((MS_U32)U16Temp0)*LUT_CFD_CV2[pstHDMI_EDIDParamInit->u8HDMISink_Desired_Content_Min_Luminance])>>12;
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMin = U16Temp1;

                    //Transfer pqcode inside TMO
                    //stu_TMO_Input.stu_CFD_TMO_Param.u16TgtMin = MS_Cfd_Luminance_To_PQCode(U16Temp1,1);
                }
                else
                {
#if 1
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMax = pstApiControlParamInit->u16Target_Max_Luminance;
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMed = pstApiControlParamInit->u16Target_Med_Luminance;
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMin = pstApiControlParamInit->u16Target_Min_Luminance;
#else //Transfer pqcode inside TMO
                    //stu_TMO_Input.stu_CFD_TMO_Param.u16TgtMax = MS_Cfd_Luminance_To_PQCode(pstApiControlParamInit->u16Target_Max_Luminance,0);
                    //stu_TMO_Input.stu_CFD_TMO_Param.u16TgtMin = MS_Cfd_Luminance_To_PQCode(pstApiControlParamInit->u16Target_Min_Luminance,1);
                    //stu_TMO_Input.stu_CFD_TMO_Param.u16Tmed = MS_Cfd_Luminance_To_PQCode(pstApiControlParamInit->u16Target_Mean_Luminance,0);
#endif
                }
            }

        }
        else
        {
            //not consider this case now.
            //u16_check_status = E_CFD_MC_ERR_BYPASS;
        }

    }
    else if (E_CFD_OUTPUT_SOURCE_PANEL == pstApiControlParamInit->u8Output_Source) //TV
    {

        //check input
        u16_check_status_tmp = Mapi_Cfd_inter_PANEL_Param_Check(pstu_Panel_Param);

        if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
        {
            u16_check_status = u16_check_status_tmp;
            u8ERR_Happens_Flag = 1;
        }

        //force output values
        stu_ControlParam.u8Output_Format = E_CFD_CFIO_RGB_NOTSPECIFIED;
        stu_ControlParam.u8Output_DataFormat = E_CFD_MC_FORMAT_RGB;
        stu_ControlParam.u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;

        if (((E_CFD_CFIO_YUV_BT2020_CL == stu_ControlParam.u8Temp_Format[1]) ||
             (E_CFD_CFIO_XVYCC_601 == stu_ControlParam.u8Temp_Format[1]) ||
             (E_CFD_CFIO_XVYCC_709 == stu_ControlParam.u8Temp_Format[1]) ||
             (E_CFD_CFIO_SYCC601 == stu_ControlParam.u8Temp_Format[1])) && ( E_CFD_MC_FORMAT_RGB != stu_ControlParam.u8Input_DataFormat))
        {
            if (E_CFD_CFIO_YUV_BT2020_CL == stu_ControlParam.u8Temp_Format[1])
            {
                stu_ControlParam.u8OutputColorPriamries = E_CFD_CFIO_CP_BT2020;

                //HDR to HDR - openHDR
                if ((E_CFIO_MODE_HDR2 == stu_ControlParam.u8Temp_HDRMode[1] ) && (E_CFIO_MODE_HDR2 == stu_ControlParam.u8Output_HDRMode ))
                {
                    stu_ControlParam.u8OutputTransferCharacterstics = E_CFD_CFIO_TR_SMPTE2084;
                }
                else if ((E_CFIO_MODE_HDR3 == stu_ControlParam.u8Temp_HDRMode[1] ) && (E_CFIO_MODE_HDR3 == stu_ControlParam.u8Output_HDRMode ))
                {
                    stu_ControlParam.u8OutputTransferCharacterstics = E_CFD_CFIO_TR_HLG;
                }
                else if ((E_CFIO_MODE_HDR1 == stu_ControlParam.u8Temp_HDRMode[1] ) && (E_CFIO_MODE_HDR1 == stu_ControlParam.u8Output_HDRMode )) //HDR to HDR - DolbyHDR
                {
                    stu_ControlParam.u8OutputTransferCharacterstics = E_CFD_CFIO_TR_DOLBYMETA;
                }
                else //SDR to SDR & HDR to SDR
                {
                    stu_ControlParam.u8OutputTransferCharacterstics = E_CFD_CFIO_TR_BT2020NCL;
                }
                stu_ControlParam.u8OutputMatrixCoefficients = E_CFD_CFIO_MC_BT2020NCL;
                stu_ControlParam.u8Output_GamutOrderIdx = E_CFD_CFIO_GT_BT2020;
            }
            else //force to 709
            {
                stu_ControlParam.u8OutputColorPriamries = E_CFD_CFIO_CP_BT709_SRGB_SYCC;

                //HDR to HDR - openHDR
                if ((E_CFIO_MODE_HDR2 == stu_ControlParam.u8Temp_HDRMode[1] ) && (E_CFIO_MODE_HDR2 == stu_ControlParam.u8Output_HDRMode ))
                {
                    stu_ControlParam.u8OutputTransferCharacterstics = E_CFD_CFIO_TR_SMPTE2084;
                }
                else if ((E_CFIO_MODE_HDR3 == stu_ControlParam.u8Temp_HDRMode[1] ) && (E_CFIO_MODE_HDR3 == stu_ControlParam.u8Output_HDRMode ))
                {
                    stu_ControlParam.u8OutputTransferCharacterstics = E_CFD_CFIO_TR_HLG;
                }
                else if ((E_CFIO_MODE_HDR1 == stu_ControlParam.u8Temp_HDRMode[1] ) && (E_CFIO_MODE_HDR1 == stu_ControlParam.u8Output_HDRMode )) //HDR to HDR - DolbyHDR
                {
                    stu_ControlParam.u8OutputTransferCharacterstics = E_CFD_CFIO_TR_DOLBYMETA;
                }
                else //SDR to SDR & HDR to SDR
                {
                    stu_ControlParam.u8OutputTransferCharacterstics = E_CFD_CFIO_TR_BT709;
                }
                stu_ControlParam.u8OutputMatrixCoefficients = E_CFD_CFIO_MC_BT709_XVYCC709;
                stu_ControlParam.u8Output_GamutOrderIdx = E_CFD_CFIO_GT_BT709;
            }
        }
        else
        {
            if (1 == pstApiControlParamInit->u8PanelOutput_GammutMapping_Mode)
            {
                stu_ControlParam.u8OutputColorPriamries = E_CFD_CFIO_CP_PANEL;

                //HDR to SDR
                if ((E_CFIO_MODE_SDR != stu_ControlParam.u8Temp_HDRMode[1] ) && (E_CFIO_MODE_SDR == stu_ControlParam.u8Output_HDRMode ))
                {
                    //keeps the current input format
                    stu_ControlParam.u8OutputTransferCharacterstics = MS_Cfd_InputFormat_SetOutTR(&stu_ControlParam);
                }
                else //SDR to SDR & HDR to HDR , keeps tr the same
                {
                    stu_ControlParam.u8OutputTransferCharacterstics = stu_ControlParam.u8TempTransferCharacterstics[1];
                }

                stu_ControlParam.u8OutputMatrixCoefficients = E_CFD_CFIO_MC_UNSPECIFIED;
                stu_ControlParam.u8Output_GamutOrderIdx = E_CFD_CFIO_GT_PANEL_reservedstart;
            }
            else //keeps output is the same as input
            {
                stu_ControlParam.u8OutputColorPriamries = stu_ControlParam.u8TempColorPriamries[1];

                //HDR to SDR
                if ((E_CFIO_MODE_SDR != stu_ControlParam.u8Temp_HDRMode[1] ) && (E_CFIO_MODE_SDR == stu_ControlParam.u8Output_HDRMode ))
                {
                    stu_ControlParam.u8OutputTransferCharacterstics = MS_Cfd_InputFormat_SetOutTR(&stu_ControlParam);
                }
                else //SDR to SDR & HDR to HDR
                {
                    stu_ControlParam.u8OutputTransferCharacterstics = stu_ControlParam.u8TempTransferCharacterstics[1];
                }

                stu_ControlParam.u8OutputMatrixCoefficients = stu_ControlParam.u8TempMatrixCoefficients[1];
                stu_ControlParam.u8Output_GamutOrderIdx = stu_ControlParam.u8Temp_GamutOrderIdx[1];
            }
        }

        //only force Y2R
        if (2 == pstApiControlParamInit->u8Input_SDRIPMode)//only for Panel Out
        {
            //assign output
            stu_ControlParam.u8Output_Format = E_CFD_CFIO_RGB_NOTSPECIFIED;
            stu_ControlParam.u8Output_DataFormat = E_CFD_MC_FORMAT_RGB;
            stu_ControlParam.u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;
            stu_ControlParam.u8Output_HDRMode = stu_ControlParam.u8Temp_HDRMode[1];
            stu_ControlParam.u8OutputColorPriamries = stu_ControlParam.u8TempColorPriamries[1];
            stu_ControlParam.u8OutputTransferCharacterstics = stu_ControlParam.u8TempTransferCharacterstics[1];
            stu_ControlParam.u8OutputMatrixCoefficients = stu_ControlParam.u8TempMatrixCoefficients[1];
            stu_ControlParam.u8Output_GamutOrderIdx = stu_ControlParam.u8Temp_GamutOrderIdx[1];
        }
        else if (0 == pstApiControlParamInit->u8Input_SDRIPMode)
        {
            //assign output
            stu_ControlParam.u8Output_Format = stu_ControlParam.u8Temp_Format[1];
            stu_ControlParam.u8Output_DataFormat = stu_ControlParam.u8Temp_DataFormat[1];
            stu_ControlParam.u8Output_IsFullRange = stu_ControlParam.u8Temp_IsFullRange[1];
            stu_ControlParam.u8Output_HDRMode = stu_ControlParam.u8Temp_HDRMode[1];
            stu_ControlParam.u8OutputColorPriamries = stu_ControlParam.u8TempColorPriamries[1];
            stu_ControlParam.u8OutputTransferCharacterstics = stu_ControlParam.u8TempTransferCharacterstics[1];
            stu_ControlParam.u8OutputMatrixCoefficients = stu_ControlParam.u8TempMatrixCoefficients[1];
            stu_ControlParam.u8Output_GamutOrderIdx = stu_ControlParam.u8Temp_GamutOrderIdx[1];
        }

        //luminance (nits) to PQ Code
        //stu_ControlParam.stu_CFD_TMO_Param.
        //Target luminance

        //if (1 == stu_TMO_Input.stu_CFD_TMO_Param.u8TMO_TargetMode)
        {
            if (1 == pstApiControlParamInit->u8TMO_TargetRefer_Mode)
            {
#if 1
                pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMax = pstu_Panel_Param->u16Panel_Max_Luminance;
                pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMed = pstu_Panel_Param->u16Panel_Med_Luminance;
                pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMin = pstu_Panel_Param->u16Panel_Min_Luminance;
#else //Transfer pqcode inside TMO
                stu_TMO_Input.stu_CFD_TMO_Param.u16TgtMax = MS_Cfd_Luminance_To_PQCode(pstu_Panel_Param->u16Panel_Max_Luminance,0);
                //stu_TMO_Input.stu_CFD_TMO_Param.u16Tmed = MS_Cfd_Luminance_To_PQCode(pstu_Panel_Param->u16Panel_Frame_Avg_Luminance,0);
                stu_TMO_Input.stu_CFD_TMO_Param.u16TgtMin = MS_Cfd_Luminance_To_PQCode(pstu_Panel_Param->u16Panel_Min_Luminance,1);
#endif
            }
            else
            {
#if 1
                pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMax = pstApiControlParamInit->u16Target_Max_Luminance;
                pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMed = pstApiControlParamInit->u16Target_Med_Luminance;
                pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMin = pstApiControlParamInit->u16Target_Min_Luminance;
#else //Transfer pqcode inside TMO
                stu_TMO_Input.stu_CFD_TMO_Param.u16TgtMax = MS_Cfd_Luminance_To_PQCode(pstApiControlParamInit->u16Target_Max_Luminance,0);
                //stu_TMO_Input.stu_CFD_TMO_Param.u16Tmed = MS_Cfd_Luminance_To_PQCode(pstApiControlParamInit->u16Target_Mean_Luminance,0);
                stu_TMO_Input.stu_CFD_TMO_Param.u16TgtMin = MS_Cfd_Luminance_To_PQCode(pstApiControlParamInit->u16Target_Min_Luminance,1);
#endif
            }
        }
        //end of output_source = Panel
    }
    else
    {
        stu_ControlParam.u8DoOutput_Force709_Flag = MS_Cfd_InputHDMI_SetColorMetry_lite_out(&stu_ControlParam,pstApiControlParamInit);

        if ((E_CFD_CFIO_XVYCC_601 == stu_ControlParam.u8Input_Format) ||
            (E_CFD_CFIO_XVYCC_709 == stu_ControlParam.u8Input_Format) ||
            (E_CFD_CFIO_SYCC601 == stu_ControlParam.u8Input_Format))
        {
            stu_ControlParam.u8Output_GamutOrderIdx = E_CFD_CFIO_GT_XVYCC;
        }
        else
        {
            stu_ControlParam.u8Output_GamutOrderIdx = u8_gamut_idx_order[stu_ControlParam.u8OutputColorPriamries];
        }
    }
//end of output source determination

//constraints for Maserati SDR IP =========================================================
    u16_check_status_tmp = MS_Cfd_Maserati_CheckSDRIPProcessIsOk(pstApiControlParamInit,&stu_ControlParam);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

//Set u8DoGamutMapping_Flag
//Set u8DoTMO_Flag
//u16_check_status = MS_Cfd_Maserati_SetGM_TMO_Flags(&stu_ControlParam);
    u16_check_status_tmp = MS_Cfd_SetGM_TMO_Flags(&stu_ControlParam);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

//assign values to TMO,HDR,SDR functions
//consider hw design

//case 1 : Kano
//implement with sub function

//u16_check_status_tmp = MS_Cfd_Kano_InterfaceControl(&stu_ControlParam,&stu_TMO_Input,&stu_HDRIP_Input,&stu_SDRIP_Input);
//u16_check_status_tmp = MS_Cfd_Kano_TMO_Control(&stu_ControlParam,&stu_TMO_Input);
//u16_check_status = MS_Cfd_Kano_HDRIP_Control(&stu_ControlParam,&stu_HDRIP_Input);
//u16_check_status = MS_Cfd_Kano_SDRIP_Control(&stu_ControlParam,&stu_SDRIP_Input);

//need check functio
#if 1
#if NowHW == Kano
    u16_check_status_tmp = MS_Cfd_Kano_TMO_Control(&stu_ControlParam,&stu_TMO_Input);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

    u16_check_status_tmp = MS_Cfd_Kano_HDRIP_Control(&stu_ControlParam,&stu_HDRIP_Input,&stu_TMO_Input);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

    u16_check_status_tmp = MS_Cfd_Kano_SDRIP_Control(&stu_ControlParam,&stu_SDRIP_Input);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

#endif
//return to top
//memcpy(pstu_Cfd_api_top->pstu_Kano_HDRIP_Param,&stu_HDRIP_Input.stu_Kano_HDRIP_Param,sizeof(STU_CFDAPI_Kano_HDRIP));
//memcpy(pstu_Cfd_api_top->pstu_Kano_SDRIP_Param,&stu_SDRIP_Input.stu_Kano_SDRIP_Param,sizeof(STU_CFDAPI_Kano_SDRIP));
//memcpy(pstu_Cfd_api_top->pstu_Kano_TMO_Param,&stu_TMO_Input.stu_Kano_TMOIP_Param,sizeof(STU_CFDAPI_Kano_TMOIP));

#if NowHW == Manhattan
//case 2 : Manhattan
    u16_check_status_tmp = MS_Cfd_Manhattan_TMO_Control(&stu_ControlParam,&stu_TMO_Input);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

    u16_check_status_tmp = MS_Cfd_Manhattan_HDRIP_Control(&stu_ControlParam,&stu_HDRIP_Input);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

    u16_check_status_tmp = MS_Cfd_Manhattan_SDRIP_Control(&stu_ControlParam,&stu_SDRIP_Input,&stu_TMO_Input,u8MainSubMode);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

#endif
//memcpy(pstu_Cfd_api_top->pstu_Manhattan_HDRIP_Param,&stu_HDRIP_Input.stu_Manhattan_HDRIP_Param,sizeof(STU_CFDAPI_Manhattan_HDRIP));
//memcpy(pstu_Cfd_api_top->pstu_Manhattan_SDRIP_Param,&stu_SDRIP_Input.stu_Manhattan_SDRIP_Param,sizeof(STU_CFDAPI_Manhattan_SDRIP));
//memcpy(pstu_Cfd_api_top->pstu_Manhattan_TMO_Param,&stu_TMO_Input.stu_Manhattan_TMOIP_Param,sizeof(STU_CFDAPI_Manhattan_TMOIP));

#endif

#if NowHW == Maserati
//case 3 : Maserati
//not modified
    if(0 == u8MainSubMode)
    {
    u16_check_status_tmp = MS_Cfd_Maserati_ColorSampling_Control(&stu_ControlParam);
    }

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

    u16_check_status_tmp = MS_Cfd_Maserati_DLC_Control(&stu_ControlParam,&stu_DLC_Input,&stu_TMO_Input);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

    if (E_CFIO_MODE_HDR1 != stu_ControlParam.u8Input_HDRMode) // todo -> find the corresponding proper flag
    {
        if(0 == u8MainSubMode)
        {
        u16_check_status_tmp = MS_Cfd_Maserati_TMO_Control(&stu_ControlParam,&stu_TMO_Input);
    }
    }

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }
//kim
    if (E_CFIO_MODE_HDR1 == stu_ControlParam.u8Input_HDRMode) // todo -> find the corresponding proper flag
    {
        u16_check_status_tmp = MS_Cfd_Maserati_Dolby_Control(pstu_Cfd_api_top->pstu_Dolby_Param); // todo -> you need to allocate(KMalloc) memory before this line
    }
    else
    {
        if (u8MainSubMode == 0)
        {
        u16_check_status_tmp = MS_Cfd_Maserati_HDRIP_Control(&stu_ControlParam,&stu_HDRIP_Input,&stu_TMO_Input);
    }
    }

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }
    u16_check_status_tmp = MS_Cfd_Maserati_SDRIP_Control(&stu_ControlParam,&stu_SDRIP_Input,&stu_TMO_Input,u8MainSubMode);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }
#endif

#if NowHW == U13_Universe

    u16_check_status_tmp = MS_Cfd_Universe_SDRIP_Control(&stu_ControlParam,&stu_SDRIP_Input,&stu_TMO_Input);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag = 1;
    }

#endif

    if(E_CFD_MC_ERR_NOERR == u16_check_status)
    {
        bCFDrun = TRUE;
    }
    else
    {
        bCFDrun = FALSE;
    }

    MS_Cfd_OSD_Control(pstu_OSD_Param,u8MainSubMode);
    memcpy(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_HDRIP_Input,&stu_HDRIP_Input,sizeof(STU_CFD_MS_ALG_INTERFACE_HDRIP));
    memcpy(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_SDRIP_Input,&stu_SDRIP_Input,sizeof(STU_CFD_MS_ALG_INTERFACE_SDRIP));
    memcpy(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_TMO_Input,&stu_TMO_Input,sizeof(STU_CFD_MS_ALG_INTERFACE_TMO));
    memcpy(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_DLC_Input,&stu_DLC_Input,sizeof(STU_CFD_MS_ALG_INTERFACE_DLC));

//record process status
    pstApiControlParamInit->u8Process_Status =  (stu_ControlParam.u8DoBT2020CLP_Flag<<3) + (stu_ControlParam.u8DoTMO_Flag<<2) + (stu_ControlParam.u8DoGamutMapping_Flag<<1) + stu_ControlParam.u8DoDLC_Flag;
    pstApiControlParamInit->u8Process_Status +=  (stu_ControlParam.u8DoBT2020CLPInHDRIP_Flag<<7) + (stu_ControlParam.u8DoTMOInHDRIP_Flag<<6) + (stu_ControlParam.u8DoGamutMappingInHDRIP_Flag<<5) + (stu_ControlParam.u8DoDLCInHDRIP_Flag<<4);

    pstApiControlParamInit->u8Process_Status2 = (stu_ControlParam.u8DoOtherIn_Force709_Flag<<3) + (stu_ControlParam.u8DoHDMIIn_Force709_Flag<<2) + (stu_ControlParam.u8DoMMIn_Force709_Flag<<1) + (stu_ControlParam.u8DoMMIn_ForceHDMI_Flag);
    pstApiControlParamInit->u8Process_Status2+= (stu_ControlParam.u8DoOutput_Force709_Flag<<4);

    pstApiControlParamInit->u8Process_Status3 = (stu_ControlParam.u8DoSDRIP_ForceNOBT2020CL_Flag<<3) + (stu_ControlParam.u8DoSDRIP_ForceNOGM_Flag<<2) + (stu_ControlParam.u8DoSDRIP_ForceNOTMO_Flag<<1) + (stu_ControlParam.u8DoHDRIP_Forcebypass_Flag);
    pstApiControlParamInit->u8Process_Status3 += (stu_ControlParam.u8DoForceEnterHDRIP_Flag<<6)+(stu_ControlParam.u8DoPathFullRange_Flag<<5) +(stu_ControlParam.u8DoPreConstraints_Flag<<4);

//print out message
//output information for IP
    //MS_Cfd_Manhattan_SDRIP_Debug(pstu_Cfd_api_top);
    MS_Cfd_top_control_Debug(pstApiControlParamInit);
    MS_Cfd_inside_control_Debug(&stu_ControlParam);
#if NowHW == Maserati

    //MS_Cfd_Maserati_HDRIP_Debug(pstu_Cfd_api_top);
    //MS_Cfd_Maserati_HDRIP_Debug(&(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_TMO_Input->stu_Maserati_TMO_Param), &(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param));

    //MS_Cfd_Maserati_SDRIP_Debug(pstu_Cfd_api_top);
    //MS_Cfd_Maserati_SDRIP_Debug(&(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_DLC_Input->stu_Maserati_DLC_Param), &(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Maserati_SDRIP_Param));

#endif

    HDR_DBG_HAL_CFD(printk("\n SDR IPs:u8DoDLC_Flag = %x\n",stu_ControlParam.u8DoDLC_Flag));
    HDR_DBG_HAL_CFD(printk("\n SDR IPs:u8DoGamutMapping_Flag = %x\n",stu_ControlParam.u8DoGamutMapping_Flag));
    HDR_DBG_HAL_CFD(printk("\n SDR IPs:u8DoTMO_Flag = %x\n",stu_ControlParam.u8DoTMO_Flag));
    HDR_DBG_HAL_CFD(printk("\n SDR IPs:u8DoBT2020CLP_Flag = %x\n",stu_ControlParam.u8DoBT2020CLP_Flag));
    HDR_DBG_HAL_CFD(printk("\n" ));

    HDR_DBG_HAL_CFD(printk("\n HDR IPs:u8DoDLC_Flag = %x\n",stu_ControlParam.u8DoDLCInHDRIP_Flag));
    HDR_DBG_HAL_CFD(printk("\n HDR IPs:u8DoGamutMapping_Flag = %x\n",stu_ControlParam.u8DoGamutMappingInHDRIP_Flag));
    HDR_DBG_HAL_CFD(printk("\n HDR IPs:u8DoTMO_Flag = %x\n",stu_ControlParam.u8DoTMOInHDRIP_Flag));
    HDR_DBG_HAL_CFD(printk("\n HDR IPs:u8DoBT2020CLP_Flag = %x\n",stu_ControlParam.u8DoBT2020CLPInHDRIP_Flag));
    HDR_DBG_HAL_CFD(printk("\n" ));

    HDR_DBG_HAL_CFD(printk("\n CFD:u8DoMMIn_ForceHDMI_Flag = %x\n",stu_ControlParam.u8DoMMIn_ForceHDMI_Flag));
    HDR_DBG_HAL_CFD(printk("\n CFD:u8DoMMIn_Force709_Flag = %x\n",stu_ControlParam.u8DoMMIn_Force709_Flag));
    HDR_DBG_HAL_CFD(printk("\n CFD:u8DoHDMIIn_Force709_Flag = %x\n",stu_ControlParam.u8DoHDMIIn_Force709_Flag));
    HDR_DBG_HAL_CFD(printk("\n CFD:u8DoOtherIn_Force709_Flag = %x\n",stu_ControlParam.u8DoOtherIn_Force709_Flag));
    HDR_DBG_HAL_CFD(printk("\n CFD:u8DoOutput_Force709_Flag = %x\n",stu_ControlParam.u8DoOutput_Force709_Flag));
    HDR_DBG_HAL_CFD(printk("\n" ));

    HDR_DBG_HAL_CFD(printk("\n CFD:u8DoHDRIP_Forcebypass_Flag = %x\n",stu_ControlParam.u8DoHDRIP_Forcebypass_Flag));
    HDR_DBG_HAL_CFD(printk("\n CFD:u8DoSDRIP_ForceNOTMO_Flag = %x\n",stu_ControlParam.u8DoSDRIP_ForceNOTMO_Flag));
    HDR_DBG_HAL_CFD(printk("\n CFD:u8DoSDRIP_ForceNOGM_Flag = %x\n",stu_ControlParam.u8DoSDRIP_ForceNOGM_Flag));
    HDR_DBG_HAL_CFD(printk("\n CFD:u8DoSDRIP_ForceNOBT2020CL_Flag = %x\n",stu_ControlParam.u8DoSDRIP_ForceNOBT2020CL_Flag));
    HDR_DBG_HAL_CFD(printk("\n" ));

    HDR_DBG_HAL_CFD(printk("\n CFD:u8DoPreConstraints_Flag = %x\n",stu_ControlParam.u8DoPreConstraints_Flag));
    HDR_DBG_HAL_CFD(printk("\n CFD:u8DoPathFullRange_Flag = %x\n",stu_ControlParam.u8DoPathFullRange_Flag));
    HDR_DBG_HAL_CFD(printk("\n CFD:u8DoForceEnterHDRIP_Flag = %x\n",stu_ControlParam.u8DoForceEnterHDRIP_Flag));
    HDR_DBG_HAL_CFD(printk("\n" ));

    HDR_DBG_HAL_CFD(printk("\nu8Process_Status = %02x\n",pstApiControlParamInit->u8Process_Status));
    HDR_DBG_HAL_CFD(printk("u8Process_Status2 = %02x\n",pstApiControlParamInit->u8Process_Status2));
    HDR_DBG_HAL_CFD(printk("u8Process_Status3 = %02x\n\n",pstApiControlParamInit->u8Process_Status3));
    HDR_DBG_HAL_CFD(printk("error message = %04x\n\n",u16_check_status));
    HDR_DBG_HAL_CFD(printk("error message count = %04x\n\n",u8ERR_Happens_Flag));

//input control information
#if NowHW == Maserati
//Restore_input_bank_and_log_Maserati(&stu_ControlParam,pstu_Cfd_api_top); //move the function body to TOP
#endif
//writeback format information
    MS_Cfd_WriteBack_Control(pstu_Cfd_api_top,&stu_ControlParam);


    return u16_check_status;
}

//Due to Kano's ygain_offset issue
/*
MS_U16 MS_Cfd_KanoPatch_TMO_For_LimitIn(MS_U8 *TMO_curve, MS_U8 *TMO_curve_patch)
{
 MS_U16 status = 0;

 return status;
}
*/

//tranfer luminance(nits) to PQ code
#if RealChip
#else
MS_U16 MS_Cfd_Luminance_To_PQCode(MS_U16 u16Luminance, MS_U8 u8Function_Mode)
{
    MS_U8 u8Range = 0;
    MS_U16 u16Step = 0;
    MS_U16 u16PQ_Code = 0;
    MS_U16 u16LuminanceOffset =0;
    MS_U8 u8temp = 0;
    MS_U8 u8temp2 = 0;

    MS_U16 u16_hbound = 0;
    MS_U16 u16_lbound = 0;
    MS_U16 u16_diffh = 0;
    MS_U16 u16_diffl = 0;

//range handle
    if ((u16Luminance>=10000) && (1 == u8Function_Mode))
    {
        u16Luminance = u16Luminance/10000;
        u8Function_Mode = 0;
    }

    if (u16Luminance < 1)
    {
        u8Range = 0;
    }
    else if (u16Luminance < 10)
    {
        u8Range = 1;
        u16Step = 0;
    }
    else if (u16Luminance < 100)
    {
        u8Range = 2;
        u16Step = 5;
    }
    else if (u16Luminance < 1000)
    {
        u8Range = 3;
        u16Step = 50;
    }
    else if (u16Luminance < 10000)
    {
        u8Range = 4;
        u16Step = 500;
    }
    else
    {
        u8Range = 0;
    }


    if  (u8Range == 0)
    {
        if (u16Luminance < 1)
        {
            u16PQ_Code = 0;
        }
        else
        {
            u16PQ_Code = 1023;
        }
    }
    else if (u8Range == 1)
    {

        if (u8Function_Mode == 1)
        {
            u8temp = 0+(u16Luminance-1)*2;
        }
        else
        {
            u8temp = 72+(u16Luminance-1)*2;
        }
        u16PQ_Code = LUT_CFD_NITS2PQCODE[u8temp];

    }
    else // (u8Range > 1)
    {
        u16LuminanceOffset = u16Luminance-(u16Step*2);

        u8temp = (u16LuminanceOffset/u16Step);

        //calculate offset of LUT_CFD_NITS2PQCODE
        if (u8Function_Mode == 1)
        {
            u8temp2 = 0 + 18*(u8Range-1);
        }
        else
        {
            u8temp2 = 72 + 18*(u8Range-1);
        }

        u16_lbound = LUT_CFD_NITS2PQCODE[u8temp2+u8temp];
        u16_hbound = LUT_CFD_NITS2PQCODE[u8temp2+u8temp+1];
        u16_diffl = u16LuminanceOffset-u16Step*u8temp;
        u16_diffh = u16Step-u16_diffl;

        u16PQ_Code = (((MS_U32)u16_hbound)*u16_diffl + ((MS_U32)u16_lbound)*u16_diffh)/u16Step;
    }

    return u16PQ_Code;
}
#endif

void MS_Cfd_ForceMMInputToRec709(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam, MS_U8 u8Current_Transfer_Characteristic)
{
    if ( E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Input_DataFormat)
    {
        pstu_ControlParam->u8Input_Format = E_CFD_CFIO_RGB_BT709;
    }
    else
    {
        pstu_ControlParam->u8Input_Format = E_CFD_CFIO_YUV_BT709;
    }

//pstu_ControlParam->u8InputColorPriamries = u8Const_LUT_MMInfor_HDMIDefinition[2][0];
//pstu_ControlParam->u8InputMatrixCoefficients = u8Const_LUT_MMInfor_HDMIDefinition[2][2];
    pstu_ControlParam->u8InputColorPriamries = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    pstu_ControlParam->u8InputMatrixCoefficients = E_CFD_CFIO_MC_BT709_XVYCC709;

    //if (E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Input_HDRMode) //HDR1
    if( MS_Cfd_CheckOpenHDR(pstu_ControlParam->u8Input_HDRMode))
    {
        pstu_ControlParam->u8InputTransferCharacterstics = u8Current_Transfer_Characteristic;
    }
    else //SDR
    {
        //pstu_ControlParam->u8InputTransferCharacterstics = u8Const_LUT_MMInfor_HDMIDefinition[2][1];
        pstu_ControlParam->u8InputTransferCharacterstics = E_CFD_CFIO_TR_BT709;
    }
}

//refer .xls
///*
void MS_IN_Cfd_TMO_Control_Param_Init(STU_IN_CFD_TMO_CONTROL *pst_TMO_Control_Param )
{

    MS_U16 test = 0;

//unit
//0x1000 = 1
//3 = 12288/0x1000
//pst_TMO_Control_Param->stu_CFD_TMO_Param.u16Rolloff = 12288;

//unit
//0x1000 = 1
//0.38623 = 1582/0x1000
//pst_TMO_Control_Param->stu_CFD_TMO_Param.u16Slope = 1582; //0.38623

//source (PQ code) 0.10
//test = MS_Cfd_Luminance_To_PQCode(0,1);
//test = MS_Cfd_Luminance_To_PQCode(5,1);
//test = MS_Cfd_Luminance_To_PQCode(18,1);
//test = MS_Cfd_Luminance_To_PQCode(918,1);
//test = MS_Cfd_Luminance_To_PQCode(4918,1);
//test = MS_Cfd_Luminance_To_PQCode(0,0);
//test = MS_Cfd_Luminance_To_PQCode(5,0);
//test = MS_Cfd_Luminance_To_PQCode(18,0);
//test = MS_Cfd_Luminance_To_PQCode(918,0);
//test = MS_Cfd_Luminance_To_PQCode(4918,0);

//pst_Control_Param->stu_CFD_TMO_Param.u16Smin = 17; //0.006067 nits
//pst_Control_Param->stu_CFD_TMO_Param.u16Smed = 537;//119.98 nits
//pst_Control_Param->stu_CFD_TMO_Param.u16Smax = 920;//3977 nits
//pst_TMO_Control_Param->stu_CFD_TMO_Param.u16Smin = MS_Cfd_Luminance_To_PQCode(61,1);
//pst_TMO_Control_Param->stu_CFD_TMO_Param.u16Smed = MS_Cfd_Luminance_To_PQCode(120,0);
//pst_TMO_Control_Param->stu_CFD_TMO_Param.u16Smax = MS_Cfd_Luminance_To_PQCode(3977,0);

//target 0.10
//pst_Control_Param->stu_CFD_TMO_Param.u16Tmin = 50;//0.05 nits
//pst_Control_Param->stu_CFD_TMO_Param.u16Tmed = 450;//50.47 nits
//pst_Control_Param->stu_CFD_TMO_Param.u16Tmax = 520;//101.7331 nits
//pst_TMO_Control_Param->stu_CFD_TMO_Param.u16TgtMin = MS_Cfd_Luminance_To_PQCode(500,1);
//pst_TMO_Control_Param->stu_CFD_TMO_Param.u16Tmed = MS_Cfd_Luminance_To_PQCode(50,0);
//pst_TMO_Control_Param->stu_CFD_TMO_Param.u16TgtMax = MS_Cfd_Luminance_To_PQCode(102,0);

//source
    pst_TMO_Control_Param->u16SourceMax = 3977;
    pst_TMO_Control_Param->u16SourceMaxFlag = 0;

    pst_TMO_Control_Param->u16SourceMed = 120;
    pst_TMO_Control_Param->u16SourceMedFlag = 0;

    pst_TMO_Control_Param->u16SourceMin = 61;
    pst_TMO_Control_Param->u16SourceMinFlag = 1;

//target
    pst_TMO_Control_Param->u16TgtMax = 100;
    pst_TMO_Control_Param->u16TgtMaxFlag = 0;

    pst_TMO_Control_Param->u16TgtMed = 50;
    pst_TMO_Control_Param->u16TgtMedFlag = 0;

    pst_TMO_Control_Param->u16TgtMin = 500;
    pst_TMO_Control_Param->u16TgtMinFlag = 1;

//PQtoPQ
    pst_TMO_Control_Param->u8TMO_domain_mode = 0;

}
//*/

/*
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

  pstu_SDRIP_Input->stu_Param.u8Input_Format  = pstu_Contol_input->u8Input_Format;
  pstu_SDRIP_Input->stu_Param.u8Input_IsFullRange = pstu_Contol_input->u8Input_IsFullRange;
  pstu_SDRIP_Input->stu_Param.u8InputColorPriamries = pstu_Contol_input->u8InputColorPriamries;
  pstu_SDRIP_Input->stu_Param.u8InputTransferCharacterstics = pstu_Contol_input->u8InputTransferCharacterstics;
  pstu_SDRIP_Input->stu_Param.u8InputMatrixCoefficients = pstu_Contol_input->u8InputMatrixCoefficients;

  //With Kano's design, no gamut mapping in SDR IP
  //force gamut keep the same
  pstu_SDRIP_Input->stu_Param.u8Output_Format  = pstu_Contol_input->u8Output_Format;
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

   pstu_HDRIP_Input->stu_Param.u8Input_Format  = pstu_Contol_input->u8Input_Format;
   pstu_HDRIP_Input->stu_Param.u8Input_IsFullRange = pstu_Contol_input->u8Input_IsFullRange;
   pstu_HDRIP_Input->stu_Param.u8InputColorPriamries = pstu_Contol_input->u8InputColorPriamries;
   pstu_HDRIP_Input->stu_Param.u8InputTransferCharacterstics = pstu_Contol_input->u8InputTransferCharacterstics;
   pstu_HDRIP_Input->stu_Param.u8InputMatrixCoefficients = pstu_Contol_input->u8InputMatrixCoefficients;

   //With Kano's design, no gamut mapping in SDR IP
   pstu_HDRIP_Input->stu_Param.u8Output_Format  = pstu_Contol_input->u8Output_Format;
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
   pstu_SDRIP_Input->stu_Param.u8Input_Format  = pstu_Contol_input->u8Output_Format;
   pstu_SDRIP_Input->stu_Param.u8Input_IsFullRange = pstu_Contol_input->u8Output_IsFullRange;
   pstu_SDRIP_Input->stu_Param.u8InputColorPriamries = pstu_Contol_input->u8OutputColorPriamries;
   pstu_SDRIP_Input->stu_Param.u8InputTransferCharacterstics = pstu_Contol_input->u8OutputTransferCharacterstics;
   pstu_SDRIP_Input->stu_Param.u8InputMatrixCoefficients = pstu_Contol_input->u8OutputMatrixCoefficients;

   pstu_SDRIP_Input->stu_Param.u8Output_Format  = pstu_Contol_input->u8Output_Format;
   pstu_SDRIP_Input->stu_Param.u8Output_IsFullRange = pstu_Contol_input->u8Output_IsFullRange;
   pstu_SDRIP_Input->stu_Param.u8OutputColorPriamries = pstu_Contol_input->u8OutputColorPriamries;
   pstu_SDRIP_Input->stu_Param.u8OutputTransferCharacterstics = pstu_Contol_input->u8OutputTransferCharacterstics;
   pstu_SDRIP_Input->stu_Param.u8OutputMatrixCoefficients = pstu_Contol_input->u8OutputMatrixCoefficients;
  }
  else
  {
   pstu_SDRIP_Input->stu_Param.u8Input_Format  = pstu_Contol_input->u8Input_Format;
   pstu_SDRIP_Input->stu_Param.u8Input_IsFullRange = pstu_Contol_input->u8Input_IsFullRange;
   pstu_SDRIP_Input->stu_Param.u8InputColorPriamries = pstu_Contol_input->u8InputColorPriamries;
   pstu_SDRIP_Input->stu_Param.u8InputTransferCharacterstics = pstu_Contol_input->u8InputTransferCharacterstics;
   pstu_SDRIP_Input->stu_Param.u8InputMatrixCoefficients = pstu_Contol_input->u8InputMatrixCoefficients;

   pstu_SDRIP_Input->stu_Param.u8Output_Format  = pstu_Contol_input->u8Output_Format;
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
*/

//assign for HDMI
//from the definition of STU_CFD_MS_ALG_COLOR_FORMAT
void MS_Cfd_SetOutputColorParam_HDMISink(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_input)
{
//only YUV
    if ( E_CFD_CFIO_GT_BT601_525 == pstu_input->u8Output_GamutOrderIdx)
    {
        pstu_input->u8Output_Format = E_CFD_CFIO_YUV_BT601_525;
        pstu_input->u8OutputColorPriamries = E_CFD_CFIO_CP_BT601525_SMPTE170M;
        pstu_input->u8OutputTransferCharacterstics = E_CFD_CFIO_TR_BT601525_601625;
        pstu_input->u8OutputMatrixCoefficients = E_CFD_CFIO_MC_BT601525_SMPTE170M;
    }
    else if ( E_CFD_CFIO_GT_BT709 == pstu_input->u8Output_GamutOrderIdx)
    {

        if (E_CFD_MC_FORMAT_RGB == pstu_input->u8Output_DataFormat)
        {
            pstu_input->u8Output_Format = E_CFD_CFIO_RGB_BT709;
        }
        else
        {
            pstu_input->u8Input_Format = E_CFD_CFIO_YUV_BT709;
        }

        pstu_input->u8OutputColorPriamries = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
        pstu_input->u8OutputTransferCharacterstics = E_CFD_CFIO_TR_BT709;
        pstu_input->u8OutputMatrixCoefficients = E_CFD_CFIO_MC_BT709_XVYCC709;
    }
    else if ( E_CFD_CFIO_GT_ADOBERGB == pstu_input->u8Output_GamutOrderIdx) //Only RGB
    {
        if (E_CFD_MC_FORMAT_RGB == pstu_input->u8Output_DataFormat)
        {
            pstu_input->u8Output_Format = E_CFD_CFIO_ADOBE_RGB;
        }
        else
        {
            pstu_input->u8Input_Format = E_CFD_CFIO_ADOBE_YCC601;
        }

        pstu_input->u8OutputColorPriamries = E_CFD_CFIO_CP_ADOBERGB;
        pstu_input->u8OutputTransferCharacterstics = E_CFD_CFIO_TR_ADOBERGB;
        pstu_input->u8OutputMatrixCoefficients = E_CFD_CFIO_MC_BT601625_XVYCC601_SYCC;
    }
    else if ( E_CFD_CFIO_GT_BT2020 == pstu_input->u8Output_GamutOrderIdx)
    {

        if (E_CFD_MC_FORMAT_RGB == pstu_input->u8Output_DataFormat)
        {
            pstu_input->u8Output_Format = E_CFD_CFIO_RGB_BT2020;
        }
        else
        {
            pstu_input->u8Input_Format = E_CFD_CFIO_YUV_BT2020_NCL;
        }

        pstu_input->u8OutputColorPriamries = E_CFD_CFIO_CP_BT2020;
        pstu_input->u8OutputTransferCharacterstics = E_CFD_CFIO_TR_BT2020NCL;
        pstu_input->u8OutputMatrixCoefficients = E_CFD_CFIO_MC_BT2020NCL;
    }
}

#if 0
MS_U8 MS_Cfd_InputMM_SetColorMetry(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam,
    STU_CFDAPI_MM_PARSER *pstu_MMParam
)
{


    MS_U8 u8Check_Status = 1;
    MS_U8 u8idx = 0;

//for i = 0:8
    for (u8idx = 0; u8idx<= 8; u8idx++)
    {
        if((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0] == pstu_MMParam->u8Colour_primaries) &&
           (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][1] == pstu_MMParam->u8Transfer_Characteristics))
        {

            if (pstu_MMParam->u8Matrix_Coeffs == E_CFD_CFIO_MC_IDENTITY) //RGB
            {
                if ((0 <= u8idx)&&(6 >= u8idx))
                {
                    pstu_ControlParam->u8InputISRGB = 1;
                    pstu_ControlParam->u8InputFormat = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3];
                    u8Check_Status = 0;
                    break;
                }
            }
            else if (pstu_MMParam->u8Matrix_Coeffs == u8Const_LUT_MMInfor_HDMIDefinition[u8idx][2]) //YUV
            {
                if ((0 <= u8idx)&&(4 >= u8idx))
                {
                    pstu_ControlParam->u8InputISRGB = 0;
                    pstu_ControlParam->u8InputFormat = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4];
                    u8Check_Status = 0;
                    break;
                }
                else if ((5 <= u8idx)&&(8 >= u8idx)&&(1 == pstu_MMParam->u8Video_Full_Range_Flag))
                {
                    pstu_ControlParam->u8InputISRGB = 0;
                    pstu_ControlParam->u8InputFormat = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4];
                    u8Check_Status = 0;
                    break;
                }
            }
            else
            {
                u8Check_Status = 1;
            }
        }
        else
        {
            u8Check_Status = 1;
        }
    }

//for to 709
    if (1 == u8Check_Status)
    {
        if (E_CFD_CFIO_TR_SMPTE2084 != pstu_MMParam->u8Transfer_Characteristics) //SDR case
        {
            if (pstu_MMParam->u8Matrix_Coeffs == E_CFD_CFIO_MC_IDENTITY) //RGB
            {
                pstu_ControlParam->u8InputISRGB = 1;
                pstu_ControlParam->u8InputFormat = E_CFD_CFIO_RGB_BT709;
                pstu_ControlParam->u8InputIsFullRange = 1;
            }
            else //YUV
            {
                pstu_ControlParam->u8InputISRGB = 0;
                pstu_ControlParam->u8InputFormat = E_CFD_CFIO_YUV_BT709;
                pstu_ControlParam->u8InputIsFullRange = 0;
            }

            pstu_ControlParam->u8InputColorPriamries = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
            pstu_ControlParam->u8InputTransferCharacterstics = E_CFD_CFIO_TR_BT709;
            pstu_ControlParam->u8InputMatrixCoefficients = E_CFD_CFIO_MC_BT709_XVYCC709;
        }
        else //treat HDR as a specail case
        {
            if (pstu_MMParam->u8Matrix_Coeffs == E_CFD_CFIO_MC_IDENTITY) //RGB
            {
                pstu_ControlParam->u8InputISRGB = 1;
                pstu_ControlParam->u8InputFormat = E_CFD_CFIO_RGB_NOTSPECIFIED;
            }
            else //YUV
            {
                pstu_ControlParam->u8InputISRGB = 0;
                pstu_ControlParam->u8InputFormat = E_CFD_CFIO_YUV_NOTSPECIFIED;
            }

            pstu_ControlParam->u8InputIsFullRange = pstu_MMParam->u8Video_Full_Range_Flag;
            pstu_ControlParam->u8InputColorPriamries = pstu_MMParam->u8Colour_primaries;
            pstu_ControlParam->u8InputTransferCharacterstics = pstu_MMParam->u8Transfer_Characteristics;
            pstu_ControlParam->u8InputMatrixCoefficients = pstu_MMParam->u8Matrix_Coeffs;

        }
    }
    else
    {
        //set:
        //pstu_ControlParam->u8InputColorPriamries
        //pstu_ControlParam->u8InputTransferCharacterstics
        //pstu_ControlParam->u8InputMatrixCoefficients

        pstu_ControlParam->u8InputColorPriamries   = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0];
        pstu_ControlParam->u8InputTransferCharacterstics = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][1];
        pstu_ControlParam->u8InputMatrixCoefficients  = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][2];

    }

//0:current MM input is one case of HDMI colormetry
//1:current MM input is not one case of HDMI colormetry,
    return u8Check_Status;
}
#else

MS_U8 MS_Cfd_SearchAndMappingHDMIcase(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam,
    STU_CFDAPI_MM_PARSER *pstu_MMParam,
    STU_CFDAPI_MAIN_CONTROL *pstu_Main_Control,
    MS_U8 *u8candidateIdx)
{
    MS_U8 u8Check_Status = 1;
    MS_U8 u8idx = 0;
    MS_U8 u8Okflag = 0;

    for (u8idx = 0; u8idx<= 8; u8idx++)
    {
        //three features are the same
        //not HDR case
        if((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0] == pstu_MMParam->u8Colour_primaries)&&
           (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][2] == pstu_MMParam->u8Matrix_Coeffs)&&
           (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][1] == pstu_MMParam->u8Transfer_Characteristics))
        {
            u8Okflag = 1;
        }
        //HDR case
        else if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0] == pstu_MMParam->u8Colour_primaries)&&
                 (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][2] == pstu_MMParam->u8Matrix_Coeffs)&&
                 (MS_Cfd_CheckOpenHDR(pstu_ControlParam->u8Input_HDRMode)))
                 //((E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Input_HDRMode)))
        {
            u8Okflag = 1;
        }
        //not HDR case, RGB
        else if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0] == pstu_MMParam->u8Colour_primaries)&&
                 (E_CFD_CFIO_MC_IDENTITY == pstu_MMParam->u8Matrix_Coeffs)&&
                 (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][1] == pstu_MMParam->u8Transfer_Characteristics))
        {
            u8Okflag = 2;
        }
        //HDR case, RGB
        else if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0] == pstu_MMParam->u8Colour_primaries)&&
                 (E_CFD_CFIO_MC_IDENTITY == pstu_MMParam->u8Matrix_Coeffs)&&
                 (MS_Cfd_CheckOpenHDR(pstu_ControlParam->u8Input_HDRMode)))
                 //((E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Input_HDRMode)))
        {
            u8Okflag = 2;
        }
        else
        {
            u8Okflag = 0;
        }

        if (0 != u8Okflag)
        {
            if (pstu_ControlParam->u8Input_DataFormat == E_CFD_MC_FORMAT_RGB) //RGB
            {
                if ((0 <= u8idx)&&(6 >= u8idx))
                {
                    pstu_ControlParam->u8Input_Format = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3];
                    u8Check_Status = 0;
                    *u8candidateIdx = u8idx;
                    break;
                }
            }
            else //YUV
            {
                if ((0 <= u8idx)&&(4 >= u8idx))
                {
                    pstu_ControlParam->u8Input_Format = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4];
                    u8Check_Status = 0;
                    *u8candidateIdx = u8idx;
                    break;
                }
                else if ((5 <= u8idx)&&(8 >= u8idx))
                {
                    if(u8idx == 6)
                    {
                        pstu_ControlParam->u8Input_Format = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4];
                        u8Check_Status = 0;
                        *u8candidateIdx = u8idx;
                        break;
                    }
                    else
                    {
                        if(E_CFD_CFIO_RANGE_FULL == pstu_ControlParam->u8Input_IsFullRange)
                        {
                            pstu_ControlParam->u8Input_Format = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4];
                            u8Check_Status = 0;
                            *u8candidateIdx = u8idx;
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            u8Check_Status = 1;
        }
    }
    return u8Check_Status;
}

MS_U8 MS_Cfd_InputMM_SetColorMetry(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam,
    STU_CFDAPI_MM_PARSER *pstu_MMParam,
    STU_CFDAPI_MAIN_CONTROL *pstu_Main_Control,
    MS_U8 u8mode
)
{


    MS_U8 u8Check_Status = 0;
    MS_U8 u8idx = 0;

    u8Check_Status = MS_Cfd_SearchAndMappingHDMIcase(pstu_ControlParam,pstu_MMParam,pstu_Main_Control,&u8idx);

//pstu_ControlParam->u8Input_DataFormat = pstu_Main_Control->u8Input_DataFormat;
    //for to 709
    if (1 == u8Check_Status)
    {
        //mode 1:Match MM to HDMI format ;  if not matched , Force 709
        if (1 == u8mode)
        {
            MS_Cfd_ForceMMInputToRec709(pstu_ControlParam,pstu_MMParam->u8Transfer_Characteristics);
        }
        //mode 3:Match MM to HDMI format ;  if not matched , output directly
        else if (3 == u8mode)
        {
            //do nothing
            //handle undefined case of color primary
            if((E_CFD_CFIO_CP_RESERVED0 == pstu_MMParam->u8Colour_primaries) ||
               (E_CFD_CFIO_CP_UNSPECIFIED == pstu_MMParam->u8Colour_primaries) ||
               (E_CFD_CFIO_CP_RESERVED3 == pstu_MMParam->u8Colour_primaries) ||
               (E_CFD_CFIO_CP_RESERVED_START <= pstu_MMParam->u8Colour_primaries))
            {
                pstu_ControlParam->u8InputColorPriamries = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
                u8Check_Status += 16;
            }
            else
            {
                pstu_ControlParam->u8InputColorPriamries = pstu_MMParam->u8Colour_primaries;
            }

            //handle undefined case of Matrix coeff
            if((E_CFD_CFIO_MC_UNSPECIFIED == pstu_MMParam->u8Matrix_Coeffs) ||
               (E_CFD_CFIO_MC_RESERVED == pstu_MMParam->u8Matrix_Coeffs) ||
               (E_CFD_CFIO_MC_RESERVED_START <= pstu_MMParam->u8Matrix_Coeffs))
            {
                pstu_ControlParam->u8InputMatrixCoefficients = E_CFD_CFIO_MC_BT709_XVYCC709;
                u8Check_Status += 16;
            }
            else
            {
                pstu_ControlParam->u8InputMatrixCoefficients = pstu_MMParam->u8Matrix_Coeffs;
            }

            //handle undefined case of transfer characteristic
            if((E_CFD_CFIO_TR_RESERVED0 == pstu_MMParam->u8Transfer_Characteristics) ||
               (E_CFD_CFIO_TR_UNSPECIFIED == pstu_MMParam->u8Transfer_Characteristics) ||
               (E_CFD_CFIO_TR_RESERVED3 == pstu_MMParam->u8Transfer_Characteristics) ||
               (E_CFD_CFIO_TR_RESERVED_START <= pstu_MMParam->u8Transfer_Characteristics))
            {
                pstu_ControlParam->u8InputTransferCharacterstics = E_CFD_CFIO_TR_BT709;
                u8Check_Status += 16;
            }
            else
            {
                pstu_ControlParam->u8InputTransferCharacterstics = pstu_MMParam->u8Transfer_Characteristics;
            }

            MS_Cfd_SearchAndMappingHDMIcase_OnlyRGB(pstu_ControlParam,pstu_MMParam,pstu_Main_Control,&u8idx);
        }

#if 0
        if (E_CFIO_MODE_SDR == pstu_ControlParam->u8Input_HDRMode) //SDR case
        {
            pstu_ControlParam->u8InputColorPriamries = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
            pstu_ControlParam->u8InputTransferCharacterstics = E_CFD_CFIO_TR_BT709;
            pstu_ControlParam->u8InputMatrixCoefficients = E_CFD_CFIO_MC_BT709_XVYCC709;
        }
        else //treat HDR as a specail case
        {

            if(E_CFD_CFIO_TR_RESERVED0 == pstu_MMParam->u8Transfer_Characteristics ||E_CFD_CFIO_TR_UNSPECIFIED == pstu_MMParam->u8Transfer_Characteristics
               ||E_CFD_CFIO_TR_RESERVED3 == pstu_MMParam->u8Transfer_Characteristics ||E_CFD_CFIO_TR_UNDEFINED == pstu_MMParam->u8Transfer_Characteristics)
            {
                pstu_ControlParam->u8InputTransferCharacterstics = E_CFD_CFIO_TR_BT709;
            }
            else
            {
                pstu_ControlParam->u8InputTransferCharacterstics = pstu_MMParam->u8Transfer_Characteristics;
            }


            //pstu_ControlParam->u8Input_IsFullRange = pstu_MMParam->u8Video_Full_Range_Flag;

        }
#endif

    }
    else
    {
        //set:
        //pstu_ControlParam->u8Input_Format has set in MS_Cfd_SearchAndMappingHDMIcase
        //pstu_ControlParam->u8InputColorPriamries
        //pstu_ControlParam->u8InputTransferCharacterstics
        //pstu_ControlParam->u8InputMatrixCoefficients

        //if(E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Input_HDRMode)
        if (MS_Cfd_CheckOpenHDR(pstu_ControlParam->u8Input_HDRMode))
        {
            pstu_ControlParam->u8InputTransferCharacterstics = pstu_MMParam->u8Transfer_Characteristics;
        }
        else if (E_CFIO_MODE_SDR == pstu_ControlParam->u8Input_HDRMode)
        {
            pstu_ControlParam->u8InputTransferCharacterstics = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][1];
        }

        pstu_ControlParam->u8InputColorPriamries   = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0];
        pstu_ControlParam->u8InputMatrixCoefficients  = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][2];
        //pstu_ControlParam->u8Input_IsFullRange = pstu_MMParam->u8Video_Full_Range_Flag;
    }

//0:current MM input is one case of HDMI colormetry
//1:current MM input is not one case of HDMI colormetry,
    return u8Check_Status;
}
#endif

//set output tr from input_format
MS_U8 MS_Cfd_InputFormat_SetOutTR(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam)
{
    MS_U8 u8Check_Status = 0;
    MS_U8 u8idx = 0;
    MS_U8 u8temp = 0;

    if ((E_CFD_CFIO_RGB_NOTSPECIFIED == pstu_ControlParam->u8Temp_Format[1])||
        (E_CFD_CFIO_YUV_NOTSPECIFIED == pstu_ControlParam->u8Temp_Format[1])||
        (E_CFD_CFIO_TR_RESERVED_START <= pstu_ControlParam->u8Temp_Format[1]))
    {
        return E_CFD_CFIO_TR_BT709;
    }

    for (u8idx =0; u8idx<=9 ; u8idx++)
    {
        if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3] == pstu_ControlParam->u8Temp_Format[1])||
            (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4] == pstu_ControlParam->u8Temp_Format[1]))
        {
            break;
        }
    }

    if (u8idx > 8)
    {
        u8idx = 8;
    }

    return u8Const_LUT_MMInfor_HDMIDefinition[u8idx][1];

}




MS_U8 MS_Cfd_InputFormat_SetColorMetry(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam,
    MS_U8 U8FormatArray_Idx
)
{
    MS_U8 u8Check_Status = 0;
    MS_U8 u8idx = 0;
    MS_U8 u8temp = 0;

//set

//stu_ControlParam.u8InputFormat
//stu_ControlParam.u8Input_DataFormat
//stu_ControlParam.u8InputIsFullRange
//stu_ControlParam.u8InputColorPriamries
//stu_ControlParam.u8InputTransferCharacterstics
//stu_ControlParam.u8InputMatrixCoefficients

//force to a specific color for reserved case
    if ((E_CFD_CFIO_RGB_NOTSPECIFIED == pstu_ControlParam->u8Temp_Format[U8FormatArray_Idx])||
        (E_CFD_CFIO_YUV_NOTSPECIFIED == pstu_ControlParam->u8Temp_Format[U8FormatArray_Idx])||
        (E_CFD_CFIO_RESERVED_START <= pstu_ControlParam->u8Temp_Format[U8FormatArray_Idx]))
    {
        u8Check_Status = 1;

        if (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Input_DataFormat) //RGB
        {
            pstu_ControlParam->u8Temp_Format[U8FormatArray_Idx] = E_CFD_CFIO_SRGB;
        }
        else
        {
            pstu_ControlParam->u8Temp_Format[U8FormatArray_Idx] = E_CFD_CFIO_YUV_BT709;
        }

        HDR_DBG_HAL_CFD(printk("warning forceto709 happens in [ %s  , %d]\n", __FUNCTION__,__LINE__));
    }

//
//assign
//stu_ControlParam.u8InputColorPriamries
//stu_ControlParam.u8InputTransferCharacterstics
//stu_ControlParam.u8InputMatrixCoefficients

    for (u8idx =0; u8idx<=9 ; u8idx++)
    {
        if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3] == pstu_ControlParam->u8Temp_Format[U8FormatArray_Idx])||
            (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4] == pstu_ControlParam->u8Temp_Format[U8FormatArray_Idx]))
        {
            break;
        }
    }

    if (u8idx > 8)
    {
        u8idx = 8;
    }

    pstu_ControlParam->u8TempColorPriamries[U8FormatArray_Idx] = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0];
    pstu_ControlParam->u8TempTransferCharacterstics[U8FormatArray_Idx] = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][1];
    pstu_ControlParam->u8TempMatrixCoefficients[U8FormatArray_Idx] = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][2];

#if 0
    if (E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Temp_HDRMode[U8FormatArray_Idx])
    {
        pstu_ControlParam->u8TempTransferCharacterstics[U8FormatArray_Idx] = E_CFD_CFIO_TR_SMPTE2084;
    }
    //else if (E_CFIO_MODE_HDR1 == pstu_ControlParam->u8Input_HDRMode)
    else if (E_CFIO_MODE_HDR1 == pstu_ControlParam->u8Temp_HDRMode[U8FormatArray_Idx])
    {
        pstu_ControlParam->u8TempTransferCharacterstics[U8FormatArray_Idx] = E_CFD_CFIO_TR_UNSPECIFIED;
    }
#else
    pstu_ControlParam->u8TempTransferCharacterstics[U8FormatArray_Idx] = MS_Cfd_GetTR_FromHDRMode(pstu_ControlParam->u8Temp_HDRMode[U8FormatArray_Idx],pstu_ControlParam->u8TempTransferCharacterstics[U8FormatArray_Idx]);

#endif

//0:current HDMI input is one case of HDMI colormetry
//1:current HDMI input is not one case of HDMI colormetry,
    return u8Check_Status;
}

// Input max 0xFF00; Output max 0xFFFF0000
// input 0xFF00 : 1
// output 0xFFF00000 : 10000 nits

#if 0
MS_U32 MDrv_HDR_PQ(MS_U16 u16In)
{
    MS_U16 u16Shift, u16Idx, u16Lsb, u16Tmp, u16Exp;
    MS_U32 u32Lut0_out, u32Lut1_out, u32Result;

    const MS_U16 u16PQCurveExpLut[705] =
    {
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
        0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
        0x1, 0x1, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
        0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x3, 0x3, 0x3, 0x3,
        0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
        0x3, 0x3, 0x3, 0x3, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4,
        0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4,
        0x4, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5,
        0x5, 0x5, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6,
        0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7,
        0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x8, 0x8,
        0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x9, 0x9, 0x9, 0x9,
        0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0xA, 0xA, 0xA, 0xA, 0xA,
        0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xB, 0xB, 0xB, 0xB, 0xB,
        0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xB, 0xC, 0xC, 0xC, 0xC,
        0xC, 0xC, 0xC, 0xC, 0xC, 0xC, 0xC, 0xD, 0xD, 0xD, 0xD, 0xD, 0xD, 0xD, 0xD, 0xD,
        0xE, 0xE, 0xE, 0xE, 0xE, 0xE, 0xE, 0xE, 0xE, 0xE, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
        0xF, 0xF, 0xF, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11,
        0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
        0x13
    };

    const MS_U16 u16PQCurveMantLut[705] =
    {
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
        0x2, 0x2, 0x2, 0x2, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x4, 0x4, 0x5, 0x5, 0x5,
        0x5, 0x6, 0x6, 0x6, 0x7, 0x7, 0x7, 0x8, 0x8, 0x8, 0x9, 0x9, 0x9, 0xA, 0xA, 0xA,
        0xB, 0xB, 0xB, 0xC, 0xC, 0xD, 0xD, 0xD, 0xE, 0xE, 0xF, 0xF, 0x10, 0x10, 0x11, 0x11,
        0x11, 0x12, 0x12, 0x13, 0x13, 0x14, 0x14, 0x15, 0x15, 0x16, 0x16, 0x17, 0x17, 0x18, 0x18, 0x19,
        0x19, 0x1A, 0x1B, 0x1B, 0x1C, 0x1C, 0x1D, 0x1D, 0x1E, 0x1F, 0x1F, 0x20, 0x20, 0x21, 0x21, 0x22,
        0x23, 0x23, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x28, 0x28, 0x29, 0x2A, 0x2A, 0x2B, 0x2C, 0x2C,
        0x2D, 0x2E, 0x2E, 0x2F, 0x30, 0x31, 0x31, 0x32, 0x33, 0x33, 0x34, 0x35, 0x36, 0x36, 0x37, 0x38,
        0x39, 0x3A, 0x3C, 0x3D, 0x3F, 0x40, 0x42, 0x44, 0x45, 0x47, 0x48, 0x4A, 0x4C, 0x4E, 0x4F, 0x51,
        0x53, 0x55, 0x56, 0x58, 0x5A, 0x5C, 0x5E, 0x60, 0x62, 0x63, 0x65, 0x67, 0x69, 0x6B, 0x6D, 0x6F,
        0x71, 0x73, 0x75, 0x77, 0x7A, 0x7C, 0x7E, 0x80, 0x82, 0x84, 0x86, 0x89, 0x8B, 0x8D, 0x8F, 0x91,
        0x94, 0x96, 0x98, 0x9B, 0x9D, 0x9F, 0xA2, 0xA4, 0xA7, 0xA9, 0xAB, 0xAE, 0xB0, 0xB3, 0xB5, 0xB8,
        0xBA, 0xBF, 0xC5, 0xCA, 0xCF, 0xD4, 0xDA, 0xDF, 0xE5, 0xEA, 0xF0, 0xF6, 0xFB, 0x101, 0x107, 0x10D,
        0x113, 0x119, 0x11F, 0x126, 0x12C, 0x132, 0x139, 0x13F, 0x146, 0x14C, 0x153, 0x159, 0x160, 0x167, 0x16E, 0x175,
        0x17C, 0x183, 0x18A, 0x191, 0x198, 0x1A0, 0x1A7, 0x1AF, 0x1B6, 0x1BE, 0x1C5, 0x1CD, 0x1D5, 0x1DC, 0x1E4, 0x1EC,
        0x1F4, 0x1FC, 0x204, 0x20D, 0x215, 0x21D, 0x225, 0x22E, 0x236, 0x23F, 0x247, 0x250, 0x259, 0x262, 0x26A, 0x273,
        0x27C, 0x28E, 0x2A1, 0x2B4, 0x2C6, 0x2DA, 0x2ED, 0x301, 0x314, 0x329, 0x33D, 0x352, 0x366, 0x37C, 0x391, 0x3A7,
        0x3BD, 0x3D3, 0x3E9, 0x400, 0x417, 0x42E, 0x445, 0x45D, 0x475, 0x48D, 0x4A6, 0x4BF, 0x4D8, 0x4F1, 0x50A, 0x524,
        0x53E, 0x559, 0x573, 0x58E, 0x5A9, 0x5C4, 0x5E0, 0x5FC, 0x618, 0x635, 0x651, 0x66E, 0x68B, 0x6A9, 0x6C7, 0x6E5,
        0x703, 0x722, 0x741, 0x760, 0x77F, 0x79F, 0x7BF, 0x7DF, 0x800, 0x820, 0x841, 0x863, 0x884, 0x8A6, 0x8C8, 0x8EB,
        0x90E, 0x954, 0x99B, 0x9E4, 0xA2E, 0xA79, 0xAC5, 0xB12, 0xB60, 0xBB0, 0xC00, 0xC52, 0xCA5, 0xCF9, 0xD4F, 0xDA6,
        0xDFD, 0xE56, 0xEB1, 0xF0C, 0xF69, 0xFC7, 0x1026, 0x1086, 0x10E8, 0x114B, 0x11AF, 0x1214, 0x127B, 0x12E3, 0x134C, 0x13B7,
        0x1423, 0x1490, 0x14FE, 0x156E, 0x15DF, 0x1651, 0x16C5, 0x173A, 0x17B1, 0x1828, 0x18A1, 0x191C, 0x1998, 0x1A15, 0x1A93, 0x1B13,
        0x1B95, 0x1C17, 0x1C9B, 0x1D21, 0x1DA8, 0x1E30, 0x1EBA, 0x1F45, 0x1FD2, 0x2060, 0x20EF, 0x2180, 0x2213, 0x22A7, 0x233C, 0x23D3,
        0x246B, 0x25A1, 0x26DC, 0x281D, 0x2965, 0x2AB2, 0x2C06, 0x2D60, 0x2EC1, 0x3028, 0x3195, 0x3309, 0x3483, 0x3604, 0x378C, 0x391A,
        0x3AB0, 0x3C4C, 0x3DEF, 0x3F99, 0x20A5, 0x2181, 0x2260, 0x2344, 0x242A, 0x2515, 0x2603, 0x26F5, 0x27EA, 0x28E3, 0x29E1, 0x2AE1,
        0x2BE6, 0x2CEF, 0x2DFB, 0x2F0C, 0x3020, 0x3139, 0x3256, 0x3376, 0x349B, 0x35C4, 0x36F1, 0x3822, 0x3958, 0x3A91, 0x3BCF, 0x3D12,
        0x3E58, 0x3FA4, 0x207A, 0x2124, 0x21D0, 0x227F, 0x232F, 0x23E3, 0x2498, 0x2550, 0x260A, 0x26C6, 0x2785, 0x2847, 0x290A, 0x29D0,
        0x2A99, 0x2C32, 0x2DD4, 0x2F81, 0x3139, 0x32FA, 0x34C7, 0x369E, 0x3880, 0x3A6D, 0x3C66, 0x3E6A, 0x203D, 0x214A, 0x225E, 0x2378,
        0x2497, 0x25BD, 0x26EA, 0x281C, 0x2955, 0x2A95, 0x2BDB, 0x2D28, 0x2E7C, 0x2FD7, 0x3138, 0x32A1, 0x3411, 0x3588, 0x3707, 0x388D,
        0x3A1B, 0x3BB0, 0x3D4D, 0x3EF2, 0x2050, 0x212A, 0x2209, 0x22EC, 0x23D3, 0x24BE, 0x25AD, 0x26A1, 0x2799, 0x2896, 0x2997, 0x2A9D,
        0x2BA8, 0x2CB7, 0x2DCB, 0x2EE3, 0x3001, 0x3123, 0x324B, 0x3377, 0x34A9, 0x35E0, 0x371C, 0x385D, 0x39A4, 0x3AF0, 0x3C42, 0x3D99,
        0x3EF6, 0x20E0, 0x2251, 0x23CE, 0x2558, 0x26ED, 0x2890, 0x2A3F, 0x2BFC, 0x2DC7, 0x2F9F, 0x3186, 0x337C, 0x3581, 0x3795, 0x39B8,
        0x3BEC, 0x3E30, 0x2043, 0x2176, 0x22B2, 0x23F6, 0x2544, 0x269C, 0x27FC, 0x2967, 0x2ADB, 0x2C5A, 0x2DE3, 0x2F77, 0x3116, 0x32C0,
        0x3475, 0x3635, 0x3802, 0x39DA, 0x3BBF, 0x3DB1, 0x3FAF, 0x20DD, 0x21EA, 0x22FD, 0x2417, 0x2539, 0x2661, 0x2791, 0x28C9, 0x2A08,
        0x2B4F, 0x2C9F, 0x2DF6, 0x2F56, 0x30BE, 0x322F, 0x33A9, 0x352C, 0x36B9, 0x384E, 0x39EE, 0x3B97, 0x3D4A, 0x3F08, 0x2068, 0x2151,
        0x2240, 0x242E, 0x2633, 0x284F, 0x2A85, 0x2CD4, 0x2F3F, 0x31C5, 0x3468, 0x3729, 0x3A09, 0x3D09, 0x2016, 0x21B9, 0x236D, 0x2535,
        0x2710, 0x2900, 0x2B04, 0x2D1E, 0x2F4F, 0x3198, 0x33F9, 0x3673, 0x3908, 0x3BB7, 0x3E83, 0x20B7, 0x223A, 0x23CE, 0x2572, 0x2727,
        0x28ED, 0x2AC6, 0x2CB1, 0x2EB0, 0x30C4, 0x32EC, 0x352A, 0x377F, 0x39EB, 0x3C6F, 0x3F0D, 0x20E2, 0x224C, 0x23C3, 0x2548, 0x26DD,
        0x2881, 0x2A36, 0x2BFB, 0x2DD1, 0x2FB9, 0x31B4, 0x33C2, 0x35E3, 0x381A, 0x3A66, 0x3CC8, 0x3F40, 0x20E9, 0x223D, 0x239E, 0x250D,
        0x2689, 0x29AC, 0x2D0C, 0x30AC, 0x3493, 0x38C4, 0x3D46, 0x210F, 0x23A9, 0x2675, 0x2975, 0x2CAF, 0x3026, 0x33DE, 0x37DD, 0x3C27,
        0x2061, 0x22DA, 0x2582, 0x285B, 0x2B6A, 0x2EB3, 0x323A, 0x3603, 0x3A14, 0x3E71, 0x2190, 0x2414, 0x26C7, 0x29AE, 0x2CCB, 0x3023,
        0x33BA, 0x3796, 0x3BBB, 0x2017, 0x227C, 0x250E, 0x27D1, 0x2AC8, 0x2DF9, 0x3167, 0x3518, 0x390F, 0x3D53, 0x20F5, 0x236D, 0x2616,
        0x28F2, 0x2C06, 0x2F57, 0x32EA, 0x36C2, 0x3AE8, 0x3F60, 0x2219, 0x24B2, 0x277F, 0x2A85, 0x2DC8, 0x314D, 0x351B, 0x3936, 0x3DA6,
        0x2129
    };

    if (u16In == 0)
        return 0;
    if (u16In > 0xFF00)
        u16In = 0xFF00;

    u16Tmp = u16In;
    for (u16Shift = 0; u16Tmp > 127; u16Shift++)
        u16Tmp >>= 1;
    u16Idx = (u16In >> u16Shift) + 64*u16Shift;
    u16Lsb = u16In & ((0x1 << u16Shift) - 1);
    u32Lut0_out = u16PQCurveMantLut[u16Idx];
    u32Lut1_out = u16PQCurveMantLut[u16Idx+1];
    u16Exp = u16PQCurveExpLut[u16Idx];
    if (u16Exp != u16PQCurveExpLut[u16Idx+1])
        u32Lut1_out <<= 1;
    if (u16Shift > u16PQCurveExpLut[u16Idx])
        u32Result = (u32Lut0_out << u16Exp) + ((u32Lut1_out - u32Lut0_out) * u16Lsb >> (u16Shift - u16Exp));
    else
        u32Result = (u32Lut0_out << u16Exp) + ((u32Lut1_out - u32Lut0_out) * u16Lsb << (u16Exp - u16Shift));
    //printk("%x -> %x; %x %x %x %x %x %x\n", u16In, u32Result, u16Idx, u16Lsb, u32Lut0_out, u32Lut1_out, u16Shift, u16Exp);
    return u32Result;
}
#endif

#if 0
MS_U16 MS_Cfd_Input_Analog_SetConfigures(
    STU_CFDAPI_MAIN_CONTROL *pstApiControlParamInit,
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam)
{
//assign values with a pre-defined table
    //find u8InputFormat,u8InputIsFullRange,u8InputGamutIdx from outside information
    //u8InputColorPriamries,u8InputTransferCharacterstics,u8InputMatrixCoefficients

    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;

    MS_U8 u8_temp_Input_AnalogIdx;

    if (pstApiControlParamInit->u8Input_AnalogIdx >= CFD_Current_MAX_AnalogIdx)
    {
        u8_temp_Input_AnalogIdx = E_CFD_INPUT_ANALOG_RF_NTSC_44;

        u16_check_status = E_CFD_MC_ERR_INPUT_ANALOGIDX;

        HDR_DBG_HAL_CFD(printk("\n  CFD MainControl current u8Input_AnalogIdx is reserved \n"));
        HDR_DBG_HAL_CFD(printk("\n  force MainControl u8Input_AnalogIdx to E_CFD_INPUT_ANALOG_RF_NTSC_44\n"));
        HDR_DBG_HAL_CFD(printk("Error code = %04x !!!, error is in [ %s  , %d]\n",u16_check_status, __FUNCTION__,__LINE__));
    }
    else
    {
        u8_temp_Input_AnalogIdx = pstApiControlParamInit->u8Input_AnalogIdx;
    }

    pstu_ControlParam->u8Input_Format = LUTU8_CFD_ANALOGIN_COLORFORMAT[u8_temp_Input_AnalogIdx];
    pstu_ControlParam->u8Input_HDRMode = E_CFIO_MODE_SDR;

    //RGB or not RGB
    if (( E_CFD_INPUT_ANALOG_SCART_RGB_NTSC == u8_temp_Input_AnalogIdx )||
        ( E_CFD_INPUT_ANALOG_SCART_RGB_PAL == u8_temp_Input_AnalogIdx))
    {
        pstu_ControlParam->u8Input_DataFormat = E_CFD_MC_FORMAT_RGB;
    }
    else if (( E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hup_Vup <= u8_temp_Input_AnalogIdx )&&
             ( E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hno_Vno_4K >= u8_temp_Input_AnalogIdx))
    {
        pstu_ControlParam->u8Input_DataFormat = E_CFD_MC_FORMAT_RGB;
    }
    else
    {
        pstu_ControlParam->u8Input_DataFormat = E_CFD_MC_FORMAT_YUV444;
    }

    //stu_input.u8InputIsFullRange
    if (( E_CFD_INPUT_ANALOG_SCART_RGB_NTSC == u8_temp_Input_AnalogIdx )||
        ( E_CFD_INPUT_ANALOG_SCART_RGB_PAL == u8_temp_Input_AnalogIdx))
    {
        pstu_ControlParam->u8Input_IsFullRange = E_CFD_CFIO_RANGE_FULL;
    }
    else if (( E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hup_Vup <= u8_temp_Input_AnalogIdx )&&
             ( E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hno_Vno_4K >= u8_temp_Input_AnalogIdx))
    {
        pstu_ControlParam->u8Input_IsFullRange = E_CFD_CFIO_RANGE_FULL;
    }
    else
    {
        pstu_ControlParam->u8Input_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
    }



    //pstu_ControlParam->u8Input_IsRGBBypass
    //this paramter is controlled by user
    /*
    if (( E_CFD_INPUT_ANALOG_SCART_RGB_NTSC == pstApiControlParamInit->u8Input_AnalogIdx )||
     ( E_CFD_INPUT_ANALOG_SCART_RGB_PAL == pstApiControlParamInit->u8Input_AnalogIdx))
    {
     pstu_ControlParam->u8Input_IsRGBBypass = 1;
    }
    else
    {
     pstu_ControlParam->u8Input_IsRGBBypass = 0;
    }*/

    //stu_input.u8InputColorPriamries
    if ( E_CFD_INPUT_ANALOG_SCART_RGB_NTSC == u8_temp_Input_AnalogIdx )
    {
        pstu_ControlParam->u8InputColorPriamries = E_CFD_CFIO_CP_BT601525_SMPTE170M;
    }
    else if ( E_CFD_INPUT_ANALOG_SCART_RGB_PAL == u8_temp_Input_AnalogIdx)
    {
        pstu_ControlParam->u8InputColorPriamries = E_CFD_CFIO_CP_BT601625;
    }
    else if (( E_CFD_INPUT_ANALOG_YPBPR_1080I_50HZ == u8_temp_Input_AnalogIdx)||
             ( E_CFD_INPUT_ANALOG_YPBPR_1080I_60HZ == u8_temp_Input_AnalogIdx)||
             ( E_CFD_INPUT_ANALOG_YPBPR_1080P_24HZ == u8_temp_Input_AnalogIdx)||
             ( E_CFD_INPUT_ANALOG_YPBPR_1080P_50HZ == u8_temp_Input_AnalogIdx)||
             ( E_CFD_INPUT_ANALOG_YPBPR_1080P_60HZ == u8_temp_Input_AnalogIdx))
    {
        pstu_ControlParam->u8InputColorPriamries = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    }
    else if (( E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hup_Vup <= u8_temp_Input_AnalogIdx )&&
             ( E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hno_Vno_4K >= u8_temp_Input_AnalogIdx))
    {
        pstu_ControlParam->u8InputColorPriamries = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    }
    else
    {
        pstu_ControlParam->u8InputColorPriamries = LUTU8_CFD_ANALOGIN_MC[u8_temp_Input_AnalogIdx];
    }

    //stu_input.u8InputTransferCharacterstics
    if (( E_CFD_INPUT_ANALOG_YPBPR_1080I_50HZ == u8_temp_Input_AnalogIdx)||
        ( E_CFD_INPUT_ANALOG_YPBPR_1080I_60HZ == u8_temp_Input_AnalogIdx)||
        ( E_CFD_INPUT_ANALOG_YPBPR_1080P_24HZ == u8_temp_Input_AnalogIdx)||
        ( E_CFD_INPUT_ANALOG_YPBPR_1080P_50HZ == u8_temp_Input_AnalogIdx)||
        ( E_CFD_INPUT_ANALOG_YPBPR_1080P_60HZ == u8_temp_Input_AnalogIdx))
    {
        pstu_ControlParam->u8InputTransferCharacterstics = E_CFD_CFIO_TR_BT709;
    }
    else if (( E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hup_Vup <= u8_temp_Input_AnalogIdx )&&
             ( E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hno_Vno_4K >= u8_temp_Input_AnalogIdx))
    {
        pstu_ControlParam->u8InputTransferCharacterstics = E_CFD_CFIO_TR_SRGB_SYCC;
    }
    else
    {
        pstu_ControlParam->u8InputTransferCharacterstics = E_CFD_CFIO_TR_BT601525_601625;
    }

    if (( E_CFD_INPUT_ANALOG_YPBPR_1080I_50HZ == u8_temp_Input_AnalogIdx)||
        ( E_CFD_INPUT_ANALOG_YPBPR_1080I_60HZ == u8_temp_Input_AnalogIdx)||
        ( E_CFD_INPUT_ANALOG_YPBPR_1080P_24HZ == u8_temp_Input_AnalogIdx)||
        ( E_CFD_INPUT_ANALOG_YPBPR_1080P_50HZ == u8_temp_Input_AnalogIdx)||
        ( E_CFD_INPUT_ANALOG_YPBPR_1080P_60HZ == u8_temp_Input_AnalogIdx))
    {
        pstu_ControlParam->u8InputMatrixCoefficients = 1;
    }
    else
    {
        pstu_ControlParam->u8InputMatrixCoefficients = LUTU8_CFD_ANALOGIN_MC[u8_temp_Input_AnalogIdx];
    }

    //input gamut index
#if 1
    if ((E_CFD_CFIO_XVYCC_601 == pstu_ControlParam->u8Input_Format) ||
        (E_CFD_CFIO_XVYCC_709 == pstu_ControlParam->u8Input_Format) ||
        (E_CFD_CFIO_SYCC601 == pstu_ControlParam->u8Input_Format))
    {
        pstu_ControlParam->u8Input_GamutOrderIdx = E_CFD_CFIO_GT_XVYCC;
    }
    else
    {
        pstu_ControlParam->u8Input_GamutOrderIdx = u8_gamut_idx_order[pstu_ControlParam->u8InputColorPriamries];
    }

    //not assign
#endif
    return u16_check_status;
}
#endif

MS_U16 MS_Cfd_CheckModes(MS_U8 *temp, MS_U8 mode_upbound)
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

MS_U8 MS_Cfd_OnlyEnable_Decision_OFF()
{
    return 0;
}


MS_U8 MS_Cfd_OnlyEnable_Decision_ON()
{
    return 1;
}

//#if 0

//#endif

void MS_Cfd_OSD_Control(STU_CFDAPI_OSD_CONTROL *pstu_OSD_Param,MS_U8 u8MainSubMode)
{
    if(1 == pstu_OSD_Param->u8OSD_UI_En)
    {
#if 1
        if(E_CFD_OSD_CONTROL_ONLY == pstu_OSD_Param->u8OSD_UI_Mode)//E_CFD_OSD_MODE
        {
            AdjustHueSatContrast(u8MainSubMode,pstu_OSD_Param->u16Hue,pstu_OSD_Param->u16Saturation,pstu_OSD_Param->u16Contrast);
            return;
        }
        else if(E_CFD_OSD_SOURCE_CHANGE == pstu_OSD_Param->u8OSD_UI_Mode)
        {
            AdjustHueSatContrast(u8MainSubMode,pstu_OSD_Param->u16Hue,pstu_OSD_Param->u16Saturation,pstu_OSD_Param->u16Contrast);
        }
#else
        AdjustHueSatContrast(u8MainSubMode,pstu_OSD_Param->u16Hue,pstu_OSD_Param->u16Saturation,pstu_OSD_Param->u16Contrast);
#endif
    }
    else
    {
        AdjustHueSatContrast(u8MainSubMode,50,128,128);//Bypass setting
    }
}

