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
#define _MDRV_XC_ISR_C_

//#define SIG_INT_EXAMPLE  //Userspace interrupt handling example(signal)
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#ifdef MSOS_TYPE_LINUX_KERNEL
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
#include "hwreg_sc.h"
#include "apiXC.h"
#include "drv_sc_ip.h"
#include "drv_sc_isr.h"
#include "xc_Analog_Reg.h"
#include "mvideo_context.h"
#include "mhal_sc.h"

#ifdef SIG_INT_EXAMPLE
#include   <linux/fs.h>
#include   <linux/sched.h>
#include   <linux/uaccess.h>
#endif
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define SC_ISR_DBG(x)       //x

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


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
XC_ISR_INFO stXCIsrInfo[MAX_SC_INT][MAX_ISR_NUM_OF_EACH_INT];
// for getting the DLC Ymax/min from VBlanking
MS_U8 g_u8DLC_Ymax_From_IP1F2 = 0x00;
MS_U8 g_u8DLC_Ymin_From_IP1F2 = 0xFF;
MS_U8 g_u8DLC_Cbmax_From_IP1F2 = 0x00;
MS_U8 g_u8DLC_Cbmin_From_IP1F2 = 0xFF;
MS_U8 g_u8DLC_Crmax_From_IP1F2 = 0x00;
MS_U8 g_u8DLC_Crmin_From_IP1F2 = 0xFF;


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
static MS_U8 _MApi_XC_GetIntSrc(SC_INT_SRC eIntNum)
{
    MS_U8 u8Int_bit;

    switch(eIntNum)
    {
        // 0x04
        default                   :
        case SC_INT_TUNE_FAIL_P     : u8Int_bit = IRQ_INT_TUNE_FAIL_P     ;    break;

        case SC_INT_VSINT         : u8Int_bit = IRQ_INT_VSINT         ;    break;
        case SC_INT_F2_VTT_CHG    : u8Int_bit = IRQ_INT_F2_VTT_CHG    ;    break;
        case SC_INT_F1_VTT_CHG    : u8Int_bit = IRQ_INT_F1_VTT_CHG    ;    break;
        case SC_INT_F2_VS_LOSE    : u8Int_bit = IRQ_INT_F2_VS_LOSE    ;    break;
        case SC_INT_F1_VS_LOSE    : u8Int_bit = IRQ_INT_F1_VS_LOSE    ;    break;
        case SC_INT_F2_JITTER     : u8Int_bit = IRQ_INT_F2_JITTER     ;    break;
        case SC_INT_F1_JITTER     : u8Int_bit = IRQ_INT_F1_JITTER     ;    break;
        case SC_INT_F2_IPVS_SB    : u8Int_bit = IRQ_INT_F2_IPVS_SB    ;    break;
        case SC_INT_F1_IPVS_SB    : u8Int_bit = IRQ_INT_F1_IPVS_SB    ;    break;
        case SC_INT_F2_IPHCS_DET  : u8Int_bit = IRQ_INT_F2_IPHCS_DET  ;    break;
        case SC_INT_F1_IPHCS_DET  : u8Int_bit = IRQ_INT_F1_IPHCS_DET  ;    break;

        // 0x10
        case SC_INT_F2_IPHCS1_DET  : u8Int_bit = IRQ_INT_F2_IPHCS1_DET  ;    break;
        case SC_INT_F1_IPHCS1_DET  : u8Int_bit = IRQ_INT_F1_IPHCS1_DET  ;    break;
        case SC_INT_F2_HTT_CHG    : u8Int_bit = IRQ_INT_F2_HTT_CHG    ;    break;
        case SC_INT_F1_HTT_CHG    : u8Int_bit = IRQ_INT_F1_HTT_CHG    ;    break;
        case SC_INT_F2_HS_LOSE    : u8Int_bit = IRQ_INT_F2_HS_LOSE    ;    break;
        case SC_INT_F1_HS_LOSE    : u8Int_bit = IRQ_INT_F1_HS_LOSE    ;    break;
        case SC_INT_DVI_CK_LOSE_F2  : u8Int_bit = IRQ_INT_DVI_CK_LOSE_F2  ;    break;
        case SC_INT_DVI_CK_LOSE_F1  : u8Int_bit = IRQ_INT_DVI_CK_LOSE_F1  ;    break;

        case SC_INT_F2_CSOG       : u8Int_bit = IRQ_INT_F2_CSOG       ;    break;
        case SC_INT_F1_CSOG       : u8Int_bit = IRQ_INT_F1_CSOG       ;    break;
  //      case SC_INT_F2_RESERVED2  : u8Int_bit = IRQ_INT_F2_RESERVED2  ;    break;
  //      case SC_INT_F1_RESERVED2  : u8Int_bit = IRQ_INT_F1_RESERVED2  ;    break;
        case SC_INT_F2_ATS_READY  : u8Int_bit = IRQ_INT_F2_ATS_READY  ;    break;
        case SC_INT_F1_ATS_READY  : u8Int_bit = IRQ_INT_F1_ATS_READY  ;    break;
        case SC_INT_F2_ATP_READY  : u8Int_bit = IRQ_INT_F2_ATP_READY  ;    break;
        case SC_INT_F1_ATP_READY  : u8Int_bit = IRQ_INT_F1_ATP_READY  ;    break;
        case SC_INT_F2_ATG_READY  : u8Int_bit = IRQ_INT_F2_ATG_READY  ;    break;
        case SC_INT_F1_ATG_READY  : u8Int_bit = IRQ_INT_F1_ATG_READY  ;    break;
 //       case SC_INT_F2_RESERVED3  : u8Int_bit = IRQ_INT_F2_RESERVED3  ;    break;
 //       case SC_INT_F1_RESERVED3  : u8Int_bit = IRQ_INT_F1_RESERVED3  ;    break;
    }

    return u8Int_bit;
}

static void _MDrv_SC_clear_interrupt(SC_INT_SRC u8IntSrc)
{
    //MS_U8 u8Bank;
    //MS_U8 u8ByteIndex;
    //MS_U8 u8ByteMask;

    //u8ByteIndex = (u8IntSrc >> 3);
    //u8ByteMask = (1 << (u8IntSrc & 0x7));

    //u8Bank = MDrv_ReadByte(BK_SELECT_00);

    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_GOPINT);
    //MDrv_WriteRegBit(L_BK_GOPINT(0x12) + u8ByteIndex, 1, u8ByteMask);

    //SC_W2BYTEMSK((REG_SC_BK00_12_L + u8ByteIndex), 1 , u8ByteMask);
    MS_U8 u8IntRemap = _MApi_XC_GetIntSrc(u8IntSrc);

    if(u8IntRemap < 16)
    {
        SC_W2BYTEMSK(REG_SC_BK00_12_L , BIT(u8IntRemap),BIT(u8IntRemap));
    }
    else
    {
        SC_W2BYTEMSK(REG_SC_BK00_13_L , BIT(u8IntRemap-16) ,BIT(u8IntRemap-16));
    }
    //MDrv_WriteByte(BK_SELECT_00, u8Bank);
}

void MDrv_SC_set_interrupt(SC_INT_SRC u8IntSrc, MS_BOOL bEnable)
{
    MS_U8 u8Bank;
    //MS_U8 u8ByteIndex;
    //MS_U8 u8ByteMask;

    //u8ByteIndex = (u8IntSrc >> 3);
    //u8ByteMask = (1 << (u8IntSrc & 0x7));
    MS_U8 u8IntRemap = _MApi_XC_GetIntSrc(u8IntSrc);

    u8Bank = MDrv_ReadByte(BK_SELECT_00);
    MDrv_WriteByte(BK_SELECT_00, REG_BANK_GOPINT);

    if(bEnable)
    {
        // clear interrupt
        // MDrv_WriteRegBit(L_BK_GOPINT(0x12) + u8ByteIndex, bEnable, u8ByteMask);

        // disable mask
        // MDrv_WriteRegBit(L_BK_GOPINT(0x14) + u8ByteIndex, !bEnable, u8ByteMask);

        if(u8IntRemap < 16)
        {
            SC_W2BYTEMSK(REG_SC_BK00_12_L ,BIT(u8IntRemap),BIT(u8IntRemap));
            SC_W2BYTEMSK(REG_SC_BK00_14_L , 0 ,BIT(u8IntRemap));
        }
        else
        {
            SC_W2BYTEMSK(REG_SC_BK00_13_L ,BIT(u8IntRemap-16),BIT(u8IntRemap-16));
            SC_W2BYTEMSK(REG_SC_BK00_15_L , 0 ,BIT(u8IntRemap-16));
        }
    }
    else
    {

        // mask it
        //MDrv_WriteRegBit(L_BK_GOPINT(0x14) + u8ByteIndex, !bEnable, u8ByteMask);

        // clear interrupt
        //MDrv_WriteRegBit(L_BK_GOPINT(0x12) + u8ByteIndex, bEnable, u8ByteMask);

        if(u8IntRemap < 16)
        {
            SC_W2BYTEMSK(REG_SC_BK00_14_L , BIT(u8IntRemap),BIT(u8IntRemap));
            SC_W2BYTEMSK(REG_SC_BK00_12_L , 0,BIT(u8IntRemap));
        }
        else
        {
            SC_W2BYTEMSK(REG_SC_BK00_15_L ,BIT(u8IntRemap-16),BIT(u8IntRemap-16));
            SC_W2BYTEMSK(REG_SC_BK00_13_L , 0 ,BIT(u8IntRemap-16));
        }

    }

    MDrv_WriteByte(BK_SELECT_00, u8Bank);
}

void MDrv_SC_get_rgb_maxmin_from_ip1f2(void)
{
    //MS_U8 u8Bank;
    static MS_U8 u8Cnt = 0;

    //u8Bank = MDrv_ReadByte(BK_SELECT_00);
    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);

    //MDrv_WriteByte(L_BK_IP1F2(0x0E), 0x11); // enable ATG
    SC_W2BYTEMSK(REG_SC_BK01_0E_L, 0x11,0x00FF); // enable ATG
    // the min value need to wait 1 Frame.
    // Read Cr->Y->Cb, Get Cb->Cr->Y
    switch(u8Cnt%3)
    {
    default:
    case 0:
        /*MDrv_WriteByteMask(L_BK_IP1F2(0x0F), 0x00, ( BIT(2) | BIT(1) | BIT(0) ));
        g_u8DLC_Cbmin_From_IP1F2 = MDrv_ReadByte(L_BK_IP1F2(0x11));
        MDrv_WriteByteMask(L_BK_IP1F2(0x0F), 0x03, ( BIT(2) | BIT(1) | BIT(0) ));
        g_u8DLC_Crmax_From_IP1F2 = MDrv_ReadByte(L_BK_IP1F2(0x11));*/
        SC_W2BYTEMSK(REG_SC_BK01_0F_L, 0x00, ( BIT(2) | BIT(1) | BIT(0) ));
        g_u8DLC_Cbmin_From_IP1F2 = SC_R2BYTE(REG_SC_BK01_11_L) & 0x00FF;
        SC_W2BYTEMSK(REG_SC_BK01_0F_L, 0x03, ( BIT(2) | BIT(1) | BIT(0) ));
        g_u8DLC_Crmax_From_IP1F2 = SC_R2BYTE(REG_SC_BK01_11_L) & 0x00FF;
        break;

    case 1:
        /*MDrv_WriteByteMask(L_BK_IP1F2(0x0F), 0x01, ( BIT(2) | BIT(1) | BIT(0) ));
        g_u8DLC_Crmin_From_IP1F2 = MDrv_ReadByte(L_BK_IP1F2(0x11));
        MDrv_WriteByteMask(L_BK_IP1F2(0x0F), 0x04,( BIT(2) | BIT(1) | BIT(0) ));
        g_u8DLC_Ymax_From_IP1F2 = MDrv_ReadByte(L_BK_IP1F2(0x11));*/
        SC_W2BYTEMSK(REG_SC_BK01_0F_L, 0x01, ( BIT(2) | BIT(1) | BIT(0) ));
        g_u8DLC_Cbmin_From_IP1F2 = SC_R2BYTE(REG_SC_BK01_11_L) & 0x00FF;
        SC_W2BYTEMSK(REG_SC_BK01_0F_L, 0x04, ( BIT(2) | BIT(1) | BIT(0) ));
        g_u8DLC_Crmax_From_IP1F2 = SC_R2BYTE(REG_SC_BK01_11_L) & 0x00FF;
        break;

    case 2:
        /*MDrv_WriteByteMask(L_BK_IP1F2(0x0F), 0x02, ( BIT(2) | BIT(1) | BIT(0) ));
        g_u8DLC_Ymin_From_IP1F2 = MDrv_ReadByte(L_BK_IP1F2(0x11));
        MDrv_WriteByteMask(L_BK_IP1F2(0x0F), 0x05, ( BIT(2) | BIT(1) | BIT(0) ));
        g_u8DLC_Cbmax_From_IP1F2 = MDrv_ReadByte(L_BK_IP1F2(0x11));*/
        SC_W2BYTEMSK(REG_SC_BK01_0F_L, 0x02, ( BIT(2) | BIT(1) | BIT(0) ));
        g_u8DLC_Cbmin_From_IP1F2 = SC_R2BYTE(REG_SC_BK01_11_L) & 0x00FF;
        SC_W2BYTEMSK(REG_SC_BK01_0F_L, 0x05, ( BIT(2) | BIT(1) | BIT(0) ));
        g_u8DLC_Crmax_From_IP1F2 = SC_R2BYTE(REG_SC_BK01_11_L) & 0x00FF;
        break;
    }
    u8Cnt++;

    //MDrv_WriteByte(BK_SELECT_00, u8Bank);

}

void MDrv_SC_get_y_maxmin_from_ip1f2(void)
{
    //MS_U8 u8Bank;

    //u8Bank = MDrv_ReadByte(BK_SELECT_00);
    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);

    //MDrv_WriteByte(L_BK_IP1F2(0x0E), 0x11); // enable ATG
    SC_W2BYTEMSK(REG_SC_BK01_0E_L, 0x11,0x00FF); // enable ATG
    // read YMin
    //MDrv_WriteByteMask(L_BK_IP1F2(0x0F), 0x01, BIT(2) | BIT(1) | BIT(0));
    SC_W2BYTEMSK(REG_SC_BK01_0F_L, 0x01, BIT(2) | BIT(1) | BIT(0));
    // prevent YUV input under shoot, to speed up, we try to do some adjust if YMin < 16
    //if(MDrv_ReadByte(L_BK_IP1F2(0x11)) >= 16)
    if((SC_R2BYTE(REG_SC_BK01_11_L) & 0x00FF) >= 16)
    {
        //g_u8DLC_Ymin_From_IP1F2 = MDrv_ReadByte(L_BK_IP1F2(0x11));
        g_u8DLC_Ymin_From_IP1F2 = SC_R2BYTE(REG_SC_BK01_11_L) & 0x00FF;
    }
    else
    {
        if(0)   //(mvideo_sc_is_pre_rgb2yuv(MAIN_WINDOW)), Daten FixMe
        {
            //g_u8DLC_Ymin_From_IP1F2 = MDrv_ReadByte(L_BK_IP1F2(0x11));
            g_u8DLC_Ymin_From_IP1F2 = SC_R2BYTE(REG_SC_BK01_11_L) & 0x00FF;
        }
        else
        {
            g_u8DLC_Ymin_From_IP1F2 = 16;           // YUV min value is 16
        }
    }

    // read YMax
    //MDrv_WriteByteMask(L_BK_IP1F2(0x0F), 0x04, ( BIT(2) | BIT(1) | BIT(0) ));
    SC_W2BYTEMSK(REG_SC_BK01_0F_L, 0x0004, ( BIT(2) | BIT(1) | BIT(0) ));
    // prevent YUV input over shoot, to speed up, we try to do some adjust if Ymax > 235
    //if(MDrv_ReadByte(L_BK_IP1F2(0x11)) <= 235)
    if((SC_R2BYTE(REG_SC_BK01_11_L) & 0x00FF) <= 235)
    {
        g_u8DLC_Ymax_From_IP1F2 = SC_R2BYTE(REG_SC_BK01_11_L) & 0x00FF;
    }
    else
    {
        if(0)   //(mvideo_sc_is_pre_rgb2yuv(MAIN_WINDOW)), Daten FixMe
        {
            g_u8DLC_Ymax_From_IP1F2 = SC_R2BYTE(REG_SC_BK01_11_L) & 0x00FF;
        }
        else
        {
            g_u8DLC_Ymax_From_IP1F2 = 235;           // YUV max value is 235
        }
    }

    //MDrv_WriteByte(BK_SELECT_00, u8Bank);
}

#ifdef MSOS_TYPE_LINUX_KERNEL
void MDrv_SC_vsync_isr(void)
{
#if 0 //ToDo.. Ryan
    if (_XC_VSyncRun)
    {
        _XC_VSyncCount++;
        if (_XC_VSyncCount >= _XC_VSyncMax)
        {
            _XC_EventFlag = 1;
            wake_up(&_XC_EventQueue);
        }
    }
#endif
}
#endif // #ifdef MSOS_TYPE_LINUX_KERNEL

#if 0
typedef struct
{
    MS_U32 u32DS_H_Ratio;
    MS_U32 u32DS_V_Ratio;
    MS_U16 u16FrameCnt;
    MS_U16 u16H_Cap;
    MS_U16 u16PhaseErr;
    MS_U16 u16PrdErr;
} DS_Info_structure;        // start from 0x02C00000

typedef struct
{
    MS_U32 u32DS_H_Ratio;
    MS_U32 u32DS_V_Ratio;
    MS_U32 u32Dummy0, u32Dummy1;
} DS_Info_brief_structure;

#define SWAP(x)     ((x >> 16) | ((x & 0xFFFF) << 16))

void MDrv_SC_vsync_isr(void)
{
    DS_Info_structure *pDS_Info;
    DS_Info_brief_structure *pDS_Brief_Info;
    static MS_U16 u16Idx = 0;
    //static MS_BOOL bMemCleared = TRUE;
    static MS_U16 u16FrameCnt = 0;

    if(SC_R2BYTE(REG_SC_BK01_02_L) & BIT(7))
    {
        u16FrameCnt = 0;
        return;
    }
    else
    {
        // delay at least 8 lines to dump data
        printf("%06d\n", u16FrameCnt);
    }

    ///////////////////////
    // fill DS_Info
    ///////////////////////
    // assign address
    pDS_Info = (DS_Info_structure *) MsOS_PA2KSEG1(0x02C20000);
    pDS_Info += u16Idx;

    // fill memory
    pDS_Info->u16FrameCnt = u16FrameCnt++;//SC_R2BYTE(REG_SC_BK12_1D_L);
//    pDS_Info->u8DisplayIdx = 0x00;//SC_R2BYTE(REG_SC_BK12_1E_L) & 0xFF;
//    pDS_Info->u8ScalerIdx = 0x00;//(SC_R2BYTE(REG_SC_BK12_1E_L) >> 8) & 0xFF;
    pDS_Info->u32DS_H_Ratio = SWAP(SC_R4BYTE(REG_SC_BK23_1C_L));
    pDS_Info->u32DS_V_Ratio = SWAP(SC_R4BYTE(REG_SC_BK23_1E_L));
    pDS_Info->u16H_Cap = SC_R2BYTE(REG_SC_BK01_07_L);
    pDS_Info->u16PhaseErr = MDrv_Read2Byte(L_BK_LPLL(0x11));
    pDS_Info->u16PrdErr = MDrv_Read2Byte(L_BK_LPLL(0x13));

    ///////////////////////
    // fill DS_Brief_Info
    ///////////////////////
    // assign address
    pDS_Brief_Info = (DS_Info_brief_structure *) MsOS_PA2KSEG1(0x02CA0000);
    pDS_Brief_Info += u16Idx;

    // fill memory
    pDS_Brief_Info->u32DS_H_Ratio = SWAP(SC_R4BYTE(REG_SC_BK23_1C_L));
    pDS_Brief_Info->u32DS_V_Ratio = SWAP(SC_R4BYTE(REG_SC_BK23_1E_L));
    pDS_Brief_Info->u32Dummy0 = pDS_Brief_Info->u32Dummy1 = 0;

    u16Idx++;
    if(u16Idx > 32768) u16Idx = 0;
}
#endif

#ifdef SIG_INT_EXAMPLE
int  usr_pid;
void notify_user(int usr)
{
    struct task_struct *p = NULL;
    mm_segment_t old_fs;
#if 1
    struct file *fp;
    char pid[8];

    fp = filp_open("/data/usr.pid", O_RDONLY, 0);
    if (IS_ERR(fp))
    {
        printk(KERN_ERR  ": file open fail\n");
        return;
    }
    
    if (fp->f_op && fp->f_op->read) {
        old_fs=get_fs(); 
        set_fs(KERNEL_DS); 
        if (fp->f_op->read(fp, pid, 8, &fp->f_pos) > 0) {
           // printk(KERN_ERR  "before find_task_by_vpid %d\n",simple_strtoul(pid, NULL, 10));
           // p = find_task_by_vpid(simple_strtoul(pid, NULL, 10));
           usr_pid = simple_strtoul(pid, NULL, 10);
           p = pid_task(find_vpid(simple_strtoul(pid, NULL, 10)), PIDTYPE_PID);
            if (NULL == p) {
                printk(KERN_ERR  "struct task_struct NULL\n");
            }
        }
       set_fs(old_fs);
    }
    filp_close(fp, NULL);
#else
    p = find_task_by_vpid(usr_pid);
#endif
    
    if (NULL == p) {
        printk(KERN_ERR  ": no registered process to notify\n");
        return;
    }

    if (usr == 1) {
        printk(KERN_NOTICE  ": sending a SIGUSR1 to process %d\n",
                usr_pid);
        send_sig(SIGUSR1, p, 0);
    } else if (usr == 2) {
        printk(KERN_NOTICE  ": sending a SIGUSR2 to process %d\n",
                usr_pid);
        send_sig(SIGUSR2, p, 0);
    }

}


#endif

#ifdef MSOS_TYPE_LINUX_KERNEL
irqreturn_t  MDrv_SC_isr(int eIntNum, void* dev_id)
#else
void MDrv_SC_isr( InterruptNum eIntNum )
#endif
{
    MS_U32 u32IntSource;
    SC_INT_SRC eSCIntNum;
    MS_U8 i;

	#ifdef SIG_INT_EXAMPLE
    if (printk_ratelimit())
        printk(KERN_EMERG "INTERRUPT!\n");
    notify_user(1);
    MDrv_SC_set_interrupt(SC_INT_VSINT, DISABLE);  
	
	#endif
	
    #if (!ENABLE_REGISTER_SPREAD)
    MS_U8 u8OldISRBank;
    #endif

    UNUSED(eIntNum);

    // we can't quarantee ISR thread not being context switched out, so if we change bank here, it's very dangerous
    // need to obtain mutex here
#ifdef MSOS_TYPE_LINUX_KERNEL
    #define _XC_MUTEX_WAIT_TIMEOUT  10
    if(!MsOS_ObtainMutex(_XC_Mutex, _XC_MUTEX_WAIT_TIMEOUT))
    {
        SC_ISR_DBG(printf("ISR return\n"));

        // re-enable IRQ_DISP
        ///MsOS_EnableInterrupt(E_INT_IRQ_DISP);
        return IRQ_HANDLED;
     }
#else
    if(!MsOS_ObtainMutex(_XC_Mutex, MSOS_WAIT_FOREVER))
    {
        SC_ISR_DBG(printf("ISR return\n"));

        // re-enable IRQ_DISP
        MsOS_EnableInterrupt(E_INT_IRQ_DISP);
        return;
    }
#endif
    // read XC ISR status

    #if (!ENABLE_REGISTER_SPREAD)
    u8OldISRBank = MDrv_ReadByte(BK_SELECT_00);
    #endif

    u32IntSource = SC_R4BYTE(REG_SC_BK00_10_L);

    u32IntSource = u32IntSource << (SC_INT_START - IRQ_INT_START);
    SC_ISR_DBG(printf("ISR %8lx\n", u32IntSource));

#ifdef MSOS_TYPE_LINUX_KERNEL
    if(u32IntSource & (1 << SC_INT_VSINT))
    {
        MDrv_SC_set_interrupt(SC_INT_VSINT, DISABLE);
        MDrv_SC_vsync_isr();
        MDrv_SC_set_interrupt(SC_INT_VSINT, ENABLE);
    }
#endif

    for(eSCIntNum=SC_INT_START; eSCIntNum<MAX_SC_INT; eSCIntNum++)
    {
        if(u32IntSource & (1 << eSCIntNum))
        {
            // this Interrupt happened, clear it
            _MDrv_SC_clear_interrupt(eSCIntNum);

            // call ISR
            for(i=0; i<MAX_ISR_NUM_OF_EACH_INT; i++)
            {
                if(stXCIsrInfo[eSCIntNum][i].aryXC_ISR != NULL)
                {
                    // execute ISR
                    (void)(stXCIsrInfo[eSCIntNum][i].aryXC_ISR) ((SC_INT_SRC)eSCIntNum, stXCIsrInfo[eSCIntNum][i].aryXC_ISR_Param);
                }
            }
        }
    }

    #if (!ENABLE_REGISTER_SPREAD)
    MDrv_WriteByte(BK_SELECT_00, u8OldISRBank);
    #endif

    MsOS_ReleaseMutex(_XC_Mutex);

#ifdef MSOS_TYPE_LINUX_KERNEL
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
MS_U8   MDrv_XC_InterruptGetMaxIntNum(void)
{
    return MAX_SC_INT;
}

//-------------------------------------------------------------------------------------------------
/// After get maximum number of interrupts by @ref MDrv_XC_InterruptGetMaxIntNum, you can
/// check each interrupt supported btuy scaler or not.
/// @param eIntNum      \b IN: the interrupt number to query
/// @return @ref MS_BOOL the interrupt number supported or not
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_XC_InterruptAvaliable(SC_INT_SRC eIntNum)
{
    MS_U8 i;
    MS_BOOL bReturn = FALSE;

    // check if out of range or not supported
    if((eIntNum >= MDrv_XC_InterruptGetMaxIntNum()) || (((1UL << eIntNum) & SUPPORTED_XC_INT) == 0))
    {
        goto Exit;
    }

    // search avaliable CB to attach
    for(i=0; i<MAX_ISR_NUM_OF_EACH_INT; i++)
    {
        if(stXCIsrInfo[eIntNum][i].aryXC_ISR == NULL)
        {
            bReturn = TRUE;
            break;
        }
    }

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
MS_BOOL MDrv_XC_InterruptIsAttached(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam)
{
    MS_U8 i;

    for(i=0; i<MAX_ISR_NUM_OF_EACH_INT; i++)
    {
        if((stXCIsrInfo[eIntNum][i].aryXC_ISR == pIntCb) && (stXCIsrInfo[eIntNum][i].aryXC_ISR_Param == pParam))
        {
            return TRUE;
        }
    }

    return FALSE;
}

//-------------------------------------------------------------------------------------------------
/// Attach interrupt to specified interrupt number
/// @param eIntNum @ref SC_INT_SRC          \b IN: the interrupt number that you want to attach
/// @param pIntCb  @ref SC_InterruptCb      \b IN: the ISR that you want to attach to the interrupt number
/// @param pParam                           \b IN: the parameter that you want to pass to the ISR when called
/// @return @ref MS_BOOL attach successed or not, FALSE means can't attach more ISR
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_XC_InterruptAttach(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam)
{
    MS_BOOL bReturn = FALSE;
    _XC_ENTRY_MUTEX(_XC_Mutex);
    bReturn = MDrv_XC_InterruptAttachWithoutMutex(eIntNum, pIntCb, pParam);
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
MS_BOOL MDrv_XC_InterruptDeAttach(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam)
{
    MS_BOOL bReturn = FALSE;

    _XC_ENTRY_MUTEX(_XC_Mutex);
    bReturn = MDrv_XC_InterruptDeAttachWithoutMutex(eIntNum, pIntCb, pParam);

    _XC_RETURN_MUTEX(_XC_Mutex);
    return bReturn;
}

MS_BOOL MDrv_XC_InterruptAttachWithoutMutex(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam)
{
    MS_U8 i;
    MS_BOOL bReturn = FALSE;

    // check if out of range or not supported
    if((eIntNum >= MDrv_XC_InterruptGetMaxIntNum()) || (((1UL << eIntNum) & SUPPORTED_XC_INT) == 0))
    {
     SC_ISR_DBG(printf("ISRAttach failed %d (max %d, supported %lx)\n", eIntNum, MDrv_XC_InterruptGetMaxIntNum(), (MS_U32)SUPPORTED_XC_INT));
        goto Exit;
    }

    // attach ISR
    for(i=0; i<MAX_ISR_NUM_OF_EACH_INT; i++)
    {
        if(stXCIsrInfo[eIntNum][i].aryXC_ISR == NULL)
        {
            // disable interrupt first to avoid racing condition, for example, interrupt come in after set pIntCb but before set pParam, will call ISR and passing parameter NULL
            MDrv_SC_set_interrupt(eIntNum, DISABLE);

            stXCIsrInfo[eIntNum][i].aryXC_ISR_Param = pParam;
            stXCIsrInfo[eIntNum][i].aryXC_ISR = pIntCb;

            // enable interrupt
            MDrv_SC_set_interrupt(eIntNum, ENABLE);

            SC_ISR_DBG(printf("ISRAttach %d successed to [%d][%d]\n", eIntNum, eIntNum, i));

            bReturn = TRUE;
            break;
        }
    }

Exit:
    return bReturn;
}


MS_BOOL MDrv_XC_InterruptDeAttachWithoutMutex(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam)
{
    MS_U8 i;
    MS_BOOL bReturn = FALSE;

    // check if out of range or not supported
    if((eIntNum >= MDrv_XC_InterruptGetMaxIntNum()) || (((1UL << eIntNum) & SUPPORTED_XC_INT) == 0))
    {
        SC_ISR_DBG(printf("ISRDeAttach failed %d (max %d, supported %lx)\n", eIntNum, MDrv_XC_InterruptGetMaxIntNum(), SUPPORTED_XC_INT));
        goto Exit;
    }

    // de-attach the ISR
    for(i=0; i<MAX_ISR_NUM_OF_EACH_INT; i++)
    {
        if((stXCIsrInfo[eIntNum][i].aryXC_ISR == pIntCb) &&
            (stXCIsrInfo[eIntNum][i].aryXC_ISR_Param == pParam))
        {
            // disable interrupt first to avoid racing condition
            MDrv_SC_set_interrupt(eIntNum, DISABLE);

            stXCIsrInfo[eIntNum][i].aryXC_ISR = NULL;
            stXCIsrInfo[eIntNum][i].aryXC_ISR_Param = NULL;
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
            if(stXCIsrInfo[eIntNum][i].aryXC_ISR != NULL)
            {
                // re-enable interrupt
                MDrv_SC_set_interrupt(eIntNum, ENABLE);
                SC_ISR_DBG(printf("ISRAttach %d re-enable ISR\n", eIntNum));
            }
        }
    }

Exit:

    return bReturn;
}
#undef _MDRV_XC_ISR_C_

