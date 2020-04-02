/****************************************************************************
* This product contains one or more programs protected under international
* and U.S. copyright laws as unpublished works.  They are confidential and
* proprietary to Dolby Laboratories.  Their reproduction or disclosure, in
* whole or in part, or the production of derivative works therefrom without
* the express permission of Dolby Laboratories is prohibited.
*
*             Copyright 2011 - 2015 by Dolby Laboratories.
* 			              All rights reserved.
****************************************************************************/
#ifndef VDRDMAPIP_FXP_H_
#define VDRDMAPIP_FXP_H_

//#include <stdint.h>
#include "KdmType.h"
#include "KdmTypeFxpCmn.h"
#include "dolbyTypeToMstarType.h"
//// the master all in one DM params for fixed point data plan
typedef struct DmExecFxp_t_
{
	// source range
	uint16_t sRangeMin, sRange;
	uint32_t sRangeInv;

	//// forward
	// yuv2rgb xfer
	int16_t m33Yuv2RgbScale2P;
	int16_t m33Yuv2Rgb[3][3];
	int32_t v3Yuv2RgbOffInRgb[3];
# if REDUCED_COMPLEXITY
	uint32_t g2L[DEF_G2L_LUT_SIZE];
# endif
	// gamma stuff
	KEotf_t sEotf;		// EOTF_MODE_BT1886, EOTF_MODE_PQ coded
	uint16_t sA, sB, sGamma;
	uint32_t sG;
	// rgb=>lms or other
	int16_t m33Rgb2OptScale2P;
	int16_t m33Rgb2Opt[3][3];
	// lms or other to ipt or other
	int16_t m33Opt2OptScale2P;
	int16_t m33Opt2Opt[3][3];
	int32_t Opt2OptOffset;
	// Helmoltz-Kohlrasush scaling
	uint16_t chromaWeight; // 64K - 1 scale

	//// TC
# if REDUCED_TC_LUT
  uint16_t tcLut[512+3];
	uint16_t tcLutMaxVal;
# else
  uint16_t tcLut[4096];
# endif

	//// blending
	uint16_t msWeight, msWeightEdge;

	//// backward
	DmLutFxp_t bwDmLut;
	// gain/offset stuff
	uint16_t gain, offset; // 4K - 1 scale
	// Saturation control
	uint16_t saturationGain;

  uint16_t tRangeMin, tRange;
  uint32_t tRangeOverOne;
  uint32_t tRangeInv;

  // to support output xyz/rgb=>yuv conversion.
  int16_t m33Rgb2YuvScale2P;
  int16_t m33Rgb2Yuv[3][3];
  int32_t v3Rgb2YuvOff[3];

# if EN_AOI
   // def of aoi[aoiRow0, aoiCol0; aoiRow1Plus1, aoiCol1Plus1)
  int16_t aoiRow0, aoiCol0, aoiRow1Plus1, aoiCol1Plus1;
# endif
} DmExecFxp_t;


//// the ks collection backward comaptible to DM v2.8.6
typedef struct DmKsFxpBack_t_
{
	//// tmp buf to store (I, P, T), post up sample or pre down sample (Y, U, V)
	int16_t rowPitchNum;   // pitch in index
	uint16_t *frmBuf0, *frmBuf1, *frmBuf2;

	DmKsFrmFmt_t ksFrmFmtI;

	//// the exec content: what data plan used
	DmExecFxp_t dmExec;

	//// internal buf and pitch 
	//// intermediated filtered result
	int16_t *tpRowBuf; // tmp buffer for convRowTp
	// tmp buffer for lin - adm
	int16_t *linAdmBuf; 
	// edge detection buffer
	int16_t *edgeBuf[2]; 

	//// tmp buf to store adm
	uint16_t *frmBuf3;

# if EN_AOI
  // tmp buffer: AOI case need to preserve adm content out of AOI
  // possible to avoid use it by use linear buffer in DM3.
  int16_t *tmpBuf;
# endif

  DmKsFrmFmt_t ksFrmFmtO;
} DmKsFxpBack_t;

#if REDUCED_COMPLEXITY == 0
// DM 286 kenerl structures. Used only for kernel dumping
typedef struct DmLutFxp286_t_ {
  int16_t dimC1, dimC2, dimC3; // dim for C1, C2, C3

  int16_t valTp;
  int16_t scale;    // NOT USED IN FIXED POINT INSTANCE

  int16_t iMinC1, iMaxC1, iMinC2, iMaxC2, iMinC3, iMaxC3;
  int32_t iDistC1Inv, iDistC2Inv, iDistC3Inv;

    // the pre-calculated  value
  int16_t pitch, slice;

  ////// lut data
  uint16_t lutMap[3*GMLUT_MAX_DIM*GMLUT_MAX_DIM*GMLUT_MAX_DIM]; // the LUT, type depends on version and type
} DmLutFxp286_t;

typedef enum ImpMethod_t_ {
  IMP_METHOD_C = 0,
  IMP_METHOD_GPU,
} ImpMethod_t;

typedef enum RunMode_t_ {
  RUN_MODE_NORMAL = 0,
  RUN_MODE_FROMHDMI = 2, // for cfg only
  RUN_MODE_NUM = 3 // always 4, to make it simply
} RunMode_t;

typedef enum EOTF_t_ {
  EOTF_MODE_BT1886 = 0,
  EOTF_MODE_PQ
} EOTF_t;

typedef enum AlgMethod_t_ {
  ALG_METHOD_ALG = 0,
  ALG_METHOD_LUT,
  ALG_METHOD_NUM
} AlgMethod_t;

typedef struct DmExecFxp286_t_
{
  ////// processing block control
  // if GPU or CPU impl:IMP_METHOD_C, IMP_METHOD_GPU
  ImpMethod_t implMethod; // NOT USED IN FIXED POINT INSTANCE

  ////// kernel control
  // run mode
  RunMode_t runMode;
  int16_t lowCmplxMode;
  uint16_t haveGr; // if graphic enabled

  // source range
  uint16_t sRangeMin, sRange;
  uint32_t sRangeInv;

  //// forward
  // yuv2rgb xfer
  int16_t m33Yuv2RgbScale2P;
  int16_t m33Yuv2Rgb[3][3];
  // v3Yuv2RgbOffInRgb: scaled by 1<<m33Yuv2RgbScale2P
  // v3Rgb = (m33Yuv2Rgb * v3Yuv - v3Yuv2RgbOffInRgb)/(1<<m33Yuv2RgbScale2P)
  int32_t v3Yuv2RgbOffInRgb[3];
  // gamma stuff
  EOTF_t sEotf;   // EOTF_MODE_BT1886, EOTF_MODE_PQ coded
  uint16_t sA, sB, sGamma;
  uint32_t sG;
  // rgb=>lms or other
  int16_t m33Rgb2OptScale2P;
  int16_t m33Rgb2Opt[3][3];
  // lms or other to ipt or other
  int16_t m33Opt2OptScale2P;
  int16_t m33Opt2Opt[3][3];
  int32_t Opt2OptOffset;
  // Helmoltz-Kohlrasush scaling
  uint16_t chromaWeight; // 64K - 1 scale
  // lut in the context
  int32_t fwPrf;      // NOT USED IN FIXED POINT INSTANCE

  //// TC
  //TcCtrlFxp_t tcCtrl;
  uint16_t *tcLut;

  //// blending
  uint16_t msFilterScale;
  int16_t msFilterRow[11];
  int16_t msFilterCol[5];
  int16_t msFilterEdgeRow[11];
  int16_t msFilterEdgeCol[5];
  uint16_t msWeight, msWeightEdge;

  //// backward
  // if use LUT or alg: ALG_METHOD_ALG, ALG_METHOD_LUT
  AlgMethod_t bwMethod; // NOT USED IN FIXED POINT INSTANCE
  DmLutFxp286_t bwDmLut;
  // gain/offset stuff
  uint16_t gain, offset; // 4K - 1 scale
  // Saturation control
  uint16_t saturationGain;
  // Gamma stuff
  EOTF_t tEotf;     // NOT USED IN FIXED POINT INSTANCE
  uint16_t tRangeMin, tRange;
  uint32_t tRangeOverOne;
  int32_t bwPrf;      // NOT USED IN FIXED POINT INSTANCE

  // LUTs for low complexity
  uint16_t gamma2PqLutSrc[512];
  uint16_t gamma2PqLutTgt[512];
  uint16_t pq2GammaLut[512];
} DmExecFxp286_t;
#endif // #if REDUCED_COMPLEXITY == 0

#endif /* VDRDMAPIP_H_ */
