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
#if (NowHW == Kastor)
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
//[1]:can do GM:0
//[2]:can do TMO:1
//[3]:can do BT2020CL to NCL:0
//[4]:can do xvYCC to other case: 0
//[5]:reserved
//[6]:reserved
//[7]:reserved
#define CFD_HDR_IP_CAPABILITY 0x06

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


} STU_CFDAPI_Curry_HDRIP;

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

//follow the column order of decision tree table
//Curry_ColorMap_DecisionTree_Table.xls
typedef struct __attribute__((packed))
{
    //MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_MAIN_CONTROL_ST_VERSION" as input
    //MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_Curry_SDRIP

    //0:off (user control)
    //1:do SDR2HDR (user control)
    //2:auto , by infoFrame/OSD process control structure
    MS_U8 u8UserMode;

    //0:bypass
    //1:SDR2HDR
    MS_U16 u8Status;

    //first R2Y
    MS_U8 u8OSD_R2Yonly_CSC_Mode;
    MS_U8 u8OSD_R2Yonly_CSC_Ratio1;
    MS_U8 u8OSD_R2Yonly_CSC_Manual_Vars_en;
    MS_U8 u8OSD_R2Yonly_CSC_MC;

    //R2R for gamut mapping
    MS_U8 u8OSD_3x3_Mode;
    MS_U16 u16OSD_3x3_Ratio2;//0x40 = 1 Q2.6
    MS_U8 u8OSD_3x3_Manual_Vars_en;
    MS_U8 u8OSD_3x3_InputCP;
    MS_U8 u8OSD_3x3_OutputCP;

    //P LUT
    //0 : linear
    //1: default
    MS_U8 u8OSD_Plut_Mode;

    //gamma
    //0:400
    //1:1000
    //2:4000
    MS_U8 u8OSD_Gamma_SRAM_Mode;
    MS_U8 u8OSD_Gamma_Manual_Vars_en;
    MS_U8 u8OSD_Gamma_TR;
    MS_U8 u8OSD_Gamma_Lut_En;
    MS_U32 *pu32OSD_Gamma_Lut_Address;
    MS_U16 u16OSD_Gamma_Lut_Length;

    //Y2R
    MS_U8 u8OSD_R2Y_CSC_Mode;
    MS_U8 u8OSD_R2Y_CSC_Ratio1;
    MS_U8 u8OSD_R2Y_CSC_Manual_Vars_en;
    MS_U8 u8OSD_R2Y_CSC_MC;

    MS_U8 u8VideoADFlag;

} STU_CFDAPI_Kastor_OSDIP;

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

//the control of OSD process in mali in Curry & Kano
typedef struct _STU_CFDAPI_OSD_PROCESS_Input
{

    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_HDMI_INFOFRAME_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_OSD_PROCESS_CONFIGS)

    //for this function
    //1: on 0:off
    MS_U8 u8OSD_SDR2HDR_en;

    //0:limit 1:full
    //default = full
    MS_U8 u8OSD_IsFullRange;

    //default = RGB444
    MS_U8 u8OSD_Dataformat;

    //default = SDR
    MS_U8 u8OSD_HDRMode;

    //E_OSD_CFIO_CP
    //default = BT709
    MS_U8 u8OSD_colorprimary;

    //default = BT709
    MS_U8 u8OSD_transferf;

    //default = 600
    MS_U16 u16OSD_MaxLumInNits;

    //default = BT2020
    MS_U8 u8Video_colorprimary;

    //default = BT2020
    MS_U8 u8Video_MatrixCoefficients;

    //default 2 = tr use PQ
    MS_U8 u8Video_HDRMode;

    //default = 4000
    MS_U16 u16Video_MaxLumInNits;

    //0:limit 1:full
    //default = limit
    MS_U8 u8Video_IsFullRange;

    //default = YUV444
    MS_U8 u8Video_Dataformat;

    //default >= u16OSD_MaxLumInNits
    MS_U16 u16AntiTMO_SourceInNits;

} STU_CFDAPI_OSD_PROCESS_Input;


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

//end of Only for Curry ===========================================================

//function for control of Curry IPs
void Mapi_Cfd_Curry_HDRIP_Param_Init(STU_CFDAPI_Curry_HDRIP *pstu_Curry_HDRIP_Param);
void Mapi_Cfd_Curry_SDRIP_Param_Init(STU_CFDAPI_Curry_SDRIP *pstu_Curry_SDRIP_Param);
void Mapi_Cfd_Curry_TMOIP_Param_Init(STU_CFDAPI_Curry_TMOIP *pstu_Curry_TMOIP_Param);

MS_U16 Mapi_Cfd_Curry_HDRIP_Param_Check(STU_CFDAPI_Curry_HDRIP *pstu_Curry_HDRIP_Param);
MS_U16 Mapi_Cfd_Curry_SDRIP_Param_Check(STU_CFDAPI_Curry_SDRIP *pstu_Curry_SDRIP_Param);
MS_U16 Mapi_Cfd_Curry_TMOIP_Param_Check(STU_CFDAPI_Curry_TMOIP *pstu_Curry_TMOIP_Param);

void MS_Cfd_Curry_HDRIP_Debug(STU_CFDAPI_Curry_TMOIP *pstu_TMO_Param, STU_CFDAPI_Curry_HDRIP *pstu_HDRIP_Param);
void MS_Cfd_Curry_SDRIP_Debug(STU_CFDAPI_Curry_DLCIP *pstu_DLC_Param, STU_CFDAPI_Curry_SDRIP *pstu_SDRIP_Param);

MS_U16 MS_Cfd_Kastor_OSD_process(STU_CFDAPI_Kastor_OSDIP  *pstu_Kastor_OSDIP_Param, STU_CFDAPI_OSD_PROCESS_Input  *pstu_Cfd_OSD_Process_Configs);

#if RealChip
//for K6
#define REG_SC_YAHAN_BK31_00_L      _PK_L_(0x31, 0x00)
#define REG_SC_YAHAN_BK31_00_H      _PK_H_(0x31, 0x00)
#define REG_SC_YAHAN_BK31_01_L      _PK_L_(0x31, 0x01)
#define REG_SC_YAHAN_BK31_01_H      _PK_H_(0x31, 0x01)
#define REG_SC_YAHAN_BK31_02_L      _PK_L_(0x31, 0x02)
#define REG_SC_YAHAN_BK31_02_H      _PK_H_(0x31, 0x02)
#define REG_SC_YAHAN_BK31_03_L      _PK_L_(0x31, 0x03)
#define REG_SC_YAHAN_BK31_03_H      _PK_H_(0x31, 0x03)
#define REG_SC_YAHAN_BK31_04_L      _PK_L_(0x31, 0x04)
#define REG_SC_YAHAN_BK31_04_H      _PK_H_(0x31, 0x04)
#define REG_SC_YAHAN_BK31_05_L      _PK_L_(0x31, 0x05)
#define REG_SC_YAHAN_BK31_05_H      _PK_H_(0x31, 0x05)
#define REG_SC_YAHAN_BK31_06_L      _PK_L_(0x31, 0x06)
#define REG_SC_YAHAN_BK31_06_H      _PK_H_(0x31, 0x06)
#define REG_SC_YAHAN_BK31_07_L      _PK_L_(0x31, 0x07)
#define REG_SC_YAHAN_BK31_07_H      _PK_H_(0x31, 0x07)
#define REG_SC_YAHAN_BK31_08_L      _PK_L_(0x31, 0x08)
#define REG_SC_YAHAN_BK31_08_H      _PK_H_(0x31, 0x08)
#define REG_SC_YAHAN_BK31_09_L      _PK_L_(0x31, 0x09)
#define REG_SC_YAHAN_BK31_09_H      _PK_H_(0x31, 0x09)
#define REG_SC_YAHAN_BK31_0a_L      _PK_L_(0x31, 0x0a)
#define REG_SC_YAHAN_BK31_0a_H      _PK_H_(0x31, 0x0a)
#define REG_SC_YAHAN_BK31_0b_L      _PK_L_(0x31, 0x0b)
#define REG_SC_YAHAN_BK31_0b_H      _PK_H_(0x31, 0x0b)
#define REG_SC_YAHAN_BK31_0c_L      _PK_L_(0x31, 0x0c)
#define REG_SC_YAHAN_BK31_0c_H      _PK_H_(0x31, 0x0c)
#define REG_SC_YAHAN_BK31_0d_L      _PK_L_(0x31, 0x0d)
#define REG_SC_YAHAN_BK31_0d_H      _PK_H_(0x31, 0x0d)
#define REG_SC_YAHAN_BK31_0e_L      _PK_L_(0x31, 0x0e)
#define REG_SC_YAHAN_BK31_0e_H      _PK_H_(0x31, 0x0e)
#define REG_SC_YAHAN_BK31_0f_L      _PK_L_(0x31, 0x0f)
#define REG_SC_YAHAN_BK31_0f_H      _PK_H_(0x31, 0x0f)
#define REG_SC_YAHAN_BK31_10_L      _PK_L_(0x31, 0x10)
#define REG_SC_YAHAN_BK31_10_H      _PK_H_(0x31, 0x10)
#define REG_SC_YAHAN_BK31_11_L      _PK_L_(0x31, 0x11)
#define REG_SC_YAHAN_BK31_11_H      _PK_H_(0x31, 0x11)
#define REG_SC_YAHAN_BK31_12_L      _PK_L_(0x31, 0x12)
#define REG_SC_YAHAN_BK31_12_H      _PK_H_(0x31, 0x12)
#define REG_SC_YAHAN_BK31_13_L      _PK_L_(0x31, 0x13)
#define REG_SC_YAHAN_BK31_13_H      _PK_H_(0x31, 0x13)
#define REG_SC_YAHAN_BK31_14_L      _PK_L_(0x31, 0x14)
#define REG_SC_YAHAN_BK31_14_H      _PK_H_(0x31, 0x14)
#define REG_SC_YAHAN_BK31_15_L      _PK_L_(0x31, 0x15)
#define REG_SC_YAHAN_BK31_15_H      _PK_H_(0x31, 0x15)
#define REG_SC_YAHAN_BK31_16_L      _PK_L_(0x31, 0x16)
#define REG_SC_YAHAN_BK31_16_H      _PK_H_(0x31, 0x16)
#define REG_SC_YAHAN_BK31_17_L      _PK_L_(0x31, 0x17)
#define REG_SC_YAHAN_BK31_17_H      _PK_H_(0x31, 0x17)
#define REG_SC_YAHAN_BK31_18_L      _PK_L_(0x31, 0x18)
#define REG_SC_YAHAN_BK31_18_H      _PK_H_(0x31, 0x18)
#define REG_SC_YAHAN_BK31_19_L      _PK_L_(0x31, 0x19)
#define REG_SC_YAHAN_BK31_19_H      _PK_H_(0x31, 0x19)
#define REG_SC_YAHAN_BK31_1a_L      _PK_L_(0x31, 0x1a)
#define REG_SC_YAHAN_BK31_1a_H      _PK_H_(0x31, 0x1a)
#define REG_SC_YAHAN_BK31_1b_L      _PK_L_(0x31, 0x1b)
#define REG_SC_YAHAN_BK31_1b_H      _PK_H_(0x31, 0x1b)
#define REG_SC_YAHAN_BK31_1c_L      _PK_L_(0x31, 0x1c)
#define REG_SC_YAHAN_BK31_1c_H      _PK_H_(0x31, 0x1c)
#define REG_SC_YAHAN_BK31_1d_L      _PK_L_(0x31, 0x1d)
#define REG_SC_YAHAN_BK31_1d_H      _PK_H_(0x31, 0x1d)
#define REG_SC_YAHAN_BK31_1e_L      _PK_L_(0x31, 0x1e)
#define REG_SC_YAHAN_BK31_1e_H      _PK_H_(0x31, 0x1e)
#define REG_SC_YAHAN_BK31_1f_L      _PK_L_(0x31, 0x1f)
#define REG_SC_YAHAN_BK31_1f_H      _PK_H_(0x31, 0x1f)
#define REG_SC_YAHAN_BK31_20_L      _PK_L_(0x31, 0x20)
#define REG_SC_YAHAN_BK31_20_H      _PK_H_(0x31, 0x20)
#define REG_SC_YAHAN_BK31_21_L      _PK_L_(0x31, 0x21)
#define REG_SC_YAHAN_BK31_21_H      _PK_H_(0x31, 0x21)
#define REG_SC_YAHAN_BK31_22_L      _PK_L_(0x31, 0x22)
#define REG_SC_YAHAN_BK31_22_H      _PK_H_(0x31, 0x22)
#define REG_SC_YAHAN_BK31_23_L      _PK_L_(0x31, 0x23)
#define REG_SC_YAHAN_BK31_23_H      _PK_H_(0x31, 0x23)
#define REG_SC_YAHAN_BK31_24_L      _PK_L_(0x31, 0x24)
#define REG_SC_YAHAN_BK31_24_H      _PK_H_(0x31, 0x24)
#define REG_SC_YAHAN_BK31_25_L      _PK_L_(0x31, 0x25)
#define REG_SC_YAHAN_BK31_25_H      _PK_H_(0x31, 0x25)
#define REG_SC_YAHAN_BK31_26_L      _PK_L_(0x31, 0x26)
#define REG_SC_YAHAN_BK31_26_H      _PK_H_(0x31, 0x26)
#define REG_SC_YAHAN_BK31_27_L      _PK_L_(0x31, 0x27)
#define REG_SC_YAHAN_BK31_27_H      _PK_H_(0x31, 0x27)
#define REG_SC_YAHAN_BK31_28_L      _PK_L_(0x31, 0x28)
#define REG_SC_YAHAN_BK31_28_H      _PK_H_(0x31, 0x28)
#define REG_SC_YAHAN_BK31_29_L      _PK_L_(0x31, 0x29)
#define REG_SC_YAHAN_BK31_29_H      _PK_H_(0x31, 0x29)
#define REG_SC_YAHAN_BK31_2a_L      _PK_L_(0x31, 0x2a)
#define REG_SC_YAHAN_BK31_2a_H      _PK_H_(0x31, 0x2a)
#define REG_SC_YAHAN_BK31_2b_L      _PK_L_(0x31, 0x2b)
#define REG_SC_YAHAN_BK31_2b_H      _PK_H_(0x31, 0x2b)
#define REG_SC_YAHAN_BK31_2c_L      _PK_L_(0x31, 0x2c)
#define REG_SC_YAHAN_BK31_2c_H      _PK_H_(0x31, 0x2c)
#define REG_SC_YAHAN_BK31_2d_L      _PK_L_(0x31, 0x2d)
#define REG_SC_YAHAN_BK31_2d_H      _PK_H_(0x31, 0x2d)
#define REG_SC_YAHAN_BK31_2e_L      _PK_L_(0x31, 0x2e)
#define REG_SC_YAHAN_BK31_2e_H      _PK_H_(0x31, 0x2e)
#define REG_SC_YAHAN_BK31_2f_L      _PK_L_(0x31, 0x2f)
#define REG_SC_YAHAN_BK31_2f_H      _PK_H_(0x31, 0x2f)
#define REG_SC_YAHAN_BK31_30_L      _PK_L_(0x31, 0x30)
#define REG_SC_YAHAN_BK31_30_H      _PK_H_(0x31, 0x30)
#define REG_SC_YAHAN_BK31_31_L      _PK_L_(0x31, 0x31)
#define REG_SC_YAHAN_BK31_31_H      _PK_H_(0x31, 0x31)
#define REG_SC_YAHAN_BK31_32_L      _PK_L_(0x31, 0x32)
#define REG_SC_YAHAN_BK31_32_H      _PK_H_(0x31, 0x32)
#define REG_SC_YAHAN_BK31_33_L      _PK_L_(0x31, 0x33)
#define REG_SC_YAHAN_BK31_33_H      _PK_H_(0x31, 0x33)
#define REG_SC_YAHAN_BK31_34_L      _PK_L_(0x31, 0x34)
#define REG_SC_YAHAN_BK31_34_H      _PK_H_(0x31, 0x34)
#define REG_SC_YAHAN_BK31_35_L      _PK_L_(0x31, 0x35)
#define REG_SC_YAHAN_BK31_35_H      _PK_H_(0x31, 0x35)
#define REG_SC_YAHAN_BK31_36_L      _PK_L_(0x31, 0x36)
#define REG_SC_YAHAN_BK31_36_H      _PK_H_(0x31, 0x36)
#define REG_SC_YAHAN_BK31_37_L      _PK_L_(0x31, 0x37)
#define REG_SC_YAHAN_BK31_37_H      _PK_H_(0x31, 0x37)
#define REG_SC_YAHAN_BK31_38_L      _PK_L_(0x31, 0x38)
#define REG_SC_YAHAN_BK31_38_H      _PK_H_(0x31, 0x38)
#define REG_SC_YAHAN_BK31_39_L      _PK_L_(0x31, 0x39)
#define REG_SC_YAHAN_BK31_39_H      _PK_H_(0x31, 0x39)
#define REG_SC_YAHAN_BK31_3a_L      _PK_L_(0x31, 0x3a)
#define REG_SC_YAHAN_BK31_3a_H      _PK_H_(0x31, 0x3a)
#define REG_SC_YAHAN_BK31_3b_L      _PK_L_(0x31, 0x3b)
#define REG_SC_YAHAN_BK31_3b_H      _PK_H_(0x31, 0x3b)
#define REG_SC_YAHAN_BK31_3c_L      _PK_L_(0x31, 0x3c)
#define REG_SC_YAHAN_BK31_3c_H      _PK_H_(0x31, 0x3c)
#define REG_SC_YAHAN_BK31_3d_L      _PK_L_(0x31, 0x3d)
#define REG_SC_YAHAN_BK31_3d_H      _PK_H_(0x31, 0x3d)
#define REG_SC_YAHAN_BK31_3e_L      _PK_L_(0x31, 0x3e)
#define REG_SC_YAHAN_BK31_3e_H      _PK_H_(0x31, 0x3e)
#define REG_SC_YAHAN_BK31_3f_L      _PK_L_(0x31, 0x3f)
#define REG_SC_YAHAN_BK31_3f_H      _PK_H_(0x31, 0x3f)
#define REG_SC_YAHAN_BK31_40_L      _PK_L_(0x31, 0x40)
#define REG_SC_YAHAN_BK31_40_H      _PK_H_(0x31, 0x40)
#define REG_SC_YAHAN_BK31_41_L      _PK_L_(0x31, 0x41)
#define REG_SC_YAHAN_BK31_41_H      _PK_H_(0x31, 0x41)
#define REG_SC_YAHAN_BK31_42_L      _PK_L_(0x31, 0x42)
#define REG_SC_YAHAN_BK31_42_H      _PK_H_(0x31, 0x42)
#define REG_SC_YAHAN_BK31_43_L      _PK_L_(0x31, 0x43)
#define REG_SC_YAHAN_BK31_43_H      _PK_H_(0x31, 0x43)
#define REG_SC_YAHAN_BK31_44_L      _PK_L_(0x31, 0x44)
#define REG_SC_YAHAN_BK31_44_H      _PK_H_(0x31, 0x44)
#define REG_SC_YAHAN_BK31_45_L      _PK_L_(0x31, 0x45)
#define REG_SC_YAHAN_BK31_45_H      _PK_H_(0x31, 0x45)
#define REG_SC_YAHAN_BK31_46_L      _PK_L_(0x31, 0x46)
#define REG_SC_YAHAN_BK31_46_H      _PK_H_(0x31, 0x46)
#define REG_SC_YAHAN_BK31_47_L      _PK_L_(0x31, 0x47)
#define REG_SC_YAHAN_BK31_47_H      _PK_H_(0x31, 0x47)
#define REG_SC_YAHAN_BK31_48_L      _PK_L_(0x31, 0x48)
#define REG_SC_YAHAN_BK31_48_H      _PK_H_(0x31, 0x48)
#define REG_SC_YAHAN_BK31_49_L      _PK_L_(0x31, 0x49)
#define REG_SC_YAHAN_BK31_49_H      _PK_H_(0x31, 0x49)
#define REG_SC_YAHAN_BK31_4a_L      _PK_L_(0x31, 0x4a)
#define REG_SC_YAHAN_BK31_4a_H      _PK_H_(0x31, 0x4a)
#define REG_SC_YAHAN_BK31_4b_L      _PK_L_(0x31, 0x4b)
#define REG_SC_YAHAN_BK31_4b_H      _PK_H_(0x31, 0x4b)
#define REG_SC_YAHAN_BK31_4c_L      _PK_L_(0x31, 0x4c)
#define REG_SC_YAHAN_BK31_4c_H      _PK_H_(0x31, 0x4c)
#define REG_SC_YAHAN_BK31_4d_L      _PK_L_(0x31, 0x4d)
#define REG_SC_YAHAN_BK31_4d_H      _PK_H_(0x31, 0x4d)
#define REG_SC_YAHAN_BK31_4e_L      _PK_L_(0x31, 0x4e)
#define REG_SC_YAHAN_BK31_4e_H      _PK_H_(0x31, 0x4e)
#define REG_SC_YAHAN_BK31_4f_L      _PK_L_(0x31, 0x4f)
#define REG_SC_YAHAN_BK31_4f_H      _PK_H_(0x31, 0x4f)
#define REG_SC_YAHAN_BK31_50_L      _PK_L_(0x31, 0x50)
#define REG_SC_YAHAN_BK31_50_H      _PK_H_(0x31, 0x50)
#define REG_SC_YAHAN_BK31_51_L      _PK_L_(0x31, 0x51)
#define REG_SC_YAHAN_BK31_51_H      _PK_H_(0x31, 0x51)
#define REG_SC_YAHAN_BK31_52_L      _PK_L_(0x31, 0x52)
#define REG_SC_YAHAN_BK31_52_H      _PK_H_(0x31, 0x52)
#define REG_SC_YAHAN_BK31_53_L      _PK_L_(0x31, 0x53)
#define REG_SC_YAHAN_BK31_53_H      _PK_H_(0x31, 0x53)
#define REG_SC_YAHAN_BK31_54_L      _PK_L_(0x31, 0x54)
#define REG_SC_YAHAN_BK31_54_H      _PK_H_(0x31, 0x54)
#define REG_SC_YAHAN_BK31_55_L      _PK_L_(0x31, 0x55)
#define REG_SC_YAHAN_BK31_55_H      _PK_H_(0x31, 0x55)
#define REG_SC_YAHAN_BK31_56_L      _PK_L_(0x31, 0x56)
#define REG_SC_YAHAN_BK31_56_H      _PK_H_(0x31, 0x56)
#define REG_SC_YAHAN_BK31_57_L      _PK_L_(0x31, 0x57)
#define REG_SC_YAHAN_BK31_57_H      _PK_H_(0x31, 0x57)
#define REG_SC_YAHAN_BK31_58_L      _PK_L_(0x31, 0x58)
#define REG_SC_YAHAN_BK31_58_H      _PK_H_(0x31, 0x58)
#define REG_SC_YAHAN_BK31_59_L      _PK_L_(0x31, 0x59)
#define REG_SC_YAHAN_BK31_59_H      _PK_H_(0x31, 0x59)
#define REG_SC_YAHAN_BK31_5a_L      _PK_L_(0x31, 0x5a)
#define REG_SC_YAHAN_BK31_5a_H      _PK_H_(0x31, 0x5a)
#define REG_SC_YAHAN_BK31_5b_L      _PK_L_(0x31, 0x5b)
#define REG_SC_YAHAN_BK31_5b_H      _PK_H_(0x31, 0x5b)
#define REG_SC_YAHAN_BK31_5c_L      _PK_L_(0x31, 0x5c)
#define REG_SC_YAHAN_BK31_5c_H      _PK_H_(0x31, 0x5c)
#define REG_SC_YAHAN_BK31_5d_L      _PK_L_(0x31, 0x5d)
#define REG_SC_YAHAN_BK31_5d_H      _PK_H_(0x31, 0x5d)
#define REG_SC_YAHAN_BK31_5e_L      _PK_L_(0x31, 0x5e)
#define REG_SC_YAHAN_BK31_5e_H      _PK_H_(0x31, 0x5e)
#define REG_SC_YAHAN_BK31_5f_L      _PK_L_(0x31, 0x5f)
#define REG_SC_YAHAN_BK31_5f_H      _PK_H_(0x31, 0x5f)
#define REG_SC_YAHAN_BK31_60_L      _PK_L_(0x31, 0x60)
#define REG_SC_YAHAN_BK31_60_H      _PK_H_(0x31, 0x60)
#define REG_SC_YAHAN_BK31_61_L      _PK_L_(0x31, 0x61)
#define REG_SC_YAHAN_BK31_61_H      _PK_H_(0x31, 0x61)
#define REG_SC_YAHAN_BK31_62_L      _PK_L_(0x31, 0x62)
#define REG_SC_YAHAN_BK31_62_H      _PK_H_(0x31, 0x62)
#define REG_SC_YAHAN_BK31_63_L      _PK_L_(0x31, 0x63)
#define REG_SC_YAHAN_BK31_63_H      _PK_H_(0x31, 0x63)
#define REG_SC_YAHAN_BK31_64_L      _PK_L_(0x31, 0x64)
#define REG_SC_YAHAN_BK31_64_H      _PK_H_(0x31, 0x64)
#define REG_SC_YAHAN_BK31_65_L      _PK_L_(0x31, 0x65)
#define REG_SC_YAHAN_BK31_65_H      _PK_H_(0x31, 0x65)
#define REG_SC_YAHAN_BK31_66_L      _PK_L_(0x31, 0x66)
#define REG_SC_YAHAN_BK31_66_H      _PK_H_(0x31, 0x66)
#define REG_SC_YAHAN_BK31_67_L      _PK_L_(0x31, 0x67)
#define REG_SC_YAHAN_BK31_67_H      _PK_H_(0x31, 0x67)
#define REG_SC_YAHAN_BK31_68_L      _PK_L_(0x31, 0x68)
#define REG_SC_YAHAN_BK31_68_H      _PK_H_(0x31, 0x68)
#define REG_SC_YAHAN_BK31_69_L      _PK_L_(0x31, 0x69)
#define REG_SC_YAHAN_BK31_69_H      _PK_H_(0x31, 0x69)
#define REG_SC_YAHAN_BK31_6a_L      _PK_L_(0x31, 0x6a)
#define REG_SC_YAHAN_BK31_6a_H      _PK_H_(0x31, 0x6a)
#define REG_SC_YAHAN_BK31_6b_L      _PK_L_(0x31, 0x6b)
#define REG_SC_YAHAN_BK31_6b_H      _PK_H_(0x31, 0x6b)
#define REG_SC_YAHAN_BK31_6c_L      _PK_L_(0x31, 0x6c)
#define REG_SC_YAHAN_BK31_6c_H      _PK_H_(0x31, 0x6c)
#define REG_SC_YAHAN_BK31_6d_L      _PK_L_(0x31, 0x6d)
#define REG_SC_YAHAN_BK31_6d_H      _PK_H_(0x31, 0x6d)
#define REG_SC_YAHAN_BK31_6e_L      _PK_L_(0x31, 0x6e)
#define REG_SC_YAHAN_BK31_6e_H      _PK_H_(0x31, 0x6e)
#define REG_SC_YAHAN_BK31_6f_L      _PK_L_(0x31, 0x6f)
#define REG_SC_YAHAN_BK31_6f_H      _PK_H_(0x31, 0x6f)
#define REG_SC_YAHAN_BK31_70_L      _PK_L_(0x31, 0x70)
#define REG_SC_YAHAN_BK31_70_H      _PK_H_(0x31, 0x70)
#define REG_SC_YAHAN_BK31_71_L      _PK_L_(0x31, 0x71)
#define REG_SC_YAHAN_BK31_71_H      _PK_H_(0x31, 0x71)
#define REG_SC_YAHAN_BK31_72_L      _PK_L_(0x31, 0x72)
#define REG_SC_YAHAN_BK31_72_H      _PK_H_(0x31, 0x72)
#define REG_SC_YAHAN_BK31_73_L      _PK_L_(0x31, 0x73)
#define REG_SC_YAHAN_BK31_73_H      _PK_H_(0x31, 0x73)
#define REG_SC_YAHAN_BK31_74_L      _PK_L_(0x31, 0x74)
#define REG_SC_YAHAN_BK31_74_H      _PK_H_(0x31, 0x74)
#define REG_SC_YAHAN_BK31_75_L      _PK_L_(0x31, 0x75)
#define REG_SC_YAHAN_BK31_75_H      _PK_H_(0x31, 0x75)
#define REG_SC_YAHAN_BK31_76_L      _PK_L_(0x31, 0x76)
#define REG_SC_YAHAN_BK31_76_H      _PK_H_(0x31, 0x76)
#define REG_SC_YAHAN_BK31_77_L      _PK_L_(0x31, 0x77)
#define REG_SC_YAHAN_BK31_77_H      _PK_H_(0x31, 0x77)
#define REG_SC_YAHAN_BK31_78_L      _PK_L_(0x31, 0x78)
#define REG_SC_YAHAN_BK31_78_H      _PK_H_(0x31, 0x78)
#define REG_SC_YAHAN_BK31_79_L      _PK_L_(0x31, 0x79)
#define REG_SC_YAHAN_BK31_79_H      _PK_H_(0x31, 0x79)
#define REG_SC_YAHAN_BK31_7a_L      _PK_L_(0x31, 0x7a)
#define REG_SC_YAHAN_BK31_7a_H      _PK_H_(0x31, 0x7a)
#define REG_SC_YAHAN_BK31_7b_L      _PK_L_(0x31, 0x7b)
#define REG_SC_YAHAN_BK31_7b_H      _PK_H_(0x31, 0x7b)
#define REG_SC_YAHAN_BK31_7c_L      _PK_L_(0x31, 0x7c)
#define REG_SC_YAHAN_BK31_7c_H      _PK_H_(0x31, 0x7c)
#define REG_SC_YAHAN_BK31_7d_L      _PK_L_(0x31, 0x7d)
#define REG_SC_YAHAN_BK31_7d_H      _PK_H_(0x31, 0x7d)
#define REG_SC_YAHAN_BK31_7e_L      _PK_L_(0x31, 0x7e)
#define REG_SC_YAHAN_BK31_7e_H      _PK_H_(0x31, 0x7e)
#define REG_SC_YAHAN_BK31_7f_L      _PK_L_(0x31, 0x7f)
#define REG_SC_YAHAN_BK31_7f_H      _PK_H_(0x31, 0x7f)

#define REG_SC_YAHAN_BK32_00_L      _PK_L_(0x32, 0x00)
#define REG_SC_YAHAN_BK32_00_H      _PK_H_(0x32, 0x00)
#define REG_SC_YAHAN_BK32_01_L      _PK_L_(0x32, 0x01)
#define REG_SC_YAHAN_BK32_01_H      _PK_H_(0x32, 0x01)
#define REG_SC_YAHAN_BK32_02_L      _PK_L_(0x32, 0x02)
#define REG_SC_YAHAN_BK32_02_H      _PK_H_(0x32, 0x02)
#define REG_SC_YAHAN_BK32_03_L      _PK_L_(0x32, 0x03)
#define REG_SC_YAHAN_BK32_03_H      _PK_H_(0x32, 0x03)
#define REG_SC_YAHAN_BK32_04_L      _PK_L_(0x32, 0x04)
#define REG_SC_YAHAN_BK32_04_H      _PK_H_(0x32, 0x04)
#define REG_SC_YAHAN_BK32_05_L      _PK_L_(0x32, 0x05)
#define REG_SC_YAHAN_BK32_05_H      _PK_H_(0x32, 0x05)
#define REG_SC_YAHAN_BK32_06_L      _PK_L_(0x32, 0x06)
#define REG_SC_YAHAN_BK32_06_H      _PK_H_(0x32, 0x06)
#define REG_SC_YAHAN_BK32_07_L      _PK_L_(0x32, 0x07)
#define REG_SC_YAHAN_BK32_07_H      _PK_H_(0x32, 0x07)
#define REG_SC_YAHAN_BK32_08_L      _PK_L_(0x32, 0x08)
#define REG_SC_YAHAN_BK32_08_H      _PK_H_(0x32, 0x08)
#define REG_SC_YAHAN_BK32_09_L      _PK_L_(0x32, 0x09)
#define REG_SC_YAHAN_BK32_09_H      _PK_H_(0x32, 0x09)
#define REG_SC_YAHAN_BK32_0a_L      _PK_L_(0x32, 0x0a)
#define REG_SC_YAHAN_BK32_0a_H      _PK_H_(0x32, 0x0a)
#define REG_SC_YAHAN_BK32_0b_L      _PK_L_(0x32, 0x0b)
#define REG_SC_YAHAN_BK32_0b_H      _PK_H_(0x32, 0x0b)
#define REG_SC_YAHAN_BK32_0c_L      _PK_L_(0x32, 0x0c)
#define REG_SC_YAHAN_BK32_0c_H      _PK_H_(0x32, 0x0c)
#define REG_SC_YAHAN_BK32_0d_L      _PK_L_(0x32, 0x0d)
#define REG_SC_YAHAN_BK32_0d_H      _PK_H_(0x32, 0x0d)
#define REG_SC_YAHAN_BK32_0e_L      _PK_L_(0x32, 0x0e)
#define REG_SC_YAHAN_BK32_0e_H      _PK_H_(0x32, 0x0e)
#define REG_SC_YAHAN_BK32_0f_L      _PK_L_(0x32, 0x0f)
#define REG_SC_YAHAN_BK32_0f_H      _PK_H_(0x32, 0x0f)
#define REG_SC_YAHAN_BK32_10_L      _PK_L_(0x32, 0x10)
#define REG_SC_YAHAN_BK32_10_H      _PK_H_(0x32, 0x10)
#define REG_SC_YAHAN_BK32_11_L      _PK_L_(0x32, 0x11)
#define REG_SC_YAHAN_BK32_11_H      _PK_H_(0x32, 0x11)
#define REG_SC_YAHAN_BK32_12_L      _PK_L_(0x32, 0x12)
#define REG_SC_YAHAN_BK32_12_H      _PK_H_(0x32, 0x12)
#define REG_SC_YAHAN_BK32_13_L      _PK_L_(0x32, 0x13)
#define REG_SC_YAHAN_BK32_13_H      _PK_H_(0x32, 0x13)
#define REG_SC_YAHAN_BK32_14_L      _PK_L_(0x32, 0x14)
#define REG_SC_YAHAN_BK32_14_H      _PK_H_(0x32, 0x14)
#define REG_SC_YAHAN_BK32_15_L      _PK_L_(0x32, 0x15)
#define REG_SC_YAHAN_BK32_15_H      _PK_H_(0x32, 0x15)
#define REG_SC_YAHAN_BK32_16_L      _PK_L_(0x32, 0x16)
#define REG_SC_YAHAN_BK32_16_H      _PK_H_(0x32, 0x16)
#define REG_SC_YAHAN_BK32_17_L      _PK_L_(0x32, 0x17)
#define REG_SC_YAHAN_BK32_17_H      _PK_H_(0x32, 0x17)
#define REG_SC_YAHAN_BK32_18_L      _PK_L_(0x32, 0x18)
#define REG_SC_YAHAN_BK32_18_H      _PK_H_(0x32, 0x18)
#define REG_SC_YAHAN_BK32_19_L      _PK_L_(0x32, 0x19)
#define REG_SC_YAHAN_BK32_19_H      _PK_H_(0x32, 0x19)
#define REG_SC_YAHAN_BK32_1a_L      _PK_L_(0x32, 0x1a)
#define REG_SC_YAHAN_BK32_1a_H      _PK_H_(0x32, 0x1a)
#define REG_SC_YAHAN_BK32_1b_L      _PK_L_(0x32, 0x1b)
#define REG_SC_YAHAN_BK32_1b_H      _PK_H_(0x32, 0x1b)
#define REG_SC_YAHAN_BK32_1c_L      _PK_L_(0x32, 0x1c)
#define REG_SC_YAHAN_BK32_1c_H      _PK_H_(0x32, 0x1c)
#define REG_SC_YAHAN_BK32_1d_L      _PK_L_(0x32, 0x1d)
#define REG_SC_YAHAN_BK32_1d_H      _PK_H_(0x32, 0x1d)
#define REG_SC_YAHAN_BK32_1e_L      _PK_L_(0x32, 0x1e)
#define REG_SC_YAHAN_BK32_1e_H      _PK_H_(0x32, 0x1e)
#define REG_SC_YAHAN_BK32_1f_L      _PK_L_(0x32, 0x1f)
#define REG_SC_YAHAN_BK32_1f_H      _PK_H_(0x32, 0x1f)
#define REG_SC_YAHAN_BK32_20_L      _PK_L_(0x32, 0x20)
#define REG_SC_YAHAN_BK32_20_H      _PK_H_(0x32, 0x20)
#define REG_SC_YAHAN_BK32_21_L      _PK_L_(0x32, 0x21)
#define REG_SC_YAHAN_BK32_21_H      _PK_H_(0x32, 0x21)
#define REG_SC_YAHAN_BK32_22_L      _PK_L_(0x32, 0x22)
#define REG_SC_YAHAN_BK32_22_H      _PK_H_(0x32, 0x22)
#define REG_SC_YAHAN_BK32_23_L      _PK_L_(0x32, 0x23)
#define REG_SC_YAHAN_BK32_23_H      _PK_H_(0x32, 0x23)
#define REG_SC_YAHAN_BK32_24_L      _PK_L_(0x32, 0x24)
#define REG_SC_YAHAN_BK32_24_H      _PK_H_(0x32, 0x24)
#define REG_SC_YAHAN_BK32_25_L      _PK_L_(0x32, 0x25)
#define REG_SC_YAHAN_BK32_25_H      _PK_H_(0x32, 0x25)
#define REG_SC_YAHAN_BK32_26_L      _PK_L_(0x32, 0x26)
#define REG_SC_YAHAN_BK32_26_H      _PK_H_(0x32, 0x26)
#define REG_SC_YAHAN_BK32_27_L      _PK_L_(0x32, 0x27)
#define REG_SC_YAHAN_BK32_27_H      _PK_H_(0x32, 0x27)
#define REG_SC_YAHAN_BK32_28_L      _PK_L_(0x32, 0x28)
#define REG_SC_YAHAN_BK32_28_H      _PK_H_(0x32, 0x28)
#define REG_SC_YAHAN_BK32_29_L      _PK_L_(0x32, 0x29)
#define REG_SC_YAHAN_BK32_29_H      _PK_H_(0x32, 0x29)
#define REG_SC_YAHAN_BK32_2a_L      _PK_L_(0x32, 0x2a)
#define REG_SC_YAHAN_BK32_2a_H      _PK_H_(0x32, 0x2a)
#define REG_SC_YAHAN_BK32_2b_L      _PK_L_(0x32, 0x2b)
#define REG_SC_YAHAN_BK32_2b_H      _PK_H_(0x32, 0x2b)
#define REG_SC_YAHAN_BK32_2c_L      _PK_L_(0x32, 0x2c)
#define REG_SC_YAHAN_BK32_2c_H      _PK_H_(0x32, 0x2c)
#define REG_SC_YAHAN_BK32_2d_L      _PK_L_(0x32, 0x2d)
#define REG_SC_YAHAN_BK32_2d_H      _PK_H_(0x32, 0x2d)
#define REG_SC_YAHAN_BK32_2e_L      _PK_L_(0x32, 0x2e)
#define REG_SC_YAHAN_BK32_2e_H      _PK_H_(0x32, 0x2e)
#define REG_SC_YAHAN_BK32_2f_L      _PK_L_(0x32, 0x2f)
#define REG_SC_YAHAN_BK32_2f_H      _PK_H_(0x32, 0x2f)
#define REG_SC_YAHAN_BK32_30_L      _PK_L_(0x32, 0x30)
#define REG_SC_YAHAN_BK32_30_H      _PK_H_(0x32, 0x30)
#define REG_SC_YAHAN_BK32_31_L      _PK_L_(0x32, 0x31)
#define REG_SC_YAHAN_BK32_31_H      _PK_H_(0x32, 0x31)
#define REG_SC_YAHAN_BK32_32_L      _PK_L_(0x32, 0x32)
#define REG_SC_YAHAN_BK32_32_H      _PK_H_(0x32, 0x32)
#define REG_SC_YAHAN_BK32_33_L      _PK_L_(0x32, 0x33)
#define REG_SC_YAHAN_BK32_33_H      _PK_H_(0x32, 0x33)
#define REG_SC_YAHAN_BK32_34_L      _PK_L_(0x32, 0x34)
#define REG_SC_YAHAN_BK32_34_H      _PK_H_(0x32, 0x34)
#define REG_SC_YAHAN_BK32_35_L      _PK_L_(0x32, 0x35)
#define REG_SC_YAHAN_BK32_35_H      _PK_H_(0x32, 0x35)
#define REG_SC_YAHAN_BK32_36_L      _PK_L_(0x32, 0x36)
#define REG_SC_YAHAN_BK32_36_H      _PK_H_(0x32, 0x36)
#define REG_SC_YAHAN_BK32_37_L      _PK_L_(0x32, 0x37)
#define REG_SC_YAHAN_BK32_37_H      _PK_H_(0x32, 0x37)
#define REG_SC_YAHAN_BK32_38_L      _PK_L_(0x32, 0x38)
#define REG_SC_YAHAN_BK32_38_H      _PK_H_(0x32, 0x38)
#define REG_SC_YAHAN_BK32_39_L      _PK_L_(0x32, 0x39)
#define REG_SC_YAHAN_BK32_39_H      _PK_H_(0x32, 0x39)
#define REG_SC_YAHAN_BK32_3a_L      _PK_L_(0x32, 0x3a)
#define REG_SC_YAHAN_BK32_3a_H      _PK_H_(0x32, 0x3a)
#define REG_SC_YAHAN_BK32_3b_L      _PK_L_(0x32, 0x3b)
#define REG_SC_YAHAN_BK32_3b_H      _PK_H_(0x32, 0x3b)
#define REG_SC_YAHAN_BK32_3c_L      _PK_L_(0x32, 0x3c)
#define REG_SC_YAHAN_BK32_3c_H      _PK_H_(0x32, 0x3c)
#define REG_SC_YAHAN_BK32_3d_L      _PK_L_(0x32, 0x3d)
#define REG_SC_YAHAN_BK32_3d_H      _PK_H_(0x32, 0x3d)
#define REG_SC_YAHAN_BK32_3e_L      _PK_L_(0x32, 0x3e)
#define REG_SC_YAHAN_BK32_3e_H      _PK_H_(0x32, 0x3e)
#define REG_SC_YAHAN_BK32_3f_L      _PK_L_(0x32, 0x3f)
#define REG_SC_YAHAN_BK32_3f_H      _PK_H_(0x32, 0x3f)
#define REG_SC_YAHAN_BK32_40_L      _PK_L_(0x32, 0x40)
#define REG_SC_YAHAN_BK32_40_H      _PK_H_(0x32, 0x40)
#define REG_SC_YAHAN_BK32_41_L      _PK_L_(0x32, 0x41)
#define REG_SC_YAHAN_BK32_41_H      _PK_H_(0x32, 0x41)
#define REG_SC_YAHAN_BK32_42_L      _PK_L_(0x32, 0x42)
#define REG_SC_YAHAN_BK32_42_H      _PK_H_(0x32, 0x42)
#define REG_SC_YAHAN_BK32_43_L      _PK_L_(0x32, 0x43)
#define REG_SC_YAHAN_BK32_43_H      _PK_H_(0x32, 0x43)
#define REG_SC_YAHAN_BK32_44_L      _PK_L_(0x32, 0x44)
#define REG_SC_YAHAN_BK32_44_H      _PK_H_(0x32, 0x44)
#define REG_SC_YAHAN_BK32_45_L      _PK_L_(0x32, 0x45)
#define REG_SC_YAHAN_BK32_45_H      _PK_H_(0x32, 0x45)
#define REG_SC_YAHAN_BK32_46_L      _PK_L_(0x32, 0x46)
#define REG_SC_YAHAN_BK32_46_H      _PK_H_(0x32, 0x46)
#define REG_SC_YAHAN_BK32_47_L      _PK_L_(0x32, 0x47)
#define REG_SC_YAHAN_BK32_47_H      _PK_H_(0x32, 0x47)
#define REG_SC_YAHAN_BK32_48_L      _PK_L_(0x32, 0x48)
#define REG_SC_YAHAN_BK32_48_H      _PK_H_(0x32, 0x48)
#define REG_SC_YAHAN_BK32_49_L      _PK_L_(0x32, 0x49)
#define REG_SC_YAHAN_BK32_49_H      _PK_H_(0x32, 0x49)
#define REG_SC_YAHAN_BK32_4a_L      _PK_L_(0x32, 0x4a)
#define REG_SC_YAHAN_BK32_4a_H      _PK_H_(0x32, 0x4a)
#define REG_SC_YAHAN_BK32_4b_L      _PK_L_(0x32, 0x4b)
#define REG_SC_YAHAN_BK32_4b_H      _PK_H_(0x32, 0x4b)
#define REG_SC_YAHAN_BK32_4c_L      _PK_L_(0x32, 0x4c)
#define REG_SC_YAHAN_BK32_4c_H      _PK_H_(0x32, 0x4c)
#define REG_SC_YAHAN_BK32_4d_L      _PK_L_(0x32, 0x4d)
#define REG_SC_YAHAN_BK32_4d_H      _PK_H_(0x32, 0x4d)
#define REG_SC_YAHAN_BK32_4e_L      _PK_L_(0x32, 0x4e)
#define REG_SC_YAHAN_BK32_4e_H      _PK_H_(0x32, 0x4e)
#define REG_SC_YAHAN_BK32_4f_L      _PK_L_(0x32, 0x4f)
#define REG_SC_YAHAN_BK32_4f_H      _PK_H_(0x32, 0x4f)
#define REG_SC_YAHAN_BK32_50_L      _PK_L_(0x32, 0x50)
#define REG_SC_YAHAN_BK32_50_H      _PK_H_(0x32, 0x50)
#define REG_SC_YAHAN_BK32_51_L      _PK_L_(0x32, 0x51)
#define REG_SC_YAHAN_BK32_51_H      _PK_H_(0x32, 0x51)
#define REG_SC_YAHAN_BK32_52_L      _PK_L_(0x32, 0x52)
#define REG_SC_YAHAN_BK32_52_H      _PK_H_(0x32, 0x52)
#define REG_SC_YAHAN_BK32_53_L      _PK_L_(0x32, 0x53)
#define REG_SC_YAHAN_BK32_53_H      _PK_H_(0x32, 0x53)
#define REG_SC_YAHAN_BK32_54_L      _PK_L_(0x32, 0x54)
#define REG_SC_YAHAN_BK32_54_H      _PK_H_(0x32, 0x54)
#define REG_SC_YAHAN_BK32_55_L      _PK_L_(0x32, 0x55)
#define REG_SC_YAHAN_BK32_55_H      _PK_H_(0x32, 0x55)
#define REG_SC_YAHAN_BK32_56_L      _PK_L_(0x32, 0x56)
#define REG_SC_YAHAN_BK32_56_H      _PK_H_(0x32, 0x56)
#define REG_SC_YAHAN_BK32_57_L      _PK_L_(0x32, 0x57)
#define REG_SC_YAHAN_BK32_57_H      _PK_H_(0x32, 0x57)
#define REG_SC_YAHAN_BK32_58_L      _PK_L_(0x32, 0x58)
#define REG_SC_YAHAN_BK32_58_H      _PK_H_(0x32, 0x58)
#define REG_SC_YAHAN_BK32_59_L      _PK_L_(0x32, 0x59)
#define REG_SC_YAHAN_BK32_59_H      _PK_H_(0x32, 0x59)
#define REG_SC_YAHAN_BK32_5a_L      _PK_L_(0x32, 0x5a)
#define REG_SC_YAHAN_BK32_5a_H      _PK_H_(0x32, 0x5a)
#define REG_SC_YAHAN_BK32_5b_L      _PK_L_(0x32, 0x5b)
#define REG_SC_YAHAN_BK32_5b_H      _PK_H_(0x32, 0x5b)
#define REG_SC_YAHAN_BK32_5c_L      _PK_L_(0x32, 0x5c)
#define REG_SC_YAHAN_BK32_5c_H      _PK_H_(0x32, 0x5c)
#define REG_SC_YAHAN_BK32_5d_L      _PK_L_(0x32, 0x5d)
#define REG_SC_YAHAN_BK32_5d_H      _PK_H_(0x32, 0x5d)
#define REG_SC_YAHAN_BK32_5e_L      _PK_L_(0x32, 0x5e)
#define REG_SC_YAHAN_BK32_5e_H      _PK_H_(0x32, 0x5e)
#define REG_SC_YAHAN_BK32_5f_L      _PK_L_(0x32, 0x5f)
#define REG_SC_YAHAN_BK32_5f_H      _PK_H_(0x32, 0x5f)
#define REG_SC_YAHAN_BK32_60_L      _PK_L_(0x32, 0x60)
#define REG_SC_YAHAN_BK32_60_H      _PK_H_(0x32, 0x60)
#define REG_SC_YAHAN_BK32_61_L      _PK_L_(0x32, 0x61)
#define REG_SC_YAHAN_BK32_61_H      _PK_H_(0x32, 0x61)
#define REG_SC_YAHAN_BK32_62_L      _PK_L_(0x32, 0x62)
#define REG_SC_YAHAN_BK32_62_H      _PK_H_(0x32, 0x62)
#define REG_SC_YAHAN_BK32_63_L      _PK_L_(0x32, 0x63)
#define REG_SC_YAHAN_BK32_63_H      _PK_H_(0x32, 0x63)
#define REG_SC_YAHAN_BK32_64_L      _PK_L_(0x32, 0x64)
#define REG_SC_YAHAN_BK32_64_H      _PK_H_(0x32, 0x64)
#define REG_SC_YAHAN_BK32_65_L      _PK_L_(0x32, 0x65)
#define REG_SC_YAHAN_BK32_65_H      _PK_H_(0x32, 0x65)
#define REG_SC_YAHAN_BK32_66_L      _PK_L_(0x32, 0x66)
#define REG_SC_YAHAN_BK32_66_H      _PK_H_(0x32, 0x66)
#define REG_SC_YAHAN_BK32_67_L      _PK_L_(0x32, 0x67)
#define REG_SC_YAHAN_BK32_67_H      _PK_H_(0x32, 0x67)
#define REG_SC_YAHAN_BK32_68_L      _PK_L_(0x32, 0x68)
#define REG_SC_YAHAN_BK32_68_H      _PK_H_(0x32, 0x68)
#define REG_SC_YAHAN_BK32_69_L      _PK_L_(0x32, 0x69)
#define REG_SC_YAHAN_BK32_69_H      _PK_H_(0x32, 0x69)
#define REG_SC_YAHAN_BK32_6a_L      _PK_L_(0x32, 0x6a)
#define REG_SC_YAHAN_BK32_6a_H      _PK_H_(0x32, 0x6a)
#define REG_SC_YAHAN_BK32_6b_L      _PK_L_(0x32, 0x6b)
#define REG_SC_YAHAN_BK32_6b_H      _PK_H_(0x32, 0x6b)
#define REG_SC_YAHAN_BK32_6c_L      _PK_L_(0x32, 0x6c)
#define REG_SC_YAHAN_BK32_6c_H      _PK_H_(0x32, 0x6c)
#define REG_SC_YAHAN_BK32_6d_L      _PK_L_(0x32, 0x6d)
#define REG_SC_YAHAN_BK32_6d_H      _PK_H_(0x32, 0x6d)
#define REG_SC_YAHAN_BK32_6e_L      _PK_L_(0x32, 0x6e)
#define REG_SC_YAHAN_BK32_6e_H      _PK_H_(0x32, 0x6e)
#define REG_SC_YAHAN_BK32_6f_L      _PK_L_(0x32, 0x6f)
#define REG_SC_YAHAN_BK32_6f_H      _PK_H_(0x32, 0x6f)
#define REG_SC_YAHAN_BK32_70_L      _PK_L_(0x32, 0x70)
#define REG_SC_YAHAN_BK32_70_H      _PK_H_(0x32, 0x70)
#define REG_SC_YAHAN_BK32_71_L      _PK_L_(0x32, 0x71)
#define REG_SC_YAHAN_BK32_71_H      _PK_H_(0x32, 0x71)
#define REG_SC_YAHAN_BK32_72_L      _PK_L_(0x32, 0x72)
#define REG_SC_YAHAN_BK32_72_H      _PK_H_(0x32, 0x72)
#define REG_SC_YAHAN_BK32_73_L      _PK_L_(0x32, 0x73)
#define REG_SC_YAHAN_BK32_73_H      _PK_H_(0x32, 0x73)
#define REG_SC_YAHAN_BK32_74_L      _PK_L_(0x32, 0x74)
#define REG_SC_YAHAN_BK32_74_H      _PK_H_(0x32, 0x74)
#define REG_SC_YAHAN_BK32_75_L      _PK_L_(0x32, 0x75)
#define REG_SC_YAHAN_BK32_75_H      _PK_H_(0x32, 0x75)
#define REG_SC_YAHAN_BK32_76_L      _PK_L_(0x32, 0x76)
#define REG_SC_YAHAN_BK32_76_H      _PK_H_(0x32, 0x76)
#define REG_SC_YAHAN_BK32_77_L      _PK_L_(0x32, 0x77)
#define REG_SC_YAHAN_BK32_77_H      _PK_H_(0x32, 0x77)
#define REG_SC_YAHAN_BK32_78_L      _PK_L_(0x32, 0x78)
#define REG_SC_YAHAN_BK32_78_H      _PK_H_(0x32, 0x78)
#define REG_SC_YAHAN_BK32_79_L      _PK_L_(0x32, 0x79)
#define REG_SC_YAHAN_BK32_79_H      _PK_H_(0x32, 0x79)
#define REG_SC_YAHAN_BK32_7a_L      _PK_L_(0x32, 0x7a)
#define REG_SC_YAHAN_BK32_7a_H      _PK_H_(0x32, 0x7a)
#define REG_SC_YAHAN_BK32_7b_L      _PK_L_(0x32, 0x7b)
#define REG_SC_YAHAN_BK32_7b_H      _PK_H_(0x32, 0x7b)
#define REG_SC_YAHAN_BK32_7c_L      _PK_L_(0x32, 0x7c)
#define REG_SC_YAHAN_BK32_7c_H      _PK_H_(0x32, 0x7c)
#define REG_SC_YAHAN_BK32_7d_L      _PK_L_(0x32, 0x7d)
#define REG_SC_YAHAN_BK32_7d_H      _PK_H_(0x32, 0x7d)
#define REG_SC_YAHAN_BK32_7e_L      _PK_L_(0x32, 0x7e)
#define REG_SC_YAHAN_BK32_7e_H      _PK_H_(0x32, 0x7e)
#define REG_SC_YAHAN_BK32_7f_L      _PK_L_(0x32, 0x7f)
#define REG_SC_YAHAN_BK32_7f_H      _PK_H_(0x32, 0x7f)
#define REG_SC_YAHAN_BK32_80_L      _PK_L_(0x32, 0x80)
#define REG_SC_YAHAN_BK32_80_H      _PK_H_(0x32, 0x80)
#define REG_SC_YAHAN_BK32_81_L      _PK_L_(0x32, 0x81)
#define REG_SC_YAHAN_BK32_81_H      _PK_H_(0x32, 0x81)
#define REG_SC_YAHAN_BK32_82_L      _PK_L_(0x32, 0x82)
#define REG_SC_YAHAN_BK32_82_H      _PK_H_(0x32, 0x82)
#define REG_SC_YAHAN_BK32_83_L      _PK_L_(0x32, 0x83)
#define REG_SC_YAHAN_BK32_83_H      _PK_H_(0x32, 0x83)
#define REG_SC_YAHAN_BK32_84_L      _PK_L_(0x32, 0x84)
#define REG_SC_YAHAN_BK32_84_H      _PK_H_(0x32, 0x84)
#define REG_SC_YAHAN_BK32_85_L      _PK_L_(0x32, 0x85)
#define REG_SC_YAHAN_BK32_85_H      _PK_H_(0x32, 0x85)
#define REG_SC_YAHAN_BK32_86_L      _PK_L_(0x32, 0x86)
#define REG_SC_YAHAN_BK32_86_H      _PK_H_(0x32, 0x86)
#define REG_SC_YAHAN_BK32_87_L      _PK_L_(0x32, 0x87)
#define REG_SC_YAHAN_BK32_87_H      _PK_H_(0x32, 0x87)
#define REG_SC_YAHAN_BK32_88_L      _PK_L_(0x32, 0x88)
#define REG_SC_YAHAN_BK32_88_H      _PK_H_(0x32, 0x88)
#define REG_SC_YAHAN_BK32_89_L      _PK_L_(0x32, 0x89)
#define REG_SC_YAHAN_BK32_89_H      _PK_H_(0x32, 0x89)
#define REG_SC_YAHAN_BK32_8a_L      _PK_L_(0x32, 0x8a)
#define REG_SC_YAHAN_BK32_8a_H      _PK_H_(0x32, 0x8a)
#define REG_SC_YAHAN_BK32_8b_L      _PK_L_(0x32, 0x8b)
#define REG_SC_YAHAN_BK32_8b_H      _PK_H_(0x32, 0x8b)
#define REG_SC_YAHAN_BK32_8c_L      _PK_L_(0x32, 0x8c)
#define REG_SC_YAHAN_BK32_8c_H      _PK_H_(0x32, 0x8c)
#define REG_SC_YAHAN_BK32_8d_L      _PK_L_(0x32, 0x8d)
#define REG_SC_YAHAN_BK32_8d_H      _PK_H_(0x32, 0x8d)
#define REG_SC_YAHAN_BK32_8e_L      _PK_L_(0x32, 0x8e)
#define REG_SC_YAHAN_BK32_8e_H      _PK_H_(0x32, 0x8e)
#define REG_SC_YAHAN_BK32_8f_L      _PK_L_(0x32, 0x8f)
#define REG_SC_YAHAN_BK32_8f_H      _PK_H_(0x32, 0x8f)
#define REG_SC_YAHAN_BK32_90_L      _PK_L_(0x32, 0x90)
#define REG_SC_YAHAN_BK32_90_H      _PK_H_(0x32, 0x90)
#define REG_SC_YAHAN_BK32_91_L      _PK_L_(0x32, 0x91)
#define REG_SC_YAHAN_BK32_91_H      _PK_H_(0x32, 0x91)
#define REG_SC_YAHAN_BK32_92_L      _PK_L_(0x32, 0x92)
#define REG_SC_YAHAN_BK32_92_H      _PK_H_(0x32, 0x92)
#define REG_SC_YAHAN_BK32_93_L      _PK_L_(0x32, 0x93)
#define REG_SC_YAHAN_BK32_93_H      _PK_H_(0x32, 0x93)
#define REG_SC_YAHAN_BK32_94_L      _PK_L_(0x32, 0x94)
#define REG_SC_YAHAN_BK32_94_H      _PK_H_(0x32, 0x94)
#define REG_SC_YAHAN_BK32_95_L      _PK_L_(0x32, 0x95)
#define REG_SC_YAHAN_BK32_95_H      _PK_H_(0x32, 0x95)
#define REG_SC_YAHAN_BK32_96_L      _PK_L_(0x32, 0x96)
#define REG_SC_YAHAN_BK32_96_H      _PK_H_(0x32, 0x96)
#define REG_SC_YAHAN_BK32_97_L      _PK_L_(0x32, 0x97)
#define REG_SC_YAHAN_BK32_97_H      _PK_H_(0x32, 0x97)
#define REG_SC_YAHAN_BK32_98_L      _PK_L_(0x32, 0x98)
#define REG_SC_YAHAN_BK32_98_H      _PK_H_(0x32, 0x98)
#define REG_SC_YAHAN_BK32_99_L      _PK_L_(0x32, 0x99)
#define REG_SC_YAHAN_BK32_99_H      _PK_H_(0x32, 0x99)
#define REG_SC_YAHAN_BK32_9a_L      _PK_L_(0x32, 0x9a)
#define REG_SC_YAHAN_BK32_9a_H      _PK_H_(0x32, 0x9a)
#define REG_SC_YAHAN_BK32_9b_L      _PK_L_(0x32, 0x9b)
#define REG_SC_YAHAN_BK32_9b_H      _PK_H_(0x32, 0x9b)
#define REG_SC_YAHAN_BK32_9c_L      _PK_L_(0x32, 0x9c)
#define REG_SC_YAHAN_BK32_9c_H      _PK_H_(0x32, 0x9c)
#define REG_SC_YAHAN_BK32_9d_L      _PK_L_(0x32, 0x9d)
#define REG_SC_YAHAN_BK32_9d_H      _PK_H_(0x32, 0x9d)
#define REG_SC_YAHAN_BK32_9e_L      _PK_L_(0x32, 0x9e)
#define REG_SC_YAHAN_BK32_9e_H      _PK_H_(0x32, 0x9e)
#define REG_SC_YAHAN_BK32_9f_L      _PK_L_(0x32, 0x9f)
#define REG_SC_YAHAN_BK32_9f_H      _PK_H_(0x32, 0x9f)
#define REG_SC_YAHAN_BK32_a0_L      _PK_L_(0x32, 0xa0)
#define REG_SC_YAHAN_BK32_a0_H      _PK_H_(0x32, 0xa0)
#define REG_SC_YAHAN_BK32_a1_L      _PK_L_(0x32, 0xa1)
#define REG_SC_YAHAN_BK32_a1_H      _PK_H_(0x32, 0xa1)
#define REG_SC_YAHAN_BK32_a2_L      _PK_L_(0x32, 0xa2)
#define REG_SC_YAHAN_BK32_a2_H      _PK_H_(0x32, 0xa2)
#define REG_SC_YAHAN_BK32_a3_L      _PK_L_(0x32, 0xa3)
#define REG_SC_YAHAN_BK32_a3_H      _PK_H_(0x32, 0xa3)
#define REG_SC_YAHAN_BK32_a4_L      _PK_L_(0x32, 0xa4)
#define REG_SC_YAHAN_BK32_a4_H      _PK_H_(0x32, 0xa4)
#define REG_SC_YAHAN_BK32_a5_L      _PK_L_(0x32, 0xa5)
#define REG_SC_YAHAN_BK32_a5_H      _PK_H_(0x32, 0xa5)
#define REG_SC_YAHAN_BK32_a6_L      _PK_L_(0x32, 0xa6)
#define REG_SC_YAHAN_BK32_a6_H      _PK_H_(0x32, 0xa6)
#define REG_SC_YAHAN_BK32_a7_L      _PK_L_(0x32, 0xa7)
#define REG_SC_YAHAN_BK32_a7_H      _PK_H_(0x32, 0xa7)
#define REG_SC_YAHAN_BK32_a8_L      _PK_L_(0x32, 0xa8)
#define REG_SC_YAHAN_BK32_a8_H      _PK_H_(0x32, 0xa8)
#define REG_SC_YAHAN_BK32_a9_L      _PK_L_(0x32, 0xa9)
#define REG_SC_YAHAN_BK32_a9_H      _PK_H_(0x32, 0xa9)
#define REG_SC_YAHAN_BK32_aa_L      _PK_L_(0x32, 0xaa)
#define REG_SC_YAHAN_BK32_aa_H      _PK_H_(0x32, 0xaa)
#define REG_SC_YAHAN_BK32_ab_L      _PK_L_(0x32, 0xab)
#define REG_SC_YAHAN_BK32_ab_H      _PK_H_(0x32, 0xab)
#define REG_SC_YAHAN_BK32_ac_L      _PK_L_(0x32, 0xac)
#define REG_SC_YAHAN_BK32_ac_H      _PK_H_(0x32, 0xac)
#define REG_SC_YAHAN_BK32_ad_L      _PK_L_(0x32, 0xad)
#define REG_SC_YAHAN_BK32_ad_H      _PK_H_(0x32, 0xad)
#define REG_SC_YAHAN_BK32_ae_L      _PK_L_(0x32, 0xae)
#define REG_SC_YAHAN_BK32_ae_H      _PK_H_(0x32, 0xae)
#define REG_SC_YAHAN_BK32_af_L      _PK_L_(0x32, 0xaf)
#define REG_SC_YAHAN_BK32_af_H      _PK_H_(0x32, 0xaf)
#define REG_SC_YAHAN_BK32_b0_L      _PK_L_(0x32, 0xb0)
#define REG_SC_YAHAN_BK32_b0_H      _PK_H_(0x32, 0xb0)
#define REG_SC_YAHAN_BK32_b1_L      _PK_L_(0x32, 0xb1)
#define REG_SC_YAHAN_BK32_b1_H      _PK_H_(0x32, 0xb1)
#define REG_SC_YAHAN_BK32_b2_L      _PK_L_(0x32, 0xb2)
#define REG_SC_YAHAN_BK32_b2_H      _PK_H_(0x32, 0xb2)
#define REG_SC_YAHAN_BK32_b3_L      _PK_L_(0x32, 0xb3)
#define REG_SC_YAHAN_BK32_b3_H      _PK_H_(0x32, 0xb3)
#define REG_SC_YAHAN_BK32_b4_L      _PK_L_(0x32, 0xb4)
#define REG_SC_YAHAN_BK32_b4_H      _PK_H_(0x32, 0xb4)
#define REG_SC_YAHAN_BK32_b5_L      _PK_L_(0x32, 0xb5)
#define REG_SC_YAHAN_BK32_b5_H      _PK_H_(0x32, 0xb5)
#define REG_SC_YAHAN_BK32_b6_L      _PK_L_(0x32, 0xb6)
#define REG_SC_YAHAN_BK32_b6_H      _PK_H_(0x32, 0xb6)
#define REG_SC_YAHAN_BK32_b7_L      _PK_L_(0x32, 0xb7)
#define REG_SC_YAHAN_BK32_b7_H      _PK_H_(0x32, 0xb7)
#define REG_SC_YAHAN_BK32_b8_L      _PK_L_(0x32, 0xb8)
#define REG_SC_YAHAN_BK32_b8_H      _PK_H_(0x32, 0xb8)
#define REG_SC_YAHAN_BK32_b9_L      _PK_L_(0x32, 0xb9)
#define REG_SC_YAHAN_BK32_b9_H      _PK_H_(0x32, 0xb9)
#define REG_SC_YAHAN_BK32_ba_L      _PK_L_(0x32, 0xba)
#define REG_SC_YAHAN_BK32_ba_H      _PK_H_(0x32, 0xba)
#define REG_SC_YAHAN_BK32_bb_L      _PK_L_(0x32, 0xbb)
#define REG_SC_YAHAN_BK32_bb_H      _PK_H_(0x32, 0xbb)
#define REG_SC_YAHAN_BK32_bc_L      _PK_L_(0x32, 0xbc)
#define REG_SC_YAHAN_BK32_bc_H      _PK_H_(0x32, 0xbc)
#define REG_SC_YAHAN_BK32_bd_L      _PK_L_(0x32, 0xbd)
#define REG_SC_YAHAN_BK32_bd_H      _PK_H_(0x32, 0xbd)
#define REG_SC_YAHAN_BK32_be_L      _PK_L_(0x32, 0xbe)
#define REG_SC_YAHAN_BK32_be_H      _PK_H_(0x32, 0xbe)
#define REG_SC_YAHAN_BK32_bf_L      _PK_L_(0x32, 0xbf)
#define REG_SC_YAHAN_BK32_bf_H      _PK_H_(0x32, 0xbf)
#define REG_SC_YAHAN_BK32_c0_L      _PK_L_(0x32, 0xc0)
#define REG_SC_YAHAN_BK32_c0_H      _PK_H_(0x32, 0xc0)
#define REG_SC_YAHAN_BK32_c1_L      _PK_L_(0x32, 0xc1)
#define REG_SC_YAHAN_BK32_c1_H      _PK_H_(0x32, 0xc1)
#define REG_SC_YAHAN_BK32_c2_L      _PK_L_(0x32, 0xc2)
#define REG_SC_YAHAN_BK32_c2_H      _PK_H_(0x32, 0xc2)
#define REG_SC_YAHAN_BK32_c3_L      _PK_L_(0x32, 0xc3)
#define REG_SC_YAHAN_BK32_c3_H      _PK_H_(0x32, 0xc3)
#define REG_SC_YAHAN_BK32_c4_L      _PK_L_(0x32, 0xc4)
#define REG_SC_YAHAN_BK32_c4_H      _PK_H_(0x32, 0xc4)
#define REG_SC_YAHAN_BK32_c5_L      _PK_L_(0x32, 0xc5)
#define REG_SC_YAHAN_BK32_c5_H      _PK_H_(0x32, 0xc5)
#define REG_SC_YAHAN_BK32_c6_L      _PK_L_(0x32, 0xc6)
#define REG_SC_YAHAN_BK32_c6_H      _PK_H_(0x32, 0xc6)
#define REG_SC_YAHAN_BK32_c7_L      _PK_L_(0x32, 0xc7)
#define REG_SC_YAHAN_BK32_c7_H      _PK_H_(0x32, 0xc7)
#define REG_SC_YAHAN_BK32_c8_L      _PK_L_(0x32, 0xc8)
#define REG_SC_YAHAN_BK32_c8_H      _PK_H_(0x32, 0xc8)
#define REG_SC_YAHAN_BK32_c9_L      _PK_L_(0x32, 0xc9)
#define REG_SC_YAHAN_BK32_c9_H      _PK_H_(0x32, 0xc9)
#define REG_SC_YAHAN_BK32_ca_L      _PK_L_(0x32, 0xca)
#define REG_SC_YAHAN_BK32_ca_H      _PK_H_(0x32, 0xca)
#define REG_SC_YAHAN_BK32_cb_L      _PK_L_(0x32, 0xcb)
#define REG_SC_YAHAN_BK32_cb_H      _PK_H_(0x32, 0xcb)
#define REG_SC_YAHAN_BK32_cc_L      _PK_L_(0x32, 0xcc)
#define REG_SC_YAHAN_BK32_cc_H      _PK_H_(0x32, 0xcc)
#define REG_SC_YAHAN_BK32_cd_L      _PK_L_(0x32, 0xcd)
#define REG_SC_YAHAN_BK32_cd_H      _PK_H_(0x32, 0xcd)
#define REG_SC_YAHAN_BK32_ce_L      _PK_L_(0x32, 0xce)
#define REG_SC_YAHAN_BK32_ce_H      _PK_H_(0x32, 0xce)
#define REG_SC_YAHAN_BK32_cf_L      _PK_L_(0x32, 0xcf)
#define REG_SC_YAHAN_BK32_cf_H      _PK_H_(0x32, 0xcf)
#define REG_SC_YAHAN_BK32_d0_L      _PK_L_(0x32, 0xd0)
#define REG_SC_YAHAN_BK32_d0_H      _PK_H_(0x32, 0xd0)
#define REG_SC_YAHAN_BK32_d1_L      _PK_L_(0x32, 0xd1)
#define REG_SC_YAHAN_BK32_d1_H      _PK_H_(0x32, 0xd1)
#define REG_SC_YAHAN_BK32_d2_L      _PK_L_(0x32, 0xd2)
#define REG_SC_YAHAN_BK32_d2_H      _PK_H_(0x32, 0xd2)
#define REG_SC_YAHAN_BK32_d3_L      _PK_L_(0x32, 0xd3)
#define REG_SC_YAHAN_BK32_d3_H      _PK_H_(0x32, 0xd3)
#define REG_SC_YAHAN_BK32_d4_L      _PK_L_(0x32, 0xd4)
#define REG_SC_YAHAN_BK32_d4_H      _PK_H_(0x32, 0xd4)
#define REG_SC_YAHAN_BK32_d5_L      _PK_L_(0x32, 0xd5)
#define REG_SC_YAHAN_BK32_d5_H      _PK_H_(0x32, 0xd5)
#define REG_SC_YAHAN_BK32_d6_L      _PK_L_(0x32, 0xd6)
#define REG_SC_YAHAN_BK32_d6_H      _PK_H_(0x32, 0xd6)
#define REG_SC_YAHAN_BK32_d7_L      _PK_L_(0x32, 0xd7)
#define REG_SC_YAHAN_BK32_d7_H      _PK_H_(0x32, 0xd7)
#define REG_SC_YAHAN_BK32_d8_L      _PK_L_(0x32, 0xd8)
#define REG_SC_YAHAN_BK32_d8_H      _PK_H_(0x32, 0xd8)
#define REG_SC_YAHAN_BK32_d9_L      _PK_L_(0x32, 0xd9)
#define REG_SC_YAHAN_BK32_d9_H      _PK_H_(0x32, 0xd9)
#define REG_SC_YAHAN_BK32_da_L      _PK_L_(0x32, 0xda)
#define REG_SC_YAHAN_BK32_da_H      _PK_H_(0x32, 0xda)
#define REG_SC_YAHAN_BK32_db_L      _PK_L_(0x32, 0xdb)
#define REG_SC_YAHAN_BK32_db_H      _PK_H_(0x32, 0xdb)
#define REG_SC_YAHAN_BK32_dc_L      _PK_L_(0x32, 0xdc)
#define REG_SC_YAHAN_BK32_dc_H      _PK_H_(0x32, 0xdc)
#define REG_SC_YAHAN_BK32_dd_L      _PK_L_(0x32, 0xdd)
#define REG_SC_YAHAN_BK32_dd_H      _PK_H_(0x32, 0xdd)
#define REG_SC_YAHAN_BK32_de_L      _PK_L_(0x32, 0xde)
#define REG_SC_YAHAN_BK32_de_H      _PK_H_(0x32, 0xde)
#define REG_SC_YAHAN_BK32_df_L      _PK_L_(0x32, 0xdf)
#define REG_SC_YAHAN_BK32_df_H      _PK_H_(0x32, 0xdf)
#define REG_SC_YAHAN_BK32_e0_L      _PK_L_(0x32, 0xe0)
#define REG_SC_YAHAN_BK32_e0_H      _PK_H_(0x32, 0xe0)
#define REG_SC_YAHAN_BK32_e1_L      _PK_L_(0x32, 0xe1)
#define REG_SC_YAHAN_BK32_e1_H      _PK_H_(0x32, 0xe1)
#define REG_SC_YAHAN_BK32_e2_L      _PK_L_(0x32, 0xe2)
#define REG_SC_YAHAN_BK32_e2_H      _PK_H_(0x32, 0xe2)
#define REG_SC_YAHAN_BK32_e3_L      _PK_L_(0x32, 0xe3)
#define REG_SC_YAHAN_BK32_e3_H      _PK_H_(0x32, 0xe3)
#define REG_SC_YAHAN_BK32_e4_L      _PK_L_(0x32, 0xe4)
#define REG_SC_YAHAN_BK32_e4_H      _PK_H_(0x32, 0xe4)
#define REG_SC_YAHAN_BK32_e5_L      _PK_L_(0x32, 0xe5)
#define REG_SC_YAHAN_BK32_e5_H      _PK_H_(0x32, 0xe5)
#define REG_SC_YAHAN_BK32_e6_L      _PK_L_(0x32, 0xe6)
#define REG_SC_YAHAN_BK32_e6_H      _PK_H_(0x32, 0xe6)
#define REG_SC_YAHAN_BK32_e7_L      _PK_L_(0x32, 0xe7)
#define REG_SC_YAHAN_BK32_e7_H      _PK_H_(0x32, 0xe7)
#define REG_SC_YAHAN_BK32_e8_L      _PK_L_(0x32, 0xe8)
#define REG_SC_YAHAN_BK32_e8_H      _PK_H_(0x32, 0xe8)
#define REG_SC_YAHAN_BK32_e9_L      _PK_L_(0x32, 0xe9)
#define REG_SC_YAHAN_BK32_e9_H      _PK_H_(0x32, 0xe9)
#define REG_SC_YAHAN_BK32_ea_L      _PK_L_(0x32, 0xea)
#define REG_SC_YAHAN_BK32_ea_H      _PK_H_(0x32, 0xea)
#define REG_SC_YAHAN_BK32_eb_L      _PK_L_(0x32, 0xeb)
#define REG_SC_YAHAN_BK32_eb_H      _PK_H_(0x32, 0xeb)
#define REG_SC_YAHAN_BK32_ec_L      _PK_L_(0x32, 0xec)
#define REG_SC_YAHAN_BK32_ec_H      _PK_H_(0x32, 0xec)
#define REG_SC_YAHAN_BK32_ed_L      _PK_L_(0x32, 0xed)
#define REG_SC_YAHAN_BK32_ed_H      _PK_H_(0x32, 0xed)
#define REG_SC_YAHAN_BK32_ee_L      _PK_L_(0x32, 0xee)
#define REG_SC_YAHAN_BK32_ee_H      _PK_H_(0x32, 0xee)
#define REG_SC_YAHAN_BK32_ef_L      _PK_L_(0x32, 0xef)
#define REG_SC_YAHAN_BK32_ef_H      _PK_H_(0x32, 0xef)
#define REG_SC_YAHAN_BK32_f0_L      _PK_L_(0x32, 0xf0)
#define REG_SC_YAHAN_BK32_f0_H      _PK_H_(0x32, 0xf0)
#define REG_SC_YAHAN_BK32_f1_L      _PK_L_(0x32, 0xf1)
#define REG_SC_YAHAN_BK32_f1_H      _PK_H_(0x32, 0xf1)
#define REG_SC_YAHAN_BK32_f2_L      _PK_L_(0x32, 0xf2)
#define REG_SC_YAHAN_BK32_f2_H      _PK_H_(0x32, 0xf2)
#define REG_SC_YAHAN_BK32_f3_L      _PK_L_(0x32, 0xf3)
#define REG_SC_YAHAN_BK32_f3_H      _PK_H_(0x32, 0xf3)
#define REG_SC_YAHAN_BK32_f4_L      _PK_L_(0x32, 0xf4)
#define REG_SC_YAHAN_BK32_f4_H      _PK_H_(0x32, 0xf4)
#define REG_SC_YAHAN_BK32_f5_L      _PK_L_(0x32, 0xf5)
#define REG_SC_YAHAN_BK32_f5_H      _PK_H_(0x32, 0xf5)
#define REG_SC_YAHAN_BK32_f6_L      _PK_L_(0x32, 0xf6)
#define REG_SC_YAHAN_BK32_f6_H      _PK_H_(0x32, 0xf6)
#define REG_SC_YAHAN_BK32_f7_L      _PK_L_(0x32, 0xf7)
#define REG_SC_YAHAN_BK32_f7_H      _PK_H_(0x32, 0xf7)
#define REG_SC_YAHAN_BK32_f8_L      _PK_L_(0x32, 0xf8)
#define REG_SC_YAHAN_BK32_f8_H      _PK_H_(0x32, 0xf8)
#define REG_SC_YAHAN_BK32_f9_L      _PK_L_(0x32, 0xf9)
#define REG_SC_YAHAN_BK32_f9_H      _PK_H_(0x32, 0xf9)
#define REG_SC_YAHAN_BK32_fa_L      _PK_L_(0x32, 0xfa)
#define REG_SC_YAHAN_BK32_fa_H      _PK_H_(0x32, 0xfa)
#define REG_SC_YAHAN_BK32_fb_L      _PK_L_(0x32, 0xfb)
#define REG_SC_YAHAN_BK32_fb_H      _PK_H_(0x32, 0xfb)
#define REG_SC_YAHAN_BK32_fc_L      _PK_L_(0x32, 0xfc)
#define REG_SC_YAHAN_BK32_fc_H      _PK_H_(0x32, 0xfc)
#define REG_SC_YAHAN_BK32_fd_L      _PK_L_(0x32, 0xfd)
#define REG_SC_YAHAN_BK32_fd_H      _PK_H_(0x32, 0xfd)
#define REG_SC_YAHAN_BK32_fe_L      _PK_L_(0x32, 0xfe)
#define REG_SC_YAHAN_BK32_fe_H      _PK_H_(0x32, 0xfe)
#endif

#endif //_HAL_COLOR_FORMAT_DRIVER_HARDWARE_H
