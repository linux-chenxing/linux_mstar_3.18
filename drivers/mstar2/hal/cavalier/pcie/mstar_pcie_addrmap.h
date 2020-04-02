///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2014 MStar Semiconductor, Inc.
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

#ifndef _MSTAR_PCIE_ADDRMAP_HEADER
#define _MSTAR_PCIE_ADDRMAP_HEADER

/*
 * ============================================================================
 * Include Headers
 * ============================================================================
 */
#include "cdn_stdint.h"
#include "cpdi.h"

/*
 * ============================================================================
 * Local Variables Definitions
 * ============================================================================
 */

/*
 * ============================================================================
 * Constant Definitions
 * ============================================================================
 */
extern ptrdiff_t mstar_pm_base;

#define CPED_PCIE_AXI_VIRT_BASE                 0x0E300000  // (CPDI_DEFAULT_ECAM_BASE_BUS_ADDRESS)
#define CPED_PCIE_AXI_PHYS_BASE                 0x0E400000  // (SRAM Resgin 0-31, OutBound Use)

//#define ZEBU_ESL

/* NOTE - These addresses are specific to the Cadence verification environment */
#ifdef ZEBU_ESL
#define REG_RC_BASE                     0x1F280E00    // Kano RC Top Wrapper base address
#define REG_AXI2MI_BASE                 0x1F281000    // Kano Axi2mi brigdge base address
#define CPDREF_RP_BASE                  0x1F3A0000    // Kano RC MAC IP Controller 
#define FUART_BASE                      0x1F220D00    // UART base
#define REG_TYPE1_CS_BASE               0x09300000    // TYPE1 Configuration Space
#define UINT64_T                        uint64_t
#else

#if defined (CONFIG_ARM64)
#define REG_TYPE1_CS_BASE               0x09300000                     // TYPE1 Configuration Space
#define REG_RC_BASE                     mstar_pm_base + 0x00280E00    // Kano RC Top Wrapper base address
#define REG_AXI2MI_BASE                 mstar_pm_base + 0x00281000    // Kano Axi2mi brigdge base address
#define CPDREF_RP_BASE                  mstar_pm_base + 0x003A0000    // Kano RC MAC IP Controller , Must Use Write32, it doesnt support 16bit write
#define REG_LOCAL_MANAGMENT             mstar_pm_base + 0x003A1000    // RC Local Management Registers, Must Use Write32/Read32
#define FUART_BASE                      mstar_pm_base + 0x00220D00    // UART base
#define REG_HST2_IRQ_MASK_31_16_        REG_INT_HYP_BASE + 0x55*4
#define REG_HST2_IRQ_STATUS_31_16_      REG_INT_HYP_BASE + 0x5D*4
#define E_IRQ_INT_PCIE_RC               E_IRQHYPH_PCIE_RC
#define MSTAR_PCIE_U3PHY_P0_DTOP_BASE   MSTAR_RIU_BASE + (0x00102000<<1)
#define MSTAR_PCIE_U3PHY_P1_DTOP_BASE   mstar_pm_base + (0x00102200<<1) // d_top bank address : 0x1022
#define MSTAR_PCIE_U3PHY_P1_ATOP_BASE   mstar_pm_base + (0x00102300<<1) // a_top bank address : 0x1023
#define MSTAR_PCIE_UTMISS1_BASE         mstar_pm_base + (0x00103880<<1)
#define MSTAR_PCIE_UPLL1_BASE           mstar_pm_base + (0x00100820<<1)
#define UINT64_T                        uint64_t

#else
#define MSTAR_RIU_BASE                  0xFD000000
#define REG_TYPE1_CS_BASE               0x09300000                     // TYPE1 Configuration Space
#define REG_RC_BASE                     MSTAR_RIU_BASE + 0x00280E00    // Kano RC Top Wrapper base address
#define REG_AXI2MI_BASE                 MSTAR_RIU_BASE + 0x00281000    // Kano Axi2mi brigdge base address
#define CPDREF_RP_BASE                  MSTAR_RIU_BASE + 0x003A0000    // Kano RC MAC IP Controller , Must Use Write32, it doesnt support 16bit write
#define REG_LOCAL_MANAGMENT             MSTAR_RIU_BASE + 0x003A1000    // RC Local Management Registers, Must Use Write32/Read32
#define FUART_BASE                      MSTAR_RIU_BASE + 0x00220D00    // UART base
#define REG_HST2_IRQ_MASK_31_16_        REG_INT_HYP_BASE + 0x55*4
#define REG_HST2_IRQ_STATUS_31_16_      REG_INT_HYP_BASE + 0x5D*4
#define E_IRQ_INT_PCIE_RC               E_IRQHYPH_PCIE_RC
#define MSTAR_PCIE_U3PHY_P0_DTOP_BASE   MSTAR_RIU_BASE + (0x00102000<<1)
#define MSTAR_PCIE_U3PHY_P1_DTOP_BASE   MSTAR_RIU_BASE + (0x00102200<<1) // d_top bank address : 0x1022
#define MSTAR_PCIE_U3PHY_P1_ATOP_BASE   MSTAR_RIU_BASE + (0x00102300<<1) // a_top bank address : 0x1023
#define MSTAR_PCIE_UTMISS1_BASE         MSTAR_RIU_BASE + (0x00103880<<1) 
#define MSTAR_PCIE_UPLL1_BASE           MSTAR_RIU_BASE + (0x00100820<<1)
#define MSTAR_REG_000F_BANK             MSTAR_RIU_BASE + (0x00000F00<<1)
#define UINT64_T                        uint32_t
#endif //end CONFIG_ARM64
#endif

#define CPDI_RP_IB_APERTURE_SIZE             19            // 1<< (19+1) == 1MB
#define CPDI_RP_OB_APERTURE_SIZE             15            // 1<< (15+1) == 64KB
#define CPDI_RP_OB_DESCRIPTOR_SIZE          128            // HW Designed Fixed Size
#define CPDI_RP_OB_DESCRIPTOR_TOTAL_SIZE   1024            // 1KB
#define CPDI_RP_OB_DESCRIPTOR_NUMBER_MAX      8            // CPDI_RP_OB_DESCRIPTOR_TOTAL_SIZE/CPDI_RP_OB_DESCRIPTOR_SIZE
#define CPDI_RP_OB_BASE_ADDR             0x10000           // 64Kb OutBound Address Translation Base 4
#define CPDI_RP_OB_REGION_NUMBER              1            // must > 0
#define CPDI_RP_OB_REGION_ENABLE             25

#define CPDREF_PCIE_AXI_PHYS_BASE_EP            0x00100000    // 1MB inbound Address Translation base  Inbound 1MB ;This field defines the base address of the memory address range.
#define CPDREF_RP_SRAM_LOCAL                    0x22600000    // BAR0 RP Inbound Physical MIU Address Translation
#define CPDREF_RP_SRAM_LOCAL_OB                 CPDREF_RP_SRAM_LOCAL + (1<<(CPDI_RP_IB_APERTURE_SIZE+1))
#define CPDREF_RP_SRAM_LOCAL_OB_DESCRIPTOR      CPDREF_RP_SRAM_LOCAL + (1<<(CPDI_RP_IB_APERTURE_SIZE+1)) + (1<<(CPDI_RP_OB_APERTURE_SIZE+1)) //use 32 bytes
#define MIU_BASE                                0x20000000
#define MIU0_BASE                               0x100000000
#define MIU1_BASE                               0x200000000

#define RC_DMA_BUFFER_SIZE              2048
#define CADENCE_VENDOR_ID               0xDEDB
#define CPDREF_TEST_FAILURE             1

/*
 * Under Cadence Spec., each RC has 4096 bytes (0x0 ~ 0x3FF) of configuration address space !
 */
#define PCI_RC_VENDOR_ID		0x00	/* 16 bits */
#define PCI_RC_DEVICE_ID		0x02	/* 16 bits */
#define PCI_RC_COMMAND		  0x04	/* 16 bits */
#define PCI_RC_STATUS	    	0x06	/* 16 bits */
#define PCI_RC_REVISION_ID	0x08	/* 16 bits */
#define PCI_RC_CLASS_CODE 	0x0A	/* 16 bits */
#define PCI_RC_BIST       	0x0C	/* 32 bits */
#define PCI_RC_BAR_0       	0x10	/* 32 bits */
#define PCI_RC_BAR_1       	0x14	/* 32 bits */
 
//OutBound DMA Engine
#define MSTAR_PCIE_DMA_INTR        0x1
#define MSTAR_PCIE_DMA_CONTI       0x2
#define MSTAR_PCIE_DMA_STOP        0x0
#define MSTAR_PCIE_DMA_WRITE       0x4 //Outbound
#define MSTAR_PCIE_DMA_READ        0x0 //Inbound
#define MSTAR_PCIE_DMA_HIGH_PRI    0x8
#define MSTAR_PCIE_DMA_NORMAL_PRI  0x0

/****************************************************************************/
/* VC enhanced capabilities register and fields                          */
/* These are defined in the ECN                                          */
/****************************************************************************/
struct CPDREF_VC_enhanced_cap {
  volatile uint32_t i_vc_enh_cap_header;          /*      0x4c0 - 0x4c4      */
  volatile uint32_t i_port_vc_cap1;               /*      0x4c4 - 0x4c8      */
  volatile uint32_t i_port_vc_cap2;               /*      0x4c8 - 0x4cc      */
  volatile uint32_t i_port_vc_ctrl_stat;          /*      0x4cc - 0x4d0      */
  volatile uint32_t i_vc_resource_cap0;           /*      0x4d0 - 0x4d4      */
  volatile uint32_t i_vc_resource_ctrl0;          /*      0x4d4 - 0x4d8      */
  volatile uint32_t i_vc_resource_stat0;          /*      0x4d8 - 0x4dc      */
  volatile uint32_t i_vc_resource_cap1;           /*      0x4dc - 0x4e0      */
  volatile uint32_t i_vc_resource_ctrl1;          /*      0x4e0 - 0x4e4      */
  volatile uint32_t i_vc_resource_stat1;          /*      0x4e4 - 0x4e8      */
};

struct CPDREF_RC_addrmap {
  volatile char pad__0[0x4C0];                    /*      0x2f4 - 0x4c0      */
  struct CPDREF_VC_enhanced_cap vc_cap_struct;    /*      0x4c0 - 0x4e8      */
  volatile char pad__1[0x910];                    /*      0x4e8 - 0x900      */
};

// For OutBound DMA Engine
#define PCIE_MSTAR_DMA_DESCRIPTOR_SIZE                              16
#define PCIE_MSTAR_DMA_DESCRIPTOR_NUMBER                             2

#define PCIE_MSTAR_DMA_DESCRIPTOR_STARTADDR_SHIFT                    0
#define PCIE_MSTAR_DMA_DESCRIPTOR_STARTADDR_WIDTH                   28
#define PCIE_MSTAR_DMA_DESCRIPTOR_STARTADDR_MASK           0x0fffffffU

#define PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_SHIFT                       0
#define PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_WIDTH                      20
#define PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA1          0xf0000000U
#define PCIE_MSTAR_DMA_DESCRIPTOR_LENGTH_MASK_PA2          0x0000ffffU

#define PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_SHIFT                      16
#define PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_WIDTH                       8
#define PCIE_MSTAR_DMA_DESCRIPTOR_AXI_ID_MASK              0x00ff0000U


#define PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_LOW_MASK       0xff000000U
#define PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_WIDTH                   32
#define PCIE_MSTAR_DMA_DESCRIPTOR_BASE_ADDR_HIGH_MASK      0x00ffffffU

#define PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_SHIFT                   24
#define PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_WIDTH                    8
#define PCIE_MSTAR_DMA_DESCRIPTOR_PARAMETER_MASK           0xff000000U


#define PCIE_MSTAR_DMA_DESCRIPTOR_NEXT_SHIFT                         0
#define PCIE_MSTAR_DMA_DESCRIPTOR_NEXT_WIDTH                        28
#define PCIE_MSTAR_DMA_DESCRIPTOR_NEXT_MASK                0x0fffffffU

struct Mstar_DMA_Descriptor {
    uint32_t              para1;     
    uint32_t              para2;        
    uint32_t              para3;         
    uint32_t              para4;   
};

#define DEBUG_RUNTIME_LOCAL_ERROR

#ifdef DEBUG_RUNTIME_LOCAL_ERROR
struct Mstar_LocalError_Descriptor {
    uint32_t PNP_RX_FIFO_PARITY_ERROR_Count;
    uint32_t COMPLETION_RX_FIFO_PARITY_ERROR_Count;
    uint32_t REPLAY_RAM_PARITY_ERROR_Count;
    uint32_t PNP_RX_FIFO_OVERFLOW_CONDITION_Count;
    uint32_t COMPLETION_RX_FIFO_OVERFLOW_CONDITION_Count;
    uint32_t REPLAY_TIMEOUT_CONDITION_Count;
    uint32_t REPLAY_TIMEOUT_ROLLOVER_CONDITION_Count;
    uint32_t PHY_ERROR_Count;
    uint32_t MALFORMED_TLP_RECEIVED_Count;
    uint32_t UNEXPECTED_COMPLETION_RECEIVED_Count;
    uint32_t FLOW_CONTROL_ERROR_Count;
    uint32_t COMPLETION_TIMEOUT_CONDITION_Count;
};
#endif

uint32_t  CPDREF_ProbeAndInitRPDriver (void *pD,uint32_t pd_size) ;
uint32_t _mdrv_RC_ob_config_read(void *pD, uint32_t addr, UINT64_T *data);
uint32_t _mdrv_RC_ob_config_write(void *pD, uint32_t addr, uint32_t data);
uint32_t _mdrv_RC_ob_message_write(void *pD, uint32_t msg_region, uint32_t msg_code, uint32_t msg_routing);
uint32_t _mdrv_RC_outbound_DMA_read(void *pD, uint32_t descriptor_addr, uint32_t data_addr,  uint32_t dma_translate_size, uint32_t offset);
uint32_t _mdrv_RC_outbound_DMA_write(void *pD, uint32_t descriptor_addr, uint32_t data_addr,  uint32_t dma_translate_size);

uint32_t _mdrv_RC_inbound_DMA_read(void *pD);
uint32_t _mdrv_RC_inbound_DMA_write(void *pD, char *FILENAME);
uint32_t _mdrv_RC_ib_msi_config(void *pD, uint32_t msi_addr, uint32_t msi_data,  unsigned short msi_ctrl);

void     _mdrv_write_pcie_bar0(void);
void     _mdrv_read_pcie_bar0(void);
uint32_t _mdrv_pcie_irq_init(void);
uint32_t _mdrv_system_init(void);
uint32_t _mdrv_check_LinkTrainingComplete(void);
uint32_t _mdrv_pcie_power_onoff(bool bflag);
#ifdef DEBUG_RUNTIME_LOCAL_ERROR
uint32_t _mdrv_pcie_trigger_error_callback(void *pD);
#endif

uint32_t  CPDREF_GetConfigurationInfo(void *pD);

uint32_t CPDREF_PrintLtssmState ( void * pD , uint32_t *pOldState );

uint32_t CPDREF_PrintLinkInfo ( void * pD  );

uint32_t CPDREF_WaitForL0 (void * pD) ;


void uart_putc(char out);
void uart_puts(char out[], int string_num);
void uart_write_U8_hex(unsigned char c);
void uart_write_U16_hex(unsigned short val);
void uart_write_U32_hex(unsigned int val);

#endif