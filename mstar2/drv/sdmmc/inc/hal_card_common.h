/***************************************************************************************************************
 *
 * FileName hal_card_platform.h
 *     @author jeremy.wang (2012/01/10)
 * Desc:
 * 	   This file is the header file of hal_card_platform_XX.c.
 *	   Every project has the same header file.
 *
 ***************************************************************************************************************/

#ifndef __HAL_CARD_PLATFORM_H
#define __HAL_CARD_PLATFORM_H

//#include "hal_card_regs.h"


//----------------------------------------------
// Platform Register Basic Address
//----------------------------------------------
//#define A_PMGPIO_BANK       GET_CARD_REG_ADDR(A_RIU_PM_BASE, 0x700)
//#define A_CHIPTOP_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x0F00)
//#define A_CLKGEN_BANK       GET_CARD_REG_ADDR(A_RIU_BASE, 0x0580)
//#define A_INTR_CTRL_BANK    GET_CARD_REG_ADDR(A_RIU_BASE, 0x0C80)


//----------------------------------------------
// Clock Level Setting (From High Speed to Low Speed)
//----------------------------------------------
// SDIO use only
#define CLK_11		200000000
#define CLK_10		100000000
#define CLK_F          48000000
#define CLK_E          43200000
#define CLK_D          40000000
#define CLK_C          36000000
#define CLK_B          32000000
#define CLK_A          27000000
#define CLK_9          20000000
#define CLK_8          12000000
#define CLK_7          300000
#define CLK_6          0
#define CLK_5          0
#define CLK_4          0
#define CLK_3          0
#define CLK_2          0
#define CLK_1          0
#define CLK_0          0


typedef enum
{
	EV_PAD1,
	EV_PAD2,
	EV_PAD3,
	EV_PAD4,
	EV_PAD5,
	EV_PAD6,
	EV_PAD7,
	EV_PAD8,
	EV_NPAD,

} PADEmType;

typedef enum
{
	EV_PULLDOWN,
	EV_PULLUP,

} PinPullEmType;

typedef enum
{
	EV_SD_CMD_DAT0_3,
	EV_SD_CDZ,
	EV_SD_WP,

} PinEmType;

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

} GPIOOptEmType;

typedef enum
{
	EV_NORVOL        = 0,
	EV_MINVOL        = 1,
	EV_LOWVOL        = 2,
	EV_HIGHVOL       = 3,
    EV_MAXVOL        = 4,

} PADVddEmType;

typedef enum
{
	EV_VDD_DUMMY     = 0,
	EV_VDD_165_195   = BIT07,
	EV_VDD_20_21     = BIT08,
	EV_VDD_21_22     = BIT09,
	EV_VDD_22_23     = BIT10,
	EV_VDD_23_24     = BIT11,
	EV_VDD_24_25     = BIT12,
	EV_VDD_25_26     = BIT13,
	EV_VDD_26_27     = BIT14,
	EV_VDD_27_28     = BIT15,
	EV_VDD_28_29     = BIT16,
	EV_VDD_29_30     = BIT17,
	EV_VDD_30_31     = BIT18,
	EV_VDD_31_32     = BIT19,
	EV_VDD_32_33     = BIT20,
	EV_VDD_33_34     = BIT21,
	EV_VDD_34_35     = BIT22,
	EV_VDD_35_36     = BIT23,
	EV_VDD_50        = BIT24,

} VddEmType;

typedef enum
{
	E_RISING        = 0,
	E_FALLING       = 1,

} E_INTR_POLARITY;

#define NONE			"\033[m"
#define RED				"\033[0;32;31m"
#define LIGHT_RED		"\033[1;31m"
#define GREEN			"\033[0;32;32m"
#define LIGHT_GREEN		"\033[1;32m"
#define BLUE			"\033[0;32;34m"
#define LIGHT_BLUE		"\033[1;34m"
#define DARY_GRAY		"\033[1;30m"
#define CYAN			"\033[0;36m"
#define LIGHT_CYAN		"\033[1;36m"
#define PURPLE			"\033[0;35m"
#define LIGHT_PURPLE	"\033[1;35m"
#define BROWN			"\033[0;33m"
#define YELLOW			"\033[1;33m"
#define LIGHT_GRAY		"\033[0;37m"
#define WHITE			"\033[1;37m"


void Hal_CARD_IPOnceSetting(IPEmType eIP);
BOOL_T Hal_CARD_Wait_Emmc_D0(void);
void Hal_CARD_IPBeginSetting(IPEmType eIP, PADEmType ePAD);
void Hal_CARD_IPEndSetting(IPEmType eIP, PADEmType ePAD);
BOOL_T Hal_CARD_Wait_D0_ForEmmc(IPEmType eIP, PADEmType ePAD);

void Hal_CARD_InitPADPin(PADEmType ePAD, BOOL_T bTwoCard);
void Hal_CARD_SetPADToPortPath(IPEmType eIP, PortEmType ePort, PADEmType ePAD, BOOL_T bTwoCard);
void Hal_CARD_PullPADPin(PADEmType ePAD, PinEmType ePin, PinPullEmType ePinPull, BOOL_T bTwoCard);

void Hal_CARD_SetClock(IPEmType eIP, U32_T u32ClkFromIPSet);
U32_T Hal_CARD_FindClockSetting(IPEmType eIP, U32_T u32ReffClk, U8_T u8PassLevel, U8_T u8DownLevel);

void Hal_CARD_SetPADPower(PADEmType ePAD, PADVddEmType ePADVdd);
void Hal_CARD_PowerOn(PADEmType ePAD, U16_T u16DelayMs);
void Hal_CARD_PowerOff(PADEmType ePAD, U16_T u16DelayMs);

void Hal_CARD_SetHighSpeed(IPEmType eIP, BOOL_T bEnable);


U32_T Hal_CARD_GetGPIONum(GPIOEmType eGPIO);
BOOL_T Hal_CARD_GetGPIOState(GPIOEmType eGPIO);
void Hal_CARD_InitGPIO(GPIOEmType eGPIO, BOOL_T bEnable);

void Hal_CARD_SetGPIOIntAttr(GPIOEmType eGPIO, GPIOOptEmType eGPIOOPT);
BOOL_T Hal_CARD_GPIOIntFilter(GPIOEmType eGPIO);

U32_T Hal_CARD_TransMIUAddr(IPEmType eIP, U64_T u64BusAddr);
void  ClockBeginSetting(void);
int  Hal_Check_Card_Pins(void);
void sd_go_18v(void);
BOOL_T Hal_GetCardWP(IPEmType eIP);
void HalCard_SetCardPower(PADEmType ePAD, U8_T u8OnOff);
void HalCard_ControlGPIO(U8_T u8GPIO, U8_T u8HighLow);
U8_T HalCard_GetWriteProtect(IPEmType eIP);
void HalCardTestBus(void);

#endif //End of __HAL_CARD_PLATFORM_H






