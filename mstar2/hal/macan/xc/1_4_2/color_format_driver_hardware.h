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
/// @file   color_format_driver.h
/// @brief  MStar XC Driver DDI HAL Level
/// @author MStar Semiconductor Inc.
/// @attention
/// <b>(OBSOLETED) <em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_COLOR_FORMAT_DRIVER_HARDWARE_H
#define _HAL_COLOR_FORMAT_DRIVER_HARDWARE_H

#ifdef _HAL_COLOR_FORMAT_DRIVER_HARDWARE_C
#define INTERFACE
#else
#define INTERFACE                               extern
#endif

//MS_U8 MS_Cfd_Maserati_IP_Control(void);
//Only for Maserati ==================================================================
//refer the details in
//Kano_ColorMap_DecisionTree_Table.xls
/*
typedef struct _STU_CFD_COLORIMETRY
{
    //order R->G->B
    MS_U16 u16Display_Primaries_x[3];      //data *0.00002 0xC350 = 1
    MS_U16 u16Display_Primaries_y[3];      //data *0.00002 0xC350 = 1
    MS_U16 u16White_point_x;               //data *0.00002 0xC350 = 1
    MS_U16 u16White_point_y;               //data *0.00002 0xC350 = 1

} STU_CFD_COLORIMETRY;
typedef struct _STU_CFDAPI_PANEL_FORMAT
{
    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_HDMI_PANEL_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_PANEL_FORMAT)

    MS_U16 u16Panel_Med_Luminance;          //data * 1 nits
    MS_U16 u16Panel_Max_Luminance;          //data * 1 nits
    MS_U16 u16Panel_Min_Luminance;          //data * 0.0001 nits

    //order R->G->B
    STU_CFD_COLORIMETRY stu_Cfd_Panel_ColorMetry;

} STU_CFDAPI_PANEL_FORMAT;
*/
typedef struct _STU_CFDAPI_Maserati_TMOIP
{
    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_KANO_TMOIP_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_Kano_TMOIP)

    //B02
    MS_U8  u8HDR_TMO_curve_enable_Mode;
    MS_U8  u8HDR_TMO_curve_Mode;
    MS_U8  u8HDR_TMO_curve_setting_Mode;
    //MS_U8  u8TMO_curve_Manual_Mode;

    //B04
    MS_U8  u8HDR_UVC_setting_Mode;

} STU_CFDAPI_Maserati_TMOIP;

typedef struct _STU_CFDAPI_Maserati_DLCIP
{
    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_KANO_TMOIP_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_Kano_TMOIP)

    MS_U8  u8DLC_curve_Mode;
    MS_U8  u8DLC_curve_enable_Mode;

    //MS_U8  u8UVC_enable_Mode;
    //MS_U8  u8UVC_setting_Mode;

} STU_CFDAPI_Maserati_DLCIP;

typedef struct __attribute__((packed))
{
    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_KANO_HDRIP_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_Kano_HDRIP)

    //TOP
    MS_U8 u8HDR_enable_Mode;

    //Composer
    MS_U8 u8HDR_Composer_Mode;

    //B01
    MS_U8 u8HDR_Module1_enable_Mode;

    //B01-02
    MS_U8 u8HDR_InputCSC_Mode;
    MS_U8 u8HDR_InputCSC_Ratio1;
    MS_U8 u8HDR_InputCSC_Manual_Vars_en;
    MS_U8 u8HDR_InputCSC_MC;

    //B01-03
    MS_U8 u8HDR_Degamma_SRAM_Mode;
    MS_U8 u8HDR_Degamma_Ratio1;//0x40 = 1 Q2.6
    MS_U16 u16HDR_Degamma_Ratio2;//0x40 = 1 Q2.6
    MS_U8 u8HDR_DeGamma_Manual_Vars_en;
    MS_U8 u8HDR_Degamma_TR;
    MS_U8 u8HDR_Degamma_Lut_En;
    MS_U32 *pu32HDR_Degamma_Lut_Address;
#if !defined (__aarch64__)
    void *pDummy;
#endif
    MS_U16 u16HDR_Degamma_Lut_Length;
    MS_U8  u8DHDR_Degamma_Max_Lum_En;
    MS_U16 u16HDR_Degamma_Max_Lum;

    //B01-04
    MS_U8 u8HDR_3x3_Mode;
    MS_U16 u16HDR_3x3_Ratio2;//0x40 = 1 Q2.6
    MS_U8 u8HDR_3x3_Manual_Vars_en;
    MS_U8 u8HDR_3x3_InputCP;
    MS_U8 u8HDR_3x3_OutputCP;

    //B01-05
    MS_U8 u8HDR_Gamma_SRAM_Mode;
    MS_U8 u8HDR_Gamma_Manual_Vars_en;
    MS_U8 u8HDR_Gamma_TR;
    MS_U8 u8HDR_Gamma_Lut_En;
    MS_U32 *pu32HDR_Gamma_Lut_Address;
#if !defined (__aarch64__)
    void *pDummy2;
#endif
    MS_U16 u16HDR_Gamma_Lut_Length;

    //B01-06
    MS_U8 u8HDR_OutputCSC_Mode;
    MS_U8 u8HDR_OutputCSC_Ratio1;
    MS_U8 u8HDR_OutputCSC_Manual_Vars_en;
    MS_U8 u8HDR_OutputCSC_MC;

    //B01-07
    MS_U8 u8HDR_Yoffset_Mode;
    //MS_U16 u16ChromaWeight;

    //MaxRGB for B02
    MS_U8 u8HDR_MAXRGB_CSC_Mode;
    MS_U8 u8HDR_MAXRGB_Ratio1;
    MS_U8 u8HDR_MAXRGB_Manual_Vars_en;
    MS_U8 u8HDR_MAXRGB_MC;

    //M IP
    MS_U8 u8HDR_NLM_enable_Mode;
    MS_U8 u8HDR_NLM_setting_Mode;
    MS_U8 u8HDR_ACGain_enable_Mode;
    MS_U8 u8HDR_ACGain_setting_Mode;

    //B03
    MS_U8 u8HDR_ACE_enable_Mode;
    MS_U8 u8HDR_ACE_setting_Mode;

    //B0501
    MS_U8 u8HDR_Dither1_setting_Mode;

    //B0502
    MS_U8 u8HDR_3DLUT_enable_Mode;
    MS_U8 u8HDR_3DLUT_SRAM_Mode;
    MS_U8 u8HDR_3DLUT_setting_Mode;

    //B06
    MS_U8 u8HDR_444to422_enable_Mode;
    MS_U8 u8HDR_Dither2_enable_Mode;
    MS_U8 u8HDR_Dither2_setting_Mode;

    MS_U8 u8HDRIP_Patch;
} STU_CFDAPI_Maserati_HDRIP;

typedef struct __attribute__((packed))
{
    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_MAIN_CONTROL_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_Kano_SDRIP)

    //IP2 CSC
    MS_U8 u8IP2_CSC_Mode;
    MS_U8 u8IP2_CSC_Ratio1;
    MS_U8 u8IP2_CSC_Manual_Vars_en;
    MS_U8 u8IP2_CSC_MC;

    //UFSC
    MS_U8 u8UFSC_YCOffset_Gain_Mode;

    //VIP
    MS_U8 u8VIP_CSC_Mode;
    MS_U8 u8VIP_PreYoffset_Mode;
    MS_U8 u8VIP_PreYgain_Mode;
    MS_U8 u8VIP_PreYgain_dither_Mode;
    MS_U8 u8VIP_PostYoffset_Mode;
    MS_U8 u8VIP_PostYgain_Mode;
    MS_U8 u8VIP_PostYoffset2_Mode;

    //VOP2 CSC
    MS_U8 u8VOP_3x3_Mode;
    MS_U8 u8VOP_3x3_Ratio1;//0x40 = 1 Q2.6
    MS_U8 u8VOP_3x3_Manual_Vars_en;
    MS_U8 u8VOP_3x3_MC;

    //VOP2
    MS_U8 u8VOP_3x3RGBClip_Mode;

    //VOP2
    MS_U8 u8LinearRGBBypass_Mode;

    //VOP2 degamma
    MS_U8 u8Degamma_enable_Mode;
    MS_U8 u8Degamma_Dither_Mode;
    MS_U8 u8Degamma_SRAM_Mode;
    MS_U8 u8Degamma_Ratio1;//0x40 = 1 Q2.6
    MS_U16 u16Degamma_Ratio2;//0x40 = 1 Q2.6
    MS_U8 u8DeGamma_Manual_Vars_en;
    MS_U8 u8Degamma_TR;
    MS_U8 u8Degamma_Lut_En;
    MS_U32 *pu32Degamma_Lut_Address;
#if !defined (__aarch64__)
    void *pDummy;
#endif
    MS_U16 u16Degamma_Lut_Length;
    MS_U8  u8Degamma_Max_Lum_En;
    MS_U16 u16Degamma_Max_Lum;

    //VOP2 3X3
    MS_U8 u83x3_enable_Mode;
    MS_U8 u83x3_Mode;
    MS_U16 u163x3_Ratio2;//0x40 = 1 Q2.6
    MS_U8 u83x3_Manual_Vars_en;
    MS_U8 u83x3_InputCP;
    MS_U8 u83x3_OutputCP;

    MS_U8 u8Compress_settings_Mode;
    MS_U8 u8Compress_dither_Mode;
    MS_U8 u83x3Clip_Mode;

    //VOP2 gamma
    MS_U8 u8Gamma_enable_Mode;
    MS_U8 u8Gamma_Dither_Mode;
    MS_U8 u8Gamma_maxdata_Mode;
    MS_U8 u8Gamma_SRAM_Mode; //not used now

    MS_U8 u8Gamma_Mode_Vars_en;
    MS_U8 u8Gamma_TR;
    MS_U8 u8Gamma_Lut_En;
    MS_U32 *pu32Gamma_Lut_Address;
#if !defined (__aarch64__)
    void *pDummy2;
#endif
    MS_U16 u16Gamma_Lut_Length;

    MS_U8 u8YHSL_R2Y_Mode;
} STU_CFDAPI_Maserati_SDRIP;

typedef struct _STU_CFD_MS_ALG_INTERFACE_DLC
{

  STU_CFDAPI_Maserati_DLCIP stu_Maserati_DLC_Param;

} STU_CFD_MS_ALG_INTERFACE_DLC;

typedef struct _STU_CFD_MS_ALG_INTERFACE_TMO
{
  MS_U8 u8Controls;
  //0 : bypass
  //1 : normal
  //2 : test

  //STU_CFDAPI_Kano_TMOIP        stu_Kano_TMOIP_Param;
  //STU_CFDAPI_Manhattan_TMOIP   stu_Manhattan_TMOIP_Param;
  STU_CFDAPI_Maserati_TMOIP    stu_Maserati_TMO_Param;

} STU_CFD_MS_ALG_INTERFACE_TMO;

typedef struct _STU_CFD_MS_ALG_INTERFACE_HDRIP
{

  //STU_CFDAPI_Kano_HDRIP stu_Kano_HDRIP_Param;
  //STU_CFDAPI_Manhattan_HDRIP stu_Manhattan_HDRIP_Param;
  STU_CFDAPI_Maserati_HDRIP  stu_Maserati_HDRIP_Param;

} STU_CFD_MS_ALG_INTERFACE_HDRIP;

typedef struct _STU_CFD_MS_ALG_INTERFACE_SDRIP
{
  MS_U8 u8Controls;
  //0 : bypass
  //1 : normal
  //2 : test

  //STU_CFDAPI_Kano_SDRIP stu_Kano_SDRIP_Param;
  //STU_CFDAPI_Manhattan_SDRIP stu_Manhattan_SDRIP_Param;
  STU_CFDAPI_Maserati_SDRIP  stu_Maserati_SDRIP_Param;

} STU_CFD_MS_ALG_INTERFACE_SDRIP;


MS_U16 MS_Cfd_Maserati_CheckModes(MS_U8 *temp, MS_U8 mode_upbound);

void Mapi_Cfd_Maserati_DLCIP_Param_Init(STU_CFDAPI_Maserati_DLCIP *pstu_Maserati_DLCIP_Param);
MS_U16 Mapi_Cfd_Maserati_TMOIP_Param_Check(STU_CFDAPI_Maserati_TMOIP *pstu_Maserati_TMOIP_Param);

void Mapi_Cfd_Maserati_TMOIP_Param_Init(STU_CFDAPI_Maserati_TMOIP *pstu_Maserati_TMOIP_Param);
MS_U16 Mapi_Cfd_Maserati_DLCIP_Param_Check(STU_CFDAPI_Maserati_DLCIP *pstu_Maserati_DLCIP_Param);

void Mapi_Cfd_Maserati_SDRIP_Param_Init(STU_CFDAPI_Maserati_SDRIP *pstu_Maserati_SDRIP_Param);
MS_U16 Mapi_Cfd_Maserati_SDRIP_Param_Check(STU_CFDAPI_Maserati_SDRIP *pstu_Maserati_SDRIP_Param);

void Mapi_Cfd_Maserati_HDRIP_Param_Init(STU_CFDAPI_Maserati_HDRIP *pstu_Maserati_HDRIP_Param);
MS_U16 Mapi_Cfd_Maserati_HDRIP_Param_Check(STU_CFDAPI_Maserati_HDRIP *pstu_Maserati_HDRIP_Param);

void MS_Cfd_Maserati_HDRIP_Debug(STU_CFDAPI_Maserati_TMOIP *pstu_Maserati_TMO_Param, STU_CFDAPI_Maserati_HDRIP *pstu_Maserati_HDRIP_Param);
void MS_Cfd_Maserati_SDRIP_Debug(STU_CFDAPI_Maserati_DLCIP *pstu_Maserati_DLC_Param, STU_CFDAPI_Maserati_SDRIP *pstu_Maserati_SDRIP_Param);
#endif //_HAL_COLOR_FORMAT_DRIVER_HARDWARE_H
