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
#ifndef MHAL_DYNAMICSCALING_H
#define MHAL_DYNAMICSCALING_H

#ifdef MHAL_DYNAMICSCALING_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define DS_OP_CMD_LEN 4  //byte
#define MS_DS_NULL_CMD_32BITS    0xFFFF0000;         //Dummy cmd BKFF_FF=0x0000
#define MS_DS_NULL_CMD_64BITS    0x00000009FFFF0000; //Dummy cmd BKFF_7F=0x0000

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
extern MS_U32 _XC_Device_Offset[];
#ifdef SC_W2BYTE
#undef SC_W2BYTE
#endif

#ifdef SC_W2BYTEMSK
#undef SC_W2BYTEMSK
#endif

#ifdef SC_R2BYTE
#undef SC_R2BYTE
#endif

#ifdef SC_R2BYTEMSK
#undef SC_R2BYTEMSK
#endif

#define SC_W2BYTE(DEVICE_ID,ADDR, VAL) \
    MHal_XC_W2BYTE((ADDR|(_XC_Device_Offset[DEVICE_ID]<<8)),VAL)
#define SC_W2BYTEMSK(DEVICE_ID,ADDR, VAL ,MSK) \
    MHal_XC_W2BYTEMSK((ADDR|(_XC_Device_Offset[DEVICE_ID]<<8)),VAL,MSK)
#define SC_R2BYTE(DEVICE_ID,ADDR) \
    MHal_XC_R2BYTE((ADDR|(_XC_Device_Offset[DEVICE_ID]<<8)))
#define SC_R2BYTEMSK(DEVICE_ID,ADDR,MSK) \
    MHal_XC_R2BYTEMSK((ADDR|(_XC_Device_Offset[DEVICE_ID]<<8)), MSK)

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

//**********************DS NEW************************//
typedef struct
{
    E_DS_CLIENT client;
    MS_U32 offset;
    MS_U32 max_depth_num;
} ds_client_data;
typedef struct
{
    ds_client_data data;
    struct ds_node *next;
}ds_node;

typedef struct
{
    MS_PHY   phyDSAddr;
    MS_U32   u32DSSize;
    MS_VIRT  virtDSAddr;    //output, not set
} KHAL_DS_ADDR;

typedef struct
{
    KHAL_DS_ADDR stIPMAddr;
    KHAL_DS_ADDR stOPMAddr;
    KHAL_DS_ADDR stIPSAddr;
    KHAL_DS_ADDR stOPSAddr;
} KHAL_DS_STORED_ADDR_INFO;
//****************************************************//

INTERFACE MS_BOOL KHAL_SC_Set_DynamicScaling(EN_KDRV_SC_DEVICE u32DeviceID,MS_PHY u32MemBaseAddr, MS_U8 u8MIU_Select, MS_U8 u8IdxDepth, MS_BOOL bOP_On, MS_BOOL bIPS_On, MS_BOOL bIPM_On, MS_U32 u32DSBufferSize, EN_KDRV_WINDOW eWindow);
INTERFACE void KHAL_SC_Set_DynamicScalingFlag(EN_KDRV_SC_DEVICE u32DeviceID, MS_BOOL bEnable);
INTERFACE MS_BOOL KHAL_SC_Get_DynamicScaling_Status(EN_KDRV_SC_DEVICE u32DeviceID);
INTERFACE MS_BOOL KHAL_SC_Enable_IPMTuneAfterDS(EN_KDRV_SC_DEVICE u32DeviceID, MS_BOOL bEnable);
INTERFACE void KHal_SC_DynamicScaling_SWReset(void);
INTERFACE void KHal_SC_ResetSWDSCommand(EN_KDRV_SC_DEVICE u32DeviceID,EN_KDRV_WINDOW eWindow, MS_U8 u8DSIndex);
INTERFACE void KHal_SC_WriteSWDSCommand(EN_KDRV_SC_DEVICE u32DeviceID,EN_KDRV_WINDOW eWindow,E_DS_CLIENT client, MS_U32 u32CmdRegAddr, MS_U16 u16CmdRegValue,k_ds_reg_ip_op_sel IPOP_Sel,k_ds_reg_source_sel Source_Select,K_XC_DS_CMDCNT *pstXC_DS_CmdCnt,MS_U8 u8DSIndex);
INTERFACE void KHal_SC_Add_NullCommand(EN_KDRV_SC_DEVICE u32DeviceID,EN_KDRV_WINDOW eWindow,E_DS_CLIENT client,k_ds_reg_ip_op_sel IPOP_Sel,K_XC_DS_CMDCNT *pstXC_DS_CmdCnt,MS_U8 u8DSIndex);
//
//#define Hal_SC_SetSeamlessZapping(args...) 0
//#define Hal_SC_GetSeamlessZappingStatus(args...) 0
//#define Hal_SC_PrepareSeamlessZapping(args...)
INTERFACE MS_BOOL KHal_XC_Get_DSForceIndexSupported(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow);
INTERFACE void KHal_XC_Set_DSIndexSourceSelect(EN_KDRV_SC_DEVICE u32DeviceID, E_K_XC_DS_INDEX_SOURCE eDSIdxSrc, EN_KDRV_WINDOW eWindow);
INTERFACE void KHal_XC_Set_DSForceIndex(EN_KDRV_SC_DEVICE u32DeviceID, MS_BOOL bEnable, MS_U8 u8Index, EN_KDRV_WINDOW eWindow);
INTERFACE void KHal_XC_Enable_DS_64Bits_Command(EN_KDRV_SC_DEVICE u32DeviceID,MS_BOOL bEn);
INTERFACE void KHal_XC_Enable_DS_64Bits_Sread_Mode(EN_KDRV_SC_DEVICE u32DeviceID,MS_BOOL bEn);
INTERFACE void KHal_XC_Enable_DS_4_Baseaddress_Mode(EN_KDRV_SC_DEVICE u32DeviceID,MS_BOOL bEn);
INTERFACE void KHal_XC_Set_DS_64Bits_MIU_Bus_Sel(EN_KDRV_SC_DEVICE u32DeviceID);
INTERFACE void KHal_XC_Set_DS_BaseAddress(EN_KDRV_SC_DEVICE u32DeviceID,MS_PHY u32Base);
INTERFACE void KHal_XC_Set_DS_IndexDepth(EN_KDRV_SC_DEVICE u32DeviceID);
INTERFACE void KHal_XC_Set_DS_MIU_Sel(EN_KDRV_SC_DEVICE u32DeviceID,MS_U16 u16DSOnOff,MS_U16 u16DS_MIU_Sel_bit1);
INTERFACE MS_U64 KHal_SC_GenSpreadModeCmd(EN_KDRV_SC_DEVICE u32DeviceID, MS_U32 u32CmdRegAddr, MS_U16 u16CmdRegValue, MS_U16 u16Mask, k_ds_reg_source_sel eSourceSelect);
INTERFACE MS_BOOL KHal_DS_set_client(EN_KDRV_SC_DEVICE u32DeviceID,E_DS_CLIENT client,MS_U32 max_depth_num);
INTERFACE MS_BOOL KHal_SC_Set_DS_StoredAddr(KHAL_DS_STORED_ADDR_INFO *pstDSStoredAddr);
INTERFACE void KHal_XC_StoreSWDSCommand(EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, MS_U32 u32CmdRegAddr,
    MS_U16 u16CmdRegValue, MS_U16 u16CmdRegMask, k_ds_reg_ip_op_sel IPOP_Sel, k_ds_reg_source_sel Source_Select, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt);
INTERFACE void KHal_XC_WriteStoredSWDSCommand(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, MS_PHY phyStoredCmdAddr,
    MS_U32 u32CmdCnt, k_ds_reg_ip_op_sel IPOP_Sel, k_ds_reg_source_sel Source_Select, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt, MS_U8 u8DSIndex);
INTERFACE void KHal_XC_WriteSWDSCommandNonXC(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, MS_U32 u32Bank, MS_U16 u32Addr, MS_U32 u16Data, MS_U16 u16Mask, k_ds_reg_ip_op_sel IPOP_Sel, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt, MS_U8 u8DSIndex);
INTERFACE void KHal_SC_WriteSWDSCommand_Mask(EN_KDRV_SC_DEVICE u32DeviceID,EN_KDRV_WINDOW eWindow,E_DS_CLIENT client, MS_U32 u32CmdRegAddr, MS_U16 u16CmdRegValue,k_ds_reg_ip_op_sel IPOP_Sel,k_ds_reg_source_sel Source_Select,K_XC_DS_CMDCNT *pstXC_DS_CmdCnt,MS_U8 u8DSIndex,MS_U16 u16Mask);
INTERFACE MS_BOOL KHal_XC_InitDynamicScalingIndex(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow);
INTERFACE MS_BOOL KHal_XC_GetDynamicScalingCurrentIndex(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, MS_U8* pu8DSIndex);
INTERFACE MS_BOOL KHal_XC_GetDynamicScalingFireIndex(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow, MS_U8* pu8DSIndex);
INTERFACE MS_BOOL KHal_XC_FireDynamicScalingIndex(EN_KDRV_SC_DEVICE u32DeviceID, EN_KDRV_WINDOW eWindow);
#undef INTERFACE
#endif //MHAL_DYNAMICSCALING_H

