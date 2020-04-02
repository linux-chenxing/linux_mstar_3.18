// Generate Time: 2017-09-19 22:58:05.551299
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
#ifndef __INFINITY2_REG_BLOCK_ISPSC__
#define __INFINITY2_REG_BLOCK_ISPSC__
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
	/* 1. Each bits means  each IP's SRAM group power ON/OFF signal.
	2. 1'b0: Power-ON
	[0] : bach sram power ctrl.: 0:power-on, 1:power-off
	[1] : aesdma sram power ctrl.: 0:power-on, 1:power-off
	[2] : cmdq sram power ctrl.: 0:power-on, 1:power-off
	[3] : emac sram power ctrl.: 0:power-on, 1:power-off
	[4] : fcie sram power ctrl.: 0:power-on, 1:power-off
	[5] : gop sram power ctrl.: 0:power-on, 1:power-off
	[6] : imi sram power ctrl.: 0:power-on, 1:power-off
	[7] : isp sram power ctrl.: 0:power-on, 1:power-off
	[8] : jpe sram power ctrl.: 0:power-on, 1:power-off
	[9] : mfe sram power ctrl.: 0:power-on, 1:power-off
	[10] : sdio sram power ctrl.: 0:power-on, 1:power-off
	[11] : usb sram power ctrl.: 0:power-on, 1:power-off
	[12] : hvsp1 sram power ctrl.: 0:power-on, 1:power-off
	[13] : hvsp2 sram power ctrl.: 0:power-on, 1:power-off
	[14] : hvsp3 sram power ctrl.: 0:power-on, 1:power-off
	[15] : vip sram power ctrl.: 0:power-on, 1:power-off
	[16] : nlm sram power ctrl.: 0:power-on, 1:power-off
	[17] : autoload sram power ctrl.: 0:power-on, 1:power-off
	[18] : dnr sram power ctrl.: 0:power-on, 1:power-off
	[19] : ldc sram power ctrl.: 0:power-on, 1:power-off
	[20] : sc1_ss sram power ctrl.: 0:power-on, 1:power-off
	[21] : sc1_frm sram power ctrl.: 0:power-on, 1:power-off
	[22] : debug sram power ctrl.: 0:power-on, 1:power-off
	[23] : sc1_jpe sram power ctrl.: 0:power-on, 1:power-off
	[24] : sc2_imi sram power ctrl.: 0:power-on, 1:power-off
	[25] : sc2_frm sram power ctrl.: 0:power-on, 1:power-off
	[26] : rsv1 sram power ctrl.: 0:power-on, 1:power-off
	[27] : rsv2 sram power ctrl.: 0:power-on, 1:power-off
	[28] : isp_dnr sram power ctrl.: 0:power-on, 1:power-off
	[29] : isp_rot sram power ctrl.: 0:power-on, 1:power-off
	[30] : reserved
	[31] : usb_p1 sram power ctrl.: 0:power-on, 1:power-off
	[32] : ive sram power ctrl.: 0:power-on, 1:power-off
	[33] : cmdq1 sram power ctrl.: 0:power-on, 1:power-off
	[34] : cmdq2 sram power ctrl.: 0:power-on, 1:power-off
	[35] : gop1 sram power ctrl.: 0:power-on, 1:power-off
	[36] : gop2 sram power ctrl.: 0:power-on, 1:power-off
	[37] : xnr sram power ctrl.: 0:power-on, 1:power-off
	[38] : mxnr sram power ctrl.: 0:power-on, 1:power-off
	[39] : prvc sram power ctrl.: 0:power-on, 1:power-off
	[40] : wdr sram power ctrl.: 0:power-on, 1:power-off
	[41] : gamma sram power ctrl.: 0:power-on, 1:power-off
	[63:42] : reserved*/
	#define offset_of_ispsc_reg_sram_sd_en (32)
	#define mask_of_ispsc_reg_sram_sd_en (0xffff)
	unsigned int reg_sram_sd_en:16;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 14
	/* 1. Each bits means  each IP's SRAM group power ON/OFF signal.
	2. 1'b0: Power-ON
	[0] : bach sram power ctrl.: 0:power-on, 1:power-off
	[1] : aesdma sram power ctrl.: 0:power-on, 1:power-off
	[2] : cmdq sram power ctrl.: 0:power-on, 1:power-off
	[3] : emac sram power ctrl.: 0:power-on, 1:power-off
	[4] : fcie sram power ctrl.: 0:power-on, 1:power-off
	[5] : gop sram power ctrl.: 0:power-on, 1:power-off
	[6] : imi sram power ctrl.: 0:power-on, 1:power-off
	[7] : isp sram power ctrl.: 0:power-on, 1:power-off
	[8] : jpe sram power ctrl.: 0:power-on, 1:power-off
	[9] : mfe sram power ctrl.: 0:power-on, 1:power-off
	[10] : sdio sram power ctrl.: 0:power-on, 1:power-off
	[11] : usb sram power ctrl.: 0:power-on, 1:power-off
	[12] : hvsp1 sram power ctrl.: 0:power-on, 1:power-off
	[13] : hvsp2 sram power ctrl.: 0:power-on, 1:power-off
	[14] : hvsp3 sram power ctrl.: 0:power-on, 1:power-off
	[15] : vip sram power ctrl.: 0:power-on, 1:power-off
	[16] : nlm sram power ctrl.: 0:power-on, 1:power-off
	[17] : autoload sram power ctrl.: 0:power-on, 1:power-off
	[18] : dnr sram power ctrl.: 0:power-on, 1:power-off
	[19] : ldc sram power ctrl.: 0:power-on, 1:power-off
	[20] : sc1_ss sram power ctrl.: 0:power-on, 1:power-off
	[21] : sc1_frm sram power ctrl.: 0:power-on, 1:power-off
	[22] : debug sram power ctrl.: 0:power-on, 1:power-off
	[23] : sc1_jpe sram power ctrl.: 0:power-on, 1:power-off
	[24] : sc2_imi sram power ctrl.: 0:power-on, 1:power-off
	[25] : sc2_frm sram power ctrl.: 0:power-on, 1:power-off
	[26] : rsv1 sram power ctrl.: 0:power-on, 1:power-off
	[27] : rsv2 sram power ctrl.: 0:power-on, 1:power-off
	[28] : isp_dnr sram power ctrl.: 0:power-on, 1:power-off
	[29] : isp_rot sram power ctrl.: 0:power-on, 1:power-off
	[30] : reserved
	[31] : usb_p1 sram power ctrl.: 0:power-on, 1:power-off
	[32] : ive sram power ctrl.: 0:power-on, 1:power-off
	[33] : cmdq1 sram power ctrl.: 0:power-on, 1:power-off
	[34] : cmdq2 sram power ctrl.: 0:power-on, 1:power-off
	[35] : gop1 sram power ctrl.: 0:power-on, 1:power-off
	[36] : gop2 sram power ctrl.: 0:power-on, 1:power-off
	[37] : xnr sram power ctrl.: 0:power-on, 1:power-off
	[38] : mxnr sram power ctrl.: 0:power-on, 1:power-off
	[39] : prvc sram power ctrl.: 0:power-on, 1:power-off
	[40] : wdr sram power ctrl.: 0:power-on, 1:power-off
	[41] : gamma sram power ctrl.: 0:power-on, 1:power-off
	[63:42] : reserved*/
	#define offset_of_ispsc_reg_sram_sd_en_1 (34)
	#define mask_of_ispsc_reg_sram_sd_en_1 (0xffff)
	unsigned int reg_sram_sd_en_1:16;

	// h0011
	unsigned int /* padding 16 bit */:16;

	// h0012, bit: 14
	/* 1. Each bits means  each IP's SRAM group power ON/OFF signal.
	2. 1'b0: Power-ON
	[0] : bach sram power ctrl.: 0:power-on, 1:power-off
	[1] : aesdma sram power ctrl.: 0:power-on, 1:power-off
	[2] : cmdq sram power ctrl.: 0:power-on, 1:power-off
	[3] : emac sram power ctrl.: 0:power-on, 1:power-off
	[4] : fcie sram power ctrl.: 0:power-on, 1:power-off
	[5] : gop sram power ctrl.: 0:power-on, 1:power-off
	[6] : imi sram power ctrl.: 0:power-on, 1:power-off
	[7] : isp sram power ctrl.: 0:power-on, 1:power-off
	[8] : jpe sram power ctrl.: 0:power-on, 1:power-off
	[9] : mfe sram power ctrl.: 0:power-on, 1:power-off
	[10] : sdio sram power ctrl.: 0:power-on, 1:power-off
	[11] : usb sram power ctrl.: 0:power-on, 1:power-off
	[12] : hvsp1 sram power ctrl.: 0:power-on, 1:power-off
	[13] : hvsp2 sram power ctrl.: 0:power-on, 1:power-off
	[14] : hvsp3 sram power ctrl.: 0:power-on, 1:power-off
	[15] : vip sram power ctrl.: 0:power-on, 1:power-off
	[16] : nlm sram power ctrl.: 0:power-on, 1:power-off
	[17] : autoload sram power ctrl.: 0:power-on, 1:power-off
	[18] : dnr sram power ctrl.: 0:power-on, 1:power-off
	[19] : ldc sram power ctrl.: 0:power-on, 1:power-off
	[20] : sc1_ss sram power ctrl.: 0:power-on, 1:power-off
	[21] : sc1_frm sram power ctrl.: 0:power-on, 1:power-off
	[22] : debug sram power ctrl.: 0:power-on, 1:power-off
	[23] : sc1_jpe sram power ctrl.: 0:power-on, 1:power-off
	[24] : sc2_imi sram power ctrl.: 0:power-on, 1:power-off
	[25] : sc2_frm sram power ctrl.: 0:power-on, 1:power-off
	[26] : rsv1 sram power ctrl.: 0:power-on, 1:power-off
	[27] : rsv2 sram power ctrl.: 0:power-on, 1:power-off
	[28] : isp_dnr sram power ctrl.: 0:power-on, 1:power-off
	[29] : isp_rot sram power ctrl.: 0:power-on, 1:power-off
	[30] : reserved
	[31] : usb_p1 sram power ctrl.: 0:power-on, 1:power-off
	[32] : ive sram power ctrl.: 0:power-on, 1:power-off
	[33] : cmdq1 sram power ctrl.: 0:power-on, 1:power-off
	[34] : cmdq2 sram power ctrl.: 0:power-on, 1:power-off
	[35] : gop1 sram power ctrl.: 0:power-on, 1:power-off
	[36] : gop2 sram power ctrl.: 0:power-on, 1:power-off
	[37] : xnr sram power ctrl.: 0:power-on, 1:power-off
	[38] : mxnr sram power ctrl.: 0:power-on, 1:power-off
	[39] : prvc sram power ctrl.: 0:power-on, 1:power-off
	[40] : wdr sram power ctrl.: 0:power-on, 1:power-off
	[41] : gamma sram power ctrl.: 0:power-on, 1:power-off
	[63:42] : reserved*/
	#define offset_of_ispsc_reg_sram_sd_en_2 (36)
	#define mask_of_ispsc_reg_sram_sd_en_2 (0xffff)
	unsigned int reg_sram_sd_en_2:16;

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

	// h0020, bit: 7
	/* */
	unsigned int :8;

	// h0020, bit: 10
	/* [2]: nlm software select signal, 0:fclk1/1:miu_clk
	[1]: invert clock
	[0]: nlm software gate signal; 1: gated*/
	#define offset_of_ispsc_reg_ckg_nlm (64)
	#define mask_of_ispsc_reg_ckg_nlm (0x700)
	unsigned int reg_ckg_nlm:3;

	// h0020, bit: 11
	/* */
	unsigned int :1;

	// h0020, bit: 12
	/* hardware signal nlm_clk_sel read back  (0:fclk1/1:miu_clk)*/
	#define offset_of_ispsc_reg_nlm_clk_sel_rd (64)
	#define mask_of_ispsc_reg_nlm_clk_sel_rd (0x1000)
	unsigned int reg_nlm_clk_sel_rd:1;

	// h0020, bit: 13
	/* hardware signal nlm_clk_gate read back (1: gated)*/
	#define offset_of_ispsc_reg_nlm_clk_gate_rd (64)
	#define mask_of_ispsc_reg_nlm_clk_gate_rd (0x2000)
	unsigned int reg_nlm_clk_gate_rd:1;

	// h0020, bit: 14
	/* */
	unsigned int :2;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 3
	/* clk_gop0_psram setting
	[3:2]: select, 00: gop clock, 01: miu clock
	[1]: invert clock
	[0]: disable clock; 1: gated*/
	#define offset_of_ispsc_reg_ckg_gop0_psram (66)
	#define mask_of_ispsc_reg_ckg_gop0_psram (0xf)
	unsigned int reg_ckg_gop0_psram:4;

	// h0021, bit: 7
	/* clk_gop1_psram setting
	[3:2]: select, 00: gop clock, 01: miu clock
	[1]: invert clock
	[0]: disable clock; 1: gated*/
	#define offset_of_ispsc_reg_ckg_gop1_psram (66)
	#define mask_of_ispsc_reg_ckg_gop1_psram (0xf0)
	unsigned int reg_ckg_gop1_psram:4;

	// h0021, bit: 11
	/* clk_gop2_psram setting
	[3:2]: select, 00: gop clock, 01: miu clock
	[1]: invert clock
	[0]: disable clock; 1: gated*/
	#define offset_of_ispsc_reg_ckg_gop2_psram (66)
	#define mask_of_ispsc_reg_ckg_gop2_psram (0xf00)
	unsigned int reg_ckg_gop2_psram:4;

	// h0021, bit: 14
	/* clk_gop3_psram setting
	[3:2]: select, 00: gop clock, 01: miu clock
	[1]: invert clock
	[0]: disable clock; 1: gated*/
	#define offset_of_ispsc_reg_ckg_gop3_psram (66)
	#define mask_of_ispsc_reg_ckg_gop3_psram (0xf000)
	unsigned int reg_ckg_gop3_psram:4;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 3
	/* clk_gop4_psram setting
	[3:2]: select, 00: gop clock, 01: miu clock
	[1]: invert clock
	[0]: disable clock; 1: gated*/
	#define offset_of_ispsc_reg_ckg_gop4_psram (68)
	#define mask_of_ispsc_reg_ckg_gop4_psram (0xf)
	unsigned int reg_ckg_gop4_psram:4;

	// h0022, bit: 14
	/* */
	unsigned int :12;

	// h0022
	unsigned int /* padding 16 bit */:16;

	// h0023, bit: 14
	/* */
	unsigned int :16;

	// h0023
	unsigned int /* padding 16 bit */:16;

	// h0024, bit: 3
	/* clk_idclk clock setting
	[0]: disable clock
	[1]: invert clock
	[3:2]: Select clock source
	00: clk_isp_p
	01: ccir clock*/
	#define offset_of_ispsc_ reg_ckg_idclk (72)
	#define mask_of_ispsc_ reg_ckg_idclk (0xf)
	unsigned int  reg_ckg_idclk:4;

	// h0024, bit: 14
	/* */
	unsigned int :12;

	// h0024
	unsigned int /* padding 16 bit */:16;

	// h0025, bit: 3
	/* clk_isp clock setting
	[0]: disable clock
	[1]: invert clock
	[3:2]: Select clock source
	00: isppll_600m
	01: isppll_300m
	others reserv*/
	#define offset_of_ispsc_reg_ckg_isp (74)
	#define mask_of_ispsc_reg_ckg_isp (0xf)
	unsigned int reg_ckg_isp:4;

	// h0025, bit: 14
	/* */
	unsigned int :12;

	// h0025
	unsigned int /* padding 16 bit */:16;

	// h0026, bit: 4
	/* clk_isp_mload clock setting
	[0]: disable clock
	[1]: invert clock
	[3:2]: Select clock source
	00: clk_isp
	01: clk_isp_img
	others reserve
	[4]: glitch free switch to 12MHz*/
	#define offset_of_ispsc_reg_ckg_isp_mload (76)
	#define mask_of_ispsc_reg_ckg_isp_mload (0x1f)
	unsigned int reg_ckg_isp_mload:5;

	// h0026, bit: 14
	/* */
	unsigned int :11;

	// h0026
	unsigned int /* padding 16 bit */:16;

	// h0027, bit: 14
	/* */
	unsigned int :16;

	// h0027
	unsigned int /* padding 16 bit */:16;

	// h0028, bit: 14
	/* */
	unsigned int :16;

	// h0028
	unsigned int /* padding 16 bit */:16;

	// h0029, bit: 14
	/* */
	unsigned int :16;

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
	/* spare register, default low*/
	#define offset_of_ispsc_reg_sc_spare_lo (96)
	#define mask_of_ispsc_reg_sc_spare_lo (0xffff)
	unsigned int reg_sc_spare_lo:16;

	// h0030
	unsigned int /* padding 16 bit */:16;

	// h0031, bit: 14
	/* spare register, default high*/
	#define offset_of_ispsc_reg_sc_spare_hi (98)
	#define mask_of_ispsc_reg_sc_spare_hi (0xffff)
	unsigned int reg_sc_spare_hi:16;

	// h0031
	unsigned int /* padding 16 bit */:16;

	// h0032, bit: 3
	/* select group IP testbus*/
	#define offset_of_ispsc_reg_isp_sc_test_in_sel (100)
	#define mask_of_ispsc_reg_isp_sc_test_in_sel (0xf)
	unsigned int reg_isp_sc_test_in_sel:4;

	// h0032, bit: 14
	/* */
	unsigned int :12;

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

	// h0040, bit: 6
	/* clk_bt656_0 clock setting (sensor pixel clock)
	[0]: disable clock
	[3:2]: select clock source*/
	#define offset_of_ispsc_reg_ckg_bt656_0 (128)
	#define mask_of_ispsc_reg_ckg_bt656_0 (0x7f)
	unsigned int reg_ckg_bt656_0:7;

	// h0040, bit: 7
	/* */
	unsigned int :1;

	// h0040, bit: 14
	/* clk_bt656_1 clock setting (sensor pixel clock)
	[0]: disable clock
	[3:2]: select clock source*/
	#define offset_of_ispsc_reg_ckg_bt656_1 (128)
	#define mask_of_ispsc_reg_ckg_bt656_1 (0x7f00)
	unsigned int reg_ckg_bt656_1:7;

	// h0040, bit: 15
	/* */
	unsigned int :1;

	// h0040
	unsigned int /* padding 16 bit */:16;

	// h0041, bit: 6
	/* clk_bt656_2 clock setting (sensor pixel clock)
	[0]: disable clock
	[3:2]: select clock source*/
	#define offset_of_ispsc_reg_ckg_bt656_2 (130)
	#define mask_of_ispsc_reg_ckg_bt656_2 (0x7f)
	unsigned int reg_ckg_bt656_2:7;

	// h0041, bit: 7
	/* */
	unsigned int :1;

	// h0041, bit: 14
	/* clk_bt656_3 clock setting (sensor pixel clock)
	[0]: disable clock
	[3:2]: select clock source*/
	#define offset_of_ispsc_reg_ckg_bt656_3 (130)
	#define mask_of_ispsc_reg_ckg_bt656_3 (0x7f00)
	unsigned int reg_ckg_bt656_3:7;

	// h0041, bit: 15
	/* */
	unsigned int :1;

	// h0041
	unsigned int /* padding 16 bit */:16;

	// h0042, bit: 4
	/* clk_snr0 clock setting (sensor pixel clock)
	[0]: disable clock
	[1]: invert clock
	[4:2]: select clock source
	  000: clk_csi2_mac_toblk0_buf0_p
	  001: clk_csi2_mac_toblk0_4_buf0_p
	  010: clk_bt656_p0_0_p
	  011: clk_bt656_p1_0_p
	  100: clk_bt656_p0_1_p
	  101: clk_bt656_p1_1_p*/
	#define offset_of_ispsc_reg_ckg_snr0 (132)
	#define mask_of_ispsc_reg_ckg_snr0 (0x1f)
	unsigned int reg_ckg_snr0:5;

	// h0042, bit: 7
	/* */
	unsigned int :3;

	// h0042, bit: 12
	/* clk_snr1 clock setting (sensor pixel clock)
	[0]: disable clock
	[1]: invert clock
	[4:2]: select clock source
	  000: clk_csi2_mac_toblk0_buf0_p
	  001: clk_csi2_mac_toblk0_4_buf0_p
	  010: clk_bt656_p0_0_p
	  011: clk_bt656_p1_0_p
	  100: clk_bt656_p0_1_p
	  101: clk_bt656_p1_1_p*/
	#define offset_of_ispsc_reg_ckg_snr1 (132)
	#define mask_of_ispsc_reg_ckg_snr1 (0x1f00)
	unsigned int reg_ckg_snr1:5;

	// h0042, bit: 14
	/* */
	unsigned int :3;

	// h0042
	unsigned int /* padding 16 bit */:16;

	// h0043, bit: 4
	/* clk_snr2 clock setting (sensor pixel clock)
	[0]: disable clock
	[1]: invert clock
	[4:2]: select clock source
	  000: clk_csi2_mac_toblk0_buf0_p
	  001: clk_csi2_mac_toblk0_4_buf0_p
	  010: clk_bt656_p0_0_p
	  011: clk_bt656_p1_0_p
	  100: clk_bt656_p0_1_p
	  101: clk_bt656_p1_1_p*/
	#define offset_of_ispsc_reg_ckg_snr2 (134)
	#define mask_of_ispsc_reg_ckg_snr2 (0x1f)
	unsigned int reg_ckg_snr2:5;

	// h0043, bit: 7
	/* */
	unsigned int :3;

	// h0043, bit: 12
	/* clk_snr3 clock setting (sensor pixel clock)
	[0]: disable clock
	[1]: invert clock
	[4:2]: select clock source
	  000: clk_csi2_mac_toblk0_buf0_p
	  001: clk_csi2_mac_toblk0_4_buf0_p
	  010: clk_bt656_p0_0_p
	  011: clk_bt656_p1_0_p
	  100: clk_bt656_p0_1_p
	  101: clk_bt656_p1_1_p*/
	#define offset_of_ispsc_reg_ckg_snr3 (134)
	#define mask_of_ispsc_reg_ckg_snr3 (0x1f00)
	unsigned int reg_ckg_snr3:5;

	// h0043, bit: 14
	/* */
	unsigned int :3;

	// h0043
	unsigned int /* padding 16 bit */:16;

	// h0044, bit: 4
	/* clk_snr2 clock setting (sensor pixel clock)
	[0]: disable clock
	[1]: invert clock
	[4:2]: select clock source
	  000: clk_csi2_mac_toblk0_buf0_p
	  001: clk_csi2_mac_toblk0_4_buf0_p
	  010: clk_bt656_p0_0_p
	  011: clk_bt656_p1_0_p
	  100: clk_bt656_p0_1_p
	  101: clk_bt656_p1_1_p*/
	#define offset_of_ispsc_reg_ckg_snr4 (136)
	#define mask_of_ispsc_reg_ckg_snr4 (0x1f)
	unsigned int reg_ckg_snr4:5;

	// h0044, bit: 7
	/* */
	unsigned int :3;

	// h0044, bit: 12
	/* clk_snr3 clock setting (sensor pixel clock)
	[0]: disable clock
	[1]: invert clock
	[4:2]: select clock source
	  000: clk_csi2_mac_toblk0_buf0_p
	  001: clk_csi2_mac_toblk0_4_buf0_p
	  010: clk_bt656_p0_0_p
	  011: clk_bt656_p1_0_p
	  100: clk_bt656_p0_1_p
	  101: clk_bt656_p1_1_p*/
	#define offset_of_ispsc_reg_ckg_snr5 (136)
	#define mask_of_ispsc_reg_ckg_snr5 (0x1f00)
	unsigned int reg_ckg_snr5:5;

	// h0044, bit: 14
	/* */
	unsigned int :3;

	// h0044
	unsigned int /* padding 16 bit */:16;

	// h0045, bit: 4
	/* clk_snr2 clock setting (sensor pixel clock)
	[0]: disable clock
	[1]: invert clock
	[4:2]: select clock source
	  000: clk_csi2_mac_toblk0_buf0_p
	  001: clk_csi2_mac_toblk0_4_buf0_p
	  010: clk_bt656_p0_0_p
	  011: clk_bt656_p1_0_p
	  100: clk_bt656_p0_1_p
	  101: clk_bt656_p1_1_p*/
	#define offset_of_ispsc_reg_ckg_snr6 (138)
	#define mask_of_ispsc_reg_ckg_snr6 (0x1f)
	unsigned int reg_ckg_snr6:5;

	// h0045, bit: 7
	/* */
	unsigned int :3;

	// h0045, bit: 12
	/* clk_snr3 clock setting (sensor pixel clock)
	[0]: disable clock
	[1]: invert clock
	[4:2]: select clock source
	  000: clk_csi2_mac_toblk0_buf0_p
	  001: clk_csi2_mac_toblk0_4_buf0_p
	  010: clk_bt656_p0_0_p
	  011: clk_bt656_p1_0_p
	  100: clk_bt656_p0_1_p
	  101: clk_bt656_p1_1_p*/
	#define offset_of_ispsc_reg_ckg_snr7 (138)
	#define mask_of_ispsc_reg_ckg_snr7 (0x1f00)
	unsigned int reg_ckg_snr7:5;

	// h0045, bit: 14
	/* */
	unsigned int :3;

	// h0045
	unsigned int /* padding 16 bit */:16;

	// h0046, bit: 4
	/* clk_snr2 clock setting (sensor pixel clock)
	[0]: disable clock
	[1]: invert clock
	[4:2]: select clock source
	  000: clk_csi2_mac_toblk0_buf0_p
	  001: clk_csi2_mac_toblk0_4_buf0_p
	  010: clk_bt656_p0_0_p
	  011: clk_bt656_p1_0_p
	  100: clk_bt656_p0_1_p
	  101: clk_bt656_p1_1_p*/
	#define offset_of_ispsc_reg_ckg_snr8 (140)
	#define mask_of_ispsc_reg_ckg_snr8 (0x1f)
	unsigned int reg_ckg_snr8:5;

	// h0046, bit: 7
	/* */
	unsigned int :3;

	// h0046, bit: 12
	/* clk_snr3 clock setting (sensor pixel clock)
	[0]: disable clock
	[1]: invert clock
	[4:2]: select clock source
	  000: clk_csi2_mac_toblk0_buf0_p
	  001: clk_csi2_mac_toblk0_4_buf0_p
	  010: clk_bt656_p0_0_p
	  011: clk_bt656_p1_0_p
	  100: clk_bt656_p0_1_p
	  101: clk_bt656_p1_1_p*/
	#define offset_of_ispsc_reg_ckg_snr9 (140)
	#define mask_of_ispsc_reg_ckg_snr9 (0x1f00)
	unsigned int reg_ckg_snr9:5;

	// h0046, bit: 14
	/* */
	unsigned int :3;

	// h0046
	unsigned int /* padding 16 bit */:16;

	// h0047, bit: 4
	/* clk_snr2 clock setting (sensor pixel clock)
	[0]: disable clock
	[1]: invert clock
	[4:2]: select clock source
	  000: clk_csi2_mac_toblk0_buf0_p
	  001: clk_csi2_mac_toblk0_4_buf0_p
	  010: clk_bt656_p0_0_p
	  011: clk_bt656_p1_0_p
	  100: clk_bt656_p0_1_p
	  101: clk_bt656_p1_1_p*/
	#define offset_of_ispsc_reg_ckg_snr10 (142)
	#define mask_of_ispsc_reg_ckg_snr10 (0x1f)
	unsigned int reg_ckg_snr10:5;

	// h0047, bit: 7
	/* */
	unsigned int :3;

	// h0047, bit: 12
	/* clk_snr3 clock setting (sensor pixel clock)
	[0]: disable clock
	[1]: invert clock
	[4:2]: select clock source
	  000: clk_csi2_mac_toblk0_buf0_p
	  001: clk_csi2_mac_toblk0_4_buf0_p
	  010: clk_bt656_p0_0_p
	  011: clk_bt656_p1_0_p
	  100: clk_bt656_p0_1_p
	  101: clk_bt656_p1_1_p*/
	#define offset_of_ispsc_reg_ckg_snr11 (142)
	#define mask_of_ispsc_reg_ckg_snr11 (0x1f00)
	unsigned int reg_ckg_snr11:5;

	// h0047, bit: 14
	/* */
	unsigned int :3;

	// h0047
	unsigned int /* padding 16 bit */:16;

	// h0048, bit: 4
	/* clk_snr2 clock setting (sensor pixel clock)
	[0]: disable clock
	[1]: invert clock
	[4:2]: select clock source
	  000: clk_csi2_mac_toblk0_buf0_p
	  001: clk_csi2_mac_toblk0_4_buf0_p
	  010: clk_bt656_p0_0_p
	  011: clk_bt656_p1_0_p
	  100: clk_bt656_p0_1_p
	  101: clk_bt656_p1_1_p*/
	#define offset_of_ispsc_reg_ckg_snr12 (144)
	#define mask_of_ispsc_reg_ckg_snr12 (0x1f)
	unsigned int reg_ckg_snr12:5;

	// h0048, bit: 7
	/* */
	unsigned int :3;

	// h0048, bit: 12
	/* clk_snr3 clock setting (sensor pixel clock)
	[0]: disable clock
	[1]: invert clock
	[4:2]: select clock source
	  000: clk_csi2_mac_toblk0_buf0_p
	  001: clk_csi2_mac_toblk0_4_buf0_p
	  010: clk_bt656_p0_0_p
	  011: clk_bt656_p1_0_p
	  100: clk_bt656_p0_1_p
	  101: clk_bt656_p1_1_p*/
	#define offset_of_ispsc_reg_ckg_snr13 (144)
	#define mask_of_ispsc_reg_ckg_snr13 (0x1f00)
	unsigned int reg_ckg_snr13:5;

	// h0048, bit: 14
	/* */
	unsigned int :3;

	// h0048
	unsigned int /* padding 16 bit */:16;

	// h0049, bit: 4
	/* clk_snr2 clock setting (sensor pixel clock)
	[0]: disable clock
	[1]: invert clock
	[4:2]: select clock source
	  000: clk_csi2_mac_toblk0_buf0_p
	  001: clk_csi2_mac_toblk0_4_buf0_p
	  010: clk_bt656_p0_0_p
	  011: clk_bt656_p1_0_p
	  100: clk_bt656_p0_1_p
	  101: clk_bt656_p1_1_p*/
	#define offset_of_ispsc_reg_ckg_snr14 (146)
	#define mask_of_ispsc_reg_ckg_snr14 (0x1f)
	unsigned int reg_ckg_snr14:5;

	// h0049, bit: 7
	/* */
	unsigned int :3;

	// h0049, bit: 12
	/* clk_snr3 clock setting (sensor pixel clock)
	[0]: disable clock
	[1]: invert clock
	[4:2]: select clock source
	  000: clk_csi2_mac_toblk0_buf0_p
	  001: clk_csi2_mac_toblk0_4_buf0_p
	  010: clk_bt656_p0_0_p
	  011: clk_bt656_p1_0_p
	  100: clk_bt656_p0_1_p
	  101: clk_bt656_p1_1_p*/
	#define offset_of_ispsc_reg_ckg_snr15 (146)
	#define mask_of_ispsc_reg_ckg_snr15 (0x1f00)
	unsigned int reg_ckg_snr15:5;

	// h0049, bit: 14
	/* */
	unsigned int :3;

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

}  __attribute__((packed, aligned(1))) infinity2_reg_block_ispsc;
#endif
