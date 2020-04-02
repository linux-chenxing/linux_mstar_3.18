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
#ifndef K_DM_TYPE_FXP_H
#define K_DM_TYPE_FXP_H

//#include <stdint.h>
#include "dolbyTypeToMstarType.h"
#include "KCdmModCtrl.h"
#include "KdmType.h"
#include "KdmTypeFxpCmn.h"

#if MSTAR_DRIVER_MOD
#define MSTAR_DEGAMMA_LUT_SIZE 512
#define MSTAR_GAMMA_LUT_SIZE   512
#endif
////// data path/kernel structure: KS
//// E=>O
typedef struct EotfParamFxp_t_
{
  // range
  uint16_t rangeMin, range;
  uint32_t rangeInv;
  uint16_t bdp;
  // gamma stuff
  KEotf_t eotf;   // CEotfBt1886, CEotfPq coded
# if (REDUCED_COMPLEXITY == 0)
  uint16_t gamma;
  uint16_t a, b;
  uint32_t g;
# endif
} EotfParamFxp_t;

//// O=>E
typedef struct OetfParamFxp_t_
{
  //// range
  uint32_t min, max; // in linear output space
  uint16_t rangeMin, range; // in final output
  uint32_t rangeOverOne;
  uint16_t bdp;

  // gamma stuff
  KEotf_t oetf;   // CEotfBt1886, CEotfPq coded
} OetfParamFxp_t;

//// input mapping
typedef struct DmKsIMapFxp_t_ {
# if !EN_IPT_PQ_ONLY_OPTION
  KClr_t clr;
  int16_t m33Yuv2RgbScale2P;
  int16_t m33Yuv2Rgb[3][3];
  int32_t v3Yuv2RgbOffInRgb[3];

  EotfParamFxp_t eotfParam;

  #if MSTAR_DRIVER_MOD
  uint32_t g2L[MSTAR_DEGAMMA_LUT_SIZE];
#else

# if REDUCED_COMPLEXITY
  uint32_t g2L[DEF_G2L_LUT_SIZE];
# endif

#endif

  int16_t m33Rgb2LmsScale2P;
  int16_t m33Rgb2Lms[3][3];

  int16_t m33Lms2IptScale2P;
  int16_t m33Lms2Ipt[3][3];
# endif

# if EN_IPT_PQ_ONLY_OPTION || DM_VER_CTRL == 3
  int32_t iptScale;
  int32_t v3IptOff[3];
# endif
} DmKsIMapFxp_t;


/// tone curve mapping
typedef struct Dm3KsTMapFxp_t_ {
# if EN_SKIP_TMAP_MS
  KTcLutDir_t tcLutDir; // to signal the lut direction or bypass
# endif

  int16_t tmLutISizeM1;

# if REDUCED_TC_LUT
  uint16_t tmInternal515Lut[512+3];
  uint16_t tmLutMaxVal;
# else
  uint16_t tmLutI[4096];
# endif
  uint16_t chromaWeight;

} DmKsTMapFxp_t;


//// ms blending
#if EN_MS_OPTION
#if EN_EDGE_SIMPLE
# define MS_FLTR_PAIR_NUM   1
#else
# define MS_FLTR_PAIR_NUM   2
#endif
typedef struct Dm3KsMsFxp_t_ {
  int16_t disMs;

  //// filter size
  int16_t fltrRadius[2]; // [0]: row, [1]: col

  //// filter scale
  int16_t fltrScale;

  // reserve memory
  int16_t fltrRowMem[MS_FLTR_PAIR_NUM][MS_ROW_FLTR_RADIUS_MAX+1];
  int16_t fltrColMem[MS_FLTR_PAIR_NUM][MS_COL_FLTR_RADIUS_MAX+1];
  //// gaussian type filter
  int16_t *fltrLp[2];

  //// the weights
  uint16_t msWeight, msEdgeWeight;

  //// internal buf and pitch 
  //// intermediated filtered result
  int16_t *tpRowBuf; // tmp buffer for convRowTp
  int16_t tpRowPitchNum; // pitch in index
  // tmp buffer for lin - adm
  int16_t *linAdmBuf; 
# if !EN_EDGE_SIMPLE
  //// laplacian type filter
  int16_t *fltrHp[2]; // edge filer across row and col, derivative of low pass filter
  // edge detection buffer
  int16_t *edgeBuf[2]; 
# endif

# if EN_AOI
  // tmp buffer: AOI case need to preserve adm content out of AOI
  // possible to avoid use it by use linear buffer in DM3.
  int16_t *tmpBuf;
# endif

} DmKsMsFxp_t;
#endif

/// output mapping

typedef struct Dm2KsOMapFxp_t_ {
  uint16_t gain, offset;
  uint16_t satGain;

  DmKs2GmLutFxp_t ksGmLut;

  uint16_t bdp;

  uint16_t tRangeMin, tRange;
  uint32_t tRangeOverOne;
  uint32_t tRangeInv;

  KClr_t clr;
  int16_t m33Rgb2YuvScale2P;
  int16_t m33Rgb2Yuv[3][3];
  int32_t v3Rgb2YuvOff[3];
} Dm2KsOMapFxp_t;

typedef struct Dm2KsOMapFxp_t_ DmKsOMapFxp_t;
//typedef Dm2KsOMapFxp_t DmKsOMapFxp_t;


// for pre-post processing: up/down sampling
// an all in one structure
typedef struct DmKsUdsFxp_t_
{
  //// for UV
# if EN_UP_DOWN_SAMPLE_OPTION 
  KChrm_t chrmIn, chrmOut;
# endif

  int16_t minUs, maxUs, minDs, maxDs;

# if EN_UP_DOWN_SAMPLE_OPTION 
  int16_t filterUvRowUsHalfSize;
  int16_t filterUvRowUsScale2P;
  int16_t filterUvRowUs0_m[8];
  int16_t filterUvRowUs1_m[8];

  int16_t filterUvColUsHalfSize;
  int16_t filterUvColUsScale2P;
  int16_t filterUvColUs_m[8];

  int16_t filterUvColDsRadius;
  int16_t filterUvColDsScale2P;
  int16_t filterUvColDs_m[16];
# endif
} DmKsUdsFxp_t;

#define DmKsUsFxp_t DmKsUdsFxp_t
#define DmKsDsFxp_t DmKsUdsFxp_t

//// the ks collection
typedef struct DmKsFxp_t_
{
# if EN_UP_DOWN_SAMPLE_OPTION || EN_MS_OPTION
  //// tmp buf to store (I, P, T), post up sample or pre down sample (Y, U, V)
  int16_t rowPitchNum;   // pitch in index

  uint16_t *frmBuf0, *frmBuf1, *frmBuf2;

# if EN_MS_OPTION
  //// tmp buf to store adm
  uint16_t *frmBuf3;
# endif
# endif


  //// HARDWARE REGISTER MAP START ////

  DmKsUdsFxp_t ksUds;
# define ksUs   ksUds
# define ksDs   ksUds

  DmKsCtrl_t ksDmCtrl;

  DmKsFrmFmt_t ksFrmFmtI;

  DmKsIMapFxp_t  ksIMap;

  DmKsTMapFxp_t ksTMap;


# if EN_MS_OPTION
  DmKsMsFxp_t ksMs;
  //// tmp buf to store adm
# endif

  DmKsOMapFxp_t ksOMap;

  DmKsFrmFmt_t ksFrmFmtO;

# if EN_RUN_MODE
  int16_t bypassShift;
# endif

  //// HARDWARE REGISTER MAP END ////

} DmKsFxp_t;


#endif // K_DM_TYPE_FXP_H
