



#ifndef _REG_RTC_H
#define _REG_RTC_H

#include "mdrv_types.h"

//------------------------------------------------------------------------------
// PIU_MISC Reg
//------------------------------------------------------------------------------

#define RIU_MAP 0xBF000000

#define REG_RTC_BASE_0          (0x1200)
#define REG_RTC_BASE_2          (0x1300)

#define REG_RTC_CTRL_REG        (0x0000)
    #define RTC_SOFT_RSTZ_BIT       (0x0001)//BIT0
    #define RTC_CNT_EN_BIT          (0x0002)//BIT1
    #define RTC_WRAP_EN_BIT         (0x0004)//BIT2
    #define RTC_LOAD_EN_BIT         (0x0008)//BIT3
    #define RTC_READ_EN_BIT         (0x0010)//BIT4
    #define RTC_INT_MASK_BIT        (0x0020)//BIT5
    #define RTC_INT_FORCE_BIT       (0x0040)//BIT6
    #define RTC_INT_CLEAR_BIT       (0x0080)//BIT7 
#define REG_RTC_FREQ_CW         (0x0002)    //BIT0-BIT31
#define REG_RTC_LOAD_VAL        (0x0006)    //BIT0-BIT31
#define REG_RTC_MATCH_VAL       (0x000A)    //BIT0-BIT31
#define REG_RTC_INT             (0x000E)
    #define RTC_RAW_INT_BIT         (0x0001)//BIT0
    #define RTC_INT_BIT             (0x0002)//BIT1
#define REG_RTC_CNT             (0x0010)    //BIT0-BIT31

// PM
#define PM_REG_BASE             (0x0700*2)
#define REG_PM_CKG_RTC          (PM_REG_BASE + 0x22*2+0)




#endif  // _REG_RTC_H

