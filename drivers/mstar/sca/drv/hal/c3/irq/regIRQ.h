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

#ifndef _REG_IRQ_H_
#define _REG_IRQ_H_


//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define RIUBASE_IRQ         0x1900   //(0x101900-0x100000)
#define RIUBASE_IRQ_EXP     0x1900   //(0x101900-0x100000)
#define RIUBASE_IRQ_FRC     0x201100 //(0x301100-0x100000)
#define RIUBASE_IRQ_EXP_FRC 0x201100 //(0x301100-0x100000)

//################
//
// For MIPS+R2 in FRC
// Host 0 : MIPS
// Host 1 : FRC-R2 (V)
//
#define REG_AEON_C_FIQ_MASK                     (RIUBASE_IRQ_FRC + (0x24*2))
#define REG_AEON_C_FIQ_CLR                      (RIUBASE_IRQ_FRC + (0x2C*2))
#define REG_AEON_FIQ_FINAL_STATUS               (RIUBASE_IRQ_FRC + (0x2C*2))

#define REG_AEON_C_IRQ_MASK                     (RIUBASE_IRQ_FRC + (0x34*2))
#define REG_AEON_IRQ_FINAL_STATUS               (RIUBASE_IRQ_FRC + (0x3C*2))

#define REG_AEON_C_FIQ_EXP_MASK                 (RIUBASE_IRQ_EXP_FRC + (0x26*2)) //NOT EXIST NOW
#define REG_AEON_C_FIQ_EXP_CLR                  (RIUBASE_IRQ_EXP_FRC + (0x2E*2)) //NOT EXIST NOW
#define REG_AEON_C_FIQ_EXP_FINAL_STATUS         (RIUBASE_IRQ_EXP_FRC + (0x2E*2)) //NOT EXIST NOW

#define REG_AEON_C_IRQ_EXP_MASK                 (RIUBASE_IRQ_EXP_FRC + (0x36*2)) //NOT EXIST NOW
#define REG_AEON_C_IRQ_EXP_FINAL_STATUS         (RIUBASE_IRQ_EXP_FRC + (0x3E*2)) //NOT EXIST NOW

//################
//
// For PM+MIPS in NonPM
// Host 0 : PM
// Host 1 : MIPS (V)
//
#define REG_MIPS_C_FIQ_MASK                     (RIUBASE_IRQ + 0x24*2)
#define REG_MIPS_C_FIQ_CLR                      (RIUBASE_IRQ + 0x2C*2)
#define REG_MIPS_FIQ_FINAL_STATUS               (RIUBASE_IRQ + 0x2C*2)

#define REG_MIPS_C_IRQ_MASK                     (RIUBASE_IRQ + 0x34*2)
#define REG_MIPS_IRQ_FINAL_STATUS               (RIUBASE_IRQ + 0x3C*2)

#define REG_MIPS_C_FIQ_EXP_MASK                 (RIUBASE_IRQ_EXP + 0x26*2)
#define REG_MIPS_C_FIQ_EXP_CLR                  (RIUBASE_IRQ_EXP + 0x2E*2)
#define REG_MIPS_C_FIQ_EXP_FINAL_STATUS         (RIUBASE_IRQ_EXP + 0x2E*2)

#define REG_MIPS_C_IRQ_EXP_MASK                 (RIUBASE_IRQ_EXP + 0x36*2)
#define REG_MIPS_C_IRQ_EXP_FINAL_STATUS         (RIUBASE_IRQ_EXP + 0x3E*2)


#ifdef MCU_AEON
#define REG_C_FIQ_MASK                          REG_AEON_C_FIQ_MASK
#define REG_C_FIQ_CLR                           REG_AEON_C_FIQ_CLR
#define REG_FIQ_FINAL_STATUS                    REG_AEON_FIQ_FINAL_STATUS

#define REG_C_IRQ_MASK                          REG_AEON_C_IRQ_MASK
#define REG_IRQ_FINAL_STATUS                    REG_AEON_IRQ_FINAL_STATUS

#define REG_C_FIQ_EXP_MASK                      REG_AEON_C_FIQ_EXP_MASK
#define REG_C_FIQ_EXP_CLR                       REG_AEON_C_FIQ_EXP_CLR
#define REG_C_FIQ_EXP_FINAL_STATUS              REG_AEON_C_FIQ_EXP_FINAL_STATUS

#define REG_C_IRQ_EXP_MASK                      REG_AEON_C_IRQ_EXP_MASK
#define REG_C_IRQ_EXP_FINAL_STATUS              REG_AEON_C_IRQ_EXP_FINAL_STATUS
#endif


#ifdef MCU_MIPS_34K
#define REG_C_FIQ_MASK                          REG_MIPS_C_FIQ_MASK
#define REG_C_FIQ_CLR                           REG_MIPS_C_FIQ_CLR
#define REG_FIQ_FINAL_STATUS                    REG_MIPS_FIQ_FINAL_STATUS

#define REG_C_IRQ_MASK                          REG_MIPS_C_IRQ_MASK
#define REG_IRQ_FINAL_STATUS                    REG_MIPS_IRQ_FINAL_STATUS

#define REG_C_FIQ_EXP_MASK                      REG_MIPS_C_FIQ_EXP_MASK
#define REG_C_FIQ_EXP_CLR                       REG_MIPS_C_FIQ_EXP_CLR
#define REG_C_FIQ_EXP_FINAL_STATUS              REG_MIPS_C_FIQ_EXP_FINAL_STATUS

#define REG_C_IRQ_EXP_MASK                      REG_MIPS_C_IRQ_EXP_MASK
#define REG_C_IRQ_EXP_FINAL_STATUS              REG_MIPS_C_IRQ_EXP_FINAL_STATUS
#endif


#ifdef MCU_MIPS_4KE

#define REG_C_FIQ_MASK                          REG_MIPS_C_FIQ_MASK
#define REG_C_FIQ_CLR                           REG_MIPS_C_FIQ_CLR
#define REG_FIQ_FINAL_STATUS                    REG_MIPS_FIQ_FINAL_STATUS

#define REG_C_IRQ_MASK                          REG_MIPS_C_IRQ_MASK
#define REG_IRQ_FINAL_STATUS                    REG_MIPS_IRQ_FINAL_STATUS

#define REG_C_FIQ_EXP_MASK                      REG_MIPS_C_FIQ_EXP_MASK
#define REG_C_FIQ_EXP_CLR                       REG_MIPS_C_FIQ_EXP_CLR
#define REG_C_FIQ_EXP_FINAL_STATUS              REG_MIPS_C_FIQ_EXP_FINAL_STATUS

#define REG_C_IRQ_EXP_MASK                      REG_MIPS_C_IRQ_EXP_MASK
#define REG_C_IRQ_EXP_FINAL_STATUS              REG_MIPS_C_IRQ_EXP_FINAL_STATUS
#endif

#ifdef MCU_MIPS_74K
#define REG_C_FIQ_MASK                          REG_MIPS_C_FIQ_MASK
#define REG_C_FIQ_CLR                           REG_MIPS_C_FIQ_CLR
#define REG_FIQ_FINAL_STATUS                    REG_MIPS_FIQ_FINAL_STATUS

#define REG_C_IRQ_MASK                          REG_MIPS_C_IRQ_MASK
#define REG_IRQ_FINAL_STATUS                    REG_MIPS_IRQ_FINAL_STATUS

#define REG_C_FIQ_EXP_MASK                      REG_MIPS_C_FIQ_EXP_MASK
#define REG_C_FIQ_EXP_CLR                       REG_MIPS_C_FIQ_EXP_CLR
#define REG_C_FIQ_EXP_FINAL_STATUS              REG_MIPS_C_FIQ_EXP_FINAL_STATUS

#define REG_C_IRQ_EXP_MASK                      REG_MIPS_C_IRQ_EXP_MASK
#define REG_C_IRQ_EXP_FINAL_STATUS              REG_MIPS_C_IRQ_EXP_FINAL_STATUS
#endif

#ifdef MCU_ARM_9
#define REG_C_FIQ_MASK                          REG_MIPS_C_FIQ_MASK
#define REG_C_FIQ_CLR                           REG_MIPS_C_FIQ_CLR
#define REG_FIQ_FINAL_STATUS                    REG_MIPS_FIQ_FINAL_STATUS

#define REG_C_IRQ_MASK                          REG_MIPS_C_IRQ_MASK
#define REG_IRQ_FINAL_STATUS                    REG_MIPS_IRQ_FINAL_STATUS

#define REG_C_FIQ_EXP_MASK                      REG_MIPS_C_FIQ_EXP_MASK
#define REG_C_FIQ_EXP_CLR                       REG_MIPS_C_FIQ_EXP_CLR
#define REG_C_FIQ_EXP_FINAL_STATUS              REG_MIPS_C_FIQ_EXP_FINAL_STATUS

#define REG_C_IRQ_EXP_MASK                      REG_MIPS_C_IRQ_EXP_MASK
#define REG_C_IRQ_EXP_FINAL_STATUS              REG_MIPS_C_IRQ_EXP_FINAL_STATUS
#endif

/*
#define REG_SEND_IRQ_FROM_CPU0                  (RIUBASE_IRQ + 0x7C)
#define REG_SEND_IRQ_FROM_CPU1                  (RIUBASE_IRQ + 0x7E)
#define REG_SEND_IRQ_FROM_CPU2                  (RIUBASE_IRQ_AEON1 + 0x7E)
*/

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------


#endif // _REG_IRQ_H_

