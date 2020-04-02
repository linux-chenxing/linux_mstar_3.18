//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>

#include "eMMC.h"
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>

#if defined(UNIFIED_eMMC_DRIVER) && UNIFIED_eMMC_DRIVER

U32 gu32_eMMCDrvExtFlag = 0;

#if defined(IF_FCIE_SHARE_IP) && IF_FCIE_SHARE_IP
// common for TV linux platform
extern bool ms_sdmmc_card_chg(unsigned int slotNo);
#endif

extern struct platform_device sg_mstar_emmc_device_st;

U32 eMMC_hw_timer_delay(U32 u32us)
{
	#if 0 // udelay not work
    volatile u32 i = 0;

	for (i = 0; i < (u32us>>0); i++)
	{
		#if 1
		volatile int j = 0, tmp;
		for (j = 0; j < 0x38; j++)
		{
			tmp = j;
		}
		#endif
	}
	#else
    U32 u32_i = u32us;

	while(u32_i > 1000)
	{
		mdelay(1);
		u32_i -= 1000;
	}
    udelay(u32_i);
	#endif
	return u32us;
}


U32 eMMC_hw_timer_sleep(U32 u32ms)
{
	U32 u32_i = u32ms;

	while(u32_i > 1000)
	{
	    msleep(1000);
		u32_i -= 1000;
	}

	msleep(u32_i);
    return u32ms;
}


//--------------------------------
// use to performance test
static U32 u32_ms_start;
U32 eMMC_hw_timer_start(void)
{
	u32_ms_start = jiffies_to_usecs(jiffies);
	return eMMC_ST_SUCCESS;
}

U32 eMMC_hw_timer_tick(void)
{
	return jiffies_to_usecs(jiffies) - u32_ms_start;
}


//--------------------------------
void eMMC_set_WatchDog(U8 u8_IfEnable)
{
	// do nothing
}

void eMMC_reset_WatchDog(void)
{
	// do nothing
}


//---------------------------------------
U32 eMMC_translate_DMA_address_Ex(U32 u32_DMAAddr, U32 u32_ByteCnt)
{
    if( u32_DMAAddr >= MSTAR_MIU1_BUS_BASE)
	{
		REG_FCIE_SETBIT( FCIE_MIU_DMA_26_16, BIT_MIU1_SELECT);
		u32_DMAAddr -= MSTAR_MIU1_BUS_BASE;
	}
	else
	{
		REG_FCIE_CLRBIT( FCIE_MIU_DMA_26_16, BIT_MIU1_SELECT);
		u32_DMAAddr -= MSTAR_MIU0_BUS_BASE;
	}

	return u32_DMAAddr;


}

U32 eMMC_DMA_MAP_address(U32 u32_Buffer, U32 u32_ByteCnt, int mode)
{
	dma_addr_t dma_addr;
	#if defined(CONFIG_ENABLE_EMMC_ACP) && CONFIG_ENABLE_EMMC_ACP
	dma_addr = dma_map_single(&sg_mstar_emmc_device_st.dev, (void*)u32_Buffer, u32_ByteCnt, DMA_ACP);
	#else
	if(mode == 0)	//write
	{
		dma_addr = dma_map_single(&sg_mstar_emmc_device_st.dev, (void*)u32_Buffer, u32_ByteCnt, DMA_TO_DEVICE);
	}
	else
	{
		dma_addr = dma_map_single(&sg_mstar_emmc_device_st.dev, (void*)u32_Buffer, u32_ByteCnt, DMA_FROM_DEVICE);
	}

    if( dma_mapping_error(&sg_mstar_emmc_device_st.dev, dma_addr) )
    {
        dma_unmap_single(&sg_mstar_emmc_device_st.dev, dma_addr, u32_ByteCnt, (mode) ? DMA_FROM_DEVICE : DMA_TO_DEVICE);
        eMMC_die("eMMC_DMA_MAP_address: Kernel can't mapping dma correctly\n");
    }

	#endif

	return dma_addr;
}

void eMMC_DMA_UNMAP_address(U32 u32_DMAAddr, U32 u32_ByteCnt, int mode)
{
	#if defined(CONFIG_ENABLE_EMMC_ACP) && CONFIG_ENABLE_EMMC_ACP
	dma_unmap_single(&sg_mstar_emmc_device_st.dev, u32_DMAAddr, u32_ByteCnt, DMA_ACP);

	#else
	if(mode == 0)	//write
	{
		dma_unmap_single(&sg_mstar_emmc_device_st.dev, u32_DMAAddr, u32_ByteCnt, DMA_TO_DEVICE);
	}
	else
	{
		dma_unmap_single(&sg_mstar_emmc_device_st.dev, u32_DMAAddr, u32_ByteCnt, DMA_FROM_DEVICE);
	}
	#endif
}


void eMMC_flush_data_cache_buffer(U32 u32_DMAAddr, U32 u32_ByteCnt)
{
    Chip_Clean_Cache_Range_VA_PA(u32_DMAAddr,__pa(u32_DMAAddr), u32_ByteCnt);
}

void eMMC_Invalidate_data_cache_buffer(U32 u32_DMAAddr, U32 u32_ByteCnt)
{
    Chip_Inv_Cache_Range_VA_PA(u32_DMAAddr,__pa(u32_DMAAddr), u32_ByteCnt);
}

void eMMC_flush_miu_pipe(void)
{

}


void eMMC_DumpChiptop(void)
{
	U16 u16_i, u16_reg;

	eMMC_debug(eMMC_DEBUG_LEVEL, 1, "\n\n chiptop:");

	for(u16_i=0 ; u16_i<0x80; u16_i++)
	{
		if(0 == u16_i%8)
			eMMC_debug(eMMC_DEBUG_LEVEL,0,"\n%02Xh:| ", u16_i);

		REG_FCIE_R(GET_REG_ADDR(PAD_CHIPTOP_BASE, u16_i), u16_reg);
		eMMC_debug(eMMC_DEBUG_LEVEL, 0, "%04Xh ", u16_reg);

		if((u16_i & 0x7) == 0x7)
			eMMC_debug(eMMC_DEBUG_LEVEL, 0, "| %02Xh ", u16_i);
	}
}

extern U32 gu32_pwrsvr_gpio_enable;
extern U32 gu32_pwrsvr_gpio_addr;
extern U32 gu32_pwrsvr_gpio_bit;
extern U32 gu32_pwrsvr_gpio_trigger;

void eMMC_CheckPowerCut(void)
{
}

void eMMC_Prepare_Power_Saving_Mode_Queue(void)
{
    REG_FCIE_SETBIT(reg_pwrgd_int_glirm, BIT_PWRGD_INT_GLIRM_EN);

    REG_FCIE_CLRBIT(reg_pwrgd_int_glirm, BIT_PWEGD_INT_GLIRM_MASK);
    REG_FCIE_SETBIT(reg_pwrgd_int_glirm, (1<<10));

    /* (1) Clear HW Enable */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x40), 0x0000);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x41),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0A);

	/* (2) Clear All Interrupt */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x42), 0xffff);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x43),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x00);

	/* (3) Clear SDE MODE Enable */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x44), 0x0000);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x45),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x10);

	/* (4) Clear SDE CTL Enable */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x46), 0x0000);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x47),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x11);

	/* (5) Reset Start */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x48), 0x4800);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x49),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x30);

	/* (6) Reset End */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x4A), 0x5800);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x4B),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x30);

	/* (7) Set "SD_MOD" */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x4C), 0x0051);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x4D),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x10);

	/* (8) Enable "csreg_sd_en" */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x4E), 0x0002);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x4F),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0A);

	/* (9) Command Content, IDLE */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x50), 0x0040);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x51),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x60);

	/* (10) Command Content, STOP */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x52), 0x0000);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x53),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x61);

	/* (11) Command Content, STOP */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x54), 0x0000);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x55),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x62);

	/* (12) Command Size */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x56), 0x0005);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x57),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0E);

	/* (13) Response Size */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x58), 0x0000);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x59),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0D);

	/* (14) Enable Interrupt, SD_CMD_END */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x5A), 0x0002);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x5B),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x01);

	/* (15) Command Enable */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x5C), 0x0004);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x5D),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x11);

	/* (16) Wait Interrupt */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x5E), 0x0000);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x5F),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WINT);

	/* (17) Clear Interrupt */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x60), 0x0002);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x61),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x00);

    /* (18) Clear HW Enable */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x62), 0x0000);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x63),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0A);

	/* (19) STOP */
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x64), 0x0000);
	REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x65),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_STOP);

    REG_FCIE_W(FCIE_PWR_SAVE_MODE, BIT_POWER_SAVE_MODE_EN | BIT_SD_POWER_SAVE_RST);
}


//--------------------------------
#if defined(ENABLE_eMMC_ATOP) && ENABLE_eMMC_ATOP

U8 gau8_eMMCPLLSel_52[eMMC_FCIE_VALID_CLK_CNT] = {
	eMMC_PLL_CLK__52M,
	eMMC_PLL_CLK__32M,
    eMMC_PLL_CLK__27M
};

U8 gau8_eMMCPLLSel_200[eMMC_FCIE_VALID_CLK_CNT] = {
	eMMC_PLL_CLK_200M,
	eMMC_PLL_CLK_160M,
	eMMC_PLL_CLK_140M
};

static  eMMC_FCIE_ATOP_SET_t sgTSetTmp_t;

static U32 eMMC_FCIE_DetectTiming_Ex(U32 u32_Skew4Result)
{
	U16 u16_i, u16_StartBit=0xFFFF, u16_EndBit=0xFFFF, u16_cnt;
	U32 u32_tmp;

	sgTSetTmp_t.u32_ScanResult = u32_Skew4Result;

	// ==================================================
	// case.1: all bits set
	if((u32_Skew4Result & ((1<<PLL_SKEW4_CNT*2)-1)) == ((1<<PLL_SKEW4_CNT*2)-1) )
	{
		eMMC_debug(eMMC_DEBUG_LEVEL,0,"  case1: all\n");
		sgTSetTmp_t.u8_Reg2Ch=1;
		sgTSetTmp_t.u8_Skew4=0;
	}
	// ==================================================
	// case.2: not wrapped
	else if( 0==(u32_Skew4Result&BIT0) || 0==(u32_Skew4Result&(1<<(PLL_SKEW4_CNT*2-1))) )
	{
		for(u16_i=0; u16_i<PLL_SKEW4_CNT*2; u16_i++)
		{
			if((u32_Skew4Result&1<<u16_i) && 0xFFFF==u16_StartBit)
				u16_StartBit = u16_i;
			else if(0xFFFF!=u16_StartBit && 0==(u32_Skew4Result&1<<u16_i))
				u16_EndBit = u16_i-1;

			if(0xFFFF != u16_EndBit)
			{
				if(u16_EndBit-u16_StartBit+1<MIN_OK_SKEW_CNT)
				{	// to ignore "discontinous case"
					u16_StartBit = 0xFFFF;
					u16_EndBit = 0xFFFF;
				}
				else
					break;
			}
		}
		if(0xFFFF==u16_EndBit)
		{
			if( u32_Skew4Result&1<<(u16_i-1) )
				u16_EndBit = u16_i-1;
			else
				u16_EndBit = u16_StartBit;
		}
		eMMC_debug(eMMC_DEBUG_LEVEL,0,"  case2: not wrapped: %u %u\n", u16_StartBit, u16_EndBit);

		if(u16_EndBit-u16_StartBit+1<MIN_OK_SKEW_CNT)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: Clk:%uMHz Case2 not enough skew4: %Xh %Xh\n",
				g_eMMCDrv.u32_ClkKHz/1000, u32_Skew4Result, u16_EndBit-u16_StartBit+1);
			return eMMC_ST_ERR_SKEW4;
		}

		if( (u16_StartBit+u16_EndBit)/2 < PLL_SKEW4_CNT)
		{
			sgTSetTmp_t.u8_Reg2Ch=0;
			sgTSetTmp_t.u8_Skew4=(u16_StartBit+u16_EndBit)/2;
		}
		else
		{	sgTSetTmp_t.u8_Reg2Ch=1;
			sgTSetTmp_t.u8_Skew4=(u16_StartBit+u16_EndBit)/2 -PLL_SKEW4_CNT;
		}
	}
	// ==================================================
	// case.3: wrapped
	else
	{	// --------------------------------
		// to ignore "discontinous case"
		u16_cnt = 0;
		for(u16_i=0; u16_i<PLL_SKEW4_CNT*2; u16_i++)
		{
			if(u32_Skew4Result&1<<u16_i)
				u16_cnt++;
			else
			{	u16_StartBit = u16_i-1;
				break;
			}
		}
		for(u16_i=PLL_SKEW4_CNT*2-1; u16_i>0; u16_i--)
		{
			if(u32_Skew4Result&1<<u16_i)
				u16_cnt++;
			else
			{	u16_EndBit = u16_i+1;
				break;
			}
		}
		if(u16_cnt < MIN_OK_SKEW_CNT)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL,0,"  case3: wrapped but goto case2: %u, %u %u\n",
				u16_cnt, u16_StartBit, u16_EndBit);
			u32_Skew4Result &= ~((1<<(u16_StartBit+1))-1);
			return eMMC_FCIE_DetectTiming_Ex(u32_Skew4Result);
		}
		else
		{	u32_tmp = u32_Skew4Result;
			for(u16_i=u16_StartBit+1; u16_i<u16_EndBit; u16_i++)
				u32_Skew4Result &= ~(1<<u16_i);

			u16_StartBit = 0xFFFF;
			u16_EndBit = 0xFFFF;
		}

		// --------------------------------
		// normal judgement
		for(u16_i=0; u16_i<PLL_SKEW4_CNT*2; u16_i++)
		{
			if(0==(u32_Skew4Result&1<<u16_i) && 0xFFFF==u16_StartBit)
				u16_StartBit = u16_i-1;
			else if(0xFFFF!=u16_StartBit && (u32_Skew4Result&1<<u16_i))
				u16_EndBit = u16_i;

			if(0xFFFF != u16_EndBit)
				break;
		}
		eMMC_debug(eMMC_DEBUG_LEVEL,0,"  case3: wrapped: %u %u, %Xh %Xh\n",
			u16_StartBit, u16_EndBit, u32_tmp, u32_Skew4Result);

		if(u16_StartBit+1 > PLL_SKEW4_CNT*2-u16_EndBit)
		{
			sgTSetTmp_t.u8_Reg2Ch=0;
			sgTSetTmp_t.u8_Skew4 = (u16_StartBit-(PLL_SKEW4_CNT*2-u16_EndBit))/2;
		}
		else
		{	sgTSetTmp_t.u8_Reg2Ch=1;
			sgTSetTmp_t.u8_Skew4 = u16_EndBit +
				((PLL_SKEW4_CNT*2-u16_EndBit)+u16_StartBit)/2 -
				PLL_SKEW4_CNT;
		}
	}

	eMMC_debug(eMMC_DEBUG_LEVEL, 0, "  %Xh %Xh \n", sgTSetTmp_t.u8_Reg2Ch, sgTSetTmp_t.u8_Skew4);
	if(sgTSetTmp_t.u8_Skew4 >= PLL_SKEW4_CNT)
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: Skew4:%u > %u\n", sgTSetTmp_t.u8_Skew4, PLL_SKEW4_CNT);

	return eMMC_ST_SUCCESS;
}


U32 eMMC_FCIE_DetectDDRTiming(void)
{
	U32 u32_i, u32_err, u32_Skew4Result=0;
	U16 u16_SkewCnt=0;

	g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_TUNING_TTABLE; // turn on tunning flag

	REG_FCIE_CLRBIT(FCIE_SM_STS, BIT11);
	for(u32_i=0; u32_i<PLL_SKEW4_CNT; u32_i++)
	{
		// make sure a complete outside clock cycle
		REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_SD_CLK_EN);

		REG_FCIE_CLRBIT(reg_emmcpll_0x03, BIT_SKEW4_MASK);
		REG_FCIE_SETBIT(reg_emmcpll_0x03, u32_i<<12);
		u32_err = eMMCTest_BlkWRC_ProbeTiming(eMMC_TEST_BLK_0);
		if(eMMC_ST_SUCCESS==u32_err)
		{
			u16_SkewCnt++;
			u32_Skew4Result |= (1<<u32_i);
		}
	}

	REG_FCIE_SETBIT(FCIE_SM_STS, BIT11);
	for(u32_i=PLL_SKEW4_CNT; u32_i<PLL_SKEW4_CNT*2; u32_i++)
	{
		// make sure a complete outside clock cycle
		REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_SD_CLK_EN);

		REG_FCIE_CLRBIT(reg_emmcpll_0x03, BIT_SKEW4_MASK);
		REG_FCIE_SETBIT(reg_emmcpll_0x03, (u32_i-PLL_SKEW4_CNT)<<12);
		u32_err = eMMCTest_BlkWRC_ProbeTiming(eMMC_TEST_BLK_0);
		if(eMMC_ST_SUCCESS==u32_err)
		{
			u16_SkewCnt++;
			u32_Skew4Result |= (1<<u32_i);
		}
	}

	g_eMMCDrv.u32_DrvFlag &= ~DRV_FLAG_TUNING_TTABLE; // turn off tunning flag

	if(0==u32_Skew4Result || u16_SkewCnt<MIN_OK_SKEW_CNT)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: Clk: %uMHz SkewResult: %Xh SkewCnt: %u\n",
			g_eMMCDrv.u32_ClkKHz, u32_Skew4Result, u16_SkewCnt);
		return eMMC_ST_ERR_SKEW4;
	}

    return eMMC_FCIE_DetectTiming_Ex(u32_Skew4Result);
}


U32 eMMC_FCIE_BuildDDRTimingTable(void)
{
	U32 u32_err, u32_ret;
	U8  u8_SetIdx, u8_ClkIdx;

	eMMC_debug(eMMC_DEBUG_LEVEL,1,"eMMC Info: building DDR table, please wait... \n");
	memset((void*)&g_eMMCDrv.TimingTable_t, 0, sizeof(g_eMMCDrv.TimingTable_t));
	u8_SetIdx = 0;

	if(!eMMC_IF_NORMAL_SDR())
	{
		u32_err = eMMC_FCIE_EnableSDRMode();
		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,0,"eMMC Err: set SDR IF fail: %X\n", u32_err);
			return u32_err;
		}
	}

	u32_err = eMMC_FCIE_EnableFastMode_Ex(FCIE_eMMC_DDR);
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,0,"eMMC Err: set DDR IF fail: %X\n", u32_err);
		return u32_err;
	}

	for(u8_ClkIdx=0; u8_ClkIdx<eMMC_FCIE_VALID_CLK_CNT; u8_ClkIdx++)
	{
		eMMC_clock_setting(gau8_eMMCPLLSel_52[u8_ClkIdx]);
		eMMC_debug(eMMC_DEBUG_LEVEL_HIGH,0,"\nclk: %uMHz\n", g_eMMCDrv.u32_ClkKHz/1000);

		// ---------------------------
		// search and set the Windows
		u32_err = eMMC_FCIE_DetectDDRTiming();

		// ---------------------------
		// set the Table
		if(eMMC_ST_SUCCESS == u32_err)
		{
			g_eMMCDrv.TimingTable_t.u8_SetCnt++;
			g_eMMCDrv.TimingTable_t.Set[u8_ClkIdx].u8_Clk = gau8_eMMCPLLSel_52[u8_ClkIdx];
			g_eMMCDrv.TimingTable_t.Set[u8_ClkIdx].u8_Reg2Ch = sgTSetTmp_t.u8_Reg2Ch;
			g_eMMCDrv.TimingTable_t.Set[u8_ClkIdx].u8_Skew4 = sgTSetTmp_t.u8_Skew4;
			g_eMMCDrv.TimingTable_t.Set[u8_ClkIdx].u32_ScanResult = sgTSetTmp_t.u32_ScanResult;
			g_eMMCDrv.TimingTable_t.u32_VerNo = eMMC_TIMING_TABLE_VERSION;
			eMMC_DumpTimingTable();
			eMMC_debug(eMMC_DEBUG_LEVEL, 0, "\nOK\n");

			g_eMMCDrv.TimingTable_t.u32_ChkSum =
		        eMMC_ChkSum((U8*)&g_eMMCDrv.TimingTable_t,
		        sizeof(g_eMMCDrv.TimingTable_t)-eMMC_TIMING_TABLE_CHKSUM_OFFSET);
		    memcpy(gau8_eMMC_SectorBuf, (U8*)&g_eMMCDrv.TimingTable_t, sizeof(g_eMMCDrv.TimingTable_t));

		    eMMC_FCIE_ErrHandler_ReInit();

			u32_err = eMMC_CMD24(eMMC_DDRTABLE_BLK_0, gau8_eMMC_SectorBuf);
	        u32_ret = eMMC_CMD24(eMMC_DDRTABLE_BLK_1, gau8_eMMC_SectorBuf);
	        if(eMMC_ST_SUCCESS!=u32_err && eMMC_ST_SUCCESS!=u32_ret)
	        {
		        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: Save TTable fail: %Xh %Xh\n",
			        u32_err, u32_ret);
		        return eMMC_ST_ERR_SAVE_DDRT_FAIL;
	        }

	        return eMMC_ST_SUCCESS;
		}
	}

	eMMC_DumpTimingTable();
	eMMC_die("");
	return eMMC_ST_ERR_SKEW4;

}


U32 eMMC_FCIE_ResetToHS200(U8 u8_ClkParam)
{
	U32 u32_err;

	// --------------------------------
	// reset FCIE & eMMC to normal SDR mode
	if(!eMMC_IF_NORMAL_SDR())
	{
		//eMMC_debug(0,1,"eMMC: re-init to SDR\n");
		g_eMMCDrv.u32_DrvFlag = 0;
	    eMMC_PlatformInit();
		u32_err = eMMC_FCIE_Init();
	    if(u32_err)
	    {
		    eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_FCIE_Init fail, %Xh\n", u32_err);
		    return u32_err;
	    }
		u32_err = eMMC_Init_Device_Ex();
		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,0,"eMMC Err: eMMC_Init_Device_Ex fail: %X\n", u32_err);
			return u32_err;
		}
	}

	// --------------------------------
	// set eMMC to HS200 mode
	u32_err = eMMC_FCIE_EnableFastMode_Ex(FCIE_eMMC_HS200);
	if(eMMC_ST_SUCCESS != u32_err)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,0,"eMMC Err: set HS200 IF fail: %X\n", u32_err);

		return u32_err;
	}

	eMMC_clock_setting(u8_ClkParam);

	return u32_err;
}

U32 eMMC_FCIE_DetectHS200Timing(void)
{
	U32 u32_i, u32_err, u32_Skew4Result=0;
	U16 u16_SkewCnt=0;

	for(u32_i=0; u32_i<PLL_SKEW4_CNT; u32_i++)
	{
		// make sure a complete outside clock cycle
		REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_SD_CLK_EN);

		g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_TUNING_TTABLE; // turn on tunning flag
		REG_FCIE_CLRBIT(FCIE_SM_STS, BIT11);

		REG_FCIE_CLRBIT(reg_emmcpll_0x03, BIT_SKEW4_MASK);
		REG_FCIE_SETBIT(reg_emmcpll_0x03, u32_i<<12);
		u32_err = eMMC_CMD21();
		if(eMMC_ST_SUCCESS==u32_err && eMMC_ST_SUCCESS==eMMCTest_BlkWRC_ProbeTiming(eMMC_TEST_BLK_0))
		{
			u16_SkewCnt++;
			u32_Skew4Result |= (1<<u32_i);
		}
		else
		{	//u32_err = eMMC_FCIE_Init();
		    eMMC_debug(0,1,"failed skew4: %u\n", u32_i);
			u32_err = eMMC_FCIE_ResetToHS200((U8)g_eMMCDrv.u16_ClkRegVal);
			if(eMMC_ST_SUCCESS != u32_err)
			    break;
		}
	}

	for(u32_i=PLL_SKEW4_CNT; u32_i<PLL_SKEW4_CNT*2; u32_i++)
	{
		// make sure a complete outside clock cycle
		REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_SD_CLK_EN);

		g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_TUNING_TTABLE; // turn on tunning flag
		REG_FCIE_SETBIT(FCIE_SM_STS, BIT11);

		REG_FCIE_CLRBIT(reg_emmcpll_0x03, BIT_SKEW4_MASK);
		REG_FCIE_SETBIT(reg_emmcpll_0x03, (u32_i-PLL_SKEW4_CNT)<<12);
		u32_err = eMMC_CMD21();
		if(eMMC_ST_SUCCESS==u32_err && eMMC_ST_SUCCESS==eMMCTest_BlkWRC_ProbeTiming(eMMC_TEST_BLK_0))
		{
			u16_SkewCnt++;
			u32_Skew4Result |= (1<<u32_i);
		}
		else
		{	//u32_err = eMMC_FCIE_Init();
		    eMMC_debug(0,1,"failed skew4: %u\n", u32_i);
			u32_err = eMMC_FCIE_ResetToHS200((U8)g_eMMCDrv.u16_ClkRegVal);
			if(eMMC_ST_SUCCESS != u32_err)
			    break;
		}
	}

	g_eMMCDrv.u32_DrvFlag &= ~DRV_FLAG_TUNING_TTABLE; // turn off tunning flag

	if(0==u32_Skew4Result || u16_SkewCnt<MIN_OK_SKEW_CNT)
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: Clk: %uMHz SkewResult: %Xh SkewCnt: %u\n",
			g_eMMCDrv.u32_ClkKHz, u32_Skew4Result, u16_SkewCnt);
		return eMMC_ST_ERR_SKEW4;
	}

	return eMMC_FCIE_DetectTiming_Ex(u32_Skew4Result);
}


U32 eMMC_FCIE_BuildHS200TimingTable(void)
{
	U32 u32_err, u32_ret;
	U8  u8_SetIdx, u8_ClkIdx;

	eMMC_debug(eMMC_DEBUG_LEVEL,0,"eMMC Info: building HS200 table, please wait... \n");
	memset((void*)&g_eMMCDrv.TimingTable_t, 0, sizeof(g_eMMCDrv.TimingTable_t));
	u8_SetIdx = 0;

	// --------------------------------
	// tuning FCIE & macro for HS200
	for(u8_ClkIdx=0; u8_ClkIdx<eMMC_FCIE_VALID_CLK_CNT; u8_ClkIdx++)
	{
		u32_err = eMMC_FCIE_ResetToHS200(gau8_eMMCPLLSel_200[u8_ClkIdx]);
		if(eMMC_ST_SUCCESS != u32_err)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: eMMC_FCIE_ResetToHS200 fail: %Xh\n", u32_err);
			return u32_err;
		}
		eMMC_debug(eMMC_DEBUG_LEVEL,0,"\nclk: %uMHz\n", g_eMMCDrv.u32_ClkKHz/1000);

		// ---------------------------
		// search and set the Windows
		u32_err = eMMC_FCIE_DetectHS200Timing();
		// ---------------------------
		// set the Table
		if(eMMC_ST_SUCCESS == u32_err)
		{
			//eMMC_FCIE_Init();
			g_eMMCDrv.TimingTable_t.u8_SetCnt++;
			g_eMMCDrv.TimingTable_t.Set[0].u8_Clk = gau8_eMMCPLLSel_200[u8_ClkIdx];
			g_eMMCDrv.TimingTable_t.Set[0].u8_Reg2Ch = sgTSetTmp_t.u8_Reg2Ch;
			g_eMMCDrv.TimingTable_t.Set[0].u8_Skew4 = sgTSetTmp_t.u8_Skew4;
			g_eMMCDrv.TimingTable_t.Set[0].u32_ScanResult = sgTSetTmp_t.u32_ScanResult;
            g_eMMCDrv.TimingTable_t.u32_VerNo = eMMC_TIMING_TABLE_VERSION;
			eMMC_DumpTimingTable();
	        eMMC_debug(eMMC_DEBUG_LEVEL, 0, "\nOK\n");

			g_eMMCDrv.TimingTable_t.u32_ChkSum =
		        eMMC_ChkSum((U8*)&g_eMMCDrv.TimingTable_t,
		        sizeof(g_eMMCDrv.TimingTable_t)-eMMC_TIMING_TABLE_CHKSUM_OFFSET);
		    memcpy(gau8_eMMC_SectorBuf, (U8*)&g_eMMCDrv.TimingTable_t, sizeof(g_eMMCDrv.TimingTable_t));

			eMMC_FCIE_ErrHandler_ReInit();

			if(!eMMC_CHK_BKG_SCAN_HS200()){
			u32_err = eMMC_CMD24(eMMC_HS200TABLE_BLK_0, gau8_eMMC_SectorBuf);
	        u32_ret = eMMC_CMD24(eMMC_HS200TABLE_BLK_1, gau8_eMMC_SectorBuf);
	        if(eMMC_ST_SUCCESS!=u32_err && eMMC_ST_SUCCESS!=u32_ret)
	        {
		        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: Save TTable fail: %Xh %Xh\n",
			        u32_err, u32_ret);
		        return eMMC_ST_ERR_SAVE_DDRT_FAIL;
	        }}
			return eMMC_ST_SUCCESS;
		}
	}

	eMMC_DumpTimingTable();
	eMMC_die("");
	return eMMC_ST_ERR_SKEW4;
}

#endif


void eMMC_dump_eMMCPLL(void);
U32  eMMC_FCIE_HS200_IrwinTest(void);

// check some fix value, print only when setting wrong
void eMMC_DumpPadClk(void)
{
	U16 u16_i, u16_reg;

	//---------------------------------------------------------------------
	eMMC_debug(eMMC_DEBUG_LEVEL, 1, "reg_ckg_fcie(0x%X):0x%x", reg_ckg_fcie ,REG_FCIE_U16(reg_ckg_fcie));
	eMMC_debug(eMMC_DEBUG_LEVEL, 0,"  FCIE Clk: %uKHz\n", g_eMMCDrv.u32_ClkKHz);
	eMMC_debug(eMMC_DEBUG_LEVEL, 0,"  Reg Val: %Xh\n", g_eMMCDrv.u16_ClkRegVal);

	eMMC_dump_eMMCPLL();

	//---------------------------------------------------------------------
	eMMC_debug(0, 0, "\n\n[pad setting]: ");
	switch(g_eMMCDrv.u8_PadType)
	{
		case FCIE_eMMC_BYPASS:	eMMC_debug(0, 0, "BYPASS\n");	break;
		case FCIE_eMMC_SDR: eMMC_debug(0, 0, "SDR\n");	break;
		case FCIE_eMMC_DDR: eMMC_debug(0, 0, "DDR\n");	break;
		case FCIE_eMMC_HS200:	eMMC_debug(0, 0, "HS200\n");	break;
		default:
			eMMC_debug(0, 0, "eMMC Err: Pad unknown, %d\n", g_eMMCDrv.u8_PadType); eMMC_die("\n");
			break;
	}

	eMMC_debug(eMMC_DEBUG_LEVEL, 0, "\nchiptop:");

	for(u16_i=0 ; u16_i<0x80; u16_i++)
	{
		if(0 == u16_i%8)
			eMMC_debug(eMMC_DEBUG_LEVEL,0,"\n%02Xh:| ", u16_i);

		REG_FCIE_R(GET_REG_ADDR(PAD_CHIPTOP_BASE, u16_i), u16_reg);
		eMMC_debug(eMMC_DEBUG_LEVEL, 0, "%04Xh ", u16_reg);
	}

	eMMC_debug(eMMC_DEBUG_LEVEL, 0, "\n");
}


// set pad first, then config clock
U32 eMMC_pads_switch(U32 u32_FCIE_IF_Type)
{
	REG_FCIE_CLRBIT(reg_test_mode, reg_test_out_mode_mask|reg_test_in_mode_mask);
    REG_FCIE_CLRBIT(reg_chiptop_0x64, BIT_CAADCONFIG|BIT_PCMADCONFIG|BIT_PCM2CTRLCONFIG);
	REG_FCIE_SETBIT(reg_sd_use_bypass, BIT_SD_USE_BYPASS);
	REG_FCIE_CLRBIT(reg_nand_config, BIT_NAND_MODE|BIT_NAND_CS1_EN);
	REG_FCIE_CLRBIT(reg_sd_config, BIT_SD_CONFIG);
    REG_FCIE_CLRBIT(reg_sdio_config, BIT_SDIO_CONFIG);
    REG_FCIE_CLRBIT(reg_emmc_config, BIT_EMMC_CONFIG_MASK);
	REG_FCIE_SETBIT(reg_emmc_config, BIT_EMMC_CONFIG_MODE1);
	REG_FCIE_CLRBIT(reg_chiptop_0x50,BIT_ALL_PAD_IN);

	REG_FCIE_CLRBIT(reg_atop_patch, BIT_ATOP_PATCH_MASK);
	REG_FCIE_CLRBIT(reg_emmcpll_0x03, BIT_CLK_PH_MASK|BIT_DQ_PH_MASK|BIT_CMD_PH_MASK|BIT_SKEW4_MASK);

	REG_FCIE_CLRBIT(FCIE_BOOT_CONFIG, BIT8|BIT9|BIT10|BIT11|BIT12|BIT14|BIT15);
	REG_FCIE_CLRBIT(FCIE_SM_STS, BIT_DQS_DELAY_CELL_MASK);
	REG_FCIE_CLRBIT(FCIE_MACRO_REDNT, BIT_CRC_STATUS_4_HS200|BIT_LATE_DATA0_W_IP_CLK);
	REG_FCIE_CLRBIT(FCIE_HS200_PATCH, BIT_HS200_PATCH_MASK);

	switch (u32_FCIE_IF_Type) {

	case FCIE_eMMC_BYPASS:
		//eMMC_debug(1, 0, "eMMC pads: BYPASS\n");
		eMMC_debug(1, 0, "eMMC Warn: Why are you using bypass mode, Daniel does not allow this!!!\n");
		REG_FCIE_SETBIT(reg_fcie2macro_bypass, BIT_FCIE2MACRO_BYPASS);
		REG_FCIE_SETBIT(FCIE_BOOT_CONFIG, BIT8|BIT10|BIT11);
		g_eMMCDrv.u8_PadType = FCIE_eMMC_BYPASS;
		break;

	case FCIE_eMMC_SDR:
		//eMMC_debug(1, 0, "eMMC pads: SDR\n");
		REG_FCIE_SETBIT(reg_fcie2macro_bypass, BIT_FCIE2MACRO_BYPASS);
		REG_FCIE_SETBIT(FCIE_BOOT_CONFIG, BIT8);
		g_eMMCDrv.u8_PadType = FCIE_eMMC_SDR;
		break;

	case FCIE_eMMC_DDR:
		//eMMC_debug(1, 0, "eMMC pads: DDR\n");
		REG_FCIE_SETBIT(FCIE_BOOT_CONFIG, BIT8|BIT9|BIT14);
		REG_FCIE_SETBIT(FCIE_MACRO_REDNT, BIT_CRC_STATUS_4_HS200|BIT_LATE_DATA0_W_IP_CLK);
		REG_FCIE_SETBIT(reg_emmcpll_0x03, BIT2);
		REG_FCIE_CLRBIT(reg_fcie2macro_bypass, BIT_FCIE2MACRO_BYPASS); // move to last to prevent glitch
		g_eMMCDrv.u8_PadType = FCIE_eMMC_DDR;
		REG_FCIE_SETBIT(reg_atop_patch, BIT_HS_RSP_META_PATCH_HW|BIT_HS_D0_META_PATCH_HW|BIT_HS_DIN0_PATCH|BIT_HS_RSP_MASK_PATCH|BIT_DDR_RSP_PATCH);
		REG_FCIE_SETBIT(FCIE_HS200_PATCH, BIT_HS200_NORSP_PATCH|BIT_HS200_WCRC_PATCH|BIT_sbit_lose_patch);
		if(g_eMMCDrv.TimingTable_t.u8_SetCnt)
		    eMMC_FCIE_ApplyTimingSet(eMMC_TIMING_SET_MAX);
		break;

	case FCIE_eMMC_HS200:
		//eMMC_debug(1, 0, "eMMC pads: SDR200\n");
		REG_FCIE_SETBIT(FCIE_BOOT_CONFIG, BIT8|BIT14|BIT15);
		REG_FCIE_SETBIT(FCIE_MACRO_REDNT, BIT_CRC_STATUS_4_HS200|BIT_LATE_DATA0_W_IP_CLK);
		REG_FCIE_CLRBIT(reg_fcie2macro_bypass, BIT_FCIE2MACRO_BYPASS); // move to last to prevent glitch
		g_eMMCDrv.u8_PadType = FCIE_eMMC_HS200;
		REG_FCIE_SETBIT(reg_atop_patch, BIT_HS200_PATCH|BIT_HS_RSP_META_PATCH_HW|BIT_HS_D0_META_PATCH_HW|BIT_HS_DIN0_PATCH|BIT_HS_EMMC_DQS_PATCH|BIT_HS_RSP_MASK_PATCH);
		REG_FCIE_SETBIT(FCIE_HS200_PATCH, BIT_HS200_PATCH_MASK);
		if(g_eMMCDrv.TimingTable_t.u8_SetCnt)
		    eMMC_FCIE_ApplyTimingSet(eMMC_TIMING_SET_MAX);
		break;

	default:
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR, 1, "eMMC Err: unknown interface: %X\n", u32_FCIE_IF_Type);
		eMMC_die(" ");
		return eMMC_ST_ERR_INVALID_PARAM;
		break;
	}

	return eMMC_ST_SUCCESS;

}


U32 eMMC_pll_setting(U16 u16_ClkParam)
{
	static U16 u16_OldClkParam=0xFFFF;
	 U32 u32_value_reg_emmc_pll_pdiv;

	 if(u16_ClkParam == u16_OldClkParam)
		 return eMMC_ST_SUCCESS;
	 else
		 u16_OldClkParam = u16_ClkParam;

	 // 1. reset emmc pll
	REG_FCIE_SETBIT(reg_emmc_pll_reset,BIT0);
	REG_FCIE_CLRBIT(reg_emmc_pll_reset,BIT0);

	// 2. synth clock
	switch(u16_ClkParam) {
	    case eMMC_PLL_CLK_200M: // 200M
		    REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
			REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
			#if 0
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x22); // 20xMHz
			REG_FCIE_SETBIT(reg_ddfset_15_00,0x8F5C);
			#else
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x24); // 195MHz
			REG_FCIE_SETBIT(reg_ddfset_15_00,0x03D8);
			#endif
			u32_value_reg_emmc_pll_pdiv =1;// PostDIV: 2
			break;
		case eMMC_PLL_CLK_160M:
			REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
			REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x2B);
			REG_FCIE_SETBIT(reg_ddfset_15_00,0x3333);
			u32_value_reg_emmc_pll_pdiv =1;// PostDIV: 2
			break;
		case eMMC_PLL_CLK_140M:
			REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
			REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x31);
			REG_FCIE_SETBIT(reg_ddfset_15_00,0x5F15);
			u32_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
			break;
		case eMMC_PLL_CLK_120M:
			REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
			REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x39);
			REG_FCIE_SETBIT(reg_ddfset_15_00,0x9999);
			u32_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
			break;
		case eMMC_PLL_CLK_100M:
			REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
			REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x45);
			REG_FCIE_SETBIT(reg_ddfset_15_00,0x1EB8);
			u32_value_reg_emmc_pll_pdiv = 1;// PostDIV: 2
			break;
		case eMMC_PLL_CLK__86M:
			REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
			REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x28);
			REG_FCIE_SETBIT(reg_ddfset_15_00,0x2FA0);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__80M:
			REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
			REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x2B);
			REG_FCIE_SETBIT(reg_ddfset_15_00,0x3333);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__72M:
			REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
			REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x30);
			REG_FCIE_SETBIT(reg_ddfset_15_00,0x0000);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__62M:
			REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
			REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x37);
			REG_FCIE_SETBIT(reg_ddfset_15_00,0xBDEF);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__52M:
			REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
			REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x42);
			REG_FCIE_SETBIT(reg_ddfset_15_00,0x7627);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__48M:
			REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
			REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x48);
			REG_FCIE_SETBIT(reg_ddfset_15_00,0x0000);
			u32_value_reg_emmc_pll_pdiv = 2;// PostDIV: 4
			break;
		case eMMC_PLL_CLK__40M:
			REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
			REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x2B);
			REG_FCIE_SETBIT(reg_ddfset_15_00,0x3333);
			u32_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
			break;
		case eMMC_PLL_CLK__36M:
			REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
			REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x30);
			REG_FCIE_SETBIT(reg_ddfset_15_00,0x0000);
			u32_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
			break;
		case eMMC_PLL_CLK__32M:
			REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
			REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x36);
			REG_FCIE_SETBIT(reg_ddfset_15_00,0x0000);
			u32_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
			break;
		case eMMC_PLL_CLK__27M:
			REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
			REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x40);
			REG_FCIE_SETBIT(reg_ddfset_15_00,0x0000);
			u32_value_reg_emmc_pll_pdiv = 4;// PostDIV: 8
			break;
		case eMMC_PLL_CLK__20M:
			REG_FCIE_CLRBIT(reg_ddfset_23_16,0xffff);
			REG_FCIE_CLRBIT(reg_ddfset_15_00,0xffff);
			REG_FCIE_SETBIT(reg_ddfset_23_16,0x2B);
			REG_FCIE_SETBIT(reg_ddfset_15_00,0x3333);
			u32_value_reg_emmc_pll_pdiv = 7;// PostDIV: 16
			break;
		default:
			eMMC_debug(0,0,"eMMC Err: emmc PLL not configed %Xh\n", u16_ClkParam);
			eMMC_die(" ");
			return eMMC_ST_ERR_UNKNOWN_CLK;
			break;
	}

	// 3. VCO clock ( loop N = 4 )
	REG_FCIE_CLRBIT(reg_emmcpll_fbdiv,0xffff);
	REG_FCIE_SETBIT(reg_emmcpll_fbdiv,0x6);// PostDIV: 8

	// 4. 1X clock
	REG_FCIE_CLRBIT(reg_emmcpll_pdiv,BIT2|BIT1|BIT0);
	REG_FCIE_SETBIT(reg_emmcpll_pdiv,u32_value_reg_emmc_pll_pdiv);// PostDIV: 8

	if(u16_ClkParam==eMMC_PLL_CLK__20M) {
	    REG_FCIE_SETBIT(reg_emmc_pll_test, BIT10);
	}
	else {
		REG_FCIE_CLRBIT(reg_emmc_pll_test, BIT10);
	}

	eMMC_hw_timer_delay(HW_TIMER_DELAY_100us); // asked by Irwin

	return eMMC_ST_SUCCESS;
}


// Notice!!! you need to set pad before config clock
U32 eMMC_clock_setting(U16 u16_ClkParam)
{
	eMMC_PlatformResetPre();

	REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_SD_CLK_EN);

	switch(u16_ClkParam) {
		case eMMC_PLL_CLK__20M	: g_eMMCDrv.u32_ClkKHz =  20000; break;
		case eMMC_PLL_CLK__27M	: g_eMMCDrv.u32_ClkKHz =  27000; break;
		case eMMC_PLL_CLK__32M	: g_eMMCDrv.u32_ClkKHz =  32000; break;
		case eMMC_PLL_CLK__36M	: g_eMMCDrv.u32_ClkKHz =  36000; break;
		case eMMC_PLL_CLK__40M	: g_eMMCDrv.u32_ClkKHz =  40000; break;
		case eMMC_PLL_CLK__48M	: g_eMMCDrv.u32_ClkKHz =  48000; break;
		case eMMC_PLL_CLK__52M	: g_eMMCDrv.u32_ClkKHz =  52000; break;
		case eMMC_PLL_CLK__62M	: g_eMMCDrv.u32_ClkKHz =  62000; break;
		case eMMC_PLL_CLK__72M	: g_eMMCDrv.u32_ClkKHz =  72000; break;
		case eMMC_PLL_CLK__80M	: g_eMMCDrv.u32_ClkKHz =  80000; break;
		case eMMC_PLL_CLK__86M	: g_eMMCDrv.u32_ClkKHz =  86000; break;
		case eMMC_PLL_CLK_100M	: g_eMMCDrv.u32_ClkKHz = 100000; break;
		case eMMC_PLL_CLK_120M	: g_eMMCDrv.u32_ClkKHz = 120000; break;
		case eMMC_PLL_CLK_140M	: g_eMMCDrv.u32_ClkKHz = 140000; break;
		case eMMC_PLL_CLK_160M	: g_eMMCDrv.u32_ClkKHz = 160000; break;
		case eMMC_PLL_CLK_200M	: g_eMMCDrv.u32_ClkKHz = 200000; break;

		case BIT_FCIE_CLK_20M	: g_eMMCDrv.u32_ClkKHz =  20000; break;
		case BIT_FCIE_CLK_27M	: g_eMMCDrv.u32_ClkKHz =  27000; break;
		case BIT_FCIE_CLK_32M	: g_eMMCDrv.u32_ClkKHz =  32000; break;
		case BIT_FCIE_CLK_36M	: g_eMMCDrv.u32_ClkKHz =  36000; break;
		case BIT_FCIE_CLK_40M	: g_eMMCDrv.u32_ClkKHz =  40000; break;
		case BIT_FCIE_CLK_43_2M	: g_eMMCDrv.u32_ClkKHz =  43200; break;
		case BIT_FCIE_CLK_300K	: g_eMMCDrv.u32_ClkKHz =    300; break;
		case BIT_FCIE_CLK_48M	: g_eMMCDrv.u32_ClkKHz =  48000; break;

		default:
			eMMC_debug(1, 1, "eMMC Err: clkgen %Xh\n", eMMC_ST_ERR_INVALID_PARAM);
			eMMC_die(" ");
			return eMMC_ST_ERR_INVALID_PARAM; break;
	}

	if(u16_ClkParam & eMMC_PLL_FLAG) {
		//eMMC_debug(0,0,"eMMC PLL: %Xh\n", u16_ClkParam);
		REG_FCIE_CLRBIT(reg_ckg_fcie, BIT_FCIE_CLK_GATING|BIT_FCIE_CLK_INVERSE|BIT_CLKGEN_FCIE_MASK);
		REG_FCIE_SETBIT(reg_ckg_fcie, BIT_FCIE_CLK_SRC_SEL|BIT_FCIE_CLK_EMMC_PLL<<2);
		eMMC_pll_setting(u16_ClkParam);
	}
	else {
		//eMMC_debug(0,0,"eMMC CLKGEN: %Xh\n", u16_ClkParam);
	    REG_FCIE_CLRBIT(reg_ckg_fcie, BIT_FCIE_CLK_GATING|BIT_FCIE_CLK_INVERSE|BIT_CLKGEN_FCIE_MASK);
        REG_FCIE_SETBIT(reg_ckg_fcie, BIT_FCIE_CLK_SRC_SEL|u16_ClkParam<<2);
	}

	g_eMMCDrv.u16_ClkRegVal = (U16)u16_ClkParam;
	return eMMC_ST_SUCCESS;
}


U32 eMMC_clock_gating(void)
{
	//eMMC_debug(0,1,"\n");
	eMMC_PlatformResetPre();
	g_eMMCDrv.u32_ClkKHz = 0;
	REG_FCIE_SETBIT(reg_ckg_fcie, BIT_FCIE_CLK_GATING);// gate clock
	REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_SD_CLK_EN);
	eMMC_PlatformResetPost();
	return eMMC_ST_SUCCESS;
}

U8 gau8_FCIEClkSel[eMMC_FCIE_VALID_CLK_CNT] = {
	BIT_FCIE_CLK_48M,
	BIT_FCIE_CLK_36M,
	BIT_FCIE_CLK_20M
};

//---------------------------------------
#if defined(ENABLE_eMMC_INTERRUPT_MODE)&&ENABLE_eMMC_INTERRUPT_MODE

static DECLARE_WAIT_QUEUE_HEAD(fcie_wait);
static volatile U32 fcie_int = 0;

#define eMMC_IRQ_DEBUG    1

irqreturn_t eMMC_FCIE_IRQ(int irq, void *dummy)
{
	volatile u16 u16_Events;

	if((REG_FCIE(FCIE_REG16h) & BIT_EMMC_ACTIVE) != BIT_EMMC_ACTIVE)
	{
		return IRQ_NONE;
	}

	// one time enable one bit
	u16_Events = REG_FCIE(FCIE_MIE_EVENT) & REG_FCIE(FCIE_MIE_INT_EN);

	if(u16_Events & BIT_MIU_LAST_DONE)
    {
        REG_FCIE_CLRBIT(FCIE_MIE_INT_EN, BIT_MIU_LAST_DONE);

        fcie_int = 1;
        wake_up(&fcie_wait);
		return IRQ_HANDLED;
    }
    else if(u16_Events & BIT_CARD_DMA_END)
    {
        REG_FCIE_CLRBIT(FCIE_MIE_INT_EN, BIT_CARD_DMA_END);

        fcie_int = 1;
        wake_up(&fcie_wait);
		return IRQ_HANDLED;
    }
	else if(u16_Events & BIT_SD_CMD_END)
	{
		REG_FCIE_CLRBIT(FCIE_MIE_INT_EN, BIT_SD_CMD_END);

		fcie_int = 1;
		wake_up(&fcie_wait);
		return IRQ_HANDLED;
	}
	#if defined(ENABLE_FCIE_HW_BUSY_CHECK)&&ENABLE_FCIE_HW_BUSY_CHECK
	else if(u16_Events & BIT_SD_BUSY_END)
	{
        REG_FCIE_CLRBIT(FCIE_MIE_INT_EN, BIT_SD_BUSY_END);
		REG_FCIE_CLRBIT(FCIE_SD_CTRL, BIT_SD_BUSY_DET_ON);
		fcie_int = 1;
		wake_up(&fcie_wait);
		return IRQ_HANDLED;
	}
    #endif
#if eMMC_IRQ_DEBUG
	if(0==fcie_int)
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Warn: Int St:%Xh, En:%Xh, Evt:%Xh \n",
		    REG_FCIE(FCIE_MIE_EVENT), REG_FCIE(FCIE_MIE_INT_EN), u16_Events);
#endif

	return IRQ_NONE;

}


U32 eMMC_WaitCompleteIntr(U32 u32_RegAddr, U16 u16_WaitEvent, U32 u32_MicroSec)
{
	U32 u32_i=0;

    #if eMMC_IRQ_DEBUG
	U32 u32_isr_tmp[2];
	unsigned long long u64_jiffies_tmp, u64_jiffies_now;
	struct timeval time_st;
	time_t sec_tmp;
	suseconds_t us_tmp;

	u32_isr_tmp[0] = fcie_int;
	do_gettimeofday(&time_st);
	sec_tmp = time_st.tv_sec;
	us_tmp = time_st.tv_usec;
	u64_jiffies_tmp = jiffies_64;
    #endif

	//----------------------------------------
	if(wait_event_timeout(fcie_wait, (fcie_int == 1), usecs_to_jiffies(u32_MicroSec)) == 0)
    {
		#if eMMC_IRQ_DEBUG
		u32_isr_tmp[1] = fcie_int;
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
			"eMMC Warn: int timeout, WaitEvt:%Xh, NowEvt:%Xh, IntEn:%Xh, ISR:%u->%u->%u \n",
			u16_WaitEvent, REG_FCIE(FCIE_MIE_EVENT), REG_FCIE(FCIE_MIE_INT_EN),
			u32_isr_tmp[0], u32_isr_tmp[1], fcie_int);

		do_gettimeofday(&time_st);
		u64_jiffies_now = jiffies_64;
	    eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
		    " PassTime: %lu s, %lu us, %llu jiffies.  WaitTime: %u us, %lu jiffies, HZ:%u.\n",
		    time_st.tv_sec-sec_tmp, time_st.tv_usec-us_tmp, u64_jiffies_now-u64_jiffies_tmp,
		    u32_MicroSec, usecs_to_jiffies(u32_MicroSec), HZ);
	    #else
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,
			"eMMC Warn: int timeout, WaitEvt:%Xh, NowEvt:%Xh, IntEn:%Xh \n",
			u16_WaitEvent, REG_FCIE(FCIE_MIE_EVENT), REG_FCIE(FCIE_MIE_INT_EN));
	    #endif

		// switch to polling
        for(u32_i=0; u32_i<u32_MicroSec; u32_i++)
        {
            if((REG_FCIE(u32_RegAddr) & u16_WaitEvent) == u16_WaitEvent )
                break;

            eMMC_hw_timer_delay(HW_TIMER_DELAY_1us);
        }

		if(u32_i == u32_MicroSec)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: events lose, WaitEvent: %Xh \n", u16_WaitEvent);
			eMMC_DumpDriverStatus();  eMMC_DumpPadClk();
		    eMMC_FCIE_DumpRegisters();eMMC_FCIE_DumpDebugBus();
			return eMMC_ST_ERR_INT_TO;
		}
		else
		{	REG_FCIE_CLRBIT(FCIE_MIE_INT_EN, u16_WaitEvent);
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Warn: but polling ok: %Xh \n", REG_FCIE(u32_RegAddr));
		}

    }

	//----------------------------------------
	if(u16_WaitEvent & BIT_MIU_LAST_DONE)
	{
		for(u32_i=0; u32_i<TIME_WAIT_1_BLK_END; u32_i++)
		{
			if(REG_FCIE(u32_RegAddr) & BIT_CARD_DMA_END)
				break; // should be very fase
			eMMC_hw_timer_delay(HW_TIMER_DELAY_1us);
		}

		if(TIME_WAIT_1_BLK_END == u32_i)
		{
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: no CARD_DMA_END\n");
			eMMC_DumpDriverStatus();  eMMC_DumpPadClk();
		    eMMC_FCIE_DumpRegisters();eMMC_FCIE_DumpDebugBus();
			return eMMC_ST_ERR_INT_TO;
		}
	}

	//----------------------------------------
	fcie_int = 0;
	return eMMC_ST_SUCCESS;

}
#endif



int mstar_mci_Housekeep(void *pData)
{
	U32 u32_cnt=0;

	#if !(defined(eMMC_HOUSEKEEP_THREAD) && eMMC_HOUSEKEEP_THREAD)
	u32_cnt = 0;
    return 0;
	#endif

	#if eMMC_PROFILE_WR
	g_eMMCDrv.u32_CNT_MinRBlk = 0xFFFFFFFF;
	g_eMMCDrv.u32_CNT_MinWBlk = 0xFFFFFFFF;
	#endif

	while(1)
	{
		u32_cnt++;
		#if defined(eMMC_PROFILE_WR) && eMMC_PROFILE_WR
		msleep(1000);
		eMMC_dump_WR_Count();
		#endif


		#if defined(eMMC_SCAN_HS200) && eMMC_SCAN_HS200
        msleep(1000*10);
		if(u32_cnt++ < 5)
			continue;
		eMMC_LockFCIE((U8*)__FUNCTION__);
		eMMC_debug(0,0,"\n\n==================================================\n");
        eMMC_SET_BKG_SCAN_HS200();
		//eMMC_DumpTimingTable();
		//eMMC_FCIE_BuildHS200TimingTable();
		eMMC_FCIE_HS200_IrwinTest();
		eMMC_CLR_BKG_SCAN_HS200();
		eMMC_UnlockFCIE((U8*)__FUNCTION__);
		#endif

		if(kthread_should_stop())
			break;
	}

	return 0;
}


int mstar_mci_exit_checkdone_Ex(void)
{
    U16 u16Reg = 0;

    u16Reg = REG_FCIE(FCIE_SD_STATUS);

    if(u16Reg & BIT_SD_D0)
        return 1;
    else
        return 0;
}

bool mstar_mci_exit_checkdone_ForCI(void)
{
	return true;
}

EXPORT_SYMBOL(mstar_mci_exit_checkdone_ForCI);

#if (defined(IF_FCIE_SHARE_IP) && IF_FCIE_SHARE_IP)
bool mstar_mci_exit_checkdone_ForSD(void)
{
	return true;
}
EXPORT_SYMBOL(mstar_mci_exit_checkdone_ForSD);
#endif

#if 0
U32 mstar_SD_CardChange(void)
{
	return 0;
}
#endif

//---------------------------------------
#if (defined(IF_FCIE_SHARE_PINS) && IF_FCIE_SHARE_PINS) || \
    (defined(IF_FCIE_SHARE_IP) && IF_FCIE_SHARE_IP)
extern struct semaphore	PfModeSem;
#endif

extern bool ms_sdmmc_wait_d0_for_emmc(void);

void eMMC_LockFCIE(U8 *pu8_str)
{
	//eMMC_debug(0,1,"%s 1\n", pu8_str);
	#if (defined(IF_FCIE_SHARE_PINS) && IF_FCIE_SHARE_PINS) || \
        (defined(IF_FCIE_SHARE_IP) && IF_FCIE_SHARE_IP)
	down(&PfModeSem);
    #endif
	//eMMC_debug(0,1,"%s 2\n", pu8_str);

	#if IF_FCIE_SHARE_IP && defined(CONFIG_MSTAR_SDMMC)
	if(false == ms_sdmmc_wait_d0_for_emmc())
	{
		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: SD keep D0 low \n");
		eMMC_FCIE_ErrHandler_Stop();
	}

	eMMC_clock_setting(g_eMMCDrv.u16_ClkRegVal);
	eMMC_pads_switch(g_eMMCDrv.u8_PadType);
	#endif

    REG_FCIE_SETBIT(FCIE_SD_MODE, BIT_SD_CLK_EN); // output clock
	REG_FCIE_SETBIT(FCIE_REG16h, BIT_EMMC_ACTIVE);

}

void eMMC_UnlockFCIE(U8 *pu8_str)
{
	//eMMC_debug(0,1,"%s 1\n", pu8_str);
	REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_SD_CLK_EN); // not output clock
	REG_FCIE_CLRBIT(FCIE_REG16h, BIT_EMMC_ACTIVE);

    #if (defined(IF_FCIE_SHARE_PINS) && IF_FCIE_SHARE_PINS) || \
        (defined(IF_FCIE_SHARE_IP) && IF_FCIE_SHARE_IP)
	up(&PfModeSem);
    #endif
	//eMMC_debug(0,1,"%s 2\n", pu8_str);
}

//---------------------------------------

U32 eMMC_PlatformResetPre(void)
{
	/**((volatile unsigned short *)(0x25007DCC))|=0x02;	// emi mask
	*((volatile unsigned short *)(0x25007C18))|=0x02;	// imi0 mask
	*((volatile unsigned short *)(0x25007C58))|=0x02;	// imi1 mask
	*/
	return eMMC_ST_SUCCESS;
}

U32 eMMC_PlatformResetPost(void)
{
	/**((volatile unsigned short *)(0x25007DCC))&=(~0x02);	// emi unmask
	*((volatile unsigned short *)(0x25007C18))&=(~0x02);	// imi0 unmask
	*((volatile unsigned short *)(0x25007C58))&=(~0x02);	// imi1 unmask
	*/
    #if defined(ENABLE_EMMC_POWER_SAVING_MODE) && ENABLE_EMMC_POWER_SAVING_MODE
	eMMC_Prepare_Power_Saving_Mode_Queue();
    #endif
	return eMMC_ST_SUCCESS;
}

U32 eMMC_PlatformInit(void)
{
    #if 1	// for eMMC 4.5 HS200 need 1.8V, unify all eMMC IO power to 1.8V
	// works both for eMMC 4.4 & 4.5
	// printf("1.8V IO power for eMMC\n");
	// Irwin Tyan: set this bit to boost IO performance at low power supply.

	if((REG_FCIE(reg_emmc_test) & BIT0) != 1) {
		//eMMC_debug(0, 0, "eMMC: set 1.8V \n");
		REG_FCIE_SETBIT(reg_emmc_test, BIT0); // 1.8V must set this bit
		REG_FCIE_SETBIT(reg_emmc_test, BIT2); // atop patch
		REG_FCIE_CLRBIT(reg_nand_pad_driving, 0xffff);// 1.8V must set this bit
		REG_FCIE_SETBIT(reg_nand_pad_driving, 0xf);// 1.8V must set this bit
	}
    #else
	printf("3.3V IO power for eMMC\n");
	pRegEmmcPll->reg_emmc_test = 0; // 3.3V must clear this bit
    #endif

	#if defined(CONFIG_ENABLE_EMMC_ACP) && CONFIG_ENABLE_EMMC_ACP
	//set once only , ACP can't be disabled
	eMMC_debug(0, 0, "eMMC: enable ACP\n");

	REG_FCIE_CLRBIT(reg_1239_Rx16, BIT8|BIT9|BIT10);
	REG_FCIE_CLRBIT(reg_1239_Rx26, BIT8|BIT9|BIT10);

	//mask acp request
	REG_FCIE_SETBIT(reg_100E_Rx03, (BIT8-1));
	//start acp
	REG_FCIE_SETBIT(reg_100E_Rx04, BIT6);
	//unmask acp request
	REG_FCIE_CLRBIT(reg_100E_Rx03, (BIT8-1));
	#endif

	eMMC_pads_switch(FCIE_DEFAULT_PAD);
	eMMC_clock_setting(FCIE_SLOWEST_CLK);

	return eMMC_ST_SUCCESS;
}


U32 eMMC_PlatformDeinit(void)
{
	return eMMC_ST_SUCCESS;
}


// --------------------------------------------
static U32 sgu32_MemGuard0 = 0xA55A;
eMMC_ALIGN0 eMMC_DRIVER g_eMMCDrv eMMC_ALIGN1;
static U32 sgu32_MemGuard1 = 0x1289;

eMMC_ALIGN0 U8 gau8_eMMC_SectorBuf[eMMC_SECTOR_BUF_16KB] eMMC_ALIGN1; // 512 bytes
eMMC_ALIGN0 U8 gau8_eMMC_PartInfoBuf[eMMC_SECTOR_512BYTE] eMMC_ALIGN1; // 512 bytes

U32 eMMC_CheckIfMemCorrupt(void)
{
	if(0xA55A != sgu32_MemGuard0 || 0x1289 != sgu32_MemGuard1)
		return eMMC_ST_ERR_MEM_CORRUPT;

	return eMMC_ST_SUCCESS;
}
// <-- [FIXME]


// ======================================================
void eMMC_dump_eMMCPLL(void)
{
	U16 u16_i, u16_reg;
	eMMC_debug(eMMC_DEBUG_LEVEL, 0, "\n emmcpll RIU bank:");
	for(u16_i=0 ; u16_i<0x60; u16_i++)
	{
		if(0 == u16_i%8)
			eMMC_debug(eMMC_DEBUG_LEVEL,0,"\n%02Xh:| ", u16_i);

		REG_FCIE_R(GET_REG_ADDR(EMMC_PLL_BASE, u16_i), u16_reg);
		eMMC_debug(eMMC_DEBUG_LEVEL, 0, "%04Xh ", u16_reg);
	}
}

#if defined(ENABLE_eMMC_HS200) && ENABLE_eMMC_HS200
// trigger level
#define Trig_lvl_0        ((0<<6)|(0<<3)|0)
#define Trig_lvl_1        ((0<<6)|(0<<3)|7)
#define Trig_lvl_2        ((0<<6)|(7<<3)|0)
#define Trig_lvl_3        ((0<<6)|(7<<3)|7)
#define Trig_lvl_4        ((7<<6)|(0<<3)|0)
#define Trig_lvl_5        ((7<<6)|(0<<3)|7)
#define Trig_lvl_6        ((7<<6)|(7<<3)|0)
#define Trig_lvl_7        ((7<<6)|(7<<3)|7)
#define Trig_lvl_MASK     Trig_lvl_7
#define reg_emmcpll_0x20  GET_REG_ADDR(EMMC_PLL_BASE, 0x20)

#define Trig_lvl_CNT      8
static  U16 sgau16_TrigLvl[Trig_lvl_CNT]=
  {Trig_lvl_0, Trig_lvl_1, Trig_lvl_2, Trig_lvl_3,
   Trig_lvl_4, Trig_lvl_5, Trig_lvl_6, Trig_lvl_7};

// skew1
#define reg_emmcpll_0x1C  GET_REG_ADDR(EMMC_PLL_BASE, 0x1C)
#define emmcpll_dqs_patch BIT6 // switch skew1

#define IRWIN_TEST_FLAG_CMD21    1
#define IRWIN_TEST_FLAG_WRC      2

void eMMC_dump_Phase(U8 au8_Phase[PLL_SKEW4_CNT*2][PLL_SKEW4_CNT*2], U8 u8_TestFlag, U16 u16_Reg03h)
{
	U8 u8_i, u8_j;

	eMMC_debug(eMMC_DEBUG_LEVEL,0,"[Skew1\\Skew4] ");
	for(u8_i=PLL_SKEW4_CNT*2; u8_i > 0; u8_i--) // print Skew4 indices
		eMMC_debug(eMMC_DEBUG_LEVEL,0,"%02u ", u8_i-1);
	eMMC_debug(eMMC_DEBUG_LEVEL,0,"\n");

	for(u8_i=0; u8_i<PLL_SKEW4_CNT*2; u8_i++)
	{
		//--------------------------------
		// Read Phase has no need to change Skew1
		if(IRWIN_TEST_FLAG_CMD21 == u8_TestFlag)
		{
			if(u8_i != (u16_Reg03h&BIT_CLK_PH_MASK))
				continue;
		}
		//--------------------------------

		eMMC_debug(eMMC_DEBUG_LEVEL,0,"  [%02u]        ", u8_i);
		for(u8_j=PLL_SKEW4_CNT*2; u8_j > 0; u8_j--)
			eMMC_debug(eMMC_DEBUG_LEVEL,0,"%2u ", au8_Phase[u8_i][u8_j-1]);
		eMMC_debug(eMMC_DEBUG_LEVEL,0,"\n");
	}
}


U32 eMMC_FCIE_HS200_IrwinTest(void)
{
	U8  au8_Phase[PLL_SKEW4_CNT*2][PLL_SKEW4_CNT*2], u8_Skew4Idx, u8_Skew1Idx, u8_TrigLvlIdx;
    U16 u16_reg;
	static U16 u16_Reg03h=0xFFFF, u16_Reg1Ch=0xFFFF;
	U32 u32_err;
	U8  u8_TestFlag=IRWIN_TEST_FLAG_CMD21;

	if(0xFFFF==u16_Reg03h && 0xFFFF==u16_Reg1Ch) // for original Skew1
	{
		REG_FCIE_R(reg_emmcpll_0x03, u16_Reg03h);
		REG_FCIE_R(reg_emmcpll_0x1C, u16_Reg1Ch);
		eMMC_debug(0,0,"Reg03h: %Xh, Reg1Ch: %Xh \n", u16_Reg03h, u16_Reg1Ch);
	}
	// ----------------------------------
	// dump emmcpll
	eMMC_dump_eMMCPLL();

	// ----------------------------------
	LABEL_TEST_START:
	eMMC_debug(eMMC_DEBUG_LEVEL,0,"\n\n==================== \n");
	switch(u8_TestFlag)
	{
		case IRWIN_TEST_FLAG_CMD21:
			eMMC_debug(eMMC_DEBUG_LEVEL,0,"CMD21 Read Phase \n");
			break;
		case IRWIN_TEST_FLAG_WRC:
			eMMC_debug(eMMC_DEBUG_LEVEL,0,"Write Phase \n");
			break;
	}
	eMMC_debug(eMMC_DEBUG_LEVEL,0,"==================== \n");

	for(u8_TrigLvlIdx=0; u8_TrigLvlIdx<Trig_lvl_CNT; u8_TrigLvlIdx++)
	{
		REG_FCIE_CLRBIT(reg_emmcpll_0x20, Trig_lvl_MASK);
		REG_FCIE_SETBIT(reg_emmcpll_0x20, sgau16_TrigLvl[u8_TrigLvlIdx]);
		REG_FCIE_R(reg_emmcpll_0x20, u16_reg);
		eMMC_debug(eMMC_DEBUG_LEVEL,0,"\nTrigger Level: %u, Reg.20h: %Xh \n", u8_TrigLvlIdx, u16_reg);

		for(u8_Skew1Idx=0; u8_Skew1Idx<PLL_SKEW4_CNT*2; u8_Skew1Idx++)
		{
			//--------------------------------
		    // Read Phase has no need to change Skew1
		    if(IRWIN_TEST_FLAG_CMD21 == u8_TestFlag)
		    {
			    if(u8_Skew1Idx != (u16_Reg03h&BIT_CLK_PH_MASK))
				    continue;
		    }

			for(u8_Skew4Idx=0; u8_Skew4Idx<PLL_SKEW4_CNT*2; u8_Skew4Idx++)
			{
				if(IRWIN_TEST_FLAG_CMD21 != u8_TestFlag)
				{if(u8_Skew1Idx<PLL_SKEW4_CNT)
			    {
			        REG_FCIE_CLRBIT(reg_emmcpll_0x1C, emmcpll_dqs_patch);
				    REG_FCIE_CLRBIT(reg_emmcpll_0x03, BIT_CLK_PH_MASK);
		            REG_FCIE_SETBIT(reg_emmcpll_0x03, u8_Skew1Idx);
			    }
			     else
			    {   REG_FCIE_SETBIT(reg_emmcpll_0x1C, emmcpll_dqs_patch);
			        REG_FCIE_CLRBIT(reg_emmcpll_0x03, BIT_CLK_PH_MASK);
				    REG_FCIE_SETBIT(reg_emmcpll_0x03, u8_Skew1Idx-PLL_SKEW4_CNT);
			    }}

				if(u8_Skew4Idx<PLL_SKEW4_CNT)
			    {
			        REG_FCIE_CLRBIT(FCIE_SM_STS, BIT11);
				    REG_FCIE_CLRBIT(reg_emmcpll_0x03, BIT_SKEW4_MASK);
		            REG_FCIE_SETBIT(reg_emmcpll_0x03, u8_Skew4Idx<<12);
			    }
			    else
			    {   REG_FCIE_SETBIT(FCIE_SM_STS, BIT11);
			        REG_FCIE_CLRBIT(reg_emmcpll_0x03, BIT_SKEW4_MASK);
				    REG_FCIE_SETBIT(reg_emmcpll_0x03, (u8_Skew4Idx-PLL_SKEW4_CNT)<<12);
			    }
				g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_TUNING_TTABLE;
				//--------------------------------
				switch(u8_TestFlag)
	            {
		            case IRWIN_TEST_FLAG_CMD21:
			            u32_err = eMMC_CMD21();
						break;
		            case IRWIN_TEST_FLAG_WRC:
			            u32_err = eMMCTest_BlkWRC_ProbeTiming(eMMC_TEST_BLK_0);
			            break;
	            }
				//--------------------------------
				g_eMMCDrv.u32_DrvFlag &= ~DRV_FLAG_TUNING_TTABLE;
				if(eMMC_ST_SUCCESS == u32_err)
				{
					au8_Phase[u8_Skew1Idx][u8_Skew4Idx]=1;
					//REG_FCIE_R(reg_emmcpll_0x03, u16_reg);
		            //eMMC_debug(eMMC_DEBUG_LEVEL,0,"Reg.03h: %04Xh, ok \n", u16_reg);
				}
				else
				{   //REG_FCIE_R(reg_emmcpll_0x03, u16_reg);
		            //eMMC_debug(eMMC_DEBUG_LEVEL,0,"Reg.03h: %04Xh, fail \n", u16_reg);
					eMMC_FCIE_ResetToHS200((U8)g_eMMCDrv.u16_ClkRegVal);
					au8_Phase[u8_Skew1Idx][u8_Skew4Idx]=0;
				}
		    }
		}

		eMMC_dump_Phase(au8_Phase, u8_TestFlag, u16_Reg03h);
	}

	// ----------------------------------
	switch(u8_TestFlag)
	{
		case IRWIN_TEST_FLAG_CMD21:
			u8_TestFlag = IRWIN_TEST_FLAG_WRC;
			goto LABEL_TEST_START;

		case IRWIN_TEST_FLAG_WRC:
			break;
	}

	// restore
	REG_FCIE_W(reg_emmcpll_0x03, u16_Reg03h);
	REG_FCIE_W(reg_emmcpll_0x1C, u16_Reg1Ch);
	eMMC_FCIE_ResetToHS200((U8)g_eMMCDrv.u16_ClkRegVal);

	return eMMC_ST_SUCCESS;
}
#endif

#endif
