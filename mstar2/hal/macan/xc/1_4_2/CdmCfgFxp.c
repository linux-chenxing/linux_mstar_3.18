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
//#include <assert.h>
#include "CdmTypeFxp.h"
#include "VdrDmApi.h"
#include "CdmTypePriFxp.h"
#include "CdmUtilFxp.h"
#include "CdmMmg.h"


#ifndef MSTAR_DRIVER_MOD
#define MSTAR_DRIVER_MOD 1
#endif

#define CLR_M2C(m)   (CClr_t)(  ((m) == 0) ? CClrYuv :           \
                                  ((m) == 1) ? CClrRgb : CClrIpt  )

#define CHRM_M2C(m)  (CChrm_t)( ((m) == 0) ? CChrm420 :            \
                                  ((m) == 1) ? CChrm422 : CChrm444  )

#define WEAV_M2C(m)   (CWeav_t)(  ((m) == 0) ? CWeavPlnr :            \
                                  ((m) == 1) ? CWeavUyVy : CWeavPlnr  )

HDmFxp_t InitDm(const HDmCfgFxp_t hDmCfg, HDmFxp_t hDm)
{
  hDm->mdsExt.source_diagonal = 0xffff; // to make MDS committed
  hDm->mdsExt.signal_eotf = 0xffff; // to force cfg update

  (void)hDmCfg; // for now



  return hDm;
}


void InitSrcSigEnv(SrcSigEnvFxp_t *pSigEnv)
{
  // default size, no AOI
  pSigEnv->RowNum = DM_DEF_ROW_NUM;
  pSigEnv->ColNum = DM_DEF_COL_NUM;

  pSigEnv->Dtp = CDtpU16;
  pSigEnv->Weav =  CWeavPlnr; // to support APP to use AllocFrmMem right
  pSigEnv->Loc =  CLocHost;

  pSigEnv->Clr = CClrYuv;   // to support APP get source file type

# if DM_FULL_SRC_SPEC
  pSigEnv->Bdp = 14;

  pSigEnv->Eotf = CEotfBt1886;

  // color space def
  pSigEnv->Rng = CRngFull;
  pSigEnv->RgbDef = CRgbDefP3d65;
# if EN_KS_DM_IN
  pSigEnv->Rgb2LmsRgbwExt = 0;
  pSigEnv->Rgb2LmsM33Ext = 0;
# endif
# if DM_VER_HIGHER_THAN211
  pSigEnv->WpExt = 0;
# endif
//  pSigEnv->WpScale = GetRgb2LmsWpByDefV3Fxp(pSigEnv->RgbDef, pSigEnv->V3Wp);
  pSigEnv->V3Wp[0] = pSigEnv->V3Wp[1] = pSigEnv->V3Wp[2] = 32767;
  pSigEnv->WpScale = 15;

  pSigEnv->YuvXferSpec = CYuvXferSpecR709;
# if EN_KS_DM_IN
  pSigEnv->Yuv2RgbExt = 0;
  pSigEnv->Yuv2RgbOffExt = 0;
# endif

/*Init gamma settings:
  min=[0.0001, 1.0]; max=[100, 10000]; gamma=[2.2, 2.8]
  A = (max^(1/gamma)-min^(1/gamma))^gamma;
  B = min^(1/gamma)/(max^(1/gamma)-min^(1/gamma));
  Gamma = gamma * 2^14;
  A = A * 2^2;
  B = B * 2^16;
  G = 2^gamma * 2^18;
*/
  pSigEnv->Gamma = (uint16_t)(2.4 * (1 << 14) + 0.5); //39322;   // 2.4, sig gamma value if CEotfBt1886 coded
#if SDR_SOURCE // if SDR 100 nits source, use this
  pSigEnv->Min = (uint32_t)(0.005 * (1 << 18));
  pSigEnv->Max = (uint32_t)(100 * (1 << 18));
  pSigEnv->MinPq = 0; //LToPQ12(pSigEnv->Min);
  pSigEnv->MaxPq = 0; //LToPQ12(pSigEnv->Max);
  pSigEnv->A = 385;
  pSigEnv->B = 1075;
  pSigEnv->G = 1383604;
#else // DoVi 4000 nits
  pSigEnv->Min = (uint32_t)(0.005 * (1 << 18));
  pSigEnv->Max = (uint32_t)(4000LL * (1 << 18));
  pSigEnv->MinPq = 0; //LToPQ12(pSigEnv->Min);
  pSigEnv->MaxPq = 0; //LToPQ12(pSigEnv->Max);
  pSigEnv->A = 15867;
  pSigEnv->B = 228;
  pSigEnv->G = 1383604;
#endif
#else // init to dummy or invalid value
  pSigEnv->Chrm = CChrmNum;  // APP shall take care of it if !DM_FULL_SRC_SPEC
  pSigEnv->Bdp = 0;

  pSigEnv->Eotf = CEotfNum;

  // color space def
  pSigEnv->Rng = CRngNum;
# if  DM_VER_HIGHER_THAN211
  pSigEnv->wpExt = 0;
# endif
  pSigEnv->V3Wp[0] = pSigEnv->V3Wp[1] = pSigEnv->V3Wp[2] = 32767;
  pSigEnv->wpScale = 15;
# endif

  pSigEnv->DiagSize = 42;   // sig target display size in inches
  pSigEnv->CrossTalk = 655;   // 0.02, scale 15
}


int GetSrcSigEnv(SrcSigEnvFxp_t *pSigEnv, HDmFxp_t hDm)
{
  if (hDm) {
    MemCpyByte(pSigEnv, &hDm->dmCfg.srcSigEnv, sizeof(SrcSigEnvFxp_t));
    return 0;
  }

  return -1;
}

void InitTgtSigEnv(TgtSigEnvFxp_t *pSigEnv)
{
  // default output:  what core DM is expected
  pSigEnv->RowNum = DM_DEF_ROW_NUM;
  pSigEnv->ColNum = DM_DEF_COL_NUM;

  pSigEnv->Dtp = CDtpU16;
  pSigEnv->Weav =  CWeavUyVy;
  pSigEnv->Loc =  CLocHost;

  pSigEnv->Chrm = CChrm422;
  pSigEnv->Bdp = 12;

  pSigEnv->Clr = CClrYuv;

  pSigEnv->Eotf = CEotfBt1886;

  pSigEnv->Rng = CRngNarrow;

# if DM_VER_HIGHER_THAN211
  pSigEnv->RgbDef = CRgbDefR709;
# if EN_KS_DM_IN
  pSigEnv->Lms2RgbRgbwExt = 0;
  pSigEnv->Lms2RgbM33Ext = 0;
# endif

  pSigEnv->WpExt = 0;
//  pSigEnv->WpScale = GetRgb2LmsWpByDefV3Fxp(pSigEnv->RgbDef, pSigEnv->V3Wp);
  pSigEnv->V3Wp[0] = pSigEnv->V3Wp[1] = pSigEnv->V3Wp[2] = 32767;
  pSigEnv->WpScale = 15;
# endif

# if DM_VER_HIGHER_THAN211 || EN_GLOBAL_DIMMING
  pSigEnv->Gamma = 39322;   // 2.4, sig gamma value if CEotfBt1886 coded
# endif

  pSigEnv->YuvXferSpec = CYuvXferSpecR709;
# if EN_KS_DM_IN
  pSigEnv->Rgb2YuvExt = 0;
  pSigEnv->Rgb2YuvOffExt = 0;
# endif

pSigEnv->Min = (uint32_t)(0.005 * (1 << 18));
  pSigEnv->Max = (uint32_t)(100 * (1 << 18));
  pSigEnv->MinPq = 0; //LToPQ12(pSigEnv->Min);
  pSigEnv->MaxPq = 0; //LToPQ12(pSigEnv->Max);

  pSigEnv->DiagSize = 42;   // sig target display size in inches
  pSigEnv->CrossTalk = 655;   // 0.02, scale 15
}

int GetTgtSigEnv(TgtSigEnvFxp_t *pSigEnv, HDmFxp_t hDm)
{
  if (hDm) {
    MemCpyByte(pSigEnv, &hDm->dmCfg.tgtSigEnv, sizeof(TgtSigEnvFxp_t));
    return 0;
  }

  return -1;
}

void InitTmCtrl(TmCtrlFxp_t *pTmCtrl)
{
  int i;
  assert(TrimTypeNum2 == TRIM_TYPE_MAX2);
  assert(TrimTypeMsWeight == TrimTypeNum2);

  ////// TC
  pTmCtrl->BwdInvertible = 0; // cfg/cli controlable but hidden
  // slope scalling factor: follow matlab code for scale
  pTmCtrl->Rolloff = 715827882LL; //DM_COVERT_TO_FXP_MATCH_FLT(1.0/3.0, (1<<31), double);
# if DM_VER_HIGHER_THAN211
  pTmCtrl->KeyWeight = 3 * 4096; // range 0 ~ 15
  pTmCtrl->IntensityVectorWeight = 6 * 4096; // range -15 ~ 15
  pTmCtrl->IntensityVectorWeightBwd = 20 * 4096;
  pTmCtrl->ChrmVectorWeight = 6 * 4096; // range -15 ~ 15
  pTmCtrl->ChrmVectorWeightBwd = 6 * 4096; // range -15 ~ 15
  pTmCtrl->BpWeight = 0 * 4096; // range 0 ~ 1.0
# endif
  pTmCtrl->TMidBias = 0;
  pTmCtrl->TMaxBias = 0;
  pTmCtrl->TMinBias = 0;
  pTmCtrl->DBrightness = 0;
  pTmCtrl->DContrast = 0;

  ////// From IPT

# if DM_VER_LOWER_THAN212
  pTmCtrl->HGain = 2048;
  pTmCtrl->HOffset = 4095;
# endif

  // level2
  pTmCtrl->Default2[TrimTypeSlope]  = 0;
  pTmCtrl->Default2[TrimTypeOffset] = 0;
  pTmCtrl->Default2[TrimTypePower]  = 0;

  pTmCtrl->Default2[TrimTypeChromaWeight] = 0;
  pTmCtrl->Default2[TrimTypeSatGain]      = 4096;
  pTmCtrl->Default2[TrimTypeMsWeight]     = 2048;  // 1.0, scale 11 as in metadata

  for (i = 0; i < TRIM_TYPE_DIM2; ++i) {
    pTmCtrl->ValueAdj2[i] = 0;
  }
# if DM_VER_LOWER_THAN212
  pTmCtrl->ValueAdj2ChromaWeightBwd = (int32_t)(0.18 * 4096);
# else
  pTmCtrl->ValueAdj2SatGainBwd = (int32_t)(0.0 * 4096);
  #endif

  pTmCtrl->CodeBias2[TrimTypeSlope]   = -2048;  // -0.5, 0,5
  pTmCtrl->CodeBias2[TrimTypeOffset]  = -2048;  // -0.5, 0.5
  pTmCtrl->CodeBias2[TrimTypePower]   = -2048;  // -0.5, 0.5
  pTmCtrl->CodeBias2[TrimTypeChromaWeight]  = -2048;  // 0, 4
  pTmCtrl->CodeBias2[TrimTypeSatGain]       =  2048;  // 0.5, 1.5
  pTmCtrl->CodeBias2[TrimTypeMsWeight]      =  0;   // 0, 2


}

int GetTmCtrl(TmCtrlFxp_t *pTmCtrl, HDmFxp_t hDm)
{
  if (hDm) {
    MemCpyByte(pTmCtrl, &hDm->dmCfg.tmCtrl, sizeof(TmCtrlFxp_t));
    return 0;
  }

  return -1;
}

#if EN_MS_OPTION
void InitMsCtrl(MsCtrlFxp_t *pMsCtrl)
{
  pMsCtrl->MsMethod = CMsMethodDbEdge;

  //pMsCtrl->MsWeight = 2048;
  //pMsCtrl->MsWeightBias = 0;

#if DM_VER_IS(2, 110)
  pMsCtrl->MsEdgeWeight = 4095 * 4; // to be bit exact to DM2.11.x
# else
  pMsCtrl->MsEdgeWeight = 4095 * 4;
# endif

  //pMsCtrl->MsSize[0] = 11;
  //pMsCtrl->MsSize[1] = 5;
  //pMsCtrl->MsSigma[0] = 2.0;
  //pMsCtrl->MsSigma[1] = 1.0;
}

int GetMsCtrl(MsCtrlFxp_t *pMsCtrl, HDmFxp_t hDm)
{
  if (hDm) {
    MemCpyByte(pMsCtrl, &hDm->dmCfg.msCtrl, sizeof(MsCtrlFxp_t));
    return 0;
  }

  return -1;
}
#endif

#if EN_GLOBAL_DIMMING
void InitGdCtrl(GdCtrlFxp_t *pGdCtrl)
{
# if DM_VER_LOWER_THAN212
  pGdCtrl->GdCap = 0;
# endif
  pGdCtrl->GdOn = 1;

  pGdCtrl->GdWMin = (uint32_t)(0.1 * (1 << 18));
  pGdCtrl->GdWMax = (uint32_t)(400 * (1 << 18));
  pGdCtrl->GdWMm = (uint32_t)(100 * (1 << 18));

  pGdCtrl->GdWMinPq = LToPQ12(pGdCtrl->GdWMin);
  pGdCtrl->GdWMaxPq = LToPQ12(pGdCtrl->GdWMax);
  pGdCtrl->GdWMmPq = LToPQ12(pGdCtrl->GdWMm);

  pGdCtrl->GdWDynRngLog10 = 3 * (1 << 18); // not used in fixed point
  pGdCtrl->GdWDynRngSqrt = 8289721; // pow(10, pGdCtrl->>gdWDynRngLog10/2) * (1 << 18);

  pGdCtrl->GdWeightMean = (uint32_t)(1 * (1 << 12));
  pGdCtrl->GdWeightStd = (uint32_t)(0.5* (1 << 12));

  pGdCtrl->GdUdPerFrmsTh = 0;
  pGdCtrl->GdUdDltTMaxTh = 0;
}

int GetGdCtrl(GdCtrlFxp_t *pGdCtrl, HDmFxp_t hDm)
{
  if (hDm) {
    MemCpyByte(pGdCtrl, &hDm->dmCfg.gdCtrl, sizeof(GdCtrlFxp_t));
    return 0;
  }

  return -1;
}
#endif

void InitDmCtrl(DmCtrlFxp_t *pDmCtrl)
{
  //set up init value
  pDmCtrl->Platform = CPlatformCpu;
  pDmCtrl->DevId = 0;

# if EN_AOI
  pDmCtrl->AoiOn = 0;
  pDmCtrl->AoiRow0 = 0;
  pDmCtrl->AoiRow1Plus1 = DM_MAX_ROW_NUM;
  pDmCtrl->AoiCol0 = 0;
  pDmCtrl->AoiCol1Plus1 = DM_MAX_COL_NUM;
# endif

#if EN_RUN_MODE
  pDmCtrl->RunMode = 0;
#endif

  pDmCtrl->Prf = 0;
}

int GetDmCtrl(DmCtrlFxp_t *pDmCtrl, HDmFxp_t hDm)
{
  if (hDm) {
    MemCpyByte(pDmCtrl, &hDm->dmCfg.dmCtrl, sizeof(DmCtrlFxp_t));
    return 0;
  }

  return -1;
}

HDmCfgFxp_t  InitDmCfg(CPlatform_t platform, HMmg_t hMmg, DmCfgFxp_t *pDmCfg)
{
  pDmCfg->hMmg = hMmg;

  pDmCfg->dmCtrl.CFxp = 1; // control is fixed
  pDmCfg->dmCtrl.Platform = platform;

  //// dm ctrl
  InitDmCtrl(&pDmCfg->dmCtrl);

  //// original target display
  InitSrcSigEnv(&pDmCfg->srcSigEnv);

  //// DM Target Display Parameters
  InitTgtSigEnv(&pDmCfg->tgtSigEnv);

  InitTmCtrl(&pDmCfg->tmCtrl);

# if EN_MS_OPTION
  //// multi scale blending control
  InitMsCtrl(&pDmCfg->msCtrl);
# endif

# if EN_GLOBAL_DIMMING
  InitGdCtrl(&pDmCfg->gdCtrl);
# endif

  return pDmCfg;
}
int GetDmCfg(DmCfgFxp_t *pDmCfg, HDmFxp_t hDm)
{
  if (hDm) {
    MemCpyByte(pDmCfg, &hDm->dmCfg, sizeof(DmCfgFxp_t));
    return 0;
  }

  return -1;
}

void  InitMdsExt(const HDmCfgFxp_t hDmCfg, HMdsExt_t hMdsExt)
{
//  const TmCtrlFxp_t *pTmCtrl = &hDmCfg->tmCtrl;
  TrimSet_t *pTrimSet = &hMdsExt->trimSets;
  Trim_t *pTrim;
  int trimType;

  // level 1 default
  hMdsExt->min_PQ = 0;
  hMdsExt->max_PQ = DM_PQ_SCALE;
  hMdsExt->mid_PQ = (hMdsExt->min_PQ + hMdsExt->max_PQ) >> 1;

  //// set default trim, for metadata bit stream, mdsExt.max_PQ value does not matter
  pTrimSet->TrimSetNum = 0;

  // default value for level 2 in [0]
  pTrim = &pTrimSet->TrimSets[0];
  pTrim->TrimLevel = 2;
  pTrim->TrimTypeDim = TRIM_TYPE_DIM2;
  pTrim->TrimNumMax = TRIM_NUM_MAX;
  pTrim->Trima = &pTrimSet->TrimaMem[0];
  pTrim->TrimNum = 0;
  // [0][] for default value
  pTrim->Trima[0] = hMdsExt->max_PQ;
  for (trimType = 1; trimType < TrimTypeMsWeight; ++trimType) {
    pTrim->Trima[trimType] =
      (uint16_t)(hDmCfg->tmCtrl.Default2[trimType] - hDmCfg->tmCtrl.CodeBias2[trimType]);
  }
//# if EN_MS_OPTION
  pTrim->Trima[TrimTypeMsWeight] =
    (uint16_t)(hDmCfg->tmCtrl.Default2[TrimTypeMsWeight] - hDmCfg->tmCtrl.CodeBias2[TrimTypeMsWeight]);
//# endif


}

static int SkipByteAligned(unsigned char **ppUc, int *pBufSize, int sb)
{
  if (*pBufSize >= sb && sb >= 0) {
    *ppUc += sb;
    *pBufSize -= sb;

    return 1;
  }
  return 0;
}

static int ReadU8(unsigned char **ppUc, int *pDataSize, unsigned char *pu8)
{
  if (*pDataSize) {
    *pu8 = *(*ppUc)++;
    *pDataSize -= 1;
    return 1;
  }

  return 0;
}

# if DM_FULL_SRC_SPEC
static int WriteU8(unsigned char **ppUc, int *pDataSize, unsigned char u8)
{
  if (*pDataSize) {
    *(*ppUc)++ = u8;
    *pDataSize -= 1;
    return 1;
  }

  return 0;
}
#endif

static int ReadU16(unsigned char **ppUc, int *pDataSize, unsigned short *pu16)
{
  if (*pDataSize >= 2) {
#   if BIN_MD_BE
    *pu16 = (((unsigned short)(*ppUc)[0])<<8) | (*ppUc)[1];
# else
    *pu16 = (((unsigned short)(*ppUc)[1])<<8) | (*ppUc)[0];
# endif

    *ppUc += 2;
    *pDataSize -= 2;

    return 1;
  }

  return 0;
}

static int ReadI16(unsigned char **ppUc, int *pDataSize, short *pi16)
{
  if (*pDataSize >= 2) {
#   if BIN_MD_BE
    *pi16 = (((short)(*ppUc)[0])<<8) | (*ppUc)[1];
# else
    *pi16 = (((short)(*ppUc)[1])<<8) | (*ppUc)[0];
# endif

    *ppUc += 2;
    *pDataSize -= 2;

    return 1;
  }

  return 0;
}

# if DM_FULL_SRC_SPEC
static int WriteU16(unsigned char **ppUc, int *pDataSize, unsigned short u16)
{
  if (*pDataSize >= 2) {
# if BIN_MD_BE
    (*ppUc)[0] = u16>>8;
    (*ppUc)[1] = u16 & 0xFF;
# else
    (*ppUc)[0] = u16 & 0xFF;
    (*ppUc)[1] = u16>>8;
# endif

    *ppUc += 2;
    *pDataSize -= 2;

    return 1;
  }

  return 0;
}
#endif

static int ReadU32(unsigned char **ppUc, int *pDataSize, unsigned *pu32)
{
  if (*pDataSize >= 4) {
# if BIN_MD_BE
    *pu32 = (((unsigned)(*ppUc)[0])<<24) | (((unsigned)(*ppUc)[1])<<16) | (((unsigned)(*ppUc)[2])<<8) | (*ppUc)[3];
# else
    *pu32 = (((unsigned)(*ppUc)[3])<<24) | (((unsigned)(*ppUc)[2])<<16) | (((unsigned)(*ppUc)[1])<<8) | (*ppUc)[0];
# endif

    *ppUc += 4;
    *pDataSize -= 4;

    return 1;
  }

  return 0;
}

#define READ_VECTOR(ReadFunc, ppBuf, pDataSize, loopVar, vecSize, vec) \
  for (loopVar = 0; loopVar < vecSize; ++loopVar) ReadFunc(ppBuf, pDataSize, vec + loopVar)

# define EXIT_ON_FALSE_RETURN(bRet) if (!(bRet)) return -1

int ParseMds(unsigned char *buf, int dataSize, HMdsExt_t hMdsExt, int owm, const DmCfgFxp_t *pDmCfg)
{
  int dataSizeIn = dataSize;
  TrimSet_t *pTrimSet;
  Trim_t  *pTrim;
  unsigned int extLen;
  unsigned char extLvl;
  unsigned char n;
  int loopVar;

  hMdsExt->invalidateMds = 0; // it is valid one

  EXIT_ON_FALSE_RETURN( ReadU8(&buf, &dataSize, &hMdsExt->affected_dm_metadata_id) );
  EXIT_ON_FALSE_RETURN( ReadU8(&buf, &dataSize, &hMdsExt->scene_refresh_flag) );

  // yuv=>rgb
  hMdsExt->m33Yuv2RgbScale2P = 13;
  EXIT_ON_FALSE_RETURN(2*9 <= dataSize);
  READ_VECTOR(ReadI16, &buf, &dataSize, loopVar, 9, &hMdsExt->m33Yuv2Rgb[0][0]);
  EXIT_ON_FALSE_RETURN(4*3 <= dataSize);
  READ_VECTOR(ReadU32, &buf, &dataSize, loopVar, 3, &hMdsExt->v3Yuv2Rgb[0]);

  // rgb=>lms
  hMdsExt->m33Rgb2WpLmsScale2P = 14;
  EXIT_ON_FALSE_RETURN(2*9 <= dataSize);
  READ_VECTOR(ReadI16, &buf, &dataSize, loopVar, 9, &hMdsExt->m33Rgb2WpLms[0][0]);

  // EOTF gamma, a, b
  EXIT_ON_FALSE_RETURN( ReadU16(&buf, &dataSize, &hMdsExt->signal_eotf) );     // 1<<14
# if DM_FULL_SRC_SPEC
  if (owm) {
    if (pDmCfg->srcSigEnv.Eotf == CEotfPq) {
      hMdsExt->signal_eotf = signal_eotf_VALUE_PQ;
    }
    else {
     // if (pDmCfg->srcSigEnv.Gamma < 5) { // the float version.... MStar_Mod
      //  hMdsExt->signal_eotf = (unsigned short)(pDmCfg->srcSigEnv.Gamma*(1<<14) + 0.5);
     // }
    //  else { // the fixed point version
        hMdsExt->signal_eotf = (unsigned short)(pDmCfg->srcSigEnv.Gamma);
    //  }
    }

    EXIT_ON_FALSE_RETURN( WriteU16(&buf, &dataSize, hMdsExt->signal_eotf) );
  }
# else
  (void)owm;
# endif // DM_FULL_SRC_SPEC
  EXIT_ON_FALSE_RETURN( ReadU16(&buf, &dataSize, &hMdsExt->signal_eotf_param0) ); // 1<<2
  EXIT_ON_FALSE_RETURN( ReadU16(&buf, &dataSize, &hMdsExt->signal_eotf_param1) ); // 1<<16
  EXIT_ON_FALSE_RETURN( ReadU32(&buf, &dataSize, &hMdsExt->signal_eotf_param2) ); // 1<<18

  // signal info
  EXIT_ON_FALSE_RETURN( ReadU8(&buf, &dataSize, &hMdsExt->signal_bit_depth) );
  EXIT_ON_FALSE_RETURN( ReadU8(&buf, &dataSize, &hMdsExt->signal_color_space) );
  EXIT_ON_FALSE_RETURN( ReadU8(&buf, &dataSize, &hMdsExt->signal_chroma_format) );
  EXIT_ON_FALSE_RETURN( ReadU8(&buf, &dataSize, &hMdsExt->signal_full_range_flag) );
# if DM_FULL_SRC_SPEC
  if (owm) {
    hMdsExt->signal_bit_depth = (unsigned char)pDmCfg->srcSigEnv.Bdp;
    hMdsExt->signal_color_space = (unsigned char)pDmCfg->srcSigEnv.Clr;
    hMdsExt->signal_chroma_format = (unsigned char)pDmCfg->srcSigEnv.Chrm;
    if (pDmCfg->srcSigEnv.Rng == CRngFull) {
      hMdsExt->signal_full_range_flag = 1;
    }
    else {
      hMdsExt->signal_full_range_flag = 0;
    }

    EXIT_ON_FALSE_RETURN( WriteU8(&buf, &dataSize, hMdsExt->signal_bit_depth) );
    EXIT_ON_FALSE_RETURN( WriteU8(&buf, &dataSize, hMdsExt->signal_color_space) );
    EXIT_ON_FALSE_RETURN( WriteU8(&buf, &dataSize, hMdsExt->signal_chroma_format) );
    EXIT_ON_FALSE_RETURN( WriteU8(&buf, &dataSize, hMdsExt->signal_full_range_flag) );
  }
# endif //DM_FULL_SRC_SPEC

  // source minitor: all PQ scale 4095
  EXIT_ON_FALSE_RETURN( ReadU16(&buf, &dataSize, &hMdsExt->source_min_PQ) );
  EXIT_ON_FALSE_RETURN( ReadU16(&buf, &dataSize, &hMdsExt->source_max_PQ) );
  EXIT_ON_FALSE_RETURN( ReadU16(&buf, &dataSize, &hMdsExt->source_diagonal) );

  // init level 1 default value
  hMdsExt->min_PQ = hMdsExt->source_min_PQ;
  hMdsExt->max_PQ = hMdsExt->source_max_PQ;
  hMdsExt->mid_PQ = (hMdsExt->min_PQ + hMdsExt->max_PQ) >> 1;

  // level 2 and 3 handling: default no trim
  pTrimSet = &(hMdsExt->trimSets);
  pTrimSet->TrimSetNum = 0;
  pTrimSet->TrimSets[0].TrimNum = 0;
  pTrimSet->TrimSets[0].Trima[0] = hMdsExt->source_max_PQ;

# if EN_GLOBAL_DIMMING
  hMdsExt->lvl4GdAvail = 0;
# endif

  hMdsExt->lvl5AoiAvail = 0;
# if EN_AOI
  hMdsExt->active_area_left_offset   = (unsigned short)(pDmCfg->dmCtrl.AoiCol0);
  hMdsExt->active_area_right_offset  = (unsigned short)(pDmCfg->srcSigEnv.ColNum - pDmCfg->dmCtrl.AoiCol1Plus1);
  hMdsExt->active_area_top_offset    = (unsigned short)(pDmCfg->dmCtrl.AoiRow0);
  hMdsExt->active_area_bottom_offset = (unsigned short)(pDmCfg->srcSigEnv.RowNum - pDmCfg->dmCtrl.AoiRow1Plus1);
# endif

  hMdsExt->lvl255RunModeAvail = 0;
  hMdsExt->dm_run_mode = 0;
  hMdsExt->dm_run_version = 0;
  hMdsExt->dm_debug0 = 0;
  hMdsExt->dm_debug0 = 1;
  hMdsExt->dm_debug0 = 2;
  hMdsExt->dm_debug0 = 3;

  ////// parsing externsion block
  EXIT_ON_FALSE_RETURN( ReadU8(&buf, &dataSize, &hMdsExt->num_ext_blocks) );

  for (n = 0; n < hMdsExt->num_ext_blocks; ++n) {
    EXIT_ON_FALSE_RETURN( ReadU32(&buf, &dataSize, &extLen) );
    EXIT_ON_FALSE_RETURN( ReadU8(&buf, &dataSize, &extLvl) );

    if (extLvl == 1) {
      //// which is level 1 in new mds
      // the ccm
      EXIT_ON_FALSE_RETURN( ReadU16(&buf, &dataSize, &hMdsExt->min_PQ) );
      EXIT_ON_FALSE_RETURN( ReadU16(&buf, &dataSize, &hMdsExt->max_PQ) );
      EXIT_ON_FALSE_RETURN( ReadU16(&buf, &dataSize, &hMdsExt->mid_PQ) );

      extLen -= 3*2;
    }
    else if ((extLvl == 2 && !pDmCfg->tmCtrl.l2off)
             ) {
      pTrim = &pTrimSet->TrimSets[extLvl - 2]; // lvl = 2 in [0]
      if (pTrim->TrimNum < pTrim->TrimNumMax) {
        // only get the first TrimNumMax
        ++(pTrim->TrimNum);
        EXIT_ON_FALSE_RETURN(2*pTrim->TrimTypeDim <= dataSize);
        READ_VECTOR(ReadU16, &buf, &dataSize, loopVar, pTrim->TrimTypeDim, &(pTrim->Trima[pTrim->TrimNum*pTrim->TrimTypeDim]));
        if (pTrim->Trima[pTrim->TrimNum*pTrim->TrimTypeDim+TrimTypeMsWeight] == 0xffff) {
            pTrim->Trima[pTrim->TrimNum*pTrim->TrimTypeDim+TrimTypeMsWeight] = pTrim->Trima[TrimTypeMsWeight] * 2 - 1;
        }

        extLen -= pTrim->TrimTypeDim*2;
      }
      // ugly, but localized and works
      pTrimSet->TrimSetNum = (pTrimSet->TrimSets[0].TrimNum) ? 1 : 0;
    }
#   if EN_GLOBAL_DIMMING
    else if (extLvl == 4) {
      hMdsExt->lvl4GdAvail = 1;
      EXIT_ON_FALSE_RETURN( ReadU16(&buf, &dataSize, &hMdsExt->filtered_mean_PQ) );
      EXIT_ON_FALSE_RETURN( ReadU16(&buf, &dataSize, &hMdsExt->filtered_power_PQ) );
      extLen -= 2*2;
    }
#   endif
    else if (extLvl == 5) {
      hMdsExt->lvl5AoiAvail = 1;
#     if EN_AOI
      EXIT_ON_FALSE_RETURN( ReadU16(&buf, &dataSize, &hMdsExt->active_area_left_offset) );
      EXIT_ON_FALSE_RETURN( ReadU16(&buf, &dataSize, &hMdsExt->active_area_right_offset) );
      EXIT_ON_FALSE_RETURN( ReadU16(&buf, &dataSize, &hMdsExt->active_area_top_offset) );
      EXIT_ON_FALSE_RETURN( ReadU16(&buf, &dataSize, &hMdsExt->active_area_bottom_offset) );
      extLen -= 4*2;
#     endif
    }
    else if (extLvl == 255) {
      hMdsExt->lvl255RunModeAvail = 1;
      EXIT_ON_FALSE_RETURN( ReadU8(&buf, &dataSize, &hMdsExt->dm_run_mode) );
      EXIT_ON_FALSE_RETURN( ReadU8(&buf, &dataSize, &hMdsExt->dm_run_version) );
      EXIT_ON_FALSE_RETURN( ReadU8(&buf, &dataSize, &hMdsExt->dm_debug0) );
      EXIT_ON_FALSE_RETURN( ReadU8(&buf, &dataSize, &hMdsExt->dm_debug1) );
      EXIT_ON_FALSE_RETURN( ReadU8(&buf, &dataSize, &hMdsExt->dm_debug2) );
      EXIT_ON_FALSE_RETURN( ReadU8(&buf, &dataSize, &hMdsExt->dm_debug3) );
      extLen -= 6*1;
    }
    // else unknow, skip everything

    EXIT_ON_FALSE_RETURN( SkipByteAligned(&buf, &dataSize, extLen) );
  }

  return dataSizeIn - dataSize;
}

#if DM_VER_LOWER_THAN212
int ParseGmLutHdr(const unsigned char *cBuf, int hdrSize, HGmLut_t hGmLut)
{
  ////// read in char part
# define GET_I16_INC(var, buf) var = (short)( ((buf)[1]<<8) | (buf)[0] ); buf += 2;

  //// version
  int i;
  if (GM_LUT_VERSION_CHARS + GM_LUT_ENDIAN_CHARS + GM_LUT_TYPE_CHARS + 9*sizeof(short) != hdrSize) {
    //printf("invalide lut file: head size is not right.\n");
    return -1;
  }

  //// version
  hGmLut->Version = 0;
  for (i = 0; i < GM_LUT_VERSION_CHARS; ++i) {
    hGmLut->Version = 10*hGmLut->Version + *cBuf - '0';
    ++cBuf;
  }

  if (hGmLut->Version < 64) {
    //printf("invalide lut file: version not supported.\n");
    return -1;
  }

  //// endian
  hGmLut->LittleEndian = (*cBuf == 'L' || *cBuf =='l');
  cBuf += GM_LUT_ENDIAN_CHARS;
  if (hGmLut->LittleEndian == 0) {
    //printf("invalide lut file: only little endia gmlut is supported\n");
    return -1;
  }

  //// type
  if ((cBuf[0] != 'i' && cBuf[0] != 'I') ||
      (cBuf[1] != 'p' && cBuf[1] != 'P') ||
      (cBuf[2] != 't' && cBuf[2] != 'T') ||
      cBuf[3] != '2')
  {
    //printf("invalide lut file: invalide lut type\n");
    return -1;
  }
  cBuf += 4;
  if ( (cBuf[0] == 'r' || cBuf[0] == 'R') &&
       (cBuf[1] == 'g' || cBuf[1] == 'G') &&
       (cBuf[2] == 'b' || cBuf[2] == 'B') )
  {
    hGmLut->Type = GmLutTypeIpt2Rgb;
  }
  else if ( (cBuf[0] == 'y' || cBuf[0] == 'Y') &&
            (cBuf[1] == 'u' || cBuf[1] == 'U') &&
            (cBuf[2] == 'v' || cBuf[2] == 'V') )
  {
    hGmLut->Type = GmLutTypeIpt2Yuv;
  }
  else {
    //printf("invalide lut file: gm lut type no supported\n");
    return -1;
  }
  cBuf += -4 + GM_LUT_TYPE_CHARS;

  //// other 9 params
  GET_I16_INC(hGmLut->DimC1, cBuf);
  GET_I16_INC(hGmLut->DimC2, cBuf);
  GET_I16_INC(hGmLut->DimC3, cBuf);

  GET_I16_INC(hGmLut->IdxMinC1, cBuf);
  GET_I16_INC(hGmLut->IdxMaxC1, cBuf);
  GET_I16_INC(hGmLut->IdxMinC2, cBuf);
  GET_I16_INC(hGmLut->IdxMaxC2, cBuf);
  GET_I16_INC(hGmLut->IdxMinC3, cBuf);
  GET_I16_INC(hGmLut->IdxMaxC3, cBuf);

  return 0;
}

void ParseGmLutMap(const unsigned short *lut, unsigned short *lutMap, int enMask, HGmLut_t hGmLut)
{
  // just interleave output of the lut
  unsigned lutEntryNum = hGmLut->DimC1*hGmLut->DimC2*hGmLut->DimC3;
  const unsigned short *lutS1 = lut;
  const unsigned short *lutS2 = lutS1 + lutEntryNum;
  const unsigned short *lutS3 = lutS2 + lutEntryNum;
  const unsigned short *lutS1End = lutS2;
  unsigned short *lutT = lutMap;

  while (lutS1 < lutS1End) {
    if (enMask) {
 #    if USE_12BITS_IN_3D_LUT
    *lutT++ = (*lutS1++) >> 4;
    *lutT++ = (*lutS2++) >> 4;
    *lutT++ = (*lutS3++) >> 4;
# elif USE_12BITS_MSB_IN_3D_LUT
    *lutT++ = (*lutS1++) & 0xfff0;
    *lutT++ = (*lutS2++) & 0xfff0;
    *lutT++ = (*lutS3++) & 0xfff0;
# else
    *lutT++ = *lutS1++;
    *lutT++ = *lutS2++;
    *lutT++ = *lutS3++;
# endif
  }
    else {
      *lutT++ = *lutS1++;
      *lutT++ = *lutS2++;
      *lutT++ = *lutS3++;
    }
  }

  hGmLut->LutMap = lutMap;
}

#if EN_GLOBAL_DIMMING
void ParsePq2GLut(const int32_t *lut, HPq2GLut_t hPq2GLut)
{
  int32_t i, j;
  int32_t *p = (int32_t *)lut;

  for (i = 0; i < 4; i++) {
    hPq2GLut->eotfLutMaxPq[i] = *p++;
  }
  for (i = 0; i < 4; i++) {
    for (j = 0; j < EOTF_LUT_SIZE; j++) {
      hPq2GLut->eotfLuts[i][j] = *p++;
    }
  }
  for (i = 0; i < 3; i++) {
    for (j = 0; j < EOTF_LUT_ALPHA_SIZE; j++) {
      hPq2GLut->eotfLutAlpha[i][j] = *p++;
    }
  }
}

void GdSetGmLut(unsigned short *lutMap, HGmLut_t hGmLut, const HDmCfg_t hCfg, HDmFxp_t hDm)
{
  hGmLut->LutMap = lutMap;
  if (hCfg->tgtSigEnv.Clr == CClrYuv) {
    // assume the input type will be 2rgb or 2yuv
    hGmLut->Type = GmLutTypeIpt2Yuv;
  }
  hDm->gdCtrlDr.hGmLut = hGmLut;
}

HGmLut_t GdGetGmLut(HDmFxp_t hDm)
{
  return hDm->gdCtrlDr.hGmLut;
}
#endif

unsigned GetGmLutMapSize(HGmLut_t hGmLut)
{
  // idx [0, dim1) by [0, dimC2) by [0, dimC3), output 3 of unsigned short
  return (hGmLut) ? 3 * hGmLut->DimC1*hGmLut->DimC2*hGmLut->DimC3 : 0;
}

void *GetGmLutMap(HGmLut_t hGmLut)
{
  return hGmLut->LutMap;
}
#endif //DM_VER_LOWER_THAN212


unsigned GetCompactFrmMemLayout(int rowNum, int colNum,
  CClr_t clr, CChrm_t chrm, CWeav_t weav, CDtp_t dtp, int *pRowPitch, int *pRowPitchC)
{
  int colPitch = GET_PXL_COL_PITCH(clr, chrm, weav, dtp);
  int byteSize;
  unsigned FrmByteSize;

  ////// for interleaved or the first and fourth components of planer
  *pRowPitch = colNum*colPitch;
  byteSize = rowNum*(*pRowPitch);

  ////// for other components
  if (weav != CWeavPlnr) {
    *pRowPitchC = 0;
    FrmByteSize = byteSize;
  }
  else {
    // need to setup the second and third components in planar
    int byteSizeC;

    if (chrm != CChrm420) {
      // for 4:4:4
      *pRowPitchC = *pRowPitch;
      byteSizeC  = byteSize;
    }
    else {
      // for 4:2:0 half the size in row and col
      // to support continuous data flow
      *pRowPitchC =  (colNum>>1)*colPitch;
      byteSizeC  = (rowNum>>1)*(*pRowPitchC);
    }

    if (clr == CClrRgba) {
      // alpha have the same config as the first component
      FrmByteSize = (byteSize<<1) + (byteSizeC<<1);
    }
    else {
      FrmByteSize = byteSize + (byteSizeC<<1);
    }
  }

  return FrmByteSize;
}

unsigned GetMinFrmMemSize(int rowNum, int colNum, CClr_t clr, CChrm_t chrm, CWeav_t weav, CDtp_t dtp)
{
  ////// for interleaved or the first and fourth components of planer
  int byteSize = rowNum*colNum*GET_PXL_COL_PITCH(clr, chrm, weav, dtp);
  // the second and third components in planar
  int byteSizeC = (chrm == CChrm420) ? byteSize / 4 : byteSize;

  ////// for other components
  if (weav != CWeavPlnr) {
    return  byteSize;
  }

  if (clr == CClrRgba) {
    // alpha have the same config as the first component
    return  (byteSize<<1) + (byteSizeC<<1);
  }
  else {
    return  byteSize + (byteSizeC<<1);
  }
}

int SyncSigEnvWithMds(HMdsExt_t hMdsExt, HDmFxp_t hDm)
{
  //// TODO: use to flag to signal external frm buffer or internal one?
  // which define the scope of frame mem layout update

SrcSigEnvFxp_t *pSigEnv;

  if (!hDm) {
    return -1;
  }

  pSigEnv = &hDm->dmCfg.srcSigEnv;

  // in fact the the value matches
  // but for maximum potability we do not assign the value directly
  pSigEnv->Clr = CLR_M2C(hMdsExt->signal_color_space);

  pSigEnv->Chrm = CHRM_M2C(hMdsExt->signal_chroma_format);

  // for MDS case, no simple intl mode
  pSigEnv->Weav = WEAV_M2C(hMdsExt->signal_chroma_format);

  pSigEnv->Dtp = CDtpU16; // it is the case so far
  pSigEnv->Bdp = (CBdp_t)(hMdsExt->signal_bit_depth);

  pSigEnv->Rng = (hMdsExt->signal_full_range_flag == 1) ? CRngFull :
                 CRngNarrow;
  pSigEnv->Eotf = (hMdsExt->signal_eotf == signal_eotf_VALUE_PQ) ? CEotfPq : CEotfBt1886;

  return 0;
}

int UpdateSrcEnvFrameMemLayout(HDmFxp_t hDm)
{
  SrcSigEnvFxp_t *pSigEnv;

  if (!hDm) {
    return -1;
  }

  pSigEnv = &hDm->dmCfg.srcSigEnv;

  GetCompactFrmMemLayout(pSigEnv->RowNum, pSigEnv->ColNum,
    pSigEnv->Clr, pSigEnv->Chrm, pSigEnv->Weav, pSigEnv->Dtp, &pSigEnv->RowPitch, &pSigEnv->RowPitchC);

  return 0;
}


const SrcSigEnvFxp_t *GetSrcSigEnvPtr(HDmFxp_t hDm)
{
  if (!hDm) {
    return 0;
  }

  return &hDm->dmCfg.srcSigEnv;
}

const TgtSigEnvFxp_t *GetTgtSigEnvPtr(HDmFxp_t hDm)
{
  if (!hDm) {
    return 0;
  }

  return &hDm->dmCfg.tgtSigEnv;
}

HDmCfgFxp_t GetHDmCfg(HDmFxp_t hDm)
{
  if (!hDm) {
    return 0;
  }

  return &hDm->dmCfg;
}

#if EN_RUN_MODE
int GetRunMode(HDm_t hDm)
{
  return (hDm->mdsExt.lvl255RunModeAvail) ? hDm->mdsExt.dm_run_mode : hDm->dmCfg.dmCtrl.RunMode;
}
#endif

