/***************************************************************************************************************
 *
 * FileName hal_card_regs.h
 *     @author jeremy.wang (2010/10/14)
 * Desc:
 * 	   This file is the header file of hal_card_regs.c.
 *
 * 	   For Base RegisterSetting:
 * 	   (1) BASE Register Address
 *	   (2) BASE Register Operation
 * 	   (3) BASE FCIE Reg Meaning Position
 *
 *	   P.S. If you want to use only IP for single card or dual cards,
 * 			please modify FCIE1 and FCIE2 setting to the same reg position.
 *
 ***************************************************************************************************************/


#include "hal_card_base.h"



#if (D_PROJECT == D_PROJECT__CLIPPERS)

//###########################################################################################################

//#define A_RIU_PM_BASE   	(0x1F000000)
#define A_RIU_BASE			(IO_ADDRESS(0x1F200000))
//#define EMMC_PLL_BASE		GET_REG_ADDR(A_RIU_BASE, 0x11F00)

#define A_FCIE1_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x10780) 		// SDIO		0x020F x 0x80
#define A_FCIE1_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x107A0)     	//
#define A_FCIE1_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x11400)     	//			0x0228 x 0x80
#define A_FCIE1_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x11480) 		//			0x0229 x 0x80

#define A_FCIE2_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08980) 		// FCIE		0x0113 x 0x80
#define A_FCIE2_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x089A0)     	//
#define A_FCIE2_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08A00)     	// 			0x0114 x 0x80
#define A_FCIE2_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08A80) 		// 			0x0115 x 0x80

#elif (D_PROJECT == D_PROJECT__MUJI)

//###########################################################################################################

#if defined(CONFIG_ARM)
	#define A_RIU_PM_BASE                     (IO_ADDRESS(0x1F000000UL))
	#define A_RIU_BASE                        (IO_ADDRESS(0x1F200000UL))
#elif defined(CONFIG_ARM64)
	extern ptrdiff_t   mstar_pm_base;
	#define A_RIU_PM_BASE                     ((uintptr_t)(mstar_pm_base))
	#define A_RIU_BASE                        ((uintptr_t)(mstar_pm_base+0x200000))
#endif

//#define EMMC_PLL_BASE		GET_REG_ADDR(A_RIU_BASE, 0x11F00)

#define A_FCIE1_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x10780) 		// SDIO		0x020F x 0x80
#define A_FCIE1_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x107A0)     	// CIFC
#define A_FCIE1_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x11400)     	//			0x0228 x 0x80
#define A_FCIE1_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x11480) 		//			0x0229 x 0x80

#define A_FCIE2_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08980) 		// FCIE		0x0113 x 0x80
#define A_FCIE2_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x089A0)     	// CIFC
#define A_FCIE2_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08A00)     	// 			0x0114 x 0x80
#define A_FCIE2_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08A80) 		// 			0x0115 x 0x80

#elif (D_PROJECT == D_PROJECT__MONET)

//###########################################################################################################

#if defined(CONFIG_ARM)
	#define A_RIU_PM_BASE                     (IO_ADDRESS(0x1F000000UL))
	#define A_RIU_BASE                        (IO_ADDRESS(0x1F200000UL))
#elif defined(CONFIG_ARM64)
	extern ptrdiff_t   mstar_pm_base;
	#define A_RIU_PM_BASE                     ((uintptr_t)(mstar_pm_base))
	#define A_RIU_BASE                        ((uintptr_t)(mstar_pm_base+0x200000))
#endif

//#define EMMC_PLL_BASE		GET_REG_ADDR(A_RIU_BASE, 0x11F00)

#define A_FCIE1_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x10780) 		// SDIO		0x020F x 0x80
#define A_FCIE1_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x107A0)     	// CIFC
#define A_FCIE1_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x11400)     	//			0x0228 x 0x80
#define A_FCIE1_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x11480) 		//			0x0229 x 0x80

#define A_FCIE2_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08980) 		// FCIE		0x0113 x 0x80
#define A_FCIE2_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x089A0)     	// CIFC
#define A_FCIE2_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08A00)     	// 			0x0114 x 0x80
#define A_FCIE2_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08A80) 		// 			0x0115 x 0x80

#elif (D_PROJECT == D_PROJECT__MANHATTAN)

//###########################################################################################################

#if defined(CONFIG_ARM)
	#define A_RIU_PM_BASE                     (IO_ADDRESS(0x1F000000UL))
	#define A_RIU_BASE                        (IO_ADDRESS(0x1F200000UL))
#elif defined(CONFIG_ARM64)
	extern ptrdiff_t   mstar_pm_base;
	#define A_RIU_PM_BASE                     ((uintptr_t)(mstar_pm_base))
	#define A_RIU_BASE                        ((uintptr_t)(mstar_pm_base+0x200000))
#endif

//#define EMMC_PLL_BASE		GET_REG_ADDR(A_RIU_BASE, 0x11F00)

#define A_FCIE1_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x10780) 		// SDIO		0x020F x 0x80
#define A_FCIE1_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x107A0)     	// CIFC
#define A_FCIE1_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x11400)     	//			0x0228 x 0x80
#define A_FCIE1_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x11480) 		//			0x0229 x 0x80

#define A_FCIE2_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08980) 		// FCIE		0x0113 x 0x80
#define A_FCIE2_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x089A0)     	// CIFC
#define A_FCIE2_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08A00)     	// 			0x0114 x 0x80
#define A_FCIE2_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08A80) 		// 			0x0115 x 0x80

#elif (D_PROJECT == D_PROJECT__MASERATI)

//###########################################################################################################

#if defined(CONFIG_ARM)
	#define A_RIU_PM_BASE                     (IO_ADDRESS(0x1F000000UL))
	#define A_RIU_BASE                        (IO_ADDRESS(0x1F200000UL))
#elif defined(CONFIG_ARM64)
	extern ptrdiff_t   mstar_pm_base;
	#define A_RIU_PM_BASE                     ((uintptr_t)(mstar_pm_base))
	#define A_RIU_BASE                        ((uintptr_t)(mstar_pm_base+0x200000))
#endif

//#define EMMC_PLL_BASE		GET_REG_ADDR(A_RIU_BASE, 0x11F00)

#define A_FCIE1_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x10780) 		// SDIO		0x020F x 0x80
#define A_FCIE1_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x107A0)     	// CIFC
#define A_FCIE1_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x11400)     	//			0x0228 x 0x80
#define A_FCIE1_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x11480) 		//			0x0229 x 0x80

#define A_FCIE2_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08980) 		// FCIE		0x0113 x 0x80
#define A_FCIE2_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x089A0)     	// CIFC
#define A_FCIE2_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08A00)     	// 			0x0114 x 0x80
#define A_FCIE2_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08A80) 		// 			0x0115 x 0x80


#elif (D_PROJECT == D_PROJECT__KANO)

//###########################################################################################################

#if defined(CONFIG_ARM)
	#define A_RIU_PM_BASE                     (IO_ADDRESS(0x1F000000UL))
	#define A_RIU_BASE                        (IO_ADDRESS(0x1F200000UL))
#elif defined(CONFIG_ARM64)
	extern ptrdiff_t   mstar_pm_base;
	#define A_RIU_PM_BASE                     ((uintptr_t)(mstar_pm_base))
	#define A_RIU_BASE                        ((uintptr_t)(mstar_pm_base+0x200000))
#endif

//#define EMMC_PLL_BASE		GET_REG_ADDR(A_RIU_BASE, 0x11F00)

#define A_FCIE1_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x8F80) 		// SDIO		0x020F x 0x80
#define A_FCIE1_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x8FA0)     	// CIFC
#define A_FCIE1_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x9000)     	//			0x0228 x 0x80
#define A_FCIE1_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x9080) 		//			0x0229 x 0x80

#define A_FCIE2_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08980) 		// FCIE		0x0113 x 0x80
#define A_FCIE2_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x089A0)     	// CIFC
#define A_FCIE2_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08A00)     	// 			0x0114 x 0x80
#define A_FCIE2_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08A80) 		// 			0x0115 x 0x80

#elif (D_PROJECT == D_PROJECT__MESSI)

//###########################################################################################################

#if defined(CONFIG_ARM)
	#define A_RIU_PM_BASE                     (IO_ADDRESS(0x1F000000UL))
	#define A_RIU_BASE                        (IO_ADDRESS(0x1F200000UL))
#elif defined(CONFIG_ARM64)
	extern ptrdiff_t   mstar_pm_base;
	#define A_RIU_PM_BASE                     ((uintptr_t)(mstar_pm_base))
	#define A_RIU_BASE                        ((uintptr_t)(mstar_pm_base+0x200000))
#endif

//#define EMMC_PLL_BASE		GET_REG_ADDR(A_RIU_BASE, 0x11F00)

#define A_FCIE1_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x10780) 		// SDIO		0x020F x 0x80
#define A_FCIE1_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x107A0)     	// CIFC
#define A_FCIE1_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x11400)     	//			0x0228 x 0x80
#define A_FCIE1_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x11480) 		//			0x0229 x 0x80

#define A_FCIE2_0_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08980) 		// FCIE		0x0113 x 0x80
#define A_FCIE2_1_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x089A0)     	// CIFC
#define A_FCIE2_2_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08A00)     	// 			0x0114 x 0x80
#define A_FCIE2_3_BANK		GET_CARD_REG_ADDR(A_RIU_BASE, 0x08A80) 		// 			0x0115 x 0x80

#endif

//***********************************************************************************************************
// (2) BASE Register Operation
//***********************************************************************************************************

//###########################################################################################################
//###########################################################################################################
#include <asm/io.h>  //IO_ADDRESS		// IO Mapping Address

#define D_MIU_WIDTH			8			// MIU Info
#define REG_OFFSET_BITS		2			// Register Offset Byte (32bit) = 4Bytes
#define GET_CARD_REG_ADDR(x, y)        ((x)+((y) << REG_OFFSET_BITS))
#define GET_REG_ADDR(x, y)              ((x)+((y) << REG_OFFSET_BITS))

#define FCIE_RIU_W16(addr,value) *((volatile unsigned short*)((uintptr_t)(addr))) = (value)
#define FCIE_RIU_R16(addr)       *((volatile unsigned short*)((uintptr_t)(addr)))
// read modify write 16 bits register macro
#define FCIE_RIU_16_ON(addr,value) FCIE_RIU_W16(addr, FCIE_RIU_R16(addr)|(value))
#define FCIE_RIU_16_OF(addr,value) FCIE_RIU_W16(addr, FCIE_RIU_R16(addr)&(~(value)))


volatile ULONG_T Hal_CREG_GET_REG_BANK(IPEmType eIP, IPBankEmType eBANK);
void Hal_CREG_SET_PORT(IPEmType eIP, PortEmType ePort);
volatile PortEmType Hal_CREG_GET_PORT(IPEmType eIP);

#define GET_CARD_BANK           Hal_CREG_GET_REG_BANK
#define GET_CARD_PORT           Hal_CREG_GET_PORT
#define SET_CARD_PORT           Hal_CREG_SET_PORT

//***********************************************************************************************************
// (3) BASE FCIE Reg Meaning Position
//***********************************************************************************************************
#define reg_emmcpll_0x02                GET_REG_ADDR(EMMC_PLL_BASE, 0x02)
#define reg_emmcpll_0x09                GET_REG_ADDR(EMMC_PLL_BASE, 0x09)
#define reg_emmcpll_0x1d                GET_REG_ADDR(EMMC_PLL_BASE, 0x1d)

#define reg_emmcpll_0x1e                GET_REG_ADDR(EMMC_PLL_BASE, 0x1e)
#define reg_emmcpll_0x1f                GET_REG_ADDR(EMMC_PLL_BASE, 0x1f)

#define reg_emmcpll_0x63                GET_REG_ADDR(EMMC_PLL_BASE, 0x63)
#define reg_emmcpll_0x68                GET_REG_ADDR(EMMC_PLL_BASE, 0x68)
#define reg_emmcpll_0x69                GET_REG_ADDR(EMMC_PLL_BASE, 0x69)
#define reg_emmcpll_0x6a                GET_REG_ADDR(EMMC_PLL_BASE, 0x6a)
#define reg_emmcpll_0x6b                GET_REG_ADDR(EMMC_PLL_BASE, 0x6b)
#define reg_emmcpll_0x6c                GET_REG_ADDR(EMMC_PLL_BASE, 0x6c)
#define reg_emmcpll_0x6d                GET_REG_ADDR(EMMC_PLL_BASE, 0x6d)
#define reg_emmcpll_0x6f                GET_REG_ADDR(EMMC_PLL_BASE, 0x6f)
#define reg_emmcpll_0x70                GET_REG_ADDR(EMMC_PLL_BASE, 0x70)
#define reg_emmcpll_0x71                GET_REG_ADDR(EMMC_PLL_BASE, 0x71)
#define reg_emmcpll_0x73                GET_REG_ADDR(EMMC_PLL_BASE, 0x73)
#define reg_emmcpll_0x74                GET_REG_ADDR(EMMC_PLL_BASE, 0x74)

//============================================
#define FCIE_REG_BASE_ADDR				A_FCIE1_0_BANK		// SDIO
#define FCIE_CMDFIFO_BASE_ADDR          A_FCIE1_1_BANK

//------------------------------------------------------------------
#define FCIE_MIE_EVENT                  GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x00)
#define FCIE_MIE_INT_EN                 GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x01)
#define FCIE_MMA_PRI_REG                GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x02)
#define FCIE_MIU_DMA_ADDR_15_0          GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x03)
#define FCIE_MIU_DMA_ADDR_31_16         GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x04)
#define FCIE_MIU_DMA_LEN_15_0           GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x05)
#define FCIE_MIU_DMA_LEN_31_16          GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x06)
#define FCIE_MIE_FUNC_CTL               GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x07)
#define FCIE_JOB_BL_CNT                 GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x08)
#define FCIE_BLK_SIZE                   GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x09)
#define FCIE_CMD_RSP_SIZE               GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x0A)
#define FCIE_SD_MODE                    GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x0B)
#define FCIE_SD_CTRL                    GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x0C)
#define FCIE_SD_STATUS                  GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x0D)
#define FCIE_BOOT_CONFIG                GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x0E)
#define FCIE_DDR_MODE                   GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x0F)
#define FCIE_DDR_TOGGLE_CNT             GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x10)
#define FCIE_SDIO_MOD                   GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x11)
//#define FCIE_SBIT_TIMER               GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x11)
#define FCIE_RSP_SHIFT_CNT              GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x12)
#define FCIE_RX_SHIFT_CNT               GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x13)
#define FCIE_ZDEC_CTL0                  GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x14)
#define FCIE_TEST_MODE                  GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x15)
#define FCIE_MMA_BANK_SIZE              GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x16)
#define FCIE_WR_SBIT_TIMER              GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x17)
//#define FCIE_SDIO_MODE                GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x17)
#define FCIE_RD_SBIT_TIMER              GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x18)
//#define FCIE_DEBUG_BUS0               GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x1E)
//#define FCIE_DEBUG_BUS1               GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x1F)
#define NC_CIFD_EVENT                   GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x30)
#define NC_CIFD_INT_EN                  GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x31)
#define FCIE_PWR_RD_MASK                GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x34)
#define FCIE_PWR_SAVE_CTL               GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x35)
#define FCIE_BIST                       GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x36)
#define FCIE_BOOT                       GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x37)
#define FCIE_EMMC_DEBUG_BUS0            GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x38)
#define FCIE_EMMC_DEBUG_BUS1            GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x39)
#define FCIE_CLK_EN						GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x3E)
#define FCIE_RST                        GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x3F)
//#define NC_WIDTH                      GET_REG_ADDR(FCIE_REG_BASE_ADDR, 0x41)

#define FCIE_CMDFIFO_ADDR(u16_pos)      GET_REG_ADDR(FCIE_CMDFIFO_BASE_ADDR, u16_pos)
#define FCIE_CMDFIFO_BYTE_CNT           0x12// 9 x 16 bits

#define NC_WBUF_CIFD_ADDR(u16_pos)      GET_REG_ADDR(FCIE_NC_WBUF_CIFD_BASE, u16_pos) // 32 x 16 bits SW Read only
#define NC_RBUF_CIFD_ADDR(u16_pos)      GET_REG_ADDR(FCIE_NC_RBUF_CIFD_BASE, u16_pos) // 32 x 16 bits	SW write/read
#define NC_CRCBUF_CIFD_ADDR(u16_pos)      GET_REG_ADDR(FCIE_NC_CRCBUF_CIFD_BASE, u16_pos) // 32 x 16 bits SW Read only

#define NC_CIFD_ADDR(u16_pos)           NC_RBUF_CIFD_ADDR(u16_pos)

#define NC_CIFD_WBUF_BYTE_CNT           0x40 // 32 x 16 bits
#define NC_CIFD_RBUF_BYTE_CNT           0x40 // 32 x 16 bits

#define FCIE_CIFD_BYTE_CNT              0x40 // 256 x 16 bits

//------------------------------------------------------------------
/* FCIE_MIE_EVENT  0x00 */
/* FCIE_MIE_INT_EN 0x01 */ // edit this reg carefully
#define BIT_DMA_END                     BIT0
#define BIT_SD_CMD_END                  BIT1
#define BIT_ERR_STS                     BIT2
#define BIT_SDIO_INT                    BIT3
#define BIT_BUSY_END_INT                BIT4
#define BIT_R2N_RDY_INT                 BIT5
#define BIT_CARD_CHANGE                 BIT6
#define BIT_CARD2_CHANGE                BIT7
#define BIT_ALL_CARD_INT_EVENTS         (BIT_DMA_END|BIT_SD_CMD_END|BIT_ERR_STS|BIT_BUSY_END_INT)

/* FCIE_MMA_PRI_REG 0x02 */
#define BIT_MIU_R_PRI                   BIT0
#define BIT_MIU_W_PRI                   BIT1
#define BIT_MIU_SELECT_MASK             (BIT3|BIT2)
#define BIT_MIU1_SELECT                 BIT2
#define BIT_MIU2_SELECT                 BIT3
#define BIT_MIU3_SELECT                 (BIT3|BIT2)
//#define BIT_DATA_SCRAMBLE             BIT3
#define BIT_MIU_BUS_TYPE_MASK           (BIT4|BIT5
#define BIT_MIU_BURST1				    (~BIT_MIU_BUS_TYPE_MASK)
#define BIT_MIU_BURST2				    (BIT4)
#define BIT_MIU_BURST4				    (BIT5)
#define BIT_MIU_BURST8				    (BIT4|BIT5)

/* FCIE_FUNC_CTL 0x07 */
#define BIT_EMMC_EN                     BIT0
#define BIT_SD_EN                       BIT1
#define BIT_SDIO_MOD                    BIT2
#define BIT_EMMC_ACTIVE					BIT12 // dummy register, software define for eMMC and SD driver share FCIE use

/* FCIE_BLK_CNT 0x08 */
#define BIT_SD_JOB_BLK_CNT_MASK         (BIT13-1)

/* FCIE_CMD_RSP_SIZE 0x0A */
#define BIT_RSP_SIZE_MASK               (BIT6-1)
#define BIT_CMD_SIZE_MASK               (BIT13|BIT12|BIT11|BIT10|BIT9|BIT8)
#define BIT_CMD_SIZE_SHIFT              8

/* FCIE_SD_MODE 0x0B */
#define R_SD_CLK_EN             BIT00

#define BIT_CLK_EN                      BIT0
#define BIT_SD_DATA_WIDTH_MASK          (BIT2|BIT1)
#define BIT_SD_DATA_WIDTH_1             0
#define BIT_SD_DATA_WIDTH_4             BIT1
#define BIT_SD_DATA_WIDTH_8             BIT2
#define BIT_SD_DATA_CIFD                BIT4
#define BIT_SD_DMA_R_CLK_STOP           BIT7 // 0: keep clock after DMA finish, 1: stop clock after DMA finish
#define BIT_DIS_BLOCK_CLK_STOP           BIT9 // for 8 bits 32 bit macron, 0: auto stop, 1: disable auto stop
#define BIT_SD_DEFAULT_MODE_REG         (BIT_CLK_EN)

/* FCIE_SD_CTRL 0x0C */
#define BIT_SD_RSPR2_EN                 BIT0
#define BIT_SD_RSP_EN                   BIT1
#define BIT_SD_CMD_EN                   BIT2
#define BIT_SD_DTRX_EN                  BIT3
#define BIT_SD_DAT_DIR_W                BIT4
#define BIT_ADMA_EN                     BIT5
#define BIT_JOB_START                   BIT6
#define BIT_CHK_CMD                     BIT7
#define BIT_BUSY_DET_ON                 BIT8
#define BIT_ERR_DET_ON                  BIT9

/* FCIE_SD_STATUS 0x0D */
#define BIT_SD_R_CRC_ERR				BIT0
#define BIT_SD_W_CRC_ERR				BIT1
#define BIT_SD_W_TIMEOUT				BIT2
#define BIT_SD_RSP_TIMEOUT				BIT3
#define BIT_SD_RSP_CRC_ERR				BIT4
#define BIT_SD_R_TIMEOUT				BIT5
#define BIT_SD_ERR_BITS					(BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)

#define BIT_SD_CARD_BUSY				BIT6 // polling this bit, not bit8

#define BIT_SD_D0                       BIT8
#define BIT_SD_CARD_D0_ST               BIT8
#define BIT_SD_CARD_D1_ST               BIT9
#define BIT_SD_CARD_D2_ST               BIT10
#define BIT_SD_CARD_D3_ST               BIT11
#define BIT_SD_CARD_D4_ST               BIT12
#define BIT_SD_CARD_D5_ST               BIT13
#define BIT_SD_CARD_D6_ST               BIT14
#define BIT_SD_CARD_D7_ST               BIT15

/* FCIE_BOOT_CONFIG 0x0E */
#define BIT_EMMC_RSTZ                   BIT0
#define BIT_EMMC_RSTZ_EN                BIT1
#define BIT_BOOT_MODE_EN                BIT2
//#define BIT_BOOT_END                  BIT3

/* FCIE_DDR_MODE 0x0F */
//#define BIT_DQS_DELAY_CELL_MASK       (BIT0|BIT1|BIT2|BIT3)
//#define BIT_DQS_DELAY_CELL_SHIFT      0
#define BIT_MACRO_MODE_MASK             (BIT7|BIT8|BIT12|BIT13|BIT14|BIT15)
#define BIT_8BIT_MACRO_EN               BIT7
#define BIT_DDR_EN                      BIT8
//#define BIT_SDR200_EN                 BIT9
//#define BIT_BYPASS_EN                 BIT10
//#define BIT_SDRIN_BYPASS_EN           BIT11
#define BIT_32BIT_MACRO_EN              BIT12
#define BIT_PAD_IN_SEL_SD               BIT13
#define BIT_FALL_LATCH                  BIT14
#define BIT_PAD_IN_MASK                 BIT15

/* FCIE_TOGGLE_CNT 0x10 */
#define BITS_8_MACRO32_SDR_TOGGLE_CNT   0x210
#define BITS_4_MACRO32_SDR_TOGGLE_CNT   0x410

#define BITS_8_MACRO32_DDR_TOGGLE_CNT   0x110
#define BITS_4_MACRO32_DDR_TOGGLE_CNT   0x210

/* FCIE_SDIO_MOD 0x11 */
#define BIT_REG_SDIO_MOD_MASK           (BIT01|BIT00)
#define SDIO_INT_MOD_0					0
#define SDIO_INT_MOD_1					BIT00
#define SDIO_INT_MOD_2					BIT01 // single block transfer
#define SDIO_INT_MOD_3					(BIT01|BIT00) // need to set back mode 0 after job finish in SW mode

#define BIT_SDIO_INT_SW_MODE			BIT02
#define BIT_SDIO_DET_INT_SRC            BIT03 // 0: edge, 1: level trigger
#define BIT_SDIO_BIT10					BIT10

#define BITS_SDIO_INT_TUNE				BIT06|BIT05|BIT04
#define BITS_SDIO_INT_TUNE_CLR			BIT09|BIT08|BIT07

#define BIT_RD_WAIT_EN					BIT11
#define BIT_BLK_GAP_DIS					BIT12
#define BIT_INT_STOP_DMA				BIT13 // if set, not detect D1 interrupt after DMA finish. Only effect mode 3
#define BIT_INT_TUNE_SW					BIT14
#define BIT_INT_ASYNC_EN				BIT15

/* FCIE_RSP_SHIFT_CNT 0x12 */
#define BIT_RSP_SHIFT_TUNE_MASK         (BIT4 - 1)
#define BIT_RSP_SHIFT_SEL               BIT4			/*SW or HW by default  0*/

/* FCIE_RX_SHIFT_CNT 0x13 */
#define BIT_RSTOP_SHIFT_TUNE_MASK       (BIT4 - 1)
#define BIT_RSTOP_SHIFT_SEL             BIT4
#define BIT_WRSTS_SHIFT_TUNE_MASK       (BIT8|BIT9|BIT10|BIT11)
#define BIT_WRSTS_SHIFT_SEL             BIT12

/* FCIE_ZDEC_CTL0 0x14 */
#define BIT_ZDEC_EN                     BIT0
#define BIT_SD2ZDEC_PTR_CLR             BIT1

/* FCIE_TEST_MODE 0x15 */
#define BIT_SDDR1                       BIT0
#define BIT_DEBUG_MODE_MASK             (BIT3|BIT2|BIT1)
#define BIT_DEBUG_MODE_SHIFT            1
#define BIT_BIST_MODE                   BIT4
//#define BIT_DS_TESTEN                 BIT1
//#define BIT_TEST_MODE                 BIT2
//#define BIT_DEBUG_MODE_MASK           BIT3|BIT4|BIT5
//#define BIT_DEBUG_MODE_SHIFT          3
//#define BIT_TEST_MIU                  BIT6
//#define BIT_TEST_MIE                  BIT7
//#define BIT_TEST_MIU_STS              BIT8
//#define BIT_TEST_MIE_STS              BIT9
//#define BIT_BIST_MODE                 BIT10

/* FCIE_WR_SBIT_TIMER 0x17 */
#define BIT_WR_SBIT_TIMER_MASK          (BIT15-1)
#define BIT_WR_SBIT_TIMER_EN            BIT15

/* FCIE_RD_SBIT_TIMER 0x18 */
#define BIT_RD_SBIT_TIMER_MASK          (BIT15-1)
#define BIT_RD_SBIT_TIMER_EN            BIT15

/* 0x2F */
#define BIT_SDIO_DET_ON                 BIT0 // after detect, auto clear.

/* NC_CIFD_EVENT 0x30 */
#define BIT_WBUF_FULL                   BIT0
#define BIT_WBUF_EMPTY_TRI              BIT1
#define BIT_RBUF_FULL_TRI               BIT2
#define BIT_RBUF_EMPTY                  BIT3

/* NC_CIFD_INT_EN 0x31 */
#define BIT_WBUF_FULL_INT_EN            BIT0
#define BIT_RBUF_EMPTY_INT_EN           BIT1
#define BIT_F_WBUF_FULL_INT             BIT2
#define BIT_F_RBUF_EMPTY_INT            BIT3

/* FCIE_PWR_SAVE_CTL 0x35 */
#define BIT_POWER_SAVE_MODE             BIT0
#define BIT_SD_POWER_SAVE_RIU           BIT1
#define BIT_POWER_SAVE_MODE_INT_EN      BIT2
#define BIT_SD_POWER_SAVE_RST           BIT3
#define BIT_POWER_SAVE_INT_FORCE        BIT4
#define BIT_RIU_SAVE_EVENT              BIT5
#define BIT_RST_SAVE_EVENT              BIT6
#define BIT_BAT_SAVE_EVENT              BIT7
#define BIT_BAT_SD_POWER_SAVE_MASK      BIT8
#define BIT_RST_SD_POWER_SAVE_MASK      BIT9
#define BIT_POWER_SAVE_MODE_INT         BIT15

/* FCIE_BOOT 0x37 */
#define BIT_NAND_BOOT_EN                BIT0
#define BIT_BOOTSRAM_ACCESS_SEL         BIT1

/* FCIE_RST 0x3E */
#define BIT_FCIE_CLK_EN					BIT0
#define BIT_TEST_CLK					BIT1
#define BIT_TEST_MIU_STS				BIT2
#define BIT_TEST_MIE_STS				BIT3
#define BIT_TEST_MCU_STS				BIT4
#define BITS_TEST_OK					(BIT_TEST_MIU_STS|BIT_TEST_MIE_STS|BIT_TEST_MCU_STS)

/* FCIE_RST 0x3F */

#define BIT_FCIE_SOFT_RST_n             BIT0
#define BIT_RST_MIU_STS                 BIT1
#define BIT_RST_MIE_STS                 BIT2
#define BIT_RST_MCU_STS                 BIT3
#define BIT_RST_STS_MASK                (BIT_RST_MIU_STS | BIT_RST_MIE_STS |BIT_RST_MCU_STS)
#define BIT_NC_DEB_SEL_SHIFT            12
#define BIT_NC_DEB_SEL_MASK             (BIT15|BIT14|BIT13|BIT12)

//------------------------------------------------------------------
/*
 * Power Save FIFO Cmd*
 */
#define PWR_BAT_CLASS    (0x1 << 13)  /* Battery lost class */
#define PWR_RST_CLASS    (0x1 << 12)  /* Reset Class */

/* Command Type */
#define PWR_CMD_WREG     (0x0 << 9)   /* Write data */
#define PWR_CMD_RDCP     (0x1 << 9)   /* Read and cmp data. If mismatch, HW retry */
#define PWR_CMD_WAIT     (0x2 << 9)   /* Wait idle, max. 128T */
#define PWR_CMD_WINT     (0x3 << 9)   /* Wait interrupt */
#define PWR_CMD_STOP     (0x7 << 9)   /* Stop */

/* RIU Bank */
#define PWR_CMD_BK0      (0x0 << 7)
#define PWR_CMD_BK1      (0x1 << 7)
#define PWR_CMD_BK2      (0x2 << 7)
#define PWR_CMD_BK3      (0x3 << 7)

#define PWR_RIU_ADDR     (0x0 << 0)







