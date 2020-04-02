#include "drvNAND.h"
#include "drvNAND_utl.h"
#include <linux/version.h>
#include <linux/kthread.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
#include <linux/spinlock.h>
#endif

#if defined(__VER_UNFD_FTL__)&&__VER_UNFD_FTL__


extern void nand_lock_fcie(void);
extern void nand_unlock_fcie(void);


struct unfd_task_queue {
    struct task_struct  *thread;
    unsigned int        flags;
    spinlock_t  lock;
    void            *data;
} *unfd_tq;

void drvNAND_TaskBGTrequest_fn(int flag)
{
    spin_lock_irq(&unfd_tq->lock);

    unfd_tq->flags |= flag;

    spin_unlock_irq(&unfd_tq->lock);
}

void drvNAND_IssueTask(void)
{
    int flag, task_flag = 0;

    spin_lock_irq(&unfd_tq->lock);
    set_current_state(TASK_INTERRUPTIBLE);
    flag = unfd_tq->flags ;
    spin_unlock_irq(&unfd_tq->lock);
    set_current_state(TASK_RUNNING);

    //down(&PfModeSem);
    nand_lock_fcie();
    if(flag & NAND_MSG_WEAR_LEVELING)
    {
        nand_Wear_Leveling();
        task_flag |= NAND_MSG_WEAR_LEVELING;
    }

    if(flag & NAND_MSG_WEAR_LEVELING1)
    {
        nand_Wear_Leveling1();
        task_flag |= NAND_MSG_WEAR_LEVELING1;
    }

    if(flag & NAND_MSG_FORCE_WRITE_BACK)
    {
        NAND_DRIVER *pNandDrv = drvNAND_get_DrvContext_address();
        U8  u8_i;
        for (u8_i=0 ; u8_i<(pNandDrv->u8_Zone1SubZoneCnt+1); u8_i++)
        {
            if(pNandDrv->au32_ZoneTotalECnt[u8_i] > WL_SAVE_EC_TIMES)
                nand_SaveEraseCounter(u8_i);
        }
        for(u8_i=0; u8_i<MAX_WBQ_CNT; u8_i++)
        {
            nand_FlushWBQ(u8_i);
        }
        task_flag |= NAND_MSG_FORCE_WRITE_BACK;
    }

    //up(&PfModeSem);
    nand_unlock_fcie();
    
    flag &= ~task_flag;
    spin_lock_irq(&unfd_tq->lock);
    set_current_state(TASK_INTERRUPTIBLE);
    unfd_tq->flags = flag;
    spin_unlock_irq(&unfd_tq->lock);
    set_current_state(TASK_RUNNING);
}

int drvNAND_Task_BGThread(void *pData)
{
    int flag, u8_i;
    while(1)
    {
        if(drvNAND_ChkRdy(0) ==0)
        {
            msleep(10000);
            continue;
        }
        drvNAND_IssueTask();
        
        msleep(2000);
        spin_lock_irq(&unfd_tq->lock);
        set_current_state(TASK_INTERRUPTIBLE);
        flag = unfd_tq->flags;
        spin_unlock_irq(&unfd_tq->lock);
        set_current_state(TASK_RUNNING);

        if(!(flag & NAND_MSG_STATE_MASK))
        {
            //down(&PfModeSem);
            nand_lock_fcie();
            for(u8_i=0; u8_i<MAX_WBQ_CNT; u8_i++)
            {
                nand_FlushWBQ(u8_i);
            }
            nand_unlock_fcie();
            //up(&PfModeSem);
            msleep(2000);
        }

        if(kthread_should_stop())
        {
            set_current_state(TASK_RUNNING);
            break;
        }
    }
    return 0;
}


int drvNAND_TaskBGT_Init(void)
{
    int ret;
    unfd_tq = kzalloc(sizeof(struct unfd_task_queue), GFP_KERNEL);
    if (!unfd_tq) {
        return -ENOMEM;
    }
    spin_lock_init(&unfd_tq->lock);

    unfd_tq->thread = kthread_run(drvNAND_Task_BGThread, unfd_tq, "unfdBGT");

    if (IS_ERR(unfd_tq->thread)) {
        ret = PTR_ERR(unfd_tq->thread);
        goto out;
    }

    wake_up_process(unfd_tq->thread);

    return 0;
out:
    kfree(unfd_tq);
    return ret;
}
#endif
