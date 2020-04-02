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
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/poll.h>
#include <linux/wait.h>
#if defined(CONFIG_MIPS)
#elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
#include <asm/io.h>
#endif
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/kthread.h>

#include "chip_int.h"
#include "mdrv_mstypes.h"
#include "mhal_xc_chip_config.h"
#include "mdrv_xc_st.h"
#include "mhal_menuload.h"
#include "mhal_xc.h"
#include "mstar/mstar_chip.h"

MS_U32 _XC_Device_Offset[]={0,128};//MAX_XC_DEVICE_NUM

MS_BOOL KHal_XC_MLoad_GetCaps(void)
{
    return TRUE;
}

MS_U16 KHal_XC_MLoad_get_status(EN_MLOAD_CLIENT_TYPE _client_type)
{
    return ((SC_R2BYTE(_client_type, XC_ADDR_L(0x1F,0x02)) & 0x8000)>>15);
}

void KHal_XC_MLoad_set_on_off(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bEn)
{
    if(bEn)
        SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x02), 0x8000, 0x8000);
    else
        SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x02), 0x0000, 0x8000);
}

void KHal_XC_MLoad_set_len(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 u16Len)
{
    u16Len &= 0x7FF;
    SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x02), u16Len, 0x7FF);
}

void KHal_XC_MLoad_set_depth(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 u16depth)
{
    SC_W2BYTE(_client_type, XC_ADDR_L(0x1F,0x01), u16depth);
}

void KHal_XC_MLoad_set_miusel(EN_MLOAD_CLIENT_TYPE _client_type,MS_U8 u8MIUSel)
{
    if (u8MIUSel == 0)
    {
      SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x1E), 0x0000, 0x0003);
    }
    else if (u8MIUSel == 1)
    {
        SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x1E), 0x0001, 0x0003);
    }
}

void KHal_XC_MLoad_set_base_addr(EN_MLOAD_CLIENT_TYPE _client_type,MS_PHY u32addr)
{
    u32addr /= MS_MLOAD_MEM_BASE_UNIT;

    SC_W2BYTE(_client_type, XC_ADDR_L(0x1F,0x03), (MS_U16)(u32addr & 0xFFFF));
    SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x04), (MS_U16)((u32addr & 0x3FF0000)>>16), 0x003FF);
}

void KHal_XC_MLoad_Set_riu(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bEn)
{
    if (bEn)
    {
        SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x10), 0x1000, 0x1000);
    }
    else
    {
        SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x10), 0x0000, 0x1000);
    }
}

void KHal_XC_MLoad_set_trigger_timing(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 u16sel)
{
    u16sel = (u16sel & 0x0003)<<12;
    SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x20,0x19), u16sel, 0x3000);
}

void KHal_XC_MLoad_set_opm_lock(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 u16sel)
{
    u16sel = (u16sel & 0x0003)<<8;
    SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x20,0x19), u16sel, 0x0300);
}

void KHal_XC_MLoad_set_trigger_delay(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 u16delay)
{
    //SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x20,0x22), u16delay, 0x0FFF);
}

//___|T_________________........__|T____ VSync
//__________|T__________________         ATP(refer the size befor memory to cal the pip sub and main length)
//_________________|T___________         Disp

//Generate TRAIN_TRIG_P from delayed line of Vsync(Setting the delay line for Auto tune area)
//Generate DISP_TRIG_P from delayed line of Vsync(Setting the delay line for Display area)
void KHal_XC_MLoad_set_trig_p(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 u16train, MS_U16 u16disp)
{
    //SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x20,0x1A), u16train, 0x0FFF);
    //SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x20,0x1B), u16disp,  0x0FFF);
}

//Get the delay line for Auto tune area
//Get the delay line for Display area
MS_BOOL KHal_XC_MLoad_get_trig_p(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 *pu16Train, MS_U16 *pu16Disp)
{
    *pu16Train = SC_R2BYTEMSK(_client_type, XC_ADDR_L(0x20,0x1A), 0x0FFF);
    *pu16Disp = SC_R2BYTEMSK(_client_type, XC_ADDR_L(0x20,0x1B), 0x0FFF);
    return TRUE;
}

void KHal_XC_MLoad_set_riu_cs(MS_BOOL bEn)
{
    if(bEn)
    {
        MDrv_WriteByteMask(0x100104, 0x10, 0x10);
    }
    else
    {
        MDrv_WriteByteMask(0x100104, 0x00, 0x10);
    }
}


void KHal_XC_MLoad_set_sw_dynamic_idx_en(MS_BOOL ben)
{
    ben = ben;
}

void KHal_XC_MLoad_set_miu_bus_sel(EN_MLOAD_CLIENT_TYPE _client_type,MS_U8 u8BitMode)
{
    SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x13), (u8BitMode << 14), 0xC000); //00: 64bit, 01:128bit, 11:256bit, ML/DS is use 0x00, DS seperate mode is use 0x01
}

void KHal_XC_MLoad_enable_watch_dog(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bEn)
{
    if(bEn)
    {
        SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x05), 0xC000, 0xF000);
    }
    else
    {
        SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x05), 0x0000, 0xF000);
    }
}

void KHal_XC_MLoad_enable_watch_dog_reset(EN_MLOAD_CLIENT_TYPE _client_type,MLoad_WD_Timer_Reset_Type enMLWDResetType)
{
    SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x06), (enMLWDResetType << 8), BIT(8)|BIT(9));
}

void KHal_XC_MLoad_set_watch_dog_time_delay(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 value)
{
    SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x05), value, 0x03FF);
}

void KHal_XC_MLoad_set_opm_arbiter_bypass(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL ben)
{
    if (ben)
    {
        SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x12,0x70), BIT(2), BIT(2));
    }
    else
    {
        SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x12,0x70), 0x00, BIT(2));
    }
}

void KHal_XC_MLoad_Enable_64BITS_COMMAND(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bEn)
{
    SC_W2BYTEMSK(_client_type,XC_ADDR_L(0x1F,0x70), bEn?BIT(0):0x00, BIT(0));
}

void KHal_XC_MLoad_Enable_64BITS_SPREAD_MODE(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bEn)
{
    SC_W2BYTEMSK(_client_type,XC_ADDR_L(0x1F,0x70), bEn?BIT(15):0x00, BIT(15));
}

void KHal_XC_MLoad_Set_64Bits_MIU_Bus_Sel(EN_MLOAD_CLIENT_TYPE _client_type)
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
        printf("MIU Bus not support !!!!!!!!!!!!!!!!!\n");
        u16sel = 0x00;
    }

    u16sel = (u16sel & 0x0003)<<14;
    SC_W2BYTEMSK(_client_type,XC_ADDR_L(0x1F,0x13), u16sel, 0xC000);
}

MS_U8 KHal_XC_MLoad_Get_64Bits_MIU_Bus_Sel(EN_MLOAD_CLIENT_TYPE _client_type)
{
    return (SC_R2BYTEMSK(_client_type,XC_ADDR_L(0x1F,0x13), 0xC000) >>14);
}

void KHal_XC_MLoad_Command_Format_initial(EN_MLOAD_CLIENT_TYPE _client_type)
{
    if (IS_SUPPORT_64BITS_COMMAND(ENABLE_64BITS_COMMAND, _client_type/CLIENT_NUM))
    {
        KHal_XC_MLoad_Enable_64BITS_COMMAND(_client_type,TRUE);
        if(ENABLE_64BITS_SPREAD_MODE)
        {
            KHal_XC_MLoad_Enable_64BITS_SPREAD_MODE(_client_type,TRUE);
        }
        //select MIU Bus : 00: 64bit, 01:128bit, 11:256bit
        KHal_XC_MLoad_Set_64Bits_MIU_Bus_Sel(_client_type);
    }
    else
    {
        KHal_XC_MLoad_set_miu_bus_sel(_client_type,MS_MLOAD_MIU_BUS_SEL);
    }
}

MS_U64 KHal_XC_MLoad_Gen_64bits_spreadMode(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask)
{
    MS_U64 u64CmdTemp = 0;
    MS_U8  u8AddrTemp = 0;
    MS_U16 u16BankTemp = 0;
    MS_U16 u16DataTemp = 0;
    MS_U16 u16MaskTemp = 0;

    u16MaskTemp = 0xFFFF;
    u16DataTemp = (SC_R2BYTE(_client_type,(0x130000 | u32Addr)) & ~u16Mask) | (u16Data & u16Mask);

    u8AddrTemp= (u32Addr & 0xFF) >> 1;
    u16BankTemp= (0x1300 | ((u32Addr >> 8) & 0xFF));// + _XC_Device_Offset[_client_type/CLIENT_NUM];

    u64CmdTemp|= (MS_U64)u16DataTemp;
    u64CmdTemp|= ((MS_U64)u8AddrTemp<<16);
    u64CmdTemp|= ((MS_U64)u16BankTemp<<23);
    u64CmdTemp|= ((MS_U64)u16MaskTemp<<48);

    return u64CmdTemp;
}

MS_BOOL KHal_XC_MLoad_parsing_64bits_spreadMode_NonXC(MS_U64 u64Cmd, MS_U32 *u32Addr, MS_U16 *u16Data)
{
    MS_U8  u8AddrTemp = 0;
    MS_U16 u16BankTemp = 0;

    *u16Data = (MS_U16)(0xFFFF&(MS_U16)u64Cmd);
    u8AddrTemp= (MS_U8)((u64Cmd>>16 & 0x7F) << 1);
    u16BankTemp= (MS_U16)((u64Cmd >> 23) & 0xFFFF);
    *u32Addr = (MS_U32)(u8AddrTemp|u16BankTemp<<8);

    return TRUE;
}

MS_U64 KHal_XC_MLoad_Gen_64bits_spreadMode_NonXC(MS_U32 u32Bank,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask)
{
    MS_U64 u64CmdTemp = 0;
    MS_U16 u16DataTemp = (MHal_XC_R2BYTE(u32Bank<<8|u32Addr) & ~u16Mask) | (u16Data & u16Mask);
    u64CmdTemp|= (MS_U64)u16DataTemp;
    u64CmdTemp|= ((MS_U64) ((u32Addr<<16) >>1));
    u64CmdTemp|= ((MS_U64)u32Bank<<23);
    u64CmdTemp|= ((MS_U64)u16Mask<<48);

    return u64CmdTemp;
}

MS_U64 KHal_XC_MLoad_Gen_64bits_subBankMode(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask)
{
    MS_MLoad_Data_64Bits_SubBank data_SBank_Mode;
    data_SBank_Mode.u32NoUse = 0x0;
    data_SBank_Mode.u8Addr = (u32Addr & 0xFF) >> 1;
    data_SBank_Mode.u8Bank = ((u32Addr >> 8) & 0xFF);// + _XC_Device_Offset[_client_type/CLIENT_NUM];

    if( u16Mask == 0xFFFF )
    {
        data_SBank_Mode.u16Data = u16Data;
    }
    else
    {
        data_SBank_Mode.u16Data = (SC_R2BYTE(_client_type,(0x130000 | u32Addr)) & ~u16Mask) | (u16Data & u16Mask);
    }
    return data_SBank_Mode.u64Cmd;
}

MS_BOOL KHal_XC_MLoad_parsing_32bits_subBankMode(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32MloadData, MS_U32 *pu32Addr, MS_U16 *pu16Data)
{
    MS_MLoad_Data data;
    data.u32Cmd = u32MloadData;
    *pu32Addr = (((MS_U32)(data.u8Addr))<<1) + ((((MS_U32)(data.u8Bank)) - _XC_Device_Offset[_client_type/CLIENT_NUM])<<8);
    *pu16Data = data.u16Data;

    return TRUE;
}

MS_U32 KHal_XC_MLoad_Gen_32bits_subBankMode(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask)
{
    MS_MLoad_Data data;

    data.u8Addr = (u32Addr & 0xFF) >> 1;
    data.u8Bank = ((u32Addr >> 8) & 0xFF) + _XC_Device_Offset[_client_type/CLIENT_NUM];

    if( u16Mask == 0xFFFF )
    {
        data.u16Data = u16Data;
    }
    else
    {
        data.u16Data = (SC_R2BYTE(_client_type, (0x130000|u32Addr)) & ~u16Mask) | (u16Data & u16Mask);
    }
    return data.u32Cmd;
}

MS_U16 KHal_XC_MLoad_Get_Depth(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 u16CmdCnt)
{
    MS_U16 result = u16CmdCnt;
    if (IS_SUPPORT_64BITS_COMMAND(ENABLE_64BITS_COMMAND, _client_type/CLIENT_NUM))
    {
        MS_U16 u16CmdLength = 0;
        MS_U16 u16CmdNum = 0;

        u16CmdLength = 8;//64 bits command = 8 bytes
        u16CmdNum = MS_MLOAD_BUS_WIDTH / u16CmdLength;
        if((u16CmdCnt%u16CmdNum)!=0)
        {
           printk("KickOff: Commands are not full!!\n");
        }
        result = u16CmdCnt/u16CmdNum;
    }
    else
    {
        result = 0;
    }

    return result;
}

void KHal_XC_MLoad_set_trigger_sync(EN_MLOAD_CLIENT_TYPE _client_type,EN_MLOAD_TRIG_SYNC eTriggerSync)
{
    switch(eTriggerSync)
    {
        case E_MLOAD_TRIGGER_BY_IP_MAIN_SYNC:
        {//trigger by IP_Main Vsync
           SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x1D), 0x2000, 0x7000);
        }
        break;
        case E_MLOAD_TRIGGER_BY_IP_SUB_SYNC:
        {//trigger by IP_Sub Vsync
           SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x1D), 0x3000, 0x7000);
        }
        break;
        default:
        {
            //default: trigger by OP Vsync
            #if 1
            if( SC_R2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x1D), 0x7000)!=0x0000)
            {
                //HW patch: menuload (triggered by ip vsync) can not stop
                SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x1D), 0x1000, 0x7000);
                SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x02), 0x0000, 0x8000);
                SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x02), 0x8000, 0x8000);
                SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x02), 0x0000, 0x8000);
            }
            #endif

            SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x1D), 0x0000, 0x7000);
        }
        break;
    }
}
void KHal_XC_MLoad_set_BitMask(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL enable)
{
    if(enable)
    {
        //Init XIU
        MDrv_WriteByteMask(0x10012E,BIT(0),BIT(0));
        //disable bit mask inverse (means: mask 0xFE can write bit 1111 1110)
        //(otherwise mask 0xFE will write bit 0000 0001)
        SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x07), 0x0000, 0x2000);
        SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x07), 0x8000, 0x8000);
    }else
    {
        MDrv_WriteByteMask(0x10012E,0,BIT(0));
        SC_W2BYTEMSK(_client_type, XC_ADDR_L(0x1F,0x07), 0x0000, 0x8000);
    }
}
