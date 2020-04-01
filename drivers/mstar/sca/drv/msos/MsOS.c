////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (?œMStar Confidential Information?? by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
///

#define MS_OS_C

/******************************************************************************/
/*                           Header Files                                     */
/* ****************************************************************************/
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/wait.h>

#include "MsCommon.h"
#include "MsTypes.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "halCHIP.h"


/********************************************************************************/
/*                           Macro                                              */
/********************************************************************************/
#define MSOS_MUTEX_USE_SEM

#define MSOS_ID_PREFIX              0x76540000
#define MSOS_ID_PREFIX_MASK         0xFFFF0000
#define MSOS_ID_MASK                0x0000FFFF //~MSOS_ID_PREFIX_MASK

#define HAS_FLAG(flag, bit)        ((flag) & (bit))
#define SET_FLAG(flag, bit)        ((flag)|= (bit))
#define RESET_FLAG(flag, bit)      ((flag)&= (~(bit)))


#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif // #ifndef MIN

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifdef MSOS_MEMPOOL_MAX
#undef MSOS_MEMPOOL_MAX
#define MSOS_MEMPOOL_MAX 2
#endif // #ifdef MSOS_MEMPOOL_MAX



/********************************************************************************/
/*                           Constant                                           */
/********************************************************************************/


/******************************************************************************/
/*                           Enum                                             */
/******************************************************************************/


/******************************************************************************/
/*                           Global Variables                                   */
/******************************************************************************/
//
// Mutex
//
typedef struct
{
    MS_BOOL             bUsed;
#ifdef MSOS_MUTEX_USE_SEM
    struct semaphore stMutex;
#else
    spinlock_t       stMutex;
#endif
    MS_U8               u8Name[MAX_MUTEX_NAME_LENGTH];
} MsOS_Mutex_Info;

static MsOS_Mutex_Info          _MsOS_Mutex_Info[MSOS_MUTEX_MAX];
static                          DEFINE_SPINLOCK(_MsOS_Mutex_Mutex);
#define MUTEX_MUTEX_LOCK()      spin_lock(&_MsOS_Mutex_Mutex)
#define MUTEX_MUTEX_UNLOCK()    spin_unlock(&_MsOS_Mutex_Mutex)

//
// Task Management
//
typedef struct
{
    MS_BOOL             bUsed;
    struct task_struct* pstThreadInfo;
} MsOS_Task_Info;

static MsOS_Task_Info   _MsOS_Task_Info[MSOS_TASK_MAX];

#ifdef MSOS_MUTEX_USE_SEM
struct semaphore _MsOS_Task_Mutex;
#define MUTEX_TASK_LOCK()    down(&_MsOS_Task_Mutex)
#define MUTEX_TASK_UNLOCK()  up(&_MsOS_Task_Mutex)
#else
static                       DEFINE_SPINLOCK(_MsOS_Task_Mutex);
#define MUTEX_TASK_LOCK()    spin_lock(&_MsOS_Task_Mutex)
#define MUTEX_TASK_UNLOCK()  spin_unlock(&_MsOS_Task_Mutex)
#endif

//
// Event Group
//
typedef struct
{
    MS_BOOL                     bUsed;
    MS_U32                      u32EventGroup;
    spinlock_t                  stMutexEvent;
    wait_queue_head_t           stSemaphore;
    // pthread_cond_t              stSemaphore; // ?????????????
} MsOS_EventGroup_Info;

static MsOS_EventGroup_Info     _MsOS_EventGroup_Info[MSOS_EVENTGROUP_MAX];
static                          DEFINE_SPINLOCK(_MsOS_EventGroup_Mutex);
#define EVENT_MUTEX_LOCK()      spin_lock(&_MsOS_EventGroup_Mutex)
#define EVENT_MUTEX_UNLOCK()    spin_unlock(&_MsOS_EventGroup_Mutex)


// Timer
//
typedef struct
{
    MS_BOOL             bUsed;
    TimerCb             pTimerCb;
    struct timer_list   timer;
    int                 period;
    int                 first;
} MsOS_Timer_Info;
static MsOS_Timer_Info  _MsOS_Timer_Info[MSOS_TIMER_MAX];
static                          DEFINE_SPINLOCK(_MsOS_Timer_Mutex);
#define TIMER_MUTEX_LOCK()      spin_lock(&_MsOS_Timer_Mutex)
#define TIMER_MUTEX_UNLOCK()    spin_unlock(&_MsOS_Timer_Mutex)

/******************************************************************************/
/*                           Local Variables                                    */
/******************************************************************************/

/******************************************************************************/
/*               P r i v a t e    F u n c t i o n s                             */
/******************************************************************************/
MS_U32 MsOS_GetSystemTime (void)
{
    struct timespec         ts;

    getnstimeofday(&ts);
    return ts.tv_sec* 1000+ ts.tv_nsec/1000000;
}

MS_U32 MsOS_Timer_DiffTimeFromNow(MS_U32 u32TaskTimer) //unit = ms
{
    return (MsOS_GetSystemTime() - u32TaskTimer);
}

void MsOS_DelayTask (MS_U32 u32Ms)
{
    //sleep in spinlock will cause deadlock
#ifdef MSOS_MUTEX_USE_SEM
    msleep_interruptible((unsigned int)u32Ms);
#else
    mdelay(u32Ms);
#endif
}


void MsOS_DelayTaskUs (MS_U32 u32Us)
{
    struct timespec TS1, TS2;
    getnstimeofday(&TS1);
    getnstimeofday(&TS2);

    while((TS2.tv_nsec - TS1.tv_nsec)< (u32Us * 1000UL))
    {
        getnstimeofday(&TS2);
    }
#if 0
    struct timespec req, rem;

    req.tv_sec = 0;
    req.tv_nsec = (long) (u32Us*1000UL);

    while(1)
    {
        int err;

        err = nanosleep(&req, &rem);
        if(err==-1)
        {
            #if 0
            switch(errno)
            {
                case EINTR:
                    req.tv_sec = rem.tv_sec;
                    req.tv_nsec = rem.tv_nsec;
                    continue;
                default:
                    printk("nanosleep is interrupted: %d\n", errno);
            }
            #endif
        }

        break;
    }
#endif
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//
// Interrupt management
//
//-------------------------------------------------------------------------------------------------
/// Attach the interrupt callback function to interrupt #
/// @param  eIntNum \b IN: Interrupt number in enumerator InterruptNum
/// @param  pIntCb  \b IN: Interrupt callback function
/// @return TRUE : succeed
/// @return FALSE :  fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_AttachInterrupt (InterruptNum eIntNum, InterruptCb pIntCb)
{
    int i;
    i = request_irq(eIntNum, (irq_handler_t)pIntCb, 0, NULL, NULL);

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Detach the interrupt callback function from interrupt #
/// @param  eIntNum \b IN: Interrupt number in enumerator InterruptNum
/// @return TRUE : succeed
/// @return FALSE :  fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_DetachInterrupt (InterruptNum eIntNum)
{
    // PTH_RET_CHK(pthread_mutex_lock(&_ISR_Mutex));
    //HAL_IRQ_Detech((int)eIntNum); TODo!!
    // PTH_RET_CHK(pthread_mutex_unlock(&_ISR_Mutex));
    free_irq(eIntNum, NULL);
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Enable (unmask) the interrupt #
/// @param  eIntNum \b IN: Interrupt number in enumerator InterruptNum
/// @return TRUE : succeed
/// @return FALSE :  fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_EnableInterrupt (InterruptNum eIntNum)
{
    enable_irq((int)eIntNum);
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Disable (mask) the interrupt #
/// @param  eIntNum \b IN: Interrupt number in enumerator InterruptNum
/// @return TRUE : succeed
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_DisableInterrupt (InterruptNum eIntNum)
{
    disable_irq((int)eIntNum);
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Disable all interrupts (including timer interrupt), the scheduler is disabled.
/// @return Interrupt register value before all interrupts disable
//-------------------------------------------------------------------------------------------------
MS_U32 MsOS_DisableAllInterrupts(void)
{
#if 0
    return CHIP_DisableAllInterrupt() ;
#else
    return 0;
#endif
}

//-------------------------------------------------------------------------------------------------
/// Restore the interrupts from last MsOS_DisableAllInterrupts.
/// @param  u32OldInterrupts \b IN: Interrupt register value from @ref MsOS_DisableAllInterrupts
/// @return TRUE : succeed
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_RestoreAllInterrupts(MS_U32 u32OldInterrupts)
{
    return TRUE;
}
//-------------------------------------------------------------------------------------------------
/// Enable all CPU interrupts.
/// @return TRUE : succeed
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_EnableAllInterrupts(void)
{
#if 0
    return CHIP_EnableAllInterrupt() ;
#else
    return TRUE;
#endif
}

//-------------------------------------------------------------------------------------------------
/// In Interuupt Context or not
/// @return TRUE : Yes
/// @return FALSE : No
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_In_Interrupt (void)
{
#if 0
    return CHIP_InISRContext();
#else
    return FALSE;
#endif
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

//
// Mutex
//
//-------------------------------------------------------------------------------------------------
/// Create a mutex in the unlocked state
/// @param  eAttribute  \b IN: E_MSOS_FIFO: suspended in FIFO order
/// @param  pMutexName  \b IN: mutex name
/// @param  u32Flag  \b IN: process data shared flag
/// @return >=0 : assigned mutex Id
/// @return <0 : fail
/// @note   A mutex has the concept of an owner, whereas a semaphore does not.
///         A mutex provides priority inheritance protocol against proiorty inversion, whereas a binary semaphore does not.
//-------------------------------------------------------------------------------------------------
MS_S32 MsOS_CreateMutex ( MsOSAttribute eAttribute, char *pMutexName1, MS_U32 u32Flag)
{
    MS_S32 s32Id, s32LstUnused = MSOS_MUTEX_MAX;
    MS_U8 pMutexName[MAX_MUTEX_NAME_LENGTH];
    MS_U32 u32MaxLen;

    if (NULL == pMutexName1)
    {
        return -1;
    }
    if (strlen(pMutexName1) >= (MAX_MUTEX_NAME_LENGTH-1))
    {
        printk("%s: Warning strlen(%s) is longer than MAX_MUTEX_NAME_LENGTH(%d). Oversize char will be discard.\n",
        __FUNCTION__,pMutexName1,MAX_MUTEX_NAME_LENGTH);
    }
    if (0 == (u32MaxLen = MIN(strlen(pMutexName1), (MAX_MUTEX_NAME_LENGTH-1))))
    {
        return -1;
    }
    strncpy((char*)pMutexName, (const char*)pMutexName1, u32MaxLen);
    pMutexName[u32MaxLen] = '\0';

    MUTEX_MUTEX_LOCK();
    for(s32Id=0;s32Id<MSOS_MUTEX_MAX;s32Id++)
    {
        // if (PTHREAD_PROCESS_SHARED == s32Prop) // @FIXME: Richard: is the mutex name always used as an id, regardless of process shared/private property?
        {
            if(TRUE == _MsOS_Mutex_Info[s32Id].bUsed)
            {
                if (0== strcmp((const char*)_MsOS_Mutex_Info[s32Id].u8Name, (const char*)pMutexName))
                {
                    break;
                }
            }
        }
        if (FALSE==_MsOS_Mutex_Info[s32Id].bUsed  && MSOS_MUTEX_MAX==s32LstUnused)
        {
            s32LstUnused = s32Id;
        }
    }
    if ((MSOS_MUTEX_MAX==s32Id) && (MSOS_MUTEX_MAX>s32LstUnused))
    {
        _MsOS_Mutex_Info[s32LstUnused].bUsed = TRUE;
        strcpy((char*)_MsOS_Mutex_Info[s32LstUnused].u8Name, (const char*)pMutexName);
#ifdef MSOS_MUTEX_USE_SEM
        sema_init(&_MsOS_Mutex_Info[s32LstUnused].stMutex, 1);
#else
        spin_lock_init(&_MsOS_Mutex_Info[s32LstUnused].stMutex);
#endif
        s32Id = s32LstUnused;
    }
    MUTEX_MUTEX_UNLOCK();

    if(MSOS_MUTEX_MAX <= s32Id)
    {
        return -1;
    }

    s32Id |= MSOS_ID_PREFIX;

    return s32Id;
}

//-------------------------------------------------------------------------------------------------
/// Delete the specified mutex
/// @param  s32MutexId  \b IN: mutex ID
/// @return TRUE : succeed
/// @return FALSE : fail
/// @note   It is important that the mutex be in the unlocked state when it is
///            destroyed, or else the behavior is undefined.
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_DeleteMutex (MS_S32 s32MutexId)
{
    if ( (s32MutexId & MSOS_ID_PREFIX_MASK) != MSOS_ID_PREFIX )
    {
        return FALSE;
    }
    else
    {
        s32MutexId &= MSOS_ID_MASK;
    }
    MUTEX_MUTEX_LOCK();

    MS_ASSERT(_MsOS_Mutex_Info[s32MutexId].bUsed);
    _MsOS_Mutex_Info[s32MutexId].bUsed = FALSE;
    _MsOS_Mutex_Info[s32MutexId].u8Name[0] = '\0';

    MUTEX_MUTEX_UNLOCK();
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Attempt to lock a mutex
/// @param  s32MutexId  \b IN: mutex ID
/// @param  u32WaitMs   \b IN: 0 ~ MSOS_WAIT_FOREVER: suspend time (ms) if the mutex is locked
/// @return TRUE : succeed
/// @return FALSE : fail
//-------------------------------------------------------------------------------------------------
// @FIXME: don't support time-out at this stage
MS_BOOL MsOS_ObtainMutex (MS_S32 s32MutexId, MS_U32 u32WaitMs)
{
    MS_BOOL bRet = FALSE;

    if ( (s32MutexId & MSOS_ID_PREFIX_MASK) != MSOS_ID_PREFIX )
    {
        return FALSE;
    }
    else
    {
        s32MutexId &= MSOS_ID_MASK;
    }

    if (u32WaitMs==MSOS_WAIT_FOREVER) //blocking wait
    {
#ifdef MSOS_MUTEX_USE_SEM
        down(&(_MsOS_Mutex_Info[s32MutexId].stMutex));
#else
        spin_lock(&(_MsOS_Mutex_Info[s32MutexId].stMutex));
#endif
        bRet = TRUE;
    }
    else if (u32WaitMs==0) //non-blocking
    {
#ifdef MSOS_MUTEX_USE_SEM
        if (!down_trylock(&_MsOS_Mutex_Info[s32MutexId].stMutex))
#else
        if (spin_trylock(&_MsOS_Mutex_Info[s32MutexId].stMutex))
#endif
        {
            bRet = TRUE;
        }
    }
    else //blocking wait with timeout
    {
#ifndef MSOS_MUTEX_USE_SEM
        MS_U32 u32CurTime = MsOS_GetSystemTime();
#endif

        while(1)
        {
#ifdef MSOS_MUTEX_USE_SEM
            if (0 == down_timeout(&_MsOS_Mutex_Info[s32MutexId].stMutex, msecs_to_jiffies(u32WaitMs)))
#else
            if (spin_trylock(&_MsOS_Mutex_Info[s32MutexId].stMutex))
#endif
            {
                bRet = TRUE;
                break;
            }
#ifdef MSOS_MUTEX_USE_SEM
            else
#else
            else if((MsOS_GetSystemTime() - u32CurTime) > u32WaitMs)
#endif
            {
                printf("Mutext TimeOut: ID:%x \n", (int)s32MutexId);
                bRet = FALSE;
                break;
            }

           // msleep(5);
        }

    }
    return bRet;
}

//-------------------------------------------------------------------------------------------------
/// Attempt to unlock a mutex
/// @param  s32MutexId  \b IN: mutex ID
/// @return TRUE : succeed
/// @return FALSE : fail
/// @note   Only the owner thread of the mutex can unlock it.
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_ReleaseMutex (MS_S32 s32MutexId)
{
    if ( (s32MutexId & MSOS_ID_PREFIX_MASK) != MSOS_ID_PREFIX )
    {
        return FALSE;
    }
    else
    {
        s32MutexId &= MSOS_ID_MASK;
    }
#ifdef MSOS_MUTEX_USE_SEM
    up(&(_MsOS_Mutex_Info[s32MutexId].stMutex));
#else
    spin_unlock(&(_MsOS_Mutex_Info[s32MutexId].stMutex));
#endif

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
// Get a mutex informaton
// @param  s32MutexId  \b IN: mutex ID
// @param  peAttribute \b OUT: ptr to suspended mode: E_MSOS_FIFO / E_MSOS_PRIORITY
// @param  pMutexName  \b OUT: ptr to mutex name
// @return TRUE : succeed
// @return FALSE : the mutex has not been created.
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_InfoMutex (MS_S32 s32MutexId, MsOSAttribute *peAttribute, char *pMutexName)
{
    if ( (s32MutexId & MSOS_ID_PREFIX_MASK) != MSOS_ID_PREFIX )
    {
        return FALSE;
    }
    else
    {
        s32MutexId &= MSOS_ID_MASK;
    }

    if(_MsOS_Mutex_Info[s32MutexId].bUsed == TRUE)
    {
        //ToDo: extend _MsOS_Mutex_Info structure ?
        *peAttribute = E_MSOS_FIFO; //only FIFO for eCos
        // @FIXME: linux porting
        // UTL_strcpy(pMutexName, "ABC");
        strcpy(pMutexName, (const char*)_MsOS_Mutex_Info[s32MutexId].u8Name);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



MS_U32 MsOS_PA2KSEG1_1(MS_U32 addr)
{

    // miu0
    if ((0x00000000 <= addr) && (HAL_MIU1_BASE > addr))
    {
        return ((MS_U32)(addr) | (0x40000000));
    }
    // miu1
    if ((HAL_MIU1_BASE <= addr) && (((MS_U64)HAL_MIU1_BASE * 2) > (MS_U64)addr))
    {
        return ((addr & ~(HAL_MIU1_BASE)) | 0xD0000000);
    }
    return 0;
}


void MsOS_FlushMemory(void)
{

}


void MsOS_ReadMemory(void)
{

}

//
// Task
//
//-------------------------------------------------------------------------------------------------
/// Create a task
/// @param  pTaskEntry       \b IN: task entry point
/// @param  u32TaskEntryData \b IN: task entry data: a pointer to some static data, or a
///          small integer, or NULL if the task does not require any additional data.
/// @param  eTaskPriority    \b IN: task priority
/// @param  bAutoStart       \b IN: start immediately or later
/// @param  pStackBase       \b IN: task stack
/// @param  u32StackSize     \b IN: stack size
/// @param  pTaskName        \b IN: task name
/// @return >=0 : assigned Task ID
/// @return < 0 : fail
//-------------------------------------------------------------------------------------------------
MS_S32 MsOS_CreateTask (TaskEntry pTaskEntry,
                     MS_U32 u32TaskEntryData,
                     TaskPriority eTaskPriority,
                     MS_BOOL bAutoStart,
                     void * pStackBase,
                     MS_U32 u32StackSize,
                     char *pTaskName)
{
#if 0
    MS_S32 s32Id;
    pthread_attr_t thread_attr;
    struct sched_param thrsched;

    PTH_RET_CHK(pthread_mutex_lock(&_MsOS_Task_Mutex));
    for( s32Id=0; s32Id<MSOS_TASK_MAX; s32Id++)
    {
        if(_MsOS_Task_Info[s32Id].bUsed == FALSE)
        {
            break;
        }
    }
    if( s32Id < MSOS_TASK_MAX)
    {
        _MsOS_Task_Info[s32Id].bUsed = TRUE;
    }
    //printk("pthread_mutex_unlock\n");
    PTH_RET_CHK(pthread_mutex_unlock(&_MsOS_Task_Mutex));

    if( s32Id >= MSOS_TASK_MAX)
    {
        return -1;
    }
    //printk("pthread_attr_init\n");
    if (PTH_RET_CHK(pthread_attr_init(&thread_attr)))
    {
        return -1;
    }

    /* - MaxCC20080205
    if (pthread_attr_setstack(&thread_attr, (void*)(((MS_U32)pStackBase)+ u32StackSize), u32StackSize))
    {
        return -1;
    }
    // . MaxCC20080205 */
    //set thread priority
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&thread_attr, SCHED_RR);
    pthread_attr_getschedparam(&thread_attr, &thrsched);
    switch(eTaskPriority)
    {
        case E_TASK_PRI_SYS:
                thrsched.sched_priority = E_LINUX_PRI_SYS;
                break;
        case E_TASK_PRI_HIGHEST:
                thrsched.sched_priority = E_LINUX_PRI_HIGHEST;
                break;
        case E_TASK_PRI_HIGH:
                thrsched.sched_priority = E_LINUX_PRI_HIGH;
                break;
        case E_TASK_PRI_MEDIUM:
                thrsched.sched_priority = E_LINUX_PRI_MEDIUM;
                break;
        case E_TASK_PRI_LOW:
                thrsched.sched_priority = E_LINUX_PRI_LOW;
                break;
        case E_TASK_PRI_LOWEST:
                thrsched.sched_priority = E_LINUX_PRI_LOWEST;
                break;
    }
    pthread_attr_setschedparam(&thread_attr, &thrsched);
    //pthread_attr_getschedparam(&thread_attr, &thrsched);printk("%d\n",thrsched.sched_priority);
    //printk("max=%d,min=%d\n",sched_get_priority_max(SCHED_RR),sched_get_priority_min(SCHED_RR));
#else
    // @FIXME:
    //     (1) eTaskPriority: Task priority is ignored here
    //     (2) pTaskName: is ignored here
    //     (3) Need mutex to protect critical section

    MS_S32 s32Id;

    MUTEX_TASK_LOCK();

    for( s32Id=0; s32Id<MSOS_TASK_MAX; s32Id++)
    {
        if(_MsOS_Task_Info[s32Id].bUsed == FALSE)
        {
            break;
        }
    }
    if( s32Id >= MSOS_TASK_MAX)
    {
        return -1;
    }

    _MsOS_Task_Info[s32Id].bUsed = TRUE;
    //_MsOS_Task_Info[s32Id].pstThreadInfo = kthread_create(((int)(void *)pTaskEntry), (void*)u32TaskEntryData, pTaskName);
    _MsOS_Task_Info[s32Id].pstThreadInfo = kthread_create(pTaskEntry, (void*)u32TaskEntryData, pTaskName);

    MUTEX_TASK_UNLOCK();

    if (bAutoStart)
    {
        wake_up_process(_MsOS_Task_Info[s32Id].pstThreadInfo);
    }
    s32Id |= MSOS_ID_PREFIX;
#endif
    return s32Id;
}


//-------------------------------------------------------------------------------------------------
/// Delete a previously created task
/// @param  s32TaskId   \b IN: task ID
/// @return TRUE : succeed
/// @return FALSE : fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_DeleteTask (MS_S32 s32TaskId)
{
#if 1
    if ( (s32TaskId & MSOS_ID_PREFIX_MASK) != MSOS_ID_PREFIX )
    {
        return FALSE;
    }
    else
    {
        s32TaskId &= MSOS_ID_MASK;
    }

    kthread_stop(_MsOS_Task_Info[s32TaskId].pstThreadInfo);
    _MsOS_Task_Info[s32TaskId].bUsed = FALSE;
#else
    printk("[%s][%d] %s is not supported\n", __FUNCTION__, __LINE__, __FUNCTION__);
#endif
    return TRUE;
}



MS_BOOL MsOS_Init (void)
{
    MS_U32 u32I;

    //
    // Task Management
    //
#ifdef MSOS_MUTEX_USE_SEM
    sema_init(&_MsOS_Task_Mutex, 1);
#else
    spin_lock_init(&_MsOS_Task_Mutex);
#endif
    for( u32I=0; u32I<MSOS_TASK_MAX; u32I++)
    {
        _MsOS_Task_Info[u32I].bUsed = FALSE;
    }

    // Even Group
    spin_lock_init(&_MsOS_EventGroup_Mutex);
    for( u32I=0; u32I<MSOS_EVENTGROUP_MAX; u32I++)
    {
        _MsOS_EventGroup_Info[u32I].bUsed = FALSE;
    }

    //
    // Timer
    //
    spin_lock_init(&_MsOS_Timer_Mutex);
    for( u32I=0; u32I<MSOS_TIMER_MAX; u32I++)
    {
        _MsOS_Timer_Info[u32I].bUsed = FALSE;
        _MsOS_Timer_Info[u32I].pTimerCb = NULL;
        _MsOS_Timer_Info[u32I].period = 0;
        _MsOS_Timer_Info[u32I].first = 0;
        init_timer(&(_MsOS_Timer_Info[u32I].timer));
    }

    return TRUE;
}


//
// Event management
//
//-------------------------------------------------------------------------------------------------
/// Create an event group
/// @param  pEventName  \b IN: event group name
/// @return >=0 : assigned Event Id
/// @return <0 : fail
//-------------------------------------------------------------------------------------------------
MS_S32 MsOS_CreateEventGroup (char *pEventName)
{
    MS_S32 s32Id;

    EVENT_MUTEX_LOCK();
    for(s32Id=0; s32Id<MSOS_EVENTGROUP_MAX; s32Id++)
    {
        if(_MsOS_EventGroup_Info[s32Id].bUsed == FALSE)
        {
            break;
        }
    }
    if(s32Id < MSOS_EVENTGROUP_MAX)
    {
        _MsOS_EventGroup_Info[s32Id].bUsed = TRUE;
        _MsOS_EventGroup_Info[s32Id].u32EventGroup= 0;
    }
    EVENT_MUTEX_UNLOCK();

    if(s32Id >= MSOS_EVENTGROUP_MAX)
    {
        return -1;
    }
    spin_lock_init(&_MsOS_EventGroup_Info[s32Id].stMutexEvent);
    init_waitqueue_head(&_MsOS_EventGroup_Info[s32Id].stSemaphore);
    s32Id |= MSOS_ID_PREFIX;
    return s32Id;
}

//-------------------------------------------------------------------------------------------------
/// Delete the event group
/// @param  s32EventGroupId \b IN: event group ID
/// @return TRUE : succeed
/// @return FALSE : fail, sb is waiting for the event flag
/// @note event group that are being waited on must not be deleted
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_DeleteEventGroup (MS_S32 s32EventGroupId)
{
    if ( (s32EventGroupId & MSOS_ID_PREFIX_MASK) != MSOS_ID_PREFIX )
    {
        return FALSE;
    }
    else
    {
        s32EventGroupId &= MSOS_ID_MASK;
    }

    init_waitqueue_head(&_MsOS_EventGroup_Info[s32EventGroupId].stSemaphore);
    // PTH_RET_CHK(pthread_mutex_destroy(&_MsOS_EventGroup_Info[s32EventGroupId].stMutex));
    EVENT_MUTEX_LOCK();
    _MsOS_EventGroup_Info[s32EventGroupId].u32EventGroup= 0;
    _MsOS_EventGroup_Info[s32EventGroupId].bUsed = FALSE;
    EVENT_MUTEX_UNLOCK();
    return TRUE;
}


//-------------------------------------------------------------------------------------------------
/// Set the event flag (bitwise OR w/ current value) in the specified event group
/// @param  s32EventGroupId \b IN: event group ID
/// @param  u32EventFlag    \b IN: event flag value
/// @return TRUE : succeed
/// @return FALSE : fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_SetEvent (MS_S32 s32EventGroupId, MS_U32 u32EventFlag)
{
    if ( (s32EventGroupId & MSOS_ID_PREFIX_MASK) != MSOS_ID_PREFIX )
    {
        return FALSE;
    }
    else
    {
        s32EventGroupId &= MSOS_ID_MASK;
    }

    spin_lock(&_MsOS_EventGroup_Info[s32EventGroupId].stMutexEvent);
    SET_FLAG(_MsOS_EventGroup_Info[s32EventGroupId].u32EventGroup, u32EventFlag);
    spin_unlock(&_MsOS_EventGroup_Info[s32EventGroupId].stMutexEvent);
    wake_up(&_MsOS_EventGroup_Info[s32EventGroupId].stSemaphore);
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Clear the specified event flag (bitwise XOR operation) in the specified event group
/// @param  s32EventGroupId \b IN: event group ID
/// @param  u32EventFlag    \b IN: event flag value
/// @return TRUE : succeed
/// @return FALSE : fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_ClearEvent (MS_S32 s32EventGroupId, MS_U32 u32EventFlag)
{
    if ( (s32EventGroupId & MSOS_ID_PREFIX_MASK) != MSOS_ID_PREFIX )
    {
        return FALSE;
    }
    else
    {
        s32EventGroupId &= MSOS_ID_MASK;
    }

    spin_lock(&_MsOS_EventGroup_Info[s32EventGroupId].stMutexEvent);
    RESET_FLAG(_MsOS_EventGroup_Info[s32EventGroupId].u32EventGroup, u32EventFlag);
    spin_unlock(&_MsOS_EventGroup_Info[s32EventGroupId].stMutexEvent);
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Wait for the specified event flag combination from the event group
/// @param  s32EventGroupId     \b IN: event group ID
/// @param  u32WaitEventFlag    \b IN: wait event flag value
/// @param  pu32RetrievedEventFlag \b OUT: retrieved event flag value
/// @param  eWaitMode           \b IN: E_AND/E_OR/E_AND_CLEAR/E_OR_CLEAR
/// @param  u32WaitMs           \b IN: 0 ~ MSOS_WAIT_FOREVER: suspend time (ms) if the event is not ready
/// @return TRUE : succeed
/// @return FALSE : fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_WaitEvent (MS_S32 s32EventGroupId,
                     MS_U32 u32WaitEventFlag,
                     MS_U32 *pu32RetrievedEventFlag,
                     EventWaitMode eWaitMode,
                     MS_U32 u32WaitMs)
{
    MS_BOOL bRet= FALSE;
    MS_BOOL bAnd;
    MS_BOOL bClear;

    *pu32RetrievedEventFlag = 0;

    if (!u32WaitEventFlag)
    {
        return FALSE;
    }

    if ( (s32EventGroupId & MSOS_ID_PREFIX_MASK) != MSOS_ID_PREFIX )
    {
        return FALSE;
    }
    else
    {
        s32EventGroupId &= MSOS_ID_MASK;
    }

    bClear= ((E_AND_CLEAR== eWaitMode) || (E_OR_CLEAR== eWaitMode))? TRUE: FALSE;
    bAnd= ((E_AND== eWaitMode)|| (E_AND_CLEAR== eWaitMode))? TRUE: FALSE;

    do{
/*
        *pu32RetrievedEventFlag= HAS_FLAG(_MsOS_EventGroup_Info[s32EventGroupId].u32EventGroup, u32WaitEventFlag);
        if ((bAnd)? (*pu32RetrievedEventFlag== u32WaitEventFlag): (0!= *pu32RetrievedEventFlag))
        {
            break;
        }
*/
        if (u32WaitMs== MSOS_WAIT_FOREVER) //blocking wait
        {
            if (bAnd)
            {
                wait_event(_MsOS_EventGroup_Info[s32EventGroupId].stSemaphore,
                           ((_MsOS_EventGroup_Info[s32EventGroupId].u32EventGroup & u32WaitEventFlag) == u32WaitEventFlag));
            }
            else
            {
                wait_event(_MsOS_EventGroup_Info[s32EventGroupId].stSemaphore,
                           ((_MsOS_EventGroup_Info[s32EventGroupId].u32EventGroup & u32WaitEventFlag) != 0));
            }
        }
        else
        {
            u32WaitMs = msecs_to_jiffies(u32WaitMs);
            if (bAnd)
            {
                wait_event_timeout(_MsOS_EventGroup_Info[s32EventGroupId].stSemaphore,
                                   ((_MsOS_EventGroup_Info[s32EventGroupId].u32EventGroup & u32WaitEventFlag) == u32WaitEventFlag),
                                   u32WaitMs);
            }
            else
            {
                wait_event_timeout(_MsOS_EventGroup_Info[s32EventGroupId].stSemaphore,
                                   ((_MsOS_EventGroup_Info[s32EventGroupId].u32EventGroup & u32WaitEventFlag) != 0),
                                   u32WaitMs);
            }
        }
        *pu32RetrievedEventFlag= HAS_FLAG(_MsOS_EventGroup_Info[s32EventGroupId].u32EventGroup, u32WaitEventFlag);
    } while (0);

    spin_lock(&_MsOS_EventGroup_Info[s32EventGroupId].stMutexEvent);
    bRet= (bAnd)? (*pu32RetrievedEventFlag== u32WaitEventFlag): (0!= *pu32RetrievedEventFlag);
    if (bRet && bClear)
    {
        RESET_FLAG(_MsOS_EventGroup_Info[s32EventGroupId].u32EventGroup, *pu32RetrievedEventFlag);
    }
    spin_unlock(&_MsOS_EventGroup_Info[s32EventGroupId].stMutexEvent);
    return bRet;
}

wait_queue_head_t* MsOS_GetEventQueue (MS_S32 s32EventGroupId)
{
    if ( (s32EventGroupId & MSOS_ID_PREFIX_MASK) != MSOS_ID_PREFIX )
    {
        return FALSE;
    }
    else
    {
        s32EventGroupId &= MSOS_ID_MASK;
    }
    return (&_MsOS_EventGroup_Info[s32EventGroupId].stSemaphore);
}


//
// Timer management
//
static void _MsOS_TimerNotify(unsigned long data)
{
    MS_S32 s32Id = (MS_S32) data;

    if (_MsOS_Timer_Info[s32Id].pTimerCb)
    {
        _MsOS_Timer_Info[s32Id].pTimerCb(0, s32Id| MSOS_ID_PREFIX);
    }
    _MsOS_Timer_Info[s32Id].timer.expires = jiffies + HZ*_MsOS_Timer_Info[s32Id].period/1000;
    add_timer(&(_MsOS_Timer_Info[s32Id].timer));
}


//-------------------------------------------------------------------------------------------------
/// Create a Timer
/// @param  pTimerCb        \b IN: timer callback function
/// @param  u32FirstTimeMs  \b IN: first ms for timer expiration
/// @param  u32PeriodTimeMs \b IN: periodic ms for timer expiration after first expiration
///                                0: one shot timer
/// @param  bStartTimer     \b IN: TRUE: activates the timer after it is created
///                                FALSE: leaves the timer disabled after it is created
/// @param  pTimerName      \b IN: Timer name (not used by eCos)
/// @return >=0 : assigned Timer ID
///         <0 : fail
//-------------------------------------------------------------------------------------------------
MS_S32 MsOS_CreateTimer (TimerCb pTimerCb,
                      MS_U32 u32FirstTimeMs,
                      MS_U32 u32PeriodTimeMs,
                      MS_BOOL bStartTimer,
                      char *pTimerName)
{
    MS_S32 s32Id;

    TIMER_MUTEX_LOCK();
    for(s32Id=0;s32Id<MSOS_TIMER_MAX;s32Id++)
    {
        if(_MsOS_Timer_Info[s32Id].bUsed == FALSE)
        {
            break;
        }
    }
    if(s32Id < MSOS_TIMER_MAX)
    {
        _MsOS_Timer_Info[s32Id].bUsed = TRUE;
    }
    TIMER_MUTEX_UNLOCK();

    if(s32Id >= MSOS_TIMER_MAX)
    {
        return -1;
    }

    _MsOS_Timer_Info[s32Id].pTimerCb=   pTimerCb;
    _MsOS_Timer_Info[s32Id].first = u32FirstTimeMs;
    _MsOS_Timer_Info[s32Id].period = u32PeriodTimeMs;
    _MsOS_Timer_Info[s32Id].timer.data = (unsigned long)s32Id;
    _MsOS_Timer_Info[s32Id].timer.expires = jiffies + HZ*u32FirstTimeMs/1000;
    _MsOS_Timer_Info[s32Id].timer.function = _MsOS_TimerNotify;
    if (bStartTimer)
    {
        add_timer(&(_MsOS_Timer_Info[s32Id].timer));
    }
    s32Id |= MSOS_ID_PREFIX;
    return s32Id;
}

//-------------------------------------------------------------------------------------------------
/// Delete the Timer
/// @param  s32TimerId  \b IN: Timer ID
/// @return TRUE : succeed
/// @return FALSE : fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_DeleteTimer (MS_S32 s32TimerId)
{
    //return FALSE;

    if ( (s32TimerId & MSOS_ID_PREFIX_MASK) != MSOS_ID_PREFIX )
    {
        return FALSE;
    }
    else
    {
        s32TimerId &= MSOS_ID_MASK;
    }

    if( _MsOS_Timer_Info[s32TimerId].bUsed )
    {
        TIMER_MUTEX_LOCK();
        del_timer(&(_MsOS_Timer_Info[s32TimerId].timer));
        _MsOS_Timer_Info[s32TimerId].bUsed = FALSE;

        _MsOS_Timer_Info[s32TimerId].pTimerCb = NULL;
        _MsOS_Timer_Info[s32TimerId].period = 0;
        _MsOS_Timer_Info[s32TimerId].first = 0;
        TIMER_MUTEX_UNLOCK();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//-------------------------------------------------------------------------------------------------
/// Start the Timer
/// @param  s32TimerId  \b IN: Timer ID
/// @return TRUE : succeed
/// @return FALSE : fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_StartTimer (MS_S32 s32TimerId)
{
    //return FALSE;

    if ( (s32TimerId & MSOS_ID_PREFIX_MASK) != MSOS_ID_PREFIX )
    {
        return FALSE;
    }
    else
    {
        s32TimerId &= MSOS_ID_MASK;
    }

    if( _MsOS_Timer_Info[s32TimerId].bUsed )
    {
        _MsOS_Timer_Info[s32TimerId].timer.expires = jiffies + _MsOS_Timer_Info[s32TimerId].period/1000;
        add_timer(&(_MsOS_Timer_Info[s32TimerId].timer));
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//-------------------------------------------------------------------------------------------------
/// Stop the Timer
/// @param  s32TimerId  \b IN: Timer ID
/// @return TRUE : succeed
/// @return FALSE : fail
/// @note   MsOS_StopTimer then MsOS_StartTimer => The timer will trigger at the same relative
///             intervals that it would have if it had not been disabled.
//-------------------------------------------------------------------------------------------------
MS_BOOL MsOS_StopTimer (MS_S32 s32TimerId)
{
    //return FALSE;

    if ( (s32TimerId & MSOS_ID_PREFIX_MASK) != MSOS_ID_PREFIX )
    {
        return FALSE;
    }
    else
    {
        s32TimerId &= MSOS_ID_MASK;
    }

    if( _MsOS_Timer_Info[s32TimerId].bUsed )
    {
        del_timer(&(_MsOS_Timer_Info[s32TimerId].timer));
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

