///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2008 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mdrv_temp.c
/// @brief  TEMP Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <asm/io.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/time.h>
#include <linux/delay.h>

//drver header files
#include "chip_int.h"
#include "mdrv_mstypes.h"
#include "reg_sc.h"
#include "mhal_sc.h"
#include "mdrv_sc.h"


//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define SC_MAX_CONT_SEND_LEN        (24)

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_SC_INT_TX_LEVEL = 0x00000001,
    E_SC_INT_CARD_IN = 0x00000002, //UART_SCSR_INT_CARDIN
    E_SC_INT_CARD_OUT = 0x00000004, //UART_SCSR_INT_CARDOUT
    E_SC_INT_CGT_TX_FAIL = 0x00000008,
    E_SC_INT_CGT_RX_FAIL = 0x00000010,
    E_SC_INT_CWT_TX_FAIL = 0x00000020,
    E_SC_INT_CWT_RX_FAIL = 0x00000040,
    E_SC_INT_BGT_FAIL = 0x00000080,
    E_SC_INT_BWT_FAIL = 0x00000100,
    E_SC_INT_PE_FAIL = 0x00000200,
    E_SC_INT_RST_TO_ATR_FAIL = 0x00000400,
    E_SC_INT_INVALID = 0xFFFFFFFF
}SC_INT_BIT_MAP;

typedef enum
{
    E_SC_ATTR_INVALID = 0x00000000,
    E_SC_ATTR_TX_LEVEL = 0x00000001
}SC_ATTR_TYPE;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static SC_Info  _scInfo[SC_DEV_NUM] = {
    {
        .bCardIn        = FALSE,
        .bLastCardIn    = FALSE,
        .u32CardStatus  = 0,
        .u16FifoRxRead  = 0,
        .u16FifoRxWrite = 0,
        .u16FifoTxRead  = 0,
        .u16FifoTxWrite = 0,
        .u32CardAttr    = E_SC_ATTR_INVALID,
    },
#if (SC_DEV_NUM > 1) // no more than 2
    {
        .bCardIn        = FALSE,
        .bLastCardIn    = FALSE,
        .u32CardStatus  = 0,
        .u16FifoRxRead  = 0,
        .u16FifoRxWrite = 0,
        .u16FifoTxRead  = 0,
        .u16FifoTxWrite = 0,
        .u32CardAttr    = E_SC_ATTR_INVALID,
    }
#endif
};

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static U32 _MDrv_SC_GetTimeUs(void)
{
    struct timeval stTime;
    U32 u32Val;

    do_gettimeofday(&stTime);

    u32Val = (U32)(stTime.tv_sec * 1000000 + stTime.tv_usec);

    return u32Val;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

BOOL MDrv_SC_ISR_Proc(U8 u8SCID)
{
    HAL_SC_TX_LEVEL_GWT_INT stTxLevelGWT_Int;
    U8  u8Reg;
    U32 cnt;
    U32 idx;
    BOOL bWakeUp = FALSE;

    // Try to get timing fail flag
    if (HAL_SC_GetIntTxLevelAndGWT(u8SCID, &stTxLevelGWT_Int))
    {
        if (stTxLevelGWT_Int.bTxLevelInt || stTxLevelGWT_Int.bCGT_TxFail ||
            stTxLevelGWT_Int.bCGT_RxFail || stTxLevelGWT_Int.bCWT_TxFail ||
            stTxLevelGWT_Int.bCWT_RxFail || stTxLevelGWT_Int.bBGT_Fail || stTxLevelGWT_Int.bBWT_Fail)
        {
            if (stTxLevelGWT_Int.bTxLevelInt)
            {
                _scInfo[u8SCID].u32CardStatus |= E_SC_INT_TX_LEVEL;
            }
            if (stTxLevelGWT_Int.bCWT_RxFail)  //CWT RX INT
            {
                _scInfo[u8SCID].u32CardStatus |= E_SC_INT_CWT_RX_FAIL;
            }
            if (stTxLevelGWT_Int.bCWT_TxFail)  //CWT TX INT
            {
                _scInfo[u8SCID].u32CardStatus |= E_SC_INT_CWT_TX_FAIL;
            }
            if (stTxLevelGWT_Int.bCGT_RxFail)  //CGT RX INT
            {
                _scInfo[u8SCID].u32CardStatus |= E_SC_INT_CGT_RX_FAIL;
            }
            if (stTxLevelGWT_Int.bCGT_TxFail)  //CGT TX INT
            {
                _scInfo[u8SCID].u32CardStatus |= E_SC_INT_CGT_TX_FAIL;
            }
            if (stTxLevelGWT_Int.bBGT_Fail)  //BGT INT
            {
                _scInfo[u8SCID].u32CardStatus |= E_SC_INT_BGT_FAIL;
            }
            if (stTxLevelGWT_Int.bBWT_Fail)  //BWT INT
            {
                //Check if BWT can be instead of ext-CWT to check timeout
                //If yes, to clear RST_TO_IO_EDGE_DET_EN flag to prevnet BWT fail int is always triggered
                if (HAL_SC_IsBwtInsteadExtCwt(u8SCID))
                {
                    HAL_SC_RstToIoEdgeDetCtrl(u8SCID, FALSE);
                }
                _scInfo[u8SCID].u32CardStatus |= E_SC_INT_BWT_FAIL;
            }

            // Clear int flag
            HAL_SC_ClearIntTxLevelAndGWT(u8SCID);
            bWakeUp = TRUE;

            if (stTxLevelGWT_Int.bTxLevelInt)
            {
                cnt = 0;
                while(1)
                {
                    if (_scInfo[u8SCID].u16FifoTxRead == _scInfo[u8SCID].u16FifoTxWrite)
                        break;

                    SC_WRITE(u8SCID, UART_TX, _scInfo[u8SCID].u8FifoTx[_scInfo[u8SCID].u16FifoTxRead++]);
                    cnt++;
                    if (_scInfo[u8SCID].u16FifoTxRead == SC_FIFO_SIZE)
                    {
                        _scInfo[u8SCID].u16FifoTxRead = 0;
                    }
                    else if (_scInfo[u8SCID].u16FifoTxRead == _scInfo[u8SCID].u16FifoTxWrite)
                    {
                        break;
                    }
                    else
                    {
                        if (cnt >= 16)
                            break;
                    }
                }
            }
        }
    }

    u8Reg = SC_READ(u8SCID, UART_IIR);
    if (HAL_SC_IsPendingINT(u8Reg))
    {
        u8Reg = HAL_SC_GetLsr(u8SCID);
        while (u8Reg & (UART_LSR_DR | UART_LSR_BI))
        {
            bWakeUp = TRUE;
            _scInfo[u8SCID].u8FifoRx[_scInfo[u8SCID].u16FifoRxWrite] = SC_READ(u8SCID, UART_RX);

            if ((_scInfo[u8SCID].u32CardStatus & E_SC_INT_CWT_RX_FAIL))
            {
                // Do nothing for CWT fail
            }
            else
            {
                if (u8Reg & UART_LSR_PE)
                {
                    _scInfo[u8SCID].u32CardStatus |= E_SC_INT_PE_FAIL;
                    break;
                }

                idx = _scInfo[u8SCID].u16FifoRxWrite + 1;
                if ((idx == SC_FIFO_SIZE) && (_scInfo[u8SCID].u16FifoRxRead != 0))
                {
                    // Not overflow but wrap
                    _scInfo[u8SCID].u16FifoRxWrite = 0;
                }
                else if (idx != _scInfo[u8SCID].u16FifoRxRead)
                {
                    // Not overflow
                    _scInfo[u8SCID].u16FifoRxWrite = idx;
                }
                else
                {
                    // overflow
                    printk("[%s][%d] RX buffer Overflow\n", __FUNCTION__, __LINE__);
                    break;
                }
            }

            u8Reg = HAL_SC_GetLsr(u8SCID);
        }

        if (u8Reg & UART_LSR_THRE)
        {
            cnt = 16;
            do
            {
                if (_scInfo[u8SCID].u16FifoTxRead == _scInfo[u8SCID].u16FifoTxWrite)
                    break;

                bWakeUp = TRUE;
                SC_WRITE(u8SCID, UART_TX, _scInfo[u8SCID].u8FifoTx[_scInfo[u8SCID].u16FifoTxRead++]);
                if (_scInfo[u8SCID].u16FifoTxRead == SC_FIFO_SIZE)
                {
                    _scInfo[u8SCID].u16FifoTxRead = 0;
                }

            } while (--cnt > 0);
        }
    }

    // Check special event from SMART
    u8Reg = SC_READ(u8SCID, UART_SCSR);
    if (u8Reg & (UART_SCSR_INT_CARDIN | UART_SCSR_INT_CARDOUT))
    {
        SC_WRITE(u8SCID, UART_SCSR, u8Reg); // clear interrupt
        _scInfo[u8SCID].u32CardStatus |= u8Reg & (UART_SCSR_INT_CARDIN | UART_SCSR_INT_CARDOUT);
        bWakeUp = TRUE;
    }

    //Check HW Rst to IO fail
    if (HAL_SC_CheckIntRstToIoEdgeFail(u8SCID))
    {
        HAL_SC_MaskIntRstToIoEdgeFail(u8SCID); //Mask int
        _scInfo[u8SCID].u32CardStatus |= E_SC_INT_RST_TO_ATR_FAIL;
        bWakeUp = TRUE;
    }

    if (bWakeUp)
    {
        wake_up_interruptible(&_scInfo[u8SCID].stWaitQue);
    }

    return TRUE; // handled
}

//-------------------------------------------------------------------------------------------------
/// Handle smart card Interrupt notification handler
/// @param  irq             \b IN: interrupt number
/// @param  devid           \b IN: device id
/// @return IRQ_HANDLED
/// @attention
//-------------------------------------------------------------------------------------------------
irqreturn_t MDrv_SC_ISR1(int irq, void *devid)
{
    if (!MDrv_SC_ISR_Proc(0))
    {
        SC_PRINT("ISR proc is failed\n");
    }

    return IRQ_HANDLED;
}

//-------------------------------------------------------------------------------------------------
/// Handle smart card Interrupt notification handler
/// @param  irq             \b IN: interrupt number
/// @param  devid           \b IN: device id
/// @return IRQ_HANDLED
/// @attention
//-------------------------------------------------------------------------------------------------
irqreturn_t MDrv_SC_ISR2(int irq, void *devid)
{
    if (!MDrv_SC_ISR_Proc(1))
    {
        SC_PRINT("ISR proc is failed\n");
    }

    return IRQ_HANDLED;
}

int MDrv_SC_Open(struct inode *inode, struct file *filp)
{
    U8 u8SCID = (U8)(int)filp->private_data;

    SC_PRINT("%s is invoked\n", __FUNCTION__);
    init_waitqueue_head(&_scInfo[u8SCID].stWaitQue);

    return 0;
}

ssize_t MDrv_SC_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    U8 u8SCID = (U8)(int)filp->private_data;
    ssize_t idx = 0;

    for (idx = 0; idx < count; idx++)
    {
        if (_scInfo[u8SCID].u16FifoRxWrite == _scInfo[u8SCID].u16FifoRxRead)
            break;

        buf[idx] = _scInfo[u8SCID].u8FifoRx[_scInfo[u8SCID].u16FifoRxRead++];
        if (_scInfo[u8SCID].u16FifoRxRead == SC_FIFO_SIZE)
        {
            _scInfo[u8SCID].u16FifoRxRead = 0;
        }
    }

    return idx;
}

ssize_t MDrv_SC_Write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    U8 u8SCID = (U8)(int)filp->private_data;
    ssize_t idx = 0;
    U32 tmp;
    U32 u32SendLen, u2Index;

    //Fill up SW TX FIFO
    for (idx = 0; idx < count; idx++)
    {
        _scInfo[u8SCID].u8FifoTx[_scInfo[u8SCID].u16FifoTxWrite] = buf[idx];

        tmp = _scInfo[u8SCID].u16FifoTxWrite + 1;
        if ((tmp == SC_FIFO_SIZE) && (_scInfo[u8SCID].u16FifoTxRead != 0))
        {
            // Not overflow but wrap
            _scInfo[u8SCID].u16FifoTxWrite = 0;
        }
        else if (tmp != _scInfo[u8SCID].u16FifoTxRead)
        {
            // Not overflow
            _scInfo[u8SCID].u16FifoTxWrite = tmp;
        }
        else
        {
            printk("[%s][%d] TX buffer Overflow\n", __FUNCTION__, __LINE__);
            break;
        }
    }

    //
    // If TX level attribute is set, then transmit TX data by TX level driven instead of TX buffer empty driven
    // This can avoid too large interval between 1st data byte and 2nd's
    //
    if (_scInfo[u8SCID].u32CardAttr & E_SC_ATTR_TX_LEVEL)
    {
        // To use tx level int in tx pkts send
        if (count > SC_MAX_CONT_SEND_LEN)
        {
            u32SendLen = SC_MAX_CONT_SEND_LEN;
        }
        else
        {
            u32SendLen = count;
        }
        for (u2Index = 0; u2Index < u32SendLen; u2Index++)
        {
            if (_scInfo[u8SCID].u16FifoTxRead == _scInfo[u8SCID].u16FifoTxWrite)
                break;

            SC_WRITE(u8SCID, UART_TX, _scInfo[u8SCID].u8FifoTx[_scInfo[u8SCID].u16FifoTxRead++]);
            if (_scInfo[u8SCID].u16FifoTxRead == SC_FIFO_SIZE)
            {
                _scInfo[u8SCID].u16FifoTxRead = 0;
            }
            else if (_scInfo[u8SCID].u16FifoTxRead == _scInfo[u8SCID].u16FifoTxWrite)
            {
                break;
            }
        }
    }
    else
    {
        if ((SC_READ(u8SCID, UART_LSR) & UART_LSR_THRE) &&
                (_scInfo[u8SCID].u16FifoTxRead != _scInfo[u8SCID].u16FifoTxWrite))
        {
            SC_WRITE(u8SCID, UART_TX, _scInfo[u8SCID].u8FifoTx[_scInfo[u8SCID].u16FifoTxRead++]);
            if (_scInfo[u8SCID].u16FifoTxRead == SC_FIFO_SIZE)
            {
                _scInfo[u8SCID].u16FifoTxRead = 0;
            }

        }
    }

    return idx;
}

unsigned int MDrv_SC_Poll(struct file *filp, poll_table *wait)
{
    U8 u8SCID = (U8)(int)filp->private_data;
    unsigned int mask = 0;

    poll_wait(filp, &_scInfo[u8SCID].stWaitQue, wait);
    if (_scInfo[u8SCID].u16FifoRxRead != _scInfo[u8SCID].u16FifoRxWrite)
    {
        mask |= POLLIN;
    }
    if (_scInfo[u8SCID].u32CardStatus)
    {
        mask |= POLLPRI;
    }

    return mask;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
int MDrv_SC_AttachInterrupt(struct file *filp, unsigned long arg)
#else
int MDrv_SC_AttachInterrupt(struct inode *inode, struct file *filp, unsigned long arg)
#endif
{
    U8 u8SCID = (U8)(int)filp->private_data;

    SC_PRINT("%s is invoked\n", __FUNCTION__);

    if (u8SCID == 0)
    {
        if (request_irq(SC_IRQ, MDrv_SC_ISR1, SA_INTERRUPT, "SC", NULL))
        {
            SC_PRINT("SC IRQ1 registartion ERROR\n");
        }
        else
        {
            SC_PRINT("SC IRQ1 registartion OK\n");
        }
    }
#if (SC_DEV_NUM > 1) // no more than 2
    else if (u8SCID == 1)
    {
        if (request_irq(SC_IRQ2, MDrv_SC_ISR2, SA_INTERRUPT, "SC", NULL))
        {
            SC_PRINT("SC IRQ2 registartion ERROR\n");
        }
        else
        {
            SC_PRINT("SC IRQ2 registartion OK\n");
        }
    }
#endif
    return 0;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
int MDrv_SC_DetachInterrupt(struct file *filp, unsigned long arg)
#else
int MDrv_SC_DetachInterrupt(struct inode *inode, struct file *filp, unsigned long arg)
#endif
{
    U8 u8SCID = (U8)(int)filp->private_data;

    SC_PRINT("%s is invoked\n", __FUNCTION__);
    if (u8SCID == 0)
    {
        free_irq(SC_IRQ, NULL);
    }
#if (SC_DEV_NUM > 1) // no more than 2
    else if (u8SCID == 1)
    {
        free_irq(SC_IRQ2, NULL);
    }
#endif

    return 0;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
int MDrv_SC_ResetFIFO(struct file *filp, unsigned long arg)
#else
int MDrv_SC_ResetFIFO(struct inode *inode, struct file *filp, unsigned long arg)
#endif
{
    U8 u8SCID = (U8)(int)filp->private_data;

    SC_PRINT("%s is invoked\n", __FUNCTION__);
    _scInfo[u8SCID].u16FifoRxRead   = 0;
    _scInfo[u8SCID].u16FifoRxWrite  = 0;
    _scInfo[u8SCID].u16FifoTxRead   = 0;
    _scInfo[u8SCID].u16FifoTxWrite  = 0;

    return 0;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
int MDrv_SC_GetEvent(struct file *filp, unsigned long arg)
#else
int MDrv_SC_GetEvent(struct inode *inode, struct file *filp, unsigned long arg)
#endif
{
    U8 u8SCID = (U8)(int)filp->private_data;

    put_user(_scInfo[u8SCID].u32CardStatus, (int __user *)arg);
    _scInfo[u8SCID].u32CardStatus = 0;

    return 0;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
int MDrv_SC_SetEvent(struct file *filp, unsigned long arg)
#else
int MDrv_SC_SetEvent(struct inode *inode, struct file *filp, unsigned long arg)
#endif
{
    U8 u8SCID = (U8)(int)filp->private_data;

    get_user(_scInfo[u8SCID].u32CardStatus, (int __user *)arg);

    return 0;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
int MDrv_SC_GetAttribute(struct file *filp, unsigned long arg)
#else
int MDrv_SC_GetAttribute(struct inode *inode, struct file *filp, unsigned long arg)
#endif
{
    U8 u8SCID = (U8)(int)filp->private_data;

    put_user(_scInfo[u8SCID].u32CardAttr, (int __user *)arg);

    return 0;
}


#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
int MDrv_SC_SetAttribute(struct file *filp, unsigned long arg)
#else
int MDrv_SC_SetAttribute(struct inode *inode, struct file *filp, unsigned long arg)
#endif
{
    U8 u8SCID = (U8)(int)filp->private_data;

    get_user(_scInfo[u8SCID].u32CardAttr, (int __user *)arg);

    return 0;
}

////////////////////////////
//MDrv_SC_CheckRstToATR is a SW patch function for rst_to_io detect
////////////////////////////
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
int MDrv_SC_CheckRstToATR(struct file *filp, unsigned long arg)
#else
int MDrv_SC_CheckRstToATR(struct inode *inode, struct file *filp, unsigned long arg)
#endif
{
    U32 u32StartTime = _MDrv_SC_GetTimeUs();
    U8 u8SCID = (U8)(int)filp->private_data;
    U32 u32RstToAtrPeriod, u32CurTime;

    get_user(u32RstToAtrPeriod, (int __user *)arg);

    u32CurTime = _MDrv_SC_GetTimeUs();

    while ((u32CurTime - u32StartTime) <= u32RstToAtrPeriod)
    {
        udelay(10);

        u32CurTime = _MDrv_SC_GetTimeUs();

        if (_scInfo[u8SCID].u16FifoRxRead != _scInfo[u8SCID].u16FifoRxWrite)
            break;
    }

    if ((u32CurTime - u32StartTime) > u32RstToAtrPeriod)
        _scInfo[u8SCID].u32CardStatus |= E_SC_INT_RST_TO_ATR_FAIL;

    return 0;
}
