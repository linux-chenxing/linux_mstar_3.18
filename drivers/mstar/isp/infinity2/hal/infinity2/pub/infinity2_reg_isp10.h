// Generate Time: 2017-09-19 22:58:04.458413
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
#ifndef __INFINITY2_REG_ISP10__
#define __INFINITY2_REG_ISP10__
typedef struct {
	// h0000, bit: 0
	/* HDR enable
	0: disable
	1: enable*/
	#define offset_of_isp10_reg_hdr_en (0)
	#define mask_of_isp10_reg_hdr_en (0x1)
	unsigned int reg_hdr_en:1;

	// h0000, bit: 1
	/* HDR stitch enable
	0: disable
	1: enable*/
	#define offset_of_isp10_reg_hdr_stitch_en (0)
	#define mask_of_isp10_reg_hdr_stitch_en (0x2)
	unsigned int reg_hdr_stitch_en:1;

	// h0000, bit: 2
	/* HDR mapping enable
	0: disable
	1: enable*/
	#define offset_of_isp10_reg_hdr_mapping_en (0)
	#define mask_of_isp10_reg_hdr_mapping_en (0x4)
	unsigned int reg_hdr_mapping_en:1;

	// h0000, bit: 3
	/* HDR pipe0 data source
	0: from previous module
	1: from dma*/
	#define offset_of_isp10_reg_hdr_pipe0_dma_src (0)
	#define mask_of_isp10_reg_hdr_pipe0_dma_src (0x8)
	unsigned int reg_hdr_pipe0_dma_src:1;

	// h0000, bit: 4
	/* HDR pipe1 data source
	0: from previous module
	1: from dma*/
	#define offset_of_isp10_reg_hdr_pipe1_dma_src (0)
	#define mask_of_isp10_reg_hdr_pipe1_dma_src (0x10)
	unsigned int reg_hdr_pipe1_dma_src:1;

	// h0000, bit: 6
	/* HDR frame merge option
	01: 2 frame merge*/
	#define offset_of_isp10_reg_hdr_merge_option (0)
	#define mask_of_isp10_reg_hdr_merge_option (0x60)
	unsigned int reg_hdr_merge_option:2;

	// h0000, bit: 8
	/* HDR frame merge very short frame select
	00: from isp pipe0
	01: from isp pipe1*/
	#define offset_of_isp10_reg_hdr_vs_sel (0)
	#define mask_of_isp10_reg_hdr_vs_sel (0x180)
	unsigned int reg_hdr_vs_sel:2;

	// h0000, bit: 10
	/* HDR frame merge short frame select
	00: from isp pipe0
	01: from isp pipe1*/
	#define offset_of_isp10_reg_hdr_s_sel (0)
	#define mask_of_isp10_reg_hdr_s_sel (0x600)
	unsigned int reg_hdr_s_sel:2;

	// h0000, bit: 14
	/* */
	unsigned int :5;

	// h0000
	unsigned int /* padding 16 bit */:16;

	// h0001, bit: 14
	/* HDR ratio for very short frame*/
	#define offset_of_isp10_reg_hdr_ratio_vs (2)
	#define mask_of_isp10_reg_hdr_ratio_vs (0xffff)
	unsigned int reg_hdr_ratio_vs:16;

	// h0001
	unsigned int /* padding 16 bit */:16;

	// h0002, bit: 9
	/* HDR combine YUV th1 for vs and s frame*/
	#define offset_of_isp10_reg_hdr_th1_vs_s (4)
	#define mask_of_isp10_reg_hdr_th1_vs_s (0x3ff)
	unsigned int reg_hdr_th1_vs_s:10;

	// h0002, bit: 14
	/* */
	unsigned int :6;

	// h0002
	unsigned int /* padding 16 bit */:16;

	// h0003, bit: 9
	/* HDR combine YUV th2 for vs and s frame*/
	#define offset_of_isp10_reg_hdr_th2_vs_s (6)
	#define mask_of_isp10_reg_hdr_th2_vs_s (0x3ff)
	unsigned int reg_hdr_th2_vs_s:10;

	// h0003, bit: 14
	/* */
	unsigned int :6;

	// h0003
	unsigned int /* padding 16 bit */:16;

	// h0004, bit: 3
	/* HDR UV lut table x0*/
	#define offset_of_isp10_reg_hdr_uvtbl_x0 (8)
	#define mask_of_isp10_reg_hdr_uvtbl_x0 (0xf)
	unsigned int reg_hdr_uvtbl_x0:4;

	// h0004, bit: 7
	/* HDR UV lut table x1*/
	#define offset_of_isp10_reg_hdr_uvtbl_x1 (8)
	#define mask_of_isp10_reg_hdr_uvtbl_x1 (0xf0)
	unsigned int reg_hdr_uvtbl_x1:4;

	// h0004, bit: 11
	/* HDR UV lut table x2*/
	#define offset_of_isp10_reg_hdr_uvtbl_x2 (8)
	#define mask_of_isp10_reg_hdr_uvtbl_x2 (0xf00)
	unsigned int reg_hdr_uvtbl_x2:4;

	// h0004, bit: 14
	/* HDR UV lut table x3*/
	#define offset_of_isp10_reg_hdr_uvtbl_x3 (8)
	#define mask_of_isp10_reg_hdr_uvtbl_x3 (0xf000)
	unsigned int reg_hdr_uvtbl_x3:4;

	// h0004
	unsigned int /* padding 16 bit */:16;

	// h0005, bit: 3
	/* HDR UV lut table x4*/
	#define offset_of_isp10_reg_hdr_uvtbl_x4 (10)
	#define mask_of_isp10_reg_hdr_uvtbl_x4 (0xf)
	unsigned int reg_hdr_uvtbl_x4:4;

	// h0005, bit: 7
	/* HDR UV lut table x5*/
	#define offset_of_isp10_reg_hdr_uvtbl_x5 (10)
	#define mask_of_isp10_reg_hdr_uvtbl_x5 (0xf0)
	unsigned int reg_hdr_uvtbl_x5:4;

	// h0005, bit: 11
	/* HDR UV lut table x6*/
	#define offset_of_isp10_reg_hdr_uvtbl_x6 (10)
	#define mask_of_isp10_reg_hdr_uvtbl_x6 (0xf00)
	unsigned int reg_hdr_uvtbl_x6:4;

	// h0005, bit: 14
	/* HDR UV lut table x7*/
	#define offset_of_isp10_reg_hdr_uvtbl_x7 (10)
	#define mask_of_isp10_reg_hdr_uvtbl_x7 (0xf000)
	unsigned int reg_hdr_uvtbl_x7:4;

	// h0005
	unsigned int /* padding 16 bit */:16;

	// h0006, bit: 12
	/* HDR UV lut table y0*/
	#define offset_of_isp10_reg_hdr_uvtbl_y0 (12)
	#define mask_of_isp10_reg_hdr_uvtbl_y0 (0x1fff)
	unsigned int reg_hdr_uvtbl_y0:13;

	// h0006, bit: 14
	/* */
	unsigned int :3;

	// h0006
	unsigned int /* padding 16 bit */:16;

	// h0007, bit: 12
	/* HDR UV lut table y1*/
	#define offset_of_isp10_reg_hdr_uvtbl_y1 (14)
	#define mask_of_isp10_reg_hdr_uvtbl_y1 (0x1fff)
	unsigned int reg_hdr_uvtbl_y1:13;

	// h0007, bit: 14
	/* */
	unsigned int :3;

	// h0007
	unsigned int /* padding 16 bit */:16;

	// h0008, bit: 12
	/* HDR UV lut table y2*/
	#define offset_of_isp10_reg_hdr_uvtbl_y2 (16)
	#define mask_of_isp10_reg_hdr_uvtbl_y2 (0x1fff)
	unsigned int reg_hdr_uvtbl_y2:13;

	// h0008, bit: 14
	/* */
	unsigned int :3;

	// h0008
	unsigned int /* padding 16 bit */:16;

	// h0009, bit: 12
	/* HDR UV lut table y3*/
	#define offset_of_isp10_reg_hdr_uvtbl_y3 (18)
	#define mask_of_isp10_reg_hdr_uvtbl_y3 (0x1fff)
	unsigned int reg_hdr_uvtbl_y3:13;

	// h0009, bit: 14
	/* */
	unsigned int :3;

	// h0009
	unsigned int /* padding 16 bit */:16;

	// h000a, bit: 12
	/* HDR UV lut table y4*/
	#define offset_of_isp10_reg_hdr_uvtbl_y4 (20)
	#define mask_of_isp10_reg_hdr_uvtbl_y4 (0x1fff)
	unsigned int reg_hdr_uvtbl_y4:13;

	// h000a, bit: 14
	/* */
	unsigned int :3;

	// h000a
	unsigned int /* padding 16 bit */:16;

	// h000b, bit: 12
	/* HDR UV lut table y5*/
	#define offset_of_isp10_reg_hdr_uvtbl_y5 (22)
	#define mask_of_isp10_reg_hdr_uvtbl_y5 (0x1fff)
	unsigned int reg_hdr_uvtbl_y5:13;

	// h000b, bit: 14
	/* */
	unsigned int :3;

	// h000b
	unsigned int /* padding 16 bit */:16;

	// h000c, bit: 12
	/* HDR UV lut table y6*/
	#define offset_of_isp10_reg_hdr_uvtbl_y6 (24)
	#define mask_of_isp10_reg_hdr_uvtbl_y6 (0x1fff)
	unsigned int reg_hdr_uvtbl_y6:13;

	// h000c, bit: 14
	/* */
	unsigned int :3;

	// h000c
	unsigned int /* padding 16 bit */:16;

	// h000d, bit: 12
	/* HDR UV lut table y7*/
	#define offset_of_isp10_reg_hdr_uvtbl_y7 (26)
	#define mask_of_isp10_reg_hdr_uvtbl_y7 (0x1fff)
	unsigned int reg_hdr_uvtbl_y7:13;

	// h000d, bit: 14
	/* */
	unsigned int :3;

	// h000d
	unsigned int /* padding 16 bit */:16;

	// h000e, bit: 12
	/* HDR UV lut table y8*/
	#define offset_of_isp10_reg_hdr_uvtbl_y8 (28)
	#define mask_of_isp10_reg_hdr_uvtbl_y8 (0x1fff)
	unsigned int reg_hdr_uvtbl_y8:13;

	// h000e, bit: 14
	/* */
	unsigned int :3;

	// h000e
	unsigned int /* padding 16 bit */:16;

	// h000f, bit: 0
	/* HDR very short frame do nr enable*/
	#define offset_of_isp10_reg_hdr_vs_nr_en (30)
	#define mask_of_isp10_reg_hdr_vs_nr_en (0x1)
	unsigned int reg_hdr_vs_nr_en:1;

	// h000f, bit: 1
	/* HDR short frame do nr enable*/
	#define offset_of_isp10_reg_hdr_s_nr_en (30)
	#define mask_of_isp10_reg_hdr_s_nr_en (0x2)
	unsigned int reg_hdr_s_nr_en:1;

	// h000f, bit: 8
	/* HDR variance threshold*/
	#define offset_of_isp10_reg_hdr_var_th (30)
	#define mask_of_isp10_reg_hdr_var_th (0x1fc)
	unsigned int reg_hdr_var_th:7;

	// h000f, bit: 11
	/* HDR variance count*/
	#define offset_of_isp10_reg_hdr_var_cnt (30)
	#define mask_of_isp10_reg_hdr_var_cnt (0xe00)
	unsigned int reg_hdr_var_cnt:3;

	// h000f, bit: 14
	/* */
	unsigned int :4;

	// h000f
	unsigned int /* padding 16 bit */:16;

	// h0010, bit: 13
	/* HDR edge th1*/
	#define offset_of_isp10_reg_hdr_edge_th1 (32)
	#define mask_of_isp10_reg_hdr_edge_th1 (0x3fff)
	unsigned int reg_hdr_edge_th1:14;

	// h0010, bit: 14
	/* */
	unsigned int :2;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 13
	/* HDR edge th2*/
	#define offset_of_isp10_reg_hdr_edge_th2 (34)
	#define mask_of_isp10_reg_hdr_edge_th2 (0x3fff)
	unsigned int reg_hdr_edge_th2:14;

	// h0011, bit: 14
	/* */
	unsigned int :2;

	// h0011
	unsigned int /* padding 16 bit */:16;

	// h0012, bit: 8
	/* HDR very short frame RGB multi ratio*/
	#define offset_of_isp10_reg_hdr_vs_ratio_128 (36)
	#define mask_of_isp10_reg_hdr_vs_ratio_128 (0x1ff)
	unsigned int reg_hdr_vs_ratio_128:9;

	// h0012, bit: 14
	/* */
	unsigned int :7;

	// h0012
	unsigned int /* padding 16 bit */:16;

	// h0013, bit: 8
	/* HDR short frame RGB multi ratio*/
	#define offset_of_isp10_reg_hdr_s_ratio_128 (38)
	#define mask_of_isp10_reg_hdr_s_ratio_128 (0x1ff)
	unsigned int reg_hdr_s_ratio_128:9;

	// h0013, bit: 14
	/* */
	unsigned int :7;

	// h0013
	unsigned int /* padding 16 bit */:16;

	// h0014, bit: 0
	/* HDR dynamic ratio enable*/
	#define offset_of_isp10_reg_hdr_dyn_rat_en (40)
	#define mask_of_isp10_reg_hdr_dyn_rat_en (0x1)
	unsigned int reg_hdr_dyn_rat_en:1;

	// h0014, bit: 1
	/* HDR dynamic ratio y or bayer sel
	0: y
	1: bayer*/
	#define offset_of_isp10_reg_hdr_dyn_rat_y_rgb (40)
	#define mask_of_isp10_reg_hdr_dyn_rat_y_rgb (0x2)
	unsigned int reg_hdr_dyn_rat_y_rgb:1;

	// h0014, bit: 3
	/* HDR ratio gain upper bound*/
	#define offset_of_isp10_reg_hdr_upper_bnd (40)
	#define mask_of_isp10_reg_hdr_upper_bnd (0xc)
	unsigned int reg_hdr_upper_bnd:2;

	// h0014, bit: 5
	/* HDR ratio gain lower bound*/
	#define offset_of_isp10_reg_hdr_lower_bnd (40)
	#define mask_of_isp10_reg_hdr_lower_bnd (0x30)
	unsigned int reg_hdr_lower_bnd:2;

	// h0014, bit: 6
	/* HDR dynamic ratio parameter select
	0: current
	1: previous*/
	#define offset_of_isp10_reg_hdr_dyn_rat_sel (40)
	#define mask_of_isp10_reg_hdr_dyn_rat_sel (0x40)
	unsigned int reg_hdr_dyn_rat_sel:1;

	// h0014, bit: 14
	/* */
	unsigned int :9;

	// h0014
	unsigned int /* padding 16 bit */:16;

	// h0015, bit: 14
	/* HDR hw ratio gain0 out*/
	#define offset_of_isp10_reg_hdr_ratio_gain0 (42)
	#define mask_of_isp10_reg_hdr_ratio_gain0 (0xffff)
	unsigned int reg_hdr_ratio_gain0:16;

	// h0015
	unsigned int /* padding 16 bit */:16;

	// h0016, bit: 14
	/* HDR hw ratio gain1 out*/
	#define offset_of_isp10_reg_hdr_ratio_gain1 (44)
	#define mask_of_isp10_reg_hdr_ratio_gain1 (0xffff)
	unsigned int reg_hdr_ratio_gain1:16;

	// h0016
	unsigned int /* padding 16 bit */:16;

	// h0017, bit: 14
	/* HDR hw ratio gain2 out*/
	#define offset_of_isp10_reg_hdr_ratio_gain2 (46)
	#define mask_of_isp10_reg_hdr_ratio_gain2 (0xffff)
	unsigned int reg_hdr_ratio_gain2:16;

	// h0017
	unsigned int /* padding 16 bit */:16;

	// h0018, bit: 14
	/* HDR hw ratio gain3 out*/
	#define offset_of_isp10_reg_hdr_ratio_gain3 (48)
	#define mask_of_isp10_reg_hdr_ratio_gain3 (0xffff)
	unsigned int reg_hdr_ratio_gain3:16;

	// h0018
	unsigned int /* padding 16 bit */:16;

	// h0019, bit: 14
	/* HDR hw ratio gain4 out*/
	#define offset_of_isp10_reg_hdr_ratio_gain4 (50)
	#define mask_of_isp10_reg_hdr_ratio_gain4 (0xffff)
	unsigned int reg_hdr_ratio_gain4:16;

	// h0019
	unsigned int /* padding 16 bit */:16;

	// h001a, bit: 14
	/* HDR hw ratio gain5 out*/
	#define offset_of_isp10_reg_hdr_ratio_gain5 (52)
	#define mask_of_isp10_reg_hdr_ratio_gain5 (0xffff)
	unsigned int reg_hdr_ratio_gain5:16;

	// h001a
	unsigned int /* padding 16 bit */:16;

	// h001b, bit: 14
	/* HDR hw ratio gain6 out*/
	#define offset_of_isp10_reg_hdr_ratio_gain6 (54)
	#define mask_of_isp10_reg_hdr_ratio_gain6 (0xffff)
	unsigned int reg_hdr_ratio_gain6:16;

	// h001b
	unsigned int /* padding 16 bit */:16;

	// h001c, bit: 14
	/* HDR hw ratio gain7 out*/
	#define offset_of_isp10_reg_hdr_ratio_gain7 (56)
	#define mask_of_isp10_reg_hdr_ratio_gain7 (0xffff)
	unsigned int reg_hdr_ratio_gain7:16;

	// h001c
	unsigned int /* padding 16 bit */:16;

	// h001d, bit: 14
	/* HDR hw ratio gain8 out*/
	#define offset_of_isp10_reg_hdr_ratio_gain8 (58)
	#define mask_of_isp10_reg_hdr_ratio_gain8 (0xffff)
	unsigned int reg_hdr_ratio_gain8:16;

	// h001d
	unsigned int /* padding 16 bit */:16;

	// h001e, bit: 14
	/* HDR hw ratio gain9 out*/
	#define offset_of_isp10_reg_hdr_ratio_gain9 (60)
	#define mask_of_isp10_reg_hdr_ratio_gain9 (0xffff)
	unsigned int reg_hdr_ratio_gain9:16;

	// h001e
	unsigned int /* padding 16 bit */:16;

	// h001f, bit: 14
	/* HDR hw ratio gain10 out*/
	#define offset_of_isp10_reg_hdr_ratio_gain10 (62)
	#define mask_of_isp10_reg_hdr_ratio_gain10 (0xffff)
	unsigned int reg_hdr_ratio_gain10:16;

	// h001f
	unsigned int /* padding 16 bit */:16;

	// h0020, bit: 14
	/* HDR hw ratio gain11 out*/
	#define offset_of_isp10_reg_hdr_ratio_gain11 (64)
	#define mask_of_isp10_reg_hdr_ratio_gain11 (0xffff)
	unsigned int reg_hdr_ratio_gain11:16;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 14
	/* HDR hw ratio gain12 out*/
	#define offset_of_isp10_reg_hdr_ratio_gain12 (66)
	#define mask_of_isp10_reg_hdr_ratio_gain12 (0xffff)
	unsigned int reg_hdr_ratio_gain12:16;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 14
	/* HDR hw ratio gain13 out*/
	#define offset_of_isp10_reg_hdr_ratio_gain13 (68)
	#define mask_of_isp10_reg_hdr_ratio_gain13 (0xffff)
	unsigned int reg_hdr_ratio_gain13:16;

	// h0022
	unsigned int /* padding 16 bit */:16;

	// h0023, bit: 14
	/* HDR hw ratio gain14 out*/
	#define offset_of_isp10_reg_hdr_ratio_gain14 (70)
	#define mask_of_isp10_reg_hdr_ratio_gain14 (0xffff)
	unsigned int reg_hdr_ratio_gain14:16;

	// h0023
	unsigned int /* padding 16 bit */:16;

	// h0024, bit: 14
	/* HDR hw ratio gain15 out*/
	#define offset_of_isp10_reg_hdr_ratio_gain15 (72)
	#define mask_of_isp10_reg_hdr_ratio_gain15 (0xffff)
	unsigned int reg_hdr_ratio_gain15:16;

	// h0024
	unsigned int /* padding 16 bit */:16;

	// h0025, bit: 14
	/* HDR ratio gain0*/
	#define offset_of_isp10_reg_hdr_sw_ratio_gain0 (74)
	#define mask_of_isp10_reg_hdr_sw_ratio_gain0 (0xffff)
	unsigned int reg_hdr_sw_ratio_gain0:16;

	// h0025
	unsigned int /* padding 16 bit */:16;

	// h0026, bit: 14
	/* HDR ratio gain1*/
	#define offset_of_isp10_reg_hdr_sw_ratio_gain1 (76)
	#define mask_of_isp10_reg_hdr_sw_ratio_gain1 (0xffff)
	unsigned int reg_hdr_sw_ratio_gain1:16;

	// h0026
	unsigned int /* padding 16 bit */:16;

	// h0027, bit: 14
	/* HDR ratio gain2*/
	#define offset_of_isp10_reg_hdr_sw_ratio_gain2 (78)
	#define mask_of_isp10_reg_hdr_sw_ratio_gain2 (0xffff)
	unsigned int reg_hdr_sw_ratio_gain2:16;

	// h0027
	unsigned int /* padding 16 bit */:16;

	// h0028, bit: 14
	/* HDR ratio gain3*/
	#define offset_of_isp10_reg_hdr_sw_ratio_gain3 (80)
	#define mask_of_isp10_reg_hdr_sw_ratio_gain3 (0xffff)
	unsigned int reg_hdr_sw_ratio_gain3:16;

	// h0028
	unsigned int /* padding 16 bit */:16;

	// h0029, bit: 14
	/* HDR ratio gain4*/
	#define offset_of_isp10_reg_hdr_sw_ratio_gain4 (82)
	#define mask_of_isp10_reg_hdr_sw_ratio_gain4 (0xffff)
	unsigned int reg_hdr_sw_ratio_gain4:16;

	// h0029
	unsigned int /* padding 16 bit */:16;

	// h002a, bit: 14
	/* HDR ratio gain5*/
	#define offset_of_isp10_reg_hdr_sw_ratio_gain5 (84)
	#define mask_of_isp10_reg_hdr_sw_ratio_gain5 (0xffff)
	unsigned int reg_hdr_sw_ratio_gain5:16;

	// h002a
	unsigned int /* padding 16 bit */:16;

	// h002b, bit: 14
	/* HDR ratio gain6*/
	#define offset_of_isp10_reg_hdr_sw_ratio_gain6 (86)
	#define mask_of_isp10_reg_hdr_sw_ratio_gain6 (0xffff)
	unsigned int reg_hdr_sw_ratio_gain6:16;

	// h002b
	unsigned int /* padding 16 bit */:16;

	// h002c, bit: 14
	/* HDR ratio gain7*/
	#define offset_of_isp10_reg_hdr_sw_ratio_gain7 (88)
	#define mask_of_isp10_reg_hdr_sw_ratio_gain7 (0xffff)
	unsigned int reg_hdr_sw_ratio_gain7:16;

	// h002c
	unsigned int /* padding 16 bit */:16;

	// h002d, bit: 14
	/* HDR ratio gain8*/
	#define offset_of_isp10_reg_hdr_sw_ratio_gain8 (90)
	#define mask_of_isp10_reg_hdr_sw_ratio_gain8 (0xffff)
	unsigned int reg_hdr_sw_ratio_gain8:16;

	// h002d
	unsigned int /* padding 16 bit */:16;

	// h002e, bit: 14
	/* HDR ratio gain9*/
	#define offset_of_isp10_reg_hdr_sw_ratio_gain9 (92)
	#define mask_of_isp10_reg_hdr_sw_ratio_gain9 (0xffff)
	unsigned int reg_hdr_sw_ratio_gain9:16;

	// h002e
	unsigned int /* padding 16 bit */:16;

	// h002f, bit: 14
	/* HDR ratio gain10*/
	#define offset_of_isp10_reg_hdr_sw_ratio_gain10 (94)
	#define mask_of_isp10_reg_hdr_sw_ratio_gain10 (0xffff)
	unsigned int reg_hdr_sw_ratio_gain10:16;

	// h002f
	unsigned int /* padding 16 bit */:16;

	// h0030, bit: 14
	/* HDR ratio gain11*/
	#define offset_of_isp10_reg_hdr_sw_ratio_gain11 (96)
	#define mask_of_isp10_reg_hdr_sw_ratio_gain11 (0xffff)
	unsigned int reg_hdr_sw_ratio_gain11:16;

	// h0030
	unsigned int /* padding 16 bit */:16;

	// h0031, bit: 14
	/* HDR ratio gain12*/
	#define offset_of_isp10_reg_hdr_sw_ratio_gain12 (98)
	#define mask_of_isp10_reg_hdr_sw_ratio_gain12 (0xffff)
	unsigned int reg_hdr_sw_ratio_gain12:16;

	// h0031
	unsigned int /* padding 16 bit */:16;

	// h0032, bit: 14
	/* HDR ratio gain13*/
	#define offset_of_isp10_reg_hdr_sw_ratio_gain13 (100)
	#define mask_of_isp10_reg_hdr_sw_ratio_gain13 (0xffff)
	unsigned int reg_hdr_sw_ratio_gain13:16;

	// h0032
	unsigned int /* padding 16 bit */:16;

	// h0033, bit: 14
	/* HDR ratio gain14*/
	#define offset_of_isp10_reg_hdr_sw_ratio_gain14 (102)
	#define mask_of_isp10_reg_hdr_sw_ratio_gain14 (0xffff)
	unsigned int reg_hdr_sw_ratio_gain14:16;

	// h0033
	unsigned int /* padding 16 bit */:16;

	// h0034, bit: 14
	/* HDR ratio gain15*/
	#define offset_of_isp10_reg_hdr_sw_ratio_gain15 (104)
	#define mask_of_isp10_reg_hdr_sw_ratio_gain15 (0xffff)
	unsigned int reg_hdr_sw_ratio_gain15:16;

	// h0034
	unsigned int /* padding 16 bit */:16;

	// h0035, bit: 2
	/* HDR gamma table inital in*/
	#define offset_of_isp10_reg_hdr_str_in (106)
	#define mask_of_isp10_reg_hdr_str_in (0x7)
	unsigned int reg_hdr_str_in:3;

	// h0035, bit: 14
	/* */
	unsigned int :13;

	// h0035
	unsigned int /* padding 16 bit */:16;

	// h0036, bit: 14
	/* HDR gamma table end in*/
	#define offset_of_isp10_reg_hdr_end_in (108)
	#define mask_of_isp10_reg_hdr_end_in (0xffff)
	unsigned int reg_hdr_end_in:16;

	// h0036
	unsigned int /* padding 16 bit */:16;

	// h0037, bit: 9
	/* HDR gamma table end out*/
	#define offset_of_isp10_reg_hdr_end_out (110)
	#define mask_of_isp10_reg_hdr_end_out (0x3ff)
	unsigned int reg_hdr_end_out:10;

	// h0037, bit: 14
	/* */
	unsigned int :6;

	// h0037
	unsigned int /* padding 16 bit */:16;

	// h0038, bit: 5
	/* HDR gamma lut table x0*/
	#define offset_of_isp10_reg_hdr_gamma_x0 (112)
	#define mask_of_isp10_reg_hdr_gamma_x0 (0x3f)
	unsigned int reg_hdr_gamma_x0:6;

	// h0038, bit: 7
	/* */
	unsigned int :2;

	// h0038, bit: 13
	/* HDR gamma lut table x1*/
	#define offset_of_isp10_reg_hdr_gamma_x1 (112)
	#define mask_of_isp10_reg_hdr_gamma_x1 (0x3f00)
	unsigned int reg_hdr_gamma_x1:6;

	// h0038, bit: 14
	/* */
	unsigned int :2;

	// h0038
	unsigned int /* padding 16 bit */:16;

	// h0039, bit: 5
	/* HDR gamma lut table x2*/
	#define offset_of_isp10_reg_hdr_gamma_x2 (114)
	#define mask_of_isp10_reg_hdr_gamma_x2 (0x3f)
	unsigned int reg_hdr_gamma_x2:6;

	// h0039, bit: 7
	/* */
	unsigned int :2;

	// h0039, bit: 13
	/* HDR gamma lut table x3*/
	#define offset_of_isp10_reg_hdr_gamma_x3 (114)
	#define mask_of_isp10_reg_hdr_gamma_x3 (0x3f00)
	unsigned int reg_hdr_gamma_x3:6;

	// h0039, bit: 14
	/* */
	unsigned int :2;

	// h0039
	unsigned int /* padding 16 bit */:16;

	// h003a, bit: 5
	/* HDR gamma lut table x4*/
	#define offset_of_isp10_reg_hdr_gamma_x4 (116)
	#define mask_of_isp10_reg_hdr_gamma_x4 (0x3f)
	unsigned int reg_hdr_gamma_x4:6;

	// h003a, bit: 7
	/* */
	unsigned int :2;

	// h003a, bit: 13
	/* HDR gamma lut table x5*/
	#define offset_of_isp10_reg_hdr_gamma_x5 (116)
	#define mask_of_isp10_reg_hdr_gamma_x5 (0x3f00)
	unsigned int reg_hdr_gamma_x5:6;

	// h003a, bit: 14
	/* */
	unsigned int :2;

	// h003a
	unsigned int /* padding 16 bit */:16;

	// h003b, bit: 5
	/* HDR gamma lut table x6*/
	#define offset_of_isp10_reg_hdr_gamma_x6 (118)
	#define mask_of_isp10_reg_hdr_gamma_x6 (0x3f)
	unsigned int reg_hdr_gamma_x6:6;

	// h003b, bit: 7
	/* */
	unsigned int :2;

	// h003b, bit: 13
	/* HDR gamma lut table x7*/
	#define offset_of_isp10_reg_hdr_gamma_x7 (118)
	#define mask_of_isp10_reg_hdr_gamma_x7 (0x3f00)
	unsigned int reg_hdr_gamma_x7:6;

	// h003b, bit: 14
	/* */
	unsigned int :2;

	// h003b
	unsigned int /* padding 16 bit */:16;

	// h003c, bit: 5
	/* HDR gamma lut table x8*/
	#define offset_of_isp10_reg_hdr_gamma_x8 (120)
	#define mask_of_isp10_reg_hdr_gamma_x8 (0x3f)
	unsigned int reg_hdr_gamma_x8:6;

	// h003c, bit: 7
	/* */
	unsigned int :2;

	// h003c, bit: 13
	/* HDR gamma lut table x9*/
	#define offset_of_isp10_reg_hdr_gamma_x9 (120)
	#define mask_of_isp10_reg_hdr_gamma_x9 (0x3f00)
	unsigned int reg_hdr_gamma_x9:6;

	// h003c, bit: 14
	/* */
	unsigned int :2;

	// h003c
	unsigned int /* padding 16 bit */:16;

	// h003d, bit: 5
	/* HDR gamma lut table x10*/
	#define offset_of_isp10_reg_hdr_gamma_x10 (122)
	#define mask_of_isp10_reg_hdr_gamma_x10 (0x3f)
	unsigned int reg_hdr_gamma_x10:6;

	// h003d, bit: 7
	/* */
	unsigned int :2;

	// h003d, bit: 13
	/* HDR gamma lut table x11*/
	#define offset_of_isp10_reg_hdr_gamma_x11 (122)
	#define mask_of_isp10_reg_hdr_gamma_x11 (0x3f00)
	unsigned int reg_hdr_gamma_x11:6;

	// h003d, bit: 14
	/* */
	unsigned int :2;

	// h003d
	unsigned int /* padding 16 bit */:16;

	// h003e, bit: 5
	/* HDR gamma lut table x12*/
	#define offset_of_isp10_reg_hdr_gamma_x12 (124)
	#define mask_of_isp10_reg_hdr_gamma_x12 (0x3f)
	unsigned int reg_hdr_gamma_x12:6;

	// h003e, bit: 7
	/* */
	unsigned int :2;

	// h003e, bit: 13
	/* HDR gamma lut table x13*/
	#define offset_of_isp10_reg_hdr_gamma_x13 (124)
	#define mask_of_isp10_reg_hdr_gamma_x13 (0x3f00)
	unsigned int reg_hdr_gamma_x13:6;

	// h003e, bit: 14
	/* */
	unsigned int :2;

	// h003e
	unsigned int /* padding 16 bit */:16;

	// h003f, bit: 5
	/* HDR gamma lut table x14*/
	#define offset_of_isp10_reg_hdr_gamma_x14 (126)
	#define mask_of_isp10_reg_hdr_gamma_x14 (0x3f)
	unsigned int reg_hdr_gamma_x14:6;

	// h003f, bit: 7
	/* */
	unsigned int :2;

	// h003f, bit: 13
	/* HDR gamma lut table x15*/
	#define offset_of_isp10_reg_hdr_gamma_x15 (126)
	#define mask_of_isp10_reg_hdr_gamma_x15 (0x3f00)
	unsigned int reg_hdr_gamma_x15:6;

	// h003f, bit: 14
	/* */
	unsigned int :2;

	// h003f
	unsigned int /* padding 16 bit */:16;

	// h0040, bit: 5
	/* HDR gamma lut table x16*/
	#define offset_of_isp10_reg_hdr_gamma_x16 (128)
	#define mask_of_isp10_reg_hdr_gamma_x16 (0x3f)
	unsigned int reg_hdr_gamma_x16:6;

	// h0040, bit: 7
	/* */
	unsigned int :2;

	// h0040, bit: 13
	/* HDR gamma lut table x17*/
	#define offset_of_isp10_reg_hdr_gamma_x17 (128)
	#define mask_of_isp10_reg_hdr_gamma_x17 (0x3f00)
	unsigned int reg_hdr_gamma_x17:6;

	// h0040, bit: 14
	/* */
	unsigned int :2;

	// h0040
	unsigned int /* padding 16 bit */:16;

	// h0041, bit: 5
	/* HDR gamma lut table x18*/
	#define offset_of_isp10_reg_hdr_gamma_x18 (130)
	#define mask_of_isp10_reg_hdr_gamma_x18 (0x3f)
	unsigned int reg_hdr_gamma_x18:6;

	// h0041, bit: 7
	/* */
	unsigned int :2;

	// h0041, bit: 13
	/* HDR gamma lut table x19*/
	#define offset_of_isp10_reg_hdr_gamma_x19 (130)
	#define mask_of_isp10_reg_hdr_gamma_x19 (0x3f00)
	unsigned int reg_hdr_gamma_x19:6;

	// h0041, bit: 14
	/* */
	unsigned int :2;

	// h0041
	unsigned int /* padding 16 bit */:16;

	// h0042, bit: 5
	/* HDR gamma lut table x20*/
	#define offset_of_isp10_reg_hdr_gamma_x20 (132)
	#define mask_of_isp10_reg_hdr_gamma_x20 (0x3f)
	unsigned int reg_hdr_gamma_x20:6;

	// h0042, bit: 7
	/* */
	unsigned int :2;

	// h0042, bit: 13
	/* HDR gamma lut table x21*/
	#define offset_of_isp10_reg_hdr_gamma_x21 (132)
	#define mask_of_isp10_reg_hdr_gamma_x21 (0x3f00)
	unsigned int reg_hdr_gamma_x21:6;

	// h0042, bit: 14
	/* */
	unsigned int :2;

	// h0042
	unsigned int /* padding 16 bit */:16;

	// h0043, bit: 9
	/* HDR gamma lut table y0*/
	#define offset_of_isp10_reg_hdr_gamma_y0 (134)
	#define mask_of_isp10_reg_hdr_gamma_y0 (0x3ff)
	unsigned int reg_hdr_gamma_y0:10;

	// h0043, bit: 14
	/* */
	unsigned int :6;

	// h0043
	unsigned int /* padding 16 bit */:16;

	// h0044, bit: 9
	/* HDR gamma lut table y1*/
	#define offset_of_isp10_reg_hdr_gamma_y1 (136)
	#define mask_of_isp10_reg_hdr_gamma_y1 (0x3ff)
	unsigned int reg_hdr_gamma_y1:10;

	// h0044, bit: 14
	/* */
	unsigned int :6;

	// h0044
	unsigned int /* padding 16 bit */:16;

	// h0045, bit: 9
	/* HDR gamma lut table y2*/
	#define offset_of_isp10_reg_hdr_gamma_y2 (138)
	#define mask_of_isp10_reg_hdr_gamma_y2 (0x3ff)
	unsigned int reg_hdr_gamma_y2:10;

	// h0045, bit: 14
	/* */
	unsigned int :6;

	// h0045
	unsigned int /* padding 16 bit */:16;

	// h0046, bit: 9
	/* HDR gamma lut table y3*/
	#define offset_of_isp10_reg_hdr_gamma_y3 (140)
	#define mask_of_isp10_reg_hdr_gamma_y3 (0x3ff)
	unsigned int reg_hdr_gamma_y3:10;

	// h0046, bit: 14
	/* */
	unsigned int :6;

	// h0046
	unsigned int /* padding 16 bit */:16;

	// h0047, bit: 9
	/* HDR gamma lut table y4*/
	#define offset_of_isp10_reg_hdr_gamma_y4 (142)
	#define mask_of_isp10_reg_hdr_gamma_y4 (0x3ff)
	unsigned int reg_hdr_gamma_y4:10;

	// h0047, bit: 14
	/* */
	unsigned int :6;

	// h0047
	unsigned int /* padding 16 bit */:16;

	// h0048, bit: 9
	/* HDR gamma lut table y5*/
	#define offset_of_isp10_reg_hdr_gamma_y5 (144)
	#define mask_of_isp10_reg_hdr_gamma_y5 (0x3ff)
	unsigned int reg_hdr_gamma_y5:10;

	// h0048, bit: 14
	/* */
	unsigned int :6;

	// h0048
	unsigned int /* padding 16 bit */:16;

	// h0049, bit: 9
	/* HDR gamma lut table y6*/
	#define offset_of_isp10_reg_hdr_gamma_y6 (146)
	#define mask_of_isp10_reg_hdr_gamma_y6 (0x3ff)
	unsigned int reg_hdr_gamma_y6:10;

	// h0049, bit: 14
	/* */
	unsigned int :6;

	// h0049
	unsigned int /* padding 16 bit */:16;

	// h004a, bit: 9
	/* HDR gamma lut table y7*/
	#define offset_of_isp10_reg_hdr_gamma_y7 (148)
	#define mask_of_isp10_reg_hdr_gamma_y7 (0x3ff)
	unsigned int reg_hdr_gamma_y7:10;

	// h004a, bit: 14
	/* */
	unsigned int :6;

	// h004a
	unsigned int /* padding 16 bit */:16;

	// h004b, bit: 9
	/* HDR gamma lut table y8*/
	#define offset_of_isp10_reg_hdr_gamma_y8 (150)
	#define mask_of_isp10_reg_hdr_gamma_y8 (0x3ff)
	unsigned int reg_hdr_gamma_y8:10;

	// h004b, bit: 14
	/* */
	unsigned int :6;

	// h004b
	unsigned int /* padding 16 bit */:16;

	// h004c, bit: 9
	/* HDR gamma lut table y9*/
	#define offset_of_isp10_reg_hdr_gamma_y9 (152)
	#define mask_of_isp10_reg_hdr_gamma_y9 (0x3ff)
	unsigned int reg_hdr_gamma_y9:10;

	// h004c, bit: 14
	/* */
	unsigned int :6;

	// h004c
	unsigned int /* padding 16 bit */:16;

	// h004d, bit: 9
	/* HDR gamma lut table y10*/
	#define offset_of_isp10_reg_hdr_gamma_y10 (154)
	#define mask_of_isp10_reg_hdr_gamma_y10 (0x3ff)
	unsigned int reg_hdr_gamma_y10:10;

	// h004d, bit: 14
	/* */
	unsigned int :6;

	// h004d
	unsigned int /* padding 16 bit */:16;

	// h004e, bit: 9
	/* HDR gamma lut table y11*/
	#define offset_of_isp10_reg_hdr_gamma_y11 (156)
	#define mask_of_isp10_reg_hdr_gamma_y11 (0x3ff)
	unsigned int reg_hdr_gamma_y11:10;

	// h004e, bit: 14
	/* */
	unsigned int :6;

	// h004e
	unsigned int /* padding 16 bit */:16;

	// h004f, bit: 9
	/* HDR gamma lut table y12*/
	#define offset_of_isp10_reg_hdr_gamma_y12 (158)
	#define mask_of_isp10_reg_hdr_gamma_y12 (0x3ff)
	unsigned int reg_hdr_gamma_y12:10;

	// h004f, bit: 14
	/* */
	unsigned int :6;

	// h004f
	unsigned int /* padding 16 bit */:16;

	// h0050, bit: 9
	/* HDR gamma lut table y13*/
	#define offset_of_isp10_reg_hdr_gamma_y13 (160)
	#define mask_of_isp10_reg_hdr_gamma_y13 (0x3ff)
	unsigned int reg_hdr_gamma_y13:10;

	// h0050, bit: 14
	/* */
	unsigned int :6;

	// h0050
	unsigned int /* padding 16 bit */:16;

	// h0051, bit: 9
	/* HDR gamma lut table y14*/
	#define offset_of_isp10_reg_hdr_gamma_y14 (162)
	#define mask_of_isp10_reg_hdr_gamma_y14 (0x3ff)
	unsigned int reg_hdr_gamma_y14:10;

	// h0051, bit: 14
	/* */
	unsigned int :6;

	// h0051
	unsigned int /* padding 16 bit */:16;

	// h0052, bit: 9
	/* HDR gamma lut table y15*/
	#define offset_of_isp10_reg_hdr_gamma_y15 (164)
	#define mask_of_isp10_reg_hdr_gamma_y15 (0x3ff)
	unsigned int reg_hdr_gamma_y15:10;

	// h0052, bit: 14
	/* */
	unsigned int :6;

	// h0052
	unsigned int /* padding 16 bit */:16;

	// h0053, bit: 9
	/* HDR gamma lut table y16*/
	#define offset_of_isp10_reg_hdr_gamma_y16 (166)
	#define mask_of_isp10_reg_hdr_gamma_y16 (0x3ff)
	unsigned int reg_hdr_gamma_y16:10;

	// h0053, bit: 14
	/* */
	unsigned int :6;

	// h0053
	unsigned int /* padding 16 bit */:16;

	// h0054, bit: 9
	/* HDR gamma lut table y17*/
	#define offset_of_isp10_reg_hdr_gamma_y17 (168)
	#define mask_of_isp10_reg_hdr_gamma_y17 (0x3ff)
	unsigned int reg_hdr_gamma_y17:10;

	// h0054, bit: 14
	/* */
	unsigned int :6;

	// h0054
	unsigned int /* padding 16 bit */:16;

	// h0055, bit: 9
	/* HDR gamma lut table y18*/
	#define offset_of_isp10_reg_hdr_gamma_y18 (170)
	#define mask_of_isp10_reg_hdr_gamma_y18 (0x3ff)
	unsigned int reg_hdr_gamma_y18:10;

	// h0055, bit: 14
	/* */
	unsigned int :6;

	// h0055
	unsigned int /* padding 16 bit */:16;

	// h0056, bit: 9
	/* HDR gamma lut table y19*/
	#define offset_of_isp10_reg_hdr_gamma_y19 (172)
	#define mask_of_isp10_reg_hdr_gamma_y19 (0x3ff)
	unsigned int reg_hdr_gamma_y19:10;

	// h0056, bit: 14
	/* */
	unsigned int :6;

	// h0056
	unsigned int /* padding 16 bit */:16;

	// h0057, bit: 9
	/* HDR gamma lut table y20*/
	#define offset_of_isp10_reg_hdr_gamma_y20 (174)
	#define mask_of_isp10_reg_hdr_gamma_y20 (0x3ff)
	unsigned int reg_hdr_gamma_y20:10;

	// h0057, bit: 14
	/* */
	unsigned int :6;

	// h0057
	unsigned int /* padding 16 bit */:16;

	// h0058, bit: 9
	/* HDR gamma lut table y21*/
	#define offset_of_isp10_reg_hdr_gamma_y21 (176)
	#define mask_of_isp10_reg_hdr_gamma_y21 (0x3ff)
	unsigned int reg_hdr_gamma_y21:10;

	// h0058, bit: 14
	/* */
	unsigned int :6;

	// h0058
	unsigned int /* padding 16 bit */:16;

	// h0059, bit: 9
	/* HDR gamma lut table y22*/
	#define offset_of_isp10_reg_hdr_gamma_y22 (178)
	#define mask_of_isp10_reg_hdr_gamma_y22 (0x3ff)
	unsigned int reg_hdr_gamma_y22:10;

	// h0059, bit: 14
	/* */
	unsigned int :6;

	// h0059
	unsigned int /* padding 16 bit */:16;

	// h005a, bit: 5
	/* HDR map dc filter 11,15, 51, 55*/
	#define offset_of_isp10_reg_hdr_map_dc_filter11 (180)
	#define mask_of_isp10_reg_hdr_map_dc_filter11 (0x3f)
	unsigned int reg_hdr_map_dc_filter11:6;

	// h005a, bit: 7
	/* */
	unsigned int :2;

	// h005a, bit: 13
	/* HDR map dc filter 12,14, 52, 54*/
	#define offset_of_isp10_reg_hdr_map_dc_filter12 (180)
	#define mask_of_isp10_reg_hdr_map_dc_filter12 (0x3f00)
	unsigned int reg_hdr_map_dc_filter12:6;

	// h005a, bit: 14
	/* */
	unsigned int :2;

	// h005a
	unsigned int /* padding 16 bit */:16;

	// h005b, bit: 5
	/* HDR map dc filter 13*/
	#define offset_of_isp10_reg_hdr_map_dc_filter13 (182)
	#define mask_of_isp10_reg_hdr_map_dc_filter13 (0x3f)
	unsigned int reg_hdr_map_dc_filter13:6;

	// h005b, bit: 7
	/* */
	unsigned int :2;

	// h005b, bit: 13
	/* HDR map dc filter 21, 24, 41, 44*/
	#define offset_of_isp10_reg_hdr_map_dc_filter21 (182)
	#define mask_of_isp10_reg_hdr_map_dc_filter21 (0x3f00)
	unsigned int reg_hdr_map_dc_filter21:6;

	// h005b, bit: 14
	/* */
	unsigned int :2;

	// h005b
	unsigned int /* padding 16 bit */:16;

	// h005c, bit: 5
	/* HDR map dc filter 22, 24, 42, 44*/
	#define offset_of_isp10_reg_hdr_map_dc_filter22 (184)
	#define mask_of_isp10_reg_hdr_map_dc_filter22 (0x3f)
	unsigned int reg_hdr_map_dc_filter22:6;

	// h005c, bit: 7
	/* */
	unsigned int :2;

	// h005c, bit: 13
	/* HDR map dc filter 23*/
	#define offset_of_isp10_reg_hdr_map_dc_filter23 (184)
	#define mask_of_isp10_reg_hdr_map_dc_filter23 (0x3f00)
	unsigned int reg_hdr_map_dc_filter23:6;

	// h005c, bit: 14
	/* */
	unsigned int :2;

	// h005c
	unsigned int /* padding 16 bit */:16;

	// h005d, bit: 5
	/* HDR map dc filter 31, 35*/
	#define offset_of_isp10_reg_hdr_map_dc_filter31 (186)
	#define mask_of_isp10_reg_hdr_map_dc_filter31 (0x3f)
	unsigned int reg_hdr_map_dc_filter31:6;

	// h005d, bit: 7
	/* */
	unsigned int :2;

	// h005d, bit: 13
	/* HDR map dc filter 32, 34*/
	#define offset_of_isp10_reg_hdr_map_dc_filter32 (186)
	#define mask_of_isp10_reg_hdr_map_dc_filter32 (0x3f00)
	unsigned int reg_hdr_map_dc_filter32:6;

	// h005d, bit: 14
	/* */
	unsigned int :2;

	// h005d
	unsigned int /* padding 16 bit */:16;

	// h005e, bit: 5
	/* HDR map dc filter 33*/
	#define offset_of_isp10_reg_hdr_map_dc_filter33 (188)
	#define mask_of_isp10_reg_hdr_map_dc_filter33 (0x3f)
	unsigned int reg_hdr_map_dc_filter33:6;

	// h005e, bit: 14
	/* */
	unsigned int :10;

	// h005e
	unsigned int /* padding 16 bit */:16;

	// h005f, bit: 4
	/* HDR map dc diff shift x0*/
	#define offset_of_isp10_reg_hdr_dc_sft_x0 (190)
	#define mask_of_isp10_reg_hdr_dc_sft_x0 (0x1f)
	unsigned int reg_hdr_dc_sft_x0:5;

	// h005f, bit: 7
	/* */
	unsigned int :3;

	// h005f, bit: 12
	/* HDR map dc diff shift x1*/
	#define offset_of_isp10_reg_hdr_dc_sft_x1 (190)
	#define mask_of_isp10_reg_hdr_dc_sft_x1 (0x1f00)
	unsigned int reg_hdr_dc_sft_x1:5;

	// h005f, bit: 14
	/* */
	unsigned int :3;

	// h005f
	unsigned int /* padding 16 bit */:16;

	// h0060, bit: 4
	/* HDR map dc diff shift x2*/
	#define offset_of_isp10_reg_hdr_dc_sft_x2 (192)
	#define mask_of_isp10_reg_hdr_dc_sft_x2 (0x1f)
	unsigned int reg_hdr_dc_sft_x2:5;

	// h0060, bit: 7
	/* */
	unsigned int :3;

	// h0060, bit: 12
	/* HDR map dc diff shift x3*/
	#define offset_of_isp10_reg_hdr_dc_sft_x3 (192)
	#define mask_of_isp10_reg_hdr_dc_sft_x3 (0x1f00)
	unsigned int reg_hdr_dc_sft_x3:5;

	// h0060, bit: 14
	/* */
	unsigned int :3;

	// h0060
	unsigned int /* padding 16 bit */:16;

	// h0061, bit: 4
	/* HDR map dc diff shift x4*/
	#define offset_of_isp10_reg_hdr_dc_sft_x4 (194)
	#define mask_of_isp10_reg_hdr_dc_sft_x4 (0x1f)
	unsigned int reg_hdr_dc_sft_x4:5;

	// h0061, bit: 7
	/* */
	unsigned int :3;

	// h0061, bit: 12
	/* HDR map dc diff shift x5*/
	#define offset_of_isp10_reg_hdr_dc_sft_x5 (194)
	#define mask_of_isp10_reg_hdr_dc_sft_x5 (0x1f00)
	unsigned int reg_hdr_dc_sft_x5:5;

	// h0061, bit: 14
	/* */
	unsigned int :3;

	// h0061
	unsigned int /* padding 16 bit */:16;

	// h0062, bit: 4
	/* HDR map dc diff shift x6*/
	#define offset_of_isp10_reg_hdr_dc_sft_x6 (196)
	#define mask_of_isp10_reg_hdr_dc_sft_x6 (0x1f)
	unsigned int reg_hdr_dc_sft_x6:5;

	// h0062, bit: 14
	/* */
	unsigned int :11;

	// h0062
	unsigned int /* padding 16 bit */:16;

	// h0063, bit: 7
	/* HDR map dc diff gain y0*/
	#define offset_of_isp10_reg_hdr_dc_gain_y0 (198)
	#define mask_of_isp10_reg_hdr_dc_gain_y0 (0xff)
	unsigned int reg_hdr_dc_gain_y0:8;

	// h0063, bit: 14
	/* HDR map dc diff gain y1*/
	#define offset_of_isp10_reg_hdr_dc_gain_y1 (198)
	#define mask_of_isp10_reg_hdr_dc_gain_y1 (0xff00)
	unsigned int reg_hdr_dc_gain_y1:8;

	// h0063
	unsigned int /* padding 16 bit */:16;

	// h0064, bit: 7
	/* HDR map dc diff gain y2*/
	#define offset_of_isp10_reg_hdr_dc_gain_y2 (200)
	#define mask_of_isp10_reg_hdr_dc_gain_y2 (0xff)
	unsigned int reg_hdr_dc_gain_y2:8;

	// h0064, bit: 14
	/* HDR map dc diff gain y3*/
	#define offset_of_isp10_reg_hdr_dc_gain_y3 (200)
	#define mask_of_isp10_reg_hdr_dc_gain_y3 (0xff00)
	unsigned int reg_hdr_dc_gain_y3:8;

	// h0064
	unsigned int /* padding 16 bit */:16;

	// h0065, bit: 7
	/* HDR map dc diff gain y4*/
	#define offset_of_isp10_reg_hdr_dc_gain_y4 (202)
	#define mask_of_isp10_reg_hdr_dc_gain_y4 (0xff)
	unsigned int reg_hdr_dc_gain_y4:8;

	// h0065, bit: 14
	/* HDR map dc diff gain y5*/
	#define offset_of_isp10_reg_hdr_dc_gain_y5 (202)
	#define mask_of_isp10_reg_hdr_dc_gain_y5 (0xff00)
	unsigned int reg_hdr_dc_gain_y5:8;

	// h0065
	unsigned int /* padding 16 bit */:16;

	// h0066, bit: 7
	/* HDR map dc diff gain y6*/
	#define offset_of_isp10_reg_hdr_dc_gain_y6 (204)
	#define mask_of_isp10_reg_hdr_dc_gain_y6 (0xff)
	unsigned int reg_hdr_dc_gain_y6:8;

	// h0066, bit: 14
	/* HDR map dc diff gain y7*/
	#define offset_of_isp10_reg_hdr_dc_gain_y7 (204)
	#define mask_of_isp10_reg_hdr_dc_gain_y7 (0xff00)
	unsigned int reg_hdr_dc_gain_y7:8;

	// h0066
	unsigned int /* padding 16 bit */:16;

	// h0067, bit: 4
	/* HDR map ac ratio shift x0*/
	#define offset_of_isp10_reg_hdr_ac_sft_x0 (206)
	#define mask_of_isp10_reg_hdr_ac_sft_x0 (0x1f)
	unsigned int reg_hdr_ac_sft_x0:5;

	// h0067, bit: 7
	/* */
	unsigned int :3;

	// h0067, bit: 12
	/* HDR map ac ratio shift x1*/
	#define offset_of_isp10_reg_hdr_ac_sft_x1 (206)
	#define mask_of_isp10_reg_hdr_ac_sft_x1 (0x1f00)
	unsigned int reg_hdr_ac_sft_x1:5;

	// h0067, bit: 14
	/* */
	unsigned int :3;

	// h0067
	unsigned int /* padding 16 bit */:16;

	// h0068, bit: 4
	/* HDR map ac ratio shift x2*/
	#define offset_of_isp10_reg_hdr_ac_sft_x2 (208)
	#define mask_of_isp10_reg_hdr_ac_sft_x2 (0x1f)
	unsigned int reg_hdr_ac_sft_x2:5;

	// h0068, bit: 7
	/* */
	unsigned int :3;

	// h0068, bit: 12
	/* HDR map ac ratio shift x3*/
	#define offset_of_isp10_reg_hdr_ac_sft_x3 (208)
	#define mask_of_isp10_reg_hdr_ac_sft_x3 (0x1f00)
	unsigned int reg_hdr_ac_sft_x3:5;

	// h0068, bit: 14
	/* */
	unsigned int :3;

	// h0068
	unsigned int /* padding 16 bit */:16;

	// h0069, bit: 4
	/* HDR map ac ratio shift x4*/
	#define offset_of_isp10_reg_hdr_ac_sft_x4 (210)
	#define mask_of_isp10_reg_hdr_ac_sft_x4 (0x1f)
	unsigned int reg_hdr_ac_sft_x4:5;

	// h0069, bit: 7
	/* */
	unsigned int :3;

	// h0069, bit: 12
	/* HDR map ac ratio shift x5*/
	#define offset_of_isp10_reg_hdr_ac_sft_x5 (210)
	#define mask_of_isp10_reg_hdr_ac_sft_x5 (0x1f00)
	unsigned int reg_hdr_ac_sft_x5:5;

	// h0069, bit: 14
	/* */
	unsigned int :3;

	// h0069
	unsigned int /* padding 16 bit */:16;

	// h006a, bit: 4
	/* HDR map ac ratio shift x6*/
	#define offset_of_isp10_reg_hdr_ac_sft_x6 (212)
	#define mask_of_isp10_reg_hdr_ac_sft_x6 (0x1f)
	unsigned int reg_hdr_ac_sft_x6:5;

	// h006a, bit: 14
	/* */
	unsigned int :11;

	// h006a
	unsigned int /* padding 16 bit */:16;

	// h006b, bit: 7
	/* HDR map ac ratio gain y0*/
	#define offset_of_isp10_reg_hdr_ac_gain_y0 (214)
	#define mask_of_isp10_reg_hdr_ac_gain_y0 (0xff)
	unsigned int reg_hdr_ac_gain_y0:8;

	// h006b, bit: 14
	/* HDR map ac ratio gain y1*/
	#define offset_of_isp10_reg_hdr_ac_gain_y1 (214)
	#define mask_of_isp10_reg_hdr_ac_gain_y1 (0xff00)
	unsigned int reg_hdr_ac_gain_y1:8;

	// h006b
	unsigned int /* padding 16 bit */:16;

	// h006c, bit: 7
	/* HDR map ac ratio gain y2*/
	#define offset_of_isp10_reg_hdr_ac_gain_y2 (216)
	#define mask_of_isp10_reg_hdr_ac_gain_y2 (0xff)
	unsigned int reg_hdr_ac_gain_y2:8;

	// h006c, bit: 14
	/* HDR map ac ratio gain y3*/
	#define offset_of_isp10_reg_hdr_ac_gain_y3 (216)
	#define mask_of_isp10_reg_hdr_ac_gain_y3 (0xff00)
	unsigned int reg_hdr_ac_gain_y3:8;

	// h006c
	unsigned int /* padding 16 bit */:16;

	// h006d, bit: 7
	/* HDR map ac ratio gain y4*/
	#define offset_of_isp10_reg_hdr_ac_gain_y4 (218)
	#define mask_of_isp10_reg_hdr_ac_gain_y4 (0xff)
	unsigned int reg_hdr_ac_gain_y4:8;

	// h006d, bit: 14
	/* HDR map ac ratio gain y5*/
	#define offset_of_isp10_reg_hdr_ac_gain_y5 (218)
	#define mask_of_isp10_reg_hdr_ac_gain_y5 (0xff00)
	unsigned int reg_hdr_ac_gain_y5:8;

	// h006d
	unsigned int /* padding 16 bit */:16;

	// h006e, bit: 7
	/* HDR map ac ratio gain y6*/
	#define offset_of_isp10_reg_hdr_ac_gain_y6 (220)
	#define mask_of_isp10_reg_hdr_ac_gain_y6 (0xff)
	unsigned int reg_hdr_ac_gain_y6:8;

	// h006e, bit: 14
	/* HDR map ac ratio gain y7*/
	#define offset_of_isp10_reg_hdr_ac_gain_y7 (220)
	#define mask_of_isp10_reg_hdr_ac_gain_y7 (0xff00)
	unsigned int reg_hdr_ac_gain_y7:8;

	// h006e
	unsigned int /* padding 16 bit */:16;

	// h006f, bit: 14
	/* */
	unsigned int :16;

	// h006f
	unsigned int /* padding 16 bit */:16;

	// h0070, bit: 14
	/* HDR histogram shift*/
	#define offset_of_isp10_reg_hdr_histogram_shft (224)
	#define mask_of_isp10_reg_hdr_histogram_shft (0xffff)
	unsigned int reg_hdr_histogram_shft:16;

	// h0070
	unsigned int /* padding 16 bit */:16;

	// h0071, bit: 0
	/* HDR histogram shift*/
	#define offset_of_isp10_reg_hdr_histogram_shft_1 (226)
	#define mask_of_isp10_reg_hdr_histogram_shft_1 (0x1)
	unsigned int reg_hdr_histogram_shft_1:1;

	// h0071, bit: 14
	/* */
	unsigned int :15;

	// h0071
	unsigned int /* padding 16 bit */:16;

	// h0072, bit: 14
	/* for HDR buufer part line count debug*/
	#define offset_of_isp10_reg_hdr_buf_line_y_dbg (228)
	#define mask_of_isp10_reg_hdr_buf_line_y_dbg (0xffff)
	unsigned int reg_hdr_buf_line_y_dbg:16;

	// h0072
	unsigned int /* padding 16 bit */:16;

	// h0073, bit: 14
	/* for HDR core part line count debug*/
	#define offset_of_isp10_reg_hdr_core_line_y_dbg (230)
	#define mask_of_isp10_reg_hdr_core_line_y_dbg (0xffff)
	unsigned int reg_hdr_core_line_y_dbg:16;

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

}  __attribute__((packed, aligned(1))) infinity2_reg_isp10;
#endif
