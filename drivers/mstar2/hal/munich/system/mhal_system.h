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
/// @file   mhal_system.h
/// @brief  System Control Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "mst_platform.h"

#ifndef _HAL_SYSTEM_H_
#define _HAL_SYSTEM_H_

//Fix me!!Needs to be reivewed!!
#define REG_ADDR(addr)                         (*((volatile U16*)(0xFD200000 + (addr << 1))))
#define REG_CKG_AEONTS0                         0x3360
#define AEON_CLK_ENABLE                         0x0000 //CLK 216MHz
#define AEON_CLK_DISABLE                        0x0001
#define MIU_PROTECT_EN                          0x1230
#define MIU_PROTECT3_ID0                        0x1232
#define MIU_PROTECT3_START_ADDR_H               0x1234
#define MIU_PROTECT3_END_ADDR_H                 0x1236
#define MIU_CLI_AEON_RW                         0x0004
#define MIU_PROTECT_4                           0x0001
#define MBX_AEON_JUDGEMENT                      0x33DE
#define MHEG5_CPU_STOP                          0x0FE6
#define STOP_AEON                               0x0001
#define MHEG5_REG_IOWINORDER                    0x0F80
#define REG_AEON_C_FIQ_MASK_L                   0x1148
#define REG_AEON_C_FIQ_MASK_H                   0x114A
#define REG_AEON_C_IRQ_MASK_L                   0x1168
#define REG_AEON_C_IRQ_MASK_H                   0x116A
#define REG_MAU0_MIU0_SIZE                      0x1842

#define REG_CHIP_NAND_MODE						0x1EDE
	#define REG_CHIP_NAND_MODE_MASK				0x00C0
	#define REG_CHIP_NAND_MODE_PCMA				0x0080
	#define REG_CHIP_NAND_MODE_PCMD				0x0040
#define REG_CHIP_PCMCFG                         0x1EC8
    #define REG_CHIP_PCMCFG_MASK                0x0030
    #define REG_CHIP_PCMCFG_CTRL_EN             0x0030 //AD pads & Ctrl pads enable

#define REG_SD_CONFIG2                          0x1EB4
    #define REG_SD_CONFIG2_MASK                 0x0F00 //reg_sd_config & reg_sd_config2
#define REG_EMMC_CONFIG                         0x1EDC
    #define REG_EMMC_CONFIG_MASK                0x00C0

#define REG_CHIP_PCM_PE                         0x1E12
    #define REG_CHIP_PCM_PE_MASK                0x00FF // PCM_D
    #define REG_CHIP_PCM_CTL_MASK               0xFF00 // PCM_CTRL (except PCM_CD, PCM_RESET)
#define REG_CHIP_PCM_PE1                        0x1E14
    #define REG_CHIP_PCM_PE1_MASK               0x00FF // PCM_A
#define REG_CHIP_PCM_D_PE                       0x1E16
    #define REG_CHIP_PCM_D_PE_MASK              0x0003
    
#define REG_CHIP_PCM_NAND_BYPASS                0x1E20
    #define REG_CHIP_PCM_NAND_BYPASS_MASK       0x0100
    #define REG_CHIP_PCM_NAND_BYPASS_ENABLE     0x0100
    #define REG_CHIP_PCM_NAND_BYPASS_DISABLE    0x0000
    
#define ARM_MIU0_BUS_BASE                      0x20000000
#define ARM_MIU1_BUS_BASE                      0xA0000000
#define ARM_MIU0_BASE                          0x00000000
#define ARM_MIU1_BASE                          0x80000000

#define ARM_MIU2_BUS_BASE                      0xFFFFFFFF
#define ARM_MIU3_BUS_BASE                      0xFFFFFFFF
#define ARM_MIU2_BASE                          0xFFFFFFFF
#define ARM_MIU3_BASE                          0xFFFFFFFF

void MHal_PCMCIA_SetPad(BOOL bRestore);
BOOL MHal_PCMCIA_WaitEMMCDone(U32 u32loopCnt);

#endif // _HAL_SYSTEM_H_

