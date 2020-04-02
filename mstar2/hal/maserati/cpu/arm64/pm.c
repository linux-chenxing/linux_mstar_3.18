/*-----------------------------------------------------------------------------
    Include Files
------------------------------------------------------------------------------*/
#include <linux/suspend.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/rtc.h>
#include <linux/sched.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/atomic.h>
#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <asm/mach/time.h>
#include <asm/mach/irq.h>
#include <asm/mach-types.h>
#include <asm/smp.h>
#include <mach/pm.h>
#include <mach/io.h>
#include <asm/cputype.h>
#include "chip_int.h"
#include "chip_setup.h"
#include <mstar/mpatch_macro.h>
#include <mach/system.h>
#include <asm/suspend.h>
extern void sleep_save_cpu_registers(void);
extern void sleep_set_wakeup_addr_phy(unsigned long phy_addr, void *virt_addr);
extern void sleep_prepare_last(void);
extern void save_performance_monitors(void *pointer);
extern void restore_performance_monitors(void *pointer);
extern void  MDrv_MBX_NotifyPMtoSetPowerOff(void);
extern void  MDrv_MBX_NotifyPMPassword(unsigned char passwd[16]);
extern int get_str_max_cnt(void);
#define PMREG(a) (*(volatile unsigned short*)(((unsigned long)mstar_pm_base)+(a)))
#define PMREG_BYTEMASK(s) (((unsigned int)0xFF)<<(s))
#define PMREG_BYTECLEAR(v,s) (((unsigned int)(v))&~(PMREG_BYTEMASK(s)))
#define PMREG_BYTE(v,s1,s2) (((((unsigned int)(v))>>(s1))&(0xFF))<<(s2))
#define PMREG_MAKVAL(v1,s1,v2,s2) (PMREG_BYTECLEAR(v1,s1)|PMREG_BYTE(v2,s2,s1))
#define PMON_SAVE_LONG_CNT 36
static unsigned long pmon_save_buf[PMON_SAVE_LONG_CNT];
DEFINE_SPINLOCK(ser_printf_lock);
static unsigned char pass_wd[16]={0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,
                                   0x11,0x00,0xFF,0xEE,0xDD,0xCC,0xBB,0xAA};

static int mstr_cnt=0;
static int pre_str_max_cnt=0;
static u32 MStar_IntMaskSave[8];
static u32 MStar_HypIntMaskSave[8];

void SerPrintChar(char ch)
{
    __asm__ volatile (
        "MOV X5, #0x2013\n"
        "LSL X5, X5, #8\n"
        "LDR X4, %0\n"
        "ADD X5, X5, X4\n"
        "1: LDR W4, [X5, #0x28]\n"
        "TST W4, #0x20\n"
        "BEQ 1b\n"
        "LDRB W4, %1\n"
        "STRB W4,[X5]\n"
        ::"m"(mstar_pm_base),"m"(ch):"r4","r5","cc","memory"
        );
}
void SerPrintStr(char *p)
{
    int nLen=strlen(p);
    int i;
    for(i=0;i<nLen;i++)
    {
        if(p[i]=='\n')SerPrintChar('\r');
        SerPrintChar(p[i]);
    }
}
void SerPrintStrAtomic(char *p)
{
    u_long flag;
    spin_lock_irqsave(&ser_printf_lock,flag);
    SerPrintStr(p);
    spin_unlock_irqrestore(&ser_printf_lock,flag);
}
void SerPrintf(char *fmt,...)
{
    char tmpbuf[500];
    int nLen;
    va_list args;
    va_start(args, fmt);
    nLen=vscnprintf(tmpbuf, 500, fmt, args);
    va_end(args);
    if(nLen<=0)
    {
        nLen=0;
    }
    else if(nLen>=500)
    {
        nLen=500-1;
    }
    tmpbuf[nLen]=0;
    SerPrintStr(tmpbuf);
}
void SerPrintfAtomic(char *fmt,...)
{
    char tmpbuf[500];
    int nLen;
    va_list args;
    va_start(args, fmt);
    nLen=vscnprintf(tmpbuf, 500, fmt, args);
    va_end(args);
    if(nLen<=0)
    {
        nLen=0;
    }
    else if(nLen>=500)
    {
        nLen=500-1;
    }
    tmpbuf[nLen]=0;
    SerPrintStrAtomic(tmpbuf);
}
int vSerPrintf(const char *fmt, va_list args)
{
    char tmpbuf[500];
    int nLen;
    nLen=vscnprintf(tmpbuf, 500, fmt, args);
    if(nLen<=0)
    {
        nLen=0;
    }
    else if(nLen>=500)
    {
        nLen=500-1;
    }
    tmpbuf[nLen]=0;
    SerPrintStr(tmpbuf);
    return nLen;
}
int vSerPrintfAtomic(const char *fmt, va_list args)
{
    char tmpbuf[500];
    int nLen;
    nLen=vscnprintf(tmpbuf, 500, fmt, args);
    if(nLen<=0)
    {
        nLen=0;
    }
    else if(nLen>=500)
    {
        nLen=500-1;
    }
    tmpbuf[nLen]=0;
    SerPrintStrAtomic(tmpbuf);
    return nLen;
}
phys_addr_t mstar_virt_to_phy(void* virtaddr)
{
    phys_addr_t rest=0;
    rest=virt_to_phys(virtaddr);
    return rest;
}

void* mstar_phy_to_virt(phys_addr_t phyaddr )
{
    void *rest=0;
    rest=phys_to_virt(phyaddr);
    return rest;
}

void mstar_sleep_cur_cpu_flush(void)
{
    Chip_Flush_Cache_All_Single();
}
static void mstar_str_notifypmmaxcnt_off(void)
{
    pass_wd[0x0A]=0xFD;
    MDrv_MBX_NotifyPMPassword(pass_wd);
    while(1);
}

#if defined(CONFIG_MSTAR_STR_ACOFF_ON_ERR)
void mstar_str_notifypmerror_off(void)
{
    pass_wd[0x0A]=0xFE;
    MDrv_MBX_NotifyPMPassword(pass_wd);
    while(1);
}
#endif

void mstar_pm_regw(unsigned short val)
{
    unsigned short tmp;
    tmp=PMREG(PMU_WAKEUP_ADDR_REGL);
    tmp=PMREG_MAKVAL(tmp,PMU_WAKEUP_ADDR_LSHIFT,val, 0);
    PMREG(PMU_WAKEUP_ADDR_REGL)=tmp;
    tmp=PMREG(PMU_WAKEUP_ADDR_REGH);
    tmp=PMREG_MAKVAL(tmp,PMU_WAKEUP_ADDR_HSHIFT,val, 8);
    PMREG(PMU_WAKEUP_ADDR_REGH)=tmp;
}
unsigned short mstar_pm_regr(void)
{
    unsigned short tmp,val=0;
    tmp=PMREG(PMU_WAKEUP_ADDR_REGL);
    val |= PMREG_BYTE(tmp,PMU_WAKEUP_ADDR_LSHIFT, 0);
    tmp=PMREG(PMU_WAKEUP_ADDR_REGH);
    val |= PMREG_BYTE(tmp,PMU_WAKEUP_ADDR_HSHIFT, 8);
    return val;
}

void mstar_prepare_secondary(void)
{
    extern int mstar_smp_spin_table_prepare_cpu(int cpu);

    mstar_smp_spin_table_prepare_cpu(1);

}
void mstar_save_int_mask(void)
{
    volatile unsigned long *int_mask_base=(volatile unsigned long *)REG_INT_BASE;
    volatile unsigned long *hypint_mask_base=(volatile unsigned long *)REG_INT_HYP_BASE;
    MStar_IntMaskSave[0]=int_mask_base[0x24];
    MStar_IntMaskSave[1]=int_mask_base[0x25];
    MStar_IntMaskSave[2]=int_mask_base[0x26];
    MStar_IntMaskSave[3]=int_mask_base[0x27];
    MStar_IntMaskSave[4]=int_mask_base[0x34];
    MStar_IntMaskSave[5]=int_mask_base[0x35];
    MStar_IntMaskSave[6]=int_mask_base[0x36];
    MStar_IntMaskSave[7]=int_mask_base[0x37];

    MStar_HypIntMaskSave[0]=hypint_mask_base[0x24];
    MStar_HypIntMaskSave[1]=hypint_mask_base[0x25];
    MStar_HypIntMaskSave[2]=hypint_mask_base[0x26];
    MStar_HypIntMaskSave[3]=hypint_mask_base[0x27];
    MStar_HypIntMaskSave[4]=hypint_mask_base[0x34];
    MStar_HypIntMaskSave[5]=hypint_mask_base[0x35];
    MStar_HypIntMaskSave[6]=hypint_mask_base[0x36];
    MStar_HypIntMaskSave[7]=hypint_mask_base[0x37];
}

//mstar_restore_int_mask use str_gic_dist_base to mask irq 31 in GIC SPI mode
#if (MP_PLATFORM_INT_1_to_1_SPI == 1)
extern void __iomem* str_gic_dist_base;
#endif

void mstar_restore_int_mask(void)
{
#if (MP_PLATFORM_INT_1_to_1_SPI == 1)
    u32 val;
#endif

    volatile unsigned long *int_mask_base=(volatile unsigned long *)REG_INT_BASE;
    volatile unsigned long *hypint_mask_base=(volatile unsigned long *)REG_INT_HYP_BASE;
    int_mask_base[0x24]=MStar_IntMaskSave[0];
    int_mask_base[0x25]=MStar_IntMaskSave[1];
    int_mask_base[0x26]=MStar_IntMaskSave[2];
    int_mask_base[0x27]=MStar_IntMaskSave[3];
    int_mask_base[0x34]=MStar_IntMaskSave[4];
    int_mask_base[0x35]=MStar_IntMaskSave[5];
    int_mask_base[0x36]=MStar_IntMaskSave[6];
    int_mask_base[0x37]=MStar_IntMaskSave[7];

    hypint_mask_base[0x24]=MStar_HypIntMaskSave[0];
    hypint_mask_base[0x25]=MStar_HypIntMaskSave[1];
    hypint_mask_base[0x26]=MStar_HypIntMaskSave[2];
    hypint_mask_base[0x27]=MStar_HypIntMaskSave[3];
    hypint_mask_base[0x34]=MStar_HypIntMaskSave[4];
    hypint_mask_base[0x35]=MStar_HypIntMaskSave[5];
    hypint_mask_base[0x36]=MStar_HypIntMaskSave[6];
    hypint_mask_base[0x37]=MStar_HypIntMaskSave[7];

#if (MP_PLATFORM_INT_1_to_1_SPI == 1)
    val = readl_relaxed(str_gic_dist_base + 0x180 + (INT_PPI_IRQ / 32) * 4);
    val= val | (0x01 << INT_PPI_IRQ );
    writel_relaxed(val, str_gic_dist_base + 0x180 + (INT_PPI_IRQ / 32) * 4);
#endif


}

void mstar_suspend_save(void)
{
    extern ptrdiff_t mstar_pm_base;
	//Ian #7
	int CPU = *((volatile unsigned int*)(mstar_pm_base + (0x100502<<1)));
	SerPrintf("\nMStar STR Suspending...\n");
	SerPrintf("\n  STR Suspending  CPU FREQ %d\n",CPU);
    
	if(pre_str_max_cnt!=get_str_max_cnt())
    {
        pre_str_max_cnt=get_str_max_cnt();
        mstr_cnt=0;
    }
    mstr_cnt++;
    mstar_save_int_mask();
    save_performance_monitors(pmon_save_buf);
}
void mstar_resume_restore(void)
{
#if (MP_CHECKPT_BOOT == 1)
#ifdef CONFIG_MP_PLATFORM_ARM
    unsigned int PiuTick = reg_readw(0x1f006090UL);
    PiuTick += (reg_readw(0x1f006094UL) << 16);
#else
    unsigned int PiuTick = *(volatile unsigned short *)(0xbf006090);
    PiuTick += (*(volatile unsigned short *)(0xbf006094)) << 16;
#endif
    unsigned int PiuTime = PiuTick / 12000;
    SerPrintf("\n[AT][KR][STR Resuming][%u]\n", PiuTime);
#endif
    extern ptrdiff_t mstar_pm_base;
	
	//Ian #7
	int CPU = *((volatile unsigned int*)(mstar_pm_base + (0x100502<<1)));
	SerPrintf("\nMStar STR Resuming...\n");
	SerPrintf("\n  STR Resuming CPU FREQ %d\n",CPU);

    restore_performance_monitors(pmon_save_buf);
#if 0
#if (MP_PLATFORM_CPU_SETTING == 1)
#if defined CONFIG_MSTAR_CPU_calibrating
    //Init Test Bus for CPU Clock Counter
    if(*(volatile u32 *)(mstar_pm_base + (0x100500 << 1)) == 0x3697)
   	{
		SerPrintf("Do Init Test Bus for CPU Clock Counter\n");
		*(volatile u32 *)(mstar_pm_base + 0x101896 *2) =0x01;
		*(volatile u32 *)(mstar_pm_base + 0x101eea *2) =0x00;
		*(volatile u32 *)(mstar_pm_base + 0x101eea *2) =0x04;
		*(volatile u32 *)(mstar_pm_base + 0x101eea *2) =0x4004;
		*(volatile u32 *)(mstar_pm_base + 0x101eee *2) =0x001f;
		*(volatile u32 *)(mstar_pm_base + 0x101e62 *2) = 0;
		*(volatile u32 *)(mstar_pm_base + 0x101e62 *2) =0x01;
   	}
#endif // defined CONFIG_MSTAR_CPU_calibrating
#endif /*MP_PLATFORM_CPU_SETTING*/
#endif
    mstar_restore_int_mask();
    mstar_prepare_secondary();
}
void mstar_str_power_off(void)
{
    SerPrintf("\nMStar STR waiting power off...\n");
    if(get_str_max_cnt()>0 && mstr_cnt>=get_str_max_cnt())
    {
        SerPrintf("Max Cnt Ac off...\n");
        mstar_str_notifypmmaxcnt_off();
    }
    else
    {
        SerPrintf("\ndo MDrv_MBX_NotifyPMtoSetPowerOff\n");
        MDrv_MBX_NotifyPMtoSetPowerOff();
    }
}
/*------------------------------------------------------------------------------
    Function: mstar_pm_enter

    Description:
        Actually enter sleep state
    Input: (The arguments were used by caller to input data.)
        state - suspend state (not used)
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        0
    Remark:
        None.
-------------------------------------------------------------------------------*/
static int mstar_pm_enter(suspend_state_t state)
{
    void *pWakeup=0;
    __asm__ volatile (
        "LDR X1, =MSTAR_WAKEUP_ENTRY\n"
        "STR X1, %0\n"
        :"=m"(pWakeup)::"r1");
    mstar_suspend_save();
    sleep_set_wakeup_addr_phy(mstar_virt_to_phy((void*)pWakeup),(void*)pWakeup);
    sleep_save_cpu_registers();
    sleep_prepare_last();
    mstar_sleep_cur_cpu_flush();
    mstar_str_power_off();
    __asm__ volatile(
        "WAITHERE: B WAITHERE\n"
        "MSTAR_WAKEUP_ENTRY:\n"
        :::"r0","r1","r2","r3","r4","r5","r6","r7","r8","r9","r10","r11","r12","r13","r14","r15","memory","cc"
    );
    mstar_resume_restore();
    mstar_sleep_cur_cpu_flush();
    return 0;
}
static struct platform_suspend_ops mstar_pm_ops =
{
    .enter      = mstar_pm_enter,
    .valid      = suspend_valid_only_mem,
};


/*------------------------------------------------------------------------------
    Function: mstar_pm_init

    Description:
        init function of power management
    Input: (The arguments were used by caller to input data.)
        None.
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        0
    Remark:
        None.
-------------------------------------------------------------------------------*/
static int __init mstar_pm_init(void)
{
    /* set operation function of suspend */
    suspend_set_ops(&mstar_pm_ops);
    return 0;
}

__initcall(mstar_pm_init);

