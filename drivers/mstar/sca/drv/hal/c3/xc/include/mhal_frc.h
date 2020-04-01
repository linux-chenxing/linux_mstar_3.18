////////////////////////////////////////////////////////////////////////////////

//

// Copyright (c) 2006-2007 MStar Semiconductor, Inc.

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



#ifndef MHAL_FRC_H

#define MHAL_FRC_H



#include "hwreg_frc.h"



#ifdef MHAL_FRC_C

#define INTERFACE

#else

#define INTERFACE extern

#endif



//-------------------------------------------------------------------------------------------------

//  Macro and Define

//------------------------------------------------------------------------------------------------- 

#define CRYSTAL_CLOCK    12000000ul//24000000ul

#define MST_CLOCK_HZ    CRYSTAL_CLOCK

#define MST_CLOCK_KHZ    (MST_CLOCK_HZ / 1000)

#define MST_CLOCK_MHZ    (MST_CLOCK_KHZ / 1000)







#if 0

//==============================================================================

//==============================================================================

//--#############################################

//--include PQ & StageTable

//--#############################################



#if (PANEL_3D_TYPE == PANEL_PASSIVE_120)

#if 0//(IS_MEMORY_TYPE_DDR2)

#include  "Ursa5_120_Ln_alt_PASSIVE_1chip_DDR2.c"

#include  "Ursa5_FHD_120_ACTIVE_1Chip.c"

#include  "Ursa5_StageTable_120_ACTIVE.c"

#include  "Ursa5_StageReg_120_ACTIVE.c"

#else

#include  "Ursa5_120_Ln_alt_PASSIVE_1chip.c"

#include  "Ursa5_120_Ln_alt_PASSIVE_1chip_Chris.c"//for Chris mode

#include  "Ursa5_StageTable_120_PASSIVE.c"

#include  "Ursa5_StageReg_120_PASSIVE.c"

#include  "Ursa5_FHD_120_ACTIVE_1Chip.c"

#include  "Ursa5_StageTable_120_ACTIVE.c"

#include  "Ursa5_StageReg_120_ACTIVE.c"

#endif

#elif ((PANEL_3D_TYPE == PANEL_ACTIVE_120) || (PANEL_3D_TYPE == PANEL_ACTIVE_120_VBI))

#include  "Ursa5_FHD_120_ACTIVE_1Chip.c"

#include  "Ursa5_StageTable_120_ACTIVE.c"

#include  "Ursa5_StageReg_120_ACTIVE.c"

#elif (PANEL_3D_TYPE == PANEL_PDP_60)

#include  "Ursa5_FHD_PDP_1Chip.c"

#include  "Ursa5_StageTable_120_ACTIVE.c"

#include  "Ursa5_StageReg_120_ACTIVE.c"

#elif 0//((PANEL_3D_TYPE == PANEL_PASSIVE_120_AU58_2560)&&(TWO_CHIP_TYPE == DRV_2CHIP_SEL_MASTER))

#include  "Ursa5_2560_PASSIVE_2chip_Master.c"

#include  "Ursa5_StageTable_120_PASSIVE.c"

#include  "Ursa5_StageReg_120_PASSIVE.c"

#include  "Ursa5_FHD_120_ACTIVE_1Chip.c"

#include  "Ursa5_StageTable_120_ACTIVE.c"

#include  "Ursa5_StageReg_120_ACTIVE.c"

#elif 0//((PANEL_3D_TYPE == PANEL_PASSIVE_120_AU58_2560)&&(TWO_CHIP_TYPE == DRV_2CHIP_SEL_SLAVE))

#include  "Ursa5_2560_PASSIVE_2chip_Slave.c"

#include  "Ursa5_StageTable_120_PASSIVE.c"

#include  "Ursa5_StageReg_120_PASSIVE.c"

#include  "Ursa5_FHD_120_ACTIVE_1Chip.c"

#include  "Ursa5_StageTable_120_ACTIVE.c"

#include  "Ursa5_StageReg_120_ACTIVE.c"

#elif 0//((PANEL_3D_TYPE == PANEL_ACTIVE_240)&&(TWO_CHIP_TYPE == DRV_2CHIP_SEL_MASTER))

#include  "Ursa5_FHD_240_ACTIVE_2Chip_Master.c"

#include  "Ursa5_StageTable_120_ACTIVE.c"

#include  "Ursa5_StageReg_120_ACTIVE.c"

#elif 0//((PANEL_3D_TYPE == PANEL_ACTIVE_240)&&(TWO_CHIP_TYPE == DRV_2CHIP_SEL_SLAVE))

#include  "Ursa5_FHD_240_ACTIVE_2Chip_Slave.c"

#include  "Ursa5_StageTable_120_ACTIVE.c"

#include  "Ursa5_StageReg_120_ACTIVE.c"

#elif (PANEL_3D_TYPE == PANEL_PASSIVE_60)

#include  "Ursa5_60_Ln_alt_PASSIVE_1chip.c"

#include  "Ursa5_StageTable_120_PASSIVE.c"

#include  "Ursa5_StageReg_120_PASSIVE.c"

#include  "Ursa5_FHD_120_ACTIVE_1Chip.c"

#include  "Ursa5_StageTable_120_ACTIVE.c"

#include  "Ursa5_StageReg_120_ACTIVE.c"

#endif



#endif

//-------------------------------------------------------------------------------------------------

//  Structure enum

//-------------------------------------------------------------------------------------------------



typedef enum 

{

    FRC_IP_MEM_IP_YC_8              = 0x00,

    FRC_IP_MEM_IP_YC_10             = 0x01,

    FRC_IP_MEM_IP_RGB_8             = 0x02,

    FRC_IP_MEM_IP_RGB_10            = 0x03,

    FRC_IP_MEM_IP_YC_10_SPECIAL  = 0x04,

    FRC_IP_MEM_IP_YUV_8             = 0x05,

    FRC_IP_MEM_IP_RGB_10_SPECIAL = 0x06,

    FRC_IP_MEM_IP_YUV_10_SPECIAL = 0x07,

    

} FRC_IP_MEM_MODE_e;



typedef enum

{

    FRC_MIRROR_OFF,

    FRC_MIRROR_H_MODE,

    FRC_MIRROR_V_MODE,

    FRC_MIRROR_HV_MODE

} FRC_MirrorModeType_e;



typedef enum 
{
    E_FRC_op2_gamma_table = 1,  // gamma table
    E_FRC_od_table                = 2, // OD table
    E_FRC_ld_table                 = 3, // Local diming table
} FRC_CLIENT_TABLE;

//-------------------------------------------------------------------------------------------------

//  Function and Variable

//-------------------------------------------------------------------------------------------------
#ifdef MSOS_TYPE_CE
#define MHal_FRC_IsRGB(...) 0



#define MHal_FRC_TGEN_SWReset(...)

#define MHal_FRC_TGEN_DoubleBuf(...)

#define MHal_FRC_TGEN_SetVTotal(...)

#define MHal_FRC_TGEN_SetHTotal(...)

#define MHal_FRC_TGEN_SetVTrigY(...)

#define MHal_FRC_TGEN_SetVTrigX(...)

#define MHal_FRC_TGEN_SetVSyncStartEndY(...)

#define MHal_FRC_TGEN_SetHSyncStartEndX(...)

#define MHal_FRC_TGEN_SetFdeStartEndY(...)

#define MHal_FRC_TGEN_SetFdeStartEndX(...)

#define MHal_FRC_TGEN_SetMdeStartEndY(...)

#define MHal_FRC_TGEN_SetMdeStartEndX(...)

#define MHal_FRC_TGEN_FpllRefPointY(...)

#define MHal_FRC_TGEN_1Clk2PixOut(...)

#define MHal_FRC_TGEN_SetSyncHcnt(...)

#define MHal_FRC_TGEN_SetClrSyncAhead(...)



#define MHal_FRC_IPM_SoftwareReset(...)

#define MHal_FRC_IPM_RWEn(...)

#define MHal_FRC_IPM_RW_CEN_Select(...)

#define MHal_FRC_IPM_SetYuv10Bit(...)

#define MHal_FRC_IPM_SetMr(...)

#define MHal_FRC_IPM_SetMemoryMode(...)

#define MHal_FRC_IPM_YC444To422Control(...)

#define MHal_FRC_IPM_SetFrameBufferNum(...)

#define MHal_FRC_IPM_CeLineCountEn(...)

#define MHal_FRC_IPM_SetBaseAddr(...)

#define MHal_FRC_IPM_SetIp2Mc(...)

#define MHal_FRC_IPM_SetYCoutLinePitch(...)

#define MHal_FRC_IPM_SetReadFetchNumber(...)

#define MHal_FRC_IPM_SetWriteFetchNumber(...)

#define MHal_FRC_IPM_SetRfifoThr(...)

#define MHal_FRC_IPM_SetWfifoThr(...)

#define MHal_FRC_IPM_SetReadLength(...)

#define MHal_FRC_IPM_SetWriteLength(...)

#define MHal_FRC_IPM_SetHTotalPixellimit(...)

#define MHal_FRC_IPM_SetVTotalPixellimit(...)

#define MHal_FRC_IPM_SetRmaskNum(...)

#define MHal_FRC_IPM_SetWmaskNum(...)

#define MHal_FRC_IPM_SetMirrorMode(...)

#define MHal_FRC_IPM_PacketInitCnt(...)

#define MHal_FRC_IPM_SetLvdsInputMode(...)

#define MHal_FRC_IPM_SetOsdWinIdx(...)

#define MHal_FRC_IPM_SetVPulseLineRst(...)

#define MHal_FRC_IPM_SetVPulseLoc(...)

#define MHal_FRC_IPM_SetLockIntCtrl(...)

#define MHal_FRC_IPM_SetBlankBoundary(...)

#define MHal_FRC_IPM_GetHTotal(...) 0

#define MHal_FRC_IPM_GetVTotal(...) 0

#define MHal_FRC_IPM_GetHde(...) 0

#define MHal_FRC_IPM_GetVde(...) 0

#define MHal_FRC_IPM_Csc(...)

#define MHal_FRC_IPM_CscDither(...)

#define MHal_FRC_IPM_CscRound(...)

#define MHal_FRC_IPM_CscSaturation(...)

#define MHal_FRC_IPM_CheckBoardEn(...)

#define MHal_FRC_IPM_SetIpCtrl(...)

#define MHal_FRC_IPM_SetHRefLock(...)

#define MHal_FRC_IPM_GetHdeCount(...) 0

#define MHal_FRC_IPM_TestPattern(...)

#define MHal_FRC_IPM_SetHTotal(...)

#define MHal_FRC_IPM_SetVTotal(...)

#define MHal_FRC_IPM_HActive(...)

#define MHal_FRC_IPM_VActive(...)

#define MHal_FRC_IPM_GetYcoutLinePitch(...) 0

#define MHal_FRC_IPM_GetWriteFetchNum(...) 0

#define MHal_FRC_IPM_GetReadFetchNum(...) 0

#define MHal_FRC_IPM_GetLineLimit(...) 0

#define MHal_FRC_IPM_SetPacketCount(...)

#define MHal_FRC_IPM_GetFrameYcout(...) 0

#define MHal_FRC_IPM_3DFlag_In_SWMode_En(...)

#define MHal_FRC_IPM_3DFlag_In_SWMode_SetIdx(...)

#define MHal_FRC_IPM_3DFlag_In_HWMode_SrcSel(...)

#define MHal_FRC_IPM_GetIPVfreqX10(...) 0







#define MHal_FRC_OPM_SetFbLineOffset_Data(...)

#define MHal_FRC_OPM_SetFbLineOffset_Me3(...)

#define MHal_FRC_OPM_SetSrcPixNum(...)

#define MHal_FRC_OPM_SetSrcLineNum(...)

#define MHal_FRC_OPM_CeLineCountEn(...)

#define MHal_FRC_OPM_SetLbiSrcPixelNum_Left(...)

#define MHal_FRC_OPM_SetLbiReadPixelNum_Left(...)

#define MHal_FRC_OPM_SetLbiSkipEn(...)

#define MHal_FRC_OPM_SetLbiSkipNum_Left(...)

#define MHal_FRC_OPM_SetLbiSrcPixelNum_Right(...)

#define MHal_FRC_OPM_SetLbiReadPixelNum_Right(...)

#define MHal_FRC_OPM_SetLbiSkipEn3D(...)

#define MHal_FRC_OPM_SetLbiSkipNum_Right(...)

#define MHal_FRC_OPM_FuncEn(...)

#define MHal_FRC_OPM_3dFuncEn(...)

#define MHal_FRC_OPM_SetBaseOffset_Data0(...)

#define MHal_FRC_OPM_SetBaseOffset_Data1(...)

#define MHal_FRC_OPM_SetBaseOffset_Me0(...)

#define MHal_FRC_OPM_SetBaseOffset_Me1(...)

#define MHal_FRC_OPM_SetBaseOffset_Me3(...)

#define MHal_FRC_OPM_SetBaseOffset_Da0_L(...)

#define MHal_FRC_OPM_SetBaseOffset_Da1_L(...)

#define MHal_FRC_OPM_SetBaseOffset_Me0_L(...)

#define MHal_FRC_OPM_SetBaseOffset_Me1_L(...)

#define MHal_FRC_OPM_SetBaseOffset_Me3_L(...)

#define MHal_FRC_OPM_SetBaseOffset_Mr1_L(...)

#define MHal_FRC_OPM_SetBaseOffset_Mr1(...)

#define MHal_FRC_OPM_SetDebugMask(...)

#define MHal_FRC_OPM_SwReset(...)

#define MHal_FRC_OPM_enableBaseAdrMr(...)

#define MHal_FRC_OPM_SetBaseAddr(...)

#define MHal_FRC_OPM_SetMlbOutRstCycle(...)

#define MHal_FRC_OPM_SetFifoTrigThr(...)

#define MHal_FRC_OPM_SetFifoMaxReadReq(...)

#define MHal_FRC_OPM_SetGatingClk(...)

#define MHal_FRC_SetMemMode(...)



#define MHal_FRC_OP2_ColorMatrixEn(...)

#define MHal_FRC_OP2_CscDitherEn(...)

#define MHal_FRC_OP2_DataPathEn(...)

#define MHal_FRC_OP2_DebugFuncEn(...)

#define MHal_FRC_OP2_SetGain_R(...)

#define MHal_FRC_OP2_SetGain_G(...)

#define MHal_FRC_OP2_SetGain_B(...)

#define MHal_FRC_OP2_SetOffset_R(...)

#define MHal_FRC_OP2_SetOffset_G(...)

#define MHal_FRC_OP2_SetOffset_B(...)

#define MHal_FRC_OP2_SetDither(...)

#define MHal_FRC_OP2_PAFRC_FuncEn(...)

#define MHal_FRC_OP2_PAFRC_FuncEn2(...)

#define MHal_FRC_OP2_PAFRC_FuncEn3(...)

#define MHal_FRC_OP2_PAFRC_Set2x2BlockRotDir(...)

#define MHal_FRC_OP2_PAFRC_TopBoxFrSeq(...)

#define MHal_FRC_OP2_PAFRC_TopBoxFrC2Seq(...)

#define MHal_FRC_OP2_PAFRC_SetBoxRotDir(...)

#define MHal_FRC_OP2_PAFRC_Set8x8BoxRotDir(...)

#define MHal_FRC_OP2_PAFRC_SetBlockEntities(...)

#define MHal_FRC_OP2_PAFRC_PolarityCtrl(...)

#define MHal_FRC_OP2_PAFRC_SetBoxLsbValue(...)



#define MHal_FRC_SCTOP_SCMI_Bypass_Enable(...)

#define MHal_FRC_SCTOP_FRC_Bypass_Enable(...)



#define MHal_FRC_SNR_SetPixelHorixontalNum(...)

#define MHal_FRC_SNR_SetLineVerticalNum(...)



#define MHal_FRC_SCP_HSU1_HSP(...)

#define MHal_FRC_SCP_HSU1_Init_Position(...)

#define MHal_FRC_SCP_HSU1_Scaling_Mode(...)

#define MHal_FRC_SCP_HSU1_444to422_Mode(...)

#define MHal_FRC_SCP_HSU1_VSU_Scaling_Mode(...)

#define MHal_FRC_SCP_HSU1_VSU_Scaling_coef(...)

#define MHal_FRC_SCP_HSU2_HSP(...)

#define MHal_FRC_SCP_HSU2_Init_Position(...)

#define MHal_FRC_SCP_HSU2_Scaling_Mode(...)



#define MHal_FRC_OD_SetBaseAddr(...)

#define MHal_FRC_LD_SetBaseAddr(...)

#define MHal_FRC_LD_Edge2D_SetBaseAddr(...)

#define MHal_FRC_ME1_SetBaseAddr(...)

#define MHal_FRC_ME2_SetBaseAddr(...)

#define MHal_FRC_2D3D_Render_SetBaseAddr(...)

#define MHal_FRC_2D3D_Render_Detection_SetBaseAddr(...)

#define MHal_FRC_Halo_SetBaseAddr(...)

#define MHal_FRC_OverDriverSwitch(...)

#define MHal_FRC_OD_Init(...)



#define MHal_FRC_Init(...)



//FRC R2 Mail Box Control

#define Hal_XC_FRC_R2_Get_SwVersion(...)

#define Hal_XC_FRC_R2_Get_CmdVersion(...)

#define Hal_XC_FRC_R2_Init(...)

#define Hal_XC_FRC_R2_Set_Timing(...)

#define Hal_XC_FRC_R2_Set_InputFrameSize(...)

#define Hal_XC_FRC_R2_Set_OutputFrameSize(...)

#define Hal_XC_FRC_R2_Set_Input3DFormat(...)

#define Hal_XC_FRC_R2_Set_MfcMode(...)

#define Hal_XC_FRC_R2_Set_MfcDemoMode(...)

#define Hal_XC_FRC_R2_Set_LocalDimmingMode(...)

#define Hal_XC_FRC_R2_Set_2DTo3DMode(...)



//FRC auto download

#define Hal_FRC_ADLG_set_base_addr(...)

#define Hal_FRC_ADLG_set_depth(...)

#define Hal_FRC_ADLG_set_dma(...)

#define Hal_FRC_ADLG_set_on_off(...)

#else

#define MHal_FRC_IsRGB(args...) 0



#define MHal_FRC_TGEN_SWReset(args...)

#define MHal_FRC_TGEN_DoubleBuf(args...)

#define MHal_FRC_TGEN_SetVTotal(args...)

#define MHal_FRC_TGEN_SetHTotal(args...)

#define MHal_FRC_TGEN_SetVTrigY(args...)

#define MHal_FRC_TGEN_SetVTrigX(args...)

#define MHal_FRC_TGEN_SetVSyncStartEndY(args...)

#define MHal_FRC_TGEN_SetHSyncStartEndX(args...)

#define MHal_FRC_TGEN_SetFdeStartEndY(args...)

#define MHal_FRC_TGEN_SetFdeStartEndX(args...)

#define MHal_FRC_TGEN_SetMdeStartEndY(args...)

#define MHal_FRC_TGEN_SetMdeStartEndX(args...)

#define MHal_FRC_TGEN_FpllRefPointY(args...)

#define MHal_FRC_TGEN_1Clk2PixOut(args...)

#define MHal_FRC_TGEN_SetSyncHcnt(args...)

#define MHal_FRC_TGEN_SetClrSyncAhead(args...)



#define MHal_FRC_IPM_SoftwareReset(args...)

#define MHal_FRC_IPM_RWEn(args...)

#define MHal_FRC_IPM_RW_CEN_Select(args...)

#define MHal_FRC_IPM_SetYuv10Bit(args...)

#define MHal_FRC_IPM_SetMr(args...)

#define MHal_FRC_IPM_SetMemoryMode(args...)

#define MHal_FRC_IPM_YC444To422Control(args...)

#define MHal_FRC_IPM_SetFrameBufferNum(args...)

#define MHal_FRC_IPM_CeLineCountEn(args...)

#define MHal_FRC_IPM_SetBaseAddr(args...)

#define MHal_FRC_IPM_SetIp2Mc(args...)

#define MHal_FRC_IPM_SetYCoutLinePitch(args...)

#define MHal_FRC_IPM_SetReadFetchNumber(args...)

#define MHal_FRC_IPM_SetWriteFetchNumber(args...)

#define MHal_FRC_IPM_SetRfifoThr(args...)

#define MHal_FRC_IPM_SetWfifoThr(args...)

#define MHal_FRC_IPM_SetReadLength(args...)

#define MHal_FRC_IPM_SetWriteLength(args...)

#define MHal_FRC_IPM_SetHTotalPixellimit(args...)

#define MHal_FRC_IPM_SetVTotalPixellimit(args...)

#define MHal_FRC_IPM_SetRmaskNum(args...)

#define MHal_FRC_IPM_SetWmaskNum(args...)

#define MHal_FRC_IPM_SetMirrorMode(args...)

#define MHal_FRC_IPM_PacketInitCnt(args...)

#define MHal_FRC_IPM_SetLvdsInputMode(args...)

#define MHal_FRC_IPM_SetOsdWinIdx(args...)

#define MHal_FRC_IPM_SetVPulseLineRst(args...)

#define MHal_FRC_IPM_SetVPulseLoc(args...)

#define MHal_FRC_IPM_SetLockIntCtrl(args...)

#define MHal_FRC_IPM_SetBlankBoundary(args...)

#define MHal_FRC_IPM_GetHTotal(args...) 0

#define MHal_FRC_IPM_GetVTotal(args...) 0

#define MHal_FRC_IPM_GetHde(args...) 0

#define MHal_FRC_IPM_GetVde(args...) 0

#define MHal_FRC_IPM_Csc(args...)

#define MHal_FRC_IPM_CscDither(args...)

#define MHal_FRC_IPM_CscRound(args...)

#define MHal_FRC_IPM_CscSaturation(args...)

#define MHal_FRC_IPM_CheckBoardEn(args...)

#define MHal_FRC_IPM_SetIpCtrl(args...)

#define MHal_FRC_IPM_SetHRefLock(args...)

#define MHal_FRC_IPM_GetHdeCount(args...) 0

#define MHal_FRC_IPM_TestPattern(args...)

#define MHal_FRC_IPM_SetHTotal(args...)

#define MHal_FRC_IPM_SetVTotal(args...)

#define MHal_FRC_IPM_HActive(args...)

#define MHal_FRC_IPM_VActive(args...)

#define MHal_FRC_IPM_GetYcoutLinePitch(args...) 0

#define MHal_FRC_IPM_GetWriteFetchNum(args...) 0

#define MHal_FRC_IPM_GetReadFetchNum(args...) 0

#define MHal_FRC_IPM_GetLineLimit(args...) 0

#define MHal_FRC_IPM_SetPacketCount(args...)

#define MHal_FRC_IPM_GetFrameYcout(args...) 0

#define MHal_FRC_IPM_3DFlag_In_SWMode_En(args...)

#define MHal_FRC_IPM_3DFlag_In_SWMode_SetIdx(args...)

#define MHal_FRC_IPM_3DFlag_In_HWMode_SrcSel(args...)

#define MHal_FRC_IPM_GetIPVfreqX10(args...) 0







#define MHal_FRC_OPM_SetFbLineOffset_Data(args...)

#define MHal_FRC_OPM_SetFbLineOffset_Me3(args...)

#define MHal_FRC_OPM_SetSrcPixNum(args...)

#define MHal_FRC_OPM_SetSrcLineNum(args...)

#define MHal_FRC_OPM_CeLineCountEn(args...)

#define MHal_FRC_OPM_SetLbiSrcPixelNum_Left(args...)

#define MHal_FRC_OPM_SetLbiReadPixelNum_Left(args...)

#define MHal_FRC_OPM_SetLbiSkipEn(args...)

#define MHal_FRC_OPM_SetLbiSkipNum_Left(args...)

#define MHal_FRC_OPM_SetLbiSrcPixelNum_Right(args...)

#define MHal_FRC_OPM_SetLbiReadPixelNum_Right(args...)

#define MHal_FRC_OPM_SetLbiSkipEn3D(args...)

#define MHal_FRC_OPM_SetLbiSkipNum_Right(args...)

#define MHal_FRC_OPM_FuncEn(args...)

#define MHal_FRC_OPM_3dFuncEn(args...)

#define MHal_FRC_OPM_SetBaseOffset_Data0(args...)

#define MHal_FRC_OPM_SetBaseOffset_Data1(args...)

#define MHal_FRC_OPM_SetBaseOffset_Me0(args...)

#define MHal_FRC_OPM_SetBaseOffset_Me1(args...)

#define MHal_FRC_OPM_SetBaseOffset_Me3(args...)

#define MHal_FRC_OPM_SetBaseOffset_Da0_L(args...)

#define MHal_FRC_OPM_SetBaseOffset_Da1_L(args...)

#define MHal_FRC_OPM_SetBaseOffset_Me0_L(args...)

#define MHal_FRC_OPM_SetBaseOffset_Me1_L(args...)

#define MHal_FRC_OPM_SetBaseOffset_Me3_L(args...)

#define MHal_FRC_OPM_SetBaseOffset_Mr1_L(args...)

#define MHal_FRC_OPM_SetBaseOffset_Mr1(args...)

#define MHal_FRC_OPM_SetDebugMask(args...)

#define MHal_FRC_OPM_SwReset(args...)

#define MHal_FRC_OPM_enableBaseAdrMr(args...)

#define MHal_FRC_OPM_SetBaseAddr(args...)

#define MHal_FRC_OPM_SetMlbOutRstCycle(args...)

#define MHal_FRC_OPM_SetFifoTrigThr(args...)

#define MHal_FRC_OPM_SetFifoMaxReadReq(args...)

#define MHal_FRC_OPM_SetGatingClk(args...)

#define MHal_FRC_SetMemMode(args...)



#define MHal_FRC_OP2_ColorMatrixEn(args...)

#define MHal_FRC_OP2_CscDitherEn(args...)

#define MHal_FRC_OP2_DataPathEn(args...)

#define MHal_FRC_OP2_DebugFuncEn(args...)

#define MHal_FRC_OP2_SetGain_R(args...)

#define MHal_FRC_OP2_SetGain_G(args...)

#define MHal_FRC_OP2_SetGain_B(args...)

#define MHal_FRC_OP2_SetOffset_R(args...)

#define MHal_FRC_OP2_SetOffset_G(args...)

#define MHal_FRC_OP2_SetOffset_B(args...)

#define MHal_FRC_OP2_SetDither(args...)

#define MHal_FRC_OP2_PAFRC_FuncEn(args...)

#define MHal_FRC_OP2_PAFRC_FuncEn2(args...)

#define MHal_FRC_OP2_PAFRC_FuncEn3(args...)

#define MHal_FRC_OP2_PAFRC_Set2x2BlockRotDir(args...)

#define MHal_FRC_OP2_PAFRC_TopBoxFrSeq(args...)

#define MHal_FRC_OP2_PAFRC_TopBoxFrC2Seq(args...)

#define MHal_FRC_OP2_PAFRC_SetBoxRotDir(args...)

#define MHal_FRC_OP2_PAFRC_Set8x8BoxRotDir(args...)

#define MHal_FRC_OP2_PAFRC_SetBlockEntities(args...)

#define MHal_FRC_OP2_PAFRC_PolarityCtrl(args...)

#define MHal_FRC_OP2_PAFRC_SetBoxLsbValue(args...)



#define MHal_FRC_SCTOP_SCMI_Bypass_Enable(args...)

#define MHal_FRC_SCTOP_FRC_Bypass_Enable(args...)



#define MHal_FRC_SNR_SetPixelHorixontalNum(args...)

#define MHal_FRC_SNR_SetLineVerticalNum(args...)



#define MHal_FRC_SCP_HSU1_HSP(args...)

#define MHal_FRC_SCP_HSU1_Init_Position(args...)

#define MHal_FRC_SCP_HSU1_Scaling_Mode(args...)

#define MHal_FRC_SCP_HSU1_444to422_Mode(args...)

#define MHal_FRC_SCP_HSU1_VSU_Scaling_Mode(args...)

#define MHal_FRC_SCP_HSU1_VSU_Scaling_coef(args...)

#define MHal_FRC_SCP_HSU2_HSP(args...)

#define MHal_FRC_SCP_HSU2_Init_Position(args...)

#define MHal_FRC_SCP_HSU2_Scaling_Mode(args...)



#define MHal_FRC_OD_SetBaseAddr(args...)

#define MHal_FRC_LD_SetBaseAddr(args...)

#define MHal_FRC_LD_Edge2D_SetBaseAddr(args...)

#define MHal_FRC_ME1_SetBaseAddr(args...)

#define MHal_FRC_ME2_SetBaseAddr(args...)

#define MHal_FRC_2D3D_Render_SetBaseAddr(args...)

#define MHal_FRC_2D3D_Render_Detection_SetBaseAddr(args...)

#define MHal_FRC_Halo_SetBaseAddr(args...)

#define MHal_FRC_OverDriverSwitch(args...)

#define MHal_FRC_OD_Init(args...)



#define MHal_FRC_Init(args...)



//FRC R2 Mail Box Control

#define Hal_XC_FRC_R2_Get_SwVersion(args...)

#define Hal_XC_FRC_R2_Get_CmdVersion(args...)

#define Hal_XC_FRC_R2_Init(args...)

#define Hal_XC_FRC_R2_Set_Timing(args...)

#define Hal_XC_FRC_R2_Set_InputFrameSize(args...)

#define Hal_XC_FRC_R2_Set_OutputFrameSize(args...)

#define Hal_XC_FRC_R2_Set_Input3DFormat(args...)

#define Hal_XC_FRC_R2_Set_MfcMode(args...)

#define Hal_XC_FRC_R2_Set_MfcDemoMode(args...)

#define Hal_XC_FRC_R2_Set_LocalDimmingMode(args...)

#define Hal_XC_FRC_R2_Set_2DTo3DMode(args...)



//FRC auto download

#define Hal_FRC_ADLG_set_base_addr(args...)

#define Hal_FRC_ADLG_set_depth(args...)

#define Hal_FRC_ADLG_set_dma(args...)

#define Hal_FRC_ADLG_set_on_off(args...)

#endif

#undef INTERFACE

#endif /* MHAL_FRC_H */



