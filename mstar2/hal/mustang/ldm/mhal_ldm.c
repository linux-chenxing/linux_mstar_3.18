///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2008 MStar Semiconductor, Inc.
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

#if (defined (CONFIG_HAS_LD) || defined (CONFIG_HAS_LD_MODULE))
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/module.h>
#include <linux/types.h>
#include "mhal_ldm.h"
#include "reg_ldm.h"


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static void MHal_LD_WriteByte(U32 u32Reg, U8 u8Val )
{
    if(u32Reg%2)
    {
        REG_WH((u32Reg-1), u8Val);
    }
    else
    {
        REG_WL(u32Reg, u8Val);
    }
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
static U8 MHal_LD_ReadByte(U32 u32Reg )
{
    if(u32Reg%2)
    {
        u32Reg = u32Reg-1 ;
        return ((REG_R2B(u32Reg) & 0xFF00)>>8);
    }
    else
    {
        return (REG_R2B(u32Reg) & 0xFF);
    }
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
static void MHal_LD_WriteByteMask(U32 u32Reg, U8 u8Val, U8 u8Mask )
{
	U8 u8Data = MHal_LD_ReadByte(u32Reg);
    if(u32Reg%2)
    {
        REG_WH((u32Reg-1), (u8Data&(~u8Mask))|(u8Val&u8Mask));
    }
    else
    {
        REG_WL(u32Reg, (u8Data&(~u8Mask))|(u8Val&u8Mask));
    }
}
/*
//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
static U8 MHal_LD_ReadByteMask(U32 u32Reg, U8 u8Mask )
{
    if(u32Reg%2)
    {
        u32Reg = u32Reg-1 ;
        return (((REG_R2B(u32Reg) & 0xFF00)>>8) & u8Mask );
    }
    else
    {
        return ((REG_R2B(u32Reg) & 0xFF) & u8Mask );
    }
}
*/

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
static void MHal_LD_Write2Byte(U32 u32Reg, U16 u16Val )
{
    REG_W2B(u32Reg, u16Val);
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
static U16 MHal_LD_Read2Byte(U32 u32Reg )
{
    return REG_R2B(u32Reg) ;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
static void MHal_LD_Write2ByteMask(U32 u32Reg, U16 u16Val, U16 u16Mask )
{
    U16 u16Data=0 ;
    u16Data = REG_R2B(u32Reg);
    u16Data = (u16Data & (0xFFFF-u16Mask))|(u16Val &u16Mask);
    REG_W2B(u32Reg, u16Data);
}
/*
//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
static U16 MHal_LD_Read2ByteMask(U32 u32Reg, U16 u16Mask )
{
    return (REG_R2B(u32Reg) & u16Mask);
}
*/
//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
static void MHal_LD_WriteRegBit(U32 u32Reg, BOOL bEnable, U8 u8Mask )
{
    U8 u8Data=0 ;
    u8Data = MHal_LD_ReadByte(u32Reg);
    u8Data = (bEnable)?(u8Data | (u8Mask))
					   :(u8Data & ~(u8Mask));
    MHal_LD_WriteByte(u32Reg, u8Data);
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
static BOOL MHal_LD_ReadRegBit(U32 u32Reg, U8 u8Mask )
{
    return ((MHal_LD_ReadByte(u32Reg) & u8Mask)==u8Mask);
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
static void MHal_LD_Write4Byte(U32 u32Reg, U32 u32Val )
{
    REG_W2B(u32Reg, u32Val&0xFFFF);
	REG_W2B((u32Reg+2), (u32Val&0xFFFF0000)>>16);
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
static U32 MHal_LD_Read4Byte(U32 u32Reg )
{
    return (REG_R2B((u32Reg+2))<<16)+REG_R2B(u32Reg) ;
}


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

void MHal_LD_SetLocalDimmingEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_ENABLE, bEn, REG_LD_ENABLE_BIT);
}

BOOL MHal_LD_GetLocalDimmingEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_ENABLE, REG_LD_ENABLE_BIT);
}

// h03[4:0] reg_bl_width
void MHal_LD_SetLDFBacklightWidth(U8 u8Width)
{
    MHal_LD_WriteByteMask(REG_LD_LDFW, u8Width-1, REG_LD_LDFW_BITS);
}

// h03[6:0] reg_bl_width
U8 MHal_LD_GetLDFBacklightWidth(void)
{
    return (MHal_LD_ReadByte(REG_LD_LDFW) & REG_LD_LDFW_BITS)+1;
}

// h03[9:5] reg_bl_height
void MHal_LD_SetLDFBacklightHeight(U8 u8Height)
{
    MHal_LD_WriteByteMask(REG_LD_LDFH, (u8Height-1), REG_LD_LDFH_BITS);
}

// h03[13:8] reg_bl_height
U16 MHal_LD_GetLDFBacklightHeight(void)
{
    return (MHal_LD_ReadByte(REG_LD_LDFH) & REG_LD_LDFH_BITS)+1;
}

// h04[25:0] reg_baseaddr0_l
void MHal_LD_SetLDFFrameBufBaseAddr_0(U32 u32Addr)
{
    MHal_LD_Write4Byte(REG_LD_LDFA0, u32Addr);
}

U32 MHal_LD_GetLDFFrameBufBaseAddr_0(void)
{
    return (MHal_LD_Read4Byte(REG_LD_LDFA0) & REG_LD_LDFA_BITS)*MHAL_LD_MIU_BUS;
}

// h06[25:0] reg_baseaddr1_l
void MHal_LD_SetLDFFrameBufBaseAddr_1(U32 u32Addr)
{
    MHal_LD_Write4Byte(REG_LD_LDFA1, u32Addr);
}

U32 MHal_LD_GetLDFFrameBufBaseAddr_1(void)
{
    return (MHal_LD_Read4Byte(REG_LD_LDFA1) & REG_LD_LDFA_BITS)*MHAL_LD_MIU_BUS;
}

// h04[26:0] reg_baseaddr0_l
U32 MHal_LD_GetLDFFrameBufBaseAddr_L0(void)
{
    return (MHal_LD_Read4Byte(REG_LD_LDFAL0) & REG_LD_LDFA_BITS)*MHAL_LD_MIU_BUS;
}

// h06[26:0] reg_baseaddr1_l
U32 MHal_LD_GetLDFFrameBufBaseAddr_L1(void)
{
    return (MHal_LD_Read4Byte(REG_LD_LDFAL1) & REG_LD_LDFA_BITS)*MHAL_LD_MIU_BUS;
}

// h10[26:0] reg_baseaddr0_l
void MHal_LD_SetLDFFrameBufBaseAddr_R0(U32 u32Addr)
{
    MHal_LD_Write4Byte(REG_LD_LDFAR0, u32Addr);
}

// h10[26:0] reg_baseaddr0_r
U32 MHal_LD_GetLDFFrameBufBaseAddr_R0(void)
{
    return (MHal_LD_Read4Byte(REG_LD_LDFAR0) & REG_LD_LDFA_BITS)*MHAL_LD_MIU_BUS;
}

// h14[26:0] reg_baseaddr0_l
void MHal_LD_SetLDFFrameBufBaseAddr_R1(U32 u32Addr)
{
    MHal_LD_Write4Byte(REG_LD_LDFAR1, u32Addr);
}

// h14[26:0] reg_baseaddr1_r
U32 MHal_LD_GetLDFFrameBufBaseAddr_R1(void)
{
    return (MHal_LD_Read4Byte(REG_LD_LDFAR1) & REG_LD_LDFA_BITS)*MHAL_LD_MIU_BUS;
}

void MHal_LD_SetInputHScalingDownRatio(U32 u32Ratio)
{
    MHal_LD_Write4Byte(REG_LD_HSDRATIO, u32Ratio);
}

U32 MHal_LD_GetInputHScalingDownRatio(void)
{
    return MHal_LD_Read4Byte(REG_LD_HSDRATIO) & REG_LD_SDRATIO_BITS;
}

// h1B[19:0] reg_ratio_v_in
void MHal_LD_SetInputVScalingDownRatio(U32 u32Ratio)
{
    MHal_LD_Write4Byte(REG_LD_VSDRATIO, u32Ratio);
}

U32 MHal_LD_GetInputVScalingDownRatio(void)
{
    return MHal_LD_Read4Byte(REG_LD_VSDRATIO) & REG_LD_SDRATIO_BITS;
}

// h1d[7:0] reg_pel_hlpf_thrd
void MHal_LD_SetLDFHLPFThreshold(U8 u8Th)
{
    MHal_LD_WriteByte(REG_LD_LDFHLPFTHD, u8Th);
}

U8 MHal_LD_GetLDFHLPFThreshold(void)
{
    return MHal_LD_ReadByte(REG_LD_LDFHLPFTHD);
}

// h1d[12] reg_pel_hlpf_en
void MHal_LD_SetLDFHLPFEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_LDFHLPFEN, bEn, REG_LD_LDFHLPFEN_BIT);
}

BOOL MHal_LD_GetLDFHLPFEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_LDFHLPFEN, REG_LD_LDFHLPFEN_BIT);
}

// h1d[13] reg_pel_hlpf_step
void MHal_LD_SetLDFHLPFStep(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_LDFHLPFSTEP, bEn, REG_LD_LDFHLPFSTEP_BIT);
}

BOOL MHal_LD_GetLDFHLPFStep(void)
{
    return MHal_LD_ReadRegBit(REG_LD_LDFHLPFSTEP, REG_LD_LDFHLPFSTEP_BIT);
}

// h1f[3:0] reg_alpha
void MHal_LD_SetLDFDCMaxAlpha(U8 u8Alpha)
{
    MHal_LD_WriteByteMask(REG_LD_LDFDCMAXALP, u8Alpha, REG_LD_LDFDCMAXALP_BITS);
}

U8 MHal_LD_GetLDFDCMaxAlpha(void)
{
    return MHal_LD_ReadByte(REG_LD_LDFDCMAXALP) & REG_LD_LDFDCMAXALP_BITS;
}

// h1f[7] reg_pel_white_mod_en
void MHal_LD_SetLDFPixelWhiteModeEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_LDFPWMODEEN, bEn, REG_LD_LDFPWMODEEN_BIT);
}

BOOL MHal_LD_GetLDFPixelWhiteModeEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_LDFPWMODEEN, REG_LD_LDFPWMODEEN_BIT);
}

// h1f[8] reg_write_dc_max_en
void MHal_LD_SetLDFWriteDCMaxEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_LDFWDCMAXEN, bEn, REG_LD_LDFWDCMAXEN_BIT);
}

BOOL MHal_LD_GetLDFWriteDCMaxEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_LDFWDCMAXEN, REG_LD_LDFWDCMAXEN_BIT);
}

// h1f[9] reg_write_dc_max_of_en
void MHal_LD_SetLDFWriteDCMaxOFEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_LDFWDCMAXOFEN, bEn, REG_LD_LDFWDCMAXOFEN_BIT);
}

BOOL MHal_LD_GetLDFWriteDCMaxOFEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_LDFWDCMAXOFEN, REG_LD_LDFWDCMAXOFEN_BIT);
}

// h22[11:0] reg_dummy0
void MHal_LD_SetSWPulseId(U16 u16Id)
{
    MHal_LD_Write2ByteMask(REG_LD_SWPULID, u16Id, REG_LD_SWPULID_BITS);
}

U16 MHal_LD_GetSWPulseId(void)
{
    return MHal_LD_Read2Byte(REG_LD_SWPULID) & REG_LD_SWPULID_BITS;
}

// h22[15:12] reg_dummy0
void MHal_LD_SetSWPulseMode(U8 u8Mode)
{
    MHal_LD_WriteByteMask(REG_LD_SWPULMODE, u8Mode, REG_LD_SWPULMODE_BITS);
}

U8 MHal_LD_GetSWPulseMode(void)
{
    return MHal_LD_ReadByte(REG_LD_SWPULMODE) >> REG_LD_SWPULMODE_SHIFT;
}

// h23[7:0] reg_dummy1
void MHal_LD_SetSWPulseLEDIntensity(U8 u8Intensity)
{
    MHal_LD_WriteByte(REG_LD_SWPULLEDINTENS, u8Intensity);
}

U8 MHal_LD_GetSWPulseLEDIntensity(void)
{
    return MHal_LD_ReadByte(REG_LD_SWPULLEDINTENS);
}

// h23[15:8] reg_dummy1
void MHal_LD_SetSWPulseLDBIntensity(U8 u8Intensity)
{
    MHal_LD_WriteByte(REG_LD_SWPULLDBINTENS, u8Intensity);
}

U8 MHal_LD_GetSWPulseLDBIntensity(void)
{
    return MHal_LD_ReadByte(REG_LD_SWPULLDBINTENS);
}

// h24[7:0] reg_dummy2
void MHal_LD_SetSWSpatialFilterStrength(U8 u8Str)
{
    MHal_LD_WriteByte(REG_LD_SWSPAFILTSTREN, u8Str);
}

U8 MHal_LD_GetSWSpatialFilterStrength(void)
{
    return MHal_LD_ReadByte(REG_LD_SWSPAFILTSTREN);
}

// h24[15:8] reg_dummy2
void MHal_LD_SetSWTemporalFilterStrengthDn(U8 u8Str)
{
    MHal_LD_WriteByte(REG_LD_SWTEMFILTSTRENDN, u8Str);
}

U8 MHal_LD_GetSWTemporalFilterStrengthDn(void)
{
    return MHal_LD_ReadByte(REG_LD_SWTEMFILTSTRENDN);
}

// h25[7:0] reg_dummy3
void MHal_LD_SetSWLocalDimmingStrength(U8 u8Str)
{
    MHal_LD_WriteByte(REG_LD_SWLDSTREN, u8Str);
}

U8 MHal_LD_GetSWLocalDimmingStrength(void)
{
    return MHal_LD_ReadByte(REG_LD_SWLDSTREN);
}

// h25[15:8] reg_dummy3
void MHal_LD_SetSWGlobalDimmingStrength(U8 u8Str)
{
    MHal_LD_WriteByte(REG_LD_SWGDSTREN, u8Str);
}

U8 MHal_LD_GetSWGlobalDimmingStrength(void)
{
    return MHal_LD_ReadByte(REG_LD_SWGDSTREN);
}

//  h28[7:0] reg_dummy2
void MHal_LD_SetSWMaxThreshold(U8 u8Th)
{
    MHal_LD_WriteByte(REG_LD_SWMAXTRD, u8Th);
}

U8 MHal_LD_GetSWMaxThreshold(void)
{
    return MHal_LD_ReadByte(REG_LD_SWMAXTRD);
}

// h30[19:0] reg_ratio_h_out
void MHal_LD_SetOutputHScalingUpRatio(U32 u32Ratio)
{
    MHal_LD_Write4Byte(REG_LD_OHSURATIO, u32Ratio);
}

U32 MHal_LD_GetOutputHScalingUpRatio(void)
{
    return MHal_LD_Read4Byte(REG_LD_OHSURATIO) & REG_LD_OHSURATIO_BITS;
}

// h32[19:0] reg_ratio_v_out
void MHal_LD_SetOutputVScalingUpRatio(U32 u32Ratio)
{
    MHal_LD_Write4Byte(REG_LD_OVSURATIO, u32Ratio);
}

U32 MHal_LD_GetOutputVScalingUpRatio(void)
{
    return MHal_LD_Read4Byte(REG_LD_OVSURATIO) & REG_LD_OVSURATIO_BITS;
}

// h33[15:0] reg_ratio_h_lsf_cbmode
void MHal_LD_SetLSFCBHRatio(U16 u16Ratio)
{
    MHal_LD_Write2Byte(REG_LD_LSFCBHRATIO, u16Ratio);
}

U16 MHal_LD_GetLSFCBHRatio(void)
{
    return MHal_LD_Read2Byte(REG_LD_LSFCBHRATIO);
}

// h34[15:0] reg_ratio_v_lsf_cbmode
void MHal_LD_SetLSFCBVRatio(U16 u16Ratio)
{
    MHal_LD_Write2Byte(REG_LD_LSFCBVRATIO, u16Ratio);
}

U16 MHal_LD_GetLSFCBVRatio(void)
{
    return MHal_LD_Read2Byte(REG_LD_LSFCBVRATIO);
}

// h37[0] reg_edge2d_gain_en
void MHal_LD_SetEdge2DGainEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_EDGE2DGAINEN, bEn, REG_LD_EDGE2DGAINEN_BIT);
}

BOOL MHal_LD_GetEdge2DGainEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_EDGE2DGAINEN, REG_LD_EDGE2DGAINEN_BIT);
}

// h37[1] reg_edge2d_en
void MHal_LD_SetEdge2DEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_EDGE2DEN, bEn, REG_LD_EDGE2DEN_BIT);
}

BOOL MHal_LD_GetEdge2DEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_EDGE2DEN, REG_LD_EDGE2DEN_BIT);
}

// h37[2] reg_edge2d_direct_type_en
void MHal_LD_SetEdge2DDirectTypeEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_EDGE2DIRTYDEN, bEn, REG_LD_EDGE2DDIRTYEN_BIT);
}

BOOL MHal_LD_GetEdge2DDirectTypeEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_EDGE2DIRTYDEN, REG_LD_EDGE2DDIRTYEN_BIT);
}

// h37[8] reg_new_edge2d_en
void MHal_LD_SetEdge2DLocalTypeEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_EDGE2DLCTYEN, bEn, REG_LD_EDGE2DLCTYEN_BIT);
}

BOOL MHal_LD_GetEdge2DLocalTypeEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_EDGE2DLCTYEN, REG_LD_EDGE2DLCTYEN_BIT);
}

// h38[19:0] reg_lsf_h_init
void MHal_LD_SetLSFHInitialPhase(U32 u32Phase)
{
    MHal_LD_Write4Byte(REG_LD_LSFHINITP, u32Phase);
}

U32 MHal_LD_GetLSFHInitialPhase(void)
{
    return MHal_LD_Read4Byte(REG_LD_LSFHINITP) & REG_LD_LSFHINITP_BITS;
}

// h55[19:0] reg_lsf_h_init_r
void MHal_LD_SetLSFHInitialPhase_R(U32 u32Phase)
{
    MHal_LD_Write4Byte(REG_LD_LSFHINITP_R, u32Phase);
}

U32 MHal_LD_GetLSFHInitialPhase_R(void)
{
    return MHal_LD_Read4Byte(REG_LD_LSFHINITP_R) & REG_LD_LSFHINITP_BITS_R;
}

// h3a[19:0] reg_lsf_v_init
void MHal_LD_SetLSFVInitialPhase(U32 u32Phase)
{
    MHal_LD_Write4Byte(REG_LD_LSFVINITP, u32Phase);
}

U32 MHal_LD_GetLSFVInitialPhase(void)
{
    return MHal_LD_Read4Byte(REG_LD_LSFVINITP) & REG_LD_LSFVINITP_BITS;
}

// h3c[0] reg_lsf_h_shift
void MHal_LD_SetLSFHBoundary(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_LSFHBD, bEn, REG_LD_LSFHBD_BIT);
}

BOOL MHal_LD_GetLSFHBoundary(void)
{
    return MHal_LD_ReadRegBit(REG_LD_LSFHBD, REG_LD_LSFHBD_BIT);
}

void MHal_LD_SetLSFHBoundary_R(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_LSFHBD_R, bEn, REG_LD_LSFHBD_R_BIT);
}

BOOL MHal_LD_GetLSFHBoundary_R(void)
{
    return MHal_LD_ReadRegBit(REG_LD_LSFHBD_R, REG_LD_LSFHBD_R_BIT);
}

// h3c[1] reg_lsf_v_shift
void MHal_LD_SetLSFVBoundary(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_LSFVBD, bEn, REG_LD_LSFVBD_BIT);
}

BOOL MHal_LD_GetLSFVBoundary(void)
{
    return MHal_LD_ReadRegBit(REG_LD_LSFVBD, REG_LD_LSFVBD_BIT);
}

// h47[6:0] reg_wid_st_offset_slv_chip
void MHal_LD_SetLSFSlaveFirst(U8 u8Blk)
{
    MHal_LD_WriteByte(REG_LD_LSFSF, u8Blk);
}

U8 MHal_LD_GetLSFSlaveFirst(void)
{
    return MHal_LD_ReadByte(REG_LD_LSFSF);
}

// h47[14:8] reg_wid_end_mas_chip
void MHal_LD_SetLSFMasterLast(U8 u8Blk)
{
    MHal_LD_WriteByte(REG_LD_LSFML, u8Blk);
}

U8 MHal_LD_GetLSFMasterLast(void)
{
    return MHal_LD_ReadByte(REG_LD_LSFML);
}

// h3c[3:2] reg_edge_level_shift
void MHal_LD_SetEdgeLevelShift(U8 u8Mode)
{
    MHal_LD_WriteByteMask(REG_LD_EDGELEVSHIFT, u8Mode << REG_LD_EDGELEVSHIFT_BITS, REG_LD_EDGELEVSHIFT_MASK);
}

U8 MHal_LD_GetEdgeLevelShift(void)
{
    return (MHal_LD_ReadByte(REG_LD_EDGELEVSHIFT) & REG_LD_EDGELEVSHIFT_MASK) >> REG_LD_EDGELEVSHIFT_BITS;
}

// h3c[8:4] reg_cmp_blend
void MHal_LD_SetCompensationBlend(U8 u8Alpha)
{
    MHal_LD_Write2ByteMask(REG_LD_COMPBLEND, ((U16)u8Alpha) << REG_LD_COMPBLEND_SHIFT, REG_LD_COMPBLEND_MSK);
}

U8 MHal_LD_GetCompensationBlend(void)
{
    return (MHal_LD_Read2Byte(REG_LD_COMPBLEND) & REG_LD_COMPBLEND_MSK) >> REG_LD_COMPBLEND_SHIFT;
}

// h3c[10] reg_bld_dither_en
void MHal_LD_SetBlendingDitherEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_BLENDDITHEREN, bEn, REG_LD_BLENDDITHEREN_BIT);
}

BOOL MHal_LD_GetBlendingDitherEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_BLENDDITHEREN, REG_LD_BLENDDITHEREN_BIT);
}

// h3c[13] reg_cmp_dither_en
void MHal_LD_SetCompensationDitherEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_COMPDITHEREN, bEn, REG_LD_COMPDITHEREN_BIT);
}

BOOL MHal_LD_GetCompensationDitherEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_COMPDITHEREN, REG_LD_COMPDITHEREN_BIT);
}

void MHal_LD_WriteCompensationLUT(U16 u16Lut)
{
    MHal_LD_Write2ByteMask(REG_LD_COMPLUTW, u16Lut, REG_LD_COMPLUT_MSK);
}

// h3e[11:0] reg_r_comp_lut (readonly)
U16 MHal_LD_ReadCompensationLUT(void)
{
    return MHal_LD_Read2Byte(REG_LD_COMPLUTR) & REG_LD_COMPLUT_MSK;
}

void MHal_LD_WriteCompensationLowLUT(U16 u16Lut)
{
    MHal_LD_Write2ByteMask(REG_LD_COMPLOWLUTW, u16Lut, REG_LD_COMPLUT_MSK);
}

// h3e[11:0] reg_r_comp_lut (readonly)
U16 MHal_LD_ReadCompensationLowLUT(void)
{
    return MHal_LD_Read2Byte(REG_LD_COMPLOWLUTR) & REG_LD_COMPLUT_MSK;
}

// h3f[7:0] reg_comp_addr
void MHal_LD_SetCompensationLUTAddress(U8 u8Addr)
{
    MHal_LD_WriteByte(REG_LD_COMPLUTADDR, u8Addr);
}
void MHal_LD_SetCompensationLUTAddressLow(U8 u8Addr)
{
    //h3F[7:0 reg_comp_addr_2
    //MHal_LD_WriteByte(0x134E7E, u8Addr);
    MHal_LD_WriteByte(REG_LD_COMPLOWLUTADDR, u8Addr);
}

U8 MHal_LD_GetCompensationLUTAddress(void)
{
    return MHal_LD_ReadByte(REG_LD_COMPLUTADDR);
}

U8 MHal_LD_GetCompensationLUTAddressLow(void)
{
//    return MHal_LD_ReadByte(0x134E7E);
    return MHal_LD_ReadByte(REG_LD_COMPLOWLUTADDR);
}

// h3f[10] reg_comp_wd
void MHal_LD_SetCompensationWriteEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_COMPWEN, bEn, REG_LD_COMPWEN_BIT);
}

BOOL MHal_LD_GetCompensationWriteEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_COMPWEN, REG_LD_COMPWEN_BIT);
}

// h3f[11] reg_comp_rd
void MHal_LD_SetCompensationReadEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_COMPREN, bEn, REG_LD_COMPREN_BIT);
}

BOOL MHal_LD_GetCompensationReadEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_COMPREN, REG_LD_COMPREN_BIT);
}

void MHal_LD_SetCompensationLowWriteEn(BOOL bEn)
{
//    MHal_LD_WriteRegBit(0x134E7F, bEn, REG_LD_COMPWEN_BIT);
    MHal_LD_WriteRegBit(REG_LD_COMPLOWWEN, bEn, REG_LD_COMPWEN_BIT);
}

BOOL MHal_LD_GetCompensationLowWriteEn(void)
{
//    return MHal_LD_ReadRegBit(0x134E7F, REG_LD_COMPWEN_BIT);
    return MHal_LD_ReadRegBit(REG_LD_COMPLOWWEN, REG_LD_COMPWEN_BIT);
}

// h3f[11] reg_comp_rd
void MHal_LD_SetCompensationLowReadEn(BOOL bEn)
{
//    MHal_LD_WriteRegBit(0x134E7F, bEn, REG_LD_COMPREN_BIT);
    MHal_LD_WriteRegBit(REG_LD_COMPLOWREN, bEn, REG_LD_COMPREN_BIT);
}

BOOL MHal_LD_GetCompensationLowReadEn(void)
{
//    return MHal_LD_ReadRegBit(0x134E7F, REG_LD_COMPREN_BIT);
    return MHal_LD_ReadRegBit(REG_LD_COMPLOWREN, REG_LD_COMPREN_BIT);
}
// h3f[12] reg_comp_mod, 1:incremental
void MHal_LD_SetCompensationMode(U8 bMode)
{
    MHal_LD_WriteRegBit(REG_LD_COMPMOD, bMode, REG_LD_COMPMOD_BIT);
}

BOOL MHal_LD_GetCompensationMode(void)
{
    return MHal_LD_ReadRegBit(REG_LD_COMPMOD, REG_LD_COMPMOD_BIT);
}

void MHal_LD_SetCompensationLowMode(U8 bMode)
{
    MHal_LD_WriteRegBit(REG_LD_COMPLOWMOD, bMode, REG_LD_COMPMOD_BIT);
}

BOOL MHal_LD_GetCompensationLowMode(void)
{
    return MHal_LD_ReadRegBit(REG_LD_COMPLOWMOD, REG_LD_COMPMOD_BIT);
}

// h40[4] reg_read_addr_idx (readonly)
BOOL MHal_LD_GetCurFrameIdx(void)
{
    return MHal_LD_ReadRegBit(REG_LD_CURFRAMIDX, REG_LD_CURFRAMIDX_BIT);
}

// h41[7:0] reg_dummy7
void MHal_LD_SetLEDBacklightWidth(U8 u8Width)
{
    MHal_LD_WriteByte(REG_LD_LEDBLW, u8Width-1);
}

U8 MHal_LD_GetLEDBacklightWidth(void)
{
    return MHal_LD_ReadByte(REG_LD_LEDBLW)+1;
}

// h41[15:8] reg_dummy7
void MHal_LD_SetLEDBacklightHeight(U8 u8Height)
{
    MHal_LD_WriteByte(REG_LD_LEDBLH, u8Height-1);
}

U8 MHal_LD_GetLEDBacklightHeight(void)
{
    return MHal_LD_ReadByte(REG_LD_LEDBLH)+1;
}

// h43[15:8] reg_dummy9
void MHal_LD_SetSWTemporalFilterStrengthUp(U8 u8Str)
{
    MHal_LD_WriteByte(REG_LD_SWTEMPFILTSTRUP, u8Str);
}

// h43[15:8] reg_dummy9
U8 MHal_LD_GetSWTemporalFilterStrengthUp(void)
{
    return MHal_LD_ReadByte(REG_LD_SWTEMPFILTSTRUP);
}

// h44[3] reg_dummy10
void MHal_LD_SetSWWriteSPIEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_SWWWSPIEN_BIT, bEn, REG_LD_SWWWSPIEN_BIT);
}

BOOL MHal_LD_GetSWWriteSPIEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_SWWWSPIEN_BIT, REG_LD_SWWWSPIEN_BIT);
}

// h44[7] reg_dummy10
void MHal_LD_SetSWWriteLDBEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_SWWLDBEN, bEn, REG_LD_SWWLDBEN_BIT);
}

BOOL MHal_LD_GetSWWriteLDBEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_SWWLDBEN, REG_LD_SWWLDBEN_BIT);
}

// h44[12] reg_dummy10
void MHal_LD_SetSWAlgorithmEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_SWALGEN, bEn, REG_LD_SWALGEN_BIT);
}

BOOL MHal_LD_GetSWAlgorithmEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_SWALGEN, REG_LD_SWALGEN_BIT);
}

// h44[13] reg_dummy10
void MHal_LD_SetSWReadLDFEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_SWRLDFEN, bEn, REG_LD_SWRLDFEN_BIT);
}

BOOL MHal_LD_GetSWReadLDFEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_SWRLDFEN, REG_LD_SWRLDFEN_BIT);
}

// h44[15:14] reg_dummy10
void MHal_LD_SetLEDType(U8 eType)
{
    MHal_LD_WriteByteMask(REG_LD_LEDTYP, eType << REG_LD_LEDTYP_SHIFT, REG_LD_LEDTYP_SMSK);
}

U8 MHal_LD_GetLEDType(void)
{
    return (MHal_LD_ReadByte(REG_LD_LEDTYP) >> REG_LD_LEDTYP_SHIFT) & REG_LD_LEDTYP_GMSK;
}

// h46[6] reg_comp_en
void MHal_LD_SetCompensationEn(BOOL bEn)
{
    MHal_LD_WriteRegBit(REG_LD_COMPEN, bEn, REG_LD_COMPEN_BIT);
}

BOOL MHal_LD_GetCompensationEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_COMPEN, REG_LD_COMPEN_BIT);
}

// h4c[6:0] reg_bl_width
void MHal_LD_SetLSFBacklightWidth(U8 u8Width)
{
    MHal_LD_WriteByteMask(REG_LD_LSFBLW, u8Width-1, REG_LD_LSFBLW_MSK);
}

U8 MHal_LD_GetLSFBacklightWidth(void)
{
    return (MHal_LD_ReadByte(REG_LD_LSFBLW) & REG_LD_LSFBLW_MSK)+1;
}

// h4c[14:8] reg_bl_height
void MHal_LD_SetLSFBacklightHeight(U8 u8Height)
{
    MHal_LD_WriteByteMask(REG_LD_LSFBLH, u8Height-1, REG_LD_LSFBLH_MSK);
}

U8 MHal_LD_GetLSFBacklightHeight(void)
{
    return (MHal_LD_ReadByte(REG_LD_LSFBLH) & REG_LD_LSFBLH_MSK)+1;
}

// h4D[25:0] reg_baseaddr0_l
void MHal_LD_SetLDBFrameBufBaseAddr_0(U32 u32Addr)
{
    MHal_LD_Write4Byte(REG_LD_LDBFBA0, u32Addr);
}

U32 MHal_LD_GetLDBFrameBufBaseAddr_0(void)
{
    return (MHal_LD_Read4Byte(REG_LD_LDBFBA0) & REG_LD_LDBFBA0_MSK)*MHAL_LD_MIU_BUS;
}

// h4F[25:0] reg_baseaddr1_l
void MHal_LD_SetLDBFrameBufBaseAddr_1(U32 u32Addr)
{
    MHal_LD_Write4Byte(REG_LD_LDBFBA1, u32Addr);
}

U32 MHal_LD_GetLDBFrameBufBaseAddr_1(void)
{
    return (MHal_LD_Read4Byte(REG_LD_LDBFBA1) & REG_LD_LDBFBA1_MSK)*MHAL_LD_MIU_BUS;
}

void MHal_LD_SetLDBFrameBufBaseAddr_L0(U32 u32Addr)
{
    MHal_LD_Write4Byte(REG_LD_LDBFBAL0, u32Addr);
}

// h4D[26:0] reg_baseaddr0_l
U32 MHal_LD_GetLDBFrameBufBaseAddr_L0(void)
{
    return (MHal_LD_Read4Byte(REG_LD_LDBFBAL0) & REG_LD_LDBFBAL0_MSK)*MHAL_LD_MIU_BUS;
}

void MHal_LD_SetLDBFrameBufBaseAddr_L1(U32 u32Addr)
{
    MHal_LD_Write4Byte(REG_LD_LDBFBAL1, u32Addr);
}

// h4F[26:0] reg_baseaddr1_l
U32 MHal_LD_GetLDBFrameBufBaseAddr_L1(void)
{
    return (MHal_LD_Read4Byte(REG_LD_LDBFBAL1) & REG_LD_LDBFBAL1_MSK)*MHAL_LD_MIU_BUS;
}

void MHal_LD_SetLDBFrameBufBaseAddr_R0(U32 u32Addr)
{
    MHal_LD_Write4Byte(REG_LD_LDBFBAR0, u32Addr);
}

// h51[26:0] reg_baseaddr0_r
U32 MHal_LD_GetLDBFrameBufBaseAddr_R0(void)
{
    return (MHal_LD_Read4Byte(REG_LD_LDBFBAR0) & REG_LD_LDBFBAR0_MSK)*MHAL_LD_MIU_BUS;
}

void MHal_LD_SetLDBFrameBufBaseAddr_R1(U32 u32Addr)
{
    MHal_LD_Write4Byte(REG_LD_LDBFBAR1, u32Addr);
}

// h53[26:0] reg_baseaddr1_r
U32 MHal_LD_GetLDBFrameBufBaseAddr_R1(void)
{
    return (MHal_LD_Read4Byte(REG_LD_LDBFBAR1) & REG_LD_LDBFBAR1_MSK)*MHAL_LD_MIU_BUS;
}

// h57[7:0] reg_dummy5
void MHal_LD_SetSWTemporalFilterLowTh(U8 u8Th)
{
    MHal_LD_WriteByte(REG_LD_TEMPFILTLTH, u8Th);
}

U8 MHal_LD_GetSWTemporalFilterLowTh(void)
{
    return MHal_LD_ReadByte(REG_LD_TEMPFILTLTH);
}

// h57[15:8] reg_dummy5
void MHal_LD_SetSWTemporalFilterHighTh(U8 u8Th)
{
    MHal_LD_WriteByte(REG_LD_TEMPFILTHTH, u8Th);
}

U8 MHal_LD_GetSWTemporalFilterHighTh(void)
{
    return MHal_LD_ReadByte(REG_LD_TEMPFILTHTH);
}

// h58[7:0] reg_dummy6
void MHal_LD_SetSWSpatialFilterStrength2(U8 u8Dist)
{
    MHal_LD_WriteByte(REG_LD_TEMPFILTSTR2, u8Dist);
}

U8 MHal_LD_GetSWSpatialFilterStrength2(void)
{
    return MHal_LD_ReadByte(REG_LD_TEMPFILTSTR2);
}

// h58[15:8] reg_dummy6
void MHal_LD_SetSWSpatialFilterStrength3(U8 u8Dist)
{
    MHal_LD_WriteByte(REG_LD_TEMPFILTSTR3, u8Dist);
}

U8 MHal_LD_GetSWSpatialFilterStrength3(void)
{
    return MHal_LD_ReadByte(REG_LD_TEMPFILTSTR3);
}

//
void MHal_LD_SetSWSpatialFilterStrength4(U8 u8Dist)
{
    MHal_LD_WriteByte(REG_LD_TEMPFILTSTR4, u8Dist);
}

U8 MHal_LD_GetSWSpatialFilterStrength4(void)
{
    return MHal_LD_ReadByte(REG_LD_TEMPFILTSTR4);
}

//
void MHal_LD_SetSWSpatialFilterStrength5(U8 u8Dist)
{
    MHal_LD_WriteByte(REG_LD_TEMPFILTSTR5, u8Dist); 
}

U8 MHal_LD_GetSWSpatialFilterStrength5(void)
{
    return MHal_LD_ReadByte(REG_LD_TEMPFILTSTR5);
}

// h63[4:0] reg_bl_width_led
void MHal_LD_SetLDBBacklightWidth(U8 u8Width)
{
    MHal_LD_WriteByteMask(REG_LD_LDBLW, u8Width-1, REG_LD_LDBLW_MSK);
}

// h63[5:0] reg_bl_width_led
U8 MHal_LD_GetLDBBacklightWidth(void)
{
    return (MHal_LD_ReadByte(REG_LD_LDBLW) & REG_LD_LDBLW_MSK)+1;
}

// h63[9:5] reg_bl_height_led
void MHal_LD_SetLDBBacklightHeight(U8 u8Height)
{
    MHal_LD_WriteByteMask(REG_LD_LDBLH, u8Height-1, REG_LD_LDBLH_MSK);
}

// h63[13:8] reg_bl_height_led
U8 MHal_LD_GetLDBBacklightHeight(void)
{
    return (MHal_LD_ReadByte(REG_LD_LDBLH) & REG_LD_LDBLH_MSK)+1;
}

// h70[12:0] reg_frm_width
void MHal_LD_SetFrameWidth(U16 u16Width)
{
    MHal_LD_Write2ByteMask(REG_LD_FW, u16Width-1, REG_LD_FW_MSK);
}

U16 MHal_LD_GetFrameWidth(void)
{
    return (MHal_LD_Read2Byte(REG_LD_FW) & REG_LD_FW_MSK)+1;
}

// h71[11:0] reg_frm_height
void MHal_LD_SetFrameHeight(U16 u16Height)
{
    MHal_LD_Write2ByteMask(REG_LD_FH, u16Height-1, REG_LD_FH_MSK);
}

U16 MHal_LD_GetFrameHeight(void)
{
    return (MHal_LD_Read2Byte(REG_LD_FH) & REG_LD_FH_MSK)+1;
}

// h72[3:1] reg_lsf_out_mod
void MHal_LD_SetLSFOutMode(U8 u8Mode)
{
    return MHal_LD_WriteByteMask(REG_LD_LSFOM, u8Mode << REG_LD_LSFOM_SHIFT, REG_LD_LSFOM_MSK);
}

U8 MHal_LD_GetLSFOutMode(void)
{
    return (MHal_LD_ReadByte(REG_LD_LSFOM) & REG_LD_LSFOM_MSK) >> REG_LD_LSFOM_SHIFT;
}

// h76[7:0] reg_dummy0
void MHal_LD_SetSWMinClampValue(U8 u8Val)
{
    MHal_LD_WriteByte(REG_LD_SWMINCLAMPV, u8Val);
}

U8 MHal_LD_GetSWMinClampValue(void)
{
    return MHal_LD_ReadByte(REG_LD_SWMINCLAMPV);
}

// h76[11:8] reg_dummy0
void MHal_LD_SetSWBacklightInGamma(U8 u8Type)
{
    MHal_LD_WriteByteMask(REG_LD_SWBLIGAM, u8Type, REG_LD_SWBLIGAM_MSK);
}

// h76[11:8] reg_dummy0
U8 MHal_LD_GetSWBacklightInGamma(void)
{
    return MHal_LD_ReadByte(REG_LD_SWBLIGAM) & REG_LD_SWBLIGAM_MSK;
}

// h76[15:12] reg_dummy0
void MHal_LD_SetSWBacklightOutGamma(U8 u8Type)
{
    MHal_LD_WriteByteMask(REG_LD_SWBLOGAM, u8Type<<REG_LD_SWBLOGAM_SHIFT, REG_LD_SSWBLOGAM_MSK);
}

// h76[15:12] reg_dummy0
U8 MHal_LD_GetSWBacklightOutGamma(void)
{
    return (MHal_LD_ReadByte(REG_LD_SWBLOGAM) >> REG_LD_SWBLOGAM_SHIFT) & REG_LD_GSWBLOGAM_MSK;
}

// h79[26:0] reg_edge_2d_baseaddr
void MHal_LD_SetEdge2DBufBaseAddr(U32 u32Addr)
{
    MHal_LD_Write4Byte(REG_LD_EG2DBA, u32Addr);
}

U32 MHal_LD_GetEdge2DBufBaseAddr(void)
{
    return (MHal_LD_Read4Byte(REG_LD_EG2DBA) & REG_LD_EG2DBA_MSK);
}

BOOL MHal_LD_GetLinearEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_LINEEN, REG_LD_LINEEN_BIT);
}

U16 MHal_LD_GetTempFilterMaxSpeed(void)
{
    return MHal_LD_Read2Byte(REG_LD_TEMPMAXSPEED);
}

void MHal_LD_SetLSFEncodingEn(BOOL bLSFEncodingEn)
{
    //h20 [0]reg_ld_coefdec
    MHal_LD_WriteByteMask(REG_LD_LSFENCEN, (U16)bLSFEncodingEn, REG_LD_LSFENCEN_MSK);

}
void MHal_LD_SetCompLowGainEn(BOOL bCompLowGainEn)
{
    //h20 [2]reg_ld_comp_low_gain_en
    MHal_LD_WriteByteMask(REG_LD_COMPLOWGAINEN, ((U16)bCompLowGainEn)<<REG_LD_COMPLOWGAINEN_SHIFT, REG_LD_COMPLOWGAINEN_MSK);
}

void MHal_LD_SetSaturationEn(BOOL bSaturationEn)
{
    //h50 [7]reg_sat_en
    MHal_LD_WriteByteMask(REG_LD_SATEN, ((U16)bSaturationEn<<REG_LD_SATEN_SHIFT), REG_LD_SATEN_MSK);
}

void MHal_LD_SetSatA(U8 u8Reg_sat_a)
{
    //h51 [7:0]reg_sat_a
    MHal_LD_WriteByteMask(REG_LD_SATA, u8Reg_sat_a, REG_LD_SATA_MSK);
}
void MHal_LD_SetSatC(U16 U16Reg_sat_c)
{
    //h55 [11:0]reg_sat_c
    MHal_LD_Write2ByteMask(REG_LD_SATC, U16Reg_sat_c, REG_LD_SATC_MSK);
}
void MHal_LD_SetSatCSft(U8 u8Reg_sat_c_sft)
{
    //h56 [3:0]reg_sat_c_sft
    MHal_LD_WriteByteMask(REG_LD_SATCSFT, u8Reg_sat_c_sft, REG_LD_SATCSFT_MSK);
}

// h7C[11:0] reg_Set report window start horizontal pel_L
void MHal_LD_SetReportWindowStartHorizon(U16 u16WinStart)
{
    MHal_LD_Write2ByteMask(REG_LD_REPORTWIN, u16WinStart - 1, REG_LD_REPORTWIN_MSK);
}

U32 MHal_LD_GetReportWindowStartHorizon(void)
{
    return (MHal_LD_Read2Byte(REG_LD_REPORTWIN) & REG_LD_REPORTWIN_MSK)+1;
}

// h72[6] reg_Set report window SWAP
void MHal_LD_SetReportWindowSwap(void)
{
    MHal_LD_WriteByteMask(REG_LD_LSFOM, REG_LD_REPORTWIN_SWAP_BIT, REG_LD_REPORTWIN_SWAP_MSK);
}

BOOL MHal_LD_GetReportWindowSwap(void)
{
    return MHal_LD_ReadRegBit(REG_LD_LSFOM, REG_LD_REPORTWIN_SWAP_BIT);
}

//LD2
// h02[15:0] reg_ratio_h_lsf_cbmode
void MHal_LD_SetLSFCBHInit(U16 u16Phase)
{
    MHal_LD_Write2Byte(REG_LD_LSFCBHINIT, u16Phase);
}

U16  MHal_LD_GetLSFCBHInit(void)
{
    return MHal_LD_Read2Byte(REG_LD_LSFCBHINIT);
}

// h03[6:0] reg_led_h_dp_cnt_ini
void MHal_LD_SetLEDSlaveFirst(U8 u8Led)
{
    MHal_LD_WriteByte(REG_LD_LEDSF, u8Led);
}

U8 MHal_LD_GetLEDSlaveFirst(void)
{
    return MHal_LD_ReadByte(REG_LD_LEDSF);
}

//guard band IN SC register sub bank CB
BOOL MHal_LD_LR_GetGuradbandDeEn(void)
{
    return MHal_LD_ReadRegBit(REG_LD_LR_GBDEEN, REG_LD_LR_GBDEEN_BIT);
}

U16  MHal_LD_LR_GetGuradbandXStL(void)
{
    return MHal_LD_Read2Byte(REG_LD_LR_GBDEXST_L);
}

U16  MHal_LD_LR_GetGuradbandXEdL(void)
{
    return MHal_LD_Read2Byte(REG_LD_LR_GBDEXEND_L);
}

U16  MHal_LD_LR_GetGuradbandXStR(void)
{
    return MHal_LD_Read2Byte(REG_LD_LR_GBDEXST_R);
}

U16  MHal_LD_LR_GetGuradbandXEdR(void)
{
    return MHal_LD_Read2Byte(REG_LD_LR_GBDEXEND_R);
}

#endif
