#ifndef _DOLBY_VISION_REG_H_
#define _DOLBY_VISION_REG_H_

#include "dolby_vision_driver.h"
#include "control_path_api.h"

//#define DEBUG_DOLBY_VISION_ENABLE

#ifdef DEBUG_DOLBY_VISION_ENABLE
#define DoVi_Printf(...)    printk(__VA_ARGS__)
#else
#define DoVi_Printf(...)
#endif

// =====================================================================================
// HDR Macros / Typedefs / Structures
// =====================================================================================

#define DoVi_Clamp(exp, min, max) (((exp) < (min)) ? (min) : ((exp) > (max)) ? (max) : (exp))

// Composer

typedef struct DoVi_Comp_ExtConfig_t_ // size = 1800 byte
{
    MS_U32  rpu_VDR_bit_depth;
    MS_U32  rpu_BL_bit_depth;
    MS_U32  rpu_EL_bit_depth;
    MS_U32  coefficient_log2_denom;
    MS_U32  num_pivots[3];
    MS_U32  pivot_value[3][MAX_PIVOT];
    MS_U32  mapping_idc[3]; // 0 : polynomial, 1 : MMR
    MS_U32  poly_order[3][MAX_PIVOT-1];
    MS_S32  poly_coef_int[3][MAX_PIVOT-1][MAX_POLY_ORDER+1];
    MS_U32  poly_coef[3][MAX_PIVOT-1][MAX_POLY_ORDER+1];
    MS_U32  MMR_order[2];
    MS_S32  MMR_coef_int[2][22];
    MS_U32  MMR_coef[2][22];
    MS_U8   NLQ_method_idc; // must be 0
    MS_U8   disable_residual_flag;
    MS_U8   el_spatial_resampling_filter_flag;
    MS_U8   reserved_8bit;
    MS_U32  NLQ_offset[3];
    MS_S32  NLQ_coeff_int[3][3];
    MS_U32  NLQ_coeff[3][3];
    MS_U32  spatial_resampling_filter_flag;
    MS_U32  spatial_resampling_explicit_filter_flag;
    MS_U32  spatial_filter_exp_coef_log2_denom;
    MS_U32  spatial_resampling_mode_hor_idc;
    MS_U32  spatial_resampling_mode_ver_idc;
    MS_U32  spatial_resampling_filter_hor_idc[3];
    MS_U32  spatial_resampling_filter_ver_idc[3];
    MS_U32  spatial_resampling_luma_pivot[2];
    MS_S32  spatial_filter_coeff_hor_int[3][8];
    MS_U32  spatial_filter_coeff_hor[3][8];
    MS_S32  spatial_filter_coeff_ver_int[2][3][6];
    MS_U32  spatial_filter_coeff_ver[2][3][6];
} DoVi_Comp_ExtConfig_t;

typedef struct MsHdr_Comp_Regtable_t_
{
    MS_BOOL bUpdate; // Flag indicating control by driver or not.
    MS_BOOL u8ModeBL; // 0:default, 1:BL output = input, 2:BL output = 0.
    MS_BOOL u8ModeEL; // 0:default, 1:EL output = input, 2:EL output = 0.
    rpu_ext_config_fixpt_main_t stConfig;
    composer_register_t stReg;
} MsHdr_Comp_Regtable_t;

// DM

#define DOVI_TRIM_NUM_MAX       17 // = max possible TrimNum + 1(default)
#define DOVI_TRIM_TYPE_MAX      8 // TrimTypeNum <= TRIM_TYPE_MAX
#define DOVI_TMO_SIZE           512
#define DOVI_3DLUT_SIZE         4944 // 0:736 1:656 2:656 3:576 4:656 5:576 6:576 7:512
#define DOVI_3DLUT_S0_SIZE      736
#define DOVI_3DLUT_S1_SIZE      656
#define DOVI_3DLUT_S2_SIZE      656
#define DOVI_3DLUT_S3_SIZE      576
#define DOVI_3DLUT_S4_SIZE      656
#define DOVI_3DLUT_S5_SIZE      576
#define DOVI_3DLUT_S6_SIZE      576
#define DOVI_3DLUT_S7_SIZE      512

#define DOVI_EOTFVAL_TO_EOTFENUM(VAL) (VAL == 0xFFFF ? EOTF_MODE_PQ : EOTF_MODE_BT1886)

typedef enum DoVi_DM_RunMode_t_
{
    DOVI_DM_RUN_MODE_NORMAL = 0,
    DOVI_DM_RUN_MODE_FROMHDMI = 2, // for cfg only
    DOVI_DM_RUN_MODE_NUM = 5
} DoVi_RunMode_t;

typedef struct DoVi_TmoInfo_t_
{
    MS_U16 u16Mode;
    MS_U16 u16Crush;
    MS_U16 u16Mid;
    MS_U16 u16Clip;
    MS_S16 s16Slope;
    MS_S16 s16Offset;
    MS_S16 s16Power;
    MS_U16 u16SminPq;
    MS_U16 u16SmaxPq;
    MS_U16 u16SdiagInches;
    MS_U16 u16TminPq;
    MS_U16 u16TmaxPq;
    MS_U16 u16TdiagInches;
    MS_S16 s16TminPqBias;
    MS_S16 s16TmidPqBias;
    MS_S16 s16TmaxPqBias;
    MS_S16 s16Contrast;
    MS_S16 s16Brightness;
    MS_S32 s32Rolloff;
} DoVi_TmoInfo_t;

// HDR Y2R (= Dolby B01-02)
typedef struct MsHdr_Y2R_RegTable_t_
{
    MS_BOOL bUpdate, bEnable;
    MS_U8 u8Shift, u8Round;
    MS_S16 s16Coef0;
    MS_S16 s16Coef1;
    MS_S16 s16Coef2;
    MS_S16 s16Coef3;
    MS_S16 s16Coef4;
    MS_S16 s16Coef5;
    MS_S16 s16Coef6;
    MS_S16 s16Coef7;
    MS_S16 s16Coef8;
    MS_S32 s32Offset0;
    MS_S32 s32Offset1;
    MS_S32 s32Offset2;
    MS_U16 u16Min;
    MS_U16 u16Max;
    MS_U32 u32Inv;
} MsHdr_Y2R_RegTable_t;

// HDR Degamma (= Dolby B01-03)
typedef struct MsHdr_Degamma_RegTable_t_
{
    MS_BOOL bUpdate, bEnable;
    MS_U32 u32Size;
    MS_U16 u16Sp;
    MS_U8 au8Eidb[16];
    MS_U16 au16Eaoff[16];
    MS_U32 u32EndDiff0;
    MS_U32 u32EndDiff1;
    MS_U32 u32EndDiff2;
    MS_U32 u32Limit0;
    MS_U32 u32Limit1;
    MS_U32 u32Limit2;
    //MS_U32 au32Table[DOVI_DEGAMMA_SIZE];
    MS_U32* pu32Table;
} MsHdr_Degamma_RegTable_t;

// HDR CSC (= Dolby B01-04)
typedef struct MsHdr_CSC_RegTable_t_
{
    MS_BOOL bUpdate, bEnable;
    MS_U8 u8Shift, u8Round;
    MS_S16 s16Coef0;
    MS_S16 s16Coef1;
    MS_S16 s16Coef2;
    MS_S16 s16Coef3;
    MS_S16 s16Coef4;
    MS_S16 s16Coef5;
    MS_S16 s16Coef6;
    MS_S16 s16Coef7;
    MS_S16 s16Coef8;
    MS_U16 u16Offset0;
    MS_U16 u16Offset1;
    MS_U16 u16Offset2;
    MS_U16 u16Min0;
    MS_U16 u16Min1;
    MS_U16 u16Min2;
    MS_U16 u16Max0;
    MS_U16 u16Max1;
    MS_U16 u16Max2;
} MsHdr_CSC_RegTable_t;

// HDR Gamma (= Dolby B01-05)
typedef struct MsHdr_Gamma_RegTable_t_
{
    MS_BOOL bUpdate, bEnable;
    MS_U32 u32Size;
    MS_U32 u32Sp;
    MS_U8 au8Eidb[32];
    MS_U16 au16Eaoff[32];
    MS_U16 u16EndDiff0;
    MS_U16 u16EndDiff1;
    MS_U16 u16EndDiff2;
    MS_U16 u16Limit0;
    MS_U16 u16Limit1;
    MS_U16 u16Limit2;
    //MS_U16 au16Table[DOVI_GAMMA_SIZE];
    MS_U16* pu16Table;
} MsHdr_Gamma_RegTable_t;

// HDR R2Y (= Dolby B01-06)
typedef struct MsHdr_R2Y_RegTable_t_
{
    MS_BOOL bUpdate, bEnable;
    MS_U8 u8Shift, u8Round;
    MS_S16 s16Coef0;
    MS_S16 s16Coef1;
    MS_S16 s16Coef2;
    MS_S16 s16Coef3;
    MS_S16 s16Coef4;
    MS_S16 s16Coef5;
    MS_S16 s16Coef6;
    MS_S16 s16Coef7;
    MS_S16 s16Coef8;
    MS_S16 s16Offset0;
    MS_S16 s16Offset1;
    MS_S16 s16Offset2;
    MS_S16 s16Min0;
    MS_S16 s16Min1;
    MS_S16 s16Min2;
    MS_S16 s16Max0;
    MS_S16 s16Max1;
    MS_S16 s16Max2;
} MsHdr_R2Y_RegTable_t;

// HDR Color Adjustment (= Dolby B01-07)
typedef struct MsHdr_CA_RegTable_t_
{
    MS_BOOL bUpdate, bEnable;
    MS_U16 u16ChromaWeight;
} MsHdr_CA_RegTable_t;

// HDR Tone Mapping (= Dolby B02)
typedef struct MsHdr_TM_RegTable_t_
{
    MS_BOOL bUpdate, bEnable;
    MS_U32 u32Size;
    MS_U8 u8InGain;
    MS_U8 u8OutGain;
    MS_U8 u8UserGain;
    MS_S16 s16Coef02; // fake_R = Y + coef02 * Cr
    MS_S16 s16Coef11; // fake_G = Y + coef11 * Cb
    MS_S16 s16Coef12; //            + coef12 * Cr
    MS_S16 s16Coef21; // fake_B = Y + coef21 * Cr
    MS_S16 s16Offset0;
    MS_S16 s16Offset1;
    MS_S16 s16Offset2;
    //MS_U16 au16Table[DOVI_TMO_SIZE];
    MS_U16* pu16Table;
} MsHdr_TM_RegTable_t;

// HDR Detail Restoration (= Dolby B03)
typedef struct MsHdr_DR_RegTable_t_
{
    MS_BOOL bUpdate, bEnable;
    MS_U16 u16MsWeightEdge;
    MS_S16 s16MsWeight;
} MsHdr_DR_RegTable_t;

// HDR Color Correction (= Dolby B04)
typedef struct MsHdr_CC_RegTable_t_
{
    MS_BOOL bUpdate, bEnable;
    MS_BOOL bClampEn;
    MS_U16 u16HuntGain;
    MS_U16 u16HuntOffset;
    MS_U16 u16SatGain;
    MS_U32 u16HighTh;
    MS_U32 u16LowTh;
    MS_U32 u16ZeroTh;
    MS_U16 u16CbOffset;
    MS_U16 u16CrOffset;
} MsHdr_CC_RegTable_t;

// HDR 3D LUT (= Dolby B05)
typedef struct MsHdr_3D_RegTable_t_
{
    MS_BOOL bUpdate, bEnable;
    MS_U32 u32Size;
    MS_S16 s16MinY;
    MS_S16 s16MaxY;
    MS_U32 u32InvY;
    MS_S16 s16MinC;
    MS_S16 s16MaxC;
    MS_U32 u32InvC;
    //MS_U16 au16Table[DOVI_3DLUT_SIZE*3];
    MS_U16* pu16Table;
} MsHdr_3D_RegTable_t;

// HDR DM Top
typedef struct MsHdr_RegTable_t_
{
    MsHdr_Y2R_RegTable_t stY2R;
    MsHdr_Degamma_RegTable_t stDegamma;
    MsHdr_CSC_RegTable_t stCSC;
    MsHdr_Gamma_RegTable_t stGamma;
    MsHdr_R2Y_RegTable_t stR2Y;
    MsHdr_CA_RegTable_t stCA;
    MsHdr_TM_RegTable_t stTM;
    MsHdr_DR_RegTable_t stDR;
    MsHdr_CC_RegTable_t stCC;
    MsHdr_3D_RegTable_t st3D;
} MsHdr_RegTable_t;

#endif
