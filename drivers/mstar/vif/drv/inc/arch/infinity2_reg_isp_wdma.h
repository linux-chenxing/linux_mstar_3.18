typedef struct {
	// h0000, bit: 0
	/* ISP WDMA enable*/
	#define offset_of_reg_isp_wdma_en (0)
	#define mask_of_reg_isp_wdma_en (0x1)
	unsigned int reg_isp_wdma_en:1;

	// h0000, bit: 1
	/* ISP WDMA enable but data not write to DRAM*/
	#define offset_of_reg_isp_wdma_mask_out (0)
	#define mask_of_reg_isp_wdma_mask_out (0x2)
	unsigned int reg_isp_wdma_mask_out:1;

	// h0000, bit: 2
	/* */
	unsigned int :1;

	// h0000, bit: 3
	/* ISP WDMA MIU wbe mask*/
	#define offset_of_reg_isp_wdma_wbe_mask (0)
	#define mask_of_reg_isp_wdma_wbe_mask (0x8)
	unsigned int reg_isp_wdma_wbe_mask:1;

	// h0000, bit: 4
	/* ISP WDMA auto vsync trigger mode*/
	#define offset_of_reg_isp_wdma_auto (0)
	#define mask_of_reg_isp_wdma_auto (0x10)
	unsigned int reg_isp_wdma_auto:1;

	// h0000, bit: 6
	/* */
	unsigned int :2;

	// h0000, bit: 7
	/* ISP WDMA high priority set*/
	#define offset_of_reg_isp_wdma_wreq_hpri_set (0)
	#define mask_of_reg_isp_wdma_wreq_hpri_set (0x80)
	unsigned int reg_isp_wdma_wreq_hpri_set:1;

	// h0000, bit: 11
	/* ISP WDMA pack mode:
	4'd0: 8-bit
	4'd1: 10-bit
	4'd2: 16-bit
	4'd3: 12-bit
	4'd4: 32-bit (1T4P, 8-bit/P)
	4'd5: 40-bit (1T4P, 10-bit/P)
	4'd6: 64-bit (1T4P, 16-bit/P)
	4'd7: 48-bit (1T4P, 12-bit/P)
	4'd8: 30-bit
	4'd9: 36-bit
	others: reserved*/
	#define offset_of_reg_isp_wdma_mode (0)
	#define mask_of_reg_isp_wdma_mode (0xf00)
	unsigned int reg_isp_wdma_mode:4;

	// h0000, bit: 14
	/* */
	unsigned int :3;

	// h0000, bit: 15
	/* ISP WDMA MIU request reset*/
	#define offset_of_reg_isp_wdma_wreq_rst (0)
	#define mask_of_reg_isp_wdma_wreq_rst (0x8000)
	unsigned int reg_isp_wdma_wreq_rst:1;

	// h0000
	unsigned int /* padding 16 bit */:16;

	// h0001, bit: 12
	/* ISP WDMA pitch*/
	#define offset_of_reg_isp_wdma_pitch (2)
	#define mask_of_reg_isp_wdma_pitch (0x1fff)
	unsigned int reg_isp_wdma_pitch:13;

	// h0001, bit: 14
	/* */
	unsigned int :3;

	// h0001
	unsigned int /* padding 16 bit */:16;

	// h0002, bit: 12
	/* ISP WDMA assert IRQ at linex*/
	#define offset_of_reg_isp_wdma_irqlinex (4)
	#define mask_of_reg_isp_wdma_irqlinex (0x1fff)
	unsigned int reg_isp_wdma_irqlinex:13;

	// h0002, bit: 14
	/* */
	unsigned int :3;

	// h0002
	unsigned int /* padding 16 bit */:16;

	// h0003, bit: 14
	/* */
	unsigned int :16;

	// h0003
	unsigned int /* padding 16 bit */:16;

	// h0004, bit: 14
	/* ISP WDMA width minus 1*/
	#define offset_of_reg_isp_wdma_width_m1 (8)
	#define mask_of_reg_isp_wdma_width_m1 (0xffff)
	unsigned int reg_isp_wdma_width_m1:16;

	// h0004
	unsigned int /* padding 16 bit */:16;

	// h0005, bit: 12
	/* ISP WDMA height minus 1*/
	#define offset_of_reg_isp_wdma_height_m1 (10)
	#define mask_of_reg_isp_wdma_height_m1 (0x1fff)
	unsigned int reg_isp_wdma_height_m1:13;

	// h0005, bit: 14
	/* */
	unsigned int :2;

	// h0005, bit: 15
	/* ISP WDMA height limit enable*/
	#define offset_of_reg_isp_wdma_height_en (10)
	#define mask_of_reg_isp_wdma_height_en (0x8000)
	unsigned int reg_isp_wdma_height_en:1;

	// h0005
	unsigned int /* padding 16 bit */:16;

	// h0006, bit: 14
	/* ISP WDMA write limit address*/
	#define offset_of_reg_isp_wdma_w_limit_adr (12)
	#define mask_of_reg_isp_wdma_w_limit_adr (0xffff)
	unsigned int reg_isp_wdma_w_limit_adr:16;

	// h0006
	unsigned int /* padding 16 bit */:16;

	// h0007, bit: 10
	/* ISP WDMA write limit address*/
	#define offset_of_reg_isp_wdma_w_limit_adr_1 (14)
	#define mask_of_reg_isp_wdma_w_limit_adr_1 (0x7ff)
	unsigned int reg_isp_wdma_w_limit_adr_1:11;

	// h0007, bit: 14
	/* */
	unsigned int :4;

	// h0007, bit: 15
	/* ISP WDMA write limit enable*/
	#define offset_of_reg_isp_wdma_w_limit_en (14)
	#define mask_of_reg_isp_wdma_w_limit_en (0x8000)
	unsigned int reg_isp_wdma_w_limit_en:1;

	// h0007
	unsigned int /* padding 16 bit */:16;

	// h0008, bit: 14
	/* ISP WDMA MIU base address*/
	#define offset_of_reg_isp_wdma_base (16)
	#define mask_of_reg_isp_wdma_base (0xffff)
	unsigned int reg_isp_wdma_base:16;

	// h0008
	unsigned int /* padding 16 bit */:16;

	// h0009, bit: 10
	/* ISP WDMA MIU base address*/
	#define offset_of_reg_isp_wdma_base_1 (18)
	#define mask_of_reg_isp_wdma_base_1 (0x7ff)
	unsigned int reg_isp_wdma_base_1:11;

	// h0009, bit: 14
	/* */
	unsigned int :5;

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

	// h000c, bit: 2
	/* ISP WDMA MIU request threshold (*4+1)*/
	#define offset_of_reg_isp_wdma_wreq_thrd (24)
	#define mask_of_reg_isp_wdma_wreq_thrd (0x7)
	unsigned int reg_isp_wdma_wreq_thrd:3;

	// h000c, bit: 3
	/* */
	unsigned int :1;

	// h000c, bit: 7
	/* ISP WDMA MIU high priority threshold*/
	#define offset_of_reg_isp_wdma_wreq_hpri (24)
	#define mask_of_reg_isp_wdma_wreq_hpri (0xf0)
	unsigned int reg_isp_wdma_wreq_hpri:4;

	// h000c, bit: 11
	/* ISP WDMA MIU burst number*/
	#define offset_of_reg_isp_wdma_wreq_max (24)
	#define mask_of_reg_isp_wdma_wreq_max (0xf00)
	unsigned int reg_isp_wdma_wreq_max:4;

	// h000c, bit: 14
	/* */
	unsigned int :4;

	// h000c
	unsigned int /* padding 16 bit */:16;

	// h000d, bit: 14
	/* SW write fence ID*/
	#define offset_of_reg_isp_sw_w_fence (26)
	#define mask_of_reg_isp_sw_w_fence (0xffff)
	unsigned int reg_isp_sw_w_fence:16;

	// h000d
	unsigned int /* padding 16 bit */:16;

	// h000e, bit: 14
	/* SW read fence ID*/
	#define offset_of_reg_isp_sw_r_fence (28)
	#define mask_of_reg_isp_sw_r_fence (0xffff)
	unsigned int reg_isp_sw_r_fence:16;

	// h000e
	unsigned int /* padding 16 bit */:16;

	// h000f, bit: 14
	/* */
	unsigned int :16;

	// h000f
	unsigned int /* padding 16 bit */:16;

	// h0010, bit: 0
	/* ISP WDMA start trigger*/
	#define offset_of_reg_isp_wdma_trigger (32)
	#define mask_of_reg_isp_wdma_trigger (0x1)
	unsigned int reg_isp_wdma_trigger:1;

	// h0010, bit: 1
	/* ISP WDMA start trigger vsync aligned mode*/
	#define offset_of_reg_isp_wdma_trigger_mode (32)
	#define mask_of_reg_isp_wdma_trigger_mode (0x2)
	unsigned int reg_isp_wdma_trigger_mode:1;

	// h0010, bit: 7
	/* */
	unsigned int :6;

	// h0010, bit: 8
	/* ISP WDMA input auto-align enable*/
	#define offset_of_reg_isp_wdma_align_en (32)
	#define mask_of_reg_isp_wdma_align_en (0x100)
	unsigned int reg_isp_wdma_align_en:1;

	// h0010, bit: 14
	/* */
	unsigned int :7;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 0
	/* ISP WDMA valid data msb align at 16bit mode*/
	#define offset_of_reg_isp_wdma_msb_align (34)
	#define mask_of_reg_isp_wdma_msb_align (0x1)
	unsigned int reg_isp_wdma_msb_align:1;

	// h0011, bit: 3
	/* */
	unsigned int :3;

	// h0011, bit: 7
	/* ISP WDMA lsb mode, right shift bits*/
	#define offset_of_reg_isp_wdma_lsb_shift (34)
	#define mask_of_reg_isp_wdma_lsb_shift (0xf0)
	unsigned int reg_isp_wdma_lsb_shift:4;

	// h0011, bit: 14
	/* */
	unsigned int :8;

	// h0011
	unsigned int /* padding 16 bit */:16;

	// h0012, bit: 14
	/* dummy*/
	#define offset_of_reg_isp_wdma_dummy (36)
	#define mask_of_reg_isp_wdma_dummy (0xffff)
	unsigned int reg_isp_wdma_dummy:16;

	// h0012
	unsigned int /* padding 16 bit */:16;

	// h0013, bit: 14
	/* dummy*/
	#define offset_of_reg_isp_wdma_dummy_1 (38)
	#define mask_of_reg_isp_wdma_dummy_1 (0xffff)
	unsigned int reg_isp_wdma_dummy_1:16;

	// h0013
	unsigned int /* padding 16 bit */:16;

#if 0
	// h0014, bit: 4
	/* 422to420 c ratio*/
	#define offset_of_reg_isp_422to420_c_ratio (40)
	#define mask_of_reg_isp_422to420_c_ratio (0x1f)
	unsigned int reg_isp_422to420_c_ratio:5;

	// h0014, bit: 5
	/* 420 c sel*/
	#define offset_of_reg_isp_prog_420c_sel (40)
	#define mask_of_reg_isp_prog_420c_sel (0x20)
	unsigned int reg_isp_prog_420c_sel:1;

	// h0014, bit: 6
	/* 422 to 420 enable*/
	#define offset_of_reg_isp_422to420_en (40)
	#define mask_of_reg_isp_422to420_en (0x40)
	unsigned int reg_isp_422to420_en:1;

	// h0014, bit: 7
	/* 422to420 align enable*/
	#define offset_of_reg_isp_align_en (40)
	#define mask_of_reg_isp_align_en (0x80)
	unsigned int reg_isp_align_en:1;

	// h0014, bit: 8
	/* 422to420 align number select*/
	#define offset_of_reg_isp_align_num_sel (40)
	#define mask_of_reg_isp_align_num_sel (0x100)
	unsigned int reg_isp_align_num_sel:1;

	// h0014, bit: 13
	/* 422to420 align number*/
	#define offset_of_reg_isp_align_num (40)
	#define mask_of_reg_isp_align_num (0x3e00)
	unsigned int reg_isp_align_num:5;

	// h0014, bit: 14
	/* 422to420 line buffer mode
	0: 1280*4
	1: 1920*2
	2:  4096*1*/
	#define offset_of_reg_isp_lb_mode (40)
	#define mask_of_reg_isp_lb_mode (0xc000)
	unsigned int reg_isp_lb_mode:2;
#else
	// h0014
	unsigned int /* padding 16 bit */:16;
#endif
	// h0014
	unsigned int /* padding 16 bit */:16;

	// h0015, bit: 14
	/* */
	unsigned int :16;

	// h0015
	unsigned int /* padding 16 bit */:16;

	// h0016, bit: 0
	/* ISP WDMA end of frame status*/
	#define offset_of_reg_isp_wdma_eof_read (44)
	#define mask_of_reg_isp_wdma_eof_read (0x1)
	unsigned int reg_isp_wdma_eof_read:1;

	// h0016, bit: 7
	/* */
	unsigned int :7;

	// h0016, bit: 8
	/* ISP WDMA LB full status*/
	#define offset_of_reg_isp_wdma_lb_full_read (44)
	#define mask_of_reg_isp_wdma_lb_full_read (0x100)
	unsigned int reg_isp_wdma_lb_full_read:1;

	// h0016, bit: 14
	/* */
	unsigned int :6;

	// h0016, bit: 15
	/* ISP WDMA status clear*/
	#define offset_of_reg_isp_wdma_status_clr (44)
	#define mask_of_reg_isp_wdma_status_clr (0x8000)
	unsigned int reg_isp_wdma_status_clr:1;

	// h0016
	unsigned int /* padding 16 bit */:16;

	// h0017, bit: 0
	/* ISP DMA mclk req force active*/
	#define offset_of_reg_mreq_always_active (46)
	#define mask_of_reg_mreq_always_active (0x1)
	unsigned int reg_mreq_always_active:1;

	// h0017, bit: 1
	/* ISP DMA mclk req force off*/
	#define offset_of_reg_mreq_force_off (46)
	#define mask_of_reg_mreq_force_off (0x2)
	unsigned int reg_mreq_force_off:1;

	// h0017, bit: 2
	/* ISP DMA mclk req wdma mode*/
	#define offset_of_reg_mreq_wdma_mode (46)
	#define mask_of_reg_mreq_wdma_mode (0x4)
	unsigned int reg_mreq_wdma_mode:1;

	// h0017, bit: 14
	/* */
	unsigned int :13;

	// h0017
	unsigned int /* padding 16 bit */:16;


	// h0018, bit: 0
	/* 0 : 1 pixel mode
	1 : 4 pixel mode*/
	#define offset_of_dmag_sub_reg_pix4_mode (48)
	#define mask_of_dmag_sub_reg_pix4_mode (0x1)
	unsigned int reg_pix4_mode:1;

	// h0018, bit: 1
	/* 0 : non-bayer format
	1 : bayer format*/
	#define offset_of_dmag_sub_reg_bayer_fmt (48)
	#define mask_of_dmag_sub_reg_bayer_fmt (0x2)
	unsigned int reg_bayer_fmt:1;

	// h0018, bit: 2
	/* 1 : 420 down sample enable*/
	#define offset_of_dmag_sub_reg_420dn_en (48)
	#define mask_of_dmag_sub_reg_420dn_en (0x4)
	unsigned int reg_420dn_en:1;

	// h0018, bit: 3
	/* 0 : 420 down sample for y
	1 : 420 down sample for c*/
	#define offset_of_dmag_sub_reg_420dn_yc_sel (48)
	#define mask_of_dmag_sub_reg_420dn_yc_sel (0x8)
	unsigned int reg_420dn_yc_sel:1;

	// h0018, bit: 7
	/* 0 : 8-bits
	1 : 10-bits
	2 : 12-bits
	3 : 16-bits
	4 : 24-bits
	5 : 30-bits
	6 : 32-bits
	7 : 36-bits
	8 : 40-bits
	9 : 48-bits
	10 : 64-bits*/
	#define offset_of_dmag_sub_reg_bits_per_rdy (48)
	#define mask_of_dmag_sub_reg_bits_per_rdy (0xf0)
	unsigned int reg_bits_per_rdy:4;

	// h0018, bit: 10
	/* 0 : 1/1
	1 : 1/2
	2 : 1/4
	3 : 1/8
	4 : 1/16
	5 : 1/32*/
	#define offset_of_dmag_sub_reg_dn_mode (48)
	#define mask_of_dmag_sub_reg_dn_mode (0x700)
	unsigned int reg_dn_mode:3;

	// h0018, bit: 11
	/* */
	unsigned int :1;

	// h0018, bit: 13
	/* 0 : keep 1st Cb/Cr
	1 : keep 2nd Cb/Cr
	3 : average of 1st and 2nd Cb/Cr*/
	#define offset_of_dmag_sub_reg_420dn_uv_mode (48)
	#define mask_of_dmag_sub_reg_420dn_uv_mode (0x3000)
	unsigned int reg_420dn_uv_mode:2;

	// h0018, bit: 14
	/* */
	unsigned int :2;

	// h0018
	unsigned int /* padding 16 bit */:16;

	// h0019, bit: 12
	/* source image width*/
	#define offset_of_dmag_sub_reg_src_width (50)
	#define mask_of_dmag_sub_reg_src_width (0x1fff)
	unsigned int reg_src_width:13;

	// h0019, bit: 14
	/* */
	unsigned int :3;

	// h0019
	unsigned int /* padding 16 bit */:16;

	// h001a, bit: 12
	/* soruce image height*/
	#define offset_of_dmag_sub_reg_src_height (52)
	#define mask_of_dmag_sub_reg_src_height (0x1fff)
	unsigned int reg_src_height:13;

	// h001a, bit: 14
	/* */
	unsigned int :3;

	// h001a
	unsigned int /* padding 16 bit */:16;

	// h001b, bit: 1
	/* y coefficient w0*/
	#define offset_of_dmag_sub_reg_420dn_y_w0 (54)
	#define mask_of_dmag_sub_reg_420dn_y_w0 (0x3)
	unsigned int reg_420dn_y_w0:2;

	// h001b, bit: 2
	/* sign of y coefficent w0*/
	#define offset_of_dmag_sub_reg_420dn_y_wo_s (54)
	#define mask_of_dmag_sub_reg_420dn_y_wo_s (0x4)
	unsigned int reg_420dn_y_wo_s:1;

	// h001b, bit: 7
	/* */
	unsigned int :5;

	// h001b, bit: 12
	/* y coefficiecnt w1*/
	#define offset_of_dmag_sub_reg_420dn_y_w1 (54)
	#define mask_of_dmag_sub_reg_420dn_y_w1 (0x1f00)
	unsigned int reg_420dn_y_w1:5;

	// h001b, bit: 14
	/* */
	unsigned int :3;

	// h001b
	unsigned int /* padding 16 bit */:16;

	// h001c, bit: 4
	/* y coefficient w2*/
	#define offset_of_dmag_sub_reg_420dn_y_w2 (56)
	#define mask_of_dmag_sub_reg_420dn_y_w2 (0x1f)
	unsigned int reg_420dn_y_w2:5;

	// h001c, bit: 7
	/* */
	unsigned int :3;

	// h001c, bit: 9
	/* y coefficiecnt w3*/
	#define offset_of_dmag_sub_reg_420dn_y_w3 (56)
	#define mask_of_dmag_sub_reg_420dn_y_w3 (0x300)
	unsigned int reg_420dn_y_w3:2;

	// h001c, bit: 10
	/* sign of y coefficient w3*/
	#define offset_of_dmag_sub_reg_420dn_y_w3_s (56)
	#define mask_of_dmag_sub_reg_420dn_y_w3_s (0x400)
	unsigned int reg_420dn_y_w3_s:1;

	// h001c, bit: 14
	/* */
	unsigned int :5;

	// h001c
	unsigned int /* padding 16 bit */:16;

	// h001d, bit: 2
	/* y result shift*/
	#define offset_of_dmag_sub_reg_420dn_y_sft (58)
	#define mask_of_dmag_sub_reg_420dn_y_sft (0x7)
	unsigned int reg_420dn_y_sft:3;

	// h001d, bit: 14
	/* */
	unsigned int :13;

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
}__attribute__((packed, aligned(1))) infinity2_isp_wdma;

typedef struct
{
	// h0014, bit: 4
	/* 422to420 c ratio*/
	#define offset_of_reg_isp_422to420_c_ratio (40)
	#define mask_of_reg_isp_422to420_c_ratio (0x1f)
	unsigned int reg_isp_422to420_c_ratio:5;

	// h0014, bit: 5
	/* 420 c sel*/
	#define offset_of_reg_isp_prog_420c_sel (40)
	#define mask_of_reg_isp_prog_420c_sel (0x20)
	unsigned int reg_isp_prog_420c_sel:1;

	// h0014, bit: 6
	/* 422 to 420 enable*/
	#define offset_of_reg_isp_422to420_en (40)
	#define mask_of_reg_isp_422to420_en (0x40)
	unsigned int reg_isp_422to420_en:1;

	// h0014, bit: 7
	/* 422to420 align enable*/
	#define offset_of_reg_isp_align_en (40)
	#define mask_of_reg_isp_align_en (0x80)
	unsigned int reg_isp_align_en:1;

	// h0014, bit: 8
	/* 422to420 align number select*/
	#define offset_of_reg_isp_align_num_sel (40)
	#define mask_of_reg_isp_align_num_sel (0x100)
	unsigned int reg_isp_align_num_sel:1;

	// h0014, bit: 13
	/* 422to420 align number*/
	#define offset_of_reg_isp_align_num (40)
	#define mask_of_reg_isp_align_num (0x3e00)
	unsigned int reg_isp_align_num:5;

	// h0014, bit: 14
	/* 422to420 line buffer mode
	0: 1280*4
	1: 1920*2
	2:  4096*1*/
	#define offset_of_reg_isp_lb_mode (40)
	#define mask_of_reg_isp_lb_mode (0xc000)
	unsigned int reg_isp_lb_mode:2;

	// h0014
	unsigned int /* padding 16 bit */:16;
}__attribute__((packed, aligned(1))) infinity2_isp_wdma_yuv_ctrl;
