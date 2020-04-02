

#if defined(CONFIG_ARM)
	#define A_RIU_PM_BASE                     (IO_ADDRESS(0x1F000000UL))
	#define A_RIU_BASE                        (IO_ADDRESS(0x1F200000UL))
#elif defined(CONFIG_ARM64)
	extern ptrdiff_t   mstar_pm_base;
	#define A_RIU_PM_BASE                     ((uintptr_t)(mstar_pm_base))
	#define A_RIU_BASE                        ((uintptr_t)(mstar_pm_base+0x200000))
#endif



#define BANK_PM_SLEEP      	GET_CARD_REG_ADDR(A_RIU_PM_BASE, 	0x0700) //			0x000E x 0x80 =
#define BANK_PM_GPIO       	GET_CARD_REG_ADDR(A_RIU_PM_BASE, 	0x0780) //			0x000F x 0x80 =
#define REG_PM_SAR			GET_CARD_REG_ADDR(A_RIU_PM_BASE,	0x0A00) //			0x0014 x 0x80 = 0x0A00

#define REG_BANK_MIU2		GET_CARD_REG_ADDR(A_RIU_BASE,		0x0300) //			0x0006 x 0x80 = 0x0300
#define REG_BANK_CHIPTOP	GET_CARD_REG_ADDR(A_RIU_BASE, 		0x0F00) // 			0x001E x 0x80 = 0x0F00
#define CLKGEN_BANK			GET_CARD_REG_ADDR(A_RIU_BASE, 		0x0580) //			0x000B x 0x80 = 0x0580
//#define RIU_BASE_SDIO     GET_CARD_REG_ADDR(A_RIU_BASE, 	   0x10780) // SDIO		0x020F x 0x80 = 0x10780
#define REG_BANK_FCIE		GET_CARD_REG_ADDR(A_RIU_BASE,	   0x08980) // FCIE 	0x0113 x 0x80
#define	REG_BANK_GPIO 		GET_CARD_REG_ADDR(A_RIU_BASE,		0x1580) // GPIO		0x002B x 0x80 =
#define REG_BANK_EMMC_PLL	GET_CARD_REG_ADDR(A_RIU_BASE,	   0x11F80) // EMMC_PLL 0x023F x 0x80 = 0x11F80


////////////////////////////////////////////////////////////////////////////////////////////////////
// BANK_PM_GPIO BANK 0x000F
////////////////////////////////////////////////////////////////////////////////////////////////////

#define PMGPIO_REG_07			GET_CARD_REG_ADDR(BANK_PM_GPIO, 0x07)
#define PMGPIO7_OEN				BIT00 // 0: output, 1: input
#define PMGPIO7_FIQ_MASK		BIT04 // 0: enable, 1: mask
#define PMGPIO7_FIQ_RAW_STS		BIT09
#define PMGPIO7_FIQ_FINAL_STS	BIT08
#define PMGPIO7_FIQ_CLR			BIT06
#define PMGPIO7_FIQ_POLARITY	BIT07


#define PMGPIO_REG_0F			GET_CARD_REG_ADDR(BANK_PM_GPIO, 0x0F) // MUJI MST215C & 072B card detect
#define PMGPIO15_OEN			BIT00 // 0: output, 1: input
#define PMGPIO15_IN				BIT02


////////////////////////////////////////////////////////////////////////////////////////////////////
// BANK_PM_SAR BANK 0x0014
////////////////////////////////////////////////////////////////////////////////////////////////////
#define PM_SAR_REG_11			GET_CARD_REG_ADDR(REG_PM_SAR, 0x11) // SAR1
#define PM_SAR1_OEN				BIT09 // 0: output, 1: input

#define PM_SAR_REG_12			GET_CARD_REG_ADDR(REG_PM_SAR, 0x12) // SAR1
#define PM_SAR1_OUT				BIT01


////////////////////////////////////////////////////////////////////////////////////////////////////
// MIU2 BANK 0x1006
////////////////////////////////////////////////////////////////////////////////////////////////////
#define MIU2_7B					GET_CARD_REG_ADDR(REG_BANK_MIU2, 0x7B)
#define MIU_SELECT_BY_FCIE		BIT09



////////////////////////////////////////////////////////////////////////////////////////////////////
// CLKGEN BANK 0x100B
////////////////////////////////////////////////////////////////////////////////////////////////////
#define CLKGEN_FCIE				GET_CARD_REG_ADDR(CLKGEN_BANK, 0x64)
#define CLKGEN_SDIO				0 // for hal_card_common.c

#define   _48M					(0xF<<2)
#define   _43M					(0x5<<2)
#define   _36M					(0x3<<2)
#define   _32M					(0x2<<2)
#define   _27M					(0x1<<2) // 20M
#define   _18M					(0x1<<2) // 20M
#define   _300K					(0xD<<2)

// clock gen for FCIE use only
#define FCIE_CLK_F 48000000
#define FCIE_CLK_5 43200000
#define FCIE_CLK_4 40000000
#define FCIE_CLK_3 36000000
#define FCIE_CLK_2 32000000
#define FCIE_CLK_1 20000000
#define FCIE_CLK_D   300000

#define FCIE_CLK_GATING		BIT0 // 0: gate, 1, open
#define FCIE_CLK_INVERSE	BIT1
#define FCIE_CLK_SOURCE_MSK (BIT2|BIT3|BIT4|BIT5)
#define FCIE_CLK_SOURCE_SEL BIT6 // 0: xtal, 1: clk_fcie_p1


////////////////////////////////////////////////////////////////////////////////////////////////////
// CHIPTOP BANK 0x101E
////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHIPTOP_0B   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x0B)
#define CHIPTOP_0C   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x0C)
#define CHIPTOP_1D   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x1D)
#define CHIPTOP_4F			GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x4F)
#define CHIPTOP_50			GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x50)
#define CHIPTOP_57			GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x57)
#define CHIPTOP_58			GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x58)
#define CHIPTOP_5B			GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x5B)
#define CHIPTOP_64			GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x64)
#define CHIPTOP_6E			GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x6E)

////////////////////////////////////////////////////////////////////////////////////////////////////
// GPIO BANK 0x102B
////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHIP_GPIO_47		GET_CARD_REG_ADDR(REG_BANK_GPIO, 0x47) // MUJI MST215A power control
#define GPIO158_OEN 		BIT1 // low --> output
#define GPIO158_OUT 		BIT0 // low power on, high power off

#define CHIP_GPIO_52		GET_CARD_REG_ADDR(REG_BANK_GPIO, 0x52) // MUJI MST215A card detect
#define GPIO87_OEN			BIT1 // high --> input
#define GPIO87_IN			BIT2 // low --> insert, high --> remove




////////////////////////////////////////////////////////////////////////////////////////////////////
// EMMC_PLL BANK 0x123F
////////////////////////////////////////////////////////////////////////////////////////////////////

#define EMMC_PLL_REG_68			GET_CARD_REG_ADDR(REG_BANK_EMMC_PLL, 0x68)
#define EMMC_PLL_REG_6D			GET_CARD_REG_ADDR(REG_BANK_EMMC_PLL, 0x6D)
#define EMMC_PLL_REG_70			GET_CARD_REG_ADDR(REG_BANK_EMMC_PLL, 0x70)
#define EMMC_PLL_REG_71			GET_CARD_REG_ADDR(REG_BANK_EMMC_PLL, 0x71)
#define EMMC_PLL_REG_73			GET_CARD_REG_ADDR(REG_BANK_EMMC_PLL, 0x73)
#define EMMC_PLL_REG_74			GET_CARD_REG_ADDR(REG_BANK_EMMC_PLL, 0x74)

////////////////////////////////////////////////////////////////////////////////////////////////////
// ?????
////////////////////////////////////////////////////////////////////////////////////////////////////


#define	SDR50			1
#define	SDR104			2
#define	DDR50			3
//#define 	SDBUS			SDR50


