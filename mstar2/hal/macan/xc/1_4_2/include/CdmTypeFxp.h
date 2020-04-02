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
#ifndef C_DM_TYPE_FXP_H
#define C_DM_TYPE_FXP_H

//#include <stdint.h>
#include "dolbyTypeToMstarType.h"
#include "KCdmModCtrl.h"
#include "CdmType.h"


/*! @brief DM configuration parameter structure

    DM configuration/Control layer parameters

*/
#define SIG_ENV_COM                       \
  /*** frame size and buffer layout ***/  \
  int32_t RowNum, ColNum;                     \
  /* for Y; R,G,B... */                   \
  int32_t RowPitch;  /* row pitch in byte */  \
  /* for U, V; */                         \
  int32_t RowPitchC;                          \
                                          \
  /*** signal definition ***/             \
  CClr_t Clr;                             \
  CChrm_t Chrm;                           \
  CWeav_t Weav;                           \
  CDtp_t Dtp;                             \
                                          \
  CLoc_t Loc;                             \
  CBdp_t Bdp

typedef struct SigEnvCom_t_
{
  SIG_ENV_COM;
} SigEnvCom_t;


/* signal and its targeted environment info*/
typedef struct SrcSigEnvFxp_t_
{
  SIG_ENV_COM;

  // leave the following two var outside of DM_FULL_SRC_SPEC
  // just for easy coding and debug purpose
  CEotf_t Eotf;   // CEotfBt1886, CEotfBtPq coded
  //// color space def
  // that for RGB<=>LMS
  CRng_t Rng; // CRngFull/Narrow/Sdi, for rgb or yuv data range(sdi only for RGB)

# if DM_FULL_SRC_SPEC
  CRgbDef_t RgbDef;   // color space def
# endif

# if DM_VER_HIGHER_THAN211
  int32_t WpExt; // 1 for given externally, keep the value
# endif
  int32_t V3Wp[3];    // the white point
  int16_t WpScale;

# if DM_FULL_SRC_SPEC
  // yuv<=>rgb xfer
  CYuvXferSpec_t YuvXferSpec;
# if EN_KS_DM_IN
  int32_t Yuv2RgbExt;
  int16_t M33Yuv2Rgb[3][3];
  int32_t M33Yuv2RgbScale2P;
  int32_t Yuv2RgbOffExt;
  int32_t V3Yuv2RgbOff[3];
  int32_t V3Yuv2RgbOffInRgb[3];

  int32_t Rgb2LmsRgbwExt;
  int32_t V8Rgbw[8];
  int32_t Rgb2LmsM33Ext;
  int16_t M33Rgb2Lms[3][3];
  int32_t M33Rgb2LmsScale2P;
# endif

  uint16_t Gamma;        // sig gamma value if CEotfBt1886 coded
  uint32_t Min, Max;     // sig env min and max range measure in linear space
  uint16_t MinPq, MaxPq; // sig min max in PQ
  uint16_t A, B;
  uint32_t G;            // sig A, B, G values for tone curve
# endif

  uint16_t DiagSize;     // sig display size in inches

  int32_t CrossTalk;
} SrcSigEnvFxp_t;

typedef struct TgtSigEnvFxp_t_
{
  SIG_ENV_COM;

  CEotf_t Eotf;   // CEotfBt1886, CEotfBtPq coded
  //// color space def
  // that for RGB<=>LMS
  CRng_t Rng; // CRngFull/Narrow/Sdi, for rgb or yuv data range(sdi only for RGB)
# if  DM_VER_HIGHER_THAN211
  CRgbDef_t RgbDef;   // color space def
  int32_t WpExt; // 1 for given externally, keep the value
  int32_t V3Wp[3];    // the white point
  int16_t WpScale;
# endif

# if  DM_VER_HIGHER_THAN211 || EN_GLOBAL_DIMMING
  uint16_t Gamma;     // sig gamma value if CEotfBt1886 coded
# endif
  // yuv<=>rgb xfer
  CYuvXferSpec_t YuvXferSpec;
# if EN_KS_DM_IN
  int32_t Rgb2YuvExt;
  int16_t M33Rgb2Yuv[3][3];
  int32_t M33Rgb2YuvScale2P;
  int32_t Rgb2YuvOffExt;
  int32_t V3Rgb2YuvOff[3];

  int32_t Lms2RgbRgbwExt;
  int32_t V8Rgbw[8];
  int32_t Lms2RgbM33Ext;
  int16_t M33Lms2Rgb[3][3];
  int32_t M33Lms2RgbScale2P;
# endif


  uint32_t Min, Max;    // sig env min and max range measure in linear space
  uint16_t MinPq, MaxPq;// sig min max in PQ
  uint16_t DiagSize;    // sig display size in inches

  int32_t CrossTalk;
} TgtSigEnvFxp_t;

#define TRIM_TYPE_MAX2   6      // TrimTypeNum2
#define TRIM_TYPE_DIM2  (TRIM_TYPE_MAX2 + 1) // Trima[][0] is deNormal tMaxPq
typedef enum TrimType2_t_ {
  TrimTypeTMaxPq2 = 0, // a must be
  TrimTypeSlope   = 1,
  TrimTypeOffset  = 2,
  TrimTypePower   = 3,
  TrimTypeChromaWeight  = 4,
  TrimTypeSatGain       = 5,
  TrimTypeMsWeight      = 6, // must be the last one
  TrimTypeNum2 = TRIM_TYPE_MAX2
} TrimType2_t; // must be sequential

# define TRIM_TYPE_MAX3   0
# define TRIM_TYPE_DIM3   0

typedef struct TmCtrlFxp_t_
{
  // if the the backward tc mapping is for invertible or subjective quality
  // for now, set to 1: invertible, shall be able to derive from panel setting
  int32_t BwdInvertible;
  int32_t Rolloff;
# if DM_VER_HIGHER_THAN211
  int32_t KeyWeight;
  int32_t IntensityVectorWeight;
  int32_t IntensityVectorWeightBwd;
  int32_t ChrmVectorWeight;
  int32_t ChrmVectorWeightBwd;
  int32_t BpWeight;
# endif
  int16_t TMinBias, TMaxBias, TMidBias;
  int16_t DContrast, DBrightness;

  //// trim related
  int32_t l2off;  // turn off level 2
  // level 2:  idxed by TrimType2_t
  int32_t Default2[TRIM_TYPE_DIM2]; // default value
  int32_t ValueAdj2[TRIM_TYPE_DIM2];  // the value adj control
  int32_t CodeBias2[TRIM_TYPE_DIM2];  // offset use when coding to mds
# if DM_VER_LOWER_THAN212
  int32_t ValueAdj2ChromaWeightBwd;
# endif
# if DM_VER_HIGHER_THAN211
  int32_t ValueAdj2SatGainBwd;
# endif
# if DM_VER_LOWER_THAN212
  uint16_t HGain, HOffset;
# endif
} TmCtrlFxp_t;

# if EN_MS_OPTION
/* blending control */
typedef struct MsCtrlFxp_t_
{
  ///// ms blending
  CMsMethod_t MsMethod; // CMsMethodOff, CMsMethodDbEdge

  // the weight in case of DB_EDGE
  //int16_t MsWeight, MsWeightBias; // already in tmCtrl level2
  int16_t MsEdgeWeight;

# if EN_MS_FILTER_CFG
  // filter and padding related params
  // do not support configurable for fixed point control
//  int32_t MsSize[2]; // [0] - row, [1] - col
//  int32_t MsSigma[2];
# endif

} MsCtrlFxp_t;
#endif

# if EN_GLOBAL_DIMMING
typedef struct GdCtrlFxp_t_
{
# if  DM_VER_LOWER_THAN212
  int32_t GdCap; // capable/or not: !0: two gm luts are given
# endif
  int32_t GdOn; //  on/off
  uint32_t GdWMin;
  uint32_t GdWMax;
  uint32_t GdWMm;
  uint32_t GdWDynRngLog10;
  uint32_t GdWDynRngSqrt; // derived from gdWDynRngLog10
  uint32_t GdWeightMean, GdWeightStd;
  // gmLut update frequency control
  uint32_t GdUdPerFrmsTh; // avoid negative, wrap around can ignore
  uint32_t GdUdDltTMaxTh;      // avoid siign unsigned compare

  // PQ derive from linear counter part
  uint16_t GdWMinPq;
  uint16_t GdWMaxPq;
  uint16_t GdWMmPq;
} GdCtrlFxp_t;
# endif


/* dm alg control, except blending part */
typedef struct DmCtrlFxp_t_
{

  // if GPU or CPU impl
  CPlatform_t Platform; // CPlatformCpu, CPlatformCuda
  int DevId;  // deivce ID used for data path
  int32_t CFxp;
  int32_t KFxp;

# if DM_EXTERNAL_CCM
  // to test ignore mds case
  int32_t Igm;
  int32_t Ccm[3];
# endif

  int32_t Lms2IptExt;
  int16_t M33Lms2Ipt[3][3];
  int32_t M33Lms2IptScale2P;
  // not necessary: one is the inverse of the other
  //int Ipt2LmsExt;
  //double M33Ipt2Lms[3][3];

# if EN_AOI
  int32_t AoiOn;
  // def of Aoi[AoiRow0, AoiCol0; AoiRow1Plus1, AoiCol1Plus1)
  int32_t AoiRow0, AoiCol0, AoiRow1Plus1, AoiCol1Plus1;
# endif

  int32_t Prf; // if enable profiling

  ////// for debug only
# if EN_RUN_MODE
  // dm mode: for DM_HARDWIRE_BYPASS == 1 RunMode = 2 will run a special kernel which simply
  // double the input row of 420 into 422 and bit shift to target output bit
  // or just bit shift if input is already 422
  int32_t RunMode;
# endif
} DmCtrlFxp_t;

// the master all in one DM params: control plan side
typedef struct DmCfgFxp_t_
{
  //// that directly defined by init value, cfg file, cli or mds
  DmCtrlFxp_t   dmCtrl;
  SrcSigEnvFxp_t  srcSigEnv;
  TmCtrlFxp_t   tmCtrl;
# if EN_MS_OPTION
  MsCtrlFxp_t   msCtrl;
# endif

# if EN_GLOBAL_DIMMING
  GdCtrlFxp_t      gdCtrl;
# endif


# if  DM_VER_LOWER_THAN212
  struct GmLut_t_ *hGmLut;
# if EN_GLOBAL_DIMMING
  struct GmLut_t_  *hGmLutA;
  struct GmLut_t_  *hGmLutB;
  struct Pq2GLut_t_ *hPq2GLut;
# endif
# endif
  TgtSigEnvFxp_t  tgtSigEnv;

  struct Mmg_t_ *hMmg;
} DmCfgFxp_t;

//// the meta data stream type
//// fixed point scale used
#define DM_PQ_SCALE_2P    12  // 4095  == 2^DM_PQ_SCALE_2P - 1
#define DM_PQ_SCALE     ((1<<DM_PQ_SCALE_2P) - 1)

# define DM_COVERT_TO_FXP_MATCH_FLT(flt_, scale_, fltType_) ( (int)((flt_)*(scale_) + 0.5)/(fltType_)(scale_) )

#define DM_COVERT_TO_FXP(flt_, scale_, fxpType_)  ( (fxpType_)((flt_)*(double)(scale_) + 0.5) )
#define DM_COVERT_TO_FLT(fxp_, scale_, fltType_)  ( (fltType_)((fxp_)/(double)(scale_)) )


#endif // #if CDM_FLOAT_POINT == 0
