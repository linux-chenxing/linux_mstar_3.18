// Generate Time: 2017-09-25 22:58:56.198877
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
#ifndef __INFINITY2_REG_ISP0__
#define __INFINITY2_REG_ISP0__
typedef struct {
	// h0000, bit: 0
	/* Enable ISP Engine
	# 1’b0: Disable and Reset to the Initial State
	# 1’b1: Enable
	 ( double buffer register )*/
	#define offset_of_isp0_reg_en_isp (0)
	#define mask_of_isp0_reg_en_isp (0x1)
	unsigned int reg_en_isp:1;

	// h0000, bit: 1
	/* Enable Sensor
	# 1'b0: Disable
	# 1'b1: Enable
	 ( double buffer register )*/
	#define offset_of_isp0_reg_en_sensor (0)
	#define mask_of_isp0_reg_en_sensor (0x2)
	unsigned int reg_en_sensor:1;

	// h0000, bit: 2
	/* Enable Optical Black Correction
	# 1'b0: Disable
	# 1'b1: Enable
	( double buffer register )*/
	#define offset_of_isp0_reg_en_isp_obc (0)
	#define mask_of_isp0_reg_en_isp_obc (0x4)
	unsigned int reg_en_isp_obc:1;

	// h0000, bit: 3
	/* Enable White Balance (before Denoise)
	# 1'b0: Disable
	# 1'b1: Enable
	 ( double buffer register )*/
	#define offset_of_isp0_reg_en_isp_wbg0 (0)
	#define mask_of_isp0_reg_en_isp_wbg0 (0x8)
	unsigned int reg_en_isp_wbg0:1;

	// h0000, bit: 4
	/* Enable White Balance (after Denoise)
	# 1'b0: Disable
	# 1'b1: Enable
	 ( double buffer register )*/
	#define offset_of_isp0_reg_en_isp_wbg1 (0)
	#define mask_of_isp0_reg_en_isp_wbg1 (0x10)
	unsigned int reg_en_isp_wbg1:1;

	// h0000, bit: 5
	/* */
	unsigned int :1;

	// h0000, bit: 6
	/* Enable ISP Pipe1 Engine (for clock)
	# 1b0: Disable and Reset to the Initial State
	# 1b1: Enable
	 ( double buffer register )*/
	#define offset_of_isp0_reg_en_isp_p1 (0)
	#define mask_of_isp0_reg_en_isp_p1 (0x40)
	unsigned int reg_en_isp_p1:1;

	// h0000, bit: 7
	/* Enable ISP Pipe2 Engine (for clock)
	# 1b0: Disable and Reset to the Initial State
	# 1b1: Enable
	 ( double buffer register )*/
	#define offset_of_isp0_reg_en_isp_p2 (0)
	#define mask_of_isp0_reg_en_isp_p2 (0x80)
	unsigned int reg_en_isp_p2:1;

	// h0000, bit: 8
	/* */
	unsigned int :1;

	// h0000, bit: 9
	/* Enable HDR Engine (for clock)
	# 1b0: Disable and Reset to the Initial State
	# 1b1: Enable
	 ( double buffer register )*/
	#define offset_of_isp0_reg_en_isp_hdr (0)
	#define mask_of_isp0_reg_en_isp_hdr (0x200)
	unsigned int reg_en_isp_hdr:1;

	// h0000, bit: 14
	/* */
	unsigned int :6;

	// h0000
	unsigned int /* padding 16 bit */:16;

	// h0001, bit: 0
	/* Software Reset for ISP
	# Low Active
	 ( single buffer register )*/
	#define offset_of_isp0_reg_isp_sw_rstz (2)
	#define mask_of_isp0_reg_isp_sw_rstz (0x1)
	unsigned int reg_isp_sw_rstz:1;

	// h0001, bit: 1
	/* Software Reset for ISP_P1
	# Low Active
	 ( single buffer register )*/
	#define offset_of_isp0_reg_isp_sw_p1_rstz (2)
	#define mask_of_isp0_reg_isp_sw_p1_rstz (0x2)
	unsigned int reg_isp_sw_p1_rstz:1;

	// h0001, bit: 2
	/* Software Reset for ISP_P2
	# Low Active
	 ( single buffer register )*/
	#define offset_of_isp0_reg_isp_sw_p2_rstz (2)
	#define mask_of_isp0_reg_isp_sw_p2_rstz (0x4)
	unsigned int reg_isp_sw_p2_rstz:1;

	// h0001, bit: 3
	/* Software Reset for ISP_P3
	# Low Active
	 ( single buffer register )*/
	#define offset_of_isp0_reg_isp_sw_p3_rstz (2)
	#define mask_of_isp0_reg_isp_sw_p3_rstz (0x8)
	unsigned int reg_isp_sw_p3_rstz:1;

	// h0001, bit: 4
	/* Enable Lens Shading Compensation
	# 1'b0: Disable
	# 1'b1: Enable
	 (double buffer register )*/
	#define offset_of_isp0_reg_en_isp_lsc (2)
	#define mask_of_isp0_reg_en_isp_lsc (0x10)
	unsigned int reg_en_isp_lsc:1;

	// h0001, bit: 6
	/* */
	unsigned int :2;

	// h0001, bit: 7
	/* Enable Denoise
	# 1'b0: Disable
	# 1'b1: Enable
	 ( dobule buffer register )*/
	#define offset_of_isp0_reg_en_isp_denoise (2)
	#define mask_of_isp0_reg_en_isp_denoise (0x80)
	unsigned int reg_en_isp_denoise:1;

	// h0001, bit: 11
	/* */
	unsigned int :4;

	// h0001, bit: 12
	/* Enable isp2pad_sr_clk
	# 1'b0: Disable
	# 1'b1: Enable
	 ( single buffer register )
	(no-used)*/
	#define offset_of_isp0_reg_en_sensor_clk (2)
	#define mask_of_isp0_reg_en_sensor_clk (0x1000)
	unsigned int reg_en_sensor_clk:1;

	// h0001, bit: 14
	/* */
	unsigned int :2;

	// h0001, bit: 15
	/* Always Load Double Buffer Register (Type 2 )
	 ( single buffer register )*/
	#define offset_of_isp0_reg_load_reg (2)
	#define mask_of_isp0_reg_load_reg (0x8000)
	unsigned int reg_load_reg:1;

	// h0001
	unsigned int /* padding 16 bit */:16;

	// h0002, bit: 13
	/* */
	unsigned int :14;

	// h0002, bit: 14
	/* double buffer register set done at batch mode*/
	#define offset_of_isp0_reg_db_batch_done (4)
	#define mask_of_isp0_reg_db_batch_done (0x4000)
	unsigned int reg_db_batch_done:1;

	// h0002, bit: 15
	/* double buffer register batch mode*/
	#define offset_of_isp0_reg_db_batch_mode (4)
	#define mask_of_isp0_reg_db_batch_mode (0x8000)
	unsigned int reg_db_batch_mode:1;

	// h0002
	unsigned int /* padding 16 bit */:16;

	// h0003, bit: 3
	/* [2] b3_atv_fix*/
	#define offset_of_isp0_reg_fire_mode (6)
	#define mask_of_isp0_reg_fire_mode (0xf)
	unsigned int reg_fire_mode:4;

	// h0003, bit: 7
	/* */
	unsigned int :4;

	// h0003, bit: 8
	/* reset sensor 1*/
	#define offset_of_isp0_reg_sensor1_rst (6)
	#define mask_of_isp0_reg_sensor1_rst (0x100)
	unsigned int reg_sensor1_rst:1;

	// h0003, bit: 9
	/* power down sensor 1*/
	#define offset_of_isp0_reg_sensor1_pwrdn (6)
	#define mask_of_isp0_reg_sensor1_pwrdn (0x200)
	unsigned int reg_sensor1_pwrdn:1;

	// h0003, bit: 10
	/* reset sensor 2*/
	#define offset_of_isp0_reg_sensor2_rst (6)
	#define mask_of_isp0_reg_sensor2_rst (0x400)
	unsigned int reg_sensor2_rst:1;

	// h0003, bit: 11
	/* power down sensor 2*/
	#define offset_of_isp0_reg_sensor2_pwrdn (6)
	#define mask_of_isp0_reg_sensor2_pwrdn (0x800)
	unsigned int reg_sensor2_pwrdn:1;

	// h0003, bit: 14
	/* */
	unsigned int :4;

	// h0003
	unsigned int /* padding 16 bit */:16;

	// h0004, bit: 0
	/* isp_dp source mux
	# 1'b0: from sensor_if
	# 1'b1: from RDMA*/
	#define offset_of_isp0_reg_isp_if_rmux_sel (8)
	#define mask_of_isp0_reg_isp_if_rmux_sel (0x1)
	unsigned int reg_isp_if_rmux_sel:1;

	// h0004, bit: 3
	/* */
	unsigned int :3;

	// h0004, bit: 6
	/* WDMA source mux
	# 3'd0: from sensor_if
	# 3'd1: from isp_dp output
	# 3'd2: from isp_dp down-sample
	# 3'd3: from isp_dp output
	# 3'd4: from video stabilization
	# 3'd7: from RGB-IR I data*/
	#define offset_of_isp0_reg_isp_if_wmux_sel (8)
	#define mask_of_isp0_reg_isp_if_wmux_sel (0x70)
	unsigned int reg_isp_if_wmux_sel:3;

	// h0004, bit: 7
	/* */
	unsigned int :1;

	// h0004, bit: 8
	/* isp_dp source mux
	# 1'b0: from rmux
	# 1'b1: from sensor freerun vs*/
	#define offset_of_isp0_reg_isp_if_rvs_sel (8)
	#define mask_of_isp0_reg_isp_if_rvs_sel (0x100)
	unsigned int reg_isp_if_rvs_sel:1;

	// h0004, bit: 9
	/* ICP ack tie 1 enable*/
	#define offset_of_isp0_reg_isp_icp_ack_tie1 (8)
	#define mask_of_isp0_reg_isp_icp_ack_tie1 (0x200)
	unsigned int reg_isp_icp_ack_tie1:1;

	// h0004, bit: 14
	/* */
	unsigned int :6;

	// h0004
	unsigned int /* padding 16 bit */:16;

	// h0005, bit: 0
	/* isp_if source mux:
	# 1'b0: from sensor
	# 1'b1: from csi*/
	#define offset_of_isp0_reg_isp_if_src_sel (10)
	#define mask_of_isp0_reg_isp_if_src_sel (0x1)
	unsigned int reg_isp_if_src_sel:1;

	// h0005, bit: 1
	/* Isp to cmdq vsync select
	# 1'b0: DVP mode, from vsync pad
	# 1'b1: use isp re-generate vsync*/
	#define offset_of_isp0_reg_isp2cmdq_vs_sel (10)
	#define mask_of_isp0_reg_isp2cmdq_vs_sel (0x2)
	unsigned int reg_isp2cmdq_vs_sel:1;

	// h0005, bit: 3
	/* */
	unsigned int :2;

	// h0005, bit: 4
	/* ISP FIFO handshaking mode enable*/
	#define offset_of_isp0_reg_isp_if_hs_mode_en (10)
	#define mask_of_isp0_reg_isp_if_hs_mode_en (0x10)
	unsigned int reg_isp_if_hs_mode_en:1;

	// h0005, bit: 7
	/* */
	unsigned int :3;

	// h0005, bit: 9
	/* isp_dp crop virtual channel selection*/
	#define offset_of_isp0_reg_isp_if_dp_vc (10)
	#define mask_of_isp0_reg_isp_if_dp_vc (0x300)
	unsigned int reg_isp_if_dp_vc:2;

	// h0005, bit: 11
	/* */
	unsigned int :2;

	// h0005, bit: 13
	/* WDMA crop virtual channel selection*/
	#define offset_of_isp0_reg_isp_if_wdma_vc (10)
	#define mask_of_isp0_reg_isp_if_wdma_vc (0x3000)
	unsigned int reg_isp_if_wdma_vc:2;

	// h0005, bit: 14
	/* */
	unsigned int :2;

	// h0005
	unsigned int /* padding 16 bit */:16;

	// h0006, bit: 0
	/* WBG LSC ALSC source select
	0: from Gamma16to16
	1: from ISP gain*/
	#define offset_of_isp0_reg_isp_wbg_lsc_alsc_src_sel (12)
	#define mask_of_isp0_reg_isp_wbg_lsc_alsc_src_sel (0x1)
	unsigned int reg_isp_wbg_lsc_alsc_src_sel:1;

	// h0006, bit: 14
	/* */
	unsigned int :15;

	// h0006
	unsigned int /* padding 16 bit */:16;

	// h0007, bit: 0
	/* Mask sensor/csi ready to ISP_IF*/
	#define offset_of_isp0_reg_isp_sensor_mask (14)
	#define mask_of_isp0_reg_isp_sensor_mask (0x1)
	unsigned int reg_isp_sensor_mask:1;

	// h0007, bit: 1
	/* Ready only mode*/
	#define offset_of_isp0_reg_isp_if_de_mode (14)
	#define mask_of_isp0_reg_isp_if_de_mode (0x2)
	unsigned int reg_isp_if_de_mode:1;

	// h0007, bit: 2
	/* Ready only mode*/
	#define offset_of_isp0_reg_isp_if_de_mode_g01 (14)
	#define mask_of_isp0_reg_isp_if_de_mode_g01 (0x4)
	unsigned int reg_isp_if_de_mode_g01:1;

	// h0007, bit: 3
	/* Ready only mode*/
	#define offset_of_isp0_reg_isp_if_de_mode_g02 (14)
	#define mask_of_isp0_reg_isp_if_de_mode_g02 (0x8)
	unsigned int reg_isp_if_de_mode_g02:1;

	// h0007, bit: 4
	/* Ready only mode*/
	#define offset_of_isp0_reg_isp_if_de_mode_g03 (14)
	#define mask_of_isp0_reg_isp_if_de_mode_g03 (0x10)
	unsigned int reg_isp_if_de_mode_g03:1;

	// h0007, bit: 6
	/* fifo mode shared select
	00: 1024 *4
	01: 2048*2
	10: 4096*1*/
	#define offset_of_isp0_reg_isp_fifo_mode (14)
	#define mask_of_isp0_reg_isp_fifo_mode (0x60)
	unsigned int reg_isp_fifo_mode:2;

	// h0007, bit: 7
	/* 444 to 422 store mode */
	#define offset_of_isp0_reg_42to40_store_en (14)
	#define mask_of_isp0_reg_42to40_store_en (0x80)
	unsigned int reg_42to40_store_en:1;

	// h0007, bit: 10
	/* reg_data_store_src_sel
	3'd0: pipe0 ALSC output
	3'd1: HDR stitch output
	3'd2: HDR output
	3'd3: pipe1 ALSC output
	If no enable wdma to do data store, the src sel need to set 3'd4~3'd7*/
	#define offset_of_isp0_reg_data_store_src_sel (14)
	#define mask_of_isp0_reg_data_store_src_sel (0x700)
	unsigned int reg_data_store_src_sel:3;

	// h0007, bit: 11
	/* data fetch mode enable*/
	#define offset_of_isp0_reg_fetch_mode_en (14)
	#define mask_of_isp0_reg_fetch_mode_en (0x800)
	unsigned int reg_fetch_mode_en:1;

	// h0007, bit: 13
	/* data fetch mode source select
	2'd1: fetch to hdr gamma mapping
	2'd2: fetch to DPC*/
	#define offset_of_isp0_reg_fetch_mode_src_sel (14)
	#define mask_of_isp0_reg_fetch_mode_src_sel (0x3000)
	unsigned int reg_fetch_mode_src_sel:2;

	// h0007, bit: 14
	/* 420 to 422 dma mode data input enable*/
	#define offset_of_isp0_reg_40to42_dma_en (14)
	#define mask_of_isp0_reg_40to42_dma_en (0x4000)
	unsigned int reg_40to42_dma_en:1;

	// h0007, bit: 15
	/* Shared Menuload MIU port mux select
	1'b0: Menuload
	1'b1: ROT DMA*/
	#define offset_of_isp0_reg_mload_miu_sel (14)
	#define mask_of_isp0_reg_mload_miu_sel (0x8000)
	unsigned int reg_mload_miu_sel:1;

	// h0007
	unsigned int /* padding 16 bit */:16;

	// h0008, bit: 0
	/* pipe0 head raw store mode
	# 0: afifo feed to 4-to-1 module
	# 1: afifo feed to DRAM (raw store)*/
	#define offset_of_isp0_reg_p0h_rawstore_mode (16)
	#define mask_of_isp0_reg_p0h_rawstore_mode (0x1)
	unsigned int reg_p0h_rawstore_mode:1;

	// h0008, bit: 1
	/* pipe0 head raw fetch mode
	# 0: 4-to-1 feed data to decompress
	# 1: DRAM feed data to decompress (raw fetch)*/
	#define offset_of_isp0_reg_p0h_rawfetch_mode (16)
	#define mask_of_isp0_reg_p0h_rawfetch_mode (0x2)
	unsigned int reg_p0h_rawfetch_mode:1;

	// h0008, bit: 2
	/* pipe0 tail raw store mode
	# 0: White-Blance-Gain feed to HDR module
	# 1: White-Blance-Gain feed to DRAM (raw store)*/
	#define offset_of_isp0_reg_p0t_rawstore_mode (16)
	#define mask_of_isp0_reg_p0t_rawstore_mode (0x4)
	unsigned int reg_p0t_rawstore_mode:1;

	// h0008, bit: 3
	/* pipe0 tail raw fetch mode
	# 0: White-Balance-Gain feed data to HDR
	# 1: DRAM feed data to HDR (raw fetch)*/
	#define offset_of_isp0_reg_p0t_rawfetch_mode (16)
	#define mask_of_isp0_reg_p0t_rawfetch_mode (0x8)
	unsigned int reg_p0t_rawfetch_mode:1;

	// h0008, bit: 4
	/* pipe1 head raw store mode
	# 0: afifo feed to 4-to-1 module
	# 1: afifo feed to DRAM (raw store)*/
	#define offset_of_isp0_reg_p1h_rawstore_mode (16)
	#define mask_of_isp0_reg_p1h_rawstore_mode (0x10)
	unsigned int reg_p1h_rawstore_mode:1;

	// h0008, bit: 5
	/* pipe1 head raw fetch mode
	# 0: 4-to-1 feed data to decompress
	# 1: DRAM feed data to decompress (raw fetch)*/
	#define offset_of_isp0_reg_p1h_rawfetch_mode (16)
	#define mask_of_isp0_reg_p1h_rawfetch_mode (0x20)
	unsigned int reg_p1h_rawfetch_mode:1;

	// h0008, bit: 6
	/* pipe1 tail raw store mode
	# 0: White-Blance-Gain feed to HDR module
	# 1: White-Blance-Gain feed to DRAM (raw store)*/
	#define offset_of_isp0_reg_p1t_rawstore_mode (16)
	#define mask_of_isp0_reg_p1t_rawstore_mode (0x40)
	unsigned int reg_p1t_rawstore_mode:1;

	// h0008, bit: 7
	/* pipe1 tail raw fetch mode
	# 0: White-Balance-Gain feed data to HDR
	# 1: DRAM feed data to HDR (raw fetch)*/
	#define offset_of_isp0_reg_p1t_rawfetch_mode (16)
	#define mask_of_isp0_reg_p1t_rawfetch_mode (0x80)
	unsigned int reg_p1t_rawfetch_mode:1;

	// h0008, bit: 8
	/* pipe2 head raw store mode
	# 0: afifo feed to 4-to-1 module
	# 1: afifo feed to DRAM (raw store)*/
	#define offset_of_isp0_reg_p2h_rawstore_mode (16)
	#define mask_of_isp0_reg_p2h_rawstore_mode (0x100)
	unsigned int reg_p2h_rawstore_mode:1;

	// h0008, bit: 9
	/* pipe2 head raw fetch mode
	# 0: 4-to-1 feed data to decompress
	# 1: DRAM feed data to decompress (raw fetch)*/
	#define offset_of_isp0_reg_p2h_rawfetch_mode (16)
	#define mask_of_isp0_reg_p2h_rawfetch_mode (0x200)
	unsigned int reg_p2h_rawfetch_mode:1;

	// h0008, bit: 10
	/* pipe2 tail raw store mode
	# 0: White-Blance-Gain feed to HDR module
	# 1: White-Blance-Gain feed to DRAM (raw store)*/
	#define offset_of_isp0_reg_p2t_rawstore_mode (16)
	#define mask_of_isp0_reg_p2t_rawstore_mode (0x400)
	unsigned int reg_p2t_rawstore_mode:1;

	// h0008, bit: 11
	/* pipe2 tail raw fetch mode
	# 0: White-Balance-Gain feed data to HDR
	# 1: DRAM feed data to HDR (raw fetch)*/
	#define offset_of_isp0_reg_p2t_rawfetch_mode (16)
	#define mask_of_isp0_reg_p2t_rawfetch_mode (0x800)
	unsigned int reg_p2t_rawfetch_mode:1;

	// h0008, bit: 12
	/* pipe3 head raw store mode
	# 0: afifo feed to 4-to-1 module
	# 1: afifo feed to DRAM (raw store)*/
	#define offset_of_isp0_reg_p3h_rawstore_mode (16)
	#define mask_of_isp0_reg_p3h_rawstore_mode (0x1000)
	unsigned int reg_p3h_rawstore_mode:1;

	// h0008, bit: 13
	/* pipe3 head raw fetch mode
	# 0: 4-to-1 feed data to decompress
	# 1: DRAM feed data to decompress (raw fetch)*/
	#define offset_of_isp0_reg_p3h_rawfetch_mode (16)
	#define mask_of_isp0_reg_p3h_rawfetch_mode (0x2000)
	unsigned int reg_p3h_rawfetch_mode:1;

	// h0008, bit: 14
	/* pipe3 tail raw store mode
	# 0: White-Blance-Gain feed to HDR module
	# 1: White-Blance-Gain feed to DRAM (raw store)*/
	#define offset_of_isp0_reg_p3t_rawstore_mode (16)
	#define mask_of_isp0_reg_p3t_rawstore_mode (0x4000)
	unsigned int reg_p3t_rawstore_mode:1;

	// h0008, bit: 15
	/* pipe3 tail raw fetch mode
	# 0: White-Balance-Gain feed data to HDR
	# 1: DRAM feed data to HDR (raw fetch)*/
	#define offset_of_isp0_reg_p3t_rawfetch_mode (16)
	#define mask_of_isp0_reg_p3t_rawfetch_mode (0x8000)
	unsigned int reg_p3t_rawfetch_mode:1;

	// h0008
	unsigned int /* padding 16 bit */:16;

	// h0009, bit: 7
	/* */
	#define offset_of_isp0_reg_sw_rst_mode (18)
	#define mask_of_isp0_reg_sw_rst_mode (0xff)
	unsigned int reg_sw_rst_mode:8;

	// h0009, bit: 14
	/* */
	unsigned int :8;

	// h0009
	unsigned int /* padding 16 bit */:16;

	// h000a, bit: 14
	/* isp_bist_fail_read*/
	#define offset_of_isp0_reg_isp_bist_fail_read (20)
	#define mask_of_isp0_reg_isp_bist_fail_read (0xffff)
	unsigned int reg_isp_bist_fail_read:16;

	// h000a
	unsigned int /* padding 16 bit */:16;

	// h000b, bit: 14
	/* isp_bist_fail_read*/
	#define offset_of_isp0_reg_isp_bist_fail_read_1 (22)
	#define mask_of_isp0_reg_isp_bist_fail_read_1 (0xffff)
	unsigned int reg_isp_bist_fail_read_1:16;

	// h000b
	unsigned int /* padding 16 bit */:16;

	// h000c, bit: 14
	/* isp_bist_fail_read*/
	#define offset_of_isp0_reg_isp_bist_fail_read_2 (24)
	#define mask_of_isp0_reg_isp_bist_fail_read_2 (0xffff)
	unsigned int reg_isp_bist_fail_read_2:16;

	// h000c
	unsigned int /* padding 16 bit */:16;

	// h000d, bit: 14
	/* isp_bist_fail_read*/
	#define offset_of_isp0_reg_isp_bist_fail_read_3 (26)
	#define mask_of_isp0_reg_isp_bist_fail_read_3 (0xffff)
	unsigned int reg_isp_bist_fail_read_3:16;

	// h000d
	unsigned int /* padding 16 bit */:16;

	// h000e, bit: 14
	/* isp_bist_fail_read*/
	#define offset_of_isp0_reg_isp_bist_fail_read_4 (28)
	#define mask_of_isp0_reg_isp_bist_fail_read_4 (0xffff)
	unsigned int reg_isp_bist_fail_read_4:16;

	// h000e
	unsigned int /* padding 16 bit */:16;

	// h000f, bit: 14
	/* isp_bist_fail_read*/
	#define offset_of_isp0_reg_isp_bist_fail_read_5 (30)
	#define mask_of_isp0_reg_isp_bist_fail_read_5 (0xffff)
	unsigned int reg_isp_bist_fail_read_5:16;

	// h000f
	unsigned int /* padding 16 bit */:16;

	// h0010, bit: 0
	/* Master Mode Definition between ISP and Sensor
	# 1'b0: Sensor: clk_sensor --> ISP: clk_sensor
	# 1'b1: ISP: clk_sensor --> Sensor: clk_sensor
	 ( single buffer register )
	(no-used)*/
	#define offset_of_isp0_reg_isp_master_mode (32)
	#define mask_of_isp0_reg_isp_master_mode (0x1)
	unsigned int reg_isp_master_mode:1;

	// h0010, bit: 1
	/* Sensor Clock Polarity
	# 1'b0: positive edge trigger
	# 1'b1: negative edge trigger
	 ( single buffer register )*/
	#define offset_of_isp0_reg_isp_clk_sensor_polarity (32)
	#define mask_of_isp0_reg_isp_clk_sensor_polarity (0x2)
	unsigned int reg_isp_clk_sensor_polarity:1;

	// h0010, bit: 3
	/* */
	unsigned int :2;

	// h0010, bit: 5
	/* Delay Unit for CLK_Sensor
	# 2'b00: Delay 1 unit
	# 2'b01: Delay 2 unit
	# 2'b10: Delay 3 unit
	# 2'b11: Delay 4 unit
	 ( single buffer register )*/
	#define offset_of_isp0_reg_isp_clk_sensor_delay (32)
	#define mask_of_isp0_reg_isp_clk_sensor_delay (0x30)
	unsigned int reg_isp_clk_sensor_delay:2;

	// h0010, bit: 7
	/* */
	unsigned int :2;

	// h0010, bit: 8
	/* 1: valid 4 pixel data pak at msb to wdma*/
	#define offset_of_isp0_reg_isp_dma_pk_msb (32)
	#define mask_of_isp0_reg_isp_dma_pk_msb (0x100)
	unsigned int reg_isp_dma_pk_msb:1;

	// h0010, bit: 9
	/* */
	unsigned int :1;

	// h0010, bit: 11
	/* Sensor Format
	# 2'b00: 8 bit
	# 2'b01: 10bit
	# 2'b10: 16bit
	# 2'b11: 12bit*/
	#define offset_of_isp0_reg_isp_sensor_format (32)
	#define mask_of_isp0_reg_isp_sensor_format (0xc00)
	unsigned int reg_isp_sensor_format:2;

	// h0010, bit: 12
	/* Sensor Input Format
	# 1'b0: YUV 422 format
	# 1'b1: RGB pattern
	 ( single buffer register )*/
	#define offset_of_isp0_reg_isp_sensor_rgb_in (32)
	#define mask_of_isp0_reg_isp_sensor_rgb_in (0x1000)
	unsigned int reg_isp_sensor_rgb_in:1;

	// h0010, bit: 13
	/* */
	unsigned int :1;

	// h0010, bit: 14
	/* Sensor 8 bit to 12 bit mode
	# 2'b00: {[7:0], [7:4]}
	# 2'b01: {[7:0], 4'b0}
	# 2'b10: {[11:4], [11:8]}
	# 2'b11: {[11:4], 4'b0}
	Sensor 10 bit to 12 bit mode
	# 2'b00: {[9:0], [9:8]}
	# 2'b01: {[9:0], 2'b0}
	# 2'b10: {[11:2], [11:10]}
	# 2'b11: {[11:2], 2'b0}*/
	#define offset_of_isp0_reg_isp_sensor_format_ext_mode (32)
	#define mask_of_isp0_reg_isp_sensor_format_ext_mode (0xc000)
	unsigned int reg_isp_sensor_format_ext_mode:2;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 1
	/* RGB Raw Data Organization
	# 2'h0: [R G R G ...; G B G B ...]
	# 2'h1: [G R G R ...; B G B G ...]
	# 2'h2: [B G B G ...; G R G R ...]
	# 2'h3: [G B G B ...; R G R G ...]
	 ( double buffer register )*/
	#define offset_of_isp0_reg_isp_sensor_array (34)
	#define mask_of_isp0_reg_isp_sensor_array (0x3)
	unsigned int reg_isp_sensor_array:2;

	// h0011, bit: 3
	/* */
	unsigned int :2;

	// h0011, bit: 4
	/* Sensor Input Priority as YUV input
	# 1'b0: [ C Y C Y ...]
	# 1'b1: [ Y C Y C ...]
	 ( double buffer register )*/
	#define offset_of_isp0_reg_isp_sensor_yuv_order (34)
	#define mask_of_isp0_reg_isp_sensor_yuv_order (0x10)
	unsigned int reg_isp_sensor_yuv_order:1;

	// h0011, bit: 5
	/* */
	unsigned int :1;

	// h0011, bit: 6
	/* Sensor Input Format
	# 1'b0: separate Y/C mode
	# 1'b1: YC 16bit mode*/
	#define offset_of_isp0_reg_isp_sensor_yc16bit (34)
	#define mask_of_isp0_reg_isp_sensor_yc16bit (0x40)
	unsigned int reg_isp_sensor_yc16bit:1;

	// h0011, bit: 14
	/* */
	unsigned int :9;

	// h0011
	unsigned int /* padding 16 bit */:16;

	// h0012, bit: 12
	/* X Start Position for Crop Window
	If Sensor is YUV input, this setting must be even
	 ( double buffer register )*/
	#define offset_of_isp0_reg_isp_crop_start_x (36)
	#define mask_of_isp0_reg_isp_crop_start_x (0x1fff)
	unsigned int reg_isp_crop_start_x:13;

	// h0012, bit: 14
	/* */
	unsigned int :3;

	// h0012
	unsigned int /* padding 16 bit */:16;

	// h0013, bit: 12
	/* Y Start Position for Crop Window
	 ( double buffer register )*/
	#define offset_of_isp0_reg_isp_crop_start_y (38)
	#define mask_of_isp0_reg_isp_crop_start_y (0x1fff)
	unsigned int reg_isp_crop_start_y:13;

	// h0013, bit: 14
	/* */
	unsigned int :3;

	// h0013
	unsigned int /* padding 16 bit */:16;

	// h0014, bit: 12
	/* Crop Window Width
	If Sensor is YUV input, this setting must be odd (0 base)
	 ( double buffer register )*/
	#define offset_of_isp0_reg_isp_crop_width (40)
	#define mask_of_isp0_reg_isp_crop_width (0x1fff)
	unsigned int reg_isp_crop_width:13;

	// h0014, bit: 14
	/* */
	unsigned int :3;

	// h0014
	unsigned int /* padding 16 bit */:16;

	// h0015, bit: 12
	/* Crop Window High
	 ( double buffer register )*/
	#define offset_of_isp0_reg_isp_crop_high (42)
	#define mask_of_isp0_reg_isp_crop_high (0x1fff)
	unsigned int reg_isp_crop_high:13;

	// h0015, bit: 14
	/* */
	unsigned int :3;

	// h0015
	unsigned int /* padding 16 bit */:16;

	// h0016, bit: 12
	/* WDMA X Start Position for Crop Window
	If Sensor is YUV input, this setting must be even
	 ( double buffer register )*/
	#define offset_of_isp0_reg_isp_wdma_crop_start_x (44)
	#define mask_of_isp0_reg_isp_wdma_crop_start_x (0x1fff)
	unsigned int reg_isp_wdma_crop_start_x:13;

	// h0016, bit: 14
	/* */
	unsigned int :3;

	// h0016
	unsigned int /* padding 16 bit */:16;

	// h0017, bit: 12
	/* WDMA Y Start Position for Crop Window
	 ( double buffer register )*/
	#define offset_of_isp0_reg_isp_wdma_crop_start_y (46)
	#define mask_of_isp0_reg_isp_wdma_crop_start_y (0x1fff)
	unsigned int reg_isp_wdma_crop_start_y:13;

	// h0017, bit: 14
	/* */
	unsigned int :3;

	// h0017
	unsigned int /* padding 16 bit */:16;

	// h0018, bit: 14
	/* WDMA Crop Window Width
	If Sensor is YUV input, this setting must be odd (0 base)
	 ( double buffer register )*/
	#define offset_of_isp0_reg_isp_wdma_crop_width (48)
	#define mask_of_isp0_reg_isp_wdma_crop_width (0xffff)
	unsigned int reg_isp_wdma_crop_width:16;

	// h0018
	unsigned int /* padding 16 bit */:16;

	// h0019, bit: 12
	/* WDMA Crop Window High (maximum = 2448)
	 ( double buffer register )*/
	#define offset_of_isp0_reg_isp_wdma_crop_high (50)
	#define mask_of_isp0_reg_isp_wdma_crop_high (0x1fff)
	unsigned int reg_isp_wdma_crop_high:13;

	// h0019, bit: 14
	/* */
	unsigned int :3;

	// h0019
	unsigned int /* padding 16 bit */:16;

	// h001a, bit: 0
	/* 1'b1 : enable , rdma_width = crop_width + crop startx
	1'b0 : disable, rdma_width = crop_width*/
	#define offset_of_isp0_reg_isp_rdma_cpx_enable (52)
	#define mask_of_isp0_reg_isp_rdma_cpx_enable (0x1)
	unsigned int reg_isp_rdma_cpx_enable:1;

	// h001a, bit: 14
	/* */
	unsigned int :15;

	// h001a
	unsigned int /* padding 16 bit */:16;

	// h001b, bit: 2
	/* RGB-IR format
	#3'd0: R0
	#3'd1: G0
	#3'd2: B0
	#3'd3: G1
	#3'd4: G2
	#3'd5: I0
	#3'd6: G3
	#3'd7: I1*/
	#define offset_of_isp0_reg_isp_rgbir_format (54)
	#define mask_of_isp0_reg_isp_rgbir_format (0x7)
	unsigned int reg_isp_rgbir_format:3;

	// h001b, bit: 14
	/* */
	unsigned int :13;

	// h001b
	unsigned int /* padding 16 bit */:16;

	// h001c, bit: 14
	/* 32 bits for sw used (can't be reset)*/
	#define offset_of_isp0_reg_isp_sw_used (56)
	#define mask_of_isp0_reg_isp_sw_used (0xffff)
	unsigned int reg_isp_sw_used:16;

	// h001c
	unsigned int /* padding 16 bit */:16;

	// h001d, bit: 14
	/* 32 bits for sw used (can't be reset)*/
	#define offset_of_isp0_reg_isp_sw_used_1 (58)
	#define mask_of_isp0_reg_isp_sw_used_1 (0xffff)
	unsigned int reg_isp_sw_used_1:16;

	// h001d
	unsigned int /* padding 16 bit */:16;

	// h001e, bit: 14
	/* isp debug out*/
	#define offset_of_isp0_reg_isp_debug (60)
	#define mask_of_isp0_reg_isp_debug (0xffff)
	unsigned int reg_isp_debug:16;

	// h001e
	unsigned int /* padding 16 bit */:16;

	// h001f, bit: 7
	/* isp debug out*/
	#define offset_of_isp0_reg_isp_debug_1 (62)
	#define mask_of_isp0_reg_isp_debug_1 (0xff)
	unsigned int reg_isp_debug_1:8;

	// h001f, bit: 10
	/* */
	unsigned int :3;

	// h001f, bit: 11
	/* Hardware Debug Signal Sets Selector :
	0 : from ISP_TOP
	1 : from ISP_DP*/
	#define offset_of_isp0_reg_isp_debug_mux (62)
	#define mask_of_isp0_reg_isp_debug_mux (0x800)
	unsigned int reg_isp_debug_mux:1;

	// h001f, bit: 14
	/* Hardware Debug Signal Sets Selector*/
	#define offset_of_isp0_reg_isp_debug_sel (62)
	#define mask_of_isp0_reg_isp_debug_sel (0xf000)
	unsigned int reg_isp_debug_sel:4;

	// h001f
	unsigned int /* padding 16 bit */:16;

	// h0020, bit: 12
	/* Sensor Line Counter*/
	#define offset_of_isp0_reg_isp_sensor_line_cnt (64)
	#define mask_of_isp0_reg_isp_sensor_line_cnt (0x1fff)
	unsigned int reg_isp_sensor_line_cnt:13;

	// h0020, bit: 14
	/* */
	unsigned int :3;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 0
	/* ISP Status*/
	#define offset_of_isp0_reg_isp_busy (66)
	#define mask_of_isp0_reg_isp_busy (0x1)
	unsigned int reg_isp_busy:1;

	// h0021, bit: 3
	/* */
	unsigned int :3;

	// h0021, bit: 4
	/* ISP P0 input vsync*/
	#define offset_of_isp0_reg_isp_p0_vsync (66)
	#define mask_of_isp0_reg_isp_p0_vsync (0x10)
	unsigned int reg_isp_p0_vsync:1;

	// h0021, bit: 5
	/* ISP P0 input hsync*/
	#define offset_of_isp0_reg_isp_p0_hsync (66)
	#define mask_of_isp0_reg_isp_p0_hsync (0x20)
	unsigned int reg_isp_p0_hsync:1;

	// h0021, bit: 14
	/* */
	unsigned int :9;

	// h0021, bit: 15
	/* Internal FIFO between Sensor and ISP full*/
	#define offset_of_isp0_reg_isp_fifo_ful (66)
	#define mask_of_isp0_reg_isp_fifo_ful (0x8000)
	unsigned int reg_isp_fifo_ful:1;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 14
	/* isp_bist_fail_read */
	#define offset_of_isp0_reg_isp_bist_fail_read_ext (68)
	#define mask_of_isp0_reg_isp_bist_fail_read_ext (0xffff)
	unsigned int reg_isp_bist_fail_read_ext:16;

	// h0022
	unsigned int /* padding 16 bit */:16;

	// h0023, bit: 11
	/* */
	unsigned int :12;

	// h0023, bit: 12
	/* Polarity for isp_debug_clk
	# 1'b0: normal
	# 1'b1: invert clock*/
	#define offset_of_isp0_reg_isp_fpga_clk_polarity (70)
	#define mask_of_isp0_reg_isp_fpga_clk_polarity (0x1000)
	unsigned int reg_isp_fpga_clk_polarity:1;

	// h0023, bit: 14
	/* */
	unsigned int :3;

	// h0023
	unsigned int /* padding 16 bit */:16;

	// h0024, bit: 3
	/* */
	unsigned int :4;

	// h0024, bit: 4
	/* Capture Enable (only for FPGA Debug Mode)
	# 1'b0: disable
	# 1'b1: enable*/
	#define offset_of_isp0_reg_capture_en (72)
	#define mask_of_isp0_reg_capture_en (0x10)
	unsigned int reg_capture_en:1;

	// h0024, bit: 5
	/* Read Data Priority for FPGA operating
	# 1'b0: normal
	# 1'b1: inverse*/
	#define offset_of_isp0_reg_fpga_read_mode (72)
	#define mask_of_isp0_reg_fpga_read_mode (0x20)
	unsigned int reg_fpga_read_mode:1;

	// h0024, bit: 6
	/* Read Data Priority for FPGA operating
	# 1'b0: normal
	# 1'b1: inverse*/
	#define offset_of_isp0_reg_dbgsen_oen (72)
	#define mask_of_isp0_reg_dbgsen_oen (0x40)
	unsigned int reg_dbgsen_oen:1;

	// h0024, bit: 14
	/* */
	unsigned int :9;

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
	/* Interrupt Mask*/
	#define offset_of_isp0_reg_c_irq_mask (80)
	#define mask_of_isp0_reg_c_irq_mask (0xffff)
	unsigned int reg_c_irq_mask:16;

	// h0028
	unsigned int /* padding 16 bit */:16;

	// h0029, bit: 14
	/* Force Interrupt Enable*/
	#define offset_of_isp0_reg_c_irq_force (82)
	#define mask_of_isp0_reg_c_irq_force (0xffff)
	unsigned int reg_c_irq_force:16;

	// h0029
	unsigned int /* padding 16 bit */:16;

	// h002a, bit: 14
	/* Interrupt Clear*/
	#define offset_of_isp0_reg_c_irq_clr (84)
	#define mask_of_isp0_reg_c_irq_clr (0xffff)
	unsigned int reg_c_irq_clr:16;

	// h002a
	unsigned int /* padding 16 bit */:16;

	// h002b, bit: 14
	/* Status of Interrupt on CPU side*/
	#define offset_of_isp0_reg_irq_final_status (86)
	#define mask_of_isp0_reg_irq_final_status (0xffff)
	unsigned int reg_irq_final_status:16;

	// h002b
	unsigned int /* padding 16 bit */:16;

	// h002c, bit: 14
	/* Status of Interrupt on IP side
	# [0] :P0 VIF frame start rising edge
	# [1]: P0 VIF frame start falling edge
	# [2]: YUV422 to 420 fetch done
	# [3] Update double buffer register done
	# [4]: AF Statistic done
	# [5]: ISP internal FIFO full
	# [6]: ISP busy rising edge
	# [7]: ISP busy falling edge
	# [8]: AWB Statistic done
	# [9]: WDMA done
	# [10]: RDMA done
	#[11]: WDMA FIFO full
	#[12]: Pip1 FIFO Full
	#[13]: Pip2 FIFO Full
	#[14]: isp_if vsync pulse
	#[15]: AE Statistic done*/
	#define offset_of_isp0_reg_irq_raw_status (88)
	#define mask_of_isp0_reg_irq_raw_status (0xffff)
	unsigned int reg_irq_raw_status:16;

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

	// h0030, bit: 2
	/* YUV444 to YUV422 filter mode*/
	#define offset_of_isp0_reg_c_filter (96)
	#define mask_of_isp0_reg_c_filter (0x7)
	unsigned int reg_c_filter:3;

	// h0030, bit: 3
	/* reg_ipm_h_mirror*/
	#define offset_of_isp0_reg_ipm_h_mirror (96)
	#define mask_of_isp0_reg_ipm_h_mirror (0x8)
	unsigned int reg_ipm_h_mirror:1;

	// h0030, bit: 4
	/* reg_44to42_dith_en*/
	#define offset_of_isp0_reg_44to42_dith_en (96)
	#define mask_of_isp0_reg_44to42_dith_en (0x10)
	unsigned int reg_44to42_dith_en:1;

	// h0030, bit: 5
	/* reg_rgb2yuv_dith_en*/
	#define offset_of_isp0_reg_rgb2yuv_dith_en (96)
	#define mask_of_isp0_reg_rgb2yuv_dith_en (0x20)
	unsigned int reg_rgb2yuv_dith_en:1;

	// h0030, bit: 14
	/* */
	unsigned int :10;

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

	// h003c, bit: 0
	/* reg_isp_r2y_cmc_en*/
	#define offset_of_isp0_reg_isp_r2y_cmc_en (120)
	#define mask_of_isp0_reg_isp_r2y_cmc_en (0x1)
	unsigned int reg_isp_r2y_cmc_en:1;

	// h003c, bit: 1
	/* reg_isp_r2y_y_sub_16_en*/
	#define offset_of_isp0_reg_isp_r2y_y_sub_16_en (120)
	#define mask_of_isp0_reg_isp_r2y_y_sub_16_en (0x2)
	unsigned int reg_isp_r2y_y_sub_16_en:1;

	// h003c, bit: 2
	/* reg_isp_r2y_r_sub_16_en*/
	#define offset_of_isp0_reg_isp_r2y_r_sub_16_en (120)
	#define mask_of_isp0_reg_isp_r2y_r_sub_16_en (0x4)
	unsigned int reg_isp_r2y_r_sub_16_en:1;

	// h003c, bit: 3
	/* reg_isp_r2y_b_sub_16_en*/
	#define offset_of_isp0_reg_isp_r2y_b_sub_16_en (120)
	#define mask_of_isp0_reg_isp_r2y_b_sub_16_en (0x8)
	unsigned int reg_isp_r2y_b_sub_16_en:1;

	// h003c, bit: 4
	/* reg_isp_r2y_y_add_16_post_en*/
	#define offset_of_isp0_reg_isp_r2y_y_add_16_post_en (120)
	#define mask_of_isp0_reg_isp_r2y_y_add_16_post_en (0x10)
	unsigned int reg_isp_r2y_y_add_16_post_en:1;

	// h003c, bit: 5
	/* reg_isp_r2y_r_add_16_post_en*/
	#define offset_of_isp0_reg_isp_r2y_r_add_16_post_en (120)
	#define mask_of_isp0_reg_isp_r2y_r_add_16_post_en (0x20)
	unsigned int reg_isp_r2y_r_add_16_post_en:1;

	// h003c, bit: 6
	/* reg_isp_r2y_b_add_16_post_en*/
	#define offset_of_isp0_reg_isp_r2y_b_add_16_post_en (120)
	#define mask_of_isp0_reg_isp_r2y_b_add_16_post_en (0x40)
	unsigned int reg_isp_r2y_b_add_16_post_en:1;

	// h003c, bit: 7
	/* reg_isp_r2y_cb_add_128_post_en*/
	#define offset_of_isp0_reg_isp_r2y_cb_add_128_post_en (120)
	#define mask_of_isp0_reg_isp_r2y_cb_add_128_post_en (0x80)
	unsigned int reg_isp_r2y_cb_add_128_post_en:1;

	// h003c, bit: 8
	/* reg_isp_r2y_cr_add_128_post_en*/
	#define offset_of_isp0_reg_isp_r2y_cr_add_128_post_en (120)
	#define mask_of_isp0_reg_isp_r2y_cr_add_128_post_en (0x100)
	unsigned int reg_isp_r2y_cr_add_128_post_en:1;

	// h003c, bit: 9
	/* reg_isp_r2y_rran*/
	#define offset_of_isp0_reg_isp_r2y_rran (120)
	#define mask_of_isp0_reg_isp_r2y_rran (0x200)
	unsigned int reg_isp_r2y_rran:1;

	// h003c, bit: 10
	/* reg_isp_r2y_gran*/
	#define offset_of_isp0_reg_isp_r2y_gran (120)
	#define mask_of_isp0_reg_isp_r2y_gran (0x400)
	unsigned int reg_isp_r2y_gran:1;

	// h003c, bit: 11
	/* reg_isp_r2y_bran*/
	#define offset_of_isp0_reg_isp_r2y_bran (120)
	#define mask_of_isp0_reg_isp_r2y_bran (0x800)
	unsigned int reg_isp_r2y_bran:1;

	// h003c, bit: 14
	/* */
	unsigned int :4;

	// h003c
	unsigned int /* padding 16 bit */:16;

	// h003d, bit: 12
	/* Coefficient 11 for RGB to YUV (s2.8)
	 ( single buffer register )*/
	#define offset_of_isp0_reg_isp_r2y_coeff_11 (122)
	#define mask_of_isp0_reg_isp_r2y_coeff_11 (0x1fff)
	unsigned int reg_isp_r2y_coeff_11:13;

	// h003d, bit: 14
	/* */
	unsigned int :3;

	// h003d
	unsigned int /* padding 16 bit */:16;

	// h003e, bit: 12
	/* Coefficient 12 for RGB to YUV (s2.8)
	 ( single buffer register )*/
	#define offset_of_isp0_reg_isp_r2y_coeff_12 (124)
	#define mask_of_isp0_reg_isp_r2y_coeff_12 (0x1fff)
	unsigned int reg_isp_r2y_coeff_12:13;

	// h003e, bit: 14
	/* */
	unsigned int :3;

	// h003e
	unsigned int /* padding 16 bit */:16;

	// h003f, bit: 12
	/* Coefficient 13 for RGB to YUV (s2.8)
	 ( single buffer register )*/
	#define offset_of_isp0_reg_isp_r2y_coeff_13 (126)
	#define mask_of_isp0_reg_isp_r2y_coeff_13 (0x1fff)
	unsigned int reg_isp_r2y_coeff_13:13;

	// h003f, bit: 14
	/* */
	unsigned int :3;

	// h003f
	unsigned int /* padding 16 bit */:16;

	// h0040, bit: 12
	/* Coefficient 21 for RGB to YUV (s2.8)
	 ( single buffer register )*/
	#define offset_of_isp0_reg_isp_r2y_coeff_21 (128)
	#define mask_of_isp0_reg_isp_r2y_coeff_21 (0x1fff)
	unsigned int reg_isp_r2y_coeff_21:13;

	// h0040, bit: 14
	/* */
	unsigned int :3;

	// h0040
	unsigned int /* padding 16 bit */:16;

	// h0041, bit: 12
	/* Coefficient 22 for RGB to YUV (s2.8)
	 ( single buffer register )*/
	#define offset_of_isp0_reg_isp_r2y_coeff_22 (130)
	#define mask_of_isp0_reg_isp_r2y_coeff_22 (0x1fff)
	unsigned int reg_isp_r2y_coeff_22:13;

	// h0041, bit: 14
	/* */
	unsigned int :3;

	// h0041
	unsigned int /* padding 16 bit */:16;

	// h0042, bit: 12
	/* Coefficient 23 for RGB to YUV (s2.8)
	 ( single buffer register )*/
	#define offset_of_isp0_reg_isp_r2y_coeff_23 (132)
	#define mask_of_isp0_reg_isp_r2y_coeff_23 (0x1fff)
	unsigned int reg_isp_r2y_coeff_23:13;

	// h0042, bit: 14
	/* */
	unsigned int :3;

	// h0042
	unsigned int /* padding 16 bit */:16;

	// h0043, bit: 12
	/* Coefficient 31 for RGB to YUV (s2.8)
	 ( single buffer register )*/
	#define offset_of_isp0_reg_isp_r2y_coeff_31 (134)
	#define mask_of_isp0_reg_isp_r2y_coeff_31 (0x1fff)
	unsigned int reg_isp_r2y_coeff_31:13;

	// h0043, bit: 14
	/* */
	unsigned int :3;

	// h0043
	unsigned int /* padding 16 bit */:16;

	// h0044, bit: 12
	/* Coefficient 32 for RGB to YUV (s2.8)
	 ( single buffer register )*/
	#define offset_of_isp0_reg_isp_r2y_coeff_32 (136)
	#define mask_of_isp0_reg_isp_r2y_coeff_32 (0x1fff)
	unsigned int reg_isp_r2y_coeff_32:13;

	// h0044, bit: 14
	/* */
	unsigned int :3;

	// h0044
	unsigned int /* padding 16 bit */:16;

	// h0045, bit: 12
	/* Coefficient 33 for RGB to YUV (s2.8)
	 ( single buffer register )*/
	#define offset_of_isp0_reg_isp_r2y_coeff_33 (138)
	#define mask_of_isp0_reg_isp_r2y_coeff_33 (0x1fff)
	unsigned int reg_isp_r2y_coeff_33:13;

	// h0045, bit: 14
	/* */
	unsigned int :3;

	// h0045
	unsigned int /* padding 16 bit */:16;

	// h0046, bit: 0
	/* Enable signal for gamma_10to10*/
	#define offset_of_isp0_reg_isp_gamma_10to10_en (140)
	#define mask_of_isp0_reg_isp_gamma_10to10_en (0x1)
	unsigned int reg_isp_gamma_10to10_en:1;

	// h0046, bit: 1
	/* Enable signal for gamma_10to10 gat max*/
	#define offset_of_isp0_reg_isp_gamma_10to10_max_en (140)
	#define mask_of_isp0_reg_isp_gamma_10to10_max_en (0x2)
	unsigned int reg_isp_gamma_10to10_max_en:1;

	// h0046, bit: 11
	/* Enable signal for gamma_10to10 max_data*/
	#define offset_of_isp0_reg_isp_gamma_10to10_max_data (140)
	#define mask_of_isp0_reg_isp_gamma_10to10_max_data (0xffc)
	unsigned int reg_isp_gamma_10to10_max_data:10;

	// h0046, bit: 14
	/* */
	unsigned int :4;

	// h0046
	unsigned int /* padding 16 bit */:16;

	// h0047, bit: 14
	/* */
	unsigned int :16;

	// h0047
	unsigned int /* padding 16 bit */:16;

	// h0048, bit: 14
	/* isp_bist_fail_read */
	#define offset_of_isp0_reg_isp_bist_fail_read_ext1 (144)
	#define mask_of_isp0_reg_isp_bist_fail_read_ext1 (0xffff)
	unsigned int reg_isp_bist_fail_read_ext1:16;

	// h0048
	unsigned int /* padding 16 bit */:16;

	// h0049, bit: 14
	/* isp_bist_fail_read */
	#define offset_of_isp0_reg_isp_bist_fail_read_ext1_1 (146)
	#define mask_of_isp0_reg_isp_bist_fail_read_ext1_1 (0xffff)
	unsigned int reg_isp_bist_fail_read_ext1_1:16;

	// h0049
	unsigned int /* padding 16 bit */:16;

	// h004a, bit: 14
	/* New Interrupt Mask*/
	#define offset_of_isp0_reg_c_irq_mask3 (148)
	#define mask_of_isp0_reg_c_irq_mask3 (0xffff)
	unsigned int reg_c_irq_mask3:16;

	// h004a
	unsigned int /* padding 16 bit */:16;

	// h004b, bit: 14
	/* New Force Interrupt Enable*/
	#define offset_of_isp0_reg_c_irq_force3 (150)
	#define mask_of_isp0_reg_c_irq_force3 (0xffff)
	unsigned int reg_c_irq_force3:16;

	// h004b
	unsigned int /* padding 16 bit */:16;

	// h004c, bit: 14
	/* New Interrupt Clear*/
	#define offset_of_isp0_reg_c_irq_clr3 (152)
	#define mask_of_isp0_reg_c_irq_clr3 (0xffff)
	unsigned int reg_c_irq_clr3:16;

	// h004c
	unsigned int /* padding 16 bit */:16;

	// h004d, bit: 14
	/* New Status of Interrupt on CPU side*/
	#define offset_of_isp0_reg_irq_final_status3 (154)
	#define mask_of_isp0_reg_irq_final_status3 (0xffff)
	unsigned int reg_irq_final_status3:16;

	// h004d
	unsigned int /* padding 16 bit */:16;

	// h004e, bit: 14
	/* New Status of Interrupt on IP side
	#[0]: Histogram window0 statistic done
	#[1]: Histogram window1 statistic done
	#[2]: AE block row count requal req_ae_int_row_num
	#[3]: menuload done
	#[4]: P0 input line count equal reg_sw_specify_int_line
	#[5]: P0 output count equal reg_sw_specify_int_line
	#[6]: hit input line count1
	#[7]: hit input line count2
	#[8]: hit input line count3
	#[9]: P1 input line count equal reg_sw_specify_int_line
	#[10]: rgbir histogram done
	#[11]: awb row done
	#[12]: histogram row done
	#[13]: histogram by pixel done
	#[14]: P2 input line count equal reg_sw_specify_int_line
	#[15]: Reserved*/
	#define offset_of_isp0_reg_irq_raw_status3 (156)
	#define mask_of_isp0_reg_irq_raw_status3 (0xffff)
	unsigned int reg_irq_raw_status3:16;

	// h004e
	unsigned int /* padding 16 bit */:16;

	// h004f, bit: 14
	/* isp_bist_fail_read */
	#define offset_of_isp0_reg_isp_bist_fail_read_ext2 (158)
	#define mask_of_isp0_reg_isp_bist_fail_read_ext2 (0xffff)
	unsigned int reg_isp_bist_fail_read_ext2:16;

	// h004f
	unsigned int /* padding 16 bit */:16;

	// h0050, bit: 14
	/* isp_bist_fail_read */
	#define offset_of_isp0_reg_isp_bist_fail_read_ext2_1 (160)
	#define mask_of_isp0_reg_isp_bist_fail_read_ext2_1 (0xffff)
	unsigned int reg_isp_bist_fail_read_ext2_1:16;

	// h0050
	unsigned int /* padding 16 bit */:16;

	// h0051, bit: 14
	/* isp_bist_fail_read */
	#define offset_of_isp0_reg_isp_bist_fail_read_ext2_2 (162)
	#define mask_of_isp0_reg_isp_bist_fail_read_ext2_2 (0xffff)
	unsigned int reg_isp_bist_fail_read_ext2_2:16;

	// h0051
	unsigned int /* padding 16 bit */:16;

	// h0052, bit: 0
	/* Gamma Correction Dither Enable*/
	#define offset_of_isp0_reg_gamma_c_dith_en (164)
	#define mask_of_isp0_reg_gamma_c_dith_en (0x1)
	unsigned int reg_gamma_c_dith_en:1;

	// h0052, bit: 14
	/* The 35th Gamma Table*/
	#define offset_of_isp0_reg_gamma_ctl_reserved (164)
	#define mask_of_isp0_reg_gamma_ctl_reserved (0xfffe)
	unsigned int reg_gamma_ctl_reserved:15;

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
	/* New Interrupt Mask*/
	#define offset_of_isp0_reg_c_irq_mask2 (172)
	#define mask_of_isp0_reg_c_irq_mask2 (0xffff)
	unsigned int reg_c_irq_mask2:16;

	// h0056
	unsigned int /* padding 16 bit */:16;

	// h0057, bit: 14
	/* New Force Interrupt Enable*/
	#define offset_of_isp0_reg_c_irq_force2 (174)
	#define mask_of_isp0_reg_c_irq_force2 (0xffff)
	unsigned int reg_c_irq_force2:16;

	// h0057
	unsigned int /* padding 16 bit */:16;

	// h0058, bit: 14
	/* New Interrupt Clear*/
	#define offset_of_isp0_reg_c_irq_clr2 (176)
	#define mask_of_isp0_reg_c_irq_clr2 (0xffff)
	unsigned int reg_c_irq_clr2:16;

	// h0058
	unsigned int /* padding 16 bit */:16;

	// h0059, bit: 14
	/* New Status of Interrupt on CPU side*/
	#define offset_of_isp0_reg_irq_final_status2 (178)
	#define mask_of_isp0_reg_irq_final_status2 (0xffff)
	unsigned int reg_irq_final_status2:16;

	// h0059
	unsigned int /* padding 16 bit */:16;

	// h005a, bit: 14
	/* New Status of Interrupt on IP side
	#[0] wdma0 line count hit
	#[1] wdma1 line count hit
	#[2] wdma2 line count hit
	#[3] Reserved
	#[4] Reserved
	#[5] Reserved
	#[6] wdma6 line count hit
	#[7] rdma0 done
	#[8] rdma1 done
	#[9] Reserved
	#[10] Reserved
	#[11]Reserved
	#[12] Reserved
	#[13] wdma0 done
	#[14] wdma1 done
	#[15] wdma2 done*/
	#define offset_of_isp0_reg_irq_raw_status2 (180)
	#define mask_of_isp0_reg_irq_raw_status2 (0xffff)
	unsigned int reg_irq_raw_status2:16;

	// h005a
	unsigned int /* padding 16 bit */:16;

	// h005b, bit: 14
	/* New Interrupt Mask*/
	#define offset_of_isp0_reg_c_irq_mask4 (182)
	#define mask_of_isp0_reg_c_irq_mask4 (0xffff)
	unsigned int reg_c_irq_mask4:16;

	// h005b
	unsigned int /* padding 16 bit */:16;

	// h005c, bit: 14
	/* New Force Interrupt Enable*/
	#define offset_of_isp0_reg_c_irq_force4 (184)
	#define mask_of_isp0_reg_c_irq_force4 (0xffff)
	unsigned int reg_c_irq_force4:16;

	// h005c
	unsigned int /* padding 16 bit */:16;

	// h005d, bit: 14
	/* New Interrupt Clear*/
	#define offset_of_isp0_reg_c_irq_clr4 (186)
	#define mask_of_isp0_reg_c_irq_clr4 (0xffff)
	unsigned int reg_c_irq_clr4:16;

	// h005d
	unsigned int /* padding 16 bit */:16;

	// h005e, bit: 14
	/* New Status of Interrupt on CPU side*/
	#define offset_of_isp0_reg_irq_final_status4 (188)
	#define mask_of_isp0_reg_irq_final_status4 (0xffff)
	unsigned int reg_irq_final_status4:16;

	// h005e
	unsigned int /* padding 16 bit */:16;

	// h005f, bit: 14
	/* New Status of Interrupt on IP side
	#[0]: Reserved
	#[1]: Reserved
	#[2]: Reserved
	#[3]: P1 VIF frame start rising edge
	#[4]: P1 VIF frame start falling edge
	#[5]: P2 VIF frame start rising edge
	#[6]: P2 VIF frame start falling edge
	#[7]: hdr array hit count
	#[8]: hdr mapping hit count
	#[9]: hdr line buffer countrol done
	#[10]: hdr frame done
	#[11]: CI frame done
	#[12]: hdr histogram done
	#[13]:  P1 hit input line count1
	#[14]:  P1 hit input line count2
	#[15]:  P1 hit input line count3*/
	#define offset_of_isp0_reg_irq_raw_status4 (190)
	#define mask_of_isp0_reg_irq_raw_status4 (0xffff)
	unsigned int reg_irq_raw_status4:16;

	// h005f
	unsigned int /* padding 16 bit */:16;

	// h0060, bit: 0
	/* */
	#define offset_of_isp0_reg_edge_enable (192)
	#define mask_of_isp0_reg_edge_enable (0x1)
	unsigned int reg_edge_enable:1;

	// h0060, bit: 14
	/* */
	unsigned int :15;

	// h0060
	unsigned int /* padding 16 bit */:16;

	// h0061, bit: 11
	/* blank space for m5x5 window engine for the last two lines*/
	#define offset_of_isp0_reg_blank_space (194)
	#define mask_of_isp0_reg_blank_space (0xfff)
	unsigned int reg_blank_space:12;

	// h0061, bit: 14
	/* */
	unsigned int :4;

	// h0061
	unsigned int /* padding 16 bit */:16;

	// h0062, bit: 0
	/* no-used*/
	#define offset_of_isp0_reg_rdpath_en (196)
	#define mask_of_isp0_reg_rdpath_en (0x1)
	unsigned int reg_rdpath_en:1;

	// h0062, bit: 1
	/* no-used*/
	#define offset_of_isp0_reg_wrpath_en (196)
	#define mask_of_isp0_reg_wrpath_en (0x2)
	unsigned int reg_wrpath_en:1;

	// h0062, bit: 14
	/* */
	unsigned int :14;

	// h0062
	unsigned int /* padding 16 bit */:16;

	// h0063, bit: 14
	/* REV for m3isp*/
	#define offset_of_isp0_reg_reserved1_m3isp (198)
	#define mask_of_isp0_reg_reserved1_m3isp (0xffff)
	unsigned int reg_reserved1_m3isp:16;

	// h0063
	unsigned int /* padding 16 bit */:16;

	// h0064, bit: 0
	/* */
	unsigned int :1;

	// h0064, bit: 1
	/* no-used*/
	#define offset_of_isp0_reg_isp_dbgw_en (200)
	#define mask_of_isp0_reg_isp_dbgw_en (0x2)
	unsigned int reg_isp_dbgw_en:1;

	// h0064, bit: 2
	/* */
	#define offset_of_isp0_reg_isp_dp_gated_clk_en (200)
	#define mask_of_isp0_reg_isp_dp_gated_clk_en (0x4)
	unsigned int reg_isp_dp_gated_clk_en:1;

	// h0064, bit: 3
	/* denoise function*/
	#define offset_of_isp0_reg_m3isp_rawdn_gated_clk_en (200)
	#define mask_of_isp0_reg_m3isp_rawdn_gated_clk_en (0x8)
	unsigned int reg_m3isp_rawdn_gated_clk_en:1;

	// h0064, bit: 4
	/* no-used*/
	#define offset_of_isp0_reg_m3isp_kernel_gated_clk_en (200)
	#define mask_of_isp0_reg_m3isp_kernel_gated_clk_en (0x10)
	unsigned int reg_m3isp_kernel_gated_clk_en:1;

	// h0064, bit: 5
	/* no-used*/
	#define offset_of_isp0_reg_isp_dp_gated_clk_mode (200)
	#define mask_of_isp0_reg_isp_dp_gated_clk_mode (0x20)
	unsigned int reg_isp_dp_gated_clk_mode:1;

	// h0064, bit: 6
	/* */
	unsigned int :1;

	// h0064, bit: 7
	/* 0: vsync double buffer
	1: frame end double buffer*/
	#define offset_of_isp0_reg_latch_mode (200)
	#define mask_of_isp0_reg_latch_mode (0x80)
	unsigned int reg_latch_mode:1;

	// h0064, bit: 9
	/* */
	unsigned int :2;

	// h0064, bit: 10
	/* 0: normal mode, riu can launch to double buffer
	1: lock mode, riu can NOT launch to double buffer*/
	#define offset_of_isp0_reg_lock_mode (200)
	#define mask_of_isp0_reg_lock_mode (0x400)
	unsigned int reg_lock_mode:1;

	// h0064, bit: 11
	/* */
	#define offset_of_isp0_reg_isp_hdr_gated_clk_en (200)
	#define mask_of_isp0_reg_isp_hdr_gated_clk_en (0x800)
	unsigned int reg_isp_hdr_gated_clk_en:1;

	// h0064, bit: 14
	/* */
	unsigned int :4;

	// h0064
	unsigned int /* padding 16 bit */:16;

	// h0065, bit: 0
	/* group0 rdma reset*/
	#define offset_of_isp0_reg_rdpath_swrst (202)
	#define mask_of_isp0_reg_rdpath_swrst (0x1)
	unsigned int reg_rdpath_swrst:1;

	// h0065, bit: 1
	/* group0 wdma reset*/
	#define offset_of_isp0_reg_wrpath_swrst (202)
	#define mask_of_isp0_reg_wrpath_swrst (0x2)
	unsigned int reg_wrpath_swrst:1;

	// h0065, bit: 2
	/* Group0 wdma/rdma sw reset*/
	#define offset_of_isp0_reg_dbgwr_swrst (202)
	#define mask_of_isp0_reg_dbgwr_swrst (0x4)
	unsigned int reg_dbgwr_swrst:1;

	// h0065, bit: 3
	/* Mask mload rst when sw rst enable
	0 : no mask (default)
	1 : mask*/
	#define offset_of_isp0_reg_mask_mload_mif_rst (202)
	#define mask_of_isp0_reg_mask_mload_mif_rst (0x8)
	unsigned int reg_mask_mload_mif_rst:1;

	// h0065, bit: 4
	/* group1rdma reset*/
	#define offset_of_isp0_reg_rdpath1_swrst (202)
	#define mask_of_isp0_reg_rdpath1_swrst (0x10)
	unsigned int reg_rdpath1_swrst:1;

	// h0065, bit: 5
	/* group1 wdma reset*/
	#define offset_of_isp0_reg_wrpath1_swrst (202)
	#define mask_of_isp0_reg_wrpath1_swrst (0x20)
	unsigned int reg_wrpath1_swrst:1;

	// h0065, bit: 6
	/* Group1 wdma/rdma sw reset*/
	#define offset_of_isp0_reg_dbgwr1_swrst (202)
	#define mask_of_isp0_reg_dbgwr1_swrst (0x40)
	unsigned int reg_dbgwr1_swrst:1;

	// h0065, bit: 7
	/* group2 rdma reset*/
	#define offset_of_isp0_reg_rdpath2_swrst (202)
	#define mask_of_isp0_reg_rdpath2_swrst (0x80)
	unsigned int reg_rdpath2_swrst:1;

	// h0065, bit: 8
	/* group2 wdma reset*/
	#define offset_of_isp0_reg_wrpath2_swrst (202)
	#define mask_of_isp0_reg_wrpath2_swrst (0x100)
	unsigned int reg_wrpath2_swrst:1;

	// h0065, bit: 9
	/* Group2 wdma/rdma sw reset*/
	#define offset_of_isp0_reg_dbgwr2_swrst (202)
	#define mask_of_isp0_reg_dbgwr2_swrst (0x200)
	unsigned int reg_dbgwr2_swrst:1;

	// h0065, bit: 10
	/* group3  rdma reset*/
	#define offset_of_isp0_reg_rdpath3_swrst (202)
	#define mask_of_isp0_reg_rdpath3_swrst (0x400)
	unsigned int reg_rdpath3_swrst:1;

	// h0065, bit: 11
	/* group3 wdma reset*/
	#define offset_of_isp0_reg_wrpath3_swrst (202)
	#define mask_of_isp0_reg_wrpath3_swrst (0x800)
	unsigned int reg_wrpath3_swrst:1;

	// h0065, bit: 12
	/* Group3 wdma/rdma sw reset*/
	#define offset_of_isp0_reg_dbgwr3_swrst (202)
	#define mask_of_isp0_reg_dbgwr3_swrst (0x1000)
	unsigned int reg_dbgwr3_swrst:1;

	// h0065, bit: 13
	/* DMA4/DMA6 rdma reset*/
	#define offset_of_isp0_reg_rdpath4_swrst (202)
	#define mask_of_isp0_reg_rdpath4_swrst (0x2000)
	unsigned int reg_rdpath4_swrst:1;

	// h0065, bit: 14
	/* DMA4/DMA6 wdma reset*/
	#define offset_of_isp0_reg_wrpath4_swrst (202)
	#define mask_of_isp0_reg_wrpath4_swrst (0x4000)
	unsigned int reg_wrpath4_swrst:1;

	// h0065, bit: 15
	/* DMA4/DMA6 wdma/rdma sw reset*/
	#define offset_of_isp0_reg_dbgwr4_swrst (202)
	#define mask_of_isp0_reg_dbgwr4_swrst (0x8000)
	unsigned int reg_dbgwr4_swrst:1;

	// h0065
	unsigned int /* padding 16 bit */:16;

	// h0066, bit: 0
	/* 0: isp_dp reset by vsync
	1: isp_dp not reset by vsync*/
	#define offset_of_isp0_reg_isp_dp_rstz_mode (204)
	#define mask_of_isp0_reg_isp_dp_rstz_mode (0x1)
	unsigned int reg_isp_dp_rstz_mode:1;

	// h0066, bit: 1
	/* rawdn_clk force disable*/
	#define offset_of_isp0_reg_m3isp_rawdn_clk_force_dis (204)
	#define mask_of_isp0_reg_m3isp_rawdn_clk_force_dis (0x2)
	unsigned int reg_m3isp_rawdn_clk_force_dis:1;

	// h0066, bit: 2
	/* isp_dp_clk force disable*/
	#define offset_of_isp0_reg_isp_dp_clk_force_dis (204)
	#define mask_of_isp0_reg_isp_dp_clk_force_dis (0x4)
	unsigned int reg_isp_dp_clk_force_dis:1;

	// h0066, bit: 3
	/* Raw data down sample to DMA enable
	(double buffer register)*/
	#define offset_of_isp0_reg_dspl2wdma_en (204)
	#define mask_of_isp0_reg_dspl2wdma_en (0x8)
	unsigned int reg_dspl2wdma_en:1;

	// h0066, bit: 6
	/* down-sampling mode
	0 : whole image
	1 : 1/4
	2 : 1/16
	3 : 1/64
	4 : 1/256*/
	#define offset_of_isp0_reg_dsp_mode (204)
	#define mask_of_isp0_reg_dsp_mode (0x70)
	unsigned int reg_dsp_mode:3;

	// h0066, bit: 7
	/* 0: Enable isp2icp frame end reset
	1: Disable*/
	#define offset_of_isp0_reg_fend_rstz_mode (204)
	#define mask_of_isp0_reg_fend_rstz_mode (0x80)
	unsigned int reg_fend_rstz_mode:1;

	// h0066, bit: 14
	/* */
	unsigned int :8;

	// h0066
	unsigned int /* padding 16 bit */:16;

	// h0067, bit: 14
	/* */
	unsigned int :16;

	// h0067
	unsigned int /* padding 16 bit */:16;

	// h0068, bit: 3
	/* hsync pipe delay for YC mode(atv mode)*/
	#define offset_of_isp0_reg_hs_pipe (208)
	#define mask_of_isp0_reg_hs_pipe (0xf)
	unsigned int reg_hs_pipe:4;

	// h0068, bit: 4
	/* Mask hsync pipe delay for YC mode(atv mode)*/
	#define offset_of_isp0_reg_s2p_hs_mask (208)
	#define mask_of_isp0_reg_s2p_hs_mask (0x10)
	unsigned int reg_s2p_hs_mask:1;

	// h0068, bit: 5
	/* hsync pipe delay polarity for YC mode
	# 1'b0: high active
	# 1'b1: low active*/
	#define offset_of_isp0_reg_s2p_hs_pol (208)
	#define mask_of_isp0_reg_s2p_hs_pol (0x20)
	unsigned int reg_s2p_hs_pol:1;

	// h0068, bit: 14
	/* */
	unsigned int :10;

	// h0068
	unsigned int /* padding 16 bit */:16;

	// h0069, bit: 14
	/* */
	unsigned int :16;

	// h0069
	unsigned int /* padding 16 bit */:16;

	// h006a, bit: 0
	/* Video stabilization enable*/
	#define offset_of_isp0_reg_vdos_en (212)
	#define mask_of_isp0_reg_vdos_en (0x1)
	unsigned int reg_vdos_en:1;

	// h006a, bit: 1
	/* Video stabilization mode:
	0: bayer mode
	1: yc mode*/
	#define offset_of_isp0_reg_vdos_yc_en (212)
	#define mask_of_isp0_reg_vdos_yc_en (0x2)
	unsigned int reg_vdos_yc_en:1;

	// h006a, bit: 3
	/* */
	unsigned int :2;

	// h006a, bit: 4
	/* Video stabilization Y/G first flag
	0: first pix is NOT Y/G
	1: first pix is Y/G*/
	#define offset_of_isp0_reg_vdos_byer_mode (212)
	#define mask_of_isp0_reg_vdos_byer_mode (0x10)
	unsigned int reg_vdos_byer_mode:1;

	// h006a, bit: 7
	/* */
	unsigned int :3;

	// h006a, bit: 8
	/* Video stabilization input select
	0: pipe0
	1: pipe1*/
	#define offset_of_isp0_reg_vdos_input_sel (212)
	#define mask_of_isp0_reg_vdos_input_sel (0x100)
	unsigned int reg_vdos_input_sel:1;

	// h006a, bit: 14
	/* */
	unsigned int :7;

	// h006a
	unsigned int /* padding 16 bit */:16;

	// h006b, bit: 2
	/* Number of blanking lines for sram dump*/
	#define offset_of_isp0_reg_vdos_sblock_blk (214)
	#define mask_of_isp0_reg_vdos_sblock_blk (0x7)
	unsigned int reg_vdos_sblock_blk:3;

	// h006b, bit: 7
	/* */
	unsigned int :5;

	// h006b, bit: 14
	/* */
	#define offset_of_isp0_reg_vdos_dummy (214)
	#define mask_of_isp0_reg_vdos_dummy (0xff00)
	unsigned int reg_vdos_dummy:8;

	// h006b
	unsigned int /* padding 16 bit */:16;

	// h006c, bit: 13
	/* Video stabilization X crop start, > 0*/
	#define offset_of_isp0_reg_vdos_x_st (216)
	#define mask_of_isp0_reg_vdos_x_st (0x3fff)
	unsigned int reg_vdos_x_st:14;

	// h006c, bit: 14
	/* */
	unsigned int :2;

	// h006c
	unsigned int /* padding 16 bit */:16;

	// h006d, bit: 13
	/* Video stabilization Y crop start, > 0*/
	#define offset_of_isp0_reg_vdos_y_st (218)
	#define mask_of_isp0_reg_vdos_y_st (0x3fff)
	unsigned int reg_vdos_y_st:14;

	// h006d, bit: 14
	/* */
	unsigned int :2;

	// h006d
	unsigned int /* padding 16 bit */:16;

	// h006e, bit: 12
	/* Video stabilization sub block width, min =2, max =170*/
	#define offset_of_isp0_reg_vdos_sblock_width (220)
	#define mask_of_isp0_reg_vdos_sblock_width (0x1fff)
	unsigned int reg_vdos_sblock_width:13;

	// h006e, bit: 14
	/* */
	unsigned int :3;

	// h006e
	unsigned int /* padding 16 bit */:16;

	// h006f, bit: 12
	/* Video stabilization sub block height, min =2, max =170*/
	#define offset_of_isp0_reg_vdos_sblock_height (222)
	#define mask_of_isp0_reg_vdos_sblock_height (0x1fff)
	unsigned int reg_vdos_sblock_height:13;

	// h006f, bit: 14
	/* */
	unsigned int :3;

	// h006f
	unsigned int /* padding 16 bit */:16;

	// h0070, bit: 0
	/* Enable CFA Interpolation (Demosaic)
	# 1'b0: Disable
	# 1'b1: Enable
	 ( single buffer register )*/
	#define offset_of_isp0_reg_cfai_en (224)
	#define mask_of_isp0_reg_cfai_en (0x1)
	unsigned int reg_cfai_en:1;

	// h0070, bit: 1
	/* Enable CFAI Data Bypass (output = m33)
	# 1'b0: Disable
	# 1'b1: Enable
	 ( single buffer register )*/
	#define offset_of_isp0_reg_cfai_bypass (224)
	#define mask_of_isp0_reg_cfai_bypass (0x2)
	unsigned int reg_cfai_bypass:1;

	// h0070, bit: 2
	/* demosaic noise reduction average g enable
	0: disable
	1: enable (use average g for blending)*/
	#define offset_of_isp0_reg_cfai_dnr_gavg_ref_en (224)
	#define mask_of_isp0_reg_cfai_dnr_gavg_ref_en (0x4)
	unsigned int reg_cfai_dnr_gavg_ref_en:1;

	// h0070, bit: 3
	/* emosaic noise reduction enable
	0: disable
	1: enable */
	#define offset_of_isp0_reg_cfai_dnr_en (224)
	#define mask_of_isp0_reg_cfai_dnr_en (0x8)
	unsigned int reg_cfai_dnr_en:1;

	// h0070, bit: 7
	/* */
	unsigned int :4;

	// h0070, bit: 10
	/* demosaic noise reduction rb channel blending ratio*/
	#define offset_of_isp0_reg_cfai_dnr_rb_ratio (224)
	#define mask_of_isp0_reg_cfai_dnr_rb_ratio (0x700)
	unsigned int reg_cfai_dnr_rb_ratio:3;

	// h0070, bit: 11
	/* */
	unsigned int :1;

	// h0070, bit: 14
	/* demosaic noise reduction g channel blending ratio*/
	#define offset_of_isp0_reg_cfai_dnr_g_ratio (224)
	#define mask_of_isp0_reg_cfai_dnr_g_ratio (0x7000)
	unsigned int reg_cfai_dnr_g_ratio:3;

	// h0070, bit: 15
	/* */
	unsigned int :1;

	// h0070
	unsigned int /* padding 16 bit */:16;

	// h0071, bit: 3
	/* Demosaic DVH Edge Wieghting Adjust Slope
	(u4)*/
	#define offset_of_isp0_reg_demosaic_dvh_slope (226)
	#define mask_of_isp0_reg_demosaic_dvh_slope (0xf)
	unsigned int reg_demosaic_dvh_slope:4;

	// h0071, bit: 7
	/* */
	unsigned int :4;

	// h0071, bit: 11
	/* Demosaic DVH Edge Wieghting Threshold
	(u4)*/
	#define offset_of_isp0_reg_demosaic_dvh_thrd (226)
	#define mask_of_isp0_reg_demosaic_dvh_thrd (0xf00)
	unsigned int reg_demosaic_dvh_thrd:4;

	// h0071, bit: 14
	/* */
	unsigned int :4;

	// h0071
	unsigned int /* padding 16 bit */:16;

	// h0072, bit: 3
	/* Demosaic Complex Region Wieghting Adjust Slope
	(u0.4)*/
	#define offset_of_isp0_reg_demosaic_cpxwei_slope (228)
	#define mask_of_isp0_reg_demosaic_cpxwei_slope (0xf)
	unsigned int reg_demosaic_cpxwei_slope:4;

	// h0072, bit: 7
	/* */
	unsigned int :4;

	// h0072, bit: 14
	/* Demosaic Complex Region Wieghting Threshold
	(u8)*/
	#define offset_of_isp0_reg_demosaic_cpxwei_thrd (228)
	#define mask_of_isp0_reg_demosaic_cpxwei_thrd (0xff00)
	unsigned int reg_demosaic_cpxwei_thrd:8;

	// h0072
	unsigned int /* padding 16 bit */:16;

	// h0073, bit: 3
	/* Neighbor Check Slope M (u2.2)
	for reduce the false color on high freq. horizontal / vertical line with width 1*/
	#define offset_of_isp0_reg_nb_slope_m (230)
	#define mask_of_isp0_reg_nb_slope_m (0xf)
	unsigned int reg_nb_slope_m:4;

	// h0073, bit: 7
	/* */
	unsigned int :4;

	// h0073, bit: 11
	/* Neighbor Check Slope S (u3.1)
	for reduce the false color on high freq. horizontal / vertical line with width 1*/
	#define offset_of_isp0_reg_nb_slope_s (230)
	#define mask_of_isp0_reg_nb_slope_s (0xf00)
	unsigned int reg_nb_slope_s:4;

	// h0073, bit: 14
	/* */
	unsigned int :4;

	// h0073
	unsigned int /* padding 16 bit */:16;

	// h0074, bit: 0
	/* False color suppression enable
	0: disable
	1: enable */
	#define offset_of_isp0_reg_rgb_falsecolor_en (232)
	#define mask_of_isp0_reg_rgb_falsecolor_en (0x1)
	unsigned int reg_rgb_falsecolor_en:1;

	// h0074, bit: 1
	/* minmax rounding enable*/
	#define offset_of_isp0_reg_rgb_falsecolor_minmax_round (232)
	#define mask_of_isp0_reg_rgb_falsecolor_minmax_round (0x2)
	unsigned int reg_rgb_falsecolor_minmax_round:1;

	// h0074, bit: 3
	/* */
	unsigned int :2;

	// h0074, bit: 6
	/* blending ratio*/
	#define offset_of_isp0_reg_rgb_falsecolor_color_ratio (232)
	#define mask_of_isp0_reg_rgb_falsecolor_color_ratio (0x70)
	unsigned int reg_rgb_falsecolor_color_ratio:3;

	// h0074, bit: 7
	/* */
	unsigned int :1;

	// h0074, bit: 10
	/* blending ratio*/
	#define offset_of_isp0_reg_rgb_falsecolor_r_ratio (232)
	#define mask_of_isp0_reg_rgb_falsecolor_r_ratio (0x700)
	unsigned int reg_rgb_falsecolor_r_ratio:3;

	// h0074, bit: 11
	/* */
	unsigned int :1;

	// h0074, bit: 14
	/* blending ratio*/
	#define offset_of_isp0_reg_rgb_falsecolor_b_ratio (232)
	#define mask_of_isp0_reg_rgb_falsecolor_b_ratio (0x7000)
	unsigned int reg_rgb_falsecolor_b_ratio:3;

	// h0074, bit: 15
	/* */
	unsigned int :1;

	// h0074
	unsigned int /* padding 16 bit */:16;

	// h0075, bit: 6
	/* edge adapt for K offset value*/
	#define offset_of_isp0_reg_edgeadapffset (234)
	#define mask_of_isp0_reg_edgeadapffset (0x7f)
	unsigned int reg_edgeadapffset:7;

	// h0075, bit: 7
	/* */
	unsigned int :1;

	// h0075, bit: 14
	/* cross check for G offset value*/
	#define offset_of_isp0_reg_crosscheckffset (234)
	#define mask_of_isp0_reg_crosscheckffset (0x7f00)
	unsigned int reg_crosscheckffset:7;

	// h0075, bit: 15
	/* */
	unsigned int :1;

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

	// h007a, bit: 6
	/* isp frame start count (vsync)*/
	#define offset_of_isp0_reg_isp_frm_init_cnt (244)
	#define mask_of_isp0_reg_isp_frm_init_cnt (0x7f)
	unsigned int reg_isp_frm_init_cnt:7;

	// h007a, bit: 7
	/* Set 1 to mask misc frame done to isp frame done*/
	#define offset_of_isp0_reg_mask_isp_misc_frame_done (244)
	#define mask_of_isp0_reg_mask_isp_misc_frame_done (0x80)
	unsigned int reg_mask_isp_misc_frame_done:1;

	// h007a, bit: 14
	/* isp frame done count*/
	#define offset_of_isp0_reg_isp_frm_done_cnt (244)
	#define mask_of_isp0_reg_isp_frm_done_cnt (0x7f00)
	unsigned int reg_isp_frm_done_cnt:7;

	// h007a, bit: 15
	/* Clear isp frame init/done count*/
	#define offset_of_isp0_reg_isp_clr_frm_cnt (244)
	#define mask_of_isp0_reg_isp_clr_frm_cnt (0x8000)
	unsigned int reg_isp_clr_frm_cnt:1;

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

	// h007d, bit: 12
	/* Sensor to Crop line count*/
	#define offset_of_isp0_reg_isp_sr2crop_line_cnt (250)
	#define mask_of_isp0_reg_isp_sr2crop_line_cnt (0x1fff)
	unsigned int reg_isp_sr2crop_line_cnt:13;

	// h007d, bit: 14
	/* */
	unsigned int :3;

	// h007d
	unsigned int /* padding 16 bit */:16;

	// h007e, bit: 12
	/* SW specify interrupt line number*/
	#define offset_of_isp0_reg_sw_specify_int_line (252)
	#define mask_of_isp0_reg_sw_specify_int_line (0x1fff)
	unsigned int reg_sw_specify_int_line:13;

	// h007e, bit: 14
	/* */
	unsigned int :3;

	// h007e
	unsigned int /* padding 16 bit */:16;

	// h007f, bit: 7
	/* debug status*/
	#define offset_of_isp0_reg_debug_status_out (254)
	#define mask_of_isp0_reg_debug_status_out (0xff)
	unsigned int reg_debug_status_out:8;

	// h007f, bit: 14
	/* clear debug status*/
	#define offset_of_isp0_reg_debug_status_clr (254)
	#define mask_of_isp0_reg_debug_status_clr (0xff00)
	unsigned int reg_debug_status_clr:8;

	// h007f
	unsigned int /* padding 16 bit */:16;

}  __attribute__((packed, aligned(1))) infinity2_reg_isp0;
#endif
