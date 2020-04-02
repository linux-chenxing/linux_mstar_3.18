/*
**  Important : This software is protected by Copyright and the information
**              contained herein is confidential. Distribution, reproduction
**              as well as exploitation and transmission of its contents is
**              not allowed except if expressly permitted. Infringments
**              result in damage claims.
**              Copyright MSTAR SAS (c) 2007
*/
/////////////////////////////////////////////////////////////////////////
//                                                                     //
// Asic          : iNfinity3                                           //
// File name     : cpu_mem_map_infinity.h                             //
// Author        :                                                 //
// Purpose       :                                                     //
// hierarchy     :                                                     //
//                                                                     //
// Notes         :                                                     //
// Synthetisable : yes                                                 //
//                                                                     //
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// Notes:                                                              //
//                                                                     //
//    This file is the definition of the cpu memory map. All base      //
//    addresses of AHB slaves and peripherals (through the bridge) are //
//    defined here.                                                    //
//                                                                     //
/////////////////////////////////////////////////////////////////////////
#ifndef __CPU_MEM_MAP_INFINITY2_H__
#define __CPU_MEM_MAP_INFINITY2_H__


//=======================================================================
// Definition of the number of slaves on AHB bus.
// This number does not include default slave.
//=======================================================================

// there is 13 slaves defined :
// - On Chip ROM
// - On Chip RAM
// - MEM0
// - MEM1
// - MEM2
// - MEM3
// - USR0
// - USR1
// - USR2
// - PERIPH
// - PERIPH 32 BITS
// - DPRAM_DSP
// - SHIFACE

#define NB_AHB_SLAVES 17
#define MAIN_OFFSET		0x00000000


//=======================================================================
// Definition of the HADDR bits used to generate the Chip Select signals
// of the different AHB slaves
//=======================================================================
// HADDR[AHB_ADD_MSB:AHB_ADD_LSB] is used by the address decoder to
// generate the chip select
#define AHB_ADD_MSB     30
#define AHB_ADD_LSB     25


//MSB or LSB can range from 0 to 31 -> 5 bit are required to code IDX
#define XSB_IDX_WIDTH 5


//=======================================================================
// Definition of the addresses ranges
// note that adresses out of range will select
// default slave.
//=======================================================================

// COMMON space definition

#define COMMON_START_ADDRESS       0x00000000
#define COMMON_END_ADDRESS         0x03FFFFFF
#define COMMON_ADD_LSB             0x1A //26 //64 Mbytes boundary


#define SPI_START_ADDRESS          0x14000000
#define SPI_END_ADDRESS            0x15FFFFFF

#define GIC_START_ADDRESS          0x16000000
#define GIC_END_ADDRESS            0x16003FFF


#define IO_START_ADDRESS           0x1F000000
#define IO_END_ADDRESS             0x1F03FFFF

#define IMI_START_ADDRESS          0xA0000000
#define IMI_END_ADDRESS            0xA0015FFF //I1 88k
//#define IMI_END_ADDRESS            0xA0013FFF //I3 128k


// On chip ROM space definition (through IMI)
/*
#define IROM_START_ADDRESS         0x04000000
#define IROM_END_ADDRESS           0x07FFFFFF
#define IROM_ADD_LSB               0x1A //26 //64 Mbytes boundary
*/

// On chip RAM space definition (through IMI)
/*
#define IRAM_START_ADDRESS         0x08000000
#define IRAM_END_ADDRESS           0x0BFFFFFF
#define IRAM_ADD_LSB               0x1A //26 //64 Mbytes boundary
*/

/* +AKO and LSM - 29/08/05 */
//#define IRAM_DSP_START_ADDRESS     0x20000
/* -AKO and LSM - 29/08/05 */

// Off chip MEM0 space definition (through EMI)
/*
#define MEM0_START_ADDRESS         0x0C000000
#define MEM0_END_ADDRESS           0x0FFFFFFF
#define MEM0_ADD_LSB               0x1A //26 //64 Mbytes boundary
*/
// Off chip MEM1 space definition (through EMI)

#define MEM1_START_ADDRESS         0x20000000
#define MEM1_END_ADDRESS           (0x20000000 + SYS_PHY_RAM_SIZE_SETTING)//0x28000000 //default 128MB
#define MEM1_ADD_LSB               0x1A //26 //64 Mbytes boundary

//Off chip MEM2 space definition (through EMI)
/*
#define MEM2_START_ADDRESS         0x14000000
#define MEM2_END_ADDRESS           0x17FFFFFF
#define MEM2_ADD_LSB               0x1A //26 //64 Mbytes boundary
*/
//Off chip MEM3 space definition (through EMI Serial Flash)

#define MEM3_START_ADDRESS         0x14000000
#define MEM3_END_ADDRESS           0x1BFFFFFF
#define MEM3_ADD_LSB               0x1A //26 //64 Mbytes boundary


//Internal SRAM. Shared with B2 and M3. Need to be configured to use
/*
#define IMI_START_ADDRESS					 0x1A000000
#define IMI_END_ADDRESS						 0x1A017FFF
#define IMI_ADD_LSB								 0x1A //26 //64 Mbytes boundary
*/
//Internal IMI2 SRAM

#define IMI2_START_ADDRESS          0xA0000000
#define IMI2_END_ADDRESS            0xA0015FFF //I1 88k
//#define IMI2_END_ADDRESS            0xA0013FFF //I3 128k

// SDRAM space definition (through EMI DMC)
/*
#define SDRAM_START_ADDRESS       MEM0_START_ADDRESS
#define SDRAM_END_ADDRESS         MEM0_END_ADDRESS
#define SDRAM_ADD_LSB             0x1A //27 //128 Mbytes boundary
*/

// Off chip USR0 space definition (through EMI)
/*
#define USR0_START_ADDRESS         0x1C000000
#define USR0_END_ADDRESS           0x1FFFFFFF
#define USR0_ADD_LSB               0x1A //26 //64 Mbytes boundary
*/
// Off chip USR1 space definition (through EMI)
/*
#define USR1_START_ADDRESS         0x20000000
#define USR1_END_ADDRESS           0x23FFFFFF
#define USR1_ADD_LSB               0x1A //26 //64 Mbytes boundary
*/
// Off chip USR2 space definition (through EMI)
/*
#define USR2_START_ADDRESS         0x24000000
#define USR2_END_ADDRESS           0x27FFFFFF
#define USR2_ADD_LSB               0x1A //26 //64 Mbytes boundary
*/




//======================================================================
//====================== MIU address domain ============================
//======================================================================

#define MIU_BASE_PHYADDR    MEM1_START_ADDRESS
#define MIU_ADDR_MASK       0x3FFFFFFF
#define MIU_MAX_PHYADDR     MEM1_END_ADDRESS//(MIU_BASE_PHYADDR + MIU_ADDR_MASK)

// define DUMMY address for periph which do not
// have a mapping yet.
#define TO_BE_DEFINED               0xFFFFFFFF


// Register macros
#define REG(Reg_Addr)       (*(volatile U16*)(Reg_Addr))
#define GET_REG_ADDR(x, y)  ((x) + ((y) << 2))
#define GET_BASE_ADDR_BY_BANK(x, y)  ((x) + ((y) << 1))

#define MS_BASE_GIC_DISTRIBUTOR_PA	    (0x16001000)
#define MS_BASE_GIC_CONTROLLER_PA       (0x16000100)

#define MS_BASE_PRIVATE_TIMER_PA        (0x16000600)

#define MS_BASE_REG_RIU_PA					(0x1F000000)
#define MS_BASE_REG_IMI_PA                  (0xA0000000)
#define MS_BASE_REG_FUART_PA              	GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x110200)
#define MS_BASE_REG_URDMA_PA              	GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x110300)
#define MS_BASE_REG_UART0_PA              	GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x100980)
#define MS_BASE_REG_UART1_PA              	GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x110900)
#define MS_BASE_REG_TIMER0_PA              	GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x003020)
//#define MS_BASE_REG_TIMER2_PA              	GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x003000)
#define MS_BASE_REG_CHIPTOP_PA              GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x101E00)
#define MS_BASE_REG_PADTOP0_PA              GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x102500)
#define MS_BASE_REG_PADTOP1_PA              GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x102600)
#define MS_BASE_REG_INTRCTL0_PA             GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x101900)
#define MS_BASE_REG_INTRCTL1_PA             GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x101000)
#define MS_BASE_REG_FSP_PA                  GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x0016C0)
#define MS_BASE_REG_QSPI_PA                 GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x001780)
#define MS_BASE_REG_CLKGEN_PA               GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x103800)
#define MS_BASE_REG_IIC0_PA					GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x111800)
#define MS_BASE_REG_IIC1_PA					GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x111900)
#define MS_BASE_REG_CSI_PA					GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x120400)
#define MS_BASE_REG_VIF_PA					GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x131200)
#define MS_BASE_REG_CKG_PA					GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x103800)
#define MS_BASE_REG_SC_GP_CTRL_PA			GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x113300)
#define MS_BASE_REG_PM_SLEEP_CKG_PA			GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x000E00)
#define MS_BASE_REG_MIU_PA			        GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x101200)

#define REG_ADDR_BASE_WDT                   GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x003000)
#define REG_ADDR_BASE_AESDMA                GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x112200 )
#define REG_ADDR_BASE_L3_BRIDGE             GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x102200 )
#define REG_ADDR_BASE_DID_KEY               GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x003800 )
#define REG_ADDR_BASE_BDMA                  GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x100200 )
#define REG_ADDR_BASE_RTC                   GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x001200 )



#define BASE_REG_TIMER      MS_BASE_REG_TIMER0_PA
#define TIMER0_BASE_REG     GET_REG_ADDR(BASE_REG_TIMER, 0x00)
#define TIMER1_BASE_REG     GET_REG_ADDR(BASE_REG_TIMER, 0x10)
#define TIMER2_BASE_REG     GET_REG_ADDR(BASE_REG_TIMER, 0x20)

#define BASE_REG_INTRCTL0   MS_BASE_REG_INTRCTL0_PA
#define INTRCTL0_HST0_BASE_REG       GET_REG_ADDR(BASE_REG_INTRCTL0, 0x00)
#define INTRCTL0_HST1_BASE_REG       GET_REG_ADDR(BASE_REG_INTRCTL0, 0x20)
#define INTRCTL0_HST2_BASE_REG       GET_REG_ADDR(BASE_REG_INTRCTL0, 0x40)
#define INTRCTL0_HST3_BASE_REG       GET_REG_ADDR(BASE_REG_INTRCTL0, 0x60)

#define BASE_REG_INTRCTL1   MS_BASE_REG_INTRCTL1_PA
#define INTRCTL1_HST0_BASE_REG       GET_REG_ADDR(BASE_REG_INTRCTL1, 0x00)
#define INTRCTL1_HST1_BASE_REG       GET_REG_ADDR(BASE_REG_INTRCTL1, 0x20)
#define INTRCTL1_HST2_BASE_REG       GET_REG_ADDR(BASE_REG_INTRCTL1, 0x40)
#define INTRCTL1_HST3_BASE_REG       GET_REG_ADDR(BASE_REG_INTRCTL1, 0x60)

#define UART0_BASE_REG     MS_BASE_REG_UART0_PA
#define UART1_BASE_REG     MS_BASE_REG_UART1_PA
#define FUART_BASE_REG     MS_BASE_REG_FUART_PA
#define URDMA_BASE_REG     MS_BASE_REG_URDMA_PA
#define CHIPTOP_BASE_REG   MS_BASE_REG_CHIPTOP_PA
#define PADTOP0_BASE_REG   MS_BASE_REG_PADTOP0_PA
#define PADTOP1_BASE_REG   MS_BASE_REG_PADTOP1_PA
#define CLKGEN_BASE_REG	   MS_BASE_REG_CLKGEN_PA
#define IIC0_BASE_REG      MS_BASE_REG_IIC0_PA
#define IIC1_BASE_REG	   MS_BASE_REG_IIC1_PA
#define AXI_BASE_REG       REG_ADDR_BASE_L3_BRIDGE
#define CSI_BASE_REG       MS_BASE_REG_CSI_PA
#define VIF_BASE_REG       MS_BASE_REG_VIF_PA
#define WDT_BASE_REG       REG_ADDR_BASE_WDT
#define MIU_BASE_REG       MS_BASE_REG_MIU_PA

#if 0
// On chip ETB RAM space definition (through ETB IF)
#define ETBRAM_START_ADDRESS       0x28000000
#define ETBRAM_END_ADDRESS         0x2BFFFFFF
#define ETBRAM_ADD_LSB             0x1A //26 //64 Mbytes boundary

// On chip ITCM space definition (through IMI and dedicated CPU IF)
#define ITCMRAM_START_ADDRESS      0x2C000000
#define ITCMRAM_END_ADDRESS        0x2FFFFFFF
#define ITCMRAM_ADD_LSB            0x1A //26 //64 Mbytes boundary

// On chip DTCM space definition (through IMI and dedicated CPU IF)
#define DTCMRAM_START_ADDRESS      0x30000000
#define DTCMRAM_END_ADDRESS        0x33FFFFFF
#define DTCMRAM_ADD_LSB            0x1A //26 //64 Mbytes boundary

// PERIPH space definition (through AHB to APB bridges)
#define PERIPH_START_ADDRESS       0x34000000
#define PERIPH_END_ADDRESS         0x37FFFFFF
#define PERIPH_ADD_LSB             0x19 //25 //32 Mbytes boundary

// USED BY BT ROM code
#define BT_MIRROR_PERIPH_START_ADDRESS 0xB4000000 //(0x34000000+0x80000000)

// PERIPH space definition (through AHB to RIU bridges)
#define RIUBRIDGE_START_ADDRESS    0x74000000
#define RIUBRIDGE_END_ADDRESS      0x74007FFF
#define RIUBRIDGE_ADD_LSB          0x1A //26 //64 Mbytes boundary

// GSS space definition (through AHB2AHB bridge)
#define GSS_START_ADDRESS         0x38000000
#define GSS_END_ADDRESS           0x3BFFFFFF
#define GSS_ADD_LSB               0x1A //26 //64 Mbytes boundary

// SDRAM CONFIG space definition (unfortunately must be defined as a primary space address because there is only one AHB IF on SDRAM CTRL shared between config and data).
#define PORT0_CFG_START_ADDRESS   0x3C000000
#define PORT0_CFG_END_ADDRESS     0x3FFFFFFF
#define PORT0_CFG_ADD_LSB         0x1A //26 //64 Mbytes boundary

// NAND_FLASH space definition (through NAND_FLASH controller)
#define NAND_FLASH_DPRAM_START_ADDRESS  0x48000000
#define NAND_FLASH_DPRAM_END_ADDRESS    0x4BFFFFFF
#define NAND_FLASH_DPRAM_ADD_LSB        0x1A //26 //64 Mbytes boundary

// SHIFACE_DPRAM space definition
#define RFDL_SHIFACE_DPRAM_START_ADDRESS  0x50000000
#define RFDL_SHIFACE_DPRAM_END_ADDRESS    0x53FFFFFF
#define RFDL_SHIFACE_DPRAM_ADD_LSB        0x1A //26 //64 Mbytes boundary



//=======================================================================
// Definition of the sub addresses
// for the bridges.
//=======================================================================

// PERIPH space definition (through AHB to APB bridge 0)
#define PERIPH0_OFFSET_ADDRESS      0x00000000
#define PERIPH0_START_ADDRESS       (PERIPH_START_ADDRESS + PERIPH0_OFFSET_ADDRESS)

// PERIPH space definition (through AHB to APB bridge 1)
#define PERIPH1_OFFSET_ADDRESS      0x00004000
#define PERIPH1_START_ADDRESS       (PERIPH_START_ADDRESS + PERIPH1_OFFSET_ADDRESS)

// PERIPH space definition (through AHB to APB bridge 2)
#define PERIPH2_OFFSET_ADDRESS      0x00008000
#define PERIPH2_START_ADDRESS       (PERIPH_START_ADDRESS + PERIPH2_OFFSET_ADDRESS)

// PERIPH space definition (through AHB to APB bridge 3)
#define PERIPH3_OFFSET_ADDRESS      0x0000C000
#define PERIPH3_START_ADDRESS       (PERIPH_START_ADDRESS + PERIPH3_OFFSET_ADDRESS)

// PERIPH space definition (through AHB to RIU bridge)
#define RIUBRIDGE0_OFFSET_ADDRESS   0x00000000
#define RIUBRIDGE0_START_ADDRESS    (RIUBRIDGE_START_ADDRESS + RIUBRIDGE0_OFFSET_ADDRESS)


//=======================================================================
// Definition of the sub addresses
// for periph directly connected to AHB.
//=======================================================================

// DMA config space definition (directly on AHB)
#define DMAC_CFG_OFFSET_ADDRESS    0x00010000
#define DMAC_CFG_START_ADDRESS     (PERIPH_START_ADDRESS + DMAC_CFG_OFFSET_ADDRESS)

// ETB config space definition (directly on AHB)
#define ETB_CFG_OFFSET_ADDRESS    0x00014000
#define ETB_CFG_START_ADDRESS     (PERIPH_START_ADDRESS + ETB_CFG_OFFSET_ADDRESS)

// USB config space definition (directly on AHB)
#define USB_CFG_OFFSET_ADDRESS    0x00018000
#define USB_CFG_START_ADDRESS     (PERIPH_START_ADDRESS + USB_CFG_OFFSET_ADDRESS)

// SDMMC config and data space definition (directly on AHB)
#define SDMMC_CFG_OFFSET_ADDRESS  0x0001C000
#define SDMMC_CFG_START_ADDRESS   (PERIPH_START_ADDRESS + SDMMC_CFG_OFFSET_ADDRESS)

// SDRAM config space definition (directly on AHB)
#define SDRAM_CFG_OFFSET_ADDRESS  0x00000000
#define SDRAM_CFG_START_ADDRESS   (PORT0_CFG_START_ADDRESS + SDRAM_CFG_OFFSET_ADDRESS)



//=======================================================================
// Definition of the sub addresses
// for shiface dpram and rfdl.
//=======================================================================
#define RF_DL_OFFSET_ADDRESS           0x00000000
#define RF_DL_START_ADDRESS            (RFDL_SHIFACE_DPRAM_START_ADDRESS + RF_DL_OFFSET_ADDRESS)

#define SHIFACE_DPRAM_OFFSET_ADDRESS   0x00001000
#define SHIFACE_DPRAM_START_ADDRESS    (RFDL_SHIFACE_DPRAM_START_ADDRESS + SHIFACE_DPRAM_OFFSET_ADDRESS)

//=======================================================================
// Definition of the sub addresses
// for the peripherals.
//=======================================================================

// define DUMMY address for periph which do not
// have a mapping yet.
#define TO_BE_DEFINED               0xFFFFFFFF



// this values are repeated to be parsed by script
#define COMMON_BASE_ADD             COMMON_START_ADDRESS
#define IROM_BASE_ADD               IROM_START_ADDRESS
#define IRAM_BASE_ADD               IRAM_START_ADDRESS
#define IRAM_DSP_BASE_ADD           IRAM_DSP_START_ADDRESS
#define MEM0_BASE_ADD               MEM0_START_ADDRESS
#define MEM1_BASE_ADD               MEM1_START_ADDRESS
#define MEM2_BASE_ADD               MEM2_START_ADDRESS
#define MEM3_BASE_ADD               MEM3_START_ADDRESS
//#define USR0_BASE_ADD               USR0_START_ADDRESS
//#define USR1_BASE_ADD               USR1_START_ADDRESS
//#define USR2_BASE_ADD               USR2_START_ADDRESS
#define ETBRAM_BASE_ADD             ETBRAM_START_ADDRESS
#define ITCMRAM_BASE_ADD            ITCMRAM_START_ADDRESS
#define DTCMRAM_BASE_ADD            DTCMRAM_START_ADDRESS
#define GSS_BASE_ADD                GSS_START_ADDRESS
#define SDRAM_BASE_ADD              SDRAM_START_ADDRESS
#define NAND_FLASH_DPRAM_BASE_ADD   NAND_FLASH_DPRAM_START_ADDRESS

#define DMAC_CFG_BASE_ADD           DMAC_CFG_START_ADDRESS
#define ETB_CFG_BASE_ADD            ETB_CFG_START_ADDRESS
#define USB_CFG_BASE_ADD            USB_CFG_START_ADDRESS
#define SDMMC_CFG_BASE_ADD          SDMMC_CFG_START_ADDRESS
#define SDRAM_CFG_BASE_ADD          SDRAM_CFG_START_ADDRESS
#define RF_DL_BASE_ADD              RF_DL_START_ADDRESS
#define SHIFACE_DPRAM_BASE_ADD      SHIFACE_DPRAM_START_ADDRESS


//=======================================================================
//=======================================================================
//====================== PERIPH 0 peripherals ===========================
//=======================================================================
//=======================================================================

//-----------------------------------------------------------------------
// CLKGEN
//-----------------------------------------------------------------------
#define WHAT_CMU_OFFSET_BASE_ADD    0x00000000
#define WHAT_CMU_BASE_ADD           (PERIPH0_START_ADDRESS + WHAT_CMU_OFFSET_BASE_ADD)

//-----------------------------------------------------------------------
// SYSREG
//-----------------------------------------------------------------------
#define SYS_APB_REG_OFFSET_BASE_ADD    0x00000400
#define SYS_APB_REG_BASE_ADD           ( PERIPH0_START_ADDRESS + SYS_APB_REG_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// KEYPAD
//-----------------------------------------------------------------------
#define KEYBOARD_OFFSET_BASE_ADD       0x00000800
#define KEYBOARD_BASE_ADD              ( PERIPH0_START_ADDRESS + KEYBOARD_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// PWM
//-----------------------------------------------------------------------
#define PWM_OFFSET_BASE_ADD            0x00000C00
#define PWM_BASE_ADD                   ( PERIPH0_START_ADDRESS + PWM_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// GPIO
//-----------------------------------------------------------------------
#define GPIO_OFFSET_BASE_ADD           0x00001000
#define GPIO_BASE_ADD                  ( PERIPH0_START_ADDRESS + GPIO_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// SEMEPHORE
//-----------------------------------------------------------------------
#define SEMAPHORE_OFFSET_BASE_ADD      0x00001700
#define SEMAPHORE_BASE_ADD             ( PERIPH0_START_ADDRESS + SEMAPHORE_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// INT controler
//-----------------------------------------------------------------------
#define IT_CTRL_OFFSET_BASE_ADD        0x00001800
#define IT_CTRL_BASE_ADD               ( PERIPH0_START_ADDRESS + IT_CTRL_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// external IT
//-----------------------------------------------------------------------
#define EXT_INT_OFFSET_BASE_ADD        0x00001C00
#define EXT_INT_BASE_ADD               ( PERIPH0_START_ADDRESS + EXT_INT_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// EXICE
//-----------------------------------------------------------------------
#define EX_ICE_OFFSET_BASE_ADD         0x00002000
#define EX_ICE_BASE_ADD                ( PERIPH0_START_ADDRESS + EX_ICE_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// EMI_CONFIG
//-----------------------------------------------------------------------
#define EMI_OFFSET_BASE_ADD            0x00002400
#define EMI_BASE_ADD                   (PERIPH0_START_ADDRESS + EMI_OFFSET_BASE_ADD)

//-----------------------------------------------------------------------
// DEBUG_MODULE
//-----------------------------------------------------------------------
#define DSP_DEBUG_MODULE_OFFSET_BASE_ADD   0x00002800
#define DSP_DEBUG_MODULE_BASE_ADD          (PERIPH0_START_ADDRESS + DSP_DEBUG_MODULE_OFFSET_BASE_ADD)

//-----------------------------------------------------------------------
// SYS_TIMER
//-----------------------------------------------------------------------
#define SYS_TIMER_OFFSET_BASE_ADD      0x00002C00
#define SYS_TIMER_BASE_ADD             (PERIPH0_START_ADDRESS + SYS_TIMER_OFFSET_BASE_ADD)

//-----------------------------------------------------------------------
// EMI_SC (Static Memory Controler) CONFIG
//-----------------------------------------------------------------------
#define EMI_SC_OFFSET_BASE_ADD     0x00002C00
#define EMI_SC_BASE_ADD            (RIUBRIDGE_START_ADDRESS + EMI_SC_OFFSET_BASE_ADD)

//-----------------------------------------------------------------------
// EMI_SMC (Static Memory Controler) CONFIG
//-----------------------------------------------------------------------
#define EMI_SMC_OFFSET_BASE_ADD    0x00002800
#define EMI_SMC_BASE_ADD           (RIUBRIDGE_START_ADDRESS + EMI_SMC_OFFSET_BASE_ADD)

//-----------------------------------------------------------------------
// EMI_SMC (Dynamic Memory Controler) CONFIG
//-----------------------------------------------------------------------
#define EMI_DMC_OFFSET_BASE_ADD    0x00002400
#define EMI_DMC_BASE_ADD           (RIUBRIDGE_START_ADDRESS + EMI_DMC_OFFSET_BASE_ADD)

//-----------------------------------------------------------------------
// EMI_MAIN CONFIG
//-----------------------------------------------------------------------
#define EMI_MAIN_OFFSET_BASE_ADD    0x00002000
#define EMI_MAIN_BASE_ADD           (RIUBRIDGE_START_ADDRESS + EMI_MAIN_OFFSET_BASE_ADD)

//-----------------------------------------------------------------------
// RF LOG
//-----------------------------------------------------------------------
#define RF_LOG_OFFSET_BASE_ADD		0x00003A00
#define RF_LOG_BASE_ADD				( PERIPH0_START_ADDRESS + RF_LOG_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// Paging SPI
//-----------------------------------------------------------------------
#define PSPI_OFFSET_BASE_ADD		0x00003E00
#define PSPI_BASE_ADD				( PERIPH0_START_ADDRESS + PSPI_OFFSET_BASE_ADD )

//=======================================================================
//=======================================================================
//====================== PERIPH 1 peripherals ===========================
//=======================================================================
//=======================================================================

//-----------------------------------------------------------------------
// I2C interface
//-----------------------------------------------------------------------
#define I2C_OFFSET_BASE_ADD         	0x00000000
#define I2C_BASE_ADD                	( PERIPH1_START_ADDRESS + I2C_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// SPI1
//-----------------------------------------------------------------------

#define FS_SPI1_OFFSET_BASE_ADD         0x00000400
#define FS_SPI1_BASE_ADD	            ( PERIPH1_START_ADDRESS + FS_SPI1_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// FS_SPI2
//-----------------------------------------------------------------------
#if 0
#define FS_SPI2_OFFSET_BASE_ADD         0x00000800
#define FS_SPI2_BASE_ADD	            ( PERIPH1_START_ADDRESS + FS_SPI2_OFFSET_BASE_ADD )
#endif
//-----------------------------------------------------------------------
// G3D
//-----------------------------------------------------------------------
#define G3D_OFFSET_BASE_ADD				0x00000800
#define G3D_BASE_ADD					( PERIPH1_START_ADDRESS + G3D_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// UART1
//-----------------------------------------------------------------------
#define UART1_OFFSET_BASE_ADD          0x00001000
#define UART1_BASE_ADD                 ( PERIPH1_START_ADDRESS + UART1_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// UART2
//-----------------------------------------------------------------------
#define UART2_OFFSET_BASE_ADD          0x00001400
#define UART2_BASE_ADD                 ( PERIPH1_START_ADDRESS + UART2_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// IRDA1  register  definitions
//-----------------------------------------------------------------------
#if 0
#define IRDA1_OFFSET_BASE_ADD          0x00001800
#define IRDA1_BASE_ADD                 ( PERIPH1_START_ADDRESS + IRDA1_OFFSET_BASE_ADD )
#endif
//-----------------------------------------------------------------------
// DISPLAY
//-----------------------------------------------------------------------
#define DISPLAY_OFFSET_BASE_ADD          0x00001800
#define DISPLAY_BASE_ADD                 ( PERIPH1_START_ADDRESS + DISPLAY_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// IRDA2  register  definitions
//-----------------------------------------------------------------------
#define IRDA2_OFFSET_BASE_ADD          0x00001C00
#define IRDA2_BASE_ADD                 ( PERIPH1_START_ADDRESS + IRDA2_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// SIM 1
//-----------------------------------------------------------------------
#define SIM1_OFFSET_BASE_ADD           0x00002000
#define SIM1_BASE_ADD                  ( PERIPH1_START_ADDRESS + SIM1_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// SIM 2
//-----------------------------------------------------------------------
#define SIM2_OFFSET_BASE_ADD           0x00002400
#define SIM2_BASE_ADD                  ( PERIPH1_START_ADDRESS + SIM2_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// NOT_USED
//-----------------------------------------------------------------------
#define NOT_USED_OFFSET_BASE_ADD       0x00002800
#define NOT_USED_BASE_ADD              ( PERIPH1_START_ADDRESS + NOT_USED_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// GEA coprocessor
//-----------------------------------------------------------------------
#define GEA_OFFSET_BASE_ADD            0x00002C00
#define GEA_BASE_ADD                   ( PERIPH1_START_ADDRESS + GEA_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// FCS coprocessor
//-----------------------------------------------------------------------
#define FCS_OFFSET_BASE_ADD            0x00003000
#define FCS_BASE_ADD                   ( PERIPH1_START_ADDRESS + FCS_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// GEA2 coprocessor
//-----------------------------------------------------------------------
#define GEA2_OFFSET_BASE_ADD           0x00003400
#define GEA2_BASE_ADD                  ( PERIPH1_START_ADDRESS + GEA2_OFFSET_BASE_ADD )

//=======================================================================
//=======================================================================
//====================== PERIPH 2 peripherals ===========================
//=======================================================================
//=======================================================================

//-----------------------------------------------------------------------
// DSMA
//-----------------------------------------------------------------------
#define DSMA_CPU_OFFSET_BASE_ADD      0x00000000
#define DSMA_CPU_BASE_ADD             ( PERIPH2_START_ADDRESS + DSMA_CPU_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// Audio
//-----------------------------------------------------------------------
#define AUDIO_REG_OFFSET_BASE_ADD      0x00000400
#define AUDIO_REG_BASE_ADD             ( PERIPH2_START_ADDRESS + AUDIO_REG_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// DSP timer
//-----------------------------------------------------------------------
#define DSP_TIMER_OFFSET_BASE_ADD      0x00000800
#define DSP_TIMER_BASE_ADD             ( PERIPH2_START_ADDRESS + DSP_TIMER_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// FRAME COUNTER
//-----------------------------------------------------------------------
#define FRAMECNT_CPU_OFFSET_BASE_ADD   0x00000C00
#define FRAMECNT_CPU_BASE_ADD          ( PERIPH2_START_ADDRESS + FRAMECNT_CPU_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// Shared interface
//-----------------------------------------------------------------------
#define SHIFACE_CPU_OFFSET_BASE_ADD    0x00001000
#define SHIFACE_CPU_BASE_ADD           ( PERIPH2_START_ADDRESS + SHIFACE_CPU_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// DSP INT controler
//-----------------------------------------------------------------------
#define DSP_ITCTRL_OFFSET_BASE_ADD     0x00001400
#define DSP_ITCTRL_BASE_ADD            ( PERIPH2_START_ADDRESS + DSP_ITCTRL_OFFSET_BASE_ADD )
#define TEAKITCTRL_BASE_ADD            DSP_ITCTRL_BASE_ADD


//-----------------------------------------------------------------------
// I2S
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// AGC
//-----------------------------------------------------------------------
#define AGC_OFFSET_BASE_ADD            0x00001800
#define AGC_BASE_ADD                   ( PERIPH2_START_ADDRESS + AGC_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// RFDL
//-----------------------------------------------------------------------
#define RFDOWNLOADER_OFFSET_BASE_ADD   0x00001C00
//#define RFDOWNLOADER_BASE_ADD          ( PERIPH2_START_ADDRESS + RFDOWNLOADER_OFFSET_BASE_ADD )

#define RFDOWNLOADER_BASE_ADD 0x34009C00



//-----------------------------------------------------------------------
// RXFIR
//-----------------------------------------------------------------------
#define RXFIR_OFFSET_BASE_ADD          0x00002000
#define RXFIR_BASE_ADD                 ( PERIPH2_START_ADDRESS + RXFIR_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// TXPORT
//-----------------------------------------------------------------------
#define TXPORT_OFFSET_BASE_ADD         0x00002400
#define TXPORT_BASE_ADD                ( PERIPH2_START_ADDRESS + TXPORT_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// DIGRF
//-----------------------------------------------------------------------
#define RXDIGRF_OFFSET_BASE_ADD        0x00002800
#define RXDIGRF_BASE_ADD               ( PERIPH2_START_ADDRESS + RXDIGRF_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// TBU
//-----------------------------------------------------------------------
#define TBU_OFFSET_BASE_ADD            0x00002C00
#define TBU_BASE_ADD                   ( PERIPH2_START_ADDRESS + TBU_OFFSET_BASE_ADD )


//=======================================================================
//=======================================================================
//====================== PERIPH 3 peripherals ===========================
//=======================================================================
//=======================================================================

//-----------------------------------------------------------------------
// ABB (Analog BaseBand)
//-----------------------------------------------------------------------
#define ABB_OFFSET_BASE_ADD      0x00000000
#define ABB_BASE_ADD             ( PERIPH3_START_ADDRESS + ABB_OFFSET_BASE_ADD )

#define ABB2_OFFSET_BASE_ADD      0x00000400
#define ABB2_BASE_ADD             ( PERIPH3_START_ADDRESS + ABB2_OFFSET_BASE_ADD )

//=======================================================================
//=======================================================================
//====================== RIU bridge 0 peripherals ===========================
//=======================================================================
//=======================================================================

//-----------------------------------------------------------------------
// DMA2D
//-----------------------------------------------------------------------

#define DMA2D_OFFSET_BASE_ADD       0x00000400
#define DMA2D_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + DMA2D_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// ISP
//-----------------------------------------------------------------------

#define ISP_OFFSET_BASE_ADD         0x00000600
#define ISP_BASE_ADD                ( RIUBRIDGE0_START_ADDRESS + ISP_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// ICP0
//-----------------------------------------------------------------------

#define ICP0_OFFSET_BASE_ADD         0x00000800
#define ICP0_BASE_ADD                ( RIUBRIDGE0_START_ADDRESS + ICP0_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// SCAL / MACE
//-----------------------------------------------------------------------

#define SCAL0_OFFSET_BASE_ADD       0x00000A00
#define SCAL0_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + SCAL0_OFFSET_BASE_ADD )

#define SCAL1_OFFSET_BASE_ADD       0x00000C00
#define SCAL1_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + SCAL1_OFFSET_BASE_ADD )

#define VIP0_OFFSET_BASE_ADD        0x00000E00
#define VIP0_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + VIP0_OFFSET_BASE_ADD )

#define VIP1_OFFSET_BASE_ADD        0x00001000
#define VIP1_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + VIP1_OFFSET_BASE_ADD )

#define VIP2_OFFSET_BASE_ADD        0x00001200
#define VIP2_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + VIP2_OFFSET_BASE_ADD )

#define SCAL_DMA_OFFSET_BASE_ADD    0x00001400
#define SCAL_DMA_BASE_ADD           ( RIUBRIDGE0_START_ADDRESS + SCAL_DMA_OFFSET_BASE_ADD )

#define DS_OFFSET_BASE_ADD          0x00001600
#define DS_BASE_ADD                 ( RIUBRIDGE0_START_ADDRESS + DS_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// JPD
//-----------------------------------------------------------------------

#define JPD_OFFSET_BASE_ADD         0x00001800
#define JPD_BASE_ADD                ( RIUBRIDGE0_START_ADDRESS + JPD_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// JPE
//-----------------------------------------------------------------------

#define JPE_OFFSET_BASE_ADD         0x00001A00
#define JPE_BASE_ADD                ( RIUBRIDGE0_START_ADDRESS + JPE_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// MMP
//-----------------------------------------------------------------------

#define MMP_OFFSET_BASE_ADD         0x00001C00
#define MMP_BASE_ADD                ( RIUBRIDGE0_START_ADDRESS + MMP_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// PAGING SPI
//-----------------------------------------------------------------------

#define PAGING_SPI_OFFSET_BASE_ADD  0x00005000
#define PAGING_SPI_BASE_ADD         ( RIUBRIDGE0_START_ADDRESS + PAGING_SPI_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// FSP SPI
//-----------------------------------------------------------------------

#define FSP_SPI_OFFSET_BASE_ADD     0x00005380
#define FSP_SPI_BASE_ADD            ( RIUBRIDGE0_START_ADDRESS + FSP_SPI_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// FSP2 SPI
//-----------------------------------------------------------------------

#define FSP2_SPI_OFFSET_BASE_ADD     0x00020700
#define FSP2_SPI_BASE_ADD            ( RIUBRIDGE0_START_ADDRESS + FSP2_SPI_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// MASTER SPI
//-----------------------------------------------------------------------

#define MASTER_SPI_OFFSET_BASE_ADD  0x00005500
#define MASTER_SPI_BASE_ADD         ( RIUBRIDGE0_START_ADDRESS + MASTER_SPI_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// GDMA
//-----------------------------------------------------------------------

#define GDMA_OFFSET_BASE_ADD        0x00005400
#define GDMA_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + GDMA_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// BDMA
//-----------------------------------------------------------------------

#define BDMA_OFFSET_BASE_ADD        0x00005600
#define BDMA_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + BDMA_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// QSPI
//-----------------------------------------------------------------------

#define QSPI_OFFSET_BASE_ADD        0x00006D00
#define QSPI_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + QSPI_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// I2C
//-----------------------------------------------------------------------

#define I2C_MAS_OFFSET_BASE_ADD     0x00007A00
#define I2C_MAS_BASE_ADD            ( RIUBRIDGE0_START_ADDRESS + I2C_MAS_OFFSET_BASE_ADD )


//-----------------------------------------------------------------------
// GARB (MIU group arbiter)
//-----------------------------------------------------------------------

#define GARB_OFFSET_BASE_ADD        0x00001E00
#define GARB_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + GARB_OFFSET_BASE_ADD )

#define GARB0_OFFSET_BASE_ADD       0x00000000
#define GARB0_BASE_ADD              ( GARB_BASE_ADD + GARB0_OFFSET_BASE_ADD )

#define GARB1_OFFSET_BASE_ADD       0x00000040
#define GARB1_BASE_ADD              ( GARB_BASE_ADD + GARB1_OFFSET_BASE_ADD )

#define GARB_IMI2_OFFSET_BASE_ADD   0x00000080
#define GARB_IMI2_BASE_ADD          ( GARB_BASE_ADD + GARB_IMI2_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// EMI
//-----------------------------------------------------------------------

#define EMI0_OFFSET_BASE_ADD        0x00002000
#define EMI0_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + EMI0_OFFSET_BASE_ADD )

#define EMI1_OFFSET_BASE_ADD        0x00002200
#define EMI1_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + EMI1_OFFSET_BASE_ADD )

#define DMC0_OFFSET_BASE_ADD        0x00002400
#define DMC0_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + DMC0_OFFSET_BASE_ADD )

#define DMC1_OFFSET_BASE_ADD        0x00002600
#define DMC1_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + DMC1_OFFSET_BASE_ADD )

#define SMC0_OFFSET_BASE_ADD        0x00002800
#define SMC0_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + SMC0_OFFSET_BASE_ADD )

#define SMC1_OFFSET_BASE_ADD        0x00002A00
#define SMC1_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + SMC1_OFFSET_BASE_ADD )

#define EMI_SC0_OFFSET_BASE_ADD     0x00002C00
#define EMI_SC0_BASE_ADD            ( RIUBRIDGE0_START_ADDRESS + EMI_SC0_OFFSET_BASE_ADD )

#define EMI_SC1_OFFSET_BASE_ADD     0x00002E00
#define EMI_SC1_BASE_ADD            ( RIUBRIDGE0_START_ADDRESS + EMI_SC1_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// DISP
//-----------------------------------------------------------------------

#define DISP0_OFFSET_BASE_ADD       0x00003000
#define DISP0_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + DISP0_OFFSET_BASE_ADD )

#define DISP1_OFFSET_BASE_ADD       0x00003200
#define DISP1_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + DISP1_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// BACH
//-----------------------------------------------------------------------

#define BACH0_OFFSET_BASE_ADD       0x00003400
#define BACH0_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + BACH0_OFFSET_BASE_ADD )

#define BACH1_OFFSET_BASE_ADD       0x00003600
#define BACH1_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + BACH1_OFFSET_BASE_ADD )

#define BACH2_OFFSET_BASE_ADD       0x00003800
#define BACH2_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + BACH2_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// MPIF
//-----------------------------------------------------------------------

#define MPIF_OFFSET_BASE_ADD        0x00003A00
#define MPIF_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + MPIF_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// EFUSE
//-----------------------------------------------------------------------

#define EFUSE_OFFSET_BASE_ADD       0x00003C00
#define EFUSE_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + EFUSE_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// USB / UTMI
//-----------------------------------------------------------------------

// Please modify addr 0x00003C00, it is used by efuse in bravo.
#define USBC_OFFSET_BASE_ADD        0x00003C00
#define USBC_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + USBC_OFFSET_BASE_ADD )

#define UTMI_OFFSET_BASE_ADD        0x00003E00
#define UTMI_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + UTMI_OFFSET_BASE_ADD )

#define USB0_OFFSET_BASE_ADD        0x00004000
#define USB0_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + USB0_OFFSET_BASE_ADD )

#define USB1_OFFSET_BASE_ADD        0x00004200
#define USB1_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + USB1_OFFSET_BASE_ADD )

#define USB2_OFFSET_BASE_ADD        0x00004400
#define USB2_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + USB2_OFFSET_BASE_ADD )

#define USB3_OFFSET_BASE_ADD        0x00004600
#define USB3_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + USB3_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// FMRX
//-----------------------------------------------------------------------

#define FMRX0_OFFSET_BASE_ADD       0x00004800
#define FMRX0_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + FMRX0_OFFSET_BASE_ADD )

#define FMRX1_OFFSET_BASE_ADD       0x00004A00
#define FMRX1_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + FMRX1_OFFSET_BASE_ADD )

#define FMRX2_OFFSET_BASE_ADD       0x00004C00
#define FMRX2_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + FMRX2_OFFSET_BASE_ADD )

#define FMRX3_OFFSET_BASE_ADD       0x00004E00
#define FMRX3_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + FMRX3_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// FCIE
//-----------------------------------------------------------------------

#define FCIE0_OFFSET_BASE_ADD       0x00005000
#define FCIE0_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + FCIE0_OFFSET_BASE_ADD )

#define FCIE1_OFFSET_BASE_ADD       0x00005200
#define FCIE1_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + FCIE1_OFFSET_BASE_ADD )

#define FCIE2_OFFSET_BASE_ADD       0x00005400
#define FCIE2_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + FCIE2_OFFSET_BASE_ADD )

#define FCIE3_OFFSET_BASE_ADD       0x00005600
#define FCIE3_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + FCIE3_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// SDIO
//-----------------------------------------------------------------------

#define SDIO0_OFFSET_BASE_ADD       0x00005800
#define SDIO0_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + SDIO0_OFFSET_BASE_ADD )

#define SDIO1_OFFSET_BASE_ADD       0x00005A00
#define SDIO1_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + SDIO1_OFFSET_BASE_ADD )

#define SDIO2_OFFSET_BASE_ADD       0x00005C00
#define SDIO2_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + SDIO2_OFFSET_BASE_ADD )

#define SDIO3_OFFSET_BASE_ADD       0x00005E00
#define SDIO3_BASE_ADD              ( RIUBRIDGE0_START_ADDRESS + SDIO3_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// BT
//-----------------------------------------------------------------------

#define BT_DMA_OFFSET_BASE_ADD      0x00006000
#define BT_DMA_BASE_ADD             ( RIUBRIDGE0_START_ADDRESS + BT_DMA_OFFSET_BASE_ADD )

#define BT_SRC_OFFSET_BASE_ADD      0x00006200
#define BT_SRC_BASE_ADD             ( RIUBRIDGE0_START_ADDRESS + BT_SRC_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// PLL
//-----------------------------------------------------------------------

#define PLL_OFFSET_BASE_ADD         0x00006400
#define PLL_BASE_ADD                ( RIUBRIDGE0_START_ADDRESS + PLL_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// PAD Ctrl
//-----------------------------------------------------------------------

#define PAD_CTRL_OFFSET_BASE_ADD    0x00006600
#define PAD_CTRL_BASE_ADD           ( RIUBRIDGE0_START_ADDRESS + PAD_CTRL_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// ADC DMA
//-----------------------------------------------------------------------

#define ADC_DMA_OFFSET_BASE_ADD     0x00006800
#define ADC_DMA_BASE_ADD            ( RIUBRIDGE0_START_ADDRESS + ADC_DMA_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// PATGEN
//-----------------------------------------------------------------------

#define PATGEN_OFFSET_BASE_ADD      0x00006A00
#define PATGEN_BASE_ADD             ( RIUBRIDGE0_START_ADDRESS + PATGEN_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// SPI
//-----------------------------------------------------------------------

#define SPI_OFFSET_BASE_ADD         0x00006C00
#define SPI_BASE_ADD                ( RIUBRIDGE0_START_ADDRESS + SPI_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// RTC
//-----------------------------------------------------------------------

#define RTC_OFFSET_BASE_ADD         0x00003A00
#define RTC_BASE_ADD                ( RIUBRIDGE0_START_ADDRESS + RTC_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// PMU
//-----------------------------------------------------------------------

#define PMU_OFFSET_BASE_ADD         0x00006E00
#define PMU_BASE_ADD                ( RIUBRIDGE0_START_ADDRESS + PMU_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// IMI
//-----------------------------------------------------------------------

#define IMI0_OFFSET_BASE_ADD        0x00007000
#define IMI0_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + IMI0_OFFSET_BASE_ADD )

#define IMI1_OFFSET_BASE_ADD        0x00007100
#define IMI1_BASE_ADD               ( RIUBRIDGE0_START_ADDRESS + IMI1_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// GPIO Ctrl
//-----------------------------------------------------------------------

#define GPIO_CTRL0_OFFSET_BASE_ADD  0x00007400
#define GPIO_CTRL0_BASE_ADD         ( RIUBRIDGE0_START_ADDRESS + GPIO_CTRL0_OFFSET_BASE_ADD )

#define GPIO_CTRL1_OFFSET_BASE_ADD  0x00007600
#define GPIO_CTRL1_BASE_ADD         ( RIUBRIDGE0_START_ADDRESS + GPIO_CTRL1_OFFSET_BASE_ADD )

#define GPIO_CTRL2_OFFSET_BASE_ADD  0x00007800
#define GPIO_CTRL2_BASE_ADD         ( RIUBRIDGE0_START_ADDRESS + GPIO_CTRL2_OFFSET_BASE_ADD )

//-----------------------------------------------------------------------
// BBTOP
//-----------------------------------------------------------------------

#define BBTOP0_OFFSET_BASE_ADD      0x00007C00
#define BBTOP0_BASE_ADD             ( RIUBRIDGE0_START_ADDRESS + BBTOP0_OFFSET_BASE_ADD )

#define BBTOP1_OFFSET_BASE_ADD      0x00007E00
#define BBTOP1_BASE_ADD             ( RIUBRIDGE0_START_ADDRESS + BBTOP1_OFFSET_BASE_ADD )

//=======================================================================
//====================== Internal Memory IRAM ==================================
//=======================================================================

// IRAM on CPU side
#define IRAM_BANK_LENGTH 	0x2000    // the size of one IRAM bank is 8kb
#define IRAM_BANK1_ADD    IRAM_BASE_ADD
#define IRAM_BANK2_ADD    IRAM_BANK1_ADD + IRAM_BANK_LENGTH
#define IRAM_BANK3_ADD    IRAM_BANK2_ADD + IRAM_BANK_LENGTH
#define IRAM_BANK4_ADD    IRAM_BANK3_ADD + IRAM_BANK_LENGTH
#define IRAM_BANK5_ADD    IRAM_BANK4_ADD + IRAM_BANK_LENGTH
#define IRAM_BANK6_ADD    IRAM_BANK5_ADD + IRAM_BANK_LENGTH
#define IRAM_BANK7_ADD    IRAM_BANK6_ADD + IRAM_BANK_LENGTH


// IRAM on DSP side
#define IRAM_DSP_BANK_LENGTH 0x4000
#define IRAM_DSP_BANK5_ADD IRAM_DSP_BASE_ADD
#define IRAM_DSP_BANK6_ADD IRAM_DSP_BANK5_ADD + IRAM_DSP_BANK_LENGTH

#define BA_CMU      WHAT_CMU_BASE_ADD
#define BA_SYSREG   SYS_APB_REG_BASE_ADD
#define BA_GPIO     GPIO_BASE_ADD
#define BA_IT_CTRL  IT_CTRL_BASE_ADD
#define BA_EXT_INT  EXT_INT_BASE_ADD
#define BA_UART1    UART1_BASE_ADD
#define BA_UART2    UART2_BASE_ADD
#define BA_INT_RAM  IRAM_START_ADDRESS
#define BA_TIMER    SYS_TIMER_BASE_ADD
#define BA_SPI1     FS_SPI1_BASE_ADD


//======================================================================
//====================== MIU address domain ============================
//======================================================================

#define MIU_BASE_PHYADDR    MEM0_START_ADDRESS
#define MIU_ADDR_MASK       0x1FFFFFFF
#define MIU_MAX_PHYADDR     (MIU_BASE_PHYADDR + MIU_ADDR_MASK)
#define MIU_BANK_SIZE       0x04000000

//======================================================================
//========================  DPM & MMAP address =========================
//======================================================================

#if !defined (__MMAP__)
#define MMAP_ADDR_LEN                       0x0
#else //!__MMAP__
#if defined (__MMAP_CUS_16MB__)
#define MMAP_ADDR_LEN                       0x1000000       //16MB
#elif defined (__MMAP_CUS_32MB__)
#define MMAP_ADDR_LEN                       0x2000000       //32MB
#elif defined (__MMAP_CUS_64MB__)
#define MMAP_ADDR_LEN                       0x4000000       //64MB
#else //__MMAP_CUS_16MB__
#define MMAP_ADDR_LEN                       0x1000000       //16MB
#endif //__MMAP_CUS_16MB__
#endif //!__MMAP__

/* DPM address length */
#if defined (__EXTRACT_BOOT_IMAGE__) || defined (__COMPRESS_SELECTED_BIN__)
#define EMMI_SLDPM_TEXT_ADDR_LEN            0x300000        //3MB
#else
#define EMMI_SLDPM_TEXT_ADDR_LEN            0x800000        //8MB
#endif
#define EMMI_SLDPM_DATA_ADDR_LEN            0x800000        //8MB
#define EXAMPLE_TEXT_ADDR_LEN               0x100000        //1MB
#define EXAMPLE_DATA_ADDR_LEN               0x100000        //1MB
#define DPM_CUS_ADDR_LEN                    0x100000        //1MB
#define DPM_ADDR_LEN                        (EMMI_SLDPM_TEXT_ADDR_LEN + EMMI_SLDPM_DATA_ADDR_LEN)


/* Start of demand paging address */
#if defined (__EXTRACT_BOOT_IMAGE__)
#define XRAM_START_ADDR                     0x10000000
#define DPM_START_ADDR                      (XRAM_START_ADDR - DPM_ADDR_LEN)
#elif defined (Z)
#if defined (__NAND__)
#define SELECTED_BIN_START_ADDR             0x8c000000
#else
#define SELECTED_BIN_START_ADDR             0x98000000
#endif
#define DPM_START_ADDR                      (SELECTED_BIN_START_ADDR - DPM_ADDR_LEN)
#else
#define DPM_START_ADDR                      0xD8000000
#endif

/* End of demand paging address */
#define DPM_END_ADDR                        DPM_START_ADDR + DPM_ADDR_LEN - 1



#if defined (__EXTRACT_BOOT_IMAGE__) || defined (__COMPRESS_SELECTED_BIN__)
#define EMMI_SLDPM_DATA_START_ADDR          DPM_START_ADDR
#define EMMI_SLDPM_DATA_END_ADDR            EMMI_SLDPM_DATA_START_ADDR + EMMI_SLDPM_DATA_ADDR_LEN - 1
#define EMMI_SLDPM_TEXT_START_ADDR          EMMI_SLDPM_DATA_START_ADDR + EMMI_SLDPM_DATA_ADDR_LEN
#define EMMI_SLDPM_TEXT_END_ADDR            EMMI_SLDPM_TEXT_START_ADDR + EMMI_SLDPM_TEXT_ADDR_LEN - 1
#else
#define EMMI_SLDPM_TEXT_START_ADDR          DPM_START_ADDR
#define EMMI_SLDPM_TEXT_END_ADDR            EMMI_SLDPM_TEXT_START_ADDR + EMMI_SLDPM_TEXT_ADDR_LEN - 1
#define EMMI_SLDPM_DATA_START_ADDR          EMMI_SLDPM_TEXT_START_ADDR + EMMI_SLDPM_TEXT_ADDR_LEN
#define EMMI_SLDPM_DATA_END_ADDR            EMMI_SLDPM_DATA_START_ADDR + EMMI_SLDPM_DATA_ADDR_LEN - 1
#endif

#define EXAMPLE_TEXT_START_ADDR             EMMI_SLDPM_DATA_END_ADDR + 1
#define EXAMPLE_TEXT_END_ADDR               EXAMPLE_TEXT_START_ADDR + EXAMPLE_TEXT_ADDR_LEN - 1
#define EXAMPLE_DATA_START_ADDR             EXAMPLE_TEXT_START_ADDR + EXAMPLE_TEXT_ADDR_LEN
#define EXAMPLE_DATA_END_ADDR               EXAMPLE_DATA_START_ADDR + EXAMPLE_DATA_ADDR_LEN - 1

#define CUS_IME_DATA_START_ADDR             EMMI_SLDPM_DATA_START_ADDR + 0x400000
#define CUS_IME_DATA_END_ADDR               EMMI_SLDPM_DATA_START_ADDR + 0x400000 + DPM_CUS_ADDR_LEN - 1
#define CUS_PLMN_DATA_START_ADDR            EMMI_SLDPM_DATA_START_ADDR + 0x500000
#define CUS_PLMN_DATA_END_ADDR              EMMI_SLDPM_DATA_START_ADDR + 0x500000 + DPM_CUS_ADDR_LEN - 1
#define CUS_HANDWRITING_DATA_START_ADDR     EMMI_SLDPM_DATA_START_ADDR + 0x600000
#define CUS_HANDWRITING_DATA_END_ADDR       EMMI_SLDPM_DATA_START_ADDR + 0x600000 + DPM_CUS_ADDR_LEN - 1

#define MMAP_CUS_ADDR_LEN                   MMAP_ADDR_LEN
#define MMAP_START_ADDRESS                  0xD0000000
#define MMAP_END_ADDRESS                    MMAP_START_ADDRESS + MMAP_ADDR_LEN - 1
#define CUS_START_ADDRESS                   MMAP_START_ADDRESS
#define CUS_END_ADDRESS                     (CUS_START_ADDRESS + MMAP_CUS_ADDR_LEN - 1)

#if defined (__SMALL_ROM__) || defined (__SMALL_ROM_32__)
#define ROM_REMAP_ADDR_LEN                  0x00100000
#else
#define ROM_REMAP_ADDR_LEN                  0x00200000
#endif
#define ROM_REMAP_START_ADDR                DPM_START_ADDR - ROM_REMAP_ADDR_LEN
#define ROM_MMI_START_ADDR                  0x000010C0
#define ROM_MMI_ADDR_LEN                    ROM_REMAP_ADDR_LEN - ROM_MMI_START_ADDR
#endif
//======================================================================
//================= vmalloc address ===================
//======================================================================
#define VMALLOC_START_ADDRESS         0xDE000000
#define VMALLOC_ADDR_LEN                   0x02000000
#define VMALLOC_END_ADDRESS             (VMALLOC_START_ADDRESS+VMALLOC_ADDR_LEN-1)


#if 0
//======================================================================
//================= SLDLM address (User Load/Unload) ===================
//======================================================================
#define USR_SLDLM_START_ADDR                0x1E000000 /* !! This address must match the first USR_SLDLM start address !! */

#define USR_SLDLM_ATVDRV_START_ADDR         0x1E000000
#define USR_SLDLM_ATVDRV_END_ADDR           0x1E0FFFFF
#define USR_SLDLM_AMRNBCODEC_START_ADDR     0x1E100000
#define USR_SLDLM_AMRNBCODEC_END_ADDR       0x1E1FFFFF
#define USR_SLDLM_MP3DEC_START_ADDR         0x1E200000
#define USR_SLDLM_MP3DEC_END_ADDR           0x1E2FFFFF
#define USR_SLDLM_SBCENC_START_ADDR         0x1E300000
#define USR_SLDLM_SBCENC_END_ADDR           0x1E3FFFFF
#define USR_SLDLM_AUDIO_POSTPROCESS_START_ADDR      0x1E400000
#define USR_SLDLM_AUDIO_POSTPROCESS_END_ADDR        0x1E4FFFFF
#define USR_SLDLM_AUDIO_SRC_START_ADDR      0x1E500000
#define USR_SLDLM_AUDIO_SRC_END_ADDR        0x1E5FFFFF
#define USR_SLDLM_AACDEC_START_ADDR         0x1E600000
#define USR_SLDLM_AACDEC_END_ADDR           0x1E6FFFFF
#define USR_SLDLM_MIDI_START_ADDR           0x1E700000
#define USR_SLDLM_MIDI_END_ADDR             0x1E7FFFFF
#define USR_SLDLM_VIDEC_COMMON_START_ADDR   0x1E800000
#define USR_SLDLM_VIDEC_COMMON_END_ADDR     0x1E8FFFFF
#define USR_SLDLM_M4VDEC_START_ADDR         0x1E900000
#define USR_SLDLM_M4VDEC_END_ADDR           0x1E9FFFFF
#define USR_SLDLM_AVCDEC_START_ADDR         0x1EA00000
#define USR_SLDLM_AVCDEC_END_ADDR           0x1EAFFFFF
#define USR_SLDLM_RVCDEC_START_ADDR         0x1EB00000
#define USR_SLDLM_RVCDEC_END_ADDR           0x1EBFFFFF
#define USR_SLDLM_COOKDEC_START_ADDR        0x1EC00000
#define USR_SLDLM_COOKDEC_END_ADDR          0x1ECFFFFF
#define USR_SLDLM_AVP_PLAYER_START_ADDR     0x1ED00000
#define USR_SLDLM_AVP_PLAYER_END_ADDR       0x1EDFFFFF
#define USR_SLDLM_CAPTURE_START_ADDR        0x1EE00000
#define USR_SLDLM_CAPTURE_END_ADDR          0x1EEFFFFF
#define USR_SLDLM_VDR_START_ADDR            0x1EF00000
#define USR_SLDLM_VDR_END_ADDR              0x1EFFFFFF
#define USR_SLDLM_AMRNBDEC_START_ADDR       0x1F000000
#define USR_SLDLM_AMRNBDEC_END_ADDR         0x1F0FFFFF
#define USR_SLDLM_AMRNBENC_START_ADDR       0x1F100000
#define USR_SLDLM_AMRNBENC_END_ADDR         0x1F1FFFFF
#define USR_SLDLM_MAP_START_ADDR            0x1F200000
#define USR_SLDLM_MAP_END_ADDR              0x1F2FFFFF
#define USR_SLDLM_MAR_START_ADDR            0x1F300000
#define USR_SLDLM_MAR_END_ADDR              0x1F3FFFFF
#define USR_SLDLM_HANDWRITING_START_ADDR    0x1F400000
#define USR_SLDLM_HANDWRITING_END_ADDR      0x1F4FFFFF
#define USR_SLDLM_WAP_START_ADDR            0x1F500000
#define USR_SLDLM_WAP_END_ADDR              0x1F5FFFFF
#define USR_SLDLM_AUDIO_ARP_START_ADDR      0x1F600000
#define USR_SLDLM_AUDIO_ARP_END_ADDR        0x1F6FFFFF
#define USR_SLDLM_SXM_CORE_START_ADDR       0X1F700000
#define USR_SLDLM_SXM_CORE_END_ADDR         0X1F7FFFFF
#define USR_SLDLM_IME_MY_START_ADDR         0x1F800000
#define USR_SLDLM_IME_MY_END_ADDR           0x1F8FFFFF
#define USR_SLDLM_MJAVA_START_ADDR          0x1F900000
#define USR_SLDLM_MJAVA_END_ADDR            0x1F9FFFFF
#define USR_SLDLM_GPS_START_ADDR            0x1FA00000
#define USR_SLDLM_GPS_END_ADDR              0x1FAFFFFF
#define USR_SLDLM_GPS_FW_START_ADDR         0x1FB00000
#define USR_SLDLM_GPS_FW_END_ADDR           0x1FBFFFFF
#define USR_SLDLM_CAMERA_START_ADDR         0x1FC00000
#define USR_SLDLM_CAMERA_END_ADDR           0x1FCFFFFF
#define USR_SLDLM_ZYUE_CORE_START_ADDR      0x1FD00000
#define USR_SLDLM_ZYUE_CORE_END_ADDR        0x1FDFFFFF
#define USR_SLDLM_ZMOL_CORE_START_ADDR      0x1FE00000
#define USR_SLDLM_ZMOL_CORE_END_ADDR        0x1FEFFFFF
#define USR_SLDLM_SKY_CORE_START_ADDR       0x1FF00000
#define USR_SLDLM_SKY_CORE_END_ADDR         0x1FFFFFFF
#define USR_SLDLM_TENCENTMCARE_START_ADDR   0x20000000
#define USR_SLDLM_TENCENTMCARE_END_ADDR     0x200FFFFF
#define USR_SLDLM_PLATINIT_START_ADDR   0x20100000
#define USR_SLDLM_PLATINIT_END_ADDR     0x201FFFFF
#define USR_SLDLM_FMRX_START_ADDR           0x20200000
#define USR_SLDLM_FMRX_END_ADDR             0x202FFFFF
#define USR_SLDLM_BLUETOOTH_START_ADDR      0x20300000
#define USR_SLDLM_BLUETOOTH_END_ADDR        0x203FFFFF
#define USR_SLDLM_OAKMONT_JAVA_START_ADDR   0x20400000
#define USR_SLDLM_OAKMONT_JAVA_END_ADDR     0x204FFFFF
#define USR_SLDLM_OAKMONT_JAVA_EX_START_ADDR 0x20500000
#define USR_SLDLM_OAKMONT_JAVA_EX_END_ADDR   0x205FFFFF
#define USR_SLDLM_USB_START_ADDR        0x20600000
#define USR_SLDLM_USB_END_ADDR          0x206FFFFF
#define USR_SLDLM_MP3DECINIT_START_ADDR     0x20700000
#define USR_SLDLM_MP3DECINIT_END_ADDR       0x207FFFFF
#define USR_SLDLM_AACDECINIT_START_ADDR     0x20800000
#define USR_SLDLM_AACDECINIT_END_ADDR       0x208FFFFF
#define USR_SLDLM_SRSWOWHD_START_ADDR  0x20900000
#define USR_SLDLM_SRSWOWHD_END_ADDR       0x209FFFFF
#define USR_SLDLM_MRFILE_BUFFER_START_ADDR      0x20A00000
#define USR_SLDLM_MRFILE_BUFFER_END_ADDR        0x20AFFFFF
#define USR_SLDLM_EMAPP_START_ADDR              0x20B00000
#define USR_SLDLM_EMAPP_END_ADDR                0x20BFFFFF
#define USR_SLDLM_OFFAPP_START_ADDR             0x20C00000
#define USR_SLDLM_OFFAPP_END_ADDR               0x20CFFFFF
#define USR_SLDLM_SOKOBANAPP_START_ADDR         0x20D00000
#define USR_SLDLM_SOKOBANAPP_END_ADDR           0x20DFFFFF
#define USR_SLDLM_CC_START_ADDR             0x20E00000
#define USR_SLDLM_CC_END_ADDR               0x20EFFFFF
#define USR_SLDLM_SS_START_ADDR             0x20F00000
#define USR_SLDLM_SS_END_ADDR               0x20FFFFFF
#define USR_SLDLM_SMS_START_ADDR            0x21000000
#define USR_SLDLM_SMS_END_ADDR              0x210FFFFF
#define USR_SLDLM_MMGMM_START_ADDR          0x21100000
#define USR_SLDLM_MMGMM_END_ADDR            0x211FFFFF
#define USR_SLDLM_INDIANCALENDAR_START_ADDR     0x21200000
#define USR_SLDLM_INDIANCALENDAR_END_ADDR       0x212FFFFF
#define USR_SLDLM_END_ADDR                  0x212FFFFF /* !! This address must match the last USR_SLDLM end address !! */

//======================================================================
//================= SLDLM address (System Load/Unload) =================
//======================================================================

#define SYS_SLDLM_START_ADDR                0x30000000 /* !! This address must match the first SYS_SLDLM start address !! */

#define SYS_SLDLM_DUMMY_START_ADDR          0x30000000
#define SYS_SLDLM_DUMMY_END_ADDR            0x300FFFFF

#define SYS_SLDLM_END_ADDR                  0x300FFFFF /* !! This address must match the last SYS_SLDLM end address !! */
#endif
#endif //__CPU_MEM_MAP_B5_H__
