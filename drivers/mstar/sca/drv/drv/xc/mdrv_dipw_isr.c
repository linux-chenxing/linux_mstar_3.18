////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// file    Mdrv_sc_isr.c
/// @brief  Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////



//     THIS FILE IS NOT FULLY IMPLEMENTED YET!!!!!!!!!!!!!!!!

#define _MDRV_XC_ISR_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/kernel.h>
#include <linux/pid.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/irqreturn.h>
#endif

// Common Definition
#include "MsCommon.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"

// Internal Definition
#include "drvXC_IOPort.h"
#include "xc_hwreg_utility2.h"
//#include "hwreg_dipw.h"
#include "apiXC.h"
#include "drv_sc_ip.h"
#include "drv_sc_isr.h"
#include "xc_Analog_Reg.h"
#include "mvideo_context.h"
#include "mhal_sc.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define SC_ISR_DBG(x)       //x
#define FRAME_NUM 8
//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------

MS_U8 g_u8DIP_ISR_RecFlag = 0;
MS_U32 g_u32DIPW_Signal_PID = 0;
atomic_t DIPW_intr_count;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
XC_ISR_INFO stIsrDIPWInfo[FRAME_NUM][MAX_ISR_NUM_OF_EACH_INT];


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------


/******************************************************************************/
/*                           Local Variables                                    */
/******************************************************************************/

/******************************************************************************/
/*               P r i v a t e    F u n c t i o n s                             */
/******************************************************************************/
//-------------------------------------------------------------------------------------------------
/// Convert SC_INT_SRC bit shift case
/// @param eIntNum      \b IN: the interrupt number of API layer
/// @return @ref SC_INT_SRC the interrupt number of driver layer
//-------------------------------------------------------------------------------------------------

#if 0
static void _MDrv_DIPW_clear_interrupt(DIPW_INT_SRC u8IntSrc)
{
/*!!!!! NOT IMPLEMENT YET!!!!!!!*/
    MS_U8 u8Bank;

    u8Bank = MDrv_ReadByte(BK_SELECT_00);
    MDrv_WriteByte(BK_SELECT_00, REG_BANK_DIPW);
    switch(u8IntSrc)
    {
	    case DIPW_INT_SC0:
		    //SC_W2BYTEMSK(REG_SC_BK00_12_L , BIT(u8IntRemap),BIT(u8IntRemap));
		    break;
		    //case DIPW_INT_SC1:
		    //        SC_W2BYTEMSK(REG_SC_BK00_12_L , BIT(u8IntRemap),BIT(u8IntRemap));
		    //        break;
		    //case DIPW_INT_SC2:
		    //        SC_W2BYTEMSK(REG_SC_BK00_12_L , BIT(u8IntRemap),BIT(u8IntRemap));
		    //        break;
	    default:
		    return;
    }
    MDrv_WriteByte(BK_SELECT_00, u8Bank);
}
#endif
void MDrv_DIPW_set_interrupt(DIPW_INT_SRC u8IntSrc, MS_BOOL bEnable)
{
    MS_U8 u8Bank;
    u8Bank = MDrv_ReadByte(BK_SELECT_00);
    MDrv_WriteByte(BK_SELECT_00, REG_BANK_DIPW);
    switch(u8IntSrc)
    {
	    case DIPW_INT_SC0:
		    //SC_W2BYTEMSK(REG_SC_BK00_12_L , BIT(u8IntRemap),BIT(u8IntRemap));
		    break;
	    //case DIPW_INT_SC1:
	    //        SC_W2BYTEMSK(REG_SC_BK00_12_L , BIT(u8IntRemap),BIT(u8IntRemap));
	    //        break;
	    //case DIPW_INT_SC2:
	    //        SC_W2BYTEMSK(REG_SC_BK00_12_L , BIT(u8IntRemap),BIT(u8IntRemap));
	    //        break;
	    default:
		return ;
    }

    if(bEnable)
    {
	    // clear interrupt
	    // MDrv_WriteRegBit(L_BK_GOPINT(0x12) + u8ByteIndex, bEnable, u8ByteMask);

	    // disable mask
	    // MDrv_WriteRegBit(L_BK_GOPINT(0x14) + u8ByteIndex, !bEnable, u8ByteMask);
	    SC_W2BYTEMSK(REG_SC_BK36_09_L , 0xFF, 0xff);//clear
	    SC_W2BYTEMSK(REG_SC_BK36_08_L , 0x00, 0xff);//mask
    }
    else
    {

	    // mask it
	    //MDrv_WriteRegBit(L_BK_GOPINT(0x14) + u8ByteIndex, !bEnable, u8ByteMask);

	    // clear interrupt
	    //MDrv_WriteRegBit(L_BK_GOPINT(0x12) + u8ByteIndex, bEnable, u8ByteMask);

	    SC_W2BYTEMSK(REG_SC_BK36_08_L , 0xff,0xff);
	    SC_W2BYTEMSK(REG_SC_BK36_09_L , 0xff,0xff);
    }

    g_u8DIP_ISR_RecFlag = 0;
    MDrv_WriteByte(BK_SELECT_00, u8Bank);
}

MS_U8 _get_intr_frame(void)
{

	return SC_R2BYTEMSK(REG_SC_BK36_0A_L, 0x00FF);
	/*
	MS_U8 frame_idx = 0;

	for( ; frame_idx < FRAME_NUM ; frame_idx ++)
	{
		if( SC_R2BYTEMSK( REG_SC_BK36_0A_L , BIT(frame_idx)) )
			return frame_idx;
	}

	//should not run here
	return 0;
	*/


}


MS_U8 MDrv_DIPW_Get_ISR_RecFlag(void)
{
    return g_u8DIP_ISR_RecFlag;
}

void MDrv_DIPW_Set_Signal_PID(MS_U32 u32PID)
{
    printf("DIPW_SINGAL_PID:%d\n",(int)u32PID);
    g_u32DIPW_Signal_PID = u32PID;
}

MS_U32 MDrv_DIPW_Get_Signal_PID(void)
{
    return g_u32DIPW_Signal_PID;
}

#ifdef MSOS_TYPE_LINUX_KERNEL
irqreturn_t  MDrv_DIPW_isr(int eIntNum, void* dev_id)
#else
void MDrv_DIPW_isr( InterruptNum eIntNum )
#endif
{

//process the frame number interrupt

//clear the frame number intterupt

//	unsigned idx = 0;
//	for(; idx < SC_NUM; idx++)
	{
		MS_U8 intr_frame = _get_intr_frame();
		g_u8DIP_ISR_RecFlag = intr_frame;
		SC_W2BYTEMSK(REG_SC_BK36_09_L , 0xFF, 0xFF);//reg_dipw_irq_clr
		//SC_W2BYTEMSK(REG_SC_BK36_09_L , BIT(intr_frame), BIT(intr_frame));//reg_dipw_irq_clr
//		SC_W2BYTEMSK(REG_SC_BK36_09_L , 0 ,BIT(intr_num));//reg_dipw_irq_clr

	}

#ifdef MSOS_TYPE_LINUX_KERNEL
    if(MDrv_DIPW_Get_Signal_PID())
    {
        struct task_struct *p = NULL;
         p = pid_task(find_vpid(MDrv_DIPW_Get_Signal_PID()), PIDTYPE_PID);
         if (NULL == p)
         {
            printf("struct task_struct NULL\n");
         }
         else
         {
            send_sig(SIGUSR1, p, 0);
         }
    }
    else
    {
        atomic_inc(&DIPW_intr_count);
    }
    
    return IRQ_HANDLED;
#else
    // re-enable IRQ_DISP
    MsOS_EnableInterrupt(E_INT_IRQ_DISP);
#endif

}

//////////////////////////////////
// Interrupt Function
//////////////////////////////////
//-------------------------------------------------------------------------------------------------
/// Get maximum number of interrupts that scaler supports
/// @return @ref MS_U8 maximum number of interrupts that scaler supports
//-------------------------------------------------------------------------------------------------
MS_U8   MDrv_DIPW_InterruptGetMaxIntNum(void)
{
    return MAX_DIPW_INT;
}

//-------------------------------------------------------------------------------------------------
/// After get maximum number of interrupts by @ref MDrv_DIPW_InterruptGetMaxIntNum, you can
/// check each interrupt supported btuy scaler or not.
/// @param eIntNum      \b IN: the interrupt number to query
/// @return @ref MS_BOOL the interrupt number supported or not
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_DIPW_InterruptAvaliable(DIPW_INT_SRC eIntNum)
{
    MS_BOOL bReturn = FALSE;

    // check if out of range or not supported
    if((eIntNum >= MDrv_DIPW_InterruptGetMaxIntNum()) )
    {
        goto Exit;
    }
#if 0 //disable for cedric by jiansu
    // search avaliable CB to attach
    for(i=0; i<MAX_ISR_NUM_OF_EACH_INT; i++)
    {
        if(stDIPWIsrInfo[eIntNum][i].aryXC_ISR == NULL)
        {
            bReturn = TRUE;
            break;
        }
    }
#endif

Exit:
    return bReturn;
}

//-------------------------------------------------------------------------------------------------
/// Check if ISR already attached or not because we allowed to attach same ISR for many times
/// @param eIntNum @ref SC_INT_SRC          \b IN: the interrupt number that you want to attach
/// @param pIntCb  @ref SC_InterruptCb      \b IN: the ISR that you want to attach to the interrupt number
/// @param pParam                           \b IN: the parameter that you want to pass to the ISR when called
/// @return @ref MS_BOOL Is ISR already attached or not.
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_DIPW_InterruptIsAttached(DIPW_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam)
{
	MS_U8 u8Bank;
	MS_BOOL bRet = FALSE;
	u8Bank = MDrv_ReadByte(BK_SELECT_00);
	MDrv_WriteByte(BK_SELECT_00, REG_BANK_DIPW);

	if( SC_R2BYTEMSK(REG_SC_BK36_08_L,0xff) )
		bRet = TRUE;
	else
		bRet = FALSE;
	MDrv_WriteByte(BK_SELECT_00, u8Bank);
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
/// Attach interrupt to specified interrupt number
/// @param eIntNum @ref SC_INT_SRC          \b IN: the interrupt number that you want to attach
/// @param pIntCb  @ref SC_InterruptCb      \b IN: the ISR that you want to attach to the interrupt number
/// @param pParam                           \b IN: the parameter that you want to pass to the ISR when called
/// @return @ref MS_BOOL attach successed or not, FALSE means can't attach more ISR
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_DIPW_InterruptAttach(DIPW_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam)
{
    MS_BOOL bReturn = FALSE;
    _XC_ENTRY_MUTEX(_XC_Mutex);
    bReturn = MDrv_DIPW_InterruptAttachWithoutMutex(eIntNum, pIntCb, pParam);
    _XC_RETURN_MUTEX(_XC_Mutex);
    return bReturn;
}

//-------------------------------------------------------------------------------------------------
/// DeAttach interrupt to specified interrupt number
/// @param eIntNum @ref SC_INT_SRC          \b IN: the interrupt number that you want to de-attach
/// @param pIntCb  @ref SC_InterruptCb      \b IN: the ISR that you want to de-attach to the interrupt number
/// @param pParam                           \b IN: the parameter that you want to pass to the ISR when called
/// @return @ref MS_BOOL attach successed or not, FALSE means can't de-attach this ISR because it not exist
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_DIPW_InterruptDeAttach(DIPW_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam)
{
    MS_BOOL bReturn = FALSE;

    _XC_ENTRY_MUTEX(_XC_Mutex);
    bReturn = MDrv_DIPW_InterruptDeAttachWithoutMutex(eIntNum, pIntCb, pParam);

    _XC_RETURN_MUTEX(_XC_Mutex);
    return bReturn;
}

MS_BOOL MDrv_DIPW_InterruptAttachWithoutMutex(DIPW_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam)
{
    MS_BOOL bReturn = FALSE;

    // check if out of range or not supported
    if((eIntNum >= MDrv_DIPW_InterruptGetMaxIntNum()) )
    {
        SC_ISR_DBG(printf("ISRAttach failed %d (max %d, supported %lx)\n", eIntNum, MDrv_DIPW_InterruptGetMaxIntNum(), (MS_U32)SUPPORTED_XC_INT));
        goto Exit;
    }
#if 0 //disable it for cedric
    // attach ISR
    for(i=0; i<MAX_ISR_NUM_OF_EACH_INT; i++)
    {
        if(stDIPWIsrInfo[eIntNum][i].aryXC_ISR == NULL)
        {
            // disable interrupt first to avoid racing condition, for example, interrupt come in after set pIntCb but before set pParam, will call ISR and passing parameter NULL
            MDrv_DIPW_set_interrupt(eIntNum, DISABLE);

            stDIPWIsrInfo[eIntNum][i].aryXC_ISR_Param = pParam;
            stDIPWIsrInfo[eIntNum][i].aryXC_ISR = pIntCb;

            // enable interrupt
            MDrv_DIPW_set_interrupt(eIntNum, ENABLE);

            SC_ISR_DBG(printf("ISRAttach %d successed to [%d][%d]\n", eIntNum, eIntNum, i));

            bReturn = TRUE;
            break;
        }
    }
#else

            MDrv_DIPW_set_interrupt(eIntNum, ENABLE);
#endif
Exit:
    return bReturn;
}


MS_BOOL MDrv_DIPW_InterruptDeAttachWithoutMutex(DIPW_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam)
{
    MS_BOOL bReturn = FALSE;

    // check if out of range or not supported
    if((eIntNum >= MDrv_DIPW_InterruptGetMaxIntNum()) )
    {
        SC_ISR_DBG(printf("ISRDeAttach failed %d (max %d, supported %lx)\n", eIntNum, MDrv_DIPW_InterruptGetMaxIntNum(), SUPPORTED_XC_INT));
        goto Exit;
    }
#if 0 //disable it for cedric
    // de-attach the ISR
    for(i=0; i<MAX_ISR_NUM_OF_EACH_INT; i++)
    {
        if((stDIPWIsrInfo[eIntNum][i].aryXC_ISR == pIntCb) &&
            (stDIPWIsrInfo[eIntNum][i].aryXC_ISR_Param == pParam))
        {
            // disable interrupt first to avoid racing condition
            MDrv_DIPW_set_interrupt(eIntNum, DISABLE);

            stDIPWIsrInfo[eIntNum][i].aryXC_ISR = NULL;
            stDIPWIsrInfo[eIntNum][i].aryXC_ISR_Param = NULL;
            bReturn = TRUE;

            SC_ISR_DBG(printf("ISRDeAttach %d successed to [%d][%d]\n", eIntNum, eIntNum, i));
            break;
        }
    }

    // re-enable interrupt
    if(bReturn)
    {
        // removed ISR, check if there is other ISR left
        for(i=0; i<MAX_ISR_NUM_OF_EACH_INT; i++)
        {
            if(stDIPWIsrInfo[eIntNum][i].aryXC_ISR != NULL)
            {
                // re-enable interrupt
                MDrv_DIPW_set_interrupt(eIntNum, ENABLE);
                SC_ISR_DBG(printf("ISRAttach %d re-enable ISR\n", eIntNum));
            }
        }
    }
#else
            MDrv_DIPW_set_interrupt(eIntNum, DISABLE);
#endif
Exit:

    return bReturn;
}
#undef _MDRV_XC_ISR_C_

