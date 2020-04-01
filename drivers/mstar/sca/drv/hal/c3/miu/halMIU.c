////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2008 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/kernel.h>
#else
#include <string.h>
#endif

#include "MsCommon.h"
#include "MsTypes.h"
#include "drvMIU.h"
#include "regMIU.h"
#include "halMIU.h"
#include "halCHIP.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
#define MIU_HAL_ERR(x, ...)			   {printf(x, ##__VA_ARGS__);}
#else
#define MIU_HAL_ERR(x, args...)        {printf(x, ##args);}
#endif
#define HAL_MIU_SSC_DBG(x)             //x

#define MIU_CLIENT_GP0  \
    MIU_CLIENT_NONE, \
    MIU_CLIENT_VIVALDI9_DECODER_R, \
    MIU_CLIENT_VIVALDI9_SE_R, \
    MIU_CLIENT_SC_LD_RW,\
    MIU_CLIENT_SECURE_R2_RW,\
    MIU_CLIENT_AU_R2_RW, \
    MIU_CLIENT_MAU_RW,\
    MIU_CLIENT_PM51_RW, \
    MIU_CLIENT_MHEG5_ICACHE_RW, \
    MIU_CLIENT_USB_UHC0_RW, \
    MIU_CLIENT_USB_UHC1_RW, \
    MIU_CLIENT_USB_UHC2_RW, \
    MIU_CLIENT_MVD_BBU_RW, \
    MIU_CLIENT_EMAC_RW, \
    MIU_CLIENT_BDMA_RW, \
    MIU_CLIENT_SC_2D3D_RW

#define MIU_CLIENT_GP1  \
    MIU_CLIENT_VIVALDI9_MAD_RW, \
    MIU_CLIENT_VIVALDI9_DMA_RW, \
    MIU_CLIENT_VIVALDI9_AUDMA_RW, \
    MIU_CLIENT_MVOP_128BIT_R, \
    MIU_CLIENT_MVOP1_R, \
    MIU_CLIENT_DISP_IPATH_NR_RW, \
    MIU_CLIENT_DISP_IPATH_MR_RW, \
    MIU_CLIENT_DISP_IPATH_DI_RW, \
    MIU_CLIENT_TSP_W, \
    MIU_CLIENT_TSP_R, \
    MIU_CLIENT_VD_TTX_RW, \
    MIU_CLIENT_VD_TTXSL_W, \
    MIU_CLIENT_VD_COMB_W, \
    MIU_CLIENT_VD_COMB_R, \
    MIU_CLIENT_VE_W, \
    MIU_CLIENT_VE_R

#define MIU_CLIENT_GP2  \
    MIU_CLIENT_GE_RW, \
    MIU_CLIENT_HVD_BBU_R, \
    MIU_CLIENT_HVD_RW, \
    MIU_CLIENT_HISPEED_UART_RW, \
    MIU_CLIENT_MVD_RW, \
    MIU_CLIENT_G3D_RW, \
    MIU_CLIENT_GPD_RW, \
    MIU_CLIENT_MFE0_W, \
    MIU_CLIENT_MFE1_R, \
    MIU_CLIENT_NAND_RW, \
    MIU_CLIENT_MPIF_RW, \
    MIU_CLIENT_DSCRMB_RW, \
    MIU_CLIENT_JPD_RW, \
    MIU_CLIENT_TSP_ORZ_W, \
    MIU_CLIENT_TSP_ORZ_R,\
    MIU_CLIENT_VP6_RW

#define MIU_CLIENT_GP3  \
    MIU_CLIENT_MIPS_RW, \
    MIU_CLIENT_GOP_W, \
    MIU_CLIENT_GOP0_R, \
    MIU_CLIENT_GOP1_R, \
    MIU_CLIENT_GOP2_R, \
    MIU_CLIENT_GOP3_R, \
    MIU_CLIENT_SC_OPMAIN_RW,\
    MIU_CLIENT_SC_IPMAIN_RW,\
    MIU_CLIENT_SC_IPSUB_RW,\
    MIU_CLIENT_SC_OD_W,\
    MIU_CLIENT_SC_OD_R

#define MIU1_CLIENT_GP0  \
    MIU_CLIENT_NONE, \
    MIU_CLIENT_VIVALDI9_DECODER_R, \
    MIU_CLIENT_VIVALDI9_SE_R, \
    MIU_CLIENT_SC_LD_RW,\
    MIU_CLIENT_SECURE_R2_RW,\
    MIU_CLIENT_AU_R2_RW, \
    MIU_CLIENT_MAU_RW,\
    MIU_CLIENT_PM51_RW, \
    MIU_CLIENT_MHEG5_ICACHE_RW, \
    MIU_CLIENT_USB_UHC0_RW, \
    MIU_CLIENT_USB_UHC1_RW, \
    MIU_CLIENT_USB_UHC2_RW, \
    MIU_CLIENT_MVD_BBU_RW, \
    MIU_CLIENT_EMAC_RW, \
    MIU_CLIENT_BDMA_RW, \
    MIU_CLIENT_SC_2D3D_RW

#define MIU1_CLIENT_GP1  \
    MIU_CLIENT_VIVALDI9_MAD_RW, \
    MIU_CLIENT_VIVALDI9_DMA_RW, \
    MIU_CLIENT_VIVALDI9_AUDMA_RW, \
    MIU_CLIENT_MVOP_128BIT_R, \
    MIU_CLIENT_MVOP1_R, \
    MIU_CLIENT_DISP_IPATH_NR_RW, \
    MIU_CLIENT_DISP_IPATH_MR_RW, \
    MIU_CLIENT_DISP_IPATH_DI_RW, \
    MIU_CLIENT_TSP_W, \
    MIU_CLIENT_TSP_R, \
    MIU_CLIENT_VD_TTX_RW, \
    MIU_CLIENT_VD_TTXSL_W, \
    MIU_CLIENT_VD_COMB_W, \
    MIU_CLIENT_VD_COMB_R, \
    MIU_CLIENT_VE_W, \
    MIU_CLIENT_VE_R

#define MIU1_CLIENT_GP2  \
    MIU_CLIENT_GE_RW, \
    MIU_CLIENT_HVD_BBU_R, \
    MIU_CLIENT_HVD_RW, \
    MIU_CLIENT_HISPEED_UART_RW, \
    MIU_CLIENT_MVD_RW, \
    MIU_CLIENT_G3D_RW, \
    MIU_CLIENT_GPD_RW, \
    MIU_CLIENT_MFE0_W, \
    MIU_CLIENT_MFE1_R, \
    MIU_CLIENT_NAND_RW, \
    MIU_CLIENT_MPIF_RW, \
    MIU_CLIENT_DSCRMB_RW, \
    MIU_CLIENT_JPD_RW, \
    MIU_CLIENT_TSP_ORZ_W, \
    MIU_CLIENT_TSP_ORZ_R,\
    MIU_CLIENT_VP6_RW

#define MIU1_CLIENT_GP3  \
    MIU_CLIENT_MIPS_RW, \
    MIU_CLIENT_GOP_W, \
    MIU_CLIENT_GOP0_R, \
    MIU_CLIENT_GOP1_R, \
    MIU_CLIENT_GOP2_R, \
    MIU_CLIENT_GOP3_R, \
    MIU_CLIENT_SC_OPMAIN_RW,\
    MIU_CLIENT_SC_IPMAIN_RW,\
    MIU_CLIENT_SC_IPSUB_RW,\
    MIU_CLIENT_SC_OD_W,\
    MIU_CLIENT_SC_OD_R

#define KHz                 (1000UL)
#define MHz                 (1000000UL)
#define MPPL                (432)
#define DDR_FACTOR          (524288)
#define DDFSPAN_FACTOR      (131072)
#define IDNUM_KERNELPROTECT (8)

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------
const eMIUClientID clientTbl[MIU_MAX_DEVICE][MIU_MAX_TBL_CLIENT] =
{
    {MIU_CLIENT_GP0, MIU_CLIENT_GP1, MIU_CLIENT_GP2, MIU_CLIENT_GP3}
    ,{MIU1_CLIENT_GP0, MIU1_CLIENT_GP1, MIU1_CLIENT_GP2, MIU1_CLIENT_GP3}
};

MS_U8 clientId_KernelProtect[IDNUM_KERNELPROTECT] =
{
    MIU_CLIENT_MIPS_RW, MIU_CLIENT_NAND_RW, MIU_CLIENT_USB_UHC0_RW, MIU_CLIENT_USB_UHC1_RW,
    MIU_CLIENT_USB_UHC2_RW, MIU_CLIENT_NONE, MIU_CLIENT_NONE, MIU_CLIENT_NONE
};
//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
#if defined(MSOS_TYPE_LINUX_KERNEL)
#define  RIU_OFFSET 0xDE000000
static MS_U32 _gMIU_MapBase = (0x1F200000 + RIU_OFFSET);      //default set to MIPS platfrom
static MS_U32 _gPM_MapBase = (0x1F000000 + RIU_OFFSET);      //default set to MIPS platfrom
#else
static MS_U32 _gMIU_MapBase = 0xBF200000;      //default set to MIPS platfrom
static MS_U32 _gPM_MapBase = 0xBF000000;      //default set to MIPS platfrom
#endif

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Type and Structure Declaration
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
void HAL_MIU_SetIOMapBase(MS_U32 u32Base)
{
    _gMIU_MapBase = u32Base;
    HAL_MIU_SSC_DBG(printf("MIU _gMIU_MapBase= %lx\n", _gMIU_MapBase));
}

void HAL_MIU_SetPMIOMapBase(MS_U32 u32Base)
{
    _gPM_MapBase = u32Base;
    HAL_MIU_SSC_DBG(printf("MIU _gPM_MapBase= %lx\n", _gPM_MapBase));
}

MS_S16 HAL_MIU_GetClientInfo(MS_U8 u8MiuDev, eMIUClientID eClientID)
{
    MS_U8 idx;

    if (MIU_MAX_DEVICE <= u8MiuDev)
    {
        printf("Wrong MIU device:%u\n", u8MiuDev);
        return (-1);
    }

    for (idx = 0; idx < MIU_MAX_TBL_CLIENT; idx++)
        if (eClientID == clientTbl[u8MiuDev][idx])
            return idx;
    return (-1);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_MIU_ReadByte
/// @brief \b Function  \b Description: read 1 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <OUT>        \b None :
/// @param <RET>        \b MS_U8
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
MS_U8 HAL_MIU_ReadByte(MS_U32 u32RegAddr)
{
    return ((volatile MS_U8*)(_gMIU_MapBase))[(u32RegAddr << 1) - (u32RegAddr & 1)];
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_MIU_PM_ReadByte
/// @brief \b Function  \b Description: read 1 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <OUT>        \b None :
/// @param <RET>        \b MS_U8
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
MS_U8 HAL_MIU_PM_ReadByte(MS_U32 u32RegAddr)
{
    return ((volatile MS_U8*)(_gPM_MapBase))[(u32RegAddr << 1) - (u32RegAddr & 1)];
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_MIU_Read4Byte
/// @brief \b Function  \b Description: read 2 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <OUT>        \b None :
/// @param <RET>        \b MS_U16
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
MS_U16 HAL_MIU_Read2Byte(MS_U32 u32RegAddr)
{
    return ((volatile MS_U16*)(_gMIU_MapBase))[u32RegAddr];
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_MIU_Read4Byte
/// @brief \b Function  \b Description: read 4 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <OUT>        \b None :
/// @param <RET>        \b MS_U32
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
MS_U32 HAL_MIU_Read4Byte(MS_U32 u32RegAddr)
{
    return (HAL_MIU_Read2Byte(u32RegAddr) | HAL_MIU_Read2Byte(u32RegAddr+2) << 16);
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_MIU_WriteByte
/// @brief \b Function  \b Description: write 1 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <IN>         \b u8Val : 1 byte data
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_MIU_WriteByte(MS_U32 u32RegAddr, MS_U8 u8Val)
{
    if (!u32RegAddr)
    {
        MIU_HAL_ERR("%s reg error!\n", __FUNCTION__);
        return FALSE;
    }

    ((volatile MS_U8*)(_gMIU_MapBase))[(u32RegAddr << 1) - (u32RegAddr & 1)] = u8Val;
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_MIU_Write2Byte
/// @brief \b Function  \b Description: write 2 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <IN>         \b u16Val : 2 byte data
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_MIU_Write2Byte(MS_U32 u32RegAddr, MS_U16 u16Val)
{
    if (!u32RegAddr)
    {
        MIU_HAL_ERR("%s reg error!\n", __FUNCTION__);
        return FALSE;
    }

    ((volatile MS_U16*)(_gMIU_MapBase))[u32RegAddr] = u16Val;
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_Write4Byte
/// @brief \b Function  \b Description: write 4 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <IN>         \b u32Val : 4 byte data
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_MIU_Write4Byte(MS_U32 u32RegAddr, MS_U32 u32Val)
{
    if (!u32RegAddr)
    {
        MIU_HAL_ERR("%s reg error!\n", __FUNCTION__);
        return FALSE;
    }

    HAL_MIU_Write2Byte(u32RegAddr, (MS_U16)(u32Val & 0x0000FFFF));
    HAL_MIU_Write2Byte(u32RegAddr+2, (MS_U16)(u32Val >> 16));
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_WriteByte
/// @brief \b Function  \b Description: write 1 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <IN>         \b u8Val : 1 byte data
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_MIU_WriteRegBit(MS_U32 u32RegAddr, MS_U8 u8Mask, MS_BOOL bEnable)
{
    MS_U8 u8Val = HAL_MIU_ReadByte(u32RegAddr);
    if (!u32RegAddr)
    {
        MIU_HAL_ERR("%s reg error!\n", __FUNCTION__);
        return FALSE;
    }

    u8Val = HAL_MIU_ReadByte(u32RegAddr);
    u8Val = (bEnable) ? (u8Val | u8Mask) : (u8Val & ~u8Mask);
    HAL_MIU_WriteByte(u32RegAddr, u8Val);
    return TRUE;
}

void HAL_MIU_Write2BytesBit(MS_U32 u32RegOffset, MS_BOOL bEnable, MS_U16 u16Mask)
{
    MS_U16 val = HAL_MIU_Read2Byte(u32RegOffset);
    val = (bEnable) ? (val | u16Mask) : (val & ~u16Mask);
    HAL_MIU_Write2Byte(u32RegOffset, val);
}

void HAL_MIU_SetProtectID(MS_U32 u32Reg, MS_U8 u8MiuDev, MS_U8 u8ClientID)
{
    MS_S16 sVal = HAL_MIU_GetClientInfo(u8MiuDev, (eMIUClientID)u8ClientID);
    if (0 > sVal)
        sVal = 0;
    HAL_MIU_WriteByte(u32Reg, (MS_U8)(sVal));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_GetDefaultClientID_KernelProtect()
/// @brief \b Function \b Description:  Get default client id array pointer for protect kernel
/// @param <RET>           \b     : The pointer of Array of client IDs
////////////////////////////////////////////////////////////////////////////////
MS_U8* HAL_MIU_GetDefaultClientID_KernelProtect()
{
     if(IDNUM_KERNELPROTECT > 0)
         return  (MS_U8 *)&clientId_KernelProtect[0];

     return NULL;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function    \b Name: HAL_MIU_ProtectAlign()
/// @brief \b Function    \b Description:  Get the page shift for MIU protect
/// @param <*u32PageShift>\b IN: Page shift
/// @param <RET>          \b OUT: None
////////////////////////////////////////////////////////////////////////////////
MS_U32 HAL_MIU_ProtectAlign(void)
{
    MS_U32 u32PageShift;

    u32PageShift = MIU_PAGE_SHIFT;
    return u32PageShift;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_Dram_Size()
/// @brief \b Function \b Description: Set up Dram size for MIU protect
/// @param MiuID        \b IN     : MIU ID
/// @param DramSize     \b IN     : Specified Dram size for MIU protect
/// @param <OUT>           \b None    :
/// @param <RET>           \b None    :
/// @param <GLOBAL>        \b None    :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_MIU_Dram_Size(MS_U8 MiuID, MS_U8 DramSize)
{
    MS_U32 u32RegAddr;

    if(E_MIU_0 == (MIU_ID)MiuID)
    {
        u32RegAddr = MIU_PROTECT_DDR_SIZE;
        switch (DramSize)
        {
            case E_MIU_DDR_32MB:
			    HAL_MIU_WriteByte(u32RegAddr, MIU_PROTECT_DDR_32MB);
                break;
            case E_MIU_DDR_64MB:
			    HAL_MIU_WriteByte(u32RegAddr, MIU_PROTECT_DDR_64MB);
                break;
            case E_MIU_DDR_128MB:
			    HAL_MIU_WriteByte(u32RegAddr, MIU_PROTECT_DDR_128MB);
                break;
            case E_MIU_DDR_256MB:
			    HAL_MIU_WriteByte(u32RegAddr, MIU_PROTECT_DDR_256MB);
                break;
            case E_MIU_DDR_512MB:
			    HAL_MIU_WriteByte(u32RegAddr, MIU_PROTECT_DDR_512MB);
                break;
            case E_MIU_DDR_1024MB:
			    HAL_MIU_WriteByte(u32RegAddr, MIU_PROTECT_DDR_1024MB);
                break;
            case E_MIU_DDR_2048MB:
			    HAL_MIU_WriteByte(u32RegAddr, MIU_PROTECT_DDR_2048MB);
                break;
            default:
                return false;
        }
    }
    else
    {
        u32RegAddr = MIU1_PROTECT_DDR_SIZE;
        switch (DramSize)
        {
            case E_MIU_DDR_32MB:
			    HAL_MIU_WriteByte(u32RegAddr, MIU_PROTECT_DDR_32MB);
                break;
            case E_MIU_DDR_64MB:
			    HAL_MIU_WriteByte(u32RegAddr, MIU_PROTECT_DDR_64MB);
                break;
            case E_MIU_DDR_128MB:
			    HAL_MIU_WriteByte(u32RegAddr, MIU_PROTECT_DDR_128MB);
                break;
            case E_MIU_DDR_256MB:
			    HAL_MIU_WriteByte(u32RegAddr, MIU_PROTECT_DDR_256MB);
                break;
            case E_MIU_DDR_512MB:
			    HAL_MIU_WriteByte(u32RegAddr, MIU_PROTECT_DDR_512MB);
                break;
            case E_MIU_DDR_1024MB:
			    HAL_MIU_WriteByte(u32RegAddr, MIU_PROTECT_DDR_1024MB);
                break;
            case E_MIU_DDR_2048MB:
			    HAL_MIU_WriteByte(u32RegAddr, MIU_PROTECT_DDR_2048MB);
                break;
            default:
                return false;
        }
     }
     return true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_Protect()
/// @brief \b Function \b Description:  Enable/Disable MIU Protection mode
/// @param u8Blockx        \b IN     : MIU Block to protect (0 ~ 4)
/// @param *pu8ProtectId   \b IN     : Allow specified client IDs to write
/// @param u32Start        \b IN     : Starting address
/// @param u32End          \b IN     : End address
/// @param bSetFlag        \b IN     : Disable or Enable MIU protection
///                                      - -Disable(0)
///                                      - -Enable(1)
/// @param <OUT>           \b None    :
/// @param <RET>           \b None    :
/// @param <GLOBAL>        \b None    :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_MIU_Protect(
                          MS_U8    u8Blockx,
                          MS_U8    *pu8ProtectId,
                          MS_U32   u32Start,
                          MS_U32   u32End,
                          MS_BOOL  bSetFlag
                         )
{
    MS_U32 u32RegAddr;
    MS_U16 u16Data;
    MS_U8  u8Data;
    MS_U8  u8Data1;
    MS_U8  u8MiuSel;
    MS_U32 u32MiuProtectEn;

    u8MiuSel = E_MIU_0;

    // Incorrect Block ID
    if(u8Blockx >= 4)
    {
        return false;
    }

    if(((u32Start & ((1 << MIU_PAGE_SHIFT) -1)) != 0) || ((u32End & ((1 << MIU_PAGE_SHIFT) -1)) != 0))
    {
        MIU_HAL_ERR("Protected address should be aligned to 8KB\n")
        return false;
    }

    if(u8Blockx <= 2)
        u8Data = 1 << u8Blockx;
    else
        u8Data = 1 << (u8Blockx - 3);

	if( u32Start < HAL_MIU1_BASE)
	{
        switch (u8Blockx)
        {
            case E_MIU_BLOCK_0:
		        u32RegAddr = MIU_PROTECT0_ID0;
		        u32MiuProtectEn=MIU_PROTECT_L_EN;
                break;
            case E_MIU_BLOCK_1:
		        u32RegAddr = MIU_PROTECT1_ID0;
		        u32MiuProtectEn=MIU_PROTECT_L_EN;
                break;
            case E_MIU_BLOCK_2:
		        u32RegAddr = MIU_PROTECT2_ID0;
		        u32MiuProtectEn=MIU_PROTECT_L_EN;
                break;
            case E_MIU_BLOCK_3:
		        u32RegAddr = MIU_PROTECT3_ID0;
		        u32MiuProtectEn=MIU_PROTECT_H_EN;
                break;
            case E_MIU_BLOCK_4:
		        u32RegAddr = MIU_PROTECT4_ID0;
		        u32MiuProtectEn=MIU_PROTECT_H_EN;
                break;
            default:
		        return false;
        }
	}
	else
	{
        switch (u8Blockx)
        {
            case E_MIU_BLOCK_0:
		        u32MiuProtectEn=MIU1_PROTECT_L_EN;
		        u32RegAddr = MIU1_PROTECT0_ID0;
                break;
            case E_MIU_BLOCK_1:
		        u32MiuProtectEn=MIU1_PROTECT_L_EN;
		        u32RegAddr = MIU1_PROTECT1_ID0;
                break;
            case E_MIU_BLOCK_2:
		        u32MiuProtectEn=MIU1_PROTECT_L_EN;
		        u32RegAddr = MIU1_PROTECT2_ID0;
                break;
            case E_MIU_BLOCK_3:
		        u32MiuProtectEn=MIU1_PROTECT_H_EN;
		        u32RegAddr = MIU1_PROTECT3_ID0;
                break;
            case E_MIU_BLOCK_4:
		        u32MiuProtectEn=MIU1_PROTECT_H_EN;
		        u32RegAddr = MIU1_PROTECT4_ID0;
                break;
            default:
		        return false;
        }
		u32Start-=HAL_MIU1_BASE;
		u32End-=HAL_MIU1_BASE;;
        u8MiuSel = E_MIU_1;
	}

    // Disable MIU protect
    HAL_MIU_WriteRegBit(u32MiuProtectEn,u8Data,DISABLE);

    if ( bSetFlag )
    {
        // Protect IDs
        HAL_MIU_SetProtectID(u32RegAddr + 0, (u8MiuSel== E_MIU_1), pu8ProtectId[0]);
        HAL_MIU_SetProtectID(u32RegAddr + 1, (u8MiuSel== E_MIU_1), pu8ProtectId[1]);

        if (u8Blockx==0)
        {
            HAL_MIU_SetProtectID(u32RegAddr + 2, (u8MiuSel== E_MIU_1), pu8ProtectId[2]);
            HAL_MIU_SetProtectID(u32RegAddr + 3, (u8MiuSel== E_MIU_1), pu8ProtectId[3]);
            HAL_MIU_SetProtectID(u32RegAddr + 4, (u8MiuSel== E_MIU_1), pu8ProtectId[4]);
            HAL_MIU_SetProtectID(u32RegAddr + 5, (u8MiuSel== E_MIU_1), pu8ProtectId[5]);
            HAL_MIU_SetProtectID(u32RegAddr + 6, (u8MiuSel== E_MIU_1), pu8ProtectId[6]);
            HAL_MIU_SetProtectID(u32RegAddr + 7, (u8MiuSel== E_MIU_1), pu8ProtectId[7]);

            // Set BIT29,30 of start address
            u8Data1 = HAL_MIU_ReadByte(u32RegAddr);
            u8Data1 = (u8Data1 & 0x3F) | (MS_U8)((u32Start >> 29) << 6);
            HAL_MIU_WriteByte(u32RegAddr, u8Data1);

            // Set BIT29,30 of end address
            u8Data1 = HAL_MIU_ReadByte(u32RegAddr + 2);
            u8Data1 = (u8Data1 & 0x3F) | (MS_U8)((u32End >> 29) << 6);
            HAL_MIU_WriteByte(u32RegAddr + 2, u8Data1);

            u32RegAddr += 8;

            // Start Address
            u16Data = (MS_U16)(u32Start >> MIU_PAGE_SHIFT);   //8k/unit
            HAL_MIU_Write2Byte(u32RegAddr, u16Data);

            // End Address
            u16Data = (MS_U16)((u32End >> MIU_PAGE_SHIFT)-1);   //8k/unit;
            HAL_MIU_Write2Byte(u32RegAddr + 2, u16Data);
        }
        else
	    {
            // Set BIT29,30 of start address
            u8Data1 = HAL_MIU_ReadByte(u32RegAddr);
            u8Data1 = (u8Data1 & 0x3F) | (MS_U8)((u32Start >> 29) << 6);
            HAL_MIU_WriteByte(u32RegAddr, u8Data1);

            // Set BIT29,30 of end address
            u8Data1 = HAL_MIU_ReadByte(u32RegAddr + 1);
            u8Data1 = (u8Data1 & 0x3F) | (MS_U8)((u32End >> 29) << 6);
            HAL_MIU_WriteByte(u32RegAddr + 1, u8Data1);

            u32RegAddr += 2;

            // Start Address
            u16Data = (MS_U16)(u32Start >> MIU_PAGE_SHIFT);   //8k/unit
            HAL_MIU_Write2Byte(u32RegAddr, u16Data);

            // End Address
            u16Data = (MS_U16)((u32End >> MIU_PAGE_SHIFT)-1);   //8k/unit;
            HAL_MIU_Write2Byte(u32RegAddr + 2, u16Data);
        }

        // Enable MIU protect
        HAL_MIU_WriteRegBit(u32MiuProtectEn, u8Data, ENABLE);
    }

    return TRUE;
}

#define GET_HIT_BLOCK(regval)       BITS_RANGE_VAL(regval, REG_MIU_PROTECT_HIT_NO)
#define GET_HIT_CLIENT(regval)      BITS_RANGE_VAL(regval, REG_MIU_PROTECT_HIT_ID)

MS_BOOL HAL_MIU_GetProtectInfo(MS_U8 u8MiuDev, MIU_PortectInfo *pInfo)
{
    MS_U16 ret = 0;
    MS_U16 loaddr = 0;
    MS_U16 hiaddr = 0;
    MS_U32 u32Reg = (u8MiuDev) ? MIU1_REG_BASE : MIU_REG_BASE;

    if (!pInfo)
        return FALSE;

    ret = HAL_MIU_Read2Byte(u32Reg+REG_MIU_PROTECT_STATUS);
    loaddr = HAL_MIU_Read2Byte(u32Reg+REG_MIU_PROTECT_LOADDR);
    hiaddr = HAL_MIU_Read2Byte(u32Reg+REG_MIU_PROTECT_HIADDR);

    if (REG_MIU_PROTECT_HIT_FALG & ret)
    {
        pInfo->bHit = TRUE;

        pInfo->u8Block = (MS_U8)GET_HIT_BLOCK(ret);
        pInfo->u8Group = (MS_U8)(GET_HIT_CLIENT(ret) >> 4);
        pInfo->u8ClientID = (MS_U8)(GET_HIT_CLIENT(ret) & 0x0F);
        pInfo->uAddress = (MS_U32)((hiaddr << 16) | loaddr) ;
        pInfo->uAddress = pInfo->uAddress * MIU_PROTECT_ADDRESS_UNIT;

        printf("MIU%u Block:%u Group:%u ClientID:%u Hitted_Address:0x%lx<->0x%lx\n", u8MiuDev,
            pInfo->u8Block, pInfo->u8Group, pInfo->u8ClientID, pInfo->uAddress, pInfo->uAddress + MIU_PROTECT_ADDRESS_UNIT - 1);

        //clear log
        HAL_MIU_Write2BytesBit(u32Reg+REG_MIU_PROTECT_STATUS, TRUE, REG_MIU_PROTECT_LOG_CLR);
        HAL_MIU_Write2BytesBit(u32Reg+REG_MIU_PROTECT_STATUS, FALSE, REG_MIU_PROTECT_LOG_CLR);
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_SetSsc()
/// @brief \b Function \b Description: MDrv_MIU_SetSsc, @Step & Span
/// @param u16Fmodulation   \b IN : 20KHz ~ 40KHz (Input Value = 20 ~ 40)
/// @param u16FDeviation    \b IN  : under 0.1% ~ 2% (Input Value = 1 ~ 20)
/// @param bEnable          \b IN    :
/// @param None             \b OUT  :
/// @param None             \b RET  :
/// @param None             \b GLOBAL :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_MIU_SetSsc(MS_U8 u8MiuDev, MS_U16 u16Fmodulation, MS_U16 u16FDeviation, MS_BOOL bEnable)
{
    MS_U32 uDDFSET, uDDR_MHz, uDDFStep, uRegBase = MIU_ATOP_BASE;
    MS_U16 u16DDFSpan;
    MS_U16 u16Input_DIV_First,u16Input_DIV_Second,u16Loop_DIV_First,u16Loop_DIV_Second;
    MS_U8  u8Temp,i;

    //Pre check the input
        if (u8MiuDev == 1)
        {
           if((HAL_MIU_Read2Byte(MIU1_REG_BASE)&BIT15))
           {
               uRegBase = MIU_ATOP_BASE+0x80;
           }
           else
           {
               printf("there is no MIU1\n");
               return 0;
           }
        }
        else
        {
           uRegBase = MIU_ATOP_BASE;
        }

        HAL_MIU_SSC_DBG(printf("MMIO base:%lx uRegBase:%lx\n", _gMIU_MapBase, uRegBase));

        if ((u16Fmodulation<20)||(u16Fmodulation>40))
        {
            MIU_HAL_ERR("SSC u16Fmodulation Error...(20KHz - 40KHz)\n");
            return 0;
        }

        if ((u16FDeviation<1)||(u16FDeviation>20))
        {
            MIU_HAL_ERR("SSC u16FDeviation Error...(0.1%% - 2%% ==> 1 ~20)\n");
            return 0;
        }

        HAL_MIU_SSC_DBG(printf("---> u16Fmodulation = %d u16FDeviation = %d \n",(int)u16Fmodulation,(int)u16FDeviation));
    //<1>.Caculate DDFM = (Loop_DIV_First * Loop_DIV_Second)/(Input_DIV_First * Input_DIV_Second);
        //Prepare Input_DIV_First
        u8Temp = ((MS_U16)(HAL_MIU_Read2Byte(uRegBase+MIU_DDRPLL_DIV_FIRST)&0x30)>>4);       //Bit 13,12 (0x110D36)
        u16Input_DIV_First = 0x01;
        for (i=0;i<u8Temp;i++)
            u16Input_DIV_First = u16Input_DIV_First << 1;
        //Prepare Input_DIV_Second
        u16Input_DIV_Second = (HAL_MIU_ReadByte(uRegBase+MIU_PLL_INPUT_DIV_2ND));     //Bit 0~7 (0x101222)
        if (u16Input_DIV_Second == 0)
            u16Input_DIV_Second = 1;
        //Prepare Loop_DIV_First
        u8Temp = ((HAL_MIU_ReadByte(uRegBase+MIU_DDRPLL_DIV_FIRST)&0xC0)>>6);         //Bit 15,14 (0x110D36)
        u16Loop_DIV_First = 0x01;
         for (i=0;i<u8Temp;i++)
            u16Loop_DIV_First = u16Loop_DIV_First << 1;

        //Prepare Loop_DIV_Second
        u16Loop_DIV_Second = (HAL_MIU_ReadByte(uRegBase+MIU_PLL_LOOP_DIV_2ND));      //Bit 0~7 (0x101223)
        if (u16Loop_DIV_Second == 0)
            u16Loop_DIV_Second = 1;

    //<2>.From DDFSET register to get DDRPLL
        uDDFSET = HAL_MIU_Read4Byte(uRegBase+MIU_DDFSET) & 0x00ffffff;
        //DDRPLL = MPPL * DDR_FACTOR * Loop_First * Loop_Second / DDFSET * Input_First * Input_Second
        HAL_MIU_SSC_DBG(printf("---> Loop_First:%u Loop_Second:%u\n", u16Loop_DIV_First, u16Loop_DIV_Second));
        HAL_MIU_SSC_DBG(printf("---> Input_first:%u Input_second:%u\n", u16Input_DIV_First, u16Input_DIV_Second));
        uDDR_MHz = (MPPL * DDR_FACTOR * u16Loop_DIV_First * u16Loop_DIV_Second)/ (uDDFSET*u16Input_DIV_First*u16Input_DIV_Second);
        HAL_MIU_SSC_DBG(printf("---> uDDFSET = 0x%lx\n",uDDFSET));
        HAL_MIU_SSC_DBG(printf("---> DDR_MHZ = 0x%lx (%d MHz)\n",uDDR_MHz,(int)uDDR_MHz));

    //<3>.Caculate DDFSPAN = (MPLL * DDFSPAN_FACTOR * MHz) / (DDFSET * Fmodulation * KHz)
        u16DDFSpan = (MS_U16)((MS_U32)((DDFSPAN_FACTOR * MPPL/u16Fmodulation)* 1000/uDDFSET));
        HAL_MIU_SSC_DBG(printf("---> DDFSPAN = 0x%x (%d)\n",u16DDFSpan,(int)u16DDFSpan));
        if (u16DDFSpan > 0x3FFF)
        {
            u16DDFSpan = 0x3FFF;
            HAL_MIU_SSC_DBG(printf("??? DDFSPAN overflow > 0x3FFF, Fource set to 0x03FF\n"));
        }

        //Write to Register
        HAL_MIU_Write2Byte(uRegBase+MIU_DDFSPAN,u16DDFSpan);
    //<4>.Caculate DDFSTEP = (FDeviation*DDFSET/10)/(DDFSPAN*100)
        uDDFStep = (MS_U32)((u16FDeviation * (uDDFSET/10))/(u16DDFSpan*100));
        HAL_MIU_SSC_DBG(printf("---> DDFSTEP = 0x%lx (%lu)\n",uDDFStep,uDDFStep));
        //Write to Register
        uDDFStep &= (0x03FF);
        HAL_MIU_Write2Byte(uRegBase+MIU_DDFSTEP,(MS_U16)((HAL_MIU_Read2Byte(uRegBase+MIU_DDFSTEP) & (~0x03FF))|uDDFStep));

    //<5>.Set ENABLE
        if(bEnable == ENABLE)
            HAL_MIU_WriteByte(uRegBase+MIU_SSC_EN,(HAL_MIU_ReadByte(uRegBase+MIU_SSC_EN)|0xC0));
        else
            HAL_MIU_WriteByte(uRegBase+MIU_SSC_EN,(HAL_MIU_ReadByte(uRegBase+MIU_SSC_EN)&(~0xC0))|0x80);
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_MaskReq()
/// @brief \b Function \b Description: Mask MIU request
/// @param u8Miu  IN        \b  : miu0 or miu1
/// @param eClientID IN     \b  : client ID
/// @param None   OUT       \b  :
/// @param None   RET       \b  :
/// @param None   GLOBAL    \b  :
////////////////////////////////////////////////////////////////////////////////
void HAL_MIU_MaskReq(MS_U8 u8Miu, eMIUClientID eClientID)
{
    MS_S16 sVal;

    sVal = HAL_MIU_GetClientInfo(u8Miu, eClientID);
    if (sVal < 0)
        printf("%s not support client ID:%u!\n", __FUNCTION__, eClientID);
    else
    {
        MS_U32 u32Reg = REG_MIU_RQX_MASK(MIU_GET_CLIENT_GROUP(sVal));

        u32Reg += (u8Miu == 0) ? MIU_REG_BASE : MIU1_REG_BASE;
        HAL_MIU_Write2BytesBit(u32Reg, TRUE, BIT(MIU_GET_CLIENT_POS(sVal)));
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_UnMaskReq()
/// @brief \b Function \b Description: Mask MIU request
/// @param u8Miu  IN        \b  : miu0 or miu1
/// @param eClientID IN      \b  : client ID
/// @param None   OUT       \b  :
/// @param None   RET       \b  :
/// @param None   GLOBAL    \b  :
////////////////////////////////////////////////////////////////////////////////
void HAL_MIU_UnMaskReq(MS_U8 u8Miu, eMIUClientID eClientID)
{
    MS_S16 sVal;

    sVal = HAL_MIU_GetClientInfo(u8Miu, eClientID);
    if (sVal < 0)
        printf("%s not support client ID:%u!\n", __FUNCTION__, eClientID);
    else
    {
        MS_U32 u32Reg = REG_MIU_RQX_MASK(MIU_GET_CLIENT_GROUP(sVal));

        u32Reg += (u8Miu == 0) ? MIU_REG_BASE : MIU1_REG_BASE;
        HAL_MIU_Write2BytesBit(u32Reg, FALSE, BIT(MIU_GET_CLIENT_POS(sVal)));
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_SelMIU()
/// @brief \b Function \b Description: MIU selection
/// @param u8MiuDev	   IN   \b  : miu device
/// @param u16ClientID IN   \b  : client ID
/// @param None   OUT       \b  :
/// @param None   RET       \b  :
/// @param None   GLOBAL    \b  :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_MIU_SelMIU(eMIU_SelType eType, eMIUClientID eClientID)
{
    MS_S16 sVal;
    MS_U32 u32Reg = 0;

    //MIU 0
    sVal = HAL_MIU_GetClientInfo(0, eClientID);
    if (sVal < 0)
        goto fail;

    u32Reg = MIU_REG_BASE + REG_MIU_SELX(MIU_GET_CLIENT_GROUP(sVal));
    HAL_MIU_Write2BytesBit(u32Reg, (MIU_SELTYPE_MIU1 == eType), BIT(MIU_GET_CLIENT_POS(sVal)));

    //MIU 1
    sVal = HAL_MIU_GetClientInfo(1, eClientID);
    if (sVal < 0)
        goto fail;
    u32Reg = MIU1_REG_BASE + REG_MIU_SELX(MIU_GET_CLIENT_GROUP(sVal));
    HAL_MIU_Write2BytesBit(u32Reg, (MIU_SELTYPE_MIU_ALL == eType), BIT(MIU_GET_CLIENT_POS(sVal)));

    return TRUE;

fail:
	printf("%s not support client ID:%u!\n", __FUNCTION__, eClientID);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_Mask_Req_OPM_R()
/// @brief \b Function \b Description: Set OPM MIU mask
/// @param u8Mask IN        \b  : miu mask
/// @param u8Miu  IN        \b  : miu0 or miu1
/// @param None   OUT       \b  :
/// @param None   RET       \b  :
/// @param None   GLOBAL    \b  :
////////////////////////////////////////////////////////////////////////////////
void HAL_MIU_Mask_Req_OPM_R(MS_U8 u8Mask, MS_U8 u8Miu)
{

    if(u8Miu == 0)
        _MaskMiuReq_OPM_R(u8Mask);
    else
       _MaskMiu1Req_OPM_R(u8Mask);

}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_Mask_Req_DNRB_R()
/// @brief \b Function \b Description: Set OPM MIU mask
/// @param u8Mask IN        \b  : miu mask
/// @param u8Miu  IN        \b  : miu0 or miu1
/// @param None   OUT       \b  :
/// @param None   RET       \b  :
/// @param None   GLOBAL    \b  :
////////////////////////////////////////////////////////////////////////////////
void HAL_MIU_Mask_Req_DNRB_R(MS_U8 u8Mask, MS_U8 u8Miu)
{

    if (u8Miu == 0)
        _MaskMiuReq_DNRB_R(u8Mask);
    else
       _MaskMiu1Req_DNRB_R(u8Mask);
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_Mask_Req_DNRB_W()
/// @brief \b Function \b Description: Set OPM MIU mask
/// @param u8Mask IN        \b  : miu mask
/// @param u8Miu  IN        \b  : miu0 or miu1
/// @param None   OUT       \b  :
/// @param None   RET       \b  :
/// @param None   GLOBAL    \b  :
////////////////////////////////////////////////////////////////////////////////
void HAL_MIU_Mask_Req_DNRB_W(MS_U8 u8Mask, MS_U8 u8Miu)
{

    if (u8Miu == 0)
        _MaskMiuReq_DNRB_W(u8Mask);
    else
       _MaskMiu1Req_DNRB_W(u8Mask);
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_Mask_Req_DNRB_RW()
/// @brief \b Function \b Description: Set OPM MIU mask
/// @param u8Mask IN        \b  : miu mask
/// @param u8Miu  IN        \b  : miu0 or miu1
/// @param None   OUT       \b  :
/// @param None   RET       \b  :
/// @param None   GLOBAL    \b  :
////////////////////////////////////////////////////////////////////////////////
void HAL_MIU_Mask_Req_DNRB_RW(MS_U8 u8Mask, MS_U8 u8Miu)
{
    if (u8Miu == 0)
        _MaskMiuReq_DNRB_RW(u8Mask);
    else
       _MaskMiu1Req_DNRB_RW(u8Mask);

}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_Mask_Req_SC_RW()
/// @brief \b Function \b Description: Set OPM MIU mask
/// @param u8Mask IN        \b  : miu mask
/// @param u8Miu  IN        \b  : miu0 or miu1
/// @param None   OUT       \b  :
/// @param None   RET       \b  :
/// @param None   GLOBAL    \b  :
////////////////////////////////////////////////////////////////////////////////
void HAL_MIU_Mask_Req_SC_RW(MS_U8 u8Mask, MS_U8 u8Miu)
{

    if (u8Miu == 0)
        _MaskMiuReq_SC_RW(u8Mask);
    else
        _MaskMiu1Req_SC_RW(u8Mask);

}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_Mask_Req_MVOP_R()
/// @brief \b Function \b Description: Set OPM MIU mask
/// @param u8Mask IN        \b  : miu mask
/// @param u8Miu  IN        \b  : miu0 or miu1
/// @param None   OUT       \b  :
/// @param None   RET       \b  :
/// @param None   GLOBAL    \b  :
////////////////////////////////////////////////////////////////////////////////
void HAL_MIU_Mask_Req_MVOP_R(MS_U8 u8Mask, MS_U8 u8Miu)
{
    HAL_MIU_MaskReq(u8Miu, MIU_CLIENT_MVOP_64BIT_R);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_Mask_Req_MVD_R()
/// @brief \b Function \b Description: Set OPM MIU mask
/// @param u8Mask IN        \b  : miu mask
/// @param u8Miu  IN        \b  : miu0 or miu1
/// @param None   OUT       \b  :
/// @param None   RET       \b  :
/// @param None   GLOBAL    \b  :
////////////////////////////////////////////////////////////////////////////////
void HAL_MIU_Mask_Req_MVD_R(MS_U8 u8Mask, MS_U8 u8Miu)
{
    _FUNC_NOT_USED();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_Mask_Req_MVD_W()
/// @brief \b Function \b Description: Set OPM MIU mask
/// @param u8Mask IN        \b  : miu mask
/// @param u8Miu  IN        \b  : miu0 or miu1
/// @param None   OUT       \b  :
/// @param None   RET       \b  :
/// @param None   GLOBAL    \b  :
////////////////////////////////////////////////////////////////////////////////
void HAL_MIU_Mask_Req_MVD_W(MS_U8 u8Mask, MS_U8 u8Miu)
{
    _FUNC_NOT_USED();
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_Mask_Req_MVD_RW()
/// @brief \b Function \b Description: Set OPM MIU mask
/// @param u8Mask IN        \b  : miu mask
/// @param u8Miu  IN        \b  : miu0 or miu1
/// @param None   OUT       \b  :
/// @param None   RET       \b  :
/// @param None   GLOBAL    \b  :
////////////////////////////////////////////////////////////////////////////////
void HAL_MIU_Mask_Req_MVD_RW(MS_U8 u8Mask, MS_U8 u8Miu)
{
    HAL_MIU_MaskReq(u8Miu, MIU_CLIENT_MVD_RW);
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_Mask_Req_AUDIO_RW()
/// @brief \b Function \b Description: Set OPM MIU mask
/// @param u8Mask IN        \b  : miu mask
/// @param u8Miu  IN        \b  : miu0 or miu1
/// @param None   OUT       \b  :
/// @param None   RET       \b  :
/// @param None   GLOBAL    \b  :
////////////////////////////////////////////////////////////////////////////////
void HAL_MIU_Mask_Req_AUDIO_RW(MS_U8 u8Mask, MS_U8 u8Miu)
{
    if (u8Miu == 0)
        _MaskMiuReq_AUDIO_RW(u8Mask);
    else
        _MaskMiu1Req_AUDIO_RW(u8Mask);

}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_GET_MUX()
/// @brief \b Function \b Description:
/// @param None IN        \b  :
/// @param None IN        \b  :
/// @param None OUT       \b  :
/// @param None RET       \b  :
/// @param None GLOBAL    \b  :
////////////////////////////////////////////////////////////////////////////////
MS_U16 HAL_MIU_GET_MUX(void)
{
    return 0x0000;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_SwitchMIU()
/// @brief \b Function \b Description:
/// @param u8MiuID        \b IN     : select MIU0 or MIU1
/// @param None \b RET:
////////////////////////////////////////////////////////////////////////////////
void HAL_MIU_VOP_SwitchMIU(MS_U8 u8MiuID)
{
	HAL_MIU_SelMIU((eMIU_SelType)u8MiuID, MIU_CLIENT_MVOP_64BIT_R);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_IsI64Mode()
/// @brief \b Function \b Description:
/// @param None \b RET: 0: not support, 64 or 128 bits
////////////////////////////////////////////////////////////////////////////////
MS_U16 HAL_MIU_IsI64Mode(void)
{
    return (HAL_MIU_ReadByte(MIU_REG_BASE+0x04) & REG_MIU_I64_MODE) ? (64) : (128);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_IsInitMiu1()
/// @brief \b Function \b Description:
/// @param None \b RET:
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_MIU_IsInitMiu1(void)
{
    return (HAL_MIU_Read2Byte(MIU1_REG_BASE) & REG_MIU_INIT_DONE) ? TRUE : FALSE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_MIU_SetGroupPriority()
/// @brief \b Function  \b Description:  This function for set each group priority
/// @param u8MiuDev     \b IN   : select MIU0 or MIU1
/// @param sPriority    \b IN   : gropu priority
/// @param None \b RET:   0: Fail 1: Ok
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_MIU_SetGroupPriority(MS_U8 u8MiuDev, MIU_GroupPriority sPriority)
{
    MS_U8 u8Val = 0;
    MS_U32 u32RegAddr = REG_MIU_GROUP_PRIORITY;

    u8Val = (sPriority.u84th << 6 | sPriority.u83rd << 4 | sPriority.u82nd << 2 | sPriority.u81st);
    printf("Change miu%u group priority:%x\n", u8MiuDev, u8Val);

	u32RegAddr += (u8MiuDev) ? MIU1_REG_BASE : MIU_REG_BASE;
	HAL_MIU_Write2BytesBit(u32RegAddr,DISABLE, BIT8);
	HAL_MIU_WriteByte(u32RegAddr, u8Val);
	HAL_MIU_Write2BytesBit(u32RegAddr,ENABLE, BIT8);
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_MIU_SetGroupPriority()
/// @brief \b Function  \b Description:  This function for set each group priority
/// @param u8MiuDev     \b IN   : select MIU0 or MIU1
/// @param eClientID    \b IN   : client ID
/// @param bMask        \b IN   : TRUE: Mask high priority FALSE: Unmask hih priority
/// @param None \b RET:   0: Fail 1: Ok
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_MIU_SetHPriorityMask(MS_U8 u8MiuDev, eMIUClientID eClientID, MS_BOOL bMask)
{
    MS_S16 sVal;

    sVal = HAL_MIU_GetClientInfo(u8MiuDev, eClientID);
    if (sVal < 0)
    {
        printf("%s not support client ID:%u!\n", __FUNCTION__, eClientID);
        return FALSE;
    }
    else
    {
        MS_U32 u32Reg = REG_MIU_RQX_HPMASK(MIU_GET_CLIENT_GROUP(sVal));

        u32Reg += (u8MiuDev == 0) ? MIU_REG_BASE : MIU1_REG_BASE;
        HAL_MIU_Write2BytesBit(u32Reg, bMask, BIT(MIU_GET_CLIENT_POS(sVal)));
    }
    return TRUE;
}

MS_BOOL HAL_MIU_GetAutoPhaseResult(MS_U32 *miu0, MS_U32 *miu1)
{
    static MS_U32 u32Miu0 = 0, u32Miu1 = 0;

    if (!u32Miu0)
    {
        u32Miu0 = HAL_MIU_Read4Byte(0x3390);    //miu0 result
        u32Miu1 = HAL_MIU_Read4Byte(0x3394);    //miu1 result
        *miu0 = u32Miu0;
        *miu1 = u32Miu1;
    }
    else
    {
        *miu0 = u32Miu0;
        *miu1 = u32Miu1;
    }
    return TRUE;
}

