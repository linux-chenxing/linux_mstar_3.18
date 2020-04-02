/***************************************************************************************************************
 *
 * FileName hal_sd_platform.h
 *     @author jeremy.wang (2017/12/20)
 * Desc:
 *     This file is the header file of hal_sd_platform_XX.c.
 *     Every project has the same header file.
 *
 ***************************************************************************************************************/

#ifndef __HAL_SD_PLATFORM_H
#define __HAL_SD_PLATFORM_H

#include "hal_sd_regs.h"

typedef enum
{
    EV_PULLDOWN,
    EV_PULLUP,

} PinPullEmType;

typedef enum
{
    EV_GPIO1        = 0,
    EV_GPIO2        = 1,
    EV_GPIO3        = 2,

} GPIOEmType;

typedef enum
{
    EV_GPIO_OPT1 = 0,
    EV_GPIO_OPT2 = 1,
    EV_GPIO_OPT3 = 2,
    EV_GPIO_OPT4 = 3,
    EV_GPIO_OPT5 = 4,

} GPIOOptEmType;


typedef enum
{
    EV_NORVOL        = 0,
    EV_LOWVOL        = 1,
    EV_MINVOL        = 2,

} PADVddEmType;


typedef enum
{
    EV_VDD_DUMMY     = 0,
    EV_VDD_165_195   = BIT07_T,
    EV_VDD_20_21     = BIT08_T,
    EV_VDD_21_22     = BIT09_T,
    EV_VDD_22_23     = BIT10_T,
    EV_VDD_23_24     = BIT11_T,
    EV_VDD_24_25     = BIT12_T,
    EV_VDD_25_26     = BIT13_T,
    EV_VDD_26_27     = BIT14_T,
    EV_VDD_27_28     = BIT15_T,
    EV_VDD_28_29     = BIT16_T,
    EV_VDD_29_30     = BIT17_T,
    EV_VDD_30_31     = BIT18_T,
    EV_VDD_31_32     = BIT19_T,
    EV_VDD_32_33     = BIT20_T,
    EV_VDD_33_34     = BIT21_T,
    EV_VDD_34_35     = BIT22_T,
    EV_VDD_35_36     = BIT23_T,
    EV_VDD_50        = BIT24_T,

} VddEmType;


/* For reference (static function for PLL define)
static void _PLTSD_PLL_Auto_Turn_ONOFF_LDO(IPEmType eIP);
static void _PLTSD_PLL_Turn_OFF_LDO(IPEmType eIP);
static BOOL_T _PLTSD_PLL_Switch_AVDD(IPEmType eIP, PADVddEmType ePADVdd);
static void _PLTSD_PLL_1XClock_Setting(IPEmType eIP, U32_T u32ClkFromIPSet);
*/


// IP Setting for SD Platform
//----------------------------------------------------------------------------------------------------------
void Hal_SDPLT_InitIPOnce(IPEmType eIP);
void Hal_SDPLT_IPBeginSettnig(IPEmType eIP);
void Hal_SDPLT_IPEndSettnig(IPEmType eIP);

// PAD Setting for SD Platform
//----------------------------------------------------------------------------------------------------------
void Hal_SDPLT_SetPADPortPath(IPEmType eIP, PortEmType ePort, PADEmType ePAD);
void Hal_SDPLT_InitPADPin(IPEmType eIP, PADEmType ePAD);
void Hal_SDPLT_PullPADPin(IPEmType eIP, PADEmType ePAD, PinPullEmType ePinPull);

// Clock Setting for SD Platform
//----------------------------------------------------------------------------------------------------------
void Hal_SDPLT_SetClock(IPEmType eIP, U32_T u32ClkFromIPSet, BOOL_T b1stRunFlag);
U32_T Hal_SDPLT_FindClockSetting(IPEmType eIP, U32_T u32ReffClk, U8_T u8PassLevel, U8_T u8DownLevel);
void Hal_SDPLT_SetBusTiming(IPEmType eIP, BusTimingEmType eBusTiming);

// Power and Voltage Setting for SD Platform
//----------------------------------------------------------------------------------------------------------
void Hal_SDPLT_PowerOn(IPEmType eIP, PADEmType ePAD, U16_T u16DelayMs);
void Hal_SDPLT_PowerOff(IPEmType eIP, PADEmType ePAD, U16_T u16DelayMs);
BOOL_T Hal_SDPLT_SetPADVdd(IPEmType eIP, PADEmType ePAD, PADVddEmType ePADVdd, U16_T u16DelayMs);

// Card Detect and GPIO Setting for Card Platform
//----------------------------------------------------------------------------------------------------------
void Hal_SDPLT_CDZInitGPIO(GPIOEmType eGPIO, PADEmType ePAD, BOOL_T bEnable);
BOOL_T Hal_SDPLT_CDZGetGPIOState(GPIOEmType eGPIO, PADEmType ePAD);


#if (D_OS == D_OS__LINUX)
void Hal_SDPLT_CDZSetGPIOState(GPIOEmType eGPIO, PADEmType ePAD, BOOL_T bOutputState);
U32_T Hal_SDPLT_CDZGetGPIONum(GPIOEmType eGPIO, PADEmType ePAD);
void Hal_SDPLT_CDZSetGPIOIntAttr(GPIOEmType eGPIO, U32_T u32CDZIntNum, GPIOOptEmType eGPIOOPT);
BOOL_T Hal_SDPLT_CDZGPIOIntFilter(GPIOEmType eGPIO, PADEmType ePAD);
#endif

// MIU Setting for Card Platform
//----------------------------------------------------------------------------------------------------------
volatile U32_T Hal_SDPLT_TransMIUAddr(volatile U32_T u32DMAAddr);


#endif //End of __HAL_SD_PLATFORM_H
