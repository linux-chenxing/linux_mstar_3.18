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
#include "inc/hal_card_platform.h"

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
#define EN_SDMMC_DCACHE_FLUSH    (TRUE)

#define EN_SDMMC_NOCDZ_NDERR     (FALSE)

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
//***********************************************************************************************************

// Multiple SDs Config Setting for Each Slot
//-----------------------------------------------------------------------------------------------------------
static IPEmType ge_IPSlot[3]                  = {D_SDMMC1_IP, D_SDMMC2_IP, D_SDMMC3_IP};
static PortEmType ge_PORTSlot[3]              = {D_SDMMC1_PORT, D_SDMMC2_PORT, D_SDMMC3_PORT};
static PADEmType  ge_PADSlot[3]               = {D_SDMMC1_PAD, D_SDMMC2_PAD, D_SDMMC3_PAD};
static MutexEmType ge_MutexSlot[3]            = {D_SDMMC1_MUTEX, D_SDMMC2_MUTEX, D_SDMMC3_MUTEX};

static U16_T  gu16_MieIntNoSlot[3]            = {V_SDMMC1_MIEIRQ, V_SDMMC2_MIEIRQ, V_SDMMC3_MIEIRQ};
static U16_T  gu16_CdzIntNoSlot[3]            = {V_SDMMC1_CDZIRQ, V_SDMMC2_CDZIRQ, V_SDMMC3_CDZIRQ};

static const U32_T  gu32_MaxClkSlot[3]        = {V_SDMMC1_MAX_CLK, V_SDMMC2_MAX_CLK, V_SDMMC3_MAX_CLK};
static const U32_T  gu32_MaxDLVLSlot[3]       = {V_SDMMC1_MAX_DLVL, V_SDMMC2_MAX_DLVL, V_SDMMC3_MAX_DLVL};
static const U32_T  gu32_PassLVLSlot[3]       = {V_SDMMC1_PASS_LVL, V_SDMMC2_PASS_LVL, V_SDMMC3_PASS_LVL};
static const U32_T  gu32_MIEIRQParaSlot[3]    = {V_SDMMC1_MIEIRQ_PARA, V_SDMMC2_MIEIRQ_PARA, V_SDMMC3_MIEIRQ_PARA};
static const U32_T  gu32_CDZIRQParaSlot[3]    = {V_SDMMC1_CDZIRQ_PARA, V_SDMMC2_CDZIRQ_PARA, V_SDMMC3_CDZIRQ_PARA};

static const BOOL_T gb_HotplugSlot[3]         = {EN_SDMMC1_HOTP, EN_SDMMC2_HOTP, EN_SDMMC3_HOTP};
static const BOOL_T gb_FakeCDZSlot[3]         = {EN_SDMMC1_FAKECDZ, EN_SDMMC2_FAKECDZ, EN_SDMMC3_FAKECDZ};
static const BOOL_T gb_ShareCDZIRQSlot[3]     = {EN_SDMMC1_CDZIRQ_SHARD, EN_SDMMC2_CDZIRQ_SHARD, EN_SDMMC3_CDZIRQ_SHARD};
static const BOOL_T gb_WakeupCDZIRQSlot[3]    = {EN_SDMMC1_CDZIRQ_WAKEUP, EN_SDMMC2_CDZIRQ_WAKEUP, EN_SDMMC3_CDZIRQ_WAKEUP};
static const BOOL_T gb_DownLVLSlot[3]         = {EV_SDMMC1_DOWN_LVL, EV_SDMMC2_DOWN_LVL, EV_SDMMC3_DOWN_LVL};
static const BOOL_T gb_SDIOIRQSlot[3]         = {EV_SDMMC1_SDIO_IRQ, EV_SDMMC2_SDIO_IRQ, EV_SDMMC3_SDIO_IRQ};
static const BOOL_T gb_SDIOPRTSlot[3]         = {EV_SDMMC1_SDIO_PRT, EV_SDMMC2_SDIO_PRT, EV_SDMMC3_SDIO_PRT};


// Global Variable
//-----------------------------------------------------------------------------------------------------------
static IntSourceStruct  gst_IntSourceSlot[3];
static spinlock_t g_RegLockSlot[3];


#if (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))
struct clk* gp_clkSlot[3];
struct clk* gp_clkMCMSlot[3];
struct clk* gp_clkSRAMSlot[3];
#endif

static volatile BOOL_T   gb_RejectSuspend     = FALSE;
static volatile BOOL_T   gb_AlwaysSwitchPAD   = (D_SDMMC1_IP==D_SDMMC2_IP) && (V_SDMMC_CARDNUMS>=2);

extern struct mutex FCIE3_mutex; // use fcie mutex in other driver
DEFINE_MUTEX(sdmmc1_mutex);
DEFINE_MUTEX(sdmmc2_mutex);
DEFINE_MUTEX(sdmmc3_mutex);


// String Name
//-----------------------------------------------------------------------------------------------------------
#define DRIVER_NAME "ms_sdmmc"
#define DRIVER_DESC "Mstar SD/MMC Card Interface driver"

// Trace Funcion
//-----------------------------------------------------------------------------------------------------------
#if (EN_SDMMC_TRFUNC)
    #define pr_sd_err(fmt, arg...)   //
    #define pr_sd_main(fmt, arg...)  printk(fmt, ##arg)
    #define pr_sd_dbg(fmt, arg...)   //printk(fmt, ##arg)
#else
    #define pr_sd_err(fmt, arg...)   printk(fmt, ##arg)
    #define pr_sd_main(fmt, arg...)  //
    #define pr_sd_dbg(fmt, arg...)   //
#endif

#if (EN_SDMMC_TRSDIO)
    #define pr_sdio_main(fmt, arg...)  printk(fmt, ##arg)
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

    Hal_CARD_IPBeginSetting(eIP);

}


// Section Process End
//------------------------------------------------------------------------------------------------
static void _CRIT_SECT_END(SlotEmType eSlot)
{
    MutexEmType eMutex = ge_MutexSlot[eSlot];
    IPEmType eIP      = ge_IPSlot[eSlot];

    Hal_CARD_IPEndSetting(eIP);

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


// Switch PAD
//------------------------------------------------------------------------------------------------
static void _SwitchPAD(SlotEmType eSlot)
{
    IPEmType eIP      = ge_IPSlot[eSlot];
    PortEmType ePort  = ge_PORTSlot[eSlot];
    PADEmType ePAD    = ge_PADSlot[eSlot];

    Hal_CARD_SetPADToPortPath(eIP, ePort, ePAD, FALSE);
    Hal_CARD_InitPADPin(ePAD, FALSE);

}


//###########################################################################################################
#if (D_PROJECT == D_PROJECT__iNF)
//###########################################################################################################
//------------------------------------------------------------------------------------------------
static void _SetPAD(SlotEmType eSlot, IPEmType eIP, PortEmType ePort, PADEmType ePAD, MutexEmType eMutex)
{
    ge_IPSlot[eSlot] = eIP;
    ge_PORTSlot[eSlot] = ePort;
    ge_PADSlot[eSlot] = ePAD;
    ge_MutexSlot[eSlot] = eMutex;
}
//###########################################################################################################
#endif


// Set Bus Voltage
//------------------------------------------------------------------------------------------------
static void _SetBusVdd(SlotEmType eSlot, U8_T u8Vdd)
{
    PADEmType ePAD    = ge_PADSlot[eSlot];

    /****** Simple Setting Here ******/
    Hal_CARD_SetPADPower(ePAD, (PADVddEmType)u8Vdd);

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
        Hal_CARD_PullPADPin(ePAD, EV_PULLDOWN, FALSE);
        Hal_CARD_PowerOff(ePAD, WT_POWEROFF); //For SD PAD

    }
    else if(u8PowerMode == MMC_POWER_UP)  // Power Up
    {
        Hal_CARD_PullPADPin(ePAD, EV_PULLUP, FALSE);
        Hal_CARD_PowerOn(ePAD, WT_POWERUP);
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
        u32RealClk = Hal_CARD_FindClockSetting(eIP, (U32_T)u32ReffClk, u8PassLevel, u8DownLevel);

    #if (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))
        clk_set_rate(gp_clkSlot[eSlot], u32RealClk);
    #else
        Hal_CARD_SetClock(eIP, u32RealClk);
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
            Hal_SDMMC_SetBusTiming(eIP, EV_BUS_DEF);
            Hal_CARD_SetBusTiming(eIP, EV_BUS_DEF);
            break;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0))
        case MMC_TIMING_UHS_SDR25:
#endif
        case MMC_TIMING_SD_HS:
        case MMC_TIMING_MMC_HS:
        case MMC_TIMING_UHS_SDR50:
        case MMC_TIMING_UHS_SDR104:
        case MMC_TIMING_MMC_HS200:
            /****** For High Speed ******/
            Hal_SDMMC_SetBusTiming(eIP, EV_BUS_HS);
            Hal_CARD_SetBusTiming(eIP, EV_BUS_HS);
            break;

        case MMC_TIMING_UHS_DDR50:
            Hal_SDMMC_SetBusTiming(eIP, EV_BUS_DDR50);
            Hal_CARD_SetBusTiming(eIP, EV_BUS_DDR50);
            break;

        default:
            /****** For 300KHz IP Issue but not for Default Speed ******/
            Hal_SDMMC_SetBusTiming(eIP, EV_BUS_LOW);
            Hal_CARD_SetBusTiming(eIP, EV_BUS_LOW);
            break;
    }


}

//------------------------------------------------------------------------------------------------
static BOOL_T _GetCardDetect(SlotEmType eSlot)
{
    if(gb_FakeCDZSlot[eSlot])
    {
        return (TRUE);
    }
    else
    {

#if(EN_SDMMC_CDZREV)
        return !Hal_CARD_GetGPIOState((GPIOEmType)eSlot);
#else
        return Hal_CARD_GetGPIOState((GPIOEmType)eSlot);
#endif

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
static U16_T _PreDataBufferMapping(TransEmType eTransType, struct mmc_data *data, struct ms_sdmmc_slot *sdmmchost, volatile U32_T *pu32AddrArr)
{
    struct scatterlist *p_sg_list = 0;
    u32 u16sg_idx = 0;
    u8 u8Dir = 0;

#if (!EN_SDMMC_BRO_DMA)
    u32 *pSGbuf = 0;
    u32 u32TranBytes = 0;
    unsigned *pDMAbuf = sdmmchost->dma_buffer;
    u32 u32TotalSize = data->blksz * data->blocks;
#endif

    if(eTransType == EV_DMA)
    {
        if(data->flags & MMC_DATA_READ)
            u8Dir = DMA_FROM_DEVICE;
        else
            u8Dir = DMA_TO_DEVICE;

#if (EN_SDMMC_BRO_DMA)
        for(u16sg_idx=0 ; u16sg_idx< data->sg_len ; u16sg_idx++)
#else
        if(data->sg_len==1)
#endif
        {
            p_sg_list = &data->sg[u16sg_idx];
            p_sg_list->dma_address = dma_map_page(NULL, sg_page(p_sg_list), p_sg_list->offset, p_sg_list->length, u8Dir);
            dma_mapping_error(NULL, p_sg_list->dma_address);  //Add to avoid unmap warning!

            if((p_sg_list->dma_address==0) || (p_sg_list->dma_address==~0))  //Mapping Error!
                return 0;

            pu32AddrArr[u16sg_idx] = (U32_T)p_sg_list->dma_address;
        }

#if (EN_SDMMC_BRO_DMA)
        return (U16_T)data->sg_len;
#else
        else
        {
            if(data->flags & MMC_DATA_WRITE)  //SGbuf => DMA buf
            {
                while(u16sg_idx < data->sg_len)
                {
                    p_sg_list = &data->sg[u16sg_idx];

                    pSGbuf = kmap_atomic(sg_page(p_sg_list), KM_BIO_SRC_IRQ) + p_sg_list->offset;
                    u32TranBytes   = min(u32TotalSize, p_sg_list->length);
                    memcpy(pDMAbuf, pSGbuf, u32TranBytes);
                    u32TotalSize -= u32TranBytes;
                    pDMAbuf += (u32TranBytes >> 2) ;
                    kunmap_atomic(pSGbuf, KM_BIO_SRC_IRQ);

                    u16sg_idx++;
                }

            }

            pu32AddrArr[0] = (U32_T) sdmmchost->dma_phy_addr;
        }

#endif

    }
    else if(eTransType == EV_CIF)
    {
        p_sg_list = &data->sg[0];
        pu32AddrArr[0] = (U32_T)( page_address(sg_page(p_sg_list)) + p_sg_list->offset );
    }

    return 1;
}
//------------------------------------------------------------------------------------------------
static void _PostDataBufferMapping(TransEmType eTransType, struct mmc_data *data, struct ms_sdmmc_slot *sdmmchost)
{
    struct scatterlist *p_sg_list = 0;
    u32 u16sg_idx = 0;
    u8  u8Dir = 0;

#if (!EN_SDMMC_BRO_DMA)
    u32 *pSGbuf = 0;
    u32 u32TranBytes = 0;
    unsigned *pDMAbuf = sdmmchost->dma_buffer;
    u32 u32TotalSize = data->blksz * data->blocks;
#endif

    if(!(eTransType == EV_DMA))
        return;

    if(data->flags & MMC_DATA_READ)
        u8Dir = DMA_FROM_DEVICE;
    else
        u8Dir = DMA_TO_DEVICE;

#if (EN_SDMMC_BRO_DMA)

    for(u16sg_idx=0 ; u16sg_idx< data->sg_len ; u16sg_idx++)
    {
        p_sg_list = &data->sg[u16sg_idx];
        dma_unmap_page(NULL, p_sg_list->dma_address, p_sg_list->length, u8Dir);
        if ( (u8Dir==DMA_FROM_DEVICE) && EN_SDMMC_DCACHE_FLUSH && (sg_page(p_sg_list) != NULL) && !PageSlab(sg_page(p_sg_list)) )
            flush_dcache_page(sg_page(p_sg_list));
    }

#else

    if(data->sg_len==1)
    {
        p_sg_list = &data->sg[0];
        dma_unmap_page(NULL, p_sg_list->dma_address, p_sg_list->length, u8Dir);
        if ( (u8Dir==DMA_FROM_DEVICE) && EN_SDMMC_DCACHE_FLUSH && (sg_page(p_sg_list) != NULL) && !PageSlab(sg_page(p_sg_list)) )
            flush_dcache_page(sg_page(p_sg_list));
    }
    else
    {
        if(data->flags & MMC_DATA_READ)  //SGbuf => DMA buf
        {
            while(u16sg_idx < data->sg_len)
            {
                p_sg_list = &data->sg[u16sg_idx];

                pSGbuf = kmap_atomic(sg_page(p_sg_list), KM_BIO_SRC_IRQ) + p_sg_list->offset;
                u32TranBytes   = min(u32TotalSize, p_sg_list->length);
                memcpy(pSGbuf, pDMAbuf, u32TranBytes);
                u32TotalSize -= u32TranBytes;
                pDMAbuf += (u32TranBytes >> 2) ;
                kunmap_atomic(pSGbuf, KM_BIO_SRC_IRQ);
                u16sg_idx++;
            }
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
        case SD_IO_RW_DIRECT:    //SDIO =>Cmd_52
        case MMC_SEND_STATUS:    //SD   =>CMD13
        case MMC_APP_CMD:        //SD   =>Cmd55
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

    IPEmType eIP = ge_IPSlot[eSlot];

    if( eErrType == EV_STS_OK )
    {
        pr_sdio_main("_[%01X]", Hal_SDMMC_GetDATBusLevel(eIP));
        pr_sd_main("@\n");
        return nErr;
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
            printk("\n>> [sdmmc_%u] Auto Re-Dectection.........\n\n", eSlot);
        }
    }

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
        printk(">> [sdmmc_%u] Downgrade Clk from (Clk=%u)=> (Real=%u)\n", eSlot, p_sdmmc_slot->currClk, p_sdmmc_slot->currRealClk);
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
    struct ms_sdmmc_slot *p_sdmmc_slot = pstIntSource->p_data;

    if(!gb_ShareCDZIRQSlot[eSlot])
        disable_irq_nosync(irq);

    if(Hal_CARD_GPIOIntFilter((GPIOEmType)eSlot))
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

#if(!EN_SDMMC_CDZREV)
    GPIOOptEmType eINSOPT = EV_GPIO_OPT3;
    GPIOOptEmType eEJTOPT = EV_GPIO_OPT4;
#else
    GPIOOptEmType eINSOPT = EV_GPIO_OPT4;
    GPIOOptEmType eEJTOPT = EV_GPIO_OPT3;
#endif

    printk("\n>> [sdmmc_%u] CDZ... ", eSlot);

LABEL_LOOP_HOTPLUG:

    if( _GetCardDetect(eSlot) ) // Insert (CDZ)
    {
        if( (FALSE) == _CardDetect_PlugDebounce(eSlot, WT_DB_PLUG, TRUE) )
            goto LABEL_LOOP_HOTPLUG;

        mmc_detect_change(p_sdmmc_slot->mmc, msecs_to_jiffies(WT_DB_SW_PLUG));
        printk("(INS) OK!\n");

        Hal_CARD_SetGPIOIntAttr((GPIOEmType)eSlot, EV_GPIO_OPT1);
        Hal_CARD_SetGPIOIntAttr((GPIOEmType)eSlot, eINSOPT);

    }
    else // Remove (CDZ)
    {
        if( (TRUE) == _CardDetect_PlugDebounce(eSlot, WT_DB_UNPLUG, FALSE) )
                goto LABEL_LOOP_HOTPLUG;

        if (p_sdmmc_slot->mmc->card)
            mmc_card_set_removed(p_sdmmc_slot->mmc->card);

        Hal_SDMMC_StopProcessCtrl(eIP, TRUE);
        mmc_detect_change(p_sdmmc_slot->mmc, msecs_to_jiffies(WT_DB_SW_UNPLUG));
        printk("(EJT) OK!\n");

        Hal_CARD_SetGPIOIntAttr((GPIOEmType)eSlot, EV_GPIO_OPT1);
        Hal_CARD_SetGPIOIntAttr((GPIOEmType)eSlot, eEJTOPT);

    }

    if(!gb_ShareCDZIRQSlot[eSlot])
        enable_irq(p_sdmmc_slot->cdzIRQNo);

}

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
    TransEmType eTransType = EV_DMA;
    CmdEmType eCmdType = EV_CMDRSP;
    SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;
    IPEmType eIP = ge_IPSlot[eSlot];
    volatile U32_T au32Addr[MAX_SEG_CNT];

    BOOL_T bCloseClock = FALSE;
    U8_T u8CMD = 0;
    U16_T u16BlkSize = 0, u16BlkCnt = 0, u16SubBlkCnt = 0;
    U32_T u32Arg = 0, u32SubLen = 0;
    U16_T u16ProcCnt = 0, u16Idx= 0;

    _CRIT_SECT_BEGIN(eSlot);

    u8CMD =(U8_T)cmd->opcode;
    u32Arg = (U32_T)cmd->arg;

    if(Hal_SDMMC_OtherPreUse(eIP) || gb_AlwaysSwitchPAD)
        _SwitchPAD(eSlot);

    if(!p_sdmmc_slot->mmc->card)
       Hal_SDMMC_StopProcessCtrl(eIP, FALSE);

#if (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))
    clk_set_rate(gp_clkSlot[eSlot], p_sdmmc_slot->currRealClk);
#else
    Hal_CARD_SetClock(eIP, p_sdmmc_slot->currRealClk);
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

        pr_sd_main("__[Sgl: %u] (TB: %u)(BSz: %u)", (U16_T)data->sg_len, u16BlkCnt, u16BlkSize);

        if(data->flags & MMC_DATA_READ)
            eCmdType = EV_CMDREAD;
        else
            eCmdType = EV_CMDWRITE;

        if(stop)
            bCloseClock = FALSE;
        else
            bCloseClock = TRUE;

        u16ProcCnt = _PreDataBufferMapping(eTransType, data, p_sdmmc_slot, au32Addr);
        if(u16ProcCnt==0)
        {
            pr_err("\n>> [sdmmc_%u] Err: DMA Mapping Addr Error!\n", eSlot);
            eErr = EV_OTHER_ERR;
            goto LABEL_SD_ERR;
        }

        Hal_SDMMC_TransCmdSetting(eIP, eTransType, u16SubBlkCnt, u16BlkSize, Hal_CARD_TransMIUAddr(au32Addr[0]), (volatile U8_T*)au32Addr[0]);
        Hal_SDMMC_SetSDIOIntBeginSetting(eIP, u8CMD, u32Arg, eCmdType, u16BlkCnt);
        eErr = Hal_SDMMC_SendCmdAndWaitProcess(eIP, eTransType, eCmdType, _TransRspType(mmc_resp_type(cmd)), bCloseClock);

        pr_sd_dbg("\n____[0] =>> (SBCnt: %u)__[Addr: 0x%08X]", u16SubBlkCnt, au32Addr[0]);

        if( ((U16_T)eErr) == EV_STS_OK )
        {
            data->bytes_xfered += u32SubLen;

            /****** Broken DMA *******/
            for(u16Idx=1 ; u16Idx<u16ProcCnt; u16Idx++)
            {
                u32SubLen = (U32_T) data->sg[u16Idx].length;
                u16SubBlkCnt = (U16_T)(u32SubLen/u16BlkSize);
                pr_sd_dbg("\n____[%u] =>> (SB: %u)__[Addr: 0x%08X]", u16Idx, u16SubBlkCnt, au32Addr[u16Idx]);

                Hal_SDMMC_TransCmdSetting(eIP, eTransType, u16SubBlkCnt, u16BlkSize, Hal_CARD_TransMIUAddr(au32Addr[u16Idx]), (volatile U8_T *)au32Addr[u16Idx]);
                eErr = Hal_SDMMC_RunBrokenDmaAndWaitProcess(eIP, eCmdType);

                if((U16_T)eErr) break;
                data->bytes_xfered += u32SubLen;
            }
        }

        Hal_SDMMC_SetSDIOIntEndSetting(eIP, eErr, u16BlkCnt);

        _PostDataBufferMapping(eTransType, data, p_sdmmc_slot);

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
    else if(data && data->error) //Data Err
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

        stop->error = _RequestEndProcess(eErr, EV_CMDRSP, p_sdmmc_slot, data);

        eRspSt = Hal_SDMMC_GetRspToken(eIP);
        stop->resp[0] = _TransArrToUInt(eRspSt->u8ArrRspToken[1], eRspSt->u8ArrRspToken[2], eRspSt->u8ArrRspToken[3], eRspSt->u8ArrRspToken[4]);

        if(stop->error)
            pr_sd_err(">> [sdmmc_%u] Err: #Cmd_12 => (E: 0x%04X)(S: 0x%08X)__(L:%u)\n", eSlot, (U16_T)eErr, stop->resp[0], eRspSt->u32ErrLine);


    }

    //Hal_CARD_SetClock(eIP, p_sdmmc_slot->pmrsaveClk); // For Power Saving

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

    /****** Clock Setting*******/
    if(p_sdmmc_slot->currClk != p_mmc_ios->clock)
    {
/* Modified by Spade: enable clk in probe
        #if (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))
        if(p_mmc_ios->clock>0)
            clk_prepare_enable(gp_clkSlot[eSlot]);
        else
            clk_disable_unprepare(gp_clkSlot[eSlot]);
        #endif
*/
        p_sdmmc_slot->currDownLevel = 0;
        p_sdmmc_slot->currClk = p_mmc_ios->clock;
        p_sdmmc_slot->currRealClk = _SetClock(eSlot, p_sdmmc_slot->currClk, 0, p_sdmmc_slot->currDownLevel);

        if( (p_sdmmc_slot->currRealClk==0) && (p_sdmmc_slot->currClk!=0) )
            printk(">> [sdmmc_%u] Set IOS => Clk=Error\n", eSlot);
        else if(p_sdmmc_slot->currRealClk <= 400000)
            _SetBusTiming(eSlot, 0xFF);
        else
            printk(">> [sdmmc_%u] Set IOS => Clk=%u (Real=%u)\n", eSlot, p_sdmmc_slot->currClk, p_sdmmc_slot->currRealClk);
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

    /****** Bus Timing Setting*******/
    if( (p_sdmmc_slot->currTiming != p_mmc_ios->timing) || !p_sdmmc_slot->initFlag)
    {
        p_sdmmc_slot->currTiming = p_mmc_ios->timing;
        _SetBusTiming(eSlot, p_sdmmc_slot->currTiming);
        pr_sd_main(">> [sdmmc_%u] Set IOS => BusTiming=%u\n", eSlot, p_sdmmc_slot->currTiming);
    }

    /****** Voltage Setting *******/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 8))
    if( (p_sdmmc_slot->currVdd != p_mmc_ios->signal_voltage) || !p_sdmmc_slot->initFlag)
    {
        p_sdmmc_slot->currVdd = p_mmc_ios->signal_voltage;
        pr_sd_main(">> [sdmmc_%u] Set IOS => Voltage=%u\n", eSlot, p_sdmmc_slot->currVdd);
        _SetBusVdd(eSlot, p_sdmmc_slot->currVdd);
    }
#else
    _SetBusVdd(eSlot, EV_NORVOL);
#endif

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

    _SwitchPAD(eSlot);

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

    _CRIT_SECT_BEGIN(eSlot);

    _SwitchPAD(eSlot);

    if( _GetCardDetect(eSlot) )
        p_sdmmc_slot->card_det = 1;
    else
        p_sdmmc_slot->card_det  = 0;

    _CRIT_SECT_END(eSlot);

    printk(">> [sdmmc_%u] Get CD => (%d)\n", eSlot, p_sdmmc_slot->card_det);

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

    Hal_SDMMC_SDIODeviceCtrl(eIP, TRUE);
    p_sdmmc_slot->sdioFlag = 1;

    printk(">> [sdmmc_%u] Found SDIO Device!\n", eSlot);

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

/**********************************************************************************************************
 * Define Static Global Structs
 **********************************************************************************************************/
/*----------------------------------------------------------------------------------------------------------
 *  st_mmc_ops
 ----------------------------------------------------------------------------------------------------------*/
static const struct mmc_host_ops st_mmc_ops =
{
    .enable          = ms_sdmmc_enable,
    .disable         = ms_sdmmc_disable,
    .request         = ms_sdmmc_request,
    .set_ios         = ms_sdmmc_set_ios,
    .get_ro          = ms_sdmmc_get_ro,
    .get_cd          = ms_sdmmc_get_cd,
    .init_card       = ms_sdmmc_init_card,
    .enable_sdio_irq = ms_sdmmc_enable_sdio_irq,

};



#if (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))
/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_dts_init
 *     @author jeremy.wang (2015/12/17)
 * Desc: Init Device Tree Setting
 *
 * @param slotNo : Slot Number
 * @param p_dev : platform device
 *
 * @return int  : Error Status; Return 0 if no error
 ----------------------------------------------------------------------------------------------------------*/
static int ms_sdmmc_dts_init(unsigned int slotNo, struct platform_device *p_dev)
{
    SlotEmType eSlot = (SlotEmType)slotNo;
    IPEmType eIP     = ge_IPSlot[eSlot];
    //int ret;

    /****** Get Device Tree Information******/
    gp_clkSlot[slotNo] = of_clk_get(p_dev->dev.of_node, (eIP*3));
    gp_clkMCMSlot[slotNo] = of_clk_get(p_dev->dev.of_node, (eIP*3) + 1);
    gp_clkSRAMSlot[slotNo] = of_clk_get(p_dev->dev.of_node, (eIP*3) + 2);

    if(IS_ERR(gp_clkSlot[slotNo]) )
    {
        pr_err(">> [sdmmc_%u] Err: Failed to get dts clock tree!\n", slotNo);
        return -ENODEV;
    }

    clk_prepare_enable(gp_clkMCMSlot[slotNo]);
    clk_prepare_enable(gp_clkSRAMSlot[slotNo]);
    clk_prepare_enable(gp_clkSlot[slotNo]);

    gu16_MieIntNoSlot[slotNo]= irq_of_parse_and_map(p_dev->dev.of_node, (eIP*2) );
    gu16_CdzIntNoSlot[slotNo]= irq_of_parse_and_map(p_dev->dev.of_node, (eIP*2) + 1);

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
        pr_err(">> [sdmmc_%u] Err: Failed to Allocate sdmmc_host DMA buffer\n", slotNo);
        return -ENOMEM;
    }
#endif

    p_sdmmc_slot->mmc          = p_mmc_host;
    p_sdmmc_slot->slotNo       = slotNo;
    p_sdmmc_slot->pmrsaveClk   = Hal_CARD_FindClockSetting(eIP, 400000, 0, 0);
    p_sdmmc_slot->mieIRQNo     = gu16_MieIntNoSlot[eSlot];
    p_sdmmc_slot->cdzIRQNo     = gu16_CdzIntNoSlot[eSlot];
    p_sdmmc_slot->cdzGPIONo    = Hal_CARD_GetGPIONum((GPIOEmType)slotNo);
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
    Hal_CARD_IPOnceSetting(eIP);

    /****** (5) Init GPIO Setting ******/
    Hal_CARD_InitGPIO((GPIOEmType)slotNo, !gb_FakeCDZSlot[eSlot]);

    /****** (6) Interrupt Source Setting ******/
    gst_IntSourceSlot[eSlot].slotNo = slotNo;
    gst_IntSourceSlot[eSlot].eIP = eIP;
    gst_IntSourceSlot[eSlot].eCardInt = EV_INT_SD;
    gst_IntSourceSlot[eSlot].p_data = p_sdmmc_slot;

    /*****  (7) Spinlock Init for Reg Protection ******/
    spin_lock_init(&g_RegLockSlot[slotNo]);

    /****** (8) Register IP IRQ *******/
    Hal_SDMMC_MIEIntCtrl(eIP, FALSE);
    if(EN_SDMMC_MIEINT_MODE)
    {
        nRet = request_irq(p_sdmmc_slot->mieIRQNo, Hal_CARD_INT_MIE, gu32_MIEIRQParaSlot[eSlot], DRIVER_NAME, &gst_IntSourceSlot[eSlot]);
        if (nRet)
        {
            pr_err(">> [sdmmc_%u] Err: Failed to request MIE Interrupt (%u)!\n", slotNo, p_sdmmc_slot->mieIRQNo);
            goto LABEL_OUT2;
        }

        Hal_SDMMC_MIEIntCtrl(eIP, TRUE);

        if(gb_SDIOIRQSlot[eSlot])
        {
            p_mmc_host->caps |= MMC_CAP_SDIO_IRQ;
            printk(">> [sdmmc_%u] Enable SDIO Interrupt Mode! \n", slotNo);

        }
    }

    mmc_add_host(p_mmc_host);

    if(!gb_HotplugSlot[eSlot])
        goto LABEL_OUT0;

    /****** (9) Register Ext CDZ IRQ  *******/
    if(!p_sdmmc_slot->cdzIRQNo)
    {
        if(p_sdmmc_slot->cdzGPIONo && gpio_to_irq(p_sdmmc_slot->cdzGPIONo)> 0 )
            p_sdmmc_slot->cdzIRQNo = gpio_to_irq(p_sdmmc_slot->cdzGPIONo);
    }


    tasklet_init(&p_sdmmc_slot->hotplug_tasklet, ms_sdmmc_hotplug, (unsigned long)p_sdmmc_slot);

    if(p_sdmmc_slot->cdzIRQNo)
    {

    #if(!EN_SDMMC_CDZREV)
        Hal_CARD_SetGPIOIntAttr((GPIOEmType)slotNo, (_GetCardDetect(eSlot)? EV_GPIO_OPT3 : EV_GPIO_OPT4));
    #else
        Hal_CARD_SetGPIOIntAttr((GPIOEmType)slotNo, (_GetCardDetect(eSlot)? EV_GPIO_OPT4 : EV_GPIO_OPT3));
    #endif

        Hal_CARD_SetGPIOIntAttr((GPIOEmType)slotNo, EV_GPIO_OPT1);

        nRet = request_irq(p_sdmmc_slot->cdzIRQNo, ms_sdmmc_cdzint, gu32_CDZIRQParaSlot[eSlot], DRIVER_NAME, &gst_IntSourceSlot[eSlot]);
        if(nRet)
        {
            pr_err(">> [sdmmc_%u] Err: Failed to request CDZ Interrupt (%u)!\n", slotNo, p_sdmmc_slot->cdzIRQNo);
            goto LABEL_OUT1;
        }

        printk(">> [sdmmc_%u] CDZ use Ext GPIO IRQ: %u\n", slotNo, p_sdmmc_slot->cdzIRQNo);

        Hal_CARD_SetGPIOIntAttr((GPIOEmType)slotNo, EV_GPIO_OPT2);

        if(gb_WakeupCDZIRQSlot[eSlot])
            irq_set_irq_wake(p_sdmmc_slot->cdzIRQNo, TRUE);

    }
    else
    {
        printk(">> [sdmmc_%u] CDZ IRQ doesn't exist!\n", slotNo);
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
    int ret = 0, tret = 0;

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


//###########################################################################################################
#if (D_PROJECT == D_PROJECT__iNF)
//###########################################################################################################
    if(1) //Temp Value
    {
        printk(">> [sdmmc_0] belong to [sdio ip]\n");
        _SetPAD(0, EV_IP_FCIE1, EV_PFCIE5_SDIO, EV_PAD1, EV_MUTEX1);
    }
    else
    {
        printk(">> [sdmmc_0] belong to [fcie ip]\n");
        _SetPAD(0, EV_IP_FCIE2, EV_PFCIE5_FCIE, EV_PAD2, EV_MUTEX1);
    }
//###########################################################################################################
#endif

    for(slotNo=0; slotNo<V_SDMMC_CARDNUMS; slotNo++)
    {


#if (defined(CONFIG_OF) && (EN_DEV_TREE_SUP))
        ret = ms_sdmmc_dts_init(slotNo, p_dev);
        if(ret!=0)
            return ret;
#endif

        ret = ms_sdmmc_init_slot(slotNo, p_sdmmc_host);
        printk(KERN_INFO ">> [sdmmc_%u] Probe Platform Devices...(Ret:%d) \n", slotNo, ret);
        if(ret!=0)
        {
            tret = ret;
        }

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

    if(gb_HotplugSlot[eSlot])
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

    for(slotNo=0; slotNo<V_SDMMC_CARDNUMS; slotNo++)
    {
        ms_sdmmc_remove_slot(slotNo, p_sdmmc_host);
        printk(">> [sdmmc_%u] Remove devices...\n", slotNo);

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

        for(slotNo=0; slotNo<V_SDMMC_CARDNUMS; slotNo++)
        {
            clk_disable_unprepare(gp_clkSlot[slotNo]);
            clk_disable_unprepare(gp_clkMCMSlot[slotNo]);
            clk_disable_unprepare(gp_clkSRAMSlot[slotNo]);

            printk(">> [sdmmc_%u] Suspend device pm...(Ret:%u) \n", slotNo, ret);
        }

        return tret;
    }

    static int ms_sdmmc_devpm_resume(struct device *dev)
    {
        unsigned int slotNo = 0;
        int ret = 0, tret = 0;

        for(slotNo=0; slotNo<V_SDMMC_CARDNUMS; slotNo++)
        {
            ret = clk_prepare_enable(gp_clkMCMSlot[slotNo]);

            if(ret!=0)
                tret = ret;

            ret = clk_prepare_enable(gp_clkSRAMSlot[slotNo]);

            if(ret!=0)
                tret = ret;

            ret = clk_prepare_enable(gp_clkSlot[slotNo]);

            if(ret!=0)
                tret = ret;

            printk(">> [sdmmc_%u] Resume device pm...(Ret:%u) \n", slotNo, ret);
        }

        return tret;
    }

    #else

    #define ms_sdmmc_devpm_prepare	NULL
    #define ms_sdmmc_devpm_complete	NULL
    #define ms_sdmmc_devpm_suspend	NULL
    #define ms_sdmmc_devpm_resume	NULL

    #endif

    static int ms_sdmmc_devpm_runtime_suspend(struct device *dev)
    {
        printk(">> [sdmmc] Runtime Suspend device pm...\n");
        return 0;
    }

    static int ms_sdmmc_devpm_runtime_resume(struct device *dev)
    {
        printk(">> [sdmmc] Runtime Resume device pm...\n");
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


            for(slotNo=0; slotNo<V_SDMMC_CARDNUMS; slotNo++)
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

            for(slotNo=0; slotNo<V_SDMMC_CARDNUMS; slotNo++)
            {
                p_mmc_host = p_sdmmc_host->sdmmc_slot[slotNo]->mmc;
                if (p_mmc_host)
                {
                    ret = mmc_resume_host(p_mmc_host);
                    printk(">> [sdmmc_%u] Resume host...(Ret:%u) \n", slotNo, ret);
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
	.suspend	= ms_sdmmc_devpm_suspend,
	.resume		= ms_sdmmc_devpm_resume,
	.prepare	= ms_sdmmc_devpm_prepare,
	.complete	= ms_sdmmc_devpm_complete,
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

    printk(KERN_INFO ">> [sdmmc] %s Driver Initializing... \n", DRIVER_NAME);

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
    pr_sdio_main(">> [sdmmc_%u] cdzgpio sw ctrl output(%u) \n", slotNo, bOutputState);
    Hal_CARD_SetGPIOState((GPIOEmType)slotNo, bOutputState);
}
EXPORT_SYMBOL(ms_sdmmc_cdzgpio_swctrl);



module_init(ms_sdmmc_init);
module_exit(ms_sdmmc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_AUTHOR("Jeremy_Wang");
