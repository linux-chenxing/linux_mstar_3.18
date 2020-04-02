/**********************************************************************
 * Copyright (C) 2014 Cadence Design Systems
 * All rights reserved.
 **********************************************************************
 * WARNING: This file is auto-generated using api-generator utility.
 *          Do not edit it manually.
 **********************************************************************/

#ifndef _CPDI_H_
#define _CPDI_H_

#include "cdn_errno.h"
#include "cdn_stdint.h"
#include "mstar_pcie_addrmap.h"

/** @defgroup ConfigInfo  Configuration and Hardware Operation Information
 *  The following definitions specify the driver operation environment that
 *  is defined by hardware configuration or client code. These defines are
 *  located in the header file of the core driver.
 *  @{
 */

/**********************************************************************
* Defines
**********************************************************************/
/** Number of AXI regions (excluding Region-0) */
#define	CPDI_AXI_NB_REGIONS 32

/** Size of one AXI Region (not Region 0) */
#define	CPDI_AXI_REGION_SIZE (1ULL <<20)

/** Overall size of AXI area */
#define	CPDI_AXI_OVERALL_SIZE (64 * (1ULL <<20))

/** Size of Region 0, equal to sum of sizes of other regions */
#define	CPDI_AXI_REGION_0_SIZE (1* (1ULL << 28))

/** Size of Region 1, equal to sum of sizes of other regions */
#define	CPDI_AXI_REGION_1_SIZE (1* (1ULL << 20))
/**
 * The maximum number of regions available
 * when using the AXI Interface
*/
#define	CPDI_MAX_AXI_WRAPPER_REGION_NUM 33

/**
 * Default address for the AXI bus. This value must be
 * overwritten during initialization if the default address
 * does not match the base address on the target system.
*/
#define	CPDI_DEFAULT_ECAM_BASE_BUS_ADDRESS 0xF0000000

/** Max ECAM bus number */
#define	CPDI_ECAM_MAX_BUS 255

/** Max ECAM device number */
#define	CPDI_ECAM_MAX_DEV 31

/** Max ECAM function number */
#define	CPDI_ECAM_MAX_FUNC 7

/** Max ECAM offset */
#define	CPDI_ECAM_MAX_OFFSET 4095

/** Max ECAM function number for ARI supported devices. */
#define	CPDI_ARI_ECAM_MAX_FUNC 255

/**
 * Defines the minimum number of address bits passed down from the
 * AXI Interface
*/
#define	CPDI_MIN_AXI_ADDR_BITS_PASSED 8

/**
 * Defines the default local vendor and device id for PFO. If the Device ID is
 * changed in RTL during hardware configuration, this define should be updated
 * to match, or the probe() API will fail.  The Device ID may also be set at
 * driver intialization time for each Function, in which case this define should
 * be left unchanged.
*/
#define	CPDI_DEFAULT_VENDOR_DEVICE_ID 0x100dedb

/**
 * Defines the maximum number of BARS available in functions (both Physical
 * and Virtual).
*/
#define	CPDI_MAX_BARS_IN_FUNC 6

/** Defines the maximum number of BARs in the RP */
#define	CPDI_MAX_BARS_IN_RP 3

/** Offset for RP management registers */
#define	CPDI_REGS_OFFSET_RP_MANAGEMENT 0x2000

/** Default offset for local management registers */
#define	CPDI_REGS_OFFSET_LOCAL_MANAGEMENT 0x1000

/** Offset for axi wrapper regs */
#define	CPDI_REG_OFFSET_AXI_WRAPPER 0x4000

/** Number of PCI physical function supported. */
#define	CPDI_NUM_PCI_FUNCTIONS 1

/** Total number of Traffic class supported. */
#define	CPDI_MAX_TRAFFIC_CLASS_NUM 8

/**
 *  @}
 */


/** @defgroup DataStructure Dynamic Data Structures
 *  This section defines the data structures used by the driver to provide
 *  hardware information, modification and dynamic operation of the driver.
 *  These data structures are defined in the header file of the core driver
 *  and utilized by the API.
 *  @{
 */

/**********************************************************************
 * Forward declarations
 **********************************************************************/
struct CPDI_UdmaConfiguration;
struct CPDI_InitValue;
struct CPDI_InitCreditRelatedParams;
struct CPDI_InitCountRelatedParams;
struct CPDI_InitTimingRelatedParams;
struct CPDI_InitIdRelatedParams;
struct CPDI_TargetLaneMapRelatedParams;
struct CPDI_InitDebugRelatedParams;
struct CPDI_InitMaskableConditions;
struct CPDI_InitRpBarParams;
struct CPDI_InitBaseAddresses;
struct CPDI_InitParam;
struct CPDI_SysReq;

/**********************************************************************
 * Enumerations
 **********************************************************************/
/** Enumeration of the LTSSM (link training and status state machine) States */
typedef enum
{
    /**
    * @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0
    * @details Transmitter is in an Electrical Idle state
    */
    CPDI_DETECT_QUIET = 0,
    /**
    * @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0
    * @details Transmitter performs Receive Detection of unconfigured lanes
    */
    CPDI_DETECT_ACTIVE = 1,
    /**
    * @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0
    * @details Transmitter sends TS1 Ordered Sets on all lanes that Detected a Receiver
    */
    CPDI_POLLING_ACTIVE = 2,
    /**
    * @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0
    * @details Transmitter sends Compliance patterns at certain data-rates and
    * de-emphasis levels
    */
    CPDI_POLLING_COMPLIANCE = 3,
    /**
    * @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0
    * @details Transmitter sends Configuration information in TS2 Ordered Sets
    */
    CPDI_POLLING_CONFIGURATION = 4,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_CONFIGURATION_LINKWIDTH_START = 5,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_CONFIGURATION_LINKWIDTH_ACCEPT = 6,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_CONFIGURATION_LANENUM_ACCEPT = 7,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_CONFIGURATION_LANENUM_WAIT = 8,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_CONFIGURATION_COMPLETE = 9,
    /**
    * @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0
    * @details Link fully configured, bit-lock and symbol-lock achieved
    */
    CPDI_CONFIGURATION_IDLE = 10,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_RECOVERY_RCVRLOCK = 11,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_RECOVERY_SPEED = 12,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_RECOVERY_RCVRCFG = 13,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_RECOVERY_IDLE = 14,
    /**
    * @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0
    * Normal operational state - full ON state
    */
    CPDI_L0 = 16,
    /**
    * @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0
    * Electrical idle state
    */
    CPDI_RX_L0S_ENTRY = 17,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_RX_L0S_IDLE = 18,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_RX_L0S_FTS = 19,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_TX_L0S_ENTRY = 20,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_TX_L0S_IDLE = 21,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_TX_L0S_FTS = 22,
    /**
    * @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0
    * @details Transmitter sends an Electrical Idle Ordered Set.
    * Links enters Electrical Idle
    */
    CPDI_L1_ENTRY = 23,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_L1_IDLE = 24,
    /**
    * @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0
    * @details Receiver in Low impedance state.Transmitter in Electrical
    * Idle for a min of 50UI
    */
    CPDI_L2_IDLE = 25,
    /**
    * @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0
    * @details Transmit Beacon signal in direction of Root Port
    */
    CPDI_L2_TRANSMITWAKE = 26,
    /**
    * @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0
    * @details Link is off and no DC common mode voltage driven
    */
    CPDI_DISABLED = 32,
    /**
    * @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0
    * @details Loopback master continuously transmits TS1 ordered sets
    */
    CPDI_LOOPBACK_ENTRY_MASTER = 33,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_LOOPBACK_ACTIVE_MASTER = 34,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_LOOPBACK_EXIT_MASTER = 35,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_LOOPBACK_ENTRY_SLAVE = 36,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_LOOPBACK_ACTIVE_SLAVE = 37,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_LOOPBACK_EXIT_SLAVE = 38,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 @details TS1s with Hot Reset bit set are exchanged */
    CPDI_HOT_RESET = 39,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_RECOVERY_EQUALIZATION_PHASE_0 = 40,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_RECOVERY_EQUALIZATION_PHASE_1 = 41,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_RECOVERY_EQUALIZATION_PHASE_2 = 42,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    CPDI_RECOVERY_EQUALIZATION_PHASE_3 = 43,
} CPDI_LtssmState;

/** Debug Mux Module Select */
typedef enum
{
    /** @brief Select Mux for debug of Physical Layer 0 LTSSM */
    CPDI_DEBUG_PHYSICAL_LAYER_LTSSM_0 = 0,
    /** @brief Select Mux for debug of Physical Layer 1 LTSSM */
    CPDI_DEBUG_PHYSICAL_LAYER_LTSSM_1 = 1,
    /** @brief Select Mux for debug of TX DLL */
    CPDI_DEBUG_DATA_LINK_LAYER_TX_SIDE = 4,
    /** @brief Select Mux for debug of RX DLL */
    CPDI_DEBUG_DATA_LINK_LAYER_RX_SIDE = 5,
    /** @brief Select Mux for debug of TX TL */
    CPDI_DEBUG_TRANSACTION_LAYER_TX_SIDE = 8,
    /** @brief Select Mux for debug of RX 0 TL */
    CPDI_DEBUG_TRANSACTION_LAYER_RX_SIDE_0 = 9,
    /** @brief Select Mux for debug of RX 1 TL */
    CPDI_DEBUG_TRANSACTION_LAYER_RX_SIDE_1 = 10,
} CPDI_DebugMuxModuleSelect;

/** Enable or Disable */
typedef enum
{
    /** Disable the associated parameter */
    CPDI_DISABLE_PARAM = 0,
    /** Enable the associated parameter */
    CPDI_ENABLE_PARAM = 1,
} CPDI_EnableOrDisable;

/** Mask or Unmask */
typedef enum
{
    /** Mask the associated parameter */
    CPDI_MASK_PARAM = 0,
    /** Unmask the associated parameter */
    CPDI_UNMASK_PARAM = 1,
} CPDI_MaskOrUnmask;

/** Receive or Transmit */
typedef enum
{
    /** Receive information */
    CPDI_RECEIVE = 0,
    /** Transmit information */
    CPDI_TRANSMIT = 1,
} CPDI_TransmitOrReceive;

/** Read or Write */
typedef enum
{
    /** Read information */
    CPDI_DO_READ = 0,
    /** Write information */
    CPDI_DO_WRITE = 1,
} CPDI_ReadOrWrite;

/** RP or EP - For a dual mode core the strapping options below indicate how the core has been strapped, or for a single mode core how it is configured. */
typedef enum
{
    /** The core is strapped as an EP */
    CPDI_CORE_EP_STRAPPED = 0,
    /** The core is strapped as an RP */
    CPDI_CORE_RP_STRAPPED = 1,
} CPDI_CoreEpOrRp;

/** Boolean true or false */
typedef enum
{
    /** Boolean false */
    CPDI_FALSE = 0,
    /** Boolean true */
    CPDI_TRUE = 1,
} CPDI_Bool;

/** Receive or Transmit Side */
typedef enum
{
    /** RX side for transfers */
    CPDI_RECEIVE_SIDE = 0,
    /** TX side for transfers */
    CPDI_TRANSMIT_SIDE = 1,
} CPDI_TransmitOrReceiveSide;

/** Upstream or Downstream */
typedef enum
{
    /** Upstream information flow */
    CPDI_UPSTREAM = 0,
    /** Downstream information flow */
    CPDI_DOWNSTREAM = 1,
} CPDI_UpstreamOrDownstream;

/** Wait For Action */
typedef enum
{
    /** Wait for some event to occur */
    CPDI_WAIT = 0,
    /** Do not wait for some event to occur */
    CPDI_DO_NOT_WAIT = 1,
} CPDI_WaitOrNot;

/** Snoop */
typedef enum
{
    /** Snoop */
    CPDI_SNOOP = 0,
    /** No Snoop */
    CPDI_NO_SNOOP = 1,
} CPDI_SnoopOrNoSnoop;

/** Wait For Completion */
typedef enum
{
    /** Wait for completion of operation */
    CPDI_WAIT_TILL_COMPLETE = 0,
    /** Don't Wait for completion of operation */
    CPDI_NO_WAIT_TILL_COMPLETE = 1,
} CPDI_WaitOrNoWaitForComplete;

/** Vendor or Subsystem Vendor IDs */
typedef enum
{
    /** Id type is Vendor ID */
    CPDI_VENDOR_ID = 0,
    /** Id type is Subsystem Vendor ID */
    CPDI_SUBSYSTEM_VENDOR_ID = 1,
} CPDI_AdvertisedIdType;

/** PCIe Lanes */
typedef enum
{
    /** Lane Number 0 */
    CPDI_LANE_0 = 0,
} CPDI_LaneNumber;

/** PCIe Lane Count */
typedef enum
{
    /** 1 Lane */
    CPDI_X1 = 0,
    /** 2 Lanes */
    CPDI_X2 = 1,
    /** 4 Lanes */
    CPDI_X4 = 2,
    /** 8 Lanes */
    CPDI_X8 = 3,
    /** 16 Lanes */
    CPDI_X16 = 4,
} CPDI_LaneCount;

/** Link Speed in terms of GB */
typedef enum
{
    /** Speed is 2.5GB */
    CPDI_SPEED_2p5_GB = 0,
    /** Speed is 5GB */
    CPDI_SPEED_5_GB = 1,
    /** Speed is 8GB */
    CPDI_SPEED_8_GB = 2,
} CPDI_LinkSpeed;

/** Debug Controllable Parameters */
typedef enum
{
    /**
    * @details When this flag is enabled, the AXI bridge places a write request on the HAL Master interface
    * in preference over a read request if both AXI write and AXI read requests are available to be
    * asserted on the same clock cycle.
    */
    CPDI_AXI_BRIDGE_WRITE_PRIORITY = 0,
    /**
    * @details When this field is enabled, it indicates that the remote node advertised Linkwidth Upconfigure
    * Capability in the training sequences in the Configuration Complete state when the link came
    * up. When disabled, it indicates that the remote node did not set the Link Upconfigure bit.
    * The user may set Disable or Enable Link Upconfigure Capability  to turn on or off the link upconfigure
    * capability of the core. Disabling this flag prevents the core from advertising the link
    * upconfigure capability in training sequences transmitted in the Configuration.
    * This flag is ENABLED by default.
    */
    CPDI_LINK_UPCONFIGURE_CAPABILITY = 3,
    /**
    * @details Enabling Fast Link Training is provided to shorten the link training time to facilitate fast
    * simulation of the design, especially at the gate level.  Enabling this flag has the following effects:
    * 1. The 1 ms, 2 ms, 12 ms, 24 ms, 32 ms and 48 ms timeout intervals in the LTSSM are shortened by a factor of 500.
    * 2. In the Polling.Active state of the LTSSM, only 16 training sequences are required to be
    * transmitted (Instead of 1024) to make the transition to the Configuration state.
    * This flag should not be enabled during normal operation of the core.
    * This flag is DISABLED by default.
    */
    CPDI_FAST_LINK_TRAINING = 4,
    /**
    * @details Disabling Electrical Idle Infer in L0 State disables the inferring of electrical idle in the L0 state.
    * Electrical idle is inferred when no flow control updates and no SKP sequences are received
    * within an interval of 128 us.  This flag should be enabled during normal operation, but may be
    * disabled for testing.
    * This flag is ENABLED by default.
    */
    CPDI_ELECTRICAL_IDLE_INFER_IN_L0_STATE = 8,
    /**
    * @details Disabling Flow Control Update Timeout will allow the core to time out and re-train the link when no
    * Flow Control Update DLLPs are received from the link within an interval of 128 us. Disabling this flag
    * disables this timeout. When the advertised receive credit of the link partner is infinity for the header
    * and payload of all credit types, this timeout is always suppressed. The setting of this flag has no
    * effect in this case.  This flag should be enabled during normal operation, but may be disabled
    * for testing.
    * This flag is ENABLED by default.
    */
    CPDI_FLOW_CONTROL_UPDATE_TIMEOUT = 9,
    /**
    * @details Disabling Ordering Checks disables the ordering check in the core between Completions and Posted requests
    * received from the link
    * This flag is ENABLED by default.
    */
    CPDI_ORDERING_CHECKS = 10,
    /**
    * @details Setting this flag to enable causes all the enabled Functions to report an error when a Type-1
    * configuration access is received by the core, targeted at any Function. Disabling the flag limits the
    * error reporting to the type-0 Function whose number matches with the Function number specified in the
    * request.  If the Function number in the request refers to an unimplemented or disabled Function,
    * all enabled Functions report the error regardless of the setting of this bit.
    * This flag is DISABLED by default.
    */
    CPDI_FUNC_SPECIFIC_RPRT_OF_TYPE1_CONF_ACC = 11,
} CPDI_DebugParams;

typedef enum
{
    /**
    * @details Base Address Register 0. Used to define the
    * Function memory and I/O address ranges.
    */
    CPDI_BAR_0 = 0,
    /**
    * @details Base Address Register 1. Used to define the
    * Function memory and I/O address ranges.
    */
    CPDI_BAR_1 = 1,
    /**
    * @details Base Address Register 2. Used to define the
    * Function memory and I/O address ranges.
    */
    CPDI_BAR_2 = 2,
    /**
    * @details Base Address Register 3. Used to define the
    * Function memory and I/O address ranges.
    */
    CPDI_BAR_3 = 3,
    /**
    * @details Base Address Register 4. Used to define the
    * Function memory and I/O address ranges.
    */
    CPDI_BAR_4 = 4,
    /**
    * @details Base Address Register 5. Used to define the
    * Function memory and I/O address ranges.
    */
    CPDI_BAR_5 = 5,
} CPDI_BarNumber;

/** BAR Aperture Coding */
typedef enum
{
    /** BAR Aperture size of 4K */
    CPDI_APERTURE_SIZE_4K = 5,
    /** BAR Aperture size of 8K */
    CPDI_APERTURE_SIZE_8K = 6,
    /** BAR Aperture size of 16K */
    CPDI_APERTURE_SIZE_16K = 7,
    /** BAR Aperture size of 32K */
    CPDI_APERTURE_SIZE_32K = 8,
    /** BAR Aperture size of 64K */
    CPDI_APERTURE_SIZE_64K = 9,
    /** BAR Aperture size of 128K */
    CPDI_APERTURE_SIZE_128K = 10,
    /** BAR Aperture size of 256K */
    CPDI_APERTURE_SIZE_256K = 11,
    /** BAR Aperture size of 512K */
    CPDI_APERTURE_SIZE_512K = 12,
    /** BAR Aperture size of 1M */
    CPDI_APERTURE_SIZE_1M = 13,
    /** BAR Aperture size of 2M */
    CPDI_APERTURE_SIZE_2M = 14,
    /** BAR Aperture size of 4M */
    CPDI_APERTURE_SIZE_4M = 15,
    /** BAR Aperture size of 8M */
    CPDI_APERTURE_SIZE_8M = 16,
    /** BAR Aperture size of 16M */
    CPDI_APERTURE_SIZE_16M = 17,
    /** BAR Aperture size of 32M */
    CPDI_APERTURE_SIZE_32M = 18,
    /** BAR Aperture size of 64M */
    CPDI_APERTURE_SIZE_64M = 19,
    /** BAR Aperture size of 128M */
    CPDI_APERTURE_SIZE_128M = 20,
    /** BAR Aperture size of 256M */
    CPDI_APERTURE_SIZE_256M = 21,
    /** BAR Aperture size of 512M */
    CPDI_APERTURE_SIZE_512M = 22,
    /** BAR Aperture size of 1G */
    CPDI_APERTURE_SIZE_1G = 23,
    /** BAR Aperture size of 2G */
    CPDI_APERTURE_SIZE_2G = 24,
    /** BAR Aperture size of 4G */
    CPDI_APERTURE_SIZE_4G = 25,
    /** BAR Aperture size of 8G */
    CPDI_APERTURE_SIZE_8G = 26,
    /** BAR Aperture size of 16G */
    CPDI_APERTURE_SIZE_16G = 27,
    /** BAR Aperture size of 32G */
    CPDI_APERTURE_SIZE_32G = 28,
    /** BAR Aperture size of 64G */
    CPDI_APERTURE_SIZE_64G = 29,
    /** BAR Aperture size of 128G */
    CPDI_APERTURE_SIZE_128G = 30,
    /** BAR Aperture size of 256G */
    CPDI_APERTURE_SIZE_256G = 31,
} CPDI_BarApertureSize;

/** BAR Control Coding */
typedef enum
{
    /** Disabled */
    CPDI_DISABLED_BAR = 0,
    /** 32 bit IO BAR */
    CPDI_IO_32_BIT_BAR = 1,
    /** 32 bit memory BAR, non-prefetchable */
    CPDI_NON_PREFETCHABLE_32_BIT_MEM_BAR = 4,
    /** 32 bit memory BAR, prefetchable */
    CPDI_PREFETCHABLE_32_BIT_MEM_BAR = 5,
    /** 64 bit memory BAR, non-prefetchable */
    CPDI_NON_PREFETCHABLE_64_BIT_MEM_BAR = 6,
    /** 64 bit memory BAR, prefetchable */
    CPDI_PREFETCHABLE_64_BIT_MEM_BAR = 7,
} CPDI_BarControl;

/** Maskable Interrupt Condition Types */
typedef enum
{
    /** @details Uncorrectable parity error detected while reading from the PNP Receive FIFO RAM */
    CPDI_PNP_RX_FIFO_PARITY_ERROR = 1,
    /** @details Uncorrectable parity error detected while reading from the Completion Receive FIFO RAM */
    CPDI_COMPLETION_RX_FIFO_PARITY_ERROR = 2,
    /** @details Uncorrectable parity error detected while reading from Replay Buffer RAM */
    CPDI_REPLAY_RAM_PARITY_ERROR = 4,
    /** @details Overflow occured in the PNP Receive FIFO */
    CPDI_PNP_RX_FIFO_OVERFLOW_CONDITION = 8,
    /** @details Overflow occured in the Receive FIFO */
    CPDI_COMPLETION_RX_FIFO_OVERFLOW_CONDITION = 16,
    /** @details Replay timer timed out */
    CPDI_REPLAY_TIMEOUT_CONDITION = 32,
    /** @details Replay timer rolled over after 4 transmissions of the same TLP */
    CPDI_REPLAY_TIMEOUT_ROLLOVER_CONDITION = 64,
    /** @details Phy error detected on receive side */
    CPDI_PHY_ERROR = 128,
    /** @details Malformed TLP received from the link */
    CPDI_MALFORMED_TLP_RECEIVED = 256,
    /** @details Unexpected completion received from the link */
    CPDI_UNEXPECTED_COMPLETION_RECEIVED = 512,
    /** @details An error was observed in the flow control advertisements from the other side */
    CPDI_FLOW_CONTROL_ERROR = 1024,
    /** @details A request timed out waiting for completion */
    CPDI_COMPLETION_TIMEOUT_CONDITION = 2048,
} CPDI_MaskableLocalInterruptConditions;

/** Count Registers that save count values */
typedef enum
{
    /**
    * @details The value referred to by this type contains the number of Transaction-Layer Packets received by
    * the core from the link since the value was last reset. This counter saturates on reaching a count
    * of all 1s.
    */
    CPDI_RECEIVE_TLP_COUNT = 0,
    /**
    * @details The value referred to by this type contains the aggregate number of payload double-words received
    * in Transaction-Layer Packets by the core from the link since the value was last reset. This
    * counter saturates on reaching a count of all 1s.
    */
    CPDI_RECEIVE_TLP_PAYLOAD_DWORD_COUNT = 1,
    /**
    * @details The value referred to by this type contains the number of Transaction-Layer Packets transmitted
    * by the core on the link since the value was last reset. This counter saturates on reaching a
    * count of all 1s.
    */
    CPDI_TRANSMIT_TLP_COUNT = 2,
    /**
    * @details The value referred to by this type contains the aggregate number of payload double-words
    * transmitted in Transaction-Layer Packets by the core on the link since the value was last reset.
    * This counter saturates on reaching a count of all 1s.
    */
    CPDI_TRANSMIT_TLP_PAYLOAD_DWORD_COUNT = 3,
    /**
    * @details Number of TLPs received with LCRC errors. This value referred to by this type contains the count
    * of the number of TLPs received by the core with LCRC errors in them. This is a 16-bit saturating
    * counter.
    */
    CPDI_TLP_LCRC_ERROR_COUNT = 4,
    /**
    * @details Number of correctable errors detected while reading from the PNP FIFO RAM.  This is an 8-bit saturating
    * counter that can be cleared by writing all 1s into it.
    */
    CPDI_ECC_CORRECTABLE_ERROR_COUNT_PNP_FIFO_RAM = 5,
    /**
    * @details Number of correctable errors detected while reading from the SC FIFO RAM.  This is an 8-bit saturating
    * counter that can be cleared by writing all 1s into it.
    */
    CPDI_ECC_CORRECTABLE_ERROR_COUNT_SC_FIFO_RAM = 6,
    /**
    * @details Number of correctable errors detected while reading from the Replay Buffer RAM. This is an 8-bit
    * saturating counter that can be cleared by writing all 1s into it.
    */
    CPDI_ECC_CORRECTABLE_ERROR_COUNT_REPLAY_RAM = 7,
    /**
    * @details FTS count received from the other side during link training for use at the 2.5 GT/s link speed.
    * The core transmits this many FTS sequences while exiting the L0S state, when operating at the
    * 2.5 GT/s speed.  These values determine the number of Fast Training Sequences transmitted by the
    * core when it exits the L0s link power state.
    */
    CPDI_RECEIVED_FTS_COUNT_FOR_2_5_GT_SPEED = 9,
    /**
    * @details FTS count received from the other side during link training for use at the 5 GT/s link speed.
    * The core transmits this many FTS sequences while exiting the L0S state, when operating at the
    * 5 GT/s speed.  These values determine the number of Fast Training Sequences transmitted by the
    * core when it exits the L0s link power state.
    */
    CPDI_RECEIVED_FTS_COUNT_FOR_5_GT_SPEED = 10,
    /**
    * @details FTS count transmitted by the core in TS1/TS2 sequences during link training at the 2.5 GT/s link speed
    * This value must be set based on the time needed by the receiver to acquire sync while
    * exiting from L0S state.
    */
    CPDI_TRANSMITTED_FTS_COUNT_FOR_2_5_GT_SPEED = 12,
    /**
    * @details FTS count transmitted by the core in TS1/TS2 sequences during link training at the 5 GT/s link speed
    * This value must be set based on the time needed by the receiver to acquire sync while
    * exiting from L0S state.
    */
    CPDI_TRANSMITTED_FTS_COUNT_FOR_5_GT_SPEED = 13,
    /**
    * @details FTS count transmitted by the core in TS1/TS2 sequences during link training at the 8 GT/s link speed
    * This value must be set based on the time needed by the receiver to acquire sync while
    * exiting from L0S state.
    */
    CPDI_TRANSMITTED_FTS_COUNT_FOR_8_GT_SPEED = 14,
} CPDI_SavedCountParams;

/** Timeout and delay values */
typedef enum
{
    /**
    * @details The value referred to by this type defines the timeout value for transitioning to the
    * power state. If the transmit side has been idle for this interval, the core will transmit the
    * idle sequence on the link and transition the state of the link to L0S.
    * The value referred to by this type Contains the timeout value (in units of 4 ns) for
    * transitioning to the L0S power state. (Setting it to 0 permanently disables the transition
    * to the L0S power state - refer to the Cadence PCIe Core User Guide)
    */
    CPDI_L0S_TIMEOUT = 0,
    /**
    * @details The value referred to by this type contains the timeout value used to detect a completion
    * timeout event for a request originated by the core from its HAL master interface,
    * (measured in 4 ns cycles).
    */
    CPDI_COMPLETION_TIMEOUT_LIMIT_0 = 1,
    /**
    * @details The value referred to by this type contains the timeout value used to detect a completion timeout
    * event for a request originated  by the core from its HAL master interface,
    * (measured in 4 ns cycles).
    */
    CPDI_COMPLETION_TIMEOUT_LIMIT_1 = 2,
    /**
    * @details The value referred to by this type specifies the time the core will wait before it re-enters
    * the L1 state if its link partner transitions the link to L0 while all the Functions of the
    * core are in D3 power state. The core will change the power state of the link from L0 to L1 if
    * no activity is detected both on the transmit and receive sides before this interval, while all
    * Function are in D3 state and the link is in L0. Setting this value to 0 disables re-rentry to
    * L1 state if the link partner returns the link to L0 from L1 when all the Functions of
    * the core are in D3 state. The value referred to by this type controls only the re-entry to L1.
    * The initial transition to L1 always occurs when the all the Functions of the core are set to
    * the D3 state. Delay to re-enter L1 after no activity is specified in units of 4 ns cycles.
    */
    CPDI_L1_STATE_RETRY_DELAY = 3,
    /**
    * @details The value referred to by this type defines the timeout value for transitioning to the L1 power
    * state under Active State Power management. If the transmit side has been idle for this
    * interval, the core will initiate a transition of its link to the L1 power state. Contains the
    * timeout value (in units of 4 ns) for transitioning to the L1 power state. Setting it to 0
    * permanently disables the transition to the L1 power state.
    */
    CPDI_ASPM_L1_ENTRY_TIMEOUT_DELAY = 4,
    /**
    * @details The value referred to by this type defines the delay between the core receiving a
    * PME_Turn_Off message from the link and generating a PME_TO_Ack message in response to it.
    * The core generates the ack only when all its Functions are in low-power states (D1 or D3).
    * Time in microseconds between the core receiving a PME_Turn_Off message TLP and sending a PME_TO_Ack
    * response to it. This field must be set to a non-zero value to enable the core to send the response.
    * Setting this field to 0 suppresses the cores response to the PME_Turn_Off message, so that
    * the client may transmit the PME_TO_Ack message through the HAL master interface.
    */
    CPDI_PME_TURNOFF_ACK_DELAY = 5,
    /**
    * @details Additional transmit-side replay timer timeout interval. This 9-bit value is added as a signed 2s
    * complement number to the internal replay timer timeout value computed by the core based on the
    * PCI Express Specifications. This enables the user to make minor adjustments to the spec-defined
    * replay timer settings. Its value is in multiples of 4 ns (maximum = +1020 ns, minimum = 1024 ns).
    */
    CPDI_TRANSMIT_SIDE_REPLAY_TIMEOUT = 6,
    /**
    * @details Additional receive side ACK-NAK timer timeout interval. This 9-bit value is added as a
    * signed 2s complement number to the internal ACK-NAK timer timeout value computed by the core
    * based on the PCI Express Specifications.
    */
    CPDI_RECEIVE_SIDE_ACK_NACK_REPLAY_TIMEOUT = 7,
} CPDI_TimeoutAndDelayValues;

/** Credit Limit Registers */
typedef enum
{
    /** @details Posted payload credit limit advertised by the core for VC 0. (in units of 4 Dwords). */
    CPDI_POSTED_PAYLOAD_CREDIT = 0,
    /** @details Posted payload credit limit advertised by the core for VC 0 (in number of packets). */
    CPDI_POSTED_HEADER_CREDIT = 1,
    /** @details Non-Posted payload credit limit advertised by the core for VC 0 (in units of 4 Dwords). */
    CPDI_NONPOSTED_PAYLOAD_CREDIT = 2,
    /** @details Non-Posted Header Credit limit advertised by the core for VC 0 (in number of packets). */
    CPDI_NONPOSTED_HEADER_CREDIT = 3,
    /** @details Completion payload credit limit advertised by the core for VC 0 (in units of 4 Dwords). */
    CPDI_COMPLETION_PAYLOAD_CREDIT = 4,
    /** @details Completion Header Credit limit advertised by the core for VC 0 (in number of packets). */
    CPDI_COMPLETION_HEADER_CREDIT = 5,
} CPDI_CreditLimitTypes;

/** Credit Update Intervals */
typedef enum
{
    /**
    * @details Minimum Posted credit update interval for posted transactions.
    * The core follows this minimum interval between issuing posted credit updates on the link.
    * This is to limit the bandwidth use of credit updates. If new credit becomes available in
    * the receive FIFO since the last update was sent, the core will issue a new update only
    * after this interval has elapsed since the last update. The value is in units of 4 ns.
    * This field is re-written by the internal logic when the negotiated link width or link
    * speed changes, to correspond to the default values. The user may override this default
    * value by writing into the value referred to by this type. The value written will be lost
    * on a change in the negotiated link width/speed.
    */
    CPDI_MIN_POSTED_CREDIT_UPDATE_INTERVAL = 0,
    /**
    * @details Minimum credit update interval for non-posted transactions. The core follows this minimum
    * interval between issuing posted credit updates on the link. This is to limit the bandwidth
    * use of credit updates. If new credit becomes available in the receive FIFO since the last
    * update was sent, the core will issue a new update only after this interval has elapsed
    * since the last update. The value is in units of 4ns.
    */
    CPDI_MIN_NONPOSTED_CREDIT_UPDATE_INTERVAL = 1,
    /**
    * @details Minimum credit update interval for Completion packets.
    * The core follows this minimum interval between issuing posted credit updates on the link.
    * This is to limit the bandwidth use of credit updates. If new credit becomes available in
    * the receive FIFO since the last update was sent, the core will issue a new update only
    * after this interval has elapsed since the last update. The value is in units of 4 ns.
    * This parameter is not used when the Completion credit is infinity.
    */
    CPDI_MIN_COMPLETION_UPDATE_INERVAL = 2,
    /**
    * Maximum credit update interval for all transactions. If no new credit has become
    * available since the last update, the core will repeat the last update after this interval.
    * This is to recover from any losses of credit update packets. The value is in units of 4 ns.
    * This field could be re-written by the internal logic when the negotiated link width or
    * link speed changes, to correspond to the default values for all link widths and speeds.
    * The user may override this default value by writing into value referred to by this
    * type field. The value written will be lost on a change in the negotiated link width/speed.
    */
    CPDI_MAX_UPDATE_INTERVAL_FOR_ALL = 3,
} CPDI_CreditUpdateIntervals;

/** RP BAR Numbers */
typedef enum
{
    /**
    * @details Base Address Register 0. Used to define the
    * RP memory and I/O address ranges.
    */
    CPDI_RP_BAR_0 = 0,
    /**
    * @details Base Address Register 1. Used to define the
    * RP memory and I/O address ranges.
    */
    CPDI_RP_BAR_1 = 1,
} CPDI_RpBarNumber;

/** RP BAR Aperture Coding */
typedef enum
{
    /** BAR Aperture size of 4K */
    CPDI_RP_APERTURE_SIZE_4K = 10,
    /** BAR Aperture size of 8K */
    CPDI_RP_APERTURE_SIZE_8K = 11,
    /** BAR Aperture size of 16K */
    CPDI_RP_APERTURE_SIZE_16K = 12,
    /** BAR Aperture size of 32K */
    CPDI_RP_APERTURE_SIZE_32K = 13,
    /** BAR Aperture size of 64K */
    CPDI_RP_APERTURE_SIZE_64K = 14,
    /** BAR Aperture size of 128K */
    CPDI_RP_APERTURE_SIZE_128K = 15,
    /** BAR Aperture size of 256K */
    CPDI_RP_APERTURE_SIZE_256K = 16,
    /** BAR Aperture size of 512K */
    CPDI_RP_APERTURE_SIZE_512K = 17,
    /** BAR Aperture size of 1M */
    CPDI_RP_APERTURE_SIZE_1M = 18,
    /** BAR Aperture size of 2M */
    CPDI_RP_APERTURE_SIZE_2M = 19,
    /** BAR Aperture size of 3M */
    CPDI_RP_APERTURE_SIZE_4M = 20,
    /** BAR Aperture size of 4M */
    CPDI_RP_APERTURE_SIZE_8M = 21,
    /** BAR Aperture size of 16M */
    CPDI_RP_APERTURE_SIZE_16M = 22,
    /** BAR Aperture size of 32M */
    CPDI_RP_APERTURE_SIZE_32M = 23,
    /** BAR Aperture size of 64M */
    CPDI_RP_APERTURE_SIZE_64M = 24,
    /** BAR Aperture size of 128M */
    CPDI_RP_APERTURE_SIZE_128M = 25,
    /** BAR Aperture size of 256M */
    CPDI_RP_APERTURE_SIZE_256M = 26,
    /** BAR Aperture size of 512M */
    CPDI_RP_APERTURE_SIZE_512M = 27,
    /** BAR Aperture size of 1G */
    CPDI_RP_APERTURE_SIZE_1G = 28,
    /** BAR Aperture size of 2G */
    CPDI_RP_APERTURE_SIZE_2G = 29,
    /** BAR Aperture size of 4G */
    CPDI_RP_APERTURE_SIZE_4G = 30,
    /** BAR Aperture size of 8G */
    CPDI_RP_APERTURE_SIZE_8G = 31,
    /** BAR Aperture size of 16G */
    CPDI_RP_APERTURE_SIZE_16G = 32,
    /** BAR Aperture size of 32G */
    CPDI_RP_APERTURE_SIZE_32G = 33,
    /** BAR Aperture size of 64G */
    CPDI_RP_APERTURE_SIZE_64G = 34,
    /** BAR Aperture size of 128G */
    CPDI_RP_APERTURE_SIZE_128G = 35,
    /** BAR Aperture size of 128G */
    CPDI_RP_APERTURE_SIZE_256G = 36,
} CPDI_RpBarApertureSize;

/** Root Port BAR Control Coding */
typedef enum
{
    /** Disabled */
    CPDI_RP_DISABLED_BAR = 0,
    /** 32 bit memory BAR */
    CPDI_RP_32_BIT_IO_BAR = 1,
    /** 32 bit memory BAR - type 0 accesses */
    CPDI_RP_TYPE_0_32_BIT_MEM_BAR = 4,
    /** 32 bit memory BAR - type 1 accesses */
    CPDI_RP_TYPE_1_32_BIT_MEM_BAR = 5,
    /** 64 bit memory BAR - type 0 accesses */
    CPDI_RP_TYPE_0_64_BIT_MEM_BAR = 6,
    /** 64 bit memory BAR - type 1 accesses */
    CPDI_RP_TYPE_1_64_BIT_MEM_BAR = 7,
} CPDI_RpBarControl;

/** Type 1 access for RP */
typedef enum
{
    /** Disabled */
    CPDI_RP_TYPE1_DISABLED_ALL = 0,
    /** 32 bit prefetchable, io disabled */
    CPDI_RP_TYPE1_PREFETCH_32_BIT_IO_DISABLED = 1,
    /** 64 bit prefetchable, io disabled */
    CPDI_RP_TYPE1_PREFETCH_64_BIT_IO_DISABLED = 2,
    /** disabled prefetchable, 16 bit io */
    CPDI_RP_TYPE1_PREFETCH_DISABLED_IO_16_BIT = 3,
    /** 32 bit prefetchable, 16 bit io */
    CPDI_RP_TYPE1_PREFETCH_32_BIT_IO_16_BIT = 4,
    /** 64 bit prefetchable, 16 bit io */
    CPDI_RP_TYPE1_PREFETCH_64_BIT_IO_16_BIT = 5,
    /** disabled prefetchable, 32 bit io */
    CPDI_RP_TYPE1_PREFETCH_DISABLED_IO_32_BIT = 6,
    /** 32 bit prefetchable, 32 bit io */
    CPDI_RP_TYPE1_PREFETCH_32_BIT_IO_32_BIT = 7,
    /** 64 bit prefetchable, 32 bit io */
    CPDI_RP_TYPE1_PREFETCH_64_BIT_IO_32_BIT = 8,
} CPDI_RpType1ConfigControl;

/** UDMA continuity modes */
typedef enum
{
    /** Read the data and write the data */
    CPDI_READ_WRITE = 0,
    /** Only read data do not write it */
    CPDI_PREFETCH = 1,
    /** Write previously read data */
    CPDI_POSTWRITE = 2,
} CPDI_UdmaContinuity;

/** UDMA buffer error types */
typedef enum
{
    /** IB buffer, uncorrected ECC errors */
    CPDI_UNCORRECTED_ECC_IB = 0,
    /** IB buffer, corrected ECC errors */
    CPDI_CORRECTED_ECC_IB = 1,
    /** OB buffer, uncorrected ECC errors */
    CPDI_UNCORRECTED_ECC_OB = 2,
    /** OB buffer, corrected ECC errors */
    CPDI_CORRECTED_ECC_OB = 3,
} CPDI_UdmaBufferErrorType;

/** Data size in bytes. */
typedef enum
{
    /** 1 byte (8 bits) */
    CPDI_1_BYTE = 1,
    /** 2 byte (16 bits) */
    CPDI_2_BYTE = 2,
    /** 4 byte (32 bits) */
    CPDI_4_BYTE = 4,
} CPDI_SizeInBytes;

/**********************************************************************
 * Callbacks
 **********************************************************************/
/** Error Callback */
typedef void (*CPDI_ErrorCallback)(uint32_t re);

/**********************************************************************
 * Structures and unions
 **********************************************************************/
/** UDMA configuration.  This struct is used to read configuration from the UDMA core */
typedef struct CPDI_UdmaConfiguration
{
    /** Number of UDMA channels supported by the core */
    uint32_t numChannels;
    /** Number of partitions in each RAM */
    uint32_t numPartitions;
    /**
     * Size of RAM partition is (128 * (2 to the power partitionSize)).
     * Note that scatter/gather transfers are limited to RAM partition size / numPartitions
    */
    uint32_t partitionSize;
    /** TRUE if system addresses are 64bit wide */
    CPDI_Bool wideSysAddr;
    /** TRUE if attributes for system bus are 64bit wide */
    CPDI_Bool wideSysAttr;
    /** TRUE if external addresses are 64bit wide */
    CPDI_Bool wideExtAddr;
    /** TRUE if attributes for external bus are 64bit wide */
    CPDI_Bool wideExtAttr;
}  CPDI_UdmaConfiguration;

/** Chip Initialization Info  - Used ONLY for initializing the chip at init */
typedef struct CPDI_InitValue
{
    /** Flag to indicate a change in default value in register */
    uint32_t changeDefault;
    /** The new value to be programmed into a register */
    uint32_t newValue;
}  CPDI_InitValue;

/** Chip Initialization - Credit Related Parameters */
typedef struct CPDI_InitCreditRelatedParams
{
    /**
     * @brief Initialization value for posted paymoad credit
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamPostedPayloadCredit;
    /**
     * @brief Initialization value for posted header credit
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamPostedHeaderCredit;
    /**
     * @brief Initialization value for non-posted payload credit
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamNonPostedPayloadCredit;
    /**
     * @brief Initialization value for non-posted header credit
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamNonPostedHeaderCredit;
    /**
     * @brief Initialization value for completion payload credit
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamCompletionPayloadCredit;
    /**
     * @brief Initialization value for completion header credit
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamCompletionHeaderCredit;
    /**
     * @brief Initialization value for min posted update interval
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamMinPostedUpdateInterval;
    /**
     * @brief Initialization value for min non-posted update interval
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamMinNonPostedUpdateInterval;
    /**
     * @brief Initialization value for credit update interval for Completion packets
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamCompletionUpdateInterval;
    /**
     * @brief Initialization value for maximum update interval for all transactions
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamMaxUpdateInterval;
}  CPDI_InitCreditRelatedParams;

/** Chip Initialization - Count Related Parameters */
typedef struct CPDI_InitCountRelatedParams
{
    /**
     * @brief Initialization value for transmitted FTS count at 2.5GB
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamTransmittedFtsCount2_5Gbs;
    /**
     * @brief Initialization value for transmitted FTS count at 5GB
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamTransmittedFtsCount5Gbs;
    /**
     * @brief Initialization value for transmitted FTS count at 8GB
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamTransmittedFtsCount8Gbs;
}  CPDI_InitCountRelatedParams;

/** Chip Initialization - Timing Related Parameters */
typedef struct CPDI_InitTimingRelatedParams
{
    /**
     * @brief Initialization value for  L0S_timeout
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamL0sTimeout;
    /**
     * @brief Initialization value for completion_timeout_limit_0
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamCompletionTimeoutLimit0;
    /**
     * @brief Initialization value for completion_timeout_limit_1
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamCompletionTimeoutLimit1;
    /**
     * @brief Initialization value for l1_retry_delay
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamL1RetryDelay;
    /**
     * @brief Initialization value for tx_replay_timeout_adjustment
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamTxReplayTimeoutAdjustment;
    /**
     * @brief Initialization value for rx_replay_timeout_adjustment
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamRxReplayTimeoutAdjustment;
    /**
     * @brief Initialization value for l1_timeout
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamL1Timeout;
    /**
     * @brief Initialization value for pme_turnoff_ack_delay
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamPmeTurnoffAckDelay;
}  CPDI_InitTimingRelatedParams;

/** Chip Initialization - Identifier Related Parameters */
typedef struct CPDI_InitIdRelatedParams
{
    /**
     * @brief Initialization value for VendorId
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamVendorId;
    /**
     * @brief Initialization value for SubsystemVendorId
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamSubsystemVendorId;
}  CPDI_InitIdRelatedParams;

/** Chip Initialization - Target Lane Map Related Parameters */
typedef struct CPDI_TargetLaneMapRelatedParams
{
    /**
     * @brief Initialization value for TargetLaneMap
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamTargetLaneMap;
}  CPDI_TargetLaneMapRelatedParams;

/** Chip Initialization - Debug Related Parameters */
typedef struct CPDI_InitDebugRelatedParams
{
    /**
     * @brief Initialization value for MuxSelect
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamMuxSelect;
    /**
     * @brief Initialization value for SyncHeaderFramingError
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamSyncHeaderFramingError;
    /**
     * @brief Initialization value for LinkReTrainingFramingError
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamLinkReTrainingFramingError;
    /**
     * @brief Initialization value for LinkUpconfigureCapability
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamLinkUpconfigureCapability;
    /**
     * @brief Initialization value for FastLinkTraining
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamFastLinkTraining;
    /**
     * @brief Initialization value for Gen3LfsrUpdateFromSkp
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamGen3LfsrUpdateFromSkp;
    /**
     * @brief Initialization value for ElectricalIdleInferInL0State
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamElectricalIdleInferInL0State;
    /**
     * @brief Initialization value for FlowControlUpdateTimeout
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamFlowControlUpdateTimeout;
    /**
     * @brief Initialization value for OrderingChecks
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamOrderingChecks;
    /**
     * @brief Initialization value for FuncSpecificRprtType1CfgAccess
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamFuncSpecificRprtType1CfgAccess;
    /**
     * @details When this value is enabled (set to a non-0 value), the core will report all
     * phy errors detected at the PIPE interface, regardless of whether it caused
     * packet corruption. If disabled, the core will report only those
     * errors that caused a TLP or DLLP to be dropped because of the phy error.
    */
    struct CPDI_InitValue initparamAllPhyErrorReporting;
    /**
     * @details This value when enabled (set to a non-0 value), will drive the PIPE_TX_SWING
     * output of the core
    */
    struct CPDI_InitValue initparamTxSwing;
    /**
     * @details When the core is operating as a Root Port, setting this to 1 causes the LTSSM to
     * initiate a loopback and become the loopback master.
     * This bit is not used in the EndPoint Mode.
    */
    struct CPDI_InitValue initparamMasterLoopback;
}  CPDI_InitDebugRelatedParams;

/** Chip Initialization - Maskable Conditions */
typedef struct CPDI_InitMaskableConditions
{
    /**
     * @brief Initialization value for PnpRxFifoParity Error interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamMaskPnpRxFifoParityError;
    /**
     * @brief Initialization value for CompletionRxFifoParity Error interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamMaskCompletionRxFifoParityError;
    /**
     * @brief Initialization value for ReplayRamParity Error interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamMaskReplayRamParityError;
    /**
     * @brief Initialization value for PnpRxFifoOverflow interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamMaskPnpRxFifoOverflow;
    /**
     * @brief Initialization value for CompletionRxFifoOverflow interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamMaskCompletionRxFifoOverflow;
    /**
     * @brief Initialization value for ReplayTimeout interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamMaskReplayTimeout;
    /**
     * @brief Initialization value for ReplayTimerRollover interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamMaskReplayTimerRollover;
    /**
     * @brief Initialization value for PhyError interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamMaskPhyError;
    /**
     * @brief Initialization value for MalformedTlpReceived interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamMaskMalformedTlpReceived;
    /**
     * @brief Initialization value for UnexpectedCompletionReceived interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamMaskUnexpectedCompletionReceived;
    /**
     * @brief Initialization value for FlowControlError interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamMaskFlowControlError;
    /**
     * @brief Initialization value for CompletionTimeout interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamMaskCompletionTimeout;
}  CPDI_InitMaskableConditions;

/** Chip Initialization - Init RP BAR Parameters */
typedef struct CPDI_InitRpBarParams
{
    /**
     * @brief Initialization value for Root Port Bar Aperture
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamRpBarAperture[3];
    /**
     * @brief Initialization value for Root Port Bar Control
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamRpBarControl[3];
    /**
     * @brief Initialization value for initparamEnableRpBarCheck
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitValue initparamEnableRpBarCheck;
}  CPDI_InitRpBarParams;

/** Chip Base Addresses */
typedef struct CPDI_InitBaseAddresses
{
    /** @details Base address for Local Management registers */
    struct CPDI_InitValue initparamLmBase;
    /** @details Base address for AXI Wrapper registers */
    struct CPDI_InitValue initparamAwBase;
    /** @details Base address for AXI Interface, to be used for ECAM config access */
    struct CPDI_InitValue initparamAxBase;
}  CPDI_InitBaseAddresses;

/** Chip Initialization - Init structure to intialize or leave at default */
typedef struct CPDI_InitParam
{
    /**
     * @brief Initialization Values for Base
     * addresses
    */
    struct CPDI_InitBaseAddresses initBaseAddresses;
    /**
     * @brief Initialization Values For Credit Related Params
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitCreditRelatedParams initCreditParams;
    /**
     * Initialization Values For Count Registers that save count values
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitCountRelatedParams initCountParams;
    /**
     * @brief Initialization Values For Timeouts, Delays and Latency Settings
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitTimingRelatedParams initTimingParams;
    /**
     * @brief Initialization Values For Identifiers
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitIdRelatedParams initIdParams;
    /**
     * @brief Initialization Values For Target Lane Map
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_TargetLaneMapRelatedParams initTargetLaneMap;
    /**
     * @brief Initialization Values For Debug Controllable Parameters
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitDebugRelatedParams initDebugParams;
    /**
     * @brief Initialization Values For Maskable Interrupt Condition Types
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitMaskableConditions initMaskableConditions;
    /**
     * @brief Initialization Values For Root Port Aperture and BARS
     * @details Refer to the Cadence PCIe Core User Guide for further information
    */
    struct CPDI_InitRpBarParams initRpBarParams;
    /**
     * @details Error Callback which will be called by the Core driver in the case
     * it encounters one or multiple errors
    */
    CPDI_ErrorCallback initErrorCallback;
}  CPDI_InitParam;

/** The structure that contains the resource requirements for driver operation */
typedef struct CPDI_SysReq
{
    /** @details The number of bytes required for driver operation */
    uint32_t pdataSize;
}  CPDI_SysReq;

/**
 *  @}
 */

/** @defgroup DriverObject Driver API Object
 *  API listing for the driver. The API is contained in the object as
 *  function pointers in the object structure. As the actual functions
 *  resides in the Driver Object, the client software must first use the
 *  global GetInstance function to obtain the Driver Object Pointer.
 *  The actual APIs then can be invoked using obj->(api_name)() syntax.
 *  These functions are defined in the header file of the core driver
 *  and utilized by the API.
 *  @{
 */

/**********************************************************************
 * API methods
 **********************************************************************/
typedef struct CPDI_OBJ
{
    /**
     * Obtain the private memory size required by the driver
     * @param[in] cfg The base address of the PCIe core
     * @param[out] sysReq The resource requirement for driver operation
     * @return 0 on success
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*probe)(uintptr_t cfg, CPDI_SysReq* sysReq);

    /**
     * Initialize the PCIe driver. Must be called before all other access
     * APIs. The function takes in the initalization parameter structure
     * and over-ride the hardware initial value when the struture
     * indicated so. Otherwise, the init function will retain the default
     * value in the hardware. *
     * @param[in] initParam Configuration parameters for driver/hardware initialization.
     * @param[in] cfg The base address of the PCIe core
     * @param[out] pD Memory pointer to the uninitialized private data of the size specified by probe.
     * @return 0 on success
     * @return EIO if driver encountered an error accessing hardware
     * @return EINVAL if illegal/inconsistent values in config
     */
    uint32_t (*init)(void* pD, uintptr_t cfg, CPDI_InitParam* initParam);

    /**
     * Destructor for the driver. (NOP) *
     * @param[in] pD Pointer to the private data initialized by init
     */
    void (*destroy)(void* pD);

    /**
     * Start IRQ Sources. (NOP) *
     * @param[in] maskableIntrParam
     * @param[in] pD Pointer to the private data initialized by init
     */
    uint32_t (*start)(void* pD, CPDI_MaskableLocalInterruptConditions maskableIntrParam);

    /**
     * Stop IRQ Sources. (NOP) *
     * @param[in] maskableIntrParam
     * @param[in] pD Pointer to the private data initialized by init
     */
    uint32_t (*stop)(void* pD, CPDI_MaskableLocalInterruptConditions maskableIntrParam);

    /**
     * interrupt service routine/polling routine (NOP) *
     * @param[in] pD Pointer to the private data initialized by init
     */
    uint32_t (*isr)(void* pD);

    /**
     * This function checks to see if link training is complete by
     * checking for the current state of link. Function does not check
     * for the validity of the base address of the PCIe core passed into
     * the function as a parameter.
     * @param[in] pD Pointer to the private data initialized by init
     * @param[out] trainingComplete Indicates if training is complete
     * @return 0 function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*isLinkTrainingComplete)(void* pD, CPDI_Bool* trainingComplete);

    /**
     * This function gets the current state that the LTSSM is in.
     * Provides current state of LTSSM during while link training is in
     * progress. DETECT_QUIET state will be returned when reset is
     * applied, and the L0 state will be returned when link training is
     * completed.
     * @param[out] ltssm_state The state of the LTSSM
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*getLinkTrainingState)(void* pD, CPDI_LtssmState* ltssm_state);

    /**
     * This function obtains link training direction. When crosslink is
     * enabled, the value returned by this function indicates whether the
     * core completed link training as an upstream port or a downstream
     * port.
     * @param[out] upOrDown Upstream or downstream port
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*getLinkTrainingDirection)(void* pD, CPDI_UpstreamOrDownstream* upOrDown);

    /**
     * This function indicates if the core is strapped an EP or an RP for
     * a dual-mode core.
     * @param[out] epOrRp This will always be RP
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*isCoreStrappedAsEpOrRp)(void* pD, CPDI_CoreEpOrRp* epOrRp);

    /**
     * This function gets the Negotiated speed of the link. This function
     * will get the re-written value of link speed when the negotiated
     * link width or link speed changes. This function gets the
     * negotiated link width when link training reaches the L0 state. To
     * check if the link is in L0 state, the function
     * "GetLinkTrainingState" may be called. This function returns the
     * updated current operating speed of the link when the link state is
     * L0.
     * @param[out] ns The negotiated Link Speed
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*getNegotiatedLinkSpeed)(void* pD, CPDI_LinkSpeed* ns);

    /**
     * Link id received from other side during link training.
     * @param[out] linkId The Link ID returned
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 if the function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*getReceivedLinkId)(void* pD, uint32_t* linkId);

    /**
     * This function indicates whether the remote node advertised
     * Linkwidth Upconfigure Capability in the training sequences in the
     * Configuration.Complete state when the link came up.
     * @param[in,out] upconfig Whether the link is upconfigurable or not
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 if the function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*isRemoteLinkwidthUpconfigurable)(void* pD, CPDI_Bool* upconfig);

    /**
     * This function allows the reading or writing of the Transmitted
     * Link id. This is the Link id transmitted by the device in training
     * sequences in the RP mode.
     * @param[in,out] linkId_rd_or_to_write The link ID to be read or written
     * @param[in] readOrWrite Is the parameter being read or written
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 if the function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*accessTransmittedLinkID)(void* pD, CPDI_ReadOrWrite readOrWrite, uint8_t* linkId_rd_or_to_write);

    /**
     * This function allows access to read or write the the Vendor ID and
     * the Subsystem Vendor ID that the device advertises during its
     * enumeration of the PCI configuration space.
     * @param[in,out] valRdOrWritten The value to be read or written
     * @param[in] readOrWrite Is the parameter being read or written
     * @param[in] id Specify whether to access Vendor or Subsystem Id
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 if the function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*accessVendorIdSubsystemVendorId)(void* pD, CPDI_AdvertisedIdType id, CPDI_ReadOrWrite readOrWrite, uint16_t* valRdOrWritten);

    /**
     * This function reads or writes timing based parameters
     * @param[in,out] valRdOrWritten The value to be read or written
     * @param[in] readOrWrite Is the parameter being read or written
     * @param[in] timeoutAndDelayParam Specify timeout and delay parameter
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 if the function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*accessTimingParams)(void* pD, CPDI_TimeoutAndDelayValues timeoutAndDelayParam, CPDI_ReadOrWrite readOrWrite, uint32_t* valRdOrWritten);

    /**
     * This function allows reading or writing the timeout value for
     * transitioning to the L0S power state. If the transmit side has
     * been idle for this interval, the core will transmit the idle
     * sequence on the link and transition the state of the link to L0S.
     * @param[in,out] valRdOrWritten The value to be read or written
     * @param[in] readOrWrite Is the parameter being read or written
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 if the function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*accessL0sTimeout)(void* pD, CPDI_ReadOrWrite readOrWrite, uint32_t* valRdOrWritten);

    /**
     * This API is disables the LTSSM from transitioning to the L0S state
     * @param[in] pD Pointer to the private data initialized by init
     */
    uint32_t (*disableRpTransitionToL0s)(void* pD);

    /**
     * This function allows access to settings that contain the initial
     * credit limits advertised by the core during the DL initialization.
     * If these settings are modified, the link must be re-trained to re-
     * initialize the DL for the modified settings to take effect.
     * @param[in] creditLimitParam The Credit Limit parameter that is to be read or written
     * @param[in] readOrWrite Is the parameter being read or written
     * @param[in,out] setOrReturnedValue The value being written or read
     * @param[in] pD Pointer to the private data initialized by init
     * @param[in] vcNum The virtual channel number.
     * @param[in] transmitOrReceive Is the paramter on the transmit or the receive side
     * @return 0 if the function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*accessCreditLimitSettings)(void* pD, CPDI_CreditLimitTypes creditLimitParam, CPDI_TransmitOrReceive transmitOrReceive, CPDI_ReadOrWrite readOrWrite, uint8_t vcNum, uint32_t* setOrReturnedValue);

    /**
     * This function controls parameters that control how frequently the
     * core sends a credit update to the opposite node.
     * @param[in,out] setOrReturnedValue The value being read or written
     * @param[in] readOrWrite Is the parameter being read or written
     * @param[in] pD Pointer to the private data initialized by init
     * @param[in] creditUpdateIntervalParam Which credit interval param are we accessing
     * @return 0 if the function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*accessTransmitCreditUpdateIntervalSettings)(void* pD, CPDI_CreditUpdateIntervals creditUpdateIntervalParam, CPDI_ReadOrWrite readOrWrite, uint32_t* setOrReturnedValue);

    /**
     * The root port side of the core contains three 64-bit memory BARs
     * that can be used for address-range checking of incoming requests
     * from devices connected to it. This function determines the
     * configuration of these BARs.
     * @param[in] barNumber The BAR number to be accessed
     * @param[in] readOrWrite Is the parameter being read or written
     * @param[in] pD Pointer to the private data initialized by init
     * @param[in,out] apertureSize The aperture size value to be read or written.  The miniumum value is 4K.
     * @return 0 if the function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*accessRootPortBarApertureSetting)(void* pD, CPDI_RpBarNumber barNumber, CPDI_ReadOrWrite readOrWrite, CPDI_RpBarApertureSize* apertureSize);

    /**
     * This function allows for accessing or modifying the configuration
     * of RP BARs.
     * @param[in,out] rpBarControl The control value to be read or written
     * @param[in] barNumber The RP BAR number to access
     * @param[in] readOrWrite Is the parameter being read or written
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 if the function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*accessRootPortBarControlSetting)(void* pD, CPDI_RpBarNumber barNumber, CPDI_ReadOrWrite readOrWrite, CPDI_RpBarControl* rpBarControl);

    /**
     * This function allows for accessing or modifying the configuration
     * for type 1 config accesses
     * @param[in] readOrWrite Is the parameter being read or written
     * @param[in,out] rpType1Config The control value to be read or written
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 if the function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*accessRootPortype1ConfigSetting)(void* pD, CPDI_ReadOrWrite readOrWrite, CPDI_RpType1ConfigControl* rpType1Config);

    /**
     * This function is used to enable BAR checking in the RP mode.  When
     * disabled, the core will forward all incoming memory requests to
     * the client logic without checking their address ranges.
     * @param[in] enOrDisen Enable or disable RP BAR Address range checking
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 if the function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*controlRootPortBarCheck)(void* pD, CPDI_EnableOrDisable enOrDisen);

    /**
     * This function is used to Enable or Disable the RP to cause the
     * LTSSM to initiate a loopback and become a loopback master. This
     * function controls Loopback functionality in RP mode only. When the
     * core is operating as a Root Port, enabling Loopback causes the
     * LTSSM to initiate a loopback and become the loopback master. This
     * function is not used in the EndPoint Mode.
     * @param[in] enOrDisen Enable or disable loopback functionality
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 if the function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*controlRpMasterLoopback)(void* pD, CPDI_EnableOrDisable enOrDisen);

    /**
     * This function allows access to the Debug Mux. This function
     * selects the group of signals within the module that are driven on
     * the debug bus.
     * @param[in] readOrWrite Whether the parameter is being read from or written to
     * @param[in,out] val_rd_or_to_write The Module Select value to be read or written
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 if the function comepleted successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*accessDebugMux)(void* pD, CPDI_ReadOrWrite readOrWrite, CPDI_DebugMuxModuleSelect* val_rd_or_to_write);

    /**
     * Debug parameters that may be enabled or disabled
     * @param[in] debugControllableParam The debug parameter that needs to be controlled
     * @param[in] enOrDisen Enable or Disable the debug parameter
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 on training complete
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*controlDebugParams)(void* pD, CPDI_DebugParams debugControllableParam, CPDI_EnableOrDisable enOrDisen);

    /**
     * This function allows access to saved count values.
     * @param[in,out] returnedCount the count value read or maybe to be written
     * @param[in] readOrWrite Is the parameter being read or written
     * @param[in] countParam The count parameter that needs to be accessed
     * @param[in] pD Pointer to the private data initialized by init
     * @param[in] resetAfterRead Whether the count should be reset after the read
     * @return 0 if the function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*accessSavedCountValues)(void* pD, CPDI_SavedCountParams countParam, CPDI_Bool resetAfterRead, CPDI_ReadOrWrite readOrWrite, uint32_t* returnedCount);

    /**
     * This function controls if "All Phy Error Reporting" is to be
     * enabled When enabled, the core will report all phy errors detected
     * at the PIPE interface, regardless of whether it caused packet
     * corruption. When disabled, the core will report only those errors
     * that caused a TLP or DLLP to be dropped because of the phy error.
     * @param[in] enOrDisen Enable or disable reporting of all phy errors
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 if function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*controlReportingOfAllPhyErrors)(void* pD, CPDI_EnableOrDisable enOrDisen);

    /**
     * This function controls if TX_SWING on the PIPE interface is driven
     * or not.
     * @param[in] enOrDisen Enable or disable reporting of all phy errors
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 if function completed successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*controlTxSwing)(void* pD, CPDI_EnableOrDisable enOrDisen);

    /**
     * This function is used to access the status of the various error
     * and abnormal conditions in the core. Any of the status bits can be
     * reset by writing a 1 into the bit position. This register does not
     * capture any errors signaled by remote devices using PCIe error
     * messages when the core is operating in the RP mode. Unless masked
     * by the setting of the Local Interrupt Mask Register, the
     * occurrence of any of these conditions causes the core to activate
     * the LOCAL_INTERRUPT output.
     * @param[in] maskOrUnmask Whether the parameter needs to be masked or unmasked
     * @param[in] maskableIntrParam The parameter that needs to be controlled
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 if function comepleted successfully
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*controlMaskingOfLocalInterrupts)(void* pD, CPDI_MaskableLocalInterruptConditions maskableIntrParam, CPDI_MaskOrUnmask maskOrUnmask);

    /**
     * This function is used to access the status of the various error
     * and abnormal conditions in the core. Any of the status bits can be
     * reset by writing a 1 into the bit position. This function does not
     * report any errors signaled by remote devices using PCIe error
     * messages when the core is operating in the RP mode. Unless masked
     * by the setting of the Local Interrupt Mask Register, the
     * occurrence of any of these conditions causes the core to activate
     * the LOCAL_INTERRUPT output.
     * @param[out] errorType The type of the error if found
     * @param[out] localErrorsPresent True if there are local errors, false otherwise
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 on training complete
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*areThereLocalErrors)(void* pD, CPDI_Bool* localErrorsPresent, uint32_t* errorType);

    /**
     * This function is used to control the reporting of status of the
     * various error and abnormal conditions in the core. Any of the
     * status bits can be reset by writing a 1 into the bit position.
     * This function does not report any errors signaled by remote
     * devices using PCIe error messages when the core is operating in
     * the RP mode. Unless masked by the setting of the Local Interrupt
     * Mask Register, the occurrence of any of these conditions causes
     * the core to activate the LOCAL_INTERRUPT output.
     * @param[in,out] localError True if an error exists on the condition, false otherwise
     * @param[in] pD Pointer to the private data initialized by init
     * @param[in] errorCond Is there an error for this condition
     * @return 0 on training complete
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*isLocalError)(void* pD, CPDI_MaskableLocalInterruptConditions errorCond, CPDI_Bool* localError);

    /**
     * This function is used to reset any Local Error conditions.
     * @param[in] maskableIntrParam Enum for maskable interrupt corresponding to the error to be reset
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 on success
     * @return EINVAL if the hardware was not found at the given location
     */
    uint32_t (*resetLocalErrorStatusCondition)(void* pD, CPDI_MaskableLocalInterruptConditions maskableIntrParam);

    /**
     * This function is used to update the traffic class for a region by
     * altering  outbound configuration descriptor 0. (User may want to
     * call setupObWrapperMemIoAccess API before calling this API.)
     * @param[in] regionNo Specifies which region is to be configured
     * @param[in] trafficClass Traffic class number to which the region has to be assigned.
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 on success
     * @return EINVAL if trafficClass is out of bounds
     */
    uint32_t (*updateObWrapperTrafficClass)(void* pD, uint8_t regionNo, uint8_t trafficClass);

    /**
     * This function is used to set the outbound configuration register
     * allocation when used for Memory and IO TLPs.
     * @param[in] regionNo Specifies which region is to be configured
     * @param[in] upperAddr Upper 32bits of address to apply for PCIe transaction Actual number of bits passed will depend on numPassedBits.
     * @param[in] numPassedBits Number of bits to pass through to AXI2HAL  One will be added to this value, i.e. numPassedBits+1 will be actually passed.
     * @param[in] pD Pointer to the private data initialized by init
     * @param[in] lowerAddr Lower 32bits of address to apply for PCIe transaction Lowest 8 bits ignored. Actual number of bits passed will depend on numPassedBits.
     * @return 0 on success
     * @return EINVAL if numPassedBits is out of bounds
     */
    uint32_t (*setupObWrapperMemIoAccess)(void* pD, uint8_t regionNo, uint8_t numPassedBits, uint32_t upperAddr, uint32_t lowerAddr);

    /**
     * This function is used to set the outbound configuration register
     * allocation when used for Message TLPs
     * @param[in] regionNo Specifies which region is to be configured
     * @param[in] upperAddr Upper bits of vendor defined message.  Should be zero for normal message.
     * @param[in] pD Pointer to the private data initialized by init
     * @param[in] lowerAddr Lower bits of vendor defined message.  Should be zero for normal message.
     * @return 0 on success
     * @return EINVAL if region number is out of bounds
     */
    uint32_t (*setupObWrapperMessageAccess)(void* pD, uint8_t regionNo, uint32_t upperAddr, uint32_t lowerAddr);

    /**
     * This function is used to set the outbound configuration register
     * allocation  when used for Configuration Transaction Layer Packets
     * (TLPs)
     * @param[in] regionNo Specifies which region is to be configured
     * @param[in] busdevfuncAddr Bus-device function address
     * @param[in] numPassedBits Number of bits to pass through to AXI2HAL  One will be added to this value, i.e. numPassedBits+1 will be actually passed.
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 on success
     * @return EINVAL if not configured as Root Port
     */
    uint32_t (*setupObWrapperConfigAccess)(void* pD, uint8_t regionNo, uint8_t numPassedBits, uint32_t busdevfuncAddr);

    /**
     * This function is used to set up inbound Root Port Address
     * Translation
     * @param[in] regionNo Specifies which region is to be configured
     * @param[in] upperAddr Upper 32bits of address to apply for SoC transaction
     * @param[in] numPassedBits Number of bits to pass through to AXI2HAL  One will be added to this value, i.e. numPassedBits+1 will be actually passed.
     * @param[in] pD Pointer to the private data initialized by init
     * @param[in] lowerAddr Lower 32bits of address to apply for SoC transaction Lowest 8bits ignored.
     * @return 0 on success
     * @return EINVAL if region number is out of bounds
     */
    uint32_t (*setupIbRootPortAddrTranslation)(void* pD, uint8_t regionNo, uint8_t numPassedBits, uint32_t upperAddr, uint32_t lowerAddr);

    /**
     * This function is used to read a value from the ECAM address space.
     * AXI region 0  must be initialized for configuration access before
     * calling this function.
     * @param[in] pD Pointer to the private data initialized by init
     * @param[out] val Value read at the specified location
     * @param[in] bus Bus number
     * @param[in] dev Device number
     * @param[in] func Function number
     * @param[in] offset Register offset
     * @param[in] size Size for the transaction, 1, 2 or 4 byte
     * @return 0 on success
     * @return EINVAL if parameters are out of bounds
     */
    uint32_t (*doConfigRead)(void* pD, uint32_t bus, uint32_t dev, uint32_t func, uint32_t offset, CPDI_SizeInBytes size, uint32_t* val);

    /**
     * This function is used to write a value in the ECAM address space.
     * AXI region 0  must be initialized for configuration access before
     * calling this function.
     * @param[in] pD Pointer to the private data initialized by init
     * @param[in] bus Bus number
     * @param[in] dev Device number
     * @param[in] func Function number
     * @param[in] offset Register offset
     * @param[in] data The data to be written
     * @param[in] size Size for the transaction, 1, 2 or 4 byte
     * @return 0 on success
     * @return EINVAL if parameters are out of bounds
     */
    uint32_t (*doConfigWrite)(void* pD, uint32_t bus, uint32_t dev, uint32_t func, uint32_t offset, CPDI_SizeInBytes size, uint32_t data);

    /**
     * This function is used to read a value from the ECAM address
     * spacefor ARI supported devices. AXI region 0 must be initialized
     * for configuration access before calling this function.
     * @param[in] pD Pointer to the private data initialized by init
     * @param[out] val Value read at the specified location
     * @param[in] bus Bus number
     * @param[in] func Function number
     * @param[in] offset Register offset
     * @param[in] size Size for the transaction, 1, 2 or 4 byte
     * @return 0 on success
     * @return EINVAL if parameters are out of bounds
     */
    uint32_t (*doAriConfigRead)(void* pD, uint32_t bus, uint32_t func, uint32_t offset, CPDI_SizeInBytes size, uint32_t* val);

    /**
     * This function is used to write a value in the ECAM address space
     * for ARI supported devices. AXI region 0  must be initialized for
     * configuration access before calling this function.
     * @param[in] pD Pointer to the private data initialized by init
     * @param[in] bus Bus number
     * @param[in] func Function number
     * @param[in] offset Register offset
     * @param[in] data The data to be written
     * @param[in] size Size for the transaction, 1, 2 or 4 byte
     * @return 0 on success
     * @return EINVAL if parameters are out of bounds
     */
    uint32_t (*doAriConfigWrite)(void* pD, uint32_t bus, uint32_t func, uint32_t offset, CPDI_SizeInBytes size, uint32_t data);

    /**
     * Enable a memory BAR on the EP using PCI Configuration Space. This
     * function is provided for convenience, and will program a 32bit
     * base address into the specified BAR on the EP,  providing that the
     * BAR is already configured with a suitable type and size.
     * @param[in] addr_lo Lower address
     * @param[in] bar Bar number
     * @param[in] pD Pointer to the private data initialized by init
     * @param[in] bus Bus number
     * @param[in] dev Device number
     * @param[in] func Function number
     * @return 0 on success
     * @return EINVAL the BAR configuration is not suitable
     */
    uint32_t (*enableBarAccess)(void* pD, uint32_t bus, uint32_t dev, uint32_t func, uint32_t bar, uint32_t addr_lo);

    /**
     * Read the aperture size of the Root Port memory BAR0 or BAR1.  This
     * function is provided as a  convenience to assist in test
     * preparation.
     * @param[out] barVal Aperture size (in bytes) read from the RP bar register.
     * @param[in] bar Bar number (only 2 bars are available on RP.)
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 on success
     * @return EINVAL if parameters are out of bounds
     */
    uint32_t (*getRootPortBAR)(void* pD, uint32_t bar, uint32_t* barVal);

    /**
     * Enable memory BAR0 or BAR1 on the RP using local management
     * registers. This function  is provided for convenience, and will
     * program a 32bit pr 64bit base address into the specified BAR,
     * providing that the BAR is already configured with a suitable type
     * and size.
     * @param[in] addr_lo Lower address
     * @param[in] addr_hi Upper address (if bar is configured as 64bit)
     * @param[in] bar Bar number (only 2 bars are available on RP.)
     * @param[in] pD Pointer to the private data initialized by init
     * @return 0 on success
     * @return EINVAL the BAR configuration is not suitable
     */
    uint32_t (*enableRpMemBarAccess)(void* pD, uint32_t bar, uint32_t addr_hi, uint32_t addr_lo);

} CPDI_OBJ;

/**
 * In order to access the CPDI APIs, the upper layer software must call
 * this global function to obtain the pointer to the driver object.
 * @return CPDI_OBJ* Driver Object Pointer
 */
extern CPDI_OBJ *CPDI_GetInstance(void);

/**
 *  @}
 */


#endif	/* _CPDI_H_ */
