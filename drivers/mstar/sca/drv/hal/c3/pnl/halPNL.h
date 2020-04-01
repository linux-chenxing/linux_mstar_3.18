////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   halPNL.h
/// @brief  Panel Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_PNL_H_
#define _HAL_PNL_H_

#ifdef __cplusplus
extern "C" {
#endif

// XC register serpead define
#define XC_REGISTER_SPREAD 1
#define SUPPORT_FRC       0


//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------
#define  GAMMA_10BIT              BIT(0)            ///< gamma value range up to 10 BIt
#define  GAMMA_12BIT              BIT(1)            ///< gamma value range up to 12 BIT

#define  GAMMA_8BIT_MAPPING       BIT(0)            ///< mapping GAMMA value to 256 sampline entries
#define  GAMMA_10BIT_MAPPING      BIT(1)            ///< mapping GAMMA value to 1024 sampling entries

typedef struct
{
    MS_U8 eSupportGammaType;                        ///< refer to HAL_PNL_GAMMA_TYPE
    MS_U8 eSupportGammaMapMode;                       ///< refero to HAL_PNL_GAMMA_MAPPEING_MODE
} PNL_HalInfo;

#define SUPPORT_OVERDRIVE                   1
#define GAMMA_TYPE                  (GAMMA_10BIT | GAMMA_12BIT)
#define GAMMA_MAPPING               (GAMMA_8BIT_MAPPING | GAMMA_10BIT_MAPPING)
#define SUPPORT_SYNC_FOR_DUAL_MODE			TRUE  //New feature after T7
#define ENABLE_Auto_ModCurrentCalibration   1
#define ENABLE_MODE_PATCH	0

// MIU Word (Bytes)
#define BYTE_PER_WORD           (32)

#define SUPPORT_TCON            FALSE
//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------


#define BK_REG_L( x, y )            ((x) | (((y) << 1)))
#define BK_REG_H( x, y )            (((x) | (((y) << 1))) + 1)

// NONPM
#define REG_CHIPTOP_BASE            0x100B00  // 0x1E00 - 0x1EFF
#if XC_REGISTER_SPREAD
#define REG_SCALER_BASE             0x130000
#else
#define REG_SCALER_BASE             0x102F00
#endif
#define REG_HDGEN_BASE              0x103000
#define REG_LPLL_BASE               0x103100
#define REG_MOD_BASE                0x103200
#define REG_UTMI1_BASE              0x103A00
#define REG_GPIO0_BASE              0x102B00
#define REG_GPIO1_BASE              0x101A00

/* TCON */
#define L_BK_TCON(x)                BK_REG_L(REG_HDGEN_BASE, x)
#define H_BK_TCON(x)                BK_REG_H(REG_HDGEN_BASE, x)

/* LPLL */
#define L_BK_LPLL(x)                BK_REG_L(REG_LPLL_BASE, x)
#define H_BK_LPLL(x)                BK_REG_H(REG_LPLL_BASE, x)

/* UTMI1 */
#define L_BK_UTMI1(x)               BK_REG_L(REG_UTMI1_BASE, x)
#define H_BK_UTMI1(x)               BK_REG_H(REG_UTMI1_BASE, x)

#define REG_CLKGEN0_52_L            (REG_CHIPTOP_BASE + 0xA4)
#define REG_CLKGEN0_53_L            (REG_CHIPTOP_BASE + 0xA6)
#define REG_CLKGEN0_57_L            (REG_CHIPTOP_BASE + 0xAE)
#define REG_CLKGEN0_58_L            (REG_CHIPTOP_BASE + 0xB0)
#define REG_CLKGEN0_59_L            (REG_CHIPTOP_BASE + 0xB2)
#define REG_CLKGEN0_5E_L            (REG_CHIPTOP_BASE + 0xBC)

#define REG_GPIO1_16_L              (REG_GPIO1_BASE + 0x2C)
#define REG_GPIO0_55_L              (REG_GPIO0_BASE + 0xAA)

#define XC_PAFRC_DITH_NOISEDITH_EN          (0x00)
#define XC_PAFRC_DITH_TAILCUT_DISABLE       (0x00)

#define LVDS_DUAL_OUTPUT          0
#define LVDS_DUAL_OUTPUT_SPECIAL  1// only for use with T8 board
#define LVDS_SINGLE_OUTPUT_A      2
#define LVDS_SINGLE_OUTPUT_B      3
#define LVDS_OUTPUT_User          4

// SCALER CLK select
#define REG_CKG_ODCLK                REG_CLKGEN0_53_L
    #define CKG_ODCLK_GATED          BIT(0)
    #define CKG_ODCLK_INVERT         BIT(1)
    #define CKG_ODCLK_MASK           (BIT(3) | BIT(2))
    #define CKG_ODCLK_CLK_SC_PLL     (0 << 2)
    #define CKG_ODCLK_27M            (2 << 2)
    #define CKG_ODCLK_CLK_LPLL       (3 << 2)
    #define CKG_ODCLK_CLK_LPLL_DIV_2 (1 << 2)

#define REG_CKG_BT656               REG_CLKGEN0_53_L
    #define CKG_BT656_GATED         BIT(8)
    #define CKG_BT656_INVERT        BIT(9)
    #define CKG_BT656_MASK          (BIT(11) | BIT(10))
    #define CKG_BT656_CLK_SC_PLL    (0 << 10)
    #define CKG_BT656_CLK_LPLL_DIV_2 (1 << 10)
    #define CKG_BT656_27M           (2 << 10)
    #define CKG_BT656_CLK_LPLL      (3 << 10)

#define REG_CKG_TX_MOD              REG_CLKGEN0_58_L
    #define CKG_TX_MOD_GATED         BIT(0)
    #define CKG_TX_MOD_INVERT        BIT(1)
    #define CKG_TX_MOD_MASK          (BIT(3) | BIT(2))
    #define CKG_TX_1X_4XDIGITAL      (0 << 2)

#define PANEL_LPLL_INPUT_DIV_1st          0x00
#define PANEL_LPLL_INPUT_DIV_2nd          0x00 // 0:/1, 1:/2, 2:/4, 3:/8
#define PANEL_LPLL_LOOP_DIV_1st           0x03 // 0:/1, 1:/2, 2:/4, 3:/8
#define PANEL_LPLL_LOOP_DIV_2nd           0x01 //
#define PANEL_LPLL_OUTPUT_DIV_1st         0x00 // 0:/1, 1:/2, 2:/4, 3:/8
#define PANEL_LPLL_OUTPUT_DIV_2nd         0x00
#define LPLL_LOOPGAIN                     16 // use at MHal_PNL_Get_LPLL_LoopGain()

#define LVDS_MPLL_CLOCK_MHZ     442 // For crystal 24Mhz
#define LVDS_SPAN_FACTOR        131072

#define VOP_DE_HSTART_MASK      (0x1FFF) //BK_10_04
#define VOP_DE_HEND_MASK        (0x1FFF) //BK_10_05
#define VOP_VSYNC_END_MASK      (0x0FFF) //BK_10_03
#define VOP_DISPLAY_HSTART_MASK (0x1FFF) //BK_10_08
#define VOP_DISPLAY_HEND_MASK   (0x1FFF) //BK_10_09


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_DRVPNL_ALLIN_MODE      = 1,
    E_DRVPNL_2X_MODE         = 2,
    E_DRVPNL_SEPARATE_MODE   = 3,
    E_DRVPNL_TYPE_NUM
}DRVPNL_OUT_SWING_TYPE;

typedef enum
{
    HAL_TI_10BIT_MODE = 0,
    HAL_TI_8BIT_MODE = 2,
    HAL_TI_6BIT_MODE = 3,
} PNL_HAL_TIMODES;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

MS_U8 MHal_MOD_PowerOn(MS_BOOL bEn, MS_U8 u8LPLL_Type,MS_U8 DualModeType, MS_U16 u16OutputCFG0_7, MS_U16 u16OutputCFG8_15, MS_U16 u16OutputCFG16_21);
MS_BOOL MHal_PNL_GetDACOut(void);
void MHal_PNL_TCON_Init(void);

void MHal_VOP_SetGammaMappingMode(MS_U8 u8Mapping);

MS_BOOL MHal_PNL_SetDbgLevel(MS_U16 u16DbgSwitch);

void MHal_Shift_LVDS_Pair(MS_U8 Type);
void MHal_Output_LVDS_Pair_Setting(MS_U8 Type, MS_U16 u16OutputCFG0_7, MS_U16 u16OutputCFG8_15, MS_U16 u16OutputCFG16_21);
void MHal_PQ_Clock_Gen_For_Gamma(void);

void MHal_VOP_SetGammaMappingMode(MS_U8 u8Mapping);
MS_BOOL Hal_VOP_Is_GammaMappingMode_enable(void);
MS_BOOL Hal_VOP_Is_GammaSupportSignalWrite(DRVPNL_GAMMA_MAPPEING_MODE u8Mapping);
void hal_PNL_WriteGamma12Bit(MS_U8 u8Channel, MS_BOOL bBurstWrite, MS_U16 u16Addr, MS_U16 u16GammaValue);
void hal_PNL_SetMaxGammaValue( MS_U8 u8Channel, MS_U16 u16MaxGammaValue);
void Hal_PNL_Set12BitGammaPerChannel( MS_U8 u8Channel, MS_U8 * u8Tab, DRVPNL_GAMMA_MAPPEING_MODE GammaMapMode );
void MHal_PNL_Init_LPLL(PNL_TYPE eLPLL_Type,PNL_MODE eLPLL_Mode,MS_U64 ldHz);
//void _MDrv_PNL_Set_12BIT_Gamma( MS_U8 u8Channel, MS_U8 * u8Tab );
MS_U16 MHal_PNL_Get_LPLL_LoopGain(MS_U8 eLPLL_Type, MS_U32 ldHz);

#define MHal_PNL_FRC_lpll_src_sel(args)

MS_U8 MHal_PNL_Get_Loop_DIV( MS_U8 u8LPLL_Mode, MS_U8 eLPLL_Type, MS_U32 ldHz);
MS_BOOL Hal_PNL_SkipTimingChange_GetCaps(void);

void MHal_PNL_HWLVDSReservedtoLRFlag(PNL_DrvHW_LVDSResInfo lvdsresinfo);
void MHal_PNL_OverDriver_Init(MS_U32 u32OD_MSB_Addr, MS_U32 u32OD_MSB_limit, MS_U32 u32OD_LSB_Addr, MS_U32 u32OD_LSB_limit);
void MHal_PNL_OverDriver_Enable(MS_BOOL bEnable);
void MHal_PNL_OverDriver_TBL(MS_U8 u8ODTbl[1056]);

void MHal_PNL_PreInit(PNL_OUTPUT_MODE eParam);
PNL_OUTPUT_MODE MHal_PNL_Get_Output_MODE(void);
void MHal_PNL_SetOutputType(PNL_OUTPUT_MODE eOutputMode, PNL_TYPE eLPLL_Type);
MS_BOOL MHal_PNL_MOD_Control_Out_Swing(MS_U16 u16Swing_Level);
MS_BOOL MHal_PNL_MOD_Control_Out_PE_Current (MS_U16 u16Current_Level);
MS_BOOL MHal_PNL_MOD_Control_Out_TTL_Resistor_OP (MS_BOOL  u16TTL_OP_Level);

void MHal_PNL_Init_MOD(PNL_InitData *pstPanelInitData);
void MHal_PNL_Init_XC_Clk(PNL_InitData *pstPanelInitData);
void MHal_PNL_DumpMODReg(MS_U32 u32Addr, MS_U16 u16Value, MS_BOOL bHiByte, MS_U16 u16Mask);
void MHal_MOD_Calibration_Init(MS_U8 U8MOD_CALI_TARGET, MS_S8 S8MOD_CALI_OFFSET);
void MHal_BD_LVDS_Output_Type(MS_U16 Type);
PNL_Result MHal_PNL_MOD_Calibration(void);
PNL_Result MHal_PNL_En(MS_BOOL bPanelOn, MS_BOOL bCalEn);
void MHal_PNL_SetOutputPattern(MS_BOOL bEnable, MS_U16 u16Red , MS_U16 u16Green, MS_U16 u16Blue);

void MHal_PNL_Switch_LPLL_SubBank(MS_U16 u16Bank);
void Mhal_PNL_Flock_LPLLSet(MS_U32 ldHz);


#ifdef __cplusplus
}
#endif

#endif // _HAL_PNL_H_

