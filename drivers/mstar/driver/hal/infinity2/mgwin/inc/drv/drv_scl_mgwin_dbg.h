////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2011 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __DRV_SCL_DBG_H__
#define __DRV_SCL_DBG_H__

typedef enum
{
    EN_DBGMG_IOCTL_CONFIG       = 0,
    EN_DBGMG_MDRV_CONFIG        = 1,
    EN_DBGMG_PRIORITY_CONFIG    = 2,
    EN_DBGMG_DRVSCLMGWIN_CONFIG = 3,
    EN_DBGMG_NUM_CONFIG         = 4,
}EN_DBGMG_CONFIG_TYPE;

typedef enum
{
    EN_DBGMG_SCLMGWINLEVEL_FRAME      = 0x01,
    EN_DBGMG_SCLMGWINLEVEL_SUBWIN     = 0x02,
    EN_DBGMG_SCLMGWINLEVEL_FPGASCRIPT = 0x04,
}EN_DBGMG_SCLMGWINLEVEL_TYPE;

extern u8 gMgwinbdbgmessage[EN_DBGMG_NUM_CONFIG];

#define  SCL_IO_DBG_ENABLE 1
#define  SCL_RTK_DBG_ENABLE 1

#define SCL_DBG_LV_MDRV_IO()            (gMgwinbdbgmessage[EN_DBGMG_MDRV_CONFIG])
#define SCL_DBG_LV_IOCTL()              (gMgwinbdbgmessage[EN_DBGMG_IOCTL_CONFIG])
#define SCL_DBG_LV_DRVSCLMGWIN()        (gMgwinbdbgmessage[EN_DBGMG_DRVSCLMGWIN_CONFIG])



#define Reset_DBGMG_FLAG() \
({\
    u8 i;\
    for(i=0;i<EN_DBGMG_NUM_CONFIG;i++)\
        gMgwinbdbgmessage[i] = 0;\
})

#define Set_DBGMG_FLAG(var,level)\
({\
    gMgwinbdbgmessage[var] = level;\
})


#ifdef   SCL_IO_DBG_ENABLE


#if defined (SCLOS_TYPE_LINUX_KERNEL)

#define SCL_ASSERT(_con)   \
    do {\
        if (!(_con)) {\
            printk(KERN_CRIT "BUG at %s:%d assert(%s)\n",\
                    __FILE__, __LINE__, #_con);\
            BUG();\
        }\
    } while (0)

#define SCL_DBG(dbglv, _fmt, _args...)             \
    do                                             \
    if(dbglv)                                      \
    {                                              \
        if(gMgwinbdbgmessage[EN_DBGMG_PRIORITY_CONFIG]) \
            printk(KERN_WARNING _fmt, ## _args);       \
        else \
            printk(KERN_DEBUG _fmt, ## _args);       \
    }while(0)


#define SCL_RTKDBG(dbglv, _fmt, _args...)


#define SCL_ERR(_fmt, _args...)       printk(KERN_WARNING _fmt, ## _args)
#define SCL_DBGERR(_fmt, _args...)    printk(KERN_DEBUG _fmt, ## _args)


#else

#define SCL_ASSERT(_con)   \
    do {\
        if (!(_con)) {\
            UartSendTrace("BUG at %s:%d assert(%s)\n", __FILE__, __LINE__, #_con);\
        }\
    } while (0); \
    while(1);

#define SCL_DBG(dbglv, _fmt, _args...)             \
    do                                             \
    if(dbglv)                                      \
    {                                              \
        if(gMgwinbdbgmessage[EN_DBGMG_PRIORITY_CONFIG]) \
            UartSendTrace(_fmt, ## _args);       \
    }while(0)

#define SCL_RTKDBG(dbglv, _fmt, _args...)             \
    do                                             \
    if(dbglv)                                      \
    {                                              \
            UartSendTrace(_fmt, ## _args);       \
    }while(0)

#define SCL_ERR(_fmt, _args...)       UartSendTrace(_fmt, ## _args)
#define SCL_DBGERR(_fmt, _args...)    UartSendTrace(_fmt, ## _args)
#endif

#define VersionCheckSuccess 0

#else
#define     SCL_ASSERT(arg)
#define     SCL_DBG(dbglv, _fmt, _args...)

#endif




#endif
