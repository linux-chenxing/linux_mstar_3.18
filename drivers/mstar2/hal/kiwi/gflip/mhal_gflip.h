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
/// @file   mhal_gflip.h
/// @brief  MStar GFLIP Driver DDI HAL Level
/// @author MStar Semiconductor Inc.
/// @attention
/// <b>(OBSOLETED) <em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_GFLIP_H
#define _HAL_GFLIP_H

#ifdef _HAL_GFLIP_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

//=============================================================================
// Includs
//=============================================================================

//=============================================================================
// Defines & Macros
//=============================================================================
#define MAX_GOP_SUPPORT         4
#define MAX_GOP0_GWIN            4
#define MAX_GOP1_GWIN            4
#define MAX_GOP2_GWIN            1
#define MAX_GOP3_GWIN            1
#define MAX_GOP_GWIN              MAX_GOP0_GWIN

#define GFLIP_GOP_BANKOFFSET    (0x3)
#define GFLIP_ADDR_ALIGN_RSHIFT (0x4)

#define GFLIP_GOP_IDX_4G             (0x0)
#define GFLIP_GOP_IDX_4GX            (0x1)
#define GFLIP_GOP_IDX_1G             (0x2)
#define GFLIP_GOP_IDX_1GX            (0x3)
#define GFLIP_GOP_IDX_DWIN           (0x4)
#define GFLIP_GOP_IDX_INVALID        (0xFFFFFFFF)

#define GFLIP_GOP_DWIN_BANKID     12

#define GFLIP_GOP_BANK_IDX_0     (0x0)
#define GFLIP_GOP_BANK_IDX_1     (0x1)

#define GFLIP_GOP_DST_IPMAIN   0x0
#define GFLIP_GOP_DST_MIXER    0x1
#define GFLIP_GOP_DST_OP0      0x2
#define GFLIP_GOP_DST_VE       0x3

#define DLC_FUNCTION  1

#if DLC_FUNCTION
#define BYTE MS_U8
#define WORD MS_U16
#define DWORD MS_U32
#define XDATA
#define extern
#define printf printk
#define DLC_DEBUG(x)        //(x)

#define REG_ADDR_DLC_HANDSHAKE                 L_BK_DLC(0x04)
#define REG_ADDR_DLC_HISTOGRAM_MODE            H_BK_DLC(0x04)
#define REG_ADDR_HISTOGRAM_TOTAL_SUM_L         L_BK_DLC(0x06)
#define REG_ADDR_HISTOGRAM_TOTAL_SUM_H         H_BK_DLC(0x06)
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_L       L_BK_DLC(0x07)
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_H       H_BK_DLC(0x07)
#define REG_ADDR_HISTOGRAM_RANGE_ENABLE        L_BK_DLC(0x08)
#define REG_ADDR_BLE_UPPER_BOND                L_BK_DLC(0x09)
#define REG_ADDR_BLE_LOWER_BOND                H_BK_DLC(0x09)
#define REG_ADDR_WLE_UPPER_BOND                L_BK_DLC(0x0A)
#define REG_ADDR_WLE_LOWER_BOND                H_BK_DLC(0x0A)
#define REG_ADDR_MAIN_MAX_VALUE                L_BK_DLC(0x0B)
#define REG_ADDR_MAIN_MIN_VALUE                H_BK_DLC(0x0B)
#define REG_ADDR_SUB_MAX_VALUE                 L_BK_DLC(0x0C)
#define REG_ADDR_SUB_MIN_VALUE                 H_BK_DLC(0x0C)
#define REG_ADDR_MAIN_BLACK_SLOP_LSB_L         L_BK_DLC(0x0D)
#define REG_ADDR_MAIN_BLACK_SLOP_L             L_BK_DLC(0x10)
#define REG_ADDR_MAIN_BLACK_SLOP_H             H_BK_DLC(0x10)
#define REG_ADDR_DLC_MAIN_Y_GAIN               L_BK_DLC(0x14)
#define REG_ADDR_DLC_MAIN_C_GAIN               H_BK_DLC(0x14)
#define REG_ADDR_DLC_SUB_Y_GAIN                L_BK_DLC(0x15)
#define REG_ADDR_DLC_SUB_C_GAIN                H_BK_DLC(0x15)
#define REG_ADDR_HISTOGRAM_DATA_8              L_BK_DLC(0x28)
#define REG_ADDR_DLC_DATA_START_MAIN           L_BK_DLC(0x30)
#define REG_ADDR_DLC_DATA_START_SUB            L_BK_DLC(0x38)
#define REG_ADDR_HISTOGRAM_DATA_32             L_BK_DLC(0x40)
#define REG_ADDR_HISTOGRAM_SUB_TOTAL_COUNT_L   L_BK_DLC(0x72)
#define REG_ADDR_HISTOGRAM_SUB_TOTAL_COUNT_H   H_BK_DLC(0x72)
#define REG_ADDR_HISTOGRAM_SUB_TOTAL_SUM_L     L_BK_DLC(0x73)
#define REG_ADDR_HISTOGRAM_SUB_TOTAL_SUM_H     H_BK_DLC(0x73)
#define REG_ADDR_DLC_DATA_EXTEND_N0_MAIN       L_BK_DLC(0x76)
#define REG_ADDR_DLC_DATA_EXTEND_16_MAIN       L_BK_DLC(0x77)
#define REG_ADDR_DLC_DATA_LSB_START_MAIN       L_BK_DLC(0x78)
#define REG_ADDR_DLC_DATA_LSB_START_SUB        L_BK_DLC(0x7B)
#define REG_ADDR_DLC_DATA_EXTEND_N0_SUB        L_BK_DLC(0x7E)
#define REG_ADDR_DLC_DATA_EXTEND_16_SUB        L_BK_DLC(0x7F)

#define REG_ADDR_VOP_SCREEN_CONTROL            L_BK_VOP(0x19)

#define REG_ADDR_OP_SW_SUB_ENABLE               L_BK_OP(0x10)

#define PRJ_MCNR

#define REG_LPLL_11_L                   (0x103100 | 0x11<< 1)
#define REG_LPLL_13_L                   (0x103100 | 0x13<< 1)

#define REG_SC_BK12_01_L                   (0x131200 | 0x01<< 1)

#define REG_SC_BK0A_02_L                   (0x130A00 | 0x02<< 1)
#define REG_SC_BK0A_05_L                   (0x130A00 | 0x05<< 1)
#define REG_SC_BK0A_06_L                   (0x130A00 | 0x06<< 1)
#define REG_SC_BK0A_07_L                   (0x130A00 | 0x07<< 1)
#define REG_SC_BK0A_0A_L                   (0x130A00 | 0x0A<< 1)
#define REG_SC_BK0A_0B_L                   (0x130A00 | 0x0B<< 1)
#define REG_SC_BK0A_0C_L                   (0x130A00 | 0x0C<< 1)
#define REG_SC_BK0A_0D_L                   (0x130A00 | 0x0D<< 1)
#define REG_SC_BK0A_0F_L                   (0x130A00 | 0x0F<< 1)

#define REG_SC_BK0A_10_L                   (0x130A00 | 0x10<< 1)
#define REG_SC_BK0A_15_L                   (0x130A00 | 0x15<< 1)
#define REG_SC_BK0A_16_L                   (0x130A00 | 0x16<< 1)
#define REG_SC_BK0A_17_L                   (0x130A00 | 0x17<< 1)
#define REG_SC_BK0A_1E_L                   (0x130A00 | 0x1E<< 1)
#define REG_SC_BK0A_1F_L                   (0x130A00 | 0x1F<< 1)

#define REG_SC_BK0A_20_L                   (0x130A00 | 0x20<< 1)
#define REG_SC_BK0A_21_L                   (0x130A00 | 0x21<< 1)
#define REG_SC_BK0A_23_L                   (0x130A00 | 0x23<< 1)
#define REG_SC_BK0A_24_L                   (0x130A00 | 0x24<< 1)
#define REG_SC_BK0A_25_L                   (0x130A00 | 0x25<< 1)
#define REG_SC_BK0A_41_L                   (0x130A00 | 0x41<< 1)
#define REG_SC_BK0A_4A_L                   (0x130A00 | 0x4A<< 1)
#define REG_SC_BK0A_4B_L                   (0x130A00 | 0x4B<< 1)

#define REG_SC_BK0A_57_L                   (0x130A00 | 0x57<< 1)
#define REG_SC_BK0A_5C_L                   (0x130A00 | 0x5C<< 1)
#define REG_SC_BK0A_5E_L                   (0x130A00 | 0x5E<< 1)

#define REG_SC_BK12_03_L                   (0x131200 | 0x03<< 1)
#define REG_SC_BK12_0F_L                   (0x131200 | 0x0F<< 1)
#define REG_SC_BK12_30_L                   (0x131200 | 0x30<< 1)
#define REG_SC_BK12_33_L                   (0x131200 | 0x33<< 1)
#define REG_SC_BK12_3A_L                   (0x131200 | 0x3A<< 1)
#define REG_SC_BK22_2A_L                   (0x132200 | 0x2A<< 1)
#define REG_SC_BK22_7A_L                   (0x132200 | 0x7A<< 1)
#define REG_SC_BK22_7C_L                   (0x132200 | 0x7C<< 1)

#define REG_SC_BK2A_02_L                   (0x132A00 | 0x02<< 1)
#define REG_SC_BK2A_29_L                   (0x132A00 | 0x29<< 1)
#define REG_SC_BK2A_2A_L                   (0x132A00 | 0x2A<< 1)
#define REG_SC_BK2A_2F_L                   (0x132A00 | 0x2F<< 1)
#define REG_SC_BK2A_7C_L                   (0x132A00 | 0x7C<< 1)
#define REG_SC_BK2A_7D_L                   (0x132A00 | 0x7D<< 1)
#define REG_SC_BK2A_7E_L                   (0x132A00 | 0x7E<< 1)

#define REG_SC_BK2D_60_L                        (0x132D00 | 0x60<< 1)
#define REG_SC_BK2D_61_L                        (0x132D00 | 0x61<< 1)
#define REG_SC_BK2D_62_L                        (0x132D00 | 0x62<< 1)
#define REG_SC_BK2D_63_L                        (0x132D00 | 0x63<< 1)
#define REG_SC_BK2D_64_L                        (0x132D00 | 0x64<< 1)
#define REG_SC_BK2D_65_L                        (0x132D00 | 0x65<< 1)
#define REG_SC_BK2D_66_L                        (0x132D00 | 0x66<< 1)
#define REG_SC_BK2D_67_L                        (0x132D00 | 0x67<< 1)
#define REG_SC_BK2D_68_L                        (0x132D00 | 0x68<< 1)
#define REG_SC_BK2D_69_L                        (0x132D00 | 0x69<< 1)
#define REG_SC_BK2D_6A_L                        (0x132D00 | 0x6A<< 1)
#define REG_SC_BK2D_6B_L                        (0x132D00 | 0x6B<< 1)
#define REG_SC_BK2D_6C_L                        (0x132D00 | 0x6C<< 1)
#define REG_SC_BK2D_6D_L                        (0x132D00 | 0x6D<< 1)
#define REG_SC_BK2D_6E_L                        (0x132D00 | 0x6E<< 1)
#define REG_SC_BK2D_6F_L                        (0x132D00 | 0x6F<< 1)
#define REG_SC_BK2D_70_L                        (0x132D00 | 0x70<< 1)
#define REG_SC_BK2D_71_L                        (0x132D00 | 0x71<< 1)

#define REG_SC_BK30_01_L                        (0x133000 | (0x01<< 1))
#define REG_SC_BK30_02_L                        (0x133000 | (0x02<< 1))
#define REG_SC_BK30_02_H                        (0x133000 | ((0x02<< 1)+1))
#define REG_SC_BK30_03_L                        (0x133000 | (0x03<< 1))
#define REG_SC_BK30_04_L                        (0x133000 | (0x04<< 1))

#define REG_SC_BK30_06_L                        (0x133000 | (0x06<< 1))
#define REG_SC_BK30_06_H                        (0x133000 | ((0x06<< 1)+1))
#define REG_SC_BK30_07_L                        (0x133000 | (0x07<< 1))
#define REG_SC_BK30_07_H                        (0x133000 | ((0x07<< 1)+1))
#define REG_SC_BK30_08_L                        (0x133000 | (0x08<< 1))
#define REG_SC_BK30_08_H                        (0x133000 | ((0x08<< 1)+1))
#define REG_SC_BK30_09_L                        (0x133000 | (0x09<< 1))
#define REG_SC_BK30_09_H                        (0x133000 | ((0x09<< 1)+1))
#define REG_SC_BK30_0A_L                        (0x133000 | (0x0A<< 1))
#define REG_SC_BK30_0A_H                        (0x133000 | ((0x0A<< 1)+1))
#define REG_SC_BK30_0B_L                        (0x133000 | (0x0B<< 1))
#define REG_SC_BK30_0B_H                        (0x133000 | ((0x0B<< 1)+1))
#define REG_SC_BK30_0C_L                        (0x133000 | (0x0C<< 1))
#define REG_SC_BK30_0C_H                        (0x133000 | ((0x0C<< 1)+1))
#define REG_SC_BK30_0D_L                        (0x133000 | (0x0D<< 1))
#define REG_SC_BK30_0D_H                        (0x133000 | ((0x0D<< 1)+1))
#define REG_SC_BK30_0E_L                        (0x133000 | (0x0E<< 1))
#define REG_SC_BK30_0E_H                        (0x133000 | ((0x0E<< 1)+1))
#define REG_SC_BK30_0F_L                        (0x133000 | (0x0F<< 1))
#define REG_SC_BK30_0F_H                        (0x133000 | ((0x0F<< 1)+1))

#define REG_SC_BK62_06_L                    (0x136200 | 0x06<< 1)
#define REG_SC_BK62_0E_L                    (0x136200 | 0x0E<< 1)
#define REG_SC_BK62_08_L                    (0x136200 | 0x08<< 1)
#define REG_SC_BK63_61_L                    (0x136300 | 0x61<< 1)

#define _BIT0           (0x0001)
#define _BIT1           (0x0002)
#define _BIT2           (0x0004)
#define _BIT3           (0x0008)
#define _BIT4           (0x0010)
#define _BIT5           (0x0020)
#define _BIT6           (0x0040)
#define _BIT7           (0x0080)

#define L_BK_VOP(_x_)                 (REG_SCALER_BASE | (REG_SC_BK_VOP << 8) | (_x_ << 1))
#define L_BK_DLC(_x_)                 (REG_SCALER_BASE | (REG_SC_BK_DLC << 8) | (_x_ << 1))
#define H_BK_DLC(_x_)                 (REG_SCALER_BASE | (REG_SC_BK_DLC << 8) | ((_x_ << 1)+1))
#define L_BK_OP(_x_)                  (REG_SCALER_BASE | (REG_SC_BK_OP << 8)  | (_x_ << 1))

#define REG_SCALER_BASE             0x130000
#define REG_SC_BK_VOP               0x10
#define REG_SC_BK_DLC               0x1A
#define REG_SC_BK_OP                0x20

#define REG_ADDR(addr)              (*((volatile unsigned short int*)(0xBF000000 + (addr << 1))))

// read 2 byte
#define REG_RR(_reg_)                ({ REG_ADDR(_reg_);})

// write low byte
#define REG_WL(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) = (REG_ADDR(_reg_) & 0xFF00) | ((_val_) & 0x00FF); }while(0)

// write high byte
#define REG_WH(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) = (REG_ADDR(_reg_)  & 0x00FF) | ((_val_) << 8); }while(0)

 // write 2 byte
#define REG_W2B(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) =(_val_) ; }while(0)

#define MAIN_WINDOW                                0
#define SUB_WINDOW                                 1
#define msDlc_FunctionExit()
#define msDlc_FunctionEnter()

#define DLC_DEFLICK_BLEND_FACTOR                    32UL
#define XC_DLC_ALGORITHM_KERNEL                     2

#define XC_DLC_SET_DLC_CURVE_BOTH_SAME    0   // 0:Main and Sub use the same DLC curve


#endif
//=============================================================================
// HAL Driver Function
//=============================================================================

INTERFACE MS_BOOL MHal_GFLIP_IntEnable(MS_U32 u32GopIdx, MS_BOOL bEnable);
INTERFACE MS_BOOL MHal_GFLIP_IsVSyncInt(MS_U32 u32GopIdx);
INTERFACE MS_U32 MHal_GFLIP_GetIntGopIdx(void);
INTERFACE MS_BOOL MHal_GFLIP_IsGOPACK(MS_U32 u32GopIdx);
INTERFACE void      MHal_GFLIP_Fire(MS_U32 u32GopBitMask);
INTERFACE MS_BOOL MHal_GFLIP_SetFlipToGop(MS_U32 u32GopIdx, MS_U32 u32GwinIdx, MS_U32 u32MainAddr, MS_U32 u32SubAddr, MS_BOOL bForceWriteIn);

INTERFACE MS_BOOL MHal_GFLIP_ClearDWINIRQ(GFLIP_DWININT_INFO *pGFlipDWinIntInfo);

INTERFACE MS_BOOL MHal_GFLIP_HandleVsyncLimitation(MS_U32 u32GopIdx);
INTERFACE MS_BOOL MHal_GFLIP_RestoreFromVsyncLimitation(MS_U32 u32GopIdx);

INTERFACE MS_BOOL MHal_GFLIP_IsTagIDBack(MS_U16 u16TagId);
INTERFACE MS_BOOL MHal_GFLIP_VECaptureEnable(MS_BOOL bEnable);
INTERFACE MS_BOOL MHal_GFLIP_CheckVEReady(void);
INTERFACE MS_U16 MHal_GFLIP_GetGopDst(MS_U32 u32GopIdx);
INTERFACE MS_U8 MHal_GFLIP_GetFrameIdx(void);
INTERFACE MS_U32 MHal_GFLIP_GetValidGWinIDPerGOPIdx(MS_U32 u32GopIdx, MS_U32 u32GwinIdx);
INTERFACE MS_U8 MHal_GFLIP_GetBankOffset(MS_U32 u32GopIdx, MS_U16 u16BankIdx);
INTERFACE void MHal_GFLIP_WriteGopReg(MS_U32 u32GopIdx, MS_U16 u16BankIdx, MS_U16 u16Addr, MS_U16 u16Val, MS_U16 u16Mask);
INTERFACE void MHal_GFLIP_ReadGopReg(MS_U32 u32GopIdx, MS_U16 u16BankIdx, MS_U16 u16Addr, MS_U16* u16Val);

INTERFACE void      MApi_GFLIP_XC_W2BYTE(DWORD u32Reg, WORD u16Val );
INTERFACE WORD      MApi_GFLIP_XC_R2BYTE(DWORD u32Reg );
INTERFACE void      MApi_GFLIP_XC_W2BYTEMSK(DWORD u32Reg, WORD u16Val, WORD u16Mask );
INTERFACE WORD      MApi_GFLIP_XC_R2BYTEMSK(DWORD u32Reg, WORD u16Mask );

//------------------------------------------------------------------------
// HDR Begin
//------------------------------------------------------------------------

///HDR Code////
typedef struct
{
    U16 smin; // 0.10
    U16 smed; // 0.10
    U16 smax; // 0.10
    U16 tmin; // 0.10
    U16 tmed; // 0.10
    U16 tmax; // 0.10
    U16 slope; // 4.12
    U16 rolloff; // 4.12
} HDR_ToneMappingData;

typedef struct
{
    BOOL sValid; // source data valid
    BOOL tValid; // target data valid
    U16 sRx; // source Rx
    U16 sRy; // source Ry
    U16 sGx; // source Gx
    U16 sGy; // source Gy
    U16 sBx; // source Bx
    U16 sBy; // source By
    U16 sWx; // source Wx
    U16 sWy; // source Wy
    U16 tRx; // target Rx
    U16 tRy; // target Ry
    U16 tGx; // target Gx
    U16 tGy; // target Gy
    U16 tBx; // target Bx
    U16 tBy; // target By
    U16 tWx; // target Wx
    U16 tWy; // target Wy
} HDR_GamutMappingData;

typedef struct
{
    BOOL sValid; // source data valid
    BOOL tValid; // target data valid
    U8 sMode; // 0:gamma, 1:DCI-P3, 2:PQ, 3:Log
    //U16 sG; // 2.14
    //U16 sB; // 0.16
    U16 sData[4]; //[0]:sG [1]:sB
    U8 tMode; // 0:gamma, 1:DCI-P3, 2:PQ, 3:Log
    U16 tData[4];
} HDR_TransferFunctionData;

// Data format follows HDR_Metadata_MpegSEI_MasteringColorVolume
typedef struct
{
    U16     display_primaries_x[3]; // x = data*0.00002    [709] {15000, 7500, 32000}
    U16     display_primaries_y[3]; // y = data*0.00002    [709] {30000, 3000, 16500}
    U16     white_point_x; // x = data*0.00002    [709] 15635
    U16     white_point_y; // y = data*0.00002    [709] 16450
    U32     max_display_mastering_luminance; // data*0.0001 nits    [600nits] 6000000
    U32     min_display_mastering_luminance; // data*0.0001 nits    [0.3nits] 3000
} HDR_Metadata_FromPanel;

// Document : w14700 SEI - D.2.15 : Tone mapping information SEI message syntax
typedef struct
{
    U8      tone_map_id;
    BOOL    tone_map_cancel_flag;
    BOOL    tone_map_persistence_flag;
    U8      coded_data_bit_depth;
    U8      target_bit_depth;
    U8      tone_map_model_id;
    U32     min_value;
    U32     max_value;
    U32     sigmoid_midpoint;
    U32     sigmoid_width;
    U16*    start_of_coded_interval; // array size = (1 << target_bit_depth)
    U16     num_pivots;
    U16*    coded_pivot_value;  // array size = num_pivots
    U16*    target_pivot_value; // array size = num_pivots
    U8      camera_iso_speed_idc;
    U32     camera_iso_speed_value;
    U8      exposure_index_idc;
    U32     exposure_index_value;
    BOOL    exposure_compensation_value_sign_flag;
    U16     exposure_compensation_value_numerator;
    U16     exposure_compensation_value_denom_idc;
    U32     ref_screen_luminance_white;
    U32     extended_range_white_level;
    U16     nominal_black_level_code_value;
    U16     nominal_white_level_code_value;
    U16     extended_white_level_code_value;
} HDR_Metadata_MpegSEI_ToneMapping;

// Document : w14700 SEI - D.2.27 : Mastering display colour volume SEI message syntax
typedef struct
{
    U16     display_primaries_x[3]; // x = data*0.00002
    U16     display_primaries_y[3]; // y = data*0.00002
    U16     white_point_x; // x = data*0.00002
    U16     white_point_y; // y = data*0.00002
    U32     max_display_mastering_luminance; // data*0.0001 nits
    U32     min_display_mastering_luminance; // data*0.0001 nits
} HDR_Metadata_MpegSEI_MasteringColorVolume;

// Document : w14700 SEI - D.2.31 : Knee function information SEI message syntax
typedef struct
{
    U8      knee_function_id;
    BOOL    knee_function_cancel_flag;
    BOOL    knee_function_persistence_flag;
    U32     input_d_range;
    U32     input_disp_luminance;
    U32     output_d_range;
    U32     output_disp_luminance;
    U16     num_knee_points_minus1; // num_knee_points_minus1 shall be in the range of 0 to 998, inclusive.
    U16*    input_knee_point; // array size = num_knee_points_minus1+1
    U16*    output_knee_point; // array size = num_knee_points_minus1+1
} HDR_Metadata_MpegSEI_KneeFunction;

// Document : CEA-861.3_V16BallotDraft
typedef struct
{
    U8      u8EOTF; // 0:SDR gamma, 1:HDR gamma, 2:SMPTE ST2084, 3:Future EOTF, 4-7:Reserved
    U8      u8Lmax; // desired content max luminance data (8 bits)
    U8      u8Lavg; // desired content max frame-average luminance data (8 bits)
    U8      u8Lmin; // desired content min luminance data (8 bits)
} HDR_Metadata_HdmiRx_DataBlock;

void MDrv_HDR_SetEnable(BOOL bEnable);
void MDrv_HDR_ConfigMetadata_MpegVUI( HDR_TransferFunctionData* pTFData, HDR_GamutMappingData* pGMData);
INTERFACE void MDrv_HDR_Main(HDR_TransferFunctionData* stTFData ,MS_U32 *u32LutData );
void MDrv_HDR_ToneMappingCurveGen(U16* pCurve18, HDR_ToneMappingData* pData,BOOL bWindow);
INTERFACE void MDrv_HDR_Set3x3Gen(BOOL bWrite);
INTERFACE BOOL msHDRInit(MS_U16 *u16HDRInit , MS_U16 u16HDRInitSize);


#if DLC_FUNCTION

void msDlcHandler(BOOL bWindow);
BOOL msGetHistogramHandler(BOOL bWindow);
void msDlcWriteCurve(BOOL bWindow);
void msDlcWriteCurveLSB(BOOL bWindow, BYTE ucIndex, BYTE ucValue);
void msDlcLumiDiffCtrl(void);
void msDlcSetCurve(MS_U8 *_u8DlcCurveInit);
void msDlcSetBleSlopPoint(MS_U16 *_u16BLESlopPoint);
BOOL msDlcInit(MS_U16 *u16DlcInit );
void msDlcSetBleOnOff(BOOL bSwitch);
BOOL msDlcGetHistogram32Info(MS_U16 *pu16Histogram);
MS_U8 msGetAverageLuminous(void);
MS_U8 msGetAverageLuminousSub(void);
BOOL msIsBlackVideoEnable( MS_BOOL bWindow );
BOOL msDLCMSTVToolReadWriteValue( void );
void msDlcSetYGain(MS_U8 u8YGain, BOOL bWindow);
MS_U8 msDlcGetYGain(BOOL bWindow);
void msDlcSetCGain(MS_U8 u8CGain, BOOL bWindow);
MS_U8 msDlcGetCGain(BOOL bWindow);

typedef struct
{
    // Default luma curve
    BYTE ucLumaCurve[16];
    BYTE ucLumaCurve2_a[16];
    BYTE ucLumaCurve2_b[16];

    BYTE ucDlcPureImageMode; // Compare difference of max and min bright
    BYTE ucDlcLevelLimit; // n = 0 ~ 4 => Limit n levels => ex. n=2, limit 2 level 0xF7, 0xE7
    BYTE ucDlcAvgDelta; // n = 0 ~ 50, default value: 12
    BYTE ucDlcAvgDeltaStill; // n = 0 ~ 15 => 0: disable still curve, 1 ~ 15 => 0.1 ~ 1.5 enable still curve
    BYTE ucDlcFastAlphaBlending; // min 17 ~ max 32
    BYTE ucDlcSlowEvent; // some event is triggered, DLC must do slowly // for PIP On/Off, msMultiPic.c
    BYTE ucDlcTimeOut; // for IsrApp.c
    BYTE ucDlcFlickAlphaStart; // for force to do fast DLC in a while
    BYTE ucDlcYAvgThresholdH; // default value: 128
    BYTE ucDlcYAvgThresholdL; // default value: 0
    BYTE ucDlcBLEPoint; // n = 24 ~ 64, default value: 48
    BYTE ucDlcWLEPoint; // n = 24 ~ 64, default value: 48
    BYTE bCGCCGainCtrl : 1; // 1: enable; 0: disable
    BYTE bEnableBLE : 1; // 1: enable; 0: disable
    BYTE bEnableWLE : 1; // 1: enable; 0: disable
    BYTE ucDlcYAvgThresholdM;
    BYTE ucDlcCurveMode;
    BYTE ucDlcCurveModeMixAlpha;
    BYTE ucDlcAlgorithmMode;
    BYTE ucDlcHistogramLimitCurve[17];
    BYTE ucDlcSepPointH;
    BYTE ucDlcSepPointL;
    WORD uwDlcBleStartPointTH;
    WORD uwDlcBleEndPointTH;
    BYTE ucDlcCurveDiff_L_TH;
    BYTE ucDlcCurveDiff_H_TH;
    WORD uwDlcBLESlopPoint_1;
    WORD uwDlcBLESlopPoint_2;
    WORD uwDlcBLESlopPoint_3;
    WORD uwDlcBLESlopPoint_4;
    WORD uwDlcBLESlopPoint_5;
    WORD uwDlcDark_BLE_Slop_Min;
    BYTE ucDlcCurveDiffCoringTH;
    BYTE ucDlcAlphaBlendingMin;
    BYTE ucDlcAlphaBlendingMax;
    BYTE ucDlcFlicker_alpha;
    BYTE ucDlcYAVG_L_TH;
    BYTE ucDlcYAVG_H_TH;
    BYTE ucDlcDiffBase_L;
    BYTE ucDlcDiffBase_M;
    BYTE ucDlcDiffBase_H;
}StuDlc_FinetuneParamaters;

#endif

typedef struct __attribute__((packed))
{
    MS_U8      u8ColorPrimaries;
    MS_U8      u8TransferCharacteristics;
    MS_U8      u8MatrixCoefficients;
} StuDlc_HDRMetadataMpegVUI;

///HDR Code////
typedef struct __attribute__((packed))
{
    MS_U16 u16Smin; // 0.10
    MS_U16 u16Smed; // 0.10
    MS_U16 u16Smax; // 0.10
    MS_U16 u16Tmin; // 0.10
    MS_U16 u16Tmed; // 0.10
    MS_U16 u16Tmax; // 0.10
    MS_U16 u16MidSourceOffset;
    MS_U16 u16MidTargetOffset;
    MS_U16 u16MidSourceRatio;
    MS_U16 u16MidTargetRatio;
} StuDlc_HDRToneMappingData;

typedef struct __attribute__((packed))
{
    MS_U16 u16tRx; // target Rx
    MS_U16 u16tRy; // target Ry
    MS_U16 u16tGx; // target Gx
    MS_U16 u16tGy; // target Gy
    MS_U16 u16tBx; // target Bx
    MS_U16 u16tBy; // target By
    MS_U16 u16tWx; // target Wx
    MS_U16 u16tWy; // target Wy
} StuDlc_HDRGamutMappingData;

// Document : CEA-861.3_V16BallotDraft
typedef struct __attribute__((packed))
{
    MS_U8      u8EOTF; // 0:SDR gamma, 1:HDR gamma, 2:SMPTE ST2084, 3:Future EOTF, 4-7:Reserved
    MS_U16     u16Rx; // display primaries Rx
    MS_U16     u16Ry; // display primaries Ry
    MS_U16     u16Gx; // display primaries Gx
    MS_U16     u16Gy; // display primaries Gy
    MS_U16     u16Bx; // display primaries Bx
    MS_U16     u16By; // display primaries By
    MS_U16     u16Wx; // display primaries Wx
    MS_U16     u16Wy; // display primaries Wy
    MS_U16     u16Lmax; // max display mastering luminance
    MS_U16     u16Lmin; // min display mastering luminance
    MS_U16     u16MaxCLL; // maximum content light level
    MS_U16     u16MaxFALL; // maximum frame-average light level
} StuDlc_HDRMetadataHdmiTxInfoFrame;

// HDR use customer DLC curve.
typedef struct __attribute__((packed))
{
    MS_BOOL bFixHdrCurve;
    MS_U16 u16DlcCurveSize;
    MS_U8 *pucDlcCurve;
} StuDLC_HDRCustomerDlcCurve;

// HDR use customer color primaries.
typedef struct __attribute__((packed))
{
    MS_BOOL bCustomerEnable;
    MS_U16 u16sWx;
    MS_U16 u16sWy;
} StuDlc_HDRCustomerColorPrimaries;

/*!
 *  Initial  HDR   Settings
 */
typedef struct __attribute__((packed))
{
    /// HDR Enable
    MS_BOOL bHDREnable;
    /// HDR Function Select
    MS_U16 u16HDRFunctionSelect;
    /// HDR Metadata Mpeg VUI
    StuDlc_HDRMetadataMpegVUI DLC_HDRMetadataMpegVUI;
    //HDR Tone Mapping Data
    StuDlc_HDRToneMappingData DLC_HDRToneMappingData;
    //HDR Gamut Mapping Data
    StuDlc_HDRGamutMappingData DLC_HDRGamutMappingData;
    //HDR Metadata Hdmi Tx Info Frame
    StuDlc_HDRMetadataHdmiTxInfoFrame DLC_HDRMetadataHdmiTxInfoFrame;
    // Customer DLC Curve
    StuDLC_HDRCustomerDlcCurve DLC_HDRCustomerDlcCurve;
    // Customer color primarie.
    StuDlc_HDRCustomerColorPrimaries DLC_HDRCustomerColorPrimaries;
} StuDlc_HDRinit;

#undef INTERFACE
#endif //_HAL_GFLIP_H

