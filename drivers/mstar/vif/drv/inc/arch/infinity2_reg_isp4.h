// Generate Time: 2017-09-19 22:58:04.059588
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
#ifndef __INFINITY2_REG_ISP4__
#define __INFINITY2_REG_ISP4__
typedef struct {
	// h0000, bit: 0
	/* arb shape roi statis enable
	0: disable
	1: enable*/
	#define offset_of_isp4_reg_roi1_statis_en (0)
	#define mask_of_isp4_reg_roi1_statis_en (0x1)
	unsigned int reg_roi1_statis_en:1;

	// h0000, bit: 2
	/* arb shape source select
	00: before HDR
	01: after HDR stitch
	10: HDR output*/
	#define offset_of_isp4_reg_roi1_pipe_src (0)
	#define mask_of_isp4_reg_roi1_pipe_src (0x6)
	unsigned int reg_roi1_pipe_src:2;

	// h0000, bit: 3
	/* roi1 acc0 x skip mode enable
	0:disable
	1: enable*/
	#define offset_of_isp4_reg_roi1_x0_skip_en (0)
	#define mask_of_isp4_reg_roi1_x0_skip_en (0x8)
	unsigned int reg_roi1_x0_skip_en:1;

	// h0000, bit: 4
	/* roi1 acc0 y skip mode enable
	0:disable
	1: enable*/
	#define offset_of_isp4_reg_roi1_y0_skip_en (0)
	#define mask_of_isp4_reg_roi1_y0_skip_en (0x10)
	unsigned int reg_roi1_y0_skip_en:1;

	// h0000, bit: 5
	/* roi1 acc1 x skip mode enable
	0:disable
	1: enable*/
	#define offset_of_isp4_reg_roi1_x1_skip_en (0)
	#define mask_of_isp4_reg_roi1_x1_skip_en (0x20)
	unsigned int reg_roi1_x1_skip_en:1;

	// h0000, bit: 6
	/* roi1 acc1 y skip mode enable
	0:disable
	1: enable*/
	#define offset_of_isp4_reg_roi1_y1_skip_en (0)
	#define mask_of_isp4_reg_roi1_y1_skip_en (0x40)
	unsigned int reg_roi1_y1_skip_en:1;

	// h0000, bit: 7
	/* roi1 acc2 x skip mode enable
	0:disable
	1: enable*/
	#define offset_of_isp4_reg_roi1_x2_skip_en (0)
	#define mask_of_isp4_reg_roi1_x2_skip_en (0x80)
	unsigned int reg_roi1_x2_skip_en:1;

	// h0000, bit: 8
	/* roi1 acc2 y skip mode enable
	0:disable
	1: enable*/
	#define offset_of_isp4_reg_roi1_y2_skip_en (0)
	#define mask_of_isp4_reg_roi1_y2_skip_en (0x100)
	unsigned int reg_roi1_y2_skip_en:1;

	// h0000, bit: 9
	/* roi1 acc3 x skip mode enable
	0:disable
	1: enable*/
	#define offset_of_isp4_reg_roi1_x3_skip_en (0)
	#define mask_of_isp4_reg_roi1_x3_skip_en (0x200)
	unsigned int reg_roi1_x3_skip_en:1;

	// h0000, bit: 10
	/* roi1 acc3 y skip mode enable
	0:disable
	1: enable*/
	#define offset_of_isp4_reg_roi1_y3_skip_en (0)
	#define mask_of_isp4_reg_roi1_y3_skip_en (0x400)
	unsigned int reg_roi1_y3_skip_en:1;

	// h0000, bit: 11
	/* roi1 acc output data mask
	0: no mask, data write to register for user read*/
	#define offset_of_isp4_reg_roi1_mask_acc_out_en (0)
	#define mask_of_isp4_reg_roi1_mask_acc_out_en (0x800)
	unsigned int reg_roi1_mask_acc_out_en:1;

	// h0000, bit: 14
	/* */
	unsigned int :4;

	// h0000
	unsigned int /* padding 16 bit */:16;

	// h0001, bit: 12
	/* roi acc0 x0 position*/
	#define offset_of_isp4_reg_roi1_acc_0_0 (2)
	#define mask_of_isp4_reg_roi1_acc_0_0 (0x1fff)
	unsigned int reg_roi1_acc_0_0:13;

	// h0001, bit: 14
	/* */
	unsigned int :3;

	// h0001
	unsigned int /* padding 16 bit */:16;

	// h0002, bit: 12
	/* roi acc0 y0 position*/
	#define offset_of_isp4_reg_roi1_acc_0_1 (4)
	#define mask_of_isp4_reg_roi1_acc_0_1 (0x1fff)
	unsigned int reg_roi1_acc_0_1:13;

	// h0002, bit: 14
	/* */
	unsigned int :3;

	// h0002
	unsigned int /* padding 16 bit */:16;

	// h0003, bit: 12
	/* roi acc0 x1 position*/
	#define offset_of_isp4_reg_roi1_acc_0_2 (6)
	#define mask_of_isp4_reg_roi1_acc_0_2 (0x1fff)
	unsigned int reg_roi1_acc_0_2:13;

	// h0003, bit: 14
	/* */
	unsigned int :3;

	// h0003
	unsigned int /* padding 16 bit */:16;

	// h0004, bit: 12
	/* roi acc0 y1 position*/
	#define offset_of_isp4_reg_roi1_acc_0_3 (8)
	#define mask_of_isp4_reg_roi1_acc_0_3 (0x1fff)
	unsigned int reg_roi1_acc_0_3:13;

	// h0004, bit: 14
	/* */
	unsigned int :3;

	// h0004
	unsigned int /* padding 16 bit */:16;

	// h0005, bit: 12
	/* roi acc0 x2 position*/
	#define offset_of_isp4_reg_roi1_acc_0_4 (10)
	#define mask_of_isp4_reg_roi1_acc_0_4 (0x1fff)
	unsigned int reg_roi1_acc_0_4:13;

	// h0005, bit: 14
	/* */
	unsigned int :3;

	// h0005
	unsigned int /* padding 16 bit */:16;

	// h0006, bit: 12
	/* roi acc0 y2 position*/
	#define offset_of_isp4_reg_roi1_acc_0_5 (12)
	#define mask_of_isp4_reg_roi1_acc_0_5 (0x1fff)
	unsigned int reg_roi1_acc_0_5:13;

	// h0006, bit: 14
	/* */
	unsigned int :3;

	// h0006
	unsigned int /* padding 16 bit */:16;

	// h0007, bit: 12
	/* roi acc0 x3 position*/
	#define offset_of_isp4_reg_roi1_acc_0_6 (14)
	#define mask_of_isp4_reg_roi1_acc_0_6 (0x1fff)
	unsigned int reg_roi1_acc_0_6:13;

	// h0007, bit: 14
	/* */
	unsigned int :3;

	// h0007
	unsigned int /* padding 16 bit */:16;

	// h0008, bit: 12
	/* roi acc0 y3 position*/
	#define offset_of_isp4_reg_roi1_acc_0_7 (16)
	#define mask_of_isp4_reg_roi1_acc_0_7 (0x1fff)
	unsigned int reg_roi1_acc_0_7:13;

	// h0008, bit: 14
	/* */
	unsigned int :3;

	// h0008
	unsigned int /* padding 16 bit */:16;

	// h0009, bit: 14
	/* r acc0 output*/
	#define offset_of_isp4_reg_roi1_r_acc_0 (18)
	#define mask_of_isp4_reg_roi1_r_acc_0 (0xffff)
	unsigned int reg_roi1_r_acc_0:16;

	// h0009
	unsigned int /* padding 16 bit */:16;

	// h000a, bit: 14
	/* r acc0 output*/
	#define offset_of_isp4_reg_roi1_r_acc_0_1 (20)
	#define mask_of_isp4_reg_roi1_r_acc_0_1 (0xffff)
	unsigned int reg_roi1_r_acc_0_1:16;

	// h000a
	unsigned int /* padding 16 bit */:16;

	// h000b, bit: 14
	/* g acc0 output*/
	#define offset_of_isp4_reg_roi1_g_acc_0 (22)
	#define mask_of_isp4_reg_roi1_g_acc_0 (0xffff)
	unsigned int reg_roi1_g_acc_0:16;

	// h000b
	unsigned int /* padding 16 bit */:16;

	// h000c, bit: 14
	/* g acc0 output*/
	#define offset_of_isp4_reg_roi1_g_acc_0_1 (24)
	#define mask_of_isp4_reg_roi1_g_acc_0_1 (0xffff)
	unsigned int reg_roi1_g_acc_0_1:16;

	// h000c
	unsigned int /* padding 16 bit */:16;

	// h000d, bit: 14
	/* b acc0 output*/
	#define offset_of_isp4_reg_roi1_b_acc_0 (26)
	#define mask_of_isp4_reg_roi1_b_acc_0 (0xffff)
	unsigned int reg_roi1_b_acc_0:16;

	// h000d
	unsigned int /* padding 16 bit */:16;

	// h000e, bit: 14
	/* b acc0 output*/
	#define offset_of_isp4_reg_roi1_b_acc_0_1 (28)
	#define mask_of_isp4_reg_roi1_b_acc_0_1 (0xffff)
	unsigned int reg_roi1_b_acc_0_1:16;

	// h000e
	unsigned int /* padding 16 bit */:16;

	// h000f, bit: 14
	/* y acc0 output*/
	#define offset_of_isp4_reg_roi1_y_acc_0 (30)
	#define mask_of_isp4_reg_roi1_y_acc_0 (0xffff)
	unsigned int reg_roi1_y_acc_0:16;

	// h000f
	unsigned int /* padding 16 bit */:16;

	// h0010, bit: 14
	/* y acc0 output*/
	#define offset_of_isp4_reg_roi1_y_acc_0_1 (32)
	#define mask_of_isp4_reg_roi1_y_acc_0_1 (0xffff)
	unsigned int reg_roi1_y_acc_0_1:16;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 14
	/* y cnt0 output*/
	#define offset_of_isp4_reg_roi1_y_cnt_0 (34)
	#define mask_of_isp4_reg_roi1_y_cnt_0 (0xffff)
	unsigned int reg_roi1_y_cnt_0:16;

	// h0011
	unsigned int /* padding 16 bit */:16;

	// h0012, bit: 5
	/* y cnt0 output*/
	#define offset_of_isp4_reg_roi1_y_cnt_0_1 (36)
	#define mask_of_isp4_reg_roi1_y_cnt_0_1 (0x3f)
	unsigned int reg_roi1_y_cnt_0_1:6;

	// h0012, bit: 14
	/* */
	unsigned int :10;

	// h0012
	unsigned int /* padding 16 bit */:16;

	// h0013, bit: 12
	/* roi acc1 x0 position*/
	#define offset_of_isp4_reg_roi1_acc_1_0 (38)
	#define mask_of_isp4_reg_roi1_acc_1_0 (0x1fff)
	unsigned int reg_roi1_acc_1_0:13;

	// h0013, bit: 14
	/* */
	unsigned int :3;

	// h0013
	unsigned int /* padding 16 bit */:16;

	// h0014, bit: 12
	/* roi acc1 y0 position*/
	#define offset_of_isp4_reg_roi1_acc_1_1 (40)
	#define mask_of_isp4_reg_roi1_acc_1_1 (0x1fff)
	unsigned int reg_roi1_acc_1_1:13;

	// h0014, bit: 14
	/* */
	unsigned int :3;

	// h0014
	unsigned int /* padding 16 bit */:16;

	// h0015, bit: 12
	/* roi acc1 x1 position*/
	#define offset_of_isp4_reg_roi1_acc_1_2 (42)
	#define mask_of_isp4_reg_roi1_acc_1_2 (0x1fff)
	unsigned int reg_roi1_acc_1_2:13;

	// h0015, bit: 14
	/* */
	unsigned int :3;

	// h0015
	unsigned int /* padding 16 bit */:16;

	// h0016, bit: 12
	/* roi acc1 y1 position*/
	#define offset_of_isp4_reg_roi1_acc_1_3 (44)
	#define mask_of_isp4_reg_roi1_acc_1_3 (0x1fff)
	unsigned int reg_roi1_acc_1_3:13;

	// h0016, bit: 14
	/* */
	unsigned int :3;

	// h0016
	unsigned int /* padding 16 bit */:16;

	// h0017, bit: 12
	/* roi acc1 x2 position*/
	#define offset_of_isp4_reg_roi1_acc_1_4 (46)
	#define mask_of_isp4_reg_roi1_acc_1_4 (0x1fff)
	unsigned int reg_roi1_acc_1_4:13;

	// h0017, bit: 14
	/* */
	unsigned int :3;

	// h0017
	unsigned int /* padding 16 bit */:16;

	// h0018, bit: 12
	/* roi acc1 y2 position*/
	#define offset_of_isp4_reg_roi1_acc_1_5 (48)
	#define mask_of_isp4_reg_roi1_acc_1_5 (0x1fff)
	unsigned int reg_roi1_acc_1_5:13;

	// h0018, bit: 14
	/* */
	unsigned int :3;

	// h0018
	unsigned int /* padding 16 bit */:16;

	// h0019, bit: 12
	/* roi acc1 x3 position*/
	#define offset_of_isp4_reg_roi1_acc_1_6 (50)
	#define mask_of_isp4_reg_roi1_acc_1_6 (0x1fff)
	unsigned int reg_roi1_acc_1_6:13;

	// h0019, bit: 14
	/* */
	unsigned int :3;

	// h0019
	unsigned int /* padding 16 bit */:16;

	// h001a, bit: 12
	/* roi acc1 y3 position*/
	#define offset_of_isp4_reg_roi1_acc_1_7 (52)
	#define mask_of_isp4_reg_roi1_acc_1_7 (0x1fff)
	unsigned int reg_roi1_acc_1_7:13;

	// h001a, bit: 14
	/* */
	unsigned int :3;

	// h001a
	unsigned int /* padding 16 bit */:16;

	// h001b, bit: 14
	/* r acc1 output*/
	#define offset_of_isp4_reg_roi1_r_acc_1 (54)
	#define mask_of_isp4_reg_roi1_r_acc_1 (0xffff)
	unsigned int reg_roi1_r_acc_1:16;

	// h001b
	unsigned int /* padding 16 bit */:16;

	// h001c, bit: 14
	/* r acc1 output*/
	#define offset_of_isp4_reg_roi1_r_acc_1_1 (56)
	#define mask_of_isp4_reg_roi1_r_acc_1_1 (0xffff)
	unsigned int reg_roi1_r_acc_1_1:16;

	// h001c
	unsigned int /* padding 16 bit */:16;

	// h001d, bit: 14
	/* g acc1 output*/
	#define offset_of_isp4_reg_roi1_g_acc_1 (58)
	#define mask_of_isp4_reg_roi1_g_acc_1 (0xffff)
	unsigned int reg_roi1_g_acc_1:16;

	// h001d
	unsigned int /* padding 16 bit */:16;

	// h001e, bit: 14
	/* g acc1 output*/
	#define offset_of_isp4_reg_roi1_g_acc_1_1 (60)
	#define mask_of_isp4_reg_roi1_g_acc_1_1 (0xffff)
	unsigned int reg_roi1_g_acc_1_1:16;

	// h001e
	unsigned int /* padding 16 bit */:16;

	// h001f, bit: 14
	/* b acc1 output*/
	#define offset_of_isp4_reg_roi1_b_acc_1 (62)
	#define mask_of_isp4_reg_roi1_b_acc_1 (0xffff)
	unsigned int reg_roi1_b_acc_1:16;

	// h001f
	unsigned int /* padding 16 bit */:16;

	// h0020, bit: 14
	/* b acc1 output*/
	#define offset_of_isp4_reg_roi1_b_acc_1_1 (64)
	#define mask_of_isp4_reg_roi1_b_acc_1_1 (0xffff)
	unsigned int reg_roi1_b_acc_1_1:16;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 14
	/* y acc1 output*/
	#define offset_of_isp4_reg_roi1_y_acc_1 (66)
	#define mask_of_isp4_reg_roi1_y_acc_1 (0xffff)
	unsigned int reg_roi1_y_acc_1:16;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 14
	/* y acc1 output*/
	#define offset_of_isp4_reg_roi1_y_acc_1_1 (68)
	#define mask_of_isp4_reg_roi1_y_acc_1_1 (0xffff)
	unsigned int reg_roi1_y_acc_1_1:16;

	// h0022
	unsigned int /* padding 16 bit */:16;

	// h0023, bit: 14
	/* y cnt1 output*/
	#define offset_of_isp4_reg_roi1_y_cnt_1 (70)
	#define mask_of_isp4_reg_roi1_y_cnt_1 (0xffff)
	unsigned int reg_roi1_y_cnt_1:16;

	// h0023
	unsigned int /* padding 16 bit */:16;

	// h0024, bit: 5
	/* y cnt1 output*/
	#define offset_of_isp4_reg_roi1_y_cnt_1_1 (72)
	#define mask_of_isp4_reg_roi1_y_cnt_1_1 (0x3f)
	unsigned int reg_roi1_y_cnt_1_1:6;

	// h0024, bit: 14
	/* */
	unsigned int :10;

	// h0024
	unsigned int /* padding 16 bit */:16;

	// h0025, bit: 12
	/* roi acc2 x0 position*/
	#define offset_of_isp4_reg_roi1_acc_2_0 (74)
	#define mask_of_isp4_reg_roi1_acc_2_0 (0x1fff)
	unsigned int reg_roi1_acc_2_0:13;

	// h0025, bit: 14
	/* */
	unsigned int :3;

	// h0025
	unsigned int /* padding 16 bit */:16;

	// h0026, bit: 12
	/* roi acc2 y0 position*/
	#define offset_of_isp4_reg_roi1_acc_2_1 (76)
	#define mask_of_isp4_reg_roi1_acc_2_1 (0x1fff)
	unsigned int reg_roi1_acc_2_1:13;

	// h0026, bit: 14
	/* */
	unsigned int :3;

	// h0026
	unsigned int /* padding 16 bit */:16;

	// h0027, bit: 12
	/* roi acc2 x1 position*/
	#define offset_of_isp4_reg_roi1_acc_2_2 (78)
	#define mask_of_isp4_reg_roi1_acc_2_2 (0x1fff)
	unsigned int reg_roi1_acc_2_2:13;

	// h0027, bit: 14
	/* */
	unsigned int :3;

	// h0027
	unsigned int /* padding 16 bit */:16;

	// h0028, bit: 12
	/* roi acc2 y1 position*/
	#define offset_of_isp4_reg_roi1_acc_2_3 (80)
	#define mask_of_isp4_reg_roi1_acc_2_3 (0x1fff)
	unsigned int reg_roi1_acc_2_3:13;

	// h0028, bit: 14
	/* */
	unsigned int :3;

	// h0028
	unsigned int /* padding 16 bit */:16;

	// h0029, bit: 12
	/* roi acc2 x2 position*/
	#define offset_of_isp4_reg_roi1_acc_2_4 (82)
	#define mask_of_isp4_reg_roi1_acc_2_4 (0x1fff)
	unsigned int reg_roi1_acc_2_4:13;

	// h0029, bit: 14
	/* */
	unsigned int :3;

	// h0029
	unsigned int /* padding 16 bit */:16;

	// h002a, bit: 12
	/* roi acc2 y2 position*/
	#define offset_of_isp4_reg_roi1_acc_2_5 (84)
	#define mask_of_isp4_reg_roi1_acc_2_5 (0x1fff)
	unsigned int reg_roi1_acc_2_5:13;

	// h002a, bit: 14
	/* */
	unsigned int :3;

	// h002a
	unsigned int /* padding 16 bit */:16;

	// h002b, bit: 12
	/* roi acc2 x3 position*/
	#define offset_of_isp4_reg_roi1_acc_2_6 (86)
	#define mask_of_isp4_reg_roi1_acc_2_6 (0x1fff)
	unsigned int reg_roi1_acc_2_6:13;

	// h002b, bit: 14
	/* */
	unsigned int :3;

	// h002b
	unsigned int /* padding 16 bit */:16;

	// h002c, bit: 12
	/* roi acc2 y3 position*/
	#define offset_of_isp4_reg_roi1_acc_2_7 (88)
	#define mask_of_isp4_reg_roi1_acc_2_7 (0x1fff)
	unsigned int reg_roi1_acc_2_7:13;

	// h002c, bit: 14
	/* */
	unsigned int :3;

	// h002c
	unsigned int /* padding 16 bit */:16;

	// h002d, bit: 14
	/* r acc2 output*/
	#define offset_of_isp4_reg_roi1_r_acc_2 (90)
	#define mask_of_isp4_reg_roi1_r_acc_2 (0xffff)
	unsigned int reg_roi1_r_acc_2:16;

	// h002d
	unsigned int /* padding 16 bit */:16;

	// h002e, bit: 14
	/* r acc2 output*/
	#define offset_of_isp4_reg_roi1_r_acc_2_1 (92)
	#define mask_of_isp4_reg_roi1_r_acc_2_1 (0xffff)
	unsigned int reg_roi1_r_acc_2_1:16;

	// h002e
	unsigned int /* padding 16 bit */:16;

	// h002f, bit: 14
	/* g acc2 output*/
	#define offset_of_isp4_reg_roi1_g_acc_2 (94)
	#define mask_of_isp4_reg_roi1_g_acc_2 (0xffff)
	unsigned int reg_roi1_g_acc_2:16;

	// h002f
	unsigned int /* padding 16 bit */:16;

	// h0030, bit: 14
	/* g acc2 output*/
	#define offset_of_isp4_reg_roi1_g_acc_2_1 (96)
	#define mask_of_isp4_reg_roi1_g_acc_2_1 (0xffff)
	unsigned int reg_roi1_g_acc_2_1:16;

	// h0030
	unsigned int /* padding 16 bit */:16;

	// h0031, bit: 14
	/* b acc2 output*/
	#define offset_of_isp4_reg_roi1_b_acc_2 (98)
	#define mask_of_isp4_reg_roi1_b_acc_2 (0xffff)
	unsigned int reg_roi1_b_acc_2:16;

	// h0031
	unsigned int /* padding 16 bit */:16;

	// h0032, bit: 14
	/* b acc2 output*/
	#define offset_of_isp4_reg_roi1_b_acc_2_1 (100)
	#define mask_of_isp4_reg_roi1_b_acc_2_1 (0xffff)
	unsigned int reg_roi1_b_acc_2_1:16;

	// h0032
	unsigned int /* padding 16 bit */:16;

	// h0033, bit: 14
	/* y acc2 output*/
	#define offset_of_isp4_reg_roi1_y_acc_2 (102)
	#define mask_of_isp4_reg_roi1_y_acc_2 (0xffff)
	unsigned int reg_roi1_y_acc_2:16;

	// h0033
	unsigned int /* padding 16 bit */:16;

	// h0034, bit: 14
	/* y acc2 output*/
	#define offset_of_isp4_reg_roi1_y_acc_2_1 (104)
	#define mask_of_isp4_reg_roi1_y_acc_2_1 (0xffff)
	unsigned int reg_roi1_y_acc_2_1:16;

	// h0034
	unsigned int /* padding 16 bit */:16;

	// h0035, bit: 14
	/* y cnt2 output*/
	#define offset_of_isp4_reg_roi1_y_cnt_2 (106)
	#define mask_of_isp4_reg_roi1_y_cnt_2 (0xffff)
	unsigned int reg_roi1_y_cnt_2:16;

	// h0035
	unsigned int /* padding 16 bit */:16;

	// h0036, bit: 5
	/* y cnt2 output*/
	#define offset_of_isp4_reg_roi1_y_cnt_2_1 (108)
	#define mask_of_isp4_reg_roi1_y_cnt_2_1 (0x3f)
	unsigned int reg_roi1_y_cnt_2_1:6;

	// h0036, bit: 14
	/* */
	unsigned int :10;

	// h0036
	unsigned int /* padding 16 bit */:16;

	// h0037, bit: 14
	/* roi ratio for src0*/
	#define offset_of_isp4_reg_roi1_ratio (110)
	#define mask_of_isp4_reg_roi1_ratio (0xffff)
	unsigned int reg_roi1_ratio:16;

	// h0037
	unsigned int /* padding 16 bit */:16;

	// h0038, bit: 7
	/* roi bayer to y r value*/
	#define offset_of_isp4_reg_roi1_r_value (112)
	#define mask_of_isp4_reg_roi1_r_value (0xff)
	unsigned int reg_roi1_r_value:8;

	// h0038, bit: 8
	/* roi bayer to y r value sign bit*/
	#define offset_of_isp4_reg_roi1_r_sign (112)
	#define mask_of_isp4_reg_roi1_r_sign (0x100)
	unsigned int reg_roi1_r_sign:1;

	// h0038, bit: 14
	/* */
	unsigned int :7;

	// h0038
	unsigned int /* padding 16 bit */:16;

	// h0039, bit: 7
	/* roi bayer to y gr value*/
	#define offset_of_isp4_reg_roi1_gr_value (114)
	#define mask_of_isp4_reg_roi1_gr_value (0xff)
	unsigned int reg_roi1_gr_value:8;

	// h0039, bit: 8
	/* roi bayer to y gr value sign bit*/
	#define offset_of_isp4_reg_roi1_gr_sign (114)
	#define mask_of_isp4_reg_roi1_gr_sign (0x100)
	unsigned int reg_roi1_gr_sign:1;

	// h0039, bit: 14
	/* */
	unsigned int :7;

	// h0039
	unsigned int /* padding 16 bit */:16;

	// h003a, bit: 7
	/* roi bayer to y gb value*/
	#define offset_of_isp4_reg_roi1_gb_value (116)
	#define mask_of_isp4_reg_roi1_gb_value (0xff)
	unsigned int reg_roi1_gb_value:8;

	// h003a, bit: 8
	/* roi bayer to y gb value sign bit*/
	#define offset_of_isp4_reg_roi1_gb_sign (116)
	#define mask_of_isp4_reg_roi1_gb_sign (0x100)
	unsigned int reg_roi1_gb_sign:1;

	// h003a, bit: 14
	/* */
	unsigned int :7;

	// h003a
	unsigned int /* padding 16 bit */:16;

	// h003b, bit: 7
	/* roi bayer to y b value*/
	#define offset_of_isp4_reg_roi1_b_value (118)
	#define mask_of_isp4_reg_roi1_b_value (0xff)
	unsigned int reg_roi1_b_value:8;

	// h003b, bit: 8
	/* roi bayer to y b value sign bit*/
	#define offset_of_isp4_reg_roi1_b_sign (118)
	#define mask_of_isp4_reg_roi1_b_sign (0x100)
	unsigned int reg_roi1_b_sign:1;

	// h003b, bit: 14
	/* */
	unsigned int :7;

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

	// h0040, bit: 0
	/* fpn enable*/
	#define offset_of_isp4_reg_fpn_en (128)
	#define mask_of_isp4_reg_fpn_en (0x1)
	unsigned int reg_fpn_en:1;

	// h0040, bit: 1
	/* 0: statis for every frame set 1: statis for just first frame set*/
	#define offset_of_isp4_reg_fpn_mode (128)
	#define mask_of_isp4_reg_fpn_mode (0x2)
	unsigned int reg_fpn_mode:1;

	// h0040, bit: 2
	/* 0:don’t clear sum result 1:  clear sum result*/
	#define offset_of_isp4_reg_fpn_sum_clr (128)
	#define mask_of_isp4_reg_fpn_sum_clr (0x4)
	unsigned int reg_fpn_sum_clr:1;

	// h0040, bit: 3
	/* SW setting eable*/
	#define offset_of_isp4_reg_sw_offset_on (128)
	#define mask_of_isp4_reg_sw_offset_on (0x8)
	unsigned int reg_sw_offset_on:1;

	// h0040, bit: 6
	/* */
	unsigned int :3;

	// h0040, bit: 7
	/* sensor hsync line start enable*/
	#define offset_of_isp4_reg_fpn_sensor_line_start (128)
	#define mask_of_isp4_reg_fpn_sensor_line_start (0x80)
	unsigned int reg_fpn_sensor_line_start:1;

	// h0040, bit: 14
	/* */
	unsigned int :8;

	// h0040
	unsigned int /* padding 16 bit */:16;

	// h0041, bit: 12
	/* ob start x */
	#define offset_of_isp4_reg_fpn_start_x (130)
	#define mask_of_isp4_reg_fpn_start_x (0x1fff)
	unsigned int reg_fpn_start_x:13;

	// h0041, bit: 14
	/* */
	unsigned int :3;

	// h0041
	unsigned int /* padding 16 bit */:16;

	// h0042, bit: 12
	/* ob start xy*/
	#define offset_of_isp4_reg_fpn_start_y (132)
	#define mask_of_isp4_reg_fpn_start_y (0x1fff)
	unsigned int reg_fpn_start_y:13;

	// h0042, bit: 14
	/* */
	unsigned int :3;

	// h0042
	unsigned int /* padding 16 bit */:16;

	// h0043, bit: 12
	/* ob width*/
	#define offset_of_isp4_reg_fpn_width (134)
	#define mask_of_isp4_reg_fpn_width (0x1fff)
	unsigned int reg_fpn_width:13;

	// h0043, bit: 14
	/* */
	unsigned int :3;

	// h0043
	unsigned int /* padding 16 bit */:16;

	// h0044, bit: 4
	/* ob height*/
	#define offset_of_isp4_reg_fpn_height (136)
	#define mask_of_isp4_reg_fpn_height (0x1f)
	unsigned int reg_fpn_height:5;

	// h0044, bit: 14
	/* */
	unsigned int :11;

	// h0044
	unsigned int /* padding 16 bit */:16;

	// h0045, bit: 14
	/* pre-offset for pixels at active area*/
	#define offset_of_isp4_reg_fpn_preoffset (138)
	#define mask_of_isp4_reg_fpn_preoffset (0x7fff)
	unsigned int reg_fpn_preoffset:15;

	// h0045, bit: 15
	/* sign of pre-offset */
	#define offset_of_isp4_reg_fpn_preoffset_sign (138)
	#define mask_of_isp4_reg_fpn_preoffset_sign (0x8000)
	unsigned int reg_fpn_preoffset_sign:1;

	// h0045
	unsigned int /* padding 16 bit */:16;

	// h0046, bit: 14
	/* midium pixel and pavg diff threshould*/
	#define offset_of_isp4_reg_fpn_diff_th (140)
	#define mask_of_isp4_reg_fpn_diff_th (0xffff)
	unsigned int reg_fpn_diff_th:16;

	// h0046
	unsigned int /* padding 16 bit */:16;

	// h0047, bit: 11
	/* compare ratio*/
	#define offset_of_isp4_reg_fpn_cmp_ratio (142)
	#define mask_of_isp4_reg_fpn_cmp_ratio (0xfff)
	unsigned int reg_fpn_cmp_ratio:12;

	// h0047, bit: 14
	/* */
	unsigned int :4;

	// h0047
	unsigned int /* padding 16 bit */:16;

	// h0048, bit: 4
	/* statis frame number*/
	#define offset_of_isp4_reg_fpn_frame_num (144)
	#define mask_of_isp4_reg_fpn_frame_num (0x1f)
	unsigned int reg_fpn_frame_num:5;

	// h0048, bit: 14
	/* */
	unsigned int :11;

	// h0048
	unsigned int /* padding 16 bit */:16;

	// h0049, bit: 14
	/* SW offset setting write data*/
	#define offset_of_isp4_reg_sw_offset_wd (146)
	#define mask_of_isp4_reg_sw_offset_wd (0xffff)
	unsigned int reg_sw_offset_wd:16;

	// h0049
	unsigned int /* padding 16 bit */:16;

	// h004a, bit: 12
	/* SW offset setting write address*/
	#define offset_of_isp4_reg_sw_offset_a (148)
	#define mask_of_isp4_reg_sw_offset_a (0x1fff)
	unsigned int reg_sw_offset_a:13;

	// h004a, bit: 14
	/* */
	unsigned int :3;

	// h004a
	unsigned int /* padding 16 bit */:16;

	// h004b, bit: 0
	/* SW offset setting write control*/
	#define offset_of_isp4_reg_sw_offset_we (150)
	#define mask_of_isp4_reg_sw_offset_we (0x1)
	unsigned int reg_sw_offset_we:1;

	// h004b, bit: 1
	/* SW setting eable*/
	#define offset_of_isp4_reg_sw_offset_re (150)
	#define mask_of_isp4_reg_sw_offset_re (0x2)
	unsigned int reg_sw_offset_re:1;

	// h004b, bit: 14
	/* */
	unsigned int :14;

	// h004b
	unsigned int /* padding 16 bit */:16;

	// h004c, bit: 14
	/* SW setting read data*/
	#define offset_of_isp4_reg_sw_offset_rd (152)
	#define mask_of_isp4_reg_sw_offset_rd (0xffff)
	unsigned int reg_sw_offset_rd:16;

	// h004c
	unsigned int /* padding 16 bit */:16;

	// h004d, bit: 12
	/* sensor width register, width -1
	active when reg_fpn_sensor_line_start enable*/
	#define offset_of_isp4_reg_fpn_sensor_width (154)
	#define mask_of_isp4_reg_fpn_sensor_width (0x1fff)
	unsigned int reg_fpn_sensor_width:13;

	// h004d, bit: 14
	/* */
	unsigned int :3;

	// h004d
	unsigned int /* padding 16 bit */:16;

	// h004e, bit: 12
	/* sensor height register,  height -1
	active when reg_fpn_sensor_line_start enable*/
	#define offset_of_isp4_reg_fpn_sensor_height (156)
	#define mask_of_isp4_reg_fpn_sensor_height (0x1fff)
	unsigned int reg_fpn_sensor_height:13;

	// h004e, bit: 14
	/* */
	unsigned int :3;

	// h004e
	unsigned int /* padding 16 bit */:16;

	// h004f, bit: 14
	/* */
	unsigned int :16;

	// h004f
	unsigned int /* padding 16 bit */:16;

	// h0050, bit: 12
	/* roi acc3 x0 position*/
	#define offset_of_isp4_reg_roi1_acc_3_0 (160)
	#define mask_of_isp4_reg_roi1_acc_3_0 (0x1fff)
	unsigned int reg_roi1_acc_3_0:13;

	// h0050, bit: 14
	/* */
	unsigned int :3;

	// h0050
	unsigned int /* padding 16 bit */:16;

	// h0051, bit: 12
	/* roi acc3 y0 position*/
	#define offset_of_isp4_reg_roi1_acc_3_1 (162)
	#define mask_of_isp4_reg_roi1_acc_3_1 (0x1fff)
	unsigned int reg_roi1_acc_3_1:13;

	// h0051, bit: 14
	/* */
	unsigned int :3;

	// h0051
	unsigned int /* padding 16 bit */:16;

	// h0052, bit: 12
	/* roi acc3 x1 position*/
	#define offset_of_isp4_reg_roi1_acc_3_2 (164)
	#define mask_of_isp4_reg_roi1_acc_3_2 (0x1fff)
	unsigned int reg_roi1_acc_3_2:13;

	// h0052, bit: 14
	/* */
	unsigned int :3;

	// h0052
	unsigned int /* padding 16 bit */:16;

	// h0053, bit: 12
	/* roi acc3 y1 position*/
	#define offset_of_isp4_reg_roi1_acc_3_3 (166)
	#define mask_of_isp4_reg_roi1_acc_3_3 (0x1fff)
	unsigned int reg_roi1_acc_3_3:13;

	// h0053, bit: 14
	/* */
	unsigned int :3;

	// h0053
	unsigned int /* padding 16 bit */:16;

	// h0054, bit: 12
	/* roi acc3 x2 position*/
	#define offset_of_isp4_reg_roi1_acc_3_4 (168)
	#define mask_of_isp4_reg_roi1_acc_3_4 (0x1fff)
	unsigned int reg_roi1_acc_3_4:13;

	// h0054, bit: 14
	/* */
	unsigned int :3;

	// h0054
	unsigned int /* padding 16 bit */:16;

	// h0055, bit: 12
	/* roi acc3 y2 position*/
	#define offset_of_isp4_reg_roi1_acc_3_5 (170)
	#define mask_of_isp4_reg_roi1_acc_3_5 (0x1fff)
	unsigned int reg_roi1_acc_3_5:13;

	// h0055, bit: 14
	/* */
	unsigned int :3;

	// h0055
	unsigned int /* padding 16 bit */:16;

	// h0056, bit: 12
	/* roi acc3 x3 position*/
	#define offset_of_isp4_reg_roi1_acc_3_6 (172)
	#define mask_of_isp4_reg_roi1_acc_3_6 (0x1fff)
	unsigned int reg_roi1_acc_3_6:13;

	// h0056, bit: 14
	/* */
	unsigned int :3;

	// h0056
	unsigned int /* padding 16 bit */:16;

	// h0057, bit: 12
	/* roi acc3 y3 position*/
	#define offset_of_isp4_reg_roi1_acc_3_7 (174)
	#define mask_of_isp4_reg_roi1_acc_3_7 (0x1fff)
	unsigned int reg_roi1_acc_3_7:13;

	// h0057, bit: 14
	/* */
	unsigned int :3;

	// h0057
	unsigned int /* padding 16 bit */:16;

	// h0058, bit: 14
	/* r acc3 output*/
	#define offset_of_isp4_reg_roi1_r_acc_3 (176)
	#define mask_of_isp4_reg_roi1_r_acc_3 (0xffff)
	unsigned int reg_roi1_r_acc_3:16;

	// h0058
	unsigned int /* padding 16 bit */:16;

	// h0059, bit: 14
	/* r acc3 output*/
	#define offset_of_isp4_reg_roi1_r_acc_3_1 (178)
	#define mask_of_isp4_reg_roi1_r_acc_3_1 (0xffff)
	unsigned int reg_roi1_r_acc_3_1:16;

	// h0059
	unsigned int /* padding 16 bit */:16;

	// h005a, bit: 14
	/* g acc3 output*/
	#define offset_of_isp4_reg_roi1_g_acc_3 (180)
	#define mask_of_isp4_reg_roi1_g_acc_3 (0xffff)
	unsigned int reg_roi1_g_acc_3:16;

	// h005a
	unsigned int /* padding 16 bit */:16;

	// h005b, bit: 14
	/* g acc3 output*/
	#define offset_of_isp4_reg_roi1_g_acc_3_1 (182)
	#define mask_of_isp4_reg_roi1_g_acc_3_1 (0xffff)
	unsigned int reg_roi1_g_acc_3_1:16;

	// h005b
	unsigned int /* padding 16 bit */:16;

	// h005c, bit: 14
	/* b acc3 output*/
	#define offset_of_isp4_reg_roi1_b_acc_3 (184)
	#define mask_of_isp4_reg_roi1_b_acc_3 (0xffff)
	unsigned int reg_roi1_b_acc_3:16;

	// h005c
	unsigned int /* padding 16 bit */:16;

	// h005d, bit: 14
	/* b acc3 output*/
	#define offset_of_isp4_reg_roi1_b_acc_3_1 (186)
	#define mask_of_isp4_reg_roi1_b_acc_3_1 (0xffff)
	unsigned int reg_roi1_b_acc_3_1:16;

	// h005d
	unsigned int /* padding 16 bit */:16;

	// h005e, bit: 14
	/* y acc3 output*/
	#define offset_of_isp4_reg_roi1_y_acc_3 (188)
	#define mask_of_isp4_reg_roi1_y_acc_3 (0xffff)
	unsigned int reg_roi1_y_acc_3:16;

	// h005e
	unsigned int /* padding 16 bit */:16;

	// h005f, bit: 14
	/* y acc3 output*/
	#define offset_of_isp4_reg_roi1_y_acc_3_1 (190)
	#define mask_of_isp4_reg_roi1_y_acc_3_1 (0xffff)
	unsigned int reg_roi1_y_acc_3_1:16;

	// h005f
	unsigned int /* padding 16 bit */:16;

	// h0060, bit: 14
	/* y cnt 3 output*/
	#define offset_of_isp4_reg_roi1_y_cnt_3 (192)
	#define mask_of_isp4_reg_roi1_y_cnt_3 (0xffff)
	unsigned int reg_roi1_y_cnt_3:16;

	// h0060
	unsigned int /* padding 16 bit */:16;

	// h0061, bit: 5
	/* y cnt 3 output*/
	#define offset_of_isp4_reg_roi1_y_cnt_3_1 (194)
	#define mask_of_isp4_reg_roi1_y_cnt_3_1 (0x3f)
	unsigned int reg_roi1_y_cnt_3_1:6;

	// h0061, bit: 14
	/* */
	unsigned int :10;

	// h0061
	unsigned int /* padding 16 bit */:16;

	// h0062, bit: 14
	/* roi1_0 histogram miu base address*/
	#define offset_of_isp4_reg_roi1_hist0_miu_addr (196)
	#define mask_of_isp4_reg_roi1_hist0_miu_addr (0xffff)
	unsigned int reg_roi1_hist0_miu_addr:16;

	// h0062
	unsigned int /* padding 16 bit */:16;

	// h0063, bit: 14
	/* roi1_0 histogram miu base address*/
	#define offset_of_isp4_reg_roi1_hist0_miu_addr_1 (198)
	#define mask_of_isp4_reg_roi1_hist0_miu_addr_1 (0xffff)
	unsigned int reg_roi1_hist0_miu_addr_1:16;

	// h0063
	unsigned int /* padding 16 bit */:16;

	// h0064, bit: 14
	/* roi1_1 histogram miu base address*/
	#define offset_of_isp4_reg_roi1_hist1_miu_addr (200)
	#define mask_of_isp4_reg_roi1_hist1_miu_addr (0xffff)
	unsigned int reg_roi1_hist1_miu_addr:16;

	// h0064
	unsigned int /* padding 16 bit */:16;

	// h0065, bit: 14
	/* roi1_1 histogram miu base address*/
	#define offset_of_isp4_reg_roi1_hist1_miu_addr_1 (202)
	#define mask_of_isp4_reg_roi1_hist1_miu_addr_1 (0xffff)
	unsigned int reg_roi1_hist1_miu_addr_1:16;

	// h0065
	unsigned int /* padding 16 bit */:16;

	// h0066, bit: 14
	/* roi1_2 histogram miu base address*/
	#define offset_of_isp4_reg_roi1_hist2_miu_addr (204)
	#define mask_of_isp4_reg_roi1_hist2_miu_addr (0xffff)
	unsigned int reg_roi1_hist2_miu_addr:16;

	// h0066
	unsigned int /* padding 16 bit */:16;

	// h0067, bit: 14
	/* roi1_2 histogram miu base address*/
	#define offset_of_isp4_reg_roi1_hist2_miu_addr_1 (206)
	#define mask_of_isp4_reg_roi1_hist2_miu_addr_1 (0xffff)
	unsigned int reg_roi1_hist2_miu_addr_1:16;

	// h0067
	unsigned int /* padding 16 bit */:16;

	// h0068, bit: 14
	/* roi1_3 histogram miu base address*/
	#define offset_of_isp4_reg_roi1_hist3_miu_addr (208)
	#define mask_of_isp4_reg_roi1_hist3_miu_addr (0xffff)
	unsigned int reg_roi1_hist3_miu_addr:16;

	// h0068
	unsigned int /* padding 16 bit */:16;

	// h0069, bit: 14
	/* roi1_3 histogram miu base address*/
	#define offset_of_isp4_reg_roi1_hist3_miu_addr_1 (210)
	#define mask_of_isp4_reg_roi1_hist3_miu_addr_1 (0xffff)
	unsigned int reg_roi1_hist3_miu_addr_1:16;

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

}  __attribute__((packed, aligned(1))) infinity2_reg_isp4;
#endif
