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
#include "color_format_input.h"
#include "color_format_driver.h"
#include "color_format_ip.h"
#include "mhal_dlc.h"
#include "mhal_xc.h"

#if (1 == CFD_debug)
#else
#define printf
#endif

extern StuDlc_HDRinit g_HDRinitParameters;
extern StuDlc_FinetuneParamaters g_DlcParameters;
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

#define  HDR_DBG_HAL_CFD(x)  x
#define  HDR_DBG_HAL_CFD1(x)  //x


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
    5,5,5,5,5,5,5,5,      //56-63
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
    5,5,5,5,5,5,5,5,      //56-63
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

#if 0
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
#endif

MS_U32 LUT_CFD_CV2[256] =
{
    0x00633864, 0x00633864, 0x0018ce19, 0x000b0644, 0x00063386, 0x0003f804, 0x0002c191, 0x00020660, 0x00018ce2, 0x00013996, 0x0000fe01, 0x0000d1ec, 0x0000b064, 0x0000964c, 0x00008198, 0x000070e4,
    0x00006338, 0x000057e4, 0x00004e65, 0x0000465c, 0x00003f80, 0x00003999, 0x0000347b, 0x00003004, 0x00002c19, 0x000028a4, 0x00002593, 0x000022d8, 0x00002066, 0x00001e34, 0x00001c39, 0x00001a6e,
    0x000018ce, 0x00001753, 0x000015f9, 0x000014bc, 0x00001399, 0x0000128e, 0x00001197, 0x000010b3, 0x00000fe0, 0x00000f1c, 0x00000e66, 0x00000dbd, 0x00000d1f, 0x00000c8b, 0x00000c01, 0x00000b80,
    0x00000b06, 0x00000a94, 0x00000a29, 0x000009c4, 0x00000965, 0x0000090b, 0x000008b6, 0x00000866, 0x0000081a, 0x000007d1, 0x0000078d, 0x0000074c, 0x0000070e, 0x000006d4, 0x0000069c, 0x00000666,
    0x00000634, 0x00000603, 0x000005d5, 0x000005a9, 0x0000057e, 0x00000556, 0x0000052f, 0x0000050a, 0x000004e6, 0x000004c4, 0x000004a3, 0x00000484, 0x00000466, 0x00000449, 0x0000042d, 0x00000412,
    0x000003f8, 0x000003df, 0x000003c7, 0x000003b0, 0x0000039a, 0x00000384, 0x0000036f, 0x0000035b, 0x00000348, 0x00000335, 0x00000323, 0x00000311, 0x00000300, 0x000002f0, 0x000002e0, 0x000002d0,
    0x000002c2, 0x000002b3, 0x000002a5, 0x00000297, 0x0000028a, 0x0000027d, 0x00000271, 0x00000265, 0x00000259, 0x0000024e, 0x00000243, 0x00000238, 0x0000022d, 0x00000223, 0x00000219, 0x00000210,
    0x00000206, 0x000001fd, 0x000001f4, 0x000001ec, 0x000001e3, 0x000001db, 0x000001d3, 0x000001cb, 0x000001c4, 0x000001bc, 0x000001b5, 0x000001ae, 0x000001a7, 0x000001a0, 0x0000019a, 0x00000193,
    0x0000018d, 0x00000187, 0x00000181, 0x0000017b, 0x00000175, 0x00000170, 0x0000016a, 0x00000165, 0x00000160, 0x0000015a, 0x00000155, 0x00000151, 0x0000014c, 0x00000147, 0x00000142, 0x0000013e,
    0x0000013a, 0x00000135, 0x00000131, 0x0000012d, 0x00000129, 0x00000125, 0x00000121, 0x0000011d, 0x00000119, 0x00000116, 0x00000112, 0x0000010f, 0x0000010b, 0x00000108, 0x00000104, 0x00000101,
    0x000000fe, 0x000000fb, 0x000000f8, 0x000000f5, 0x000000f2, 0x000000ef, 0x000000ec, 0x000000e9, 0x000000e6, 0x000000e4, 0x000000e1, 0x000000de, 0x000000dc, 0x000000d9, 0x000000d7, 0x000000d4,
    0x000000d2, 0x000000d0, 0x000000cd, 0x000000cb, 0x000000c9, 0x000000c6, 0x000000c4, 0x000000c2, 0x000000c0, 0x000000be, 0x000000bc, 0x000000ba, 0x000000b8, 0x000000b6, 0x000000b4, 0x000000b2,
    0x000000b0, 0x000000af, 0x000000ad, 0x000000ab, 0x000000a9, 0x000000a8, 0x000000a6, 0x000000a4, 0x000000a3, 0x000000a1, 0x0000009f, 0x0000009e, 0x0000009c, 0x0000009b, 0x00000099, 0x00000098,
    0x00000096, 0x00000095, 0x00000093, 0x00000092, 0x00000091, 0x0000008f, 0x0000008e, 0x0000008d, 0x0000008b, 0x0000008a, 0x00000089, 0x00000088, 0x00000086, 0x00000085, 0x00000084, 0x00000083,
    0x00000082, 0x00000080, 0x0000007f, 0x0000007e, 0x0000007d, 0x0000007c, 0x0000007b, 0x0000007a, 0x00000079, 0x00000078, 0x00000077, 0x00000076, 0x00000075, 0x00000074, 0x00000073, 0x00000072,
    0x00000071, 0x00000070, 0x0000006f, 0x0000006e, 0x0000006d, 0x0000006c, 0x0000006b, 0x0000006b, 0x0000006a, 0x00000069, 0x00000068, 0x00000067, 0x00000066, 0x00000066, 0x00000065, 0x00000064
};

//Mapping color gamut of MM VUI to E_CFD_CFIO_GAMUTORDER_IDX
//E_CFD_CFIO_CP
//idx is from u8InputColorPriamries

MS_U8 u8_gamut_idx_order[23] =
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
    E_CFD_CFIO_GT_DCI_P3,
    E_CFD_CFIO_GT_DCI_P3,
    E_CFD_CFIO_GT_ADOBERGB,
    E_CFD_CFIO_GT_PANEL_reservedstart,
    E_CFD_CFIO_GT_BT709,
    E_CFD_CFIO_GT_BT709,
    E_CFD_CFIO_GT_BT709,
    E_CFD_CFIO_GT_BT709,
    E_CFD_CFIO_GT_BT709,
    E_CFD_CFIO_GT_BT709,
    E_CFD_CFIO_GT_BT709,
    E_CFD_CFIO_GT_BT601_525
};

//for MS_U8 MS_Cfd_InputMM_SetColorMetry()
MS_U8 u8Const_LUT_MMInfor_HDMIDefinition[11][5] =
    {
        {E_CFD_CFIO_CP_BT601625,            E_CFD_CFIO_TR_BT601525_601625,  E_CFD_CFIO_MC_BT601625_XVYCC601_SYCC,  E_CFD_CFIO_RGB_BT601_625,E_CFD_CFIO_YUV_BT601_625},              //0:BT601_625 RGB/YUV
        {E_CFD_CFIO_CP_BT601525_SMPTE170M,  E_CFD_CFIO_TR_BT601525_601625,  E_CFD_CFIO_MC_BT601525_SMPTE170M,      E_CFD_CFIO_RGB_BT601_525,E_CFD_CFIO_YUV_BT601_525},              //1:BT601_525 RGB/YUV
        {E_CFD_CFIO_CP_BT709_SRGB_SYCC,     E_CFD_CFIO_TR_BT709,            E_CFD_CFIO_MC_BT709_XVYCC709,          E_CFD_CFIO_RGB_BT709,E_CFD_CFIO_YUV_BT709},                      //2:BT601_709 RGB/YUV
        {E_CFD_CFIO_CP_BT2020,              E_CFD_CFIO_TR_BT2020NCL,        E_CFD_CFIO_MC_BT2020NCL,               E_CFD_CFIO_RGB_BT2020,E_CFD_CFIO_YUV_BT2020_NCL},               //3:BT601_2020ncl RGB/YUV
        {E_CFD_CFIO_CP_BT2020,              E_CFD_CFIO_TR_BT2020CL,         E_CFD_CFIO_MC_BT2020CL,                E_CFD_CFIO_RGB_BT2020,E_CFD_CFIO_YUV_BT2020_CL},               //4:BT601_2020cl  RGB/YUV
        {E_CFD_CFIO_CP_BT709_SRGB_SYCC,     E_CFD_CFIO_TR_SRGB_SYCC,        E_CFD_CFIO_MC_BT601625_XVYCC601_SYCC,  E_CFD_CFIO_SRGB,E_CFD_CFIO_SYCC601},                            //5:sRGB/sYCC601 RGB/YUV
        {E_CFD_CFIO_CP_ADOBERGB,            E_CFD_CFIO_TR_ADOBERGB,         E_CFD_CFIO_MC_BT601625_XVYCC601_SYCC,  E_CFD_CFIO_ADOBE_RGB,E_CFD_CFIO_ADOBE_YCC601},                 //6:AdobeRGB/AdobeYCC601 RGB/YUV
        {E_CFD_CFIO_CP_BT709_SRGB_SYCC,     E_CFD_CFIO_TR_XVYCC,            E_CFD_CFIO_MC_BT601625_XVYCC601_SYCC,  E_CFD_CFIO_RGB_NOTSPECIFIED,E_CFD_CFIO_XVYCC_601},              //7:xvYCC601 only YUV
        {E_CFD_CFIO_CP_BT709_SRGB_SYCC,     E_CFD_CFIO_TR_XVYCC,            E_CFD_CFIO_MC_BT709_XVYCC709,          E_CFD_CFIO_RGB_NOTSPECIFIED,E_CFD_CFIO_XVYCC_709},              //8:xvYCC709 only YUV
        {E_CFD_CFIO_CP_DCIP3_D65,           E_CFD_CFIO_TR_GAMMA2P6,         E_CFD_CFIO_MC_BT709_XVYCC709,          E_CFD_CFIO_RGB_DCIP3_D65,E_CFD_CFIO_YUV_NOTSPECIFIED},          //9:DCI P3 D65 only RGB
        {E_CFD_CFIO_CP_DCIP3_D65,           E_CFD_CFIO_TR_GAMMA2P6,         E_CFD_CFIO_MC_BT709_XVYCC709,          E_CFD_CFIO_RGB_DCIP3_THEATER,E_CFD_CFIO_YUV_NOTSPECIFIED},          //9:DCI P3 D65 only RGB
    };
    //table[i][0] = Color primaries
    //table[i][1] = Transfer characteristics
    //table[i][2] = Matrix coefficient
    //table[i][3] = RGB
    //table[i][4] = YUV

//for all cases control of HDR & SDR

//when seamless is not turned on
//current status
//for Kano
#if 1
MS_U8 u8Const_LUT_output_HDRmode_noseamless[3][4] =
{
    //output
    //SDR TV ,         HLG only TV,      PQ only TV,       HLG & PQ TV       //input
    {E_CFIO_MODE_SDR,  E_CFIO_MODE_SDR,  E_CFIO_MODE_SDR,  E_CFIO_MODE_SDR}, //SDR
    {E_CFIO_MODE_SDR,  E_CFIO_MODE_SDR,  E_CFIO_MODE_HDR2, E_CFIO_MODE_HDR2},//HDR PQ
    {E_CFIO_MODE_SDR,  E_CFIO_MODE_HDR3, E_CFIO_MODE_SDR,  E_CFIO_MODE_HDR3},//HDR HLG
};
#endif

//new status
//add HLG2PQ
//for Curry/K6/K6lite
#if 0
MS_U8 u8Const_LUT_output_HDRmode_noseamless[3][4] =
{
    //output
    //SDR TV ,         HLG only TV,       PQ only TV,        HLG & PQ TV        //input
    {E_CFIO_MODE_SDR,  E_CFIO_MODE_SDR,   E_CFIO_MODE_SDR,   E_CFIO_MODE_SDR},  //SDR
    {E_CFIO_MODE_SDR,  E_CFIO_MODE_SDR,   E_CFIO_MODE_HDR2,  E_CFIO_MODE_HDR2}, //HDR PQ
    {E_CFIO_MODE_SDR,  E_CFIO_MODE_HDR3,  E_CFIO_MODE_HDR2,  E_CFIO_MODE_HDR3}, //HDR HLG
};
#endif


//target
#if 0
MS_U8 u8Const_LUT_output_HDRmode_noseamless[3][4] =
{
    //output
    //SDR TV ,         HLG only TV,      PQ only TV,       HLG & PQ TV       //input
    {E_CFIO_MODE_SDR,  E_CFIO_MODE_SDR,  E_CFIO_MODE_SDR,  E_CFIO_MODE_SDR}, //SDR
    {E_CFIO_MODE_SDR,  E_CFIO_MODE_HDR3, E_CFIO_MODE_HDR2, E_CFIO_MODE_HDR2},//HDR PQ
    {E_CFIO_MODE_SDR,  E_CFIO_MODE_HDR3, E_CFIO_MODE_HDR2, E_CFIO_MODE_HDR3},//HDR HLG
};
#endif

//seamless target
#if 0
MS_U8 u8Const_LUT_output_HDRmode_seamless[3][4] =
{
    //output
    //SDR TV ,         HLG only TV,      PQ only TV,       HLG & PQ TV       //input
    {E_CFIO_MODE_SDR,  E_CFIO_MODE_HDR3, E_CFIO_MODE_HDR2, E_CFIO_MODE_HDR2},//SDR
    {E_CFIO_MODE_SDR,  E_CFIO_MODE_HDR3, E_CFIO_MODE_HDR2, E_CFIO_MODE_HDR2},//HDR PQ
    {E_CFIO_MODE_SDR,  E_CFIO_MODE_HDR3, E_CFIO_MODE_HDR2, E_CFIO_MODE_HDR2},//HDR HLG
};
#endif

//seamless current target
#if 1
MS_U8 u8Const_LUT_output_HDRmode_seamless[3][4] =
{
    //output
    //SDR TV ,         HLG only TV,      PQ only TV,       HLG & PQ TV       //input
    {E_CFIO_MODE_SDR,  E_CFIO_MODE_SDR,  E_CFIO_MODE_HDR2, E_CFIO_MODE_HDR2},//SDR
    {E_CFIO_MODE_SDR,  E_CFIO_MODE_SDR,  E_CFIO_MODE_HDR2, E_CFIO_MODE_HDR2},//HDR PQ
    {E_CFIO_MODE_SDR,  E_CFIO_MODE_HDR3, E_CFIO_MODE_HDR2, E_CFIO_MODE_HDR2},//HDR HLG
};
#endif

MS_U8 MS_Cfd_FindMatchedIdx(MS_U8 u8TempFormat)
{

    MS_U8 u8idx;
    MS_U8 u8idxub;

    u8idxub = sizeof (u8Const_LUT_MMInfor_HDMIDefinition) / (sizeof(MS_U8)*5);

    for (u8idx =0;u8idx < u8idxub ;u8idx++)
    {
        if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3] == u8TempFormat)||
            (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4] == u8TempFormat))
        {
            break;
        }
    }

    if (u8idx>=11)
    {
        u8idx = 10;
    }

    return u8idx;
}

MS_U8 Mapi_Cfd_checkColorprimaryIsOk(MS_U8 *pu8Color_primary)
{
    //0: in range
    //1: out of range
    MS_U8 u8status;

    u8status = 0;

    if ((*pu8Color_primary > E_CFD_CFIO_CP_PANEL) && ( *pu8Color_primary < E_CFD_CFIO_CP_EBU3213))
    {
        u8status = 1;
    }

    if (*pu8Color_primary >= E_CFD_CFIO_CP_RESERVED_START)
    {
        u8status = 1;
    }

    return u8status;
}

MS_U8 MS_Cfd_checkColorprimaryIsUndefined(MS_U8 *pu8Color_primary)
{
    //0: in range
    //1: out of range
    MS_U8 u8status;

    u8status = 0;

    if((E_CFD_CFIO_CP_RESERVED0 == *pu8Color_primary) ||
       (E_CFD_CFIO_CP_UNSPECIFIED == *pu8Color_primary) ||
       (E_CFD_CFIO_CP_RESERVED3 == *pu8Color_primary))
    {
        u8status = 1;
    }

    if ((*pu8Color_primary > E_CFD_CFIO_CP_PANEL) && ( *pu8Color_primary < E_CFD_CFIO_CP_EBU3213))
    {
        u8status = 1;
    }

    if (*pu8Color_primary >= E_CFD_CFIO_CP_RESERVED_START)
    {
        u8status = 1;
    }

    return u8status;
}

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



//nits2code_max_EDID
//based on CEA-861.3
MS_U8 MS_Cfd_nits2code_max_EDID(MS_U16 u16Nits, MS_U16 *nits_table)
{

    MS_U8 u8Code;
    MS_U8 u8temp;
    MS_U16 u16highbound;
    MS_U16 u16lowbound;

    if (u16Nits <= nits_table[0])
    {
        u8Code = 0;
    }
    else if (u16Nits >= nits_table[255])
    {
        u8Code = 255;
    }
    else
    {
        for (u8temp = 0;u8temp<255;u8temp++)
        {
            if ((u16Nits >= nits_table[u8temp]) && (u16Nits <= nits_table[u8temp+1]))
            {

                u16highbound = nits_table[u8temp+1];
                u16lowbound = nits_table[u8temp];
                break;
            }
        }

        if ((u16highbound - u16Nits) <= (u16Nits - u16lowbound))
        {
            u8Code = u8temp+1;
        }
        else
        {
            u8Code = u8temp;
        }

    }

    return u8Code;
}


//code2nits_max_EDID
MS_U16 MS_Cfd_code2nits_max_EDID(MS_U8 u8Code, MS_U16 *nits_table)
{
    return nits_table[u8Code];
}



//nits2code_min_EDID
//u8DataRange_flag:0 -> unit = 1nit, range = 1~10000 nits
//u8DataRange_flag:1 -> unit = 1e-4 nit, range = 1e-4 ~ 1 nits
#if 1
MS_U8 MS_Cfd_nits2code_min_EDID(MS_U16 u16Nits, MS_U32 *nits_table, MS_U16 U16_max_luminance, MS_U8 *pu8DataRange_flag)
{

    MS_U8 u8Code;
    MS_U8 u8temp;
    MS_U32 u32highbound;
    MS_U32 u32lowbound;

    //MS_U16 u16MinOverMax = 0;

    MS_U32 u32ratio;
    MS_U32 u32Temp_max ;

    u32Temp_max = U16_max_luminance;

    //division protect
    if (u16Nits == 0)
    u16Nits = 1;

    if (0 == *pu8DataRange_flag)
    {
        u32ratio = U16_max_luminance/u16Nits;
    }
    else
    {
        u32ratio = (((MS_U32)U16_max_luminance)*10000/u16Nits);
    }

    //starts from 1
    if (u32ratio >= nits_table[1])
    {
        u8Code = 0;
    }
    else if (u32ratio <= nits_table[255])
    {
        u8Code = 255;
    }
    else
    {
        for (u8temp = 1;u8temp<255;u8temp++) // for i = 1:1:254
        {
            if ((u32ratio <= nits_table[u8temp]) && (u32ratio >= nits_table[u8temp+1]))
            {

                u32highbound = nits_table[u8temp];
                u32lowbound = nits_table[u8temp+1];
                break;
            }
        }

        if ((u32highbound - u16Nits) <= (u16Nits - u32lowbound))
        {
            u8Code = u8temp;
        }
        else
        {
            u8Code = u8temp+1;
        }

    }

    return u8Code;
}

#endif

//code2nits_min_EDID
//u8DataRange_flag:0 -> unit = 1nit, range = 1~10000 nits
//u8DataRange_flag:1 -> unit = 1e-4 nit, range = 1e-4 ~ 1 nits
MS_U16 MS_Cfd_code2nits_min_EDID(MS_U8 u8Code, MS_U32 *nits_table, MS_U16 U16_max_luminance, MS_U8 *pu8DataRange_flag)
{
    MS_U16 u16Min_nits = 0;

    //ratio = 100*((255/CV)^2) = max/min
    MS_U32 u32ratio = nits_table[u8Code];
    MS_U32 u32Temp_max ;

    u32Temp_max = U16_max_luminance;


    if (u32Temp_max>= u32ratio)
    {
        *pu8DataRange_flag = 0;
        u16Min_nits = u32Temp_max/u32ratio;
    }
    else
    {
        *pu8DataRange_flag = 1;
        u16Min_nits = (u32Temp_max*10000)/u32ratio;
    }

    return u16Min_nits;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//specify which color space is supported by HDMI sink ++++++++++++++++++++++++++++++++++++++++

//for current design of Kano
//only support 4 main gamuts

//MS_U8 u8_gamut_array[4][3] =
//{
//  {0,1,E_CFD_CFIO_GT_BT601_525},
//  {0,2,E_CFD_CFIO_GT_BT709},
//  {0,2,E_CFD_CFIO_GT_ADOBERGB},
//  {0,2,E_CFD_CFIO_GT_BT2020}
//};

//[x][0] -> 0:not supported by EDID & Current HW, 1:supported by EDID & Current HW
//[x][1] -> 0:only RGB, 1:only YUV 2:both
//[x][2] -> colormetry supported by this HW

//@param

//u8Mode
//0: for outputformat
//1: for temp format[1]

//----------------------------------------------------------------------------------------------------------------------------------------------------
void MS_Cfd_SetOutputGamut_HDMISink(MS_U8 *pu8temp_GamutOrderIdx, STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParamInit,STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam,STU_CFDAPI_MAIN_CONTROL *pstApiControlParamInit, MS_U8 u8Mode)
{
    //
    MS_U8 temp_DataFormat;
    MS_U8 temp_input_GamutOrderIdx;


    if (0 == u8Mode)
    {
        temp_DataFormat = pstu_ControlParam->u8Output_DataFormat;
        temp_input_GamutOrderIdx = pstu_ControlParam->u8Temp_GamutOrderIdx[1];
    }
    else if (1 == u8Mode)
    {
        temp_DataFormat = pstu_ControlParam->u8Temp_DataFormat[1];
        temp_input_GamutOrderIdx = pstu_ControlParam->u8Temp_GamutOrderIdx[0];
    }
    else if (2 == u8Mode)
    {
        temp_DataFormat = E_CFD_MC_FORMAT_YUV444;
        temp_input_GamutOrderIdx = pstu_ControlParam->u8Input_GamutOrderIdx;
    }

    //from 0 ~ 8
                /*
                typedef enum
                {
                    E_CFD_CFIO_GT_BT601_525  = 0x0, //used colorspace has min gamut
                                                    //SMPTE 170M, SMPTE 240M

                    E_CFD_CFIO_GT_BT709      = 0x1, //sRGB
                    E_CFD_CFIO_GT_BT601_625  = 0x2,
                    E_CFD_CFIO_GT_ADOBERGB   = 0x3, //AdobeRGB/YUV
                    E_CFD_CFIO_GT_BT4706     = 0x4, //BT470_6
                    E_CFD_CFIO_GT_GF         = 0x5, //Generic film
                    E_CFD_CFIO_GT_XVYCC      = 0x6, //sYCC601,xvYCC601,xvYCC709
                    E_CFD_CFIO_GT_BT2020     = 0x7, //BT2020
                    E_CFD_CFIO_GT_XYZ        = 0x8, //CIE XYZ

                    E_CFD_CFIO_GT_PANEL      = 0x9  //Panel or user-defined gamut

                }E_CFD_CFIO_GAMUTORDER_IDX;
                */

                MS_U8 u8_gamut_array[5][3] =
                {
                    {1,1,E_CFD_CFIO_GT_BT601_525},
                    {1,2,E_CFD_CFIO_GT_BT709},
                    {1,2,E_CFD_CFIO_GT_ADOBERGB},
                    {1,0,E_CFD_CFIO_GT_DCI_P3},
                    {1,2,E_CFD_CFIO_GT_BT2020}
                };

                MS_U8 temp = 0;
                MS_U8 temp_lb = 0;
                MS_U8 temp_hb = 0;
                MS_U8 temp_hb_valid = 0;
                MS_U8 temp_lb_valid = 0;
                MS_U8 u8Match_Flag = 0;
                MS_U8 u8Result = 0;
                MS_U8 tempub = 0 ;

                tempub = sizeof (u8_gamut_array) /(sizeof(MS_U8)*3);

                //RGB is supported by Sink in default
                //Sink support YUV
                if ((pstHDMI_EDIDParamInit->u8HDMISink_Support_YUVFormat&0x07)!=0)
                {
                    u8_gamut_array[0][0] = E_CFD_VALID;
                }
                else
                {
                    u8_gamut_array[0][0] = E_CFD_NOT_VALID;
                }

                //Even sink support xvYCC , but Kano not support
                //E_CFD_CFIO_GT_ADOBERGB
                if (((pstHDMI_EDIDParamInit->u8HDMISink_Extended_Colorspace&0x10)==0x10) && ((CFD_HDMISINK_COLORIMETRY_CAPABILITY&0x10)==0x10))
                {
                    u8_gamut_array[2][0] = E_CFD_VALID;
                }
                else
                {
                    u8_gamut_array[2][0] = E_CFD_NOT_VALID;
                }

                //E_CFD_CFIO_GT_DCIP3
                if (((pstHDMI_EDIDParamInit->u8HDMISink_Extended_Colorspace_byte4&0x10)==0x10) && ((CFD_HDMISINK_COLORIMETRY_CAPABILITY_byte4&0x10)==0x10))
                {
                    u8_gamut_array[3][0] = E_CFD_VALID;
                }
                else
                {
                    u8_gamut_array[3][0] = E_CFD_NOT_VALID;
                }

                //E_CFD_CFIO_GT_BT2020
                if (((pstHDMI_EDIDParamInit->u8HDMISink_Extended_Colorspace&0xE0)!=0x00) && ((CFD_HDMISINK_COLORIMETRY_CAPABILITY&0xE0)!=0x00))
                {
                    u8_gamut_array[4][0] = E_CFD_VALID;
                }
                else
                {
                    u8_gamut_array[4][0] = E_CFD_NOT_VALID;
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

                for (temp = 0; temp < tempub; temp++)
                {

                    u8Match_Flag = 0;


                    if (E_CFD_VALID == u8_gamut_array[temp][0]) //this case is avaliable from the information from EDID
                    {
                        //RGB
                        if ((E_CFD_MC_FORMAT_RGB == temp_DataFormat) && ((0 == u8_gamut_array[temp][1] ) || (2 == u8_gamut_array[temp][1] )))
                        {
                            u8Match_Flag = 1;
                        }
                        //YUV
                        else if ((E_CFD_MC_FORMAT_RGB != temp_DataFormat) && ((1 == u8_gamut_array[temp][1] ) || (2 == u8_gamut_array[temp][1] )))
                        {
                            u8Match_Flag = 1;
                        }

                        if ((1 == u8_gamut_array[temp][0]) && (temp_input_GamutOrderIdx>=u8_gamut_array[temp][2]) && (1 == u8Match_Flag))
                        {
                            temp_lb = u8_gamut_array[temp][2];
                            temp_lb_valid = 1;
                        }

                        if ((1 == u8_gamut_array[temp][0]) && (temp_input_GamutOrderIdx<=u8_gamut_array[temp][2]) && (1 == u8Match_Flag))
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
                        //pstu_ControlParam->u8Output_GamutOrderIdx = temp_hb;
                        u8Result = temp_hb;
                    }
                    else
                    {
                        //gamut extension
                        if (0 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_MethodMode)
                        {
                            //pstu_ControlParam->u8Output_GamutOrderIdx = temp_hb;
                            u8Result = temp_hb;
                        }
                        else //gamut compression
                        {
                            //pstu_ControlParam->u8Output_GamutOrderIdx = temp_lb;
                            u8Result = temp_lb;
                        }
                    }
                }
                else if ((1 == temp_lb_valid) && (0 == temp_hb_valid))
                {
                    //pstu_ControlParam->u8Output_GamutOrderIdx = temp_lb;
                    u8Result = temp_lb;
                }
                else if ((1 == temp_hb_valid) && (0 == temp_lb_valid))
                {
                    //pstu_ControlParam->u8Output_GamutOrderIdx = temp_hb;
                    u8Result = temp_hb;
                }
                else //force 709
                {
                    //pstu_ControlParam->u8Output_GamutOrderIdx = E_CFD_CFIO_GT_BT709;
                    u8Result = E_CFD_CFIO_GT_BT709;
                }

                } //for test - for (MS_U8 temp1 = 0;temp1<=8;temp1++)

    *pu8temp_GamutOrderIdx = u8Result;

}

MS_U8 MS_Cfd_SetFormat_OnlyY2Rlite(MS_U8 u8input_format)
{
    MS_U8 u8temp = 0;


    switch(u8input_format)
    {
        case E_CFD_CFIO_YUV_NOTSPECIFIED:
            u8temp = E_CFD_CFIO_RGB_NOTSPECIFIED;
            break;
        case E_CFD_CFIO_YUV_BT601_625 :
            u8temp = E_CFD_CFIO_RGB_BT601_625;
            break;
        case E_CFD_CFIO_YUV_BT601_525 :
            u8temp = E_CFD_CFIO_RGB_BT601_525;
            break;
        case E_CFD_CFIO_YUV_BT709 :
            u8temp = E_CFD_CFIO_RGB_BT709;
            break;
        case E_CFD_CFIO_YUV_BT2020_NCL :
            u8temp = E_CFD_CFIO_RGB_BT2020;
            break;
        case E_CFD_CFIO_YUV_BT2020_CL :
            u8temp = E_CFD_CFIO_RGB_BT2020;
            break;
        case E_CFD_CFIO_XVYCC_601 :
            u8temp = E_CFD_CFIO_SRGB;
            break;
        case E_CFD_CFIO_XVYCC_709 :
            u8temp = E_CFD_CFIO_SRGB;
            break;
        case E_CFD_CFIO_SYCC601 :
            u8temp = E_CFD_CFIO_SRGB;
            break;
        case E_CFD_CFIO_ADOBE_YCC601:
            u8temp = E_CFD_CFIO_ADOBE_RGB;
            break;
        default:
            u8temp = E_CFD_CFIO_RGB_NOTSPECIFIED;
            break;
    }

    return u8temp;
}

//----------------------------------------------------------------------------------------

//@param
//u8input_format (only YUV)


//@return value
//output format (only RGB)

//----------------------------------------------------------------------------------------

MS_U8 MS_Cfd_SetFormat_OnlyY2R(MS_U8 u8input_format, STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam)
{
    MS_U8 u8temp = 0;

    pstu_ControlParam->u8XVYCC2SRGBFlag = 0;

    switch(u8input_format)
    {
        case E_CFD_CFIO_YUV_NOTSPECIFIED:
            u8temp = E_CFD_CFIO_RGB_NOTSPECIFIED;
            break;
        case E_CFD_CFIO_YUV_BT601_625 :
            u8temp = E_CFD_CFIO_RGB_BT601_625;
            break;
        case E_CFD_CFIO_YUV_BT601_525 :
            u8temp = E_CFD_CFIO_RGB_BT601_525;
            break;
        case E_CFD_CFIO_YUV_BT709 :
            u8temp = E_CFD_CFIO_RGB_BT709;
            break;
        case E_CFD_CFIO_YUV_BT2020_NCL :
            u8temp = E_CFD_CFIO_RGB_BT2020;
            break;
        case E_CFD_CFIO_YUV_BT2020_CL :
            u8temp = E_CFD_CFIO_RGB_BT2020;
            break;
        case E_CFD_CFIO_XVYCC_601 :
            u8temp = E_CFD_CFIO_SRGB;
            pstu_ControlParam->u8XVYCC2SRGBFlag = 1;
            break;
        case E_CFD_CFIO_XVYCC_709 :
            u8temp = E_CFD_CFIO_SRGB;
            pstu_ControlParam->u8XVYCC2SRGBFlag = 1;
            break;
        case E_CFD_CFIO_SYCC601 :
            u8temp = E_CFD_CFIO_SRGB;
            pstu_ControlParam->u8XVYCC2SRGBFlag = 1;
            break;
        case E_CFD_CFIO_ADOBE_YCC601:
            u8temp = E_CFD_CFIO_ADOBE_RGB;
            break;
        default:
            u8temp = E_CFD_CFIO_RGB_NOTSPECIFIED;
            break;
    }

    return u8temp;
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

    printf("u8Input_HDRIPMode                      :0x%02x\n", pstu_ControlParam->u8Input_HDRIPMode         );
    printf("u8Input_SDRIPMode                      :0x%02x\n", pstu_ControlParam->u8Input_SDRIPMode         );
    printf("\n");

    printf("u8Input_DataFormat                     :0x%02x\n", pstu_ControlParam->u8Input_DataFormat         );
    printf("u8Input_IsFullRange                    :0x%02x\n", pstu_ControlParam->u8Input_IsFullRange         );
    printf("u8Input_HDRMode                        :0x%02x\n", pstu_ControlParam->u8Input_HDRMode         );
    printf("u8Input_GamutOrderIdx                  :0x%02x\n", pstu_ControlParam->u8Input_GamutOrderIdx         );
    printf("u8InputColorPriamries                  :0x%02x\n", pstu_ControlParam->u8InputColorPriamries         );
    printf("u8InputTransferCharacterstics          :0x%02x\n", pstu_ControlParam->u8InputTransferCharacterstics         );
    printf("u8InputMatrixCoefficients              :0x%02x\n", pstu_ControlParam->u8InputMatrixCoefficients         );
    printf("\n");

    printf("u8Temp_DataFormat[0]                   :0x%02x\n", pstu_ControlParam->u8Temp_DataFormat[0]        );
    printf("u8Temp_IsFullRange[0]                  :0x%02x\n", pstu_ControlParam->u8Temp_IsFullRange[0]           );
    printf("u8Temp_HDRMode[0]                      :0x%02x\n", pstu_ControlParam->u8Temp_HDRMode[0]          );
    printf("u8Temp_GamutOrderIdx[0]                :0x%02x\n", pstu_ControlParam->u8Temp_GamutOrderIdx[0]         );
    printf("u8TempColorPriamries[0]                :0x%02x\n", pstu_ControlParam->u8TempColorPriamries[0]         );
    printf("u8TempTransferCharacterstics[0]        :0x%02x\n", pstu_ControlParam->u8TempTransferCharacterstics[0]         );
    printf("u8TempMatrixCoefficients[0]            :0x%02x\n", pstu_ControlParam->u8TempMatrixCoefficients[0]         );
    printf("\n");

    printf("u8Temp_DataFormat[1]                   :0x%02x\n", pstu_ControlParam->u8Temp_DataFormat[1]        );
    printf("u8Temp_IsFullRange[1]                  :0x%02x\n", pstu_ControlParam->u8Temp_IsFullRange[1]           );
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
    printf("test vector                          :0x%02x\n",CFD_USE_TESTVECTOR  );
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

            HDR_DBG_HAL_CFD(printf("\nForce:Current HDR IP can not support BT2020CL to NCL,CFD will force u8Input_HDRIPMode = 0\n"));
        }

        //case 1:xvYCC601,709,sYCC601 to other gamut
        if (((E_CFD_CFIO_XVYCC_601 == pstu_ControlParam->u8Temp_Format[0]) || (E_CFD_CFIO_XVYCC_709 == pstu_ControlParam->u8Temp_Format[0])||(E_CFD_CFIO_SYCC601 == pstu_ControlParam->u8Temp_Format[0]))
            && (pstu_ControlParam->u8Temp_Format[0] != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_GMforXVYCC_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printf("\nForce:Current HDR IP is not suggested to do gamut mapping for the current input colro space"));
            HDR_DBG_HAL_CFD(printf("\nCFD will force u8Input_HDRIPMode = 0\n"));
        }

        //case 2: wrong TMO : SDR to HDR
        if ((E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[0]) && (E_CFIO_MODE_SDR != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_SDR2HDR_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printf("\nForce:Current HDR IP can not support SDR to HDR\n"));
        }

             //case 3: wrong TMO :HDRx to HDRy
#if 0
        if ((E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Temp_HDRMode[0]) && (E_CFIO_MODE_HDR1 == pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_HDRXtoHDRY_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printk("\nForce:Current HDR IP can not support HDR2 to HDR1\n"));
        }
#else
        if ((E_CFIO_MODE_SDR != pstu_ControlParam->u8Temp_HDRMode[0]) &&
            (E_CFIO_MODE_SDR != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode) &&
            (pstu_ControlParam->u8Temp_HDRMode[0] != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_HDRXtoHDRY_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printk("\nForce:Current HDR IP can not support HDRx to HDRy\n"));
        }
#endif

    }

    if (E_CFD_MC_ERR_NOERR != u16Check_status)
    {
        HDR_DBG_HAL_CFD(printf("\nError:HDR IP cannot support this process, check IO of HDR IP\n"));
        HDR_DBG_HAL_CFD(printf("Error code = %04x !!!, error is in [ %s  , %d]\n",u16Check_status, __FUNCTION__,__LINE__));
}

    return u16Check_status;
}

#if (NowHW == Kano)
MS_U16 MS_Cfd_Kano_CheckHDRIPProcessIsOk(STU_CFDAPI_MAIN_CONTROL *pstApiControlParamInit, STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam, MS_U8 u8_IP_capability)
{
    MS_U16 u16Check_status = E_CFD_MC_ERR_NOERR;

    MS_U8 doGMFlag = 0;
    MS_U8 doTMOFlag = 0;
    MS_U8 doBT2020CL = 0;
    MS_U8 doXVYCC = 0;

    //for HDR IP

    if ((1 == pstu_ControlParam->u8Input_HDRIPMode) || (2 == pstu_ControlParam->u8Input_HDRIPMode))
    {

        //bit[1] of u8_IP_capability
        //Gamut mapping is supported or not
        if ((pstu_ControlParam->u8Temp_Format[0] != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format) && (0x00 == (u8_IP_capability&0x02)))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_GM_NOTSUPPORTED;

            //HDR_DBG_HAL_CFD(printk("\nForce:Current HDR IP can not support Gamut mapping, CFD will force u8Input_HDRIPMode = 0\n"));
            printf("\033[1;35m###[CFD][%s][%d]### Current HDR IP can not support Gamut mapping, force bypass \033[0m\n",__FUNCTION__,__LINE__);
        }

        if ((pstu_ControlParam->u8Temp_Format[0] != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format) && (0x02 == (u8_IP_capability&0x02)))
        {
            doGMFlag = 1;
        }

        //bit[2] of u8_IP_capability
        //TMO is supported or not
        if ((E_CFIO_MODE_SDR != pstu_ControlParam->u8Temp_HDRMode[0]) && (E_CFIO_MODE_SDR == pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode) && (0x00 == (u8_IP_capability&0x04)))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_TMO_NOTSUPPORTED;

            //HDR_DBG_HAL_CFD(printk("\nForce:Current HDR IP can not support TMO, CFD will force u8Input_HDRIPMode = 0\n"));
            printf("\033[1;35m###[CFD][%s][%d]### Current HDR IP can not support TMO, force bypass \033[0m\n",__FUNCTION__,__LINE__);
        }

        if ((E_CFIO_MODE_SDR != pstu_ControlParam->u8Temp_HDRMode[0]) && (E_CFIO_MODE_SDR == pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode) && (0x04 == (u8_IP_capability&0x04)))
        {
            doTMOFlag = 1;
        }

        //bit[3] of u8_IP_capability
        //YUV BT2020CL to NCL is supported or not
        if ((E_CFD_CFIO_YUV_BT2020_CL == pstu_ControlParam->u8Temp_Format[0]) && (E_CFD_CFIO_YUV_BT2020_CL != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format) && (0x00 == (u8_IP_capability&0x08)))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_BT2020CLtoNCL_NOTSUPPORTED;

            //HDR_DBG_HAL_CFD(printk("\nForce:Current HDR IP can not support BT2020CL to NCL,CFD will force u8Input_HDRIPMode = 0\n"));
            printf("\033[1;35m###[CFD][%s][%d]### Current HDR IP can not support BT2020CL to NCL, force bypass \033[0m\n",__FUNCTION__,__LINE__);
        }

        if ((E_CFD_CFIO_YUV_BT2020_CL == pstu_ControlParam->u8Temp_Format[0]) && (E_CFD_CFIO_YUV_BT2020_CL != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format) && (0x08 == (u8_IP_capability&0x08)))
        {
            doBT2020CL = 1;
        }

        //bit[4] of u8_IP_capability
        //case 1:xvYCC601,709,sYCC601 to other gamut
        //xvYCC to other case is supported or not
        if (((E_CFD_CFIO_XVYCC_601 == pstu_ControlParam->u8Temp_Format[0]) || (E_CFD_CFIO_XVYCC_709 == pstu_ControlParam->u8Temp_Format[0])||(E_CFD_CFIO_SYCC601 == pstu_ControlParam->u8Temp_Format[0]))
            && (pstu_ControlParam->u8Temp_Format[0] != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format))
        {
            if (0x00 == (u8_IP_capability&0x10))
            {
                u16Check_status = E_CFD_MC_ERR_HW_IPS_GMforXVYCC_NOTSUPPORTED;

                //HDR_DBG_HAL_CFD(printk("\nForce:Current HDR IP is not suggested to do gamut mapping for the current input colro space"));
                //HDR_DBG_HAL_CFD(printk("\nCFD will force u8Input_HDRIPMode = 0\n"));
                printf("\033[1;35m###[CFD][%s][%d]### Current HDR IP can not support GM for current colorspace, force bypass \033[0m\n",__FUNCTION__,__LINE__);
            }
        }

        if (((E_CFD_CFIO_XVYCC_601 == pstu_ControlParam->u8Temp_Format[0]) || (E_CFD_CFIO_XVYCC_709 == pstu_ControlParam->u8Temp_Format[0])||(E_CFD_CFIO_SYCC601 == pstu_ControlParam->u8Temp_Format[0]))
            && (pstu_ControlParam->u8Temp_Format[0] != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format))
        {
            if (0x10 == (u8_IP_capability&0x10))
            {
                doXVYCC = 1;
            }
        }

        //wrong settings case: wrong TMO : SDR to HDR
        if ((E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[0]) && (E_CFIO_MODE_SDR != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_SDR2HDR_NOTSUPPORTED;

            //HDR_DBG_HAL_CFD(printk("\nForce:Current HDR IP can not support SDR to HDR\n"));
            printf("\033[1;35m###[CFD][%s][%d]### Current HDR IP can not support SDR2HDR, force bypass \033[0m\n",__FUNCTION__,__LINE__);
        }

#if 0
        if ((E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Temp_HDRMode[0]) && (E_CFIO_MODE_HDR1 == pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_HDRXtoHDRY_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printk("\nForce:Current HDR IP can not support HDR2 to HDR1\n"));
            printf("\033[1;35m###[CFD][%s][%d]### Current HDR IP can not support HDR2 to HDR1, force bypass \033[0m\n",__FUNCTION__,__LINE__);
        }
#endif

        if ((E_CFIO_MODE_SDR != pstu_ControlParam->u8Temp_HDRMode[0]) &&
            (E_CFIO_MODE_SDR != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode) &&
            (pstu_ControlParam->u8Temp_HDRMode[0] != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_HDRXtoHDRY_NOTSUPPORTED;

            //HDR_DBG_HAL_CFD(printk("\nForce:Current HDR IP can not support HDRx to HDRy\n"));
            printf("\033[1;35m###[CFD][%s][%d]### Current HDR IP can not support HDRx to HDRy, force bypass \033[0m\n",__FUNCTION__,__LINE__);
        }

        //bypass HDRIP , when SDR input not do GM, BT2020CL, xvYCC process
        if ((E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[0]) && (doGMFlag == 0) && (doBT2020CL == 0) && (doXVYCC == 0))
        {
            pstApiControlParamInit->u8Input_HDRIPMode = 0;
        }
    }

#if 0
    if (E_CFD_MC_ERR_NOERR != u16Check_status)
    {
        HDR_DBG_HAL_CFD(printk("\nError:HDR IP cannot support this process, check IO of HDR IP\n"));
        HDR_DBG_HAL_CFD(printk("Error code = %04x !!!, error is in [ %s  , %d]\n",u16Check_status, __FUNCTION__,__LINE__));
    }
#endif

    return u16Check_status;
}
#endif
//---------------------------------------------------------------------------------------------------------------
//@param

//return value
//0: no GM happens
//1: GM will happens

//---------------------------------------------------------------------------------------------------------------


MS_U8 MS_Cfd_CheckDoGM(MS_U8 input_format, MS_U8 input_dataformat, MS_U8 output_format, MS_U8 output_dataformat)
{
    //transfer:
    //0: R2R
    //1: R2Y
    //2: Y2Y
    //3: Y2R

    MS_U8 transfer = 0;
    MS_U8 u8CheckDoGMFlag = 0;
    MS_U8 u8TempFormtat =0 ;

    if (( E_CFD_MC_FORMAT_RGB == input_dataformat) && (E_CFD_MC_FORMAT_RGB == output_dataformat))
    {
        transfer = 0;
    }
    else if (( E_CFD_MC_FORMAT_RGB == input_dataformat) && (E_CFD_MC_FORMAT_RGB != output_dataformat))
    {
        transfer = 1;
    }
    else if (( E_CFD_MC_FORMAT_RGB != input_dataformat) && (E_CFD_MC_FORMAT_RGB != output_dataformat))
    {
        transfer = 2;
    }
    else //Y2R
    {
        transfer = 3;
    }

    if ((0 == transfer) || (2 == transfer))
    {
             if ((input_format == output_format) ||
            ((input_format>E_CFD_CFIO_RGB_NOTSPECIFIED)&&(input_format<E_CFD_CFIO_RGB_END)&&(output_format == E_CFD_CFIO_RGB_NOTSPECIFIED)) ||
            ((input_format>E_CFD_CFIO_YUV_NOTSPECIFIED)&&(input_format<E_CFD_CFIO_YUV_END)&&(output_format == E_CFD_CFIO_YUV_NOTSPECIFIED)))
        {
            u8CheckDoGMFlag = 0;
        }
        else
        {
            u8CheckDoGMFlag = 1;
        }
    }
    else
    {

        //R2Y
        if (1 == transfer)
        {
            //call y2r() to compare
            u8TempFormtat = MS_Cfd_SetFormat_OnlyY2Rlite(output_format);

            if (input_format == u8TempFormtat)
            {
                u8CheckDoGMFlag = 0;
            }
            else
            {
                u8CheckDoGMFlag = 1;
            }
        }
        else //Y2R
        {
            //call y2r() to compare
            u8TempFormtat = MS_Cfd_SetFormat_OnlyY2Rlite(input_format);

            if (output_format == u8TempFormtat)
            {
                u8CheckDoGMFlag = 0;
            }
            else
            {
                u8CheckDoGMFlag = 1;
            }

        }

    }

    return u8CheckDoGMFlag;
}

#if ((NowHW == Curry) || (NowHW == Kastor))
MS_U16 MS_Cfd_Curry_CheckHDRIPProcessIsOk(STU_CFDAPI_MAIN_CONTROL *pstApiControlParamInit, STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam, MS_U8 u8_IP_capability)
{
    MS_U16 u16Check_status = E_CFD_MC_ERR_NOERR;

    //for HDR IP

    if ((1 == pstu_ControlParam->u8Input_HDRIPMode) || (2 == pstu_ControlParam->u8Input_HDRIPMode))
    {

        //bit[1] of u8_IP_capability
        //Gamut mapping is supported or not
        //if ((pstu_ControlParam->u8Temp_Format[0] != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format) && (0x00 == (u8_IP_capability&0x02)))

        if ((0x00 == (u8_IP_capability&0x02)) && (1 == MS_Cfd_CheckDoGM(pstu_ControlParam->u8Temp_Format[0],pstu_ControlParam->u8Temp_DataFormat[0],pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format,pstApiControlParamInit->stu_Middle_Format[0].u8Mid_DataFormat)))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_GM_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printf("\nForce:Current HDR IP can not support Gamut mapping,CFD will force u8Input_HDRIPMode = 0\n"));
        }

        //bit[2] of u8_IP_capability
        //TMO is supported or not
        if ((E_CFIO_MODE_SDR != pstu_ControlParam->u8Temp_HDRMode[0]) && (E_CFIO_MODE_SDR == pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode) && (0x00 == (u8_IP_capability&0x04)))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_TMO_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printf("\nForce:Current HDR IP can not support TMO, CFD will force u8Input_HDRIPMode = 0\n"));
        }

        //bit[3] of u8_IP_capability
        //YUV BT2020CL to NCL is supported or not
        if ((E_CFD_CFIO_YUV_BT2020_CL == pstu_ControlParam->u8Temp_Format[0]) && (E_CFD_CFIO_YUV_BT2020_CL != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format) && (0x00 == (u8_IP_capability&0x08)))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_BT2020CLtoNCL_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printf("\nForce:Current HDR IP can not support BT2020CL to NCL,CFD will force u8Input_HDRIPMode = 0\n"));
        }

        //bit[4] of u8_IP_capability
        //case 1:xvYCC601,709,sYCC601 to other gamut
        //xvYCC to other case is supported or not
        if (((E_CFD_CFIO_XVYCC_601 == pstu_ControlParam->u8Temp_Format[0]) || (E_CFD_CFIO_XVYCC_709 == pstu_ControlParam->u8Temp_Format[0])||(E_CFD_CFIO_SYCC601 == pstu_ControlParam->u8Temp_Format[0]))
            && (pstu_ControlParam->u8Temp_Format[0] != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format))
        {
            if (0x00 == (u8_IP_capability&0x10))
            {
                u16Check_status = E_CFD_MC_ERR_HW_IPS_GMforXVYCC_NOTSUPPORTED;

                HDR_DBG_HAL_CFD(printf("\nForce:Current HDR IP is not suggested to do gamut mapping for the current input colro space"));
                HDR_DBG_HAL_CFD(printf("\nCFD will force u8Input_HDRIPMode = 0\n"));
            }
        }

#if 0
        //wrong settings case: wrong TMO : SDR to HDR
        if ((E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[0]) && (E_CFIO_MODE_SDR != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_SDR2HDR_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printf("\nForce:Current HDR IP can not support SDR to HDR\n"));
        }

        if ((E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Temp_HDRMode[0]) && (E_CFIO_MODE_HDR1 == pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_HDRXtoHDRY_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printk("\nForce:Current HDR IP can not support HDR2 to HDR1\n"));
        }

        if ((E_CFIO_MODE_SDR != pstu_ControlParam->u8Temp_HDRMode[0]) &&
            (E_CFIO_MODE_SDR != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode) &&
            (pstu_ControlParam->u8Temp_HDRMode[0] != pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode))
        {
            u16Check_status = E_CFD_MC_ERR_HW_IPS_HDRXtoHDRY_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printk("\nForce:Current HDR IP can not support HDRx to HDRy\n"));
        }
#endif

    }

    if (E_CFD_MC_ERR_NOERR != u16Check_status)
    {
        HDR_DBG_HAL_CFD(printf("\nError:HDR IP cannot support this process, check IO of HDR IP\n"));
        HDR_DBG_HAL_CFD(printf("Error code = %04x !!!, error is in [ %s  , %d]\n",u16Check_status, __FUNCTION__,__LINE__));
    }

    return u16Check_status;
}
#endif

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

                HDR_DBG_HAL_CFD(printf("\n Error: Current SDR IP group can not support YUV out and Do BT2020CL handle at the same time\n"));
                u16Check_status = E_CFD_MC_ERR_HW_IPS_BT2020CLtoNCL_NOTSUPPORTED;

                HDR_DBG_HAL_CFD(printf("\n Force: output format is the same as input format of SDR IP\n"));
                pstu_ControlParam->u8DoSDRIP_ForceNOBT2020CL_Flag = 1;

                pstu_ControlParam->u8Output_Format = E_CFD_CFIO_YUV_BT2020_CL;
                pstu_ControlParam->u8OutputColorPriamries = pstu_ControlParam->u8TempColorPriamries[1];
                pstu_ControlParam->u8OutputTransferCharacterstics = pstu_ControlParam->u8TempTransferCharacterstics[1];
                pstu_ControlParam->u8OutputMatrixCoefficients = pstu_ControlParam->u8TempMatrixCoefficients[1];
                pstu_ControlParam->u8Output_GamutOrderIdx = pstu_ControlParam->u8Temp_GamutOrderIdx[1];
            }

            if (pstu_ControlParam->u8Output_GamutOrderIdx != pstu_ControlParam->u8Temp_GamutOrderIdx[1])
            {

                HDR_DBG_HAL_CFD(printf("\n Force:Current SDR IP can not support YUV out and Do GM at the same time\n"));
                u16Check_status = E_CFD_MC_ERR_HW_IPS_GM_NOTSUPPORTED;

                HDR_DBG_HAL_CFD(printf("\n Force: output format is the same as input format of SDR IP\n"));
                pstu_ControlParam->u8DoSDRIP_ForceNOGM_Flag = 1;

                pstu_ControlParam->u8Output_Format = pstu_ControlParam->u8Temp_Format[1];
                pstu_ControlParam->u8OutputColorPriamries = pstu_ControlParam->u8TempColorPriamries[1];
                pstu_ControlParam->u8OutputTransferCharacterstics = pstu_ControlParam->u8TempTransferCharacterstics[1];
                pstu_ControlParam->u8OutputMatrixCoefficients = pstu_ControlParam->u8TempMatrixCoefficients[1];
                pstu_ControlParam->u8Output_GamutOrderIdx = pstu_ControlParam->u8Temp_GamutOrderIdx[1];
            }

            if ((pstu_ControlParam->u8Temp_HDRMode[1] != pstApiControlParamInit->u8Output_HDRMode) && ( E_CFIO_MODE_SDR == pstApiControlParamInit->u8Output_HDRMode))
            {

                HDR_DBG_HAL_CFD(printf("\n Force:Current SDR IP can not support YUV out and Do TMO\n"));
                u16Check_status = E_CFD_MC_ERR_HW_IPS_TMO_NOTSUPPORTED;

                HDR_DBG_HAL_CFD(printf("\n Force: output format HDR mode is the same as input HDR mode of SDR IP\n"));
                pstu_ControlParam->u8DoSDRIP_ForceNOTMO_Flag = 1;

                pstApiControlParamInit->u8Output_HDRMode = pstu_ControlParam->u8Temp_HDRMode[1];
            }
        }

        //case 2: wrong TMO : SDR to HDR
        if ((E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[1]) && (E_CFIO_MODE_SDR != pstApiControlParamInit->u8Output_HDRMode))
        {

            HDR_DBG_HAL_CFD(printf("\nForce:Current SDR IP can not support SDR to HDR\n"));
            u16Check_status = E_CFD_MC_ERR_HW_IPS_SDR2HDR_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printf("\n Force: output format HDR mode is the same as input HDR mode of SDR IP\n"));
            pstu_ControlParam->u8DoSDRIP_ForceNOTMO_Flag = 1;

            pstApiControlParamInit->u8Output_HDRMode = pstu_ControlParam->u8Temp_HDRMode[1];
        }

        //if ((E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Temp_HDRMode[1]) && (E_CFIO_MODE_HDR1 == pstApiControlParamInit->u8Output_HDRMode))
        if ((E_CFIO_MODE_SDR != pstu_ControlParam->u8Temp_HDRMode[1]) &&
            (E_CFIO_MODE_SDR != pstApiControlParamInit->u8Output_HDRMode) &&
            (pstu_ControlParam->u8Temp_HDRMode[1] != pstApiControlParamInit->u8Output_HDRMode))
        {

            HDR_DBG_HAL_CFD(printf("\nForce:Current SDR IP can not support HDR2 to HDR1\n"));
            u16Check_status = E_CFD_MC_ERR_HW_IPS_HDRXtoHDRY_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printf("\n Force: output format HDR mode is the same as input HDR mode of SDR IP\n"));
            pstu_ControlParam->u8DoSDRIP_ForceNOTMO_Flag = 1;

            pstApiControlParamInit->u8Output_HDRMode = pstu_ControlParam->u8Temp_HDRMode[1];
        }

    }

    if (E_CFD_MC_ERR_NOERR != u16Check_status)
    {
        HDR_DBG_HAL_CFD(printf("\nError:SDR IP cannot support this process, check IO of SDR IP\n"));
        HDR_DBG_HAL_CFD(printf("Error code = %04x !!!, error is in [ %s  , %d]\n",u16Check_status, __FUNCTION__,__LINE__));
    }

    return u16Check_status;
}

#if ((NowHW == Curry) || (NowHW == Kastor) || (NowHW == Kano))
MS_U16 MS_Cfd_STB_CheckSDRIPProcessIsOk(STU_CFDAPI_MAIN_CONTROL *pstApiControlParamInit, STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam, MS_U8 u8_IP_capability)
{
    MS_U16 u16Check_status = E_CFD_MC_ERR_NOERR;

    //for Curry SDR IP
    //can not do
    //2.wrong TMO settings

    //if (1 == pstu_ControlParam->u8Input_SDRIPMode)
    {


#if 0   //this part is only for TV cases
        if ((( E_CFD_MC_FORMAT_RGB != pstu_ControlParam->u8Output_DataFormat) && (E_CFD_MS_SC0_MAIN == pstu_ControlParam->u8HW_MainSub_Mode)) ||
            (E_CFD_MS_SC1_SUB == pstu_ControlParam->u8HW_MainSub_Mode))
        {
            if ((E_CFD_MC_FORMAT_RGB != pstu_ControlParam->u8Temp_DataFormat[1]) && (E_CFD_CFIO_YUV_BT2020_CL == pstu_ControlParam->u8Temp_Format[1]) && (E_CFD_CFIO_YUV_BT2020_CL != pstu_ControlParam->u8Output_Format))
            {

                HDR_DBG_HAL_CFD(printf("\n Error: Current SDR IP group can not support YUV out and Do BT2020CL handle at the same time\n"));
                u16Check_status = E_CFD_MC_ERR_HW_IPS_BT2020CLtoNCL_NOTSUPPORTED;

                HDR_DBG_HAL_CFD(printf("\n Force: output format is the same as input format of SDR IP\n"));
                pstu_ControlParam->u8DoSDRIP_ForceNOBT2020CL_Flag = 1;

                pstu_ControlParam->u8Output_Format = E_CFD_CFIO_YUV_BT2020_CL;
                pstu_ControlParam->u8OutputColorPriamries = pstu_ControlParam->u8TempColorPriamries[1];
                pstu_ControlParam->u8OutputTransferCharacterstics = pstu_ControlParam->u8TempTransferCharacterstics[1];
                pstu_ControlParam->u8OutputMatrixCoefficients = pstu_ControlParam->u8TempMatrixCoefficients[1];
                pstu_ControlParam->u8Output_GamutOrderIdx = pstu_ControlParam->u8Temp_GamutOrderIdx[1];
            }

            if (pstu_ControlParam->u8Output_GamutOrderIdx != pstu_ControlParam->u8Temp_GamutOrderIdx[1])
            {

                HDR_DBG_HAL_CFD(printf("\n Force:Current SDR IP can not support YUV out and Do GM at the same time\n"));
                u16Check_status = E_CFD_MC_ERR_HW_IPS_GM_NOTSUPPORTED;

                HDR_DBG_HAL_CFD(printf("\n Force: output format is the same as input format of SDR IP\n"));
                pstu_ControlParam->u8DoSDRIP_ForceNOGM_Flag = 1;

                pstu_ControlParam->u8Output_Format = pstu_ControlParam->u8Temp_Format[1];
                pstu_ControlParam->u8OutputColorPriamries = pstu_ControlParam->u8TempColorPriamries[1];
                pstu_ControlParam->u8OutputTransferCharacterstics = pstu_ControlParam->u8TempTransferCharacterstics[1];
                pstu_ControlParam->u8OutputMatrixCoefficients = pstu_ControlParam->u8TempMatrixCoefficients[1];
                pstu_ControlParam->u8Output_GamutOrderIdx = pstu_ControlParam->u8Temp_GamutOrderIdx[1];
            }

            if ((pstu_ControlParam->u8Temp_HDRMode[1] != pstApiControlParamInit->u8Output_HDRMode) && ( E_CFIO_MODE_SDR == pstApiControlParamInit->u8Output_HDRMode))
            {

                HDR_DBG_HAL_CFD(printf("\n Force:Current SDR IP can not support YUV out and Do TMO\n"));
                u16Check_status = E_CFD_MC_ERR_HW_IPS_TMO_NOTSUPPORTED;

                HDR_DBG_HAL_CFD(printf("\n Force: output format HDR mode is the same as input HDR mode of SDR IP\n"));
                pstu_ControlParam->u8DoSDRIP_ForceNOTMO_Flag = 1;

                pstApiControlParamInit->u8Output_HDRMode = pstu_ControlParam->u8Temp_HDRMode[1];
            }
        }
#endif

        //bit[1] of u8_IP_capability
        //Gamut mapping is supported or not
        if ((pstu_ControlParam->u8Output_GamutOrderIdx != pstu_ControlParam->u8Temp_GamutOrderIdx[1]) && (0x00 == (u8_IP_capability&0x02)))
        {
            if (0 == pstu_ControlParam->u8XVYCC2SRGBFlag)
            {
                HDR_DBG_HAL_CFD(printf("\n Force:Current SDR IP can not support GM \n"));
                u16Check_status = E_CFD_MC_ERR_HW_IPS_GM_NOTSUPPORTED;

                HDR_DBG_HAL_CFD(printf("\n Force: output format is the same as input format of SDR IP\n"));
                //pstu_ControlParam->u8DoSDRIP_ForceNOGM_Flag = 1;

                pstu_ControlParam->u8Output_Format = pstu_ControlParam->u8Temp_Format[1];
                pstu_ControlParam->u8OutputColorPriamries = pstu_ControlParam->u8TempColorPriamries[1];
                pstu_ControlParam->u8OutputTransferCharacterstics = pstu_ControlParam->u8TempTransferCharacterstics[1];
                pstu_ControlParam->u8OutputMatrixCoefficients = pstu_ControlParam->u8TempMatrixCoefficients[1];
                pstu_ControlParam->u8Output_GamutOrderIdx = pstu_ControlParam->u8Temp_GamutOrderIdx[1];
            }
        }

        //bit[2] of u8_IP_capability
        //TMO is supported or not
        if ((pstu_ControlParam->u8Temp_HDRMode[1] != pstApiControlParamInit->u8Output_HDRMode) && ( E_CFIO_MODE_SDR == pstApiControlParamInit->u8Output_HDRMode) && (0x00 == (u8_IP_capability&0x04)))
        {

                HDR_DBG_HAL_CFD(printf("\n Force:Current SDR IP can not support TMO\n"));
                u16Check_status = E_CFD_MC_ERR_HW_IPS_TMO_NOTSUPPORTED;

                HDR_DBG_HAL_CFD(printf("\n Force: output format HDR mode is the same as input HDR mode of SDR IP\n"));
                //pstu_ControlParam->u8DoSDRIP_ForceNOTMO_Flag = 1;

                pstApiControlParamInit->u8Output_HDRMode = pstu_ControlParam->u8Temp_HDRMode[1];
        }


        //bit[3] of u8_IP_capability
        //YUV BT2020CL to NCL is supported or not
        if ((E_CFD_CFIO_YUV_BT2020_CL == pstu_ControlParam->u8Temp_Format[1]) && (E_CFD_CFIO_YUV_BT2020_CL != pstu_ControlParam->u8Output_Format) && (0x00 == (u8_IP_capability&0x08)))
        {

                //force Curry to handle this case
                //HDR_DBG_HAL_CFD(printf("\n Error: Current SDR IP group can not support YUV out and Do BT2020CL handle at the same time\n"));
                //u16Check_status = E_CFD_MC_ERR_HW_IPS_BT2020CLtoNCL_NOTSUPPORTED;

                //HDR_DBG_HAL_CFD(printf("\n Force: output format is the same as input format of SDR IP\n"));
                //pstu_ControlParam->u8DoSDRIP_ForceNOBT2020CL_Flag = 1;

                //pstu_ControlParam->u8Output_Format = E_CFD_CFIO_YUV_BT2020_CL;
                //pstu_ControlParam->u8OutputColorPriamries = pstu_ControlParam->u8TempColorPriamries[1];
                //pstu_ControlParam->u8OutputTransferCharacterstics = pstu_ControlParam->u8TempTransferCharacterstics[1];
                //pstu_ControlParam->u8OutputMatrixCoefficients = pstu_ControlParam->u8TempMatrixCoefficients[1];
                //pstu_ControlParam->u8Output_GamutOrderIdx = pstu_ControlParam->u8Temp_GamutOrderIdx[1];
        }

        //case 2: wrong TMO : SDR to HDR
        if ((E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[1]) && (E_CFIO_MODE_SDR != pstApiControlParamInit->u8Output_HDRMode))
        {

            HDR_DBG_HAL_CFD(printf("\nForce:Current SDR IP can not support SDR to HDR\n"));
            u16Check_status = E_CFD_MC_ERR_HW_IPS_SDR2HDR_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printf("\n Force: output format HDR mode is the same as input HDR mode of SDR IP\n"));
            pstu_ControlParam->u8DoSDRIP_ForceNOTMO_Flag = 1;

            pstApiControlParamInit->u8Output_HDRMode = pstu_ControlParam->u8Temp_HDRMode[1];
        }

        //if ((E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Temp_HDRMode[1]) && (E_CFIO_MODE_HDR1 == pstApiControlParamInit->u8Output_HDRMode))
        if ((E_CFIO_MODE_SDR != pstu_ControlParam->u8Temp_HDRMode[1]) &&
            (E_CFIO_MODE_SDR != pstApiControlParamInit->u8Output_HDRMode) &&
            (pstu_ControlParam->u8Temp_HDRMode[1] != pstApiControlParamInit->u8Output_HDRMode))
        {

            HDR_DBG_HAL_CFD(printf("\nForce:Current SDR IP can not support HDR2 to HDR1\n"));
            u16Check_status = E_CFD_MC_ERR_HW_IPS_HDRXtoHDRY_NOTSUPPORTED;

            HDR_DBG_HAL_CFD(printf("\n Force: output format HDR mode is the same as input HDR mode of SDR IP\n"));
            pstu_ControlParam->u8DoSDRIP_ForceNOTMO_Flag = 1;

            pstApiControlParamInit->u8Output_HDRMode = pstu_ControlParam->u8Temp_HDRMode[1];
        }

    }

    if (E_CFD_MC_ERR_NOERR != u16Check_status)
    {
        HDR_DBG_HAL_CFD(printf("\nError:SDR IP cannot support this process, check IO of SDR IP\n"));
        HDR_DBG_HAL_CFD(printf("Error code = %04x !!!, error is in [ %s  , %d]\n",u16Check_status, __FUNCTION__,__LINE__));
    }

    return u16Check_status;
}
#endif

void MS_Cfd_WriteBack_Control(STU_CFDAPI_TOP_CONTROL *pstu_Cfd_api_top,STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam)
{
    MS_U8 u8CFDversion = 0x01;
    pstu_Cfd_api_top->pstu_Main_Control->u8Input_Format = pstu_ControlParam->u8Input_Format;
    pstu_Cfd_api_top->pstu_Main_Control->u8Input_DataFormat = pstu_ControlParam->u8Input_DataFormat;
    pstu_Cfd_api_top->pstu_Main_Control->u8Input_IsFullRange = pstu_ControlParam->u8Input_IsFullRange;
    pstu_Cfd_api_top->pstu_Main_Control->u8Input_HDRMode = pstu_ControlParam->u8Input_HDRMode;
    pstu_Cfd_api_top->pstu_Main_Control->stu_Middle_Format[0].u8Mid_Format = pstu_ControlParam->u8Temp_Format[1];
    pstu_Cfd_api_top->pstu_Main_Control->stu_Middle_Format[0].u8Mid_DataFormat = pstu_ControlParam->u8Temp_DataFormat[1];
    pstu_Cfd_api_top->pstu_Main_Control->stu_Middle_Format[0].u8Mid_IsFullRange = pstu_ControlParam->u8Temp_IsFullRange[1];
    pstu_Cfd_api_top->pstu_Main_Control->stu_Middle_Format[0].u8Mid_HDRMode = pstu_ControlParam->u8Temp_HDRMode[1];

#if RealChip
#if (1 == DUMMY_BANK30_EXIST)
    if(MApi_GFLIP_XC_R2BYTE(REG_SC_BK30_01_L) == 0x001C)
    {
        MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_02_L, ((pstu_ControlParam->u8Input_Format<<8)+pstu_ControlParam->u8Input_DataFormat));
        MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_03_L, ((pstu_ControlParam->u8Input_SDRIPMode<<8)+pstu_ControlParam->u8Input_HDRMode));
        MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_04_L, ((pstu_ControlParam->u8InputColorPriamries<<8)+pstu_ControlParam->u8InputTransferCharacterstics));
        MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_06_L, ((pstu_ControlParam->u8InputMatrixCoefficients<<8)+pstu_ControlParam->u8Input_IsFullRange));
        MApi_GFLIP_XC_W2BYTE(REG_SC_BK30_08_L, ((pstu_ControlParam->u8Input_IsRGBBypass<<8)+u8CFDversion));
    }
#endif
#endif
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

    MS_U8 u8idxub = 0;

    //set

    //stu_ControlParam.u8InputFormat
    //stu_ControlParam.u8Input_DataFormat
    //stu_ControlParam.u8InputIsFullRange
    //stu_ControlParam.u8InputColorPriamries
    //stu_ControlParam.u8InputTransferCharacterstics
    //stu_ControlParam.u8InputMatrixCoefficients

    //pstu_ControlParam->u8Input_DataFormat = pstu_Main_Control->u8Input_DataFormat;
    //pstu_ControlParam->u8Input_Format     = pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace;

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

#if 0
    u8idxub = sizeof (u8Const_LUT_MMInfor_HDMIDefinition) / (sizeof(MS_U8)*5);

    for (u8idx =0;u8idx< u8idxub ;u8idx++)
    {
        if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3] == pstu_ControlParam->u8Input_Format)||
            (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4] == pstu_ControlParam->u8Input_Format))
        {
            break;
        }
    }
#else
    u8idx = MS_Cfd_FindMatchedIdx(pstu_ControlParam->u8Input_Format);
#endif

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
    MS_U8 u8idxub = 0;

    //set

    //stu_ControlParam.u8InputFormat
    //stu_ControlParam.u8Input_DataFormat
    //stu_ControlParam.u8InputIsFullRange
    //stu_ControlParam.u8InputColorPriamries
    //stu_ControlParam.u8InputTransferCharacterstics
    //stu_ControlParam.u8InputMatrixCoefficients

    //pstu_ControlParam->u8Input_DataFormat = pstu_Main_Control->u8Input_DataFormat;
    //pstu_ControlParam->u8Input_Format     = pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace;

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

        HDR_DBG_HAL_CFD(printf("warning forceto709 happens in [ %s  , %d]\n", __FUNCTION__,__LINE__));
    }

    //assign u8InputIsFullRange
    //u8HDMISource_Support_Format = {Q1 Q0},{YQ1 YQ0},{Y2 Y1 Y0}

    //
    //assign
    //stu_ControlParam.u8InputColorPriamries
    //stu_ControlParam.u8InputTransferCharacterstics
    //stu_ControlParam.u8InputMatrixCoefficients
#if 0
    u8idxub = sizeof (u8Const_LUT_MMInfor_HDMIDefinition) / (sizeof(MS_U8)*5);

    for (u8idx =0; u8idx < u8idxub ;u8idx++)
    {
        if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3] == pstu_ControlParam->u8Input_Format)||
            (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4] == pstu_ControlParam->u8Input_Format))
        {
            break;
        }
    }
#else
    u8idx = MS_Cfd_FindMatchedIdx(pstu_ControlParam->u8Input_Format);
#endif
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

MS_U8 MS_Cfd_OutputHDMI_SetColorMetry_lite_out(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam,
    STU_CFDAPI_MAIN_CONTROL *pstu_Main_Control
    )
{
    MS_U8 u8Check_Status = 0;
    MS_U8 u8idx = 0;
    MS_U8 u8idxub = 0;

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

        HDR_DBG_HAL_CFD(printf("warning forceto709 happens in [ %s  , %d]\n", __FUNCTION__,__LINE__));
    }

    //assign
    //stu_ControlParam.u8InputColorPriamries
    //stu_ControlParam.u8InputTransferCharacterstics
    //stu_ControlParam.u8InputMatrixCoefficients
#if 0
    u8idxub = sizeof (u8Const_LUT_MMInfor_HDMIDefinition) / (sizeof(MS_U8)*5);

    for (u8idx =0; u8idx < u8idxub ;u8idx++)
    {
        if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3] == pstu_ControlParam->u8Output_Format)||
            (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4] == pstu_ControlParam->u8Output_Format))
        {
            break;
        }
    }
#else
    u8idx = MS_Cfd_FindMatchedIdx(pstu_ControlParam->u8Output_Format);
#endif
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

//this function is only for Kano and Curry
MS_U8 MS_Cfd_OutputHDMI_SetColorMetry_lite_out2(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam,STU_CFDAPI_MAIN_CONTROL *pstu_Main_Control)
{
    MS_U8 u8Check_Status = 0;
    MS_U8 u8idx = 0;
    MS_U8 u8mode;

    MS_U8 u8idxub = 0;

    u8mode = pstu_Main_Control->u8MMInput_ColorimetryHandle_Mode;


    if ((0 == u8mode)||(3 == u8mode)&& ((E_CFD_INPUT_SOURCE_STORAGE == pstu_Main_Control->u8Input_Source)||
        (E_CFD_INPUT_SOURCE_DTV == pstu_Main_Control->u8Input_Source)||
        (E_CFD_INPUT_SOURCE_JPEG == pstu_Main_Control->u8Input_Source)))
    {
        //force to a specific color for reserved case
        if (E_CFD_CFIO_RESERVED_START <= pstu_ControlParam->u8Output_Format)
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

            printf("\033[1;35m###[CFD][%s][%d]### forceto709\033[0m\n",__FUNCTION__,__LINE__);
        }

        //assign
        //stu_ControlParam.u8InputColorPriamries
        //stu_ControlParam.u8InputTransferCharacterstics
        //stu_ControlParam.u8InputMatrixCoefficients

        pstu_ControlParam->u8OutputColorPriamries = pstu_ControlParam->u8TempColorPriamries[1];
        pstu_ControlParam->u8OutputTransferCharacterstics = pstu_ControlParam->u8TempTransferCharacterstics[1];
        pstu_ControlParam->u8OutputMatrixCoefficients = pstu_ControlParam->u8TempMatrixCoefficients[1];

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
        //else if (E_CFIO_MODE_SDR == pstu_ControlParam->u8Output_HDRMode)
        //{
        //  pstu_ControlParam->u8OutputTransferCharacterstics = E_CFD_CFIO_TR_GAMMA2P2;
        //}
    }
    else
    {
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

            //HDR_DBG_HAL_CFD(printk("warning forceto709 happens in [ %s  , %d]\n", __FUNCTION__,__LINE__));
            printf("\033[1;35m###[CFD][%s][%d]### forceto709\033[0m\n",__FUNCTION__,__LINE__);
        }

        //assign
        //stu_ControlParam.u8InputColorPriamries
        //stu_ControlParam.u8InputTransferCharacterstics
        //stu_ControlParam.u8InputMatrixCoefficients
#if 0
        u8idxub = sizeof (u8Const_LUT_MMInfor_HDMIDefinition) / (sizeof(MS_U8)*5);

        for (u8idx =0; u8idx < u8idxub ;u8idx++)
        {
            if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3] == pstu_ControlParam->u8Output_Format)||
                (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4] == pstu_ControlParam->u8Output_Format))
            {
                break;
            }
        }
#else
        u8idx = MS_Cfd_FindMatchedIdx(pstu_ControlParam->u8Output_Format);
#endif
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
    }

    //0:current HDMI input is one case of HDMI colormetry
    //1:current HDMI input is not one case of HDMI colormetry,
    return u8Check_Status;
}



MS_U16 MS_Cfd_Maserati_Dolby_Control(STU_CFDAPI_DOLBY_CONTROL *pstu_Dolby_Param)
{
    // do nothing here
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

#if (NowHW == Kano)
        if ((E_CFD_CFIO_YUV_BT2020_CL == pstu_ControlParam->u8Temp_Format[0]) && (E_CFD_CFIO_YUV_BT2020_CL != pstu_ControlParam->u8Temp_Format[1]))
        {
            pstu_ControlParam->u8DoBT2020CLPInHDRIP_Flag = 1;
        }
        else
        {
            pstu_ControlParam->u8DoBT2020CLPInHDRIP_Flag = 0;
        }
#else
        //current design is not supported this process
        pstu_ControlParam->u8DoBT2020CLPInHDRIP_Flag = 0;
#endif

        if (pstu_ControlParam->u8Temp_GamutOrderIdx[1] != pstu_ControlParam->u8Temp_GamutOrderIdx[0] )
        {
            if ((E_CFD_CFIO_GT_BT709 == pstu_ControlParam->u8Temp_GamutOrderIdx[0] ) &&
                (E_CFD_CFIO_GT_XVYCC == pstu_ControlParam->u8Temp_GamutOrderIdx[1]  ))
            {
                pstu_ControlParam->u8DoGamutMappingInHDRIP_Flag = 0;
            }
            else
            {
                pstu_ControlParam->u8DoGamutMappingInHDRIP_Flag = 1;

                //this process is only for this project, not a general handle
                if (MS_Cfd_CheckOpenHDR(pstu_ControlParam->u8Temp_HDRMode[1]))
                {
                    pstu_ControlParam->u8DoGamutMappingInHDRIP_Flag = 0;
                }

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
            //pstu_ControlParam->u8DoTMOInHDRIP_Flag = 0;
            //only support this case : SDR to PQ
            if ((E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[0] ) && (E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Temp_HDRMode[1] ))
            {
                pstu_ControlParam->u8DoSDRtoHDR_Flag = 1;
            }
            else
            {
                pstu_ControlParam->u8DoSDRtoHDR_Flag = 0;
                u16_check_status = E_CFD_MC_ERR_HW_IPS_SDR2HDR_NOTSUPPORTED;
            }
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
                pstu_ControlParam->u8DoHDRXtoHDRY_Flag = 1;
                //u16_check_status = E_CFD_MC_ERR_HW_IPS_HDRXtoHDRY_NOTSUPPORTED;
            }
        }
        else
        {
            pstu_ControlParam->u8DoTMOInHDRIP_Flag = 0;
        }

        if ((E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Temp_HDRMode[0] ) && (E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Temp_HDRMode[1] ))
        {
            pstu_ControlParam->u8DoHDRbypassInHDRIP_Flag = 1;
        }

        if ((E_CFIO_MODE_HDR3 == pstu_ControlParam->u8Temp_HDRMode[0] ) && (E_CFIO_MODE_HDR3 == pstu_ControlParam->u8Temp_HDRMode[1] ))
        {
            pstu_ControlParam->u8DoHDRbypassInHDRIP_Flag = 2;
        }

#if (NowHW == Maserati) || (NowHW == Mainz)
        //force image enter HDRIP when input_source = HDMI
        if (E_CFD_INPUT_SOURCE_HDMI == pstu_ControlParam->u8Input_Source)
        {
            pstu_ControlParam->u8DoForceEnterHDRIP_Flag = 1;
        }
        else
        {
            pstu_ControlParam->u8DoForceEnterHDRIP_Flag = 0;
        }
#endif

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

    if ((pstu_ControlParam->u8Output_GamutOrderIdx != pstu_ControlParam->u8Temp_GamutOrderIdx[1])) // && (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_DataFormat))
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
        printf("\n Error:sothing wrong in set TMO GM flags function \n");
        printf("\n Error code = %04x !!!, error is in [ %s  , %d] \n",u16_check_status, __FUNCTION__,__LINE__);
    }

    return u16_check_status;

}

#if (NowHW == Maserati) || (NowHW == Mainz)
void Restore_input_bank_and_log_Maserati(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param)
{

#if RealChip
    msDlc_FunctionEnter();
    if(1 == GetDlcFunctionControl())
    {
        //msWriteByte(REG_SC_Ali_BK30_01_L,pstApiControlParamInit->u8Process_Mode );//0x1     ;     //0:off 1:on - normal mode 2:on - test mode
        //pstControlParamInit->u8HW_Structure                             = ;//E_CFD_HWS_STB_TYPE1 ;     //assign by E_CFD_MC_HW_STRUCTURE
        //pstControlParamInit->u8HW_PatchEn                               = ;//0x1     ;     //0: patch off, 1:patch on
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
    printf("u8Input_Source                          :%d REG_SC_Ali_BK30_01_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_Source                       );
    //printf("u8Input_AnalogIdx                     :%d REG_SC_Ali_BK30_02_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_AnalogIdx                    );
    printf("u8Input_Format                          :%d REG_SC_Ali_BK30_02_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_Format                       );
    printf("u8Input_DataFormat                      :%d REG_SC_Ali_BK30_03_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_DataFormat                   );
    printf("u8Input_IsFullRange                     :%d REG_SC_Ali_BK30_03_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_IsFullRange                  );
    printf("u8Input_HDRMode                         :%d REG_SC_Ali_BK30_04_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_HDRMode                      );
    printf("u8Input_IsRGBBypass                     :%d REG_SC_Ali_BK30_04_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Input_IsRGBBypass                  );
    printf("u8Output_Source                         :%d REG_SC_Ali_BK30_05_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_Source                      );
    printf("u8Output_Format                         :%d REG_SC_Ali_BK30_05_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_Format                      );
    printf("u8Output_DataFormat                     :%d REG_SC_Ali_BK30_06_L \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_DataFormat                  );
    printf("u8Output_IsFullRange                    :%d REG_SC_Ali_BK30_06_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_IsFullRange                 );
    printf("u8Output_HDRMode                        :%d REG_SC_Ali_BK30_07_H \n", pstu_CfdAPI_Top_Param->pstu_Main_Control->u8Output_HDRMode                     );
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

//-----------------------------------------------------------------------------------------
//assign for HDRMode
//from the definition

//@param : u32define
//u32define : from CFD_IN_SDR_FORCEOUTPUT
//indicate use the predefind value or not

//output
//pstControlParamInit
//------------------------------------------------------------------------------------------

void CFD_in_PreConstraints_ForceHDRMode(STU_CFDAPI_MAIN_CONTROL *pstControlParamInit, MS_U32 u32define)
{
    pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = (MS_U8)u32define;
    pstControlParamInit->u8Output_HDRMode = (MS_U8)u32define;
}

//-----------------------------------------------------------------------------------------
//set HDRmode depends on the u8inputHDRMode

//@param : u8SetMode
//0:set SDR
//1:set u8inputHDRMode
//2:set u8OutputHDRMode
//else:SDR

//@param : u8OutputHDRMode
//@param : u8inputHDRMode
//output
//u8HDRMode
//------------------------------------------------------------------------------------------
MS_U8 CFD_in_SetHDRMode_byMode(MS_U8 u8SetMode, MS_U8 u8OutputHDRMode, MS_U8 u8inputHDRMode)
{
    MS_U8 u8HDRMode;

    if (0 == u8SetMode)
    {
        u8HDRMode = E_CFIO_MODE_SDR;
    }
    else if (1 == u8SetMode)
    {
        u8HDRMode = u8inputHDRMode;
    }
    else if (2 == u8SetMode)
    {
        u8HDRMode = u8OutputHDRMode;
    }
    else
    {
        u8HDRMode = E_CFIO_MODE_SDR;
    }

//force mode, patch for HLG
#if (CFD_SW_VERSION >= 0x00000014)
#if (0x01 == CFD_IN_SDR_FORCEOUTPUT)
    if (E_CFIO_MODE_SDR == u8inputHDRMode)
    {
        u8HDRMode = CFD_IN_SDR_OUTPUT;
    }
#endif

#if (0x01 == CFD_IN_DOLBY_FORCEOUTPUT)
    if (E_CFIO_MODE_HDR1 == u8inputHDRMode)
    {
        u8HDRMode = CFD_IN_DOLBY_OUTPUT;
    }
#endif

#if (0x01 == CFD_IN_HDR10_FORCEOUTPUT)
    if (E_CFIO_MODE_HDR2 == u8inputHDRMode)
    {
        u8HDRMode = CFD_IN_HDR10_OUTPUT;
    }
#endif

#if (0x01 == CFD_IN_HLG_FORCEOUTPUT)
    if (1 != u8SetMode) {
        if (E_CFIO_MODE_HDR3 == u8inputHDRMode)
        {
            u8HDRMode = CFD_IN_HLG_OUTPUT;
        }
    }
#endif
#endif

    return u8HDRMode;
}

//only for HDR input
MS_U8 CFD_in_SetHDRMode_TopbyUI(MS_U8 u8SetMode, MS_U8 u8OutputHDRMode, MS_U8 u8inputHDRMode, MS_U8 u8HDRUI_H2SMode)
{
    MS_U8 u8HDRMode;

#if (CFD_SW_VERSION >= 0x00000014)

    if (0 == u8HDRUI_H2SMode) //auto
    {
        u8HDRMode = CFD_in_SetHDRMode_byMode(u8SetMode,u8OutputHDRMode,u8inputHDRMode);
    }
    else if (1 == u8HDRUI_H2SMode) //on : force H2S
    {
        u8HDRMode = CFD_in_SetHDRMode_byMode(0,u8OutputHDRMode, u8inputHDRMode);
    }
    else if (2 == u8HDRUI_H2SMode) //off : force not H2S
    {
        u8HDRMode = CFD_in_SetHDRMode_byMode(1,u8OutputHDRMode, u8inputHDRMode);
    }
    else
    {
        u8HDRMode = CFD_in_SetHDRMode_byMode(u8SetMode,u8OutputHDRMode, u8inputHDRMode);
    }

#else

    u8HDRMode = CFD_in_SetHDRMode_byMode(u8SetMode, u8OutputHDRMode, u8inputHDRMode);

#endif

    return u8HDRMode;
}

MS_U8 Mapi_CFD_DecideOutputHDRMode_HDMITX(STU_CFDAPI_MAIN_CONTROL *pstControlParamInit, STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParamInit, STU_CFDAPI_OSD_CONTROL  *pstu_OSD_Param)
{
    MS_U8 u8HdrMode;
    MS_U8 u8InputIdx;
    MS_U8 u8OutputIdx;

    MS_U8 u8OutputSupportPQ;
    MS_U8 u8OutputSupportHLG;
    MS_U8 u8temp;

    u8HdrMode = 0;
    u8InputIdx = 0;
    u8OutputIdx = 0;

    u8OutputSupportPQ = 0;
    u8OutputSupportHLG = 0;

    //find input
    //input HDRmode is assigned from Mhal_xc flow
    switch(pstControlParamInit->u8Input_HDRMode)
    {
        case E_CFIO_MODE_SDR:
            u8InputIdx = 0;
            break;
        case E_CFIO_MODE_HDR2:
            u8InputIdx = 1;
            break;
        case E_CFIO_MODE_HDR3:
            u8InputIdx = 2;
            break;
        default :
            u8InputIdx = 0;
            break;
    }

    if ((E_CFD_VALID == pstHDMI_EDIDParamInit->u8HDMISink_EDID_Valid ) &&
        (E_CFD_VALID == pstHDMI_EDIDParamInit->u8HDMISink_HDRData_Block_Valid) &&
        (2 <= pstHDMI_EDIDParamInit->u8HDMISink_HDRData_Block_Length))
    {
        if ((0x04 == (pstHDMI_EDIDParamInit->u8HDMISink_EOTF&0x04))) //SMPTE 2084
        {
            u8OutputSupportPQ = 1;
        }
        else
        {
            u8OutputSupportPQ = 0;
        }

        if ((0x08 == (pstHDMI_EDIDParamInit->u8HDMISink_EOTF&0x08))) // HLG
        {
            u8OutputSupportHLG = 1;
        }
        else
        {
            u8OutputSupportHLG = 0;
        }
    }
    else
    {
        u8OutputSupportPQ = 0;
        u8OutputSupportHLG = 0;
    }



    //find output
#if 0
    u8temp = ((u8OutputSupportPQ<<1) + u8OutputSupportHLG);
    switch(u8temp)
    {
        case 0:
            u8OutputIdx = 0;
            break;
        case 1:
            u8OutputIdx = 1;
            break;
        case 2:
            u8OutputIdx = 2;
            break;
        case 3:
            u8OutputIdx = 3;
            break;
        default :
            u8OutputIdx = 0;
            break;
    }
#else
    u8OutputIdx = ((u8OutputSupportPQ<<1) + u8OutputSupportHLG);
#endif

    if (1 == pstControlParamInit->stu_seamless.u8seamless_en)
    {
        u8HdrMode = u8Const_LUT_output_HDRmode_seamless[u8InputIdx][u8OutputIdx];
    }
    else
    {
        u8HdrMode = u8Const_LUT_output_HDRmode_noseamless[u8InputIdx][u8OutputIdx];
    }

    //1:always do HDR2SDR for HDR input
    //2:always not do HDR2SDR for HDR input
    if (0 != pstu_OSD_Param->u8HDR_UI_H2SMode)
    {
        if (1 == pstu_OSD_Param->u8HDR_UI_H2SMode)
        {
            if (MS_Cfd_CheckOpenHDR(pstControlParamInit->u8Input_HDRMode))
            {
                u8HdrMode = E_CFIO_MODE_SDR;
            }
        }
        else if (2 == pstu_OSD_Param->u8HDR_UI_H2SMode)
        {
            if ((MS_Cfd_CheckOpenHDR(pstControlParamInit->u8Input_HDRMode)) && (E_CFIO_MODE_SDR == u8HdrMode))
            {
                u8HdrMode = pstControlParamInit->u8Input_HDRMode;
            }
        }
    }
    //no update for u8HDR_UI_H2SMode = 0;

    return u8HdrMode;
}

#if ((NowHW == Curry) || (NowHW == Kastor))
MS_U16 Mapi_Cfd_inter_Main_Control_PreConstraints_Curry(STU_CFDAPI_MAIN_CONTROL *pstControlParamInit, STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam, STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParamInit, MS_U8 u8TempMode, STU_CFDAPI_OSD_CONTROL  *pstu_OSD_Param)
{

    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status_tmp = E_CFD_MC_ERR_NOERR;
    //MS_U16 u16Temp;

    MS_U8 u8flag;
    u8flag = 0;

    //force not used in STB
    if (1 == pstControlParamInit->u8Input_IsRGBBypass)
    {
        pstControlParamInit->u8Input_IsRGBBypass = 0;
        HDR_DBG_HAL_CFD(printk("\nForce:Current HW can not support u8Input_IsRGBBypass = 1, force u8Input_IsRGBBypass to 0\n"));
        u16_check_status_tmp = E_CFD_MC_ERR_INPUT_ISRGBBYPASS_FORCE0;
    }

//force no gamut mapping for SDR input
#if ((CFD_SW_VERSION >= 0x00000017) && (0 == CFD_SDR_withGM))
   if ((3 != pstControlParamInit->u8HDMIOutput_GammutMapping_Mode) && (E_CFIO_MODE_SDR == pstControlParamInit->u8Input_HDRMode))
   {
       pstControlParamInit->u8HDMIOutput_GammutMapping_Mode = 0;
   }
#endif

    //RGB
    if (E_CFD_MC_FORMAT_RGB == pstControlParamInit->u8Input_DataFormat)
    {
        if (1 == pstControlParamInit->u8Input_IsRGBBypass)
        {
            if (E_CFIO_MODE_SDR == pstControlParamInit->u8Input_HDRMode)
            {

                pstControlParamInit->u8Input_HDRIPMode = 1;
                pstControlParamInit->u8Input_SDRIPMode = 1;

                if (0 == u8TempMode)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                }
                else
                {
                    //from MS_Cfd_SetFormat_FromGamutOrderIdx()
                }

                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_RGB;
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;

#if 0
                if (HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }
#else
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
#endif

                if (0 == u8TempMode)
                {
                    pstControlParamInit->u8Output_Format = pstControlParamInit->u8Input_Format;
                }
                else
                {
                    pstControlParamInit->u8Output_Format = pstControlParamInit->stu_Middle_Format->u8Mid_Format;
                }
                pstControlParamInit->u8Output_DataFormat =  E_CFD_MC_FORMAT_RGB;
                pstControlParamInit->u8Output_HDRMode = E_CFIO_MODE_SDR;
                pstControlParamInit->u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;
            }
            //else if (E_CFIO_MODE_HDR2 == pstControlParamInit->u8Input_HDRMode)
            else if (MS_Cfd_CheckOpenHDR(pstControlParamInit->u8Input_HDRMode))
            {
                pstControlParamInit->u8Input_HDRIPMode = 1;
                pstControlParamInit->u8Input_SDRIPMode = 1;
                //pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                //pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_RGB;
                //pstControlParamInit->stu_Middle_Format->u8Mid_Format = E_CFD_CFIO_YUV_BT709;
                if (0 == u8TempMode)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_Format = MS_Cfd_SetFormat_OnlyR2Y(pstControlParamInit->u8Input_Format);
                }
                else
                {
                    //from MS_Cfd_SetFormat_FromGamutOrderIdx()
                }
                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;

                if (HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }

                if (0 == u8TempMode)
                {
                    pstControlParamInit->u8Output_Format = pstControlParamInit->u8Input_Format;
                }
                else
                {
                    //pstControlParamInit->u8Output_Format = pstControlParamInit->stu_Middle_Format->u8Mid_Format;
                    pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format,pstu_ControlParam);
                }
                pstControlParamInit->u8Output_DataFormat =  E_CFD_MC_FORMAT_RGB;
                //pstControlParamInit->u8Output_HDRMode = E_CFIO_MODE_SDR;
                pstControlParamInit->u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;


#if (1 == PRECONSTRAINTS_REFERTO_EDID)
    if (E_CFD_OUTPUT_SOURCE_HDMI == pstControlParamInit->u8Output_Source)
    {
        if ((1 == pstHDMI_EDIDParamInit->u8HDMISink_EDID_Valid )&& (1 == pstHDMI_EDIDParamInit->u8HDMISink_HDRData_Block_Valid))
        {
                if ((0x04 == (pstHDMI_EDIDParamInit->u8HDMISink_EOTF&0x04)) && (E_CFIO_MODE_HDR2 == pstControlParamInit->u8Input_HDRMode)) //SMPTE 2084
                {
                        pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(1,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
                        u8flag = 1;
                }
                else if ((0x08 == (pstHDMI_EDIDParamInit->u8HDMISink_EOTF&0x08)) && (E_CFIO_MODE_HDR3 == pstControlParamInit->u8Input_HDRMode)) // HLG
                {
                        pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(1,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
                        u8flag = 1;
                }
        }
    }
#endif

    //SDR TV
    if (0 == u8flag)
    {
        if (0 == pstu_OSD_Param->u8HDR_UI_H2SMode)
        {
            pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(0,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
        }
        else
        {
            pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(1,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
        }
    }


#if 1
                //HDR to SDR case
                if (E_CFIO_MODE_SDR == pstControlParamInit->u8Output_HDRMode)
                {
#if 0
                    pstControlParamInit->u8Output_HDRMode = pstControlParamInit->u8Input_HDRMode;
                    pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = pstControlParamInit->u8Input_HDRMode;

                    //not support this case, return error
                    u16_check_status_tmp = E_CFD_MC_ERR_HW_IPS_TMO_INRGB_NOTSUPPORTED;
                    HDR_DBG_HAL_CFD(printk("\nForce:Current HW can not support HDR RGB in, Force bypass\n"));
                    pstu_ControlParam->u8DoHDRIP_Forcebypass_Flag = 1;
                    pstControlParamInit->u8Input_HDRIPMode = 0;
                    pstControlParamInit->u8Input_SDRIPMode = 0;
#endif
                }
                else //HDR bypass case
                {
                    pstControlParamInit->u8Input_HDRIPMode = 0;
                    pstControlParamInit->u8Input_SDRIPMode = 0;
                }
#endif

            }
        }
        else // (0 == pstControlParamInit->u8Input_IsRGBBypass)
        {
            if (E_CFIO_MODE_SDR == pstControlParamInit->u8Input_HDRMode)
            {
                //pstControlParamInit->u8Input_HDRIPMode = 0;
                pstControlParamInit->u8Input_HDRIPMode = 1;
                pstControlParamInit->u8Input_SDRIPMode = 1;
                //pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                //pstControlParamInit->stu_Middle_Format->u8Mid_Format = MS_Cfd_SetFormat_OnlyR2Y(pstControlParamInit->u8Input_Format);
                if (0 == u8TempMode)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                }
                else
                {
                    //from MS_Cfd_SetFormat_FromGamutOrderIdx()
                }
                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_RGB;
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;

#if 0
                if (HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }
#else
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
#endif

                if (0 == u8TempMode)
                {
                pstControlParamInit->u8Output_Format = pstControlParamInit->u8Input_Format;
                }
                else
                {
                pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format,pstu_ControlParam);
                }
                pstControlParamInit->u8Output_DataFormat =  E_CFD_MC_FORMAT_RGB;
                pstControlParamInit->u8Output_HDRMode = E_CFIO_MODE_SDR;
                pstControlParamInit->u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;
            }
            //else if (E_CFIO_MODE_HDR2 == pstControlParamInit->u8Input_HDRMode)
            else if (MS_Cfd_CheckOpenHDR(pstControlParamInit->u8Input_HDRMode))
            {

                pstControlParamInit->u8Input_HDRIPMode = 1;
                pstControlParamInit->u8Input_SDRIPMode = 1;
                //pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                //pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_RGB;
                //pstControlParamInit->stu_Middle_Format->u8Mid_Format = E_CFD_CFIO_YUV_BT709;
                if (0 == u8TempMode)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_Format = MS_Cfd_SetFormat_OnlyR2Y(pstControlParamInit->u8Input_Format);
                }
                else
                {
                }
                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;

                if (HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }

                if (0 == u8TempMode)
                {
                pstControlParamInit->u8Output_Format = pstControlParamInit->u8Input_Format;
                }
                else
                {
                    pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format,pstu_ControlParam);
                }
                pstControlParamInit->u8Output_DataFormat =  E_CFD_MC_FORMAT_RGB;
                //pstControlParamInit->u8Output_HDRMode = E_CFIO_MODE_SDR;
                pstControlParamInit->u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;

#if (1 == PRECONSTRAINTS_REFERTO_EDID)

    if (E_CFD_OUTPUT_SOURCE_HDMI == pstControlParamInit->u8Output_Source)
    {
        if ((1 == pstHDMI_EDIDParamInit->u8HDMISink_EDID_Valid )&& (1 == pstHDMI_EDIDParamInit->u8HDMISink_HDRData_Block_Valid))
        {
                if ((0x04 == (pstHDMI_EDIDParamInit->u8HDMISink_EOTF&0x04)) && (E_CFIO_MODE_HDR2 == pstControlParamInit->u8Input_HDRMode)) //SMPTE 2084
                {
                        pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(1,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
                        u8flag = 1;
                }
                else if ((0x08 == (pstHDMI_EDIDParamInit->u8HDMISink_EOTF&0x08)) && (E_CFIO_MODE_HDR3 == pstControlParamInit->u8Input_HDRMode)) // HLG
                {
                        pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(1,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
                        u8flag = 1;
                }
            }
        }

#endif

//SDR TV
    if (0 == u8flag)
    {
        if (0 == pstu_OSD_Param->u8HDR_UI_H2SMode)
        {
            pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(0,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
        }
        else
        {
            pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(1,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
        }
    }

#if 1
                //HDR to SDR case
                if (E_CFIO_MODE_SDR == pstControlParamInit->u8Output_HDRMode)
                {
#if 0
                    pstControlParamInit->u8Output_HDRMode = pstControlParamInit->u8Input_HDRMode;
                    pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = pstControlParamInit->u8Input_HDRMode;

                    //not support this case, return error
                    u16_check_status_tmp = E_CFD_MC_ERR_HW_IPS_TMO_INRGB_NOTSUPPORTED;
                    HDR_DBG_HAL_CFD(printk("\nForce:Current HW can not support HDR RGB in, Force bypass\n"));
                    pstu_ControlParam->u8DoHDRIP_Forcebypass_Flag = 1;
                    pstControlParamInit->u8Input_HDRIPMode = 0;
                    pstControlParamInit->u8Input_SDRIPMode = 0;
#endif
                }
                else //HDR bypass case
                {
                    pstControlParamInit->u8Input_HDRIPMode = 0;
                    pstControlParamInit->u8Input_SDRIPMode = 0;
                }
#endif
            }
        }
    }
    else // YUV
    {

        if ((E_CFIO_MODE_SDR == pstControlParamInit->u8Input_HDRMode) && (E_CFD_CFIO_YUV_BT2020_CL != pstControlParamInit->u8Input_Format))
        {
                pstControlParamInit->u8Input_HDRIPMode = 1;
                pstControlParamInit->u8Input_SDRIPMode = 1;
                if (0 == u8TempMode)
                {
                pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                }
                else
                {

                }
                //pstControlParamInit->stu_Middle_Format->u8Mid_Format = E_CFD_CFIO_YUV_BT709;
                //pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;
                //pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;

#if 0
                if (HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }
#else
                //pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
#endif

                //pstControlParamInit->u8Output_Format = E_CFD_CFIO_RGB_NOTSPECIFIED;

#if 0           //not need to do this now
                //force RGB output now
                pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format,pstu_ControlParam);
                pstControlParamInit->u8Output_DataFormat =  E_CFD_MC_FORMAT_RGB;
                pstControlParamInit->u8Output_HDRMode = E_CFIO_MODE_SDR;
                pstControlParamInit->u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;
#endif

                if (E_CFD_OUTPUT_SOURCE_HDMI == pstControlParamInit->u8Output_Source)
                {
                    pstControlParamInit->u8Output_HDRMode = Mapi_CFD_DecideOutputHDRMode_HDMITX(pstControlParamInit, pstHDMI_EDIDParamInit, pstu_OSD_Param);
                }
                else
                {
                    pstControlParamInit->u8Output_HDRMode = E_CFIO_MODE_SDR;
                }

                //SDR bypass case
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = pstControlParamInit->u8Output_HDRMode;

                //when bypass, keep range the same as input
                if (E_CFIO_MODE_SDR == pstControlParamInit->u8Output_HDRMode)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = pstControlParamInit->u8Input_IsFullRange;
                }
                else //SDR to HDR case
                {
                    //nothing special now
                }

                //middle point must be Y
               //output is R
               if (E_CFD_MC_FORMAT_RGB == pstControlParamInit->u8Output_DataFormat)
               {
                   pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format,pstu_ControlParam);
               }
               else
               {
                   pstControlParamInit->u8Output_Format = pstControlParamInit->stu_Middle_Format->u8Mid_Format;
               }

        }
        else if ((E_CFIO_MODE_SDR == pstControlParamInit->u8Input_HDRMode) && (E_CFD_CFIO_YUV_BT2020_CL == pstControlParamInit->u8Input_Format))
        {

                pstControlParamInit->u8Input_HDRIPMode = 1;
                pstControlParamInit->u8Input_SDRIPMode = 1;
                //pstControlParamInit->stu_Middle_Format->u8Mid_Format = E_CFD_CFIO_YUV_BT2020_NCL;
                if (0 == u8TempMode)
                {
                pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                }
                else
                {
                }
                //pstControlParamInit->stu_Middle_Format->u8Mid_Format = E_CFD_CFIO_YUV_BT709;
                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;

#if 0
                if (HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }
#else
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
#endif
                //middle point must be Y
                //output is R
                if (E_CFD_MC_FORMAT_RGB == pstControlParamInit->u8Output_DataFormat)
                {
                    pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format,pstu_ControlParam);
                }
                else
                {
                    pstControlParamInit->u8Output_Format = pstControlParamInit->stu_Middle_Format->u8Mid_Format;
                }
        }
        //else if (E_CFIO_MODE_HDR2 == pstControlParamInit->u8Input_HDRMode)
        else if (MS_Cfd_CheckOpenHDR(pstControlParamInit->u8Input_HDRMode))
        {
                pstControlParamInit->u8Input_HDRIPMode = 1;
                pstControlParamInit->u8Input_SDRIPMode = 1;
                if (0 == u8TempMode)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                }
                else
                {
                }
                //pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_RGB;
                //pstControlParamInit->stu_Middle_Format->u8Mid_Format = E_CFD_CFIO_YUV_BT709;
                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;

                if (HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }

#if 0
#if (1 == PRECONSTRAINTS_REFERTO_EDID)

    if (E_CFD_OUTPUT_SOURCE_HDMI == pstControlParamInit->u8Output_Source)
    {
        if ((1 == pstHDMI_EDIDParamInit->u8HDMISink_EDID_Valid )&& (1 == pstHDMI_EDIDParamInit->u8HDMISink_HDRData_Block_Valid))
        {
            if ((0x04 == (pstHDMI_EDIDParamInit->u8HDMISink_EOTF&0x04)) && (E_CFIO_MODE_HDR2 == pstControlParamInit->u8Input_HDRMode)) //SMPTE 2084
            {
                    pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(1,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
                    u8flag = 1;
            }
            else if ((0x08 == (pstHDMI_EDIDParamInit->u8HDMISink_EOTF&0x08)) && (E_CFIO_MODE_HDR3 == pstControlParamInit->u8Input_HDRMode)) // HLG
            {
                    pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(1,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
                    u8flag = 1;
            }
        }
    }

#endif

    //SDR TV
    if (0 == u8flag)
    {
        if (0 == pstu_OSD_Param->u8HDR_UI_H2SMode)
        {
            pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(0,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
        }
        else
        {
            pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(1,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
        }
    }

#else
                if (E_CFD_OUTPUT_SOURCE_HDMI == pstControlParamInit->u8Output_Source)
                {
                    pstControlParamInit->u8Output_HDRMode = Mapi_CFD_DecideOutputHDRMode_HDMITX(pstControlParamInit, pstHDMI_EDIDParamInit, pstu_OSD_Param);
                }
                else
                {
                    pstControlParamInit->u8Output_HDRMode = E_CFIO_MODE_SDR;
                }
#endif

                //HDR to SDR case
#if 0
                if (E_CFIO_MODE_SDR == pstControlParamInit->u8Output_HDRMode)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;
                    pstControlParamInit->u8Input_HDRIPMode = 1;

                    //pstControlParamInit->u8Output_Format = E_CFD_CFIO_RGB_NOTSPECIFIED;
                    //pstControlParamInit->u8Output_Format = E_CFD_CFIO_RGB_BT709;

                    //force RGB output
                    pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format,pstu_ControlParam);
                    pstControlParamInit->u8Output_DataFormat =  E_CFD_MC_FORMAT_RGB;
                    pstControlParamInit->u8Output_HDRMode = E_CFIO_MODE_SDR;
                    pstControlParamInit->u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }
                else //HDR bypass case
                {
                    //pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_HDR2;
                    pstControlParamInit->u8Input_HDRIPMode = 0;
                    pstControlParamInit->u8Input_SDRIPMode = 0;
                }
#else
                //input = HDRx , output = SDR
                if ((E_CFIO_MODE_SDR == pstControlParamInit->u8Output_HDRMode) && (E_CFIO_MODE_SDR != pstControlParamInit->u8Input_HDRMode))
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;
                }
                //input = HDRx , output = HDRy
                else if ((pstControlParamInit->u8Output_HDRMode != pstControlParamInit->u8Input_HDRMode) && (E_CFIO_MODE_SDR != pstControlParamInit->u8Input_HDRMode))
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = pstControlParamInit->u8Output_HDRMode;
                    //when this case, keep range the same as input
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = pstControlParamInit->u8Input_IsFullRange;
                }
                else //HDR bypass case
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = pstControlParamInit->u8Input_HDRMode;
                    //when bypass, keep range the same as input
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = pstControlParamInit->u8Input_IsFullRange;
                }
                //pstControlParamInit->u8Input_HDRIPMode = 1;

                //force RGB output
                //pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format,pstu_ControlParam);
                //pstControlParamInit->u8Output_DataFormat =  E_CFD_MC_FORMAT_RGB;
                //pstControlParamInit->u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;

                //middle point must be Y
                //output is R
                if (E_CFD_MC_FORMAT_RGB == pstControlParamInit->u8Output_DataFormat)
                {
                    pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format,pstu_ControlParam);
                }
                else
                {
                    pstControlParamInit->u8Output_Format = pstControlParamInit->stu_Middle_Format->u8Mid_Format;
                }

#endif
        } //(E_CFIO_MODE_HDR2 == pstControlParamInit->u8Input_HDRMode)

    }

    return u16_check_status_tmp;
}
#endif

#if (NowHW == Kano)
//MS_U16 Mapi_Cfd_inter_Main_Control_PreConstraints_Kano(STU_CFDAPI_MAIN_CONTROL *pstControlParamInit, STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam)
MS_U16 Mapi_Cfd_inter_Main_Control_PreConstraints_Kano(STU_CFDAPI_MAIN_CONTROL *pstControlParamInit, STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam, STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParamInit, MS_U8 u8TempMode, STU_CFDAPI_OSD_CONTROL  *pstu_OSD_Param)
{

    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status_tmp = E_CFD_MC_ERR_NOERR;
    MS_U8 u8flag;
    u8flag = 0;

    //force not used in STB
    if (1 == pstControlParamInit->u8Input_IsRGBBypass)
    {
        pstControlParamInit->u8Input_IsRGBBypass = 0;
        //printf("\nForce:Current HW can not support u8Input_IsRGBBypass = 1, force u8Input_IsRGBBypass to 0\n");
        printf("\033[1;35m###[CFD][%s][%d]### Force:Current HW can not support u8Input_IsRGBBypass = 1, force u8Input_IsRGBBypass to 0\033[0m\n",__FUNCTION__,__LINE__);
        u16_check_status_tmp = E_CFD_MC_ERR_INPUT_ISRGBBYPASS_FORCE0;
    }

//force no gamut mapping for SDR input
#if ((CFD_SW_VERSION >= 0x00000017) && (0 == CFD_SDR_withGM))
    if ((3 != pstControlParamInit->u8HDMIOutput_GammutMapping_Mode) && (E_CFIO_MODE_SDR == pstControlParamInit->u8Input_HDRMode))
    {
        pstControlParamInit->u8HDMIOutput_GammutMapping_Mode = 0;
    }
#endif

    //RGB
    if (E_CFD_MC_FORMAT_RGB == pstControlParamInit->u8Input_DataFormat)
    {
        if (1 == pstControlParamInit->u8Input_IsRGBBypass)
        {
            if (E_CFIO_MODE_SDR == pstControlParamInit->u8Input_HDRMode)
            {

                pstControlParamInit->u8Input_HDRIPMode = 1;
                pstControlParamInit->u8Input_SDRIPMode = 1;

                if (0 == u8TempMode) //GM does not happens or can not happen
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                }
                else
                {
                    //from MS_Cfd_SetFormat_FromGamutOrderIdx()
                }

                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_RGB;
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;

#if 0
                if (HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }
#else
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
#endif

                if (0 == u8TempMode)
                {
                    pstControlParamInit->u8Output_Format = pstControlParamInit->u8Input_Format;
                }
                else
                {
                    pstControlParamInit->u8Output_Format = pstControlParamInit->stu_Middle_Format->u8Mid_Format;
                }
                pstControlParamInit->u8Output_DataFormat  = E_CFD_MC_FORMAT_RGB;
                pstControlParamInit->u8Output_HDRMode     = E_CFIO_MODE_SDR;
                pstControlParamInit->u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;
            }
            else if (MS_Cfd_CheckOpenHDR(pstControlParamInit->u8Input_HDRMode))
            {
                pstControlParamInit->u8Input_HDRIPMode = 1;
                pstControlParamInit->u8Input_SDRIPMode = 1;

                if (0 == u8TempMode)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_Format = MS_Cfd_SetFormat_OnlyR2Y(pstControlParamInit->u8Input_Format);
                }
                else
                {
                    //from MS_Cfd_SetFormat_FromGamutOrderIdx()
                }
                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;

                if (HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }

                if (0 == u8TempMode)
                {
                    pstControlParamInit->u8Output_Format = pstControlParamInit->u8Input_Format;
                }
                else
                {
                    //pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format);
                    pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format, pstu_ControlParam);
                }
                pstControlParamInit->u8Output_DataFormat =  E_CFD_MC_FORMAT_RGB;
                pstControlParamInit->u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;

#if (1 == PRECONSTRAINTS_REFERTO_EDID)
                if (E_CFD_OUTPUT_SOURCE_HDMI == pstControlParamInit->u8Output_Source)
                {
                    if ((1 == pstHDMI_EDIDParamInit->u8HDMISink_EDID_Valid )&& (1 == pstHDMI_EDIDParamInit->u8HDMISink_HDRData_Block_Valid))
                    {
                            if ((0x04 == (pstHDMI_EDIDParamInit->u8HDMISink_EOTF&0x04)) && (E_CFIO_MODE_HDR2 == pstControlParamInit->u8Input_HDRMode)) //SMPTE 2084
                            {
                                    pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(1,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
                                    u8flag = 1;
                            }
                            else if ((0x08 == (pstHDMI_EDIDParamInit->u8HDMISink_EOTF&0x08)) && (E_CFIO_MODE_HDR3 == pstControlParamInit->u8Input_HDRMode)) // HLG
                            {
                                    pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(1,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
                                    u8flag = 1;
                            }
                    }
                }
#endif
                //SDR TV
                if (0 == u8flag)
                {
                    if (0 == pstu_OSD_Param->u8HDR_UI_H2SMode)
                    {
                        pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(0,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
                    }
                    else
                    {
                        pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(1,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
                    }
                }

                //HDR to SDR case
                if (E_CFIO_MODE_SDR == pstControlParamInit->u8Output_HDRMode)
                {
                    pstControlParamInit->u8Output_HDRMode = pstControlParamInit->u8Input_HDRMode;
                    pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = pstControlParamInit->u8Input_HDRMode;

                    //not support this case, return error
                    u16_check_status_tmp = E_CFD_MC_ERR_HW_IPS_TMO_INRGB_NOTSUPPORTED;
                    //HDR_DBG_HAL_CFD(printk("\nForce:Current HW can not support HDR RGB in, Force bypass\n"));
                    printf("\033[1;35m###[CFD][%s][%d]### Force:Current HW can not support HDR RGB in, Force bypass\033[0m\n",__FUNCTION__,__LINE__);
                    pstu_ControlParam->u8DoHDRIP_Forcebypass_Flag = 1;
                    pstControlParamInit->u8Input_HDRIPMode = 0;
                    pstControlParamInit->u8Input_SDRIPMode = 0;
                }
                else //HDR bypass case
                {
                    pstControlParamInit->u8Input_HDRIPMode = 0;
                    pstControlParamInit->u8Input_SDRIPMode = 0;
                }
            }
        }
        else // (0 == pstControlParamInit->u8Input_IsRGBBypass)
        {
            if (E_CFIO_MODE_SDR == pstControlParamInit->u8Input_HDRMode)
            {
                pstControlParamInit->u8Input_HDRIPMode = 1;
                pstControlParamInit->u8Input_SDRIPMode = 1;

                if (0 == u8TempMode)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                }
                else
                {
                    //from MS_Cfd_SetFormat_FromGamutOrderIdx()
                }
                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_RGB;
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;

#if 0
                if (HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }
#else
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
#endif

                if (0 == u8TempMode)
                {
                    pstControlParamInit->u8Output_Format = pstControlParamInit->u8Input_Format;
                }
                else
                {
                    pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format, pstu_ControlParam);
                }
                pstControlParamInit->u8Output_DataFormat =  E_CFD_MC_FORMAT_RGB;
                pstControlParamInit->u8Output_HDRMode = E_CFIO_MODE_SDR;
                pstControlParamInit->u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;
            }
            else if (MS_Cfd_CheckOpenHDR(pstControlParamInit->u8Input_HDRMode))
            {

                pstControlParamInit->u8Input_HDRIPMode = 1;
                pstControlParamInit->u8Input_SDRIPMode = 1;
                if (0 == u8TempMode)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_Format = MS_Cfd_SetFormat_OnlyR2Y(pstControlParamInit->u8Input_Format);
                }
                else
                {
                }
                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;

                if (HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }

                if (0 == u8TempMode)
                {
                    pstControlParamInit->u8Output_Format = pstControlParamInit->u8Input_Format;
                }
                else
                {
                    //pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format);
                    pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format, pstu_ControlParam);
                }
                pstControlParamInit->u8Output_DataFormat =  E_CFD_MC_FORMAT_RGB;
                pstControlParamInit->u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;

#if (1 == PRECONSTRAINTS_REFERTO_EDID)

                if (E_CFD_OUTPUT_SOURCE_HDMI == pstControlParamInit->u8Output_Source)
                {
                    if ((1 == pstHDMI_EDIDParamInit->u8HDMISink_EDID_Valid )&& (1 == pstHDMI_EDIDParamInit->u8HDMISink_HDRData_Block_Valid))
                    {
                         if ((0x04 == (pstHDMI_EDIDParamInit->u8HDMISink_EOTF&0x04)) && (E_CFIO_MODE_HDR2 == pstControlParamInit->u8Input_HDRMode)) //SMPTE 2084
                         {
                              pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(1,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
                              u8flag = 1;
                         }
                         else if ((0x08 == (pstHDMI_EDIDParamInit->u8HDMISink_EOTF&0x08)) && (E_CFIO_MODE_HDR3 == pstControlParamInit->u8Input_HDRMode)) // HLG
                         {
                              pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(1,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
                              u8flag = 1;
                         }
                    }
                }
#endif
                //SDR TV
                if (0 == u8flag)
                {
                    if (0 == pstu_OSD_Param->u8HDR_UI_H2SMode)
                    {
                        pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(0,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
                    }
                    else
                    {
                        pstControlParamInit->u8Output_HDRMode = CFD_in_SetHDRMode_TopbyUI(1,pstControlParamInit->u8Input_HDRMode,pstControlParamInit->u8Input_HDRMode,pstu_OSD_Param->u8HDR_UI_H2SMode);
                    }
                }


                //HDR to SDR case
                if (E_CFIO_MODE_SDR == pstControlParamInit->u8Output_HDRMode)
                {

                    pstControlParamInit->u8Output_HDRMode = pstControlParamInit->u8Input_HDRMode;
                    pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = pstControlParamInit->u8Input_HDRMode;

                    //not support this case, return error
                    u16_check_status_tmp = E_CFD_MC_ERR_HW_IPS_TMO_INRGB_NOTSUPPORTED;
                    //HDR_DBG_HAL_CFD(printk("\nForce:Current HW can not support HDR RGB in, Force bypass\n"));
                    printf("\033[1;35m###[CFD][%s][%d]### Force:Current HW can not support HDR RGB in, Force bypass\033[0m\n",__FUNCTION__,__LINE__);
                    pstu_ControlParam->u8DoHDRIP_Forcebypass_Flag = 1;
                    pstControlParamInit->u8Input_HDRIPMode = 0;
                    pstControlParamInit->u8Input_SDRIPMode = 0;
                }
                else //HDR bypass case
                {
                    pstControlParamInit->u8Input_HDRIPMode = 0;
                    pstControlParamInit->u8Input_SDRIPMode = 0;
                }
            }
        }
    }
    else // YUV
    {
        //input = SDR, not BT2020 CL
        if ((E_CFIO_MODE_SDR == pstControlParamInit->u8Input_HDRMode) && (E_CFD_CFIO_YUV_BT2020_CL != pstControlParamInit->u8Input_Format))
        {
                pstControlParamInit->u8Input_HDRIPMode = 1;
                pstControlParamInit->u8Input_SDRIPMode = 1;

                //GM does not happens or can not happen
                if (0 == u8TempMode)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                }
                else //gamut mapping happens
                {

                }
                //pstControlParamInit->stu_Middle_Format->u8Mid_Format = E_CFD_CFIO_YUV_BT709;
                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;
                //pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;

#if 0
                if (HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }
#else
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
#endif

                if (E_CFD_OUTPUT_SOURCE_HDMI == pstControlParamInit->u8Output_Source)
                {
                    pstControlParamInit->u8Output_HDRMode = Mapi_CFD_DecideOutputHDRMode_HDMITX(pstControlParamInit, pstHDMI_EDIDParamInit, pstu_OSD_Param);
                }
                else
                {
                    pstControlParamInit->u8Output_HDRMode = E_CFIO_MODE_SDR;
                }

                //SDR bypass case
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = pstControlParamInit->u8Output_HDRMode;

                //when bypass, keep range the same as input
                if (E_CFIO_MODE_SDR == pstControlParamInit->u8Output_HDRMode)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = pstControlParamInit->u8Input_IsFullRange;
                }
                else //SDR to HDR case
                {
                    //nothing special now
                }

                //middle point must be Y
                //output is R
                if (E_CFD_MC_FORMAT_RGB == pstControlParamInit->u8Output_DataFormat)
                {
                    pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format,pstu_ControlParam);
                }
                else
                {
                    pstControlParamInit->u8Output_Format = pstControlParamInit->stu_Middle_Format->u8Mid_Format;
                }

        }
        //input = SDR, BT2020 CL, Kano can handle this case
        else if ((E_CFIO_MODE_SDR == pstControlParamInit->u8Input_HDRMode) && (E_CFD_CFIO_YUV_BT2020_CL == pstControlParamInit->u8Input_Format))
        {

                pstControlParamInit->u8Input_HDRIPMode = 1;
                pstControlParamInit->u8Input_SDRIPMode = 1;
                //pstControlParamInit->stu_Middle_Format->u8Mid_Format = E_CFD_CFIO_YUV_BT2020_NCL;

                //GM does not happens or can not happen
                if (0 == u8TempMode)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                    //pstControlParamInit->stu_Middle_Format->u8Mid_Format = E_CFD_CFIO_YUV_BT2020_NCL;
                }
                else
                {
                }
                //pstControlParamInit->stu_Middle_Format->u8Mid_Format = E_CFD_CFIO_YUV_BT709;
                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;

#if 0
                if (HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }
#else
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
#endif
                //middle point must be Y
                //output is R
                if (E_CFD_MC_FORMAT_RGB == pstControlParamInit->u8Output_DataFormat)
                {
                    pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format,pstu_ControlParam);
                }
                else
                {
                    pstControlParamInit->u8Output_Format = pstControlParamInit->stu_Middle_Format->u8Mid_Format;
                }
        }
        //else if (E_CFIO_MODE_HDR2 == pstControlParamInit->u8Input_HDRMode)
        else if (MS_Cfd_CheckOpenHDR(pstControlParamInit->u8Input_HDRMode))
        {
                pstControlParamInit->u8Input_HDRIPMode = 1;
                pstControlParamInit->u8Input_SDRIPMode = 1;
                if (0 == u8TempMode)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_Format = pstControlParamInit->u8Input_Format;
                }
                else
                {
                }
                //pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_RGB;
                //pstControlParamInit->stu_Middle_Format->u8Mid_Format = E_CFD_CFIO_YUV_BT709;
                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;

#if 0
                if (HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
                }
                else
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }
#else
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
#endif

                if (E_CFD_OUTPUT_SOURCE_HDMI == pstControlParamInit->u8Output_Source)
                {
                    pstControlParamInit->u8Output_HDRMode = Mapi_CFD_DecideOutputHDRMode_HDMITX(pstControlParamInit, pstHDMI_EDIDParamInit, pstu_OSD_Param);
                }
                else
                {
                    pstControlParamInit->u8Output_HDRMode = E_CFIO_MODE_SDR;
                }

                //HDR to SDR case
#if 0
                if (E_CFIO_MODE_SDR == pstControlParamInit->u8Output_HDRMode)
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;
                    pstControlParamInit->u8Input_HDRIPMode = 1;

                    //pstControlParamInit->u8Output_Format = E_CFD_CFIO_RGB_NOTSPECIFIED;
                    //pstControlParamInit->u8Output_Format = E_CFD_CFIO_RGB_BT709;

                    //force RGB output
                    //pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format);
                    pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format, pstu_ControlParam);
                    pstControlParamInit->u8Output_DataFormat =  E_CFD_MC_FORMAT_RGB;
                    pstControlParamInit->u8Output_HDRMode = E_CFIO_MODE_SDR;
                    pstControlParamInit->u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;
                }
                else //HDR bypass case
                {
                    //pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_HDR2;
                    pstControlParamInit->u8Input_HDRIPMode = 0;
                    pstControlParamInit->u8Input_SDRIPMode = 0;
                }
#else
                //input = HDRx , output = SDR
                if ((E_CFIO_MODE_SDR == pstControlParamInit->u8Output_HDRMode) && (E_CFIO_MODE_SDR != pstControlParamInit->u8Input_HDRMode))
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;
                }
                //input = HDRx , output = HDRy
                else if ((pstControlParamInit->u8Output_HDRMode != pstControlParamInit->u8Input_HDRMode) && (E_CFIO_MODE_SDR != pstControlParamInit->u8Input_HDRMode))
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = pstControlParamInit->u8Output_HDRMode;
                    //when this case, keep range the same as input
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = pstControlParamInit->u8Input_IsFullRange;
                }
                else //HDR bypass case
                {
                    pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = pstControlParamInit->u8Input_HDRMode;
                    //when bypass, keep range the same as input
                    pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = pstControlParamInit->u8Input_IsFullRange;
                }
                //pstControlParamInit->u8Input_HDRIPMode = 1;

                //force RGB output
                //pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format,pstu_ControlParam);
                //pstControlParamInit->u8Output_DataFormat =  E_CFD_MC_FORMAT_RGB;
                //pstControlParamInit->u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;

                //middle point must be Y
                //output is R
                if (E_CFD_MC_FORMAT_RGB == pstControlParamInit->u8Output_DataFormat)
                {
                    pstControlParamInit->u8Output_Format = MS_Cfd_SetFormat_OnlyY2R(pstControlParamInit->stu_Middle_Format->u8Mid_Format,pstu_ControlParam);
                }
                else
                {
                    pstControlParamInit->u8Output_Format = pstControlParamInit->stu_Middle_Format->u8Mid_Format;
                }

#endif

        } //(E_CFIO_MODE_HDR2 == pstControlParamInit->u8Input_HDRMode)

    }

    return u16_check_status_tmp;
}
#endif


MS_U16 Mapi_Cfd_inter_Main_Control_PostConstraints_STB(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam )
{

    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status_tmp = E_CFD_MC_ERR_NOERR;
    //MS_U16 u16Temp;

    //not support RGB limit out
#if 0
    if (1 == pstControlParamInit->u8Input_IsRGBBypass)
    {
        pstControlParamInit->u8Input_IsRGBBypass = 0;
        HDR_DBG_HAL_CFD(printf("\nForce:Current HW can not u8Input_IsRGBBypass = 1, force u8Input_IsRGBBypass to 0\n"));
        u16_check_status_tmp = E_CFD_MC_ERR_INPUT_ISRGBBYPASS_FORCE0;
    }
#endif

    //

    //not support RGB limit out
#if 0
    if ((E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_DataFormat) && (E_CFD_CFIO_RANGE_LIMIT == pstu_ControlParam->u8Output_IsFullRange))
    {
        pstu_ControlParam->u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;
        HDR_DBG_HAL_CFD(printf("\nForce:Current HW can not RGB limit out, force u8Output_IsFullRange to E_CFD_CFIO_RANGE_FULL\n"));
        u16_check_status_tmp = E_CFD_MC_ERR_OUTPUT_ISFULLRANGE_FORCE1;
    }
#endif

    return 1;
}

#if 0
MS_U16 Mapi_Cfd_inter_Main_Control_PostConstraints_Curry(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam )
{

    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
    MS_U16 u16_check_status_tmp = E_CFD_MC_ERR_NOERR;
    //MS_U16 u16Temp;

    //not support RGB limit out
#if 0
    if (1 == pstControlParamInit->u8Input_IsRGBBypass)
    {
        pstControlParamInit->u8Input_IsRGBBypass = 0;
        HDR_DBG_HAL_CFD(printf("\nForce:Current HW can not u8Input_IsRGBBypass = 1, force u8Input_IsRGBBypass to 0\n"));
        u16_check_status_tmp = E_CFD_MC_ERR_INPUT_ISRGBBYPASS_FORCE0;
            }
#endif

    //

    //not support RGB limit out
    if ((E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_DataFormat) && (E_CFD_CFIO_RANGE_LIMIT == pstu_ControlParam->u8Output_IsFullRange))
    {
        pstu_ControlParam->u8Output_IsFullRange = E_CFD_CFIO_RANGE_FULL;
        HDR_DBG_HAL_CFD(printf("\nForce:Current HW can not RGB limit out, force u8Output_IsFullRange to E_CFD_CFIO_RANGE_FULL\n"));
        u16_check_status_tmp = E_CFD_MC_ERR_OUTPUT_ISFULLRANGE_FORCE1;
    }


    return 1;
}
#endif

MS_U16 Mapi_Cfd_inter_Main_Control_PreConstraints(STU_CFDAPI_MAIN_CONTROL *pstControlParamInit )
{

    MS_U16 u16_check_status = E_CFD_MC_ERR_NOERR;
    MS_U8 u8_check_status2 = E_CFD_MC_ERR_NOERR;
    //MS_U16 u16Temp;

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

#if HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
#else
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
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

#if HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
#else
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
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
                //force format RGB to YUV
                pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_YUV444;
                //pstControlParamInit->stu_Middle_Format->u8Mid_DataFormat = E_CFD_MC_FORMAT_RGB;
                pstControlParamInit->stu_Middle_Format->u8Mid_HDRMode = E_CFIO_MODE_SDR;

#if HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
#else
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
#endif
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

#if HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
#else
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
#endif
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

#if HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
#else
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
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

#if HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_LIMIT;
#else
                pstControlParamInit->stu_Middle_Format->u8Mid_IsFullRange = E_CFD_CFIO_RANGE_FULL;
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

    return E_CFD_MC_ERR_NOERR;
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
    MS_U8 u8idxub = 0;

    u8idxub = sizeof (u8Const_LUT_MMInfor_HDMIDefinition) / (sizeof(MS_U8)*5);

    for (u8idx =0; u8idx < u8idxub ;u8idx++)
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
                //if ((0 <= u8idx)&&(6 >= u8idx))
                if ( E_CFD_CFIO_RGB_NOTSPECIFIED != u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3])
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

//set static metadata by default
void MS_Cfd_Set_StaticMD_InfoFrame_default(STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT *pstu_out_HDMI_InfoFrame)
{

    //order R->G->B
    //BT709
    //data *0.00002 0xC350 = 1
    pstu_out_HDMI_InfoFrame->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[0] = 0x7D00; //0.64
    pstu_out_HDMI_InfoFrame->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[1] = 0x3A98; //0.3
    pstu_out_HDMI_InfoFrame->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[2] = 0x1D4C; //0.15

    pstu_out_HDMI_InfoFrame->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[0] = 0x4047; //0.33
    pstu_out_HDMI_InfoFrame->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[1] = 0x7530; //0.6
    pstu_out_HDMI_InfoFrame->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[2] = 0x0BB8; //0.06

    pstu_out_HDMI_InfoFrame->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16White_point_x = 0x3D13; //0.3127
    pstu_out_HDMI_InfoFrame->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16White_point_y = 0x4042; //0.3290


    pstu_out_HDMI_InfoFrame->u16Master_Panel_Max_Luminance = 4000;
    //data * 1 nits

    pstu_out_HDMI_InfoFrame->u16Master_Panel_Min_Luminance = 500;
    //data * 0.0001 nits

    pstu_out_HDMI_InfoFrame->u16Max_Content_Light_Level = 600;
    //data * 1 nits

    pstu_out_HDMI_InfoFrame->u16Max_Frame_Avg_Light_Level = 128;
    //data * 1 nits

}

//set Mastering display primaries from color primary in VUI header
//input u8Colour_primaries
//output STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT *pstu_out_HDMI_InfoFrame
void MS_Cfd_SetPrimaries_fromCP(STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT *pstu_out_HDMI_InfoFrame, MS_U8 u8Colour_primaries)
{

    STU_CFD_COLORIMETRY stu;

    switch(u8Colour_primaries)
    {
        case E_CFD_CFIO_CP_RESERVED0:
        case E_CFD_CFIO_CP_BT709_SRGB_SYCC:
        case E_CFD_CFIO_CP_UNSPECIFIED:
        case E_CFD_CFIO_CP_RESERVED3:
        case E_CFD_CFIO_CP_PANEL:
        default:
        //BT709
        stu.u16Display_Primaries_x[0] = 0x7D00;  //0.64
        stu.u16Display_Primaries_x[1] = 0x3A98;  //0.3
        stu.u16Display_Primaries_x[2] = 0x1D4C;  //0.15

        stu.u16Display_Primaries_y[0] = 0x4074;  //0.33
        stu.u16Display_Primaries_y[1] = 0x7530;  //0.6
        stu.u16Display_Primaries_y[2] = 0x0BB8;  //0.06

        //D65
        stu.u16White_point_x = 0x3D13;  //0.3127
        stu.u16White_point_y = 0x4042;  //0.3290
        break;

        case E_CFD_CFIO_CP_BT470_6:
        stu.u16Display_Primaries_x[0] = 0x82DC;  //0.67
        stu.u16Display_Primaries_x[1] = 0x2904;  //0.21
        stu.u16Display_Primaries_x[2] = 0x1B58;  //0.14

        stu.u16Display_Primaries_y[0] = 0x4074;  //0.33
        stu.u16Display_Primaries_y[1] = 0x8AAC;  //0.71
        stu.u16Display_Primaries_y[2] = 0x0FA0;  //0.08

        //C
        stu.u16White_point_x = 0x3C8C;  //0.3100
        stu.u16White_point_y = 0x3DB8;  //0.3160

        break;

        case E_CFD_CFIO_CP_BT601625:
        stu.u16Display_Primaries_x[0] = 0x7D00;  //0.64
        stu.u16Display_Primaries_x[1] = 0x38A4;  //0.29
        stu.u16Display_Primaries_x[2] = 0x1D4C;  //0.15

        stu.u16Display_Primaries_y[0] = 0x4074;  //0.33
        stu.u16Display_Primaries_y[1] = 0x7530;  //0.60
        stu.u16Display_Primaries_y[2] = 0x0BB8;  //0.06

        //D65
        stu.u16White_point_x = 0x3D13;  //0.3127
        stu.u16White_point_y = 0x4042;  //0.3290

        break;

        case E_CFD_CFIO_CP_BT601525_SMPTE170M:
        case E_CFD_CFIO_CP_SMPTE240M:
        stu.u16Display_Primaries_x[0] = 0x7B0C;  //0.63
        stu.u16Display_Primaries_x[1] = 0x3C8C;  //0.31
        stu.u16Display_Primaries_x[2] = 0x1E46;  //0.155

        stu.u16Display_Primaries_y[0] = 0x4268;  //0.34
        stu.u16Display_Primaries_y[1] = 0x7436;  //0.595
        stu.u16Display_Primaries_y[2] = 0x0DAC;  //0.07

        //D65
        stu.u16White_point_x = 0x3D13;  //0.3127
        stu.u16White_point_y = 0x4042;  //0.3290

        break;

        case E_CFD_CFIO_CP_GENERIC_FILM:

        stu.u16Display_Primaries_x[0] = 0x8502;  //0.681
        stu.u16Display_Primaries_x[1] = 0x2F76;  //0.243
        stu.u16Display_Primaries_x[2] = 0x1C52;  //0.145

        stu.u16Display_Primaries_y[0] = 0x3E4E;  //0.319
        stu.u16Display_Primaries_y[1] = 0x8728;  //0.692
        stu.u16Display_Primaries_y[2] = 0x0992;  //0.049

        //C
        stu.u16White_point_x = 0x3C8C;  //0.3100
        stu.u16White_point_y = 0x3DB8;  //0.3160

        break;

        case E_CFD_CFIO_CP_BT2020:

        stu.u16Display_Primaries_x[0] = 0x8A48;  //0.708
        stu.u16Display_Primaries_x[1] = 0x2134;  //0.170
        stu.u16Display_Primaries_x[2] = 0x1996;  //0.131

        stu.u16Display_Primaries_y[0] = 0x3908;  //0.292
        stu.u16Display_Primaries_y[1] = 0x9BAA;  //0.797
        stu.u16Display_Primaries_y[2] = 0x08FC;  //0.046

        //D65
        stu.u16White_point_x = 0x3D13;  //0.3127
        stu.u16White_point_y = 0x4042;  //0.3290

        break;

        case E_CFD_CFIO_CP_CIEXYZ:

        stu.u16Display_Primaries_x[0] = 0xC350;  //1
        stu.u16Display_Primaries_x[1] = 0x0000;  //0
        stu.u16Display_Primaries_x[2] = 0x0000;  //0

        stu.u16Display_Primaries_y[0] = 0x0000;  //0
        stu.u16Display_Primaries_y[1] = 0xC350;  //1
        stu.u16Display_Primaries_y[2] = 0x0000;  //0

        //
        stu.u16White_point_x = 0x4119;  //0.3333
        stu.u16White_point_y = 0x4119;  //0.3333

        break;

        case E_CFD_CFIO_CP_ADOBERGB:

        stu.u16Display_Primaries_x[0] = 0x7D00;  //0.64
        stu.u16Display_Primaries_x[1] = 0x2904;  //0.21
        stu.u16Display_Primaries_x[2] = 0x1D4C;  //0.15

        stu.u16Display_Primaries_y[0] = 0x4074;  //0.33
        stu.u16Display_Primaries_y[1] = 0x8AAC;  //0.71
        stu.u16Display_Primaries_y[2] = 0x0BB8;  //0.06

        //D65
        stu.u16White_point_x = 0x3D13;  //0.3127
        stu.u16White_point_y = 0x4042;  //0.3290

        break;
    }

    memcpy(&(pstu_out_HDMI_InfoFrame->stu_Cfd_HDMISource_MasterPanel_ColorMetry),&stu, sizeof(STU_CFD_COLORIMETRY));
}

//=========================================================================================================
//set static metadata descriptor type1 from MM
//input : STU_CFDAPI_MM_PARSER *pstu_MM
//output : STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT *pstu_out_HDMI_InfoFrame

//Return value
//1 : can output mastering display information
//0 : can not output mastering display information
//=======================a==================================================================================
MS_U8 MS_Cfd_Set_StaticMD_InfoFrame_fromMM(STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT *pstu_out_HDMI_InfoFrame,STU_CFDAPI_MM_PARSER *pstu_MM_Param, STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam)
{
    MS_U8 u8MasteringSEIIsReady;
    MS_U8 u8CLLSEIISReady;
    MS_U8 u8ReturnValue;

    u8ReturnValue = 0;

    //for mastering_display_colour SEI
    if (E_CFD_VALID == pstu_MM_Param->u8Mastering_Display_Infor_Valid)
    {
        //memcpy(&(pstu_out_HDMI_InfoFrame->stu_Cfd_HDMISource_MasterPanel_ColorMetry),&(pstu_MM_Param->stu_Cfd_MM_MasterPanel_ColorMetry),sizeof(STU_CFD_COLORIMETRY));

        memcpy(&(pstu_out_HDMI_InfoFrame->stu_Cfd_HDMISource_MasterPanel_ColorMetry),&(pstu_ControlParam->stu_source_mastering_display),sizeof(STU_CFD_COLORIMETRY));

        pstu_out_HDMI_InfoFrame->u16Master_Panel_Max_Luminance = (pstu_MM_Param->u32Master_Panel_Max_Luminance/10000);

        if (pstu_MM_Param->u32Master_Panel_Min_Luminance > 0xffff)
        {
            pstu_out_HDMI_InfoFrame->u16Master_Panel_Min_Luminance = 0xffff;
        }
        else
        {
            pstu_out_HDMI_InfoFrame->u16Master_Panel_Min_Luminance = (MS_U16)pstu_MM_Param->u32Master_Panel_Min_Luminance;
        }

        u8MasteringSEIIsReady = 1;
    }
    else
#if 0
    {
        //set by color primary from VUI
        MS_Cfd_SetPrimaries_fromCP(pstu_out_HDMI_InfoFrame, pstu_MM_Param->u8Colour_primaries);

        //pstu_out_HDMI_InfoFrame->u16Master_Panel_Max_Luminance = 1000; //1000 nits

        //pstu_out_HDMI_InfoFrame->u16Master_Panel_Min_Luminance = 5000; //0.05 nits

        pstu_out_HDMI_InfoFrame->u16Master_Panel_Max_Luminance = pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax; //1000 nits

        if (0 == pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMinFlag)
        {
            pstu_out_HDMI_InfoFrame->u16Master_Panel_Min_Luminance = 10000;
        }
        else
        {
            pstu_out_HDMI_InfoFrame->u16Master_Panel_Min_Luminance = pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMin; //0.05 nits
        }

        u8MasteringSEIIsReady = 0;
    }
#else
    {
        //set by color primary from VUI
        //MS_Cfd_SetPrimaries_fromCP(pstu_out_HDMI_InfoFrame, pstu_MM_Param->u8Colour_primaries);

        memset(&(pstu_out_HDMI_InfoFrame->stu_Cfd_HDMISource_MasterPanel_ColorMetry),0x00,sizeof(STU_CFD_COLORIMETRY));

        pstu_out_HDMI_InfoFrame->u16Master_Panel_Max_Luminance = 0; //1000 nits

        pstu_out_HDMI_InfoFrame->u16Master_Panel_Min_Luminance = 0; //0.05 nits

        u8MasteringSEIIsReady = 1;
    }
#endif

    //for content_light_level SEI
    if (E_CFD_VALID == pstu_MM_Param->u8MM_HDR_ContentLightMetaData_Valid)
    {
        pstu_out_HDMI_InfoFrame->u16Max_Content_Light_Level = pstu_MM_Param->u16Max_content_light_level;
        pstu_out_HDMI_InfoFrame->u16Max_Frame_Avg_Light_Level = pstu_MM_Param->u16Max_pic_average_light_level;
        u8CLLSEIISReady = 1;
    }
    else
    {
        pstu_out_HDMI_InfoFrame->u16Max_Content_Light_Level = 0;
        pstu_out_HDMI_InfoFrame->u16Max_Frame_Avg_Light_Level = 0;
        u8CLLSEIISReady = 0;
    }

    if ((1 == u8MasteringSEIIsReady) || (1 == u8CLLSEIISReady))
    {
        u8ReturnValue = 1;
    }
    else
    {
        u8ReturnValue = 0;
    }

    return u8ReturnValue;
}

//========================================================================================================
//set static metadata descriptor type1 from InfoFrame
//input : STU_CFDAPI_MM_PARSER *pstu_MM
//output : STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT *pstu_out_HDMI_InfoFrame

//Return value
//1 : can output mastering display information
//0 : can not output mastering display information

//========================================================================================================

MS_U8 MS_Cfd_Set_StaticMD_InfoFrame_fromHDMI(STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT *pstu_out_HDMI_InfoFrame,
                                            STU_CFDAPI_HDMI_INFOFRAME_PARSER *pstu_in_HDMI_InfoFrame,
                                            STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam)
{
    MS_U8 u8ReturnValue;

    u8ReturnValue = 0;

    if (E_CFD_VALID == pstu_in_HDMI_InfoFrame->u8Mastering_Display_Infor_Valid)
    {
        //memcpy(&(pstu_out_HDMI_InfoFrame->stu_Cfd_HDMISource_MasterPanel_ColorMetry),&(pstu_in_HDMI_InfoFrame->stu_Cfd_HDMISource_MasterPanel_ColorMetry),sizeof(STU_CFD_COLORIMETRY));

        memcpy(&(pstu_out_HDMI_InfoFrame->stu_Cfd_HDMISource_MasterPanel_ColorMetry),&(pstu_ControlParam->stu_source_mastering_display),sizeof(STU_CFD_COLORIMETRY));

        pstu_out_HDMI_InfoFrame->u16Master_Panel_Max_Luminance = pstu_in_HDMI_InfoFrame->u16Master_Panel_Max_Luminance;

        pstu_out_HDMI_InfoFrame->u16Master_Panel_Min_Luminance = pstu_in_HDMI_InfoFrame->u16Master_Panel_Min_Luminance;

        pstu_out_HDMI_InfoFrame->u16Max_Content_Light_Level = pstu_in_HDMI_InfoFrame->u16Max_Content_Light_Level;

        pstu_out_HDMI_InfoFrame->u16Max_Frame_Avg_Light_Level = pstu_in_HDMI_InfoFrame->u16Max_Frame_Avg_Light_Level;

        u8ReturnValue = 1;
    }
    else
    {
        //set by color primary from VUI
        MS_Cfd_SetPrimaries_fromCP(pstu_out_HDMI_InfoFrame, pstu_ControlParam->u8InputColorPriamries);

        //pstu_out_HDMI_InfoFrame->u16Master_Panel_Max_Luminance = 1000; //1000 nits

        //pstu_out_HDMI_InfoFrame->u16Master_Panel_Min_Luminance = 5000; //0.05 nits

        pstu_out_HDMI_InfoFrame->u16Master_Panel_Max_Luminance = pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax; //1000 nits

        if (0 == pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMinFlag)
        {
            pstu_out_HDMI_InfoFrame->u16Master_Panel_Min_Luminance = 10000;
        }
        else
        {
            pstu_out_HDMI_InfoFrame->u16Master_Panel_Min_Luminance = pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMin; //0.05 nits
        }

        pstu_out_HDMI_InfoFrame->u16Max_Content_Light_Level = 0;
        pstu_out_HDMI_InfoFrame->u16Max_Frame_Avg_Light_Level = 0;

        u8ReturnValue = 0;
    }

    return u8ReturnValue;
}


//set static metadata descriptor type1 from Output format of CFD
//for not bypass case
//input :STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam
//output:STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT *pstu_out_HDMI_InfoFrame

void MS_Cfd_Set_StaticMD_InfoFrame_fromOutputformat(STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT *pstu_out_HDMI_InfoFrame, STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam)
{
    //set by color primary from VUI
    MS_Cfd_SetPrimaries_fromCP(pstu_out_HDMI_InfoFrame, pstu_ControlParam->u8OutputColorPriamries);

    pstu_out_HDMI_InfoFrame->u16Master_Panel_Max_Luminance = 1000; //1000 nits

    pstu_out_HDMI_InfoFrame->u16Master_Panel_Min_Luminance = 5000; //0.05 nits

    pstu_out_HDMI_InfoFrame->u16Max_Content_Light_Level = 0;
    pstu_out_HDMI_InfoFrame->u16Max_Frame_Avg_Light_Level = 0;
}

MS_U8 MS_Cfd_OSD_Process_SetColorMetry(STU_CFDAPI_OSD_PROCESS_CONFIGS *pstu_OSD_Process_Configs,
                                       STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT *pstu_api_HDMI_InfoFrame_Param_out)
{
    MS_U8 u8Check_Status = 0;
    MS_U8 u8idx = 0;

#if 0
    for (u8idx =0;u8idx<=9 ;u8idx++)
    {
        if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3] == pstu_api_HDMI_InfoFrame_Param_out->u8HDMISource_Colorspace)||
            (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4] == pstu_api_HDMI_InfoFrame_Param_out->u8HDMISource_Colorspace))
        {
            break;
        }
    }

    if (u8idx > 8)
    {
        u8idx = 8;
    }
#else
    u8idx = MS_Cfd_FindMatchedIdx(pstu_api_HDMI_InfoFrame_Param_out->u8HDMISource_Colorspace);
#endif

    pstu_OSD_Process_Configs->u8Video_colorprimary = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0];
    //pstu_ControlParam->u8InputTransferCharacterstics = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][1];
    pstu_OSD_Process_Configs->u8Video_MatrixCoefficients = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][2];

    //0:current HDMI input is one case of HDMI colormetry
    //1:current HDMI input is not one case of HDMI colormetry,
    return u8Check_Status;
}

//sync Video information to the paramters in OSD process function
void MS_Cfd_OSD_Process_configs_update(STU_CFDAPI_OSD_PROCESS_CONFIGS *pstu_OSD_Process_Configs,
                                       STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT *pstu_api_HDMI_InfoFrame_Param_out)
{

    MS_U8 u8Check_Status = 0;

    //pstu_OSD_Process_Configs->u8Video_colorprimary;
    u8Check_Status = MS_Cfd_OSD_Process_SetColorMetry(pstu_OSD_Process_Configs,pstu_api_HDMI_InfoFrame_Param_out);

}

void MS_Cfd_OSD_Process_SetColorMetry_fromFormat(STU_CFDAPI_OSD_PROCESS_CONFIGS *pstu_OSD_Process_Configs, MS_U8 TempFormat)
{
    //MS_U8 u8Check_Status = 0;
    MS_U8 u8idx = 0;
    MS_U8 u8idxub = 0;

#if 0
    u8idxub = sizeof(u8Const_LUT_MMInfor_HDMIDefinition)/(sizeof(MS_U8)*5);

    for (u8idx =0; u8idx < u8idxub; u8idx++)
    {
        if ((u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3] == TempFormat)||
            (u8Const_LUT_MMInfor_HDMIDefinition[u8idx][4] == TempFormat))
        {
            break;
        }
    }
#else
    u8idx = MS_Cfd_FindMatchedIdx(TempFormat);
#endif
    pstu_OSD_Process_Configs->u8Video_colorprimary = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0];
    //pstu_ControlParam->u8InputTransferCharacterstics = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][1];
    pstu_OSD_Process_Configs->u8Video_MatrixCoefficients = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][2];

    //0:current HDMI input is one case of HDMI colormetry
    //1:current HDMI input is not one case of HDMI colormetry,
    //return u8Check_Status;
}

void MS_Cfd_OSD_Process_configs_print(STU_CFDAPI_OSD_PROCESS_CONFIGS *pstu_OSD_Process_Configs)
{
    printf("\n  OSD:  \n");
    printf("\n  OSD:  u8OSD_SDR2HDR_en=%d;    \n", pstu_OSD_Process_Configs->u8OSD_SDR2HDR_en );
    printf("\n  OSD:  u8OSD_IsFullRange=%d;    \n", pstu_OSD_Process_Configs->u8OSD_IsFullRange );
    printf("\n  OSD:  u8OSD_Dataformat=%d;    \n", pstu_OSD_Process_Configs->u8OSD_Dataformat );
    printf("\n  OSD:  u8OSD_HDRMode=%d;    \n", pstu_OSD_Process_Configs->u8OSD_HDRMode );
    printf("\n  OSD:  u8OSD_colorprimary=%d;    \n", pstu_OSD_Process_Configs->u8OSD_colorprimary );
    printf("\n  OSD:  u8OSD_transferf=%d;    \n", pstu_OSD_Process_Configs->u8OSD_transferf );
    printf("\n  OSD:  u16OSD_MaxLumInNits=%d;    \n", pstu_OSD_Process_Configs->u16OSD_MaxLumInNits );
    printf("\n  OSD:  u16AntiTMO_SourceInNits=%d;    \n", pstu_OSD_Process_Configs->u16AntiTMO_SourceInNits );
    printf("\n");
    printf("\n  OSD:  u8Video_colorprimary=%d;    \n", pstu_OSD_Process_Configs->u8Video_colorprimary );
    printf("\n  OSD:  u8Video_MatrixCoefficients=%d;    \n", pstu_OSD_Process_Configs->u8Video_MatrixCoefficients );
    printf("\n  OSD:  u8Video_HDRMode=%d;    \n", pstu_OSD_Process_Configs->u8Video_HDRMode );
    printf("\n  OSD:  u16Video_MaxLumInNits=%d;    \n", pstu_OSD_Process_Configs->u16Video_MaxLumInNits );
    printf("\n  OSD:  u8Video_IsFullRange=%d;    \n", pstu_OSD_Process_Configs->u8Video_IsFullRange );
    printf("\n  OSD:  u8Video_Dataformat=%d;    \n", pstu_OSD_Process_Configs->u8Video_Dataformat );

#if (0 == RealChip)

//produce test vectors

    FILE *fp;

    fp = fopen("testvector_OSD_process.txt","wt");

    fprintf(fp,"\n\n");
    fprintf(fp,"\n pstu_OSD_Process_Configs->u8OSD_SDR2HDR_en=%d    \n", pstu_OSD_Process_Configs->u8OSD_SDR2HDR_en );
    fprintf(fp,"\n pstu_OSD_Process_Configs->u8OSD_IsFullRange=%d    \n", pstu_OSD_Process_Configs->u8OSD_IsFullRange );
    fprintf(fp,"\n pstu_OSD_Process_Configs->u8OSD_Dataformat=%d    \n", pstu_OSD_Process_Configs->u8OSD_Dataformat );
    fprintf(fp,"\n pstu_OSD_Process_Configs->u8OSD_HDRMode=%d    \n", pstu_OSD_Process_Configs->u8OSD_HDRMode );
    fprintf(fp,"\n pstu_OSD_Process_Configs->u8OSD_colorprimary=%d    \n", pstu_OSD_Process_Configs->u8OSD_colorprimary );
    fprintf(fp,"\n pstu_OSD_Process_Configs->u8OSD_transferf=%d    \n", pstu_OSD_Process_Configs->u8OSD_transferf );
    fprintf(fp,"\n pstu_OSD_Process_Configs->u16OSD_MaxLumInNits=%d    \n", pstu_OSD_Process_Configs->u16OSD_MaxLumInNits );
    fprintf(fp,"\n");
    fprintf(fp,"\n pstu_OSD_Process_Configs->u8Video_colorprimary=%d    \n", pstu_OSD_Process_Configs->u8Video_colorprimary );
    fprintf(fp,"\n pstu_OSD_Process_Configs->u8Video_MatrixCoefficients=%d    \n", pstu_OSD_Process_Configs->u8Video_MatrixCoefficients );
    fprintf(fp,"\n pstu_OSD_Process_Configs->u8Video_HDRMode=%d    \n", pstu_OSD_Process_Configs->u8Video_HDRMode );
    fprintf(fp,"\n pstu_OSD_Process_Configs->u16Video_MaxLumInNits=%d    \n", pstu_OSD_Process_Configs->u16Video_MaxLumInNits );
    fprintf(fp,"\n pstu_OSD_Process_Configs->u8Video_IsFullRange=%d    \n", pstu_OSD_Process_Configs->u8Video_IsFullRange );
    fprintf(fp,"\n pstu_OSD_Process_Configs->u8Video_Dataformat=%d    \n", pstu_OSD_Process_Configs->u8Video_Dataformat );

    fclose(fp);

#endif

}

//for the output infoFrame of box case
void MS_Cfd_HandleOutInfoFrame(STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT *pstu_api_HDMI_InfoFrame_Param,
                               STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam,
                               STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParamInit,
                               STU_CFDAPI_MAIN_CONTROL *pstApiControlParamInit,
                               STU_CFDAPI_MM_PARSER    *pstu_MM_Param,
                               STU_CFDAPI_HDMI_INFOFRAME_PARSER *pstu_api_HDMI_InfoFrame_Param_in,
                               STU_CFDAPI_OSD_CONTROL  *pstu_OSD_Param,
                               STU_CFDAPI_OSD_PROCESS_CONFIGS *pstu_OSD_Process_Configs)
{

    MS_U16 tempx;
    MS_U16 tempy;

    //1: output
    //0: not output
    MS_U8 u8OutputMasteringInfor;

    tempx = 0;
    tempy = 0;

    //
    pstu_OSD_Process_Configs->u8OSD_SDR2HDR_en = 0;
    pstu_OSD_Process_Configs->u8Video_HDRMode = E_CFIO_MODE_SDR;
    //check HDR data block exists

    if (E_CFD_VALID == pstHDMI_EDIDParamInit->u8HDMISink_HDRData_Block_Valid)
    {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_HDR_InfoFrame_Valid = E_CFD_VALID;
    }
    else
    {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_HDR_InfoFrame_Valid = E_CFD_NOT_VALID;
    }

    if (E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Output_HDRMode)
    {
        if (2 != pstu_OSD_Param->u8HDR_UI_H2SMode)
        {
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_EOTF = E_CFD_HDMI_EOTF_SMPTE2084;

            pstu_OSD_Process_Configs->u8OSD_SDR2HDR_en = 1;
            pstu_OSD_Process_Configs->u8Video_HDRMode = E_CFIO_MODE_HDR2;

        }
        else
        {
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_EOTF = E_CFD_HDMI_EOTF_HDR_GAMMA;
        }
    }
    else if (E_CFIO_MODE_HDR3 == pstu_ControlParam->u8Output_HDRMode)
    {
        if (2 != pstu_OSD_Param->u8HDR_UI_H2SMode)
        {
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_EOTF = E_CFD_HDMI_EOTF_FUTURE_EOTF;
            //the current OSD SDR2HDR function cannot support this case
            pstu_OSD_Process_Configs->u8OSD_SDR2HDR_en = 0;
            pstu_OSD_Process_Configs->u8Video_HDRMode = E_CFIO_MODE_HDR3;
        }
        else
        {
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_EOTF = E_CFD_HDMI_EOTF_HDR_GAMMA;
        }
    }
    else
    {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_EOTF = E_CFD_HDMI_EOTF_HDR_GAMMA;
    }

//printf("\033[1;35m###[Brian][%s][%d]### u8Video_HDRMode2 = %d !!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,pstu_OSD_Process_Configs->u8Video_HDRMode);


#if 0

    pstu_api_HDMI_InfoFrame_Param->u16Master_Panel_Max_Luminance = 4000;
    //data * 1 nits

    pstu_api_HDMI_InfoFrame_Param->u16Master_Panel_Min_Luminance = 500;
    //data * 0.0001 nits

    pstu_api_HDMI_InfoFrame_Param->u16Max_Content_Light_Level = 600;
    //data * 1 nits

    pstu_api_HDMI_InfoFrame_Param->u16Max_Frame_Avg_Light_Level = 128;
    //data * 1 nits

    //order R->G->B
    //BT709
    //data *0.00002 0xC350 = 1
    pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[0] = 0x7D00; //0.64
    pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[1] = 0x3A98; //0.3
    pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[2] = 0x1D4C; //0.15

    pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[0] = 0x4047; //0.33
    pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[1] = 0x7530; //0.6
    pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[2] = 0x0BB8; //0.06

    pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16White_point_x = 0x3D13; //0.3127
    pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16White_point_y = 0x4042; //0.3290

#endif

    //bypass or not bypass

    if ((0 == pstu_ControlParam->u8Input_HDRIPMode) && (0 == pstu_ControlParam->u8Input_SDRIPMode))
    {
        //MM case
        if ((E_CFD_INPUT_SOURCE_STORAGE == pstApiControlParamInit->u8Input_Source)||
            (E_CFD_INPUT_SOURCE_DTV == pstApiControlParamInit->u8Input_Source)||
            (E_CFD_INPUT_SOURCE_JPEG == pstApiControlParamInit->u8Input_Source))
        {

            u8OutputMasteringInfor = MS_Cfd_Set_StaticMD_InfoFrame_fromMM(pstu_api_HDMI_InfoFrame_Param,pstu_MM_Param,pstu_ControlParam);
        }
        else if  (E_CFD_INPUT_SOURCE_HDMI == pstApiControlParamInit->u8Input_Source)//
        {
            u8OutputMasteringInfor = MS_Cfd_Set_StaticMD_InfoFrame_fromHDMI(pstu_api_HDMI_InfoFrame_Param, pstu_api_HDMI_InfoFrame_Param_in, pstu_ControlParam);
        }
        else
        {
            MS_Cfd_Set_StaticMD_InfoFrame_default(pstu_api_HDMI_InfoFrame_Param);
            u8OutputMasteringInfor = 0;
        }
    }
    else
    {
        if ((E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Output_HDRMode) || (E_CFIO_MODE_HDR3 == pstu_ControlParam->u8Output_HDRMode))
        {
            //MM case
            if ((E_CFD_INPUT_SOURCE_STORAGE == pstApiControlParamInit->u8Input_Source)||
                (E_CFD_INPUT_SOURCE_DTV == pstApiControlParamInit->u8Input_Source)||
                (E_CFD_INPUT_SOURCE_JPEG == pstApiControlParamInit->u8Input_Source))
            {

                u8OutputMasteringInfor = MS_Cfd_Set_StaticMD_InfoFrame_fromMM(pstu_api_HDMI_InfoFrame_Param,pstu_MM_Param,pstu_ControlParam);

            }
            else if  (E_CFD_INPUT_SOURCE_HDMI == pstApiControlParamInit->u8Input_Source)//
            {
                u8OutputMasteringInfor = MS_Cfd_Set_StaticMD_InfoFrame_fromHDMI(pstu_api_HDMI_InfoFrame_Param, pstu_api_HDMI_InfoFrame_Param_in, pstu_ControlParam);

            }
            else
            {
                MS_Cfd_Set_StaticMD_InfoFrame_default(pstu_api_HDMI_InfoFrame_Param);
                u8OutputMasteringInfor = 0;
            }
        }
        else
        {
            MS_Cfd_Set_StaticMD_InfoFrame_fromOutputformat(pstu_api_HDMI_InfoFrame_Param,pstu_ControlParam);
            u8OutputMasteringInfor = 0;
        }
    }

    if (1 == u8OutputMasteringInfor)
    {
    pstu_api_HDMI_InfoFrame_Param->u8Mastering_Display_Infor_Valid = E_CFD_VALID;
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_SMD_ID = E_CFD_HDMI_META_TYPE1;
    }
    else
    {
        pstu_api_HDMI_InfoFrame_Param->u8Mastering_Display_Infor_Valid = E_CFD_NOT_VALID;
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_SMD_ID = E_CFD_HDMI_META_RESERVED;
    }

    //current case is RGB full range out
    //{Y2 Y1 Y0} = {0 0 0} for RGB default
    //{YQ1 YQ0} = {0 1} means full range
    //{Q1 Q0} = {0 1}

    pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format = 0x00;

    //set RGB/YUV
    //YUV default is YUV422

    if (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_DataFormat)
    {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format &= 0xF8;

        //set {YQ1 YQ0}
        //limit
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format &= 0xE7;

        //set {Q1 Q0}
        //LIMIT
        if (E_CFD_CFIO_RANGE_LIMIT == pstu_ControlParam->u8Output_IsFullRange)
        {
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format &= 0x9F;
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format |= 0x20;
        }
        else
        {
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format &= 0x9F;
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format |= 0x40;
        }
    }
    else
    {
        if (E_CFD_MC_FORMAT_YUV422 == pstu_ControlParam->u8Output_DataFormat)
        {
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format &= 0xF8;
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format |= 0x01;
        }
        else if (E_CFD_MC_FORMAT_YUV444 == pstu_ControlParam->u8Output_DataFormat)
        {
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format &= 0xF8;
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format |= 0x02;
        }
        else // (E_CFD_MC_FORMAT_YUV420 == pstu_ControlParam->u8Output_DataFormat)
        {
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format &= 0xF8;
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format |= 0x03;
        }

        //set {YQ1 YQ0}
        //limit
        if (E_CFD_CFIO_RANGE_LIMIT == pstu_ControlParam->u8Output_IsFullRange)
        {
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format &= 0xE7;
        }
        else
        {
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format &= 0xE7;
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format |= 0x08;
        }

        //set {Q1 Q0}
        //default
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format &= 0x9F;
    }

    //MS_U8 u8HDMISource_Support_Format;
    //[2:0] = {Y2 Y1 Y0}
    //[4:3] = {YQ1 YQ0}
    //[6:5] = {Q1 Q0}
    //information in AVI infoFrame

#if (1 == CFD_BypassHDRInforFrameReferEDID)

if (1 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_Mode)
{
    if ((E_CFD_CFIO_ADOBE_RGB == pstu_ControlParam->u8Output_Format) &&
        (0x10==(pstHDMI_EDIDParamInit->u8HDMISink_Extended_Colorspace&0x10)) &&
        (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_DataFormat))
    {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = E_CFD_CFIO_ADOBE_RGB;
    }
    else if ((E_CFD_CFIO_RGB_BT2020 == pstu_ControlParam->u8Output_Format) &&
            (0x80==(pstHDMI_EDIDParamInit->u8HDMISink_Extended_Colorspace&0x80)) &&
            (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_DataFormat))
    {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = E_CFD_CFIO_RGB_BT2020;
    }
    else if ((E_CFD_CFIO_YUV_BT2020_NCL == pstu_ControlParam->u8Output_Format) &&
            (0x40==(pstHDMI_EDIDParamInit->u8HDMISink_Extended_Colorspace&0x40)) &&
            (E_CFD_MC_FORMAT_RGB != pstu_ControlParam->u8Output_DataFormat))
    {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = E_CFD_CFIO_YUV_BT2020_NCL;
    }
    else if ((E_CFD_CFIO_ADOBE_YCC601 == pstu_ControlParam->u8Output_Format) &&
            (0x08==(pstHDMI_EDIDParamInit->u8HDMISink_Extended_Colorspace&0x08)) &&
            (E_CFD_MC_FORMAT_RGB != pstu_ControlParam->u8Output_DataFormat))
    {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = E_CFD_CFIO_ADOBE_YCC601;
    }
    else if ((E_CFD_CFIO_YUV_BT601_525 == pstu_ControlParam->u8Output_Format) &&
            (E_CFD_MC_FORMAT_RGB != pstu_ControlParam->u8Output_DataFormat))
    {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = E_CFD_CFIO_YUV_BT601_525;
    }

    /*
    else if ((E_CFD_CFIO_YUV_BT2020_NCL == pstu_ControlParam->u8Output_Format) &&
             (0x80==(pstHDMI_EDIDParamInit->u8HDMISink_Extended_Colorspace&0x80) &&
             (0 == pstu_ControlParam->u8Input_HDRIPMode) &&
             (0 == pstu_ControlParam->u8Input_SDRIPMode)))
    {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = E_CFD_CFIO_RGB_BT2020;
    }
    else if ((E_CFD_CFIO_ADOBE_YCC601 == pstu_ControlParam->u8Output_Format) &&
             (0x80==(pstHDMI_EDIDParamInit->u8HDMISink_Extended_Colorspace&0x80) &&
             (0 == pstu_ControlParam->u8Input_HDRIPMode) &&
             (0 == pstu_ControlParam->u8Input_SDRIPMode)))
    {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = E_CFD_CFIO_ADOBE_RGB;
    }*/
    else
    {
        if (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_DataFormat)
        {
               pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = E_CFD_CFIO_RGB_BT709;
    }
        else
        {
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = E_CFD_CFIO_YUV_BT709;
        }
    }
}
else
{
    pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = pstu_ControlParam->u8Output_Format;
}
#else
if (1 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_Mode)
{
    if ((E_CFD_CFIO_ADOBE_RGB == pstu_ControlParam->u8Output_Format)&&
        (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_DataFormat))
    {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = E_CFD_CFIO_ADOBE_RGB;
    }
    else if ((E_CFD_CFIO_RGB_BT2020 == pstu_ControlParam->u8Output_Format)&&
            (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_DataFormat))
    {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = E_CFD_CFIO_RGB_BT2020;
    }
    else if ((E_CFD_CFIO_YUV_BT2020_NCL == pstu_ControlParam->u8Output_Format) &&
            (E_CFD_MC_FORMAT_RGB != pstu_ControlParam->u8Output_DataFormat))
    {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = E_CFD_CFIO_YUV_BT2020_NCL;
    }
    else if ((E_CFD_CFIO_ADOBE_YCC601 == pstu_ControlParam->u8Output_Format) &&
            (E_CFD_MC_FORMAT_RGB != pstu_ControlParam->u8Output_DataFormat))
    {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = E_CFD_CFIO_ADOBE_YCC601;
    }
    else if ((E_CFD_CFIO_YUV_BT601_525 == pstu_ControlParam->u8Output_Format) &&
            (E_CFD_MC_FORMAT_RGB != pstu_ControlParam->u8Output_DataFormat))
    {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = E_CFD_CFIO_YUV_BT601_525;
    }
    else
    {
        if (E_CFD_MC_FORMAT_RGB == pstu_ControlParam->u8Output_DataFormat)
        {
        pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = E_CFD_CFIO_RGB_BT709;
    }
        else
        {
            pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = E_CFD_CFIO_YUV_BT709;
        }
    }
}
else
{
    pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace = pstu_ControlParam->u8Output_Format;
}
#endif

    pstu_api_HDMI_InfoFrame_Param->u8Output_DataFormat = pstu_ControlParam->u8Output_DataFormat;

    //MS_Cfd_OSD_Process_configs_update(pstu_OSD_Process_Configs,pstu_api_HDMI_InfoFrame_Param);
    MS_Cfd_OSD_Process_SetColorMetry_fromFormat(pstu_OSD_Process_Configs,pstu_ControlParam->u8Temp_Format[1]);

    //in current driver, force blending in YUV full domain
    pstu_OSD_Process_Configs->u8Video_Dataformat = E_CFD_MC_FORMAT_YUV444;

    pstu_OSD_Process_Configs->u8Video_IsFullRange = pstu_ControlParam->u8Temp_IsFullRange[1];

    if (pstu_api_HDMI_InfoFrame_Param->u16Master_Panel_Max_Luminance != 0)
    {
        pstu_OSD_Process_Configs->u16Video_MaxLumInNits = pstu_api_HDMI_InfoFrame_Param->u16Master_Panel_Max_Luminance;
    }
    else
    {
        pstu_OSD_Process_Configs->u16Video_MaxLumInNits = 4000;
    }

    MS_Cfd_OSD_Process_configs_print(pstu_OSD_Process_Configs);

    //current order is R->G->B
    //change order to G->B->R

#if 1

    tempx = pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[0];
    tempy = pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[0];

    pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[0] = pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[1];
    pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[0] = pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[1];

    pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[1] = pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[2];
    pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[1] = pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[2];

    pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[2] = tempx;
    pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[2] = tempy;

#endif


    //add a display function
    //use printf

    printf("\n  infoFrame for HDMI TX:  u8HDMISource_HDR_InfoFrame_Valid=%d    \n", pstu_api_HDMI_InfoFrame_Param->u8HDMISource_HDR_InfoFrame_Valid );
    printf("\n  infoFrame for HDMI TX:  u8HDMISource_EOTF=%d    \n",                    pstu_api_HDMI_InfoFrame_Param->u8HDMISource_EOTF );
    printf("\n  infoFrame for HDMI TX:  u8HDMISource_SMD_ID=%d    \n",                  pstu_api_HDMI_InfoFrame_Param->u8HDMISource_SMD_ID );
    printf("\n  infoFrame for HDMI TX:  u16Master_Panel_Max_Luminance=%d    \n",        pstu_api_HDMI_InfoFrame_Param->u16Master_Panel_Max_Luminance );
    printf("\n  infoFrame for HDMI TX:  u16Master_Panel_Min_Luminance=%d    \n",        pstu_api_HDMI_InfoFrame_Param->u16Master_Panel_Min_Luminance );
    printf("\n  infoFrame for HDMI TX:  u16Max_Content_Light_Level=%d    \n",           pstu_api_HDMI_InfoFrame_Param->u16Max_Content_Light_Level );
    printf("\n  infoFrame for HDMI TX:  u16Max_Frame_Avg_Light_Level=%d    \n",     pstu_api_HDMI_InfoFrame_Param->u16Max_Frame_Avg_Light_Level );
    printf("\n  infoFrame for HDMI TX:  u16Display_Primaries_x[0]=%d    \n",            pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[0] );
    printf("\n  infoFrame for HDMI TX:  u16Display_Primaries_y[0]=%d    \n",            pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[0] );
    printf("\n  infoFrame for HDMI TX:  u16Display_Primaries_x[1]=%d    \n",            pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[1] );
    printf("\n  infoFrame for HDMI TX:  u16Display_Primaries_y[1]=%d    \n",            pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[1] );
    printf("\n  infoFrame for HDMI TX:  u16Display_Primaries_x[2]=%d    \n",            pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_x[2] );
    printf("\n  infoFrame for HDMI TX:  u16Display_Primaries_y[2]=%d    \n",            pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16Display_Primaries_y[2] );
    printf("\n  infoFrame for HDMI TX:  u16White_point_x=%d    \n",                     pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16White_point_x );
    printf("\n  infoFrame for HDMI TX:  u16White_point_y=%d    \n",                     pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry.u16White_point_y );
    printf("\n  infoFrame for HDMI TX:  u8Mastering_Display_Infor_Valid=%d    \n",  pstu_api_HDMI_InfoFrame_Param->u8Mastering_Display_Infor_Valid );
    printf("\n  infoFrame for HDMI TX:  u8HDMISource_Support_Format=0x%02x    \n",      pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Support_Format );
    printf("\n  infoFrame for HDMI TX:  u8HDMISource_Colorspace=%d    \n",              pstu_api_HDMI_InfoFrame_Param->u8HDMISource_Colorspace );
    printf("\n  infoFrame for HDMI TX:  u8Output_DataFormat=%d    \n",              pstu_api_HDMI_InfoFrame_Param->u8Output_DataFormat );

}


MS_U8 MS_Cfd_SetFormat_FromGamutOrderIdx(MS_U8 u8GamutOrderIdx, MS_U8 u8DataFormat)
{

    MS_U8 temp_GamutOrderIdx;
    MS_U8 temp_DataFormat;
    MS_U8 temp_Format;
    //MS_U8 temp_ColorPriamries;
    //MS_U8 temp_TransferCharacterstics;
    //MS_U8 temp_MatrixCoefficients;

    temp_GamutOrderIdx = u8GamutOrderIdx;
    temp_DataFormat = u8DataFormat;

    //only YUV
    if ( E_CFD_CFIO_GT_BT601_525 == temp_GamutOrderIdx)
    {
        temp_Format = E_CFD_CFIO_YUV_BT601_525;
    }
    else if ( E_CFD_CFIO_GT_BT709 == temp_GamutOrderIdx)
    {
        if (E_CFD_MC_FORMAT_RGB == temp_DataFormat)
        {
            temp_Format = E_CFD_CFIO_RGB_BT709;
        }
        else
        {
            temp_Format = E_CFD_CFIO_YUV_BT709;
        }
    }
    else if ( E_CFD_CFIO_GT_ADOBERGB == temp_GamutOrderIdx) //Only RGB
    {
        if (E_CFD_MC_FORMAT_RGB == temp_DataFormat)
        {
            temp_Format = E_CFD_CFIO_ADOBE_RGB;
        }
        else
        {
            temp_Format = E_CFD_CFIO_ADOBE_YCC601;
        }
    }
    else if ( E_CFD_CFIO_GT_BT2020 == temp_GamutOrderIdx)
    {

        if (E_CFD_MC_FORMAT_RGB == temp_DataFormat)
        {
            temp_Format = E_CFD_CFIO_RGB_BT2020;
        }
        else
        {
            temp_Format = E_CFD_CFIO_YUV_BT2020_NCL;
        }
    }
    else
    {
        if (E_CFD_MC_FORMAT_RGB == temp_DataFormat)
        {
            temp_Format = E_CFD_CFIO_RGB_BT709;
        }
        else
        {
            temp_Format = E_CFD_CFIO_YUV_BT709;
        }
    }

    return temp_Format;

}

MS_U16 MS_Cfd_HDREDID_ErrDetect(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam)
{
    MS_U8 u8Err_flag = 0;

    if (pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMax < 100)
    {
        u8Err_flag = 1;
    }

    if (pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMax < pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMed)
    {
        u8Err_flag = 1;
    }

    if (0 == pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMinFlag)
    {
        if (pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMed < pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMin)
        {
            u8Err_flag = 1;
        }

    }

    if (1 == pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMinFlag)
    {
        if (pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMed <= 1)
        {
            u8Err_flag = 1;
        }
    }

    if (u8Err_flag!=0)
        return E_CFD_ERR_HDMI_EDID_HDR_LUM;
    else
        return E_CFD_MC_ERR_NOERR;
}

void MS_Cfd_HDREDID_ErrHandle(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam, STU_CFDAPI_MAIN_CONTROL *pstApiControlParamInit, MS_U16 u16Err_Status)
{
    if (E_CFD_MC_ERR_NOERR != u16Err_Status)
    {
        pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMax = pstApiControlParamInit->u16Target_Max_Luminance;
        pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMed = pstApiControlParamInit->u16Target_Med_Luminance;
        pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMin = pstApiControlParamInit->u16Target_Min_Luminance;
    }
}

//check if we can do bypass
//have to do GM or not
//0:
//1:

MS_U8 MS_Cfd_CheckGM_HDMISink(STU_CFDAPI_MAIN_CONTROL *pstApiControlParamInit, STU_CFDAPI_HDMI_EDID_PARSER  *pstHDMI_EDIDParamInit)
{
    MS_U8 u8Status = 1;
    MS_U8 u8CInput_Format = 0;
    MS_U8 u8CHDMISink_Extended_Colorspace = 0;

    u8CInput_Format = pstApiControlParamInit->u8Input_Format;
    u8CHDMISink_Extended_Colorspace = pstHDMI_EDIDParamInit->u8HDMISink_Extended_Colorspace;

    if ((E_CFD_CFIO_XVYCC_601 == u8CInput_Format) && (0x01 == (u8CHDMISink_Extended_Colorspace&0x01) ))
    {
        u8Status = 0;
    }
    else if ((E_CFD_CFIO_XVYCC_709 == u8CInput_Format) && (0x02 == (u8CHDMISink_Extended_Colorspace&0x02) ))
    {
        u8Status = 0;
    }
    else if ((E_CFD_CFIO_SYCC601 == u8CInput_Format) && (0x04 == (u8CHDMISink_Extended_Colorspace&0x04) ))
    {
        u8Status = 0;
    }
    else if ((E_CFD_CFIO_ADOBE_YCC601 == u8CInput_Format) && (0x08 == (u8CHDMISink_Extended_Colorspace&0x08) ))
    {
        u8Status = 0;
    }
    else if ((E_CFD_CFIO_ADOBE_RGB == u8CInput_Format) && (0x10 == (u8CHDMISink_Extended_Colorspace&0x10) ))
    {
        u8Status = 0;
    }
    else if ((E_CFD_CFIO_YUV_BT2020_CL == u8CInput_Format) && (0x20 == (u8CHDMISink_Extended_Colorspace&0x20) ))
    {
        u8Status = 0;
    }
    else if ((E_CFD_CFIO_YUV_BT2020_NCL == u8CInput_Format) && (0x40 == (u8CHDMISink_Extended_Colorspace&0x40) ))
    {
        u8Status = 0;
    }
    else if ((E_CFD_CFIO_RGB_BT2020 == u8CInput_Format) && (0x80 == (u8CHDMISink_Extended_Colorspace&0x80) ))
    {
        u8Status = 0;
    }

    return u8Status;
}


void MS_Cfd_SetColorimetryByDefault(STU_CFD_COLORIMETRY *pstu_colorimetry)
{
    //the default order is R->G->B
#if 0
    //set color primaries to BT709 and white porint to D65
    pstu_colorimetry->u16Display_Primaries_x[0] = 32000;
    pstu_colorimetry->u16Display_Primaries_y[0] = 16500;
    pstu_colorimetry->u16Display_Primaries_x[1] = 15000;
    pstu_colorimetry->u16Display_Primaries_y[1] = 30000;
    pstu_colorimetry->u16Display_Primaries_x[2] = 7500;
    pstu_colorimetry->u16Display_Primaries_y[2] = 3000;

    pstu_colorimetry->u16White_point_x = 15635;
    pstu_colorimetry->u16White_point_y = 16450;
#else
    //set color primaries to BT2020 and white porint to D65
    pstu_colorimetry->u16Display_Primaries_x[0] = 35400;
    pstu_colorimetry->u16Display_Primaries_y[0] = 14600;
    pstu_colorimetry->u16Display_Primaries_x[1] = 8500;
    pstu_colorimetry->u16Display_Primaries_y[1] = 39850;
    pstu_colorimetry->u16Display_Primaries_x[2] = 6550;
    pstu_colorimetry->u16Display_Primaries_y[2] = 2300;

    pstu_colorimetry->u16White_point_x = 15635;
    pstu_colorimetry->u16White_point_y = 16450;
#endif
}

//---------------------------------------------------------------------------------------------------------------
//this function parses the color primaries from HDMI HDR infoFrame,
//set these values to CFD internal variables with R->G->B order

//@Param
//input  : *pstu_colorimetry_input
//output : *pstu_colorimetry_pasing

//return values : 0 : no error
//                1 : sth wrong , make the primaries equal to BT2020
//---------------------------------------------------------------------------------------------------------------

MS_U16 MS_Cfd_AutoParseColorimetry(STU_CFD_COLORIMETRY *pstu_colorimetry_pasing, STU_CFD_COLORIMETRY *pstu_colorimetry_input)
{

    MS_U16 u16msg;

    MS_U8 g_index;
    MS_U8 r_index;
    MS_U8 b_index;

    MS_U8 temp_index;

    g_index = 0;
    r_index = 0;

    u16msg = E_CFD_MC_ERR_NOERR;

    for (temp_index = 1; temp_index<3 ; temp_index++)
    {
        if (pstu_colorimetry_input->u16Display_Primaries_x[temp_index] > pstu_colorimetry_input->u16Display_Primaries_x[r_index])
        {
            r_index = temp_index;
        }

        if (pstu_colorimetry_input->u16Display_Primaries_y[temp_index] > pstu_colorimetry_input->u16Display_Primaries_y[g_index])
        {
            g_index = temp_index;
        }
    }

    if (r_index == g_index)
    {
        u16msg = E_CFD_MC_ERR_HDMI_INFOFRAME_Mastering_Display_Coordinates;
    }
    else
    {

        b_index = 3-(g_index+r_index);

        //R
        pstu_colorimetry_pasing->u16Display_Primaries_x[0] = pstu_colorimetry_input->u16Display_Primaries_x[r_index];
        pstu_colorimetry_pasing->u16Display_Primaries_y[0] = pstu_colorimetry_input->u16Display_Primaries_y[r_index];
        //G
        pstu_colorimetry_pasing->u16Display_Primaries_x[1] = pstu_colorimetry_input->u16Display_Primaries_x[g_index];
        pstu_colorimetry_pasing->u16Display_Primaries_y[1] = pstu_colorimetry_input->u16Display_Primaries_y[g_index];
        //B
        pstu_colorimetry_pasing->u16Display_Primaries_x[2] = pstu_colorimetry_input->u16Display_Primaries_x[b_index];
        pstu_colorimetry_pasing->u16Display_Primaries_y[2] = pstu_colorimetry_input->u16Display_Primaries_y[b_index];
    }

    return u16msg;
}

MS_U16 MS_Cfd_SourceMax_Clip_MM(STU_CFDAPI_MM_PARSER *pstu_MM_Param)
{
    MS_U16 u16ClipMax;

    //both valid
    if ((E_CFD_VALID == pstu_MM_Param->u8Mastering_Display_Infor_Valid) && (E_CFD_VALID == pstu_MM_Param->u8MM_HDR_ContentLightMetaData_Valid))
    {
        u16ClipMax = min((pstu_MM_Param->u32Master_Panel_Max_Luminance/10000),pstu_MM_Param->u16Max_content_light_level);
    }
    else if ((E_CFD_VALID == pstu_MM_Param->u8Mastering_Display_Infor_Valid) && (E_CFD_NOT_VALID == pstu_MM_Param->u8MM_HDR_ContentLightMetaData_Valid))
    {
        u16ClipMax = (pstu_MM_Param->u32Master_Panel_Max_Luminance/10000);
    }
    else if ((E_CFD_NOT_VALID == pstu_MM_Param->u8Mastering_Display_Infor_Valid) && (E_CFD_VALID == pstu_MM_Param->u8MM_HDR_ContentLightMetaData_Valid))
    {
        u16ClipMax = pstu_MM_Param->u16Max_content_light_level;
    }
    else //both not valid
    {
        u16ClipMax = 0;
    }

    if (DePQClampNOffset < u16ClipMax)
    {
    u16ClipMax = max((u16ClipMax-DePQClampNOffset),DePQClampMin);
    }

    return u16ClipMax;
}

MS_U16 MS_Cfd_SourceMax_Clip_HDMI(STU_CFDAPI_HDMI_INFOFRAME_PARSER *pstu_api_HDMI_InfoFrame_Param)
{
    MS_U16 u16ClipMax;

    //HDR infoFrame valid
    if ((E_CFD_VALID == pstu_api_HDMI_InfoFrame_Param->u8HDMISource_HDR_InfoFrame_Valid) &&
       (E_CFD_VALID == pstu_api_HDMI_InfoFrame_Param->u8Mastering_Display_Infor_Valid) &&
       (0 == pstu_api_HDMI_InfoFrame_Param->u8HDMISource_SMD_ID))
    {
        u16ClipMax = min(pstu_api_HDMI_InfoFrame_Param->u16Master_Panel_Max_Luminance,pstu_api_HDMI_InfoFrame_Param->u16Max_Content_Light_Level);
    }
    else //both not valid
    {
        u16ClipMax = 0;
    }

    if (DePQClampNOffset < u16ClipMax)
    {
        u16ClipMax = max((u16ClipMax-DePQClampNOffset),DePQClampMin);
    }

    return u16ClipMax;
}

MS_U16 Mapi_Cfd_ColorFormatDriver_Control(STU_CFDAPI_TOP_CONTROL *pstu_Cfd_api_top)
{

    MS_U8 u8_unit_flag = 0;
    MS_U8 *pu8_unit_flag;
    pu8_unit_flag = &u8_unit_flag;

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

    MS_U8 u8tempIdx;
    MS_U8 u8tempMode;

    MS_U8 u8HaveToDoFlag;
    MS_U8 u8tempFlag;

    //0: before input format parsing
    //1: after input format parsing
    MS_U8 after_preconstraints_control;

    #if ((NowHW == Curry) || (NowHW == Kastor) || (NowHW == Kano))
    after_preconstraints_control = 1;
    #else
    after_preconstraints_control = 0;
    #endif

    //printf("\033[1;35m###[Brian][%s][%d]### Into CFD main function!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__);

    //interior stucture
    STU_CFD_MS_ALG_COLOR_FORMAT stu_ControlParam;
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam;
    pstu_ControlParam = &stu_ControlParam;
    memset(&stu_ControlParam, 0, sizeof(STU_CFD_MS_ALG_COLOR_FORMAT));
    MS_IN_Cfd_TMO_Control_Param_Init(&(stu_ControlParam.stu_CFD_TMO_Param));
    pstu_ControlParam->u8HW_MainSub_Mode = pstu_Cfd_api_top->pstu_HW_IP_Param->u8HW_MainSub_Mode;
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

    memcpy(&stu_HDRIP_Input,pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_HDRIP_Input,sizeof(STU_CFD_MS_ALG_INTERFACE_HDRIP));
    memcpy(&stu_SDRIP_Input,pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_SDRIP_Input,sizeof(STU_CFD_MS_ALG_INTERFACE_SDRIP));
    memcpy(&stu_TMO_Input,pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_TMO_Input,sizeof(STU_CFD_MS_ALG_INTERFACE_TMO));
    memcpy(&stu_DLC_Input,pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_DLC_Input,sizeof(STU_CFD_MS_ALG_INTERFACE_DLC));

//Maserati M+D IP patch 0
#if (NowHW == Maserati) || (NowHW == Mainz)
#if HW_PIPE_RANGE == E_CFD_CFIO_RANGE_LIMIT
    stu_HDRIP_Input.stu_Maserati_HDRIP_Param.u8HDRIP_Patch = 0;
    stu_HDRIP_Input.stu_Maserati_HDRIP_Param.u8HDRIP_Patch = (stu_HDRIP_Input.stu_Maserati_HDRIP_Param.u8HDRIP_Patch | 0x01);
#endif
#endif

    //API structure group
    STU_CFDAPI_MAIN_CONTROL                 *pstApiControlParamInit;
    STU_CFDAPI_MM_PARSER                    *pstu_MM_Param;
    STU_CFDAPI_HDMI_EDID_PARSER             *pstHDMI_EDIDParamInit;
    STU_CFDAPI_HDMI_INFOFRAME_PARSER        *pstu_api_HDMI_InfoFrame_Param;
    //STU_CFDAPI_HDR_FORMAT            *pstu_HDR_Format_Param;
    STU_CFDAPI_PANEL_FORMAT                 *pstu_Panel_Param;
    STU_CFDAPI_OSD_CONTROL                  *pstu_OSD_Param;
    STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT    *pstu_api_HDMI_InfoFrame_Param_out;

    STU_CFDAPI_OSD_PROCESS_CONFIGS          *pstu_OSD_Process_Configs;

    //STU_CFDAPI_TMO_CONTROL *pstu_TMO_Param;

    pstApiControlParamInit =                pstu_Cfd_api_top->pstu_Main_Control;
    pstu_MM_Param          =                pstu_Cfd_api_top->pstu_MM_Param;
    pstHDMI_EDIDParamInit  =                pstu_Cfd_api_top->pstu_HDMI_EDID_Param;
    pstu_api_HDMI_InfoFrame_Param =         pstu_Cfd_api_top->pstu_HDMI_InfoFrame_Param;
    pstu_api_HDMI_InfoFrame_Param_out =     pstu_Cfd_api_top->pstu_HDMI_InfoFrame_Param_out;
    pstu_Panel_Param       =                pstu_Cfd_api_top->pstu_Panel_Param;
    pstu_OSD_Param         =                pstu_Cfd_api_top->pstu_OSD_Param;
    //pstu_TMO_Param    =               pstu_Cfd_api_top->pstu_TMO_Param;

    pstu_OSD_Process_Configs =              pstu_Cfd_api_top->pstu_OSD_Process_Configs;

    stu_ControlParam.pstu_Panel_Param_Colorimetry = &(pstu_Cfd_api_top->pstu_Panel_Param->stu_Cfd_Panel_ColorMetry);

    //assign TMO controls to interior parameters

    stu_ControlParam.stu_CFD_TMO_Param.u8TMO_SourceUserModeEn = pstApiControlParamInit->u8TMO_SourceUserModeEn;
    stu_ControlParam.stu_CFD_TMO_Param.u8TMO_SourceUserMode   = pstApiControlParamInit->u8TMO_SourceUserMode;
    stu_ControlParam.stu_CFD_TMO_Param.u8TMO_TargetUserModeEn = pstApiControlParamInit->u8TMO_TargetUserModeEn;
    stu_ControlParam.stu_CFD_TMO_Param.u8TMO_TargetUserMode   = pstApiControlParamInit->u8TMO_TargetUserMode;
    stu_ControlParam.stu_CFD_TMO_Param.u8TMO_TargetRefer_Mode = pstApiControlParamInit->u8TMO_TargetRefer_Mode;

    MS_Cfd_SetColorimetryByDefault(&(pstu_ControlParam->stu_source_mastering_display));

#if (NowHW == Maserati) || (NowHW == Mainz)

    //force some values when not Dolby
    if (( E_CFD_CFIO_DOLBY_HDR_TEMP != pstApiControlParamInit->u8Input_Format) || ( E_CFIO_MODE_HDR1 != pstApiControlParamInit->u8Input_HDRMode))
    {
        pstApiControlParamInit->stu_Middle_Format[0].u8Mid_IsFullRange = HW_PIPE_RANGE;
    }

#endif

    u8MainSubMode = pstu_Cfd_api_top->pstu_HW_IP_Param->u8HW_MainSub_Mode;

#if (0 == RealChip) //not used in STB?
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
#endif

    //input control check  function ================================================================

    u16_check_status_tmp = Mapi_Cfd_inter_Main_Control_Param_Check(pstApiControlParamInit,1);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
    }

    u16_check_status_tmp = Mapi_Cfd_inter_TOP_Param_CrossCheck(pstu_Cfd_api_top);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
    }

    //==============================================================================================

#if (NowHW == Maserati) || (NowHW == Mainz)

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

#elif (NowHW == Kano)

#ifdef HE_PIPE_PRECONSTRAINTS

    if (3 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_Mode)
    {
        u8tempMode = 1;
    }
    else
    {
        u8tempMode = 0;
    }

    u16_check_status_tmp = Mapi_Cfd_inter_Main_Control_PreConstraints_Kano(pstApiControlParamInit,pstu_ControlParam,pstHDMI_EDIDParamInit, u8tempMode, pstu_OSD_Param);
    stu_ControlParam.u8DoPreConstraints_Flag = 1;

#endif

#elif ((NowHW == Curry) || (NowHW == Kastor))

#ifdef HE_PIPE_PRECONSTRAINTS
    //if (( E_CFD_CFIO_DOLBY_HDR_TEMP != pstApiControlParamInit->u8Input_Format) || ( E_CFIO_MODE_HDR1 != pstApiControlParamInit->u8Input_HDRMode))
    if (( E_CFD_CFIO_DOLBY_HDR_TEMP != pstApiControlParamInit->u8Input_Format) || ( E_CFIO_MODE_HDR1 != pstApiControlParamInit->u8Input_HDRMode))
    {

        if (3 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_Mode)
        {
            u8tempMode = 1;
        }
        else
        {
            u8tempMode = 0;
        }
        //printf("\033[1;35m###[Brian][%s][%d]### Into CFD PreConstraints!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__);
        u16_check_status_tmp = Mapi_Cfd_inter_Main_Control_PreConstraints_Curry(pstApiControlParamInit,pstu_ControlParam,pstHDMI_EDIDParamInit, u8tempMode, pstu_OSD_Param);
        stu_ControlParam.u8DoPreConstraints_Flag = 1;
    }
#endif

#endif

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
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
            //check for MM input
            u16_check_status_tmp = Mapi_Cfd_inter_MM_Param_Check(pstu_MM_Param);

            if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
            {
                u16_check_status = u16_check_status_tmp;
                u8ERR_Happens_Flag ++;
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
                            u8ERR_Happens_Flag ++;
                        }

#if 0
                    if (1 == stu_ControlParam.u8DoMMIn_Force709_Flag)
                    {
                        printf("\n  Force:input_format is forced to BT709 RGB or YUV due to not mapping to a HDMI case\n"));
                    }
#endif
            }
            else if (2 == pstApiControlParamInit->u8MMInput_ColorimetryHandle_Mode)
            {
                    //force MMinput to SDR YUV limit 709
                    stu_ControlParam.u8DoMMIn_Force709_Flag = 1;
                    MS_Cfd_ForceMMInputToRec709(&stu_ControlParam,pstu_MM_Param->u8Transfer_Characteristics);
            }
            else
            {
                    //do nothing
                    //stu_ControlParam.u8InputColorPriamries = pstu_MM_Param->u8Colour_primaries;
                    //stu_ControlParam.u8InputTransferCharacterstics = pstu_MM_Param->u8Transfer_Characteristics;
                    //stu_ControlParam.u8InputMatrixCoefficients = pstu_MM_Param->u8Matrix_Coeffs;
                    //handle undefined case of color primary
#if 0
                    if((E_CFD_CFIO_CP_RESERVED0 == pstu_MM_Param->u8Colour_primaries) ||
                                (E_CFD_CFIO_CP_UNSPECIFIED == pstu_MM_Param->u8Colour_primaries) ||
                                (E_CFD_CFIO_CP_RESERVED3 == pstu_MM_Param->u8Colour_primaries) ||
                                (E_CFD_CFIO_CP_RESERVED_START <= pstu_MM_Param->u8Colour_primaries))
#endif
                        if (MS_Cfd_checkColorprimaryIsUndefined(&(pstu_MM_Param->u8Colour_primaries)))
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
                            u8ERR_Happens_Flag ++;
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
#if (0 == DePQClamp_EN)
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = pstu_MM_Param->u32Master_Panel_Max_Luminance/10000;
#else
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = MS_Cfd_SourceMax_Clip_MM(pstu_MM_Param);
#endif
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMin = pstu_MM_Param->u32Master_Panel_Min_Luminance;
            pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_SourceStatus = 0;
        }
        else if ((2 == pstApiControlParamInit->u8TMO_TargetRefer_Mode) && (E_CFD_VALID == pstu_MM_Param->u8Mastering_Display_Infor_Valid))
        {

#if (0 == DePQClamp_EN)
            if ((0 == pstApiControlParamInit->u8TMO_SourceUserModeEn) || ((1 == pstApiControlParamInit->u8TMO_SourceUserModeEn) && (0 == pstApiControlParamInit->u8TMO_SourceUserMode)))
            {
                pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = pstu_MM_Param->u32Master_Panel_Max_Luminance/10000;
            }
            else
            {
                pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = pstApiControlParamInit->u16Source_Max_Luminance;
            }
#else
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = MS_Cfd_SourceMax_Clip_MM(pstu_MM_Param);
            //printf("\033[1;35m###[Brian][%s][%d]### pstu_Control_Param->stu_CFD_TMO_Param.u16SourceMax 0 = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax);
#endif
            if ((0 == pstApiControlParamInit->u8TMO_SourceUserModeEn) || ((1 == pstApiControlParamInit->u8TMO_SourceUserModeEn) && (0 == pstApiControlParamInit->u8TMO_SourceUserMode)))
            {
                pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMin = pstu_MM_Param->u32Master_Panel_Min_Luminance;
                pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_SourceStatus = 0;
            }
            else
            {
                pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMin = pstApiControlParamInit->u16Source_Min_Luminance;
                pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_SourceStatus = 2;
            }
        }
        else
        {
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = pstApiControlParamInit->u16Source_Max_Luminance;
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMin = pstApiControlParamInit->u16Source_Min_Luminance;
            pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_SourceStatus = 2;
        }
        pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMed = pstApiControlParamInit->u16Source_Med_Luminance;

        //source mastering display color primaries

        if (E_CFD_VALID == pstu_MM_Param->u8Mastering_Display_Infor_Valid)
        {
            //order is R->G->B
            memcpy(&(pstu_ControlParam->stu_source_mastering_display),&(pstu_MM_Param->stu_Cfd_MM_MasterPanel_ColorMetry),sizeof(STU_CFD_COLORIMETRY));
        }
        else
        {
            //set 709 gamut
            MS_Cfd_SetColorimetryByDefault(&(pstu_ControlParam->stu_source_mastering_display));
        }

    }
    else if  (E_CFD_INPUT_SOURCE_HDMI == pstApiControlParamInit->u8Input_Source) //TV, STB
    {

        //find u8InputFormat,u8InputIsFullRange,u8InputGamutIdx from outside information
        //u8InputColorPriamries,u8InputTransferCharacterstics,u8InputMatrixCoefficients

        //check input
        u16_check_status_tmp = Mapi_Cfd_inter_HDMI_InfoFrame_Param_Check(pstu_api_HDMI_InfoFrame_Param);

        if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
        {
            u16_check_status = u16_check_status_tmp;
            u8ERR_Happens_Flag ++;
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
                    u8ERR_Happens_Flag ++;
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
        if ((1 == pstApiControlParamInit->u8TMO_TargetRefer_Mode) &&
            (E_CFD_VALID == pstu_api_HDMI_InfoFrame_Param->u8HDMISource_HDR_InfoFrame_Valid) &&
            (E_CFD_VALID == pstu_api_HDMI_InfoFrame_Param->u8Mastering_Display_Infor_Valid))
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
            pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_SourceStatus = 0;
        }
        else if ((2 == pstApiControlParamInit->u8TMO_TargetRefer_Mode) &&
            (E_CFD_VALID == pstu_api_HDMI_InfoFrame_Param->u8HDMISource_HDR_InfoFrame_Valid) &&
            (E_CFD_VALID == pstu_api_HDMI_InfoFrame_Param->u8Mastering_Display_Infor_Valid))
        {
#if (0 == DePQClamp_EN)
            if ((0 == pstApiControlParamInit->u8TMO_SourceUserModeEn) || ((1 == pstApiControlParamInit->u8TMO_SourceUserModeEn) && (0 == pstApiControlParamInit->u8TMO_SourceUserMode)))
            {
                pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = pstu_api_HDMI_InfoFrame_Param->u16Master_Panel_Max_Luminance;
            }
            else
            {
                pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = pstApiControlParamInit->u16Source_Max_Luminance;
            }
#else
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = MS_Cfd_SourceMax_Clip_HDMI(pstu_api_HDMI_InfoFrame_Param);
#endif
            if ((0 == pstApiControlParamInit->u8TMO_SourceUserModeEn) || ((1 == pstApiControlParamInit->u8TMO_SourceUserModeEn) && (0 == pstApiControlParamInit->u8TMO_SourceUserMode)))
            {
                pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMin = pstu_api_HDMI_InfoFrame_Param->u16Master_Panel_Min_Luminance;
                pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_SourceStatus = 0;
            }
            else
            {
                //pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = pstApiControlParamInit->u16Source_Max_Luminance;
                pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMin = pstApiControlParamInit->u16Source_Min_Luminance;
                pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_SourceStatus = 2;
            }
        }
        else
        {
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMax = pstApiControlParamInit->u16Source_Max_Luminance;
            pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMin = pstApiControlParamInit->u16Source_Min_Luminance;
            pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_SourceStatus = 2;
        }

        pstu_ControlParam->stu_CFD_TMO_Param.u16SourceMed = pstApiControlParamInit->u16Source_Med_Luminance;
#endif

        if ((E_CFD_VALID == pstu_api_HDMI_InfoFrame_Param->u8HDMISource_HDR_InfoFrame_Valid) && (E_CFD_VALID == pstu_api_HDMI_InfoFrame_Param->u8Mastering_Display_Infor_Valid))
        {
            //order is R->G->B
            //memcpy(&(pstu_ControlParam->stu_source_mastering_display),&(pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry),sizeof(STU_CFD_COLORIMETRY));

            //based on
            u16_check_status_tmp = MS_Cfd_AutoParseColorimetry(&(pstu_ControlParam->stu_source_mastering_display), &(pstu_api_HDMI_InfoFrame_Param->stu_Cfd_HDMISource_MasterPanel_ColorMetry));

            if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
            {
                u16_check_status = u16_check_status_tmp;
                u8ERR_Happens_Flag ++;
            }
        }
        else
        {
            //set 709 gamut
            MS_Cfd_SetColorimetryByDefault(&(pstu_ControlParam->stu_source_mastering_display));
        }
    }
    //else if    (E_CFD_MC_SOURCE_ANALOG == pstApiControlParamInit->u8Input_Source)
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
        printf("\033[31m [%s] Analog source\033[m\n", __func__);
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

#if ((NowHW == Curry) || (NowHW == Kastor) || (NowHW == Kano))
#ifdef HE_PIPE_PRECONSTRAINTS

    //assign u8OutputIsFullRange -----------------------------------------------------------------
    if (E_CFD_OUTPUT_SOURCE_HDMI == pstApiControlParamInit->u8Output_Source)
    {
        //update from interior variables
        pstApiControlParamInit->u8Input_Format = stu_ControlParam.u8Input_Format;

        //check Gammutmapping is nessary?
        stu_ControlParam.u8MustGMFlag = MS_Cfd_CheckGM_HDMISink(pstApiControlParamInit,pstHDMI_EDIDParamInit);

        if ((1 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_Mode) && (1 == stu_ControlParam.u8MustGMFlag))
        {
                u16_check_status_tmp = Mapi_Cfd_inter_HDMI_EDID_Param_Check(pstHDMI_EDIDParamInit);

                if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
                {
                    u16_check_status = u16_check_status_tmp;
                    u8ERR_Happens_Flag ++;
                }

                //set u8Output_GamutOrderIdx by EDID or user controls
                MS_Cfd_SetOutputGamut_HDMISink(&u8tempIdx,pstHDMI_EDIDParamInit, &stu_ControlParam, pstApiControlParamInit, 2);

                //set by u8Output_GamutOrderIdx
                //MS_Cfd_SetOutputColorParam_HDMISink(&stu_ControlParam,2);

                pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format = MS_Cfd_SetFormat_FromGamutOrderIdx(u8tempIdx,E_CFD_MC_FORMAT_YUV444);

                u8tempMode = 1;
        }
        else if ((2 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_Mode) && (1 == stu_ControlParam.u8MustGMFlag))
        {
                pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format = E_CFD_CFIO_YUV_BT709;
                u8tempMode = 1;
        }
        else if (3 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_Mode)
        {
                u8tempMode = 1;
        }
        else
        {
                u8tempMode = 0;
        }

    }
    else
    {
        u8tempMode = 0;
    }

#if (NowHW != Kano)
        if ((after_preconstraints_control == 1) && ((E_CFD_INPUT_SOURCE_STORAGE == pstApiControlParamInit->u8Input_Source)||
            (E_CFD_INPUT_SOURCE_DTV == pstApiControlParamInit->u8Input_Source)||
            (E_CFD_INPUT_SOURCE_JPEG == pstApiControlParamInit->u8Input_Source))){
        //if (( E_CFD_CFIO_DOLBY_HDR_TEMP != pstApiControlParamInit->u8Input_Format) || ( E_CFIO_MODE_HDR1 != pstApiControlParamInit->u8Input_HDRMode))
        if (( E_CFD_CFIO_DOLBY_HDR_TEMP != pstApiControlParamInit->u8Input_Format) || ( E_CFIO_MODE_HDR1 != pstApiControlParamInit->u8Input_HDRMode))
        {
            pstApiControlParamInit->u8Input_Format = stu_ControlParam.u8Input_Format;
            u16_check_status_tmp = Mapi_Cfd_inter_Main_Control_PreConstraints_Curry(pstApiControlParamInit,pstu_ControlParam,pstHDMI_EDIDParamInit, u8tempMode, pstu_OSD_Param);
            stu_ControlParam.u8DoPreConstraints_Flag = 1;
        }
        }
#else
    if ((after_preconstraints_control == 1) && ((E_CFD_INPUT_SOURCE_STORAGE == pstApiControlParamInit->u8Input_Source)||
        (E_CFD_INPUT_SOURCE_DTV == pstApiControlParamInit->u8Input_Source)||
        (E_CFD_INPUT_SOURCE_JPEG == pstApiControlParamInit->u8Input_Source)))
    {
            pstApiControlParamInit->u8Input_Format = stu_ControlParam.u8Input_Format;
            u16_check_status_tmp = Mapi_Cfd_inter_Main_Control_PreConstraints_Kano(pstApiControlParamInit,pstu_ControlParam,pstHDMI_EDIDParamInit, u8tempMode, pstu_OSD_Param);
            stu_ControlParam.u8DoPreConstraints_Flag = 1;

    }
#endif

#endif
#endif //#if ((NowHW == Curry) || (NowHW == Kastor) || (NowHW == Kano))

        if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
        {
            u16_check_status = u16_check_status_tmp;
            u8ERR_Happens_Flag ++;
        }

//#endif



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
    //for Maserati constraints 0x00/0x01
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

#if (NowHW == Maserati) || (NowHW == Mainz)
    //constraints for Maserati M+D IP =========================================================
    u16_check_status_tmp = MS_Cfd_Maserati_CheckHDRIPProcessIsOk(pstApiControlParamInit,&stu_ControlParam);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
            u16_check_status = u16_check_status_tmp;
            u8ERR_Happens_Flag ++;
    }

    if (u16_check_status_tmp != E_CFD_MC_ERR_NOERR)
    {
        pstApiControlParamInit->u8Input_HDRIPMode = 0;
        stu_ControlParam.u8Input_HDRIPMode = 0;

        stu_ControlParam.u8DoHDRIP_Forcebypass_Flag = 1;
    }
    //=========================================================================================

#elif (NowHW == Kano)
    //constraints for Kano HDR IP =========================================================
    u16_check_status_tmp = MS_Cfd_Kano_CheckHDRIPProcessIsOk(pstApiControlParamInit, &stu_ControlParam, CFD_HDR_IP_CAPABILITY);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
            u16_check_status = u16_check_status_tmp;
            u8ERR_Happens_Flag ++;
    }

    if (u16_check_status_tmp != E_CFD_MC_ERR_NOERR)
    {
        pstApiControlParamInit->u8Input_HDRIPMode = 0;
        stu_ControlParam.u8Input_HDRIPMode = 0;

        stu_ControlParam.u8DoHDRIP_Forcebypass_Flag = 1;
    }
    //=========================================================================================

#elif ((NowHW == Curry) || (NowHW == Kastor))

    //constraints for Curry HDR IP =========================================================
    u16_check_status_tmp = MS_Cfd_Curry_CheckHDRIPProcessIsOk(pstApiControlParamInit, &stu_ControlParam, CFD_HDR_IP_CAPABILITY);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
            u16_check_status = u16_check_status_tmp;
            u8ERR_Happens_Flag ++;
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
    else if ((E_CFD_OUTPUT_SOURCE_HDMI == pstApiControlParamInit->u8Output_Source) && (HW_PIPE_CONSTRAINTS == 1) && (2 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_Mode))
    {
        //stu_ControlParam.u8Temp_Format[1] = stu_ControlParam.u8Temp_Format[0];
        //stu_ControlParam.u8Temp_DataFormat[1] = stu_ControlParam.u8Temp_DataFormat[0];
        //stu_ControlParam.u8Temp_IsFullRange[1] = stu_ControlParam.u8Temp_IsFullRange[0];
        //stu_ControlParam.u8Temp_HDRMode[1] = stu_ControlParam.u8Temp_HDRMode[0];
        stu_ControlParam.u8Temp_Format[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format;
           stu_ControlParam.u8Temp_DataFormat[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_DataFormat;
           stu_ControlParam.u8Temp_IsFullRange[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_IsFullRange;
           stu_ControlParam.u8Temp_HDRMode[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode;
    }
    else
    {
        if ((E_CFD_OUTPUT_SOURCE_HDMI != pstApiControlParamInit->u8Output_Source) ||
            ((E_CFD_OUTPUT_SOURCE_HDMI == pstApiControlParamInit->u8Output_Source) && (1 != pstApiControlParamInit->u8HDMIOutput_GammutMapping_Mode) && (1 == HW_PIPE_CONSTRAINTS)))
        {
        stu_ControlParam.u8Temp_Format[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format;
        stu_ControlParam.u8Temp_DataFormat[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_DataFormat;
        stu_ControlParam.u8Temp_IsFullRange[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_IsFullRange;
        stu_ControlParam.u8Temp_HDRMode[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode;
#if (NowHW == Maserati) || (NowHW == Mainz)
            if (stu_ControlParam.u8Temp_Format[0] == E_CFD_CFIO_XVYCC_601)
            {
                stu_ControlParam.u8Temp_Format[1] = E_CFD_CFIO_YUV_BT601_625;
                stu_ControlParam.u8Temp_DataFormat[1] = E_CFD_MC_FORMAT_YUV444;
                stu_ControlParam.u8Temp_IsFullRange[1] = E_CFD_CFIO_RANGE_LIMIT;
            }
            else if (stu_ControlParam.u8Temp_Format[0] == E_CFD_CFIO_XVYCC_709)
            {
                stu_ControlParam.u8Temp_Format[1] = E_CFD_CFIO_YUV_BT709;
                stu_ControlParam.u8Temp_DataFormat[1] = E_CFD_MC_FORMAT_YUV444;
                stu_ControlParam.u8Temp_IsFullRange[1] = E_CFD_CFIO_RANGE_LIMIT;
            }
            else if (stu_ControlParam.u8Temp_Format[0] == E_CFD_CFIO_SYCC601)
            {
                stu_ControlParam.u8Temp_Format[1] = E_CFD_CFIO_YUV_BT601_625;
                stu_ControlParam.u8Temp_DataFormat[1] = E_CFD_MC_FORMAT_YUV444;
                stu_ControlParam.u8Temp_IsFullRange[1] = E_CFD_CFIO_RANGE_LIMIT;
            }
            else if ((stu_ControlParam.u8Temp_Format[0] == E_CFD_CFIO_SRGB) && (stu_ControlParam.u8Temp_Format[1] == E_CFD_CFIO_SYCC601))
            {
                stu_ControlParam.u8Temp_Format[1] = E_CFD_CFIO_YUV_BT709;
                stu_ControlParam.u8Temp_DataFormat[1] = E_CFD_MC_FORMAT_YUV444;
                stu_ControlParam.u8Temp_IsFullRange[1] = E_CFD_CFIO_RANGE_LIMIT;
            }
#endif
        }
        else if ((E_CFD_OUTPUT_SOURCE_HDMI == pstApiControlParamInit->u8Output_Source) && (1 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_Mode) && (1 == HW_PIPE_CONSTRAINTS))
        {
            //these variables should be assign by preconstraints function
            stu_ControlParam.u8Temp_DataFormat[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_DataFormat;
            stu_ControlParam.u8Temp_IsFullRange[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_IsFullRange;
            stu_ControlParam.u8Temp_HDRMode[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode;

#if ((NowHW == Curry) || (NowHW == Kastor) || (NowHW == Kano))

            stu_ControlParam.u8Temp_Format[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format;
#else

            u16_check_status_tmp = Mapi_Cfd_inter_HDMI_EDID_Param_Check(pstHDMI_EDIDParamInit);

            if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
            {
                u16_check_status = u16_check_status_tmp;
                u8ERR_Happens_Flag ++;
            }
            //set u8Output_GamutOrderIdx by EDID or user controls
            //0: for outputformat
            //1: for temp format[1]
            //get stu_ControlParam.u8Temp_GamutOrderIdx[1]
            MS_Cfd_SetOutputGamut_HDMISink(&(stu_ControlParam.u8Temp_GamutOrderIdx[1]), pstHDMI_EDIDParamInit, &stu_ControlParam, pstApiControlParamInit,1);

            //set by u8Output_GamutOrderIdx
            //0: for outputformat
            //1: for temp format[1]
            //get stu_ControlParam.u8Temp_Format[1]
            MS_Cfd_SetOutputColorParam_HDMISink(&stu_ControlParam,1);
#endif

        }
        else if ((E_CFD_OUTPUT_SOURCE_HDMI == pstApiControlParamInit->u8Output_Source) && (1 != HW_PIPE_CONSTRAINTS))
        {
            stu_ControlParam.u8Temp_Format[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_Format;
            stu_ControlParam.u8Temp_DataFormat[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_DataFormat;
            stu_ControlParam.u8Temp_IsFullRange[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_IsFullRange;
            stu_ControlParam.u8Temp_HDRMode[1] = pstApiControlParamInit->stu_Middle_Format[0].u8Mid_HDRMode;
        }

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

                if (pstu_ControlParam->u8Temp_HDRMode[1] == pstu_ControlParam->u8Temp_HDRMode[0])
                {
                    pstu_ControlParam->u8TempTransferCharacterstics[1] = pstu_ControlParam->u8TempTransferCharacterstics[0];
                }
                //else if (( E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[1] ) && ( E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Temp_HDRMode[0]))
                else if (( E_CFIO_MODE_SDR == pstu_ControlParam->u8Temp_HDRMode[1] ) && ( MS_Cfd_CheckOpenHDR(pstu_ControlParam->u8Temp_HDRMode[0])))
                {
                    pstu_ControlParam->u8TempTransferCharacterstics[1] = E_CFD_CFIO_TR_GAMMA2P2;
                }
                else
                {
                    pstu_ControlParam->u8TempTransferCharacterstics[1] = pstu_ControlParam->u8TempTransferCharacterstics[0];
                }

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
#if (NowHW == Maserati) || (NowHW == Mainz)

    //if (0x01 == (stu_HDRIP_Input.stu_Maserati_HDRIP_Param.u8HDRIP_Patch&0x01))
    //{
    //  stu_ControlParam.u8TempTransferCharacterstics[1] = E_CFD_CFIO_TR_GAMMA2P2;
    //}

#endif

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


//#if (HW_PIPE_CONSTRAINTS == 1)
    if (E_CFD_OUTPUT_SOURCE_HDMI == pstApiControlParamInit->u8Output_Source) //STB
    {
        //check input
        u16_check_status_tmp = Mapi_Cfd_inter_HDMI_EDID_Param_Check(pstHDMI_EDIDParamInit);

        if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
        {
            u16_check_status = u16_check_status_tmp;
            u8ERR_Happens_Flag ++;
        }

        //QS & QY in EDID in Video Capability Data Block(VCDB)
        MS_U8 u8Output_range_flag = 0;

        if (E_CFD_VALID == pstHDMI_EDIDParamInit->u8HDMISink_EDID_Valid )
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

            stu_ControlParam.u8Output_DataFormat = pstApiControlParamInit->u8Output_DataFormat;
            stu_ControlParam.u8Output_IsFullRange = pstApiControlParamInit->u8Output_IsFullRange;
            stu_ControlParam.u8Output_HDRMode = pstApiControlParamInit->u8Output_HDRMode;

#if (HW_PIPE_CONSTRAINTS != 1)

            //assign u8OutputIsFullRange -----------------------------------------------------------------
            if (1 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_Mode)
            {

                //set u8Output_GamutOrderIdx by EDID or user controls
                MS_Cfd_SetOutputGamut_HDMISink(&(stu_ControlParam.u8Output_GamutOrderIdx),pstHDMI_EDIDParamInit, &stu_ControlParam, pstApiControlParamInit, 0);

                //set by u8Output_GamutOrderIdx
                MS_Cfd_SetOutputColorParam_HDMISink(&stu_ControlParam,0);

            }
            //else // pstApiControlParamInit->u8HDMIOutput_GammutMapping_Mode = 0
            else if ((0 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_Mode) || (2 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_Mode))
            {
                //set by user-defined u8Output_Format & u8Output_HDRMode
                if (2 == pstApiControlParamInit->u8HDMIOutput_GammutMapping_Mode)
                {
                    if (E_CFD_MC_FORMAT_RGB == stu_ControlParam.u8Output_DataFormat)
                    {
                        stu_ControlParam.u8Output_Format = E_CFD_CFIO_RGB_BT709;
                    }
                    else
                    {
                        stu_ControlParam.u8Output_Format = E_CFD_CFIO_YUV_BT709;
                    }
                }

#if 0
                for (u8idx =0;u8idx<=9 ;u8idx++)
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
#else
                u8idx = MS_Cfd_FindMatchedIdx(stu_ControlParam.u8Output_Format);
#endif
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

#elif  (HW_PIPE_CONSTRAINTS == 1)

#if ((NowHW == Curry) || (NowHW == Kastor) || (NowHW == Kano))
            stu_ControlParam.u8DoOutput_Force709_Flag = MS_Cfd_OutputHDMI_SetColorMetry_lite_out2(&stu_ControlParam,pstApiControlParamInit);
#else
            stu_ControlParam.u8DoOutput_Force709_Flag = MS_Cfd_OutputHDMI_SetColorMetry_lite_out(&stu_ControlParam,pstApiControlParamInit);
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

#endif

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


            //printf("\033[1;35m###[Brian][%s][%d]### u8TMO_TargetRefer_Mode = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,pstApiControlParamInit->u8TMO_TargetRefer_Mode);
            //printf("\033[1;35m###[Brian][%s][%d]### u8HDMISink_HDRData_Block_Valid = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,pstHDMI_EDIDParamInit->u8HDMISink_HDRData_Block_Valid);
            //printf("\033[1;35m###[Brian][%s][%d]### u8HDMISink_HDRData_Block_Length = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,pstHDMI_EDIDParamInit->u8HDMISink_HDRData_Block_Length);

            u8tempFlag = 0;

            if ((1 == pstApiControlParamInit->u8TMO_TargetRefer_Mode) &&
                (E_CFD_VALID == pstHDMI_EDIDParamInit->u8HDMISink_HDRData_Block_Valid) &&
                (7 == pstHDMI_EDIDParamInit->u8HDMISink_HDRData_Block_Length))
            {
                u8tempFlag = 1;
                pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_TargetStatus = 0;
            }
            else if ((2 == pstApiControlParamInit->u8TMO_TargetRefer_Mode) &&
                     (E_CFD_VALID == pstHDMI_EDIDParamInit->u8HDMISink_HDRData_Block_Valid) &&
                     (7 == pstHDMI_EDIDParamInit->u8HDMISink_HDRData_Block_Length))
            {
                if ((0 == pstApiControlParamInit->u8TMO_TargetUserModeEn) || ((1 == pstApiControlParamInit->u8TMO_TargetUserModeEn) && (0 == pstApiControlParamInit->u8TMO_TargetUserMode)))
                {
                    u8tempFlag = 1;
                    pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_TargetStatus = 0;
                }
                else
                {
                    u8tempFlag = 0;
                    pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_TargetStatus = 2;
                }
            }
            else
            {
                u8tempFlag = 0;
                pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_TargetStatus = 2;
            }

            //printf("\033[1;35m###[Brian][%s][%d]### u8tempFlag = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,u8tempFlag);
            //printf("\033[1;35m###[Brian][%s][%d]### u8TMO_TargetStatus = %d!!!!!!!!!!!!!!!!!\033[0m\n",__FUNCTION__,__LINE__,pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_TargetStatus);

            {
                //if ((1 <= pstApiControlParamInit->u8TMO_TargetRefer_Mode) && (E_CFD_VALID == pstHDMI_EDIDParamInit->u8HDMISink_HDRData_Block_Valid))
                if (1 == u8tempFlag)
                {
                    //due to CEA-861.3

                    //MS_U16 MS_Cfd_code2nits_max_EDID(MS_U8 u8Code, MS_U16 *nits_table)
                    U16Temp0 = MS_Cfd_code2nits_max_EDID(pstHDMI_EDIDParamInit->u8HDMISink_Desired_Content_Max_Luminance,LUT_CFD_CV1);
                                pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMax = U16Temp0;
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMaxFlag = 0;

                    //Transfer pqcode inside TMO
                    //stu_TMO_Input.stu_CFD_TMO_Param.u16TgtMax = MS_Cfd_Luminance_To_PQCode(U16Temp0,0);

                    //U16Temp0 = LUT_CFD_CV1[pstHDMI_EDIDParamInit->u8HDMISink_Desired_Content_Max_Frame_Avg_Luminance];
                    U16Temp1 = MS_Cfd_code2nits_max_EDID(pstHDMI_EDIDParamInit->u8HDMISink_Desired_Content_Max_Frame_Avg_Luminance,LUT_CFD_CV1);
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMed = U16Temp1;
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMedFlag = 0;

                    //Transfer pqcode inside TMO
                    //stu_TMO_Input.stu_CFD_TMO_Param.u16Tmed = MS_Cfd_Luminance_To_PQCode(U16Temp0,0);
                    U16Temp1 = MS_Cfd_code2nits_min_EDID(pstHDMI_EDIDParamInit->u8HDMISink_Desired_Content_Min_Luminance, LUT_CFD_CV2, U16Temp0, pu8_unit_flag);
                    //U16Temp1 = (((MS_U32)U16Temp0)*LUT_CFD_CV2[pstHDMI_EDIDParamInit->u8HDMISink_Desired_Content_Min_Luminance])>>12;
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMin = U16Temp1;
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMinFlag = *pu8_unit_flag;

                    //detect if something wrong
                    u16_check_status_tmp = MS_Cfd_HDREDID_ErrDetect(pstu_ControlParam);

                    //use CFD default value if error happens
                    MS_Cfd_HDREDID_ErrHandle(pstu_ControlParam, pstApiControlParamInit, u16_check_status_tmp);

                    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
                    {
                        u16_check_status = u16_check_status_tmp;
                        u8ERR_Happens_Flag ++;
                    }
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

        }//end of if (1 == pstHDMI_EDIDParamInit->u8HDMISink_EDID_Valid )
        else
        {
#if ((NowHW == Curry) || (NowHW == Kastor) || (NowHW == Kano))
            stu_ControlParam.u8DoOutput_Force709_Flag = MS_Cfd_OutputHDMI_SetColorMetry_lite_out2(&stu_ControlParam,pstApiControlParamInit);
#else
            stu_ControlParam.u8DoOutput_Force709_Flag = MS_Cfd_OutputHDMI_SetColorMetry_lite_out(&stu_ControlParam,pstApiControlParamInit);
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

    }
//#endif

    if (E_CFD_OUTPUT_SOURCE_PANEL == pstApiControlParamInit->u8Output_Source) //TV
    {

        //check input
        u16_check_status_tmp = Mapi_Cfd_inter_PANEL_Param_Check(pstu_Panel_Param);

        if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
        {
            u16_check_status = u16_check_status_tmp;
            u8ERR_Happens_Flag ++;
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
                pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_TargetStatus = 0;

            }
            else if (2 == pstApiControlParamInit->u8TMO_TargetRefer_Mode)
            {
                if ((0 == pstApiControlParamInit->u8TMO_TargetUserModeEn) || ((1 == pstApiControlParamInit->u8TMO_TargetUserModeEn) && (0 == pstApiControlParamInit->u8TMO_TargetUserMode)))
                {
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMax = pstu_Panel_Param->u16Panel_Max_Luminance;
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMed = pstu_Panel_Param->u16Panel_Med_Luminance;
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMin = pstu_Panel_Param->u16Panel_Min_Luminance;
                    pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_TargetStatus = 0;
                }
                else
                {
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMax = pstApiControlParamInit->u16Target_Max_Luminance;
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMed = pstApiControlParamInit->u16Target_Med_Luminance;
                    pstu_ControlParam->stu_CFD_TMO_Param.u16TgtMin = pstApiControlParamInit->u16Target_Min_Luminance;
                    pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_TargetStatus = 2;
                }
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
                pstu_ControlParam->stu_CFD_TMO_Param.u8TMO_TargetStatus = 2;
            }
        }
        //end of output_source = Panel
    }

    if ((E_CFD_OUTPUT_SOURCE_HDMI != pstApiControlParamInit->u8Output_Source) && (E_CFD_OUTPUT_SOURCE_PANEL != pstApiControlParamInit->u8Output_Source))
    {

#if ((NowHW == Curry) || (NowHW == Kastor) || (NowHW == Kano))
          stu_ControlParam.u8DoOutput_Force709_Flag = MS_Cfd_OutputHDMI_SetColorMetry_lite_out2(&stu_ControlParam,pstApiControlParamInit);
#else
          stu_ControlParam.u8DoOutput_Force709_Flag = MS_Cfd_OutputHDMI_SetColorMetry_lite_out(&stu_ControlParam,pstApiControlParamInit);
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
    //end of output source determination

#if (NowHW == Maserati) || (NowHW == Mainz)
#if defined HE_PIPE_POST_CONSTRAINTS
    if (( E_CFD_CFIO_DOLBY_HDR_TEMP != pstApiControlParamInit->u8Input_Format) || ( E_CFIO_MODE_HDR1 != pstApiControlParamInit->u8Input_HDRMode))
    {
        //Mapi_Cfd_inter_Main_Control_PostConstraints(pstApiControlParamInit);
    }
#endif
#elif ((NowHW == Curry) || (NowHW == Kastor) || (NowHW == Kano))
#if defined HE_PIPE_POST_CONSTRAINTS
    if (( E_CFD_CFIO_DOLBY_HDR_TEMP != pstApiControlParamInit->u8Input_Format) || ( E_CFIO_MODE_HDR1 != pstApiControlParamInit->u8Input_HDRMode))
    {
        Mapi_Cfd_inter_Main_Control_PostConstraints_STB(pstu_ControlParam);
    }
#endif
#endif


    //constraints for Maserati SDR IP =========================================================
#if (NowHW == Maserati) || (NowHW == Mainz)
    u16_check_status_tmp = MS_Cfd_Maserati_CheckSDRIPProcessIsOk(pstApiControlParamInit,&stu_ControlParam);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
    }

#elif ((NowHW == Curry) || (NowHW == Kastor) || (NowHW == Kano))

    u16_check_status_tmp = MS_Cfd_STB_CheckSDRIPProcessIsOk(pstApiControlParamInit,&stu_ControlParam, CFD_SDR_IP_CAPABILITY);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
    }

#endif

    //Set u8DoGamutMapping_Flag
    //Set u8DoTMO_Flag
    //u16_check_status = MS_Cfd_Maserati_SetGM_TMO_Flags(&stu_ControlParam);
    u16_check_status_tmp = MS_Cfd_SetGM_TMO_Flags(&stu_ControlParam);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
    }

    //assign values to TMO,HDR,SDR functions
    //consider hw design
    //need check function
#if 1
#if (NowHW == Kano)

    //write HW mode constraints here
    //stu_TMO_Input.stu_Kano_TMOIP_Param.u8HDR_TMO_curve_Mode = function1(stu_TMO_Input.stu_Kano_TMOIP_Param.u8HDR_TMO_curve_Mode,MS_Cfd_OnlyEnable_Decision_OFF());

    u16_check_status_tmp = MS_Cfd_Kano_TMO_Control(&stu_ControlParam,&stu_TMO_Input);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
    }

    u16_check_status_tmp = MS_Cfd_Kano_HDRIP_Control(&stu_ControlParam,&stu_HDRIP_Input,&stu_TMO_Input);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
    }

    u16_check_status_tmp = MS_Cfd_Kano_SDRIP_Control(&stu_ControlParam,&stu_SDRIP_Input,&stu_TMO_Input,u8MainSubMode);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
    }

#endif

#if ((NowHW == Curry) || (NowHW == Kastor))

    u16_check_status_tmp = MS_Cfd_Curry_TMO_Control(&stu_ControlParam,&stu_TMO_Input);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
    }

    u16_check_status_tmp = MS_Cfd_Curry_HDRIP_Control(&stu_ControlParam,&stu_HDRIP_Input,&stu_TMO_Input);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
    }

    u16_check_status_tmp = MS_Cfd_Curry_SDRIP_Control(&stu_ControlParam,&stu_SDRIP_Input,&stu_TMO_Input,u8MainSubMode);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
    }

#endif
    //return to top
    //memcpy(pstu_Cfd_api_top->pstu_Kano_HDRIP_Param,&stu_HDRIP_Input.stu_Kano_HDRIP_Param,sizeof(STU_CFDAPI_Kano_HDRIP));
    //memcpy(pstu_Cfd_api_top->pstu_Kano_SDRIP_Param,&stu_SDRIP_Input.stu_Kano_SDRIP_Param,sizeof(STU_CFDAPI_Kano_SDRIP));
    //memcpy(pstu_Cfd_api_top->pstu_Kano_TMO_Param,&stu_TMO_Input.stu_Kano_TMOIP_Param,sizeof(STU_CFDAPI_Kano_TMOIP));

#if (NowHW == Manhattan)
    //case 2 : Manhattan
    u16_check_status_tmp = MS_Cfd_Manhattan_TMO_Control(&stu_ControlParam,&stu_TMO_Input);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
    }

    u16_check_status_tmp = MS_Cfd_Manhattan_HDRIP_Control(&stu_ControlParam,&stu_HDRIP_Input);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
    }

    u16_check_status_tmp = MS_Cfd_Manhattan_SDRIP_Control(&stu_ControlParam,&stu_SDRIP_Input,&stu_TMO_Input,u8MainSubMode);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
    }

#endif
    //memcpy(pstu_Cfd_api_top->pstu_Manhattan_HDRIP_Param,&stu_HDRIP_Input.stu_Manhattan_HDRIP_Param,sizeof(STU_CFDAPI_Manhattan_HDRIP));
    //memcpy(pstu_Cfd_api_top->pstu_Manhattan_SDRIP_Param,&stu_SDRIP_Input.stu_Manhattan_SDRIP_Param,sizeof(STU_CFDAPI_Manhattan_SDRIP));
    //memcpy(pstu_Cfd_api_top->pstu_Manhattan_TMO_Param,&stu_TMO_Input.stu_Manhattan_TMOIP_Param,sizeof(STU_CFDAPI_Manhattan_TMOIP));

#endif

#if (NowHW == Maserati) || (NowHW == Mainz)

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
        u8ERR_Happens_Flag ++;
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
        u8ERR_Happens_Flag ++;
    }

    if (E_CFIO_MODE_HDR1 == stu_ControlParam.u8Input_HDRMode) // todo -> find the corresponding proper flag
    {
        u16_check_status_tmp = MS_Cfd_Maserati_Dolby_Control(pstu_Cfd_api_top->pstu_Dolby_Param);
    }
    else
    {
        if(0 == u8MainSubMode)
        {
            u16_check_status_tmp = MS_Cfd_Maserati_HDRIP_Control(&stu_ControlParam,&stu_HDRIP_Input,&stu_TMO_Input);
        }
    }

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
    }

    u16_check_status_tmp = MS_Cfd_Maserati_SDRIP_Control(&stu_ControlParam,&stu_SDRIP_Input,&stu_TMO_Input,u8MainSubMode);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
    }

#endif

#if (NowHW == U13_Universe)

    u16_check_status_tmp = MS_Cfd_Universe_SDRIP_Control(&stu_ControlParam,&stu_SDRIP_Input,&stu_TMO_Input);

    if ((E_CFD_MC_ERR_NOERR != u16_check_status_tmp) && (0 == u8ERR_Happens_Flag))
    {
        u16_check_status = u16_check_status_tmp;
        u8ERR_Happens_Flag ++;
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

#if (NowHW == Maserati) || (NowHW == Mainz)

    //MS_Cfd_Maserati_HDRIP_Debug(pstu_Cfd_api_top);
    MS_Cfd_Maserati_HDRIP_Debug(&(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_TMO_Input->stu_Maserati_TMO_Param), &(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Maserati_HDRIP_Param));

    //MS_Cfd_Maserati_SDRIP_Debug(pstu_Cfd_api_top);
    MS_Cfd_Maserati_SDRIP_Debug(&(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_DLC_Input->stu_Maserati_DLC_Param), &(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Maserati_SDRIP_Param));

#elif (NowHW == Kano)

    //MS_Cfd_Maserati_HDRIP_Debug(pstu_Cfd_api_top);
    MS_Cfd_Kano_HDRIP_Debug(&(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_TMO_Input->stu_Kano_TMOIP_Param), &(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Kano_HDRIP_Param));

    //MS_Cfd_Maserati_SDRIP_Debug(pstu_Cfd_api_top);
    MS_Cfd_Kano_SDRIP_Debug(&(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_DLC_Input->stu_Kano_DLC_Param), &(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Kano_SDRIP_Param));

#elif ((NowHW == Curry) || (NowHW == Kastor))

    //MS_Cfd_Maserati_HDRIP_Debug(pstu_Cfd_api_top);
    MS_Cfd_Curry_HDRIP_Debug(&(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_TMO_Input->stu_Curry_TMOIP_Param), &(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_HDRIP_Input->stu_Curry_HDRIP_Param));

    //MS_Cfd_Maserati_SDRIP_Debug(pstu_Cfd_api_top);
    MS_Cfd_Curry_SDRIP_Debug(&(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_DLC_Input->stu_Curry_DLC_Param), &(pstu_Cfd_api_top->pstu_HW_IP_Param->pstu_SDRIP_Input->stu_Curry_SDRIP_Param));

    #endif

    printf("\n SDR IPs:u8DoDLC_Flag = %x\n",stu_ControlParam.u8DoDLC_Flag);
    printf("\n SDR IPs:u8DoGamutMapping_Flag = %x\n",stu_ControlParam.u8DoGamutMapping_Flag);
    printf("\n SDR IPs:u8DoTMO_Flag = %x\n",stu_ControlParam.u8DoTMO_Flag);
    printf("\n SDR IPs:u8DoBT2020CLP_Flag = %x\n",stu_ControlParam.u8DoBT2020CLP_Flag);
    printf("\n" );

    printf("\n HDR IPs:u8DoDLC_Flag = %x\n",stu_ControlParam.u8DoDLCInHDRIP_Flag);
    printf("\n HDR IPs:u8DoGamutMapping_Flag = %x\n",stu_ControlParam.u8DoGamutMappingInHDRIP_Flag);
    printf("\n HDR IPs:u8DoTMO_Flag = %x\n",stu_ControlParam.u8DoTMOInHDRIP_Flag);
    printf("\n HDR IPs:u8DoBT2020CLP_Flag = %x\n",stu_ControlParam.u8DoBT2020CLPInHDRIP_Flag);
    printf("\n HDR IPs:u8DoHDRbypassInHDRIP_Flag = %x\n",stu_ControlParam.u8DoHDRbypassInHDRIP_Flag);
    printf("\n HDR IPs:u8DoHDRXtoHDRY_Flag = %x\n",stu_ControlParam.u8DoHDRXtoHDRY_Flag);
    printf("\n HDR IPs:u8DoSDRtoHDR_Flag = %x\n",stu_ControlParam.u8DoSDRtoHDR_Flag);
    printf("\n" );

    printf("\n CFD:u8DoMMIn_ForceHDMI_Flag = %x\n",stu_ControlParam.u8DoMMIn_ForceHDMI_Flag);
    printf("\n CFD:u8DoMMIn_Force709_Flag = %x\n",stu_ControlParam.u8DoMMIn_Force709_Flag);
    printf("\n CFD:u8DoHDMIIn_Force709_Flag = %x\n",stu_ControlParam.u8DoHDMIIn_Force709_Flag);
    printf("\n CFD:u8DoOtherIn_Force709_Flag = %x\n",stu_ControlParam.u8DoOtherIn_Force709_Flag);
    printf("\n CFD:u8DoOutput_Force709_Flag = %x\n",stu_ControlParam.u8DoOutput_Force709_Flag);
    printf("\n" );

    printf("\n CFD:u8DoHDRIP_Forcebypass_Flag = %x\n",stu_ControlParam.u8DoHDRIP_Forcebypass_Flag);
    printf("\n CFD:u8DoSDRIP_ForceNOTMO_Flag = %x\n",stu_ControlParam.u8DoSDRIP_ForceNOTMO_Flag);
    printf("\n CFD:u8DoSDRIP_ForceNOGM_Flag = %x\n",stu_ControlParam.u8DoSDRIP_ForceNOGM_Flag);
    printf("\n CFD:u8DoSDRIP_ForceNOBT2020CL_Flag = %x\n",stu_ControlParam.u8DoSDRIP_ForceNOBT2020CL_Flag);
    printf("\n" );

    printf("\n CFD:u8DoPreConstraints_Flag = %x\n",stu_ControlParam.u8DoPreConstraints_Flag);
    printf("\n CFD:u8DoPathFullRange_Flag = %x\n",stu_ControlParam.u8DoPathFullRange_Flag);
    printf("\n CFD:u8DoForceEnterHDRIP_Flag = %x\n",stu_ControlParam.u8DoForceEnterHDRIP_Flag);
    printf("\n CFD:u8MustGMFlag = %x\n",stu_ControlParam.u8MustGMFlag);
    printf("\n CFD:u8XVYCC2SRGBFlag = %x\n",stu_ControlParam.u8XVYCC2SRGBFlag);
    printf("\n" );

    printf("\n CFD:u8TMO_SourceStatus = %x\n",stu_ControlParam.stu_CFD_TMO_Param.u8TMO_SourceStatus);
    printf("\n CFD:u8TMO_SourceStatusM = %x\n",stu_ControlParam.stu_CFD_TMO_Param.u8TMO_SourceStatusM);
    printf("\n CFD:u8TMO_TargetStatus = %x\n",stu_ControlParam.stu_CFD_TMO_Param.u8TMO_TargetStatus);
    printf("\n CFD:u8TMO_TargetStatusM = %x\n",stu_ControlParam.stu_CFD_TMO_Param.u8TMO_TargetStatusM);

//#if (0 == RealChip)

    printf("TMO: u16Smin = %d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smin);
    printf("TMO: u16Smed = %d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smed);
    printf("TMO: u16Smax = %d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Smax);
    printf("TMO: u16Tmin = %d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmin);
    printf("TMO: u16Tmed = %d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmed);
    printf("TMO: u16Tmax = %d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u16Tmax);
    printf("TMO: u8TgtMinFlag = %d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TgtMinFlag);
    printf("TMO: u8TMO_TargetMode = %d\n",g_HDRinitParameters.DLC_HDRNewToneMappingData.u8TMO_TargetMode);

//#endif

    printf("\n" );

    printf("\nu8Process_Status = %02x\n",pstApiControlParamInit->u8Process_Status);
    printf("u8Process_Status2 = %02x\n",pstApiControlParamInit->u8Process_Status2);
    printf("u8Process_Status3 = %02x\n\n",pstApiControlParamInit->u8Process_Status3);
    printf("error message = %04x\n\n",u16_check_status);
    printf("error message count = %04x\n\n",u8ERR_Happens_Flag);

    //input control information
#if (NowHW == Maserati) || (NowHW == Mainz)
    //Restore_input_bank_and_log_Maserati(&stu_ControlParam,pstu_Cfd_api_top); //move the function body to TOP
#endif

    //writeback format information
    MS_Cfd_WriteBack_Control(pstu_Cfd_api_top,&stu_ControlParam);

    //handle output HDMI infoFrame
    //STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT *pstu_api_HDMI_InfoFrame_Param;

       //only for box now
#if ((NowHW == Curry) || (NowHW == Kastor) || (NowHW == Kano))
    if (E_CFD_OUTPUT_SOURCE_HDMI == pstApiControlParamInit->u8Output_Source)
    {
        MS_Cfd_HandleOutInfoFrame(pstu_api_HDMI_InfoFrame_Param_out, pstu_ControlParam, pstHDMI_EDIDParamInit, pstApiControlParamInit, pstu_MM_Param, pstu_api_HDMI_InfoFrame_Param,pstu_OSD_Param,pstu_OSD_Process_Configs);
    }
#endif

#if ((CFD_SW_VERSION >= 0x00000014) && (CFD_SW_RETURN_ST_VERSION >= 1))

    pstu_Cfd_api_top->pstu_SW_Return->u8VideoADFlag = pstu_ControlParam->u8VideoADFlag;

#endif

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

    pstu_ControlParam->u8InputColorPriamries = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
    pstu_ControlParam->u8InputMatrixCoefficients = E_CFD_CFIO_MC_BT709_XVYCC709;

    if( MS_Cfd_CheckOpenHDR(pstu_ControlParam->u8Input_HDRMode))
    {
        pstu_ControlParam->u8InputTransferCharacterstics = u8Current_Transfer_Characteristic;
    }
    else //SDR
    {
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
*/

//-----------------------------------------------------------------------------------------
//assign for HDMI
//from the definition of STU_CFD_MS_ALG_COLOR_FORMAT

//@param : u8mode
//0: for output format : output of SDR IP
//1: for temp format [1] : input of SDR IP
//------------------------------------------------------------------------------------------
void MS_Cfd_SetOutputColorParam_HDMISink(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_input, MS_U8 u8mode)
{

    MS_U8 temp_GamutOrderIdx;
    MS_U8 temp_DataFormat;
    MS_U8 temp_Format;
    MS_U8 temp_ColorPriamries;
    MS_U8 temp_TransferCharacterstics;
    MS_U8 temp_MatrixCoefficients;

    if (0 == u8mode) //for output format
    {
        temp_GamutOrderIdx = pstu_input->u8Output_GamutOrderIdx;
        temp_DataFormat = pstu_input->u8Output_DataFormat;
    }
    else if (1 <= u8mode) //
{
        temp_GamutOrderIdx = pstu_input->u8Temp_GamutOrderIdx[1];
        temp_DataFormat = pstu_input->u8Temp_DataFormat[1];
    }

    //only YUV
    if ( E_CFD_CFIO_GT_BT601_525 == temp_GamutOrderIdx)
    {
        temp_Format = E_CFD_CFIO_YUV_BT601_525;
        temp_ColorPriamries = E_CFD_CFIO_CP_BT601525_SMPTE170M;
        temp_TransferCharacterstics = E_CFD_CFIO_TR_BT601525_601625;
        temp_MatrixCoefficients = E_CFD_CFIO_MC_BT601525_SMPTE170M;
    }
    else if ( E_CFD_CFIO_GT_BT709 == temp_GamutOrderIdx)
    {

        if (E_CFD_MC_FORMAT_RGB == temp_DataFormat)
        {
            temp_Format = E_CFD_CFIO_RGB_BT709;
        }
        else
        {
            temp_Format = E_CFD_CFIO_YUV_BT709;
        }

        temp_ColorPriamries = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
        temp_TransferCharacterstics = E_CFD_CFIO_TR_BT709;
        temp_MatrixCoefficients = E_CFD_CFIO_MC_BT709_XVYCC709;
    }
    else if ( E_CFD_CFIO_GT_ADOBERGB == temp_GamutOrderIdx) //Only RGB
    {
        if (E_CFD_MC_FORMAT_RGB == temp_DataFormat)
        {
            temp_Format = E_CFD_CFIO_ADOBE_RGB;
        }
        else
        {
            temp_Format = E_CFD_CFIO_ADOBE_YCC601;
        }

        temp_ColorPriamries = E_CFD_CFIO_CP_ADOBERGB;
        temp_TransferCharacterstics = E_CFD_CFIO_TR_ADOBERGB;
        temp_MatrixCoefficients = E_CFD_CFIO_MC_BT601625_XVYCC601_SYCC;
    }
    else if ( E_CFD_CFIO_GT_BT2020 == temp_GamutOrderIdx)
    {

        if (E_CFD_MC_FORMAT_RGB == temp_DataFormat)
        {
            temp_Format = E_CFD_CFIO_RGB_BT2020;
        }
        else
        {
            temp_Format = E_CFD_CFIO_YUV_BT2020_NCL;
        }

        temp_ColorPriamries = E_CFD_CFIO_CP_BT2020;
        temp_TransferCharacterstics = E_CFD_CFIO_TR_BT2020NCL;
        temp_MatrixCoefficients = E_CFD_CFIO_MC_BT2020NCL;
    }

       temp_TransferCharacterstics = MS_Cfd_GetTR_FromHDRMode(pstu_input->u8Output_HDRMode, pstu_input->u8OutputTransferCharacterstics);

    if (0 == u8mode) //for output format
    {
        pstu_input->u8Output_Format = temp_Format;
        pstu_input->u8OutputColorPriamries = temp_ColorPriamries;
        pstu_input->u8OutputTransferCharacterstics = temp_TransferCharacterstics;
        pstu_input->u8OutputMatrixCoefficients = temp_MatrixCoefficients;
    }
    else if (1 >= u8mode) //
    {
        pstu_input->u8Temp_Format[1] = temp_Format;
        //pstu_input->u8TempColorPriamries[1] = temp_ColorPriamries;
        //pstu_input->u8TempTransferCharacterstics[1] = temp_TransferCharacterstics;
        //pstu_input->u8TempMatrixCoefficients[1] = temp_MatrixCoefficients;
    }

}

#if 0
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
    MS_U8 u8idxub = 0;

    u8idxub = sizeof (u8Const_LUT_MMInfor_HDMIDefinition) / (sizeof(MS_U8)*5);
    //for (u8idx = 0; u8idx<= 8; u8idx++)
    for (u8idx =0; u8idx < u8idxub ;u8idx++)
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
                 //(E_CFIO_MODE_HDR2 == pstu_ControlParam->u8Input_HDRMode))
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
                //if ((0 <= u8idx)&&(6 >= u8idx))
                if (E_CFD_CFIO_RGB_NOTSPECIFIED != u8Const_LUT_MMInfor_HDMIDefinition[u8idx][3])
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
    MS_U8 u8mode)
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
#if 0
        if((E_CFD_CFIO_CP_RESERVED0 == pstu_MMParam->u8Colour_primaries) ||
                (E_CFD_CFIO_CP_UNSPECIFIED == pstu_MMParam->u8Colour_primaries) ||
                (E_CFD_CFIO_CP_RESERVED3 == pstu_MMParam->u8Colour_primaries) ||
                (E_CFD_CFIO_CP_RESERVED_START <= pstu_MMParam->u8Colour_primaries))
#endif
            if (MS_Cfd_checkColorprimaryIsUndefined(&(pstu_MMParam->u8Colour_primaries)))
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
            pstu_ControlParam->u8InputTransferCharacterstics    = pstu_MMParam->u8Transfer_Characteristics;
        }
        else if (E_CFIO_MODE_SDR == pstu_ControlParam->u8Input_HDRMode)
        {
            pstu_ControlParam->u8InputTransferCharacterstics    = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][1];
        }

        pstu_ControlParam->u8InputColorPriamries            = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][0];
        pstu_ControlParam->u8InputMatrixCoefficients        = u8Const_LUT_MMInfor_HDMIDefinition[u8idx][2];
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

#if 0
    for (u8idx =0;u8idx<=9 ;u8idx++)
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
#else
    u8idx = MS_Cfd_FindMatchedIdx(pstu_ControlParam->u8Temp_Format[1]);
#endif

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

        HDR_DBG_HAL_CFD(printf("warning forceto709 happens in [ %s  , %d]\n", __FUNCTION__,__LINE__));
    }

    //
    //assign
    //stu_ControlParam.u8InputColorPriamries
    //stu_ControlParam.u8InputTransferCharacterstics
    //stu_ControlParam.u8InputMatrixCoefficients

#if 0
    for (u8idx =0;u8idx<=9 ;u8idx++)
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
#else
    u8idx = MS_Cfd_FindMatchedIdx(pstu_ControlParam->u8Temp_Format[U8FormatArray_Idx]);
#endif

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
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
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
      0x13 };

    const MS_U16 u16PQCurveMantLut[705] =
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
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
    //printf("%x -> %x; %x %x %x %x %x %x\n", u16In, u32Result, u16Idx, u16Lsb, u32Lut0_out, u32Lut1_out, u16Shift, u16Exp);
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

        HDR_DBG_HAL_CFD(printf("\n  CFD MainControl current u8Input_AnalogIdx is reserved \n"));
        HDR_DBG_HAL_CFD(printf("\n  force MainControl u8Input_AnalogIdx to E_CFD_INPUT_ANALOG_RF_NTSC_44\n"));
        HDR_DBG_HAL_CFD(printf("Error code = %04x !!!, error is in [ %s  , %d]\n",u16_check_status, __FUNCTION__,__LINE__));
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
            HDR_DBG_HAL_CFD(printf("\n  out of range [%s]\n",__func__));
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
