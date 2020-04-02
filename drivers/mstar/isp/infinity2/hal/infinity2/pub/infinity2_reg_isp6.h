// Generate Time: 2017-09-19 22:58:04.188662
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
#ifndef __INFINITY2_REG_ISP6__
#define __INFINITY2_REG_ISP6__
typedef struct {
	// h0000, bit: 0
	/* ISP statistic enable*/
	#define offset_of_isp6_reg_isp_statis_en (0)
	#define mask_of_isp6_reg_isp_statis_en (0x1)
	unsigned int reg_isp_statis_en:1;

	// h0000, bit: 1
	/* Trigger of ISP statistic one frame mode*/
	#define offset_of_isp6_reg_statis_once_trig (0)
	#define mask_of_isp6_reg_statis_once_trig (0x2)
	unsigned int reg_statis_once_trig:1;

	// h0000, bit: 2
	/* ISP statistic AE enable*/
	#define offset_of_isp6_reg_isp_statis_ae_en (0)
	#define mask_of_isp6_reg_isp_statis_ae_en (0x4)
	unsigned int reg_isp_statis_ae_en:1;

	// h0000, bit: 3
	/* 3A sync cycle, 0: 4T, 1:3T*/
	#define offset_of_isp6_reg_sta_sync_3t_dis (0)
	#define mask_of_isp6_reg_sta_sync_3t_dis (0x8)
	unsigned int reg_sta_sync_3t_dis:1;

	// h0000, bit: 7
	/* */
	unsigned int :4;

	// h0000, bit: 14
	/* AE block row number for interrupt*/
	#define offset_of_isp6_reg_ae_int_row_num (0)
	#define mask_of_isp6_reg_ae_int_row_num (0x7f00)
	unsigned int reg_ae_int_row_num:7;

	// h0000, bit: 15
	/* */
	unsigned int :1;

	// h0000
	unsigned int /* padding 16 bit */:16;

	// h0001, bit: 10
	/* AE block width minus one*/
	#define offset_of_isp6_reg_ae_blksize_x_m1 (2)
	#define mask_of_isp6_reg_ae_blksize_x_m1 (0x7ff)
	unsigned int reg_ae_blksize_x_m1:11;

	// h0001, bit: 14
	/* */
	unsigned int :5;

	// h0001
	unsigned int /* padding 16 bit */:16;

	// h0002, bit: 10
	/* AE block height minus one*/
	#define offset_of_isp6_reg_ae_blksize_y_m1 (4)
	#define mask_of_isp6_reg_ae_blksize_y_m1 (0x7ff)
	unsigned int reg_ae_blksize_y_m1:11;

	// h0002, bit: 14
	/* */
	unsigned int :5;

	// h0002
	unsigned int /* padding 16 bit */:16;

	// h0003, bit: 6
	/* AE Horizontal block number minus one*/
	#define offset_of_isp6_reg_ae_blknum_x_m1 (6)
	#define mask_of_isp6_reg_ae_blknum_x_m1 (0x7f)
	unsigned int reg_ae_blknum_x_m1:7;

	// h0003, bit: 7
	/* */
	unsigned int :1;

	// h0003, bit: 14
	/* AE vertical block number minus one*/
	#define offset_of_isp6_reg_ae_blknum_y_m1 (6)
	#define mask_of_isp6_reg_ae_blknum_y_m1 (0x7f00)
	unsigned int reg_ae_blknum_y_m1:7;

	// h0003, bit: 15
	/* */
	unsigned int :1;

	// h0003
	unsigned int /* padding 16 bit */:16;

	// h0004, bit: 12
	/* AE horizontal block pixel offset*/
	#define offset_of_isp6_reg_ae_blk_h_offset (8)
	#define mask_of_isp6_reg_ae_blk_h_offset (0x1fff)
	unsigned int reg_ae_blk_h_offset:13;

	// h0004, bit: 14
	/* */
	unsigned int :3;

	// h0004
	unsigned int /* padding 16 bit */:16;

	// h0005, bit: 12
	/* AE veritcal block pixel offset*/
	#define offset_of_isp6_reg_ae_blk_v_offset (10)
	#define mask_of_isp6_reg_ae_blk_v_offset (0x1fff)
	unsigned int reg_ae_blk_v_offset:13;

	// h0005, bit: 14
	/* */
	unsigned int :3;

	// h0005
	unsigned int /* padding 16 bit */:16;

	// h0006, bit: 14
	/* AE statistic  frame buffer (unit : 16bytes)*/
	#define offset_of_isp6_reg_ae_statis_base (12)
	#define mask_of_isp6_reg_ae_statis_base (0xffff)
	unsigned int reg_ae_statis_base:16;

	// h0006
	unsigned int /* padding 16 bit */:16;

	// h0007, bit: 10
	/* AE statistic  frame buffer (unit : 16bytes)*/
	#define offset_of_isp6_reg_ae_statis_base_1 (14)
	#define mask_of_isp6_reg_ae_statis_base_1 (0x7ff)
	unsigned int reg_ae_statis_base_1:11;

	// h0007, bit: 14
	/* */
	unsigned int :5;

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
	/* Histo statistic  frame buffer (unit : 16bytes)*/
	#define offset_of_isp6_reg_histo_statis_base (20)
	#define mask_of_isp6_reg_histo_statis_base (0xffff)
	unsigned int reg_histo_statis_base:16;

	// h000a
	unsigned int /* padding 16 bit */:16;

	// h000b, bit: 10
	/* Histo statistic  frame buffer (unit : 16bytes)*/
	#define offset_of_isp6_reg_histo_statis_base_1 (22)
	#define mask_of_isp6_reg_histo_statis_base_1 (0x7ff)
	unsigned int reg_histo_statis_base_1:11;

	// h000b, bit: 14
	/* */
	unsigned int :5;

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
	/* AE average statistic  max miu write address*/
	#define offset_of_isp6_reg_ae_max_wadr (32)
	#define mask_of_isp6_reg_ae_max_wadr (0xffff)
	unsigned int reg_ae_max_wadr:16;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 10
	/* AE average statistic  max miu write address*/
	#define offset_of_isp6_reg_ae_max_wadr_1 (34)
	#define mask_of_isp6_reg_ae_max_wadr_1 (0x7ff)
	unsigned int reg_ae_max_wadr_1:11;

	// h0011, bit: 14
	/* */
	unsigned int :5;

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
	/* Histogram statistic  max miu write address*/
	#define offset_of_isp6_reg_histo_max_wadr (40)
	#define mask_of_isp6_reg_histo_max_wadr (0xffff)
	unsigned int reg_histo_max_wadr:16;

	// h0014
	unsigned int /* padding 16 bit */:16;

	// h0015, bit: 10
	/* Histogram statistic  max miu write address*/
	#define offset_of_isp6_reg_histo_max_wadr_1 (42)
	#define mask_of_isp6_reg_histo_max_wadr_1 (0x7ff)
	unsigned int reg_histo_max_wadr_1:11;

	// h0015, bit: 14
	/* */
	unsigned int :5;

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
	/* AE block average divisor A minus one*/
	#define offset_of_isp6_reg_ae_div_a_m1 (48)
	#define mask_of_isp6_reg_ae_div_a_m1 (0xffff)
	unsigned int reg_ae_div_a_m1:16;

	// h0018
	unsigned int /* padding 16 bit */:16;

	// h0019, bit: 14
	/* AE block average divisor B minus one*/
	#define offset_of_isp6_reg_ae_div_b_m1 (50)
	#define mask_of_isp6_reg_ae_div_b_m1 (0xffff)
	unsigned int reg_ae_div_b_m1:16;

	// h0019
	unsigned int /* padding 16 bit */:16;

	// h001a, bit: 14
	/* AE block average divisor C minus one*/
	#define offset_of_isp6_reg_ae_div_c_m1 (52)
	#define mask_of_isp6_reg_ae_div_c_m1 (0xffff)
	unsigned int reg_ae_div_c_m1:16;

	// h001a
	unsigned int /* padding 16 bit */:16;

	// h001b, bit: 14
	/* AE block average divisor D minus one*/
	#define offset_of_isp6_reg_ae_div_d_m1 (54)
	#define mask_of_isp6_reg_ae_div_d_m1 (0xffff)
	unsigned int reg_ae_div_d_m1:16;

	// h001b
	unsigned int /* padding 16 bit */:16;

	// h001c, bit: 14
	/* AE block average divisor E minus one*/
	#define offset_of_isp6_reg_ae_div_e_m1 (56)
	#define mask_of_isp6_reg_ae_div_e_m1 (0xffff)
	unsigned int reg_ae_div_e_m1:16;

	// h001c
	unsigned int /* padding 16 bit */:16;

	// h001d, bit: 14
	/* AE block average divisor F minuso one*/
	#define offset_of_isp6_reg_ae_div_f_m1 (58)
	#define mask_of_isp6_reg_ae_div_f_m1 (0xffff)
	unsigned int reg_ae_div_f_m1:16;

	// h001d
	unsigned int /* padding 16 bit */:16;

	// h001e, bit: 3
	/* AE accumulation shift register*/
	#define offset_of_isp6_reg_ae_acc_sft (60)
	#define mask_of_isp6_reg_ae_acc_sft (0xf)
	unsigned int reg_ae_acc_sft:4;

	// h001e, bit: 14
	/* */
	unsigned int :12;

	// h001e
	unsigned int /* padding 16 bit */:16;

	// h001f, bit: 1
	/* */
	unsigned int :2;

	// h001f, bit: 2
	/* Window0 histogram enable*/
	#define offset_of_isp6_reg_histo_win0_en (62)
	#define mask_of_isp6_reg_histo_win0_en (0x4)
	unsigned int reg_histo_win0_en:1;

	// h001f, bit: 3
	/* Window1 histogram enable*/
	#define offset_of_isp6_reg_histo_win1_en (62)
	#define mask_of_isp6_reg_histo_win1_en (0x8)
	unsigned int reg_histo_win1_en:1;

	// h001f, bit: 14
	/* */
	unsigned int :12;

	// h001f
	unsigned int /* padding 16 bit */:16;

	// h0020, bit: 10
	/* Histogram block width minus one*/
	#define offset_of_isp6_reg_histo_blksize_x_m1 (64)
	#define mask_of_isp6_reg_histo_blksize_x_m1 (0x7ff)
	unsigned int reg_histo_blksize_x_m1:11;

	// h0020, bit: 14
	/* */
	unsigned int :5;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 10
	/* Histogram block height minus one*/
	#define offset_of_isp6_reg_histo_blksize_y_m1 (66)
	#define mask_of_isp6_reg_histo_blksize_y_m1 (0x7ff)
	unsigned int reg_histo_blksize_y_m1:11;

	// h0021, bit: 14
	/* */
	unsigned int :5;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 6
	/* Histogram horizontal block number minus one*/
	#define offset_of_isp6_reg_histo_blknum_x_m1 (68)
	#define mask_of_isp6_reg_histo_blknum_x_m1 (0x7f)
	unsigned int reg_histo_blknum_x_m1:7;

	// h0022, bit: 7
	/* */
	unsigned int :1;

	// h0022, bit: 14
	/* Histogram vertical block number minus one*/
	#define offset_of_isp6_reg_histo_blknum_y_m1 (68)
	#define mask_of_isp6_reg_histo_blknum_y_m1 (0x7f00)
	unsigned int reg_histo_blknum_y_m1:7;

	// h0022, bit: 15
	/* */
	unsigned int :1;

	// h0022
	unsigned int /* padding 16 bit */:16;

	// h0023, bit: 12
	/* Histogram horizontal block pixel offset*/
	#define offset_of_isp6_reg_histo_blk_h_offset (70)
	#define mask_of_isp6_reg_histo_blk_h_offset (0x1fff)
	unsigned int reg_histo_blk_h_offset:13;

	// h0023, bit: 14
	/* */
	unsigned int :3;

	// h0023
	unsigned int /* padding 16 bit */:16;

	// h0024, bit: 12
	/* Histogram veritcal block pixel offset*/
	#define offset_of_isp6_reg_histo_blk_v_offset (72)
	#define mask_of_isp6_reg_histo_blk_v_offset (0x1fff)
	unsigned int reg_histo_blk_v_offset:13;

	// h0024, bit: 14
	/* */
	unsigned int :3;

	// h0024
	unsigned int /* padding 16 bit */:16;

	// h0025, bit: 14
	/* */
	unsigned int :16;

	// h0025
	unsigned int /* padding 16 bit */:16;

	// h0026, bit: 14
	/* */
	unsigned int :16;

	// h0026
	unsigned int /* padding 16 bit */:16;

	// h0027, bit: 14
	/* */
	unsigned int :16;

	// h0027
	unsigned int /* padding 16 bit */:16;

	// h0028, bit: 14
	/* Histogram block average divisor A minus one*/
	#define offset_of_isp6_reg_histo_div_a_m1 (80)
	#define mask_of_isp6_reg_histo_div_a_m1 (0xffff)
	unsigned int reg_histo_div_a_m1:16;

	// h0028
	unsigned int /* padding 16 bit */:16;

	// h0029, bit: 14
	/* Histogram block average divisor B minus one*/
	#define offset_of_isp6_reg_histo_div_b_m1 (82)
	#define mask_of_isp6_reg_histo_div_b_m1 (0xffff)
	unsigned int reg_histo_div_b_m1:16;

	// h0029
	unsigned int /* padding 16 bit */:16;

	// h002a, bit: 14
	/* Histogram block average divisor C minus one*/
	#define offset_of_isp6_reg_histo_div_c_m1 (84)
	#define mask_of_isp6_reg_histo_div_c_m1 (0xffff)
	unsigned int reg_histo_div_c_m1:16;

	// h002a
	unsigned int /* padding 16 bit */:16;

	// h002b, bit: 14
	/* Histogram block average divisor D minus one*/
	#define offset_of_isp6_reg_histo_div_d_m1 (86)
	#define mask_of_isp6_reg_histo_div_d_m1 (0xffff)
	unsigned int reg_histo_div_d_m1:16;

	// h002b
	unsigned int /* padding 16 bit */:16;

	// h002c, bit: 14
	/* Histogram block average divisor E minus one*/
	#define offset_of_isp6_reg_histo_div_e_m1 (88)
	#define mask_of_isp6_reg_histo_div_e_m1 (0xffff)
	unsigned int reg_histo_div_e_m1:16;

	// h002c
	unsigned int /* padding 16 bit */:16;

	// h002d, bit: 14
	/* Histogram block average divisor F minus one*/
	#define offset_of_isp6_reg_histo_div_f_m1 (90)
	#define mask_of_isp6_reg_histo_div_f_m1 (0xffff)
	unsigned int reg_histo_div_f_m1:16;

	// h002d
	unsigned int /* padding 16 bit */:16;

	// h002e, bit: 3
	/* Histogram accumulation shift register*/
	#define offset_of_isp6_reg_histo_acc_sft (92)
	#define mask_of_isp6_reg_histo_acc_sft (0xf)
	unsigned int reg_histo_acc_sft:4;

	// h002e, bit: 14
	/* */
	unsigned int :12;

	// h002e
	unsigned int /* padding 16 bit */:16;

	// h002f, bit: 14
	/* */
	unsigned int :16;

	// h002f
	unsigned int /* padding 16 bit */:16;

	// h0030, bit: 6
	/* Horizontal block offset of histogram window0*/
	#define offset_of_isp6_reg_histo_stawin_x_offset_0 (96)
	#define mask_of_isp6_reg_histo_stawin_x_offset_0 (0x7f)
	unsigned int reg_histo_stawin_x_offset_0:7;

	// h0030, bit: 7
	/* */
	unsigned int :1;

	// h0030, bit: 14
	/* Horizontal block number minus one of histogram window0*/
	#define offset_of_isp6_reg_histo_stawin_x_size_m1_0 (96)
	#define mask_of_isp6_reg_histo_stawin_x_size_m1_0 (0x7f00)
	unsigned int reg_histo_stawin_x_size_m1_0:7;

	// h0030, bit: 15
	/* */
	unsigned int :1;

	// h0030
	unsigned int /* padding 16 bit */:16;

	// h0031, bit: 6
	/* Vertical block offset of histogram window0*/
	#define offset_of_isp6_reg_histo_stawin_y_offset_0 (98)
	#define mask_of_isp6_reg_histo_stawin_y_offset_0 (0x7f)
	unsigned int reg_histo_stawin_y_offset_0:7;

	// h0031, bit: 7
	/* */
	unsigned int :1;

	// h0031, bit: 14
	/* Vertical block number minus one of histogram window0*/
	#define offset_of_isp6_reg_histo_stawin_y_size_m1_0 (98)
	#define mask_of_isp6_reg_histo_stawin_y_size_m1_0 (0x7f00)
	unsigned int reg_histo_stawin_y_size_m1_0:7;

	// h0031, bit: 15
	/* */
	unsigned int :1;

	// h0031
	unsigned int /* padding 16 bit */:16;

	// h0032, bit: 6
	/* Horizontal block offset of histogram window1*/
	#define offset_of_isp6_reg_histo_stawin_x_offset_1 (100)
	#define mask_of_isp6_reg_histo_stawin_x_offset_1 (0x7f)
	unsigned int reg_histo_stawin_x_offset_1:7;

	// h0032, bit: 7
	/* */
	unsigned int :1;

	// h0032, bit: 14
	/* Horizontal block number minus one of histogram window1*/
	#define offset_of_isp6_reg_histo_stawin_x_size_m1_1 (100)
	#define mask_of_isp6_reg_histo_stawin_x_size_m1_1 (0x7f00)
	unsigned int reg_histo_stawin_x_size_m1_1:7;

	// h0032, bit: 15
	/* */
	unsigned int :1;

	// h0032
	unsigned int /* padding 16 bit */:16;

	// h0033, bit: 6
	/* Vertical block offset of histogram window1*/
	#define offset_of_isp6_reg_histo_stawin_y_offset_1 (102)
	#define mask_of_isp6_reg_histo_stawin_y_offset_1 (0x7f)
	unsigned int reg_histo_stawin_y_offset_1:7;

	// h0033, bit: 7
	/* */
	unsigned int :1;

	// h0033, bit: 14
	/* Vertical block number minus one of histogram window1*/
	#define offset_of_isp6_reg_histo_stawin_y_size_m1_1 (102)
	#define mask_of_isp6_reg_histo_stawin_y_size_m1_1 (0x7f00)
	unsigned int reg_histo_stawin_y_size_m1_1:7;

	// h0033, bit: 15
	/* */
	unsigned int :1;

	// h0033
	unsigned int /* padding 16 bit */:16;

	// h0034, bit: 0
	/* Histogram window0 roi mode*/
	#define offset_of_isp6_reg_histo_roi_0_mode (104)
	#define mask_of_isp6_reg_histo_roi_0_mode (0x1)
	unsigned int reg_histo_roi_0_mode:1;

	// h0034, bit: 1
	/* Histogram window1 roi mode*/
	#define offset_of_isp6_reg_histo_roi_1_mode (104)
	#define mask_of_isp6_reg_histo_roi_1_mode (0x2)
	unsigned int reg_histo_roi_1_mode:1;

	// h0034, bit: 14
	/* */
	unsigned int :14;

	// h0034
	unsigned int /* padding 16 bit */:16;

	// h0035, bit: 3
	/* AE source selection
	4'b0000: from alsc (pipe0)
	4'b0001: from alsc (pipe1)
	4'b0010: no used
	4'b0011: no used
	4'b0100: from bwbg (pipe0)
	4'b0101: from bwbg (pipe1)
	4'b0110: no used
	4'b0111: no used */
	#define offset_of_isp6_reg_isp_ae_in_mode (106)
	#define mask_of_isp6_reg_isp_ae_in_mode (0xf)
	unsigned int reg_isp_ae_in_mode:4;

	// h0035, bit: 7
	/* Histo source selection
	4'b0000: from alsc (pipe0)
	4'b0001: from alsc (pipe1)
	4'b0010: no used
	4'b0011: no used
	4'b0100: from bwbg (pipe0)
	4'b0101: from bwbg (pipe1)
	4'b0110: no used
	4'b0111: no used */
	#define offset_of_isp6_reg_isp_histo_in_mode (106)
	#define mask_of_isp6_reg_isp_histo_in_mode (0xf0)
	unsigned int reg_isp_histo_in_mode:4;

	// h0035, bit: 14
	/* */
	unsigned int :8;

	// h0035
	unsigned int /* padding 16 bit */:16;

	// h0036, bit: 14
	/* */
	unsigned int :16;

	// h0036
	unsigned int /* padding 16 bit */:16;

	// h0037, bit: 14
	/* */
	unsigned int :16;

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

	// h0047, bit: 0
	/* Post White Balance Gain Dither Enable*/
	#define offset_of_isp6_reg_wbg1_dith_en (142)
	#define mask_of_isp6_reg_wbg1_dith_en (0x1)
	unsigned int reg_wbg1_dith_en:1;

	// h0047, bit: 14
	/* */
	unsigned int :15;

	// h0047
	unsigned int /* padding 16 bit */:16;

	// h0048, bit: 12
	/* Post White Balance Gain for R channel (3.10)
	 ( double buffer register )*/
	#define offset_of_isp6_reg_isp_wbg1_r_gain (144)
	#define mask_of_isp6_reg_isp_wbg1_r_gain (0x1fff)
	unsigned int reg_isp_wbg1_r_gain:13;

	// h0048, bit: 14
	/* */
	unsigned int :2;

	// h0048, bit: 15
	/* Post White Balance Offset for R channel (s10)
	 ( double buffer register )*/
	#define offset_of_isp6_reg_isp_wbg1_r_offset_b16 (144)
	#define mask_of_isp6_reg_isp_wbg1_r_offset_b16 (0x8000)
	unsigned int reg_isp_wbg1_r_offset_b16:1;

	// h0048
	unsigned int /* padding 16 bit */:16;

	// h0049, bit: 14
	/* Post White Balance Offset for R channel (s10)
	 ( double buffer register )*/
	#define offset_of_isp6_reg_isp_wbg1_r_offset (146)
	#define mask_of_isp6_reg_isp_wbg1_r_offset (0xffff)
	unsigned int reg_isp_wbg1_r_offset:16;

	// h0049
	unsigned int /* padding 16 bit */:16;

	// h004a, bit: 12
	/* Post White Balance Gain for Gr channel (3.10)
	 ( double buffer register )*/
	#define offset_of_isp6_reg_isp_wbg1_gr_gain (148)
	#define mask_of_isp6_reg_isp_wbg1_gr_gain (0x1fff)
	unsigned int reg_isp_wbg1_gr_gain:13;

	// h004a, bit: 14
	/* */
	unsigned int :2;

	// h004a, bit: 15
	/* Post White Balance Offset for Gr channel (s10)
	 ( double buffer register )*/
	#define offset_of_isp6_reg_isp_wbg1_gr_offset_b16 (148)
	#define mask_of_isp6_reg_isp_wbg1_gr_offset_b16 (0x8000)
	unsigned int reg_isp_wbg1_gr_offset_b16:1;

	// h004a
	unsigned int /* padding 16 bit */:16;

	// h004b, bit: 14
	/* Post White Balance Offset for Gr channel (s10)
	 ( double buffer register )*/
	#define offset_of_isp6_reg_isp_wbg1_gr_offset (150)
	#define mask_of_isp6_reg_isp_wbg1_gr_offset (0xffff)
	unsigned int reg_isp_wbg1_gr_offset:16;

	// h004b
	unsigned int /* padding 16 bit */:16;

	// h004c, bit: 12
	/* Post White Balance Gain for B channel (3.10)
	 ( double buffer register )*/
	#define offset_of_isp6_reg_isp_wbg1_b_gain (152)
	#define mask_of_isp6_reg_isp_wbg1_b_gain (0x1fff)
	unsigned int reg_isp_wbg1_b_gain:13;

	// h004c, bit: 14
	/* */
	unsigned int :2;

	// h004c, bit: 15
	/* Post White Balance Offset for B channel (s10)
	 ( double buffer register )*/
	#define offset_of_isp6_reg_isp_wbg1_b_offset_b16 (152)
	#define mask_of_isp6_reg_isp_wbg1_b_offset_b16 (0x8000)
	unsigned int reg_isp_wbg1_b_offset_b16:1;

	// h004c
	unsigned int /* padding 16 bit */:16;

	// h004d, bit: 14
	/* Post White Balance Offset for B channel (s10)
	 ( double buffer register )*/
	#define offset_of_isp6_reg_isp_wbg1_b_offset (154)
	#define mask_of_isp6_reg_isp_wbg1_b_offset (0xffff)
	unsigned int reg_isp_wbg1_b_offset:16;

	// h004d
	unsigned int /* padding 16 bit */:16;

	// h004e, bit: 12
	/* Post White Balance Gain for Gb channel (3.10)
	 ( double buffer register )*/
	#define offset_of_isp6_reg_isp_wbg1_gb_gain (156)
	#define mask_of_isp6_reg_isp_wbg1_gb_gain (0x1fff)
	unsigned int reg_isp_wbg1_gb_gain:13;

	// h004e, bit: 14
	/* */
	unsigned int :2;

	// h004e, bit: 15
	/* Post White Balance Offset for Gb channel (s10)
	 ( double buffer register )*/
	#define offset_of_isp6_reg_isp_wbg1_gb_offset_b16 (156)
	#define mask_of_isp6_reg_isp_wbg1_gb_offset_b16 (0x8000)
	unsigned int reg_isp_wbg1_gb_offset_b16:1;

	// h004e
	unsigned int /* padding 16 bit */:16;

	// h004f, bit: 14
	/* Post White Balance Offset for Gb channel (s10)
	 ( double buffer register )*/
	#define offset_of_isp6_reg_isp_wbg1_gb_offset (158)
	#define mask_of_isp6_reg_isp_wbg1_gb_offset (0xffff)
	unsigned int reg_isp_wbg1_gb_offset:16;

	// h004f
	unsigned int /* padding 16 bit */:16;

	// h0050, bit: 0
	/* group 8 round robin enable*/
	#define offset_of_isp6_reg_rq8_round_robin (160)
	#define mask_of_isp6_reg_rq8_round_robin (0x1)
	unsigned int reg_rq8_round_robin:1;

	// h0050, bit: 1
	/* group 8 set priority pulse*/
	#define offset_of_isp6_reg_rq8_set_priority (160)
	#define mask_of_isp6_reg_rq8_set_priority (0x2)
	unsigned int reg_rq8_set_priority:1;

	// h0050, bit: 2
	/* group 8 member limit enable*/
	#define offset_of_isp6_reg_rq8_member_limit_en (160)
	#define mask_of_isp6_reg_rq8_member_limit_en (0x4)
	unsigned int reg_rq8_member_limit_en:1;

	// h0050, bit: 3
	/* group 8 group limit enable*/
	#define offset_of_isp6_reg_rq8_group_limit_en (160)
	#define mask_of_isp6_reg_rq8_group_limit_en (0x8)
	unsigned int reg_rq8_group_limit_en:1;

	// h0050, bit: 4
	/* group 8 timeout enable*/
	#define offset_of_isp6_reg_rq8_timeout_en (160)
	#define mask_of_isp6_reg_rq8_timeout_en (0x10)
	unsigned int reg_rq8_timeout_en:1;

	// h0050, bit: 5
	/* group 8 deadline enable*/
	#define offset_of_isp6_reg_rq8_group_deadline_en (160)
	#define mask_of_isp6_reg_rq8_group_deadline_en (0x20)
	unsigned int reg_rq8_group_deadline_en:1;

	// h0050, bit: 6
	/* group 8 ceaseless enable*/
	#define offset_of_isp6_reg_rq8_ceaseless_en (160)
	#define mask_of_isp6_reg_rq8_ceaseless_en (0x40)
	unsigned int reg_rq8_ceaseless_en:1;

	// h0050, bit: 7
	/* group 8 limit with last enable*/
	#define offset_of_isp6_reg_rq8_limit_with_last (160)
	#define mask_of_isp6_reg_rq8_limit_with_last (0x80)
	unsigned int reg_rq8_limit_with_last:1;

	// h0050, bit: 8
	/* group 8 flow control 0 enable*/
	#define offset_of_isp6_reg_rq8_cnt0_ctrl_en (160)
	#define mask_of_isp6_reg_rq8_cnt0_ctrl_en (0x100)
	unsigned int reg_rq8_cnt0_ctrl_en:1;

	// h0050, bit: 9
	/* group 8 flow control 1 enable*/
	#define offset_of_isp6_reg_rq8_cnt1_ctrl_en (160)
	#define mask_of_isp6_reg_rq8_cnt1_ctrl_en (0x200)
	unsigned int reg_rq8_cnt1_ctrl_en:1;

	// h0050, bit: 11
	/* group 8 order control ratio*/
	#define offset_of_isp6_reg_rq8_order_ctrl_ratio (160)
	#define mask_of_isp6_reg_rq8_order_ctrl_ratio (0xc00)
	unsigned int reg_rq8_order_ctrl_ratio:2;

	// h0050, bit: 12
	/* group 8 last_done_z off*/
	#define offset_of_isp6_reg_rq8_last_done_z_off (160)
	#define mask_of_isp6_reg_rq8_last_done_z_off (0x1000)
	unsigned int reg_rq8_last_done_z_off:1;

	// h0050, bit: 13
	/* group 8 pre-arbiter cut in enable*/
	#define offset_of_isp6_reg_rq8_member_cut_in_en (160)
	#define mask_of_isp6_reg_rq8_member_cut_in_en (0x2000)
	unsigned int reg_rq8_member_cut_in_en:1;

	// h0050, bit: 14
	/* group 8 group cut in enable*/
	#define offset_of_isp6_reg_rq8_group_cut_in_en (160)
	#define mask_of_isp6_reg_rq8_group_cut_in_en (0x4000)
	unsigned int reg_rq8_group_cut_in_en:1;

	// h0050, bit: 15
	/* group 8 skip empty ready on*/
	#define offset_of_isp6_reg_rq8_arbiter_skip_on (160)
	#define mask_of_isp6_reg_rq8_arbiter_skip_on (0x8000)
	unsigned int reg_rq8_arbiter_skip_on:1;

	// h0050
	unsigned int /* padding 16 bit */:16;

	// h0051, bit: 7
	/* group 8 member max service number, unit 4*/
	#define offset_of_isp6_reg_rq8_member_max (162)
	#define mask_of_isp6_reg_rq8_member_max (0xff)
	unsigned int reg_rq8_member_max:8;

	// h0051, bit: 14
	/* group 8 group max service nember, unit 4*/
	#define offset_of_isp6_reg_rq8_group_max (162)
	#define mask_of_isp6_reg_rq8_group_max (0xff00)
	unsigned int reg_rq8_group_max:8;

	// h0051
	unsigned int /* padding 16 bit */:16;

	// h0052, bit: 14
	/* group 8 time out number*/
	#define offset_of_isp6_reg_rq8_timeout (164)
	#define mask_of_isp6_reg_rq8_timeout (0xffff)
	unsigned int reg_rq8_timeout:16;

	// h0052
	unsigned int /* padding 16 bit */:16;

	// h0053, bit: 14
	/* group 8 request mask*/
	#define offset_of_isp6_reg_rq8_mask (166)
	#define mask_of_isp6_reg_rq8_mask (0xffff)
	unsigned int reg_rq8_mask:16;

	// h0053
	unsigned int /* padding 16 bit */:16;

	// h0054, bit: 14
	/* group 8 high priority mask*/
	#define offset_of_isp6_reg_rq8_hpmask (168)
	#define mask_of_isp6_reg_rq8_hpmask (0xffff)
	unsigned int reg_rq8_hpmask:16;

	// h0054
	unsigned int /* padding 16 bit */:16;

	// h0055, bit: 3
	/* group 8 1st priority id*/
	#define offset_of_isp6_reg_rq80_priority (170)
	#define mask_of_isp6_reg_rq80_priority (0xf)
	unsigned int reg_rq80_priority:4;

	// h0055, bit: 7
	/* group 8 2nd priority id*/
	#define offset_of_isp6_reg_rq81_priority (170)
	#define mask_of_isp6_reg_rq81_priority (0xf0)
	unsigned int reg_rq81_priority:4;

	// h0055, bit: 11
	/* group 8 3rd priority id*/
	#define offset_of_isp6_reg_rq82_priority (170)
	#define mask_of_isp6_reg_rq82_priority (0xf00)
	unsigned int reg_rq82_priority:4;

	// h0055, bit: 14
	/* group 8 4th priority id*/
	#define offset_of_isp6_reg_rq83_priority (170)
	#define mask_of_isp6_reg_rq83_priority (0xf000)
	unsigned int reg_rq83_priority:4;

	// h0055
	unsigned int /* padding 16 bit */:16;

	// h0056, bit: 3
	/* group 8 5th priority id*/
	#define offset_of_isp6_reg_rq84_priority (172)
	#define mask_of_isp6_reg_rq84_priority (0xf)
	unsigned int reg_rq84_priority:4;

	// h0056, bit: 7
	/* group 8 6th priority id*/
	#define offset_of_isp6_reg_rq85_priority (172)
	#define mask_of_isp6_reg_rq85_priority (0xf0)
	unsigned int reg_rq85_priority:4;

	// h0056, bit: 11
	/* group 8 7th priority id*/
	#define offset_of_isp6_reg_rq86_priority (172)
	#define mask_of_isp6_reg_rq86_priority (0xf00)
	unsigned int reg_rq86_priority:4;

	// h0056, bit: 14
	/* group 8 8th priority id*/
	#define offset_of_isp6_reg_rq87_priority (172)
	#define mask_of_isp6_reg_rq87_priority (0xf000)
	unsigned int reg_rq87_priority:4;

	// h0056
	unsigned int /* padding 16 bit */:16;

	// h0057, bit: 3
	/* group 8 9th priority id*/
	#define offset_of_isp6_reg_rq88_priority (174)
	#define mask_of_isp6_reg_rq88_priority (0xf)
	unsigned int reg_rq88_priority:4;

	// h0057, bit: 7
	/* group 8 10th priority id*/
	#define offset_of_isp6_reg_rq89_priority (174)
	#define mask_of_isp6_reg_rq89_priority (0xf0)
	unsigned int reg_rq89_priority:4;

	// h0057, bit: 11
	/* group 8 11th priority id*/
	#define offset_of_isp6_reg_rq8a_priority (174)
	#define mask_of_isp6_reg_rq8a_priority (0xf00)
	unsigned int reg_rq8a_priority:4;

	// h0057, bit: 14
	/* group 8 12th priority id*/
	#define offset_of_isp6_reg_rq8b_priority (174)
	#define mask_of_isp6_reg_rq8b_priority (0xf000)
	unsigned int reg_rq8b_priority:4;

	// h0057
	unsigned int /* padding 16 bit */:16;

	// h0058, bit: 3
	/* group 8 13th priority id*/
	#define offset_of_isp6_reg_rq8c_priority (176)
	#define mask_of_isp6_reg_rq8c_priority (0xf)
	unsigned int reg_rq8c_priority:4;

	// h0058, bit: 7
	/* group 8 14th priority id*/
	#define offset_of_isp6_reg_rq8d_priority (176)
	#define mask_of_isp6_reg_rq8d_priority (0xf0)
	unsigned int reg_rq8d_priority:4;

	// h0058, bit: 11
	/* group 8 15th priority id*/
	#define offset_of_isp6_reg_rq8e_priority (176)
	#define mask_of_isp6_reg_rq8e_priority (0xf00)
	unsigned int reg_rq8e_priority:4;

	// h0058, bit: 14
	/* group 8 16th priority id*/
	#define offset_of_isp6_reg_rq8f_priority (176)
	#define mask_of_isp6_reg_rq8f_priority (0xf000)
	unsigned int reg_rq8f_priority:4;

	// h0058
	unsigned int /* padding 16 bit */:16;

	// h0059, bit: 14
	/* group 8 last done z check in select*/
	#define offset_of_isp6_reg_rq8_last_done_z_sel (178)
	#define mask_of_isp6_reg_rq8_last_done_z_sel (0xffff)
	unsigned int reg_rq8_last_done_z_sel:16;

	// h0059
	unsigned int /* padding 16 bit */:16;

	// h005a, bit: 3
	/* group 8 flow control 0 id0*/
	#define offset_of_isp6_reg_rq8_cnt0_id0 (180)
	#define mask_of_isp6_reg_rq8_cnt0_id0 (0xf)
	unsigned int reg_rq8_cnt0_id0:4;

	// h005a, bit: 7
	/* group 8 flow control 0 id1*/
	#define offset_of_isp6_reg_rq8_cnt0_id1 (180)
	#define mask_of_isp6_reg_rq8_cnt0_id1 (0xf0)
	unsigned int reg_rq8_cnt0_id1:4;

	// h005a, bit: 14
	/* group 8 flow control 0 period number*/
	#define offset_of_isp6_reg_rq8_cnt0_period (180)
	#define mask_of_isp6_reg_rq8_cnt0_period (0xff00)
	unsigned int reg_rq8_cnt0_period:8;

	// h005a
	unsigned int /* padding 16 bit */:16;

	// h005b, bit: 3
	/* group 8 flow control 1 id0*/
	#define offset_of_isp6_reg_rq8_cnt1_id0 (182)
	#define mask_of_isp6_reg_rq8_cnt1_id0 (0xf)
	unsigned int reg_rq8_cnt1_id0:4;

	// h005b, bit: 7
	/* group 8 flow control 1 id1*/
	#define offset_of_isp6_reg_rq8_cnt1_id1 (182)
	#define mask_of_isp6_reg_rq8_cnt1_id1 (0xf0)
	unsigned int reg_rq8_cnt1_id1:4;

	// h005b, bit: 14
	/* group 8 flow control 1 period number*/
	#define offset_of_isp6_reg_rq8_cnt1_period (182)
	#define mask_of_isp6_reg_rq8_cnt1_period (0xff00)
	unsigned int reg_rq8_cnt1_period:8;

	// h005b
	unsigned int /* padding 16 bit */:16;

	// h005c, bit: 14
	/* group 8 type priority select*/
	#define offset_of_isp6_reg_rq8_type_priority_sel (184)
	#define mask_of_isp6_reg_rq8_type_priority_sel (0xffff)
	unsigned int reg_rq8_type_priority_sel:16;

	// h005c
	unsigned int /* padding 16 bit */:16;

	// h005d, bit: 14
	/* request group 8 order control*/
	#define offset_of_isp6_reg_rq8_order_ctrl_en (186)
	#define mask_of_isp6_reg_rq8_order_ctrl_en (0xffff)
	unsigned int reg_rq8_order_ctrl_en:16;

	// h005d
	unsigned int /* padding 16 bit */:16;

	// h005e, bit: 14
	/* group 8 client limit mask*/
	#define offset_of_isp6_reg_rq8_limit_mask (188)
	#define mask_of_isp6_reg_rq8_limit_mask (0xffff)
	unsigned int reg_rq8_limit_mask:16;

	// h005e
	unsigned int /* padding 16 bit */:16;

	// h005f, bit: 7
	/* group 8 deadline timer number, unit 64*/
	#define offset_of_isp6_reg_rq8_group_deadline (190)
	#define mask_of_isp6_reg_rq8_group_deadline (0xff)
	unsigned int reg_rq8_group_deadline:8;

	// h005f, bit: 14
	/* group 8 deadline timer number, unit 64*/
	#define offset_of_isp6_reg_reserved_0f (190)
	#define mask_of_isp6_reg_reserved_0f (0xff00)
	unsigned int reg_reserved_0f:8;

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

}  __attribute__((packed, aligned(1))) infinity2_reg_isp6;
#endif
