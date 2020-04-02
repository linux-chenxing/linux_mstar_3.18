/***************************************************************************************************************
 *
 * FileName hal_sd_regs.h
 *     @author jeremy.wang (2017/12/12)
 * Desc:
 *     This file is the header file of hal_sd_regs.c.
 *
 *     For Base RegisterSetting:
 *     (1) BASE Register Address
 *     (2) BASE Register Operation
 *     (3) BASE FCIE Reg Meaning Position (Including File)
 *
 *
 ***************************************************************************************************************/

#ifndef __HAL_SD_REGS_H
#define __HAL_SD_REGS_H

#include "hal_sd_base.h"

//***********************************************************************************************************
// (1) BASE Register Address
//***********************************************************************************************************


//###########################################################################################################
#if (D_PROJECT == D_PROJECT__iNF2)
//###########################################################################################################

#define A_RIU_BASE          (0x1F000000)

#define A_FCIE1_0_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xB8500)   //BANK: 0x170A  (SDIO30_0)
#define A_FCIE1_1_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xB8580)   //BANK: 0x170B  (SDIO30_1)
#define A_FCIE1_2_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xB8600)   //BANK: 0x170C  (SDIO30_2)
#define A_FCIE1_3_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xB8680)   //BANK: 0x170D  (SDIO30_3)

#define A_FCIE2_0_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x88F80)   //BANK: 0x111F  (SDIO20_0)
#define A_FCIE2_1_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x89000)   //BANK: 0x1120  (SDIO20_1)
#define A_FCIE2_2_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x89080)   //BANK: 0x1121  (SDIO20)2)
#define A_FCIE2_3_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xB8680)   //BANK: 0x170D  (No Design)

#define A_FCIE3_0_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0980)   //IP
#define A_FCIE3_1_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0A00)   //IP
#define A_FCIE3_2_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0A80)   //IP
#define A_FCIE3_3_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0B00)   //IP

//###########################################################################################################
#elif (D_PROJECT == D_PROJECT__C4P)
//###########################################################################################################

#define A_RIU_BASE          (0x1F000000)

#define A_FCIE1_0_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x00A00)   //IP SDIO0
#define A_FCIE1_1_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x00A80)
#define A_FCIE1_2_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x00B00)
#define A_FCIE1_3_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x09000)   //IP SD_PLL0

#define A_FCIE2_0_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x09100)   //IP SDIO1
#define A_FCIE2_1_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x09180)
#define A_FCIE2_2_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x09200)
#define A_FCIE2_3_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x09080)   //IP SD_PLL1

#define A_FCIE3_0_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0980)   //IP
#define A_FCIE3_1_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0A00)   //IP
#define A_FCIE3_2_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0A80)   //IP
#define A_FCIE3_3_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0B00)   //IP

//###########################################################################################################
#elif (D_PROJECT == D_PROJECT__MV5)
//###########################################################################################################

#define A_RIU_BASE          (0x1F000000)

#define A_FCIE1_0_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0980)   //IP SDIO30
#define A_FCIE1_1_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0A00)   //IP SDIO30
#define A_FCIE1_2_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0A80)   //IP SDIO30
#define A_FCIE1_3_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0B00)   //IP SDIO30_PLL

#define A_FCIE2_0_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0800)   //IP
#define A_FCIE2_1_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0880)   //IP
#define A_FCIE2_2_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0900)   //IP
#define A_FCIE2_3_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0900)   //IP NOPLL

#define A_FCIE3_0_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0800)   //IP
#define A_FCIE3_1_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0880)   //IP
#define A_FCIE3_2_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0900)   //IP
#define A_FCIE3_3_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0900)   //IP

//###########################################################################################################
#elif (D_PROJECT == D_PROJECT__iNF3) || (D_PROJECT == D_PROJECT__iNF)
//###########################################################################################################

#define A_RIU_BASE          (0x1F000000)

#define A_FCIE1_0_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0800)      //SDIO0_0_BANK
#define A_FCIE1_1_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0880)      //SDIO0_1_BANK
#define A_FCIE1_2_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0900)      //SDIO0_2_BANK

#define A_FCIE2_0_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0100)      //FCIE0_0_BANK
#define A_FCIE2_1_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0180)      //FCIE0_1_BANK
#define A_FCIE2_2_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0200)      //FCIE0_2_BANK

#define A_FCIE3_0_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0100)
#define A_FCIE3_1_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0180)
#define A_FCIE3_2_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0xA0200)

//###########################################################################################################
#endif


//***********************************************************************************************************
// (2) BASE Register Operation
//***********************************************************************************************************

//###########################################################################################################
#if (D_OS == D_OS__LINUX)
//###########################################################################################################
    //#if (D_PROJECT == D_PROJECT__C3) || (D_PROJECT == D_PROJECT__C4) || (D_PROJECT == D_PROJECT__iNF)
        #include "../../../mstar/include/ms_platform.h"  //IO_ADDRESS       // IO Mapping Address
        #define IO_MAPADDR(Reg_Addr)    IO_ADDRESS(Reg_Addr)
    //#endif
//###########################################################################################################
#else
//###########################################################################################################
#define IO_MAPADDR(Reg_Addr)    Reg_Addr
//###########################################################################################################
#endif

#define D_MIU_WIDTH                            8    // Special MIU WIDTH for FCIE4
#define REG_OFFSET_BITS                        2    // Register Offset Byte  (2= 4byte = 32bits)
#define GET_CARD_REG_ADDR(x, y)                ((x)+((y) << REG_OFFSET_BITS))

#define CARD_REG(Reg_Addr)                     (*(volatile U16_T*)(IO_MAPADDR(Reg_Addr)) )
#define CARD_REG_L8(Reg_Addr)                  (*(volatile U8_T*)(IO_MAPADDR(Reg_Addr)) )
#define CARD_REG_H8(Reg_Addr)                  (*( (volatile U8_T*)(IO_MAPADDR(Reg_Addr))+1) )

#define CARD_REG_SETBIT(Reg_Addr, Value)       CARD_REG(Reg_Addr) |= (Value)
#define CARD_REG_CLRBIT(Reg_Addr, Value)       CARD_REG(Reg_Addr) &= (~(Value))

#define CARD_REG_L8_SETBIT(Reg_Addr, Value)    CARD_REG_L8(Reg_Addr) |= (Value)
#define CARD_REG_H8_SETBIT(Reg_Addr, Value)    CARD_REG_H8(Reg_Addr) |= (Value)
#define CARD_REG_L8_CLRBIT(Reg_Addr, Value)    CARD_REG_L8(Reg_Addr) &= (~(Value))
#define CARD_REG_H8_CLRBIT(Reg_Addr, Value)    CARD_REG_H8(Reg_Addr) &= (~(Value))

#define CARD_BANK(Bank_Addr)                   IO_MAPADDR(Bank_Addr)

volatile void* Hal_CREG_GET_REG_BANK(IPEmType eIP, U8_T u8Bank);
void Hal_CREG_SET_PORT(IPEmType eIP, PortEmType ePort);
volatile PortEmType Hal_CREG_GET_PORT(IPEmType eIP);

#define GET_SD_BANK                            Hal_CREG_GET_REG_BANK
#define GET_SD_PORT                            Hal_CREG_GET_PORT
#define SET_SD_PORT                            Hal_CREG_SET_PORT

//***********************************************************************************************************
// (3) BASE FCIE Reg Meaning Position (Including File)
//***********************************************************************************************************

//###########################################################################################################
#if (D_FCIE_M_VER == D_FCIE_M_VER__04)
//###########################################################################################################
#include "hal_sd_regs_v4.h"
//###########################################################################################################
#elif (D_FCIE_M_VER == D_FCIE_M_VER__05)
//###########################################################################################################
#include "hal_sd_regs_v5.h"
//###########################################################################################################
#endif


#endif //End of __HAL_SD_REGS_H
