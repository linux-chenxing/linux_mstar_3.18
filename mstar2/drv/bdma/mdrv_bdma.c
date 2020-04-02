#include <linux/module.h>
#include <linux/init.h>
#include <linux/autoconf.h>
#include <linux/mii.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/pci.h>
#include <linux/crc32.h>
#include <linux/ethtool.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/version.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

#include "mdrv_types.h"
#include "mhal_bdma.h"
#include "mdrv_bdma.h"

#define BDMA_WAIT_TIME              (HAL_BDMA_MS(100)*0x400)
#define BDMA_IS_TIMEOUT(x)          ((x) ? FALSE : TRUE)

struct mutex mutex;
bool bdma_mutex = false ; 


EXPORT_SYMBOL(MDrv_BDMA_CH1_MemCopy_MIU0toMIU0);
EXPORT_SYMBOL(MDrv_BDMA_CH1_MemCopy_MIU0toMIU1);
EXPORT_SYMBOL(MDrv_BDMA_CH1_MemCopy_MIU1toMIU0);
EXPORT_SYMBOL(MDrv_BDMA_CH1_MemCopy_MIU1toMIU1);

typedef struct _BDMA_OP_CB  //control block
{
    U8     u8OpCfg;
    U8     u8SrcDevCfg;
    U8     u8DstDevCfg;
    U8     u8DmyWrCnt;
    long  phy64SrcAddr;
    long  phy64DstAddr;
    U32    u32DataSize;
    U32    u32Cmd0;
    U32    u32Cmd1;
    U32    u32Cmd2;
    BDMA_Act        eAct;
    BDMA_ISR_CBF    pCbf;
}BDMA_OP_CB;

/////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_BDMA_CH0_MemCopy_MIU02MIU0
/// @brief \b Function  \b Description: Copy dram to dram 
/// @param phy64SrcAddr   \b IN: Physical Source address
/// @param phy64DstAddr   \b IN: Physical Dst address
/// @param u32Len       \b IN: data length
/// @return             \b BDMA_Result
////////////////////////////////////////////////////////////////////////////////
BDMA_Result MDrv_BDMA_CH0_MemCopy_MIU0toMIU0(long phy64SrcAddr, long phy64DstAddr, U32 u32Len)
{
    U8 eSrc, eDst;
    BDMA_CpyType eCpyType;

    eSrc = E_BDMA_DEV_MIU0;
    eDst = E_BDMA_DEV_MIU0;
    eCpyType = (BDMA_CpyType)(eSrc | _LShift(eDst, 8));
    return MDrv_BDMA_CopyHnd_internal(phy64SrcAddr, phy64DstAddr, u32Len, eCpyType, BDMA_OPCFG_DEF, 0);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_BDMA_CH0_MemCopy_MIU0toMIU1
/// @brief \b Function  \b Description: Copy dram to dram 
/// @param phy64SrcAddr   \b IN: Physical Source address
/// @param phy64DstAddr   \b IN: Physical Dst address
/// @param u32Len       \b IN: data length
/// @return             \b BDMA_Result
////////////////////////////////////////////////////////////////////////////////
BDMA_Result MDrv_BDMA_CH0_MemCopy_MIU0toMIU1(long phy64SrcAddr, long phy64DstAddr, U32 u32Len)
{
    U8 eSrc, eDst;
    BDMA_CpyType eCpyType;

    eSrc = E_BDMA_DEV_MIU0;
    eDst = E_BDMA_DEV_MIU1;
    eCpyType = (BDMA_CpyType)(eSrc | _LShift(eDst, 8));

    return MDrv_BDMA_CopyHnd_internal(phy64SrcAddr, phy64DstAddr, u32Len, eCpyType, BDMA_OPCFG_DEF, 0);
}
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_BDMA_CH0_MemCopy_MIU1toMIU0
/// @brief \b Function  \b Description: Copy dram to dram 
/// @param phy64SrcAddr   \b IN: Physical Source address
/// @param phy64DstAddr   \b IN: Physical Dst address
/// @param u32Len       \b IN: data length
/// @return             \b BDMA_Result
////////////////////////////////////////////////////////////////////////////////
BDMA_Result MDrv_BDMA_CH0_MemCopy_MIU1toMIU0(long phy64SrcAddr, long phy64DstAddr, U32 u32Len)
{
    U8 eSrc, eDst;
    BDMA_CpyType eCpyType;

    eSrc = E_BDMA_DEV_MIU1;
    eDst = E_BDMA_DEV_MIU0;
    eCpyType = (BDMA_CpyType)(eSrc | _LShift(eDst, 8));

    return MDrv_BDMA_CopyHnd_internal(phy64SrcAddr, phy64DstAddr, u32Len, eCpyType, BDMA_OPCFG_DEF, 0);
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_BDMA_CH0_MemCopy_MIU1toMIU1
/// @brief \b Function  \b Description: Copy dram to dram 
/// @param phy64SrcAddr   \b IN: Physical Source address
/// @param phy64DstAddr   \b IN: Physical Dst address
/// @param u32Len       \b IN: data length
/// @return             \b BDMA_Result
////////////////////////////////////////////////////////////////////////////////
BDMA_Result MDrv_BDMA_CH0_MemCopy_MIU1toMIU1(long phy64SrcAddr, long phy64DstAddr, U32 u32Len)
{
    U8 eSrc, eDst;
    BDMA_CpyType eCpyType;

    eSrc = E_BDMA_DEV_MIU1;
    eDst = E_BDMA_DEV_MIU1;
    eCpyType = (BDMA_CpyType)(eSrc | _LShift(eDst, 8));
    return MDrv_BDMA_CopyHnd_internal(phy64SrcAddr, phy64DstAddr, u32Len, eCpyType, BDMA_OPCFG_DEF, 0);
}

/////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_BDMA_CH1_MemCopy_MIU0toMIU0
/// @brief \b Function  \b Description: Copy dram to dram 
/// @param phy64SrcAddr   \b IN: Physical Source address
/// @param phy64DstAddr   \b IN: Physical Dst address
/// @param u32Len       \b IN: data length
/// @return             \b BDMA_Result
////////////////////////////////////////////////////////////////////////////////
BDMA_Result MDrv_BDMA_CH1_MemCopy_MIU0toMIU0(long phy64SrcAddr, long phy64DstAddr, U32 u32Len)
{
    U8 eSrc, eDst;
    BDMA_CpyType eCpyType;

    eSrc = E_BDMA_DEV_MIU0;
    eDst = E_BDMA_DEV_MIU0;
    eCpyType = (BDMA_CpyType)(eSrc | _LShift(eDst, 8));
    return MDrv_BDMA_CopyHnd_internal(phy64SrcAddr, phy64DstAddr, u32Len, eCpyType, BDMA_OPCFG_DEF, 1);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_BDMA_CH1_MemCopy_MIU0toMIU1
/// @brief \b Function  \b Description: Copy dram to dram 
/// @param phy64SrcAddr   \b IN: Physical Source address
/// @param phy64DstAddr   \b IN: Physical Dst address
/// @param u32Len       \b IN: data length
/// @return             \b BDMA_Result
////////////////////////////////////////////////////////////////////////////////
BDMA_Result MDrv_BDMA_CH1_MemCopy_MIU0toMIU1(long phy64SrcAddr, long phy64DstAddr, U32 u32Len)
{
    U8 eSrc, eDst;
    BDMA_CpyType eCpyType;

    eSrc = E_BDMA_DEV_MIU0;
    eDst = E_BDMA_DEV_MIU1;
    eCpyType = (BDMA_CpyType)(eSrc | _LShift(eDst, 8));
    return MDrv_BDMA_CopyHnd_internal(phy64SrcAddr, phy64DstAddr, u32Len, eCpyType, BDMA_OPCFG_DEF, 1);
}
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_BDMA_CH1_MemCopy_MIU1toMIU0
/// @brief \b Function  \b Description: Copy dram to dram 
/// @param phy64SrcAddr   \b IN: Physical Source address
/// @param phy64DstAddr   \b IN: Physical Dst address
/// @param u32Len       \b IN: data length
/// @return             \b BDMA_Result
////////////////////////////////////////////////////////////////////////////////
BDMA_Result MDrv_BDMA_CH1_MemCopy_MIU1toMIU0(long phy64SrcAddr, long phy64DstAddr, U32 u32Len)
{
    U8 eSrc, eDst;
    BDMA_CpyType eCpyType;

    eSrc = E_BDMA_DEV_MIU1;
    eDst = E_BDMA_DEV_MIU0;
    eCpyType = (BDMA_CpyType)(eSrc | _LShift(eDst, 8));
    return MDrv_BDMA_CopyHnd_internal(phy64SrcAddr, phy64DstAddr, u32Len, eCpyType, BDMA_OPCFG_DEF, 1);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_BDMA_CH1_MemCopy_MIU1toMIU1
/// @brief \b Function  \b Description: Copy dram to dram 
/// @param phy64SrcAddr   \b IN: Physical Source address
/// @param phy64DstAddr   \b IN: Physical Dst address
/// @param u32Len       \b IN: data length
/// @return             \b BDMA_Result
////////////////////////////////////////////////////////////////////////////////
BDMA_Result MDrv_BDMA_CH1_MemCopy_MIU1toMIU1(long phy64SrcAddr, long phy64DstAddr, U32 u32Len)
{
    U8 eSrc, eDst;
    BDMA_CpyType eCpyType;

    eSrc = E_BDMA_DEV_MIU1;
    eDst = E_BDMA_DEV_MIU1;
    eCpyType = (BDMA_CpyType)(eSrc | _LShift(eDst, 8));

    return MDrv_BDMA_CopyHnd_internal(phy64SrcAddr, phy64DstAddr, u32Len, eCpyType, BDMA_OPCFG_DEF, 1);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: BDMA_WaitDone
/// @brief \b Function  \b Description: wait done for bdma action
/// @param <IN>         \b eAct: action
/// @param <IN>         \b u8Ch: channel
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Success FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
static BDMA_Result BDMA_WaitDone(BDMA_Act eAct, U8 u8Ch)
{
    U32 u32Delay = 0;

    u32Delay = BDMA_WAIT_TIME;
    do
    {
        if (HAL_BDMA_Is_Done(eAct, u8Ch))
        {

            return E_BDMA_OK;
        }
        u32Delay--;
    }while(!BDMA_IS_TIMEOUT(u32Delay));

    printk("BDMA ch%u wait act:%u done timeout!\n ",u8Ch, eAct);

    return E_BDMA_TIMEOUT;
}

/// @brief \b Function  \b Name: BDMA_CmnHnd
/// @brief \b Function  \b Description: Handle for common actions
/// @param <IN>         \b sOpCB : control block
/// @param <OUT>        \b u8Ch : action channel
/// @param <RET>        \b TRUE : Success FALSE: Fail
/// @param <GLOBAL>     \b sOpCB : global control block
////////////////////////////////////////////////////////////////////////////////
static BDMA_Result BDMA_CmnHnd(BDMA_OP_CB sOpCB, U8 *pu8Ch)
{
    if(bdma_mutex == false)
    {
        bdma_mutex = true;
        mutex_init(&mutex);
    }

    mutex_lock(&mutex);
    BDMA_Result eRet;

    eRet = E_BDMA_FAIL;
    //clear status first
    HAL_BDMA_Clear_Status(sOpCB.eAct, *pu8Ch);

    //Set start address
    HAL_BDMA_SetSrcAddr(sOpCB.eAct, sOpCB.phy64SrcAddr, *pu8Ch);
    HAL_BDMA_SetDstAddr(sOpCB.eAct, sOpCB.phy64DstAddr, *pu8Ch);
    //Set size
    HAL_BDMA_SetLen(sOpCB.eAct, sOpCB.u32DataSize, *pu8Ch);

    HAL_BDMA_SetSrcDev(sOpCB.u8SrcDevCfg, *pu8Ch);
    HAL_BDMA_SetDstDev(sOpCB.u8DstDevCfg, *pu8Ch);
    HAL_BDMA_SetCmd0(sOpCB.u32Cmd0 ,*pu8Ch);
    HAL_BDMA_SetCmd1(sOpCB.u32Cmd1 ,*pu8Ch);
    HAL_BDMA_SetCmd2(sOpCB.u32Cmd2 ,*pu8Ch);
    // Set INT
    HAL_BDMA_Enable_INT(((sOpCB.pCbf)?TRUE:FALSE), *pu8Ch);
    // Set address direction
    HAL_BDMA_Set_Addr_Dec((sOpCB.u8OpCfg & BDMA_OPCFG_INV_COPY), *pu8Ch);
    // Set crc reflection
    HAL_BDMA_Set_CRC_Reflect((sOpCB.u8OpCfg & BDMA_OPCFG_CRC_REFLECT), *pu8Ch);

    HAL_BDMA_SetDmyWrCnt(sOpCB.u8DmyWrCnt, *pu8Ch);

    //while(1);

    if(BDMA_Start(sOpCB.u8SrcDevCfg, sOpCB.u8DstDevCfg, *pu8Ch))
	{
		eRet = BDMA_WaitDone(sOpCB.eAct, *pu8Ch);
	}
    mutex_unlock(&mutex);
    return eRet;
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_BDMA_CopyHnd
/// @brief \b Function  \b Description: Handle for BDMA copy data from src to dst
/// @param phy64SrcAddr   \b IN: Physical source address
/// @param phy64DstAddr   \b IN: Physical dst address
/// @param u32Len       \b IN: data length
/// @param eCpyType     \b IN: BDMA copy type
/// @param u8OpCfg      \b IN: u8OpCfg: default is BDMA_OPCFG_DEF
///                         - Bit0: inverse mode --> BDMA_OPCFG_INV_COPY
///                         - Bit2: Copy & CRC check in wait mode --> BDMA_OPCFG_CRC_COPY
///                         - Bit3: Copy without waiting --> BDMA_OPCFG_NOWAIT_COPY
/// @return             \b BDMA_Result
////////////////////////////////////////////////////////////////////////////////
BDMA_Result MDrv_BDMA_CopyHnd_internal(long phy64SrcAddr, long phy64DstAddr, U32 u32Len, BDMA_CpyType eCpyType, U8 u8OpCfg, U8 Channel)
{
    U8 u8MiuSel;
    U8 u8Ch = Channel;
    U32 u32CRCVal = 0xFFFFFFFF;
    U32 u32Offset;
    BDMA_OP_CB sOpCB;
    BDMA_Dev eSrcDev, eDstDev;
    BDMA_Result eRet = E_BDMA_FAIL;

    eSrcDev = (BDMA_Dev)(eCpyType & 0x0F);
    eDstDev = (BDMA_Dev)_RShift(eCpyType, 8);
    if (eSrcDev == eDstDev && phy64SrcAddr == phy64DstAddr)
    {
        return E_BDMA_OK;
    }

    if ((!u32Len) || (E_BDMA_CPYTYPE_MAX <= eCpyType))
    {
        return eRet;
    }

    sOpCB.phy64SrcAddr = phy64SrcAddr;
    sOpCB.phy64DstAddr = phy64DstAddr;
    sOpCB.u32DataSize = u32Len;

    //avoid address overlapping
    if (eSrcDev == eDstDev)
    {
        if ((phy64SrcAddr < phy64DstAddr) && ((phy64SrcAddr+u32Len-1) >= phy64DstAddr))
        {
            u8OpCfg |= BDMA_OPCFG_INV_COPY;
        }
    }

    if (u8OpCfg & BDMA_OPCFG_INV_COPY)
    {
        sOpCB.phy64SrcAddr += (sOpCB.u32DataSize-1);
        sOpCB.phy64DstAddr += (sOpCB.u32DataSize-1);
    }

    sOpCB.u8SrcDevCfg = _BDMA_GetDevCfg(eSrcDev);
    sOpCB.u8DstDevCfg = _BDMA_GetDevCfg(eDstDev);

    if ((E_BDMA_DEV_MIU0 == eDstDev) || (E_BDMA_DEV_MIU1 == eDstDev) || (E_BDMA_DEV_MIU2 == eDstDev) || (E_BDMA_DEV_MIU3 == eDstDev))
    {
        sOpCB.u8DmyWrCnt = HAL_BDMA_DMY_WRCNT;
    }
    else
    {
        sOpCB.u8DmyWrCnt = 0;
    }

    sOpCB.u32Cmd0 = sOpCB.phy64DstAddr;
    sOpCB.u32Cmd1 = 0;
    sOpCB.u32Cmd2 = 0;

    sOpCB.u8OpCfg = u8OpCfg;
    sOpCB.eAct = (E_BDMA_DEV_FLASH == eSrcDev) ? E_BDMA_ACT_COPY_FLASH : E_BDMA_ACT_COPY_MEM;
    //TBD
    sOpCB.pCbf = (void *)0;

    BDMA_CmnHnd(sOpCB, &u8Ch);

    return eRet;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: BDMA_Start
/// @brief \b Function  \b Description: Start BDMA action for channel 0/1
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Success FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
static BDMA_Result BDMA_Start(U8 u8SrcDev, U8 u8DstDev, U8 u8Ch)
{

    if (HAL_BDMA_Is_Queued(u8Ch) || HAL_BDMA_Is_Busy(u8Ch))
    {
        return E_BDMA_FAIL;
    }
    HAL_BDMA_TrigOn(u8SrcDev, u8DstDev, u8Ch);
    return E_BDMA_OK;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: _BDMA_GetDevCfg
/// @brief \b Function  \b Description: Get device cfg for BDMA copy
/// @param <IN>         \b eDev: device
/// @param <OUT>        \b None :
/// @param <RET>        \b src & dst device configuration for bdma copy
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
static U8 _BDMA_GetDevCfg(BDMA_Dev eDev)
{
    return (U8)(HAL_BDMA_GetDevId(eDev) | HAL_BDMA_GetDevDw(eDev));
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_BDMA_PatternFill
/// @brief \b Function  \b Description: Fill pattern to filled dst device
/// @param u32Addr      \b IN: Physical Start address
/// @param u32Len       \b IN: Fill Pattern length
/// @param u32Pattern   \b IN: Fill Pattern data
/// @param eDev         \b IN: filled dst device
/// @return             \b BDMA_Result
////////////////////////////////////////////////////////////////////////////////
BDMA_Result MDrv_BDMA_CH0_PatternFill(long phy64Addr, U32 u32Len, U32 u32Pattern, BDMA_DstDev eDev)
{
    U8 u8Ch = 0;
    BDMA_OP_CB sOpCB;

    sOpCB.eAct = E_BDMA_ACT_MEM_FILL;
    sOpCB.phy64SrcAddr = 0;
    sOpCB.phy64DstAddr = phy64Addr;
    sOpCB.u32DataSize = u32Len;

    sOpCB.u8SrcDevCfg = _BDMA_GetDevCfg(E_BDMA_DEV_MEM_FILL);
    sOpCB.u8DstDevCfg = _BDMA_GetDevCfg((BDMA_Dev)eDev);
    sOpCB.u32Cmd0 = u32Pattern;
    sOpCB.u32Cmd1 = 0;
    sOpCB.u32Cmd2 = 0;
    sOpCB.u8OpCfg = BDMA_OPCFG_DEF;
    sOpCB.u8DmyWrCnt = HAL_BDMA_DMY_WRCNT;
    sOpCB.pCbf = (void *)0;

    return BDMA_CmnHnd(sOpCB, &u8Ch);
}



////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_BDMA_PatternFill
/// @brief \b Function  \b Description: Fill pattern to filled dst device
/// @param u32Addr      \b IN: Physical Start address
/// @param u32Len       \b IN: Fill Pattern length
/// @param u32Pattern   \b IN: Fill Pattern data
/// @param eDev         \b IN: filled dst device
/// @return             \b BDMA_Result
////////////////////////////////////////////////////////////////////////////////
BDMA_Result MDrv_BDMA_CH1_PatternFill(long phy64Addr, U32 u32Len, U32 u32Pattern, BDMA_DstDev eDev)
{
    U8 u8Ch = 1;
    BDMA_OP_CB sOpCB;

    sOpCB.eAct = E_BDMA_ACT_MEM_FILL;
    sOpCB.phy64SrcAddr = 0;
    sOpCB.phy64DstAddr = phy64Addr;
    sOpCB.u32DataSize = u32Len;

    sOpCB.u8SrcDevCfg = _BDMA_GetDevCfg(E_BDMA_DEV_MEM_FILL);
    sOpCB.u8DstDevCfg = _BDMA_GetDevCfg((BDMA_Dev)eDev);
    sOpCB.u32Cmd0 = u32Pattern;
    sOpCB.u32Cmd1 = 0;
    sOpCB.u32Cmd2 = 0;
    sOpCB.u8OpCfg = BDMA_OPCFG_DEF;
    sOpCB.u8DmyWrCnt = HAL_BDMA_DMY_WRCNT;
    sOpCB.pCbf = (void *)0;

    return BDMA_CmnHnd(sOpCB, &u8Ch);
}


