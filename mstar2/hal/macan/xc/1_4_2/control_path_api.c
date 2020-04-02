/**
 * This product contains one or more programs protected under international
 * and U.S. copyright laws as unpublished works.  They are confidential and
 * proprietary to Dolby Laboratories.  Their reproduction or disclosure, in
 * whole or in part, or the production of derivative works therefrom without
 * the express permission of Dolby Laboratories is prohibited.
 * Copyright 2011 - 2015 by Dolby Laboratories.
 * All rights reserved.
 *
 * @brief DoVi Control Path
 * @file control_path_api.c
 *
 * $Id$
 */

//#include <assert.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <stdint.h>
//#include <stdarg.h>


#include <rpu_ext_config.h>
#include <KdmTypeFxp.h>
#include <VdrDmApi.h>
#include <CdmTypePriFxp.h>
#include <control_path_api.h>
#include <control_path_priv.h>
//#include <dm2_x/VdrDmAPIpFxp.h>



static uint32_t (*printLog)(uint32_t, const char *);
#if !(IPCORE)
static uint64_t (*printLog64)(uint64_t, const char *);
#endif
static void (*custom_sprintf)(char*, const char*, ...);


static uint16_t dm_metadata_2_dm_param
(
    dm_metadata_t   *p_dm_md,
    MdsExt_t        *p_mds_ext,
    const DmCfgFxp_t      *p_dm_cfg,
    cp_context_t* p_ctx
)
{
    dm_metadata_base_t* p_base_md = &p_dm_md->base;
    dm_metadata_ext_t* p_ext_md = p_dm_md->ext;
    TargetDisplayConfig_t *p_target_cfg=&p_ctx->cur_pq_config->target_display_config;
    TrimSet_t  *p_trim_set;
    Trim_t     *p_trim;
    int        i;

    p_mds_ext->affected_dm_metadata_id = p_base_md->dm_metadata_id;
    p_mds_ext->scene_refresh_flag = p_base_md->scene_refresh_flag;

    p_mds_ext->m33Yuv2RgbScale2P = 13;
    p_mds_ext->m33Yuv2Rgb[0][0] = (p_base_md->YCCtoRGB_coef0_hi << 8) | p_base_md->YCCtoRGB_coef0_lo;
    p_mds_ext->m33Yuv2Rgb[0][1] = (p_base_md->YCCtoRGB_coef1_hi << 8) | p_base_md->YCCtoRGB_coef1_lo;
    p_mds_ext->m33Yuv2Rgb[0][2] = (p_base_md->YCCtoRGB_coef2_hi << 8) | p_base_md->YCCtoRGB_coef2_lo;
    p_mds_ext->m33Yuv2Rgb[1][0] = (p_base_md->YCCtoRGB_coef3_hi << 8) | p_base_md->YCCtoRGB_coef3_lo;
    p_mds_ext->m33Yuv2Rgb[1][1] = (p_base_md->YCCtoRGB_coef4_hi << 8) | p_base_md->YCCtoRGB_coef4_lo;
    p_mds_ext->m33Yuv2Rgb[1][2] = (p_base_md->YCCtoRGB_coef5_hi << 8) | p_base_md->YCCtoRGB_coef5_lo;
    p_mds_ext->m33Yuv2Rgb[2][0] = (p_base_md->YCCtoRGB_coef6_hi << 8) | p_base_md->YCCtoRGB_coef6_lo;
    p_mds_ext->m33Yuv2Rgb[2][1] = (p_base_md->YCCtoRGB_coef7_hi << 8) | p_base_md->YCCtoRGB_coef7_lo;
    p_mds_ext->m33Yuv2Rgb[2][2] = (p_base_md->YCCtoRGB_coef8_hi << 8) | p_base_md->YCCtoRGB_coef8_lo;

    p_mds_ext->v3Yuv2Rgb[0] = (p_base_md->YCCtoRGB_offset0_byte3 << 24) |
                              (p_base_md->YCCtoRGB_offset0_byte2 << 16) |
                              (p_base_md->YCCtoRGB_offset0_byte1 <<  8) |
                              p_base_md->YCCtoRGB_offset0_byte0;
    p_mds_ext->v3Yuv2Rgb[1] = (p_base_md->YCCtoRGB_offset1_byte3 << 24) |
                              (p_base_md->YCCtoRGB_offset1_byte2 << 16) |
                              (p_base_md->YCCtoRGB_offset1_byte1 <<  8) |
                              p_base_md->YCCtoRGB_offset1_byte0;
    p_mds_ext->v3Yuv2Rgb[2] = (p_base_md->YCCtoRGB_offset2_byte3 << 24) |
                              (p_base_md->YCCtoRGB_offset2_byte2 << 16) |
                              (p_base_md->YCCtoRGB_offset2_byte1 <<  8) |
                              p_base_md->YCCtoRGB_offset2_byte0;

    p_mds_ext->m33Rgb2WpLmsScale2P = 14;
    p_mds_ext->m33Rgb2WpLms[0][0] = (p_base_md->RGBtoLMS_coef0_hi << 8) | p_base_md->RGBtoLMS_coef0_lo;
    p_mds_ext->m33Rgb2WpLms[0][1] = (p_base_md->RGBtoLMS_coef1_hi << 8) | p_base_md->RGBtoLMS_coef1_lo;
    p_mds_ext->m33Rgb2WpLms[0][2] = (p_base_md->RGBtoLMS_coef2_hi << 8) | p_base_md->RGBtoLMS_coef2_lo;
    p_mds_ext->m33Rgb2WpLms[1][0] = (p_base_md->RGBtoLMS_coef3_hi << 8) | p_base_md->RGBtoLMS_coef3_lo;
    p_mds_ext->m33Rgb2WpLms[1][1] = (p_base_md->RGBtoLMS_coef4_hi << 8) | p_base_md->RGBtoLMS_coef4_lo;
    p_mds_ext->m33Rgb2WpLms[1][2] = (p_base_md->RGBtoLMS_coef5_hi << 8) | p_base_md->RGBtoLMS_coef5_lo;
    p_mds_ext->m33Rgb2WpLms[2][0] = (p_base_md->RGBtoLMS_coef6_hi << 8) | p_base_md->RGBtoLMS_coef6_lo;
    p_mds_ext->m33Rgb2WpLms[2][1] = (p_base_md->RGBtoLMS_coef7_hi << 8) | p_base_md->RGBtoLMS_coef7_lo;
    p_mds_ext->m33Rgb2WpLms[2][2] = (p_base_md->RGBtoLMS_coef8_hi << 8) | p_base_md->RGBtoLMS_coef8_lo;

    /* EOTF GAMMA, A, B */
    p_mds_ext->signal_eotf = (p_base_md->signal_eotf_hi << 8) | p_base_md->signal_eotf_lo;
    p_mds_ext->signal_eotf_param0 = (p_base_md->signal_eotf_param0_hi << 8) |
                                    p_base_md->signal_eotf_param0_lo;
    p_mds_ext->signal_eotf_param1 = (p_base_md->signal_eotf_param1_hi << 8) |
                                    p_base_md->signal_eotf_param1_lo;
    p_mds_ext->signal_eotf_param2 = (p_base_md->signal_eotf_param2_byte3 << 24) |
                                    (p_base_md->signal_eotf_param2_byte2 << 16) |
                                    (p_base_md->signal_eotf_param2_byte1 <<  8) |
                                    p_base_md->signal_eotf_param2_byte0;

    /* signal info */
    p_mds_ext->signal_bit_depth       = p_base_md->signal_bit_depth;
    p_mds_ext->signal_color_space     = p_base_md->signal_color_space;
    p_mds_ext->signal_chroma_format   = p_base_md->signal_chroma_format;
    p_mds_ext->signal_full_range_flag = p_base_md->signal_full_range_flag;

    /* source monitor: all PQ scale 4095 */
    p_mds_ext->source_min_PQ   = (p_base_md->source_min_PQ_hi << 8) |
                                 p_base_md->source_min_PQ_lo;
    p_mds_ext->source_max_PQ   = (p_base_md->source_max_PQ_hi << 8) |
                                 p_base_md->source_max_PQ_lo;
    p_mds_ext->source_diagonal = (p_base_md->source_diagonal_hi << 8) |
                                 p_base_md->source_diagonal_lo;

    /* initialize level 1 default value */
    p_mds_ext->min_PQ = p_mds_ext->source_min_PQ;
    p_mds_ext->max_PQ = p_mds_ext->source_max_PQ;
    p_mds_ext->mid_PQ = (p_mds_ext->source_min_PQ + p_mds_ext->max_PQ) >> 1;

    p_mds_ext->num_ext_blocks = p_base_md->num_ext_blocks;

    p_trim_set = &p_mds_ext->trimSets;
    p_trim_set->TrimSetNum = 0;
    p_trim_set->TrimSets[0].TrimNum = 0;
    p_trim_set->TrimSets[0].Trima[0] = p_mds_ext->source_max_PQ;

    p_mds_ext->lvl4GdAvail  = 0;


    p_mds_ext->lvl5AoiAvail = !!(p_target_cfg->tuningMode & TUNINGMODE_FORCE_ABSOLUTE);
    p_mds_ext->dm_run_mode  = !!(p_target_cfg->tuningMode & TUNINGMODE_FORCE_ABSOLUTE);
    p_mds_ext->dm_run_version = 0;
    p_mds_ext->dm_debug0 = 0;
    p_mds_ext->dm_debug0 = 1;
    p_mds_ext->dm_debug0 = 2;
    p_mds_ext->dm_debug0 = 3;

# if CP_EN_AOI
    p_mds_ext->active_area_left_offset   = 0;
    p_mds_ext->active_area_right_offset  = 0;
    p_mds_ext->active_area_top_offset    = 0;
    p_mds_ext->active_area_bottom_offset = 0;
#endif

    for (i = 0; i < p_mds_ext->num_ext_blocks; ++i)
    {
        if ((p_ext_md[i].ext_block_level == 1) && !(p_target_cfg->tuningMode & TUNINGMODE_EXTLEVEL1_DISABLE))
        {
            p_mds_ext->min_PQ = (p_ext_md[i].l.level_1.min_PQ_hi << 8) | p_ext_md[i].l.level_1.min_PQ_lo;
            p_mds_ext->max_PQ = (p_ext_md[i].l.level_1.max_PQ_hi << 8) | p_ext_md[i].l.level_1.max_PQ_lo;
            p_mds_ext->mid_PQ = (p_ext_md[i].l.level_1.avg_PQ_hi << 8) | p_ext_md[i].l.level_1.avg_PQ_lo;
        }
        else if ((p_ext_md[i].ext_block_level == 2 && !p_dm_cfg->tmCtrl.l2off) && !(p_target_cfg->tuningMode & TUNINGMODE_EXTLEVEL2_DISABLE))
        {
            p_trim = &p_trim_set->TrimSets[p_ext_md[i].ext_block_level - 2]; /* lvl = 2 in [0] */
            if (p_trim->TrimNum < p_trim->TrimNumMax)
            {
                /* only get the first TrimNumMax */
                ++(p_trim->TrimNum);
                p_trim->Trima[p_trim->TrimNum * p_trim->TrimTypeDim + TrimTypeTMaxPq2]      = (p_ext_md[i].l.level_2.target_max_PQ_hi << 8)        | p_ext_md[i].l.level_2.target_max_PQ_lo;
                p_trim->Trima[p_trim->TrimNum * p_trim->TrimTypeDim + TrimTypeSlope]        = (p_ext_md[i].l.level_2.trim_slope_hi << 8)           | p_ext_md[i].l.level_2.trim_slope_lo;
                p_trim->Trima[p_trim->TrimNum * p_trim->TrimTypeDim + TrimTypeOffset]       = (p_ext_md[i].l.level_2.trim_offset_hi << 8)          | p_ext_md[i].l.level_2.trim_offset_lo;
                p_trim->Trima[p_trim->TrimNum * p_trim->TrimTypeDim + TrimTypePower]        = (p_ext_md[i].l.level_2.trim_power_hi << 8)           | p_ext_md[i].l.level_2.trim_power_lo;
                p_trim->Trima[p_trim->TrimNum * p_trim->TrimTypeDim + TrimTypeChromaWeight] = (p_ext_md[i].l.level_2.trim_chroma_weight_hi << 8)   | p_ext_md[i].l.level_2.trim_chroma_weight_lo;
                p_trim->Trima[p_trim->TrimNum * p_trim->TrimTypeDim + TrimTypeSatGain]      = (p_ext_md[i].l.level_2.trim_saturation_gain_hi << 8) | p_ext_md[i].l.level_2.trim_saturation_gain_lo;
                p_trim->Trima[p_trim->TrimNum * p_trim->TrimTypeDim + TrimTypeMsWeight]     = (p_ext_md[i].l.level_2.ms_weight_hi << 8)            | p_ext_md[i].l.level_2.ms_weight_lo;
                if ((p_trim->Trima[p_trim->TrimNum * p_trim->TrimTypeDim + TrimTypeMsWeight] & 0x1FFF) == DM_MS_WEIGHT_UNDEFINED_VALUE)
                    p_trim->Trima[p_trim->TrimNum * p_trim->TrimTypeDim + TrimTypeMsWeight] = p_target_cfg->mSWeight;
            }
            ++p_trim_set->TrimSetNum;
        }
        else if ((p_ext_md[i].ext_block_level == 4)  && !(p_target_cfg->tuningMode & TUNINGMODE_EXTLEVEL4_DISABLE))
        {
            p_mds_ext->lvl4GdAvail = 1;
            p_mds_ext->filtered_mean_PQ  = (p_ext_md[i].l.level_4.anchor_PQ_hi    << 8) | p_ext_md[i].l.level_4.anchor_PQ_lo;
            p_mds_ext->filtered_power_PQ = (p_ext_md[i].l.level_4.anchor_power_hi << 8) | p_ext_md[i].l.level_4.anchor_power_lo;

        }
        else if ((p_ext_md[i].ext_block_level == 5)  && !(p_target_cfg->tuningMode & TUNINGMODE_EXTLEVEL5_DISABLE))
        {
            p_mds_ext->lvl5AoiAvail = 1;
# if CP_EN_AOI
            p_mds_ext->active_area_left_offset   = (p_ext_md[i].l.level_5.active_area_left_offset_hi   << 8) | p_ext_md[i].l.level_5.active_area_left_offset_lo;
            p_mds_ext->active_area_right_offset  = (p_ext_md[i].l.level_5.active_area_right_offset_hi  << 8) | p_ext_md[i].l.level_5.active_area_right_offset_lo;
            p_mds_ext->active_area_top_offset    = (p_ext_md[i].l.level_5.active_area_top_offset_hi    << 8) | p_ext_md[i].l.level_5.active_area_top_offset_lo;
            p_mds_ext->active_area_bottom_offset = (p_ext_md[i].l.level_5.active_area_bottom_offset_hi << 8) | p_ext_md[i].l.level_5.active_area_bottom_offset_lo;
#endif
#if IPCORE
            p_ctx->active_area_left_offset   = (p_ext_md[i].l.level_5.active_area_left_offset_hi   << 8) | p_ext_md[i].l.level_5.active_area_left_offset_lo;
            p_ctx->active_area_right_offset  = (p_ext_md[i].l.level_5.active_area_right_offset_hi  << 8) | p_ext_md[i].l.level_5.active_area_right_offset_lo;
            p_ctx->active_area_top_offset    = (p_ext_md[i].l.level_5.active_area_top_offset_hi    << 8) | p_ext_md[i].l.level_5.active_area_top_offset_lo;
            p_ctx->active_area_bottom_offset = (p_ext_md[i].l.level_5.active_area_bottom_offset_hi << 8) | p_ext_md[i].l.level_5.active_area_bottom_offset_lo;
#endif
        }
        else if ((p_ext_md[i].ext_block_level == 255) && (p_mds_ext->lvl255RunModeAvail == 0)) /* only use md if not already set by config file */
        {
            p_mds_ext->lvl255RunModeAvail = 1;
            /* p_mds_ext->dm_run_mode    = p_ext_md[i].l.level_255.dm_run_mode   ;  Nawwar mod
             p_mds_ext->dm_run_version = p_ext_md[i].l.level_255.dm_run_version;
             p_mds_ext->dm_debug0      = p_ext_md[i].l.level_255.dm_debug0     ;
             p_mds_ext->dm_debug1      = p_ext_md[i].l.level_255.dm_debug1     ;
             p_mds_ext->dm_debug2      = p_ext_md[i].l.level_255.dm_debug2     ;
             p_mds_ext->dm_debug3      = p_ext_md[i].l.level_255.dm_debug3     ;*/
        }
    }

    return 0;
}

static uint16_t fill_bypass_comp_cfg(rpu_ext_config_fixpt_main_t * p_comp, int src_bit_depth)
{
    int cmp;
    memset(p_comp, 0, sizeof(*p_comp));
    p_comp->rpu_VDR_bit_depth = 12;
    p_comp->rpu_BL_bit_depth = src_bit_depth;
    p_comp->coefficient_log2_denom = 23;
    p_comp->disable_residual_flag = 1;

    for (cmp = 0; cmp <3; cmp++)
    {
        p_comp->num_pivots[cmp] = 2;
        p_comp->pivot_value[cmp][0] = 0;
        p_comp->pivot_value[cmp][1] = (uint32_t)((1 << p_comp->rpu_BL_bit_depth) - 1);
        p_comp->mapping_idc[cmp] = 0;
        p_comp->poly_order[cmp][0] = 1;
        p_comp->poly_coef_int[cmp][0][0] = 0;
        p_comp->poly_coef[cmp][0][0] = 0;
        p_comp->poly_coef_int[cmp][0][1] = 1;
        if (p_comp->rpu_BL_bit_depth == 8)
            p_comp->poly_coef[cmp][0][1] = 0x7878; /* (4095/(16*255) -1) * 2^23 */
        else
            p_comp->poly_coef[cmp][0][1] = 0x1806; /* (4095/(4*1023) -1) * 2^23 */
    }
    return 0;
}

char cStr[30];
static uint32_t printLog_deb(uint32_t val, const char *paramName)
{
    printf("%-30s\t= 0x%08x\n",paramName,val);
    return(val);
}

#if !(IPCORE)
static uint64_t printLog64_deb(uint64_t val, const char *paramName)
{
    printf("%-30s\t= 0x%016llx\n", paramName, (long long unsigned int)val);
    return(val);
}
#endif

void custom_sprintf_deb(char* string, const char* format, ...)
{
    va_list args;
    va_start (args, format);
    vsnprintf (string, 30, format, args);
    va_end (args);
}

static uint32_t printLog_rel(uint32_t val, const char *paramName)
{
    return(val);
}

#if !(IPCORE)
static uint64_t printLog64_rel(uint64_t val, const char *paramName)
{
    return(val);
}
#endif

void custom_sprintf_rel(char* string, const char* format, ...)
{
}

#if IPCORE
static void dmKs2dmreg(DmKsFxp_t *pKs, register_ipcore_t *p_dm_reg_ipcore, dm_metadata_t *p_src_dm_metadata, cp_context_t* p_ctx)
{
    int left_abs, right_abs, bottom_abs, top_abs;
    int left_rel, right_rel, bottom_rel, top_rel;
    static int frame_nr = 0;

    if ((p_ctx->dbgExecParamsPrintPeriod != 0) && (frame_nr % p_ctx->dbgExecParamsPrintPeriod == 0))
    {
        //printf("\nProcessing Frame %d\n",frame_nr);
        printLog       =  &printLog_deb      ;
        custom_sprintf =  &custom_sprintf_deb;
    }
    else
    {
        printLog       =  &printLog_rel      ;
        custom_sprintf =  &custom_sprintf_rel;
    }

    p_dm_reg_ipcore->SRange           = printLog((uint32_t)((uint16_t)pKs->ksIMap.eotfParam.range | (pKs->ksIMap.eotfParam.rangeMin << 16)),"sRangeMin|sRange");
    p_dm_reg_ipcore->Srange_Inverse   = printLog(pKs->ksIMap.eotfParam.rangeInv,"sRangeInv");
    p_dm_reg_ipcore->Frame_Format_1   = printLog((uint32_t)0,"FrameFormat_1");
    p_dm_reg_ipcore->Frame_Format_2   = printLog((uint32_t)0,"FrameFormat_2");
    p_dm_reg_ipcore->pixDef           = printLog((uint32_t)0,"frameOutPixDef");


    p_dm_reg_ipcore->Y2RGB_Coefficient_1 = printLog((uint32_t)((uint16_t)pKs->ksIMap.m33Yuv2Rgb[0][0] | (pKs->ksIMap.m33Yuv2Rgb[0][1]<<16)) ,"YToRGBC1|YToRGBC0");
    p_dm_reg_ipcore->Y2RGB_Coefficient_2 = printLog((uint32_t)((uint16_t)pKs->ksIMap.m33Yuv2Rgb[0][2] | (pKs->ksIMap.m33Yuv2Rgb[1][0]<<16)) ,"YToRGBC3|YToRGBC2");
    p_dm_reg_ipcore->Y2RGB_Coefficient_3 = printLog((uint32_t)((uint16_t)pKs->ksIMap.m33Yuv2Rgb[1][1] | (pKs->ksIMap.m33Yuv2Rgb[1][2]<<16)) ,"YToRGBC5|YToRGBC4");
    p_dm_reg_ipcore->Y2RGB_Coefficient_4 = printLog((uint32_t)((uint16_t)pKs->ksIMap.m33Yuv2Rgb[2][0] | (pKs->ksIMap.m33Yuv2Rgb[2][1]<<16)) ,"YToRGBC7|YToRGBC6");
    p_dm_reg_ipcore->Y2RGB_Coefficient_5 = printLog((uint32_t)((uint16_t)pKs->ksIMap.m33Yuv2Rgb[2][2] | (pKs->ksIMap.m33Yuv2RgbScale2P<<16)) ,"YToRGBScale|YToRGBC8");

    p_dm_reg_ipcore->Y2RGB_Offset_1    =   printLog((uint32_t)pKs->ksIMap.v3Yuv2RgbOffInRgb[0],"YToRGBOffset_0");
    p_dm_reg_ipcore->Y2RGB_Offset_2    =   printLog((uint32_t)pKs->ksIMap.v3Yuv2RgbOffInRgb[1],"YToRGBOffset_1");
    p_dm_reg_ipcore->Y2RGB_Offset_3    =   printLog((uint32_t)pKs->ksIMap.v3Yuv2RgbOffInRgb[2],"YToRGBOffset_2");

    p_dm_reg_ipcore->EOTF              =   printLog((uint32_t)pKs->ksIMap.eotfParam.eotf,"signaleotf");
    p_dm_reg_ipcore->Sparam_1          =   printLog((uint32_t)0,"s_param1");
    p_dm_reg_ipcore->Sparam_2          =   printLog((uint32_t)0,"s_param2");
    p_dm_reg_ipcore->Sgamma            =   printLog((uint32_t)0,"s_gamma");

    p_dm_reg_ipcore->A2B_Coefficient_1 = printLog((uint32_t)((uint16_t)pKs->ksIMap.m33Rgb2Lms[0][0] | (pKs->ksIMap.m33Rgb2Lms[0][1]<<16)) ,"AtoBCoeff1|AtoBCoeff0");
    p_dm_reg_ipcore->A2B_Coefficient_2 = printLog((uint32_t)((uint16_t)pKs->ksIMap.m33Rgb2Lms[0][2] | (pKs->ksIMap.m33Rgb2Lms[1][0]<<16)) ,"AtoBCoeff3|AtoBCoeff2");
    p_dm_reg_ipcore->A2B_Coefficient_3 = printLog((uint32_t)((uint16_t)pKs->ksIMap.m33Rgb2Lms[1][1] | (pKs->ksIMap.m33Rgb2Lms[1][2]<<16)) ,"AtoBCoeff5|AtoBCoeff4");
    p_dm_reg_ipcore->A2B_Coefficient_4 = printLog((uint32_t)((uint16_t)pKs->ksIMap.m33Rgb2Lms[2][0] | (pKs->ksIMap.m33Rgb2Lms[2][1]<<16)) ,"AtoBCoeff7|AtoBCoeff6");
    p_dm_reg_ipcore->A2B_Coefficient_5 = printLog((uint32_t)((uint16_t)pKs->ksIMap.m33Rgb2Lms[2][2] | (pKs->ksIMap.m33Rgb2LmsScale2P<<16)) ,"AtoBCoeff_scale|AtoBCoeff8");

    p_dm_reg_ipcore->C2D_Coefficient_1   = printLog((uint32_t)((uint16_t)pKs->ksIMap.m33Lms2Ipt[0][0] | (pKs->ksIMap.m33Lms2Ipt[0][1]<<16)) ,"CtoDCoeff1|CtoDCoeff0");
    p_dm_reg_ipcore->C2D_Coefficient_2   = printLog((uint32_t)((uint16_t)pKs->ksIMap.m33Lms2Ipt[0][2] | (pKs->ksIMap.m33Lms2Ipt[1][0]<<16)) ,"CtoDCoeff3|CtoDCoeff2");
    p_dm_reg_ipcore->C2D_Coefficient_3   = printLog((uint32_t)((uint16_t)pKs->ksIMap.m33Lms2Ipt[1][1] | (pKs->ksIMap.m33Lms2Ipt[1][2]<<16)) ,"CtoDCoeff5|CtoDCoeff4");
    p_dm_reg_ipcore->C2D_Coefficient_4   = printLog((uint32_t)((uint16_t)pKs->ksIMap.m33Lms2Ipt[2][0] | (pKs->ksIMap.m33Lms2Ipt[2][1]<<16)) ,"CtoDCoeff7|CtoDCoeff6");
    p_dm_reg_ipcore->C2D_Coefficient_5   = printLog((uint32_t)((uint16_t)pKs->ksIMap.m33Lms2Ipt[2][2] | (pKs->ksIMap.m33Lms2IptScale2P<<16)) ,"CtoDCoeff_scale|CtoDCoeff8");

    p_dm_reg_ipcore->C2D_Offset         = printLog((uint32_t)0,"CtoDOffset");
    p_dm_reg_ipcore->Chroma_Weight      = printLog((uint32_t)(0xFFFF & pKs->ksTMap.chromaWeight),"chroma_weight");
    p_dm_reg_ipcore->mFilter_Scale      = printLog((uint32_t)12,"mFilterScale");
    p_dm_reg_ipcore->msWeight           = printLog((uint32_t)((uint16_t)pKs->ksMs.msEdgeWeight | (pKs->ksMs.msWeight<<16)),"msWeight|msWeightEdge");

    p_dm_reg_ipcore->Hunt_Value         = printLog((uint32_t)((0xFFF & pKs->ksOMap.gain) | ((0xFFF & pKs->ksOMap.offset) << 12)),"huntOffset|huntGain");
    p_dm_reg_ipcore->Saturation_Gain    = printLog((uint32_t)pKs->ksOMap.satGain ,"saturationGain");
    p_dm_reg_ipcore->Min_C_1            = printLog((uint32_t)((pKs->ksOMap.ksGmLut.iMinC2<<16)|(uint16_t)pKs->ksOMap.ksGmLut.iMinC1),"minC2|minC1");
    p_dm_reg_ipcore->Min_C_2            = printLog((uint32_t)pKs->ksOMap.ksGmLut.iMinC3,"minC3");
    p_dm_reg_ipcore->Max_C              = printLog((uint32_t)((pKs->ksOMap.ksGmLut.iMaxC2<<16)|(uint16_t)pKs->ksOMap.ksGmLut.iMaxC1),"maxC2|maxC1");


    p_dm_reg_ipcore->C1_Inverse = printLog(pKs->ksOMap.ksGmLut.iDistC1Inv,"C1Inv");
    p_dm_reg_ipcore->C2_Inverse = printLog(pKs->ksOMap.ksGmLut.iDistC2Inv,"C2Inv");
    p_dm_reg_ipcore->C3_Inverse = printLog(pKs->ksOMap.ksGmLut.iDistC3Inv,"C3Inv");

    /* 13 bit relative offset AOI coordinates from metadata */
    left_abs   = p_ctx->active_area_left_offset;
    right_abs  = p_ctx->active_area_right_offset;
    top_abs    = p_ctx->active_area_top_offset;
    bottom_abs = p_ctx->active_area_bottom_offset;

#if 0
    /*
        AOI relative to absolute convertion for ip_gold_2_09212015
        Top left coordinates.
        X = floor(0 + active_area_left_offset/4096 * XSize/4 + 0.5)
        Y = floor(0 + active_area_top_offset/4096 * YSize/4 + 0.5)
        Bottom right coordinates.
        X = floor(XSize - active_area_right_offset/4096 * XSize/4 + 0.5)
        Y = floor(YSize - active_area_bottom_offset/4096 * YSize/4 + 0.5)
    */
    left_rel   = (left_abs  * 4096*4)/pKs->ksFrmFmtI.colNum;
    top_rel    = (top_abs   * 4096*4)/pKs->ksFrmFmtI.rowNum;
    right_rel  = (right_abs * 4096*4)/pKs->ksFrmFmtI.colNum;
    bottom_rel = (bottom_abs* 4096*4)/pKs->ksFrmFmtI.rowNum;
#else
    /* AOI relative offset to absolute coordinates conversion */
    left_rel   = left_abs;
    top_rel    = top_abs;
    right_rel  = pKs->ksFrmFmtI.colNum - right_abs - 1;
    bottom_rel = pKs->ksFrmFmtI.rowNum - bottom_abs - 1;
#endif
    /* Clip to 4095. But AOI should never be larger than a quarter of the image */
    if (left_rel   > 4095) left_rel   = 4095;
    if (top_rel    > 4095) top_rel    = 4095;
    if (right_rel  > 4095) right_rel  = 4095;
    if (bottom_rel > 4095) bottom_rel = 4095;

    p_dm_reg_ipcore->Active_area_left_top       =   printLog((uint32_t)(left_rel  | (top_rel   <<12)) ,"AOI_left_top  ");
    p_dm_reg_ipcore->Active_area_bottom_right   =   printLog((uint32_t)(right_rel | (bottom_rel<<12)) ,"AOI_right_bottom ");

    frame_nr++;
}

static uint16_t comp_cfg_2_comp_reg
(
    rpu_ext_config_fixpt_main_t *p_md,
    register_ipcore_t *p_reg,
    int disable_el, int xres, int yres, cp_context_t* p_ctx
)
{
    int i,j;
    static int frame_nr = 0;

    uint32_t temp;

    if ((p_ctx->dbgExecParamsPrintPeriod != 0) && (frame_nr % p_ctx->dbgExecParamsPrintPeriod == 0))
    {
        printLog       =  &printLog_deb      ;
        custom_sprintf =  &custom_sprintf_deb;
    }
    else
    {
        printLog       =  &printLog_rel      ;
        custom_sprintf =  &custom_sprintf_rel;
    }
    p_reg->Composer_Mode              = printLog((uint32_t)((disable_el) || p_md->disable_residual_flag),"composer_mode");
    p_reg->VDR_Resolution             = printLog((uint32_t)((yres<<13) | xres),"vdr_yres|vdr_xres");
    p_reg->Bit_Depth                  = printLog((uint32_t)((p_md->rpu_VDR_bit_depth<<8) |
                                        (p_md->rpu_EL_bit_depth<<4) |
                                        p_md->rpu_BL_bit_depth),"VDR_bpp|EL_bpp|BL_bpp");
    p_reg->Coefficient_Log2_Denominator  = printLog((uint32_t)p_md->coefficient_log2_denom,"coefficient_log2_denom");
    p_reg->BL_Num_Pivots_Y            = printLog((uint32_t)p_md->num_pivots[0],"BL_Pivot_Number");

    temp          =(p_md->pivot_value[0][1]<<10) | p_md->pivot_value[0][0];
    p_reg->BL_Pivot[0] = printLog(temp,"bl_pivot_y1|bl_pivot_y0");
    temp          =(p_md->pivot_value[0][3]<<10) | p_md->pivot_value[0][2];
    p_reg->BL_Pivot[1] = printLog(temp,"bl_pivot_y3|bl_pivot_y2");
    temp          =(p_md->pivot_value[0][5]<<10) | p_md->pivot_value[0][4];
    p_reg->BL_Pivot[2] = printLog(temp,"bl_pivot_y5|bl_pivot_y4");
    temp          =(p_md->pivot_value[0][7]<<10) | p_md->pivot_value[0][6];
    p_reg->BL_Pivot[3] = printLog(temp,"bl_pivot_y7|bl_pivot_y6");
    p_reg->BL_Pivot[4] = printLog(p_md->pivot_value[0][8],"bl_pivot_y8");

    temp  =  p_md->poly_order[0][0];
    temp |= (p_md->poly_order[0][1]<<2);
    temp |= (p_md->poly_order[0][2]<<4);
    temp |= (p_md->poly_order[0][3]<<6);
    temp |= (p_md->poly_order[0][4]<<8);
    temp |= (p_md->poly_order[0][5]<<10);
    temp |= (p_md->poly_order[0][6]<<12);
    temp |= (p_md->poly_order[0][7]<<14);
    p_reg->BL_Order = printLog(temp,"bl_order_y");

    for(i=0 ; i < 8; i++)
    {
        for(j=0 ; j < 3; j++)
        {
            custom_sprintf(cStr,"bl_coeff_y%d%d",i,j);
            p_reg->BL_Coefficient_Y[i][j]   = printLog((uint32_t)(((p_md->poly_coef_int[0][i][j]<<p_md->coefficient_log2_denom) | p_md->poly_coef[0][i][j]) & 0x3FFFFFFF),cStr);
        }
    }
    p_reg->EL_NLQ_Offset_Y            = printLog((uint32_t)p_md->NLQ_offset[0],"el_nlq_offset_y");
    p_reg->EL_Coefficient_Y[0]        = printLog((uint32_t)(((p_md->NLQ_coeff_int[0][0]<<p_md->coefficient_log2_denom) | p_md->NLQ_coeff[0][0]) & 0x00FFFFFF),"el_coeff_y0");
    p_reg->EL_Coefficient_Y[1]        = printLog((uint32_t)(((p_md->NLQ_coeff_int[0][1]<<p_md->coefficient_log2_denom) | p_md->NLQ_coeff[0][1]) & 0x00FFFFFF),"el_coeff_y1");
    p_reg->EL_Coefficient_Y[2]        = printLog((uint32_t)(((p_md->NLQ_coeff_int[0][2]<<p_md->coefficient_log2_denom) | p_md->NLQ_coeff[0][2]) & 0x00FFFFFF),"el_coeff_y2");

    // U
    p_reg->Mapping_IDC_U              = printLog((uint32_t)p_md->mapping_idc[1] ,"mapping_idc_u");
    p_reg->BL_Num_Pivots_U = printLog((uint32_t)p_md->num_pivots[1], "bl_num_pivots_u");

    temp  = p_md->pivot_value[1][0];
    temp |= (p_md->pivot_value[1][1]<<10);
    p_reg->BL_Pivot_U[0]          = printLog(temp,"bl_pivot_u_reg1");
    temp  = p_md->pivot_value[1][2];
    temp |= (p_md->pivot_value[1][3]<<10);
    p_reg->BL_Pivot_U[1]          = printLog(temp,"bl_pivot_u_reg2");
    p_reg->BL_Pivot_U[2]          = printLog(p_md->pivot_value[1][4],"bl_pivot_u_reg3");

    temp  =  p_md->poly_order[1][0];
    temp |= (p_md->poly_order[1][1]<<2);
    temp |= (p_md->poly_order[1][2]<<4);
    temp |= (p_md->poly_order[1][3]<<6);
    p_reg->BL_Order_U               = printLog(temp,"bl_order_u");


    for(i=0 ; i < 4; i++)
    {
        for(j=0 ; j < 3; j++)
        {
            custom_sprintf(cStr,"bl_coeff_u%d%d",i,j);
            p_reg->BL_Coefficient_U[i][j]   = printLog((uint32_t)(((p_md->poly_coef_int[1][i][j]<<p_md->coefficient_log2_denom) | p_md->poly_coef[1][i][j]) & 0x3FFFFFFF),cStr);
        }
    }

    for(i=0 ; i < 22; i++)
    {
        uint64_t u64_temp;
        custom_sprintf(cStr,"mmr_coeff_u%d",i);
        u64_temp         = ((int64_t)p_md->MMR_coef_int[0][i]<<p_md->coefficient_log2_denom) | p_md->MMR_coef[0][i];

        p_reg->MMR_Coefficient_U[i][0] = printLog(u64_temp & 0xFFFFFFFF,cStr);
        p_reg->MMR_Coefficient_U[i][1] = printLog((u64_temp>>32) & 0xFF,cStr);
    }
    p_reg->MMR_Order_U                = printLog((uint32_t)p_md->MMR_order[0],"mmr_order_u");

    p_reg->EL_NLQ_Offset_U            = printLog((uint32_t)p_md->NLQ_offset[1],"el_nlq_offset_u");
    p_reg->EL_Coefficient_U[0]        = printLog((uint32_t)(((p_md->NLQ_coeff_int[1][0]<<p_md->coefficient_log2_denom) | p_md->NLQ_coeff[1][0]) & 0x00FFFFFF),"el_coeff_u0");
    p_reg->EL_Coefficient_U[1]        = printLog((uint32_t)(((p_md->NLQ_coeff_int[1][1]<<p_md->coefficient_log2_denom) | p_md->NLQ_coeff[1][1]) & 0x00FFFFFF),"el_coeff_u1");
    p_reg->EL_Coefficient_U[2]        = printLog((uint32_t)(((p_md->NLQ_coeff_int[1][2]<<p_md->coefficient_log2_denom) | p_md->NLQ_coeff[1][2]) & 0x00FFFFFF),"el_coeff_u2");

    // V
    p_reg->Mapping_IDC_V              = printLog((uint32_t)p_md->mapping_idc[2] ,"mapping_idc_v");
    p_reg->BL_Num_Pivots_V            = printLog((uint32_t)p_md->num_pivots[2]  ,"bl_num_pivots_v");

    temp  = p_md->pivot_value[2][0];
    temp |= (p_md->pivot_value[2][1]<<10);
    p_reg->BL_Pivot_V[0]          = printLog(temp,"bl_pivot_v_reg1");
    temp  = p_md->pivot_value[2][2];
    temp |= (p_md->pivot_value[2][3]<<10);
    p_reg->BL_Pivot_V[1]          = printLog(temp,"bl_pivot_v_reg2");
    p_reg->BL_Pivot_V[2]          = printLog(p_md->pivot_value[2][4],"bl_pivot_v_reg3");

    temp  =  p_md->poly_order[2][0];
    temp |= (p_md->poly_order[2][1]<<2);
    temp |= (p_md->poly_order[2][2]<<4);
    temp |= (p_md->poly_order[2][3]<<6);
    p_reg->BL_Order_V               = printLog(temp,"bl_order_v");


    for(i=0 ; i < 4; i++)
    {
        for(j=0 ; j < 3; j++)
        {
            custom_sprintf(cStr,"bl_coeff_v%d%d",i,j);
            p_reg->BL_Coefficient_V[i][j]   = printLog((uint32_t)(((p_md->poly_coef_int[2][i][j]<<p_md->coefficient_log2_denom) | p_md->poly_coef[2][i][j]) & 0x3FFFFFFF),cStr);
        }
    }

    for(i=0 ; i < 22; i++)
    {
        uint64_t u64_temp;
        custom_sprintf(cStr,"mmr_coeff_v%d",i);
        u64_temp = ((int64_t)p_md->MMR_coef_int[1][i]<<p_md->coefficient_log2_denom) | p_md->MMR_coef[1][i];

        p_reg->MMR_Coefficient_V[i][0] = printLog(u64_temp & 0xFFFFFFFF,cStr);
        p_reg->MMR_Coefficient_V[i][1] = printLog((u64_temp>>32) & 0xFF,cStr);
    }
    p_reg->MMR_Order_V                = printLog((uint32_t)p_md->MMR_order[1],"mmr_order_v");

    p_reg->EL_NLQ_Offset_V            = printLog((uint32_t)p_md->NLQ_offset[2],"el_nlq_offset_u");
    p_reg->EL_Coefficient_V[0]        = printLog((uint32_t)(((p_md->NLQ_coeff_int[2][0]<<p_md->coefficient_log2_denom) | p_md->NLQ_coeff[2][0]) & 0x00FFFFFF),"el_coeff_u0");
    p_reg->EL_Coefficient_V[1]        = printLog((uint32_t)(((p_md->NLQ_coeff_int[2][1]<<p_md->coefficient_log2_denom) | p_md->NLQ_coeff[2][1]) & 0x00FFFFFF),"el_coeff_u1");
    p_reg->EL_Coefficient_V[2]        = printLog((uint32_t)(((p_md->NLQ_coeff_int[2][2]<<p_md->coefficient_log2_denom) | p_md->NLQ_coeff[2][2]) & 0x00FFFFFF),"el_coeff_u2");

    frame_nr++;
    return 0;
}

static void dmKs2dmlut(DmKsFxp_t *pKs, dm_lut_t *p_dm_lut)
{
    int16_t i;
    uint64_t *lut_pack_lo, *lut_pack_hi;
    const unsigned short *lut = pKs->ksOMap.ksGmLut.lutMap;

    for (i = 0; i < 256; i++)
    {
        p_dm_lut->tcLut[i] = ((0xFFF & pKs->ksTMap.tmInternal515Lut[2*i+2]) << 12) | (0xFFF & pKs->ksTMap.tmInternal515Lut[2*i+1]);
    }
    for (i = 0; i < DEF_G2L_LUT_SIZE; i++)
        p_dm_lut->g2L[i] = pKs->ksIMap.g2L[i];
    /* 3 * 3 lut entries per 16 byte memory line, with 20 bit padding */
#if USE_12BITS_IN_3D_LUT == 0
    for (i = 0; i < (GMLUT_MAX_DIM*GMLUT_MAX_DIM*GMLUT_MAX_DIM) / 3; i++)
    {
        lut_pack_lo = (uint64_t*)&p_dm_lut->lut3D[16 * i];
        lut_pack_hi = (uint64_t*)&p_dm_lut->lut3D[16 * i + 8];
        *lut_pack_lo =
            (uint64_t)(0xFFF & (lut[9 * i    ] >>4)) |
            ((uint64_t)(0xFFF & (lut[9 * i + 1] >>4)) << 12) |
            ((uint64_t)(0xFFF & (lut[9 * i + 2] >>4)) << 24) |
            ((uint64_t)(0xFFF & (lut[9 * i + 3] >>4)) << 36) |
            ((uint64_t)(0xFFF & (lut[9 * i + 4] >>4)) << 48) |
            ((uint64_t)(0xFFF & (lut[9 * i + 5] >>4)) << 60);
        *lut_pack_hi =
            ((uint64_t)(0xFFF & (lut[9 * i + 5] >>4)) >>  4) |
            ((uint64_t)(0xFFF & (lut[9 * i + 6] >>4)) <<  8) |
            ((uint64_t)(0xFFF & (lut[9 * i + 7] >>4)) << 20) |
            ((uint64_t)(0xFFF & (lut[9 * i + 8] >>4)) << 32);
    }
    /* handle last incomplete line with only 2 * 3 entries */
    lut_pack_lo = (uint64_t*)&p_dm_lut->lut3D[16 * i];
    lut_pack_hi = (uint64_t*)&p_dm_lut->lut3D[16 * i + 8];
    *lut_pack_lo =
        (uint64_t)(0xFFF & (lut[9 * i    ] >>4)) |
        ((uint64_t)(0xFFF & (lut[9 * i + 1] >>4)) << 12) |
        ((uint64_t)(0xFFF & (lut[9 * i + 2] >>4)) << 24) |
        ((uint64_t)(0xFFF & (lut[9 * i + 3] >>4)) << 36) |
        ((uint64_t)(0xFFF & (lut[9 * i + 4] >>4)) << 48) |
        ((uint64_t)(0xFFF & (lut[9 * i + 5] >>4)) << 60);
    *lut_pack_hi =
        ((uint64_t)(0xFFF & (lut[9 * i + 5] >>4)) >> 4);
#else
    for (i = 0; i < (GMLUT_MAX_DIM*GMLUT_MAX_DIM*GMLUT_MAX_DIM) / 3; i++)
    {
        lut_pack_lo = (uint64_t*)&p_dm_lut->lut3D[16 * i];
        lut_pack_hi = (uint64_t*)&p_dm_lut->lut3D[16 * i + 8];
        *lut_pack_lo =
            (uint64_t)(0xFFF & lut[9 * i    ]) |
            ((uint64_t)(0xFFF & lut[9 * i + 1]) << 12) |
            ((uint64_t)(0xFFF & lut[9 * i + 2]) << 24) |
            ((uint64_t)(0xFFF & lut[9 * i + 3]) << 36) |
            ((uint64_t)(0xFFF & lut[9 * i + 4]) << 48) |
            ((uint64_t)(0xFFF & lut[9 * i + 5]) << 60);
        *lut_pack_hi =
            ((uint64_t)(0xFFF & lut[9 * i + 5]) >>  4) |
            ((uint64_t)(0xFFF & lut[9 * i + 6]) <<  8) |
            ((uint64_t)(0xFFF & lut[9 * i + 7]) << 20) |
            ((uint64_t)(0xFFF & lut[9 * i + 8]) << 32);
    }
    /* handle last incomplete line with only 2 * 3 entries */
    lut_pack_lo = (uint64_t*)&p_dm_lut->lut3D[16 * i];
    lut_pack_hi = (uint64_t*)&p_dm_lut->lut3D[16 * i + 8];
    *lut_pack_lo =
        (uint64_t)(0xFFF & lut[9 * i    ]) |
        ((uint64_t)(0xFFF & lut[9 * i + 1]) << 12) |
        ((uint64_t)(0xFFF & lut[9 * i + 2]) << 24) |
        ((uint64_t)(0xFFF & lut[9 * i + 3]) << 36) |
        ((uint64_t)(0xFFF & lut[9 * i + 4]) << 48) |
        ((uint64_t)(0xFFF & lut[9 * i + 5]) << 60);
    *lut_pack_hi =
        ((uint64_t)(0xFFF & lut[9 * i + 5]) >> 4);
#endif /* USE_12BITS_IN_3D_LUT */
}

#else
static void dmKs2dmreg(DmKsFxp_t *pKs, dm_register_t *p_dm_reg, dm_metadata_t *p_src_dm_metadata, cp_context_t* p_ctx)
{
    int16_t i, j;
    static int frame_nr = 0;

    if ((p_ctx->dbgExecParamsPrintPeriod != 0) && (frame_nr % p_ctx->dbgExecParamsPrintPeriod == 0))
    {
        //printf("\nProcessing Frame %d\n",frame_nr);
        printLog       =  &printLog_deb      ;
        custom_sprintf =  &custom_sprintf_deb;
    }
    else
    {
        printLog       =  &printLog_rel      ;
        custom_sprintf =  &custom_sprintf_rel;
    }

    p_dm_reg->colNum    = printLog(pKs->ksFrmFmtI.colNum, "colNum");
    p_dm_reg->rowNum    = printLog(pKs->ksFrmFmtI.rowNum, "rowNum");
    p_dm_reg->in_clr    = printLog(pKs->ksFrmFmtI.clr   , "in_clr");
    p_dm_reg->in_bdp    = printLog(pKs->ksFrmFmtI.bdp   , "in_bdp");
    p_dm_reg->in_chrm   = printLog(pKs->ksFrmFmtI.chrm   , "in_chrm");
    p_dm_reg->sRangeMin = printLog(pKs->ksIMap.eotfParam.rangeMin, "sRangeMin");
    p_dm_reg->sRange    = printLog(pKs->ksIMap.eotfParam.range   , "sRange   ");
    p_dm_reg->sRangeInv = printLog(pKs->ksIMap.eotfParam.rangeInv, "sRangeInv");
    p_dm_reg->sEotf     = printLog(pKs->ksIMap.eotfParam.eotf    , "sEotf    ");

    p_dm_reg->m33Yuv2RgbScale2P = printLog(pKs->ksIMap.m33Yuv2RgbScale2P, "m33Yuv2RgbScale2P");
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
        {
            custom_sprintf(cStr,"m33Yuv2Rgb_%d_%d",i,j);
            p_dm_reg->m33Yuv2Rgb[i][j] = printLog(pKs->ksIMap.m33Yuv2Rgb[i][j], cStr);
        }
    for (i = 0; i < 3; i++)
    {
        custom_sprintf(cStr,"v3Yuv2RgbOffInRgb_%d",i);
        p_dm_reg->v3Yuv2RgbOffInRgb[i] = printLog(pKs->ksIMap.v3Yuv2RgbOffInRgb[i], cStr);
    }

    p_dm_reg->m33Rgb2OptScale2P = printLog(pKs->ksIMap.m33Rgb2LmsScale2P, "m33Rgb2LmsScale2P");
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
        {
            custom_sprintf(cStr,"m33Rgb2Lms_%d_%d",i,j);
            p_dm_reg->m33Rgb2Opt[i][j] = printLog(pKs->ksIMap.m33Rgb2Lms[i][j], cStr);
        }

    p_dm_reg->m33Opt2OptScale2P = printLog(pKs->ksIMap.m33Lms2IptScale2P, "m33Rgb2LmsScale2P");
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
        {
            custom_sprintf(cStr,"m33Lms2Ipt_%d_%d",i,j);
            p_dm_reg->m33Opt2Opt[i][j] = printLog(pKs->ksIMap.m33Lms2Ipt[i][j], cStr);
        }
    p_dm_reg->Opt2OptOffset = printLog(0, "Opt2OptOffset");


    p_dm_reg->chromaWeight = printLog(pKs->ksTMap.chromaWeight, "chromaWeight");
    p_dm_reg->msWeight     = printLog(pKs->ksMs.msWeight      , "msWeight    ");
    p_dm_reg->msWeightEdge = printLog(pKs->ksMs.msEdgeWeight  , "msEdgeWeight");

    p_dm_reg->gain           = printLog(pKs->ksOMap.gain   , "gain   ");
    p_dm_reg->offset         = printLog(pKs->ksOMap.offset , "offset ");
    p_dm_reg->saturationGain = printLog(pKs->ksOMap.satGain, "satGain");

    p_dm_reg->tRangeMin     = printLog(pKs->ksOMap.tRangeMin    , "tRangeMin    ");
    p_dm_reg->tRange        = printLog(pKs->ksOMap.tRange       , "tRange       ");
    p_dm_reg->tRangeOverOne = printLog(pKs->ksOMap.tRangeOverOne, "tRangeOverOne");
    p_dm_reg->tRangeInv     = printLog(pKs->ksOMap.tRangeInv    , "tRangeInv    ");

    p_dm_reg->m33Rgb2YuvScale2P = printLog(pKs->ksOMap.m33Rgb2YuvScale2P, "m33Rgb2YuvScale2P");
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
        {
            custom_sprintf(cStr,"m33Rgb2Yuv_%d_%d",i,j);
            p_dm_reg->m33Rgb2Yuv[i][j] = printLog(pKs->ksOMap.m33Rgb2Yuv[i][j], cStr);
        }
    for (i = 0; i < 3; i++)
    {
        custom_sprintf(cStr,"v3Rgb2YuvOff_%d",i);
        p_dm_reg->v3Rgb2YuvOff[i]  = printLog(pKs->ksOMap.v3Rgb2YuvOff[i], cStr);
    }
    p_dm_reg->maxC[0] = printLog(pKs->ksOMap.ksGmLut.iMaxC1    , "maxC");
    p_dm_reg->maxC[1] = printLog(pKs->ksOMap.ksGmLut.iMaxC2    , "maxC");
    p_dm_reg->maxC[2] = printLog(pKs->ksOMap.ksGmLut.iMaxC3    , "maxC");
    p_dm_reg->minC[0] = printLog(pKs->ksOMap.ksGmLut.iMinC1    , "minC");
    p_dm_reg->minC[1] = printLog(pKs->ksOMap.ksGmLut.iMinC2    , "minC");
    p_dm_reg->minC[2] = printLog(pKs->ksOMap.ksGmLut.iMinC3    , "minC");
    p_dm_reg->CInv[0] = printLog(pKs->ksOMap.ksGmLut.iDistC1Inv, "CInv");
    p_dm_reg->CInv[1] = printLog(pKs->ksOMap.ksGmLut.iDistC2Inv, "CInv");
    p_dm_reg->CInv[2] = printLog(pKs->ksOMap.ksGmLut.iDistC3Inv, "CInv");
    p_dm_reg->out_clr = printLog(pKs->ksFrmFmtO.clr         , "out_clr");
    p_dm_reg->out_bdp = printLog(pKs->ksFrmFmtO.bdp         , "out_bdp");
    p_dm_reg->lut_type    = printLog(pKs->ksOMap.ksGmLut.valTp  , "lut_type   ");
    p_dm_reg->tcLutMaxVal = printLog(pKs->ksTMap.tmLutMaxVal    , "tcLutMaxVal");
# if CP_EN_AOI
    p_dm_reg->aoiRow0      = printLog(pKs->ksDmCtrl.aoiRow0     , "AOI_Row0     ");
    p_dm_reg->aoiRow1Plus1 = printLog(pKs->ksDmCtrl.aoiRow1Plus1, "AOI_Row1Plus1");
    p_dm_reg->aoiCol0      = printLog(pKs->ksDmCtrl.aoiCol0     , "AOI_Col0     ");
    p_dm_reg->aoiCol1Plus1 = printLog(pKs->ksDmCtrl.aoiCol1Plus1, "AOI_Col1Plus1");
#endif
    frame_nr++;
}

static uint16_t comp_cfg_2_comp_reg
(
    rpu_ext_config_fixpt_main_t *p_md,
    composer_register_t *p_reg,
    int disable_el, int xres, int yres, cp_context_t* p_ctx
)
{
    int i,j;
    static int frame_nr = 0;

    if ((p_ctx->dbgExecParamsPrintPeriod != 0) && (frame_nr % p_ctx->dbgExecParamsPrintPeriod == 0))
    {
        printLog       =  &printLog_deb      ;
        printLog64     =  &printLog64_deb    ;
        custom_sprintf =  &custom_sprintf_deb;
    }
    else
    {
        printLog       =  &printLog_rel      ;
        printLog64     =  &printLog64_rel    ;
        custom_sprintf =  &custom_sprintf_rel;
    }
    p_reg->rpu_VDR_bit_depth                 = printLog(p_md->rpu_VDR_bit_depth                  , "rpu_VDR_bit_depth       ");
    p_reg->rpu_BL_bit_depth                  = printLog(p_md->rpu_BL_bit_depth                   , "rpu_BL_bit_depth        ");
    p_reg->rpu_EL_bit_depth                  = printLog(p_md->rpu_EL_bit_depth                   , "rpu_EL_bit_depth        ");
    p_reg->coefficient_log2_denom            = printLog(p_md->coefficient_log2_denom             , "coefficient_log2_denom  ");
    p_reg->disable_EL_flag                   = printLog(p_md->disable_residual_flag || disable_el, "disable_EL_flag         ");
    p_reg->el_spatial_resampling_filter_flag = printLog(p_md->el_spatial_resampling_filter_flag  , "el_resampling           ");


    /* BL Mapping */

    /* Coefficients for Luma */
    p_reg->num_pivots_y = printLog(p_md->num_pivots[0], "num_pivots_y");
    for (i=0; i<8; i++)
    {
        custom_sprintf(cStr,"pivot_value_y_%d",i);
        p_reg->pivot_value_y[i]= printLog(p_md->pivot_value[0][i], cStr);
    }
    for (i=0; i<8; i++)
    {
        custom_sprintf(cStr,"order_y_%d",i);
        p_reg->order_y[i]      = printLog(p_md->poly_order[0][i] , cStr);
    }
    for (i=0; i<8; i++)
    {
        for(j=0 ; j < 3; j++)
        {
            custom_sprintf(cStr,"coeff_y_%d",i*3+j);
            p_reg->coeff_y[i*3 + j] = printLog(((p_md->poly_coef_int[0][i][j]<<p_md->coefficient_log2_denom) | p_md->poly_coef[0][i][j])  & 0x3FFFFFFF, cStr);
        }
    }
    p_reg->pivot_value_y[8]= printLog(p_md->pivot_value[0][8], "pivot_value_y_8");

    /* Coefficients for Chroma b */
    p_reg->num_pivots_cb = printLog(p_md->num_pivots[1], "num_pivots_cb");
    for (i=0; i<5; i++)
    {
        custom_sprintf(cStr,"pivot_value_cb_%d",i);
        p_reg->pivot_value_cb[i] = printLog(p_md->pivot_value[1][i], cStr);
    }
    p_reg->mapping_idc_cb        = printLog(p_md->mapping_idc[1], "mapping_idc_cb");
    if (p_reg->mapping_idc_cb == 0)
    {
        for (i=0; i<4; i++)
        {
            custom_sprintf(cStr,"order_cb_%d",i);
            p_reg->order_cb[i] = printLog(p_md->poly_order[1][i], cStr);
        }
    }
    else
    {
        p_reg->order_cb[0] = printLog(p_md->MMR_order[0], "order_cb");
        for (i=1; i<4; i++)
        {
            p_reg->order_cb[i] = 0;
        }
    }
    for (i=0; i<4; i++)
    {
        for(j=0 ; j < 3; j++)
        {
            custom_sprintf(cStr,"coeff_cb_%d", 3*i + j);
            p_reg->coeff_cb[i*3 + j] = printLog(((p_md->poly_coef_int[1][i][j]<<p_md->coefficient_log2_denom) | p_md->poly_coef[1][i][j]) & 0x3FFFFFFF, cStr);
        }
    }
    for (i=0; i<22; i++)
    {
        custom_sprintf(cStr,"coeff_mmr_cb_%d",i);
        p_reg->coeff_mmr_cb[i] = printLog64((((int64_t)(p_md->MMR_coef_int[0][i]<<p_md->coefficient_log2_denom)) | p_md->MMR_coef[0][i]) & 0xFFFFFFFFFFULL, cStr);
    }

    /* Coefficients for Chroma r */
    p_reg->num_pivots_cr = printLog(p_md->num_pivots[2], "num_pivots_cr");
    for (i=0; i<5; i++)
    {
        custom_sprintf(cStr,"pivot_value_cr_%d",i);
        p_reg->pivot_value_cr[i] = printLog(p_md->pivot_value[2][i], cStr);
    }
    p_reg->mapping_idc_cr = printLog(p_md->mapping_idc[2], "mapping_idc_cr");
    if (p_reg->mapping_idc_cr == 0)
    {
        for (i=0; i<4; i++)
        {
            custom_sprintf(cStr,"order_cr_%d",i);
            p_reg->order_cr[i] = printLog(p_md->poly_order[2][i], cStr);
        }
    }
    else
    {
        p_reg->order_cr[0] = printLog(p_md->MMR_order[1], "order_cr");
        for (i=1; i<4; i++)
        {
            p_reg->order_cr[i] = 0;
        }
    }
    for (i=0; i<4; i++)
    {
        for(j=0; j<3; j++)
        {
            custom_sprintf(cStr,"coeff_cr_%d", 3*i + j);
            p_reg->coeff_cr[i*3 + j] = printLog(((p_md->poly_coef_int[2][i][j]<<p_md->coefficient_log2_denom) | p_md->poly_coef[2][i][j]) & 0x3FFFFFFF, cStr);
        }
    }
    for (i=0; i<22; i++)
    {
        custom_sprintf(cStr,"coeff_mmr_cr_%d",i);
        p_reg->coeff_mmr_cr[i] = printLog64(((int64_t)(p_md->MMR_coef_int[1][i]<<p_md->coefficient_log2_denom) | p_md->MMR_coef[1][i]) & 0xFFFFFFFFFFULL, cStr);
    }


    /* EL NLdQ */
    if (!p_reg->disable_EL_flag)
    {
        p_reg->NLQ_offset_y = printLog(p_md->NLQ_offset[0], "NLQ_offset_y");
        for (i=0; i<3; i++)
        {
            custom_sprintf(cStr,"NLQ_coeff_y_%d",i);
            p_reg->NLQ_coeff_y[i] = printLog((p_md->NLQ_coeff_int[0][i]<<p_md->coefficient_log2_denom) | p_md->NLQ_coeff[0][i], cStr);
        }
        p_reg->NLQ_offset_cb  = printLog(p_md->NLQ_offset[1], "NLQ_offset_cb");
        for (i=0; i<3; i++)
        {
            custom_sprintf(cStr,"NLQ_coeff_cb_%d",i);
            p_reg->NLQ_coeff_cb[i] = printLog((p_md->NLQ_coeff_int[1][i]<<p_md->coefficient_log2_denom) | p_md->NLQ_coeff[1][i], cStr);
        }
        p_reg->NLQ_offset_cr = printLog(p_md->NLQ_offset[2], "NLQ_offset_cr");
        for (i=0; i<3; i++)
        {
            custom_sprintf(cStr,"NLQ_coeff_cr_%d",i);
            p_reg->NLQ_coeff_cr[i] = printLog((p_md->NLQ_coeff_int[2][i]<<p_md->coefficient_log2_denom) | p_md->NLQ_coeff[2][i], cStr);
        }
    }

    frame_nr++;
    return 0;
}

static void dmKs2dmlut(DmKsFxp_t *pKs, dm_lut_t *p_dm_lut)
{
    int16_t i;

# if REDUCED_TC_LUT
    for (i = 0; i < (512+3); i++)
        p_dm_lut->tcLut[i] = pKs->ksTMap.tmInternal515Lut[i];
//        p_dm_lut->tcLutMaxVal = pKs->ksTMap.tmLutMaxVal;
# else
    for (i = 0; i < 4096; i++)
        p_dm_lut->tcLut[i] = pKs->ksTMap.tmLutI[i];
# endif
    for (i = 0; i < DEF_G2L_LUT_SIZE; i++)
        p_dm_lut->g2L[i] = pKs->ksIMap.g2L[i];
    for (i = 0; i < 3*GMLUT_MAX_DIM*GMLUT_MAX_DIM*GMLUT_MAX_DIM; i++)
        p_dm_lut->lut3D[i] = pKs->ksOMap.ksGmLut.lutMap[i];
}
#endif

int get_dm_kernel_buf(h_cp_context_t h_ctx, char* buf)
{
    cp_context_t* p_ctx = h_ctx;
    int size;
    size = 0;//DumpKsBinBuf(p_ctx->h_ks, buf);

    return size;
}

int init_cp(h_cp_context_t h_ctx, run_mode_t *run_mode, char* lut_buf, char* dm_ctx_buf)
{

    cp_context_t* p_ctx = h_ctx;

    DmCfgFxp_t   *p_dm_cfg = &p_ctx->dm_cfg;


    p_ctx->last_view_mode_id = -1; // set last viewing mode to an invalid number so it will change on the first frame
    p_ctx->last_input_format = FORMAT_INVALID; // set last input format to an invalid number so it will change on the first frame


    p_ctx->h_ks      = (HDmKsFxp_t)(dm_ctx_buf                          );
    p_ctx->h_mds_ext = (HMdsExt_t )((char*)p_ctx->h_ks      + sizeof(DmKsFxp_t));
    p_ctx->h_dm      = (HDmFxp_t  )((char*)p_ctx->h_mds_ext + sizeof(MdsExt_t ));

    /* CTRL: create dm cfg/ctrl plan */
    memset(p_dm_cfg, 0, sizeof(DmCfgFxp_t));

    /* 1st round set up: internal default cfg value */
    InitDmCfg(CPlatformCpu, &p_ctx->mmg, p_dm_cfg);
    if (p_dm_cfg == NULL)
    {
        //printf("InitDmCfg() failed\n");
        return -1;
    }

    /* Initialize the kernel structure */
    if ((InitDmKs(p_dm_cfg, p_ctx->h_ks)) == NULL)
    {
        return -1;
    }

    if ((InitDm(p_dm_cfg, p_ctx->h_dm)) == NULL)
    {
        return -1;
    }

    InitMdsExt(p_dm_cfg, p_ctx->h_mds_ext);
    return 0;

}

static int setup_src_config(cp_context_t* p_ctx, signal_format_t  input_format, src_param_t *p_src_param)
{
    DmCfgFxp_t   *p_dm_cfg = &p_ctx->dm_cfg;
    p_dm_cfg->tgtSigEnv.RowNum = p_src_param->height;
    p_dm_cfg->tgtSigEnv.ColNum = p_src_param->width;

    /******************************/
    /* Source Config              */
    /******************************/
    p_dm_cfg->srcSigEnv.RowNum = p_src_param->height;
    p_dm_cfg->srcSigEnv.ColNum = p_src_param->width;
    p_dm_cfg->srcSigEnv.Dtp    = CDtpU16;
    p_dm_cfg->srcSigEnv.Clr    = CClrYuv;
    p_dm_cfg->srcSigEnv.CrossTalk   = p_ctx->cur_pq_config->target_display_config.crossTalk;

    if (input_format == FORMAT_DOVI)
    {
        /* DoVi source config will be updated by metadata */
        p_dm_cfg->srcSigEnv.Chrm   = CChrm420;
        p_dm_cfg->srcSigEnv.Weav   = CWeavPlnr;
    }
    else if (input_format == FORMAT_HDR10)
    {
        p_dm_cfg->srcSigEnv.Clr    = CClrYuv;
        p_dm_cfg->srcSigEnv.Chrm   = (p_src_param->src_chroma_format == 0) ? CChrm420 : CChrm422;
        p_dm_cfg->srcSigEnv.Weav   = (p_src_param->src_chroma_format == 0) ? CWeavPlnr : CWeavUyVy;
        p_dm_cfg->srcSigEnv.Eotf   = CEotfPq;
        p_dm_cfg->srcSigEnv.Rng    = (p_src_param->src_yuv_range == 0) ? CRngNarrow : CRngFull;
        p_dm_cfg->srcSigEnv.RgbDef      = CRgbDefR2020;
        p_dm_cfg->srcSigEnv.YuvXferSpec = CYuvXferSpecR2020;
        p_dm_cfg->srcSigEnv.Bdp    =  p_src_param->src_bit_depth;//Mstar p_ctx->run_mode.hdmi_mode ? p_src_param->src_bit_depth : 12;
        p_dm_cfg->srcSigEnv.Min    = (p_src_param->hdr10_param.min_display_mastering_luminance*(1<<18))/10000;
        p_dm_cfg->srcSigEnv.Max    = (uint32_t)(((uint64_t)p_src_param->hdr10_param.max_display_mastering_luminance*(1<<18))/10000);
        p_dm_cfg->srcSigEnv.MinPq  = LToPQ12(p_dm_cfg->srcSigEnv.Min);
        p_dm_cfg->srcSigEnv.MaxPq  = LToPQ12(p_dm_cfg->srcSigEnv.Max);
    }
    else if (input_format == FORMAT_SDR)
    {
        p_dm_cfg->srcSigEnv.Clr    = CClrYuv;
        p_dm_cfg->srcSigEnv.Chrm   = (p_src_param->src_chroma_format == 0) ? CChrm420 : CChrm422;
        p_dm_cfg->srcSigEnv.Weav   = (p_src_param->src_chroma_format == 0) ? CWeavPlnr : CWeavUyVy;
        p_dm_cfg->srcSigEnv.Eotf   = CEotfBt1886;
        p_dm_cfg->srcSigEnv.Bdp    = (p_src_param->input_mode == INPUT_MODE_HDMI) ? p_src_param->src_bit_depth : 12;
        p_dm_cfg->srcSigEnv.Rng    = (p_src_param->src_yuv_range == 0) ? CRngNarrow : CRngFull;
        p_dm_cfg->srcSigEnv.RgbDef      = CRgbDefR709;
        p_dm_cfg->srcSigEnv.YuvXferSpec = CYuvXferSpecR709;
        p_dm_cfg->srcSigEnv.Gamma  = SDR_DEFAULT_GAMMA;
        p_dm_cfg->srcSigEnv.Min    = SDR_DEFAULT_MIN_LUM;
        p_dm_cfg->srcSigEnv.Max    = SDR_DEFAULT_MAX_LUM;
        p_dm_cfg->srcSigEnv.MinPq  = LToPQ12(p_dm_cfg->srcSigEnv.Min);
        p_dm_cfg->srcSigEnv.MaxPq  = LToPQ12(p_dm_cfg->srcSigEnv.Max);
        p_dm_cfg->srcSigEnv.A      =     385;
        p_dm_cfg->srcSigEnv.B      =    1075;
        p_dm_cfg->srcSigEnv.G      = 1383604;
    }

# if CP_EN_AOI
    p_dm_cfg->dmCtrl.AoiRow0      = CLAMPS(p_dm_cfg->dmCtrl.AoiRow0,      0, p_dm_cfg->srcSigEnv.RowNum-1);
    p_dm_cfg->dmCtrl.AoiRow1Plus1 = CLAMPS(p_dm_cfg->dmCtrl.AoiRow1Plus1, 1, p_dm_cfg->srcSigEnv.RowNum  );
    p_dm_cfg->dmCtrl.AoiCol0      = CLAMPS(p_dm_cfg->dmCtrl.AoiCol0,      0, p_dm_cfg->srcSigEnv.ColNum-1);
    p_dm_cfg->dmCtrl.AoiCol1Plus1 = CLAMPS(p_dm_cfg->dmCtrl.AoiCol1Plus1, 1, p_dm_cfg->srcSigEnv.ColNum  );
#endif
    return 0;
}

int setup_src_config_external(h_cp_context_t  p_ctx, signal_format_t  input_format, src_param_t *p_src_param)
{
    DmCfgFxp_t   *p_dm_cfg = &p_ctx->dm_cfg;
    p_dm_cfg->tgtSigEnv.RowNum = p_src_param->height;
    p_dm_cfg->tgtSigEnv.ColNum = p_src_param->width;

    /******************************/
    /* Source Config              */
    /******************************/
    p_dm_cfg->srcSigEnv.RowNum = p_src_param->height;
    p_dm_cfg->srcSigEnv.ColNum = p_src_param->width;
    p_dm_cfg->srcSigEnv.Dtp    = CDtpU16;
    p_dm_cfg->srcSigEnv.Clr    = CClrYuv;
    p_dm_cfg->srcSigEnv.CrossTalk   = p_ctx->cur_pq_config->target_display_config.crossTalk;

    if (input_format == FORMAT_DOVI)
    {
        /* DoVi source config will be updated by metadata */
        p_dm_cfg->srcSigEnv.Chrm   = CChrm420;
        p_dm_cfg->srcSigEnv.Weav   = CWeavPlnr;
    }
    else if (input_format == FORMAT_HDR10)
    {
        p_dm_cfg->srcSigEnv.Clr    = CClrYuv;
        p_dm_cfg->srcSigEnv.Chrm   = (p_src_param->src_chroma_format == 0) ? CChrm420 : CChrm422;
        p_dm_cfg->srcSigEnv.Weav   = (p_src_param->src_chroma_format == 0) ? CWeavPlnr : CWeavUyVy;
        p_dm_cfg->srcSigEnv.Eotf   = CEotfPq;
        p_dm_cfg->srcSigEnv.Rng    = (p_src_param->src_yuv_range == 0) ? CRngNarrow : CRngFull;
        p_dm_cfg->srcSigEnv.RgbDef      = CRgbDefR2020;
        p_dm_cfg->srcSigEnv.YuvXferSpec = CYuvXferSpecR2020;
        p_dm_cfg->srcSigEnv.Bdp    =  p_src_param->src_bit_depth;//Mstar p_ctx->run_mode.hdmi_mode ? p_src_param->src_bit_depth : 12;
        p_dm_cfg->srcSigEnv.Min    = (p_src_param->hdr10_param.min_display_mastering_luminance*(1<<18))/10000;
        p_dm_cfg->srcSigEnv.Max    = (uint32_t)(((uint64_t)p_src_param->hdr10_param.max_display_mastering_luminance*(1<<18))/10000);
        p_dm_cfg->srcSigEnv.MinPq  = LToPQ12(p_dm_cfg->srcSigEnv.Min);
        p_dm_cfg->srcSigEnv.MaxPq  = LToPQ12(p_dm_cfg->srcSigEnv.Max);
    }
    else if (input_format == FORMAT_SDR)
    {
        p_dm_cfg->srcSigEnv.Clr    = CClrYuv;
        p_dm_cfg->srcSigEnv.Chrm   = (p_src_param->src_chroma_format == 0) ? CChrm420 : CChrm422;
        p_dm_cfg->srcSigEnv.Weav   = (p_src_param->src_chroma_format == 0) ? CWeavPlnr : CWeavUyVy;
        p_dm_cfg->srcSigEnv.Eotf   = CEotfBt1886;
        p_dm_cfg->srcSigEnv.Bdp    = (p_src_param->input_mode == INPUT_MODE_HDMI) ? p_src_param->src_bit_depth : 12;
        p_dm_cfg->srcSigEnv.Rng    = (p_src_param->src_yuv_range == 0) ? CRngNarrow : CRngFull;
        p_dm_cfg->srcSigEnv.RgbDef      = CRgbDefR709;
        p_dm_cfg->srcSigEnv.YuvXferSpec = CYuvXferSpecR709;
        p_dm_cfg->srcSigEnv.Gamma  = SDR_DEFAULT_GAMMA;
        p_dm_cfg->srcSigEnv.Min    = SDR_DEFAULT_MIN_LUM;
        p_dm_cfg->srcSigEnv.Max    = SDR_DEFAULT_MAX_LUM;
        p_dm_cfg->srcSigEnv.MinPq  = LToPQ12(p_dm_cfg->srcSigEnv.Min);
        p_dm_cfg->srcSigEnv.MaxPq  = LToPQ12(p_dm_cfg->srcSigEnv.Max);
        p_dm_cfg->srcSigEnv.A      =     385;
        p_dm_cfg->srcSigEnv.B      =    1075;
        p_dm_cfg->srcSigEnv.G      = 1383604;
    }

# if CP_EN_AOI
    p_dm_cfg->dmCtrl.AoiRow0      = CLAMPS(p_dm_cfg->dmCtrl.AoiRow0,      0, p_dm_cfg->srcSigEnv.RowNum-1);
    p_dm_cfg->dmCtrl.AoiRow1Plus1 = CLAMPS(p_dm_cfg->dmCtrl.AoiRow1Plus1, 1, p_dm_cfg->srcSigEnv.RowNum  );
    p_dm_cfg->dmCtrl.AoiCol0      = CLAMPS(p_dm_cfg->dmCtrl.AoiCol0,      0, p_dm_cfg->srcSigEnv.ColNum-1);
    p_dm_cfg->dmCtrl.AoiCol1Plus1 = CLAMPS(p_dm_cfg->dmCtrl.AoiCol1Plus1, 1, p_dm_cfg->srcSigEnv.ColNum  );
#endif
    return 0;
}

int commit_target_config(cp_context_t* p_ctx, pq_config_t* pq_config)
{

    int ret = 0;
    int i,j;
    TargetDisplayConfig_t*  td_config = &pq_config->target_display_config;
    DmCfgFxp_t   *p_dm_cfg = &p_ctx->dm_cfg;

    p_ctx->config_changed = 1;
    p_ctx->dbgExecParamsPrintPeriod = td_config->dbgExecParamsPrintPeriod;

    /******************************/
    /* Target Config              */
    /******************************/
    p_dm_cfg->tgtSigEnv.Bdp    = td_config->bitDepth;
    p_dm_cfg->tgtSigEnv.Eotf   = td_config->eotf;

    p_dm_cfg->tgtSigEnv.YuvXferSpec = CYuvXferSpecR709;
    p_dm_cfg->tgtSigEnv.Rng         = td_config->rangeSpec;
    p_dm_cfg->tgtSigEnv.MinPq       = td_config->minPq;
    p_dm_cfg->tgtSigEnv.MaxPq       = td_config->maxPq;
    p_dm_cfg->tgtSigEnv.Min         = td_config->min_lin;
    p_dm_cfg->tgtSigEnv.Max         = td_config->max_lin;
    p_dm_cfg->tgtSigEnv.Gamma       = td_config->gamma;
    p_dm_cfg->tgtSigEnv.DiagSize    = td_config->diagSize;
    p_dm_cfg->tgtSigEnv.CrossTalk   = td_config->crossTalk;
    //  CRgbDef_t RgbDef;   // color space def

    /* White point */
    p_dm_cfg->tgtSigEnv.WpExt = 1; /* 1 for given externally */
    for (i = 0; i < 3; i++)
        p_dm_cfg->tgtSigEnv.V3Wp[i] = td_config->ocscConfig.whitePoint[i];
    p_dm_cfg->tgtSigEnv.WpScale = td_config->ocscConfig.whitePointScale;

    /* RGB to LMS */
    p_dm_cfg->tgtSigEnv.Rgb2YuvExt = 1;  /* 1 for given externally */
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
        {
            p_dm_cfg->tgtSigEnv.M33Rgb2Yuv[i][j] = td_config->gdConfig.gdM33Rgb2Yuv[i][j];
        }
    p_dm_cfg->tgtSigEnv.M33Rgb2YuvScale2P = td_config->gdConfig.gdM33Rgb2YuvScale2P;
    p_dm_cfg->tgtSigEnv.Rgb2YuvOffExt = 1;  /* 1 for given externally */
    for (i = 0; i < 3; i++)
        p_dm_cfg->tgtSigEnv.V3Rgb2YuvOff[i] =  td_config->gdConfig.gdV3Rgb2YuvOff[i];


    /* LMS to RGB */
    p_dm_cfg->tgtSigEnv.Lms2RgbM33Ext = 1; /* 1 for given externally */
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
        {
            p_dm_cfg->tgtSigEnv.M33Lms2Rgb[i][j] = td_config->ocscConfig.lms2RgbMat[i][j];
        }
    p_dm_cfg->tgtSigEnv.M33Lms2RgbScale2P = td_config->ocscConfig.lms2RgbMatScale;

    /* initialize tone mapping control parameters */
    p_dm_cfg->tmCtrl.TMidBias    = td_config->midPQBias;
    p_dm_cfg->tmCtrl.TMaxBias    = td_config->maxPQBias;
    p_dm_cfg->tmCtrl.TMinBias    = td_config->minPQBias;
    p_dm_cfg->tmCtrl.DBrightness = td_config->brightness;
    p_dm_cfg->tmCtrl.DContrast   = td_config->contrast;
    p_dm_cfg->tmCtrl.ChrmVectorWeight        = td_config->chromaVectorWeight;
    p_dm_cfg->tmCtrl.IntensityVectorWeight   = td_config->intensityVectorWeight;
    p_dm_cfg->tmCtrl.KeyWeight               = td_config->keyWeight;

    /* default level2 */
    /* DM internal definition of these three values is different than cfg file, we need to adjust these values with the coding bias */
    p_dm_cfg->tmCtrl.Default2[TrimTypeChromaWeight]  = td_config->chromaWeight   + p_dm_cfg->tmCtrl.CodeBias2[TrimTypeChromaWeight];
    p_dm_cfg->tmCtrl.Default2[TrimTypeSatGain]       = td_config->saturationGain + p_dm_cfg->tmCtrl.CodeBias2[TrimTypeSatGain];
    p_dm_cfg->tmCtrl.Default2[TrimTypeMsWeight]      = td_config->mSWeight       + p_dm_cfg->tmCtrl.CodeBias2[TrimTypeMsWeight];

    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypeSlope]         = td_config->trimSlopeBias;
    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypeOffset]        = td_config->trimOffsetBias;
    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypePower]         = td_config->trimPowerBias;
    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypeChromaWeight]  = td_config->chromaWeightBias;
    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypeSatGain]       = td_config->saturationGainBias;
    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypeMsWeight]      = td_config->msWeightBias;


    if (!(td_config->tuningMode & TUNINGMODE_EXTLEVEL4_DISABLE))
    {
        p_dm_cfg->gdCtrl.GdOn            = td_config->gdConfig.gdEnable;
        p_dm_cfg->gdCtrl.GdWMin          = td_config->gdConfig.gdWMin;
        p_dm_cfg->gdCtrl.GdWMax          = td_config->gdConfig.gdWMax;
        p_dm_cfg->gdCtrl.GdWMm           = td_config->gdConfig.gdWMm;
        p_dm_cfg->gdCtrl.GdWMinPq        = LToPQ12(p_dm_cfg->gdCtrl.GdWMin);
        p_dm_cfg->gdCtrl.GdWMaxPq        = LToPQ12(p_dm_cfg->gdCtrl.GdWMax);
        p_dm_cfg->gdCtrl.GdWMmPq         = LToPQ12(p_dm_cfg->gdCtrl.GdWMm);
        p_dm_cfg->gdCtrl.GdWDynRngSqrt   = td_config->gdConfig.gdWDynRngSqrt;
        p_dm_cfg->gdCtrl.GdWeightMean    = td_config->gdConfig.gdWeightMean;
        p_dm_cfg->gdCtrl.GdWeightStd     = td_config->gdConfig.gdWeightStd ;
        p_dm_cfg->gdCtrl.GdUdPerFrmsTh = td_config->gdConfig.gdTriggerPeriod;
        p_dm_cfg->gdCtrl.GdUdDltTMaxTh = td_config->gdConfig.gdTriggerLinThresh;

    }
    else
    {
        p_dm_cfg->gdCtrl.GdOn = 0;
    }

    ret = CommitDmCfg(p_dm_cfg, p_ctx->h_ks, p_ctx->h_dm);

    return ret;

}

int commit_target_config_external(h_cp_context_t p_ctx, pq_config_t* pq_config)
{

    int ret = 0;
    int i,j;
    TargetDisplayConfig_t*  td_config = &pq_config->target_display_config;
    DmCfgFxp_t   *p_dm_cfg = &p_ctx->dm_cfg;

    p_ctx->config_changed = 1;
    p_ctx->dbgExecParamsPrintPeriod = td_config->dbgExecParamsPrintPeriod;

    /******************************/
    /* Target Config              */
    /******************************/
    p_dm_cfg->tgtSigEnv.Bdp    = td_config->bitDepth;
    p_dm_cfg->tgtSigEnv.Eotf   = td_config->eotf;

    p_dm_cfg->tgtSigEnv.YuvXferSpec = CYuvXferSpecR709;
    p_dm_cfg->tgtSigEnv.Rng         = td_config->rangeSpec;
    p_dm_cfg->tgtSigEnv.MinPq       = td_config->minPq;
    p_dm_cfg->tgtSigEnv.MaxPq       = td_config->maxPq;
    p_dm_cfg->tgtSigEnv.Min         = td_config->min_lin;
    p_dm_cfg->tgtSigEnv.Max         = td_config->max_lin;
    p_dm_cfg->tgtSigEnv.Gamma       = td_config->gamma;
    p_dm_cfg->tgtSigEnv.DiagSize    = td_config->diagSize;
    p_dm_cfg->tgtSigEnv.CrossTalk   = td_config->crossTalk;
    //  CRgbDef_t RgbDef;   // color space def

    /* White point */
    p_dm_cfg->tgtSigEnv.WpExt = 1; /* 1 for given externally */
    for (i = 0; i < 3; i++)
        p_dm_cfg->tgtSigEnv.V3Wp[i] = td_config->ocscConfig.whitePoint[i];
    p_dm_cfg->tgtSigEnv.WpScale = td_config->ocscConfig.whitePointScale;

    /* RGB to LMS */
    p_dm_cfg->tgtSigEnv.Rgb2YuvExt = 1;  /* 1 for given externally */
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
        {
            p_dm_cfg->tgtSigEnv.M33Rgb2Yuv[i][j] = td_config->gdConfig.gdM33Rgb2Yuv[i][j];
        }
    p_dm_cfg->tgtSigEnv.M33Rgb2YuvScale2P = td_config->gdConfig.gdM33Rgb2YuvScale2P;
    p_dm_cfg->tgtSigEnv.Rgb2YuvOffExt = 1;  /* 1 for given externally */
    for (i = 0; i < 3; i++)
        p_dm_cfg->tgtSigEnv.V3Rgb2YuvOff[i] =  td_config->gdConfig.gdV3Rgb2YuvOff[i];


    /* LMS to RGB */
    p_dm_cfg->tgtSigEnv.Lms2RgbM33Ext = 1; /* 1 for given externally */
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
        {
            p_dm_cfg->tgtSigEnv.M33Lms2Rgb[i][j] = td_config->ocscConfig.lms2RgbMat[i][j];
        }
    p_dm_cfg->tgtSigEnv.M33Lms2RgbScale2P = td_config->ocscConfig.lms2RgbMatScale;

    /* initialize tone mapping control parameters */
    p_dm_cfg->tmCtrl.TMidBias    = td_config->midPQBias;
    p_dm_cfg->tmCtrl.TMaxBias    = td_config->maxPQBias;
    p_dm_cfg->tmCtrl.TMinBias    = td_config->minPQBias;
    p_dm_cfg->tmCtrl.DBrightness = td_config->brightness;
    p_dm_cfg->tmCtrl.DContrast   = td_config->contrast;
    p_dm_cfg->tmCtrl.ChrmVectorWeight        = td_config->chromaVectorWeight;
    p_dm_cfg->tmCtrl.IntensityVectorWeight   = td_config->intensityVectorWeight;
    p_dm_cfg->tmCtrl.KeyWeight               = td_config->keyWeight;

    /* default level2 */
    /* DM internal definition of these three values is different than cfg file, we need to adjust these values with the coding bias */
    p_dm_cfg->tmCtrl.Default2[TrimTypeChromaWeight]  = td_config->chromaWeight   + p_dm_cfg->tmCtrl.CodeBias2[TrimTypeChromaWeight];
    p_dm_cfg->tmCtrl.Default2[TrimTypeSatGain]       = td_config->saturationGain + p_dm_cfg->tmCtrl.CodeBias2[TrimTypeSatGain];
    p_dm_cfg->tmCtrl.Default2[TrimTypeMsWeight]      = td_config->mSWeight       + p_dm_cfg->tmCtrl.CodeBias2[TrimTypeMsWeight];

    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypeSlope]         = td_config->trimSlopeBias;
    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypeOffset]        = td_config->trimOffsetBias;
    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypePower]         = td_config->trimPowerBias;
    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypeChromaWeight]  = td_config->chromaWeightBias;
    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypeSatGain]       = td_config->saturationGainBias;
    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypeMsWeight]      = td_config->msWeightBias;


    if (!(td_config->tuningMode & TUNINGMODE_EXTLEVEL4_DISABLE))
    {
        p_dm_cfg->gdCtrl.GdOn            = td_config->gdConfig.gdEnable;
        p_dm_cfg->gdCtrl.GdWMin          = td_config->gdConfig.gdWMin;
        p_dm_cfg->gdCtrl.GdWMax          = td_config->gdConfig.gdWMax;
        p_dm_cfg->gdCtrl.GdWMm           = td_config->gdConfig.gdWMm;
        p_dm_cfg->gdCtrl.GdWMinPq        = LToPQ12(p_dm_cfg->gdCtrl.GdWMin);
        p_dm_cfg->gdCtrl.GdWMaxPq        = LToPQ12(p_dm_cfg->gdCtrl.GdWMax);
        p_dm_cfg->gdCtrl.GdWMmPq         = LToPQ12(p_dm_cfg->gdCtrl.GdWMm);
        p_dm_cfg->gdCtrl.GdWDynRngSqrt   = td_config->gdConfig.gdWDynRngSqrt;
        p_dm_cfg->gdCtrl.GdWeightMean    = td_config->gdConfig.gdWeightMean;
        p_dm_cfg->gdCtrl.GdWeightStd     = td_config->gdConfig.gdWeightStd ;
        p_dm_cfg->gdCtrl.GdUdPerFrmsTh = td_config->gdConfig.gdTriggerPeriod;
        p_dm_cfg->gdCtrl.GdUdDltTMaxTh = td_config->gdConfig.gdTriggerLinThresh;

    }
    else
    {
        p_dm_cfg->gdCtrl.GdOn = 0;
    }

    ret = CommitDmCfg(p_dm_cfg, p_ctx->h_ks, p_ctx->h_dm);

    return ret;

}
#if 0
int commit_target_config_external_with3dLut(h_cp_context_t p_ctx, pq_config_t* pq_config)
{

    int ret = 0;
    int i,j;
    TargetDisplayConfig_t*  td_config = &pq_config->target_display_config;
    DmCfgFxp_t   *p_dm_cfg = &p_ctx->dm_cfg;

    p_ctx->config_changed = 1;
    p_ctx->dbgExecParamsPrintPeriod = td_config->dbgExecParamsPrintPeriod;

    /******************************/
    /* Target Config              */
    /******************************/
    p_dm_cfg->tgtSigEnv.Bdp    = td_config->bitDepth;
    p_dm_cfg->tgtSigEnv.Eotf   = td_config->eotf;

    p_dm_cfg->tgtSigEnv.YuvXferSpec = CYuvXferSpecR709;
    p_dm_cfg->tgtSigEnv.Rng         = td_config->rangeSpec;
    p_dm_cfg->tgtSigEnv.MinPq       = td_config->minPq;
    p_dm_cfg->tgtSigEnv.MaxPq       = td_config->maxPq;
    p_dm_cfg->tgtSigEnv.Min         = td_config->min_lin;
    p_dm_cfg->tgtSigEnv.Max         = td_config->max_lin;
    p_dm_cfg->tgtSigEnv.Gamma       = td_config->gamma;
    p_dm_cfg->tgtSigEnv.DiagSize    = td_config->diagSize;

    //  CRgbDef_t RgbDef;   // color space def

    /* White point */
    /* White point is already integrated in LMS2RGB coefficients in config file */
//    p_dm_cfg->tgtSigEnv.WpExt = 1; /* 1 for given externally */
//    for (i = 0; i < 3; i++)
//        p_dm_cfg->tgtSigEnv.V3Wp[i] = td_config->ocscConfig.whitePoint[i];
//    p_dm_cfg->tgtSigEnv.WpScale = td_config->ocscConfig.whitePointScale;

    /* RGB to LMS */
    p_dm_cfg->tgtSigEnv.Rgb2YuvExt = 1;  /* 1 for given externally */
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
        {
            p_dm_cfg->tgtSigEnv.M33Rgb2Yuv[i][j] = td_config->gdConfig.gdM33Rgb2Yuv[i][j];
        }
    p_dm_cfg->tgtSigEnv.M33Rgb2YuvScale2P = td_config->gdConfig.gdM33Rgb2YuvScale2P;
    p_dm_cfg->tgtSigEnv.Rgb2YuvOffExt = 1;  /* 1 for given externally */
    for (i = 0; i < 3; i++)
        p_dm_cfg->tgtSigEnv.V3Rgb2YuvOff[i] =  td_config->gdConfig.gdV3Rgb2YuvOff[i];


    /* LMS to RGB */
    p_dm_cfg->tgtSigEnv.Lms2RgbM33Ext = 1; /* 1 for given externally */
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
        {
            p_dm_cfg->tgtSigEnv.M33Lms2Rgb[i][j] = td_config->ocscConfig.lms2RgbMat[i][j];
        }
    p_dm_cfg->tgtSigEnv.M33Lms2RgbScale2P = td_config->ocscConfig.lms2RgbMatScale;

    /* initialize tone mapping control parameters */
    p_dm_cfg->tmCtrl.TMidBias    = td_config->midPQBias;
    p_dm_cfg->tmCtrl.TMaxBias    = td_config->maxPQBias;
    p_dm_cfg->tmCtrl.TMinBias    = td_config->minPQBias;
    p_dm_cfg->tmCtrl.DBrightness = td_config->brightness;
    p_dm_cfg->tmCtrl.DContrast   = td_config->contrast;
    p_dm_cfg->tmCtrl.ChrmVectorWeight        = td_config->chromaVectorWeight;
    p_dm_cfg->tmCtrl.IntensityVectorWeight   = td_config->intensityVectorWeight;
    p_dm_cfg->tmCtrl.KeyWeight               = td_config->keyWeight;

    /* default level2 */
    /* DM internal definition of these three values is different than cfg file, we need to adjust these values with the coding bias */
    p_dm_cfg->tmCtrl.Default2[TrimTypeChromaWeight]  = td_config->chromaWeight   + p_dm_cfg->tmCtrl.CodeBias2[TrimTypeChromaWeight];
    p_dm_cfg->tmCtrl.Default2[TrimTypeSatGain]       = td_config->saturationGain + p_dm_cfg->tmCtrl.CodeBias2[TrimTypeSatGain];
    p_dm_cfg->tmCtrl.Default2[TrimTypeMsWeight]      = td_config->mSWeight       + p_dm_cfg->tmCtrl.CodeBias2[TrimTypeMsWeight];

    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypeSlope]         = td_config->trimSlopeBias;
    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypeOffset]        = td_config->trimOffsetBias;
    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypePower]         = td_config->trimPowerBias;
    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypeChromaWeight]  = td_config->chromaWeightBias;
    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypeSatGain]       = td_config->saturationGainBias;
    p_dm_cfg->tmCtrl.ValueAdj2[TrimTypeMsWeight]      = td_config->msWeightBias;

    ParseGmLutHdr(pq_config->default_gm_lut, GM_LUT_HDR_SIZE, &p_ctx->gm_lut);
    ParseGmLutMap((const unsigned short *)(pq_config->default_gm_lut+GM_LUT_HDR_SIZE), p_ctx->gm_lut.LutMap, 1, &p_ctx->gm_lut);
    p_dm_cfg->hGmLut   = &p_ctx->gm_lut;


    if (!(td_config->tuningMode & TUNINGMODE_EXTLEVEL4_DISABLE))
    {
        GdSetGmLut(p_ctx->new_gm_lut_map, &p_ctx->new_gm_lut, p_dm_cfg, p_ctx->h_dm);
        p_dm_cfg->gdCtrl.GdOn            = td_config->gdConfig.gdEnable;
        p_dm_cfg->gdCtrl.GdWMin          = td_config->gdConfig.gdWMin;
        p_dm_cfg->gdCtrl.GdWMax          = td_config->gdConfig.gdWMax;
        p_dm_cfg->gdCtrl.GdWMm           = td_config->gdConfig.gdWMm;
        p_dm_cfg->gdCtrl.GdWMinPq        = LToPQ12(p_dm_cfg->gdCtrl.GdWMin);
        p_dm_cfg->gdCtrl.GdWMaxPq        = LToPQ12(p_dm_cfg->gdCtrl.GdWMax);
        p_dm_cfg->gdCtrl.GdWMmPq         = LToPQ12(p_dm_cfg->gdCtrl.GdWMm);
        p_dm_cfg->gdCtrl.GdWDynRngSqrt   = td_config->gdConfig.gdWDynRngSqrt;
        p_dm_cfg->gdCtrl.GdWeightMean    = td_config->gdConfig.gdWeightMean;
        p_dm_cfg->gdCtrl.GdWeightStd     = td_config->gdConfig.gdWeightStd ;

        ParseGmLutHdr(pq_config->gd_gm_lut_min, GM_LUT_HDR_SIZE, &p_ctx->gd_gm_lut_min);
        ParseGmLutMap((const unsigned short *)(pq_config->gd_gm_lut_min+GM_LUT_HDR_SIZE), p_ctx->gd_gm_lut_min.LutMap, 0, &p_ctx->gd_gm_lut_min);
        p_dm_cfg->hGmLutA  = &p_ctx->gd_gm_lut_min;

        ParseGmLutHdr(pq_config->gd_gm_lut_max, GM_LUT_HDR_SIZE, &p_ctx->gd_gm_lut_max);
        ParseGmLutMap((const unsigned short *)(pq_config->gd_gm_lut_max+GM_LUT_HDR_SIZE), p_ctx->gd_gm_lut_max.LutMap, 0, &p_ctx->gd_gm_lut_max);
        p_dm_cfg->hGmLutB  = &p_ctx->gd_gm_lut_max;

        ParsePq2GLut((const int32_t *)pq_config->pq2gamma, p_ctx->p_pq2g_lut);
        p_dm_cfg->hPq2GLut = p_ctx->p_pq2g_lut;

        /* copy the LUT header from the GD min lut to the new output LUT */
        p_ctx->new_gm_lut = p_ctx->gd_gm_lut_min;

        GdSetGmLut(p_ctx->new_gm_lut_map, &p_ctx->new_gm_lut, p_dm_cfg, p_ctx->h_dm);
    }
    else
    {
        p_dm_cfg->gdCtrl.GdOn = 0;
    }

    ret = CommitDmCfg(p_dm_cfg, p_ctx->h_ks, p_ctx->h_dm);

    return ret;

}
#endif
int destroy_cp(h_cp_context_t h_ctx)
{
    /* Nothing to do here currently. We keep it for future use. */
    return 0;
}


static int32_t dovi_handle_ui_menu(
    TargetDisplayConfig_t   *pTargetDisplayConfig,
    uint16_t                  u16BackLightUIVal,
    uint16_t                  u16BrightnessUIVal,
    uint16_t                  u16ContrastUIVal)
{


    int16_t x_x1,x1,y2,y1;
    int32_t temp;
    (void)u16BrightnessUIVal;
    (void)u16ContrastUIVal;

    u16BackLightUIVal = CLAMPS(u16BackLightUIVal,1,100);
    u16BackLightUIVal +=2;

    x1   = u16BackLightUIVal>>LOG2_UI_STEPSIZE;
    x_x1 = u16BackLightUIVal - (x1<<LOG2_UI_STEPSIZE);

    y1 = pTargetDisplayConfig->midPQBiasLut[x1];
    y2 = pTargetDisplayConfig->midPQBiasLut[x1+1];
    y2 = y2-y1 ; //Y2-Y1
    //X2-X1 = 8;
    temp = (y2 * x_x1) >>LOG2_UI_STEPSIZE   ;//(y2-y1)/(x2-x1) * (x-x1)
    pTargetDisplayConfig->midPQBias = (int16_t)(temp + y1);


#if 0
    y1 = pTargetDisplayConfig->chromaWeightBiasLut[x1];
    y2 = pTargetDisplayConfig->chromaWeightBiasLut[x1+1];
    y2 = y2-y1 ; //Y2-Y1
    temp = (y2 * x_x1) >>LOG2_UI_STEPSIZE   ;//(y2-y1)/(x2-x1) * (x-x1)
    pTargetDisplayConfig->chromaWeightBias =(int16_t)(temp + y1);

    y1 = pTargetDisplayConfig->saturationGainBiasLut[x1];
    y2 = pTargetDisplayConfig->saturationGainBiasLut[x1+1];
    y2 = y2-y1 ; //Y2-Y1
    temp = (y2 * x_x1) >>LOG2_UI_STEPSIZE   ;//(y2-y1)/(x2-x1) * (x-x1)
    pTargetDisplayConfig->saturationGainBias =(int16_t)(temp + y1);
#endif
    y1 = pTargetDisplayConfig->slopeBiasLut[x1];
    y2 = pTargetDisplayConfig->slopeBiasLut[x1+1];
    y2 = y2-y1 ; //Y2-Y1
    temp = (y2 * x_x1) >>LOG2_UI_STEPSIZE   ;//(y2-y1)/(x2-x1) * (x-x1)
    pTargetDisplayConfig->trimSlopeBias =(int16_t)(temp + y1);
#if 0
    y1 = pTargetDisplayConfig->offsetBiasLut[x1];
    y2 = pTargetDisplayConfig->offsetBiasLut[x1+1];
    y2 = y2-y1 ; //Y2-Y1
    temp = (y2 * x_x1) >>LOG2_UI_STEPSIZE   ;//(y2-y1)/(x2-x1) * (x-x1)
    pTargetDisplayConfig->trimOffsetBias =(int16_t)(temp + y1);
#endif

    // backlight scaler, default is 0 + 4096
    y1 = pTargetDisplayConfig->backlightBiasLut[x1];
    y2 = pTargetDisplayConfig->backlightBiasLut[x1+1];
    y2 = y2-y1 ; //Y2-Y1
    temp = (y2 * x_x1) >>LOG2_UI_STEPSIZE   ;//(y2-y1)/(x2-x1) * (x-x1)
    pTargetDisplayConfig->backlight_scaler =(int16_t)(temp + y1 + 4096);



    return 0;
}


void init_cp_mmg(cp_mmg_t * p_cp_mmg)
{
    Mmg_t dm_mmg;

    p_cp_mmg->cp_ctx_size = sizeof(cp_context_t);

    p_cp_mmg->lut_mem_size = 0;


    InitMmg(&dm_mmg);

    p_cp_mmg->dm_ctx_size = dm_mmg.dmCtxtSize + dm_mmg.mdsExtSize + dm_mmg.dmKsSize;
}

void (*cbFxHandle)(uint8_t backlight_pwm_val , uint32_t delay_in_millisec , void *p_rsrvd);

void register_dovi_callback(
    void (*call_back_hadler)(uint8_t backlight_pwm_val , uint32_t delay_in_millisec , void *p_rsrvd)
)
{
    cbFxHandle = call_back_hadler;
}

int commit_reg(h_cp_context_t h_ctx,
               signal_format_t  input_format,
               dm_metadata_t *p_src_dm_metadata,
               rpu_ext_config_fixpt_main_t *p_src_comp_metadata,
               pq_config_t* pq_config,
               int view_mode_id,
               src_param_t *p_src_param,
               ui_menu_params_t *ui_menu_params,
#if IPCORE
               register_ipcore_t *p_dst_reg,
#else
               composer_register_t *p_dst_comp_reg,
               dm_register_t *p_dst_dm_reg,
#endif
               dm_lut_t *p_dm_lut,
               uint16_t *backlight_return_val)
{
    cp_context_t* p_ctx = h_ctx;
    int cp_ret = 0;
    DmCfgFxp_t   *p_dm_cfg = &p_ctx->dm_cfg;
    TargetDisplayConfig_t*  td_config;
    int ui_params_changed = 0;
    int dm_ret = 0;
    uint32_t wMin, wMax, wMaxPQ;

    if (view_mode_id   != p_ctx->last_view_mode_id)
    {
        p_ctx->cur_pq_config = &pq_config[view_mode_id];
    }
    td_config = &p_ctx->cur_pq_config->target_display_config;

    if (input_format   != p_ctx->last_input_format)
    {
        setup_src_config(p_ctx, input_format, p_src_param);
        p_ctx->last_input_format = input_format;
    }
    if ((ui_menu_params->u16BackLightUIVal  != p_ctx->last_ui_menu_params.u16BackLightUIVal ) ||
        (ui_menu_params->u16BrightnessUIVal != p_ctx->last_ui_menu_params.u16BrightnessUIVal) ||
        (ui_menu_params->u16ContrastUIVal   != p_ctx->last_ui_menu_params.u16ContrastUIVal  ))
    {
        dovi_handle_ui_menu(
            td_config,
            ui_menu_params->u16BackLightUIVal,
            ui_menu_params->u16BrightnessUIVal,
            ui_menu_params->u16ContrastUIVal);
        p_ctx->last_ui_menu_params = *ui_menu_params;
        ui_params_changed = 1;
    }

    if ((view_mode_id   != p_ctx->last_view_mode_id) ||
        (ui_params_changed))
    {
        dm_ret |= commit_target_config(p_ctx, p_ctx->cur_pq_config);
        p_ctx->last_view_mode_id = view_mode_id;
    }

    /* set default maxPQ to target maxPQ, will be overwritten if GD is available */
    wMaxPQ = td_config->maxPq;

    /* Commit metadata if the input format is Dolby Vision */
    if (input_format == FORMAT_DOVI)
    {
        dm_metadata_2_dm_param(p_src_dm_metadata, p_ctx->h_mds_ext, p_dm_cfg, p_ctx);
        if (p_ctx->config_changed)
        {
            /* this is needed until libdm has it's own mechanism to force mds update */
            p_ctx->h_dm->mdsExt.m33Yuv2RgbScale2P = 0;
            p_ctx->config_changed = 0;
        }
        dm_ret |= CommitMds(p_ctx->h_mds_ext, p_ctx->h_dm);
        if (p_ctx->h_mds_ext->lvl4GdAvail)
        {
            GetGdActiveTgtWindow(&wMin, &wMax, p_ctx->h_dm);
            wMaxPQ = LToPQ12(wMax);
        }
    }

    *backlight_return_val = (p_ctx->cur_pq_config->backlight_lut[wMaxPQ] * td_config->backlight_scaler)>>12;
    *backlight_return_val = CLAMPS(*backlight_return_val, 10, 255);

    /* Only generate composer registers if we are not in HDMI mode */
    if (p_src_param->input_mode == INPUT_MODE_OTT)
    {
        if ((input_format == FORMAT_SDR) || (input_format == FORMAT_HDR10))
        {
            rpu_ext_config_fixpt_main_t src_comp_metadata = { 0 };
            fill_bypass_comp_cfg(&src_comp_metadata, p_src_param->src_bit_depth);
            comp_cfg_2_comp_reg(&src_comp_metadata,
#if IPCORE
                                p_dst_reg,
#else
                                p_dst_comp_reg,
#endif
                                td_config->tuningMode&TUNINGMODE_EL_FORCEDDISABLE, p_src_param->width, p_src_param->height, p_ctx);
        }
        else
        {
            comp_cfg_2_comp_reg(p_src_comp_metadata,
#if IPCORE
                                p_dst_reg,
#else
                                p_dst_comp_reg,
#endif
                                td_config->tuningMode&TUNINGMODE_EL_FORCEDDISABLE, p_src_param->width, p_src_param->height, p_ctx);
        }
    }

    /* Convert the DM kernel structure to DM registers.
       These functions may need to be updated by the customer */
    dmKs2dmreg(p_ctx->h_ks,
#if IPCORE
               p_dst_reg,
#else
               p_dst_dm_reg,
#endif
               p_src_dm_metadata, p_ctx);
    dmKs2dmlut(p_ctx->h_ks, p_dm_lut);


    /* Convert DM return value to 3 simpler return values */
    if (dm_ret < 0)
    {
        cp_ret = dm_ret;
    }
    else
    {
        if (p_src_param->input_mode == INPUT_MODE_OTT)  cp_ret |= CP_CHANGE_COMP_REG;
        if (dm_ret > 0)                  cp_ret |= CP_CHANGE_DM_REG;
        if (dm_ret & FLAG_CHANGE_GD)     cp_ret |= CP_CHANGE_GD ;
        if (dm_ret & FLAG_CHANGE_TC)     cp_ret |= CP_CHANGE_TC ;
    }

    cbFxHandle((uint8_t)*backlight_return_val ,
               p_src_param->input_mode == INPUT_MODE_HDMI ?
               p_ctx->cur_pq_config->target_display_config.gdConfig.gdDelayMilliSec_hdmi :
               p_ctx->cur_pq_config->target_display_config.gdConfig.gdDelayMilliSec_ott ,
               (void *)&p_ctx->cur_pq_config->target_display_config.max_lin);
    return cp_ret;
}
int read_md(dm_metadata_t *p_md, MS_U8* fp)
{
    int size, i;
    int ext_len;
    size = (int) fread(p_md, 1, sizeof(p_md->base), fp);
    if(size==0) return 0;
    for( i = 0; i < p_md->base.num_ext_blocks; i++ )
    {
        size += (int) fread(&p_md->ext[i], 1, 5, fp);
        ext_len =  (int) ((p_md->ext[i].ext_block_length_byte3<<24) +
                          (p_md->ext[i].ext_block_length_byte2<<16) +
                          (p_md->ext[i].ext_block_length_byte1<< 8) +
                          p_md->ext[i].ext_block_length_byte0);
        if ((ext_len > sizeof(p_md->ext[i].l)) || (ext_len < 0))
        {
            printk("Metadata error, num_ext_blocks: %d, ext[%d] len: %d\n", p_md->base.num_ext_blocks, i, ext_len);
        }
        else
        {
            size += (int) fread(&p_md->ext[i].l, 1, ext_len, fp);
        }
    }
    return size;
}
int commit_dm_reg(h_cp_context_t h_ctx,
                  dm_metadata_t *p_src_dm_metadata ,
                  signal_format_t  input_format,
                  pq_config_t* pq_config,
                  src_param_t *p_src_param,
                  ui_menu_params_t *ui_menu_params,
                  uint16_t *backlight_return_val,
                  int view_mode_id)
{
    uint32_t wMin, wMax, wMaxPQ;
    cp_context_t* p_ctx = h_ctx;
    DmCfgFxp_t   *p_dm_cfg = &p_ctx->dm_cfg;
    TargetDisplayConfig_t*  td_config;
    int ui_params_changed = 0;
    int dm_ret = 0;
    static bool bOnlyOnce = 0;

    if (view_mode_id   != p_ctx->last_view_mode_id )
    {
        p_ctx->cur_pq_config = &pq_config[view_mode_id];
    }

    //  if (input_format   != p_ctx->last_input_format) {  //what if we changed to a different HDR10 stream with differnt metadata, if we use it like this setup_src_config will not be called
    //      setup_src_config(p_ctx, input_format, p_src_param);
    //      printf("DOLBY INPUT FORMAT CHANGED\n");
    //p_ctx->last_input_format = input_format;
    //  }

    td_config = &p_ctx->cur_pq_config->target_display_config;

    if ((ui_menu_params->u16BackLightUIVal  != p_ctx->last_ui_menu_params.u16BackLightUIVal ) ||
        (ui_menu_params->u16BrightnessUIVal != p_ctx->last_ui_menu_params.u16BrightnessUIVal) ||
        (ui_menu_params->u16ContrastUIVal   != p_ctx->last_ui_menu_params.u16ContrastUIVal  ) ||
        (view_mode_id   != p_ctx->last_view_mode_id) )// added this line to initalize td_config->backlight_scaler when changing the mode
    {
        dovi_handle_ui_menu(
            td_config,
            ui_menu_params->u16BackLightUIVal,
            ui_menu_params->u16BrightnessUIVal,
            ui_menu_params->u16ContrastUIVal);
        p_ctx->last_ui_menu_params = *ui_menu_params;
        ui_params_changed = 1;
    }

    /* set default maxPQ to target maxPQ, will be overwritten if GD is available */
    wMaxPQ = td_config->maxPq;




    if ((view_mode_id   != p_ctx->last_view_mode_id) || (ui_params_changed) || (input_format   != p_ctx->last_input_format)  || input_format == FORMAT_HDR10)
    {
        dm_ret |= commit_target_config(p_ctx, p_ctx->cur_pq_config);
        p_ctx->last_view_mode_id = view_mode_id;
        p_ctx->last_input_format = input_format;
    }

    /* Commit metadata if the input format is Dolby Vision */
    if (input_format == FORMAT_DOVI)
    {
        dm_metadata_2_dm_param(p_src_dm_metadata, p_ctx->h_mds_ext, p_dm_cfg, p_ctx);
        if (p_ctx->config_changed)
        {
            /* this is needed until libdm has it's own mechanism to force mds update */
            p_ctx->h_dm->mdsExt.m33Yuv2RgbScale2P = 0;
            p_ctx->config_changed = 0;
        }
        dm_ret |= CommitMds(p_ctx->h_mds_ext, p_ctx->h_dm);
        if (p_ctx->h_mds_ext->lvl4GdAvail)
        {
            GetGdActiveTgtWindow(&wMin, &wMax, p_ctx->h_dm);
            wMaxPQ = LToPQ12(wMax);
        }
    }
    *backlight_return_val = (p_ctx->cur_pq_config->backlight_lut[wMaxPQ] * td_config->backlight_scaler)>>12;
    *backlight_return_val = CLAMPS(*backlight_return_val, 10, 255);

    return dm_ret;
}
int commit_comp_reg( h_cp_context_t h_ctx,
                     signal_format_t  input_format,
                     rpu_ext_config_fixpt_main_t *p_src_comp_metadata,
                     composer_register_t *p_dst_comp_reg,
                     src_param_t *p_src_param)
{

    cp_context_t* p_ctx = h_ctx;
    //TargetDisplayConfig_t*  td_config;

    //td_config = &p_ctx->cur_pq_config->target_display_config;
    /* Only generate composer registers if we are not in HDMI mode */
    if (p_src_param->input_mode == INPUT_MODE_OTT)
    {
        if ((input_format == FORMAT_SDR) || (input_format == FORMAT_HDR10))
        {
            rpu_ext_config_fixpt_main_t src_comp_metadata = { 0 };
            fill_bypass_comp_cfg(&src_comp_metadata, p_src_param->src_bit_depth);
            comp_cfg_2_comp_reg(&src_comp_metadata,
#if IPCORE
                                p_dst_reg,
#else
                                p_dst_comp_reg,
#endif
                                /*td_config->tuningMode*/0&TUNINGMODE_EL_FORCEDDISABLE, p_src_param->width, p_src_param->height, p_ctx);
        }
        else
        {
            comp_cfg_2_comp_reg(p_src_comp_metadata,
#if IPCORE
                                p_dst_reg,
#else
                                p_dst_comp_reg,
#endif
                                /*td_config->tuningMode*/0&TUNINGMODE_EL_FORCEDDISABLE, p_src_param->width, p_src_param->height, p_ctx);
        }
    }



}



