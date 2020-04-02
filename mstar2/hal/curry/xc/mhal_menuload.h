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
#ifndef MHAL_MENULOAD_H
#define MHAL_MENULOAD_H

//Select the source to trigger menuload
#define TRIG_SRC_FE_VFDE        0
#define TRIG_SRC_RE_VSYNC       1
#define TRIG_SRC_FE_VSYNC       2
#define TRIG_SRC_DELAY_LINE     3

#define OPM_LOCK_INIT_STATE     0
#define OPM_LOCK_FE_VSYNC       1
#define OPM_LOCK_TRAIN_TRIG_P   2
#define OPM_LOCK_DS             3

#define  MS_MLOAD_CMD_ALIGN     4
#define  MS_MLOAD_REG_LEN       0x04 // how many data in one MIU request
#define  MS_MLOAD_CMD_LEN       BYTE_PER_WORD
#define  MS_MLOAD_MEM_BASE_UNIT BYTE_PER_WORD
#define  MS_MLOAD_BUS_WIDTH     (16)
#define  MS_MLOAD_XC_MAX_CMD_CNT   1024
#define  MS_MLOAD_HDR_MAX_CMD_CNT  1024 // viable count that RD suggest.
#define  MS_MLOAD_MAX_CMD_CNT(_client_type) (((_client_type % CLIENT_NUM) == 0) ? MS_MLOAD_XC_MAX_CMD_CNT : MS_MLOAD_HDR_MAX_CMD_CNT)
#define  MS_MLOAD_NULL_CMD      0xFF010000
#define  MS_MLOAD_DUMMY_CMD_CNT(x)  ((x+0x0003)&~0x0003)
#define  MS_MLOAD_END_CMD(x) (0x1F020000 | (MS_MLOAD_REG_LEN) | (_XC_Device_Offset[_client_type/CLIENT_NUM] << 24))
#define  MS_MLOAD_MIU_BUS_SEL   0x00

#define  MS_MLOAD_CMD_LEN_64BITS 8 //8 bytes
#define  MS_MLOAD_NULL_CMD_SPREAD_MODE    (((MS_U64)0x13FF<<23)|((MS_U64)0x01<<16)|((MS_U64)0x0000))  //0x00000009FF810000
#define  MS_MLOAD_END_CMD_SPREAD_MODE(x)   ( ((MS_U64)0x0000<<48) | (((MS_U64)0x131F<<23)|((MS_U64)0x02<<16)|((MS_U64)0x0000)) | (MS_MLOAD_REG_LEN) ) //(0x000000098F820000 | (MS_MLOAD_REG_LEN)) = 0x000000098F820004
#define  MS_MLOAD_END_CMD_DEV1_SPREAD_MODE(x)   ( ((MS_U64)0x0000<<48) | (((MS_U64)0x139F<<23)|((MS_U64)0x02<<16)|((MS_U64)0x0000)) | (MS_MLOAD_REG_LEN) ) //(0x000000098F820000 | (MS_MLOAD_REG_LEN)) = 0x000000098F820004


#define  MS_MLG_REG_LEN         0x40 // how many data in one MIU request
#define  MS_MLG_CMD_LEN         BYTE_PER_WORD
#define  MS_MLG_MEM_BASE_UNIT   BYTE_PER_WORD

#define CLIENT_NUM 2

#define XC_ADDR_L(BANK, ADDR) (0x130000 | ((MS_U16)(BANK) << 8) | (MS_U16)((ADDR)*2))
#define XC_ADDR_H(BANK, ADDR) (0x130000 | ((MS_U16)(BANK) << 8) | (MS_U16)((ADDR)*2+1))

#define SC_W2BYTE(DEVICE_ID,ADDR, VAL) \
    MHal_XC_W2BYTE((ADDR|(_XC_Device_Offset[DEVICE_ID/CLIENT_NUM]<<8)),VAL)
#define SC_W2BYTEMSK(DEVICE_ID,ADDR, VAL ,MSK) \
    MHal_XC_W2BYTEMSK((ADDR|(_XC_Device_Offset[DEVICE_ID/CLIENT_NUM]<<8)),VAL,MSK)
#define SC_R2BYTE(DEVICE_ID,ADDR) \
    MHal_XC_R2BYTE((ADDR|(_XC_Device_Offset[DEVICE_ID/CLIENT_NUM]<<8)))
#define SC_R2BYTEMSK(DEVICE_ID,ADDR,MSK) \
    MHal_XC_R2BYTEMSK((ADDR|(_XC_Device_Offset[DEVICE_ID/CLIENT_NUM]<<8)), MSK)
#define MDrv_WriteByteMask(ADDR, VAL, MSK) \
   MHal_XC_WriteByteMask(ADDR, VAL, MSK)
#define MDrv_WriteByte(ADDR, VAL) \
   MHal_XC_WriteByte(ADDR, VAL)

#define BIT(_bit_) (1 << (_bit_))
typedef struct
{
    union
    {
        struct
        {
        	MS_U16 u16Data;
        	MS_U8 u8Addr;
        	MS_U8 u8Bank;
        };
        MS_U32 u32Cmd;
    };
}MS_MLoad_Data;

typedef struct
{
    union
    {
        struct
        {
        	MS_U16 u16Data;
        	MS_U8  u8Addr; //addr 0 ~ 0x7F
        	MS_U8  u8Bank; //subbank 0 ~ 0xFF
        	MS_U32 u32NoUse;
        };
        MS_U64 u64Cmd;
    };
}MS_MLoad_Data_64Bits_SubBank;

#ifdef MLG_1024 // Gamma_1024
typedef struct
{
    MS_U64 BData0 : 12;     //0
    MS_U64 BData1 : 12;
    MS_U64 BData2 : 12;
    MS_U64 GData0 : 12;
    MS_U64 GData1 : 12;
    MS_U64 GData2_L : 4;      //63

    MS_U64 GData2_H : 8;      //64
    MS_U64 RData0 : 12;
    MS_U64 RData1 : 12;
    MS_U64 RData2 : 12;
    MS_U64 Dummy0 :20;      //127

    MS_U64 Dummy1 : 16 ;       //128
    MS_U64 BEnable : 1 ;
    MS_U64 GEnable : 1 ;
    MS_U64 REnable : 1 ;
    MS_U64 Dummy2 :45;      //191

    MS_U64 Dummy3;      //192~255
} MS_SC_MLG_TBL;
#else // Gamma_256 or not support MLG case
typedef struct
{
    MS_U16 u16B;
    MS_U16 u16G;
    MS_U16 u16R;
    MS_U16 u16Enable;
    MS_U16 u16Dummy[4];
} MS_SC_MLG_TBL;
#endif

typedef enum
{
    MLOAD_TRIGGER_BY_OP_SYNC=0,
    MLOAD_TRIGGER_BY_IP_MAIN_SYNC=1,
    MLOAD_TRIGGER_BY_IP_SUB_SYNC=2,
    MLOAD_TRIGGER_BY_SW=3,
    MLOAD_TRIGGER_MAX,
}MLoad_Trigger_Sync;

typedef enum
{
    MLoad_WD_Timer_Reset_DMA = 0,
    MLoad_WD_Timer_Reset_MIU = 1,
    MLoad_WD_Timer_Reset_ALL = 3,
    MLoad_WD_Timer_Reset_MAX,
}MLoad_WD_Timer_Reset_Type;
// Define store value
typedef enum
{
    E_K_STORE_VALUE_AUTO_TUNE_AREA_TRIG = 0, //Generate TRAIN_TRIG_P from delayed line of Vsync(Setting the delay line for Auto tune area)
    E_K_STORE_VALUE_DISP_AREA_TRIG,  //Generate DISP_TRIG_P from delayed line of Vsync(Setting the delay line for Display area)
    E_K_STORE_VALUE_IP_AUTO_TUNE_AREA_TRIG, //  DI change to ip and add auto tune area trig for ip
    E_K_STORE_VALUE_IP_DISP_AREA_TRIG,  // DI change to ip and add display area trig for ip
    E_K_STORE_VALUE_INIT_CMD_NUM, //BK20_90 init cmd number
    E_K_STORE_VALUE_MAX
}EN_K_STORE_VALUE_TYPE;
typedef struct
{
    MS_U16 u16OldValue[E_K_STORE_VALUE_MAX];
    MS_U8 u8XCMLoadMIUSel;
    MS_PHY g_u32MLoadPhyAddr_Suspend;
    MS_U32 g_u32MLoadBufByteLen_Suspend;
}MLOAD_PRIVATE;

MS_U16 KHal_XC_MLoad_get_status(EN_MLOAD_CLIENT_TYPE _client_type);
void KHal_XC_MLoad_set_on_off(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bEn);
void KHal_XC_MLoad_set_len(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 u16Len);
void KHal_XC_MLoad_set_depth(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 u16depth);
void KHal_XC_MLoad_set_miusel(EN_MLOAD_CLIENT_TYPE _client_type,MS_U8 u8MIUSel);
void KHal_XC_MLoad_set_base_addr(EN_MLOAD_CLIENT_TYPE _client_type,MS_PHY u32addr);
void KHal_XC_MLoad_set_trigger_timing(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 u16sel);
void KHal_XC_MLoad_set_opm_lock(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 u16sel);
void KHal_XC_MLoad_set_trigger_delay(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 u16delay);
void KHal_XC_MLoad_set_trig_p(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 u16train, MS_U16 u16disp);
MS_BOOL KHal_XC_MLoad_get_trig_p(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 *pu16Train, MS_U16 *pu16Disp);
void KHal_XC_MLoad_Set_riu(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bEn);
MS_BOOL KHal_XC_MLoad_GetCaps(void);     //
void KHal_XC_MLoad_set_riu_cs(MS_BOOL bEn);      //
void KHal_XC_MLoad_set_sw_dynamic_idx_en(MS_BOOL ben);       //
void KHal_XC_MLoad_set_opm_arbiter_bypass(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL ben);
void KHal_XC_MLoad_set_miu_bus_sel(EN_MLOAD_CLIENT_TYPE _client_type,MS_U8 u8BitMode);
void KHal_XC_MLoad_enable_watch_dog(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bEn);
void KHal_XC_MLoad_set_watch_dog_time_delay(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 value);
void KHal_XC_MLoad_enable_watch_dog_reset(EN_MLOAD_CLIENT_TYPE _client_type,MLoad_WD_Timer_Reset_Type enMLWDResetType);

void KHal_XC_MLoad_Enable_64BITS_COMMAND(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bEn);
void KHal_XC_MLoad_Enable_64BITS_SPREAD_MODE(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bEn);
void KHal_XC_MLoad_Command_Format_initial(EN_MLOAD_CLIENT_TYPE _client_type);
void KHal_XC_MLoad_Set_64Bits_MIU_Bus_Sel(EN_MLOAD_CLIENT_TYPE _client_type);
MS_U8 KHal_XC_MLoad_Get_64Bits_MIU_Bus_Sel(EN_MLOAD_CLIENT_TYPE _client_type);
MS_U64 KHal_XC_MLoad_Gen_64bits_spreadMode(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);      //
MS_BOOL KHal_XC_MLoad_parsing_64bits_spreadMode_NonXC(MS_U64 u64Cmd, MS_U32 *u32Addr, MS_U16 *u16Data);      //
MS_U64 KHal_XC_MLoad_Gen_64bits_spreadMode_NonXC(MS_U32 u32Bank,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);     //
MS_U64 KHal_XC_MLoad_Gen_64bits_subBankMode(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);     //
MS_U32 KHal_XC_MLoad_Gen_32bits_subBankMode(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);     //
MS_BOOL KHal_XC_MLoad_parsing_32bits_subBankMode(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32MloadData, MS_U32 *pu32Addr, MS_U16 *pu16Data);       //
MS_U16 KHal_XC_MLoad_Get_Depth(EN_MLOAD_CLIENT_TYPE _client_type,MS_U16 u16CmdCnt);        //

void KHal_XC_MLoad_set_trigger_sync(EN_MLOAD_CLIENT_TYPE _client_type,EN_MLOAD_TRIG_SYNC eTriggerSync);
void KHal_XC_MLoad_set_BitMask(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL enable);
#endif

