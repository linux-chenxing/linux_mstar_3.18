#ifndef __UNFD_KIWI_LINUX_H__
#define __UNFD_KIWI_LINUX_H__

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/dma-mapping.h>
#include <linux/semaphore.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/list.h>

#include <asm/io.h>
#include <asm/ptrace.h>

#include "chip_int.h"
#include "chip_setup.h"


//=====================================================
//Operation Definition
//=====================================================
#ifdef REG
#undef REG
#endif
#define REG(Reg_Addr)                       (*(volatile U16*)((uintptr_t)(Reg_Addr)))
#define REG_OFFSET_SHIFT_BITS               2
#define GET_REG_ADDR(x, y)                  (x+((y)<<REG_OFFSET_SHIFT_BITS))

#define REG_WRITE_UINT16(reg_addr, val)     REG(reg_addr) = val
#define REG_READ_UINT16(reg_addr, val)      val = REG(reg_addr)
#define HAL_WRITE_UINT16(reg_addr, val)     (REG(reg_addr) = val)
#define HAL_READ_UINT16(reg_addr, val)      val = REG(reg_addr)
#define REG_SET_BITS_UINT16(reg_addr, val)  REG(reg_addr) |= (val)
#define REG_CLR_BITS_UINT16(reg_addr, val)  REG(reg_addr) &= ~(val)
#define REG_W1C_BITS_UINT16(reg_addr, val)  REG_WRITE_UINT16(reg_addr, REG(reg_addr)&(val))


//=====================================================
//PM banks Definition
//=====================================================

#define RIU_PM_BASE                         0xBF000000

#define REG_BANK_PM_SLEEP                   (0x700UL)
#define REG_BANK_TIMER0                     (0x1800UL)

#define PM_SLEEP_REG_BASE_ADDR              GET_REG_ADDR(RIU_PM_BASE, REG_BANK_PM_SLEEP)
#define TIMER0_REG_BASE_ADDR                GET_REG_ADDR(RIU_PM_BASE, REG_BANK_TIMER0)

//=====================================================
//Non-PM banks Definition
//=====================================================

#define RIU_BASE                            0xBF200000

#define REG_BANK_CLKGEN                     (0x580UL)
#define REG_BANK_CHIPTOP                    (0xF00UL)
#define REG_BANK_FCIE0                      (0x8980UL)
#define REG_BANK_FCIE2                      (0x8A00UL)
#define REG_BANK_FCIEPOWERSAVEMODE          (0x8A80UL)

#define MPLL_CLK_REG_BASE_ADDR              GET_REG_ADDR(RIU_BASE, REG_BANK_CLKGEN)
#define CHIPTOP_REG_BASE_ADDR               GET_REG_ADDR(RIU_BASE, REG_BANK_CHIPTOP)
#define FCIE_REG_BASE_ADDR                  GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE0)
#define FCIE_NC_CIFD_BASE                   GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE2)
#define FCIE_NC_WBUF_CIFD_BASE 	GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE2)	
#define FCIE_NC_RBUF_CIFD_BASE 	GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE2 + 0x20UL)	

#define FCIE_POWEER_SAVE_MODE_BASE          GET_REG_ADDR(RIU_BASE, REG_BANK_FCIEPOWERSAVEMODE)

//=====================================================
//FCIE IP Definition
//=====================================================

#define REG50_ECC_CTRL_INIT_VALUE           0

#define UNFD_ST_PLAT                        0x80000000
#define IF_IP_VERIFY                        0 // [CAUTION]: to verify IP and HAL code, defaut 0

#define NC_SEL_FCIE5			1
#if NC_SEL_FCIE5
#include "drvNAND_reg_v5.h"
#else
#error "Error! no FCIE registers selected."
#endif

//DDR & Timing Parameters

#define ENABLE_32BIT_MACRO			1
#define ENABLE_8BIT_MACRO				0

#define DDR_NAND_SUPPORT                          0

#define FCIE_LFSR                           1

#if (defined(DDR_NAND_SUPPORT) && DDR_NAND_SUPPORT)
#define NC_SET_DDR_MODE()                   REG_WRITE_UINT16(NC_DDR_CTRL, pNandDrv->u16_Reg58_DDRCtrl);
#define NC_CLR_DDR_MODE()                   REG_CLR_BITS_UINT16(NC_DDR_CTRL, BIT_DDR_MASM);
#else
#define NC_SET_DDR_MODE()
#define NC_CLR_DDR_MODE()
#endif

#if (defined(DDR_NAND_SUPPORT) && DDR_NAND_SUPPORT)

#if defined(ENABLE_8BIT_MACRO) && ENABLE_8BIT_MACRO
#define DQS_MODE_0P0T		0
#define DQS_MODE_0P5T		1
#define DQS_MODE_1P0T		2
#define DQS_MODE_1P5T		3
#define DQS_MODE_2P0T		4

#define DQS_MODE_TABLE_CNT	5
#define DQS_MODE_SEARCH_TABLE	{DQS_MODE_1P5T, DQS_MODE_2P0T, DQS_MODE_0P5T, DQS_MODE_1P0T, DQS_MODE_0P0T}

#elif defined(ENABLE_32BIT_MACRO) && ENABLE_32BIT_MACRO
#define DQS_MODE_TABLE_CNT		5
#define DQS_MODE_SEARCH_TABLE 	{0, 1, 2, 3, 4}

#define SKEW_CLK_PHASE_CNT		3

#endif

#define	NC_ONFI_DEFAULT_TRR                 12
#define	NC_ONFI_DEFAULT_TCS                 7
#define NC_ONFI_DEFAULT_TWW                 8
#define NC_ONFI_DEFAULT_TWHR                5
#define NC_ONFI_DEFAULT_TADL                6
#define NC_ONFI_DEFAULT_TCWAW               4
#define	NC_ONFI_DEFAULT_RX40CMD             4
#define	NC_ONFI_DEFAULT_RX40ADR             7
#define	NC_ONFI_DEFAULT_RX56                10

#define	NC_TOGGLE_DEFAULT_TRR               8
#define	NC_TOGGLE_DEFAULT_TCS               6
#define NC_TOGGLE_DEFAULT_TWW               7
#define	NC_TOGGLE_DEFAULT_TWHR              5
#define	NC_TOGGLE_DEFAULT_TADL              7
#define	NC_TOGGLE_DEFAULT_TCWAW             2
#define	NC_TOGGLE_DEFAULT_RX40CMD           4
#define	NC_TOGGLE_DEFAULT_RX40ADR           5
#define	NC_TOGGLE_DEFAULT_RX56              15
#endif

#define	NC_SDR_DEFAULT_TRR                  7
#define	NC_SDR_DEFAULT_TCS                  6
#define NC_SDR_DEFAULT_TWW                  5
#define	NC_SDR_DEFAULT_TWHR                 4
#define	NC_SDR_DEFAULT_TADL                 8
#define	NC_SDR_DEFAULT_TCWAW                2
#define	NC_SDR_DEFAULT_RX40CMD              4
#define	NC_SDR_DEFAULT_RX40ADR              5
#define	NC_SDR_DEFAULT_RX56                 5

#define	NC_INST_DELAY                       1
#define	NC_HWCMD_DELAY                      1
#define	NC_TRR_TCS                          1
#define	NC_TWHR_TCLHZ                       1
#define	NC_TCWAW_TADL                       1

//PAD & SHARE & MISC Definition

#define NAND_PAD_BYPASS_MODE                1
#define NAND_PAD_TOGGLE_MODE                2
#define NAND_PAD_ONFI_SYNC_MODE             3

#define MACRO_TYPE_8BIT			1
#define MACRO_TYPE_32BIT			2

#define IF_FCIE_SHARE_PINS                  0 // 1: need to nand_pads_switch at HAL's functions.
#define IF_FCIE_SHARE_CLK                   0 // 1: need to nand_clock_setting at HAL's functions.
#define IF_FCIE_SHARE_IP                    0 // A3 might use SD

#define ENABLE_NAND_INTERRUPT_MODE          1

#define ENABLE_NAND_POWER_SAVING_MODE       1
#define ENABLE_NAND_POWER_SAVING_DEGLITCH   1

#define NAND_DRIVER_ROM_VERSION             0 // to save code size
#define AUTO_FORMAT_FTL                     0

#define ENABLE_CUS_READ_ENHANCEMENT         0

#define __VER_UNFD_FTL__                    0

//FCIE5 Definition

#define SPARE_DMA_ADDR_AUTO_INC 		0 //spare would increase its address when dma

//=====================================================
// Nand Driver configs
//=====================================================
#define NAND_BUF_USE_STACK                  0
#define NAND_ENV_FPGA                       1
#define NAND_ENV_ASIC                       2
#ifdef __FPGA_MODE__
#define NAND_DRIVER_ENV                     NAND_ENV_FPGA
#else
#define NAND_DRIVER_ENV                     NAND_ENV_ASIC
#endif

#define UNFD_CACHE_LINE                     0x80
//=====================================================
// tool-chain attributes
//=====================================================
#define UNFD_PACK0
#define UNFD_PACK1                          __attribute__((__packed__))
#define UNFD_ALIGN0
#define UNFD_ALIGN1                         __attribute__((aligned(UNFD_CACHE_LINE)))

//=====================================================
// debug option
//=====================================================
#define NAND_TEST_IN_DESIGN                 0      /* [CAUTION] */

#ifndef NAND_DEBUG_MSG
#define NAND_DEBUG_MSG                      1
#endif

/* Define trace levels. */
#define UNFD_DEBUG_LEVEL_ERROR              (1)    /* Error condition debug messages. */
#define UNFD_DEBUG_LEVEL_WARNING            (2)    /* Warning condition debug messages. */
#define UNFD_DEBUG_LEVEL_HIGH               (3)    /* Debug messages (high debugging). */
#define UNFD_DEBUG_LEVEL_MEDIUM             (4)    /* Debug messages. */
#define UNFD_DEBUG_LEVEL_LOW                (5)    /* Debug messages (low debugging). */

/* Higer debug level means more verbose */
#ifndef UNFD_DEBUG_LEVEL
#define UNFD_DEBUG_LEVEL                    UNFD_DEBUG_LEVEL_WARNING
#endif

#if defined(NAND_DEBUG_MSG) && NAND_DEBUG_MSG
#define nand_print_tmt
#define nand_printf                         printk
#define nand_debug(dbg_lv, tag, str, ...)           \
    do {                                            \
        if (dbg_lv > UNFD_DEBUG_LEVEL)              \
            break;                                  \
        else {                                      \
            if (tag)                                \
            {                                       \
                nand_printf(KERN_ERR"[%s]\t",__func__);     \
            }                                       \
                                                    \
            nand_printf(KERN_ERR str, ##__VA_ARGS__);        \
        }                                           \
    } while(0)
#else /* NAND_DEBUG_MSG */
#define nand_printf(...)
#define nand_debug(enable, tag, str, ...)   {}
#endif /* NAND_DEBUG_MSG */

static __inline void nand_assert(int condition)
{
    if (!condition) {
        panic("%s\n UNFD Assert(%d)\n", __func__, __LINE__);
    }
}

#define nand_die()      \
    do {                \
        nand_assert(0); \
    } while(0);

#define nand_stop() \
    while(1)  nand_reset_WatchDog();

//=====================================================
// HW Timer for Delay
//=====================================================
#define TIMER0_ENABLE                       GET_REG_ADDR(TIMER0_REG_BASE_ADDR, 0x20)
#define TIMER0_HIT                          GET_REG_ADDR(TIMER0_REG_BASE_ADDR, 0x21)
#define TIMER0_MAX_LOW                      GET_REG_ADDR(TIMER0_REG_BASE_ADDR, 0x22)
#define TIMER0_MAX_HIGH                     GET_REG_ADDR(TIMER0_REG_BASE_ADDR, 0x23)
#define TIMER0_CAP_LOW                      GET_REG_ADDR(TIMER0_REG_BASE_ADDR, 0x24)
#define TIMER0_CAP_HIGH                     GET_REG_ADDR(TIMER0_REG_BASE_ADDR, 0x25)

#define HW_TIMER_DELAY_1us                  1
#define HW_TIMER_DELAY_10us                 10
#define HW_TIMER_DELAY_100us                100
#define HW_TIMER_DELAY_1ms                  (1000 * HW_TIMER_DELAY_1us)
#define HW_TIMER_DELAY_5ms                  (5    * HW_TIMER_DELAY_1ms)
#define HW_TIMER_DELAY_10ms                 (10   * HW_TIMER_DELAY_1ms)
#define HW_TIMER_DELAY_100ms                (100  * HW_TIMER_DELAY_1ms)
#define HW_TIMER_DELAY_500ms                (500  * HW_TIMER_DELAY_1ms)
#define HW_TIMER_DELAY_1s                   (1000 * HW_TIMER_DELAY_1ms)

extern void delay_us(unsigned us);
extern U32  nand_hw_timer_delay(U32 u32usTick);

//=====================================================
// Pads Switch
//=====================================================
#define reg_allpad_in           			GET_REG_ADDR(CHIPTOP_REG_BASE_ADDR, 0x50)

#define reg_sync_pe						    GET_REG_ADDR(CHIPTOP_REG_BASE_ADDR, 0x24)
#define reg_s_gpio_pe						GET_REG_ADDR(CHIPTOP_REG_BASE_ADDR, 0x25)

extern U32 nand_pads_switch(U32 u32EnableFCIE);
extern U32 nand_check_DDR_pad(void);
//--------------------------------power saving mode----------------------------
#define reg_pwrgd_int_glirm             GET_REG_ADDR(PM_SLEEP_REG_BASE_ADDR, 0x61)
#define BIT_PWRGD_INT_GLIRM_EN          BIT9
#define BIT_PWEGD_INT_GLIRM_MASK        (BIT15|BIT14|BIT13|BIT12|BIT11|BIT10)

//=====================================================
// set FCIE clock
//=====================================================
#define DECIDE_CLOCK_BY_NAND			1

#define NFIE_CLK_MASK                       (BIT5|BIT4|BIT3|BIT2)
#define NFIE_CLK_5_4M                       (2<<2)
#define NFIE_CLK_13M                        (3<<2)
#define NFIE_CLK_18M                        (4<<2)
#define NFIE_CLK_22_7M                      (5<<2)
#define NFIE_CLK_27M                        (6<<2)
#define NFIE_CLK_32M                        (7<<2)
#define NFIE_CLK_36M                        (0<<2)
#define NFIE_CLK_40M                        (1<<2)
#define NFIE_CLK_43_2M                      (8<<2)
#define NFIE_CLK_54M                        (9<<2)
#define NFIE_CLK_72M                        (10<<2)
#define NFIE_CLK_86M                        (11<<2)

#define NFIE_CLK_TABLE_CNT	12
#define NFIE_CLK_TABLE	{	 NFIE_CLK_5_4M, NFIE_CLK_13M, NFIE_CLK_18M, \
							NFIE_CLK_22_7M, NFIE_CLK_27M, NFIE_CLK_32M, NFIE_CLK_36M, \
							NFIE_CLK_40M, NFIE_CLK_43_2M, NFIE_CLK_54M, NFIE_CLK_72M, \
							NFIE_CLK_86M}

#define NFIE_CLK_TABLE_STR	{	"5.4M", "13M", "18M",\
							"22.7M", "27M", "32M", "36M", \
							"40M", "43.2M", "54M", "72M", \
							"86M"}

#define NFIE_5_4M_VALUE     5400000
#define NFIE_13M_VALUE      13000000
#define NFIE_18M_VALUE      18000000
#define NFIE_22_7M_VALUE    22700000
#define NFIE_27M_VALUE      27000000
#define NFIE_32M_VALUE      32000000
#define NFIE_36M_VALUE      26000000
#define NFIE_40M_VALUE      40000000
#define NFIE_43_2M_VALUE    43200000
#define NFIE_54M_VALUE      54000000
#define NFIE_72M_VALUE      72000000
#define NFIE_86M_VALUE      86000000


#define NFIE_CLK_VALUE_TABLE	{	NFIE_5_4M_VALUE, NFIE_13M_VALUE, NFIE_18M_VALUE, NFIE_22_7M_VALUE, \
							NFIE_27M_VALUE, NFIE_32M_VALUE, NFIE_36M_VALUE, NFIE_40M_VALUE, \
							NFIE_43_2M_VALUE, NFIE_54M_VALUE, NFIE_72M_VALUE, NFIE_86M_VALUE}


/*Define 1 cycle Time for each clock note: define value must be the (real value -1)*/
#define NFIE_1T_5_4M        185
#define NFIE_1T_13M         76
#define NFIE_1T_18M         55
#define NFIE_1T_22_7M       44
#define NFIE_1T_27M         37
#define NFIE_1T_32M         31
#define NFIE_1T_36M         27
#define NFIE_1T_40M         25
#define NFIE_1T_43_2M       23
#define NFIE_1T_54M         18
#define NFIE_1T_72M         13
#define NFIE_1T_86M         11


#define NFIE_1T_TABLE		{	NFIE_1T_5_4M, NFIE_1T_13M, NFIE_1T_18M, \
							NFIE_1T_22_7M, NFIE_1T_27M, NFIE_1T_32M, NFIE_1T_36M, \
							NFIE_1T_40M, NFIE_1T_43_2M, NFIE_1T_54M, NFIE_1T_72M, \
							NFIE_1T_86M}

#define DUTY_CYCLE_PATCH                    0 // 1: to enlarge low width for tREA's worst case of 25ns
#if DUTY_CYCLE_PATCH
#define FCIE3_SW_DEFAULT_CLK                NFIE_1T_86M
#define FCIE_REG41_VAL                      ((2<<9)|(2<<3)) // RE,WR pulse, Low:High=3:1
#define REG57_ECO_FIX_INIT_VALUE            0
#else
#define FCIE3_SW_DEFAULT_CLK                NFIE_CLK_54M
#define FCIE_REG41_VAL                      0               // RE,WR pulse, Low:High=1:1
#define REG57_ECO_FIX_INIT_VALUE            BIT_NC_LATCH_DATA_1_0_T	// delay 1.0T
#endif
#define FCIE3_SW_SLOWEST_CLK                NFIE_1T_13M

#define NAND_SEQ_ACC_TIME_TOL               16 //in unit of ns

#define reg_ckg_fcie				GET_REG_ADDR(MPLL_CLK_REG_BASE_ADDR, 0x64)
#define BIT_CLK_SELECT              BIT6

extern U32  nand_clock_setting(U32 u32ClkParam);

extern void nand_DumpPadClk(void);

extern U32 NC_CheckStorageType(void);

extern U32 NC_CheckBlankPage(U8 * pu8_Buffer);

extern void nand_flush_cache_post_read(uintptr_t u32_DMAAddr, U32 u32_ByteCnt);
//=====================================================
// transfer DMA Address
//=====================================================
#define MIU_BUS_WIDTH_BITS                  3 // Need to confirm

#define MIU_CHECK_LAST_DONE                 1

//=====================================================
// misc
//=====================================================
//#define BIG_ENDIAN
#define LITTLE_ENDIAN

typedef struct NAND_DRIVER_PLATFORM_DEPENDENT
{
    U8 *pu8_PageSpareBuf;
    U8 *pu8_PageDataBuf;
    U32	u32_DMAAddrOffset;
    U32	u32_RAMBufferOffset;
    U32	u32_RAMBufferLen;

}NAND_DRIVER_PLAT_CTX, *P_NAND_DRIVER_PLAT;

#define NANDINFO_ECC_TYPE                   ECC_TYPE_40BIT1KB
#define malloc(x)                           kmalloc(x, GFP_KERNEL)
#define free                                kfree

#endif
