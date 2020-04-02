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
/// @file   EMAC.h
/// @author MStar Semiconductor Inc.
/// @brief  EMAC Driver Interface
///////////////////////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// Linux EMAC.h define start
// -----------------------------------------------------------------------------
#ifndef __DRV_EMAC__
#define __DRV_EMAC__

//-------------------------------------------------------------------------------------------------
//  Include files
//-------------------------------------------------------------------------------------------------
#include <mstar/mstar_chip.h>
//-------------------------------------------------------------------------------------------------
//  Define Enable or Compiler Switches
//-------------------------------------------------------------------------------------------------
#define XIU32_MODE
#define CHIP_FLUSH_READ
#define EMAC_RX_ADDRESS_32BIT
//#define MEMORY_PROTECT
#define RX_DESC_MODE
#ifdef CONFIG_EMAC_RX_DMA
#define RX_ZERO_COPY
#endif
#define RX_CHECKSUM
#ifdef CONFIG_EMAC_RX_NAPI
#define RX_NAPI
#endif
#ifdef CONFIG_EMAC_RX_GRO
#define RX_GRO
#endif
#ifdef CONFIG_EMAC_RX_DELAY_INTERRUPT
#define RX_DELAY_INTERRUPT
#endif
#ifdef CONFIG_EMAC_TX_DMA
#define TX_ZERO_COPY
#endif
#ifdef CONFIG_EMAC_TX_DESC_MODE
#define TX_DESC_MODE
#else
#ifdef CONFIG_EMAC_NEW_TX_QUEUE
#define NEW_TX_QUEUE
#else
#define TX_QUEUE_4
#endif
#endif
#ifdef CONFIG_EMAC_TX_SOFTWARE_QUEUE
#define TX_SOFTWARE_QUEUE
#endif
#ifdef CONFIG_MSTAR_HW_TX_CHECKSUM
#define HW_TX_CHECKSUM
#define HW_TX_CHECKSUM_IPV6
#endif
#ifdef CONFIG_EMAC_10T_RANDOM_WAVEFORM
#define EMAC_10T_RANDOM_WAVEFORM
#endif
//#define TX_COM_ENABLE
//#define TX_NAPI
// Compiler Switches
#define URANUS_ETHER_ADDR_CONFIGURABLE  /* MAC address can be changed? */
//--------------------------------------------------------------------------------------------------
//  Constant definition
//--------------------------------------------------------------------------------------------------
#define EMAC_DBG(fmt, args...)              {printk("Mstar_emac: "); printk(fmt, ##args);}
#define TRUE                        1
#define FALSE                       0

#define SOFTWARE_DESCRIPTOR_ENABLE  0x0001UL
#define RX_CHECKSUM_ENABLE          0x000EUL
#define EMAC_SPEED_100              100
#define MAX_INT_COUNTER             100
#define TX_CHECKSUM_ENABLE          (0x00000470UL)
#define TX_V6_CHECKSUM_ENABLE       (0x0000000CUL)
#define EMAC_MAX_TX_QUEUE           TX_LOW_PRI_DESC_NUMBER
//Interrupt
#define MAC_IRQ                     E_IRQ_EMAC
#define EMAC_INT_MASK               (0xdffUL)
#define EMAC_INT_ALL                (0xffff)
#ifdef TX_COM_ENABLE
#define EMAC_INT_ENABLE             EMAC_INT_DONE|EMAC_INT_RBNA|EMAC_INT_TUND|EMAC_INT_RTRY|EMAC_INT_ROVR|EMAC_INT_TCOM|(0x0000E000UL)
#else
#define EMAC_INT_ENABLE             EMAC_INT_DONE|EMAC_INT_RBNA|EMAC_INT_TUND|EMAC_INT_RTRY|EMAC_INT_ROVR|(0x0000E000UL)
#endif
// Base address here:
#if defined(CONFIG_ARM)
#define  mstar_pm_base              0xFD000000
#elif defined(CONFIG_MIPS)
#define  mstar_pm_base              0xBF000000
#endif
#define RIU_REG_BASE                mstar_pm_base
#define REG_ALBANY0_BANK            0x0031UL
#define REG_EMAC0_BANK              0x1220UL
#define REG_EMAC0_XIU32_BANK        0x1A21UL
#define INTERNEL_PHY_REG_BASE       RIU_REG_BASE + ((REG_ALBANY0_BANK*0x100UL)<<1)
#define EMAC0_XIU16_REG_ADDR_BASE   RIU_REG_BASE + ((REG_EMAC0_BANK*0x100UL)<<1)
#define EMAC0_XIU32_REG_ADDR_BASE   RIU_REG_BASE + ((REG_EMAC0_XIU32_BANK*0x100UL)<<1)
#define REG_ADDR_BASE_XIU16         EMAC0_XIU16_REG_ADDR_BASE
#define REG_ADDR_BASE_XIU32         EMAC0_XIU32_REG_ADDR_BASE
#define REG_ADDR_BASE               REG_ADDR_BASE_XIU16
#define MIU0_BUS_BASE               MSTAR_MIU0_BUS_BASE
// Config Registers Value
#define WRITE_PROTECT_ENABLE        EMAC_MIU_WRITE_PROTECT|EMAC_MIU_WP_INT_EN
#define SOFTWARE_RESET              EMAC_SW_RESET_MIU|EMAC_SW_RS_EMAC_TO_MIU|EMAC_SW_RESET_APB|EMAC_SW_RESET_AHB
#define EXTERNAL_PHY                EMAC_RMII|EMAC_RMII_12
#ifdef CONFIG_ETHERNET_ALBANY
    #ifdef MEMORY_PROTECT
#define CONFIG2_VAL                 EMAC_POWER_UP|SOFTWARE_RESET|WRITE_PROTECT_ENABLE
    #else
#define CONFIG2_VAL                 EMAC_POWER_UP|SOFTWARE_RESET
    #endif
#else
    #ifdef MEMORY_PROTECT
#define CONFIG2_VAL                 EMAC_POWER_UP|SOFTWARE_RESET|EMAC_RMII|EMAC_RMII_12|WRITE_PROTECT_ENABLE
    #else
#define CONFIG2_VAL                 EMAC_POWER_UP|SOFTWARE_RESET|EMAC_RMII|EMAC_RMII_12
    #endif
#endif
// Delay Interrupt mode:
//#define DELAY_NUMBER                0x40
//#define DELAY_TIME                  0x06
#define DELAY_NUMBER                CONFIG_EMAC_DELAY_INTERRUPT_NUMBER
#define DELAY_TIME                  CONFIG_EMAC_DELAY_INTERRUPT_TIMEOUT
#define DELAY_INTERRUPT_CONFIG      EMAC_INT_DELAY_MODE_EN|(DELAY_NUMBER<<EMAC_INT_DELAY_NUMBER_SHIFT)|(DELAY_TIME<<EMAC_INT_DELAY_TIME_SHIFT)
// RX Descriptor here:
#define EMAC_CLEAR_BUFF             0x0
#define RX_DESC_SIZE                0x0008UL                                //2words = 8bytes = 64bits
#ifdef RX_ZERO_COPY
#define RX_DESC_NUMBER              0x0400UL                                //32 descriptor
#else
#define RX_DESC_NUMBER              0x0100UL                                //32 descriptor
#endif
#define RX_DESC_TABLE_SIZE          (RX_DESC_NUMBER*RX_DESC_SIZE)
#define RX_DESC_REFILL_NUMBER       RX_DESC_NUMBER/2
// RX Buffer here:
#define RX_BUFF_ENTRY_SIZE          0x800UL                                 //2048bytes
#define RX_BUFF_SIZE                (RX_DESC_NUMBER*RX_BUFF_ENTRY_SIZE)
// TX Descriptor here:
#define TX_DESC_DELAY_FREE          0x04
#define TX_DESC_SIZE                0x0010UL                                //4words = 16bytes = 128bits
#define TX_LOW_PRI_DESC_NUMBER      CONFIG_EMAC_TX_DESC_NUMBER                                //32 descriptor
#define TX_LOW_PRI_DESC_TABLE_SIZE  (TX_DESC_SIZE*TX_LOW_PRI_DESC_NUMBER)
#define TX_HIGN_PRI_DESC_NUMBER     TX_LOW_PRI_DESC_NUMBER                  //32 descriptor
#define TX_HIGN_PRI_DESC_TABLE_SIZE (TX_DESC_SIZE*TX_HIGN_PRI_DESC_NUMBER)
#define TX_DESC_REFILL_NUMBER       TX_LOW_PRI_DESC_NUMBER/2
// TX Buffer here:
#define TX_BUFF_ENTRY_SIZE          0x800UL
#ifdef NEW_TX_QUEUE
//#define TX_BUFF_ENTRY_NUMBER        0x20UL  //N=MAX 3F
#define TX_BUFF_ENTRY_NUMBER        CONFIG_EMAC_NEW_TX_QUEUE_THRESHOLD
#endif
#ifdef TX_DESC_MODE
#define TX_BUFF_ENTRY_NUMBER        TX_LOW_PRI_DESC_NUMBER
#endif
#ifndef TX_BUFF_ENTRY_NUMBER
#define TX_BUFF_ENTRY_NUMBER        0x08UL
#endif
//PHY Register
#define PHY_REG_BASIC               (0)
#define PHY_REG_STATUS              (1)
//PHY setting
#define PHY_REGADDR_OFFSET          (18)
#define PHY_ADDR_OFFSET             (23)

#ifdef RX_NAPI
//#define RX_NAPI_WEIGHT              0x30
#define RX_NAPI_WEIGHT              CONFIG_EMAC_RX_NAPI_WEIGHT
#endif

#ifdef TX_NAPI
#define TX_NAPI_WEIGHT              0x40
#endif

#ifdef TX_SOFTWARE_QUEUE
#define TX_SW_QUEUE_SIZE            (1024)  //effected size = TX_RING_SIZE - 1
#define TX_DESC_CLEARED             0
#define TX_DESC_WROTE               1
#define TX_DESC_READ                2
#define TX_FIFO_SIZE                4 //HW FIFO size

#define TX_SW_QUEUE_IN_GENERAL_TX   0
#define TX_SW_QUEUE_IN_IRQ          1
#define TX_SW_QUEUE_IN_TIMER        2
#endif

//-------------------------------------------------------------------------------------------------
//  Bit Define
//-------------------------------------------------------------------------------------------------
#define EMAC_MIU_RW       ( 0x3UL << 10)   //EMAC power on clk

// -------- EMAC_CTL : (EMAC Offset: 0x0)  --------
#define EMAC_LB           ( 0x1UL <<  0) // (EMAC) Loopback. Optional. When set, loopback signal is at high level.
#define EMAC_LBL          ( 0x1UL <<  1) // (EMAC) Loopback local.
#define EMAC_RE           ( 0x1UL <<  2) // (EMAC) Receive enable.
#define EMAC_TE           ( 0x1UL <<  3) // (EMAC) Transmit enable.
#define EMAC_MPE          ( 0x1UL <<  4) // (EMAC) Management port enable.
#define EMAC_CSR          ( 0x1UL <<  5) // (EMAC) Clear statistics registers.
#define EMAC_ISR          ( 0x1UL <<  6) // (EMAC) Increment statistics registers.
#define EMAC_WES          ( 0x1UL <<  7) // (EMAC) Write enable for statistics registers.
#define EMAC_BP           ( 0x1UL <<  8) // (EMAC) Back pressure.
// -------- EMAC_CFG : (EMAC Offset: 0x4) Network Configuration Register --------
#define EMAC_SPD          ( 0x1UL <<  0) // (EMAC) Speed.
#define EMAC_FD           ( 0x1UL <<  1) // (EMAC) Full duplex.
#define EMAC_BR           ( 0x1UL <<  2) // (EMAC) Bit rate.
#define EMAC_CAF          ( 0x1UL <<  4) // (EMAC) Copy all frames.
#define EMAC_NBC          ( 0x1UL <<  5) // (EMAC) No broadcast.
#define EMAC_MTI          ( 0x1UL <<  6) // (EMAC) Multicast hash enable
#define EMAC_UNI          ( 0x1UL <<  7) // (EMAC) Unicast hash enable.
#define EMAC_RLF          ( 0x1UL <<  8) // (EMAC) Receive Long Frame.
#define EMAC_EAE          ( 0x1UL <<  9) // (EMAC) External address match enable.
#define EMAC_CLK          ( 0x3UL << 10) // (EMAC)
#define EMAC_CLK_HCLK_8   ( 0x0UL << 10) // (EMAC) HCLK divided by 8
#define EMAC_CLK_HCLK_16  ( 0x1UL << 10) // (EMAC) HCLK divided by 16
#define EMAC_CLK_HCLK_32  ( 0x2UL << 10) // (EMAC) HCLK divided by 32
#define EMAC_CLK_HCLK_64  ( 0x3UL << 10) // (EMAC) HCLK divided by 64
#define EMAC_RT           ( 0x1UL << 12) // (EMAC) Retry test
#define EMAC_CAMMEG       ( 0x1UL << 13) // (EMAC)
// -------- EMAC_SR : (EMAC Offset: 0x8) Network Status Register --------
#define EMAC_MDIO         ( 0x1UL <<  1) // (EMAC)
#define EMAC_IDLE         ( 0x1UL <<  2) // (EMAC)
// -------- EMAC_TCR : (EMAC Offset: 0x10) Transmit Control Register --------
#define EMAC_LEN          ( 0x7FFUL << 0) // (EMAC)
#define EMAC_NCRC         ( 0x1UL << 15) // (EMAC)
// -------- EMAC_TSR : (EMAC Offset: 0x14) Transmit Control Register --------
#define EMAC_OVR          ( 0x1UL <<  0) // (EMAC)
#define EMAC_COL          ( 0x1UL <<  1) // (EMAC)
#define EMAC_RLE          ( 0x1UL <<  2) // (EMAC)
//#define EMAC_TXIDLE     ( 0x1UL <<  3) // (EMAC)
#define EMAC_IDLETSR      ( 0x1UL <<  3) // (EMAC)
#define EMAC_BNQ          ( 0x1UL <<  4) // (EMAC)
#define EMAC_COMP         ( 0x1UL <<  5) // (EMAC)
#define EMAC_UND          ( 0x1UL <<  6) // (EMAC)
#define EMAC_TBNQ         ( 0x1UL <<  7) // (EMAC)
#define EMAC_FBNQ         ( 0x1UL <<  8) // (EMAC)
#define EMAC_FIFO1IDLE    ( 0x1UL <<  9) // (EMAC)
#define EMAC_FIFO2IDLE    ( 0x1UL << 10) // (EMAC)
#define EMAC_FIFO3IDLE    ( 0x1UL << 11) // (EMAC)
#define EMAC_FIFO4IDLE    ( 0x1UL << 12) // (EMAC)
// -------- EMAC_RSR : (EMAC Offset: 0x20) Receive Status Register --------
#define EMAC_DNA          ( 0x1UL <<  0) // (EMAC)
#define EMAC_REC          ( 0x1UL <<  1) // (EMAC)
#define EMAC_RSROVR       ( 0x1UL <<  2) // (EMAC)
#define EMAC_BNA          ( 0x1UL <<  3) // (EMAC)
// -------- EMAC_ISR : (EMAC Offset: 0x24) Interrupt Status Register --------
#define EMAC_INT_DONE     ( 0x1UL <<  0) // (EMAC)
#define EMAC_INT_RCOM     ( 0x1UL <<  1) // (EMAC)
#define EMAC_INT_RBNA     ( 0x1UL <<  2) // (EMAC)
#define EMAC_INT_TOVR     ( 0x1UL <<  3) // (EMAC)
#define EMAC_INT_TUND     ( 0x1UL <<  4) // (EMAC)
#define EMAC_INT_RTRY     ( 0x1UL <<  5) // (EMAC)
#define EMAC_INT_TBRE     ( 0x1UL <<  6) // (EMAC)
#define EMAC_INT_TCOM     ( 0x1UL <<  7) // (EMAC)
#define EMAC_INT_TIDLE    ( 0x1UL <<  8) // (EMAC)
#define EMAC_INT_LINK     ( 0x1UL <<  9) // (EMAC)
#define EMAC_INT_ROVR     ( 0x1UL << 10) // (EMAC)
#define EMAC_INT_HRESP    ( 0x1UL << 11) // (EMAC)
// -------- EMAC_IER : (EMAC Offset: 0x28) Interrupt Enable Register --------
// -------- EMAC_IDR : (EMAC Offset: 0x2c) Interrupt Disable Register --------
// -------- EMAC_IMR : (EMAC Offset: 0x30) Interrupt Mask Register --------
// -------- EMAC_MAN : (EMAC Offset: 0x34) PHY Maintenance Register --------
#define EMAC_DATA         ( 0xFFFFUL << 0) // (EMAC)
#define EMAC_CODE         ( 0x3UL << 16) // (EMAC)
#define EMAC_CODE_802_3   ( 0x2UL << 16) // (EMAC) Write Operation
#define EMAC_REGA         ( 0x1FUL << 18) // (EMAC)
#define EMAC_PHYA         ( 0x1FUL << 23) // (EMAC)
#define EMAC_RW           ( 0x3UL << 28) // (EMAC)
#define EMAC_RW_R         ( 0x2UL << 28) // (EMAC) Read Operation
#define EMAC_RW_W         ( 0x1UL << 28) // (EMAC) Write Operation
#define EMAC_HIGH         ( 0x1UL << 30) // (EMAC)
#define EMAC_LOW          ( 0x1UL << 31) // (EMAC)
// -------- EMAC_RBRP: (EMAC Offset: 0x38) Receive Buffer First full pointer--------
#define EMAC_WRAP_R       ( 0x1UL << 14) // Wrap bit
// -------- EMAC_RBWP: (EMAC Offset: 0x3C) Receive Buffer Current pointer--------
#define EMAC_WRAP_W       ( 0x1UL << 14) // Wrap bit
// -------- EMAC_CFG2: (EMAC Offset: 0x100) Network Configuration Register2 --------
#define EMAC_POWER_UP           ( 0x1UL <<  0)      // (EMAC)
#define EMAC_RMII               ( 0x1UL <<  1)      // (EMAC)
#define EMAC_RMII_12            ( 0x1UL <<  2)      // (EMAC)
#define EMAC_MIU_WRITE_PROTECT  ( 0x1UL <<  6)      // (EMAC)
#define EMAC_MIU_WP_INT_EN      ( 0x1UL <<  7)      // (EMAC)
#define EMAC_SW_RESET_MIU       ( 0x1UL << 12)      // (EMAC)
#define EMAC_SW_RS_EMAC_TO_MIU  ( 0x1UL << 13)      // (EMAC)
#define EMAC_SW_RESET_APB       ( 0x1UL << 14)      // (EMAC)
#define EMAC_SW_RESET_AHB       ( 0x1UL << 15)      // (EMAC)
// -------- EMAC_CFG3: (EMAC Offset: 0x102) Network Configuration Register3 --------
#define EMAC_MASK_ALL_INT       ( 0x1UL <<  0)      // (EMAC)
#define EMAC_FORCE_INT          ( 0x1UL <<  1)      // (EMAC)
#define EMAC_MIU0_2048MB        ( 0x00UL << 6)      // (EMAC)
#define EMAC_MIU0_1024MB        ( 0x01UL << 6)      // (EMAC)
#define EMAC_MIU0_512MB         ( 0x10UL << 6)      // (EMAC)
#define EMAC_MIU0_256MB         ( 0x11UL << 6)      // (EMAC)
// -------- EMAC_CFG3: (EMAC Offset: 0x104) Network Configuration Register3 --------
#define EMAC_INT_DELAY_MODE_EN  ( 0x1UL <<  7)      // (EMAC)
#define EMAC_INT_DELAY_NUMBER_SHIFT 16              // (EMAC)
#define EMAC_INT_DELAY_NUMBER_MASK  0xFF00FFFFUL    // (EMAC)
#define EMAC_INT_DELAY_TIME_SHIFT   24              // (EMAC)
#define EMAC_INT_DELAY_TIME_MASK    0x00FFFFFFUL    // (EMAC)
// -------- EMAC_RX_DELAY_MODE: (EMAC Offset: 0x108) Network Configuration Register --------

// -------- REG_RW_LOW_PRI_TX_DESC_THRESHOLD --------
#define EMAC_RW_TX_DESC_EN_R    ( 0x1UL << 15) // TX_Descriptor Enable Read
#define EMAC_RW_TX_DESC_EN_W    ( 0x1UL << 15) // TX_Descriptor Enable Write
// -------- REG_RO_LOW_PRI_TX_DESC_QUEUED --------
// -------- REG_RO_HIGH_PRI_TX_DESC_QUEUED --------
#define EMAC_RO_TX_DESC_OVERRUN ( 0x1UL << 15) // TX_Descriptor Enable Read
/* ........................................................................ */
#define EMAC_DESC_DONE                  0x00000001UL  /* bit for if DMA is done */
#define EMAC_DESC_WRAP                  0x00000002UL  /* bit for wrap */
#define EMAC_BROADCAST                  0x80000000UL  /* broadcast address */
#define EMAC_MULTICAST                  0x40000000UL  /* multicast address */
#define EMAC_UNICAST                    0x20000000UL  /* unicast address */
#define EMAC_MIU1_ADDR                  0x80000000UL

#define EMAC_TX_DESC_WRAP               (0x1UL << 14) /* bit for wrap */
#define EMAC_DESC_IPV4                  (0x1UL << 11)
#define EMAC_DESC_UDP                   (0x1UL << 18)
#define EMAC_DESC_TCP                   (0x1UL << 19)
#define EMAC_DESC_IP_CSUM               (0x1UL << 20)
#define EMAC_DESC_TCP_UDP_CSUM          (0x1UL << 21)
// Constant: ----------------------------------------------------------------
// Register MAP:
#define REG_ETH_CTL                         0x00000000UL    // Network control register
#define REG_ETH_CFG                         0x00000004UL    // Network configuration register
#define REG_ETH_SR                          0x00000008UL    // Network status register
#define REG_ETH_TAR                         0x0000000CUL    // Transmit address register
#define REG_ETH_TCR                         0x00000010UL    // Transmit control register
#define REG_ETH_TSR                         0x00000014UL    // Transmit status register
#define REG_ETH_RBQP                        0x00000018UL    // Receive buffer queue pointer
#define REG_ETH_BUFF                        0x0000001CUL    // Receive Buffer Configuration
#define REG_ETH_RSR                         0x00000020UL    // Receive status register
#define REG_ETH_ISR                         0x00000024UL    // Interrupt status register
#define REG_ETH_IER                         0x00000028UL    // Interrupt enable register
#define REG_ETH_IDR                         0x0000002CUL    // Interrupt disable register
#define REG_ETH_IMR                         0x00000030UL    // Interrupt mask register
#define REG_ETH_MAN                         0x00000034UL    // PHY maintenance register
#define REG_ETH_BUFFRDPTR                   0x00000038UL    // Receive First Full Pointer
#define REG_ETH_BUFFWRPTR                   0x0000003CUL    // Receive Current pointer
#define REG_ETH_FRA                         0x00000040UL    // Frames transmitted OK
#define REG_ETH_SCOL                        0x00000044UL    // Single collision frames
#define REG_ETH_MCOL                        0x00000048UL    // Multiple collision frames
#define REG_ETH_OK                          0x0000004CUL    // Frames received OK
#define REG_ETH_SEQE                        0x00000050UL    // Frame check sequence errors
#define REG_ETH_ALE                         0x00000054UL    // Alignment errors
#define REG_ETH_DTE                         0x00000058UL    // Deferred transmission frames
#define REG_ETH_LCOL                        0x0000005CUL    // Late collisions
#define REG_ETH_ECOL                        0x00000060UL    // Excessive collisions
#define REG_ETH_TUE                         0x00000064UL    // Transmit under-run errors
#define REG_ETH_CSE                         0x00000068UL    // Carrier sense errors
#define REG_ETH_RE                          0x0000006CUL    // Receive resource error
#define REG_ETH_ROVR                        0x00000070UL    // Received overrun
#define REG_ETH_SE                          0x00000074UL    // Received symbols error
#define REG_ETH_ELR                         0x00000078UL    // Excessive length errors
#define REG_ETH_RJB                         0x0000007CUL    // Receive jabbers
#define REG_ETH_USF                         0x00000080UL    // Undersize frames
#define REG_ETH_SQEE                        0x00000084UL    // SQE test errors
#define REG_ETH_HSL                         0x00000090UL    // Hash register [31:0]
#define REG_ETH_HSH                         0x00000094UL    // Hash register [63:32]
#define REG_ETH_SA1L                        0x00000098UL    // Specific address 1 first 4 bytes
#define REG_ETH_SA1H                        0x0000009CUL    // Specific address 1 last  2 bytes
#define REG_ETH_SA2L                        0x000000A0UL    // Specific address 2 first 4 bytes
#define REG_ETH_SA2H                        0x000000A4UL    // Specific address 2 last  2 bytes
#define REG_ETH_SA3L                        0x000000A8UL    // Specific address 3 first 4 bytes
#define REG_ETH_SA3H                        0x000000ACUL    // Specific address 3 last  2 bytes
#define REG_ETH_SA4L                        0x000000B0UL    // Specific address 4 first 4 bytes
#define REG_ETH_SA4H                        0x000000B4UL    // Specific address 4 last  2 bytes
#define REG_TAG_TYPE                        0x000000B8UL    // tag type of the frame
#define REG_CAMA0_l                         0x00000200UL    // 16 LSB of CAM address  0
#define REG_CAMA0_h                         0x00000204UL    // 32 MSB of CAM address  0
#define REG_CAMA62_l                        0x000003F0UL    // 16 LSB of CAM address 62
#define REG_CAMA62_h                        0x000003F4UL    // 32 MSB of CAM address 62
#define REG_RW32_CFG2                       0x00000100UL
#define REG_RW32_CFG3                       0x00000104UL
#define REG_RW32_RX_DELAY_MODE_STATUS       0x00000108UL
#define REG_RW32_CFG4                       0x00000144UL
#define REG_RW32_LOW_PRI_TX_DESC_BASE       0x0000015EUL
#define REG_RO16_LOW_PRI_TX_DESC_PTR        0x00000162UL
#define REG_RO16_LOW_PRI_TX_DESC_QUEUED     0x00000164UL
#define REG_RW16_LOW_PRI_TX_DESC_THRESHOLD  0x00000166UL
#define REG_RW32_HIGH_PRI_TX_DESC_BASE      0x00000168UL
#define REG_RO16_HIGH_PRI_TX_DESC_PTR       0x0000016CUL
#define REG_RO16_HIGH_PRI_TX_DESC_QUEUED    0x0000016EUL
#define REG_RW16_HIGH_PRI_TX_DESC_THRESHOLD 0x00000170UL
#define REG_WO08_LOW_PRI_TX_DESC_TRANSMIT0  0x00000180UL
#define REG_WO08_LOW_PRI_TX_DESC_TRANSMIT1  0x00000182UL
#define REG_WO08_HIGH_PRI_TX_DESC_TRANSMIT0 0x00000184UL
#define REG_WO08_HIGH_PRI_TX_DESC_TRANSMIT1 0x00000186UL
#ifdef NEW_TX_QUEUE
#define EMAC_REG_NEW_TX_QUEUE           0x00000148
#define EMAC_NEW_TXQ_CNT                ( 0x7F << 0)
#define EMAC_NEW_TXQ_OV                 ( 0x1 << 7)
#define EMAC_NEW_TXQ_THR_OFFSET         8
#define EMAC_NEW_TXQ_THR                ( 0x3F << EMAC_NEW_TXQ_THR_OFFSET)
#define EMAC_NEW_TXQ_EN                 ( 0x1 << 15)
#endif /* NEW_TX_QUEUE */

#define REG_EMAC_JULIAN_0100            0x00000100UL
#define REG_EMAC_JULIAN_0104            0x00000104UL
#define REG_EMAC_JULIAN_0108            0x00000108UL
#define REG_EMAC_JULIAN_0138            0x00000138UL

u16 MHal_EMAC_ReadReg16( u32 bank, u32 reg );
u32 MHal_EMAC_ReadReg32_XIU16( u32 xoffset );
u32 MHal_EMAC_ReadReg32_XIU32( u32 xoffset );
u32 MHal_EMAC_ReadReg32( u32 xoffset );
u32 MHal_EMAC_ReadRam32( phys_addr_t uRamAddr, u32 xoffset);
u32 MHal_EMAC_NegotiationPHY( u8 phyaddr );
u32 MHal_EMAC_CableConnection( u8 phyaddr );
u32 MHal_EMAC_get_SA1H_addr(void);
u32 MHal_EMAC_get_SA1L_addr(void);
u32 MHal_EMAC_get_SA2H_addr(void);
u32 MHal_EMAC_get_SA2L_addr(void);
u32 MHal_EMAC_Read_CTL(void);
u32 MHal_EMAC_Read_CFG(void);
u32 MHal_EMAC_Read_RBQP(void);
u32 MHal_EMAC_Read_ISR(void);
u32 MHal_EMAC_Read_IDR(void);
u32 MHal_EMAC_Read_IER(void);
u32 MHal_EMAC_Read_IMR(void);
u32 MHal_EMAC_Read_RDPTR(void);
u32 MHal_EMAC_Read_BUFF(void);
u32 MHal_EMAC_Read_FRA(void);
u32 MHal_EMAC_Read_SCOL(void);
u32 MHal_EMAC_Read_MCOL(void);
u32 MHal_EMAC_Read_OK(void);
u32 MHal_EMAC_Read_SEQE(void);
u32 MHal_EMAC_Read_ALE(void);
u32 MHal_EMAC_Read_LCOL(void);
u32 MHal_EMAC_Read_ECOL(void);
u32 MHal_EMAC_Read_TUE(void);
u32 MHal_EMAC_Read_TCR( void );
u32 MHal_EMAC_Read_TSR(void);
u32 MHal_EMAC_Read_RSR( void );
u32 MHal_EMAC_Read_CSE(void);
u32 MHal_EMAC_Read_RE(void);
u32 MHal_EMAC_Read_ROVR(void);
u32 MHal_EMAC_Read_SE(void);
u32 MHal_EMAC_Read_ELR(void);
u32 MHal_EMAC_Read_RJB(void);
u32 MHal_EMAC_Read_USF(void);
u32 MHal_EMAC_Read_SQEE(void);
u32 MHal_EMAC_Read_Network_config_register2(void);
u32 MHal_EMAC_Read_Network_config_register3(void);
u32 MHal_EMAC_Read_Delay_interrupt_status(void);
u32 MHal_EMAC_Read_LOW_PRI_TX_DESC_BASE(void);
u16 MHal_EMAC_Read_LOW_PRI_TX_DESC_PTR(void);
u16 MHal_EMAC_Read_LOW_PRI_TX_DESC_QUEUED(void);
u16 MHal_EMAC_Read_LOW_PRI_TX_DESC_THRESHOLD(void);
u32 MHal_EMAC_Read_HIGH_PRI_TX_DESC_BASE(void);
u16 MHal_EMAC_Read_HIGH_PRI_TX_DESC_PTR(void);
u16 MHal_EMAC_Read_HIGH_PRI_TX_DESC_QUEUED(void);
u16 MHal_EMAC_Read_HIGH_PRI_TX_DESC_THRESHOLD(void);
u8  MHal_EMAC_Read_LOW_PRI_TX_DESC_TRANSMIT0(void);
u8  MHal_EMAC_Read_LOW_PRI_TX_DESC_TRANSMIT1(void);
u32 MHal_EMAC_Read_Network_config_register4(void);
u32 MHal_EMAC_Get_Miu_Priority(void);
u32 MHal_EMAC_LOW_PRI_TX_DESC_MODE_OVRN_Get(void);
#ifdef NEW_TX_QUEUE
void MHal_EMAC_New_TX_QUEUE_Enable(void);
u32 MHal_EMAC_New_TX_QUEUE_COUNT_Get(void);
u32 MHal_EMAC_New_TX_QUEUE_OVRN_Get(void);
void MHal_EMAC_New_TX_QUEUE_Threshold_Set(u32 thr);
#endif /* NEW_TX_QUEUE */

void MHal_EMAC_WritReg16( u32 bank, u32 reg, u16 val );
void MHal_EMAC_WritReg32_XIU16( u32 xoffset, u32 xval );
void MHal_EMAC_WritReg32_XIU32( u32 xoffset, u32 xval );
void MHal_EMAC_WritReg32( u32 xoffset, u32 xval );
void MHal_EMAC_update_HSH(u32 mc0, u32 mc1);
void MHal_EMAC_Write_CTL(u32 xval);
void MHal_EMAC_Write_CFG(u32 xval);
void MHal_EMAC_Write_RBQP(u32 u32des);
void MHal_EMAC_Write_BUFF(u32 xval);
void MHal_EMAC_Write_MAN(u32 xval);
void MHal_EMAC_Write_TAR(u32 xval);
void MHal_EMAC_Write_TCR(u32 xval);
void MHal_EMAC_Write_TSR( u32 xval );
void MHal_EMAC_Write_RSR( u32 xval );
void MHal_EMAC_Write_RDPTR(u32 xval);
void MHal_EMAC_Write_WRPTR(u32 xval);
void MHal_EMAC_Write_ISR( u32 xval );
void MHal_EMAC_Write_IER(u32 xval);
void MHal_EMAC_Write_IDR(u32 xval);
void MHal_EMAC_Write_IMR(u32 xval);
void MHal_EMAC_Write_SA1H(u32 xval);
void MHal_EMAC_Write_SA1L(u32 xval);
void MHal_EMAC_Write_SA2H(u32 xval);
void MHal_EMAC_Write_SA2L(u32 xval);
void MHal_EMAC_Write_Network_config_register2(u32 xval);
void MHal_EMAC_Write_Network_config_register3(u32 xval);
void MHal_EMAC_Write_LOW_PRI_TX_DESC_BASE(u32 xval);
void MHal_EMAC_Write_LOW_PRI_TX_DESC_THRESHOLD(u16 xval);
void MHal_EMAC_Write_LOW_PRI_TX_DESC_TRANSMIT0(u8 xval);
void MHal_EMAC_Write_LOW_PRI_TX_DESC_TRANSMIT1(u8 xval);
void MHal_EMAC_Write_HIGH_PRI_TX_DESC_BASE(u32 xval);
void MHal_EMAC_Write_HIGH_PRI_TX_DESC_THRESHOLD(u16 xval);
void MHal_EMAC_Write_HIGH_PRI_TX_DESC_TRANSMIT0(u8 xval);
void MHal_EMAC_Write_HIGH_PRI_TX_DESC_TRANSMIT1(u8 xval);
void MHal_EMAC_Write_Network_config_register4(u32 xval);
void MHal_EMAC_Set_Tx_JULIAN_T(u32 xval);
u32 MHal_EMAC_Get_Tx_FIFO_Threshold(void);
void MHal_EMAC_Set_Rx_FIFO_Enlarge(u32 xval);
u32 MHal_EMAC_Get_Rx_FIFO_Enlarge(void);
void MHal_EMAC_Set_Miu_Priority(u32 xval);
void MHal_EMAC_Set_Tx_Hang_Fix_ECO(u32 xval);
void MHal_EMAC_Set_MIU_Out_Of_Range_Fix(u32 xval);
void MHal_EMAC_Set_Rx_Tx_Burst16_Mode(u32 xval);
void MHal_EMAC_Set_Tx_Rx_Req_Priority_Switch(u32 xval);
void MHal_EMAC_Set_Rx_Byte_Align_Offset(u32 xval);
void MHal_EMAC_Write_Protect(u32 start_addr, u32 length);
void MHal_EMAC_HW_init(void);
void MHal_EMAC_Power_On_Clk( void );
void MHal_EMAC_Power_Off_Clk(void);
void MHal_EMAC_timer_callback(u32 value);
void MHal_EMAC_WritRam32(phys_addr_t uRamAddr, u32 xoffset,u32 xval);
void MHal_EMAC_enable_mdi(void);
void MHal_EMAC_disable_mdi(void);
void MHal_EMAC_write_phy (u8 phy_addr, u8 address, u32 value);
void MHal_EMAC_read_phy(u8 phy_addr, u8 address,u32 *value);
void MHal_EMAC_enable_phyirq (void);
void MHal_EMAC_disable_phyirq (void);
void MHal_EMAC_update_speed_duplex(u32 uspeed, u32 uduplex);
void MHal_EMAC_Write_SA1_MAC_Address(u8 m0,u8 m1,u8 m2,u8 m3,u8 m4,u8 m5);
void MHal_EMAC_Write_SA2_MAC_Address(u8 m0,u8 m1,u8 m2,u8 m3,u8 m4,u8 m5);
void MHal_EMAC_Write_SA3_MAC_Address(u8 m0,u8 m1,u8 m2,u8 m3,u8 m4,u8 m5);
void MHal_EMAC_Write_SA4_MAC_Address(u8 m0,u8 m1,u8 m2,u8 m3,u8 m4,u8 m5);
void MHal_EMAC_Force_10(void);
#endif
// -----------------------------------------------------------------------------
// Linux EMAC.c End
// -----------------------------------------------------------------------------
