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
//#define RX_SOFTWARE_DESCRIPTOR
#define SOFTWARE_DESCRIPTOR
#define RX_CHECKSUM
#define INT_JULIAN_D
#define CHIP_FLUSH_READ
#define TX_QUEUE_4
//#define ISR_BOTTOM_HALF
#define EMAC_TX_COUNT       (14) //offset of TX counter in TSR

#ifdef TX_QUEUE_4
#define EMAC_INT_MASK       (0xdffUL)
#else
#define EMAC_INT_MASK       (0xdffUL)
#endif

#ifdef RX_SOFTWARE_DESCRIPTOR
//#define RX_BYTE_ALIGN_OFFSET
#endif
// Compiler Switches
#define REG_BIT_MAP
#define URANUS_ETHER_ADDR_CONFIGURABLE  /* MAC address can be changed? */
//--------------------------------------------------------------------------------------------------
//  Constant definition
//--------------------------------------------------------------------------------------------------
#define TRUE                                1
#define FALSE                               0

#define SOFTWARE_DESCRIPTOR_ENABLE          0x0001UL
#define CHECKSUM_ENABLE                     0x0FEUL
#define RX_CHECKSUM_ENABLE                  0x000EUL
#define CONFIG_EMAC_MOA                     1   // System Type
#define EMAC_SPEED_100                      100

#if defined(CONFIG_ARM)
#define  mstar_pm_base                      0xFD000000
#endif

#define EMAC_ALLFF                          0xFFFFFFFFUL
#define EMAC_ABSO_MEM_BASE                  0xA0000000UL//EMAC_ABSO_MEM_BASE                  0xA0000000
#define INTERNEL_PHY_REG_BASE               mstar_pm_base + (0x3100UL<<1)
#define EMAC_RIU_REG_BASE                   mstar_pm_base
#define EMAC_ABSO_PHY_BASE                  0x80000000UL//EMAC_ABSO_MEM_BASE
#define EMAC_ABSO_MEM_SIZE                  0x30000UL//0x16000//0x180000//0x16000//(48 * 1024)     // More than: (32 + 16(0x3FFF)) KB
#define EMAC_MEM_SIZE_SQU                   4    // More than: (32 + 16(0x3FFF)) KB
#define EMAC_BUFFER_MEM_SIZE                0x0004000UL
#define EMAC_MAX_TX_QUEUE                   30
// Base address here:
#define MIU0_BUS_BASE                       MSTAR_MIU0_BUS_BASE
#define REG_ADDR_BASE                       mstar_pm_base + (0x122000UL<<1)  // The register address base. Depends on system define.

#ifdef CONFIG_MSTAR_EMAC_NAPI
#define RBQP_LENG                           0xd0//0x0100  0x40//                // ==?descriptors
#else
#define RBQP_LENG                           0x40//0x0100  0x40//                // ==?descriptors
#endif

#define MAX_RX_DESCR                        RBQP_LENG//32   /* max number of receive buffers */

#ifdef CONFIG_MSTAR_EMAC_NAPI
#define SOFTWARE_DESC_LEN                   0x1000UL
#else
#define SOFTWARE_DESC_LEN                   0x800UL
#endif

#ifdef SOFTWARE_DESCRIPTOR
#define RX_BUFFER_SEL                       0x0001UL          // 0x0=2KB,0x1=4KB,0x2=8KB,0x3=16KB, 0x09=1MB
#define RX_BUFFER_SIZE                      (RBQP_LENG*SOFTWARE_DESC_LEN)  //0x10000//0x20000//
#else
#define RX_BUFFER_SEL                       0x0003UL          // 0x0=2KB,0x1=4KB,0x2=8KB,0x3=16KB, 0x09=1MB
#define RX_BUFFER_SIZE                      (0x2000UL<<RX_BUFFER_SEL)//0x10000//0x20000//
#endif
#define RBQP_SIZE                           (0x0008UL*RBQP_LENG)      // ==8bytes*?==?descriptors

#define MAX_INT_COUNTER                     100

#ifdef CONFIG_ETHERNET_ALBANY
    #define JULIAN_100_VAL                      (0x0000F011UL)
#else
    #define JULIAN_100_VAL                      (0x0000F017UL)
#endif

#ifdef CONFIG_MSTAR_EMAC_NAPI
#define JULIAN_104_VAL                      (0x04010080UL)
#else
#define JULIAN_104_VAL                      (0x04020080UL)
#endif

#define TX_CHECKSUM_ENABLE                  (0x00000470UL)
//PHY Register
#define PHY_REG_BASIC                       (0)
#define PHY_REG_STATUS                      (1)
//PHY setting
#define PHY_REGADDR_OFFSET                  (18)
#define PHY_ADDR_OFFSET                     (23)

//-------------------------------------------------------------------------------------------------
//  Bit Define
//-------------------------------------------------------------------------------------------------
#define EMAC_MIU_RW       (0x3UL <<  10)   //EMAC power on clk

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
#define EMAC_LEN          ( 0x7FFUL <<  0) // (EMAC)
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
#define EMAC_FIFO2IDLE    ( 0x1UL <<  10) // (EMAC)
#define EMAC_FIFO3IDLE    ( 0x1UL <<  11) // (EMAC)
#define EMAC_FIFO4IDLE    ( 0x1UL <<  12) // (EMAC)
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
#define EMAC_DATA         ( 0xFFFFUL <<  0) // (EMAC)
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
// ........................................................................ //
//URANUS PHY //
#define MII_URANUS_ID                   0x01111UL//Test value
//URANUS specific registers //
#define MII_USCR_REG                    16
#define MII_USCSR_REG                   17
#define MII_USINTR_REG                  21
/* ........................................................................ */
#define EMAC_DESC_DONE                  0x00000001UL  /* bit for if DMA is done */
#define EMAC_DESC_WRAP                  0x00000002UL  /* bit for wrap */
#define EMAC_BROADCAST                  0x80000000UL  /* broadcast address */
#define EMAC_MULTICAST                  0x40000000UL  /* multicast address */
#define EMAC_UNICAST                    0x20000000UL  /* unicast address */

#define EMAC_DESC_IPV4                  (0x1UL << 11)
#define EMAC_DESC_UDP                   (0x1UL << 18)
#define EMAC_DESC_TCP                   (0x1UL << 19)
#define EMAC_DESC_IP_CSUM               (0x1UL << 20)
#define EMAC_DESC_TCP_UDP_CSUM          (0x1UL << 21)
// Constant: ----------------------------------------------------------------
// Register MAP:
#define REG_ETH_CTL                     0x00000000UL         // Network control register
#define REG_ETH_CFG                     0x00000004UL         // Network configuration register
#define REG_ETH_SR                      0x00000008UL         // Network status register
#define REG_ETH_TAR                     0x0000000CUL         // Transmit address register
#define REG_ETH_TCR                     0x00000010UL         // Transmit control register
#define REG_ETH_TSR                     0x00000014UL         // Transmit status register
#define REG_ETH_RBQP                    0x00000018UL         // Receive buffer queue pointer
#define REG_ETH_BUFF                    0x0000001CUL         // Receive Buffer Configuration
#define REG_ETH_RSR                     0x00000020UL         // Receive status register
#define REG_ETH_ISR                     0x00000024UL         // Interrupt status register
#define REG_ETH_IER                     0x00000028UL         // Interrupt enable register
#define REG_ETH_IDR                     0x0000002CUL         // Interrupt disable register
#define REG_ETH_IMR                     0x00000030UL         // Interrupt mask register
#define REG_ETH_MAN                     0x00000034UL         // PHY maintenance register
#define REG_ETH_BUFFRDPTR               0x00000038UL         // Receive First Full Pointer
#define REG_ETH_BUFFWRPTR               0x0000003CUL         // Receive Current pointer
#define REG_ETH_FRA                     0x00000040UL         // Frames transmitted OK
#define REG_ETH_SCOL                    0x00000044UL         // Single collision frames
#define REG_ETH_MCOL                    0x00000048UL         // Multiple collision frames
#define REG_ETH_OK                      0x0000004CUL         // Frames received OK
#define REG_ETH_SEQE                    0x00000050UL         // Frame check sequence errors
#define REG_ETH_ALE                     0x00000054UL         // Alignment errors
#define REG_ETH_DTE                     0x00000058UL         // Deferred transmission frames
#define REG_ETH_LCOL                    0x0000005CUL         // Late collisions
#define REG_ETH_ECOL                    0x00000060UL         // Excessive collisions
#define REG_ETH_TUE                     0x00000064UL         // Transmit under-run errors
#define REG_ETH_CSE                     0x00000068UL         // Carrier sense errors
#define REG_ETH_RE                      0x0000006CUL         // Receive resource error
#define REG_ETH_ROVR                    0x00000070UL         // Received overrun
#define REG_ETH_SE                      0x00000074UL         // Received symbols error
#define REG_ETH_ELR                     0x00000078UL         // Excessive length errors
#define REG_ETH_RJB                     0x0000007CUL         // Receive jabbers
#define REG_ETH_USF                     0x00000080UL         // Undersize frames
#define REG_ETH_SQEE                    0x00000084UL         // SQE test errors
#define REG_ETH_HSL                     0x00000090UL         // Hash register [31:0]
#define REG_ETH_HSH                     0x00000094UL         // Hash register [63:32]
#define REG_ETH_SA1L                    0x00000098UL         // Specific address 1 first 4 bytes
#define REG_ETH_SA1H                    0x0000009CUL         // Specific address 1 last  2 bytes
#define REG_ETH_SA2L                    0x000000A0UL         // Specific address 2 first 4 bytes
#define REG_ETH_SA2H                    0x000000A4UL         // Specific address 2 last  2 bytes
#define REG_ETH_SA3L                    0x000000A8UL         // Specific address 3 first 4 bytes
#define REG_ETH_SA3H                    0x000000ACUL         // Specific address 3 last  2 bytes
#define REG_ETH_SA4L                    0x000000B0UL         // Specific address 4 first 4 bytes
#define REG_ETH_SA4H                    0x000000B4UL         // Specific address 4 last  2 bytes
#define REG_TAG_TYPE                    0x000000B8UL         // tag type of the frame
#define REG_CAMA0_l                     0x00000200UL         // 16 LSB of CAM address  0
#define REG_CAMA0_h                     0x00000204UL         // 32 MSB of CAM address  0
#define REG_CAMA62_l                    0x000003F0UL         // 16 LSB of CAM address 62
#define REG_CAMA62_h                    0x000003F4UL         // 32 MSB of CAM address 62

#define REG_EMAC_JULIAN_0100            0x00000100UL
#define REG_EMAC_JULIAN_0104            0x00000104UL
#define REG_EMAC_JULIAN_0108            0x00000108UL
#define REG_EMAC_JULIAN_0138            0x00000138UL

u32 MHal_EMAC_ReadReg32( u32 xoffset );
u32 MHal_EMAC_ReadRam32( phys_addr_t uRamAddr, u32 xoffset);
u32 MHal_EMAC_NegotiationPHY(void);
u32 MHal_EMAC_CableConnection(void);
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
u32 MHal_EMAC_Read_JULIAN_0100(void);
u32 MHal_EMAC_Read_JULIAN_0104(void);
u32 MHal_EMAC_Read_JULIAN_0108(void);

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
void MHal_EMAC_Write_JULIAN_0100(u32 xval);
void MHal_EMAC_Write_JULIAN_0104(u32 xval);
void MHal_EMAC_Write_JULIAN_0108(u32 xval);
void MHal_EMAC_Set_Tx_JULIAN_T(u32 xval);
u32 MHal_EMAC_Get_Tx_FIFO_Threshold(void);
void MHal_EMAC_Set_Rx_FIFO_Enlarge(u32 xval);
u32 MHal_EMAC_Get_Rx_FIFO_Enlarge(void);
void MHal_EMAC_Set_Miu_Priority(u32 xval);
u32 MHal_EMAC_Get_Miu_Priority(void);
void MHal_EMAC_Set_Tx_Hang_Fix_ECO(u32 xval);
void MHal_EMAC_Set_MIU_Out_Of_Range_Fix(u32 xval);
void MHal_EMAC_Set_Rx_Tx_Burst16_Mode(u32 xval);
void MHal_EMAC_Set_Tx_Rx_Req_Priority_Switch(u32 xval);
void MHal_EMAC_Set_Rx_Byte_Align_Offset(u32 xval);
void MHal_EMAC_Write_Protect(u32 start_addr, u32 length);
void MHal_EMAC_HW_init(void);
void MHal_EMAC_Power_On_Clk( void );
void MHal_EMAC_Power_Off_Clk(void);
void MHal_EMAC_timer_callback(unsigned long value);
void MHal_EMAC_WritRam32(phys_addr_t uRamAddr, u32 xoffset,u32 xval);
void MHal_EMAC_enable_mdi(void);
void MHal_EMAC_disable_mdi(void);
void MHal_EMAC_write_phy (unsigned char phy_addr, unsigned char address, u32 value);
void MHal_EMAC_read_phy(unsigned char phy_addr, unsigned char address,u32 *value);
void MHal_EMAC_enable_phyirq (void);
void MHal_EMAC_disable_phyirq (void);
void MHal_EMAC_update_speed_duplex(u32 uspeed, u32 uduplex);
void MHal_EMAC_Write_SA1_MAC_Address(u8 m0,u8 m1,u8 m2,u8 m3,u8 m4,u8 m5);
void MHal_EMAC_Write_SA2_MAC_Address(u8 m0,u8 m1,u8 m2,u8 m3,u8 m4,u8 m5);
void MHal_EMAC_Write_SA3_MAC_Address(u8 m0,u8 m1,u8 m2,u8 m3,u8 m4,u8 m5);
void MHal_EMAC_Write_SA4_MAC_Address(u8 m0,u8 m1,u8 m2,u8 m3,u8 m4,u8 m5);
#endif
// -----------------------------------------------------------------------------
// Linux EMAC.c End
// -----------------------------------------------------------------------------


