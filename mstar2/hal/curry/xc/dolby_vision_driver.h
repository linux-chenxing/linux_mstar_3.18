#ifndef _DOLBY_VISION_H_
#define _DOLBY_VISION_H_

//#define DEBUG_DOLBY_VISION_ENABLE

#ifdef DEBUG_DOLBY_VISION_ENABLE
#define DoVi_Printf(...)    printk(__VA_ARGS__)
#else
#define DoVi_Printf(...)
#endif

// =====================================================================================
// HDR Macros / Typedefs / Structures
// =====================================================================================
#define DOLBY_DS_VERSION 2
#define DoVi_Clamp(exp, min, max) (((exp) < (min)) ? (min) : ((exp) > (max)) ? (max) : (exp))

// Composer

#define MAX_PIVOT       9
#define MAX_POLY_ORDER  2

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
    DoVi_Comp_ExtConfig_t stConfig;
} MsHdr_Comp_Regtable_t;

// DM

#define DOVI_TRIM_NUM_MAX   17 // = max possible TrimNum + 1(default)
#define DOVI_TRIM_TYPE_MAX  8 // TrimTypeNum <= TRIM_TYPE_MAX
#define DOVI_DEGAMMA_SIZE   512
#define DOVI_GAMMA_SIZE     512
#define DOVI_TMO_SIZE       512
#define DOVI_3DLUT_SIZE     4944 // 0:736 1:656 2:656 3:576 4:656 5:576 6:576 7:512
#define DOVI_3DLUT_S0_SIZE  736
#define DOVI_3DLUT_S1_SIZE  656
#define DOVI_3DLUT_S2_SIZE  656
#define DOVI_3DLUT_S3_SIZE  576
#define DOVI_3DLUT_S4_SIZE  656
#define DOVI_3DLUT_S5_SIZE  576
#define DOVI_3DLUT_S6_SIZE  576
#define DOVI_3DLUT_S7_SIZE  512

typedef enum DoVi_DM_RunMode_t_
{
    DOVI_DM_RUN_MODE_NORMAL = 0,
    DOVI_DM_RUN_MODE_FROMHDMI = 2, // for cfg only
    DOVI_DM_RUN_MODE_NUM = 5
} DoVi_RunMode_t;

typedef enum DoVi_EOTF_t_
{
    DOVI_DM_EOTF_MODE_BT1886 = 0,
    DOVI_DM_EOTF_MODE_PQ
    // EOTF_MODE_DCI: since this value is coupled with SigRange, omitted
} DoVi_EOTF_t;
#define DOVI_EOTFVAL_TO_EOTFENUM(VAL) (VAL == 0xFFFF ? DOVI_DM_EOTF_MODE_PQ : DOVI_DM_EOTF_MODE_BT1886)

typedef enum DoVi_SignalRange_t_
{
    SIG_RANGE_NARROW = 0, // head
    SIG_RANGE_FULL,  // will be in data type(bits) range
    SIG_RANGE_SDI // pq
} DoVi_SigRange_t;

typedef enum DoVi_TrimType_t_
{
    TrimTypeTMaxPq = 0, // a must be
    TrimTypeSlope,
    TrimTypeOffset,
    TrimTypePower,
    TrimTypeChromaWeight,
    TrimTypeSaturationGain,
    TrimTypeMsWeight,
    TrimTypeNum
} DoVi_TrimType_t; // must be sequential

typedef struct DoVi_Config_t_
{
    // Basic SPEC
    MS_U8 u8InputBits; // Input bit depth of DM
    MS_U8 u8OutputBits; // Output bit depth of DM
    MS_PHY phyLutBaseAddr; // DRAM base address for degamma, gamma, TMO, 3DLUT
    MS_U8 u8Lut3DIdx; // Select 3DLUT

    // Video SPEC : will be changed by metadatra
    MS_U16 u16SminPQ; // = 62;   // 12b
    MS_U16 u16SmaxPQ; // = 2081; // 100 -> 2081, 500 -> 2771, 600 -> 2851, 1000 -> 3079, 2000 -> 3388, 4000 -> 3696
    MS_U16 u16Sgamma; // = 39322; // Format 2.14
    MS_U8  u8SMode; // = 0; // 0 Rec709
    //MS_U8  u8SBitDepth; // = 16;
    MS_U8  u8SEOTF; // = 0; // // Bt1886 0, PQ 1
    MS_U8  u8SSignalRange; // = 1; // Narrow 0, Full 1, SDI 2
    MS_U16 u16Sdiagonalinches; // = 42;

    // Panel SPEC
    MS_S32 s32CrossTalk; // = 655 // =0.02  (0.15)
    MS_U16 u16TminPQ; // = 62;   // 12b
    MS_U16 u16TmaxPQ; // = 2081; // 100 -> 2081, 500 -> 2771, 600 -> 2851, 1000 -> 3079, 2000 -> 3388, 4000 -> 3696
    MS_U16 u16Tgamma; // = 39322; // Format 2.14
    MS_U8  u8TMode; // = 0; // 0 Rec709, 1 P3D65, 2 Rec2020, 3 ACES
    //MS_U8  u8TBitDepth; // = 16;
    MS_U8  u8TEOTF; // = 0; // // 0 Bt1886, 1 PQ
    MS_U8  u8TSignalRange; // = 1; // Narrow 0, Full 1, SDI 2
    MS_U16 u16Tdiagonalinches; // = 42;

    // Tone Mapping SPEC
    MS_U8  u8RunMode;
    MS_S16 s16TMinPQBias; // = 0
    MS_S16 s16TMidPQBias; // = 0
    MS_S16 s16TMaxPQBias; // = 0
    MS_S16 s16TrimSlopeBias; // = 0
    MS_S16 s16TrimOffsetBias; // = 0
    MS_S16 s16TrimPowerBias; // = 0
    MS_S16 s16TContrast; // = 0
    MS_S16 s16TBrightness; // = 0
    MS_S32 s32Rolloff; // = 1/3

    // Detail Restoration SPEC
    MS_U8  u8MsMethod; // = 4 // 0 : MS_METHOD_OFF 4 : MS_METHOD_DB_EDGE
    MS_S16 s16MsWeight; // = 2048
    MS_S16 s16MsWeightBias; // = 0
    MS_U16 u16MsWeightEdge; // = 16380

    // Adjustment SPEC
    MS_U16 u16CrossTalk; // = 655 // = 0.02 // Format 1.15
    MS_U16 u16ChromaWeight; // = 0 //
    MS_S16 s16ChromaWeightBias; // = 0
    MS_U16 u16CcGain; // = 2048 // = 0.5 // Format 0.12
    MS_U16 u16CcOffset; // = 4095 // = 1.0 // Format 0.15
    MS_U16 u16SaturationGain; // = 4095
    MS_S16 s16SaturationGainBias; // = 0

    // 3DLUT SPEC
    MS_S16 s16Lut3DMin[3];
    MS_S16 s16Lut3DMax[3];
} DoVi_Config_t;

// the meta data stream type
typedef struct DoVi_MdsExt_t_
{
    MS_U8 affected_dm_metadata_id;
    MS_U8 scene_refresh_flag;

    // yuv=>rgb
    //MS_U16 m33Yuv2RgbScale2P;     // NOT in metadata: fixed as 13

    MS_S16 YCCtoRGB_coef0;
    MS_S16 YCCtoRGB_coef1;
    MS_S16 YCCtoRGB_coef2;
    MS_S16 YCCtoRGB_coef3;
    MS_S16 YCCtoRGB_coef4;
    MS_S16 YCCtoRGB_coef5;
    MS_S16 YCCtoRGB_coef6;
    MS_S16 YCCtoRGB_coef7;
    MS_S16 YCCtoRGB_coef8;

    MS_S32 YCCtoRGB_offset0;
    MS_S32 YCCtoRGB_offset1;
    MS_S32 YCCtoRGB_offset2;

    // rgb=>lms
    //MS_U16 m33Rgb2WpLmsScale2P;   // NOT in metadata: fixed as 15

    MS_S16 RGBtoOpt_coef0;
    MS_S16 RGBtoOpt_coef1;
    MS_S16 RGBtoOpt_coef2;
    MS_S16 RGBtoOpt_coef3;
    MS_S16 RGBtoOpt_coef4;
    MS_S16 RGBtoOpt_coef5;
    MS_S16 RGBtoOpt_coef6;
    MS_S16 RGBtoOpt_coef7;
    MS_S16 RGBtoOpt_coef8;

    // EOTF gamma, a, b
    MS_U16 signal_eotf;         //1<<14
    MS_U16 signal_eotf_param0;  // 1<<2
    MS_U16 signal_eotf_param1;  // 1<<16
    MS_U32 signal_eotf_param2; // 4 bytes // 1<<18

    // signal info
    MS_U8 signal_bit_depth;
    MS_U8 signal_color_space;
    MS_U8 signal_chroma_format;
    MS_U8 signal_full_range_flag;

    // source monitor
    MS_U16 source_min_PQ;
    MS_U16 source_max_PQ;
    MS_U16 source_diagonal;

    // extension
    MS_U8 extLvl;
    MS_U8 num_ext_blocks;
    //// for level two
    MS_U16 TrimNum;         // TrimNum = 0 => no trim pass
    MS_U16 Trima[DOVI_TRIM_TYPE_MAX][DOVI_TRIM_NUM_MAX];// Trims[][0] is the default

    //// for level 1:
    // ------ this must be the last one and the order does not change to make sure CheckMds() return right status ----
    // the ccm
    MS_U16 min_PQ;
    MS_U16 max_PQ;
    MS_U16 mid_PQ;

    //after committrim
    MS_S32 trimSlope_final        ;
    MS_S32 trimOffset_final       ;
    MS_S32 trimPower_final        ;
    MS_U32 chromaWeight_final     ;
    MS_U32 saturationGain_final   ;
    MS_U32 msWeight_final         ;

} DoVi_MdsExt_t;

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
typedef struct MsHdr_3D_RegTable_t_dv
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
} MsHdr_3D_RegTable_t_dv;

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
    MsHdr_3D_RegTable_t_dv st3D;
} MsHdr_RegTable_t;

// =====================================================================================
// HDR Vision Instances and Functions
// =====================================================================================

// Composer

// Utility
MsHdr_Comp_Regtable_t* MsHdr_CompAllocRegTable(void);
void MsHdr_CompFreeRegTable(MsHdr_Comp_Regtable_t* pRegTable);
DoVi_Comp_ExtConfig_t* DoVi_CompAllocConfig(void);
void DoVi_CompFreeConfig(DoVi_Comp_ExtConfig_t* pConfig);
// Function for debug
void DoVi_CompDumpConfig(DoVi_Comp_ExtConfig_t* pConfExt);
// Function called after system boot
void DoVi_CompSetDefaultConfig(DoVi_Comp_ExtConfig_t* pConfExt);
// Function called after metadata ready
int DoVi_CompReadMetadata(DoVi_Comp_ExtConfig_t* pConfExt, const MS_U8* pu8MDS, MS_U16 u16Len);
// Function called after /DoVi_DmReadMetadata
void DoVi_CompFrameDeCalculate(MsHdr_Comp_Regtable_t* pRegTable, const DoVi_Comp_ExtConfig_t* pConfExt);
// Function called at blanking interrupt
void DoVi_CompBlankingUpdate(const MsHdr_Comp_Regtable_t* pRegTable);

// DM

// Utility
MsHdr_RegTable_t* MsHdr_DmAllocRegTable(void);
void MsHdr_DmFreeRegTable(MsHdr_RegTable_t* pRegTable);
DoVi_Config_t* DoVi_DmAllocConfig(void);
void DoVi_DmFreeConfig(DoVi_Config_t* pConfig);
// Function for debug
void DoVi_DmDumpMetadata(const DoVi_MdsExt_t* pMdsExt);
// Function called after system boot
void DoVi_DmSetDefaultConfig(DoVi_Config_t* pConfig);
// Function called after metadata ready
int DoVi_DmReadMetadata(DoVi_MdsExt_t* pMdsExt, const MS_U8* pu8MDS, MS_U16 u16Len, const DoVi_Config_t* pConfig);
// Function called after /DoVi_DmReadMetadata
void DoVi_DmFrameDeCalculate(MsHdr_RegTable_t* pRegTable, const DoVi_Config_t* pConfig, const DoVi_MdsExt_t* pMdsExt, DoVi_Comp_ExtConfig_t* pComConfig);
// Function called at blanking interrupt
void DoVi_DmBlankingUpdate(const MsHdr_RegTable_t* pRegTable);

// Call this to update 3DLUT
int DoVi_Replace_3DLut(MsHdr_3D_RegTable_t_dv* p3DLutReg, const MS_U8* pu8Array, MS_U32 u32Size);
int DoVi_Trigger3DLutUpdate(void);

// External tone mapping function call
//extern void DoVi_CreateToneCurve(const DoVi_TmoInfo_t* pTmoInfo, MS_U16 *toneCurve512LUT);
void DoVi_Prepare_DS(MS_U8 u8Version);
void DoVi_GetCmdCnt(K_XC_DS_CMDCNT *pstXCDSCmdCnt);
#endif
