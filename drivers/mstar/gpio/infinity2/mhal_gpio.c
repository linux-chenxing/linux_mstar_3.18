////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (��MStar Confidential Information��) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "MsCommon.h"
//#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <asm/io.h>
#include "mhal_gpio.h"
#include "mhal_gpio_reg.h"
#include "ms_platform.h"
#include "gpio.h"
#include "irqs.h"

#ifndef MS_ASSERT

#define MS_ASSERT(expr) do { \
	if(!(expr)) \
			printk("MVD assert fail %s %d!\n", __FILE__, __LINE__); \
} while(0)
#endif

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define _CONCAT( a, b )     a##b
#define CONCAT( a, b )      _CONCAT( a, b )

// Dummy
#define GPIO999_OEN     0, 0
#define GPIO999_OUT     0, 0
#define GPIO999_IN      0, 0


#include "GPIO_TABLE.h"

U32 gPadTop1BaseAddr=0xFD205200;
U32 gChipBaseAddr=0xFD203C00;
U32 gPmSleepBaseAddr=0xFD001C00;
U32 gSarBaseAddr=0xFD002800;
U32 gRIUBaseAddr=0xFD000000;

#define MHal_PADTOP1_REG(addr)             (*(volatile U8*)(gChipBaseAddr + (((addr) & ~1)<<1) + (addr & 1)))
#define MHal_CHIPTOP_REG(addr)             (*(volatile U8*)(gChipBaseAddr + (((addr) & ~1)<<1) + (addr & 1)))
#define MHal_PM_SLEEP_REG(addr)            (*(volatile U8*)(gPmSleepBaseAddr + (((addr) & ~1)<<1) + (addr & 1)))
#define MHal_SAR_GPIO_REG(addr)            (*(volatile U8*)(gSarBaseAddr + (((addr) & ~1)<<1) + (addr & 1)))
#define MHal_RIU_REG(addr)                 (*(volatile U8*)(gRIUBaseAddr + (((addr) & ~1)<<1) + (addr & 1)))

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//the functions of this section set to initialize
void MHal_GPIO_Init(void)
{
//    printk("MHal_GPIO_Init gBaseAddr=%x\n",gPadBaseAddr);
    MHal_PADTOP1_REG(REG_ALL_PAD_IN) &= ~BIT0;
}

void MHal_CHIPTOP_WriteRegBit(U32 u32Reg, U8 u8Enable, U8 u8BitMsk)
{
    if(u8Enable)
        MHal_CHIPTOP_REG(u32Reg) |= u8BitMsk;
    else
        MHal_CHIPTOP_REG(u32Reg) &= (~u8BitMsk);
}

U8 MHal_CHIPTOP_ReadRegBit(U32 u32Reg, U8 u8BitMsk)
{
    return ((MHal_CHIPTOP_REG(u32Reg)&u8BitMsk)? 1 : 0);
}

U8 MHal_CHIPTOP_ReadRegMsk(U32 u32Reg, U8 u8BitMsk)
{
    return (MHal_CHIPTOP_REG(u32Reg)&u8BitMsk);
}

void MHal_PM_SLEEP_WriteRegBit(U32 u32Reg, U8 u8Enable, U8 u8BitMsk)
{
    if(u8Enable)
        MHal_PM_SLEEP_REG(u32Reg) |= u8BitMsk;
    else
        MHal_PM_SLEEP_REG(u32Reg) &= (~u8BitMsk);
}

U8 MHal_PM_SLEEP_ReadRegBit(U32 u32Reg, U8 u8BitMsk)
{
    return ((MHal_PM_SLEEP_REG(u32Reg)&u8BitMsk)? 1 : 0);
}

U8 MHal_PM_SLEEP_ReadRegMsk(U32 u32Reg, U8 u8BitMsk)
{
    return (MHal_PM_SLEEP_REG(u32Reg)&u8BitMsk);
}

void MHal_SAR_GPIO_WriteRegBit(U32 u32Reg, U8 u8Enable, U8 u8BitMsk)
{
    if(u8Enable)
        MHal_SAR_GPIO_REG(u32Reg) |= u8BitMsk;
    else
        MHal_SAR_GPIO_REG(u32Reg) &= (~u8BitMsk);
}

void MHal_FuartPAD_DisableFunction(void)
{
	printk("TBD MHal_FuartPAD_DisableFunction\n");
}

void MHal_SPI0PAD_DisableFunction(void)
{
	printk("TBD MHal_FuartPAD_DisableFunction\n");
}

void MHal_GPIO_Pad_Set(U16 u16IndexGPIO)
{
    //printk("[gpio]MHal_GPIO_Pad_Set %d\n", u8IndexGPIO);
    switch(u16IndexGPIO)
    {
		case PAD_SAR_GPIO0:
				MHal_SAR_GPIO_WriteRegBit(REG_SAR_MODE,DISABLE,BIT0);
				break;
		case PAD_SAR_GPIO1:
				MHal_SAR_GPIO_WriteRegBit(REG_SAR_MODE,DISABLE,BIT1);
				break;
		case PAD_SAR_GPIO2:
				MHal_SAR_GPIO_WriteRegBit(REG_SAR_MODE,DISABLE,BIT2);
				break;
		case PAD_SAR_GPIO3:
				MHal_SAR_GPIO_WriteRegBit(REG_SAR_MODE,DISABLE,BIT3);
				break;
		case PAD_SAR_GPIO4:
				MHal_SAR_GPIO_WriteRegBit(REG_SAR_MODE,DISABLE,BIT4);
				break;
            default:
                break;
    }
}

void MHal_GPIO_Pad_Oen(U16 u16IndexGPIO)
{
	if (u16IndexGPIO >= 0 && u16IndexGPIO < GPIO_NR)
	{
    	MHal_RIU_REG(gpio_table[u16IndexGPIO].r_oen) &= (~gpio_table[u16IndexGPIO].m_oen);
	}
	else
	{
		MS_ASSERT(0);
	}
}

void MHal_GPIO_Pad_Odn(U16 u16IndexGPIO)
{
	if (u16IndexGPIO >= 0 && u16IndexGPIO < GPIO_NR)
	{
    	MHal_RIU_REG(gpio_table[u16IndexGPIO].r_oen) |= gpio_table[u16IndexGPIO].m_oen;
	}
	else
	{
		MS_ASSERT(0);
	}
}

U16 MHal_GPIO_Pad_Level(U16 u16IndexGPIO)
{

	if (u16IndexGPIO >= 0 && u16IndexGPIO < GPIO_NR)
	{
		return ((MHal_RIU_REG(gpio_table[u16IndexGPIO].r_in)&gpio_table[u16IndexGPIO].m_in)? 1 : 0);
	}
	else
	{
		MS_ASSERT(0);
		return -1;
	}
}

U16 MHal_GPIO_Pad_InOut(U16 u16IndexGPIO)
{
	if (u16IndexGPIO >= 0 && u16IndexGPIO < GPIO_NR)
	{
		return ((MHal_RIU_REG(gpio_table[u16IndexGPIO].r_oen)&gpio_table[u16IndexGPIO].m_oen)? 1 : 0);
	}
	else
	{
		MS_ASSERT(0);
		return -1;
	}
}

void MHal_GPIO_Pull_High(U16 u16IndexGPIO)
{
	if (u16IndexGPIO >= 0 && u16IndexGPIO < GPIO_NR)
	{
    	MHal_RIU_REG(gpio_table[u16IndexGPIO].r_out) |= gpio_table[u16IndexGPIO].m_out;
	}
	else
	{
		MS_ASSERT(0);
	}
}

void MHal_GPIO_Pull_Low(U16 u16IndexGPIO)
{
	if (u16IndexGPIO >= 0 && u16IndexGPIO < GPIO_NR)
	{
    	MHal_RIU_REG(gpio_table[u16IndexGPIO].r_out) &= (~gpio_table[u16IndexGPIO].m_out);
	}
	else
	{
		MS_ASSERT(0);
	}
}

void MHal_GPIO_Set_High(U16 u16IndexGPIO)
{
	if (u16IndexGPIO >= 0 && u16IndexGPIO < GPIO_NR)
	{
	    MHal_RIU_REG(gpio_table[u16IndexGPIO].r_oen) &= (~gpio_table[u16IndexGPIO].m_oen);
	    MHal_RIU_REG(gpio_table[u16IndexGPIO].r_out) |= gpio_table[u16IndexGPIO].m_out;
	}
	else
	{
		MS_ASSERT(0);
	}
}

void MHal_GPIO_Set_Low(U16 u16IndexGPIO)
{
	if (u16IndexGPIO >= 0 && u16IndexGPIO < GPIO_NR)
	{
	    MHal_RIU_REG(gpio_table[u16IndexGPIO].r_oen) &= (~gpio_table[u16IndexGPIO].m_oen);
	    MHal_RIU_REG(gpio_table[u16IndexGPIO].r_out) &= (~gpio_table[u16IndexGPIO].m_out);
	}
	else
	{
		MS_ASSERT(0);
	}
}

void MHal_Enable_GPIO_INT(U16 u16IndexGPIO)
{
/*     switch(u8IndexGPIO)
    {
  case PAD_GPIO7:
        MHal_CHIPTOP_WriteRegBit(1,DISABLE,BIT7);
        MHal_CHIPTOP_WriteRegBit(2,ENABLE,BIT0);
        break;
    case PAD_GPIO8:
        MHal_CHIPTOP_WriteRegBit(REG_GPIO1_25,ENABLE,BIT1);
        break;
    case PAD_GPIO9:
        MHal_CHIPTOP_WriteRegBit(REG_GPIO1_25,ENABLE,BIT2);
        break;
    case PAD_GPIO13:
        MHal_CHIPTOP_WriteRegBit(REG_GPIO1_25,ENABLE,BIT3);
        break;
    case PAD_GPIO28:
        MHal_CHIPTOP_WriteRegBit(REG_GPIO1_25,ENABLE,BIT4);
        break;
    case PAD_GPIO29:
        MHal_CHIPTOP_WriteRegBit(REG_GPIO1_21,DISABLE,BIT2);
        MHal_CHIPTOP_WriteRegBit(REG_GPIO1_25,ENABLE,BIT5);
        break;
    case PAD_GPIO30:
        MHal_CHIPTOP_WriteRegBit(REG_GPIO1_20,DISABLE,BIT5);
        MHal_CHIPTOP_WriteRegBit(REG_GPIO1_02,DISABLE,BIT3);
        MHal_CHIPTOP_WriteRegBit(REG_GPIO1_20,DISABLE,BIT4);
        MHal_CHIPTOP_WriteRegBit(REG_GPIO1_25,ENABLE,BIT6);
        break;
    case PAD_GPIO31:
        MHal_CHIPTOP_WriteRegBit(REG_GPIO1_20,DISABLE,BIT5);
        MHal_CHIPTOP_WriteRegBit(REG_GPIO1_02,DISABLE,BIT3);
        MHal_CHIPTOP_WriteRegBit(REG_GPIO1_20,DISABLE,BIT4);
        MHal_CHIPTOP_WriteRegBit(REG_GPIO1_25,ENABLE,BIT7);
        break;
	default:
	break;
    }*/
}

int MHal_GPIO_To_Irq(U16 u16IndexGPIO)
{
    
    //256 is PMSLEEP virtual irq start
    int IRQ_NUR = 0;

    if(u16IndexGPIO>=PAD_PM_GPIO0 && u16IndexGPIO<=PAD_PM_GPIO15)
        return (u16IndexGPIO - PAD_PM_GPIO0 + (GPIO_0+GIC_SPI_MS_IRQ1_END+GIC_HWIRQ_MS_START));

    else if(u16IndexGPIO>=PAD_PM_GPIO16 && u16IndexGPIO<=PAD_PM_GPIO19)
    {   
        return (u16IndexGPIO - PAD_PM_GPIO16 + (GPIO_16+GIC_SPI_MS_IRQ1_END+GIC_HWIRQ_MS_START));
    }

    else if(u16IndexGPIO==PAD_PM_IRIN)
        return GPIO_20_IR + GIC_SPI_MS_IRQ1_END+GIC_HWIRQ_MS_START;

    else if(u16IndexGPIO==PAD_PM_CEC)
        return GPIO_22_CEC + GIC_SPI_MS_IRQ1_END+GIC_HWIRQ_MS_START;

    else if(u16IndexGPIO>=PAD_PM_SD30_CDZ && u16IndexGPIO<=PAD_PM_SD20_CDZ)
    {   
        IRQ_NUR = GPIO_78_PAD_PM_SD30_CDZ + GIC_SPI_MS_IRQ1_END+GIC_HWIRQ_MS_START;
        return  (u16IndexGPIO - PAD_PM_SD30_CDZ + IRQ_NUR);
    }

    else if(u16IndexGPIO>=PAD_VID0 && u16IndexGPIO<=PAD_PM_LED1)
    {   
        IRQ_NUR = GPIO_72_PAD_VID0 + GIC_SPI_MS_IRQ1_END+GIC_HWIRQ_MS_START;
        return  (u16IndexGPIO - PAD_VID0 + IRQ_NUR);
    }

    else if(u16IndexGPIO==PAD_PM_SPI_CZ1)
        return GPIO_76_PAD_PM_SPI_CZ1 + GIC_SPI_MS_IRQ1_END+GIC_HWIRQ_MS_START;

// [edie] sar to be check
//	else if(u8IndexGPIO>=PAD_SAR_GPIO0 && u8IndexGPIO<=PAD_SAR_GPIO3)
//        return (u8IndexGPIO - PAD_SAR_GPIO0 + (INT_FIQ_SAR_GPIO_0)+GIC_SGI_NR+GIC_PPI_NR);
    else
        return -1;
}

void MHal_GPIO_Set_POLARITY(U16 u16IndexGPIO,U16 reverse)
{
//printk("TBD MHal_GPIO_Set_POLARITY\n");
/*
IdxGPIO  GPIOx  IdxFIQ
70  --GPIO31 -- 63 -- ext_gpio_int[7]  -- reg_hst0_fiq_polarity_63_48_  -- [h00b,h00b]
71  --GPIO30 -- 58 -- ext_gpio_int[6]  -- reg_hst0_fiq_polarity_63_48_  -- [h00b,h00b]
72  --GPIO29 -- 57 -- ext_gpio_int[5]  -- reg_hst0_fiq_polarity_63_48_  -- [h00b,h00b]
73  -- GPIO28 -- 56 -- ext_gpio_int[4]  -- reg_hst0_fiq_polarity_63_48_  -- [h00b,h00b]
113 --GPIO13 -- 55 -- ext_gpio_int[3]  -- reg_hst0_fiq_polarity_63_48_  -- [h00b,h00b]
117 --GPIO9   -- 47 -- ext_gpio_int[2]  -- reg_hst0_fiq_polarity_47_32_  -- [h00a,h00a]
118 --GPIO8   -- 43 -- ext_gpio_int[1]  -- reg_hst0_fiq_polarity_47_32_  -- [h00a,h00a]
119 --GPIO7   -- 39 -- ext_gpio_int[0]  -- reg_hst0_fiq_polarity_47_32_  -- [h00a,h00a]
*/

    switch(u16IndexGPIO)
    {
/*    case 119:  //INT_FIQ_EXT_GPIO0
        if(reverse==0)
            SETREG16(GET_REG_ADDR(MS_INT_BASE, 0x0a), 1 << 7); //Set To Falling edge trigger
        else
            CLRREG16(GET_REG_ADDR(MS_INT_BASE, 0x0a), 1 << 7); //Set To Raising edge trigger
        break;
    case 118:
        if(reverse==0)
            SETREG16(GET_REG_ADDR(MS_INT_BASE, 0x0a), 1 << 11); //Set To Falling edge trigger
        else
            CLRREG16(GET_REG_ADDR(MS_INT_BASE, 0x0a), 1 << 11); //Set To Raising edge trigger
        break;
    case 117:
        if(reverse==0)
            SETREG16(GET_REG_ADDR(MS_INT_BASE, 0x0a), 1 << 15); //Set To Falling edge trigger
        else
            CLRREG16(GET_REG_ADDR(MS_INT_BASE, 0x0a), 1 << 15); //Set To Raising edge trigger
        break;
    case 113:
        if(reverse==0)
            SETREG16(GET_REG_ADDR(MS_INT_BASE, 0x0b), 1 << 7); //Set To Falling edge trigger
        else
            CLRREG16(GET_REG_ADDR(MS_INT_BASE, 0x0b), 1 << 7); //Set To Raising edge trigger
        break;
    case 73:
        if(reverse==0)
            SETREG16(GET_REG_ADDR(MS_INT_BASE, 0x0b), 1 << 8); //Set To Falling edge trigger
        else
            CLRREG16(GET_REG_ADDR(MS_INT_BASE, 0x0b), 1 << 8); //Set To Raising edge trigger
        break;
    case 72:
        if(reverse==0)
            SETREG16(GET_REG_ADDR(MS_INT_BASE, 0x0b), 1 << 9); //Set To Falling edge trigger
        else
            CLRREG16(GET_REG_ADDR(MS_INT_BASE, 0x0b), 1 << 9); //Set To Raising edge trigger
        break;
    case 71:
        if(reverse==0)
            SETREG16(GET_REG_ADDR(MS_INT_BASE, 0x0b), 1 << 10); //Set To Falling edge trigger
        else
            CLRREG16(GET_REG_ADDR(MS_INT_BASE, 0x0b), 1 << 10); //Set To Raising edge trigger
        break;
    case 70:
        if(reverse==0)
            SETREG16(GET_REG_ADDR(MS_INT_BASE, 0x0b), 1 << 15); //Set To Falling edge trigger
        else
            CLRREG16(GET_REG_ADDR(MS_INT_BASE, 0x0b), 1 << 15); //Set To Raising edge trigger
        break;
*/
    default:
        break;
    }
}



