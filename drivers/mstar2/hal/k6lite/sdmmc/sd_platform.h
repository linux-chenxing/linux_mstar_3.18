

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

#define REG_BANK_MIU2		GET_CARD_REG_ADDR(A_RIU_BASE,		0x0300) //			0x0006 x 0x80 =
#define REG_BANK_CHIPTOP	GET_CARD_REG_ADDR(A_RIU_BASE, 		0x0F00) // 			0x001E x 0x80 = 0x0F00
#define CLKGEN_BANK			GET_CARD_REG_ADDR(A_RIU_BASE, 		0x0580) //			0x000B x 0x80 = 0x0580
#define CLKGEN2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE,		0x0500)
//#define RIU_BASE_SDIO     GET_CARD_REG_ADDR(A_RIU_BASE, 	   0x10780) // SDIO		0x020F x 0x80 = 0x10780
#define REG_BANK_FCIE		GET_CARD_REG_ADDR(A_RIU_BASE,	   0x08F80) // FCIE 	0x0113 x 0x80
#define	REG_BANK_GPIO 		GET_CARD_REG_ADDR(A_RIU_BASE,		0x1280) // GPIO		0x0025 x 0x80 =
#define REG_BANK_SDIO_PLL	GET_CARD_REG_ADDR(A_RIU_BASE,	   0x11F00) // SDIO_PLL 0x023E x 0x80 = 0x11F00
#define REG_BANK_ONEWAY		GET_CARD_REG_ADDR(A_RIU_BASE,	   0x30780)	// ONEWAY	0x060F x 0x80 = 0x30780
#define	REG_BANK_SAR		GET_CARD_REG_ADDR(A_RIU_PM_BASE,		0xA00) // GPIO 0x002B x 0x80 =

////////////////////////////////////////////////////////////////////////////////////////////////////
// BANK_PM_GPIO BANK 0x000F
////////////////////////////////////////////////////////////////////////////////////////////////////
#define PMGPIO_REG_01			GET_CARD_REG_ADDR(BANK_PM_GPIO, 0x01) // J2 PIN 3
#define PMGPIO_REG_02			GET_CARD_REG_ADDR(BANK_PM_GPIO, 0x02) // J2 PIN 2
#define PMGPIO_REG_0E			GET_CARD_REG_ADDR(BANK_PM_GPIO, 0x0E) // J2 PIN 7
#define OEN						BIT0
#define OUT						BIT1

#define PMGPIO_REG_07			GET_CARD_REG_ADDR(BANK_PM_GPIO, 0x07) // MUJI MST215C & 072B card detect

#define PMGPIO7_OEN				BIT00 // 0: output, 1: input
#define PMGPIO7_IN				BIT02


////////////////////////////////////////////////////////////////////////////////////////////////////
// CHIPTOP BANK 0x101E
////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHIPTOP_08   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x08)
#define CHIPTOP_0C   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x0C)
#define CHIPTOP_12   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x12)
#define CHIPTOP_19   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x19)
#define CHIPTOP_1F   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x1F)
#define CHIPTOP_42   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x42)
#define CHIPTOP_43			GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x43)
#define CHIPTOP_4F			GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x4F)
#define CHIPTOP_50			GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x50)
#define CHIPTOP_52			GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x52)
#define CHIPTOP_5A			GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x5A)
#define CHIPTOP_5D   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x5D)
#define CHIPTOP_64   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x64)
#define CHIPTOP_6E   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x6E)
#define CHIPTOP_6F   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x6F)
#define CHIPTOP_70   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x70)
#define CHIPTOP_75   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x75)
#define CHIPTOP_76   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x76)
#define CHIPTOP_77   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x77)
#define CHIPTOP_7B   		GET_CARD_REG_ADDR(REG_BANK_CHIPTOP, 0x7B)


////////////////////////////////////////////////////////////////////////////////////////////////////
// FCIE BANK 0x1113
////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////
// MIU2 BANK 0x1006
////////////////////////////////////////////////////////////////////////////////////////////////////
#define MIU2_7B				GET_CARD_REG_ADDR(MIU2_BANK, 0x7B)


////////////////////////////////////////////////////////////////////////////////////////////////////
// GPIO BANK 0x102B
////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHIP_GPIO_55		GET_CARD_REG_ADDR(REG_BANK_GPIO, 0x55) // Kano J2 pin 7 GPIO
#define GPIO_OEN			BIT5 // low --> output
#define GPIO_OUT	 		BIT4 //

#define CHIP_GPIO_47		GET_CARD_REG_ADDR(REG_BANK_GPIO, 0x47) // MUJI MST215A power control
#define GPIO158_OEN 		BIT1 // low --> output
#define GPIO158_OUT 		BIT0 // low power on, high power off

#define CHIP_GPIO_52		GET_CARD_REG_ADDR(REG_BANK_GPIO, 0x52) // MUJI MST215A card detect
#define GPIO87_OEN			BIT1 // high --> input
#define GPIO87_IN			BIT2 // low --> insert, high --> remove

#define 	GPIO_SAR1_SETTING			GET_CARD_REG_ADDR(REG_BANK_SAR,0x11)
#define   GPIO_SAR1_EN		BIT1
#define	GPIO_SAR1_OEN		BIT9

#define 	GPIO_SAR1			GET_CARD_REG_ADDR(REG_BANK_SAR,0x12)
#define	GPIO_SAR1_IN		BIT9

////////////////////////////////////////////////////////////////////////////////////////////////////
// CLKGEN BANK 0x100B
////////////////////////////////////////////////////////////////////////////////////////////////////
#define CLKGEN_FCIE			GET_CARD_REG_ADDR(CLKGEN_BANK, 0x64)
#define CLKGEN_SDIO			GET_CARD_REG_ADDR(CLKGEN2_BANK, 0x3e)

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
// MIU2 BANK 0x1006
////////////////////////////////////////////////////////////////////////////////////////////////////
#define MIU2_7A					GET_CARD_REG_ADDR(REG_BANK_MIU2, 0x7A)
#define MIU_SELECT_BY_IP		BIT10

////////////////////////////////////////////////////////////////////////////////////////////////////
// EMMC_PLL BANK 0x123F
////////////////////////////////////////////////////////////////////////////////////////////////////
#define EMMC_PLL_REG_10			GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x10)
#define EMMC_PLL_REG_1A			GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x1a)
#define EMMC_PLL_REG_1C			GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x1c)
#define EMMC_PLL_REG_1D			GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x1d)
#define EMMC_PLL_REG_37			GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x37)
#define EMMC_PLL_REG_63			GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x63)
#define EMMC_PLL_REG_68			GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x68)
#define EMMC_PLL_REG_69			GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x69)
#define EMMC_PLL_REG_6A			GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x6a)
#define EMMC_PLL_REG_6B			GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x6b)
#define EMMC_PLL_REG_6D			GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x6D)
#define EMMC_PLL_REG_6F			GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x6f)
#define EMMC_PLL_REG_70			GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x70)
#define EMMC_PLL_REG_71			GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x71)
#define EMMC_PLL_REG_73			GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x73)
#define EMMC_PLL_REG_74			GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x74)

////////////////////////////////////////////////////////////////////////////////////////////////////
// ?????
////////////////////////////////////////////////////////////////////////////////////////////////////
#define R_SDIO_PLL_0x36					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x36)
#define R_SDIO_PLL_0x37					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x37)
#define R_SDIO_PLL_0x10					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x10)
#define R_SDIO_PLL_0x1A					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x1A)
#define R_SDIO_PLL_0x1C					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x1C)
#define R_SDIO_PLL_0x1D					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x1D) // 32-bits scan only
#define R_SDIO_PLL_0x03					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x03)
#define R_SDIO_PLL_0x6C					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x6C)
#define R_SDIO_PLL_0x20					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x20)
#define R_SDIO_PLL_0x63					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x63)
#define R_SDIO_PLL_0x68					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x68)
#define R_SDIO_PLL_0x69					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x69)
#define R_SDIO_PLL_0x6A					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x6A)
#define R_SDIO_PLL_0x6B					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x6B)
#define R_SDIO_PLL_0x6D					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x6D)
#define R_SDIO_PLL_0x6F					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x6F)
#define R_SDIO_PLL_0x70					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x70)
#define R_SDIO_PLL_0x71					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x71)
#define R_SDIO_PLL_0x73					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x73)
#define R_SDIO_PLL_0x74					GET_CARD_REG_ADDR(REG_BANK_SDIO_PLL, 0x74)

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

#define reg_sdiopll_fbdiv               GET_REG_ADDR(REG_BANK_SDIO_PLL, 0x04)
#define reg_sdiopll_pdiv                GET_REG_ADDR(REG_BANK_SDIO_PLL, 0x05)
#define reg_emmc_pll_reset              GET_REG_ADDR(REG_BANK_SDIO_PLL, 0x06)
#define reg_emmc_pll_test               GET_REG_ADDR(REG_BANK_SDIO_PLL, 0x07)

#define reg_ddfset_15_00                GET_REG_ADDR(REG_BANK_SDIO_PLL, 0x18)
#define reg_ddfset_23_16                GET_REG_ADDR(REG_BANK_SDIO_PLL, 0x19)
#define reg_emmc_test                   GET_REG_ADDR(REG_BANK_SDIO_PLL, 0x1A)
#define reg_atop_patch                  GET_REG_ADDR(REG_BANK_SDIO_PLL, 0x1C)


////////////////////////////////////////////////////////////////////////////////////////////////////
// REG_BANK_ONEWAY 0x160F
////////////////////////////////////////////////////////////////////////////////////////////////////
#define ONEWAY_70				GET_CARD_REG_ADDR(REG_BANK_ONEWAY, 0x70)


#define	SDIO30			TRUE
#ifdef SDIO30
#define	SDR50			1
#define	SDR104			2
#define	DDR50			3
#define 	SDBUS			SDR104
#endif


#define SDIO_D1_INTR_MODE_SW	1
#define SDIO_D1_INTR_MODE_HW	2

#define SDIO_D1_INTR_MODE		SDIO_D1_INTR_MODE_HW


