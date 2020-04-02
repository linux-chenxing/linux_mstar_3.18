// Generate Time: 2017-09-19 22:58:04.122583
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
#ifndef __INFINITY2_REG_ISP5__
#define __INFINITY2_REG_ISP5__
typedef struct {
	// h0000, bit: 1
	/* */
	#define offset_of_isp5_reg_statis_af_mi_lenx4 (0)
	#define mask_of_isp5_reg_statis_af_mi_lenx4 (0x3)
	unsigned int reg_statis_af_mi_lenx4:2;

	// h0000, bit: 7
	/* */
	#define offset_of_isp5_reg_statis_af_mi_mask_cyclex16 (0)
	#define mask_of_isp5_reg_statis_af_mi_mask_cyclex16 (0xfc)
	unsigned int reg_statis_af_mi_mask_cyclex16:6;

	// h0000, bit: 14
	/* */
	unsigned int :8;

	// h0000
	unsigned int /* padding 16 bit */:16;

	// h0001, bit: 0
	/* */
	#define offset_of_isp5_reg_af_kr_sign (2)
	#define mask_of_isp5_reg_af_kr_sign (0x1)
	unsigned int reg_af_kr_sign:1;

	// h0001, bit: 1
	/* */
	#define offset_of_isp5_reg_af_kgr_sign (2)
	#define mask_of_isp5_reg_af_kgr_sign (0x2)
	unsigned int reg_af_kgr_sign:1;

	// h0001, bit: 2
	/* */
	#define offset_of_isp5_reg_af_kgb_sign (2)
	#define mask_of_isp5_reg_af_kgb_sign (0x4)
	unsigned int reg_af_kgb_sign:1;

	// h0001, bit: 3
	/* */
	#define offset_of_isp5_reg_af_kb_sign (2)
	#define mask_of_isp5_reg_af_kb_sign (0x8)
	unsigned int reg_af_kb_sign:1;

	// h0001, bit: 4
	/* */
	#define offset_of_isp5_reg_rgbir2x2_g_loc (2)
	#define mask_of_isp5_reg_rgbir2x2_g_loc (0x10)
	unsigned int reg_rgbir2x2_g_loc:1;

	// h0001, bit: 14
	/* */
	unsigned int :11;

	// h0001
	unsigned int /* padding 16 bit */:16;

	// h0002, bit: 7
	/* */
	#define offset_of_isp5_reg_af_kr_value (4)
	#define mask_of_isp5_reg_af_kr_value (0xff)
	unsigned int reg_af_kr_value:8;

	// h0002, bit: 14
	/* */
	#define offset_of_isp5_reg_af_kgr_value (4)
	#define mask_of_isp5_reg_af_kgr_value (0xff00)
	unsigned int reg_af_kgr_value:8;

	// h0002
	unsigned int /* padding 16 bit */:16;

	// h0003, bit: 7
	/* */
	#define offset_of_isp5_reg_af_kgb_value (6)
	#define mask_of_isp5_reg_af_kgb_value (0xff)
	unsigned int reg_af_kgb_value:8;

	// h0003, bit: 14
	/* */
	#define offset_of_isp5_reg_af_kb_value (6)
	#define mask_of_isp5_reg_af_kb_value (0xff00)
	unsigned int reg_af_kb_value:8;

	// h0003
	unsigned int /* padding 16 bit */:16;

	// h0004, bit: 0
	/* */
	#define offset_of_isp5_reg_high_image_pipe_en (8)
	#define mask_of_isp5_reg_high_image_pipe_en (0x1)
	unsigned int reg_high_image_pipe_en:1;

	// h0004, bit: 1
	/* */
	#define offset_of_isp5_reg_low_image_pipe_en (8)
	#define mask_of_isp5_reg_low_image_pipe_en (0x2)
	unsigned int reg_low_image_pipe_en:1;

	// h0004, bit: 2
	/* */
	#define offset_of_isp5_reg_luma_image_pipe_en (8)
	#define mask_of_isp5_reg_luma_image_pipe_en (0x4)
	unsigned int reg_luma_image_pipe_en:1;

	// h0004, bit: 3
	/* */
	#define offset_of_isp5_reg_sobelv_en (8)
	#define mask_of_isp5_reg_sobelv_en (0x8)
	unsigned int reg_sobelv_en:1;

	// h0004, bit: 4
	/* */
	#define offset_of_isp5_reg_sobelh_en (8)
	#define mask_of_isp5_reg_sobelh_en (0x10)
	unsigned int reg_sobelh_en:1;

	// h0004, bit: 5
	/* */
	#define offset_of_isp5_reg_high_iir_mem_frame_clr (8)
	#define mask_of_isp5_reg_high_iir_mem_frame_clr (0x20)
	unsigned int reg_high_iir_mem_frame_clr:1;

	// h0004, bit: 6
	/* */
	#define offset_of_isp5_reg_low_iir_mem_frame_clr (8)
	#define mask_of_isp5_reg_low_iir_mem_frame_clr (0x40)
	unsigned int reg_low_iir_mem_frame_clr:1;

	// h0004, bit: 7
	/* */
	#define offset_of_isp5_reg_sobelv_ysat_en (8)
	#define mask_of_isp5_reg_sobelv_ysat_en (0x80)
	unsigned int reg_sobelv_ysat_en:1;

	// h0004, bit: 14
	/* */
	unsigned int :8;

	// h0004
	unsigned int /* padding 16 bit */:16;

	// h0005, bit: 9
	/* */
	#define offset_of_isp5_reg_high_1st_low_clip (10)
	#define mask_of_isp5_reg_high_1st_low_clip (0x3ff)
	unsigned int reg_high_1st_low_clip:10;

	// h0005, bit: 14
	/* */
	unsigned int :6;

	// h0005
	unsigned int /* padding 16 bit */:16;

	// h0006, bit: 9
	/* */
	#define offset_of_isp5_reg_high_1st_high_clip (12)
	#define mask_of_isp5_reg_high_1st_high_clip (0x3ff)
	unsigned int reg_high_1st_high_clip:10;

	// h0006, bit: 14
	/* */
	unsigned int :6;

	// h0006
	unsigned int /* padding 16 bit */:16;

	// h0007, bit: 9
	/* */
	#define offset_of_isp5_reg_high_iir_b0 (14)
	#define mask_of_isp5_reg_high_iir_b0 (0x3ff)
	unsigned int reg_high_iir_b0:10;

	// h0007, bit: 14
	/* */
	unsigned int :6;

	// h0007
	unsigned int /* padding 16 bit */:16;

	// h0008, bit: 10
	/* */
	#define offset_of_isp5_reg_high_iir_b1 (16)
	#define mask_of_isp5_reg_high_iir_b1 (0x7ff)
	unsigned int reg_high_iir_b1:11;

	// h0008, bit: 14
	/* */
	unsigned int :5;

	// h0008
	unsigned int /* padding 16 bit */:16;

	// h0009, bit: 9
	/* */
	#define offset_of_isp5_reg_high_iir_b2 (18)
	#define mask_of_isp5_reg_high_iir_b2 (0x3ff)
	unsigned int reg_high_iir_b2:10;

	// h0009, bit: 14
	/* */
	unsigned int :6;

	// h0009
	unsigned int /* padding 16 bit */:16;

	// h000a, bit: 7
	/* */
	#define offset_of_isp5_reg_high_iir_a1 (20)
	#define mask_of_isp5_reg_high_iir_a1 (0xff)
	unsigned int reg_high_iir_a1:8;

	// h000a, bit: 14
	/* */
	unsigned int :8;

	// h000a
	unsigned int /* padding 16 bit */:16;

	// h000b, bit: 7
	/* */
	#define offset_of_isp5_reg_high_iir_a2 (22)
	#define mask_of_isp5_reg_high_iir_a2 (0xff)
	unsigned int reg_high_iir_a2:8;

	// h000b, bit: 14
	/* */
	unsigned int :8;

	// h000b
	unsigned int /* padding 16 bit */:16;

	// h000c, bit: 9
	/* */
	#define offset_of_isp5_reg_high_2nd_low_clip (24)
	#define mask_of_isp5_reg_high_2nd_low_clip (0x3ff)
	unsigned int reg_high_2nd_low_clip:10;

	// h000c, bit: 14
	/* */
	unsigned int :6;

	// h000c
	unsigned int /* padding 16 bit */:16;

	// h000d, bit: 9
	/* */
	#define offset_of_isp5_reg_high_2nd_high_clip (26)
	#define mask_of_isp5_reg_high_2nd_high_clip (0x3ff)
	unsigned int reg_high_2nd_high_clip:10;

	// h000d, bit: 14
	/* */
	unsigned int :6;

	// h000d
	unsigned int /* padding 16 bit */:16;

	// h000e, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af0_h_start (28)
	#define mask_of_isp5_reg_high_af0_h_start (0x1fff)
	unsigned int reg_high_af0_h_start:13;

	// h000e, bit: 14
	/* */
	unsigned int :3;

	// h000e
	unsigned int /* padding 16 bit */:16;

	// h000f, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af0_h_end (30)
	#define mask_of_isp5_reg_high_af0_h_end (0x1fff)
	unsigned int reg_high_af0_h_end:13;

	// h000f, bit: 14
	/* */
	unsigned int :3;

	// h000f
	unsigned int /* padding 16 bit */:16;

	// h0010, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af0_v_start (32)
	#define mask_of_isp5_reg_high_af0_v_start (0x1fff)
	unsigned int reg_high_af0_v_start:13;

	// h0010, bit: 14
	/* */
	unsigned int :3;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af0_v_end (34)
	#define mask_of_isp5_reg_high_af0_v_end (0x1fff)
	unsigned int reg_high_af0_v_end:13;

	// h0011, bit: 14
	/* */
	unsigned int :3;

	// h0011
	unsigned int /* padding 16 bit */:16;

	// h0012, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af1_h_start (36)
	#define mask_of_isp5_reg_high_af1_h_start (0x1fff)
	unsigned int reg_high_af1_h_start:13;

	// h0012, bit: 14
	/* */
	unsigned int :3;

	// h0012
	unsigned int /* padding 16 bit */:16;

	// h0013, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af1_h_end (38)
	#define mask_of_isp5_reg_high_af1_h_end (0x1fff)
	unsigned int reg_high_af1_h_end:13;

	// h0013, bit: 14
	/* */
	unsigned int :3;

	// h0013
	unsigned int /* padding 16 bit */:16;

	// h0014, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af1_v_start (40)
	#define mask_of_isp5_reg_high_af1_v_start (0x1fff)
	unsigned int reg_high_af1_v_start:13;

	// h0014, bit: 14
	/* */
	unsigned int :3;

	// h0014
	unsigned int /* padding 16 bit */:16;

	// h0015, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af1_v_end (42)
	#define mask_of_isp5_reg_high_af1_v_end (0x1fff)
	unsigned int reg_high_af1_v_end:13;

	// h0015, bit: 14
	/* */
	unsigned int :3;

	// h0015
	unsigned int /* padding 16 bit */:16;

	// h0016, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af2_h_start (44)
	#define mask_of_isp5_reg_high_af2_h_start (0x1fff)
	unsigned int reg_high_af2_h_start:13;

	// h0016, bit: 14
	/* */
	unsigned int :3;

	// h0016
	unsigned int /* padding 16 bit */:16;

	// h0017, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af2_h_end (46)
	#define mask_of_isp5_reg_high_af2_h_end (0x1fff)
	unsigned int reg_high_af2_h_end:13;

	// h0017, bit: 14
	/* */
	unsigned int :3;

	// h0017
	unsigned int /* padding 16 bit */:16;

	// h0018, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af2_v_start (48)
	#define mask_of_isp5_reg_high_af2_v_start (0x1fff)
	unsigned int reg_high_af2_v_start:13;

	// h0018, bit: 14
	/* */
	unsigned int :3;

	// h0018
	unsigned int /* padding 16 bit */:16;

	// h0019, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af2_v_end (50)
	#define mask_of_isp5_reg_high_af2_v_end (0x1fff)
	unsigned int reg_high_af2_v_end:13;

	// h0019, bit: 14
	/* */
	unsigned int :3;

	// h0019
	unsigned int /* padding 16 bit */:16;

	// h001a, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af3_h_start (52)
	#define mask_of_isp5_reg_high_af3_h_start (0x1fff)
	unsigned int reg_high_af3_h_start:13;

	// h001a, bit: 14
	/* */
	unsigned int :3;

	// h001a
	unsigned int /* padding 16 bit */:16;

	// h001b, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af3_h_end (54)
	#define mask_of_isp5_reg_high_af3_h_end (0x1fff)
	unsigned int reg_high_af3_h_end:13;

	// h001b, bit: 14
	/* */
	unsigned int :3;

	// h001b
	unsigned int /* padding 16 bit */:16;

	// h001c, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af3_v_start (56)
	#define mask_of_isp5_reg_high_af3_v_start (0x1fff)
	unsigned int reg_high_af3_v_start:13;

	// h001c, bit: 14
	/* */
	unsigned int :3;

	// h001c
	unsigned int /* padding 16 bit */:16;

	// h001d, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af3_v_end (58)
	#define mask_of_isp5_reg_high_af3_v_end (0x1fff)
	unsigned int reg_high_af3_v_end:13;

	// h001d, bit: 14
	/* */
	unsigned int :3;

	// h001d
	unsigned int /* padding 16 bit */:16;

	// h001e, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af4_h_start (60)
	#define mask_of_isp5_reg_high_af4_h_start (0x1fff)
	unsigned int reg_high_af4_h_start:13;

	// h001e, bit: 14
	/* */
	unsigned int :3;

	// h001e
	unsigned int /* padding 16 bit */:16;

	// h001f, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af4_h_end (62)
	#define mask_of_isp5_reg_high_af4_h_end (0x1fff)
	unsigned int reg_high_af4_h_end:13;

	// h001f, bit: 14
	/* */
	unsigned int :3;

	// h001f
	unsigned int /* padding 16 bit */:16;

	// h0020, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af4_v_start (64)
	#define mask_of_isp5_reg_high_af4_v_start (0x1fff)
	unsigned int reg_high_af4_v_start:13;

	// h0020, bit: 14
	/* */
	unsigned int :3;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af4_v_end (66)
	#define mask_of_isp5_reg_high_af4_v_end (0x1fff)
	unsigned int reg_high_af4_v_end:13;

	// h0021, bit: 14
	/* */
	unsigned int :3;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af5_h_start (68)
	#define mask_of_isp5_reg_high_af5_h_start (0x1fff)
	unsigned int reg_high_af5_h_start:13;

	// h0022, bit: 14
	/* */
	unsigned int :3;

	// h0022
	unsigned int /* padding 16 bit */:16;

	// h0023, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af5_h_end (70)
	#define mask_of_isp5_reg_high_af5_h_end (0x1fff)
	unsigned int reg_high_af5_h_end:13;

	// h0023, bit: 14
	/* */
	unsigned int :3;

	// h0023
	unsigned int /* padding 16 bit */:16;

	// h0024, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af5_v_start (72)
	#define mask_of_isp5_reg_high_af5_v_start (0x1fff)
	unsigned int reg_high_af5_v_start:13;

	// h0024, bit: 14
	/* */
	unsigned int :3;

	// h0024
	unsigned int /* padding 16 bit */:16;

	// h0025, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af5_v_end (74)
	#define mask_of_isp5_reg_high_af5_v_end (0x1fff)
	unsigned int reg_high_af5_v_end:13;

	// h0025, bit: 14
	/* */
	unsigned int :3;

	// h0025
	unsigned int /* padding 16 bit */:16;

	// h0026, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af6_h_start (76)
	#define mask_of_isp5_reg_high_af6_h_start (0x1fff)
	unsigned int reg_high_af6_h_start:13;

	// h0026, bit: 14
	/* */
	unsigned int :3;

	// h0026
	unsigned int /* padding 16 bit */:16;

	// h0027, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af6_h_end (78)
	#define mask_of_isp5_reg_high_af6_h_end (0x1fff)
	unsigned int reg_high_af6_h_end:13;

	// h0027, bit: 14
	/* */
	unsigned int :3;

	// h0027
	unsigned int /* padding 16 bit */:16;

	// h0028, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af6_v_start (80)
	#define mask_of_isp5_reg_high_af6_v_start (0x1fff)
	unsigned int reg_high_af6_v_start:13;

	// h0028, bit: 14
	/* */
	unsigned int :3;

	// h0028
	unsigned int /* padding 16 bit */:16;

	// h0029, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af6_v_end (82)
	#define mask_of_isp5_reg_high_af6_v_end (0x1fff)
	unsigned int reg_high_af6_v_end:13;

	// h0029, bit: 14
	/* */
	unsigned int :3;

	// h0029
	unsigned int /* padding 16 bit */:16;

	// h002a, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af7_h_start (84)
	#define mask_of_isp5_reg_high_af7_h_start (0x1fff)
	unsigned int reg_high_af7_h_start:13;

	// h002a, bit: 14
	/* */
	unsigned int :3;

	// h002a
	unsigned int /* padding 16 bit */:16;

	// h002b, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af7_h_end (86)
	#define mask_of_isp5_reg_high_af7_h_end (0x1fff)
	unsigned int reg_high_af7_h_end:13;

	// h002b, bit: 14
	/* */
	unsigned int :3;

	// h002b
	unsigned int /* padding 16 bit */:16;

	// h002c, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af7_v_start (88)
	#define mask_of_isp5_reg_high_af7_v_start (0x1fff)
	unsigned int reg_high_af7_v_start:13;

	// h002c, bit: 14
	/* */
	unsigned int :3;

	// h002c
	unsigned int /* padding 16 bit */:16;

	// h002d, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af7_v_end (90)
	#define mask_of_isp5_reg_high_af7_v_end (0x1fff)
	unsigned int reg_high_af7_v_end:13;

	// h002d, bit: 14
	/* */
	unsigned int :3;

	// h002d
	unsigned int /* padding 16 bit */:16;

	// h002e, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af8_h_start (92)
	#define mask_of_isp5_reg_high_af8_h_start (0x1fff)
	unsigned int reg_high_af8_h_start:13;

	// h002e, bit: 14
	/* */
	unsigned int :3;

	// h002e
	unsigned int /* padding 16 bit */:16;

	// h002f, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af8_h_end (94)
	#define mask_of_isp5_reg_high_af8_h_end (0x1fff)
	unsigned int reg_high_af8_h_end:13;

	// h002f, bit: 14
	/* */
	unsigned int :3;

	// h002f
	unsigned int /* padding 16 bit */:16;

	// h0030, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af8_v_start (96)
	#define mask_of_isp5_reg_high_af8_v_start (0x1fff)
	unsigned int reg_high_af8_v_start:13;

	// h0030, bit: 14
	/* */
	unsigned int :3;

	// h0030
	unsigned int /* padding 16 bit */:16;

	// h0031, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af8_v_end (98)
	#define mask_of_isp5_reg_high_af8_v_end (0x1fff)
	unsigned int reg_high_af8_v_end:13;

	// h0031, bit: 14
	/* */
	unsigned int :3;

	// h0031
	unsigned int /* padding 16 bit */:16;

	// h0032, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af9_h_start (100)
	#define mask_of_isp5_reg_high_af9_h_start (0x1fff)
	unsigned int reg_high_af9_h_start:13;

	// h0032, bit: 14
	/* */
	unsigned int :3;

	// h0032
	unsigned int /* padding 16 bit */:16;

	// h0033, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af9_h_end (102)
	#define mask_of_isp5_reg_high_af9_h_end (0x1fff)
	unsigned int reg_high_af9_h_end:13;

	// h0033, bit: 14
	/* */
	unsigned int :3;

	// h0033
	unsigned int /* padding 16 bit */:16;

	// h0034, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af9_v_start (104)
	#define mask_of_isp5_reg_high_af9_v_start (0x1fff)
	unsigned int reg_high_af9_v_start:13;

	// h0034, bit: 14
	/* */
	unsigned int :3;

	// h0034
	unsigned int /* padding 16 bit */:16;

	// h0035, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af9_v_end (106)
	#define mask_of_isp5_reg_high_af9_v_end (0x1fff)
	unsigned int reg_high_af9_v_end:13;

	// h0035, bit: 14
	/* */
	unsigned int :3;

	// h0035
	unsigned int /* padding 16 bit */:16;

	// h0036, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af10_h_start (108)
	#define mask_of_isp5_reg_high_af10_h_start (0x1fff)
	unsigned int reg_high_af10_h_start:13;

	// h0036, bit: 14
	/* */
	unsigned int :3;

	// h0036
	unsigned int /* padding 16 bit */:16;

	// h0037, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af10_h_end (110)
	#define mask_of_isp5_reg_high_af10_h_end (0x1fff)
	unsigned int reg_high_af10_h_end:13;

	// h0037, bit: 14
	/* */
	unsigned int :3;

	// h0037
	unsigned int /* padding 16 bit */:16;

	// h0038, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af10_v_start (112)
	#define mask_of_isp5_reg_high_af10_v_start (0x1fff)
	unsigned int reg_high_af10_v_start:13;

	// h0038, bit: 14
	/* */
	unsigned int :3;

	// h0038
	unsigned int /* padding 16 bit */:16;

	// h0039, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af10_v_end (114)
	#define mask_of_isp5_reg_high_af10_v_end (0x1fff)
	unsigned int reg_high_af10_v_end:13;

	// h0039, bit: 14
	/* */
	unsigned int :3;

	// h0039
	unsigned int /* padding 16 bit */:16;

	// h003a, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af11_h_start (116)
	#define mask_of_isp5_reg_high_af11_h_start (0x1fff)
	unsigned int reg_high_af11_h_start:13;

	// h003a, bit: 14
	/* */
	unsigned int :3;

	// h003a
	unsigned int /* padding 16 bit */:16;

	// h003b, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af11_h_end (118)
	#define mask_of_isp5_reg_high_af11_h_end (0x1fff)
	unsigned int reg_high_af11_h_end:13;

	// h003b, bit: 14
	/* */
	unsigned int :3;

	// h003b
	unsigned int /* padding 16 bit */:16;

	// h003c, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af11_v_start (120)
	#define mask_of_isp5_reg_high_af11_v_start (0x1fff)
	unsigned int reg_high_af11_v_start:13;

	// h003c, bit: 14
	/* */
	unsigned int :3;

	// h003c
	unsigned int /* padding 16 bit */:16;

	// h003d, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af11_v_end (122)
	#define mask_of_isp5_reg_high_af11_v_end (0x1fff)
	unsigned int reg_high_af11_v_end:13;

	// h003d, bit: 14
	/* */
	unsigned int :3;

	// h003d
	unsigned int /* padding 16 bit */:16;

	// h003e, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af12_h_start (124)
	#define mask_of_isp5_reg_high_af12_h_start (0x1fff)
	unsigned int reg_high_af12_h_start:13;

	// h003e, bit: 14
	/* */
	unsigned int :3;

	// h003e
	unsigned int /* padding 16 bit */:16;

	// h003f, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af12_h_end (126)
	#define mask_of_isp5_reg_high_af12_h_end (0x1fff)
	unsigned int reg_high_af12_h_end:13;

	// h003f, bit: 14
	/* */
	unsigned int :3;

	// h003f
	unsigned int /* padding 16 bit */:16;

	// h0040, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af12_v_start (128)
	#define mask_of_isp5_reg_high_af12_v_start (0x1fff)
	unsigned int reg_high_af12_v_start:13;

	// h0040, bit: 14
	/* */
	unsigned int :3;

	// h0040
	unsigned int /* padding 16 bit */:16;

	// h0041, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af12_v_end (130)
	#define mask_of_isp5_reg_high_af12_v_end (0x1fff)
	unsigned int reg_high_af12_v_end:13;

	// h0041, bit: 14
	/* */
	unsigned int :3;

	// h0041
	unsigned int /* padding 16 bit */:16;

	// h0042, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af13_h_start (132)
	#define mask_of_isp5_reg_high_af13_h_start (0x1fff)
	unsigned int reg_high_af13_h_start:13;

	// h0042, bit: 14
	/* */
	unsigned int :3;

	// h0042
	unsigned int /* padding 16 bit */:16;

	// h0043, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af13_h_end (134)
	#define mask_of_isp5_reg_high_af13_h_end (0x1fff)
	unsigned int reg_high_af13_h_end:13;

	// h0043, bit: 14
	/* */
	unsigned int :3;

	// h0043
	unsigned int /* padding 16 bit */:16;

	// h0044, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af13_v_start (136)
	#define mask_of_isp5_reg_high_af13_v_start (0x1fff)
	unsigned int reg_high_af13_v_start:13;

	// h0044, bit: 14
	/* */
	unsigned int :3;

	// h0044
	unsigned int /* padding 16 bit */:16;

	// h0045, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af13_v_end (138)
	#define mask_of_isp5_reg_high_af13_v_end (0x1fff)
	unsigned int reg_high_af13_v_end:13;

	// h0045, bit: 14
	/* */
	unsigned int :3;

	// h0045
	unsigned int /* padding 16 bit */:16;

	// h0046, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af14_h_start (140)
	#define mask_of_isp5_reg_high_af14_h_start (0x1fff)
	unsigned int reg_high_af14_h_start:13;

	// h0046, bit: 14
	/* */
	unsigned int :3;

	// h0046
	unsigned int /* padding 16 bit */:16;

	// h0047, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af14_h_end (142)
	#define mask_of_isp5_reg_high_af14_h_end (0x1fff)
	unsigned int reg_high_af14_h_end:13;

	// h0047, bit: 14
	/* */
	unsigned int :3;

	// h0047
	unsigned int /* padding 16 bit */:16;

	// h0048, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af14_v_start (144)
	#define mask_of_isp5_reg_high_af14_v_start (0x1fff)
	unsigned int reg_high_af14_v_start:13;

	// h0048, bit: 14
	/* */
	unsigned int :3;

	// h0048
	unsigned int /* padding 16 bit */:16;

	// h0049, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af14_v_end (146)
	#define mask_of_isp5_reg_high_af14_v_end (0x1fff)
	unsigned int reg_high_af14_v_end:13;

	// h0049, bit: 14
	/* */
	unsigned int :3;

	// h0049
	unsigned int /* padding 16 bit */:16;

	// h004a, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af15_h_start (148)
	#define mask_of_isp5_reg_high_af15_h_start (0x1fff)
	unsigned int reg_high_af15_h_start:13;

	// h004a, bit: 14
	/* */
	unsigned int :3;

	// h004a
	unsigned int /* padding 16 bit */:16;

	// h004b, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af15_h_end (150)
	#define mask_of_isp5_reg_high_af15_h_end (0x1fff)
	unsigned int reg_high_af15_h_end:13;

	// h004b, bit: 14
	/* */
	unsigned int :3;

	// h004b
	unsigned int /* padding 16 bit */:16;

	// h004c, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af15_v_start (152)
	#define mask_of_isp5_reg_high_af15_v_start (0x1fff)
	unsigned int reg_high_af15_v_start:13;

	// h004c, bit: 14
	/* */
	unsigned int :3;

	// h004c
	unsigned int /* padding 16 bit */:16;

	// h004d, bit: 12
	/* */
	#define offset_of_isp5_reg_high_af15_v_end (154)
	#define mask_of_isp5_reg_high_af15_v_end (0x1fff)
	unsigned int reg_high_af15_v_end:13;

	// h004d, bit: 14
	/* */
	unsigned int :3;

	// h004d
	unsigned int /* padding 16 bit */:16;

	// h004e, bit: 9
	/* */
	#define offset_of_isp5_reg_low_1st_low_clip (156)
	#define mask_of_isp5_reg_low_1st_low_clip (0x3ff)
	unsigned int reg_low_1st_low_clip:10;

	// h004e, bit: 14
	/* */
	unsigned int :6;

	// h004e
	unsigned int /* padding 16 bit */:16;

	// h004f, bit: 9
	/* */
	#define offset_of_isp5_reg_low_1st_high_clip (158)
	#define mask_of_isp5_reg_low_1st_high_clip (0x3ff)
	unsigned int reg_low_1st_high_clip:10;

	// h004f, bit: 14
	/* */
	unsigned int :6;

	// h004f
	unsigned int /* padding 16 bit */:16;

	// h0050, bit: 9
	/* */
	#define offset_of_isp5_reg_low_iir_b0 (160)
	#define mask_of_isp5_reg_low_iir_b0 (0x3ff)
	unsigned int reg_low_iir_b0:10;

	// h0050, bit: 14
	/* */
	unsigned int :6;

	// h0050
	unsigned int /* padding 16 bit */:16;

	// h0051, bit: 10
	/* */
	#define offset_of_isp5_reg_low_iir_b1 (162)
	#define mask_of_isp5_reg_low_iir_b1 (0x7ff)
	unsigned int reg_low_iir_b1:11;

	// h0051, bit: 14
	/* */
	unsigned int :5;

	// h0051
	unsigned int /* padding 16 bit */:16;

	// h0052, bit: 9
	/* */
	#define offset_of_isp5_reg_low_iir_b2 (164)
	#define mask_of_isp5_reg_low_iir_b2 (0x3ff)
	unsigned int reg_low_iir_b2:10;

	// h0052, bit: 14
	/* */
	unsigned int :6;

	// h0052
	unsigned int /* padding 16 bit */:16;

	// h0053, bit: 7
	/* */
	#define offset_of_isp5_reg_low_iir_a1 (166)
	#define mask_of_isp5_reg_low_iir_a1 (0xff)
	unsigned int reg_low_iir_a1:8;

	// h0053, bit: 14
	/* */
	unsigned int :8;

	// h0053
	unsigned int /* padding 16 bit */:16;

	// h0054, bit: 7
	/* */
	#define offset_of_isp5_reg_low_iir_a2 (168)
	#define mask_of_isp5_reg_low_iir_a2 (0xff)
	unsigned int reg_low_iir_a2:8;

	// h0054, bit: 14
	/* */
	unsigned int :8;

	// h0054
	unsigned int /* padding 16 bit */:16;

	// h0055, bit: 9
	/* */
	#define offset_of_isp5_reg_low_2nd_low_clip (170)
	#define mask_of_isp5_reg_low_2nd_low_clip (0x3ff)
	unsigned int reg_low_2nd_low_clip:10;

	// h0055, bit: 14
	/* */
	unsigned int :6;

	// h0055
	unsigned int /* padding 16 bit */:16;

	// h0056, bit: 9
	/* */
	#define offset_of_isp5_reg_low_2nd_high_clip (172)
	#define mask_of_isp5_reg_low_2nd_high_clip (0x3ff)
	unsigned int reg_low_2nd_high_clip:10;

	// h0056, bit: 14
	/* */
	unsigned int :6;

	// h0056
	unsigned int /* padding 16 bit */:16;

	// h0057, bit: 9
	/* */
	#define offset_of_isp5_reg_luma_low_clip (174)
	#define mask_of_isp5_reg_luma_low_clip (0x3ff)
	unsigned int reg_luma_low_clip:10;

	// h0057, bit: 14
	/* */
	unsigned int :6;

	// h0057
	unsigned int /* padding 16 bit */:16;

	// h0058, bit: 9
	/* */
	#define offset_of_isp5_reg_luma_high_clip (176)
	#define mask_of_isp5_reg_luma_high_clip (0x3ff)
	unsigned int reg_luma_high_clip:10;

	// h0058, bit: 14
	/* */
	unsigned int :6;

	// h0058
	unsigned int /* padding 16 bit */:16;

	// h0059, bit: 9
	/* */
	#define offset_of_isp5_reg_sobelv_1st_low_clip (178)
	#define mask_of_isp5_reg_sobelv_1st_low_clip (0x3ff)
	unsigned int reg_sobelv_1st_low_clip:10;

	// h0059, bit: 14
	/* */
	unsigned int :6;

	// h0059
	unsigned int /* padding 16 bit */:16;

	// h005a, bit: 9
	/* */
	#define offset_of_isp5_reg_sobelv_1st_high_clip (180)
	#define mask_of_isp5_reg_sobelv_1st_high_clip (0x3ff)
	unsigned int reg_sobelv_1st_high_clip:10;

	// h005a, bit: 14
	/* */
	unsigned int :6;

	// h005a
	unsigned int /* padding 16 bit */:16;

	// h005b, bit: 9
	/* */
	#define offset_of_isp5_reg_sobelv_2nd_low_clip (182)
	#define mask_of_isp5_reg_sobelv_2nd_low_clip (0x3ff)
	unsigned int reg_sobelv_2nd_low_clip:10;

	// h005b, bit: 14
	/* */
	unsigned int :6;

	// h005b
	unsigned int /* padding 16 bit */:16;

	// h005c, bit: 9
	/* */
	#define offset_of_isp5_reg_sobelv_2nd_high_clip (184)
	#define mask_of_isp5_reg_sobelv_2nd_high_clip (0x3ff)
	unsigned int reg_sobelv_2nd_high_clip:10;

	// h005c, bit: 14
	/* */
	unsigned int :6;

	// h005c
	unsigned int /* padding 16 bit */:16;

	// h005d, bit: 9
	/* */
	#define offset_of_isp5_reg_sobelh_1st_low_clip (186)
	#define mask_of_isp5_reg_sobelh_1st_low_clip (0x3ff)
	unsigned int reg_sobelh_1st_low_clip:10;

	// h005d, bit: 14
	/* */
	unsigned int :6;

	// h005d
	unsigned int /* padding 16 bit */:16;

	// h005e, bit: 9
	/* */
	#define offset_of_isp5_reg_sobelh_1st_high_clip (188)
	#define mask_of_isp5_reg_sobelh_1st_high_clip (0x3ff)
	unsigned int reg_sobelh_1st_high_clip:10;

	// h005e, bit: 14
	/* */
	unsigned int :6;

	// h005e
	unsigned int /* padding 16 bit */:16;

	// h005f, bit: 9
	/* */
	#define offset_of_isp5_reg_sobelh_2nd_low_clip (190)
	#define mask_of_isp5_reg_sobelh_2nd_low_clip (0x3ff)
	unsigned int reg_sobelh_2nd_low_clip:10;

	// h005f, bit: 14
	/* */
	unsigned int :6;

	// h005f
	unsigned int /* padding 16 bit */:16;

	// h0060, bit: 9
	/* */
	#define offset_of_isp5_reg_sobelh_2nd_high_clip (192)
	#define mask_of_isp5_reg_sobelh_2nd_high_clip (0x3ff)
	unsigned int reg_sobelh_2nd_high_clip:10;

	// h0060, bit: 14
	/* */
	unsigned int :6;

	// h0060
	unsigned int /* padding 16 bit */:16;

	// h0061, bit: 14
	/* */
	#define offset_of_isp5_reg_af_dma_addr (194)
	#define mask_of_isp5_reg_af_dma_addr (0xffff)
	unsigned int reg_af_dma_addr:16;

	// h0061
	unsigned int /* padding 16 bit */:16;

	// h0062, bit: 10
	/* */
	#define offset_of_isp5_reg_af_dma_addr_1 (196)
	#define mask_of_isp5_reg_af_dma_addr_1 (0x7ff)
	unsigned int reg_af_dma_addr_1:11;

	// h0062, bit: 14
	/* */
	unsigned int :5;

	// h0062
	unsigned int /* padding 16 bit */:16;

	// h0063, bit: 14
	/* */
	#define offset_of_isp5_reg_af_max_wadr (198)
	#define mask_of_isp5_reg_af_max_wadr (0xffff)
	unsigned int reg_af_max_wadr:16;

	// h0063
	unsigned int /* padding 16 bit */:16;

	// h0064, bit: 10
	/* */
	#define offset_of_isp5_reg_af_max_wadr_1 (200)
	#define mask_of_isp5_reg_af_max_wadr_1 (0x7ff)
	unsigned int reg_af_max_wadr_1:11;

	// h0064, bit: 14
	/* */
	unsigned int :5;

	// h0064
	unsigned int /* padding 16 bit */:16;

	// h0065, bit: 0
	/* */
	#define offset_of_isp5_reg_af_filter_int (202)
	#define mask_of_isp5_reg_af_filter_int (0x1)
	unsigned int reg_af_filter_int:1;

	// h0065, bit: 14
	/* */
	unsigned int :15;

	// h0065
	unsigned int /* padding 16 bit */:16;

	// h0066, bit: 9
	/* */
	#define offset_of_isp5_reg_sobelv_ythreshold (204)
	#define mask_of_isp5_reg_sobelv_ythreshold (0x3ff)
	unsigned int reg_sobelv_ythreshold:10;

	// h0066, bit: 14
	/* */
	unsigned int :6;

	// h0066
	unsigned int /* padding 16 bit */:16;

	// h0067, bit: 3
	/* Histo source selection
	4'b0000: from alsc (pipe0)
	4'b0001: from alsc (pipe1)
	4'b0010: no used
	4'b0011: no used
	4'b0100: from bwbg (pipe0)
	4'b0101: from bwbg (pipe1)
	4'b0110: no used
	4'b0111: no used */
	#define offset_of_isp5_reg_isp_af_in_mode (206)
	#define mask_of_isp5_reg_isp_af_in_mode (0xf)
	unsigned int reg_isp_af_in_mode:4;

	// h0067, bit: 14
	/* */
	unsigned int :12;

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

}  __attribute__((packed, aligned(1))) infinity2_reg_isp5;
#endif
