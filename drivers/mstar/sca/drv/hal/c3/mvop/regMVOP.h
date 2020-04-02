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

#ifndef _REG_VOP_H_
#define _REG_VOP_H_


//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Base Address
//------------------------------------------------------------------------------
#define MVOP_REG_BASE                           0x1400  // 0x1400 - 0x14FF
#define MVOP_SUB_REG_BASE                       0x1300  // 0x1300 - 0x137F
#define MVOP_3RD_REG_BASE                       0x1380  // 0x1380 - 0x13FF
#define CHIP_REG_BASE                           0x0B00  //chiptop CLKGEN0

//------------------------------------------------------------------------------
// MVOP Reg
//------------------------------------------------------------------------------
#define VOP_FRAME_VCOUNT                        (MVOP_REG_BASE + 0x00)
#define VOP_FRAME_HCOUNT                        (MVOP_REG_BASE + 0x02)
#define VOP_VB0_STR                             (MVOP_REG_BASE + 0x04)
#define VOP_VB0_END                             (MVOP_REG_BASE + 0x06)
#define VOP_VB1_STR                             (MVOP_REG_BASE + 0x08)
#define VOP_VB1_END                             (MVOP_REG_BASE + 0x0A)
#define VOP_TF_STR                              (MVOP_REG_BASE + 0x0C)
#define VOP_BF_STR                              (MVOP_REG_BASE + 0x0E)
#define VOP_HACT_STR                            (MVOP_REG_BASE + 0x10)
#define VOP_IMG_HSTR                            (MVOP_REG_BASE + 0x12)
#define VOP_IMG_VSTR0                           (MVOP_REG_BASE + 0x14)
#define VOP_IMG_VSTR1                           (MVOP_REG_BASE + 0x16)
#define VOP_TF_VS                               (MVOP_REG_BASE + 0x18)
#define VOP_BF_VS                               (MVOP_REG_BASE + 0x1A)


///TOP field Vsync start line number to MVD
#define VOP_TF_VS_MVD                           (MVOP_REG_BASE + 0x1C) //u3 new
///Bottom field Vsync start line number to MVD
#define VOP_BF_VS_MVD                           (MVOP_REG_BASE + 0x1E) //u3 new

    #define VOP_FSYNC_EN    BIT4    //frame sync enable
#define VOP_CTRL0                               (MVOP_REG_BASE + 0x22)

    #define VOP_FLIP_UV     BIT0
    #define VOP_FLIP_YC     BIT1
    #define VOP_FLD_INV     BIT2
    #define VOP_OFLD_INV    BIT4
    #define VOP_CCIR_MD     BIT5
    #define VOP_MVD_VS_MD   BIT6   //u3 new 0: Original mode;1: Use new vsync (tf_vs_mvd, bf_vs_mvd)
    #define VOP_MVD_VS_SEL  BIT7
#define VOP_CTRL1                               (MVOP_REG_BASE + 0x23)

#define VOP_TST_IMG                             (MVOP_REG_BASE + 0x24)

#define VOP_U_PAT                               (MVOP_REG_BASE + 0x26)

    #define VOP_DMA_THD      (BIT0|BIT1|BIT2|BIT3|BIT4)
            //DMA FIFO threshold
            //= reg_dma_thd x 2 (reg_miu128b=1)
            //= reg_dma_thd x 4 (reg_miu128b=0)
    #define VOP_BURST_ST_SEL BIT7
            //Timing to calculate burst length (only valid when reg_burst_ext = all)
            //0: at mi2dc_rdy; 1: at dc2mi_rdy
#define VOP_DMA0                                (MVOP_REG_BASE + 0x28)  //t3 new

    #define VOP_BURST_EXT    (BIT0|BIT1|BIT2)
            //DMA burst length
            //0:  4 (reg_miu128b=1),   8 (reg_miu128b=0)
            //1:  8 (reg_miu128b=1),  16 (reg_miu128b=0)
            //2: 16 (reg_miu128b=1),  32 (reg_miu128b=0)
            //3: 24 (reg_miu128b=1),  48 (reg_miu128b=0)
            //4: 32 (reg_miu128b=1),  64 (reg_miu128b=0)
            //5: 48 (reg_miu128b=1),  96 (reg_miu128b=0)
            //6: 64 (reg_miu128b=1), 128 (reg_miu128b=0)
            //7: all
    #define VOP_HI_TSH       (BIT3|BIT4|BIT5|BIT6) //DMA High priority threshold
            //(assert high priority if data count less then reg_hi_tsh x 8)
    #define VOP_FORCE_HIGH   BIT7 //Force DMA High priority
#define VOP_DMA1                                (MVOP_REG_BASE + 0x29)  //t3 new

#define VOP_DC_STRIP_H                          (MVOP_REG_BASE + 0x30)

#define VOP_INT_MASK                            (MVOP_REG_BASE + 0x3E)
#define VOP_MPG_JPG_SWITCH                      (MVOP_REG_BASE + 0x40)
#define VOP_DC_STRIP                            (MVOP_REG_BASE + 0x41)
#define VOP_JPG_YSTR0_L                         (MVOP_REG_BASE + 0x42)
#define VOP_JPG_YSTR0_H                         (MVOP_REG_BASE + 0x44)
#define VOP_JPG_UVSTR0_L                        (MVOP_REG_BASE + 0x46)
#define VOP_JPG_UVSTR0_H                        (MVOP_REG_BASE + 0x48)
    #define VOP_YUV_STR_HIBITS (BIT4 | BIT3 | BIT2 |BIT1 | BIT0)        //Bits(28:24)

#define VOP_JPG_HSIZE                           (MVOP_REG_BASE + 0x4A)
#define VOP_JPG_VSIZE                           (MVOP_REG_BASE + 0x4C)

    #define VOP_LOAD_REG        BIT0 //load new value into active registers 0x20-0x26
    #define VOP_TILE_FORMAT     BIT1 //0: 8x32, 1: 16x32
    #define VOP_BUF_DUAL        BIT2
    #define VOP_FORCELOAD_REG   BIT4 //force load registers
#define VOP_REG_WR                              (MVOP_REG_BASE + 0x4E)

    #define VOP_MVD_EN          BIT0 //t8 new
    #define VOP_H264_PUREY      BIT1
    //#define VOP_RVD_EN          BIT2  //a3: removed
    #define VOP_HVD_EN          BIT3
    #define VOP_FORCE_SC_RDY    BIT4 //u3 new: force sc2mvop_rdy = 1
    #define VOP_DEBUG_SEL       (BIT5 | BIT6 | BIT7) //u3 new: MVOP debug out select
#define VOP_INPUT_SWITCH                        (MVOP_REG_BASE + 0x51)

    #define VOP_RAMAP_LUMA_VAL  0x1f
    #define VOP_RAMAP_LUMA_EN   BIT7
#define VOP_RAMAP_LUMA                          (MVOP_REG_BASE + 0x52)
//u3 new: Luma range mapping for VC1 (value = 8~16)

    #define VOP_RAMAP_CHROMA_VAL  0x1f
    #define VOP_RAMAP_CHROMA_EN   BIT7
#define VOP_RAMAP_CHROMA                        (MVOP_REG_BASE + 0x53)
//u3 new: Chroma range mapping for VC1 (value = 8~16)

// [T3 new
#define VOP_DEBUG_2A                            (MVOP_REG_BASE + 0x54) //2-byte
#define VOP_DEBUG_2B                            (MVOP_REG_BASE + 0x56)
#define VOP_DEBUG_2C                            (MVOP_REG_BASE + 0x58)
#define VOP_DEBUG_2D                            (MVOP_REG_BASE + 0x5A)
#define VOP_DEBUG_2E                            (MVOP_REG_BASE + 0x5C)

    #define VOP_UF                BIT0 //buf underflow
    #define VOP_OF                BIT1 //buf overflow
#define VOP_DEBUG_2F_L                          (MVOP_REG_BASE + 0x5E)

    #define VOP_BIST_FAIL         BIT0 //YUV fifo bist fail
    #define VOP_RIU_DEBUG_SEL     (BIT6|BIT7) //RIU debug register select
#define VOP_DEBUG_2F_H                          (MVOP_REG_BASE + 0x5F)
// ]

#define VOP_UV_SHIFT                            (MVOP_REG_BASE + 0x60)

    #define VOP_GCLK_MIU_ON     BIT2 //clk_miu use 0: free-run clock; 1: gated clock
    #define VOP_GCLK_VCLK_ON    BIT3 //clk_dc0 use 0: free-run clock; 1: gated clock
#define VOP_GCLK                                (MVOP_REG_BASE + 0x60) //u3 new

#define VOP_MIU_IF                              (MVOP_REG_BASE + 0x60)
    #define VOP_MIU_128BIT      BIT4 //MIU bus use 0: 64bit 1:128bit
    #define VOP_MIU_128B_I64    BIT5
    #define VOP_MIU_REQ_DIS     BIT6

#define VOP_MIU_BUS                             (MVOP_REG_BASE + 0x60) //t3 new

#define VOP_JPG_YSTR1_L                         (MVOP_REG_BASE + 0x62)
#define VOP_JPG_YSTR1_H                         (MVOP_REG_BASE + 0x64)
#define VOP_JPG_UVSTR1_L                        (MVOP_REG_BASE + 0x66)
#define VOP_JPG_UVSTR1_H                        (MVOP_REG_BASE + 0x68)

#define VOP_SYNC_FRAME_V                        (MVOP_REG_BASE + 0x6A)
#define VOP_SYNC_FRAME_H                        (MVOP_REG_BASE + 0x6C)

//For 3D L/R mode, insert vertical blanking lines between L & R.
#define VOP_INST_BLANK_VSIZE                    (MVOP_REG_BASE + 0x6F)

#define VOP_INFO_FROM_CODEC_L                   (MVOP_REG_BASE + 0x70)
    #define VOP_INFO_FROM_CODEC_BASE_ADDR    (BIT0) //base address
    #define VOP_INFO_FROM_CODEC_PITCH        (BIT1) //pitch
    #define VOP_INFO_FROM_CODEC_SIZE         (BIT2) //size
    #define VOP_INFO_FROM_CODEC_PROG_SEQ     (BIT3) //progressive sequence
    #define VOP_INFO_FROM_CODEC_FIELD        (BIT4) //field
    #define VOP_INFO_FROM_CODEC_RANGE_MAP    (BIT5) //range map
    #define VOP_INFO_FROM_CODEC_COMP_MODE    (BIT6) //compression mode
    #define VOP_INFO_FROM_CODEC_422_FMT      (BIT7) //422 format

#define VOP_INFO_FROM_CODEC_H                   (MVOP_REG_BASE + 0x71)
    #define VOP_INFO_FROM_CODEC_DUAL_BUFF    (BIT0) //dual buffer flag
    #define VOP_INFO_FROM_CODEC_BPIC_REDUCT  (BIT1) //bpic reduction

#define VOP_MIRROR_CFG                          (MVOP_REG_BASE + 0x76)
    #define VOP_MIRROR_CFG_VEN    (BIT0) //vertical mirror enable
    #define VOP_MIRROR_CFG_HEN    (BIT1) //horizontal mirror enable
    #define VOP_HW_FLD_BASE       (BIT5) //Hardware calculate field jump base address
    #define VOP_MASK_BASE_LSB     (BIT7) //mask LSB of base address from Codec (always get top field base address)
    #define VOP_MIRROR_CFG_ENABLE (BIT3 | BIT4 | BIT5 | BIT6 | BIT7)

#define VOP_MIRROR_CFG_HI                       (MVOP_REG_BASE + 0x77)
    #define VOP_REF_SELF_FLD      (BIT1) //source field flag set by internal timing generator

#define VOP_MULTI_WIN_CFG0                      (MVOP_REG_BASE + 0x78)
    #define VOP_LR_BUF_MODE       (BIT0) //3D L/R dual buffer mode
    #define VOP_P2I_MODE          (BIT1) //progressive input, interlace output
                                         //to SC vsync is twice of to MVD vsync
    #define VOP_LR_LA_OUT         (BIT2) //3D L/R dual buffer line alternative output
    #define VOP_LR_DIFF_SIZE      (BIT7) //3D L/R dual buffer with difference size

#define VOP_RGB_FMT                             (MVOP_REG_BASE + 0x79)
    #define VOP_RGB_FMT_565       (BIT0) //RGB 565
    #define VOP_RGB_FMT_888       (BIT1) //RGB 888
    #define VOP_RGB_FMT_SEL       (BIT0 | BIT1) //RGB format selection

#define VOP_REG_STRIP_ALIGN                     (MVOP_REG_BASE + 0x7E)
#define VOP_REG_WEIGHT_CTRL                     (MVOP_REG_BASE + 0x7E)
#define VOP_REG_FRAME_RST                       (MVOP_REG_BASE + 0x7E)  //BIT15

//------------------------------------------------------------------------------
// chip top
//------------------------------------------------------------------------------
#define REG_CKG_DC0                             (CHIP_REG_BASE + 0x98)
    #define CKG_DC0_GATED           BIT0
    #define CKG_DC0_INVERT          BIT1
    #define CKG_DC0_MASK            (BIT3 | BIT2) //select clk src
        #define CKG_DC0_SYNCHRONOUS     (0 << 2)
        #define CKG_DC0_FREERUN         (1 << 2)
        #define CKG_DC0_160MHZ          (2 << 2)
        #define CKG_DC0_144MHZ          (3 << 2)

#define REG_CKG_SUB_DC0                             (CHIP_REG_BASE + 0x9A)
    #define CKG_SUB_DC0_GATED           BIT0
    #define CKG_SUB_DC0_INVERT          BIT1
    #define CKG_SUB_DC0_MASK            (BIT3 | BIT2) //select clk src
    #define CKG_SUB_DC0_SYNCHRONOUS     (0 << 2)
    #define CKG_SUB_DC0_FREERUN         (1 << 2)
    #define CKG_SUB_DC0_160MHZ          (2 << 2)
    #define CKG_SUB_DC0_144MHZ          (3 << 2)

#define REG_CKG_DC_F3                              (CHIP_REG_BASE + 0x9D)
    #define CKG_DC_F3_GATED             BIT0
    #define CKG_DC_F3_INVERT            BIT1
    #define CKG_DC_F3_MASK              (BIT3 | BIT2)
    #define CKG_DC_F3_160MHZ            (0 << 2)
    #define CKG_DC_F3_86MHZ             (1 << 2)    //86.4MHz
    #define CKG_DC_F3_54MHZ             (2 << 2)
    #define CKG_DC_F3_27MHZ             (3 << 2)

#define REG_DC0_NUM0                            (CHIP_REG_BASE + 0xE8)
#define REG_DC0_NUM1                            (CHIP_REG_BASE + 0xE9)
#define REG_DC0_DEN0                            (CHIP_REG_BASE + 0xEA)
#define REG_DC0_DEN1                            (CHIP_REG_BASE + 0xEB)

//For main mvop
#define REG_UPDATE_DC0_CW                       (CHIP_REG_BASE + 0xE0)
    #define UPDATE_DC0_FREERUN_CW       BIT0
    #define UPDATE_DC0_SYNC_CW          BIT1
#define REG_DC0_FREERUN_CW_L                    (CHIP_REG_BASE + 0xE4)
#define REG_DC0_FREERUN_CW_H                    (CHIP_REG_BASE + 0xE6)
#define REG_DC0_NUM                             (CHIP_REG_BASE + 0xE8)
#define REG_DC0_DEN                             (CHIP_REG_BASE + 0xEA)

//For sub mvop
#define REG_UPDATE_DC1_CW                       (CHIP_REG_BASE + 0xE1)
    #define UPDATE_DC1_FREERUN_CW       BIT0
    #define UPDATE_DC1_SYNC_CW          BIT1
#define REG_DC1_FREERUN_CW_L                    (CHIP_REG_BASE + 0xEC)
#define REG_DC1_FREERUN_CW_H                    (CHIP_REG_BASE + 0xEE)
#define REG_DC1_NUM                             (CHIP_REG_BASE + 0xF0)
#define REG_DC1_DEN                             (CHIP_REG_BASE + 0xF2)

#define  REG_UPDATE_STC0_CW                     (CHIP_REG_BASE + 0x08)
    #define UPDATE_STC0_CW              BIT2

#define REG_STC0SYN_CW_LL                       (CHIP_REG_BASE + 0x0A)
#define REG_STC0SYN_CW_LH                       (CHIP_REG_BASE + 0x0B)
#define REG_STC0SYN_CW_HL                       (CHIP_REG_BASE + 0x0C)
#define REG_STC0SYN_CW_HH                       (CHIP_REG_BASE + 0x0D)

#endif // _REG_VOP_H_

