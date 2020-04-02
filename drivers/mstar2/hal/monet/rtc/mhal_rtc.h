
#ifndef _HAL_RTC_H_
#define _HAL_RTC_H_


#include "mdrv_types.h"

typedef enum
{
    E_RTC_0,
    E_RTC_2,
}E_MS_RTC;

void MHAL_RTC_Reading(E_MS_RTC eRtc, bool bEnable); // Value to load into RTC counter
void MHAL_RTC_Loading (E_MS_RTC eRtc, bool bEnable); // Load enable for loading value into RTC counter
void MHAL_RTC_RESET(E_MS_RTC eRtc, bool bEnable); // RTC software reset (low active)
void MHAL_RTC_Counter(E_MS_RTC eRtc, bool bEnable); //Enable RTC Counter

void MHAL_RTC_Init(E_MS_RTC eRtc,U32 u32Xtal);
void MHAL_RTC_SetCounter(E_MS_RTC eRtc,U32 u32RtcSetCounter);
U32 MHAL_RTC_GetCounter(E_MS_RTC eRtc);
void MHAL_RTC_SetMatchCounter(E_MS_RTC eRtc,U32 u32RtcSetCounter);
U32 MHAL_RTC_GetMatchCounter(E_MS_RTC eRtc);
#endif

