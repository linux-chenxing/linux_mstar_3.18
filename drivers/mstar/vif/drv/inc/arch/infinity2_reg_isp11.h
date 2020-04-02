// Generate Time: 2017-09-19 22:58:04.528546
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
#ifndef __INFINITY2_REG_ISP11__
#define __INFINITY2_REG_ISP11__
typedef struct {
	// h0000, bit: 0
	/* fpn enable*/
	#define offset_of_isp11_reg_fpn_en (0)
	#define mask_of_isp11_reg_fpn_en (0x1)
	unsigned int reg_fpn_en:1;

	// h0000, bit: 1
	/* 0: statis for every frame set 1: statis for just first frame set*/
	#define offset_of_isp11_reg_fpn_mode (0)
	#define mask_of_isp11_reg_fpn_mode (0x2)
	unsigned int reg_fpn_mode:1;

	// h0000, bit: 2
	/* 0:don’t clear sum result 1:  clear sum result*/
	#define offset_of_isp11_reg_fpn_sum_clr (0)
	#define mask_of_isp11_reg_fpn_sum_clr (0x4)
	unsigned int reg_fpn_sum_clr:1;

	// h0000, bit: 3
	/* SW setting eable*/
	#define offset_of_isp11_reg_sw_offset_on (0)
	#define mask_of_isp11_reg_sw_offset_on (0x8)
	unsigned int reg_sw_offset_on:1;

	// h0000, bit: 8
	/* ob height*/
	#define offset_of_isp11_reg_fpn_height (0)
	#define mask_of_isp11_reg_fpn_height (0x1f0)
	unsigned int reg_fpn_height:5;

	// h0000, bit: 9
	/* sensor hsync line start enable*/
	#define offset_of_isp11_reg_fpn_sensor_line_start (0)
	#define mask_of_isp11_reg_fpn_sensor_line_start (0x200)
	unsigned int reg_fpn_sensor_line_start:1;

	// h0000, bit: 14
	/* statis frame number*/
	#define offset_of_isp11_reg_fpn_frame_num (0)
	#define mask_of_isp11_reg_fpn_frame_num (0x7c00)
	unsigned int reg_fpn_frame_num:5;

	// h0000, bit: 15
	/* */
	unsigned int :1;

	// h0000
	unsigned int /* padding 16 bit */:16;

	// h0001, bit: 12
	/* ob start x */
	#define offset_of_isp11_reg_fpn_start_x (2)
	#define mask_of_isp11_reg_fpn_start_x (0x1fff)
	unsigned int reg_fpn_start_x:13;

	// h0001, bit: 14
	/* */
	unsigned int :3;

	// h0001
	unsigned int /* padding 16 bit */:16;

	// h0002, bit: 12
	/* ob start xy*/
	#define offset_of_isp11_reg_fpn_start_y (4)
	#define mask_of_isp11_reg_fpn_start_y (0x1fff)
	unsigned int reg_fpn_start_y:13;

	// h0002, bit: 14
	/* */
	unsigned int :3;

	// h0002
	unsigned int /* padding 16 bit */:16;

	// h0003, bit: 12
	/* ob width*/
	#define offset_of_isp11_reg_fpn_width (6)
	#define mask_of_isp11_reg_fpn_width (0x1fff)
	unsigned int reg_fpn_width:13;

	// h0003, bit: 14
	/* */
	unsigned int :3;

	// h0003
	unsigned int /* padding 16 bit */:16;

	// h0004, bit: 14
	/* pre-offset for pixels at active area*/
	#define offset_of_isp11_reg_fpn_preoffset (8)
	#define mask_of_isp11_reg_fpn_preoffset (0x7fff)
	unsigned int reg_fpn_preoffset:15;

	// h0004, bit: 15
	/* sign of pre-offset */
	#define offset_of_isp11_reg_fpn_preoffset_sign (8)
	#define mask_of_isp11_reg_fpn_preoffset_sign (0x8000)
	unsigned int reg_fpn_preoffset_sign:1;

	// h0004
	unsigned int /* padding 16 bit */:16;

	// h0005, bit: 14
	/* midium pixel and pavg diff threshould*/
	#define offset_of_isp11_reg_fpn_diff_th (10)
	#define mask_of_isp11_reg_fpn_diff_th (0xffff)
	unsigned int reg_fpn_diff_th:16;

	// h0005
	unsigned int /* padding 16 bit */:16;

	// h0006, bit: 11
	/* compare ratio*/
	#define offset_of_isp11_reg_fpn_cmp_ratio (12)
	#define mask_of_isp11_reg_fpn_cmp_ratio (0xfff)
	unsigned int reg_fpn_cmp_ratio:12;

	// h0006, bit: 14
	/* */
	unsigned int :4;

	// h0006
	unsigned int /* padding 16 bit */:16;

	// h0007, bit: 14
	/* SW offset setting write data*/
	#define offset_of_isp11_reg_sw_offset_wd (14)
	#define mask_of_isp11_reg_sw_offset_wd (0xffff)
	unsigned int reg_sw_offset_wd:16;

	// h0007
	unsigned int /* padding 16 bit */:16;

	// h0008, bit: 12
	/* SW offset setting write address*/
	#define offset_of_isp11_reg_sw_offset_a (16)
	#define mask_of_isp11_reg_sw_offset_a (0x1fff)
	unsigned int reg_sw_offset_a:13;

	// h0008, bit: 14
	/* */
	unsigned int :3;

	// h0008
	unsigned int /* padding 16 bit */:16;

	// h0009, bit: 0
	/* SW offset setting write control*/
	#define offset_of_isp11_reg_sw_offset_we (18)
	#define mask_of_isp11_reg_sw_offset_we (0x1)
	unsigned int reg_sw_offset_we:1;

	// h0009, bit: 1
	/* SW setting eable*/
	#define offset_of_isp11_reg_sw_offset_re (18)
	#define mask_of_isp11_reg_sw_offset_re (0x2)
	unsigned int reg_sw_offset_re:1;

	// h0009, bit: 14
	/* */
	unsigned int :14;

	// h0009
	unsigned int /* padding 16 bit */:16;

	// h000a, bit: 14
	/* SW setting read data*/
	#define offset_of_isp11_reg_sw_offset_rd (20)
	#define mask_of_isp11_reg_sw_offset_rd (0xffff)
	unsigned int reg_sw_offset_rd:16;

	// h000a
	unsigned int /* padding 16 bit */:16;

	// h000b, bit: 12
	/* sensor width register, width -1
	active when reg_fpn_sensor_line_start enable*/
	#define offset_of_isp11_reg_fpn_sensor_width (22)
	#define mask_of_isp11_reg_fpn_sensor_width (0x1fff)
	unsigned int reg_fpn_sensor_width:13;

	// h000b, bit: 14
	/* */
	unsigned int :3;

	// h000b
	unsigned int /* padding 16 bit */:16;

	// h000c, bit: 12
	/* sensor height register,  height -1
	active when reg_fpn_sensor_line_start enable*/
	#define offset_of_isp11_reg_fpn_sensor_height (24)
	#define mask_of_isp11_reg_fpn_sensor_height (0x1fff)
	unsigned int reg_fpn_sensor_height:13;

	// h000c, bit: 14
	/* */
	unsigned int :3;

	// h000c
	unsigned int /* padding 16 bit */:16;

	// h000d, bit: 12
	/* obc line cnt number1 for interrupt*/
	#define offset_of_isp11_reg_isp_obc_line_cnt1 (26)
	#define mask_of_isp11_reg_isp_obc_line_cnt1 (0x1fff)
	unsigned int reg_isp_obc_line_cnt1:13;

	// h000d, bit: 14
	/* */
	unsigned int :3;

	// h000d
	unsigned int /* padding 16 bit */:16;

	// h000e, bit: 12
	/* obc line cnt number2 for interrupt*/
	#define offset_of_isp11_reg_isp_obc_line_cnt2 (28)
	#define mask_of_isp11_reg_isp_obc_line_cnt2 (0x1fff)
	unsigned int reg_isp_obc_line_cnt2:13;

	// h000e, bit: 14
	/* */
	unsigned int :3;

	// h000e
	unsigned int /* padding 16 bit */:16;

	// h000f, bit: 12
	/* obc line cnt number3 for interrupt*/
	#define offset_of_isp11_reg_isp_obc_line_cnt3 (30)
	#define mask_of_isp11_reg_isp_obc_line_cnt3 (0x1fff)
	unsigned int reg_isp_obc_line_cnt3:13;

	// h000f, bit: 14
	/* */
	unsigned int :3;

	// h000f
	unsigned int /* padding 16 bit */:16;

	// h0010, bit: 11
	/* Shift Base for Optical Black Correction
	input pixel is R,
	formula: Cout = (Cin – reg_isp_obc_r_ofst) * b
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_obc_r_ofst (32)
	#define mask_of_isp11_reg_isp_obc_r_ofst (0xfff)
	unsigned int reg_isp_obc_r_ofst:12;

	// h0010, bit: 14
	/* */
	unsigned int :4;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 11
	/* Shift Base for Optical Black Correction
	input pixel is GR,
	formula: Cout = (Cin – reg_isp_obc_gr_ofst) * b
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_obc_gr_ofst (34)
	#define mask_of_isp11_reg_isp_obc_gr_ofst (0xfff)
	unsigned int reg_isp_obc_gr_ofst:12;

	// h0011, bit: 14
	/* */
	unsigned int :4;

	// h0011
	unsigned int /* padding 16 bit */:16;

	// h0012, bit: 11
	/* Shift Base for Optical Black Correction
	input pixel is GB,
	formula: Cout = (Cin – reg_isp_obc_gb_ofst) * b
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_obc_gb_ofst (36)
	#define mask_of_isp11_reg_isp_obc_gb_ofst (0xfff)
	unsigned int reg_isp_obc_gb_ofst:12;

	// h0012, bit: 14
	/* */
	unsigned int :4;

	// h0012
	unsigned int /* padding 16 bit */:16;

	// h0013, bit: 11
	/* Shift Base for Optical Black Correction
	input pixel is B,
	formula: Cout = (Cin – reg_isp_obc_b_ofst) * b
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_obc_b_ofst (38)
	#define mask_of_isp11_reg_isp_obc_b_ofst (0xfff)
	unsigned int reg_isp_obc_b_ofst:12;

	// h0013, bit: 14
	/* */
	unsigned int :4;

	// h0013
	unsigned int /* padding 16 bit */:16;

	// h0014, bit: 13
	/* Shift Gain for Optical Black Correction
	formula: Cout = (Cin – ofst) * b
	input pixel is R,
	b = reg_isp_obc_r_gain
	precision 4.10
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_obc_r_gain (40)
	#define mask_of_isp11_reg_isp_obc_r_gain (0x3fff)
	unsigned int reg_isp_obc_r_gain:14;

	// h0014, bit: 14
	/* */
	unsigned int :2;

	// h0014
	unsigned int /* padding 16 bit */:16;

	// h0015, bit: 13
	/* Shift Gain for Optical Black Correction
	formula: Cout = (Cin – ofst) * b
	input pixel is GR,
	b = reg_isp_obc_gr_gain
	precision 4.10
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_obc_gr_gain (42)
	#define mask_of_isp11_reg_isp_obc_gr_gain (0x3fff)
	unsigned int reg_isp_obc_gr_gain:14;

	// h0015, bit: 14
	/* */
	unsigned int :2;

	// h0015
	unsigned int /* padding 16 bit */:16;

	// h0016, bit: 13
	/* Shift Gain for Optical Black Correction
	formula: Cout = (Cin – ofst) * b
	input pixel is GB,
	b = reg_isp_obc_gb_gain
	precision 4.10
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_obc_gb_gain (44)
	#define mask_of_isp11_reg_isp_obc_gb_gain (0x3fff)
	unsigned int reg_isp_obc_gb_gain:14;

	// h0016, bit: 14
	/* */
	unsigned int :2;

	// h0016
	unsigned int /* padding 16 bit */:16;

	// h0017, bit: 13
	/* Shift Gain for Optical Black Correction
	formula: Cout = (Cin – ofst) * b
	input pixel is b,
	b = reg_isp_obc_b_gain
	precision 4.10
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_obc_b_gain (46)
	#define mask_of_isp11_reg_isp_obc_b_gain (0x3fff)
	unsigned int reg_isp_obc_b_gain:14;

	// h0017, bit: 14
	/* */
	unsigned int :2;

	// h0017
	unsigned int /* padding 16 bit */:16;

	// h0018, bit: 0
	/* 1'b1: Enable AE gain
	1'b0: Disable(default)*/
	#define offset_of_isp11_reg_isp_ae_en (48)
	#define mask_of_isp11_reg_isp_ae_en (0x1)
	unsigned int reg_isp_ae_en:1;

	// h0018, bit: 1
	/* 1'b1: Enable OBC gain
	1'b0: Disable(default)*/
	#define offset_of_isp11_reg_isp_obc_en (48)
	#define mask_of_isp11_reg_isp_obc_en (0x2)
	unsigned int reg_isp_obc_en:1;

	// h0018, bit: 2
	/* obc dither enable*/
	#define offset_of_isp11_reg_obc_dith_en (48)
	#define mask_of_isp11_reg_obc_dith_en (0x4)
	unsigned int reg_obc_dith_en:1;

	// h0018, bit: 14
	/* */
	unsigned int :13;

	// h0018
	unsigned int /* padding 16 bit */:16;

	// h0019, bit: 13
	/* AE gain
	out = in * reg_isp_ae_gain
	precision 4.10
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_ae_gain (50)
	#define mask_of_isp11_reg_isp_ae_gain (0x3fff)
	unsigned int reg_isp_ae_gain:14;

	// h0019, bit: 14
	/* */
	unsigned int :2;

	// h0019
	unsigned int /* padding 16 bit */:16;

	// h001a, bit: 11
	/* Enable 12-bits to 12-bits Gamma at bayer domain*/
	#define offset_of_isp11_reg_gm12to12_max_data (52)
	#define mask_of_isp11_reg_gm12to12_max_data (0xfff)
	unsigned int reg_gm12to12_max_data:12;

	// h001a, bit: 12
	/* Enable 12-bits to 12-bits Gamma at bayer domain*/
	#define offset_of_isp11_reg_gm12to12_en (52)
	#define mask_of_isp11_reg_gm12to12_en (0x1000)
	unsigned int reg_gm12to12_en:1;

	// h001a, bit: 13
	/* Enable 12-bits to 12-bits Gamma at bayer domain*/
	#define offset_of_isp11_reg_gm12to12_max_en (52)
	#define mask_of_isp11_reg_gm12to12_max_en (0x2000)
	unsigned int reg_gm12to12_max_en:1;

	// h001a, bit: 14
	/* */
	unsigned int :2;

	// h001a
	unsigned int /* padding 16 bit */:16;

	// h001b, bit: 0
	/* White Balance Gain Dither Enable*/
	#define offset_of_isp11_reg_wbg_dith_en (54)
	#define mask_of_isp11_reg_wbg_dith_en (0x1)
	unsigned int reg_wbg_dith_en:1;

	// h001b, bit: 1
	/* White Balance pre-Gain Enable*/
	#define offset_of_isp11_reg_isp_wbg0_en (54)
	#define mask_of_isp11_reg_isp_wbg0_en (0x2)
	unsigned int reg_isp_wbg0_en:1;

	// h001b, bit: 14
	/* */
	unsigned int :14;

	// h001b
	unsigned int /* padding 16 bit */:16;

	// h001c, bit: 12
	/* White Balance Gain for R channel (3.10)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wbg_r_gain (56)
	#define mask_of_isp11_reg_isp_wbg_r_gain (0x1fff)
	unsigned int reg_isp_wbg_r_gain:13;

	// h001c, bit: 14
	/* */
	unsigned int :3;

	// h001c
	unsigned int /* padding 16 bit */:16;

	// h001d, bit: 12
	/* White Balance Offset for R channel (s10)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wbg_r_offset (58)
	#define mask_of_isp11_reg_isp_wbg_r_offset (0x1fff)
	unsigned int reg_isp_wbg_r_offset:13;

	// h001d, bit: 14
	/* */
	unsigned int :3;

	// h001d
	unsigned int /* padding 16 bit */:16;

	// h001e, bit: 12
	/* White Balance Gain for Gr channel (3.10)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wbg_gr_gain (60)
	#define mask_of_isp11_reg_isp_wbg_gr_gain (0x1fff)
	unsigned int reg_isp_wbg_gr_gain:13;

	// h001e, bit: 14
	/* */
	unsigned int :3;

	// h001e
	unsigned int /* padding 16 bit */:16;

	// h001f, bit: 12
	/* White Balance Offset for Gr channel (s10)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wbg_gr_offset (62)
	#define mask_of_isp11_reg_isp_wbg_gr_offset (0x1fff)
	unsigned int reg_isp_wbg_gr_offset:13;

	// h001f, bit: 14
	/* */
	unsigned int :3;

	// h001f
	unsigned int /* padding 16 bit */:16;

	// h0020, bit: 12
	/* White Balance Gain for B channel (3.10)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wbg_b_gain (64)
	#define mask_of_isp11_reg_isp_wbg_b_gain (0x1fff)
	unsigned int reg_isp_wbg_b_gain:13;

	// h0020, bit: 14
	/* */
	unsigned int :3;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 12
	/* White Balance Offset for B channel (s10)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wbg_b_offset (66)
	#define mask_of_isp11_reg_isp_wbg_b_offset (0x1fff)
	unsigned int reg_isp_wbg_b_offset:13;

	// h0021, bit: 14
	/* */
	unsigned int :3;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 12
	/* White Balance Gain for Gb channel (3.10)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wbg_gb_gain (68)
	#define mask_of_isp11_reg_isp_wbg_gb_gain (0x1fff)
	unsigned int reg_isp_wbg_gb_gain:13;

	// h0022, bit: 14
	/* */
	unsigned int :3;

	// h0022
	unsigned int /* padding 16 bit */:16;

	// h0023, bit: 12
	/* White Balance Offset for Gb channel (s10)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wbg_gb_offset (70)
	#define mask_of_isp11_reg_isp_wbg_gb_offset (0x1fff)
	unsigned int reg_isp_wbg_gb_offset:13;

	// h0023, bit: 14
	/* */
	unsigned int :3;

	// h0023
	unsigned int /* padding 16 bit */:16;

	// h0024, bit: 0
	/* Post White Balance Gain Dither Enable*/
	#define offset_of_isp11_reg_wbg1_dith_en (72)
	#define mask_of_isp11_reg_wbg1_dith_en (0x1)
	unsigned int reg_wbg1_dith_en:1;

	// h0024, bit: 1
	/* Post White Balance Gain Enable*/
	#define offset_of_isp11_reg_isp_wbg1_en (72)
	#define mask_of_isp11_reg_isp_wbg1_en (0x2)
	unsigned int reg_isp_wbg1_en:1;

	// h0024, bit: 14
	/* */
	unsigned int :14;

	// h0024
	unsigned int /* padding 16 bit */:16;

	// h0025, bit: 12
	/* Post White Balance Gain for R channel (3.10)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wbg1_r_gain (74)
	#define mask_of_isp11_reg_isp_wbg1_r_gain (0x1fff)
	unsigned int reg_isp_wbg1_r_gain:13;

	// h0025, bit: 14
	/* */
	unsigned int :3;

	// h0025
	unsigned int /* padding 16 bit */:16;

	// h0026, bit: 12
	/* Post White Balance Offset for R channel (s10)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wbg1_r_offset (76)
	#define mask_of_isp11_reg_isp_wbg1_r_offset (0x1fff)
	unsigned int reg_isp_wbg1_r_offset:13;

	// h0026, bit: 14
	/* */
	unsigned int :3;

	// h0026
	unsigned int /* padding 16 bit */:16;

	// h0027, bit: 12
	/* Post White Balance Gain for Gr channel (3.10)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wbg1_gr_gain (78)
	#define mask_of_isp11_reg_isp_wbg1_gr_gain (0x1fff)
	unsigned int reg_isp_wbg1_gr_gain:13;

	// h0027, bit: 14
	/* */
	unsigned int :3;

	// h0027
	unsigned int /* padding 16 bit */:16;

	// h0028, bit: 12
	/* Post White Balance Offset for Gr channel (s10)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wbg1_gr_offset (80)
	#define mask_of_isp11_reg_isp_wbg1_gr_offset (0x1fff)
	unsigned int reg_isp_wbg1_gr_offset:13;

	// h0028, bit: 14
	/* */
	unsigned int :3;

	// h0028
	unsigned int /* padding 16 bit */:16;

	// h0029, bit: 12
	/* Post White Balance Gain for B channel (3.10)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wbg1_b_gain (82)
	#define mask_of_isp11_reg_isp_wbg1_b_gain (0x1fff)
	unsigned int reg_isp_wbg1_b_gain:13;

	// h0029, bit: 14
	/* */
	unsigned int :3;

	// h0029
	unsigned int /* padding 16 bit */:16;

	// h002a, bit: 12
	/* Post White Balance Offset for B channel (s10)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wbg1_b_offset (84)
	#define mask_of_isp11_reg_isp_wbg1_b_offset (0x1fff)
	unsigned int reg_isp_wbg1_b_offset:13;

	// h002a, bit: 14
	/* */
	unsigned int :3;

	// h002a
	unsigned int /* padding 16 bit */:16;

	// h002b, bit: 12
	/* Post White Balance Gain for Gb channel (3.10)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wbg1_gb_gain (86)
	#define mask_of_isp11_reg_isp_wbg1_gb_gain (0x1fff)
	unsigned int reg_isp_wbg1_gb_gain:13;

	// h002b, bit: 14
	/* */
	unsigned int :3;

	// h002b
	unsigned int /* padding 16 bit */:16;

	// h002c, bit: 12
	/* Post White Balance Offset for Gb channel (s10)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wbg1_gb_offset (88)
	#define mask_of_isp11_reg_isp_wbg1_gb_offset (0x1fff)
	unsigned int reg_isp_wbg1_gb_offset:13;

	// h002c, bit: 14
	/* */
	unsigned int :3;

	// h002c
	unsigned int /* padding 16 bit */:16;

	// h002d, bit: 0
	/* lsc enable*/
	#define offset_of_isp11_reg_lsc_en (90)
	#define mask_of_isp11_reg_lsc_en (0x1)
	unsigned int reg_lsc_en:1;

	// h002d, bit: 1
	/* lsc dither enable*/
	#define offset_of_isp11_reg_lsc_dith_en (90)
	#define mask_of_isp11_reg_lsc_dith_en (0x2)
	unsigned int reg_lsc_dith_en:1;

	// h002d, bit: 14
	/* */
	unsigned int :14;

	// h002d
	unsigned int /* padding 16 bit */:16;

	// h002e, bit: 12
	/* X Center Position for Lens Shading Compensation
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_center_x (92)
	#define mask_of_isp11_reg_isp_lsc_center_x (0x1fff)
	unsigned int reg_isp_lsc_center_x:13;

	// h002e, bit: 14
	/* */
	unsigned int :3;

	// h002e
	unsigned int /* padding 16 bit */:16;

	// h002f, bit: 12
	/* Y Center Position for Lens Shading Compensation
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_center_y (94)
	#define mask_of_isp11_reg_isp_lsc_center_y (0x1fff)
	unsigned int reg_isp_lsc_center_y:13;

	// h002f, bit: 14
	/* */
	unsigned int :3;

	// h002f
	unsigned int /* padding 16 bit */:16;

	// h0030, bit: 4
	/* Shift Factor for Lens Shading Compensation
	# 4'b0000: No shift
	# 4'b0001: Shift Right 1 bit
	# 4'b0010: Shift Right 2 bit
	# 4'b0011: Shift Right 3 bit
	# 4'b0100: Shift Right 4 bit
	# 4'b0101: Shift Right 5 bit
	# 4'b0110: Shift Right 6 bit
	# 4'b0111: Shift Right 7 bit
	# 4'b1000: Shift Right 8 bit
	# 4'b1001: Shift Right 9 bit
	# 4'b1010: Shift Right 10 bit
	# 4'b1011: Shift Right 11 bit
	# 4'b1100: Shift Right 12 bit
	# 4'b1101: Shift Right 13 bit
	# 4'b1110: Shift Right 14 bit
	# 4'b1111: Shift Right 15 bit
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_shift (96)
	#define mask_of_isp11_reg_isp_lsc_shift (0x1f)
	unsigned int reg_isp_lsc_shift:5;

	// h0030, bit: 14
	/* */
	unsigned int :11;

	// h0030
	unsigned int /* padding 16 bit */:16;

	// h0031, bit: 7
	/* Global Gain Table for Lens Shading Compensation
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_glb_gain (98)
	#define mask_of_isp11_reg_isp_lsc_glb_gain (0xff)
	unsigned int reg_isp_lsc_glb_gain:8;

	// h0031, bit: 14
	/* */
	unsigned int :8;

	// h0031
	unsigned int /* padding 16 bit */:16;

	// h0032, bit: 7
	/* Gain Table 0 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_0 (100)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_0 (0xff)
	unsigned int reg_isp_lsc_r_gain_table_0:8;

	// h0032, bit: 14
	/* Gain Table 1 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_1 (100)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_1 (0xff00)
	unsigned int reg_isp_lsc_r_gain_table_1:8;

	// h0032
	unsigned int /* padding 16 bit */:16;

	// h0033, bit: 7
	/* Gain Table 2 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_2 (102)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_2 (0xff)
	unsigned int reg_isp_lsc_r_gain_table_2:8;

	// h0033, bit: 14
	/* Gain Table 3 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_3 (102)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_3 (0xff00)
	unsigned int reg_isp_lsc_r_gain_table_3:8;

	// h0033
	unsigned int /* padding 16 bit */:16;

	// h0034, bit: 7
	/* Gain Table 4 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_4 (104)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_4 (0xff)
	unsigned int reg_isp_lsc_r_gain_table_4:8;

	// h0034, bit: 14
	/* Gain Table 5 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_5 (104)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_5 (0xff00)
	unsigned int reg_isp_lsc_r_gain_table_5:8;

	// h0034
	unsigned int /* padding 16 bit */:16;

	// h0035, bit: 7
	/* Gain Table 6 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_6 (106)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_6 (0xff)
	unsigned int reg_isp_lsc_r_gain_table_6:8;

	// h0035, bit: 14
	/* Gain Table 7 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_7 (106)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_7 (0xff00)
	unsigned int reg_isp_lsc_r_gain_table_7:8;

	// h0035
	unsigned int /* padding 16 bit */:16;

	// h0036, bit: 7
	/* Gain Table 8 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_8 (108)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_8 (0xff)
	unsigned int reg_isp_lsc_r_gain_table_8:8;

	// h0036, bit: 14
	/* Gain Table 9 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_9 (108)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_9 (0xff00)
	unsigned int reg_isp_lsc_r_gain_table_9:8;

	// h0036
	unsigned int /* padding 16 bit */:16;

	// h0037, bit: 7
	/* Gain Table 10 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_10 (110)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_10 (0xff)
	unsigned int reg_isp_lsc_r_gain_table_10:8;

	// h0037, bit: 14
	/* Gain Table 11 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_11 (110)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_11 (0xff00)
	unsigned int reg_isp_lsc_r_gain_table_11:8;

	// h0037
	unsigned int /* padding 16 bit */:16;

	// h0038, bit: 7
	/* Gain Table 12 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:0]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_12 (112)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_12 (0xff)
	unsigned int reg_isp_lsc_r_gain_table_12:8;

	// h0038, bit: 14
	/* Gain Table 13 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_13 (112)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_13 (0xff00)
	unsigned int reg_isp_lsc_r_gain_table_13:8;

	// h0038
	unsigned int /* padding 16 bit */:16;

	// h0039, bit: 7
	/* Gain Table 14 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_14 (114)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_14 (0xff)
	unsigned int reg_isp_lsc_r_gain_table_14:8;

	// h0039, bit: 14
	/* Gain Table 15 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_15 (114)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_15 (0xff00)
	unsigned int reg_isp_lsc_r_gain_table_15:8;

	// h0039
	unsigned int /* padding 16 bit */:16;

	// h003a, bit: 7
	/* Gain Table 16 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_16 (116)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_16 (0xff)
	unsigned int reg_isp_lsc_r_gain_table_16:8;

	// h003a, bit: 14
	/* Gain Table 17 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_17 (116)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_17 (0xff00)
	unsigned int reg_isp_lsc_r_gain_table_17:8;

	// h003a
	unsigned int /* padding 16 bit */:16;

	// h003b, bit: 7
	/* Gain Table 18 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_18 (118)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_18 (0xff)
	unsigned int reg_isp_lsc_r_gain_table_18:8;

	// h003b, bit: 14
	/* Gain Table 19 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_19 (118)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_19 (0xff00)
	unsigned int reg_isp_lsc_r_gain_table_19:8;

	// h003b
	unsigned int /* padding 16 bit */:16;

	// h003c, bit: 7
	/* Gain Table 20 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_20 (120)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_20 (0xff)
	unsigned int reg_isp_lsc_r_gain_table_20:8;

	// h003c, bit: 14
	/* Gain Table 21 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_21 (120)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_21 (0xff00)
	unsigned int reg_isp_lsc_r_gain_table_21:8;

	// h003c
	unsigned int /* padding 16 bit */:16;

	// h003d, bit: 7
	/* Gain Table 22 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_22 (122)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_22 (0xff)
	unsigned int reg_isp_lsc_r_gain_table_22:8;

	// h003d, bit: 14
	/* Gain Table 23 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_23 (122)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_23 (0xff00)
	unsigned int reg_isp_lsc_r_gain_table_23:8;

	// h003d
	unsigned int /* padding 16 bit */:16;

	// h003e, bit: 7
	/* Gain Table 24 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_24 (124)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_24 (0xff)
	unsigned int reg_isp_lsc_r_gain_table_24:8;

	// h003e, bit: 14
	/* Gain Table 25 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_25 (124)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_25 (0xff00)
	unsigned int reg_isp_lsc_r_gain_table_25:8;

	// h003e
	unsigned int /* padding 16 bit */:16;

	// h003f, bit: 7
	/* Gain Table 26 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_26 (126)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_26 (0xff)
	unsigned int reg_isp_lsc_r_gain_table_26:8;

	// h003f, bit: 14
	/* Gain Table 27 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_27 (126)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_27 (0xff00)
	unsigned int reg_isp_lsc_r_gain_table_27:8;

	// h003f
	unsigned int /* padding 16 bit */:16;

	// h0040, bit: 7
	/* Gain Table 28 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_28 (128)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_28 (0xff)
	unsigned int reg_isp_lsc_r_gain_table_28:8;

	// h0040, bit: 14
	/* Gain Table 29 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_29 (128)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_29 (0xff00)
	unsigned int reg_isp_lsc_r_gain_table_29:8;

	// h0040
	unsigned int /* padding 16 bit */:16;

	// h0041, bit: 7
	/* Gain Table 30 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_30 (130)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_30 (0xff)
	unsigned int reg_isp_lsc_r_gain_table_30:8;

	// h0041, bit: 14
	/* Gain Table 31 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_r_gain_table_31 (130)
	#define mask_of_isp11_reg_isp_lsc_r_gain_table_31 (0xff00)
	unsigned int reg_isp_lsc_r_gain_table_31:8;

	// h0041
	unsigned int /* padding 16 bit */:16;

	// h0042, bit: 7
	/* Gain Table 0 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_0 (132)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_0 (0xff)
	unsigned int reg_isp_lsc_g_gain_table_0:8;

	// h0042, bit: 14
	/* Gain Table 1 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_1 (132)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_1 (0xff00)
	unsigned int reg_isp_lsc_g_gain_table_1:8;

	// h0042
	unsigned int /* padding 16 bit */:16;

	// h0043, bit: 7
	/* Gain Table 2 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_2 (134)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_2 (0xff)
	unsigned int reg_isp_lsc_g_gain_table_2:8;

	// h0043, bit: 14
	/* Gain Table 3 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_3 (134)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_3 (0xff00)
	unsigned int reg_isp_lsc_g_gain_table_3:8;

	// h0043
	unsigned int /* padding 16 bit */:16;

	// h0044, bit: 7
	/* Gain Table 4 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_4 (136)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_4 (0xff)
	unsigned int reg_isp_lsc_g_gain_table_4:8;

	// h0044, bit: 14
	/* Gain Table 5 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_5 (136)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_5 (0xff00)
	unsigned int reg_isp_lsc_g_gain_table_5:8;

	// h0044
	unsigned int /* padding 16 bit */:16;

	// h0045, bit: 7
	/* Gain Table 6 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_6 (138)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_6 (0xff)
	unsigned int reg_isp_lsc_g_gain_table_6:8;

	// h0045, bit: 14
	/* Gain Table 7 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_7 (138)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_7 (0xff00)
	unsigned int reg_isp_lsc_g_gain_table_7:8;

	// h0045
	unsigned int /* padding 16 bit */:16;

	// h0046, bit: 7
	/* Gain Table 8 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_8 (140)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_8 (0xff)
	unsigned int reg_isp_lsc_g_gain_table_8:8;

	// h0046, bit: 14
	/* Gain Table 9 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_9 (140)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_9 (0xff00)
	unsigned int reg_isp_lsc_g_gain_table_9:8;

	// h0046
	unsigned int /* padding 16 bit */:16;

	// h0047, bit: 7
	/* Gain Table 10 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_10 (142)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_10 (0xff)
	unsigned int reg_isp_lsc_g_gain_table_10:8;

	// h0047, bit: 14
	/* Gain Table 11 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_11 (142)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_11 (0xff00)
	unsigned int reg_isp_lsc_g_gain_table_11:8;

	// h0047
	unsigned int /* padding 16 bit */:16;

	// h0048, bit: 7
	/* Gain Table 12 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_12 (144)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_12 (0xff)
	unsigned int reg_isp_lsc_g_gain_table_12:8;

	// h0048, bit: 14
	/* Gain Table 13 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_13 (144)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_13 (0xff00)
	unsigned int reg_isp_lsc_g_gain_table_13:8;

	// h0048
	unsigned int /* padding 16 bit */:16;

	// h0049, bit: 7
	/* Gain Table 14 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_14 (146)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_14 (0xff)
	unsigned int reg_isp_lsc_g_gain_table_14:8;

	// h0049, bit: 14
	/* Gain Table 15 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_15 (146)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_15 (0xff00)
	unsigned int reg_isp_lsc_g_gain_table_15:8;

	// h0049
	unsigned int /* padding 16 bit */:16;

	// h004a, bit: 7
	/* Gain Table 16 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_16 (148)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_16 (0xff)
	unsigned int reg_isp_lsc_g_gain_table_16:8;

	// h004a, bit: 14
	/* Gain Table 17 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_17 (148)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_17 (0xff00)
	unsigned int reg_isp_lsc_g_gain_table_17:8;

	// h004a
	unsigned int /* padding 16 bit */:16;

	// h004b, bit: 7
	/* Gain Table 18 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_18 (150)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_18 (0xff)
	unsigned int reg_isp_lsc_g_gain_table_18:8;

	// h004b, bit: 14
	/* Gain Table 19 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_19 (150)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_19 (0xff00)
	unsigned int reg_isp_lsc_g_gain_table_19:8;

	// h004b
	unsigned int /* padding 16 bit */:16;

	// h004c, bit: 7
	/* Gain Table 20 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_20 (152)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_20 (0xff)
	unsigned int reg_isp_lsc_g_gain_table_20:8;

	// h004c, bit: 14
	/* Gain Table 21 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_21 (152)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_21 (0xff00)
	unsigned int reg_isp_lsc_g_gain_table_21:8;

	// h004c
	unsigned int /* padding 16 bit */:16;

	// h004d, bit: 7
	/* Gain Table 22 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_22 (154)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_22 (0xff)
	unsigned int reg_isp_lsc_g_gain_table_22:8;

	// h004d, bit: 14
	/* Gain Table 23 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_23 (154)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_23 (0xff00)
	unsigned int reg_isp_lsc_g_gain_table_23:8;

	// h004d
	unsigned int /* padding 16 bit */:16;

	// h004e, bit: 7
	/* Gain Table 24 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_24 (156)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_24 (0xff)
	unsigned int reg_isp_lsc_g_gain_table_24:8;

	// h004e, bit: 14
	/* Gain Table 25 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_25 (156)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_25 (0xff00)
	unsigned int reg_isp_lsc_g_gain_table_25:8;

	// h004e
	unsigned int /* padding 16 bit */:16;

	// h004f, bit: 7
	/* Gain Table 26 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_26 (158)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_26 (0xff)
	unsigned int reg_isp_lsc_g_gain_table_26:8;

	// h004f, bit: 14
	/* Gain Table 27 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_27 (158)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_27 (0xff00)
	unsigned int reg_isp_lsc_g_gain_table_27:8;

	// h004f
	unsigned int /* padding 16 bit */:16;

	// h0050, bit: 7
	/* Gain Table 28 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_28 (160)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_28 (0xff)
	unsigned int reg_isp_lsc_g_gain_table_28:8;

	// h0050, bit: 14
	/* Gain Table 29 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_29 (160)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_29 (0xff00)
	unsigned int reg_isp_lsc_g_gain_table_29:8;

	// h0050
	unsigned int /* padding 16 bit */:16;

	// h0051, bit: 7
	/* Gain Table 30 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_30 (162)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_30 (0xff)
	unsigned int reg_isp_lsc_g_gain_table_30:8;

	// h0051, bit: 14
	/* Gain Table 31 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_g_gain_table_31 (162)
	#define mask_of_isp11_reg_isp_lsc_g_gain_table_31 (0xff00)
	unsigned int reg_isp_lsc_g_gain_table_31:8;

	// h0051
	unsigned int /* padding 16 bit */:16;

	// h0052, bit: 7
	/* Gain Table 0 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_0 (164)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_0 (0xff)
	unsigned int reg_isp_lsc_b_gain_table_0:8;

	// h0052, bit: 14
	/* Gain Table 1 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_1 (164)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_1 (0xff00)
	unsigned int reg_isp_lsc_b_gain_table_1:8;

	// h0052
	unsigned int /* padding 16 bit */:16;

	// h0053, bit: 7
	/* Gain Table 2 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_2 (166)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_2 (0xff)
	unsigned int reg_isp_lsc_b_gain_table_2:8;

	// h0053, bit: 14
	/* Gain Table 3 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_3 (166)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_3 (0xff00)
	unsigned int reg_isp_lsc_b_gain_table_3:8;

	// h0053
	unsigned int /* padding 16 bit */:16;

	// h0054, bit: 7
	/* Gain Table 4 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_4 (168)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_4 (0xff)
	unsigned int reg_isp_lsc_b_gain_table_4:8;

	// h0054, bit: 14
	/* Gain Table 5 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_5 (168)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_5 (0xff00)
	unsigned int reg_isp_lsc_b_gain_table_5:8;

	// h0054
	unsigned int /* padding 16 bit */:16;

	// h0055, bit: 7
	/* Gain Table 6 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_6 (170)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_6 (0xff)
	unsigned int reg_isp_lsc_b_gain_table_6:8;

	// h0055, bit: 14
	/* Gain Table 7 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_7 (170)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_7 (0xff00)
	unsigned int reg_isp_lsc_b_gain_table_7:8;

	// h0055
	unsigned int /* padding 16 bit */:16;

	// h0056, bit: 7
	/* Gain Table 8 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_8 (172)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_8 (0xff)
	unsigned int reg_isp_lsc_b_gain_table_8:8;

	// h0056, bit: 14
	/* Gain Table 9 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_9 (172)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_9 (0xff00)
	unsigned int reg_isp_lsc_b_gain_table_9:8;

	// h0056
	unsigned int /* padding 16 bit */:16;

	// h0057, bit: 7
	/* Gain Table 10 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_10 (174)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_10 (0xff)
	unsigned int reg_isp_lsc_b_gain_table_10:8;

	// h0057, bit: 14
	/* Gain Table 11 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_11 (174)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_11 (0xff00)
	unsigned int reg_isp_lsc_b_gain_table_11:8;

	// h0057
	unsigned int /* padding 16 bit */:16;

	// h0058, bit: 7
	/* Gain Table 12 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_12 (176)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_12 (0xff)
	unsigned int reg_isp_lsc_b_gain_table_12:8;

	// h0058, bit: 14
	/* Gain Table 13 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_13 (176)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_13 (0xff00)
	unsigned int reg_isp_lsc_b_gain_table_13:8;

	// h0058
	unsigned int /* padding 16 bit */:16;

	// h0059, bit: 7
	/* Gain Table 14 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_14 (178)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_14 (0xff)
	unsigned int reg_isp_lsc_b_gain_table_14:8;

	// h0059, bit: 14
	/* Gain Table 15 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_15 (178)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_15 (0xff00)
	unsigned int reg_isp_lsc_b_gain_table_15:8;

	// h0059
	unsigned int /* padding 16 bit */:16;

	// h005a, bit: 7
	/* Gain Table 16 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_16 (180)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_16 (0xff)
	unsigned int reg_isp_lsc_b_gain_table_16:8;

	// h005a, bit: 14
	/* Gain Table 17 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_17 (180)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_17 (0xff00)
	unsigned int reg_isp_lsc_b_gain_table_17:8;

	// h005a
	unsigned int /* padding 16 bit */:16;

	// h005b, bit: 7
	/* Gain Table 18 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_18 (182)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_18 (0xff)
	unsigned int reg_isp_lsc_b_gain_table_18:8;

	// h005b, bit: 14
	/* Gain Table 19 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_19 (182)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_19 (0xff00)
	unsigned int reg_isp_lsc_b_gain_table_19:8;

	// h005b
	unsigned int /* padding 16 bit */:16;

	// h005c, bit: 7
	/* Gain Table 20 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_20 (184)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_20 (0xff)
	unsigned int reg_isp_lsc_b_gain_table_20:8;

	// h005c, bit: 14
	/* Gain Table 21 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_21 (184)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_21 (0xff00)
	unsigned int reg_isp_lsc_b_gain_table_21:8;

	// h005c
	unsigned int /* padding 16 bit */:16;

	// h005d, bit: 7
	/* Gain Table 22 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_22 (186)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_22 (0xff)
	unsigned int reg_isp_lsc_b_gain_table_22:8;

	// h005d, bit: 14
	/* Gain Table 23 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_23 (186)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_23 (0xff00)
	unsigned int reg_isp_lsc_b_gain_table_23:8;

	// h005d
	unsigned int /* padding 16 bit */:16;

	// h005e, bit: 7
	/* Gain Table 24 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_24 (188)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_24 (0xff)
	unsigned int reg_isp_lsc_b_gain_table_24:8;

	// h005e, bit: 14
	/* Gain Table 25 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_25 (188)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_25 (0xff00)
	unsigned int reg_isp_lsc_b_gain_table_25:8;

	// h005e
	unsigned int /* padding 16 bit */:16;

	// h005f, bit: 7
	/* Gain Table 26 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_26 (190)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_26 (0xff)
	unsigned int reg_isp_lsc_b_gain_table_26:8;

	// h005f, bit: 14
	/* Gain Table 27 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_27 (190)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_27 (0xff00)
	unsigned int reg_isp_lsc_b_gain_table_27:8;

	// h005f
	unsigned int /* padding 16 bit */:16;

	// h0060, bit: 7
	/* Gain Table 28 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_28 (192)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_28 (0xff)
	unsigned int reg_isp_lsc_b_gain_table_28:8;

	// h0060, bit: 14
	/* Gain Table 29 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_29 (192)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_29 (0xff00)
	unsigned int reg_isp_lsc_b_gain_table_29:8;

	// h0060
	unsigned int /* padding 16 bit */:16;

	// h0061, bit: 7
	/* Gain Table 30 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_30 (194)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_30 (0xff)
	unsigned int reg_isp_lsc_b_gain_table_30:8;

	// h0061, bit: 14
	/* Gain Table 31 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_lsc_b_gain_table_31 (194)
	#define mask_of_isp11_reg_isp_lsc_b_gain_table_31 (0xff00)
	unsigned int reg_isp_lsc_b_gain_table_31:8;

	// h0061
	unsigned int /* padding 16 bit */:16;

	// h0062, bit: 0
	/* ALSC enable*/
	#define offset_of_isp11_reg_en_alsc (196)
	#define mask_of_isp11_reg_en_alsc (0x1)
	unsigned int reg_en_alsc:1;

	// h0062, bit: 1
	/* */
	unsigned int :1;

	// h0062, bit: 2
	/* ALSC dither enable*/
	#define offset_of_isp11_reg_alsc_dither_en (196)
	#define mask_of_isp11_reg_alsc_dither_en (0x4)
	unsigned int reg_alsc_dither_en:1;

	// h0062, bit: 3
	/* */
	unsigned int :1;

	// h0062, bit: 5
	/* ALSC x block mode :  (blk size)
	0: 8
	1: 16
	2: 32
	3: 64*/
	#define offset_of_isp11_reg_alsc_x_blk_mode (196)
	#define mask_of_isp11_reg_alsc_x_blk_mode (0x30)
	unsigned int reg_alsc_x_blk_mode:2;

	// h0062, bit: 7
	/* ALSC y block mode :  (blk size)
	0: 8
	1: 16
	2: 32
	3: 64*/
	#define offset_of_isp11_reg_alsc_y_blk_mode (196)
	#define mask_of_isp11_reg_alsc_y_blk_mode (0xc0)
	unsigned int reg_alsc_y_blk_mode:2;

	// h0062, bit: 10
	/* ALSC debug mode enable ([0]:R; [1]:G;[2]:B)*/
	#define offset_of_isp11_reg_alsc_dbg_en (196)
	#define mask_of_isp11_reg_alsc_dbg_en (0x700)
	unsigned int reg_alsc_dbg_en:3;

	// h0062, bit: 12
	/* ALSC debug gain right shift*/
	#define offset_of_isp11_reg_alsc_dbg_shift (196)
	#define mask_of_isp11_reg_alsc_dbg_shift (0x1800)
	unsigned int reg_alsc_dbg_shift:2;

	// h0062, bit: 14
	/* */
	unsigned int :3;

	// h0062
	unsigned int /* padding 16 bit */:16;

	// h0063, bit: 12
	/* ALSC horizontal offset*/
	#define offset_of_isp11_reg_alsc_offsetx (198)
	#define mask_of_isp11_reg_alsc_offsetx (0x1fff)
	unsigned int reg_alsc_offsetx:13;

	// h0063, bit: 14
	/* */
	unsigned int :3;

	// h0063
	unsigned int /* padding 16 bit */:16;

	// h0064, bit: 12
	/* ALSC vertical offset*/
	#define offset_of_isp11_reg_alsc_offsety (200)
	#define mask_of_isp11_reg_alsc_offsety (0x1fff)
	unsigned int reg_alsc_offsety:13;

	// h0064, bit: 14
	/* */
	unsigned int :3;

	// h0064
	unsigned int /* padding 16 bit */:16;

	// h0065, bit: 7
	/* ALSC gain table width minus one*/
	#define offset_of_isp11_reg_alsc_gtbl_width_m1 (202)
	#define mask_of_isp11_reg_alsc_gtbl_width_m1 (0xff)
	unsigned int reg_alsc_gtbl_width_m1:8;

	// h0065, bit: 14
	/* ALSC gain table height minus one*/
	#define offset_of_isp11_reg_alsc_gtbl_height_m1 (202)
	#define mask_of_isp11_reg_alsc_gtbl_height_m1 (0xff00)
	unsigned int reg_alsc_gtbl_height_m1:8;

	// h0065
	unsigned int /* padding 16 bit */:16;

	// h0066, bit: 7
	/* ALSC gain table pitch*/
	#define offset_of_isp11_reg_alsc_gtbl_pitch (204)
	#define mask_of_isp11_reg_alsc_gtbl_pitch (0xff)
	unsigned int reg_alsc_gtbl_pitch:8;

	// h0066, bit: 14
	/* */
	unsigned int :8;

	// h0066
	unsigned int /* padding 16 bit */:16;

	// h0067, bit: 14
	/* ALSC interpolation coefficients*/
	#define offset_of_isp11_reg_coef_buf_wd (206)
	#define mask_of_isp11_reg_coef_buf_wd (0xffff)
	unsigned int reg_coef_buf_wd:16;

	// h0067
	unsigned int /* padding 16 bit */:16;

	// h0068, bit: 14
	/* ALSC interpolation coefficients*/
	#define offset_of_isp11_reg_coef_buf_wd_1 (208)
	#define mask_of_isp11_reg_coef_buf_wd_1 (0xffff)
	unsigned int reg_coef_buf_wd_1:16;

	// h0068
	unsigned int /* padding 16 bit */:16;

	// h0069, bit: 5
	/* ALSC coefficient  table address
	H : 0~32
	V : 0~32*/
	#define offset_of_isp11_reg_coef_buf_adr (210)
	#define mask_of_isp11_reg_coef_buf_adr (0x3f)
	unsigned int reg_coef_buf_adr:6;

	// h0069, bit: 6
	/* */
	unsigned int :1;

	// h0069, bit: 7
	/* ALSC coefficient  table select :
	0 : Horizontal
	1 : Vertical*/
	#define offset_of_isp11_reg_coef_buf_sel_v (210)
	#define mask_of_isp11_reg_coef_buf_sel_v (0x80)
	unsigned int reg_coef_buf_sel_v:1;

	// h0069, bit: 8
	/* ALSC coefficient  table write*/
	#define offset_of_isp11_reg_coef_buf_we (210)
	#define mask_of_isp11_reg_coef_buf_we (0x100)
	unsigned int reg_coef_buf_we:1;

	// h0069, bit: 9
	/* ALSC coefficient  table read*/
	#define offset_of_isp11_reg_coef_buf_re (210)
	#define mask_of_isp11_reg_coef_buf_re (0x200)
	unsigned int reg_coef_buf_re:1;

	// h0069, bit: 14
	/* */
	unsigned int :6;

	// h0069
	unsigned int /* padding 16 bit */:16;

	// h006a, bit: 14
	/* ALSC interpolation coefficients*/
	#define offset_of_isp11_reg_coef_buf_rd (212)
	#define mask_of_isp11_reg_coef_buf_rd (0xffff)
	unsigned int reg_coef_buf_rd:16;

	// h006a
	unsigned int /* padding 16 bit */:16;

	// h006b, bit: 14
	/* ALSC interpolation coefficients*/
	#define offset_of_isp11_reg_coef_buf_rd_1 (214)
	#define mask_of_isp11_reg_coef_buf_rd_1 (0xffff)
	unsigned int reg_coef_buf_rd_1:16;

	// h006b
	unsigned int /* padding 16 bit */:16;

	// h006c, bit: 2
	/* ALSC table maping
	3'b000: no rotation
	3'b001: clockwise 90
	3'b010: clockwise 180
	3'b011: clockwise 270
	3'b100: horizontal mirror
	3'b101: vertical mirror*/
	#define offset_of_isp11_reg_asymlsc_orientation (216)
	#define mask_of_isp11_reg_asymlsc_orientation (0x7)
	unsigned int reg_asymlsc_orientation:3;

	// h006c, bit: 14
	/* */
	unsigned int :13;

	// h006c
	unsigned int /* padding 16 bit */:16;

	// h006d, bit: 11
	/* ALSC table scalar (input frame height mapping  to 1920)*/
	#define offset_of_isp11_reg_asymlsc_scale_x (218)
	#define mask_of_isp11_reg_asymlsc_scale_x (0xfff)
	unsigned int reg_asymlsc_scale_x:12;

	// h006d, bit: 14
	/* */
	unsigned int :4;

	// h006d
	unsigned int /* padding 16 bit */:16;

	// h006e, bit: 11
	/* ALSC table scalar (input frame height mapping  to 1080)*/
	#define offset_of_isp11_reg_asymlsc_scale_y (220)
	#define mask_of_isp11_reg_asymlsc_scale_y (0xfff)
	unsigned int reg_asymlsc_scale_y:12;

	// h006e, bit: 14
	/* */
	unsigned int :4;

	// h006e
	unsigned int /* padding 16 bit */:16;

	// h006f, bit: 0
	/* Enable ISP Engine
	# 1’b0: Disable and Reset to the Initial State
	# 1’b1: Enable
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_en (222)
	#define mask_of_isp11_reg_isp_en (0x1)
	unsigned int reg_isp_en:1;

	// h006f, bit: 1
	/* Sensor Input Format
	# 1'b0: YUV 422 format
	# 1'b1: RGB pattern
	 ( single buffer register )*/
	#define offset_of_isp11_reg_isp_sensor_rgb_in (222)
	#define mask_of_isp11_reg_isp_sensor_rgb_in (0x2)
	unsigned int reg_isp_sensor_rgb_in:1;

	// h006f, bit: 2
	/* Sensor Input Format
	# 1'b0: separate Y/C mode
	# 1'b1: YC 16bit mode*/
	#define offset_of_isp11_reg_isp_sensor_yc16bit (222)
	#define mask_of_isp11_reg_isp_sensor_yc16bit (0x4)
	unsigned int reg_isp_sensor_yc16bit:1;

	// h006f, bit: 4
	/* RGB Raw Data Organization
	# 2'h0: [R G R G ...; G B G B ...]
	# 2'h1: [G R G R ...; B G B G ...]
	# 2'h2: [B G B G ...; G R G R ...]
	# 2'h3: [G B G B ...; R G R G ...]
	 ( double buffer register )*/
	#define offset_of_isp11_reg_sensor_array (222)
	#define mask_of_isp11_reg_sensor_array (0x18)
	unsigned int reg_sensor_array:2;

	// h006f, bit: 5
	/* isp_dp source mux
	# 1'b0: from rmux
	# 1'b1: from sensor freerun vs*/
	#define offset_of_isp11_reg_isp_if_rvs_sel (222)
	#define mask_of_isp11_reg_isp_if_rvs_sel (0x20)
	unsigned int reg_isp_if_rvs_sel:1;

	// h006f, bit: 6
	/* Sensor Input Priority as YUV input
	# 1'b0: [ C Y C Y ...]
	# 1'b1: [ Y C Y C ...]
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_sensor_yuv_order (222)
	#define mask_of_isp11_reg_isp_sensor_yuv_order (0x40)
	unsigned int reg_isp_sensor_yuv_order:1;

	// h006f, bit: 7
	/* isp_dp source mux
	# 1'b0: from sensor_if
	# 1'b1: from RDMA*/
	#define offset_of_isp11_reg_isp_if_rmux_sel (222)
	#define mask_of_isp11_reg_isp_if_rmux_sel (0x80)
	unsigned int reg_isp_if_rmux_sel:1;

	// h006f, bit: 10
	/* WDMA source mux
	# 3'd0: from sensor_if
	# 3'd1: from isp_dp output
	# 3'd2: from isp_dp down-sample
	# 3'd3: from isp_dp output
	# 3'd4: from video stabilization
	# 3'd7: from RGB-IR I data*/
	#define offset_of_isp11_reg_isp_if_wmux_sel (222)
	#define mask_of_isp11_reg_isp_if_wmux_sel (0x700)
	unsigned int reg_isp_if_wmux_sel:3;

	// h006f, bit: 12
	/* Sensor Format
	# 2'b00: 8 bit
	# 2'b01: 10bit
	# 2'b10: 16bit
	# 2'b11: 12bit*/
	#define offset_of_isp11_reg_isp_sensor_format (222)
	#define mask_of_isp11_reg_isp_sensor_format (0x1800)
	unsigned int reg_isp_sensor_format:2;

	// h006f, bit: 13
	/* Always Load Double Buffer Register (Type 2 )
	 ( single buffer register )*/
	#define offset_of_isp11_reg_load_reg (222)
	#define mask_of_isp11_reg_load_reg (0x2000)
	unsigned int reg_load_reg:1;

	// h006f, bit: 14
	/* double buffer register set done at batch mode*/
	#define offset_of_isp11_reg_db_batch_done (222)
	#define mask_of_isp11_reg_db_batch_done (0x4000)
	unsigned int reg_db_batch_done:1;

	// h006f, bit: 15
	/* double buffer register batch mode*/
	#define offset_of_isp11_reg_db_batch_mode (222)
	#define mask_of_isp11_reg_db_batch_mode (0x8000)
	unsigned int reg_db_batch_mode:1;

	// h006f
	unsigned int /* padding 16 bit */:16;

	// h0070, bit: 0
	/* Ready only mode*/
	#define offset_of_isp11_reg_isp_if_de_mode (224)
	#define mask_of_isp11_reg_isp_if_de_mode (0x1)
	unsigned int reg_isp_if_de_mode:1;

	// h0070, bit: 1
	/* ISP FIFO handshaking mode enable*/
	#define offset_of_isp11_reg_isp_if_hs_mode_en (224)
	#define mask_of_isp11_reg_isp_if_hs_mode_en (0x2)
	unsigned int reg_isp_if_hs_mode_en:1;

	// h0070, bit: 2
	/* Raw data down sample to DMA enable
	(double buffer register)*/
	#define offset_of_isp11_reg_dspl2wdma_en (224)
	#define mask_of_isp11_reg_dspl2wdma_en (0x4)
	unsigned int reg_dspl2wdma_en:1;

	// h0070, bit: 5
	/* down-sampling mode
	0 : whole image
	1 : 1/4
	2 : 1/16
	3 : 1/64
	4 : 1/256*/
	#define offset_of_isp11_reg_dsp_mode (224)
	#define mask_of_isp11_reg_dsp_mode (0x38)
	unsigned int reg_dsp_mode:3;

	// h0070, bit: 6
	/* 0: vsync double buffer
	1: frame end double buffer*/
	#define offset_of_isp11_reg_latch_mode (224)
	#define mask_of_isp11_reg_latch_mode (0x40)
	unsigned int reg_latch_mode:1;

	// h0070, bit: 7
	/* 0: isp_dp reset by vsync
	1: isp_dp not reset by vsync*/
	#define offset_of_isp11_reg_isp_dp_rstz_mode (224)
	#define mask_of_isp11_reg_isp_dp_rstz_mode (0x80)
	unsigned int reg_isp_dp_rstz_mode:1;

	// h0070, bit: 8
	/* 0: Enable isp2icp frame end reset
	1: Disable*/
	#define offset_of_isp11_reg_fend_rstz_mode (224)
	#define mask_of_isp11_reg_fend_rstz_mode (0x100)
	unsigned int reg_fend_rstz_mode:1;

	// h0070, bit: 9
	/* isp to dma data pack at msb*/
	#define offset_of_isp11_reg_isp_dma_pk_msb (224)
	#define mask_of_isp11_reg_isp_dma_pk_msb (0x200)
	unsigned int reg_isp_dma_pk_msb:1;

	// h0070, bit: 10
	/* Ready only mode*/
	#define offset_of_isp11_reg_isp_if_de_mode_g11 (224)
	#define mask_of_isp11_reg_isp_if_de_mode_g11 (0x400)
	unsigned int reg_isp_if_de_mode_g11:1;

	// h0070, bit: 11
	/* Ready only mode*/
	#define offset_of_isp11_reg_isp_if_de_mode_g12 (224)
	#define mask_of_isp11_reg_isp_if_de_mode_g12 (0x800)
	unsigned int reg_isp_if_de_mode_g12:1;

	// h0070, bit: 12
	/* Ready only mode*/
	#define offset_of_isp11_reg_isp_if_de_mode_g13 (224)
	#define mask_of_isp11_reg_isp_if_de_mode_g13 (0x1000)
	unsigned int reg_isp_if_de_mode_g13:1;

	// h0070, bit: 14
	/* fifo mode select
	0: 1024*4
	1: 2048*2
	2: 4096*1*/
	#define offset_of_isp11_reg_isp_fifo_mode_sel (224)
	#define mask_of_isp11_reg_isp_fifo_mode_sel (0x6000)
	unsigned int reg_isp_fifo_mode_sel:2;

	// h0070, bit: 15
	/* 422to420 data store enable*/
	#define offset_of_isp11_reg_42to40_store_en (224)
	#define mask_of_isp11_reg_42to40_store_en (0x8000)
	unsigned int reg_42to40_store_en:1;

	// h0070
	unsigned int /* padding 16 bit */:16;

	// h0071, bit: 3
	/* [2] b3_atv_fix*/
	#define offset_of_isp11_reg_fire_mode (226)
	#define mask_of_isp11_reg_fire_mode (0xf)
	unsigned int reg_fire_mode:4;

	// h0071, bit: 14
	/* */
	unsigned int :12;

	// h0071
	unsigned int /* padding 16 bit */:16;

	// h0072, bit: 12
	/* X Start Position for Crop Window
	If Sensor is YUV input, this setting must be even
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_crop_start_x (228)
	#define mask_of_isp11_reg_isp_crop_start_x (0x1fff)
	unsigned int reg_isp_crop_start_x:13;

	// h0072, bit: 14
	/* */
	unsigned int :3;

	// h0072
	unsigned int /* padding 16 bit */:16;

	// h0073, bit: 12
	/* Y Start Position for Crop Window
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_crop_start_y (230)
	#define mask_of_isp11_reg_isp_crop_start_y (0x1fff)
	unsigned int reg_isp_crop_start_y:13;

	// h0073, bit: 14
	/* */
	unsigned int :3;

	// h0073
	unsigned int /* padding 16 bit */:16;

	// h0074, bit: 12
	/* Crop Window Width
	If Sensor is YUV input, this setting must be odd (0 base)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_crop_width (232)
	#define mask_of_isp11_reg_isp_crop_width (0x1fff)
	unsigned int reg_isp_crop_width:13;

	// h0074, bit: 14
	/* */
	unsigned int :3;

	// h0074
	unsigned int /* padding 16 bit */:16;

	// h0075, bit: 12
	/* Crop Window High
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_crop_high (234)
	#define mask_of_isp11_reg_isp_crop_high (0x1fff)
	unsigned int reg_isp_crop_high:13;

	// h0075, bit: 14
	/* */
	unsigned int :3;

	// h0075
	unsigned int /* padding 16 bit */:16;

	// h0076, bit: 12
	/* WDMA X Start Position for Crop Window
	If Sensor is YUV input, this setting must be even
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wdma_crop_start_x (236)
	#define mask_of_isp11_reg_isp_wdma_crop_start_x (0x1fff)
	unsigned int reg_isp_wdma_crop_start_x:13;

	// h0076, bit: 14
	/* */
	unsigned int :3;

	// h0076
	unsigned int /* padding 16 bit */:16;

	// h0077, bit: 12
	/* WDMA Y Start Position for Crop Window
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wdma_crop_start_y (238)
	#define mask_of_isp11_reg_isp_wdma_crop_start_y (0x1fff)
	unsigned int reg_isp_wdma_crop_start_y:13;

	// h0077, bit: 14
	/* */
	unsigned int :3;

	// h0077
	unsigned int /* padding 16 bit */:16;

	// h0078, bit: 14
	/* WDMA Crop Window Width
	If Sensor is YUV input, this setting must be odd (0 base)
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wdma_crop_width (240)
	#define mask_of_isp11_reg_isp_wdma_crop_width (0xffff)
	unsigned int reg_isp_wdma_crop_width:16;

	// h0078
	unsigned int /* padding 16 bit */:16;

	// h0079, bit: 12
	/* WDMA Crop Window High
	 ( double buffer register )*/
	#define offset_of_isp11_reg_isp_wdma_crop_high (242)
	#define mask_of_isp11_reg_isp_wdma_crop_high (0x1fff)
	unsigned int reg_isp_wdma_crop_high:13;

	// h0079, bit: 14
	/* */
	unsigned int :3;

	// h0079
	unsigned int /* padding 16 bit */:16;

	// h007a, bit: 12
	/* Sensor Line Counter*/
	#define offset_of_isp11_reg_isp_sensor_line_cnt (244)
	#define mask_of_isp11_reg_isp_sensor_line_cnt (0x1fff)
	unsigned int reg_isp_sensor_line_cnt:13;

	// h007a, bit: 14
	/* */
	unsigned int :3;

	// h007a
	unsigned int /* padding 16 bit */:16;

	// h007b, bit: 3
	/* Hardware Debug Signal Sets Selector*/
	#define offset_of_isp11_reg_isp_debug_sel (246)
	#define mask_of_isp11_reg_isp_debug_sel (0xf)
	unsigned int reg_isp_debug_sel:4;

	// h007b, bit: 14
	/* */
	unsigned int :12;

	// h007b
	unsigned int /* padding 16 bit */:16;

	// h007c, bit: 14
	/* isp debug out*/
	#define offset_of_isp11_reg_isp_debug_out (248)
	#define mask_of_isp11_reg_isp_debug_out (0xffff)
	unsigned int reg_isp_debug_out:16;

	// h007c
	unsigned int /* padding 16 bit */:16;

	// h007d, bit: 12
	/* SW specify interrupt line number*/
	#define offset_of_isp11_reg_sw_specify_int_line (250)
	#define mask_of_isp11_reg_sw_specify_int_line (0x1fff)
	unsigned int reg_sw_specify_int_line:13;

	// h007d, bit: 14
	/* */
	unsigned int :3;

	// h007d
	unsigned int /* padding 16 bit */:16;

	// h007e, bit: 12
	/* Sensor to Crop line count*/
	#define offset_of_isp11_reg_isp_sr2crop_line_cnt (252)
	#define mask_of_isp11_reg_isp_sr2crop_line_cnt (0x1fff)
	unsigned int reg_isp_sr2crop_line_cnt:13;

	// h007e, bit: 14
	/* */
	unsigned int :3;

	// h007e
	unsigned int /* padding 16 bit */:16;

	// h007f, bit: 14
	/* */
	unsigned int :16;

	// h007f
	unsigned int /* padding 16 bit */:16;

}  __attribute__((packed, aligned(1))) infinity2_reg_isp11;
#endif
