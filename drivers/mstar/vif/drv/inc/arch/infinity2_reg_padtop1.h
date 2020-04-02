// Generate Time: 2017-09-19 22:58:05.619169
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
#ifndef __INFINITY2_REG_PADTOP1__
#define __INFINITY2_REG_PADTOP1__
typedef struct {
	// h0000, bit: 0
	/* reg_all_pad_in*/
	#define offset_of_padtop1_reg_all_pad_in (0)
	#define mask_of_padtop1_reg_all_pad_in (0x1)
	unsigned int reg_all_pad_in:1;

	// h0000, bit: 14
	/* */
	unsigned int :15;

	// h0000
	unsigned int /* padding 16 bit */:16;

	// h0001, bit: 2
	/* reg_ForthUARTMode*/
	#define offset_of_padtop1_reg_forthuartmode (2)
	#define mask_of_padtop1_reg_forthuartmode (0x7)
	unsigned int reg_forthuartmode:3;

	// h0001, bit: 5
	/* reg_SecondUARTMode*/
	#define offset_of_padtop1_reg_seconduartmode (2)
	#define mask_of_padtop1_reg_seconduartmode (0x38)
	unsigned int reg_seconduartmode:3;

	// h0001, bit: 8
	/* reg_ThirdUARTMode*/
	#define offset_of_padtop1_reg_thirduartmode (2)
	#define mask_of_padtop1_reg_thirduartmode (0x1c0)
	unsigned int reg_thirduartmode:3;

	// h0001, bit: 14
	/* */
	unsigned int :7;

	// h0001
	unsigned int /* padding 16 bit */:16;

	// h0002, bit: 0
	/* reg_bt_i2s_rx_0_mode*/
	#define offset_of_padtop1_reg_bt_i2s_rx_0_mode (4)
	#define mask_of_padtop1_reg_bt_i2s_rx_0_mode (0x1)
	unsigned int reg_bt_i2s_rx_0_mode:1;

	// h0002, bit: 1
	/* reg_bt_i2s_trx_0_mode*/
	#define offset_of_padtop1_reg_bt_i2s_trx_0_mode (4)
	#define mask_of_padtop1_reg_bt_i2s_trx_0_mode (0x2)
	unsigned int reg_bt_i2s_trx_0_mode:1;

	// h0002, bit: 2
	/* reg_bt_i2s_tx_0_mode*/
	#define offset_of_padtop1_reg_bt_i2s_tx_0_mode (4)
	#define mask_of_padtop1_reg_bt_i2s_tx_0_mode (0x4)
	unsigned int reg_bt_i2s_tx_0_mode:1;

	// h0002, bit: 3
	/* reg_ccir0_16b_mode*/
	#define offset_of_padtop1_reg_ccir0_16b_mode (4)
	#define mask_of_padtop1_reg_ccir0_16b_mode (0x8)
	unsigned int reg_ccir0_16b_mode:1;

	// h0002, bit: 4
	/* reg_ccir0_8b_mode*/
	#define offset_of_padtop1_reg_ccir0_8b_mode (4)
	#define mask_of_padtop1_reg_ccir0_8b_mode (0x10)
	unsigned int reg_ccir0_8b_mode:1;

	// h0002, bit: 7
	/* reg_ccir0_clk_mode*/
	#define offset_of_padtop1_reg_ccir0_clk_mode (4)
	#define mask_of_padtop1_reg_ccir0_clk_mode (0xe0)
	unsigned int reg_ccir0_clk_mode:3;

	// h0002, bit: 9
	/* reg_ccir0_ctrl_mode*/
	#define offset_of_padtop1_reg_ccir0_ctrl_mode (4)
	#define mask_of_padtop1_reg_ccir0_ctrl_mode (0x300)
	unsigned int reg_ccir0_ctrl_mode:2;

	// h0002, bit: 10
	/* reg_ccir1_8b_mode*/
	#define offset_of_padtop1_reg_ccir1_8b_mode (4)
	#define mask_of_padtop1_reg_ccir1_8b_mode (0x400)
	unsigned int reg_ccir1_8b_mode:1;

	// h0002, bit: 12
	/* reg_ccir1_clk_mode*/
	#define offset_of_padtop1_reg_ccir1_clk_mode (4)
	#define mask_of_padtop1_reg_ccir1_clk_mode (0x1800)
	unsigned int reg_ccir1_clk_mode:2;

	// h0002, bit: 13
	/* reg_ccir1_ctrl_mode*/
	#define offset_of_padtop1_reg_ccir1_ctrl_mode (4)
	#define mask_of_padtop1_reg_ccir1_ctrl_mode (0x2000)
	unsigned int reg_ccir1_ctrl_mode:1;

	// h0002, bit: 14
	/* reg_ccir2_16b_mode*/
	#define offset_of_padtop1_reg_ccir2_16b_mode (4)
	#define mask_of_padtop1_reg_ccir2_16b_mode (0x4000)
	unsigned int reg_ccir2_16b_mode:1;

	// h0002, bit: 15
	/* reg_ccir2_8b_mode*/
	#define offset_of_padtop1_reg_ccir2_8b_mode (4)
	#define mask_of_padtop1_reg_ccir2_8b_mode (0x8000)
	unsigned int reg_ccir2_8b_mode:1;

	// h0002
	unsigned int /* padding 16 bit */:16;

	// h0003, bit: 2
	/* reg_ccir2_clk_mode*/
	#define offset_of_padtop1_reg_ccir2_clk_mode (6)
	#define mask_of_padtop1_reg_ccir2_clk_mode (0x7)
	unsigned int reg_ccir2_clk_mode:3;

	// h0003, bit: 4
	/* reg_ccir2_ctrl_mode*/
	#define offset_of_padtop1_reg_ccir2_ctrl_mode (6)
	#define mask_of_padtop1_reg_ccir2_ctrl_mode (0x18)
	unsigned int reg_ccir2_ctrl_mode:2;

	// h0003, bit: 5
	/* reg_ccir3_8b_mode*/
	#define offset_of_padtop1_reg_ccir3_8b_mode (6)
	#define mask_of_padtop1_reg_ccir3_8b_mode (0x20)
	unsigned int reg_ccir3_8b_mode:1;

	// h0003, bit: 7
	/* reg_ccir3_clk_mode*/
	#define offset_of_padtop1_reg_ccir3_clk_mode (6)
	#define mask_of_padtop1_reg_ccir3_clk_mode (0xc0)
	unsigned int reg_ccir3_clk_mode:2;

	// h0003, bit: 8
	/* reg_ccir3_ctrl_mode*/
	#define offset_of_padtop1_reg_ccir3_ctrl_mode (6)
	#define mask_of_padtop1_reg_ccir3_ctrl_mode (0x100)
	unsigned int reg_ccir3_ctrl_mode:1;

	// h0003, bit: 9
	/* reg_codec_i2s_rx_0_mode*/
	#define offset_of_padtop1_reg_codec_i2s_rx_0_mode (6)
	#define mask_of_padtop1_reg_codec_i2s_rx_0_mode (0x200)
	unsigned int reg_codec_i2s_rx_0_mode:1;

	// h0003, bit: 10
	/* reg_codec_i2s_rx_1_mode*/
	#define offset_of_padtop1_reg_codec_i2s_rx_1_mode (6)
	#define mask_of_padtop1_reg_codec_i2s_rx_1_mode (0x400)
	unsigned int reg_codec_i2s_rx_1_mode:1;

	// h0003, bit: 11
	/* reg_codec_i2s_rx_2_mode*/
	#define offset_of_padtop1_reg_codec_i2s_rx_2_mode (6)
	#define mask_of_padtop1_reg_codec_i2s_rx_2_mode (0x800)
	unsigned int reg_codec_i2s_rx_2_mode:1;

	// h0003, bit: 12
	/* reg_codec_i2s_rx_3_mode*/
	#define offset_of_padtop1_reg_codec_i2s_rx_3_mode (6)
	#define mask_of_padtop1_reg_codec_i2s_rx_3_mode (0x1000)
	unsigned int reg_codec_i2s_rx_3_mode:1;

	// h0003, bit: 13
	/* reg_codec_i2s_rx_mck_mode*/
	#define offset_of_padtop1_reg_codec_i2s_rx_mck_mode (6)
	#define mask_of_padtop1_reg_codec_i2s_rx_mck_mode (0x2000)
	unsigned int reg_codec_i2s_rx_mck_mode:1;

	// h0003, bit: 14
	/* reg_codec_i2s_tx_0_mode*/
	#define offset_of_padtop1_reg_codec_i2s_tx_0_mode (6)
	#define mask_of_padtop1_reg_codec_i2s_tx_0_mode (0x4000)
	unsigned int reg_codec_i2s_tx_0_mode:1;

	// h0003, bit: 15
	/* reg_codec_i2s_tx_mute_mode*/
	#define offset_of_padtop1_reg_codec_i2s_tx_mute_mode (6)
	#define mask_of_padtop1_reg_codec_i2s_tx_mute_mode (0x8000)
	unsigned int reg_codec_i2s_tx_mute_mode:1;

	// h0003
	unsigned int /* padding 16 bit */:16;

	// h0004, bit: 1
	/* reg_dmic_0_mode*/
	#define offset_of_padtop1_reg_dmic_0_mode (8)
	#define mask_of_padtop1_reg_dmic_0_mode (0x3)
	unsigned int reg_dmic_0_mode:2;

	// h0004, bit: 3
	/* reg_dmic_1_mode*/
	#define offset_of_padtop1_reg_dmic_1_mode (8)
	#define mask_of_padtop1_reg_dmic_1_mode (0xc)
	unsigned int reg_dmic_1_mode:2;

	// h0004, bit: 5
	/* reg_dmic_2_mode*/
	#define offset_of_padtop1_reg_dmic_2_mode (8)
	#define mask_of_padtop1_reg_dmic_2_mode (0x30)
	unsigned int reg_dmic_2_mode:2;

	// h0004, bit: 7
	/* reg_dmic_3_mode*/
	#define offset_of_padtop1_reg_dmic_3_mode (8)
	#define mask_of_padtop1_reg_dmic_3_mode (0xc0)
	unsigned int reg_dmic_3_mode:2;

	// h0004, bit: 9
	/* reg_ej_ceva_mode*/
	#define offset_of_padtop1_reg_ej_ceva_mode (8)
	#define mask_of_padtop1_reg_ej_ceva_mode (0x300)
	unsigned int reg_ej_ceva_mode:2;

	// h0004, bit: 10
	/* reg_ej_mode*/
	#define offset_of_padtop1_reg_ej_mode (8)
	#define mask_of_padtop1_reg_ej_mode (0x400)
	unsigned int reg_ej_mode:1;

	// h0004, bit: 12
	/* reg_emmc_config*/
	#define offset_of_padtop1_reg_emmc_config (8)
	#define mask_of_padtop1_reg_emmc_config (0x1800)
	unsigned int reg_emmc_config:2;

	// h0004, bit: 14
	/* reg_emmc_rstn_en*/
	#define offset_of_padtop1_reg_emmc_rstn_en (8)
	#define mask_of_padtop1_reg_emmc_rstn_en (0x6000)
	unsigned int reg_emmc_rstn_en:2;

	// h0004, bit: 15
	/* */
	unsigned int :1;

	// h0004
	unsigned int /* padding 16 bit */:16;

	// h0005, bit: 1
	/* reg_fast_uart_rtx_mode*/
	#define offset_of_padtop1_reg_fast_uart_rtx_mode (10)
	#define mask_of_padtop1_reg_fast_uart_rtx_mode (0x3)
	unsigned int reg_fast_uart_rtx_mode:2;

	// h0005, bit: 3
	/* reg_fuart_emmc_mode*/
	#define offset_of_padtop1_reg_fuart_emmc_mode (10)
	#define mask_of_padtop1_reg_fuart_emmc_mode (0xc)
	unsigned int reg_fuart_emmc_mode:2;

	// h0005, bit: 5
	/* reg_fuart_mode*/
	#define offset_of_padtop1_reg_fuart_mode (10)
	#define mask_of_padtop1_reg_fuart_mode (0x30)
	unsigned int reg_fuart_mode:2;

	// h0005, bit: 6
	/* reg_gt0_mdio*/
	#define offset_of_padtop1_reg_gt0_mdio (10)
	#define mask_of_padtop1_reg_gt0_mdio (0x40)
	unsigned int reg_gt0_mdio:1;

	// h0005, bit: 8
	/* reg_gt0_mode*/
	#define offset_of_padtop1_reg_gt0_mode (10)
	#define mask_of_padtop1_reg_gt0_mode (0x180)
	unsigned int reg_gt0_mode:2;

	// h0005, bit: 10
	/* reg_gt1_mode*/
	#define offset_of_padtop1_reg_gt1_mode (10)
	#define mask_of_padtop1_reg_gt1_mode (0x600)
	unsigned int reg_gt1_mode:2;

	// h0005, bit: 11
	/* reg_hdmirx_arc_mode*/
	#define offset_of_padtop1_reg_hdmirx_arc_mode (10)
	#define mask_of_padtop1_reg_hdmirx_arc_mode (0x800)
	unsigned int reg_hdmirx_arc_mode:1;

	// h0005, bit: 12
	/* reg_hdmitx_arc_mode*/
	#define offset_of_padtop1_reg_hdmitx_arc_mode (10)
	#define mask_of_padtop1_reg_hdmitx_arc_mode (0x1000)
	unsigned int reg_hdmitx_arc_mode:1;

	// h0005, bit: 13
	/* reg_hdmitx_ddc_mode*/
	#define offset_of_padtop1_reg_hdmitx_ddc_mode (10)
	#define mask_of_padtop1_reg_hdmitx_ddc_mode (0x2000)
	unsigned int reg_hdmitx_ddc_mode:1;

	// h0005, bit: 14
	/* reg_hsync_en*/
	#define offset_of_padtop1_reg_hsync_en (10)
	#define mask_of_padtop1_reg_hsync_en (0x4000)
	unsigned int reg_hsync_en:1;

	// h0005, bit: 15
	/* */
	unsigned int :1;

	// h0005
	unsigned int /* padding 16 bit */:16;

	// h0006, bit: 1
	/* reg_i2cm0_mode*/
	#define offset_of_padtop1_reg_i2cm0_mode (12)
	#define mask_of_padtop1_reg_i2cm0_mode (0x3)
	unsigned int reg_i2cm0_mode:2;

	// h0006, bit: 4
	/* reg_i2cm1_mode*/
	#define offset_of_padtop1_reg_i2cm1_mode (12)
	#define mask_of_padtop1_reg_i2cm1_mode (0x1c)
	unsigned int reg_i2cm1_mode:3;

	// h0006, bit: 7
	/* reg_i2cm2_mode*/
	#define offset_of_padtop1_reg_i2cm2_mode (12)
	#define mask_of_padtop1_reg_i2cm2_mode (0xe0)
	unsigned int reg_i2cm2_mode:3;

	// h0006, bit: 10
	/* reg_i2cm3_mode*/
	#define offset_of_padtop1_reg_i2cm3_mode (12)
	#define mask_of_padtop1_reg_i2cm3_mode (0x700)
	unsigned int reg_i2cm3_mode:3;

	// h0006, bit: 12
	/* reg_i2cm4_mode*/
	#define offset_of_padtop1_reg_i2cm4_mode (12)
	#define mask_of_padtop1_reg_i2cm4_mode (0x1800)
	unsigned int reg_i2cm4_mode:2;

	// h0006, bit: 13
	/* reg_i2s_in_mode*/
	#define offset_of_padtop1_reg_i2s_in_mode (12)
	#define mask_of_padtop1_reg_i2s_in_mode (0x2000)
	unsigned int reg_i2s_in_mode:1;

	// h0006, bit: 14
	/* reg_i2s_out_mode*/
	#define offset_of_padtop1_reg_i2s_out_mode (12)
	#define mask_of_padtop1_reg_i2s_out_mode (0x4000)
	unsigned int reg_i2s_out_mode:1;

	// h0006, bit: 15
	/* */
	unsigned int :1;

	// h0006
	unsigned int /* padding 16 bit */:16;

	// h0007, bit: 1
	/* reg_i2s_out_mode2*/
	#define offset_of_padtop1_reg_i2s_out_mode2 (14)
	#define mask_of_padtop1_reg_i2s_out_mode2 (0x3)
	unsigned int reg_i2s_out_mode2:2;

	// h0007, bit: 2
	/* reg_i2s_out_mute_mode*/
	#define offset_of_padtop1_reg_i2s_out_mute_mode (14)
	#define mask_of_padtop1_reg_i2s_out_mute_mode (0x4)
	unsigned int reg_i2s_out_mute_mode:1;

	// h0007, bit: 3
	/* reg_i2s_trx_mode*/
	#define offset_of_padtop1_reg_i2s_trx_mode (14)
	#define mask_of_padtop1_reg_i2s_trx_mode (0x8)
	unsigned int reg_i2s_trx_mode:1;

	// h0007, bit: 4
	/* reg_mipi_lvds_tx_2ch_mode*/
	#define offset_of_padtop1_reg_mipi_lvds_tx_2ch_mode (14)
	#define mask_of_padtop1_reg_mipi_lvds_tx_2ch_mode (0x10)
	unsigned int reg_mipi_lvds_tx_2ch_mode:1;

	// h0007, bit: 5
	/* reg_mipi_lvds_tx_4ch_mode*/
	#define offset_of_padtop1_reg_mipi_lvds_tx_4ch_mode (14)
	#define mask_of_padtop1_reg_mipi_lvds_tx_4ch_mode (0x20)
	unsigned int reg_mipi_lvds_tx_4ch_mode:1;

	// h0007, bit: 6
	/* reg_misc_i2s_rx_0_mode*/
	#define offset_of_padtop1_reg_misc_i2s_rx_0_mode (14)
	#define mask_of_padtop1_reg_misc_i2s_rx_0_mode (0x40)
	unsigned int reg_misc_i2s_rx_0_mode:1;

	// h0007, bit: 7
	/* reg_misc_i2s_rx_mck_mode*/
	#define offset_of_padtop1_reg_misc_i2s_rx_mck_mode (14)
	#define mask_of_padtop1_reg_misc_i2s_rx_mck_mode (0x80)
	unsigned int reg_misc_i2s_rx_mck_mode:1;

	// h0007, bit: 8
	/* reg_misc_i2s_tx_0_mode*/
	#define offset_of_padtop1_reg_misc_i2s_tx_0_mode (14)
	#define mask_of_padtop1_reg_misc_i2s_tx_0_mode (0x100)
	unsigned int reg_misc_i2s_tx_0_mode:1;

	// h0007, bit: 9
	/* reg_misc_i2s_tx_1_mode*/
	#define offset_of_padtop1_reg_misc_i2s_tx_1_mode (14)
	#define mask_of_padtop1_reg_misc_i2s_tx_1_mode (0x200)
	unsigned int reg_misc_i2s_tx_1_mode:1;

	// h0007, bit: 10
	/* reg_misc_i2s_tx_2_mode*/
	#define offset_of_padtop1_reg_misc_i2s_tx_2_mode (14)
	#define mask_of_padtop1_reg_misc_i2s_tx_2_mode (0x400)
	unsigned int reg_misc_i2s_tx_2_mode:1;

	// h0007, bit: 14
	/* */
	unsigned int :5;

	// h0007
	unsigned int /* padding 16 bit */:16;

	// h0008, bit: 1
	/* reg_mspi1_mode1*/
	#define offset_of_padtop1_reg_mspi1_mode1 (16)
	#define mask_of_padtop1_reg_mspi1_mode1 (0x3)
	unsigned int reg_mspi1_mode1:2;

	// h0008, bit: 3
	/* reg_mspi1_mode2*/
	#define offset_of_padtop1_reg_mspi1_mode2 (16)
	#define mask_of_padtop1_reg_mspi1_mode2 (0xc)
	unsigned int reg_mspi1_mode2:2;

	// h0008, bit: 5
	/* reg_mspi1_mode3*/
	#define offset_of_padtop1_reg_mspi1_mode3 (16)
	#define mask_of_padtop1_reg_mspi1_mode3 (0x30)
	unsigned int reg_mspi1_mode3:2;

	// h0008, bit: 7
	/* reg_mspi2_mode1*/
	#define offset_of_padtop1_reg_mspi2_mode1 (16)
	#define mask_of_padtop1_reg_mspi2_mode1 (0xc0)
	unsigned int reg_mspi2_mode1:2;

	// h0008, bit: 10
	/* reg_mspi3_mode1*/
	#define offset_of_padtop1_reg_mspi3_mode1 (16)
	#define mask_of_padtop1_reg_mspi3_mode1 (0x700)
	unsigned int reg_mspi3_mode1:3;

	// h0008, bit: 11
	/* reg_nand_cs1_en*/
	#define offset_of_padtop1_reg_nand_cs1_en (16)
	#define mask_of_padtop1_reg_nand_cs1_en (0x800)
	unsigned int reg_nand_cs1_en:1;

	// h0008, bit: 13
	/* reg_nand_mode*/
	#define offset_of_padtop1_reg_nand_mode (16)
	#define mask_of_padtop1_reg_nand_mode (0x3000)
	unsigned int reg_nand_mode:2;

	// h0008, bit: 14
	/* */
	unsigned int :2;

	// h0008
	unsigned int /* padding 16 bit */:16;

	// h0009, bit: 1
	/* reg_pwm0_mode*/
	#define offset_of_padtop1_reg_pwm0_mode (18)
	#define mask_of_padtop1_reg_pwm0_mode (0x3)
	unsigned int reg_pwm0_mode:2;

	// h0009, bit: 3
	/* reg_pwm1_mode*/
	#define offset_of_padtop1_reg_pwm1_mode (18)
	#define mask_of_padtop1_reg_pwm1_mode (0xc)
	unsigned int reg_pwm1_mode:2;

	// h0009, bit: 5
	/* reg_pwm2_mode*/
	#define offset_of_padtop1_reg_pwm2_mode (18)
	#define mask_of_padtop1_reg_pwm2_mode (0x30)
	unsigned int reg_pwm2_mode:2;

	// h0009, bit: 7
	/* reg_pwm3_mode*/
	#define offset_of_padtop1_reg_pwm3_mode (18)
	#define mask_of_padtop1_reg_pwm3_mode (0xc0)
	unsigned int reg_pwm3_mode:2;

	// h0009, bit: 9
	/* reg_pwm4_mode*/
	#define offset_of_padtop1_reg_pwm4_mode (18)
	#define mask_of_padtop1_reg_pwm4_mode (0x300)
	unsigned int reg_pwm4_mode:2;

	// h0009, bit: 11
	/* reg_pwm5_mode*/
	#define offset_of_padtop1_reg_pwm5_mode (18)
	#define mask_of_padtop1_reg_pwm5_mode (0xc00)
	unsigned int reg_pwm5_mode:2;

	// h0009, bit: 13
	/* reg_pwm6_mode*/
	#define offset_of_padtop1_reg_pwm6_mode (18)
	#define mask_of_padtop1_reg_pwm6_mode (0x3000)
	unsigned int reg_pwm6_mode:2;

	// h0009, bit: 14
	/* reg_pwm7_mode*/
	#define offset_of_padtop1_reg_pwm7_mode (18)
	#define mask_of_padtop1_reg_pwm7_mode (0xc000)
	unsigned int reg_pwm7_mode:2;

	// h0009
	unsigned int /* padding 16 bit */:16;

	// h000a, bit: 14
	/* */
	unsigned int :16;

	// h000a
	unsigned int /* padding 16 bit */:16;

	// h000b, bit: 1
	/* reg_spdif_in_mode*/
	#define offset_of_padtop1_reg_spdif_in_mode (22)
	#define mask_of_padtop1_reg_spdif_in_mode (0x3)
	unsigned int reg_spdif_in_mode:2;

	// h000b, bit: 3
	/* reg_spdif_out_mode*/
	#define offset_of_padtop1_reg_spdif_out_mode (22)
	#define mask_of_padtop1_reg_spdif_out_mode (0xc)
	unsigned int reg_spdif_out_mode:2;

	// h000b, bit: 5
	/* reg_test_in_mode*/
	#define offset_of_padtop1_reg_test_in_mode (22)
	#define mask_of_padtop1_reg_test_in_mode (0x30)
	unsigned int reg_test_in_mode:2;

	// h000b, bit: 7
	/* reg_test_out_mode*/
	#define offset_of_padtop1_reg_test_out_mode (22)
	#define mask_of_padtop1_reg_test_out_mode (0xc0)
	unsigned int reg_test_out_mode:2;

	// h000b, bit: 8
	/* reg_ttl_out*/
	#define offset_of_padtop1_reg_ttl_out (22)
	#define mask_of_padtop1_reg_ttl_out (0x100)
	unsigned int reg_ttl_out:1;

	// h000b, bit: 10
	/* reg_usb30vctl_mode*/
	#define offset_of_padtop1_reg_usb30vctl_mode (22)
	#define mask_of_padtop1_reg_usb30vctl_mode (0x600)
	unsigned int reg_usb30vctl_mode:2;

	// h000b, bit: 12
	/* reg_usb30vctl_mode1*/
	#define offset_of_padtop1_reg_usb30vctl_mode1 (22)
	#define mask_of_padtop1_reg_usb30vctl_mode1 (0x1800)
	unsigned int reg_usb30vctl_mode1:2;

	// h000b, bit: 13
	/* reg_vsync_en*/
	#define offset_of_padtop1_reg_vsync_en (22)
	#define mask_of_padtop1_reg_vsync_en (0x2000)
	unsigned int reg_vsync_en:1;

	// h000b, bit: 14
	/* */
	unsigned int :2;

	// h000b
	unsigned int /* padding 16 bit */:16;

	// h000c, bit: 0
	/* reg_bt_i2c_rx_bck_drv*/
	#define offset_of_padtop1_reg_bt_i2c_rx_bck_drv (24)
	#define mask_of_padtop1_reg_bt_i2c_rx_bck_drv (0x1)
	unsigned int reg_bt_i2c_rx_bck_drv:1;

	// h000c, bit: 1
	/* reg_bt_i2c_rx_bck_ie*/
	#define offset_of_padtop1_reg_bt_i2c_rx_bck_ie (24)
	#define mask_of_padtop1_reg_bt_i2c_rx_bck_ie (0x2)
	unsigned int reg_bt_i2c_rx_bck_ie:1;

	// h000c, bit: 2
	/* reg_bt_i2c_rx_bck_pe*/
	#define offset_of_padtop1_reg_bt_i2c_rx_bck_pe (24)
	#define mask_of_padtop1_reg_bt_i2c_rx_bck_pe (0x4)
	unsigned int reg_bt_i2c_rx_bck_pe:1;

	// h000c, bit: 3
	/* reg_bt_i2c_rx_bck_ps*/
	#define offset_of_padtop1_reg_bt_i2c_rx_bck_ps (24)
	#define mask_of_padtop1_reg_bt_i2c_rx_bck_ps (0x8)
	unsigned int reg_bt_i2c_rx_bck_ps:1;

	// h000c, bit: 4
	/* reg_bt_i2c_rx_sdi_drv*/
	#define offset_of_padtop1_reg_bt_i2c_rx_sdi_drv (24)
	#define mask_of_padtop1_reg_bt_i2c_rx_sdi_drv (0x10)
	unsigned int reg_bt_i2c_rx_sdi_drv:1;

	// h000c, bit: 5
	/* reg_bt_i2c_rx_sdi_ie*/
	#define offset_of_padtop1_reg_bt_i2c_rx_sdi_ie (24)
	#define mask_of_padtop1_reg_bt_i2c_rx_sdi_ie (0x20)
	unsigned int reg_bt_i2c_rx_sdi_ie:1;

	// h000c, bit: 6
	/* reg_bt_i2c_rx_sdi_pe*/
	#define offset_of_padtop1_reg_bt_i2c_rx_sdi_pe (24)
	#define mask_of_padtop1_reg_bt_i2c_rx_sdi_pe (0x40)
	unsigned int reg_bt_i2c_rx_sdi_pe:1;

	// h000c, bit: 7
	/* reg_bt_i2c_rx_sdi_ps*/
	#define offset_of_padtop1_reg_bt_i2c_rx_sdi_ps (24)
	#define mask_of_padtop1_reg_bt_i2c_rx_sdi_ps (0x80)
	unsigned int reg_bt_i2c_rx_sdi_ps:1;

	// h000c, bit: 8
	/* reg_bt_i2c_rx_sdo_drv*/
	#define offset_of_padtop1_reg_bt_i2c_rx_sdo_drv (24)
	#define mask_of_padtop1_reg_bt_i2c_rx_sdo_drv (0x100)
	unsigned int reg_bt_i2c_rx_sdo_drv:1;

	// h000c, bit: 9
	/* reg_bt_i2c_rx_sdo_ie*/
	#define offset_of_padtop1_reg_bt_i2c_rx_sdo_ie (24)
	#define mask_of_padtop1_reg_bt_i2c_rx_sdo_ie (0x200)
	unsigned int reg_bt_i2c_rx_sdo_ie:1;

	// h000c, bit: 10
	/* reg_bt_i2c_rx_sdo_pe*/
	#define offset_of_padtop1_reg_bt_i2c_rx_sdo_pe (24)
	#define mask_of_padtop1_reg_bt_i2c_rx_sdo_pe (0x400)
	unsigned int reg_bt_i2c_rx_sdo_pe:1;

	// h000c, bit: 11
	/* reg_bt_i2c_rx_sdo_ps*/
	#define offset_of_padtop1_reg_bt_i2c_rx_sdo_ps (24)
	#define mask_of_padtop1_reg_bt_i2c_rx_sdo_ps (0x800)
	unsigned int reg_bt_i2c_rx_sdo_ps:1;

	// h000c, bit: 12
	/* reg_bt_i2c_rx_wck_drv*/
	#define offset_of_padtop1_reg_bt_i2c_rx_wck_drv (24)
	#define mask_of_padtop1_reg_bt_i2c_rx_wck_drv (0x1000)
	unsigned int reg_bt_i2c_rx_wck_drv:1;

	// h000c, bit: 13
	/* reg_bt_i2c_rx_wck_ie*/
	#define offset_of_padtop1_reg_bt_i2c_rx_wck_ie (24)
	#define mask_of_padtop1_reg_bt_i2c_rx_wck_ie (0x2000)
	unsigned int reg_bt_i2c_rx_wck_ie:1;

	// h000c, bit: 14
	/* reg_bt_i2c_rx_wck_pe*/
	#define offset_of_padtop1_reg_bt_i2c_rx_wck_pe (24)
	#define mask_of_padtop1_reg_bt_i2c_rx_wck_pe (0x4000)
	unsigned int reg_bt_i2c_rx_wck_pe:1;

	// h000c, bit: 15
	/* reg_bt_i2c_rx_wck_ps*/
	#define offset_of_padtop1_reg_bt_i2c_rx_wck_ps (24)
	#define mask_of_padtop1_reg_bt_i2c_rx_wck_ps (0x8000)
	unsigned int reg_bt_i2c_rx_wck_ps:1;

	// h000c
	unsigned int /* padding 16 bit */:16;

	// h000d, bit: 0
	/* reg_codec_i2s_tx_bck_drv*/
	#define offset_of_padtop1_reg_codec_i2s_tx_bck_drv (26)
	#define mask_of_padtop1_reg_codec_i2s_tx_bck_drv (0x1)
	unsigned int reg_codec_i2s_tx_bck_drv:1;

	// h000d, bit: 1
	/* reg_dmic_bck_drv*/
	#define offset_of_padtop1_reg_dmic_bck_drv (26)
	#define mask_of_padtop1_reg_dmic_bck_drv (0x2)
	unsigned int reg_dmic_bck_drv:1;

	// h000d, bit: 2
	/* reg_dmic_bck_ie*/
	#define offset_of_padtop1_reg_dmic_bck_ie (26)
	#define mask_of_padtop1_reg_dmic_bck_ie (0x4)
	unsigned int reg_dmic_bck_ie:1;

	// h000d, bit: 3
	/* reg_dmic_bck_pe*/
	#define offset_of_padtop1_reg_dmic_bck_pe (26)
	#define mask_of_padtop1_reg_dmic_bck_pe (0x8)
	unsigned int reg_dmic_bck_pe:1;

	// h000d, bit: 4
	/* reg_dmic_bck_ps*/
	#define offset_of_padtop1_reg_dmic_bck_ps (26)
	#define mask_of_padtop1_reg_dmic_bck_ps (0x10)
	unsigned int reg_dmic_bck_ps:1;

	// h000d, bit: 5
	/* reg_dmic_ch0_drv*/
	#define offset_of_padtop1_reg_dmic_ch0_drv (26)
	#define mask_of_padtop1_reg_dmic_ch0_drv (0x20)
	unsigned int reg_dmic_ch0_drv:1;

	// h000d, bit: 6
	/* reg_dmic_ch0_ie*/
	#define offset_of_padtop1_reg_dmic_ch0_ie (26)
	#define mask_of_padtop1_reg_dmic_ch0_ie (0x40)
	unsigned int reg_dmic_ch0_ie:1;

	// h000d, bit: 7
	/* reg_dmic_ch0_pe*/
	#define offset_of_padtop1_reg_dmic_ch0_pe (26)
	#define mask_of_padtop1_reg_dmic_ch0_pe (0x80)
	unsigned int reg_dmic_ch0_pe:1;

	// h000d, bit: 8
	/* reg_dmic_ch0_ps*/
	#define offset_of_padtop1_reg_dmic_ch0_ps (26)
	#define mask_of_padtop1_reg_dmic_ch0_ps (0x100)
	unsigned int reg_dmic_ch0_ps:1;

	// h000d, bit: 9
	/* reg_dmic_ch1_drv*/
	#define offset_of_padtop1_reg_dmic_ch1_drv (26)
	#define mask_of_padtop1_reg_dmic_ch1_drv (0x200)
	unsigned int reg_dmic_ch1_drv:1;

	// h000d, bit: 10
	/* reg_dmic_ch1_ie*/
	#define offset_of_padtop1_reg_dmic_ch1_ie (26)
	#define mask_of_padtop1_reg_dmic_ch1_ie (0x400)
	unsigned int reg_dmic_ch1_ie:1;

	// h000d, bit: 11
	/* reg_dmic_ch1_pe*/
	#define offset_of_padtop1_reg_dmic_ch1_pe (26)
	#define mask_of_padtop1_reg_dmic_ch1_pe (0x800)
	unsigned int reg_dmic_ch1_pe:1;

	// h000d, bit: 12
	/* reg_dmic_ch1_ps*/
	#define offset_of_padtop1_reg_dmic_ch1_ps (26)
	#define mask_of_padtop1_reg_dmic_ch1_ps (0x1000)
	unsigned int reg_dmic_ch1_ps:1;

	// h000d, bit: 13
	/* reg_dmic_ch2_drv*/
	#define offset_of_padtop1_reg_dmic_ch2_drv (26)
	#define mask_of_padtop1_reg_dmic_ch2_drv (0x2000)
	unsigned int reg_dmic_ch2_drv:1;

	// h000d, bit: 14
	/* reg_dmic_ch2_ie*/
	#define offset_of_padtop1_reg_dmic_ch2_ie (26)
	#define mask_of_padtop1_reg_dmic_ch2_ie (0x4000)
	unsigned int reg_dmic_ch2_ie:1;

	// h000d, bit: 15
	/* reg_dmic_ch2_pe*/
	#define offset_of_padtop1_reg_dmic_ch2_pe (26)
	#define mask_of_padtop1_reg_dmic_ch2_pe (0x8000)
	unsigned int reg_dmic_ch2_pe:1;

	// h000d
	unsigned int /* padding 16 bit */:16;

	// h000e, bit: 0
	/* reg_dmic_ch2_ps*/
	#define offset_of_padtop1_reg_dmic_ch2_ps (28)
	#define mask_of_padtop1_reg_dmic_ch2_ps (0x1)
	unsigned int reg_dmic_ch2_ps:1;

	// h000e, bit: 1
	/* reg_dmic_ch3_drv*/
	#define offset_of_padtop1_reg_dmic_ch3_drv (28)
	#define mask_of_padtop1_reg_dmic_ch3_drv (0x2)
	unsigned int reg_dmic_ch3_drv:1;

	// h000e, bit: 2
	/* reg_dmic_ch3_ie*/
	#define offset_of_padtop1_reg_dmic_ch3_ie (28)
	#define mask_of_padtop1_reg_dmic_ch3_ie (0x4)
	unsigned int reg_dmic_ch3_ie:1;

	// h000e, bit: 3
	/* reg_dmic_ch3_pe*/
	#define offset_of_padtop1_reg_dmic_ch3_pe (28)
	#define mask_of_padtop1_reg_dmic_ch3_pe (0x8)
	unsigned int reg_dmic_ch3_pe:1;

	// h000e, bit: 4
	/* reg_dmic_ch3_ps*/
	#define offset_of_padtop1_reg_dmic_ch3_ps (28)
	#define mask_of_padtop1_reg_dmic_ch3_ps (0x10)
	unsigned int reg_dmic_ch3_ps:1;

	// h000e, bit: 5
	/* reg_fuart_cts_drv*/
	#define offset_of_padtop1_reg_fuart_cts_drv (28)
	#define mask_of_padtop1_reg_fuart_cts_drv (0x20)
	unsigned int reg_fuart_cts_drv:1;

	// h000e, bit: 6
	/* reg_fuart_rts_drv*/
	#define offset_of_padtop1_reg_fuart_rts_drv (28)
	#define mask_of_padtop1_reg_fuart_rts_drv (0x40)
	unsigned int reg_fuart_rts_drv:1;

	// h000e, bit: 7
	/* reg_fuart_rx_drv*/
	#define offset_of_padtop1_reg_fuart_rx_drv (28)
	#define mask_of_padtop1_reg_fuart_rx_drv (0x80)
	unsigned int reg_fuart_rx_drv:1;

	// h000e, bit: 8
	/* reg_fuart_tx_drv*/
	#define offset_of_padtop1_reg_fuart_tx_drv (28)
	#define mask_of_padtop1_reg_fuart_tx_drv (0x100)
	unsigned int reg_fuart_tx_drv:1;

	// h000e, bit: 14
	/* */
	unsigned int :7;

	// h000e
	unsigned int /* padding 16 bit */:16;

	// h000f, bit: 3
	/* reg_gpio0_drv*/
	#define offset_of_padtop1_reg_gpio0_drv (30)
	#define mask_of_padtop1_reg_gpio0_drv (0xf)
	unsigned int reg_gpio0_drv:4;

	// h000f, bit: 5
	/* reg_gpio0_pe*/
	#define offset_of_padtop1_reg_gpio0_pe (30)
	#define mask_of_padtop1_reg_gpio0_pe (0x30)
	unsigned int reg_gpio0_pe:2;

	// h000f, bit: 7
	/* reg_gpio0_ps*/
	#define offset_of_padtop1_reg_gpio0_ps (30)
	#define mask_of_padtop1_reg_gpio0_ps (0xc0)
	unsigned int reg_gpio0_ps:2;

	// h000f, bit: 14
	/* */
	unsigned int :8;

	// h000f
	unsigned int /* padding 16 bit */:16;

	// h0010, bit: 14
	/* reg_gpio_drv*/
	#define offset_of_padtop1_reg_gpio_drv (32)
	#define mask_of_padtop1_reg_gpio_drv (0xffff)
	unsigned int reg_gpio_drv:16;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 14
	/* reg_gpio_pe*/
	#define offset_of_padtop1_reg_gpio_pe (34)
	#define mask_of_padtop1_reg_gpio_pe (0xffff)
	unsigned int reg_gpio_pe:16;

	// h0011
	unsigned int /* padding 16 bit */:16;

	// h0012, bit: 14
	/* reg_gpio_ps*/
	#define offset_of_padtop1_reg_gpio_ps (36)
	#define mask_of_padtop1_reg_gpio_ps (0xffff)
	unsigned int reg_gpio_ps:16;

	// h0012
	unsigned int /* padding 16 bit */:16;

	// h0013, bit: 11
	/* reg_gt0_c0*/
	#define offset_of_padtop1_reg_gt0_c0 (38)
	#define mask_of_padtop1_reg_gt0_c0 (0xfff)
	unsigned int reg_gt0_c0:12;

	// h0013, bit: 14
	/* */
	unsigned int :4;

	// h0013
	unsigned int /* padding 16 bit */:16;

	// h0014, bit: 11
	/* reg_gt0_c1*/
	#define offset_of_padtop1_reg_gt0_c1 (40)
	#define mask_of_padtop1_reg_gt0_c1 (0xfff)
	unsigned int reg_gt0_c1:12;

	// h0014, bit: 14
	/* */
	unsigned int :4;

	// h0014
	unsigned int /* padding 16 bit */:16;

	// h0015, bit: 11
	/* reg_gt0_pe*/
	#define offset_of_padtop1_reg_gt0_pe (42)
	#define mask_of_padtop1_reg_gt0_pe (0xfff)
	unsigned int reg_gt0_pe:12;

	// h0015, bit: 14
	/* */
	unsigned int :4;

	// h0015
	unsigned int /* padding 16 bit */:16;

	// h0016, bit: 11
	/* reg_gt0_ps*/
	#define offset_of_padtop1_reg_gt0_ps (44)
	#define mask_of_padtop1_reg_gt0_ps (0xfff)
	unsigned int reg_gt0_ps:12;

	// h0016, bit: 14
	/* */
	unsigned int :4;

	// h0016
	unsigned int /* padding 16 bit */:16;

	// h0017, bit: 11
	/* reg_gt1_c0*/
	#define offset_of_padtop1_reg_gt1_c0 (46)
	#define mask_of_padtop1_reg_gt1_c0 (0xfff)
	unsigned int reg_gt1_c0:12;

	// h0017, bit: 14
	/* */
	unsigned int :4;

	// h0017
	unsigned int /* padding 16 bit */:16;

	// h0018, bit: 11
	/* reg_gt1_c1*/
	#define offset_of_padtop1_reg_gt1_c1 (48)
	#define mask_of_padtop1_reg_gt1_c1 (0xfff)
	unsigned int reg_gt1_c1:12;

	// h0018, bit: 14
	/* */
	unsigned int :4;

	// h0018
	unsigned int /* padding 16 bit */:16;

	// h0019, bit: 11
	/* reg_gt1_pe*/
	#define offset_of_padtop1_reg_gt1_pe (50)
	#define mask_of_padtop1_reg_gt1_pe (0xfff)
	unsigned int reg_gt1_pe:12;

	// h0019, bit: 14
	/* */
	unsigned int :4;

	// h0019
	unsigned int /* padding 16 bit */:16;

	// h001a, bit: 11
	/* reg_gt1_ps*/
	#define offset_of_padtop1_reg_gt1_ps (52)
	#define mask_of_padtop1_reg_gt1_ps (0xfff)
	unsigned int reg_gt1_ps:12;

	// h001a, bit: 14
	/* */
	unsigned int :4;

	// h001a
	unsigned int /* padding 16 bit */:16;

	// h001b, bit: 0
	/* reg_hdmitx_arc_bypass*/
	#define offset_of_padtop1_reg_hdmitx_arc_bypass (54)
	#define mask_of_padtop1_reg_hdmitx_arc_bypass (0x1)
	unsigned int reg_hdmitx_arc_bypass:1;

	// h001b, bit: 1
	/* reg_hdmitx_arc_gpio_en*/
	#define offset_of_padtop1_reg_hdmitx_arc_gpio_en (54)
	#define mask_of_padtop1_reg_hdmitx_arc_gpio_en (0x2)
	unsigned int reg_hdmitx_arc_gpio_en:1;

	// h001b, bit: 2
	/* reg_hdmitx_arc_pd_rt*/
	#define offset_of_padtop1_reg_hdmitx_arc_pd_rt (54)
	#define mask_of_padtop1_reg_hdmitx_arc_pd_rt (0x4)
	unsigned int reg_hdmitx_arc_pd_rt:1;

	// h001b, bit: 3
	/* reg_hsync_out_drv*/
	#define offset_of_padtop1_reg_hsync_out_drv (54)
	#define mask_of_padtop1_reg_hsync_out_drv (0x8)
	unsigned int reg_hsync_out_drv:1;

	// h001b, bit: 4
	/* reg_i2c0_scl_drv*/
	#define offset_of_padtop1_reg_i2c0_scl_drv (54)
	#define mask_of_padtop1_reg_i2c0_scl_drv (0x10)
	unsigned int reg_i2c0_scl_drv:1;

	// h001b, bit: 5
	/* reg_i2c0_sda_drv*/
	#define offset_of_padtop1_reg_i2c0_sda_drv (54)
	#define mask_of_padtop1_reg_i2c0_sda_drv (0x20)
	unsigned int reg_i2c0_sda_drv:1;

	// h001b, bit: 6
	/* reg_i2c2_scl_drv*/
	#define offset_of_padtop1_reg_i2c2_scl_drv (54)
	#define mask_of_padtop1_reg_i2c2_scl_drv (0x40)
	unsigned int reg_i2c2_scl_drv:1;

	// h001b, bit: 7
	/* reg_i2c2_scl_ie*/
	#define offset_of_padtop1_reg_i2c2_scl_ie (54)
	#define mask_of_padtop1_reg_i2c2_scl_ie (0x80)
	unsigned int reg_i2c2_scl_ie:1;

	// h001b, bit: 8
	/* reg_i2c2_scl_pe*/
	#define offset_of_padtop1_reg_i2c2_scl_pe (54)
	#define mask_of_padtop1_reg_i2c2_scl_pe (0x100)
	unsigned int reg_i2c2_scl_pe:1;

	// h001b, bit: 9
	/* reg_i2c2_scl_ps*/
	#define offset_of_padtop1_reg_i2c2_scl_ps (54)
	#define mask_of_padtop1_reg_i2c2_scl_ps (0x200)
	unsigned int reg_i2c2_scl_ps:1;

	// h001b, bit: 10
	/* reg_i2c2_sda_drv*/
	#define offset_of_padtop1_reg_i2c2_sda_drv (54)
	#define mask_of_padtop1_reg_i2c2_sda_drv (0x400)
	unsigned int reg_i2c2_sda_drv:1;

	// h001b, bit: 11
	/* reg_i2c2_sda_ie*/
	#define offset_of_padtop1_reg_i2c2_sda_ie (54)
	#define mask_of_padtop1_reg_i2c2_sda_ie (0x800)
	unsigned int reg_i2c2_sda_ie:1;

	// h001b, bit: 12
	/* reg_i2c2_sda_pe*/
	#define offset_of_padtop1_reg_i2c2_sda_pe (54)
	#define mask_of_padtop1_reg_i2c2_sda_pe (0x1000)
	unsigned int reg_i2c2_sda_pe:1;

	// h001b, bit: 13
	/* reg_i2c2_sda_ps*/
	#define offset_of_padtop1_reg_i2c2_sda_ps (54)
	#define mask_of_padtop1_reg_i2c2_sda_ps (0x2000)
	unsigned int reg_i2c2_sda_ps:1;

	// h001b, bit: 14
	/* reg_i2c3_scl_drv*/
	#define offset_of_padtop1_reg_i2c3_scl_drv (54)
	#define mask_of_padtop1_reg_i2c3_scl_drv (0x4000)
	unsigned int reg_i2c3_scl_drv:1;

	// h001b, bit: 15
	/* reg_i2c3_scl_ie*/
	#define offset_of_padtop1_reg_i2c3_scl_ie (54)
	#define mask_of_padtop1_reg_i2c3_scl_ie (0x8000)
	unsigned int reg_i2c3_scl_ie:1;

	// h001b
	unsigned int /* padding 16 bit */:16;

	// h001c, bit: 0
	/* reg_i2c3_scl_pe*/
	#define offset_of_padtop1_reg_i2c3_scl_pe (56)
	#define mask_of_padtop1_reg_i2c3_scl_pe (0x1)
	unsigned int reg_i2c3_scl_pe:1;

	// h001c, bit: 1
	/* reg_i2c3_scl_ps*/
	#define offset_of_padtop1_reg_i2c3_scl_ps (56)
	#define mask_of_padtop1_reg_i2c3_scl_ps (0x2)
	unsigned int reg_i2c3_scl_ps:1;

	// h001c, bit: 2
	/* reg_i2c3_sda_drv*/
	#define offset_of_padtop1_reg_i2c3_sda_drv (56)
	#define mask_of_padtop1_reg_i2c3_sda_drv (0x4)
	unsigned int reg_i2c3_sda_drv:1;

	// h001c, bit: 3
	/* reg_i2c3_sda_ie*/
	#define offset_of_padtop1_reg_i2c3_sda_ie (56)
	#define mask_of_padtop1_reg_i2c3_sda_ie (0x8)
	unsigned int reg_i2c3_sda_ie:1;

	// h001c, bit: 4
	/* reg_i2c3_sda_pe*/
	#define offset_of_padtop1_reg_i2c3_sda_pe (56)
	#define mask_of_padtop1_reg_i2c3_sda_pe (0x10)
	unsigned int reg_i2c3_sda_pe:1;

	// h001c, bit: 5
	/* reg_i2c3_sda_ps*/
	#define offset_of_padtop1_reg_i2c3_sda_ps (56)
	#define mask_of_padtop1_reg_i2c3_sda_ps (0x20)
	unsigned int reg_i2c3_sda_ps:1;

	// h001c, bit: 6
	/* reg_i2s_rx_bck_drv*/
	#define offset_of_padtop1_reg_i2s_rx_bck_drv (56)
	#define mask_of_padtop1_reg_i2s_rx_bck_drv (0x40)
	unsigned int reg_i2s_rx_bck_drv:1;

	// h001c, bit: 7
	/* reg_i2s_rx_bck_ie*/
	#define offset_of_padtop1_reg_i2s_rx_bck_ie (56)
	#define mask_of_padtop1_reg_i2s_rx_bck_ie (0x80)
	unsigned int reg_i2s_rx_bck_ie:1;

	// h001c, bit: 8
	/* reg_i2s_rx_bck_pe*/
	#define offset_of_padtop1_reg_i2s_rx_bck_pe (56)
	#define mask_of_padtop1_reg_i2s_rx_bck_pe (0x100)
	unsigned int reg_i2s_rx_bck_pe:1;

	// h001c, bit: 9
	/* reg_i2s_rx_bck_ps*/
	#define offset_of_padtop1_reg_i2s_rx_bck_ps (56)
	#define mask_of_padtop1_reg_i2s_rx_bck_ps (0x200)
	unsigned int reg_i2s_rx_bck_ps:1;

	// h001c, bit: 10
	/* reg_i2s_rx_mck_drv*/
	#define offset_of_padtop1_reg_i2s_rx_mck_drv (56)
	#define mask_of_padtop1_reg_i2s_rx_mck_drv (0x400)
	unsigned int reg_i2s_rx_mck_drv:1;

	// h001c, bit: 11
	/* reg_i2s_rx_mck_ie*/
	#define offset_of_padtop1_reg_i2s_rx_mck_ie (56)
	#define mask_of_padtop1_reg_i2s_rx_mck_ie (0x800)
	unsigned int reg_i2s_rx_mck_ie:1;

	// h001c, bit: 12
	/* reg_i2s_rx_mck_pe*/
	#define offset_of_padtop1_reg_i2s_rx_mck_pe (56)
	#define mask_of_padtop1_reg_i2s_rx_mck_pe (0x1000)
	unsigned int reg_i2s_rx_mck_pe:1;

	// h001c, bit: 13
	/* reg_i2s_rx_mck_ps*/
	#define offset_of_padtop1_reg_i2s_rx_mck_ps (56)
	#define mask_of_padtop1_reg_i2s_rx_mck_ps (0x2000)
	unsigned int reg_i2s_rx_mck_ps:1;

	// h001c, bit: 14
	/* reg_i2s_rx_sdi0_drv*/
	#define offset_of_padtop1_reg_i2s_rx_sdi0_drv (56)
	#define mask_of_padtop1_reg_i2s_rx_sdi0_drv (0x4000)
	unsigned int reg_i2s_rx_sdi0_drv:1;

	// h001c, bit: 15
	/* reg_i2s_rx_sdi0_ie*/
	#define offset_of_padtop1_reg_i2s_rx_sdi0_ie (56)
	#define mask_of_padtop1_reg_i2s_rx_sdi0_ie (0x8000)
	unsigned int reg_i2s_rx_sdi0_ie:1;

	// h001c
	unsigned int /* padding 16 bit */:16;

	// h001d, bit: 0
	/* reg_i2s_rx_sdi0_pe*/
	#define offset_of_padtop1_reg_i2s_rx_sdi0_pe (58)
	#define mask_of_padtop1_reg_i2s_rx_sdi0_pe (0x1)
	unsigned int reg_i2s_rx_sdi0_pe:1;

	// h001d, bit: 1
	/* reg_i2s_rx_sdi0_ps*/
	#define offset_of_padtop1_reg_i2s_rx_sdi0_ps (58)
	#define mask_of_padtop1_reg_i2s_rx_sdi0_ps (0x2)
	unsigned int reg_i2s_rx_sdi0_ps:1;

	// h001d, bit: 2
	/* reg_i2s_rx_sdi1_drv*/
	#define offset_of_padtop1_reg_i2s_rx_sdi1_drv (58)
	#define mask_of_padtop1_reg_i2s_rx_sdi1_drv (0x4)
	unsigned int reg_i2s_rx_sdi1_drv:1;

	// h001d, bit: 3
	/* reg_i2s_rx_sdi1_ie*/
	#define offset_of_padtop1_reg_i2s_rx_sdi1_ie (58)
	#define mask_of_padtop1_reg_i2s_rx_sdi1_ie (0x8)
	unsigned int reg_i2s_rx_sdi1_ie:1;

	// h001d, bit: 4
	/* reg_i2s_rx_sdi1_pe*/
	#define offset_of_padtop1_reg_i2s_rx_sdi1_pe (58)
	#define mask_of_padtop1_reg_i2s_rx_sdi1_pe (0x10)
	unsigned int reg_i2s_rx_sdi1_pe:1;

	// h001d, bit: 5
	/* reg_i2s_rx_sdi1_ps*/
	#define offset_of_padtop1_reg_i2s_rx_sdi1_ps (58)
	#define mask_of_padtop1_reg_i2s_rx_sdi1_ps (0x20)
	unsigned int reg_i2s_rx_sdi1_ps:1;

	// h001d, bit: 6
	/* reg_i2s_rx_sdi2_drv*/
	#define offset_of_padtop1_reg_i2s_rx_sdi2_drv (58)
	#define mask_of_padtop1_reg_i2s_rx_sdi2_drv (0x40)
	unsigned int reg_i2s_rx_sdi2_drv:1;

	// h001d, bit: 7
	/* reg_i2s_rx_sdi2_ie*/
	#define offset_of_padtop1_reg_i2s_rx_sdi2_ie (58)
	#define mask_of_padtop1_reg_i2s_rx_sdi2_ie (0x80)
	unsigned int reg_i2s_rx_sdi2_ie:1;

	// h001d, bit: 8
	/* reg_i2s_rx_sdi2_pe*/
	#define offset_of_padtop1_reg_i2s_rx_sdi2_pe (58)
	#define mask_of_padtop1_reg_i2s_rx_sdi2_pe (0x100)
	unsigned int reg_i2s_rx_sdi2_pe:1;

	// h001d, bit: 9
	/* reg_i2s_rx_sdi2_ps*/
	#define offset_of_padtop1_reg_i2s_rx_sdi2_ps (58)
	#define mask_of_padtop1_reg_i2s_rx_sdi2_ps (0x200)
	unsigned int reg_i2s_rx_sdi2_ps:1;

	// h001d, bit: 10
	/* reg_i2s_rx_sdi3_drv*/
	#define offset_of_padtop1_reg_i2s_rx_sdi3_drv (58)
	#define mask_of_padtop1_reg_i2s_rx_sdi3_drv (0x400)
	unsigned int reg_i2s_rx_sdi3_drv:1;

	// h001d, bit: 11
	/* reg_i2s_rx_sdi3_ie*/
	#define offset_of_padtop1_reg_i2s_rx_sdi3_ie (58)
	#define mask_of_padtop1_reg_i2s_rx_sdi3_ie (0x800)
	unsigned int reg_i2s_rx_sdi3_ie:1;

	// h001d, bit: 12
	/* reg_i2s_rx_sdi3_pe*/
	#define offset_of_padtop1_reg_i2s_rx_sdi3_pe (58)
	#define mask_of_padtop1_reg_i2s_rx_sdi3_pe (0x1000)
	unsigned int reg_i2s_rx_sdi3_pe:1;

	// h001d, bit: 13
	/* reg_i2s_rx_sdi3_ps*/
	#define offset_of_padtop1_reg_i2s_rx_sdi3_ps (58)
	#define mask_of_padtop1_reg_i2s_rx_sdi3_ps (0x2000)
	unsigned int reg_i2s_rx_sdi3_ps:1;

	// h001d, bit: 14
	/* reg_i2s_rx_wck_drv*/
	#define offset_of_padtop1_reg_i2s_rx_wck_drv (58)
	#define mask_of_padtop1_reg_i2s_rx_wck_drv (0x4000)
	unsigned int reg_i2s_rx_wck_drv:1;

	// h001d, bit: 15
	/* reg_i2s_rx_wck_ie*/
	#define offset_of_padtop1_reg_i2s_rx_wck_ie (58)
	#define mask_of_padtop1_reg_i2s_rx_wck_ie (0x8000)
	unsigned int reg_i2s_rx_wck_ie:1;

	// h001d
	unsigned int /* padding 16 bit */:16;

	// h001e, bit: 0
	/* reg_i2s_rx_wck_pe*/
	#define offset_of_padtop1_reg_i2s_rx_wck_pe (60)
	#define mask_of_padtop1_reg_i2s_rx_wck_pe (0x1)
	unsigned int reg_i2s_rx_wck_pe:1;

	// h001e, bit: 1
	/* reg_i2s_rx_wck_ps*/
	#define offset_of_padtop1_reg_i2s_rx_wck_ps (60)
	#define mask_of_padtop1_reg_i2s_rx_wck_ps (0x2)
	unsigned int reg_i2s_rx_wck_ps:1;

	// h001e, bit: 2
	/* reg_i2s_tx_bck_ie*/
	#define offset_of_padtop1_reg_i2s_tx_bck_ie (60)
	#define mask_of_padtop1_reg_i2s_tx_bck_ie (0x4)
	unsigned int reg_i2s_tx_bck_ie:1;

	// h001e, bit: 3
	/* reg_i2s_tx_bck_pe*/
	#define offset_of_padtop1_reg_i2s_tx_bck_pe (60)
	#define mask_of_padtop1_reg_i2s_tx_bck_pe (0x8)
	unsigned int reg_i2s_tx_bck_pe:1;

	// h001e, bit: 4
	/* reg_i2s_tx_bck_ps*/
	#define offset_of_padtop1_reg_i2s_tx_bck_ps (60)
	#define mask_of_padtop1_reg_i2s_tx_bck_ps (0x10)
	unsigned int reg_i2s_tx_bck_ps:1;

	// h001e, bit: 5
	/* reg_i2s_tx_sdo_drv*/
	#define offset_of_padtop1_reg_i2s_tx_sdo_drv (60)
	#define mask_of_padtop1_reg_i2s_tx_sdo_drv (0x20)
	unsigned int reg_i2s_tx_sdo_drv:1;

	// h001e, bit: 6
	/* reg_i2s_tx_sdo_ie*/
	#define offset_of_padtop1_reg_i2s_tx_sdo_ie (60)
	#define mask_of_padtop1_reg_i2s_tx_sdo_ie (0x40)
	unsigned int reg_i2s_tx_sdo_ie:1;

	// h001e, bit: 7
	/* reg_i2s_tx_sdo_pe*/
	#define offset_of_padtop1_reg_i2s_tx_sdo_pe (60)
	#define mask_of_padtop1_reg_i2s_tx_sdo_pe (0x80)
	unsigned int reg_i2s_tx_sdo_pe:1;

	// h001e, bit: 8
	/* reg_i2s_tx_sdo_ps*/
	#define offset_of_padtop1_reg_i2s_tx_sdo_ps (60)
	#define mask_of_padtop1_reg_i2s_tx_sdo_ps (0x100)
	unsigned int reg_i2s_tx_sdo_ps:1;

	// h001e, bit: 9
	/* reg_i2s_tx_wck_drv*/
	#define offset_of_padtop1_reg_i2s_tx_wck_drv (60)
	#define mask_of_padtop1_reg_i2s_tx_wck_drv (0x200)
	unsigned int reg_i2s_tx_wck_drv:1;

	// h001e, bit: 10
	/* reg_i2s_tx_wck_ie*/
	#define offset_of_padtop1_reg_i2s_tx_wck_ie (60)
	#define mask_of_padtop1_reg_i2s_tx_wck_ie (0x400)
	unsigned int reg_i2s_tx_wck_ie:1;

	// h001e, bit: 11
	/* reg_i2s_tx_wck_pe*/
	#define offset_of_padtop1_reg_i2s_tx_wck_pe (60)
	#define mask_of_padtop1_reg_i2s_tx_wck_pe (0x800)
	unsigned int reg_i2s_tx_wck_pe:1;

	// h001e, bit: 12
	/* reg_i2s_tx_wck_ps*/
	#define offset_of_padtop1_reg_i2s_tx_wck_ps (60)
	#define mask_of_padtop1_reg_i2s_tx_wck_ps (0x1000)
	unsigned int reg_i2s_tx_wck_ps:1;

	// h001e, bit: 13
	/* reg_jtag_tck_drv*/
	#define offset_of_padtop1_reg_jtag_tck_drv (60)
	#define mask_of_padtop1_reg_jtag_tck_drv (0x2000)
	unsigned int reg_jtag_tck_drv:1;

	// h001e, bit: 14
	/* reg_jtag_tck_pe*/
	#define offset_of_padtop1_reg_jtag_tck_pe (60)
	#define mask_of_padtop1_reg_jtag_tck_pe (0x4000)
	unsigned int reg_jtag_tck_pe:1;

	// h001e, bit: 15
	/* reg_jtag_tck_ps*/
	#define offset_of_padtop1_reg_jtag_tck_ps (60)
	#define mask_of_padtop1_reg_jtag_tck_ps (0x8000)
	unsigned int reg_jtag_tck_ps:1;

	// h001e
	unsigned int /* padding 16 bit */:16;

	// h001f, bit: 0
	/* reg_jtag_tdi_drv*/
	#define offset_of_padtop1_reg_jtag_tdi_drv (62)
	#define mask_of_padtop1_reg_jtag_tdi_drv (0x1)
	unsigned int reg_jtag_tdi_drv:1;

	// h001f, bit: 1
	/* reg_jtag_tdi_pe*/
	#define offset_of_padtop1_reg_jtag_tdi_pe (62)
	#define mask_of_padtop1_reg_jtag_tdi_pe (0x2)
	unsigned int reg_jtag_tdi_pe:1;

	// h001f, bit: 2
	/* reg_jtag_tdi_ps*/
	#define offset_of_padtop1_reg_jtag_tdi_ps (62)
	#define mask_of_padtop1_reg_jtag_tdi_ps (0x4)
	unsigned int reg_jtag_tdi_ps:1;

	// h001f, bit: 3
	/* reg_jtag_tdo_drv*/
	#define offset_of_padtop1_reg_jtag_tdo_drv (62)
	#define mask_of_padtop1_reg_jtag_tdo_drv (0x8)
	unsigned int reg_jtag_tdo_drv:1;

	// h001f, bit: 4
	/* reg_jtag_tdo_pe*/
	#define offset_of_padtop1_reg_jtag_tdo_pe (62)
	#define mask_of_padtop1_reg_jtag_tdo_pe (0x10)
	unsigned int reg_jtag_tdo_pe:1;

	// h001f, bit: 5
	/* reg_jtag_tdo_ps*/
	#define offset_of_padtop1_reg_jtag_tdo_ps (62)
	#define mask_of_padtop1_reg_jtag_tdo_ps (0x20)
	unsigned int reg_jtag_tdo_ps:1;

	// h001f, bit: 6
	/* reg_jtag_tms_drv*/
	#define offset_of_padtop1_reg_jtag_tms_drv (62)
	#define mask_of_padtop1_reg_jtag_tms_drv (0x40)
	unsigned int reg_jtag_tms_drv:1;

	// h001f, bit: 7
	/* reg_jtag_tms_pe*/
	#define offset_of_padtop1_reg_jtag_tms_pe (62)
	#define mask_of_padtop1_reg_jtag_tms_pe (0x80)
	unsigned int reg_jtag_tms_pe:1;

	// h001f, bit: 8
	/* reg_jtag_tms_ps*/
	#define offset_of_padtop1_reg_jtag_tms_ps (62)
	#define mask_of_padtop1_reg_jtag_tms_ps (0x100)
	unsigned int reg_jtag_tms_ps:1;

	// h001f, bit: 9
	/* reg_misc_i2s_rx_bck_drv*/
	#define offset_of_padtop1_reg_misc_i2s_rx_bck_drv (62)
	#define mask_of_padtop1_reg_misc_i2s_rx_bck_drv (0x200)
	unsigned int reg_misc_i2s_rx_bck_drv:1;

	// h001f, bit: 10
	/* reg_misc_i2s_rx_bck_ie*/
	#define offset_of_padtop1_reg_misc_i2s_rx_bck_ie (62)
	#define mask_of_padtop1_reg_misc_i2s_rx_bck_ie (0x400)
	unsigned int reg_misc_i2s_rx_bck_ie:1;

	// h001f, bit: 11
	/* reg_misc_i2s_rx_bck_pe*/
	#define offset_of_padtop1_reg_misc_i2s_rx_bck_pe (62)
	#define mask_of_padtop1_reg_misc_i2s_rx_bck_pe (0x800)
	unsigned int reg_misc_i2s_rx_bck_pe:1;

	// h001f, bit: 12
	/* reg_misc_i2s_rx_bck_ps*/
	#define offset_of_padtop1_reg_misc_i2s_rx_bck_ps (62)
	#define mask_of_padtop1_reg_misc_i2s_rx_bck_ps (0x1000)
	unsigned int reg_misc_i2s_rx_bck_ps:1;

	// h001f, bit: 13
	/* reg_misc_i2s_rx_mck_drv*/
	#define offset_of_padtop1_reg_misc_i2s_rx_mck_drv (62)
	#define mask_of_padtop1_reg_misc_i2s_rx_mck_drv (0x2000)
	unsigned int reg_misc_i2s_rx_mck_drv:1;

	// h001f, bit: 14
	/* reg_misc_i2s_rx_mck_ie*/
	#define offset_of_padtop1_reg_misc_i2s_rx_mck_ie (62)
	#define mask_of_padtop1_reg_misc_i2s_rx_mck_ie (0x4000)
	unsigned int reg_misc_i2s_rx_mck_ie:1;

	// h001f, bit: 15
	/* reg_misc_i2s_rx_mck_pe*/
	#define offset_of_padtop1_reg_misc_i2s_rx_mck_pe (62)
	#define mask_of_padtop1_reg_misc_i2s_rx_mck_pe (0x8000)
	unsigned int reg_misc_i2s_rx_mck_pe:1;

	// h001f
	unsigned int /* padding 16 bit */:16;

	// h0020, bit: 0
	/* reg_misc_i2s_rx_mck_ps*/
	#define offset_of_padtop1_reg_misc_i2s_rx_mck_ps (64)
	#define mask_of_padtop1_reg_misc_i2s_rx_mck_ps (0x1)
	unsigned int reg_misc_i2s_rx_mck_ps:1;

	// h0020, bit: 1
	/* reg_misc_i2s_rx_sdio_drv*/
	#define offset_of_padtop1_reg_misc_i2s_rx_sdio_drv (64)
	#define mask_of_padtop1_reg_misc_i2s_rx_sdio_drv (0x2)
	unsigned int reg_misc_i2s_rx_sdio_drv:1;

	// h0020, bit: 2
	/* reg_misc_i2s_rx_sdio_ie*/
	#define offset_of_padtop1_reg_misc_i2s_rx_sdio_ie (64)
	#define mask_of_padtop1_reg_misc_i2s_rx_sdio_ie (0x4)
	unsigned int reg_misc_i2s_rx_sdio_ie:1;

	// h0020, bit: 3
	/* reg_misc_i2s_rx_sdio_pe*/
	#define offset_of_padtop1_reg_misc_i2s_rx_sdio_pe (64)
	#define mask_of_padtop1_reg_misc_i2s_rx_sdio_pe (0x8)
	unsigned int reg_misc_i2s_rx_sdio_pe:1;

	// h0020, bit: 4
	/* reg_misc_i2s_rx_sdio_ps*/
	#define offset_of_padtop1_reg_misc_i2s_rx_sdio_ps (64)
	#define mask_of_padtop1_reg_misc_i2s_rx_sdio_ps (0x10)
	unsigned int reg_misc_i2s_rx_sdio_ps:1;

	// h0020, bit: 5
	/* reg_misc_i2s_rx_wck_drv*/
	#define offset_of_padtop1_reg_misc_i2s_rx_wck_drv (64)
	#define mask_of_padtop1_reg_misc_i2s_rx_wck_drv (0x20)
	unsigned int reg_misc_i2s_rx_wck_drv:1;

	// h0020, bit: 6
	/* reg_misc_i2s_rx_wck_ie*/
	#define offset_of_padtop1_reg_misc_i2s_rx_wck_ie (64)
	#define mask_of_padtop1_reg_misc_i2s_rx_wck_ie (0x40)
	unsigned int reg_misc_i2s_rx_wck_ie:1;

	// h0020, bit: 7
	/* reg_misc_i2s_rx_wck_pe*/
	#define offset_of_padtop1_reg_misc_i2s_rx_wck_pe (64)
	#define mask_of_padtop1_reg_misc_i2s_rx_wck_pe (0x80)
	unsigned int reg_misc_i2s_rx_wck_pe:1;

	// h0020, bit: 8
	/* reg_misc_i2s_rx_wck_ps*/
	#define offset_of_padtop1_reg_misc_i2s_rx_wck_ps (64)
	#define mask_of_padtop1_reg_misc_i2s_rx_wck_ps (0x100)
	unsigned int reg_misc_i2s_rx_wck_ps:1;

	// h0020, bit: 9
	/* reg_misc_i2s_tx_bck_drv*/
	#define offset_of_padtop1_reg_misc_i2s_tx_bck_drv (64)
	#define mask_of_padtop1_reg_misc_i2s_tx_bck_drv (0x200)
	unsigned int reg_misc_i2s_tx_bck_drv:1;

	// h0020, bit: 10
	/* reg_misc_i2s_tx_bck_ie*/
	#define offset_of_padtop1_reg_misc_i2s_tx_bck_ie (64)
	#define mask_of_padtop1_reg_misc_i2s_tx_bck_ie (0x400)
	unsigned int reg_misc_i2s_tx_bck_ie:1;

	// h0020, bit: 11
	/* reg_misc_i2s_tx_bck_pe*/
	#define offset_of_padtop1_reg_misc_i2s_tx_bck_pe (64)
	#define mask_of_padtop1_reg_misc_i2s_tx_bck_pe (0x800)
	unsigned int reg_misc_i2s_tx_bck_pe:1;

	// h0020, bit: 12
	/* reg_misc_i2s_tx_bck_ps*/
	#define offset_of_padtop1_reg_misc_i2s_tx_bck_ps (64)
	#define mask_of_padtop1_reg_misc_i2s_tx_bck_ps (0x1000)
	unsigned int reg_misc_i2s_tx_bck_ps:1;

	// h0020, bit: 13
	/* reg_misc_i2s_tx_sdo_drv*/
	#define offset_of_padtop1_reg_misc_i2s_tx_sdo_drv (64)
	#define mask_of_padtop1_reg_misc_i2s_tx_sdo_drv (0x2000)
	unsigned int reg_misc_i2s_tx_sdo_drv:1;

	// h0020, bit: 14
	/* reg_misc_i2s_tx_sdo_ie*/
	#define offset_of_padtop1_reg_misc_i2s_tx_sdo_ie (64)
	#define mask_of_padtop1_reg_misc_i2s_tx_sdo_ie (0x4000)
	unsigned int reg_misc_i2s_tx_sdo_ie:1;

	// h0020, bit: 15
	/* reg_misc_i2s_tx_sdo_pe*/
	#define offset_of_padtop1_reg_misc_i2s_tx_sdo_pe (64)
	#define mask_of_padtop1_reg_misc_i2s_tx_sdo_pe (0x8000)
	unsigned int reg_misc_i2s_tx_sdo_pe:1;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 0
	/* reg_misc_i2s_tx_sdo_ps*/
	#define offset_of_padtop1_reg_misc_i2s_tx_sdo_ps (66)
	#define mask_of_padtop1_reg_misc_i2s_tx_sdo_ps (0x1)
	unsigned int reg_misc_i2s_tx_sdo_ps:1;

	// h0021, bit: 1
	/* reg_misc_i2s_tx_wck_drv*/
	#define offset_of_padtop1_reg_misc_i2s_tx_wck_drv (66)
	#define mask_of_padtop1_reg_misc_i2s_tx_wck_drv (0x2)
	unsigned int reg_misc_i2s_tx_wck_drv:1;

	// h0021, bit: 2
	/* reg_misc_i2s_tx_wck_ie*/
	#define offset_of_padtop1_reg_misc_i2s_tx_wck_ie (66)
	#define mask_of_padtop1_reg_misc_i2s_tx_wck_ie (0x4)
	unsigned int reg_misc_i2s_tx_wck_ie:1;

	// h0021, bit: 3
	/* reg_misc_i2s_tx_wck_pe*/
	#define offset_of_padtop1_reg_misc_i2s_tx_wck_pe (66)
	#define mask_of_padtop1_reg_misc_i2s_tx_wck_pe (0x8)
	unsigned int reg_misc_i2s_tx_wck_pe:1;

	// h0021, bit: 4
	/* reg_misc_i2s_tx_wck_ps*/
	#define offset_of_padtop1_reg_misc_i2s_tx_wck_ps (66)
	#define mask_of_padtop1_reg_misc_i2s_tx_wck_ps (0x10)
	unsigned int reg_misc_i2s_tx_wck_ps:1;

	// h0021, bit: 5
	/* reg_nand_ale_drv*/
	#define offset_of_padtop1_reg_nand_ale_drv (66)
	#define mask_of_padtop1_reg_nand_ale_drv (0x20)
	unsigned int reg_nand_ale_drv:1;

	// h0021, bit: 6
	/* reg_nand_ale_pe*/
	#define offset_of_padtop1_reg_nand_ale_pe (66)
	#define mask_of_padtop1_reg_nand_ale_pe (0x40)
	unsigned int reg_nand_ale_pe:1;

	// h0021, bit: 7
	/* reg_nand_ale_ps*/
	#define offset_of_padtop1_reg_nand_ale_ps (66)
	#define mask_of_padtop1_reg_nand_ale_ps (0x80)
	unsigned int reg_nand_ale_ps:1;

	// h0021, bit: 8
	/* reg_nand_cez0_drv*/
	#define offset_of_padtop1_reg_nand_cez0_drv (66)
	#define mask_of_padtop1_reg_nand_cez0_drv (0x100)
	unsigned int reg_nand_cez0_drv:1;

	// h0021, bit: 9
	/* reg_nand_cez0_pe*/
	#define offset_of_padtop1_reg_nand_cez0_pe (66)
	#define mask_of_padtop1_reg_nand_cez0_pe (0x200)
	unsigned int reg_nand_cez0_pe:1;

	// h0021, bit: 10
	/* reg_nand_cez0_ps*/
	#define offset_of_padtop1_reg_nand_cez0_ps (66)
	#define mask_of_padtop1_reg_nand_cez0_ps (0x400)
	unsigned int reg_nand_cez0_ps:1;

	// h0021, bit: 11
	/* reg_nand_cez1_drv*/
	#define offset_of_padtop1_reg_nand_cez1_drv (66)
	#define mask_of_padtop1_reg_nand_cez1_drv (0x800)
	unsigned int reg_nand_cez1_drv:1;

	// h0021, bit: 12
	/* reg_nand_cez1_pe*/
	#define offset_of_padtop1_reg_nand_cez1_pe (66)
	#define mask_of_padtop1_reg_nand_cez1_pe (0x1000)
	unsigned int reg_nand_cez1_pe:1;

	// h0021, bit: 13
	/* reg_nand_cez1_ps*/
	#define offset_of_padtop1_reg_nand_cez1_ps (66)
	#define mask_of_padtop1_reg_nand_cez1_ps (0x2000)
	unsigned int reg_nand_cez1_ps:1;

	// h0021, bit: 14
	/* reg_nand_cle_drv0*/
	#define offset_of_padtop1_reg_nand_cle_drv0 (66)
	#define mask_of_padtop1_reg_nand_cle_drv0 (0x4000)
	unsigned int reg_nand_cle_drv0:1;

	// h0021, bit: 15
	/* reg_nand_cle_drv1*/
	#define offset_of_padtop1_reg_nand_cle_drv1 (66)
	#define mask_of_padtop1_reg_nand_cle_drv1 (0x8000)
	unsigned int reg_nand_cle_drv1:1;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 0
	/* reg_nand_cle_pe*/
	#define offset_of_padtop1_reg_nand_cle_pe (68)
	#define mask_of_padtop1_reg_nand_cle_pe (0x1)
	unsigned int reg_nand_cle_pe:1;

	// h0022, bit: 1
	/* reg_nand_cle_ps*/
	#define offset_of_padtop1_reg_nand_cle_ps (68)
	#define mask_of_padtop1_reg_nand_cle_ps (0x2)
	unsigned int reg_nand_cle_ps:1;

	// h0022, bit: 2
	/* reg_nand_da0_drv*/
	#define offset_of_padtop1_reg_nand_da0_drv (68)
	#define mask_of_padtop1_reg_nand_da0_drv (0x4)
	unsigned int reg_nand_da0_drv:1;

	// h0022, bit: 3
	/* reg_nand_da0_pe*/
	#define offset_of_padtop1_reg_nand_da0_pe (68)
	#define mask_of_padtop1_reg_nand_da0_pe (0x8)
	unsigned int reg_nand_da0_pe:1;

	// h0022, bit: 4
	/* reg_nand_da0_ps*/
	#define offset_of_padtop1_reg_nand_da0_ps (68)
	#define mask_of_padtop1_reg_nand_da0_ps (0x10)
	unsigned int reg_nand_da0_ps:1;

	// h0022, bit: 5
	/* reg_nand_da1_drv*/
	#define offset_of_padtop1_reg_nand_da1_drv (68)
	#define mask_of_padtop1_reg_nand_da1_drv (0x20)
	unsigned int reg_nand_da1_drv:1;

	// h0022, bit: 6
	/* reg_nand_da1_pe*/
	#define offset_of_padtop1_reg_nand_da1_pe (68)
	#define mask_of_padtop1_reg_nand_da1_pe (0x40)
	unsigned int reg_nand_da1_pe:1;

	// h0022, bit: 7
	/* reg_nand_da1_ps*/
	#define offset_of_padtop1_reg_nand_da1_ps (68)
	#define mask_of_padtop1_reg_nand_da1_ps (0x80)
	unsigned int reg_nand_da1_ps:1;

	// h0022, bit: 8
	/* reg_nand_da2_drv*/
	#define offset_of_padtop1_reg_nand_da2_drv (68)
	#define mask_of_padtop1_reg_nand_da2_drv (0x100)
	unsigned int reg_nand_da2_drv:1;

	// h0022, bit: 9
	/* reg_nand_da2_pe*/
	#define offset_of_padtop1_reg_nand_da2_pe (68)
	#define mask_of_padtop1_reg_nand_da2_pe (0x200)
	unsigned int reg_nand_da2_pe:1;

	// h0022, bit: 10
	/* reg_nand_da2_ps*/
	#define offset_of_padtop1_reg_nand_da2_ps (68)
	#define mask_of_padtop1_reg_nand_da2_ps (0x400)
	unsigned int reg_nand_da2_ps:1;

	// h0022, bit: 11
	/* reg_nand_da3_drv*/
	#define offset_of_padtop1_reg_nand_da3_drv (68)
	#define mask_of_padtop1_reg_nand_da3_drv (0x800)
	unsigned int reg_nand_da3_drv:1;

	// h0022, bit: 12
	/* reg_nand_da3_pe*/
	#define offset_of_padtop1_reg_nand_da3_pe (68)
	#define mask_of_padtop1_reg_nand_da3_pe (0x1000)
	unsigned int reg_nand_da3_pe:1;

	// h0022, bit: 13
	/* reg_nand_da3_ps*/
	#define offset_of_padtop1_reg_nand_da3_ps (68)
	#define mask_of_padtop1_reg_nand_da3_ps (0x2000)
	unsigned int reg_nand_da3_ps:1;

	// h0022, bit: 14
	/* reg_nand_da4_drv*/
	#define offset_of_padtop1_reg_nand_da4_drv (68)
	#define mask_of_padtop1_reg_nand_da4_drv (0x4000)
	unsigned int reg_nand_da4_drv:1;

	// h0022, bit: 15
	/* reg_nand_da4_pe*/
	#define offset_of_padtop1_reg_nand_da4_pe (68)
	#define mask_of_padtop1_reg_nand_da4_pe (0x8000)
	unsigned int reg_nand_da4_pe:1;

	// h0022
	unsigned int /* padding 16 bit */:16;

	// h0023, bit: 0
	/* reg_nand_da4_ps*/
	#define offset_of_padtop1_reg_nand_da4_ps (70)
	#define mask_of_padtop1_reg_nand_da4_ps (0x1)
	unsigned int reg_nand_da4_ps:1;

	// h0023, bit: 1
	/* reg_nand_da5_drv*/
	#define offset_of_padtop1_reg_nand_da5_drv (70)
	#define mask_of_padtop1_reg_nand_da5_drv (0x2)
	unsigned int reg_nand_da5_drv:1;

	// h0023, bit: 2
	/* reg_nand_da5_pe*/
	#define offset_of_padtop1_reg_nand_da5_pe (70)
	#define mask_of_padtop1_reg_nand_da5_pe (0x4)
	unsigned int reg_nand_da5_pe:1;

	// h0023, bit: 3
	/* reg_nand_da5_ps*/
	#define offset_of_padtop1_reg_nand_da5_ps (70)
	#define mask_of_padtop1_reg_nand_da5_ps (0x8)
	unsigned int reg_nand_da5_ps:1;

	// h0023, bit: 4
	/* reg_nand_da6_drv*/
	#define offset_of_padtop1_reg_nand_da6_drv (70)
	#define mask_of_padtop1_reg_nand_da6_drv (0x10)
	unsigned int reg_nand_da6_drv:1;

	// h0023, bit: 5
	/* reg_nand_da6_pe*/
	#define offset_of_padtop1_reg_nand_da6_pe (70)
	#define mask_of_padtop1_reg_nand_da6_pe (0x20)
	unsigned int reg_nand_da6_pe:1;

	// h0023, bit: 6
	/* reg_nand_da6_ps*/
	#define offset_of_padtop1_reg_nand_da6_ps (70)
	#define mask_of_padtop1_reg_nand_da6_ps (0x40)
	unsigned int reg_nand_da6_ps:1;

	// h0023, bit: 7
	/* reg_nand_da7_drv0*/
	#define offset_of_padtop1_reg_nand_da7_drv0 (70)
	#define mask_of_padtop1_reg_nand_da7_drv0 (0x80)
	unsigned int reg_nand_da7_drv0:1;

	// h0023, bit: 8
	/* reg_nand_da7_drv1*/
	#define offset_of_padtop1_reg_nand_da7_drv1 (70)
	#define mask_of_padtop1_reg_nand_da7_drv1 (0x100)
	unsigned int reg_nand_da7_drv1:1;

	// h0023, bit: 9
	/* reg_nand_da7_pe*/
	#define offset_of_padtop1_reg_nand_da7_pe (70)
	#define mask_of_padtop1_reg_nand_da7_pe (0x200)
	unsigned int reg_nand_da7_pe:1;

	// h0023, bit: 10
	/* reg_nand_dqs_drv0*/
	#define offset_of_padtop1_reg_nand_dqs_drv0 (70)
	#define mask_of_padtop1_reg_nand_dqs_drv0 (0x400)
	unsigned int reg_nand_dqs_drv0:1;

	// h0023, bit: 11
	/* reg_nand_dqs_drv1*/
	#define offset_of_padtop1_reg_nand_dqs_drv1 (70)
	#define mask_of_padtop1_reg_nand_dqs_drv1 (0x800)
	unsigned int reg_nand_dqs_drv1:1;

	// h0023, bit: 12
	/* reg_nand_dqs_pe*/
	#define offset_of_padtop1_reg_nand_dqs_pe (70)
	#define mask_of_padtop1_reg_nand_dqs_pe (0x1000)
	unsigned int reg_nand_dqs_pe:1;

	// h0023, bit: 13
	/* reg_nand_dqs_ps*/
	#define offset_of_padtop1_reg_nand_dqs_ps (70)
	#define mask_of_padtop1_reg_nand_dqs_ps (0x2000)
	unsigned int reg_nand_dqs_ps:1;

	// h0023, bit: 14
	/* reg_nand_rbz_drv*/
	#define offset_of_padtop1_reg_nand_rbz_drv (70)
	#define mask_of_padtop1_reg_nand_rbz_drv (0x4000)
	unsigned int reg_nand_rbz_drv:1;

	// h0023, bit: 15
	/* reg_nand_rbz_pe*/
	#define offset_of_padtop1_reg_nand_rbz_pe (70)
	#define mask_of_padtop1_reg_nand_rbz_pe (0x8000)
	unsigned int reg_nand_rbz_pe:1;

	// h0023
	unsigned int /* padding 16 bit */:16;

	// h0024, bit: 0
	/* reg_nand_rbz_ps*/
	#define offset_of_padtop1_reg_nand_rbz_ps (72)
	#define mask_of_padtop1_reg_nand_rbz_ps (0x1)
	unsigned int reg_nand_rbz_ps:1;

	// h0024, bit: 1
	/* reg_nand_rez_drv*/
	#define offset_of_padtop1_reg_nand_rez_drv (72)
	#define mask_of_padtop1_reg_nand_rez_drv (0x2)
	unsigned int reg_nand_rez_drv:1;

	// h0024, bit: 2
	/* reg_nand_rez_pe*/
	#define offset_of_padtop1_reg_nand_rez_pe (72)
	#define mask_of_padtop1_reg_nand_rez_pe (0x4)
	unsigned int reg_nand_rez_pe:1;

	// h0024, bit: 3
	/* reg_nand_rez_ps*/
	#define offset_of_padtop1_reg_nand_rez_ps (72)
	#define mask_of_padtop1_reg_nand_rez_ps (0x8)
	unsigned int reg_nand_rez_ps:1;

	// h0024, bit: 4
	/* reg_nand_wez_drv*/
	#define offset_of_padtop1_reg_nand_wez_drv (72)
	#define mask_of_padtop1_reg_nand_wez_drv (0x10)
	unsigned int reg_nand_wez_drv:1;

	// h0024, bit: 5
	/* reg_nand_wez_pe*/
	#define offset_of_padtop1_reg_nand_wez_pe (72)
	#define mask_of_padtop1_reg_nand_wez_pe (0x20)
	unsigned int reg_nand_wez_pe:1;

	// h0024, bit: 6
	/* reg_nand_wez_ps*/
	#define offset_of_padtop1_reg_nand_wez_ps (72)
	#define mask_of_padtop1_reg_nand_wez_ps (0x40)
	unsigned int reg_nand_wez_ps:1;

	// h0024, bit: 7
	/* reg_nand_wpz_drv*/
	#define offset_of_padtop1_reg_nand_wpz_drv (72)
	#define mask_of_padtop1_reg_nand_wpz_drv (0x80)
	unsigned int reg_nand_wpz_drv:1;

	// h0024, bit: 8
	/* reg_nand_wpz_pe*/
	#define offset_of_padtop1_reg_nand_wpz_pe (72)
	#define mask_of_padtop1_reg_nand_wpz_pe (0x100)
	unsigned int reg_nand_wpz_pe:1;

	// h0024, bit: 9
	/* reg_nand_wpz_ps*/
	#define offset_of_padtop1_reg_nand_wpz_ps (72)
	#define mask_of_padtop1_reg_nand_wpz_ps (0x200)
	unsigned int reg_nand_wpz_ps:1;

	// h0024, bit: 10
	/* reg_pwm0_drv*/
	#define offset_of_padtop1_reg_pwm0_drv (72)
	#define mask_of_padtop1_reg_pwm0_drv (0x400)
	unsigned int reg_pwm0_drv:1;

	// h0024, bit: 11
	/* reg_pwm1_drv*/
	#define offset_of_padtop1_reg_pwm1_drv (72)
	#define mask_of_padtop1_reg_pwm1_drv (0x800)
	unsigned int reg_pwm1_drv:1;

	// h0024, bit: 12
	/* reg_sd_clk_drv*/
	#define offset_of_padtop1_reg_sd_clk_drv (72)
	#define mask_of_padtop1_reg_sd_clk_drv (0x1000)
	unsigned int reg_sd_clk_drv:1;

	// h0024, bit: 13
	/* reg_sd_cmd_drv*/
	#define offset_of_padtop1_reg_sd_cmd_drv (72)
	#define mask_of_padtop1_reg_sd_cmd_drv (0x2000)
	unsigned int reg_sd_cmd_drv:1;

	// h0024, bit: 14
	/* reg_sd_d0_drv*/
	#define offset_of_padtop1_reg_sd_d0_drv (72)
	#define mask_of_padtop1_reg_sd_d0_drv (0x4000)
	unsigned int reg_sd_d0_drv:1;

	// h0024, bit: 15
	/* reg_sd_d1_drv*/
	#define offset_of_padtop1_reg_sd_d1_drv (72)
	#define mask_of_padtop1_reg_sd_d1_drv (0x8000)
	unsigned int reg_sd_d1_drv:1;

	// h0024
	unsigned int /* padding 16 bit */:16;

	// h0025, bit: 0
	/* reg_sd_d2_drv*/
	#define offset_of_padtop1_reg_sd_d2_drv (74)
	#define mask_of_padtop1_reg_sd_d2_drv (0x1)
	unsigned int reg_sd_d2_drv:1;

	// h0025, bit: 1
	/* reg_sd_d3_drv*/
	#define offset_of_padtop1_reg_sd_d3_drv (74)
	#define mask_of_padtop1_reg_sd_d3_drv (0x2)
	unsigned int reg_sd_d3_drv:1;

	// h0025, bit: 2
	/* reg_snr0_gpio3_lsh18_en*/
	#define offset_of_padtop1_reg_snr0_gpio3_lsh18_en (74)
	#define mask_of_padtop1_reg_snr0_gpio3_lsh18_en (0x4)
	unsigned int reg_snr0_gpio3_lsh18_en:1;

	// h0025, bit: 3
	/* reg_snr0_gpio4_lsh18_en*/
	#define offset_of_padtop1_reg_snr0_gpio4_lsh18_en (74)
	#define mask_of_padtop1_reg_snr0_gpio4_lsh18_en (0x8)
	unsigned int reg_snr0_gpio4_lsh18_en:1;

	// h0025, bit: 4
	/* reg_snr0_gpio5_lsh18_en*/
	#define offset_of_padtop1_reg_snr0_gpio5_lsh18_en (74)
	#define mask_of_padtop1_reg_snr0_gpio5_lsh18_en (0x10)
	unsigned int reg_snr0_gpio5_lsh18_en:1;

	// h0025, bit: 5
	/* reg_snr0_gpio6_lsh18_en*/
	#define offset_of_padtop1_reg_snr0_gpio6_lsh18_en (74)
	#define mask_of_padtop1_reg_snr0_gpio6_lsh18_en (0x20)
	unsigned int reg_snr0_gpio6_lsh18_en:1;

	// h0025, bit: 6
	/* reg_snr0_gpio7_lsh18_en*/
	#define offset_of_padtop1_reg_snr0_gpio7_lsh18_en (74)
	#define mask_of_padtop1_reg_snr0_gpio7_lsh18_en (0x40)
	unsigned int reg_snr0_gpio7_lsh18_en:1;

	// h0025, bit: 14
	/* */
	unsigned int :9;

	// h0025
	unsigned int /* padding 16 bit */:16;

	// h0026, bit: 14
	/* */
	unsigned int :16;

	// h0026
	unsigned int /* padding 16 bit */:16;

	// h0027, bit: 9
	/* reg_snr0_d_drv*/
	#define offset_of_padtop1_reg_snr0_d_drv (78)
	#define mask_of_padtop1_reg_snr0_d_drv (0x3ff)
	unsigned int reg_snr0_d_drv:10;

	// h0027, bit: 14
	/* */
	unsigned int :6;

	// h0027
	unsigned int /* padding 16 bit */:16;

	// h0028, bit: 9
	/* reg_snr0_d_ie*/
	#define offset_of_padtop1_reg_snr0_d_ie (80)
	#define mask_of_padtop1_reg_snr0_d_ie (0x3ff)
	unsigned int reg_snr0_d_ie:10;

	// h0028, bit: 14
	/* */
	unsigned int :6;

	// h0028
	unsigned int /* padding 16 bit */:16;

	// h0029, bit: 9
	/* reg_snr0_d_pe*/
	#define offset_of_padtop1_reg_snr0_d_pe (82)
	#define mask_of_padtop1_reg_snr0_d_pe (0x3ff)
	unsigned int reg_snr0_d_pe:10;

	// h0029, bit: 14
	/* */
	unsigned int :6;

	// h0029
	unsigned int /* padding 16 bit */:16;

	// h002a, bit: 9
	/* reg_snr0_d_ps*/
	#define offset_of_padtop1_reg_snr0_d_ps (84)
	#define mask_of_padtop1_reg_snr0_d_ps (0x3ff)
	unsigned int reg_snr0_d_ps:10;

	// h002a, bit: 14
	/* */
	unsigned int :6;

	// h002a
	unsigned int /* padding 16 bit */:16;

	// h002b, bit: 7
	/* reg_snr0_gpio_drv*/
	#define offset_of_padtop1_reg_snr0_gpio_drv (86)
	#define mask_of_padtop1_reg_snr0_gpio_drv (0xff)
	unsigned int reg_snr0_gpio_drv:8;

	// h002b, bit: 14
	/* reg_snr0_gpio_ie*/
	#define offset_of_padtop1_reg_snr0_gpio_ie (86)
	#define mask_of_padtop1_reg_snr0_gpio_ie (0xff00)
	unsigned int reg_snr0_gpio_ie:8;

	// h002b
	unsigned int /* padding 16 bit */:16;

	// h002c, bit: 7
	/* reg_snr0_gpio_pe*/
	#define offset_of_padtop1_reg_snr0_gpio_pe (88)
	#define mask_of_padtop1_reg_snr0_gpio_pe (0xff)
	unsigned int reg_snr0_gpio_pe:8;

	// h002c, bit: 14
	/* reg_snr0_gpio_ps*/
	#define offset_of_padtop1_reg_snr0_gpio_ps (88)
	#define mask_of_padtop1_reg_snr0_gpio_ps (0xff00)
	unsigned int reg_snr0_gpio_ps:8;

	// h002c
	unsigned int /* padding 16 bit */:16;

	// h002d, bit: 9
	/* reg_snr1_d_drv*/
	#define offset_of_padtop1_reg_snr1_d_drv (90)
	#define mask_of_padtop1_reg_snr1_d_drv (0x3ff)
	unsigned int reg_snr1_d_drv:10;

	// h002d, bit: 14
	/* */
	unsigned int :6;

	// h002d
	unsigned int /* padding 16 bit */:16;

	// h002e, bit: 9
	/* reg_snr1_d_ie*/
	#define offset_of_padtop1_reg_snr1_d_ie (92)
	#define mask_of_padtop1_reg_snr1_d_ie (0x3ff)
	unsigned int reg_snr1_d_ie:10;

	// h002e, bit: 14
	/* */
	unsigned int :6;

	// h002e
	unsigned int /* padding 16 bit */:16;

	// h002f, bit: 9
	/* reg_snr1_d_pe*/
	#define offset_of_padtop1_reg_snr1_d_pe (94)
	#define mask_of_padtop1_reg_snr1_d_pe (0x3ff)
	unsigned int reg_snr1_d_pe:10;

	// h002f, bit: 14
	/* */
	unsigned int :6;

	// h002f
	unsigned int /* padding 16 bit */:16;

	// h0030, bit: 9
	/* reg_snr1_d_ps*/
	#define offset_of_padtop1_reg_snr1_d_ps (96)
	#define mask_of_padtop1_reg_snr1_d_ps (0x3ff)
	unsigned int reg_snr1_d_ps:10;

	// h0030, bit: 14
	/* */
	unsigned int :6;

	// h0030
	unsigned int /* padding 16 bit */:16;

	// h0031, bit: 7
	/* reg_snr1_gpio_drv*/
	#define offset_of_padtop1_reg_snr1_gpio_drv (98)
	#define mask_of_padtop1_reg_snr1_gpio_drv (0xff)
	unsigned int reg_snr1_gpio_drv:8;

	// h0031, bit: 8
	/* reg_snr1_gpio_ie*/
	#define offset_of_padtop1_reg_snr1_gpio_ie (98)
	#define mask_of_padtop1_reg_snr1_gpio_ie (0x100)
	unsigned int reg_snr1_gpio_ie:1;

	// h0031, bit: 14
	/* */
	unsigned int :7;

	// h0031
	unsigned int /* padding 16 bit */:16;

	// h0032, bit: 7
	/* reg_snr1_gpio_pe*/
	#define offset_of_padtop1_reg_snr1_gpio_pe (100)
	#define mask_of_padtop1_reg_snr1_gpio_pe (0xff)
	unsigned int reg_snr1_gpio_pe:8;

	// h0032, bit: 14
	/* reg_snr1_gpio_ps*/
	#define offset_of_padtop1_reg_snr1_gpio_ps (100)
	#define mask_of_padtop1_reg_snr1_gpio_ps (0xff00)
	unsigned int reg_snr1_gpio_ps:8;

	// h0032
	unsigned int /* padding 16 bit */:16;

	// h0033, bit: 9
	/* reg_snr2_d_drv*/
	#define offset_of_padtop1_reg_snr2_d_drv (102)
	#define mask_of_padtop1_reg_snr2_d_drv (0x3ff)
	unsigned int reg_snr2_d_drv:10;

	// h0033, bit: 14
	/* */
	unsigned int :6;

	// h0033
	unsigned int /* padding 16 bit */:16;

	// h0034, bit: 9
	/* reg_snr2_d_ie*/
	#define offset_of_padtop1_reg_snr2_d_ie (104)
	#define mask_of_padtop1_reg_snr2_d_ie (0x3ff)
	unsigned int reg_snr2_d_ie:10;

	// h0034, bit: 14
	/* */
	unsigned int :6;

	// h0034
	unsigned int /* padding 16 bit */:16;

	// h0035, bit: 9
	/* reg_snr2_d_pe*/
	#define offset_of_padtop1_reg_snr2_d_pe (106)
	#define mask_of_padtop1_reg_snr2_d_pe (0x3ff)
	unsigned int reg_snr2_d_pe:10;

	// h0035, bit: 14
	/* */
	unsigned int :6;

	// h0035
	unsigned int /* padding 16 bit */:16;

	// h0036, bit: 9
	/* reg_snr2_d_ps*/
	#define offset_of_padtop1_reg_snr2_d_ps (108)
	#define mask_of_padtop1_reg_snr2_d_ps (0x3ff)
	unsigned int reg_snr2_d_ps:10;

	// h0036, bit: 11
	/* */
	unsigned int :2;

	// h0036, bit: 12
	/* reg_snr2_gpio5_lsh18_en*/
	#define offset_of_padtop1_reg_snr2_gpio5_lsh18_en (108)
	#define mask_of_padtop1_reg_snr2_gpio5_lsh18_en (0x1000)
	unsigned int reg_snr2_gpio5_lsh18_en:1;

	// h0036, bit: 14
	/* */
	unsigned int :3;

	// h0036
	unsigned int /* padding 16 bit */:16;

	// h0037, bit: 7
	/* reg_snr2_gpio_drv*/
	#define offset_of_padtop1_reg_snr2_gpio_drv (110)
	#define mask_of_padtop1_reg_snr2_gpio_drv (0xff)
	unsigned int reg_snr2_gpio_drv:8;

	// h0037, bit: 14
	/* reg_snr2_gpio_ie*/
	#define offset_of_padtop1_reg_snr2_gpio_ie (110)
	#define mask_of_padtop1_reg_snr2_gpio_ie (0xff00)
	unsigned int reg_snr2_gpio_ie:8;

	// h0037
	unsigned int /* padding 16 bit */:16;

	// h0038, bit: 7
	/* reg_snr2_gpio_pe*/
	#define offset_of_padtop1_reg_snr2_gpio_pe (112)
	#define mask_of_padtop1_reg_snr2_gpio_pe (0xff)
	unsigned int reg_snr2_gpio_pe:8;

	// h0038, bit: 14
	/* reg_snr2_gpio_ps*/
	#define offset_of_padtop1_reg_snr2_gpio_ps (112)
	#define mask_of_padtop1_reg_snr2_gpio_ps (0xff00)
	unsigned int reg_snr2_gpio_ps:8;

	// h0038
	unsigned int /* padding 16 bit */:16;

	// h0039, bit: 9
	/* reg_snr3_d_drv*/
	#define offset_of_padtop1_reg_snr3_d_drv (114)
	#define mask_of_padtop1_reg_snr3_d_drv (0x3ff)
	unsigned int reg_snr3_d_drv:10;

	// h0039, bit: 14
	/* */
	unsigned int :6;

	// h0039
	unsigned int /* padding 16 bit */:16;

	// h003a, bit: 9
	/* reg_snr3_d_ie*/
	#define offset_of_padtop1_reg_snr3_d_ie (116)
	#define mask_of_padtop1_reg_snr3_d_ie (0x3ff)
	unsigned int reg_snr3_d_ie:10;

	// h003a, bit: 14
	/* */
	unsigned int :6;

	// h003a
	unsigned int /* padding 16 bit */:16;

	// h003b, bit: 9
	/* reg_snr3_d_pe*/
	#define offset_of_padtop1_reg_snr3_d_pe (118)
	#define mask_of_padtop1_reg_snr3_d_pe (0x3ff)
	unsigned int reg_snr3_d_pe:10;

	// h003b, bit: 14
	/* */
	unsigned int :6;

	// h003b
	unsigned int /* padding 16 bit */:16;

	// h003c, bit: 9
	/* reg_snr3_d_ps*/
	#define offset_of_padtop1_reg_snr3_d_ps (120)
	#define mask_of_padtop1_reg_snr3_d_ps (0x3ff)
	unsigned int reg_snr3_d_ps:10;

	// h003c, bit: 14
	/* */
	unsigned int :6;

	// h003c
	unsigned int /* padding 16 bit */:16;

	// h003d, bit: 7
	/* reg_snr3_gpio_drv*/
	#define offset_of_padtop1_reg_snr3_gpio_drv (122)
	#define mask_of_padtop1_reg_snr3_gpio_drv (0xff)
	unsigned int reg_snr3_gpio_drv:8;

	// h003d, bit: 14
	/* reg_snr3_gpio_ie*/
	#define offset_of_padtop1_reg_snr3_gpio_ie (122)
	#define mask_of_padtop1_reg_snr3_gpio_ie (0xff00)
	unsigned int reg_snr3_gpio_ie:8;

	// h003d
	unsigned int /* padding 16 bit */:16;

	// h003e, bit: 7
	/* reg_snr3_gpio_pe*/
	#define offset_of_padtop1_reg_snr3_gpio_pe (124)
	#define mask_of_padtop1_reg_snr3_gpio_pe (0xff)
	unsigned int reg_snr3_gpio_pe:8;

	// h003e, bit: 14
	/* reg_snr3_gpio_ps*/
	#define offset_of_padtop1_reg_snr3_gpio_ps (124)
	#define mask_of_padtop1_reg_snr3_gpio_ps (0xff00)
	unsigned int reg_snr3_gpio_ps:8;

	// h003e
	unsigned int /* padding 16 bit */:16;

	// h003f, bit: 0
	/* reg_spdif_out_drv*/
	#define offset_of_padtop1_reg_spdif_out_drv (126)
	#define mask_of_padtop1_reg_spdif_out_drv (0x1)
	unsigned int reg_spdif_out_drv:1;

	// h003f, bit: 1
	/* reg_spi0_ck_drv*/
	#define offset_of_padtop1_reg_spi0_ck_drv (126)
	#define mask_of_padtop1_reg_spi0_ck_drv (0x2)
	unsigned int reg_spi0_ck_drv:1;

	// h003f, bit: 2
	/* reg_spi0_cz0_drv*/
	#define offset_of_padtop1_reg_spi0_cz0_drv (126)
	#define mask_of_padtop1_reg_spi0_cz0_drv (0x4)
	unsigned int reg_spi0_cz0_drv:1;

	// h003f, bit: 3
	/* reg_spi0_di_drv*/
	#define offset_of_padtop1_reg_spi0_di_drv (126)
	#define mask_of_padtop1_reg_spi0_di_drv (0x8)
	unsigned int reg_spi0_di_drv:1;

	// h003f, bit: 4
	/* reg_spi0_do_drv*/
	#define offset_of_padtop1_reg_spi0_do_drv (126)
	#define mask_of_padtop1_reg_spi0_do_drv (0x10)
	unsigned int reg_spi0_do_drv:1;

	// h003f, bit: 5
	/* reg_spi1_ck_drv*/
	#define offset_of_padtop1_reg_spi1_ck_drv (126)
	#define mask_of_padtop1_reg_spi1_ck_drv (0x20)
	unsigned int reg_spi1_ck_drv:1;

	// h003f, bit: 6
	/* reg_spi1_ck_ie*/
	#define offset_of_padtop1_reg_spi1_ck_ie (126)
	#define mask_of_padtop1_reg_spi1_ck_ie (0x40)
	unsigned int reg_spi1_ck_ie:1;

	// h003f, bit: 7
	/* reg_spi1_ck_pe*/
	#define offset_of_padtop1_reg_spi1_ck_pe (126)
	#define mask_of_padtop1_reg_spi1_ck_pe (0x80)
	unsigned int reg_spi1_ck_pe:1;

	// h003f, bit: 8
	/* reg_spi1_ck_ps*/
	#define offset_of_padtop1_reg_spi1_ck_ps (126)
	#define mask_of_padtop1_reg_spi1_ck_ps (0x100)
	unsigned int reg_spi1_ck_ps:1;

	// h003f, bit: 9
	/* reg_spi1_cz0_drv*/
	#define offset_of_padtop1_reg_spi1_cz0_drv (126)
	#define mask_of_padtop1_reg_spi1_cz0_drv (0x200)
	unsigned int reg_spi1_cz0_drv:1;

	// h003f, bit: 10
	/* reg_spi1_cz0_ie*/
	#define offset_of_padtop1_reg_spi1_cz0_ie (126)
	#define mask_of_padtop1_reg_spi1_cz0_ie (0x400)
	unsigned int reg_spi1_cz0_ie:1;

	// h003f, bit: 11
	/* reg_spi1_cz0_pe*/
	#define offset_of_padtop1_reg_spi1_cz0_pe (126)
	#define mask_of_padtop1_reg_spi1_cz0_pe (0x800)
	unsigned int reg_spi1_cz0_pe:1;

	// h003f, bit: 12
	/* reg_spi1_cz0_ps*/
	#define offset_of_padtop1_reg_spi1_cz0_ps (126)
	#define mask_of_padtop1_reg_spi1_cz0_ps (0x1000)
	unsigned int reg_spi1_cz0_ps:1;

	// h003f, bit: 13
	/* reg_spi1_di_drv*/
	#define offset_of_padtop1_reg_spi1_di_drv (126)
	#define mask_of_padtop1_reg_spi1_di_drv (0x2000)
	unsigned int reg_spi1_di_drv:1;

	// h003f, bit: 14
	/* reg_spi1_di_ie*/
	#define offset_of_padtop1_reg_spi1_di_ie (126)
	#define mask_of_padtop1_reg_spi1_di_ie (0x4000)
	unsigned int reg_spi1_di_ie:1;

	// h003f, bit: 15
	/* reg_spi1_di_pe*/
	#define offset_of_padtop1_reg_spi1_di_pe (126)
	#define mask_of_padtop1_reg_spi1_di_pe (0x8000)
	unsigned int reg_spi1_di_pe:1;

	// h003f
	unsigned int /* padding 16 bit */:16;

	// h0040, bit: 0
	/* reg_spi1_di_ps*/
	#define offset_of_padtop1_reg_spi1_di_ps (128)
	#define mask_of_padtop1_reg_spi1_di_ps (0x1)
	unsigned int reg_spi1_di_ps:1;

	// h0040, bit: 1
	/* reg_spi1_do_drv*/
	#define offset_of_padtop1_reg_spi1_do_drv (128)
	#define mask_of_padtop1_reg_spi1_do_drv (0x2)
	unsigned int reg_spi1_do_drv:1;

	// h0040, bit: 2
	/* reg_spi1_do_ie*/
	#define offset_of_padtop1_reg_spi1_do_ie (128)
	#define mask_of_padtop1_reg_spi1_do_ie (0x4)
	unsigned int reg_spi1_do_ie:1;

	// h0040, bit: 3
	/* reg_spi1_do_pe*/
	#define offset_of_padtop1_reg_spi1_do_pe (128)
	#define mask_of_padtop1_reg_spi1_do_pe (0x8)
	unsigned int reg_spi1_do_pe:1;

	// h0040, bit: 4
	/* reg_spi1_do_ps*/
	#define offset_of_padtop1_reg_spi1_do_ps (128)
	#define mask_of_padtop1_reg_spi1_do_ps (0x10)
	unsigned int reg_spi1_do_ps:1;

	// h0040, bit: 5
	/* reg_spi2_ck_drv*/
	#define offset_of_padtop1_reg_spi2_ck_drv (128)
	#define mask_of_padtop1_reg_spi2_ck_drv (0x20)
	unsigned int reg_spi2_ck_drv:1;

	// h0040, bit: 6
	/* reg_spi2_ck_ie*/
	#define offset_of_padtop1_reg_spi2_ck_ie (128)
	#define mask_of_padtop1_reg_spi2_ck_ie (0x40)
	unsigned int reg_spi2_ck_ie:1;

	// h0040, bit: 7
	/* reg_spi2_ck_pe*/
	#define offset_of_padtop1_reg_spi2_ck_pe (128)
	#define mask_of_padtop1_reg_spi2_ck_pe (0x80)
	unsigned int reg_spi2_ck_pe:1;

	// h0040, bit: 8
	/* reg_spi2_ck_ps*/
	#define offset_of_padtop1_reg_spi2_ck_ps (128)
	#define mask_of_padtop1_reg_spi2_ck_ps (0x100)
	unsigned int reg_spi2_ck_ps:1;

	// h0040, bit: 9
	/* reg_spi2_cz0_drv*/
	#define offset_of_padtop1_reg_spi2_cz0_drv (128)
	#define mask_of_padtop1_reg_spi2_cz0_drv (0x200)
	unsigned int reg_spi2_cz0_drv:1;

	// h0040, bit: 10
	/* reg_spi2_cz0_ie*/
	#define offset_of_padtop1_reg_spi2_cz0_ie (128)
	#define mask_of_padtop1_reg_spi2_cz0_ie (0x400)
	unsigned int reg_spi2_cz0_ie:1;

	// h0040, bit: 11
	/* reg_spi2_cz0_pe*/
	#define offset_of_padtop1_reg_spi2_cz0_pe (128)
	#define mask_of_padtop1_reg_spi2_cz0_pe (0x800)
	unsigned int reg_spi2_cz0_pe:1;

	// h0040, bit: 12
	/* reg_spi2_cz0_ps*/
	#define offset_of_padtop1_reg_spi2_cz0_ps (128)
	#define mask_of_padtop1_reg_spi2_cz0_ps (0x1000)
	unsigned int reg_spi2_cz0_ps:1;

	// h0040, bit: 13
	/* reg_spi2_di_drv*/
	#define offset_of_padtop1_reg_spi2_di_drv (128)
	#define mask_of_padtop1_reg_spi2_di_drv (0x2000)
	unsigned int reg_spi2_di_drv:1;

	// h0040, bit: 14
	/* reg_spi2_di_ie*/
	#define offset_of_padtop1_reg_spi2_di_ie (128)
	#define mask_of_padtop1_reg_spi2_di_ie (0x4000)
	unsigned int reg_spi2_di_ie:1;

	// h0040, bit: 15
	/* reg_spi2_di_pe*/
	#define offset_of_padtop1_reg_spi2_di_pe (128)
	#define mask_of_padtop1_reg_spi2_di_pe (0x8000)
	unsigned int reg_spi2_di_pe:1;

	// h0040
	unsigned int /* padding 16 bit */:16;

	// h0041, bit: 0
	/* reg_spi2_di_ps*/
	#define offset_of_padtop1_reg_spi2_di_ps (130)
	#define mask_of_padtop1_reg_spi2_di_ps (0x1)
	unsigned int reg_spi2_di_ps:1;

	// h0041, bit: 1
	/* reg_spi2_do_drv*/
	#define offset_of_padtop1_reg_spi2_do_drv (130)
	#define mask_of_padtop1_reg_spi2_do_drv (0x2)
	unsigned int reg_spi2_do_drv:1;

	// h0041, bit: 2
	/* reg_spi2_do_ie*/
	#define offset_of_padtop1_reg_spi2_do_ie (130)
	#define mask_of_padtop1_reg_spi2_do_ie (0x4)
	unsigned int reg_spi2_do_ie:1;

	// h0041, bit: 3
	/* reg_spi2_do_pe*/
	#define offset_of_padtop1_reg_spi2_do_pe (130)
	#define mask_of_padtop1_reg_spi2_do_pe (0x8)
	unsigned int reg_spi2_do_pe:1;

	// h0041, bit: 4
	/* reg_spi2_do_ps*/
	#define offset_of_padtop1_reg_spi2_do_ps (130)
	#define mask_of_padtop1_reg_spi2_do_ps (0x10)
	unsigned int reg_spi2_do_ps:1;

	// h0041, bit: 5
	/* reg_ttl_clk_drv0*/
	#define offset_of_padtop1_reg_ttl_clk_drv0 (130)
	#define mask_of_padtop1_reg_ttl_clk_drv0 (0x20)
	unsigned int reg_ttl_clk_drv0:1;

	// h0041, bit: 6
	/* reg_ttl_clk_drv1*/
	#define offset_of_padtop1_reg_ttl_clk_drv1 (130)
	#define mask_of_padtop1_reg_ttl_clk_drv1 (0x40)
	unsigned int reg_ttl_clk_drv1:1;

	// h0041, bit: 7
	/* reg_ttl_clk_lsh18_en*/
	#define offset_of_padtop1_reg_ttl_clk_lsh18_en (130)
	#define mask_of_padtop1_reg_ttl_clk_lsh18_en (0x80)
	unsigned int reg_ttl_clk_lsh18_en:1;

	// h0041, bit: 8
	/* reg_ttl_clk_pe*/
	#define offset_of_padtop1_reg_ttl_clk_pe (130)
	#define mask_of_padtop1_reg_ttl_clk_pe (0x100)
	unsigned int reg_ttl_clk_pe:1;

	// h0041, bit: 9
	/* reg_ttl_clk_ps*/
	#define offset_of_padtop1_reg_ttl_clk_ps (130)
	#define mask_of_padtop1_reg_ttl_clk_ps (0x200)
	unsigned int reg_ttl_clk_ps:1;

	// h0041, bit: 14
	/* */
	unsigned int :6;

	// h0041
	unsigned int /* padding 16 bit */:16;

	// h0042, bit: 14
	/* reg_ttl_d_drv0*/
	#define offset_of_padtop1_reg_ttl_d_drv0 (132)
	#define mask_of_padtop1_reg_ttl_d_drv0 (0xffff)
	unsigned int reg_ttl_d_drv0:16;

	// h0042
	unsigned int /* padding 16 bit */:16;

	// h0043, bit: 7
	/* reg_ttl_d_drv0*/
	#define offset_of_padtop1_reg_ttl_d_drv0_1 (134)
	#define mask_of_padtop1_reg_ttl_d_drv0_1 (0xff)
	unsigned int reg_ttl_d_drv0_1:8;

	// h0043, bit: 14
	/* */
	unsigned int :8;

	// h0043
	unsigned int /* padding 16 bit */:16;

	// h0044, bit: 14
	/* reg_ttl_d_drv1*/
	#define offset_of_padtop1_reg_ttl_d_drv1 (136)
	#define mask_of_padtop1_reg_ttl_d_drv1 (0xffff)
	unsigned int reg_ttl_d_drv1:16;

	// h0044
	unsigned int /* padding 16 bit */:16;

	// h0045, bit: 7
	/* reg_ttl_d_drv1*/
	#define offset_of_padtop1_reg_ttl_d_drv1_1 (138)
	#define mask_of_padtop1_reg_ttl_d_drv1_1 (0xff)
	unsigned int reg_ttl_d_drv1_1:8;

	// h0045, bit: 14
	/* */
	unsigned int :8;

	// h0045
	unsigned int /* padding 16 bit */:16;

	// h0046, bit: 14
	/* reg_ttl_d_lsh18_en*/
	#define offset_of_padtop1_reg_ttl_d_lsh18_en (140)
	#define mask_of_padtop1_reg_ttl_d_lsh18_en (0xffff)
	unsigned int reg_ttl_d_lsh18_en:16;

	// h0046
	unsigned int /* padding 16 bit */:16;

	// h0047, bit: 7
	/* reg_ttl_d_lsh18_en*/
	#define offset_of_padtop1_reg_ttl_d_lsh18_en_1 (142)
	#define mask_of_padtop1_reg_ttl_d_lsh18_en_1 (0xff)
	unsigned int reg_ttl_d_lsh18_en_1:8;

	// h0047, bit: 14
	/* */
	unsigned int :8;

	// h0047
	unsigned int /* padding 16 bit */:16;

	// h0048, bit: 14
	/* reg_ttl_d_pe*/
	#define offset_of_padtop1_reg_ttl_d_pe (144)
	#define mask_of_padtop1_reg_ttl_d_pe (0xffff)
	unsigned int reg_ttl_d_pe:16;

	// h0048
	unsigned int /* padding 16 bit */:16;

	// h0049, bit: 7
	/* reg_ttl_d_pe*/
	#define offset_of_padtop1_reg_ttl_d_pe_1 (146)
	#define mask_of_padtop1_reg_ttl_d_pe_1 (0xff)
	unsigned int reg_ttl_d_pe_1:8;

	// h0049, bit: 14
	/* */
	unsigned int :8;

	// h0049
	unsigned int /* padding 16 bit */:16;

	// h004a, bit: 14
	/* reg_ttl_d_ps*/
	#define offset_of_padtop1_reg_ttl_d_ps (148)
	#define mask_of_padtop1_reg_ttl_d_ps (0xffff)
	unsigned int reg_ttl_d_ps:16;

	// h004a
	unsigned int /* padding 16 bit */:16;

	// h004b, bit: 7
	/* reg_ttl_d_ps*/
	#define offset_of_padtop1_reg_ttl_d_ps_1 (150)
	#define mask_of_padtop1_reg_ttl_d_ps_1 (0xff)
	unsigned int reg_ttl_d_ps_1:8;

	// h004b, bit: 14
	/* */
	unsigned int :8;

	// h004b
	unsigned int /* padding 16 bit */:16;

	// h004c, bit: 0
	/* reg_ttl_de_drv0*/
	#define offset_of_padtop1_reg_ttl_de_drv0 (152)
	#define mask_of_padtop1_reg_ttl_de_drv0 (0x1)
	unsigned int reg_ttl_de_drv0:1;

	// h004c, bit: 1
	/* reg_ttl_de_drv1*/
	#define offset_of_padtop1_reg_ttl_de_drv1 (152)
	#define mask_of_padtop1_reg_ttl_de_drv1 (0x2)
	unsigned int reg_ttl_de_drv1:1;

	// h004c, bit: 2
	/* reg_ttl_de_lsh18_en*/
	#define offset_of_padtop1_reg_ttl_de_lsh18_en (152)
	#define mask_of_padtop1_reg_ttl_de_lsh18_en (0x4)
	unsigned int reg_ttl_de_lsh18_en:1;

	// h004c, bit: 3
	/* reg_ttl_de_pe*/
	#define offset_of_padtop1_reg_ttl_de_pe (152)
	#define mask_of_padtop1_reg_ttl_de_pe (0x8)
	unsigned int reg_ttl_de_pe:1;

	// h004c, bit: 4
	/* reg_ttl_de_ps*/
	#define offset_of_padtop1_reg_ttl_de_ps (152)
	#define mask_of_padtop1_reg_ttl_de_ps (0x10)
	unsigned int reg_ttl_de_ps:1;

	// h004c, bit: 5
	/* reg_ttl_gpio0_drv*/
	#define offset_of_padtop1_reg_ttl_gpio0_drv (152)
	#define mask_of_padtop1_reg_ttl_gpio0_drv (0x20)
	unsigned int reg_ttl_gpio0_drv:1;

	// h004c, bit: 6
	/* reg_ttl_gpio0_ie*/
	#define offset_of_padtop1_reg_ttl_gpio0_ie (152)
	#define mask_of_padtop1_reg_ttl_gpio0_ie (0x40)
	unsigned int reg_ttl_gpio0_ie:1;

	// h004c, bit: 7
	/* reg_ttl_gpio0_pe*/
	#define offset_of_padtop1_reg_ttl_gpio0_pe (152)
	#define mask_of_padtop1_reg_ttl_gpio0_pe (0x80)
	unsigned int reg_ttl_gpio0_pe:1;

	// h004c, bit: 8
	/* reg_ttl_gpio0_ps*/
	#define offset_of_padtop1_reg_ttl_gpio0_ps (152)
	#define mask_of_padtop1_reg_ttl_gpio0_ps (0x100)
	unsigned int reg_ttl_gpio0_ps:1;

	// h004c, bit: 9
	/* reg_ttl_gpio1_drv*/
	#define offset_of_padtop1_reg_ttl_gpio1_drv (152)
	#define mask_of_padtop1_reg_ttl_gpio1_drv (0x200)
	unsigned int reg_ttl_gpio1_drv:1;

	// h004c, bit: 10
	/* reg_ttl_gpio1_ie*/
	#define offset_of_padtop1_reg_ttl_gpio1_ie (152)
	#define mask_of_padtop1_reg_ttl_gpio1_ie (0x400)
	unsigned int reg_ttl_gpio1_ie:1;

	// h004c, bit: 11
	/* reg_ttl_gpio1_pe*/
	#define offset_of_padtop1_reg_ttl_gpio1_pe (152)
	#define mask_of_padtop1_reg_ttl_gpio1_pe (0x800)
	unsigned int reg_ttl_gpio1_pe:1;

	// h004c, bit: 12
	/* reg_ttl_gpio1_ps*/
	#define offset_of_padtop1_reg_ttl_gpio1_ps (152)
	#define mask_of_padtop1_reg_ttl_gpio1_ps (0x1000)
	unsigned int reg_ttl_gpio1_ps:1;

	// h004c, bit: 13
	/* reg_ttl_gpio2_drv*/
	#define offset_of_padtop1_reg_ttl_gpio2_drv (152)
	#define mask_of_padtop1_reg_ttl_gpio2_drv (0x2000)
	unsigned int reg_ttl_gpio2_drv:1;

	// h004c, bit: 14
	/* reg_ttl_gpio2_ie*/
	#define offset_of_padtop1_reg_ttl_gpio2_ie (152)
	#define mask_of_padtop1_reg_ttl_gpio2_ie (0x4000)
	unsigned int reg_ttl_gpio2_ie:1;

	// h004c, bit: 15
	/* reg_ttl_gpio2_pe*/
	#define offset_of_padtop1_reg_ttl_gpio2_pe (152)
	#define mask_of_padtop1_reg_ttl_gpio2_pe (0x8000)
	unsigned int reg_ttl_gpio2_pe:1;

	// h004c
	unsigned int /* padding 16 bit */:16;

	// h004d, bit: 0
	/* reg_ttl_gpio2_ps*/
	#define offset_of_padtop1_reg_ttl_gpio2_ps (154)
	#define mask_of_padtop1_reg_ttl_gpio2_ps (0x1)
	unsigned int reg_ttl_gpio2_ps:1;

	// h004d, bit: 1
	/* reg_ttl_hsync_drv0*/
	#define offset_of_padtop1_reg_ttl_hsync_drv0 (154)
	#define mask_of_padtop1_reg_ttl_hsync_drv0 (0x2)
	unsigned int reg_ttl_hsync_drv0:1;

	// h004d, bit: 2
	/* reg_ttl_hsync_drv1*/
	#define offset_of_padtop1_reg_ttl_hsync_drv1 (154)
	#define mask_of_padtop1_reg_ttl_hsync_drv1 (0x4)
	unsigned int reg_ttl_hsync_drv1:1;

	// h004d, bit: 3
	/* reg_ttl_hsync_lsh18_en*/
	#define offset_of_padtop1_reg_ttl_hsync_lsh18_en (154)
	#define mask_of_padtop1_reg_ttl_hsync_lsh18_en (0x8)
	unsigned int reg_ttl_hsync_lsh18_en:1;

	// h004d, bit: 4
	/* reg_ttl_hsync_pe*/
	#define offset_of_padtop1_reg_ttl_hsync_pe (154)
	#define mask_of_padtop1_reg_ttl_hsync_pe (0x10)
	unsigned int reg_ttl_hsync_pe:1;

	// h004d, bit: 5
	/* reg_ttl_hsync_ps*/
	#define offset_of_padtop1_reg_ttl_hsync_ps (154)
	#define mask_of_padtop1_reg_ttl_hsync_ps (0x20)
	unsigned int reg_ttl_hsync_ps:1;

	// h004d, bit: 6
	/* reg_ttl_vsync_drv0*/
	#define offset_of_padtop1_reg_ttl_vsync_drv0 (154)
	#define mask_of_padtop1_reg_ttl_vsync_drv0 (0x40)
	unsigned int reg_ttl_vsync_drv0:1;

	// h004d, bit: 7
	/* reg_ttl_vsync_drv1*/
	#define offset_of_padtop1_reg_ttl_vsync_drv1 (154)
	#define mask_of_padtop1_reg_ttl_vsync_drv1 (0x80)
	unsigned int reg_ttl_vsync_drv1:1;

	// h004d, bit: 8
	/* reg_ttl_vsync_lsh18_en*/
	#define offset_of_padtop1_reg_ttl_vsync_lsh18_en (154)
	#define mask_of_padtop1_reg_ttl_vsync_lsh18_en (0x100)
	unsigned int reg_ttl_vsync_lsh18_en:1;

	// h004d, bit: 9
	/* reg_ttl_vsync_pe*/
	#define offset_of_padtop1_reg_ttl_vsync_pe (154)
	#define mask_of_padtop1_reg_ttl_vsync_pe (0x200)
	unsigned int reg_ttl_vsync_pe:1;

	// h004d, bit: 10
	/* reg_ttl_vsync_ps*/
	#define offset_of_padtop1_reg_ttl_vsync_ps (154)
	#define mask_of_padtop1_reg_ttl_vsync_ps (0x400)
	unsigned int reg_ttl_vsync_ps:1;

	// h004d, bit: 11
	/* reg_uart0_rx_drv*/
	#define offset_of_padtop1_reg_uart0_rx_drv (154)
	#define mask_of_padtop1_reg_uart0_rx_drv (0x800)
	unsigned int reg_uart0_rx_drv:1;

	// h004d, bit: 12
	/* reg_uart0_rx_ie*/
	#define offset_of_padtop1_reg_uart0_rx_ie (154)
	#define mask_of_padtop1_reg_uart0_rx_ie (0x1000)
	unsigned int reg_uart0_rx_ie:1;

	// h004d, bit: 13
	/* reg_uart0_rx_pe*/
	#define offset_of_padtop1_reg_uart0_rx_pe (154)
	#define mask_of_padtop1_reg_uart0_rx_pe (0x2000)
	unsigned int reg_uart0_rx_pe:1;

	// h004d, bit: 14
	/* reg_uart0_rx_ps*/
	#define offset_of_padtop1_reg_uart0_rx_ps (154)
	#define mask_of_padtop1_reg_uart0_rx_ps (0x4000)
	unsigned int reg_uart0_rx_ps:1;

	// h004d, bit: 15
	/* reg_uart0_tx_drv*/
	#define offset_of_padtop1_reg_uart0_tx_drv (154)
	#define mask_of_padtop1_reg_uart0_tx_drv (0x8000)
	unsigned int reg_uart0_tx_drv:1;

	// h004d
	unsigned int /* padding 16 bit */:16;

	// h004e, bit: 0
	/* reg_uart0_tx_ie*/
	#define offset_of_padtop1_reg_uart0_tx_ie (156)
	#define mask_of_padtop1_reg_uart0_tx_ie (0x1)
	unsigned int reg_uart0_tx_ie:1;

	// h004e, bit: 1
	/* reg_uart0_tx_pe*/
	#define offset_of_padtop1_reg_uart0_tx_pe (156)
	#define mask_of_padtop1_reg_uart0_tx_pe (0x2)
	unsigned int reg_uart0_tx_pe:1;

	// h004e, bit: 2
	/* reg_uart0_tx_ps*/
	#define offset_of_padtop1_reg_uart0_tx_ps (156)
	#define mask_of_padtop1_reg_uart0_tx_ps (0x4)
	unsigned int reg_uart0_tx_ps:1;

	// h004e, bit: 3
	/* reg_uart1_rx_drv*/
	#define offset_of_padtop1_reg_uart1_rx_drv (156)
	#define mask_of_padtop1_reg_uart1_rx_drv (0x8)
	unsigned int reg_uart1_rx_drv:1;

	// h004e, bit: 4
	/* reg_uart1_rx_ie*/
	#define offset_of_padtop1_reg_uart1_rx_ie (156)
	#define mask_of_padtop1_reg_uart1_rx_ie (0x10)
	unsigned int reg_uart1_rx_ie:1;

	// h004e, bit: 5
	/* reg_uart1_rx_pe*/
	#define offset_of_padtop1_reg_uart1_rx_pe (156)
	#define mask_of_padtop1_reg_uart1_rx_pe (0x20)
	unsigned int reg_uart1_rx_pe:1;

	// h004e, bit: 6
	/* reg_uart1_rx_ps*/
	#define offset_of_padtop1_reg_uart1_rx_ps (156)
	#define mask_of_padtop1_reg_uart1_rx_ps (0x40)
	unsigned int reg_uart1_rx_ps:1;

	// h004e, bit: 7
	/* reg_uart1_tx_drv*/
	#define offset_of_padtop1_reg_uart1_tx_drv (156)
	#define mask_of_padtop1_reg_uart1_tx_drv (0x80)
	unsigned int reg_uart1_tx_drv:1;

	// h004e, bit: 8
	/* reg_uart1_tx_ie*/
	#define offset_of_padtop1_reg_uart1_tx_ie (156)
	#define mask_of_padtop1_reg_uart1_tx_ie (0x100)
	unsigned int reg_uart1_tx_ie:1;

	// h004e, bit: 9
	/* reg_uart1_tx_pe*/
	#define offset_of_padtop1_reg_uart1_tx_pe (156)
	#define mask_of_padtop1_reg_uart1_tx_pe (0x200)
	unsigned int reg_uart1_tx_pe:1;

	// h004e, bit: 10
	/* reg_uart1_tx_ps*/
	#define offset_of_padtop1_reg_uart1_tx_ps (156)
	#define mask_of_padtop1_reg_uart1_tx_ps (0x400)
	unsigned int reg_uart1_tx_ps:1;

	// h004e, bit: 11
	/* reg_uart2_rx_drv*/
	#define offset_of_padtop1_reg_uart2_rx_drv (156)
	#define mask_of_padtop1_reg_uart2_rx_drv (0x800)
	unsigned int reg_uart2_rx_drv:1;

	// h004e, bit: 12
	/* reg_uart2_rx_ie*/
	#define offset_of_padtop1_reg_uart2_rx_ie (156)
	#define mask_of_padtop1_reg_uart2_rx_ie (0x1000)
	unsigned int reg_uart2_rx_ie:1;

	// h004e, bit: 13
	/* reg_uart2_rx_pe*/
	#define offset_of_padtop1_reg_uart2_rx_pe (156)
	#define mask_of_padtop1_reg_uart2_rx_pe (0x2000)
	unsigned int reg_uart2_rx_pe:1;

	// h004e, bit: 14
	/* reg_uart2_rx_ps*/
	#define offset_of_padtop1_reg_uart2_rx_ps (156)
	#define mask_of_padtop1_reg_uart2_rx_ps (0x4000)
	unsigned int reg_uart2_rx_ps:1;

	// h004e, bit: 15
	/* reg_uart2_tx_drv*/
	#define offset_of_padtop1_reg_uart2_tx_drv (156)
	#define mask_of_padtop1_reg_uart2_tx_drv (0x8000)
	unsigned int reg_uart2_tx_drv:1;

	// h004e
	unsigned int /* padding 16 bit */:16;

	// h004f, bit: 14
	/* */
	unsigned int :16;

	// h004f
	unsigned int /* padding 16 bit */:16;

	// h0050, bit: 0
	/* reg_uart2_tx_ie*/
	#define offset_of_padtop1_reg_uart2_tx_ie (160)
	#define mask_of_padtop1_reg_uart2_tx_ie (0x1)
	unsigned int reg_uart2_tx_ie:1;

	// h0050, bit: 1
	/* reg_uart2_tx_pe*/
	#define offset_of_padtop1_reg_uart2_tx_pe (160)
	#define mask_of_padtop1_reg_uart2_tx_pe (0x2)
	unsigned int reg_uart2_tx_pe:1;

	// h0050, bit: 2
	/* reg_uart2_tx_ps*/
	#define offset_of_padtop1_reg_uart2_tx_ps (160)
	#define mask_of_padtop1_reg_uart2_tx_ps (0x4)
	unsigned int reg_uart2_tx_ps:1;

	// h0050, bit: 3
	/* reg_vsync_out_drv*/
	#define offset_of_padtop1_reg_vsync_out_drv (160)
	#define mask_of_padtop1_reg_vsync_out_drv (0x8)
	unsigned int reg_vsync_out_drv:1;

	// h0050, bit: 5
	/* reg_comb_gcr_sel_in*/
	#define offset_of_padtop1_reg_comb_gcr_sel_in (160)
	#define mask_of_padtop1_reg_comb_gcr_sel_in (0x30)
	unsigned int reg_comb_gcr_sel_in:2;

	// h0050, bit: 6
	/* reg_comb_tx_bist_en*/
	#define offset_of_padtop1_reg_comb_tx_bist_en (160)
	#define mask_of_padtop1_reg_comb_tx_bist_en (0x40)
	unsigned int reg_comb_tx_bist_en:1;

	// h0050, bit: 10
	/* reg_BT_GPIO_OUT*/
	#define offset_of_padtop1_reg_bt_gpio_out (160)
	#define mask_of_padtop1_reg_bt_gpio_out (0x780)
	unsigned int reg_bt_gpio_out:4;

	// h0050, bit: 14
	/* */
	unsigned int :5;

	// h0050
	unsigned int /* padding 16 bit */:16;

	// h0051, bit: 14
	/* reg_CHIP_TOP_GPIO_OUT*/
	#define offset_of_padtop1_reg_chip_top_gpio_out (162)
	#define mask_of_padtop1_reg_chip_top_gpio_out (0xffff)
	unsigned int reg_chip_top_gpio_out:16;

	// h0051
	unsigned int /* padding 16 bit */:16;

	// h0052, bit: 9
	/* reg_CODEC_I2S_GPIO_OUT*/
	#define offset_of_padtop1_reg_codec_i2s_gpio_out (164)
	#define mask_of_padtop1_reg_codec_i2s_gpio_out (0x3ff)
	unsigned int reg_codec_i2s_gpio_out:10;

	// h0052, bit: 14
	/* */
	unsigned int :6;

	// h0052
	unsigned int /* padding 16 bit */:16;

	// h0053, bit: 4
	/* reg_DMIC_GPIO_OUT*/
	#define offset_of_padtop1_reg_dmic_gpio_out (166)
	#define mask_of_padtop1_reg_dmic_gpio_out (0x1f)
	unsigned int reg_dmic_gpio_out:5;

	// h0053, bit: 7
	/* */
	unsigned int :3;

	// h0053, bit: 11
	/* reg_FUART_GPIO_OUT*/
	#define offset_of_padtop1_reg_fuart_gpio_out (166)
	#define mask_of_padtop1_reg_fuart_gpio_out (0xf00)
	unsigned int reg_fuart_gpio_out:4;

	// h0053, bit: 14
	/* reg_HDMITX_GPIO_OUT*/
	#define offset_of_padtop1_reg_hdmitx_gpio_out (166)
	#define mask_of_padtop1_reg_hdmitx_gpio_out (0x7000)
	unsigned int reg_hdmitx_gpio_out:3;

	// h0053, bit: 15
	/* */
	unsigned int :1;

	// h0053
	unsigned int /* padding 16 bit */:16;

	// h0054, bit: 1
	/* reg_I2C0_GPIO_OUT*/
	#define offset_of_padtop1_reg_i2c0_gpio_out (168)
	#define mask_of_padtop1_reg_i2c0_gpio_out (0x3)
	unsigned int reg_i2c0_gpio_out:2;

	// h0054, bit: 3
	/* reg_I2C2_GPIO_OUT*/
	#define offset_of_padtop1_reg_i2c2_gpio_out (168)
	#define mask_of_padtop1_reg_i2c2_gpio_out (0xc)
	unsigned int reg_i2c2_gpio_out:2;

	// h0054, bit: 5
	/* reg_I2C3_GPIO_OUT*/
	#define offset_of_padtop1_reg_i2c3_gpio_out (168)
	#define mask_of_padtop1_reg_i2c3_gpio_out (0x30)
	unsigned int reg_i2c3_gpio_out:2;

	// h0054, bit: 9
	/* reg_JTAG_GPIO_OUT*/
	#define offset_of_padtop1_reg_jtag_gpio_out (168)
	#define mask_of_padtop1_reg_jtag_gpio_out (0x3c0)
	unsigned int reg_jtag_gpio_out:4;

	// h0054, bit: 14
	/* */
	unsigned int :6;

	// h0054
	unsigned int /* padding 16 bit */:16;

	// h0055, bit: 9
	/* reg_MIPI_TX_GPIO_OUT*/
	#define offset_of_padtop1_reg_mipi_tx_gpio_out (170)
	#define mask_of_padtop1_reg_mipi_tx_gpio_out (0x3ff)
	unsigned int reg_mipi_tx_gpio_out:10;

	// h0055, bit: 14
	/* */
	unsigned int :6;

	// h0055
	unsigned int /* padding 16 bit */:16;

	// h0056, bit: 6
	/* reg_MISC_I2S_GPIO_OUT*/
	#define offset_of_padtop1_reg_misc_i2s_gpio_out (172)
	#define mask_of_padtop1_reg_misc_i2s_gpio_out (0x7f)
	unsigned int reg_misc_i2s_gpio_out:7;

	// h0056, bit: 7
	/* */
	unsigned int :1;

	// h0056, bit: 12
	/* reg_DMIC_GPIO_OEN*/
	#define offset_of_padtop1_reg_dmic_gpio_oen (172)
	#define mask_of_padtop1_reg_dmic_gpio_oen (0x1f00)
	unsigned int reg_dmic_gpio_oen:5;

	// h0056, bit: 14
	/* */
	unsigned int :3;

	// h0056
	unsigned int /* padding 16 bit */:16;

	// h0057, bit: 14
	/* reg_NAND_GPIO_OUT*/
	#define offset_of_padtop1_reg_nand_gpio_out (174)
	#define mask_of_padtop1_reg_nand_gpio_out (0xffff)
	unsigned int reg_nand_gpio_out:16;

	// h0057
	unsigned int /* padding 16 bit */:16;

	// h0058, bit: 13
	/* reg_RGM0_GPIO_OUT*/
	#define offset_of_padtop1_reg_rgm0_gpio_out (176)
	#define mask_of_padtop1_reg_rgm0_gpio_out (0x3fff)
	unsigned int reg_rgm0_gpio_out:14;

	// h0058, bit: 14
	/* */
	unsigned int :2;

	// h0058
	unsigned int /* padding 16 bit */:16;

	// h0059, bit: 11
	/* reg_RGM1_GPIO_OUT*/
	#define offset_of_padtop1_reg_rgm1_gpio_out (178)
	#define mask_of_padtop1_reg_rgm1_gpio_out (0xfff)
	unsigned int reg_rgm1_gpio_out:12;

	// h0059, bit: 14
	/* */
	unsigned int :4;

	// h0059
	unsigned int /* padding 16 bit */:16;

	// h005a, bit: 1
	/* reg_PWM_GPIO_OUT*/
	#define offset_of_padtop1_reg_pwm_gpio_out (180)
	#define mask_of_padtop1_reg_pwm_gpio_out (0x3)
	unsigned int reg_pwm_gpio_out:2;

	// h005a, bit: 7
	/* reg_SD_GPIO_OUT*/
	#define offset_of_padtop1_reg_sd_gpio_out (180)
	#define mask_of_padtop1_reg_sd_gpio_out (0xfc)
	unsigned int reg_sd_gpio_out:6;

	// h005a, bit: 13
	/* reg_SD_IO_GPIO_OUT*/
	#define offset_of_padtop1_reg_sd_io_gpio_out (180)
	#define mask_of_padtop1_reg_sd_io_gpio_out (0x3f00)
	unsigned int reg_sd_io_gpio_out:6;

	// h005a, bit: 14
	/* */
	unsigned int :2;

	// h005a
	unsigned int /* padding 16 bit */:16;

	// h005b, bit: 14
	/* reg_SNR0_GPIO_OUT*/
	#define offset_of_padtop1_reg_snr0_gpio_out (182)
	#define mask_of_padtop1_reg_snr0_gpio_out (0xffff)
	unsigned int reg_snr0_gpio_out:16;

	// h005b
	unsigned int /* padding 16 bit */:16;

	// h005c, bit: 1
	/* reg_SNR0_GPIO_OUT*/
	#define offset_of_padtop1_reg_snr0_gpio_out_1 (184)
	#define mask_of_padtop1_reg_snr0_gpio_out_1 (0x3)
	unsigned int reg_snr0_gpio_out_1:2;

	// h005c, bit: 14
	/* */
	unsigned int :14;

	// h005c
	unsigned int /* padding 16 bit */:16;

	// h005d, bit: 14
	/* reg_SNR1_GPIO_OUT*/
	#define offset_of_padtop1_reg_snr1_gpio_out (186)
	#define mask_of_padtop1_reg_snr1_gpio_out (0xffff)
	unsigned int reg_snr1_gpio_out:16;

	// h005d
	unsigned int /* padding 16 bit */:16;

	// h005e, bit: 1
	/* reg_SNR1_GPIO_OUT*/
	#define offset_of_padtop1_reg_snr1_gpio_out_1 (188)
	#define mask_of_padtop1_reg_snr1_gpio_out_1 (0x3)
	unsigned int reg_snr1_gpio_out_1:2;

	// h005e, bit: 14
	/* */
	unsigned int :14;

	// h005e
	unsigned int /* padding 16 bit */:16;

	// h005f, bit: 14
	/* reg_SNR2_GPIO_OUT*/
	#define offset_of_padtop1_reg_snr2_gpio_out (190)
	#define mask_of_padtop1_reg_snr2_gpio_out (0xffff)
	unsigned int reg_snr2_gpio_out:16;

	// h005f
	unsigned int /* padding 16 bit */:16;

	// h0060, bit: 1
	/* reg_SNR2_GPIO_OUT*/
	#define offset_of_padtop1_reg_snr2_gpio_out_1 (192)
	#define mask_of_padtop1_reg_snr2_gpio_out_1 (0x3)
	unsigned int reg_snr2_gpio_out_1:2;

	// h0060, bit: 14
	/* */
	unsigned int :14;

	// h0060
	unsigned int /* padding 16 bit */:16;

	// h0061, bit: 14
	/* reg_SNR3_GPIO_OUT*/
	#define offset_of_padtop1_reg_snr3_gpio_out (194)
	#define mask_of_padtop1_reg_snr3_gpio_out (0xffff)
	unsigned int reg_snr3_gpio_out:16;

	// h0061
	unsigned int /* padding 16 bit */:16;

	// h0062, bit: 1
	/* reg_SNR3_GPIO_OUT*/
	#define offset_of_padtop1_reg_snr3_gpio_out_1 (196)
	#define mask_of_padtop1_reg_snr3_gpio_out_1 (0x3)
	unsigned int reg_snr3_gpio_out_1:2;

	// h0062, bit: 14
	/* */
	unsigned int :14;

	// h0062
	unsigned int /* padding 16 bit */:16;

	// h0063, bit: 3
	/* reg_SPI0_GPIO_OUT*/
	#define offset_of_padtop1_reg_spi0_gpio_out (198)
	#define mask_of_padtop1_reg_spi0_gpio_out (0xf)
	unsigned int reg_spi0_gpio_out:4;

	// h0063, bit: 7
	/* reg_SPI1_GPIO_OUT*/
	#define offset_of_padtop1_reg_spi1_gpio_out (198)
	#define mask_of_padtop1_reg_spi1_gpio_out (0xf0)
	unsigned int reg_spi1_gpio_out:4;

	// h0063, bit: 11
	/* reg_SPI2_GPIO_OUT*/
	#define offset_of_padtop1_reg_spi2_gpio_out (198)
	#define mask_of_padtop1_reg_spi2_gpio_out (0xf00)
	unsigned int reg_spi2_gpio_out:4;

	// h0063, bit: 12
	/* reg_SPDIF_GPIO_OUT*/
	#define offset_of_padtop1_reg_spdif_gpio_out (198)
	#define mask_of_padtop1_reg_spdif_gpio_out (0x1000)
	unsigned int reg_spdif_gpio_out:1;

	// h0063, bit: 14
	/* reg_SYNC_GPIO_OUT*/
	#define offset_of_padtop1_reg_sync_gpio_out (198)
	#define mask_of_padtop1_reg_sync_gpio_out (0x6000)
	unsigned int reg_sync_gpio_out:2;

	// h0063, bit: 15
	/* */
	unsigned int :1;

	// h0063
	unsigned int /* padding 16 bit */:16;

	// h0064, bit: 14
	/* reg_TTL_GPIO_OUT*/
	#define offset_of_padtop1_reg_ttl_gpio_out (200)
	#define mask_of_padtop1_reg_ttl_gpio_out (0xffff)
	unsigned int reg_ttl_gpio_out:16;

	// h0064
	unsigned int /* padding 16 bit */:16;

	// h0065, bit: 14
	/* reg_TTL_GPIO_OUT*/
	#define offset_of_padtop1_reg_ttl_gpio_out_1 (202)
	#define mask_of_padtop1_reg_ttl_gpio_out_1 (0x7fff)
	unsigned int reg_ttl_gpio_out_1:15;

	// h0065, bit: 15
	/* */
	unsigned int :1;

	// h0065
	unsigned int /* padding 16 bit */:16;

	// h0066, bit: 5
	/* reg_UART_GPIO_OUT*/
	#define offset_of_padtop1_reg_uart_gpio_out (204)
	#define mask_of_padtop1_reg_uart_gpio_out (0x3f)
	unsigned int reg_uart_gpio_out:6;

	// h0066, bit: 7
	/* */
	unsigned int :2;

	// h0066, bit: 11
	/* reg_BT_GPIO_OEN*/
	#define offset_of_padtop1_reg_bt_gpio_oen (204)
	#define mask_of_padtop1_reg_bt_gpio_oen (0xf00)
	unsigned int reg_bt_gpio_oen:4;

	// h0066, bit: 14
	/* */
	unsigned int :4;

	// h0066
	unsigned int /* padding 16 bit */:16;

	// h0067, bit: 14
	/* reg_CHIP_TOP_GPIO_OEN*/
	#define offset_of_padtop1_reg_chip_top_gpio_oen (206)
	#define mask_of_padtop1_reg_chip_top_gpio_oen (0xffff)
	unsigned int reg_chip_top_gpio_oen:16;

	// h0067
	unsigned int /* padding 16 bit */:16;

	// h0068, bit: 9
	/* reg_CODEC_I2S_GPIO_OEN*/
	#define offset_of_padtop1_reg_codec_i2s_gpio_oen (208)
	#define mask_of_padtop1_reg_codec_i2s_gpio_oen (0x3ff)
	unsigned int reg_codec_i2s_gpio_oen:10;

	// h0068, bit: 14
	/* */
	unsigned int :6;

	// h0068
	unsigned int /* padding 16 bit */:16;

	// h0069, bit: 3
	/* reg_FUART_GPIO_OEN*/
	#define offset_of_padtop1_reg_fuart_gpio_oen (210)
	#define mask_of_padtop1_reg_fuart_gpio_oen (0xf)
	unsigned int reg_fuart_gpio_oen:4;

	// h0069, bit: 6
	/* reg_HDMITX_GPIO_OEN*/
	#define offset_of_padtop1_reg_hdmitx_gpio_oen (210)
	#define mask_of_padtop1_reg_hdmitx_gpio_oen (0x70)
	unsigned int reg_hdmitx_gpio_oen:3;

	// h0069, bit: 8
	/* reg_I2C0_GPIO_OEN*/
	#define offset_of_padtop1_reg_i2c0_gpio_oen (210)
	#define mask_of_padtop1_reg_i2c0_gpio_oen (0x180)
	unsigned int reg_i2c0_gpio_oen:2;

	// h0069, bit: 10
	/* reg_I2C2_GPIO_OEN*/
	#define offset_of_padtop1_reg_i2c2_gpio_oen (210)
	#define mask_of_padtop1_reg_i2c2_gpio_oen (0x600)
	unsigned int reg_i2c2_gpio_oen:2;

	// h0069, bit: 12
	/* reg_I2C3_GPIO_OEN*/
	#define offset_of_padtop1_reg_i2c3_gpio_oen (210)
	#define mask_of_padtop1_reg_i2c3_gpio_oen (0x1800)
	unsigned int reg_i2c3_gpio_oen:2;

	// h0069, bit: 14
	/* */
	unsigned int :3;

	// h0069
	unsigned int /* padding 16 bit */:16;

	// h006a, bit: 3
	/* reg_JTAG_GPIO_OEN*/
	#define offset_of_padtop1_reg_jtag_gpio_oen (212)
	#define mask_of_padtop1_reg_jtag_gpio_oen (0xf)
	unsigned int reg_jtag_gpio_oen:4;

	// h006a, bit: 13
	/* reg_MIPI_TX_GPIO_OEN*/
	#define offset_of_padtop1_reg_mipi_tx_gpio_oen (212)
	#define mask_of_padtop1_reg_mipi_tx_gpio_oen (0x3ff0)
	unsigned int reg_mipi_tx_gpio_oen:10;

	// h006a, bit: 14
	/* */
	unsigned int :2;

	// h006a
	unsigned int /* padding 16 bit */:16;

	// h006b, bit: 6
	/* reg_MISC_I2S_GPIO_OEN*/
	#define offset_of_padtop1_reg_misc_i2s_gpio_oen (214)
	#define mask_of_padtop1_reg_misc_i2s_gpio_oen (0x7f)
	unsigned int reg_misc_i2s_gpio_oen:7;

	// h006b, bit: 8
	/* reg_PWM_GPIO_OEN*/
	#define offset_of_padtop1_reg_pwm_gpio_oen (214)
	#define mask_of_padtop1_reg_pwm_gpio_oen (0x180)
	unsigned int reg_pwm_gpio_oen:2;

	// h006b, bit: 11
	/* */
	unsigned int :3;

	// h006b, bit: 12
	/* reg_SPDIF_GPIO_OEN*/
	#define offset_of_padtop1_reg_spdif_gpio_oen (214)
	#define mask_of_padtop1_reg_spdif_gpio_oen (0x1000)
	unsigned int reg_spdif_gpio_oen:1;

	// h006b, bit: 14
	/* */
	unsigned int :3;

	// h006b
	unsigned int /* padding 16 bit */:16;

	// h006c, bit: 14
	/* reg_NAND_GPIO_OEN*/
	#define offset_of_padtop1_reg_nand_gpio_oen (216)
	#define mask_of_padtop1_reg_nand_gpio_oen (0xffff)
	unsigned int reg_nand_gpio_oen:16;

	// h006c
	unsigned int /* padding 16 bit */:16;

	// h006d, bit: 13
	/* reg_RGM0_GPIO_OEN*/
	#define offset_of_padtop1_reg_rgm0_gpio_oen (218)
	#define mask_of_padtop1_reg_rgm0_gpio_oen (0x3fff)
	unsigned int reg_rgm0_gpio_oen:14;

	// h006d, bit: 14
	/* */
	unsigned int :2;

	// h006d
	unsigned int /* padding 16 bit */:16;

	// h006e, bit: 11
	/* reg_RGM1_GPIO_OEN*/
	#define offset_of_padtop1_reg_rgm1_gpio_oen (220)
	#define mask_of_padtop1_reg_rgm1_gpio_oen (0xfff)
	unsigned int reg_rgm1_gpio_oen:12;

	// h006e, bit: 14
	/* */
	unsigned int :4;

	// h006e
	unsigned int /* padding 16 bit */:16;

	// h006f, bit: 5
	/* reg_SD_GPIO_OEN*/
	#define offset_of_padtop1_reg_sd_gpio_oen (222)
	#define mask_of_padtop1_reg_sd_gpio_oen (0x3f)
	unsigned int reg_sd_gpio_oen:6;

	// h006f, bit: 7
	/* */
	unsigned int :2;

	// h006f, bit: 13
	/* reg_SD_IO_GPIO_OEN*/
	#define offset_of_padtop1_reg_sd_io_gpio_oen (222)
	#define mask_of_padtop1_reg_sd_io_gpio_oen (0x3f00)
	unsigned int reg_sd_io_gpio_oen:6;

	// h006f, bit: 14
	/* */
	unsigned int :2;

	// h006f
	unsigned int /* padding 16 bit */:16;

	// h0070, bit: 14
	/* reg_SNR0_GPIO_OEN*/
	#define offset_of_padtop1_reg_snr0_gpio_oen (224)
	#define mask_of_padtop1_reg_snr0_gpio_oen (0xffff)
	unsigned int reg_snr0_gpio_oen:16;

	// h0070
	unsigned int /* padding 16 bit */:16;

	// h0071, bit: 1
	/* reg_SNR0_GPIO_OEN*/
	#define offset_of_padtop1_reg_snr0_gpio_oen_1 (226)
	#define mask_of_padtop1_reg_snr0_gpio_oen_1 (0x3)
	unsigned int reg_snr0_gpio_oen_1:2;

	// h0071, bit: 14
	/* */
	unsigned int :14;

	// h0071
	unsigned int /* padding 16 bit */:16;

	// h0072, bit: 14
	/* reg_SNR1_GPIO_OEN*/
	#define offset_of_padtop1_reg_snr1_gpio_oen (228)
	#define mask_of_padtop1_reg_snr1_gpio_oen (0xffff)
	unsigned int reg_snr1_gpio_oen:16;

	// h0072
	unsigned int /* padding 16 bit */:16;

	// h0073, bit: 1
	/* reg_SNR1_GPIO_OEN*/
	#define offset_of_padtop1_reg_snr1_gpio_oen_1 (230)
	#define mask_of_padtop1_reg_snr1_gpio_oen_1 (0x3)
	unsigned int reg_snr1_gpio_oen_1:2;

	// h0073, bit: 14
	/* */
	unsigned int :14;

	// h0073
	unsigned int /* padding 16 bit */:16;

	// h0074, bit: 14
	/* reg_SNR2_GPIO_OEN*/
	#define offset_of_padtop1_reg_snr2_gpio_oen (232)
	#define mask_of_padtop1_reg_snr2_gpio_oen (0xffff)
	unsigned int reg_snr2_gpio_oen:16;

	// h0074
	unsigned int /* padding 16 bit */:16;

	// h0075, bit: 1
	/* reg_SNR2_GPIO_OEN*/
	#define offset_of_padtop1_reg_snr2_gpio_oen_1 (234)
	#define mask_of_padtop1_reg_snr2_gpio_oen_1 (0x3)
	unsigned int reg_snr2_gpio_oen_1:2;

	// h0075, bit: 14
	/* */
	unsigned int :14;

	// h0075
	unsigned int /* padding 16 bit */:16;

	// h0076, bit: 14
	/* reg_SNR3_GPIO_OEN*/
	#define offset_of_padtop1_reg_snr3_gpio_oen (236)
	#define mask_of_padtop1_reg_snr3_gpio_oen (0xffff)
	unsigned int reg_snr3_gpio_oen:16;

	// h0076
	unsigned int /* padding 16 bit */:16;

	// h0077, bit: 1
	/* reg_SNR3_GPIO_OEN*/
	#define offset_of_padtop1_reg_snr3_gpio_oen_1 (238)
	#define mask_of_padtop1_reg_snr3_gpio_oen_1 (0x3)
	unsigned int reg_snr3_gpio_oen_1:2;

	// h0077, bit: 14
	/* */
	unsigned int :14;

	// h0077
	unsigned int /* padding 16 bit */:16;

	// h0078, bit: 3
	/* reg_SPI0_GPIO_OEN*/
	#define offset_of_padtop1_reg_spi0_gpio_oen (240)
	#define mask_of_padtop1_reg_spi0_gpio_oen (0xf)
	unsigned int reg_spi0_gpio_oen:4;

	// h0078, bit: 7
	/* reg_SPI1_GPIO_OEN*/
	#define offset_of_padtop1_reg_spi1_gpio_oen (240)
	#define mask_of_padtop1_reg_spi1_gpio_oen (0xf0)
	unsigned int reg_spi1_gpio_oen:4;

	// h0078, bit: 11
	/* reg_SPI2_GPIO_OEN*/
	#define offset_of_padtop1_reg_spi2_gpio_oen (240)
	#define mask_of_padtop1_reg_spi2_gpio_oen (0xf00)
	unsigned int reg_spi2_gpio_oen:4;

	// h0078, bit: 13
	/* reg_SYNC_GPIO_OEN*/
	#define offset_of_padtop1_reg_sync_gpio_oen (240)
	#define mask_of_padtop1_reg_sync_gpio_oen (0x3000)
	unsigned int reg_sync_gpio_oen:2;

	// h0078, bit: 14
	/* */
	unsigned int :2;

	// h0078
	unsigned int /* padding 16 bit */:16;

	// h0079, bit: 14
	/* reg_TTL_GPIO_OEN*/
	#define offset_of_padtop1_reg_ttl_gpio_oen (242)
	#define mask_of_padtop1_reg_ttl_gpio_oen (0xffff)
	unsigned int reg_ttl_gpio_oen:16;

	// h0079
	unsigned int /* padding 16 bit */:16;

	// h007a, bit: 14
	/* reg_TTL_GPIO_OEN*/
	#define offset_of_padtop1_reg_ttl_gpio_oen_1 (244)
	#define mask_of_padtop1_reg_ttl_gpio_oen_1 (0x7fff)
	unsigned int reg_ttl_gpio_oen_1:15;

	// h007a, bit: 15
	/* */
	unsigned int :1;

	// h007a
	unsigned int /* padding 16 bit */:16;

	// h007b, bit: 5
	/* reg_UART_GPIO_OEN*/
	#define offset_of_padtop1_reg_uart_gpio_oen (246)
	#define mask_of_padtop1_reg_uart_gpio_oen (0x3f)
	unsigned int reg_uart_gpio_oen:6;

	// h007b, bit: 14
	/* */
	unsigned int :10;

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

}  __attribute__((packed, aligned(1))) infinity2_reg_padtop1;
#endif
