



#ifndef _REG_RTC_H
#define _REG_RTC_H

#include "mdrv_types.h"

//------------------------------------------------------------------------------
// PIU_MISC Reg
//------------------------------------------------------------------------------
#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
#define RIU_MAP 0xFD000000
#elif defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define RIU_MAP           (mstar_pm_base)
#endif

#define REG_RTC_BASE_0          (0x1200UL)
#define REG_RTC_BASE_2          (0x1300UL)

#define REG_RTC_CTRL_REG        (0x0000UL)
    #define RTC_SOFT_RSTZ_BIT       (0x0001UL)//BIT0
    #define RTC_CNT_EN_BIT          (0x0002UL)//BIT1
    #define RTC_WRAP_EN_BIT         (0x0004UL)//BIT2
    #define RTC_LOAD_EN_BIT         (0x0008UL)//BIT3
    #define RTC_READ_EN_BIT         (0x0010UL)//BIT4
    #define RTC_INT_MASK_BIT        (0x0020UL)//BIT5
    #define RTC_INT_FORCE_BIT       (0x0040UL)//BIT6
    #define RTC_INT_CLEAR_BIT       (0x0080UL)//BIT7 
#define REG_RTC_FREQ_CW         (0x0002UL)    //BIT0-BIT31
#define REG_RTC_LOAD_VAL        (0x0006UL)    //BIT0-BIT31
#define REG_RTC_MATCH_VAL       (0x000EUL)    //BIT0-BIT31
#define REG_RTC_INT             (0x0000UL)
    #define RTC_RAW_INT_BIT         (0x0100UL)//BIT0
    #define RTC_INT_BIT             (0x0200UL)//BIT1
#define REG_RTC_CNT             (0x0016UL)    //BIT0-BIT31

// PM
#define PM_REG_BASE             (0x0700UL*2)
#define REG_PM_CKG_RTC          (PM_REG_BASE + 0x22UL*2+0)




#endif  // _REG_RTC_H

