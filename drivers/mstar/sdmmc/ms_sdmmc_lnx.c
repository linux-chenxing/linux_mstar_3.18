/***************************************************************************************************************
 *
 * FileName ms_sdmmc_lnx.c
 *     @author jeremy.wang (2012/01/10)
 * Desc:
 *     This layer between Linux SD Driver layer and IP Hal layer.
 *     (1) The goal is we don't need to change any Linux SD Driver code, but we can handle here.
 *     (2) You could define Function/Ver option for using, but don't add Project option here.
 *     (3) You could use function option by Project option, but please add to ms_sdmmc.h
 *
 ***************************************************************************************************************/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/mmc/sd.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/version.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <asm/io.h>

#include "inc/ms_sdmmc_lnx.h"
#include "inc/hal_sd_platform.h"

//###########################################################################################################
#if (D_FCIE_M_VER == D_FCIE_M_VER__04)
//###########################################################################################################
#include "inc/hal_sdmmc.h"
#include "inc/hal_card_intr.h"
//###########################################################################################################
#elif (D_FCIE_M_VER == D_FCIE_M_VER__05)
//###########################################################################################################
#include "inc/hal_sdmmc_v5.h"
#include "inc/hal_card_intr_v5.h"
//###########################################################################################################
#endif

#if (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/clk.h>
#endif


//***********************************************************************************************************
// Config Setting (Internel)
//***********************************************************************************************************
#define EN_SDMMC_TRFUNC          (FALSE)
#define EN_SDMMC_TRSDIO          (FALSE)
#define EN_SDMMC_MIEINT_MODE     (TRUE)
#define EN_SDMMC_BRO_DMA         (TRUE)
#define EN_SDMMC_ADV_DMA         (TRUE)
#define EN_SDMMC_DCACHE_FLUSH    (TRUE)

#define EN_SDMMC_POLLING_CDZ     (TRUE)
#define EN_SDMMC_CHG_PWR_PIN     (FALSE)
#define EN_SDMMC_NOCDZ_NDERR     (FALSE)
#define EN_SDMMC_AUTO_REDET      (FALSE)

/****** For Allocation buffer *******/
#define MAX_BLK_SIZE              512       //Maximum Transfer Block Size
#define MAX_BLK_COUNT             1024      //Maximum Transfer Block Count
#define MAX_SEG_CNT               128

/****** For broken DMA *******/
#define MAX_BRO_BLK_COUNT         1024      //Maximum Broken DMA Transfer Block Count

/****** For SD Debounce Setting *******/
#define WT_DB_PLUG                30       //Waiting time for Insert Debounce
#define WT_DB_UNPLUG              30       //Waiting time for Unplug Debounce
#define WT_DB_SW_PLUG             300       //Waiting time for Plug Delay Process
#define WT_DB_SW_UNPLUG           0         //Waiting time for Uplug Delay Process

//***********************************************************************************************************

// Const Variable for Dynanmic Setting
//-----------------------------------------------------------------------------------------------------------

static const IPEmType ge_IPSet[3]             = {D_IP1_IP, D_IP2_IP, D_IP3_IP};
static const PortEmType ge_PORTSet[3]         = {D_IP1_PORT, D_IP2_PORT, D_IP3_PORT};
static const U16_T gu16_MieIntNoSet[3]        = {V_IP1_MIEIRQ, V_IP2_MIEIRQ, V_IP3_MIEIRQ};

static const PADEmType  ge_PADSet[3]          = {EV_PAD1, EV_PAD2, EV_PAD3};
static const U16_T gu16_CdzIntNoSet[3]        = {V_PAD1_CDZIRQ, V_PAD2_CDZIRQ, V_PAD3_CDZIRQ};

static const BOOL_T gb_ShareCDZIRQSet[3]      = {EN_PAD1_CDZIRQ_SHARD, EN_PAD2_CDZIRQ_SHARD, EN_PAD3_CDZIRQ_SHARD};
static const BOOL_T gb_WakeupCDZIRQSet[3]     = {EN_PAD1_CDZIRQ_WAKEUP, EN_PAD2_CDZIRQ_WAKEUP, EN_PAD3_CDZIRQ_WAKEUP};
static const U32_T  gu32_CDZIRQParaSet[3]     = {V_PAD1_CDZIRQ_PARA, V_PAD2_CDZIRQ_PARA, V_PAD3_CDZIRQ_PARA};

// Global Variable for Each Slot
//-----------------------------------------------------------------------------------------------------------
static U8_T gu8_IPOrderSlot[3]                = {V_SLOT0_IP, V_SLOT1_IP, V_SLOT2_IP};
static U8_T gu8_PADOrderSlot[3]               = {V_SLOT0_PAD, V_SLOT1_PAD, V_SLOT2_PAD};

static IPEmType ge_IPSlot[3]                  = {D_IP1_IP, D_IP2_IP, D_IP3_IP};
static PortEmType ge_PORTSlot[3]              = {D_IP1_PORT, D_IP2_PORT, D_IP3_PORT};
static U16_T gu16_MieIntNoSlot[3]             = {V_IP1_MIEIRQ, V_IP2_MIEIRQ, V_IP3_MIEIRQ};

static PADEmType  ge_PADSlot[3]               = {EV_PAD1, EV_PAD2, EV_PAD3};
static U16_T gu16_CdzIntNoSlot[3]             = {V_PAD1_CDZIRQ, V_PAD2_CDZIRQ, V_PAD3_CDZIRQ};

static BOOL_T gb_ShareCDZIRQSlot[3]           = {EN_PAD1_CDZIRQ_SHARD, EN_PAD2_CDZIRQ_SHARD, EN_PAD3_CDZIRQ_SHARD};
static BOOL_T gb_WakeupCDZIRQSlot[3]          = {EN_PAD1_CDZIRQ_WAKEUP, EN_PAD2_CDZIRQ_WAKEUP, EN_PAD3_CDZIRQ_WAKEUP};
static U32_T  gu32_CDZIRQParaSlot[3]          = {V_PAD1_CDZIRQ_PARA, V_PAD2_CDZIRQ_PARA, V_PAD3_CDZIRQ_PARA};

static MutexEmType ge_MutexSlot[3]            = {D_SDMMC1_MUTEX, D_SDMMC2_MUTEX, D_SDMMC3_MUTEX};

static U32_T  gu32_MaxDLVLSlot[3]             = {V_SDMMC1_MAX_DLVL, V_SDMMC2_MAX_DLVL, V_SDMMC3_MAX_DLVL};
static U32_T  gu32_PassLVLSlot[3]             = {V_SDMMC1_PASS_LVL, V_SDMMC2_PASS_LVL, V_SDMMC3_PASS_LVL};

static BOOL_T gb_DownLVLSlot[3]               = {EV_SDMMC1_DOWN_LVL, EV_SDMMC2_DOWN_LVL, EV_SDMMC3_DOWN_LVL};
static BOOL_T gb_SDIOIRQSlot[3]               = {EV_SDMMC1_SDIO_IRQ, EV_SDMMC2_SDIO_IRQ, EV_SDMMC3_SDIO_IRQ};
static BOOL_T gb_SDIOPRTSlot[3]               = {EV_SDMMC1_SDIO_PRT, EV_SDMMC2_SDIO_PRT, EV_SDMMC3_SDIO_PRT};

//Dync DTS Setting
static U32_T  gu32_MaxClkSlot[3]              = {V_SDMMC1_MAX_CLK, V_SDMMC2_MAX_CLK, V_SDMMC3_MAX_CLK};
static BOOL_T gb_IntCDZSlot[3]                = {EN_SDMMC1_INTCDZ, EN_SDMMC2_INTCDZ, EN_SDMMC3_INTCDZ};
static BOOL_T gb_FakeCDZSlot[3]               = {EN_SDMMC1_FAKECDZ, EN_SDMMC2_FAKECDZ, EN_SDMMC3_FAKECDZ};
static U32_T  gu32_PwrNoSlot[3]               = {V_PAD1_PWRGPIO, V_PAD2_PWRGPIO, V_PAD3_PWRGPIO};



static IntSourceStruct  gst_IntSourceSlot[3];

#if (D_FCIE_M_VER == D_FCIE_M_VER__04)
static spinlock_t g_RegLockSlot[3];
#endif

#if (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))
struct clk* gp_clkSlot[3];
struct clk* gp_clkMCMSlot[3];
struct clk* gp_clkSRAMSlot[3];
struct clk* gp_clkSDSync;
#endif


// Global Variable for All Slot:
//-----------------------------------------------------------------------------------------------------------
static BOOL_T  gb_ReverseCDZ                  = EN_SDMMC_CDZREV;
static U8_T   gu8_SlotNums                    = V_SDMMC_SLOTNUMS;

static volatile BOOL_T   gb_RejectSuspend     = (FALSE);
static volatile BOOL_T   gb_AlwaysSwitchPAD   = (FALSE);

extern struct mutex FCIE3_mutex; // use fcie mutex in other driver
DEFINE_MUTEX(sdmmc1_mutex);
DEFINE_MUTEX(sdmmc2_mutex);
DEFINE_MUTEX(sdmmc3_mutex);


#define L3_FLUSH_PIPE()   Chip_Flush_MIU_Pipe()


// String Name
//-----------------------------------------------------------------------------------------------------------
#define DRIVER_NAME "ms_sdmmc"
#define DRIVER_DESC "Mstar SD/MMC Card Interface driver"

// Trace Funcion
//-----------------------------------------------------------------------------------------------------------
#define pr_sd(fmt, arg...)          printk(KERN_CONT fmt, ##arg)     //always print

#if (EN_SDMMC_TRFUNC)
    #define pr_sd_err(fmt, arg...)   //
    #define pr_sd_main(fmt, arg...)  printk(KERN_CONT fmt, ##arg)
    #define pr_sd_dbg(fmt, arg...)   //printk(fmt, ##arg)
#else
    #define pr_sd_err(fmt, arg...)   printk(KERN_CONT fmt, ##arg)
    #define pr_sd_main(fmt, arg...)  //
    #define pr_sd_dbg(fmt, arg...)   //
#endif

#if (EN_SDMMC_TRSDIO)
    #define pr_sdio_main(fmt, arg...)  printk(KERN_CONT, fmt, ##arg)
#else
    #define pr_sdio_main(fmt, arg...)
#endif


// Section Process Begin
//------------------------------------------------------------------------------------------------
static void _CRIT_SECT_BEGIN(SlotEmType eSlot)
{
    MutexEmType eMutex = ge_MutexSlot[eSlot];
    IPEmType eIP      = ge_IPSlot[eSlot];

    if(eMutex == EV_MUTEX1)
        mutex_lock(&sdmmc1_mutex);
    //else if(eMutex == EV_MUTEXS)
        //mutex_lock(&FCIE3_mutex);
    else if(eMutex == EV_MUTEX2)
        mutex_lock(&sdmmc2_mutex);
    else if(eMutex == EV_MUTEX3)
        mutex_lock(&sdmmc3_mutex);

    Hal_SDPLT_IPBeginSettnig(eIP);

}


// Section Process End
//------------------------------------------------------------------------------------------------
static void _CRIT_SECT_END(SlotEmType eSlot)
{
    MutexEmType eMutex = ge_MutexSlot[eSlot];
    IPEmType eIP      = ge_IPSlot[eSlot];

    Hal_SDPLT_IPEndSettnig(eIP);

    if(eMutex == EV_MUTEX1)
        mutex_unlock(&sdmmc1_mutex);
    //else if(eMutex == EV_MUTEXS)
        //mutex_unlock(&FCIE3_mutex);
    else if(eMutex == EV_MUTEX2)
        mutex_unlock(&sdmmc2_mutex);
    else if(eMutex == EV_MUTEX3)
        mutex_unlock(&sdmmc3_mutex);

}


//
//-----------------------------------------------------------------------------------------------------------
#if (D_FCIE_M_VER == D_FCIE_M_VER__04)
static void _CRIT_SetMIEIntEn_ForV4(SlotEmType eSlot, TransEmType eTransType)
{
    unsigned long flags;
    IPEmType eIP  = ge_IPSlot[eSlot];

    spin_lock_irqsave(&g_RegLockSlot[eSlot], flags);

    Hal_SDMMC_SetMIEIntEn_ForV4(eIP, eTransType);

    spin_unlock_irqrestore(&g_RegLockSlot[eSlot], flags);

}
#else
#define _CRIT_SetMIEIntEn_ForV4(Slot, TransType)   //Null for FCIE5...
#endif


// Dynamic IP PAD Mapping
//------------------------------------------------------------------------------------------------
static void _Dyn_IP_PAD_Mappingg(SlotEmType eSlot)
{

    U8_T u8IP_idx   = gu8_IPOrderSlot[eSlot];
    U8_T u8PAD_idx  = gu8_PADOrderSlot[eSlot];

    ge_IPSlot[eSlot]                        = ge_IPSet[u8IP_idx];
    ge_PORTSlot[eSlot]                      = ge_PORTSet[u8IP_idx];
    //gu16_MieIntNoSlot[eSlot]                = gu16_MieIntNoSet[u8IP_idx];

    ge_PADSlot[eSlot]                       = ge_PADSet[u8PAD_idx];
    gu16_CdzIntNoSlot[eSlot]                = gu16_CdzIntNoSet[u8PAD_idx];

    gb_ShareCDZIRQSlot[eSlot]               = gb_ShareCDZIRQSet[u8PAD_idx];
    gb_WakeupCDZIRQSlot[eSlot]              = gb_WakeupCDZIRQSet[u8PAD_idx];
    gu32_CDZIRQParaSlot[eSlot]              = gu32_CDZIRQParaSet[u8PAD_idx];

}


// Initail Config
//------------------------------------------------------------------------------------------------
static S32_T _InitConfig(SlotEmType eSlot)
{
    U32_T u32RealClk =0 ;
    S32_T s32Ret = 0;
    IPEmType eIP      = ge_IPSlot[eSlot];
    PortEmType ePort  = ge_PORTSlot[eSlot];
    PADEmType ePAD    = ge_PADSlot[eSlot];

    Hal_SDPLT_SetPADPortPath(eIP, ePort, ePAD);
    Hal_SDPLT_InitIPOnce(eIP);
    Hal_SDPLT_InitPADPin(eIP, ePAD);

    //Enable sd clk source for IP_PLL(1st time) or IP
    u32RealClk = Hal_SDPLT_FindClockSetting(eIP, 400000, 0, 0);

    //Slot1 could not use dts clock, so i use none dts solution to solve it
    #if (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))

    //Mark for I2: Don't need use DTS Clock
    /*
    if(ePort == EV_PFCIE5_SDIO_PLL)
    {
        if( (s32Ret = clk_prepare_enable(gp_clkSDSync)) != 0 )
        {
            pr_err(">> [sdmmc_%u] Err: Failed to prepare SDSync Clock...(Ret:%d)!\n\n", eSlot, s32Ret);
            return s32Ret;
        }
        clk_set_rate(gp_clkSDSync, 450000000); //442000000
    }

    if( (s32Ret = clk_prepare_enable(gp_clkSlot[eSlot])) !=0 )
    {
        pr_err(">> [sdmmc_%u] Err: Failed to prepare SD Clock...(Ret:%d)!\n\n", eSlot, s32Ret);
        return s32Ret;
    }
    clk_set_rate(gp_clkSlot[eSlot], u32RealClk);
    */

    //clk_prepare_enable(gp_clkMCMSlot[slotNo]);
    //clk_prepare_enable(gp_clkSRAMSlot[slotNo]);
    Hal_SDPLT_SetClock(eIP, u32RealClk, TRUE);

    //pr_err(">> [sdmmc_%u] clk_set_rate (%u) \n", eSlot, u32RealClk);

    #else

    Hal_SDPLT_SetClock(eIP, u32RealClk, TRUE);

    #endif

    return s32Ret;

}


// Switch PAD
//------------------------------------------------------------------------------------------------
static void _SwitchPAD(SlotEmType eSlot)
{
    IPEmType eIP      = ge_IPSlot[eSlot];
    PortEmType ePort  = ge_PORTSlot[eSlot];
    PADEmType ePAD    = ge_PADSlot[eSlot];

    Hal_SDPLT_SetPADPortPath(eIP, ePort, ePAD);
    Hal_SDPLT_InitPADPin(eIP, ePAD);

}


// Set Bus Voltage
//------------------------------------------------------------------------------------------------
static unsigned char _SetBusVdd(SlotEmType eSlot, U8_T u8Vdd)
{
    IPEmType eIP      = ge_IPSlot[eSlot];
    PADEmType ePAD    = ge_PADSlot[eSlot];
    unsigned char bRet = 0;

    if(u8Vdd ==MMC_SIGNAL_VOLTAGE_180)
    {
        Hal_SDMMC_ClkCtrl(eIP, FALSE, 5);
        bRet = Hal_SDPLT_SetPADVdd(eIP, ePAD, EV_LOWVOL, 10);
        Hal_SDMMC_ClkCtrl(eIP, TRUE, 5);
    }
    else
    {
    /****** Simple Setting Here ******/
        bRet = Hal_SDPLT_SetPADVdd(eIP, ePAD, EV_NORVOL, 0);
    }

    return bRet;
}


// Set Power
//------------------------------------------------------------------------------------------------
static void _SetPower(SlotEmType eSlot, U8_T u8PowerMode)
{
    IPEmType eIP      = ge_IPSlot[eSlot];
    PADEmType ePAD    = ge_PADSlot[eSlot];

    if(u8PowerMode == MMC_POWER_OFF) // Power Off
    {
        Hal_SDMMC_ClkCtrl(eIP, FALSE, 0);
        Hal_SDPLT_PullPADPin(eIP, ePAD, EV_PULLDOWN);

        if(EN_SDMMC_CHG_PWR_PIN)
        {
            if(gu32_PwrNoSlot[eSlot]>0)
            {
                gpio_direction_output(gu32_PwrNoSlot[eSlot], 1); // GPIO Power Off
                msleep(WT_POWEROFF);
            }
        }
        else
        {
            Hal_SDPLT_PowerOff(eIP, ePAD, WT_POWEROFF); //For SD PAD
        }

    }
    else if(u8PowerMode == MMC_POWER_UP)  // Power Up
    {
        if(EN_SDMMC_CHG_PWR_PIN)
        {
            if(gu32_PwrNoSlot[eSlot]>0)
            {
                gpio_direction_output(gu32_PwrNoSlot[eSlot], 0); // GPIO Power Off
            }
        }
        else
        {
            Hal_SDPLT_PowerOn(eIP, ePAD, 5);
        }

        Hal_SDPLT_PullPADPin(eIP, ePAD, EV_PULLUP);

        //Add this for some linux version (Non 3.3V enable flow)
        Hal_SDPLT_SetPADVdd(eIP, ePAD, EV_NORVOL, WT_POWERUP);

    }
    else if(u8PowerMode == MMC_POWER_ON) // Power On
    {
        Hal_SDMMC_ClkCtrl(eIP, TRUE, WT_POWERON);
        Hal_SDMMC_Reset(eIP);
    }

}


//------------------------------------------------------------------------------------------------
static U32_T _SetClock(SlotEmType eSlot, unsigned int u32ReffClk, U8_T u8PassLevel, U8_T u8DownLevel)
{
    U32_T u32RealClk =0 ;
    IPEmType eIP = ge_IPSlot[eSlot];

    if(u32ReffClk)
    {
        u32RealClk = Hal_SDPLT_FindClockSetting(eIP, (U32_T)u32ReffClk, u8PassLevel, u8DownLevel);

    #if (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))
        //clk_set_rate(gp_clkSlot[eSlot], u32RealClk);
        Hal_SDPLT_SetClock(eIP, u32RealClk, FALSE);
    #else
        Hal_SDPLT_SetClock(eIP, u32RealClk, FALSE);
    #endif

        Hal_SDMMC_SetNrcDelay(eIP, u32RealClk);
    }

    return u32RealClk;
}


//------------------------------------------------------------------------------------------------
static void _SetBusWidth(SlotEmType eSlot, U8_T u8BusWidth)
{
    IPEmType eIP = ge_IPSlot[eSlot];

    switch(u8BusWidth)
    {
        case MMC_BUS_WIDTH_1:
            Hal_SDMMC_SetDataWidth(eIP, EV_BUS_1BIT);
            break;
        case MMC_BUS_WIDTH_4:
            Hal_SDMMC_SetDataWidth(eIP, EV_BUS_4BITS);
            break;
        case MMC_BUS_WIDTH_8:
            Hal_SDMMC_SetDataWidth(eIP, EV_BUS_8BITS);
            break;
    }
}


//------------------------------------------------------------------------------------------------
static void _SetBusTiming(SlotEmType eSlot, U8_T u8BusTiming)
{
    IPEmType eIP = ge_IPSlot[eSlot];

    switch(u8BusTiming)
    {

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0))
        case MMC_TIMING_UHS_SDR12:
#endif
        case MMC_TIMING_LEGACY:
            /****** For Default Speed ******/
            Hal_SDMMC_SetBusTiming(eIP, EV_BUS_DEF_SDR12);
            Hal_SDPLT_SetBusTiming(eIP, EV_BUS_DEF_SDR12);
            break;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0))
        case MMC_TIMING_UHS_SDR25:
#endif
        case MMC_TIMING_SD_HS:
        case MMC_TIMING_MMC_HS:
            /****** For High Speed ******/
            Hal_SDMMC_SetBusTiming(eIP, EV_BUS_HS_SDR25);
            Hal_SDPLT_SetBusTiming(eIP, EV_BUS_HS_SDR25);
            break;

        case MMC_TIMING_UHS_SDR50:
        case MMC_TIMING_UHS_SDR104:
        case MMC_TIMING_MMC_HS200:
            Hal_SDMMC_SetBusTiming(eIP, EV_BUS_SDR104_HS200);
            Hal_SDPLT_SetBusTiming(eIP, EV_BUS_SDR104_HS200);
            break;


        case MMC_TIMING_UHS_DDR50:
            Hal_SDMMC_SetBusTiming(eIP, EV_BUS_DDR50);
            Hal_SDPLT_SetBusTiming(eIP, EV_BUS_DDR50);
            break;

        default:
            /****** For 300KHz IP Issue but not for Default Speed ******/
            Hal_SDMMC_SetBusTiming(eIP, EV_BUS_LOW);
            Hal_SDPLT_SetBusTiming(eIP, EV_BUS_LOW);
            break;
    }


}

//------------------------------------------------------------------------------------------------
static BOOL_T _GetCardDetect(SlotEmType eSlot)
{
    PADEmType ePAD = ge_PADSlot[eSlot];

    if(gb_FakeCDZSlot[eSlot])
    {
        return (TRUE);
    }
    else
    {

        if(gb_ReverseCDZ)
            return !Hal_SDPLT_CDZGetGPIOState((GPIOEmType)eSlot, ePAD);
        else
            return Hal_SDPLT_CDZGetGPIOState((GPIOEmType)eSlot, ePAD);

    }

    return (FALSE);
}

//------------------------------------------------------------------------------------------------
static BOOL_T _GetWriteProtect(SlotEmType eSlot)
{

#if(EN_SDMMC_TCARD)
    return (FALSE);
#else
    IPEmType eIP = ge_IPSlot[eSlot];

    #if(EN_SDMMC_WPREV)
    return !Hal_SDMMC_WriteProtect(eIP);
    #else
    return  Hal_SDMMC_WriteProtect(eIP);
    #endif

#endif

}

//------------------------------------------------------------------------------------------------
static BOOL_T _CardDetect_PlugDebounce(SlotEmType eSlot, U32_T u32WaitMs, BOOL_T bPrePlugStatus)
{
    BOOL_T bCurrPlugStatus = bPrePlugStatus;
    U32_T u32DiffTime = 0;

    while(u32DiffTime < u32WaitMs)
    {
        mdelay(1);
        u32DiffTime++;

        bCurrPlugStatus = _GetCardDetect(eSlot);

        if (bPrePlugStatus != bCurrPlugStatus)
        {
            /****** Print the Debounce ******/
            /*if(bPrePlugStatus)
                printk("#");
            else
                printk("$");*/
            /*********************************/
            break;
        }
    }
    return bCurrPlugStatus;
}

//------------------------------------------------------------------------------------------------
static U16_T _PreDataBufferProcess(TransEmType eTransType, struct mmc_data *data, struct ms_sdmmc_slot *sdmmchost, volatile U32_T *pu32AddrArr)
{
    struct scatterlist *p_sg = 0;
    U8_T u8Dir = ( (data->flags & MMC_DATA_READ) ? DMA_FROM_DEVICE : DMA_TO_DEVICE );
    U16_T u16sg_idx = 0;

#if (!EN_SDMMC_BRO_DMA)
    U32_T *pSGbuf = 0;
    U32_T u32TranBytes = 0;
    U32_T u32TotalSize = data->blksz * data->blocks;
    unsigned *pDMAbuf = sdmmchost->dma_buffer;

#elif (EN_SDMMC_BRO_DMA) && (EN_SDMMC_ADV_DMA)
    U16_T u16SubBCnt = 0;
    U32_T u32SubLen = 0;
    U32_T u32MIUAddr = 0;
    BOOL_T bEnd = (FALSE);
    unsigned *pADMAbuf = sdmmchost->adma_buffer;

#endif

    if(eTransType == EV_CIF)
    {
        p_sg = &data->sg[0];
        pu32AddrArr[0] = (U32_T)( page_address(sg_page(p_sg)) + p_sg->offset );
        return 1;
    }

#if (EN_SDMMC_BRO_DMA)
    for(u16sg_idx=0 ; u16sg_idx< data->sg_len ; u16sg_idx++)
#else
    if(data->sg_len==1)
#endif
    {
        p_sg = &data->sg[u16sg_idx];
        p_sg->dma_address = dma_map_page(NULL, sg_page(p_sg), p_sg->offset, p_sg->length, u8Dir);

        if(dma_mapping_error(NULL, p_sg->dma_address)) //Add to avoid unmap warning!
            return 0;

        if((p_sg->dma_address==0) || (p_sg->dma_address==~0))  //Mapping Error!
            return 0;

        pu32AddrArr[u16sg_idx] = (U32_T)p_sg->dma_address;
    }

#if (EN_SDMMC_BRO_DMA) && (EN_SDMMC_ADV_DMA)

    for(u16sg_idx=0 ; u16sg_idx< data->sg_len ; u16sg_idx++)
    {
        if( u16sg_idx==((data->sg_len)-1) )
            bEnd = (TRUE);

        u32SubLen = data->sg[u16sg_idx].length;
        u16SubBCnt = (U16_T)(u32SubLen/data->blksz);
        u32MIUAddr = Hal_SDPLT_TransMIUAddr((U32_T)pu32AddrArr[u16sg_idx]);
        Hal_SDMMC_ADMASetting((volatile void *)pADMAbuf, u16sg_idx, u32SubLen, u16SubBCnt, u32MIUAddr, 0, bEnd);
    }

    L3_FLUSH_PIPE();  //Avoid L3 issue

    pu32AddrArr[0] = (U32_T) sdmmchost->adma_phy_addr;
    return 1;

#elif (EN_SDMMC_BRO_DMA)

    if(data->flags & MMC_DATA_WRITE)
        L3_FLUSH_PIPE(); //Avoid L3 issue

    return (U16_T)data->sg_len;

#else
    else
    {
        if(data->flags & MMC_DATA_WRITE)  //SGbuf => DMA buf
        {
            while(u16sg_idx < data->sg_len)
            {
                p_sg = &data->sg[u16sg_idx];

                pSGbuf = kmap_atomic(sg_page(p_sg), KM_BIO_SRC_IRQ) + p_sg->offset;
                u32TranBytes   = min(u32TotalSize, p_sg->length);
                memcpy(pDMAbuf, pSGbuf, u32TranBytes);
                u32TotalSize -= u32TranBytes;
                pDMAbuf += (u32TranBytes >> 2) ;
                kunmap_atomic(pSGbuf, KM_BIO_SRC_IRQ);

                u16sg_idx++;
            }

            L3_FLUSH_PIPE(); //Avoid L3 issue

        }

        pu32AddrArr[0] = (U32_T) sdmmchost->dma_phy_addr;

    }

    return 1;

#endif

}
//------------------------------------------------------------------------------------------------
static void _PostDataBufferProcess(TransEmType eTransType, struct mmc_data *data, struct ms_sdmmc_slot *sdmmchost)
{
    struct scatterlist *p_sg = 0;
    U8_T  u8Dir = ( (data->flags & MMC_DATA_READ) ? DMA_FROM_DEVICE : DMA_TO_DEVICE );
    U16_T u16sg_idx = 0;

#if (!EN_SDMMC_BRO_DMA)
    U32_T *pSGbuf = 0;
    U32_T u32TranBytes = 0;
    U32_T u32TotalSize = data->blksz * data->blocks;
    unsigned *pDMAbuf = sdmmchost->dma_buffer;
#endif

    if(eTransType == EV_CIF)
        return;


#if (EN_SDMMC_BRO_DMA)

    for(u16sg_idx=0 ; u16sg_idx< data->sg_len ; u16sg_idx++)
    {
        p_sg = &data->sg[u16sg_idx];
        dma_unmap_page(NULL, p_sg->dma_address, p_sg->length, u8Dir);
        if ( (u8Dir==DMA_FROM_DEVICE) && EN_SDMMC_DCACHE_FLUSH && (sg_page(p_sg) != NULL) && !PageSlab(sg_page(p_sg)) )
            flush_dcache_page(sg_page(p_sg));
    }

#else

    if(data->sg_len==1)
    {
        p_sg = &data->sg[0];
        dma_unmap_page(NULL, p_sg->dma_address, p_sg->length, u8Dir);
        if ( (u8Dir==DMA_FROM_DEVICE) && EN_SDMMC_DCACHE_FLUSH && (sg_page(p_sg) != NULL) && !PageSlab(sg_page(p_sg)) )
            flush_dcache_page(sg_page(p_sg));
    }
    else
    {
        if(data->flags & MMC_DATA_READ)  //SGbuf => DMA buf
        {
            for(u16sg_idx=0 ; u16sg_idx< data->sg_len ; u16sg_idx++)
            {
                p_sg = &data->sg[u16sg_idx];

                pSGbuf = kmap_atomic(sg_page(p_sg), KM_BIO_SRC_IRQ) + p_sg->offset;
                u32TranBytes   = min(u32TotalSize, p_sg->length);
                memcpy(pSGbuf, pDMAbuf, u32TranBytes);
                u32TotalSize -= u32TranBytes;
                pDMAbuf += (u32TranBytes >> 2) ;

                flush_dcache_page(sg_page(p_sg));
                kunmap_atomic(pSGbuf, KM_BIO_SRC_IRQ);
            }

            //Avoid L3 issue
            L3_FLUSH_PIPE();

        }
    }

#endif

}

//------------------------------------------------------------------------------------------------
static U32_T _TransArrToUInt(U8_T u8Sep1, U8_T u8Sep2, U8_T u8Sep3, U8_T u8Sep4)
{
    return ((((uint)u8Sep1)<<24) | (((uint)u8Sep2)<<16) | (((uint)u8Sep3)<<8) | ((uint)u8Sep4));
}
//------------------------------------------------------------------------------------------------
static SDMMCRspEmType _TransRspType(unsigned int u32Rsp)
{

    switch(u32Rsp)
    {
        case MMC_RSP_NONE:
            return EV_NO;
        case MMC_RSP_R1:
        //case MMC_RSP_R5:
        //case MMC_RSP_R6:
        //case MMC_RSP_R7:
            return EV_R1;
        case MMC_RSP_R1B:
            return EV_R1B;
        case MMC_RSP_R2:
            return EV_R2;
        case MMC_RSP_R3:
        //case MMC_RSP_R4:
            return EV_R3;
        default:
            return EV_R1;
    }

}
//------------------------------------------------------------------------------------------------
static BOOL_T _PassPrintCMD(SlotEmType eSlot, U8_T u32Cmd, U32_T u32Arg, BOOL_T bSDIODev)
{

    if( (u32Cmd == SD_IO_RW_DIRECT) && bSDIODev)
        return (FALSE);


    if(gb_SDIOPRTSlot[eSlot]) //SDIO Use
    {
        if(u32Cmd == SD_SEND_IF_COND)
        {
            return (TRUE);
        }
        else if(u32Cmd == SD_IO_RW_DIRECT)
        {
            if( (u32Arg == 0x00000C00) || (u32Arg == 0x80000C08))
                return (TRUE);
        }
        return (FALSE);
    }

    // SD Use
    switch(u32Cmd)
    {
        case MMC_SEND_OP_COND:   //MMC  =>Cmd_1
        case SD_IO_SEND_OP_COND: //SDIO =>Cmd_5
        case SD_SEND_IF_COND:    //SD   =>Cmd_8
        case SD_IO_RW_DIRECT:    //SDIO =>Cmd_52
        case MMC_SEND_STATUS:    //SD   =>CMD13
        case MMC_APP_CMD:        //SD   =>Cmd55
        case MMC_SEND_TUNING_BLOCK:  //SD => Cmd19
            return (TRUE);
            break;

    }

    return (FALSE);

}

//------------------------------------------------------------------------------------------------
static int _RequestEndProcess(CmdEmType eCmdType, RspErrEmType eErrType, struct ms_sdmmc_slot *p_sdmmc_slot, struct mmc_data *data)
{
    int nErr = 0;
    BOOL_T bNeedDownLevel = FALSE;
    ErrGrpEmType eErrGrp;
    SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;

    #if (EN_SDMMC_AUTO_REDET)
    IPEmType eIP = ge_IPSlot[eSlot];
    #endif

    if( eErrType == EV_STS_OK )
    {
        pr_sdio_main("_[%01X]", Hal_SDMMC_GetDATBusLevel(eIP));
        pr_sd_main("@\n");
    }
    else
    {
        pr_sd_main("=> (Err: 0x%04X)", (U16_T)eErrType);
        nErr = (U32_T) eErrType;

        if(eCmdType != EV_CMDRSP)
        {

            eErrGrp = Hal_SDMMC_ErrGroup(eErrType);

            switch((U16_T)eErrGrp)
            {
                case EV_EGRP_TOUT:
                    nErr =  -ETIMEDOUT;
                    break;

                case EV_EGRP_COMM:
                    nErr = -EILSEQ;
                    bNeedDownLevel = TRUE;
                    break;
            }



        }

    }

    /****** (1) SD Re-Detection after timeout******/
    #if (EN_SDMMC_AUTO_REDET)
    if(gb_IntCDZSlot[eSlot] || (!EN_SDMMC_POLLING_CDZ))
    {
    if(eCmdType != EV_CMDRSP)
    {
        if( eErrType == EV_STS_OK )
            p_sdmmc_slot->currTimeoutCnt = 0;
        else if(eErrType == EV_STS_MIE_TOUT)
            p_sdmmc_slot->currTimeoutCnt++;

        if(p_sdmmc_slot->currTimeoutCnt>=3)
        {
            Hal_SDMMC_StopProcessCtrl(eIP, TRUE);
            mmc_detect_change(p_sdmmc_slot->mmc, msecs_to_jiffies(WT_DB_SW_UNPLUG));
            p_sdmmc_slot->currTimeoutCnt =0;
                pr_sd("\n>> [sdmmc_%u] Auto Re-Dectection.........\n\n", eSlot);
        }
    }
    }
    #endif

    if( eErrType == EV_STS_OK )
        return nErr;


    /****** (2) Special Error Process for Stop Wait Process ******/
    if(eErrType == EV_SWPROC_ERR && data && EN_SDMMC_NOCDZ_NDERR)
    {
        data->bytes_xfered = data->blksz * data->blocks;
        nErr = 0;
        pr_sd_main("_Pass");
    }

    pr_sd_main("\n");

    /****** (3) Downgrad Clock Speed for Some Bus Errors ******/
    if(bNeedDownLevel && gb_DownLVLSlot[eSlot])
    {
        if( (p_sdmmc_slot->currDownLevel+1) <= gu32_MaxDLVLSlot[eSlot])
            p_sdmmc_slot->currDownLevel += 1;

        p_sdmmc_slot->currRealClk = _SetClock(eSlot, p_sdmmc_slot->currClk, gu32_PassLVLSlot[eSlot], p_sdmmc_slot->currDownLevel);
        pr_sd(">> [sdmmc_%u] Downgrade Clk from (Clk=%u)=> (Real=%u)\n", eSlot, p_sdmmc_slot->currClk, p_sdmmc_slot->currRealClk);
    }


    return nErr;

}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_cdzint
 *     @author jeremy.wang (2012/5/8)
 * Desc: Int funtion for GPIO Card Detection
 *
 * @param irq :
 * @param p_dev_id :
 *
 * @return irqreturn_t  :
 ----------------------------------------------------------------------------------------------------------*/
static irqreturn_t ms_sdmmc_cdzint(int irq, void *p_dev_id)
{
    irqreturn_t irq_t = IRQ_NONE;
    IntSourceStruct* pstIntSource = p_dev_id;
    SlotEmType eSlot = (SlotEmType)pstIntSource->slotNo;
    PADEmType ePAD   = ge_PADSlot[eSlot];
    struct ms_sdmmc_slot *p_sdmmc_slot = pstIntSource->p_data;

    if(!gb_ShareCDZIRQSlot[eSlot])
        disable_irq_nosync(irq);

    if(Hal_SDPLT_CDZGPIOIntFilter((GPIOEmType)eSlot, ePAD))
    {
        tasklet_schedule(&p_sdmmc_slot->hotplug_tasklet);
        irq_t = IRQ_HANDLED;
    }

    return irq_t;
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_hotplug
 *     @author jeremy.wang (2012/1/5)
 * Desc: Hotplug function for Card Detection
 *
 * @param data : ms_sdmmc_slot struct pointer
 ----------------------------------------------------------------------------------------------------------*/
static void ms_sdmmc_hotplug(unsigned long data)
{
    struct ms_sdmmc_slot  *p_sdmmc_slot = (struct ms_sdmmc_slot  *) data;
    SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;
    IPEmType eIP = ge_IPSlot[eSlot];
    GPIOOptEmType eINSOPT = EV_GPIO_OPT3;
    GPIOOptEmType eEJTOPT = EV_GPIO_OPT4;


    if (gb_ReverseCDZ)
    {
        eINSOPT = EV_GPIO_OPT4;
        eEJTOPT = EV_GPIO_OPT3;
    }


    pr_sd("\n>> [sdmmc_%u] CDZ... ", eSlot);

LABEL_LOOP_HOTPLUG:

    if( _GetCardDetect(eSlot) ) // Insert (CDZ)
    {
        if( (FALSE) == _CardDetect_PlugDebounce(eSlot, WT_DB_PLUG, TRUE) )
            goto LABEL_LOOP_HOTPLUG;

        mmc_detect_change(p_sdmmc_slot->mmc, msecs_to_jiffies(WT_DB_SW_PLUG));
        pr_sd("(INS) OK!\n");

        Hal_SDPLT_CDZSetGPIOIntAttr((GPIOEmType)eSlot, p_sdmmc_slot->cdzIRQNo, EV_GPIO_OPT1);
        Hal_SDPLT_CDZSetGPIOIntAttr((GPIOEmType)eSlot, p_sdmmc_slot->cdzIRQNo, eINSOPT);


    }
    else // Remove (CDZ)
    {
        if( (TRUE) == _CardDetect_PlugDebounce(eSlot, WT_DB_UNPLUG, FALSE) )
                goto LABEL_LOOP_HOTPLUG;

        if (p_sdmmc_slot->mmc->card)
            mmc_card_set_removed(p_sdmmc_slot->mmc->card);

        Hal_SDMMC_StopProcessCtrl(eIP, TRUE);
        mmc_detect_change(p_sdmmc_slot->mmc, msecs_to_jiffies(WT_DB_SW_UNPLUG));
        pr_sd("(EJT) OK!\n");

        Hal_SDPLT_CDZSetGPIOIntAttr((GPIOEmType)eSlot, p_sdmmc_slot->cdzIRQNo, EV_GPIO_OPT1);
        Hal_SDPLT_CDZSetGPIOIntAttr((GPIOEmType)eSlot, p_sdmmc_slot->cdzIRQNo, eEJTOPT);

    }

    if(!gb_ShareCDZIRQSlot[eSlot])
        enable_irq(p_sdmmc_slot->cdzIRQNo);

}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 1, 0))
/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_enable
 *     @author jeremy.wang (2012/3/21)
 * Desc: Something we want to do before Host Enable
 *
 * @param p_mmc_host :
 *
 * @return int  : 0 = Success, >0 = Error
 ----------------------------------------------------------------------------------------------------------*/
static int ms_sdmmc_enable(struct mmc_host *p_mmc_host)
{
    gb_RejectSuspend = TRUE;
    return 0;
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_disable
 *     @author jeremy.wang (2012/3/21)
 * Desc: Something we want to do before Host Disable
 *
 * @param p_mmc_host :
 * @param lazy : msec for delay work
 *
 * @return int  : 0 = No Delay, <0 = Error, >0 = Delay time
 ----------------------------------------------------------------------------------------------------------*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 4, 0))
static int ms_sdmmc_disable(struct mmc_host *p_mmc_host, int lazy)
{
    gb_RejectSuspend = FALSE;
    return lazy;
}
#else
static int ms_sdmmc_disable(struct mmc_host *p_mmc_host)
{
    gb_RejectSuspend = FALSE;
    return 0;
}
#endif

#endif //#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 1, 0))


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_request
 *     @author jeremy.wang (2011/5/19)
 * Desc: Request funciton for any commmand
 *
 * @param p_mmc_host : mmc_host structure pointer
 * @param p_mmc_req :  mmc_request structure pointer
 ----------------------------------------------------------------------------------------------------------*/
static void ms_sdmmc_request(struct mmc_host *p_mmc_host, struct mmc_request *p_mmc_req)
{
    struct ms_sdmmc_slot   *p_sdmmc_slot  = mmc_priv(p_mmc_host);
    struct mmc_command *cmd = p_mmc_req->cmd;
    struct mmc_command *stop = p_mmc_req->stop;
    struct mmc_data *data = p_mmc_req->data;

    RspStruct * eRspSt;
    RspErrEmType eErr = EV_STS_OK;
    CmdEmType eCmdType = EV_CMDRSP;
    TransEmType eTransType = ((EN_SDMMC_ADV_DMA) && (EN_SDMMC_BRO_DMA) ? EV_ADMA : EV_DMA );
    SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;
    IPEmType eIP = ge_IPSlot[eSlot];
    volatile U32_T au32Addr[MAX_SEG_CNT];

    BOOL_T bCloseClock = FALSE;
    U8_T u8CMD = 0;
    U16_T u16BlkSize = 0, u16BlkCnt = 0, u16SubBlkCnt = 0;
    U16_T u16ProcCnt = 0, u16Idx= 0;
    U32_T u32Arg = 0, u32SubLen = 0;


    _CRIT_SECT_BEGIN(eSlot);

    u8CMD =(U8_T)cmd->opcode;
    u32Arg = (U32_T)cmd->arg;

    if(Hal_SDMMC_OtherPreUse(eIP) || gb_AlwaysSwitchPAD)
        _SwitchPAD(eSlot);

    if(!p_sdmmc_slot->mmc->card)
       Hal_SDMMC_StopProcessCtrl(eIP, FALSE);

#if (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))
    clk_set_rate(gp_clkSlot[eSlot], p_sdmmc_slot->currRealClk);
    //Hal_SDPLT_SetClock(eIP, p_sdmmc_slot->currRealClk, FALSE);
#else
    Hal_SDPLT_SetClock(eIP, p_sdmmc_slot->currRealClk, FALSE);
#endif

    pr_sdio_main("_[%01X]_", Hal_SDMMC_GetDATBusLevel(eIP));
    pr_sd_main(">> [sdmmc_%u] CMD_%u (0x%08X)", eSlot, u8CMD, u32Arg);

    /***** Specail Case for FCIE4 SDIO ******/
    _CRIT_SetMIEIntEn_ForV4(eSlot, (!data ? EV_EMP : EV_DMA));

    Hal_SDMMC_SetCmdToken(eIP, u8CMD, u32Arg);
    /****** Simple SD command *******/
    if(!data)
    {
        Hal_SDMMC_SetSDIOIntBeginSetting(eIP, u8CMD, u32Arg, EV_CMDRSP, 0);
        eErr = Hal_SDMMC_SendCmdAndWaitProcess(eIP, EV_EMP, EV_CMDRSP, _TransRspType(mmc_resp_type(cmd)), TRUE);
        Hal_SDMMC_SetSDIOIntEndSetting(eIP, eErr, 0);
    }
    else // R/W SD Command
    {
        u16BlkSize = (U16_T)data->blksz;
        u16BlkCnt  = (U16_T)data->blocks;
        u32SubLen = (U32_T) data->sg[0].length;
        u16SubBlkCnt = (U16_T)(u32SubLen/u16BlkSize);

        eCmdType = ( (data->flags & MMC_DATA_READ) ? EV_CMDREAD : EV_CMDWRITE );
        bCloseClock = ( (stop) ? FALSE : TRUE );

        pr_sd_main("__[Sgl: %u] (TB: %u)(BSz: %u)", (U16_T)data->sg_len, u16BlkCnt, u16BlkSize);

        u16ProcCnt = _PreDataBufferProcess(eTransType, data, p_sdmmc_slot, au32Addr);
        if(u16ProcCnt==0)
        {
            pr_err("\n>> [sdmmc_%u] Err: DMA Mapping Addr Error!\n", eSlot);
            eErr = EV_OTHER_ERR;
            goto LABEL_SD_ERR;
        }
        else if(u16ProcCnt==1)
        {
            u32SubLen = u16BlkSize * u16BlkCnt;
            u16SubBlkCnt = u16BlkCnt;
        }

        pr_sd_dbg("\n____[0] =>> (SBCnt: %u)__[Addr: 0x%08X]", u16SubBlkCnt, au32Addr[0]);

        Hal_SDMMC_TransCmdSetting(eIP, eTransType, u16SubBlkCnt, u16BlkSize, Hal_SDPLT_TransMIUAddr(au32Addr[0]), (volatile U8_T*)au32Addr[0]);
        Hal_SDMMC_SetSDIOIntBeginSetting(eIP, u8CMD, u32Arg, eCmdType, u16BlkCnt);
        eErr = Hal_SDMMC_SendCmdAndWaitProcess(eIP, eTransType, eCmdType, _TransRspType(mmc_resp_type(cmd)), bCloseClock);


        if( ((U16_T)eErr) == EV_STS_OK )
        {
            data->bytes_xfered += u32SubLen;

            /****** Broken DMA *******/
            for(u16Idx=1 ; u16Idx<u16ProcCnt; u16Idx++)
            {
                u32SubLen = (U32_T) data->sg[u16Idx].length;
                u16SubBlkCnt = (U16_T)(u32SubLen/u16BlkSize);
                pr_sd_dbg("\n____[%u] =>> (SBCnt: %u)__[Addr: 0x%08X]", u16Idx, u16SubBlkCnt, au32Addr[u16Idx]);

                Hal_SDMMC_TransCmdSetting(eIP, eTransType, u16SubBlkCnt, u16BlkSize, Hal_SDPLT_TransMIUAddr(au32Addr[u16Idx]), (volatile U8_T *)au32Addr[u16Idx]);
                eErr = Hal_SDMMC_RunBrokenDmaAndWaitProcess(eIP, eCmdType);

                if((U16_T)eErr) break;
                data->bytes_xfered += u32SubLen;
            }
        }

        Hal_SDMMC_SetSDIOIntEndSetting(eIP, eErr, u16BlkCnt);

        _PostDataBufferProcess(eTransType, data, p_sdmmc_slot);

    }


LABEL_SD_ERR:

    cmd->error = _RequestEndProcess(eCmdType, eErr, p_sdmmc_slot, data);

    if(data)
        data->error = cmd->error;

    eRspSt = Hal_SDMMC_GetRspToken(eIP);
    cmd->resp[0] = _TransArrToUInt(eRspSt->u8ArrRspToken[1], eRspSt->u8ArrRspToken[2], eRspSt->u8ArrRspToken[3], eRspSt->u8ArrRspToken[4]);
    if(eRspSt->u8RspSize == 0x10)
    {
        cmd->resp[1] = _TransArrToUInt(eRspSt->u8ArrRspToken[5], eRspSt->u8ArrRspToken[6], eRspSt->u8ArrRspToken[7], eRspSt->u8ArrRspToken[8]);
        cmd->resp[2] = _TransArrToUInt(eRspSt->u8ArrRspToken[9], eRspSt->u8ArrRspToken[10], eRspSt->u8ArrRspToken[11], eRspSt->u8ArrRspToken[12]);
        cmd->resp[3] = _TransArrToUInt(eRspSt->u8ArrRspToken[13], eRspSt->u8ArrRspToken[14], eRspSt->u8ArrRspToken[15], 0);
    }

    /****** Print Error Message******/
    if(!data && cmd->error && !_PassPrintCMD(eSlot, u8CMD, u32Arg, (BOOL_T)p_sdmmc_slot->sdioFlag)) //Cmd Err but Pass Print Some Cmds
    {
        if(cmd->error == -EILSEQ)
            pr_sd_err(">> [sdmmc_%u] Warn: #Cmd_%u (0x%08X)=>(E: 0x%04X)(S: 0x%08X)__(L:%u)\n", eSlot, u8CMD, u32Arg, (U16_T)eErr, cmd->resp[0], eRspSt->u32ErrLine);
        else
            pr_sd_err(">> [sdmmc_%u] Err: #Cmd_%u (0x%08X)=>(E: 0x%04X)(S: 0x%08X)__(L:%u)\n", eSlot, u8CMD, u32Arg, (U16_T)eErr, cmd->resp[0], eRspSt->u32ErrLine);
    }
    else if(data && data->error && !_PassPrintCMD(eSlot, u8CMD, u32Arg, (BOOL_T)p_sdmmc_slot->sdioFlag)) //Data Err
    {
        if(data->error == -EILSEQ)
            pr_sd_err(">> [sdmmc_%u] Warn: #Cmd_%u (0x%08X)=>(E: 0x%04X)(S: 0x%08X)__(L:%u)(B:%u/%u)(I:%u/%u)\n", \
                  eSlot, u8CMD, u32Arg, (U16_T)eErr, cmd->resp[0], eRspSt->u32ErrLine, u16SubBlkCnt, u16BlkCnt, u16Idx, u16ProcCnt);
        else
            pr_sd_err(">> [sdmmc_%u] Err: #Cmd_%u (0x%08X)=>(E: 0x%04X)(S: 0x%08X)__(L:%u)(B:%u/%u)(I:%u/%u)\n", \
                  eSlot, u8CMD, u32Arg, (U16_T)eErr, cmd->resp[0], eRspSt->u32ErrLine, u16SubBlkCnt, u16BlkCnt, u16Idx, u16ProcCnt);
    }

    /****** Send Stop Cmd ******/
    if(stop)
    {
        u8CMD = (U8_T)stop->opcode;
        u32Arg = (U32_T)stop->arg;
        pr_sd_main(">> [sdmmc_%u]_CMD_%u (0x%08X)", eSlot, u8CMD, u32Arg);

        /***** Specail Case for FCIE4 SDIO ******/
        _CRIT_SetMIEIntEn_ForV4(eSlot, (!data ? EV_EMP : EV_DMA));

        Hal_SDMMC_SetCmdToken(eIP, u8CMD, u32Arg);
        Hal_SDMMC_SetSDIOIntBeginSetting(eIP, u8CMD, u32Arg, EV_CMDRSP, 0);
        eErr = Hal_SDMMC_SendCmdAndWaitProcess(eIP, EV_EMP, EV_CMDRSP, _TransRspType(mmc_resp_type(stop)), TRUE);
        Hal_SDMMC_SetSDIOIntEndSetting(eIP, eErr, 0);

        stop->error = _RequestEndProcess(EV_CMDRSP, eErr, p_sdmmc_slot, data);

        eRspSt = Hal_SDMMC_GetRspToken(eIP);
        stop->resp[0] = _TransArrToUInt(eRspSt->u8ArrRspToken[1], eRspSt->u8ArrRspToken[2], eRspSt->u8ArrRspToken[3], eRspSt->u8ArrRspToken[4]);

        if(stop->error)
            pr_sd_err(">> [sdmmc_%u] Err: #Cmd_12 => (E: 0x%04X)(S: 0x%08X)__(L:%u)\n", eSlot, (U16_T)eErr, stop->resp[0], eRspSt->u32ErrLine);


    }

    //Hal_SDPLT_SetClock(eIP, p_sdmmc_slot->pmrsaveClk); // For Power Saving

    _CRIT_SECT_END(eSlot);

    mmc_request_done(p_mmc_host, p_mmc_req);


}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_set_ios
 *     @author jeremy.wang (2011/5/19)
 * Desc: Set IO bus Behavior
 *
 * @param p_mmc_host : mmc_host structure pointer
 * @param p_mmc_ios :  mmc_ios  structure pointer
 ----------------------------------------------------------------------------------------------------------*/
static void ms_sdmmc_set_ios(struct mmc_host *p_mmc_host, struct mmc_ios *p_mmc_ios)
{
    struct ms_sdmmc_slot *p_sdmmc_slot = mmc_priv(p_mmc_host);
    SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;

    _CRIT_SECT_BEGIN(eSlot);

    _SwitchPAD(eSlot);

    /****** Bus Timing Setting*******/
    if( (p_sdmmc_slot->currTiming != p_mmc_ios->timing) || !p_sdmmc_slot->initFlag)
    {
        p_sdmmc_slot->currTiming = p_mmc_ios->timing;
        _SetBusTiming(eSlot, p_sdmmc_slot->currTiming);
        pr_sd_main(">> [sdmmc_%u] Set IOS => BusTiming=%u\n", eSlot, p_sdmmc_slot->currTiming);
    }


    /****** Clock Setting*******/
    if(p_sdmmc_slot->currClk != p_mmc_ios->clock)
    {
        p_sdmmc_slot->currDownLevel = 0;
        p_sdmmc_slot->currClk = p_mmc_ios->clock;
        p_sdmmc_slot->currRealClk = _SetClock(eSlot, p_sdmmc_slot->currClk, 0, p_sdmmc_slot->currDownLevel);

        if( (p_sdmmc_slot->currRealClk==0) && (p_sdmmc_slot->currClk!=0) )
            pr_sd(">> [sdmmc_%u] Set IOS => Clk=Error\n", eSlot);
        else if(p_sdmmc_slot->currRealClk <= 400000)
            _SetBusTiming(eSlot, 0xFF);
        else
            pr_sd(">> [sdmmc_%u] Set IOS => Clk=%u (Real=%u)\n", eSlot, p_sdmmc_slot->currClk, p_sdmmc_slot->currRealClk);
    }

    /****** Power Switch Setting *******/
    if(p_sdmmc_slot->currPowrMode != p_mmc_ios->power_mode)
    {
        p_sdmmc_slot->currPowrMode = p_mmc_ios->power_mode;
        pr_sd_main(">> [sdmmc_%u] Set IOS => Power=%u\n", eSlot, p_sdmmc_slot->currPowrMode);
        _SetPower(eSlot, p_sdmmc_slot->currPowrMode);

        if(p_sdmmc_slot->currPowrMode == MMC_POWER_OFF)
        {
            p_sdmmc_slot->initFlag = 0;
            p_sdmmc_slot->sdioFlag = 0;
        }
    }

    /****** Bus Width Setting*******/
    if( (p_sdmmc_slot->currWidth != p_mmc_ios->bus_width) || !p_sdmmc_slot->initFlag)
    {
        p_sdmmc_slot->currWidth = p_mmc_ios->bus_width;
        _SetBusWidth(eSlot, p_sdmmc_slot->currWidth);
        pr_sd_main(">> [sdmmc_%u] Set IOS => BusWidth=%u\n", eSlot, p_sdmmc_slot->currWidth);
    }


    /****** Voltage Setting *******/
    if( (p_sdmmc_slot->currVdd != p_mmc_ios->signal_voltage) || !p_sdmmc_slot->initFlag)
    {
        p_sdmmc_slot->currVdd = p_mmc_ios->signal_voltage;  //just record value to currVdd
        //pr_sd_main(">> [sdmmc_%u] Set IOS => Voltage=%u\n", eSlot, p_sdmmc_slot->currVdd);
    }

    p_sdmmc_slot->initFlag = 1;

    _CRIT_SECT_END(eSlot);


}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_get_ro
 *     @author jeremy.wang (2011/5/19)
 * Desc:  Get SD card read/write permission
 *
 * @param p_mmc_host : mmc_host structure pointer
 *
 * @return int  :  1 = read-only, 0 = read-write.
 ----------------------------------------------------------------------------------------------------------*/
static int ms_sdmmc_get_ro(struct mmc_host *p_mmc_host)
{
    struct ms_sdmmc_slot   *p_sdmmc_slot  = mmc_priv(p_mmc_host);
    SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;

    _CRIT_SECT_BEGIN(eSlot);

    //_SwitchPAD(eSlot);

    if( _GetWriteProtect(eSlot) )    //For CB2 HW Circuit, WP=>NWP
        p_sdmmc_slot->read_only = 1;
    else
        p_sdmmc_slot->read_only = 0;

    _CRIT_SECT_END(eSlot);

    pr_sd_main(">> [sdmmc_%u] Get RO => (%d)\n", eSlot, p_sdmmc_slot->read_only);

    return p_sdmmc_slot->read_only;
}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_get_cd
 *     @author jeremy.wang (2011/6/17)
 * Desc: Get SD card detection status
 *
 * @param p_mmc_host : mmc_host structure pointer
 *
 * @return int  :  1 = Present
 ----------------------------------------------------------------------------------------------------------*/
static int ms_sdmmc_get_cd(struct mmc_host *p_mmc_host)
{
    struct ms_sdmmc_slot   *p_sdmmc_slot  = mmc_priv(p_mmc_host);
    SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;


    if( _GetCardDetect(eSlot) )
        p_sdmmc_slot->card_det = 1;
    else
        p_sdmmc_slot->card_det  = 0;

    if(!EN_SDMMC_POLLING_CDZ)
        pr_sd(">> [sdmmc_%u] Get CD => (%d)\n", eSlot, p_sdmmc_slot->card_det);

    return p_sdmmc_slot->card_det;
}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_init_card
 *     @author jeremy.wang (2012/2/20)
 * Desc:
 *
 * @param p_mmc_host :
 * @param p_mmc_card :
 ----------------------------------------------------------------------------------------------------------*/
static void ms_sdmmc_init_card(struct mmc_host *p_mmc_host, struct mmc_card *p_mmc_card)
{
    struct ms_sdmmc_slot   *p_sdmmc_slot  = mmc_priv(p_mmc_host);
    SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;
    IPEmType eIP     = ge_IPSlot[eSlot];


    if( (p_mmc_card->type == MMC_TYPE_SDIO) || (p_mmc_card->type == MMC_TYPE_SDIO))
    {
    Hal_SDMMC_SDIODeviceCtrl(eIP, TRUE);
    p_sdmmc_slot->sdioFlag = 1;
        pr_sd(">> [sdmmc_%u] Found SDIO Device!\n", eSlot);

    }


}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_enable_sdio_irq
 *     @author jeremy.wang (2012/2/20)
 * Desc:
 *
 * @param p_mmc_host :
 * @param enable :
 ----------------------------------------------------------------------------------------------------------*/
static void ms_sdmmc_enable_sdio_irq(struct mmc_host *p_mmc_host, int enable)
{
    struct ms_sdmmc_slot   *p_sdmmc_slot  = mmc_priv(p_mmc_host);
    SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;
    IPEmType eIP     = ge_IPSlot[eSlot];

#if (D_FCIE_M_VER == D_FCIE_M_VER__04)
    unsigned long flags;
    spin_lock_irqsave(&g_RegLockSlot[eSlot], flags);
#endif

    Hal_SDMMC_SDIOIntDetCtrl(eIP, (BOOL_T)enable);

#if (D_FCIE_M_VER == D_FCIE_M_VER__04)
    spin_unlock_irqrestore(&g_RegLockSlot[eSlot], flags);
#endif

    if(enable)
    {
        pr_sdio_main(">> [sdmmc_%u] =========> SDIO IRQ EN=> (%d)\n", eSlot, enable);
    }

}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_switch_busvdd
 *     @author jeremy.wang (2018/1/8)
 * Desc:
 *
 * @param p_mmc_host :
 * @param p_mmc_ios :
 *
 * @return int  :
 ----------------------------------------------------------------------------------------------------------*/
static int ms_sdmmc_switch_busvdd(struct mmc_host *p_mmc_host, struct mmc_ios *p_mmc_ios)
{
    struct ms_sdmmc_slot   *p_sdmmc_slot  = mmc_priv(p_mmc_host);
    SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;

    pr_sd_main(">> [sdmmc_%u] Switch BusVdd (%u)\n", eSlot, p_mmc_ios->signal_voltage);

    if(_SetBusVdd(eSlot, p_mmc_ios->signal_voltage))
    {
        pr_err(">> [sdmmc_%u] Err: Single Volt (%u) doesn't ready!\n", eSlot, p_mmc_ios->signal_voltage);
        return 1;
    }

    return 0;
}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_card_busy
 *     @author jeremy.wang (2018/1/10)
 * Desc:
 *
 * @param p_mmc_host :
 *
 * @return int  :
 ----------------------------------------------------------------------------------------------------------*/
static int ms_sdmmc_card_busy(struct mmc_host *p_mmc_host)
{
    struct ms_sdmmc_slot   *p_sdmmc_slot  = mmc_priv(p_mmc_host);
    SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;
    IPEmType eIP     = ge_IPSlot[eSlot];

    return !Hal_SDMMC_GetDATBusLevel(eIP);  //Level 0 -> busy 1, Level 1 -> busy 0

}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_exec_tuning
 *     @author jeremy.wang (2018/1/9)
 * Desc:
 *
 * @param p_mmc_host :
 * @param opcode :
 *
 * @return int  :
 ----------------------------------------------------------------------------------------------------------*/
static int ms_sdmmc_exec_tuning(struct mmc_host *p_mmc_host, u32 opcode)
{
    struct ms_sdmmc_slot   *p_sdmmc_slot  = mmc_priv(p_mmc_host);
    SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;
    IPEmType eIP     = ge_IPSlot[eSlot];
    unsigned char u8BusTiming = p_mmc_host->ios.timing;
    unsigned char u8Phase = 0;
    char s8retPhase = 0;


    //Clean All Pass Phase
    Hal_SD_SavePassPhase(eIP, u8Phase, TRUE);

    //Scan SDR Phase
    if( (u8BusTiming == MMC_TIMING_UHS_SDR50) || (u8BusTiming == MMC_TIMING_UHS_SDR104) )
    {
        for(u8Phase = 0; u8Phase < 18; u8Phase++)
        {
            Hal_SD_SetPhase(eIP, EV_SD30_SDR, u8Phase);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 19, 0))
            if(!mmc_send_tuning(p_mmc_host))
#else
            if(!mmc_send_tuning(p_mmc_host, opcode, NULL))
#endif
            {
                if(Hal_SD_SavePassPhase(eIP, u8Phase, FALSE) )
                {
                    pr_err(">> [sdmmc_%u] Err: Operation over MAX_PHASE (1)!\n", eSlot);
                    return 1;
                }

                pr_sd_main(">> [sdmmc_%u] SDR Tuning ...... Good Phase (%u)\n", eSlot, u8Phase);
            }
        }

        s8retPhase = Hal_SD_FindFitPhaseSetting(eIP, 17);

        pr_sd(">> [sdmmc_%u] Exc Tuning => Sel SDR Phase (%d)\n", eSlot, s8retPhase);

        if(s8retPhase<0)
        {
            pr_err(">> [sdmmc_%u] Err: Operation over MAX_PHASE (2)!\n", eSlot);
            return 1;
        }

        Hal_SD_SetPhase(eIP, EV_SD30_SDR, (U8_T)s8retPhase);

        pr_sd(">> [sdmmc_%u] ", eSlot);
        Hal_SD_GoodPhases_Dump(eIP);
        pr_sd("\n");

    } //Scan DDR Phase
    else if (u8BusTiming == MMC_TIMING_UHS_DDR50)
    {
        for(u8Phase = 0; u8Phase < 7; u8Phase++)
        {
            Hal_SD_SetPhase(eIP, EV_SD30_DDR, u8Phase);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 19, 0))
            if(!mmc_send_tuning(p_mmc_host))
#else
            if(!mmc_send_tuning(p_mmc_host, opcode, NULL))
#endif
            {
                if(Hal_SD_SavePassPhase(eIP, u8Phase, FALSE) )
                {
                    pr_err(">> [sdmmc_%u] Err: Operation over MAX_PHASE (1)!\n", eSlot);
                    return 1;
                }

                pr_sd_main(">> [sdmmc_%u] DDR Tuning ...... Good Phase (%u)\n", eSlot, u8Phase);
            }


        }

        s8retPhase = Hal_SD_FindFitPhaseSetting(eIP, 6);

        pr_sd(">> [sdmmc_%u] Exc Tuning => Sel DDR Phase (%d)\n", eSlot, s8retPhase);

        if(s8retPhase<0)
        {
            pr_err(">> [sdmmc_%u] Err: Operation over MAX_PHASE (2)!\n", eSlot);
            return 1;
        }


        Hal_SD_SetPhase(eIP, EV_SD30_DDR, (U8_T)s8retPhase);

        pr_sd(">> [sdmmc_%u] ", eSlot);
        Hal_SD_GoodPhases_Dump(eIP);
        pr_sd("\n");

    }

    return 0;

}


/**********************************************************************************************************
 * Define Static Global Structs
 **********************************************************************************************************/
/*----------------------------------------------------------------------------------------------------------
 *  st_mmc_ops
 ----------------------------------------------------------------------------------------------------------*/
static const struct mmc_host_ops st_mmc_ops =
{

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 1, 0))
    .enable          = ms_sdmmc_enable,
    .disable         = ms_sdmmc_disable,
#endif

    .request                     = ms_sdmmc_request,
    .set_ios                     = ms_sdmmc_set_ios,
    .get_ro                      = ms_sdmmc_get_ro,
    .get_cd                      = ms_sdmmc_get_cd,
    .init_card                   = ms_sdmmc_init_card,
    .enable_sdio_irq             = ms_sdmmc_enable_sdio_irq,
    .start_signal_voltage_switch = ms_sdmmc_switch_busvdd,
    .card_busy                   = ms_sdmmc_card_busy,
    .execute_tuning              = ms_sdmmc_exec_tuning,

};



#if (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_dts_init
 *     @author jeremy.wang (2017/3/24)
 * Desc: Device Tree Init
 *
 * @param p_dev : platform device
 ----------------------------------------------------------------------------------------------------------*/
static int ms_sdmmc_dts_init(struct platform_device *p_dev)
{
    U8_T  slotNo, ipidx = 0;
    SlotEmType eSlot;
    U32_T u32_IPOrderSlot[3];
    U32_T u32_PADOrderSlot[3];
    U32_T u32_IntCDZSlot[3];
    U32_T u32_FakeCDZSlot[3];
    U32_T u32_SlotNums = 0;
    U32_T u32_ReverseCDZ = 0;

    if(of_property_read_u32(p_dev->dev.of_node, "slotnum" , &u32_SlotNums))
    {
       pr_err(">> [sdmmc] Err: Could not get dts [slotnum] option!\n");
        return 1;
    }

    if(of_property_read_u32(p_dev->dev.of_node, "revcdz" , &u32_ReverseCDZ))
    {
        pr_err(">> [sdmmc] Err: Could not get dts [revcdz] option!\n");
        return 1;
    }

    if(of_property_read_u32_array(p_dev->dev.of_node, "slot-ip-orders", (U32_T*)u32_IPOrderSlot, 3))
    {
        pr_err(">> [sdmmc] Err: Could not get dts [slot-ip-orders] option!\n");
        return 1;
    }

    if(of_property_read_u32_array(p_dev->dev.of_node, "slot-pad-orders", (U32_T*)u32_PADOrderSlot, 3))
    {
        pr_err(">> [sdmmc] Err: Could not get dts [slot-pad-orders] option!\n");
        return 1;
    }

    if(of_property_read_u32_array(p_dev->dev.of_node, "slot-fakecdzs", (U32_T*)u32_FakeCDZSlot, 3))
    {
        pr_err(">> [sdmmc] Err: Could not get dts [slot-fakecdzs] option!\n");
        return 1;
    }

    if(of_property_read_u32_array(p_dev->dev.of_node, "slot-intcdzs", (U32_T*)u32_IntCDZSlot, 3))
    {
        pr_err(">> [sdmmc] Err: Could not get dts [slot-intcdzs] option!\n");
        return 1;
    }

    if(of_property_read_u32_array(p_dev->dev.of_node, "slot-max-clks", (U32_T*)gu32_MaxClkSlot, 3))
    {
        pr_err(">> [sdmmc] Err: Could not get dts [slot-max-clks] option!\n");
        return 1;
    }

    if(of_property_read_u32_array(p_dev->dev.of_node, "slot-pwr-gpios", (U32_T *)gu32_PwrNoSlot, 3))
    {
        pr_err(">> [sdmmc] Err: Could not get dts [slot-pwr-gpios] option!\n");
        return 1;
    }


    //Debug
    /*
    pr_err(">> [sdmmc] SlotNums= %u\n", u32_SlotNums);
    pr_err(">> [sdmmc] SlotIPs[0-2]= %u, %u, %u \n", u32_IPOrderSlot[0], u32_IPOrderSlot[1], u32_IPOrderSlot[2]);
    pr_err(">> [sdmmc] SlotPADs[0-2]= %u, %u, %u \n", u32_PADOrderSlot[0], u32_PADOrderSlot[1], u32_PADOrderSlot[2]);
    pr_err(">> [sdmmc] SlotMaxClk[0-2]= %u, %u, %u \n", gu32_MaxClkSlot[0], gu32_MaxClkSlot[1], gu32_MaxClkSlot[2]);
    pr_err(">> [sdmmc] SlotFakeCDZ[0-2]= %u, %u, %u \n", u32_FakeCDZSlot[0], u32_FakeCDZSlot[1], u32_FakeCDZSlot[2]);
    pr_err(">> [sdmmc] SlotHotplug[0-2]= %u, %u, %u \n", u32_HotplugSlot[0], u32_HotplugSlot[1], u32_HotplugSlot[2]);
    pr_err(">> [sdmmc] RevCDZ= %u\n", u32_ReverseCDZ);
    */

    gu8_SlotNums = (U8_T)u32_SlotNums;
    gb_ReverseCDZ = (BOOL_T)u32_ReverseCDZ;

    for(slotNo =0; slotNo<3; slotNo++)
    {
        eSlot = (SlotEmType)slotNo;

        gu8_IPOrderSlot[eSlot] = (U8_T)u32_IPOrderSlot[eSlot];
        gu8_PADOrderSlot[eSlot] = (U8_T)u32_PADOrderSlot[eSlot];
        gb_FakeCDZSlot[eSlot] = (BOOL_T)u32_FakeCDZSlot[eSlot];
        gb_IntCDZSlot[eSlot] = (BOOL_T)u32_IntCDZSlot[eSlot];

    }

    //Mark for I2: Don't need use DTS Clock
    /*gp_clkSDSync = of_clk_get(p_dev->dev.of_node, 0);

    if(IS_ERR(gp_clkSDSync))
    {
        pr_err(">> [sdmmc_%u] Err: Failed to get dts clock tree!\n", slotNo);
        return 1;
    }
    */

    for(slotNo =0; slotNo<gu8_SlotNums; slotNo++)
    {
        eSlot = (SlotEmType)slotNo;
        ipidx   = gu8_IPOrderSlot[eSlot];

        /****** Get Device Tree Information******/
        //Mark for I2: Don't need use DTS Clock
        //gp_clkSlot[eSlot] = of_clk_get(p_dev->dev.of_node, (1 + ipidx));
        //gp_clkMCMSlot[eSlot] = of_clk_get(p_dev->dev.of_node, (ipidx*3) + 1);
        //gp_clkSRAMSlot[eSlot] = of_clk_get(p_dev->dev.of_node, (ipidx*3) + 2);
        //if(IS_ERR(gp_clkSlot[eSlot]) /*|| IS_ERR(gp_clkMCMSlot[eSlot]) || IS_ERR(gp_clkSRAMSlot[eSlot])*/ )
        //{
            //pr_err(">> [sdmmc_%u] Err: Failed to get dts clock!\n", slotNo);
            //return 1;
        //}

        gu16_MieIntNoSlot[eSlot] = irq_of_parse_and_map(p_dev->dev.of_node, ipidx);


        if(!gu16_MieIntNoSlot[eSlot])
        {
            pr_err(">> [sdmmc_%u] Err: Failed to get dts interrupt!\n", slotNo);
            return 1;
        }

    }

    return 0;

}

#endif


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_init_slot
 *     @author jeremy.wang (2015/12/9)
 * Desc: Init Slot Setting
 *
 * @param slotNo : Slot Number
 * @param p_sdmmc_host : ms_sdmmc_host
 *
 * @return int  : Error Status; Return 0 if no error
 ----------------------------------------------------------------------------------------------------------*/
static int ms_sdmmc_init_slot(unsigned int slotNo, struct ms_sdmmc_host *p_sdmmc_host)
{
    struct ms_sdmmc_slot    *p_sdmmc_slot;
    struct mmc_host         *p_mmc_host;
    SlotEmType eSlot = (SlotEmType)slotNo;
    IPEmType eIP     = ge_IPSlot[eSlot];
    PADEmType ePAD   = ge_PADSlot[eSlot];
    PortEmType ePort  = ge_PORTSlot[eSlot];


    int nRet = 0;

    /****** (1) Allocte MMC and SDMMC host ******/
    p_mmc_host = mmc_alloc_host(sizeof(struct ms_sdmmc_slot), &p_sdmmc_host->pdev->dev);

    if (!p_mmc_host)
    {
        pr_err(">> [sdmmc_%u] Err: Failed to Allocate mmc_host!\n", slotNo);
        return -ENOMEM;
    }

    /****** (2) SDMMC host setting ******/
    p_sdmmc_slot               = mmc_priv(p_mmc_host);

#if (!EN_SDMMC_BRO_DMA)

    p_sdmmc_slot->dma_buffer   = dma_alloc_coherent(NULL, MAX_BLK_COUNT * MAX_BLK_SIZE, &p_sdmmc_slot->dma_phy_addr, GFP_KERNEL);
    if (!p_sdmmc_slot->dma_buffer)
    {
        pr_err(">> [sdmmc_%u] Err: Failed to Allocate sdmmc_host DMA buffer!\n", slotNo);
        return -ENOMEM;
    }

#elif (EN_SDMMC_ADV_DMA)

    p_sdmmc_slot->adma_buffer   = dma_alloc_coherent(NULL, sizeof(AdmaDescStruct) * MAX_SEG_CNT, &p_sdmmc_slot->adma_phy_addr, GFP_KERNEL);
    if (!p_sdmmc_slot->adma_buffer)
    {
        pr_err(">> [sdmmc_%u] Err: Failed to Allocate sdmmc_host ADMA buffer!\n", slotNo);
        return -ENOMEM;
    }

#endif

    p_sdmmc_slot->mmc          = p_mmc_host;
    p_sdmmc_slot->slotNo       = slotNo;
    p_sdmmc_slot->pmrsaveClk   = Hal_SDPLT_FindClockSetting(eIP, 400000, 0, 0);
    p_sdmmc_slot->mieIRQNo     = gu16_MieIntNoSlot[eSlot];
    p_sdmmc_slot->cdzIRQNo     = gu16_CdzIntNoSlot[eSlot];
    p_sdmmc_slot->cdzGPIONo    = Hal_SDPLT_CDZGetGPIONum((GPIOEmType)slotNo, ePAD);
    p_sdmmc_slot->pwrGPIONo    = gu32_PwrNoSlot[eSlot];
    p_sdmmc_slot->initFlag     = 0;
    p_sdmmc_slot->sdioFlag     = 0;

    p_sdmmc_slot->currClk      = 0;
    p_sdmmc_slot->currWidth    = 0;
    p_sdmmc_slot->currTiming   = 0;
    p_sdmmc_slot->currPowrMode = 0;
    p_sdmmc_slot->currVdd      = 0;
    p_sdmmc_slot->currDDR      = 0;


    /***** (3) MMC host setting ******/
    p_mmc_host->ops = &st_mmc_ops;
    p_mmc_host->f_min = p_sdmmc_slot->pmrsaveClk;
    p_mmc_host->f_max = gu32_MaxClkSlot[eSlot];

    p_mmc_host->ocr_avail = MMC_VDD_32_33|MMC_VDD_31_32|MMC_VDD_30_31|MMC_VDD_29_30|MMC_VDD_28_29|MMC_VDD_27_28|MMC_VDD_165_195;
    p_mmc_host->caps = MMC_CAP_4_BIT_DATA|MMC_CAP_MMC_HIGHSPEED|MMC_CAP_SD_HIGHSPEED|MMC_CAP_NONREMOVABLE;

    if(!gb_IntCDZSlot[eSlot] && EN_SDMMC_POLLING_CDZ)
        p_mmc_host->caps |= MMC_CAP_NEEDS_POLL;

    if(ePort == EV_PFCIE5_SDIO_PLL)
        p_mmc_host->caps |= MMC_CAP_UHS_SDR50 /*| MMC_CAP_UHS_SDR104 | MMC_CAP_UHS_DDR50*/;


#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 4, 0))
    p_mmc_host->caps |= MMC_CAP_DISABLE;
#endif

#if (EN_SDMMC_BRO_DMA)
    p_mmc_host->max_blk_count  = MAX_BRO_BLK_COUNT;
#else
    p_mmc_host->max_blk_count  = MAX_BLK_COUNT;
#endif
    p_mmc_host->max_blk_size   = MAX_BLK_SIZE;

    p_mmc_host->max_req_size   = p_mmc_host->max_blk_count  * p_mmc_host->max_blk_size;
    p_mmc_host->max_seg_size   = p_mmc_host->max_req_size;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 39))
    p_mmc_host->max_segs       = MAX_SEG_CNT;
#else
    p_mmc_host->max_phys_segs  = MAX_SEG_CNT;
    p_mmc_host->max_hw_segs    = MAX_SEG_CNT;
#endif

    p_sdmmc_host->sdmmc_slot[slotNo] = p_sdmmc_slot;

    /****** (4) IP Once Setting for Different Platform ******/
    Hal_SDPLT_InitIPOnce(eIP);

    /****** (5) Init GPIO Setting ******/
    Hal_SDPLT_CDZInitGPIO((GPIOEmType)slotNo, ePAD, !gb_FakeCDZSlot[eSlot]);

    if(EN_SDMMC_CHG_PWR_PIN)
    {
        nRet = gpio_request(p_sdmmc_slot->pwrGPIONo, "SD Power Pin");

        if(nRet > 0 )
            pr_sd(">> [sdmmc_%u] PWR Ctrl use Dyn GPIO: (%u)\n", slotNo, p_sdmmc_slot->pwrGPIONo);
        else
        {
            pr_sd(">> [sdmmc_%u] Err: Faild to request PWR GPIO (%u)\n", slotNo, p_sdmmc_slot->pwrGPIONo);
            gu32_PwrNoSlot[eSlot] = 0;
        }

    }


    /****** (6) Interrupt Source Setting ******/
    gst_IntSourceSlot[eSlot].slotNo = slotNo;
    gst_IntSourceSlot[eSlot].eIP = eIP;
    gst_IntSourceSlot[eSlot].eCardInt = EV_INT_SD;
    gst_IntSourceSlot[eSlot].p_data = p_sdmmc_slot;

    /*****  (7) Spinlock Init for Reg Protection ******/
#if (D_FCIE_M_VER == D_FCIE_M_VER__04)
    spin_lock_init(&g_RegLockSlot[slotNo]);
#endif

    /****** (8) Register IP IRQ *******/
    Hal_SDMMC_MIEIntCtrl(eIP, FALSE);
    if(EN_SDMMC_MIEINT_MODE)
    {
        nRet = request_irq(p_sdmmc_slot->mieIRQNo, Hal_CARD_INT_MIE, V_IP_MIEIRQ_PARA, DRIVER_NAME, &gst_IntSourceSlot[eSlot]);
        if (nRet)
        {
            pr_err(">> [sdmmc_%u] Err: Failed to request MIE Interrupt (%u)!\n", slotNo, p_sdmmc_slot->mieIRQNo);
            goto LABEL_OUT2;
        }
        else
            pr_sd(">> [sdmmc_%u] request MIE Interrupt Pass (%u)!\n", slotNo, p_sdmmc_slot->mieIRQNo);


        Hal_SDMMC_MIEIntCtrl(eIP, TRUE);

        if(gb_SDIOIRQSlot[eSlot])
        {
            p_mmc_host->caps |= MMC_CAP_SDIO_IRQ;
            pr_sd(">> [sdmmc_%u] Enable SDIO Interrupt Mode!\n", slotNo);

        }
    }

    mmc_add_host(p_mmc_host);

    if(!gb_IntCDZSlot[eSlot])
    {
        if(EN_SDMMC_POLLING_CDZ)
            pr_sd(">> [sdmmc_%u] Polling CDZ use Fake CDZ: (%u)\n", slotNo, gb_FakeCDZSlot[eSlot]);
        else
            pr_sd(">> [sdmmc_%u] Normal CDZ use Fake CDZ: (%u)\n", slotNo, gb_FakeCDZSlot[eSlot]);

        goto LABEL_OUT0;
    }


    /****** (9) Register Ext CDZ IRQ  *******/
    if(!p_sdmmc_slot->cdzIRQNo)
    {
        if(p_sdmmc_slot->cdzGPIONo && gpio_to_irq(p_sdmmc_slot->cdzGPIONo)> 0 )
            p_sdmmc_slot->cdzIRQNo = gpio_to_irq(p_sdmmc_slot->cdzGPIONo);
    }


    tasklet_init(&p_sdmmc_slot->hotplug_tasklet, ms_sdmmc_hotplug, (unsigned long)p_sdmmc_slot);

    if(p_sdmmc_slot->cdzIRQNo)
    {

        Hal_SDPLT_CDZSetGPIOIntAttr((GPIOEmType)slotNo, p_sdmmc_slot->cdzIRQNo, (_GetCardDetect(eSlot)? EV_GPIO_OPT3 : EV_GPIO_OPT4));
        Hal_SDPLT_CDZSetGPIOIntAttr((GPIOEmType)slotNo, p_sdmmc_slot->cdzIRQNo, EV_GPIO_OPT1);

        nRet = request_irq(p_sdmmc_slot->cdzIRQNo, ms_sdmmc_cdzint, gu32_CDZIRQParaSlot[eSlot], DRIVER_NAME, &gst_IntSourceSlot[eSlot]);
        if(nRet)
        {
            pr_err(">> [sdmmc_%u] Err: Failed to request CDZ Interrupt (%u)!\n", slotNo, p_sdmmc_slot->cdzIRQNo);
            goto LABEL_OUT1;
        }

        pr_sd(">> [sdmmc_%u] Int CDZ use Ext GPIO IRQ: (%u)\n", slotNo, p_sdmmc_slot->cdzIRQNo);

        Hal_SDPLT_CDZSetGPIOIntAttr((GPIOEmType)slotNo, p_sdmmc_slot->cdzIRQNo, EV_GPIO_OPT2);

        if(gb_WakeupCDZIRQSlot[eSlot])
            irq_set_irq_wake(p_sdmmc_slot->cdzIRQNo, TRUE);

    }
    else
    {
        pr_sd(">> [sdmmc_%u] CDZ IRQ doesn't exist!\n", slotNo);
        BUG_ON(1); //Coult not continue, it must be solved first (config issue)
    }


LABEL_OUT0:
    return 0;

LABEL_OUT1:
    tasklet_kill(&p_sdmmc_slot->hotplug_tasklet);
    free_irq(p_sdmmc_slot->mieIRQNo, &gst_IntSourceSlot[eSlot]);

    mmc_remove_host(p_mmc_host);
    mmc_free_host(p_mmc_host);

LABEL_OUT2:
#if (!EN_SDMMC_BRO_DMA)
    if (p_sdmmc_slot->dma_buffer)
        dma_free_coherent(NULL, MAX_BLK_COUNT * MAX_BLK_SIZE, p_sdmmc_slot->dma_buffer, p_sdmmc_slot->dma_phy_addr);
#endif

    return nRet;
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_probe
 *     @author jeremy.wang (2011/5/18)
 * Desc: Probe Platform Device
 *
 * @param p_dev : platform_device
 *
 * @return int : Error Status; Return 0 if no error.
 ----------------------------------------------------------------------------------------------------------*/
//struct ms_sdmmc_host *p_sdmmc_host;
static int ms_sdmmc_probe(struct platform_device *p_dev)
{
    struct ms_sdmmc_host *p_sdmmc_host;
    unsigned int slotNo = 0;
    int ret1 = 0, ret2 = 0, tret = 0;

    pr_sd(">> [sdmmc] ms_sdmmc_probe \n");

    p_sdmmc_host = kzalloc(sizeof(struct ms_sdmmc_host), GFP_KERNEL);

    if (!p_sdmmc_host)
    {
        pr_err(">> [sdmmc] Err: Failed to Allocate p_sdmmc_host!\n\n");
        return -ENOMEM;
    }

    p_sdmmc_host->pdev = p_dev;

    /***** device data setting ******/
    platform_set_drvdata(p_dev, p_sdmmc_host);

    /***** device PM wakeup setting ******/
    device_init_wakeup(&p_dev->dev, 1);


    /***** load DTS Solution ******/
#if (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))
    if(ms_sdmmc_dts_init(p_dev))
    {
        pr_err(">> [sdmmc] Err: Failed to use DTS solution!\n\n");
        return 1;
    }
    pr_sd(">> [sdmmc] Use DTS solution! \n");
#endif

    for(slotNo =0; slotNo<gu8_SlotNums; slotNo++)
    {
        _Dyn_IP_PAD_Mappingg((SlotEmType)slotNo);

        ret1 = _InitConfig((SlotEmType)slotNo);

        ret2 = ms_sdmmc_init_slot(slotNo, p_sdmmc_host);
        pr_sd(">> [sdmmc_%u] Probe Platform Devices...(Ret:%d) \n", slotNo, ret2);

        if(ret1 != 0)
            tret = ret1;
        else if(ret2 != 0)
            tret = ret2;

        }


    if(tret!=0)
        kfree(p_sdmmc_host);

    return tret;
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_remove_slot
 *     @author jeremy.wang (2015/12/9)
 * Desc: Remove Slot Setting
 *
 * @param slotNo : Slot Number
 * @param p_sdmmc_host : ms_sdmmc_host
 ----------------------------------------------------------------------------------------------------------*/
static void ms_sdmmc_remove_slot(unsigned int slotNo, struct ms_sdmmc_host *p_sdmmc_host)
{
    struct ms_sdmmc_slot *p_sdmmc_slot = p_sdmmc_host->sdmmc_slot[slotNo];
    struct mmc_host      *p_mmc_host = p_sdmmc_slot->mmc;
    SlotEmType eSlot = (SlotEmType)slotNo;

    mmc_remove_host(p_mmc_host);

    if (p_sdmmc_slot->dma_buffer)
        dma_free_coherent(NULL, MAX_BLK_COUNT*MAX_BLK_SIZE, p_sdmmc_slot->dma_buffer, p_sdmmc_slot->dma_phy_addr);

    if(EN_SDMMC_MIEINT_MODE)
        free_irq(p_sdmmc_slot->mieIRQNo, &gst_IntSourceSlot[eSlot]);

    mmc_free_host(p_mmc_host);

    if(gb_IntCDZSlot[eSlot])
    {
        tasklet_kill(&p_sdmmc_slot->hotplug_tasklet);
        if(p_sdmmc_slot->cdzIRQNo)
            free_irq(p_sdmmc_slot->cdzIRQNo, &gst_IntSourceSlot[eSlot]);
    }

}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_remove
 *     @author jeremy.wang (2011/5/18)
 * Desc: Revmoe MMC host
 *
 * @param p_dev :  platform device structure
 *
 * @return int  : Error Status; Return 0 if no error.
 ----------------------------------------------------------------------------------------------------------*/
static int ms_sdmmc_remove(struct platform_device *p_dev)
{
    struct ms_sdmmc_host *p_sdmmc_host = platform_get_drvdata(p_dev);
    unsigned int slotNo = 0;

    platform_set_drvdata(p_dev, NULL);

    for(slotNo=0; slotNo<gu8_SlotNums; slotNo++)
    {
        ms_sdmmc_remove_slot(slotNo, p_sdmmc_host);
        pr_sd(">> [sdmmc_%u] Remove devices...\n", slotNo);

    }

    return 0;
}


#if (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))

    #ifdef CONFIG_PM
    static int ms_sdmmc_devpm_prepare(struct device *dev)
    {
        return 0;
    }

    static void ms_sdmmc_devpm_complete(struct device *dev)
    {

    }

    static int ms_sdmmc_devpm_suspend(struct device *dev)
    {
        unsigned int slotNo = 0;
        int ret = 0, tret = 0;

        for(slotNo=0; slotNo<gu8_SlotNums; slotNo++)
        {
            //if(ge_PORTSlot[(SlotEmType)slotNo] == EV_PFCIE5_SDIO_PLL)
                //clk_disable_unprepare(gp_clkSDSync);

            //clk_disable_unprepare(gp_clkSlot[slotNo]);
            //clk_disable_unprepare(gp_clkMCMSlot[slotNo]);
            //clk_disable_unprepare(gp_clkSRAMSlot[slotNo]);

            pr_sd(">> [sdmmc_%u] Suspend device pm...(Ret:%u) \n", slotNo, ret);

            if(ret!=0)
            tret = ret;

        }

        return tret;
    }

    static int ms_sdmmc_devpm_resume(struct device *dev)
    {
        unsigned int slotNo = 0;
        int ret = 0, tret = 0;

        for(slotNo=0; slotNo<gu8_SlotNums; slotNo++)
        {
            ret = _InitConfig((SlotEmType)slotNo);


            //Mark for I2: Don't need use DTS Clock
            /*
            ret = clk_prepare_enable(gp_clkMCMSlot[slotNo]);

            if(ret!=0)
                tret = ret;

            ret = clk_prepare_enable(gp_clkSRAMSlot[slotNo]);

            if(ret!=0)
                tret = ret;
            */

            if(ret!=0)
                tret = ret;

            pr_sd(">> [sdmmc_%u] Resume device pm...(Ret:%u) \n", slotNo, ret);
        }

        return tret;
    }

    #else

    #define ms_sdmmc_devpm_prepare  NULL
    #define ms_sdmmc_devpm_complete NULL
    #define ms_sdmmc_devpm_suspend  NULL
    #define ms_sdmmc_devpm_resume   NULL

    #endif

    static int ms_sdmmc_devpm_runtime_suspend(struct device *dev)
    {
        pr_sd(">> [sdmmc] Runtime Suspend device pm...\n");
        return 0;
    }

    static int ms_sdmmc_devpm_runtime_resume(struct device *dev)
    {
        pr_sd(">> [sdmmc] Runtime Resume device pm...\n");
        return 0;
    }

    #define ms_sdmmc_suspend        NULL
    #define ms_sdmmc_resume         NULL


#else

    #ifdef CONFIG_PM

        #if ( LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0) ) // CONFIG_PM

        /*----------------------------------------------------------------------------------------------------------
         *
         * Function: ms_sdmmc_suspend
         *     @author jeremy.wang (2011/5/18)
         * Desc: Suspend MMC host
         *
         * @param p_dev :   platform device structure
         * @param state :   Power Management Transition State
         *
         * @return int  :   Error Status; Return 0 if no error.
         ----------------------------------------------------------------------------------------------------------*/
        static int ms_sdmmc_suspend(struct platform_device *p_dev, pm_message_t state)
        {
            struct ms_sdmmc_host *p_sdmmc_host = platform_get_drvdata(p_dev);
            struct mmc_host      *p_mmc_host;
            unsigned int slotNo = 0;
            int ret = 0, tret = 0;


            for(slotNo=0; slotNo<gu8_SlotNums; slotNo++)
            {
                if(gb_RejectSuspend)
                    return -1;

                p_mmc_host = p_sdmmc_host->sdmmc_slot[slotNo]->mmc;

                if (p_mmc_host)
                {

                #if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
                    ret = mmc_suspend_host(p_mmc_host);
                #else
                    ret = mmc_suspend_host(p_mmc_host, state);
                #endif
                    printk(">> [sdmmc_%u] Suspend host...(Ret:%u) \n", slotNo, ret);

                    if(ret!=0)
                        tret = ret;
                }

            }

            return tret;
        }

        /*----------------------------------------------------------------------------------------------------------
         *
         * Function: ms_sdmmc_resume
         *     @author jeremy.wang (2011/5/18)
         * Desc:   Resume MMC host
         *
         * @param p_dev :   platform device structure
         * @return int  :   Error Status; Return 0 if no error.
         ----------------------------------------------------------------------------------------------------------*/
        static int ms_sdmmc_resume(struct platform_device *p_dev)
        {
            struct ms_sdmmc_host *p_sdmmc_host = platform_get_drvdata(p_dev);
            struct mmc_host      *p_mmc_host;
            unsigned int slotNo = 0;
            int ret = 0, tret = 0;

            for(slotNo=0; slotNo<gu8_SlotNums; slotNo++)
            {
                p_mmc_host = p_sdmmc_host->sdmmc_slot[slotNo]->mmc;
                if (p_mmc_host)
                {
                    ret = mmc_resume_host(p_mmc_host);
                    pr_sd(">> [sdmmc_%u] Resume host...(Ret:%u) \n", slotNo, ret);

                    if(ret!=0)
                        tret = ret;
                }
            }

            return tret;
        }

        #else

        static int ms_sdmmc_suspend(struct platform_device *p_dev, pm_message_t state)
        {
            int ret = 0;
            return ret;
        }

        static int ms_sdmmc_resume(struct platform_device *p_dev)
        {
            int ret = 0;
            return ret;
        }

        #endif

     #else  // !CONFIG_PM

        //Current driver does not support following two functions, therefore set them to NULL.
        #define ms_sdmmc_suspend        NULL
        #define ms_sdmmc_resume         NULL

     #endif  // End of CONFIG_PM



#endif


/**********************************************************************************************************
 * Define Static Global Structs
 **********************************************************************************************************/

#if (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))
/*----------------------------------------------------------------------------------------------------------
 *  ms_sdmmc_of_match_table
 ----------------------------------------------------------------------------------------------------------*/
static const struct of_device_id ms_sdmmc_of_match_table[] = {
    { .compatible = "mstar,sdmmc" },
    {}
};


/*----------------------------------------------------------------------------------------------------------
 *  ms_sdmmc_dev_pm_ops
 ----------------------------------------------------------------------------------------------------------*/
static struct dev_pm_ops ms_sdmmc_dev_pm_ops = {
    .suspend    = ms_sdmmc_devpm_suspend,
    .resume     = ms_sdmmc_devpm_resume,
    .prepare    = ms_sdmmc_devpm_prepare,
    .complete   = ms_sdmmc_devpm_complete,
    .runtime_suspend = ms_sdmmc_devpm_runtime_suspend,
    .runtime_resume = ms_sdmmc_devpm_runtime_resume,
};

#else

/*----------------------------------------------------------------------------------------------------------
 *  st_ms_sdmmc_device
 ----------------------------------------------------------------------------------------------------------*/
static u64 mmc_dmamask = 0xffffffffUL;
static struct platform_device ms_sdmmc_pltdev =
{
    .name = DRIVER_NAME,
    .id = 0,
    .dev =
    {
        .dma_mask = &mmc_dmamask,
        .coherent_dma_mask = 0xffffffffUL,
    },
};

#endif //End of (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))


/*----------------------------------------------------------------------------------------------------------
 *  st_ms_sdmmc_driver
 ----------------------------------------------------------------------------------------------------------*/
static struct platform_driver ms_sdmmc_pltdrv =
{
    .remove  = ms_sdmmc_remove,/*__exit_p(ms_sdmmc_remove)*/
    .suspend = ms_sdmmc_suspend,
    .resume  = ms_sdmmc_resume,
    .probe   = ms_sdmmc_probe,
    .driver  =
    {
        .name  = DRIVER_NAME,
        .owner = THIS_MODULE,

#if (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))
        .of_match_table = of_match_ptr(ms_sdmmc_of_match_table),
        .pm = &ms_sdmmc_dev_pm_ops,
#endif

    },
};


/**********************************************************************************************************
 * Init & Exit Modules
 **********************************************************************************************************/

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_mci_init
 *     @author jeremy.wang (2011/7/18)
 * Desc: Linux Module Function for Init
 *
 * @return s32 __init  :  Error Status; Return 0 if no error.
 ----------------------------------------------------------------------------------------------------------*/
static s32 ms_sdmmc_init(void)
{

    pr_sd(">> [sdmmc] %s Driver Initializing...(V1.0) \n", DRIVER_NAME);

#if !(defined(CONFIG_OF) && (EN_DEV_TREE_SUP))
    platform_device_register(&ms_sdmmc_pltdev);
#endif

    return platform_driver_register(&ms_sdmmc_pltdrv);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_exit
 *     @author jeremy.wang (2011/9/8)
 * Desc: Linux Module Function for Exit
 ----------------------------------------------------------------------------------------------------------*/
static void ms_sdmmc_exit(void)
{
    platform_driver_unregister(&ms_sdmmc_pltdrv);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_cdzgpio_swctrl
 *     @author jeremy.wang (2015/1/7)
 * Desc: Switch CDZ GPIO to output state and control its state
 *
 * @param slotNo : GPIO1/GPIO2/GPIO3...
 * @param bOutputState : SW control output state
 ----------------------------------------------------------------------------------------------------------*/
void ms_sdmmc_cdzgpio_swctrl(unsigned int slotNo, bool bOutputState)
{
    PADEmType ePAD = ge_PADSlot[slotNo];

    pr_sdio_main(">> [sdmmc_%u] cdzgpio sw ctrl output(%u) \n", slotNo, bOutputState);
    Hal_SDPLT_CDZSetGPIOState((GPIOEmType)slotNo, ePAD, bOutputState);
}
EXPORT_SYMBOL(ms_sdmmc_cdzgpio_swctrl);



module_init(ms_sdmmc_init);
module_exit(ms_sdmmc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_AUTHOR("Jeremy_Wang");
