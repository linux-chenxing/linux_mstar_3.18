// Generate Time: 2017-09-19 22:58:05.086419
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
#ifndef __INFINITY2_REG_ISP_DMAG_SUB_M2__
#define __INFINITY2_REG_ISP_DMAG_SUB_M2__
typedef struct {
	// h0000, bit: 14
	/* */
	unsigned int :16;

	// h0000
	unsigned int /* padding 16 bit */:16;

	// h0001, bit: 14
	/* */
	unsigned int :16;

	// h0001
	unsigned int /* padding 16 bit */:16;

	// h0002, bit: 14
	/* */
	unsigned int :16;

	// h0002
	unsigned int /* padding 16 bit */:16;

	// h0003, bit: 14
	/* */
	unsigned int :16;

	// h0003
	unsigned int /* padding 16 bit */:16;

	// h0004, bit: 14
	/* */
	unsigned int :16;

	// h0004
	unsigned int /* padding 16 bit */:16;

	// h0005, bit: 14
	/* */
	unsigned int :16;

	// h0005
	unsigned int /* padding 16 bit */:16;

	// h0006, bit: 14
	/* */
	unsigned int :16;

	// h0006
	unsigned int /* padding 16 bit */:16;

	// h0007, bit: 14
	/* */
	unsigned int :16;

	// h0007
	unsigned int /* padding 16 bit */:16;

	// h0008, bit: 14
	/* */
	unsigned int :16;

	// h0008
	unsigned int /* padding 16 bit */:16;

	// h0009, bit: 14
	/* */
	unsigned int :16;

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

	// h000c, bit: 14
	/* */
	unsigned int :16;

	// h000c
	unsigned int /* padding 16 bit */:16;

	// h000d, bit: 14
	/* */
	unsigned int :16;

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

	// h0010, bit: 14
	/* */
	unsigned int :16;

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

	// h0018, bit: 14
	/* */
	unsigned int :16;

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

	// h001f, bit: 14
	/* */
	unsigned int :16;

	// h001f
	unsigned int /* padding 16 bit */:16;

	// h0020, bit: 0
	/* ISP WDMA enable*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_en (64)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_en (0x1)
	unsigned int reg_isp_wdma1_en:1;

	// h0020, bit: 1
	/* ISP WDMA enable but data not write to DRAM*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_mask_out (64)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_mask_out (0x2)
	unsigned int reg_isp_wdma1_mask_out:1;

	// h0020, bit: 2
	/* */
	unsigned int :1;

	// h0020, bit: 3
	/* ISP WDMA MIU wbe mask*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_wbe_mask (64)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_wbe_mask (0x8)
	unsigned int reg_isp_wdma1_wbe_mask:1;

	// h0020, bit: 4
	/* ISP WDMA auto vsync trigger mode*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_auto (64)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_auto (0x10)
	unsigned int reg_isp_wdma1_auto:1;

	// h0020, bit: 6
	/* */
	unsigned int :2;

	// h0020, bit: 7
	/* ISP WDMA high priority set*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_wreq_hpri_set (64)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_wreq_hpri_set (0x80)
	unsigned int reg_isp_wdma1_wreq_hpri_set:1;

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
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_mode (64)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_mode (0xf00)
	unsigned int reg_isp_wdma1_mode:4;

	// h0020, bit: 14
	/* */
	unsigned int :3;

	// h0020, bit: 15
	/* ISP WDMA MIU request reset*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_wreq_rst (64)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_wreq_rst (0x8000)
	unsigned int reg_isp_wdma1_wreq_rst:1;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 12
	/* ISP WDMA pitch*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_pitch (66)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_pitch (0x1fff)
	unsigned int reg_isp_wdma1_pitch:13;

	// h0021, bit: 14
	/* */
	unsigned int :3;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 12
	/* ISP WDMA assert IRQ at linex*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_irqlinex (68)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_irqlinex (0x1fff)
	unsigned int reg_isp_wdma1_irqlinex:13;

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
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_width_m1 (72)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_width_m1 (0xffff)
	unsigned int reg_isp_wdma1_width_m1:16;

	// h0024
	unsigned int /* padding 16 bit */:16;

	// h0025, bit: 12
	/* ISP WDMA height minus 1*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_height_m1 (74)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_height_m1 (0x1fff)
	unsigned int reg_isp_wdma1_height_m1:13;

	// h0025, bit: 14
	/* */
	unsigned int :2;

	// h0025, bit: 15
	/* ISP WDMA height limit enable*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_height_en (74)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_height_en (0x8000)
	unsigned int reg_isp_wdma1_height_en:1;

	// h0025
	unsigned int /* padding 16 bit */:16;

	// h0026, bit: 14
	/* ISP WDMA write limit address*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_w_limit_adr (76)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_w_limit_adr (0xffff)
	unsigned int reg_isp_wdma1_w_limit_adr:16;

	// h0026
	unsigned int /* padding 16 bit */:16;

	// h0027, bit: 10
	/* ISP WDMA write limit address*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_w_limit_adr_1 (78)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_w_limit_adr_1 (0x7ff)
	unsigned int reg_isp_wdma1_w_limit_adr_1:11;

	// h0027, bit: 14
	/* */
	unsigned int :4;

	// h0027, bit: 15
	/* ISP WDMA write limit enable*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_w_limit_en (78)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_w_limit_en (0x8000)
	unsigned int reg_isp_wdma1_w_limit_en:1;

	// h0027
	unsigned int /* padding 16 bit */:16;

	// h0028, bit: 14
	/* ISP WDMA MIU base address*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_base (80)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_base (0xffff)
	unsigned int reg_isp_wdma1_base:16;

	// h0028
	unsigned int /* padding 16 bit */:16;

	// h0029, bit: 10
	/* ISP WDMA MIU base address*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_base_1 (82)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_base_1 (0x7ff)
	unsigned int reg_isp_wdma1_base_1:11;

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
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_wreq_thrd (88)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_wreq_thrd (0x7)
	unsigned int reg_isp_wdma1_wreq_thrd:3;

	// h002c, bit: 3
	/* */
	unsigned int :1;

	// h002c, bit: 7
	/* ISP WDMA MIU high priority threshold*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_wreq_hpri (88)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_wreq_hpri (0xf0)
	unsigned int reg_isp_wdma1_wreq_hpri:4;

	// h002c, bit: 11
	/* ISP WDMA MIU burst number*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_wreq_max (88)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_wreq_max (0xf00)
	unsigned int reg_isp_wdma1_wreq_max:4;

	// h002c, bit: 14
	/* */
	unsigned int :4;

	// h002c
	unsigned int /* padding 16 bit */:16;

	// h002d, bit: 14
	/* SW write fence ID*/
	#define offset_of_dmag_sub_m2_reg_isp_sw_w_fence1 (90)
	#define mask_of_dmag_sub_m2_reg_isp_sw_w_fence1 (0xffff)
	unsigned int reg_isp_sw_w_fence1:16;

	// h002d
	unsigned int /* padding 16 bit */:16;

	// h002e, bit: 14
	/* SW read fence ID*/
	#define offset_of_dmag_sub_m2_reg_isp_sw_r_fence1 (92)
	#define mask_of_dmag_sub_m2_reg_isp_sw_r_fence1 (0xffff)
	unsigned int reg_isp_sw_r_fence1:16;

	// h002e
	unsigned int /* padding 16 bit */:16;

	// h002f, bit: 14
	/* */
	unsigned int :16;

	// h002f
	unsigned int /* padding 16 bit */:16;

	// h0030, bit: 0
	/* ISP WDMA start trigger*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_trigger (96)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_trigger (0x1)
	unsigned int reg_isp_wdma1_trigger:1;

	// h0030, bit: 1
	/* ISP WDMA start trigger vsync aligned mode*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_trigger_mode (96)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_trigger_mode (0x2)
	unsigned int reg_isp_wdma1_trigger_mode:1;

	// h0030, bit: 7
	/* */
	unsigned int :6;

	// h0030, bit: 8
	/* ISP WDMA input auto-align enable*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_align_en (96)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_align_en (0x100)
	unsigned int reg_isp_wdma1_align_en:1;

	// h0030, bit: 14
	/* */
	unsigned int :7;

	// h0030
	unsigned int /* padding 16 bit */:16;

	// h0031, bit: 0
	/* ISP WDMA valid data msb align at 16bit mode*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_msb_align (98)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_msb_align (0x1)
	unsigned int reg_isp_wdma1_msb_align:1;

	// h0031, bit: 3
	/* */
	unsigned int :3;

	// h0031, bit: 7
	/* ISP WDMA lsb mode, right shift bits*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_lsb_shift (98)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_lsb_shift (0xf0)
	unsigned int reg_isp_wdma1_lsb_shift:4;

	// h0031, bit: 14
	/* */
	unsigned int :8;

	// h0031
	unsigned int /* padding 16 bit */:16;

	// h0032, bit: 14
	/* dummy*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_dummy (100)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_dummy (0xffff)
	unsigned int reg_isp_wdma1_dummy:16;

	// h0032
	unsigned int /* padding 16 bit */:16;

	// h0033, bit: 14
	/* dummy*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_dummy_1 (102)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_dummy_1 (0xffff)
	unsigned int reg_isp_wdma1_dummy_1:16;

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
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_eof_read (108)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_eof_read (0x1)
	unsigned int reg_isp_wdma1_eof_read:1;

	// h0036, bit: 7
	/* */
	unsigned int :7;

	// h0036, bit: 8
	/* ISP WDMA LB full status*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_lb_full_read (108)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_lb_full_read (0x100)
	unsigned int reg_isp_wdma1_lb_full_read:1;

	// h0036, bit: 14
	/* */
	unsigned int :6;

	// h0036, bit: 15
	/* ISP WDMA status clear*/
	#define offset_of_dmag_sub_m2_reg_isp_wdma1_status_clr (108)
	#define mask_of_dmag_sub_m2_reg_isp_wdma1_status_clr (0x8000)
	unsigned int reg_isp_wdma1_status_clr:1;

	// h0036
	unsigned int /* padding 16 bit */:16;

	// h0037, bit: 0
	/* ISP DMA mclk req force active*/
	#define offset_of_dmag_sub_m2_reg_mreq1_always_active (110)
	#define mask_of_dmag_sub_m2_reg_mreq1_always_active (0x1)
	unsigned int reg_mreq1_always_active:1;

	// h0037, bit: 1
	/* ISP DMA mclk req force off*/
	#define offset_of_dmag_sub_m2_reg_mreq1_force_off (110)
	#define mask_of_dmag_sub_m2_reg_mreq1_force_off (0x2)
	unsigned int reg_mreq1_force_off:1;

	// h0037, bit: 2
	/* ISP DMA mclk req wdma mode*/
	#define offset_of_dmag_sub_m2_reg_mreq1_wdma_mode (110)
	#define mask_of_dmag_sub_m2_reg_mreq1_wdma_mode (0x4)
	unsigned int reg_mreq1_wdma_mode:1;

	// h0037, bit: 14
	/* */
	unsigned int :13;

	// h0037
	unsigned int /* padding 16 bit */:16;

	// h0038, bit: 14
	/* */
	unsigned int :16;

	// h0038
	unsigned int /* padding 16 bit */:16;

	// h0039, bit: 14
	/* */
	unsigned int :16;

	// h0039
	unsigned int /* padding 16 bit */:16;

	// h003a, bit: 14
	/* */
	unsigned int :16;

	// h003a
	unsigned int /* padding 16 bit */:16;

	// h003b, bit: 14
	/* */
	unsigned int :16;

	// h003b
	unsigned int /* padding 16 bit */:16;

	// h003c, bit: 14
	/* */
	unsigned int :16;

	// h003c
	unsigned int /* padding 16 bit */:16;

	// h003d, bit: 14
	/* */
	unsigned int :16;

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

}  __attribute__((packed, aligned(1))) infinity2_reg_isp_dmag_sub_m2;
#endif
