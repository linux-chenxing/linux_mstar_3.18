

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
#include <asm/delay.h>

#include "mhal_rtc.h"
#include "mdrv_rtc.h"


void MDrv_RTC_Init(PM_RtcParam *pPmRtcParam)
{
   printk(KERN_EMERG "==RTC== %s, %d\n" , __FUNCTION__, __LINE__);
   MHAL_RTC_Init(pPmRtcParam->u8PmRtcIndex, pPmRtcParam->u32RtcCtrlWord);
}

void MDrv_RTC_SetCount(PM_RtcParam *pPmRtcParam)
{
   //printk(KERN_EMERG "==RTC== %s, %d\n" , __FUNCTION__, __LINE__);
   MHAL_RTC_SetCounter(pPmRtcParam->u8PmRtcIndex, pPmRtcParam->u32RtcSetCounter);
}

U32 MDrv_RTC_GetCount(PM_RtcParam *pPmRtcParam)
{
    //printk(KERN_EMERG "==RTC== %s, %d\n" , __FUNCTION__, __LINE__);
    return MHAL_RTC_GetCounter(pPmRtcParam->u8PmRtcIndex);
}

void MDrv_RTC_SetMatchCount(PM_RtcParam *pPmRtcParam)
{
   //printk(KERN_EMERG "==RTC== %s, %d\n" , __FUNCTION__, __LINE__);
   MHAL_RTC_SetMatchCounter(pPmRtcParam->u8PmRtcIndex, pPmRtcParam->u32RtcSetMatchCounter);
}

U32 MDrv_RTC_GetMatchCount(PM_RtcParam *pPmRtcParam)
{
    //printk(KERN_EMERG "==RTC== %s, %d\n" , __FUNCTION__, __LINE__);
    return MHAL_RTC_GetMatchCounter(pPmRtcParam->u8PmRtcIndex);
}

