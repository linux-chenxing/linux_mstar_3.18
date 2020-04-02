/**********************************************************************
 * copyright (C) 2011-2014 Cadence Design Systems
 * All rights reserved.
 ***********************************************************************
 * cpd_main.c
 *
 * Cadence PCIe Core Driver
 * main component function
 ***********************************************************************/





#include "cdn_errno.h"
#include "cdn_stdint.h"

#include "cpdi.h"

#include "cps_v2.h"
#include "cpdi_core_driver.h"

CPDI_OBJ CPDDrv = {

    CPDI_Probe   ,  /* probe    */
    CPDI_Init    ,  /* init     */

    CPDI_Destroy,  /* destroy  */
    CPDI_Start  ,  /* start    */
    CPDI_Stop   ,  /* stop     */
    CPDI_Isr    ,  /* isr      */

    /****************************************************************************/
    /* PHYSICAL LAYER TRAINING INFORMATION                                      */
    /****************************************************************************/

    CPDI_IsLinkTrainingComplete                            ,
    CPDI_GetLinkTrainingState                              ,
    CPDI_GetLinkTrainingDirection                          ,
    CPDI_IsCoreStrappedAsEpOrRp                            ,

    /****************************************************************************/
    /* PHYSICAL LAYER LANE COUNT AND LINK SPEED INFORMATION                     */
    /****************************************************************************/



    CPDI_GetNegotiatedLinkSpeed                            ,



    /****************************************************************************/
    /* PHYSICAL LAYER REMOTE INFO RECEIVED DURING TRAINING                      */
    /****************************************************************************/

    CPDI_GetReceivedLinkId                                 ,


    CPDI_IsRemoteLinkwidthUpconfigurable                   ,


    /****************************************************************************/
    /* PHYSICAL_LAYER INFO SENT DURING TRAINING                                 */
    /****************************************************************************/

    CPDI_AccessTransmittedLinkId                           ,

    /****************************************************************************/
    /* VENDOR AND SUBSYSTEM VENDOR IDs                                          */
    /****************************************************************************/

    CPDI_AccessVendorIdSubsystemVendorId                   ,

    /****************************************************************************/
    /* TIMING PARAMS INCLUDE TIMEOUTS, DELAYS, LATENCY SETTINGS AND SCALES      */
    /****************************************************************************/

    CPDI_AccessTimingParams                                ,
    CPDI_AccessL0sTimeout                                  ,

    /****************************************************************************/
    /* TRANSITION INTO L0S                                                      */
    /****************************************************************************/

    CPDI_DisableRpTransitionToL0s                         ,



    /****************************************************************************/
    /* RP RETRAINING CONTROL                                                    */
    /****************************************************************************/
        



    /****************************************************************************/
    /* TRANSMIT AND RECEIVE CREDIT LIMITS AND UPDATE INTERVAL ROUTINES          */
    /****************************************************************************/

    CPDI_AccessCreditLimitSettings                         ,
    CPDI_AccessTransmitCreditUpdateIntervalSettings        ,

    /****************************************************************************/
    /* BAR APERTURE AND CONTROL CONFIGURATION                                   */
    /****************************************************************************/
    CPDI_AccessRootPortBarApertureSetting               ,
    CPDI_AccessRootPortBarControlSetting                ,
    CPDI_AccessRootPortType1ConfigSetting               ,

    CPDI_ControlRootPortBarCheck                        ,
                       
    /****************************************************************************/
    /* DEBUG CONTROL                                                            */
    /****************************************************************************/

    CPDI_ControlRpMasterLoopback                           ,
    CPDI_AccessDebugMux                                    ,
    CPDI_ControlDebugParams                                ,

    /****************************************************************************/
    /* COUNT STATISTICS ROUTINES                                                */
    /****************************************************************************/

    CPDI_AccessSavedCountValues                            ,

    /****************************************************************************/
    /* EVENT MASKING ROUTINES                                                   */
    /****************************************************************************/

    CPDI_ControlReportingOfAllPhyErrors                    ,
    CPDI_ControlTxSwing                                    ,
    CPDI_ControlMaskingOfLocalInterrupts                   ,
    CPDI_AreThereLocalErrors                               ,
    CPDI_IsLocalError                                      ,
    CPDI_ResetLocalErrorStatusCondition                    ,




/****************************************************************************/
/* AXI Wrapper Functions                                                    */
/****************************************************************************/

    CPDI_UpdateObWrapperTrafficClass                       ,
    CPDI_SetupObWrapperMemIoAccess                         ,
    CPDI_SetupObWrapperMessageAccess                       ,
    CPDI_SetupObWrapperConfigAccess                        ,
    CPDI_SetupIbRootPortAddrTranslation                    ,


    CPDI_DoConfigRead                                      ,
    CPDI_DoConfigWrite                                     ,
    CPDI_DoAriConfigRead                                   ,
    CPDI_DoAriConfigWrite                                  ,
    CPDI_EnableBarAccess                                   ,
    CPDI_GetRootPortBAR                                    ,
    CPDI_EnableRpMemBarAccess                              ,



};

CPDI_OBJ *CPDI_GetInstance(void) {
    return &CPDDrv;
}

