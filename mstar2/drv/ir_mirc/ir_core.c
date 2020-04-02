/* ir-core.c - handle IR pulse/space events
 *
 * Copyright (C) 2010 by Mauro Carvalho Chehab <mchehab@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include <linux/export.h>
#include <linux/input.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/kmod.h>
#include <linux/sched.h>
#include <linux/freezer.h>
#include "mstar_ir.h"
#include "ir_core.h"
#include "ir_common.h"

/* Used to handle IR raw handler extensions */
static DEFINE_MUTEX(ir_decoder_lock);
static LIST_HEAD(ir_decoder_list);

u64  protocols = 0; //support protocols for decoder
static unsigned long  _ulLastKeyPresentTime; //record last key press time
IR_DBG_LEVEL_e _edbglevel = IR_DBG_NONE; //debug level for /sys/device/ir/IRDebug
extern IRModHandle IRDev;
extern u32 u32IRHeaderCode[2];

#ifdef CONFIG_MIRC_INPUT_DEVICE
#define IR_DEFAULT_TIMEOUT 150  //timeout maybe > two IR Waveform cycle times
static unsigned long  eventtimeout = IR_DEFAULT_TIMEOUT; //for  event timeout

unsigned long MIRC_Get_Event_Timeout(void)
{
    return eventtimeout;
}

void MIRC_Set_Event_Timeout(unsigned long time)
{
    eventtimeout = time;
}
#endif

IR_SPEED_LEVEL_e MIRC_Get_IRSpeed_Level(void)
{
    return IRDev.pIRDev->speed;
}

void MIRC_Set_IRSpeed_Level(IR_SPEED_LEVEL_e level)
{
    IRDev.pIRDev->speed = level;
    return;
}

IR_DBG_LEVEL_e MIRC_Get_IRDBG_Level(void)
{
    return _edbglevel;
}

void MIRC_Set_IRDBG_Level(IR_DBG_LEVEL_e level)
{
    _edbglevel = level;
    return;
}

u64 MIRC_Get_Protocols(void)
{
    return protocols;
}
void MIRC_Set_Protocols(u64 data)
{
    protocols = data;
    IRDev.pIRDev->enabled_protocols = protocols;
}

unsigned long MIRC_Get_System_Time(void)
{
    return((unsigned long)((jiffies)*(1000/HZ)));
}

unsigned long MIRC_Get_LastKey_Time(void)
{
    return _ulLastKeyPresentTime;
}

static int MIRC_Data_Ctrl_Thread(void *data)
{
    struct ir_raw_data ev;
    struct ir_raw_data_ctrl *raw = (struct ir_raw_data_ctrl *)data;
    int retval;
    int get_scancode;

    while (!kthread_should_stop())
    {
        get_scancode = 0;
        spin_lock_irq(&raw->lock);
        retval = kfifo_len(&raw->kfifo);

        if (retval < sizeof(ev))
        {
            set_current_state(TASK_INTERRUPTIBLE);

            if (kthread_should_stop())
                set_current_state(TASK_RUNNING);
            spin_unlock_irq(&raw->lock);
            schedule();
            continue;
        }

        retval = kfifo_out(&raw->kfifo, &ev, sizeof(ev));
        spin_unlock_irq(&raw->lock);

        mutex_lock(&ir_decoder_lock);
        if(raw->dev->ir_mode == IR_TYPE_SWDECODE_MODE) //sw decoder
        {
            struct ir_decoder_handler *handler;
            IRDBG_MSG("shotcount[ %5d  %d ]\n",ev.duration, ev.pulse);
            list_for_each_entry(handler, &ir_decoder_list, list){
                if(handler->decode(raw->dev, ev) > 0){
                    get_scancode = 1;
                    break;
                }
            }
        }
        else //hw decoder
        {
            raw->this_sc.scancode = ev.duration;
            raw->u8RepeatFlag = ev.pulse;
            get_scancode = 1;
            IRDBG_MSG("keycode =%x  repeatflag =%d \n",raw->this_sc.scancode,raw->u8RepeatFlag);
        }

        if (get_scancode)
        {
            _ulLastKeyPresentTime = MIRC_Get_System_Time();

#ifdef CONFIG_MIRC_INPUT_DEVICE
            //diff protocol
            if (raw->this_sc.scancode_protocol!= raw->prev_sc.scancode_protocol)
            {
                //if before timeout send keyup + keydown
                if (time_after_eq((raw->keyup_jiffies+msecs_to_jiffies(eventtimeout)),jiffies))
                {
                    MIRC_Keyup(raw->dev);
                }
                IRDBG_INFO("[2]========= Down First!\n");
                MIRC_Keydown(raw->dev, raw->this_sc.scancode);
            }
            else// same protocol
            {
                if (raw->this_sc.scancode != raw->prev_sc.scancode)// diff key press
                {
                    //if before timeout send keyup + keydown
                    if (time_after_eq((raw->keyup_jiffies+msecs_to_jiffies(eventtimeout)),jiffies))
                    {
                        MIRC_Keyup(raw->dev);
                    }
                    IRDBG_INFO("[1]========= Down First!\n");
                    MIRC_Keydown(raw->dev, raw->this_sc.scancode);//first keydown
                }
                else
                {
                    if((raw->u8RepeatFlag == 0)&& (!raw->dev->keypressed ))// press the same fast
                    {
                        MIRC_Keydown(raw->dev, raw->this_sc.scancode);
                    }
                    else
                    {
                        IRDBG_INFO("========= Repeat!\n");
                    }
                }
            }

            del_timer(&(raw->timer_keyup));
            raw->timer_keyup.expires = jiffies + msecs_to_jiffies(eventtimeout);
            raw->keyup_jiffies = jiffies;
            raw->timer_keyup.data = (unsigned long)raw->dev;
            add_timer(&(raw->timer_keyup));
#else
            // for DFB polling read   /dev/ir
            if (down_trylock(&raw->dev->sem)==0)
            {
                IRDBG_INFO("scancode =%x \n",raw->this_sc.scancode);
                if (kfifo_in(&raw->dev->read_fifo, &raw->this_sc.scancode, sizeof(u32)) != sizeof(u32))
                {
                    up(&raw->dev->sem);
                    mutex_unlock(&ir_decoder_lock);
                    return -ENOMEM;
                }
                up(&raw->dev->sem);
                wake_up_interruptible(&raw->dev->read_wait);
            }
#endif
            //record IR last scancode
            raw->prev_sc.scancode_protocol = raw->this_sc.scancode_protocol;
            raw->prev_sc.scancode = raw->this_sc.scancode;
            raw->this_sc.scancode_protocol = 0;
            raw->this_sc.scancode = 0x0;
            raw->u8RepeatFlag = 0;
        }
        mutex_unlock(&ir_decoder_lock);
    }

    return 0;
}

int MIRC_Data_Store(struct mstar_ir_dev *dev, struct ir_raw_data *ev)
{
    if (!dev->raw)
        return -EINVAL;
    if (kfifo_in(&dev->raw->kfifo, ev, sizeof(*ev)) != sizeof(*ev))
        return -ENOMEM;

    return 0;
}

void MIRC_Data_Wakeup(struct mstar_ir_dev *dev)
{
    unsigned long flags;

    if (!dev->raw)
        return;

    spin_lock_irqsave(&dev->raw->lock, flags);
    wake_up_process(dev->raw->thread);
    spin_unlock_irqrestore(&dev->raw->lock, flags);
}

/*
 * Used to (un)register raw event clients
 */
int MIRC_Data_Ctrl_Init(struct mstar_ir_dev *dev)
{
    int ret;
    if (!dev)
        return -EINVAL;

    dev->raw = kzalloc(sizeof(*dev->raw), GFP_KERNEL);
    if (!dev->raw)
        return -ENOMEM;

    dev->raw->dev = dev;
    dev->enabled_protocols = 0;
    ret= kfifo_alloc(&dev->raw->kfifo,
            sizeof(struct ir_raw_data) * MAX_IR_DATA_SIZE,
            GFP_KERNEL);
    if (ret < 0)
        goto out;

    spin_lock_init(&dev->raw->lock);
    dev->raw->thread = kthread_run(MIRC_Data_Ctrl_Thread, dev->raw,"Mstar_ir");

    if (IS_ERR(dev->raw->thread)) {
        ret = PTR_ERR(dev->raw->thread);
        goto out;
    }
#ifdef CONFIG_MIRC_INPUT_DEVICE
    // init timer event up
    dev->raw->keyup_jiffies = jiffies;//record timeout for event up
    init_timer(&(dev->raw->timer_keyup));
    dev->raw->timer_keyup.function = MIRC_Timer_Proc;
    dev->raw->timer_keyup.expires =jiffies+ msecs_to_jiffies(eventtimeout);
    dev->raw->timer_keyup.data = (unsigned long)dev;
#endif
    return 0;
out:
    kfree(dev->raw);
    dev->raw = NULL;
    return ret;
}

void MIRC_Data_Ctrl_DeInit(struct mstar_ir_dev *dev)
{
    if (!dev || !dev->raw)
        return;
#ifdef CONFIG_MIRC_INPUT_DEVICE
    del_timer(&(dev->raw->timer_keyup));
#endif
    kthread_stop(dev->raw->thread);
    kfifo_free(&dev->raw->kfifo);
    kfree(dev->raw);
    dev->raw = NULL;
}

int MIRC_IRCustomer_Remove(IR_Profile_t *stIr)
{
    int i,j;
    struct mstar_ir_dev *dev = IRDev.pIRDev;
    if ((NULL == dev) || (NULL == stIr))
        return -EINVAL;
    for(i = 0; i < dev->support_num;i++)
    {
        if(memcmp(&dev->support_ir[i],&stIr,sizeof(IR_Profile_t)) == 0)
        {
            break;
        }
    }
    if(i < dev->support_num)
    {
        dev->support_num--;
       for(j = i;j < dev->support_num;j++)
       {
            memcpy(&dev->support_ir[j],&dev->support_ir[j+1],sizeof(IR_Profile_t));
       }
       return 0;
    }
    return -1;
}
EXPORT_SYMBOL_GPL(MIRC_IRCustomer_Remove);

int MIRC_IRCustomer_Add(IR_Profile_t *stIr)
{
    struct mstar_ir_dev *dev = IRDev.pIRDev;
    if ((NULL == dev) || (NULL == stIr))
        return -EINVAL;
    if(dev->support_num >= IR_SUPPORT_MAX)
        return -EINVAL;
    IRDBG_INFO("eIRType = %d,u32HeadCode = %x,u32IRSpeed = %d\n",stIr->eIRType,stIr->u32HeadCode,stIr->u32IRSpeed);
    memcpy(&dev->support_ir[dev->support_num],stIr,sizeof(IR_Profile_t));
    dev->support_num++;
    return 0;
}
EXPORT_SYMBOL_GPL(MIRC_IRCustomer_Add);

int MIRC_IRCustomer_Init(void)
{
    struct mstar_ir_dev *dev = IRDev.pIRDev;
    IR_Type_e IrType = IR_TYPE_MAX;
    u8 i = 0;
    if (NULL == dev)
        return -EINVAL;
    if(dev->support_num == 1)
        IrType = dev->support_ir[0].eIRType;
    for(i = 0;i<(dev->support_num - 1);i++)
    {
        if(dev->support_ir[i].eIRType == dev->support_ir[i+1].eIRType)
        {
            IrType = dev->support_ir[i].eIRType;
        }
        else
        {
            dev->ir_mode = IR_TYPE_SWDECODE_MODE;
            for(i = 0;i<(dev->support_num);i++)
            {
                switch(dev->support_ir[i].eIRType)
                {
                    case IR_TYPE_NEC :
                        nec_decode_init();
                        break;
                    case IR_TYPE_RCA:
                        rca_decode_init();
                        break;
                    case IR_TYPE_P7051:
                        p7051_decode_init();
                        break;
                    case IR_TYPE_RC5:
                    case IR_TYPE_RC5X:
                        rc5_decode_init();
                        break;
                    case IR_TYPE_RC6_MODE0:
                    case IR_TYPE_RC6:
                        rc6_decode_init();
                        break;
                    case IR_TYPE_TOSHIBA:
                        toshiba_decode_init();
                        break;
                    default :
                        IRDBG_ERR("IR TYPE not define,please check!!!\n");
                        break;
                }
            }
            return 0;
        }
    }
    if(IrType == IR_TYPE_NEC)
    {
        if(dev->support_num <=2)
        {
            //set customer code0
            u32IRHeaderCode[0] = dev->support_ir[0].u32HeadCode;
            u32IRHeaderCode[1] = dev->support_ir[1].u32HeadCode;
            dev->ir_mode = IR_TYPE_FULLDECODE_MODE;
        }
        else
        {
            dev->ir_mode = IR_TYPE_RAWDATA_MODE;
        }
    }
    else if(IrType == IR_TYPE_RC5)
    {
        dev->ir_mode = IR_TYPE_HWRC5_MODE;
    }
    else if(IrType == IR_TYPE_RC5X)
    {
        dev->ir_mode = IR_TYPE_HWRC5X_MODE;
    }
    else if(IrType == IR_TYPE_RC6_MODE0)
    {
        dev->ir_mode = IR_TYPE_HWRC6_MODE;
    }
    else
    {
        IRDBG_WRN("[IR Warring]No ir support \n");
    }
    return 0;
}
EXPORT_SYMBOL_GPL(MIRC_IRCustomer_Init);

int MIRC_IRCustomer_Config(IR_Profile_t *stIr,u8 num)
{
    u8 i = 0;
    if (NULL == stIr)
        return -EINVAL;
    if(num > IR_SUPPORT_MAX)
    {
        num = IR_SUPPORT_MAX;
    }
    for(i=0;i<num;i++)
    {
        if(MIRC_IRCustomer_Add(&stIr[i]) != 0 )
            return -EINVAL;
    }
    MIRC_IRCustomer_Init();
    return 0;
}

int MIRC_IRCustomer_DeConfig(IR_Profile_t *stIr,u8 num)
{
    struct mstar_ir_dev *dev = IRDev.pIRDev;
    u8 i = 0;
    if ((NULL == stIr)||(NULL == dev))
        return -EINVAL;
    for(i=0;i<num;i++)
    {
        MIRC_IRCustomer_Remove(&stIr[i]);
    }
    return 0;
}

/*
 * Extension interface - used to register the IR decoders
 */
int MIRC_Decoder_Register(struct ir_decoder_handler *handler)
{
    mutex_lock(&ir_decoder_lock);
    list_add_tail(&handler->list, &ir_decoder_list);
    protocols |= handler->protocols;
    MIRC_Set_Protocols(protocols);
    mutex_unlock(&ir_decoder_lock);

    return 0;
}
EXPORT_SYMBOL(MIRC_Decoder_Register);

void MIRC_Decoder_UnRegister(struct ir_decoder_handler *handler)
{
    mutex_lock(&ir_decoder_lock);
    list_del(&handler->list);
    protocols &= ~(handler->protocols);
    MIRC_Set_Protocols(protocols);
    mutex_unlock(&ir_decoder_lock);
}
EXPORT_SYMBOL(MIRC_Decoder_UnRegister);

#ifdef CONFIG_MIRC_INPUT_DEVICE

static LIST_HEAD(keymap_list);

static DEFINE_SPINLOCK(key_map_lock);

//key map functions
int MIRC_Map_Register(struct key_map_list *map)
{
    spin_lock(&key_map_lock);
    list_add_tail(&map->list, &keymap_list);
    spin_unlock(&key_map_lock);
    return 0;
}
EXPORT_SYMBOL_GPL(MIRC_Map_Register);

void MIRC_Map_UnRegister(struct key_map_list *map)
{
    spin_lock(&key_map_lock);
    list_del(&map->list);
    spin_unlock(&key_map_lock);
}
EXPORT_SYMBOL_GPL(MIRC_Map_UnRegister);

void MIRC_Timer_Proc(unsigned long data)
{
    struct mstar_ir_dev *dev = (struct mstar_ir_dev *)data;
    MIRC_Keyup(dev);
    dev->raw->prev_sc.scancode = 0;
    //IR_PRINT("upupup\n");
}

static u32 MIRC_Keycode_From_Map(u8 keymapnum,u32 scancode)
{
    struct key_map_list *map = NULL;
    u32 keycode = KEY_RESERVED;
    u8 i=0;
    bool match_flag = false;
    spin_lock(&key_map_lock);
    list_for_each_entry(map, &keymap_list, list)
    {
        if ((scancode>>8) == map->map.headcode)
        {
            IRDBG_INFO("[Match] Keymap Name: %s\n",map->map.name);
            match_flag = true;
            break;//find IR map
        }
    }
    if (match_flag)
    {
        for (i = 0; i<map->map.size; i++)
        {
            if ((map->map.scan[i].scancode&0xff) == (scancode&0xff))
            {
                IRDBG_INFO("[Match] Scancode =%x -----> Keycode= %x\n",scancode,map->map.scan[i].keycode);
                keycode = map->map.scan[i].keycode;
                break;//match scancode, and return keycode
            }
        }
    }
    spin_unlock(&key_map_lock);
    return keycode;
}

static void MIRC_Do_Keyup(struct mstar_ir_dev *dev, bool sync)
{
    if (!dev->keypressed)
        return;

    input_report_key(dev->input_dev, dev->last_keycode, 0);
    if (sync)
        input_sync(dev->input_dev);
    dev->keypressed = false;
}

void MIRC_Keyup(struct mstar_ir_dev *dev)
{
    unsigned long flags;

    spin_lock_irqsave(&dev->keylock, flags);
    MIRC_Do_Keyup(dev, true);
    spin_unlock_irqrestore(&dev->keylock, flags);
}

static void MIRC_Do_Keydown(struct mstar_ir_dev *dev, int scancode,u32 keycode)
{
    bool new_event = !dev->keypressed ||
        dev->last_scancode != scancode;

    if (new_event && dev->keypressed)
        MIRC_Do_Keyup(dev, false);

    input_event(dev->input_dev, EV_MSC, MSC_SCAN, scancode);

    if (new_event && keycode != KEY_RESERVED) {
        /* Register a keypress */
        dev->keypressed = true;
        dev->last_scancode = scancode;
        dev->last_keycode = keycode;
        input_report_key(dev->input_dev, keycode, 1);
    }

    input_sync(dev->input_dev);
}

void MIRC_Keydown(struct mstar_ir_dev *dev, int scancode)
{
    unsigned long flags;
    u32 keycode = MIRC_Keycode_From_Map(dev->map_num, scancode);
    if (keycode == KEY_RESERVED)
    {
        IRDBG_ERR("Key Map Match scancode Failed!\n");
        return ;
    }
    spin_lock_irqsave(&dev->keylock, flags);
    MIRC_Do_Keydown(dev, scancode, keycode);
    spin_unlock_irqrestore(&dev->keylock, flags);
}
#endif
