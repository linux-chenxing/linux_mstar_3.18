/**********************************************************************
 * copyright (C) 2012-2014 Cadence Design Systems
 * All rights reserved.
 ***********************************************************************
 * cpd_core_driver.c
 *
 * Cadence PCIe Core Driver
 * Implementation of driver API functions
 ***********************************************************************/


#include <linux/types.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/device.h>

#include <linux/kernel.h>
#include <linux/pci.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <linux/fs.h>

#include <linux/module.h>
#include <linux/delay.h>

#include "cdn_errno.h"
#include "cdn_stdint.h"

#include "cpdi_int.h"
#include "cps_v2.h"
#include "pcie_top.h"
#include "cpdi_core_driver.h"

/****************************************************************************/
/*                                                                          */
/****************************************************************************/

#include "cdn_axi_wrapper_ob.h"
#include "cdn_axi_wrapper_ib.h"
#include "mstar_pcie_addrmap.h"

#define AXI_WRAPPER_TYPE0_WRITE  0xA
#define AXI_WRAPPER_TYPE1_WRITE  0xB
#define AXI_WRAPPER_MSG_WRITE    0xC
#define AXI_WRAPPER_V_MSG_WRITE  0xD
#define AXI_WRAPPER_IO_WRITE     0x6
#define AXI_WRAPPER_MEM_WRITE    0x2
#define AXI_WRAPPER_ECRC_SHIFT    21

#define AXI_REGS_IB_OFFSET               0x800
#define AXI_REGS_IB_WAW_OFFSET           0x810
#define MAX_AXI_IB_ROOTPORT_REGION_NUM   1
#define OB_REG_SIZE_SHIFT                5
#define IB_ROOT_PORT_REG_SIZE_SHIFT      3


extern uint32_t _mdrv_RC_ob_memio_read(void *pD, uint32_t offset, uint32_t *data);
extern uint32_t _mdrv_RC_ob_memio_write(void *pD, uint32_t offset, uint32_t data);


#ifdef ZEBU_ESL
/* see cps.h */
uint8_t CPS_UncachedRead8(volatile uint8_t* address) {
    return *address;
}

/* see cps.h */
uint16_t CPS_UncachedRead16(volatile uint16_t* address) {
    return *address;
}

/* see cps.h */
uint32_t CPS_UncachedRead32(volatile uint32_t* address) {

    uart_puts("C P S _ R e a d 3 2 :",22); uart_putc('\n');
    uart_write_U32_hex(address); uart_puts("= >", 4); uart_write_U32_hex(*address);  uart_putc('\n');

    return *address;
}

/* see cps.h */
void CPS_UncachedWrite8(volatile uint8_t* address, uint8_t value) {
    *address = value;
}

/* see cps.h */
void CPS_UncachedWrite16(volatile uint16_t* address, uint16_t value) {
    *address = value;
}

/* see cps.h */
void CPS_UncachedWrite32(volatile uint32_t* address, uint32_t value) {

    uart_puts("C P S _ W r i t e 3 2 :",24); uart_putc('\n');
    uart_write_U32_hex(address); uart_puts("= >", 4); uart_write_U32_hex(value);  uart_putc('\n');

    *address = value;
}

#else
/* see cps.h */
uint8_t CPS_UncachedRead8(volatile uint8_t* address) {
    return readb(address);
}

/* see cps.h */
uint16_t CPS_UncachedRead16(volatile uint16_t* address) {
    return readw(address);
}

/* see cps.h */
uint32_t CPS_UncachedRead32(volatile uint32_t* address) {
    return readl(address);
}

/* see cps.h */
void CPS_UncachedWrite8(volatile uint8_t* address, uint8_t value) {
    writeb (value, address);
}

/* see cps.h */
void CPS_UncachedWrite16(volatile uint16_t* address, uint16_t value) {
    writew (value, address);
}

/* see cps.h */
void CPS_UncachedWrite32(volatile uint32_t* address, uint32_t value) {
    writel (value, address);
}

#endif

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static uint32_t CPDI_CallErrorCallbackIfError (void * pD) {
    CPDI_Bool le;
    uint32_t et;

    if (! pD)
        return EINVAL;

    if (CPDI_AreThereLocalErrors (pD, & le, & et)) {
        return EINVAL;
    }

    if (le == CPDI_TRUE) /* Call the registered callback */
        if (((pvt_data_t *) pD)->p_ec_func) {
            ((pvt_data_t *) pD)->p_ec_func (et);
        }

    return 0;
}

/****************************************************************************/
/****************************************************************************/
/* PHYSICAL LAYER TRAINING INFORMATION                                      */
/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
/* CPDI_Is_Link_Training_Complete                                           */
/****************************************************************************/
uint32_t
CPDI_IsLinkTrainingComplete (void      *pD,
                             CPDI_Bool *pTrainingComplete) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;

    if (! pD)
        return EINVAL;
    if (! pTrainingComplete)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_pl_config_0_reg));

    if (PCIE_LM_ADDRMAP__PL_CONFIG_0_REG__LS__READ(regVal))
       *pTrainingComplete = CPDI_TRUE;
    else
       *pTrainingComplete = CPDI_FALSE;

    printk ("     CPDI_IsLinkTrainingComplete \n");
    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}


/****************************************************************************/
/* CPDI_Get_Link_Training_State                                             */
/****************************************************************************/
uint32_t
CPDI_GetLinkTrainingState (void            *pD,
                           CPDI_LtssmState *pLtssmState) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;
    uint8_t stateVal;

    if (! pD)
        return EINVAL;
    if (! pLtssmState)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_pl_config_0_reg));
    stateVal = PCIE_LM_ADDRMAP__PL_CONFIG_0_REG__LTSSM__READ(regVal);

    switch ((CPDI_LtssmState) stateVal) {
        case CPDI_DETECT_QUIET                  :
        case CPDI_DETECT_ACTIVE                 :
        case CPDI_CONFIGURATION_COMPLETE        :
        case CPDI_CONFIGURATION_IDLE            :
        case CPDI_RECOVERY_IDLE                 :
        case CPDI_L0                            :
        case CPDI_L1_ENTRY                      :
        case CPDI_L1_IDLE                       :
        case CPDI_L2_IDLE                       :
        case CPDI_DISABLED                      :
        case CPDI_LOOPBACK_ENTRY_MASTER         :
        case CPDI_LOOPBACK_ACTIVE_MASTER        :
        case CPDI_LOOPBACK_EXIT_MASTER          :
        case CPDI_LOOPBACK_ENTRY_SLAVE          :
        case CPDI_LOOPBACK_ACTIVE_SLAVE         :
        case CPDI_LOOPBACK_EXIT_SLAVE           :
        case CPDI_HOT_RESET                     :
        case CPDI_CONFIGURATION_LINKWIDTH_START :
        case CPDI_CONFIGURATION_LINKWIDTH_ACCEPT:
        case CPDI_CONFIGURATION_LANENUM_ACCEPT  :
        case CPDI_CONFIGURATION_LANENUM_WAIT    :
        case CPDI_RECOVERY_RCVRLOCK             :
        case CPDI_RECOVERY_SPEED                :
        case CPDI_RECOVERY_RCVRCFG              :
        case CPDI_POLLING_ACTIVE                :
        case CPDI_POLLING_COMPLIANCE            :
        case CPDI_POLLING_CONFIGURATION         :
        case CPDI_RX_L0S_ENTRY                  :
        case CPDI_RX_L0S_IDLE                   :
        case CPDI_RX_L0S_FTS                    :
        case CPDI_TX_L0S_ENTRY                  :
        case CPDI_TX_L0S_IDLE                   :
        case CPDI_TX_L0S_FTS                    :
        case CPDI_RECOVERY_EQUALIZATION_PHASE_0 :
        case CPDI_RECOVERY_EQUALIZATION_PHASE_1 :
        case CPDI_RECOVERY_EQUALIZATION_PHASE_2 :
        case CPDI_RECOVERY_EQUALIZATION_PHASE_3 :
        case CPDI_L2_TRANSMITWAKE               :
         break;
       default: return EINVAL;
    }
    *pLtssmState = (CPDI_LtssmState) stateVal;

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************/
/* CPDI_Get_Link_Training_Direction                                         */
/****************************************************************************/
uint32_t
CPDI_GetLinkTrainingDirection (void                      *pD,
                               CPDI_UpstreamOrDownstream *pUpOrDown) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;

    if (! pD)
        return EINVAL;
    if (! pUpOrDown)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_pl_config_0_reg));

    if (PCIE_LM_ADDRMAP__PL_CONFIG_0_REG__LTD__READ(regVal))
        *pUpOrDown = CPDI_DOWNSTREAM;
    else
        *pUpOrDown = CPDI_UPSTREAM;

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************/
/* CPDI_Is_Core_Strapped_As_EP_Or_RP                                        */
/****************************************************************************/
uint32_t
CPDI_IsCoreStrappedAsEpOrRp (void            *pD,
                             CPDI_CoreEpOrRp *pEpOrRp) {



    if (! pD)
        return EINVAL;
    if (! pEpOrRp)
        return EINVAL;



        *pEpOrRp = CPDI_CORE_RP_STRAPPED;


    return 0;
}

/****************************************************************************/
/****************************************************************************/
/* PHYSICAL LAYER LANE COUNT AND LINK SPEED INFORMATION                     */
/****************************************************************************/
/****************************************************************************/



/****************************************************************************/
/* CPDI_Get_Negotiated_Link_Speed                                           */
/****************************************************************************/
uint32_t
CPDI_GetNegotiatedLinkSpeed (void           *pD,
                             CPDI_LinkSpeed *ns) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;
    uint8_t  ls;

    if (! pD)
        return EINVAL;
    if (! ns)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_pl_config_0_reg));
    ls = PCIE_LM_ADDRMAP__PL_CONFIG_0_REG__NS__READ(regVal);

    switch (ls) {
        case 0 : *ns = CPDI_SPEED_2p5_GB; break;
        case 1 : *ns = CPDI_SPEED_5_GB  ; break;
        case 2 : *ns = CPDI_SPEED_8_GB  ; break;
        default: return EINVAL;
    }

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}



/****************************************************************************/
/****************************************************************************/
/* PHYSICAL LAYER REMOTE INFO RECEIVED DURING TRAINING                      */
/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
/* CPDI_Get_Received_Link_ID                                                */
/****************************************************************************/
uint32_t
CPDI_GetReceivedLinkId (void     *pD,
                        uint32_t *pLinkId) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;

    if (! pD)
        return EINVAL;
    if (! pLinkId)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_pl_config_0_reg));

    *pLinkId = (uint32_t) PCIE_LM_ADDRMAP__PL_CONFIG_0_REG__RLID__READ(regVal);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}


/****************************************************************************/
/* CPDI_Is_Remote_Linkwidth_Upconfigurable                                  */
/****************************************************************************/
uint32_t
CPDI_IsRemoteLinkwidthUpconfigurable (void      *pD,
                                      CPDI_Bool *upconfig) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;

    if (! pD)
        return EINVAL;
    if (! upconfig)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_pl_config_0_reg));

    if (PCIE_LM_ADDRMAP__PL_CONFIG_0_REG__R0__READ(regVal))
       *upconfig = CPDI_TRUE;
    else
       *upconfig = CPDI_FALSE;

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}



/****************************************************************************/
/****************************************************************************/
/* PHYSICAL_LAYER INFO SENT DURING TRAINING                                 */
/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
/* CPDI_Access_Transmitted_Link_ID                                          */
/****************************************************************************/
uint32_t
CPDI_AccessTransmittedLinkId (
                         void                 *pD,
                         CPDI_ReadOrWrite      rdOrWr,
                         uint8_t              *pLinkId) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;

    if (! pD)
        return EINVAL;
    if (! pLinkId)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_pl_config_1_reg));

    switch (rdOrWr) {

        case CPDI_DO_READ:

            *pLinkId =
                       PCIE_LM_ADDRMAP__PL_CONFIG_1_REG__TLI__READ(regVal);
            break;

        case CPDI_DO_WRITE:

            PCIE_LM_ADDRMAP__PL_CONFIG_1_REG__TLI__MODIFY(regVal,*pLinkId);
            CPDI_REG_WR (&(pcieAddr->i_pl_config_1_reg), regVal);

            break;

        default: return EINVAL;
    }

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************/
/****************************************************************************/
/* VENDOR AND SUBSYSTEM VENDOR IDs                                          */
/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
/* CPDI_Access_Vendor_ID_Subsystem_Vendor_ID                                */
/****************************************************************************/
uint32_t CPDI_AccessVendorIdSubsystemVendorId (
                         void                  *pD,
                         CPDI_AdvertisedIdType  id,
                         CPDI_ReadOrWrite       rdOrWr,
                         uint16_t              *pRdWrVal) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;

    if (! pD)
        return EINVAL;
    if (! pRdWrVal)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    switch (id) {

        case CPDI_VENDOR_ID: {

            uint32_t regVal = (uint32_t)
                          CPDI_REG_RD (&(pcieAddr->i_vendor_id_reg));

            switch (rdOrWr) {
                case CPDI_DO_READ:
                    *pRdWrVal = PCIE_LM_ADDRMAP__VENDOR_ID_REG__VID__READ(regVal);
                    break;
                case CPDI_DO_WRITE:
                    PCIE_LM_ADDRMAP__VENDOR_ID_REG__VID__MODIFY(regVal,*pRdWrVal);

                    CPDI_REG_WR (&(pcieAddr->i_vendor_id_reg), regVal);
                    break;
                default: return EINVAL;
            }
        }
        break;

        case CPDI_SUBSYSTEM_VENDOR_ID: {

            uint32_t regVal =  (uint32_t)
                          CPDI_REG_RD (&(pcieAddr->i_vendor_id_reg));

            switch (rdOrWr) {
                case CPDI_DO_READ:
                    *pRdWrVal = PCIE_LM_ADDRMAP__VENDOR_ID_REG__SVID__READ(regVal);
                    break;
                case CPDI_DO_WRITE:
                    PCIE_LM_ADDRMAP__VENDOR_ID_REG__SVID__MODIFY(regVal,*pRdWrVal);

                    CPDI_REG_WR (&(pcieAddr->i_vendor_id_reg), regVal);
                    break;
                default: return EINVAL;
            }
        }
        break;
        default:
           return EINVAL;
    }

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}


/****************************************************************************/
/****************************************************************************/
/* TIMING PARAMS INCLUDE TIMEOUTS, DELAYS, LATENCY SETTINGS AND SCALES      */
/****************************************************************************/
/****************************************************************************/

/****************************************************************************
   CPDI_Access_Timing_Params
*****************************************************************************/
uint32_t CPDI_AccessTimingParams (
                         void                           *pD,
                         CPDI_TimeoutAndDelayValues      timingVal,
                         CPDI_ReadOrWrite                rdOrWr,
                         uint32_t                       *pRdWrVal) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;

    if (! pD)
        return EINVAL;
    if (! pRdWrVal)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    switch (timingVal) {
        case CPDI_L0S_TIMEOUT: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_L0S_timeout_limit_reg));

                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pRdWrVal = PCIE_LM_ADDRMAP__L0S_TIMEOUT_LIMIT_REG__LT__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
/* Check if the value to be written is in valid range */
if((*pRdWrVal)>>PCIE_LM_ADDRMAP__L0S_TIMEOUT_LIMIT_REG__LT__WIDTH)
{
return EINVAL;
}
                        PCIE_LM_ADDRMAP__L0S_TIMEOUT_LIMIT_REG__LT__MODIFY(regVal,*pRdWrVal);
                        CPDI_REG_WR (&(pcieAddr->i_L0S_timeout_limit_reg), regVal);
                        break;
                    default: return EINVAL;
                }
            }
            break;
       case CPDI_COMPLETION_TIMEOUT_LIMIT_0: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_compln_tmout_lim_0_reg));

                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pRdWrVal = PCIE_LM_ADDRMAP__COMPLN_TMOUT_LIM_0_REG__CTL__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                      /* Check if the value to be written is in valid range */
                      if((*pRdWrVal)>>PCIE_LM_ADDRMAP__COMPLN_TMOUT_LIM_0_REG__CTL__WIDTH)
                      {
                        return EINVAL;
                      }
                        PCIE_LM_ADDRMAP__COMPLN_TMOUT_LIM_0_REG__CTL__MODIFY(regVal,*pRdWrVal);
                        CPDI_REG_WR (&(pcieAddr->i_compln_tmout_lim_0_reg), regVal);
                        break;
                    default: return EINVAL;
                }
            }
            break;

       case CPDI_COMPLETION_TIMEOUT_LIMIT_1: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_compln_tmout_lim_1_reg));

                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pRdWrVal = PCIE_LM_ADDRMAP__COMPLN_TMOUT_LIM_1_REG__CTL__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                      /* Check if the value to be written is in valid range */
                      if((*pRdWrVal)>>PCIE_LM_ADDRMAP__COMPLN_TMOUT_LIM_1_REG__CTL__WIDTH)
                      {
                        return EINVAL;
                      }
                        PCIE_LM_ADDRMAP__COMPLN_TMOUT_LIM_1_REG__CTL__MODIFY(regVal,*pRdWrVal);
                        CPDI_REG_WR (&(pcieAddr->i_compln_tmout_lim_1_reg), regVal);
                        break;
                    default: return EINVAL;
                }
            }
            break;

       case CPDI_L1_STATE_RETRY_DELAY: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_L1_st_reentry_delay_reg));

                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pRdWrVal = PCIE_LM_ADDRMAP__L1_ST_REENTRY_DELAY_REG__L1RD__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                        PCIE_LM_ADDRMAP__L1_ST_REENTRY_DELAY_REG__L1RD__MODIFY(regVal,*pRdWrVal);
                        CPDI_REG_WR (&(pcieAddr->i_L1_st_reentry_delay_reg), regVal);
                        break;
                    default: return EINVAL;
                }
            }
            break;

       case CPDI_ASPM_L1_ENTRY_TIMEOUT_DELAY: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_aspm_L1_entry_tmout_delay_reg));

                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pRdWrVal = PCIE_LM_ADDRMAP__ASPM_L1_ENTRY_TMOUT_DELAY_REG__L1T__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                      /* Check if the value to be written is in valid range */
                      if((*pRdWrVal)>>PCIE_LM_ADDRMAP__ASPM_L1_ENTRY_TMOUT_DELAY_REG__L1T__WIDTH)
                      {
                        return EINVAL;
                      }
                        PCIE_LM_ADDRMAP__ASPM_L1_ENTRY_TMOUT_DELAY_REG__L1T__MODIFY(regVal,*pRdWrVal);
                        CPDI_REG_WR (&(pcieAddr->i_aspm_L1_entry_tmout_delay_reg), regVal);
                        break;
                    default: return EINVAL;
                }
            }
            break;

       case CPDI_PME_TURNOFF_ACK_DELAY: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_pme_turnoff_ack_delay_reg));

                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pRdWrVal = PCIE_LM_ADDRMAP__PME_TURNOFF_ACK_DELAY_REG__PTOAD__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                      /* Check if the value to be written is in valid range */
                      if((*pRdWrVal)>>PCIE_LM_ADDRMAP__PME_TURNOFF_ACK_DELAY_REG__PTOAD__WIDTH)
                      {
                        return EINVAL;
                      }
                        PCIE_LM_ADDRMAP__PME_TURNOFF_ACK_DELAY_REG__PTOAD__MODIFY(regVal,*pRdWrVal);
                        CPDI_REG_WR (&(pcieAddr->i_pme_turnoff_ack_delay_reg), regVal);
                        break;
                    default: return EINVAL;
                }
            }
            break;

       case CPDI_TRANSMIT_SIDE_REPLAY_TIMEOUT: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_dll_tmr_config_reg));

                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pRdWrVal = PCIE_LM_ADDRMAP__DLL_TMR_CONFIG_REG__TSRT__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                      /* Check if the value to be written is in valid range */
                      if((*pRdWrVal)>>PCIE_LM_ADDRMAP__DLL_TMR_CONFIG_REG__TSRT__WIDTH)
                      {
                        return EINVAL;
                      }
                        PCIE_LM_ADDRMAP__DLL_TMR_CONFIG_REG__TSRT__MODIFY(regVal,*pRdWrVal);
                        CPDI_REG_WR (&(pcieAddr->i_dll_tmr_config_reg), regVal);
                        break;
                    default: return EINVAL;
                }
            }
            break;

        case CPDI_RECEIVE_SIDE_ACK_NACK_REPLAY_TIMEOUT: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_dll_tmr_config_reg));

                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pRdWrVal = PCIE_LM_ADDRMAP__DLL_TMR_CONFIG_REG__RSART__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                      /* Check if the value to be written is in valid range */
                      if((*pRdWrVal)>>PCIE_LM_ADDRMAP__DLL_TMR_CONFIG_REG__RSART__WIDTH)
                      {
                        return EINVAL;
                      }
                        PCIE_LM_ADDRMAP__DLL_TMR_CONFIG_REG__RSART__MODIFY(regVal,*pRdWrVal);
                        CPDI_REG_WR (&(pcieAddr->i_dll_tmr_config_reg), regVal);
                        break;
                    default: return EINVAL;
                }
            }
            break;




            default: return EINVAL;

    }

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}
/****************************************************************************/
/* CPDI_Access_L0sTimeout                                          */
/****************************************************************************/
uint32_t CPDI_AccessL0sTimeout (
                         void                 *pD,
                         CPDI_ReadOrWrite      rdOrWr,
                         uint32_t             *pRdWrVal) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;

    if (! pD)
        return EINVAL;
    if (! pRdWrVal)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_L0S_timeout_limit_reg));

    switch (rdOrWr) {

        case CPDI_DO_READ:

            *pRdWrVal = PCIE_LM_ADDRMAP__L0S_TIMEOUT_LIMIT_REG__LT__READ(regVal);
            break;

        case CPDI_DO_WRITE:
          /* Check if the value to be written is in valid range */
          if((*pRdWrVal)>>PCIE_LM_ADDRMAP__L0S_TIMEOUT_LIMIT_REG__LT__WIDTH)
          {
            return EINVAL;
          }
            PCIE_LM_ADDRMAP__L0S_TIMEOUT_LIMIT_REG__LT__MODIFY(regVal,*pRdWrVal);
            CPDI_REG_WR (&(pcieAddr->i_L0S_timeout_limit_reg), regVal);
            break;

        default: return EINVAL;
    }

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;
    return 0;
}

/****************************************************************************/
/****************************************************************************/
/* TRANSITION INTO L2 / L0S                                                 */
/****************************************************************************/
/****************************************************************************/


/****************************************************************************/
/* CPDI_DisableRpTransitionToL0s                                */
/****************************************************************************/
uint32_t CPDI_DisableRpTransitionToL0s (
                          void                 *pD) {
    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;
    CPDI_CoreEpOrRp epOrRp;

    if (! pD)
        return EINVAL;

    if (CPDI_IsCoreStrappedAsEpOrRp (pD, & epOrRp) || epOrRp == CPDI_CORE_EP_STRAPPED )
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_L0S_timeout_limit_reg));

    /* Write a 0 here to disable the transition into L0S permanently */
    PCIE_LM_ADDRMAP__L0S_TIMEOUT_LIMIT_REG__LT__MODIFY(regVal,0);

    CPDI_REG_WR (&(pcieAddr->i_L0S_timeout_limit_reg), regVal);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;
    return 0;
}





/****************************************************************************/
/****************************************************************************/
/* TRANSMIT AND RECEIVE CREDIT LIMITS AND UPDATE INTERVAL ROUTINES          */
/****************************************************************************/
/****************************************************************************/


/* Local function returning the address of the receive register required
 *  The function return 0 ,for invalid input*/
static uintptr_t  rcvClRegArrayfunc(void *pD, uint8_t vcNum, uint8_t regNum){

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    /* value of regNum should be either 0 or 1 */
    if((regNum!=0) &&(regNum!=1)){
        return 0;
    }
    switch (vcNum) {
        case 0:
            return (uintptr_t)((regNum==0) ? (&(pcieAddr->i_rcv_cred_lim_0_reg)):
                    (&(pcieAddr->i_rcv_cred_lim_1_reg)));

       default:
           return 0;
    }
}

/* Local function returning the address of the transmit register required
 * The function return 0 ,for invalid input */
static uintptr_t transmClRegArrayfunc(void *pD, uint8_t vcNum, uint8_t regNum){

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;
    switch (vcNum) {
        case 0:
            return (uintptr_t)((regNum==0)? (&(pcieAddr->i_transm_cred_lim_0_reg)):
                    (&(pcieAddr->i_transm_cred_lim_1_reg)));
       default:
           return 0;
    }
}



/****************************************************************************/
/* CPDI_Access_Credit_Limit_Settings                                        */
/****************************************************************************/
uint32_t CPDI_AccessCreditLimitSettings (
                          void                       *pD,
                          CPDI_CreditLimitTypes       creditLimitParam,
                          CPDI_TransmitOrReceive      transmitOrReceive,
                          CPDI_ReadOrWrite            rdOrWr,
                          uint8_t                     vcNum,
                          uint32_t                   *pRdWrVal) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    cred_lim_setting cl_setting;

    if (! pD)
        return EINVAL;
    if (! pRdWrVal)
        return EINVAL;
    if(vcNum >= 1)   // vcNum should be less than total virtual channels allowed for this configuration.
        return EINVAL;
    if((rdOrWr>CPDI_DO_WRITE) ||(transmitOrReceive>CPDI_TRANSMIT)||
            (creditLimitParam>CPDI_COMPLETION_HEADER_CREDIT)){
        return EINVAL;
    }

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    cl_setting  = (creditLimitParam & 0x0f)<< 8 ;
    cl_setting |= (transmitOrReceive & 0x0f)<< 4;
    cl_setting |= (rdOrWr & 0x0f);

    switch (cl_setting) {

        case CPD_PPC_TRANSM_READ: {
                uint32_t regVal =  (uint32_t) CPDI_REG_RD (transmClRegArrayfunc(pD, vcNum,0));
                switch(vcNum){
                             case 0 :
                                 *pRdWrVal = PCIE_LM_ADDRMAP__TRANSM_CRED_LIM_0_REG_VC0__PPC__READ(regVal);
                                  break;
                             default:
                                 return EINVAL;
                }
                break;
        }

        case CPD_PPC_RCV_READ: {
            uint32_t regVal =  (uint32_t) CPDI_REG_RD (rcvClRegArrayfunc(pD, vcNum,0));
            switch(vcNum){
                case 0 :
                    *pRdWrVal = PCIE_LM_ADDRMAP__RCV_CRED_LIM_0_REG_VC0__PPC__READ(regVal);
                    break;
                default:
                    return EINVAL;
            }
            break;
        }

        case CPD_PPC_RCV_WRITE: {
            uint32_t regVal =  (uint32_t) CPDI_REG_RD (rcvClRegArrayfunc(pD, vcNum,0));
            switch(vcNum){
                         case 0 :
                 /* Check if the value to be written is within valid range*/
                 if((*pRdWrVal)>> PCIE_LM_ADDRMAP__RCV_CRED_LIM_0_REG_VC0__PPC__WIDTH)
                 {
                     return EINVAL;
                 }
                 PCIE_LM_ADDRMAP__RCV_CRED_LIM_0_REG_VC0__PPC__MODIFY(regVal,*pRdWrVal);
                 break;
              default:
                  return EINVAL;
             }
             CPDI_REG_WR (rcvClRegArrayfunc(pD, vcNum,0), regVal);
             break;
        }

        case CPD_PHC_TRANSM_READ: {
            uint32_t regVal =  (uint32_t) CPDI_REG_RD (transmClRegArrayfunc(pD, vcNum,0));

            switch(vcNum){
                case 0 :
                    *pRdWrVal = PCIE_LM_ADDRMAP__TRANSM_CRED_LIM_0_REG_VC0__PHC__READ(regVal);
                    break;
                default:
                   return EINVAL;
            }
            break;
        }

        case CPD_PHC_RCV_READ: {
            uint32_t regVal =  (uint32_t) CPDI_REG_RD (rcvClRegArrayfunc(pD, vcNum,0));
            switch(vcNum){
                case 0 :
                    *pRdWrVal = PCIE_LM_ADDRMAP__RCV_CRED_LIM_0_REG_VC0__PHC__READ(regVal);
                    break;
                default:
                    return EINVAL;
            }
            break;
        }

        case CPD_PHC_RCV_WRITE: {
            uint32_t regVal =  (uint32_t) CPDI_REG_RD (rcvClRegArrayfunc(pD, vcNum,0));
            switch(vcNum){
                         case 0 :
                 /* Check if the value to be written is within valid range*/
                 if((*pRdWrVal)>> PCIE_LM_ADDRMAP__RCV_CRED_LIM_0_REG_VC0__PHC__WIDTH)
                 {
                     return EINVAL;
                 }
                 PCIE_LM_ADDRMAP__RCV_CRED_LIM_0_REG_VC0__PHC__MODIFY(regVal,*pRdWrVal);
                 break;
              default:
                  return EINVAL;
             }
             CPDI_REG_WR (rcvClRegArrayfunc(pD, vcNum,0), regVal);
             break;
        }

        case CPD_NPPC_TRANSM_READ: {
            uint32_t regVal =  (uint32_t) CPDI_REG_RD (transmClRegArrayfunc(pD, vcNum,0));

            switch(vcNum){
                case 0 :
                    *pRdWrVal = PCIE_LM_ADDRMAP__TRANSM_CRED_LIM_0_REG_VC0__NPPC__READ(regVal);
                    break;
                default:
                   return EINVAL;
            }
            break;
        }

        case CPD_NPPC_RCV_READ: {
            uint32_t regVal =  (uint32_t) CPDI_REG_RD (rcvClRegArrayfunc(pD, vcNum,0));
            switch(vcNum){
                case 0 :
                    *pRdWrVal = PCIE_LM_ADDRMAP__RCV_CRED_LIM_0_REG_VC0__NPPC__READ(regVal);
                    break;
                default:
                    return EINVAL;
            }
            break;
        }

        case CPD_NPPC_RCV_WRITE: {
            uint32_t regVal =  (uint32_t) CPDI_REG_RD (rcvClRegArrayfunc(pD, vcNum,0));
            switch(vcNum){
                         case 0 :
                 /* Check if the value to be written is within valid range*/
                 if((*pRdWrVal)>> PCIE_LM_ADDRMAP__RCV_CRED_LIM_0_REG_VC0__NPPC__WIDTH)
                 {
                     return EINVAL;
                 }
                 PCIE_LM_ADDRMAP__RCV_CRED_LIM_0_REG_VC0__NPPC__MODIFY(regVal,*pRdWrVal);
                 break;
              default:
                  return EINVAL;
             }
             CPDI_REG_WR (rcvClRegArrayfunc(pD, vcNum,0), regVal);
             break;
        }
        case CPD_NPHC_TRANSM_READ: {
             uint32_t regVal =  (uint32_t) CPDI_REG_RD (transmClRegArrayfunc(pD, vcNum,1));

             switch(vcNum){
                 case 0 :
                     *pRdWrVal = PCIE_LM_ADDRMAP__TRANSM_CRED_LIM_1_REG_VC0__NPHC__READ(regVal);
                     break;
                 default:
                    return EINVAL;
             }
             break;
         }

         case CPD_NPHC_RCV_READ: {
             uint32_t regVal =  (uint32_t) CPDI_REG_RD (rcvClRegArrayfunc(pD, vcNum,1));
             switch(vcNum){
                 case 0 :
                     *pRdWrVal = PCIE_LM_ADDRMAP__RCV_CRED_LIM_1_REG_VC0__NPHCL__READ(regVal);
                     break;
                 default:
                     return EINVAL;
             }
             break;
         }

         case CPD_NPHC_RCV_WRITE: {
             uint32_t regVal =  (uint32_t) CPDI_REG_RD (rcvClRegArrayfunc(pD, vcNum,1));
             switch(vcNum){
                          case 0 :
                  /* Check if the value to be written is within valid range*/
                  if((*pRdWrVal)>> PCIE_LM_ADDRMAP__RCV_CRED_LIM_1_REG_VC0__NPHCL__WIDTH)
                  {
                      return EINVAL;
                  }
                  PCIE_LM_ADDRMAP__RCV_CRED_LIM_1_REG_VC0__NPHCL__MODIFY(regVal,*pRdWrVal);
                  break;
               default:
                   return EINVAL;
              }
              CPDI_REG_WR (rcvClRegArrayfunc(pD, vcNum,1), regVal);
              break;
         }

         case CPD_CPC_TRANSM_READ: {
              uint32_t regVal =  (uint32_t) CPDI_REG_RD (transmClRegArrayfunc(pD, vcNum,1));

              switch(vcNum){
                  case 0 :
                      *pRdWrVal = PCIE_LM_ADDRMAP__TRANSM_CRED_LIM_1_REG_VC0__CPC__READ(regVal);
                      break;
                  default:
                     return EINVAL;
              }
              break;
          }

          case CPD_CPC_RCV_READ: {
              uint32_t regVal =  (uint32_t) CPDI_REG_RD (rcvClRegArrayfunc(pD, vcNum,1));
              switch(vcNum){
                  case 0 :
                      *pRdWrVal = PCIE_LM_ADDRMAP__RCV_CRED_LIM_1_REG_VC0__CPC__READ(regVal);
                      break;
                  default:
                      return EINVAL;
              }
              break;
          }

          case CPD_CPC_RCV_WRITE: {
              uint32_t regVal =  (uint32_t) CPDI_REG_RD (rcvClRegArrayfunc(pD, vcNum,1));
              switch(vcNum){
                           case 0 :
                   /* Check if the value to be written is within valid range*/
                   if((*pRdWrVal)>> PCIE_LM_ADDRMAP__RCV_CRED_LIM_1_REG_VC0__CPC__WIDTH)
                   {
                       return EINVAL;
                   }
                   PCIE_LM_ADDRMAP__RCV_CRED_LIM_1_REG_VC0__CPC__MODIFY(regVal,*pRdWrVal);
                   break;
                default:
                    return EINVAL;
               }
               CPDI_REG_WR (rcvClRegArrayfunc(pD, vcNum,1), regVal);
               break;
          }

          case CPD_CHC_TRANSM_READ: {
              uint32_t regVal =  (uint32_t) CPDI_REG_RD (transmClRegArrayfunc(pD, vcNum,1));

              switch(vcNum){
                  case 0 :
                      *pRdWrVal = PCIE_LM_ADDRMAP__TRANSM_CRED_LIM_1_REG_VC0__CHC__READ(regVal);
                      break;
                  default:
                     return EINVAL;
              }
              break;
          }

          case CPD_CHC_RCV_READ: {
              uint32_t regVal =  (uint32_t) CPDI_REG_RD (rcvClRegArrayfunc(pD, vcNum,1));
              switch(vcNum){
                  case 0 :
                      *pRdWrVal = PCIE_LM_ADDRMAP__RCV_CRED_LIM_1_REG_VC0__CHC__READ(regVal);
                      break;
                  default:
                      return EINVAL;
              }
              break;
          }

          case CPD_CHC_RCV_WRITE: {
              uint32_t regVal =  (uint32_t) CPDI_REG_RD (rcvClRegArrayfunc(pD, vcNum,1));
              switch(vcNum){
                           case 0 :
                   /* Check if the value to be written is within valid range*/
                   if((*pRdWrVal)>> PCIE_LM_ADDRMAP__RCV_CRED_LIM_1_REG_VC0__CHC__WIDTH)
                   {
                       return EINVAL;
                   }
                   PCIE_LM_ADDRMAP__RCV_CRED_LIM_1_REG_VC0__CHC__MODIFY(regVal,*pRdWrVal);
                   break;
                default:
                    return EINVAL;
               }
               CPDI_REG_WR (rcvClRegArrayfunc(pD, vcNum,1), regVal);
               break;
          }
          default:
              return EINVAL;
    }

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************/
/* CPDI_Access_Transmit_Credit_Update_Interval_Settings                     */
/****************************************************************************/
uint32_t CPDI_AccessTransmitCreditUpdateIntervalSettings (
                          void                          *pD,
                          CPDI_CreditUpdateIntervals     creditUpdateIntervalParam,
                          CPDI_ReadOrWrite               rdOrWr,
                          uint32_t                      *pRdWrVal) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;

    if (! pD)
        return EINVAL;
    if (! pRdWrVal)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    switch (creditUpdateIntervalParam) {

       case CPDI_MIN_POSTED_CREDIT_UPDATE_INTERVAL: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_transm_cred_update_int_config_0_reg));

                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pRdWrVal = PCIE_LM_ADDRMAP__TRANSM_CRED_UPDATE_INT_CONFIG_0_REG__MPUI__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                      if((*pRdWrVal)>> PCIE_LM_ADDRMAP__TRANSM_CRED_UPDATE_INT_CONFIG_0_REG__MPUI__WIDTH)
                      {
                        return EINVAL;
                      }
                        PCIE_LM_ADDRMAP__TRANSM_CRED_UPDATE_INT_CONFIG_0_REG__MPUI__MODIFY(regVal,*pRdWrVal);
                        CPDI_REG_WR (&(pcieAddr->i_transm_cred_update_int_config_0_reg), regVal);
                        break;
                    default: return EINVAL;
                }
            }
            break;

       case CPDI_MIN_NONPOSTED_CREDIT_UPDATE_INTERVAL: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_transm_cred_update_int_config_0_reg));

                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pRdWrVal = PCIE_LM_ADDRMAP__TRANSM_CRED_UPDATE_INT_CONFIG_0_REG__MNUI__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                      if((*pRdWrVal)>> PCIE_LM_ADDRMAP__TRANSM_CRED_UPDATE_INT_CONFIG_0_REG__MNUI__WIDTH)
                      {
                        return EINVAL;
                      }
                        PCIE_LM_ADDRMAP__TRANSM_CRED_UPDATE_INT_CONFIG_0_REG__MNUI__MODIFY(regVal,*pRdWrVal);
                        CPDI_REG_WR (&(pcieAddr->i_transm_cred_update_int_config_0_reg), regVal);
                        break;
                    default: return EINVAL;
                }
            }
            break;

       case CPDI_MIN_COMPLETION_UPDATE_INERVAL: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_transm_cred_update_int_config_1_reg));

                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pRdWrVal = PCIE_LM_ADDRMAP__TRANSM_CRED_UPDATE_INT_CONFIG_1_REG__CUI__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                      if((*pRdWrVal)>> PCIE_LM_ADDRMAP__TRANSM_CRED_UPDATE_INT_CONFIG_1_REG__CUI__WIDTH)
                      {
                        return EINVAL;
                      }
                        PCIE_LM_ADDRMAP__TRANSM_CRED_UPDATE_INT_CONFIG_1_REG__CUI__MODIFY(regVal,*pRdWrVal);
                        CPDI_REG_WR (&(pcieAddr->i_transm_cred_update_int_config_1_reg), regVal);
                        break;
                    default: return EINVAL;
                }
            }
            break;

       case CPDI_MAX_UPDATE_INTERVAL_FOR_ALL: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_transm_cred_update_int_config_1_reg));

                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pRdWrVal = PCIE_LM_ADDRMAP__TRANSM_CRED_UPDATE_INT_CONFIG_1_REG__MUI__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                      if((*pRdWrVal)>> PCIE_LM_ADDRMAP__TRANSM_CRED_UPDATE_INT_CONFIG_1_REG__MUI__WIDTH)
                      {
                        return EINVAL;
                      }
                        PCIE_LM_ADDRMAP__TRANSM_CRED_UPDATE_INT_CONFIG_1_REG__MUI__MODIFY(regVal,*pRdWrVal);
                        CPDI_REG_WR (&(pcieAddr->i_transm_cred_update_int_config_1_reg), regVal);
                        break;
                    default: return EINVAL;
                }
            }
            break;
        default: return EINVAL;
    }

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}


/****************************************************************************/
/****************************************************************************/
/* BAR APERTURE AND CONTROL CONFIGURATION                                   */
/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
/*     Process_Physical_Func_Aperture                                       */
/****************************************************************************/
uint32_t
ProcessPhysicalFuncAperture (
                      void                 *pD,
                      CPDI_BarNumber        barNumber,
                      uint32_t              funcNumber,
                      volatile uint32_t    *reg_0,
                      volatile uint32_t    *reg_1,
                      CPDI_ReadOrWrite      rdOrWr,
                      CPDI_BarApertureSize *pApertureSize) {

    switch (barNumber) {

        case CPDI_BAR_0: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (reg_0);
                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pApertureSize = PCIE_LM_ADDRMAP__PF_BAR_CONFIG_0_REG__BAR0A__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                        PCIE_LM_ADDRMAP__PF_BAR_CONFIG_0_REG__BAR0A__MODIFY(regVal,*pApertureSize);
                        CPDI_REG_WR (reg_0, regVal);
                        break;
                    default: return EINVAL;
                 }
            }
            break;

        case CPDI_BAR_1: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (reg_0);
                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pApertureSize = PCIE_LM_ADDRMAP__PF_BAR_CONFIG_0_REG__BAR1A__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                        PCIE_LM_ADDRMAP__PF_BAR_CONFIG_0_REG__BAR1A__MODIFY(regVal,*pApertureSize);
                        CPDI_REG_WR (reg_0, regVal);
                        break;
                    default: return EINVAL;
                 }
            }
            break;

        case CPDI_BAR_2: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (reg_0);
                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pApertureSize = PCIE_LM_ADDRMAP__PF_BAR_CONFIG_0_REG__BAR2A__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                        PCIE_LM_ADDRMAP__PF_BAR_CONFIG_0_REG__BAR2A__MODIFY(regVal,*pApertureSize);
                        CPDI_REG_WR (reg_0, regVal);
                        break;
                    default: return EINVAL;
                 }
            }
            break;

        case CPDI_BAR_3: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (reg_0);
                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pApertureSize = PCIE_LM_ADDRMAP__PF_BAR_CONFIG_0_REG__BAR3A__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                        PCIE_LM_ADDRMAP__PF_BAR_CONFIG_0_REG__BAR3A__MODIFY(regVal,*pApertureSize);
                        CPDI_REG_WR (reg_0, regVal);
                        break;
                    default: return EINVAL;
                 }
            }
            break;

        case CPDI_BAR_4: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (reg_1);

                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pApertureSize = PCIE_LM_ADDRMAP__PF_BAR_CONFIG_1_REG__BAR4A__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                        PCIE_LM_ADDRMAP__PF_BAR_CONFIG_1_REG__BAR4A__MODIFY(regVal,*pApertureSize);
                        CPDI_REG_WR (reg_1, regVal);
                        break;
                    default: return EINVAL;
                 }
            }
            break;

        case CPDI_BAR_5: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (reg_1);
                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pApertureSize = PCIE_LM_ADDRMAP__PF_BAR_CONFIG_1_REG__BAR5A__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                        PCIE_LM_ADDRMAP__PF_BAR_CONFIG_1_REG__BAR5A__MODIFY(regVal,*pApertureSize);
                        CPDI_REG_WR (reg_1, regVal);
                        break;
                    default: return EINVAL;
                 }
            }
            break;
        default: return EINVAL;
    }

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************/
/*     Process_Physical_Func_Control                                        */
/****************************************************************************/
uint32_t
ProcessPhysicalFuncControl (
                      void                 *pD,
                      CPDI_BarNumber        barNumber,
                      uint32_t              funcNumber,
                      volatile uint32_t    *reg_0,
                      volatile uint32_t    *reg_1,
                      CPDI_ReadOrWrite      rdOrWr,
                      CPDI_BarControl      *pBarControl) {
    switch (barNumber) {

        case CPDI_BAR_0: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (reg_0);
                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pBarControl = PCIE_LM_ADDRMAP__PF_BAR_CONFIG_0_REG__BAR0C__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                        PCIE_LM_ADDRMAP__PF_BAR_CONFIG_0_REG__BAR0C__MODIFY(regVal,*pBarControl);
                        CPDI_REG_WR (reg_0, regVal);
                        break;
                    default: return EINVAL;
                 }
            }
            break;

        case CPDI_BAR_1: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (reg_0);
                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pBarControl = PCIE_LM_ADDRMAP__PF_BAR_CONFIG_0_REG__BAR1C__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                        PCIE_LM_ADDRMAP__PF_BAR_CONFIG_0_REG__BAR1C__MODIFY(regVal,*pBarControl);
                        CPDI_REG_WR (reg_0, regVal);
                        break;
                    default: return EINVAL;
                 }
            }
            break;

        case CPDI_BAR_2: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (reg_0);
                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pBarControl = PCIE_LM_ADDRMAP__PF_BAR_CONFIG_0_REG__BAR2C__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                        PCIE_LM_ADDRMAP__PF_BAR_CONFIG_0_REG__BAR2C__MODIFY(regVal,*pBarControl);
                        CPDI_REG_WR (reg_0, regVal);
                        break;
                    default: return EINVAL;
                 }
            }
            break;

        case CPDI_BAR_3: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (reg_0);
                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pBarControl = PCIE_LM_ADDRMAP__PF_BAR_CONFIG_0_REG__BAR3C__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                        PCIE_LM_ADDRMAP__PF_BAR_CONFIG_0_REG__BAR3C__MODIFY(regVal,*pBarControl);
                        CPDI_REG_WR (reg_0, regVal);
                        break;
                    default: return EINVAL;
                 }
            }
            break;

        case CPDI_BAR_4: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (reg_1);
                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pBarControl = PCIE_LM_ADDRMAP__PF_BAR_CONFIG_1_REG__BAR4C__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                        PCIE_LM_ADDRMAP__PF_BAR_CONFIG_1_REG__BAR4C__MODIFY(regVal,*pBarControl);
                        CPDI_REG_WR (reg_1, regVal);
                        break;
                    default: return EINVAL;
                }
            }
            break;

        case CPDI_BAR_5: {

                uint32_t regVal =  (uint32_t) CPDI_REG_RD (reg_1);
                switch (rdOrWr) {
                    case CPDI_DO_READ:
                        *pBarControl = PCIE_LM_ADDRMAP__PF_BAR_CONFIG_1_REG__BAR5C__READ(regVal);
                        break;
                    case CPDI_DO_WRITE:
                        PCIE_LM_ADDRMAP__PF_BAR_CONFIG_1_REG__BAR5C__MODIFY(regVal,*pBarControl);
                        CPDI_REG_WR (reg_1, regVal);
                        break;
                    default: return EINVAL;
                 }
            }
            break;
        default: return EINVAL;
    }

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************/
/* CPDI_Access_Function_BAR_Aperture_Setting                                */
/****************************************************************************/
uint32_t CPDI_AccessFunctionBarApertureSetting (
                         void                      *pD,
                         uint32_t                   funcNumber,
                         CPDI_BarNumber             barNumber,
                         CPDI_ReadOrWrite           rdOrWr,
                         CPDI_BarApertureSize      *pApertureSize) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;

    if (! pD)
        return EINVAL;
    if (! pApertureSize)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    /* if write, make sure that the aperture size being written is valid.  Minimum size is 4K */

    if (rdOrWr ==CPDI_DO_WRITE) {
        switch (barNumber) {

            case CPDI_BAR_0:
            case CPDI_BAR_2:
            case CPDI_BAR_4:

                switch (*pApertureSize) {
                    case CPDI_APERTURE_SIZE_4K    :
                    case CPDI_APERTURE_SIZE_8K    : case CPDI_APERTURE_SIZE_16K   : case CPDI_APERTURE_SIZE_32K   :
                    case CPDI_APERTURE_SIZE_64K   : case CPDI_APERTURE_SIZE_128K  : case CPDI_APERTURE_SIZE_256K  :
                    case CPDI_APERTURE_SIZE_512K  : case CPDI_APERTURE_SIZE_1M    : case CPDI_APERTURE_SIZE_2M    :
                    case CPDI_APERTURE_SIZE_4M    : case CPDI_APERTURE_SIZE_8M    : case CPDI_APERTURE_SIZE_16M   :
                    case CPDI_APERTURE_SIZE_32M   : case CPDI_APERTURE_SIZE_64M   : case CPDI_APERTURE_SIZE_128M  :
                    case CPDI_APERTURE_SIZE_256M  : case CPDI_APERTURE_SIZE_512M  : case CPDI_APERTURE_SIZE_1G    :
                    case CPDI_APERTURE_SIZE_2G    : case CPDI_APERTURE_SIZE_4G    : case CPDI_APERTURE_SIZE_8G    :
                    case CPDI_APERTURE_SIZE_16G   : case CPDI_APERTURE_SIZE_32G   : case CPDI_APERTURE_SIZE_64G   :
                    case CPDI_APERTURE_SIZE_128G  : case CPDI_APERTURE_SIZE_256G  :
                        break;
                    default: return EINVAL;
                }
                break;

            case CPDI_BAR_1:
            case CPDI_BAR_3:
            case CPDI_BAR_5:

                switch (*pApertureSize) {
                    case CPDI_APERTURE_SIZE_4K    :
                    case CPDI_APERTURE_SIZE_8K    : case CPDI_APERTURE_SIZE_16K   : case CPDI_APERTURE_SIZE_32K   :
                    case CPDI_APERTURE_SIZE_64K   : case CPDI_APERTURE_SIZE_128K  : case CPDI_APERTURE_SIZE_256K  :
                    case CPDI_APERTURE_SIZE_512K  : case CPDI_APERTURE_SIZE_1M    : case CPDI_APERTURE_SIZE_2M    :
                    case CPDI_APERTURE_SIZE_4M    : case CPDI_APERTURE_SIZE_8M    : case CPDI_APERTURE_SIZE_16M   :
                    case CPDI_APERTURE_SIZE_32M   : case CPDI_APERTURE_SIZE_64M   : case CPDI_APERTURE_SIZE_128M  :
                    case CPDI_APERTURE_SIZE_256M  : case CPDI_APERTURE_SIZE_512M  : case CPDI_APERTURE_SIZE_1G    :
                    case CPDI_APERTURE_SIZE_2G    :
                        break;
                    default: return EINVAL;
                }
                break;
            default:
                return EINVAL;
        }
    }

    switch (funcNumber) {
        case 0:
            return ProcessPhysicalFuncAperture (
                                    pD,
                                    barNumber,
                                    funcNumber,
                                    &(pcieAddr->i_pf_0_BAR_config_0_reg),
                                    &(pcieAddr->i_pf_0_BAR_config_1_reg),
                                    rdOrWr,
                                    pApertureSize);
            break;
         default: return EINVAL;
    }
}

/****************************************************************************/
/* CPDI_Access_Function_BAR_Control_Setting                                 */
/****************************************************************************/
uint32_t CPDI_AccessFunctionBarControlSetting (
                         void                   *pD,
                         uint32_t                funcNumber,
                         CPDI_BarNumber          barNumber,
                         CPDI_ReadOrWrite        rdOrWr,
                         CPDI_BarControl        *pBarControl) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;

    if (! pD)
        return EINVAL;
    if (! pBarControl)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    /* if write, make sure that the control value being written is valid */

    if (rdOrWr ==CPDI_DO_WRITE) {
        switch (barNumber) {

            case CPDI_BAR_0:
            case CPDI_BAR_2:
            case CPDI_BAR_4:

                switch (*pBarControl) {
                    case CPDI_DISABLED_BAR                    :
                    case CPDI_IO_32_BIT_BAR                   :
                    case CPDI_NON_PREFETCHABLE_32_BIT_MEM_BAR :
                    case CPDI_PREFETCHABLE_32_BIT_MEM_BAR     :
                    case CPDI_NON_PREFETCHABLE_64_BIT_MEM_BAR :
                    case CPDI_PREFETCHABLE_64_BIT_MEM_BAR     : break;
                    default: return EINVAL;
                }
                break;

            case CPDI_BAR_1:
            case CPDI_BAR_3:
            case CPDI_BAR_5:

                switch (*pBarControl) {
                    case CPDI_DISABLED_BAR                    :
                    case CPDI_IO_32_BIT_BAR                   :
                    case CPDI_NON_PREFETCHABLE_32_BIT_MEM_BAR :
                    case CPDI_PREFETCHABLE_32_BIT_MEM_BAR     : break;
                    default: return EINVAL;
                }
                break;
            default:
                return EINVAL;
        }
    }
    switch (funcNumber) {
         case 0:
             return ProcessPhysicalFuncControl (
                                   pD,
                                   barNumber,
                                   funcNumber,
                                   &(pcieAddr->i_pf_0_BAR_config_0_reg),
                                   &(pcieAddr->i_pf_0_BAR_config_1_reg),
                                   rdOrWr,
                                   pBarControl);
            break;
         default: return EINVAL;
    }
}

/****************************************************************************/
/* CPDI_Access_Root_Port_BAR_Aperture_Setting                            */
/****************************************************************************/
uint32_t CPDI_AccessRootPortBarApertureSetting (
                         void                   *pD,
                         CPDI_RpBarNumber        barNumber,
                         CPDI_ReadOrWrite        rdOrWr,
                         CPDI_RpBarApertureSize *pApertureSize) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;

    CPDI_CoreEpOrRp epOrRp;

    if (! pD)
        return EINVAL;
    if (! pApertureSize)
        return EINVAL;

    if (CPDI_IsCoreStrappedAsEpOrRp (pD, & epOrRp) || epOrRp == CPDI_CORE_EP_STRAPPED )
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_rc_BAR_config_reg));

    if (rdOrWr ==CPDI_DO_READ) {
        switch (barNumber) {

            case CPDI_RP_BAR_0:

                *pApertureSize = (CPDI_RpBarApertureSize)
                           PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBAR0A__READ(regVal);
                break;

            case CPDI_RP_BAR_1:

                *pApertureSize = (CPDI_RpBarApertureSize)
                           PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBAR1A__READ(regVal);
                break;


            default: return EINVAL;

        }

        if (CPDI_CallErrorCallbackIfError (pD))
            return EINVAL;

        return 0;
    }

    /* can we support this aperture size for this BAR */
    switch (*pApertureSize) {
        case CPDI_RP_APERTURE_SIZE_4K    :
        case CPDI_RP_APERTURE_SIZE_8K    :
        case CPDI_RP_APERTURE_SIZE_16K   :
        case CPDI_RP_APERTURE_SIZE_32K   :
        case CPDI_RP_APERTURE_SIZE_64K   :
        case CPDI_RP_APERTURE_SIZE_128K  :
        case CPDI_RP_APERTURE_SIZE_256K  :
        case CPDI_RP_APERTURE_SIZE_512K  :
        case CPDI_RP_APERTURE_SIZE_1M    :
        case CPDI_RP_APERTURE_SIZE_2M    :
        case CPDI_RP_APERTURE_SIZE_4M    :
        case CPDI_RP_APERTURE_SIZE_8M    :
        case CPDI_RP_APERTURE_SIZE_16M   :
        case CPDI_RP_APERTURE_SIZE_32M   :
        case CPDI_RP_APERTURE_SIZE_64M   :
        case CPDI_RP_APERTURE_SIZE_128M  :
        case CPDI_RP_APERTURE_SIZE_256M  :
        case CPDI_RP_APERTURE_SIZE_512M  :
        case CPDI_RP_APERTURE_SIZE_1G    :
        case CPDI_RP_APERTURE_SIZE_2G    :
                                            break;
        case CPDI_RP_APERTURE_SIZE_4G    :
        case CPDI_RP_APERTURE_SIZE_8G    :
        case CPDI_RP_APERTURE_SIZE_16G   :
        case CPDI_RP_APERTURE_SIZE_32G   :
        case CPDI_RP_APERTURE_SIZE_64G   :
        case CPDI_RP_APERTURE_SIZE_128G  :
        case CPDI_RP_APERTURE_SIZE_256G  :
                                      if (barNumber == CPDI_RP_BAR_1)
                                            return EINVAL;
                                      else
                                            break;
        default: return EINVAL;
    }

    if (rdOrWr !=CPDI_DO_WRITE)
        return EINVAL;

    /* Write */
    switch (barNumber) {

        case CPDI_RP_BAR_0:

            PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBAR0A__MODIFY(regVal,*pApertureSize);
            break;

        case CPDI_RP_BAR_1:

            PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBAR1A__MODIFY(regVal,*pApertureSize);
            break;


        default: return EINVAL;

    }

    CPDI_REG_WR (&(pcieAddr->i_rc_BAR_config_reg), regVal);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************/
/* CPDI_Access_Root_Port_BAR_Control_Setting                             */
/****************************************************************************/
uint32_t CPDI_AccessRootPortBarControlSetting (
                         void                 *pD,
                         CPDI_RpBarNumber      barNumber,
                         CPDI_ReadOrWrite      rdOrWr,
                         CPDI_RpBarControl    *pRpBarControl) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    CPDI_CoreEpOrRp epOrRp;
    uint32_t regVal;

    if (! pD)
        return EINVAL;
    if (! pRpBarControl)
        return EINVAL;

    if (CPDI_IsCoreStrappedAsEpOrRp (pD, & epOrRp) || epOrRp == CPDI_CORE_EP_STRAPPED )
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_rc_BAR_config_reg));

    switch (barNumber) {

        case CPDI_RP_BAR_0:

            switch (rdOrWr) {
                case CPDI_DO_READ:
                    *pRpBarControl = (CPDI_RpBarControl)
                        PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBAR0C__READ(regVal);
                    break;
                case CPDI_DO_WRITE:
                    switch (*pRpBarControl){
                        case CPDI_RP_DISABLED_BAR:
                        case CPDI_RP_32_BIT_IO_BAR:
                        case CPDI_RP_TYPE_0_32_BIT_MEM_BAR:
                        case CPDI_RP_TYPE_0_64_BIT_MEM_BAR:
                        case CPDI_RP_TYPE_1_32_BIT_MEM_BAR:
                        case CPDI_RP_TYPE_1_64_BIT_MEM_BAR:
                            PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBAR0C__MODIFY(regVal, *pRpBarControl);
                            break;
                        default: return EINVAL;
                    }
                    break;
                default: return EINVAL;
            }
            break;

        case CPDI_RP_BAR_1:

            switch (rdOrWr) {
                case CPDI_DO_READ:
                    *pRpBarControl = (CPDI_RpBarControl)
                        PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBAR1C__READ(regVal);
                    break;
                case CPDI_DO_WRITE:
                    switch (*pRpBarControl){
                        case CPDI_RP_DISABLED_BAR:
                        case CPDI_RP_32_BIT_IO_BAR:
                        case CPDI_RP_TYPE_0_32_BIT_MEM_BAR:
                        case CPDI_RP_TYPE_1_32_BIT_MEM_BAR:
                            PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBAR1C__MODIFY(regVal, *pRpBarControl);
                            break;
                        default: return EINVAL;
                    }
                    break;
                default: return EINVAL;
            }
            break;


        default: return EINVAL;

    }

    if (rdOrWr ==CPDI_DO_WRITE)
        CPDI_REG_WR (&(pcieAddr->i_rc_BAR_config_reg), regVal);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}


/****************************************************************************/
/* CPDI_Access_Root_Port_Type1_Config_Setting                            */
/****************************************************************************/
uint32_t CPDI_AccessRootPortType1ConfigSetting (
                         void                       *pD,
                         CPDI_ReadOrWrite            rdOrWr,
                         CPDI_RpType1ConfigControl  *pRpType1Config) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    CPDI_CoreEpOrRp epOrRp;
    uint32_t regVal;
    uint8_t rpConfig;

    if (! pD)
        return EINVAL;
    if (! pRpType1Config)
        return EINVAL;

    if (CPDI_IsCoreStrappedAsEpOrRp (pD, & epOrRp) || epOrRp == CPDI_CORE_EP_STRAPPED )
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_rc_BAR_config_reg));

    switch (rdOrWr) {
        case CPDI_DO_READ:
            {
                rpConfig = (PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPME__READ(regVal)<<0) +
                            (PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPMS__READ(regVal)<<1) +
                            (PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIE__READ(regVal)<<2) +
                            (PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIS__READ(regVal)<<3);

                switch(rpConfig)
                {
                    case 0x00:
                    case 0x02:
                    case 0x08:
                    case 0x0a:
                        *pRpType1Config = CPDI_RP_TYPE1_DISABLED_ALL;
                        break;
                    case 0x1:
                    case 0x9:
                        *pRpType1Config = CPDI_RP_TYPE1_PREFETCH_32_BIT_IO_DISABLED;
                        break;
                    case 0x3:
                    case 0xb:
                        *pRpType1Config = CPDI_RP_TYPE1_PREFETCH_64_BIT_IO_DISABLED;
                        break;
                    case 0x4:
                    case 0x6:
                        *pRpType1Config = CPDI_RP_TYPE1_PREFETCH_DISABLED_IO_16_BIT;
                        break;
                    case 0x5:
                        *pRpType1Config = CPDI_RP_TYPE1_PREFETCH_32_BIT_IO_16_BIT;
                        break;
                    case 0x7:
                        *pRpType1Config = CPDI_RP_TYPE1_PREFETCH_64_BIT_IO_16_BIT;
                        break;
                    case 0xc:
                    case 0xe:
                        *pRpType1Config = CPDI_RP_TYPE1_PREFETCH_DISABLED_IO_32_BIT;
                        break;
                    case 0x0d:
                        *pRpType1Config = CPDI_RP_TYPE1_PREFETCH_32_BIT_IO_32_BIT;
                        break;
                    case 0x0f:
                        *pRpType1Config = CPDI_RP_TYPE1_PREFETCH_64_BIT_IO_32_BIT;
                        break;
                    default:
                        return EINVAL;
                        break;
               }
            }
            break;
        case CPDI_DO_WRITE:
            switch (*pRpType1Config) {
                case CPDI_RP_TYPE1_DISABLED_ALL:
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPME__CLR(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPMS__CLR(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIE__CLR(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIS__CLR(regVal);
                    break;
                case CPDI_RP_TYPE1_PREFETCH_32_BIT_IO_DISABLED:
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPME__SET(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPMS__CLR(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIE__CLR(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIS__CLR(regVal);
                    break;
                case CPDI_RP_TYPE1_PREFETCH_64_BIT_IO_DISABLED:
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPME__SET(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPMS__SET(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIE__CLR(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIS__CLR(regVal);
                    break;
                case CPDI_RP_TYPE1_PREFETCH_DISABLED_IO_16_BIT:
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPME__CLR(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPMS__CLR(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIE__SET(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIS__CLR(regVal);
                    break;
                case CPDI_RP_TYPE1_PREFETCH_DISABLED_IO_32_BIT:
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPME__CLR(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPMS__CLR(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIE__SET(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIS__SET(regVal);
                    break;
                case CPDI_RP_TYPE1_PREFETCH_32_BIT_IO_16_BIT:
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPME__SET(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPMS__CLR(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIE__SET(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIS__CLR(regVal);
                    break;
                case CPDI_RP_TYPE1_PREFETCH_32_BIT_IO_32_BIT:
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPME__SET(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPMS__CLR(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIE__SET(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIS__SET(regVal);
                    break;
                case CPDI_RP_TYPE1_PREFETCH_64_BIT_IO_16_BIT:
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPME__SET(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPMS__SET(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIE__SET(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIS__CLR(regVal);
                    break;
                case CPDI_RP_TYPE1_PREFETCH_64_BIT_IO_32_BIT:
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPME__SET(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPMS__SET(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIE__SET(regVal);
                    PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBARPIS__SET(regVal);
                    break;
                default: return EINVAL;
            };
            CPDI_REG_WR (&(pcieAddr->i_rc_BAR_config_reg), regVal);
            break;
        default: return EINVAL;
    }

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}


/****************************************************************************/
/* CPDI_Control_Root_Port_BAR_Check                                         */
/****************************************************************************/
uint32_t CPDI_ControlRootPortBarCheck (
                         void                    *pD,
                         CPDI_EnableOrDisable enableOrDisable) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;
    CPDI_CoreEpOrRp epOrRp;

    if (! pD)
        return EINVAL;

    if (CPDI_IsCoreStrappedAsEpOrRp (pD, & epOrRp) || epOrRp == CPDI_CORE_EP_STRAPPED )
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_rc_BAR_config_reg));

    switch (enableOrDisable) {

        case CPDI_ENABLE_PARAM:

            PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBCE__SET(regVal);
            break;

        case CPDI_DISABLE_PARAM:

            PCIE_LM_ADDRMAP__RC_BAR_CONFIG_REG__RCBCE__CLR(regVal);
            break;

        default: return EINVAL;
    }
    CPDI_REG_WR (&(pcieAddr->i_rc_BAR_config_reg), regVal);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************/
/****************************************************************************/
/* DEBUG CONTROL                                                            */
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/* CPDI_ControlRpMaster_Loopback                                          */
/****************************************************************************/
uint32_t CPDI_ControlRpMasterLoopback (
                         void                     *pD,
                         CPDI_EnableOrDisable  enableOrDisable) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;
    CPDI_CoreEpOrRp epOrRp;

    if (! pD)
        return EINVAL;

    if (CPDI_IsCoreStrappedAsEpOrRp (pD, & epOrRp) || epOrRp == CPDI_CORE_EP_STRAPPED )
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_pl_config_0_reg));

    switch (enableOrDisable) {
        case CPDI_ENABLE_PARAM:
            PCIE_LM_ADDRMAP__PL_CONFIG_0_REG__MLE__SET(regVal);
            break;
        case CPDI_DISABLE_PARAM:
            PCIE_LM_ADDRMAP__PL_CONFIG_0_REG__MLE__CLR(regVal);
            break;
        default: return EINVAL;
    }
    CPDI_REG_WR (&(pcieAddr->i_pl_config_0_reg), regVal);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************
   CPDI_Access_Debug_Mux
*****************************************************************************/
uint32_t CPDI_AccessDebugMux (
                         void                     *pD,
                         CPDI_ReadOrWrite      rdOrWr,
                         CPDI_DebugMuxModuleSelect *pRdWrVal) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;

    if (! pD)
        return EINVAL;
    if (! pRdWrVal)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_debug_mux_control_reg));

    if (rdOrWr ==CPDI_DO_READ) {
        *pRdWrVal = (CPDI_DebugMuxModuleSelect)
                  PCIE_LM_ADDRMAP__DEBUG_MUX_CONTROL_REG__MS__READ(regVal);

        if (CPDI_CallErrorCallbackIfError (pD))
            return EINVAL;

        return 0;
    }
    if (rdOrWr !=CPDI_DO_WRITE)
        return EINVAL;

    switch (*pRdWrVal) {
        case CPDI_DEBUG_PHYSICAL_LAYER_LTSSM_0     :
        case CPDI_DEBUG_PHYSICAL_LAYER_LTSSM_1     :
        case CPDI_DEBUG_DATA_LINK_LAYER_TX_SIDE    :
        case CPDI_DEBUG_DATA_LINK_LAYER_RX_SIDE    :
        case CPDI_DEBUG_TRANSACTION_LAYER_TX_SIDE  :
        case CPDI_DEBUG_TRANSACTION_LAYER_RX_SIDE_0:
        case CPDI_DEBUG_TRANSACTION_LAYER_RX_SIDE_1: break;
        default: return EINVAL;
    };

    PCIE_LM_ADDRMAP__DEBUG_MUX_CONTROL_REG__MS__MODIFY(regVal, *pRdWrVal);

    CPDI_REG_WR (&(pcieAddr->i_debug_mux_control_reg), regVal);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************/
/* CPDI_Control_Debug_Params                                                */
/****************************************************************************/
uint32_t CPDI_ControlDebugParams (
                     void                     *pD,
                     CPDI_DebugParams       debugParam,
                     CPDI_EnableOrDisable  enableOrDisable) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;

    if (! pD)
        return EINVAL;


    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_debug_mux_control_reg));

    switch (debugParam) {



        case CPDI_AXI_BRIDGE_WRITE_PRIORITY:

            switch (enableOrDisable) {
                case CPDI_ENABLE_PARAM:
                    PCIE_LM_ADDRMAP__DEBUG_MUX_CONTROL_REG__AWRPRI__SET(regVal);
                    break;
                case CPDI_DISABLE_PARAM:
                    PCIE_LM_ADDRMAP__DEBUG_MUX_CONTROL_REG__AWRPRI__CLR(regVal);
                    break;
                default: return EINVAL;
            }
            break;



        case CPDI_LINK_UPCONFIGURE_CAPABILITY:

            switch (enableOrDisable) {
                case CPDI_ENABLE_PARAM:
                    PCIE_LM_ADDRMAP__DEBUG_MUX_CONTROL_REG__DLUC__CLR(regVal);
                    break;
                case CPDI_DISABLE_PARAM:
                    PCIE_LM_ADDRMAP__DEBUG_MUX_CONTROL_REG__DLUC__SET(regVal);
                    break;
                default: return EINVAL;
            }
            break;

        case CPDI_FAST_LINK_TRAINING:

            switch (enableOrDisable) {
                case CPDI_ENABLE_PARAM:
                    PCIE_LM_ADDRMAP__DEBUG_MUX_CONTROL_REG__EFLT__SET(regVal);
                    break;
                case CPDI_DISABLE_PARAM:
                    PCIE_LM_ADDRMAP__DEBUG_MUX_CONTROL_REG__EFLT__CLR(regVal);
                    break;
                default: return EINVAL;
            }
            break;






        case CPDI_ELECTRICAL_IDLE_INFER_IN_L0_STATE:

            switch (enableOrDisable) {
                case CPDI_ENABLE_PARAM:
                    PCIE_LM_ADDRMAP__DEBUG_MUX_CONTROL_REG__DEI__CLR(regVal);
                    break;
                case CPDI_DISABLE_PARAM:
                    PCIE_LM_ADDRMAP__DEBUG_MUX_CONTROL_REG__DEI__SET(regVal);
                    break;
                default: return EINVAL;
            }
            break;

        case CPDI_FLOW_CONTROL_UPDATE_TIMEOUT:

            switch (enableOrDisable) {
                case CPDI_ENABLE_PARAM:
                    PCIE_LM_ADDRMAP__DEBUG_MUX_CONTROL_REG__DFCUT__CLR(regVal);
                    break;
                case CPDI_DISABLE_PARAM:
                    PCIE_LM_ADDRMAP__DEBUG_MUX_CONTROL_REG__DFCUT__SET(regVal);
                    break;
                default: return EINVAL;
            }
            break;

        case CPDI_ORDERING_CHECKS:

            switch (enableOrDisable) {
                case CPDI_ENABLE_PARAM:
                    PCIE_LM_ADDRMAP__DEBUG_MUX_CONTROL_REG__DOC__CLR(regVal);
                    break;
                case CPDI_DISABLE_PARAM:
                    PCIE_LM_ADDRMAP__DEBUG_MUX_CONTROL_REG__DOC__SET(regVal);
                    break;
                default: return EINVAL;
            }
            break;

        case CPDI_FUNC_SPECIFIC_RPRT_OF_TYPE1_CONF_ACC:

            switch (enableOrDisable) {
                case CPDI_ENABLE_PARAM:
                    PCIE_LM_ADDRMAP__DEBUG_MUX_CONTROL_REG__EFSRTCA__SET(regVal);
                    break;
                case CPDI_DISABLE_PARAM:
                    PCIE_LM_ADDRMAP__DEBUG_MUX_CONTROL_REG__EFSRTCA__CLR(regVal);
                    break;
                default: return EINVAL;
            }
            break;
        default: return EINVAL;
    }

    CPDI_REG_WR (&(pcieAddr->i_debug_mux_control_reg), regVal);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}


/****************************************************************************/
/****************************************************************************/
/* COUNT STATISTICS ROUTINES                                                */
/****************************************************************************/
/****************************************************************************/
uint32_t CPDI_AccessSavedCountValues (
                         void                      *pD,
                         CPDI_SavedCountParams  countParam,
                         CPDI_Bool              resetAfterRd,
                         CPDI_ReadOrWrite       rdOrWr,
                         uint32_t              *pCountVal) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;

    if (! pD)
        return EINVAL;
    if (! pCountVal)
        return EINVAL;
    if (resetAfterRd != CPDI_FALSE && resetAfterRd != CPDI_TRUE)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    switch (countParam) {

        case CPDI_RECEIVE_TLP_COUNT:

           if (rdOrWr !=CPDI_DO_READ)
               return EINVAL;
           regVal = (uint32_t) CPDI_REG_RD (&(pcieAddr->i_receive_tlp_count_reg));
           *pCountVal = PCIE_LM_ADDRMAP__RECEIVE_TLP_COUNT_REG__RTC__READ(regVal);

           if (resetAfterRd) {
               PCIE_LM_ADDRMAP__RECEIVE_TLP_COUNT_REG__RTC__MODIFY(regVal, 1);
               CPDI_REG_WR (&(pcieAddr->i_receive_tlp_count_reg), regVal);
           }
           break;

        case CPDI_RECEIVE_TLP_PAYLOAD_DWORD_COUNT:

           if (rdOrWr !=CPDI_DO_READ)
               return EINVAL;
           regVal = (uint32_t) CPDI_REG_RD (&(pcieAddr->i_receive_tlp_payload_dword_count_reg));
           *pCountVal = PCIE_LM_ADDRMAP__RECEIVE_TLP_PAYLOAD_DWORD_COUNT_REG__RTPDC__READ(regVal);

           if (resetAfterRd) {
               PCIE_LM_ADDRMAP__RECEIVE_TLP_PAYLOAD_DWORD_COUNT_REG__RTPDC__MODIFY(regVal, 1);
               CPDI_REG_WR (&(pcieAddr->i_receive_tlp_payload_dword_count_reg), regVal);
           }
           break;

        case CPDI_TRANSMIT_TLP_COUNT:

           if (rdOrWr !=CPDI_DO_READ)
               return EINVAL;
           regVal = (uint32_t) CPDI_REG_RD (&(pcieAddr->i_transmit_tlp_count_reg));
           *pCountVal = PCIE_LM_ADDRMAP__TRANSMIT_TLP_COUNT_REG__TTC__READ(regVal);

           if (resetAfterRd) {
               PCIE_LM_ADDRMAP__TRANSMIT_TLP_COUNT_REG__TTC__MODIFY(regVal, 1);
               CPDI_REG_WR (&(pcieAddr->i_transmit_tlp_count_reg), regVal);
           }
           break;

        case CPDI_TRANSMIT_TLP_PAYLOAD_DWORD_COUNT:

           if (rdOrWr !=CPDI_DO_READ)
               return EINVAL;
           regVal = (uint32_t) CPDI_REG_RD (&(pcieAddr->i_transmit_tlp_payload_dword_count_reg));
           *pCountVal = PCIE_LM_ADDRMAP__TRANSMIT_TLP_PAYLOAD_DWORD_COUNT_REG__TTPBC__READ(regVal);

           if (resetAfterRd) {
               PCIE_LM_ADDRMAP__TRANSMIT_TLP_PAYLOAD_DWORD_COUNT_REG__TTPBC__MODIFY(regVal, 1);
               CPDI_REG_WR (&(pcieAddr->i_transmit_tlp_payload_dword_count_reg), regVal);
           }
           break;

        case CPDI_TLP_LCRC_ERROR_COUNT:

           if (rdOrWr !=CPDI_DO_READ)
               return EINVAL;
           regVal = (uint32_t) CPDI_REG_RD (&(pcieAddr->i_lcrc_err_count_reg));
           *pCountVal = PCIE_LM_ADDRMAP__LCRC_ERR_COUNT_REG__LEC__READ(regVal);

           if (resetAfterRd) {
               /* Mask all WOCLR bits in regval */
               regVal &= ~PCIE_LM_ADDRMAP__LCRC_ERR_COUNT_REG__WOCLR;
               PCIE_LM_ADDRMAP__LCRC_ERR_COUNT_REG__LEC__MODIFY(regVal, 1);
               CPDI_REG_WR (&(pcieAddr->i_lcrc_err_count_reg), regVal);
           }
           break;

        case CPDI_ECC_CORRECTABLE_ERROR_COUNT_PNP_FIFO_RAM:

           if (rdOrWr !=CPDI_DO_READ)
               return EINVAL;
           regVal = (uint32_t) CPDI_REG_RD (&(pcieAddr->i_ecc_corr_err_count_reg));
           *pCountVal = PCIE_LM_ADDRMAP__ECC_CORR_ERR_COUNT_REG__PFRCER__READ(regVal);

           if (resetAfterRd) {
               /* Mask all WOCLR bits in regval */
               regVal &= ~PCIE_LM_ADDRMAP__ECC_CORR_ERR_COUNT_REG__WOCLR;
               PCIE_LM_ADDRMAP__ECC_CORR_ERR_COUNT_REG__PFRCER__MODIFY(regVal, 1);
               CPDI_REG_WR (&(pcieAddr->i_ecc_corr_err_count_reg), regVal);
           }
           break;

        case CPDI_ECC_CORRECTABLE_ERROR_COUNT_SC_FIFO_RAM:

           if (rdOrWr !=CPDI_DO_READ)
               return EINVAL;
           regVal = (uint32_t) CPDI_REG_RD (&(pcieAddr->i_ecc_corr_err_count_reg));
           *pCountVal = PCIE_LM_ADDRMAP__ECC_CORR_ERR_COUNT_REG__SFRCER__READ(regVal);

           if (resetAfterRd) {
               /* Mask all WOCLR bits in regval */
               regVal &= ~PCIE_LM_ADDRMAP__ECC_CORR_ERR_COUNT_REG__WOCLR;
               PCIE_LM_ADDRMAP__ECC_CORR_ERR_COUNT_REG__SFRCER__MODIFY(regVal, 1);
               CPDI_REG_WR (&(pcieAddr->i_ecc_corr_err_count_reg), regVal);
           }
           break;

        case CPDI_ECC_CORRECTABLE_ERROR_COUNT_REPLAY_RAM:

           if (rdOrWr !=CPDI_DO_READ)
               return EINVAL;
           regVal = (uint32_t) CPDI_REG_RD (&(pcieAddr->i_ecc_corr_err_count_reg));
           *pCountVal = PCIE_LM_ADDRMAP__ECC_CORR_ERR_COUNT_REG__RRCER__READ(regVal);

           if (resetAfterRd) {
               /* Mask all WOCLR bits in regval */
               regVal &= ~PCIE_LM_ADDRMAP__ECC_CORR_ERR_COUNT_REG__WOCLR;
               PCIE_LM_ADDRMAP__ECC_CORR_ERR_COUNT_REG__RRCER__MODIFY(regVal, 1);
               CPDI_REG_WR (&(pcieAddr->i_ecc_corr_err_count_reg), regVal);
           }
           break;




        case CPDI_RECEIVED_FTS_COUNT_FOR_2_5_GT_SPEED:

           if (rdOrWr !=CPDI_DO_READ)
               return EINVAL;
           regVal = (uint32_t) CPDI_REG_RD (&(pcieAddr->i_pl_config_0_reg));
           *pCountVal = PCIE_LM_ADDRMAP__PL_CONFIG_0_REG__RFC__READ(regVal);

           break;

        case CPDI_RECEIVED_FTS_COUNT_FOR_5_GT_SPEED:

           if (rdOrWr !=CPDI_DO_READ)
               return EINVAL;
           regVal = (uint32_t) CPDI_REG_RD (&(pcieAddr->i_receive_fts_count_reg));
           *pCountVal = PCIE_LM_ADDRMAP__RECEIVE_FTS_COUNT_REG__RFC5S__READ(regVal);

           break;



        case CPDI_TRANSMITTED_FTS_COUNT_FOR_2_5_GT_SPEED:

           regVal = (uint32_t) CPDI_REG_RD (&(pcieAddr->i_pl_config_1_reg));

           switch (rdOrWr) {
               case CPDI_DO_READ:
                   *pCountVal = PCIE_LM_ADDRMAP__PL_CONFIG_1_REG__TFC1__READ(regVal);
                   break;
               case CPDI_DO_WRITE:
                 if((*pCountVal)>>PCIE_LM_ADDRMAP__PL_CONFIG_1_REG__TFC1__WIDTH)
           {
                   return EINVAL;
           }
                   PCIE_LM_ADDRMAP__PL_CONFIG_1_REG__TFC1__MODIFY(regVal, *pCountVal);
                   CPDI_REG_WR (&(pcieAddr->i_pl_config_1_reg), regVal);
                   break;
                default: return EINVAL;
           };
           break;

        case CPDI_TRANSMITTED_FTS_COUNT_FOR_5_GT_SPEED:

           regVal = (uint32_t) CPDI_REG_RD (&(pcieAddr->i_pl_config_1_reg));

           switch (rdOrWr) {
               case CPDI_DO_READ:
                   *pCountVal = PCIE_LM_ADDRMAP__PL_CONFIG_1_REG__TFC2__READ(regVal);
                   break;
               case CPDI_DO_WRITE:
                 if((*pCountVal)>>PCIE_LM_ADDRMAP__PL_CONFIG_1_REG__TFC2__WIDTH)
           {
                   return EINVAL;
           }
                   PCIE_LM_ADDRMAP__PL_CONFIG_1_REG__TFC2__MODIFY(regVal, *pCountVal);
                   CPDI_REG_WR (&(pcieAddr->i_pl_config_1_reg), regVal);
                   break;
                default: return EINVAL;
           };
           break;

        case CPDI_TRANSMITTED_FTS_COUNT_FOR_8_GT_SPEED:

           regVal = (uint32_t) CPDI_REG_RD (&(pcieAddr->i_pl_config_1_reg));

           switch (rdOrWr) {
               case CPDI_DO_READ:
                   *pCountVal = PCIE_LM_ADDRMAP__PL_CONFIG_1_REG__TFC3__READ(regVal);
                   break;
               case CPDI_DO_WRITE:
                 if((*pCountVal)>>PCIE_LM_ADDRMAP__PL_CONFIG_1_REG__TFC3__WIDTH)
           {
                   return EINVAL;
           }
                   PCIE_LM_ADDRMAP__PL_CONFIG_1_REG__TFC3__MODIFY(regVal, *pCountVal);
                   CPDI_REG_WR (&(pcieAddr->i_pl_config_1_reg), regVal);
                   break;
                default: return EINVAL;
           };
           break;
        default: return EINVAL;
    }

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}


/****************************************************************************/
/****************************************************************************/
/* EVENT MASKING ROUTINES                                                   */
/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
/* CPDI_Control_Reporting_Of_All_Phy_Errors                                 */
/****************************************************************************/
uint32_t CPDI_ControlReportingOfAllPhyErrors (
                         void                    *pD,
                         CPDI_EnableOrDisable enableOrDisable) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;

    if (! pD)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_pl_config_0_reg));

    switch (enableOrDisable) {

        case CPDI_ENABLE_PARAM:

            PCIE_LM_ADDRMAP__PL_CONFIG_0_REG__APER__SET(regVal);
            break;

        case CPDI_DISABLE_PARAM:

            PCIE_LM_ADDRMAP__PL_CONFIG_0_REG__APER__CLR(regVal);
            break;
        default: return EINVAL;
    }

    CPDI_REG_WR (&(pcieAddr->i_pl_config_0_reg), regVal);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************/
/* CPDI_Control_Tx_Swing                                                    */
/****************************************************************************/
uint32_t CPDI_ControlTxSwing (
                         void *pD,
                         CPDI_EnableOrDisable enableOrDisable) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;

    if (! pD)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_pl_config_0_reg));

    switch (enableOrDisable) {

        case CPDI_ENABLE_PARAM:

            PCIE_LM_ADDRMAP__PL_CONFIG_0_REG__TSS__SET(regVal);
            break;

        case CPDI_DISABLE_PARAM:

            PCIE_LM_ADDRMAP__PL_CONFIG_0_REG__TSS__CLR(regVal);
            break;
        default: return EINVAL;
    }

    CPDI_REG_WR (&(pcieAddr->i_pl_config_0_reg), regVal);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************
   CPDI_Control_Masking_Of_Local_Interrupts
*****************************************************************************/
uint32_t CPDI_ControlMaskingOfLocalInterrupts (
               void                                      *pD,
               CPDI_MaskableLocalInterruptConditions maskableIntrParam,
               CPDI_MaskOrUnmask                      maskOrUnmask) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;

    if (! pD)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_local_intrpt_mask_reg));

    switch (maskableIntrParam) {

        case CPDI_PNP_RX_FIFO_PARITY_ERROR:

            switch (maskOrUnmask) {
                case CPDI_MASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__PRFPE__MODIFY(regVal, 1);
                    break;
                case CPDI_UNMASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__PRFPE__MODIFY(regVal, 0);
                    break;
                default: return EINVAL;
            }
            break;

        case CPDI_COMPLETION_RX_FIFO_PARITY_ERROR:

            switch (maskOrUnmask) {
                case CPDI_MASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__CRFPE__MODIFY(regVal, 1);
                    break;
                case CPDI_UNMASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__CRFPE__MODIFY(regVal, 0);
                    break;
                default: return EINVAL;
            }
            break;

        case CPDI_REPLAY_RAM_PARITY_ERROR:

            switch (maskOrUnmask) {
                case CPDI_MASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__RRPE__MODIFY(regVal, 1);
                    break;
                case CPDI_UNMASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__RRPE__MODIFY(regVal, 0);
                    break;
                default: return EINVAL;
            }
            break;

        case CPDI_PNP_RX_FIFO_OVERFLOW_CONDITION:

            switch (maskOrUnmask) {
                case CPDI_MASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__PRFO__MODIFY(regVal, 1);
                    break;
                case CPDI_UNMASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__PRFO__MODIFY(regVal, 0);
                    break;
                default: return EINVAL;
            }
            break;

        case CPDI_COMPLETION_RX_FIFO_OVERFLOW_CONDITION:

            switch (maskOrUnmask) {
                case CPDI_MASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__CRFO__MODIFY(regVal, 1);
                    break;
                case CPDI_UNMASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__CRFO__MODIFY(regVal, 0);
                    break;
                default: return EINVAL;
            }
            break;

        case CPDI_REPLAY_TIMEOUT_CONDITION:

            switch (maskOrUnmask) {
                case CPDI_MASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__RT__MODIFY(regVal, 1);
                    break;
                case CPDI_UNMASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__RT__MODIFY(regVal, 0);
                    break;
                default: return EINVAL;
            }
            break;

        case CPDI_REPLAY_TIMEOUT_ROLLOVER_CONDITION:

            switch (maskOrUnmask) {
                case CPDI_MASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__RTR__MODIFY(regVal, 1);
                    break;
                case CPDI_UNMASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__RTR__MODIFY(regVal, 0);
                    break;
                default: return EINVAL;
            }
            break;

        case CPDI_PHY_ERROR:

            switch (maskOrUnmask) {
                case CPDI_MASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__PE__MODIFY(regVal, 1);
                    break;
                case CPDI_UNMASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__PE__MODIFY(regVal, 0);
                    break;
                default: return EINVAL;
            }
            break;

        case CPDI_MALFORMED_TLP_RECEIVED:

            switch (maskOrUnmask) {
                case CPDI_MASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__MTR__MODIFY(regVal, 1);
                    break;
                case CPDI_UNMASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__MTR__MODIFY(regVal, 0);
                    break;
                default: return EINVAL;
            }
            break;

        case CPDI_UNEXPECTED_COMPLETION_RECEIVED:

            switch (maskOrUnmask) {
                case CPDI_MASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__UCR__MODIFY(regVal, 1);
                    break;
                case CPDI_UNMASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__UCR__MODIFY(regVal, 0);
                    break;
                default: return EINVAL;
            }
            break;

        case CPDI_FLOW_CONTROL_ERROR:

            switch (maskOrUnmask) {
                case CPDI_MASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__FCE__MODIFY(regVal, 1);
                    break;
                case CPDI_UNMASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__FCE__MODIFY(regVal, 0);
                    break;
                default: return EINVAL;
            }
            break;

        case CPDI_COMPLETION_TIMEOUT_CONDITION:

            switch (maskOrUnmask) {
                case CPDI_MASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__CT__MODIFY(regVal, 1);
                    break;
                case CPDI_UNMASK_PARAM:
                    PCIE_LM_ADDRMAP__LOCAL_INTRPT_MASK_REG__CT__MODIFY(regVal, 0);
                    break;
                default: return EINVAL;
            }
            break;




        default: return EINVAL;
    };

    CPDI_REG_WR (&(pcieAddr->i_local_intrpt_mask_reg), regVal);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************/
/* CPDI_Is_Local_Error_Status                                               */
/****************************************************************************/
uint32_t CPDI_AreThereLocalErrors (
                         void *pD,
                         CPDI_Bool *pLocalErrorsPresent,
                         uint32_t *pErrorType) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;

    if (! pD)
        return EINVAL;
    if (! pLocalErrorsPresent)
        return EINVAL;
    if (! pErrorType)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    *pErrorType = (uint32_t) CPDI_REG_RD (&(pcieAddr->i_local_error_status_register));

    *pLocalErrorsPresent = (*pErrorType) ?  CPDI_TRUE : CPDI_FALSE;

    return 0;
}

/****************************************************************************/
/* CPDI_Get_Local_Error_Status_Values                                       */
/****************************************************************************/
uint32_t  CPDI_IsLocalError (
                         void *pD,
                         CPDI_MaskableLocalInterruptConditions errorCondition,
                         CPDI_Bool *pLocalError) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;

    if (! pD)
        return EINVAL;
    if (! pLocalError)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_local_error_status_register));

    switch (errorCondition) {

        case CPDI_PNP_RX_FIFO_PARITY_ERROR:

            if (PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__PRFPE__READ(regVal))
                 *pLocalError = CPDI_TRUE;
            else *pLocalError = CPDI_FALSE;
            break;

        case CPDI_COMPLETION_RX_FIFO_PARITY_ERROR:

            if (PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__CRFPE__READ(regVal))
                 *pLocalError = CPDI_TRUE;
            else *pLocalError = CPDI_FALSE;
            break;

        case CPDI_REPLAY_RAM_PARITY_ERROR:

            if (PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__RRPE__READ(regVal))
                 *pLocalError = CPDI_TRUE;
            else *pLocalError = CPDI_FALSE;
            break;

        case CPDI_PNP_RX_FIFO_OVERFLOW_CONDITION:

            if (PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__PRFO__READ(regVal))
                 *pLocalError = CPDI_TRUE;
            else *pLocalError = CPDI_FALSE;
            break;

        case CPDI_COMPLETION_RX_FIFO_OVERFLOW_CONDITION:

            if (PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__CRFO__READ(regVal))
                 *pLocalError = CPDI_TRUE;
            else *pLocalError = CPDI_FALSE;
            break;

        case CPDI_REPLAY_TIMEOUT_CONDITION:

            if (PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__RT__READ(regVal))
                 *pLocalError = CPDI_TRUE;
            else *pLocalError = CPDI_FALSE;
            break;

        case CPDI_REPLAY_TIMEOUT_ROLLOVER_CONDITION:

            if (PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__RTR__READ(regVal))
                 *pLocalError = CPDI_TRUE;
            else *pLocalError = CPDI_FALSE;
            break;

        case CPDI_PHY_ERROR:

            if (PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__PE__READ(regVal))
                 *pLocalError = CPDI_TRUE;
            else *pLocalError = CPDI_FALSE;
            break;

        case CPDI_MALFORMED_TLP_RECEIVED:

            if (PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__MTR__READ(regVal))
                 *pLocalError = CPDI_TRUE;
            else *pLocalError = CPDI_FALSE;
            break;

        case CPDI_UNEXPECTED_COMPLETION_RECEIVED:

            if (PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__UCR__READ(regVal))
                 *pLocalError = CPDI_TRUE;
            else *pLocalError = CPDI_FALSE;
            break;

        case CPDI_FLOW_CONTROL_ERROR:

            if (PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__FCE__READ(regVal))
                 *pLocalError = CPDI_TRUE;
            else *pLocalError = CPDI_FALSE;
            break;

        case CPDI_COMPLETION_TIMEOUT_CONDITION:

            if (PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__CT__READ(regVal))
                 *pLocalError = CPDI_TRUE;
            else *pLocalError = CPDI_FALSE;
            break;





        default:

            return EINVAL;
    }

    /* We do not call CallErrorCallbackifError() here, to allow this function to be called */
    /* from the error callback if required */
    return 0;
}

/****************************************************************************/
/* CPDI_Reset_Local_Error_Status_Condition                                  */
/****************************************************************************/
uint32_t CPDI_ResetLocalErrorStatusCondition (
            void                                      *pD,
            CPDI_MaskableLocalInterruptConditions maskableIntrParam) {

    struct pcie_LM_addrmap__regf_lm_pcie_base *pcieAddr;
    uint32_t regVal;

    if (! pD)
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;

    regVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_local_error_status_register));

    /* Mask all WOCLR bits.*/
    regVal &= ~PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__WOCLR;

    /* For a WOCLR bit, the __CLR() macro will set that bit in the supplied value */
    switch (maskableIntrParam) {

        case CPDI_PNP_RX_FIFO_PARITY_ERROR:

            PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__PRFPE__CLR(regVal);
            break;

        case CPDI_COMPLETION_RX_FIFO_PARITY_ERROR:

            PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__CRFPE__CLR(regVal);
            break;

        case CPDI_REPLAY_RAM_PARITY_ERROR:

            PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__RRPE__CLR(regVal);
            break;

        case CPDI_PNP_RX_FIFO_OVERFLOW_CONDITION:

            PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__PRFO__CLR(regVal);
            break;

        case CPDI_COMPLETION_RX_FIFO_OVERFLOW_CONDITION:

            PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__CRFO__CLR(regVal);
            break;

        case CPDI_REPLAY_TIMEOUT_CONDITION:

            PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__RT__CLR(regVal);
            break;

        case CPDI_REPLAY_TIMEOUT_ROLLOVER_CONDITION:

            PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__RTR__CLR(regVal);
            break;

        case CPDI_PHY_ERROR:

            PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__PE__CLR(regVal);
            break;

        case CPDI_MALFORMED_TLP_RECEIVED:

            PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__MTR__CLR(regVal);
            break;

        case CPDI_UNEXPECTED_COMPLETION_RECEIVED:

            PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__UCR__CLR(regVal);
            break;

        case CPDI_FLOW_CONTROL_ERROR:

            PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__FCE__CLR(regVal);
            break;

        case CPDI_COMPLETION_TIMEOUT_CONDITION:

            PCIE_LM_ADDRMAP__LOCAL_ERROR_STATUS_REGISTER__CT__CLR(regVal);
            break;





        default:

            return EINVAL;
    }

    CPDI_REG_WR(&(pcieAddr->i_local_error_status_register), regVal);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************/
/****************************************************************************/
/* MESSAGE GENERATION CONTROL                                               */
/****************************************************************************/
/****************************************************************************/



/****************************************************************************/
/* CPDI_Probe                                                               */
/****************************************************************************/
uint32_t
CPDI_Probe (uintptr_t cfg,
            CPDI_SysReq *sysReq) {

   //uint32_t vid_did = 0;

   if (! sysReq)
        return EINVAL;
   if (! cfg)
        return EINVAL;

    //vid_did = CPS_UncachedRead32 ((volatile uint32_t *) cfg);
    //printk ("     CPDI_Probe , Vid_Did = %x'h \n", vid_did);

    /* Check if we have the right device */
    if ((unsigned int)CPS_UncachedRead32 ((volatile uint32_t *) cfg) != CPDI_DEFAULT_VENDOR_DEVICE_ID) {

        sysReq->pdataSize = 0;
        return EINVAL;
    }

    sysReq->pdataSize = sizeof (pvt_data_t);

    return 0;
}

/****************************************************************************/
/* CPDI_Start                                                               */
/****************************************************************************/
uint32_t
CPDI_Start (void * pD,
            CPDI_MaskableLocalInterruptConditions maskableIntrParam) {

    return (CPDI_ControlMaskingOfLocalInterrupts (pD,
                                                  maskableIntrParam,
                                                  CPDI_UNMASK_PARAM));
}

/****************************************************************************/
/* CPDI_Stop                                                                */
/****************************************************************************/
uint32_t
CPDI_Stop  (void * pD,
            CPDI_MaskableLocalInterruptConditions maskableIntrParam) {

    return (CPDI_ControlMaskingOfLocalInterrupts (pD,
                                                  maskableIntrParam,
                                                  CPDI_MASK_PARAM));
}

/****************************************************************************/
/* CPDI_Isr                                                                 */
/****************************************************************************/
uint32_t
CPDI_Isr (void * pD) {

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************/
/* CPDI_Destroy                                                             */
/****************************************************************************/
void
CPDI_Destroy (void * pD) {

    return;
}

/****************************************************************************/
/* CPDI_Init                                                                */
/****************************************************************************/
uint32_t CPDI_Init (void *pD,
                    uintptr_t cfg,
                    CPDI_InitParam *pInitParam) {

    uint32_t retVal = 0;
    struct pcie_LM_addrmap__regf_lm_pcie_base * pcieAddr;

    if (! pD)
        return EINVAL;
    if (! pInitParam)
        return EINVAL;
    if (! cfg)
        return EINVAL;

    ((pvt_data_t *) pD)->p_pcie_base = cfg;



    if (pInitParam->initBaseAddresses.initparamLmBase. changeDefault) {
        ((pvt_data_t *) pD)->p_lm_base = (struct pcie_LM_addrmap__regf_lm_pcie_base *)
                         pInitParam->initBaseAddresses.initparamLmBase.newValue;
    } else {
        ((pvt_data_t *) pD)->p_lm_base = (struct pcie_LM_addrmap__regf_lm_pcie_base *)
              (((uintptr_t)((pvt_data_t *) pD)->p_pcie_base) + CPDI_REGS_OFFSET_LOCAL_MANAGEMENT);
    }


    if (pInitParam->initBaseAddresses.initparamAwBase. changeDefault) {
        ((pvt_data_t *) pD)->p_aw_base = (struct cdn_axi_wrapper_ob *)
                         pInitParam->initBaseAddresses.initparamAwBase.newValue;
    } else {
        ((pvt_data_t *) pD)->p_aw_base = (struct cdn_axi_wrapper_ob *)
              (((uintptr_t)((pvt_data_t *) pD)->p_pcie_base) + CPDI_REG_OFFSET_AXI_WRAPPER);
    }

    if (pInitParam->initBaseAddresses.initparamAxBase. changeDefault) {
        ((pvt_data_t *) pD)->p_ax_base = (uintptr_t)
                         pInitParam->initBaseAddresses.initparamAxBase.newValue;
    } else {
        ((pvt_data_t *) pD)->p_ax_base = (uintptr_t) CPDI_DEFAULT_ECAM_BASE_BUS_ADDRESS;
    }




    /* Set Error Callback */
    if (pInitParam->initErrorCallback) {
         ((pvt_data_t *) pD)->p_ec_func = pInitParam->initErrorCallback;
    }
    else {
        ((pvt_data_t *) pD)->p_ec_func = 0;
    }

    pcieAddr = ((pvt_data_t *) pD)->p_lm_base;


    if (pInitParam->initDebugParams.initparamAllPhyErrorReporting. changeDefault) {
            CPDI_ControlReportingOfAllPhyErrors (
                         pD,
                         pInitParam->initDebugParams.initparamAllPhyErrorReporting. newValue
                         ? CPDI_ENABLE_PARAM : CPDI_DISABLE_PARAM);
    }


    if (pInitParam->initDebugParams.initparamTxSwing.changeDefault) {
            CPDI_ControlTxSwing (
                         pD,
                         pInitParam->initDebugParams.initparamTxSwing.newValue
                         ? CPDI_ENABLE_PARAM : CPDI_DISABLE_PARAM);
    }


    if (pInitParam->initDebugParams.initparamMasterLoopback .changeDefault) {
            retVal = CPDI_ControlRpMasterLoopback (
                         pD,
                         pInitParam->initDebugParams.initparamMasterLoopback. newValue
                         ? CPDI_ENABLE_PARAM : CPDI_DISABLE_PARAM);

            if (retVal) return retVal;
    }


    if (pInitParam->initDebugParams.initparamMuxSelect. changeDefault) {
            CPDI_DebugMuxModuleSelect val = pInitParam->initDebugParams.initparamMuxSelect .newValue;
            retVal = CPDI_AccessDebugMux (
                         pD,
                         CPDI_DO_WRITE,
                         &val);

            if (retVal) return retVal;
    }



    if (pInitParam->initCountParams.initparamTransmittedFtsCount2_5Gbs.changeDefault) {
            uint32_t * pval = &(pInitParam->initCountParams.initparamTransmittedFtsCount2_5Gbs.newValue);
            retVal = CPDI_AccessSavedCountValues (
                         pD,
                         CPDI_TRANSMITTED_FTS_COUNT_FOR_2_5_GT_SPEED,
                         CPDI_FALSE,
                         CPDI_DO_WRITE,
                         pval);

            if (retVal) return retVal;
    }

    if (pInitParam->initCountParams.initparamTransmittedFtsCount5Gbs.changeDefault) {
            uint32_t * pval = &(pInitParam->initCountParams.initparamTransmittedFtsCount5Gbs.newValue);
            retVal = CPDI_AccessSavedCountValues (
                         pD,
                         CPDI_TRANSMITTED_FTS_COUNT_FOR_5_GT_SPEED,
                         CPDI_FALSE,
                         CPDI_DO_WRITE,
                         pval);

            if (retVal) return retVal;
    }

    if (pInitParam->initCountParams.initparamTransmittedFtsCount8Gbs.changeDefault) {
            uint32_t * pval = &(pInitParam->initCountParams.initparamTransmittedFtsCount8Gbs.newValue);
            retVal = CPDI_AccessSavedCountValues (
                         pD,
                         CPDI_TRANSMITTED_FTS_COUNT_FOR_8_GT_SPEED,
                         CPDI_FALSE,
                         CPDI_DO_WRITE,
                         pval);

            if (retVal) return retVal;
    }


    if (pInitParam->initTimingParams.initparamTxReplayTimeoutAdjustment.changeDefault) {
            uint32_t * pval = &(pInitParam->initTimingParams.initparamTxReplayTimeoutAdjustment.newValue);
            retVal = CPDI_AccessTimingParams (
                         pD,
                         CPDI_TRANSMIT_SIDE_REPLAY_TIMEOUT,
                         CPDI_DO_WRITE,
                         pval);

            if (retVal) return retVal;
    }

    if (pInitParam->initTimingParams.initparamRxReplayTimeoutAdjustment .changeDefault) {
            uint32_t * pval = &(pInitParam->initTimingParams.initparamRxReplayTimeoutAdjustment.newValue);
            retVal = CPDI_AccessTimingParams (
                          pD,
                          CPDI_RECEIVE_SIDE_ACK_NACK_REPLAY_TIMEOUT,
                          CPDI_DO_WRITE,
                          pval);

            if (retVal) return retVal;
    }


    if (pInitParam->initCreditParams.initparamPostedPayloadCredit.changeDefault) {
             uint32_t * pval = &(pInitParam->initCreditParams.initparamPostedPayloadCredit.newValue);
             retVal = CPDI_AccessCreditLimitSettings (
                          pD,
                          CPDI_POSTED_PAYLOAD_CREDIT,
                          CPDI_RECEIVE,
                          CPDI_DO_WRITE,
                          0,
                          pval);

            if (retVal) return retVal;
    }

    if (pInitParam->initCreditParams.initparamPostedHeaderCredit.changeDefault) {
             uint32_t * pval = &(pInitParam->initCreditParams.initparamPostedHeaderCredit.newValue);
             retVal = CPDI_AccessCreditLimitSettings (
                          pD,
                          CPDI_POSTED_HEADER_CREDIT,
                          CPDI_RECEIVE,
                          CPDI_DO_WRITE,
                          0,
                          pval);

            if (retVal) return retVal;
    }

    if (pInitParam->initCreditParams.initparamNonPostedPayloadCredit.changeDefault) {
             uint32_t * pval = &(pInitParam->initCreditParams.initparamNonPostedPayloadCredit.newValue);
             retVal = CPDI_AccessCreditLimitSettings (
                          pD,
                          CPDI_NONPOSTED_PAYLOAD_CREDIT,
                          CPDI_RECEIVE,
                          CPDI_DO_WRITE,
                          0,
                          pval);

            if (retVal) return retVal;
    }

    if (pInitParam->initCreditParams.initparamNonPostedHeaderCredit.changeDefault) {
             uint32_t * pval = &(pInitParam->initCreditParams.initparamNonPostedHeaderCredit.newValue);
             retVal = CPDI_AccessCreditLimitSettings (
                          pD,
                          CPDI_NONPOSTED_HEADER_CREDIT,
                          CPDI_RECEIVE,
                          CPDI_DO_WRITE,
                          0,
                          pval);

            if (retVal) return retVal;
    }

    if (pInitParam->initCreditParams.initparamCompletionPayloadCredit.changeDefault) {
             uint32_t * pval = &(pInitParam->initCreditParams.initparamCompletionPayloadCredit.newValue);
             retVal = CPDI_AccessCreditLimitSettings (
                          pD,
                          CPDI_COMPLETION_PAYLOAD_CREDIT,
                          CPDI_RECEIVE,
                          CPDI_DO_WRITE,
                          0,
                          pval);

            if (retVal) return retVal;
    }

    if (pInitParam->initCreditParams.initparamCompletionHeaderCredit.changeDefault) {
             uint32_t * pval = &(pInitParam->initCreditParams.initparamCompletionHeaderCredit.newValue);
             retVal = CPDI_AccessCreditLimitSettings (
                          pD,
                          CPDI_COMPLETION_HEADER_CREDIT,
                          CPDI_RECEIVE,
                          CPDI_DO_WRITE,
                          0,
                          pval);

            if (retVal) return retVal;
    }

    if (pInitParam->initCreditParams.initparamMinPostedUpdateInterval.changeDefault) {
             uint32_t * pval = &(pInitParam->initCreditParams.initparamMinPostedUpdateInterval.newValue);
             retVal = CPDI_AccessTransmitCreditUpdateIntervalSettings (
                          pD,
                          CPDI_MIN_POSTED_CREDIT_UPDATE_INTERVAL,
                          CPDI_DO_WRITE,
                          pval);

            if (retVal) return retVal;
    }

    if (pInitParam->initCreditParams.initparamMinNonPostedUpdateInterval.changeDefault) {
             uint32_t * pval = &(pInitParam->initCreditParams.initparamMinNonPostedUpdateInterval.newValue);
             retVal = CPDI_AccessTransmitCreditUpdateIntervalSettings (
                          pD,
                          CPDI_MIN_NONPOSTED_CREDIT_UPDATE_INTERVAL,
                          CPDI_DO_WRITE,
                          pval);

            if (retVal) return retVal;
    }

    if (pInitParam->initCreditParams.initparamCompletionUpdateInterval.changeDefault) {
             uint32_t * pval = &(pInitParam->initCreditParams.initparamCompletionUpdateInterval.newValue);
             retVal = CPDI_AccessTransmitCreditUpdateIntervalSettings (
                          pD,
                          CPDI_MIN_COMPLETION_UPDATE_INERVAL,
                          CPDI_DO_WRITE,
                          pval);

            if (retVal) return retVal;
    }

    if (pInitParam->initCreditParams.initparamMaxUpdateInterval.changeDefault) {
             uint32_t * pval = &(pInitParam->initCreditParams.initparamMaxUpdateInterval.newValue);
             retVal = CPDI_AccessTransmitCreditUpdateIntervalSettings (
                          pD,
                          CPDI_MAX_UPDATE_INTERVAL_FOR_ALL,
                          CPDI_DO_WRITE,
                          pval);

            if (retVal) return retVal;
    }
        if (pInitParam->initTimingParams.initparamL0sTimeout.changeDefault) {
             uint32_t * pval = &(pInitParam->initTimingParams.initparamL0sTimeout.newValue);
             retVal = CPDI_AccessL0sTimeout (
                          pD,
                          CPDI_DO_WRITE,
                          pval);

            if (retVal) return retVal;
    }

    if (pInitParam->initTimingParams.initparamCompletionTimeoutLimit0 .changeDefault) {
            uint32_t * pval = &(pInitParam ->initTimingParams.initparamCompletionTimeoutLimit0 .newValue);
            retVal = CPDI_AccessTimingParams (
                         pD,
                         CPDI_COMPLETION_TIMEOUT_LIMIT_0,
                         CPDI_DO_WRITE,
                         pval);

            if (retVal) return retVal;
    }

    if (pInitParam ->initTimingParams.initparamCompletionTimeoutLimit1 .changeDefault) {
            uint32_t * pval = &(pInitParam ->initTimingParams.initparamCompletionTimeoutLimit1 .newValue);
            retVal = CPDI_AccessTimingParams (
                         pD,
                         CPDI_COMPLETION_TIMEOUT_LIMIT_1,
                         CPDI_DO_WRITE,
                         pval);

            if (retVal) return retVal;
    }

    if (pInitParam ->initTimingParams.initparamL1RetryDelay .changeDefault) {
            uint32_t * pval = &(pInitParam ->initTimingParams.initparamL1RetryDelay .newValue);
            retVal = CPDI_AccessTimingParams (
                         pD,
                         CPDI_L1_STATE_RETRY_DELAY,
                         CPDI_DO_WRITE,
                         pval);

            if (retVal) return retVal;
    }

    if (pInitParam ->initIdParams.initparamVendorId .changeDefault) {
            uint32_t * pval = &(pInitParam ->initIdParams.initparamVendorId .newValue);
            uint16_t val = (uint16_t) *pval;

            /* Check if the value to be written is in valid range */
            if((*pval)>>PCIE_LM_ADDRMAP__VENDOR_ID_REG__VID__WIDTH)
            {
                return EINVAL;
            }
            retVal = CPDI_AccessVendorIdSubsystemVendorId (
                         pD,
                         CPDI_VENDOR_ID,
                         CPDI_DO_WRITE,
                         (uint16_t*) &val);

            if (retVal) return retVal;
    }

    if (pInitParam ->initIdParams.initparamSubsystemVendorId .changeDefault) {
            uint32_t * pval = &(pInitParam ->initIdParams.initparamSubsystemVendorId .newValue);
            uint16_t val = (uint16_t) *pval;

            /* Check if the value to be written is in valid range */
            if((*pval)>>PCIE_LM_ADDRMAP__VENDOR_ID_REG__SVID__WIDTH)
            {
                return EINVAL;
            }
            retVal = CPDI_AccessVendorIdSubsystemVendorId (
                         pD,
                         CPDI_SUBSYSTEM_VENDOR_ID,
                         CPDI_DO_WRITE,
                         (uint16_t*) &val);

            if (retVal) return retVal;
    }

    if (pInitParam ->initTimingParams.initparamL1Timeout .changeDefault) {
            uint32_t * pval = &(pInitParam ->initTimingParams.initparamL1Timeout .newValue);
            retVal = CPDI_AccessTimingParams (
                         pD,
                         CPDI_ASPM_L1_ENTRY_TIMEOUT_DELAY,
                         CPDI_DO_WRITE,
                         pval);

            if (retVal) return retVal;
    }

    if (pInitParam ->initTimingParams.initparamPmeTurnoffAckDelay .changeDefault) {
            uint32_t * pval = &(pInitParam ->initTimingParams.initparamPmeTurnoffAckDelay .newValue);
            retVal = CPDI_AccessTimingParams (
                         pD,
                         CPDI_PME_TURNOFF_ACK_DELAY,
                         CPDI_DO_WRITE,
                         pval);

            if (retVal) return retVal;
    }




    if (pInitParam ->initDebugParams. initparamLinkUpconfigureCapability .changeDefault) {
            retVal = CPDI_ControlDebugParams (
                         pD,
                         CPDI_LINK_UPCONFIGURE_CAPABILITY,
                         pInitParam->initDebugParams.initparamLinkUpconfigureCapability.newValue
                         ? CPDI_ENABLE_PARAM : CPDI_DISABLE_PARAM);

            if (retVal) return retVal;
    }

    if (pInitParam ->initDebugParams. initparamFastLinkTraining .changeDefault) {
            retVal = CPDI_ControlDebugParams (
                         pD,
                         CPDI_FAST_LINK_TRAINING,
                         pInitParam->initDebugParams.initparamFastLinkTraining.newValue
                         ? CPDI_ENABLE_PARAM : CPDI_DISABLE_PARAM);

            if (retVal) return retVal;
    }





    if (pInitParam ->initDebugParams. initparamElectricalIdleInferInL0State .changeDefault) {
            retVal = CPDI_ControlDebugParams (
                         pD,
                         CPDI_ELECTRICAL_IDLE_INFER_IN_L0_STATE,
                         pInitParam->initDebugParams.initparamElectricalIdleInferInL0State.newValue
                         ? CPDI_ENABLE_PARAM : CPDI_DISABLE_PARAM);

            if (retVal) return retVal;
    }

    if (pInitParam ->initDebugParams. initparamFlowControlUpdateTimeout .changeDefault) {
            retVal = CPDI_ControlDebugParams (
                         pD,
                         CPDI_FLOW_CONTROL_UPDATE_TIMEOUT,
                         pInitParam->initDebugParams.initparamFlowControlUpdateTimeout.newValue
                         ? CPDI_ENABLE_PARAM : CPDI_DISABLE_PARAM);

            if (retVal) return retVal;
    }

    if (pInitParam ->initDebugParams. initparamOrderingChecks .changeDefault) {
            retVal = CPDI_ControlDebugParams (
                         pD,
                         CPDI_ORDERING_CHECKS,
                         pInitParam->initDebugParams.initparamOrderingChecks.newValue
                         ? CPDI_ENABLE_PARAM : CPDI_DISABLE_PARAM);

            if (retVal) return retVal;
    }

    if (pInitParam ->initDebugParams. initparamFuncSpecificRprtType1CfgAccess .changeDefault) {
            retVal = CPDI_ControlDebugParams (
                         pD,
                         CPDI_FUNC_SPECIFIC_RPRT_OF_TYPE1_CONF_ACC,
                         pInitParam->initDebugParams.initparamFuncSpecificRprtType1CfgAccess.newValue
                         ? CPDI_ENABLE_PARAM : CPDI_DISABLE_PARAM);

            if (retVal) return retVal;
    }

    if (pInitParam ->initMaskableConditions. initparamMaskPnpRxFifoParityError .changeDefault) {
            retVal = CPDI_ControlMaskingOfLocalInterrupts (
                         pD,
                         CPDI_PNP_RX_FIFO_PARITY_ERROR,
                         pInitParam ->initMaskableConditions. initparamMaskPnpRxFifoParityError .newValue
                         ? CPDI_MASK_PARAM : CPDI_UNMASK_PARAM);

            if (retVal) return retVal;
    }

    if (pInitParam ->initMaskableConditions. initparamMaskCompletionRxFifoParityError .changeDefault) {
            retVal = CPDI_ControlMaskingOfLocalInterrupts (
                         pD,
                         CPDI_COMPLETION_RX_FIFO_PARITY_ERROR,
                         pInitParam ->initMaskableConditions. initparamMaskCompletionRxFifoParityError .newValue
                         ? CPDI_MASK_PARAM : CPDI_UNMASK_PARAM);

            if (retVal) return retVal;
    }

    if (pInitParam ->initMaskableConditions. initparamMaskReplayRamParityError .changeDefault) {
            retVal = CPDI_ControlMaskingOfLocalInterrupts (
                         pD,
                         CPDI_REPLAY_RAM_PARITY_ERROR,
                         pInitParam ->initMaskableConditions. initparamMaskReplayRamParityError .newValue
                         ? CPDI_MASK_PARAM : CPDI_UNMASK_PARAM);

            if (retVal) return retVal;
    }

    if (pInitParam ->initMaskableConditions. initparamMaskPnpRxFifoOverflow .changeDefault) {
            retVal = CPDI_ControlMaskingOfLocalInterrupts (
                         pD,
                         CPDI_PNP_RX_FIFO_OVERFLOW_CONDITION,
                         pInitParam ->initMaskableConditions. initparamMaskPnpRxFifoOverflow .newValue
                         ? CPDI_MASK_PARAM : CPDI_UNMASK_PARAM);

            if (retVal) return retVal;
    }

    if (pInitParam ->initMaskableConditions. initparamMaskCompletionRxFifoOverflow .changeDefault) {
            retVal = CPDI_ControlMaskingOfLocalInterrupts (
                         pD,
                         CPDI_COMPLETION_RX_FIFO_OVERFLOW_CONDITION,
                         pInitParam ->initMaskableConditions. initparamMaskCompletionRxFifoOverflow .newValue
                         ? CPDI_MASK_PARAM : CPDI_UNMASK_PARAM);

            if (retVal) return retVal;
    }

    if (pInitParam ->initMaskableConditions. initparamMaskReplayTimeout .changeDefault) {
            retVal = CPDI_ControlMaskingOfLocalInterrupts (
                         pD,
                         CPDI_REPLAY_TIMEOUT_CONDITION,
                         pInitParam ->initMaskableConditions. initparamMaskReplayTimeout .newValue
                         ? CPDI_MASK_PARAM : CPDI_UNMASK_PARAM);

            if (retVal) return retVal;
    }

    if (pInitParam ->initMaskableConditions. initparamMaskReplayTimerRollover .changeDefault) {
            retVal = CPDI_ControlMaskingOfLocalInterrupts (
                         pD,
                         CPDI_REPLAY_TIMEOUT_ROLLOVER_CONDITION,
                         pInitParam ->initMaskableConditions. initparamMaskReplayTimerRollover .newValue
                         ? CPDI_MASK_PARAM : CPDI_UNMASK_PARAM);

            if (retVal) return retVal;
    }

    if (pInitParam ->initMaskableConditions. initparamMaskPhyError .changeDefault) {
            retVal = CPDI_ControlMaskingOfLocalInterrupts (
                         pD,
                         CPDI_PHY_ERROR,
                         pInitParam ->initMaskableConditions. initparamMaskPhyError .newValue
                         ? CPDI_MASK_PARAM : CPDI_UNMASK_PARAM);

            if (retVal) return retVal;
    }

    if (pInitParam ->initMaskableConditions. initparamMaskMalformedTlpReceived .changeDefault) {
            retVal = CPDI_ControlMaskingOfLocalInterrupts (
                         pD,
                         CPDI_MALFORMED_TLP_RECEIVED,
                         pInitParam ->initMaskableConditions. initparamMaskMalformedTlpReceived .newValue
                         ? CPDI_MASK_PARAM : CPDI_UNMASK_PARAM);

            if (retVal) return retVal;
    }

    if (pInitParam ->initMaskableConditions. initparamMaskUnexpectedCompletionReceived .changeDefault) {
            retVal = CPDI_ControlMaskingOfLocalInterrupts (
                         pD,
                         CPDI_UNEXPECTED_COMPLETION_RECEIVED,
                         pInitParam ->initMaskableConditions. initparamMaskUnexpectedCompletionReceived .newValue
                         ? CPDI_MASK_PARAM : CPDI_UNMASK_PARAM);

            if (retVal) return retVal;
    }

    if (pInitParam ->initMaskableConditions. initparamMaskFlowControlError .changeDefault) {
            retVal = CPDI_ControlMaskingOfLocalInterrupts (
                         pD,
                         CPDI_FLOW_CONTROL_ERROR,
                         pInitParam ->initMaskableConditions. initparamMaskFlowControlError .newValue
                         ? CPDI_MASK_PARAM : CPDI_UNMASK_PARAM);

            if (retVal) return retVal;
    }

    if (pInitParam ->initMaskableConditions. initparamMaskCompletionTimeout .changeDefault) {
            retVal = CPDI_ControlMaskingOfLocalInterrupts (
                         pD,
                         CPDI_COMPLETION_TIMEOUT_CONDITION,
                         pInitParam ->initMaskableConditions. initparamMaskCompletionTimeout .newValue
                         ? CPDI_MASK_PARAM : CPDI_UNMASK_PARAM);

            if (retVal) return retVal;
    }


    {
        int rpBarNum;

        for (rpBarNum=0; rpBarNum < CPDI_MAX_BARS_IN_RP; rpBarNum++) {

            if (pInitParam ->initRpBarParams.initparamRpBarAperture [rpBarNum] .changeDefault) {
                CPDI_RpBarApertureSize val = pInitParam ->initRpBarParams.initparamRpBarAperture [rpBarNum] .newValue;
                retVal = CPDI_AccessRootPortBarApertureSetting (
                    pD,
                    rpBarNum,
                    CPDI_DO_WRITE,
                    &val);

                if (retVal) return retVal;
            }

            if (pInitParam ->initRpBarParams.initparamRpBarControl  [rpBarNum].changeDefault) {
                CPDI_RpBarControl val = pInitParam ->initRpBarParams.initparamRpBarControl [rpBarNum] .newValue;
                retVal = CPDI_AccessRootPortBarControlSetting (
                    pD,
                    rpBarNum,
                    CPDI_DO_WRITE,
                    &val);
                if (retVal) return retVal;
            }
        }
 }

    if (pInitParam ->initRpBarParams. initparamEnableRpBarCheck .changeDefault) {

        CPDI_EnableOrDisable val = pInitParam ->initRpBarParams.initparamEnableRpBarCheck.newValue;
        retVal = CPDI_ControlRootPortBarCheck (
            pD,
            val);
        if (retVal) return retVal;
    }

    return 0;
}


/****************************************************************************/
/* AXI Wrapper Functions                                                    */
/****************************************************************************/

/****************************************************************************/
/* Setup Traffic class for AXI Wrapper regions                              */
/****************************************************************************/
uint32_t
CPDI_UpdateObWrapperTrafficClass (void   *pD,
                               uint8_t   regionNo     ,
                               uint8_t   trafficClass ) {

    uint32_t ob_desc_0 = 0;

    struct cdn_axi_wrapper_ob *pcieAddr;

    uintptr_t aw_base;

    if (! pD)
        return EINVAL;
    if (regionNo >= CPDI_MAX_AXI_WRAPPER_REGION_NUM)
        return EINVAL;
    if (trafficClass >= CPDI_MAX_TRAFFIC_CLASS_NUM)
        return EINVAL;

    aw_base = (uintptr_t) ((pvt_data_t *) pD)->p_aw_base;
    aw_base += (regionNo << 5);

    pcieAddr = (struct cdn_axi_wrapper_ob *) aw_base;

    /** Read the existing value of descriptor 0 */
    ob_desc_0 =  CPDI_REG_RD(&(pcieAddr->desc0));

    /* write ob-desc0.  Bits [19:17] define the PCIe traffic class */
    CPD_AXI_WRAPPER_OB_REGION_TRAFFIC_CLASS_DATA_MODIFY(ob_desc_0, trafficClass);

    /** write the register */
    CPDI_REG_WR(&(pcieAddr->desc0), ob_desc_0);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}


/****************************************************************************/
/* AXI Wrapper Setup for MemIO region                                       */
/****************************************************************************/
uint32_t
CPDI_SetupObWrapperMemIoAccess (void   *pD       ,
                              uint8_t   regionNo     ,
                              uint8_t   numPassedBits,
                              uint32_t  upperAddr    ,
                              uint32_t  lowerAddr    ) {

    uint32_t ob_addr_0 = 0;
    uint32_t ob_addr_1 = 0;
    uint32_t ob_desc_0 = 0;
    uint32_t ob_desc_1 = 0;  //bit 0~ 7 is bus number
    uint32_t ob_desc_2 = 0;
    uint32_t ob_desc_3 = 0;

    struct cdn_axi_wrapper_ob *pcieAddr;

    uintptr_t aw_base;

    if (! pD)
        return EINVAL;
    if (regionNo >= CPDI_MAX_AXI_WRAPPER_REGION_NUM)
        return EINVAL;
    if ((numPassedBits+1) < CPDI_MIN_AXI_ADDR_BITS_PASSED)
        return EINVAL;
    if (numPassedBits > 63)
        return EINVAL;

    /* Check that number of bits passed is no greater than max size for this region */
    if (regionNo==0) {
        if (CPDI_AXI_REGION_0_SIZE < (2ULL << numPassedBits ))
            return EINVAL;
    } else {
        if (CPDI_AXI_REGION_SIZE < (2ULL << numPassedBits ))
            return EINVAL;
    }

    aw_base = (uintptr_t) ((pvt_data_t *) pD)->p_aw_base;
    aw_base += (regionNo << 5);

    pcieAddr = (struct cdn_axi_wrapper_ob *) aw_base;

    /* write ob-addr0 */
    CDN_AXI_WRAPPER_OB__AXIW_OB_REGION_AD0__NUM_BITS__MODIFY(ob_addr_0, numPassedBits);
    CDN_AXI_WRAPPER_OB__AXIW_OB_REGION_AD0__DATA__MODIFY(ob_addr_0, lowerAddr);

    /* write ob-addr1 */
    CDN_AXI_WRAPPER_OB__AXIW_OB_REGION_AD1__DATA__MODIFY(ob_addr_1, upperAddr);

    /* write ob-desc0.  Bits [3:0] define the transaction type */
    CDN_AXI_WRAPPER_OB__AXIW_OB_REGION_DR0__DATA__MODIFY(ob_desc_0, AXI_WRAPPER_MEM_WRITE);//  + (1<<AXI_WRAPPER_ECRC_SHIFT));

    /** write the registers */

    CPDI_REG_WR(&(pcieAddr->addr0), ob_addr_0);  ////Bank 0x1D40
    CPDI_REG_WR(&(pcieAddr->addr1), ob_addr_1);
    CPDI_REG_WR(&(pcieAddr->desc0), ob_desc_0);
    CPDI_REG_WR(&(pcieAddr->desc1), ob_desc_1);
    CPDI_REG_WR(&(pcieAddr->desc2), ob_desc_2);
    CPDI_REG_WR(&(pcieAddr->desc3), ob_desc_3);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}


/****************************************************************************/
/* AXI Wrapper Setup for Message Access                                     */
/****************************************************************************/
uint32_t
CPDI_SetupObWrapperMessageAccess (void     *pD     ,
                                  uint8_t   regionNo ,
                                  uint32_t  upperAddr,
                                  uint32_t  lowerAddr) {

    uint32_t ob_addr_0 = 0;
    uint32_t ob_addr_1 = 0;
    uint32_t ob_desc_0 = 0;
    uint32_t ob_desc_1 = 0;
    uint32_t ob_desc_2 = 0;
    uint32_t ob_desc_3 = 0;

    struct cdn_axi_wrapper_ob *pcieAddr;

    uintptr_t aw_base;

    if (! pD)
        return EINVAL;
    if (regionNo >= CPDI_MAX_AXI_WRAPPER_REGION_NUM)
        return EINVAL;

    aw_base = (uintptr_t) ((pvt_data_t *) pD)->p_aw_base;
    aw_base += (regionNo << OB_REG_SIZE_SHIFT);

    pcieAddr = (struct cdn_axi_wrapper_ob *) aw_base;

    /* write ob-addr0 */
    CDN_AXI_WRAPPER_OB__AXIW_OB_REGION_AD0__DATA__MODIFY(ob_addr_0, lowerAddr);

    /* write ob-addr1 */
    CDN_AXI_WRAPPER_OB__AXIW_OB_REGION_AD1__DATA__MODIFY(ob_addr_1, upperAddr);

    /* write ob-desc0.  Bits [3:0] define the transaction type */
    CDN_AXI_WRAPPER_OB__AXIW_OB_REGION_DR0__DATA__MODIFY(ob_desc_0, AXI_WRAPPER_MSG_WRITE + (1<<AXI_WRAPPER_ECRC_SHIFT));

    /** write the registers */
    CPDI_REG_WR(&(pcieAddr->addr0), ob_addr_0);
    CPDI_REG_WR(&(pcieAddr->addr1), ob_addr_1);
    CPDI_REG_WR(&(pcieAddr->desc0), ob_desc_0);
    CPDI_REG_WR(&(pcieAddr->desc1), ob_desc_1);
    CPDI_REG_WR(&(pcieAddr->desc2), ob_desc_2);
    CPDI_REG_WR(&(pcieAddr->desc3), ob_desc_3);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************/
/* AXI Wrapper Setup for Config region                                      */
/****************************************************************************/
uint32_t
CPDI_SetupObWrapperConfigAccess (void     *pD        ,
                               uint8_t   regionNo      ,
                               uint8_t   numPassedBits ,
                               uint32_t  busDevFuncAddr) {

    CPDI_CoreEpOrRp epOrRp;
    uint32_t ob_addr_0 = 0;
    uint32_t ob_addr_1 = 0;
    uint32_t ob_desc_0 = 0;
    uint32_t ob_desc_1 = 0;
    uint32_t ob_desc_2 = 0;
    uint32_t ob_desc_3 = 0;

    struct cdn_axi_wrapper_ob *pcieAddr;

    uintptr_t aw_base;

    if (! pD)
        return EINVAL;
    if (CPDI_IsCoreStrappedAsEpOrRp (pD, & epOrRp) || epOrRp == CPDI_CORE_EP_STRAPPED )
        return EINVAL;
    if (regionNo >= CPDI_MAX_AXI_WRAPPER_REGION_NUM)
        return EINVAL;
    if ((numPassedBits+1) < CPDI_MIN_AXI_ADDR_BITS_PASSED)
        return EINVAL;
    if (numPassedBits > 63)
        return EINVAL;
    if (regionNo==0) {
        if (CPDI_AXI_REGION_0_SIZE < (2ULL << numPassedBits ))
            return EINVAL;
    } else {
        if (CPDI_AXI_REGION_SIZE <  (2ULL << numPassedBits ))
            return EINVAL;
    }

    aw_base = (uintptr_t) ((pvt_data_t *) pD)->p_aw_base;
    aw_base += (regionNo << OB_REG_SIZE_SHIFT);

    pcieAddr = (struct cdn_axi_wrapper_ob *) aw_base;

    /* write ob-addr0 */
    CDN_AXI_WRAPPER_OB__AXIW_OB_REGION_AD0__NUM_BITS__MODIFY(ob_addr_0, numPassedBits);
    CDN_AXI_WRAPPER_OB__AXIW_OB_REGION_AD0__DATA__MODIFY(ob_addr_0, busDevFuncAddr);

    /* write ob-desc0.  Bits [3:0] define the transaction type; Bits[21] define force ECRC Enable */
    CDN_AXI_WRAPPER_OB__AXIW_OB_REGION_DR0__DATA__MODIFY(ob_desc_0, AXI_WRAPPER_TYPE0_WRITE + (1<<AXI_WRAPPER_ECRC_SHIFT));

    /** write the registers */
    CPDI_REG_WR(&(pcieAddr->addr0), ob_addr_0);
    CPDI_REG_WR(&(pcieAddr->addr1), ob_addr_1);
    CPDI_REG_WR(&(pcieAddr->desc0), ob_desc_0);
    CPDI_REG_WR(&(pcieAddr->desc1), ob_desc_1);
    CPDI_REG_WR(&(pcieAddr->desc2), ob_desc_2);
    CPDI_REG_WR(&(pcieAddr->desc3), ob_desc_3);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************/
/* Root Port Address Translation                                            */
/****************************************************************************/
uint32_t
CPDI_SetupIbRootPortAddrTranslation (void    *pD     ,
                                     uint8_t  regionNo ,
                                     uint8_t  numPassedBits ,
                                     uint32_t upperAddr,
                                     uint32_t lowerAddr) {

    uint32_t ib_addr_0 = 0;
    uint32_t ib_addr_1 = 0;

    struct cdn_axi_wrapper_ib *pcieAddr;

    uintptr_t aw_base;

    if (! pD)
        return EINVAL;
    if (regionNo > MAX_AXI_IB_ROOTPORT_REGION_NUM)
        return EINVAL;
    if ((numPassedBits+1) < CPDI_MIN_AXI_ADDR_BITS_PASSED)
        return EINVAL;
    if (numPassedBits > 63)
        return EINVAL;

    aw_base = (uintptr_t) ((pvt_data_t *) pD)->p_aw_base;
    //aw_base += AXI_REGS_IB_OFFSET;
    aw_base +=AXI_REGS_IB_WAW_OFFSET ; //For No BAR Check
    aw_base += (regionNo << IB_ROOT_PORT_REG_SIZE_SHIFT);

    pcieAddr  = (struct cdn_axi_wrapper_ib *) aw_base;

    /* write ib-addr0 */
    CDN_AXI_WRAPPER_IB__AXIW_IB_REGION_AD0__NUM_BITS__MODIFY(ib_addr_0, numPassedBits);
    CDN_AXI_WRAPPER_IB__AXIW_IB_REGION_AD0__DATA__MODIFY(ib_addr_0, lowerAddr);

    /* write ib-addr1 */
    CDN_AXI_WRAPPER_IB__AXIW_IB_REGION_AD1__DATA__MODIFY(ib_addr_1, upperAddr);

    /** write the registers */
    CPDI_REG_WR(&(pcieAddr->addr0), ib_addr_0);  //Bank 0x1D48
    CPDI_REG_WR(&(pcieAddr->addr1), ib_addr_1);

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}

/****************************************************************************/
/* ECAM config read                                                         */
/****************************************************************************/
static uint32_t CPDI_DoEcamConfigRead (
                         void     *pD  ,
                         uint32_t  bus   ,
                         uint32_t  dev   ,
                         uint32_t  func  ,
                         uint32_t  offset,
                         CPDI_SizeInBytes  size  ,
                         uint32_t *val   ) {

    uintptr_t pcieAddr;
    uintptr_t eaddr;
    uint32_t ret=0, remaining_offset = 0;
    uint32_t rdata;
#ifdef CONFIG_MSTAR_PCIE_OB_MEMIO_ENABLE
    uint32_t IndirectMemIO=0;
#endif

    if (! val)
        return EINVAL;

    if (! pD)
        return EINVAL;

    /*
    if ((bus    > CPDI_ECAM_MAX_BUS   ) ||
        (dev    > CPDI_ECAM_MAX_DEV   ) ||
        (func   > CPDI_ECAM_MAX_FUNC  ) ||
        (offset > CPDI_ECAM_MAX_OFFSET) )
    {
        printk("     CPDI_DoEcamConfigRead() Error: offset =0x%x \n", offset);
        return EINVAL;
    }
    */

    // Add by Nelson.Yu for mask other device temporal
    if(bus ==0 && dev > 0 && offset ==0)
    { *val = 0; return 0;}

    //if(bus ==1 && dev > 1 && offset ==0)
    //{ *val = 0; return 0;}

    pcieAddr = ((pvt_data_t *) pD)->p_ax_base;

    remaining_offset = offset & 0x03;
    offset &= 0xFFFFFFFC;
    eaddr = ECAMADDR (pcieAddr, bus, dev, func, offset);


    // indirect MemIO
    if (offset >= 0x1000)
    {
#ifdef CONFIG_MSTAR_PCIE_OB_MEMIO_ENABLE
        IndirectMemIO = 1;
        //printk("[PCIE] IndirMemRd %04X:%02X:%02X off:%08X len:%02X\n", bus,dev,func, offset, size);
#else
        printk("[PCIE] config read with invalid offset 0x%x\n", offset);
        return EINVAL;
#endif
    }

    /**
     * read the required width
     */
    switch (size) {
        case CPDI_1_BYTE:
            //*val = CPS_UncachedRead8 ((volatile uint8_t *) eaddr);

#ifdef CONFIG_MSTAR_PCIE_OB_MEMIO_ENABLE
            // indirect Mem read
            if (IndirectMemIO)
            {
                ret = _mdrv_RC_ob_memio_read(NULL, offset, &rdata);
            }
            else
#endif
            {
#ifdef BUS0_RC_ENABLE
if(bus == 0)
            rdata = readl((void*) (CPDREF_RP_BASE + offset));
else
#endif
				{
            ret = _mdrv_RC_ob_config_read(NULL, eaddr, &rdata);
				}
            }

            if(remaining_offset == 0)
                *val = rdata & 0x000000FF;
            else if(remaining_offset == 1)
                *val = ((rdata & 0x0000FF00) >>8);
            else if(remaining_offset == 2)
                *val = ((rdata & 0x00FF0000) >>16);
            else
                *val = ((rdata & 0xFF000000) >>24);
            break;
        case CPDI_2_BYTE:
            //*val = CPS_UncachedRead16 ((volatile uint16_t *) eaddr);

#ifdef CONFIG_MSTAR_PCIE_OB_MEMIO_ENABLE
            // indirect Mem read
            if (IndirectMemIO)
            {
                ret = _mdrv_RC_ob_memio_read(NULL, offset, &rdata);
            }
            else
#endif
            {
#ifdef BUS0_RC_ENABLE
if(bus == 0)
            rdata = readl((void*) (CPDREF_RP_BASE + offset));
else
#endif
				{
            ret = _mdrv_RC_ob_config_read(NULL, eaddr, &rdata);
				}
            }

            if(remaining_offset == 0)
                *val = rdata & 0x0000FFFF;
            else if(remaining_offset == 1)
            {
                *val = ((rdata & 0x00FFFF00) >>8);
                printk("     CPDI_DoEcamConfigRead() WARNING: size = %d, remaining_offset = %d, rdata = 0x%x \n", size, remaining_offset, rdata);
            }
            else if(remaining_offset == 2)
                *val = ((rdata & 0xFFFF0000) >>16);
            else
            {
                *val = ((rdata & 0xFF000000) >>24);
                printk("     CPDI_DoEcamConfigRead() WARNING: size = %d, remaining_offset = %d, rdata = 0x%x \n", size, remaining_offset, rdata);
            }
            break;
        case CPDI_4_BYTE:
            //*val = CPS_UncachedRead32 ((volatile uint32_t *) eaddr);
#ifdef CONFIG_MSTAR_PCIE_OB_MEMIO_ENABLE
            if(IndirectMemIO)
            {
//				printk("\n Read 4 byte at offext:0x%x",offset);
                ret = _mdrv_RC_ob_memio_read(NULL, offset, &rdata);

               //ret = _mdrv_RC_ob_config_read(NULL, ( 0x2000000 + (0x100000*(CPDI_RP_OB_REGION_NUMBER-1))+ offset), &rdata);
               //rdata = _mdrv_RC_outbound_DM_read(NULL, offset);   // K6 direct memory mapping

               //printk("     CPDI_DoEcamConfigRead() WARNING: offset = %x , rdata = %x\n", offset, rdata);
            }
            else
#endif
            {

#ifdef BUS0_RC_ENABLE
if(bus == 0)
{
               rdata = readl((void*) (CPDREF_RP_BASE + offset));
               //printk("     CPDI_DoEcamConfigRead() WARNING: offset = %x , rdata = %x\n", offset, rdata);
            }
            else
#endif
				{
               //if(offset >= 0x10 && offset <= 0x24)
               //  printk("     CPDI_DoEcamConfigRead() WARNING: offset = %x , rdata = %x\n", offset, rdata);
                ret = _mdrv_RC_ob_config_read(NULL, eaddr, &rdata);
            }
            }
            *val = rdata;
            break;
        default: return  EINVAL;
    }

//    if(bus == 0)
//    printk("CPDI_DoEcamConfigRead():  bus = %d, offset = %x, *val = %x \n",bus, offset, *val);
#ifdef BUS0_RC_ENABLE
    //mdelay(1);
#endif
    //if(offset == 0)
    //  *val = 0x761214c3;

    return 0;
}

/****************************************************************************/
/* ECAM config write                                                        */
/****************************************************************************/
static uint32_t CPDI_DoEcamConfigWrite (
                         void    *pD  ,
                         uint32_t bus   ,
                         uint32_t dev   ,
                         uint32_t func  ,
                         uint32_t offset,
                         CPDI_SizeInBytes size  ,
                         uint32_t data  ) {

    uintptr_t pcieAddr;
    uintptr_t eaddr;
    uint32_t ret_val = 0 ,remaining_offset = 0;
    uint32_t rdata;
#ifdef CONFIG_MSTAR_PCIE_OB_MEMIO_ENABLE
    uint32_t IndirectMemIO=0;
#endif

    if (! pD)
        return EINVAL;
/*
    if ((bus    > CPDI_ECAM_MAX_BUS   ) ||
        (dev    > CPDI_ECAM_MAX_DEV   ) ||
        (func   > CPDI_ECAM_MAX_FUNC  ) ||
        (offset > CPDI_ECAM_MAX_OFFSET) )
    {
        printk("     CPDI_DoEcamConfigWrite() Error: offset =0x%x \n", offset);
        return EINVAL;
    }
*/
    pcieAddr = ((pvt_data_t *) pD)->p_ax_base;

    /**
     * Hardware is strapped (only) for ECAM addressing
     */
    remaining_offset = offset & 0x03;
    offset &= 0xFFFFFFFC;

    eaddr = ECAMADDR (pcieAddr, bus, dev, func, offset);

    // Indirect MemIO
    if(offset >= 0x1000)
    {
#ifdef CONFIG_MSTAR_PCIE_OB_MEMIO_ENABLE
        IndirectMemIO = 1;
#else
        printk("[PCIE] config write with invalid offset 0x%x\n", offset);
        return EINVAL;
#endif
    }


    /**
     * write the required width
     */

    switch (size) {
        case CPDI_1_BYTE:
            //CPS_UncachedWrite8  ((volatile uint8_t  *) eaddr, (uint8_t) data);
            if(data > 0x000000FF)
                    printk("     CPDI_DoEcamConfigWrite() Warining Data unreasonable: size = %d, offset = %d, data = 0x%x \n", size, offset, data);

#ifdef CONFIG_MSTAR_PCIE_OB_MEMIO_ENABLE
            // Indirect MemIO
            if(IndirectMemIO)
                ret_val = _mdrv_RC_ob_memio_read(NULL, offset, &rdata);
            else
#endif
            {
#ifdef BUS0_RC_ENABLE
if(bus == 0)
            rdata = readl((void*) (CPDREF_RP_BASE + offset));
else
#endif

            ret_val = _mdrv_RC_ob_config_read(NULL, eaddr, &rdata);
            }

            if(remaining_offset == 0)
            {
                rdata = rdata & 0xFFFFFF00;
                data =  data | rdata;
            }
            else if(remaining_offset == 1)
            {
                data = data <<8;
                rdata = rdata & 0xFFFF00FF;
                data =  data | rdata;
            }
            else if(remaining_offset == 2)
            {
                data = data <<16;
                rdata = rdata & 0xFF00FFFF;
                data =  data | rdata;
            }
            else
            {
                data = data <<24;
                rdata = rdata & 0x00FFFFFF;
                data =  data | rdata;
            }

#ifdef CONFIG_MSTAR_PCIE_OB_MEMIO_ENABLE
            if(IndirectMemIO)
            {
               ret_val = _mdrv_RC_ob_memio_write(NULL, offset, data);
               //printk("     CPDI_DoEcamConfigWrite() WARNING: offset = %x, data = %x\n", offset, data);
            }
            else
#endif
            {
#ifdef BUS0_RC_ENABLE
if(bus == 0)
{
            //printk("CPDI_DoEcamConfigWrite() CPDI_1_BYTE :  offset = %x, remaining_offset = %d ,data = %x ,  rdata = %x\n",offset, remaining_offset, data, rdata);
            writel(data, (void*) (CPDREF_RP_BASE + offset));
}
else
#endif
            ret_val = _mdrv_RC_ob_config_write(NULL, eaddr, data);
            }
            break;
        case CPDI_2_BYTE:
            //CPS_UncachedWrite16 ((volatile uint16_t *) eaddr, (uint16_t) data);
            if(data > 0x0000FFFF)
                    printk("     CPDI_DoEcamConfigWrite() Warining Data unreasonable: size = %d, offset = %d, data = 0x%x \n", size, offset, data);

#ifdef CONFIG_MSTAR_PCIE_OB_MEMIO_ENABLE
            if(IndirectMemIO)
               ret_val = _mdrv_RC_ob_memio_read(NULL, offset, &rdata);
            else
#endif
            {
#ifdef BUS0_RC_ENABLE
if(bus == 0)
            rdata = readl((void*) (CPDREF_RP_BASE + offset));
else
#endif
            ret_val = _mdrv_RC_ob_config_read(NULL, eaddr, &rdata);
            }

            if(remaining_offset == 0)
            {
                rdata = rdata & 0xFFFF0000;
                data =  data | rdata;
            }
            else if(remaining_offset == 1)
            {
                printk("     CPDI_DoEcamConfigWrite() Warining offset unreasonable: remaining_offset = %d, remaining_offset = %d, data = 0x%x \n", size, remaining_offset, data);
            }
            else if(remaining_offset == 2)
            {
                //printk("CPDI_DoEcamConfigWrite() CPDI_2_BYTE be:  offset = %x, remaining_offset = %d, data = %x , rdata = %x\n",offset, remaining_offset , data, rdata);
                data = data <<16;
                rdata = rdata & 0x0000FFFF;
                data =  data | rdata;
            }
            else
                printk("     CPDI_DoEcamConfigWrite() Warining offset unreasonable: remaining_offset = %d, remaining_offset = %d, data = 0x%x \n", size, remaining_offset, data);

#ifdef CONFIG_MSTAR_PCIE_OB_MEMIO_ENABLE
            if(IndirectMemIO)
            {
               ret_val = _mdrv_RC_ob_memio_write(NULL, offset, data);
               //printk("     CPDI_DoEcamConfigWrite() WARNING: offset = %x, data = %x\n", offset, data);
            }
            else
#endif
            {
#ifdef BUS0_RC_ENABLE
if(bus == 0)
{
            //printk("CPDI_DoEcamConfigWrite() CPDI_2_BYTE :  offset = %x, remaining_offset = %d, data = %x , rdata = %x\n",offset, remaining_offset , data, rdata);
            writel(data, (void*) (CPDREF_RP_BASE + offset));
}
else
#endif

            ret_val = _mdrv_RC_ob_config_write(NULL, eaddr, data);
            }
            break;
        case CPDI_4_BYTE:
            //CPS_UncachedWrite32 ((volatile uint32_t *) eaddr, data);
#ifdef CONFIG_MSTAR_PCIE_OB_MEMIO_ENABLE
            if(IndirectMemIO)
            {
                ret_val = _mdrv_RC_ob_memio_write(NULL, offset, data);
               //ret_val = _mdrv_RC_ob_config_write(NULL, ( 0x2000000 + (0x100000*(CPDI_RP_OB_REGION_NUMBER-1)) + offset), data);
               //_mdrv_RC_outbound_DM_write(NULL, offset, data);  // K6 direct memory mapping

               //printk("     CPDI_DoEcamConfigWrite() WARNING: offset = %x, data = %x\n", offset, data);
            }
            else
#endif
            {

#ifdef BUS0_RC_ENABLE
if(bus == 0)
{
                //printk("CPDI_DoEcamConfigWrite() CPDI_4_BYTE:  offset = %x, data = %x \n",offset, data);
                writel(data, (void*) (CPDREF_RP_BASE + offset));
            }
            else
#endif
                //if(offset >= 0x10 && offset <= 0x24)
                //   printk("CPDI_DoEcamConfigWrite() CPDI_4_BYTE:  offset = %x, data = %x \n",offset, data);
            ret_val = _mdrv_RC_ob_config_write(NULL, eaddr, data);
            }
            break;
        default: return EINVAL;
    }

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;


    //printk("CPDI_DoEcamConfigWrite() :  offset = %x, data = %x \n",offset, data);
#ifdef BUS0_RC_ENABLE
    //mdelay(1);
#endif

    return 0;
}

/****************************************************************************/
/* ECAM ARI config read                                                     */
/****************************************************************************/
static uint32_t CPDI_DoAriEcamConfigRead (
                         void     *pD  ,
                         uint32_t  bus   ,
                         uint32_t  func  ,
                         uint32_t  offset,
                         CPDI_SizeInBytes  size  ,
                         uint32_t *val   ) {

    uintptr_t pcieAddr;
    uintptr_t eaddr;

    if (! val)
        return EINVAL;

    if (! pD)
        return EINVAL;
    if ((bus    > CPDI_ECAM_MAX_BUS     ) ||
        (func   > CPDI_ARI_ECAM_MAX_FUNC) ||
        (offset > CPDI_ECAM_MAX_OFFSET  ) )
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_ax_base;

    eaddr = ARI_ECAMADDR (pcieAddr, bus, func, offset);

    /**
     * read the required width
     */
    switch (size) {
        case CPDI_1_BYTE:
            *val = CPS_UncachedRead8 ((volatile uint8_t *) eaddr);
            break;
        case CPDI_2_BYTE:
            *val = CPS_UncachedRead16 ((volatile uint16_t *) eaddr);
            break;
        case CPDI_4_BYTE:
            *val = CPS_UncachedRead32 ((volatile uint32_t *) eaddr);
            break;
        default: return  EINVAL;
    }

    return 0;
}

/****************************************************************************/
/* ECAM config write with ARI form of addressing                            */
/****************************************************************************/
static uint32_t CPDI_DoAriEcamConfigWrite (
                         void    *pD  ,
                         uint32_t bus   ,
                         uint32_t func  ,
                         uint32_t offset,
                         CPDI_SizeInBytes size  ,
                         uint32_t data  ) {

    uintptr_t pcieAddr;
    uintptr_t eaddr;
    if (! pD)
        return EINVAL;
    if ((bus    > CPDI_ECAM_MAX_BUS     ) ||
        (func   > CPDI_ARI_ECAM_MAX_FUNC) ||
        (offset > CPDI_ECAM_MAX_OFFSET  ) )
        return EINVAL;

    pcieAddr = ((pvt_data_t *) pD)->p_ax_base;

    /**
     * Hardware is strapped (only) for ECAM addressing
     */
    eaddr = ARI_ECAMADDR (pcieAddr, bus, func, offset);

    /**
     * write the required width
     */
    switch (size) {
        case CPDI_1_BYTE:
            CPS_UncachedWrite8  ((volatile uint8_t  *) eaddr,
                                 (uint8_t) data);
            break;
        case CPDI_2_BYTE:
            CPS_UncachedWrite16 ((volatile uint16_t *) eaddr,
                                 (uint16_t) data);
            break;
        case CPDI_4_BYTE:
            CPS_UncachedWrite32 ((volatile uint32_t *) eaddr,
                                 data);
            break;
        default: return EINVAL;
    }

    if (CPDI_CallErrorCallbackIfError (pD))
        return EINVAL;

    return 0;
}


/****************************************************************************/
/* Config write                                                             */
/****************************************************************************/
uint32_t CPDI_DoConfigWrite (void    *pD  ,
                             uint32_t bus   ,
                             uint32_t dev   ,
                             uint32_t func  ,
                             uint32_t offset,
                             CPDI_SizeInBytes size  ,
                             uint32_t data  ) {
    CPDI_CoreEpOrRp epOrRp;

    if (! pD)
        return EINVAL;

    if (CPDI_IsCoreStrappedAsEpOrRp (pD, & epOrRp) || epOrRp == CPDI_CORE_EP_STRAPPED )
        return EINVAL;



    if (CPDI_DoEcamConfigWrite (pD, bus, dev, func, offset, size, data)) {
        return  EINVAL;
    }



    return 0;
}

/****************************************************************************/
/* Config read                                                              */
/****************************************************************************/
uint32_t CPDI_DoConfigRead  (void    *pD  ,
                             uint32_t bus   ,
                             uint32_t dev   ,
                             uint32_t func  ,
                             uint32_t offset,
                             CPDI_SizeInBytes size  ,
                             uint32_t * data  ) {

    CPDI_CoreEpOrRp epOrRp;
    uint32_t var;

    if (! pD)
        return EINVAL;
    if (! data)
        return EINVAL;

    if (CPDI_IsCoreStrappedAsEpOrRp (pD, & epOrRp) || epOrRp == CPDI_CORE_EP_STRAPPED )
        return EINVAL;



    if (CPDI_DoEcamConfigRead (pD, bus, dev, func, offset, size, & var)) {

        *data = 0;
        return EINVAL;
    }

    *data = var;



    return 0;
}


/****************************************************************************/
/* ARI Config write                                                         */
/****************************************************************************/
uint32_t CPDI_DoAriConfigWrite (void    *pD  ,
                             uint32_t bus   ,
                             uint32_t func  ,
                             uint32_t offset,
                             CPDI_SizeInBytes size  ,
                             uint32_t data  ) {
    CPDI_CoreEpOrRp epOrRp;

    if (! pD)
        return EINVAL;

    if (CPDI_IsCoreStrappedAsEpOrRp (pD, & epOrRp) || epOrRp == CPDI_CORE_EP_STRAPPED )
        return EINVAL;



    if (CPDI_DoAriEcamConfigWrite (pD, bus, func, offset, size, data)) {
        return  EINVAL;
    }



    return 0;
}

/****************************************************************************/
/* ARI Config read                                                          */
/****************************************************************************/
uint32_t CPDI_DoAriConfigRead  (void    *pD  ,
                             uint32_t bus   ,
                             uint32_t func  ,
                             uint32_t offset,
                             CPDI_SizeInBytes size,
                             uint32_t * data  ) {

    CPDI_CoreEpOrRp epOrRp;
    uint32_t var;

    if (! pD)
        return EINVAL;
    if (! data)
        return EINVAL;

    if (CPDI_IsCoreStrappedAsEpOrRp (pD, & epOrRp) || epOrRp == CPDI_CORE_EP_STRAPPED )
        return EINVAL;



    if (CPDI_DoAriEcamConfigRead (pD, bus, func, offset, size, & var)) {

        *data = 0;
        return EINVAL;
    }

    *data = var;



    return 0;
}


/****************************************************************************/
/* Update BAR registers                                                     */
/****************************************************************************/
uint32_t CPDI_EnableBarAccess (void * pD,
                               uint32_t bus ,
                               uint32_t dev ,
                               uint32_t func,
                               uint32_t bar ,
                               uint32_t addr_lo) {

    unsigned v, reg, io, memtype, sz, v2, sz2, r, base64 = 3;
    CPDI_CoreEpOrRp epOrRp;

    if (! pD)
        return EINVAL;
     if (CPDI_IsCoreStrappedAsEpOrRp (pD, & epOrRp) || epOrRp == CPDI_CORE_EP_STRAPPED )
        return EINVAL;

    reg = 0x10 + (bar << 2);

    CPDI_DoConfigRead (pD, bus, dev, func, reg, CPDI_4_BYTE, (uint32_t *) & v);

    io = v & 1;
    memtype = (v >> 1) & 3;

    if(io) {
        return EINVAL;
    } else {
        switch(memtype) {
            case 0:
                /**
                 * get size
                 */
                CPDI_DoConfigWrite (pD, bus, dev, func, reg, CPDI_4_BYTE, 0xfffffff0 | v);
                CPDI_DoConfigRead  (pD, bus, dev, func, reg, CPDI_4_BYTE,(uint32_t *) & r);

                if(r & 0xfffffff0)
                     sz = ~(r & ~0xf) + 1;
                else sz = 0;
                /**
                 * restore BAR
                 */
                CPDI_DoConfigWrite (pD, bus, dev, func, reg, CPDI_4_BYTE, v);

                if(sz) {
                    /**
                     * map it and set BAR
                     */
                    unsigned tmp = 0x10 + (bar << 2);

                    CPDI_DoConfigWrite (pD, bus, dev, func, tmp  , CPDI_4_BYTE, addr_lo | (v & 0xf));

                }
                break;
            case 2:
                /**
                 * get size
                 */
                CPDI_DoConfigRead  (pD, bus, dev, func, reg+4, CPDI_4_BYTE,(uint32_t *) & v2);
                CPDI_DoConfigWrite (pD, bus, dev, func, reg  , CPDI_4_BYTE, 0xfffffff0 | v);
                CPDI_DoConfigWrite (pD, bus, dev, func, reg+4, CPDI_4_BYTE, 0xffffffff);
                CPDI_DoConfigRead  (pD, bus, dev, func, reg  , CPDI_4_BYTE,(uint32_t *) & r);
                sz = ~(r & ~0xf) + 1;
                CPDI_DoConfigRead  (pD, bus, dev, func, reg+4, CPDI_4_BYTE,(uint32_t *) & r);

                if(r)
                     sz2 = ~r;
                else sz2 =  0;

                /**
                 * restore BARs
                 */
                CPDI_DoConfigWrite (pD, bus, dev, func, reg  , CPDI_4_BYTE, v );
                CPDI_DoConfigWrite (pD, bus, dev, func, reg+4, CPDI_4_BYTE, v2);

                /**
                 * map it
                 */
                if(!sz2 && (sz < 0x00400000)) {

                    /**
                     * map it as 32-bit
                     */
                    unsigned tmp = 0x10 + (bar << 2);

                    CPDI_DoConfigWrite (pD, bus, dev, func, tmp  , CPDI_4_BYTE, addr_lo | (v & 0xf));
                    CPDI_DoConfigWrite (pD, bus, dev, func, reg+4, CPDI_4_BYTE, 0);

                } else {
                    /**
                     * map it as full 64-bit
                     */
                    CPDI_DoConfigWrite (pD, bus, dev, func, reg+4, CPDI_4_BYTE, base64);
                    CPDI_DoConfigWrite (pD, bus, dev, func, reg  , CPDI_4_BYTE, v     );
                }
                break;
            default:
                /**
                 * undefined memory type from device
                 */
                return EINVAL;
        }
        /**
         * Enable memory access and  Bus Mastering
         */
        CPDI_DoConfigRead(pD, bus, dev, func, PCI_RC_COMMAND, CPDI_4_BYTE, (uint32_t *) & v);
		v |= 0x06; // command =0x0006, Bus master, memory
        CPDI_DoConfigWrite(pD, bus, dev, func, PCI_RC_COMMAND, CPDI_4_BYTE, v);
    }

    return 0;
}

/****************************************************************************/
/* Read the value of RP BAR register mentioned in 'bar' parameter           */
/****************************************************************************/
uint32_t CPDI_GetRootPortBAR (void    *pD,
               uint32_t bar ,
               uint32_t *barVal) {

    struct pcie_RC_addrmap__regf_rc_pcie_base *pcieAddr;
    CPDI_CoreEpOrRp epOrRp;

    if ((!pD) || (!barVal))
        return EINVAL;
    if (CPDI_IsCoreStrappedAsEpOrRp (pD, & epOrRp) || epOrRp == CPDI_CORE_EP_STRAPPED )
        return EINVAL;

    pcieAddr = (struct pcie_RC_addrmap__regf_rc_pcie_base *)
        (((uintptr_t)((pvt_data_t *) pD)->p_pcie_base) + CPDI_REGS_OFFSET_RP_MANAGEMENT);
    switch (bar) {
    case 0:
        CPDI_REG_WR (&(pcieAddr->i_RC_BAR_0), 0xFFFFFFF0);
        *barVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_RC_BAR_0));
        break;
    case 1:
        CPDI_REG_WR (&(pcieAddr->i_RC_BAR_1), 0xFFFFFFF0);
        *barVal =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_RC_BAR_1));
        break;
    default:
        return EINVAL;
        break;
    }
    return 0;
}

/****************************************************************************/
/* Update BAR registers on RP                                               */
/* This is using local management bus, no need for config writes            */
/****************************************************************************/
uint32_t CPDI_EnableRpMemBarAccess (void * pD,
                               uint32_t bar ,
                               uint32_t addr_hi,
                               uint32_t addr_lo) {

    struct pcie_RC_addrmap__regf_rc_pcie_base *pcieAddr;
    CPDI_CoreEpOrRp epOrRp;
    uint32_t barsize;

    if (! pD)
        return EINVAL;

    if (CPDI_IsCoreStrappedAsEpOrRp (pD, & epOrRp) || epOrRp == CPDI_CORE_EP_STRAPPED )
        return EINVAL;

    pcieAddr = (struct pcie_RC_addrmap__regf_rc_pcie_base *)
        (((uintptr_t)((pvt_data_t *) pD)->p_pcie_base) + CPDI_REGS_OFFSET_RP_MANAGEMENT);
    switch (bar) {
    case 0:
        CPDI_REG_WR (&(pcieAddr->i_RC_BAR_0), 0xFFFFFFF0);
        barsize =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_RC_BAR_0));
        if (barsize & 0x1) {
            /* Bar is configured for IO access, fail */
            return EINVAL;
        }
        if ( (~barsize & addr_lo)&0xFFFFFFF0 ) {
            /* Some bits in requested address are not writeable, fail. */
            return EINVAL;
        }
        if (barsize & 0x4) {
            /* Bar is configured for 64bits, write the upper address in bar 1 */
            CPDI_REG_WR (&(pcieAddr->i_RC_BAR_1), 0xFFFFFFF0);
            barsize = (uint32_t) CPDI_REG_RD (&(pcieAddr->i_RC_BAR_1));
            if ( (~barsize & addr_hi)&0xFFFFFFF0 ) {
                /* Some bits in requested address are not writeable, fail. */
                return EINVAL;
            }
            CPDI_REG_WR (&(pcieAddr->i_RC_BAR_1),PCIE_RC_ADDRMAP__RC_BAR_1__BAMR0__WRITE(addr_hi>>4));
        }
        CPDI_REG_WR (&(pcieAddr->i_RC_BAR_0),PCIE_RC_ADDRMAP__RC_BAR_0__BAMR0__WRITE(addr_lo>>4));
        break;
    case 1:
        CPDI_REG_WR (&(pcieAddr->i_RC_BAR_1), 0xFFFFFFF0);
        barsize =  (uint32_t) CPDI_REG_RD (&(pcieAddr->i_RC_BAR_1));
        if (barsize & 0x1) {
            /* Bar is configured for IO access, fail */
            return EINVAL;
        }
        if (barsize & 0x4) {
            /* Bar is configured for 64bits, fail*/
            return EINVAL;
        }
        if ( (~barsize & addr_lo)&0xFFFFFFF0 ) {
            /* Some bits in requested address are not writeable, fail. */
            return EINVAL;
        }
        CPDI_REG_WR (&(pcieAddr->i_RC_BAR_1),PCIE_RC_ADDRMAP__RC_BAR_1__BAMR0__WRITE(addr_lo>>4));
        break;

    default:
        return EINVAL;
        break;
    }
    return 0;
}



