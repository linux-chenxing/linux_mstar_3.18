#ifndef HAL_ISP_PRIVATE_H
#define HAL_ISP_PRIVATE_H

#include <isp_types.h>
#include <infinity2_reg.h>
#include <drv_ms_isp_general.h>
#include <hal_isp.h>

//////// DMA ////////
typedef struct
{
    u32 reg_isp_rdma_en             : 1; //h00
    u32 /* reserved */              : 1;
    u32 reg_isp_dma_ring_buf_en     : 1 ; //h00
    u32 /* reserved */              : 1;
    u32 reg_isp_rdma_auto           : 1 ; //h00
    u32 /* reserved */              : 2;
    u32 reg_isp_rdma_rreq_hpri_set  : 1 ; //h00
    u32 reg_isp_rdma_mode           : 2 /* ISP RDMA depack mode:
                                          2'd0: 8-bit
                                          2'd1: 10-bit
                                          2'd2: 16-bit
                                          2'd3: 12-bit
                                        */
    ;//h00
    u32 /* reserved */              : 5;
    u32 reg_isp_rdma_rreq_rst       : 1 ; //h00
    PAD16BITS;

    u32 reg_isp_rdma_pitch          : 12; //h01
    u32 /* reserved */              : 4;
    PAD16BITS;

    u32 reg_isp_dma_ring_buf_sz     : 4 /* Ring buffer of 2^(ring_buf_sz+1) lines memory space (2~1024 lines) */
    ;//h02
    u32 /* reserved */              : 12;
    PAD16BITS;

    RSVD(0x03, 0x07);

    u32 reg_isp_rdma_base_l         : 16; //h08
    PAD16BITS;
    u32 reg_isp_rdma_base_h         : 11; //h09
    u32 /* reserved */              : 5;
    PAD16BITS;

    RSVD(0x0a, 0x0b);

    u32 reg_isp_rdma_rreq_thrd      : 3 ; //h0c
    u32 /* reserved */              : 1;
    u32 reg_isp_rdma_rreq_hpri      : 4 ; //h0c
    u32 reg_isp_rdma_rreq_max       : 4 ; //h0c
    u32 /* reserved */              : 4;
    PAD16BITS;

    u32 reg_isp_rdma_tg_hblk        : 6 ; //h0d
    u32 /* reserved */              : 10;
    PAD16BITS;

    RSVD(0x0e, 0x0f);

    u32 reg_isp_rdma_trigger        : 1 ; //h10
    u32 /* reserved */              : 15;
    PAD16BITS;

    RSVD(0x11, 0x16);

    u32 reg_sw_rst_mode             : 8 ; //h17
    u32 /* reserved */              : 8;
    PAD16BITS;

    RSVD(0x18, 0x1e);

    u32 reg_isp_rdma_eof_read       : 1 ; //h1f
    u32 /* reserved */              : 15;
    PAD16BITS;
} PACK_ALIGN_TAG(1) isp_rdma_cfg;

typedef struct
{
    u32 reg_isp_wdma_en             : 1 ; //h20
    u32 /* reserved */              : 2;
    u32 reg_isp_wdma_wbe_mask       : 1 ; //h20
    u32 reg_isp_wdma_auto           : 1 ; //h20
    u32 /* reserved */              : 2;
    u32 reg_isp_wdma_wreq_hpri_set  : 1 ; //h20
    u32 reg_isp_wdma_mode           : 2 /* ISP WDMA depack mode:
                                          2'd0: 8-bit
                                          2'd1: 10-bit
                                          2'd2: 16-bit
                                          2'd3: 12-bit
                                       */
    ;//h20
    u32 /* reserved */              : 5;
    u32 reg_isp_wdma_wreq_rst       : 1 ; //h20
    PAD16BITS;


    u32 reg_isp_wdma_pitch          : 13; //h21
    u32 /* reserved */              : 3;
    PAD16BITS;

    RSVD(0x22, 0x24);

    u32 /* reserved */              : 15; //h25
    u32 reg_isp_wdma_height_en      : 1 ; //h25
    PAD16BITS;

    u32 reg_isp_wdma_w_limit_adr_l  : 16; //h26
    PAD16BITS;
    u32 reg_isp_wdma_w_limit_adr_h  : 11; //h27
    u32 /* reserved */              : 4;
    u32 reg_isp_wdma_w_limit_en     : 1 ; //h27
    PAD16BITS;

    u32 reg_isp_wdma_base_l         : 16; //h28
    PAD16BITS;
    u32 reg_isp_wdma_base_h         : 11; //h29
    u32 /* reserved */              : 5;
    PAD16BITS;

    RSVD(0x2a, 0x2b);

    u32 reg_isp_wdma_wreq_thrd      : 3 ; //h2c ISP WDMA MIU request threshold (*4+1)
    u32 /* reserved */              : 1;
    u32 reg_isp_wdma_wreq_hpri      : 4 ; //h2c
    u32 reg_isp_wdma_wreq_max       : 4 ; //h2c
    u32 /* reserved */              : 4;
    PAD16BITS;

    RSVD(0x2d, 0x2f);

    u32 reg_isp_wdma_trigger        : 1; //0x30 //ISP WDMA start trigger
    u32 reg_isp_wdma_trigger_mode   : 1; //ISP WDMA start trigger vsync aligned mode
    u32                             : 6;
    u32 reg_isp_wdma_align_en       : 1; //ISP WDMA input auto-align enable
    u32                             : 7;
    PAD16BITS;

    RSVD(0x31, 0x3e);

    u32 reg_isp_wdma_eof_read       : 1 ; //h3f
    u32 /* reserved */              : 7;
    u32 reg_isp_wdma_lb_full_read   : 1 ; //h3f
    u32 /* reserved */              : 6;
    u32 reg_isp_wdma_status_clr     : 1 ; //h3f
    PAD16BITS;
} PACK_ALIGN_TAG(1) isp_wdma_cfg;

typedef struct
{
    isp_rdma_cfg    rdma_cfg;
    isp_wdma_cfg    wdma_cfg;

    u32 reg_mreq_always_active      : 1 ; //h40
    u32 reg_mreq_force_off          : 1 ; //h40
    u32 reg_mreq_wdma_mode          : 1 ; //h40
    u32 /* reserved */              : 13;
    PAD16BITS;

} PACK_ALIGN_TAG(1) isp6_dma_cfg_t;

typedef struct
{
    u32 reg_pat_tgen_en	: 1;
    u32 reg_pat_dgen_en : 1;
    u32 reg_pat_hsgen_en : 1;
    u32 reg_pat_dgen_rst : 1;
    u32 reg_pat_sensor_array: 2;
    u32 reg_pat_de_rate : 3;
    u32 : 7;
    PAD16BITS;
} PACK_ALIGN_TAG(1) isp7_pgen_cfg_t;

///////////////////////isp i2c///////////////////////
typedef struct
{
    u32 reg_i2c_en                  : 1 ; //h78
    u32 /* reserved */              : 7;
    u32 reg_i2c_rstz                : 1 ; //h78
    u32 /* reserved */              : 7;
    PAD16BITS;

    u32 reg_sen_m2s_2nd_reg_adr     : 8 ; //h79
    u32 /* reserved */              : 8;
    PAD16BITS;

    u32 reg_sen_m2s_sw_ctrl         : 1 ; //h7a
    u32 reg_sen_m2s_mode            : 1 ; //h7a
    u32 reg_sen_m2s_cmd_bl          : 1 ; //h7a
    u32 reg_sen_m2s_reg_adr_mode    : 1 ; //h7a
    u32 /* reserved */              : 10;
    u32 reg_sda_i                   : 1 ; //h7a
    u32 reg_m2s_status              : 1 ; //h7a
    PAD16BITS;

    u32 reg_sen_m2s_cmd             : 16; //h7b
    PAD16BITS;

    u32 reg_sen_m2s_rw_d            : 16; //h7c
    PAD16BITS;

    u32 reg_sen_prescale            : 10; //h7d
    u32 /* reserved */              : 6;
    PAD16BITS;

    u32 reg_sen_sw_scl_oen          : 1 ; //h7e
    u32 reg_sen_sw_sda_oen          : 1 ; //h7e
    u32 /* reserved */              : 14;
    PAD16BITS;

    u32 reg_sen_rd                  : 16; //h7f
    PAD16BITS;
} PACK_ALIGN_TAG(1) isp7_i2c_cfg_t;

/*
    {RIUBASE_CHIPTOP,    BANK_TO_ADDR32(0x101E) },
    {RIUBASE_CLKGEN,     BANK_TO_ADDR32(0x1038) },
    {RIUBASE_PADTOP,     BANK_TO_ADDR32(0x103C) },
    {RIUBASE_CSI_MAC,     BANK_TO_ADDR32(0x1204) },
    {RIUBASE_DPHY_CSI_ANA,     BANK_TO_ADDR32(0x1202) },
    {RIUBASE_DPHY_CSI_DIG,     BANK_TO_ADDR32(0x1203) },
 */

typedef struct
{
    volatile infinity2_reg_isp0  *isp0_cfg;
    volatile infinity2_reg_isp1  *isp1_cfg;
    volatile infinity2_reg_isp2  *isp2_cfg;
    volatile infinity2_reg_isp3  *isp3_cfg;
    volatile infinity2_reg_isp3  *stats_alsc_dnr_cfg;
    volatile infinity2_reg_isp4  *isp4_cfg;
    volatile infinity2_reg_isp5  *isp5_cfg;
    volatile infinity2_reg_isp6  *isp6_cfg;
    volatile infinity2_reg_isp7  *isp7_cfg;
    volatile infinity2_reg_isp8  *isp8_cfg;
    volatile infinity2_reg_isp9  *isp9_cfg;
    volatile infinity2_reg_isp10 *isp10_cfg;
    volatile infinity2_reg_isp11 *isp11_cfg;
    volatile infinity2_reg_isp12 *isp12_cfg;
    volatile infinity2_reg_isp_miu2sram *mload_cfg;
    volatile infinity2_reg_isp_miu2sram *scl_mload_cfg;
    volatile infinity2_reg_isp_wdma  *wdma0_cfg; //PIPE0
    volatile infinity2_reg_isp_wdma  *wdma1_cfg; //PIPE1
    volatile infinity2_reg_isp_wdma  *wdma2_cfg; //PIPE2
    volatile infinity2_reg_isp_wdma  *wdma3_cfg; //GENERAL
    volatile infinity2_reg_isp_rdma  *rdma0_cfg; //PIPE0
    volatile infinity2_reg_isp_rdma  *rdma1_cfg; //PIPE1
    ISP_CROP_WIN ImgRect;
    ISP_AWB_WIN AwbWin;

    // debug
    //ISP_DBG_LEVEL               eDbgLevel;
    //int isp_dev;
} isp_handle_t, isp_hal_handle;

#endif
