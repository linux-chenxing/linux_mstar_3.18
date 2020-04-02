///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2007 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "MsCommon.h"
#include <linux/autoconf.h>
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

#include "mhal_iic.h"
#include "mhal_iic_reg.h"

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
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
void MHal_IIC_Init(void)
{
    //printk("%s is invoked\n", __FUNCTION__ );

    MHal_CHIP_REG(REG_IIC_ALLPADIN) &= ~BIT15;
    //iic ddcr mode is enabled, bit 1~0 set to 2'b10
    MHal_CHIP_REG(REG_IIC_MODE) &= ~BIT1;
    MHal_CHIP_REG(REG_IIC_MODE) &= ~BIT0;

    // clock = (Xtal / 4T) / div
    MHal_IIC_REG(REG_IIC_CLK_SEL) = 3; // 28.8M / 4 / 16 = 450KHz
    MHal_IIC_REG(REG_IIC_CTRL) = 0x80UL; // enable IIC
}

void MHal_IIC_Clock_Select(U8 u8ClockIIC)
{
    //master iic clock select.
    //if 1:clk/4, if 2:clk/8, if 3:clk/16, if 4:clk/32,
    //if 5:clk/64, if 6:clk/128, if 7:clk/256, if 8:clk/512,
    //if 9:clk/1024, otherwise: clk/2
    MHal_IIC_REG(REG_IIC_CLK_SEL) = u8ClockIIC;
}

void MHal_IIC_Start(void)
{
    //reset iic circuit
    MHal_IIC_REG(REG_IIC_STATUS) = 0x08UL;
    MHal_IIC_REG(REG_IIC_STATUS) = 0x00UL;

    //reset iic circuit
    MHal_IIC_REG(REG_IIC_CTRL) = 0xC0UL;
    udelay(100);
}

void MHal_IIC_Stop(void)
{
    MHal_IIC_REG(REG_IIC_CTRL) = 0xA0UL;
    //MHal_IIC_DELAY();
    udelay(100);
}

void MHal_IIC_NoAck(void)
{
    MHal_IIC_REG(REG_IIC_CTRL) = 0x90UL;
}

B16 MHal_IIC_SendData(U8 u8DataIIC)
{


    U32 u32cur_jiffies;
#define _TIMEOUT_SEND_DATA  (HZ/2)  /* 0.5 sec */
    //static U32 max_loop;
    //U32 loop = 0;


    MHal_IIC_REG(REG_IIC_STATUS) = 0x02UL; /* clear interrupt */
    MHal_IIC_REG(REG_IIC_WDATA) = u8DataIIC;


#if 0
    {   //tune the dalay
    #if 1
        U8 u8Delay;
        for(u8Delay=0; u8Delay<100; u8Delay++);
        udelay(1000);
    #else
        udelay(500);
    #endif
    }
#endif




    u32cur_jiffies = jiffies;
    while( jiffies - u32cur_jiffies <= _TIMEOUT_SEND_DATA )

    {
        //loop++

        if (MHal_IIC_REG(REG_IIC_STATUS) & 0x01UL)
        {
            //if( loop > max_loop )
            //{
            //  max_loop = loop;
            //  printk("_IIC_SendData MAX: %d(%d)\n", max_loop, jiffies - u32cur_jiffies );
            //}

            MHal_IIC_REG(REG_IIC_STATUS) = 0x02UL; /* clear interrupt */
            if (MHal_IIC_REG(REG_IIC_CTRL) & 0x08UL) /* no ACK */
            {
                return FALSE;
            }
            else
            {
                udelay(1);
                return TRUE;
            }
        }
    }

    return FALSE;
}

B16 MHal_IIC_SendByte(U8 u8DataIIC)
{
    U8 u8I;

    for(u8I=0;u8I<10;u8I++)
    {
        if(MHal_IIC_SendData(u8DataIIC) == TRUE)
            return TRUE;
    }
    return FALSE;
}

B16 MHal_IIC_GetByte(U8* pu8DataIIC) /* auto generate ACK */
{


    U32 u32cur_jiffies;
#define _TIMEOUT_GET_BYTE   (HZ/2)  /* 0.5 sec */
    //static U32 max_loop;
    //U32 loop = 0;


    /* clear interrupt & start byte reading */
    MHal_IIC_REG(REG_IIC_STATUS) = 0x04UL;


#if 0
    {   //tune the dalay
        U8 u8Delay;
        for(u8Delay=0; u8Delay<100; u8Delay++);
        udelay(1000);
    }
#endif




    u32cur_jiffies = jiffies;
    while( jiffies - u32cur_jiffies <= _TIMEOUT_GET_BYTE )

    {
        //loop++;

        if (MHal_IIC_REG(REG_IIC_STATUS))
        {
            MHal_IIC_REG(REG_IIC_STATUS) = 0x02UL; /* clear interrupt */
            *pu8DataIIC = MHal_IIC_REG(REG_IIC_RDATA);
            udelay(1);

            //if( loop > max_loop )
            //{
            //  max_loop = loop;
                //printk("_IIC_GetByte MAX: %d(%d)\n", max_loop, jiffies - u32cur_jiffies);
            //}

            return TRUE;
        }
    }

    return FALSE;
}

