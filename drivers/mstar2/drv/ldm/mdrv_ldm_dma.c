//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2012 Mstar Semiconductor, Inc.
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
/// file    mdrv_ldm_dma.c
/// @brief  local dimming  DMA Driver 
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
#if (defined (CONFIG_HAS_LD_DMA_MODE))

#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/types.h>
#include <asm/io.h>
#include <mstar/mstar_chip.h>
#include "mst_devid.h"
#include <linux/slab.h>
#include "mdrv_mstypes.h"
#include "mdrv_types.h"
#include "mhal_ldm_dma.h"


MS_U8 MDrv_LDM_DMA_SetMenuloadNumber(LDMA_CH_e eCH, MS_U32 u32MenuldNum )
{
    return Mhal_LDMA_SetMenuloadNumber( eCH,  u32MenuldNum);
}

MS_U8 MDrv_LDM_DMA_SetSPICommandFormat( LDMA_CH_e eCH,MS_U8 u8CmdLen, MS_U16* pu16CmdBuf )
{
    return  Mhal_LDMA_SetSPICommandFormat( eCH, u8CmdLen,  pu16CmdBuf  );

}

MS_U8 MDrv_LDMA_SetCheckSumMode(LDMA_CH_e eCH,LDMA_CheckSumMode_e eSumMode)
{
    return  Mhal_LDMA_SetCheckSumMode( eCH, eSumMode);
}

MS_U8 MDrv_LDM_DMA_SetSpiTriggerMode( LDMA_CH_e eCH, LDMA_SPITriggerMode_e eTriggerMode )
{
    return Mhal_LDMA_SetSPITriggerMode( eCH,  eTriggerMode);
}

MS_U8 MDrv_LDMA_SetTrigDelay(LDMA_CH_e eCH,LDMA_TrigDelay_st *pstTrigDelay)
{
    return  Mhal_LDMA_SetTrigDelay(eCH, pstTrigDelay);
}

MS_U8 MDrv_LDMA_EnableCS(LDMA_CH_e eCH, MS_BOOL bEnable)
{
    return  Mhal_LDMA_EnableCS( eCH,  bEnable);
}

MS_U8 MDrv_LDMA_SetSPI16BitMode(LDMA_CH_e eCH, MS_BOOL bEnable)
{
    return  Mhal_LDMA_SetSPI16BitMode( eCH,  bEnable);
}

MS_U8  MDrv_LDM_DMA_Init(LDMA_CH_e eCH)
{
    return  Mhal_LDMA_Init( eCH);
}

#endif

