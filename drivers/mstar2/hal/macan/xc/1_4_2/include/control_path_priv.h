/**
* This product contains one or more programs protected under international
* and U.S. copyright laws as unpublished works.  They are confidential and
* proprietary to Dolby Laboratories.  Their reproduction or disclosure, in
* whole or in part, or the production of derivative works therefrom without
* the express permission of Dolby Laboratories is prohibited.
*
*             Copyright 2011 - 2015 by Dolby Laboratories.
*                           All rights reserved.
*
*
* @brief Control Path private header file.
* @file control_path_priv.h
*
*/

#ifndef CONTROL_PATH_PRIV_H_
#define CONTROL_PATH_PRIV_H_

//#include <stdlib.h>
//#include <stdio.h>
//#include <assert.h>
#include <KdmTypeFxp.h>
#include <VdrDmApi.h>
#include <CdmTypePriFxp.h>
#include <dm2_x/VdrDmAPIpFxp.h>
#include <target_display_config.h>

#ifdef __cplusplus
extern "C" {
#endif


#define DM_MS_WEIGHT_UNDEFINED_VALUE    0x1FFF
#define LOG2_UI_STEPSIZE                3

#define SDR_DEFAULT_GAMMA      39322 /* 39322 = Gamma 2.4 */
#define SDR_DEFAULT_MIN_LUM     1310 /*     1310 = 0.005 nits*/
#define SDR_DEFAULT_MAX_LUM 26214400 /* 26214400 =   100 nits */


typedef struct cp_context_s cp_context_t;

/*! @brief Library context.
*/
struct cp_context_s
{
    HDmFxp_t      h_dm;      /**<@brief DM context handle        */
    HMdsExt_t     h_mds_ext; /**<@brief DM metadata context      */
    HDmKsFxp_t    h_ks;      /**<@brief DM kernel handle         */
    DmCfgFxp_t    dm_cfg;    /**<@brief DM config handle         */
    Mmg_t         mmg;       /**<@brief Context of DM memory manager */

    pq_config_t* cur_pq_config;
    #if IPCORE
    uint32_t active_area_left_offset  ;
    uint32_t active_area_right_offset ;
    uint32_t active_area_top_offset   ;
    uint32_t active_area_bottom_offset;
    #endif

    int last_view_mode_id;
    signal_format_t last_input_format;
    ui_menu_params_t last_ui_menu_params;
    int config_changed;
    int dbgExecParamsPrintPeriod;
};


#ifdef __cplusplus
}
#endif


#endif //CONTROL_PATH_API_H_
