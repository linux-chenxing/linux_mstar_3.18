
#ifndef _DRV_RTC_H
#define _DRV_RTC_H


#include "mdrv_types.h"

typedef struct
{
    U8 u8PmRtcIndex;
    U32 u32RtcCtrlWord;
    U32 u32RtcSetMatchCounter;
    U32 u32RtcGetMatchCounter;
    U32 u32RtcSetCounter;
    U32 u32RtcGetCounter;

} PM_RtcParam;

void MDrv_RTC_Init(PM_RtcParam *pPmRtcParam);
void MDrv_RTC_SetCount(PM_RtcParam *pPmRtcParam);
U32 MDrv_RTC_GetCount(PM_RtcParam *pPmRtcParam);
void MDrv_RTC_SetMatchCount(PM_RtcParam *pPmRtcParam);
U32 MDrv_RTC_GetMatchCount(PM_RtcParam *pPmRtcParam);

#endif
