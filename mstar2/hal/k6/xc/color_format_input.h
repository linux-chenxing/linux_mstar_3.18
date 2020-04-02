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
/// @file   color_format_input.h
/// @brief  MStar XC Driver DDI HAL Level
/// @author MStar Semiconductor Inc.
/// @attention
/// <b>(OBSOLETED) <em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_COLOR_FORMAT_INPUT_H
#define _HAL_COLOR_FORMAT_INPUT_H
#include "color_format_driver.h"
#include "color_format_driver_hardware.h"
#ifdef _HAL_COLOR_FORMAT_INPUT_C
#define INTERFACE
#else
#define INTERFACE                               extern
#endif

#define CFD_debug 0

#define CFD_MAININOPUT_DummyRegs_Debug 0
#define CFD_OSD_DummyRegs_Debug 0

#define RealChip 1

#define  HDR_DBG_HAL(x)  //x
#define  HDR_DBG_HAL_CFD(x)  //x
//Version definition of structure +++++++++++++++++++++++++++++++++++++++++++++

//0 : not assume
//1 : assume
#define CFD_AssumeHDRTVhasBT2020EDID 1

#if defined _WIN32
#define CFD_CModel_VERSION 0x0000
#endif

#define MaskForMode_LB 0x3f
#define MaskForMode_HB 0xC0
#define CFD_SW_VERSION 0x02070000

// 0 : disable
// 1 : enable
#define CFD_USE_TESTVECTOR 0

#define CFD_TESTVECTORS_VERSION 0x0000
#define CFD_MAIN_CONTROL_ST_VERSION 3
#define CFD_MM_ST_VERSION 1
#define CFD_HDMI_INFOFRAME_ST_VERSION 0
#define CFD_HDMI_INFOFRAME_OUT_ST_VERSION 0
#define CFD_HDMI_EDID_ST_VERSION 1

#define CFD_OSD_PROCESS_ST_VERSION 1

#define CFD_ENUM_VERSION 2

#define CFD_OSD_ST_VERSION 1
#define CFD_PANEL_ST_VERSION 0
#define CFD_HDR_METADATA_VERSION 0
#define CFD_TMO_ST_VERSION 0

#define CFD_SW_RETURN_ST_VERSION 1

#define CFD_CURRY_DLCIP_ST_VERSION 0
#define CFD_CURRY_TMOIP_ST_VERSION 0
#define CFD_CURRY_HDRIP_ST_VERSION 0
#define CFD_CURRY_SDRIP_ST_VERSION 0

#define CFD_TV 0
#define CFD_STB 1

#define CFD_PRODUCT CFD_STB

//0:off
//1:1
//default is 0
//should use as u8TMO_TargetRefer_Mode = 2
#define DePQClamp_EN 1

//unit in nits
//default is 0
#define DePQClampNOffset 0

//unit in nits
//default is 800
#define DePQClampMin 800

//-----------------------------------------------------------------------------


/*
//for color primaries and white point defined in colorimetry
typedef struct _STU_CFD_COLORIMETRY
{
    //order R->G->B
    MS_U16 u16Display_Primaries_x[3];      //data *0.00002 0xC350 = 1
    MS_U16 u16Display_Primaries_y[3];      //data *0.00002 0xC350 = 1
    MS_U16 u16White_point_x;               //data *0.00002 0xC350 = 1
    MS_U16 u16White_point_y;               //data *0.00002 0xC350 = 1

} STU_CFD_COLORIMETRY;
*/

//no specific content , wait for usage
typedef struct _STU_CFDAPI_DOLBY_HDR_METADATA_FORMAT
{

    MS_U8 u8IsDolbyHDREn;

} STU_CFDAPI_DOLBY_HDR_METADATA_FORMAT;

//no specific content , wait for usage
typedef struct _STU_CFDAPI_HDR_METADATA_FORMAT
{
    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_HDMI_HDR_METADATA_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_HDR_METADATA_FORMAT)

    STU_CFDAPI_DOLBY_HDR_METADATA_FORMAT stu_Cfd_Dolby_HDR_Param;

} STU_CFDAPI_HDR_METADATA_FORMAT;

//for information from panel
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

//controls from Panel OSD
typedef struct _STU_CFDAPI_OSD_CONTROL
{
    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_HDMI_OSD_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_OSD_CONTROL)

    MS_U16 u16Hue;
    MS_U16 u16Saturation;
    MS_U16 u16Contrast;

    //0:off
    //1:on
    //default on , not in the document
    MS_U8  u8OSD_UI_En;

    //Mode 0: update matrix by OSD and color format driver
    //Mode 1: only update matrix by OSD controls
    //for mode1 : the configures of matrix keep the same as the values by calling CFD last time
    MS_U8  u8OSD_UI_Mode;

    //0:auto depends on STB rule
    //1:always do HDR2SDR for HDR input
    //2:always not do HDR2SDR for HDR input
    MS_U8  u8HDR_UI_H2SMode;
} STU_CFDAPI_OSD_CONTROL;

//controls from Panel OSD
typedef struct
{
    MS_S16 s16ColorCorrectionMatrix[3][3];
    MS_S16 s16YVUtoRGBMatrix[3][3];
    MS_U16 u16R;
    MS_U16 u16G;
    MS_U16 u16B;
} STU_CFDAPI_ACE_CONTROL;

//purpose : allow user to control color

//format controls for a control point of CFD
typedef struct _STU_CFDAPI_MAIN_CONTROL_FORMAT
{

    //0:Like HDMI case, for this point, CFD only depends on u8Curr_Format
    //1:Like MM case, u8Curr_Format is not used,
    //u8Curr_Colour_primaries,u8Curr_Transfer_Characteristics,and u8Curr_Matrix_Coeffs
    //are used to determine u8Curr_Format

    MS_U8 u8Mid_Format_Mode;

    //E_CFD_CFIO
    MS_U8 u8Mid_Format;

    //E_CFD_MC_FORMAT
    MS_U8 u8Mid_DataFormat;

    //E_CFD_CFIO_RANGE
    MS_U8 u8Mid_IsFullRange;

    //E_CFIO_HDR_STATUS
    MS_U8 u8Mid_HDRMode;

    //assign by E_CFD_CFIO_CP
    MS_U8 u8Mid_Colour_primaries;

    //assign by E_CFD_CFIO_TR
    MS_U8 u8Mid_Transfer_Characteristics;

    //assign by E_CFD_CFIO_MC
    MS_U8 u8Mid_Matrix_Coeffs;

} STU_CFDAPI_MAIN_CONTROL_FORMAT;


typedef struct _STU_CFDAPI_SEAMLESS
{

    //0: off
    //1: on
    MS_U8 u8seamless_en;

    //0: auto
    //1: based on the value of u8seamless_colorspace;
    MS_U8 u8seamless_colorspace_mode;

    //assign by E_CFD_SL_CS
    MS_U8 u8seamless_colorspace;

    //0: auto
    //1: based on the value of u8seamless_hdrmode;
    MS_U8 u8seamless_hdrmode_mode;

    //agssign by
    MS_U8 u8seamless_hdrmode;

} STU_CFDAPI_SEAMLESS;

//main controls from user for color format driver
typedef struct _STU_CFDAPI_MAIN_CONTROL
{

    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_MAIN_CONTROL_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFD_MAIN_CONTROL)

    //EMU:E_CFD_MC_HW_STRUCTURE
    //define the HW stucture use this function
    //MS_U8 u8HW_Structure;

    //force 1 now
    //only for u8HW_Structure = E_CFD_HWS_STB_TYPE1
    //MS_U8 u8HW_PatchEn;

    //E_CFD_MC_SOURCE
    //specify which input source
    MS_U8 u8Input_Source;

    //E_CFD_INPUT_ANALOG_FORMAT
    //MS_U8 u8Input_AnalogIdx;

    //E_CFD_CFIO
    MS_U8 u8Input_Format;

    //E_CFD_MC_FORMAT
    //specify RGB/YUV format of the input of the first HDR/SDR IP
    //E_CFD_MC_FORMAT_RGB       = 0x00,
    //E_CFD_MC_FORMAT_YUV422    = 0x01,
    //E_CFD_MC_FORMAT_YUV444    = 0x02,
    //E_CFD_MC_FORMAT_YUV420    = 0x03,
    MS_U8 u8Input_DataFormat;

    //limit/full
    //assign with E_CFD_CFIO_RANGE
    //0:limit 1:full
    MS_U8 u8Input_IsFullRange;

    //SDR/HDR
    //E_CFIO_HDR_STATUS
    //0:SDR
    //1:HDR1
    //2:HDR2
    MS_U8 u8Input_HDRMode;

    //only for information of Analog input
    //assign by E_CFD_CFIO_CP
    MS_U8 u8Input_ext_Colour_primaries;
    MS_U8 u8Input_ext_Transfer_Characteristics;
    MS_U8 u8Input_ext_Matrix_Coeffs;

    //control from Qmap
    //0:RGB not bypass CSC (not R2Y)
    //1:RGB bypass CSC (force R2Y)
    MS_U8 u8Input_IsRGBBypass;

    //special control
    //especially for Maserati
    //0: bypass all SDR IP
    //1: SDR IP by decision tree or user
    //2: bypass all SDR IP besides the last OutputCSC
    //With this value, Each IC will has its own control decision tree.
    MS_U8 u8Input_SDRIPMode;

    //0: bypass all HDR IP
    //1: HDR IP by decision tree or user
    //2: set HDR IP to open HDR
    //3: set HDR IP to Dolby HDR
    MS_U8 u8Input_HDRIPMode;



    STU_CFDAPI_MAIN_CONTROL_FORMAT stu_Middle_Format[1];


#if 0
    //0: use decision tree to configure
    //1: set dolby mode or dolby function for all HDR IP
    //MS_U8 u8Input_IsHDRIPFromDolbyDriver;

    //E_CFD_CFIO
    //Temp_Format[1] : output of HDR IP, input of SDR IP
    MS_U8 u8Temp_Format;

    //E_CFD_MC_FORMAT
    MS_U8 u8Temp_DataFormat;

    //E_CFD_CFIO_RANGE
    MS_U8 u8Temp_IsFullRange;

    //E_CFIO_HDR_STATUS
    MS_U8 u8Temp_HDRMode;

    //0:not force
    //1:force CP/TR/MC for temp_format in CFD kernel
    MS_U8 u8ForceTempFormatDetails_Flag;

    //assign by E_CFD_CFIO_CP
    MS_U8 u8ForceTemp_Colour_primaries;

    //assign by E_CFD_CFIO_TR
    MS_U8 u8ForceTemp_Transfer_Characteristics;

    //assign by E_CFD_CFIO_MC
    MS_U8 u8ForceTemp_Matrix_Coeffs;
#endif

    //specify RGB/YUV format of the output of the last HDR/SDR IP
    //E_CFD_MC_SOURCE
    MS_U8 u8Output_Source;

    //E_CFD_CFIO
    MS_U8 u8Output_Format;

    //E_CFD_MC_FORMAT
    MS_U8 u8Output_DataFormat;

    //0:limit 1:full
    MS_U8 u8Output_IsFullRange;

    //E_CFIO_HDR_STATUS
    //0:SDR
    //1:HDR1
    //2:HDR2
    MS_U8 u8Output_HDRMode;


    //only for HDMI out ==================================================
    //find a better one  for HDMI out, although user assigns a specific output colormetry
    //mode 0:no GM happens , output format is the same as input format
    //mode 1:output colorimetry is based on the information from u8HDMISink_Extended_Colorspace,which is parsed from  EDID
    //mode 2:output colorimetry is assigned to 709 directly
    //mode 3:output colorimetry is assigned to X , X refers to the midformat/output format set by user
    MS_U8 u8HDMIOutput_GammutMapping_Mode;

    //when sink support multiple colormetries, find out a new one.
    //MS_U8 u9HDMIOutput_GammutMapping_Mode;
    //mode 0: when input colorimetry and requested output colorimetry are not same , force gamut extension, like BT709 to BT2020
    //mode 1: when input colorimetry and requested output colorimetry are not same , force gamut compression, like BT2020 to BT709
    MS_U8 u8HDMIOutput_GammutMapping_MethodMode;

    //only for MM input =================================================
    //please force 1 for current status
    //mode 0:off
    //mode 1:Match MM to HDMI format ;  if not matched , Force 709
    //mode 2:Force 709
    //mode 3:Match MM to HDMI format ;  if not matched , output directly
    //MS_U8 u8Force_InputColoriMetryRemapping;
    MS_U8 u8MMInput_ColorimetryHandle_Mode;

    //only for Panel output =============================================
    //mode0:off
    //mode1:do GamutMapping to Panel
    MS_U8 u8PanelOutput_GammutMapping_Mode;
    //====================================================================


    //============================================================================

    //0:use default value
    //use u16Source_Max_Luminance,u16Source_Min_Luminance,u16Source_Mean_Luminance

    //1:use Panel/HDMI EDID infor for target Luminance range
    //if HDR EDID does not exist , use default values as mode 0

    //2:auto mode, refer to u8TMO_SourceUserModeEn ,u8TMO_SourceUserMode
    //             refer to u8TMO_TargetUserModeEn ,u8TMO_TargetUserMode

    MS_U8 u8TMO_TargetRefer_Mode;

    //============================================================================

    //for TMO version 0
    //range 1 nits to 10000 nits
    MS_U16 u16Source_Max_Luminance;    //data * 1 nits
    MS_U16 u16Source_Med_Luminance;    //data * 1 nits

    //range 1e-4 nits to 6.55535 nits
    MS_U16 u16Source_Min_Luminance;    //data * 0.0001 nits

    MS_U16 u16Target_Med_Luminance;    //data * 1 nits

    //for TMO version 0 and 1
    //range 1 nits to 10000 nits
    MS_U16 u16Target_Max_Luminance;    //data * 1 nits

    //range 1e-4 nits to 6.55535 nits
    MS_U16 u16Target_Min_Luminance;    //data * 0.0001 nits

    //report Process
    //bit0 DoDLCflag
    //bit1 DoGamutMappingflag
    //bit2 DoTMOflag

    MS_U8 u8Process_Status;
    MS_U8 u8Process_Status2;
    MS_U8 u8Process_Status3;

    //Process Mode
    //0: color format driver on - auto
    //CFD process depends on the input of CFD
    //1: shows SDR UI, UI 709, no video

    MS_U8 u8PredefinedProcess;

    //for TMO parameter control method extension
    MS_U8 u8TMO_SourceUserModeEn; //0: auto
                                  //priority : infoFrame > driver > CFD default
                                  //1: force, refer to u8SourceUserMode
    //write
    MS_U8 u8TMO_SourceUserMode;   //0: refer to infoFrame
                                  //1: refer to driver
                                  //2: refer to CFD default
    //read
    MS_U8 u8TMO_SourceStatus;     //0: refer to infoFrame
                                  //1: refer to driver
                                  //2: refer to CFD default

    MS_U8 u8TMO_TargetUserModeEn; //0: auto
                                  //priority : EDID > driver > CFD default
                                  //1: force, refer to u8SourceUserMode
    //write
    MS_U8 u8TMO_TargetUserMode;   //0: refer to EDID
                                  //1: refer to driver
                                  //2: refer to CFD default

    //read
    MS_U8 u8TMO_TargetStatus;     //0: refer to infoFrame
                                  //1: refer to driver
                                  //2: refer to CFD default


    //for seamless control of HDMI
    //only useful when u8Output_Source is HDMI
    STU_CFDAPI_SEAMLESS stu_seamless;


} STU_CFDAPI_MAIN_CONTROL;


//follow the definition in HEVC spec
//information from MM
typedef struct _STU_CFDAPI_MM_PARSER
{

    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_MM_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_MM_PARSER)


    //MS_U8 u8MM_Codec;
    //1:HEVC 0:others

    //sMS_U8 u8ColorDescription_Valid;
    //assign by E_CFD_VALIDORNOT
    //0 :Not valid
    //1 :valid

    //assign by E_CFD_CFIO_CP
    MS_U8 u8Colour_primaries;
    //0:Reserverd
    //1:BT. 709/sRGB/sYCC
    //2:unspecified
    //3:Reserverd
    //4:BT. 470-6
    //5:BT. 601_625/PAL/SECAM
    //6:BT. 601_525/NTSC/SMPTE_170M
    //7:SMPTE_240M
    //8:Generic film
    //9:BT. 2020
    //10:CIEXYZ
    //11-255:Reserverd

    MS_U8 u8Transfer_Characteristics;
    //assign by E_CFD_CFIO_TR

    //0 Reserverd
    //1 BT. 709
    //2 unspecified
    //3 Reserverd
    //4 Assume display gamma 2.2
    //5 Assume display gamma 2.8
    //6 BT. 601_525/BT. 601_525
    //7 SMPTE_240M
    //8 linear
    //9 Logarithmic (100:1 range)
    //10    Logarithmic (100*sqrt(10):1 range)
    //11    xvYCC
    //12    BT. 1361 extend color gamut system
    //13    sRGB/sYCC
    //14    BT. 2020
    //15    BT. 2020
    //16    SMPTE ST2084 for 10.12.14.16-bit systetm
    //17    SMPTE ST428-1
    //18-255:Reserverd

    MS_U8 u8Matrix_Coeffs;
    //assign by E_CFD_CFIO_MC
    //0 Identity
    //1 BT. 709/xvYCC709
    //2 unspecified
    //3 Reserverd
    //4 USFCCT 47
    //5 BT. 601_625/PAL/SECAM/xvYCC601/sYCC
    //6 BT. 601_525/NTSC/SMPTE_170M
    //7 SMPTE_240M
    //8 YCgCo
    //9 BT. 2020NCL(non-constant luminance)
    //10    BT. 2020CL(constant luminance)
    //11-255:Reserverd

    MS_U8 u8Video_Full_Range_Flag;
    //assign by E_CFD_CFIO_RANGE
    //0:limit range
    //1:full range

    MS_U32 u32Master_Panel_Max_Luminance;    //data * 0.0001 nits
    MS_U32 u32Master_Panel_Min_Luminance;    //data * 0.0001 nits
    //MS_U16 u16Max_Content_Light_Level;       //data * 1 nits
    //MS_U16 u16Max_Frame_Avg_Light_Level;     //data * 1 nits


    STU_CFD_COLORIMETRY stu_Cfd_MM_MasterPanel_ColorMetry;

    MS_U8  u8Mastering_Display_Infor_Valid;  //1:valid

    //based on w15138 JCT-VC document
    //update API for handle of content light level information SEI

    //if these values are equal to zero, means these values are not avaliable
    MS_U8  u8MM_HDR_ContentLightMetaData_Valid;      // 1:valid 0:not valid
    MS_U16 u16Max_content_light_level;    //data * 1 nits
    MS_U16 u16Max_pic_average_light_level;    //data * 1 nits



} STU_CFDAPI_MM_PARSER;

//information from HDMI EDID
typedef struct _STU_CFDAPI_HDMI_EDID_PARSER
{

    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_HDMI_EDID_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_HDMI_EDID_PARSER)

    MS_U8 u8HDMISink_HDRData_Block_Valid;
    //assign by E_CFD_VALIDORNOT
    //0 :Not valid
    //1 :valid

    MS_U8 u8HDMISink_EOTF;
    //byte 3 in HDR static Metadata Data block

    MS_U8 u8HDMISink_SM;
    //byte 4 in HDR static Metadata Data block

    MS_U8 u8HDMISink_Desired_Content_Max_Luminance;           //need a LUT to transfer
    MS_U8 u8HDMISink_Desired_Content_Max_Frame_Avg_Luminance; //need a LUT to transfer
    MS_U8 u8HDMISink_Desired_Content_Min_Luminance;           //need a LUT to transfer
    //byte 5 ~ 7 in HDR static Metadata Data block

    MS_U8 u8HDMISink_HDRData_Block_Length;
    //byte 1[4:0] in HDR static Metadata Data block

    //order R->G->B
    //MS_U16 u16display_primaries_x[3];                       //data *1/1024 0x03FF = 0.999
    //MS_U16 u16display_primaries_y[3];                       //data *1/1024 0x03FF = 0.999
    //MS_U16 u16white_point_x;                                //data *1/1024 0x03FF = 0.999
    //MS_U16 u16white_point_y;                                //data *1/1024 0x03FF = 0.999
    STU_CFD_COLORIMETRY stu_Cfd_HDMISink_Panel_ColorMetry;
    //address 0x19h to 22h in base EDID

    MS_U8 u8HDMISink_EDID_base_block_version;                //for debug
    //address 0x12h in EDID base block

    MS_U8 u8HDMISink_EDID_base_block_reversion;              //for debug
    //address 0x13h in EDID base block

    MS_U8 u8HDMISink_EDID_CEA_block_reversion;               //for debug
    //address 0x01h in EDID CEA block

    //table 59 Video Capability Data Block (VCDB)
    //0:VCDB is not avaliable
    //1:VCDB is avaliable
    MS_U8 u8HDMISink_VCDB_Valid;

    MS_U8 u8HDMISink_Support_YUVFormat;
    //bit 0:Support_YUV444
    //bit 1:Support_YUV422
    //bit 2:Support_YUV420

    //QY in Byte#3 in table 59 Video Capability Data Block (VCDB)
    //bit 3:RGB_quantization_range

    //QS in Byte#3 in table 59 Video Capability Data Block (VCDB)
    //bit 4:Y_quantization_range 0:no data(due to CE or IT video) ; 1:selectable


    MS_U8 u8HDMISink_Extended_Colorspace;
    //byte 3 of Colorimetry Data Block
    //bit 0:xvYCC601
    //bit 1:xvYCC709
    //bit 2:sYCC601
    //bit 3:Adobeycc601
    //bit 4:Adobergb
    //bit 5:BT2020 cl
    //bit 6:BT2020 ncl
    //bit 7:BT2020 RGB

    MS_U8 u8HDMISink_EDID_Valid;
    //assign by E_CFD_VALIDORNOT
    //0 :Not valid
    //1 :valid

    //byte 4 of Colorimetry Data Block
    MS_U8 u8HDMISink_Extended_Colorspace_byte4;
    //bit 6 - 0 : reserved
    //bit 7:DCIP3

} STU_CFDAPI_HDMI_EDID_PARSER;

//information from HDMI InfoFrame
typedef struct _STU_CFDAPI_HDMI_INFOFRAME_PARSER
{

    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_HDMI_INFOFRAME_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_HDMI_INFOFRAME_PARSER)

    //for debug
    MS_U8 u8HDMISource_HDR_InfoFrame_Valid;
    //assign by E_CFD_VALIDORNOT
    //0 :Not valid
    //1 :valid

    //for debug
    MS_U8 u8HDMISource_EOTF;
    //assign by E_CFD_HDMI_HDR_INFOFRAME_EOTF
    //Data byte 1 in Dynamic range and mastering infoFrame
    //E_CFD_HDMI_HDR_INFOFRAME_EOTF
    //E_CFD_HDMI_EOTF_SDR_GAMMA     = 0x0,
    //E_CFD_HDMI_EOTF_HDR_GAMMA     = 0x1,
    //E_CFD_HDMI_EOTF_SMPTE2084     = 0x2,
    //E_CFD_HDMI_EOTF_FUTURE_EOTF       = 0x3,
    //E_CFD_HDMI_EOTF_RESERVED      = 0x4
    //8-255 is reserved

    //for debug
    //Static Metadata Descriptor ID in CEA861.3
    MS_U8 u8HDMISource_SMD_ID;
    //Data byte 2 in Dynamic range and mastering infoFrame
    //8-255 is reserved

    //from Static_Metadata_Descriptor
    MS_U16 u16Master_Panel_Max_Luminance;    //data * 1 nits
    MS_U16 u16Master_Panel_Min_Luminance;    //data * 0.0001 nits
    MS_U16 u16Max_Content_Light_Level;       //data * 1 nits
    MS_U16 u16Max_Frame_Avg_Light_Level;     //data * 1 nits

    STU_CFD_COLORIMETRY stu_Cfd_HDMISource_MasterPanel_ColorMetry;

    MS_U8  u8Mastering_Display_Infor_Valid;
    //assign by E_CFD_VALIDORNOT
    //0 :Not valid
    //1 :valid

    //for debug
    MS_U8 u8HDMISource_Support_Format;
    //[2:0] = {Y2 Y1 Y0}
    //[4:3] = {YQ1 YQ0}
    //[6:5] = {Q1 Q0}
    //information in AVI infoFrame

    //for debug
    MS_U8 u8HDMISource_Colorspace;
    //assign by E_CFD_CFIO
    //0 = RGB
    //1 = BT601_625
    //...
    //255 = undefined/reserved

    //MS_U8 u8HDMISource_AVIInfoFrame_Valid;
    //assign by E_CFD_VALIDORNOT
    //0 :Not valid
    //1 :valid

} STU_CFDAPI_HDMI_INFOFRAME_PARSER;

//return status for driver
typedef struct _STU_CFDAPI_HAL_SW_RETURN
{
    MS_U32 u32Version;
    MS_U16 u16Length;

    //0: Video path needs to do autodownload
    //1: Video path does not do autodownload
    //2: reserved
    MS_U8 u8VideoADFlag;

    //0: OSD path needs to do autodownload
    //1: OSD path does not do autodownload
    //2: reserved
    MS_U8 u8OSDADFlag;

} STU_CFDAPI_HAL_SW_RETURN;

//information from HDMI InfoFrame
typedef struct _STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT
{

    MS_U32 u32Version;   ///<Version of current structure. Please always set to "CFD_HDMI_INFOFRAME_ST_VERSION" as input
    MS_U16 u16Length;    ///<Length of this structure, u16Length=sizeof(STU_CFDAPI_HDMI_INFOFRAME_PARSER)

    //for debug
    MS_U8 u8HDMISource_HDR_InfoFrame_Valid;
    //assign by E_CFD_VALIDORNOT
    //0:Not valid
    //1:valid

    //for debug
    MS_U8 u8HDMISource_EOTF;
    //assign by E_CFD_HDMI_HDR_INFOFRAME_EOTF
    //Data byte 1 in Dynamic range and mastering infoFrame
    //E_CFD_HDMI_HDR_INFOFRAME_EOTF
    //E_CFD_HDMI_EOTF_SDR_GAMMA     = 0x0,
    //E_CFD_HDMI_EOTF_HDR_GAMMA     = 0x1,
    //E_CFD_HDMI_EOTF_SMPTE2084     = 0x2,
    //E_CFD_HDMI_EOTF_FUTURE_EOTF       = 0x3,
    //E_CFD_HDMI_EOTF_RESERVED      = 0x4
    //8-255 is reserved

    //for debug
    MS_U8 u8HDMISource_SMD_ID;
    //Data byte 2 in Dynamic range and mastering infoFrame
    //8-255 is reserved

    //from Static_Metadata_Descriptor
    MS_U16 u16Master_Panel_Max_Luminance;    //data * 1 nits
    MS_U16 u16Master_Panel_Min_Luminance;    //data * 0.0001 nits
    MS_U16 u16Max_Content_Light_Level;       //data * 1 nits
    MS_U16 u16Max_Frame_Avg_Light_Level;     //data * 1 nits

    STU_CFD_COLORIMETRY stu_Cfd_HDMISource_MasterPanel_ColorMetry;

    MS_U8  u8Mastering_Display_Infor_Valid;
    //assign by E_CFD_VALIDORNOT
    //0:Not valid
    //1:valid

    //for debug
    MS_U8 u8HDMISource_Support_Format;
    //[2:0] = {Y2 Y1 Y0}
    //[4:3] = {YQ1 YQ0}
    //[6:5] = {Q1 Q0}
    //information in AVI infoFrame

    //for debug
    MS_U8 u8HDMISource_Colorspace;
    //assign by E_CFD_CFIO
    //0 = RGB
    //1 = BT601_625
    //...
    //255 = undefined/reserved

    //MS_U8 u8HDMISource_AVIInfoFrame_Valid;
    //assign by E_CFD_VALIDORNOT
    //0:Not valid
    //1:valid

    MS_U8 u8Output_DataFormat;

} STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT;


//the control of OSD process in mali in Curry & Kano
typedef struct _STU_CFDAPI_OSD_PROCESS_CONFIGS
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

} STU_CFDAPI_OSD_PROCESS_CONFIGS;


//information from HDMI InfoFrame
typedef struct _STU_CFDAPI_HAL_CONTROL
{

    //0: SW does not need to do fire
    //1: SW should to fire
    //2: reserved
    MS_U8 u8AutodownloadFirePermission;

} STU_CFDAPI_HAL_CONTROL;

typedef struct _STU_CFDAPI_DOLBY_CONTROL
{
    MS_U8* pu8CompMetadta;
    MS_U8* pu8DmMetadta;
    void* pCompRegtable; //MsHdr_Comp_Regtable_t
    void* pCompConfig; //DoVi_Comp_ExtConfig_t
    void* pDmRegtable; //MsHdr_RegTable_t
    void* pDmConfig; //DoVi_Config_t
    void* pDmMds; //DoVi_MdsExt_t

} STU_CFDAPI_DOLBY_CONTROL;

typedef struct _STU_CFDAPI_HW_IPS
{
    //Main sub control mode
    MS_U8 u8HW_MainSub_Mode;
    //0: current control is for SC0 (Main)
    //1: current control is for SC1 (Sub)

    //2-255 is reversed

    STU_CFD_MS_ALG_INTERFACE_DLC *pstu_DLC_Input;
    STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input;
    STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Input;
    STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Input;

//#if (NowHW == Kastor)
//   STU_CFD_MS_ALG_INTERFACE_OSDIP *pstu_OSDIP_Input;
//#endif

} STU_CFDAPI_HW_IPS;

typedef struct _STU_CFDAPI_Custom
{
    //0:off, by default setting
    //1:on
    MS_U8 u8ForceOutputBT2020en;

} STU_CFDAPI_Custom;

typedef struct _STU_CFDAPI_TOP_CONTROL
{
    //share with different HW
    STU_CFDAPI_MAIN_CONTROL *pstu_Main_Control;
    STU_CFDAPI_MM_PARSER    *pstu_MM_Param;
    STU_CFDAPI_HDMI_EDID_PARSER *pstu_HDMI_EDID_Param;
    STU_CFDAPI_HDMI_INFOFRAME_PARSER *pstu_HDMI_InfoFrame_Param;
    STU_CFDAPI_HDR_METADATA_FORMAT   *pstu_HDR_Metadata_Format_Param;
    STU_CFDAPI_PANEL_FORMAT *pstu_Panel_Param;
    STU_CFDAPI_OSD_CONTROL  *pstu_OSD_Param;

    //only for HDMI out case
    STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT *pstu_HDMI_InfoFrame_Param_out;

    STU_CFDAPI_OSD_PROCESS_CONFIGS *pstu_OSD_Process_Configs;

    STU_CFDAPI_HAL_SW_RETURN *pstu_SW_Return;

    //HDR vendor-specific zone
    STU_CFDAPI_DOLBY_CONTROL  *pstu_Dolby_Param;

    STU_CFDAPI_HW_IPS *pstu_HW_IP_Param;

    STU_CFDAPI_Custom *pstu_CustomDefine;

} STU_CFDAPI_TOP_CONTROL;

typedef struct _HDR_CONTENT_LIGHT_MATADATA
{
    MS_U8 u8MetaData_Valid;
    MS_U16 u16MaxContentLightLevel;
    MS_U16 u16MaxFrameAverageLightLevel;
} HDR_CONTENT_LIGHT_MATADATA;
//enum for share ============================================================

typedef enum _E_CFD_SL_CS
{

    E_CFD_SL_CS_REC601 = 0x00,
    E_CFD_SL_CS_REC709 = 0x01,
    E_CFD_SL_CS_REC2020 = 0x02,
    E_CFD_SL_CS_RESERVED_START = 0x03,

} E_CFD_SL_CS;

typedef enum _E_CFD_SL_HDR
{
    E_CFD_SL_HDR_TRADITIONAL_GAMMA_SDR = 0x0,
    E_CFD_SL_HDR_TRADITIONAL_GAMMA_HDR = 0x1,
    E_CFD_SL_HDR_SMPTE2084 = 0x2,
    E_CFD_SL_HDR_HLG = 0x3,
    E_CFD_SL_HDR_RESERVED_START = 0x04

} E_CFD_SL_HDR;

//for MS_U8 u8Input_Source
typedef enum _E_CFD_MC_SOURCE
{
    //include VDEC series
    //E_CFD_MC_SOURCE_MM      = 0x00,
    //E_CFD_MC_SOURCE_HDMI    = 0x01,
    //E_CFD_MC_SOURCE_ANALOG  = 0x02,
    //E_CFD_MC_SOURCE_PANEL   = 0x03,
    //E_CFD_MC_SOURCE_DVI     = 0x04,
    //E_CFD_MC_SOURCE_ULSA    = 0x04,
    //E_CFD_MC_SOURCE_RESERVED_START,

    /// VGA
    E_CFD_INPUT_SOURCE_VGA,

    /// ATV
    E_CFD_INPUT_SOURCE_TV,

    /// CVBS
    E_CFD_INPUT_SOURCE_CVBS,

    /// S-video
    E_CFD_INPUT_SOURCE_SVIDEO,

    /// Component
    E_CFD_INPUT_SOURCE_YPBPR,

    /// Scart
    E_CFD_INPUT_SOURCE_SCART,

    /// HDMI
    E_CFD_INPUT_SOURCE_HDMI,

    /// DTV
    E_CFD_INPUT_SOURCE_DTV,

    /// DVI
    E_CFD_INPUT_SOURCE_DVI,

    // Application source
    /// Storage
    E_CFD_INPUT_SOURCE_STORAGE,

    /// KTV
    E_CFD_INPUT_SOURCE_KTV,

    /// JPEG
    E_CFD_INPUT_SOURCE_JPEG,

    //RX for ulsa
    E_CFD_INPUT_SOURCE_RX,

    /// The max support number of PQ input source
    E_CFD_INPUT_SOURCE_RESERVED_START,

    /// None
    E_CFD_INPUT_SOURCE_NONE = E_CFD_INPUT_SOURCE_RESERVED_START,


} E_CFD_INPUT_SOURCE;

#define CFD_IS_HDMI(x) ((x) == E_CFD_INPUT_SOURCE_HDMI)
#define CFD_IS_MM(x) (((x) == E_CFD_INPUT_SOURCE_STORAGE) || ((x) == E_CFD_INPUT_SOURCE_JPEG))
#define CFD_IS_DTV(x) ((x) == E_CFD_INPUT_SOURCE_DTV)
#define CFD_IS_VGA(x)       ((x) == E_CFD_INPUT_SOURCE_VGA)
#define CFD_IS_COMPONENT(x) ((x) == E_CFD_INPUT_SOURCE_YPBPR)
#define CFD_IS_ATV(x) ((x) == E_CFD_INPUT_SOURCE_TV)
#define CFD_IS_CVBS(x) ((x) == E_CFD_INPUT_SOURCE_CVBS)
#define CFD_IS_SCART(x) ((x) == E_CFD_INPUT_SOURCE_SCART)
#define CFD_IS_VD(x) (CFD_IS_ATV(x) || CFD_IS_CVBS(x) || CFD_IS_SCART(x))
#define CFD_IS_ANALOG(X) (((x) == E_CFD_INPUT_SOURCE_VGA) || ((x) == E_CFD_INPUT_SOURCE_TV) || ((x) == E_CFD_INPUT_SOURCE_CVBS) || \
                                        ((x) == E_CFD_INPUT_SOURCE_SVIDEO) || ((x) == E_CFD_INPUT_SOURCE_YPBPR) || ((x) == E_CFD_INPUT_SOURCE_SCART) || \
                                        ((x) == E_CFD_INPUT_SOURCE_DVI)

//for output source
typedef enum _E_CFD_OUTPUT_SOURCE
{
    //include VDEC series
    E_CFD_OUTPUT_SOURCE_MM      = 0x00,
    E_CFD_OUTPUT_SOURCE_HDMI    = 0x01,
    E_CFD_OUTPUT_SOURCE_ANALOG  = 0x02,
    E_CFD_OUTPUT_SOURCE_PANEL   = 0x03,
    E_CFD_OUTPUT_SOURCE_ULSA    = 0x04,
    E_CFD_OUTPUT_SOURCE_RESERVED_START,

} E_CFD_OUTPUT_SOURCE;

#if 0
typedef enum _E_CFD_INPUT_ANALOG_FORMAT//u8HDMISource_EOTF
{
    E_CFD_INPUT_ANALOG_RF_NTSC_44 = 0x00,
    E_CFD_INPUT_ANALOG_RF_NTSC_M,
    E_CFD_INPUT_ANALOG_RF_PAL_BGHI,
    E_CFD_INPUT_ANALOG_RF_PAL_60,
    E_CFD_INPUT_ANALOG_RF_PAL_M,
    E_CFD_INPUT_ANALOG_RF_PAL_N,
    E_CFD_INPUT_ANALOG_RF_SECAM,
    E_CFD_INPUT_ANALOG_VIF_NTSC_44,
    E_CFD_INPUT_ANALOG_VIF_NTSC_M,
    E_CFD_INPUT_ANALOG_VIF_PAL_BGHI,
    E_CFD_INPUT_ANALOG_VIF_PAL_60,
    E_CFD_INPUT_ANALOG_VIF_PAL_M,
    E_CFD_INPUT_ANALOG_VIF_PAL_N,
    E_CFD_INPUT_ANALOG_VIF_SECAM,
    E_CFD_INPUT_ANALOG_SV_NTSC_44,
    E_CFD_INPUT_ANALOG_SV_NTSC_M,
    E_CFD_INPUT_ANALOG_SV_PAL_BGHI,
    E_CFD_INPUT_ANALOG_SV_PAL_60,
    E_CFD_INPUT_ANALOG_SV_PAL_M,
    E_CFD_INPUT_ANALOG_SV_PAL_N,
    E_CFD_INPUT_ANALOG_SV_SECAM,
    E_CFD_INPUT_ANALOG_AV_NTSC_44,
    E_CFD_INPUT_ANALOG_AV_NTSC_M,
    E_CFD_INPUT_ANALOG_AV_PAL_BGHI,
    E_CFD_INPUT_ANALOG_AV_PAL_60,
    E_CFD_INPUT_ANALOG_AV_PAL_M,
    E_CFD_INPUT_ANALOG_AV_PAL_N,
    E_CFD_INPUT_ANALOG_AV_SECAM,
    E_CFD_INPUT_ANALOG_SCART_AV_NTSC_44,
    E_CFD_INPUT_ANALOG_SCART_AV_NTSC_M,
    E_CFD_INPUT_ANALOG_SCART_AV_PAL_BGHI,
    E_CFD_INPUT_ANALOG_SCART_AV_PAL_60,
    E_CFD_INPUT_ANALOG_SCART_AV_PAL_M,
    E_CFD_INPUT_ANALOG_SCART_AV_PAL_N,
    E_CFD_INPUT_ANALOG_SCART_AV_SECAM,
    E_CFD_INPUT_ANALOG_SCART_SV_NTSC_44,
    E_CFD_INPUT_ANALOG_SCART_SV_NTSC_M,
    E_CFD_INPUT_ANALOG_SCART_SV_PAL_BGHI,
    E_CFD_INPUT_ANALOG_SCART_SV_PAL_60,
    E_CFD_INPUT_ANALOG_SCART_SV_PAL_M,
    E_CFD_INPUT_ANALOG_SCART_SV_PAL_N,
    E_CFD_INPUT_ANALOG_SCART_SV_SECAM,
    E_CFD_INPUT_ANALOG_SCART_RGB_NTSC,
    E_CFD_INPUT_ANALOG_SCART_RGB_PAL,
    E_CFD_INPUT_ANALOG_YPBPR_480I,
    E_CFD_INPUT_ANALOG_YPBPR_576I,
    E_CFD_INPUT_ANALOG_YPBPR_480P,
    E_CFD_INPUT_ANALOG_YPBPR_576P,
    E_CFD_INPUT_ANALOG_YPBPR_720P_24HZ,
    E_CFD_INPUT_ANALOG_YPBPR_720P_50HZ,
    E_CFD_INPUT_ANALOG_YPBPR_720P_60HZ,
    E_CFD_INPUT_ANALOG_YPBPR_1080I_50HZ,
    E_CFD_INPUT_ANALOG_YPBPR_1080I_60HZ,
    E_CFD_INPUT_ANALOG_YPBPR_1080P_24HZ,
    E_CFD_INPUT_ANALOG_YPBPR_1080P_50HZ,
    E_CFD_INPUT_ANALOG_YPBPR_1080P_60HZ,
    E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hup_Vup,
    E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hup_Vdown,
    E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hup_Vno,
    E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hdown_Vup,
    E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hdown_Vdown,
    E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hdown_Vno,
    E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hno_Vup,
    E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hno_Vdown,
    E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hno_Vno,
    E_CFD_INPUT_ANALOG_DVI_Dsub_HDMI_RGB_PC_Hno_Vno_4K,
    E_CFD_INPUT_ANALOG_RESERVED_START

} E_CFD_INPUT_ANALOG_FORMAT;
#endif

//follow Tomato's table
typedef enum _E_CFD_CFIO
{
    //start of RGB group
    E_CFD_CFIO_RGB_NOTSPECIFIED      = 0x0, //means RGB, but no specific colorspace
    E_CFD_CFIO_RGB_BT601_625         = 0x1,
    E_CFD_CFIO_RGB_BT601_525         = 0x2,
    E_CFD_CFIO_RGB_BT709             = 0x3,
    E_CFD_CFIO_RGB_BT2020            = 0x4,
    E_CFD_CFIO_SRGB                  = 0x5,
    E_CFD_CFIO_ADOBE_RGB             = 0x6,
    E_CFD_CFIO_RGB_DCIP3_D65         = 0x7,
    E_CFD_CFIO_RGB_DCIP3_THEATER     = 0x8,
    E_CFD_CFIO_RGB_END               = 0x9,

    //start of YUV group
    E_CFD_CFIO_YUV_NOTSPECIFIED      = 0x80, //means YUV, but no specific colorspace
    E_CFD_CFIO_YUV_BT601_625         = 0x81,
    E_CFD_CFIO_YUV_BT601_525         = 0x82,
    E_CFD_CFIO_YUV_BT709             = 0x83,
    E_CFD_CFIO_YUV_BT2020_NCL        = 0x84, //132
    E_CFD_CFIO_YUV_BT2020_CL         = 0x85,
    E_CFD_CFIO_XVYCC_601             = 0x86,
    E_CFD_CFIO_XVYCC_709             = 0x87,
    E_CFD_CFIO_SYCC601               = 0x88,
    E_CFD_CFIO_ADOBE_YCC601          = 0x89,
    E_CFD_CFIO_YUV_END               = 0x8A,

    //start of RGB/YUV group
    E_CFD_CFIO_DOLBY_HDR_TEMP        = 0x8B,
    E_CFD_CFIO_RESERVED_START

} E_CFD_CFIO;

//follow Tomato's table
typedef enum _E_CFD_CFIO_pre
{
    E_CFD_CFIO_RGB_NOTSPECIFIED_pre      = 0x0, //means RGB, but no specific colorspace
    E_CFD_CFIO_RGB_BT601_625_pre         = 0x1,
    E_CFD_CFIO_RGB_BT601_525_pre         = 0x2,
    E_CFD_CFIO_RGB_BT709_pre             = 0x3,
    E_CFD_CFIO_RGB_BT2020_pre            = 0x4,
    E_CFD_CFIO_SRGB_pre                  = 0x5,
    E_CFD_CFIO_ADOBE_RGB_pre             = 0x6,
    E_CFD_CFIO_YUV_NOTSPECIFIED_pre      = 0x7, //means RGB, but no specific colorspace
    E_CFD_CFIO_YUV_BT601_625_pre         = 0x8,
    E_CFD_CFIO_YUV_BT601_525_pre         = 0x9,
    E_CFD_CFIO_YUV_BT709_pre             = 0xA,
    E_CFD_CFIO_YUV_BT2020_NCL_pre        = 0xB,
    E_CFD_CFIO_YUV_BT2020_CL_pre         = 0xC,
    E_CFD_CFIO_XVYCC_601_pre             = 0xD,
    E_CFD_CFIO_XVYCC_709_pre             = 0xE,
    E_CFD_CFIO_SYCC601_pre               = 0xF,
    E_CFD_CFIO_ADOBE_YCC601_pre          = 0x10,
    E_CFD_CFIO_DOLBY_HDR_TEMP_pre        = 0x11,
    E_CFD_CFIO_RESERVED_START_pre

} E_CFD_CFIO_pre;



typedef enum _E_CFD_MC_FORMAT
{

    E_CFD_MC_FORMAT_RGB     = 0x00,
    E_CFD_MC_FORMAT_YUV422  = 0x01,
    E_CFD_MC_FORMAT_YUV444  = 0x02,
    E_CFD_MC_FORMAT_YUV420  = 0x03,
    E_CFD_MC_FORMAT_RESERVED_START

} E_CFD_MC_FORMAT;

typedef enum _E_CFD_CFIO_RANGE//u8MM_Codec
{
    E_CFD_CFIO_RANGE_LIMIT   = 0x0,
    E_CFD_CFIO_RANGE_FULL    = 0x1,
    E_CFD_CFIO_RANGE_RESERVED_START

} E_CFD_CFIO_RANGE;

typedef enum _E_CFD_HDR_STATUS
{
    E_CFIO_MODE_SDR           = 0x0,
    E_CFIO_MODE_HDR1          = 0x1,
    E_CFIO_MODE_HDR2          = 0x2, //SMPTE2084
    E_CFIO_MODE_HDR3          = 0x3, //Hybrid log gamma
    E_CFIO_MODE_RESERVED_START

} E_CFD_HDR_STATUS;

//ColorPrimary
//use order in HEVC spec and add AdobeRGB

//updated by JCTVC-W1005-v4

typedef enum _E_CFD_CFIO_CP
{
    E_CFD_CFIO_CP_RESERVED0             = 0x0, //means RGB, but no specific colorspace
    E_CFD_CFIO_CP_BT709_SRGB_SYCC       = 0x1,
    E_CFD_CFIO_CP_UNSPECIFIED           = 0x2,
    E_CFD_CFIO_CP_RESERVED3             = 0x3,
    E_CFD_CFIO_CP_BT470_6               = 0x4,
    E_CFD_CFIO_CP_BT601625              = 0x5,
    E_CFD_CFIO_CP_BT601525_SMPTE170M    = 0x6,
    E_CFD_CFIO_CP_SMPTE240M             = 0x7,
    E_CFD_CFIO_CP_GENERIC_FILM          = 0x8,
    E_CFD_CFIO_CP_BT2020                = 0x9,
    E_CFD_CFIO_CP_CIEXYZ                = 0xA,
    E_CFD_CFIO_CP_DCIP3_THEATER         = 0xB,
    E_CFD_CFIO_CP_DCIP3_D65             = 0xC,
    E_CFD_CFIO_CP_ADOBERGB              = 0xD,
    E_CFD_CFIO_CP_PANEL                 = 0xE,
    E_CFD_CFIO_CP_EBU3213               = 0x16,
    E_CFD_CFIO_CP_RESERVED_START

} E_CFD_CFIO_CP;

//Transfer characteristics
//use order in HEVC spec and add AdobeRGB

//add E_CFD_CFIO_TR_GAMMA2P6 for HDMI input with DCIP3

typedef enum _E_CFD_CFIO_TR
{
    E_CFD_CFIO_TR_RESERVED0             = 0x0, //means RGB, but no specific colorspace
    E_CFD_CFIO_TR_BT709                 = 0x1,
    E_CFD_CFIO_TR_UNSPECIFIED           = 0x2,
    E_CFD_CFIO_TR_RESERVED3             = 0x3,
    E_CFD_CFIO_TR_GAMMA2P2              = 0x4,
    E_CFD_CFIO_TR_GAMMA2P8              = 0x5,
    E_CFD_CFIO_TR_BT601525_601625       = 0x6,
    E_CFD_CFIO_TR_SMPTE240M             = 0x7,
    E_CFD_CFIO_TR_LINEAR                = 0x8,
    E_CFD_CFIO_TR_LOG0                  = 0x9,
    E_CFD_CFIO_TR_LOG1                  = 0xA,
    E_CFD_CFIO_TR_XVYCC                 = 0xB,
    E_CFD_CFIO_TR_BT1361                = 0xC,
    E_CFD_CFIO_TR_SRGB_SYCC             = 0xD,
    E_CFD_CFIO_TR_BT2020NCL             = 0xE,
    E_CFD_CFIO_TR_BT2020CL              = 0xF,
    E_CFD_CFIO_TR_SMPTE2084             = 0x10,
    E_CFD_CFIO_TR_SMPTE428              = 0x11,
    E_CFD_CFIO_TR_HLG                   = 0x12,
    E_CFD_CFIO_TR_BT1886                = 0x13,
    E_CFD_CFIO_TR_DOLBYMETA             = 0x14,
    E_CFD_CFIO_TR_ADOBERGB              = 0x15,
    E_CFD_CFIO_TR_GAMMA2P6              = 0x16,
    E_CFD_CFIO_TR_RESERVED_START

} E_CFD_CFIO_TR;

//Matrix coefficient
//use order in HEVC spec

//updated by JCTVC-W1005-v4

typedef enum _E_CFD_CFIO_MC
{
    E_CFD_CFIO_MC_IDENTITY              = 0x0, //means RGB, but no specific colorspace
    E_CFD_CFIO_MC_BT709_XVYCC709        = 0x1,
    E_CFD_CFIO_MC_UNSPECIFIED           = 0x2,
    E_CFD_CFIO_MC_RESERVED              = 0x3,
    E_CFD_CFIO_MC_USFCCT47              = 0x4,
    E_CFD_CFIO_MC_BT601625_XVYCC601_SYCC    = 0x5,
    E_CFD_CFIO_MC_BT601525_SMPTE170M        = 0x6,
    E_CFD_CFIO_MC_SMPTE240M             = 0x7,
    E_CFD_CFIO_MC_YCGCO                 = 0x8,
    E_CFD_CFIO_MC_BT2020NCL             = 0x9,
    E_CFD_CFIO_MC_BT2020CL              = 0xA,
    E_CFD_CFIO_MC_YDZDX                 = 0xB,
    E_CFD_CFIO_MC_CD_NCL                = 0xC,
    E_CFD_CFIO_MC_CL_CL                 = 0xD,
    E_CFD_CFIO_MC_RESERVED_START

} E_CFD_CFIO_MC;

typedef enum _E_CFD_MAIN_SUB_MODE
{
    E_CFD_MS_SC0_MAIN      = 0x0,
    E_CFD_MS_SC1_SUB       = 0x1,

} E_CFD_MAIN_SUB_MODE;

typedef enum _E_CFD_HDMI_HDR_INFOFRAME_EOTF//u8HDMISource_EOTF
{
    E_CFD_HDMI_EOTF_SDR_GAMMA       = 0x0,
    E_CFD_HDMI_EOTF_HDR_GAMMA       = 0x1,
    E_CFD_HDMI_EOTF_SMPTE2084       = 0x2,
    E_CFD_HDMI_EOTF_FUTURE_EOTF     = 0x3,
    E_CFD_HDMI_EOTF_RESERVED        = 0x4

} E_CFD_HDMI_HDR_INFOFRAME_EOTF;

typedef enum _E_CFD_HDMI_HDR_INFOFRAME_METADATAs//u8HDMISource_SMD_ID
{
    E_CFD_HDMI_META_TYPE1       = 0x0,
    E_CFD_HDMI_META_RESERVED    = 0x1

} E_CFD_HDMI_HDR_INFOFRAME_METADATA;

typedef enum _E_CFD_VALIDORNOT//u8MM_Codec
{
    E_CFD_NOT_VALID   = 0x0,
    E_CFD_VALID       = 0x1,
    E_CFD_VALID_EMUEND = 0x2

} E_CFD_VALIDORNOT;


typedef enum _E_CFD_MC_MODE//enum for u8HDR2SDR_Mode/u8SDR_Mode
{
    E_CFD_MC_MODE_BYPASS,
    E_CFD_MC_MODE_NORMAL,
    E_CFD_MC_MODE_TEST,
    E_CFD_MC_MODE_EMUEND

} E_CFD_MC_MODE;

typedef enum _E_CFD_MC_PROCESS//enum for u8HDR2SDR_Mode/u8SDR_Mode
{
    E_CFD_PROCESS_HDRBYPASS = 0x00,
    E_CFD_PROCESS_SDRBYPASS,
    E_CFD_PROCESS_HDR2SDR,
    E_CFD_PROCESS_SDR2HDR

} E_CFD_MC_PROCESS;



typedef enum _E_CFD_MC_HW_STRUCTURE//enum for u8HDR2SDR_Mode/u8SDR_Mode
{
    //Muji/Manhattan
    E_CFD_HWS_TV_TYPE0 = 0x00,

    //Maserati
    E_CFD_HWS_TV_TYPE1,

    //Curry
    E_CFD_HWS_STB_TYPE0,

    //Kano
    //input without analog in
    E_CFD_HWS_STB_TYPE1,
    E_CFD_HWS_EMUEND

} E_CFD_MC_HW_STRUCTURE;


typedef enum _E_CFD_ASSIGN_MODE//enum for u8HDR2SDR_Mode/u8SDR_Mode
{
    E_CFD_MODE_AT0x00 = 0x00,
    E_CFD_MODE_AT0x01 = 0x01,
    E_CFD_MODE_AT0x02 = 0x02,
    E_CFD_MODE_AT0x03 = 0x03,
    E_CFD_MODE_AT0x04 = 0x04,
    E_CFD_MODE_AT0x05 = 0x05,
    E_CFD_MODE_RESERVED_START

} E_CFD_ASSIGN_MODE;

typedef enum _E_CFD_RESERVED_START//enum for u8HDR2SDR_Mode/u8SDR_Mode
{
    E_CFD_RESERVED_AT0x00 = 0x00,
    E_CFD_RESERVED_AT0x01 = 0x01,
    E_CFD_RESERVED_AT0x02 = 0x02,
    E_CFD_RESERVED_AT0x03 = 0x03,
    E_CFD_RESERVED_AT0x04 = 0x04,
    E_CFD_RESERVED_AT0x05 = 0x05,
    E_CFD_RESERVED_AT0x08 = 0x08,
    E_CFD_RESERVED_AT0x0D = 0x0D,
    E_CFD_RESERVED_AT0x0E = 0x0E,
    E_CFD_RESERVED_AT0x80 = 0x80,
    E_CFD_RESERVED_RESERVED_START

} E_CFD_RESERVED_START;

typedef enum _E_CFD_MC_ERR//enum for u8HDR2SDR_Mode/u8SDR_Mode
{
    //Main control starts from 0x0000
    //if error happens, see Mapi_Cfd_inter_Main_Control_Param_Check()
    E_CFD_MC_ERR_NOERR      = 0x0000, //process is ok
    E_CFD_MC_ERR_INPUT_SOURCE    = 0x0001, //input source is over defined range, please check. force input_source to E_CFD_MC_SOURCE_HDMI now!!
    E_CFD_MC_ERR_INPUT_ANALOGIDX = 0x0002, //input analog idx is over defined range, please check. force input analog idx to E_CFD_INPUT_ANALOG_RF_NTSC_44 now!!
    E_CFD_MC_ERR_INPUT_FORMAT = 0x0003, //input format is over defined range, please check. force input format to E_CFD_CFIO_YUV_BT709 now!!

    E_CFD_MC_ERR_INPUT_DATAFORMAT = 0x0004, //input data format is over defined range, please check. force input data format to E_CFD_MC_FORMAT_YUV422 now!!
    E_CFD_MC_ERR_INPUT_ISFULLRANGE = 0x0005, //input data format is over defined range, please check. force MainControl u8Input_IsFullRange to E_CFD_CFIO_RANGE_LIMIT!!

    E_CFD_MC_ERR_INPUT_HDRMODE = 0x0006, //input HDR mode is over defined range, please check. force MainControl u8Input_HDRMode to E_CFIO_MODE_SDR!!
    E_CFD_MC_ERR_INPUT_ISRGBBYPASS = 0X0007,

    E_CFD_MC_ERR_INPUT_SDRIPMODE = 0x0008,
    E_CFD_MC_ERR_INPUT_HDRIPMODE = 0x0009,

    E_CFD_MC_ERR_INPUT_Mid_Format_Mode = 0x000a,
    E_CFD_MC_ERR_INPUT_Mid_Format       = 0x000b,
    E_CFD_MC_ERR_INPUT_Mid_DataFormat   = 0x000c,
    E_CFD_MC_ERR_INPUT_Mid_IsFullRange  = 0x000d,
    E_CFD_MC_ERR_INPUT_Mid_HDRMode      = 0x000e,
    E_CFD_MC_ERR_INPUT_Mid_Colour_primaries = 0x000f,
    E_CFD_MC_ERR_INPUT_Mid_Transfer_Characteristics = 0x0010,
    E_CFD_MC_ERR_INPUT_Mid_Matrix_Coeffs = 0x0011,

    E_CFD_MC_ERR_OUTPUT_SOURCE    = 0x0012,
    E_CFD_MC_ERR_OUTPUT_FORMAT    = 0x0013,

    E_CFD_MC_ERR_OUTPUT_DATAFORMAT  = 0x0014,
    E_CFD_MC_ERR_OUTPUT_ISFULLRANGE = 0x0015,

    E_CFD_MC_ERR_OUTPUT_HDRMODE = 0x0016,

    E_CFD_MC_ERR_HDMIOutput_GammutMapping_Mode = 0x0017,
    E_CFD_MC_ERR_HDMIOutput_GammutMapping_MethodMode = 0x0018,
    E_CFD_MC_ERR_MMInput_ColorimetryHandle_Mode = 0x0019,
    E_CFD_MC_ERR_PanelOutput_GammutMapping_Mode = 0x001a,
    E_CFD_MC_ERR_TMO_TargetRefer_Mode = 0x001b,
    E_CFD_MC_ERR_Target_Max_Luminance = 0x001c,
    E_CFD_MC_ERR_Target_Med_Luminance = 0x001d,
    E_CFD_MC_ERR_Target_Min_Luminance = 0x001e,
    E_CFD_MC_ERR_Source_Max_Luminance = 0x001f,
    E_CFD_MC_ERR_Source_Med_Luminance = 0x0020,
    E_CFD_MC_ERR_Source_Min_Luminance = 0x0021,

    E_CFD_MC_ERR_INPUT_HDRIPMODE_HDRMODE_RULE_VIOLATION = 0X0040,
    E_CFD_MC_ERR_INPUT_FORMAT_DATAFORMAT_NOT_MATCH,
    E_CFD_MC_ERR_INPUT_MAIN_CONTROLS,

    //force input
    E_CFD_MC_ERR_INPUT_ISRGBBYPASS_FORCE0,

    //force output
    E_CFD_MC_ERR_OUTPUT_ISFULLRANGE_FORCE1,

    E_CFD_MC_ERR_OVERRANGE,

    //MM starts from 0x0100
    E_CFD_MC_ERR_MM_Colour_primaries = 0x0100,
    E_CFD_MC_ERR_MM_Transfer_Characteristics = 0x0101,
    E_CFD_MC_ERR_MM_Matrix_Coeffs = 0x0102,
    E_CFD_MC_ERR_MM_IsFullRange = 0x0103,
    E_CFD_MC_ERR_MM_Mastering_Display = 0x0104,
    E_CFD_MC_ERR_MM_Mastering_Display_Coordinates = 0x0105,
    E_CFD_MC_ERR_MM_Handle_Undefined_Case = 0x0106,
    E_CFD_MC_ERR_MM_Input_OutofRange = 0x0107,

    //HDMI EDID starts from 0x0200
    E_CFD_MC_ERR_HDMI_EDID = 0x0200,
    E_CFD_MC_ERR_HDMI_EDID_Mastering_Display_Coordinates = 0x0201,
    E_CFD_ERR_HDMI_EDID_HDR_LUM = 0x0202,

    //HDMI InfoFrame starts from 0x0280,
    E_CFD_MC_ERR_HDMI_INFOFRAME = 0x0280,
    E_CFD_MC_ERR_HDMI_INFOFRAME_HDR_Infor = 0x0281,
    E_CFD_MC_ERR_HDMI_INFOFRAME_Mastering_Display_Coordinates = 0x0282,

    //Panel starts from 0x0300
    E_CFD_MC_ERR_Panel_infor = 0x0300,
    E_CFD_MC_ERR_Panel_infor_Mastering_Display_Coordinates = 0x0301,

    //OSD starts from 0x380
    E_CFD_MC_ERR_OSD_infor = 0x0380,

    //HW control starts from 0x390
    E_CFD_MC_ERR_HW_Main_Param = 0x0390,

    //controls for HW
    E_CFD_MC_ERR_HW_IPS_PARAM_OVERRANGE = 0x03A0, //parameters for HW IP control
    E_CFD_MC_ERR_HW_IPS_PARAM_HDR_OVERRANGE = 0x03A1, //parameters for HW IP control
    E_CFD_MC_ERR_HW_IPS_PARAM_SDR_OVERRANGE = 0x03A2, //parameters for HW IP control
    E_CFD_MC_ERR_HW_IPS_PARAM_TMO_OVERRANGE = 0x03A3, //parameters for HW IP control
    E_CFD_MC_ERR_HW_IPS_PARAM_DLC_OVERRANGE = 0x03A4, //parameters for HW IP control
    E_CFD_MC_ERR_HW_IPS_MODE_HDR_NOTSUPPORTED = 0x03A5, //parameters for HW IP control
    E_CFD_MC_ERR_HW_IPS_MODE_SDR_NOTSUPPORTED = 0x03A6, //parameters for HW IP control
    E_CFD_MC_ERR_HW_IPS_MODE_TMO_NOTSUPPORTED = 0x03A7, //parameters for HW IP control
    E_CFD_MC_ERR_HW_IPS_MODE_DLC_NOTSUPPORTED = 0x03A8, //parameters for HW IP control

    //interrelation of API
    E_CFD_MC_ERR_INPUT_MAIN_CONTROLS_MM_INTERRELATION,
    E_CFD_MC_ERR_INPUT_MAIN_CONTROLS_HDMI_INTERRELATION,
    E_CFD_MC_ERR_OUTPUT_PANEL_SDRIPMODE_INTERRELATION,

    //HW IP Capability
    E_CFD_MC_ERR_HW_IPS_BT2020CLtoNCL_NOTSUPPORTED = 0x0400,
    E_CFD_MC_ERR_HW_IPS_GMforXVYCC_NOTSUPPORTED = 0x0401,
    E_CFD_MC_ERR_HW_IPS_SDR2HDR_NOTSUPPORTED = 0x0402, //no SDR to HDR now
    E_CFD_MC_ERR_HW_IPS_HDRXtoHDRY_NOTSUPPORTED = 0x0403,
    E_CFD_MC_ERR_HW_IPS_GM_NOTSUPPORTED = 0x0404,
    E_CFD_MC_ERR_HW_IPS_TMO_NOTSUPPORTED = 0x0404,
    E_CFD_MC_ERR_HW_IPS_TMO_INRGB_NOTSUPPORTED = 0x0405,

    //
    //E_CFD_MC_ERR_BYPASS       , //STB/TV can not handle this case, so force all IP bypass
    //E_CFD_MC_ERR_CONTROLINPUT,//something wrong in setting for STU_CFDAPI_MAIN_CONTROL, please check settings
    //E_CFD_MC_ERR_CONTROL_MM_INPUT , ////something wrong in parameters of structures of main control and MM
    //E_CFD_MC_ERR_MMINPUT_FORCE709   , //force input_format to BT709, some information is not avaliable for MM
    //E_CFD_MC_ERR_MMINPUT0   ,
    //E_CFD_MC_ERR_HDMIINPUT  , //this function can not support current HDMI input
    //E_CFD_MC_ERR_ANALOGINPUT , //this function can not support current Analog input
    //E_CFD_MC_ERR_WRONGINPUT  , //this function can not support current input
    //E_CFD_MC_ERR_WRONGINPUTSOURCE  ,//can not support current input source, check the value of u8Input_Source
    E_CFD_MC_ERR_WRONGOUTPUTSOURCE  ,//can not support current output source, check the value of u8Output_Source
    E_CFD_MC_ERR_PROCESSOFF   , //process off
    //E_CFD_MC_ERR_NOSDR2HDRNOW , //no SDR to HDR now, check u8Input_HDRMode & u8Output_HDRMode
    //E_CFD_MC_ERR_WRONGTMOSET  , //not support such TMO , check u8Input_HDRMode & u8Output_HDRMode
    //E_CFD_MC_ERR_PARTIALWRONG , //STB/TV can not handle this case, so force all IP bypass
    //E_CFD_MC_ERR_HW_NOT_SUPPORT_THIS_INPUT, //for current HW, this input format can not supported
    //E_CFD_MC_ERR_HW_NOT_SUPPORT_THIS_OUTPUT, //for current HW, this input format can not supported

    //HW IP process function from Ali
    E_CFD_MC_ERR_HW_IP_PARAMETERS, //check the parameters from CFD to IP process functions from Ali

    //E_CFD_MC_ERR_DLCIP_PARAMETERS, //need to check the parameters for DLC IPs, setting is out of range
    //E_CFD_MC_ERR_0x1001_ERR_IN_Input_Analog_SetConfigures = 0x1001, //some errors happens in Input_Analog_SetConfigures(), please debug

    E_CFD_MC_ERR_EMUEND

} E_CFD_MC_ERR;



//function prototype ==================================================================================
#if (1 == RealChip)
MS_BOOL Mhal_Cfd_HDRIP_copy(MS_U8 u8window, STU_CFDAPI_Curry_HDRIP* pstHDRIP);
#endif

void MS_Cfd_OSD_Process_configs_print(STU_CFDAPI_OSD_PROCESS_CONFIGS *pstu_OSD_Process_Configs);

void Mhal_Cfd_CustomDefine_Set_ForceBT2020output(MS_U8 u8value );

void Mhal_CFD_ForceSupportBT2020_HDR_HDMI_SINK_v1(STU_CFDAPI_HDMI_EDID_PARSER *pstu_HDMI_EDID_Param);


void Mhal_Cfd_OSD_H2SUI_Set(MS_U8 u8Value);
MS_BOOL Mhal_Cfd_Hal_Control_Get(STU_CFDAPI_HAL_CONTROL* pst_cfd_hal_control);
void Mhal_Cfd_Hal_SetAutodownloadFirePermission(MS_U8 u8Value);
void Mapi_Cfd_InputFormat_Preconstrain(STU_CFDAPI_TOP_CONTROL *pstu_Cfd_api_top);

//main function of color format driver
INTERFACE MS_U16 Mapi_Cfd_ColorFormatDriver_Control(STU_CFDAPI_TOP_CONTROL *pstu_Cfd_api_top);

INTERFACE MS_U8 MS_Cfd_nits2code_max_EDID(MS_U16 u16Nits, MS_U16 *nits_table);

INTERFACE MS_U8 MS_Cfd_nits2code_min_EDID(MS_U16 u16Nits, MS_U32 *nits_table, MS_U16 U16_max_luminance, MS_U8 *pu8DataRange_flag);


//turn off color format driver
//INTERFACE void Mapi_Cfd_TurnOff(STU_CFDAPI_MAIN_CONTROL *pstControlParamInit);

//turn on color format driver
//INTERFACE void Mapi_Cfd_TurnOn(STU_CFDAPI_MAIN_CONTROL *pstControlParamInit);

INTERFACE MS_U16 Mapi_Cfd_inter_TOP_Param_CrossCheck(STU_CFDAPI_TOP_CONTROL *pstu_Cfd_api_top);

//function for STU_CFDAPI_MAIN_CONTROL structure
INTERFACE void Mapi_Cfd_inter_Main_Control_Param_Init(STU_CFDAPI_MAIN_CONTROL *pstControlParamInit );

//MS_U8 u8enum_type
//0: old
//1: new
MS_U16 Mapi_Cfd_inter_Main_Control_Param_Check(STU_CFDAPI_MAIN_CONTROL *pstControlParamInit, MS_U8 u8enum_type);

INTERFACE void Mapi_Cfd_inter_Main_Control_Param_Set(STU_CFDAPI_MAIN_CONTROL *pstControlParamInit);
INTERFACE void Mapi_Cfd_inter_Main_Control_Param_Get(STU_CFDAPI_MAIN_CONTROL *pstControlParam_get);
INTERFACE void Mapi_Cfd_Set_Main_Input(STU_CFDAPI_MAIN_CONTROL *pstu_Main_Control);
INTERFACE void Mapi_Cfd_Set_Main_MiddleControl(STU_CFDAPI_MAIN_CONTROL *pstu_Main_Control, MS_U8 u8EDIDhasBT2020);
INTERFACE void Mapi_Cfd_Set_Main_Gamutmapping(STU_CFDAPI_MAIN_CONTROL *pstu_Main_Control, MS_U8 u8value);


//function for STU_CFDAPI_MM_PARSER structure of parsing MM
INTERFACE void Mapi_Cfd_inter_MM_Param_Init(STU_CFDAPI_MM_PARSER *pstMMParamInita);
MS_U16 Mapi_Cfd_inter_MM_Param_Check(STU_CFDAPI_MM_PARSER *pstMMParamInita);
INTERFACE void Mapi_Cfd_inter_MM_Param_Set(STU_CFDAPI_MM_PARSER *pstMMParamInita);
INTERFACE void Mapi_Cfd_inter_MM_Param_Get(STU_CFDAPI_MM_PARSER *pstMMParam_get);
INTERFACE void Mapi_Cfd_Set_MM_SDR_Rec2020(STU_CFDAPI_MM_PARSER *pstu_MM_Param);
INTERFACE void Mapi_Cfd_Set_MM_SDR_Rec709(STU_CFDAPI_MM_PARSER *pstu_MM_Param);
INTERFACE void Mapi_Cfd_Set_MM_HDRPQ_Rec2020(STU_CFDAPI_MM_PARSER *pstu_MM_Param);
INTERFACE void Mapi_Cfd_Set_MM_HDRPQ_Rec709(STU_CFDAPI_MM_PARSER *pstu_MM_Param);



//function for STU_CFDAPI_HDMI_INFOFRAME_PARSER structure of parsing HDMI InfoFrame
INTERFACE void Mapi_Cfd_inter_HDMI_InfoFrame_Param_Init(STU_CFDAPI_HDMI_INFOFRAME_PARSER *pstHDMI_INFOFRAME_ParamInita);
MS_U16 Mapi_Cfd_inter_HDMI_InfoFrame_Param_Check(STU_CFDAPI_HDMI_INFOFRAME_PARSER *pstHDMI_INFOFRAME_ParamInita);
INTERFACE void Mapi_Cfd_inter_HDMI_InfoFrame_Param_Set(STU_CFDAPI_HDMI_INFOFRAME_PARSER *pstHDMI_INFOFRAME_ParamInita);
INTERFACE void Mapi_Cfd_inter_HDMI_InfoFrame_Param_Out_Get(STU_CFDAPI_HDMI_INFOFRAME_PARSER_OUT *pstHDMI_INFOFRAME_OUT_ParamInita);
INTERFACE void Mapi_Cfd_inter_OSD_Process_Param_Out_Get(STU_CFDAPI_OSD_PROCESS_CONFIGS *pstu_Cfd_OSD_Process_Configs);

//function for STU_CFDAPI_HDMI_EDID_PARSER structure of parsing HDMI EDID
INTERFACE void Mapi_Cfd_inter_HDMI_EDID_Param_Init(STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParamInita);
MS_U16 Mapi_Cfd_inter_HDMI_EDID_Param_Check(STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParamInita);
INTERFACE void Mapi_Cfd_inter_HDMI_EDID_Param_Set(STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParamInita);
INTERFACE void Mapi_Cfd_inter_HDMI_EDID_Param_Get(STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParam_get);
INTERFACE MS_U8 Mapi_Cfd_inter_HDMI_EDID_Param_CheckSupportBT2020(STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParam);

//set SDR/HDR in EDID
INTERFACE void Mapi_Cfd_inter_HDMI_EDID_Param_SetSDR(STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParamInita);
INTERFACE void Mapi_Cfd_inter_HDMI_EDID_Param_SetHDRPQ(STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParamInita);

//set colorspace in EDID
INTERFACE void Mapi_Cfd_inter_HDMI_EDID_Param_SetRec709Only(STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParamInita);
INTERFACE void Mapi_Cfd_inter_HDMI_EDID_Param_SetRec2020ncl(STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParamInita);


//funtion for STU_CFDAPI_HDR_FORMAT structure of HDR metadata
INTERFACE void Mapi_Cfd_inter_HDR_Metadata_Param_Init(STU_CFDAPI_HDR_METADATA_FORMAT *pstu_HDR_Metadata_Param);
INTERFACE MS_U8 Mapi_Cfd_inter_HDR_Metadata_Param_Check(STU_CFDAPI_HDR_METADATA_FORMAT *pstu_HDR_Metadata_Param);
INTERFACE void Mapi_Cfd_inter_HDR_Metadata_Param_Set(STU_CFDAPI_HDR_METADATA_FORMAT *pstu_HDR_Metadata_Param);


//function for STU_CFDAPI_PANEL_FORMAT structure of output Panel
INTERFACE void Mapi_Cfd_inter_PANEL_Param_Init(STU_CFDAPI_PANEL_FORMAT *pstPanelParamInita);
MS_U16 Mapi_Cfd_inter_PANEL_Param_Check(STU_CFDAPI_PANEL_FORMAT *pstPanelParamInita);
INTERFACE void Mapi_Cfd_inter_PANEL_Param_Set(STU_CFDAPI_PANEL_FORMAT *pstPanelParamInita);

//function for STU_CFDAPI_OSD_CONTROL structure of OSD
INTERFACE void Mapi_Cfd_inter_OSD_Param_Init(STU_CFDAPI_OSD_CONTROL *pstu_OSD_Param);
INTERFACE MS_U16 Mapi_Cfd_inter_OSD_Param_Check(STU_CFDAPI_OSD_CONTROL *pstu_OSD_Param);
INTERFACE void Mapi_Cfd_inter_OSD_Param_Set(STU_CFDAPI_OSD_CONTROL *pstu_OSD_Param);
//function for STU_CFDAPI_TMO_CONTROL structure of TMO algorithm
//void Mapi_Cfd_TMO_Param_Init(STU_CFDAPI_TMO_CONTROL *pstu_TMO_Param);
//MS_U8 Mapi_Cfd_TMO_Param_Check(STU_CFDAPI_TMO_CONTROL *pstu_TMO_Param);

//void Mapi_Cfd_Manhattan_TMOIP_Param_Init(STU_CFDAPI_Manhattan_TMOIP *pstu_Manhattan_TMOIP_Param);
//void Mapi_Cfd_Manhattan_HDRIP_Param_Init(STU_CFDAPI_Manhattan_HDRIP *pstu_Manhattan_HDRIP_Param);
//void Mapi_Cfd_Manhattan_SDRIP_Param_Init(STU_CFDAPI_Manhattan_SDRIP *pstu_Manhattan_SDRIP_Param);

//void Mapi_Cfd_Curry_DLCIP_Param_Init(STU_CFDAPI_Curry_DLCIP *pstu_Curry_DLCIP_Param);
MS_U16 Mapi_Cfd_Curry_TMOIP_Param_Check(STU_CFDAPI_Curry_TMOIP *pstu_Curry_TMOIP_Param);
INTERFACE void Mapi_Cfd_Curry_TMOIP_Param_Set(MS_U8 u8HWMainSubMode, STU_CFDAPI_Curry_TMOIP *pstu_Curry_TMOIP_Param);

void Mapi_Cfd_Curry_TMOIP_Param_Init(STU_CFDAPI_Curry_TMOIP *pstu_Curry_TMOIP_Param);
//MS_U16 Mapi_Cfd_Curry_DLCIP_Param_Check(STU_CFDAPI_Curry_DLCIP *pstu_Curry_DLCIP_Param);
//INTERFACE void Mapi_Cfd_Curry_DLCIP_Param_Set(MS_U8 u8HWMainSubMode, STU_CFDAPI_Curry_DLCIP *pstu_Curry_DLCIP_Param);
INTERFACE void Mapi_Cfd_Curry_DLCIP_CurveMode_Set(MS_U8 u8HWMainSubMode, MS_BOOL bUserMode);

void Mapi_Cfd_Curry_SDRIP_Param_Init(STU_CFDAPI_Curry_SDRIP *pstu_Curry_SDRIP_Param);
MS_U16 Mapi_Cfd_Curry_SDRIP_Param_Check(STU_CFDAPI_Curry_SDRIP *pstu_Curry_SDRIP_Param);
INTERFACE void Mapi_Cfd_Curry_SDRIP_Param_Set(MS_U8 u8HWMainSubMode, STU_CFDAPI_Curry_SDRIP *pstu_Curry_SDRIP_Param);

void Mapi_Cfd_Curry_HDRIP_Param_Init(STU_CFDAPI_Curry_HDRIP *pstu_Curry_HDRIP_Param);
MS_U16 Mapi_Cfd_Curry_HDRIP_Param_Check(STU_CFDAPI_Curry_HDRIP *pstu_Curry_HDRIP_Param);
INTERFACE void Mapi_Cfd_Curry_HDRIP_Param_Set(MS_U8 u8HWMainSubMode, STU_CFDAPI_Curry_HDRIP *pstu_Curry_HDRIP_Param);


//clip function of u16Luminance
//u16Luminance = clip(u16Luminance,u16UPBound,u16LowBound)

//return TRUE is input u16Luminance is in the range
//return FALSE is input u16Luminance is out of range
INTERFACE MS_U8 MS_Cfd_CheckLuminanceBound(MS_U16 *u16Luminance, MS_U16 u16UPBound, MS_U16 u16LowBound);


//function: MS_Cfd_Check_Chromaticity_Coordinates
//purpose: (a) check the order of x and y of RGB is ok
//         (b) check the range of x and y is ok

//parameters:
//p_stu_colormetry : pointer of colorimetry strtucture contains 3 primaries and a white point
//u16_xy_highbound : high bound of value of x of y of color primaries

//return value
//bit 0 : indicate that Rx > Gx > Bx, Gy > Ry > By is followed
//bit 1 : indicate that default range [0~1] of xy is followed

INTERFACE MS_U8 MS_Cfd_inter_Check_Chromaticity_Coordinates(STU_CFD_COLORIMETRY *p_stu_colormetry, MS_U16 u16_xy_highbound);


INTERFACE void Mapi_Cfd_Main_Control_PrintResult(STU_CFDAPI_TOP_CONTROL *pstu_Cfd_top);

MS_U8 MS_Cfd_InputMM_SetColorMetry(STU_CFD_MS_ALG_COLOR_FORMAT * pstu_ControlParam, STU_CFDAPI_MM_PARSER *pstu_MMParam, STU_CFDAPI_MAIN_CONTROL * pstu_Main_Control, MS_U8 u8mode);

//=======================================================================================
//purpose
//set colormetry due to HDMI definition

INTERFACE MS_U16 MS_Cfd_Maserati_Dolby_Control(STU_CFDAPI_DOLBY_CONTROL *pstu_Dolby_Param);

INTERFACE void MS_Cfd_OSD_Control(STU_CFDAPI_OSD_CONTROL *pstu_OSD_Param,MS_U8 u8MainSubMode );

//INTERFACE void MS_Cfd_SetOutputGamut_HDMISink(STU_CFDAPI_HDMI_EDID_PARSER *pstHDMI_EDIDParamInit,STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam,STU_CFDAPI_MAIN_CONTROL *pstApiControlParamInit);


//end of function prototype ==================================================================================

//test vector ================================================================================================

//INTERFACE void Mapi_Cfd_Main_Control_Param_Init_test000(STU_CFDAPI_MAIN_CONTROL *pstControlParamInit );
INTERFACE void Mapi_Cfd_testvector001(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector002(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector003(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector004(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector005(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector006(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector007(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector008(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector009(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector010(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);

INTERFACE void Mapi_Cfd_testvector011(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector012(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector013(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector014(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector015(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);

//void Mapi_Cfd_testvector016(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector020(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);

INTERFACE void Mapi_Cfd_testvector021(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector022(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector023(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector024(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector025(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector301(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector302(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector303(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector034(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector035(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Mapi_Cfd_testvector036(STU_CFDAPI_TOP_CONTROL *pstu_CfdAPI_Top_Param);
INTERFACE void Color_Format_Driver(void);
INTERFACE void Color_Format_Driver_Dolby_Set(MS_U8 u8Dolby_Mode);
INTERFACE void Mapi_Cfd_ResetLastDolbyMode();
#undef INTERFACE
#endif

