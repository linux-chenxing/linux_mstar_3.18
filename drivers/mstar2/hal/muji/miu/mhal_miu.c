///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2007 MStar Semiconductor, Inc.
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
/// @file   Mhal_mtlb.c
/// @brief  MTLB Driver
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include files
//-------------------------------------------------------------------------------------------------
#include <linux/printk.h>
#include "MsTypes.h"
#include "mdrv_types.h"
#include "mdrv_miu.h"
#include "regMIU.h"
#include "mhal_miu.h"
#if defined(CONFIG_COMPAT)
#include <linux/compat.h>
#endif
#include "mdrv_system.h"

#if defined(CONFIG_MIPS)
#include <asm/mips-boards/prom.h>
#include "mhal_chiptop_reg.h"
#elif defined(CONFIG_ARM)
#include <prom.h>
#include <asm/mach/map.h>
#elif defined(CONFIG_ARM64)
#include <asm/arm-boards/prom.h>
#include <asm/mach/map.h>
#endif
#include "chip_setup.h"

//-------------------------------------------------------------------------------------------------
//  Define
//-------------------------------------------------------------------------------------------------
#define MIU_CLIENT_GP0  \
    MIU_CLIENT_NONE, \
    MIU_CLIENT_VIVALDI9_DECODER_R, \
    MIU_CLIENT_SECAU_R2_RW, \
    MIU_CLIENT_USB3_RW,\
    MIU_CLIENT_SECURE_R2_RW,\
    MIU_CLIENT_AU_R2_RW, \
    MIU_CLIENT_VD_R2D_RW,\
    MIU_CLIENT_PM51_RW, \
    MIU_CLIENT_VD_R2I_R, \
    MIU_CLIENT_USB_UHC0_RW, \
    MIU_CLIENT_USB_UHC1_RW, \
    MIU_CLIENT_USB_UHC2_RW, \
    MIU_CLIENT_MVD_BBU_RW, \
    MIU_CLIENT_EMAC_RW, \
    MIU_CLIENT_BDMA_RW, \
    MIU_CLIENT_DUMMY

#define MIU_CLIENT_GP1  \
    MIU_CLIENT_VIVALDI9_MAD_RW, \
    MIU_CLIENT_DEMOD_W, \
    MIU_CLIENT_DEMOD_R, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_SC_DIPW_RW, \
    MIU_CLIENT_SC_LOCALDIMING_RW, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_TSP_W, \
    MIU_CLIENT_TSP_PVR0_W, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_VD_TTXSL_W, \
    MIU_CLIENT_VD_COMB_W, \
    MIU_CLIENT_VD_COMB_R, \
    MIU_CLIENT_VE_W, \
    MIU_CLIENT_VIF_ADC_W

#define MIU_CLIENT_GP2  \
    MIU_CLIENT_CMD_QUEUE_RW, \
    MIU_CLIENT_HVD_BBU_R, \
    MIU_CLIENT_T3D_W, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_MVD_RW, \
    MIU_CLIENT_G3D_RW, \
    MIU_CLIENT_GPD_RW, \
    MIU_CLIENT_MFE0_W, \
    MIU_CLIENT_MFE1_R, \
    MIU_CLIENT_NAND_RW, \
    MIU_CLIENT_SDIO_RW, \
    MIU_CLIENT_DSCRMB_RW, \
    MIU_CLIENT_TSP_FIQ_RW, \
    MIU_CLIENT_TSP_ORZ_W, \
    MIU_CLIENT_TSP_ORZ_R,\
    MIU_CLIENT_TSO_RW

#define MIU_CLIENT_GP3  \
    MIU_CLIENT_EVD_R2D_RW, \
    MIU_CLIENT_EVD_R2I_R, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_UART_DMA_RW, \
    MIU_CLIENT_ZDEC_RW, \
    MIU_CLIENT_ZDEC_ACP_RW, \
    MIU_CLIENT_DUMMY,\
    MIU_CLIENT_USB30_1_RW,\
    MIU_CLIENT_USB30_2_RW,\
    MIU_CLIENT_DUMMY,\
    MIU_CLIENT_JPD720P_RW,\
    MIU_CLIENT_DUMMY,\
    MIU_CLIENT_JPD_RW,\
    MIU_CLIENT_DUMMY,\
    MIU_CLIENT_MIIC0_RW,\
    MIU_CLIENT_DUMMY

#define MIU_CLIENT_GP4  \
    MIU_CLIENT_MIPS_RW, \
    MIU_CLIENT_GE_RW, \
    MIU_CLIENT_HVD_RW, \
    MIU_CLIENT_SECHVD_RW, \
    MIU_CLIENT_3RDHVD_RW, \
    MIU_CLIENT_VP9_RW, \
    MIU_CLIENT_DUMMY,\
    MIU_CLIENT_EVD_RW,\
    MIU_CLIENT_EVD_MTFC_W, \
    MIU_CLIENT_EVD_MTFY_W, \
    MIU_CLIENT_MVOP_128BIT_R,\
    MIU_CLIENT_MVOP1_R,\
    MIU_CLIENT_GOP3_PDW0_RW,\
    MIU_CLIENT_MFDEC_R,\
    MIU_CLIENT_SECMFDEC_R,\
    MIU_CLIENT_MIUTEST_R

#define MIU_CLIENT_GP5  \
    MIU_CLIENT_PDW1_RW, \
    MIU_CLIENT_GOP0_R, \
    MIU_CLIENT_GOP1_R, \
    MIU_CLIENT_GOP2_R, \
    MIU_CLIENT_GOP3_PDW0_RW, \
    MIU_CLIENT_FRC_R, \
    MIU_CLIENT_SC_IPMAIN_RW, \
    MIU_CLIENT_SC1_IPMAIN_RW, \
    MIU_CLIENT_SC_OPMAIN_RW,\
    MIU_CLIENT_FRCM_W,\
    MIU_CLIENT_SC_IPMAIN2_RW,\
    MIU_CLIENT_SC2_OPMAIN_RW,\
    MIU_CLIENT_SC_OD_RW,\
    MIU_CLIENT_SC_OPSUB_W,\
    MIU_CLIENT_SC_IPSUB_RW, \
    MIU_CLIENT_FRCS_W

#define MIU1_CLIENT_GP0  \
    MIU_CLIENT_NONE, \
    MIU_CLIENT_VIVALDI9_DECODER_R, \
    MIU_CLIENT_SECAU_R2_RW, \
    MIU_CLIENT_USB3_RW,\
    MIU_CLIENT_SECURE_R2_RW,\
    MIU_CLIENT_AU_R2_RW, \
    MIU_CLIENT_VD_R2D_RW,\
    MIU_CLIENT_PM51_RW, \
    MIU_CLIENT_VD_R2I_R, \
    MIU_CLIENT_USB_UHC0_RW, \
    MIU_CLIENT_USB_UHC1_RW, \
    MIU_CLIENT_USB_UHC2_RW, \
    MIU_CLIENT_MVD_BBU_RW, \
    MIU_CLIENT_EMAC_RW, \
    MIU_CLIENT_BDMA_RW, \
    MIU_CLIENT_DUMMY

#define MIU1_CLIENT_GP1  \
    MIU_CLIENT_VIVALDI9_MAD_RW, \
    MIU_CLIENT_DEMOD_W, \
    MIU_CLIENT_DEMOD_R, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_SC_DIPW_RW, \
    MIU_CLIENT_SC_LOCALDIMING_RW, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_TSP_W, \
    MIU_CLIENT_TSP_PVR0_W, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_VD_TTXSL_W, \
    MIU_CLIENT_VD_COMB_W, \
    MIU_CLIENT_VD_COMB_R, \
    MIU_CLIENT_VE_W, \
    MIU_CLIENT_VIF_ADC_W

#define MIU1_CLIENT_GP2  \
    MIU_CLIENT_CMD_QUEUE_RW, \
    MIU_CLIENT_HVD_BBU_R, \
    MIU_CLIENT_T3D_W, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_MVD_RW, \
    MIU_CLIENT_G3D_RW, \
    MIU_CLIENT_GPD_RW, \
    MIU_CLIENT_MFE0_W, \
    MIU_CLIENT_MFE1_R, \
    MIU_CLIENT_NAND_RW, \
    MIU_CLIENT_SDIO_RW, \
    MIU_CLIENT_DSCRMB_RW, \
    MIU_CLIENT_TSP_FIQ_RW, \
    MIU_CLIENT_TSP_ORZ_W, \
    MIU_CLIENT_TSP_ORZ_R,\
    MIU_CLIENT_TSO_RW

#define MIU1_CLIENT_GP3  \
    MIU_CLIENT_EVD_R2D_RW, \
    MIU_CLIENT_EVD_R2I_R, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_UART_DMA_RW, \
    MIU_CLIENT_ZDEC_RW, \
    MIU_CLIENT_ZDEC_ACP_RW, \
    MIU_CLIENT_DUMMY,\
    MIU_CLIENT_USB30_1_RW,\
    MIU_CLIENT_USB30_2_RW,\
    MIU_CLIENT_DUMMY,\
    MIU_CLIENT_JPD720P_RW,\
    MIU_CLIENT_DUMMY,\
    MIU_CLIENT_JPD_RW,\
    MIU_CLIENT_DUMMY,\
    MIU_CLIENT_MIIC0_RW,\
    MIU_CLIENT_DUMMY

#define MIU1_CLIENT_GP4  \
    MIU_CLIENT_MIPS_RW, \
    MIU_CLIENT_GE_RW, \
    MIU_CLIENT_HVD_RW, \
    MIU_CLIENT_SECHVD_RW, \
    MIU_CLIENT_3RDHVD_RW, \
    MIU_CLIENT_VP9_RW, \
    MIU_CLIENT_DUMMY,\
    MIU_CLIENT_EVD_RW,\
    MIU_CLIENT_EVD_MTFC_W, \
    MIU_CLIENT_EVD_MTFY_W, \
    MIU_CLIENT_MVOP_128BIT_R,\
    MIU_CLIENT_MVOP1_R,\
    MIU_CLIENT_GOP3_PDW0_RW,\
    MIU_CLIENT_MFDEC_R,\
    MIU_CLIENT_SECMFDEC_R,\
    MIU_CLIENT_MIUTEST_R

#define MIU1_CLIENT_GP5  \
    MIU_CLIENT_PDW1_RW, \
    MIU_CLIENT_GOP0_R, \
    MIU_CLIENT_GOP1_R, \
    MIU_CLIENT_GOP2_R, \
    MIU_CLIENT_GOP3_PDW0_RW, \
    MIU_CLIENT_FRC_R, \
    MIU_CLIENT_SC_IPMAIN_RW, \
    MIU_CLIENT_SC1_IPMAIN_RW, \
    MIU_CLIENT_SC_OPMAIN_RW,\
    MIU_CLIENT_FRCM_W,\
    MIU_CLIENT_SC_IPMAIN2_RW,\
    MIU_CLIENT_SC2_OPMAIN_RW,\
    MIU_CLIENT_SC_OD_RW,\
    MIU_CLIENT_SC_OPSUB_W,\
    MIU_CLIENT_SC_IPSUB_RW, \
    MIU_CLIENT_FRCS_W

#define MIU2_CLIENT_GP0  \
    MIU_CLIENT_NONE, \
    MIU_CLIENT_VIVALDI9_DECODER_R, \
    MIU_CLIENT_SECAU_R2_RW, \
    MIU_CLIENT_USB3_RW,\
    MIU_CLIENT_SECURE_R2_RW,\
    MIU_CLIENT_AU_R2_RW, \
    MIU_CLIENT_VD_R2D_RW,\
    MIU_CLIENT_PM51_RW, \
    MIU_CLIENT_VD_R2I_R, \
    MIU_CLIENT_USB_UHC0_RW, \
    MIU_CLIENT_USB_UHC1_RW, \
    MIU_CLIENT_USB_UHC2_RW, \
    MIU_CLIENT_MVD_BBU_RW, \
    MIU_CLIENT_EMAC_RW, \
    MIU_CLIENT_BDMA_RW, \
    MIU_CLIENT_DUMMY

#define MIU2_CLIENT_GP1  \
    MIU_CLIENT_VIVALDI9_MAD_RW, \
    MIU_CLIENT_DEMOD_W, \
    MIU_CLIENT_DEMOD_R, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_SC_DIPW_RW, \
    MIU_CLIENT_SC_LOCALDIMING_RW, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_TSP_W, \
    MIU_CLIENT_TSP_PVR0_W, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_VD_TTXSL_W, \
    MIU_CLIENT_VD_COMB_W, \
    MIU_CLIENT_VD_COMB_R, \
    MIU_CLIENT_VE_W, \
    MIU_CLIENT_VIF_ADC_W

#define MIU2_CLIENT_GP2  \
    MIU_CLIENT_CMD_QUEUE_RW, \
    MIU_CLIENT_HVD_BBU_R, \
    MIU_CLIENT_T3D_W, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_MVD_RW, \
    MIU_CLIENT_G3D_RW, \
    MIU_CLIENT_GPD_RW, \
    MIU_CLIENT_MFE0_W, \
    MIU_CLIENT_MFE1_R, \
    MIU_CLIENT_NAND_RW, \
    MIU_CLIENT_SDIO_RW, \
    MIU_CLIENT_DSCRMB_RW, \
    MIU_CLIENT_TSP_FIQ_RW, \
    MIU_CLIENT_TSP_ORZ_W, \
    MIU_CLIENT_TSP_ORZ_R,\
    MIU_CLIENT_TSO_RW

#define MIU2_CLIENT_GP3  \
    MIU_CLIENT_EVD_R2D_RW, \
    MIU_CLIENT_EVD_R2I_R, \
    MIU_CLIENT_DUMMY, \
    MIU_CLIENT_UART_DMA_RW, \
    MIU_CLIENT_ZDEC_RW, \
    MIU_CLIENT_ZDEC_ACP_RW, \
    MIU_CLIENT_DUMMY,\
    MIU_CLIENT_USB30_1_RW,\
    MIU_CLIENT_USB30_2_RW,\
    MIU_CLIENT_DUMMY,\
    MIU_CLIENT_JPD720P_RW,\
    MIU_CLIENT_DUMMY,\
    MIU_CLIENT_JPD_RW,\
    MIU_CLIENT_DUMMY,\
    MIU_CLIENT_MIIC0_RW,\
    MIU_CLIENT_DUMMY

#define MIU2_CLIENT_GP4  \
    MIU_CLIENT_MIPS_RW, \
    MIU_CLIENT_GE_RW, \
    MIU_CLIENT_HVD_RW, \
    MIU_CLIENT_SECHVD_RW, \
    MIU_CLIENT_3RDHVD_RW, \
    MIU_CLIENT_VP9_RW, \
    MIU_CLIENT_DUMMY,\
    MIU_CLIENT_EVD_RW,\
    MIU_CLIENT_EVD_MTFC_W, \
    MIU_CLIENT_EVD_MTFY_W, \
    MIU_CLIENT_MVOP_128BIT_R,\
    MIU_CLIENT_MVOP1_R,\
    MIU_CLIENT_GOP3_PDW0_RW,\
    MIU_CLIENT_MFDEC_R,\
    MIU_CLIENT_SECMFDEC_R,\
    MIU_CLIENT_MIUTEST_R

#define MIU2_CLIENT_GP5  \
    MIU_CLIENT_PDW1_RW, \
    MIU_CLIENT_GOP0_R, \
    MIU_CLIENT_GOP1_R, \
    MIU_CLIENT_GOP2_R, \
    MIU_CLIENT_GOP3_PDW0_RW, \
    MIU_CLIENT_FRC_R, \
    MIU_CLIENT_SC_IPMAIN_RW, \
    MIU_CLIENT_SC1_IPMAIN_RW, \
    MIU_CLIENT_SC_OPMAIN_RW,\
    MIU_CLIENT_FRCM_W,\
    MIU_CLIENT_SC_IPMAIN2_RW,\
    MIU_CLIENT_SC2_OPMAIN_RW,\
    MIU_CLIENT_SC_OD_RW,\
    MIU_CLIENT_SC_OPSUB_W,\
    MIU_CLIENT_SC_IPSUB_RW, \
    MIU_CLIENT_FRCS_W

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------
#ifdef CONFIG_CMA
#define IDNUM_KERNELPROTECT (14)
#else
#define IDNUM_KERNELPROTECT (12)
#endif

const eMIUClientID clientTbl[MIU_MAX_DEVICE][MIU_MAX_TBL_CLIENT] =
{
    {MIU_CLIENT_GP0, MIU_CLIENT_GP1, MIU_CLIENT_GP2, MIU_CLIENT_GP3, MIU_CLIENT_GP4, MIU_CLIENT_GP5}
    ,{MIU1_CLIENT_GP0, MIU1_CLIENT_GP1, MIU1_CLIENT_GP2, MIU1_CLIENT_GP3, MIU1_CLIENT_GP4, MIU1_CLIENT_GP5}
    ,{MIU2_CLIENT_GP0, MIU2_CLIENT_GP1, MIU2_CLIENT_GP2, MIU2_CLIENT_GP3, MIU2_CLIENT_GP4, MIU2_CLIENT_GP5}
};

MS_U8 clientId_KernelProtect[IDNUM_KERNELPROTECT] =
{
    MIU_CLIENT_MIPS_RW, MIU_CLIENT_NAND_RW, MIU_CLIENT_USB_UHC0_RW, MIU_CLIENT_USB_UHC1_RW,
    MIU_CLIENT_USB_UHC2_RW, MIU_CLIENT_G3D_RW, MIU_CLIENT_USB3_RW, MIU_CLIENT_SDIO_RW,
    MIU_CLIENT_SATA_RW, MIU_CLIENT_USB_UHC3_RW, MIU_CLIENT_USB30_1_RW, MIU_CLIENT_USB30_2_RW
#ifdef CONFIG_CMA
    ,MIU_CLIENT_GOP3_PDW0_RW
    ,MIU_CLIENT_GE_RW
#endif
};

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
typedef enum
{
  E_CHIP_MIU_0 = 0,
  E_CHIP_MIU_1,
  E_CHIP_MIU_2,
  E_CHIP_MIU_3,
  E_CHIP_MIU_NUM,
} CHIP_MIU_ID;

//-------------------------------------------------------------------------------------------------
//  Macros
//-------------------------------------------------------------------------------------------------
#define _phy_to_miu_offset(MiuSel, Offset, PhysAddr) if (PhysAddr < ARM_MIU1_BASE_ADDR) \
                                                        {MiuSel = E_CHIP_MIU_0; Offset = PhysAddr;} \
                                                     else if ((PhysAddr >= ARM_MIU1_BASE_ADDR) && (PhysAddr < ARM_MIU2_BASE_ADDR)) \
                                                         {MiuSel = E_CHIP_MIU_1; Offset = PhysAddr - ARM_MIU1_BASE_ADDR;} \
                                                     else \
                                                         {MiuSel = E_CHIP_MIU_2; Offset = PhysAddr - ARM_MIU2_BASE_ADDR;}

#define _miu_offset_to_phy(MiuSel, Offset, PhysAddr) if (MiuSel == E_CHIP_MIU_0) \
                                                        {PhysAddr = Offset;} \
                                                     else if (MiuSel == E_CHIP_MIU_1) \
                                                         {PhysAddr = Offset + ARM_MIU1_BASE_ADDR;} \
                                                     else \
                                                         {PhysAddr = Offset + ARM_MIU2_BASE_ADDR;}

#define MIU_HAL_ERR(fmt, args...)   printk(KERN_ERR "miu hal error %s:%d" fmt,__FUNCTION__,__LINE__,## args)

//-------------------------------------------------------------------------------------------------
//  Local Variable
//-------------------------------------------------------------------------------------------------
//static MS_U32 _gMIU_MapBase = 0xBF200000;      //default set to MIPS platfrom
#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
static MS_U32 _gMIU_MapBase = 0xFD200000UL;   //default set to arm 32bit platfrom
#elif defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
static ptrdiff_t _gMIU_MapBase;
#endif

MS_BOOL IDEnables[MIU_MAX_DEVICE][MIU_MAX_PROTECT_BLOCK][MIU_MAX_PROTECT_ID] = {{{0},{0},{0},{0},{0}}, {{0},{0},{0},{0},{0}}, {{0},{0},{0},{0},{0}}}; //ID enable for protect block 0~3
MS_U8 IDs[MIU_MAX_DEVICE][MIU_MAX_PROTECT_ID] = {{0}, {0}, {0}}; //IDs for protection

static SRAM_Slit_FlagGroup *_gSlit0_FlagGroupBase;
static SRAM_Slit_FlagGroup *_gSlit1_FlagGroupBase;

//-------------------------------------------------------------------------------------------------
//  MTLB HAL internal function
//-------------------------------------------------------------------------------------------------
MS_U32 HAL_MIU_BA2PA(MS_U32 u32BusAddr)
{
    MS_PHYADDR u32PhyAddr = 0x0UL;

    // pa = ba - offset
    if( (u32BusAddr >= ARM_MIU0_BUS_BASE) && (u32BusAddr < ARM_MIU1_BUS_BASE) )    // MIU0
        u32PhyAddr = u32BusAddr - ARM_MIU0_BUS_BASE + ARM_MIU0_BASE_ADDR;
    else if( (u32BusAddr >= ARM_MIU1_BUS_BASE) && (u32BusAddr < ARM_MIU2_BUS_BASE))    // MIU1
        u32PhyAddr = u32BusAddr - ARM_MIU1_BUS_BASE + ARM_MIU1_BASE_ADDR;
    else
        u32PhyAddr = u32BusAddr - ARM_MIU2_BUS_BASE + ARM_MIU2_BASE_ADDR;    // MIU2

    return u32PhyAddr;
}

MS_S16 HAL_MIU_GetClientInfo(MS_U8 u8MiuDev, eMIUClientID eClientID)
{
    MS_U8 idx;

    if (MIU_MAX_DEVICE <= u8MiuDev)
    {
        MIU_HAL_ERR("Wrong MIU device:%u\n", u8MiuDev);
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
#if defined(CONFIG_ARM64)
    _gMIU_MapBase = (mstar_pm_base + 0x00200000UL);
#endif
    return ((volatile MS_U8*)(_gMIU_MapBase))[(u32RegAddr << 1) - (u32RegAddr & 1)];
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
#if defined(CONFIG_ARM64)
    _gMIU_MapBase = (mstar_pm_base + 0x00200000UL);
#endif
    return ((volatile MS_U16*)(_gMIU_MapBase))[u32RegAddr];
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

#if defined(CONFIG_ARM64)
    _gMIU_MapBase = (mstar_pm_base + 0x00200000UL);
#endif
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

#if defined(CONFIG_ARM64)
    _gMIU_MapBase = (mstar_pm_base + 0x00200000UL);
#endif
    ((volatile MS_U16*)(_gMIU_MapBase))[u32RegAddr] = u16Val;
    return TRUE;
}

void HAL_MIU_SetProtectID(MS_U32 u32Reg, MS_U8 u8MiuDev, MS_U8 u8ClientID)
{
    MS_S16 sVal = HAL_MIU_GetClientInfo(u8MiuDev, (eMIUClientID)u8ClientID);
    MS_S16 sIDVal;

    if (0 > sVal)
        sVal = 0;

    sIDVal = HAL_MIU_ReadByte(u32Reg);
    sIDVal &= 0x80;
    sIDVal |= sVal;
    HAL_MIU_WriteByte(u32Reg, sIDVal);
}

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

MS_BOOL HAL_MIU_SetGroupID(MS_U8 u8MiuSel, MS_U8 u8Blockx, MS_U8 *pu8ProtectId, MS_U32 u32RegAddrID, MS_U32 u32RegAddrIDenable)
{
    MS_U32 u32index0, u32index1;
    MS_U8 u8ID;
    MS_U8 u8isfound0, u8isfound1;
    MS_U16 u16idenable;

    //reset IDenables for protect u8Blockx
    for(u32index0 = 0; u32index0 < MIU_MAX_PROTECT_ID; u32index0++)
    {
        IDEnables[u8MiuSel][u8Blockx][u32index0] = 0;
    }

    for(u32index0 = 0; u32index0 < MIU_MAX_PROTECT_ID; u32index0++)
    {
        u8ID = pu8ProtectId[u32index0];

        //Unused ID
        if(u8ID == 0)
           continue;

        u8isfound0 = FALSE;

        for(u32index1 = 0; u32index1 < MIU_MAX_PROTECT_ID; u32index1++)
        {
            if(IDs[u8MiuSel][u32index1] == u8ID)
            {
                //ID reused former setting
                IDEnables[u8MiuSel][u8Blockx][u32index1] = 1;
                u8isfound0 = TRUE;
                break;
            }
        }

        //Need to create new ID in IDs
        if(u8isfound0 != TRUE)
        {
            u8isfound1 = FALSE;

            for(u32index1 = 0; u32index1 < MIU_MAX_PROTECT_ID; u32index1++)
            {
                if(IDs[u8MiuSel][u32index1] == 0)
                {
                    IDs[u8MiuSel][u32index1] = u8ID;
                    IDEnables[u8MiuSel][u8Blockx][u32index1] = 1;
                    u8isfound1 = TRUE;
                    break;
                }
            }

            //ID overflow
            if(u8isfound1 == FALSE)
                return FALSE;
        }
    }

    u16idenable = 0;

    for(u32index0 = 0; u32index0 < MIU_MAX_PROTECT_ID; u32index0++)
    {
        if(IDEnables[u8MiuSel][u8Blockx][u32index0] == 1)
            u16idenable |= (1<<u32index0);
    }

    HAL_MIU_Write2Byte(u32RegAddrIDenable, u16idenable);

    for(u32index0 = 0; u32index0 < MIU_MAX_PROTECT_ID; u32index0++)
    {
         HAL_MIU_SetProtectID(u32RegAddrID + u32index0, u8MiuSel, IDs[u8MiuSel][u32index0]);
    }

    return TRUE;
}

MS_BOOL HAL_MIU_ResetGroupID(MS_U8 u8MiuSel, MS_U8 u8Blockx, MS_U8 *pu8ProtectId, MS_U32 u32RegAddrID, MS_U32 u32RegAddrIDenable)
{
    MS_U32 u32index0, u32index1;
    MS_U8 u8isIDNoUse;
    MS_U16 u16idenable;

    //reset IDenables for protect u8Blockx
    for(u32index0 = 0; u32index0 < MIU_MAX_PROTECT_ID; u32index0++)
    {
        IDEnables[u8MiuSel][u8Blockx][u32index0] = 0;
    }

    u16idenable = 0x0UL;

    HAL_MIU_Write2Byte(u32RegAddrIDenable, u16idenable);

    for(u32index0 = 0; u32index0 < MIU_MAX_PROTECT_ID; u32index0++)
    {
        u8isIDNoUse  = FALSE;

        for(u32index1 = 0; u32index1 < MIU_MAX_PROTECT_BLOCK; u32index1++)
        {
            if(IDEnables[u8MiuSel][u32index1][u32index0] == 1)
            {
                //protect ID is still be used
                u8isIDNoUse  = FALSE;
                break;
            }
            u8isIDNoUse  = TRUE;
        }

        if(u8isIDNoUse == TRUE)
            IDs[u8MiuSel][u32index0] = 0;
    }

    for(u32index0 = 0; u32index0 < MIU_MAX_PROTECT_ID; u32index0++)
    {
         HAL_MIU_SetProtectID(u32RegAddrID + u32index0, u8MiuSel, IDs[u8MiuSel][u32index0]);
    }

    return TRUE;
}

void HAL_MIU_Write2BytesBit(MS_U32 u32RegOffset, MS_BOOL bEnable, MS_U16 u16Mask)
{
    MS_U16 val = HAL_MIU_Read2Byte(u32RegOffset);
    val = (bEnable) ? (val | u16Mask) : (val & ~u16Mask);
    HAL_MIU_Write2Byte(u32RegOffset, val);
}

//-------------------------------------------------------------------------------------------------
//  MTLB HAL function
//-------------------------------------------------------------------------------------------------

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
/// @brief \b Function \b Name: HAL_MIU_Protect()
/// @brief \b Function \b Description:  Enable/Disable MIU Protection mode
/// @param u8Blockx        \b IN     : MIU Block to protect (0 ~ 4)
/// @param *pu8ProtectId   \b IN     : Allow specified client IDs to write
/// @param u32Start        \b IN     : Starting bus address
/// @param u32End          \b IN     : End bus address
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
                          MS_U32   u32BusStart,
                          MS_U32   u32BusEnd,
                          MS_BOOL  bSetFlag
                         )
{
    MS_U32 u32RegAddr;
    MS_U32 u32Reg;
    MS_U32 u32RegAddrStar;
    MS_U32 u32RegAddrMSB;
    MS_U32 u32RegAddrIDenable;
    MS_U32 u32MiuProtectEn;
    MS_U32 u32StartOffset;
    MS_U32 u32EndOffset;
    MS_U16 u16Data;
    MS_U16 u16Data1;
    MS_U16 u16Data2;
    MS_U8  u8Data;
    MS_U8  u8MiuSel;
    MS_U32 u32Start, u32End;

    u32Start = HAL_MIU_BA2PA(u32BusStart);
    u32End = HAL_MIU_BA2PA(u32BusEnd);

    // Get MIU selection and offset
    _phy_to_miu_offset(u8MiuSel, u32EndOffset, u32End);
    _phy_to_miu_offset(u8MiuSel, u32StartOffset, u32Start);

    u32Start = u32StartOffset;
    u32End = u32EndOffset;

    // Incorrect Block ID
    if(u8Blockx >= E_MIU_BLOCK_NUM)
    {
        MIU_HAL_ERR("Err: Out of the number of protect device\n");
        return FALSE;
    }
    else if(((u32Start & ((1 << MIU_PAGE_SHIFT) -1)) != 0) || ((u32End & ((1 << MIU_PAGE_SHIFT) -1)) != 0))
    {
        MIU_HAL_ERR("Err: Protected address should be aligned to 8KB\n");
        return FALSE;
    }
    else if(u32Start >= u32End)
    {
        MIU_HAL_ERR("Err: Start address is equal to or more than end address\n");
        return FALSE;
    }

    //write_enable
    u8Data = 1 << u8Blockx;

    if(u8MiuSel == E_CHIP_MIU_0)
    {
        u32RegAddrMSB = MIU_PROTECT0_MSB;
        u16Data1 = HAL_MIU_Read2Byte(u32RegAddrMSB);

        u32RegAddr = MIU_PROTECT0_ID0;
        u32MiuProtectEn=MIU_PROTECT_EN_INTERNAL;
        u32Reg = MIU_REG_BASE;

        switch (u8Blockx)
        {
            case E_MIU_BLOCK_0:
                u32RegAddrStar = MIU_PROTECT0_START;
                u32RegAddrIDenable = MIU_PROTECT0_ID_ENABLE;
                u16Data2 = (u16Data1 & 0xFFF0UL);
                break;
            case E_MIU_BLOCK_1:
                u32RegAddrStar = MIU_PROTECT1_START;
                u32RegAddrIDenable = MIU_PROTECT1_ID_ENABLE;
                u16Data2 = (u16Data1 & 0xFF0FUL);
                break;
            case E_MIU_BLOCK_2:
                u32RegAddrStar = MIU_PROTECT2_START;
                u32RegAddrIDenable = MIU_PROTECT2_ID_ENABLE;
                u16Data2 = (u16Data1 & 0xF0FFUL);
                break;
            case E_MIU_BLOCK_3:
                u32RegAddrStar = MIU_PROTECT3_START;
                u32RegAddrIDenable = MIU_PROTECT3_ID_ENABLE;
                u16Data2 = (u16Data1 & 0x0FFFUL);
                break;
            default:
                return FALSE;
        }
    }
    else if(u8MiuSel == E_CHIP_MIU_1)
    {
        u32RegAddrMSB = MIU1_PROTECT0_MSB;
        u16Data1 = HAL_MIU_Read2Byte(u32RegAddrMSB);

        u32RegAddr = MIU1_PROTECT0_ID0;
        u32MiuProtectEn=MIU1_PROTECT_EN;
        u32Reg = MIU1_REG_BASE;

        switch (u8Blockx)
        {
            case E_MIU_BLOCK_0:
                u32RegAddrStar = MIU1_PROTECT0_START;
                u32RegAddrIDenable = MIU1_PROTECT0_ID_ENABLE;
                u16Data2 = (u16Data1 & 0xFFF0UL);
                break;
            case E_MIU_BLOCK_1:
                u32RegAddrStar = MIU1_PROTECT1_START;
                u32RegAddrIDenable = MIU1_PROTECT1_ID_ENABLE;
                u16Data2 = (u16Data1 & 0xFF0FUL);
                break;
            case E_MIU_BLOCK_2:
                u32RegAddrStar = MIU1_PROTECT2_START;
                u32RegAddrIDenable = MIU1_PROTECT2_ID_ENABLE;
                u16Data2 = (u16Data1 & 0xF0FFUL);
                break;
            case E_MIU_BLOCK_3:
                u32RegAddrStar = MIU1_PROTECT3_START;
                u32RegAddrIDenable = MIU1_PROTECT3_ID_ENABLE;
                u16Data2 = (u16Data1 & 0x0FFFUL);
                break;
            default:
                return FALSE;
        }
     }
    else if(u8MiuSel == E_CHIP_MIU_2)
    {
        u32RegAddrMSB = MIU2_PROTECT0_MSB;
        u16Data1 = HAL_MIU_Read2Byte(u32RegAddrMSB);

        u32RegAddr = MIU2_PROTECT0_ID0;
        u32MiuProtectEn=MIU2_PROTECT_EN;
        u32Reg = MIU2_REG_BASE;

        switch (u8Blockx)
        {
            case E_MIU_BLOCK_0:
                u32RegAddrStar = MIU2_PROTECT0_START;
                u32RegAddrIDenable = MIU2_PROTECT0_ID_ENABLE;
                u16Data2 = (u16Data1 & 0xFFF0UL);
                break;
            case E_MIU_BLOCK_1:
                u32RegAddrStar = MIU2_PROTECT1_START;
                u32RegAddrIDenable = MIU2_PROTECT1_ID_ENABLE;
                u16Data2 = (u16Data1 & 0xFF0FUL);
                break;
            case E_MIU_BLOCK_2:
                u32RegAddrStar = MIU2_PROTECT2_START;
                u32RegAddrIDenable = MIU2_PROTECT2_ID_ENABLE;
                u16Data2 = (u16Data1 & 0xF0FFUL);
                break;
            case E_MIU_BLOCK_3:
                u32RegAddrStar = MIU2_PROTECT3_START;
                u32RegAddrIDenable = MIU2_PROTECT3_ID_ENABLE;
                u16Data2 = (u16Data1 & 0x0FFFUL);
                break;
            default:
                return FALSE;
        }
    }
    else
    {
        MIU_HAL_ERR("%s not support MIU%u!\n", __FUNCTION__, u8MiuSel );
        return FALSE;
    }

    // Disable MIU protect
    HAL_MIU_WriteRegBit(u32MiuProtectEn,u8Data,DISABLE);

    if ( bSetFlag )
    {
        // Set Protect IDs
        if(HAL_MIU_SetGroupID(u8MiuSel, u8Blockx, pu8ProtectId, u32RegAddr, u32RegAddrIDenable) == FALSE)
        {
            return FALSE;
        }

        // Set BIT29,30 of start/end address
        u16Data2 = u16Data2 | (MS_U16)((u32Start >> 29) << (u8Blockx*4));                        // u16Data2 for start_ext addr
        u16Data1 = u16Data2 | (MS_U16)(((u32End - 1) >> 29) << (u8Blockx*4+2));
        HAL_MIU_Write2Byte(u32RegAddrMSB, u16Data1);

        // Start Address
        u16Data = (MS_U16)(u32Start >> MIU_PAGE_SHIFT);   //8k/unit
        HAL_MIU_Write2Byte(u32RegAddrStar , u16Data);

        // End Address
        u16Data = (MS_U16)((u32End >> MIU_PAGE_SHIFT)-1);   //8k/unit;
        HAL_MIU_Write2Byte(u32RegAddrStar + 2, u16Data);

        // Enable MIU protect
        HAL_MIU_WriteRegBit(u32MiuProtectEn, u8Data, ENABLE);
    }
    else
    {
        // Reset Protect IDs
        HAL_MIU_ResetGroupID(u8MiuSel, u8Blockx, pu8ProtectId, u32RegAddr, u32RegAddrIDenable);
    }

    // clear log
    HAL_MIU_Write2BytesBit(u32Reg+REG_MIU_PROTECT_STATUS, TRUE, REG_MIU_PROTECT_LOG_CLR);
    HAL_MIU_Write2BytesBit(u32Reg+REG_MIU_PROTECT_STATUS, FALSE, REG_MIU_PROTECT_LOG_CLR);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_MIU_ParseOccupiedResource
/// @brief \b Function  \b Description: Parse occupied resource to software structure
/// @return             \b 0: Fail 1: OK
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_MIU_ParseOccupiedResource(void)
{
    MS_U8  u8MiuSel;
    MS_U8  u8Blockx;
    MS_U8  u8ClientID;
    MS_U16 u16idenable;
    MS_U32 u32index;
    MS_U32 u32RegAddr;
    MS_U32 u32RegAddrIDenable;

    for(u8MiuSel = E_MIU_0; u8MiuSel < MIU_MAX_DEVICE; u8MiuSel++)
    {
        for(u8Blockx = E_MIU_BLOCK_0; u8Blockx < E_MIU_BLOCK_NUM; u8Blockx++)
        {
            if(u8MiuSel == E_MIU_0)
            {
                u32RegAddr = MIU_PROTECT0_ID0;

                switch (u8Blockx)
                {
                    case E_MIU_BLOCK_0:
                        u32RegAddrIDenable = MIU_PROTECT0_ID_ENABLE;
                        break;
                    case E_MIU_BLOCK_1:
                        u32RegAddrIDenable = MIU_PROTECT1_ID_ENABLE;
                        break;
                    case E_MIU_BLOCK_2:
                        u32RegAddrIDenable = MIU_PROTECT2_ID_ENABLE;
                        break;
                    case E_MIU_BLOCK_3:
                        u32RegAddrIDenable = MIU_PROTECT3_ID_ENABLE;
                        break;
                    default:
                        return false;
                }
            }
            else if(u8MiuSel == E_MIU_1)
            {
                u32RegAddr = MIU1_PROTECT0_ID0;

                switch (u8Blockx)
                {
                case E_MIU_BLOCK_0:
                     u32RegAddrIDenable = MIU1_PROTECT0_ID_ENABLE;
                     break;
                 case E_MIU_BLOCK_1:
                     u32RegAddrIDenable = MIU1_PROTECT1_ID_ENABLE;
                     break;
                 case E_MIU_BLOCK_2:
                     u32RegAddrIDenable = MIU1_PROTECT2_ID_ENABLE;
                     break;
                 case E_MIU_BLOCK_3:
                     u32RegAddrIDenable = MIU1_PROTECT3_ID_ENABLE;
                     break;
                 default:
                     return false;
                }
            }
            else if(u8MiuSel == E_MIU_2)
            {
                u32RegAddr = MIU2_PROTECT0_ID0;

                switch (u8Blockx)
                {
                case E_MIU_BLOCK_0:
                     u32RegAddrIDenable = MIU2_PROTECT0_ID_ENABLE;
                     break;
                 case E_MIU_BLOCK_1:
                     u32RegAddrIDenable = MIU2_PROTECT1_ID_ENABLE;
                     break;
                 case E_MIU_BLOCK_2:
                     u32RegAddrIDenable = MIU2_PROTECT2_ID_ENABLE;
                     break;
                 case E_MIU_BLOCK_3:
                     u32RegAddrIDenable = MIU2_PROTECT3_ID_ENABLE;
                     break;
                 default:
                     return false;
                }
            }
            else
            {
                printk(KERN_ERR "%s not support MIU%u!\n", __FUNCTION__, u8MiuSel );
                return FALSE;
            }

            u16idenable = HAL_MIU_Read2Byte(u32RegAddrIDenable);
            for(u32index = 0; u32index < MIU_MAX_PROTECT_ID; u32index++)
            {
                IDEnables[u8MiuSel][u8Blockx][u32index] = ((u16idenable >> u32index) & 0x1UL)? 1: 0;
            }
        }//for(u8Blockx = E_MIU_BLOCK_0; u8Blockx < E_MIU_BLOCK_NUM; u8Blockx++)

        for(u32index = 0; u32index < MIU_MAX_PROTECT_ID; u32index++)
        {
            u8ClientID = HAL_MIU_ReadByte(u32RegAddr + u32index) & 0x7F;
            IDs[u8MiuSel][u32index] = clientTbl[u8MiuSel][u8ClientID];
        }
    }//for(u8MiuSel = E_MIU_0; u8MiuSel < E_MIU_NUM; u8MiuSel++)

    return TRUE;
}

#ifdef CONFIG_MP_CMA_PATCH_DEBUG_STATIC_MIU_PROTECT
#define GET_HIT_BLOCK(regval)       BITS_RANGE_VAL(regval, REG_MIU_PROTECT_HIT_NO)
#define GET_HIT_CLIENT(regval)      BITS_RANGE_VAL(regval, REG_MIU_PROTECT_HIT_ID)

MS_BOOL HAL_MIU_GetProtectInfo(MS_U8 u8MiuDev, MIU_PortectInfo *pInfo)
{
    MS_U16 ret = 0;
    MS_U16 loaddr = 0;
    MS_U16 hiaddr = 0;
    MS_U32 u32Address = 0;
    MS_U32 u32Reg = (u8MiuDev) ? MIU1_REG_BASE : MIU_REG_BASE;

    if (!pInfo)
        return FALSE;

    ret = HAL_MIU_Read2Byte(u32Reg+REG_MIU_PROTECT_STATUS);
    loaddr = HAL_MIU_Read2Byte(u32Reg+REG_MIU_PROTECT_LOADDR);
    hiaddr = HAL_MIU_Read2Byte(u32Reg+REG_MIU_PROTECT_HIADDR);

    pInfo->bHit = false;

    if (REG_MIU_PROTECT_HIT_FALG & ret)
    {
        pInfo->bHit = TRUE;

        pInfo->u8Block = (MS_U8)GET_HIT_BLOCK(ret);
        pInfo->u8Group = (MS_U8)(GET_HIT_CLIENT(ret) >> 4);
        pInfo->u8ClientID = (MS_U8)(GET_HIT_CLIENT(ret) & 0x0F);
        u32Address = (MS_U32)((hiaddr << 16) | loaddr) ;
        u32Address = u32Address * MIU_PROTECT_ADDRESS_UNIT;

        printk(KERN_ERR "MIU%u Block:%u Group:%u ClientID:%u Hitted_Address:0x%lx<->0x%lx\n", u8MiuDev,
        pInfo->u8Block, pInfo->u8Group, pInfo->u8ClientID, u32Address, u32Address + MIU_PROTECT_ADDRESS_UNIT - 1);

        //clear log
        HAL_MIU_Write2BytesBit(u32Reg+REG_MIU_PROTECT_STATUS, TRUE, REG_MIU_PROTECT_LOG_CLR);
        HAL_MIU_Write2BytesBit(u32Reg+REG_MIU_PROTECT_STATUS, FALSE, REG_MIU_PROTECT_LOG_CLR);
    }

    return TRUE;
}
#endif

MS_U16 MIU_PROTECT_EN_T[4];
MS_U16 MIU_PROTECT0_ID_ENABLE_T[4];
MS_U16 MIU_PROTECT1_ID_ENABLE_T[4];
MS_U16 MIU_PROTECT2_ID_ENABLE_T[4];
MS_U16 MIU_PROTECT3_ID_ENABLE_T[4];
MS_U16 MIU_PROTECT0_START_T[4];
MS_U16 MIU_PROTECT1_START_T[4];
MS_U16 MIU_PROTECT2_START_T[4];
MS_U16 MIU_PROTECT3_START_T[4];
MS_U16 MIU_PROTECT0_END_T[4];
MS_U16 MIU_PROTECT1_END_T[4];
MS_U16 MIU_PROTECT2_END_T[4];
MS_U16 MIU_PROTECT3_END_T[4];
MS_U16 MIU_GROUPCLIENT[4][MIU_MAX_PROTECT_ID];


MS_BOOL HAL_Protect_Save(void)
{
    int index;

    MIU_PROTECT_EN_T[0] = HAL_MIU_Read2Byte(MIU_PROTECT_EN_INTERNAL);
    MIU_PROTECT_EN_T[1] = HAL_MIU_Read2Byte(MIU1_PROTECT_EN);
    MIU_PROTECT_EN_T[2] = HAL_MIU_Read2Byte(MIU2_PROTECT_EN);
    //MIU_PROTECT_EN_T[3] = HAL_MIU_Read2Byte(MIU3_PROTECT_EN);

    MIU_PROTECT0_ID_ENABLE_T[0] = HAL_MIU_Read2Byte(MIU_PROTECT0_ID_ENABLE);
    MIU_PROTECT0_ID_ENABLE_T[1] = HAL_MIU_Read2Byte(MIU1_PROTECT0_ID_ENABLE);
    MIU_PROTECT0_ID_ENABLE_T[2] = HAL_MIU_Read2Byte(MIU2_PROTECT0_ID_ENABLE);
    //MIU_PROTECT0_ID_ENABLE_T[3] = HAL_MIU_Read2Byte(MIU3_PROTECT0_ID_ENABLE);

    MIU_PROTECT1_ID_ENABLE_T[0] = HAL_MIU_Read2Byte(MIU_PROTECT1_ID_ENABLE);
    MIU_PROTECT1_ID_ENABLE_T[1] = HAL_MIU_Read2Byte(MIU1_PROTECT1_ID_ENABLE);
    MIU_PROTECT1_ID_ENABLE_T[2] = HAL_MIU_Read2Byte(MIU2_PROTECT1_ID_ENABLE);
    //MIU_PROTECT1_ID_ENABLE_T[3] = HAL_MIU_Read2Byte(MIU3_PROTECT1_ID_ENABLE);

    MIU_PROTECT2_ID_ENABLE_T[0] = HAL_MIU_Read2Byte(MIU_PROTECT2_ID_ENABLE);
    MIU_PROTECT2_ID_ENABLE_T[1] = HAL_MIU_Read2Byte(MIU1_PROTECT2_ID_ENABLE);
    MIU_PROTECT2_ID_ENABLE_T[2] = HAL_MIU_Read2Byte(MIU2_PROTECT2_ID_ENABLE);
    //MIU_PROTECT2_ID_ENABLE_T[3] = HAL_MIU_Read2Byte(MIU3_PROTECT2_ID_ENABLE);

    MIU_PROTECT3_ID_ENABLE_T[0] = HAL_MIU_Read2Byte(MIU_PROTECT3_ID_ENABLE);
    MIU_PROTECT3_ID_ENABLE_T[1] = HAL_MIU_Read2Byte(MIU1_PROTECT3_ID_ENABLE);
    MIU_PROTECT3_ID_ENABLE_T[2] = HAL_MIU_Read2Byte(MIU2_PROTECT3_ID_ENABLE);
    //MIU_PROTECT3_ID_ENABLE_T[3] = HAL_MIU_Read2Byte(MIU3_PROTECT3_ID_ENABLE);

    MIU_PROTECT0_START_T[0] = HAL_MIU_Read2Byte(MIU_PROTECT0_START);
    MIU_PROTECT0_START_T[1] = HAL_MIU_Read2Byte(MIU1_PROTECT0_START);
    MIU_PROTECT0_START_T[2] = HAL_MIU_Read2Byte(MIU2_PROTECT0_START);
    //MIU_PROTECT0_START_T[3] = HAL_MIU_Read2Byte(MIU3_PROTECT0_START);

    MIU_PROTECT1_START_T[0] = HAL_MIU_Read2Byte(MIU_PROTECT1_START);
    MIU_PROTECT1_START_T[1] = HAL_MIU_Read2Byte(MIU1_PROTECT1_START);
    MIU_PROTECT1_START_T[2] = HAL_MIU_Read2Byte(MIU2_PROTECT1_START);
    //MIU_PROTECT1_START_T[3] = HAL_MIU_Read2Byte(MIU3_PROTECT1_START);

    MIU_PROTECT2_START_T[0] = HAL_MIU_Read2Byte(MIU_PROTECT2_START);
    MIU_PROTECT2_START_T[1] = HAL_MIU_Read2Byte(MIU1_PROTECT2_START);
    MIU_PROTECT2_START_T[2] = HAL_MIU_Read2Byte(MIU2_PROTECT2_START);
    //MIU_PROTECT2_START_T[3] = HAL_MIU_Read2Byte(MIU3_PROTECT2_START);

    MIU_PROTECT3_START_T[0] = HAL_MIU_Read2Byte(MIU_PROTECT3_START);
    MIU_PROTECT3_START_T[1] = HAL_MIU_Read2Byte(MIU1_PROTECT3_START);
    MIU_PROTECT3_START_T[2] = HAL_MIU_Read2Byte(MIU2_PROTECT3_START);
    //MIU_PROTECT3_START_T[3] = HAL_MIU_Read2Byte(MIU3_PROTECT3_START);

    MIU_PROTECT0_END_T[0] = HAL_MIU_Read2Byte(MIU_PROTECT0_START+2);
    MIU_PROTECT0_END_T[1] = HAL_MIU_Read2Byte(MIU1_PROTECT0_START+2);
    MIU_PROTECT0_END_T[2] = HAL_MIU_Read2Byte(MIU2_PROTECT0_START+2);
    //MIU_PROTECT0_END_T[3] = HAL_MIU_Read2Byte(MIU3_PROTECT0_START+2);

    MIU_PROTECT1_END_T[0] = HAL_MIU_Read2Byte(MIU_PROTECT1_START+2);
    MIU_PROTECT1_END_T[1] = HAL_MIU_Read2Byte(MIU1_PROTECT1_START+2);
    MIU_PROTECT1_END_T[2] = HAL_MIU_Read2Byte(MIU2_PROTECT1_START+2);
    //MIU_PROTECT1_END_T[3] = HAL_MIU_Read2Byte(MIU3_PROTECT1_START+2);

    MIU_PROTECT2_END_T[0] = HAL_MIU_Read2Byte(MIU_PROTECT2_START+2);
    MIU_PROTECT2_END_T[1] = HAL_MIU_Read2Byte(MIU1_PROTECT2_START+2);
    MIU_PROTECT2_END_T[2] = HAL_MIU_Read2Byte(MIU2_PROTECT2_START+2);
    //MIU_PROTECT2_END_T[3] = HAL_MIU_Read2Byte(MIU3_PROTECT2_START+2);

    MIU_PROTECT3_END_T[0] = HAL_MIU_Read2Byte(MIU_PROTECT3_START+2);
    MIU_PROTECT3_END_T[1] = HAL_MIU_Read2Byte(MIU1_PROTECT3_START+2);
    MIU_PROTECT3_END_T[2] = HAL_MIU_Read2Byte(MIU2_PROTECT3_START+2);
    //MIU_PROTECT3_END_T[3] = HAL_MIU_Read2Byte(MIU3_PROTECT3_START+2);

    for(index = 0; index < MIU_MAX_PROTECT_ID; index++){
        MIU_GROUPCLIENT[0][index] = HAL_MIU_ReadByte(MIU_PROTECT0_ID0+index);
    }

    for(index = 0; index < MIU_MAX_PROTECT_ID; index++){
        MIU_GROUPCLIENT[1][index] = HAL_MIU_ReadByte(MIU1_PROTECT0_ID0+index);
    }

    for(index = 0; index < MIU_MAX_PROTECT_ID; index++){
        MIU_GROUPCLIENT[2][index] = HAL_MIU_ReadByte(MIU2_PROTECT0_ID0+index);
    }

    /*for(index = 0; index < MIU_MAX_PROTECT_ID; index++){
        MIU_GROUPCLIENT[3][index] = HAL_MIU_ReadByte(MIU3_PROTECT0_ID0+index);
    }*/
    return TRUE;
}

MS_BOOL HAL_Protect_Restore(void)
{
    int index;

    HAL_MIU_Write2BytesBit(MIU_PROTECT_EN_INTERNAL, FALSE, BIT3 | BIT2 | BIT1 | BIT0);
    HAL_MIU_Write2BytesBit(MIU1_PROTECT_EN, FALSE, BIT3 | BIT2 | BIT1 | BIT0);
    HAL_MIU_Write2BytesBit(MIU2_PROTECT_EN, FALSE, BIT3 | BIT2 | BIT1 | BIT0);
    //HAL_MIU_Write2BytesBit(MIU3_PROTECT_EN, FALSE, BIT3 | BIT2 | BIT1 | BIT0);

    HAL_MIU_Write2Byte(MIU_PROTECT0_ID_ENABLE,MIU_PROTECT0_ID_ENABLE_T[0]);
    HAL_MIU_Write2Byte(MIU1_PROTECT0_ID_ENABLE,MIU_PROTECT0_ID_ENABLE_T[1]);
    HAL_MIU_Write2Byte(MIU2_PROTECT0_ID_ENABLE,MIU_PROTECT0_ID_ENABLE_T[2]);
    //HAL_MIU_Write2Byte(MIU3_PROTECT0_ID_ENABLE,MIU_PROTECT0_ID_ENABLE_T[3]);

    HAL_MIU_Write2Byte(MIU_PROTECT1_ID_ENABLE,MIU_PROTECT1_ID_ENABLE_T[0]);
    HAL_MIU_Write2Byte(MIU1_PROTECT1_ID_ENABLE,MIU_PROTECT1_ID_ENABLE_T[1]);
    HAL_MIU_Write2Byte(MIU2_PROTECT1_ID_ENABLE,MIU_PROTECT1_ID_ENABLE_T[2]);
    //HAL_MIU_Write2Byte(MIU3_PROTECT1_ID_ENABLE,MIU_PROTECT1_ID_ENABLE_T[3]);

    HAL_MIU_Write2Byte(MIU_PROTECT2_ID_ENABLE,MIU_PROTECT2_ID_ENABLE_T[0]);
    HAL_MIU_Write2Byte(MIU1_PROTECT2_ID_ENABLE,MIU_PROTECT2_ID_ENABLE_T[1]);
    HAL_MIU_Write2Byte(MIU2_PROTECT2_ID_ENABLE,MIU_PROTECT2_ID_ENABLE_T[2]);
    //HAL_MIU_Write2Byte(MIU3_PROTECT2_ID_ENABLE,MIU_PROTECT2_ID_ENABLE_T[3]);

    HAL_MIU_Write2Byte(MIU_PROTECT3_ID_ENABLE,MIU_PROTECT3_ID_ENABLE_T[0]);
    HAL_MIU_Write2Byte(MIU1_PROTECT3_ID_ENABLE,MIU_PROTECT3_ID_ENABLE_T[1]);
    HAL_MIU_Write2Byte(MIU2_PROTECT3_ID_ENABLE,MIU_PROTECT3_ID_ENABLE_T[2]);
    //HAL_MIU_Write2Byte(MIU3_PROTECT3_ID_ENABLE,MIU_PROTECT3_ID_ENABLE_T[3]);

    HAL_MIU_Write2Byte(MIU_PROTECT0_START,MIU_PROTECT0_START_T[0]);
    HAL_MIU_Write2Byte(MIU1_PROTECT0_START,MIU_PROTECT0_START_T[1]);
    HAL_MIU_Write2Byte(MIU2_PROTECT0_START,MIU_PROTECT0_START_T[2]);
    //HAL_MIU_Write2Byte(MIU3_PROTECT0_START,MIU_PROTECT0_START_T[3]);

    HAL_MIU_Write2Byte(MIU_PROTECT1_START,MIU_PROTECT1_START_T[0]);
    HAL_MIU_Write2Byte(MIU1_PROTECT1_START,MIU_PROTECT1_START_T[1]);
    HAL_MIU_Write2Byte(MIU2_PROTECT1_START,MIU_PROTECT1_START_T[2]);
    //HAL_MIU_Write2Byte(MIU3_PROTECT1_START,MIU_PROTECT1_START_T[3]);

    HAL_MIU_Write2Byte(MIU_PROTECT2_START,MIU_PROTECT2_START_T[0]);
    HAL_MIU_Write2Byte(MIU1_PROTECT2_START,MIU_PROTECT2_START_T[1]);
    HAL_MIU_Write2Byte(MIU2_PROTECT2_START,MIU_PROTECT2_START_T[2]);
    //HAL_MIU_Write2Byte(MIU3_PROTECT2_START,MIU_PROTECT2_START_T[3]);

    HAL_MIU_Write2Byte(MIU_PROTECT3_START,MIU_PROTECT3_START_T[0]);
    HAL_MIU_Write2Byte(MIU1_PROTECT3_START,MIU_PROTECT3_START_T[1]);
    HAL_MIU_Write2Byte(MIU2_PROTECT3_START,MIU_PROTECT3_START_T[2]);
    //HAL_MIU_Write2Byte(MIU3_PROTECT3_START,MIU_PROTECT3_START_T[3]);

    HAL_MIU_Write2Byte(MIU_PROTECT0_START+2,MIU_PROTECT0_END_T[0]);
    HAL_MIU_Write2Byte(MIU1_PROTECT0_START+2,MIU_PROTECT0_END_T[1]);
    HAL_MIU_Write2Byte(MIU2_PROTECT0_START+2,MIU_PROTECT0_END_T[2]);
    //HAL_MIU_Write2Byte(MIU3_PROTECT0_START+2,MIU_PROTECT0_END_T[3]);

    HAL_MIU_Write2Byte(MIU_PROTECT1_START+2,MIU_PROTECT1_END_T[0]);
    HAL_MIU_Write2Byte(MIU1_PROTECT1_START+2,MIU_PROTECT1_END_T[1]);
    HAL_MIU_Write2Byte(MIU2_PROTECT1_START+2,MIU_PROTECT1_END_T[2]);
    //HAL_MIU_Write2Byte(MIU3_PROTECT1_START+2,MIU_PROTECT1_END_T[3]);

    HAL_MIU_Write2Byte(MIU_PROTECT2_START+2,MIU_PROTECT2_END_T[0]);
    HAL_MIU_Write2Byte(MIU1_PROTECT2_START+2,MIU_PROTECT2_END_T[1]);
    HAL_MIU_Write2Byte(MIU2_PROTECT2_START+2,MIU_PROTECT2_END_T[2]);
    //HAL_MIU_Write2Byte(MIU3_PROTECT2_START+2,MIU_PROTECT2_END_T[3]);

    HAL_MIU_Write2Byte(MIU_PROTECT3_START+2,MIU_PROTECT3_END_T[0]);
    HAL_MIU_Write2Byte(MIU1_PROTECT3_START+2,MIU_PROTECT3_END_T[1]);
    HAL_MIU_Write2Byte(MIU2_PROTECT3_START+2,MIU_PROTECT3_END_T[2]);
    //HAL_MIU_Write2Byte(MIU3_PROTECT3_START+2,MIU_PROTECT3_END_T[3]);

    for(index = 0; index < MIU_MAX_PROTECT_ID; index++){
        HAL_MIU_WriteByte(MIU_PROTECT0_ID0+index,MIU_GROUPCLIENT[0][index]);
    }

    for(index = 0; index < MIU_MAX_PROTECT_ID; index++){
        HAL_MIU_WriteByte(MIU1_PROTECT0_ID0+index,MIU_GROUPCLIENT[1][index]);
    }

    for(index = 0; index < MIU_MAX_PROTECT_ID; index++){
        HAL_MIU_WriteByte(MIU2_PROTECT0_ID0+index,MIU_GROUPCLIENT[2][index]);
    }

    /*for(index = 0; index < MIU_MAX_PROTECT_ID; index++){
        HAL_MIU_WriteByte(MIU3_PROTECT0_ID0+index,MIU_GROUPCLIENT[3][index]);
    }*/

    HAL_MIU_Write2Byte(MIU_PROTECT_EN_INTERNAL,MIU_PROTECT_EN_T[0]);
    HAL_MIU_Write2Byte(MIU1_PROTECT_EN,MIU_PROTECT_EN_T[1]);
    HAL_MIU_Write2Byte(MIU2_PROTECT_EN,MIU_PROTECT_EN_T[2]);
    //HAL_MIU_Write2Byte(MIU3_PROTECT_EN,MIU_PROTECT_EN_T[3]);

    return TRUE;
}

#if 0
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_MIU_SlitInit
/// @brief \b Function  \b Description: Initialize slit sram
/// @return             \b 0: Fail 1: OK
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_MIU_SlitInit(void)
{
    MS_U32 u32RegAddr0;
    MS_U32 u32RegAddr1;
    MS_U16 u16Val;
    MS_BOOL ret;

    ret = TRUE;

    // Map virtual address to _gSlit0_FlagGroupBase
    if(pfn_valid(__phys_to_pfn(ARM_MIU0_BASE_ADDR)))
    {
        _gSlit0_FlagGroupBase = (SRAM_Slit_FlagGroup *)__phys_to_virt(ARM_MIU0_BASE_ADDR);
    }
    else
    {
        _gSlit0_FlagGroupBase = (SRAM_Slit_FlagGroup *)ioremap(ARM_MIU0_BASE_ADDR, sizeof(SRAM_Slit_FlagGroup));
     MIU_HAL_ERR("ioremap _gSlit0_FlagGroupBase = 0x%zx fail\n", (size_t)_gSlit0_FlagGroupBase);
        BUG_ON(!_gSlit0_FlagGroupBase);
        ret = FALSE;
    }

    // Map virtual address to _gSlit1_FlagGroupBase
    if(pfn_valid(__phys_to_pfn(ARM_MIU1_BASE_ADDR)))
    {
        _gSlit1_FlagGroupBase = (SRAM_Slit_FlagGroup *)__phys_to_virt(ARM_MIU1_BASE_ADDR);
    }
    else
    {
        _gSlit1_FlagGroupBase = (SRAM_Slit_FlagGroup *)ioremap(ARM_MIU1_BASE_ADDR, sizeof(SRAM_Slit_FlagGroup));
     MIU_HAL_ERR("ioremap _gSlit1_FlagGroupBase = 0x%zx fail\n", (size_t)_gSlit1_FlagGroupBase);
        BUG_ON(!_gSlit1_FlagGroupBase);
        ret = FALSE;
    }

    printk("_gSlit0_FlagGroupBase = 0x%zx \n", (size_t)_gSlit0_FlagGroupBase);
    printk("_gSlit1_FlagGroupBase = 0x%zx \n", (size_t)_gSlit1_FlagGroupBase);

    // Setup HEMCH into programable ID list
    u32RegAddr0 = REG_MIU0_SLIT_RW_ID0;
    u16Val = HAL_MIU_GetClientInfo(E_MIU_0, (eMIUClientID)MIU_CLIENT_MIPS_RW);
    HAL_MIU_Write2Byte(u32RegAddr0, u16Val);

    u32RegAddr0 = REG_MIU1_SLIT_RW_ID0;
    u16Val = HAL_MIU_GetClientInfo(E_MIU_1, (eMIUClientID)MIU_CLIENT_MIPS_RW);
    HAL_MIU_Write2Byte(u32RegAddr0, u16Val);

    // Setup slit sram base address as 0x00000000
    u32RegAddr0 = REG_MIU0_SLIT_BASE_ADDR_LOW;
    HAL_MIU_Write2Byte(u32RegAddr0, 0x0000);
    u32RegAddr0 = REG_MIU0_SLIT_BASE_ADDR_HIGH;
    HAL_MIU_Write2Byte(u32RegAddr0, 0x0000);

    u32RegAddr0 = REG_MIU1_SLIT_BASE_ADDR_LOW;
    HAL_MIU_Write2Byte(u32RegAddr0, 0x0000);
    u32RegAddr0 = REG_MIU1_SLIT_BASE_ADDR_HIGH;
    HAL_MIU_Write2Byte(u32RegAddr0, 0x0000);

    // Enable accessibility of slit sram
    u32RegAddr0 = REG_MIU0_SLIT_CTL;
    u32RegAddr1 = REG_MIU1_SLIT_CTL;

    u16Val = HAL_MIU_Read2Byte(u32RegAddr0);
    u16Val |= REG_MIU_SLIT_TOP_EN;
    HAL_MIU_Write2Byte(u32RegAddr0, u16Val);

    u16Val = HAL_MIU_Read2Byte(u32RegAddr1);
    u16Val |= REG_MIU_SLIT_TOP_EN;
    HAL_MIU_Write2Byte(u32RegAddr1, u16Val);

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: HAL_MIU_SetSlitRange()
/// @brief \b Function \b Description:  Enable/Disable MIU Slit range
/// @param u8Blockx        \b IN     : MIU Block to protect (5)
/// @param *pu8ProtectId   \b IN     : Allow specified client IDs to write
/// @param u32Start        \b IN     : Starting bus address
/// @param u32End          \b IN     : End bus address
/// @param bSetFlag        \b IN     : Disable or Enable MIU protection
///                                      - -Disable(0)
///                                      - -Enable(1)
/// @param <OUT>           \b None    :
/// @param <RET>           \b None    :
/// @param <GLOBAL>        \b None    :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_MIU_SetSlitRange(
                          MS_U8    u8Blockx,
                          MS_U8    *pu8ProtectId,
                          MS_U32   u32BusStart,
                          MS_U32   u32BusEnd,
                          MS_BOOL  bSetFlag
                         )
{
    MS_U32 u32RegAddrIDenable;
    MS_U32 u32RegProtectEn;
    MS_U32 u32RegLowBound;
    MS_U32 u32RegHighBound;
    MS_U32 u32LowBoundOffset;
    MS_U32 u32HighBoundOffset;
    MS_U32 u32PhyStart;
    MS_U32 u32PhyEnd;
    MS_U32 u32RegId;
    MS_U32 u32RegProtectLog;
    MS_U16 u16Data;
    MS_U8  u8MiuSel;

    u32PhyStart = HAL_MIU_BA2PA(u32BusStart);
    u32PhyEnd = HAL_MIU_BA2PA(u32BusEnd);

    // Get MIU selection and offset
    _phy_to_miu_offset(u8MiuSel, u32HighBoundOffset, u32PhyEnd);
    _phy_to_miu_offset(u8MiuSel, u32LowBoundOffset, u32PhyStart);

    // Incorrect Block ID
    if((u8Blockx < E_SLIT_0) || (u8Blockx >= E_MIU_BLOCK_NUM))
    {
        MIU_HAL_ERR("Err: u8Blockx = %d Out of the number of protect device\n", u8Blockx );
        return FALSE;
    }
    else if(((u32PhyStart & ((1 << MIU_SLIT_SHIFT) -1)) != 0) || ((u32PhyEnd & ((1 << MIU_SLIT_SHIFT) -1)) != 0))
    {
        MIU_HAL_ERR("u32PhyStart = 0x%x\n", u32PhyStart);
        MIU_HAL_ERR("u32PhyEnd = 0x%x\n", u32PhyEnd);
        MIU_HAL_ERR("Err: Protected address should be aligned to 1MB\n");
        return FALSE;
    }
    else if(u32PhyStart >= u32PhyEnd)
    {
        MIU_HAL_ERR("u32StartOffset = 0x%x\n", u32PhyStart);
        MIU_HAL_ERR("u32PhyEnd = 0x%x\n", u32PhyEnd);
        MIU_HAL_ERR("Err: Start address is equal to or more than end address\n");
        return FALSE;
    }

    if(u8MiuSel == E_CHIP_MIU_0)
    {
        u32RegLowBound = REG_MIU0_SLIT_LOW_BOUND;
        u32RegHighBound = REG_MIU0_SLIT_HIGH_BOUND;
        u32RegAddrIDenable = REG_MIU0_SLIT_PROTECT0_ID_ENABLE;
        u32RegProtectEn = REG_MIU0_SLIT_PROTECT_EN;
        u32RegId = MIU_PROTECT0_ID0;
        u32RegProtectLog = REG_MIU_PROTECT_LOG;
    }
    else if(u8MiuSel == E_CHIP_MIU_1)
    {
        u32RegLowBound = REG_MIU1_SLIT_LOW_BOUND;
        u32RegHighBound = REG_MIU1_SLIT_HIGH_BOUND;
        u32RegAddrIDenable = REG_MIU1_SLIT_PROTECT0_ID_ENABLE;
        u32RegProtectEn = REG_MIU1_SLIT_PROTECT_EN;
        u32RegId = MIU1_PROTECT0_ID0;
        u32RegProtectLog = REG_MIU1_PROTECT_LOG;
     }
    else
    {
        MIU_HAL_ERR("%s not support MIU%u!\n", __FUNCTION__, u8MiuSel );
        return FALSE;
    }

    // Offset block ID
    u8Blockx = u8Blockx - E_MIU_SLIT_0 + E_MIU_BLOCK_3 + 1;

    // Disable MIU protect
    HAL_MIU_WriteRegBit(u32RegProtectEn, BIT0, DISABLE);

    if(bSetFlag)
    {
        // Set Protect IDs
        if(HAL_MIU_SetGroupID(u8MiuSel, u8Blockx, pu8ProtectId, u32RegId, u32RegAddrIDenable) == FALSE)
        {
            return FALSE;
        }

        // Low Bound
        u16Data = (MS_U16)(u32LowBoundOffset >> MIU_SLIT_SHIFT);   //1MB unit
        HAL_MIU_Write2Byte(u32RegLowBound , u16Data);

        // High bound
        u16Data = (MS_U16)((u32HighBoundOffset >> MIU_PAGE_SHIFT) - 1);   //1MB unit
        HAL_MIU_Write2Byte(u32RegHighBound, u16Data);

        // Enable MIU protect
        HAL_MIU_WriteRegBit(u32RegProtectEn, BIT0, ENABLE);
    }
    else
    {
        // Reset Protect IDs
        HAL_MIU_ResetGroupID(u8MiuSel, u8Blockx, pu8ProtectId, u32RegId, u32RegAddrIDenable);
    }

    // clear log
    HAL_MIU_Write2BytesBit(u32RegProtectLog, TRUE, REG_MIU_PROTECT_LOG_CLR);
    HAL_MIU_Write2BytesBit(u32RegProtectLog, FALSE, REG_MIU_PROTECT_LOG_CLR);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_Slits()
/// @brief \b Function \b Description:  Enable/Disable MIU Slit
/// @param u8Blockx        \b IN     : MIU Block to protect (E_MIU_SLIT_0)
/// @param u32Start        \b IN     : Starting address(bus address)
/// @param u32End          \b IN     : End address(bus address)
/// @param bSetFlag        \b IN     : Disable or Enable MIU protection
///                                      - -Disable(0)
///                                      - -Enable(1)
/// @param <OUT>           \b None    :
/// @param <RET>           \b None    :
/// @param <GLOBAL>        \b None    :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL HAL_MIU_Slits(MS_U8 u8Blockx, MS_U32 u32SlitsStart, MS_U32 u32SlitsEnd, MS_BOOL bSetFlag)
{
    SRAM_Slit_FlagGroup *_gSlit_FlagGroupBase;
    MS_U32 u32PhyStart;
    MS_U32 u32PhyEnd;
    MS_U32 u32StartOffset;
    MS_U32 u32EndOffset;
    MS_U32 u32RegAddr0;
    MS_U32 u32RegLowBound;
    MS_U32 u32RegHighBound;
    MS_U16 u16LowBoundidx;
    MS_U16 u16HighBoundidx;
    MS_U16 u16Headidx;
    MS_U16 u16Tailidx;
    MS_U16 u16idx;
    MS_U16 u16RegVal;
    MS_U8  u8MiuSel;

    u32PhyStart = HAL_MIU_BA2PA(u32SlitsStart);
    u32PhyEnd = HAL_MIU_BA2PA(u32SlitsEnd);

    // Get MIU selection and offset
    _phy_to_miu_offset(u8MiuSel, u32EndOffset, u32PhyEnd);
    _phy_to_miu_offset(u8MiuSel, u32StartOffset, u32PhyStart);

    // Incorrect Block ID
    if((u8Blockx < E_SLIT_0) || (u8Blockx >= E_MIU_BLOCK_NUM))
    {
        MIU_HAL_ERR("Err: u8Blockx = %d Out of the number of protect device\n", u8Blockx );
        return FALSE;
    }
    else if(u32PhyStart >= u32PhyEnd)
    {
        MIU_HAL_ERR("u32StartOffset = 0x%x\n", u32PhyStart);
        MIU_HAL_ERR("u32PhyEnd = 0x%x\n", u32PhyEnd);
        MIU_HAL_ERR("Err: Start address is equal to or more than end address\n");
        return FALSE;
    }

    if(u8MiuSel == E_CHIP_MIU_0)
    {
        u32RegAddr0 = REG_MIU0_SLIT_RW_ID0;
        u32RegLowBound = REG_MIU0_SLIT_LOW_BOUND;
        u32RegHighBound = REG_MIU0_SLIT_HIGH_BOUND;
        _gSlit_FlagGroupBase = _gSlit0_FlagGroupBase;
    }
    else if(u8MiuSel == E_CHIP_MIU_1)
    {
        u32RegAddr0 = REG_MIU1_SLIT_RW_ID0;
        u32RegLowBound = REG_MIU1_SLIT_LOW_BOUND;
        u32RegHighBound = REG_MIU1_SLIT_HIGH_BOUND;
        _gSlit_FlagGroupBase = _gSlit1_FlagGroupBase;
    }
    else
    {
        MIU_HAL_ERR("%s not support MIU%u!\n", __FUNCTION__, u8MiuSel );
        return FALSE;
    }

    // Get low bound base idx, an index means 1MB entry
    u16LowBoundidx = HAL_MIU_Read2Byte(u32RegLowBound);
    u16HighBoundidx = HAL_MIU_Read2Byte(u32RegLowBound);

    // Get head and tail index, an index means 1MB entry
    u16Headidx = u32StartOffset >> MIU_SLIT_SHIFT;
    u16Tailidx = u32EndOffset >> MIU_SLIT_SHIFT;

    // Enable the accessibility of of slit sram for HEMCU
    u16RegVal = HAL_MIU_Read2Byte(u32RegAddr0);
    u16RegVal |= REG_MIU_SLIT_ID_ENABLE;
    HAL_MIU_Write2Byte(u32RegAddr0, u16RegVal);

    if(u16Headidx < u16LowBoundidx)
    {
        MIU_HAL_ERR("u16Headidx=%d shuold not be less than u16LowBoundidx=%d\n",u16Headidx, u16LowBoundidx);
        return FALSE;
    }
    else if(u16Tailidx > u16HighBoundidx)
    {
        MIU_HAL_ERR("u16Tailidx=%d shuold not be larger than u16HighBoundidx=%d\n",u16Tailidx, u16HighBoundidx);
        return FALSE;
    }

    for(u16idx = u16Headidx; u16idx <= u16Headidx; u16idx++)
    {
        if(bSetFlag)
        {
            _gSlit_FlagGroupBase->cluster[u16idx].flag[0] = 0x01;
        }
        else
        {
            _gSlit_FlagGroupBase->cluster[u16idx].flag[0] = 0x00;
        }

        //Flush memory
    #if defined(CONFIG_MIPS)
        Chip_Flush_Memory_Range((unsigned int)(&(_gSlit_FlagGroupBase->cluster[u16idx].flag[0])) & 0x0FFFFFFF,
                                 sizeof(_gSlit_FlagGroupBase->cluster[u16idx].flag[0]));
    #elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
        Chip_Flush_Cache_Range_VA_PA((unsigned long)(&(_gSlit_FlagGroupBase->cluster[u16idx].flag[0])),
                                     (unsigned long)__pa((&(_gSlit_FlagGroupBase->cluster[u16idx].flag[0]))),
                                     sizeof(_gSlit_FlagGroupBase->cluster[u16idx].flag[0]));
    #else
        #ERROR
    #endif
    }

    // Disable the accessibility of of slit sram for HEMCU
    u16RegVal = HAL_MIU_Read2Byte(u32RegAddr0);
    u16RegVal &= ~REG_MIU_SLIT_ID_ENABLE;
    HAL_MIU_Write2Byte(u32RegAddr0, u16RegVal);

    return TRUE;
}
#endif
