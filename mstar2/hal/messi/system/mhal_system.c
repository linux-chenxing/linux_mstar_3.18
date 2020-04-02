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
/// file    drv_system_io.c
/// @brief  System Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/types.h>

#include "mdrv_types.h"
#include "mst_platform.h"
#include "mhal_system.h"
#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
extern bool mstar_mci_exit_checkdone_ForCI(void);
#endif

// NOTE
// PE capability :
// PAD as output
//  PE = 0 / 1, don't care
// PAD as input
//  PE = 0, Hi-Z (input disable)
//  PE = 1, internal pull up
//
// for PAD = PCM_A(output), PE don't care
// for PAD = PCM_D(input/output), PE must be 1

#define PAD_PCM_D_ENABLE() \
    REG_ADDR(REG_CHIP_PCM_PE)   |= REG_CHIP_PCM_PE_MASK;

#define PAD_PCM_A_ENABLE() \
    REG_ADDR(REG_CHIP_PCM_PE1)   |= REG_CHIP_PCM_PE1_MASK;

#define PAD_PCM_D_DISABLE() \
    REG_ADDR(REG_CHIP_PCM_PE)   &= ~REG_CHIP_PCM_PE_MASK;

#define PAD_PCM_A_DISABLE() \
    REG_ADDR(REG_CHIP_PCM_PE1)   &= ~REG_CHIP_PCM_PE1_MASK;

static U16 u16TmpReg[7];

#if defined(CONFIG_MMC_MSTAR_MMC_EMMC) && CONFIG_MMC_MSTAR_MMC_EMMC
BOOL MHal_PCMCIA_WaitEMMCDone(U32 u32loopCnt)
{
    return mstar_mci_exit_checkdone_ForCI();
}
#endif

void MHal_PCMCIA_SetPad(BOOL bRestore)
{
    if(bRestore == FALSE)
    {
        //u16Bypass = REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) & REG_CHIP_PCM_NAND_BYPASS_MASK;
        u16TmpReg[0] = REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) & REG_CHIP_PCM_NAND_BYPASS_MASK;
        REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) |= REG_CHIP_PCM_NAND_BYPASS_ENABLE;

        //u16RegVal1 = REG_ADDR(REG_CHIP_PCMCFG) & REG_CHIP_PCMCFG_MASK;
        u16TmpReg[3] = REG_ADDR(REG_CHIP_PCMCFG) & REG_CHIP_PCMCFG_MASK;
        REG_ADDR(REG_CHIP_PCMCFG) |= REG_CHIP_PCMCFG_CTRL_EN;

        // disable PCM_A PE for NEOTION CAM issue (mantis id:0365320)
        PAD_PCM_A_DISABLE();
        PAD_PCM_D_ENABLE();
    }
    else
    {
        // restore padmux to original
        PAD_PCM_D_DISABLE();

        REG_ADDR(REG_CHIP_PCMCFG) = (REG_ADDR(REG_CHIP_PCMCFG) & ~REG_CHIP_PCMCFG_MASK) | u16TmpReg[3];

        REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) = 
            (REG_ADDR(REG_CHIP_PCM_NAND_BYPASS) & ~(REG_CHIP_PCM_NAND_BYPASS_MASK)) | u16TmpReg[0];;

    }

}
