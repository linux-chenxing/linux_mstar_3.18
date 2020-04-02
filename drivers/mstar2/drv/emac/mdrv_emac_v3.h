////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   EMAC.h
/// @author MStar Semiconductor Inc.
/// @brief  EMAC Driver Interface
///////////////////////////////////////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------------
// Linux EMAC.h define start
// -----------------------------------------------------------------------------

#ifndef __DRV_EMAC_H_
#define __DRV_EMAC_H_

#define EMAC_DBG(fmt, args...)              {printk(KERN_EMERG"Mstar_emac: "); printk(fmt, ##args);}
#define EMAC_INFO                           {printk("Line:%u\n", __LINE__);}
#define EMAC_DRVNAME                        "mstar emac"
#define EMAC_DRV_VERSION                    "3.0.0"
#define EMAC_TEST_STRING_LEN                0
//-------------------------------------------------------------------------------------------------
//  Define Enable or Compiler Switches
//-------------------------------------------------------------------------------------------------
#define EMAC_MTU                            (1518)
//--------------------------------------------------------------------------------------------------
//  Constant definition
//--------------------------------------------------------------------------------------------------
#define EP_FLAG_OPEND                       0X00000001UL
#define EP_FLAG_SUSPENDING                  0X00000002UL
#define EP_FLAG_SUSPENDING_OPEND            0X00000004UL

#define ETHERNET_TEST_NO_LINK               0x00000000UL
#define ETHERNET_TEST_AUTO_NEGOTIATION      0x00000001UL
#define ETHERNET_TEST_LINK_SUCCESS          0x00000002UL
#define ETHERNET_TEST_RESET_STATE           0x00000003UL
#define ETHERNET_TEST_SPEED_100M            0x00000004UL
#define ETHERNET_TEST_DUPLEX_FULL           0x00000008UL
#define ETHERNET_TEST_INIT_FAIL             0x00000010UL

#define EMAC_RX_TMR                         (0)
#define EMAC_LINK_TMR                       (1)

#define EMAC_CHECK_LINK_TIME                (HZ)
#define EMAC_CHECK_CNT                      (500000)
#define RTL_8210                            (0x1CUL)
//--------------------------------------------------------------------------------------------------
//  Global variable
//--------------------------------------------------------------------------------------------------
u8 MY_MAC[6] = { 0x00UL, 0x75UL, 0x90UL, 0x00UL, 0x00UL, 0x01UL };
//-------------------------------------------------------------------------------------------------
//  Data structure
//-------------------------------------------------------------------------------------------------
#define EMAC_STATS_STRING_LEN 23

static char mstar_emac_stat_string[EMAC_STATS_STRING_LEN][ETH_GSTRING_LEN]=
{
    {"rx_packets"},
    {"rx_bytes"},
    {"rx_errors"},
    {"rx_dropped"},
    {"rx_length_errors"},
    {"rx_over_errors"},
    {"rx_crc_errors"},
    {"rx_frame_errors"},
    {"rx_fifo_errors"},
    {"rx_missed_errors"},
    {"rx_compressed"},
    {"tx_packets"},
    {"tx_bytes"},
    {"tx_errors"},
    {"tx_dropped"},
    {"tx_aborted_errors"},
    {"tx_carrier_errors"},
    {"tx_fifo_errors"},
    {"tx_heartbeat_errors"},
    {"tx_window_errors"},
    {"tx_compressed"},
    {"multicast"},
    {"collisions"}
};
#ifdef EMAC_10T_RANDOM_WAVEFORM
#define EMAC_PRIV_FLAGS_STRING_LEN      1
#define EMAC_PRIVATE_FLAG_10T_RANDOM    0x1UL

static char mstar_emac_private_flag_string[EMAC_PRIV_FLAGS_STRING_LEN][ETH_GSTRING_LEN]=
{
    {"10T_Random"},
};
#else
#define EMAC_PRIV_FLAGS_STRING_LEN      0
#endif


#ifdef TX_DESC_MODE
struct tx_descriptor
{
  u32   addr;
  u32   low_tag;
  u32   reserve0;
  u32   reserve1;
};
#endif

#ifdef TX_SOFTWARE_QUEUE
struct tx_ring
{
    u8 used;
    struct sk_buff *skb;        /* holds skb until xmit interrupt completes */
    dma_addr_t skb_physaddr;    /* phys addr from pci_map_single */
};
#endif

#ifdef RX_DESC_MODE
#ifdef EMAC_GMAC_DESCRIPTOR
struct rx_descriptor
{
  u32   addr;
  u32   low_tag;
  u32   high_tag;
  u32   reserve;
};
#else
struct rx_descriptor
{
  u32   addr;
  u32   size;
};
#endif
#endif

struct _BasicConfigEMAC
{
    u8 connected;           // 0:No, 1:Yes    <== (20070515) Wait for Julian's reply
    u8 speed;               // 10:10Mbps, 100:100Mbps
    // ETH_CTL Register:
    u8 wes;                 // 0:Disable, 1:Enable (WR_ENABLE_STATISTICS_REGS)
    // ETH_CFG Register:
    u8 duplex;              // 1:Half-duplex, 2:Full-duplex
    u8 cam;                 // 0:No CAM, 1:Yes
    u8 rcv_bcast;           // 0:No, 1:Yes
    u8 rlf;                 // 0:No, 1:Yes receive long frame(1522)
    // MAC Address:
    u8 sa1[6];              // Specific Addr 1 (MAC Address)
    u8 sa2[6];              // Specific Addr 2
    u8 sa3[6];              // Specific Addr 3
    u8 sa4[6];              // Specific Addr 4
};
typedef struct _BasicConfigEMAC BasicConfigEMAC;

struct _UtilityVarsEMAC
{
    u32 cntChkINTCounter;
    u32 readIdxRBQP;        // Reset = 0x00000000
    u32 rxOneFrameAddr;     // Reset = 0x00000000 (Store the Addr of "ReadONE_RX_Frame")
    // Statistics Counters : (accumulated)
    u32 cntREG_ETH_FRA;
    u32 cntREG_ETH_SCOL;
    u32 cntREG_ETH_MCOL;
    u32 cntREG_ETH_OK;
    u32 cntREG_ETH_SEQE;
    u32 cntREG_ETH_ALE;
    u32 cntREG_ETH_DTE;
    u32 cntREG_ETH_LCOL;
    u32 cntREG_ETH_ECOL;
    u32 cntREG_ETH_TUE;
    u32 cntREG_ETH_CSE;
    u32 cntREG_ETH_RE;
    u32 cntREG_ETH_ROVR;
    u32 cntREG_ETH_SE;
    u32 cntREG_ETH_ELR;
    u32 cntREG_ETH_RJB;
    u32 cntREG_ETH_USF;
    u32 cntREG_ETH_SQEE;
    // Interrupt Counter :
    u32 cntHRESP;           // Reset = 0x0000
    u32 cntROVR;            // Reset = 0x0000
    u32 cntLINK;            // Reset = 0x0000
    u32 cntTIDLE;           // Reset = 0x0000
    u32 cntTCOM;            // Reset = 0x0000
    u32 cntTBRE;            // Reset = 0x0000
    u32 cntRTRY;            // Reset = 0x0000
    u32 cntTUND;            // Reset = 0x0000
    u32 cntTOVR;            // Reset = 0x0000
    u32 cntRBNA;            // Reset = 0x0000
    u32 cntRCOM;            // Reset = 0x0000
    u32 cntDONE;            // Reset = 0x0000
    // Flags:
    u8  flagMacTxPermit;    // 0:No,1:Permitted.  Initialize as "permitted"
    u8  flagISR_INT_RCOM;
    u8  flagISR_INT_RBNA;
    u8  flagISR_INT_DONE;
    u8  flagPowerOn;        // 0:Poweroff, 1:Poweron
    u8  initedEMAC;         // 0:Not initialized, 1:Initialized.
    u8  flagRBNA;
    // Misc Counter:
    u32 cntRxFrames;        // Reset = 0x00000000 (Counter of RX frames,no matter it's me or not)
    u32 cntReadONE_RX;      // Counter for ReadONE_RX_Frame
    u32 cntCase20070806;
    u32 cntChkToTransmit;
    // Misc Variables:
    u32 mainThreadTasks;    // (20071029_CHARLES) b0=Poweroff,b1=Poweron
};
typedef struct _UtilityVarsEMAC UtilityVarsEMAC;

struct EMAC_private
{
    struct net_device *dev;
    struct net_device_stats stats;
    struct mii_if_info mii;             /* ethtool support */
    struct timer_list Link_timer;
#ifdef TX_ZERO_COPY
    struct timer_list TX_free_timer;
#endif
    u32 PreLinkStatus;
    u32 initstate;
    u32 msglvl;
    u32 private_flag;
    struct task_struct *ewave_task;
    BasicConfigEMAC ThisBCE;
    UtilityVarsEMAC ThisUVE;
    /* Memory */
    phys_addr_t     RAM_VA_BASE;
    phys_addr_t     RAM_PA_BASE;
    phys_addr_t     RAM_VA_PA_OFFSET;
    phys_addr_t     RX_DESC_BASE;
#ifndef RX_ZERO_COPY
    phys_addr_t     RX_BUFFER_BASE;
#endif
#ifdef TX_DESC_MODE
    phys_addr_t     TX_LP_DESC_BASE;
    phys_addr_t     TX_HP_DESC_BASE;
#endif
#ifndef TX_ZERO_COPY
    phys_addr_t     TX_BUFFER_BASE;
#endif
    /* PHY */
    u8  phyaddr;
    u32 phy_status_register;
    u32 phy_type;             /* type of PHY (PHY_ID) */
    spinlock_t irq_lock;                /* lock for MDI interface */
    spinlock_t tx_lock;                 /* lock for tx */
    spinlock_t rx_lock;                 /* lock for rx */
    short phy_media;                    /* media interface type */
    /* Transmit */
    u32 tx_index;
    u32 tx_ring_entry_number;
#ifdef TX_DESC_MODE
    struct tx_descriptor *tx_desc_list;
    struct sk_buff *tx_desc_sk_buff_list[TX_LOW_PRI_DESC_NUMBER];
    u32 tx_desc_write_index;
    u32 tx_desc_read_index;
    u32 tx_desc_queued_number;
    u32 tx_desc_count;
    u32 tx_desc_full_count;
	u32 tx_desc_start_free;
#endif
#ifdef TX_SOFTWARE_QUEUE
    struct tx_ring tx_swq[TX_SW_QUEUE_SIZE];
    unsigned int tx_rdidx;              /* TX_SW_QUEUE read to hw index */
    unsigned int tx_wridx;              /* TX_SW_QUEUE write index */
    unsigned int tx_clidx;              /* TX_SW_QUEUE clear index */

    unsigned int tx_rdwrp;              /* TX_SW_QUEUE read to hw index wrap*/
    unsigned int tx_wrwrp;              /* TX_SW_QUEUE write index wrap*/
    unsigned int tx_clwrp;              /* TX_SW_QUEUE clear index wrap */
    unsigned int tx_swq_full_cnt;       /* TX_SW_QUEUE full stopped count*/

    unsigned int irqcnt;
    unsigned int tx_irqcnt;
#endif
#ifdef TX_NAPI
    struct napi_struct napi_tx;
#endif
    /* Receive */
    u32 ROVR_count;
    u32 full_budge_count;
    u32 polling_count;
    u32 max_polling;
    u32 rx_ring_entry_number;
#ifdef RX_DESC_MODE
    struct rx_descriptor *rx_desc_list;
    u32 rx_desc_read_index;
#ifdef RX_ZERO_COPY
    struct sk_buff *rx_desc_sk_buff_list[RX_DESC_NUMBER];
    u32 rx_desc_free_index;
    u32 rx_desc_free_number;
#endif
#endif
#ifdef RX_NAPI
    struct napi_struct napi_rx;
#endif
    /* suspend/resume */
    u32 ep_flag;
};

#endif
// -----------------------------------------------------------------------------
// Linux EMAC.h End
// -----------------------------------------------------------------------------
