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
#ifndef __INFINITY3_REG_ISP9__
#define __INFINITY3_REG_ISP9__
typedef struct {
	// h0000, bit: 0
	/* HDR 16to12 gain  enable*/
	unsigned int reg_hdr_16to12_gain_en:1;

	// h0000, bit: 1
	/* HDR 16to12 gain smooth enable*/
	unsigned int reg_hdr_16to12_gbl_smooth_en:1;

	// h0000, bit: 5
	/* HDR 16to12 gain base*/
	unsigned int reg_hdr_16to12_gain_base:4;

	// h0000, bit: 14
	/* */
	unsigned int :10;

	// h0000
	unsigned int /* padding 16 bit */:16;

	// h0001, bit: 14
	/* HDR 16to12 gain low bound*/
	unsigned int reg_hdr_16to12_gain_lw:16;

	// h0001
	unsigned int /* padding 16 bit */:16;

	// h0002, bit: 14
	/* HDR 16to12 gain high bound*/
	unsigned int reg_hdr_16to12_gain_hi:16;

	// h0002
	unsigned int /* padding 16 bit */:16;

	// h0003, bit: 14
	/* HDR 16to12 offset for R*/
	unsigned int reg_hdr_16to12_offset_r:16;

	// h0003
	unsigned int /* padding 16 bit */:16;

	// h0004, bit: 14
	/* HDR 16to12 offset for G*/
	unsigned int reg_hdr_16to12_offset_g:16;

	// h0004
	unsigned int /* padding 16 bit */:16;

	// h0005, bit: 14
	/* HDR 16to12 offset for B*/
	unsigned int reg_hdr_16to12_offset_b:16;

	// h0005
	unsigned int /* padding 16 bit */:16;

	// h0006, bit: 3
	/* HDR 16to12 curve x 0*/
	unsigned int reg_hdr_16to12_curve_x_0:4;

	// h0006, bit: 7
	/* HDR 16to12 curve x 1*/
	unsigned int reg_hdr_16to12_curve_x_1:4;

	// h0006, bit: 11
	/* HDR 16to12 curve x 2*/
	unsigned int reg_hdr_16to12_curve_x_2:4;

	// h0006, bit: 14
	/* HDR 16to12 curve x 3*/
	unsigned int reg_hdr_16to12_curve_x_3:4;

	// h0006
	unsigned int /* padding 16 bit */:16;

	// h0007, bit: 3
	/* HDR 16to12 curve x 4*/
	unsigned int reg_hdr_16to12_curve_x_4:4;

	// h0007, bit: 7
	/* HDR 16to12 curve x 5*/
	unsigned int reg_hdr_16to12_curve_x_5:4;

	// h0007, bit: 11
	/* HDR 16to12 curve x 6*/
	unsigned int reg_hdr_16to12_curve_x_6:4;

	// h0007, bit: 14
	/* HDR 16to12 curve x 7*/
	unsigned int reg_hdr_16to12_curve_x_7:4;

	// h0007
	unsigned int /* padding 16 bit */:16;

	// h0008, bit: 3
	/* HDR 16to12 curve x 8*/
	unsigned int reg_hdr_16to12_curve_x_8:4;

	// h0008, bit: 7
	/* HDR 16to12 curve x 9*/
	unsigned int reg_hdr_16to12_curve_x_9:4;

	// h0008, bit: 11
	/* HDR 16to12 curve x 10*/
	unsigned int reg_hdr_16to12_curve_x_10:4;

	// h0008, bit: 14
	/* HDR 16to12 curve x 11*/
	unsigned int reg_hdr_16to12_curve_x_11:4;

	// h0008
	unsigned int /* padding 16 bit */:16;

	// h0009, bit: 3
	/* HDR 16to12 curve x 12*/
	unsigned int reg_hdr_16to12_curve_x_12:4;

	// h0009, bit: 7
	/* HDR 16to12 curve x 13*/
	unsigned int reg_hdr_16to12_curve_x_13:4;

	// h0009, bit: 11
	/* HDR 16to12 curve x 14*/
	unsigned int reg_hdr_16to12_curve_x_14:4;

	// h0009, bit: 14
	/* HDR 16to12 curve x 15*/
	unsigned int reg_hdr_16to12_curve_x_15:4;

	// h0009
	unsigned int /* padding 16 bit */:16;

	// h000a, bit: 3
	/* HDR 16to12 curve x 16*/
	unsigned int reg_hdr_16to12_curve_x_16:4;

	// h000a, bit: 7
	/* HDR 16to12 curve x 17*/
	unsigned int reg_hdr_16to12_curve_x_17:4;

	// h000a, bit: 11
	/* HDR 16to12 curve x 18*/
	unsigned int reg_hdr_16to12_curve_x_18:4;

	// h000a, bit: 14
	/* HDR 16to12 curve x 19*/
	unsigned int reg_hdr_16to12_curve_x_19:4;

	// h000a
	unsigned int /* padding 16 bit */:16;

	// h000b, bit: 14
	/* HDR 16to12 curve 0*/
	unsigned int reg_hdr_16to12_curve_0:16;

	// h000b
	unsigned int /* padding 16 bit */:16;

	// h000c, bit: 14
	/* HDR 16to12 curve 1*/
	unsigned int reg_hdr_16to12_curve_1:16;

	// h000c
	unsigned int /* padding 16 bit */:16;

	// h000d, bit: 14
	/* HDR 16to12 curve 2*/
	unsigned int reg_hdr_16to12_curve_2:16;

	// h000d
	unsigned int /* padding 16 bit */:16;

	// h000e, bit: 14
	/* HDR 16to12 curve 3*/
	unsigned int reg_hdr_16to12_curve_3:16;

	// h000e
	unsigned int /* padding 16 bit */:16;

	// h000f, bit: 14
	/* HDR 16to12 curve 4*/
	unsigned int reg_hdr_16to12_curve_4:16;

	// h000f
	unsigned int /* padding 16 bit */:16;

	// h0010, bit: 14
	/* HDR 16to12 curve 5*/
	unsigned int reg_hdr_16to12_curve_5:16;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 14
	/* HDR 16to12 curve 6*/
	unsigned int reg_hdr_16to12_curve_6:16;

	// h0011
	unsigned int /* padding 16 bit */:16;

	// h0012, bit: 14
	/* HDR 16to12 curve 7*/
	unsigned int reg_hdr_16to12_curve_7:16;

	// h0012
	unsigned int /* padding 16 bit */:16;

	// h0013, bit: 14
	/* HDR 16to12 curve 8*/
	unsigned int reg_hdr_16to12_curve_8:16;

	// h0013
	unsigned int /* padding 16 bit */:16;

	// h0014, bit: 14
	/* HDR 16to12 curve 9*/
	unsigned int reg_hdr_16to12_curve_9:16;

	// h0014
	unsigned int /* padding 16 bit */:16;

	// h0015, bit: 14
	/* HDR 16to12 curve 10*/
	unsigned int reg_hdr_16to12_curve_10:16;

	// h0015
	unsigned int /* padding 16 bit */:16;

	// h0016, bit: 14
	/* HDR 16to12 curve 11*/
	unsigned int reg_hdr_16to12_curve_11:16;

	// h0016
	unsigned int /* padding 16 bit */:16;

	// h0017, bit: 14
	/* HDR 16to12 curve 12*/
	unsigned int reg_hdr_16to12_curve_12:16;

	// h0017
	unsigned int /* padding 16 bit */:16;

	// h0018, bit: 14
	/* HDR 16to12 curve 13*/
	unsigned int reg_hdr_16to12_curve_13:16;

	// h0018
	unsigned int /* padding 16 bit */:16;

	// h0019, bit: 14
	/* HDR 16to12 curve 14*/
	unsigned int reg_hdr_16to12_curve_14:16;

	// h0019
	unsigned int /* padding 16 bit */:16;

	// h001a, bit: 14
	/* HDR 16to12 curve 15*/
	unsigned int reg_hdr_16to12_curve_15:16;

	// h001a
	unsigned int /* padding 16 bit */:16;

	// h001b, bit: 14
	/* HDR 16to12 curve 16*/
	unsigned int reg_hdr_16to12_curve_16:16;

	// h001b
	unsigned int /* padding 16 bit */:16;

	// h001c, bit: 14
	/* HDR 16to12 curve 17*/
	unsigned int reg_hdr_16to12_curve_17:16;

	// h001c
	unsigned int /* padding 16 bit */:16;

	// h001d, bit: 14
	/* HDR 16to12 curve 18*/
	unsigned int reg_hdr_16to12_curve_18:16;

	// h001d
	unsigned int /* padding 16 bit */:16;

	// h001e, bit: 14
	/* HDR 16to12 curve 19*/
	unsigned int reg_hdr_16to12_curve_19:16;

	// h001e
	unsigned int /* padding 16 bit */:16;

	// h001f, bit: 14
	/* HDR 16to12 curve 20*/
	unsigned int reg_hdr_16to12_curve_20:16;

	// h001f
	unsigned int /* padding 16 bit */:16;

	// h0020, bit: 0
	/* Histogram enable*/
	unsigned int reg_hdr_16to12_hist_en:1;

	// h0020, bit: 14
	/* */
	unsigned int :15;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 11
	/* Histogram block offsetx*/
	unsigned int reg_hdr_16to12_hist_offsetx:12;

	// h0021, bit: 14
	/* */
	unsigned int :4;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 11
	/* Histogram block offsety*/
	unsigned int reg_hdr_16to12_hist_offsety:12;

	// h0022, bit: 14
	/* */
	unsigned int :4;

	// h0022
	unsigned int /* padding 16 bit */:16;

	// h0023, bit: 7
	/* Block Width= (reg_hist_blk_width + 1)*/
	unsigned int reg_hdr_16to12_hist_blk_width:8;

	// h0023, bit: 14
	/* Block Height= (reg_hist_blk_height + 1)*/
	unsigned int reg_hdr_16to12_hist_blk_height:8;

	// h0023
	unsigned int /* padding 16 bit */:16;

	// h0024, bit: 7
	/* ROI offsetx (unit: ROI)*/
	unsigned int reg_hdr_16to12_roi_offsetx:8;

	// h0024, bit: 14
	/* ROI offsety (unit: ROI)*/
	unsigned int reg_hdr_16to12_roi_offsety:8;

	// h0024
	unsigned int /* padding 16 bit */:16;

	// h0025, bit: 6
	/* ROI X size = (reg_roi_sizex + 1) ROI*/
	unsigned int reg_hdr_16to12_roi_sizex:7;

	// h0025, bit: 7
	/* */
	unsigned int :1;

	// h0025, bit: 14
	/* ROI Y size = (reg_roi_sizey + 1) ROI*/
	unsigned int reg_hdr_16to12_roi_sizey:8;

	// h0025
	unsigned int /* padding 16 bit */:16;

	// h0026, bit: 14
	/* Hist = (sum_ir * reg_roi_div) >> (16+4)*/
	unsigned int reg_hdr_16to12_roi_div:16;

	// h0026
	unsigned int /* padding 16 bit */:16;

	// h0027, bit: 14
	/* MIU Base address (total 256 bytes)*/
	unsigned int reg_hdr_16to12_miu_base_addr_0:16;

	// h0027
	unsigned int /* padding 16 bit */:16;

	// h0028, bit: 14
	/* MIU Base address (total 256 bytes)*/
	unsigned int reg_hdr_16to12_miu_base_addr_1:16;

	// h0028
	unsigned int /* padding 16 bit */:16;

	// h0029, bit: 1
	/* Histogram arbiter mi lenx4*/
	unsigned int reg_hdr_16to12_mi_lenx4:2;

	// h0029, bit: 7
	/* Histogram arbiter mi mask cycx4*/
	unsigned int reg_hdr_16to12_mi_mask_cycx4:6;

	// h0029, bit: 14
	/* */
	unsigned int :8;

	// h0029
	unsigned int /* padding 16 bit */:16;

	// h002a, bit: 14
	/* Histogram arbiter max wadr*/
	unsigned int reg_hdr_16to12_max_wadr_0:16;

	// h002a
	unsigned int /* padding 16 bit */:16;

	// h002b, bit: 10
	/* Histogram arbiter max wadr*/
	unsigned int reg_hdr_16to12_max_wadr_1:11;

	// h002b, bit: 14
	/* */
	unsigned int :5;

	// h002b
	unsigned int /* padding 16 bit */:16;

	// h002c, bit: 14
	/* */
	unsigned int :16;

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

	// h0030, bit: 14
	/* */
	unsigned int :16;

	// h0030
	unsigned int /* padding 16 bit */:16;

	// h0031, bit: 14
	/* */
	unsigned int :16;

	// h0031
	unsigned int /* padding 16 bit */:16;

	// h0032, bit: 14
	/* */
	unsigned int :16;

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
	/* For opr test*/
	unsigned int reg_hdr_16to12_dummy:16;

	// h007f
	unsigned int /* padding 16 bit */:16;

}  __attribute__((packed, aligned(1))) bank_9;
#endif
