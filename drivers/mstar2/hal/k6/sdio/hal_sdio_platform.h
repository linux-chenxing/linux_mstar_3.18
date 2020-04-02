///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2011 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __HAL_SDIO_PLATFORM_H__
#define __HAL_SDIO_PLATFORM_H__

// this file is use only for k6 project 2016.4.18

//#include <common.h> //printf()
//#include <sys/common/MsTypes.h>
#include <mstar/mstar_chip.h> // MSTAR_MIU0_BUS_BASE
#include <linux/irqreturn.h> // irqreturn_t
#include <linux/mmc/core.h> // struct mmc_command, struct mmc_data, struct mmc_request
#include <linux/scatterlist.h> // struct scatterlist
#include "chip_setup.h" // Chip_Clean_Cache_Range_VA_PA()
#include <chip_int.h> // interrupt vector
//int printf(const char *format, ...);


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//                               PLATFORM FUNCTION DEFINITION
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//                               DATATYPE DEFINITION
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NULL
#define NULL    ((void*)0)
#endif

#define U64  unsigned long long
#define U32  unsigned int
#define U16  unsigned short
#define U8   unsigned char
#define S32  signed int


#define BIT00 0x0001
#define BIT01 0x0002
#define BIT02 0x0004
#define BIT03 0x0008
#define BIT04 0x0010
#define BIT05 0x0020
#define BIT06 0x0040
#define BIT07 0x0080
#define BIT08 0x0100
#define BIT09 0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000


#define U32BEND2LEND(X) ( ((X&0x000000FF)<<24) + ((X&0x0000FF00)<<8) + ((X&0x00FF0000)>>8) + ((X&0xFF000000)>>24) )
#define U16BEND2LEND(X) ( ((X&0x00FF)<<8) + ((X&0xFF00)>>8) )


///////////////////////////////////////////////////////////////////////////////////////////////////
// RIU MACRO
////////////////////////////////////////////////////////////////////////////////////////////////////
#define REG_OFFSET_SHIFT_BITS           2
#define GET_REG_ADDR(x, y)              ((x)+((y) << REG_OFFSET_SHIFT_BITS))

#define FCIE_RIU_W16(addr,value)	*((volatile U16*)(uintptr_t)(addr)) = (value)
#define FCIE_RIU_R16(addr)			*((volatile U16*)(uintptr_t)(addr))
#define FCIE_RIU_W8(addr, value)	*((volatile U8*)(uintptr_t)(addr)) = (value)
#define FCIE_RIU_R8(addr)			*((volatile U8*)(uintptr_t)(addr))

// read modify write 16 bits register macro
#define FCIE_RIU_16_ON(addr,value) FCIE_RIU_W16(addr, FCIE_RIU_R16(addr)|(value))
#define FCIE_RIU_16_OF(addr,value) FCIE_RIU_W16(addr, FCIE_RIU_R16(addr)&(~(value)))
#define FCIE_RIU_W1C(addr, val)    FCIE_RIU_W16(addr, FCIE_RIU_R16(addr)&(val))

#define FCIE_RIU_8_ON(addr,value) FCIE_RIU_W8(addr, FCIE_RIU_R8(addr)|(value))
#define FCIE_RIU_8_OF(addr,value) FCIE_RIU_W8(addr, FCIE_RIU_R8(addr)&(~(value)))


////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU BASE ADDRESS
////////////////////////////////////////////////////////////////////////////////////////////////////

// kernel
//#define RIU_PM_BASE			0xFD000000
//#define RIU_BASE				0xFD200000

// uboot
//#define RIU_PM_BASE			0x1F000000
//#define RIU_BASE				0x1F200000

#define FCIE_IP					1
#define SDIO_IP					2
#define ALTERNATIVE_IP			SDIO_IP

#define SDIO_IP_VERIFY

#if defined(CONFIG_ARM)
	#define RIU_BASE                     (IO_ADDRESS(0x1F000000UL))
#elif defined(CONFIG_ARM64)
	extern ptrdiff_t   mstar_pm_base;
	#define RIU_BASE                     ((uintptr_t)(mstar_pm_base))
#endif

#define RIU_BANK_2_BASE(BANK)		(RIU_BASE+(BANK<<9))
// register num per bank --> 0x80 = 128
// register addr offset  --> 4

////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU BANK DEFINITION
////////////////////////////////////////////////////////////////////////////////////////////////////
#define RIU_BANK_PM_SLEPP		0x000EUL
#define RIU_BANK_PM_GPIO		0x000FUL
#define RIU_BANK_PM_SAR			0x0014UL
#define RIU_BANK_TIMER1			0x0030UL
#define RIU_BANK_MIU2			0x1006UL
#define RIU_BANK_CLKGEN2		0x100AUL
#define RIU_BANK_CLKGEN0		0x100BUL
#define RIU_BANK_CHIPTOP		0x101EUL
#if (ALTERNATIVE_IP == FCIE_IP)
#define RIU_BANK_SDIO0			0x1113UL
#define RIU_BANK_SDIO1			0x1114UL
#define RIU_BANK_SDIO2			0x1115UL
#elif (ALTERNATIVE_IP == SDIO_IP)
#define RIU_BANK_SDIO0			0x111FUL
#define RIU_BANK_SDIO1			0x1120UL
#define RIU_BANK_SDIO2			0x1121UL
#endif
#define RIU_BANK_CHIP_GPIO		0x102BUL
//#define RIU_BANK_SDIO_PLL		0x123EUL // these is no SDIO PLL in maxim
#define RIU_BANK_EMMC_PLL		0x123FUL // SDR mode EMMC IO 13 C3 need this bank


#define RIU_BASE_PM_SLEEP		RIU_BANK_2_BASE(RIU_BANK_PM_SLEPP)
#define RIU_BASE_PM_GPIO		RIU_BANK_2_BASE(RIU_BANK_PM_GPIO)
#define RIU_BASE_PM_SAR			RIU_BANK_2_BASE(RIU_BANK_PM_SAR)
#define RIU_BASE_TIMER1			RIU_BANK_2_BASE(RIU_BANK_TIMER1)
#define RIU_BASE_MIU2			RIU_BANK_2_BASE(RIU_BANK_MIU2)
#define RIU_BASE_CLKGEN2		RIU_BANK_2_BASE(RIU_BANK_CLKGEN2)
#define RIU_BASE_CLKGEN0		RIU_BANK_2_BASE(RIU_BANK_CLKGEN0)
#define RIU_BASE_CHIPTOP		RIU_BANK_2_BASE(RIU_BANK_CHIPTOP)
#define RIU_BASE_SDIO0			RIU_BANK_2_BASE(RIU_BANK_SDIO0) // main bank
#define RIU_BASE_SDIO1			RIU_BANK_2_BASE(RIU_BANK_SDIO1) // CIFD + CRC
#define RIU_BASE_SDIO2			RIU_BANK_2_BASE(RIU_BANK_SDIO2) // power save bank
#define RIU_BASE_CHIP_GPIO		RIU_BANK_2_BASE(RIU_BANK_CHIP_GPIO)
#define RIU_BASE_EMMC_PLL		RIU_BANK_2_BASE(RIU_BANK_EMMC_PLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

#define FCIE_REG_BASE_ADDR              (RIU_BASE_SDIO0)
#define FCIE_CMDFIFO_BASE_ADDR          (RIU_BASE_SDIO0 + (0x20<<2)) // CIFC command FIFO

#define FCIE_CIFD_FIFO_W				(RIU_BASE_SDIO1)
#define FCIE_CIFD_FIFO_R				(RIU_BASE_SDIO1 + (0x20<<2))
#define FCIE_CRC_BUF					(RIU_BASE_SDIO1 + (0x40<<2))

#define FCIE_POWER_SAVE_MODE_BASE		(RIU_BASE_SDIO2)

#define SD_USE_FCIE5		1
#define SDIO_D1_INTR_VER	2

#include "hal_reg_sdio.h"

#define RIU_UNIT_SHIFT           2


////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU_BANK_PM_SLEPP 0x000E
////////////////////////////////////////////////////////////////////////////////////////////////////
#define REG_PWM_PM_IS_GPIO				(RIU_BASE_PM_SLEEP+(0x1C<<RIU_UNIT_SHIFT))
#define IS_GPIO							(BIT05) // 0: normal use, 1: GPIO


////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU_BANK_PM_GPIO 0x000F
////////////////////////////////////////////////////////////////////////////////////////////////////
#define REG_PM_GPIO_07h					(RIU_BASE_PM_GPIO+(0x07<<RIU_UNIT_SHIFT))

// 0x07
#define PMGPIO7_OEN						(BIT00) // 0: output, 1: input
#define PMGPIO7_IN						(BIT02)


////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU_BANK_PM_SAR 0x0014
////////////////////////////////////////////////////////////////////////////////////////////////////
#define REG_PM_SAR_11h					(RIU_BASE_PM_SAR+(0x11<<RIU_UNIT_SHIFT))
#define REG_PM_SAR_12h					(RIU_BASE_PM_SAR+(0x12<<RIU_UNIT_SHIFT))

// 0x11
#define REG_SAR_AISE_1					(BIT01)
#define SAR_GPIO_1_OEN					(BIT09)

// 0x12
#define SAR_GPIO_1_IN					(BIT09)


////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU_BANK_TIMER1 0x0030
////////////////////////////////////////////////////////////////////////////////////////////////////
#define TIMER1_ENABLE                   (RIU_BASE_TIMER1+(0x20<<RIU_UNIT_SHIFT))
#define TIMER1_HIT                      (RIU_BASE_TIMER1+(0x21<<RIU_UNIT_SHIFT))
#define TIMER1_MAX_LOW                  (RIU_BASE_TIMER1+(0x22<<RIU_UNIT_SHIFT))
#define TIMER1_MAX_HIGH                 (RIU_BASE_TIMER1+(0x23<<RIU_UNIT_SHIFT))
#define TIMER1_CAP_LOW                  (RIU_BASE_TIMER1+(0x24<<RIU_UNIT_SHIFT))
#define TIMER1_CAP_HIGH                 (RIU_BASE_TIMER1+(0x25<<RIU_UNIT_SHIFT))


////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU_BANK_MIU2 0x1006
////////////////////////////////////////////////////////////////////////////////////////////////////
#define MIU2_7A							(RIU_BASE_MIU2+(0x7A << RIU_UNIT_SHIFT))
#define MIU_SELECT_BY_FCIE				(BIT09)
#define MIU_SELECT_BY_SDIO				(BIT00)


////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU_BANK_CLKGEN2 0x100A
////////////////////////////////////////////////////////////////////////////////////////////////////
#define REG_CLK_SDIO					(RIU_BASE_CLKGEN2+(0x3E<<RIU_UNIT_SHIFT))

#define BIT_SDIO_CLK_GATING				(BIT00)
#define BIT_SDIO_CLK_INVERSE			(BIT01)
#define BIT_CLKGEN_SDIO_MASK			(BIT05|BIT04|BIT03|BIT02)
#define BIT_CLKGEN_SDIO_SHIFT			2
#define BIT_SDIO_CLK_SRC_SEL			(BIT06) // 0: clk_xtal 12M, 1: clk_nfie_p1


////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU_BANK_CLKGEN0 0x100B
////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU_BANK_CHIPTOP 0x101E
////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHIPTOP_08h						(RIU_BASE_CHIPTOP+(0x08 << RIU_UNIT_SHIFT))
#define CHIPTOP_50h						(RIU_BASE_CHIPTOP+(0x50 << RIU_UNIT_SHIFT))

// CHIPTOP_08
#define REG_SDIO_CONFIG_MSK				(BIT11)
#define REG_SDIO_MODE_1					(BIT11)

// CHIPTOP_50h
#define REG_ALL_PAD_IN					(BIT15)


////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU_BANK_CHIP_GPIO 0x102B
////////////////////////////////////////////////////////////////////////////////////////////////////
#define REG_CHIPGPIO_02h				(RIU_BASE_CHIP_GPIO+(0x02 << RIU_UNIT_SHIFT))

// 0x02
#define REG_GPIO4_OEN					(BIT01)
#define REG_GPIO4_IN					(BIT02)


////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU_BANK_SDIO_PLL 0x123F
////////////////////////////////////////////////////////////////////////////////////////////////////
#define EMMC_PLL_1Ah					(RIU_BASE_EMMC_PLL+(0x1A << RIU_UNIT_SHIFT))

// 0x1A
#define REG_C2_EN_4_IO13				(BIT11)


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//                               EXTERN GLOBAL FUNCTION
//
///////////////////////////////////////////////////////////////////////////////////////////////////


#define SDIO_MODE_GPIO_PAD_BPS			1
#define SDIO_MODE_GPIO_PAD_SDR			2
#define SDIO_MODE_UNKNOWN				7

//#define SDIO_PAD_SDR104				SDIO_MODE_32BITS_MACRO_SDR104
//#define SDIO_PAD_SDR50				SDIO_MODE_32BITS_MACRO_SDR104
//#define SDIO_PAD_DDR50				SDIO_MODE_8BITS_MACRO_DDR
#define SDIO_PAD_SDR25					SDIO_MODE_GPIO_PAD_SDR
#define SDIO_PAD_SDR12					SDIO_MODE_GPIO_PAD_SDR

///////////////////////////////////////////////////////////////////////////////////////////////////
// interrutp vector number
// check mstar2/hal/[project]/cpu/chip_int.h
//       mstar2/hal/[project]/cpu/arm64/chip_int.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#define SDIO_INT_VECTOR E_IRQEXPH_SDIO


///////////////////////////////////////////////////////////////////////////////////////////////////
// new feature or patch different with other project
///////////////////////////////////////////////////////////////////////////////////////////////////
#define SDIO_NEW_R2N_PATCH // R2N mode new patch from manhattan 2015.06.09
#if (ALTERNATIVE_IP == SDIO_IP)
#define SDIO_R3_CRC_PATCH // response CRC false alarm patch 2015.06.09, SDIO has patch only
#endif

#define SDIO_ERRDET 			1

#if (defined SDIO_ERRDET) && (SDIO_ERRDET==1)
#define SDIO_ERRDET_RSP_TO		1
#define SDIO_ERRDET_RSP_CRC_ERR	1
#define SDIO_ERRDET_R_CRC_ERR	1
#define SDIO_ERRDET_W_CRC_STS	1
#define SDIO_ERRDET_R_SBIT_TO	1 // Manhattan fix counter 2 issue
#define SDIO_ERRDET_W_MIU_TO	1 //
#endif

#define ENABLE_SDIO_INTERRUPT_MODE	1
#define ENABLE_SDIO_ADMA			1

typedef struct _SKEWER {

	unsigned int u32LatchOKStart;
	unsigned int u32LatchOKEnd;
	unsigned int u32LatchBest;
	unsigned char u8_edge;

} SKEWER;

//#define CONFIG_MIU0_BUSADDR 0x20000000
//#define CONFIG_MIU1_BUSADDR 0xA0000000

U32		HalSdio_SlectBestSkew4(U32 u32_Candidate, SKEWER * pSkewer);
void	HalSdio_SetTuning(U8 u8Type, U8 u8Count);
void	HalSdio_SetTriggerLevel(U8 u8STrigLevel);


void	HalSdio_ResetIP(void);
void	HalSdio_Platform_InitChiptop(void);
U8		HalSdio_GetPadType(void);
void	HalSdio_SwitchPad(unsigned char);
void	HalSdioDelayMs(U32 u32Ms);
void	HalSdioDelayUs(U32 u32Us);
U32		HalSdio_SetClock(U32 u32Clock);
void	HalSdio_DumpDebugBus(void);
void	HalSdio_GetSBootGPIOConfig(void);
S32		HalSdio_GetCardDetect(void);
S32		HalSdio_GetWriteProtect(void);
void	HalSdio_SetCardPower(U8 u8OnOff);

#endif // #ifndef __HAL_SDIO_PLATFORM_H__
