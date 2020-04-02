////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    drvMIU.c
/// @brief  MIU Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------------------------------
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/kernel.h>
#else
#include <string.h>
#endif

#include "MsCommon.h"
#include "MsVersion.h"
#include "MsTypes.h"
#include "drvMIU.h"
#include "halMIU.h"
#include "drvMMIO.h"

//-------------------------------------------------------------------------------------------------
// Local Defines
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Local Variables
//-------------------------------------------------------------------------------------------------
static MSIF_Version _drv_miu_version = {
#if defined(MSOS_TYPE_CE) || defined(DISABLE_C99) || defined(MSOS_TYPE_LINUX) || defined(MSOS_TYPE_LINUX_KERNEL)
	{ MIU_DRV_VERSION },
#else
    .DDI = { MIU_DRV_VERSION },
#endif
};

MS_U8 MIU0_request_mask_count[MIU_CLIENT_MAX];
MS_U8 MIU1_request_mask_count[MIU_CLIENT_MAX];

//-------------------------------------------------------------------------------------------------
// Local Function Prototypes
//-------------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_MIU_SetIOMapBase
/// @brief \b Function  \b Description: set MIU I/O Map base address
/// @param None         \b IN :
/// @param None         \b OUT :
/// @param MS_BOOL      \b RET
/// @param None         \b GLOBAL :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL MDrv_MIU_SetIOMapBase(void)
{
    MS_U32 u32MMIOBaseAdr = 0;
    MS_U32 u32NonPMBankSize = 0;

    if( !MDrv_MMIO_GetBASE(&u32MMIOBaseAdr, &u32NonPMBankSize, MS_MODULE_MIU))
    {
        //MS_DEBUG_MSG(printf("Get IOMap failure\n"));
        MS_ASSERT(0);
        return FALSE;
    }
    HAL_MIU_SetIOMapBase(u32MMIOBaseAdr);


    if( !MDrv_MMIO_GetBASE(&u32MMIOBaseAdr, &u32NonPMBankSize, MS_MODULE_PM))
    {
        //MS_DEBUG_MSG(printf("Get IOMap failure\n"));
        MS_ASSERT(0);
        return FALSE;
    }
    HAL_MIU_SetPMIOMapBase(u32MMIOBaseAdr);
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_GetDefaultClientID_KernelProtect()
/// @brief \b Function \b Description:  Get default client id array pointer for protect kernel
/// @param <RET>           \b     : The pointer of Array of client IDs
////////////////////////////////////////////////////////////////////////////////
MS_U8* MDrv_MIU_GetDefaultClientID_KernelProtect(void)
{
    return HAL_MIU_GetDefaultClientID_KernelProtect();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function    \b Name: MDrv_MIU_ProtectAlign()
/// @brief \b Function    \b Description:  Get the page shift for MIU protect
/// @param <*u32PageShift>\b IN: Page shift
/// @param <RET>          \b OUT: None
////////////////////////////////////////////////////////////////////////////////
MS_BOOL MDrv_MIU_ProtectAlign(MS_U32 *u32PageShift)
{
    *u32PageShift = HAL_MIU_ProtectAlign();

    if(*u32PageShift > 0)
        return TRUE;
    else
        return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_Dram_Size()
/// @brief \b Function \b Description:  Enable/Disable MIU Protection mode
/// @param MiuID        \b IN     : MIU ID
/// @param DramSize     \b IN     : Specified Dram size for MIU protect
/// @param <OUT>           \b None    :
/// @param <RET>           \b None    :
/// @param <GLOBAL>        \b None    :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL MDrv_MIU_Dram_Size(MS_U8 MiuID, MS_U8 DramSize)
{
    return HAL_MIU_Dram_Size(MiuID, DramSize);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_Protect()
/// @brief \b Function \b Description:  Enable/Disable MIU Protection mode
/// @param u8Blockx        \b IN     : MIU Block to protect (0 ~ 3)
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
MS_BOOL MDrv_MIU_Protect(
                          MS_U8    u8Blockx,
                          MS_U8    *pu8ProtectId,
                          MS_U32   u32Start,
                          MS_U32   u32End,
                          MS_BOOL  bSetFlag
                         )
{
    MS_BOOL Result;

    Result = HAL_MIU_Protect(u8Blockx, pu8ProtectId, u32Start, u32End, bSetFlag);

    if(Result == TRUE)
    {
        return TRUE;
    }
    else
    {
       MS_ASSERT(0);
       return FALSE;
    }
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_SetSsc()
/// @brief \b Function \b Description: MDrv_MIU_SetSsc, @Step & Span
/// @param u16Fmodulation   \b IN : 20KHz ~ 40KHz (Input Value = 20 ~ 40)
/// @param u16FDeviation    \b IN  : under 0.1% ~ 3% (Input Value = 1 ~ 30)
/// @param bEnable          \b IN    :
/// @param None             \b OUT  :
/// @param None             \b RET  :
/// @param None             \b GLOBAL :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL MDrv_MIU_SetSsc(MS_U16 u16Fmodulation,
                        MS_U16 u16FDeviation,
                        MS_BOOL bEnable)
{
    return HAL_MIU_SetSsc(0, u16Fmodulation, u16FDeviation, bEnable);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_SetSscValue()
/// @brief \b Function \b Description: MDrv_MIU_SetSscValue, @Step & Span
/// @param u8Miu            \b IN   : 0: MIU0 1:MIU1
/// @param u16Fmodulation   \b IN   : 20KHz ~ 40KHz (Input Value = 20 ~ 40)
/// @param u16FDeviation    \b IN   : under 0.1% ~ 3% (Input Value = 1 ~ 30)
/// @param bEnable          \b IN   :
/// @param None             \b OUT  :
/// @param None             \b RET  :
/// @param None             \b GLOBAL :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL MDrv_MIU_SetSscValue(MS_U8 u8MiuDev,
                             MS_U16 u16Fmodulation,
                             MS_U16 u16FDeviation,
                             MS_BOOL bEnable)
{
    return HAL_MIU_SetSsc(u8MiuDev, u16Fmodulation, u16FDeviation, bEnable);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_MaskReq()
/// @brief \b Function \b Description: Mask MIU request
/// @param u8Miu        \b IN   : 0: MIU0 1:MIU1
/// @param eClientID    \b IN   : Client ID
/// @param None         \b OUT  :
/// @param None         \b RET  :
///////////////////////////////////////////////////////////////////////////////
void MDrv_MIU_MaskReq(MS_U8 u8Miu, eMIUClientID eClientID)
{
    HAL_MIU_MaskReq(u8Miu, eClientID);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_UnMaskReq()
/// @brief \b Function \b Description: Unmak MIU request
/// @param u8Miu        \b IN   : 0: MIU0 1:MIU1
/// @param eClientID    \b IN   : Client ID
/// @param None         \b OUT  :
/// @param None         \b RET  :
///////////////////////////////////////////////////////////////////////////////
void MDrv_MIU_UnMaskReq(MS_U8 u8Miu, eMIUClientID eClientID)
{
    HAL_MIU_UnMaskReq(u8Miu, eClientID);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_Mask_Req_OPM_R()
/// @brief \b Function \b Description:
/// @param None  \b IN   :
/// @param None  \b OUT  :
/// @param None  \b RET  :
///////////////////////////////////////////////////////////////////////////////
void MDrv_MIU_Mask_Req_OPM_R(MS_U8 u8Mask, MS_U8 u8Miu)
{
    HAL_MIU_Mask_Req_OPM_R(u8Mask, u8Miu);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_Mask_Req_DNRB_R()
/// @brief \b Function \b Description:
/// @param None  \b IN   :
/// @param None  \b OUT  :
/// @param None  \b RET  :
///////////////////////////////////////////////////////////////////////////////
void MDrv_MIU_Mask_Req_DNRB_R(MS_U8 u8Mask, MS_U8 u8Miu)
{
    HAL_MIU_Mask_Req_DNRB_R( u8Mask,  u8Miu);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_Mask_Req_DNRB_W()
/// @brief \b Function \b Description:
/// @param None  \b IN   :
/// @param None  \b OUT  :
/// @param None  \b RET  :
///////////////////////////////////////////////////////////////////////////////
void MDrv_MIU_Mask_Req_DNRB_W(MS_U8 u8Mask, MS_U8 u8Miu)
{
    HAL_MIU_Mask_Req_DNRB_W( u8Mask,  u8Miu);

}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_Mask_Req_DNRB_RW()
/// @brief \b Function \b Description:
/// @param None  \b IN   :
/// @param None  \b OUT  :
/// @param None  \b RET  :
///////////////////////////////////////////////////////////////////////////////
void MDrv_MIU_Mask_Req_DNRB_RW(MS_U8 u8Mask, MS_U8 u8Miu)
{
   HAL_MIU_Mask_Req_DNRB_RW( u8Mask,  u8Miu);
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_Mask_Req_SC_RW()
/// @brief \b Function \b Description:
/// @param None  \b IN   :
/// @param None  \b OUT  :
/// @param None  \b RET  :
///////////////////////////////////////////////////////////////////////////////
void MDrv_MIU_Mask_Req_SC_RW(MS_U8 u8Mask, MS_U8 u8Miu)
{
    HAL_MIU_Mask_Req_SC_RW( u8Mask,  u8Miu);
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_Mask_Req_MVOP_R()
/// @brief \b Function \b Description:
/// @param None  \b IN   :
/// @param None  \b OUT  :
/// @param None  \b RET  :
///////////////////////////////////////////////////////////////////////////////
void MDrv_MIU_Mask_Req_MVOP_R(MS_U8 u8Mask, MS_U8 u8Miu)
{
    HAL_MIU_Mask_Req_MVOP_R( u8Mask,  u8Miu);
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_Mask_Req_MVD_R()
/// @brief \b Function \b Description:
/// @param None  \b IN   :
/// @param None  \b OUT  :
/// @param None  \b RET  :
///////////////////////////////////////////////////////////////////////////////
void MDrv_MIU_Mask_Req_MVD_R(MS_U8 u8Mask, MS_U8 u8Miu)
{
    HAL_MIU_Mask_Req_MVD_R( u8Mask,  u8Miu);
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_Mask_Req_MVD_W()
/// @brief \b Function \b Description:
/// @param None  \b IN   :
/// @param None  \b OUT  :
/// @param None  \b RET  :
///////////////////////////////////////////////////////////////////////////////
void MDrv_MIU_Mask_Req_MVD_W(MS_U8 u8Mask, MS_U8 u8Miu)
{
    HAL_MIU_Mask_Req_MVD_W( u8Mask,  u8Miu);
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_Mask_Req_MVD_RW()
/// @brief \b Function \b Description:
/// @param None  \b IN   :
/// @param None  \b OUT  :
/// @param None  \b RET  :
///////////////////////////////////////////////////////////////////////////////
void MDrv_MIU_Mask_Req_MVD_RW(MS_U8 u8Mask, MS_U8 u8Miu)
{
    HAL_MIU_Mask_Req_MVD_RW( u8Mask,  u8Miu);
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_Mask_Req_AUDIO_RW()
/// @brief \b Function \b Description:
/// @param None  \b IN   :
/// @param None  \b OUT  :
/// @param None  \b RET  :
///////////////////////////////////////////////////////////////////////////////
void MDrv_MIU_Mask_Req_AUDIO_RW(MS_U8 u8Mask, MS_U8 u8Miu)
{
    HAL_MIU_Mask_Req_AUDIO_RW( u8Mask,  u8Miu);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrb_MIU_InitCounter()
/// @brief \b Function \b Description:  This function clear all request mask counter of
///                                     MIU driver
/// @param None \b IN:
/// @param None \b RET:
////////////////////////////////////////////////////////////////////////////////
void MDrv_MIU_InitCounter(void)
{
    MS_U8 i;

    for(i=0;i<MIU_CLIENT_MAX;i++)
    {
        MIU0_request_mask_count[i]=0;
        MIU1_request_mask_count[i]=0;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_SwitchMIU()
/// @brief \b Function \b Description:  This function for switch miu
///
/// @param u8MiuID        \b IN     : select MIU0 or MIU1
/// @param None \b RET:
////////////////////////////////////////////////////////////////////////////////
void MDrv_MIU_SwitchMIU(MS_U8 u8MiuID)
{
      HAL_MIU_VOP_SwitchMIU(u8MiuID);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_SwitchMIU()
/// @brief \b Function \b Description:  This function for switch miu
///
/// @param u8MiuID        \b IN     : select MIU0 or MIU1
/// @param None \b RET:
////////////////////////////////////////////////////////////////////////////////
MS_BOOL MDrv_MIU_SelMIU(eMIUClientID eClientID, eMIU_SelType eType)
{
    MS_BOOL ret = TRUE;

    MDrv_MIU_MaskReq(0, eClientID);
    MDrv_MIU_MaskReq(1, eClientID);
    ret &= HAL_MIU_SelMIU(eType, eClientID);
    if (MIU_SELTYPE_MIU_ALL == eType)
    {
        MDrv_MIU_UnMaskReq(0, eClientID);
        MDrv_MIU_UnMaskReq(1, eClientID);
    }
    else
        MDrv_MIU_UnMaskReq((MS_U8)eType, eClientID);

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_IsSupportMIU1()
/// @brief \b Function \b Description:  This function for checking if support miu1
///
/// @param None \b RET:   TRUE: Support miu1 FALSE: no miu1
////////////////////////////////////////////////////////////////////////////////
MS_BOOL MDrv_MIU_IsSupportMIU1(void)
{
      return HAL_MIU_IsInitMiu1();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_MIU_GetBusWidth()
/// @brief \b Function \b Description:  This function for querying data bus width
///
/// @param None \b RET:   0: not support or 64 or 128 bits
////////////////////////////////////////////////////////////////////////////////
MS_U16 MDrv_MIU_GetBusWidth(void)
{
    return HAL_MIU_IsI64Mode();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_MIU_GetClientInfo()
/// @brief \b Function  \b Description:  This function for querying client ID info
/// @param u8MiuDev     \b IN   : select MIU0 or MIU1
/// @param eClientID    \b IN   : Client ID
/// @param pInfo        \b OUT  : Client Info
/// @param None \b RET:   0: Fail 1: Ok
////////////////////////////////////////////////////////////////////////////////
MS_BOOL MDrv_MIU_GetClientInfo(MS_U8 u8MiuDev, eMIUClientID eClientID, MIU_ClientInfo *pInfo)
{
    MS_S16 sVal;
    if (!pInfo)
        return FALSE;

    sVal = HAL_MIU_GetClientInfo(u8MiuDev, eClientID);
    if (sVal < 0)
        return FALSE;
    pInfo->u8BitPos = (MS_U8)MIU_GET_CLIENT_POS(sVal);
    pInfo->u8Gp = (MS_U8)MIU_GET_CLIENT_GROUP(sVal);
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_MIU_GetProtectInfo()
/// @brief \b Function  \b Description:  This function for querying client ID info
/// @param u8MiuDev     \b IN   : select MIU0 or MIU1
/// @param eClientID    \b IN   : Client ID
/// @param pInfo        \b OUT  : Client Info
/// @param None \b RET:   0: Fail 1: Ok
////////////////////////////////////////////////////////////////////////////////
MS_BOOL MDrv_MIU_GetProtectInfo(MS_U8 u8MiuDev, MIU_PortectInfo *pInfo)
{
    return HAL_MIU_GetProtectInfo(u8MiuDev, pInfo);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_MIU_GetProtectInfo()
/// @brief \b Function  \b Description:  This function for querying client ID info
/// @param u8MiuDev     \b IN   : select MIU0 or MIU1
/// @param eClientID    \b IN   : Client ID
/// @param pInfo        \b OUT  : Client Info
/// @param None \b RET:   0: Fail 1: Ok
////////////////////////////////////////////////////////////////////////////////
MS_BOOL MDrv_MIU_SetGroupPriority(MS_U8 u8MiuDev, MIU_GroupPriority sPriority)
{
    return HAL_MIU_SetGroupPriority(u8MiuDev, sPriority);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_MIU_SetHPriorityMask()
/// @brief \b Function  \b Description:  Set miu high priority mask
/// @param u8MiuDev     \b IN   : select MIU0 or MIU1
/// @param eClientID    \b IN   : Client ID
/// @param pInfo        \b OUT  : Client Info
/// @param None \b RET:   0: Fail 1: Ok
////////////////////////////////////////////////////////////////////////////////
MS_BOOL MDrv_MIU_SetHPriorityMask(MS_U8 u8MiuDev, eMIUClientID eClientID, MS_BOOL bMask)
{
    return HAL_MIU_SetHPriorityMask(u8MiuDev, eClientID, bMask);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_MIU_GetLibVer
/// @brief \b Function  \b Description: Show the MIU driver version
/// @param ppVersion    \b Out: Library version string
/// @return             \b 0: Fail 1: Ok
////////////////////////////////////////////////////////////////////////////////
MS_BOOL MDrv_MIU_GetLibVer(const MSIF_Version **ppVersion)
{
    if (!ppVersion)
        return FALSE;

    *ppVersion = &_drv_miu_version;
    return TRUE;
}

