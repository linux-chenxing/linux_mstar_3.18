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
#ifndef __INFINITY3_REG_ISP8__
#define __INFINITY3_REG_ISP8__
typedef struct {
	// h0000, bit: 0
	/* RGB-IR enable*/
	unsigned int reg_rgbir_en:1;

	// h0000, bit: 1
	/* 4X4 mode*/
	unsigned int reg_mode4x4:1;

	// h0000, bit: 2
	/* 2X2 used IR_mode*/
	unsigned int reg_ir_mode:1;

	// h0000, bit: 3
	/* remove-IR offset enable*/
	unsigned int reg_ir_offset_en:1;

	// h0000, bit: 4
	/* IR histogram enable*/
	unsigned int reg_hist_ir_en:1;

	// h0000, bit: 5
	/* I Monitor enable*/
	unsigned int reg_imonitor_en:1;

	// h0000, bit: 14
	/* */
	unsigned int :10;

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

	// h0004, bit: 3
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_ratio_by_y_shift_0:4;

	// h0004, bit: 7
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_ratio_by_y_shift_1:4;

	// h0004, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_ratio_by_y_shift_2:4;

	// h0004, bit: 14
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_ratio_by_y_shift_3:4;

	// h0004
	unsigned int /* padding 16 bit */:16;

	// h0005, bit: 3
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_ratio_by_y_shift_4:4;

	// h0005, bit: 14
	/* */
	unsigned int :12;

	// h0005
	unsigned int /* padding 16 bit */:16;

	// h0006, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_r_ratio_by_y_0:12;

	// h0006, bit: 14
	/* */
	unsigned int :4;

	// h0006
	unsigned int /* padding 16 bit */:16;

	// h0007, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_r_ratio_by_y_1:12;

	// h0007, bit: 14
	/* */
	unsigned int :4;

	// h0007
	unsigned int /* padding 16 bit */:16;

	// h0008, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_r_ratio_by_y_2:12;

	// h0008, bit: 14
	/* */
	unsigned int :4;

	// h0008
	unsigned int /* padding 16 bit */:16;

	// h0009, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_r_ratio_by_y_3:12;

	// h0009, bit: 14
	/* */
	unsigned int :4;

	// h0009
	unsigned int /* padding 16 bit */:16;

	// h000a, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_r_ratio_by_y_4:12;

	// h000a, bit: 14
	/* */
	unsigned int :4;

	// h000a
	unsigned int /* padding 16 bit */:16;

	// h000b, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_r_ratio_by_y_5:12;

	// h000b, bit: 14
	/* */
	unsigned int :4;

	// h000b
	unsigned int /* padding 16 bit */:16;

	// h000c, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_g_ratio_by_y_0:12;

	// h000c, bit: 14
	/* */
	unsigned int :4;

	// h000c
	unsigned int /* padding 16 bit */:16;

	// h000d, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_g_ratio_by_y_1:12;

	// h000d, bit: 14
	/* */
	unsigned int :4;

	// h000d
	unsigned int /* padding 16 bit */:16;

	// h000e, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_g_ratio_by_y_2:12;

	// h000e, bit: 14
	/* */
	unsigned int :4;

	// h000e
	unsigned int /* padding 16 bit */:16;

	// h000f, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_g_ratio_by_y_3:12;

	// h000f, bit: 14
	/* */
	unsigned int :4;

	// h000f
	unsigned int /* padding 16 bit */:16;

	// h0010, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_g_ratio_by_y_4:12;

	// h0010, bit: 14
	/* */
	unsigned int :4;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_g_ratio_by_y_5:12;

	// h0011, bit: 14
	/* */
	unsigned int :4;

	// h0011
	unsigned int /* padding 16 bit */:16;

	// h0012, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_b_ratio_by_y_0:12;

	// h0012, bit: 14
	/* */
	unsigned int :4;

	// h0012
	unsigned int /* padding 16 bit */:16;

	// h0013, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_b_ratio_by_y_1:12;

	// h0013, bit: 14
	/* */
	unsigned int :4;

	// h0013
	unsigned int /* padding 16 bit */:16;

	// h0014, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_b_ratio_by_y_2:12;

	// h0014, bit: 14
	/* */
	unsigned int :4;

	// h0014
	unsigned int /* padding 16 bit */:16;

	// h0015, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_b_ratio_by_y_3:12;

	// h0015, bit: 14
	/* */
	unsigned int :4;

	// h0015
	unsigned int /* padding 16 bit */:16;

	// h0016, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_b_ratio_by_y_4:12;

	// h0016, bit: 14
	/* */
	unsigned int :4;

	// h0016
	unsigned int /* padding 16 bit */:16;

	// h0017, bit: 11
	/* 4X4 Remove-IR ratio*/
	unsigned int reg_ir_oft_b_ratio_by_y_5:12;

	// h0017, bit: 14
	/* */
	unsigned int :4;

	// h0017
	unsigned int /* padding 16 bit */:16;

	// h0018, bit: 14
	/* dummy bits*/
	unsigned int reg_ir_dummy:16;

	// h0018
	unsigned int /* padding 16 bit */:16;

	// h0019, bit: 11
	/* 4x4 compare ratio*/
	unsigned int reg_ir_oft_cmp_ratio:12;

	// h0019, bit: 14
	/* 4x4 dirH/dirV scaler*/
	unsigned int reg_ir_dir_scaler:4;

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

	// h0020, bit: 14
	/* */
	unsigned int :16;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 14
	/* */
	unsigned int :16;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 11
	/* Histogram block offsetx*/
	unsigned int reg_hist_offsetx:12;

	// h0022, bit: 14
	/* */
	unsigned int :4;

	// h0022
	unsigned int /* padding 16 bit */:16;

	// h0023, bit: 11
	/* Histogram block offsety*/
	unsigned int reg_hist_offsety:12;

	// h0023, bit: 14
	/* */
	unsigned int :4;

	// h0023
	unsigned int /* padding 16 bit */:16;

	// h0024, bit: 7
	/* Block Width= (reg_hist_blk_width + 1)*/
	unsigned int reg_hist_blk_width:8;

	// h0024, bit: 14
	/* Block Height= (reg_hist_blk_height + 1)*/
	unsigned int reg_hist_blk_height:8;

	// h0024
	unsigned int /* padding 16 bit */:16;

	// h0025, bit: 7
	/* ROI offsetx (unit: ROI)*/
	unsigned int reg_roi_offsetx:8;

	// h0025, bit: 14
	/* ROI offsety (unit: ROI)*/
	unsigned int reg_roi_offsety:8;

	// h0025
	unsigned int /* padding 16 bit */:16;

	// h0026, bit: 6
	/* ROI X size = (reg_roi_sizex + 1) ROI*/
	unsigned int reg_roi_sizex:7;

	// h0026, bit: 7
	/* */
	unsigned int :1;

	// h0026, bit: 14
	/* ROI Y size = (reg_roi_sizey + 1) ROI*/
	unsigned int reg_roi_sizey:8;

	// h0026
	unsigned int /* padding 16 bit */:16;

	// h0027, bit: 14
	/* Hist = (sum_ir * reg_roi_div) >> (16+4)*/
	unsigned int reg_roi_div:16;

	// h0027
	unsigned int /* padding 16 bit */:16;

	// h0028, bit: 14
	/* MIU Base address (total 256 bytes)*/
	unsigned int reg_miu_base_addr_0:16;

	// h0028
	unsigned int /* padding 16 bit */:16;

	// h0029, bit: 14
	/* MIU Base address (total 256 bytes)*/
	unsigned int reg_miu_base_addr_1:16;

	// h0029
	unsigned int /* padding 16 bit */:16;

	// h002a, bit: 14
	/* MIU AWB Max address*/
	unsigned int reg_rgbir_awb_max_addr_0:16;

	// h002a
	unsigned int /* padding 16 bit */:16;

	// h002b, bit: 10
	/* MIU AWB Max address*/
	unsigned int reg_rgbir_awb_max_addr_1:11;

	// h002b, bit: 14
	/* */
	unsigned int :5;

	// h002b
	unsigned int /* padding 16 bit */:16;

	// h002c, bit: 1
	/* MIU AWB Lenx4*/
	unsigned int reg_rgbir_awb_mi_lenx4:2;

	// h002c, bit: 3
	/* */
	unsigned int :2;

	// h002c, bit: 9
	/* MIU AWB Mask CycX16*/
	unsigned int reg_rgbir_awb_mi_mask_cycx16:6;

	// h002c, bit: 14
	/* */
	unsigned int :6;

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
	/* */
	unsigned int :16;

	// h007f
	unsigned int /* padding 16 bit */:16;

}  __attribute__((packed, aligned(1))) bank_8;
#endif
