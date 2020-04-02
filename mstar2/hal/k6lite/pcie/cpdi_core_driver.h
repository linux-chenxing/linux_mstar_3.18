/**********************************************************************
 * copyright (C) 2012-2014 Cadence Design Systems
 * All rights reserved.
 ***********************************************************************
 * cpd_core_driver.h
 *
 * Cadence PCIe Core Driver
 * Header for main source file.
 ***********************************************************************/

 

#ifndef __CPDI_CORE_DRIVER_H__
#define __CPDI_CORE_DRIVER_H__

#include "cdn_stdint.h"
#include "mstar_pcie_addrmap.h"

/****************************************************************************/
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/****************************************************************************/
/*                                MACROS                                    */
/****************************************************************************/
/****************************************************************************/

#define CPDI_REG_RD(x)      CPS_UncachedRead32((volatile uint32_t *) x)
#define CPDI_REG_WR(x,y)    CPS_UncachedWrite32((volatile uint32_t *) x, (uint32_t) y)


/** ECAM address shift for the Bus number
 */
#define S_ECAM_B                                       20

/** ECAM address shift for the Device number
 */
#define S_ECAM_D                                       15

/** ECAM address shift for the Function number
 */
#define S_ECAM_F                                       12

/** Form the ECAM address
 */
#define ECAMADDR(base, bus, device, function, reg) \
                                             ( base                           | \
                                             ((bus      & 0xff ) << S_ECAM_B) | \
                                             ((device   & 0x1f ) << S_ECAM_D) | \
                                             ((function & 0x7  ) << S_ECAM_F) | \
                                             ((reg      & 0xfff)            ) )

/** Form the ECAM address for ARI devices
 */
#define ARI_ECAMADDR(base, bus, function, reg) \
                                             ( base                           | \
                                             ((bus      & 0xff ) << S_ECAM_B) | \
                                             ((function & 0xff ) << S_ECAM_F) | \
                                             ((reg      & 0xfff)            ) )






/****************************************************************************/
/* Settings of the credit limit, includes name, direction and read/wite info*/
/****************************************************************************/
typedef enum {

    CPD_PPC_RCV_READ       = 0x000,
    CPD_PPC_RCV_WRITE      = 0x001,
    CPD_PPC_TRANSM_READ    = 0x010,
    CPD_PPC_TRANSM_WRITE   = 0x011,
    CPD_PHC_RCV_READ       = 0x100,
    CPD_PHC_RCV_WRITE      = 0x101,
    CPD_PHC_TRANSM_READ    = 0x110,
    CPD_PHC_TRANSM_WRITE   = 0x111,
    CPD_NPPC_RCV_READ      = 0x200,
    CPD_NPPC_RCV_WRITE     = 0x201,
    CPD_NPPC_TRANSM_READ   = 0x210,
    CPD_NPPC_TRANSM_WRITE  = 0x211,
    CPD_NPHC_RCV_READ      = 0x300,
    CPD_NPHC_RCV_WRITE     = 0x301,
    CPD_NPHC_TRANSM_READ   = 0x310,
    CPD_NPHC_TRANSM_WRITE  = 0x311,
    CPD_CPC_RCV_READ       = 0x400,
    CPD_CPC_RCV_WRITE      = 0x401,
    CPD_CPC_TRANSM_READ    = 0x410,
    CPD_CPC_TRANSM_WRITE   = 0x411,
    CPD_CHC_RCV_READ       = 0x500,
    CPD_CHC_RCV_WRITE      = 0x501,
    CPD_CHC_TRANSM_READ    = 0x510,
    CPD_CHC_TRANSM_WRITE   = 0x511

} cred_lim_setting;

/****************************************************************************/
/* Macro to modify trafic class bits for AXI wrapper descriptor 0          */
/****************************************************************************/
#define CPD_AXI_WRAPPER_OB_REGION_TRAFFIC_CLASS_DATA_MODIFY(dst, src) \
                    (dst) = ((dst) &\
                    ~0x000E0000U) | (((uint32_t)(src) << 17)&\
                    0x000E0000U) 
 
 


/****************************************************************************/
/* Private Data Struct                                                      */
/****************************************************************************/
typedef struct {
    UINT64_T                                    p_pcie_base;
    struct pcie_LM_addrmap__regf_lm_pcie_base * p_lm_base  ;
    struct cdn_axi_wrapper_ob                 * p_aw_base  ;
    struct pcie_udma_addrmap                  * p_ud_base  ;
    uintptr_t                                   p_ax_base  ; //ECAM address(32bit) 

    CPDI_ErrorCallback                          p_ec_func  ;
} pvt_data_t;


/****************************************************************************/
/****************************************************************************/
/*                                  API                                     */
/****************************************************************************/
/****************************************************************************/
uint32_t CPDI_Probe   (UINT64_T cfg,
                       CPDI_SysReq *sysReq);

uint32_t CPDI_Start   (void * r_pD,
                       CPDI_MaskableLocalInterruptConditions maskableIntrParam);

uint32_t CPDI_Stop    (void * r_pD,
                       CPDI_MaskableLocalInterruptConditions maskableIntrParam);

uint32_t CPDI_Isr     (void * r_pD);

void      CPDI_Destroy (void * r_pD);

uint32_t CPDI_Init    (void *r_pD,
                       UINT64_T cfg,
                       CPDI_InitParam *initValues);

/****************************************************************************/
/* PHYSICAL LAYER TRAINING INFORMATION                                      */
/****************************************************************************/

uint32_t CPDI_IsLinkTrainingComplete (
                           void *r_pD,
                           CPDI_Bool *trainingComplete);

uint32_t CPDI_GetLinkTrainingState (
                           void *r_pD,
                           CPDI_LtssmState *ltssmState);

uint32_t CPDI_GetLinkTrainingDirection (
                           void *r_pD,
                           CPDI_UpstreamOrDownstream *upOrDown);

uint32_t CPDI_IsCoreStrappedAsEpOrRp (
                           void *r_pD,
                           CPDI_CoreEpOrRp *epOrRp);


/****************************************************************************/
/* PHYSICAL LAYER LANE COUNT AND LINK SPEED INFORMATION                     */
/****************************************************************************/



uint32_t CPDI_GetNegotiatedLinkSpeed (
                           void *r_pD,
                           CPDI_LinkSpeed *ns);



uint32_t CPDI_AreLaneNumbersReversed (
                           void *r_pD,
			   CPDI_Bool *reversed);


/****************************************************************************/
/* PHYSICAL LAYER REMOTE INFO RECEIVED DURING TRAINING                      */
/****************************************************************************/

uint32_t CPDI_GetReceivedLinkId (
                           void *r_PD,
                           uint32_t *linkId);


uint32_t CPDI_IsRemoteLinkwidthUpconfigurable (
                           void *r_PD,
                           CPDI_Bool *upconfig);


/****************************************************************************/
/* PHYSICAL_LAYER INFO SENT DURING TRAINING                                 */
/****************************************************************************/

uint32_t CPDI_AccessTransmittedLinkId (
                           void *r_pD,
                           CPDI_ReadOrWrite readOrWrite,
                           uint8_t *linkIdRdOrToWrite);


/****************************************************************************/
/* VENDOR AND SUBSYSTEM VENDOR IDs                                          */
/****************************************************************************/

uint32_t CPDI_AccessVendorIdSubsystemVendorId (
                          void *r_pD,
                          CPDI_AdvertisedIdType id,
                          CPDI_ReadOrWrite readOrWrite,
                          uint16_t *valRdOrToWrite);
                         

/****************************************************************************/
/* TIMING PARAMS INCLUDE TIMEOUTS, DELAYS, LATENCY SETTINGS AND SCALES      */
/****************************************************************************/

uint32_t CPDI_AccessTimingParams (
                          void *r_pD,
                          CPDI_TimeoutAndDelayValues timeoutAndDelayaParam,
                          CPDI_ReadOrWrite readOrWrite,
                          uint32_t *valRdOrToWrite);

uint32_t CPDI_AccessL0sTimeout (
                          void *r_pD,
                          CPDI_ReadOrWrite readOrWrite,
                          uint32_t *valRdOrToWrite);


/****************************************************************************/
/* TRANSITION INTO L0s                                              */
/****************************************************************************/

uint32_t  CPDI_DisableRpTransitionToL0s (
                          void *r_pD);



/****************************************************************************/
/* RP RETRAINING CONTROL                                                    */
/****************************************************************************/
        


 
/****************************************************************************/
/* TRANSMIT AND RECEIVE CREDIT LIMITS AND UPDATE INTERVAL ROUTINES          */
/****************************************************************************/

uint32_t CPDI_AccessCreditLimitSettings (
                          void *r_pD,
                          CPDI_CreditLimitTypes creditLimitParam,
                          CPDI_TransmitOrReceive transmitOrReceive,
                          CPDI_ReadOrWrite readOrWrite,
                          uint8_t virtualChannelNumber,
                          uint32_t *set_or_returned_value);

uint32_t CPDI_AccessTransmitCreditUpdateIntervalSettings (
                          void *r_pD,
                          CPDI_CreditUpdateIntervals creditUpdateIntervalParam,
                          CPDI_ReadOrWrite readOrWrite,
                          uint32_t *set_or_returned_value);


/****************************************************************************/
/* BAR APERTURE AND CONTROL CONFIGURATION                                   */
/****************************************************************************/

uint32_t CPDI_AccessFunctionBarApertureSetting (
                         void *r_pD,
                         uint32_t        funcNumber,
                         CPDI_BarNumber barNumber,
                         CPDI_ReadOrWrite readOrWrite,
                         CPDI_BarApertureSize *aperture_size);

uint32_t CPDI_AccessFunctionBarControlSetting (
                         void *r_pD,
                         uint32_t        funcNumber,
                         CPDI_BarNumber barNumber,
                         CPDI_ReadOrWrite readOrWrite,
                         CPDI_BarControl *bar_control);

uint32_t CPDI_AccessRootPortBarApertureSetting (
                         void *r_pD,
                         CPDI_RpBarNumber barNumber,
                         CPDI_ReadOrWrite readOrWrite,
                         CPDI_RpBarApertureSize *aperture_size);

uint32_t CPDI_AccessRootPortBarControlSetting (
                         void *r_pD,
                         CPDI_RpBarNumber barNumber,
                         CPDI_ReadOrWrite readOrWrite,
                         CPDI_RpBarControl *rp_bar_control);


uint32_t CPDI_AccessRootPortType1ConfigSetting (
                         void  *r_pD,
                         CPDI_ReadOrWrite readOrWrite,
                         CPDI_RpType1ConfigControl *rp_type1_config);

uint32_t CPDI_ControlRootPortBarCheck (
                         void *r_pD,
                         CPDI_EnableOrDisable enOrDisen);

                       
/****************************************************************************/
/* DEBUG CONTROL                                                            */
/****************************************************************************/

uint32_t CPDI_ControlRpMasterLoopback (
                         void *r_pD,
                         CPDI_EnableOrDisable enOrDisen);

uint32_t CPDI_AccessDebugMux (
                         void *r_pD,
                         CPDI_ReadOrWrite readOrWrite,
                         CPDI_DebugMuxModuleSelect *valRdOrToWrite);

uint32_t CPDI_ControlDebugParams (
                         void *r_pD,
                         CPDI_DebugParams debug_controllable_param,
                         CPDI_EnableOrDisable enOrDisen);


/****************************************************************************/
/* COUNT STATISTICS ROUTINES                                                */
/****************************************************************************/

uint32_t CPDI_AccessSavedCountValues (
                         void *r_PD,
                         CPDI_SavedCountParams countParam,
                         CPDI_Bool reset_after_read,
                         CPDI_ReadOrWrite readOrWrite,
                         uint32_t *returnedCount);


/****************************************************************************/
/* EVENT MASKING ROUTINES                                                   */
/****************************************************************************/

uint32_t CPDI_ControlReportingOfAllPhyErrors (
                         void *r_pD,
                         CPDI_EnableOrDisable enOrDisen);

uint32_t CPDI_ControlTxSwing (
                         void *r_pD,
                         CPDI_EnableOrDisable enOrDisen);

uint32_t CPDI_ControlMaskingOfLocalInterrupts (
                         void *r_pD,
                         CPDI_MaskableLocalInterruptConditions maskableIntrParam,
                         CPDI_MaskOrUnmask maskOrUnmask);

uint32_t CPDI_AreThereLocalErrors (
                         void *r_pD,
                         CPDI_Bool *localErrorsPresent,
                         uint32_t *errorType);

uint32_t  CPDI_IsLocalError (
                         void *r_pD,
                         CPDI_MaskableLocalInterruptConditions errorCond,
                         CPDI_Bool *localError);

uint32_t  CPDI_ResetLocalErrorStatusCondition (
                         void *r_pD,
                         CPDI_MaskableLocalInterruptConditions maskableIntrParam);





/****************************************************************************/
/* AXI 3 WRAPPER SETUP                                                      */
/****************************************************************************/
uint32_t CPDI_UpdateObWrapperTrafficClass (void   *r_pD,
                         uint8_t   region_num   ,
                         uint8_t   trafficClass_num);

uint32_t CPDI_SetupObWrapperMemIoAccess (
                         void *r_pD,
                         uint8_t region_num,
                         uint8_t num_passed_bits,
                         uint32_t upper_addr,
                         uint32_t lower_addr);

uint32_t CPDI_SetupObWrapperMessageAccess (
                         void *r_pD,
                         uint8_t region_num,
                         uint32_t upper_addr,
                         uint32_t lower_addr);

uint32_t CPDI_SetupObWrapperConfigAccess (
                         void *r_pD,
                         uint8_t regionNum,
                         uint8_t num_passedBits,
                         uint32_t busdevfuncAddr);

uint32_t CPDI_SetupIbRootPortAddrTranslation (
                         void *r_pD,
                         uint8_t region_num,
                         uint8_t num_passed_bits,
                         uint32_t upper_addr,
                         uint32_t lower_addr);




uint32_t CPDI_DoConfigWrite (
                         void *pD,
                         uint32_t bus,
                         uint32_t dev,
                         uint32_t func,
                         uint32_t offset,
                         CPDI_SizeInBytes size,
                         uint32_t data);

uint32_t CPDI_DoConfigRead  (
                         void *pD,
                         uint32_t bus,
                         uint32_t dev,
                         uint32_t func,
                         uint32_t offset,
                         CPDI_SizeInBytes size,
                         uint32_t * data);

uint32_t CPDI_DoAriConfigWrite (
                         void *pD,
                         uint32_t bus,
                         uint32_t func,
                         uint32_t offset,
                         CPDI_SizeInBytes size,
                         uint32_t data);

uint32_t CPDI_DoAriConfigRead  (
                         void *pD,
                         uint32_t bus,
                         uint32_t func,
                         uint32_t offset,
                         CPDI_SizeInBytes size,
                         uint32_t * data);


uint32_t CPDI_EnableBarAccess (
                         void *pD,
                         uint32_t bus,
                         uint32_t dev,
                         uint32_t func,
                         uint32_t bar,
                         uint32_t addr_lo);

uint32_t CPDI_GetRootPortBAR (
                         void    *pD,
                         uint32_t bar ,
                         uint32_t *barVal);


uint32_t CPDI_EnableRpMemBarAccess (
                             void * pD,
                             uint32_t bar,
                             uint32_t addr_hi,
                             uint32_t addr_lo);



/****************************************************************************/
/* end                                                                      */
/****************************************************************************/

#endif /* __CPDI_CORE_DRIVER_H__ */
