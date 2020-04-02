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

#if (RealChip == 0)
#if (NowHW == Curry)
#endif

#define RealChip 1

//Only for Curry ===========================================================
//for test and report
//specify the process of each IP

#define Curry_Control_HDMITX_CSC 0

//for ygain/offset patch
#define CFD_CURRY_PATCH0 0

#define CFD_CURRY_Force_Inside_MC709 1

//refer to "constraints_for_Curry(from Kano).txt"
#define CFD_CURRY_SPECIAL_HANDLE_3P3 1

//0: not support
//1: support
#define CFD_SDR_withGM 0

//0: not support
//1: support
#define CFD_HAL_SUPPORT_DLC 0

//HDR IP
//[0]:can do DLC:0
//[1]:can do GM:1
//[2]:can do TMO:1
//[3]:can do BT2020CL to NCL:0
//[4]:can do xvYCC to other case: 0
//[5]:can do HDRx to HDRy : 1
//[6]:can do SDR to HDR : 1
//[7]:reserved
#define CFD_HDR_IP_CAPABILITY 0x66

//SDR IP
//[0]:can do DLC:0
//[1]:can do GM:0
//[2]:can do TMO:0
//[3]:can do BT2020CL to NCL:0
//[4]:reserved
//[5]:reserved
//[6]:reserved
//[7]:reserved
#define CFD_SDR_IP_CAPABILITY 0x00

//u8HDMISink_Extended_Colorspace
//[0]:xvYCC601
//[1]:xvYCC709
//[2]:sYCC601
//[3]:Adobeycc601
//[4]:Adobergb
//[5]:BT2020 cl
//[6]:BT2020 ncl
//[7]:BT2020 RGB
#define CFD_HDMISINK_COLORIMETRY_CAPABILITY 0xC0

//u8HDMISink_Extended_Colorspace byte 4
//[0]:0
//[1]:0
//[2]:0
//[3]:0
//[4]:0
//[5]:0
//[6]:0
//[7]:DCI P3 RGB
#define CFD_HDMISINK_COLORIMETRY_CAPABILITY_byte4 0x80


//0: not force output HDRMode, auto
//1: force output HDRMode
#define CFD_IN_SDR_FORCEOUTPUT 0x01
//#define CFD_IN_DOLBY_FORCEOUTPUT 0x00
#define CFD_IN_HDR10_FORCEOUTPUT 0x00
#define CFD_IN_HLG_FORCEOUTPUT 0x01

//0:SDR
//1:Dolby
//2:HDR10
//3:HLG

#define CFD_IN_SDR_OUTPUT 0x00
#define CFD_IN_DOLBY_OUTPUT 0x00
#define CFD_IN_HDR10_OUTPUT 0x00
#define CFD_IN_HLG_OUTPUT 0x00

#endif

//set from user
typedef struct
{
    //for PQ EOTF
    MS_U8  dePQclamp_en;

    //PQ code
    //0xff00 = 1
    MS_U16 dePQclamp_value;

    //for HLG2PQ
    MS_U8  u8deHLG_eotf_nor_en;

    //unit is in nits
    //max = 10000
    MS_U16 u16deHLG_eotf_nor_lum_value;

    //OOTF_gamma
    MS_U8 u8deHLG_ootf_gamma_en;

    //gamma : 300nit => gamma 1 , 1000 nits => gamma 1.2
    MS_U8 u8deHLG_ootf_gamma_nitsm100;

    //for SDR2PQ
    MS_U8  u8SDR2PQ_eotf_nor_en;

    //unit is in nits
    //max = 10000
    MS_U16 u16SDR2PQ_eotf_nor_lum_value;

}ST_DEGAMMA_PARAMETERS_EXTENSION;

//used in CFD inside
typedef struct
{
    //for PQ EOTF
    MS_U8  dePQclamp_en;

    //PQ code
    //0xff00 = 1
    MS_U16 dePQclamp_value;

    //for EOTF normalized function
    MS_U8  u8eotf_nor_en;

    //for EOTF normalized function
    //unit is in nits
    //max = 10000
    MS_U16 u16eotf_nor_lum_value;


}ST_DEGAMMA_PARAMETERS_EXTENSION_IN;

typedef struct _STU_CFDAPI_Curry_DLCIP
{
    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_CURRY_TMOIP_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_Curry_TMOIP)

} STU_CFDAPI_Curry_DLCIP;

//refer the details in
//Curry_ColorMap_DecisionTree_Table.xls
typedef struct _STU_CFDAPI_Curry_TMOIP
{
    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_KANO_TMOIP_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_Curry_TMOIP)

    //B02
    MS_U8  u8HDR_TMO_curve_enable_Mode;
    MS_U8  u8HDR_TMO_curve_Mode;
    MS_U8  u8HDR_TMO_curve_setting_Mode;
    //MS_U8  u8TMO_curve_Manual_Mode;

    //B04
    MS_U8  u8HDR_UVC_setting_Mode;

    //for TMO algorithm ,  from user and driver
    //0: not set
    //1: set
    MS_U8   u8HDR_TMO_param_SetbyDriver;

} STU_CFDAPI_Curry_TMOIP;

#if 1
typedef struct __attribute__((packed))
{
    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_Curry_HDRIP_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_Curry_HDRIP)

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

    //STU_CFDAPI_PANEL_FORMAT *pstu_Panel_Param;

    MS_U8 u8HDRIP_Patch;
    //bit 0 : When TMO in M+D IP, assign gamma2.2 to TR of gamma function

    ST_DEGAMMA_PARAMETERS_EXTENSION st_degamma_extension;

} STU_CFDAPI_Curry_HDRIP;
#else
#endif

//follow the column order of decision tree table
//Curry_ColorMap_DecisionTree_Table.xls
typedef struct __attribute__((packed))
{
    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_MAIN_CONTROL_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_Curry_SDRIP)

    //IP2 CSC
    MS_U8 u8SDR_IP2_CSC_Mode;
    MS_U8 u8SDR_IP2_CSC_Ratio1;
    MS_U8 u8SDR_IP2_CSC_Manual_Vars_en;
    MS_U8 u8SDR_IP2_CSC_MC;

    //VIP CM
    MS_U8 u8SDR_VIP_CM_Mode;
    MS_U8 u8SDR_VIP_CM_Ratio1;
    MS_U8 u8SDR_VIP_CM_Manual_Vars_en;
    MS_U8 u8SDR_VIP_CM_MC;

    //Conv420 CM
    MS_U8 u8SDR_Conv420_CM_Mode;
    MS_U8 u8SDR_Conv420_CM_Ratio1;
    MS_U8 u8SDR_Conv420_CM_Manual_Vars_en;
    MS_U8 u8SDR_Conv420_CM_MC;

    #if (Curry_Control_HDMITX_CSC == 1)
    //HDMI TX CSC
    MS_U8 u8SDR_HDMITX_CSC_Mode;
    MS_U8 u8SDR_HDMITX_CSC_Ratio1;
    MS_U8 u8SDR_HDMITX_CSC_Manual_Vars_en;
    MS_U8 u8SDR_HDMITX_CSC_MC;
    #endif

} STU_CFDAPI_Curry_SDRIP;

typedef struct _STU_CFD_MS_ALG_INTERFACE_DLC
{

    STU_CFDAPI_Curry_DLCIP stu_Curry_DLC_Param;

} STU_CFD_MS_ALG_INTERFACE_DLC;

typedef struct _STU_CFD_MS_ALG_INTERFACE_TMO
{
  MS_U8 u8Controls;
  //0 : bypass
  //1 : normal
  //2 : test

    STU_CFDAPI_Curry_TMOIP        stu_Curry_TMOIP_Param;

} STU_CFD_MS_ALG_INTERFACE_TMO;

typedef struct _STU_CFD_MS_ALG_INTERFACE_HDRIP
{

    STU_CFDAPI_Curry_HDRIP stu_Curry_HDRIP_Param;

} STU_CFD_MS_ALG_INTERFACE_HDRIP;

typedef struct _STU_CFD_MS_ALG_INTERFACE_SDRIP
{
  MS_U8 u8Controls;
  //0 : bypass
  //1 : normal
  //2 : test

    STU_CFDAPI_Curry_SDRIP stu_Curry_SDRIP_Param;

} STU_CFD_MS_ALG_INTERFACE_SDRIP;

#if (0==RealChip)

typedef struct
{
    // TMO
    MS_U16 u16SrcMinRatio;                    //default 10
    MS_U16 u16SrcMedRatio;                    //default 512
    MS_U16 u16SrcMaxRatio;                    //default 990

    MS_U8 u8TgtMinFlag;                        //default 1
    MS_U16 u16TgtMin;                          //default 500
    MS_U8 u8TgtMaxFlag;                        //default 0
    MS_U16 u16TgtMax;                          //default 300
    MS_U16 u16TgtMed;

    MS_U16 u16FrontSlopeMin;                    //default 256
    MS_U16 u16FrontSlopeMax;                    //default 512
    MS_U16 u16BackSlopeMin;                     //default 128
    MS_U16 u16BackSlopeMax;                     //default 256

    MS_U16 u16SceneChangeThrd;                  //default 1024
    MS_U16 u16SceneChangeRatioMax;              //default 1024

    MS_U8 u8IIRRatio;                           //default 31
    MS_U8 u8TMO_TargetMode;      // default 0. 0 : keeps the value in initial function  1 : from output source
    MS_U8 u8TMO_Algorithm;                // default 0.  0: 18 level TMO algorithm, 1: 512 level TMO algorithm.
    MS_U16 u16SDRPanelGain;        //

    MS_U16 u16Smin;
    MS_U16 u16Smed;
    MS_U16 u16Smax;
    MS_U16 u16Tmin;
    MS_U16 u16Tmed;
    MS_U16 u16Tmax;

    //0: from ini
    //1: from CFD
    MS_U8  u8TMO_targetconfigs_source;

   //1: has go into TMO set function
   //0: not go into yet
    MS_U8  u8TMO_intoTMOset;


} StuDlc_HDRNewToneMapping;

/*!
 *  Initial  HDR   Settings
 */
typedef struct
{
    /// HDR Enable
    MS_BOOL bHDREnable;
    /// HDR Function Select
    MS_U16 u16HDRFunctionSelect;
    /// HDR Metadata Mpeg VU
    // New tone mapping parameters.
    StuDlc_HDRNewToneMapping DLC_HDRNewToneMappingData;
} StuDlc_HDRinit;

#endif


//function for control of Curry IPs
void Mapi_Cfd_Curry_HDRIP_Param_Init(STU_CFDAPI_Curry_HDRIP *pstu_Curry_HDRIP_Param);
void Mapi_Cfd_Curry_SDRIP_Param_Init(STU_CFDAPI_Curry_SDRIP *pstu_Curry_SDRIP_Param);
void Mapi_Cfd_Curry_TMOIP_Param_Init(STU_CFDAPI_Curry_TMOIP *pstu_Curry_TMOIP_Param);

MS_U16 Mapi_Cfd_Curry_HDRIP_Param_Check(STU_CFDAPI_Curry_HDRIP *pstu_Curry_HDRIP_Param);
MS_U16 Mapi_Cfd_Curry_SDRIP_Param_Check(STU_CFDAPI_Curry_SDRIP *pstu_Curry_SDRIP_Param);
MS_U16 Mapi_Cfd_Curry_TMOIP_Param_Check(STU_CFDAPI_Curry_TMOIP *pstu_Curry_TMOIP_Param);

void MS_Cfd_Curry_HDRIP_Debug(STU_CFDAPI_Curry_TMOIP *pstu_TMO_Param, STU_CFDAPI_Curry_HDRIP *pstu_HDRIP_Param);
void MS_Cfd_Curry_SDRIP_Debug(STU_CFDAPI_Curry_DLCIP *pstu_DLC_Param, STU_CFDAPI_Curry_SDRIP *pstu_SDRIP_Param);

#endif //_HAL_COLOR_FORMAT_DRIVER_HARDWARE_H
