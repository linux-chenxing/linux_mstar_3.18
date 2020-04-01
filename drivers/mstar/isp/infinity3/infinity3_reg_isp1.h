// Generate Time: 2016-07-20 09:23:47.578000
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
#ifndef __INFINITY3_REG_ISP1__
#define __INFINITY3_REG_ISP1__
typedef struct {
	// h0000, bit: 14
	/* */
	unsigned int :16;

	// h0000
	unsigned int /* padding 16 bit */:16;

	// h0001, bit: 11
	/* X Center Position for Lens Shading Compensation
	 ( double buffer register )*/
	unsigned int reg_isp_lsc_center_x:12;

	// h0001, bit: 14
	/* */
	unsigned int :4;

	// h0001
	unsigned int /* padding 16 bit */:16;

	// h0002, bit: 11
	/* Y Center Position for Lens Shading Compensation
	 ( double buffer register )*/
	unsigned int reg_isp_lsc_center_y:12;

	// h0002, bit: 14
	/* */
	unsigned int :4;

	// h0002
	unsigned int /* padding 16 bit */:16;

	// h0003, bit: 4
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
	unsigned int reg_isp_lsc_shift:5;

	// h0003, bit: 14
	/* */
	unsigned int :11;

	// h0003
	unsigned int /* padding 16 bit */:16;

	// h0004, bit: 0
	/* obc dither enable*/
	unsigned int reg_obc_dith_en:1;

	// h0004, bit: 1
	/* lsc dither enable*/
	unsigned int reg_lsc_dith_en:1;

	// h0004, bit: 14
	/* */
	unsigned int :14;

	// h0004
	unsigned int /* padding 16 bit */:16;

	// h0005, bit: 14
	/* */
	unsigned int :16;

	// h0005
	unsigned int /* padding 16 bit */:16;

	// h0006, bit: 9
	/* Enable 12-bits to 10-bits Gamma at bayer domain*/
	unsigned int reg_gm12to10_max_data:10;

	// h0006, bit: 11
	/* */
	unsigned int :2;

	// h0006, bit: 12
	/* Enable 12-bits to 10-bits Gamma at bayer domain*/
	unsigned int reg_gm12to10_en:1;

	// h0006, bit: 13
	/* Enable 12-bits to 10-bits Gamma at bayer domain*/
	unsigned int reg_gm12to10_max_en:1;

	// h0006, bit: 14
	/* */
	unsigned int :2;

	// h0006
	unsigned int /* padding 16 bit */:16;

	// h0007, bit: 0
	/* White Balance Gain Dither Enable*/
	unsigned int reg_wbg_dith_en:1;

	// h0007, bit: 14
	/* */
	unsigned int :15;

	// h0007
	unsigned int /* padding 16 bit */:16;

	// h0008, bit: 12
	/* White Balance Gain for R channel (3.10)
	 ( double buffer register )*/
	unsigned int reg_isp_wbg_r_gain:13;

	// h0008, bit: 14
	/* */
	unsigned int :3;

	// h0008
	unsigned int /* padding 16 bit */:16;

	// h0009, bit: 10
	/* White Balance Offset for R channel (s10)
	 ( double buffer register )*/
	unsigned int reg_isp_wbg_r_offset:11;

	// h0009, bit: 14
	/* */
	unsigned int :5;

	// h0009
	unsigned int /* padding 16 bit */:16;

	// h000a, bit: 12
	/* White Balance Gain for Gr channel (3.10)
	 ( double buffer register )*/
	unsigned int reg_isp_wbg_gr_gain:13;

	// h000a, bit: 14
	/* */
	unsigned int :3;

	// h000a
	unsigned int /* padding 16 bit */:16;

	// h000b, bit: 10
	/* White Balance Offset for Gr channel (s10)
	 ( double buffer register )*/
	unsigned int reg_isp_wbg_gr_offset:11;

	// h000b, bit: 14
	/* */
	unsigned int :5;

	// h000b
	unsigned int /* padding 16 bit */:16;

	// h000c, bit: 12
	/* White Balance Gain for B channel (3.10)
	 ( double buffer register )*/
	unsigned int reg_isp_wbg_b_gain:13;

	// h000c, bit: 14
	/* */
	unsigned int :3;

	// h000c
	unsigned int /* padding 16 bit */:16;

	// h000d, bit: 10
	/* White Balance Offset for B channel (s10)
	 ( double buffer register )*/
	unsigned int reg_isp_wbg_b_offset:11;

	// h000d, bit: 14
	/* */
	unsigned int :5;

	// h000d
	unsigned int /* padding 16 bit */:16;

	// h000e, bit: 12
	/* White Balance Gain for Gb channel (3.10)
	 ( double buffer register )*/
	unsigned int reg_isp_wbg_gb_gain:13;

	// h000e, bit: 14
	/* */
	unsigned int :3;

	// h000e
	unsigned int /* padding 16 bit */:16;

	// h000f, bit: 10
	/* White Balance Offset for Gb channel (s10)
	 ( double buffer register )*/
	unsigned int reg_isp_wbg_gb_offset:11;

	// h000f, bit: 14
	/* */
	unsigned int :5;

	// h000f
	unsigned int /* padding 16 bit */:16;

	// h0010, bit: 7
	/* Gain Table 0 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_0:8;

	// h0010, bit: 14
	/* Gain Table 1 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_1:8;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 7
	/* Gain Table 2 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_2:8;

	// h0011, bit: 14
	/* Gain Table 3 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_3:8;

	// h0011
	unsigned int /* padding 16 bit */:16;

	// h0012, bit: 7
	/* Gain Table 4 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_4:8;

	// h0012, bit: 14
	/* Gain Table 5 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_5:8;

	// h0012
	unsigned int /* padding 16 bit */:16;

	// h0013, bit: 7
	/* Gain Table 6 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_6:8;

	// h0013, bit: 14
	/* Gain Table 7 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_7:8;

	// h0013
	unsigned int /* padding 16 bit */:16;

	// h0014, bit: 7
	/* Gain Table 8 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_8:8;

	// h0014, bit: 14
	/* Gain Table 9 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_9:8;

	// h0014
	unsigned int /* padding 16 bit */:16;

	// h0015, bit: 7
	/* Gain Table 10 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_10:8;

	// h0015, bit: 14
	/* Gain Table 11 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_11:8;

	// h0015
	unsigned int /* padding 16 bit */:16;

	// h0016, bit: 7
	/* Gain Table 12 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:0]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_12:8;

	// h0016, bit: 14
	/* Gain Table 13 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_13:8;

	// h0016
	unsigned int /* padding 16 bit */:16;

	// h0017, bit: 7
	/* Gain Table 14 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_14:8;

	// h0017, bit: 14
	/* Gain Table 15 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_15:8;

	// h0017
	unsigned int /* padding 16 bit */:16;

	// h0018, bit: 7
	/* Gain Table 16 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_16:8;

	// h0018, bit: 14
	/* Gain Table 17 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_17:8;

	// h0018
	unsigned int /* padding 16 bit */:16;

	// h0019, bit: 7
	/* Gain Table 18 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_18:8;

	// h0019, bit: 14
	/* Gain Table 19 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_19:8;

	// h0019
	unsigned int /* padding 16 bit */:16;

	// h001a, bit: 7
	/* Gain Table 20 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_20:8;

	// h001a, bit: 14
	/* Gain Table 21 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_21:8;

	// h001a
	unsigned int /* padding 16 bit */:16;

	// h001b, bit: 7
	/* Gain Table 22 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_22:8;

	// h001b, bit: 14
	/* Gain Table 23 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_23:8;

	// h001b
	unsigned int /* padding 16 bit */:16;

	// h001c, bit: 7
	/* Gain Table 24 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_24:8;

	// h001c, bit: 14
	/* Gain Table 25 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_25:8;

	// h001c
	unsigned int /* padding 16 bit */:16;

	// h001d, bit: 7
	/* Gain Table 26 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_26:8;

	// h001d, bit: 14
	/* Gain Table 27 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_27:8;

	// h001d
	unsigned int /* padding 16 bit */:16;

	// h001e, bit: 7
	/* Gain Table 28 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_28:8;

	// h001e, bit: 14
	/* Gain Table 29 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_29:8;

	// h001e
	unsigned int /* padding 16 bit */:16;

	// h001f, bit: 7
	/* Gain Table 30 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_30:8;

	// h001f, bit: 14
	/* Gain Table 31 for R Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_r_gain_table_31:8;

	// h001f
	unsigned int /* padding 16 bit */:16;

	// h0020, bit: 7
	/* Gain Table 0 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_0:8;

	// h0020, bit: 14
	/* Gain Table 1 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_1:8;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 7
	/* Gain Table 2 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_2:8;

	// h0021, bit: 14
	/* Gain Table 3 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_3:8;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 7
	/* Gain Table 4 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_4:8;

	// h0022, bit: 14
	/* Gain Table 5 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_5:8;

	// h0022
	unsigned int /* padding 16 bit */:16;

	// h0023, bit: 7
	/* Gain Table 6 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_6:8;

	// h0023, bit: 14
	/* Gain Table 7 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_7:8;

	// h0023
	unsigned int /* padding 16 bit */:16;

	// h0024, bit: 7
	/* Gain Table 8 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_8:8;

	// h0024, bit: 14
	/* Gain Table 9 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_9:8;

	// h0024
	unsigned int /* padding 16 bit */:16;

	// h0025, bit: 7
	/* Gain Table 10 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_10:8;

	// h0025, bit: 14
	/* Gain Table 11 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_11:8;

	// h0025
	unsigned int /* padding 16 bit */:16;

	// h0026, bit: 7
	/* Gain Table 12 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_12:8;

	// h0026, bit: 14
	/* Gain Table 13 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_13:8;

	// h0026
	unsigned int /* padding 16 bit */:16;

	// h0027, bit: 7
	/* Gain Table 14 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_14:8;

	// h0027, bit: 14
	/* Gain Table 15 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_15:8;

	// h0027
	unsigned int /* padding 16 bit */:16;

	// h0028, bit: 7
	/* Gain Table 16 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_16:8;

	// h0028, bit: 14
	/* Gain Table 17 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_17:8;

	// h0028
	unsigned int /* padding 16 bit */:16;

	// h0029, bit: 7
	/* Gain Table 18 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_18:8;

	// h0029, bit: 14
	/* Gain Table 19 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_19:8;

	// h0029
	unsigned int /* padding 16 bit */:16;

	// h002a, bit: 7
	/* Gain Table 20 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_20:8;

	// h002a, bit: 14
	/* Gain Table 21 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_21:8;

	// h002a
	unsigned int /* padding 16 bit */:16;

	// h002b, bit: 7
	/* Gain Table 22 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_22:8;

	// h002b, bit: 14
	/* Gain Table 23 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_23:8;

	// h002b
	unsigned int /* padding 16 bit */:16;

	// h002c, bit: 7
	/* Gain Table 24 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_24:8;

	// h002c, bit: 14
	/* Gain Table 25 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_25:8;

	// h002c
	unsigned int /* padding 16 bit */:16;

	// h002d, bit: 7
	/* Gain Table 26 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_26:8;

	// h002d, bit: 14
	/* Gain Table 27 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_27:8;

	// h002d
	unsigned int /* padding 16 bit */:16;

	// h002e, bit: 7
	/* Gain Table 28 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_28:8;

	// h002e, bit: 14
	/* Gain Table 29 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_29:8;

	// h002e
	unsigned int /* padding 16 bit */:16;

	// h002f, bit: 7
	/* Gain Table 30 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_30:8;

	// h002f, bit: 14
	/* Gain Table 31 for G Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_g_gain_table_31:8;

	// h002f
	unsigned int /* padding 16 bit */:16;

	// h0030, bit: 7
	/* Gain Table 0 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_0:8;

	// h0030, bit: 14
	/* Gain Table 1 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_1:8;

	// h0030
	unsigned int /* padding 16 bit */:16;

	// h0031, bit: 7
	/* Gain Table 2 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_2:8;

	// h0031, bit: 14
	/* Gain Table 3 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_3:8;

	// h0031
	unsigned int /* padding 16 bit */:16;

	// h0032, bit: 7
	/* Gain Table 4 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_4:8;

	// h0032, bit: 14
	/* Gain Table 5 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_5:8;

	// h0032
	unsigned int /* padding 16 bit */:16;

	// h0033, bit: 7
	/* Gain Table 6 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_6:8;

	// h0033, bit: 14
	/* Gain Table 7 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_7:8;

	// h0033
	unsigned int /* padding 16 bit */:16;

	// h0034, bit: 7
	/* Gain Table 8 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_8:8;

	// h0034, bit: 14
	/* Gain Table 9 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_9:8;

	// h0034
	unsigned int /* padding 16 bit */:16;

	// h0035, bit: 7
	/* Gain Table 10 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_10:8;

	// h0035, bit: 14
	/* Gain Table 11 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_11:8;

	// h0035
	unsigned int /* padding 16 bit */:16;

	// h0036, bit: 7
	/* Gain Table 12 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_12:8;

	// h0036, bit: 14
	/* Gain Table 13 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_13:8;

	// h0036
	unsigned int /* padding 16 bit */:16;

	// h0037, bit: 7
	/* Gain Table 14 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_14:8;

	// h0037, bit: 14
	/* Gain Table 15 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_15:8;

	// h0037
	unsigned int /* padding 16 bit */:16;

	// h0038, bit: 7
	/* Gain Table 16 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_16:8;

	// h0038, bit: 14
	/* Gain Table 17 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_17:8;

	// h0038
	unsigned int /* padding 16 bit */:16;

	// h0039, bit: 7
	/* Gain Table 18 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_18:8;

	// h0039, bit: 14
	/* Gain Table 19 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_19:8;

	// h0039
	unsigned int /* padding 16 bit */:16;

	// h003a, bit: 7
	/* Gain Table 20 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_20:8;

	// h003a, bit: 14
	/* Gain Table 21 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_21:8;

	// h003a
	unsigned int /* padding 16 bit */:16;

	// h003b, bit: 7
	/* Gain Table 22 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_22:8;

	// h003b, bit: 14
	/* Gain Table 23 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_23:8;

	// h003b
	unsigned int /* padding 16 bit */:16;

	// h003c, bit: 7
	/* Gain Table 24 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_24:8;

	// h003c, bit: 14
	/* Gain Table 25 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_25:8;

	// h003c
	unsigned int /* padding 16 bit */:16;

	// h003d, bit: 7
	/* Gain Table 26 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_26:8;

	// h003d, bit: 14
	/* Gain Table 27 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_27:8;

	// h003d
	unsigned int /* padding 16 bit */:16;

	// h003e, bit: 7
	/* Gain Table 28 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_28:8;

	// h003e, bit: 14
	/* Gain Table 29 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_29:8;

	// h003e
	unsigned int /* padding 16 bit */:16;

	// h003f, bit: 7
	/* Gain Table 30 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_30:8;

	// h003f, bit: 14
	/* Gain Table 31 for B Lens Shading Compensation
	# [7:0]   Interval Gain Base, (.8)
	# [15:8]  Interval Gain Slop (.8)
	 ( single buffer register )*/
	unsigned int reg_isp_lsc_b_gain_table_31:8;

	// h003f
	unsigned int /* padding 16 bit */:16;

	// h0040, bit: 0
	/* 1'b1 : Enable Green Equalization 1'b0 : disable (default)*/
	unsigned int reg_isp_geq_enable:1;

	// h0040, bit: 3
	/* */
	unsigned int :3;

	// h0040, bit: 13
	/* Green Equalization threshold*/
	unsigned int reg_isp_geq_th:10;

	// h0040, bit: 14
	/* */
	unsigned int :2;

	// h0040
	unsigned int /* padding 16 bit */:16;

	// h0041, bit: 9
	/* Green Equalization statistic threshold*/
	unsigned int reg_isp_geq_th_stat:10;

	// h0041, bit: 14
	/* */
	unsigned int :6;

	// h0041
	unsigned int /* padding 16 bit */:16;

	// h0042, bit: 14
	/* Green Equalization statistic max count*/
	unsigned int reg_o_isp_geq_max_count:16;

	// h0042
	unsigned int /* padding 16 bit */:16;

	// h0043, bit: 3
	/* Green Equalization statistic max group*/
	unsigned int reg_o_isp_geq_max_group:4;

	// h0043, bit: 14
	/* */
	unsigned int :12;

	// h0043
	unsigned int /* padding 16 bit */:16;

	// h0044, bit: 2
	/* Green Equalization absolute value shift (abs shift)*/
	unsigned int reg_isp_geq_abs_shift:3;

	// h0044, bit: 7
	/* */
	unsigned int :5;

	// h0044, bit: 10
	/* Green Equalization absolute value shift2 (abs shift)*/
	unsigned int reg_isp_geq_abs_shift2:3;

	// h0044, bit: 14
	/* */
	unsigned int :5;

	// h0044
	unsigned int /* padding 16 bit */:16;

	// h0045, bit: 9
	/* Green Equalization threshold2*/
	unsigned int reg_isp_geq_th2:10;

	// h0045, bit: 14
	/* */
	unsigned int :6;

	// h0045
	unsigned int /* padding 16 bit */:16;

	// h0046, bit: 9
	/* Green Equalization threshold max value*/
	unsigned int reg_isp_geq_th_max:10;

	// h0046, bit: 14
	/* */
	unsigned int :6;

	// h0046
	unsigned int /* padding 16 bit */:16;

	// h0047, bit: 0
	/* 1'b1 : Enable Crosstalk 1'b0 : disable (default)*/
	unsigned int reg_ci_anti_ct_en:1;

	// h0047, bit: 14
	/* */
	unsigned int :15;

	// h0047
	unsigned int /* padding 16 bit */:16;

	// h0048, bit: 7
	/* Crosstalk threshold*/
	unsigned int reg_ci_anti_ct_th:8;

	// h0048, bit: 14
	/* Crosstalk absolute value shift (abs shift)*/
	unsigned int reg_ci_anti_ct_sft:8;

	// h0048
	unsigned int /* padding 16 bit */:16;

	// h0049, bit: 0
	/* 1'b1 : Enable Green Equalization 1'b0 : disable (default)*/
	unsigned int reg_isp_geq_enable_2:1;

	// h0049, bit: 2
	/* Green Equalization absolute value shift (abs shift)*/
	unsigned int reg_isp_geq_abs_shift_2:2;

	// h0049, bit: 3
	/* */
	unsigned int :1;

	// h0049, bit: 11
	/* Green Equalization threshold*/
	unsigned int reg_isp_geq_th_2:8;

	// h0049, bit: 14
	/* */
	unsigned int :4;

	// h0049
	unsigned int /* padding 16 bit */:16;

	// h004a, bit: 7
	/* Green Equalization statistic threshold*/
	unsigned int reg_isp_geq_th_stat_2:8;

	// h004a, bit: 14
	/* */
	unsigned int :8;

	// h004a
	unsigned int /* padding 16 bit */:16;

	// h004b, bit: 14
	/* Green Equalization statistic max count*/
	unsigned int reg_o_isp_geq_max_count_2:16;

	// h004b
	unsigned int /* padding 16 bit */:16;

	// h004c, bit: 3
	/* Green Equalization statistic max group*/
	unsigned int reg_o_isp_geq_max_group_2:4;

	// h004c, bit: 14
	/* */
	unsigned int :12;

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

	// h0050, bit: 7
	/* SNR active threshold (R pixel)*/
	unsigned int reg_snr_std_low_thrd_r_pix:8;

	// h0050, bit: 14
	/* SNR active threshold (B pixel)*/
	unsigned int reg_snr_std_low_thrd_b_pix:8;

	// h0050
	unsigned int /* padding 16 bit */:16;

	// h0051, bit: 3
	/* SNR strength F2 (R pixel)*/
	unsigned int reg_snr_strength_gain_r_pix:4;

	// h0051, bit: 4
	/* */
	unsigned int :1;

	// h0051, bit: 7
	/* SNR alpha step (R pixel)*/
	unsigned int reg_snr_alpha_step_r_pix:3;

	// h0051, bit: 11
	/* SNR strength F2 (B pixel)*/
	unsigned int reg_snr_strength_gain_b_pix:4;

	// h0051, bit: 12
	/* */
	unsigned int :1;

	// h0051, bit: 14
	/* SNR alpha step (B pixel)*/
	unsigned int reg_snr_alpha_step_b_pix:3;

	// h0051
	unsigned int /* padding 16 bit */:16;

	// h0052, bit: 14
	/* SNR Alpha LUT (R pixel)*/
	unsigned int reg_snr_table_r_pix_0:16;

	// h0052
	unsigned int /* padding 16 bit */:16;

	// h0053, bit: 14
	/* SNR Alpha LUT (R pixel)*/
	unsigned int reg_snr_table_r_pix_1:16;

	// h0053
	unsigned int /* padding 16 bit */:16;

	// h0054, bit: 14
	/* SNR Alpha LUT (R pixel)*/
	unsigned int reg_snr_table_r_pix_2:16;

	// h0054
	unsigned int /* padding 16 bit */:16;

	// h0055, bit: 14
	/* SNR Alpha LUT (R pixel)*/
	unsigned int reg_snr_table_r_pix_3:16;

	// h0055
	unsigned int /* padding 16 bit */:16;

	// h0056, bit: 14
	/* SNR Alpha LUT (B pixel)*/
	unsigned int reg_snr_table_b_pix_0:16;

	// h0056
	unsigned int /* padding 16 bit */:16;

	// h0057, bit: 14
	/* SNR Alpha LUT (B pixel)*/
	unsigned int reg_snr_table_b_pix_1:16;

	// h0057
	unsigned int /* padding 16 bit */:16;

	// h0058, bit: 14
	/* SNR Alpha LUT (B pixel)*/
	unsigned int reg_snr_table_b_pix_2:16;

	// h0058
	unsigned int /* padding 16 bit */:16;

	// h0059, bit: 14
	/* SNR Alpha LUT (B pixel)*/
	unsigned int reg_snr_table_b_pix_3:16;

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

	// h0060, bit: 5
	/* Noise masking gain F2 (R pixel)*/
	unsigned int reg_snr_nm_gain_r_pix:6;

	// h0060, bit: 7
	/* */
	unsigned int :2;

	// h0060, bit: 13
	/* Noise masking gain F2 (B pixel)*/
	unsigned int reg_snr_nm_gain_b_pix:6;

	// h0060, bit: 14
	/* */
	unsigned int :2;

	// h0060
	unsigned int /* padding 16 bit */:16;

	// h0061, bit: 3
	/* Noise masking max threshold bound (R pixel)*/
	unsigned int reg_snr_nm_max_thrd_r_pix:4;

	// h0061, bit: 7
	/* Noise masking min threshold bound (R pixel)*/
	unsigned int reg_snr_nm_min_thrd_r_pix:4;

	// h0061, bit: 11
	/* Noise masking max threshold bound (B pixel)*/
	unsigned int reg_snr_nm_max_thrd_b_pix:4;

	// h0061, bit: 14
	/* Noise masking min threshold bound (B pixel)*/
	unsigned int reg_snr_nm_min_thrd_b_pix:4;

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

	// h0064, bit: 11
	/* obc line cnt number1 for interrupt*/
	unsigned int reg_isp_obc_line_cnt1:12;

	// h0064, bit: 14
	/* */
	unsigned int :4;

	// h0064
	unsigned int /* padding 16 bit */:16;

	// h0065, bit: 11
	/* obc line cnt number2 for interrupt*/
	unsigned int reg_isp_obc_line_cnt2:12;

	// h0065, bit: 14
	/* */
	unsigned int :4;

	// h0065
	unsigned int /* padding 16 bit */:16;

	// h0066, bit: 11
	/* obc line cnt number3 for interrupt*/
	unsigned int reg_isp_obc_line_cnt3:12;

	// h0066, bit: 14
	/* */
	unsigned int :4;

	// h0066
	unsigned int /* padding 16 bit */:16;

	// h0067, bit: 7
	/* Shift Base for Optical Black Correction
	input pixel is R,
	formula: Cout = (Cin – reg_isp_obc_r_ofst) * b
	 ( double buffer register )*/
	unsigned int reg_isp_obc_r_ofst:8;

	// h0067, bit: 14
	/* Shift Base for Optical Black Correction
	input pixel is GR,
	formula: Cout = (Cin – reg_isp_obc_gr_ofst) * b
	 ( double buffer register )*/
	unsigned int reg_isp_obc_gr_ofst:8;

	// h0067
	unsigned int /* padding 16 bit */:16;

	// h0068, bit: 7
	/* Global Gain Table for Lens Shading Compensation
	 ( double buffer register )*/
	unsigned int reg_isp_lsc_glb_gain:8;

	// h0068, bit: 14
	/* */
	unsigned int :8;

	// h0068
	unsigned int /* padding 16 bit */:16;

	// h0069, bit: 7
	/* Shift Base for Optical Black Correction
	input pixel is GB,
	formula: Cout = (Cin – reg_isp_obc_gb_ofst) * b
	 ( double buffer register )*/
	unsigned int reg_isp_obc_gb_ofst:8;

	// h0069, bit: 14
	/* Shift Base for Optical Black Correction
	input pixel is B,
	formula: Cout = (Cin – reg_isp_obc_b_ofst) * b
	 ( double buffer register )*/
	unsigned int reg_isp_obc_b_ofst:8;

	// h0069
	unsigned int /* padding 16 bit */:16;

	// h006a, bit: 13
	/* Shift Gain for Optical Black Correction
	formula: Cout = (Cin – ofst) * b
	input pixel is R,
	b = reg_isp_obc_r_gain
	precision 4.10
	 ( double buffer register )*/
	unsigned int reg_isp_obc_r_gain:14;

	// h006a, bit: 14
	/* */
	unsigned int :2;

	// h006a
	unsigned int /* padding 16 bit */:16;

	// h006b, bit: 13
	/* Shift Gain for Optical Black Correction
	formula: Cout = (Cin – ofst) * b
	input pixel is GR,
	b = reg_isp_obc_gr_gain
	precision 4.10
	 ( double buffer register )*/
	unsigned int reg_isp_obc_gr_gain:14;

	// h006b, bit: 14
	/* */
	unsigned int :2;

	// h006b
	unsigned int /* padding 16 bit */:16;

	// h006c, bit: 13
	/* Shift Gain for Optical Black Correction
	formula: Cout = (Cin – ofst) * b
	input pixel is GB,
	b = reg_isp_obc_gb_gain
	precision 4.10
	 ( double buffer register )*/
	unsigned int reg_isp_obc_gb_gain:14;

	// h006c, bit: 14
	/* */
	unsigned int :2;

	// h006c
	unsigned int /* padding 16 bit */:16;

	// h006d, bit: 13
	/* Shift Gain for Optical Black Correction
	formula: Cout = (Cin – ofst) * b
	input pixel is b,
	b = reg_isp_obc_b_gain
	precision 4.10
	 ( double buffer register )*/
	unsigned int reg_isp_obc_b_gain:14;

	// h006d, bit: 14
	/* */
	unsigned int :2;

	// h006d
	unsigned int /* padding 16 bit */:16;

	// h006e, bit: 0
	/* 1'b1: Enable AE gain
	1'b0: Disable(default)*/
	unsigned int reg_isp_ae_en:1;

	// h006e, bit: 14
	/* */
	unsigned int :15;

	// h006e
	unsigned int /* padding 16 bit */:16;

	// h006f, bit: 13
	/* AE gain
	out = in * reg_isp_ae_gain
	precision 4.10
	 ( double buffer register )*/
	unsigned int reg_isp_ae_gain:14;

	// h006f, bit: 14
	/* */
	unsigned int :2;

	// h006f
	unsigned int /* padding 16 bit */:16;

	// h0070, bit: 0
	/* SNR enable F2*/
	unsigned int reg_snr_en:1;

	// h0070, bit: 1
	/* mean filter R/G/B use separate param enable
	0: disable
	1: enable */
	unsigned int reg_mean_rgb_sep_en:1;

	// h0070, bit: 2
	/* noise masking R/G/B use separate param enable
	0: disable
	1: enable */
	unsigned int reg_nm_rgb_sep_en:1;

	// h0070, bit: 7
	/* */
	unsigned int :5;

	// h0070, bit: 14
	/* SNR active threshold*/
	unsigned int reg_snr_std_low_thrd:8;

	// h0070
	unsigned int /* padding 16 bit */:16;

	// h0071, bit: 3
	/* SNR strength F2*/
	unsigned int reg_snr_strength_gain:4;

	// h0071, bit: 4
	/* */
	unsigned int :1;

	// h0071, bit: 7
	/* SNR alpha step*/
	unsigned int reg_snr_alpha_step:3;

	// h0071, bit: 14
	/* */
	unsigned int :8;

	// h0071
	unsigned int /* padding 16 bit */:16;

	// h0072, bit: 14
	/* SNR Alpha LUT*/
	unsigned int reg_snr_table_0:16;

	// h0072
	unsigned int /* padding 16 bit */:16;

	// h0073, bit: 14
	/* SNR Alpha LUT*/
	unsigned int reg_snr_table_1:16;

	// h0073
	unsigned int /* padding 16 bit */:16;

	// h0074, bit: 14
	/* SNR Alpha LUT*/
	unsigned int reg_snr_table_2:16;

	// h0074
	unsigned int /* padding 16 bit */:16;

	// h0075, bit: 14
	/* SNR Alpha LUT*/
	unsigned int reg_snr_table_3:16;

	// h0075
	unsigned int /* padding 16 bit */:16;

	// h0076, bit: 0
	/* Noise masking enable F2*/
	unsigned int reg_snr_nm_filter_en:1;

	// h0076, bit: 14
	/* */
	unsigned int :15;

	// h0076
	unsigned int /* padding 16 bit */:16;

	// h0077, bit: 5
	/* Noise masking gain F2*/
	unsigned int reg_snr_nm_gain:6;

	// h0077, bit: 14
	/* */
	unsigned int :10;

	// h0077
	unsigned int /* padding 16 bit */:16;

	// h0078, bit: 3
	/* Noise masking max threshold bound*/
	unsigned int reg_snr_nm_max_thrd:4;

	// h0078, bit: 7
	/* Noise masking min threshold bound*/
	unsigned int reg_snr_nm_min_thrd:4;

	// h0078, bit: 14
	/* */
	unsigned int :8;

	// h0078
	unsigned int /* padding 16 bit */:16;

	// h0079, bit: 0
	/* Spike NR Enable F2.*/
	unsigned int reg_spike_nr_en:1;

	// h0079, bit: 7
	/* */
	unsigned int :7;

	// h0079, bit: 11
	/* Spike NR Coefficient.*/
	unsigned int reg_spike_nr_coef:4;

	// h0079, bit: 14
	/* */
	unsigned int :4;

	// h0079
	unsigned int /* padding 16 bit */:16;

	// h007a, bit: 7
	/* Spike NR P threshold 0.*/
	unsigned int reg_p_thrd:8;

	// h007a, bit: 12
	/* Spike NR P threshold 1.*/
	unsigned int reg_p_thrd_1:5;

	// h007a, bit: 14
	/* */
	unsigned int :3;

	// h007a
	unsigned int /* padding 16 bit */:16;

	// h007b, bit: 7
	/* Spike NR P threshold 2.*/
	unsigned int reg_p_thrd_2:8;

	// h007b, bit: 14
	/* Spike NR P threshold 3.*/
	unsigned int reg_p_thrd_3:8;

	// h007b
	unsigned int /* padding 16 bit */:16;

	// h007c, bit: 2
	/* Spike NR D11_21 Step.*/
	unsigned int reg_d_11_21_step:3;

	// h007c, bit: 3
	/* */
	unsigned int :1;

	// h007c, bit: 6
	/* Spike NR D31 Step.*/
	unsigned int reg_d_31_step:3;

	// h007c, bit: 7
	/* */
	unsigned int :1;

	// h007c, bit: 10
	/* Spike NR YP22 Step.*/
	unsigned int reg_yp_22_step:3;

	// h007c, bit: 14
	/* */
	unsigned int :5;

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

}  __attribute__((packed, aligned(1))) bank_1;
#endif
