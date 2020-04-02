////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2008 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _MHALTVENCODER_H
#define _MHALTVENCODER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "MsCommon.h"


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

//Video Encoder Capability
#define VE_SUPPORTED    TRUE
#define ENABLE_VE_SUBTITLE  1
#define BYTE_PER_WORD           16

#define VE_SUPPORT_SOURCE_SCALER_OP 0

#define REG_CKG_VE_IN           0x1E38
#define CKG_VE_IN_GATED         BIT(0)
#define CKG_VE_IN_INVERT        BIT(1)
#define CKG_VE_IN_MASK          (BIT(5) | BIT(4) | BIT(3) | BIT(2))
#define CKG_VE_IN_CLK_ADC       (0 << 2)
#define CKG_VE_IN_CLK_DVI       (1 << 2)
#define CKG_VE_IN_CLK_VD        (2 << 2)
#define CKG_VE_IN_CLK_MPEG0     (3 << 2)
#define CKG_VE_IN_1             (4 << 2) //IDclk2 for IP_MAIN
#define CKG_VE_IN_CLK_EXT_DI    (5 << 2)
#define CKG_VE_IN_0             (6 << 2) //Sub MPEG0
#define CKG_VE_IN_0_            (7 << 2) //ODclk
#define CKG_VE_IN_DFT_LIVE      (8 << 2)

#define VE_NTSC_FRAME_SIZE  ((MS_U32)(720ul * 480ul)>>3) // 720*480*16bits/64bits
#define VE_PAL_FRAME_SIZE   ((MS_U32)(720ul * 576ul)>>3) // 720*576*16bits/64bits

 //VE out
#define VE_OUT_CVBS_YCC         0x00
#define VE_OUT_CVBS_YCbCr       0x01
#define VE_OUT_CVBS_RGB         0x02
#define VE_OUT_NONE             0xFF

#define VE_V_SCALE_DOWN_RATIO(IN, OUT)  (((MS_U32)(OUT) * 2048ul / (MS_U32)(IN))+1)
#define VE_H_SCALE_DOWN_RATIO(IN, OUT)  ((MS_U32)(OUT) * 2048ul / (MS_U32)(IN))
#define VE_V_SCALE_UP_RATIO(IN, OUT)  ((MS_U32)(IN - 1) * 0x10000ul / (MS_U32)(OUT - 1))
#define VE_H_SCALE_UP_RATIO(IN, OUT)  ((MS_U32)(IN - 1) * 0x10000ul/ (MS_U32)(OUT - 1))

typedef enum
{
    EN_VE_DEMODE         = 0x01,
    EN_VE_CCIR656_IN     = 0x02,
    EN_VE_RGB_IN         = 0x04,
    EN_VE_INVERSE_HSYNC  = 0x08,
}EN_VE_DISPLAY_STATUS;

//MV Type
typedef enum
{
    MS_VE_MV_NTSC_START = 0,
    MS_VE_MV_NTSC_TYPE1 = MS_VE_MV_NTSC_START,
    MS_VE_MV_NTSC_TYPE2,
    MS_VE_MV_NTSC_TYPE3,
    MS_VE_MV_PAL_START = 0x10,
    MS_VE_MV_PAL_TYPE1 = MS_VE_MV_PAL_START,
    MS_VE_MV_PAL_TYPE2,
    MS_VE_MV_PAL_TYPE3,
} MS_VE_MV_TYPE;

//VE capability struct
typedef struct
{
    MS_BOOL bSupport_UpScale;
    MS_BOOL bSupport_CropMode;
}MS_VE_Cap;

typedef enum
{
    MS_VE_DRAM_FORMAT_Y8C8 = 0,
    MS_VE_DRAM_FORMAT_Y8C8M4,
}MS_VE_DRAM_FORMAT;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

void MDrv_VE_PowerOn(void);
void MDrv_VE_PowerOff(void);
void Hal_VE_set_fix_color_out(MS_BOOL ben, MS_U8 u8color_y, MS_U8 u8color_cb, MS_U8 u8color_cr);

void Hal_VE_set_wss_data(MS_BOOL ben, MS_U16 u16wssdata);
MS_U16 Hal_VE_get_wss_data(void);
void Hal_VE_set_color_convert(MS_VE_VIDEOSYS VideoSys, MS_U8 u8DACType);

void Hal_VE_init(void);
MS_BOOL Hal_VE_set_output_video_std(MS_VE_VIDEOSYS VideoSystem);
MS_VE_VIDEOSYS Hal_VE_Get_Output_Video_Std(void);


void Hal_VE_set_capture_window(MS_U16 u16hstart, MS_U16 u16hend, MS_U16 u16vstart, MS_U16 u16vend);
void Hal_VE_set_hsync_inverse(MS_BOOL bEn);
void Hal_VE_set_cvbs_buffer_out(MS_U16 u16chnl, MS_U16 u16mux, MS_U16 u16clamp, MS_U16 u16test);
void Hal_VE_set_frc(MS_BOOL bfulldrop, MS_U32 u32fullnum, MS_U32 u32emptynum, MS_BOOL binterlace, MS_BOOL b3FrameMode);
void Hal_VE_set_field_inverse(MS_BOOL ben);
void Hal_VE_set_ccir656_out_pal(MS_BOOL bpal);
void Hal_VE_sofeware_reset(MS_BOOL ben);
void Hal_VE_set_vbi(MS_BOOL ben);
void Hal_VE_set_reg_load(MS_BOOL ben);
void Hal_VE_set_ctrl(MS_U16 u16enval);
void Hal_VE_set_h_scaling(MS_BOOL ben, MS_U16 u16ratio);
void Hal_VE_set_v_scaling(MS_BOOL ben, MS_U16 u16ratio);
void Hal_VE_set_v_scaling_Traditional(MS_BOOL ben, MS_U16 u16ratio);
void Hal_VE_set_out_sel(MS_U8 u8out_sel);
void Hal_VE_set_source_sync_inv(MS_U8 u8val, MS_U8 u8Mask);
void Hal_VE_set_sog(MS_BOOL ben);
void Hal_VE_set_inputsource(PMS_VE_InputSrc_Info pInputSrcInfo);
void Hal_VE_set_mux(MS_U8 u8clk, MS_U8 u8data);
void Hal_VE_set_rgb_in(MS_BOOL ben);
void Hal_VE_set_ccir656_in(MS_BOOL ben);
void Hal_VE_set_source_interlace(MS_BOOL ben);
void Hal_VE_set_clk_on_off(MS_BOOL ben);
void Hal_VE_set_ve_on_off(MS_BOOL ben);
void Hal_VE_set_blackscreen(MS_BOOL ben);
MS_BOOL Hal_VE_is_blackscreen_enabled(void);
void Hal_VE_init_riu_base(MS_U32 u32riu_base);
void Hal_VE_EnableTtx(MS_BOOL bEnable);
void Hal_VE_Set_ttx_Buffer(MS_U32 u32StartAddr, MS_U32 u32Size);
void Hal_VE_Clear_ttxReadDoneStatus(void);
MS_BOOL Hal_VE_Get_ttxReadDoneStatus(void);
void Hal_VE_Set_VbiTtxActiveLines(MS_U8 u8LinePerField);
void Hal_VE_SetVbiTtxRange(MS_U16 odd_start, MS_U16 odd_end,
                         MS_U16 even_start, MS_U16 enen_end);
void Hal_VE_Set_VbiTtxActiveLinesBitmap(MS_U32 u32Bitmap);
void Hal_VE_Set_winodw_offset(MS_U32 u32offsetaddr);
void Hal_VE_Set_OSD(MS_BOOL bEnable);
void Hal_VE_EnableCcSw(MS_BOOL bEnable);
void Hal_VE_SetCcRange(MS_U16 odd_start, MS_U16 odd_end, MS_U16 even_start, MS_U16 enen_end);
void Hal_VE_SendCcData(MS_BOOL bIsOdd, MS_U16 data);
void Hal_VE_Set_TestPattern(MS_BOOL ben);

void Hal_VE_W2BYTE_MSK(MS_U32 u32Reg, MS_U16 u16Val, MS_U16 u16Mask);
MS_U16 Hal_VE_R2BYTE_MSK(MS_U32 u32Reg, MS_U16 u16Mask);

//Dump Table
void Hal_VE_DumpTable(MS_U8 *pVeTable, MS_U8 u8TableType);

//Set MV
VE_Result Hal_VE_SetMV(MS_BOOL bEnable, MS_VE_MV_TYPE eMvType);

MS_BOOL Hal_VE_DisableRegWrite_GetCaps(void);

// show internal color bar
// TRUE: enable color bar
// FALSE: disable color bar
void Hal_VE_ShowColorBar(MS_BOOL bEnable);
VE_Result Hal_VE_AdjustPositionBase(MS_S32 s32WAddrAdjustment, MS_S32 s32RAddrAdjustment);
void Hal_VE_set_field_size(MS_U16 u16FieldSize);
void Hal_VE_SelMIU(MS_BOOL bMIU1);
void Hal_VE_SetMemAddr(MS_U32 *pu32RDAddress, MS_U32 *pu32WRAddress);
VE_Result HAL_VE_SetFrameLock(MS_U32 u32IDclk, MS_U32 u32ODclk, MS_U32 u32InitPll, MS_BOOL bEnable);
VE_Result Hal_VE_GetCaps(MS_VE_Cap *cap);
void Hal_VE_set_h_upscaling(MS_BOOL ben, MS_U32 u32ratio);
void Hal_VE_set_v_upscaling(MS_BOOL ben, MS_U32 u32ratio);
MS_VE_DRAM_FORMAT Hal_VE_Get_DRAM_Format(void);
void Hal_VE_Set_DRAM_Format(MS_VE_DRAM_FORMAT fmt);
MS_U8 Hal_VE_Get_Pack_Num_Per_Line(void);
void Hal_VE_Set_Pack_Num_Per_Line(MS_U8 PackNumPerLine);
MS_U16 Hal_VE_Get_Field_Line_Number(void);
void Hal_VE_Set_Field_Line_Number(MS_U16 FieldLineNumber);
void Hal_VE_Set_Crop_OSD_Offset(MS_U16 hstart, MS_U16 vstart);
void Hal_VE_Set_VScale_Output_Line_Number(MS_U16 line);
void Hal_VE_Set_HScale_Output_Line_Number(MS_U16 line);
VE_Result Hal_VE_Adjust_FrameStart(MS_S16 pixel_offset);

#ifdef __cplusplus
}
#endif

#endif

