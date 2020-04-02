// Generate Time: 2017-09-19 22:58:04.729357
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2016 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (MStar Confidential Information) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __INFINITY2_REG_ISP_DMAG__
#define __INFINITY2_REG_ISP_DMAG__
typedef struct {
	// h0000, bit: 0
	/* ISP RDMA enable*/
	#define offset_of_dmag_reg_isp_rdma_en (0)
	#define mask_of_dmag_reg_isp_rdma_en (0x1)
	unsigned int reg_isp_rdma_en:1;

	// h0000, bit: 1
	/* */
	unsigned int :1;

	// h0000, bit: 2
	/* ISP DMA ring buffer mode enable*/
	#define offset_of_dmag_reg_isp_dma_ring_buf_en (0)
	#define mask_of_dmag_reg_isp_dma_ring_buf_en (0x4)
	unsigned int reg_isp_dma_ring_buf_en:1;

	// h0000, bit: 3
	/* */
	unsigned int :1;

	// h0000, bit: 4
	/* ISP RDMA auto vsync trigger mode*/
	#define offset_of_dmag_reg_isp_rdma_auto (0)
	#define mask_of_dmag_reg_isp_rdma_auto (0x10)
	unsigned int reg_isp_rdma_auto:1;

	// h0000, bit: 6
	/* */
	unsigned int :2;

	// h0000, bit: 7
	/* ISP RDMA high priority set*/
	#define offset_of_dmag_reg_isp_rdma_rreq_hpri_set (0)
	#define mask_of_dmag_reg_isp_rdma_rreq_hpri_set (0x80)
	unsigned int reg_isp_rdma_rreq_hpri_set:1;

	// h0000, bit: 11
	/* ISP RDMA depack mode:
	4'd0: 8-bit
	4'd1: 10-bit
	4'd2: 16-bit
	4'd3: 12-bit
	4'd4: reserved
	4'd5: reserved
	4'd6: reserved
	4'd7: 48-bit
	4'd8: 30-bit
	4'd9: 36-bit
	others: reserved*/
	#define offset_of_dmag_reg_isp_rdma_mode (0)
	#define mask_of_dmag_reg_isp_rdma_mode (0xf00)
	unsigned int reg_isp_rdma_mode:4;

	// h0000, bit: 14
	/* */
	unsigned int :3;

	// h0000, bit: 15
	/* ISP RDMA MIU request reset*/
	#define offset_of_dmag_reg_isp_rdma_rreq_rst (0)
	#define mask_of_dmag_reg_isp_rdma_rreq_rst (0x8000)
	unsigned int reg_isp_rdma_rreq_rst:1;

	// h0000
	unsigned int /* padding 16 bit */:16;

	// h0001, bit: 12
	/* ISP RDMA pitch*/
	#define offset_of_dmag_reg_isp_rdma_pitch (2)
	#define mask_of_dmag_reg_isp_rdma_pitch (0x1fff)
	unsigned int reg_isp_rdma_pitch:13;

	// h0001, bit: 14
	/* */
	unsigned int :3;

	// h0001
	unsigned int /* padding 16 bit */:16;

	// h0002, bit: 12
	/* Ring buffer lines memory space (lines minus 1)*/
	#define offset_of_dmag_reg_isp_dma_ring_buf_sz (4)
	#define mask_of_dmag_reg_isp_dma_ring_buf_sz (0x1fff)
	unsigned int reg_isp_dma_ring_buf_sz:13;

	// h0002, bit: 14
	/* */
	unsigned int :3;

	// h0002
	unsigned int /* padding 16 bit */:16;

	// h0003, bit: 14
	/* dummy*/
	#define offset_of_dmag_reg_isp_rdma_dummy (6)
	#define mask_of_dmag_reg_isp_rdma_dummy (0xffff)
	unsigned int reg_isp_rdma_dummy:16;

	// h0003
	unsigned int /* padding 16 bit */:16;

	// h0004, bit: 14
	/* ISP RDMA width minus 1*/
	#define offset_of_dmag_reg_isp_rdma_width_m1 (8)
	#define mask_of_dmag_reg_isp_rdma_width_m1 (0xffff)
	unsigned int reg_isp_rdma_width_m1:16;

	// h0004
	unsigned int /* padding 16 bit */:16;

	// h0005, bit: 12
	/* ISP RDMA height minus 1*/
	#define offset_of_dmag_reg_isp_rdma_height_m1 (10)
	#define mask_of_dmag_reg_isp_rdma_height_m1 (0x1fff)
	unsigned int reg_isp_rdma_height_m1:13;

	// h0005, bit: 14
	/* */
	unsigned int :3;

	// h0005
	unsigned int /* padding 16 bit */:16;

	// h0006, bit: 5
	/* */
	#define offset_of_dmag_reg_isp_rdma_startx (12)
	#define mask_of_dmag_reg_isp_rdma_startx (0x3f)
	unsigned int reg_isp_rdma_startx:6;

	// h0006, bit: 14
	/* */
	unsigned int :10;

	// h0006
	unsigned int /* padding 16 bit */:16;

	// h0007, bit: 14
	/* */
	unsigned int :16;

	// h0007
	unsigned int /* padding 16 bit */:16;

	// h0008, bit: 14
	/* ISP RDMA MIU base address*/
	#define offset_of_dmag_reg_isp_rdma_base (16)
	#define mask_of_dmag_reg_isp_rdma_base (0xffff)
	unsigned int reg_isp_rdma_base:16;

	// h0008
	unsigned int /* padding 16 bit */:16;

	// h0009, bit: 10
	/* ISP RDMA MIU base address*/
	#define offset_of_dmag_reg_isp_rdma_base_1 (18)
	#define mask_of_dmag_reg_isp_rdma_base_1 (0x7ff)
	unsigned int reg_isp_rdma_base_1:11;

	// h0009, bit: 14
	/* */
	unsigned int :5;

	// h0009
	unsigned int /* padding 16 bit */:16;

	// h000a, bit: 14
	/* */
	unsigned int :16;

	// h000a
	unsigned int /* padding 16 bit */:16;

	// h000b, bit: 14
	/* */
	unsigned int :16;

	// h000b
	unsigned int /* padding 16 bit */:16;

	// h000c, bit: 2
	/* ISP RDMA MIU request threshold*/
	#define offset_of_dmag_reg_isp_rdma_rreq_thrd (24)
	#define mask_of_dmag_reg_isp_rdma_rreq_thrd (0x7)
	unsigned int reg_isp_rdma_rreq_thrd:3;

	// h000c, bit: 3
	/* */
	unsigned int :1;

	// h000c, bit: 7
	/* ISP RDMA MIU high priority threshold*/
	#define offset_of_dmag_reg_isp_rdma_rreq_hpri (24)
	#define mask_of_dmag_reg_isp_rdma_rreq_hpri (0xf0)
	unsigned int reg_isp_rdma_rreq_hpri:4;

	// h000c, bit: 11
	/* ISP RDMA MIU burst number*/
	#define offset_of_dmag_reg_isp_rdma_rreq_max (24)
	#define mask_of_dmag_reg_isp_rdma_rreq_max (0xf00)
	unsigned int reg_isp_rdma_rreq_max:4;

	// h000c, bit: 14
	/* */
	unsigned int :4;

	// h000c
	unsigned int /* padding 16 bit */:16;

	// h000d, bit: 5
	/* ISP RDMA output blanking*/
	#define offset_of_dmag_reg_isp_rdma_tg_hblk (26)
	#define mask_of_dmag_reg_isp_rdma_tg_hblk (0x3f)
	unsigned int reg_isp_rdma_tg_hblk:6;

	// h000d, bit: 14
	/* */
	unsigned int :10;

	// h000d
	unsigned int /* padding 16 bit */:16;

	// h000e, bit: 14
	/* */
	unsigned int :16;

	// h000e
	unsigned int /* padding 16 bit */:16;

	// h000f, bit: 14
	/* */
	unsigned int :16;

	// h000f
	unsigned int /* padding 16 bit */:16;

	// h0010, bit: 0
	/* ISP RDMA start trigger*/
	#define offset_of_dmag_reg_isp_rdma_trigger (32)
	#define mask_of_dmag_reg_isp_rdma_trigger (0x1)
	unsigned int reg_isp_rdma_trigger:1;

	// h0010, bit: 14
	/* */
	unsigned int :15;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 14
	/* */
	unsigned int :16;

	// h0011
	unsigned int /* padding 16 bit */:16;

	// h0012, bit: 14
	/* */
	unsigned int :16;

	// h0012
	unsigned int /* padding 16 bit */:16;

	// h0013, bit: 14
	/* */
	unsigned int :16;

	// h0013
	unsigned int /* padding 16 bit */:16;

	// h0014, bit: 14
	/* */
	unsigned int :16;

	// h0014
	unsigned int /* padding 16 bit */:16;

	// h0015, bit: 14
	/* */
	unsigned int :16;

	// h0015
	unsigned int /* padding 16 bit */:16;

	// h0016, bit: 14
	/* */
	unsigned int :16;

	// h0016
	unsigned int /* padding 16 bit */:16;

	// h0017, bit: 14
	/* */
	unsigned int :16;

	// h0017
	unsigned int /* padding 16 bit */:16;

	// h0018, bit: 0
	/* ISP RDMA valid data msb align at 16bit mode*/
	#define offset_of_dmag_reg_isp_rdma_msb_align (48)
	#define mask_of_dmag_reg_isp_rdma_msb_align (0x1)
	unsigned int reg_isp_rdma_msb_align:1;

	// h0018, bit: 2
	/* ISP RDMA valid data mode at 16bit mode
	2'd0: 8-bit
	2'd1:10-bit
	2'd2: 16-bit
	2'd3: 12-bit*/
	#define offset_of_dmag_reg_isp_rdma_valid_mode (48)
	#define mask_of_dmag_reg_isp_rdma_valid_mode (0x6)
	unsigned int reg_isp_rdma_valid_mode:2;

	// h0018, bit: 14
	/* */
	unsigned int :13;

	// h0018
	unsigned int /* padding 16 bit */:16;

	// h0019, bit: 14
	/* */
	unsigned int :16;

	// h0019
	unsigned int /* padding 16 bit */:16;

	// h001a, bit: 14
	/* */
	unsigned int :16;

	// h001a
	unsigned int /* padding 16 bit */:16;

	// h001b, bit: 14
	/* */
	unsigned int :16;

	// h001b
	unsigned int /* padding 16 bit */:16;

	// h001c, bit: 14
	/* */
	unsigned int :16;

	// h001c
	unsigned int /* padding 16 bit */:16;

	// h001d, bit: 14
	/* */
	unsigned int :16;

	// h001d
	unsigned int /* padding 16 bit */:16;

	// h001e, bit: 14
	/* */
	unsigned int :16;

	// h001e
	unsigned int /* padding 16 bit */:16;

	// h001f, bit: 0
	/* ISP RDMA end of frame status*/
	#define offset_of_dmag_reg_isp_rdma_eof_read (62)
	#define mask_of_dmag_reg_isp_rdma_eof_read (0x1)
	unsigned int reg_isp_rdma_eof_read:1;

	// h001f, bit: 14
	/* */
	unsigned int :15;

	// h001f
	unsigned int /* padding 16 bit */:16;

	// h0020, bit: 0
	/* ISP WDMA enable*/
	#define offset_of_dmag_reg_isp_wdma_en (64)
	#define mask_of_dmag_reg_isp_wdma_en (0x1)
	unsigned int reg_isp_wdma_en:1;

	// h0020, bit: 2
	/* */
	unsigned int :2;

	// h0020, bit: 3
	/* ISP WDMA MIU wbe mask*/
	#define offset_of_dmag_reg_isp_wdma_wbe_mask (64)
	#define mask_of_dmag_reg_isp_wdma_wbe_mask (0x8)
	unsigned int reg_isp_wdma_wbe_mask:1;

	// h0020, bit: 4
	/* ISP WDMA auto vsync trigger mode*/
	#define offset_of_dmag_reg_isp_wdma_auto (64)
	#define mask_of_dmag_reg_isp_wdma_auto (0x10)
	unsigned int reg_isp_wdma_auto:1;

	// h0020, bit: 6
	/* */
	unsigned int :2;

	// h0020, bit: 7
	/* ISP WDMA high priority set*/
	#define offset_of_dmag_reg_isp_wdma_wreq_hpri_set (64)
	#define mask_of_dmag_reg_isp_wdma_wreq_hpri_set (0x80)
	unsigned int reg_isp_wdma_wreq_hpri_set:1;

	// h0020, bit: 11
	/* ISP WDMA pack mode:
	4'd0: 8-bit
	4'd1: 10-bit
	4'd2: 16-bit
	4'd3: 12-bit
	4'd4: 32-bit (1T4P, 8-bit/P)
	4'd5: 40-bit (1T4P, 10-bit/P)
	4'd6: 64-bit (1T4P, 16-bit/P)
	4'd7: 48-bit (1T4P, 12-bit/P)
	4'd8: 30-bit
	4'd9: 36-bit
	others: reserved*/
	#define offset_of_dmag_reg_isp_wdma_mode (64)
	#define mask_of_dmag_reg_isp_wdma_mode (0xf00)
	unsigned int reg_isp_wdma_mode:4;

	// h0020, bit: 14
	/* */
	unsigned int :3;

	// h0020, bit: 15
	/* ISP WDMA MIU request reset*/
	#define offset_of_dmag_reg_isp_wdma_wreq_rst (64)
	#define mask_of_dmag_reg_isp_wdma_wreq_rst (0x8000)
	unsigned int reg_isp_wdma_wreq_rst:1;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 12
	/* ISP WDMA pitch*/
	#define offset_of_dmag_reg_isp_wdma_pitch (66)
	#define mask_of_dmag_reg_isp_wdma_pitch (0x1fff)
	unsigned int reg_isp_wdma_pitch:13;

	// h0021, bit: 14
	/* */
	unsigned int :3;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 12
	/* ISP WDMA assert IRQ at linex*/
	#define offset_of_dmag_reg_isp_wdma_irqlinex (68)
	#define mask_of_dmag_reg_isp_wdma_irqlinex (0x1fff)
	unsigned int reg_isp_wdma_irqlinex:13;

	// h0022, bit: 14
	/* */
	unsigned int :3;

	// h0022
	unsigned int /* padding 16 bit */:16;

	// h0023, bit: 14
	/* */
	unsigned int :16;

	// h0023
	unsigned int /* padding 16 bit */:16;

	// h0024, bit: 14
	/* ISP WDMA width minus 1*/
	#define offset_of_dmag_reg_isp_wdma_width_m1 (72)
	#define mask_of_dmag_reg_isp_wdma_width_m1 (0xffff)
	unsigned int reg_isp_wdma_width_m1:16;

	// h0024
	unsigned int /* padding 16 bit */:16;

	// h0025, bit: 12
	/* ISP WDMA height minus 1*/
	#define offset_of_dmag_reg_isp_wdma_height_m1 (74)
	#define mask_of_dmag_reg_isp_wdma_height_m1 (0x1fff)
	unsigned int reg_isp_wdma_height_m1:13;

	// h0025, bit: 14
	/* */
	unsigned int :2;

	// h0025, bit: 15
	/* ISP WDMA height limit enable*/
	#define offset_of_dmag_reg_isp_wdma_height_en (74)
	#define mask_of_dmag_reg_isp_wdma_height_en (0x8000)
	unsigned int reg_isp_wdma_height_en:1;

	// h0025
	unsigned int /* padding 16 bit */:16;

	// h0026, bit: 14
	/* ISP WDMA write limit address*/
	#define offset_of_dmag_reg_isp_wdma_w_limit_adr (76)
	#define mask_of_dmag_reg_isp_wdma_w_limit_adr (0xffff)
	unsigned int reg_isp_wdma_w_limit_adr:16;

	// h0026
	unsigned int /* padding 16 bit */:16;

	// h0027, bit: 10
	/* ISP WDMA write limit address*/
	#define offset_of_dmag_reg_isp_wdma_w_limit_adr_1 (78)
	#define mask_of_dmag_reg_isp_wdma_w_limit_adr_1 (0x7ff)
	unsigned int reg_isp_wdma_w_limit_adr_1:11;

	// h0027, bit: 14
	/* */
	unsigned int :4;

	// h0027, bit: 15
	/* ISP WDMA write limit enable*/
	#define offset_of_dmag_reg_isp_wdma_w_limit_en (78)
	#define mask_of_dmag_reg_isp_wdma_w_limit_en (0x8000)
	unsigned int reg_isp_wdma_w_limit_en:1;

	// h0027
	unsigned int /* padding 16 bit */:16;

	// h0028, bit: 14
	/* ISP WDMA MIU base address*/
	#define offset_of_dmag_reg_isp_wdma_base (80)
	#define mask_of_dmag_reg_isp_wdma_base (0xffff)
	unsigned int reg_isp_wdma_base:16;

	// h0028
	unsigned int /* padding 16 bit */:16;

	// h0029, bit: 10
	/* ISP WDMA MIU base address*/
	#define offset_of_dmag_reg_isp_wdma_base_1 (82)
	#define mask_of_dmag_reg_isp_wdma_base_1 (0x7ff)
	unsigned int reg_isp_wdma_base_1:11;

	// h0029, bit: 14
	/* */
	unsigned int :5;

	// h0029
	unsigned int /* padding 16 bit */:16;

	// h002a, bit: 14
	/* */
	unsigned int :16;

	// h002a
	unsigned int /* padding 16 bit */:16;

	// h002b, bit: 14
	/* */
	unsigned int :16;

	// h002b
	unsigned int /* padding 16 bit */:16;

	// h002c, bit: 2
	/* ISP WDMA MIU request threshold (*4+1)*/
	#define offset_of_dmag_reg_isp_wdma_wreq_thrd (88)
	#define mask_of_dmag_reg_isp_wdma_wreq_thrd (0x7)
	unsigned int reg_isp_wdma_wreq_thrd:3;

	// h002c, bit: 3
	/* */
	unsigned int :1;

	// h002c, bit: 7
	/* ISP WDMA MIU high priority threshold*/
	#define offset_of_dmag_reg_isp_wdma_wreq_hpri (88)
	#define mask_of_dmag_reg_isp_wdma_wreq_hpri (0xf0)
	unsigned int reg_isp_wdma_wreq_hpri:4;

	// h002c, bit: 11
	/* ISP WDMA MIU burst number*/
	#define offset_of_dmag_reg_isp_wdma_wreq_max (88)
	#define mask_of_dmag_reg_isp_wdma_wreq_max (0xf00)
	unsigned int reg_isp_wdma_wreq_max:4;

	// h002c, bit: 14
	/* */
	unsigned int :4;

	// h002c
	unsigned int /* padding 16 bit */:16;

	// h002d, bit: 14
	/* */
	unsigned int :16;

	// h002d
	unsigned int /* padding 16 bit */:16;

	// h002e, bit: 14
	/* */
	unsigned int :16;

	// h002e
	unsigned int /* padding 16 bit */:16;

	// h002f, bit: 14
	/* */
	unsigned int :16;

	// h002f
	unsigned int /* padding 16 bit */:16;

	// h0030, bit: 0
	/* ISP WDMA start trigger*/
	#define offset_of_dmag_reg_isp_wdma_trigger (96)
	#define mask_of_dmag_reg_isp_wdma_trigger (0x1)
	unsigned int reg_isp_wdma_trigger:1;

	// h0030, bit: 1
	/* ISP WDMA start trigger vsync aligned mode*/
	#define offset_of_dmag_reg_isp_wdma_trigger_mode (96)
	#define mask_of_dmag_reg_isp_wdma_trigger_mode (0x2)
	unsigned int reg_isp_wdma_trigger_mode:1;

	// h0030, bit: 7
	/* */
	unsigned int :6;

	// h0030, bit: 8
	/* ISP WDMA input auto-align enable*/
	#define offset_of_dmag_reg_isp_wdma_align_en (96)
	#define mask_of_dmag_reg_isp_wdma_align_en (0x100)
	unsigned int reg_isp_wdma_align_en:1;

	// h0030, bit: 14
	/* */
	unsigned int :7;

	// h0030
	unsigned int /* padding 16 bit */:16;

	// h0031, bit: 0
	/* ISP WDMA valid data msb align at 16bit mode*/
	#define offset_of_dmag_reg_isp_wdma_msb_align (98)
	#define mask_of_dmag_reg_isp_wdma_msb_align (0x1)
	unsigned int reg_isp_wdma_msb_align:1;

	// h0031, bit: 3
	/* */
	unsigned int :3;

	// h0031, bit: 7
	/* ISP WDMA lsb mode, right shift bits*/
	#define offset_of_dmag_reg_isp_wdma_lsb_shift (98)
	#define mask_of_dmag_reg_isp_wdma_lsb_shift (0xf0)
	unsigned int reg_isp_wdma_lsb_shift:4;

	// h0031, bit: 14
	/* */
	unsigned int :8;

	// h0031
	unsigned int /* padding 16 bit */:16;

	// h0032, bit: 14
	/* dummy*/
	#define offset_of_dmag_reg_isp_wdma_dummy (100)
	#define mask_of_dmag_reg_isp_wdma_dummy (0xffff)
	unsigned int reg_isp_wdma_dummy:16;

	// h0032
	unsigned int /* padding 16 bit */:16;

	// h0033, bit: 14
	/* */
	unsigned int :16;

	// h0033
	unsigned int /* padding 16 bit */:16;

	// h0034, bit: 14
	/* */
	unsigned int :16;

	// h0034
	unsigned int /* padding 16 bit */:16;

	// h0035, bit: 14
	/* */
	unsigned int :16;

	// h0035
	unsigned int /* padding 16 bit */:16;

	// h0036, bit: 0
	/* ISP WDMA end of frame status*/
	#define offset_of_dmag_reg_isp_wdma_eof_read (108)
	#define mask_of_dmag_reg_isp_wdma_eof_read (0x1)
	unsigned int reg_isp_wdma_eof_read:1;

	// h0036, bit: 7
	/* */
	unsigned int :7;

	// h0036, bit: 8
	/* ISP WDMA LB full status*/
	#define offset_of_dmag_reg_isp_wdma_lb_full_read (108)
	#define mask_of_dmag_reg_isp_wdma_lb_full_read (0x100)
	unsigned int reg_isp_wdma_lb_full_read:1;

	// h0036, bit: 14
	/* */
	unsigned int :6;

	// h0036, bit: 15
	/* ISP WDMA status clear*/
	#define offset_of_dmag_reg_isp_wdma_status_clr (108)
	#define mask_of_dmag_reg_isp_wdma_status_clr (0x8000)
	unsigned int reg_isp_wdma_status_clr:1;

	// h0036
	unsigned int /* padding 16 bit */:16;

	// h0037, bit: 0
	/* ISP DMA mclk req force active*/
	#define offset_of_dmag_reg_mreq_always_active (110)
	#define mask_of_dmag_reg_mreq_always_active (0x1)
	unsigned int reg_mreq_always_active:1;

	// h0037, bit: 1
	/* ISP DMA mclk req force off*/
	#define offset_of_dmag_reg_mreq_force_off (110)
	#define mask_of_dmag_reg_mreq_force_off (0x2)
	unsigned int reg_mreq_force_off:1;

	// h0037, bit: 2
	/* ISP DMA mclk req wdma mode*/
	#define offset_of_dmag_reg_mreq_wdma_mode (110)
	#define mask_of_dmag_reg_mreq_wdma_mode (0x4)
	unsigned int reg_mreq_wdma_mode:1;

	// h0037, bit: 14
	/* */
	unsigned int :13;

	// h0037
	unsigned int /* padding 16 bit */:16;

	// h0038, bit: 0
	/* 0 : 1 pixel mode
	1 : 4 pixel mode*/
	#define offset_of_dmag_reg_pix4_mode_0 (112)
	#define mask_of_dmag_reg_pix4_mode_0 (0x1)
	unsigned int reg_pix4_mode_0:1;

	// h0038, bit: 1
	/* 0 : non-bayer format
	1 : bayer format*/
	#define offset_of_dmag_reg_bayer_fmt_0 (112)
	#define mask_of_dmag_reg_bayer_fmt_0 (0x2)
	unsigned int reg_bayer_fmt_0:1;

	// h0038, bit: 2
	/* 1 : 420 down sample enable*/
	#define offset_of_dmag_reg_420dn_en_0 (112)
	#define mask_of_dmag_reg_420dn_en_0 (0x4)
	unsigned int reg_420dn_en_0:1;

	// h0038, bit: 3
	/* 0 : 420 down sample for y
	1 : 420 down sample for c*/
	#define offset_of_dmag_reg_420dn_yc_sel_0 (112)
	#define mask_of_dmag_reg_420dn_yc_sel_0 (0x8)
	unsigned int reg_420dn_yc_sel_0:1;

	// h0038, bit: 7
	/* 0 : 8-bits
	1 : 10-bits
	2 : 12-bits
	3 : 16-bits
	4 : 24-bits
	5 : 30-bits
	6 : 32-bits
	7 : 36-bits
	8 : 40-bits
	9 : 48-bits
	10 : 64-bits*/
	#define offset_of_dmag_reg_bits_per_rdy_0 (112)
	#define mask_of_dmag_reg_bits_per_rdy_0 (0xf0)
	unsigned int reg_bits_per_rdy_0:4;

	// h0038, bit: 10
	/* 0 : 1/1
	1 : 1/2
	2 : 1/4
	3 : 1/8
	4 : 1/16
	5 : 1/32*/
	#define offset_of_dmag_reg_dn_mode_0 (112)
	#define mask_of_dmag_reg_dn_mode_0 (0x700)
	unsigned int reg_dn_mode_0:3;

	// h0038, bit: 11
	/* */
	unsigned int :1;

	// h0038, bit: 13
	/* 0 : keep 1st Cb/Cr
	1 : keep 2nd Cb/Cr
	3 : average of 1st and 2nd Cb/Cr*/
	#define offset_of_dmag_reg_420dn_uv_mode_0 (112)
	#define mask_of_dmag_reg_420dn_uv_mode_0 (0x3000)
	unsigned int reg_420dn_uv_mode_0:2;

	// h0038, bit: 14
	/* */
	unsigned int :2;

	// h0038
	unsigned int /* padding 16 bit */:16;

	// h0039, bit: 12
	/* source image width*/
	#define offset_of_dmag_reg_src_width_0 (114)
	#define mask_of_dmag_reg_src_width_0 (0x1fff)
	unsigned int reg_src_width_0:13;

	// h0039, bit: 14
	/* */
	unsigned int :3;

	// h0039
	unsigned int /* padding 16 bit */:16;

	// h003a, bit: 12
	/* soruce image height*/
	#define offset_of_dmag_reg_src_height_0 (116)
	#define mask_of_dmag_reg_src_height_0 (0x1fff)
	unsigned int reg_src_height_0:13;

	// h003a, bit: 14
	/* */
	unsigned int :3;

	// h003a
	unsigned int /* padding 16 bit */:16;

	// h003b, bit: 1
	/* y coefficient w0*/
	#define offset_of_dmag_reg_420dn_y_w0_0 (118)
	#define mask_of_dmag_reg_420dn_y_w0_0 (0x3)
	unsigned int reg_420dn_y_w0_0:2;

	// h003b, bit: 2
	/* sign of y coefficent w0*/
	#define offset_of_dmag_reg_420dn_y_wo_s_0 (118)
	#define mask_of_dmag_reg_420dn_y_wo_s_0 (0x4)
	unsigned int reg_420dn_y_wo_s_0:1;

	// h003b, bit: 7
	/* */
	unsigned int :5;

	// h003b, bit: 12
	/* y coefficiecnt w1*/
	#define offset_of_dmag_reg_420dn_y_w1_0 (118)
	#define mask_of_dmag_reg_420dn_y_w1_0 (0x1f00)
	unsigned int reg_420dn_y_w1_0:5;

	// h003b, bit: 14
	/* */
	unsigned int :3;

	// h003b
	unsigned int /* padding 16 bit */:16;

	// h003c, bit: 4
	/* y coefficient w2*/
	#define offset_of_dmag_reg_420dn_y_w2_0 (120)
	#define mask_of_dmag_reg_420dn_y_w2_0 (0x1f)
	unsigned int reg_420dn_y_w2_0:5;

	// h003c, bit: 7
	/* */
	unsigned int :3;

	// h003c, bit: 9
	/* y coefficiecnt w3*/
	#define offset_of_dmag_reg_420dn_y_w3_0 (120)
	#define mask_of_dmag_reg_420dn_y_w3_0 (0x300)
	unsigned int reg_420dn_y_w3_0:2;

	// h003c, bit: 10
	/* sign of y coefficient w3*/
	#define offset_of_dmag_reg_420dn_y_w3_s_0 (120)
	#define mask_of_dmag_reg_420dn_y_w3_s_0 (0x400)
	unsigned int reg_420dn_y_w3_s_0:1;

	// h003c, bit: 14
	/* */
	unsigned int :5;

	// h003c
	unsigned int /* padding 16 bit */:16;

	// h003d, bit: 2
	/* y result shift*/
	#define offset_of_dmag_reg_420dn_y_sft_0 (122)
	#define mask_of_dmag_reg_420dn_y_sft_0 (0x7)
	unsigned int reg_420dn_y_sft_0:3;

	// h003d, bit: 14
	/* */
	unsigned int :13;

	// h003d
	unsigned int /* padding 16 bit */:16;

	// h003e, bit: 14
	/* */
	unsigned int :16;

	// h003e
	unsigned int /* padding 16 bit */:16;

	// h003f, bit: 14
	/* */
	unsigned int :16;

	// h003f
	unsigned int /* padding 16 bit */:16;

	// h0040, bit: 14
	/* */
	unsigned int :16;

	// h0040
	unsigned int /* padding 16 bit */:16;

	// h0041, bit: 14
	/* */
	unsigned int :16;

	// h0041
	unsigned int /* padding 16 bit */:16;

	// h0042, bit: 14
	/* */
	unsigned int :16;

	// h0042
	unsigned int /* padding 16 bit */:16;

	// h0043, bit: 14
	/* */
	unsigned int :16;

	// h0043
	unsigned int /* padding 16 bit */:16;

	// h0044, bit: 14
	/* */
	unsigned int :16;

	// h0044
	unsigned int /* padding 16 bit */:16;

	// h0045, bit: 14
	/* */
	unsigned int :16;

	// h0045
	unsigned int /* padding 16 bit */:16;

	// h0046, bit: 14
	/* */
	unsigned int :16;

	// h0046
	unsigned int /* padding 16 bit */:16;

	// h0047, bit: 14
	/* */
	unsigned int :16;

	// h0047
	unsigned int /* padding 16 bit */:16;

	// h0048, bit: 14
	/* */
	unsigned int :16;

	// h0048
	unsigned int /* padding 16 bit */:16;

	// h0049, bit: 14
	/* */
	unsigned int :16;

	// h0049
	unsigned int /* padding 16 bit */:16;

	// h004a, bit: 14
	/* */
	unsigned int :16;

	// h004a
	unsigned int /* padding 16 bit */:16;

	// h004b, bit: 14
	/* */
	unsigned int :16;

	// h004b
	unsigned int /* padding 16 bit */:16;

	// h004c, bit: 14
	/* */
	unsigned int :16;

	// h004c
	unsigned int /* padding 16 bit */:16;

	// h004d, bit: 14
	/* */
	unsigned int :16;

	// h004d
	unsigned int /* padding 16 bit */:16;

	// h004e, bit: 14
	/* */
	unsigned int :16;

	// h004e
	unsigned int /* padding 16 bit */:16;

	// h004f, bit: 14
	/* */
	unsigned int :16;

	// h004f
	unsigned int /* padding 16 bit */:16;

	// h0050, bit: 14
	/* */
	unsigned int :16;

	// h0050
	unsigned int /* padding 16 bit */:16;

	// h0051, bit: 14
	/* */
	unsigned int :16;

	// h0051
	unsigned int /* padding 16 bit */:16;

	// h0052, bit: 14
	/* */
	unsigned int :16;

	// h0052
	unsigned int /* padding 16 bit */:16;

	// h0053, bit: 14
	/* */
	unsigned int :16;

	// h0053
	unsigned int /* padding 16 bit */:16;

	// h0054, bit: 14
	/* */
	unsigned int :16;

	// h0054
	unsigned int /* padding 16 bit */:16;

	// h0055, bit: 14
	/* */
	unsigned int :16;

	// h0055
	unsigned int /* padding 16 bit */:16;

	// h0056, bit: 14
	/* */
	unsigned int :16;

	// h0056
	unsigned int /* padding 16 bit */:16;

	// h0057, bit: 14
	/* */
	unsigned int :16;

	// h0057
	unsigned int /* padding 16 bit */:16;

	// h0058, bit: 14
	/* */
	unsigned int :16;

	// h0058
	unsigned int /* padding 16 bit */:16;

	// h0059, bit: 14
	/* */
	unsigned int :16;

	// h0059
	unsigned int /* padding 16 bit */:16;

	// h005a, bit: 14
	/* */
	unsigned int :16;

	// h005a
	unsigned int /* padding 16 bit */:16;

	// h005b, bit: 14
	/* */
	unsigned int :16;

	// h005b
	unsigned int /* padding 16 bit */:16;

	// h005c, bit: 14
	/* */
	unsigned int :16;

	// h005c
	unsigned int /* padding 16 bit */:16;

	// h005d, bit: 14
	/* */
	unsigned int :16;

	// h005d
	unsigned int /* padding 16 bit */:16;

	// h005e, bit: 14
	/* */
	unsigned int :16;

	// h005e
	unsigned int /* padding 16 bit */:16;

	// h005f, bit: 14
	/* */
	unsigned int :16;

	// h005f
	unsigned int /* padding 16 bit */:16;

	// h0060, bit: 14
	/* */
	unsigned int :16;

	// h0060
	unsigned int /* padding 16 bit */:16;

	// h0061, bit: 14
	/* */
	unsigned int :16;

	// h0061
	unsigned int /* padding 16 bit */:16;

	// h0062, bit: 14
	/* */
	unsigned int :16;

	// h0062
	unsigned int /* padding 16 bit */:16;

	// h0063, bit: 14
	/* */
	unsigned int :16;

	// h0063
	unsigned int /* padding 16 bit */:16;

	// h0064, bit: 14
	/* */
	unsigned int :16;

	// h0064
	unsigned int /* padding 16 bit */:16;

	// h0065, bit: 14
	/* */
	unsigned int :16;

	// h0065
	unsigned int /* padding 16 bit */:16;

	// h0066, bit: 14
	/* */
	unsigned int :16;

	// h0066
	unsigned int /* padding 16 bit */:16;

	// h0067, bit: 14
	/* */
	unsigned int :16;

	// h0067
	unsigned int /* padding 16 bit */:16;

	// h0068, bit: 14
	/* */
	unsigned int :16;

	// h0068
	unsigned int /* padding 16 bit */:16;

	// h0069, bit: 14
	/* */
	unsigned int :16;

	// h0069
	unsigned int /* padding 16 bit */:16;

	// h006a, bit: 14
	/* */
	unsigned int :16;

	// h006a
	unsigned int /* padding 16 bit */:16;

	// h006b, bit: 14
	/* */
	unsigned int :16;

	// h006b
	unsigned int /* padding 16 bit */:16;

	// h006c, bit: 14
	/* */
	unsigned int :16;

	// h006c
	unsigned int /* padding 16 bit */:16;

	// h006d, bit: 14
	/* */
	unsigned int :16;

	// h006d
	unsigned int /* padding 16 bit */:16;

	// h006e, bit: 14
	/* */
	unsigned int :16;

	// h006e
	unsigned int /* padding 16 bit */:16;

	// h006f, bit: 14
	/* */
	unsigned int :16;

	// h006f
	unsigned int /* padding 16 bit */:16;

	// h0070, bit: 14
	/* */
	unsigned int :16;

	// h0070
	unsigned int /* padding 16 bit */:16;

	// h0071, bit: 14
	/* */
	unsigned int :16;

	// h0071
	unsigned int /* padding 16 bit */:16;

	// h0072, bit: 14
	/* */
	unsigned int :16;

	// h0072
	unsigned int /* padding 16 bit */:16;

	// h0073, bit: 14
	/* */
	unsigned int :16;

	// h0073
	unsigned int /* padding 16 bit */:16;

	// h0074, bit: 14
	/* */
	unsigned int :16;

	// h0074
	unsigned int /* padding 16 bit */:16;

	// h0075, bit: 14
	/* */
	unsigned int :16;

	// h0075
	unsigned int /* padding 16 bit */:16;

	// h0076, bit: 14
	/* */
	unsigned int :16;

	// h0076
	unsigned int /* padding 16 bit */:16;

	// h0077, bit: 14
	/* */
	unsigned int :16;

	// h0077
	unsigned int /* padding 16 bit */:16;

	// h0078, bit: 14
	/* */
	unsigned int :16;

	// h0078
	unsigned int /* padding 16 bit */:16;

	// h0079, bit: 14
	/* */
	unsigned int :16;

	// h0079
	unsigned int /* padding 16 bit */:16;

	// h007a, bit: 14
	/* */
	unsigned int :16;

	// h007a
	unsigned int /* padding 16 bit */:16;

	// h007b, bit: 14
	/* */
	unsigned int :16;

	// h007b
	unsigned int /* padding 16 bit */:16;

	// h007c, bit: 14
	/* */
	unsigned int :16;

	// h007c
	unsigned int /* padding 16 bit */:16;

	// h007d, bit: 14
	/* */
	unsigned int :16;

	// h007d
	unsigned int /* padding 16 bit */:16;

	// h007e, bit: 14
	/* */
	unsigned int :16;

	// h007e
	unsigned int /* padding 16 bit */:16;

	// h007f, bit: 14
	/* */
	unsigned int :16;

	// h007f
	unsigned int /* padding 16 bit */:16;

}  __attribute__((packed, aligned(1))) infinity2_reg_isp_dmag;
#endif
