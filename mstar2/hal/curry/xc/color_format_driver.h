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

#ifndef _HAL_COLOR_FORMAT_DRIVER_H
#define _HAL_COLOR_FORMAT_DRIVER_H

#ifdef _HAL_COLOR_FORMAT_DRIVER_C
#define INTERFACE
#else
#define INTERFACE                               extern
#endif

#include "color_format_driver_hardware.h"

#define Manhattan 0
#define Maserati 1
#define Mainz 2
#define Mooney 3
#define Kano 100
#define Curry 101
#define Kastor 102
#define U13_Universe 200

#define CFD_limit 0
#define CFD_full 1

//0: not refer to EDID
//1: refer to EDID
#define CFD_BypassHDRInforFrameReferEDID 1

#if (1 == RealChip)
#define  HDR_DBG_HAL_CFD(x)  //x
#define function1(var1,var2) var1 = (var1&MaskForMode_HB) | (var2&MaskForMode_LB);
#define WriteRegsiterFlag(Mode) (Mode & 0x40) >>6
#define FunctionMode(Mode) ((Mode) & 0x3F)
#endif

#define NowHW Curry

#if (NowHW == Curry)
//0:nothing before HDR IP
//1:IP2 CSC before HDR IP
#define HW_PIPE_MODE 0

#define HW_PIPE_CONSTRAINTS 1

#define HW_NO_BT2020CL
#define HW_NO_XVYCC

//#define SW_PRECONS_PLACE 1

#define HW_PIPE_RANGE CFD_full
#define HE_PIPE_PRECONSTRAINTS
#define HE_PIPE_POST_CONSTRAINTS

#define PRECONSTRAINTS_REFERTO_EDID 1
#if (0 == RealChip)
#include "Color_map_driver_lib_Curry.h"
#endif
#endif

//controls for TMO curve algorithm
typedef struct _STU_IN_CFD_TMO_CONTROL
{

    MS_U16 u16SourceMax;          //target maximum in nits, 1~10000
    MS_U8  u16SourceMaxFlag;      //flag of target maximum
                                  //0: the unit of u16Luminance is 1 nits
                                  //1: the unit of u16Luminance is 0.0001 nits

    MS_U16 u16SourceMed;          //target minimum in nits, 1~10000
    MS_U8  u16SourceMedFlag;      //flag of target maximum
                                  //0: the unit of u16Luminance is 1 nits
                                  //1: the unit of u16Luminance is 0.0001 nits

    MS_U16 u16SourceMin;          //target minimum in nits, 1~10000
    MS_U8  u16SourceMinFlag;      //flag of target minimum
                                  //0: the unit of u16Luminance is 1 nits
                                  //1: the unit of u16Luminance is 0.0001 nits

    MS_U16 u16TgtMax;             //target maximum in nits, 1~10000
    MS_U8  u16TgtMaxFlag;         //flag of target maximum
                                  //0: the unit of u16Luminance is 1 nits
                                  //1: the unit of u16Luminance is 0.0001 nits

    MS_U16 u16TgtMed;             //target maximum in nits, 1~10000
    MS_U8  u16TgtMedFlag;         //flag of target minimum
                                  //0: the unit of u16Luminance is 1 nits
                                  //1: the unit of u16Luminance is 0.0001 nits

    MS_U16 u16TgtMin;             //target minimum in nits, 1~10000
    MS_U8  u16TgtMinFlag;         //flag of target minimum
                                  //0: the unit of u16Luminance is 1 nits
                                  //1: the unit of u16Luminance is 0.0001 nits

    //MS_U8 u8TMO_TargetMode      //0 : keeps the value in initial function
                                  //1 : from output source

    //0: TMO in PQ to PQ
    //1: TMO in Gamma to Gamma
    //2-255 reserved
    MS_U8  u8TMO_domain_mode;


    //for max/min, 3 stages for priority
    //for med , 2 stages for priority

    //for TMO parameter control method extension
    MS_U8 u8TMO_SourceUserModeEn; //0: auto
                                  //priority : infoFrame/SEI > driver > CFD default
                                  //1: force, refer to u8SourceUserMode
    //write
    MS_U8 u8TMO_SourceUserMode;   //0: refer to infoFrame/SEI
                                  //1: refer to driver
                                  //2: refer to CFD default
    //read for max/min
    MS_U8 u8TMO_SourceStatus;     //0: refer to infoFrame/SEI
                                  //1: refer to driver
                                  //2: refer to CFD default

    //read for med only
    MS_U8 u8TMO_SourceStatusM;    //0: refer to infoFrame/SEI
                                  //1: refer to driver
                                  //2: refer to CFD default


    MS_U8 u8TMO_TargetUserModeEn; //0: auto
                                  //priority : EDID > driver > CFD default
                                  //1: force, refer to u8SourceUserMode
    //write
    MS_U8 u8TMO_TargetUserMode;   //0: refer to EDID
                                  //1: refer to driver
                                  //2: refer to CFD default

    //read for max/min
    MS_U8 u8TMO_TargetStatus;     //0: refer to infoFrame
                                  //1: refer to driver
                                  //2: refer to CFD default

    //read for med only
    MS_U8 u8TMO_TargetStatusM;     //0: refer to infoFrame
                                  //1: refer to driver
                                  //2: refer to CFD default


    MS_U8 u8TMO_TargetRefer_Mode;


} STU_IN_CFD_TMO_CONTROL;

//for color primaries and white point defined in colorimetry
typedef struct _STU_CFD_COLORIMETRY
{
  //the default order is R->G->B
  MS_U16 u16Display_Primaries_x[3];      //data *0.00002 0xC350 = 1
  MS_U16 u16Display_Primaries_y[3];      //data *0.00002 0xC350 = 1
  MS_U16 u16White_point_x;               //data *0.00002 0xC350 = 1
  MS_U16 u16White_point_y;               //data *0.00002 0xC350 = 1

} STU_CFD_COLORIMETRY;

typedef struct _STU_CFD_MS_ALG_COLOR_FORMAT
{
    //color space
    //assign with E_CFD_CFIO

    //E_CFD_CFIO_RGB
    //E_CFD_CFIO_YUV
    //E_CFD_CFIO
    MS_U8 u8Input_Format;

    //E_CFD_MC_FORMAT
    MS_U8 u8Input_DataFormat;

    //limit/full
    //assign with E_CFD_CFIO_RANGE
    //0:limit 1:full
    MS_U8 u8Input_IsFullRange;

    //SDR/HDR
    //0:SDR
    //1:HDR1
    //2:HDR2
    MS_U8 u8Input_HDRMode;

    //Only for TV
    //0:PQ path
    //1:bypass CSC
    MS_U8 u8Input_IsRGBBypass;

    //special control
    //especially for Maserati
    //0: bypass all SDR IP
    //1: HDR IP by decision tree or user
    //2: bypass all SDR IP besides OutputCSC, for Panel out case
    MS_U8 u8Input_SDRIPMode;

    //0: bypass all HDR IP
    //1: HDR IP by decision tree or user
    //2: set HDR IP to open HDR
    //3: set HDR IP to Dolby HDR
    MS_U8 u8Input_HDRIPMode;

    //MS_U8 u8Input_IsHDRIPFromDolbyDriver;

    //follow E_CFD_CFIO_GAMUTORDER_IDX
    //use for non-panel output
    MS_U8 u8Input_GamutOrderIdx;


    //E_CFD_CFIO_RGB
    //E_CFD_CFIO_YUV
    //E_CFD_CFIO
    MS_U8 u8Output_Format;

    MS_U8 u8Output_DataFormat;

    MS_U8 u8Output_GamutOrderIdx;
    //follow E_CFD_CFIO_GAMUTORDER_IDX
    //use for non-panel output

    MS_U8 u8Output_IsFullRange;

    //SDR/HDR
    //0:SDR
    //1:HDR1
    //2:HDR2
    MS_U8 u8Output_HDRMode;

    //Temp_Format[0] : output of IP2 CSC, input of HDR IP
    //Temp_Format[1] : output of HDR IP, input of SDR IP
    MS_U8 u8Temp_Format[2];

    //E_CFD_MC_FORMAT
    MS_U8 u8Temp_DataFormat[2];

    //E_CFD_CFIO_RANGE
    MS_U8 u8Temp_IsFullRange[2];

    //E_CFIO_HDR_STATUS
    MS_U8 u8Temp_HDRMode[2];

    MS_U8 u8Temp_GamutOrderIdx[2];

    //redefinition
    //item 0-10 are the same as table E.3 in HEVC spec
    MS_U8 u8InputColorPriamries;
    MS_U8 u8OutputColorPriamries;
    MS_U8 u8TempColorPriamries[2];
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
    //11:AdobeRGB
    //255:undefined

    //redefinition
    //item 0-17 are the same as table E.4 in HEVC spec
    MS_U8 u8InputTransferCharacterstics;
    MS_U8 u8OutputTransferCharacterstics;
    MS_U8 u8TempTransferCharacterstics[2];
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
    //18   AdobeRGB
    //255  undefined

    //the same the same as table E.5 in HEVC spec
    MS_U8 u8InputMatrixCoefficients;
    MS_U8 u8OutputMatrixCoefficients;
    MS_U8 u8TempMatrixCoefficients[2];
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
    //255:Reserverd

    //for process status
    //In SDR IP
    MS_U8 u8DoTMO_Flag;
    MS_U8 u8DoGamutMapping_Flag;
    MS_U8 u8DoDLC_Flag;
    MS_U8 u8DoBT2020CLP_Flag;

    //In HDR IP
    MS_U8 u8DoTMOInHDRIP_Flag;
    MS_U8 u8DoGamutMappingInHDRIP_Flag;
    MS_U8 u8DoDLCInHDRIP_Flag;
    MS_U8 u8DoBT2020CLPInHDRIP_Flag;
    //MS_U8 u8DoFull2LimitInHDRIP_Flag;
    MS_U8 u8DoForceEnterHDRIP_Flag;
    //MS_U8 u8DoForceFull2LimitInHDRIP_Flag;
    MS_U8 u8DoHDRXtoHDRY_Flag;
    MS_U8 u8DoSDRtoHDR_Flag;

    //0 : no bypass
    //1 : HDR PQ bypass
    //2 : HDR HLG bypass
    MS_U8 u8DoHDRbypassInHDRIP_Flag;
    //MS_U8 u8DoHDRHLGbypassInHDRIP_Flag;

    //for process status2
    MS_U8 u8DoMMIn_ForceHDMI_Flag;
    MS_U8 u8DoMMIn_Force709_Flag;
    MS_U8 u8DoHDMIIn_Force709_Flag;
    MS_U8 u8DoOtherIn_Force709_Flag;
    MS_U8 u8DoOutput_Force709_Flag;

    //for process status 3
    MS_U8 u8DoHDRIP_Forcebypass_Flag;
    MS_U8 u8DoSDRIP_ForceNOTMO_Flag;
    MS_U8 u8DoSDRIP_ForceNOGM_Flag;
    MS_U8 u8DoSDRIP_ForceNOBT2020CL_Flag;

    //
    MS_U8 u8DoPreConstraints_Flag;

    //E_CFD_CFIO_RANGE_LIMIT
    //E_CFD_CFIO_RANGE_FULL
    MS_U8 u8DoPathFullRange_Flag;

    //for process status 3
    //MS_U8 u8Output_format_ForcedChange_Flag;

    //HW Patch
    MS_U8 u8HW_PatchMode;

    //temp variable for debug
    MS_U16 u16_check_status;

    //other interior structure
    STU_IN_CFD_TMO_CONTROL stu_CFD_TMO_Param;

    //Main/sub control from outside
    MS_U8 u8HW_MainSub_Mode;

    //input source from outside API
    MS_U8 u8Input_Source;

    STU_CFD_COLORIMETRY *pstu_Panel_Param_Colorimetry;

    STU_CFD_COLORIMETRY stu_source_mastering_display;

    //0: need to do
    //1: not need to do
    MS_U8 u8VideoADFlag;

    //1: input colorimetry can not bypass
    //0: input colorimetry can bypass
    MS_U8 u8MustGMFlag;

    //1: happens
    //0: not happens
    MS_U8 u8XVYCC2SRGBFlag;

} STU_CFD_MS_ALG_COLOR_FORMAT;


//follow the order of gamut area
//gamut of colorspace.xlsx
typedef enum _E_CFD_CFIO_GAMUTORDER_IDX
{
    E_CFD_CFIO_GT_BT601_525  = 0x0, //used colorspace has min gamut
                                    //SMPTE 170M, SMPTE 240M

    E_CFD_CFIO_GT_BT709      = 0x1, //sRGB
    E_CFD_CFIO_GT_BT601_625  = 0x2,
    E_CFD_CFIO_GT_ADOBERGB   = 0x3, //AdobeRGB/YUV
    E_CFD_CFIO_GT_DCI_P3     = 0x4, //DCI P3
    E_CFD_CFIO_GT_BT4706     = 0x5, //BT470_6
    E_CFD_CFIO_GT_GF         = 0x6, //Generic film
    E_CFD_CFIO_GT_XVYCC      = 0x7, //sYCC601,xvYCC601,xvYCC709
    E_CFD_CFIO_GT_BT2020     = 0x8, //BT2020
    E_CFD_CFIO_GT_XYZ        = 0x9, //CIE XYZ

    E_CFD_CFIO_GT_PANEL_reservedstart,//Panel or user-defined gamut

}E_CFD_CFIO_GAMUTORDER_IDX;

typedef enum _E_CFD_IP_CSC_PROCESS
{
    E_CFD_IP_CSC_OFF  = 0x0,
    E_CFD_IP_CSC_RFULL_TO_RLIMIT  = 0x1,
    E_CFD_IP_CSC_RFULL_TO_YFULL   = 0x2,
    E_CFD_IP_CSC_RFULL_TO_YLIMIT  = 0x3,
    E_CFD_IP_CSC_RLIMIT_TO_RFULL   = 0x4,
    E_CFD_IP_CSC_RLIMIT_TO_YFULL   = 0x5,
    E_CFD_IP_CSC_RLIMIT_TO_YLIMIT  = 0x6,
    E_CFD_IP_CSC_YFULL_TO_RFULL   = 0x7,
    E_CFD_IP_CSC_YFULL_TO_RLIMIT  = 0x8,
    E_CFD_IP_CSC_YFULL_TO_YLIMIT  = 0x9,
    E_CFD_IP_CSC_YLIMIT_TO_RFULL   = 0xA,
    E_CFD_IP_CSC_YLIMIT_TO_RLIMIT  = 0xB,
    E_CFD_IP_CSC_YLIMIT_TO_YFULL   = 0xC

}E_CFD_IP_CSC_PROCESS;

typedef enum _E_CFD_IP_STATUS_RETURN
{
    E_CFD_IP_STATUS_PROCESSOK            = 0x0,
    E_CFD_IP_STATUS_PARTIALPROCESS       = 0x1,
    E_CFD_IP_STATUS_PARAMETER_ERR        = 0x2,
    E_CFD_IP_STATUS_PRODUCESETTING_ERR   = 0x3,
    E_CFD_IP_STATUS_WRITEREG_ERR         = 0x4

}E_CFD_IP_STATUS_RETURN;

typedef enum _E_CFD_HWIP_PIPE_ORDER
{
    E_CFD_NOTHING_BEFORE_HDRIP      = 0x0,
    E_CFD_IP2CSC_BEFORE_HDRIP       = 0x1,

}E_CFD_HWIP_PIPE_ORDER;

typedef enum _E_CFD_OSD_MODE
{
    E_CFD_OSD_SOURCE_CHANGE           = 0x0,
    E_CFD_OSD_CONTROL_ONLY       = 0x1,

}E_CFD_OSD_MODE;


MS_U8 Mapi_Cfd_checkColorprimaryIsOk(MS_U8 *pu8Color_primary);

MS_U8 MS_Cfd_checkColorprimaryIsUndefined(MS_U8 *pu8Color_primary);

MS_U8 MS_Cfd_CheckOpenHDR(MS_U8 u8HDR_mode);

//function prototype
MS_U32 MDrv_HDR_PQ(MS_U16 u16In);

MS_U8 MS_Cfd_InputFormat_SetOutTR(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam);

//set DoGM flag & DoTMO flag
MS_U16 MS_Cfd_Maserati_SetGM_TMO_Flags(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam);


void MS_Cfd_SetOutputColorParam_HDMISink(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_input, MS_U8 u8mode);

//initialize control structure in color format driver
void MS_IN_Cfd_TMO_Control_Param_Init(STU_IN_CFD_TMO_CONTROL *pst_TMO_Control_Param);

//force input to Rec 709
void MS_Cfd_ForceMMInputToRec709(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam, MS_U8 u8Current_Transfer_Characteristic);

//======================================================================================
//based on "DLC(TMO)_range_convert_patch.xlsx"
//use TMO to do Ygain/offset
//if input is Y limit, after TMO with TMO_curve_patch[18] , the output is Y full

MS_U16 MS_Cfd_KanoPatch_TMO_For_LimitIn(MS_U8 *TMO_curve, MS_U8 *TMO_curve_patch);
//parameters
//MS_U8 TMO_curve : pointer to TMO_curve[18]
//MS_U8 TMO_curve_patch : pointer to TMO_curve_patch[18]

//return valle
//======================================================================================
//tranfer luminance(nits) to PQ code

MS_U16 MS_Cfd_Luminance_To_PQCode(MS_U16 u16Luminance, MS_U8 u8Function_Mode);
//parameters
//MS_U16 u16Luminance Q10.0

//MS_U8 u8Function_Mode Q8.0
//0: the unit of u16Luminance is 1 nits
//1: the unit of u16Luminance is 0.0001 nits

//return value
//MS_U16 PQ_code Q0.10

//contraints
//1<=u16Luminance<=10000
//0<=u8Function_Mode<=1

//=======================================================================================

MS_U8 MS_Cfd_InputFormat_SetColorMetry(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_ControlParam,
    MS_U8 U8FormatArray_Idx
    );

//prupose
//set colormetry due to U8FormatArray_Idx(0/1)

//set temp format before HDR IP and after HDR IP

//=======================================================================================

#if ((NowHW == Curry) ||(NowHW == Kastor))

MS_U16 MS_Cfd_Curry_TMO_Control(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,
    //STU_IN_CFD_TMO_CONTROL       *pstu_TMO_control,
    STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input
    );

MS_U16 MS_Cfd_Curry_HDRIP_Control(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,
    STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param,
    STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input);

MS_U16 MS_Cfd_Curry_SDRIP_Control(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,
    STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param,
    STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input,
    MS_U8 u8MainSubMode);

#endif

#if (NowHW == Kano)

MS_U16 MS_Cfd_Kano_TMO_Control(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,
    //STU_IN_CFD_TMO_CONTROL       *pstu_TMO_control,
    STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input
    );

MS_U16 MS_Cfd_Kano_HDRIP_Control(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,
    STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param,
    STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input);

MS_U16 MS_Cfd_Kano_SDRIP_Control(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,
    STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param,
    STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input,
    MS_U8 u8MainSubMode);

#endif

#if (NowHW == Manhattan)

MS_U16 MS_Cfd_Manhattan_TMO_Control(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,
    //STU_IN_CFD_TMO_CONTROL       *pstu_TMO_control,
    STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input
    );

MS_U16 MS_Cfd_Manhattan_HDRIP_Control(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,
    STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param
    );

MS_U16 MS_Cfd_Manhattan_SDRIP_Control(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,
    STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param,
    STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input,MS_U8 u8MainSubMode);

#endif

#if (NowHW == Maserati) || (NowHW == Mainz)

MS_U16 MS_Cfd_Maserati_DLC_Control(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,
    STU_CFD_MS_ALG_INTERFACE_DLC *pstu_DLC_Input,
    STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input);


MS_U16 MS_Cfd_Maserati_TMO_Control(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,
    //STU_IN_CFD_TMO_CONTROL       *pstu_TMO_control,
    STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input
    );

MS_U16 MS_Cfd_Maserati_HDRIP_Control(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,
    STU_CFD_MS_ALG_INTERFACE_HDRIP *pstu_HDRIP_Param,
    STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input);

MS_U16 MS_Cfd_Maserati_SDRIP_Control(
    STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,
    STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param,
    STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input,MS_U8 u8MainSubMode);

MS_U16 MS_Cfd_Maserati_ColorSampling_Control(STU_CFD_MS_ALG_COLOR_FORMAT * pstu_Control_Param);
MS_U8 MS_Cfd_Maserati_InputCSC_Decision(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param);

#endif

#if (NowHW == U13_Universe)

MS_U16 MS_Cfd_Universe_SDRIP_Control(STU_CFD_MS_ALG_COLOR_FORMAT *pstu_Control_Param,STU_CFD_MS_ALG_INTERFACE_SDRIP *pstu_SDRIP_Param,STU_CFD_MS_ALG_INTERFACE_TMO *pstu_TMO_Input);

#endif
//=======================================================================================
//decision tree for Kano

//0: OFF
//1: ON
INTERFACE MS_U8 MS_Cfd_OnlyEnable_Decision_OFF(void);
INTERFACE MS_U8 MS_Cfd_OnlyEnable_Decision_ON(void);

#undef INTERFACE
#endif //_HAL_COLOR_FORMAT_DRIVER_H
