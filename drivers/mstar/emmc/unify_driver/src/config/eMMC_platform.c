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
#if defined(UNIFIED_eMMC_DRIVER) && UNIFIED_eMMC_DRIVER

void eMMC_Prepare_Power_Saving_Mode_Queue(void)
{
    REG_FCIE_SETBIT(reg_pwrgd_int_glirm, BIT_PWRGD_INT_GLIRM_EN);

    REG_FCIE_CLRBIT(reg_pwrgd_int_glirm, BIT_PWEGD_INT_GLIRM_MASK);
    REG_FCIE_SETBIT(reg_pwrgd_int_glirm, (0x2<<10));
    REG_FCIE_CLRBIT(FCIE_PWR_SAVE_CTL, BIT_BAT_SD_POWER_SAVE_MASK);

    /* (1) Clear HW Enable */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x00), 0x0000);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x01),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x07);

    /* (2) Clear All Interrupt */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x02), 0xffff);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x03),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x00);

    /* (3) Clear SD MODE Enable */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x04), 0x0000);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x05),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0B);

    /* (4) Clear SD CTL Enable */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x06), 0x0000);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x07),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0C);

    /* (5) Reset Start */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x08), 0x0000);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x09),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x3F);

    /* (6) Reset End */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x0A), 0x0001);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x0B),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x3F);

    /* (7) Set "FCIE_DDR_MODE" */
    #if defined(ENABLE_eMMC_HS400) && ENABLE_eMMC_HS400
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x0C), 0x1100);
    #elif defined(ENABLE_eMMC_HS200) && ENABLE_eMMC_HS200
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x0C), 0x1000);
    #elif defined(ENABLE_eMMC_ATOP) && ENABLE_eMMC_ATOP
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x0C), 0x0180);
    #endif
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x0D),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0F);

    /* (8) Set "SD_MOD" */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x0E), 0x0021);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x0F),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0B);

    /* (9) Enable "reg_sd_en" */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x10), 0x0001);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x11),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x07);

    /* (10) Command Content, IDLE */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x12), 0x0040);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x13),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x20);

    /* (11) Command Content, STOP */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x14), 0x0000);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x15),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x21);

    /* (12) Command Content, STOP */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x16), 0x0000);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x17),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x22);

    /* (13) Command & Response Size */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x18), 0x0500);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x19),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0A);

    /* (14) Enable Interrupt, SD_CMD_END */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x1A), 0x0002);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x1B),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x01);

    /* (15) Command Enable + job Start */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x1C), 0x0044);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x1D),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WREG | PWR_CMD_BK0 | 0x0C);

    /* (16) Wait Interrupt */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x1E), 0x0000);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x1F),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_WINT);

    /* (17) STOP */
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x20), 0x0000);
    REG_FCIE_W(GET_REG_ADDR(FCIE_POWEER_SAVE_MODE_BASE, 0x21),
               PWR_BAT_CLASS | PWR_RST_CLASS | PWR_CMD_STOP);

    REG_FCIE_CLRBIT(FCIE_PWR_SAVE_CTL, BIT_SD_POWER_SAVE_RST);
    REG_FCIE_SETBIT(FCIE_PWR_SAVE_CTL, BIT_SD_POWER_SAVE_RST);

    REG_FCIE_SETBIT(FCIE_PWR_SAVE_CTL, BIT_POWER_SAVE_MODE|BIT_POWER_SAVE_MODE_INT_EN);
}


//=============================================================
#if (defined(eMMC_DRV_INFINITY2_LINUX)&&eMMC_DRV_INFINITY2_LINUX)
//=============================================================
U8 gau8_FCIEClkSel[eMMC_FCIE_VALID_CLK_CNT]={
	BIT_FCIE_CLK_4X_192M,
	BIT_FCIE_CLK_4X_160M,
	BIT_FCIE_CLK_4X_144M,
	BIT_FCIE_CLK_4X_108M,
	BIT_FCIE_CLK_1X_27M,
};
U32 gu32_eMMCDrvExtFlag = 0;

#if 0 //defined(CONFIG_OF)
struct clk_data{
    int num_parents;
    struct clk **clk_fcie;
//  struct clk *clk_ecc;
};
extern struct clk_data *clkdata;
#endif

void mdelay_MacroToFun(u32 time)
{
    mdelay(time);
}

U32 eMMC_hw_timer_delay(U32 u32usTick)
{
	volatile U32 u32_i=u32usTick;

    while(u32_i>1000)
    {
		udelay(1000);
		u32_i-=1000;
    }

    udelay(u32usTick);
    return u32usTick + 1;
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
U32 eMMC_hw_timer_start(void)
{
	return 0;
}

U32 eMMC_hw_timer_tick(void)
{
	return 0;
}

void eMMC_DumpPadClk(void)
{
    //---------------------------------------------------------------------
    eMMC_debug(0, 0, "[pad setting]:\r\n");
    switch(g_eMMCDrv.u8_PadType)
    {
		case FCIE_eMMC_BYPASS:          eMMC_debug(0, 0, "Bypass\r\n");  break;
        default:
            eMMC_debug(0, 0, "eMMC Err: Pad unknown, %d\r\n", g_eMMCDrv.u8_PadType); eMMC_die("\r\n");
            break;
    }
}

U8 gu8_NANDeMMC_need_preset_flag = 1;

U32 eMMC_pads_switch(U32 u32_FCIE_IF_Type)
{
	g_eMMCDrv.u8_PadType = u32_FCIE_IF_Type;

	// chiptop
	REG_FCIE_CLRBIT(reg_all_pad_in, BIT_ALL_PAD_IN);
	REG_FCIE_CLRBIT(reg_sd_config, BIT_SD_MODE_MASK);
	REG_FCIE_CLRBIT(reg_nand_config,BIT_NAND_MODE_MASK);
    REG_FCIE_SETBIT(reg_emmc_config, BIT11);
    REG_FCIE_W(reg_padtop1_0x22, 0xB6D9);
    REG_FCIE_W(reg_padtop1_0x23, 0x826D);
    REG_FCIE_W(reg_padtop1_0x24, 0x036D);
    REG_FCIE_SETBIT(reg_padtop0_0x16, BIT4);

	// fcie
	REG_FCIE_CLRBIT(FCIE_DDR_MODE, BIT_MACRO_MODE_MASK);
	REG_FCIE_SETBIT(FCIE_DDR_MODE, BIT14|BIT13|BIT10);

	g_eMMCDrv.u32_DrvFlag |= DRV_FLAG_SPEED_HIGH;

	return eMMC_ST_SUCCESS;

}



U32 eMMC_clock_setting(U16 u16_ClkParam)
{
	eMMC_PlatformResetPre();

    REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_CLK_EN);

    switch(u16_ClkParam) {
        case BIT_FCIE_CLK_1X_XTAL:  g_eMMCDrv.u32_ClkKHz =   3000;  break;
        case BIT_FCIE_CLK_1X_12M:   g_eMMCDrv.u32_ClkKHz =  12000;  break;
        case BIT_FCIE_CLK_1X_15_5M: g_eMMCDrv.u32_ClkKHz =  15500;  break;
        case BIT_FCIE_CLK_1X_18M:   g_eMMCDrv.u32_ClkKHz =  18000;  break;
        case BIT_FCIE_CLK_1X_21_5M: g_eMMCDrv.u32_ClkKHz =  21500;  break;
        case BIT_FCIE_CLK_1X_27M:   g_eMMCDrv.u32_ClkKHz =  27000;  break;
        case BIT_FCIE_CLK_1X_36M:   g_eMMCDrv.u32_ClkKHz =  36000;  break;
        case BIT_FCIE_CLK_1X_40M:   g_eMMCDrv.u32_ClkKHz =  40000;  break;
        case BIT_FCIE_CLK_1X_48M:   g_eMMCDrv.u32_ClkKHz =  48000;  break;
        case BIT_FCIE_CLK_1X_54M:   g_eMMCDrv.u32_ClkKHz =  54000;  break;
        case BIT_FCIE_CLK_1X_60M:   g_eMMCDrv.u32_ClkKHz =  60000;  break;
        case BIT_FCIE_CLK_1X_88_5M: g_eMMCDrv.u32_ClkKHz =  88500;  break;
        case BIT_FCIE_CLK_1X_75K:   g_eMMCDrv.u32_ClkKHz =     75;  break;
        case BIT_FCIE_CLK_1X_80M:   g_eMMCDrv.u32_ClkKHz =  80000;  break;

        case BIT_FCIE_CLK_4X_XTAL:  g_eMMCDrv.u32_ClkKHz =  12000;  break;
        case BIT_FCIE_CLK_4X_48M:   g_eMMCDrv.u32_ClkKHz =  48000;  break;
        case BIT_FCIE_CLK_4X_62M:   g_eMMCDrv.u32_ClkKHz =  62000;  break;
        case BIT_FCIE_CLK_4X_72M:   g_eMMCDrv.u32_ClkKHz =  72000;  break;
        case BIT_FCIE_CLK_4X_86M:   g_eMMCDrv.u32_ClkKHz =  86000;  break;
        case BIT_FCIE_CLK_4X_108M:  g_eMMCDrv.u32_ClkKHz = 108000;  break;
        case BIT_FCIE_CLK_4X_144M:  g_eMMCDrv.u32_ClkKHz = 144000;  break;
        case BIT_FCIE_CLK_4X_160M:  g_eMMCDrv.u32_ClkKHz = 160000;  break;
        case BIT_FCIE_CLK_4X_192M:  g_eMMCDrv.u32_ClkKHz = 192000;  break;
        case BIT_FCIE_CLK_4X_216M:  g_eMMCDrv.u32_ClkKHz = 216000;  break;
        case BIT_FCIE_CLK_4X_240M:  g_eMMCDrv.u32_ClkKHz = 240000;  break;
        case BIT_FCIE_CLK_4X_354M:  g_eMMCDrv.u32_ClkKHz = 354000;  break;
        case BIT_FCIE_CLK_4X_300K:  g_eMMCDrv.u32_ClkKHz =    300;  break;
        case BIT_FCIE_CLK_4X_320M:  g_eMMCDrv.u32_ClkKHz = 320000;  break;

        default:
            eMMC_debug(1, 1, "eMMC Err: clkgen %Xh\n", eMMC_ST_ERR_INVALID_PARAM);
            eMMC_die(" ");
            return eMMC_ST_ERR_INVALID_PARAM; break;
    }

    REG_FCIE_CLRBIT(reg_ckg_fcie, BIT_FCIE_CLK_SRC_SEL);
    REG_FCIE(reg_ckg_fcie);
    udelay(7);
    REG_FCIE_CLRBIT(reg_ckg_fcie, BIT_FCIE_CLK_GATING|BIT_FCIE_CLK_INVERSE|BIT_CLKGEN_FCIE_MASK|BIT_CKG_NFIE_DVI4_EN);

    if(u16_ClkParam & eMMC_4X_FLAG) {
        //eMMC_debug(0,0,"eMMC PLL: %Xh\n", u16_ClkParam);
        if( g_eMMCDrv.u32_DrvFlag & DRV_FLAG_DDR_MODE ) // DDR52
        {
            REG_FCIE_CLRBIT(reg_ckg_fcie, BIT_CKG_NFIE_DVI4_EN);
            g_eMMCDrv.u32_ClkKHz >>= 2;
        }
        else if( g_eMMCDrv.u32_DrvFlag & DRV_FLAG_SPEED_HIGH ) // HS
        {
            REG_FCIE_CLRBIT(reg_ckg_fcie, BIT_CKG_NFIE_DVI4_EN);
            g_eMMCDrv.u32_ClkKHz >>= 2;
        }
        else
            REG_FCIE_CLRBIT(reg_ckg_fcie, BIT_CKG_NFIE_DVI4_EN);
    }
    else
        REG_FCIE_SETBIT(reg_ckg_fcie, BIT_CKG_NFIE_DVI4_EN);

    REG_FCIE_SETBIT(reg_ckg_fcie, (u16_ClkParam & 0xF)<<10);
    REG_FCIE_SETBIT(reg_ckg_fcie, BIT_FCIE_CLK_SRC_SEL);

    g_eMMCDrv.u16_ClkRegVal = (U16)u16_ClkParam;

	eMMC_PlatformResetPost();

	return eMMC_ST_SUCCESS;
}


U32 eMMC_clock_gating(void)
{
	eMMC_PlatformResetPre();
	REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_CLK_EN);
	eMMC_PlatformResetPost();
	return eMMC_ST_SUCCESS;
}

void eMMC_set_WatchDog(U8 u8_IfEnable)
{
	// do nothing
}

void eMMC_reset_WatchDog(void)
{
	// do nothing
}
extern struct platform_device sg_mstar_emmc_device_st;

dma_addr_t eMMC_DMA_MAP_address(uintptr_t ulongBuffer, U32 u32_ByteCnt, int mode)
{
    dma_addr_t dma_addr;

    if(mode == 0)   //write
    {
        dma_addr = dma_map_single(&sg_mstar_emmc_device_st.dev, (void*)ulongBuffer, u32_ByteCnt, DMA_TO_DEVICE);
    }
    else
    {
        dma_addr = dma_map_single(&sg_mstar_emmc_device_st.dev, (void*)ulongBuffer, u32_ByteCnt, DMA_FROM_DEVICE);
    }

    if( dma_mapping_error(&sg_mstar_emmc_device_st.dev, dma_addr) )
    {
        dma_unmap_single(&sg_mstar_emmc_device_st.dev, dma_addr, u32_ByteCnt, (mode) ? DMA_FROM_DEVICE : DMA_TO_DEVICE);
        eMMC_die("eMMC_DMA_MAP_address: Kernel can't mapping dma correctly\n");
    }

    return dma_addr;
}

void eMMC_DMA_UNMAP_address(dma_addr_t dma_DMAAddr, U32 u32_ByteCnt, int mode)
{
    if(mode == 0)   //write
    {
        dma_unmap_single(&sg_mstar_emmc_device_st.dev, dma_DMAAddr, u32_ByteCnt, DMA_TO_DEVICE);
    }
    else
    {
        dma_unmap_single(&sg_mstar_emmc_device_st.dev, dma_DMAAddr, u32_ByteCnt, DMA_FROM_DEVICE);
    }
}

#if 0
U32 eMMC_translate_DMA_address_Ex(U32 u32_DMAAddr, U32 u32_ByteCnt, int mode)
{
#if 1
		extern void Chip_Clean_Cache_Range_VA_PA(unsigned long u32VAddr,unsigned long u32PAddr,unsigned long u32Size);
		extern void Chip_Flush_Cache_Range_VA_PA(unsigned long u32VAddr,unsigned long u32PAddr,unsigned long u32Size);
    //mode 0 for write, 1 for read
    if( mode == WRITE_TO_eMMC )	//Write
    {
        //Write (DRAM->NAND)-> flush
        Chip_Clean_Cache_Range_VA_PA(u32_DMAAddr,__pa(u32_DMAAddr), u32_ByteCnt);
    }
    else //Read
    {
        //Read (NAND->DRAM) -> inv
        Chip_Flush_Cache_Range_VA_PA(u32_DMAAddr,__pa(u32_DMAAddr), u32_ByteCnt);
    }
    /*
	if(virt_to_phys((void *)u32_DMAAddr) >= MSTAR_MIU1_BUS_BASE)
	{
    	REG_SET_BITS_UINT16( NC_MIU_DMA_SEL, BIT_MIU1_SELECT);
    }
    else
    	REG_CLR_BITS_UINT16( NC_MIU_DMA_SEL, BIT_MIU1_SELECT);
		*/

	return virt_to_phys((void *)u32_DMAAddr);
#else
	flush_cache(u32_DMAAddr, u32_ByteCnt);
	return (u32_DMAAddr);
#endif
}
#else
extern U32 MIU0_BUS_ADDR;
U32 eMMC_translate_DMA_address_Ex(dma_addr_t dma_DMAAddr, U32 u32_ByteCnt)
{
    REG_FCIE_CLRBIT(FCIE_MMA_PRI_REG, BIT_MIU_SELECT_MASK);
    dma_DMAAddr -= MSTAR_MIU0_BUS_BASE;

    return ((U32)dma_DMAAddr);
}

#endif
/*
void eMMC_Invalidate_data_cache_buffer(U32 u32_addr, S32 s32_size)
{
	flush_cache(u32_addr, s32_size);
}

void eMMC_flush_miu_pipe(void)
{

}
*/


//---------------------------------------
#if defined(ENABLE_eMMC_INTERRUPT_MODE)&&ENABLE_eMMC_INTERRUPT_MODE

static DECLARE_WAIT_QUEUE_HEAD(fcie_wait);
static volatile U32 fcie_int = 0;

#define eMMC_IRQ_DEBUG    0

irqreturn_t eMMC_FCIE_IRQ(int irq, void *dummy)
{
	volatile u16 u16_Events;

	// one time enable one bit

    REG_FCIE_R(FCIE_PWR_SAVE_CTL, u16_Events);

    if(u16_Events & BIT_POWER_SAVE_MODE_INT)
    {
        REG_FCIE_CLRBIT(FCIE_PWR_SAVE_CTL, BIT_POWER_SAVE_MODE_INT_EN);
        fcie_int = 1;
        eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,0, "SAR5 eMMC WARN.\n");
        while(1);
        wake_up(&fcie_wait);
        return IRQ_HANDLED;
    }

    if((REG_FCIE(FCIE_MIE_FUNC_CTL) & BIT_EMMC_ACTIVE) != BIT_EMMC_ACTIVE)
    {
        return IRQ_NONE;
    }

	u16_Events = REG_FCIE(FCIE_MIE_EVENT) & REG_FCIE(FCIE_MIE_INT_EN);

	if(u16_Events & (BIT_DMA_END|BIT_ERR_STS))
    {
        REG_FCIE_CLRBIT(FCIE_MIE_INT_EN, (BIT_DMA_END|BIT_ERR_STS));

        fcie_int = 1;
        wake_up(&fcie_wait);
		return IRQ_HANDLED;
    }
    else if(u16_Events & BIT_CMD_END)
    {
        REG_FCIE_CLRBIT(FCIE_MIE_INT_EN, BIT_CMD_END);

        fcie_int = 1;
        wake_up(&fcie_wait);
		return IRQ_HANDLED;
    }
	#if defined(ENABLE_FCIE_HW_BUSY_CHECK)&&ENABLE_FCIE_HW_BUSY_CHECK
    else if(u16_Events & BIT_BUSY_END_INT)
    {
        REG_FCIE_CLRBIT(FCIE_MIE_INT_EN, BIT_BUSY_END_INT);
        REG_FCIE_CLRBIT(FCIE_SD_CTRL, BIT_BUSY_DET_ON);

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


U32 eMMC_WaitCompleteIntr(uintptr_t u32_RegAddr, U16 u16_WaitEvent, U32 u32_MicroSec)
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
            REG_FCIE_CLRBIT(FCIE_MIE_INT_EN, u16_WaitEvent);
			return eMMC_ST_ERR_INT_TO;
		}
		else
		{	REG_FCIE_CLRBIT(FCIE_MIE_INT_EN, u16_WaitEvent);
			eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Warn: but polling ok: %Xh \n", REG_FCIE(u32_RegAddr));
		}

    }
	//----------------------------------------
	fcie_int = 0;
	return eMMC_ST_SUCCESS;
}

#endif



//---------------------------------------
//extern struct semaphore	PfModeSem;
//#include <linux/semaphore.h>
//extern struct semaphore	PfModeSem;
extern struct mutex FCIE3_mutex;
//#define CRIT_SECT_BEGIN(x)	mutex_lock(x)
//#define CRIT_SECT_END(x)	mutex_unlock(x)

extern bool ms_sdmmc_wait_d0_for_emmc(void);

void eMMC_LockFCIE(U8 *pu8_str)
{
	mutex_lock(&FCIE3_mutex);
	#if 0 //defined (CONFIG_OF)
    {
        int i;
        for(i = 0 ;i < clkdata->num_parents; i ++)
            clk_prepare_enable(clkdata->clk_fcie[i]);
    }
	#endif

	#if IF_FCIE_SHARE_IP // && defined(CONFIG_MS_SDMMC)
//	if(false == ms_sdmmc_wait_d0_for_emmc())
//	{
//		eMMC_debug(eMMC_DEBUG_LEVEL_ERROR,1,"eMMC Err: SD keep D0 low \n");
//		eMMC_FCIE_ErrHandler_Stop();
//	}

    REG_FCIE_CLRBIT(FCIE_TEST_MODE, BIT_DEBUG_MODE_MASK);
    REG_FCIE_SETBIT(FCIE_TEST_MODE, 2<<BIT_DEBUG_MODE_SHIFT); // 2: card_data1_dbus = {xxxxx, Wrstate, RDstate}

    REG_FCIE_CLRBIT(FCIE_EMMC_DEBUG_BUS1, BIT11|BIT10|BIT9|BIT8);
    REG_FCIE_SETBIT(FCIE_EMMC_DEBUG_BUS1, 5<<8); // 5: card


    if(REG_FCIE(FCIE_EMMC_DEBUG_BUS0)&0x0FFF) //Check FICE5 StateMachine
        eMMC_FCIE_Init();


	eMMC_clock_setting(g_eMMCDrv.u16_ClkRegVal);
	eMMC_pads_switch(g_eMMCDrv.u8_PadType);
	#endif
	REG_FCIE_SETBIT(FCIE_SD_MODE, BIT_SD_CLK_EN); // output clock

    REG_FCIE_SETBIT(FCIE_MIE_FUNC_CTL, BIT_EMMC_ACTIVE);

}

void eMMC_UnlockFCIE(U8 *pu8_str)
{
    REG_FCIE_CLRBIT(FCIE_MIE_FUNC_CTL, BIT_EMMC_ACTIVE);
	REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_SD_CLK_EN); // not output clock
	#if 0 //defined (CONFIG_OF)
    {
        int i;
        for(i = 0 ;i < clkdata->num_parents; i ++)
            clk_disable_unprepare(clkdata->clk_fcie[i]);
    }
	#endif

	mutex_unlock(&FCIE3_mutex);
}

U32 eMMC_PlatformResetPre(void)
{

	return eMMC_ST_SUCCESS;
}

U32 eMMC_PlatformResetPost(void)
{
    #if defined(ENABLE_EMMC_POWER_SAVING_MODE) && ENABLE_EMMC_POWER_SAVING_MODE
	eMMC_Prepare_Power_Saving_Mode_Queue();
    #endif

	return eMMC_ST_SUCCESS;
}
struct page *eMMC_SectorPage = 0;
struct page *eMMC_PartInfoPage = 0;
U8 *gau8_eMMC_SectorBuf = 0; // 512 bytes
U8 *gau8_eMMC_PartInfoBuf =0; // 512 bytes

U32 eMMC_PlatformInit(void)
{
	eMMC_pads_switch(FCIE_eMMC_BYPASS);
	eMMC_clock_setting(FCIE_SLOWEST_CLK);

    if(gau8_eMMC_SectorBuf == NULL)
    {
        eMMC_SectorPage = alloc_pages(__GFP_COMP, 2);
        if(eMMC_SectorPage ==NULL)
        {
            eMMC_debug(0, 1, "Err allocate page 1 fails\n");
            eMMC_die();
        }
        gau8_eMMC_SectorBuf =(U8*) kmap(eMMC_SectorPage);
    }

    if(gau8_eMMC_PartInfoBuf == NULL)
    {
        eMMC_PartInfoPage = alloc_pages(__GFP_COMP, 0);
        if(eMMC_PartInfoPage ==NULL)
        {
            eMMC_debug(0, 1, "Err allocate page 2 fails\n");
            eMMC_die();
        }
        gau8_eMMC_PartInfoBuf = (U8*)kmap(eMMC_PartInfoPage);
    }

	return eMMC_ST_SUCCESS;
}


U32 eMMC_BootPartitionHandler_WR(U8 *pDataBuf, U16 u16_PartType, U32 u32_StartSector, U32 u32_SectorCnt, U8 u8_OP)
{
	return eMMC_ST_SUCCESS;
}


U32 eMMC_BootPartitionHandler_E(U16 u16_PartType)
{
	return eMMC_ST_SUCCESS;
}

// --------------------------------------------
static U32 sgu32_MemGuard0 = 0xA55A;
eMMC_ALIGN0 eMMC_DRIVER g_eMMCDrv eMMC_ALIGN1;
static U32 sgu32_MemGuard1 = 0x1289;

U32 eMMC_CheckIfMemCorrupt(void)
{
	if(0xA55A != sgu32_MemGuard0 || 0x1289 != sgu32_MemGuard1)
		return eMMC_ST_ERR_MEM_CORRUPT;

	return eMMC_ST_SUCCESS;
}

int mstar_mci_Housekeep(void *pData)
{
    #if !(defined(eMMC_HOUSEKEEP_THREAD) && eMMC_HOUSEKEEP_THREAD)
    return 0;
    #endif



    while(1)
    {
        if(kthread_should_stop())
            break;
    }

    return 0;
}

U32 eMMC_PlatformDeinit(void)
{
    return eMMC_ST_SUCCESS;
}

#else


  #error "Error! no platform functions."
#endif
#endif
