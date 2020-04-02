/****************************************************************************
* This product contains one or more programs protected under international
* and U.S. copyright laws as unpublished works.  They are confidential and
* proprietary to Dolby Laboratories.  Their reproduction or disclosure, in
* whole or in part, or the production of derivative works therefrom without
* the express permission of Dolby Laboratories is prohibited.
*
*             Copyright 2011 - 2015 by Dolby Laboratories.
*                     All rights reserved.
****************************************************************************/
#ifndef C_DM_TYPE_PRI_FXP_H
#define C_DM_TYPE_PRI_FXP_H

#include "KCdmModCtrl.h"
#include "VdrDmApi.h"
#include "CdmTypeFxp.h"
#include "KdmTypeFxp.h"


/*! @brief DM scene info structure

    Meta data for frame(s)

*/

/* trim def */
#define TRIM_NUM_MAX    16      // = max possible TrimNum for all kinds of trims
#define TRIM_NUM_DIM    (TRIM_NUM_MAX + 1)

typedef struct Trim_t_ {
  // preset value
  int16_t TrimLevel;  // 2 or 3
  int16_t TrimTypeDim, TrimNumMax;  // trim matrix def: TRIM_TYPE_DIM2/3, TRIM_NUM_MAX
  // actual value ine mds
  int16_t TrimNum;    // trim number
  uint16_t *Trima; // layout as [TrimNumMax+1][TrimTypeDim]
  // Trima[0][trimType] is the default, Trima[1...TrimNum][trimType] the trims
  // Trima[0][0] is sMaxPq, Trima[1...TrimNumMax][0] the tMaxPq in trims
} Trim_t;

typedef struct TrimSet_t_ {
  int16_t     TrimSetNum;   // TrimNum = 0 => no trim pass
  Trim_t  TrimSets[1];  // only 1 trim sets are supported [0] level2
  uint16_t TrimaMem[TRIM_NUM_DIM * (TRIM_TYPE_DIM2 + TRIM_TYPE_DIM3)];
} TrimSet_t;

////// the fxp bin syntax: big endian
#define signal_eotf_VALUE_PQ  ((uint16_t)0xFFFF)

typedef struct MdsExt_t_ {
  uint8_t invalidateMds;  // to invalidate the cached the version and let commitMds do
  uint8_t affected_dm_metadata_id;
  uint8_t scene_refresh_flag;

  // yuv=>rgb
  int16_t m33Yuv2RgbScale2P;  // NOT in metadata: fixed as 13
  int16_t m33Yuv2Rgb[3][3];
  uint32_t v3Yuv2Rgb[3];

  // rgb=>lms
  int16_t m33Rgb2WpLmsScale2P;  // NOT in metadata: fixed as 15
  int16_t m33Rgb2WpLms[3][3];

  // EOTF gamma, a, b
  uint16_t signal_eotf;     //1<<14
  uint16_t signal_eotf_param0;  // 1<<2
  uint16_t signal_eotf_param1;  // 1<<16
  uint32_t signal_eotf_param2; // 4 bytes // 1<<18

  // signal info
  uint8_t signal_bit_depth;
  uint8_t signal_color_space;
  uint8_t signal_chroma_format;
  uint8_t signal_full_range_flag;

  // source monitor
  uint16_t source_min_PQ;
  uint16_t source_max_PQ;
  uint16_t source_diagonal;

  // extension
  uint8_t num_ext_blocks;
  ////// that under extension
  // for level 2, 3
  TrimSet_t trimSets; // NOT all in metadata: derived from level flag = 2, 3

# if EN_GLOBAL_DIMMING
  // level4 global dimming
  int16_t lvl4GdAvail;                           // NOT in metadata, if in this mds
  uint16_t filtered_mean_PQ;
  uint16_t filtered_power_PQ;
# endif

  int16_t lvl5AoiAvail;
# if EN_AOI
  uint16_t active_area_left_offset, active_area_right_offset;
  uint16_t active_area_top_offset,  active_area_bottom_offset;
# endif

  int16_t lvl255RunModeAvail;
  uint8_t dm_run_mode;
  uint8_t dm_run_version;
  uint8_t dm_debug0;
  uint8_t dm_debug1;
  uint8_t dm_debug2;
  uint8_t dm_debug3;

  //// for level 1:
  // ------ this must be the last one and the order does not change to make sure CheckMds() return right status ----
  // the ccm
  uint16_t min_PQ;
  uint16_t max_PQ;
  uint16_t mid_PQ;
} MdsExt_t;

// if   & 0x01 != 0 => ipt output, signed
// else & 0x02 != 0 => rgb out
typedef enum GmLutType_t_ {
  GmLutTypeIpt2Ipt = 1, // 01
  GmLutTypeRgb2Ipt = 3, // 11
  GmLutTypeIpt2Yuv = 0, // 00
  GmLutTypeIpt2Rgb = 2  // 10
} GmLutType_t;

# if DM_VER_LOWER_THAN212
// 3d Lut
#define GM_LUT_VERSION_CHARS  4
#define GM_LUT_ENDIAN_CHARS   1
#define GM_LUT_TYPE_CHARS     8

typedef struct GmLut_t_ {
  ////// lut hdr( chars+1 for '\0' )
  uint32_t Version;     // from 4 CC
  int32_t LittleEndian; // from 1 CC, b: big or l: little endian
  GmLutType_t Type;     // from 8 CC, ???2???_

  int16_t DimC1, DimC2, DimC3; // dim for C1, C2, C3
  int16_t IdxMinC1, IdxMaxC1, IdxMinC2, IdxMaxC2, IdxMinC3, IdxMaxC3;  // the min, max idx

  ////// lut data
  uint16_t *LutMap; // the LUT, type depends on version and type
} GmLut_t;

typedef struct Pq2GLut_t_ {
# define EOTF_LUT_SIZE       1024
# define EOTF_LUT_ALPHA_SIZE 16
  uint16_t eotfLutMaxPq[4];
  int32_t eotfLuts[4][EOTF_LUT_SIZE];
  int32_t eotfLutAlpha[3][EOTF_LUT_ALPHA_SIZE];
} Pq2GLut_t;

#endif

//// tone curve mapping related only
typedef enum CTcLutDir_t_ {
  CTcLutDirFwd = 0,
  CTcLutDirBwd,
  CTcLutDirPass,
  CTcLutDirNum
} CTcLutDir_t;

typedef struct TcSigFxp_t_
{
  //// for MDS, may chang per frame
  uint16_t minPq, maxPq;  // norm in PQ
  uint16_t diagSize;

  //// CCM: that is changed per frame per mds and tcLutDir
  uint16_t crush, clip, mid;// norm in PQ
} TcSigFxp_t;

typedef struct TcCtrlFxp_t_
{
  TcSigFxp_t tcSrcSig;
  TcSigFxp_t tcTgtSig;

  //// control params
  CTcLutDir_t tcLutDir;

  int32_t bwdInvertible; // when fwd, use what in mds if available, when bwd, use the inverse of fwd
  int32_t rolloff, rolloffInv;

# if DM_VER_HIGHER_THAN211
  int32_t keyWeight;
  int32_t intensityVectorWeight;
  int32_t chrmVectorWeight;
  int32_t bpWeight;
# endif
  int32_t ratio;        //  the S2Tratio, derived
//  double screanRatio;
//  double slopeScaled; // Slope/Rolloff, derived

  int16_t tMinBias, tMaxBias, tMidBias;
  int16_t dContrast, dBrightness;
  // the trims params
  int32_t level2[TRIM_TYPE_DIM2];

  //// ccm after map
  uint16_t tCrush, tClip, tMid;

  // the Cs
//  double c1, c2, c3;

# if (DM_VER_CTRL == 2) && DM_VER_HIGHER_THAN211
  // to make the TM portion coding easier, duplicate kernel member here
  int16_t tmLutISizeM1, tmLutSSizeM1;
  int16_t tmLutI[TM1_LUT_MAX_SIZE + 3];
  int16_t tmLutS[TM1_LUT_MAX_SIZE];

  int16_t smLutISizeM1, smLutSSizeM1;
  int16_t smLutI[TM1_LUT_MAX_SIZE];
  int16_t smLutS[TM1_LUT_MAX_SIZE];
# endif
} TcCtrlFxp_t;

//// for CSC related matrix
typedef struct CscCtrlFxp_t_ {
  // yuv<=>rgb has only two input modes, sigEnv can hamdle it.

# if DM_FULL_SRC_SPEC
  int16_t sM33Rgb2Lms[3][3];
  int32_t sM33Rgb2LmsScale2P;
  int32_t sV3Wp[3];
  int32_t sV3WpScale2P;
# endif


# if  DM_VER_HIGHER_THAN211
  int16_t tM33Lms2Rgb[3][3];
  int32_t tM33Lms2RgbScale2P;
  int32_t tV3Wp[3];
  int32_t tV3WpScale2P;
# endif

  // the local ipt def
  int16_t m33Lms2Ipt[3][3];
  int32_t m33Lms2IptScale2P;
  int16_t m33Ipt2Lms[3][3];
  int32_t m33Ipt2LmsScale2P;
} CscCtrlFxp_t;

#if EN_GLOBAL_DIMMING
typedef struct GdCtrlDrFxp_t_ {
  // setup time init
  uint32_t gdWDynRngSqrt;
  uint32_t fltrdMeanUpBound;
  uint32_t fltrdMeanLowBound;
  uint32_t gdWeightMean, gdWeightStd;
  // per frame
  uint16_t gdActiveLstFrm; // if gd is on for last frame
  uint16_t fltrdMeanPqDnLstFrm; // the ksOMap has synced to this value
  int32_t fltrdAnchorPqLstFrm; // the ksOMap has synced to this value
  uint32_t tMaxEff, tMinEff;

  // to save ctrl plan cycle, catch these two
  uint32_t fltrdMeanPqDft;
  int32_t fltrdStdPqDft;
  // to control the 3D lut update frequency
  uint32_t gdUdPerFrmsTh, gdUdFrms;   // per frames limit, ... frames ac so far
  uint32_t gdUdDltAnchTh; // delta tMaxEff threshold

  // this is what is actually used
  uint32_t fltrMeanUsed;
  uint32_t fltrdAnchorUsed;

# if DM_VER_LOWER_THAN212
  // setup time init
  //int32_t inRngMin, inRngR; assume it is full and 16 bits
  uint16_t otRngMin, otRng;
  uint32_t rangeOverOne;
  int16_t m33Rgb2YuvScale2P;
  int16_t m33Rgb2Yuv[3][3];
  int32_t v3Rgb2YuvOff[3];
  // per frame
# define EOTF_CRR_LUT_SIZE   1024
  uint16_t eotfCrrLut[EOTF_CRR_LUT_SIZE+1];
  struct GmLut_t_   *hGmLut;
  uint16_t gmLutBlendingAlpha;// just for debug purpose
# endif
} GdCtrlDrFxp_t;
#endif //EN_GLOBAL_DIMMING

#if DM_VER_HIGHER_THAN211
typedef struct Dm3OMapFxp_t_ {
  // lms => target rgb
  int16_t m33Ipt2LmsScale2P;
  int16_t m33Ipt2Lms[3][3];

  // lms => target rgb
  int16_t m33Lms2RgbScale2P;
  int16_t m33Lms2Rgb[3][3];

  OetfParamFxp_t oetfParam;

  int32_t preMapA, preMapB;
  int32_t postMapA, postMapB;

  int16_t l2nlLutAScale2P;
  uint64_t l2nlLutA[DEF_L2NL_LUT_NODES];
  int16_t l2nlLutBScale2P;
  uint16_t l2nlLutB[DEF_L2NL_LUT_NODES];
  int16_t l2nlLutXScale2P;
  uint32_t l2nlLutX[DEF_L2NL_LUT_NODES];

  KClr_t clr;
  int16_t m33Rgb2YuvScale2P;
  int16_t m33Rgb2Yuv[3][3];
  int32_t v3Rgb2YuvOff[3];

  int32_t iptScale;
  int32_t v3IptOff[3];
} Dm3OMapFxp_t;
#endif


// the master all in one DM params: bridge control plan and data
typedef struct DmCtxtFxp_t_ {
  //// input setting.. copy for book keeping: copy after everything is commited. used to
  // (1) retrive runing cfg/mds
  // (2) possible to check if cfg/mds changed or nor during commit
  DmCfgFxp_t dmCfg;
  //// what in metadata bitstream
  MdsExt_t mdsExt;

  //// cache the key matrix and wp
  CscCtrlFxp_t cscCtrl;

  //// for calculation CVM
  TcCtrlFxp_t tcCtrl;
  TcCtrlFxp_t tcCtrlUsed;

# if EN_GLOBAL_DIMMING
  GdCtrlDrFxp_t gdCtrlDr;
# endif

# if DM_VER_IS(2, 120)
  Dm3OMapFxp_t dm3OMap;
# endif

  //// the exec content: what data plan used
  HDmKs_t hDmKs;
} DmCtxtFxp_t;


#endif // #if CDM_FLOAT_POINT == 0
