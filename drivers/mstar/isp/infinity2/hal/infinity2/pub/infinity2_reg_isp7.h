// Generate Time: 2017-09-25 22:58:56.736794
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
#ifndef __INFINITY2_REG_ISP7__
#define __INFINITY2_REG_ISP7__
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

	// h0009, bit: 0
	/* Set 1 for no rotate, 0 for rotate*/
	#define offset_of_isp7_reg_dipr_en (18)
	#define mask_of_isp7_reg_dipr_en (0x1)
	unsigned int reg_dipr_en:1;

	// h0009, bit: 1
	/* dipr software trigger*/
	#define offset_of_isp7_reg_dipr_sw_trig (18)
	#define mask_of_isp7_reg_dipr_sw_trig (0x2)
	unsigned int reg_dipr_sw_trig:1;

	// h0009, bit: 2
	/* 420 linear format for google encoder, Set 1*/
	#define offset_of_isp7_reg_dip_420_linear (18)
	#define mask_of_isp7_reg_dip_420_linear (0x4)
	unsigned int reg_dip_420_linear:1;

	// h0009, bit: 3
	/* 1: enable rotate, 0: no rotate (normal path)*/
	#define offset_of_isp7_reg_rot_422_en (18)
	#define mask_of_isp7_reg_rot_422_en (0x8)
	unsigned int reg_rot_422_en:1;

	// h0009, bit: 4
	/* yuv422 rotate direction
	0: right rotate
	1: left rotate with mirror*/
	#define offset_of_isp7_reg_rot_422_dir (18)
	#define mask_of_isp7_reg_rot_422_dir (0x10)
	unsigned int reg_rot_422_dir:1;

	// h0009, bit: 5
	/* yuv 422 rotate color select for u
	0: select row color
	1: select column color*/
	#define offset_of_isp7_reg_rot_422_u_mode (18)
	#define mask_of_isp7_reg_rot_422_u_mode (0x20)
	unsigned int reg_rot_422_u_mode:1;

	// h0009, bit: 6
	/* yuv 422 rotate color select for v
	0: select row color
	1: select column color*/
	#define offset_of_isp7_reg_rot_422_v_mode (18)
	#define mask_of_isp7_reg_rot_422_v_mode (0x40)
	unsigned int reg_rot_422_v_mode:1;

	// h0009, bit: 7
	/* dip  crop enable*/
	#define offset_of_isp7_reg_dipr_crop_en (18)
	#define mask_of_isp7_reg_dipr_crop_en (0x80)
	unsigned int reg_dipr_crop_en:1;

	// h0009, bit: 8
	/* 1: 3T sync, 0: 4T sync*/
	#define offset_of_isp7_reg_rot_sync_3t_dis (18)
	#define mask_of_isp7_reg_rot_sync_3t_dis (0x100)
	unsigned int reg_rot_sync_3t_dis:1;

	// h0009, bit: 9
	/* 0: normal mode, riu can launch to double buffer
	1: lock mode, riu can NOT launch to double buffer*/
	#define offset_of_isp7_reg_lock_mode (18)
	#define mask_of_isp7_reg_lock_mode (0x200)
	unsigned int reg_lock_mode:1;

	// h0009, bit: 10
	/* Always Load Double Buffer Register (Type 2 )
	 ( single buffer register )*/
	#define offset_of_isp7_reg_load_reg (18)
	#define mask_of_isp7_reg_load_reg (0x400)
	unsigned int reg_load_reg:1;

	// h0009, bit: 11
	/* 1: HW frame control dmag (must enable)*/
	#define offset_of_isp7_reg_hw_rot_en (18)
	#define mask_of_isp7_reg_hw_rot_en (0x800)
	unsigned int reg_hw_rot_en:1;

	// h0009, bit: 14
	/* */
	unsigned int :4;

	// h0009
	unsigned int /* padding 16 bit */:16;

	// h000a, bit: 14
	/* Dip frame buffer base address for y (inc with 32 Byte for no rot, 16 Byte for rot)*/
	#define offset_of_isp7_reg_dipr_base_adr0 (20)
	#define mask_of_isp7_reg_dipr_base_adr0 (0xffff)
	unsigned int reg_dipr_base_adr0:16;

	// h000a
	unsigned int /* padding 16 bit */:16;

	// h000b, bit: 10
	/* Dip frame buffer base address for y (inc with 32 Byte for no rot, 16 Byte for rot)*/
	#define offset_of_isp7_reg_dipr_base_adr0_1 (22)
	#define mask_of_isp7_reg_dipr_base_adr0_1 (0x7ff)
	unsigned int reg_dipr_base_adr0_1:11;

	// h000b, bit: 14
	/* */
	unsigned int :5;

	// h000b
	unsigned int /* padding 16 bit */:16;

	// h000c, bit: 14
	/* Dip frame buffer base address for c  (inc with 32 Byte for no rot, 16 Byte for rot)*/
	#define offset_of_isp7_reg_dipr_base_adr1 (24)
	#define mask_of_isp7_reg_dipr_base_adr1 (0xffff)
	unsigned int reg_dipr_base_adr1:16;

	// h000c
	unsigned int /* padding 16 bit */:16;

	// h000d, bit: 10
	/* Dip frame buffer base address for c  (inc with 32 Byte for no rot, 16 Byte for rot)*/
	#define offset_of_isp7_reg_dipr_base_adr1_1 (26)
	#define mask_of_isp7_reg_dipr_base_adr1_1 (0x7ff)
	unsigned int reg_dipr_base_adr1_1:11;

	// h000d, bit: 14
	/* */
	unsigned int :5;

	// h000d
	unsigned int /* padding 16 bit */:16;

	// h000e, bit: 5
	/* ISP ROT output blanking (mul 2)*/
	#define offset_of_isp7_reg_rot_hblk (28)
	#define mask_of_isp7_reg_rot_hblk (0x3f)
	unsigned int reg_rot_hblk:6;

	// h000e, bit: 14
	/* */
	unsigned int :10;

	// h000e
	unsigned int /* padding 16 bit */:16;

	// h000f, bit: 14
	/* */
	unsigned int :16;

	// h000f
	unsigned int /* padding 16 bit */:16;

	// h0010, bit: 12
	/* dip read frame width (inc with 1 Byte)*/
	#define offset_of_isp7_reg_dipr_frame_width (32)
	#define mask_of_isp7_reg_dipr_frame_width (0x1fff)
	unsigned int reg_dipr_frame_width:13;

	// h0010, bit: 14
	/* */
	unsigned int :3;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 11
	/* dip read frame height (inc with 1 Byte)*/
	#define offset_of_isp7_reg_dipr_frame_height (34)
	#define mask_of_isp7_reg_dipr_frame_height (0xfff)
	unsigned int reg_dipr_frame_height:12;

	// h0011, bit: 14
	/* */
	unsigned int :4;

	// h0011
	unsigned int /* padding 16 bit */:16;

	// h0012, bit: 12
	/* fetch number of one line for dipr (no rotate)*/
	#define offset_of_isp7_reg_dipr_fetch_num (36)
	#define mask_of_isp7_reg_dipr_fetch_num (0x1fff)
	unsigned int reg_dipr_fetch_num:13;

	// h0012, bit: 14
	/* */
	unsigned int :3;

	// h0012
	unsigned int /* padding 16 bit */:16;

	// h0013, bit: 6
	/* request number once for last request (HSIZE/32's factor minus 1 for yuv420 burst length) (for no rotate)*/
	#define offset_of_isp7_reg_dipr_last_req_num_m1 (38)
	#define mask_of_isp7_reg_dipr_last_req_num_m1 (0x7f)
	unsigned int reg_dipr_last_req_num_m1:7;

	// h0013, bit: 7
	/* */
	unsigned int :1;

	// h0013, bit: 14
	/* request number once for 32 Byte MIU (32/32's factor minus 1 for yuv420 burst length)(for no rotate)*/
	#define offset_of_isp7_reg_dipr_req_num_256_m1 (38)
	#define mask_of_isp7_reg_dipr_req_num_256_m1 (0x7f00)
	unsigned int reg_dipr_req_num_256_m1:7;

	// h0013, bit: 15
	/* */
	unsigned int :1;

	// h0013
	unsigned int /* padding 16 bit */:16;

	// h0014, bit: 12
	/* dip crop frame width (inc with 1 Byte)*/
	#define offset_of_isp7_reg_dipr_crop_frame_width (40)
	#define mask_of_isp7_reg_dipr_crop_frame_width (0x1fff)
	unsigned int reg_dipr_crop_frame_width:13;

	// h0014, bit: 14
	/* */
	unsigned int :3;

	// h0014
	unsigned int /* padding 16 bit */:16;

	// h0015, bit: 11
	/* dip crop frame height (inc with 1 Byte)*/
	#define offset_of_isp7_reg_dipr_crop_frame_height (42)
	#define mask_of_isp7_reg_dipr_crop_frame_height (0xfff)
	unsigned int reg_dipr_crop_frame_height:12;

	// h0015, bit: 14
	/* */
	unsigned int :4;

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

	// h0019, bit: 0
	/* mrq vsync reset enable*/
	#define offset_of_isp7_reg_mrq_vs_rst_en (50)
	#define mask_of_isp7_reg_mrq_vs_rst_en (0x1)
	unsigned int reg_mrq_vs_rst_en:1;

	// h0019, bit: 1
	/* save bw mode for miu 256 , must disable*/
	#define offset_of_isp7_reg_sav_bw (50)
	#define mask_of_isp7_reg_sav_bw (0x2)
	unsigned int reg_sav_bw:1;

	// h0019, bit: 2
	/* rreq delta mode enable */
	#define offset_of_isp7_reg_rreq_delta_en (50)
	#define mask_of_isp7_reg_rreq_delta_en (0x4)
	unsigned int reg_rreq_delta_en:1;

	// h0019, bit: 3
	/* evd arbiter */
	#define offset_of_isp7_reg_hp_cut_in_en0 (50)
	#define mask_of_isp7_reg_hp_cut_in_en0 (0x8)
	unsigned int reg_hp_cut_in_en0:1;

	// h0019, bit: 4
	/* evd arbiter */
	#define offset_of_isp7_reg_hp_cut_in_en1 (50)
	#define mask_of_isp7_reg_hp_cut_in_en1 (0x10)
	unsigned int reg_hp_cut_in_en1:1;

	// h0019, bit: 5
	/* dipr depack yc swap*/
	#define offset_of_isp7_reg_dipr_yc_swap (50)
	#define mask_of_isp7_reg_dipr_yc_swap (0x20)
	unsigned int reg_dipr_yc_swap:1;

	// h0019, bit: 6
	/* force rreq thrd equal to reg_dipr_req_thrd*/
	#define offset_of_isp7_reg_dipr_force_rreq_thrd (50)
	#define mask_of_isp7_reg_dipr_force_rreq_thrd (0x40)
	unsigned int reg_dipr_force_rreq_thrd:1;

	// h0019, bit: 7
	/* */
	#define offset_of_isp7_reg_wfifo_src_rlb_r (50)
	#define mask_of_isp7_reg_wfifo_src_rlb_r (0x80)
	unsigned int reg_wfifo_src_rlb_r:1;

	// h0019, bit: 8
	/* */
	#define offset_of_isp7_reg_dipw_src_dpk_r (50)
	#define mask_of_isp7_reg_dipw_src_dpk_r (0x100)
	unsigned int reg_dipw_src_dpk_r:1;

	// h0019, bit: 10
	/* rreq delta mode: 0: div2 1: div4 2: div8 3: div16
	enable while reg_rreq_delta_en is asserted*/
	#define offset_of_isp7_reg_rreq_delta_md (50)
	#define mask_of_isp7_reg_rreq_delta_md (0x600)
	unsigned int reg_rreq_delta_md:2;

	// h0019, bit: 11
	/* dipr hbk enable for osdb*/
	#define offset_of_isp7_reg_mask_hbk (50)
	#define mask_of_isp7_reg_mask_hbk (0x800)
	unsigned int reg_mask_hbk:1;

	// h0019, bit: 13
	/* evd arbiter */
	#define offset_of_isp7_reg_hpmask (50)
	#define mask_of_isp7_reg_hpmask (0x3000)
	unsigned int reg_hpmask:2;

	// h0019, bit: 14
	/* evd arbiter */
	#define offset_of_isp7_reg_req_mask (50)
	#define mask_of_isp7_reg_req_mask (0xc000)
	unsigned int reg_req_mask:2;

	// h0019
	unsigned int /* padding 16 bit */:16;

	// h001a, bit: 0
	/* dipr force frame index, must enable*/
	#define offset_of_isp7_reg_dipr_force_frm_index (52)
	#define mask_of_isp7_reg_dipr_force_frm_index (0x1)
	unsigned int reg_dipr_force_frm_index:1;

	// h001a, bit: 3
	/* force frame index*/
	#define offset_of_isp7_reg_dipr_frm_index (52)
	#define mask_of_isp7_reg_dipr_frm_index (0xe)
	unsigned int reg_dipr_frm_index:3;

	// h001a, bit: 4
	/* sram power down control*/
	#define offset_of_isp7_reg_dip_sram_pd (52)
	#define mask_of_isp7_reg_dip_sram_pd (0x10)
	unsigned int reg_dip_sram_pd:1;

	// h001a, bit: 14
	/* */
	unsigned int :11;

	// h001a
	unsigned int /* padding 16 bit */:16;

	// h001b, bit: 14
	/* v blanking number*/
	#define offset_of_isp7_reg_vbk_num (54)
	#define mask_of_isp7_reg_vbk_num (0xffff)
	unsigned int reg_vbk_num:16;

	// h001b
	unsigned int /* padding 16 bit */:16;

	// h001c, bit: 4
	/* dipr h blanking number*/
	#define offset_of_isp7_reg_mask_hbk_num (56)
	#define mask_of_isp7_reg_mask_hbk_num (0x1f)
	unsigned int reg_mask_hbk_num:5;

	// h001c, bit: 7
	/* */
	unsigned int :3;

	// h001c, bit: 12
	/* high priority threshold for read request*/
	#define offset_of_isp7_reg_dipr_hi_pri_num (56)
	#define mask_of_isp7_reg_dipr_hi_pri_num (0x1f00)
	unsigned int reg_dipr_hi_pri_num:5;

	// h001c, bit: 14
	/* */
	unsigned int :3;

	// h001c
	unsigned int /* padding 16 bit */:16;

	// h001d, bit: 6
	/* read request maximum number*/
	#define offset_of_isp7_reg_dipr_req_max (58)
	#define mask_of_isp7_reg_dipr_req_max (0x7f)
	unsigned int reg_dipr_req_max:7;

	// h001d, bit: 7
	/* */
	unsigned int :1;

	// h001d, bit: 13
	/* rlb empty count threshold for read request*/
	#define offset_of_isp7_reg_dipr_req_thrd (58)
	#define mask_of_isp7_reg_dipr_req_thrd (0x3f00)
	unsigned int reg_dipr_req_thrd:6;

	// h001d, bit: 14
	/* */
	unsigned int :2;

	// h001d
	unsigned int /* padding 16 bit */:16;

	// h001e, bit: 7
	/* evd arbiter */
	#define offset_of_isp7_reg_hp_cut_min0 (60)
	#define mask_of_isp7_reg_hp_cut_min0 (0xff)
	unsigned int reg_hp_cut_min0:8;

	// h001e, bit: 14
	/* evd arbiter */
	#define offset_of_isp7_reg_hp_cut_min1 (60)
	#define mask_of_isp7_reg_hp_cut_min1 (0xff00)
	unsigned int reg_hp_cut_min1:8;

	// h001e
	unsigned int /* padding 16 bit */:16;

	// h001f, bit: 7
	/* evd arbiter*/
	#define offset_of_isp7_reg_max_length_r0 (62)
	#define mask_of_isp7_reg_max_length_r0 (0xff)
	unsigned int reg_max_length_r0:8;

	// h001f, bit: 14
	/* evd arbiter*/
	#define offset_of_isp7_reg_max_length_r1 (62)
	#define mask_of_isp7_reg_max_length_r1 (0xff00)
	unsigned int reg_max_length_r1:8;

	// h001f
	unsigned int /* padding 16 bit */:16;

	// h0020, bit: 7
	/* evd arbiter*/
	#define offset_of_isp7_reg_max_length_all (64)
	#define mask_of_isp7_reg_max_length_all (0xff)
	unsigned int reg_max_length_all:8;

	// h0020, bit: 14
	/* */
	unsigned int :8;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 14
	/* */
	unsigned int :16;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 14
	/* */
	unsigned int :16;

	// h0022
	unsigned int /* padding 16 bit */:16;

	// h0023, bit: 14
	/* */
	unsigned int :16;

	// h0023
	unsigned int /* padding 16 bit */:16;

	// h0024, bit: 14
	/* */
	unsigned int :16;

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

	// h0030, bit: 0
	/* */
	#define offset_of_isp7_reg_arb_vs_rst_en (96)
	#define mask_of_isp7_reg_arb_vs_rst_en (0x1)
	unsigned int reg_arb_vs_rst_en:1;

	// h0030, bit: 1
	/* */
	#define offset_of_isp7_reg_pdw_arb_rdy1_first (96)
	#define mask_of_isp7_reg_pdw_arb_rdy1_first (0x2)
	unsigned int reg_pdw_arb_rdy1_first:1;

	// h0030, bit: 2
	/* */
	#define offset_of_isp7_reg_pdw_arb_flow_ctrl_en (96)
	#define mask_of_isp7_reg_pdw_arb_flow_ctrl_en (0x4)
	unsigned int reg_pdw_arb_flow_ctrl_en:1;

	// h0030, bit: 7
	/* */
	unsigned int :5;

	// h0030, bit: 14
	/* */
	#define offset_of_isp7_reg_pdw_arb_rdy0_thd (96)
	#define mask_of_isp7_reg_pdw_arb_rdy0_thd (0xff00)
	unsigned int reg_pdw_arb_rdy0_thd:8;

	// h0030
	unsigned int /* padding 16 bit */:16;

	// h0031, bit: 7
	/* */
	#define offset_of_isp7_reg_pdw_arb_rdy1_thd (98)
	#define mask_of_isp7_reg_pdw_arb_rdy1_thd (0xff)
	unsigned int reg_pdw_arb_rdy1_thd:8;

	// h0031, bit: 14
	/* */
	#define offset_of_isp7_reg_pdw_arb_max_req_num (98)
	#define mask_of_isp7_reg_pdw_arb_max_req_num (0xff00)
	unsigned int reg_pdw_arb_max_req_num:8;

	// h0031
	unsigned int /* padding 16 bit */:16;

	// h0032, bit: 0
	/* */
	#define offset_of_isp7_reg_pdw_arb_thd_hit_last_en (100)
	#define mask_of_isp7_reg_pdw_arb_thd_hit_last_en (0x1)
	unsigned int reg_pdw_arb_thd_hit_last_en:1;

	// h0032, bit: 1
	/* */
	#define offset_of_isp7_reg_pdw_arb_bypass_en (100)
	#define mask_of_isp7_reg_pdw_arb_bypass_en (0x2)
	unsigned int reg_pdw_arb_bypass_en:1;

	// h0032, bit: 2
	/* */
	#define offset_of_isp7_reg_pdw_arb_sw_rst (100)
	#define mask_of_isp7_reg_pdw_arb_sw_rst (0x4)
	unsigned int reg_pdw_arb_sw_rst:1;

	// h0032, bit: 3
	/* */
	#define offset_of_isp7_reg_dipw_tlb (100)
	#define mask_of_isp7_reg_dipw_tlb (0x8)
	unsigned int reg_dipw_tlb:1;

	// h0032, bit: 4
	/* */
	#define offset_of_isp7_reg_dipr_tlb (100)
	#define mask_of_isp7_reg_dipr_tlb (0x10)
	unsigned int reg_dipr_tlb:1;

	// h0032, bit: 14
	/* */
	unsigned int :11;

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

	// h0035, bit: 0
	/* ISP RDMA enable*/
	#define offset_of_isp7_reg_isp_rdma_en (106)
	#define mask_of_isp7_reg_isp_rdma_en (0x1)
	unsigned int reg_isp_rdma_en:1;

	// h0035, bit: 1
	/* */
	unsigned int :1;

	// h0035, bit: 2
	/* ISP DMA ring buffer mode enable*/
	#define offset_of_isp7_reg_isp_dma_ring_buf_en (106)
	#define mask_of_isp7_reg_isp_dma_ring_buf_en (0x4)
	unsigned int reg_isp_dma_ring_buf_en:1;

	// h0035, bit: 3
	/* */
	unsigned int :1;

	// h0035, bit: 4
	/* ISP RDMA auto vsync trigger mode*/
	#define offset_of_isp7_reg_isp_rdma_auto (106)
	#define mask_of_isp7_reg_isp_rdma_auto (0x10)
	unsigned int reg_isp_rdma_auto:1;

	// h0035, bit: 6
	/* */
	unsigned int :2;

	// h0035, bit: 7
	/* ISP RDMA high priority set*/
	#define offset_of_isp7_reg_isp_rdma_rreq_hpri_set (106)
	#define mask_of_isp7_reg_isp_rdma_rreq_hpri_set (0x80)
	unsigned int reg_isp_rdma_rreq_hpri_set:1;

	// h0035, bit: 9
	/* ISP RDMA depack mode:
	2'd0: 8-bit
	2'd1: 10-bit
	2'd2: 16-bit
	2'd3: 12-bit*/
	#define offset_of_isp7_reg_isp_rdma_mode (106)
	#define mask_of_isp7_reg_isp_rdma_mode (0x300)
	unsigned int reg_isp_rdma_mode:2;

	// h0035, bit: 14
	/* */
	unsigned int :5;

	// h0035, bit: 15
	/* ISP RDMA MIU request reset*/
	#define offset_of_isp7_reg_isp_rdma_rreq_rst (106)
	#define mask_of_isp7_reg_isp_rdma_rreq_rst (0x8000)
	unsigned int reg_isp_rdma_rreq_rst:1;

	// h0035
	unsigned int /* padding 16 bit */:16;

	// h0036, bit: 12
	/* ISP RDMA pitch*/
	#define offset_of_isp7_reg_isp_rdma_pitch (108)
	#define mask_of_isp7_reg_isp_rdma_pitch (0x1fff)
	unsigned int reg_isp_rdma_pitch:13;

	// h0036, bit: 14
	/* */
	unsigned int :3;

	// h0036
	unsigned int /* padding 16 bit */:16;

	// h0037, bit: 12
	/* Ring buffer lines memory space (lines minus 1)*/
	#define offset_of_isp7_reg_isp_dma_ring_buf_sz (110)
	#define mask_of_isp7_reg_isp_dma_ring_buf_sz (0x1fff)
	unsigned int reg_isp_dma_ring_buf_sz:13;

	// h0037, bit: 14
	/* */
	unsigned int :3;

	// h0037
	unsigned int /* padding 16 bit */:16;

	// h0038, bit: 14
	/* dummy*/
	#define offset_of_isp7_reg_isp_rdma_dummy (112)
	#define mask_of_isp7_reg_isp_rdma_dummy (0xffff)
	unsigned int reg_isp_rdma_dummy:16;

	// h0038
	unsigned int /* padding 16 bit */:16;

	// h0039, bit: 14
	/* ISP RDMA width minus 1*/
	#define offset_of_isp7_reg_isp_rdma_width_m1 (114)
	#define mask_of_isp7_reg_isp_rdma_width_m1 (0xffff)
	unsigned int reg_isp_rdma_width_m1:16;

	// h0039
	unsigned int /* padding 16 bit */:16;

	// h003a, bit: 12
	/* ISP RDMA height minus 1*/
	#define offset_of_isp7_reg_isp_rdma_height_m1 (116)
	#define mask_of_isp7_reg_isp_rdma_height_m1 (0x1fff)
	unsigned int reg_isp_rdma_height_m1:13;

	// h003a, bit: 14
	/* */
	unsigned int :3;

	// h003a
	unsigned int /* padding 16 bit */:16;

	// h003b, bit: 5
	/* */
	#define offset_of_isp7_reg_isp_rdma_startx (118)
	#define mask_of_isp7_reg_isp_rdma_startx (0x3f)
	unsigned int reg_isp_rdma_startx:6;

	// h003b, bit: 14
	/* */
	unsigned int :10;

	// h003b
	unsigned int /* padding 16 bit */:16;

	// h003c, bit: 14
	/* ISP RDMA MIU base address*/
	#define offset_of_isp7_reg_isp_rdma_base (120)
	#define mask_of_isp7_reg_isp_rdma_base (0xffff)
	unsigned int reg_isp_rdma_base:16;

	// h003c
	unsigned int /* padding 16 bit */:16;

	// h003d, bit: 10
	/* ISP RDMA MIU base address*/
	#define offset_of_isp7_reg_isp_rdma_base_1 (122)
	#define mask_of_isp7_reg_isp_rdma_base_1 (0x7ff)
	unsigned int reg_isp_rdma_base_1:11;

	// h003d, bit: 14
	/* */
	unsigned int :5;

	// h003d
	unsigned int /* padding 16 bit */:16;

	// h003e, bit: 2
	/* ISP RDMA MIU request threshold, set 7 */
	#define offset_of_isp7_reg_isp_rdma_rreq_thrd (124)
	#define mask_of_isp7_reg_isp_rdma_rreq_thrd (0x7)
	unsigned int reg_isp_rdma_rreq_thrd:3;

	// h003e, bit: 3
	/* */
	unsigned int :1;

	// h003e, bit: 7
	/* ISP RDMA MIU high priority threshold*/
	#define offset_of_isp7_reg_isp_rdma_rreq_hpri (124)
	#define mask_of_isp7_reg_isp_rdma_rreq_hpri (0xf0)
	unsigned int reg_isp_rdma_rreq_hpri:4;

	// h003e, bit: 11
	/* ISP RDMA MIU burst number*/
	#define offset_of_isp7_reg_isp_rdma_rreq_max (124)
	#define mask_of_isp7_reg_isp_rdma_rreq_max (0xf00)
	unsigned int reg_isp_rdma_rreq_max:4;

	// h003e, bit: 14
	/* */
	unsigned int :4;

	// h003e
	unsigned int /* padding 16 bit */:16;

	// h003f, bit: 5
	/* ISP RDMA output blanking, set 0*/
	#define offset_of_isp7_reg_isp_rdma_tg_hblk (126)
	#define mask_of_isp7_reg_isp_rdma_tg_hblk (0x3f)
	unsigned int reg_isp_rdma_tg_hblk:6;

	// h003f, bit: 14
	/* */
	unsigned int :10;

	// h003f
	unsigned int /* padding 16 bit */:16;

	// h0040, bit: 0
	/* ISP RDMA start trigger*/
	#define offset_of_isp7_reg_isp_rdma_trigger (128)
	#define mask_of_isp7_reg_isp_rdma_trigger (0x1)
	unsigned int reg_isp_rdma_trigger:1;

	// h0040, bit: 14
	/* */
	unsigned int :15;

	// h0040
	unsigned int /* padding 16 bit */:16;

	// h0041, bit: 0
	/* ISP RDMA valid data msb align at 16bit mode*/
	#define offset_of_isp7_reg_isp_rdma_msb_align (130)
	#define mask_of_isp7_reg_isp_rdma_msb_align (0x1)
	unsigned int reg_isp_rdma_msb_align:1;

	// h0041, bit: 2
	/* ISP RDMA valid data mode at 16bit mode
	2'd0: 8-bit
	2'd1:10-bit
	2'd2: 16-bit
	2'd3: 12-bit*/
	#define offset_of_isp7_reg_isp_rdma_valid_mode (130)
	#define mask_of_isp7_reg_isp_rdma_valid_mode (0x6)
	unsigned int reg_isp_rdma_valid_mode:2;

	// h0041, bit: 14
	/* */
	unsigned int :13;

	// h0041
	unsigned int /* padding 16 bit */:16;

	// h0042, bit: 0
	/* ISP RDMA end of frame status*/
	#define offset_of_isp7_reg_isp_rdma_eof_read (132)
	#define mask_of_isp7_reg_isp_rdma_eof_read (0x1)
	unsigned int reg_isp_rdma_eof_read:1;

	// h0042, bit: 14
	/* */
	unsigned int :15;

	// h0042
	unsigned int /* padding 16 bit */:16;

	// h0043, bit: 0
	/* ISP DMA mclk req force active*/
	#define offset_of_isp7_reg_mreq_always_active (134)
	#define mask_of_isp7_reg_mreq_always_active (0x1)
	unsigned int reg_mreq_always_active:1;

	// h0043, bit: 1
	/* ISP DMA mclk req force off*/
	#define offset_of_isp7_reg_mreq_force_off (134)
	#define mask_of_isp7_reg_mreq_force_off (0x2)
	unsigned int reg_mreq_force_off:1;

	// h0043, bit: 14
	/* */
	unsigned int :14;

	// h0043
	unsigned int /* padding 16 bit */:16;

	// h0044, bit: 14
	/* */
	unsigned int :16;

	// h0044
	unsigned int /* padding 16 bit */:16;
#if 0
	// h0045, bit: 0
	/* ISP RDMA enable*/
	#define offset_of_isp7_reg_isp_rdma_en (138)
	#define mask_of_isp7_reg_isp_rdma_en (0x1)
	unsigned int reg_isp_rdma_en:1;

	// h0045, bit: 1
	/* */
	unsigned int :1;

	// h0045, bit: 2
	/* ISP DMA ring buffer mode enable*/
	#define offset_of_isp7_reg_isp_dma_ring_buf_en (138)
	#define mask_of_isp7_reg_isp_dma_ring_buf_en (0x4)
	unsigned int reg_isp_dma_ring_buf_en:1;

	// h0045, bit: 3
	/* */
	unsigned int :1;

	// h0045, bit: 4
	/* ISP RDMA auto vsync trigger mode*/
	#define offset_of_isp7_reg_isp_rdma_auto (138)
	#define mask_of_isp7_reg_isp_rdma_auto (0x10)
	unsigned int reg_isp_rdma_auto:1;

	// h0045, bit: 6
	/* */
	unsigned int :2;

	// h0045, bit: 7
	/* ISP RDMA high priority set*/
	#define offset_of_isp7_reg_isp_rdma_rreq_hpri_set (138)
	#define mask_of_isp7_reg_isp_rdma_rreq_hpri_set (0x80)
	unsigned int reg_isp_rdma_rreq_hpri_set:1;

	// h0045, bit: 9
	/* ISP RDMA depack mode:
	2'd0: 8-bit
	2'd1: 10-bit
	2'd2: 16-bit
	2'd3: 12-bit*/
	#define offset_of_isp7_reg_isp_rdma_mode (138)
	#define mask_of_isp7_reg_isp_rdma_mode (0x300)
	unsigned int reg_isp_rdma_mode:2;

	// h0045, bit: 14
	/* */
	unsigned int :5;

	// h0045, bit: 15
	/* ISP RDMA MIU request reset*/
	#define offset_of_isp7_reg_isp_rdma_rreq_rst (138)
	#define mask_of_isp7_reg_isp_rdma_rreq_rst (0x8000)
	unsigned int reg_isp_rdma_rreq_rst:1;

	// h0045
	unsigned int /* padding 16 bit */:16;

	// h0046, bit: 12
	/* ISP RDMA pitch*/
	#define offset_of_isp7_reg_isp_rdma_pitch (140)
	#define mask_of_isp7_reg_isp_rdma_pitch (0x1fff)
	unsigned int reg_isp_rdma_pitch:13;

	// h0046, bit: 14
	/* */
	unsigned int :3;

	// h0046
	unsigned int /* padding 16 bit */:16;

	// h0047, bit: 12
	/* Ring buffer lines memory space (lines minus 1)*/
	#define offset_of_isp7_reg_isp_dma_ring_buf_sz (142)
	#define mask_of_isp7_reg_isp_dma_ring_buf_sz (0x1fff)
	unsigned int reg_isp_dma_ring_buf_sz:13;

	// h0047, bit: 14
	/* */
	unsigned int :3;

	// h0047
	unsigned int /* padding 16 bit */:16;

	// h0048, bit: 14
	/* dummy*/
	#define offset_of_isp7_reg_isp_rdma_dummy (144)
	#define mask_of_isp7_reg_isp_rdma_dummy (0xffff)
	unsigned int reg_isp_rdma_dummy:16;

	// h0048
	unsigned int /* padding 16 bit */:16;

	// h0049, bit: 14
	/* ISP RDMA width minus 1*/
	#define offset_of_isp7_reg_isp_rdma_width_m1 (146)
	#define mask_of_isp7_reg_isp_rdma_width_m1 (0xffff)
	unsigned int reg_isp_rdma_width_m1:16;

	// h0049
	unsigned int /* padding 16 bit */:16;

	// h004a, bit: 12
	/* ISP RDMA height minus 1*/
	#define offset_of_isp7_reg_isp_rdma_height_m1 (148)
	#define mask_of_isp7_reg_isp_rdma_height_m1 (0x1fff)
	unsigned int reg_isp_rdma_height_m1:13;

	// h004a, bit: 14
	/* */
	unsigned int :3;

	// h004a
	unsigned int /* padding 16 bit */:16;

	// h004b, bit: 5
	/* */
	#define offset_of_isp7_reg_isp_rdma_startx (150)
	#define mask_of_isp7_reg_isp_rdma_startx (0x3f)
	unsigned int reg_isp_rdma_startx:6;

	// h004b, bit: 14
	/* */
	unsigned int :10;

	// h004b
	unsigned int /* padding 16 bit */:16;

	// h004c, bit: 14
	/* ISP RDMA MIU base address*/
	#define offset_of_isp7_reg_isp_rdma_base (152)
	#define mask_of_isp7_reg_isp_rdma_base (0xffff)
	unsigned int reg_isp_rdma_base:16;

	// h004c
	unsigned int /* padding 16 bit */:16;

	// h004d, bit: 10
	/* ISP RDMA MIU base address*/
	#define offset_of_isp7_reg_isp_rdma_base_1 (154)
	#define mask_of_isp7_reg_isp_rdma_base_1 (0x7ff)
	unsigned int reg_isp_rdma_base_1:11;

	// h004d, bit: 14
	/* */
	unsigned int :5;

	// h004d
	unsigned int /* padding 16 bit */:16;

	// h004e, bit: 2
	/* ISP RDMA MIU request threshold, set 7 */
	#define offset_of_isp7_reg_isp_rdma_rreq_thrd (156)
	#define mask_of_isp7_reg_isp_rdma_rreq_thrd (0x7)
	unsigned int reg_isp_rdma_rreq_thrd:3;

	// h004e, bit: 3
	/* */
	unsigned int :1;

	// h004e, bit: 7
	/* ISP RDMA MIU high priority threshold*/
	#define offset_of_isp7_reg_isp_rdma_rreq_hpri (156)
	#define mask_of_isp7_reg_isp_rdma_rreq_hpri (0xf0)
	unsigned int reg_isp_rdma_rreq_hpri:4;

	// h004e, bit: 11
	/* ISP RDMA MIU burst number*/
	#define offset_of_isp7_reg_isp_rdma_rreq_max (156)
	#define mask_of_isp7_reg_isp_rdma_rreq_max (0xf00)
	unsigned int reg_isp_rdma_rreq_max:4;

	// h004e, bit: 14
	/* */
	unsigned int :4;

	// h004e
	unsigned int /* padding 16 bit */:16;

	// h004f, bit: 5
	/* ISP RDMA output blanking, set 0*/
	#define offset_of_isp7_reg_isp_rdma_tg_hblk (158)
	#define mask_of_isp7_reg_isp_rdma_tg_hblk (0x3f)
	unsigned int reg_isp_rdma_tg_hblk:6;

	// h004f, bit: 14
	/* */
	unsigned int :10;

	// h004f
	unsigned int /* padding 16 bit */:16;

	// h0050, bit: 0
	/* ISP RDMA start trigger*/
	#define offset_of_isp7_reg_isp_rdma_trigger (160)
	#define mask_of_isp7_reg_isp_rdma_trigger (0x1)
	unsigned int reg_isp_rdma_trigger:1;

	// h0050, bit: 14
	/* */
	unsigned int :15;

	// h0050
	unsigned int /* padding 16 bit */:16;

	// h0051, bit: 0
	/* ISP RDMA valid data msb align at 16bit mode*/
	#define offset_of_isp7_reg_isp_rdma_msb_align (162)
	#define mask_of_isp7_reg_isp_rdma_msb_align (0x1)
	unsigned int reg_isp_rdma_msb_align:1;

	// h0051, bit: 2
	/* ISP RDMA valid data mode at 16bit mode
	2'd0: 8-bit
	2'd1:10-bit
	2'd2: 16-bit
	2'd3: 12-bit*/
	#define offset_of_isp7_reg_isp_rdma_valid_mode (162)
	#define mask_of_isp7_reg_isp_rdma_valid_mode (0x6)
	unsigned int reg_isp_rdma_valid_mode:2;

	// h0051, bit: 14
	/* */
	unsigned int :13;

	// h0051
	unsigned int /* padding 16 bit */:16;

	// h0052, bit: 0
	/* ISP RDMA end of frame status*/
	#define offset_of_isp7_reg_isp_rdma_eof_read (164)
	#define mask_of_isp7_reg_isp_rdma_eof_read (0x1)
	unsigned int reg_isp_rdma_eof_read:1;

	// h0052, bit: 14
	/* */
	unsigned int :15;

	// h0052
	unsigned int /* padding 16 bit */:16;

	// h0053, bit: 0
	/* ISP DMA mclk req force active*/
	#define offset_of_isp7_reg_mreq_always_active (166)
	#define mask_of_isp7_reg_mreq_always_active (0x1)
	unsigned int reg_mreq_always_active:1;

	// h0053, bit: 1
	/* ISP DMA mclk req force off*/
	#define offset_of_isp7_reg_mreq_force_off (166)
	#define mask_of_isp7_reg_mreq_force_off (0x2)
	unsigned int reg_mreq_force_off:1;

	// h0053, bit: 14
	/* */
	unsigned int :14;

	// h0053
	unsigned int /* padding 16 bit */:16;
#else
	unsigned int rdma_c[0xE]; //rdma for 420 C part
#endif
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

	// h0056, bit: 0
	/* */
	#define offset_of_isp7_reg_mload_crop2_mrq_rff_full_status (172)
	#define mask_of_isp7_reg_mload_crop2_mrq_rff_full_status (0x1)
	unsigned int reg_mload_crop2_mrq_rff_full_status:1;

	// h0056, bit: 1
	/* */
	#define offset_of_isp7_reg_mload_crop2_mrq_wff_full_status (172)
	#define mask_of_isp7_reg_mload_crop2_mrq_wff_full_status (0x2)
	unsigned int reg_mload_crop2_mrq_wff_full_status:1;

	// h0056, bit: 13
	/* */
	#define offset_of_isp7_reg_mload_crop2_mrq_no_answer_cnt_latch (172)
	#define mask_of_isp7_reg_mload_crop2_mrq_no_answer_cnt_latch (0x3ffc)
	unsigned int reg_mload_crop2_mrq_no_answer_cnt_latch:12;

	// h0056, bit: 14
	/* */
	unsigned int :2;

	// h0056
	unsigned int /* padding 16 bit */:16;

	// h0057, bit: 0
	/* */
	#define offset_of_isp7_reg_mload_crop2_mrq_hp_cut_in_en0 (174)
	#define mask_of_isp7_reg_mload_crop2_mrq_hp_cut_in_en0 (0x1)
	unsigned int reg_mload_crop2_mrq_hp_cut_in_en0:1;

	// h0057, bit: 1
	/* */
	#define offset_of_isp7_reg_mload_crop2_mrq_hp_cut_in_en1 (174)
	#define mask_of_isp7_reg_mload_crop2_mrq_hp_cut_in_en1 (0x2)
	unsigned int reg_mload_crop2_mrq_hp_cut_in_en1:1;

	// h0057, bit: 2
	/* */
	#define offset_of_isp7_reg_mrq_mload_crop2_rst (174)
	#define mask_of_isp7_reg_mrq_mload_crop2_rst (0x4)
	unsigned int reg_mrq_mload_crop2_rst:1;

	// h0057, bit: 4
	/* */
	#define offset_of_isp7_reg_mload_crop2_mrq_hpmask (174)
	#define mask_of_isp7_reg_mload_crop2_mrq_hpmask (0x18)
	unsigned int reg_mload_crop2_mrq_hpmask:2;

	// h0057, bit: 6
	/* */
	#define offset_of_isp7_reg_mload_crop2_mrq_req_mask (174)
	#define mask_of_isp7_reg_mload_crop2_mrq_req_mask (0x60)
	unsigned int reg_mload_crop2_mrq_req_mask:2;

	// h0057, bit: 14
	/* */
	unsigned int :8;

	// h0057, bit: 15
	/* */
	#define offset_of_isp7_reg_mload_crop2_mrq_debug_clr (174)
	#define mask_of_isp7_reg_mload_crop2_mrq_debug_clr (0x8000)
	unsigned int reg_mload_crop2_mrq_debug_clr:1;

	// h0057
	unsigned int /* padding 16 bit */:16;

	// h0058, bit: 7
	/* */
	#define offset_of_isp7_reg_mload_crop2_mrq_max_length_r0 (176)
	#define mask_of_isp7_reg_mload_crop2_mrq_max_length_r0 (0xff)
	unsigned int reg_mload_crop2_mrq_max_length_r0:8;

	// h0058, bit: 14
	/* */
	#define offset_of_isp7_reg_mload_crop2_mrq_max_length_r1 (176)
	#define mask_of_isp7_reg_mload_crop2_mrq_max_length_r1 (0xff00)
	unsigned int reg_mload_crop2_mrq_max_length_r1:8;

	// h0058
	unsigned int /* padding 16 bit */:16;

	// h0059, bit: 7
	/* */
	#define offset_of_isp7_reg_mload_crop2_mrq_max_length_all (178)
	#define mask_of_isp7_reg_mload_crop2_mrq_max_length_all (0xff)
	unsigned int reg_mload_crop2_mrq_max_length_all:8;

	// h0059, bit: 14
	/* */
	unsigned int :8;

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

	// h0060, bit: 7
	/* */
	#define offset_of_isp7_reg_mload_crop2_mrq_hp_cut_min0 (192)
	#define mask_of_isp7_reg_mload_crop2_mrq_hp_cut_min0 (0xff)
	unsigned int reg_mload_crop2_mrq_hp_cut_min0:8;

	// h0060, bit: 14
	/* */
	#define offset_of_isp7_reg_mload_crop2_mrq_hp_cut_min1 (192)
	#define mask_of_isp7_reg_mload_crop2_mrq_hp_cut_min1 (0xff00)
	unsigned int reg_mload_crop2_mrq_hp_cut_min1:8;

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

	// h0078, bit: 0
	/* i2c clock enable*/
	#define offset_of_isp7_reg_i2c_en (240)
	#define mask_of_isp7_reg_i2c_en (0x1)
	unsigned int reg_i2c_en:1;

	// h0078, bit: 7
	/* */
	unsigned int :7;

	// h0078, bit: 8
	/* i2c resetz*/
	#define offset_of_isp7_reg_i2c_rstz (240)
	#define mask_of_isp7_reg_i2c_rstz (0x100)
	unsigned int reg_i2c_rstz:1;

	// h0078, bit: 14
	/* */
	unsigned int :7;

	// h0078
	unsigned int /* padding 16 bit */:16;

	// h0079, bit: 7
	/* 2nd Register Address for 16 bit register address series mode
	 (single buffer)*/
	#define offset_of_isp7_reg_sen_m2s_2nd_reg_adr (242)
	#define mask_of_isp7_reg_sen_m2s_2nd_reg_adr (0xff)
	unsigned int reg_sen_m2s_2nd_reg_adr:8;

	// h0079, bit: 14
	/* */
	unsigned int :8;

	// h0079
	unsigned int /* padding 16 bit */:16;

	// h007a, bit: 0
	/* M2S Signal Control
	# 1'b0: by HW
	# 1'b1: by SW
	 ( single buffer register)*/
	#define offset_of_isp7_reg_sen_m2s_sw_ctrl (244)
	#define mask_of_isp7_reg_sen_m2s_sw_ctrl (0x1)
	unsigned int reg_sen_m2s_sw_ctrl:1;

	// h007a, bit: 1
	/* M2S mode.
	# 1'b1: OmniVision SIF.
	# 1'b0: Others.
	 ( single buffer register )*/
	#define offset_of_isp7_reg_sen_m2s_mode (244)
	#define mask_of_isp7_reg_sen_m2s_mode (0x2)
	unsigned int reg_sen_m2s_mode:1;

	// h007a, bit: 2
	/* M2S burst command length.
	# 1'b0: length is 1.
	# 1'b1: length is 2.
	 ( single buffer register )*/
	#define offset_of_isp7_reg_sen_m2s_cmd_bl (244)
	#define mask_of_isp7_reg_sen_m2s_cmd_bl (0x4)
	unsigned int reg_sen_m2s_cmd_bl:1;

	// h007a, bit: 3
	/* M2S Register Address Mode
	# 1'b0 : 8 bit mode
	# 1'b1 : 16 bit series mode
	 (single buffer register)*/
	#define offset_of_isp7_reg_sen_m2s_reg_adr_mode (244)
	#define mask_of_isp7_reg_sen_m2s_reg_adr_mode (0x8)
	unsigned int reg_sen_m2s_reg_adr_mode:1;

	// h007a, bit: 13
	/* */
	unsigned int :10;

	// h007a, bit: 14
	/* M2S SDA Input
	 ( single buffer register )*/
	#define offset_of_isp7_reg_sda_i (244)
	#define mask_of_isp7_reg_sda_i (0x4000)
	unsigned int reg_sda_i:1;

	// h007a, bit: 15
	/* M2S Status
	# 1'b0: Idle
	# 1'b1: Busy
	 ( single buffer register )*/
	#define offset_of_isp7_reg_m2s_status (244)
	#define mask_of_isp7_reg_m2s_status (0x8000)
	unsigned int reg_m2s_status:1;

	// h007a
	unsigned int /* padding 16 bit */:16;

	// h007b, bit: 14
	/* M2S command
	# [0]: 1: read, 0: write.
	# [7:1]: Slave address.
	# [15:8]: Register address for 8 bit mode / first register address for 16 bit series mode
	 ( single buffer register )*/
	#define offset_of_isp7_reg_sen_m2s_cmd (246)
	#define mask_of_isp7_reg_sen_m2s_cmd (0xffff)
	unsigned int reg_sen_m2s_cmd:16;

	// h007b
	unsigned int /* padding 16 bit */:16;

	// h007c, bit: 14
	/* M2S Write data.
	# [7:0]: The first byte of Write data.
	# [15:8]: The second byte of Write data.
	Not valid if reg_sen_m2s_cmd_bl is 0.
	 ( single buffer register )*/
	#define offset_of_isp7_reg_sen_m2s_rw_d (248)
	#define mask_of_isp7_reg_sen_m2s_rw_d (0xffff)
	unsigned int reg_sen_m2s_rw_d:16;

	// h007c
	unsigned int /* padding 16 bit */:16;

	// h007d, bit: 9
	/* Factor of division for generating M2S clock.
	Must be programmed less than SYSCLK /(4*M2S_frequency) minus 1
	 ( single buffer register )*/
	#define offset_of_isp7_reg_sen_prescale (250)
	#define mask_of_isp7_reg_sen_prescale (0x3ff)
	unsigned int reg_sen_prescale:10;

	// h007d, bit: 14
	/* */
	unsigned int :6;

	// h007d
	unsigned int /* padding 16 bit */:16;

	// h007e, bit: 0
	/* M2S scl control
	# 1'b0: scl low
	# 1'b1: scl high*/
	#define offset_of_isp7_reg_sen_sw_scl_oen (252)
	#define mask_of_isp7_reg_sen_sw_scl_oen (0x1)
	unsigned int reg_sen_sw_scl_oen:1;

	// h007e, bit: 1
	/* M2S sda control
	# 1'b0: sda output low
	# 1'b1: sda input or sda output high*/
	#define offset_of_isp7_reg_sen_sw_sda_oen (252)
	#define mask_of_isp7_reg_sen_sw_sda_oen (0x2)
	unsigned int reg_sen_sw_sda_oen:1;

	// h007e, bit: 14
	/* */
	unsigned int :14;

	// h007e
	unsigned int /* padding 16 bit */:16;

	// h007f, bit: 14
	/* M2S Read data.
	# [7:0]: The first byte of Read data.
	# [15:8]: The second byte of Read data.
	Not valid if reg_sen_m2s_cmd_bl is 0.
	 ( single buffer register )*/
	#define offset_of_isp7_reg_sen_rd (254)
	#define mask_of_isp7_reg_sen_rd (0xffff)
	unsigned int reg_sen_rd:16;

	// h007f
	unsigned int /* padding 16 bit */:16;

}  __attribute__((packed, aligned(1))) infinity2_reg_isp7;
#endif
