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

// this file is use only for manhattan project 2015.5.21

//#include <common.h> //printf()
//#include <sys/common/MsTypes.h>
#include <mstar/mstar_chip.h> // MSTAR_MIU0_BUS_BASE
#include <linux/irqreturn.h> // irqreturn_t
#include <linux/mmc/core.h> // struct mmc_command, struct mmc_data, struct mmc_request
#include <linux/scatterlist.h> // struct scatterlist
#include "chip_setup.h" // Chip_Clean_Cache_Range_VA_PA()
#include <chip_int.h> // interrupr vector

//int printf(const char *format, ...);


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//                               PLATFORM FUNCTION DEFINITION
//
///////////////////////////////////////////////////////////////////////////////////////////////////

/* Enable D-CACHE */
#define MMC_CACHE  1

//#define USE_SD_MODE
#define SDR_MODE
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


#define BIT00 0x01
#define BIT01 0x02
#define BIT02 0x04
#define BIT03 0x08
#define BIT04 0x10
#define BIT05 0x20
#define BIT06 0x40
#define BIT07 0x80

#define WBIT00 0x0001
#define WBIT01 0x0002
#define WBIT02 0x0004
#define WBIT03 0x0008
#define WBIT04 0x0010
#define WBIT05 0x0020
#define WBIT06 0x0040
#define WBIT07 0x0080
#define WBIT08 0x0100
#define WBIT09 0x0200
#define WBIT10 0x0400
#define WBIT11 0x0800
#define WBIT12 0x1000
#define WBIT13 0x2000
#define WBIT14 0x4000
#define WBIT15 0x8000


#define U32BEND2LEND(X) ( ((X&0x000000FF)<<24) + ((X&0x0000FF00)<<8) + ((X&0x00FF0000)>>8) + ((X&0xFF000000)>>24) )
#define U16BEND2LEND(X) ( ((X&0x00FF)<<8) + ((X&0xFF00)>>8) )


///////////////////////////////////////////////////////////////////////////////////////////////////
#define BY_PASS_MODE  1
//#define _8BIT_MARCO  1
//#define SDR_MODE    0

//=====================================================
// HW registers
//=====================================================
#define REG_OFFSET_SHIFT_BITS           2

#define REG_FCIE_U16(Reg_Addr)          (*(volatile U16*)((uintptr_t)(Reg_Addr)))
#define GET_REG_ADDR(x, y)              ((x)+((y) << REG_OFFSET_SHIFT_BITS))

#define REG_FCIE(reg_addr)              REG_FCIE_U16(reg_addr)
#define REG_FCIE_W(reg_addr, val)       REG_FCIE(reg_addr) = (val)
#define REG_FCIE_R(reg_addr, val)       val = REG_FCIE(reg_addr)
#define REG_FCIE_SETBIT(reg_addr, val)  REG_FCIE(reg_addr) |= (val)
#define REG_FCIE_CLRBIT(reg_addr, val)  REG_FCIE(reg_addr) &= ~(val)
#define REG_FCIE_W1C(reg_addr, val)     REG_FCIE_W(reg_addr, REG_FCIE(reg_addr)&(val))

#define FCIE_RIU_W16(addr,value) *((volatile U16*)(addr)) = (value)
#define FCIE_RIU_R16(addr)       *((volatile U16*)(addr))
// read modify write 16 bits register macro
#define FCIE_RIU_16_ON(addr,value) FCIE_RIU_W16(addr, FCIE_RIU_R16(addr)|(value))
#define FCIE_RIU_16_OF(addr,value) FCIE_RIU_W16(addr, FCIE_RIU_R16(addr)&(~(value)))




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
#define RIU_BANK_PMGPIO			0x000FUL
#define RIU_BANK_TIMER1			0x0030UL
#define RIU_BANK_MIU2			0x1006UL
#define RIU_BANK_CLKGEN0		0x100BUL
#define RIU_BANK_CHIPTOP		0x101EUL
#if (ALTERNATIVE_IP == FCIE_IP)
#define RIU_BANK_SDIO0			0x1113UL
#define RIU_BANK_SDIO1			0x1114UL
#define RIU_BANK_SDIO2			0x1115UL
#elif (ALTERNATIVE_IP == SDIO_IP)
#define RIU_BANK_SDIO0			0x120FUL
#define RIU_BANK_SDIO1			0x1228UL
#define RIU_BANK_SDIO2			0x1229UL
#endif
#define RIU_BANK_CHIP_GPIO		0x102BUL
//#define RIU_BANK_SDIO_PLL		0x123EUL // these is no SDIO PLL in manhattan
#define RIU_BANK_EMMC_PLL		0x123FUL


#define RIU_BASE_PM_SLEEP		RIU_BANK_2_BASE(RIU_BANK_PM_SLEPP)
#define RIU_BASE_PMGPIO			RIU_BANK_2_BASE(RIU_BANK_PMGPIO)
#define RIU_BASE_TIMER1			RIU_BANK_2_BASE(RIU_BANK_TIMER1)
#define RIU_BASE_MIU2			RIU_BANK_2_BASE(RIU_BANK_MIU2)
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
#define IS_GPIO							BIT05 // 0: normal use, 1: GPIO


////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU_BANK_PMGPIO 0x000F
////////////////////////////////////////////////////////////////////////////////////////////////////
#define PMGPIO_REG_07					(RIU_BASE_PMGPIO+(0x07<<RIU_UNIT_SHIFT))
#define PMGPIO7_OEN						BIT00 // 0: output, 1: input
#define PMGPIO7_IN						BIT02

#define PMGPIO_REG_08					(RIU_BASE_PMGPIO+(0x08<<RIU_UNIT_SHIFT))
#define PMGPIO8_OEN						BIT00 // 0: output, 1: input
#define PMGPIO8_IN						BIT02

#define PMGPIO_REG_0B					(RIU_BASE_PMGPIO+(0x0B<<RIU_UNIT_SHIFT))
#define PMGPIO11_OEN					BIT00 // 0: output, 1: input
#define PMGPIO11_OUT					BIT01

#define PMGPIO_REG_14					(RIU_BASE_PMGPIO+(0x14<<RIU_UNIT_SHIFT))
#define PM_PWM_OEN						BIT00 // 0: output, 1: input
#define PM_PWM_OUT						BIT01


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
#define MIU2_7A					(RIU_BASE_MIU2+(0x7A << RIU_UNIT_SHIFT))
#define MIU_SELECT_BY_SDIO		BIT10
#define MIU_SELECT_BY_FCIE		BIT09


////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU_BANK_CLKGEN0 0x100B
////////////////////////////////////////////////////////////////////////////////////////////////////
#if (ALTERNATIVE_IP == FCIE_IP)
#define REG_CLK_SDIO            (RIU_BASE_CLKGEN0+(0x64<<RIU_UNIT_SHIFT))
#elif (ALTERNATIVE_IP == SDIO_IP)
#define REG_CLK_SDIO            (RIU_BASE_CLKGEN0+(0x69<<RIU_UNIT_SHIFT))
#endif

#define BIT_SDIO_CLK_GATING		BIT0
#define BIT_SDIO_CLK_INVERSE	BIT1
#define BIT_CLKGEN_SDIO_MASK	(BIT5|BIT4|BIT3|BIT2)
#define BIT_SDIO_CLK_SRC_SEL	BIT6 // 0: clk_xtal 12M, 1: clk_nfie_p1


////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU_BANK_CHIPTOP 0x101E
////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHIPTOP_0Ah				(RIU_BASE_CHIPTOP+(0x0A << RIU_UNIT_SHIFT))
#define CHIPTOP_0Bh				(RIU_BASE_CHIPTOP+(0x0B << RIU_UNIT_SHIFT))

#define CHIPTOP_50h				(RIU_BASE_CHIPTOP+(0x50 << RIU_UNIT_SHIFT))
#define CHIPTOP_57h				(RIU_BASE_CHIPTOP+(0x57 << RIU_UNIT_SHIFT))
#define CHIPTOP_5Ah				(RIU_BASE_CHIPTOP+(0x5A << RIU_UNIT_SHIFT))
#define CHIPTOP_64h				(RIU_BASE_CHIPTOP+(0x64 << RIU_UNIT_SHIFT))
#define CHIPTOP_6Eh				(RIU_BASE_CHIPTOP+(0x6E << RIU_UNIT_SHIFT))
#define CHIPTOP_6Fh				(RIU_BASE_CHIPTOP+(0x6F << RIU_UNIT_SHIFT))
#define CHIPTOP_7Bh				(RIU_BASE_CHIPTOP+(0x7B << RIU_UNIT_SHIFT))

// CHIPTOP_50h
#define REG_ALL_PAD_IN			BIT15

// CHIPTOP_5Ah
#define REG_SD_CONFIG_MSK		(BIT09|BIT08)
#define REG_SD_MODE_1			BIT08
#define REG_SD_MODE_2			BIT09
#define REG_SD_MODE_3			(BIT09|BIT08)

// CHIPTOP_64h
#define REG_PCMADCONFIG			BIT04
#define REG_CIADCONFIG			BIT00
#define REG_PCM2CTRLCONFIG		BIT03

// CHIPTOP_7Bh
#define REG_SDIO_CONFIG_MSK		(BIT05|BIT04)
#define REG_SDIO_MODE_1			BIT04
#define REG_SDIO_MODE_2			BIT05
#define REG_SDIO_MODE_3			REG_SDIO_CONFIG_MSK


////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU_BANK_CHIP_GPIO 0x102B
////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHIP_GPIO_47		(RIU_BASE_CHIP_GPIO+(0x47 << RIU_UNIT_SHIFT)) // manhattan MST232A
#define TGPIO1_OEN 		BIT1 // low --> output
#define TGPIO1_OUT 		BIT0 // low power on, high power off


//--------------------------------macro define---------------------------------
#define ENABLE_8BIT_MACRO               0

#if !(defined(ENABLE_8BIT_MACRO) && ENABLE_8BIT_MACRO)
#define ENABLE_32BIT_MACRO              1
#endif

//--------------------------------clock gen------------------------------------

#define BIT_FCIE_CLK_XTAL			0x00
#define BIT_FCIE_CLK_20M			0x01
#define BIT_FCIE_CLK_32M			0x02
#define BIT_FCIE_CLK_36M			0x03
#define BIT_FCIE_CLK_40M			0x04
#define BIT_FCIE_CLK_43M			0x05
#define BIT_FCIE_CLK_54M			0x06
#define BIT_FCIE_CLK_62M			0x07
#define BIT_FCIE_CLK_72M			0x08
#define BIT_FCIE_CLK_86M			0x09
#define BIT_FCIE_CLK_EMMC_PLL_1X	0x0B // 8 bits macro & 32 bit macro HS200
#define BIT_FCIE_CLK_EMMC_PLL_2X	0x0C // 32 bit macroDDR & HS400
#define BIT_FCIE_CLK_300K			0x0D
#define BIT_FCIE_CLK_XTAL2			0x0E
#define BIT_FCIE_CLK_48M			0x0F

#define BIT_FCIE_CLK_52M                0x10 // emmc pll use only
#define BIT_FCIE_CLK_120M               0x11 // emmc pll use only
#define BIT_FCIE_CLK_140M               0x12 // emmc pll use only
#define BIT_FCIE_CLK_160M               0x13 // emmc pll use only
#define BIT_FCIE_CLK_200M               0x14 // emmc pll use only

#define eMMC_PLL_FLAG                   0x80
#define eMMC_PLL_CLK__20M               (0x01|eMMC_PLL_FLAG)
#define eMMC_PLL_CLK__27M               (0x02|eMMC_PLL_FLAG)
#define eMMC_PLL_CLK__32M               (0x03|eMMC_PLL_FLAG)
#define eMMC_PLL_CLK__36M               (0x04|eMMC_PLL_FLAG)
#define eMMC_PLL_CLK__40M               (0x05|eMMC_PLL_FLAG)
#define eMMC_PLL_CLK__48M               (0x06|eMMC_PLL_FLAG)
#define eMMC_PLL_CLK__52M               (0x07|eMMC_PLL_FLAG)
#define eMMC_PLL_CLK__62M               (0x08|eMMC_PLL_FLAG)
#define eMMC_PLL_CLK__72M               (0x09|eMMC_PLL_FLAG)
#define eMMC_PLL_CLK__80M               (0x0A|eMMC_PLL_FLAG)
#define eMMC_PLL_CLK__86M               (0x0B|eMMC_PLL_FLAG)
#define eMMC_PLL_CLK_100M               (0x0C|eMMC_PLL_FLAG)
#define eMMC_PLL_CLK_120M               (0x0D|eMMC_PLL_FLAG)
#define eMMC_PLL_CLK_140M               (0x0E|eMMC_PLL_FLAG)
#define eMMC_PLL_CLK_160M               (0x0F|eMMC_PLL_FLAG)
#define eMMC_PLL_CLK_200M               (0x10|eMMC_PLL_FLAG)



#define eMMC_FCIE_VALID_CLK_CNT         1// FIXME

#define PLL_SKEW4_CNT               9
#define MIN_OK_SKEW_CNT             5


////////////////////////////////////////////////////////////////////////////////////////////////////
// RIU_BANK_SDIO_PLL 0x123F
////////////////////////////////////////////////////////////////////////////////////////////////////
#define EMMC_PLL_1Ah					(RIU_BASE_EMMC_PLL+(0x1A << RIU_UNIT_SHIFT))
#define BIT_NEW_PATCH2					BIT11

//#define R_SDIO_PLL_0x1D					GET_REG_ADDR(RIU_BASE_EMMC_PLL, 0x1D)








//------------------------------


//=====================================================
// unit for HW Timer delay (unit of us)
//=====================================================

/*
#define HW_TIMER_DELAY_1us              1
#define HW_TIMER_DELAY_5us              1
#define HW_TIMER_DELAY_10us             1
#define HW_TIMER_DELAY_100us            1
#define HW_TIMER_DELAY_500us            1
#define HW_TIMER_DELAY_1ms              (1 * HW_TIMER_DELAY_1us)
#define HW_TIMER_DELAY_5ms              (5    * HW_TIMER_DELAY_1ms) / 3
#define HW_TIMER_DELAY_10ms             (10   * HW_TIMER_DELAY_1ms) / 3
#define HW_TIMER_DELAY_100ms            (100  * HW_TIMER_DELAY_1ms) / 3
#define HW_TIMER_DELAY_500ms            (500  * HW_TIMER_DELAY_1ms) / 3
#define HW_TIMER_DELAY_1s               (1000 * HW_TIMER_DELAY_1ms) / 3
*/

#define HW_TIMER_DELAY_1us              1
#define HW_TIMER_DELAY_5us              5
#define HW_TIMER_DELAY_10us             10
#define HW_TIMER_DELAY_100us            100
#define HW_TIMER_DELAY_500us            500
#define HW_TIMER_DELAY_1ms              (1000 * HW_TIMER_DELAY_1us)
#define HW_TIMER_DELAY_5ms              (5    * HW_TIMER_DELAY_1ms)
#define HW_TIMER_DELAY_10ms             (10   * HW_TIMER_DELAY_1ms)
#define HW_TIMER_DELAY_100ms            (100  * HW_TIMER_DELAY_1ms)
#define HW_TIMER_DELAY_500ms            (500  * HW_TIMER_DELAY_1ms)
#define HW_TIMER_DELAY_1s               (1000 * HW_TIMER_DELAY_1ms)


//=====================================================
// set FCIE clock
//=====================================================
// [FIXME] -->
#define FCIE_SLOWEST_CLK                BIT_FCIE_CLK_300K
#define FCIE_SLOW_CLK                   BIT_FCIE_CLK_48M
#define FCIE_DEFAULT_CLK                BIT_FCIE_CLK_48M // BIT_FCIE_CLK_36M
#define FCIE_HS200_CLK                  BIT_FCIE_CLK_200M //BIT_FCIE_CLK_52M
// <-- [FIXME]
//=====================================================
// transfer DMA Address
//=====================================================
#define MIU_BUS_WIDTH_BITS              4// 8 bytes width [FIXME]
/*
 * Important:
 * The following buffers should be large enough for a whole eMMC block
 */
// FIXME, this is only for verifing IP
#define DMA_W_ADDR                      0x00300000
#define DMA_R_ADDR                      0x00600000
#define DMA_W_SPARE_ADDR                0x00900000
#define DMA_R_SPARE_ADDR                0x00A80000
#define DMA_BAD_BLK_BUF                 0x00C00000

//int printf(const char *format, ...);


//=====================================================
// misc
//=====================================================
//#define BIG_ENDIAN
#define LITTLE_ENDIAN

#define BIT_DQS_MODE_MASK               (BIT0|BIT1|BIT2)
#define BIT_DQS_MODE_SHIFT              0





///////////////////////////////////////////////////////////////////////////////////////////////////
//
//                               EXTERN GLOBAL FUNCTION
//
///////////////////////////////////////////////////////////////////////////////////////////////////


#define SDIO_MODE_GPIO_PAD_BPS			1
#define SDIO_MODE_GPIO_PAD_SDR			2
#define SDIO_MODE_8BITS_MACRO_SDR		3
#define SDIO_MODE_8BITS_MACRO_DDR		4
#define SDIO_MODE_32BITS_MACRO_DDR		5
#define SDIO_MODE_32BITS_MACRO_SDR104	6
#define SDIO_MODE_UNKNOWN				7

#define SDIO_PAD_SDR104			SDIO_MODE_32BITS_MACRO_SDR104
#define SDIO_PAD_SDR50			SDIO_MODE_32BITS_MACRO_SDR104
#define SDIO_PAD_DDR50			SDIO_MODE_8BITS_MACRO_DDR
#if 0
#define SDIO_PAD_SDR25			SDIO_MODE_GPIO_PAD_BPS
#define SDIO_PAD_SDR12			SDIO_MODE_GPIO_PAD_BPS
#else
#define SDIO_PAD_SDR25			SDIO_MODE_GPIO_PAD_SDR
#define SDIO_PAD_SDR12			SDIO_MODE_GPIO_PAD_SDR
#endif

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

#define SDIO_ERRDET 			0

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
U32		HalSdio_clock_setting(U16 u16_ClkParam);
void	HalSdioDelayMs(U32 u32Ms);
void	HalSdioDelayUs(U32 u32Us);
U32		HalSdio_SetClock(U32 u32Clock);
void	HalSdio_DumpDebugBus(void);
S32		HalSdio_GetCardDetect(void);
S32		HalSdio_GetWriteProtect(void);
void	HalSdio_SetCardPower(U8 u8OnOff);

#endif // #ifndef __HAL_SDIO_PLATFORM_H__


