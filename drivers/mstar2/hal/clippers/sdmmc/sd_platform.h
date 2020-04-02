

#define A_RIU_PM_BASE   	(IO_ADDRESS(0x1F000000))



//#define A_FCIE2_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08980) 	// FCIE 0x0113 x 0x80


#define BANK_PM_SLEEP      	GET_CARD_REG_ADDR(A_RIU_PM_BASE, 	0x0700) //		0x000E x 0x80 =
#define BANK_PM_GPIO       	GET_CARD_REG_ADDR(A_RIU_PM_BASE, 	0x0780) //		0x000F x 0x80 =

#define MIU2_BANK			GET_CARD_REG_ADDR(A_RIU_BASE,		0x0300) //		0x0006 x 0x80 =
#define A_CHIPTOP_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 		0x0F00) // 		0x001E x 0x80 = 0x0F00
#define CLKGEN_BANK			GET_CARD_REG_ADDR(A_RIU_BASE, 		0x0580) //		0x000B x 0x80 = 0x0580
#define RIU_BASE_FCIE       GET_CARD_REG_ADDR(A_RIU_BASE, 	   0x10780) // SDIO 0x020F x 0x80 = 0x10780
#define	REG_BANK_GPIO 		GET_CARD_REG_ADDR(A_RIU_BASE,		0x1580) // GPIO 0x002B x 0x80 =
#define SDIO_PLL_BASE		GET_CARD_REG_ADDR(A_RIU_BASE,	   0x11F00) // 		0x023E x 0x80 =
#define EMMC_PLL_BASE		GET_CARD_REG_ADDR(A_RIU_BASE,	   0x11F80) // 		0x023F x 0x80 =



////////////////////////////////////////////////////////////////////////////////////////////////////
// PM_GPIO BANK 0x000E
////////////////////////////////////////////////////////////////////////////////////////////////////
/*#define PMGPIO_OE			GET_CARD_REG_ADDR(BANK_PM_GPIO, 0x0F)
#define PMGPIO_MASK		 	GET_CARD_REG_ADDR(BANK_PM_GPIO, 0x00)
#define PMGPIO_RAWST		GET_CARD_REG_ADDR(BANK_PM_GPIO, 0x0C)
#define PMGPIO_FINALST		GET_CARD_REG_ADDR(BANK_PM_GPIO, 0x0A)
#define PMGPIO_CLR			GET_CARD_REG_ADDR(BANK_PM_GPIO, 0x04)
#define PMGPIO_POL			GET_CARD_REG_ADDR(BANK_PM_GPIO, 0x06)*/

#define PMGPIO_REG_07			GET_CARD_REG_ADDR(BANK_PM_GPIO, 0x07)

#define PMGPIO7_OEN				BIT00 // 0: output, 1: input
#define PMGPIO7_FIQ_MASK		BIT04 // 0: enable, 1: mask
#define PMGPIO7_FIQ_RAW_STS		BIT09
#define PMGPIO7_FIQ_FINAL_STS	BIT08
#define PMGPIO7_FIQ_CLR			BIT06
#define PMGPIO7_FIQ_POLARITY	BIT07

////////////////////////////////////////////////////////////////////////////////////////////////////
// CHIPTOP BANK 0x101E
////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHIPTOP_0A   		GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x0A)
#define CHIPTOP_0C   		GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x0C)
#define CHIPTOP_12   		GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x12)
#define CHIPTOP_1F   		GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x1F)
#define CHIPTOP_42   		GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x42)
#define CHIPTOP_43			GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x43)
#define CHIPTOP_4F			GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x4F)
#define CHIPTOP_50			GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x50)
#define CHIPTOP_52			GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x52)
#define CHIPTOP_5A			GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x5A)
#define CHIPTOP_7B   		GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x7B)
#define CHIPTOP_5D   		GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x5D)
#define CHIPTOP_64   		GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x64)
#define CHIPTOP_6E   		GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x6E)
#define CHIPTOP_6F   		GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x6F)
#define CHIPTOP_70   		GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x70)


////////////////////////////////////////////////////////////////////////////////////////////////////
// SDIO BANK 0x120F
////////////////////////////////////////////////////////////////////////////////////////////////////
#define SD_MODE             GET_CARD_REG_ADDR(RIU_BASE_FCIE, 0x10)
#define FCIE_2F             GET_CARD_REG_ADDR(RIU_BASE_FCIE, 0x2F)
#define FCIE_2D             GET_CARD_REG_ADDR(RIU_BASE_FCIE, 0x2D)


////////////////////////////////////////////////////////////////////////////////////////////////////
// MIU2 BANK 0x1006
////////////////////////////////////////////////////////////////////////////////////////////////////
#define MIU2_7B				GET_CARD_REG_ADDR(MIU2_BANK, 0x7B)


////////////////////////////////////////////////////////////////////////////////////////////////////
// GPIO BANK 0x102B
////////////////////////////////////////////////////////////////////////////////////////////////////
#define   SD_GPIO_29        GET_CARD_REG_ADDR(REG_BANK_GPIO, 0x29)
#define   SD_GPIO_2A        GET_CARD_REG_ADDR(REG_BANK_GPIO, 0x2A)


////////////////////////////////////////////////////////////////////////////////////////////////////
// SDIO_PLL BANK 0x123E
////////////////////////////////////////////////////////////////////////////////////////////////////
#define  SDIOPLL_10   	   	GET_CARD_REG_ADDR(SDIO_PLL_BASE, 0x10)
#define  SDIOPLL_1D   	   	GET_CARD_REG_ADDR(SDIO_PLL_BASE, 0x1D)
#define  SDIOPLL_36   	    GET_CARD_REG_ADDR(SDIO_PLL_BASE, 0x36)





#define BIT_GPIO_IN         BIT02
#define BIT_GPIO_FIQ_MASK   BIT04
#define BIT_GPIO_FIQ_CLR    BIT06
#define BIT_GPIO_FIQ_POL    BIT07
#define BIT_GPIO_FIQ_FINAL  BIT08

////////////////////////////////////////////////////////////////////////////////////////////////////
// CLKGEN BANK 0x100B
////////////////////////////////////////////////////////////////////////////////////////////////////
#define CLKGEN_FCIE			GET_CARD_REG_ADDR(CLKGEN_BANK, 0x64)
#define CLKGEN_SDIO			GET_CARD_REG_ADDR(CLKGEN_BANK, 0x69)

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

//--------------------------------sdio pll--------------------------------------

#define REG_EMMC_PLL_RX01               GET_REG_ADDR(SDIO_PLL_BASE, 0x01)
#define reg_sdiopll_0x02                GET_REG_ADDR(SDIO_PLL_BASE, 0x02)
#define BIT_SKEW1_MASK				    (BIT3|BIT2|BIT1|BIT0)
#define BIT_SKEW2_MASK				    (BIT7|BIT6|BIT5|BIT4)
#define BIT_SKEW3_MASK				    (BIT11|BIT10|BIT9|BIT8)
#define BIT_SKEW4_MASK				    (BIT15|BIT14|BIT13|BIT12)

#define reg_sdiopll_fbdiv               GET_REG_ADDR(SDIO_PLL_BASE, 0x04)
#define reg_sdiopll_pdiv                GET_REG_ADDR(SDIO_PLL_BASE, 0x05)
#define reg_emmc_pll_reset              GET_REG_ADDR(SDIO_PLL_BASE, 0x06)
#define reg_emmc_pll_test               GET_REG_ADDR(SDIO_PLL_BASE, 0x07)

#define reg_sdiopll_0x09                GET_REG_ADDR(SDIO_PLL_BASE, 0x09)

#define reg_ddfset_15_00                GET_REG_ADDR(SDIO_PLL_BASE, 0x18)
#define reg_ddfset_23_16                GET_REG_ADDR(SDIO_PLL_BASE, 0x19)
#define reg_emmc_test                   GET_REG_ADDR(SDIO_PLL_BASE, 0x1A)
#define reg_atop_patch                  GET_REG_ADDR(SDIO_PLL_BASE, 0x1C)
#define BIT_HS200_PATCH                 BIT0
#define BIT_HS_RSP_META_PATCH_HW        BIT2
#define BIT_HS_D0_META_PATCH_HW         BIT4
#define BIT_HS_DIN0_PATCH               BIT5
#define BIT_HS_EMMC_DQS_PATCH           BIT6
#define BIT_HS_RSP_MASK_PATCH           BIT7
#define BIT_DDR_RSP_PATCH               BIT8
#define BIT_ATOP_PATCH_MASK            (BIT0|BIT1|BIT2|BIT4|BIT5|BIT6|BIT7|BIT8)

#define reg_sdiopll_0x1a                GET_REG_ADDR(SDIO_PLL_BASE, 0x1A)
#define reg_sdiopll_0x1d                GET_REG_ADDR(SDIO_PLL_BASE, 0x1D)
#define reg_sdiopll_0x1e                GET_REG_ADDR(SDIO_PLL_BASE, 0x1E)
#define reg_sdiopll_0x1f                GET_REG_ADDR(SDIO_PLL_BASE, 0x1F)
#define reg_sdiopll_0x20                GET_REG_ADDR(SDIO_PLL_BASE, 0x20)
#define REG_EMMC_PLL_RX30               GET_REG_ADDR(SDIO_PLL_BASE, 0x30)
#define REG_EMMC_PLL_RX32               GET_REG_ADDR(SDIO_PLL_BASE, 0x32)
#define REG_EMMC_PLL_RX33               GET_REG_ADDR(SDIO_PLL_BASE, 0x33)
#define REG_EMMC_PLL_RX34               GET_REG_ADDR(SDIO_PLL_BASE, 0x34)
#define reg_sdiopll_0x03				GET_REG_ADDR(SDIO_PLL_BASE, 0x03)
#define reg_sdiopll_0x1d				GET_REG_ADDR(SDIO_PLL_BASE, 0x1D)
#define reg_sdiopll_0x36                GET_REG_ADDR(SDIO_PLL_BASE, 0x36)
#define reg_sdiopll_0x63                GET_REG_ADDR(SDIO_PLL_BASE, 0x63)
#define reg_sdiopll_0x68                GET_REG_ADDR(SDIO_PLL_BASE, 0x68)
#define reg_sdiopll_0x69                GET_REG_ADDR(SDIO_PLL_BASE, 0x69)
#define reg_sdiopll_0x6a                GET_REG_ADDR(SDIO_PLL_BASE, 0x6A)
#define reg_sdiopll_0x6b                GET_REG_ADDR(SDIO_PLL_BASE, 0x6B)
#define reg_sdiopll_0x6c                GET_REG_ADDR(SDIO_PLL_BASE, 0x6C)
#define reg_sdiopll_0x6d                GET_REG_ADDR(SDIO_PLL_BASE, 0x6D)
#define reg_sdiopll_0x6f                GET_REG_ADDR(SDIO_PLL_BASE, 0x6F)
#define reg_sdiopll_0x70                GET_REG_ADDR(SDIO_PLL_BASE, 0x70)
#define reg_sdiopll_0x71                GET_REG_ADDR(SDIO_PLL_BASE, 0x71)
#define reg_sdiopll_0x73                GET_REG_ADDR(SDIO_PLL_BASE, 0x73)
#define reg_sdiopll_0x74                GET_REG_ADDR(SDIO_PLL_BASE, 0x74)

#define eMMC_PLL_FLAG                   	0x80
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

#define MIU2_BASE		GET_CARD_REG_ADDR(A_RIU_BASE,0x300)
#define MIU2_79				  GET_REG_ADDR(MIU2_BANK, 0x79)

#define	SDR50			1
#define	SDR104			2
#define	DDR50			3
//#define 	SDBUS			SDR50

