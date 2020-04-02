#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/of.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <mach/io.h>
#include <asm/setup.h>
#include <chip_int.h>
#include <irqchip.h>
#include <mstar/mpatch_macro.h>

#if (MP_PLATFORM_INT_1_to_1_SPI == 1)
/*
 * 2 bits/per interrupt
 * b'00: level
 * b'10: edge
 */
/*irq 181 190 199 208 is active low, someone who want to use need to set reg to convert the signal*/
unsigned int interrupt_configs[MSTAR_CHIP_INT_END / 16] =
{
    0x00000000, /*   0~ 15 sgi, don't care */
    0x00000000, /*  16~ 31 ppi, don't care */
    0x00000000, /*  32~ 47 spi, set level for mstar irq */
    0x00000000, /*  64~ 79 spi, set level for mstar irq */
    0x00000000, /*  64~ 79 spi, set level for mstar irq */
    0x00000000, /*  80~ 95 spi, set level for mstar irq */
    0x00000000, /*  96~111 spi, set level for mstar fiq */
    0x00000000, /* 112~127 spi, set level for mstar fiq */
    0x00000000, /* 128~143 spi, set level for mstar fiq */
    0x00000000, /* 144~159 spi, set level for mstar fiq */
    0xA2A00000, /* 160~175 spi, set edge/level for mstar fiq */
    0x0AAA82AA, /* 176~191 spi, set edge/level for mstar fiq */
                /* set the rest by init_chip_spi_config() */
};

//array index is SPI number,value is vector table number
unsigned int spi_to_ppi[NR_IRQS]=
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    //mstar IRQ/IRQEXP(mstar 64-127 ,SPI 32-95) 
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,  
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,  
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,  
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,  

    //mstar FIQ/FIQEXP(mstar 0-63, SPI 96-159)
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,  
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,  
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,  
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,  

    //not used mstar IRQ/FIQ number
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

    //mstar IRQHYP(mstar 320-339,SPI 210-229)
    0x140,0x141,0x142,0x143,0x144,0x145,0x146,0x147,0x148,0x149,0x14A,0x14B,0x14C,0x14D,0x14E,0x14F,     
    0x150,0x151,0x152,0x153, 

    //mstar FIQHYP(mstar 256-262,SPI 230-236)  
    0x100,0x101,0x102,0x103,0x104,0x105,0x106, 

};


#define BIT_PER_IRQ     2
#define IRQ_PER_UINT    16
#define EDGE            2
#define LEVEL           0

static inline void set_edge(unsigned int irq)
{
    interrupt_configs[irq/IRQ_PER_UINT] |= (EDGE << ((irq % IRQ_PER_UINT) * BIT_PER_IRQ));
}

static inline void set_level(unsigned int irq)
{
    interrupt_configs[irq/IRQ_PER_UINT] |= (LEVEL << ((irq % IRQ_PER_UINT) * BIT_PER_IRQ));
}
void init_chip_spi_config(void)
{
    int i = 0;
#if 0
    set_edge(170); /* clock switch interrupt */
    set_edge(171); /* riu/xiu timerout interrupt */
    set_edge(172); /* scu event abort interrupt */

    /* neon/fpu exception flag */
    set_edge(174);
    set_edge(175);
    set_edge(176);
    set_edge(177);
    set_edge(178);
    set_edge(179);
    set_edge(180);

    /* neon/fpu exception flag */
    set_edge(183);
    set_edge(184);
    set_edge(185);
    set_edge(186);
    set_edge(187);
    set_edge(188);
    set_edge(189);
#endif
    /* neon/fpu exception flag */
    set_edge(192);
    set_edge(193);
    set_edge(194);
    set_edge(195);
    set_edge(196);
    set_edge(197);
    set_edge(198);

    /* neon/fpu exception flag */
    set_edge(201);
    set_edge(202);
    set_edge(203);
    set_edge(204);
    set_edge(205);
    set_edge(206);
    set_edge(207);

    /*
    208:CTI exception flag(active low),209: PMU exception event flag, 210 - 229:HYP IRQ
    230 - 236: HYP FIQ, 237: SUP IRQ
    */
    for(i = 208; i <= 237; i++ )
    set_level(i);
}
#endif/*MP_PLATFORM_INT_1_to_1_SPI*/

/* Clear FIQ (Clear is not supported for IRQ) */
void chip_irq_ack(unsigned int irq)
{
    u16 tmp = 0;

    if(irq < 16)
    {
        tmp = (0x01 << irq);
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x2c << 2)) );
    }
    else if((irq >= 16) && (irq < 32))
    {
        tmp = (0x01 << (irq - 16));
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x2d << 2)) );
    }
    else if( (irq >= 32) && (irq < 48))
    {
        tmp = (0x01) << (irq - 32);
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x2e << 2)) );
    }
    else if( (irq >= 48) && (irq < 64))
    {
        tmp = (0x01) << (irq - 48);
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x2f << 2)) );
    }
}

/* Mask IRQ/FIQ */
void chip_irq_mask(unsigned int irq)
{
    u16 tmp = 0;

    if(irq <16)
    {
        tmp = readw_relaxed((void __iomem *)(REG_INT_BASE + (0x24 << 2)));
        tmp |= (0x01) << irq;
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x24 << 2)) );
    }
    else if((irq >= 16) && (irq < 32))
    {
        tmp = readw_relaxed((void __iomem *)(REG_INT_BASE + (0x25 << 2)));
        tmp |= (0x01) << (irq - 16);
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x25 << 2)) );
    }
    else if((irq >= 32) && (irq < 48))
    {
        tmp = readw_relaxed((void __iomem *)(REG_INT_BASE + (0x26 << 2)));
        tmp |= (0x01) << (irq - 32);
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x26 << 2)) );
    }
    else if((irq >= 48) && (irq < 64))
    {
        tmp = readw_relaxed((void __iomem *)(REG_INT_BASE + (0x27 << 2)));
        tmp |= (0x01) << (irq - 48);
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x27 << 2)) );
    }
    else if((irq >= 64) && (irq < 80))
    {
        tmp = readw_relaxed((void __iomem *)(REG_INT_BASE + (0x34 << 2)));
        tmp |= (0x01) << (irq - 64);
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x34 << 2)) );
    }
    else if((irq >= 80) && (irq < 96))
    {
        tmp = readw_relaxed((void __iomem *)(REG_INT_BASE + (0x35 << 2)));
        tmp |= (0x01) << (irq - 80);
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x35 << 2)) );
    }
    else if((irq >= 96) && (irq < 112))
    {
        tmp = readw_relaxed((void __iomem *)(REG_INT_BASE + (0x36 << 2)));
        tmp |= (0x01) << (irq - 96);
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x36 << 2)) );
    }
    else if((irq >= 112) && (irq < 128))
    {
        tmp = readw_relaxed((void __iomem *)(REG_INT_BASE + (0x37 << 2)));
        tmp |= (0x01) << (irq - 112);
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x37 << 2)) );
    }

}

/* Un-Mask IRQ/FIQ */
void chip_irq_unmask(unsigned int irq)
{
    u16 tmp = 0;

    //printk(KERN_WARNING "chip_irq_unmask(irq=%d)\n",irq);

    if(irq < 16)
    {
        tmp = readw_relaxed((void __iomem *)(REG_INT_BASE + (0x24 << 2)));
        tmp &= ~((0x01) << irq);
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x24 << 2)) );
    }
    else if((irq >= 16) && (irq < 32))
    {
        tmp = readw_relaxed((void __iomem *)(REG_INT_BASE + (0x25 << 2)));
        tmp &= ~((0x01) << (irq - 16));
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x25 << 2)) );
    }
    else if((irq >= 32) && (irq < 48))
    {
        tmp = readw_relaxed((void __iomem *)(REG_INT_BASE + (0x26 << 2)));
        tmp &= ~((0x01) << (irq - 32));
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x26 << 2)) );
    }
    else if((irq >= 48) && (irq < 64))
    {
        tmp = readw_relaxed((void __iomem *)(REG_INT_BASE + (0x27 << 2)));
        tmp &= ~((0x01) << (irq - 48));
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x27 << 2)) );
    }
    else if((irq >= 64) && (irq < 80))
    {
        tmp = readw_relaxed((void __iomem *)(REG_INT_BASE + (0x34 << 2)));
        tmp &= ~((0x01) << (irq - 64));
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x34 << 2)) );
    }
    else if((irq >= 80) && (irq < 96))
    {
        tmp = readw_relaxed((void __iomem *)(REG_INT_BASE + (0x35 << 2)));
        tmp &= ~((0x01) << (irq - 80));
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x35 << 2)) );
    }
    else if((irq >= 96) && (irq < 112))
    {
        tmp = readw_relaxed((void __iomem *)(REG_INT_BASE + (0x36 << 2)));
        tmp &= ~((0x01) << (irq - 96));
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x36 << 2)) );
    }
    else if((irq >= 112) && (irq < 128))
    {
        tmp = readw_relaxed((void __iomem *)(REG_INT_BASE + (0x37 << 2)));
        tmp &= ~((0x01) << (irq - 112));
        writew_relaxed(tmp, (void __iomem *)(REG_INT_BASE + (0x37 << 2)) );
    }
}

/* Since kenrnel run on EL1, it forbidden to access secure bank  */
int __init init_irq_fiq_merge(void)
{
#if 0
    u16 tmp = 0;

    tmp = readw_relaxed((void __iomem *)(mstar_pm_base + (0x123964UL << 1)));
    tmp &= 0xFF8F;
    tmp |= 0x0050;
    writew_relaxed(tmp, (void __iomem *)(mstar_pm_base + (0x123964UL << 1)));
#endif

    return 0;
}

extern int __init gic_of_init(struct device_node *node, struct device_node *parent);
IRQCHIP_DECLARE(mstar_a53_gic, "arm,cortex-a15-gic", gic_of_init);


