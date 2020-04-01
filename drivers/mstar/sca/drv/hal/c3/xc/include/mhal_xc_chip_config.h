////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2009-2010 MStar Semiconductor, Inc.
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
#ifndef MHAL_XC_CONFIG_H
#define MHAL_XC_CONFIG_H

//-------------------------------------------------------------------------------------------------
//  C3
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Chip Configuration
//-------------------------------------------------------------------------------------------------

#define MAX_WINDOW_NUM          (6)
#define MAX_FRAME_NUM_IN_MEM    (4) // Progressive
#define MAX_FIELD_NUM_IN_MEM    (16) // Interlace
#define NUM_OF_DIGITAL_DDCRAM   (1)

// SC0
#define SCALER_LINE_BUFFER_MAX              (1366)
#define MST_LINE_BFF_MAX                    MAX(1366, SCALER_LINE_BUFFER_MAX)

#define SUB_MAIN_LINEOFFSET_GUARD_BAND      16
#define SUB_SCALER_LINE_BUFFER_MAX          682UL - SUB_MAIN_LINEOFFSET_GUARD_BAND
#define SUB_MST_LINE_BFF_MAX                SUB_SCALER_LINE_BUFFER_MAX

// SC1
#define SCALER_LINE_BUFFER_MAX_SC1          (1920UL)
#define MST_LINE_BFF_MAX_SC1                MAX(1920, SCALER_LINE_BUFFER_MAX_SC1)

// SC2
#define SCALER_LINE_BUFFER_MAX_SC2          (1366)
#define MST_LINE_BFF_MAX_SC2                MAX(1366, SCALER_LINE_BUFFER_MAX_SC2)

#define SCALER_LB_SIZE						(2048)
#define SCALER_LB_SIZE_SC1					(1092)
#define SCALER_LB_SIZE_SC2					(2048)

#define SUB_MAIN_LINEOFFSET_GUARD_BAND_SC2  16
#define SUB_SCALER_LINE_BUFFER_MAX_SC2      682UL - SUB_MAIN_LINEOFFSET_GUARD_BAND_SC2
#define SUB_MST_LINE_BFF_MAX_SC2            SUB_SCALER_LINE_BUFFER_MAX_SC2


#define MS_3D_LINE_BFF_MAX      (1440UL)

// MIU Word (Bytes)
#define BYTE_PER_WORD           (16)
#define OFFSET_PIXEL_ALIGNMENT  (32)
#define LPLL_LOOPGAIN           (16)
#define LVDS_MPLL_CLOCK_MHZ     (442)

#define DEFAULT_STEP_P          4 //conservative step value
#define DEFAULT_STEP_I          ((DEFAULT_STEP_P*DEFAULT_STEP_P)/2)
#define STEP_P                  2 //recommended step value -> more faster fpll(T3)
#define STEP_I                  ((STEP_P*STEP_P)/2)
#define IPGAIN_REFACTOR         5

#define F2_WRITE_LIMIT_EN   BIT(31) //BK12_1b[15]
#define F2_WRITE_LIMIT_MIN  BIT(30) //BK12_1b[14]

#define F1_WRITE_LIMIT_EN   BIT(31) //BK12_5b[15]
#define F1_WRITE_LIMIT_MIN  BIT(30) //BK12_5b[14]

#define F2_V_WRITE_LIMIT_EN    BIT(15) //BK12_18[12]
#define F1_V_WRITE_LIMIT_EN    BIT(15) //BK12_58[12]

#define ADC_MAX_CLK                     (3500)

#define SUPPORTED_XC_INT        ((1UL << SC_INT_VSINT) |            \
                                 (1UL << SC_INT_F2_VTT_CHG) |       \
                                 (1UL << SC_INT_F1_VTT_CHG) |       \
                                 (1UL << SC_INT_F2_VS_LOSE) |       \
                                 (1UL << SC_INT_F1_VS_LOSE) |       \
                                 (1UL << SC_INT_F2_JITTER) |        \
                                 (1UL << SC_INT_F1_JITTER) |        \
                                 (1UL << SC_INT_F2_IPVS_SB) |       \
                                 (1UL << SC_INT_F1_IPVS_SB) |       \
                                 (1UL << SC_INT_F2_IPHCS_DET) |     \
                                 (1UL << SC_INT_F1_IPHCS_DET) |     \
                                 (1UL << SC_INT_F2_HTT_CHG) |       \
                                 (1UL << SC_INT_F1_HTT_CHG) |       \
                                 (1UL << SC_INT_F2_HS_LOSE) |       \
                                 (1UL << SC_INT_F1_HS_LOSE) |       \
                                 (1UL << SC_INT_F2_CSOG) |          \
                                 (1UL << SC_INT_F1_CSOG) |          \
                                 (1UL << SC_INT_F2_ATP_READY) |     \
                                 (1UL << SC_INT_F1_ATP_READY))


//These table definition is from SC_BK0 spec.
//Because some chip development is different, it need to check and remap when INT function is used
#define IRQ_INT_START        4
#define IRQ_INT_RESERVED1     IRQ_INT_START

#define IRQ_INT_TUNE_FAIL_P		4
#define IRQ_INT_VSINT			5
#define IRQ_INT_F2_VTT_CHG		6
#define IRQ_INT_F1_VTT_CHG		7
#define IRQ_INT_F2_VS_LOSE		9
#define IRQ_INT_F1_VS_LOSE    9
#define IRQ_INT_F2_JITTER     10
#define IRQ_INT_F1_JITTER     11
#define IRQ_INT_F2_IPVS_SB    12
#define IRQ_INT_F1_IPVS_SB    13
#define IRQ_INT_F2_IPHCS_DET  14
#define IRQ_INT_F1_IPHCS_DET  15

#define IRQ_INT_F2_IPHCS1_DET  16
#define IRQ_INT_F1_IPHCS1_DET  17
#define IRQ_INT_F2_HTT_CHG    18
#define IRQ_INT_F1_HTT_CHG    19
#define IRQ_INT_F2_HS_LOSE    20
#define IRQ_INT_F1_HS_LOSE    21
#define IRQ_INT_DVI_CK_LOSE_F2  22
#define IRQ_INT_DVI_CK_LOSE_F1  23
#define IRQ_INT_F2_CSOG       24
#define IRQ_INT_F1_CSOG       25
#define IRQ_INT_F2_ATS_READY  26
#define IRQ_INT_F1_ATS_READY  27
#define IRQ_INT_F2_ATP_READY  28
#define IRQ_INT_F1_ATP_READY  29
#define IRQ_INT_F2_ATG_READY  30
#define IRQ_INT_F1_ATG_READY  31

//-------------------------------------------------------------------------------------------------
//  Chip Feature
//-------------------------------------------------------------------------------------------------
/* 12 frame mode for progessive */
#define _12FRAME_BUFFER_PMODE_SUPPORTED     0
/* 8 frame mode for progessive */
#define _8FRAME_BUFFER_PMODE_SUPPORTED      1
/* 6 frame mode for progessive */
#define _6FRAME_BUFFER_PMODE_SUPPORTED      0
/* 4 frame mode for progessive */
#define _4FRAME_BUFFER_PMODE_SUPPORTED      1
/* 3 frame mode for progessive */
#define _3FRAME_BUFFER_PMODE_SUPPORTED      1

/*
   Field-packing ( Customized name )
   This is a feature in M10. M10 only needs one IPM buffer address. (Other chips need two or three
   IPM buffer address). We show one of memory format for example at below.

   Block :       Y0      C0      L       M        Y1       C1
   Each block contain 4 fields (F0 ~ F3) and each fields in one block is 64 bits
   Y0 has 64 * 4 bits ( 8 pixel for each field ).
   Y1 has 64 * 4 bits ( 8 pixel for each field ).
   So, in this memory format, pixel alignment is 16 pixels (OFFSET_PIXEL_ALIGNMENT = 16).
   For cropping, OPM address offset have to multiple 4.
*/
#define _FIELD_PACKING_MODE_SUPPORTED       1

#if (_FIELD_PACKING_MODE_SUPPORTED)

/* Linear mode */
#define _LINEAR_ADDRESS_MODE_SUPPORTED      0

#else
/* Linear mode */
#define _LINEAR_ADDRESS_MODE_SUPPORTED      1

#endif

/* Because fix loop_div, lpll initial set is different between singal port and dual port */
#define _FIX_LOOP_DIV_SUPPORTED             1

// You can only enable ENABLE_8_FIELD_SUPPORTED or ENABLE_16_FIELD_SUPPORTED. (one of them)
// 16 field mode include 8 field configurion in it. ENABLE_8_FIELD_SUPPORTED is specital case in T7
#define ENABLE_8_FIELD_SUPPORTED            0
#define ENABLE_16_FIELD_SUPPORTED           1
#define ENABLE_OPM_WRITE_SUPPORTED          1
#define ENABLE_YPBPR_PRESCALING_TO_ORIGINAL 0
#define ENABLE_VD_PRESCALING_TO_DOT75       0
#define ENABLE_NONSTD_INPUT_MCNR            0
#define ENABLE_REGISTER_SPREAD              1

#define ENABLE_REQUEST_FBL                  1
#define DELAY_LINE_SC_UP                    7
#define DELAY_LINE_SC_DOWN                  8

#define SUPPORT_IMMESWITCH                  1
#define SUPPORT_DVI_AUTO_EQ                 1

// Special frame lock means that the frame rates of input and output are the same in HW design spec.
#define SUPPORT_SPECIAL_FRAMELOCK           FALSE

#define LD_ENABLE                           0

#define FRC_INSIDE                          FALSE


// 480p and 576p have FPLL problem in HV mode.
// So only allow HV mode for 720P
#define ONLY_ALLOW_HV_MODE_FOR_720P         1

// T12, T13 cannot use IP_HDMI for HV mode
// We must use IP_HDMI for HV mode, otherwise 480i 576i will have color space proble,m
//Note: if use IP_HDMI, MApi_XC_GetDEWindow() cannot get value correctly
// and IP_HDMI is set in MApi_XC_SetInputSource(), so cannot change dynamically
// Thus, chip could use this flag to determine whether could do HV mode or not.
#define SUPPORT_IP_HDMI_FOR_HV_MODE           1

#define HW_DESIGN_3D_VER                    (4)
#define HW_2DTO3D_SUPPORT                   FALSE
//#define HW_2DTO3D_PATCH   TRUE //a1 u01:2d to 3d hw bug
//M10, A2, J2 ,A5,A6,A3,Agate HW will automatic use IPM fetch's reg setting to alignment IPM fetch, so skip sw alignment
//and for mirror cbcr swap, need check IPM fetch to decide if need swap
#define HW_IPM_FETCH_ALIGNMENT              TRUE

#define HW_CLK_CTRL                         TRUE

//#define FA_1920X540_OUTPUT

//-------------------------------------------------------------------------------------------------
//  Register base
//-------------------------------------------------------------------------------------------------

// PM
#define REG_DDC_BASE                0x000400
#define REG_PM_SLP_BASE             0x000E00
#define REG_PM_SLEEP_BASE           REG_PM_SLP_BASE//0x0E00//alex_tung

// NONPM
#define REG_CHIPTOP_ORI_BASE 		0x101E00
#define REG_MIU0_BASE               0x101200
#define REG_MIU1_BASE               0x100600
#define REG_CHIPTOP_BASE            0x100B00  // 0x1E00 - 0x1EFF
#define REG_CLKGEN1_BASE            0x103300
#define REG_UHC0_BASE               0x102400
#define REG_UHC1_BASE               0x100D00
#define REG_ADC_ATOP_BASE           0x102500  // 0x2500 - 0x25FF
#define REG_ADC_DTOP_BASE           0x102600  // 0x2600 - 0x26EF
#define REG_HDMI_BASE               0x102700  // 0x2700 - 0x27FF
#define REG_HDMI2_BASE              0x101A00
#define REG_IPMUX_BASE              0x102E00
#if ENABLE_REGISTER_SPREAD
#define REG_SCALER_BASE             0x130000
#else
#define REG_SCALER_BASE             0x102F00
#endif
#define REG_LPLL_BASE               0x103100
#define REG_MOD_BASE                0x103200
#define REG_AFEC_BASE               0x103500
#define REG_COMB_BASE               0x103600


#define REG_DVI_ATOP_BASE           0x110900
#define REG_DVI_DTOP_BASE           0x110A00
#define REG_DVI_EQ_BASE             0x110A80     // EQ started from 0x80
#define REG_HDCP_BASE               0x110AC0     // HDCP started from 0xC0
#define REG_ADC_DTOPB_BASE          0x111200     // ADC DTOPB
#define REG_DVI_ATOP1_BASE          0x113200
#define REG_DVI_DTOP1_BASE          0x113300
#define REG_DVI_EQ1_BASE            0x113380     // EQ started from 0x80
#define REG_HDCP1_BASE              0x1133C0     // HDCP started from 0xC0
#define REG_DVI_ATOP2_BASE          0x113400
#define REG_DVI_DTOP2_BASE          0x113500
#define REG_DVI_EQ2_BASE            0x113580     // EQ started from 0x80
#define REG_HDCP2_BASE              0x1135C0     // HDCP started from 0xC0
#define REG_DVI_PS_BASE             0x113600     // DVI power saving
#define REG_DVI_PS1_BASE            0x113640     // DVI power saving1
#define REG_DVI_PS2_BASE            0x113680     // DVI power saving2
#define REG_DVI_PS3_BASE            0x1136C0     // DVI power saving3
#define REG_DVI_DTOP3_BASE          0x113700
#define REG_DVI_EQ3_BASE            0x113780     // EQ started from 0x80
#define REG_HDCP3_BASE              0x1137C0     // HDCP started from 0xC0
#define REG_ROTATE_BASE             0x150E00     // Rotate
#define REG_iDAC_BASE               0x153E00	 // iDAC

#define REG_CHIP_ID_MAJOR           0x1ECC
#define REG_CHIP_ID_MINOR           0x1ECD
#define REG_CHIP_VERSION            0x1ECE
#define REG_CHIP_REVISION           0x1ECF


// store bank
#define LPLL_BK_STORE

// restore bank
#define LPLL_BK_RESTORE

// switch bank
#define LPLL_BK_SWITCH(_x_)

//------------------------------------------------------------------------------
// Register configure
//------------------------------------------------------------------------------
#define REG_CHIPTOP_75			(REG_CHIPTOP_ORI_BASE +0xEA )
#define REG_CHIPTOP_76			(REG_CHIPTOP_ORI_BASE +0xEC )
#define REG_CHIPTOP_77			(REG_CHIPTOP_ORI_BASE +0xEE )
#define REG_CKG_DACA2           (REG_CHIPTOP_BASE + 0x4C ) //DAC out
    #define CKG_DACA2_GATED         BIT(0)
    #define CKG_DACA2_INVERT        BIT(1)
    #define CKG_DACA2_MASK          BMASK(3:2)
    #define CKG_DACA2_VIF_CLK       (0 << 2)
    #define CKG_DACA2_VD_CLK        (1 << 2)
    #define CKG_DACA2_EXT_TEST_CLK  (2 << 2)
    #define CKG_DACA2_XTAL          (3 << 2)

#define REG_CKG_DACB2           (REG_CHIPTOP_BASE + 0x4D ) //DAC out
    #define CKG_DACB2_GATED         BIT(0)
    #define CKG_DACB2_INVERT        BIT(1)
    #define CKG_DACB2_MASK          BMASK(3:2)
    #define CKG_DACB2_VIF_CLK       (0 << 2)
    #define CKG_DACB2_VD_CLK        (1 << 2)
    #define CKG_DACB2_EXT_TEST_CLK  (2 << 2)
    #define CKG_DACB2_XTAL          (3 << 2)

//------------------------------------------------------------------------------
//  SC2 FECLK
#define REG_CKG_SC2_FICLK_F1    (REG_CLKGEN1_BASE + 0x48)
    #define CKG_SC2_FICLK_F1_GATED      BIT(0)
    #define CKG_SC2_FICLK_F1_INVERT     BIT(1)
    #define CKG_SC2_FICLK_F1_MASK       BMASK(3:2)
    #define CKG_SC2_FICLK_F1_IDCLK1     (0 << 2)
    #define CKG_SC2_FICLK_F1_FLK        (1 << 2)
    //#define CKG_FICLK_F1_XTAL       (3 << 2)


#define REG_CKG_SC2_FICLK_F2    (REG_CLKGEN1_BASE + 0x48)
    #define CKG_SC2_FICLK_F2_GATED      BIT(4)
    #define CKG_SC2_FICLK_F2_INVERT     BIT(5)
    #define CKG_SC2_FICLK_F2_MASK      BMASK(7:6)
    #define CKG_SC2_FICLK_F2_IDCLK2     (0 << 6)
    #define CKG_SC2_FICLK_F2_FLK        (1 << 6)
    //#define CKG_FICLK_F2_XTAL       (3 << 2)


//  SC1 FECLK
#define REG_CKG_SC1_FICLK_F2    (REG_CLKGEN1_BASE + 0x49)
    #define CKG_SC1_FICLK_F2_GATED      BIT(0)
    #define CKG_SC1_FICLK_F2_INVERT     BIT(1)
    #define CKG_SC1_FICLK_F2_MASK      BMASK(3:2)
    #define CKG_SC1_FICLK_F2_IDCLK1     (0 << 2)
    #define CKG_SC1_FICLK_F2_FLK        (1 << 2)
    //#define CKG_FICLK_F2_XTAL       (3 << 2)


//  SC0 FECLK
#define REG_CKG_FICLK_F1        (REG_CHIPTOP_BASE + 0xA2 ) // scaling line buffer, set to fclk if post scaling, set to idclk is pre-scaling
    #define CKG_FICLK_F1_GATED      BIT(0)
    #define CKG_FICLK_F1_INVERT     BIT(1)
    #define CKG_FICLK_F1_MASK       BMASK(3:2)
    #define CKG_FICLK_F1_IDCLK1     (0 << 2)
    #define CKG_FICLK_F1_FLK        (1 << 2)
    //#define CKG_FICLK_F1_XTAL       (3 << 2)

#define REG_CKG_FICLK_F2        (REG_CHIPTOP_BASE + 0xA3 ) // scaling line buffer, set to fclk if post scaling, set to idclk is pre-scaling
    #define CKG_FICLK_F2_GATED      BIT(0)
    #define CKG_FICLK_F2_INVERT     BIT(1)
    #define CKG_FICLK_F2_MASK      BMASK(3:2)
    #define CKG_FICLK_F2_IDCLK2     (0 << 2)
    #define CKG_FICLK_F2_FLK        (1 << 2)
    //#define CKG_FICLK_F2_XTAL       (3 << 2)

//------------------------------------------------------------------------------
// SC0/SC1/SC2 Common FCLK
#define REG_CKG_FCLK            (REG_CHIPTOP_BASE + 0xA5 ) // after memory, before fodclk
    #define CKG_FCLK_GATED          BIT(0)
    #define CKG_FCLK_INVERT         BIT(1)
    #define CKG_FCLK_MASK           BMASK(4:2)
    #define CKG_FCLK_170MHZ         (0 << 2)
    #define CKG_FCLK_CLK_MIU        (1 << 2)
    #define CKG_FCLK_CLK_ODCLK      (2 << 2)
    #define CKG_FCLK_216MHZ         (3 << 2)
    #define CKG_FCLK_192MHZ         (4 << 2)
    #define CKG_FCLK_SCPLL          (5 << 2)
    #define CKG_FCLK_0              (6 << 2)
    #define CKG_FCLK_XTAL           (7 << 2)
    #define CKG_FCLK_XTAL_          CKG_FCLK_XTAL//(8 << 2) for A5 no XTAL

// SC0/SC1/SC2 Common EDCLK
#define REG_CKG_EDCLK            (REG_CHIPTOP_BASE + 0xBE )
    #define CKG_EDCLK_GATED         BIT(0)
    #define CKG_EDCLK_INVERT        BIT(1)
    #define CKG_EDCLK_MASK          BMASK(2:2)
    #define CKG_EDCLK_172_BUF       (0 << 2)


#define REG_CKG_HDGEN_SRC_SEL       (REG_CHIPTOP_BASE + 0x67)
    #define CKG_HDGEN_SRC_SEL_MASK             BMASK(5:5)
    #define CKG_HDGEN_SRC_SEL_CLK_HDMI_OUT_DAC (0<<5)
    #define CKG_HDGEN_SRC_SEL_108MHZ           (1<<5)

#define REG_CKG_SC_HDMIPLL          (REG_CHIPTOP_BASE + 0xBF)
    #define CKG_SC_HDMIPLL_CLK_MSK          BMASK(4:2)
    #define CKG_SC_HDMIPLL_CLK_HDGEN_SRC_1  (0<<2)
    #define CKG_SC_HDMIPLL_CLK_HDGEN_SRC_2  (1<<2)
    #define CKG_SC_HDMIPLL_CLK_HDGEN_SRC_4  (2<<2)
    #define CKG_SC_HDMIPLL_CLK_HDGEN_SRC_8  (3<<2)

#define REG_CKG_VE_CLK              (REG_CHIPTOP_BASE + 0x48)
    #define CKG_VE_CLK_GATED            BIT(0)
    #define CKG_VE_CLK_INVERT           BIT(1)
    #define CKG_VE_CLK_MASK             BMASK(3:2)
    #define CKG_VE_CLK_27MHZ            (0<<2)

#define REG_CKG_VEDAC_CLK           (REG_CHIPTOP_BASE + 0x49)
    #define CKG_VEDAC_CLK_GATED         BIT(0)
    #define CKG_VEDAC_CLK_INVERT        BIT(1)
    #define CKG_VEDAC_CLK_MASK          BMASK(4:2)
    #define CKG_VEDAC_CLK_108M          (0<<2)
    #define CKG_VEDAC_CLK_54M           (1<<2)
    #define CKG_VEDAC_CLK_27M           (2<<2)
    #define CKG_VEDAC_CLK_OTHER         (3<<2)


#define REG_CKG_DAC1_CLK             (REG_CHIPTOP_BASE + 0x6E)
    #define CKG_DAC1_CLK_GATED          BIT(0)
    #define CKG_DAC1_CLK_INVERT         BIT(1)
    #define CKG_DAC1_CLK_MASK           BMASK(4:2)
    #define CKG_DAC1_CLK_RESERVED       (0<<2)
    #define CKG_DAC1_CLK_VDEAC_DIGI     (1<<2)
    #define CKG_DAC1_CLK_VD             (2<<2)
    #define CKG_DAC1_CLK_12M            (3<<2)
    #define CKG_DAC1_CLK_DAC_EXTERNAL   (4<<2)

#define REG_CKG_HDMI_TX_CLK           (REG_CHIPTOP_BASE + 0x68)
    #define CKG_HDMI_TX_CLK_GATED       BIT(0)
    #define CKG_HDMI_TX_CLK_INVERT      BIT(1)
    #define CKG_HDMI_TX_CLK_MASK        BMASK(4:2)
    #define CKG_HDMI_TX_CLK_HDGEN_SRC_1 (0<<2)
    #define CKG_HDMI_TX_CLK_HDGEN_SRC_2 (1<<2)
    #define CKG_HDMI_TX_CLK_HDGEN_SRC_4 (2<<2)
    #define CKG_HDMI_TX_CLK_HDGEN_SRC_8 (3<<2)

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// SC2 ODCLK
#define REG_CKG_SC2_ODCLK           ( REG_CLKGEN1_BASE + 0x4A )
    #define CKG_SC2_ODCLK_GATE              BIT(0)
    #define CKG_SC2_ODCLK_INVERT            BIT(1)

#define REG_CKG_SC2_ODCLK_SYN       ( REG_CHIPTOP_BASE + 0x96 )
    #define CKG_SC2_ODCLK_SYN_GATED         BIT(0)
    #define CKG_SC2_ODCLK_SYN_INVERT        BIT(1)
    #define CKG_SC2_ODCLK_SYN_MASK          BMASK(3:2)
    #define CKG_SC2_ODCLK_SYN_432MHZ        (0 << 2)
    #define CKG_SC2_ODCLK_SYN_216MHZ        (1 << 2)
    #define CKG_SC2_ODCLK_SYN_CLK_XTRAIL_12 (2 << 2)
    #define CKG_SC2_ODCLK_SYN_RESERVED      (3 << 2)
    //#define CKG_ODCLK_XTAL          (8 << 2)

// REG_SC2_ODCLK_SYNTH_CTRL[31:0] = 216*(2^27)/target_clock_freq
#define REG_SC2_ODCLK_SYNTH_CTRL_0   (REG_CHIPTOP_BASE + 0x92)
#define REG_SC2_ODCLK_SYNTH_CTRL_1   (REG_CHIPTOP_BASE + 0x94)

// SC1 ODCLK
#define REG_CKG_SC1_ODCLK          (REG_CHIPTOP_BASE + 0xA6 ) // output dot clock, usually select LPLL, select XTAL when debug
    #define CKG_SC1_ODCLK_GATED         BIT(4)
    #define CKG_SC1_ODCLK_INVERT        BIT(5)
    #define CKG_SC1_ODCLK_MASK          BMASK(7:6)
    #define CKG_SC1_ODCLK_HDMI_PLL      (0 << 6)
    #define CKG_SC1_ODCLK_LPLL_DIV2     (1 << 6)
    #define CKG_SC1_ODCLK_RESERVED      (2 << 6)
    #define CKG_SC1_ODCLK_CLK_LPLL      (3 << 6)
    //#define CKG_ODCLK_XTAL          (8 << 2)

// SC0 ODCLK
#define REG_CKG_ODCLK           (REG_CHIPTOP_BASE + 0xA6 ) // output dot clock, usually select LPLL, select XTAL when debug
    #define CKG_ODCLK_GATED         BIT(0)
    #define CKG_ODCLK_INVERT        BIT(1)
    #define CKG_ODCLK_MASK          BMASK(3:2)
    #define CKG_ODCLK_SYNTH_CLK     (0 << 2)
    #define CKG_ODCLK_LPLL_DIV2     (1 << 2)
    #define CKG_ODCLK_27M           (2 << 2)
    #define CKG_ODCLK_CLK_LPLL      (3 << 2)
    //#define CKG_ODCLK_XTAL          (8 << 2)

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// SC2 IDCLK
#define REG_CKG_SC2_IDCLK0          (REG_CLKGEN1_BASE + 0x44 ) // off-line detect idclk
    #define CKG_SC2_IDCLK0_GATED        BIT(0)
    #define CKG_SC2_IDCLK0_INVERT       BIT(1)
    #define CKG_SC2_IDCLK0_MASK         BMASK(4:2)
    #define CKG_SC2_IDCLK0_PRE_MUX_OUT  (0 << 2)
    #define CKG_SC2_IDCLK0_ODCLK        (1 << 2)
    #define CKG_SC2_IDCLK0_CLK_VD       (2 << 2)
    #define CKG_SC2_IDCLK0_CLK_DC0      (3 << 2)
    #define CKG_SC2_IDCLK0_SC1_ODCLK    (4 << 2)
    #define CKG_SC2_IDCLK0_CLK_ICP      (5 << 2)
    #define CKG_SC2_IDCLK0_SC2_ODCLK    (6 << 2)
    #define CKG_SC2_IDCLK0_RESERVED  (7 << 2)               // same as 5 --> also is 0
    #define CKG_SC2_IDCLK0_XTAL         CKG_SC2_IDCLK0_ODCLK//(8 << 2) for A5 no XTAL, select as OD

#define REG_CKG_SC2_PRE_IDCLK0      (REG_CLKGEN1_BASE + 0x42)
    #define CKG_SC2_PRE_IDCLK0_MASK     BMASK(1:0)
    #define CKG_SC2_PRE_IDCLK0_ADC      (0x00 << 0)
    #define CKG_SC2_PRE_IDCLK0_EXTDI    (0x01 << 0)
    #define CKG_SC2_PRE_IDCLK0_EXTDI2   (0x02 << 0)
    #define CKG_SC2_PRE_IDCLK0_VD_ADC   (0x03 << 0)

#define REG_CKG_SC2_IDCLK1          (REG_CLKGEN1_BASE + 0x45 ) // sub main window idclk
    #define CKG_SC2_IDCLK1_GATED        BIT(0)
    #define CKG_SC2_IDCLK1_INVERT       BIT(1)
    #define CKG_SC2_IDCLK1_MASK         BMASK(4:2)
    #define CKG_SC2_IDCLK1_PRE_MUX_OUT  (0 << 2)
    #define CKG_SC2_IDCLK1_ODCLK        (1 << 2)
    #define CKG_SC2_IDCLK1_CLK_VD       (2 << 2)
    #define CKG_SC2_IDCLK1_CLK_DC0      (3 << 2)
    #define CKG_SC2_IDCLK1_SC1_ODCLK    (4 << 2)
    #define CKG_SC2_IDCLK1_CLK_ICP      (5 << 2)
    #define CKG_SC2_IDCLK1_SC2_ODCLK    (6 << 2)
    #define CKG_SC2_IDCLK1_RESERVED     (7 << 2)               // same as 5 --> also is 0
    #define CKG_SC2_IDCLK1_XTAL         CKG_SC2_IDCLK1_ODCLK//(8 << 2) for A5 no XTAL,select as OD

#define REG_CKG_SC2_PRE_IDCLK1      (REG_CLKGEN1_BASE + 0x42)
    #define CKG_SC2_PRE_IDCLK1_MASK     BMASK(5:4)
    #define CKG_SC2_PRE_IDCLK1_ADC      (0x00 << 4)
    #define CKG_SC2_PRE_IDCLK1_EXTDI    (0x01 << 4)
    #define CKG_SC2_PRE_IDCLK1_EXTDI2   (0x02 << 4)
    #define CKG_SC2_PRE_IDCLK1_VD_ADC   (0x03 << 4)


#define REG_CKG_SC2_IDCLK2          (REG_CLKGEN1_BASE + 0x46 ) // main window idclk
    #define CKG_SC2_IDCLK2_GATED        BIT(0)
    #define CKG_SC2_IDCLK2_INVERT       BIT(1)
    #define CKG_SC2_IDCLK2_MASK         BMASK(4:2)
    #define CKG_SC2_IDCLK2_PRE_MUX_OUT  (0 << 2)
    #define CKG_SC2_IDCLK2_ODCLK        (1 << 2)
    #define CKG_SC2_IDCLK2_CLK_VD       (2 << 2)
    #define CKG_SC2_IDCLK2_CLK_DC0      (3 << 2)
    #define CKG_SC2_IDCLK2_SC1_ODCLK    (4 << 2)
    #define CKG_SC2_IDCLK2_CLK_ICP      (5 << 2)
    #define CKG_SC2_IDCLK2_SC2_ODCLK    (6 << 2)
    #define CKG_SC2_IDCLK2_RESERVED     (7 << 2)               // same as 5 --> also is 0
    #define CKG_SC2_IDCLK2_XTAL         CKG_SC2_IDCLK2_ODCLK//(8 << 2)no XTAL select as OD

#define REG_CKG_SC2_PRE_IDCLK2      (REG_CLKGEN1_BASE + 0x43)
    #define CKG_SC2_PRE_IDCLK2_MASK     BMASK(1:0)
    #define CKG_SC2_PRE_IDCLK2_ADC      (0x00 << 0)
    #define CKG_SC2_PRE_IDCLK2_EXTDI     (0x01 << 0)
    #define CKG_SC2_PRE_IDCLK2_EXTDI2   (0x02 << 0)
    #define CKG_SC2_PRE_IDCLK2_VD_ADC   (0x03 << 0)

// SC1 IDCLK
#define REG_CKG_SC1_IDCLK0          (REG_CLKGEN1_BASE + 0x4C ) // off-line detect idclk
    #define CKG_SC1_IDCLK0_GATED        BIT(0)
    #define CKG_SC1_IDCLK0_INVERT       BIT(1)
    #define CKG_SC1_IDCLK0_MASK         BMASK(4:2)
    #define CKG_SC1_IDCLK0_PRE_MUX_OUT  (0 << 2)
    #define CKG_SC1_IDCLK0_ODCLK        (1 << 2)
    #define CKG_SC1_IDCLK0_CLK_VD       (2 << 2)
    #define CKG_SC1_IDCLK0_CLK_DC0      (3 << 2)
    #define CKG_SC1_IDCLK0_SC1_ODCLK    (4 << 2)
    #define CKG_SC1_IDCLK0_CLK_ICP      (5 << 2)
    #define CKG_SC1_IDCLK0_SC2_ODCLK    (6 << 2)
    #define CKG_SC1_IDCLK0_RESERVED     (7 << 2)               // same as 5 --> also is 0
    #define CKG_SC1_IDCLK0_XTAL         CKG_SC1_IDCLK0_ODCLK//(8 << 2) for A5 no XTAL, select as OD

#define REG_CKG_SC1_PRE_IDCLK0      (REG_CLKGEN1_BASE + 0x4A)
    #define CKG_SC1_PRE_IDCLK0_MASK     BMASK(4:3)
    #define CKG_SC1_PRE_IDCLK0_ADC      (0x00 << 4)
    #define CKG_SC1_PRE_IDCLK0_EXTDI    (0x01 << 4)
    #define CKG_SC1_PRE_IDCLK0_EXTDI2   (0x02 << 4)
    #define CKG_SC1_PRE_IDCLK0_VD_ADC   (0x03 << 4)

#define REG_CKG_SC1_IDCLK2          (REG_CLKGEN1_BASE + 0x4D ) // sub main window idclk
    #define CKG_SC1_IDCLK2_GATED        BIT(0)
    #define CKG_SC1_IDCLK2_INVERT       BIT(1)
    #define CKG_SC1_IDCLK2_MASK         BMASK(4:2)
    #define CKG_SC1_IDCLK2_PRE_MUX_OUT  (0 << 2)
    #define CKG_SC1_IDCLK2_ODCLK        (1 << 2)
    #define CKG_SC1_IDCLK2_CLK_VD       (2 << 2)
    #define CKG_SC1_IDCLK2_CLK_DC0      (3 << 2)
    #define CKG_SC1_IDCLK2_SC1_ODCLK    (4 << 2)
    #define CKG_SC1_IDCLK2_CLK_ICP      (5 << 2)
    #define CKG_SC1_IDCLK2_SC2_ODCLK    (6 << 2)
    #define CKG_SC1_IDCLK2_RESERVED     (7 << 2)               // same as 5 --> also is 0
    #define CKG_SC1_IDCLK2_XTAL         CKG_SC1_IDCLK2_ODCLK//(8 << 2) for A5 no XTAL,select as OD

#define REG_CKG_SC1_PRE_IDCLK2      (REG_CLKGEN1_BASE + 0x4B)
    #define CKG_SC1_PRE_IDCLK2_MASK     BMASK(1:0)
    #define CKG_SC1_PRE_IDCLK2_ADC      (0x00 << 0)
    #define CKG_SC1_PRE_IDCLK2_EXTDI    (0x01 << 0)
    #define CKG_SC1_PRE_IDCLK2_EXTDI2   (0x02 << 0)
    #define CKG_SC1_PRE_IDCLK2_VD_ADC   (0x03 << 0)


// SC0 IDCLK
#define REG_CKG_IDCLK0          (REG_CHIPTOP_BASE + 0xA8 ) // off-line detect idclk
    #define CKG_IDCLK0_GATED        BIT(0)
    #define CKG_IDCLK0_INVERT       BIT(1)
    #define CKG_IDCLK0_MASK         BMASK(4:2)
    #define CKG_IDCLK0_PRE_MUX_OUT  (0 << 2)
    #define CKG_IDCLK0_ODCLK        (1 << 2)
    #define CKG_IDCLK0_CLK_VD       (2 << 2)
    #define CKG_IDCLK0_CLK_DC0      (3 << 2)
    #define CKG_IDCLK0_SC1_ODCLK    (4 << 2)
    #define CKG_IDCLK0_CLK_ICP      (5 << 2)
    #define CKG_IDCLK0_SC2_ODCLK    (6 << 2)
    #define CKG_IDCLK0_RESERVED     (7 << 2)               // same as 5 --> also is 0
    #define CKG_IDCLK0_XTAL         CKG_IDCLK0_ODCLK//(8 << 2) for A5 no XTAL, select as OD

#define REG_CKG_PRE_IDCLK0      (REG_CHIPTOP_BASE + 0xBC)
    #define CKG_PRE_IDCLK0_MASK     BMASK(1:0)
    #define CKG_PRE_IDCLK0_ADC      (0x00 << 0)
    #define CKG_PRE_IDCLK0_EXTDI    (0x01 << 0)
    #define CKG_PRE_IDCLK0_EXTDI2   (0x02 << 0)
    #define CKG_PRE_IDCLK0_VD_ADC   (0x03 << 0)

#define REG_CKG_IDCLK1          (REG_CHIPTOP_BASE + 0xA9 ) // sub main window idclk
    #define CKG_IDCLK1_GATED        BIT(0)
    #define CKG_IDCLK1_INVERT       BIT(1)
    #define CKG_IDCLK1_MASK         BMASK(4:2)
    #define CKG_IDCLK1_PRE_MUX_OUT  (0 << 2)
    #define CKG_IDCLK1_ODCLK        (1 << 2)
    #define CKG_IDCLK1_CLK_VD       (2 << 2)
    #define CKG_IDCLK1_CLK_DC0      (3 << 2)
    #define CKG_IDCLK1_SC1_ODCLK    (4 << 2)
    #define CKG_IDCLK1_CLK_ICP      (5 << 2)
    #define CKG_IDCLK1_SC2_ODCLK    (6 << 2)
    #define CKG_IDCLK1_RESERVED     (7 << 2)               // same as 5 --> also is 0
    #define CKG_IDCLK1_XTAL         CKG_IDCLK1_ODCLK//(8 << 2) for A5 no XTAL,select as OD

#define REG_CKG_PRE_IDCLK1      (REG_CHIPTOP_BASE + 0xBC)
    #define CKG_PRE_IDCLK1_MASK     BMASK(4:3)
    #define CKG_PRE_IDCLK1_ADC      (0x00 << 3)
    #define CKG_PRE_IDCLK1_EXTDI    (0x01 << 3)
    #define CKG_PRE_IDCLK1_EXTDI2   (0x02 << 3)
    #define CKG_PRE_IDCLK1_VD_ADC   (0x03 << 3)


#define REG_CKG_IDCLK2          (REG_CHIPTOP_BASE + 0xAA ) // main window idclk
    #define CKG_IDCLK2_GATED        BIT(0)
    #define CKG_IDCLK2_INVERT       BIT(1)
    #define CKG_IDCLK2_MASK         BMASK(4:2)
    #define CKG_IDCLK2_PRE_MUX_OUT  (0 << 2)
    #define CKG_IDCLK2_ODCLK        (1 << 2)
    #define CKG_IDCLK2_CLK_VD       (2 << 2)
    #define CKG_IDCLK2_CLK_DC0      (3 << 2)
    #define CKG_IDCLK2_SC1_ODCLK    (4 << 2)
    #define CKG_IDCLK2_CLK_ICP      (5 << 2)
    #define CKG_IDCLK2_SC2_ODCLK    (6 << 2)
    #define CKG_IDCLK2_RESERVED     (7 << 2)               // same as 5 --> also is 0
    #define CKG_IDCLK2_XTAL         CKG_IDCLK2_ODCLK//(8 << 2)no XTAL select as OD

#define REG_CKG_PRE_IDCLK2      (REG_CHIPTOP_BASE + 0xBC)
    #define CKG_PRE_IDCLK2_MASK     BMASK(7:6)
    #define CKG_PRE_IDCLK2_ADC      (0x00 << 6)
    #define CKG_PRE_IDCLK2_EXTDI    (0x01 << 6)
    #define CKG_PRE_IDCLK2_EXTDI2   (0x02 << 6)
    #define CKG_PRE_IDCLK2_VD_ADC   (0x03 << 6)


#define REG_CKG_IDCLK3          (REG_CHIPTOP_BASE + 0xAB )
    #define CKG_IDCLK3_GATED        BIT(0)
    #define CKG_IDCLK3_INVERT       BIT(1)
    #define CKG_IDCLK3_MASK         BMASK(5:2)
    #define CKG_IDCLK3_PRE_MUX_OUT  (0 << 2)
    #define CKG_IDCLK3_ODCLK        (1 << 2)
    #define CKG_IDCLK3_CLK_VD       (2 << 2)
    #define CKG_IDCLK3_CLK_DC0      (3 << 2)
    #define CKG_IDCLK3_SC1_ODCLK    (4 << 2)
    #define CKG_IDCLK3_CLK_ICP      (5 << 2)
    #define CKG_IDCLK3_SC2_ODCLK    (6 << 2)
    #define CKG_IDCLK3_RESERVED     (7 << 2)               // same as 5 --> also is 0
    #define CKG_IDCLK3_XTAL         (8 << 2)

#define REG_DE_ONLY_F2          (REG_CHIPTOP_BASE + 0xA0 )
    #define DE_ONLY_F2_MASK         BIT(2)

#define REG_DE_ONLY_F1          (REG_CHIPTOP_BASE + 0xA0 )
    #define DE_ONLY_F1_MASK         BIT(1)

#define REG_DE_ONLY_F0          (REG_CHIPTOP_BASE + 0xA0 )
    #define DE_ONLY_F0_MASK         BIT(0)

#define REG_PM_DVI_SRC_CLK      (REG_PM_SLP_BASE +  0x96)
#define REG_PM_DDC_CLK          (REG_PM_SLP_BASE +  0x42)

#define REG_CLKGEN0_50_L        (REG_CHIPTOP_BASE + 0xA0)
#define REG_CLKGEN0_51_L        (REG_CHIPTOP_BASE + 0xA2)

#define REG_CLKGEN1_24_L        (REG_CLKGEN1_BASE + 0x48)

#define REG_DIP_DYN_CLK_GATE     (REG_CHIPTOP_ORI_BASE + 0x85)
#define DIP_DYN_CLK_GATE         (1<<4)
#define REG_DIP_DYN_CLK_GATE_MSK (1<<4)

#define CLK_SRC_IDCLK2  0
#define CLK_SRC_FCLK    1
#define CLK_SRC_XTAL    3

#define MIU0_G0_REQUEST_MASK    (REG_MIU0_BASE + 0x46)
#define MIU0_G1_REQUEST_MASK    (REG_MIU0_BASE + 0x66)
#define MIU0_G2_REQUEST_MASK    (REG_MIU0_BASE + 0x86)
#define MIU0_G3_REQUEST_MASK    (REG_MIU0_BASE + 0xA6)

#define MIU1_G0_REQUEST_MASK    (REG_MIU1_BASE + 0x46)
#define MIU1_G1_REQUEST_MASK    (REG_MIU1_BASE + 0x66)
#define MIU1_G2_REQUEST_MASK    (REG_MIU1_BASE + 0x86)
#define MIU1_G3_REQUEST_MASK    (REG_MIU1_BASE + 0xA6)

#define MIU_SC_G0REQUEST_MASK   (0x0000)
#define MIU_SC_G1REQUEST_MASK   (0x0000)
#define MIU_SC_G2REQUEST_MASK   (0x0000)
#define MIU_SC_G3REQUEST_MASK   (0x01C0)

#define VOP_DE_HSTART_MASK      (0x1FFF) //BK_10_04
#define VOP_DE_HEND_MASK        (0x1FFF) //BK_10_05
#define VOP_VSYNC_END_MASK      (0x0FFF) //BK_10_03
#define VOP_DISPLAY_HSTART_MASK (0x1FFF) //BK_10_08
#define VOP_DISPLAY_HEND_MASK   (0x1FFF) //BK_10_09

#endif /* MHAL_XC_CONFIG_H */

