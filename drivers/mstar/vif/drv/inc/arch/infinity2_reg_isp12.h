// Generate Time: 2017-09-19 22:58:04.602577
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
#ifndef __INFINITY2_REG_ISP12__
#define __INFINITY2_REG_ISP12__
typedef struct {
	// h0000, bit: 0
	/* ISP WDMA enable*/
	#define offset_of_isp12_reg_isp_wdma2_en (0)
	#define mask_of_isp12_reg_isp_wdma2_en (0x1)
	unsigned int reg_isp_wdma2_en:1;

	// h0000, bit: 1
	/* ISP WDMA enable but data not write to DRAM*/
	#define offset_of_isp12_reg_isp_wdma2_mask_out (0)
	#define mask_of_isp12_reg_isp_wdma2_mask_out (0x2)
	unsigned int reg_isp_wdma2_mask_out:1;

	// h0000, bit: 2
	/* */
	unsigned int :1;

	// h0000, bit: 3
	/* ISP WDMA MIU wbe mask*/
	#define offset_of_isp12_reg_isp_wdma2_wbe_mask (0)
	#define mask_of_isp12_reg_isp_wdma2_wbe_mask (0x8)
	unsigned int reg_isp_wdma2_wbe_mask:1;

	// h0000, bit: 4
	/* ISP WDMA auto vsync trigger mode*/
	#define offset_of_isp12_reg_isp_wdma2_auto (0)
	#define mask_of_isp12_reg_isp_wdma2_auto (0x10)
	unsigned int reg_isp_wdma2_auto:1;

	// h0000, bit: 6
	/* */
	unsigned int :2;

	// h0000, bit: 7
	/* ISP WDMA high priority set*/
	#define offset_of_isp12_reg_isp_wdma2_wreq_hpri_set (0)
	#define mask_of_isp12_reg_isp_wdma2_wreq_hpri_set (0x80)
	unsigned int reg_isp_wdma2_wreq_hpri_set:1;

	// h0000, bit: 11
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
	#define offset_of_isp12_reg_isp_wdma2_mode (0)
	#define mask_of_isp12_reg_isp_wdma2_mode (0xf00)
	unsigned int reg_isp_wdma2_mode:4;

	// h0000, bit: 14
	/* */
	unsigned int :3;

	// h0000, bit: 15
	/* ISP WDMA MIU request reset*/
	#define offset_of_isp12_reg_isp_wdma2_wreq_rst (0)
	#define mask_of_isp12_reg_isp_wdma2_wreq_rst (0x8000)
	unsigned int reg_isp_wdma2_wreq_rst:1;

	// h0000
	unsigned int /* padding 16 bit */:16;

	// h0001, bit: 12
	/* ISP WDMA pitch*/
	#define offset_of_isp12_reg_isp_wdma2_pitch (2)
	#define mask_of_isp12_reg_isp_wdma2_pitch (0x1fff)
	unsigned int reg_isp_wdma2_pitch:13;

	// h0001, bit: 14
	/* */
	unsigned int :3;

	// h0001
	unsigned int /* padding 16 bit */:16;

	// h0002, bit: 12
	/* ISP WDMA assert IRQ at linex*/
	#define offset_of_isp12_reg_isp_wdma2_irqlinex (4)
	#define mask_of_isp12_reg_isp_wdma2_irqlinex (0x1fff)
	unsigned int reg_isp_wdma2_irqlinex:13;

	// h0002, bit: 14
	/* */
	unsigned int :3;

	// h0002
	unsigned int /* padding 16 bit */:16;

	// h0003, bit: 14
	/* */
	unsigned int :16;

	// h0003
	unsigned int /* padding 16 bit */:16;

	// h0004, bit: 14
	/* ISP WDMA width minus 1*/
	#define offset_of_isp12_reg_isp_wdma2_width_m1 (8)
	#define mask_of_isp12_reg_isp_wdma2_width_m1 (0xffff)
	unsigned int reg_isp_wdma2_width_m1:16;

	// h0004
	unsigned int /* padding 16 bit */:16;

	// h0005, bit: 12
	/* ISP WDMA height minus 1*/
	#define offset_of_isp12_reg_isp_wdma2_height_m1 (10)
	#define mask_of_isp12_reg_isp_wdma2_height_m1 (0x1fff)
	unsigned int reg_isp_wdma2_height_m1:13;

	// h0005, bit: 14
	/* */
	unsigned int :2;

	// h0005, bit: 15
	/* ISP WDMA height limit enable*/
	#define offset_of_isp12_reg_isp_wdma2_height_en (10)
	#define mask_of_isp12_reg_isp_wdma2_height_en (0x8000)
	unsigned int reg_isp_wdma2_height_en:1;

	// h0005
	unsigned int /* padding 16 bit */:16;

	// h0006, bit: 14
	/* ISP WDMA write limit address*/
	#define offset_of_isp12_reg_isp_wdma2_w_limit_adr (12)
	#define mask_of_isp12_reg_isp_wdma2_w_limit_adr (0xffff)
	unsigned int reg_isp_wdma2_w_limit_adr:16;

	// h0006
	unsigned int /* padding 16 bit */:16;

	// h0007, bit: 10
	/* ISP WDMA write limit address*/
	#define offset_of_isp12_reg_isp_wdma2_w_limit_adr_1 (14)
	#define mask_of_isp12_reg_isp_wdma2_w_limit_adr_1 (0x7ff)
	unsigned int reg_isp_wdma2_w_limit_adr_1:11;

	// h0007, bit: 14
	/* */
	unsigned int :4;

	// h0007, bit: 15
	/* ISP WDMA write limit enable*/
	#define offset_of_isp12_reg_isp_wdma2_w_limit_en (14)
	#define mask_of_isp12_reg_isp_wdma2_w_limit_en (0x8000)
	unsigned int reg_isp_wdma2_w_limit_en:1;

	// h0007
	unsigned int /* padding 16 bit */:16;

	// h0008, bit: 14
	/* ISP WDMA MIU base address*/
	#define offset_of_isp12_reg_isp_wdma2_base (16)
	#define mask_of_isp12_reg_isp_wdma2_base (0xffff)
	unsigned int reg_isp_wdma2_base:16;

	// h0008
	unsigned int /* padding 16 bit */:16;

	// h0009, bit: 10
	/* ISP WDMA MIU base address*/
	#define offset_of_isp12_reg_isp_wdma2_base_1 (18)
	#define mask_of_isp12_reg_isp_wdma2_base_1 (0x7ff)
	unsigned int reg_isp_wdma2_base_1:11;

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
	/* ISP WDMA MIU request threshold (*4+1)*/
	#define offset_of_isp12_reg_isp_wdma2_wreq_thrd (24)
	#define mask_of_isp12_reg_isp_wdma2_wreq_thrd (0x7)
	unsigned int reg_isp_wdma2_wreq_thrd:3;

	// h000c, bit: 3
	/* */
	unsigned int :1;

	// h000c, bit: 7
	/* ISP WDMA MIU high priority threshold*/
	#define offset_of_isp12_reg_isp_wdma2_wreq_hpri (24)
	#define mask_of_isp12_reg_isp_wdma2_wreq_hpri (0xf0)
	unsigned int reg_isp_wdma2_wreq_hpri:4;

	// h000c, bit: 11
	/* ISP WDMA MIU burst number*/
	#define offset_of_isp12_reg_isp_wdma2_wreq_max (24)
	#define mask_of_isp12_reg_isp_wdma2_wreq_max (0xf00)
	unsigned int reg_isp_wdma2_wreq_max:4;

	// h000c, bit: 14
	/* */
	unsigned int :4;

	// h000c
	unsigned int /* padding 16 bit */:16;

	// h000d, bit: 14
	/* SW write fence ID*/
	#define offset_of_isp12_reg_isp_sw_w_fence2 (26)
	#define mask_of_isp12_reg_isp_sw_w_fence2 (0xffff)
	unsigned int reg_isp_sw_w_fence2:16;

	// h000d
	unsigned int /* padding 16 bit */:16;

	// h000e, bit: 14
	/* SW read fence ID*/
	#define offset_of_isp12_reg_isp_sw_r_fence2 (28)
	#define mask_of_isp12_reg_isp_sw_r_fence2 (0xffff)
	unsigned int reg_isp_sw_r_fence2:16;

	// h000e
	unsigned int /* padding 16 bit */:16;

	// h000f, bit: 14
	/* */
	unsigned int :16;

	// h000f
	unsigned int /* padding 16 bit */:16;

	// h0010, bit: 0
	/* ISP WDMA start trigger*/
	#define offset_of_isp12_reg_isp_wdma2_trigger (32)
	#define mask_of_isp12_reg_isp_wdma2_trigger (0x1)
	unsigned int reg_isp_wdma2_trigger:1;

	// h0010, bit: 1
	/* ISP WDMA start trigger vsync aligned mode*/
	#define offset_of_isp12_reg_isp_wdma2_trigger_mode (32)
	#define mask_of_isp12_reg_isp_wdma2_trigger_mode (0x2)
	unsigned int reg_isp_wdma2_trigger_mode:1;

	// h0010, bit: 7
	/* */
	unsigned int :6;

	// h0010, bit: 8
	/* ISP WDMA input auto-align enable*/
	#define offset_of_isp12_reg_isp_wdma2_align_en (32)
	#define mask_of_isp12_reg_isp_wdma2_align_en (0x100)
	unsigned int reg_isp_wdma2_align_en:1;

	// h0010, bit: 14
	/* */
	unsigned int :7;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 0
	/* ISP WDMA valid data msb align at 16bit mode*/
	#define offset_of_isp12_reg_isp_wdma2_msb_align (34)
	#define mask_of_isp12_reg_isp_wdma2_msb_align (0x1)
	unsigned int reg_isp_wdma2_msb_align:1;

	// h0011, bit: 3
	/* */
	unsigned int :3;

	// h0011, bit: 7
	/* ISP WDMA lsb mode, right shift bits*/
	#define offset_of_isp12_reg_isp_wdma2_lsb_shift (34)
	#define mask_of_isp12_reg_isp_wdma2_lsb_shift (0xf0)
	unsigned int reg_isp_wdma2_lsb_shift:4;

	// h0011, bit: 14
	/* */
	unsigned int :8;

	// h0011
	unsigned int /* padding 16 bit */:16;

	// h0012, bit: 14
	/* dummy*/
	#define offset_of_isp12_reg_isp_wdma2_dummy (36)
	#define mask_of_isp12_reg_isp_wdma2_dummy (0xffff)
	unsigned int reg_isp_wdma2_dummy:16;

	// h0012
	unsigned int /* padding 16 bit */:16;

	// h0013, bit: 14
	/* dummy*/
	#define offset_of_isp12_reg_isp_wdma2_dummy_1 (38)
	#define mask_of_isp12_reg_isp_wdma2_dummy_1 (0xffff)
	unsigned int reg_isp_wdma2_dummy_1:16;

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

	// h0016, bit: 0
	/* ISP WDMA end of frame status*/
	#define offset_of_isp12_reg_isp_wdma2_eof_read (44)
	#define mask_of_isp12_reg_isp_wdma2_eof_read (0x1)
	unsigned int reg_isp_wdma2_eof_read:1;

	// h0016, bit: 7
	/* */
	unsigned int :7;

	// h0016, bit: 8
	/* ISP WDMA LB full status*/
	#define offset_of_isp12_reg_isp_wdma2_lb_full_read (44)
	#define mask_of_isp12_reg_isp_wdma2_lb_full_read (0x100)
	unsigned int reg_isp_wdma2_lb_full_read:1;

	// h0016, bit: 14
	/* */
	unsigned int :6;

	// h0016, bit: 15
	/* ISP WDMA status clear*/
	#define offset_of_isp12_reg_isp_wdma2_status_clr (44)
	#define mask_of_isp12_reg_isp_wdma2_status_clr (0x8000)
	unsigned int reg_isp_wdma2_status_clr:1;

	// h0016
	unsigned int /* padding 16 bit */:16;

	// h0017, bit: 0
	/* ISP DMA mclk req force active*/
	#define offset_of_isp12_reg_mreq2_always_active (46)
	#define mask_of_isp12_reg_mreq2_always_active (0x1)
	unsigned int reg_mreq2_always_active:1;

	// h0017, bit: 1
	/* ISP DMA mclk req force off*/
	#define offset_of_isp12_reg_mreq2_force_off (46)
	#define mask_of_isp12_reg_mreq2_force_off (0x2)
	unsigned int reg_mreq2_force_off:1;

	// h0017, bit: 2
	/* ISP DMA mclk req wdma mode*/
	#define offset_of_isp12_reg_mreq2_wdma_mode (46)
	#define mask_of_isp12_reg_mreq2_wdma_mode (0x4)
	unsigned int reg_mreq2_wdma_mode:1;

	// h0017, bit: 14
	/* */
	unsigned int :13;

	// h0017
	unsigned int /* padding 16 bit */:16;

	// h0018, bit: 0
	/* 0 : 1 pixel mode
	1 : 4 pixel mode*/
	#define offset_of_isp12_reg_pix4_mode_0 (48)
	#define mask_of_isp12_reg_pix4_mode_0 (0x1)
	unsigned int reg_pix4_mode_0:1;

	// h0018, bit: 1
	/* 0 : non-bayer format
	1 : bayer format*/
	#define offset_of_isp12_reg_bayer_fmt_0 (48)
	#define mask_of_isp12_reg_bayer_fmt_0 (0x2)
	unsigned int reg_bayer_fmt_0:1;

	// h0018, bit: 2
	/* 1 : 420 down sample enable*/
	#define offset_of_isp12_reg_420dn_en_0 (48)
	#define mask_of_isp12_reg_420dn_en_0 (0x4)
	unsigned int reg_420dn_en_0:1;

	// h0018, bit: 3
	/* 0 : 420 down sample for y
	1 : 420 down sample for c*/
	#define offset_of_isp12_reg_420dn_yc_sel_0 (48)
	#define mask_of_isp12_reg_420dn_yc_sel_0 (0x8)
	unsigned int reg_420dn_yc_sel_0:1;

	// h0018, bit: 7
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
	#define offset_of_isp12_reg_bits_per_rdy_0 (48)
	#define mask_of_isp12_reg_bits_per_rdy_0 (0xf0)
	unsigned int reg_bits_per_rdy_0:4;

	// h0018, bit: 10
	/* 0 : 1/1
	1 : 1/2
	2 : 1/4
	3 : 1/8
	4 : 1/16
	5 : 1/32*/
	#define offset_of_isp12_reg_dn_mode_0 (48)
	#define mask_of_isp12_reg_dn_mode_0 (0x700)
	unsigned int reg_dn_mode_0:3;

	// h0018, bit: 11
	/* */
	unsigned int :1;

	// h0018, bit: 13
	/* 0 : keep 1st Cb/Cr
	1 : keep 2nd Cb/Cr
	3 : average of 1st and 2nd Cb/Cr*/
	#define offset_of_isp12_reg_420dn_uv_mode_0 (48)
	#define mask_of_isp12_reg_420dn_uv_mode_0 (0x3000)
	unsigned int reg_420dn_uv_mode_0:2;

	// h0018, bit: 14
	/* */
	unsigned int :2;

	// h0018
	unsigned int /* padding 16 bit */:16;

	// h0019, bit: 12
	/* source image width*/
	#define offset_of_isp12_reg_src_width_0 (50)
	#define mask_of_isp12_reg_src_width_0 (0x1fff)
	unsigned int reg_src_width_0:13;

	// h0019, bit: 14
	/* */
	unsigned int :3;

	// h0019
	unsigned int /* padding 16 bit */:16;

	// h001a, bit: 12
	/* soruce image height*/
	#define offset_of_isp12_reg_src_height_0 (52)
	#define mask_of_isp12_reg_src_height_0 (0x1fff)
	unsigned int reg_src_height_0:13;

	// h001a, bit: 14
	/* */
	unsigned int :3;

	// h001a
	unsigned int /* padding 16 bit */:16;

	// h001b, bit: 1
	/* y coefficient w0*/
	#define offset_of_isp12_reg_420dn_y_w0_0 (54)
	#define mask_of_isp12_reg_420dn_y_w0_0 (0x3)
	unsigned int reg_420dn_y_w0_0:2;

	// h001b, bit: 2
	/* sign of y coefficent w0*/
	#define offset_of_isp12_reg_420dn_y_wo_s_0 (54)
	#define mask_of_isp12_reg_420dn_y_wo_s_0 (0x4)
	unsigned int reg_420dn_y_wo_s_0:1;

	// h001b, bit: 7
	/* */
	unsigned int :5;

	// h001b, bit: 12
	/* y coefficiecnt w1*/
	#define offset_of_isp12_reg_420dn_y_w1_0 (54)
	#define mask_of_isp12_reg_420dn_y_w1_0 (0x1f00)
	unsigned int reg_420dn_y_w1_0:5;

	// h001b, bit: 14
	/* */
	unsigned int :3;

	// h001b
	unsigned int /* padding 16 bit */:16;

	// h001c, bit: 4
	/* y coefficient w2*/
	#define offset_of_isp12_reg_420dn_y_w2_0 (56)
	#define mask_of_isp12_reg_420dn_y_w2_0 (0x1f)
	unsigned int reg_420dn_y_w2_0:5;

	// h001c, bit: 7
	/* */
	unsigned int :3;

	// h001c, bit: 9
	/* y coefficiecnt w3*/
	#define offset_of_isp12_reg_420dn_y_w3_0 (56)
	#define mask_of_isp12_reg_420dn_y_w3_0 (0x300)
	unsigned int reg_420dn_y_w3_0:2;

	// h001c, bit: 10
	/* sign of y coefficient w3*/
	#define offset_of_isp12_reg_420dn_y_w3_s_0 (56)
	#define mask_of_isp12_reg_420dn_y_w3_s_0 (0x400)
	unsigned int reg_420dn_y_w3_s_0:1;

	// h001c, bit: 14
	/* */
	unsigned int :5;

	// h001c
	unsigned int /* padding 16 bit */:16;

	// h001d, bit: 2
	/* y result shift*/
	#define offset_of_isp12_reg_420dn_y_sft_0 (58)
	#define mask_of_isp12_reg_420dn_y_sft_0 (0x7)
	unsigned int reg_420dn_y_sft_0:3;

	// h001d, bit: 14
	/* */
	unsigned int :13;

	// h001d
	unsigned int /* padding 16 bit */:16;

	// h001e, bit: 14
	/* */
	unsigned int :16;

	// h001e
	unsigned int /* padding 16 bit */:16;

	// h001f, bit: 14
	/* */
	unsigned int :16;

	// h001f
	unsigned int /* padding 16 bit */:16;

	// h0020, bit: 0
	/* ISP WDMA enable*/
	#define offset_of_isp12_reg_isp_wdma3_en (64)
	#define mask_of_isp12_reg_isp_wdma3_en (0x1)
	unsigned int reg_isp_wdma3_en:1;

	// h0020, bit: 1
	/* ISP WDMA enable but data not write to DRAM*/
	#define offset_of_isp12_reg_isp_wdma3_mask_out (64)
	#define mask_of_isp12_reg_isp_wdma3_mask_out (0x2)
	unsigned int reg_isp_wdma3_mask_out:1;

	// h0020, bit: 2
	/* */
	unsigned int :1;

	// h0020, bit: 3
	/* ISP WDMA MIU wbe mask*/
	#define offset_of_isp12_reg_isp_wdma3_wbe_mask (64)
	#define mask_of_isp12_reg_isp_wdma3_wbe_mask (0x8)
	unsigned int reg_isp_wdma3_wbe_mask:1;

	// h0020, bit: 4
	/* ISP WDMA auto vsync trigger mode*/
	#define offset_of_isp12_reg_isp_wdma3_auto (64)
	#define mask_of_isp12_reg_isp_wdma3_auto (0x10)
	unsigned int reg_isp_wdma3_auto:1;

	// h0020, bit: 6
	/* */
	unsigned int :2;

	// h0020, bit: 7
	/* ISP WDMA high priority set*/
	#define offset_of_isp12_reg_isp_wdma3_wreq_hpri_set (64)
	#define mask_of_isp12_reg_isp_wdma3_wreq_hpri_set (0x80)
	unsigned int reg_isp_wdma3_wreq_hpri_set:1;

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
	#define offset_of_isp12_reg_isp_wdma3_mode (64)
	#define mask_of_isp12_reg_isp_wdma3_mode (0xf00)
	unsigned int reg_isp_wdma3_mode:4;

	// h0020, bit: 14
	/* */
	unsigned int :3;

	// h0020, bit: 15
	/* ISP WDMA MIU request reset*/
	#define offset_of_isp12_reg_isp_wdma3_wreq_rst (64)
	#define mask_of_isp12_reg_isp_wdma3_wreq_rst (0x8000)
	unsigned int reg_isp_wdma3_wreq_rst:1;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 12
	/* ISP WDMA pitch*/
	#define offset_of_isp12_reg_isp_wdma3_pitch (66)
	#define mask_of_isp12_reg_isp_wdma3_pitch (0x1fff)
	unsigned int reg_isp_wdma3_pitch:13;

	// h0021, bit: 14
	/* */
	unsigned int :3;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 12
	/* ISP WDMA assert IRQ at linex*/
	#define offset_of_isp12_reg_isp_wdma3_irqlinex (68)
	#define mask_of_isp12_reg_isp_wdma3_irqlinex (0x1fff)
	unsigned int reg_isp_wdma3_irqlinex:13;

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
	#define offset_of_isp12_reg_isp_wdma3_width_m1 (72)
	#define mask_of_isp12_reg_isp_wdma3_width_m1 (0xffff)
	unsigned int reg_isp_wdma3_width_m1:16;

	// h0024
	unsigned int /* padding 16 bit */:16;

	// h0025, bit: 12
	/* ISP WDMA height minus 1*/
	#define offset_of_isp12_reg_isp_wdma3_height_m1 (74)
	#define mask_of_isp12_reg_isp_wdma3_height_m1 (0x1fff)
	unsigned int reg_isp_wdma3_height_m1:13;

	// h0025, bit: 14
	/* */
	unsigned int :2;

	// h0025, bit: 15
	/* ISP WDMA height limit enable*/
	#define offset_of_isp12_reg_isp_wdma3_height_en (74)
	#define mask_of_isp12_reg_isp_wdma3_height_en (0x8000)
	unsigned int reg_isp_wdma3_height_en:1;

	// h0025
	unsigned int /* padding 16 bit */:16;

	// h0026, bit: 14
	/* ISP WDMA write limit address*/
	#define offset_of_isp12_reg_isp_wdma3_w_limit_adr (76)
	#define mask_of_isp12_reg_isp_wdma3_w_limit_adr (0xffff)
	unsigned int reg_isp_wdma3_w_limit_adr:16;

	// h0026
	unsigned int /* padding 16 bit */:16;

	// h0027, bit: 10
	/* ISP WDMA write limit address*/
	#define offset_of_isp12_reg_isp_wdma3_w_limit_adr_1 (78)
	#define mask_of_isp12_reg_isp_wdma3_w_limit_adr_1 (0x7ff)
	unsigned int reg_isp_wdma3_w_limit_adr_1:11;

	// h0027, bit: 14
	/* */
	unsigned int :4;

	// h0027, bit: 15
	/* ISP WDMA write limit enable*/
	#define offset_of_isp12_reg_isp_wdma3_w_limit_en (78)
	#define mask_of_isp12_reg_isp_wdma3_w_limit_en (0x8000)
	unsigned int reg_isp_wdma3_w_limit_en:1;

	// h0027
	unsigned int /* padding 16 bit */:16;

	// h0028, bit: 14
	/* ISP WDMA MIU base address*/
	#define offset_of_isp12_reg_isp_wdma3_base (80)
	#define mask_of_isp12_reg_isp_wdma3_base (0xffff)
	unsigned int reg_isp_wdma3_base:16;

	// h0028
	unsigned int /* padding 16 bit */:16;

	// h0029, bit: 10
	/* ISP WDMA MIU base address*/
	#define offset_of_isp12_reg_isp_wdma3_base_1 (82)
	#define mask_of_isp12_reg_isp_wdma3_base_1 (0x7ff)
	unsigned int reg_isp_wdma3_base_1:11;

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
	#define offset_of_isp12_reg_isp_wdma3_wreq_thrd (88)
	#define mask_of_isp12_reg_isp_wdma3_wreq_thrd (0x7)
	unsigned int reg_isp_wdma3_wreq_thrd:3;

	// h002c, bit: 3
	/* */
	unsigned int :1;

	// h002c, bit: 7
	/* ISP WDMA MIU high priority threshold*/
	#define offset_of_isp12_reg_isp_wdma3_wreq_hpri (88)
	#define mask_of_isp12_reg_isp_wdma3_wreq_hpri (0xf0)
	unsigned int reg_isp_wdma3_wreq_hpri:4;

	// h002c, bit: 11
	/* ISP WDMA MIU burst number*/
	#define offset_of_isp12_reg_isp_wdma3_wreq_max (88)
	#define mask_of_isp12_reg_isp_wdma3_wreq_max (0xf00)
	unsigned int reg_isp_wdma3_wreq_max:4;

	// h002c, bit: 14
	/* */
	unsigned int :4;

	// h002c
	unsigned int /* padding 16 bit */:16;

	// h002d, bit: 14
	/* SW write fence ID*/
	#define offset_of_isp12_reg_isp_sw_w_fence3 (90)
	#define mask_of_isp12_reg_isp_sw_w_fence3 (0xffff)
	unsigned int reg_isp_sw_w_fence3:16;

	// h002d
	unsigned int /* padding 16 bit */:16;

	// h002e, bit: 14
	/* SW read fence ID*/
	#define offset_of_isp12_reg_isp_sw_r_fence3 (92)
	#define mask_of_isp12_reg_isp_sw_r_fence3 (0xffff)
	unsigned int reg_isp_sw_r_fence3:16;

	// h002e
	unsigned int /* padding 16 bit */:16;

	// h002f, bit: 14
	/* */
	unsigned int :16;

	// h002f
	unsigned int /* padding 16 bit */:16;

	// h0030, bit: 0
	/* ISP WDMA start trigger*/
	#define offset_of_isp12_reg_isp_wdma3_trigger (96)
	#define mask_of_isp12_reg_isp_wdma3_trigger (0x1)
	unsigned int reg_isp_wdma3_trigger:1;

	// h0030, bit: 1
	/* ISP WDMA start trigger vsync aligned mode*/
	#define offset_of_isp12_reg_isp_wdma3_trigger_mode (96)
	#define mask_of_isp12_reg_isp_wdma3_trigger_mode (0x2)
	unsigned int reg_isp_wdma3_trigger_mode:1;

	// h0030, bit: 7
	/* */
	unsigned int :6;

	// h0030, bit: 8
	/* ISP WDMA input auto-align enable*/
	#define offset_of_isp12_reg_isp_wdma3_align_en (96)
	#define mask_of_isp12_reg_isp_wdma3_align_en (0x100)
	unsigned int reg_isp_wdma3_align_en:1;

	// h0030, bit: 14
	/* */
	unsigned int :7;

	// h0030
	unsigned int /* padding 16 bit */:16;

	// h0031, bit: 0
	/* ISP WDMA valid data msb align at 16bit mode*/
	#define offset_of_isp12_reg_isp_wdma3_msb_align (98)
	#define mask_of_isp12_reg_isp_wdma3_msb_align (0x1)
	unsigned int reg_isp_wdma3_msb_align:1;

	// h0031, bit: 3
	/* */
	unsigned int :3;

	// h0031, bit: 7
	/* ISP WDMA lsb mode, right shift bits*/
	#define offset_of_isp12_reg_isp_wdma3_lsb_shift (98)
	#define mask_of_isp12_reg_isp_wdma3_lsb_shift (0xf0)
	unsigned int reg_isp_wdma3_lsb_shift:4;

	// h0031, bit: 14
	/* */
	unsigned int :8;

	// h0031
	unsigned int /* padding 16 bit */:16;

	// h0032, bit: 14
	/* dummy*/
	#define offset_of_isp12_reg_isp_wdma3_dummy (100)
	#define mask_of_isp12_reg_isp_wdma3_dummy (0xffff)
	unsigned int reg_isp_wdma3_dummy:16;

	// h0032
	unsigned int /* padding 16 bit */:16;

	// h0033, bit: 14
	/* dummy*/
	#define offset_of_isp12_reg_isp_wdma3_dummy_1 (102)
	#define mask_of_isp12_reg_isp_wdma3_dummy_1 (0xffff)
	unsigned int reg_isp_wdma3_dummy_1:16;

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
	#define offset_of_isp12_reg_isp_wdma3_eof_read (108)
	#define mask_of_isp12_reg_isp_wdma3_eof_read (0x1)
	unsigned int reg_isp_wdma3_eof_read:1;

	// h0036, bit: 7
	/* */
	unsigned int :7;

	// h0036, bit: 8
	/* ISP WDMA LB full status*/
	#define offset_of_isp12_reg_isp_wdma3_lb_full_read (108)
	#define mask_of_isp12_reg_isp_wdma3_lb_full_read (0x100)
	unsigned int reg_isp_wdma3_lb_full_read:1;

	// h0036, bit: 14
	/* */
	unsigned int :6;

	// h0036, bit: 15
	/* ISP WDMA status clear*/
	#define offset_of_isp12_reg_isp_wdma3_status_clr (108)
	#define mask_of_isp12_reg_isp_wdma3_status_clr (0x8000)
	unsigned int reg_isp_wdma3_status_clr:1;

	// h0036
	unsigned int /* padding 16 bit */:16;

	// h0037, bit: 0
	/* ISP DMA mclk req force active*/
	#define offset_of_isp12_reg_mreq3_always_active (110)
	#define mask_of_isp12_reg_mreq3_always_active (0x1)
	unsigned int reg_mreq3_always_active:1;

	// h0037, bit: 1
	/* ISP DMA mclk req force off*/
	#define offset_of_isp12_reg_mreq3_force_off (110)
	#define mask_of_isp12_reg_mreq3_force_off (0x2)
	unsigned int reg_mreq3_force_off:1;

	// h0037, bit: 2
	/* ISP DMA mclk req wdma mode*/
	#define offset_of_isp12_reg_mreq3_wdma_mode (110)
	#define mask_of_isp12_reg_mreq3_wdma_mode (0x4)
	unsigned int reg_mreq3_wdma_mode:1;

	// h0037, bit: 14
	/* */
	unsigned int :13;

	// h0037
	unsigned int /* padding 16 bit */:16;

	// h0038, bit: 0
	/* 0 : 1 pixel mode
	1 : 4 pixel mode*/
	#define offset_of_isp12_reg_pix4_mode_1 (112)
	#define mask_of_isp12_reg_pix4_mode_1 (0x1)
	unsigned int reg_pix4_mode_1:1;

	// h0038, bit: 1
	/* 0 : non-bayer format
	1 : bayer format*/
	#define offset_of_isp12_reg_bayer_fmt_1 (112)
	#define mask_of_isp12_reg_bayer_fmt_1 (0x2)
	unsigned int reg_bayer_fmt_1:1;

	// h0038, bit: 2
	/* 1 : 420 down sample enable*/
	#define offset_of_isp12_reg_420dn_en_1 (112)
	#define mask_of_isp12_reg_420dn_en_1 (0x4)
	unsigned int reg_420dn_en_1:1;

	// h0038, bit: 3
	/* 0 : 420 down sample for y
	1 : 420 down sample for c*/
	#define offset_of_isp12_reg_420dn_yc_sel_1 (112)
	#define mask_of_isp12_reg_420dn_yc_sel_1 (0x8)
	unsigned int reg_420dn_yc_sel_1:1;

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
	#define offset_of_isp12_reg_bits_per_rdy_1 (112)
	#define mask_of_isp12_reg_bits_per_rdy_1 (0xf0)
	unsigned int reg_bits_per_rdy_1:4;

	// h0038, bit: 10
	/* 0 : 1/1
	1 : 1/2
	2 : 1/4
	3 : 1/8
	4 : 1/16
	5 : 1/32*/
	#define offset_of_isp12_reg_dn_mode_1 (112)
	#define mask_of_isp12_reg_dn_mode_1 (0x700)
	unsigned int reg_dn_mode_1:3;

	// h0038, bit: 11
	/* */
	unsigned int :1;

	// h0038, bit: 13
	/* 0 : keep 1st Cb/Cr
	1 : keep 2nd Cb/Cr
	3 : average of 1st and 2nd Cb/Cr*/
	#define offset_of_isp12_reg_420dn_uv_mode_1 (112)
	#define mask_of_isp12_reg_420dn_uv_mode_1 (0x3000)
	unsigned int reg_420dn_uv_mode_1:2;

	// h0038, bit: 14
	/* */
	unsigned int :2;

	// h0038
	unsigned int /* padding 16 bit */:16;

	// h0039, bit: 12
	/* source image width*/
	#define offset_of_isp12_reg_src_width_1 (114)
	#define mask_of_isp12_reg_src_width_1 (0x1fff)
	unsigned int reg_src_width_1:13;

	// h0039, bit: 14
	/* */
	unsigned int :3;

	// h0039
	unsigned int /* padding 16 bit */:16;

	// h003a, bit: 12
	/* soruce image height*/
	#define offset_of_isp12_reg_src_height_1 (116)
	#define mask_of_isp12_reg_src_height_1 (0x1fff)
	unsigned int reg_src_height_1:13;

	// h003a, bit: 14
	/* */
	unsigned int :3;

	// h003a
	unsigned int /* padding 16 bit */:16;

	// h003b, bit: 1
	/* y coefficient w0*/
	#define offset_of_isp12_reg_420dn_y_w0_1 (118)
	#define mask_of_isp12_reg_420dn_y_w0_1 (0x3)
	unsigned int reg_420dn_y_w0_1:2;

	// h003b, bit: 2
	/* sign of y coefficent w0*/
	#define offset_of_isp12_reg_420dn_y_wo_s_1 (118)
	#define mask_of_isp12_reg_420dn_y_wo_s_1 (0x4)
	unsigned int reg_420dn_y_wo_s_1:1;

	// h003b, bit: 7
	/* */
	unsigned int :5;

	// h003b, bit: 12
	/* y coefficiecnt w1*/
	#define offset_of_isp12_reg_420dn_y_w1_1 (118)
	#define mask_of_isp12_reg_420dn_y_w1_1 (0x1f00)
	unsigned int reg_420dn_y_w1_1:5;

	// h003b, bit: 14
	/* */
	unsigned int :3;

	// h003b
	unsigned int /* padding 16 bit */:16;

	// h003c, bit: 4
	/* y coefficient w2*/
	#define offset_of_isp12_reg_420dn_y_w2_1 (120)
	#define mask_of_isp12_reg_420dn_y_w2_1 (0x1f)
	unsigned int reg_420dn_y_w2_1:5;

	// h003c, bit: 7
	/* */
	unsigned int :3;

	// h003c, bit: 9
	/* y coefficiecnt w3*/
	#define offset_of_isp12_reg_420dn_y_w3_1 (120)
	#define mask_of_isp12_reg_420dn_y_w3_1 (0x300)
	unsigned int reg_420dn_y_w3_1:2;

	// h003c, bit: 10
	/* sign of y coefficient w3*/
	#define offset_of_isp12_reg_420dn_y_w3_s_1 (120)
	#define mask_of_isp12_reg_420dn_y_w3_s_1 (0x400)
	unsigned int reg_420dn_y_w3_s_1:1;

	// h003c, bit: 14
	/* */
	unsigned int :5;

	// h003c
	unsigned int /* padding 16 bit */:16;

	// h003d, bit: 2
	/* y result shift*/
	#define offset_of_isp12_reg_420dn_y_sft_1 (122)
	#define mask_of_isp12_reg_420dn_y_sft_1 (0x7)
	unsigned int reg_420dn_y_sft_1:3;

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

	// h006f, bit: 0
	/* Enable ISP Engine
	# 1’b0: Disable and Reset to the Initial State
	# 1’b1: Enable
	 ( double buffer register )*/
	#define offset_of_isp12_reg_isp_en (222)
	#define mask_of_isp12_reg_isp_en (0x1)
	unsigned int reg_isp_en:1;

	// h006f, bit: 1
	/* Sensor Input Format
	# 1'b0: YUV 422 format
	# 1'b1: RGB pattern
	 ( single buffer register )*/
	#define offset_of_isp12_reg_isp_sensor_rgb_in (222)
	#define mask_of_isp12_reg_isp_sensor_rgb_in (0x2)
	unsigned int reg_isp_sensor_rgb_in:1;

	// h006f, bit: 2
	/* Sensor Input Format
	# 1'b0: separate Y/C mode
	# 1'b1: YC 16bit mode*/
	#define offset_of_isp12_reg_isp_sensor_yc16bit (222)
	#define mask_of_isp12_reg_isp_sensor_yc16bit (0x4)
	unsigned int reg_isp_sensor_yc16bit:1;

	// h006f, bit: 4
	/* RGB Raw Data Organization
	# 2'h0: [R G R G ...; G B G B ...]
	# 2'h1: [G R G R ...; B G B G ...]
	# 2'h2: [B G B G ...; G R G R ...]
	# 2'h3: [G B G B ...; R G R G ...]
	 ( double buffer register )*/
	#define offset_of_isp12_reg_sensor_array (222)
	#define mask_of_isp12_reg_sensor_array (0x18)
	unsigned int reg_sensor_array:2;

	// h006f, bit: 5
	/* isp_dp source mux
	# 1'b0: from rmux
	# 1'b1: from sensor freerun vs*/
	#define offset_of_isp12_reg_isp_if_rvs_sel (222)
	#define mask_of_isp12_reg_isp_if_rvs_sel (0x20)
	unsigned int reg_isp_if_rvs_sel:1;

	// h006f, bit: 6
	/* Sensor Input Priority as YUV input
	# 1'b0: [ C Y C Y ...]
	# 1'b1: [ Y C Y C ...]
	 ( double buffer register )*/
	#define offset_of_isp12_reg_isp_sensor_yuv_order (222)
	#define mask_of_isp12_reg_isp_sensor_yuv_order (0x40)
	unsigned int reg_isp_sensor_yuv_order:1;

	// h006f, bit: 7
	/* isp_dp source mux
	# 1'b0: from sensor_if
	# 1'b1: from RDMA*/
	#define offset_of_isp12_reg_isp_if_rmux_sel (222)
	#define mask_of_isp12_reg_isp_if_rmux_sel (0x80)
	unsigned int reg_isp_if_rmux_sel:1;

	// h006f, bit: 10
	/* WDMA source mux
	# 3'd0: from sensor_if
	# 3'd1: from isp_dp output
	# 3'd2: from isp_dp down-sample
	# 3'd3: from isp_dp output
	# 3'd4: from video stabilization
	# 3'd7: from RGB-IR I data*/
	#define offset_of_isp12_reg_isp_if_wmux_sel (222)
	#define mask_of_isp12_reg_isp_if_wmux_sel (0x700)
	unsigned int reg_isp_if_wmux_sel:3;

	// h006f, bit: 12
	/* Sensor Format
	# 2'b00: 8 bit
	# 2'b01: 10bit
	# 2'b10: 16bit
	# 2'b11: 12bit*/
	#define offset_of_isp12_reg_isp_sensor_format (222)
	#define mask_of_isp12_reg_isp_sensor_format (0x1800)
	unsigned int reg_isp_sensor_format:2;

	// h006f, bit: 13
	/* Always Load Double Buffer Register (Type 2 )
	 ( single buffer register )*/
	#define offset_of_isp12_reg_load_reg (222)
	#define mask_of_isp12_reg_load_reg (0x2000)
	unsigned int reg_load_reg:1;

	// h006f, bit: 14
	/* double buffer register set done at batch mode*/
	#define offset_of_isp12_reg_db_batch_done (222)
	#define mask_of_isp12_reg_db_batch_done (0x4000)
	unsigned int reg_db_batch_done:1;

	// h006f, bit: 15
	/* double buffer register batch mode*/
	#define offset_of_isp12_reg_db_batch_mode (222)
	#define mask_of_isp12_reg_db_batch_mode (0x8000)
	unsigned int reg_db_batch_mode:1;

	// h006f
	unsigned int /* padding 16 bit */:16;

	// h0070, bit: 0
	/* Ready only mode*/
	#define offset_of_isp12_reg_isp_if_de_mode (224)
	#define mask_of_isp12_reg_isp_if_de_mode (0x1)
	unsigned int reg_isp_if_de_mode:1;

	// h0070, bit: 1
	/* ISP FIFO handshaking mode enable*/
	#define offset_of_isp12_reg_isp_if_hs_mode_en (224)
	#define mask_of_isp12_reg_isp_if_hs_mode_en (0x2)
	unsigned int reg_isp_if_hs_mode_en:1;

	// h0070, bit: 2
	/* Raw data down sample to DMA enable
	(double buffer register)*/
	#define offset_of_isp12_reg_dspl2wdma_en (224)
	#define mask_of_isp12_reg_dspl2wdma_en (0x4)
	unsigned int reg_dspl2wdma_en:1;

	// h0070, bit: 5
	/* down-sampling mode
	0 : whole image
	1 : 1/4
	2 : 1/16
	3 : 1/64
	4 : 1/256*/
	#define offset_of_isp12_reg_dsp_mode (224)
	#define mask_of_isp12_reg_dsp_mode (0x38)
	unsigned int reg_dsp_mode:3;

	// h0070, bit: 6
	/* 0: vsync double buffer
	1: frame end double buffer*/
	#define offset_of_isp12_reg_latch_mode (224)
	#define mask_of_isp12_reg_latch_mode (0x40)
	unsigned int reg_latch_mode:1;

	// h0070, bit: 7
	/* 0: isp_dp reset by vsync
	1: isp_dp not reset by vsync*/
	#define offset_of_isp12_reg_isp_dp_rstz_mode (224)
	#define mask_of_isp12_reg_isp_dp_rstz_mode (0x80)
	unsigned int reg_isp_dp_rstz_mode:1;

	// h0070, bit: 8
	/* 0: Enable isp2icp frame end reset
	1: Disable*/
	#define offset_of_isp12_reg_fend_rstz_mode (224)
	#define mask_of_isp12_reg_fend_rstz_mode (0x100)
	unsigned int reg_fend_rstz_mode:1;

	// h0070, bit: 9
	/* isp to dma data pack at msb*/
	#define offset_of_isp12_reg_isp_dma_pk_msb (224)
	#define mask_of_isp12_reg_isp_dma_pk_msb (0x200)
	unsigned int reg_isp_dma_pk_msb:1;

	// h0070, bit: 14
	/* */
	unsigned int :6;

	// h0070
	unsigned int /* padding 16 bit */:16;

	// h0071, bit: 3
	/* [2] b3_atv_fix*/
	#define offset_of_isp12_reg_fire_mode (226)
	#define mask_of_isp12_reg_fire_mode (0xf)
	unsigned int reg_fire_mode:4;

	// h0071, bit: 14
	/* */
	unsigned int :12;

	// h0071
	unsigned int /* padding 16 bit */:16;

	// h0072, bit: 11
	/* X Start Position for Crop Window
	If Sensor is YUV input, this setting must be even
	 ( double buffer register )*/
	#define offset_of_isp12_reg_isp_crop_start_x (228)
	#define mask_of_isp12_reg_isp_crop_start_x (0xfff)
	unsigned int reg_isp_crop_start_x:12;

	// h0072, bit: 14
	/* */
	unsigned int :4;

	// h0072
	unsigned int /* padding 16 bit */:16;

	// h0073, bit: 11
	/* Y Start Position for Crop Window
	 ( double buffer register )*/
	#define offset_of_isp12_reg_isp_crop_start_y (230)
	#define mask_of_isp12_reg_isp_crop_start_y (0xfff)
	unsigned int reg_isp_crop_start_y:12;

	// h0073, bit: 14
	/* */
	unsigned int :4;

	// h0073
	unsigned int /* padding 16 bit */:16;

	// h0074, bit: 11
	/* Crop Window Width
	If Sensor is YUV input, this setting must be odd (0 base)
	 ( double buffer register )*/
	#define offset_of_isp12_reg_isp_crop_width (232)
	#define mask_of_isp12_reg_isp_crop_width (0xfff)
	unsigned int reg_isp_crop_width:12;

	// h0074, bit: 14
	/* */
	unsigned int :4;

	// h0074
	unsigned int /* padding 16 bit */:16;

	// h0075, bit: 11
	/* Crop Window High
	 ( double buffer register )*/
	#define offset_of_isp12_reg_isp_crop_high (234)
	#define mask_of_isp12_reg_isp_crop_high (0xfff)
	unsigned int reg_isp_crop_high:12;

	// h0075, bit: 14
	/* */
	unsigned int :4;

	// h0075
	unsigned int /* padding 16 bit */:16;

	// h0076, bit: 11
	/* WDMA X Start Position for Crop Window
	If Sensor is YUV input, this setting must be even
	 ( double buffer register )*/
	#define offset_of_isp12_reg_isp_wdma_crop_start_x (236)
	#define mask_of_isp12_reg_isp_wdma_crop_start_x (0xfff)
	unsigned int reg_isp_wdma_crop_start_x:12;

	// h0076, bit: 14
	/* */
	unsigned int :4;

	// h0076
	unsigned int /* padding 16 bit */:16;

	// h0077, bit: 11
	/* WDMA Y Start Position for Crop Window
	 ( double buffer register )*/
	#define offset_of_isp12_reg_isp_wdma_crop_start_y (238)
	#define mask_of_isp12_reg_isp_wdma_crop_start_y (0xfff)
	unsigned int reg_isp_wdma_crop_start_y:12;

	// h0077, bit: 14
	/* */
	unsigned int :4;

	// h0077
	unsigned int /* padding 16 bit */:16;

	// h0078, bit: 14
	/* WDMA Crop Window Width
	If Sensor is YUV input, this setting must be odd (0 base)
	 ( double buffer register )*/
	#define offset_of_isp12_reg_isp_wdma_crop_width (240)
	#define mask_of_isp12_reg_isp_wdma_crop_width (0xffff)
	unsigned int reg_isp_wdma_crop_width:16;

	// h0078
	unsigned int /* padding 16 bit */:16;

	// h0079, bit: 11
	/* WDMA Crop Window High
	 ( double buffer register )*/
	#define offset_of_isp12_reg_isp_wdma_crop_high (242)
	#define mask_of_isp12_reg_isp_wdma_crop_high (0xfff)
	unsigned int reg_isp_wdma_crop_high:12;

	// h0079, bit: 14
	/* */
	unsigned int :4;

	// h0079
	unsigned int /* padding 16 bit */:16;

	// h007a, bit: 11
	/* Sensor Line Counter*/
	#define offset_of_isp12_reg_isp_sensor_line_cnt (244)
	#define mask_of_isp12_reg_isp_sensor_line_cnt (0xfff)
	unsigned int reg_isp_sensor_line_cnt:12;

	// h007a, bit: 14
	/* */
	unsigned int :4;

	// h007a
	unsigned int /* padding 16 bit */:16;

	// h007b, bit: 3
	/* Hardware Debug Signal Sets Selector*/
	#define offset_of_isp12_reg_isp_debug_sel (246)
	#define mask_of_isp12_reg_isp_debug_sel (0xf)
	unsigned int reg_isp_debug_sel:4;

	// h007b, bit: 14
	/* */
	unsigned int :12;

	// h007b
	unsigned int /* padding 16 bit */:16;

	// h007c, bit: 14
	/* isp debug out*/
	#define offset_of_isp12_reg_isp_debug_out (248)
	#define mask_of_isp12_reg_isp_debug_out (0xffff)
	unsigned int reg_isp_debug_out:16;

	// h007c
	unsigned int /* padding 16 bit */:16;

	// h007d, bit: 11
	/* SW specify interrupt line number*/
	#define offset_of_isp12_reg_sw_specify_int_line (250)
	#define mask_of_isp12_reg_sw_specify_int_line (0xfff)
	unsigned int reg_sw_specify_int_line:12;

	// h007d, bit: 14
	/* */
	unsigned int :4;

	// h007d
	unsigned int /* padding 16 bit */:16;

	// h007e, bit: 11
	/* Sensor to Crop line count*/
	#define offset_of_isp12_reg_isp_sr2crop_line_cnt (252)
	#define mask_of_isp12_reg_isp_sr2crop_line_cnt (0xfff)
	unsigned int reg_isp_sr2crop_line_cnt:12;

	// h007e, bit: 14
	/* */
	unsigned int :4;

	// h007e
	unsigned int /* padding 16 bit */:16;

	// h007f, bit: 14
	/* */
	unsigned int :16;

	// h007f
	unsigned int /* padding 16 bit */:16;

}  __attribute__((packed, aligned(1))) infinity2_reg_isp12;
#endif
