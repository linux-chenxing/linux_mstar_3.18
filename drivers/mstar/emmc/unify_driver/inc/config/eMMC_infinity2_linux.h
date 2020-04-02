#ifndef __eMMC_INFINITY2_LINUX__
#define __eMMC_INFINITY2_LINUX__

#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/dma-mapping.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/blkdev.h>
#include <linux/err.h>
#include <linux/mmc/host.h>
#include <linux/scatterlist.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/kthread.h>

#include "ms_platform.h"
#include "ms_types.h"

#ifndef U32
#define U32  unsigned int
#endif
#ifndef U16
#define U16  unsigned short
#endif
#ifndef U8
#define U8   unsigned char
#endif
#ifndef S32
#define S32  signed long
#endif
#ifndef S16
#define S16  signed short
#endif
#ifndef S8
#define S8   signed char
#endif


//=====================================================
// HW registers
//=====================================================

#define REG_FCIE_U16(Reg_Addr)          (*(volatile U16*)(IO_ADDRESS(Reg_Addr)))
#define REG_OFFSET_SHIFT_BITS           2
#ifndef  GET_REG_ADDR
#define GET_REG_ADDR(x, y)              (x+((y)<<REG_OFFSET_SHIFT_BITS))
#endif

#define REG_FCIE(reg_addr)              REG_FCIE_U16(reg_addr)
#define REG_FCIE_W(reg_addr, val)       REG_FCIE(reg_addr) = (val)
#define REG_FCIE_R(reg_addr, val)       val = REG_FCIE(reg_addr)
#define REG_FCIE_SETBIT(reg_addr, val)  REG_FCIE(reg_addr) |= (val)
#define REG_FCIE_CLRBIT(reg_addr, val)  REG_FCIE(reg_addr) &= ~(val)
#define REG_FCIE_W1C(reg_addr, val)     REG_FCIE_W(reg_addr, REG_FCIE(reg_addr)&(val))

//------------------------------
#define RIU_PM_BASE                     0x1F000000
#define RIU_BASE                        0x1F200000

#define REG_PAD_TOP0                    0x1280U
#define REG_PAD_TOP1                    0x1300U

#define REG_BANK_FCIE0                  0x8980U
#define REG_BANK_FCIE1                  0x8A00U
#define REG_BANK_FCIE2                  0x8A80U

#define REG_BANK_EMMC_PLL               0x11F80U

#define FCIE0_BASE                      GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE0)
#define FCIE1_BASE                      GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE1)
#define FCIE2_BASE                      GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE2)

#define PAD_TOP0_BASE                   GET_REG_ADDR(RIU_BASE, REG_PAD_TOP0)
#define PAD_TOP1_BASE                   GET_REG_ADDR(RIU_BASE, REG_PAD_TOP1)

#define FCIE0_BASE                      GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE0)
#define FCIE1_BASE                      GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE1)
#define FCIE2_BASE                      GET_REG_ADDR(RIU_BASE, REG_BANK_FCIE2)

#define EMMC_PLL_BASE                   GET_REG_ADDR(RIU_BASE, REG_BANK_EMMC_PLL)

#define FCIE_REG_BASE_ADDR              GET_REG_ADDR(FCIE0_BASE, 0x00)
#define FCIE_CMDFIFO_BASE_ADDR          GET_REG_ADDR(FCIE0_BASE, 0x20)
#define FCIE_CIFD_BASE_ADDR             GET_REG_ADDR(FCIE1_BASE, 0x00)

#define FCIE_NC_WBUF_CIFD_BASE          GET_REG_ADDR(FCIE1_BASE, 0x00)
#define FCIE_NC_RBUF_CIFD_BASE          GET_REG_ADDR(FCIE1_BASE, 0x20)

#define FCIE_POWEER_SAVE_MODE_BASE      GET_REG_ADDR(FCIE2_BASE, 0x00)

#include "eMMC_reg_v5.h"

//--------------------------------clock gen------------------------------------
#define REG_BANK_CLKGEN0                0x580U	// (0x100B - 0x1000) x 80h
#define CLKGEN0_BASE                    GET_REG_ADDR(RIU_BASE, REG_BANK_CLKGEN0)

#define reg_ckg_fcie                    GET_REG_ADDR(CLKGEN0_BASE, 0x64)
#define BIT_FCIE_CLK_GATING             BIT8
#define BIT_FCIE_CLK_INVERSE            BIT9
#define BIT_CLKGEN_FCIE_MASK            (BIT13|BIT12|BIT11|BIT10)
#define BIT_FCIE_CLK_SRC_SEL            BIT14 // 0: clk_xtal 12M, 1: clk_nfie_p1
#define BIT_CKG_NFIE_DVI4_EN            BIT7

//--------------------------------pad top--------------------------------------
#define reg_emmc_rstz_en                GET_REG_ADDR(PAD_TOP1_BASE, 0x04)
#define BIT_emmc_rstz_en                (BIT14|BIT13)

#define reg_all_pad_in                  GET_REG_ADDR(PAD_TOP1_BASE, 0x00)
#define BIT_ALL_PAD_IN                  BIT0

#define reg_sd_config                   GET_REG_ADDR(PAD_TOP1_BASE, 0x0A)
#define BIT_SD_MODE_MASK                BIT6

#define reg_emmc_config                 GET_REG_ADDR(PAD_TOP1_BASE, 0x04)
#define BIT_EMMC_MODE_MASK              (BIT12|BIT11)
#define BIT_EMMC_MODE_1                 BIT11
#define BIT_EMMC_MODE_2                 BIT12

#define reg_nand_config                 GET_REG_ADDR(PAD_TOP1_BASE, 0x08)
#define BIT_NAND_MODE_MASK              (BIT13|BIT12)

#define reg_padtop0_0x16                GET_REG_ADDR(PAD_TOP0_BASE, 0x16)

#define reg_padtop1_0x22                GET_REG_ADDR(PAD_TOP1_BASE, 0x22)
#define reg_padtop1_0x23                GET_REG_ADDR(PAD_TOP1_BASE, 0x23)
#define reg_padtop1_0x24                GET_REG_ADDR(PAD_TOP1_BASE, 0x24)
#define reg_padtop1_0x28                GET_REG_ADDR(PAD_TOP1_BASE, 0x28)
#define reg_padtop1_0x29                GET_REG_ADDR(PAD_TOP1_BASE, 0x29)
#define reg_padtop1_0x2A                GET_REG_ADDR(PAD_TOP1_BASE, 0x2A)
#define reg_padtop1_0x2B                GET_REG_ADDR(PAD_TOP1_BASE, 0x2B)
#define reg_padtop1_0x2C                GET_REG_ADDR(PAD_TOP1_BASE, 0x2C)


//--------------------------------clock gen------------------------------------
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

#define eMMC_FCIE_VALID_CLK_CNT         5 // FIXME

#define PLL_SKEW4_CNT                   9
#define MIN_OK_SKEW_CNT                 5
extern  U8 gau8_FCIEClkSel[];

typedef struct _eMMC_FCIE_ATOP_SET {
    U32 u32_ScanResult;
    U8  u8_Clk;
    U8  u8_Reg2Ch, u8_Skew4;
    U8  u8_Cell;
    U8  u8_Skew2, u8_CellCnt;
} eMMC_FCIE_ATOP_SET_t;


typedef struct _eMMC_FCIE_ATOP_SET_EXT {
    U32 au32_RXDLLResult[5];
    U8 u8_Skew4Idx;
    U8 au8_Reg2Ch[5], au8_Skew4[5];
    U8 au8_Cell[5], au8_CellCnt[5];
} eMMC_FCIE_ATOP_SET_EXT_t;


#define eMMC_RST_L()                    {REG_FCIE_CLRBIT(reg_emmc_rstz_en, BIT_emmc_rstz_en);\
                                         REG_FCIE_SETBIT(FCIE_BOOT_CONFIG, BIT_EMMC_RSTZ_EN);\
                                         REG_FCIE_CLRBIT(FCIE_BOOT_CONFIG, BIT_EMMC_RSTZ);}
#define eMMC_RST_H()                    {REG_FCIE_CLRBIT(reg_emmc_rstz_en, BIT_emmc_rstz_en);\
                                         REG_FCIE_SETBIT(FCIE_BOOT_CONFIG, BIT_EMMC_RSTZ_EN);\
                                         REG_FCIE_SETBIT(FCIE_BOOT_CONFIG, BIT_EMMC_RSTZ);}


#define REG_BANK_TIMER1                 0x1800
#define TIMER1_BASE                     GET_REG_ADDR(RIU_PM_BASE, REG_BANK_TIMER1)

#define TIMER1_ENABLE                   GET_REG_ADDR(TIMER1_BASE, 0x20)
#define TIMER1_HIT                      GET_REG_ADDR(TIMER1_BASE, 0x21)
#define TIMER1_MAX_LOW                  GET_REG_ADDR(TIMER1_BASE, 0x22)
#define TIMER1_MAX_HIGH                 GET_REG_ADDR(TIMER1_BASE, 0x23)
#define TIMER1_CAP_LOW                  GET_REG_ADDR(TIMER1_BASE, 0x24)
#define TIMER1_CAP_HIGH                 GET_REG_ADDR(TIMER1_BASE, 0x25)

//--------------------------------power saving mode----------------------------
#define REG_BANK_PM_SLEEP               (0x700)
#define PM_SLEEP_REG_BASE_ADDR          GET_REG_ADDR(RIU_PM_BASE, REG_BANK_PM_SLEEP)
#define reg_pwrgd_int_glirm             GET_REG_ADDR(PM_SLEEP_REG_BASE_ADDR, 0x61)
#define BIT_PWRGD_INT_GLIRM_EN          BIT8
#define BIT_PWEGD_INT_GLIRM_MASK        (BIT15|BIT14|BIT13|BIT12|BIT11|BIT10)


//=====================================================
// API declarations
//=====================================================

extern  U32 eMMC_hw_timer_delay(U32 u32us);
extern  U32 eMMC_hw_timer_sleep(U32 u32ms);

#define eMMC_HW_TIMER_MHZ               12000000//12MHz  [FIXME]
// define what latch method (mode) fcie has
// implement switch pad function with below cases

#define FCIE_eMMC_BYPASS                0

#define ENABLE_AFIFO                    0


// define what speed we want this chip/project run
//------------------------------
// DDR48, DDR52, HS200, HS400
#define IF_DETECT_eMMC_DDR_TIMING       0 // DDR48 (digital macro)
#define ENABLE_eMMC_ATOP                0
#define eMMC_IF_TUNING_TTABLE()         (g_eMMCDrv.u32_DrvFlag&DRV_FLAG_TUNING_TTABLE)

// mboot use this config

#define WRITE_TO_eMMC                   0
#define READ_FROM_eMMC                  1

#define MSTAR_MIU0_BUS_BASE             0x20000000

#ifndef MSTAR_MIU0_BUS_BASE
#define MSTAR_MIU0_BUS_BASE MIU0_BUS_ADDR
#endif

extern  U32 eMMC_pads_switch(U32 u32_FCIE_IF_Type);
extern  U32 eMMC_clock_setting(U16 u16_ClkParam);
extern  U32 eMMC_clock_gating(void);
//extern  U32 eMMC_config_clock(U16 u16_SeqAccessTime);
extern void eMMC_set_WatchDog(U8 u8_IfEnable);
extern void eMMC_reset_WatchDog(void);
extern  U32 eMMC_translate_DMA_address_Ex(U32 u32_DMAAddr, U32 u32_ByteCnt);
dma_addr_t eMMC_DMA_MAP_address(uintptr_t ulongBuffer, U32 u32_ByteCnt, int mode);
void eMMC_DMA_UNMAP_address(dma_addr_t dma_DMAAddr, U32 u32_ByteCnt, int mode);
extern void eMMC_Invalidate_data_cache_buffer(U32 u32_addr, S32 s32_size);
extern void eMMC_flush_miu_pipe(void);
extern  U32 eMMC_PlatformResetPre(void);
extern  U32 eMMC_PlatformResetPost(void);
extern  U32 eMMC_PlatformInit(void);
extern  U32 eMMC_CheckIfMemCorrupt(void);
extern void eMMC_DumpPadClk(void);
#define eMMC_BOOT_PART_W   BIT0
#define eMMC_BOOT_PART_R   BIT1
extern  U32 eMMC_BootPartitionHandler_WR(U8 *pDataBuf, U16 u16_PartType, U32 u32_StartSector, U32 u32_SectorCnt, U8 u8_OP);
extern  U32 eMMC_BootPartitionHandler_E(U16 u16_PartType);
extern  U32 eMMC_hw_timer_start(void);
extern  U32 eMMC_hw_timer_tick(void);
extern  irqreturn_t eMMC_FCIE_IRQ(int irq, void *dummy); // [FIXME]
extern  U32 eMMC_WaitCompleteIntr(uintptr_t u32_RegAddr, U16 u16_WaitEvent, U32 u32_MicroSec);
//extern  struct mutex FCIE3_mutex;
extern  void eMMC_LockFCIE(U8 *pu8_str);
extern  void eMMC_UnlockFCIE(U8 *pu8_str);
extern 	int mstar_mci_Housekeep(void *pData);
extern U32 eMMC_PlatformDeinit(void);

//=====================================================
// partitions config
//=====================================================
// every blk is 512 bytes (reserve 2MB for internal use)
// reserve 0x1200 x 0x200, more than 2MB
#define eMMC_DRV_RESERVED_BLK_CNT       (0x200000/0x200)

#define eMMC_CIS_NNI_BLK_CNT            2
#define eMMC_CIS_PNI_BLK_CNT            2
#define eMMC_TEST_BLK_CNT               (0x100000/0x200)

#define eMMC_CIS_BLK_0                  (64*1024/512) // from 64KB
#define eMMC_NNI_BLK_0                  (eMMC_CIS_BLK_0+0)
#define eMMC_NNI_BLK_1                  (eMMC_CIS_BLK_0+1)
#define eMMC_PNI_BLK_0                  (eMMC_CIS_BLK_0+2)
#define eMMC_PNI_BLK_1                  (eMMC_CIS_BLK_0+3)
#define eMMC_DDRTABLE_BLK_0             (eMMC_CIS_BLK_0+4)
#define eMMC_DDRTABLE_BLK_1             (eMMC_CIS_BLK_0+5)

// last 1MB in reserved area, use for eMMC test
#define eMMC_TEST_BLK_0                 (eMMC_DRV_RESERVED_BLK_CNT-eMMC_TEST_BLK_CNT)


#define eMMC_LOGI_PART		    0x8000 // bit-or if the partition needs Wear-Leveling
#define eMMC_HIDDEN_PART	    0x4000 // bit-or if this partition is hidden, normally it is set for the LOGI PARTs.

#define eMMC_PART_HWCONFIG		(1|eMMC_LOGI_PART)
#define eMMC_PART_BOOTLOGO		(2|eMMC_LOGI_PART)
#define eMMC_PART_BL			(3|eMMC_LOGI_PART|eMMC_HIDDEN_PART)
#define eMMC_PART_OS			(4|eMMC_LOGI_PART)
#define eMMC_PART_CUS			(5|eMMC_LOGI_PART)
#define eMMC_PART_UBOOT			(6|eMMC_LOGI_PART|eMMC_HIDDEN_PART)
#define eMMC_PART_SECINFO		(7|eMMC_LOGI_PART|eMMC_HIDDEN_PART)
#define eMMC_PART_OTP			(8|eMMC_LOGI_PART|eMMC_HIDDEN_PART)
#define eMMC_PART_RECOVERY		(9|eMMC_LOGI_PART)
#define eMMC_PART_E2PBAK		(10|eMMC_LOGI_PART)
#define eMMC_PART_NVRAMBAK		(11|eMMC_LOGI_PART)
#define eMMC_PART_APANIC		(12|eMMC_LOGI_PART)
#define eMMC_PART_MISC			(14|eMMC_LOGI_PART)


#define eMMC_PART_FDD			(17|eMMC_LOGI_PART)
#define eMMC_PART_TDD			(18|eMMC_LOGI_PART)

#define eMMC_PART_E2P0          (19|eMMC_LOGI_PART)
#define eMMC_PART_E2P1          (20|eMMC_LOGI_PART)
#define eMMC_PART_NVRAM0		(21|eMMC_LOGI_PART)
#define eMMC_PART_NVRAM1		(22|eMMC_LOGI_PART)
#define eMMC_PART_SYSTEM		(23|eMMC_LOGI_PART)
#define eMMC_PART_CACHE			(24|eMMC_LOGI_PART)
#define eMMC_PART_DATA			(25|eMMC_LOGI_PART)
#define eMMC_PART_FAT 			(26|eMMC_LOGI_PART)



#define eMMC_PART_ENV			(0x0D|eMMC_LOGI_PART|eMMC_HIDDEN_PART) // uboot env
#define eMMC_PART_DEV_NODE		(0x0F|eMMC_LOGI_PART|eMMC_HIDDEN_PART)
#define eMMC_PART_MIU	(0x20|eMMC_LOGI_PART|eMMC_HIDDEN_PART)
#define eMMC_PART_EBOOT (0x21|eMMC_LOGI_PART|eMMC_HIDDEN_PART)
#define eMMC_PART_RTK (0x22|eMMC_LOGI_PART|eMMC_HIDDEN_PART)
#define eMMC_PART_PARAMS (0x23|eMMC_LOGI_PART|eMMC_HIDDEN_PART)
#define eMMC_PART_NVRAM 	(0x88|eMMC_LOGI_PART) //default start from 31 count




//extern char *gpas8_eMMCPartName[];

//=====================================================
// Driver configs
//=====================================================
#define DRIVER_NAME                     "mstar_mci"
#define eMMC_UPDATE_FIRMWARE            0

#define eMMC_ST_PLAT                    0x80000000
// [CAUTION]: to verify IP and HAL code, defaut 0
#define IF_IP_VERIFY						0 // [FIXME] -->
// [CAUTION]: to detect DDR timiing parameters, only for DL
// need to eMMC_pads_switch
// need to eMMC_clock_setting
#define IF_FCIE_SHARE_IP                    1

//------------------------------
#define FICE_BYTE_MODE_ENABLE               1 // always 1
#define ENABLE_eMMC_INTERRUPT_MODE			0
#define ENABLE_eMMC_RIU_MODE				0 // for debug cache issue

#if ENABLE_eMMC_RIU_MODE
#undef IF_DETECT_eMMC_DDR_TIMING
#define IF_DETECT_eMMC_DDR_TIMING			0 // RIU mode can NOT use DDR
#endif
// <-- [FIXME]

#define ENABLE_FCIE_HW_BUSY_CHECK           1

#define ENABLE_EMMC_ASYNC_IO                1
#define ENABLE_EMMC_PRE_DEFINED_BLK         1
#define ENABLE_FCIE_ADMA                    1

//------------------------------
#define eMMC_FEATURE_RELIABLE_WRITE         0
#if eMMC_UPDATE_FIRMWARE
#undef  eMMC_FEATURE_RELIABLE_WRITE
#define eMMC_FEATURE_RELIABLE_WRITE         0
#endif

//------------------------------
//#define eMMC_RSP_FROM_RAM_SAVE              1
//#define eMMC_RSP_FROM_RAM_LOAD              0
#define eMMC_RSP_FROM_RAM               0

#define eMMC_FCIE_LINUX_DRIVER	1

//------------------------------
//-------------------------------------------------------
// Devices has to be in 512B block length mode by default
// after power-on, or software reset.
//-------------------------------------------------------

#define eMMC_SECTOR_512BYTEa       0x200
#define eMMC_SECTOR_512BYTE_BITSa  9
#define eMMC_SECTOR_512BYTE_MASKa  (eMMC_SECTOR_512BYTEa-1)

#define eMMC_SECTOR_BUF_16KBa      (eMMC_SECTOR_512BYTEa * 0x20)

#define eMMC_SECTOR_BYTECNTa       eMMC_SECTOR_512BYTEa
#define eMMC_SECTOR_BYTECNT_BITSa  eMMC_SECTOR_512BYTE_BITSa

#define eMMC_SECTOR_BUF_BYTECTN   eMMC_SECTOR_BUF_16KBa
extern U8 *gau8_eMMC_SectorBuf; // 512 bytes
extern U8 *gau8_eMMC_PartInfoBuf; // 512 bytes
//U8 gau8_eMMC_SectorBuf[eMMC_SECTOR_BUF_16KBa]; // 512 bytes
//U8 gau8_eMMC_PartInfoBuf[eMMC_SECTOR_512BYTEa]; // 512 bytes
//------------------------------
// Boot Partition:
//   [FIXME]: if platform has ROM code like G2P
//------------------------------
//	No Need in A3
#define BL_BLK_OFFSET          0
#define BL_BLK_CNT            (0xF200/0x200)
#define OTP_BLK_OFFSET         BL_BLK_CNT
#define OTP_BLK_CNT           (0x8000/0x200)
#define SecInfo_BLK_OFFSET    (BL_BLK_CNT+OTP_BLK_CNT)
#define SecInfo_BLK_CNT       (0x1000/0x200)
#define BOOT_PART_TOTAL_CNT   (BL_BLK_CNT+OTP_BLK_CNT+SecInfo_BLK_CNT)
// <-- [FIXME]

#define eMMC_CACHE_LINE		   0x40 // [FIXME]

//=====================================================
// tool-chain attributes
//===================================================== [FIXME] -->
#define eMMC_PACK0
#define eMMC_PACK1					__attribute__((__packed__))
#define eMMC_ALIGN0
#define eMMC_ALIGN1					__attribute__((aligned(eMMC_CACHE_LINE)))
// <-- [FIXME]

//=====================================================
// debug option
//=====================================================
#define eMMC_TEST_IN_DESIGN					0 // [FIXME]: set 1 to verify HW timer

#ifndef eMMC_DEBUG_MSG
#define eMMC_DEBUG_MSG						1
#endif

/* Define trace levels. */
#define eMMC_DEBUG_LEVEL_ERROR				(1)    /* Error condition debug messages. */
#define eMMC_DEBUG_LEVEL_WARNING			(2)    /* Warning condition debug messages. */
#define eMMC_DEBUG_LEVEL_HIGH				(3)    /* Debug messages (high debugging). */
#define eMMC_DEBUG_LEVEL_MEDIUM				(4)    /* Debug messages. */
#define eMMC_DEBUG_LEVEL_LOW				(5)    /* Debug messages (low debugging). */

/* Higer debug level means more verbose */
#ifndef eMMC_DEBUG_LEVEL
#define eMMC_DEBUG_LEVEL					eMMC_DEBUG_LEVEL_WARNING//
#endif

#if defined(eMMC_DEBUG_MSG) && eMMC_DEBUG_MSG
#define eMMC_printf    pr_info                       // <-- [FIXME]
#define eMMC_debug(dbg_lv, tag, str, ...)	         \
	do {	                                         \
		if (dbg_lv > eMMC_DEBUG_LEVEL)				 \
			break;									 \
		else if(eMMC_IF_DDRT_TUNING())               \
			break;                                   \
		else {										 \
			if (tag)								 \
				eMMC_printf("[ %s() ] ", __func__);\
													 \
			eMMC_printf(str, ##__VA_ARGS__);		 \
		} \
	} while(0)
#else /* eMMC_DEBUG_MSG */
#define eMMC_printf(...)
#define eMMC_debug(enable, tag, str, ...)	do{}while(0)
#endif /* eMMC_DEBUG_MSG */

#define eMMC_die(msg) while(1);//SYS_FAIL(""msg);

#define eMMC_stop() \
	while(1)  eMMC_reset_WatchDog();

//=====================================================
// unit for HW Timer delay (unit of us)
//=====================================================
#define HW_TIMER_DELAY_1us	    1
#define HW_TIMER_DELAY_5us	    5
#define HW_TIMER_DELAY_10us     10
#define HW_TIMER_DELAY_100us	100
#define HW_TIMER_DELAY_500us	500
#define HW_TIMER_DELAY_1ms	    (1000 * HW_TIMER_DELAY_1us)
#define HW_TIMER_DELAY_5ms	    (5    * HW_TIMER_DELAY_1ms)
#define HW_TIMER_DELAY_10ms	    (10   * HW_TIMER_DELAY_1ms)
#define HW_TIMER_DELAY_100ms	(100  * HW_TIMER_DELAY_1ms)
#define HW_TIMER_DELAY_500ms	(500  * HW_TIMER_DELAY_1ms)
#define HW_TIMER_DELAY_1s	    (1000 * HW_TIMER_DELAY_1ms)

//=====================================================
// set FCIE clock
//=====================================================
#define FCIE_SLOWEST_CLK                BIT_FCIE_CLK_4X_300K
#define FCIE_SLOW_CLK                   BIT_FCIE_CLK_4X_48M
#define FCIE_DEFAULT_CLK                BIT_FCIE_CLK_4X_192M

//=====================================================
// transfer DMA Address
//=====================================================
#define MIU_BUS_WIDTH_BITS					3 // 8 bytes width [FIXME]
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


#endif /* __eMMC_INFINITY2_LINUX__ */
