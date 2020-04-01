////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    drvIRQ.c
/// @brief  IRQ Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#include "MsCommon.h"
#include "drvIRQ.h"
#include "halIRQ.h"
#include "drvMMIO.h"
#include "MsVersion.h"
#include "regIRQ.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  External Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static MSIF_Version _drv_drvirq_version = {
#if defined( MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
	{ DRVIRQ_DRV_VERSION },
#else
    .DDI = { DRVIRQ_DRV_VERSION },
#endif
};
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_MSOS_GetLibVer
/// @brief \b Function  \b Description: Show the MSOS driver version
/// @param ppVersion    \b Out: Library version string
/// @return             \b Result
////////////////////////////////////////////////////////////////////////////////
MS_BOOL MDrv_IRQ_GetLibVer(const MSIF_Version **ppVersion)
{
    if (!ppVersion)
        return FALSE;

    *ppVersion = &_drv_drvirq_version;
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_IRQ_Init()
{
#if defined (MSOS_TYPE_NOS)
    MS_U32 u32Base, u32Size;

    if (MDrv_MMIO_GetBASE(&u32Base, &u32Size, MS_MODULE_IRQ))
    {
        HAL_IRQ_Set_IOMap(u32Base);
        HAL_IRQ_Init();
        return TRUE;
    }
#endif // #if defined (MSOS_TYPE_NOS)
    return FALSE;
}

void MDrv_IRQ_Attach(MS_U32 u32Vector, void *pIntCb)
{
#if defined (MSOS_TYPE_NOS)
    HAL_IRQ_Attach(u32Vector, pIntCb);
#endif // #if defined (MSOS_TYPE_NOS)
}

void MDrv_IRQ_Detech(MS_U32 u32Vector)
{
#if defined (MSOS_TYPE_NOS)
    HAL_IRQ_Detech(u32Vector);
#endif // #if defined (MSOS_TYPE_NOS)
}

void MDrv_IRQ_Restore()
{
#if defined (MSOS_TYPE_NOS)
    HAL_IRQ_Restore();
#endif // #if defined (MSOS_TYPE_NOS)
}

void MDrv_IRQ_MaskAll()
{
#if defined (MSOS_TYPE_NOS)
    HAL_IRQ_MaskAll(TRUE);
#endif // #if defined (MSOS_TYPE_NOS)
}

void MDrv_IRQ_Mask(MS_U32 u32Vector)
{
#if defined (MSOS_TYPE_NOS)
    HAL_IRQ_Mask(u32Vector);
#endif // #if defined (MSOS_TYPE_NOS)
}

void MDrv_IRQ_UnMaskAll( )
{
#if defined (MSOS_TYPE_NOS)
    HAL_IRQ_MaskAll(FALSE);
#endif // #if defined (MSOS_TYPE_NOS)
}

void MDrv_IRQ_UnMask(MS_U32 u32Vector)
{
#if defined (MSOS_TYPE_NOS) || defined (MSOS_TYPE_CE)
    HAL_IRQ_UnMask(u32Vector);
#endif // #if defined (MSOS_TYPE_NOS)
}

void MDrv_IRQ_NotifyCpu(IRQ_CPU_TYPE eType)
{
#if defined (MSOS_TYPE_NOS)
    HAL_IRQ_NotifyCpu(eType);
#endif // #if defined (MSOS_TYPE_NOS)
}

MS_BOOL MDrv_IRQ_InISR()
{
#if defined (MSOS_TYPE_NOS)
    return HAL_IRQ_InISR();
#else
    return FALSE;
#endif // #if defined (MSOS_TYPE_NOS)
}

