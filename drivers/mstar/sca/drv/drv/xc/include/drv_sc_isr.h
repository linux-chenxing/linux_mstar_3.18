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
#ifndef _MDRV_XC_ISR_H_
#define _MDRV_XC_ISR_H_

#ifdef _MDRV_XC_ISR_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

// XC interrupt
#define MAX_ISR_NUM_OF_EACH_INT     (10)

typedef struct
{
    void * aryXC_ISR_Param;         // parameter will be passed to ISR when calling it.
    SC_InterruptCb aryXC_ISR;       // attach ISR
} XC_ISR_INFO;

extern  XC_ISR_INFO stXCIsrInfo[MAX_SC_INT][MAX_ISR_NUM_OF_EACH_INT];
extern  XC_ISR_INFO stXC1_IsrInfo[MAX_SC_INT][MAX_ISR_NUM_OF_EACH_INT];
extern  XC_ISR_INFO stXC2_IsrInfo[MAX_SC_INT][MAX_ISR_NUM_OF_EACH_INT];
//////////////////////////////////
// Interrupt Function
//////////////////////////////////
INTERFACE MS_U8   MDrv_XC_InterruptGetMaxIntNum(void);
INTERFACE MS_BOOL MDrv_XC_InterruptAvaliable(SC_INT_SRC eIntNum);

INTERFACE MS_BOOL MDrv_XC_InterruptIsAttached(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_BOOL MDrv_XC_InterruptAttach(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_BOOL MDrv_XC_InterruptDeAttach(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_BOOL MDrv_XC_InterruptAttachWithoutMutex(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_BOOL MDrv_XC_InterruptDeAttachWithoutMutex(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);

#ifdef MSOS_TYPE_LINUX_KERNEL
INTERFACE void MDrv_SC_set_interrupt(SC_INT_SRC u8IntSrc, MS_BOOL bEnable);
irqreturn_t MDrv_SC_isr(int eIntNum, void* dev_id);
#else
INTERFACE void MDrv_SC_isr(InterruptNum eIntNum);
#endif


/*function for disp1*/
INTERFACE MS_U8   MDrv_XC1_InterruptGetMaxIntNum(void);
INTERFACE MS_BOOL MDrv_XC1_InterruptAvaliable(SC_INT_SRC eIntNum);

INTERFACE MS_BOOL MDrv_XC1_InterruptIsAttached(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_BOOL MDrv_XC1_InterruptAttach(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_BOOL MDrv_XC1_InterruptDeAttach(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_BOOL MDrv_XC1_InterruptAttachWithoutMutex(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_BOOL MDrv_XC1_InterruptDeAttachWithoutMutex(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);

#ifdef MSOS_TYPE_LINUX_KERNEL
INTERFACE void MDrv_SC1_set_interrupt(SC_INT_SRC u8IntSrc, MS_BOOL bEnable);
irqreturn_t MDrv_SC1_isr(int eIntNum, void* dev_id);
#else
INTERFACE void MDrv_SC1_isr(InterruptNum eIntNum);
#endif

/*function for disp2*/
INTERFACE MS_U8   MDrv_XC2_InterruptGetMaxIntNum(void);
INTERFACE MS_BOOL MDrv_XC2_InterruptAvaliable(SC_INT_SRC eIntNum);

INTERFACE MS_BOOL MDrv_XC2_InterruptIsAttached(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_BOOL MDrv_XC2_InterruptAttach(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_BOOL MDrv_XC2_InterruptDeAttach(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_BOOL MDrv_XC2_InterruptAttachWithoutMutex(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_BOOL MDrv_XC2_InterruptDeAttachWithoutMutex(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);

#ifdef MSOS_TYPE_LINUX_KERNEL
INTERFACE void MDrv_SC2_set_interrupt(SC_INT_SRC u8IntSrc, MS_BOOL bEnable);
irqreturn_t MDrv_SC2_isr(int eIntNum, void* dev_id);
#else
INTERFACE void MDrv_SC2_isr(InterruptNum eIntNum);
#endif

/*function for dipw */
INTERFACE MS_BOOL MDrv_DIPW_InterruptIsAttached(DIPW_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_BOOL MDrv_DIPW_InterruptAttach(DIPW_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_BOOL MDrv_DIPW_InterruptDeAttach(DIPW_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_BOOL MDrv_DIPW_InterruptAttachWithoutMutex(DIPW_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_BOOL MDrv_DIPW_InterruptDeAttachWithoutMutex(DIPW_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam);
INTERFACE MS_U8 MDrv_DIPW_Get_ISR_RecFlag(void);
INTERFACE void MDrv_DIPW_Set_Signal_PID(MS_U32 u32PID);
INTERFACE MS_U8 MDrv_DIPW_Get_ISR_RecFlag(void);

#ifdef MSOS_TYPE_LINUX_KERNEL
irqreturn_t MDrv_DIPW_isr(int eIntNum, void* dev_id);
#endif

#undef INTERFACE
#endif //_MDRV_XC_ISR_H_

