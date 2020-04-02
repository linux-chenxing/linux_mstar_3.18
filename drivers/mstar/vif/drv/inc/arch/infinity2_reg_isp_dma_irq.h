// Generate Time: 2017-09-25 23:19:41.473964
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
#ifndef __INFINITY2_REG_ISP_DMA_IRQ__
#define __INFINITY2_REG_ISP_DMA_IRQ__
typedef struct {
	// h0000, bit: 14
	/* Interrupt Mask*/
	#define offset_of_reg_c_irq_mask1 (0)
	#define mask_of_reg_c_irq_mask1 (0xffff)
	unsigned int reg_c_irq_mask1:16;

	// h0000
	unsigned int /* padding 16 bit */:16;

	// h0001, bit: 14
	/* Force Interrupt Enable*/
	#define offset_of_reg_c_irq_force1 (2)
	#define mask_of_reg_c_irq_force1 (0xffff)
	unsigned int reg_c_irq_force1:16;

	// h0001
	unsigned int /* padding 16 bit */:16;

	// h0002, bit: 14
	/* Interrupt Clear*/
	#define offset_of_reg_c_irq_clr1 (4)
	#define mask_of_reg_c_irq_clr1 (0xffff)
	unsigned int reg_c_irq_clr1:16;

	// h0002
	unsigned int /* padding 16 bit */:16;

	// h0003, bit: 14
	/* Status of Interrupt on CPU side*/
	#define offset_of_reg_irq_final_status1 (6)
	#define mask_of_reg_irq_final_status1 (0xffff)
	unsigned int reg_irq_final_status1:16;

	// h0003
	unsigned int /* padding 16 bit */:16;

	// h0004, bit: 14
	/* Status of Interrupt on IP side (FIFO FUll)
	# [0] : Group0 fifo0 full
	# [1] : Group0 fifo1 full
	# [2] : Group0 fifo2 full
	# [3] : Group0 fifo3 full
	# [4] : Group1 fifo0 full
	# [5] : Group1 fifo1 full
	# [6] : Group1 fifo2 full
	# [7] : Group1 fifo3 full
	# [8] : Group2 fifo0 full
	# [9] : Group2 fifo1 full
	# [10]:Group2 fifo2 full
	# [11]:Group2 fifo3 full
	# [12]:Group3 fifo0 full
	# [13]:Group3 fifo1 full
	# [14]:Group3 fifo2 full
	# [15]:Group3 fifo3 full*/
	#define offset_of_reg_irq_raw_status1 (8)
	#define mask_of_reg_irq_raw_status1 (0xffff)
	unsigned int reg_irq_raw_status1:16;

	// h0004
	unsigned int /* padding 16 bit */:16;

	// h0005, bit: 14
	/* Interrupt Mask*/
	#define offset_of_reg_c_irq_mask2 (10)
	#define mask_of_reg_c_irq_mask2 (0xffff)
	unsigned int reg_c_irq_mask2:16;

	// h0005
	unsigned int /* padding 16 bit */:16;

	// h0006, bit: 14
	/* Force Interrupt Enable*/
	#define offset_of_reg_c_irq_force2 (12)
	#define mask_of_reg_c_irq_force2 (0xffff)
	unsigned int reg_c_irq_force2:16;

	// h0006
	unsigned int /* padding 16 bit */:16;

	// h0007, bit: 14
	/* Interrupt Clear*/
	#define offset_of_reg_c_irq_clr2 (14)
	#define mask_of_reg_c_irq_clr2 (0xffff)
	unsigned int reg_c_irq_clr2:16;

	// h0007
	unsigned int /* padding 16 bit */:16;

	// h0008, bit: 14
	/* Status of Interrupt on CPU side*/
	#define offset_of_reg_irq_final_status2 (16)
	#define mask_of_reg_irq_final_status2 (0xffff)
	unsigned int reg_irq_final_status2:16;

	// h0008
	unsigned int /* padding 16 bit */:16;

	// h0009, bit: 14
	/* Status of Interrupt on IP side (frame start)
	# [0] : vif0 vs
	# [1] : vif1 vs
	# [2] : vif2 vs
	# [3] : vif3 vs
	# [4] : vif4 vs
	# [5] : vif5 vs
	# [6] : vif6 vs
	# [7] : vif7 vs
	# [8] : vif8 vs
	# [9] : vif9 vs
	# [10]:vif10 vs
	# [11]:vif11 vs
	# [12]:vif12 vs
	# [13]:vif13 vs
	# [14]:vif14 vs
	# [15]:vif15 vs*/
	#define offset_of_reg_irq_raw_status2 (18)
	#define mask_of_reg_irq_raw_status2 (0xffff)
	unsigned int reg_irq_raw_status2:16;

	// h0009
	unsigned int /* padding 16 bit */:16;

	// h000a, bit: 14
	/* Interrupt Mask*/
	#define offset_of_reg_c_irq_mask3 (20)
	#define mask_of_reg_c_irq_mask3 (0xffff)
	unsigned int reg_c_irq_mask3:16;

	// h000a
	unsigned int /* padding 16 bit */:16;

	// h000b, bit: 14
	/* Force Interrupt Enable*/
	#define offset_of_reg_c_irq_force3 (22)
	#define mask_of_reg_c_irq_force3 (0xffff)
	unsigned int reg_c_irq_force3:16;

	// h000b
	unsigned int /* padding 16 bit */:16;

	// h000c, bit: 14
	/* Interrupt Clear*/
	#define offset_of_reg_c_irq_clr3 (24)
	#define mask_of_reg_c_irq_clr3 (0xffff)
	unsigned int reg_c_irq_clr3:16;

	// h000c
	unsigned int /* padding 16 bit */:16;

	// h000d, bit: 14
	/* Status of Interrupt on CPU side*/
	#define offset_of_reg_irq_final_status3 (26)
	#define mask_of_reg_irq_final_status3 (0xffff)
	unsigned int reg_irq_final_status3:16;

	// h000d
	unsigned int /* padding 16 bit */:16;

	// h000e, bit: 14
	/* Status of Interrupt on IP side (DMA Done)
	# [0] : Group0 Channel0 yc done
	# [1] : Group0 Channel1 yc done
	# [2] : Group0 Channel2 yc done
	# [3] : Group0 Channel3 yc done
	# [4] : Group1 Channel0 yc done
	# [5] : Group1 Channel1 yc done
	# [6] : Group1 Channel2 yc done
	# [7] : Group1 Channel3 yc done
	# [8] : Group2 Channel0 yc done
	# [9] : Group2 Channel1 yc done
	# [10] : Group2 Channel2 yc done
	# [11] : Group2 Channel3 yc done
	# [12] : Group3 Channel0 yc done
	# [13] : Group3 Channel1 yc done
	# [14] : Group3 Channel2 yc done
	# [15] : Group3 Channel3 yc done*/
	#define offset_of_reg_irq_raw_status3 (28)
	#define mask_of_reg_irq_raw_status3 (0xffff)
	unsigned int reg_irq_raw_status3:16;

	// h000e
	unsigned int /* padding 16 bit */:16;

	// h000f, bit: 14
	/* */
	unsigned int :16;

	// h000f
	unsigned int /* padding 16 bit */:16;

	// h0010, bit: 14
	/* Interrupt Mask*/
	#define offset_of_reg_c_irq_mask4 (32)
	#define mask_of_reg_c_irq_mask4 (0xffff)
	unsigned int reg_c_irq_mask4:16;

	// h0010
	unsigned int /* padding 16 bit */:16;

	// h0011, bit: 14
	/* Force Interrupt3 Enable*/
	#define offset_of_reg_c_irq_force4 (34)
	#define mask_of_reg_c_irq_force4 (0xffff)
	unsigned int reg_c_irq_force4:16;

	// h0011
	unsigned int /* padding 16 bit */:16;

	// h0012, bit: 14
	/* Interrupt Clear*/
	#define offset_of_reg_c_irq_clr4 (36)
	#define mask_of_reg_c_irq_clr4 (0xffff)
	unsigned int reg_c_irq_clr4:16;

	// h0012
	unsigned int /* padding 16 bit */:16;

	// h0013, bit: 14
	/* Status of Interrupt on CPU side*/
	#define offset_of_reg_irq_final_status4 (38)
	#define mask_of_reg_irq_final_status4 (0xffff)
	unsigned int reg_irq_final_status4:16;

	// h0013
	unsigned int /* padding 16 bit */:16;

	// h0014, bit: 14
	/* Status of Interrupt on IP side (DMA Done)
	# [0] : Group0 Ch0 yc downsampe done
	# [1] : Group0 Ch1 yc downsampe done
	# [2] : Group0 Ch2 yc downsampe done
	# [3] : Group0 Ch3 yc downsampe done
	# [4] : Group1 Ch0 yc downsampe done
	# [5] : Group1 Ch1 yc downsampe done
	# [6] : Group1 Ch2 yc downsampe done
	# [7] : Group1 Ch3 yc downsampe done
	# [8] : Group2 Ch0 yc downsampe done
	# [9] : Group2 Ch1 yc downsampe done
	# [10] : Group2 Ch2 yc downsampe done
	# [11] : Group2 Ch3 yc downsampe done
	# [12] : Group3 Ch0 yc downsampe done
	# [13] : Group3 Ch1 yc downsampe done
	# [14] : Group3 Ch2 yc downsampe done
	# [15] : Group3 Ch3 yc downsampe done*/
	#define offset_of_reg_irq_raw_status4 (40)
	#define mask_of_reg_irq_raw_status4 (0xffff)
	unsigned int reg_irq_raw_status4:16;

	// h0014
	unsigned int /* padding 16 bit */:16;

	// h0015, bit: 14
	/* Interrupt Mask*/
	#define offset_of_reg_c_irq_mask5 (42)
	#define mask_of_reg_c_irq_mask5 (0xffff)
	unsigned int reg_c_irq_mask5:16;

	// h0015
	unsigned int /* padding 16 bit */:16;

	// h0016, bit: 14
	/* Force Interrupt Enable*/
	#define offset_of_reg_c_irq_force5 (44)
	#define mask_of_reg_c_irq_force5 (0xffff)
	unsigned int reg_c_irq_force5:16;

	// h0016
	unsigned int /* padding 16 bit */:16;

	// h0017, bit: 14
	/* Interrupt Clear*/
	#define offset_of_reg_c_irq_clr5 (46)
	#define mask_of_reg_c_irq_clr5 (0xffff)
	unsigned int reg_c_irq_clr5:16;

	// h0017
	unsigned int /* padding 16 bit */:16;

	// h0018, bit: 14
	/* Status of Interrupt on CPU side*/
	#define offset_of_reg_irq_final_status5 (48)
	#define mask_of_reg_irq_final_status5 (0xffff)
	unsigned int reg_irq_final_status5:16;

	// h0018
	unsigned int /* padding 16 bit */:16;

	// h0019, bit: 14
	/* Status of Interrupt on IP side (DMA Done)
	# [0] : Group0 ch0  line cnt0  hit
	# [1] : Group0 ch0  line cnt1  hit
	# [2] : Group0 ch0  line cnt2  hit
	# [3] : Group0 ch0  line cnt3  hit
	# [4] : Group0 ch1  line cnt0  hit
	# [5] : Group0 ch1  line cnt1  hit
	# [6] : Group0 ch1  line cnt2  hit
	# [7] : Group0 ch1  line cnt3  hit
	# [8] : Group0 ch2  line cnt0  hit
	# [9] : Group0 ch2  line cnt1  hit
	# [10] : Group0 ch2  line cnt2  hit
	# [11] : Group0 ch2  line cnt3  hit
	# [12] : Group0 ch3  line cnt0  hit
	# [13] : Group0 ch3  line cnt1  hit
	# [14] : Group0 ch3  line cnt2  hit
	# [15] : Group0 ch3  line cnt3  hit*/
	#define offset_of_reg_irq_raw_status5 (50)
	#define mask_of_reg_irq_raw_status5 (0xffff)
	unsigned int reg_irq_raw_status5:16;

	// h0019
	unsigned int /* padding 16 bit */:16;

	// h001a, bit: 14
	/* Interrupt Mask*/
	#define offset_of_reg_c_irq_mask6 (52)
	#define mask_of_reg_c_irq_mask6 (0xffff)
	unsigned int reg_c_irq_mask6:16;

	// h001a
	unsigned int /* padding 16 bit */:16;

	// h001b, bit: 14
	/* Force Interrupt Enable*/
	#define offset_of_reg_c_irq_force6 (54)
	#define mask_of_reg_c_irq_force6 (0xffff)
	unsigned int reg_c_irq_force6:16;

	// h001b
	unsigned int /* padding 16 bit */:16;

	// h001c, bit: 14
	/* Interrupt Clear*/
	#define offset_of_reg_c_irq_clr6 (56)
	#define mask_of_reg_c_irq_clr6 (0xffff)
	unsigned int reg_c_irq_clr6:16;

	// h001c
	unsigned int /* padding 16 bit */:16;

	// h001d, bit: 14
	/* Status of Interrupt on CPU side*/
	#define offset_of_reg_irq_final_status6 (58)
	#define mask_of_reg_irq_final_status6 (0xffff)
	unsigned int reg_irq_final_status6:16;

	// h001d
	unsigned int /* padding 16 bit */:16;

	// h001e, bit: 14
	/* Status of Interrupt on IP side (DMA Done)
	# [0] : Group1 ch0  line cnt0  hit
	# [1] : Group1 ch0  line cnt1  hit
	# [2] : Group1 ch0  line cnt2  hit
	# [3] : Group1 ch0  line cnt3  hit
	# [4] : Group1 ch1  line cnt0  hit
	# [5] : Group1 ch1  line cnt1  hit
	# [6] : Group1 ch1  line cnt2  hit
	# [7] : Group1 ch1  line cnt3  hit
	# [8] : Group1 ch2  line cnt0  hit
	# [9] : Group1 ch2  line cnt1  hit
	# [10] : Group1 ch2  line cnt2  hit
	# [11] : Group1 ch2  line cnt3  hit
	# [12] : Group1 ch3  line cnt0  hit
	# [13] : Group1 ch3  line cnt1  hit
	# [14] : Group1 ch3  line cnt2  hit
	# [15] : Group1 ch3  line cnt3  hit*/
	#define offset_of_reg_irq_raw_status6 (60)
	#define mask_of_reg_irq_raw_status6 (0xffff)
	unsigned int reg_irq_raw_status6:16;

	// h001e
	unsigned int /* padding 16 bit */:16;

	// h001f, bit: 14
	/* */
	unsigned int :16;

	// h001f
	unsigned int /* padding 16 bit */:16;

	// h0020, bit: 14
	/* Interrupt Mask*/
	#define offset_of_reg_c_irq_mask7 (64)
	#define mask_of_reg_c_irq_mask7 (0xffff)
	unsigned int reg_c_irq_mask7:16;

	// h0020
	unsigned int /* padding 16 bit */:16;

	// h0021, bit: 14
	/* Force Interrupt Enable*/
	#define offset_of_reg_c_irq_force7 (66)
	#define mask_of_reg_c_irq_force7 (0xffff)
	unsigned int reg_c_irq_force7:16;

	// h0021
	unsigned int /* padding 16 bit */:16;

	// h0022, bit: 14
	/* Interrupt Clear*/
	#define offset_of_reg_c_irq_clr7 (68)
	#define mask_of_reg_c_irq_clr7 (0xffff)
	unsigned int reg_c_irq_clr7:16;

	// h0022
	unsigned int /* padding 16 bit */:16;

	// h0023, bit: 14
	/* Status of Interrupt on CPU side*/
	#define offset_of_reg_irq_final_status7 (70)
	#define mask_of_reg_irq_final_status7 (0xffff)
	unsigned int reg_irq_final_status7:16;

	// h0023
	unsigned int /* padding 16 bit */:16;

	// h0024, bit: 14
	/* Status of Interrupt on IP side (DMA Done)
	# [0] : Group2 ch0  line cnt0  hit
	# [1] : Group2 ch0  line cnt1  hit
	# [2] : Group2 ch0  line cnt2  hit
	# [3] : Group2 ch0  line cnt3  hit
	# [4] : Group2 ch1  line cnt0  hit
	# [5] : Group2 ch1  line cnt1  hit
	# [6] : Group2 ch1  line cnt2  hit
	# [7] : Group2 ch1  line cnt3  hit
	# [8] : Group2 ch2  line cnt0  hit
	# [9] : Group2 ch2  line cnt1  hit
	# [10] : Group2 ch2  line cnt2  hit
	# [11] : Group2 ch2  line cnt3  hit
	# [12] : Group2 ch3  line cnt0  hit
	# [13] : Group2 ch3  line cnt1  hit
	# [14] : Group2 ch3  line cnt2  hit
	# [15] : Group2 ch3  line cnt3  hit*/
	#define offset_of_reg_irq_raw_status7 (72)
	#define mask_of_reg_irq_raw_status7 (0xffff)
	unsigned int reg_irq_raw_status7:16;

	// h0024
	unsigned int /* padding 16 bit */:16;

	// h0025, bit: 14
	/* Interrupt Mask*/
	#define offset_of_reg_c_irq_mask8 (74)
	#define mask_of_reg_c_irq_mask8 (0xffff)
	unsigned int reg_c_irq_mask8:16;

	// h0025
	unsigned int /* padding 16 bit */:16;

	// h0026, bit: 14
	/* Force Interrupt Enable*/
	#define offset_of_reg_c_irq_force8 (76)
	#define mask_of_reg_c_irq_force8 (0xffff)
	unsigned int reg_c_irq_force8:16;

	// h0026
	unsigned int /* padding 16 bit */:16;

	// h0027, bit: 14
	/* Interrupt Clear*/
	#define offset_of_reg_c_irq_clr8 (78)
	#define mask_of_reg_c_irq_clr8 (0xffff)
	unsigned int reg_c_irq_clr8:16;

	// h0027
	unsigned int /* padding 16 bit */:16;

	// h0028, bit: 14
	/* Status of Interrupt on CPU side*/
	#define offset_of_reg_irq_final_status8 (80)
	#define mask_of_reg_irq_final_status8 (0xffff)
	unsigned int reg_irq_final_status8:16;

	// h0028
	unsigned int /* padding 16 bit */:16;

	// h0029, bit: 14
	/* Status of Interrupt on IP side (DMA Done)
	# [0] : Group3 ch0  line cnt0  hit
	# [1] : Group3 ch0  line cnt1  hit
	# [2] : Group3 ch0  line cnt2  hit
	# [3] : Group3 ch0  line cnt3  hit
	# [4] : Group3 ch1  line cnt0  hit
	# [5] : Group3 ch1  line cnt1  hit
	# [6] : Group3 ch1  line cnt2  hit
	# [7] : Group3 ch1  line cnt3  hit
	# [8] : Group3 ch2  line cnt0  hit
	# [9] : Group3 ch2  line cnt1  hit
	# [10] : Group3 ch2  line cnt2  hit
	# [11] : Group3 ch2  line cnt3  hit
	# [12] : Group3 ch3  line cnt0  hit
	# [13] : Group3 ch3  line cnt1  hit
	# [14] : Group3 ch3  line cnt2  hit
	# [15] : Group3 ch3  line cnt3  hit*/
	#define offset_of_reg_irq_raw_status8 (82)
	#define mask_of_reg_irq_raw_status8 (0xffff)
	unsigned int reg_irq_raw_status8:16;

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
	/* Interrupt Mask*/
	#define offset_of_reg_cmdq_irq_mask (96)
	#define mask_of_reg_cmdq_irq_mask (0xffff)
	unsigned int reg_cmdq_irq_mask:16;

	// h0030
	unsigned int /* padding 16 bit */:16;

	// h0031, bit: 14
	/* Force Interrupt Enable*/
	#define offset_of_reg_cmdq_irq_force (98)
	#define mask_of_reg_cmdq_irq_force (0xffff)
	unsigned int reg_cmdq_irq_force:16;

	// h0031
	unsigned int /* padding 16 bit */:16;

	// h0032, bit: 14
	/* Interrupt Clear*/
	#define offset_of_reg_cmdq_irq_clr (100)
	#define mask_of_reg_cmdq_irq_clr (0xffff)
	unsigned int reg_cmdq_irq_clr:16;

	// h0032
	unsigned int /* padding 16 bit */:16;

	// h0033, bit: 14
	/* Status of Interrupt on CMDQ side*/
	#define offset_of_reg_cmdq_irq_final_status (102)
	#define mask_of_reg_cmdq_irq_final_status (0xffff)
	unsigned int reg_cmdq_irq_final_status:16;

	// h0033
	unsigned int /* padding 16 bit */:16;

	// h0034, bit: 14
	/* Status of Interrupt on IP side (DMA Done)
	# [0] : Group0 Channel0 yc done
	# [1] : Group0 Channel1 yc done
	# [2] : Group0 Channel2 yc done
	# [3] : Group0 Channel3 yc done
	# [4] : Group1 Channel0 yc done
	# [5] : Group1 Channel1 yc done
	# [6] : Group1 Channel2 yc done
	# [7] : Group1 Channel3 yc done
	# [8] : Group2 Channel0 yc done
	# [9] : Group2 Channel1 yc done
	# [10] : Group2 Channel2 yc done
	# [11] : Group2 Channel3 yc done
	# [12] : Group3 Channel0 yc done
	# [13] : Group3 Channel1 yc done
	# [14] : Group3 Channel2 yc done
	# [15] : Group3 Channel3 yc done*/
	#define offset_of_reg_cmdq_irq_raw_status (104)
	#define mask_of_reg_cmdq_irq_raw_status (0xffff)
	unsigned int reg_cmdq_irq_raw_status:16;

	// h0034
	unsigned int /* padding 16 bit */:16;

	// h0035, bit: 14
	/* Interrupt Mask*/
	#define offset_of_reg_cmdq_irq_mask1 (106)
	#define mask_of_reg_cmdq_irq_mask1 (0xffff)
	unsigned int reg_cmdq_irq_mask1:16;

	// h0035
	unsigned int /* padding 16 bit */:16;

	// h0036, bit: 14
	/* Force Interrupt Enable*/
	#define offset_of_reg_cmdq_irq_force1 (108)
	#define mask_of_reg_cmdq_irq_force1 (0xffff)
	unsigned int reg_cmdq_irq_force1:16;

	// h0036
	unsigned int /* padding 16 bit */:16;

	// h0037, bit: 14
	/* Interrupt Clear*/
	#define offset_of_reg_cmdq_irq_clr1 (110)
	#define mask_of_reg_cmdq_irq_clr1 (0xffff)
	unsigned int reg_cmdq_irq_clr1:16;

	// h0037
	unsigned int /* padding 16 bit */:16;

	// h0038, bit: 14
	/* Status of Interrupt on CMDQ side*/
	#define offset_of_reg_cmdq_irq_final_status1 (112)
	#define mask_of_reg_cmdq_irq_final_status1 (0xffff)
	unsigned int reg_cmdq_irq_final_status1:16;

	// h0038
	unsigned int /* padding 16 bit */:16;

	// h0039, bit: 14
	/* Status of Interrupt on IP side
	#[0]: ISP busy rising edge
	#[1]: ISP busy falling edge
	#[2]: hit input line count1
	#[3]:  P1 hit input line count1
	#[4]: WDMA done
	#[5]: RDMA done
	#[6] rdma0 done
	#[7] rdma1 done
	#[8] wdma0 done
	#[9] wdma1 done
	#[10] wdma2 done
	#[11] wdma4 done
	#[12] rdma4 done
	#[13-15] reserved*/
	#define offset_of_reg_cmdq_irq_raw_status1 (114)
	#define mask_of_reg_cmdq_irq_raw_status1 (0xffff)
	unsigned int reg_cmdq_irq_raw_status1:16;

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

}  __attribute__((packed, aligned(1))) infinity2_reg_isp_dma_irq;
#endif
