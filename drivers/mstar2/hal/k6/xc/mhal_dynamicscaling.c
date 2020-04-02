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
// Common Definition
#ifndef MHAL_DYNAMICSCALING_C
#define MHAL_DYNAMICSCALING_C
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
#include <linux/version.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/proc_fs.h>



#include "chip_int.h"
#include "mdrv_mstypes.h"
#include "mhal_xc_chip_config.h"
#include "mdrv_xc_st.h"
#include "mhal_xc.h"
#include "mhal_menuload.h"
#include "mhal_dynamicscaling.h"
#include "mstar/mstar_chip.h"

#ifdef  MSOS_TYPE_LINUX
#include <assert.h>
#else
#define assert(b)   \
    do {    \
        if (!(b)){  \
            printk("assert at [%s][%d] \n", __FUNCTION__, __LINE__);   \
            *((int*)0) = 0; \
        }   \
    } while(0)
#endif

#define DS_DBG(x)   //x
#define SLZ_DBG(x)  //x
//clear DS Buffer

#define DEVICE_NUM 2

//********************DS NEW******************//
ds_node *ds_client_list = NULL;
//********************************************//

typedef struct
{
    MS_U32 u32DynamicScalingBaseAddr;
#if ENABLE_64BITS_SPREAD_MODE
    MS_U32 u32DynamicScalingBaseAddr_OPM;
    MS_U32 u32DynamicScalingBaseAddr_IPS;
    MS_U32 u32DynamicScalingBaseAddr_OPS;
#endif
    MS_U16  u8DynamicScalingIndexDepth;
    MS_U32 u32DynamicScalingBufSize;
    MS_BOOL bDynamicScalingEnable;
    MS_VIRT va_phy;
    MS_U8 au8DSCurrentIndex[E_KDRV_MAX_WINDOW]; // index of writing ds data
    MS_U8 au8DSFireIndex[E_KDRV_MAX_WINDOW];    // index of firing ds data
} KHAL_DS_INFO;


typedef struct
{
    MS_U16 su16OldValue;
    MS_U16 su16OldValue_BWD_Status;
    MS_BOOL sbOldValueReaded;
    MS_BOOL bTrig_pStored;
} KHAL_DS_PRIVATE;
KHAL_DS_INFO client_ds[E_KDRV_XC_MAX];      //client_ds[u32DeviceID]
KHAL_DS_PRIVATE ds_private[E_KDRV_XC_MAX];
extern MLOAD_PRIVATE _mload_private;
static KHAL_DS_STORED_ADDR_INFO _stDSStoredAddrInfo;
static  MS_BOOL  _bIOUnmap[4] = {FALSE, FALSE, FALSE, FALSE};
static DEFINE_MUTEX(_ds_mutex);

//********************DS NEW******************//
void ll_add( ds_client_data data )
{
    ds_node *temp,*right;
    MS_U32 offset = 0;

    temp= (ds_node *)kmalloc(sizeof(ds_node),GFP_KERNEL);
    memcpy(&(temp->data),&data,sizeof(ds_client_data));
    right=(ds_node *)ds_client_list;
    if(ds_client_list == NULL)
    {
        ds_client_list = temp;
        temp->next=NULL;
        temp->data.offset = offset;                 //offset calcultaion
        //printk("\033[1;32m[%s:%d]ds_info_client has been create & add [%d] -->offset = %d ,max_depth_num=%d\033[m\n",__FUNCTION__,__LINE__
        //        ,temp->data.client
        //        ,temp->data.offset
        //        ,temp->data.max_depth_num);
    }
    else
    {
        while(right->next != NULL)
        {
            offset += right->data.max_depth_num;    //offset calculation
            right=right->next;
        }
        offset += right->data.max_depth_num;    //offset calculation
        temp->next=NULL;
        temp->data.offset = offset;                 //offset calculation
        right->next =temp;
        //printk("\033[1;32m[%s:%d]ds_info_client has been added [%d] -->offset = %d,max_depth_num=%d\033[m\n",__FUNCTION__,__LINE__
        //        ,temp->data.client
        //        ,temp->data.offset
        //        ,temp->data.max_depth_num);
    }
}
MS_BOOL ll_delete(ds_client_data data)
{
    ds_node *temp, *prev;
    temp = ds_client_list;
    while(temp!=NULL)
    {
        if(!memcmp(&(temp->data),&data,sizeof(ds_client_data)))
        {
            if(temp==ds_client_list)
            {
                ds_client_list=temp->next;
                //printk("\033[1;32m[%s:%d]ds_info_client has been deleted [%d]\033[m\n",__FUNCTION__,__LINE__,temp->data.client);
                kfree(temp);
                return TRUE;
            }
            else
            {
                prev->next=temp->next;
                //printk("\033[1;32m[%s:%d]ds_info_client has been deleted [%d]\033[m\n",__FUNCTION__,__LINE__,temp->data.client);
                kfree(temp);
                return TRUE;
            }
        }
        else
        {
            prev=temp;
            temp= temp->next;
        }
    }
    return FALSE;
}
MS_BOOL ll_delete_tail(void)
{
    ds_node *temp, *var;
    temp=ds_client_list;
    while(temp->next != NULL)
    {
        var=temp;
        temp=temp->next;
    }
    if(temp == ds_client_list)
    {
        ds_client_list=temp->next;
        kfree(temp);
        return TRUE;
    }
    //printk("data deleted from list is %d",temp->data);
    var->next=NULL;
    kfree(temp);
    return TRUE;
}
MS_BOOL ll_delete_all(void)
{
    while(ds_client_list!=NULL)
        ll_delete_tail();
    //printk("\033[1;32m[%s:%d]ds_info_client has been erased\033[m\n",__FUNCTION__,__LINE__);
    return TRUE;
}
MS_BOOL ll_get_data(E_DS_CLIENT client,ds_client_data* data)
{
    ds_node *temp;
    temp = ds_client_list;
    if(temp == NULL)
        return FALSE;
    while(temp->data.client!=client)
    {
        if(temp->next == NULL)
            return FALSE;
        temp = temp->next;
    }
    memcpy(data,&(temp->data),sizeof(ds_client_data));
    //printk("\033[1;32m[%s:%d][%d] -->offset = %d,max_depth_num=%d\033[m\n",__FUNCTION__,__LINE__
    //        ,temp->data.client
    //        ,temp->data.offset
    //        ,temp->data.max_depth_num);
    return TRUE;
}
//********************************************//
MS_VIRT _DS_PA2VA(phys_addr_t phy,phys_addr_t len)
{
    phys_addr_t tmp_phy;

    if(phy<ARM_MIU1_BASE_ADDR)
    {
        tmp_phy = phy + ARM_MIU0_BUS_BASE ;
    }
    else if((phy >= ARM_MIU1_BASE_ADDR) && (phy<ARM_MIU2_BASE_ADDR))
    {
        tmp_phy = phy - ARM_MIU1_BASE_ADDR + ARM_MIU1_BUS_BASE ;
    }
    else
    {
        tmp_phy = phy - ARM_MIU2_BASE_ADDR + ARM_MIU2_BUS_BASE ;
    }

    if (pfn_valid(__phys_to_pfn(tmp_phy)))
    {
        return __va(tmp_phy);
    }
    else
    {
        return ioremap(tmp_phy, len);
    }
}

MS_VIRT _DSGET_PA2VA(EN_KDRV_SC_DEVICE u32DeviceID,MS_PHYADDR DstAddr)
{
    MS_PHYADDR tmp_diff=0;
    if((DstAddr >= client_ds[u32DeviceID].u32DynamicScalingBaseAddr) && (DstAddr< (client_ds[u32DeviceID].u32DynamicScalingBaseAddr + client_ds[u32DeviceID].u32DynamicScalingBufSize)))
    {
        tmp_diff = DstAddr - client_ds[u32DeviceID].u32DynamicScalingBaseAddr;
        return (client_ds[u32DeviceID].va_phy + tmp_diff);
    }
    else
    {
        printk("%s: No PA 2 VA MAPPING!, BaseAddr: 0x%x, DstAddr: 0x%x\n", __FUNCTION__, client_ds[u32DeviceID].u32DynamicScalingBaseAddr, DstAddr);
        return 0;
    }
}

//CMD in DRAM: with dummy command
//ex: (MS_MLOAD_BUS_WIDTH == 16) && (BYTE_PER_WORD == 32)

// |------ 128 bit ------|
// |  CMD0   |   CMD1    |
// |  dummy  |   dummy   |
// |  CMD2   |   CMD3    |
// |  dummy  |   dummy   |
//CMD in DRAM:   no dummy command
//ex: ((MS_MLOAD_BUS_WIDTH == 16) && (BYTE_PER_WORD == 16))  or ((MS_MLOAD_BUS_WIDTH == 32) && (BYTE_PER_WORD == 32))

// |------ 128 bit ------|
// |  CMD0   |   CMD1    |
// |  CMD2   |   CMD3    |
// |  CMD4   |   CMD5    |
// |  CMD6   |   CMD7    |
static MS_U32 KHal_XC_GetMemOffset(MS_U32 u32Index)
{
    MS_U32 u32Offset = 0;
    MS_U32 u32CmdCntPerMIUBus = (MS_MLOAD_BUS_WIDTH / MS_MLOAD_CMD_LEN_64BITS);

    u32Offset = (u32Index / u32CmdCntPerMIUBus) * MS_MLOAD_MEM_BASE_UNIT + (u32Index % u32CmdCntPerMIUBus) * MS_MLOAD_CMD_LEN_64BITS;

    return u32Offset;
}

void KHal_SC_ResetSWDSCommand(EN_KDRV_SC_DEVICE u32DeviceID,EN_KDRV_WINDOW eWindow, MS_U8 u8DSIndex)
{
#if ENABLE_64BITS_SPREAD_MODE

    MS_PHY u32IndexBase = client_ds[u32DeviceID].u32DynamicScalingBaseAddr + u8DSIndex * (client_ds[u32DeviceID].u8DynamicScalingIndexDepth * BYTE_PER_WORD);

    MS_U16 i = 0;

#if SUPPORT_DS_MULTI_USERS
    ds_client_data client_data;
    MS_U8 u8XCDepth = 0;

    if(ll_get_data(E_DS_CLIENT_XC, &client_data))
    {
        u8XCDepth = client_data.max_depth_num;
    }
    //printk("%s u8XCDepth 0x%lx!!!\n", __FUNCTION__,u8XCDepth);

    MS_U16 u16SWRstCmd_Pointer = 0;
    if(u8XCDepth == 0)
    {
        u16SWRstCmd_Pointer = ((BYTE_PER_WORD*(client_ds[u32DeviceID].u8DynamicScalingIndexDepth-1))/DS_CMD_LEN_64BITS);
    }
    else
    {
        u16SWRstCmd_Pointer = (u8XCDepth-1)*((BYTE_PER_WORD/DS_CMD_LEN_64BITS)/2);
    }
#else
    MS_U16 u16SWRstCmd_Pointer = ((BYTE_PER_WORD*(client_ds[u32DeviceID].u8DynamicScalingIndexDepth-1))/DS_CMD_LEN_64BITS);
#endif
    MS_U64 *pu64Data = NULL;
    MS_U32 ActiveSWClearEn = 0;

    MS_U16 u16CmdCntPerIndex = client_ds[u32DeviceID].u8DynamicScalingIndexDepth*((BYTE_PER_WORD/DS_CMD_LEN_64BITS));
    //printk("%s u32IndexBase 0x%lx u8DSIndex %d u8DynamicScalingIndexDepth %d, %d, %d!!!\n", __FUNCTION__,u32IndexBase,u8DSIndex,client_ds[u32DeviceID].u8DynamicScalingIndexDepth,u16CmdCntPerIndex,u16SWRstCmd_Pointer);

    //printk("%s u16CmdCntPerIndex 0x%lx 0x%lx!!!\n", __FUNCTION__,u16SWRstCmd_Pointer,u16CmdCntPerIndex);
    if((u16SWRstCmd_Pointer == 0 ) || (u16SWRstCmd_Pointer == 1))
    {
        printk("%s :u16SWRstCmd_Pointer <= 0,error!!!\n", __FUNCTION__);
        //MS_ASSERT(u16SWRstCmd_Pointer > 0);
    }
    else
    {
        MS_PHY u32IPBaseAddr = u32IndexBase;
        for(i = 0; i < u16CmdCntPerIndex; i++)
        {
            pu64Data  = (MS_U64*)_DSGET_PA2VA(u32DeviceID,(u32IPBaseAddr + i*DS_CMD_LEN_64BITS));
            #if 0
            if(i == 0)
            {
                //0xFFFF00098F97048F; //BK1F_17[10] : reg_ds_ipm_active_sel: 0:HW 1:SW

                MS_U32 u32SW_Reset_Enable = 0;
                //ActiveSWClearEn enable in first DS IP command ;
                u32SW_Reset_Enable = 0x4000; // bit14: reg_ds_active_sw_clr_en,set ds_activating cleared by SW
                ActiveSWClearEn = SC_R2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x17), 0xFFFF);
                ActiveSWClearEn |= u32SW_Reset_Enable;
                *pu64Data = KHal_SC_GenSpreadModeCmd(u32DeviceID,(((MS_U16)(0x1F) << 8) | (MS_U16)((0x17)*2)) , (MS_U16)(ActiveSWClearEn), 0xFFFF, K_DS_XC);
            }
            else if(i == u16SWRstCmd_Pointer)
            {
                //toggle reg_ds_ipm_active_set in last DS IP command
                //0xFFFF00098F97058F;//BK1F_17[8] : reg_ds_ipm_active_set
                MS_U32 u32SW_Reset_IPM = 0;
                u32SW_Reset_IPM =  0x0010 ;//bit4 : reg_ds_ipm_active_clr
                ActiveSWClearEn |= u32SW_Reset_IPM;
                *pu64Data = KHal_SC_GenSpreadModeCmd(u32DeviceID, (((MS_U16)(0x1F) << 8) | (MS_U16)((0x17)*2)), (MS_U16)(ActiveSWClearEn), 0xFFFF, K_DS_XC);
            }
            else
            #endif
            {
                *pu64Data = MS_DS_NULL_CMD_64BITS; //Dummy cmd BKFF_7F=0x0000
            }
        }

        MS_PHY u32OPBaseAddr = u32IndexBase + client_ds[u32DeviceID].u32DynamicScalingBufSize/2;

        for(i = 0; i < u16CmdCntPerIndex; i++)
        {
            pu64Data  = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32OPBaseAddr + i*DS_CMD_LEN_64BITS);
            *pu64Data = MS_DS_NULL_CMD_64BITS; //Dummy cmd BKFF_7F=0x0000
        }
    }
#else
    MS_PHY u32IndexBase = client_ds[u32DeviceID].u32DynamicScalingBaseAddr + (MS_U32)u8DSIndex * client_ds[u32DeviceID]u8DynamicScalingIndexDepth * MS_MLOAD_MEM_BASE_UNIT;

    MS_U32 *pu32Data = NULL;
    int i = 0;

    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] u32DSBaseAddr:%lx  u32IndexBase:%ld  u8DSIndexDepth:%d  \n",__func__,__LINE__,client_ds[u32DeviceID].u32DynamicScalingBaseAddr,u32IndexBase,client_ds[u32DeviceID].u8DynamicScalingIndexDepth);

    for(i = 0; i < (client_ds[u32DeviceID].u8DynamicScalingIndexDepth * MS_MLOAD_MEM_BASE_UNIT)/4; i++) //need check
    {
        if(i % 8 < 4)
        {
            if(E_KDRV_MAIN_WINDOW == eWindow)
            {
                if(i % 4 < 2)
                {
                    pu32Data = (MS_U32*)_DSGET_PA2VA(u32DeviceID,(u32IndexBase + (i*4)));
                    if(0 == pu32Data)
                    {
                        printk("%s :pu32Data_1 = 0!,error!!!\n", __FUNCTION__);
                        assert(pu32Data != 0);
                    }
                    *pu32Data = 0xFFFF0000;
                }
            }
            else //SUB_WINDOW == eWindow
            {
                if(i % 4 >= 2)
                {
                    pu32Data = (MS_U32*)_DSGET_PA2VA(u32DeviceID,(u32IndexBase + (i*4)));
                    if(0 == pu32Data)
                    {
                        printk("%s :pu32Data_2 = 0!,error!!!\n", __FUNCTION__);
                        assert(pu32Data != 0);
                    }
                    *pu32Data = 0xFFFF0000;
                }
            }
        }
        else
        {
            if(E_KDRV_MAIN_WINDOW == eWindow)
            {
                if(i % 4 < 2)
                {
                    pu32Data = (MS_U32*)_DSGET_PA2VA(u32DeviceID,(u32IndexBase + (i*4)));
                    if(0 == pu32Data)
                    {
                        printk("%s :pu32Data_3 = 0!,error!!!\n", __FUNCTION__);
                        assert(pu32Data != 0);
                    }
                    *pu32Data = 0xFFFF0000;//0;

                    //printk("[szuhua] [%s,%5d] [IP] 0xFFFF0000 \n",__func__,__LINE__);
                }
            }
            else //SUB_WINDOW == eWindow
            {
                if(i % 4 >= 2)
                {
                    pu32Data = (MS_U32*)_DSGET_PA2VA(u32DeviceID,(u32IndexBase + (i*4)));
                    if(0 == pu32Data)
                    {
                        printk("%s :pu32Data_4 = 0!,error!!!\n", __FUNCTION__);
                        assert(pu32Data != 0);
                    }
                    *pu32Data = 0xFFFF0000;
                }
            }
        }
    }
#endif
}

void KHal_SC_WriteSWDSCommand(EN_KDRV_SC_DEVICE u32DeviceID,EN_KDRV_WINDOW eWindow,E_DS_CLIENT client, MS_U32 u32CmdRegAddr, MS_U16 u16CmdRegValue,k_ds_reg_ip_op_sel IPOP_Sel,k_ds_reg_source_sel Source_Select,K_XC_DS_CMDCNT *pstXC_DS_CmdCnt,MS_U8 u8DSIndex)
{

    MS_U32 offset = 0;
    MS_U8 depth;

#if SUPPORT_DS_MULTI_USERS
    ds_client_data client_data;
    if(ll_get_data(client,&client_data))
    {
        offset += client_data.offset;
    }
#endif
#if ENABLE_DS_4_BASEADDR_MODE
    MS_PHY u32Addr = client_ds[u32DeviceID].u32DynamicScalingBaseAddr + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth+offset) * MS_MLOAD_MEM_BASE_UNIT;
    MS_PHY u32Addr_OPM = client_ds[u32DeviceID].u32DynamicScalingBaseAddr_OPM + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth+offset) * MS_MLOAD_MEM_BASE_UNIT;
    MS_PHY u32Addr_IPS = client_ds[u32DeviceID].u32DynamicScalingBaseAddr_IPS + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth+offset) * MS_MLOAD_MEM_BASE_UNIT;
    MS_PHY u32Addr_OPS = client_ds[u32DeviceID].u32DynamicScalingBaseAddr_OPS + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth+offset) * MS_MLOAD_MEM_BASE_UNIT;
#else
    MS_PHY u32Addr = client_ds[u32DeviceID].u32DynamicScalingBaseAddr + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth+offset) * MS_MLOAD_MEM_BASE_UNIT;
#endif

    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] u32DynamicScalingBaseAddr:%x  CurIdx:%d  IdxDepth:%d  BYTE_PER_WORD:%d  \n",__func__,__LINE__,(int)client_ds[u32DeviceID].u32DynamicScalingBaseAddr,(int)client_ds[u32DeviceID].u8DynamicScalingCurrentIndex[eWindow],(int)client_ds[u32DeviceID].u8DynamicScalingIndexDepth,(int)BYTE_PER_WORD);
    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] u32Addr:%x  \n",__func__,__LINE__,(int)u32Addr);

#if ENABLE_DS_4_BASEADDR_MODE

    MS_U64 *pu64Data = NULL;
    MS_U64 u64CmdTemp = 0;
    MS_U8  u8AddrTemp = 0;
    MS_U16 u16BankTemp = 0;
    MS_U16 u16DataTemp = 0;
    MS_U16 u16MaskTemp = 0;
    //MS_U16 u16MaxCmdCnt = client_ds[u32DeviceID].u8DynamicScalingIndexDepth * (Hal_XC_MLoad_Get_64Bits_MIU_Bus_Sel(pInstance) + 1);

    depth=client_ds[u32DeviceID].u8DynamicScalingIndexDepth;
#if SUPPORT_DS_MULTI_USERS
    if(ll_get_data(client,&client_data))
    {
        depth = client_data.max_depth_num;
    }
#endif
    MS_U16 u16MaxCmdCnt = depth * ((SC_R2BYTEMSK(u32DeviceID,XC_ADDR_L(0x1F,0x13), 0xC000) >>14) + 1);
    //MS_U16 u16MaxCmdCnt = client_ds[u32DeviceID].u8DynamicScalingIndexDepth * ((SC_R2BYTEMSK(u32DeviceID,XC_ADDR_L(0x1F,0x13), 0xC000) >>14) + 1);

//===========================================================//
//  64bit command format (spread mode)
//  |-------------|-------------|----------------------------|-------------|
//  |    Bit-En   |    Unused   |    Addr(FULL RIU ADDR)     |     Data    |
//  |-------------|-------------|----------------------------|-------------|
//  |   [63:48]   |   [47:40]   |           [39:16]          |   [15:0]    |
//  |-------------|-------------|----------------------------|-------------|

//Addr: FULL RIU ADDRESS !!!

//For example:  Subbank:0x01            addr:0x40 Data:0x0001
//              Spread mode :  0x1301   addr:0x40 Data:0x0001

//|----------------------------------------------------|
//|                      Bank             |    addr    |
//|----------------------------------------------------|
//|                     0x1301            |    0x40    |
//|----------------------------------------------------|
// FULL RIU ADDRESS: |0001 0011 0000 0001 |  100 0000  |
// FULL RIU ADDRESS:              0x0980C1
//===========================================================//

    if(Source_Select == K_DS_XC)
    {
        u16MaskTemp = 0xFFFF;
        u16DataTemp = (SC_R2BYTE(u32DeviceID,(0x130000 | u32CmdRegAddr)) & ~0xFFFF) | (u16CmdRegValue & 0xFFFF);        //marcos,have to do sth here

        u8AddrTemp= (u32CmdRegAddr & 0x000000FF) >> 1;
        u16BankTemp= 0x1300 | ((u32CmdRegAddr >> 8) & 0x000000FF); // 0x13XX xc sread mode address

        u64CmdTemp|= (MS_U64)u16DataTemp;
        u64CmdTemp|= ((MS_U64)u8AddrTemp<<16);
        u64CmdTemp|= ((MS_U64)u16BankTemp<<23);
        u64CmdTemp|= ((MS_U64)u16MaskTemp<<48);

        if(IPOP_Sel == K_DS_IP)
        {
            if(eWindow == E_KDRV_MAIN_WINDOW)
            {
                MS_U32 u32MemShift_IPM;
                u32MemShift_IPM = KHal_XC_GetMemOffset(pstXC_DS_CmdCnt->u16CMDCNT_IPM);

                //XC_PRINTF("[%s,%5d] [DS_IP]pstXC_DS_CmdCnt->u16CMDCNT_IPM:%d u32MemShift_IPM:%lx \n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM,(MS_U32)u32MemShift_IPM);
                //u32Addr = u32Addr + DS_CMD_LEN_64BITS * pstXC_DS_CmdCnt->u16CMDCNT_IPM;
                u32Addr = u32Addr + u32MemShift_IPM;
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr);//IPM

                if(pstXC_DS_CmdCnt->u16CMDCNT_IPM < u16MaxCmdCnt)
                {
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_IPM++;
                }
                else
                {
                    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] IPM Command count overflow !!  u16CMDCNT_IPM:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM,client_ds[u32DeviceID].u8DynamicScalingIndexDepth);
                }
            }
            else//subwindow
            {
                u32Addr_IPS = u32Addr_IPS + DS_CMD_LEN_64BITS * pstXC_DS_CmdCnt->u16CMDCNT_IPS;
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr_IPS); // IPS

                if(pstXC_DS_CmdCnt->u16CMDCNT_IPS < u16MaxCmdCnt)
                {
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_IPS++;
                }
                else
                {
                    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] IPS Command count overflow !!  u16CMDCNT_IPS:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPS,client_ds[u32DeviceID].u8DynamicScalingIndexDepth);
                }
            }
        }
        else//DS_OP
        {
            if(eWindow == E_KDRV_MAIN_WINDOW)
            {
                MS_U32 u32MemShift_OPM;
                u32MemShift_OPM = KHal_XC_GetMemOffset(pstXC_DS_CmdCnt->u16CMDCNT_OPM);

                //XC_PRINTF("[%s,%5d] [DS_OP]pstXC_DS_CmdCnt->u16CMDCNT_OPM:%d u32MemShift_OPM:%lx \n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_OPM,(MS_U32)u32MemShift_OPM);
                //u32Addr_OPM = u32Addr_OPM + DS_CMD_LEN_64BITS * pstXC_DS_CmdCnt->u16CMDCNT_OPM;
                u32Addr_OPM = u32Addr_OPM + u32MemShift_OPM;
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr_OPM);//OPM

                //XC_PRINTF("[%s,%5d] u64CmdTemp:%llx  \n",__func__,__LINE__,u64CmdTemp);

                if(pstXC_DS_CmdCnt->u16CMDCNT_OPM < u16MaxCmdCnt)
                {
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_OPM++;
                }
                else
                {
                    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] OPM Command count overflow !!  u16CMDCNT_OPM:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_OPM,client_ds[u32DeviceID].u8DynamicScalingIndexDepth);
                }
            }
            else
            {
                u32Addr_OPS = u32Addr_OPS + DS_CMD_LEN_64BITS * pstXC_DS_CmdCnt->u16CMDCNT_OPS;
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr_OPS); // OPS

                if(pstXC_DS_CmdCnt->u16CMDCNT_OPS < u16MaxCmdCnt)
                {
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_OPS++;
                }
                else
                {
                    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] OPS Command count overflow !!  u16CMDCNT_OPS:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_OPS,client_ds[u32DeviceID].u8DynamicScalingIndexDepth);
                }
            }
        }
    }
    else if(Source_Select == K_DS_MVOP)// only in IP cmd
    {
        if(IPOP_Sel == K_DS_IP)
        {
            u16DataTemp = u16CmdRegValue;
            u8AddrTemp = (u32CmdRegAddr&0x000000FF);
            u16BankTemp = (u32CmdRegAddr&0x00FFFF00) >> 8; //EX: mvop mainwinodw: 0x1014 mvop bank
            u16MaskTemp = 0xFFFF;

            u64CmdTemp|= (MS_U64)u16DataTemp;
            u64CmdTemp|= ((MS_U64)u8AddrTemp<<16) >> 1;
            u64CmdTemp|= ((MS_U64)u16BankTemp<<23);
            u64CmdTemp|= ((MS_U64)u16MaskTemp<<48);

            if(eWindow == E_KDRV_MAIN_WINDOW)
            {
                MS_U32 u32MemShift_IPM;
                u32MemShift_IPM = KHal_XC_GetMemOffset(pstXC_DS_CmdCnt->u16CMDCNT_IPM);

                //XC_PRINTF("[%s,%5d] [DS_IP(MVOP)]pstXC_DS_CmdCnt->u16CMDCNT_IPM:%d u32MemShift_IPM:%lx \n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM,(MS_U32)u32MemShift_IPM);
                //u32Addr = u32Addr + DS_CMD_LEN_64BITS * pstXC_DS_CmdCnt->u16CMDCNT_IPM;
                u32Addr = u32Addr + u32MemShift_IPM;
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr);//IPM

                if(pstXC_DS_CmdCnt->u16CMDCNT_IPM < u16MaxCmdCnt)
                {
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_IPM++;
                }
                else
                {
                    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] Command count overflow !!  u16CMDCNT_IPM:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM,client_ds[u32DeviceID].u8DynamicScalingIndexDepth);
                }
            }
            else
            {
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr_IPS);//IPS

                if(pstXC_DS_CmdCnt->u16CMDCNT_IPS < u16MaxCmdCnt)
                {
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_IPS++;
                }
                else
                {
                    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] Command count overflow !!  u16CMDCNT_IPS:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPS,client_ds[u32DeviceID].u8DynamicScalingIndexDepth);
                }
            }
        }
        else
        {
            //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] MVOP is IP cmd not OP cmd !! \n",__func__,__LINE__);
        }

    }
    else if(Source_Select == K_DS_GOP)
    {

    }
    else
    {
        //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] Only can support XC/MVOP/GOP NOW !! \n",__func__,__LINE__);
    }

#else

//=============================================//
//  32bit command format
//  |-------------|-------------|-------------|
//  |     Bank    |     Addr    |     Data    |
//  |-------------|-------------|-------------|
//  |   [31:24]   |   [23:16]   |    [15:0]   |
//  |-------------|-------------|-------------|

//Bank: Subbank
//=============================================//

    if(IPOP_Sel == K_DS_OP)
    {
        MS_PHY u32OPAddr = u32Addr;
        MS_U32 *pu32Data = NULL;
        MS_U8 u8OPCount = client_ds[u32DeviceID].u8DynamicScalingIndexDepth;
        // MS_U8 u8OPCount = IdxDepth;
        MS_PHY u32Command = 0;

        u32Command = u16CmdRegValue + ((u32CmdRegAddr & 0x00FF) << 15) + ((u32CmdRegAddr & 0xFF00) << 16);

        while(u8OPCount > 0)
        {
            pu32Data = (MS_U32*)_DSGET_PA2VA(u32DeviceID,u32OPAddr);
            if(0 == pu32Data)
            {
                printk("%s :pu32Data = 0!,error!!!\n", __FUNCTION__);

                assert(pu32Data != 0);
            }
            // Found null command
            if((MS_U32)(*pu32Data) == 0xFFFF0000)
            {
                break;
            }
            // Found duplicated command
            if((MS_U32)(*pu32Data) == u32Command)
            {
                //SC_DBG(printk("Command duplicated\n"));

                return;
            }
            u32OPAddr += MS_MLOAD_MEM_BASE_UNIT;
            u8OPCount--;
        }

        if((MS_U32)(*pu32Data) == 0xFFFF0000)
        {
            *pu32Data = u32Command;
        }
        else
        {
            printk("DS OP command buffer is full for this index %u!\n", u8DSIndex);
        }
        //printk("@@[%s][%u]u16CmdRegAddr=%x, u16CmdRegValue=%x\n",__FUNCTION__, __LINE__, u16CmdRegAddr, u16CmdRegValue);
    }

    else
    {
        // (OP : IP : Unused : Unused) (4 byte : 4 byte : 4 byte : 4 byte) if MIU 128bit
        // (OPmain : IPmain : OPsub : IPsub) (4 byte : 4 byte : 4 byte : 4 byte) if MIU 128bit

        MS_PHY u32IPAddr = u32Addr + DS_OP_CMD_LEN;
        MS_U32 *pu32Data = NULL;
        MS_U8 u8IPCount = client_ds[u32DeviceID].u8DynamicScalingIndexDepth;
        //MS_U8 u8IPCount = IdxDepth;
        MS_PHY u32Command = 0;

        u32Command = u16CmdRegValue + ((u32CmdRegAddr & 0x00FF) << 15) + ((u32CmdRegAddr & 0xFF00) << 16);

        while(u8IPCount > 0)
        {
            pu32Data = (MS_U32*)_DSGET_PA2VA(u32DeviceID,u32IPAddr);
            if(0 == pu32Data)
            {
                printk("%s :pu32Data = 0!,error!!!\n", __FUNCTION__);

                assert(pu32Data != 0);
            }
            // Found null command
            if((MS_U32)(*pu32Data) == 0xFFFF0000)
            {
                break;
            }
            // Found duplicated command
            if((MS_U32)(*pu32Data) == u32Command)
            {
                return;
            }
            u32IPAddr += MS_MLOAD_MEM_BASE_UNIT;
            u8IPCount--;
        }

        if((MS_U32)(*pu32Data) == 0xFFFF0000)
        {
            *pu32Data = u32Command;
        }
        else
        {
            printk("DS IP command buffer is full for this index %u!\n", u8DSIndex);
        }
        //printk("@@[%s][%u]u16CmdRegAddr=%x, u16CmdRegValue=%x\n",__FUNCTION__, __LINE__, u16CmdRegAddr, u16CmdRegValue);
    }
#endif
}

void KHal_XC_WriteSWDSCommandNonXC(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, MS_U32 u32Bank, MS_U16 u32Addr, MS_U32 u16Data, MS_U16 u16Mask, k_ds_reg_ip_op_sel IPOP_Sel, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt, MS_U8 u8DSIndex)
{
    MS_U32 offset = 0;
    MS_U8 depth;

#if SUPPORT_DS_MULTI_USERS
    ds_client_data client_data;
    if(ll_get_data(client,&client_data))
    {
        offset += client_data.offset;
    }
#endif

#if ENABLE_DS_4_BASEADDR_MODE
    MS_PHY u32Addr_IPM = client_ds[u32DeviceID].u32DynamicScalingBaseAddr + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth + offset) * MS_MLOAD_MEM_BASE_UNIT;
    MS_PHY u32Addr_OPM = client_ds[u32DeviceID].u32DynamicScalingBaseAddr_OPM + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth + offset) * MS_MLOAD_MEM_BASE_UNIT;
    MS_PHY u32Addr_IPS = client_ds[u32DeviceID].u32DynamicScalingBaseAddr_IPS + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth + offset) * MS_MLOAD_MEM_BASE_UNIT;
    MS_PHY u32Addr_OPS = client_ds[u32DeviceID].u32DynamicScalingBaseAddr_OPS + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth + offset) * MS_MLOAD_MEM_BASE_UNIT;
#else
    MS_PHY u32Addr_IPM = client_ds[u32DeviceID].u32DynamicScalingBaseAddr + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth + offset) * MS_MLOAD_MEM_BASE_UNIT;
#endif

    depth = client_ds[u32DeviceID].u8DynamicScalingIndexDepth;
#if SUPPORT_DS_MULTI_USERS
    if(ll_get_data(client,&client_data))
    {
        depth = client_data.max_depth_num;
    }
#endif
    //MS_U8 u16MaxCmdCnt = client_ds[u32DeviceID].u8DynamicScalingIndexDepth * 2;
    MS_U16 u16MaxCmdCnt = depth * ((SC_R2BYTEMSK(u32DeviceID,XC_ADDR_L(0x1F,0x13), 0xC000) >>14) + 1);

    MS_U64 *pu64Data = NULL;
    MS_U64 u64CmdTemp = 0;

    u64CmdTemp = ((MS_U64)((SC_R2BYTE(u32DeviceID, (u32Bank<<8)| u32Addr) & ~u16Mask) | (u16Data & u16Mask)));
    u64CmdTemp|= ((MS_U64) ((u32Addr<<16) >>1));
    u64CmdTemp|= ((MS_U64)u32Bank<<23);
    u64CmdTemp|= ((MS_U64)0xFFFF<<48);

    if(IPOP_Sel == K_DS_IP)
    {
        if(eWindow == MAIN_WINDOW)
        {
            MS_U32 u32MemShift_IPM;
            u32MemShift_IPM = KHal_XC_GetMemOffset(pstXC_DS_CmdCnt->u16CMDCNT_IPM);

            //XC_PRINTF("[%s,%5d] [DS_IP]pstXC_DS_CmdCnt->u16CMDCNT_IPM:%d u32MemShift_IPM:%lx \n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM,(MS_U32)u32MemShift_IPM);
            //u32Addr = u32Addr + DS_CMD_LEN_64BITS * pstXC_DS_CmdCnt->u16CMDCNT_IPM;
            u32Addr_IPM = u32Addr_IPM + u32MemShift_IPM;
            pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID, u32Addr_IPM);//IPM

            if(pstXC_DS_CmdCnt->u16CMDCNT_IPM < u16MaxCmdCnt)
            {
                *pu64Data = u64CmdTemp;
                pstXC_DS_CmdCnt->u16CMDCNT_IPM++;
            }
            else
            {
                printk("[%s,%5d] [DS] IPM Command count overflow !!  u16CMDCNT_IPM:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM, u16MaxCmdCnt);
            }
        }
        else//subwindow
        {
            MS_U32 u32MemShift_IPS;
            u32MemShift_IPS = KHal_XC_GetMemOffset(pstXC_DS_CmdCnt->u16CMDCNT_IPS);

            u32Addr_IPS = u32Addr_IPS + u32MemShift_IPS;
            pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID, u32Addr_IPS); // IPS

            if(pstXC_DS_CmdCnt->u16CMDCNT_IPS < u16MaxCmdCnt)
            {
                *pu64Data = u64CmdTemp;
                pstXC_DS_CmdCnt->u16CMDCNT_IPS++;
            }
            else
            {
                printk("[%s,%5d] [DS] IPS Command count overflow !!  u16CMDCNT_IPS:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPS, u16MaxCmdCnt);
            }
        }
    }
    else// DS_OP
    {
        if(eWindow == MAIN_WINDOW)
        {
            MS_U32 u32MemShift_OPM;

            u32MemShift_OPM = KHal_XC_GetMemOffset(pstXC_DS_CmdCnt->u16CMDCNT_OPM);
            //XC_PRINTF("[%s,%5d] [DS_OP]pstXC_DS_CmdCnt->u16CMDCNT_OPM:%d u32MemShift_OPM:%lx \n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_OPM,(MS_U32)u32MemShift_OPM);
            //u32Addr_OPM = u32Addr_OPM + DS_CMD_LEN_64BITS * pstXC_DS_CmdCnt->u16CMDCNT_OPM;
            u32Addr_OPM = u32Addr_OPM + u32MemShift_OPM;
            pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID, u32Addr_OPM);//OPM

            //XC_PRINTF("[%s,%5d] u64CmdTemp:%llx  \n",__func__,__LINE__,u64CmdTemp);

            if(pstXC_DS_CmdCnt->u16CMDCNT_OPM < u16MaxCmdCnt)
            {
                *pu64Data = u64CmdTemp;
                pstXC_DS_CmdCnt->u16CMDCNT_OPM++;
            }
            else
            {
                printk("[%s,%5d] [DS] OPM Command count overflow !!  u16CMDCNT_OPM:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_OPM, u16MaxCmdCnt);
            }

        }
        else
        {
            MS_U32 u32MemShift_OPS;

            u32MemShift_OPS = KHal_XC_GetMemOffset(pstXC_DS_CmdCnt->u16CMDCNT_OPS);

            u32Addr_OPS = u32Addr_OPS + u32MemShift_OPS;
            pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID, u32Addr_OPS); // OPS

            if(pstXC_DS_CmdCnt->u16CMDCNT_OPS < u16MaxCmdCnt)
            {
                *pu64Data = u64CmdTemp;
                pstXC_DS_CmdCnt->u16CMDCNT_OPS++;
            }
            else
            {
                printk("[%s,%5d] [DS] OPS Command count overflow !!  u16CMDCNT_OPS:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_OPS, u16MaxCmdCnt);
            }
        }
    }

}

void KHal_SC_WriteSWDSCommand_Mask(EN_KDRV_SC_DEVICE u32DeviceID,EN_KDRV_WINDOW eWindow,E_DS_CLIENT client, MS_U32 u32CmdRegAddr, MS_U16 u16CmdRegValue,k_ds_reg_ip_op_sel IPOP_Sel,k_ds_reg_source_sel Source_Select,K_XC_DS_CMDCNT *pstXC_DS_CmdCnt,MS_U8 u8DSIndex,MS_U16 u16Mask)
{

    MS_U32 offset = 0;
    MS_U8 depth;

#if SUPPORT_DS_MULTI_USERS
    ds_client_data client_data;
    if(ll_get_data(client,&client_data))
    {
        offset += client_data.offset;
    }
#endif
#if ENABLE_DS_4_BASEADDR_MODE
    MS_PHY u32Addr = client_ds[u32DeviceID].u32DynamicScalingBaseAddr + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth+offset) * MS_MLOAD_MEM_BASE_UNIT;
    MS_PHY u32Addr_OPM = client_ds[u32DeviceID].u32DynamicScalingBaseAddr_OPM + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth+offset) * MS_MLOAD_MEM_BASE_UNIT;
    MS_PHY u32Addr_IPS = client_ds[u32DeviceID].u32DynamicScalingBaseAddr_IPS + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth+offset) * MS_MLOAD_MEM_BASE_UNIT;
    MS_PHY u32Addr_OPS = client_ds[u32DeviceID].u32DynamicScalingBaseAddr_OPS + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth+offset) * MS_MLOAD_MEM_BASE_UNIT;
#else
    MS_PHY u32Addr = client_ds[u32DeviceID].u32DynamicScalingBaseAddr + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth+offset) * MS_MLOAD_MEM_BASE_UNIT;
#endif

    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] u32DynamicScalingBaseAddr:%x  CurIdx:%d  IdxDepth:%d  BYTE_PER_WORD:%d  \n",__func__,__LINE__,(int)client_ds[u32DeviceID].u32DynamicScalingBaseAddr,(int)client_ds[u32DeviceID].u8DynamicScalingCurrentIndex[eWindow],(int)client_ds[u32DeviceID].u8DynamicScalingIndexDepth,(int)BYTE_PER_WORD);
    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] u32Addr:%x  \n",__func__,__LINE__,(int)u32Addr);

    //printk("\033[1;31m[%s:%d] u32CmdRegAddr:%x u16CmdRegValue:%x   u16Mask:%x  \033[m\n",__FUNCTION__,__LINE__,u32CmdRegAddr,u16CmdRegValue,u16Mask);

#if ENABLE_DS_4_BASEADDR_MODE

    MS_U64 *pu64Data = NULL;
    MS_U64 u64CmdTemp = 0;
    MS_U8  u8AddrTemp = 0;
    MS_U16 u16BankTemp = 0;
    MS_U16 u16DataTemp = 0;
    MS_U16 u16MaskTemp = 0;
    //MS_U16 u16MaxCmdCnt = client_ds[u32DeviceID].u8DynamicScalingIndexDepth * (Hal_XC_MLoad_Get_64Bits_MIU_Bus_Sel(pInstance) + 1);

    depth=client_ds[u32DeviceID].u8DynamicScalingIndexDepth;
#if SUPPORT_DS_MULTI_USERS
    if(ll_get_data(client,&client_data))
    {
        depth = client_data.max_depth_num;
    }
#endif
    MS_U16 u16MaxCmdCnt = depth * ((SC_R2BYTEMSK(u32DeviceID,XC_ADDR_L(0x1F,0x13), 0xC000) >>14) + 1);
    //MS_U16 u16MaxCmdCnt = client_ds[u32DeviceID].u8DynamicScalingIndexDepth * ((SC_R2BYTEMSK(u32DeviceID,XC_ADDR_L(0x1F,0x13), 0xC000) >>14) + 1);

//===========================================================//
//  64bit command format (spread mode)
//  |-------------|-------------|----------------------------|-------------|
//  |    Bit-En   |    Unused   |    Addr(FULL RIU ADDR)     |     Data    |
//  |-------------|-------------|----------------------------|-------------|
//  |   [63:48]   |   [47:40]   |           [39:16]          |   [15:0]    |
//  |-------------|-------------|----------------------------|-------------|

//Addr: FULL RIU ADDRESS !!!

//For example:  Subbank:0x01            addr:0x40 Data:0x0001
//              Spread mode :  0x1301   addr:0x40 Data:0x0001

//|----------------------------------------------------|
//|                      Bank             |    addr    |
//|----------------------------------------------------|
//|                     0x1301            |    0x40    |
//|----------------------------------------------------|
// FULL RIU ADDRESS: |0001 0011 0000 0001 |  100 0000  |
// FULL RIU ADDRESS:              0x0980C1
//===========================================================//

    if(Source_Select == K_DS_XC)
    {
        //u16MaskTemp = 0xFFFF;
        //u16DataTemp = (SC_R2BYTE(u32DeviceID,(0x130000 | u32CmdRegAddr)) & ~0xFFFF) | (u16CmdRegValue & 0xFFFF);        //marcos,have to do sth here

        u8AddrTemp= (u32CmdRegAddr & 0x000000FF) >> 1;
        u16BankTemp= 0x1300 | ((u32CmdRegAddr >> 8) & 0x000000FF); // 0x13XX xc sread mode address

        //u64CmdTemp|= (MS_U64)u16DataTemp;
        u64CmdTemp|= (MS_U64)u16CmdRegValue;
        u64CmdTemp|= ((MS_U64)u8AddrTemp<<16);
        u64CmdTemp|= ((MS_U64)u16BankTemp<<23);
        //u64CmdTemp|= ((MS_U64)u16MaskTemp<<48);
        u64CmdTemp|= ((MS_U64)(~u16Mask)<<48);

        if(IPOP_Sel == K_DS_IP)
        {
            if(eWindow == E_KDRV_MAIN_WINDOW)
            {
                MS_U32 u32MemShift_IPM;
                u32MemShift_IPM = KHal_XC_GetMemOffset(pstXC_DS_CmdCnt->u16CMDCNT_IPM);

                u32Addr = u32Addr + u32MemShift_IPM;
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr);//IPM

                if(pstXC_DS_CmdCnt->u16CMDCNT_IPM < u16MaxCmdCnt)
                {
                    //printk("\033[1;31m[%s:%d] u32Addr:%x u64CmdTemp:%llx \033[m\n",__FUNCTION__,__LINE__,u32Addr,u64CmdTemp);
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_IPM++;
                }
                else
                {
                    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] IPM Command count overflow !!  u16CMDCNT_IPM:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM,client_ds[u32DeviceID].u8DynamicScalingIndexDepth);
                }
            }
            else//subwindow
            {
                u32Addr_IPS = u32Addr_IPS + DS_CMD_LEN_64BITS * pstXC_DS_CmdCnt->u16CMDCNT_IPS;
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr_IPS); // IPS

                if(pstXC_DS_CmdCnt->u16CMDCNT_IPS < u16MaxCmdCnt)
                {
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_IPS++;
                }
                else
                {
                    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] IPS Command count overflow !!  u16CMDCNT_IPS:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPS,client_ds[u32DeviceID].u8DynamicScalingIndexDepth);
                }
            }
        }
        else//DS_OP
        {
            if(eWindow == E_KDRV_MAIN_WINDOW)
            {
                MS_U32 u32MemShift_OPM;
                u32MemShift_OPM = KHal_XC_GetMemOffset(pstXC_DS_CmdCnt->u16CMDCNT_OPM);

                u32Addr_OPM = u32Addr_OPM + u32MemShift_OPM;
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr_OPM);//OPM

                if(pstXC_DS_CmdCnt->u16CMDCNT_OPM < u16MaxCmdCnt)
                {
                    //printk("\033[1;31m[%s:%d] u32Addr:%x u64CmdTemp:%llx \033[m\n",__FUNCTION__,__LINE__,u32Addr,u64CmdTemp);
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_OPM++;
                }
                else
                {
                    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] OPM Command count overflow !!  u16CMDCNT_OPM:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_OPM,client_ds[u32DeviceID].u8DynamicScalingIndexDepth);
                }
            }
            else
            {
                u32Addr_OPS = u32Addr_OPS + DS_CMD_LEN_64BITS * pstXC_DS_CmdCnt->u16CMDCNT_OPS;
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr_OPS); // OPS

                if(pstXC_DS_CmdCnt->u16CMDCNT_OPS < u16MaxCmdCnt)
                {
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_OPS++;
                }
                else
                {
                    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] OPS Command count overflow !!  u16CMDCNT_OPS:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_OPS,client_ds[u32DeviceID].u8DynamicScalingIndexDepth);
                }
            }
        }
    }
    else if(Source_Select == K_DS_MVOP)// only in IP cmd
    {
        if(IPOP_Sel == K_DS_IP)
        {
            //u16DataTemp = u16CmdRegValue;
            u8AddrTemp = (u32CmdRegAddr&0x000000FF);
            u16BankTemp = (u32CmdRegAddr&0x00FFFF00) >> 8; //EX: mvop mainwinodw: 0x1014 mvop bank
            // u16MaskTemp = 0xFFFF;

            //u64CmdTemp|= (MS_U64)u16DataTemp;
            u64CmdTemp|= (MS_U64)u16CmdRegValue;
            u64CmdTemp|= ((MS_U64)u8AddrTemp<<16) >> 1;
            u64CmdTemp|= ((MS_U64)u16BankTemp<<23);
            //u64CmdTemp|= ((MS_U64)u16MaskTemp<<48);
            u64CmdTemp|= ((MS_U64)~u16Mask<<48);

            if(eWindow == E_KDRV_MAIN_WINDOW)
            {
                MS_U32 u32MemShift_IPM;
                u32MemShift_IPM = KHal_XC_GetMemOffset(pstXC_DS_CmdCnt->u16CMDCNT_IPM);

                u32Addr = u32Addr + u32MemShift_IPM;
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr);//IPM

                if(pstXC_DS_CmdCnt->u16CMDCNT_IPM < u16MaxCmdCnt)
                {
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_IPM++;
                }
                else
                {
                    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] Command count overflow !!  u16CMDCNT_IPM:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM,client_ds[u32DeviceID].u8DynamicScalingIndexDepth);
                }
            }
            else
            {
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr_IPS);//IPS

                if(pstXC_DS_CmdCnt->u16CMDCNT_IPS < u16MaxCmdCnt)
                {
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_IPS++;
                }
                else
                {
                    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] Command count overflow !!  u16CMDCNT_IPS:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPS,client_ds[u32DeviceID].u8DynamicScalingIndexDepth);
                }
            }
        }
        else
        {
            //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] MVOP is IP cmd not OP cmd !! \n",__func__,__LINE__);
        }

    }
    else
    {
        //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] Only can support XC/MVOP/GOP NOW !! \n",__func__,__LINE__);
    }
#endif
}

MS_BOOL KHal_XC_InitDynamicScalingIndex(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow)
{
    mutex_lock(&_ds_mutex);
    client_ds[u32DeviceID].au8DSCurrentIndex[eWindow] = 0;
    client_ds[u32DeviceID].au8DSFireIndex[eWindow] = 0;

    mutex_unlock(&_ds_mutex);

    return TRUE;
}

MS_BOOL KHal_XC_GetDynamicScalingCurrentIndex(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, MS_U8* pu8DSIndex)
{
    mutex_lock(&_ds_mutex);

    *pu8DSIndex = client_ds[u32DeviceID].au8DSCurrentIndex[eWindow];

    mutex_unlock(&_ds_mutex);

    return TRUE;
}

MS_BOOL KHal_XC_GetDynamicScalingFireIndex(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, MS_U8* pu8DSIndex)
{
    mutex_lock(&_ds_mutex);

    *pu8DSIndex = client_ds[u32DeviceID].au8DSFireIndex[eWindow];
    mutex_unlock(&_ds_mutex);

    return TRUE;
}

MS_BOOL KHal_XC_FireDynamicScalingIndex(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow)
{
    mutex_lock(&_ds_mutex);
    //printk("\033[1;35m###[River][%s][%d]### %d\033[0m\n",__FUNCTION__,__LINE__,client_ds[u32DeviceID].au8DSCurrentIndex[eWindow]);

    client_ds[u32DeviceID].au8DSFireIndex[eWindow] = client_ds[u32DeviceID].au8DSCurrentIndex[eWindow];
    client_ds[u32DeviceID].au8DSCurrentIndex[eWindow]++;
    client_ds[u32DeviceID].au8DSCurrentIndex[eWindow] %= DS_BUFFER_NUM_EX;

    mutex_unlock(&_ds_mutex);

    return TRUE;
}

void KHal_SC_Add_NullCommand(EN_KDRV_SC_DEVICE u32DeviceID,EN_KDRV_WINDOW eWindow,E_DS_CLIENT client,k_ds_reg_ip_op_sel IPOP_Sel,K_XC_DS_CMDCNT *pstXC_DS_CmdCnt,MS_U8 u8DSIndex)
{
    MS_U8 depth;
    depth=client_ds[u32DeviceID].u8DynamicScalingIndexDepth;
#if SUPPORT_DS_MULTI_USERS
    ds_client_data client_data;
    if(ll_get_data(client,&client_data))
    {
        depth = client_data.max_depth_num;
    }
#endif
    MS_U16 u16MaxCmdCnt = depth * ((SC_R2BYTEMSK(u32DeviceID,XC_ADDR_L(0x1F,0x13), 0xC000) >>14) + 1); // It has four 64bit commands per MIU bus width
    //MS_U16 u16MaxCmdCnt = client_ds[u32DeviceID].u8DynamicScalingIndexDepth * ((SC_R2BYTEMSK(u32DeviceID,XC_ADDR_L(0x1F,0x13), 0xC000) >>14) + 1); // It has four 64bit commands per MIU bus width
    MS_U16 i = 0;

    if(eWindow == MAIN_WINDOW)
    {
        i = (IPOP_Sel == K_DS_IP) ? pstXC_DS_CmdCnt->u16CMDCNT_IPM : pstXC_DS_CmdCnt->u16CMDCNT_OPM;
    }
    else
    {
        i = (IPOP_Sel == K_DS_IP) ? pstXC_DS_CmdCnt->u16CMDCNT_IPS : pstXC_DS_CmdCnt->u16CMDCNT_OPS;
    }

    while(i < u16MaxCmdCnt)
    {
        i++;
        //Dummy cmd BKFF_7F=0x0000
        KHal_SC_WriteSWDSCommand(u32DeviceID, eWindow, client,(((MS_U16)(0xFF) << 8) | (MS_U16)((0x7F)*2)), 0x0000, IPOP_Sel, K_DS_XC, pstXC_DS_CmdCnt,u8DSIndex);
    }
}

MS_BOOL KHAL_SC_Set_DynamicScaling(EN_KDRV_SC_DEVICE u32DeviceID,MS_PHY u32MemBaseAddr, MS_U8 u8MIU_Select, MS_U8 u8IdxDepth, MS_BOOL bOP_On, MS_BOOL bIPS_On, MS_BOOL bIPM_On, MS_U32 u32DSBufferSize, MS_BOOL bEnable_ForceP, EN_KDRV_WINDOW eWindow)
{
    MS_U16 u16OnOff = 0;
    MS_U16 u16MIU_Sel_bit1 = 0;

    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] u8MIU_Select:%d  \n",__func__,__LINE__,u8MIU_Select);
    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] bOP_On:%d  bIPS_On:%d bIPM_On:%d  \n",__func__,__LINE__,bOP_On,bIPS_On,bIPM_On);
//*****************init value*******************
    ds_private[u32DeviceID].su16OldValue = 0;
    ds_private[u32DeviceID].su16OldValue_BWD_Status = 0;
    ds_private[u32DeviceID].sbOldValueReaded = FALSE;
    ds_private[u32DeviceID].bTrig_pStored = FALSE;
//**********************************************
    // disable DS before modifying DS configuration
    //if (MDrv_XC_MLoad_GetStatus(pInstance) == E_MLOAD_ENABLED)
    //{
    //    _MLOAD_ENTRY(pInstance);
    //    MDrv_XC_MLoad_WriteCmd(pInstance, REG_SC_BK1F_10_L, 0, 0x0E00);
    //    MDrv_XC_MLoad_Fire(pInstance, TRUE);
    //    _MLOAD_RETURN(pInstance);
    //}
    //else
    //{
    SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x10), 0, 0x0E00);
    //}

    // enable/disable
    // we only memorize riu enable bit because mload and ds shares the same riu enable bit
    u16OnOff = SC_R2BYTE(u32DeviceID, XC_ADDR_L(0x1F,0x10)) & 0x1000;

    u16OnOff |= ((bOP_On << 9) | (bIPS_On << 10) | (bIPM_On << 11));

#if ENABLE_DS_4_BASEADDR_MODE

//===============================================================//
//  old 32bit command in DRAM  (per 16 Byte)
//  |-------------|-------------|-------------|-------------|
//  | DS_OP(main) | DS_IP(main) |  DS_OP(sub) |  DS_IP(sub) |
//  |-------------|-------------|-------------|-------------|
//  |   4 byte    |   4 byte    |   4 byte    |   4 byte    |
//  |-------------|-------------|-------------|-------------|

//  new 64bit command in DRAM (per 16 Byte)
//  |---------------------------|---------------------------|
//  |        DS_IP(main)        |        DS_IP(main)        |
//  |---------------------------|---------------------------|
//  |          8 byte           |          8 byte           |
//  |---------------------------|---------------------------|
//===============================================================//

    //pXCResourcePrivate->sthal_SC.u32DynamicScalingBufSize = BYTE_PER_WORD * u8IdxDepth * DS_MAX_INDEX;

    //Index depth is decided by utopia
     //client_ds[u32DeviceID].u8DynamicScalingIndexDepth = 255;
    if(u8IdxDepth == 0xFF)
    {
        client_ds[u32DeviceID].u8DynamicScalingIndexDepth = 1024/2;
        client_ds[u32DeviceID].u32DynamicScalingBufSize = BYTE_PER_WORD * client_ds[u32DeviceID].u8DynamicScalingIndexDepth * 6 * 2;
    }
    else
    {
        client_ds[u32DeviceID].u8DynamicScalingIndexDepth = u8IdxDepth / 2 / (MS_MLOAD_MEM_BASE_UNIT/MS_MLOAD_BUS_WIDTH);
        //For compatiable with FW DS(IP to OP command offset 0xC00)
        //BufSize Need * 2 for Main sub window
        client_ds[u32DeviceID].u32DynamicScalingBufSize = BYTE_PER_WORD * client_ds[u32DeviceID].u8DynamicScalingIndexDepth * 6 * 2 * 2;
    }

     //client_ds[u32DeviceID].u32DynamicScalingBufSize = 0x30000; //For pureSN case, DS DRAM size is 0x1800
     printk("[%s,%5d] [DS] %d 0x%lx 0x%lx\n",__func__,__LINE__,u8IdxDepth,client_ds[u32DeviceID].u8DynamicScalingIndexDepth,client_ds[u32DeviceID].u32DynamicScalingBufSize);

     //client_ds[u32DeviceID].u32DynamicScalingBufSize = 0x30000; //For pureSN case, DS DRAM size is 0x1800

    //***********************IOREMAP************************//

    client_ds[u32DeviceID].va_phy = _DS_PA2VA(u32MemBaseAddr,client_ds[u32DeviceID].u32DynamicScalingBufSize);
    //*******************************************************

    MS_U32 u32DSBufLen = client_ds[u32DeviceID].u32DynamicScalingBufSize / 4;

    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] u32DynamicScalingBufSize:%d u32DSBufLen:%d  \n",__func__,__LINE__,client_ds[u32DeviceID].u32DynamicScalingBufSize,u32DSBufLen);

    if(eWindow == E_KDRV_MAIN_WINDOW)
    {
        client_ds[u32DeviceID].u32DynamicScalingBaseAddr = u32MemBaseAddr; //IPM
        client_ds[u32DeviceID].u32DynamicScalingBaseAddr_OPM = u32MemBaseAddr + u32DSBufLen*2;
        //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] u32DynamicScalingBaseAddr_IPM:%x  u32DynamicScalingBaseAddr_OPM:%x  \n",__func__,__LINE__,(int)client_ds[u32DeviceID].u32DynamicScalingBaseAddr,(int)client_ds[u32DeviceID].u32DynamicScalingBaseAddr_OPM);
    }

    if(eWindow == E_KDRV_SUB_WINDOW)
    {
        client_ds[u32DeviceID].u32DynamicScalingBaseAddr_IPS = u32MemBaseAddr + u32DSBufLen;
        client_ds[u32DeviceID].u32DynamicScalingBaseAddr_OPS = u32MemBaseAddr + u32DSBufLen*3;
        //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] u32DynamicScalingBaseAddr_IPS:%x  u32DynamicScalingBaseAddr_OPS:%x  \n",__func__,__LINE__,(int)client_ds[u32DeviceID].u32DynamicScalingBaseAddr_OPM,(int)client_ds[u32DeviceID].u32DynamicScalingBaseAddr_OPS);
    }
    KHal_XC_Enable_DS_4_Baseaddress_Mode(u32DeviceID, TRUE);
#else
    client_ds[u32DeviceID].u32DynamicScalingBaseAddr = u32MemBaseAddr;
    client_ds[u32DeviceID].u8DynamicScalingIndexDepth = u8IdxDepth; //MaxDepth(IP/OP)
    client_ds[u32DeviceID].u32DynamicScalingBufSize = BYTE_PER_WORD * u8IdxDepth * DS_MAX_INDEX;
#endif

    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] u32DynamicScalingBaseAddr:%x  u8DynamicScalingIndexDepth:%d u32DynamicScalingBufSize:%d   \n",__func__,__LINE__,(int)client_ds[u32DeviceID].u32DynamicScalingBaseAddr,(int)client_ds[u32DeviceID].u8DynamicScalingIndexDepth,client_ds[u32DeviceID].u32DynamicScalingBufSize);

    //Befor DS On, Clean the DS memory
    if(u16OnOff & 0x0E00)
    {
//        if( (pXCResourcePrivate->stdrvXC_3D.ePreInputMode == E_XC_3D_INPUT_MODE_NONE) && (pXCResourcePrivate->stdrvXC_3D.ePreOutputMode == E_XC_3D_OUTPUT_MODE_NONE)
//        && (pXCResourcePrivate->stdrvXC_3D._eInput3DMode[E_KDRV_MAIN_WINDOW] == E_XC_3D_INPUT_MODE_NONE) && (pXCResourcePrivate->stdrvXC_3D._eOutput3DMode == E_XC_3D_OUTPUT_MODE_NONE) ) //mantis:0740226
//        {
        //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] u8IdxDepth:%d DS_MAX_INDEX:%d BYTE_PER_WORD:%d  \n",__func__,__LINE__,u8IdxDepth,DS_MAX_INDEX,BYTE_PER_WORD);

        MS_U8 u8index = 0;

        for(u8index=0; u8index < DS_BUFFER_NUM_EX; u8index++)
        {
            KHal_SC_ResetSWDSCommand(u32DeviceID,eWindow, u8index);
        }

        //}
    }

    if(u16OnOff & 0x0E00)
    {
        // ENABLE DS
        u16OnOff |= BIT(12);     // enable write register through RIU
        client_ds[u32DeviceID].bDynamicScalingEnable = TRUE;

        // currently, these 3 will use same MIU
        u16OnOff |= ((u8MIU_Select & 0x01) << 13);                       // MIU select of OP
        u16OnOff |= ((u8MIU_Select & 0x01) << 14);                       // MIU select of IPS
        u16OnOff |= ((u8MIU_Select & 0x01) << 15);                       // MIU select of IPM

        u16MIU_Sel_bit1 |= ((u8MIU_Select & 0x02) << 12);                // MIU select of OP
        u16MIU_Sel_bit1 |= ((u8MIU_Select & 0x02) << 13);                // MIU select of IPS
        u16MIU_Sel_bit1 |= ((u8MIU_Select & 0x02) << 14);                // MIU select of IPM

        // patch IP1F2_21[15:14] r/w bank

        if(ds_private[u32DeviceID].sbOldValueReaded == FALSE)
        {
            ds_private[u32DeviceID].su16OldValue = SC_R2BYTE(u32DeviceID, XC_ADDR_L(0x01,0x21));
            ds_private[u32DeviceID].su16OldValue_BWD_Status = SC_R2BYTEMSK(u32DeviceID, XC_ADDR_L(0x46,0x01),0x000C);

            ds_private[u32DeviceID].sbOldValueReaded = TRUE;
        }
        //SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x01,0x21), 0x4000, 0xC000);
        //SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x46,0x01), 0x0, 0x000C); //Disable BWD [2]:BWDcrtl_memconfig  [3]:BWDcrtl_baseaddr

        //Hal_XC_MLoad_set_opm_lock(pInstance, OPM_LOCK_DS);
        SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x20,0x19), (((MS_U16)OPM_LOCK_DS)& 0x0003)<<8, 0x0300);
    }
    else
    {
        client_ds[u32DeviceID].bDynamicScalingEnable = FALSE;
        // DISABLE DS
        if(ds_private[u32DeviceID].sbOldValueReaded)
        {
            //SC_W2BYTE(u32DeviceID, XC_ADDR_L(0x01,0x21), ds_private[u32DeviceID].su16OldValue);
            //SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x46,0x01), ds_private[u32DeviceID].su16OldValue_BWD_Status, 0x000C);

            ds_private[u32DeviceID].sbOldValueReaded = FALSE;
        }
        //Hal_XC_MLoad_set_opm_lock(pInstance, OPM_LOCK_INIT_STATE);
        SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x20,0x19), (((MS_U16)OPM_LOCK_INIT_STATE)& 0x0003)<<8, 0x0300);
    }

    // if disabled, need to set dynamic scaling ratio to 1:1
    if(!bOP_On)
    {
        //SC_W4BYTE(u32DeviceID, XC_ADDR_L(0x23,0x1C), 0x00100000);
        //SC_W4BYTE(u32DeviceID, XC_ADDR_L(0x23,0x1E), 0x00100000);
        SC_W2BYTE(u32DeviceID, XC_ADDR_L(0x23,0x1C), 0x0000);
        SC_W2BYTE(u32DeviceID, XC_ADDR_L(0x23,0x1D), 0x0010);
        SC_W2BYTE(u32DeviceID, XC_ADDR_L(0x23,0x1E), 0x0000);
        SC_W2BYTE(u32DeviceID, XC_ADDR_L(0x23,0x1F), 0x0010);
    }

    if(u16OnOff & 0x0E00)
    {
        //if (FALSE == ds_private[u32DeviceID].bTrig_pStored)
        {
            KHal_XC_Set_DS_BaseAddress(u32DeviceID,u32MemBaseAddr);
            KHal_XC_Set_DS_IndexDepth(u32DeviceID);
            KHal_XC_Set_DS_MIU_Sel(u32DeviceID,u16OnOff,u16MIU_Sel_bit1);

            SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x12,0x06), BIT(7), BIT(7));//enable ipm tune after DS when DS on

            // set DMA threthold, length, fixed value, no need to change
            SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x10), 0x88, 0x00FF);

            // store
            //Hal_XC_MLoad_get_trig_p(pInstance, &pXCResourcePrivate->sthal_SC.u16OldTrain, &pXCResourcePrivate->sthal_SC.u16OldDisp);
            //_mload_private.u16OldValue[E_K_STORE_VALUE_AUTO_TUNE_AREA_TRIG] = SC_R2BYTEMSK(u32DeviceID, XC_ADDR_L(0x20,0x1A), 0x0FFF);
            //_mload_private.u16OldValue[E_K_STORE_VALUE_DISP_AREA_TRIG] = SC_R2BYTEMSK(u32DeviceID, XC_ADDR_L(0x20,0x1B), 0x0FFF);
            //ds_private[u32DeviceID].bTrig_pStored = TRUE;

            // set the trigger point from delayed line,  DS: 0x08, Train:0x14, Disp:0x18
            //SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x20,0x21), 0x08, 0xFF);  // ds_trig_dly
            //Hal_XC_MLoad_set_trig_p(pInstance, 0x14, 0x18);
            //SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x20,0x1A), 0x14, 0x0FFF);
            //SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x20,0x1B), 0x18,  0x0FFF);
            //Hal_XC_MLoad_set_trig_p(pInstance, 0x0A, 0x18);

            //add
            //SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x10,0x5D), 0x0828, 0xFFFF);

            // set trigger source to trigger
            //SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x20,0x19), 0x08B8, 0x08BC);

            //set the delay line cnt to vfde end
            //SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x20,0x19), BIT(6), BIT(6)|BIT(0));

            if (IS_SUPPORT_64BITS_COMMAND(ENABLE_64BITS_COMMAND, u32DeviceID))
            {
                KHal_XC_Enable_DS_64Bits_Command(u32DeviceID,TRUE);
                if(ENABLE_64BITS_SPREAD_MODE)
                {
                    KHal_XC_Enable_DS_64Bits_Sread_Mode(u32DeviceID,TRUE);
                }
                KHal_XC_Set_DS_64Bits_MIU_Bus_Sel(u32DeviceID);
            }
        }

    }
    else
    {
        // restore
        if (0)//ds_private[u32DeviceID].bTrig_pStored)
        {
            ds_private[u32DeviceID].bTrig_pStored = FALSE;
            //Hal_XC_MLoad_set_trig_p(pInstance, pXCResourcePrivate->sthal_SC.u16OldTrain, pXCResourcePrivate->sthal_SC.u16OldDisp);
            SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x20,0x1A), _mload_private.u16OldValue[E_K_STORE_VALUE_AUTO_TUNE_AREA_TRIG], 0x0FFF);
            SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x20,0x1B), _mload_private.u16OldValue[E_K_STORE_VALUE_DISP_AREA_TRIG],  0x0FFF);

            //MDrv_XC_MLoad_set_IP_trig_p(pInstance, 0x03, 0x05); // reset to HW default setting
#if (HW_DESIGN_4K2K_VER == 4)
            SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x57,0x1A), 0x03, 0x0FFF);
            SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x57,0x1B), 0x05,  0x0FFF);
#endif


            SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x12,0x06), 0x0, BIT(7));//disable ipm tune after DS when DS off

            //set the delay line cnt to V sync starXC_ADDR_L(0x
            SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x20,0x19), 0, BIT(6)|BIT(0));
        }
    }

    // set DS in IPS[10],ipm[11] On/Off
    //if (MDrv_XC_MLoad_GetStatus(pInstance) == E_MLOAD_ENABLED)
    //{
    //    _MLOAD_ENTRY(pInstance);
    //    MDrv_XC_MLoad_WriteCmd(pInstance, XC_ADDR_L(0x1F,0x10), u16OnOff & 0x1D00, 0x1D00);
    //    MDrv_XC_MLoad_Fire(pInstance, TRUE);
    //    _MLOAD_RETURN(pInstance);
    //}
    //else
    //{
    SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x10), u16OnOff & 0x1D00, 0x1D00);
    //}

    //Only IP DS in FBL/RFBL case
    //if(!((gSrcInfo[MAIN_WINDOW].bR_FBL) || (gSrcInfo[MAIN_WINDOW].bFBL)))
    //{
    // set DS in OP[9] On/Off
    //if (MDrv_XC_MLoad_GetStatus(pInstance) == E_MLOAD_ENABLED)
    //{
    //    _MLOAD_ENTRY(pInstance);
    //    MDrv_XC_MLoad_WriteCmd(pInstance, XC_ADDR_L(0x1F,0x10), u16OnOff & 0x0200, 0x0200);
    //    MDrv_XC_MLoad_Fire(pInstance, TRUE);
    //    _MLOAD_RETURN(pInstance);
    //}
    //else
    //{
    SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x10), u16OnOff & 0x0200, 0x0200);
    //}
    //}

    return TRUE;
}

void KHAL_SC_Set_DynamicScalingFlag(EN_KDRV_SC_DEVICE u32DeviceID, MS_BOOL bEnable)
{
    client_ds[u32DeviceID].bDynamicScalingEnable = bEnable;
}

MS_BOOL KHAL_SC_Get_DynamicScaling_Status(EN_KDRV_SC_DEVICE u32DeviceID)
{
    return client_ds[u32DeviceID].bDynamicScalingEnable;
}

MS_BOOL KHAL_SC_Enable_IPMTuneAfterDS(EN_KDRV_SC_DEVICE u32DeviceID, MS_BOOL bEnable)
{
    //Null function for Monet , ipm mask(IPM tune after DS) is controlled by first IP command

    // ENABLE IPM TUNE AFTER DS
    //SC_W2BYTEMSK(psXCInstPri->u32DeviceID, REG_SC_BK12_06_L, BIT(7), BIT(7));

    //reg_ds_ipm_active_sel: 0:HW 1:SW
    //SC_W2BYTEMSK(psXCInstPri->u32DeviceID, REG_SC_BK1F_17_L, BIT(10), BIT(10));

    // Clear DS active pulse every vsync
    //SC_W2BYTEMSK(psXCInstPri->u32DeviceID, REG_SC_BK1F_17_L, BIT(6), BIT(6));
    return TRUE;
}

void KHal_SC_DynamicScaling_SWReset(void)      //to do: is sub required this function as well
{
    SC_W2BYTEMSK(0, XC_ADDR_L(0x1F,0x13), 0x400, 0x400);
    SC_W2BYTEMSK(0, XC_ADDR_L(0x1F,0x13), 0x800, 0x800);
    SC_W2BYTEMSK(0, XC_ADDR_L(0x1F,0x13), 0x1000, 0x1000);

    SC_W2BYTEMSK(0, XC_ADDR_L(0x1F,0x13), 0x0, 0x1000);
    SC_W2BYTEMSK(0, XC_ADDR_L(0x1F,0x13), 0x0, 0x800);
    SC_W2BYTEMSK(0, XC_ADDR_L(0x1F,0x13), 0x0, 0x400);
}
MS_BOOL KHal_XC_Get_DSForceIndexSupported(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow)
{
    if (eWindow >= E_KDRV_MAX_WINDOW)
    {
        printk("[%s,%5d] maximum window exceeded",__FUNCTION__,__LINE__);
        return FALSE;
    }

    return TRUE;
}

void KHal_XC_Set_DSForceIndex(EN_KDRV_SC_DEVICE u32DeviceID, MS_BOOL bEnable, MS_U8 u8Index, EN_KDRV_WINDOW eWindow)
{
    if (eWindow >= E_KDRV_MAX_WINDOW)
    {
        printk("[%s,%5d] maximum window exceeded",__FUNCTION__,__LINE__);
        return;
    }

    if (eWindow == E_KDRV_MAIN_WINDOW)
    {
        // reg_idxg_force_en
        SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x77), bEnable?BIT(0):0 , BIT(0));

        // reg_idxg_force_idx_f2
        SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x76), (MS_U16)u8Index << 8 , 0xFF00);
    }
    else // eWindow == SUB_WINDOW
    {
        SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x77), bEnable?BIT(8):0 , BIT(8));

        // reg_idxg_force_idx_f1
        SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x75), (MS_U16)u8Index << 8 , 0xFF00);
    }
}

void KHal_XC_Set_DSIndexSourceSelect(EN_KDRV_SC_DEVICE u32DeviceID, E_K_XC_DS_INDEX_SOURCE eDSIdxSrc, EN_KDRV_WINDOW eWindow)
{
    if (eWindow >= E_KDRV_MAX_WINDOW)
    {
        printk("[%s,%5d] maximum window exceeded",__FUNCTION__,__LINE__);
        return;
    }

    if (eDSIdxSrc >= E_K_XC_DS_INDEX_MAX)
    {
        printk("[%s,%5d] Maximum index exceeded, the index is (%d)\n",__FUNCTION__,__LINE__,eDSIdxSrc);
        return;
    }

    if(eWindow == E_KDRV_MAIN_WINDOW)
    {
        // reg_idxg_en_f2
        SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x00,0x37), eDSIdxSrc, BIT(0));
    }
    else
    {
        // reg_idxg_en_f1
        SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x00,0x37), eDSIdxSrc << 8, BIT(8));
    }
}

void KHal_XC_Enable_DS_64Bits_Command(EN_KDRV_SC_DEVICE u32DeviceID,MS_BOOL bEn)
{
    SC_W2BYTEMSK(u32DeviceID,XC_ADDR_L(0x1F,0x70), bEn?BIT(0):0x00, BIT(0));
}

void KHal_XC_Enable_DS_64Bits_Sread_Mode(EN_KDRV_SC_DEVICE u32DeviceID,MS_BOOL bEn)
{
    SC_W2BYTEMSK(u32DeviceID,XC_ADDR_L(0x1F,0x70), bEn?BIT(15):0x00, BIT(15));
}

void KHal_XC_Enable_DS_4_Baseaddress_Mode(EN_KDRV_SC_DEVICE u32DeviceID,MS_BOOL bEn)
{
    SC_W2BYTEMSK(u32DeviceID,XC_ADDR_L(0x1F,0x17), bEn?BIT(7):0x00, BIT(7));
}

void KHal_XC_Set_DS_64Bits_MIU_Bus_Sel(EN_KDRV_SC_DEVICE u32DeviceID)
{
    MS_U16 u16sel = 0x00;

    if( MS_MLOAD_BUS_WIDTH == 8 )
    {
        u16sel = 0x00;
    }
    else if( MS_MLOAD_BUS_WIDTH == 16 )
    {
        u16sel = 0x01;
    }
    else if( MS_MLOAD_BUS_WIDTH == 32 )
    {
        u16sel = 0x3;
    }
    else
    {
        printk("MIU Bus not support !!!!!!!!!!!!!!!!!\n");
        u16sel = 0x00;
    }

    u16sel = (u16sel & 0x0003)<<14;
    SC_W2BYTEMSK(u32DeviceID,XC_ADDR_L(0x1F,0x13), u16sel, 0xC000);
}

void KHal_XC_Set_DS_BaseAddress(EN_KDRV_SC_DEVICE u32DeviceID,MS_PHY u32Base)
{

    if( u32Base > ARM_MIU1_BASE_ADDR)
    {
        u32Base = u32Base - ARM_MIU1_BASE_ADDR;
    }

    MS_U32 u32Base_OPM = 0;
    u32Base_OPM = client_ds[u32DeviceID].u32DynamicScalingBaseAddr_OPM;

    if( u32Base_OPM  > ARM_MIU1_BASE_ADDR)
    {
        u32Base_OPM  = u32Base_OPM  - ARM_MIU1_BASE_ADDR;
    }

    u32Base = u32Base/MS_MLOAD_MEM_BASE_UNIT;

#if ENABLE_DS_4_BASEADDR_MODE
    // set IPM
    SC_W2BYTE(u32DeviceID, XC_ADDR_L(0x1F,0x11), u32Base & 0xFFFF);
    SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x12), (u32Base >> 16),0xFFFF);

    //set IPS
    SC_W2BYTE(u32DeviceID, XC_ADDR_L(0x1F,0x0C), (client_ds[u32DeviceID].u32DynamicScalingBaseAddr_IPS / MS_MLOAD_MEM_BASE_UNIT) & 0xFFFF);
    SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x0D), ((client_ds[u32DeviceID].u32DynamicScalingBaseAddr_IPS / MS_MLOAD_MEM_BASE_UNIT) >> 16),0xFFFF);

    //if(!((gSrcInfo[MAIN_WINDOW].bR_FBL) || (gSrcInfo[MAIN_WINDOW].bFBL)))
    //{
    //set OPM
    SC_W2BYTE(u32DeviceID, XC_ADDR_L(0x1F,0x0E), (u32Base_OPM / MS_MLOAD_MEM_BASE_UNIT) & 0xFFFF);
    SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x0F), ((u32Base_OPM / MS_MLOAD_MEM_BASE_UNIT) >> 16),0xFFFF);

    //set OPS
    SC_W2BYTE(u32DeviceID, XC_ADDR_L(0x1F,0x60), (client_ds[u32DeviceID].u32DynamicScalingBaseAddr_OPS / MS_MLOAD_MEM_BASE_UNIT) & 0xFFFF);
    SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x61), ((client_ds[u32DeviceID].u32DynamicScalingBaseAddr_OPS / MS_MLOAD_MEM_BASE_UNIT) >> 16),0xFFFF);
    //}
#else
    // set DS base address
    SC_W2BYTE(u32DeviceID, XC_ADDR_L(0x1F,0x11), u32Base & 0xFFFF);
    SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x12), (u32Base >> 16),0xFFFF);
#endif

}

void KHal_XC_Set_DS_IndexDepth(EN_KDRV_SC_DEVICE u32DeviceID)
{
    SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x13), client_ds[u32DeviceID].u8DynamicScalingIndexDepth & 0xFF , 0x00FF);
    SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x2E), ((client_ds[u32DeviceID].u8DynamicScalingIndexDepth >> 8) & 0xFF) , 0x00FF);
}

void KHal_XC_Set_DS_MIU_Sel(EN_KDRV_SC_DEVICE u32DeviceID,MS_U16 u16DSOnOff,MS_U16 u16DS_MIU_Sel_bit1)
{
    SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x10), u16DSOnOff & 0xE000, 0xE000);
    SC_W2BYTEMSK(u32DeviceID, XC_ADDR_L(0x1F,0x71), u16DS_MIU_Sel_bit1 & 0xE000, 0xE000);
}
MS_U64 KHal_SC_GenSpreadModeCmd(EN_KDRV_SC_DEVICE u32DeviceID, MS_U32 u32CmdRegAddr, MS_U16 u16CmdRegValue, MS_U16 u16Mask, k_ds_reg_source_sel eSourceSelect)
{
    MS_U64 u64CmdTemp = 0;
    MS_U8  u8AddrTemp = 0;
    MS_U16 u16BankTemp = 0;
    MS_U16 u16DataTemp = 0;

    if(eSourceSelect == K_DS_XC)
    {
        u64CmdTemp = KHal_XC_MLoad_Gen_64bits_spreadMode(u32DeviceID*CLIENT_NUM, u32CmdRegAddr, u16CmdRegValue, u16Mask);
    }
    else if(eSourceSelect == K_DS_MVOP)
    {
        u16DataTemp = u16CmdRegValue;
        u8AddrTemp = (u32CmdRegAddr&0x000000FF);
        u16BankTemp = (u32CmdRegAddr&0x00FFFF00) >> 8; //EX: mvop mainwinodw: 0x1014 mvop bank
        u64CmdTemp = KHal_XC_MLoad_Gen_64bits_spreadMode_NonXC(u16BankTemp, u8AddrTemp, u16DataTemp, u16Mask);
    }
    else if(eSourceSelect == K_DS_GOP)
    {
        //TO DO
    }
    else
    {
        //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] Only can support XC/MVOP/GOP NOW !! \n",__func__,__LINE__);
    }

    return u64CmdTemp;
}
MS_BOOL KHal_DS_set_client(EN_KDRV_SC_DEVICE u32DeviceID,E_DS_CLIENT client,MS_U32 max_depth_num)
{
    if((SC_R2BYTEMSK(u32DeviceID,XC_ADDR_L(0x1F,0x13), 0xC000) >>14)!= 0)
    {
        max_depth_num =( (max_depth_num+ (SC_R2BYTEMSK(u32DeviceID,XC_ADDR_L(0x1F,0x13), 0xC000) >>14) )/((SC_R2BYTEMSK(u32DeviceID,XC_ADDR_L(0x1F,0x13), 0xC000) >>14)+1) );
    }
    //********check max num********//
    MS_U32 offset = 0;
    ds_node *temp;
    temp =ds_client_list;

    if(ds_client_list!=NULL)
    {
        while(temp->next != NULL)
        {
            //printk("\033[1;32m[%s:%d][%d],max_depth_num=%d\033[m\n",__FUNCTION__,__LINE__,right->data.client,right->data.max_depth_num);
            offset += temp->data.max_depth_num;    //offset calculation
            temp=temp->next;
        }
        offset += temp->data.max_depth_num;    //offset calculation
    }
    offset += max_depth_num;
    if(client_ds[u32DeviceID].u8DynamicScalingIndexDepth < offset)
    {
        //printk("\033[1;34m[%s:%d]u8DynamicScalingIndexDepth=%d,offset=%d\033[m\n",__FUNCTION__,__LINE__,client_ds[u32DeviceID].u8DynamicScalingIndexDepth,offset);
        return FALSE;
    }
    //******************************//
    ds_client_data data;
    data.client = client;
    data.offset = 0;
    data.max_depth_num = max_depth_num;

    ll_add( data );
    return TRUE;
}

static MS_VIRT _PA2VA(MS_PHY phy, MS_U32 u32Len, MS_BOOL *pbIOUnmap)
{
    phys_addr_t tmp_phy = phy;
#if 0

    if(phy<ARM_MIU1_BASE_ADDR)
    {
        tmp_phy = phy + ARM_MIU0_BUS_BASE ;
    }
    else if((phy >= ARM_MIU1_BASE_ADDR) && (phy<ARM_MIU2_BASE_ADDR))
    {
        tmp_phy = phy - ARM_MIU1_BASE_ADDR + ARM_MIU1_BUS_BASE ;
    }
    else
    {
        tmp_phy = phy - ARM_MIU2_BASE_ADDR + ARM_MIU2_BUS_BASE ;
    }
#endif
    if (pfn_valid(__phys_to_pfn(tmp_phy)))
    {
        *pbIOUnmap = FALSE;
        return __va(tmp_phy);
    }
    else
    {
        *pbIOUnmap = TRUE;
        return ioremap(tmp_phy, u32Len);
    }
}

static MS_VIRT _GET_PA2VA(KHAL_DS_ADDR *pstDSAddr, MS_PHYADDR phyDstAddr)
{
    MS_PHYADDR tmp_diff=0;
    if((phyDstAddr >= pstDSAddr->phyDSAddr) && (phyDstAddr < (pstDSAddr->phyDSAddr + pstDSAddr->u32DSSize)))
    {
        tmp_diff = phyDstAddr - pstDSAddr->phyDSAddr;
        return (pstDSAddr->virtDSAddr + tmp_diff);
    }
    else
    {
        printf("%s: No PA 2 VA MAPPING!\n", __FUNCTION__);
        return 0;
    }
}

static MS_U32 _Get_StoredDSCmdBufferSize(MS_U32 u32CmdCnt)
{
    MS_U32 u32BufferSize = 0;

#if ENABLE_DS_4_BASEADDR_MODE
    if((MS_MLOAD_BUS_WIDTH == 16) && (BYTE_PER_WORD == 32))
    {
        if((u32CmdCnt % 2) == 0)
        {
            u32BufferSize = (u32CmdCnt / 2) * MS_MLOAD_MEM_BASE_UNIT;
        }
        else
        {
            u32BufferSize = (u32CmdCnt / 2) * MS_MLOAD_MEM_BASE_UNIT  + DS_CMD_LEN_64BITS;
        }
    }
    else // ((MS_MLOAD_BUS_WIDTH == 16) && (BYTE_PER_WORD == 16))  or ((MS_MLOAD_BUS_WIDTH == 32) && (BYTE_PER_WORD == 32))
    {
        u32BufferSize = u32CmdCnt * DS_CMD_LEN_64BITS;
    }
#else
    u32BufferSize = u32CmdCnt * MS_MLOAD_MEM_BASE_UNIT;
#endif

    return u32BufferSize;
}

MS_BOOL KHal_SC_Set_DS_StoredAddr(KHAL_DS_STORED_ADDR_INFO *pstDSStoredAddr)
{
    // IOummap the lastest virt addr
    if (_bIOUnmap[0] && (_stDSStoredAddrInfo.stIPMAddr.virtDSAddr != 0))
    {
        iounmap(_stDSStoredAddrInfo.stIPMAddr.virtDSAddr);
    }

    if (_bIOUnmap[1] && (_stDSStoredAddrInfo.stOPMAddr.virtDSAddr != 0))
    {
        iounmap(_stDSStoredAddrInfo.stOPMAddr.virtDSAddr);
    }

    if (_bIOUnmap[2] && (_stDSStoredAddrInfo.stIPSAddr.virtDSAddr != 0))
    {
        iounmap(_stDSStoredAddrInfo.stIPSAddr.virtDSAddr);
    }

    if (_bIOUnmap[3] && (_stDSStoredAddrInfo.stOPSAddr.virtDSAddr != 0))
    {
        iounmap(_stDSStoredAddrInfo.stOPSAddr.virtDSAddr);
    }

    //init new addr info
    memset(&_stDSStoredAddrInfo, 0, sizeof(KHAL_DS_STORED_ADDR_INFO));

    _stDSStoredAddrInfo.stIPMAddr.phyDSAddr  = pstDSStoredAddr->stIPMAddr.phyDSAddr;
    _stDSStoredAddrInfo.stIPMAddr.u32DSSize = pstDSStoredAddr->stIPMAddr.u32DSSize;
    if (_stDSStoredAddrInfo.stIPMAddr.phyDSAddr != 0)
    {
        _stDSStoredAddrInfo.stIPMAddr.virtDSAddr = _PA2VA(_stDSStoredAddrInfo.stIPMAddr.phyDSAddr, _stDSStoredAddrInfo.stIPMAddr.u32DSSize, &_bIOUnmap[0]);
    }

    _stDSStoredAddrInfo.stOPMAddr.phyDSAddr  = pstDSStoredAddr->stOPMAddr.phyDSAddr;
    _stDSStoredAddrInfo.stOPMAddr.u32DSSize = pstDSStoredAddr->stOPMAddr.u32DSSize;
    if (_stDSStoredAddrInfo.stIPMAddr.phyDSAddr != 0)
    {
        _stDSStoredAddrInfo.stOPMAddr.virtDSAddr = _PA2VA(_stDSStoredAddrInfo.stOPMAddr.phyDSAddr, _stDSStoredAddrInfo.stOPMAddr.u32DSSize, &_bIOUnmap[1]);
    }

    _stDSStoredAddrInfo.stIPSAddr.phyDSAddr  = pstDSStoredAddr->stIPSAddr.phyDSAddr;
    _stDSStoredAddrInfo.stIPSAddr.u32DSSize = pstDSStoredAddr->stIPSAddr.u32DSSize;
    if (_stDSStoredAddrInfo.stIPSAddr.phyDSAddr != 0)
    {
        _stDSStoredAddrInfo.stIPSAddr.virtDSAddr = _PA2VA(_stDSStoredAddrInfo.stIPSAddr.phyDSAddr, _stDSStoredAddrInfo.stIPSAddr.u32DSSize, &_bIOUnmap[2]);
    }

    _stDSStoredAddrInfo.stOPSAddr.phyDSAddr  = pstDSStoredAddr->stOPSAddr.phyDSAddr;
    _stDSStoredAddrInfo.stOPSAddr.u32DSSize = pstDSStoredAddr->stOPSAddr.u32DSSize;
    if (_stDSStoredAddrInfo.stOPSAddr.phyDSAddr != 0)
    {
        _stDSStoredAddrInfo.stOPSAddr.virtDSAddr = _PA2VA(_stDSStoredAddrInfo.stOPSAddr.phyDSAddr, _stDSStoredAddrInfo.stOPSAddr.u32DSSize, &_bIOUnmap[3]);
    }

    return TRUE;
}

static MS_BOOL KHal_XC_Parsing_64bits_SpreadMode_NonXC(MS_U64 u64Cmd, MS_U32 *u32Addr, MS_U16 *u16Data)
{
    MS_U8  u8AddrTemp = 0;
    MS_U16 u16BankTemp = 0;

    *u16Data = (MS_U16)(0xFFFF&(MS_U16)u64Cmd);
    u8AddrTemp= (MS_U8)((u64Cmd>>16 & 0x7F) << 1);
    u16BankTemp= (MS_U16)((u64Cmd >> 23) & 0xFFFF);
    *u32Addr = (MS_U32)(u8AddrTemp|u16BankTemp<<8);

    return TRUE;
}

static MS_U64 KHal_XC_Gen_64bits_SpreadMode_NonXC(MS_U32 u32Bank,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask)
{
    MS_U64 u64CmdTemp = 0;

    u64CmdTemp|= (MS_U64)u16Data;
    u64CmdTemp|= ((MS_U64) ((u32Addr<<16) >>1));
    u64CmdTemp|= ((MS_U64)u32Bank<<23);
    u64CmdTemp|= ((MS_U64)u16Mask<<48);

    return u64CmdTemp;
}

void KHal_XC_StoreSWDSCommand(EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, MS_U32 u32CmdRegAddr,
                              MS_U16 u16CmdRegValue, MS_U16 u16CmdRegMask, k_ds_reg_ip_op_sel IPOP_Sel, k_ds_reg_source_sel Source_Select, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt)
{

    MS_U32 offset = 0;
    MS_U8 depth;
    MS_U16 u16MaxCmdCnt = 0;

    MS_PHY u32Addr_IPM = _stDSStoredAddrInfo.stIPMAddr.phyDSAddr;
    MS_PHY u32Addr_OPM = _stDSStoredAddrInfo.stOPMAddr.phyDSAddr;
    MS_PHY u32Addr_IPS = _stDSStoredAddrInfo.stIPSAddr.phyDSAddr;
    MS_PHY u32Addr_OPS = _stDSStoredAddrInfo.stOPSAddr.phyDSAddr;

    //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[%s,%5d] [DS] u32Addr_IPM:%x  \n",__func__,__LINE__,(int)u32Addr_IPM);

#if ENABLE_64BITS_SPREAD_MODE

    MS_U64 *pu64Data = NULL;
    MS_U64 u64CmdTemp = 0;
    MS_U8  u8AddrTemp = 0;
    MS_U16 u16BankTemp = 0;
    MS_U16 u16DataTemp = 0;
    MS_U16 u16MaskTemp = 0;

    //===========================================================//
    //  64bit command format (spread mode)
    //  |-------------|-------------|----------------------------|-------------|
    //  |    Bit-En   |    Unused   |    Addr(FULL RIU ADDR)     |     Data    |
    //  |-------------|-------------|----------------------------|-------------|
    //  |   [63:48]   |   [47:40]   |           [39:16]          |   [15:0]    |
    //  |-------------|-------------|----------------------------|-------------|

    //Addr: FULL RIU ADDRESS !!!

    //For example:  Subbank:0x01            addr:0x40 Data:0x0001
    //              Spread mode :  0x1301   addr:0x40 Data:0x0001

    //|----------------------------------------------------|
    //|                      Bank             |    addr    |
    //|----------------------------------------------------|
    //|                     0x1301            |    0x40    |
    //|----------------------------------------------------|
    // FULL RIU ADDRESS: |0001 0011 0000 0001 |  100 0000  |
    // FULL RIU ADDRESS:              0x0980C1
    //===========================================================//

    if(Source_Select == K_DS_XC)
    {
        u16MaskTemp = 0xFFFF;
        u16DataTemp = (u16CmdRegValue & u16CmdRegMask);

        u8AddrTemp= (u32CmdRegAddr & 0x000000FF) >> 1;
        u16BankTemp= 0x1300 | ((u32CmdRegAddr >> 8) & 0x000000FF); // 0x13XX xc sread mode address

        u64CmdTemp|= (MS_U64)u16DataTemp;
        u64CmdTemp|= ((MS_U64)u8AddrTemp<<16);
        u64CmdTemp|= ((MS_U64)u16BankTemp<<23);
        u64CmdTemp|= ((MS_U64)u16MaskTemp<<48);

        if (u16CmdRegMask != 0xFFFF)
        {
            MS_U32 u32WriteMemShift = 0;
            MS_U16 u16CmdIndex = 0;
            MS_U16 u16CurCmd = 0;
            KHAL_DS_ADDR stKdrvDsAddr;

            if(IPOP_Sel == K_DS_IP)
            {
                if(eWindow == E_KDRV_MAIN_WINDOW)
                {
                    u16CurCmd = pstXC_DS_CmdCnt->u16CMDCNT_IPM;
                    stKdrvDsAddr = _stDSStoredAddrInfo.stIPMAddr;
                }
                else
                {
                    u16CurCmd = pstXC_DS_CmdCnt->u16CMDCNT_IPS;
                    stKdrvDsAddr = _stDSStoredAddrInfo.stIPSAddr;
                }
            }
            else
            {
                if(eWindow == E_KDRV_MAIN_WINDOW)
                {
                    u16CurCmd = pstXC_DS_CmdCnt->u16CMDCNT_OPM;
                    stKdrvDsAddr = _stDSStoredAddrInfo.stOPMAddr;
                }
                else
                {
                    u16CurCmd = pstXC_DS_CmdCnt->u16CMDCNT_OPS;
                    stKdrvDsAddr = _stDSStoredAddrInfo.stOPSAddr;
                }
            }

            for (; u16CmdIndex < u16CurCmd; u16CmdIndex++)
            {
                u32WriteMemShift = KHal_XC_GetMemOffset(u16CmdIndex);

                MS_PHYADDR phyDstAddr = u32Addr_IPM + u32WriteMemShift;
                MS_U64 *pu64Addr = (MS_U64 *)_GET_PA2VA(&stKdrvDsAddr, phyDstAddr);
                MS_U32 u32CurrentAddr = 0;
                MS_U16 u16CurrentData = 0;
                MS_U32 u32OldAddr = 0;
                MS_U16 u16OldData = 0;
                KHal_XC_Parsing_64bits_SpreadMode_NonXC(*pu64Addr, &u32OldAddr, &u16OldData);
                KHal_XC_Parsing_64bits_SpreadMode_NonXC(u64CmdTemp, &u32CurrentAddr, &u16CurrentData);
                if ((u32OldAddr == u32CurrentAddr)
                    &&((u32CurrentAddr & REG_SCALER_BASE) == REG_SCALER_BASE)) //only xc reg check
                {
                    if (u16OldData != u16CurrentData)
                    {
                        MS_U16 u16NewData = 0;
                        u16NewData = (u16OldData & ~u16CmdRegMask) | (u16CurrentData & u16CmdRegMask);
                        u64CmdTemp = KHal_XC_Gen_64bits_SpreadMode_NonXC((u32CurrentAddr>>8) & 0xFFFF, u32CurrentAddr & 0xFF, u16NewData, 0xFFFF);
                        *pu64Addr = u64CmdTemp;
                    }
                    return;
                }
            }
        }

        if(IPOP_Sel == K_DS_IP)
        {
            if(eWindow == E_KDRV_MAIN_WINDOW)
            {
                MS_U32 u32MemShift_IPM;

                u16MaxCmdCnt = _stDSStoredAddrInfo.stIPMAddr.u32DSSize / BYTE_PER_WORD * MS_MLOAD_MEM_BASE_UNIT / DS_CMD_LEN_64BITS;
                //printk("u16MaxCmdCnt %d, size: %d\n", u16MaxCmdCnt, _stDSStoredAddrInfo.stIPMAddr.u32DSSize);
                u32MemShift_IPM = KHal_XC_GetMemOffset(pstXC_DS_CmdCnt->u16CMDCNT_IPM);

                u32Addr_IPM = u32Addr_IPM + u32MemShift_IPM;
                //printk("[%s,%5d] data: 0x%llX, xu32MemShift_IPM:%lx \n",__func__,__LINE__, u64CmdTemp,(MS_U32)u32MemShift_IPM);
                pu64Data = (MS_U64*)_GET_PA2VA(&_stDSStoredAddrInfo.stIPMAddr, u32Addr_IPM);//IPM
                //printk("[%s,%5d] u16MaxCmdCnt: 0x%llX\n",__func__,__LINE__, u16MaxCmdCnt);

                if(pstXC_DS_CmdCnt->u16CMDCNT_IPM < u16MaxCmdCnt)
                {
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_IPM++;
                }
                else
                {
                    printk("[%s,%5d] [DS] IPM Command count overflow !!  u16CMDCNT_IPM:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM, u16MaxCmdCnt);
                }
            }
            else//subwindow
            {
                u16MaxCmdCnt = _stDSStoredAddrInfo.stIPSAddr.u32DSSize / BYTE_PER_WORD * MS_MLOAD_MEM_BASE_UNIT / DS_CMD_LEN_64BITS;

                u32Addr_IPS = u32Addr_IPS + DS_CMD_LEN_64BITS * pstXC_DS_CmdCnt->u16CMDCNT_IPS;
                pu64Data = (MS_U64*)_GET_PA2VA(&_stDSStoredAddrInfo.stIPSAddr, u32Addr_IPS); // IPS

                if(pstXC_DS_CmdCnt->u16CMDCNT_IPS < u16MaxCmdCnt)
                {
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_IPS++;
                }
                else
                {
                    printk("[%s,%5d] [DS] IPS Command count overflow !!  u16CMDCNT_IPS:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPS, u16MaxCmdCnt);
                }
            }
        }
        else//DS_OP
        {
            if(eWindow == E_KDRV_MAIN_WINDOW)
            {
                MS_U32 u32MemShift_OPM;
                u16MaxCmdCnt = _stDSStoredAddrInfo.stOPMAddr.u32DSSize / BYTE_PER_WORD * MS_MLOAD_MEM_BASE_UNIT / DS_CMD_LEN_64BITS;
                u32MemShift_OPM = KHal_XC_GetMemOffset(pstXC_DS_CmdCnt->u16CMDCNT_OPM);

                //XC_PRINTF("[%s,%5d] [DS_OP]pstXC_DS_CmdCnt->u16CMDCNT_OPM:%d u32MemShift_OPM:%lx \n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_OPM,(MS_U32)u32MemShift_OPM);
                u32Addr_OPM = u32Addr_OPM + u32MemShift_OPM;
                pu64Data = (MS_U64*)_GET_PA2VA(&_stDSStoredAddrInfo.stOPMAddr, u32Addr_OPM);//OPM

                if(pstXC_DS_CmdCnt->u16CMDCNT_OPM < u16MaxCmdCnt)
                {
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_OPM++;
                }
                else
                {
                    printk("[%s,%5d] [DS] OPM Command count overflow !!  u16CMDCNT_OPM:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_OPM, u16MaxCmdCnt);
                }
            }
            else
            {
                u16MaxCmdCnt = _stDSStoredAddrInfo.stOPSAddr.u32DSSize / BYTE_PER_WORD * MS_MLOAD_MEM_BASE_UNIT / DS_CMD_LEN_64BITS;
                u32Addr_OPS = u32Addr_OPS + DS_CMD_LEN_64BITS * pstXC_DS_CmdCnt->u16CMDCNT_OPS;
                pu64Data = (MS_U64*)_GET_PA2VA(&_stDSStoredAddrInfo.stOPSAddr, u32Addr_OPS); // OPS

                if(pstXC_DS_CmdCnt->u16CMDCNT_OPS < u16MaxCmdCnt)
                {
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_OPS++;
                }
                else
                {
                    printk("[%s,%5d] [DS] OPS Command count overflow !!  u16CMDCNT_OPS:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_OPS, u16MaxCmdCnt);
                }
            }
        }
    }
    else if(Source_Select == K_DS_MVOP)// only in IP cmd
    {
        if(IPOP_Sel == K_DS_IP)
        {
            u16DataTemp = u16CmdRegValue;
            u8AddrTemp = (u32CmdRegAddr&0x000000FF);
            u16BankTemp = (u32CmdRegAddr&0x00FFFF00) >> 8; //EX: mvop mainwinodw: 0x1014 mvop bank
            u16MaskTemp = 0xFFFF;

            u64CmdTemp|= (MS_U64)u16DataTemp;
            u64CmdTemp|= ((MS_U64)u8AddrTemp<<16) >> 1;
            u64CmdTemp|= ((MS_U64)u16BankTemp<<23);
            u64CmdTemp|= ((MS_U64)u16MaskTemp<<48);

            if(eWindow == E_KDRV_MAIN_WINDOW)
            {
                MS_U32 u32MemShift_IPM;
                u16MaxCmdCnt = _stDSStoredAddrInfo.stIPMAddr.u32DSSize / BYTE_PER_WORD * MS_MLOAD_MEM_BASE_UNIT / DS_CMD_LEN_64BITS;
                u32MemShift_IPM = KHal_XC_GetMemOffset(pstXC_DS_CmdCnt->u16CMDCNT_IPM);

                //XC_PRINTF("[%s,%5d] [DS_IP(MVOP)]pstXC_DS_CmdCnt->u16CMDCNT_IPM:%d u32MemShift_IPM:%lx \n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM,(MS_U32)u32MemShift_IPM);
                u32Addr_IPM = u32Addr_IPM + u32MemShift_IPM;
                pu64Data = (MS_U64*)_GET_PA2VA(&_stDSStoredAddrInfo.stIPMAddr, u32Addr_IPM);//IPM

                if(pstXC_DS_CmdCnt->u16CMDCNT_IPM < u16MaxCmdCnt)
                {
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_IPM++;
                }
                else
                {
                    printk("[%s,%5d] [DS] Command count overflow !!  u16CMDCNT_IPM:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM, u16MaxCmdCnt);
                }
            }
            else
            {
                u16MaxCmdCnt = _stDSStoredAddrInfo.stIPSAddr.u32DSSize / BYTE_PER_WORD * MS_MLOAD_MEM_BASE_UNIT / DS_CMD_LEN_64BITS;
                pu64Data = (MS_U64*)_GET_PA2VA(&_stDSStoredAddrInfo.stIPSAddr, u32Addr_IPS);//IPS

                if(pstXC_DS_CmdCnt->u16CMDCNT_IPS < u16MaxCmdCnt)
                {
                    *pu64Data = u64CmdTemp;
                    pstXC_DS_CmdCnt->u16CMDCNT_IPS++;
                }
                else
                {
                    printk("[%s,%5d] [DS] Command count overflow !!  u16CMDCNT_IPS:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPS, u16MaxCmdCnt);
                }
            }
        }
        else
        {
            printk("[%s,%5d] [DS] MVOP is IP cmd not OP cmd !! \n",__func__,__LINE__);
        }

    }
    else if(Source_Select == K_DS_GOP)
    {

    }
    else
    {
        printk("[%s,%5d] [DS] Only can support XC/MVOP/GOP NOW !! \n",__func__,__LINE__);
    }

#else

    //=============================================//
    //  32bit command format
    //  |-------------|-------------|-------------|
    //  |     Bank    |     Addr    |     Data    |
    //  |-------------|-------------|-------------|
    //  |   [31:24]   |   [23:16]   |    [15:0]   |
    //  |-------------|-------------|-------------|

    //Bank: Subbank
    //=============================================//

    if(IPOP_Sel == K_DS_OP)
    {
        MS_PHY u32OPAddr = _stDSStoredAddrInfo.stOPMAddr.phyDSAddr;
        MS_U32 *pu32Data = NULL;
        MS_U8 u8OPCount = _stDSStoredAddrInfo.stOPMAddr.u32DSSize / MS_MLOAD_MEM_BASE_UNIT;
        MS_PHY u32Command = 0;

        u32Command = u16CmdRegValue + ((u32CmdRegAddr & 0x00FF) << 15) + ((u32CmdRegAddr & 0xFF00) << 16);

        while(u8OPCount > 0)
        {
            pu32Data = (MS_U32*)_GET_PA2VA(&_stDSStoredAddrInfo.stOPMAddr, u32OPAddr);
            if(0 == pu32Data)
            {
                printf("%s :pu32Data = 0!,error!!!\n", __FUNCTION__);

                assert(pu32Data != 0);
            }
            // Found null command
            if((MS_U32)(*pu32Data) == 0xFFFF0000)
            {
                break;
            }
            // Found duplicated command
            if((MS_U32)(*pu32Data) == u32Command)
            {
                //SC_DBG(printf("Command duplicated\n"));

                return;
            }
            u32OPAddr += MS_MLOAD_MEM_BASE_UNIT;
            u8OPCount--;
        }

        if((MS_U32)(*pu32Data) == 0xFFFF0000)
        {
            *pu32Data = u32Command;
        }
        else
        {
            printf("DS OP command buffer is full for this index!\n");
        }
        //printf("@@[%s][%u]u16CmdRegAddr=%x, u16CmdRegValue=%x\n",__FUNCTION__, __LINE__, u16CmdRegAddr, u16CmdRegValue);
    }

    else
    {
        // (OP : IP : Unused : Unused) (4 byte : 4 byte : 4 byte : 4 byte) if MIU 128bit
        // (OPmain : IPmain : OPsub : IPsub) (4 byte : 4 byte : 4 byte : 4 byte) if MIU 128bit

        MS_PHY u32IPAddr = _stDSStoredAddrInfo.stIPMAddr.phyDSAddr + DS_OP_CMD_LEN;
        MS_U32 *pu32Data = NULL;
        MS_U8 u8IPCount = _stDSStoredAddrInfo.stIPMAddr.u32DSSize / MS_MLOAD_MEM_BASE_UNIT;
        //MS_U8 u8IPCount = IdxDepth;
        MS_PHY u32Command = 0;

        u32Command = u16CmdRegValue + ((u32CmdRegAddr & 0x00FF) << 15) + ((u32CmdRegAddr & 0xFF00) << 16);

        while(u8IPCount > 0)
        {
            pu32Data = (MS_U32*)_GET_PA2VA(&_stDSStoredAddrInfo.stIPMAddr, u32IPAddr);
            if(0 == pu32Data)
            {
                printf("%s :pu32Data = 0!,error!!!\n", __FUNCTION__);

                assert(pu32Data != 0);
            }
            // Found null command
            if((MS_U32)(*pu32Data) == 0xFFFF0000)
            {
                break;
            }
            // Found duplicated command
            if((MS_U32)(*pu32Data) == u32Command)
            {
                return;
            }
            u32IPAddr += MS_MLOAD_MEM_BASE_UNIT;
            u8IPCount--;
        }

        if((MS_U32)(*pu32Data) == 0xFFFF0000)
        {
            *pu32Data = u32Command;
        }
        else
        {
            printf("DS IP command buffer is full for this index!\n");
        }
        //printf("@@[%s][%u]u16CmdRegAddr=%x, u16CmdRegValue=%x\n",__FUNCTION__, __LINE__, u16CmdRegAddr, u16CmdRegValue);
    }
#endif
}

#define DBGDS0(x) //x
void KHal_XC_WriteStoredSWDSCommand(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, MS_U8 *pu8CmdData,
                                    MS_U32 u32CmdCnt, k_ds_reg_ip_op_sel IPOP_Sel, k_ds_reg_source_sel Source_Select, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt, MS_U8 u8DSIndex)
{

    MS_U32 offset = 0;
    MS_U32 offsettowrite = 0;
    MS_U16 depth;

#if SUPPORT_DS_MULTI_USERS
    ds_client_data client_data;
    if(ll_get_data(client,&client_data))
    {
        offset += client_data.offset;
    }
#endif
    offsettowrite = offset * MS_MLOAD_MEM_BASE_UNIT;
    MS_U32 u32BufferSize = KHal_XC_GetMemOffset(u32CmdCnt);
    offset = 0;

    DBGDS0(printk("[%s,%5d] [DS] u32CmdCnt:%ld u32BufferSize:0x%lx ,0x%lx \n",__func__,__LINE__,u32CmdCnt,u32BufferSize,offset));
    //printk("[%s,%5d] [DS] phyStoredCmdAddr:0x%lx \n",__func__,__LINE__,phyStoredCmdAddr);

    //MS_BOOL bIOUnmap = FALSE;
    //MS_U8 *pu8CmdData = (MS_U8 *)_PA2VA(phyStoredCmdAddr, u32BufferSize, &bIOUnmap);

#if ENABLE_DS_4_BASEADDR_MODE
    MS_PHY u32Addr = client_ds[u32DeviceID].u32DynamicScalingBaseAddr + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth+offset) * MS_MLOAD_MEM_BASE_UNIT;
    MS_PHY u32Addr_OPM = client_ds[u32DeviceID].u32DynamicScalingBaseAddr_OPM + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth+offset) * MS_MLOAD_MEM_BASE_UNIT;
    MS_PHY u32Addr_IPS = client_ds[u32DeviceID].u32DynamicScalingBaseAddr_IPS + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth+offset) * MS_MLOAD_MEM_BASE_UNIT;
    MS_PHY u32Addr_OPS = client_ds[u32DeviceID].u32DynamicScalingBaseAddr_OPS + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth+offset) * MS_MLOAD_MEM_BASE_UNIT;
#else
    MS_PHY u32Addr = client_ds[u32DeviceID].u32DynamicScalingBaseAddr + (u8DSIndex * client_ds[u32DeviceID].u8DynamicScalingIndexDepth+offset) * MS_MLOAD_MEM_BASE_UNIT;
#endif

    DBGDS0(printk("[%s,%5d] [DS] u32DynamicScalingBaseAddr:%x  CurIdx:%d  IdxDepth:%d  BYTE_PER_WORD:%d  \n",__func__,__LINE__,(int)client_ds[u32DeviceID].u32DynamicScalingBaseAddr,u8DSIndex,(int)client_ds[u32DeviceID].u8DynamicScalingIndexDepth,(int)BYTE_PER_WORD));
    DBGDS0(printk("[%s,%5d] [DS] u32Addr:%x  \n",__func__,__LINE__,(int)u32Addr));

#if ENABLE_DS_4_BASEADDR_MODE

    MS_U64 *pu64Data = NULL;
    MS_U64 u64CmdTemp = 0;
    MS_U8  u8AddrTemp = 0;
    MS_U16 u16BankTemp = 0;
    MS_U16 u16DataTemp = 0;
    MS_U16 u16MaskTemp = 0;
    //MS_U16 u16MaxCmdCnt = client_ds[u32DeviceID].u8DynamicScalingIndexDepth * (Hal_XC_MLoad_Get_64Bits_MIU_Bus_Sel(pInstance) + 1);

    depth=client_ds[u32DeviceID].u8DynamicScalingIndexDepth;
    DBGDS0(printk("[%s,%5d] [DS] depth:%d \n",__func__,__LINE__,depth));
#if SUPPORT_DS_MULTI_USERS
    if(ll_get_data(client,&client_data))
    {
        depth = client_data.max_depth_num;
    }
#endif

    MS_U16 u16MaxCmdCnt = depth * ((SC_R2BYTEMSK(u32DeviceID,XC_ADDR_L(0x1F,0x13), 0xC000) >>14) + 1);
    //MS_U16 u16MaxCmdCnt = client_ds[u32DeviceID].u8DynamicScalingIndexDepth * ((SC_R2BYTEMSK(u32DeviceID,XC_ADDR_L(0x1F,0x13), 0xC000) >>14) + 1);
    DBGDS0(printk("[%s,%5d] [DS] depth:%d  u16MaxCmdCnt:%d\n",__func__,__LINE__,depth,u16MaxCmdCnt));

    //===========================================================//
    //  64bit command format (spread mode)
    //  |-------------|-------------|----------------------------|-------------|
    //  |    Bit-En   |    Unused   |    Addr(FULL RIU ADDR)     |     Data    |
    //  |-------------|-------------|----------------------------|-------------|
    //  |   [63:48]   |   [47:40]   |           [39:16]          |   [15:0]    |
    //  |-------------|-------------|----------------------------|-------------|

    //Addr: FULL RIU ADDRESS !!!

    //For example:  Subbank:0x01            addr:0x40 Data:0x0001
    //              Spread mode :  0x1301   addr:0x40 Data:0x0001

    //|----------------------------------------------------|
    //|                      Bank             |    addr    |
    //|----------------------------------------------------|
    //|                     0x1301            |    0x40    |
    //|----------------------------------------------------|
    // FULL RIU ADDRESS: |0001 0011 0000 0001 |  100 0000  |
    // FULL RIU ADDRESS:              0x0980C1
    //===========================================================//

    if(Source_Select == K_DS_XC)
    {
        if(IPOP_Sel == K_DS_IP)
        {
            if(eWindow == E_KDRV_MAIN_WINDOW)
            {
                MS_U32 u32MemShift_IPM;
                u32MemShift_IPM = KHal_XC_GetMemOffset(pstXC_DS_CmdCnt->u16CMDCNT_IPM);
                DBGDS0(printk("[%s,%5d] [DS] 0x%x \n",__func__,__LINE__,u32Addr));

                //XC_PRINTF("[%s,%5d] [DS_IP]pstXC_DS_CmdCnt->u16CMDCNT_IPM:%d u32MemShift_IPM:%lx \n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM,(MS_U32)u32MemShift_IPM);
                //printk("[%s,%5d] [DS] %d 0x%lx\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM,u32MemShift_IPM);
                u32Addr = u32Addr + u32MemShift_IPM;
                u32Addr = u32Addr + offsettowrite;
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID, u32Addr);//IPM
                //printk("mrmcpy: 0x%lx, 0x%lx, %d\n", u32Addr, phyStoredCmdAddr, u32BufferSize);
                DBGDS0(printk("mrmcpy: 0x%lx, 0x%lx, %d\n", pu64Data, pu8CmdData, u32BufferSize));
                //printk("[%s,%5d] [DS] %d 0x%lx\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM,u32MemShift_IPM);
                if(pstXC_DS_CmdCnt->u16CMDCNT_IPM < u16MaxCmdCnt)
                {
                    DBGDS0(printk("[%s,%5d] [DS] 0x%x \n",__func__,__LINE__,(int)u32Addr));
                    memcpy(pu64Data, pu8CmdData, u32BufferSize);
                    pstXC_DS_CmdCnt->u16CMDCNT_IPM += u32CmdCnt;
                }
                else
                {
                    printk("[%s,%5d] [DS] IPM Command count overflow !!  u16CMDCNT_IPM:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM, u16MaxCmdCnt);
                }
            }
            else//subwindow
            {
                u32Addr_IPS = u32Addr_IPS + DS_CMD_LEN_64BITS * pstXC_DS_CmdCnt->u16CMDCNT_IPS;
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr_IPS); // IPS

                if(pstXC_DS_CmdCnt->u16CMDCNT_IPS < u16MaxCmdCnt)
                {
                    memcpy(pu64Data, pu8CmdData, u32BufferSize);
                    pstXC_DS_CmdCnt->u16CMDCNT_IPS += u32CmdCnt;
                }
                else
                {
                    printk("[%s,%5d] [DS] IPS Command count overflow !!  u16CMDCNT_IPS:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPS, u16MaxCmdCnt);
                }
            }
        }
        else//DS_OP
        {
            if(eWindow == E_KDRV_MAIN_WINDOW)
            {
                MS_U32 u32MemShift_OPM;
                u32MemShift_OPM = KHal_XC_GetMemOffset(pstXC_DS_CmdCnt->u16CMDCNT_OPM);

                //XC_PRINTF("[%s,%5d] [DS_OP]pstXC_DS_CmdCnt->u16CMDCNT_OPM:%d u32MemShift_OPM:%lx \n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_OPM,(MS_U32)u32MemShift_OPM);
                //u32Addr_OPM = u32Addr_OPM + DS_CMD_LEN_64BITS * pstXC_DS_CmdCnt->u16CMDCNT_OPM;
                u32Addr_OPM = u32Addr_OPM + u32MemShift_OPM;
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr_OPM);//OPM

                //XC_PRINTF("[%s,%5d] u64CmdTemp:%llx  \n",__func__,__LINE__,u64CmdTemp);

                if(pstXC_DS_CmdCnt->u16CMDCNT_OPM < u16MaxCmdCnt)
                {
                    memcpy(pu64Data, pu8CmdData, u32BufferSize);
                    pstXC_DS_CmdCnt->u16CMDCNT_OPM += u32CmdCnt;
                }
                else
                {
                    printk("[%s,%5d] [DS] OPM Command count overflow !!  u16CMDCNT_OPM:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_OPM, u16MaxCmdCnt);
                }
            }
            else
            {
                u32Addr_OPS = u32Addr_OPS + DS_CMD_LEN_64BITS * pstXC_DS_CmdCnt->u16CMDCNT_OPS;
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr_OPS); // OPS

                if(pstXC_DS_CmdCnt->u16CMDCNT_OPS < u16MaxCmdCnt)
                {
                    memcpy(pu64Data, pu8CmdData, u32BufferSize);
                    pstXC_DS_CmdCnt->u16CMDCNT_OPS += u32CmdCnt;
                }
                else
                {
                    printk("[%s,%5d] [DS] OPS Command count overflow !!  u16CMDCNT_OPS:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_OPS, u16MaxCmdCnt);
                }
            }
        }
    }
    else if(Source_Select == K_DS_MVOP)// only in IP cmd
    {
        if(IPOP_Sel == K_DS_IP)
        {

            if(eWindow == E_KDRV_MAIN_WINDOW)
            {
                MS_U32 u32MemShift_IPM;
                u32MemShift_IPM = KHal_XC_GetMemOffset(pstXC_DS_CmdCnt->u16CMDCNT_IPM);

                //XC_PRINTF("[%s,%5d] [DS_IP(MVOP)]pstXC_DS_CmdCnt->u16CMDCNT_IPM:%d u32MemShift_IPM:%lx \n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM,(MS_U32)u32MemShift_IPM);
                //u32Addr = u32Addr + DS_CMD_LEN_64BITS * pstXC_DS_CmdCnt->u16CMDCNT_IPM;
                u32Addr = u32Addr + u32MemShift_IPM;
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr);//IPM

                if(pstXC_DS_CmdCnt->u16CMDCNT_IPM < u16MaxCmdCnt)
                {
                    memcpy(pu64Data, pu8CmdData, u32BufferSize);
                    pstXC_DS_CmdCnt->u16CMDCNT_IPM += u32CmdCnt;
                }
                else
                {
                    printk("[%s,%5d] [DS] Command count overflow !!  u16CMDCNT_IPM:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPM, u16MaxCmdCnt);
                }
            }
            else
            {
                pu64Data = (MS_U64*)_DSGET_PA2VA(u32DeviceID,u32Addr_IPS);//IPS

                if(pstXC_DS_CmdCnt->u16CMDCNT_IPS < u16MaxCmdCnt)
                {
                    memcpy(pu64Data, pu8CmdData, u32BufferSize);
                    pstXC_DS_CmdCnt->u16CMDCNT_IPS += u32CmdCnt;
                }
                else
                {
                    printk("[%s,%5d] [DS] Command count overflow !!  u16CMDCNT_IPS:%d  Max command count: %d\n",__func__,__LINE__,pstXC_DS_CmdCnt->u16CMDCNT_IPS, u16MaxCmdCnt);
                }
            }
        }
        else
        {
            printk("[%s,%5d] [DS] MVOP is IP cmd not OP cmd !! \n",__func__,__LINE__);
        }

    }
    else if(Source_Select == K_DS_GOP)
    {

    }
    else
    {
        printk("[%s,%5d] [DS] Only can support XC/MVOP/GOP NOW !! \n",__func__,__LINE__);
    }

#else

//=============================================//
//  32bit command format
//  |-------------|-------------|-------------|
//  |     Bank    |     Addr    |     Data    |
//  |-------------|-------------|-------------|
//  |   [31:24]   |   [23:16]   |    [15:0]   |
//  |-------------|-------------|-------------|

//Bank: Subbank
//=============================================//

    if(IPOP_Sel == K_DS_OP)
    {
        MS_PHY u32OPAddr = u32Addr + pstXC_DS_CmdCnt->u16CMDCNT_IPM * MS_MLOAD_MEM_BASE_UNIT;
        MS_U32 *pu32Data = (MS_U32*)_DSGET_PA2VA(u32DeviceID, u32OPAddr);

        memcpy(pu32Data, pu8CmdData, u32BufferSize);
        //printf("@@[%s][%u]u16CmdRegAddr=%x, u16CmdRegValue=%x\n",__FUNCTION__, __LINE__, u16CmdRegAddr, u16CmdRegValue);
    }

    else
    {
        // (OP : IP : Unused : Unused) (4 byte : 4 byte : 4 byte : 4 byte) if MIU 128bit
        // (OPmain : IPmain : OPsub : IPsub) (4 byte : 4 byte : 4 byte : 4 byte) if MIU 128bit

        MS_PHY u32IPAddr = u32Addr + pstXC_DS_CmdCnt->u16CMDCNT_IPM * MS_MLOAD_MEM_BASE_UNIT + DS_OP_CMD_LEN;
        MS_U32 *pu32Data = (MS_U32*)_DSGET_PA2VA(u32DeviceID, u32IPAddr);

        memcpy(pu32Data, pu8CmdData, u32BufferSize);
        //printf("@@[%s][%u]u16CmdRegAddr=%x, u16CmdRegValue=%x\n",__FUNCTION__, __LINE__, u16CmdRegAddr, u16CmdRegValue);
    }

    if (bIOUnmap == TRUE)
    {
        iounmap(pu8CmdData);
    }
#endif
}

#endif // MHAL_DYNAMICSCALING_C

