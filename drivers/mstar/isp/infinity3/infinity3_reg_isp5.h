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
#ifndef __INFINITY3_REG_ISP5__
#define __INFINITY3_REG_ISP5__
typedef struct {
	// h0000, bit: 1
	/* */
	unsigned int reg_statis_af_mi_lenx4:2;

	// h0000, bit: 7
	/* */
	unsigned int reg_statis_af_mi_mask_cyclex16:6;

	// h0000, bit: 14
	/* */
	unsigned int :8;

	// h0000
	unsigned int /* padding 16 bit */:16;

	// h0001, bit: 0
	/* */
	unsigned int reg_af_kr_sign:1;

	// h0001, bit: 1
	/* */
	unsigned int reg_af_kgr_sign:1;

	// h0001, bit: 2
	/* */
	unsigned int reg_af_kgb_sign:1;

	// h0001, bit: 3
	/* */
	unsigned int reg_af_kb_sign:1;

	// h0001, bit: 4
	/* */
	unsigned int reg_rgbir2x2_g_loc:1;

	// h0001, bit: 14
	/* */
	unsigned int :11;

	// h0001
	unsigned int /* padding 16 bit */:16;

	// h0002, bit: 7
	/* */
	unsigned int reg_af_kr_value:8;

	// h0002, bit: 14
	/* */
	unsigned int reg_af_kgr_value:8;

	// h0002
	unsigned int /* padding 16 bit */:16;

	// h0003, bit: 7
	/* */
	unsigned int reg_af_kgb_value:8;

	// h0003, bit: 14
	/* */
	unsigned int reg_af_kb_value:8;

	// h0003
	unsigned int /* padding 16 bit */:16;

	// h0004, bit: 0
	/* */
	unsigned int reg_high_image_pipe_en:1;

	// h0004, bit: 1
	/* */
	unsigned int reg_low_image_pipe_en:1;

	// h0004, bit: 2
	/* */
	unsigned int reg_luma_image_pipe_en:1;

	// h0004, bit: 3
	/* */
	unsigned int reg_sobelv_en:1;

	// h0004, bit: 4
	/* */
	unsigned int reg_sobelh_en:1;

	// h0004, bit: 5
	/* */
	unsigned int reg_high_iir_mem_frame_clr:1;

	// h0004, bit: 6
	/* */
	unsigned int reg_low_iir_mem_frame_clr:1;

	// h0004, bit: 14
	/* */
	unsigned int :9;

	// h0004
	unsigned int /* padding 16 bit */:16;

	// h0005, bit: 9
	/* */
	unsigned int reg_high_1st_low_clip:10;

	// h0005, bit: 14
	/* */
	unsigned int :6;

	// h0005
	unsigned int /* padding 16 bit */:16;

	// h0006, bit: 9
	/* */
	unsigned int reg_high_1st_high_clip:10;

	// h0006, bit: 14
	/* */
	unsigned int :6;

	// h0006
	unsigned int /* padding 16 bit */:16;

	// h0007, bit: 9
	/* */
	unsigned int reg_high_iir_b0:10;

	// h0007, bit: 14
	/* */
	unsigned int :6;

	// h0007
	unsigned int /* padding 16 bit */:16;

	// h0008, bit: 10
	/* */
	unsigned int reg_high_iir_b1:11;

	// h0008, bit: 14
	/* */
	unsigned int :5;

	// h0008
	unsigned int /* padding 16 bit */:16;

	// h0009, bit: 9
	/* */
	unsigned int reg_high_iir_b2:10;

	// h0009, bit: 14
	/* */
	unsigned int :6;

	// h0009
	unsigned int /* padding 16 bit */:16;

	// h000a, bit: 7
	/* */
	unsigned int reg_high_iir_a1:8;

	// h000a, bit: 14
	/* */
	unsigned int :8;

	// h000a
	unsigned int /* padding 16 bit */:16;

	// h000b, bit: 7
	/* */
	unsigned int reg_high_iir_a2:8;

	// h000b, bit: 14
	/* */
	unsigned int :8;

	// h000b
	unsigned int /* padding 16 bit */:16;

	// h000c, bit: 9
	/* */
	unsigned int reg_high_2nd_low_clip:10;

	// h000c, bit: 14
	/* */
	unsigned int :6;

	// h000c
	unsigned int /* padding 16 bit */:16;

	// h000d, bit: 9
	/* */
	unsigned int reg_high_2nd_high_clip:10;

	// h000d, bit: 14
	/* */
	unsigned int :6;

	// h000d
	unsigned int /* padding 16 bit */:16;

	// h000e, bit: 11
	/* */
	unsigned int reg_high_af0_h_start:12;

	// h000e, bit: 14
	/* */
	unsigned int :4;

	// h000e
	unsigned int /* padding 16 bit */:16;

	// h000f, bit: 11
	/* */
	unsigned int reg_high_af0_h_end:12;

	// h000f, bit: 14
	/* */
	unsigned int :4;

	// h000f
	unsigned int /* padding 16 bit */:16;

	// h0010, bit: 11
	/* */
	unsigned int reg_high_af0_v_start:12;

	// h0010, bit: 14
	/* */
	unsigned int :4;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 11
	/* */
	unsigned int reg_high_af0_v_end:12;

	// h0011, bit: 14
	/* */
	unsigned int :4;

	// h0011
	unsigned int /* padding 16 bit */:16;

	// h0012, bit: 11
	/* */
	unsigned int reg_high_af1_h_start:12;

	// h0012, bit: 14
	/* */
	unsigned int :4;

	// h0012
	unsigned int /* padding 16 bit */:16;

	// h0013, bit: 11
	/* */
	unsigned int reg_high_af1_h_end:12;

	// h0013, bit: 14
	/* */
	unsigned int :4;

	// h0013
	unsigned int /* padding 16 bit */:16;

	// h0014, bit: 11
	/* */
	unsigned int reg_high_af1_v_start:12;

	// h0014, bit: 14
	/* */
	unsigned int :4;

	// h0014
	unsigned int /* padding 16 bit */:16;

	// h0015, bit: 11
	/* */
	unsigned int reg_high_af1_v_end:12;

	// h0015, bit: 14
	/* */
	unsigned int :4;

	// h0015
	unsigned int /* padding 16 bit */:16;

	// h0016, bit: 11
	/* */
	unsigned int reg_high_af2_h_start:12;

	// h0016, bit: 14
	/* */
	unsigned int :4;

	// h0016
	unsigned int /* padding 16 bit */:16;

	// h0017, bit: 11
	/* */
	unsigned int reg_high_af2_h_end:12;

	// h0017, bit: 14
	/* */
	unsigned int :4;

	// h0017
	unsigned int /* padding 16 bit */:16;

	// h0018, bit: 11
	/* */
	unsigned int reg_high_af2_v_start:12;

	// h0018, bit: 14
	/* */
	unsigned int :4;

	// h0018
	unsigned int /* padding 16 bit */:16;

	// h0019, bit: 11
	/* */
	unsigned int reg_high_af2_v_end:12;

	// h0019, bit: 14
	/* */
	unsigned int :4;

	// h0019
	unsigned int /* padding 16 bit */:16;

	// h001a, bit: 11
	/* */
	unsigned int reg_high_af3_h_start:12;

	// h001a, bit: 14
	/* */
	unsigned int :4;

	// h001a
	unsigned int /* padding 16 bit */:16;

	// h001b, bit: 11
	/* */
	unsigned int reg_high_af3_h_end:12;

	// h001b, bit: 14
	/* */
	unsigned int :4;

	// h001b
	unsigned int /* padding 16 bit */:16;

	// h001c, bit: 11
	/* */
	unsigned int reg_high_af3_v_start:12;

	// h001c, bit: 14
	/* */
	unsigned int :4;

	// h001c
	unsigned int /* padding 16 bit */:16;

	// h001d, bit: 11
	/* */
	unsigned int reg_high_af3_v_end:12;

	// h001d, bit: 14
	/* */
	unsigned int :4;

	// h001d
	unsigned int /* padding 16 bit */:16;

	// h001e, bit: 11
	/* */
	unsigned int reg_high_af4_h_start:12;

	// h001e, bit: 14
	/* */
	unsigned int :4;

	// h001e
	unsigned int /* padding 16 bit */:16;

	// h001f, bit: 11
	/* */
	unsigned int reg_high_af4_h_end:12;

	// h001f, bit: 14
	/* */
	unsigned int :4;

	// h001f
	unsigned int /* padding 16 bit */:16;

	// h0020, bit: 11
	/* */
	unsigned int reg_high_af4_v_start:12;

	// h0020, bit: 14
	/* */
	unsigned int :4;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 11
	/* */
	unsigned int reg_high_af4_v_end:12;

	// h0021, bit: 14
	/* */
	unsigned int :4;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 11
	/* */
	unsigned int reg_high_af5_h_start:12;

	// h0022, bit: 14
	/* */
	unsigned int :4;

	// h0022
	unsigned int /* padding 16 bit */:16;

	// h0023, bit: 11
	/* */
	unsigned int reg_high_af5_h_end:12;

	// h0023, bit: 14
	/* */
	unsigned int :4;

	// h0023
	unsigned int /* padding 16 bit */:16;

	// h0024, bit: 11
	/* */
	unsigned int reg_high_af5_v_start:12;

	// h0024, bit: 14
	/* */
	unsigned int :4;

	// h0024
	unsigned int /* padding 16 bit */:16;

	// h0025, bit: 11
	/* */
	unsigned int reg_high_af5_v_end:12;

	// h0025, bit: 14
	/* */
	unsigned int :4;

	// h0025
	unsigned int /* padding 16 bit */:16;

	// h0026, bit: 11
	/* */
	unsigned int reg_high_af6_h_start:12;

	// h0026, bit: 14
	/* */
	unsigned int :4;

	// h0026
	unsigned int /* padding 16 bit */:16;

	// h0027, bit: 11
	/* */
	unsigned int reg_high_af6_h_end:12;

	// h0027, bit: 14
	/* */
	unsigned int :4;

	// h0027
	unsigned int /* padding 16 bit */:16;

	// h0028, bit: 11
	/* */
	unsigned int reg_high_af6_v_start:12;

	// h0028, bit: 14
	/* */
	unsigned int :4;

	// h0028
	unsigned int /* padding 16 bit */:16;

	// h0029, bit: 11
	/* */
	unsigned int reg_high_af6_v_end:12;

	// h0029, bit: 14
	/* */
	unsigned int :4;

	// h0029
	unsigned int /* padding 16 bit */:16;

	// h002a, bit: 11
	/* */
	unsigned int reg_high_af7_h_start:12;

	// h002a, bit: 14
	/* */
	unsigned int :4;

	// h002a
	unsigned int /* padding 16 bit */:16;

	// h002b, bit: 11
	/* */
	unsigned int reg_high_af7_h_end:12;

	// h002b, bit: 14
	/* */
	unsigned int :4;

	// h002b
	unsigned int /* padding 16 bit */:16;

	// h002c, bit: 11
	/* */
	unsigned int reg_high_af7_v_start:12;

	// h002c, bit: 14
	/* */
	unsigned int :4;

	// h002c
	unsigned int /* padding 16 bit */:16;

	// h002d, bit: 11
	/* */
	unsigned int reg_high_af7_v_end:12;

	// h002d, bit: 14
	/* */
	unsigned int :4;

	// h002d
	unsigned int /* padding 16 bit */:16;

	// h002e, bit: 11
	/* */
	unsigned int reg_high_af8_h_start:12;

	// h002e, bit: 14
	/* */
	unsigned int :4;

	// h002e
	unsigned int /* padding 16 bit */:16;

	// h002f, bit: 11
	/* */
	unsigned int reg_high_af8_h_end:12;

	// h002f, bit: 14
	/* */
	unsigned int :4;

	// h002f
	unsigned int /* padding 16 bit */:16;

	// h0030, bit: 11
	/* */
	unsigned int reg_high_af8_v_start:12;

	// h0030, bit: 14
	/* */
	unsigned int :4;

	// h0030
	unsigned int /* padding 16 bit */:16;

	// h0031, bit: 11
	/* */
	unsigned int reg_high_af8_v_end:12;

	// h0031, bit: 14
	/* */
	unsigned int :4;

	// h0031
	unsigned int /* padding 16 bit */:16;

	// h0032, bit: 11
	/* */
	unsigned int reg_high_af9_h_start:12;

	// h0032, bit: 14
	/* */
	unsigned int :4;

	// h0032
	unsigned int /* padding 16 bit */:16;

	// h0033, bit: 11
	/* */
	unsigned int reg_high_af9_h_end:12;

	// h0033, bit: 14
	/* */
	unsigned int :4;

	// h0033
	unsigned int /* padding 16 bit */:16;

	// h0034, bit: 11
	/* */
	unsigned int reg_high_af9_v_start:12;

	// h0034, bit: 14
	/* */
	unsigned int :4;

	// h0034
	unsigned int /* padding 16 bit */:16;

	// h0035, bit: 11
	/* */
	unsigned int reg_high_af9_v_end:12;

	// h0035, bit: 14
	/* */
	unsigned int :4;

	// h0035
	unsigned int /* padding 16 bit */:16;

	// h0036, bit: 9
	/* */
	unsigned int reg_low_1st_low_clip:10;

	// h0036, bit: 14
	/* */
	unsigned int :6;

	// h0036
	unsigned int /* padding 16 bit */:16;

	// h0037, bit: 9
	/* */
	unsigned int reg_low_1st_high_clip:10;

	// h0037, bit: 14
	/* */
	unsigned int :6;

	// h0037
	unsigned int /* padding 16 bit */:16;

	// h0038, bit: 9
	/* */
	unsigned int reg_low_iir_b0:10;

	// h0038, bit: 14
	/* */
	unsigned int :6;

	// h0038
	unsigned int /* padding 16 bit */:16;

	// h0039, bit: 10
	/* */
	unsigned int reg_low_iir_b1:11;

	// h0039, bit: 14
	/* */
	unsigned int :5;

	// h0039
	unsigned int /* padding 16 bit */:16;

	// h003a, bit: 9
	/* */
	unsigned int reg_low_iir_b2:10;

	// h003a, bit: 14
	/* */
	unsigned int :6;

	// h003a
	unsigned int /* padding 16 bit */:16;

	// h003b, bit: 7
	/* */
	unsigned int reg_low_iir_a1:8;

	// h003b, bit: 14
	/* */
	unsigned int :8;

	// h003b
	unsigned int /* padding 16 bit */:16;

	// h003c, bit: 7
	/* */
	unsigned int reg_low_iir_a2:8;

	// h003c, bit: 14
	/* */
	unsigned int :8;

	// h003c
	unsigned int /* padding 16 bit */:16;

	// h003d, bit: 9
	/* */
	unsigned int reg_low_2nd_low_clip:10;

	// h003d, bit: 14
	/* */
	unsigned int :6;

	// h003d
	unsigned int /* padding 16 bit */:16;

	// h003e, bit: 9
	/* */
	unsigned int reg_low_2nd_high_clip:10;

	// h003e, bit: 14
	/* */
	unsigned int :6;

	// h003e
	unsigned int /* padding 16 bit */:16;

	// h003f, bit: 9
	/* */
	unsigned int reg_luma_low_clip:10;

	// h003f, bit: 14
	/* */
	unsigned int :6;

	// h003f
	unsigned int /* padding 16 bit */:16;

	// h0040, bit: 9
	/* */
	unsigned int reg_luma_high_clip:10;

	// h0040, bit: 14
	/* */
	unsigned int :6;

	// h0040
	unsigned int /* padding 16 bit */:16;

	// h0041, bit: 9
	/* */
	unsigned int reg_sobelv_1st_low_clip:10;

	// h0041, bit: 14
	/* */
	unsigned int :6;

	// h0041
	unsigned int /* padding 16 bit */:16;

	// h0042, bit: 9
	/* */
	unsigned int reg_sobelv_1st_high_clip:10;

	// h0042, bit: 14
	/* */
	unsigned int :6;

	// h0042
	unsigned int /* padding 16 bit */:16;

	// h0043, bit: 9
	/* */
	unsigned int reg_sobelv_2nd_low_clip:10;

	// h0043, bit: 14
	/* */
	unsigned int :6;

	// h0043
	unsigned int /* padding 16 bit */:16;

	// h0044, bit: 9
	/* */
	unsigned int reg_sobelv_2nd_high_clip:10;

	// h0044, bit: 14
	/* */
	unsigned int :6;

	// h0044
	unsigned int /* padding 16 bit */:16;

	// h0045, bit: 9
	/* */
	unsigned int reg_sobelh_1st_low_clip:10;

	// h0045, bit: 14
	/* */
	unsigned int :6;

	// h0045
	unsigned int /* padding 16 bit */:16;

	// h0046, bit: 9
	/* */
	unsigned int reg_sobelh_1st_high_clip:10;

	// h0046, bit: 14
	/* */
	unsigned int :6;

	// h0046
	unsigned int /* padding 16 bit */:16;

	// h0047, bit: 9
	/* */
	unsigned int reg_sobelh_2nd_low_clip:10;

	// h0047, bit: 14
	/* */
	unsigned int :6;

	// h0047
	unsigned int /* padding 16 bit */:16;

	// h0048, bit: 9
	/* */
	unsigned int reg_sobelh_2nd_high_clip:10;

	// h0048, bit: 14
	/* */
	unsigned int :6;

	// h0048
	unsigned int /* padding 16 bit */:16;

	// h0049, bit: 14
	/* */
	unsigned int reg_af_dma_addr_0:16;

	// h0049
	unsigned int /* padding 16 bit */:16;

	// h004a, bit: 10
	/* */
	unsigned int reg_af_dma_addr_1:11;

	// h004a, bit: 14
	/* */
	unsigned int :5;

	// h004a
	unsigned int /* padding 16 bit */:16;

	// h004b, bit: 14
	/* */
	unsigned int reg_af_max_wadr_0:16;

	// h004b
	unsigned int /* padding 16 bit */:16;

	// h004c, bit: 10
	/* */
	unsigned int reg_af_max_wadr_1:11;

	// h004c, bit: 14
	/* */
	unsigned int :5;

	// h004c
	unsigned int /* padding 16 bit */:16;

	// h004d, bit: 0
	/* */
	unsigned int reg_af_filter_int:1;

	// h004d, bit: 14
	/* */
	unsigned int :15;

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

}  __attribute__((packed, aligned(1))) bank_5;
#endif
