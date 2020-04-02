//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2012 MStar Semiconductor, Inc.
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
//==============================================================================
#ifndef  _MDRV_XC_MENULOAD_C_
#define  _MDRV_XC_MENULOAD_C_
#define MDRV_XC_MENULOAD

// Common Definition
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/slab.h>

#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/wait.h>
#include <linux/types.h>
#include <linux/semaphore.h>
#include <linux/hrtimer.h>

#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#if defined(CONFIG_MIPS)
#elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
#include <asm/io.h>
#endif
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/proc_fs.h>


#include "mdrv_mstypes.h"
#include "mdrv_xc_st.h"
#include "mdrv_xc_menuload.h"
#include "mhal_xc.h"
#ifdef SUPPORT_KERNEL_MLOAD
#include "mhal_xc_chip_config.h"
#include "mhal_menuload.h"
#endif
#include "mstar/mstar_chip.h"

#define  MLDBG_WARN(x)   x // open when debug time for serious issue.
#define  MLDBG(x) //(printk("[MLOAD %d] ",__LINE__), x)
#define  MLG(x) //(printk("[MLG] "), x)

#define _AlignTo(value, align)  ( ((value) + ((align)-1)) & ~((align)-1) )
#define UNUSED(x) (void )(x)

static DEFINE_MUTEX(mload_main_xc_mutex);
static DEFINE_MUTEX(mload_main_hdr_mutex);
static DEFINE_MUTEX(mload_sub_xc_mutex);
static DEFINE_MUTEX(mload_sub_hdr_mutex);

static DEFINE_MUTEX(mload_mutex_fire);
static DEFINE_MUTEX(mload_sub_mutex_fire);

typedef enum
{
  E_CHIP_MIU_0 = 0,
  E_CHIP_MIU_1,
  E_CHIP_MIU_2,
  E_CHIP_MIU_3,
  E_CHIP_MIU_NUM,
} CHIP_MIU_ID;

#define _phy_to_miu_offset(MiuSel, Offset, PhysAddr) if (PhysAddr < ARM_MIU1_BASE_ADDR) \
                                                        {MiuSel = E_CHIP_MIU_0; Offset = PhysAddr;} \
                                                     else if ((PhysAddr >= ARM_MIU1_BASE_ADDR) && (PhysAddr < ARM_MIU2_BASE_ADDR)) \
                                                         {MiuSel = E_CHIP_MIU_1; Offset = PhysAddr - ARM_MIU1_BASE_ADDR;} \
                                                     else \
                                                         {MiuSel = E_CHIP_MIU_2; Offset = PhysAddr - ARM_MIU2_BASE_ADDR;}
//typedef struct
//{
//    MS_U16 u16OldValue[E_K_STORE_VALUE_MAX];
//    MS_U8 u8XCMLoadMIUSel;
//    MS_PHY g_u32MLoadPhyAddr_Suspend;
//    MS_U32 g_u32MLoadBufByteLen_Suspend;
//}MLOAD_PRIVATE;

#ifdef SUPPORT_KERNEL_MLOAD
MLOAD_PRIVATE _mload_private;
#endif

KDRV_MS_MLoad_Info _client[E_CLIENT_MAX];
static MS_BOOL _abCleard[E_CLIENT_MAX] = {FALSE};
static MS_BOOL _abInit[E_CLIENT_MAX] = {FALSE};
static MS_VIRT _abInitAddr[E_CLIENT_MAX] = {0};
MS_BOOL  INITED[2] = {FALSE,FALSE};
extern MS_U32 _XC_Device_Offset[2];
//===================================================================================================
MS_BOOL KDrv_XC_MLoad_Check_Done(EN_MLOAD_CLIENT_TYPE _client_type);
void KDrv_XC_MLoad_Wait_HW_Done(EN_MLOAD_CLIENT_TYPE _client_type);
void  KDrv_XC_MLoad_Init(EN_MLOAD_CLIENT_TYPE _client_type, MS_PHY phyAddr);
void KDrv_XC_MLoad_Trigger(EN_MLOAD_CLIENT_TYPE _client_type,MS_PHY startAddr, MS_U16 u16CmdCnt);
void KDrv_XC_MLoad_AddCmd(EN_MLOAD_CLIENT_TYPE _client_type, MS_U32 u32Cmd);
MS_U32 KDrv_XC_MLoad_GetCmd(MS_PHY DstAddr); //NO_NEED
MS_U64 KDrv_XC_MLoad_GetCmd_64Bits(MS_PHY DstAddr);
MS_BOOL KDrv_XC_MLoad_BufferEmpty(EN_MLOAD_CLIENT_TYPE _client_type);
MS_BOOL KDrv_XC_MLoad_KickOff(EN_MLOAD_CLIENT_TYPE _client_type);
MS_BOOL KDrv_XC_MLoad_GetCaps(void);
void KDrv_XC_MLoad_AddNull(EN_MLOAD_CLIENT_TYPE _client_type);
void KDrv_XC_MLoad_Add_32Bits_Cmd(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Cmd, MS_U16 u16Mask);
void KDrv_XC_MLoad_Add_64Bits_Cmd(EN_MLOAD_CLIENT_TYPE _client_type,MS_U64 u64Cmd, MS_U16 u16Mask);
MS_BOOL KDrv_XC_MLoad_WriteCommand(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);
MS_BOOL KDrv_XC_MLoad_WriteCommand_NonXC(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Bank,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);
MS_BOOL KDrv_XC_MLoad_Fire(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bImmediate);
EN_KDRV_MLOAD_TYPE KDrv_XC_MLoad_GetStatus(EN_MLOAD_CLIENT_TYPE _client_type);
void KDrv_XC_MLoad_Enable(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bEnable);
MS_BOOL KDrv_XC_MLoad_WriteCmd(EN_MLOAD_CLIENT_TYPE _client_type, MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);
//===================================================================================================
void _MLOAD_ENTRY(EN_MLOAD_CLIENT_TYPE client)
{
    if(client == E_CLIENT_MAIN_XC)
        mutex_lock(&mload_main_xc_mutex);
    else if(client == E_CLIENT_MAIN_HDR)
        mutex_lock(&mload_main_hdr_mutex);
    else if(client == E_CLIENT_SUB_XC)
        mutex_lock(&mload_sub_xc_mutex);
    else if(client == E_CLIENT_SUB_HDR)
        mutex_lock(&mload_sub_hdr_mutex);
}
void _MLOAD_RETURN(EN_MLOAD_CLIENT_TYPE client)
{
    if(client == E_CLIENT_MAIN_XC)
        mutex_unlock(&mload_main_xc_mutex);
    else if(client == E_CLIENT_MAIN_HDR)
        mutex_unlock(&mload_main_hdr_mutex);
    else if(client == E_CLIENT_SUB_XC)
        mutex_unlock(&mload_sub_xc_mutex);
    else if(client == E_CLIENT_SUB_HDR)
        mutex_unlock(&mload_sub_hdr_mutex);
}
void _MLOAD_FIRE_ENTRY(EN_MLOAD_CLIENT_TYPE client)
{
    if(client <= E_CLIENT_MAIN_HDR)
        mutex_lock(&mload_mutex_fire);
    else if((client > E_CLIENT_MAIN_HDR)&&(client <= E_CLIENT_SUB_HDR))
        mutex_lock(&mload_sub_mutex_fire);
}
void _MLOAD_FIRE_RETURN(EN_MLOAD_CLIENT_TYPE client)
{
    if(client <= E_CLIENT_MAIN_HDR)
        mutex_unlock(&mload_mutex_fire);
    else if((client > E_CLIENT_MAIN_HDR)&&(client <= E_CLIENT_SUB_HDR))
        mutex_unlock(&mload_sub_mutex_fire);
}
//======PA 2 VA MAPPING=======
MS_VIRT _PA2VA(EN_MLOAD_CLIENT_TYPE _client_type,phys_addr_t phy,phys_addr_t len)
{
#ifdef SUPPORT_KERNEL_MLOAD
    phys_addr_t tmp_phy;

    if(phy<ARM_MIU1_BASE_ADDR)
    {
        tmp_phy = phy + ARM_MIU0_BUS_BASE ;
    }else if((phy >= ARM_MIU1_BASE_ADDR) && (phy<ARM_MIU2_BASE_ADDR))
    {
        tmp_phy = phy - ARM_MIU1_BASE_ADDR + ARM_MIU1_BUS_BASE ;
    }else
    {
        tmp_phy = phy - ARM_MIU2_BASE_ADDR + ARM_MIU2_BUS_BASE ;
    }
    _client[_client_type].len = len;
    if (pfn_valid(__phys_to_pfn(tmp_phy)))
    {
        _client[_client_type].va_phy = __va(tmp_phy);
    }
    else
    {
        _client[_client_type].va_phy =  ioremap(tmp_phy, len);
    }
    MLDBG(printk("%s: client = %d,va = 0x%lx\n", __FUNCTION__, _client_type, _client[_client_type].va_phy));
    return (MS_VIRT)_client[_client_type].va_phy;
#else
    return 0;
#endif
}
//======Get PA 2 VA======
MS_VIRT _GET_PA2VA(MS_PHYADDR DstAddr)
{
    MS_U32 i=0;
    MS_PHYADDR tmp_diff=0;
    for(i<0;i<E_CLIENT_MAX;i++)
    {
        if((DstAddr >= _client[i].PhyAddr) && (DstAddr< (_client[i].PhyAddr + _client[i].len)))
        {
            tmp_diff = DstAddr - _client[i].PhyAddr;
            return (_client[i].va_phy + tmp_diff);
        }
    }
    MLDBG_WARN(printk("%s: No PA 2 VA MAPPING!\n", __FUNCTION__));
    return 0;
}

void kdrv_dumpcmd(EN_MLOAD_CLIENT_TYPE _client_type)
{
#ifdef SUPPORT_KERNEL_MLOAD
    MS_U16 i, j;
    MS_U64 *pu64Addr = NULL;
    MS_PHYADDR DstAddr;
    MS_U32 u32WriteMemShift = 0;
    MS_U8 *p = (MS_U8 *)_GET_PA2VA(_client[_client_type].PhyAddr);

    printk("dump cmd buffer: 0x%x, WPoint %d\n", _client[_client_type].PhyAddr,_client[_client_type].u16WPoint);
    for (i = 0; i < _client[_client_type].u16WPoint; i++)
    {
        printk("%04d: ", i);
        for (j = 0; j < MS_MLOAD_CMD_LEN; j++)
        {
            printk("%02x", p[i*MS_MLOAD_CMD_LEN + j]);
            if((j + 1) % (MS_MLOAD_CMD_LEN_64BITS) == 0)
            {
                printk(" ");
            }
        }
        printk(" \n");
    }
#endif
}

void _DelayTask (MS_U32 u32Ms)
{
    struct timespec req, rem;
    req.tv_sec = 0;
    req.tv_nsec = (long) (u32Ms*1000000UL);
    hrtimer_nanosleep(&req, &rem, HRTIMER_MODE_REL, CLOCK_MONOTONIC);
}

void _DelayTaskUs (MS_U32 u32Us)
{
    struct timespec TS1, TS2;
    getnstimeofday(&TS1);
    getnstimeofday(&TS2);

    while((TS2.tv_nsec - TS1.tv_nsec)< (u32Us * 1000UL))
    {
        getnstimeofday(&TS2);
    }
}

static MS_U32 _MDrv_XC_GetMLoadMemOffset(MS_U32 u32Index)
{

    MS_U32 u32Offset = 0;
#ifdef SUPPORT_KERNEL_MLOAD
    MS_U32 u32CmdCntPerMIUBus = (MS_MLOAD_BUS_WIDTH / MS_MLOAD_CMD_LEN_64BITS);

    u32Offset = (u32Index / u32CmdCntPerMIUBus) * MS_MLOAD_MEM_BASE_UNIT + (u32Index % u32CmdCntPerMIUBus) * MS_MLOAD_CMD_LEN_64BITS;
#endif

    return u32Offset;
}

MS_BOOL KDrv_XC_MLoad_Check_Done(EN_MLOAD_CLIENT_TYPE _client_type)
{
    MS_BOOL bEn = FALSE;

#ifdef SUPPORT_KERNEL_MLOAD
    if(KHal_XC_MLoad_get_status(_client_type))
        bEn = FALSE;
    else
        bEn = TRUE;
#endif

    return bEn;
}

void KDrv_XC_MLoad_Wait_HW_Done(EN_MLOAD_CLIENT_TYPE _client_type)
{
#ifdef SUPPORT_KERNEL_MLOAD

    MS_U32 u32Delayms = 0;
    while((KDrv_XC_MLoad_Check_Done(_client_type) == FALSE) && u32Delayms < 600)
    {
        _DelayTask(1);
        u32Delayms++;
    }
#endif
}
void KDrv_XC_MLoad_Init(EN_MLOAD_CLIENT_TYPE _client_type, MS_PHY phyAddr)
{
#ifdef SUPPORT_KERNEL_MLOAD
    MS_PHY u32StartOffset;
    MS_U8 u8MIUSel = 0;
    MS_U16 u16Cmd;
    //MS_U32 i = 0;

    MLDBG(printk("%s: 0x%lx\n", __FUNCTION__, phyAddr));
    // remove the MIU offset
    if(1)//(!INITED[_client_type/CLIENT_NUM])    //review later for better solution or is it needed?
    {
        _phy_to_miu_offset(u8MIUSel, u32StartOffset, phyAddr);
        UNUSED(u32StartOffset);
        KHal_XC_MLoad_set_opm_arbiter_bypass(_client_type,TRUE);

        //set command format
        KHal_XC_MLoad_Command_Format_initial(_client_type);

        //Hal_XC_MLoad_set_on_off(DISABLE);
        KHal_XC_MLoad_set_trigger_timing(_client_type,TRIG_SRC_DELAY_LINE); //By REG_MLOAD_TRIG_DLY
        KHal_XC_MLoad_set_trigger_delay(_client_type,0x05);
        KHal_XC_MLoad_set_trig_p(_client_type,0x08, 0x0A);
        KHal_XC_MLoad_get_trig_p(_client_type,&_mload_private.u16OldValue[E_K_STORE_VALUE_AUTO_TUNE_AREA_TRIG], &_mload_private.u16OldValue[E_K_STORE_VALUE_DISP_AREA_TRIG]);
    #if (HW_DESIGN_4K2K_VER == 4)
        // set ip trig as hw default value
        KDrv_XC_MLoad_set_IP_trig_p(_client_type,0x03, 0x05);
        KDrv_XC_MLoad_get_IP_trig_p(_client_type,&_mload_private.u16OldValue[E_K_STORE_VALUE_IP_AUTO_TUNE_AREA_TRIG], &_mload_private.u16OldValue[E_K_STORE_VALUE_IP_DISP_AREA_TRIG]);
    #endif
        //Hal_XC_MLoad_set_miu_bus_sel(pInstance, MS_MLOAD_MIU_BUS_SEL);
        KHal_XC_MLoad_set_miusel(_client_type,u8MIUSel);
        _mload_private.u8XCMLoadMIUSel = u8MIUSel;

        //***********watch dog***************
        /*
        **enable watch dog
        */
        KHal_XC_MLoad_enable_watch_dog(_client_type,TRUE);
        /*
        ** enlarge watch dog response time
        */
        // Dolby HDR need to use DS, and DS index depth is changed to max
        // Then some movies will show garbage when start play
        // DS need more time to set more DS cmds
        // Joeanna's experiement suggestion:
        // BK1301_09[3:0] = 0x0C (Y_Lock)
        // BK131F_10[7:4] = 0xE (DMA request)
        // BK131F_05[9:0] = 0x3FF (reg_ds_dma_wd_timer)
        KHal_XC_MLoad_set_watch_dog_time_delay(_client_type,0x03FF);
        /*
        ** watch dog timer reset type 00: dma only, 01: miu, 11: all
        */
        KHal_XC_MLoad_enable_watch_dog_reset(_client_type,MLoad_WD_Timer_Reset_ALL);
        //***********************************

        KHal_XC_MLoad_set_BitMask(_client_type,TRUE);
        //INITED[_client_type/CLIENT_NUM] = TRUE;
    }
    if (KDrv_XC_MLoad_GetCaps())
    {
        for (u16Cmd = 0; u16Cmd < 32; u16Cmd++)
        {
            if(IS_SUPPORT_64BITS_COMMAND(ENABLE_64BITS_COMMAND, _client_type/CLIENT_NUM))
            {
                if(ENABLE_64BITS_SPREAD_MODE)
                {
                    KDrv_XC_MLoad_Add_64Bits_Cmd(_client_type,(MS_U64)MS_MLOAD_NULL_CMD_SPREAD_MODE, 0xFFFF);
                }
                else
                {
                    KDrv_XC_MLoad_Add_64Bits_Cmd(_client_type,(MS_U64)MS_MLOAD_NULL_CMD, 0xFFFF);
                }
            }
            else
            {
                KDrv_XC_MLoad_Add_32Bits_Cmd(_client_type,(MS_U32)MS_MLOAD_NULL_CMD, 0xFFFF);
            }
        }
        KDrv_XC_MLoad_AddNull(_client_type);
        _client[_client_type].u16FPoint = _client[_client_type].u16WPoint - 1;

        // when system is power on, the read count of MLoad may be not 0.
        // If we enable MLoad RIU_En, the garbage data in fifo will over-write
        // unexpected register.
        // Hence, add below code before Kickoff to avoid it.

        KHal_XC_MLoad_set_riu_cs(TRUE);
        _DelayTask(1);

        KHal_XC_MLoad_Set_riu(_client_type,1);

        MDrv_WriteByte(0x102F00, 0xFF);

        KHal_XC_MLoad_set_riu_cs(FALSE);

        _DelayTask(1);

        KDrv_XC_MLoad_KickOff(_client_type);
        _abCleard[_client_type] = FALSE;
    }
#endif
}

MS_BOOL KDrv_XC_MLoad_GetCaps(void)
{
#ifdef SUPPORT_KERNEL_MLOAD
    return KHal_XC_MLoad_GetCaps();
#else
    return FALSE;
#endif
}

void KDrv_XC_MLoad_Trigger(EN_MLOAD_CLIENT_TYPE _client_type,MS_PHY startAddr, MS_U16 u16CmdCnt)
{
#ifdef SUPPORT_KERNEL_MLOAD
    MS_PHY u32StartOffset = 0;
    MS_U8 u8MIUSel = 0;
    MLDBG(printk("%s 0x%lx, %u\n\n",__FUNCTION__, startAddr, u16CmdCnt));

    //dumpcmd();
    #if 1
    _phy_to_miu_offset(u8MIUSel, u32StartOffset, startAddr);
    UNUSED(u8MIUSel);
    KHal_XC_MLoad_set_base_addr(_client_type,u32StartOffset);
    KHal_XC_MLoad_set_depth(_client_type,u16CmdCnt);
    KHal_XC_MLoad_set_len(_client_type,MS_MLOAD_REG_LEN);//length of DMA request
    KHal_XC_MLoad_set_on_off(_client_type,ENABLE);
    _DelayTaskUs(3);
    KHal_XC_MLoad_Set_riu(_client_type,ENABLE);

    #else
    KHal_XC_MLoad_trigger(startAddr, MS_MLOAD_REG_LEN, (MS_U8)u16CmdCnt);
    #endif
#endif
}

void KDrv_XC_MLoad_AddCmd(EN_MLOAD_CLIENT_TYPE _client_type, MS_U32 u32Cmd)
{
#ifdef SUPPORT_KERNEL_MLOAD

    MS_U32 *pu32Addr = NULL;
    MS_PHYADDR DstAddr;

    DstAddr = _client[_client_type].PhyAddr + _client[_client_type].u16WPoint * MS_MLOAD_CMD_LEN;
    pu32Addr = (MS_U32 *)_GET_PA2VA(DstAddr);
    *pu32Addr = u32Cmd;
    _client[_client_type].u16WPoint++;
#endif
}

void KDrv_XC_MLoad_SetCmd(MS_PHY DstAddr, MS_U32 u32Cmd)
{
#ifdef SUPPORT_KERNEL_MLOAD

    MS_U32 *pu32Addr = NULL;

    pu32Addr = (MS_U32 *)_GET_PA2VA(DstAddr);
    *pu32Addr = u32Cmd;
#endif
}

void KDrv_XC_MLoad_SetCmd_64Bits(MS_PHY DstAddr, MS_U64 u64Cmd)
{
#ifdef SUPPORT_KERNEL_MLOAD

    MS_U64 *pu64Addr = NULL;

    pu64Addr = (MS_U64 *)_GET_PA2VA(DstAddr);
    *pu64Addr = u64Cmd;
#endif
}

MS_U32 KDrv_XC_MLoad_GetCmd(MS_PHY DstAddr)
{
#ifdef SUPPORT_KERNEL_MLOAD

    MS_VIRT u32Addr;

    u32Addr = _GET_PA2VA(DstAddr);
    return (MS_U32)(*(MS_U32 *)u32Addr);
#else
    return 0;
#endif
}

MS_U64 KDrv_XC_MLoad_GetCmd_64Bits(MS_PHY DstAddr)
{
#ifdef SUPPORT_KERNEL_MLOAD

    MS_VIRT u32Addr;

    u32Addr = _GET_PA2VA(DstAddr);
    return (MS_U64)(*(MS_U64 *)u32Addr);
#else
    return 0;
#endif
}

MS_BOOL KDrv_XC_MLoad_BufferEmpty(EN_MLOAD_CLIENT_TYPE _client_type)
{
#ifdef SUPPORT_KERNEL_MLOAD
    if(KDrv_XC_MLoad_Check_Done(_client_type) &&
       (_client[_client_type].u16WPoint == _client[_client_type].u16RPoint) &&
       (_client[_client_type].u16WPoint == _client[_client_type].u16FPoint+1))
        return TRUE;
    else
        return FALSE;
#else
    return FALSE;
#endif
}

//-------------------------------------------------------------------------------------------------
/// Write command to the Menuload buffer
/// @param  u32Cmd                 \b IN: the command to write into the buffer
/// @return  TRUE if succeed, FALSE if failed
//-------------------------------------------------------------------------------------------------
static MS_BOOL _KDrv_XC_MLoad_WriteCmd_32Bits(EN_MLOAD_CLIENT_TYPE _client_type, MS_U32 u32Cmd, MS_U16 u16Mask)
{
    MS_BOOL bRet = FALSE;
#ifdef SUPPORT_KERNEL_MLOAD
    MS_U16 u16DummyCmdIdx;
    MS_U16 u16QueueCmd;
#ifdef ENABLE_SCALING_WO_MUTE
    if ((_client[_client_type].u16WPoint == _client[_client_type].u16RPoint) &&
        (_client[_client_type].u16WPoint == _client[_client_type].u16FPoint+1))
    {
        KDrv_XC_MLoad_Wait_HW_Done(_client_type);
    }
#endif

    if( KDrv_XC_MLoad_BufferEmpty(_client_type) )
    {
        _client[_client_type].u16RPoint = 0;
        _client[_client_type].u16WPoint = 0;
        _client[_client_type].u16FPoint = 0;
        KDrv_XC_MLoad_Add_32Bits_Cmd(_client_type, u32Cmd, u16Mask);
        bRet = TRUE;
    }
    else
    {
        u16DummyCmdIdx = _AlignTo(_client[_client_type].u16WPoint+1 + 16 , 4);

        if (u16DummyCmdIdx == _client[_client_type].u16WPoint+1)
        {
            u16DummyCmdIdx = _client[_client_type].u16WPoint+1+MS_MLOAD_CMD_ALIGN;
        }

        if(u16DummyCmdIdx < _client[_client_type].u16MaxCmdCnt)
        {
            u16QueueCmd = _client[_client_type].u16WPoint - _client[_client_type].u16RPoint + 1;

            if(u16QueueCmd == MS_MLOAD_MAX_CMD_CNT(_client_type) - 1) //max cmd depth is MS_MLOAD_MAX_CMD_CNT
            {
                KDrv_XC_MLoad_Add_32Bits_Cmd(_client_type, MS_MLOAD_NULL_CMD, 0xFFFF);
            }
            KDrv_XC_MLoad_Add_32Bits_Cmd(_client_type, u32Cmd, u16Mask);

            bRet = TRUE;
        }
        else
        {
            MLDBG(printk("WPoint=%x, MaxCnt=%x, DummyIdx=%x\n"
                        , _client[_client_type].u16WPoint
                        , _client[_client_type].u16MaxCmdCnt
                        , u16DummyCmdIdx));
            bRet = FALSE;
        }
    }
#endif

    return bRet;
}

//-------------------------------------------------------------------------------------------------
/// Write command to the Menuload buffer
/// @param  u64Cmd                 \b IN: the command to write into the buffer
/// @return  TRUE if succeed, FALSE if failed
//-------------------------------------------------------------------------------------------------
static MS_BOOL _KDrv_XC_MLoad_WriteCmd_64Bits(EN_MLOAD_CLIENT_TYPE _client_type, MS_U64 u64Cmd, MS_U16 u16Mask)
{
    MS_BOOL bRet = FALSE;
#ifdef SUPPORT_KERNEL_MLOAD
    MS_U16 u16DummyCmdIdx;
    MS_U16 u16QueueCmd;
#ifdef ENABLE_SCALING_WO_MUTE
    if ((_client[_client_type].u16WPoint == pXCResourcePrivate->_client[_client_type].u16RPoint) &&
        (_client[_client_type].u16WPoint == pXCResourcePrivate->_client[_client_type].u16FPoint+1))
    {
        KDrv_XC_MLoad_Wait_HW_Done(_client_type);
    }
#endif

    if( KDrv_XC_MLoad_BufferEmpty(_client_type) )
    {
        _client[_client_type].u16RPoint = 0;
        _client[_client_type].u16WPoint = 0;
        _client[_client_type].u16FPoint = 0;
        KDrv_XC_MLoad_Add_64Bits_Cmd(_client_type, u64Cmd, u16Mask);

        bRet = TRUE;
    }
    else
    {
        u16DummyCmdIdx = _AlignTo(_client[_client_type].u16WPoint+1 + 16 , 4);

        if (u16DummyCmdIdx == _client[_client_type].u16WPoint+1)
        {
            u16DummyCmdIdx = _client[_client_type].u16WPoint+1+MS_MLOAD_CMD_ALIGN;
        }

        if(u16DummyCmdIdx < _client[_client_type].u16MaxCmdCnt)
        {
            u16QueueCmd = _client[_client_type].u16WPoint - _client[_client_type].u16RPoint + 1;

            if(u16QueueCmd == MS_MLOAD_MAX_CMD_CNT(_client_type) - 1) //max cmd depth is MS_MLOAD_MAX_CMD_CNT
            {
                KDrv_XC_MLoad_Add_64Bits_Cmd(_client_type, MS_MLOAD_NULL_CMD_SPREAD_MODE, 0xFFFF);
            }
            KDrv_XC_MLoad_Add_64Bits_Cmd(_client_type, u64Cmd, u16Mask);

            bRet = TRUE;
        }
        else
        {
            MLDBG(printk("WPoint=%x, MaxCnt=%x, DummyIdx=%x\n"
                        , _client[_client_type].u16WPoint
                        , _client[_client_type].u16MaxCmdCnt
                        , u16DummyCmdIdx));
            bRet = FALSE;
        }
    }
#endif

    return bRet;
}
extern void Chip_Flush_Miu_Pipe(void);
MS_BOOL KDrv_XC_MLoad_KickOff(EN_MLOAD_CLIENT_TYPE _client_type)
{
    //MS_MLOAD_END_CMD use instance, does not remove this
    MS_BOOL bRet = FALSE;
#ifdef SUPPORT_KERNEL_MLOAD
    MS_U16 u16CmdCnt;
    MS_U16 u16QueueCmd, u16FireIdx;
    MS_PHY CmdBufAddr;

    MLDBG(printk("%s _client_type %d\n",__FUNCTION__,_client_type));
    if(KDrv_XC_MLoad_Check_Done(_client_type) && _client[_client_type].u16FPoint)
    {
        if(_client[_client_type].u16FPoint > _client[_client_type].u16RPoint)
        {
            u16QueueCmd = _client[_client_type].u16WPoint - _client[_client_type].u16RPoint;

            if(u16QueueCmd > MS_MLOAD_MAX_CMD_CNT(_client_type))
            {
                MLDBG_WARN(printk("Queue Too Many !!!!!!!!!!!!!!!!!\n"));
                MLDBG_WARN(printk("WPoint=%d, FirePoint=%d, RPoint=%d\n"
                                  ,_client[_client_type].u16WPoint
                                  ,_client[_client_type].u16FPoint
                                  ,_client[_client_type].u16RPoint));
            }

            u16FireIdx = _client[_client_type].u16FPoint;
            MLDBG(printk("u16QueueCmd= %u, u16FireIdx= %u\n", u16QueueCmd, u16FireIdx));

            if (IS_SUPPORT_64BITS_COMMAND(ENABLE_64BITS_COMMAND, _client_type/CLIENT_NUM))
            {
                MS_U64 u64EndCmd;
                MS_PHY u32FireMemShift = 0;
                MS_PHY u32ReadMemShift = 0;

#if ENABLE_64BITS_SPREAD_MODE
                if((_client[_client_type].u16FPoint % 2) != 0)
                {

                    u32FireMemShift = _MDrv_XC_GetMLoadMemOffset(u16FireIdx);
                }

                CmdBufAddr = _client[_client_type].PhyAddr + u32FireMemShift;


                u64EndCmd = KDrv_XC_MLoad_GetCmd_64Bits(CmdBufAddr);
                if(u64EndCmd != MS_MLOAD_NULL_CMD_SPREAD_MODE)
                {
                    MLDBG_WARN(printk("WPoint=%d, FirePoint=%d, RPoint=%d\n"
                        , _client[_client_type].u16WPoint
                        , _client[_client_type].u16FPoint
                        , _client[_client_type].u16RPoint));
                    MLDBG_WARN(printk("KickOff: Non Null Cmd. Trigger is not executed!!\n"));
                    bRet = FALSE;
                }
                else
                {
                    u16CmdCnt = u16FireIdx - _client[_client_type].u16RPoint + 1;
                    u16CmdCnt = KHal_XC_MLoad_Get_Depth(_client_type,u16CmdCnt);
                    if((_client_type/CLIENT_NUM)== 0)
                    {
                        u64EndCmd = MS_MLOAD_END_CMD_SPREAD_MODE(((MS_U64)u16CmdCnt));
                    }
                    else
                    {
                        u64EndCmd = MS_MLOAD_END_CMD_DEV1_SPREAD_MODE(((MS_U64)u16CmdCnt));
                    }
                    KDrv_XC_MLoad_SetCmd_64Bits(CmdBufAddr, u64EndCmd);

                    Chip_Flush_Miu_Pipe();

                    u32ReadMemShift = _MDrv_XC_GetMLoadMemOffset(_client[_client_type].u16RPoint);

                    CmdBufAddr = (_client[_client_type].PhyAddr + u32ReadMemShift);

                    MLDBG(printk("CmdBufAddr = 0x%lx, u16CmdCnt = 0x%lx\n", CmdBufAddr, u16CmdCnt));

                    KDrv_XC_MLoad_Trigger(_client_type,CmdBufAddr, u16CmdCnt);
                    _client[_client_type].u16RPoint = u16FireIdx + 1;
                    bRet = TRUE;
                    MLDBG(printk("after fire u16WPoint=%u u16RPoint=%u u16FPoint=%u\n",
                        _client[_client_type].u16WPoint,
                        _client[_client_type].u16RPoint,
                        _client[_client_type].u16FPoint));
                }
#else
                //check the last command to see whether it is null command
                CmdBufAddr = _client[_client_type].PhyAddr + u16FireIdx * MS_MLOAD_CMD_LEN_64BITS;
                u64EndCmd = KDrv_XC_MLoad_GetCmd_64Bits(CmdBufAddr);
                if(u64EndCmd != MS_MLOAD_NULL_CMD)
                {
                    MLDBG_WARN(printk("WPoint=%d, FirePoint=%d, RPoint=%d\n"
                        , _client[_client_type].u16WPoint
                        , _client[_client_type].u16FPoint
                        , _client[_client_type].u16RPoint));
                    MLDBG_WARN(printk("KickOff: Non Null Cmd. Trigger is not executed!!\n"));
                    bRet = FALSE;
                }
                else
                {
                    u16CmdCnt = u16FireIdx - _client[_client_type].u16RPoint + 1;
                    u16CmdCnt = KHal_XC_MLoad_Get_Depth(_client_type,u16CmdCnt);
                    u64EndCmd = (MS_U64) MS_MLOAD_END_CMD(((MS_U32)u16CmdCnt));

                    KDrv_XC_MLoad_SetCmd_64Bits(CmdBufAddr, u64EndCmd);

                    Chip_Flush_Miu_Pipe();

                    CmdBufAddr = _client[_client_type].PhyAddr + _client[_client_type].u16RPoint * MS_MLOAD_CMD_LEN_64BITS;
                    MLDBG(printk("CmdBufAddr = 0x%lx, u16CmdCnt = 0x%lx\n", CmdBufAddr, u16CmdCnt));

                    KDrv_XC_MLoad_Trigger(_client_type,CmdBufAddr, u16CmdCnt);
                    _client[_client_type].u16RPoint = u16FireIdx + 1;
                    bRet = TRUE;
                    MLDBG(printk("after fire u16WPoint=%u u16RPoint=%u u16FPoint=%u\n",
                        _client[_client_type].u16WPoint,
                        _client[_client_type].u16RPoint,
                        _client[_client_type].u16FPoint));
                }
#endif
            }
            else
            {
                MS_U32 u32EndCmd;

                //check the last command to see whether it is null command
                CmdBufAddr = _client[_client_type].PhyAddr + u16FireIdx * MS_MLOAD_CMD_LEN;
                u32EndCmd = KDrv_XC_MLoad_GetCmd(CmdBufAddr);
                if(u32EndCmd != MS_MLOAD_NULL_CMD)
                {
                    MLDBG_WARN(printk("WPoint=%d, FirePoint=%d, RPoint=%d\n"
                        , _client[_client_type].u16WPoint
                        , _client[_client_type].u16FPoint
                        , _client[_client_type].u16RPoint));
                    MLDBG_WARN(printk("KickOff: Non Null Cmd. Trigger is not executed!!\n"));
                    bRet = FALSE;
                }
                else
                {
                    u16CmdCnt = u16FireIdx - _client[_client_type].u16RPoint + 1;

                    u32EndCmd = MS_MLOAD_END_CMD(((MS_U32)u16CmdCnt));
                    MLDBG(printk("CmdBufAddr = 0x%lx, u32EndCmd = 0x%lx\n", CmdBufAddr, u32EndCmd));
                    KDrv_XC_MLoad_SetCmd(CmdBufAddr, u32EndCmd);

                    Chip_Flush_Miu_Pipe();


                    CmdBufAddr = _client[_client_type].PhyAddr + _client[_client_type].u16RPoint * MS_MLOAD_CMD_LEN;
                    MLDBG(printk("CmdBufAddr = 0x%lx, u16CmdCnt = 0x%lx\n", CmdBufAddr, u16CmdCnt));
                    KDrv_XC_MLoad_Trigger(_client_type,CmdBufAddr, u16CmdCnt);
                    _client[_client_type].u16RPoint = u16FireIdx + 1;
                    bRet = TRUE;
                    MLDBG(printk("after fire u16WPoint=%u u16RPoint=%u u16FPoint=%u\n",
                        _client[_client_type].u16WPoint,
                        _client[_client_type].u16RPoint,
                        _client[_client_type].u16FPoint));
                }
            }

        }
        else if(_client[_client_type].u16FPoint == _client[_client_type].u16RPoint &&
                _client[_client_type].u16FPoint == _client[_client_type].u16WPoint-1)
        {
            bRet = TRUE;
        }
        else
        {
            MLDBG(printk("MenuLoad: WPoint=%u, FPoint=%u, RPoint=%u !!\n",
                    _client[_client_type].u16WPoint,
                    _client[_client_type].u16FPoint,
                    _client[_client_type].u16RPoint));
            bRet = FALSE;
        }
    }
    else
    {
        bRet = TRUE;
    }
#endif

    return bRet;
}

void KDrv_XC_MLoad_AddNull(EN_MLOAD_CLIENT_TYPE _client_type)
{
#ifdef SUPPORT_KERNEL_MLOAD
    MLDBG(printk("%s\n",__FUNCTION__));
    do
    {
        if (IS_SUPPORT_64BITS_COMMAND(ENABLE_64BITS_COMMAND, _client_type/CLIENT_NUM))
        {
            if(ENABLE_64BITS_SPREAD_MODE)
            {
                KDrv_XC_MLoad_Add_64Bits_Cmd(_client_type,(MS_U64)MS_MLOAD_NULL_CMD_SPREAD_MODE, 0xFFFF);
            }
            else
            {
                KDrv_XC_MLoad_Add_64Bits_Cmd(_client_type,(MS_U64)MS_MLOAD_NULL_CMD, 0xFFFF);
            }
        }
        else
        {
            KDrv_XC_MLoad_Add_32Bits_Cmd(_client_type,(MS_U32)MS_MLOAD_NULL_CMD, 0xFFFF);
        }

    }while (_client[_client_type].u16WPoint%MS_MLOAD_CMD_ALIGN != 0);
    //kdrv_dumpcmd(_client_type);
#endif

}

//-------------------------------------------------------------------------------------------------
/// Initialize the Menuload
/// @param  PhyAddr                 \b IN: the physical address for the menuload
/// @param  u32BufByteLen           \b IN: the buffer length of the menu load commands
//-------------------------------------------------------------------------------------------------
void KApi_XC_MLoad_Init(EN_MLOAD_CLIENT_TYPE _client_type, MS_U64 PhyAddr, MS_U32 u32BufByteLen)
{
#ifdef SUPPORT_KERNEL_MLOAD
    MLDBG(printk("%s: 0x%lx, 0x%x\n", __FUNCTION__, PhyAddr, u32BufByteLen));
    if(u32BufByteLen == 0)
    {
        printk("%s: client %d, u32BufByteLen is 0, force return failed\n", __FUNCTION__,_client_type);
        return;
    }

    if(KDrv_XC_MLoad_GetCaps())
    {

        _MLOAD_ENTRY(_client_type);

        if(_abInit[_client_type]==FALSE) //prevent vmalloc leak for ioremap
        {
            _abInitAddr[_client_type]=_PA2VA(_client_type,PhyAddr,u32BufByteLen);
            _abInit[_client_type]=TRUE;
        }

        memset(_abInitAddr[_client_type], 0, u32BufByteLen);
        _client[_client_type].bEnable = FALSE;
        _client[_client_type].u16RPoint = 0;
        _client[_client_type].u16WPoint = 0;
        _client[_client_type].u16FPoint = 0;
        if (IS_SUPPORT_64BITS_COMMAND(ENABLE_64BITS_COMMAND, _client_type/CLIENT_NUM))
        {
            if ((u32BufByteLen / MS_MLOAD_CMD_LEN_64BITS) < 0x3FFFC)
            {
                _client[_client_type].u16MaxCmdCnt = (MS_U16)(u32BufByteLen / MS_MLOAD_CMD_LEN_64BITS);
            }
            else
            {
                _client[_client_type].u16MaxCmdCnt = 0xFFFF;
                MLDBG_WARN(printk("Too Large menuload u32BufByteLen. u16MaxCmdCnt might overflow!\n"));
            }
        }
        else
        {
            if ((u32BufByteLen / MS_MLOAD_CMD_LEN) < 0xFFFF)
            {
                _client[_client_type].u16MaxCmdCnt = (MS_U16)(u32BufByteLen / MS_MLOAD_CMD_LEN);
            }
            else
            {
                _client[_client_type].u16MaxCmdCnt = 0xFFFF;
                MLDBG_WARN(printk("Too Large menuload u32BufByteLen. u16MaxCmdCnt might overflow!\n"));
            }
        }
        _client[_client_type].PhyAddr = PhyAddr;
        _mload_private.g_u32MLoadPhyAddr_Suspend = PhyAddr;
        _mload_private.g_u32MLoadBufByteLen_Suspend = u32BufByteLen;
        KDrv_XC_MLoad_Init(_client_type ,PhyAddr);

        _MLOAD_RETURN(_client_type);
    }
#endif
}


//-------------------------------------------------------------------------------------------------
/// Enable/Disable the MLoad
/// @return  void
//-------------------------------------------------------------------------------------------------
void KDrv_XC_MLoad_Enable(EN_MLOAD_CLIENT_TYPE _client_type, MS_BOOL bEnable)
{
#ifdef SUPPORT_KERNEL_MLOAD
#if PIP_PATCH_USING_SC1_MAIN_AS_SC0_SUB
    if( _mload_private.g_u32MLoadPhyAddr_Suspend != 0)
#endif
    {
        _client[_client_type].bEnable = bEnable;
    }
#endif
}

void KApi_XC_MLoad_Enable(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bEnable)
{
#ifdef SUPPORT_KERNEL_MLOAD
    _MLOAD_ENTRY(_client_type);
    KDrv_XC_MLoad_Enable(_client_type,bEnable);
    _MLOAD_RETURN(_client_type);
#endif
}


//-------------------------------------------------------------------------------------------------
/// Get the status of MLoad
/// @return  MLOAD_TYPE
//-------------------------------------------------------------------------------------------------
EN_KDRV_MLOAD_TYPE KDrv_XC_MLoad_GetStatus(EN_MLOAD_CLIENT_TYPE _client_type)
{
    EN_KDRV_MLOAD_TYPE type = E_KDRV_MLOAD_UNSUPPORTED;

#ifdef SUPPORT_KERNEL_MLOAD
    if (KDrv_XC_MLoad_GetCaps())
    {
        if (_client[_client_type].bEnable)
        {
            type = E_KDRV_MLOAD_ENABLED;
        }
        else
        {
            type = E_KDRV_MLOAD_DISABLED;
        }
    }
#endif

    return type;
}

EN_KDRV_MLOAD_TYPE KApi_XC_MLoad_GetStatus(EN_MLOAD_CLIENT_TYPE _client_type)
{
    EN_KDRV_MLOAD_TYPE eMode = E_KDRV_MLOAD_UNSUPPORTED;

#ifdef SUPPORT_KERNEL_MLOAD
    eMode = KDrv_XC_MLoad_GetStatus(_client_type);
#endif
    return eMode;
}

//-------------------------------------------------------------------------------------------------
/// Fire the Menuload commands
/// @return  TRUE if succeed, FALSE if failed
//-------------------------------------------------------------------------------------------------
MS_BOOL KApi_XC_MLoad_Fire(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bImmediate)
{
    MS_BOOL bReturn = FALSE;
#ifdef SUPPORT_KERNEL_MLOAD
    _MLOAD_FIRE_ENTRY(_client_type);
    bReturn = KDrv_XC_MLoad_Fire(_client_type,bImmediate);
    _MLOAD_FIRE_RETURN(_client_type);
#endif
    return bReturn;
}

MS_BOOL KDrv_XC_MLoad_Fire(EN_MLOAD_CLIENT_TYPE _client_type, MS_BOOL bImmediate)
{
#ifdef SUPPORT_KERNEL_MLOAD
    MS_BOOL bRet = FALSE;
    MS_U32 u32Delayms = 0;
    MS_U16 u16Boundary = 16, i = 0;

    // SC1 mload may not enable
    if (E_KDRV_MLOAD_ENABLED != KDrv_XC_MLoad_GetStatus(_client_type))
    {
        return bRet;
    }

    MLDBG(printk("%s, u16WPoint=%u u16RPoint=%u u16FPoint=%u\n", __FUNCTION__,
        _client[_client_type].u16WPoint,
        _client[_client_type].u16RPoint,
        _client[_client_type].u16FPoint));

    u32Delayms = 0;
    while((KDrv_XC_MLoad_Check_Done(_client_type) == FALSE) && u32Delayms < 600)
    {
        _DelayTask(1);
        u32Delayms++;
    }

    // Patch HW outpu Vsync plus width too short and cause MLoad missing.
    // T3 U06 will fix it.
    if(KDrv_XC_MLoad_Check_Done(_client_type) == FALSE)
    {
        KHal_XC_MLoad_set_on_off(_client_type,DISABLE);
        _client[_client_type].u16RPoint = 0;
        _client[_client_type].u16WPoint = 0;
        _client[_client_type].u16FPoint = 0;
        bRet = FALSE;
        return FALSE;
    }

    if (KDrv_XC_MLoad_BufferEmpty(_client_type) || _abCleard[_client_type])
    {
        _client[_client_type].u16RPoint = 0;
        _client[_client_type].u16WPoint = 0;
        _client[_client_type].u16FPoint = 0;
        _abCleard[_client_type] = TRUE;
        MLDBG(printk("%s MLoad buffer is empty, Skip menuload fire.\n", __FUNCTION__));
        bRet = TRUE;
    }
    else
    {
        // 16 entry as boundary for menuload to prevent cmd sent unpredictable.
        for (i = 0 ; i < u16Boundary ; i++)
        {


            if (IS_SUPPORT_64BITS_COMMAND(ENABLE_64BITS_COMMAND,_client_type/CLIENT_NUM))
            {
                if(ENABLE_64BITS_SPREAD_MODE)
                {
                    KDrv_XC_MLoad_Add_64Bits_Cmd(_client_type,(MS_U64)MS_MLOAD_NULL_CMD_SPREAD_MODE, 0xFFFF);
                }
                else
                {
                    KDrv_XC_MLoad_Add_64Bits_Cmd(_client_type,(MS_U64)MS_MLOAD_NULL_CMD, 0xFFFF);
                }
            }
            else
            {
                KDrv_XC_MLoad_Add_32Bits_Cmd(_client_type,(MS_U32)MS_MLOAD_NULL_CMD, 0xFFFF);
            }

        }

        KDrv_XC_MLoad_AddNull(_client_type);

        _client[_client_type].u16FPoint = _client[_client_type].u16WPoint - 1;

        MLDBG(printk("u16WPoint=%u u16RPoint=%u u16FPoint=%u\n",
            _client[_client_type].u16WPoint,
            _client[_client_type].u16RPoint,
            _client[_client_type].u16FPoint));

        KDrv_XC_MLoad_KickOff(_client_type);

        MLDBG(printk("[River2] KDrv_XC_MLoad_KickOff End\n"));
        //kdrv_dumpcmd(_client_type);

        if(bImmediate)
        {    // we need to make sure the value is
            u32Delayms = 0;
            while((KDrv_XC_MLoad_Check_Done(_client_type) == FALSE) && u32Delayms < 600)
            {
                _DelayTask(1);
                u32Delayms++;
            }

            // Patch HW outpu Vsync plus width too short and cause MLoad missing.
            // T3 U06 will fix it.
            if(KDrv_XC_MLoad_Check_Done(_client_type) == FALSE)
            {
                MLDBG_WARN(printk("MLoad, TimeOut!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"));
                KHal_XC_MLoad_set_on_off(_client_type,DISABLE);
                _client[_client_type].u16RPoint = 0;
                _client[_client_type].u16WPoint = 0;
                _client[_client_type].u16FPoint = 0;
                bRet = FALSE;
            }
            else
            {
                _abCleard[_client_type] = FALSE;
                bRet = TRUE;
            }
        }
        else
        {
            bRet = TRUE;
        }
    }
    return bRet;
#endif
}

//-------------------------------------------------------------------------------------------------
/// Write command to the Menuload buffer by WORD
/// @param  u32Addr                 \b IN: the address (sub-bank + 8-bit address)
/// @param  u16Data                 \b IN: the data
/// @param  u16Mask                 \b IN: the mask
/// @return  TRUE if succeed, FALSE if failed
/// how to use this function
/// if you want write REG_SC_BK01_01_L bitN and bitM
/// 1. you must:
/// MDrv_XC_MLoad_WriteCmd(pInstance, REG_SC_BK01_01_L, BIT(N)|BIT(M), BIT(N)|BIT(M));
/// MApi_XC_MLoad_Fire();
/// 2. you mustn't:
/// MDrv_XC_MLoad_WriteCmd(pInstance, REG_SC_BK01_01_L, BIT(N), BIT(N));
/// MDrv_XC_MLoad_WriteCmd(pInstance, REG_SC_BK01_01_L, BIT(M), BIT(M));
/// MApi_XC_MLoad_Fire();
/// only REG_SC_BKXX_XX_L bitM will be download reg if you do like step2
//-------------------------------------------------------------------------------------------------
MS_BOOL KApi_XC_MLoad_WriteCmd(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask)
{
    MS_BOOL bReturn = FALSE;
#ifdef SUPPORT_KERNEL_MLOAD
    _MLOAD_ENTRY(_client_type);
    bReturn = KDrv_XC_MLoad_WriteCmd(_client_type,u32Addr, u16Data, u16Mask);
    _MLOAD_RETURN(_client_type);
#endif
    return bReturn;
}

//-------------------------------------------------------------------------------------------------
/// how to use this function
/// if you want write REG_SC_BKXX_XX_L bitN and bitM
/// 1. you must:
/// MDrv_XC_MLoad_WriteCmd(pInstance, REG_SC_BK01_01_L, BIT(N)|BIT(M), BIT(N)|BIT(M));
/// MApi_XC_MLoad_Fire();
/// 2. you mustn't:
/// MDrv_XC_MLoad_WriteCmd(pInstance, REG_SC_BK01_01_L, BIT(N), BIT(N));
/// MDrv_XC_MLoad_WriteCmd(pInstance, REG_SC_BK01_01_L, BIT(M), BIT(M));
/// MApi_XC_MLoad_Fire();
/// only REG_SC_BKXX_XX_L bitM will be download reg if you do like step2
//-------------------------------------------------------------------------------------------------

MS_BOOL KDrv_XC_MLoad_WriteCmd(EN_MLOAD_CLIENT_TYPE _client_type, MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask)
{
#ifdef SUPPORT_KERNEL_MLOAD
    MS_BOOL bRet;
#ifdef A3_MLG  // need to refine later, test the capibility first
    MS_U8 u8MLoadMIUSel = KHal_XC_MLoad_get_miusel();

    if (u8MLoadMIUSel != pXCResourcePrivate->stdrvXC_MVideo_Context.stdrvXC_Menuload[psXCInstPri->u32DeviceID].u8XCMLoadMIUSel)
        KHal_XC_MLoad_set_miusel(pXCResourcePrivate->stdrvXC_MVideo_Context.stdrvXC_Menuload[psXCInstPri->u32DeviceID].u8XCMLoadMIUSel);
#endif
    MS_BOOL bReturn;
    bReturn = KDrv_XC_MLoad_WriteCommand(_client_type,u32Addr, u16Data, u16Mask);
    if(bReturn == TRUE)
    {
        _abCleard[_client_type] = FALSE;
        //printk("[%s] Cmd %4lx %x %x \n",__FUNCTION__,u32Addr, u16Data, u16Mask);
        bRet = TRUE;
    }
    else
    {
        MLDBG(printk("Fail: %04lx %04x %04x \n", u32Addr, u16Data, u16Mask));
        bRet = FALSE;
    }
    return bRet;
#endif
}

MS_BOOL KApi_XC_MLoad_WriteCmd_And_Fire(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask)
{
    MS_BOOL bRet = TRUE;
#ifdef SUPPORT_KERNEL_MLOAD
    _MLOAD_ENTRY(_client_type);

    if (!KDrv_XC_MLoad_WriteCmd(_client_type,u32Addr, u16Data, u16Mask))
    {
        bRet = FALSE;
        MLDBG(printk("Fail: Cmd %4lx %x %x \n", u32Addr, u16Data, u16Mask));
    }
    else
    {
       if (!KDrv_XC_MLoad_Fire(_client_type,TRUE))
       {
            bRet = FALSE;
            MLDBG(printk("Fire Error!!!!\n "));
       }
    }
    _MLOAD_RETURN(_client_type);
#endif
    return bRet;
}


MS_BOOL KApi_XC_MLoad_WriteCmds_And_Fire(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 *pu32Addr, MS_U16 *pu16Data, MS_U16 *pu16Mask, MS_U16 u16CmdCnt)
{
    MS_BOOL bRet = TRUE;
#ifdef SUPPORT_KERNEL_MLOAD
    MS_U16 i = 0, j = 0;
    _MLOAD_ENTRY(_client_type);

    //Note: If there are some same regs in the Mload cmd queue,
    //  MLoad will use the value of the last reg address to fire. others same regs will be ignore.
    // So we handle this situation here, to combine every same reg's setting value to the same.
    for(i=0; i<u16CmdCnt; i++)
    {
        for(j=i+1; j<u16CmdCnt; j++)
        {
            if(pu32Addr[i] == pu32Addr[j])
            {
                pu16Mask[i] |= pu16Mask[j];
                pu16Data[i] |= pu16Data[j];
                pu16Mask[j] = pu16Mask[i];
                pu16Data[j] = pu16Data[i];
            }
        }
    }

    for(i=0; i<u16CmdCnt; i++)
    {

        if (!KDrv_XC_MLoad_WriteCmd(_client_type,pu32Addr[i], pu16Data[i], pu16Mask[i]))
        {
            bRet = FALSE;
            MLDBG(printk("Fail: Cmd %4lx %x %x \n", pu32Addr[i], pu16Data[i], pu16Mask[i]));
        }
    }

    if (!KDrv_XC_MLoad_Fire(_client_type,TRUE))
    {
        bRet = FALSE;
        MLDBG(printk("Fire Error!!!!\n "));
    }
    _MLOAD_RETURN(_client_type);
#endif
    return bRet;
}



//___|T_________________........__|T____ VSync
//__________|T__________________         ATP(refer the size befor memory to cal the pip sub and main length)
//_________________|T___________         Disp

//Generate TRAIN_TRIG_P from delayed line of Vsync(Setting the delay line for Auto tune area)(monaco ip DI point add trig engine)
//Generate DISP_TRIG_P from delayed line of Vsync(Setting the delay line for Display area)(monaco ip DI point add trig engine)
MS_BOOL KDrv_XC_MLoad_set_IP_trig_p(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 u16train, MS_U16 u16disp)
{
#ifdef SUPPORT_KERNEL_MLOAD
#if (HW_DESIGN_4K2K_VER == 4)
    SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x57,0x1A), u16train, 0x0FFF);
    SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x57,0x1B), u16disp,  0x0FFF);
#endif
#endif
    return TRUE;
}

//Get the delay line for Auto tune area
//Get the delay line for Display area
MS_BOOL KDrv_XC_MLoad_get_IP_trig_p(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 *pu16Train, MS_U16 *pu16Disp)
{
#ifdef SUPPORT_KERNEL_MLOAD
#if (HW_DESIGN_4K2K_VER == 4)
    *pu16Train = SC_R2BYTEMSK(_client_type, XC_ADDR_L(0x57,0x1A), 0x0FFF);
    *pu16Disp = SC_R2BYTEMSK(_client_type, XC_ADDR_L(0x57,0x1B), 0x0FFF);
#endif
#endif
    return TRUE;
}

void KDrv_XC_MLoad_Add_32Bits_Cmd(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Cmd, MS_U16 u16Mask)
{
#ifdef SUPPORT_KERNEL_MLOAD
    MS_U32 *pu32Addr = NULL;
    MS_PHYADDR DstAddr;
    if (  (ENABLE_MLOAD_SAME_REG_COMBINE == 1)
        &&(u32Cmd != MS_MLOAD_NULL_CMD))
    {
        MS_U16 u16WPoint = _client[_client_type].u16WPoint;
        MS_U16 u16CmdIndex = _client[_client_type].u16RPoint;
        MS_U32 u32CurrentAddr = 0;
        MS_U16 u16CurrentData = 0;
        MS_U32 u32OldAddr = 0;
        MS_U16 u16OldData = 0;
        for (u16CmdIndex = 0; u16CmdIndex < u16WPoint; u16CmdIndex++)
        {
            DstAddr = _client[_client_type].PhyAddr + u16CmdIndex * MS_MLOAD_CMD_LEN;
            pu32Addr = (MS_U32 *)_GET_PA2VA(DstAddr);
            KHal_XC_MLoad_parsing_32bits_subBankMode(_client_type, u32Cmd, &u32CurrentAddr, &u16CurrentData);
            KHal_XC_MLoad_parsing_32bits_subBankMode(_client_type, *pu32Addr, &u32OldAddr, &u16OldData);
            if (u32OldAddr == u32CurrentAddr)
            {
                if (u16OldData != u16CurrentData)
                {
                    MS_U16 u16NewData = 0;
                    u16NewData = (u16OldData & ~u16Mask) | (u16CurrentData & u16Mask);
                    MLDBG(printk("[Mload] [%s][line %d]  Old Cmd=%x \n",__FUNCTION__, __LINE__, u32Cmd));
                    u32Cmd =  KHal_XC_MLoad_Gen_32bits_subBankMode(_client_type, u32OldAddr, u16NewData, 0xFFFF);
                    MLDBG(printk("[Mload] [%s][line %d]  Old Cmd=%x \n",__FUNCTION__, __LINE__, u32Cmd));
                    *pu32Addr = u32Cmd;
                }

                return;
            }
        }
    }
    DstAddr = _client[_client_type].PhyAddr + _client[_client_type].u16WPoint * MS_MLOAD_CMD_LEN;
    pu32Addr = (MS_U32 *)_GET_PA2VA(DstAddr);
    *pu32Addr = u32Cmd;
    _client[_client_type].u16WPoint++;
#endif
}

void KDrv_XC_MLoad_Add_64Bits_Cmd(EN_MLOAD_CLIENT_TYPE _client_type,MS_U64 u64Cmd, MS_U16 u16Mask)
{
#ifdef SUPPORT_KERNEL_MLOAD
    MS_U64 *pu64Addr = NULL;
    MS_PHYADDR DstAddr;

    MS_U32 u32WriteMemShift = 0;
    if (  (ENABLE_MLOAD_SAME_REG_COMBINE == 1)
        &&(u64Cmd != MS_MLOAD_NULL_CMD_SPREAD_MODE))
    {
        MS_U16 u16CmdIndex = _client[_client_type].u16RPoint;
        MS_U16 u16WPoint = _client[_client_type].u16WPoint;

        for (; u16CmdIndex < u16WPoint; u16CmdIndex++)
        {
            MS_U32 u32CurrentAddr = 0;
            MS_U16 u16CurrentData = 0;
            MS_U32 u32OldAddr = 0;
            MS_U16 u16OldData = 0;
            u32WriteMemShift = _MDrv_XC_GetMLoadMemOffset(u16CmdIndex);

            DstAddr = _client[_client_type].PhyAddr + u32WriteMemShift;
            pu64Addr = (MS_U64 *)_GET_PA2VA(DstAddr);
            KHal_XC_MLoad_parsing_64bits_spreadMode_NonXC(*pu64Addr, &u32OldAddr, &u16OldData);
            KHal_XC_MLoad_parsing_64bits_spreadMode_NonXC(u64Cmd, &u32CurrentAddr, &u16CurrentData);
            if (  (u32OldAddr == u32CurrentAddr)
                &&((u32CurrentAddr&REG_SCALER_BASE) == REG_SCALER_BASE)) //only xc reg check
            {
                if (u16OldData != u16CurrentData)
                {
                    MS_U16 u16NewData = 0;
                    u16NewData = (u16OldData & ~u16Mask) | (u16CurrentData & u16Mask);
                    MLDBG(printk("[Mload] [%s][line %d]  Old Cmd=%llx \n",__FUNCTION__, __LINE__, u64Cmd));
                    u64Cmd = KHal_XC_MLoad_Gen_64bits_spreadMode_NonXC((u32CurrentAddr>>8)&0xFFFF, u32CurrentAddr&0xFF, u16NewData, 0xFFFF);
                    MLDBG(printk("[Mload] [%s][line %d]  New Cmd=%llx \n",__FUNCTION__, __LINE__, u64Cmd));
                    *pu64Addr = u64Cmd;
                }
                return;
            }
        }
    }

    u32WriteMemShift = _MDrv_XC_GetMLoadMemOffset(_client[_client_type].u16WPoint);

    DstAddr = _client[_client_type].PhyAddr + u32WriteMemShift;
    pu64Addr = (MS_U64 *)_GET_PA2VA(DstAddr);
    *pu64Addr = u64Cmd;
    _client[_client_type].u16WPoint++;
#endif
}

MS_BOOL KDrv_XC_MLoad_WriteCommand(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask)
{
    MS_BOOL bReturn = FALSE;
#ifdef SUPPORT_KERNEL_MLOAD
    // SC1 mload may not enable
    if (E_KDRV_MLOAD_ENABLED != KDrv_XC_MLoad_GetStatus(_client_type))
    {
        SC_W2BYTEMSK(_client_type, (0x130000 | u32Addr), u16Data, u16Mask);
    }
    else
    {
        if (IS_SUPPORT_64BITS_COMMAND(ENABLE_64BITS_COMMAND,_client_type/CLIENT_NUM))
        {
            MS_U64 u64Command = 0;

            if(ENABLE_64BITS_SPREAD_MODE)//64 bits spread mode
            {
                u64Command = KHal_XC_MLoad_Gen_64bits_spreadMode(_client_type,u32Addr,u16Data,u16Mask);
            }
            else//64 bits sub-bank mode
            {
                u64Command = KHal_XC_MLoad_Gen_64bits_subBankMode(_client_type,u32Addr,u16Data,u16Mask);
            }

            if(_KDrv_XC_MLoad_WriteCmd_64Bits(_client_type,u64Command, u16Mask))
            {
                bReturn = TRUE;
            }
            else
            {
                MLDBG(printk("Fail: %04lx %04x %04x \n", u32Addr, u16Data, u16Mask));
                bReturn = FALSE;
            }
        }
        else//32 bits sub-bank mode
        {
            MS_U32 u32Command = 0;
            u32Command = KHal_XC_MLoad_Gen_32bits_subBankMode(_client_type,u32Addr,u16Data,u16Mask);

            if(_KDrv_XC_MLoad_WriteCmd_32Bits(_client_type, u32Command, u16Mask))
            {
                bReturn = TRUE;
            }
            else
            {
                MLDBG(printk("Fail: %04lx %04x %04x \n", u32Addr, u16Data, u16Mask));
                bReturn = FALSE;
            }
        }
    }
#endif

    return bReturn;
}

//-------------------------------------------------------------------------------------------------
/// Write command to the Menuload buffer by WORD
/// @param  u32Bank                 \b IN: the address (direct-bank)
/// @param  u32Addr                 \b IN: the address (8-bit address)
/// @param  u16Data                 \b IN: the data
/// @param  u16Mask                 \b IN: the mask
/// @return  TRUE if succeed, FALSE if failed

// This function can support write non xc registers with menuload.
// If you want to write xc registers,you should use KApi_XC_MLoad_WriteCmd.

//-------------------------------------------------------------------------------------------------
MS_BOOL KApi_XC_MLoad_WriteCmd_NonXC(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Bank,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask)
{
    MS_BOOL bReturn = FALSE;
#ifdef SUPPORT_KERNEL_MLOAD
    _MLOAD_ENTRY(_client_type);
    bReturn = KDrv_XC_MLoad_WriteCommand_NonXC(_client_type,u32Bank,u32Addr, u16Data, u16Mask);
    _MLOAD_RETURN(_client_type);
#endif
    return bReturn;
}

MS_BOOL KDrv_XC_MLoad_WriteCommand_NonXC(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Bank,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask)
{
    MS_BOOL bReturn = FALSE;
#ifdef SUPPORT_KERNEL_MLOAD

    if (IS_SUPPORT_64BITS_COMMAND(ENABLE_64BITS_COMMAND,_client_type/CLIENT_NUM))
    {
        MS_U64 u64Command = 0;

        #if(ENABLE_64BITS_SPREAD_MODE)//64 bits spread mode
            u64Command = KHal_XC_MLoad_Gen_64bits_spreadMode_NonXC(u32Bank,u32Addr,u16Data,u16Mask);
        #else//64 bits sub-bank mode
            printk("Not support sub bank mode for non xc bank!");
        #endif

        if(_KDrv_XC_MLoad_WriteCmd_64Bits(_client_type,u64Command, u16Mask))
        {
            bReturn = TRUE;
        }
        else
        {
            MLDBG(printk("Fail: %04lx %04x %04x \n", u32Addr, u16Data, u16Mask));
            bReturn = FALSE;
        }
    }
    else //32 bits sub-bank mode
    {
        printk("Not support sub bank mode for non xc bank!");
        bReturn = FALSE;
    }
#endif
    return bReturn;
}

void KDrv_XC_MLoad_set_trigger_sync(EN_MLOAD_CLIENT_TYPE _client_type,EN_MLOAD_TRIG_SYNC eTriggerSync)
{
#ifdef SUPPORT_KERNEL_MLOAD
    KHal_XC_MLoad_set_trigger_sync(_client_type,eTriggerSync);
#endif
}

EXPORT_SYMBOL(KApi_XC_MLoad_Init);
EXPORT_SYMBOL(KApi_XC_MLoad_Enable);
EXPORT_SYMBOL(KApi_XC_MLoad_GetStatus);
EXPORT_SYMBOL(KApi_XC_MLoad_Fire);
EXPORT_SYMBOL(KApi_XC_MLoad_WriteCmd);
EXPORT_SYMBOL(KApi_XC_MLoad_WriteCmd_NonXC);
EXPORT_SYMBOL(KDrv_XC_MLoad_set_IP_trig_p);
EXPORT_SYMBOL(KDrv_XC_MLoad_get_IP_trig_p);
EXPORT_SYMBOL(KDrv_XC_MLoad_set_trigger_sync);

#endif //_MDRV_XC_MENULOAD_C_
