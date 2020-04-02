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
* @mainpage Dolby Vision Control Execution Library
*
* The purpose of this library is to generate the hardware register values for the Composer
* and Display Management hardware.
*
* @brief Control Path API.
* @file control_path_api.h
*
*/

#ifndef CONTROL_PATH_API_H_
#define CONTROL_PATH_API_H_

//#include <stdlib.h>
//#include <stdio.h>
//#include <assert.h>
#include "dolbyTypeToMstarType.h"
#include <target_display_config.h>
#include <rpu_ext_config.h>
#ifdef __cplusplus
extern "C" {
#endif

#define IPCORE 0
#define CP_EN_AOI 0



#define DEF_G2L_LUT_SIZE_2P        8
#define DEF_G2L_LUT_SIZE           (1<<DEF_G2L_LUT_SIZE_2P)
#define GMLUT_MAX_DIM        17

/*! @defgroup general Enumerations and data structures
 *
 * @{
 */
/*! @brief Signal format
*/
typedef enum signal_format_e
{
    FORMAT_INVALID = -1,
    FORMAT_DOVI = 0,
    FORMAT_HDR10 = 1,
    FORMAT_SDR = 2
} signal_format_t;

/*! @brief Input mode
*/
typedef enum input_mode_e
{
    INPUT_MODE_OTT = 0,
    INPUT_MODE_HDMI= 1
} input_mode_t;

/*! @brief Signal EOTF mode
*/
typedef enum cp_eotf_t
{
    EOTF_MODE_BT1886 = 0,
    EOTF_MODE_PQ = 1
} cp_eotf_t;

/*! @brief Signal range spec
*/
typedef enum cp_signal_range_t
{
    SIG_RANGE_SMPTE = 0,                              /* head range */
    SIG_RANGE_FULL  = 1,                              /* full range */
    SIG_RANGE_SDI   = 2                                       /* PQ */
} cp_signal_range_t;


/** @brief Run mode */
typedef struct run_mode_s
{
    uint16_t width;
    uint16_t height;
    uint16_t el_width;
    uint16_t el_height;
    uint16_t hdmi_mode;
} run_mode_t;


/** @brief UI menu parameters */
typedef struct ui_menu_params_s
{
    uint16_t u16BackLightUIVal;
    uint16_t u16BrightnessUIVal;
    uint16_t u16ContrastUIVal;
} ui_menu_params_t;

#if IPCORE
/** @brief Composer and DM registers for IPCORE */
typedef struct register_ipcore_s
{
    uint32_t SRange;                        /**< @brief Address 0x18 */
    uint32_t Srange_Inverse;                /**< @brief Address 0x1C */
    uint32_t Frame_Format_1;                /**< @brief Address 0x20 */
    uint32_t Frame_Format_2;                /**< @brief Address 0x24 */
    uint32_t Frame_Pixel_Def;               /**< @brief Address 0x28 */
    uint32_t Y2RGB_Coefficient_1;           /**< @brief Address 0x2C */
    uint32_t Y2RGB_Coefficient_2;           /**< @brief Address 0x30 */
    uint32_t Y2RGB_Coefficient_3;           /**< @brief Address 0x34 */
    uint32_t Y2RGB_Coefficient_4;           /**< @brief Address 0x38 */
    uint32_t Y2RGB_Coefficient_5;           /**< @brief Address 0x3C */
    uint32_t Y2RGB_Offset_1;                /**< @brief Address 0x40 */
    uint32_t Y2RGB_Offset_2;                /**< @brief Address 0x44 */
    uint32_t Y2RGB_Offset_3;                /**< @brief Address 0x48 */
    uint32_t EOTF;                          /**< @brief Address 0x4C */
    uint32_t Sparam_1;                      /**< @brief Address 0x50 */
    uint32_t Sparam_2;                      /**< @brief Address 0x54 */
    uint32_t Sgamma;                        /**< @brief Address 0x58 */
    uint32_t A2B_Coefficient_1;             /**< @brief Address 0x5C */
    uint32_t A2B_Coefficient_2;             /**< @brief Address 0x60 */
    uint32_t A2B_Coefficient_3;             /**< @brief Address 0x64 */
    uint32_t A2B_Coefficient_4;             /**< @brief Address 0x68 */
    uint32_t A2B_Coefficient_5;             /**< @brief Address 0x6C */
    uint32_t C2D_Coefficient_1;             /**< @brief Address 0x70 */
    uint32_t C2D_Coefficient_2;             /**< @brief Address 0x74 */
    uint32_t C2D_Coefficient_3;             /**< @brief Address 0x78 */
    uint32_t C2D_Coefficient_4;             /**< @brief Address 0x7C */
    uint32_t C2D_Coefficient_5;             /**< @brief Address 0x80 */
    uint32_t C2D_Offset;                    /**< @brief Address 0x84 */
    uint32_t Chroma_Weight;                 /**< @brief Address 0x88 */
    uint32_t mFilter_Scale;                 /**< @brief Address 0x8C */
    uint32_t msWeight;                      /**< @brief Address 0x90 */
    uint32_t Hunt_Value;                    /**< @brief Address 0x94 */
    uint32_t Saturation_Gain;               /**< @brief Address 0x98 */
    uint32_t Min_C_1;                       /**< @brief Address 0x9C */
    uint32_t Min_C_2;                       /**< @brief Address 0xA0 */
    uint32_t Max_C;                         /**< @brief Address 0xA4 */
    uint32_t C1_Inverse;                    /**< @brief Address 0xA8 */
    uint32_t C2_Inverse;                    /**< @brief Address 0xAC */
    uint32_t C3_Inverse;                    /**< @brief Address 0xB0 */
    uint32_t pixDef;                        /**< @brief Address 0xB4 */
    uint32_t Active_area_left_top;          /**< @brief Address 0xB8 */
    uint32_t Active_area_bottom_right;      /**< @brief Address 0xBC */
    uint32_t reserved_dm[2];                /**< @brief Address 0xC0-0xC4 */
    uint32_t Composer_Mode;                 /**< @brief Address 0xC8 */
    uint32_t VDR_Resolution;                /**< @brief Address 0xCC */
    uint32_t Bit_Depth;                     /**< @brief Address 0xD0 */
    uint32_t Coefficient_Log2_Denominator;  /**< @brief Address 0xD4 */
    uint32_t BL_Num_Pivots_Y;               /**< @brief Address 0xD8 */
    uint32_t BL_Pivot[5];                   /**< @brief Address 0xDC-0xEC */
    uint32_t BL_Order;                      /**< @brief Address 0xF0 */
    uint32_t BL_Coefficient_Y[8][3];        /**< @brief Address 0xF4-0x150 */
    uint32_t EL_NLQ_Offset_Y;               /**< @brief Address 0x154 */
    uint32_t EL_Coefficient_Y[3];           /**< @brief Address 0x158-0x160 */
    uint32_t Mapping_IDC_U;                 /**< @brief Address 0x164 */
    uint32_t BL_Num_Pivots_U;               /**< @brief Address 0x168 */
    uint32_t BL_Pivot_U[3];                 /**< @brief Address 0x16C-0x174 */
    uint32_t BL_Order_U;                    /**< @brief Address 0x178 */
    uint32_t BL_Coefficient_U[4][3];        /**< @brief Address 0x17C-0x1A8 */
    uint32_t MMR_Coefficient_U[22][2];      /**< @brief Address 0x1AC-0x258 */
    uint32_t MMR_Order_U;                   /**< @brief Address 0x25C */
    uint32_t EL_NLQ_Offset_U;               /**< @brief Address 0x260 */
    uint32_t EL_Coefficient_U[3];           /**< @brief Address 0x264-0x26C */
    uint32_t Mapping_IDC_V;                 /**< @brief Address 0x270 */
    uint32_t BL_Num_Pivots_V;               /**< @brief Address 0x274 */
    uint32_t BL_Pivot_V[3];                 /**< @brief Address 0x278-0x280 */
    uint32_t BL_Order_V;                    /**< @brief Address 0x284 */
    uint32_t BL_Coefficient_V[4][3];        /**< @brief Address 0x288-0x2B4 */
    uint32_t MMR_Coefficient_V[22][2];      /**< @brief Address 0x2B8-0x364 */
    uint32_t MMR_Order_V;                   /**< @brief Address 0x368 */
    uint32_t EL_NLQ_Offset_V;               /**< @brief Address 0x36C */
    uint32_t EL_Coefficient_V[3];           /**< @brief Address 0x370-0x378 */
    uint32_t reserved_comp[6];              /**< @brief Address 0x380-0x398 */
}  register_ipcore_t;

/** @brief DM luts for IPCORE */
typedef struct dm_lut_s
{
    uint8_t lut3D[((GMLUT_MAX_DIM*GMLUT_MAX_DIM*GMLUT_MAX_DIM)/3+1)*16];
    uint32_t tcLut[64*4];
    uint32_t g2L[DEF_G2L_LUT_SIZE];
} dm_lut_t;
#else
#pragma pack(push, 1)
/** @brief Composer registers for a general composer */
typedef struct composer_register_s
{
    uint32_t  rpu_VDR_bit_depth;
    uint32_t  rpu_BL_bit_depth;
    uint32_t  rpu_EL_bit_depth;
    uint32_t  coefficient_log2_denom;
    uint32_t  disable_EL_flag;
    uint32_t  el_spatial_resampling_filter_flag;
    uint32_t  num_pivots_y;
    uint32_t  pivot_value_y[9];
    uint32_t  order_y[8];
    uint32_t  coeff_y[24];
    uint32_t  NLQ_offset_y;
    uint32_t  NLQ_coeff_y[3];
    uint32_t  num_pivots_cb;
    uint32_t  pivot_value_cb[5];
    uint32_t  mapping_idc_cb;
    uint32_t  order_cb[4];
    uint32_t  coeff_cb[12];
    uint64_t  coeff_mmr_cb[22];
    uint32_t  NLQ_offset_cb;
    uint32_t  NLQ_coeff_cb[3];
    uint32_t  num_pivots_cr;
    uint32_t  pivot_value_cr[5];
    uint32_t  mapping_idc_cr;
    uint32_t  order_cr[4];
    uint32_t  coeff_cr[12];
    uint64_t  coeff_mmr_cr[22];
    uint32_t  NLQ_offset_cr;
    uint32_t  NLQ_coeff_cr[3];
} composer_register_t;
#pragma pack(pop)

/** @brief DM registers for a general DM implementation */
typedef struct dm_register_s
{

    uint32_t colNum;
    uint32_t rowNum;
    uint32_t in_clr;
    uint32_t in_bdp;
    uint32_t in_chrm;

    // source range
    uint32_t sRangeMin, sRange;
    uint32_t sRangeInv;

    //// forward
    // yuv2rgb xfer
    uint32_t m33Yuv2RgbScale2P;
    uint32_t m33Yuv2Rgb[3][3];      /**< @brief Y2RGB Coefficient */
    uint32_t v3Yuv2RgbOffInRgb[3];  /**< @brief Y2RGB Offset */

    // gamma stuff
    uint32_t sEotf;                 /**< @brief 0: gamma, 1: PQ */
    // rgb=>lms or other
    uint32_t m33Rgb2OptScale2P;     /**< @brief A2B */
    uint32_t m33Rgb2Opt[3][3];      /**< @brief A2B */
    // lms or other to ipt or other
    uint32_t m33Opt2OptScale2P;     /**< @brief C2D */
    uint32_t m33Opt2Opt[3][3];      /**< @brief C2D */
    uint32_t Opt2OptOffset;         /**< @brief C2D */
    // Helmoltz-Kohlrasush scaling
    uint32_t chromaWeight;          /**< @brief 64K - 1 scale */

    //// blending
    uint32_t msWeight, msWeightEdge;

    // gain/offset stuff
    uint32_t gain, offset;          /**< @brief Hunt value  4K - 1 scale */
    // Saturation control
    uint32_t saturationGain;

    uint32_t tRangeMin, tRange;
    uint32_t tRangeOverOne;
    uint32_t tRangeInv;

    uint32_t minC[3];
    uint32_t maxC[3];
    uint32_t CInv[3];

    // to support output xyz/rgb=>yuv conversion.
    uint32_t m33Rgb2YuvScale2P;
    uint32_t m33Rgb2Yuv[3][3];
    uint32_t v3Rgb2YuvOff[3];

    uint32_t out_clr;
    uint32_t out_bdp;

    uint32_t lut_type;
    uint32_t tcLutMaxVal;

    #if CP_EN_AOI
    /* AOI */
    uint32_t aoiRow0     ;
    uint32_t aoiRow1Plus1;
    uint32_t aoiCol0     ;
    uint32_t aoiCol1Plus1;
    #endif
} dm_register_t;

#pragma pack(push)
/** @brief DM luts for a general DM implementation */
typedef struct dm_lut_s
{
    uint16_t lut3D[3*GMLUT_MAX_DIM*GMLUT_MAX_DIM*GMLUT_MAX_DIM];
    uint32_t g2L[DEF_G2L_LUT_SIZE];
    uint16_t tcLut[512+3];
} dm_lut_t;
#pragma pack(pop)
#endif

/** @brief Memory management data stucture */
typedef struct cp_mmg_s {
    unsigned cp_ctx_size;    /**< @brief Size of control path context */
    unsigned lut_mem_size;   /**< @brief Size of all LUTs */
    unsigned dm_ctx_size;    /**< @brief Size of DM context */
} cp_mmg_t;

/*! @brief Opaque library context.
*/
typedef struct cp_context_s *h_cp_context_t;

/*! @brief HDR10 parameters data structure.
This is the data structure used for the HDR10 SEI messages.
Details for each entry can be found in HEVC specification
*/
typedef struct hdr10_param_s
{
    uint32_t min_display_mastering_luminance; /**< @brief min_display_mastering_luminance in units of 0.0001 nits */
    uint32_t max_display_mastering_luminance; /**< @brief max_display_mastering_luminance in units of 0.0001 nits */
    uint16_t Rx;                              /**< @brief */
    uint16_t Ry;                              /**< @brief */
    uint16_t Gx;                              /**< @brief */
    uint16_t Gy;                              /**< @brief */
    uint16_t Bx;                              /**< @brief */
    uint16_t By;                              /**< @brief */
    uint16_t Wx;                              /**< @brief */
    uint16_t Wy;                              /**< @brief */
    uint16_t max_content_light_level;         /**< @brief max_content_light_level in units of 1 nit */
    uint16_t max_pic_average_light_level;     /**< @brief max_pic_average_light_level in units of 1 nit */
} hdr10_param_t;


/*! @brief Source parameters.
*/
typedef struct src_param_s
{
    int src_bit_depth;                 /**< @brief Input bit depth*/
    int src_chroma_format;             /**< @brief Input chroma format*/
    cp_signal_range_t src_yuv_range;   /**< @brief Input yuv range */
    input_mode_t input_mode;           /**< @brief Input mode */
    int width;                         /**< @brief Image width */
    int height;                        /**< @brief Image height */
    hdr10_param_t hdr10_param;         /**< @brief HDR10 source parameters */
} src_param_t;

/*!
 * @}
 */

/*! @defgroup functions API functions
 *
 * @{
 */

/*! @brief Initializes the control path memory manager.
 *         This function needs to be called once to initialize the memory manager.
 *  @param[in]  *p_ctx            Control path context.
 */
void init_cp_mmg(cp_mmg_t * p_cp_mmg);

/*! @brief Initializes the control path context.
 *         This function needs to be called once to set up the control path context.
 *  @param[in]  *p_ctx            Control path context.
 *  @param[in]  *run_mode         Run mode.
 *  @param[in]  *lut_buf          Buffer for all DM Luts
 *  @param[in]  *dm_ctx_buf       Buffer for DM context
 *  @return
 *      @li 0       success
 *      @li <0      error
 */
int init_cp(h_cp_context_t p_ctx, run_mode_t * run_mode, char* lut_buf, char* dm_ctx_buf);



/* return flags for commit_reg */
#define CP_CHANGE_COMP_REG     0x000001      /**< Some composer register values changed */
#define CP_CHANGE_DM_REG       0x000002      /**< Some dm register values changed */
#define CP_CHANGE_GD           0x000004      /**< 3D Lut changed               */
#define CP_CHANGE_TC           0x000008      /**< Tone Curve LUT changed       */


/*! @brief Generates the Composer and DM registers
 *  @param[in]  *p_ctx                Control path context.
 *  @param[in]  input_format          Input format (DOVI, HDR10 or SDR)
 *  @param[in]  *p_pq_config          Target Display configuration (includes all viewing modes)
 *  @param[in]  view_mode_id          Viewing mode ID
 *  @param[in]  *p_src_dm_metadata    Source DM Metadata
                                      Only needed for Dolby Vision input.
 *  @param[in]  *p_src_comp_metadata  Source Composer Metadata
                                      Only needed for Dolby Vision input.
 *  @param[in]  *p_src_param          In case of HDR10 or SDR this structure needs to be set up
                                      instead of the Composer and DM metadata.
                                      This parameter shall be NULL in case of DOVI input.
 *  @param[in]  *ui_menu_params       Backlight, Brightness and contrast values
 *  @param[out] *p_dst_comp_reg       Generic or IPCORE Register Mapping for Composer.
 *  @param[out] *p_dm_reg             Generic or IPCORE Register Mapping for DM.
 *  @param[out] *p_dm_lut             Generic LUT structure for DM.
 *  @param[out] *backlight_return_val Backlight output value of Global Dimming.
 *  @return
 *      @li  0 success without any change from last frame.
 *      @li >0 success with change from last frame. See CP_CHANGE_* define statements above for details
 *      @li <0 error
 */
int commit_reg(h_cp_context_t p_ctx,
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
               dm_register_t *p_dm_reg,
               #endif
               dm_lut_t *p_dm_lut,
               uint16_t *backlight_return_val);

/*! @brief Register the callback function for dolby vision
 *  @param[in]  *call_back_handler                Call Back Function pointer that implements delay logic to apply backlight value.
 *  @return
 *      @li 0       success
 *      @li <0      error
 */

void register_dovi_callback(
                            void (*call_back_handler)(uint8_t backlight_pwm_val , uint32_t delay_in_millisec , void *p_rsrvd)
                            );



/*! @brief Destroys the control path context.
 *  @param[in]  *p_ctx     Control path context.
 *  @return
 *      @li 0       success
 *      @li <0      error
 */
int destroy_cp(h_cp_context_t p_ctx);

/* For internal use only. Not needed on SOC */
int get_dm_kernel_buf(h_cp_context_t h_ctx, char* buf);
int commit_dm_reg(h_cp_context_t h_ctx,
                dm_metadata_t *p_src_dm_metadata ,
                signal_format_t  input_format,
                 pq_config_t* pq_config,
                   src_param_t *p_src_param,
                   ui_menu_params_t *ui_menu_params,
                 uint16_t *backlight_return_val,
                  int view_mode_id);
int read_md(dm_metadata_t *p_md, MS_U8* fp);
int commit_comp_reg( h_cp_context_t h_ctx,
	         signal_format_t  input_format,
                rpu_ext_config_fixpt_main_t *p_src_comp_metadata,
                composer_register_t *p_dst_comp_reg,
                src_param_t *p_src_param);
//int commit_target_config(cp_context_t* p_ctx, pq_config_t* pq_config);
int commit_target_config_external(h_cp_context_t p_ctx, pq_config_t* pq_config) ;
//int commit_target_config_external_with3dLut(h_cp_context_t p_ctx, pq_config_t* pq_config) ;
//int commit_target_config_hdr10(h_cp_context_t p_ctx, pq_config_t* pq_config);
 int setup_src_config_external(h_cp_context_t  p_ctx, signal_format_t  input_format, src_param_t *p_src_param);
/*!
 * @}
 */

#ifdef __cplusplus
}
#endif


#endif //CONTROL_PATH_API_H_
