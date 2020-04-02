/***************************************************************************************************************
 *
 * FileName hal_card_intr.c
 *     @author jeremy.wang (2011/07/21)
 * Desc:
 *     The Interrupt behavior of all cards will run here.
 *     The goal is that we don't need to change HAL Level code (But its h file code)
 *
 *     The limitations were listed as below:
 *     (1) This c file belongs to HAL level.
 *     (2) Its h file is included by driver API or HAL level, not driver flow process.
 *     (3) MIE Event Int and Card Change Event Int function belong to here.
 *     (4) FCIE/SDIO IP interrupt register function belong to here.
 *     (5) Because ISR belongs to OS design, so we must use OS define option to separate them.
 *     (6) This c file could not use project/cpu/icver/specific define option here, but its h file could.
 *
 *     P.S. EN_XX for ON/OFF Define, V_XX  for Value Define,
 *          RT_XX for Retry Times Define, WT_XX for Wait Time Define, M_XX for Mask Value Define
 *
 ***************************************************************************************************************/
#include "hal_card_regs5.h"
#include "hal_card_intr.h"
#include "hal_card_common.h"
#include <linux/mmc/host.h>
#include "ms_sdmmc_lnx.h"
#include "sd_platform.h"

//***********************************************************************************************************
// Config Setting (Internel)
//***********************************************************************************************************
typedef enum
{
    IRQ_TYPE_NONE	= 0,
    IRQ_TYPE_EVENT	= 1,
    IRQ_TYPE_D1INT	= 2,

} E_IRQ_TYPE;

// Enable Setting
//-----------------------------------------------------------------------------------------------------------
#define EN_TRFUNC         (TRUE)

// Value Setting
//-----------------------------------------------------------------------------------------------------------
#define V_MAXSLOTS          2


//***********************************************************************************************************


// Reg Static Init Setting
//-----------------------------------------------------------------------------------------------------------
#define V_SD_MIE_INT_INIT   (R_SD_CMD_END_EN|R_SD_DATA_END_EN|R_CARD_DMA_END_EN|R_MMA_LSDONE_END_EN)

// Mask Range
//-----------------------------------------------------------------------------------------------------------
#define M_O_CARD_STS        (R_MS_STS_CHG|R_CF_STS_CHG|R_SM_STS_CHG|R_XD_STS_CHG)

#define M_CARD_INT_EN		  (BIT_DMA_END|BIT_SD_CMD_END|BIT_ERR_STS|BIT_BUSY_END_INT)

//IP_FCIE or IP_SDIO Register Basic Address
//-----------------------------------------------------------------------------------------------------------
#define A_SD_REG_BANK(IP)           GET_CARD_BANK(IP, EV_REG_BANK)

#define A_MIE_EVENT_REG(IP)         GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x00)
#define A_MIE_INT_EN_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x01)
#define A_MIE_FUNC_CTL_REG(IP)      GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x07)
#define A_SDIO_MODE(IP)				GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x11)
#define A_SDIO_INTR_DET(IP)			GET_CARD_REG_ADDR(A_SD_REG_BANK(IP), 0x2F)

// Reg Dynamic Variable
//-----------------------------------------------------------------------------------------------------------
static volatile U16_T	gu16_MIEINT_Mode[2] = {0};
static volatile U16_T	gu16_MIEEvent_ForInt[2] = {0};   //MIEEvent for Interrupt
static volatile BOOL_T  gb_StopWaitMIE[2] = {0};

// Trace Funcion
//-----------------------------------------------------------------------------------------------------------
#if (EN_TRFUNC)
	#define TR_INT(p)			p
#else
	#define TR_INT(p)
#endif



BOOL_T _CARD_INT_Filter(IPEmType eIP, IntCardEmType eCardInt)
{
	if(eCardInt == EV_INT_SD)
	{

		if(FCIE_RIU_R16(A_MIE_FUNC_CTL_REG(eIP)) & (BIT_SD_EN|BIT_SDIO_MOD)) //Card Event
			return (TRUE);
	}

	return (FALSE);

}


E_IRQ_TYPE _CARD_INT_SaveMIEEvent(IPEmType eIP, struct mmc_host * host)
{

	U16_T u16Reg = FCIE_RIU_R16(A_MIE_EVENT_REG(eIP));

#if defined SDIO_D1_INTR_MODE && SDIO_D1_INTR_MODE

	if(u16Reg & BIT_SDIO_INT)
	{
 		mmc_signal_sdio_irq(host);
		FCIE_RIU_W16(A_MIE_EVENT_REG(eIP), BIT_SDIO_INT); // W1C event
		return IRQ_TYPE_D1INT;
	}
	else

#endif

	{
		gu16_MIEEvent_ForInt[eIP] |= u16Reg; // Summary All 0x00Reg Event

		if(u16Reg & BIT_DMA_END)
	    {
	        FCIE_RIU_16_OF(A_MIE_INT_EN_REG(eIP), BIT_DMA_END);
	    }

		if(u16Reg & BIT_SD_CMD_END)
		{
			FCIE_RIU_16_OF(A_MIE_INT_EN_REG(eIP), BIT_SD_CMD_END);
		}

		return IRQ_TYPE_EVENT;
	}

	return IRQ_TYPE_NONE;

}





/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_MIEModeCtrl
 *     @author jeremy.wang (2011/12/20)
 * Desc: Open Card MIE Interrupt Mode
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eCardInt : SD/MS/CF/XD...
 * @param bOpen : Open Card Interrupt Mode or Not
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_INT_MIEModeCtrl(IPEmType eIP, IntCardEmType eCardInt, BOOL_T bOpen)
{
	if(bOpen)
		gu16_MIEINT_Mode[eIP] |= eCardInt;
	else
		gu16_MIEINT_Mode[eIP]  &= (~eCardInt);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_MIEModeRunning
 *     @author jeremy.wang (2011/9/7)
 * Desc: Check Int Mode Running or Not
 *
 * @param eIP :  FCIE1/FCIE2/...
 * @param eCardInt : SD/MS/CF/XD...
 *
 * @return BOOL_T  : Interrupt Running or Not
 ----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_CARD_INT_MIEModeRunning(IPEmType eIP, IntCardEmType eCardInt)
{
	if(gu16_MIEINT_Mode[eIP] & eCardInt )
		return (TRUE);
	else
		return (FALSE);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_SetMIEIntEn
 *     @author jeremy.wang (2011/12/20)
 * Desc: Set MIE_Int_En to Trig Interrupt
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eCardInt : SD/MS/CF/XD...
 * @param u16RegMIEIntEN : MIE_Int_En Reg Setting

 1. set command end interrupt when send simple command
 2. set cmd end & dma end interrutp when read
 3. set command end interrupt when write cmd first, then set dma end interrupt second

 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_INT_SetMIEIntEn(IPEmType eIP, IntCardEmType eCardInt, U16_T u16RegMIEIntEN)
{
	U16_T u16IntEn;

	// Do not touch BIT_SDIO_INT, control D1 interrupt independantly

	u16IntEn = FCIE_RIU_R16(A_MIE_INT_EN_REG(eIP));

	u16IntEn &= ~BIT_ALL_CARD_INT_EVENTS;

	u16IntEn |= u16RegMIEIntEN;

	FCIE_RIU_W16(A_MIE_INT_EN_REG(eIP), u16IntEn);
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_ClearMIEEvent
 *     @author jeremy.wang (2011/9/7)
 * Desc: Clear MIE Event That We Have Saved in Interrupt Mode
 *
 * @param eIP : FCIE1/FCIE2/...
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_INT_ClearMIEEvent(IPEmType eIP)
{
	gu16_MIEEvent_ForInt[eIP] = 0;
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_GetMIEEvent
 *     @author jeremy.wang (2011/9/7)
 * Desc: Get Current MIE Event That We Have Saved in Interrupt Mode
 *
 * @param eIP : FCIE1/FCIE2/...

 * @return U16_T  : Current MIE Event
 ----------------------------------------------------------------------------------------------------------*/
U16_T Hal_CARD_INT_GetMIEEvent(IPEmType eIP)
{
	return gu16_MIEEvent_ForInt[eIP];
}


//***********************************************************************************************************
// Interrupt Handler Function
//***********************************************************************************************************

//###########################################################################################################
#if(D_OS == D_OS__LINUX)
//###########################################################################################################
static DECLARE_WAIT_QUEUE_HEAD(fcie1_mieint_wait);
static DECLARE_WAIT_QUEUE_HEAD(fcie2_mieint_wait);

#endif
//###########################################################################################################


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_WaitMIEEvent
 *     @author jeremy.wang (2011/12/20)
 * Desc: Set WaitQueue Handler to Wait MIE Event
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param u16ReqEvent : MIE Event for waiting
 * @param u32WaitMs : Waiting Time (ms)
 *
 * @return BOOL_T  : TRUE (Success), FALSE (Timeout)
 ----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_CARD_INT_WaitMIEEvent(IPEmType eIP, U16_T u16ReqEvent, U32_T u32WaitMs)
{
//###########################################################################################################
#if (D_OS == D_OS__LINUX)
//###########################################################################################################
	if(eIP==EV_IP_FCIE1)
		wait_event_timeout(fcie1_mieint_wait, ((gu16_MIEEvent_ForInt[eIP]& u16ReqEvent) == u16ReqEvent) || gb_StopWaitMIE[eIP], msecs_to_jiffies(u32WaitMs+WT_INT_RISKTIME));
	else if((eIP==EV_IP_FCIE2))
		wait_event_timeout(fcie2_mieint_wait, ((gu16_MIEEvent_ForInt[eIP]& u16ReqEvent) == u16ReqEvent) || gb_StopWaitMIE[eIP], msecs_to_jiffies(u32WaitMs+WT_INT_RISKTIME));

	if((gu16_MIEEvent_ForInt[eIP] & u16ReqEvent) != u16ReqEvent)
	{
		printk("wait %04Xh timeout\n", u16ReqEvent);
		return FALSE;
	}

	return TRUE;
//###########################################################################################################
#endif

	/****** Not Support (Ex: D_OS_ARCH = D_OS_UBOOT) ******/
	return FALSE;
}

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_WaitMIEEventCtrl
 *     @author jeremy.wang (2011/12/20)
 * Desc: Stop Wait MIE Event to Avoid Long Time Waiting
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param bStop : Stop Wait or Not
 ----------------------------------------------------------------------------------------------------------*/
void Hal_CARD_INT_WaitMIEEventCtrl(IPEmType eIP, BOOL_T bStop)
{
	gb_StopWaitMIE[eIP] = bStop;

//###########################################################################################################
#if (D_OS == D_OS__LINUX)
//###########################################################################################################
	if(gb_StopWaitMIE[eIP])
	{
		 /****** Break Current Wait Event *******/
		if(eIP==EV_IP_FCIE1)
			wake_up(&fcie1_mieint_wait);
		else if((eIP==EV_IP_FCIE2))
			wake_up(&fcie2_mieint_wait);
	}
#endif
//###########################################################################################################

}





//***********************************************************************************************************
// Interrupt Function
//***********************************************************************************************************

//###########################################################################################################
#if(D_OS == D_OS__LINUX)
//###########################################################################################################

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_CARD_INT_MIE
 *     @author jeremy.wang (2012/2/13)
 * Desc:
 *
 * @param irq :
 * @param p_dev_id :
 *
 * @return irqreturn_t  :
 ----------------------------------------------------------------------------------------------------------*/
irqreturn_t Hal_CARD_INT_MIE(int irq, void *p_dev_id)
{
	irqreturn_t irq_t = IRQ_NONE;
	IntSourceStruct* pstIntSource = p_dev_id;
	IPEmType eIP  = pstIntSource->eIP;
	struct ms_sdmmc_slot * sdio_host;
	E_IRQ_TYPE eIrqType;

	if( FCIE_RIU_R16(A_MIE_FUNC_CTL_REG(eIP)) & BIT_EMMC_ACTIVE )
	{
		return IRQ_NONE;
	}

	sdio_host = (struct ms_sdmmc_slot *)pstIntSource->sdio_host;

	eIrqType = _CARD_INT_SaveMIEEvent(eIP, sdio_host->mmc);

	if(eIrqType ==IRQ_TYPE_EVENT)
	{
		if(eIP==EV_IP_FCIE1) // SDIO
		{
			wake_up(&fcie1_mieint_wait);
		}
		else if((eIP==EV_IP_FCIE2))
		{
			wake_up(&fcie2_mieint_wait);
		}

		irq_t = IRQ_HANDLED;
	}
	else if(eIrqType ==IRQ_TYPE_D1INT) // no need to wake up wait queue head
	{
		irq_t = IRQ_HANDLED;
	}

	return irq_t;
}


//###########################################################################################################
#endif








