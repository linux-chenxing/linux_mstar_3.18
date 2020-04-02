/***************************************************************************************************************
 *
 * FileName ms_sdmmc_lnx.c
 *     @author jeremy.wang (2012/01/10)
 * Desc:
 * 	   This layer between Linux SD Driver layer and IP Hal layer.
 * 	   (1) The goal is we don't need to change any Linux SD Driver code, but we can handle here.
 * 	   (2) You could define Function/Ver option for using, but don't add Project option here.
 * 	   (3) You could use function option by Project option, but please add to ms_sdmmc.h
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
#include <linux/mmc/mmc.h>
#include <linux/mmc/host.h>
#include <linux/version.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/mmc/card.h>
#include <mstar/mstar_chip.h>
#include <linux/kthread.h>
#include <linux/mmc/sd.h>
#include <asm/io.h>


#include "ms_sdmmc_lnx.h"
#include "hal_card_common.h"
#include "hal_card_intr.h"
#include "hal_sdmmc.h"
#include "sd_platform.h"


#include "hal_card_regs5.h"

//***********************************************************************************************************
// Config Setting (Internel)
//***********************************************************************************************************
#define EN_SDMMC_TRFUNC          (FALSE)//(TRUE)
#define EN_SDMMC_MIEINT_MODE     (TRUE)
#define EN_SDMMC_BRO_DMA         (TRUE)
#define EN_SDMMC_NOCDZ_NDERR     (TRUE)//(FALSE)

/****** For Allocation buffer *******/
#define MAX_BLK_SIZE              512       //Maximum Transfer Block Size
#define MAX_BLK_COUNT             1024      //Maximum Transfer Block Count
#define MAX_SEG_CNT               128

/****** For broken DMA *******/
#define MAX_BRO_BLK_COUNT         1024      //Maximum Broken DMA Transfer Block Count

/****** For SD Debounce Setting *******/
#define WT_DB_PLUG                500       //Waiting time for Insert Debounce
#define WT_DB_SW_PLUG             300       //Waiting time for Plug Delay Process
#define WT_DB_SW_UNPLUG           0         //Waiting time for Uplug Delay Process

//***********************************************************************************************************
//***********************************************************************************************************

// Dual Dual SD Cards Setting for Each Slot
//-----------------------------------------------------------------------------------------------------------
static const IPEmType ge_IPSlot[2]            = {D_SDMMC1_IP, D_SDMMC2_IP};
static const PortEmType ge_PORTSlot[2]        = {D_SDMMC1_PORT, D_SDMMC2_PORT};
static const PADEmType  ge_PADSlot[2]         = {D_SDMMC1_PAD, D_SDMMC2_PAD};
static const MutexEmType ge_MutexSlot[2]      = {D_SDMMC1_MUTEX, D_SDMMC2_MUTEX};

static const U16_T	gu16_MieIntNoSlot[2]      = {V_SDMMC1_MIEIRQ, V_SDMMC2_MIEIRQ};
static const U16_T  gu16_CdzIntNoSlot[2]      = {V_SDMMC1_CDZIRQ, V_SDMMC2_CDZIRQ};
static const U32_T  gu32_MaxClkSlot[2]        = {V_SDMMC1_MAX_CLK, V_SDMMC2_MAX_CLK};
static const U32_T  gu32_MaxDLVLSlot[2]       = {V_SDMMC1_MAX_DLVL, V_SDMMC2_MAX_DLVL};
static const U32_T  gu32_PassLVLSlot[2]       = {V_SDMMC1_PASS_LVL, V_SDMMC2_PASS_LVL};
static const U32_T  gu32_MIEIRQParaSlot[2]	  = {V_SDMMC1_MIEIRQ_PARA, V_SDMMC2_MIEIRQ_PARA};
static const U32_T  gu32_CDZIRQParaSlot[2]	  = {V_SDMMC1_CDZIRQ_PARA, V_SDMMC2_CDZIRQ_PARA};

static const BOOL_T gb_HotplugSlot[2]         = {EN_SDMMC1_HOTP, EN_SDMMC2_HOTP};
static const BOOL_T gb_FakeCDZSlot[2]         = {EN_SDMMC1_FAKECDZ, EN_SDMMC2_FAKECDZ};
static const BOOL_T gb_ShareCDZIRQSlot[2]     = {EN_SDMMC1_CDZIRQ_SHARD, EN_SDMMC2_CDZIRQ_SHARD};
static const BOOL_T gb_WakeupCDZIRQSlot[2]    = {EN_SDMMC1_CDZIRQ_WAKEUP, EN_SDMMC2_CDZIRQ_WAKEUP};
static const BOOL_T gb_RunDDRSDRSlot[2]       = {EN_SDMMC1_RUN_DDRSDR, EN_SDMMC2_RUN_DDRSDR};
static const BOOL_T gb_DownLVLSlot[2]         = {EV_SDMMC1_DOWN_LVL, EV_SDMMC2_DOWN_LVL};

// Global Variable
//-----------------------------------------------------------------------------------------------------------
static volatile BOOL_T   gb_RejectSuspend     = FALSE;
static volatile BOOL_T   gb_AlwaysSwitchPAD   = (D_SDMMC1_IP==D_SDMMC2_IP) && EN_SDMMC_DUAL_CARDS;
//static volatile BOOL_T   gb_SDIOSlot[2]       = {FALSE, FALSE};
static IntSourceStruct  gst_IntSourceSlot[2];

#if !defined(EN_SDMMC_CDZ_POLLING)
static bool gb_card_chg[2] = {FALSE, FALSE};
#endif

#ifdef  FCIE_V5_ADMA
#define FCIE_ADMA_DESC_COUNT    512
struct _AdmaDescriptor  gSDAdmaDesc[FCIE_ADMA_DESC_COUNT] __attribute__((aligned(0x20)));
#endif

#if (D_PRODUCT == D_TV)
extern struct semaphore	PfModeSem;
#else
extern struct mutex FCIE3_mutex; // use fcie mutex in other driver
#endif
DEFINE_MUTEX(sdmmc1_mutex);
DEFINE_MUTEX(sdmmc2_mutex);

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

// Section Process Begin
//------------------------------------------------------------------------------------------------
static void _CRIT_SECT_BEGIN(SlotEmType eSlot)
{
	MutexEmType eMutex = ge_MutexSlot[eSlot];
	IPEmType eIP      = ge_IPSlot[eSlot];
	PADEmType ePAD    = ge_PADSlot[eSlot];

	if(eMutex == EV_MUTEX1)
	{
		mutex_lock(&sdmmc1_mutex);
	}
	else if(eMutex == EV_MUTEXS)
	{
		#if (D_PRODUCT == D_TV)
        	down(&PfModeSem);
			//printk("sd get semaphore\n");
		#else
			mutex_lock(&FCIE3_mutex);
		#endif
	}
	else if(eMutex == EV_MUTEX2)
	{
		mutex_lock(&sdmmc2_mutex);
	}

    Hal_CARD_Wait_Emmc_D0();

	Hal_CARD_IPBeginSetting(eIP, ePAD);

}

// Section Process End
//------------------------------------------------------------------------------------------------
static void _CRIT_SECT_END(SlotEmType eSlot)
{
	MutexEmType eMutex = ge_MutexSlot[eSlot];
	IPEmType eIP      = ge_IPSlot[eSlot];
	PADEmType ePAD    = ge_PADSlot[eSlot];

	Hal_CARD_IPEndSetting(eIP, ePAD);

	#ifdef IP_VERSION_FCIE5
		HalFcieSdio_WaitStateMachineFinsih(eIP);
	#endif

	if(eMutex == EV_MUTEX1)
		mutex_unlock(&sdmmc1_mutex);
	else if(eMutex == EV_MUTEXS)
	{
		#if (D_PRODUCT == D_TV)
			//printk("sd rels semaphore\n");
        	up(&PfModeSem);
		#else
			mutex_unlock(&FCIE3_mutex);
		#endif
	}
	else if(eMutex == EV_MUTEX2)
		mutex_unlock(&sdmmc2_mutex);

}

// Switch PAD
//------------------------------------------------------------------------------------------------
static void _SwitchPAD(SlotEmType eSlot)
{
	IPEmType eIP      = ge_IPSlot[eSlot];
	PortEmType ePort  = ge_PORTSlot[eSlot];
	PADEmType ePAD    = ge_PADSlot[eSlot];

	//printk("switch pad %d\n", ePAD);

	Hal_CARD_SetPADToPortPath(eIP, ePort, ePAD, FALSE);
	Hal_CARD_InitPADPin(ePAD, FALSE);

}

// Set Bus Voltage
//------------------------------------------------------------------------------------------------
static void _SetBusVdd(SlotEmType eSlot, U8_T u8Vdd)
{
	PADEmType ePAD    = ge_PADSlot[eSlot];

	/****** Simple Setting Here ******/
	Hal_CARD_SetPADPower(ePAD, (PADVddEmType)u8Vdd);

	/*if(u8Vdd == MMC_SIGNAL_VOLTAGE_330)
	{
		Hal_CARD_SetPADPower(ePAD, EV_NORVOL);
	}
	else if(u8Vdd == MMC_SIGNAL_VOLTAGE_180)
	{
		Hal_CARD_SetPADPower(ePAD, EV_MINVOL);
	}*/
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
		Hal_CARD_PullPADPin(ePAD, EV_SD_CMD_DAT0_3, EV_PULLDOWN, FALSE);
		Hal_CARD_PowerOff(ePAD, WT_POWEROFF); //For SD PAD

	}
    else if(u8PowerMode == MMC_POWER_UP)  // Power Up
    {
		Hal_SDMMC_Reset(eIP); // For SRAM Issue

		Hal_CARD_PullPADPin(ePAD, EV_SD_CMD_DAT0_3, EV_PULLUP, FALSE);
		Hal_CARD_PowerOn(ePAD, WT_POWERUP);
    }
	else if(u8PowerMode == MMC_POWER_ON) // Power On
	{
		Hal_SDMMC_ClkCtrl(eIP, TRUE, WT_POWERON);
	}

}

//------------------------------------------------------------------------------------------------
static U32_T _SetClock(SlotEmType eSlot, unsigned int u32ReffClk, U8_T u8PassLevel, U8_T u8DownLevel)
{
	U32_T u32RealClk =0 ;
	IPEmType eIP = ge_IPSlot[eSlot];

	u32RealClk = Hal_CARD_FindClockSetting(eIP, (U32_T)u32ReffClk, u8PassLevel, u8DownLevel);
	Hal_CARD_SetClock(eIP, u32RealClk);
	Hal_SDMMC_SetNcrDelay(eIP, u32RealClk);

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

	if(gb_RunDDRSDRSlot[eSlot])
	{
		switch(u8BusTiming)
		{
			case MMC_TIMING_LEGACY:
				Hal_CARD_SetHighSpeed(eIP, FALSE);
				break;
			case MMC_TIMING_MMC_HS:
			case MMC_TIMING_SD_HS:
				Hal_CARD_SetHighSpeed(eIP, TRUE);
				break;
		}

		Hal_SDMMC_SetDataSync(eIP, TRUE);
	}
	else
	{
		switch(u8BusTiming)
		{
			case MMC_TIMING_LEGACY:
				//printk("legacy speed\n");
				Hal_SDMMC_SetDataSync(eIP, FALSE);
				break;
			case MMC_TIMING_MMC_HS:
			case MMC_TIMING_SD_HS:
				//printk("high speed\n");
				Hal_SDMMC_SetDataSync(eIP, TRUE);
				break;
			default:
				//printk("other speed ?? %d\n", u8BusTiming);
				/****** For 300KHz IP Issue but not for Default Speed ******/
				Hal_SDMMC_SetDataSync(eIP, TRUE);
				break;
		}
	}

}

//------------------------------------------------------------------------------------------------
BOOL_T _GetCardDetect(SlotEmType eSlot)
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

	#if 0
		U32_T u32DiffTime = 0;
	#else
		struct timespec time_spec_curre;
		struct timespec time_spec_start;
		U32 u32WaitNs = u32WaitMs*1000000;
	#endif


	#if 0

		while(u32DiffTime < u32WaitMs)
		{
			mdelay(1);
			u32DiffTime++;

			bCurrPlugStatus = _GetCardDetect(eSlot);

	        if (bPrePlugStatus != bCurrPlugStatus)
	            break;
		}

	#else

		getnstimeofday(&time_spec_start);

		while(1)
		{
			bCurrPlugStatus = _GetCardDetect(eSlot);

	        if (bPrePlugStatus != bCurrPlugStatus)
	            break;

			getnstimeofday(&time_spec_curre);

			//printk("tv_sec = %d, tv_nsec = %9d\n", time_spec_curre.tv_sec, time_spec_curre.tv_nsec);

			if( time_spec_curre.tv_sec - time_spec_start.tv_sec >= 2 )
			{
				break;
			}
			else if( time_spec_curre.tv_sec - time_spec_start.tv_sec == 1 )
			{
				if( time_spec_curre.tv_nsec + (1000000000 - time_spec_start.tv_nsec) >= u32WaitNs )
				{
					break;
				}
 			}
			else
			{
				if( time_spec_curre.tv_nsec - time_spec_start.tv_nsec >= u32WaitNs )
				{
					break;
				}
			}
		}

		//printk("start: tv_sec = %d, tv_nsec = %9d\n", time_spec_start.tv_sec, time_spec_start.tv_nsec);
		//printk("curre: tv_sec = %d, tv_nsec = %9d\n", time_spec_curre.tv_sec, time_spec_curre.tv_nsec);

	#endif

    return bCurrPlugStatus;
}

#ifdef FCIE_V5_ADMA
extern void Chip_Clean_Cache_Range_VA_PA(unsigned long u32VAddr,unsigned long u32PAddr,unsigned long u32Size);

static U16_T _PreDataBufferMapping(TransEmType eTransType, struct mmc_data *data, struct ms_sdmmc_slot *sdmmchost, volatile U64_T *pu64BusAddr)
{
    struct scatterlist *p_sg_list = 0;
	u8 u8Dir = 0;
	dma_addr_t dmaaddr          = 0;
   	U32 u32_dma_addr            = 0;
	u32 dmalen                  = 0;
	int i,idx;


	if(data->flags & MMC_DATA_READ)
		u8Dir = DMA_FROM_DEVICE;
	else
		u8Dir = DMA_TO_DEVICE;


	p_sg_list = data->sg;

	 memset(gSDAdmaDesc, 0, sizeof(struct _AdmaDescriptor)*FCIE_ADMA_DESC_COUNT);
        dma_map_sg(NULL, p_sg_list, data->sg_len, u8Dir);

    //printk("sg_len:%x\n",data->sg_len);
	idx=0;
    for(i=0; i < data->sg_len; i++)
    {
        dmaaddr = sg_dma_address(p_sg_list);
        dmalen = sg_dma_len(p_sg_list);
		if (dmalen==0) continue;
	//printk("dmaaddr:%x\n",dmaaddr);
	//printk("dmalen:%x\n",dmalen);

        #ifdef MSTAR_MIU2_BUS_BASE
        if( dmaaddr >= MSTAR_MIU2_BUS_BASE) // MIU2
        {
            dmaaddr -= MSTAR_MIU2_BUS_BASE;
            gSDAdmaDesc[idx].u32_MiuSel = 2;
        }
        else
        #endif
        if( dmaaddr >= MSTAR_MIU1_BUS_BASE) // MIU1
        {
            dmaaddr -= MSTAR_MIU1_BUS_BASE;
            gSDAdmaDesc[idx].u32_MiuSel = 1;
        }
        else // MIU0
        {
            dmaaddr -= MSTAR_MIU0_BUS_BASE;
            gSDAdmaDesc[idx].u32_MiuSel = 0;
        }

        gSDAdmaDesc[idx].u32_Address = dmaaddr;
        gSDAdmaDesc[idx].u32_DmaLen = dmalen;
        gSDAdmaDesc[idx].u32_JobCnt = (dmalen / data->blksz );
		idx++;
        p_sg_list = sg_next(p_sg_list);
    }

 //   gSDAdmaDesc[data->sg_len-1].u32_End = 1;
    gSDAdmaDesc[idx-1].u32_End = 1;

    Chip_Clean_Cache_Range_VA_PA((u32)gSDAdmaDesc,
                                 (u32)virt_to_phys(gSDAdmaDesc),
                                 sizeof(struct _AdmaDescriptor)*data->sg_len);

	u32_dma_addr = (u32)virt_to_phys(gSDAdmaDesc);
//    u32_dma_addr = Hal_CARD_TransMIUAddr(0 , u32_dma_addr);
    pu64BusAddr[0]= u32_dma_addr;

	return 1;


}
#else
//------------------------------------------------------------------------------------------------
static U16_T _PreDataBufferMapping(TransEmType eTransType, struct mmc_data *data, struct ms_sdmmc_slot *sdmmchost, volatile U64_T *pu64BusAddr)
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
			p_sg_list->dma_address = dma_map_page(mmc_dev(sdmmchost->mmc), sg_page(p_sg_list), p_sg_list->offset, p_sg_list->length, u8Dir);
			if((p_sg_list->dma_address==0) || (p_sg_list->dma_address==~0))  //Mapping Error!
				return 0;

			pu64BusAddr[u16sg_idx] = p_sg_list->dma_address;
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

			pu64BusAddr[0] = (U32_T) sdmmchost->dma_phy_addr;
		}

#endif

	}

	/*else if(eTransType == EV_CIF)
	{
		p_sg_list = &data->sg[0];
		pu32AddrArr[0] = (U32_T)( page_address(sg_page(p_sg_list)) + p_sg_list->offset );
	}
	*/

	return 1;
}
#endif
#ifdef FCIE_V5_ADMA
static void _PostDataBufferMapping(TransEmType eTransType, struct mmc_data *data, struct ms_sdmmc_slot *sdmmchost)
{
	u8	u8Dir = 0;
#ifdef DEBUG_FCIE5_ADMA
   	U32 dmaaddr            = 0;
	u32 dmalen                  = 0;
       struct scatterlist *p_sg_list = 0;
	u8 *ptr;
	int i;
#endif

	if(data->flags & MMC_DATA_READ)
		u8Dir = DMA_FROM_DEVICE;
	else
		u8Dir = DMA_TO_DEVICE;
#ifdef DEBUG_FCIE5_ADMA
	p_sg_list = data->sg;
        dmaaddr = sg_dma_address(p_sg_list);
        dmalen = sg_dma_len(p_sg_list);
	ptr=(u8*)phys_to_virt(dmaaddr);
	printk("ADDR:%x  -->\n",dmaaddr);

	for (i=0; i < dmalen ; i++)
		printk("%x ",*ptr++);

	printk("\n");
#endif

	        dma_unmap_sg(NULL, data->sg, (int)data->sg_len, u8Dir);

}
#else
//------------------------------------------------------------------------------------------------
static void _PostDataBufferMapping(TransEmType eTransType, struct mmc_data *data, struct ms_sdmmc_slot *sdmmchost)
{
	struct scatterlist *p_sg_list = 0;
	u32 u16sg_idx = 0;
	u8	u8Dir = 0;

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
		dma_unmap_page(mmc_dev(sdmmchost->mmc), p_sg_list->dma_address, p_sg_list->length, u8Dir);
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
#endif

static void ms_sdmmc_dump_mem_line(char *buf, int cnt)
{
	int i;
	for(i = 0; i< cnt; i ++)
		printk("%02X ", (unsigned char)buf[i]);
	printk("|");
	for(i = 0; i< cnt; i ++)
		printk("%c", (buf[i] >= 32 && buf[i] < 128) ? buf[i] : '.');

	printk("\n");
}

void ms_sdmmc_dump_mem(void * buf, unsigned int count)
{
	unsigned int i;
	unsigned char * uc_pointer = NULL;

	uc_pointer = (unsigned char *)buf;

	printk("ms_sdmmc_dump_mem(0x%p, %d)\n", buf, count);

	for(i = 0; i < count; i +=16)
	{
		printk("0x%p: ", uc_pointer+i);
		ms_sdmmc_dump_mem_line(uc_pointer + i, (count<16)?count:16);
		if(i%512==496) printk("\n");
	}
}

void ms_sdmmc_dump_data(struct mmc_data * pData)
{
	U32 i, u32BusAddr, u32DmaLeng;
	struct scatterlist  *pScatterList = 0;

	for(i=0; i<pData->sg_len; i++) {

		pScatterList = &(pData->sg[i]);
		u32BusAddr = sg_dma_address(pScatterList);
		u32DmaLeng = sg_dma_len(pScatterList);

		#if 0
			// 64 bit kernel get 0 in sg_dma_len(), don't know why
			ms_sdmmc_dump_mem(phys_to_virt(u32BusAddr), u32DmaLeng);
		#else
			if(pData->sg_len==1)
			{
				//printk(YELLOW"length = %d\n"NONE, pScatterList->length);
				ms_sdmmc_dump_mem(phys_to_virt(u32BusAddr), pScatterList->length);
			}
			else
			{
				//printk(YELLOW"sg_dma_length = %d\n"NONE, u32DmaLeng);
				ms_sdmmc_dump_mem(phys_to_virt(u32BusAddr), u32DmaLeng);
			}
		#endif
	}
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
static int _RequestErrProcess(CmdEmType eCmdType, RspErrEmType eErrType, struct ms_sdmmc_slot *p_sdmmc_slot, struct mmc_data *data)
{
	int nErr = 0;
	BOOL_T bNeedDownLevel = FALSE;
	SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;

	if(eCmdType != EV_CMDRSP)
	{
		if( eErrType == EV_STS_OK )
			{
			    p_sdmmc_slot->currTimeoutCnt = 0;
			}
		else if(eErrType == EV_STS_MIE_TOUT)
		{
			p_sdmmc_slot->currTimeoutCnt++;
		}

		if(p_sdmmc_slot->currTimeoutCnt>=2)
		{
			p_sdmmc_slot->currTimeoutCnt =0;
			p_sdmmc_slot->bad_card = TRUE;
		}
	}

	if( eErrType == EV_STS_OK )
	{
		pr_sd_main("\n");
		return nErr;
	}
	else
	{
		//printk("=> (Err: 0x%04X)", (U16_T)eErrType);
		nErr = (U32_T) eErrType;

		if(eCmdType != EV_CMDRSP)
		{
			switch( (U16_T)eErrType )
			{
				case EV_STS_NORSP:
				case EV_STS_RIU_ERR:
				case EV_STS_MIE_TOUT:
				case EV_STS_FIFO_NRDY:
				case EV_STS_DAT0_BUSY:
					nErr = -ETIMEDOUT;
					break;

				case EV_STS_DAT_CERR:
				case EV_STS_DAT_STSERR:
				case EV_STS_DAT_STSNEG:
				case EV_STS_RSP_CERR:
					nErr = -EILSEQ;
					bNeedDownLevel = TRUE;
					break;
			}
		}

	}

	/****** Special Error Process for Stop Wait Process ******/
	if(eErrType == EV_SWPROC_ERR && data && EN_SDMMC_NOCDZ_NDERR)
	{
		data->bytes_xfered = data->blksz * data->blocks;
		nErr = 0;
		pr_sd_main("_Pass");
	}

	pr_sd_main("\n");

	/****** (2) Downgrad Clock Speed for Some Bus Errors ******/
	if(bNeedDownLevel && gb_DownLVLSlot[eSlot])
	{
		if( (p_sdmmc_slot->currDownLevel+1) <= gu32_MaxDLVLSlot[eSlot])
		{
			p_sdmmc_slot->currDownLevel++;
		}
		p_sdmmc_slot->currRealClk = _SetClock(eSlot, p_sdmmc_slot->currClk, gu32_PassLVLSlot[eSlot], p_sdmmc_slot->currDownLevel);
		printk(">> [sdmmc_%u] Downgrade Clk from (Clk=%u)=> (Real=%u)\n", eSlot, p_sdmmc_slot->currClk, p_sdmmc_slot->currRealClk);
	}

	return nErr;

}

#if !defined(EN_SDMMC_CDZ_POLLING)
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
	    gb_card_chg[eSlot] = TRUE;
		tasklet_schedule(&p_sdmmc_slot->hotplug_tasklet);
		irq_t = IRQ_HANDLED;
	}

	return irq_t;
}
#endif
/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_hotplug
 *     @author jeremy.wang (2012/1/5)
 * Desc: Hotplug function for Card Detection
 *
 * @param data : ms_sdmmc_slot struct pointer
 ----------------------------------------------------------------------------------------------------------*/
#if defined(EN_SDMMC_CDZ_POLLING)
int ms_sdmmc_hotplug(void *data)
{
	struct ms_sdmmc_slot  *p_sdmmc_slot = (struct ms_sdmmc_slot  *) data;
	SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;
	IPEmType eIP = ge_IPSlot[eSlot];

	while (1)
	{
		LABEL_LOOP_HOTPLUG:

		if( _GetCardDetect(eSlot) ) // Insert (CDZ)
		{
			if (p_sdmmc_slot->card_det == FALSE)
			{
				//printk("sd plug in\n");
				if( (FALSE) == _CardDetect_PlugDebounce(eSlot, WT_DB_PLUG, TRUE) )
					goto LABEL_LOOP_HOTPLUG;

				Hal_SDMMC_WaitProcessCtrl(eIP, FALSE);
				mmc_detect_change(p_sdmmc_slot->mmc, msecs_to_jiffies(WT_DB_SW_PLUG));
				printk("\n>> [sdmmc_%u] ##########....(Inserted) OK! \n", eSlot);
			}
			p_sdmmc_slot->card_det = TRUE;
			Hal_CARD_SetGPIOIntAttr((GPIOEmType)eSlot, EV_GPIO_OPT3);
		}
		else // Remove (CDZ)
		{
			if (p_sdmmc_slot->card_det == TRUE)
			{
				//printk("sd plug out\n");

				if (p_sdmmc_slot->mmc->card)
					p_sdmmc_slot->mmc->card->state |= MMC_CARD_REMOVED;

				Hal_SDMMC_WaitProcessCtrl(eIP, TRUE);
				mmc_detect_change(p_sdmmc_slot->mmc, msecs_to_jiffies(WT_DB_SW_UNPLUG));
				printk("\n>> [sdmmc_%u] ###########...(Ejected) OK!\n", eSlot);
			}
			p_sdmmc_slot->card_det = FALSE;
			p_sdmmc_slot->bad_card = FALSE;
			p_sdmmc_slot->rca = 0;
			Hal_CARD_SetGPIOIntAttr((GPIOEmType)eSlot, EV_GPIO_OPT4);
		}

		//  if(!gb_ShareCDZIRQSlot[eSlot])
		//enable_irq(p_sdmmc_slot->cdzIRQNo); //Always Run
		msleep(50);

	}
}

#else
static void ms_sdmmc_hotplug(unsigned long data)
{
	struct ms_sdmmc_slot  *p_sdmmc_slot = (struct ms_sdmmc_slot  *) data;
	SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;
	IPEmType eIP = ge_IPSlot[eSlot];

LABEL_LOOP_HOTPLUG:

	if( _GetCardDetect(eSlot) ) // Insert (CDZ)
	{
	    if (p_sdmmc_slot->card_det == FALSE)
	    {
		    if( (FALSE) == _CardDetect_PlugDebounce(eSlot, WT_DB_PLUG, TRUE) )
			     goto LABEL_LOOP_HOTPLUG;

            Hal_SDMMC_WaitProcessCtrl(eIP, FALSE);
            mmc_detect_change(p_sdmmc_slot->mmc, msecs_to_jiffies(WT_DB_SW_PLUG));
            printk("\n>> [sdmmc_%u] ##########....(Inserted) OK! \n", eSlot);
	    }
	    p_sdmmc_slot->card_det = TRUE;
		Hal_CARD_SetGPIOIntAttr((GPIOEmType)eSlot, EV_GPIO_OPT3);
	}
	else // Remove (CDZ)
	{
	    if (p_sdmmc_slot->card_det == TRUE)
	    {
		    if( (TRUE) == _CardDetect_PlugDebounce(eSlot, WT_DB_UNPLUG, FALSE) )
                goto LABEL_LOOP_HOTPLUG;


			if (p_sdmmc_slot->mmc->card)
                p_sdmmc_slot->mmc->card->state |= MMC_CARD_REMOVED;
            Hal_SDMMC_WaitProcessCtrl(eIP, TRUE);
            mmc_detect_change(p_sdmmc_slot->mmc, msecs_to_jiffies(WT_DB_SW_UNPLUG));
            printk("\n>> [sdmmc_%u] ###########...(Ejected) OK!\n", eSlot);
	    }
		p_sdmmc_slot->card_det = FALSE;
		p_sdmmc_slot->bad_card = FALSE;
		Hal_CARD_SetGPIOIntAttr((GPIOEmType)eSlot, EV_GPIO_OPT4);
	}

    if(!gb_ShareCDZIRQSlot[eSlot])
    	enable_irq(p_sdmmc_slot->cdzIRQNo); //Always Run

}

#endif
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
static int ms_sdmmc_disable(struct mmc_host *p_mmc_host)
{
	gb_RejectSuspend = FALSE;
	return 0;
}
#ifdef  SDIO30
extern void GoSDR50(int scan);
extern void GoDDR50(int scan);

extern int gScan;
U8_T  skew[18];
int  FindBestSkew(U8_T *skew)
{
	U8_T count;
	int first=0 , best,i;

	count=0;
	for (i=0 ; i < 18 ; i++)
	{
		if (skew[i]) count++;
	}
	if (skew[0]==1)
	{
	   first=0;
	    for (i=17 ; i >=0 ; i--)
	    {
	    	 if (skew[i]==1)	first=i;


		 if (skew[i]==0)    break;
	    }
		best=first+(count/2) ;
		if (best > 17 ) best-=18;

	}
	else		//skew[0]==0
	{

		for (i=0 ; i < 18 ; i++)
		{
			if (skew[i]==1) break;
		}
		first=i;
		best=first+count/2;
	}
	return best;
}
static int sd_execute_tuning(struct mmc_host *mmc, u32 opcode)
{
	struct ms_sdmmc_slot   *p_sdmmc_slot  = mmc_priv(mmc);
	SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;
	IPEmType eIP = ge_IPSlot[eSlot];
	U8_T u8CMD = 0;
	U8_T  Pattern[64];
	int i,j,min,max;
	dma_addr_t dma_addr;
	BOOL_T	DataErr;
	RspErrEmType eErr = EV_STS_OK;

	printk("sd execute tuning:%x  %x\n",opcode,eSlot);

	if (p_sdmmc_slot->currRealClk < CLK_10) 		//50M and below ,no need to tuning
	{
		gScan=0;
		return 0;
	}

	_CRIT_SECT_BEGIN(eSlot);

	_SwitchPAD(eSlot);

	Hal_CARD_SetClock(eIP, p_sdmmc_slot->currRealClk);

	min=255;

	for (j=0 ; j <=0x11 ; j++)
	{
//#if (SDBUS==DDR50)
	//  GoDDR50(j);
//#else
	  GoSDR50(j);
//#endif
	  Hal_SDMMC_SetCmdTokenToCIFC(eIP, opcode, 0); // fill CIFC
	  dma_addr = dma_map_single(NULL, (void*)Pattern, 64, DMA_FROM_DEVICE);

	  Hal_SDMMC_TransCmdSetting(eIP, EV_DMA, EV_CMDREAD, 1, 64, Hal_CARD_TransMIUAddr(eIP, dma_addr));

      eErr=Hal_SDMMC_SendCmdAndWaitProcess(eIP, EV_DMA, EV_CMDREAD, EV_R1, TRUE, FALSE,FALSE);

	  dma_unmap_single(NULL, dma_addr, 64, DMA_FROM_DEVICE);

	  if (eErr & 0x1f)
	  {
	  	//printk("status:%x\n",eErr);
		//continue;
		skew[j]=0;
	  }
	  else
	  {
	  	//printk("pass\n");
	  	skew[j]=1;
	  }



	}
	gScan=FindBestSkew(skew);
	printk("select skew:%x\n",gScan);
	_CRIT_SECT_END(eSlot);

  return 0;
}
#endif

void ms_sdmmc_send_cmd13(IPEmType eIP, struct ms_sdmmc_slot * p_sdmmc_slot)
{
	unsigned int card_status;
	RspStruct * eRspSt;
	RspErrEmType eErr = EV_STS_OK;

	Hal_SDMMC_SetCmdTokenToCIFC(eIP, 13, p_sdmmc_slot->rca); // fill CIFC
	Hal_SDMMC_SendCmdAndWaitProcess(eIP, EV_EMP, EV_CMDRSP, EV_R1, TRUE, FALSE,FALSE);
	eRspSt = Hal_SDMMC_GetRspTokenFromCIFC(eIP, eErr);
	card_status = _TransArrToUInt(eRspSt->u8ArrRspToken[1], eRspSt->u8ArrRspToken[2], eRspSt->u8ArrRspToken[3], eRspSt->u8ArrRspToken[4]);
	printk(LIGHT_PURPLE"C13[%08Xh] RSP: %02Xh_%08Xh(%u)\n"NONE, p_sdmmc_slot->rca, eRspSt->u8ArrRspToken[0], card_status, (card_status&0x1E00)>>9);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_request
 *     @author jeremy.wang (2011/5/19)
 * Desc: Request funciton for any commmand
 *
 * @param p_mmc_host : mmc_host structure pointer
 * @param p_mmc_req :  mmc_request structure pointer
 ----------------------------------------------------------------------------------------------------------*/
static void ms_sdmmc_request(struct mmc_host *p_mmc_host, struct mmc_request * p_mmc_req)
{
	struct ms_sdmmc_slot *p_sdmmc_slot  = mmc_priv(p_mmc_host);
	struct mmc_command *cmd = p_mmc_req->cmd;
	struct mmc_command *stop = p_mmc_req->stop;
	struct mmc_data *data = p_mmc_req->data;

	RspStruct * eRspSt;
	RspErrEmType eErr = EV_STS_OK;
	TransEmType eTransType = EV_DMA;
	CmdEmType eCmdType = EV_CMDRSP;
	SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;
	IPEmType eIP = ge_IPSlot[eSlot];
	volatile U64_T au64BusAddr[MAX_SEG_CNT];

	BOOL_T bCloseClock = FALSE,auto_stop_clk,adma_flag=FALSE;
	U8_T u8CMD = 0;
	U16_T u16BlkSize = 0, u16BlkCnt = 0, u16SubBlkCnt = 0;
	U32_T u32Arg = 0, u32SubLen = 0;
	U16_T u16ProcCnt = 0, u16Idx= 0;
	#if (defined SDIO_D1_INTR_MODE) && (SDIO_D1_INTR_MODE == SDIO_D1_INTR_MODE_SW) // enable D1 in software mode
	U8_T u8IsThisReqMode3 = 0;
	#endif

	if (p_sdmmc_slot->bad_card==TRUE)
	{
		printk("bad card reject request CMD%02d_%08Xh\n", p_mmc_req->cmd->opcode, p_mmc_req->cmd->arg);
		cmd->error = -EIO;
		cmd->retries = 0;
		mmc_request_done(p_mmc_host, p_mmc_req);
		return;
	}

	_CRIT_SECT_BEGIN(eSlot);

	#if (defined SDIO_D1_INTR_MODE) && (SDIO_D1_INTR_MODE == SDIO_D1_INTR_MODE_SW) // enable D1 in software mode

		if(p_mmc_req->data)
		{
			if(p_mmc_req->data->blocks==1)
			{
				HalSdio_SetIntMode(eIP, 2);
			}
			else
			{
				HalSdio_SetIntMode(eIP, 3);
				u8IsThisReqMode3 = 1;
			}
		}
		else
		{
			HalSdio_SetIntMode(eIP, 0);
		}

	#endif

	#if 0 //defined PRINT_CMD_ARG_RSP

	if(p_sdmmc_slot->slotNo==0) // SDIO
	{
		if(!p_mmc_req->data)
			printk(LIGHT_RED"MRQ --> CMD%02d_%08Xh\n"NONE, p_mmc_req->cmd->opcode, p_mmc_req->cmd->arg);
		else
			printk(LIGHT_RED"MRQ --> CMD%02d_%08Xh_%dx%d\n"NONE, p_mmc_req->cmd->opcode, p_mmc_req->cmd->arg, p_mmc_req->data->blocks, p_mmc_req->data->blksz);
	}
	else if(p_sdmmc_slot->slotNo==1) // FCIE
	{
		if(!p_mmc_req->data)
			printk(YELLOW"MRQ --> CMD%02d_%08Xh\n"NONE, p_mmc_req->cmd->opcode, p_mmc_req->cmd->arg);
		else
			printk(YELLOW"MRQ --> CMD%02d_%08Xh_%dx%d\n"NONE, p_mmc_req->cmd->opcode, p_mmc_req->cmd->arg, p_mmc_req->data->blocks, p_mmc_req->data->blksz);
	}

	#endif

	/****** Speed up during non-card *******/
	//if(!_GetCardDetect(eSlot) )

	if(!p_sdmmc_slot->mmc->card)
	   Hal_SDMMC_WaitProcessCtrl(eIP, FALSE);

	u8CMD =(U8_T)cmd->opcode;
	u32Arg = (U32_T)cmd->arg;

	//if(Hal_SDMMC_OtherPreUse(eIP) || gb_AlwaysSwitchPAD)
	_SwitchPAD(eSlot);

	Hal_CARD_SetClock(eIP, p_sdmmc_slot->currRealClk); // set clock before each request

	//printk(">> [sdmmc_%u] CMD_%u (0x%08X)\n", eSlot, u8CMD, u32Arg);

	Hal_SDMMC_SetCmdTokenToCIFC(eIP, u8CMD, u32Arg); // fill CIFC

	/****** Simple SD command *******/
	if(!data)
	{
		if(u8CMD==11)
			eErr = Hal_SDMMC_SendCmdAndWaitProcess(eIP, EV_EMP, EV_CMDRSP, _TransRspType(mmc_resp_type(cmd)), FALSE, FALSE,FALSE);
		else
			eErr = Hal_SDMMC_SendCmdAndWaitProcess(eIP, EV_EMP, EV_CMDRSP, _TransRspType(mmc_resp_type(cmd)), TRUE, FALSE,FALSE);
	}
	else // R/W SD Command
	{
		u16BlkSize = (U16_T)data->blksz;
		u16BlkCnt  = (U16_T)data->blocks;
		u32SubLen = (U32_T) data->sg[0].length;
		u16SubBlkCnt = (U16_T)(u32SubLen/u16BlkSize);
		data->bytes_xfered =0;

		#ifdef PRINT_CMD_ARG_RSP
			if(p_sdmmc_slot->slotNo==0) // SDIO
			{
				printk(LIGHT_RED"_%d"NONE, u16SubBlkCnt);
			}
			else
			{
				printk(YELLOW"_%d"NONE, u16SubBlkCnt);
			}
		#endif

		pr_sd_main("__[Sgl: %u] (TB: %u)(BSz: %u)", (U16_T)data->sg_len, u16BlkCnt, u16BlkSize);

		//	if((u16BlkCnt == 1) && (u16BlkSize<=512))
			//eTransType = EV_CIF;
		//else
			//eTransType = EV_DMA;

		if(data->flags & MMC_DATA_READ)
		{
			//printk("read\n");
			eCmdType = EV_CMDREAD;
		}
		else
		{
			//printk("write\n");
			eCmdType = EV_CMDWRITE;
		}

		if(stop)
			bCloseClock = FALSE;
		else
			bCloseClock = TRUE;

		//printk("before pre data buf map %08Xh, %08Xh\n", au32Addr[0], au32Addr[1]);
		u16ProcCnt = _PreDataBufferMapping(eTransType, data, p_sdmmc_slot, au64BusAddr);
		//printk("after pre data buf map %08Xh, %08Xh\n", au32Addr[0], au32Addr[1]);
		if(u16ProcCnt==0)
		{
			pr_err("\n>> [sdmmc_%u] Err: DMA Mapping Addr Error!\n", eSlot);
			eErr = EV_OTHER_ERR;
			goto LABEL_SD_ERR;
		}

		// add to test WiFi module
		if (u16BlkCnt==1)	auto_stop_clk = FALSE;
		else				auto_stop_clk = TRUE;

		#ifdef FCIE_V5_ADMA
		auto_stop_clk = FALSE;
		adma_flag=TRUE;
		#endif

		//if( (eTransType == EV_CIF) && (eCmdType == EV_CMDWRITE) )
			//Hal_SDMMC_CIFD_DATA_IO(eIP, EV_CMDWRITE, u16BlkSize, (volatile U16_T *)au32Addr[0]);

		Hal_SDMMC_TransCmdSetting(eIP, eTransType, eCmdType, u16SubBlkCnt, u16BlkSize, Hal_CARD_TransMIUAddr(eIP, au64BusAddr[0]));

		eErr = Hal_SDMMC_SendCmdAndWaitProcess(eIP, eTransType, eCmdType, _TransRspType(mmc_resp_type(cmd)), bCloseClock, auto_stop_clk,adma_flag);

		pr_sd_dbg("\n____[0] =>> (SBCnt: %u)__[Addr: 0x%08llX]", u16SubBlkCnt, au64BusAddr[0]);

		if( ((U16_T)eErr) == EV_STS_OK )
		{
			data->bytes_xfered += u32SubLen;

			/****** Broken DMA *******/
			for(u16Idx=1 ; u16Idx<u16ProcCnt; u16Idx++)
			{
				u32SubLen = (U32_T) data->sg[u16Idx].length;
				u16SubBlkCnt = (U16_T)(u32SubLen/u16BlkSize);
				pr_sd_dbg("\n____[%u] =>> (SB: %u)__[Addr: 0x%08llX]", u16Idx, u16SubBlkCnt, au64BusAddr[u16Idx]);

				Hal_SDMMC_TransCmdSetting(eIP, eTransType, eCmdType, u16SubBlkCnt, u16BlkSize, Hal_CARD_TransMIUAddr(eIP, au64BusAddr[u16Idx]));

				eErr = Hal_SDMMC_RunBrokenDmaAndWaitProcess(eIP, eCmdType);

				if((U16_T)eErr) break;
				data->bytes_xfered += u32SubLen;
			}
		}
		data->bytes_xfered = u16BlkSize*u16BlkCnt;

		//ms_sdmmc_dump_data(data);

		//	if( (eTransType == EV_CIF) && (eCmdType == EV_CMDREAD) )
			//Hal_SDMMC_CIFD_DATA_IO(eIP, EV_CMDREAD, u16BlkSize, (volatile U16_T *)au32Addr[0]);

		_PostDataBufferMapping(eTransType, data, p_sdmmc_slot);

	}


LABEL_SD_ERR:

	cmd->error = _RequestErrProcess(eCmdType, eErr, p_sdmmc_slot, data);
	if(data)
		data->error = cmd->error;

	eRspSt = Hal_SDMMC_GetRspTokenFromCIFC(eIP, eErr); // get response back here
	cmd->resp[0] = _TransArrToUInt(eRspSt->u8ArrRspToken[1], eRspSt->u8ArrRspToken[2], eRspSt->u8ArrRspToken[3], eRspSt->u8ArrRspToken[4]);
	if(eRspSt->u8RspSize == 0x10)
	{
		cmd->resp[1] = _TransArrToUInt(eRspSt->u8ArrRspToken[5], eRspSt->u8ArrRspToken[6], eRspSt->u8ArrRspToken[7], eRspSt->u8ArrRspToken[8]);
		cmd->resp[2] = _TransArrToUInt(eRspSt->u8ArrRspToken[9], eRspSt->u8ArrRspToken[10], eRspSt->u8ArrRspToken[11], eRspSt->u8ArrRspToken[12]);
		cmd->resp[3] = _TransArrToUInt(eRspSt->u8ArrRspToken[13], eRspSt->u8ArrRspToken[14], eRspSt->u8ArrRspToken[15], 0);
	}

	#if (defined RSP_CRC_1_BIT_SHIFT_CHECK) && (RSP_CRC_1_BIT_SHIFT_CHECK==1)
	if( (!eErr) && (p_mmc_req->cmd->flags&MMC_RSP_PRESENT) )
	{
		if(p_mmc_req->cmd->flags&MMC_RSP_OPCODE)
		{
			if(eRspSt->u8ArrRspToken[0]!=p_mmc_req->cmd->opcode)
			{
				printk("opcode not checked\n");
				p_mmc_req->cmd->error = -ETIMEDOUT;
			}
		}
		else
		{
			if(eRspSt->u8ArrRspToken[0]!=0x3F)
			{
				printk("reserved bits not checked\n");
				p_mmc_req->cmd->error = -ETIMEDOUT;
			}
		}
	}
	#endif

	if(cmd->opcode==SD_SEND_RELATIVE_ADDR)
	{
		//printk(LIGHT_PURPLE"record rca %04Xh\n"NONE, cmd->resp[0]&0xFFFF0000);
		p_sdmmc_slot->rca = cmd->resp[0]&0xFFFF0000;
	}


	if(cmd->error)
	{
		if( cmd->opcode!=52 && cmd->opcode!=5 ) // skip SDIO command
			printk("SDREQ CMD%d_%08X Fail: %04Xh\n", cmd->opcode, cmd->arg, eErr);
	}

	if(data && data->error)
	{
		pr_sd_err("\n>> [sdmmc_%u] Err: #Cmd: %u =>(E: 0x%04X)__(L:%u)(B:%u/%u)(I:%u/%u)\n\n", \
				  eSlot, u8CMD, (U16_T)eErr, eRspSt->u32ErrLine, u16SubBlkCnt, u16BlkCnt, u16Idx, u16ProcCnt);

		//ms_sdmmc_dump_data(data);
		//panic("sd die");
	}

	if(cmd->error && data)
	{
		if(p_sdmmc_slot->rca && cmd->opcode!=55)
		{
			ms_sdmmc_send_cmd13(eIP, p_sdmmc_slot);
		}
	}

	/****** Send Stop Cmd ******/
	if(stop)
	{
		u8CMD = (U8_T)stop->opcode;
		u32Arg = (U32_T)stop->arg;
		pr_sd_main(">> [sdmmc_%u]_CMD_%u (0x%08X)", eSlot, u8CMD, u32Arg);

		Hal_SDMMC_SetCmdTokenToCIFC(eIP, u8CMD, u32Arg);

		eErr = Hal_SDMMC_SendCmdAndWaitProcess(eIP, EV_EMP, EV_CMDRSP, _TransRspType(mmc_resp_type(cmd)), TRUE, FALSE,FALSE);
		stop->error = _RequestErrProcess(EV_CMDRSP,eErr, p_sdmmc_slot, data);

		eRspSt = Hal_SDMMC_GetRspTokenFromCIFC(eIP, eErr);
		stop->resp[0] = _TransArrToUInt(eRspSt->u8ArrRspToken[1], eRspSt->u8ArrRspToken[2], eRspSt->u8ArrRspToken[3], eRspSt->u8ArrRspToken[4]);
	}

    /*if(_REG_WaitDat0HI(eIP, 3000)!=EV_OK)
    {
        pr_sd_err("[sdmmc_%u] wait D0 high time out!\r\n", eSlot);
    }*/

	//Hal_CARD_SetClock(eIP, p_sdmmc_slot->pmrsaveClk); // For Power Saving

	#if (defined SDIO_D1_INTR_MODE) && (SDIO_D1_INTR_MODE == SDIO_D1_INTR_MODE_SW) // enable D1 in software mode
		if(u8IsThisReqMode3)
		{
			HalSdio_SetIntMode(eIP, 0); // need to set back in SW SDIO interrupt mode
		}
	#endif

	if(cmd->error && data)
	{
		if(p_sdmmc_slot->rca && cmd->opcode!=55 && stop)
		{
			ms_sdmmc_send_cmd13(eIP, p_sdmmc_slot);
		}
	}

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

	//printk("p_mmc_ios->timing = %d\n", p_mmc_ios->timing);

    /*printk("\33[1;31m");
    printk("%s %s() %d", __FILE__, __FUNCTION__, __LINE__);
    printk("\33[m\r\n");*/
    //printk("\33[1;35m"); // magenta

    /****** Clock Setting*******/
	if(p_sdmmc_slot->currClk != p_mmc_ios->clock)
	{
		//printk("update current clock = %d\n", p_mmc_ios->clock);
		p_sdmmc_slot->currDownLevel = 0;
		p_sdmmc_slot->currClk = p_mmc_ios->clock;
		p_sdmmc_slot->currRealClk = _SetClock(eSlot, p_sdmmc_slot->currClk, 0, p_sdmmc_slot->currDownLevel);
		//	if(p_sdmmc_slot->currRealClk > 400000)
		//	printk(">> [sdmmc_%u] Set IOS => Clk=%u (Real=%u)\n", eSlot, p_sdmmc_slot->currClk, p_sdmmc_slot->currRealClk);
		if (p_sdmmc_slot->currRealClk <= 400000)
			_SetBusTiming(eSlot, 0xFF);
		else
			printk(">> [sdmmc_%u] Set IOS => Clk=%u (Real=%u)\n", eSlot, p_sdmmc_slot->currClk, p_sdmmc_slot->currRealClk);
	}

	/****** Bus Width Setting*******/
	if(p_sdmmc_slot->currWidth != p_mmc_ios->bus_width)
	{
		p_sdmmc_slot->currWidth = p_mmc_ios->bus_width;
		_SetBusWidth(eSlot, p_sdmmc_slot->currWidth);
		pr_sd_main(">> [sdmmc_%u] Set IOS => BusWidth=%u\n", eSlot, p_sdmmc_slot->currWidth);
	}

	/****** Bus Timing Setting*******/
	if(p_sdmmc_slot->currTiming != p_mmc_ios->timing)
	{
		//printk("update current timing = %d\n", p_mmc_ios->timing);
		p_sdmmc_slot->currTiming = p_mmc_ios->timing;
		_SetBusTiming(eSlot, p_sdmmc_slot->currTiming);
		pr_sd_main(">> [sdmmc_%u] Set IOS => BusTiming=%u\n", eSlot, p_sdmmc_slot->currTiming);
	}

	/****** Voltage Setting *******/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 8))
	if(p_sdmmc_slot->currVdd != p_mmc_ios->signal_voltage)
	{
		p_sdmmc_slot->currVdd = p_mmc_ios->signal_voltage;
		pr_sd_main(">> [sdmmc_%u] Set IOS => Voltage=%u\n", eSlot, p_sdmmc_slot->currVdd);
		_SetBusVdd(eSlot, p_sdmmc_slot->currVdd);
	}
#else
	_SetBusVdd(eSlot, EV_NORVOL);
#endif

    /****** Power Switch Setting *******/
	if(p_sdmmc_slot->currPowrMode != p_mmc_ios->power_mode)
	{
		p_sdmmc_slot->currPowrMode = p_mmc_ios->power_mode;
		pr_sd_main(">> [sdmmc_%u] Set IOS => Power=%u\n", eSlot, p_sdmmc_slot->currPowrMode);
		_SetPower(eSlot, p_sdmmc_slot->currPowrMode);
	}


	_CRIT_SECT_END(eSlot);

}
#ifdef SDBUS
static int ms_sdmmc_busy(struct mmc_host *mmc)
{

	return Hal_Check_Card_Pins();


}
static int ms_start_signal_voltage_switch(struct mmc_host *mmc,struct mmc_ios *ios)
{

	if (ios->signal_voltage==MMC_SIGNAL_VOLTAGE_180)
		sd_go_18v();

	return 0;
}
#endif
/*----------------------------------------------------------------------------------------------------------
 *
 * Function: msb2501_mci_get_ro
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

	if( _GetWriteProtect(eSlot) )	 //For CB2 HW Circuit, WP=>NWP
		p_sdmmc_slot->read_only = 1;
	else
		p_sdmmc_slot->read_only = 0;

	_CRIT_SECT_END(eSlot);

	printk(">> [sdmmc_%u] Get RO => (%d)\n", eSlot, p_sdmmc_slot->read_only);

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
	{
		p_sdmmc_slot->card_det = 1;
	}
	else
	{
		p_sdmmc_slot->card_det = 0;
	}

	_CRIT_SECT_END(eSlot);

	printk(">> [sdmmc_%u] Get CD => (%d)\n",eSlot, p_sdmmc_slot->card_det);

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
	/*struct ms_sdmmc_slot   *p_sdmmc_slot  = mmc_priv(p_mmc_host);
	SlotEmType eSlot = (SlotEmType)p_sdmmc_slot->slotNo;

	gb_SDIOSlot[eSlot] = TRUE;
	printk(">> [sdmmc_%u] Found SDIO Device (%d)\n", eSlot);*/

}

#if defined SDIO_D1_INTR_MODE && SDIO_D1_INTR_MODE

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

	if(enable)
	{
		HalSdio_InterruptCtrl(eIP, enable);
	}
	else
	{
		HalSdio_InterruptCtrl(eIP, enable);
	}
}

#endif

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

#if defined SDIO_D1_INTR_MODE && SDIO_D1_INTR_MODE
	.enable_sdio_irq = ms_sdmmc_enable_sdio_irq,
#endif

#ifdef SDBUS
	.card_busy	= ms_sdmmc_busy,
	.start_signal_voltage_switch	= ms_start_signal_voltage_switch,
	.execute_tuning			= sd_execute_tuning,
#endif

};

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_init_slot
 *     @author jeremy.wang (2011/12/21)
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
	#if defined(EN_SDMMC_CDZ_POLLING)
		struct task_struct *ts;
	#endif

    int nRet = 0;

	/****** (1) Allocte MMC and SDMMC host ******/
	p_mmc_host = mmc_alloc_host(sizeof(struct ms_sdmmc_slot), &p_sdmmc_host->pdev->dev);

    if (!p_mmc_host)
    {
		pr_err(">> [sdmmc_%u] Err: Failed to Allocate mmc_host!\n", slotNo);
        return -ENOMEM;
    }

	/****** (2) SDMMC host setting ******/
    p_sdmmc_slot                = mmc_priv(p_mmc_host);

#if (!EN_SDMMC_BRO_DMA)
	p_sdmmc_slot->dma_buffer 	= dma_alloc_coherent(NULL, MAX_BLK_COUNT * MAX_BLK_SIZE, &p_sdmmc_slot->dma_phy_addr, GFP_KERNEL);
    if (!p_sdmmc_slot->dma_buffer)
    {
		pr_err(">> [sdmmc_%u] Err: Failed to Allocate sdmmc_host DMA buffer\n", slotNo);
        nRet = -ENOMEM;
        goto LABEL_OUT2;
    }
#endif

	p_sdmmc_slot->mmc          = p_mmc_host;
	p_sdmmc_slot->slotNo       = slotNo;
	p_sdmmc_slot->pmrsaveClk   = Hal_CARD_FindClockSetting(eIP, 400000, 0, 0);
	p_sdmmc_slot->mieIRQNo	   = gu16_MieIntNoSlot[eSlot];
	p_sdmmc_slot->cdzIRQNo     = gu16_CdzIntNoSlot[eSlot];
	p_sdmmc_slot->cdzGPIONo    = Hal_CARD_GetGPIONum((GPIOEmType)slotNo);

	p_sdmmc_slot->currClk	   = 0;
	p_sdmmc_slot->currWidth    = 0;
	p_sdmmc_slot->currTiming   = 0;
	p_sdmmc_slot->currPowrMode = 0;
	p_sdmmc_slot->currVdd      = 0;
	p_sdmmc_slot->currDDR      = 0;
	p_sdmmc_slot->bad_card		= FALSE;

	/***** (3) MMC host setting ******/
    p_mmc_host->ops = &st_mmc_ops;
    p_mmc_host->f_min = p_sdmmc_slot->pmrsaveClk;
	p_mmc_host->f_max = gu32_MaxClkSlot[eSlot];
	//   p_mmc_host->f_max = 50000000;	//104M

    p_mmc_host->ocr_avail = MMC_VDD_32_33|MMC_VDD_31_32|MMC_VDD_30_31|MMC_VDD_29_30|MMC_VDD_28_29|MMC_VDD_27_28|MMC_VDD_165_195;

#ifdef SDBUS
#if (SDBUS == SDR50)
    p_mmc_host->caps = MMC_CAP_4_BIT_DATA|MMC_CAP_MMC_HIGHSPEED|MMC_CAP_SD_HIGHSPEED| MMC_CAP_UHS_SDR50  ;
#elif (SDBUS== SDR104)
    p_mmc_host->caps = MMC_CAP_4_BIT_DATA|MMC_CAP_MMC_HIGHSPEED|MMC_CAP_SD_HIGHSPEED| MMC_CAP_UHS_SDR104  ;
#elif (SDBUS==DDR50)
    p_mmc_host->caps = MMC_CAP_4_BIT_DATA|MMC_CAP_MMC_HIGHSPEED|MMC_CAP_SD_HIGHSPEED| MMC_CAP_UHS_DDR50  ;
#endif
#endif

#ifndef SDBUS
    p_mmc_host->caps = MMC_CAP_4_BIT_DATA|MMC_CAP_MMC_HIGHSPEED|MMC_CAP_SD_HIGHSPEED;
#endif
	if(!gb_HotplugSlot[eSlot])
		p_mmc_host->caps |= MMC_CAP_NONREMOVABLE;

#if defined SDIO_D1_INTR_MODE && SDIO_D1_INTR_MODE
	if(slotNo==0) // only SDIO support D1 interrupt
	{
		p_mmc_host->caps |= MMC_CAP_SDIO_IRQ; // enable SDIO IRQ
	}
#endif

#if (EN_SDMMC_BRO_DMA)
	p_mmc_host->max_blk_count  = MAX_BRO_BLK_COUNT;
#else
    p_mmc_host->max_blk_count  = MAX_BLK_COUNT;
#endif
    p_mmc_host->max_blk_size   = MAX_BLK_SIZE;

    p_mmc_host->max_req_size   = p_mmc_host->max_blk_count  * p_mmc_host->max_blk_size;
    p_mmc_host->max_seg_size   = p_mmc_host->max_req_size;

#ifndef FCIE_V5_ADMA
	p_mmc_host->max_segs       = 1;
#else
	p_mmc_host->max_segs       = MAX_SEG_CNT;
#endif

	p_sdmmc_host->sdmmc_slot[slotNo] = p_sdmmc_slot;

	/****** (4) IP Once Setting for Different Platform ******/
	Hal_CARD_IPOnceSetting(eIP);

	/****** (5) Ext CDZ Setting ******/
      Hal_CARD_InitGPIO((GPIOEmType)slotNo, !gb_FakeCDZSlot[eSlot]);
	/****** (6) Interrupt Source Setting ******/
	gst_IntSourceSlot[eSlot].slotNo = slotNo;
	gst_IntSourceSlot[eSlot].eIP = eIP;
	gst_IntSourceSlot[eSlot].eCardInt = EV_INT_SD;
	gst_IntSourceSlot[eSlot].sdio_host = p_sdmmc_slot;

	/****** (7) Register IP IRQ *******/
	if(EN_SDMMC_MIEINT_MODE)
	{
		nRet = request_irq(p_sdmmc_slot->mieIRQNo, Hal_CARD_INT_MIE, gu32_MIEIRQParaSlot[eSlot], DRIVER_NAME, &gst_IntSourceSlot[eSlot]);
		if (nRet)
		{
			pr_err(">> [sdmmc_%u] Err: Failed to request MIE Interrupt (%u)!\n", slotNo, p_sdmmc_slot->mieIRQNo);
			goto LABEL_OUT2;
		}

		if(EN_SDMMC_MIEINT_MODE)
			Hal_CARD_INT_MIEModeCtrl(eIP, EV_INT_SD, TRUE);
		else
			Hal_CARD_INT_MIEModeCtrl(eIP, EV_INT_SD, FALSE);

	}

	mmc_add_host(p_mmc_host);

	if(!gb_HotplugSlot[eSlot])
	{
		goto LABEL_OUT0;
	}

	#if defined(EN_SDMMC_CDZ_POLLING)

    	ts = kthread_run(ms_sdmmc_hotplug,  p_sdmmc_slot, "sdhotplugthread");

	#else

		/****** (8) Register Ext CDZ IRQ  *******/
		if(!p_sdmmc_slot->cdzIRQNo)
		{
			if(p_sdmmc_slot->cdzGPIONo)
			{
				p_sdmmc_slot->cdzIRQNo = gpio_to_irq(p_sdmmc_slot->cdzGPIONo);
			}
			else
			{
				p_sdmmc_slot->cdzIRQNo = 0;
			}
		}

		tasklet_init(&p_sdmmc_slot->hotplug_tasklet, ms_sdmmc_hotplug, (unsigned long)p_sdmmc_slot);

		if(p_sdmmc_slot->cdzIRQNo)
		{
			Hal_CARD_SetGPIOIntAttr((GPIOEmType)slotNo, EV_GPIO_OPT1);

			nRet = request_irq(p_sdmmc_slot->cdzIRQNo, ms_sdmmc_cdzint, gu32_CDZIRQParaSlot[eSlot], DRIVER_NAME, &gst_IntSourceSlot[eSlot]);
			if(nRet)
			{
				pr_err(">> [sdmmc_%u] Err: Failed to request CDZ Interrupt (%u)!\n", slotNo, p_sdmmc_slot->cdzIRQNo);
				goto LABEL_OUT1;
			}

			printk(">> [sdmmc_%u] CDZ use Ext GPIO IRQ: %u\n", slotNo, p_sdmmc_slot->cdzIRQNo);

			Hal_CARD_SetGPIOIntAttr((GPIOEmType)slotNo, EV_GPIO_OPT2);

			if (gb_WakeupCDZIRQSlot[eSlot])
			{
				#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 39))
					irq_set_irq_wake(p_sdmmc_slot->cdzIRQNo, TRUE);
				#else
					set_irq_wake(p_sdmmc_slot->cdzIRQNo, TRUE);
				#endif
			}

		}
		else
		{
			printk(">> [sdmmc_%u] CDZ IRQ doesn't exist!\n", slotNo);
			BUG_ON(1);
		}

	#endif // EN_SDMMC_CDZ_POLLING

LABEL_OUT0:

    return 0;

#if !defined(EN_SDMMC_CDZ_POLLING)
LABEL_OUT1:
	free_irq(p_sdmmc_slot->mieIRQNo, &gst_IntSourceSlot[eSlot]);
#endif

LABEL_OUT2:
	if (p_sdmmc_slot->dma_buffer)
		dma_free_coherent(NULL, MAX_BLK_COUNT * MAX_BLK_SIZE, p_sdmmc_slot->dma_buffer, p_sdmmc_slot->dma_phy_addr);

	mmc_free_host(p_mmc_host);

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
static int ms_sdmmc_probe(struct platform_device *p_dev)
{
	struct ms_sdmmc_host *p_sdmmc_host;
	unsigned int slotNo = 0;
	int ret = 0, tret = 0;

    /*printk("\33[1;33m");
    printk("%s %s() %d", __FILE__, __FUNCTION__, __LINE__);
    printk("\33[m\r\n");*/

	//HalFcie_DumpRegister();

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


#if (EN_SDMMC_DUAL_CARDS)

	printk("Enable SD card using FCIE\n");

	for(slotNo=0; slotNo<2; slotNo++)

#endif

    {
		ret = ms_sdmmc_init_slot(slotNo, p_sdmmc_host);

		#if 0
		if(slotNo==0)
		{
			printk(LIGHT_RED">> [sdmmc_%u] Probe Devices...(Ret:%d)\n"NONE, slotNo, ret);
		}
		else if(slotNo==1)
		{
			printk(YELLOW">> [sdmmc_%u] Probe Devices...(Ret:%d)\n"NONE, slotNo, ret);
		}
		#endif

		printk(">> [sdmmc_%u] Probe Devices...(Ret:%d) \n", slotNo, ret);
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
 *     @author jeremy.wang (2011/9/22)
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

    mmc_remove_host(p_sdmmc_slot->mmc);

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

#if (EN_SDMMC_DUAL_CARDS)
	for(slotNo=0; slotNo<2; slotNo++)
#endif
	{
		ms_sdmmc_remove_slot(slotNo, p_sdmmc_host);
		printk(">> [sdmmc_%u] Remove devices...\n", slotNo);

	}

    return 0;
}

#if defined(CONFIG_PM) // CONFIG_PM
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

	printk("sd suspend\n");
#if (EN_SDMMC_DUAL_CARDS)
	for(slotNo=0; slotNo<2; slotNo++)
#endif
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
			printk(">> [sdmmc_%u] Suspend devices...(Ret:%u) \n", slotNo, ret);

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
#if (EN_SDMMC_DUAL_CARDS)
	for(slotNo=0; slotNo<2; slotNo++)
#endif
	{

	      Hal_CARD_InitGPIO((GPIOEmType)slotNo,TRUE);
	//  Hal_CARD_SetGPIOIntAttr((GPIOEmType)slotNo, EV_GPIO_OPT1);

		p_mmc_host = p_sdmmc_host->sdmmc_slot[slotNo]->mmc;
		if (p_mmc_host)
		{
			ret = mmc_resume_host(p_mmc_host);
			printk(">> [sdmmc_%u] Resume devices...(Ret:%u) \n", slotNo, ret);
			if(ret!=0)
				tret = ret;
		}
	}

    return tret;
}

#else   // !CONFIG_PM
//Current driver does not support following two functions, therefore set them to NULL.
#define ms_sdmmc_suspend        NULL
#define ms_sdmmc_resume         NULL

#endif  // CONFIG_PM


/**********************************************************************************************************
 * Define Static Global Structs
 **********************************************************************************************************/
/*----------------------------------------------------------------------------------------------------------
 *  st_ms_sdmmc_device
 ----------------------------------------------------------------------------------------------------------*/
static u64 mmc_dmamask = 0xffffffffUL;
static struct platform_device st_ms_sdmmc_device =
{
    .name = DRIVER_NAME,
    .id = 0,
    .dev =
    {
        .dma_mask = &mmc_dmamask,
        .coherent_dma_mask = 0xffffffffUL,
    },
};

/*----------------------------------------------------------------------------------------------------------
 *  st_ms_sdmmc_driver
 ----------------------------------------------------------------------------------------------------------*/
static struct platform_driver st_ms_sdmmc_driver =
{
    .remove  = ms_sdmmc_remove,/*__exit_p(ms_sdmmc_remove)*/
    .suspend = ms_sdmmc_suspend,
    .resume  = ms_sdmmc_resume,
    .probe   = ms_sdmmc_probe,
    .driver  =
    {
        .name  = DRIVER_NAME,
        .owner = THIS_MODULE,
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
    printk(">> [sdmmc] %s Driver Initializing... \n", DRIVER_NAME);

    platform_device_register(&st_ms_sdmmc_device);
    return platform_driver_register(&st_ms_sdmmc_driver);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: ms_sdmmc_exit
 *     @author jeremy.wang (2011/9/8)
 * Desc: Linux Module Function for Exit
 ----------------------------------------------------------------------------------------------------------*/
static void ms_sdmmc_exit(void)
{
    platform_driver_unregister(&st_ms_sdmmc_driver);
}

module_init(ms_sdmmc_init);
module_exit(ms_sdmmc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_AUTHOR("Jeremy_Wang");

