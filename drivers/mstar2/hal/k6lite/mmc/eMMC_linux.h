#ifndef __eMMC_K6_LINUX__
#define __eMMC_K6_LINUX__

#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/dma-mapping.h>
#include <linux/mmc/host.h>
#include <linux/scatterlist.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/kthread.h>
#include <mstar/mstar_chip.h>
#include <mach/io.h>
#include <linux/random.h>
#include "chip_int.h"
#include "chip_setup.h"

extern ptrdiff_t mstar_pm_base;

//=====================================================
// tool-chain attributes
//=====================================================
//[FIXME] -->
#define eMMC_CACHE_LINE                 0x40 // [FIXME]

#define eMMC_PACK0
#define eMMC_PACK1                      __attribute__((__packed__))
#define eMMC_ALIGN0
#define eMMC_ALIGN1                     __attribute__((aligned(eMMC_CACHE_LINE)))
// <-- [FIXME]

//=====================================================
// HW registers
//=====================================================
#define REG_OFFSET_SHIFT_BITS           2

#define REG_FCIE_U16(Reg_Addr)          (*(volatile U16*)((uintptr_t)Reg_Addr))
#define GET_REG_ADDR(x, y)              ((x)+((y) << REG_OFFSET_SHIFT_BITS))

#define REG_FCIE(reg_addr)              REG_FCIE_U16(reg_addr)
#define REG_FCIE_W(reg_addr, val)       REG_FCIE(reg_addr) = (val)
#define REG_FCIE_R(reg_addr, val)       val = REG_FCIE(reg_addr)
#define REG_FCIE_SETBIT(reg_addr, val)  REG_FCIE(reg_addr) |= (val)
#define REG_FCIE_CLRBIT(reg_addr, val)  REG_FCIE(reg_addr) &= ~(val)
#define REG_FCIE_W1C(reg_addr, val)     REG_FCIE_W(reg_addr, REG_FCIE(reg_addr)&(val))

//------------------------------
#if defined(CONFIG_ARM)
#define RIU_PM_BASE                     (IO_ADDRESS(0x1F000000UL))
#define RIU_BASE                        (IO_ADDRESS(0x1F200000UL))
#elif defined(CONFIG_ARM64)
#define RIU_PM_BASE                     ((uintptr_t)(mstar_pm_base))
#define RIU_BASE                        ((uintptr_t)(mstar_pm_base+0x200000))
#endif

#define REG_BANK_FCIE0                  0x8980
#define REG_BANK_FCIE1                  0x8A00
#define REG_BANK_FCIE2                  0x8A80

#define FCIE0_BASE                      GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE0)
#define FCIE1_BASE                      GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE1)
#define FCIE2_BASE                      GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE2)

#define FCIE_REG_BASE_ADDR              GET_REG_ADDR(FCIE0_BASE, 0x00)
#define FCIE_CMDFIFO_BASE_ADDR          GET_REG_ADDR(FCIE0_BASE, 0x20)
#define FCIE_CIFD_BASE_ADDR             GET_REG_ADDR(FCIE1_BASE, 0x00)
#define FCIE_POWEER_SAVE_MODE_BASE      GET_REG_ADDR(FCIE2_BASE, 0x00)

#define FCIE_NC_WBUF_CIFD_BASE          GET_REG_ADDR(FCIE1_BASE, 0x00)
#define FCIE_NC_RBUF_CIFD_BASE          GET_REG_ADDR(FCIE1_BASE, 0x20)

#include "eMMC_reg_v5.h"

//--------------------------------clock gen------------------------------------
#define REG_BANK_CLKGEN0                0x0580	// (0x100B - 0x1000) x 80h
#define CLKGEN0_BASE                    GET_REG_ADDR(RIU_BASE, REG_BANK_CLKGEN0)

#define reg_ckg_fcie                    GET_REG_ADDR(CLKGEN0_BASE, 0x64)
#define BIT_FCIE_CLK_GATING             BIT8
#define BIT_FCIE_CLK_INVERSE            BIT9
#define BIT_CLKGEN_FCIE_MASK            (BIT13|BIT12|BIT11|BIT10)
#define BIT_FCIE_CLK_SRC_SEL            BIT14
#define BIT_CKG_NFIE_DVI4_EN            BIT7


// 1X clocks
#define BIT_FCIE_CLK_1X_XTAL            0x0
#define BIT_FCIE_CLK_1X_12M             0x1
#define BIT_FCIE_CLK_1X_15_5M           0x2
#define BIT_FCIE_CLK_1X_18M             0x3
#define BIT_FCIE_CLK_1X_21_5M           0x4
#define BIT_FCIE_CLK_1X_27M             0x5
#define BIT_FCIE_CLK_1X_36M             0x6
#define BIT_FCIE_CLK_1X_40M             0x7
#define BIT_FCIE_CLK_1X_48M             0x8
#define BIT_FCIE_CLK_1X_54M             0x9
#define BIT_FCIE_CLK_1X_60M             0xA
#define BIT_FCIE_CLK_1X_88_5M           0xB
#define BIT_FCIE_CLK_1X_75K             0xC
#define BIT_FCIE_CLK_1X_80M             0xD

// 4X clocks
#define eMMC_4X_FLAG                    0x80

#define BIT_FCIE_CLK_4X_XTAL            (0x0|eMMC_4X_FLAG)
#define BIT_FCIE_CLK_4X_48M             (0x1|eMMC_4X_FLAG)
#define BIT_FCIE_CLK_4X_62M             (0x2|eMMC_4X_FLAG)
#define BIT_FCIE_CLK_4X_72M             (0x3|eMMC_4X_FLAG)
#define BIT_FCIE_CLK_4X_86M             (0x4|eMMC_4X_FLAG)
#define BIT_FCIE_CLK_4X_108M            (0x5|eMMC_4X_FLAG)
#define BIT_FCIE_CLK_4X_144M            (0x6|eMMC_4X_FLAG)
#define BIT_FCIE_CLK_4X_160M            (0x7|eMMC_4X_FLAG)
#define BIT_FCIE_CLK_4X_192M            (0x8|eMMC_4X_FLAG)
#define BIT_FCIE_CLK_4X_216M            (0x9|eMMC_4X_FLAG)
#define BIT_FCIE_CLK_4X_240M            (0xA|eMMC_4X_FLAG)
#define BIT_FCIE_CLK_4X_354M            (0xB|eMMC_4X_FLAG)
#define BIT_FCIE_CLK_4X_300K            (0xC|eMMC_4X_FLAG)
#define BIT_FCIE_CLK_4X_320M            (0xD|eMMC_4X_FLAG)

#define BIT_FCIE_CLK_12M                BIT_FCIE_CLK_1X_XTAL
#define BIT_FCIE_CLK_48M                BIT_FCIE_CLK_4X_48M
#define BIT_FCIE_CLK_300K               BIT_FCIE_CLK_4X_300K

#define eMMC_PLL_CLK_200M               BIT_FCIE_CLK_4X_192M
#define eMMC_PLL_CLK_160M               BIT_FCIE_CLK_4X_160M
#define eMMC_PLL_CLK_140M               BIT_FCIE_CLK_4X_144M
#define eMMC_PLL_CLK_120M               BIT_FCIE_CLK_4X_108M

#define eMMC_FCIE_VALID_CLK_CNT         3   // FIXME

#define PLL_SKEW4_CNT                   9
#define MIN_OK_SKEW_CNT                 7

extern  U8 gau8_FCIEClkSel[];
extern  U8 gau8_eMMCPLLSel_52[];
extern  U8 gau8_eMMCPLLSel_200[];           // for DDR52 or HS200

typedef eMMC_PACK0 struct _eMMC_FCIE_ATOP_SET {
    U32 u32_ScanResult;
    U8  u8_Clk;
    U8  u8_Reg2Ch, u8_Skew4;
    U8  u8_Cell;
    U8  u8_Skew2, u8_CellCnt;
} eMMC_PACK1 eMMC_FCIE_ATOP_SET_t;

typedef eMMC_PACK0 struct _eMMC_FCIE_ATOP_SET_EXT {
    U32 au32_RXDLLResult[5];
    U8 u8_Skew4Idx;
    U8 au8_Reg2Ch[5], au8_Skew4[5];
    U8 au8_Cell[5], au8_CellCnt[5];
} eMMC_PACK1 eMMC_FCIE_ATOP_SET_EXT_t;


//--------------------------------chiptop--------------------------------------
#define REG_BANK_CHIPTOP                0x0F00	// (0x101E - 0x1000) x 80h
#define PAD_CHIPTOP_BASE                GET_REG_ADDR(RIU_BASE, REG_BANK_CHIPTOP)

#define reg_chiptop_0x08				GET_REG_ADDR(PAD_CHIPTOP_BASE, 0x08)
#define BIT_SD_CONFIG                   BIT8
#define BIT_NAND_MODE                   BIT3
#define BIT_EMMC_CONFIG                 BIT0

#define reg_nand_pad_driving            GET_REG_ADDR(PAD_CHIPTOP_BASE, 0x30)

#define reg_chiptop_0x36                GET_REG_ADDR(PAD_CHIPTOP_BASE, 0x36)
#define BIT_reg_emmc_rstz_sw            BIT5
#define BIT_reg_emmc_rstz_en            BIT4

#define reg_chiptop_0x50                GET_REG_ADDR(PAD_CHIPTOP_BASE, 0x50)
#define BIT_ALL_PAD_IN                  BIT15

//--------------------------------emmc pll--------------------------------------
#define REG_BANK_EMMC_PLL               0x11F80	// (0x123F - 0x1000) x 80h
#define EMMC_PLL_BASE                   GET_REG_ADDR(RIU_BASE, REG_BANK_EMMC_PLL)

#define reg_emmcpll_0x1a                GET_REG_ADDR(EMMC_PLL_BASE, 0x1A)
#define reg_emmc_test                   reg_emmcpll_0x1a

#define reg_emmcpll_0x1c                GET_REG_ADDR(EMMC_PLL_BASE, 0x1c)

#define reg_emmcpll_0x68                GET_REG_ADDR(EMMC_PLL_BASE, 0x68)
#define reg_emmcpll_0x6a                GET_REG_ADDR(EMMC_PLL_BASE, 0x6a)
#define reg_emmcpll_0x6b                GET_REG_ADDR(EMMC_PLL_BASE, 0x6b)
#define reg_emmcpll_0x6c                GET_REG_ADDR(EMMC_PLL_BASE, 0x6c)
#define reg_emmcpll_0x6d                GET_REG_ADDR(EMMC_PLL_BASE, 0x6d)
#define reg_emmcpll_0x6f                GET_REG_ADDR(EMMC_PLL_BASE, 0x6f)

#define BIT_DQS_DELAY_CELL_MASK         (BIT4|BIT5|BIT6|BIT7)
#define BIT_DQS_DELAY_CELL_SHIFT        4
#define BIT_DQS_MODE_MASK               (BIT0|BIT1|BIT2)
#define BIT_DQS_MODE_SHIFT              0
#define BIT_DQS_MODE_0T                 (0 << BIT_DQS_MODE_SHIFT)
#define BIT_DQS_MODE_0_5T               (1 << BIT_DQS_MODE_SHIFT)
#define BIT_DQS_MODE_1T                 (2 << BIT_DQS_MODE_SHIFT)
#define BIT_DQS_MODE_1_5T               (3 << BIT_DQS_MODE_SHIFT)
#define BIT_DQS_MODE_2T                 (4 << BIT_DQS_MODE_SHIFT)
#define BIT_DQS_MODE_2_5T               (5 << BIT_DQS_MODE_SHIFT)

//---------------------------------pad top-------------------------------------
#define REG_BANK_PADTOP                 0x1280	// (0x1025 - 0x1000) x 80h
#define PADTOP_BASE                     GET_REG_ADDR(RIU_BASE, REG_BANK_PADTOP)

#define reg_nand_gpio                   GET_REG_ADDR(PADTOP_BASE, 0x44)
#define BIT_NAND_GPIO_OEN_12            BIT5
#define BIT_NAND_GPIO_OUT_12            BIT4

// ----------------------------------------------
#define eMMC_RST_L()                    {REG_FCIE_SETBIT(reg_chiptop_0x36, BIT_reg_emmc_rstz_en);\
                                         REG_FCIE_CLRBIT(reg_chiptop_0x36, BIT_reg_emmc_rstz_sw);\
                                         REG_FCIE_SETBIT(FCIE_BOOT_CONFIG, BIT_EMMC_RSTZ_EN);\
                                         REG_FCIE_CLRBIT(FCIE_BOOT_CONFIG, BIT_EMMC_RSTZ);\
                                         REG_FCIE_CLRBIT(reg_nand_gpio, BIT_NAND_GPIO_OEN_12);\
                                         REG_FCIE_CLRBIT(reg_nand_gpio, BIT_NAND_GPIO_OUT_12);}

#define eMMC_RST_H()                    {REG_FCIE_SETBIT(reg_chiptop_0x36, BIT_reg_emmc_rstz_en);\
                                         REG_FCIE_SETBIT(reg_chiptop_0x36, BIT_reg_emmc_rstz_sw);\
                                         REG_FCIE_SETBIT(FCIE_BOOT_CONFIG, BIT_EMMC_RSTZ_EN);\
                                         REG_FCIE_SETBIT(FCIE_BOOT_CONFIG, BIT_EMMC_RSTZ);\
                                         REG_FCIE_CLRBIT(reg_nand_gpio, BIT_NAND_GPIO_OEN_12);\
                                         REG_FCIE_SETBIT(reg_nand_gpio, BIT_NAND_GPIO_OUT_12);}


//--------------------------------power saving mode----------------------------
#define REG_BANK_PM_SLEEP               (0x700)
#define PM_SLEEP_REG_BASE_ADDR          GET_REG_ADDR(RIU_PM_BASE, REG_BANK_PM_SLEEP)
#define reg_pwrgd_int_glirm             GET_REG_ADDR(PM_SLEEP_REG_BASE_ADDR, 0x61)
#define BIT_PWRGD_INT_GLIRM_EN          BIT9
#define BIT_PWEGD_INT_GLIRM_MASK        (BIT15|BIT14|BIT13|BIT12|BIT11|BIT10)

#define REG_PM_TOP                      (0xF00)
#define PM_TOP_REG_BASE_ADDR            GET_REG_ADDR(RIU_PM_BASE, REG_PM_TOP)
#define reg_chip_id                     GET_REG_ADDR(PM_TOP_REG_BASE_ADDR, 0x00)
#define reg_chip_version                GET_REG_ADDR(PM_TOP_REG_BASE_ADDR, 0x01)

//=====================================================
// API declarations
//=====================================================
extern  U32 eMMC_hw_timer_delay(U32 u32us);
extern  U32 eMMC_hw_timer_sleep(U32 u32ms);

#define eMMC_HW_TIMER_HZ                1000000
#define FCIE_eMMC_DISABLE               0
#define FCIE_eMMC_BYPASS                1
#define FCIE_eMMC_SDR                   2
#define FCIE_eMMC_DDR_8BIT_MACRO        3
#define FCIE_eMMC_DDR                   FCIE_eMMC_DDR_8BIT_MACRO
#define FCIE_eMMC_HS200                 5
#define FCIE_eMMC_HS400                 6

extern  U32 eMMC_pads_switch(U32 u32_FCIE_IF_Type);
extern  U32 eMMC_clock_setting(U16 u16_ClkParam);
extern  U32 eMMC_clock_gating(void);
extern void eMMC_set_WatchDog(U8 u8_IfEnable);
extern void eMMC_reset_WatchDog(void);
extern  U32 eMMC_translate_DMA_address_Ex(dma_addr_t dma_DMAAddr, U32 u32_ByteCnt);
extern dma_addr_t eMMC_DMA_MAP_address(uintptr_t ulongBuffer, U32 u32_ByteCnt, int mode);
extern void eMMC_DMA_UNMAP_address(dma_addr_t dma_DMAAddr, U32 u32_ByteCnt, int mode);
extern void eMMC_flush_data_cache_buffer(uintptr_t ulongDMAAddr, U32 u32_ByteCnt);
extern void eMMC_Invalidate_data_cache_buffer(uintptr_t ulongDMAAddr, U32 u32_ByteCnt);
extern void eMMC_flush_miu_pipe(void);
extern  U32 eMMC_PlatformResetPre(void);
extern  U32 eMMC_PlatformResetPost(void);
extern  U32 eMMC_PlatformInit(void);
extern  U32 eMMC_PlatformDeinit(void);
extern  U32 eMMC_CheckIfMemCorrupt(void);
extern void eMMC_DumpPadClk(void);

#define eMMC_BOOT_PART_W                BIT0
#define eMMC_BOOT_PART_R                BIT1

extern U32 eMMC_BootPartitionHandler_WR(U8 *pDataBuf, U16 u16_PartType, U32 u32_StartSector, U32 u32_SectorCnt, U8 u8_OP);
extern U32 eMMC_BootPartitionHandler_E(U16 u16_PartType);
extern U32 eMMC_hw_timer_start(void);
extern U32 eMMC_hw_timer_tick(void);
extern irqreturn_t eMMC_FCIE_IRQ(int irq, void *dummy); // [FIXME]
extern U32 eMMC_WaitCompleteIntr(uintptr_t ulongRegAddr, U16 u16_WaitEvent, U32 u32_MicroSec);
extern struct mutex FCIE3_mutex;
extern void eMMC_LockFCIE(U8 *pu8_str);
extern void eMMC_UnlockFCIE(U8 *pu8_str);
extern int  mstar_mci_Housekeep(void *pData);
extern U32  mstar_SD_CardChange(void);

//=====================================================
// partitions config
//=====================================================
// every blk is 512 bytes (reserve 2MB-64KB for internal use)
#define eMMC_DRV_RESERVED_BLK_CNT       ((0x200000-0x10000)/0x200)

#define eMMC_CIS_NNI_BLK_CNT            2
#define eMMC_CIS_PNI_BLK_CNT            2
#define eMMC_TEST_BLK_CNT               (0x100000/0x200) // 1MB

#define eMMC_CIS_BLK_0                  (64*1024/512) // from 64KB
#define eMMC_NNI_BLK_0                  (eMMC_CIS_BLK_0+0)
#define eMMC_NNI_BLK_1                  (eMMC_CIS_BLK_0+1)
#define eMMC_PNI_BLK_0                  (eMMC_CIS_BLK_0+2)
#define eMMC_PNI_BLK_1                  (eMMC_CIS_BLK_0+3)
#define eMMC_DDRTABLE_BLK_0             (eMMC_CIS_BLK_0+4)
#define eMMC_DDRTABLE_BLK_1             (eMMC_CIS_BLK_0+5)
#define eMMC_HS200TABLE_BLK_0           (eMMC_CIS_BLK_0+6)
#define eMMC_HS200TABLE_BLK_1           (eMMC_CIS_BLK_0+7)
#define eMMC_HS400TABLE_BLK_0           (eMMC_CIS_BLK_0+8)
#define eMMC_HS400TABLE_BLK_1           (eMMC_CIS_BLK_0+9)
#define eMMC_HS400EXTTABLE_BLK_0        (eMMC_CIS_BLK_0+10)
#define eMMC_HS400EXTTABLE_BLK_1        (eMMC_CIS_BLK_0+11)
#define eMMC_ALLRSP_BLK_0               (eMMC_CIS_BLK_0+12)
#define eMMC_ALLRSP_BLK_1               (eMMC_CIS_BLK_0+13)
#define eMMC_BURST_LEN_BLK_0            (eMMC_CIS_BLK_0+14)

#define eMMC_CIS_BLK_END                eMMC_BURST_LEN_BLK_0
// last 1MB in reserved area, use for eMMC test
#define eMMC_TEST_BLK_0                 (eMMC_CIS_BLK_END+1)


//=====================================================
// Driver configs
//=====================================================
#define DRIVER_NAME                     "mstar_mci"
#define eMMC_UPDATE_FIRMWARE            0

#define eMMC_ST_PLAT                    0x80000000
// [CAUTION]: to verify IP and HAL code, defaut 0
#define IF_IP_VERIFY                    0 // [FIXME] -->

// need to eMMC_pads_switch
#define IF_FCIE_SHARE_PINS              0

// need to eMMC_clock_setting
#define IF_FCIE_SHARE_CLK               0

#define IF_FCIE_SHARE_IP                1
#if defined(CONFIG_MSTAR_FCIE_HOST) && CONFIG_MSTAR_FCIE_HOST
#define CONFIG_MSTAR_SDMMC 1
#endif
#if !(defined(CONFIG_MSTAR_SDMMC)&&CONFIG_MSTAR_SDMMC)
#undef IF_FCIE_SHARE_IP
#define IF_FCIE_SHARE_IP                0
#endif

//------------------------------
#define FICE_BYTE_MODE_ENABLE           1 // always 1
#define ENABLE_eMMC_INTERRUPT_MODE      1
#define ENABLE_eMMC_RIU_MODE            0 // for debug cache issue
#if ENABLE_eMMC_RIU_MODE
#undef ENABLE_eMMC_INTERRUPT_MODE
#define ENABLE_eMMC_INTERRUPT_MODE      0
#endif

#define ENABLE_EMMC_POWER_SAVING_MODE   1
#define ENABLE_FCIE_HW_BUSY_CHECK       1

#define ENABLE_EMMC_ASYNC_IO            1
#define ENABLE_EMMC_PRE_DEFINED_BLK     1
#define ENABLE_FCIE_ADMA                1

#define eMMC_EMULATE_WR_FAIL            0
//------------------------------
// DDR48, DDR52, HS200, HS400
#define ENABLE_eMMC_ATOP                1 // DDR52 (ATOP)
#define ENABLE_eMMC_AFIFO               0
#define ENABLE_eMMC_HS200               0 // HS200
#define ENABLE_eMMC_HS400               0 // HS400

#if ENABLE_eMMC_RIU_MODE || ENABLE_eMMC_ATOP
#undef IF_DETECT_eMMC_DDR_TIMING
#define IF_DETECT_eMMC_DDR_TIMING       0
#endif

#define eMMC_IF_TUNING_TTABLE()         (g_eMMCDrv.u32_DrvFlag&DRV_FLAG_TUNING_TTABLE)

//------------------------------
#define eMMC_FEATURE_RELIABLE_WRITE     1
#if eMMC_UPDATE_FIRMWARE
#undef  eMMC_FEATURE_RELIABLE_WRITE
#define eMMC_FEATURE_RELIABLE_WRITE     0
#endif

//------------------------------
#define eMMC_RSP_FROM_RAM               0
#define eMMC_BURST_LEN_AUTOCFG          0

#define eMMC_HOUSEKEEP_THREAD           0
#define eMMC_PROFILE_WR                 0
#define eMMC_SCAN_HS200                 0 // for Irwin

//------------------------------
#define eMMC_SECTOR_BUF_BYTECTN         eMMC_SECTOR_BUF_16KB
extern U8 gau8_eMMC_SectorBuf[];
extern U8 gau8_eMMC_PartInfoBuf[];


//=====================================================
// debug option
//=====================================================
#define eMMC_TEST_IN_DESIGN             0 // [FIXME]: set 1 to verify HW timer

#ifndef eMMC_DEBUG_MSG
#define eMMC_DEBUG_MSG                  1
#endif

/* Define trace levels. */
#define eMMC_DEBUG_LEVEL_ERROR          (1)    /* Error condition debug messages. */
#define eMMC_DEBUG_LEVEL_WARNING        (2)    /* Warning condition debug messages. */
#define eMMC_DEBUG_LEVEL_HIGH           (3)    /* Debug messages (high debugging). */
#define eMMC_DEBUG_LEVEL_MEDIUM         (4)    /* Debug messages. */
#define eMMC_DEBUG_LEVEL_LOW            (5)    /* Debug messages (low debugging). */

/* Higer debug level means more verbose */
#ifndef eMMC_DEBUG_LEVEL
#define eMMC_DEBUG_LEVEL                eMMC_DEBUG_LEVEL_WARNING
#endif

#if defined(eMMC_DEBUG_MSG) && eMMC_DEBUG_MSG
#if eMMC_HOUSEKEEP_THREAD && eMMC_SCAN_HS200
#define eMMC_printf(fmt, arg...)        printk(fmt, ##arg)
#else
#define eMMC_printf(fmt, arg...)        printk(fmt, ##arg)
#endif

#define eMMC_debug(dbg_lv, tag, str, ...)						\
	do {										\
		if (dbg_lv > eMMC_DEBUG_LEVEL)						\
			break;								\
		else {									\
			if (tag)							\
				eMMC_printf("[ %s() Ln.%u ] ", __FUNCTION__, __LINE__);	\
											\
			eMMC_printf(str, ##__VA_ARGS__);				\
		}									\
	} while(0)
#else
#define eMMC_printf(...)
#define eMMC_debug(enable, tag, str, ...)	do{}while(0)
#endif

#define eMMC_die(str) {eMMC_printf("eMMC Die: %s() Ln.%u, %s \n", __FUNCTION__, __LINE__, str); \
	                   panic("\n");}

#define eMMC_stop() \
	while(1)  eMMC_reset_WatchDog();

//=====================================================
// unit for HW Timer delay (unit of us)
//=====================================================
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
#define FCIE_SLOWEST_CLK                BIT_FCIE_CLK_4X_300K
#define FCIE_SLOW_CLK                   BIT_FCIE_CLK_4X_XTAL
#define FCIE_DEFAULT_CLK                BIT_FCIE_CLK_4X_192M

#define eMMC_PLL_CLK_SLOW               BIT_FCIE_CLK_4X_XTAL
#define eMMC_PLL_CLK_FAST               BIT_FCIE_CLK_4X_192M

#define eMMC_FCIE_CLK_DIS()             {REG_FCIE_CLRBIT(FCIE_SD_MODE, BIT_SD_CLK_EN);\
                                         REG_FCIE_CLRBIT(FCIE_DDR_MODE, BIT_8BIT_MACRO_EN);}

// <-- [FIXME]
//=====================================================
// transfer DMA Address
//=====================================================
#define MIU_BUS_WIDTH_BITS              4 // Need to confirm
/*
 * Important:
 * The following buffers should be large enough for a whole eMMC block
 */
// FIXME, this is only for verifing IP
#define DMA_W_ADDR                      0x40C00000
#define DMA_R_ADDR                      0x40D00000
#define DMA_W_SPARE_ADDR                0x40E00000
#define DMA_R_SPARE_ADDR                0x40E80000
#define DMA_BAD_BLK_BUF                 0x40F00000

//=====================================================
// misc
//=====================================================
//#define BIG_ENDIAN
#define LITTLE_ENDIAN

#endif /* __eMMC_K6_LINUX__ */
