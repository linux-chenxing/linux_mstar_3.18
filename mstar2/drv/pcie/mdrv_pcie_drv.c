///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2014 MStar Semiconductor, Inc.
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


/*
 * ============================================================================
 * Include Headers
 * ============================================================================
 */

#include <linux/types.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#include <linux/kernel_stat.h>

/* Define start addresses for PCIE registers */
//#include <mach/pcie_addr.h>

#include "cpdi.h"
#include "cps_v2.h"
#include "cdn_stdint.h"
#include "cpdi_core_driver.h"
#include "mstar_pcie_addrmap.h"
#include "mstar_pcie_axi2mi_bridge_reg.h"
#include "mstar_pcie_rc_reg.h"
#include "chip_int.h"
//#include <string.h>
#ifdef PCIE_ENABLE_PPC
#include "xhci-mstar.h"
#endif

/*
 * ============================================================================
 * Constant Global Definitions
 * ============================================================================
 */

 extern uint32_t g_rc_data;

 #ifdef DEBUG_RUNTIME_LOCAL_ERROR
 extern struct Mstar_LocalError_Descriptor  localerror_count;
 #endif

 #ifdef CONFIG_MSTAR_K6
 void __iomem *reg_dm_base;
 #endif

/*
 * ============================================================================
 * Local Variables & Functions Definitions
 * ============================================================================
 */
 uint32_t check_interrupt_assert(void);
 uint32_t check_interrupt_deassert(void);
 uint32_t check_dma_int_assert(void);
 uint32_t check_dma_int_deassert(void);

 static void _mdrv_Mstar_U3phy_MS28_init(uintptr_t U3PHY_D_base, uintptr_t U3PHY_A_base);
 static void _mdrv_Mstar_U3phy_MS28_poweroff(uintptr_t U3PHY_D_base, uintptr_t U3PHY_A_base);
 int _mdrv_RC_ob_memio_init(void *pD, uint32_t iomem_base, int region_sta, int region_count);

// static void _mdrv_Mstar_U3phy_testbus_setting(void);
/*
 * ============================================================================
 * Forward Declarations
 * ============================================================================
 */
#ifdef ZEBU_ESL
unsigned char Read8(unsigned long long addr)
{
    volatile unsigned char *tmp_addr = (volatile unsigned char *) addr;
    return *(tmp_addr);
}
unsigned short Read16(unsigned long long addr)
{
    volatile unsigned short *tmp_addr = (volatile unsigned short *) addr;
    uart_puts("R e a d 1 6 :", 14);
    uart_write_U32_hex(addr); uart_puts("= >", 4); uart_write_U16_hex(*tmp_addr);  uart_putc('\n');
    return *(tmp_addr);
}
unsigned int32_t Read32(unsigned long long addr)
{
    volatile unsigned int *tmp_addr = (volatile unsigned int *) addr;
    uart_puts("R e a d 3 2 :", 14);
    uart_write_U32_hex(addr); uart_puts("= >", 4); uart_write_U32_hex(*tmp_addr);  uart_putc('\n');

    return *(tmp_addr);
}
unsigned long long Read64(unsigned long long addr)
{
    volatile unsigned long long *tmp_addr = (volatile unsigned long long *) addr;

    volatile unsigned int addr_hi = (volatile unsigned int) (addr >> 32);
    volatile unsigned int addr_lo = (volatile unsigned int) (addr);
    volatile unsigned int tmp_hi = (volatile unsigned int) (*tmp_addr >> 32);
    volatile unsigned int tmp_lo = (volatile unsigned int) (*tmp_addr);

    //uart_puts("R e a d 6 4 :", 14);
    //uart_write_U32_hex(addr_hi); uart_write_U32_hex(addr_lo); uart_puts("= >", 4); uart_write_U32_hex(*tmp_hi); uart_write_U32_hex(*tmp_lo);  uart_putc('\n');

    return *(tmp_addr);
}
uint32_t revert(uint32_t input)
{
    uint32_t output=0;

    output = ((input & 0xFF000000)>>24) | ((input & 0x00FF0000)>>8) | ((input & 0x0000FF00)<<8) | ((input & 0x000000FF)<<24);
    return output;
}
void Write8(unsigned long long addr, unsigned char data)
{
    volatile unsigned char *tmp_addr = (volatile unsigned char *) addr;
    *(tmp_addr) = data;
}
void Write16(unsigned long long addr, unsigned short data)
{
    volatile unsigned short *tmp_addr = (volatile unsigned short *) addr;
    uart_puts("W r i t e 1 6 :", 16);
    uart_write_U32_hex(addr); uart_puts("= >", 4); uart_write_U16_hex(data);  uart_putc('\n');
    *(tmp_addr) = data;
}
void Write32(unsigned long long addr, unsigned int data)
{
    volatile unsigned int *tmp_addr = (volatile unsigned int *) addr;
    uart_puts("W r i t e 3 2 :", 16);
    uart_write_U32_hex(addr); uart_puts("= >", 4); uart_write_U32_hex(data);  uart_putc('\n');

    *(tmp_addr) = data;
}
void Zebu_memset(unsigned long long addr, uint32_t data, uint32_t number)
{
    uint32_t i;
    volatile uint32_t *tmp_addr = (volatile uint32_t *) addr;
    for(i=0; i<number; i=i+4)
    {
        *(tmp_addr) = data;
        tmp_addr +=1;
    }
}

void uart_putc(char out)
{
    volatile char *byte_addr;
    byte_addr = (volatile char*) (FUART_BASE);
    *(byte_addr+0*8) = out;
}
void uart_puts(char out[], int string_num)
{
   int i;

   if(string_num <= 0)
       return;

   for(i=0; i<string_num; i++){
       uart_putc(out[i]);
       i++;
   }
}
void uart_write_U8_hex(unsigned char c)
{
    unsigned char tmp=0;
    tmp=c;
    if((c>>4)>=10)
    {
        c=(0x61+(c>>4)-10);
        uart_putc(c);
    }
    else
    {
        c=(0x30+(c>>4));
        uart_putc(c);
    }

    c=tmp;
    if((c&0x0f)>=10)
    {
        c=(0x61+(c&0x0f)-10);
        uart_putc(c);
    }
    else
    {
        c=(0x30+(c&0x0f));
        uart_putc(c);
    }
}
void uart_write_U16_hex(unsigned short val)
{
    unsigned int tmp=0;
    unsigned char *ptr=NULL;
    int i=0;
    tmp=val;

    ptr = (unsigned char *) &tmp;
    for(i=1;i>=0;i--)
    {
        uart_write_U8_hex(ptr[i]);
    }
}
void uart_write_U32_hex(unsigned int val)
{
    unsigned int tmp=0;
    unsigned char *ptr=NULL;
    int i=0;
    tmp=val;

    ptr = (unsigned char *) &tmp;
    for(i=3;i>=0;i--)
    {
        uart_write_U8_hex(ptr[i]);
    }
}

void uart_test()
{
    volatile char *byte_addr;
    byte_addr = (volatile char *) (FUART_BASE);
    *(byte_addr+3*8) = 0x80;
    *(byte_addr+1*8) = 0x00;
    *(byte_addr+0*8) = 0x04;
    *(byte_addr+3*8) = 0x03;
    *(byte_addr+4*8) = 0x22;
    *(byte_addr+2*8) = 0xc1;
    *(byte_addr+1*8) = 0x01;
    *(byte_addr+0*8) = 0x01;
    //*(byte_addr+0*8) = 0x5a;
    //*(byte_addr+0*8) = 0x5a;
    //*(byte_addr+0*8) = 0x5a;
    *(byte_addr+0*8) = '\n';
}

#else
uint8_t Read8(unsigned long long addr)
{
    volatile unsigned char *tmp_addr = (volatile unsigned char *) addr;
    pr_info ("     Read8()  addr= %llx\n", addr);
    pr_info ("     Read8()  *(tmp_addr)= %x\n", *(tmp_addr));
    return *(tmp_addr);
}
uint16_t Read16(unsigned long long addr)
{
    volatile unsigned short *tmp_addr = (volatile unsigned short *) addr;
    return *(tmp_addr);
}
uint32_t Read32(unsigned long long addr)
{
    volatile unsigned int *tmp_addr = (volatile unsigned int *) addr;
    return *(tmp_addr);
}
unsigned long long Read64(unsigned long long addr)
{
    volatile unsigned long long *tmp_addr = (volatile unsigned long long *) addr;
    return *(tmp_addr);
}
void Write8(unsigned long long addr, unsigned char data)
{
    volatile unsigned char *tmp_addr = (volatile unsigned char *) addr;
    *(tmp_addr) = data;
}
void Write16(unsigned long long addr, unsigned short data)
{
    volatile unsigned short *tmp_addr = (volatile unsigned short *) addr;
    *(tmp_addr) = data;
}
void Write32(unsigned long long addr, unsigned int data)
{
    volatile unsigned int *tmp_addr = (volatile unsigned int *) addr;
    *(tmp_addr) = data;
}
#endif


uint32_t check_interrupt_assert(void)
{
    unsigned short u32done_int = 0, u32count = 0 ,  u32i = 0;

    while(u32count < 1000)
    {
        u32done_int = (Read16 (REG_PCIE_CM_DONE_INT) & REG_PCIE_CM_DONE_INT_MASK);
        u32i++;

        if(u32done_int)
        {
            Write16(REG_PCIE_CM_DONE_INT_CLR, REG_PCIE_CM_DONE_INT_CLR_MASK);
            break;
        }
        else if(u32i >= 1000)
        {
            u32i = 0;
            u32count++;
            //mdelay(1);
        }
    }

    if(u32count >= 1000)
    {
#ifdef ZEBU_ESL
        uart_puts("C I A E r r o r !", 18);
        uart_putc('\n');
#else
        printk("    check_interrupt_assert() Error !\n");
        return -1;
#endif
    }
    return 0;
}

uint32_t check_interrupt_deassert(void)
{
    unsigned short u32done_int = 0, u32count = 0 ,  u32i = 0;

    while(u32count < 1000)
    {
        u32done_int = (Read16 (REG_PCIE_CM_DONE_INT) & REG_PCIE_CM_DONE_INT_MASK);
        u32i++;

        if(u32done_int == 0)
        {
            break;
        }
        else
        {
            Write16(REG_PCIE_CM_DONE_INT_CLR, REG_PCIE_CM_DONE_INT_CLR_MASK);
        }

        if(u32i >= 1000)
        {
            u32i = 0;
            u32count++;
            //Sleep(100);
        }
    }

    if(u32count >= 1000)
    {
#ifdef ZEBU_ESL
        uart_puts("C I A E r r o r !", 18);
        uart_putc('\n');
#else
        printk("    check_interrupt_deassert() Error !\n");
        return -1;
#endif
    }
    return 0;
}

uint32_t check_dma_int_assert(void)
{
    unsigned short u32done_int = 0, u32count = 0 ,  u32i = 0;

    while(u32count < 1000)
    {
        u32done_int = (Read16 (REG_CH0_LLIST_DONE_INT) & REG_CH0_LLIST_DONE_INT_MASK);
        u32i++;

        if(u32done_int)
        {
            Write16(REG_CH0_LLIST_DONE_INT_CLR, REG_CH0_LLIST_DONE_INT_CLR_MASK);
            break;
        }
        else if(u32i >= 1000)
        {
            u32i = 0;
            u32count++;
            //mdelay(1);
        }
    }

    if(u32count >= 1000)
    {
#ifdef ZEBU_ESL
        uart_puts("C I A E r r o r !", 18);
        uart_putc('\n');
#else
        printk("   check_dma_int_assert() Error !\n");
        return -1;
#endif
    }
    return 0;
}

uint32_t check_dma_int_deassert(void)
{
    unsigned short u32done_int = 0, u32count = 0 ,  u32i = 0;

    while(u32count < 1000)
    {
        u32done_int = (Read16 (REG_CH0_LLIST_DONE_INT) & REG_CH0_LLIST_DONE_INT_MASK);
        u32i++;

        if(u32done_int == 0)
        {
            break;
        }
        else
        {
            Write16(REG_CH0_LLIST_DONE_INT_CLR, REG_CH0_LLIST_DONE_INT_CLR_MASK);
        }

        if(u32i >= 1000)
        {
            u32i = 0;
            u32count++;
            //Sleep(100);
        }
    }

    if(u32count >= 1000)
    {
#ifdef ZEBU_ESL
        uart_puts("C I A E r r o r !", 18);
        uart_putc('\n');
#else
        printk("    check_dma_int_deassert() Error !\n");
        return -1;
#endif
    }
    return 0;
}

    static int _mdrv_RC_ib_memory_init(void *pD, int rpBarNum, CPDI_RpBarControl control_val, CPDI_RpBarApertureSize aperture_val, CPDI_EnableOrDisable check_enable, uint8_t numPassedBits);
           int _mdrv_RC_ob_memory_init(void *pD, CPDI_BarControl control_val, CPDI_BarApertureSize aperture_val, CPDI_EnableOrDisable check_enable);
/*
 * ============================================================================
 * Function Implementations
 * ============================================================================
 */
uint32_t _mdrv_RC_MIU_translation(uint32_t input_addr)
{
  uint32_t output_addr;

  if(input_addr <= MIU_BASE)
    return 0;

  output_addr = (input_addr - MIU_BASE)>>4;

  return output_addr;
}

uint32_t _mdrv_RC_get_device_bar_info(void *pD, uint32_t bus , uint32_t dev , uint32_t func, uint32_t bar, uint32_t *bar_address, uint32_t *bar_aperture_size)
{
    CPDI_OBJ * obj = CPDI_GetInstance();
    uint32_t reg = 0x10 + (bar << 2);
    uint32_t v, io, memtype, sz, r, v2, sz2;

    obj->doConfigRead (pD, bus, dev, func, reg, CPDI_4_BYTE, (uint32_t *) & v);

    *bar_address = 0xfffffff0 & v;
    //*bar_address = CPDREF_RP_SRAM_LOCAL | (v&0xf); //CPDREF_RP_SRAM_LOCAL_OB

    io = v & 1;
    memtype = (v >> 1) & 3;

      if(io) {
        return EINVAL;
    } else {
        switch(memtype) {
            case 0:
                /**
                 * get size
                 */
                obj->doConfigWrite (pD, bus, dev, func, reg, CPDI_4_BYTE, 0xfffffff0 | v);
                obj->doConfigRead  (pD, bus, dev, func, reg, CPDI_4_BYTE,(uint32_t *) & r);

                if(r & 0xfffffff0)
                     sz = ~(r & ~0xf) + 1;
                else sz = 0;

                *bar_aperture_size = sz;
                /**
                 * restore BAR
                 */
                //obj->doConfigWrite (pD, bus, dev, func, reg, CPDI_4_BYTE, v);
                obj->doConfigWrite (pD, bus, dev, func, reg, CPDI_4_BYTE, *bar_address);

#if(0)
                if(sz) {
                    /**
                     * map it and set BAR
                     */
                    unsigned tmp = 0x10 + (bar << 2);

                    obj->doConfigWrite (pD, bus, dev, func, tmp  , CPDI_4_BYTE, addr_lo | (v & 0xf));

                }
#endif
                break;

           case 2:
                /**
                 * get size
                 */

                CPDI_DoConfigRead  (pD, bus, dev, func, reg+4, CPDI_4_BYTE,(uint32_t *) & v2);
                CPDI_DoConfigWrite (pD, bus, dev, func, reg  , CPDI_4_BYTE, 0xfffffff0 | v);
                CPDI_DoConfigWrite (pD, bus, dev, func, reg+4, CPDI_4_BYTE, 0xffffffff);
                CPDI_DoConfigRead  (pD, bus, dev, func, reg  , CPDI_4_BYTE,(uint32_t *) & r);
                sz = ~(r & ~0xf) + 1;
                CPDI_DoConfigRead  (pD, bus, dev, func, reg+4, CPDI_4_BYTE,(uint32_t *) & r);

                if(r)
                     sz2 = ~r;
                else sz2 =  0;

                *bar_aperture_size = sz;
                /**
                 * restore BARs
                 */
                //CPDI_DoConfigWrite (pD, bus, dev, func, reg  , CPDI_4_BYTE, v );
                CPDI_DoConfigWrite (pD, bus, dev, func, reg  , CPDI_4_BYTE, *bar_address);
                CPDI_DoConfigWrite (pD, bus, dev, func, reg+4, CPDI_4_BYTE, v2);

#if(0)
                /**
                 * map it
                 */
                if(!sz2 && (sz < 0x00400000)) {

                    /**
                     * map it as 32-bit
                     */
                    unsigned tmp = 0x10 + (bar << 2);


                    CPDI_DoConfigWrite (pD, bus, dev, func, tmp  , CPDI_4_BYTE, addr_lo | (v & 0xf));
                    CPDI_DoConfigWrite (pD, bus, dev, func, reg+4, CPDI_4_BYTE, 0);

                } else {
                    /**
                     * map it as full 64-bit
                     */
                    CPDI_DoConfigWrite (pD, bus, dev, func, reg+4, CPDI_4_BYTE, base64);
                    CPDI_DoConfigWrite (pD, bus, dev, func, reg  , CPDI_4_BYTE, v     );
                }
#endif
                break;
            default:
                /**
                 * undefined memory type from device
                 */
                return EINVAL;
        }
        /**
         * Enable memory access
         */
        obj->doConfigRead (pD, bus, dev, func, PCI_RC_COMMAND, CPDI_4_BYTE, (uint32_t *) & v);
		v |= 0x06; // command =0x0006, Bus master, memory
        obj->doConfigWrite (pD, bus, dev, func, PCI_RC_COMMAND, CPDI_4_BYTE, v);
    }

    return 0;
}

/*-------------------------------------------------------------------------------*/
/*  Interrupt Handlers to be hooked up to LOCAL_INTERRUPT, or can be polled at    */
/*  regular intervals to allow interrupts to be simulated                        */
/*-------------------------------------------------------------------------------*/
static void CPDREF_LocalInterruptHandler_rp (uint32_t et) {

    CPDI_OBJ * obj = CPDI_GetInstance();

    /* Call the driver's interrupt handler routine.  If a callback was           */
    /* registered at init() then it will be called by the driver interrupt       */
    /* handler if there are any interrupts to report.  If there are no interrupts*/
    /* active, the isr() function will return without doing anything             */
    obj->isr((void *)&g_rc_data);
}

/*-------------------------------------------------------------------------------*/
/*  Called by core driver if local interrupt condition  reported in              */
/*   local_error_status_register)                                                */
/*-------------------------------------------------------------------------------*/
static void CPDREF_CallbackHandler_rp (uint32_t et) {

#ifndef ZEBU_ESL
       uint16_t tmp16_1, tmp16_2;
       //uint16_t i;
       uint32_t ret_val = 0;

       printk ("     Error: PCIe CallbackHandler Called with error status 0x%x \n",et);

       tmp16_1 = Read16(REG_PCIE_BRI_STATUS_0);
       tmp16_2 = Read16(REG_PCIE_BRI_STATUS_1);
       printk ("     CPDREF_CallbackHandler_rp(): REG_PCIE_BRI_STATUS_0 = 0x%04x , REG_PCIE_BRI_STATUS_1 = 0x%04x \n", tmp16_1, tmp16_2);
/*
       for(i = 0; i<4; i++)
       {
           tmp16_1 = Read16(MSTAR_PCIE_U3PHY_P1_DTOP_BASE+0x20*4 +i*8);
           tmp16_2 = Read16(MSTAR_PCIE_U3PHY_P1_DTOP_BASE+0x21*4 +i*8);
           printk ("     MSTAR_PCIE_U3PHY_P1_DTOP_BASE Addr:0x%x = 0x%04x , Addr:0x%x = 0x%04x \n",0x20+i*2, tmp16_1, 0x21+i*2, tmp16_2);

       }
*/

       if(et & CPDI_PNP_RX_FIFO_PARITY_ERROR)
       {
#ifdef DEBUG_RUNTIME_LOCAL_ERROR
             localerror_count.PNP_RX_FIFO_PARITY_ERROR_Count++;
#endif
       }

       if(et & CPDI_COMPLETION_RX_FIFO_PARITY_ERROR)
       {
#ifdef DEBUG_RUNTIME_LOCAL_ERROR
             localerror_count.COMPLETION_RX_FIFO_PARITY_ERROR_Count++;
#endif
       }

       if(et & CPDI_REPLAY_RAM_PARITY_ERROR)
       {
#ifdef DEBUG_RUNTIME_LOCAL_ERROR
             localerror_count.REPLAY_RAM_PARITY_ERROR_Count++;
#endif
       }

       if(et & CPDI_PNP_RX_FIFO_OVERFLOW_CONDITION)
       {
#ifdef DEBUG_RUNTIME_LOCAL_ERROR
             localerror_count.PNP_RX_FIFO_OVERFLOW_CONDITION_Count++;
#endif
       }

       if(et & CPDI_COMPLETION_RX_FIFO_OVERFLOW_CONDITION)
       {
#ifdef DEBUG_RUNTIME_LOCAL_ERROR
             localerror_count.COMPLETION_RX_FIFO_OVERFLOW_CONDITION_Count++;
#endif
       }

       if(et & CPDI_REPLAY_TIMEOUT_CONDITION)
       {
#ifdef DEBUG_RUNTIME_LOCAL_ERROR
             localerror_count.REPLAY_TIMEOUT_CONDITION_Count++;
#endif
       }

       if(et & CPDI_REPLAY_TIMEOUT_ROLLOVER_CONDITION)
       {
#ifdef DEBUG_RUNTIME_LOCAL_ERROR
             localerror_count.REPLAY_TIMEOUT_ROLLOVER_CONDITION_Count++;
#endif
       }

       if(et & CPDI_PHY_ERROR)
       {
#ifdef DEBUG_RUNTIME_LOCAL_ERROR
             localerror_count.PHY_ERROR_Count++;
#endif
       }

       if(et & CPDI_MALFORMED_TLP_RECEIVED)
       {
#ifdef DEBUG_RUNTIME_LOCAL_ERROR
             localerror_count.MALFORMED_TLP_RECEIVED_Count++;
#endif
       }

       if(et & CPDI_UNEXPECTED_COMPLETION_RECEIVED)
       {
#ifdef DEBUG_RUNTIME_LOCAL_ERROR
             localerror_count.UNEXPECTED_COMPLETION_RECEIVED_Count++;
#endif
       }

       if(et & CPDI_FLOW_CONTROL_ERROR)
       {
#ifdef DEBUG_RUNTIME_LOCAL_ERROR
             localerror_count.FLOW_CONTROL_ERROR_Count++;
#endif
       }

       if(et & CPDI_COMPLETION_TIMEOUT_CONDITION)
       {
#ifdef DEBUG_RUNTIME_LOCAL_ERROR
             localerror_count.COMPLETION_TIMEOUT_CONDITION_Count++;
             ret_val = _mdrv_pcie_reset_link();
#endif
       }


    CPDI_ResetLocalErrorStatusCondition((void *) &g_rc_data, et);  //for reset the local error status
#else

    uart_puts("C P D _ C a l l b a c k :", 26);   uart_putc('\n');
#endif
}


/*-------------------------------------------------------------------------------*/
/* Probe for and setup the RP core driver                                        */
/*-------------------------------------------------------------------------------*/
uint32_t  CPDREF_ProbeAndInitRPDriver (void *pD,uint32_t pd_size) {

    uint32_t ret_val = 0;
    uint8_t bitsInRegion0Address;

    CPDI_InitParam initParams;
    CPDI_SysReq     sysReq;

    CPDI_OBJ * obj = CPDI_GetInstance();

#ifdef ZEBU_ESL
    /*
        Reset disabled of PCIe RC
        Enable RC ip-sapce mem-space bus_master so that the core will enter D0 active
    */
    Write16(REG_RC_RST_CTRL, 0x0);
    Write16(REG_LINK_TRAINING_ENABLE, 0x1);
    Write32(CPDREF_RP_BASE+0x04, 0x7);

    //****enable ASPM L0s. and retrain link
    //Write32(CPDREF_RP_BASE+0xd0, 0x00000021);

    uart_putc('P');   uart_putc('\n');

    Zebu_memset ((unsigned long long) &(initParams), 0, sizeof (CPDI_InitParam));
#else
    memset((void *) & initParams, 0, sizeof (CPDI_InitParam));
#endif


    /*--- call core driver API - probe for RP --*/
#ifdef ZEBU_ESL
    ret_val = CPDI_Probe (CPDREF_RP_BASE, & sysReq);
#else
    ret_val = obj->probe (CPDREF_RP_BASE, & sysReq);
#endif
    //if (ret_val || sysReq.pdataSize > pd_size) {
        //printk (">> Probe failed for RP %x \n", ret_val);
    //    uart_puts("P r o b e !",12); uart_putc('\n');
    //    return ret_val;
    //}

    /*-- initialize the callbacks for errors --*/
    initParams.initErrorCallback = & CPDREF_CallbackHandler_rp;

    /* If an interrupt handler has not been installed, mask all interrupts.  These will still
     * be reported in the local_error_status_register, which will be polled on each call to the
     * driver API.  */
    initParams.initMaskableConditions.initparamMaskPnpRxFifoParityError.newValue = CPDI_MASK_PARAM;
    initParams.initMaskableConditions.initparamMaskPnpRxFifoParityError.changeDefault = 1;
    initParams.initMaskableConditions.initparamMaskCompletionRxFifoParityError.newValue = CPDI_MASK_PARAM;
    initParams.initMaskableConditions.initparamMaskCompletionRxFifoParityError.changeDefault = 1;
    initParams.initMaskableConditions.initparamMaskReplayRamParityError.newValue = CPDI_MASK_PARAM;
    initParams.initMaskableConditions.initparamMaskReplayRamParityError.changeDefault = 1;
    initParams.initMaskableConditions.initparamMaskPnpRxFifoOverflow.newValue = CPDI_MASK_PARAM;
    initParams.initMaskableConditions.initparamMaskPnpRxFifoOverflow.changeDefault = 1;
    initParams.initMaskableConditions.initparamMaskCompletionRxFifoOverflow.newValue = CPDI_MASK_PARAM;
    initParams.initMaskableConditions.initparamMaskCompletionRxFifoOverflow.changeDefault = 1;
    initParams.initMaskableConditions.initparamMaskReplayTimeout.newValue = CPDI_MASK_PARAM;
    initParams.initMaskableConditions.initparamMaskReplayTimeout.changeDefault = 1;
    initParams.initMaskableConditions.initparamMaskReplayTimerRollover.newValue = CPDI_MASK_PARAM;
    initParams.initMaskableConditions.initparamMaskReplayTimerRollover.changeDefault = 1;
    initParams.initMaskableConditions.initparamMaskPhyError.newValue = CPDI_MASK_PARAM;
    initParams.initMaskableConditions.initparamMaskPhyError.changeDefault = 1;
    initParams.initMaskableConditions.initparamMaskMalformedTlpReceived.newValue = CPDI_MASK_PARAM;
    initParams.initMaskableConditions.initparamMaskMalformedTlpReceived.changeDefault = 1;
    initParams.initMaskableConditions.initparamMaskUnexpectedCompletionReceived.newValue = CPDI_MASK_PARAM;
    initParams.initMaskableConditions.initparamMaskUnexpectedCompletionReceived.changeDefault = 1;
    initParams.initMaskableConditions.initparamMaskFlowControlError.newValue = CPDI_MASK_PARAM;
    initParams.initMaskableConditions.initparamMaskFlowControlError.changeDefault = 1;
    initParams.initMaskableConditions.initparamMaskCompletionTimeout.newValue = CPDI_MASK_PARAM;
    initParams.initMaskableConditions.initparamMaskCompletionTimeout.changeDefault = 1;





    /* Set the base address of AXI Region 0, for use in ECAM config read/writes   */
    initParams.initBaseAddresses.initparamAxBase.newValue = 0x00000000; //CPDI_DEFAULT_ECAM_BASE_BUS_ADDRESS ; Only 28~31bit can use
    initParams.initBaseAddresses.initparamAxBase.changeDefault = 1;

    initParams.initBaseAddresses.initparamLmBase.newValue = (CPDREF_RP_BASE + CPDI_REGS_OFFSET_LOCAL_MANAGEMENT);  //Bank 0x1D10
    initParams.initBaseAddresses.initparamLmBase.changeDefault = 0;

    initParams.initBaseAddresses.initparamAwBase.newValue = (CPDREF_RP_BASE + CPDI_REG_OFFSET_AXI_WRAPPER);        //Bank 0x1D40
    initParams.initBaseAddresses.initparamAwBase.changeDefault = 0;

    /* The vendor and subsystem vendor ID can be configured in RTL after            */
    /* delivery (see Cadence IP Core User Guide).  The following configuration      */
    /* is only necessary if you wish to change these at run time                    */

    initParams.initIdParams.initparamVendorId.newValue = CADENCE_VENDOR_ID;
    initParams.initIdParams.initparamVendorId.changeDefault = 1;
    initParams.initIdParams.initparamSubsystemVendorId.newValue = CADENCE_VENDOR_ID;
    initParams.initIdParams.initparamSubsystemVendorId.changeDefault = 1;

#ifdef ZEBU_ESL
    uart_puts("I n i t   S t a r t !", 22); uart_putc('\n');
    /*--- call core driver API - init for RP --*/
    ret_val = CPDI_Init (pD, CPDREF_RP_BASE, & initParams);
    uart_puts("I n i t   D o n e !",20); uart_putc('\n');
#else
    ret_val = obj->init (pD, CPDREF_RP_BASE, & initParams);
#endif


    /* Work out how many bits are needed to address all of region 0 */
    bitsInRegion0Address =1;
    while ( (2ULL << bitsInRegion0Address) < CPDI_AXI_REGION_0_SIZE) {
        bitsInRegion0Address++;
    }

    /* Set up AXI Region 0 for PCIE config space.                                   */
    /* This is required before using doConfigRead(), doConfigWrite() or             */
    /* enableBarAccess()
    */

#ifdef ZEBU_ESL
    uart_puts("O b W r a p p e r C o n f i g",30); uart_putc('\n');

    if(!(CPDI_SetupObWrapperConfigAccess (pD, 0, bitsInRegion0Address, 0)))
    uart_puts("O b W r a p p e r C o n f i g   D o n e",40); uart_putc('\n');
#else
    if(!(obj->setupObWrapperConfigAccess(pD, 0, bitsInRegion0Address, 0)))
      printk("     OB Configure Region Init Done\n");
#endif

#ifdef CONFIG_MSTAR_PCIE_OB_MSG_ENABLE
#ifdef ZEBU_ESL
    uart_puts("O b   M S G   R e g i o n   I n i t",36); uart_putc('\n');
    if(!(CPDI_SetupObWrapperMessageAccess (pD, MSTAR_PCIE_OB_MSG_REG_START, 0, 0)))
    uart_puts("O b   M S G   R e g i o n   I n i t   D o n e",46); uart_putc('\n');
#else
    if(!(obj->setupObWrapperMessageAccess(pD, MSTAR_PCIE_OB_MSG_REG_START, 0, 0)))
      printk("     OB MSG Region Init Done\n");
#endif
#endif

#ifdef ZEBU_ESL
    //TYPE 0  is non prefetchable, TYPE 1 is prefetchable
    _mdrv_RC_ib_memory_init(pD, 0, CPDI_RP_TYPE_0_32_BIT_MEM_BAR, CPDI_RP_APERTURE_SIZE_16M, CPDI_ENABLE_PARAM, CPDI_RP_IB_APERTURE_SIZE);
     uart_puts("R C I B   m e m o r y   i n i t   D o n e",42); uart_putc('\n');
#else
    //CPDREF_PCIE_AXI_PHYS_BASE_EP is equal to CPDI_RP_APERTURE_SIZE_1M size
    if(!(_mdrv_RC_ib_memory_init(pD, 0, CPDI_RP_TYPE_0_32_BIT_MEM_BAR, CPDI_RP_APERTURE_SIZE_1M, CPDI_DISABLE_PARAM, CPDI_RP_IB_APERTURE_SIZE)))
      printk("     RC Inbound memory Init Done\n");
#endif


/*
#ifdef ZEBU_ESL
    uart_puts("O b   M E M   C o n f i g   I n i t",36); uart_putc('\n');
    _mdrv_RC_ob_memory_init(pD, CPDI_NON_PREFETCHABLE_32_BIT_MEM_BAR, CPDI_APERTURE_SIZE_1M, CPDI_ENABLE_PARAM);
    uart_puts("O b   M E M   C o n f i g   I n i t   D o n e",46); uart_putc('\n');
#else
    //because device boot up too slowly, so , we call ob memory init later
    if(!(_mdrv_RC_ob_memory_init(pD, CPDI_NON_PREFETCHABLE_32_BIT_MEM_BAR, CPDI_APERTURE_SIZE_1M, CPDI_ENABLE_PARAM)))
      printk("     RC Outbound memory Init Done\n");
#endif
*/
    if(_mdrv_RC_ob_memio_init(pD, MSTAR_PCIE_OB_MEMIO_REG1_BASE, MSTAR_PCIE_OB_MEMIO_REG_START, MSTAR_PCIE_OB_MEMIO_REGS) == 0)
      pr_info("     RC Outbound memory Init Done\n");
    else
      pr_info("     RC Outbound memory Init Fail !\n");

    return ret_val;
}

// cpd_lnx_drv.c  cpdLnx_Probe should call inbound DMA
static int _mdrv_RC_ib_memory_init(void *pD, int rpBarNum, CPDI_RpBarControl   control_val, CPDI_RpBarApertureSize aperture_val, CPDI_EnableOrDisable   check_enable, uint8_t numPassedBits)
{
    //void __iomem * pcieAddr = (void __iomem *) (CPED_PCIE_REGS_VIRT_BASE + PCI_RC_BAR_0);
#ifndef ZEBU_ESL
    CPDI_OBJ * obj = CPDI_GetInstance();
#endif
    uint32_t  retVal  = 0;
    uint8_t  regionNo = 0;
    CPDI_RpBarApertureSize aperture_b1val = CPDI_RP_APERTURE_SIZE_1M;
//#ifdef CONFIG_MSTAR_PCIE_HW_VAPA_ADDRESS_TRANSLATION
//    uint16_t tmp      = 0;
//    numPassedBits     = 31; // Enable 4GB memory mapping
//#endif


    //uint32_t addr_data = 0xFFFFFFFF;

#ifdef ZEBU_ESL
    uint32_t tmp;
    retVal = CPDI_AccessRootPortBarApertureSetting (
#else
    retVal = obj->accessRootPortBarApertureSetting (
#endif
             pD,
             rpBarNum,
             CPDI_DO_WRITE,
             &aperture_val);
    if (retVal)
    {
#ifdef ZEBU_ESL
          uart_puts("E r r o r ! : 1",16); uart_putc('\n');
#else
          printk("_mdrv_RC_ib_memory_init() Eror: accessRootPortBar0ApertureSetting() \n");
#endif
        return retVal;
    }

#ifdef ZEBU_ESL
    uint32_t tmp;
    retVal = CPDI_AccessRootPortBarApertureSetting (
#else
    retVal = obj->accessRootPortBarApertureSetting (
#endif
             pD,
             1,
             CPDI_DO_WRITE,
             &aperture_b1val);
    if (retVal)
    {
#ifdef ZEBU_ESL
          uart_puts("E r r o r ! : 1",16); uart_putc('\n');
#else
          printk("_mdrv_RC_ib_memory_init() Eror: accessRootPortBar1ApertureSetting() \n");
#endif
        return retVal;
    }

#ifdef ZEBU_ESL
    retVal = CPDI_AccessRootPortBarControlSetting (
#else
    retVal = obj->accessRootPortBarControlSetting (
#endif
             pD,
             rpBarNum,
             CPDI_DO_WRITE,
             &control_val);
    if (retVal)
    {
#ifdef ZEBU_ESL
          uart_puts("E r r o r ! : 2",16); uart_putc('\n');
#else
          printk("_mdrv_RC_ib_memory_init() Eror: accessRootPortBarControlSetting() \n");
#endif
        return retVal;
    }

    // DMA Security must set
#ifdef ZEBU_ESL
    retVal = CPDI_ControlRootPortBarCheck (pD, check_enable);
#else
    retVal = obj->controlRootPortBarCheck (pD, check_enable);
#endif
    if (retVal)
    {
#ifdef ZEBU_ESL
          uart_puts("E r r o r ! : 3",16); uart_putc('\n');
#else
          printk("_mdrv_RC_ib_memory_init() Eror: controlRootPortBarCheck() \n");
#endif
        return retVal;
    }

#ifdef ZEBU_ESL
    retVal = CPDI_EnableRpMemBarAccess (
#else
    retVal = obj->enableRpMemBarAccess (
#endif
             pD,
             0 ,    //bar 0
             0 ,    //addr_hi
             CPDREF_PCIE_AXI_PHYS_BASE_EP /*+ CPDI_AXI_REGION_0_SIZE*/ ); //addr_lo
    if (retVal)
    {
#ifdef ZEBU_ESL
         uart_puts("E r r o r ! : 4",16); uart_putc('\n');
#else
         printk("_mdrv_RC_ib_memory_init() Eror: enableRpMemBarAccess() \n");
#endif
        return retVal;
    }

     /* Set inbound address for RP region 0 and MIU section Ctrl must set to MIU0 or MIU1*/
#ifdef ZEBU_ESL
    retVal = CPDI_SetupIbRootPortAddrTranslation((void *)&g_rc_data, regionNo, numPassedBits, 0, CPDREF_RP_SRAM_LOCAL>>8);
#else
    retVal = obj->setupIbRootPortAddrTranslation((void *)&g_rc_data, regionNo, numPassedBits, 0, (CPDREF_RP_SRAM_LOCAL-MIU_BASE)>>8);
#endif
    if (retVal)
    {
#ifdef ZEBU_ESL
          uart_puts("E r r o r ! : 5",16); uart_putc('\n');
#else
          printk("_mdrv_RC_ib_memory_init() Eror: setupIbRootPortAddrTranslation() \n");
#endif
        return retVal;
    }

//#ifdef CONFIG_MSTAR_PCIE_HW_VAPA_ADDRESS_TRANSLATION
//    tmp = Read16(REG_BRI_DUMMY_REG1);
//    tmp = tmp | ((MIU_BASE>>28)<<REG_U03_VAPA_ADDRESS_OFFSET_VALUE) | REG_U03_VAPA_ADDRESS_TRANSLATION;
//    Write16(REG_BRI_DUMMY_REG1 , tmp);  //Enable VAPA Address Translation
//#endif

    Write32(CPDREF_RP_BASE+0x04, 0x7);  //Enable RC Bus-Master , IO-Space , Mem-Space

#ifdef ZEBU_ESL
    //Enable INTx ;  this is useless
    //Write32(CPDREF_RP_BASE+0x12C, 0xFFFFFFFF);

    //Disable INT0 & INT1 MASK
    Write16(REG_INT_MSK0, 0x0000);
    Write16(REG_INT_MSK1, 0x0000);

    //Disable INT mask for PCIe RC in system interrupt controller
    Write16(0x1F202154, 0xFDFF);

    //Set the MAX 256 Payload Size
    tmp = Read32(CPDREF_RP_BASE+0xC8); //bit[5] = 1 means 256 bytes
    Write32(CPDREF_RP_BASE+0xC8, tmp | 0x20);
    //Read16(CPDREF_RP_BASE+0xC8);
#endif

    return 0;
}


int _mdrv_RC_ob_mem_init_resume(void)
{
	_mdrv_RC_ob_memio_init((void *)&g_rc_data,
		MSTAR_PCIE_OB_MEMIO_REG1_BASE,
		MSTAR_PCIE_OB_MEMIO_REG_START,
		MSTAR_PCIE_OB_MEMIO_REGS);
	return 0;
}

#if 0
int _mdrv_RC_ob_memory_init(void *pD, CPDI_BarControl   control_val, CPDI_BarApertureSize aperture_val, CPDI_EnableOrDisable   check_enable)
{
    //void __iomem * pcieAddr = (void __iomem *) (CPED_PCIE_REGS_VIRT_BASE + PCI_RC_BAR_0);
    CPDI_OBJ * obj = CPDI_GetInstance();
    uint32_t  retVal = 0, bar_address, bar_aperture_size;
#ifdef MULTI_BAR_SUPPORT
	uint32_t	tmp_bar_address, tmp_bar_aperture_size;
#endif
    uint8_t   bitsInRegion1Address = 0;  //Set Region 1 for OutBound Config Registers
    CPDI_BarNumber	BarNumber;

#if(1)
    /* Work out how many bits are needed to address all of region 1 */
    bitsInRegion1Address =1;
    while ( (2ULL << bitsInRegion1Address) < CPDI_AXI_REGION_1_SIZE) {
        bitsInRegion1Address++;
    }
#endif
     /* Write the base addresses into BAR0. */
     //Mark enableBarAccess(); Because when Device boot up, it will change it back to original value
/*
    retVal = obj->enableBarAccess(
             pD,
             0 ,    //bus
             0 ,    //device
             0 ,    //function
             0 ,    //bar 0
             0); //addr_lo,


    if (retVal)
    {
#ifdef ZEBU_ESL
          uart_puts("E r r o r ! : 1",16); uart_putc('\n');
#else
          printk("_mdrv_RC_ob_memory_init() Eror: enableBarAccess() \n");
#endif
        return retVal;
    }
*/

	//
	// BAR address scan, only the 1st availabe BAR will be adopted,1 BAR only.
	//
	BarNumber=CPDI_BAR_0;
#ifdef MULTI_BAR_SUPPORT
	bar_address=0;
	bar_aperture_size=0;
#endif
	do {
	    retVal = _mdrv_RC_get_device_bar_info(pD,
#ifdef BUS0_RC_ENABLE
	                                           1,  //bus
#else
	                                           0,  //bus
#endif
	                                           0,  //device
	                                           0,  //function
	                                           BarNumber,  //bar
#ifndef MULTI_BAR_SUPPORT
	                                           &bar_address,
	                                           &bar_aperture_size
#else
	                                           &tmp_bar_address,
	                                           &tmp_bar_aperture_size
#endif
	                                           );

#ifndef MULTI_BAR_SUPPORT
		if (retVal==0 && bar_address!=0 && bar_aperture_size!=0)
			break;
#else

		//
		// support multi BAR address within a device's function, BAR size has 1MB limitation
		//
		if (bar_aperture_size==0) {
			// case 1 BAR, LEN is not assigned
			//pr_info(" case 1:\n");
			bar_address = tmp_bar_address;
			bar_aperture_size = tmp_bar_aperture_size;
			if (bar_aperture_size > (1024*1024))	{
				retVal = EINVAL;
				pr_err("pcie OB memory out of range!\n");
				break;
			}

		} else {
			// 0 BAR,Len : keep on checking next BAR
			if (retVal==0 && tmp_bar_address==0 && tmp_bar_aperture_size==0)
				continue;

			// case 2 next BAR+LEN >= (1st BAR+LEN)
			if (tmp_bar_address >= (bar_address+bar_aperture_size)) {
				//pr_info(" case 2:\n");
				bar_aperture_size = (tmp_bar_address+tmp_bar_aperture_size)-bar_address;
				if (bar_aperture_size > (1024*1024)) {
					retVal = EINVAL;
					pr_err("pcie OB memory out of range!\n");
					break;
				}
			}
			else
			// case 3 next BAR+LEN <= 1st BAR
			if ((tmp_bar_address+tmp_bar_aperture_size) <= bar_address) {
				//pr_info(" case 3:\n");
				bar_address = tmp_bar_address;
				bar_aperture_size = (bar_address+bar_aperture_size) - tmp_bar_address;
				if (bar_aperture_size > (1024*1024)) {
					retVal = EINVAL;
					pr_err("pcie OB memory out of range!\n");
					break;
				}
			}
		}
#endif

	} while (BarNumber++ <= CPDI_BAR_5);

#ifdef MULTI_BAR_SUPPORT
	// if no BAR, len available, return fail
	if (bar_address==0 && bar_aperture_size==0)
		retVal = EINVAL;
#endif

    if(retVal)
    {
        printk("     _mdrv_RC_ob_memory_init() Error: _mdrv_RC_get_bar_info() \n");
        return retVal;
    }
    printk("     _mdrv_RC_ob_memory_init(): bar_address = 0x%08x, bar_aperture_size = 0x%x \n", bar_address, bar_aperture_size);

    if(bar_aperture_size == 0 || bar_address ==0)
      return -1;

    /*
    while((bar_aperture_size&1)==0)
    {
        bitsInRegion1Address++;
        bar_aperture_size >>=1;
    }
    */
     // for intel aperture_size

     /* Set outbound address translation for RP region 1 */
#ifdef ZEBU_ESL
    retVal = CPDI_SetupObWrapperMemIoAccess((void *)&g_rc_data, CPDI_RP_OB_REGION_NUMBER, bitsInRegion1Address, 0, (CPDREF_RP_SRAM_LOCAL_OB>>8));
#else
    if(bitsInRegion1Address > 11 && bar_address != 0x0)
    {
        retVal = obj->setupObWrapperMemIoAccess((void *)&g_rc_data, CPDI_RP_OB_REGION_NUMBER, (bitsInRegion1Address) /*CPDI_RP_OB_APERTURE_SIZE*/, 0, (bar_address>>8));
#endif
        if (retVal)
        {
#ifdef ZEBU_ESL
              uart_puts("E r r o r ! : 1",16); uart_putc('\n');
#else
              printk("_mdrv_RC_ob_memory_init() Eror: setupObWrapperMemIoAccess() \n");
#endif
            return retVal;
        }
        g_bitsInRegion1Address = bitsInRegion1Address;
        g_bar_address = bar_address;
    }
    return 0;
}
#endif

int _mdrv_RC_ob_memio_init(void *pD, uint32_t iomem_base, int region_sta, int region_count)
{
	CPDI_OBJ * obj = CPDI_GetInstance();
	uint8_t   bitsInRegion1Address = 0, ii;
	uint32_t  retVal = 0, iomem_addr;

	/* Work out how many bits are needed to address all of region 1 */
	bitsInRegion1Address =1;
	while ( (2ULL << bitsInRegion1Address) < CPDI_AXI_REGION_1_SIZE) {
		bitsInRegion1Address++;
	}

	for (ii = 0; ii < region_count; ii++)
	{
		iomem_addr = iomem_base + CPDI_AXI_REGION_1_SIZE * ii;

		retVal = obj->setupObWrapperMemIoAccess(pD,
			region_sta+ii,
			bitsInRegion1Address,
			0,
			iomem_addr >> 8 );

		printk("[PCIE] setup ob memio region %d to addr 0x%x\n", region_sta+ii, iomem_addr);

		if (retVal)
		{
			printk("[PCIE] setup ob memio failed\n");
			return retVal;
		}
	}

	printk("RC Outbound memory Init... done\n");

	return 0;
}

uint32_t _mdrv_RC_ob_config_read(void *pD, uint32_t addr, uint32_t *data)
{
    uint32_t ret = 0;
    uint32_t alignment_4bytes = 0;

    alignment_4bytes = addr & 0x03;
    if(alignment_4bytes != 0)
    {
        printk("     _mdrv_RC_ob_config_read() Error: Addrress isn't aligned => 0x%08x !\n", addr);
        return -1;
    }

    ret = check_interrupt_deassert();
    if(ret !=0)
    {
        printk("     _mdrv_RC_ob_config_read() Error: Addrress => 0x%08x !\n", addr);
        return -1;
    }
    Write16( REG_PCIE_CM_ADDR ,(unsigned short) (addr & 0x0000FFFF));  // reg_pcie_cm_addr
    Write16( REG_PCIE_CM_ADDR+4 , (unsigned short) ((addr & 0xFFFF0000) >> 16));  // reg_pcie_cm_addr
    Write16( REG_PCIE_CM_DIRECTION, 0x0020);  //REG_PCIE_CM_DIRECTION=0,  REG_PCIE_CM_DONE_INT_EN=1, REG_PCIE_CM_AXI_ID = 0 ;
    Write16( REG_PCIE_CM_FIRE, 0x1 & REG_PCIE_CM_FIRE_MASK);

    // polling or Interrupt event of REG_PCIE_CM_DONE_INT
    ret = check_interrupt_assert();
    if(ret !=0)
    {
        printk("     _mdrv_RC_ob_config_read() Error: Addrress => 0x%08x !\n", addr);
        ret = _mdrv_check_LinkTrainingComplete();
        if(ret != 0)
        {
            printk("     _mdrv_check_LinkTrainingComplete() Error !\n");
        }

        return -1;
    }

    *data = Read16(REG_PCIE_CM_RDATA) + (Read16(REG_PCIE_CM_RDATA+4) << 16);

    //if(addr == 0x09300000)
    //  *data = 0x0;

    return 0;
}

uint32_t _mdrv_RC_ob_config_write(void *pD, uint32_t addr, uint32_t data)
{
    uint32_t ret = 0;
    uint32_t alignment_4bytes = 0;

    alignment_4bytes = addr & 0x03;
    if(alignment_4bytes != 0)
    {
        printk("     _mdrv_RC_ob_config_write() Error: Addrress isn't aligned => 0x%08x !\n", addr);
        return -1;
    }

    ret = check_interrupt_deassert();
    if(ret !=0)
    {
        printk("     _mdrv_RC_ob_config_write() Error: Addrress => 0x%08x !\n", addr);
        return -1;
    }

    Write16( REG_PCIE_CM_ADDR , (unsigned short) (addr & 0x0000FFFF));  // reg_pcie_cm_addr
    Write16( REG_PCIE_CM_ADDR+4 , (unsigned short) ((addr & 0xFFFF0000) >> 16));
    Write16( REG_PCIE_CM_WDATA ,(unsigned short) (data & 0x0000FFFF));  // reg_pcie_cm_wdata
    Write16( REG_PCIE_CM_WDATA+4 ,(unsigned short) ((data & 0xFFFF0000) >> 16));
    Write16( REG_PCIE_CM_DIRECTION, 0x003F);  // REG_PCIE_CM_WSTRB = 0xF, REG_PCIE_CM_DIRECTION=1,  REG_PCIE_CM_DONE_INT_EN=1, REG_PCIE_CM_AXI_ID = 0
    Write16( REG_PCIE_CM_FIRE, 0x1);

    // polling or Interrupt event of REG_PCIE_CM_DONE_INT
    ret = check_interrupt_assert();
    if(ret !=0)
    {
        printk("     _mdrv_RC_ob_config_write() Error: Addrress => 0x%08x , data => 0x%08x\n \n", addr, data);
        return -1;
    }

    return 0;
}
#ifdef CONFIG_MSTAR_PCIE_OB_MEMIO_ENABLE
uint32_t _mdrv_RC_ob_memio_read(void *pD, uint32_t offset, uint32_t *data)
{
	uint32_t	region_num, addr, sizemask, sizebits;

	//For compatible with previous interface, use bit31 for enable memory IO and offset is not full address
	if ( offset >= 0x80000000 && offset < (0x80000000 + CPDI_AXI_REGION_1_SIZE) )
		offset = (offset & 0x7FFFFFFF) + MSTAR_PCIE_OB_MEMIO_REG1_BASE;

	if ( (offset < MSTAR_PCIE_OB_MEMIO_REG1_BASE) ||
		(offset >= MSTAR_PCIE_OB_MEMIO_REG1_BASE + MSTAR_PCIE_OB_MEMIO_REGS * CPDI_AXI_REGION_1_SIZE) )
	{
		printk("[PCIE] ob mmio with illegal offset 0x%08x !\n", offset);
		return -1;
	}

	sizebits = ffs(CPDI_AXI_REGION_1_SIZE) - 1;
	sizemask = CPDI_AXI_REGION_1_SIZE - 1;

	region_num = ((offset - MSTAR_PCIE_OB_MEMIO_REG1_BASE) >> sizebits) & 0x1F;
	region_num += MSTAR_PCIE_OB_MEMIO_REG_START;
	addr = (1<<CPDI_RP_OB_REGION_ENABLE) | ((region_num-1) << sizebits) | (offset & sizemask);

	return _mdrv_RC_ob_config_read(pD, addr, data);
}

uint32_t _mdrv_RC_ob_memio_write(void *pD, uint32_t offset, uint32_t data)
{
	uint32_t	region_num, addr, sizemask, sizebits;

	//For compatible with previous interface, use bit31 for enable memory IO and offset is not full address
	if ( offset >= 0x80000000 && offset < (0x80000000 + CPDI_AXI_REGION_1_SIZE) )
		offset = (offset & 0x7FFFFFFF) + MSTAR_PCIE_OB_MEMIO_REG1_BASE;

	if ( (offset < MSTAR_PCIE_OB_MEMIO_REG1_BASE) ||
		(offset >= MSTAR_PCIE_OB_MEMIO_REG1_BASE + MSTAR_PCIE_OB_MEMIO_REGS * CPDI_AXI_REGION_1_SIZE) )
	{
		printk("[PCIE] ob mmio with illegal offset 0x%08x !\n", offset);
		return -1;
	}

	sizebits = ffs(CPDI_AXI_REGION_1_SIZE) - 1;
	sizemask = CPDI_AXI_REGION_1_SIZE - 1;

	region_num = ((offset - MSTAR_PCIE_OB_MEMIO_REG1_BASE) >> sizebits) & 0x1F;
	region_num += MSTAR_PCIE_OB_MEMIO_REG_START;
	addr = (1<<CPDI_RP_OB_REGION_ENABLE) | ((region_num-1) << sizebits) | (offset & sizemask);

	return _mdrv_RC_ob_config_write(pD, addr, data);
}
#endif

// Inbound DMA write Verify
// 1. //sim_esl/pcie_rc/PA_Zebu/export_run_vap.tcl load the IMG_001.bin to 0x0(mapping to 0x1,0000,0000)
// 2. GUI type CMD => send_data_from_file  file_format  file_name  start_addr  total_size(4 byte base unit)  payload_size(MAX:0x40, 4 byte base unit)
// 3. Write16(0x1F202154, 0xFDFF) : Disable System Intr MASK.   Read16(0x1F202174, 0x0200) : Read bit9 for system PCIe Intr status
// 4. i_xtor_ctrl => send_data_from_file_burst bin IMG_001.bin 0x400000 0x40000 0x200 0x20 (1MB file_size 128 payload size)
uint32_t _mdrv_RC_inbound_DMA_write(void *pD, char *FILENAME)
{

#ifdef ZEBU_ESL

    uint32_t i = 0; intr_val = 0, diff=0, ret_val = 0;
    uint32_t retval_hi, retval_lo;
    uint32_t comp_rx_lo, comp_rx_hi;
    uint32_t acc_size = 0, buff_size = 2048, file_size = 1024*1024; // //1MB
    unsigned long long retval;
    uint16_t tmp = 0;

    //Unmask int_inta_out and int_msg_rcv must mask, because the 0x1F202174 (Sysyem Intr) will Or int_inta_out and int_msg_rcv
    Write16(REG_INT_MSK1, 0x00FE);
    do
    {
        for(i=0; i<buff_size; i=i+8)
        {
            retval = Read64(MIU0_BASE + acc_size + i);
            retval_hi = (uint32_t) (retval >>32);
            retval_lo = (uint32_t) (retval);
            //uart_write_U32_hex(retval_hi); uart_write_U32_hex(retval_lo);  uart_putc('\n');
            do
            {
                 intr_val = Read32(0x1F202174);
            }while(intr_val = 0);

            comp_rx_lo = Read32(CPDREF_RP_SRAM_LOCAL + acc_size +i);
            comp_rx_hi = Read32(CPDREF_RP_SRAM_LOCAL + acc_size +i+4);
            //comp_rx_hi = revert(comp_rx_hi);
            //comp_rx_lo = revert(comp_rx_lo);

            if((retval_hi != comp_rx_hi) | (retval_lo != comp_rx_lo))
            {
                diff = 1;
                uart_write_U32_hex(retval_hi); uart_write_U32_hex(retval_lo); uart_putc('\n');
                uart_write_U32_hex(CPDREF_RP_SRAM_LOCAL+i); uart_puts("= >", 4); uart_write_U32_hex(comp_rx_hi); uart_write_U32_hex(comp_rx_lo); uart_puts("F a i l", 8); uart_putc('\n');
                break;
            }
        }

        if(diff == 0)
        {
             file_size -= buff_size;
             acc_size  += buff_size;
             ret_val = _mdrv_RC_ob_message_write(pD, MSTAR_PCIE_OB_MSG_REG_START, 0x7E, 0x80); //Send Vendor defined MSG to EP to disable  0x1F202174 (System Intr)

             if((acc_size % (buff_size * 8)) == 0)
             {uart_write_U32_hex(CPDREF_RP_SRAM_LOCAL + acc_size);  uart_puts("    P A S S", 12);}
             //intr_val = Read32(0x1F202174);
             //uart_write_U32_hex(intr_val); uart_putc('\n');
             //break;
        }
        else
            break;

    }while(file_size > 0);

    if(diff ==1)
    {
        uart_puts("D M A   W r i t e   F a i l !", 30); uart_putc('\n');
    }
    else
    {
        uart_puts("D M A   W r i t e   S u c c e s s f u l", 40); uart_putc('\n');
    }


    Write16(REG_INT_CLR0, REG_INT_CLR0_MASK);

    intr_val = Read32(0x1F202174);
    uart_write_U32_hex(intr_val);

#else
/*
    FILE *g_fp = NULL;
    int   retVal = 0, file_size = 0, read_size = 0;
    uint8_t rc_buffer[RC_DMA_BUFFER_SIZE] = {0};

    g_fp = fopen(FILENAME ,"rb");
    retVal = fseek(g_fp, 0, SEEK_END);
    file_size = ftell(g_fp);
    retVal = fseek(g_fp, 0, SEEK_SET);

    if (file_size <= 0)
        return -1;

    while(file_size > 0)
    {
        if(file_size >= RC_DMA_BUFFER_SIZE)
        {
            read_size = fread(rc_buffer, 1, RC_DMA_BUFFER_SIZE, g_fp);

            if(read_size != RC_DMA_BUFFER_SIZE)
                return -1;
        }
        else
        {
            read_size = fread(rc_buffer, 1, file_size, g_fp);
            if(read_size != file_size)
                return -1;
        }

        file_size -= read_size;
    }
    */
#endif
    return 0;
}

// 1.  i_xtor_ctrl -> receive_data_to_file bin IMG_002.bin 0x400000  0x40000 0x20
// 2.  xxd IMG_002.bin > IMG.txt
uint32_t _mdrv_RC_inbound_DMA_read(void *pD)
{
#ifdef ZEBU_ESL
    uint32_t i, retval_hi, retval_lo;
    uint32_t file_size = 2048;
    unsigned long long retval;

   for(i=0; i<file_size; i=i+8)
   {
       retval = Read64(MIU0_BASE + i);
       retval_hi = (uint32_t) (retval >>32);
       retval_lo = (uint32_t) (retval);
       //uart_write_U32_hex(retval_hi); uart_write_U32_hex(retval_lo);  uart_putc('\n');

       Write32(CPDREF_RP_SRAM_LOCAL + i, retval_lo);
       Write32(CPDREF_RP_SRAM_LOCAL + i + 4, retval_hi);
    }

   uart_puts("D M A   R e a d", 16); uart_putc('\n');
#endif
    return 0;
}
#ifdef K6_IoCtlTest
//1. start_read_ob_memrd_from_file IMG_003.bin  bin 0x2000 //8k PDI_RP_OB_APERTURE_SIZE = 8k file size
//2. close_read_ob_memrd_from_file
uint32_t _mdrv_RC_outbound_DMA_read(void *pD, uint32_t descriptor_addr, uint32_t data_addr, uint32_t dma_translate_size, uint32_t offset)
{
    struct Mstar_DMA_Descriptor *buf1, *buf2;
    uint32_t miu_DesAddr = 0x0, miu_DataAddr = 0x0;
    uint32_t access_offset = ((CPDI_RP_OB_REGION_NUMBER-1)<<20) + (1<<CPDI_RP_OB_REGION_ENABLE);
    uint32_t axi_base_addr;
    uint32_t dma_translate_size_max = 1<<(CPDI_RP_OB_APERTURE_SIZE+1);
    uint32_t data_size = 128, accu_data_size = 0;  //data_size must be 16 multiple ; data size range:1 ~ 1024
    uint32_t remaining_size = dma_translate_size;
    uint32_t ret = 0;
    int32_t rt_val = 0;
#ifdef ZEBU_ESL
    uint32_t comp_rx_lo, comp_rx_hi, i;
    uint32_t refval_lo, refval_hi, diff = 0;
    unsigned long long retval;
#endif

    uint8_t AXI_ID = 0x1;

    uint8_t buf1_parameter = MSTAR_PCIE_DMA_CONTI | MSTAR_PCIE_DMA_READ | MSTAR_PCIE_DMA_NORMAL_PRI;
    uint8_t buf2_parameter = MSTAR_PCIE_DMA_STOP | MSTAR_PCIE_DMA_READ | MSTAR_PCIE_DMA_NORMAL_PRI | MSTAR_PCIE_DMA_INTR;

    if((dma_translate_size < 16) || (dma_translate_size > dma_translate_size_max)){
      rt_val = -1;
	 goto end;
    }

    if(offset > 0x100000){ //1MB size
      rt_val = -1;
	 goto end;
    }
    access_offset += offset;
    miu_DesAddr  = _mdrv_RC_MIU_translation(descriptor_addr);
    miu_DataAddr = _mdrv_RC_MIU_translation(data_addr);
    if (miu_DesAddr == 0 || miu_DataAddr ==0){
      rt_val = -2;
	 goto end;
    }
    buf1 = (struct Mstar_DMA_Descriptor *) ioremap(descriptor_addr, sizeof(struct Mstar_DMA_Descriptor)*2);
    buf2 = (struct Mstar_DMA_Descriptor *) (buf1 + 1); // 1 means next buf1 size
    Write16( REG_CH0_HEAD_DESC_ADDR ,     (unsigned short) (miu_DesAddr & 0x0000FFFF));
    Write16( REG_CH0_HEAD_DESC_ADDR + 4 , (unsigned short) ((miu_DesAddr & 0x0FFF0000) >> 16));
    Write16( REG_CH0_LLIST_DONE_INT_EN,   (unsigned short) REG_CH0_LLIST_DONE_INT_EN_MASK); //Enable Intr

    //printk("buf1 = %x, buf2 = %x, \n", buf1, buf2);
    //printk("buf2=>para2 = %x buf2=>para3 = %x  \n", &(buf2->para2), &(buf2->para3));

    //while(accu_data_size < dma_translate_size)
    while(remaining_size>0)
    {
        ret = check_dma_int_deassert();
        if(ret !=0){
            printk("     _mdrv_RC_outbound_DMA_read() check_dma_int_deassert Error: Addrress => 0x%08x !\n", data_addr);
            rt_val = -3;
		  goto end;
        }

        // Prepare Descriptor Table List at DRAM Memory before Fire
        if(remaining_size>data_size*2)
        {
            axi_base_addr = access_offset + accu_data_size;
            buf1->para1 = (miu_DataAddr & PCIE_MSTAR_DMA_DESCRIPTOR_STARTADDR_MASK);   // 28bits
            buf1->para1 = buf1->para1 | (((data_size & 0x0000000F)<<28) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA1);
            buf1->para2 = ((data_size & 0x000FFFF0)>>4) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA2;     // 20bits
            buf1->para2 = buf1->para2 | ((AXI_ID<<PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_MASK);
            buf1->para2 = buf1->para2 | (((axi_base_addr & 0x000000FF)<<24) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_LOW_MASK);
            buf1->para3 = (((axi_base_addr & 0xFFFFFF00)>>8) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_HIGH_MASK);
            buf1->para3 = buf1->para3 | ((buf1_parameter << PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_MASK);
            buf1->para4 = (miu_DesAddr+1) & PCIE_MSTAR_DMA_DESCRIPTOR_NEXT_MASK;
            accu_data_size += data_size;

            axi_base_addr = access_offset + accu_data_size;
            buf2->para1 = buf1->para1 + (data_size >>4); //28bits
            buf2->para1 = buf2->para1 | (((data_size & 0x0000000F)<<28) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA1);
            buf2->para2 = ((data_size & 0x000FFFF0)>>4) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA2;     // 20bits
            buf2->para2 = buf2->para2 + ((AXI_ID<<PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_MASK);
            buf2->para2 = buf2->para2 | (((axi_base_addr & 0x000000FF)<<24) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_LOW_MASK);
            buf2->para3 = (((axi_base_addr & 0xFFFFFF00)>>8) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_HIGH_MASK);
            buf2->para3 = buf2->para3 + ((buf2_parameter << PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_MASK);
            buf2->para4 = 0x0;
            accu_data_size += data_size;
            remaining_size -= (data_size*2);
        }
        else if (remaining_size>data_size)
        {
            axi_base_addr = access_offset + accu_data_size;
            buf1->para1 = (miu_DataAddr & PCIE_MSTAR_DMA_DESCRIPTOR_STARTADDR_MASK);   // 28bits
            buf1->para1 = buf1->para1 | (((data_size & 0x0000000F)<<28) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA1);
            buf1->para2 = ((data_size & 0x000FFFF0)>>4) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA2;     // 20bits
            buf1->para2 = buf1->para2 | ((AXI_ID<<PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_MASK);
            buf1->para2 = buf1->para2 | (((axi_base_addr & 0x000000FF)<<24) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_LOW_MASK);
            buf1->para3 = (((axi_base_addr & 0xFFFFFF00)>>8) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_HIGH_MASK);
            buf1->para3 = buf1->para3 | ((buf1_parameter << PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_MASK);
            buf1->para4 = (miu_DesAddr+1) & PCIE_MSTAR_DMA_DESCRIPTOR_NEXT_MASK;
            accu_data_size += data_size;
            remaining_size -= data_size;

            axi_base_addr = access_offset + accu_data_size;
            buf2->para1 = buf1->para1 + (data_size >>4); //28bits
            buf2->para1 = buf2->para1 | (((remaining_size & 0x0000000F)<<28) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA1);
            buf2->para2 = ((remaining_size & 0x000FFFF0)>>4) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA2;     // 20bits
            buf2->para2 = buf2->para2 + ((AXI_ID<<PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_MASK);
            buf2->para2 = buf2->para2 | (((axi_base_addr & 0x000000FF)<<24) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_LOW_MASK);
            buf2->para3 = (((axi_base_addr & 0xFFFFFF00)>>8) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_HIGH_MASK);
            buf2->para3 = buf2->para3 + ((buf2_parameter << PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_MASK);
            buf2->para4 = 0x0;
            accu_data_size += remaining_size;
            remaining_size = 0;
        }
        else
        {
            axi_base_addr = access_offset + accu_data_size;
            buf1->para1 = (miu_DataAddr & PCIE_MSTAR_DMA_DESCRIPTOR_STARTADDR_MASK);   // 28bits
            buf1->para1 = buf1->para1 | (((remaining_size & 0x0000000F)<<28) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA1);
            buf1->para2 = ((remaining_size & 0x000FFFF0)>>4) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA2;     // 20bits
            buf1->para2 = buf1->para2 | ((AXI_ID<<PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_MASK);
            buf1->para2 = buf1->para2 | (((axi_base_addr & 0x000000FF)<<24) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_LOW_MASK);
            buf1->para3 = (((axi_base_addr & 0xFFFFFF00)>>8) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_HIGH_MASK);
            buf1->para3 = buf1->para3 | ((buf2_parameter << PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_MASK);
            buf1->para4 = 0x0;
            accu_data_size += remaining_size;
            remaining_size = 0;
        }

        Write16( REG_CH0_HEAD_DESC_FIRE,   (unsigned short) REG_CH0_HEAD_DESC_FIRE_MASK);

        ret = check_dma_int_assert();
        if(ret !=0){
            printk("     _mdrv_RC_outbound_DMA_read() check_dma_int_assert Error: Addrress => 0x%08x !\n", data_addr);
            rt_val = -3;
		  goto end;
        }

        miu_DataAddr = _mdrv_RC_MIU_translation(data_addr+accu_data_size);


#ifdef ZEBU_ESL
        for(i=0; i < (CPDI_RP_OB_DESCRIPTOR_SIZE/8)*2 ; i=i+8)
        {
            comp_rx_lo = Read32(descriptor_addr +i);
            comp_rx_hi = Read32(descriptor_addr + i +4);
            uart_write_U32_hex(descriptor_addr+i); uart_puts("= >", 4); uart_write_U32_hex(comp_rx_hi); uart_write_U32_hex(comp_rx_lo); uart_putc('\n');
            uart_putc('\n');
        }

        for(i=0; i < data_size*2 /* 2 descriptors */ ; i=i+8)
        {
            refval = Read64(MIU_BASE + (accu_data_size-data_size*2) + i);
            refval_hi = (uint32_t) (retval >>32);
            refval_lo = (uint32_t) (retval);

            comp_rx_lo = Read32(data_addr + (accu_data_size-data_size*2) + i);
            comp_rx_hi = Read32(data_addr + (accu_data_size-data_size*2) + i + 4);

            if((refval_hi != comp_rx_hi) | (refval_lo != comp_rx_lo))
            {
                diff = 1;
                uart_write_U32_hex (data_addr + (accu_data_size-data_size*2) + i); uart_puts("= >", 4); uart_write_U32_hex(comp_rx_hi); uart_write_U32_hex(comp_rx_lo); uart_putc('\n');
                break;
            }
        }
#endif

    }
    printk("     _mdrv_RC_outbound_DMA_read(): axi_base_addr = %x \n", axi_base_addr);

    end:
    iounmap(buf1);
    return rt_val;
}

//1. start_dump_ob_memwr_to_file IMG_003.bin  bin 0x2000 //8k PDI_RP_OB_APERTURE_SIZE = 8k file size
//2. start_dump_ob_memwr_to
uint32_t _mdrv_RC_outbound_DMA_write(void *pD, uint32_t descriptor_addr, uint32_t data_addr, uint32_t dma_translate_size)
{

    struct Mstar_DMA_Descriptor *buf1, *buf2;
    uint32_t miu_DesAddr = 0x0, miu_DataAddr = 0x0;
    uint32_t access_offset = 0x4000 + ((CPDI_RP_OB_REGION_NUMBER-1)<<20) + (1<<CPDI_RP_OB_REGION_ENABLE);
    uint32_t axi_base_addr;
    uint32_t dma_translate_size_max = 1<<(CPDI_RP_OB_APERTURE_SIZE+1);
    uint32_t  remaining_size = dma_translate_size;
    uint32_t data_size = 128, accu_data_size = 0;  //data_size must be 16 multiple ; data size range:1 ~ 1024
    uint32_t ret = 0;
    int32_t rt_val = 0;

    uint8_t AXI_ID = 0x3;
    uint8_t buf1_parameter = MSTAR_PCIE_DMA_CONTI | MSTAR_PCIE_DMA_WRITE | MSTAR_PCIE_DMA_NORMAL_PRI;
    uint8_t buf2_parameter = MSTAR_PCIE_DMA_STOP | MSTAR_PCIE_DMA_WRITE | MSTAR_PCIE_DMA_NORMAL_PRI | MSTAR_PCIE_DMA_INTR;

    if((dma_translate_size < 16) || (dma_translate_size > dma_translate_size_max)){
      rt_val = -1;
	 goto end;
    }
    miu_DesAddr  = _mdrv_RC_MIU_translation(descriptor_addr);
    miu_DataAddr = _mdrv_RC_MIU_translation(data_addr);
    if (miu_DesAddr == 0 || miu_DataAddr ==0){
      rt_val = -2;
	 goto end;
    }
    buf1 = (struct Mstar_DMA_Descriptor *) ioremap(descriptor_addr, sizeof(struct Mstar_DMA_Descriptor)*2);
    buf2 = (struct Mstar_DMA_Descriptor *) (buf1 + 1); // 1 means next buf1 size
    Write16( REG_CH0_HEAD_DESC_ADDR ,     (unsigned short) (miu_DesAddr & 0x0000FFFF));
    Write16( REG_CH0_HEAD_DESC_ADDR + 4 , (unsigned short) ((miu_DesAddr & 0x0FFF0000) >> 16));
    Write16( REG_CH0_LLIST_DONE_INT_EN,   (unsigned short) REG_CH0_LLIST_DONE_INT_EN_MASK); //Enable Intr

    //while(accu_data_size < dma_translate_size)
    while(remaining_size>0)
    {
        ret = check_dma_int_deassert();
        if(ret !=0)
        {
            printk("     _mdrv_RC_outbound_DMA_write() check_dma_int_deassert Error: Addrress => 0x%08x !\n", data_addr);
            rt_val = -3;
	       goto end;
        }

        // Prepare Descriptor Table List at DRAM Memory before Fire
        if(remaining_size>data_size*2)
        {
            axi_base_addr = access_offset + accu_data_size;
            buf1->para1 = (miu_DataAddr & PCIE_MSTAR_DMA_DESCRIPTOR_STARTADDR_MASK);   // 28bits
            buf1->para1 = buf1->para1 | (((data_size & 0x0000000F)<<28) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA1);
            buf1->para2 = ((data_size & 0x000FFFF0)>>4) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA2;     // 20bits
            buf1->para2 = buf1->para2 | ((AXI_ID<<PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_MASK);
            buf1->para2 = buf1->para2 | (((axi_base_addr & 0x000000FF)<<24) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_LOW_MASK);
            buf1->para3 = (((axi_base_addr & 0xFFFFFF00)>>8) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_HIGH_MASK);
            buf1->para3 = buf1->para3 | ((buf1_parameter << PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_MASK);
            buf1->para4 = (miu_DesAddr+1) & PCIE_MSTAR_DMA_DESCRIPTOR_NEXT_MASK;
            accu_data_size += data_size;

            axi_base_addr = access_offset + accu_data_size;
            buf2->para1 = buf1->para1 + (data_size >>4); //28bits
            buf2->para1 = buf2->para1 | (((data_size & 0x0000000F)<<28) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA1);
            buf2->para2 = ((data_size & 0x000FFFF0)>>4) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA2;     // 20bits
            buf2->para2 = buf2->para2 + ((AXI_ID<<PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_MASK);
            buf2->para2 = buf2->para2 | (((axi_base_addr & 0x000000FF)<<24) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_LOW_MASK);
            buf2->para3 = (((axi_base_addr & 0xFFFFFF00)>>8) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_HIGH_MASK);
            buf2->para3 = buf2->para3 + ((buf2_parameter << PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_MASK);
            buf2->para4 = 0x0;
            accu_data_size += data_size;
            remaining_size -= (data_size*2);
        }
        else if (remaining_size>data_size)
        {
            axi_base_addr = access_offset + accu_data_size;
            buf1->para1 = (miu_DataAddr & PCIE_MSTAR_DMA_DESCRIPTOR_STARTADDR_MASK);   // 28bits
            buf1->para1 = buf1->para1 | (((data_size & 0x0000000F)<<28) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA1);
            buf1->para2 = ((data_size & 0x000FFFF0)>>4) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA2;     // 20bits
            buf1->para2 = buf1->para2 | ((AXI_ID<<PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_MASK);
            buf1->para2 = buf1->para2 | (((axi_base_addr & 0x000000FF)<<24) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_LOW_MASK);
            buf1->para3 = (((axi_base_addr & 0xFFFFFF00)>>8) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_HIGH_MASK);
            buf1->para3 = buf1->para3 | ((buf1_parameter << PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_MASK);
            buf1->para4 = (miu_DesAddr+1) & PCIE_MSTAR_DMA_DESCRIPTOR_NEXT_MASK;
            accu_data_size += data_size;
            remaining_size -= data_size;

            axi_base_addr = access_offset + accu_data_size;
            buf2->para1 = buf1->para1 + (data_size >>4); //28bits
            buf2->para1 = buf2->para1 | (((remaining_size & 0x0000000F)<<28) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA1);
            buf2->para2 = ((remaining_size & 0x000FFFF0)>>4) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA2;     // 20bits
            buf2->para2 = buf2->para2 + ((AXI_ID<<PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_MASK);
            buf2->para2 = buf2->para2 | (((axi_base_addr & 0x000000FF)<<24) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_LOW_MASK);
            buf2->para3 = (((axi_base_addr & 0xFFFFFF00)>>8) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_HIGH_MASK);
            buf2->para3 = buf2->para3 + ((buf2_parameter << PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_MASK);
            buf2->para4 = 0x0;
            accu_data_size += remaining_size;
            remaining_size = 0;
        }
        else
        {
            axi_base_addr = access_offset + accu_data_size;
            buf1->para1 = (miu_DataAddr & PCIE_MSTAR_DMA_DESCRIPTOR_STARTADDR_MASK);   // 28bits
            buf1->para1 = buf1->para1 | (((remaining_size & 0x0000000F)<<28) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA1);
            buf1->para2 = ((remaining_size & 0x000FFFF0)>>4) &  PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA2;     // 20bits
            buf1->para2 = buf1->para2 | ((AXI_ID<<PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_MASK);
            buf1->para2 = buf1->para2 | (((axi_base_addr & 0x000000FF)<<24) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_LOW_MASK);
            buf1->para3 = (((axi_base_addr & 0xFFFFFF00)>>8) & PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_HIGH_MASK);
            buf1->para3 = buf1->para3 | ((buf2_parameter << PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_SHIFT) & PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_MASK);
            buf1->para4 = 0x0;
            accu_data_size += remaining_size;
            remaining_size = 0;
        }

        Write16( REG_CH0_HEAD_DESC_FIRE,   (unsigned short) REG_CH0_HEAD_DESC_FIRE_MASK);

        ret = check_dma_int_assert();
        if(ret !=0)
        {
            printk("     _mdrv_RC_outbound_DMA_write(): check_dma_int_assert Error: Addrress => 0x%08x !\n", data_addr);
            rt_val = -3;
		  goto end;
        }

        miu_DataAddr = _mdrv_RC_MIU_translation(data_addr+accu_data_size);

      /*
#ifdef ZEBU_ESL
        for(int i=0; i < (CPDI_RP_OB_DESCRIPTOR_SIZE/8)*2 ; i=i+8)
        {
            comp_rx_lo = Read32(descriptor_addr +i);
            comp_rx_hi = Read32(descriptor_addr + i +4);
            uart_write_U32_hex(descriptor_addr+i); uart_puts("= >", 4); uart_write_U32_hex(comp_rx_hi); uart_write_U32_hex(comp_rx_lo); uart_putc('\n');
            uart_putc('\n');
        }
#endif
      */

    }

    printk("     _mdrv_RC_outbound_DMA_write(): axi_base_addr = %x \n", axi_base_addr);
    end:
    iounmap(buf1);
    return rt_val;
}
#endif
/*
Message Code = 0x7E => Triggers detection of UR by Completer if not implemented.
Message Code = 0x7F => Silently discarded by Completer if not implemented.

Table 2-18: Message Routing
r[2:0](b)    Description                        Bytes 8 Through 15^10
000         Routed to Root Complex                  Reserved
001         Routed by Address11                     Address
010         Routed by ID                            See Section 2.2.4.2
011         Broadcast from Root Complex             Reserved
100         Local - Terminate at Receiver           Reserved
101         Gathered and routed to Root Complex     Reserved
110-111     Reserved - Terminate at Receiver        Reserved
*/

//_mdrv_RC_ob_message_write(pD, 2, 0x7E, 0x80); //Send Vendor defined MSG to EP to disable  0x1F202174 (System Intr)
#ifdef CONFIG_MSTAR_PCIE_OB_MSG_ENABLE
uint32_t _mdrv_RC_ob_message_write(void *pD, uint32_t msg_region, uint32_t msg_code, uint32_t msg_routing)
{


    uint32_t ret = 0;
    unsigned short data_lo = (unsigned short) ((msg_code<<8) | msg_routing); //bit[15~8]:Message Code, bit[7~5]: Message Routing
    unsigned short data_hi = (unsigned short) ((msg_region-1)<<4 | 0x0200);  //bit[25] =0x1, bit[24:20] = ¡§Region¡¨-1

    if(msg_region<1)
        return -1;

    ret = check_interrupt_deassert();
    if(ret !=0)
    {
        printk("     _mdrv_RC_ob_message_write() check_interrupt_deassert Error: msg_code => 0x%08x , msg_routubg = > 0x%08x!\n", msg_code, msg_routing);
        return -1;
    }
    Write16(REG_PCIE_CM_ADDR,   data_lo);
    Write16(REG_PCIE_CM_ADDR+4, data_hi);
    Write16(REG_PCIE_CM_WDATA,   0);
    Write16(REG_PCIE_CM_WDATA+4, 0);
    Write16(REG_PCIE_CM_WSTRB,   0x043f);
    Write16(REG_PCIE_CM_FIRE,    0x0001);

    // polling or Interrupt event of REG_PCIE_CM_DONE_INT
    ret = check_interrupt_assert();
    if(ret !=0)
    {
        printk("     _mdrv_RC_ob_message_write()=> check_interrupt_assert Error: msg_code => 0x%08x , msg_routubg = > 0x%08x!\n", msg_code, msg_routing);
        return -1;
    }
    return 0;
}
#endif

//1. Set the msi config address, data
//2. device send msi
//3. system intr will raise and rc reg_int_status1 will raise   b[5]: int_msi_queue   b[6]: int_msi_vtable
uint32_t _mdrv_RC_ib_msi_config(void *pD, uint32_t msi_addr, uint32_t msi_data, unsigned short msi_ctrl)
{
#ifdef ZEBU_ESL
    uint32_t ret_val = 0;
    unsigned short addr_lo = (unsigned short) (msi_addr & 0x0000FFFF);
    unsigned short addr_hi = (unsigned short) ((msi_addr & 0xFFFF0000) >> 16);
    unsigned short data_lo = (unsigned short) (msi_data & 0x0000FFFF);
    unsigned short data_hi = (unsigned short) ((msi_data & 0xFFFF0000) >> 16);

    Write16(REG_MSIRCV_MADDR,   addr_lo);
    Write16(REG_MSIRCV_MADDR+4,   addr_hi);
    Write16(REG_MSIRCV_MDATA,   data_lo);
    Write16(REG_MSIRCV_MDATA+4,   data_hi);
    Write16(REG_MSIRCV_CONTROL, msi_ctrl);  //    #define REG_MSIRCV_MSI2QUEUE_EN           (1<<1)    #define REG_MSIRCV_MSI2VTABLE_EN          (1<<2)

    do
    {
        ret_val =  Read16(REG_INT_STATUS1);
    }while(ret_val = 0);
#endif
    return 0;
}
void _mdrv_write_pcie_bar0()
{
    uint32_t ret = 0;
    //*** write EP bar 0 0xFFFFFFFF to get aperture ***
    Write16(REG_PCIE_CM_ADDR,    0x0010);
    Write16(REG_PCIE_CM_ADDR+4,  0x0930);
    Write16(REG_PCIE_CM_WDATA,   0xfff0);
    Write16(REG_PCIE_CM_WDATA+4, 0xffff);
    Write16(REG_PCIE_CM_WSTRB,   0x033f);
    Write16(REG_PCIE_CM_FIRE,    0x0001);

    //[PCIE] A2M_BRIDGE interrupt subroutine begin
    //[PCIE] EP Received CfgWr0 TLP Packet
    //[PCIE] A2M_BRIDGE CM done interrupt asserted
    ret = check_interrupt_assert();

    //[PCIE] A2M_BRIDGE CM done interrupt de-asserted
    ret = check_interrupt_deassert();
    //[PCIE] A2M_BRIDGE interrupt subroutine done
    //[PCIE] write CFG_WR EP bar 0 0xFFFFFFFF done
}

void _mdrv_read_pcie_bar0()
{
    uint16_t tmp;
    uint32_t ret = 0;
    //*** read EP bar 0 to get aperture ***
    Write16(REG_PCIE_CM_ADDR,    0x0010);
    Write16(REG_PCIE_CM_ADDR+4,  0x0930);
    Write16(REG_PCIE_CM_WSTRB,   0x0320);
    Write16(REG_PCIE_CM_FIRE,    0x0001);

    //[PCIE] A2M_BRIDGE interrupt subroutine begin
    //[PCIE] EP Received CfgRd0 TLP Packet
    //[PCIE] RC Received Cpld TLP Packet
    //[PCIE] A2M_BRIDGE CM done interrupt asserted
    ret = check_interrupt_assert();

    //[PCIE] A2M_BRIDGE CM done interrupt de-asserted
    ret = check_interrupt_deassert();

    //[PCIE] A2M_BRIDGE interrupt subroutine done
    tmp = Read16(REG_PCIE_CM_RDATA+4);
    tmp = Read16(REG_PCIE_CM_RDATA);

}


irqreturn_t _mdrv_pcie_isr(int irq, void *devid)
{
    uint16_t  reg_intr_status0 = 0x0 , reg_intr_status1 = 0x0;
    uint32_t  ret;

    reg_intr_status0 = Read16(REG_INT_STATUS0);
    reg_intr_status1 = Read16(REG_INT_STATUS1);

    //printk("     _mdrv_pcie_isr()  REG_INT_STATUS0= 0x%x,  REG_INT_STATUS1=0x%x!\n",  reg_intr_status0,reg_intr_status1);

    if(reg_intr_status0)
       printk("     _mdrv_pcie_isr()  REG_INT_STATUS0= 0x%x,  REG_INT_STATUS1=0x%x!\n",  reg_intr_status0,reg_intr_status1);
    if(reg_intr_status0 & REG_INT_LOCAL_INTERRUPT)  // Level Three Interrupt => Check Loca Error
    {
        CPDREF_LocalInterruptHandler_rp(ret);
    }


    if(reg_intr_status1 & REG_INT_INTA_OUT)
    {
        //printk("     _mdrv_pcie_isr(): REG_INT_INTA_OUT \n");
        generic_handle_irq(PCIE_IRQ_INTA);
    }
    if(reg_intr_status1 & REG_INT_INTB_OUT)
    {
        //printk("     _mdrv_pcie_isr(): REG_INT_INTB_OUT \n");
        generic_handle_irq(PCIE_IRQ_INTB);
    }
    if(reg_intr_status1 & REG_INT_INTC_OUT)
    {
        //printk("     _mdrv_pcie_isr(): REG_INT_INTC_OUT \n");
        generic_handle_irq(PCIE_IRQ_INTC);
    }
    if(reg_intr_status1 & REG_INT_INTD_OUT)
    {
        //printk("     _mdrv_pcie_isr(): REG_INT_INTD_OUT \n");
        generic_handle_irq(PCIE_IRQ_INTD);
    }


    //mdelay(1);

    return IRQ_HANDLED;
}


uint32_t _mdrv_pcie_irq_init(void)
{
    //uint16_t i;
    int32_t  irq_ret;


    //Unmask int_inta_out and int_msg_rcv must mask, because the 0x1F202174 (Sysyem Intr) will Or int_inta_out and int_msg_rcv
    Write16(REG_INT_MSK0, REG_INT_MSK0_MASK);
    Write16(REG_INT_MSK1, 0x00F0);

    //If request_irq succeeded, The REG_HST2_IRQ_MASK_31_16 will unmask the PCIe bit
    irq_ret = request_irq((E_IRQ_INT_PCIE_RC), _mdrv_pcie_isr, IRQF_DISABLED, "PCIE", NULL);  //IRQF_DISABLED(SA_INTERRUPT), IRQF_SHARED
#if defined(CONFIG_MP_PLATFORM_INT_1_to_1_SPI) && defined(CONFIG_MP_PLATFORM_GIC_SET_MULTIPLE_CPUS)
	irq_set_affinity_hint(E_IRQ_INT_PCIE_RC, cpu_online_mask);
	irq_set_affinity(E_IRQ_INT_PCIE_RC, cpu_online_mask);
#endif
/*
    for(i=0 ; i< 4 ; i++)
    {
        ret_val = Read16(REG_INT_HYP_BASE + 0x54*4 + i*4);
        printk("     _mdrv_pcie_irq_init() reg_hst2_irq_mask_%d = %x !\n",i, ret_val);
    }
*/
    if(irq_ret)
    {
        printk("     _mdrv_pcie_irq_init() irq_ret = %d !\n", irq_ret);
        return 1;
    }
    else
        return 0;
}

#ifdef DEBUG_RUNTIME_LOCAL_ERROR
uint32_t _mdrv_pcie_trigger_error_callback(void *pD)
{
    if (CPDI_Isr (pD))
        return EINVAL;

    return 0;
}
#endif

#ifdef CONFIG_MSTAR_K6
static void _mdrv_Mstar_phy_port1_init(uintptr_t U3PHY_D_base, uintptr_t U3PHY_A_base)
{
/*
    // USB3 Phy SS port 1 init
    WREG        (MS_BUS_RIU_ADDR + (0x102306 << 1)), 0x0104
    WREG        (MS_BUS_RIU_ADDR + (0x102300 << 1)), 0x0000
    WREG        (MS_BUS_RIU_ADDR + (0x102302 << 1)), 0x0000
    WREG        (MS_BUS_RIU_ADDR + (0x10233A << 1)), 0x0000

    WREG        (MS_BUS_RIU_ADDR + (0x102218 << 1)), 0x0160
    WREG        (MS_BUS_RIU_ADDR + (0x102220 << 1)), 0x0000
    WREG        (MS_BUS_RIU_ADDR + (0x102222 << 1)), 0x0000

    WREG        (MS_BUS_RIU_ADDR + (0x10233A << 1)), 0x0308
    RREG        w0, (MS_BUS_RIU_ADDR + (0x102302 << 1))
    ldr         w1, =0x4400
    bic         w0, w0, w1
    REG_C2M     w0, (MS_BUS_RIU_ADDR + (0x102302 << 1))

    WREG        (MS_BUS_RIU_ADDR + (0x1022C0 << 1)), 0x9374
    WREG        (MS_BUS_RIU_ADDR + (0x1022C2 << 1)), 0x0018
    WREG        (MS_BUS_RIU_ADDR + (0x1022C4 << 1)), 0x7002
    WREG        (MS_BUS_RIU_ADDR + (0x1022C6 << 1)), 0x04D8

    WREG        (MS_BUS_RIU_ADDR + (0x102360 << 1)), 0x3932
    WREG        (MS_BUS_RIU_ADDR + (0x102362 << 1)), 0x3939
    WREG        (MS_BUS_RIU_ADDR + (0x102364 << 1)), 0x3932
    WREG        (MS_BUS_RIU_ADDR + (0x102366 << 1)), 0x3939
    WREG        (MS_BUS_RIU_ADDR + (0x102312 << 1)), 0x0400

    WREG_B       ((MS_BUS_RIU_ADDR + (0x1023A0 << 1)) + 1), 0x0
    WREG_B      (MS_BUS_RIU_ADDR + (0x102212 << 1)), 0xF4

    RREG        w0, (MS_BUS_RIU_ADDR + (0x10230E << 1))
    ldr         w1, =0x0400
    bic         w0, w0, w1
    REG_C2M     w0, (MS_BUS_RIU_ADDR + (0x10230E << 1))


*/

    writew(0x0104,    (void*) (U3PHY_A_base+0x06*2));
    writew(0x0000,    (void*) (U3PHY_A_base+0x00*2));
    writew(0x0000,    (void*) (U3PHY_A_base+0x02*2));
    writew(0x0000,    (void*) (U3PHY_A_base+0x3A*2));

    writew(0x0160,    (void*) (U3PHY_D_base+0x18*2));
    writew(0x0000,    (void*) (U3PHY_D_base+0x20*2));
    writew(0x0000,    (void*) (U3PHY_D_base+0x22*2));

    writew(0x0308,    (void*) (U3PHY_A_base+0x3A*2));
    writew(readw((void*) (U3PHY_A_base+0x2*2)) & 0xBBFF, (void*) (U3PHY_A_base+0x2*2)) ;

    writew(0x9374,    (void*) (U3PHY_D_base+0xC0*2));
    writew(0x0018,    (void*) (U3PHY_D_base+0xC2*2));
    writew(0x7002,    (void*) (U3PHY_D_base+0xC4*2));
    writew(0x04D8,    (void*) (U3PHY_D_base+0xC6*2));

    writew(0x3932,    (void*) (U3PHY_A_base+0x60*2));
    writew(0x3939,    (void*) (U3PHY_A_base+0x62*2));
    writew(0x3932,    (void*) (U3PHY_A_base+0x64*2));
    writew(0x3939,    (void*) (U3PHY_A_base+0x66*2));
    writew(0x0400,    (void*) (U3PHY_A_base+0x12*2));

    writeb(0x00  ,    (void*) (U3PHY_A_base+0xA0*2 + 1));
    writeb(0xF4  ,    (void*) (U3PHY_D_base+0x12*2));
    writew(readw((void*) (U3PHY_A_base+0xE*2)) & 0xFBFF, (void*) (U3PHY_A_base+0xE*2)) ;
}
#endif

static void _mdrv_Mstar_U3phy_MS28_init(uintptr_t U3PHY_D_base, uintptr_t U3PHY_A_base)
{

        //-- 28 hpm mstar only---
#if(1)

#ifdef CONFIG_MSTAR_K6
        writeb(0x08, (void*) (U3PHY_D_base+0xF0*2+1));  //APHY+0xf1 [3] to 1¡¦b1.
        writeb(0x10, (void*) (U3PHY_D_base+0x84*2));  //([4]=reg_dedicate_pcie_phy)
#endif
        //Set ATOP+0x34[0] to 1¡¦b1 (16-bit addressing)
        writeb(0x01,    (void*) (U3PHY_A_base+0x34*4));

        writeb(0x0,    (void*) (U3PHY_D_base+0x3*2-1));
        writeb(0xC0,    (void*) (U3PHY_D_base+0x8*2));
        writeb(0x10,    (void*) (U3PHY_D_base+0x9*2-1));


        writew(0x0104, (void*) (U3PHY_A_base+0x6*2));  // for Enable 1G clock pass to UTMI //[2] reg_pd_usb3_purt [7:6] reg_gcr_hpd_vsel

        //U3phy initial sequence
        writew(0x0,    (void*) (U3PHY_A_base));                // power on rx atop ;
        writew(0x0,    (void*) (U3PHY_A_base+0x2*2));          // power on tx atop
        //writew(0x0910, (void*) (U3PHY_D_base+0x4*2));        // the same as default
        writew(0x0,    (void*) (U3PHY_A_base+0x3A*2));  // overwrite power on rx/tx atop
        writew(0x0560, (void*) (U3PHY_D_base+0x18*2));
        writew(0x0,    (void*) (U3PHY_D_base+0x20*2)); // power on u3_phy clockgen
        writew(0x0,    (void*) (U3PHY_D_base+0x22*2)); // power on u3_phy clockgen

        //writew(0x013F, (void*) (U3PHY_D_base+0x4A*2));      // the same as default
        //writew(0x1010, (void*) (U3PHY_D_base+0x4C*2));      // the same as default

        writew(0x0,    (void*) (U3PHY_A_base+0x3A*2)); // override PD control
                                                         // Add bit 0 enable 2016/01/26
        writew(0x309,    (void*) (U3PHY_A_base+0x3A*2)); // [9,8,3] PD_TXCLK_USB3TXPLL, PD_USB3_IBIAS, PD_USB3TXPLL override enable
        writeb(readb((void*)(U3PHY_A_base+0x3*2-1)) & 0xbb,     (void*)(U3PHY_A_base+0x3*2-1)); // override value 0

        // --SSC--
        //writew(0x04D8,      (void*) (U3PHY_D_base+0xC6*2));  //reg_tx_synth_span
        //writew(0x7002,       (void*) (U3PHY_D_base+0xC4*2));  //reg_tx_synth_step
        //writew(0x9374,       (void*) (U3PHY_D_base+0xC0*2));  //reg_tx_synth_set
        //writeb(0x18,    (void*) (U3PHY_D_base+0xC2*2));  //reg_tx_synth_set

       // -- TX current ---
        //writeb(0x01,         (void*) (U3PHY_A_base+0x35*2-1));
        writew(0x3932,  (void*) (U3PHY_A_base+0x60*2));
        writew(0x3939,  (void*) (U3PHY_A_base+0x62*2));
        writew(0x3932,  (void*) (U3PHY_A_base+0x64*2));
        writew(0x3939,  (void*) (U3PHY_A_base+0x66*2));
        writew(0x0400,  (void*) (U3PHY_A_base+0x12*2));
        // ---------------

        //-------- New for MS28 ---------
        writeb(0x0,    (void*) (U3PHY_A_base+0xA1*2-1));  //bit[15] EQ override
        //writeb(0xF4,   (void*) (U3PHY_D_base+0x12*2));   //TX lock threshold
        writeb(readb((void*)(U3PHY_A_base+0xF*2-1))&(u8)(~0x4),       (void*)(U3PHY_A_base+0xF*2-1));  // 0xF[10]  Fix AEQ RX-reset issue

        // ------- Add by PCIe PHY SW setting from USB3 PHY ---------
        //rx lock
        writeb(0xF1,   (void*) (U3PHY_D_base+0xA*2));   //  [13:0]=reg_rx_lock_value
        writeb(0x1,    (void*) (U3PHY_D_base+0xB*2-1));
        //tx lock
        writeb(0xF1,   (void*) (U3PHY_D_base+0x12*2));  //  [13:0]=reg_tx_lock_value
        writeb(0x1,    (void*) (U3PHY_D_base+0x13*2-1));

        //-------- PCIe Setting ---------
        writeb(0xDF,    (void*) (U3PHY_D_base+0xB4*2)); //  [0]=reg_pcie_md               [1]=reg_pcie_new_elastic_buf_en

        writeb(readb((void*)(U3PHY_D_base+0x3D*2-1)) | 0x10,      (void*)(U3PHY_D_base+0x3D*2-1));  //0x1E bit12 =1;
        writeb(readb((void*)(U3PHY_D_base+0x3A*2))   | 0x02,      (void*)(U3PHY_D_base+0x3A*2));    //0x1D bit 1 =1;

        writeb(0x0D,    (void*) (MSTAR_PCIE_U3PHY_P0_DTOP_BASE + 0x1E*2)); //  [3]=reg_2port_in_different_mode
                                                                           //  in projects with u3(P0) and pcie(P1) need to set this bit to bank of P0

        writeb(0x04,    (void*) (U3PHY_D_base + 0x1E*2));

        // Tune REFCLK
        writeb(0x18,    (void*) (MSTAR_PCIE_UTMISS1_BASE+0x2C*2)); 		// set TX Swing bit[6:4] for better REFCLK+-
        writeb(0x03,    (void*) (MSTAR_PCIE_UTMISS1_BASE+0x2B*2 -1));	// set TX reference bit[1:0] +50mv

        writeb(0x42,    (void*) (MSTAR_PCIE_UTMISS1_BASE+0x2F*2-1)); //  [30] & [25] to enable PCIE REF clock

        writeb(0x80,    (void*) (U3PHY_A_base+0x25*2-1)); //  turn on PD demux,  TEST_RXPLL[47]=1 for turning on PD demux

#ifdef CONFIG_MSTAR_K6
        // This should be noticed after K6 (included)
        // clear reg_pd_sqh & reg_pd_sqh_igen before reg_sqh_cal_start
        writeb(0x00,    (void*) (U3PHY_A_base+0x6C*2 +1)); //wriu APHY+0x6d  0x00
#endif
        writeb(0x01,    (void*) (U3PHY_A_base+0x6E*2)); //  calibrate SQH , [0]=reg_sqh_cal_start

#ifdef CONFIG_MSTAR_KANO
        //GCR_USB3TXPLLICTRL[1:0]=2¡¦b10
        writeb(0x02,    (void*) (U3PHY_A_base+0x12*2)); //  [1:0]=reg_gcr_usb3txpllictrl[1:0]
#endif
        //override utmi HS_SE0 for REFCLK 100M
        writeb(0x03,    (void*) (MSTAR_PCIE_UTMISS1_BASE+0x00*2)); //  [1]=reg_term_override
        //writeb(0xC0,    (void*) (MSTAR_PCIE_UTMISS1_BASE+0x10*2)); //  [7]=reg_se0_set
        writeb(0x00,    (void*) (MSTAR_PCIE_UTMISS1_BASE+0x10*2)); //  [7]=reg_se0_set
        //IREF_PDN=0 & PD_BG_CURRENT=0
        writeb(0x00,    (void*) (MSTAR_PCIE_UTMISS1_BASE+0x01*2-1)); //  [6]=IREF_PDN
        writeb(0x0F,    (void*) (MSTAR_PCIE_UTMISS1_BASE+0x08*2)); //  [7]=PD_BG_CURRENT

        //overwrite EQ @PCIE mode in FT/verification/normal function
        //writeb(0x82,    (void*) (U3PHY_A_base+0xA1*2-1));  // TEST_RXPLL[47]=1 for turning on PD demux
        writeb(0x88,    (void*) (U3PHY_A_base+0xA1*2-1));  // TEST_RXPLL[47]=1 for turning on PD demux
        //de-emphasis level best is preamp current 8, for best eye diagram
        writeb(0x08,    (void*) (U3PHY_A_base+0x62*2));
        //ictrl_txpll=01 is best (2X), original 10 is worst

        writeb(0x01,    (void*) (U3PHY_A_base+0x12*2));
        //upll bw can set to higher IX1.25 could gain TIE 6-8ps? (only in PCIE mode)
        writeb(0x10,    (void*) (MSTAR_PCIE_UPLL1_BASE+0x01*2-1));
        //reg_cal_start wait 10ms and then set 1
        mdelay(10);
        writeb(0x01,    (void*) (U3PHY_A_base+0x6E*2));


        mdelay(100);
        //writeb(readb((void*)(MSTAR_REG_000F_BANK+0x0*2)) & 0xfe,      (void*)(MSTAR_REG_000F_BANK+0x0*2));  //GPIO_PM 0(PERST) set high
        //writeb(readb((void*)(MSTAR_REG_000F_BANK+0x0*2)) | 0x2,       (void*)(MSTAR_REG_000F_BANK+0x0*2));
        //writeb(readb((void*)(MSTAR_REG_000F_BANK+0x6*2)) & 0xfe,      (void*)(MSTAR_REG_000F_BANK+0x6*2));  //GPIO_PM 3(3.3V) set low
        //writeb(readb((void*)(MSTAR_REG_000F_BANK+0x6*2)) & 0xfd,      (void*)(MSTAR_REG_000F_BANK+0x6*2));


#else

writeb( 0xc0 ,(void*) (MSTAR_RIU_BASE+0x00100800*2+0x00*2 ));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00100800*2+0x0e*2 ));
writeb( 0x10 ,(void*) (MSTAR_RIU_BASE+0x00100800*2+0x04*2 ));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00100800*2+0x05*2 -1));
writeb( 0x0f ,(void*) (MSTAR_RIU_BASE+0x00103a00*2+0x88*2 ));
writeb( 0x04 ,(void*) (MSTAR_RIU_BASE+0x00103a00*2+0x89*2 -1));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00103a00*2+0x80*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00103a00*2+0x81*2 -1));
writeb( 0x0f ,(void*) (MSTAR_RIU_BASE+0x00103a00*2+0x08*2 ));
writeb( 0x04 ,(void*) (MSTAR_RIU_BASE+0x00103a00*2+0x09*2 -1));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00103a00*2+0x00*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00103a00*2+0x01*2 -1));
writeb( 0x0f ,(void*) (MSTAR_RIU_BASE+0x00103900*2+0x08*2 ));
writeb( 0x04 ,(void*) (MSTAR_RIU_BASE+0x00103900*2+0x09*2 -1));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00103900*2+0x00*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00103900*2+0x01*2 -1));
writeb( 0x0f ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x08*2 ));
writeb( 0x04 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x09*2 -1));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x00*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x01*2 -1));
writeb( 0xc0 ,(void*) (MSTAR_RIU_BASE+0x00100800*2+0x20*2 ));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00100800*2+0x2e*2 ));
writeb( 0x10 ,(void*) (MSTAR_RIU_BASE+0x00100800*2+0x24*2 ));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00100800*2+0x25*2 -1));
writeb( 0x0f ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x88*2 ));
writeb( 0x04 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x89*2 -1));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x80*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x81*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00101e00*2+0xa0*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00101e00*2+0xa1*2 -1));
writeb( 0x04 ,(void*) (MSTAR_RIU_BASE+0x00102100*2+0x06*2 ));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00102100*2+0x07*2 -1));
writeb( 0x04 ,(void*) (MSTAR_RIU_BASE+0x00102300*2+0x06*2 ));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00102300*2+0x07*2 -1));
writeb( 0xc0 ,(void*) (MSTAR_RIU_BASE+0x00100800*2+0x00*2 ));
writeb( 0x10 ,(void*) (MSTAR_RIU_BASE+0x00100800*2+0x04*2 ));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00100800*2+0x05*2 -1));
writeb( 0xc0 ,(void*) (MSTAR_RIU_BASE+0x00100800*2+0x20*2 ));
writeb( 0x10 ,(void*) (MSTAR_RIU_BASE+0x00100800*2+0x24*2 ));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00100800*2+0x25*2 -1));
writeb( 0x0f ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x08*2 ));
writeb( 0x04 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x09*2 -1));
writeb( 0x84 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x02*2 ));
writeb( 0x90 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x03*2 -1));
writeb( 0x0b ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x14*2 ));
writeb( 0x40 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x15*2 -1));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x00*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x01*2 -1));
writeb( 0x0f ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x88*2 ));
writeb( 0x04 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x89*2 -1));
writeb( 0x84 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x82*2 ));
writeb( 0x90 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x83*2 -1));
writeb( 0x0b ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x94*2 ));
writeb( 0x40 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x95*2 -1));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x80*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0x81*2 -1));
writeb( 0xdf ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xb4*2 ));
writeb( 0x0d ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0x1e*2 ));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00102300*2+0x6e*2 ));
writeb( 0x42 ,(void*) (MSTAR_RIU_BASE+0x00103800*2+0xaf*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0x02*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0x03*2 -1));
writeb( 0x81 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0x08*2 ));
writeb( 0x11 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0x09*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0x02*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0x03*2 -1));
writeb( 0xc0 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0x08*2 ));
writeb( 0x10 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0x09*2 -1));
writeb( 0xf4 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0x0a*2 ));
writeb( 0x01 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0x0b*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102100*2+0xa1*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102100*2+0x02*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102100*2+0x03*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102100*2+0x00*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102100*2+0x01*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102300*2+0xa1*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102300*2+0x02*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102300*2+0x03*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102300*2+0x00*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102300*2+0x01*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102100*2+0x3a*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102100*2+0x3b*2 -1));
writeb( 0x60 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0x18*2 ));
writeb( 0x05 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0x19*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102300*2+0x3a*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102300*2+0x3b*2 -1));

writew(0x308,    (void*) (U3PHY_A_base+0x3A*2)); // [9,8,3] PD_TXCLK_USB3TXPLL, PD_USB3_IBIAS, PD_USB3TXPLL override enable
writeb( 0x60 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0x18*2 ));
writeb( 0x05 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0x19*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0x20*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0x21*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0x22*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0x23*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0x20*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0x21*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0x22*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0x23*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0x3c*2 ));
writeb( 0x02 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0x3d*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0x3c*2 ));
writeb( 0x02 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0x3d*2 -1));
writeb( 0x03 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xc4*2 ));
writeb( 0x30 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xc5*2 -1));
writeb( 0xa0 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xc6*2 ));
writeb( 0x04 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xc7*2 -1));
writeb( 0x04 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xc8*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xc9*2 -1));
writeb( 0x75 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xc0*2 ));
writeb( 0x93 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xc1*2 -1));
writeb( 0x18 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xc2*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xc3*2 -1));
writeb( 0x75 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xe0*2 ));
writeb( 0x93 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xe1*2 -1));
writeb( 0x18 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xe2*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xe3*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xe4*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xe5*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xe6*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xe7*2 -1));
writeb( 0x04 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xe8*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102000*2+0xe9*2 -1));
writeb( 0x03 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xc4*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xc5*2 -1));
writeb( 0xa0 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xc6*2 ));
writeb( 0x04 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xc7*2 -1));
writeb( 0x04 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xc8*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xc9*2 -1));
writeb( 0x75 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xc0*2 ));
writeb( 0x93 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xc1*2 -1));
writeb( 0x18 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xc2*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xc3*2 -1));
writeb( 0xee ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xe0*2 ));
writeb( 0xeb ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xe1*2 -1));
writeb( 0x30 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xe2*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xe3*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xe4*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xe5*2 -1));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xe6*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xe7*2 -1));
writeb( 0x04 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xe8*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xe9*2 -1));
writeb( 0x1a ,(void*) (MSTAR_RIU_BASE+0x00102300*2+0xa3*2 -1));
writeb( 0x40 ,(void*) (MSTAR_RIU_BASE+0x00100100*2+0x3c*2 ));
writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00100100*2+0x3d*2 -1));
#endif

}

static void _mdrv_Mstar_U3phy_MS28_poweroff(uintptr_t U3PHY_D_base, uintptr_t U3PHY_A_base)
{
    writeb(readb((void*)(U3PHY_A_base + (0x1D<<2)))   | 0x02,    (void*)(U3PHY_A_base + (0x1D<<2)));   // USB3_A_B1+1d bit1 =1
    writeb(readb((void*)(U3PHY_A_base + (0x01<<2)+1)) | 0x02,    (void*)(U3PHY_A_base + (0x01<<2)+1)); // USB3_A_B1+01 bit9 =1
    writeb(readb((void*)(U3PHY_A_base + (0x1D<<2)))   | 0x20,    (void*)(U3PHY_A_base + (0x1D<<2)));   // USB3_A_B1+1d bit5 =1
    writeb(readb((void*)(U3PHY_A_base + (0x01<<2)))   | 0xff,    (void*)(U3PHY_A_base + (0x01<<2)));   // USB3_A_B1+01 bit0~7 =ff

    writeb(readb((void*)(U3PHY_A_base + (0x1D<<2)))   | 0x01,    (void*)(U3PHY_A_base + (0x1D<<2)));   // USB3_A_B1+1d bit0 =1
    writeb(readb((void*)(U3PHY_A_base + (0x00<<2)+1)) | 0x02,    (void*)(U3PHY_A_base + (0x00<<2)+1)); // USB3_A_B1+00 bit9 =1
    writeb(readb((void*)(U3PHY_A_base + (0x1D<<2)))   | 0x10,    (void*)(U3PHY_A_base + (0x1D<<2)));   // USB3_A_B1+1d bit4 =1
    writeb(readb((void*)(U3PHY_A_base + (0x00<<2)))   | 0xff,    (void*)(U3PHY_A_base + (0x00<<2)));   // USB3_A_B1+00 bit0~7 =ff

    writeb(readb((void*)(U3PHY_A_base + (0x1D<<2)))   | 0x40,    (void*)(U3PHY_A_base + (0x1D<<2)));   // USB3_A_B1+1d bit6  =1
    writeb(readb((void*)(U3PHY_A_base + (0x00<<2)+1)) | 0x40,    (void*)(U3PHY_A_base + (0x00<<2)+1)); // USB3_A_B1+00 bit14 =1
    writeb(readb((void*)(U3PHY_A_base + (0x1D<<2)))   | 0x40,    (void*)(U3PHY_A_base + (0x1D<<2)));   // USB3_A_B1+1d bit6  =1
    writeb(readb((void*)(U3PHY_A_base + (0x00<<2)+1)) | 0x20,    (void*)(U3PHY_A_base + (0x00<<2)+1)); // USB3_A_B1+00 bit13 =1

    writeb(readb((void*)(U3PHY_A_base + (0x1D<<2)+1)) | 0x01,    (void*)(U3PHY_A_base + (0x1D<<2)+1)); // USB3_A_B1+1d bit8  =1
    writeb(readb((void*)(U3PHY_A_base + (0x01<<2)+1)) | 0x40,    (void*)(U3PHY_A_base + (0x01<<2)+1)); // USB3_A_B1+01 bit14 =1
    writeb(readb((void*)(U3PHY_A_base + (0x1D<<2)))   | 0x08,    (void*)(U3PHY_A_base + (0x1D<<2)));   // USB3_A_B1+1d bit3  =1
    writeb(readb((void*)(U3PHY_A_base + (0x01<<2)+1)) | 0x04,    (void*)(U3PHY_A_base + (0x01<<2)+1)); // USB3_A_B1+01 bit10 =1

    writeb(readb((void*)(U3PHY_A_base + (0x1D<<2)+1)) | 0x02,    (void*)(U3PHY_A_base + (0x1D<<2)+1)); // USB3_A_B1+1d bit9  =1
    writeb(readb((void*)(U3PHY_A_base + (0x01<<2)+1)) | 0x04,    (void*)(U3PHY_A_base + (0x01<<2)+1)); // USB3_A_B1+01 bit10 =1
    writeb(readb((void*)(U3PHY_A_base + (0x1D<<2)))   | 0x04,    (void*)(U3PHY_A_base + (0x1D<<2)));   // USB3_A_B1+1d bit2  =1
    writeb(readb((void*)(U3PHY_A_base + (0x00<<2)+1)) | 0x04,    (void*)(U3PHY_A_base + (0x00<<2)+1)); // USB3_A_B1+00 bit10 =1

    writeb(readb((void*)(U3PHY_A_base + (0x40<<2)))   | 0x01,    (void*)(U3PHY_A_base + (0x40<<2)));   // USB3_A_B1+40 bit0  =1
    writeb(readb((void*)(U3PHY_A_base + (0x41<<2)))   & 0xFE,    (void*)(U3PHY_A_base + (0x41<<2)));   // USB3_A_B1+41 bit0  =0
    writeb(readb((void*)(U3PHY_A_base + (0x1A<<2)))   | 0x01,    (void*)(U3PHY_A_base + (0x1A<<2)));   // USB3_A_B1+1a bit0  =1
    writeb(readb((void*)(U3PHY_A_base + (0x1B<<2)))   & 0xFE,    (void*)(U3PHY_A_base + (0x1B<<2)));   // USB3_A_B1+1b bit0  =0

    writeb(readb((void*)(U3PHY_A_base + (0x1A<<2)))   | 0x01,    (void*)(U3PHY_A_base + (0x1A<<2)));   // USB3_A_B1+1a bit0  =1
    writeb(readb((void*)(U3PHY_A_base + (0x1B<<2)))   & 0xFE,    (void*)(U3PHY_A_base + (0x1B<<2)));   // USB3_A_B1+1b bit0  =0
    writeb(readb((void*)(U3PHY_A_base + (0x1A<<2)))   | 0x40,    (void*)(U3PHY_A_base + (0x1A<<2)));   // USB3_A_B1+1a bit6  =1
    writeb(readb((void*)(U3PHY_A_base + (0x1B<<2)))   & 0xDF,    (void*)(U3PHY_A_base + (0x1B<<2)));   // USB3_A_B1+1b bit5  =0

    writeb(readb((void*)(U3PHY_A_base + (0x36<<2)+1)) & 0xBF,    (void*)(U3PHY_A_base + (0x36<<2)+1)); // USB3_A_B1+36 bit14 =0
    writeb(readb((void*)(U3PHY_A_base + (0x36<<2)+1)) & 0x7F,    (void*)(U3PHY_A_base + (0x36<<2)+1)); // USB3_A_B1+36 bit15 =0
}

uint32_t _mdrv_check_LinkTrainingComplete(void)
{
    uint16_t tmp16 = 0, i=0;
    uint16_t tmp16_2 = 0;
    uint32_t count = 5;			// link check 5 times max, +10ms per step
    uint32_t tmp32 = 0;
    uint32_t retrain_count = 0;
    uint32_t delay_time=0;

    do
    {

ReTrain:
        msleep(delay_time);
        tmp32 = Read32(REG_LOCAL_MANAGMENT + 0x208); //i_debug_mux_control_reg
        tmp32 |= 0x05;
        Write32(REG_LOCAL_MANAGMENT + 0x208, tmp32);

        Write16(REG_LATCH_EN_CTRL, REG_LINK_STATUS_LATCH_EN);
        tmp16 = Read16(REG_LINK_STATUS);
        tmp16 = tmp16&REG_LINK_STATUS_MASK;

        tmp16_2 = Read16(MSTAR_PCIE_U3PHY_P1_ATOP_BASE+0x3E*2);

        //tmp32 = Read32(REG_LOCAL_MANAGMENT+0x20C);
        //printk ("     _mdrv_check_LinkTrainingComplete() i=%d , tmp16=0x%X, tmp16_2 = 0x%X \n",i, tmp16, tmp16_2);

        if (delay_time < 50)		// increase 10ms
            delay_time += 10;

        i++;
        if(i == count)
          break;
    }while(tmp16 != DL_INITIALIZATION_COMPLETED);

    if(i == count)
    {
        tmp32 = Read32(REG_LOCAL_MANAGMENT);
        //printk ("     _mdrv_check_LinkTrainingComplete() i_pl_config_0_reg = 0x%x\n", tmp32);
        //if((tmp32&0x01) == 0)

        //tmp32 = Read32(CPDREF_RP_BASE+0x110);
        //printk ("     _mdrv_check_LinkTrainingComplete() i_corr_err_status = 0x%x\n", tmp32);

        //tmp32 = Read32(CPDREF_RP_BASE+0x20C);
        //printk ("     _mdrv_check_LinkTrainingComplete() i_local_error_status_register = 0x%x\n", tmp32);

        //Reset and ReTrain  , WorkAround
        Write16(REG_RC_RST_CTRL, 0x7);
        msleep(100);
        Write16(REG_RC_RST_CTRL, 0x0);
        Write16(REG_LINK_TRAINING_ENABLE, 0x1);

        i = 0;
        retrain_count++;

        if(retrain_count == 5)		// max retry 5 times
        {
            printk("     pci-e link training failed.\n");
            return -1;
        }
        else
        {
            delay_time=0;
            goto ReTrain;
        }
    }
    printk("     pci-e link training done. %d\n", retrain_count);
    return 0;
}

uint32_t _mdrv_pcie_power_onoff(bool bflag)
{
    if(bflag == 1) //power on
    {
        int ret_val = 0;

        _mdrv_Mstar_U3phy_MS28_init(MSTAR_PCIE_U3PHY_P1_DTOP_BASE, MSTAR_PCIE_U3PHY_P1_ATOP_BASE);
        Write16(REG_RC_RST_CTRL, 0x00);
        Write16(REG_LINK_TRAINING_ENABLE, 0x1);
            //Polling link status register until training finished
        ret_val = _mdrv_check_LinkTrainingComplete();
        if(ret_val != 0)
        {
          printk ("     _mdrv_check_LinkTrainingComplete() Fail !\n");
          return ret_val;
        }
        printk ("     _mdrv_pcie_power_onoff() Power On !\n");
    }
    else
    {
        _mdrv_Mstar_U3phy_MS28_poweroff(MSTAR_PCIE_U3PHY_P1_DTOP_BASE, MSTAR_PCIE_U3PHY_P1_ATOP_BASE);
        Write16(REG_RC_RST_CTRL, 0x3F);
        printk ("     _mdrv_pcie_power_onoff() Power Off !\n");
    }

    return 0;
}

uint32_t _mdrv_pcie_reset_link(void)
{
    uint32_t ret_val = 0;
    Write16(REG_RC_RST_CTRL, 0x7);
    msleep(100);
    Write16(REG_RC_RST_CTRL, 0x0);
    Write16(REG_LINK_TRAINING_ENABLE, 0x1);

    //Polling link status register until training finished
    ret_val = _mdrv_check_LinkTrainingComplete();
    if(ret_val != 0)
    {
      printk ("     _mdrv_check_LinkTrainingComplete() Fail !\n");
      return ret_val;
    }

    return 0;
}

#if (0)
static void _mdrv_Mstar_U3phy_testbus_setting(void)
{
    //wriu 0x101ea1 0x00  //reg_allpad_in
    //wriu 0x101e24 0x10  // [5:4]=reg_test_out_mode
    //wriu 0x101e25 0x00
    //wriu 0x101eea 0x00  //
    //wriu 0x101eeb 0x40  // [14]=reg_testbus_en
    writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00101E00*2+0xa1*2 -1));
    writeb( 0x10 ,(void*) (MSTAR_RIU_BASE+0x00101E00*2+0x24*2 ));
    writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00101E00*2+0x25*2 -1));
    writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00101E00*2+0xea*2 ));
    writeb( 0x40 ,(void*) (MSTAR_RIU_BASE+0x00101E00*2+0xeb*2 -1));

    //wriu 0x101e16 0x00
    //wriu 0x101e17 0x00
    //wriu 0x101ea0 0x00
    //wriu 0x101ea1 0x00
    writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00101E00*2+0x16*2 ));
    writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00101E00*2+0x17*2 -1));
    writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00101E00*2+0xa0*2 ));
    writeb( 0x00 ,(void*) (MSTAR_RIU_BASE+0x00101E00*2+0xa1*2 -1));

    // Select P0 or P1
    //wriu 0x101eee 0x0e    //reg_test_bus24b_sel, P0

    //wriu 0x101eee 0x0f      //reg_test_bus24b_sel, P1
    writeb( 0x0F ,(void*) (MSTAR_RIU_BASE+0x00101E00*2+0xEE*2));

    // debug pipe_phy_ctrl of P1
    //wriu 0x1022d5 0x09    // [12:8]=reg_testbus_sel

    // debug pipe_in of P1
    //wriu 0x1022d5 0x11    // [12:8]=reg_testbus_sel
    //writeb( 0x17 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xD5*2 -1));
    // debug pipe_in of P1
    //wriu 0x1022d5 0x13  // [12:8]=reg_testbus_sel
    writeb( 0x19 ,(void*) (MSTAR_RIU_BASE+0x00102200*2+0xD5*2 -1));
}
#endif

uint32_t _mdrv_pcie_ASPM_onoff(bool bflag)
{
    uint32_t tmp;

    if(bflag == 1) //ASPM on
    {
        tmp = Read32(CPDREF_RP_BASE+0xD0); //bit[0~1] = 0x3
        Write32(CPDREF_RP_BASE+0xD0, tmp | 0x3);
        printk ("     _mdrv_pcie_ASPM_onoff() On !\n");
    }
    else
    {
        tmp = Read32(CPDREF_RP_BASE+0xD0); //bit[0~1] = 0x0
        Write32(CPDREF_RP_BASE+0xD0, tmp & 0xFFFFFFFC);
        printk ("     _mdrv_pcie_ASPM_onoff() Off !\n");
    }

    return 0;

}

uint32_t _mdrv_pcie_legacy_onoff(bool bflag)
{

    if(bflag == 1) //legacy Enable
    {
        Write16(REG_INT_MSK1, 0x00F0);
        printk ("     _mdrv_pcie_legacy_onoff() LEGACY Enable !\n");
    }
    else
    {
        Write16(REG_INT_MSK1, 0x00FF);
        printk ("     _mdrv_pcie_legacy_onoff() LEGACY Disable !\n");
    }

    return 0;
}

uint32_t _mdrv_pcie_resume(void)
{
    int ret;

    printk ("     ===============  PCIE Resume Log   =============== \n");
    ret = _mdrv_system_init();

    if(ret == 0)
       printk ("     PCIE HW Init Success !\n");
    else
      {
           printk ("     PCIE HW Init Fail !\n");
           return EINVAL;
      }

    /**
     * Call core driver API - init for RP
     */
    printk ("     Initializing core driver\n");

    ret = CPDREF_ProbeAndInitRPDriver((void *)&g_rc_data, sizeof(g_rc_data));

    if (ret) {
        printk ("    >> cpdLnx/Error! Unable to initialize the core-driver\n");
        return -EINVAL;
    }

    //Unmask int_inta_out and int_msg_rcv must mask, because the 0x1F202174 (Sysyem Intr) will Or int_inta_out and int_msg_rcv
    Write16(REG_INT_MSK0, REG_INT_MSK0_MASK);
    Write16(REG_INT_MSK1, 0x00F0);

   return 0;
}

#ifdef CONFIG_MSTAR_K6
uint32_t _mdrv_pcie_change_speed(uint32_t speed)
{
    uint32_t tmp = 0;
    uint32_t count = 0;

    //speed = 1 is 5G, speed = 0 is 2.5G
    if(speed  > 1)
      return -EINVAL;

    /***** Change Speed *****/
    Write16(REG_PCIE_GENERATION_SEL, speed);
    /***** Set Target Link Speed to 5GT/s Gen2*****/
    Write32(CPDREF_RP_BASE + 0xf0, 0x2);
    /***** Retrain Link *****/
    Write32(CPDREF_RP_BASE + 0xd0, 0x20);  //bit[5] : Retrain Link

    do
    {
        mdelay(1);
        tmp = Read32(CPDREF_RP_BASE + 0xd0);
        tmp = tmp & 0x000F0000;   // bit[19:16] is Negotiated Link Speed
        printk ("     _mdrv_pcie_change_speed(),  tmp = 0x%x\n", tmp);
        count++;

        if(count ==30)
          break;
    }while(tmp != (0x10000 <<speed));       // bit[16] is Gen1, bit[17] is Gen2

    return 0;
}

uint32_t _mdrv_RC_outbound_DM_read(void *pD, uint32_t addr_offset)
{
    //printk ("     _mdrv_RC_outbound_DM_read() offset = 0x%08x\n", REG_PCIE_DM_BASE+addr_offset);
    return readl(reg_dm_base + addr_offset);
}

void _mdrv_RC_outbound_DM_write(void *pD, uint32_t addr_offset, uint32_t value)
{
    writel(value, reg_dm_base + addr_offset);
}
#endif


#ifdef PCIE_ENABLE_PPC
/**
	static void mdrv_enable_ppc(uintptr_t U3TOP_base)
	\desc pcie port power control
		  U3TOP_base[0xEE:0xF1] Power control GPIO
		  U3TOP_base[0xF2:0xF5] PERST GPIO
*/
uint32_t mdrv_pcie_enable_ppc(uintptr_t U3TOP_base)
{
	u16 offset;
	u16 addr_w, bit_num;
	uintptr_t addr;
	u8  value, low_active;

		// pcie power control GPIO offset
		offset = PCIE_POWERCTRL_GPIO_OFFSET;
		addr_w = readw((void*)(U3TOP_base+(offset+0)*2));
		addr = (uintptr_t)addr_w << 8;
		addr_w = readw((void*)(U3TOP_base+(offset+2)*2));
		addr |= addr_w & 0xFF;

		bit_num = (addr_w >> 8) & 0x7;
		low_active = (u8)((addr_w >> 8) & 0x8);

		if (addr)
		{
			printk("[PCIE] Turn on PCIE port power \n");
			printk("       Addr: 0x%lx bit_num: %d low_active:%d\n", addr, bit_num, low_active);

			value = (u8)(1 << bit_num);

			if (low_active)
			{
				// assert
				if (addr & 0x1)
					writeb(readb((void*)(_MSTAR_PM_BASE+addr*2-1)) & (u8)(~value), (void*)(_MSTAR_PM_BASE+addr*2-1));
				else
					writeb(readb((void*)(_MSTAR_PM_BASE+addr*2)) & (u8)(~value), (void*)(_MSTAR_PM_BASE+addr*2));
			}
			else
			{
				// deassert
				if (addr & 0x1)
					writeb(readb((void*)(_MSTAR_PM_BASE+addr*2-1)) | value, (void*)(_MSTAR_PM_BASE+addr*2-1));
				else
					writeb(readb((void*)(_MSTAR_PM_BASE+addr*2)) | value, (void*)(_MSTAR_PM_BASE+addr*2));
			}

			return addr;
		}
	return 0;
}

uint32_t mdrv_pcie_perst_deassert(uintptr_t U3TOP_base)
{
	u16 offset;
	u16 addr_w, bit_num;
	uintptr_t addr;
	u8  value, low_active;

		// pcie power control GPIO offset
		offset = PCIE_PERST_GPIO_OFFSET;
		addr_w = readw((void*)(U3TOP_base+(offset+0)*2));
		addr = (uintptr_t)addr_w << 8;
		addr_w = readw((void*)(U3TOP_base+(offset+2)*2));
		addr |= addr_w & 0xFF;

		bit_num = (addr_w >> 8) & 0x7;
		low_active = (u8)((addr_w >> 8) & 0x8);

		if (addr)
		{
			printk("[PCIE] PCIE PERST on\n");
			printk("       Addr: 0x%lx bit_num: %d low_active:%d\n", addr, bit_num, low_active);

			value = (u8)(1 << bit_num);

			if (!low_active)
			{
				// deassert
				if (addr & 0x1)
					writeb(readb((void*)(_MSTAR_PM_BASE+addr*2-1)) & (u8)(~value), (void*)(_MSTAR_PM_BASE+addr*2-1));
				else
					writeb(readb((void*)(_MSTAR_PM_BASE+addr*2)) & (u8)(~value), (void*)(_MSTAR_PM_BASE+addr*2));
			}
			else
			{
				// assert
				if (addr & 0x1)
					writeb(readb((void*)(_MSTAR_PM_BASE+addr*2-1)) | value, (void*)(_MSTAR_PM_BASE+addr*2-1));
				else
					writeb(readb((void*)(_MSTAR_PM_BASE+addr*2)) | value, (void*)(_MSTAR_PM_BASE+addr*2));
			}

			return addr;
		}
	return 0;
}

#endif

#if (defined(CONFIG_MSTAR_PCIE_MIU01_DYNAMIC_SELECTION) || defined(CONFIG_MSTAR_PCIE_HW_VAPA_ADDRESS_TRANSLATION))
void pcie_hw_eco_set_reg(uint32_t reg_off_mask, uint32_t reg_val)
{
	writew(((readw((void *)REG_BRI_DUMMY_REG1) & reg_off_mask) | reg_val), (void *)REG_BRI_DUMMY_REG1);
}
#endif


uint32_t _mdrv_system_init(void)
{
    uint32_t ret_val = 0;
    //uint32_t tmp;


    // PCI-e Power control, turn-on power
#ifdef PCIE_ENABLE_PPC
    if (mdrv_pcie_enable_ppc(_MSTAR_U3TOP_BASE))
		pr_info("[PCIE] power on\n");
	else
#endif
	{
	    writeb(readb((void*)(MSTAR_REG_000F_BANK+0x6*2)) & 0xfe,      (void*)(MSTAR_REG_000F_BANK+0x6*2));  //GPIO_PM 3(3.3V) set low for Wifi Module power
	    writeb(readb((void*)(MSTAR_REG_000F_BANK+0x6*2)) & 0xfd,      (void*)(MSTAR_REG_000F_BANK+0x6*2));
	}

#ifdef CONFIG_MSTAR_K6
    _mdrv_Mstar_phy_port1_init(MSTAR_PCIE_U3PHY_P1_DTOP_BASE,  MSTAR_PCIE_U3PHY_P1_ATOP_BASE);
#endif
    //_mdrv_Mstar_U3phy_testbus_setting();
    // PCIE/USB3 PHY initial setting
    _mdrv_Mstar_U3phy_MS28_init(MSTAR_PCIE_U3PHY_P1_DTOP_BASE, MSTAR_PCIE_U3PHY_P1_ATOP_BASE);

    /*
        Reset disabled of PCIe RC
        Enable RC ip-sapce mem-space bus_master so that the core will enter D0 active
    */
    Write16(REG_RC_RST_CTRL, 0x01);
    Read16(REG_RC_RST_CTRL);
    Write16(REG_RC_RST_CTRL, 0x00);
    Write16(REG_LINK_TRAINING_ENABLE, 0x1);

    Write32(REG_LOCAL_MANAGMENT, 0x40); //i_pl_config__reg b[6]=>Phy Error Reporting Enable

    mdelay(10);
#ifdef CONFIG_MSTAR_KANO
#ifdef PCIE_ENABLE_PPC
	if (mdrv_pcie_perst_deassert(_MSTAR_U3TOP_BASE))
		pr_info("[PCIE] PERST deassert\n");
	else
#endif
	{
	    writeb(readb((void*)(MSTAR_REG_000F_BANK+0x0*2)) & 0xfe,      (void*)(MSTAR_REG_000F_BANK+0x0*2));  //GPIO_PM 0(PERST) set high
	    writeb(readb((void*)(MSTAR_REG_000F_BANK+0x0*2)) | 0x2,       (void*)(MSTAR_REG_000F_BANK+0x0*2));
	}
#endif
#ifdef CONFIG_MSTAR_K6
#ifdef PCIE_ENABLE_PPC
	if (mdrv_pcie_perst_deassert(_MSTAR_U3TOP_BASE))
		pr_info("[PCIE] PERST deassert\n");
	else
#endif
	{
		writeb(readb((void*)(MSTAR_REG_0014_BANK+0x23*2-1)) & 0xfe,      (void*)(MSTAR_REG_0014_BANK+0x23*2-1));  //SAR0(PERST#) output
		writeb(readb((void*)(MSTAR_REG_0014_BANK+0x24*2))   | 0x01,      (void*)(MSTAR_REG_0014_BANK+0x24*2));    // drive high, reset end
	}

	//suggested by analog team after measuring phy Tx/Rx
	writeb(0x18, (void *)(MSTAR_PCIE_U3PHY_P1_ATOP_BASE + (0x62<<1)));		// de-emphasis adjustment
	writeb(0x58, (void *)(MSTAR_PCIE_UTMISS1_BASE + (0x2C<<1)));			// PCIE Tx current adjustment
	writeb(0x01, (void *)(MSTAR_PCIE_U3PHY_P1_DTOP_BASE + (0xC0<<1)));		// adjust synth freq
	writeb(0x90, (void *)(MSTAR_PCIE_U3PHY_P1_DTOP_BASE + (0xC1<<1)-1));
	writeb(0x18, (void *)(MSTAR_PCIE_U3PHY_P1_DTOP_BASE + (0xC2<<1)));
	if(!reg_dm_base){
	    reg_dm_base = ioremap(REG_PCIE_DM_BASE, RIU_PCIE_DM_SIZE); //Direct memory mapping
	}
#endif

#ifdef CONFIG_MSTAR_PCIE_MIU01_DYNAMIC_SELECTION
	pcie_hw_eco_set_reg(ECO_MIU_SEL_OFF_MASK, ECO_MIU_SEL_VAL);
#endif
#ifdef CONFIG_MSTAR_PCIE_HW_VAPA_ADDRESS_TRANSLATION
	pcie_hw_eco_set_reg(ECO_HW_VAPA_ADR_TRANS_OFF_MASK, ECO_HW_VAPA_ADR_TRANS_VAL);
#endif

    //Write32(REG_LOCAL_MANAGMENT+4, 0xFF00); //i_pl_config_1_reg b[15:8]=>NFTS count at2.5G Speed
    //Write32(REG_LOCAL_MANAGMENT+ 0x210, 0xC0000); //Enable Local_error
    //Write32(CPDREF_RP_BASE+0x04, 0x7);  //Enable Bus-Master , IO-Space , Mem-Space

    //****enable ASPM L0s. and retrain link
    //Write32(CPDREF_RP_BASE+0xd0, 0x00000021);

    //Polling link status register until training finished
    ret_val = _mdrv_check_LinkTrainingComplete();
    if(ret_val != 0)
    {
      printk ("     _mdrv_check_LinkTrainingComplete() Fail !\n");
      return ret_val;
    }

    //Disable INT0 & INT1 MASK
    Write16(REG_INT_MSK0, 0x0000);
    Write16(REG_INT_MSK1, 0x0000);

    Write32(CPDREF_RP_BASE + 0x18, 0x010100); //i_debug_mux_control_reg


    //Set the Axi2Brige setting
    Write16(REG_64B_WFUL_ON, 0x0);
    Write16(REG_G3D_RREQ_RD_LEN, 0x148);
    //Set the MAX 256 Payload Size
   //tmp = Read32(CPDREF_RP_BASE+0xC8); //bit[5] = 1 means 256 bytes
   // Write32(CPDREF_RP_BASE+0xC8, tmp | 0x20);

    //Write32(CPDREF_RP_BASE+ 0x14, 0x400000);
    Write32(CPDREF_RP_BASE_RO_MODIFY+ 0x08, 0x06040100);


#ifdef CONFIG_MSTAR_K6
    //Change Speed to 5G
    ret_val = _mdrv_pcie_change_speed(1);
    if(ret_val != 0)
    {
      printk ("     _mdrv_pcie_change_speed() Fail at Gen2 and Change To Gen1 \n");
      ret_val = _mdrv_pcie_reset_link();
    }
#endif


    return 0;
}
