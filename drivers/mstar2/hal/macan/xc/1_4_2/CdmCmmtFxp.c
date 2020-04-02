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
#include "dolbyTypeToMstarType.h"
#include "CdmMmg.h"
#include "VdrDmApi.h"
#include "KdmTypeFxp.h"
#include "CdmTypePriFxp.h"
#include "CdmUtilFxp.h"


#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

#ifndef MSTAR_DRIVER_MOD
#define MSTAR_DRIVER_MOD 1
#endif

#include "dm2_x/VdrDmAPIpFxp.h"
extern void DM2xLoad3DLut(struct DmKsFxp_t_ *pKs, struct DmKsFxpBack_t_ *pKsBack);
extern void DM2xAdaptor(struct DmKsFxp_t_ *pKs, struct DmKsFxpBack_t_ *pKsBack);

#include "tcgen/DolbyDisplayManagement.h"

#if REDUCED_COMPLEXITY
#include "KdmLutG2L.h"
#include "KdmG2L.h"
#endif


#if MSTAR_DRIVER_MOD
#include "dolby_vision_lut.h"
#endif

#include "KdmLutL2PQA.h"
#include "KdmLutL2PQB.h"
#include "KdmLutL2PQX.h"
#include "KdmLutPQ2L.h"

extern uint32_t PQToL(uint16_t u16_x0);

uint32_t PQ12ToL(uint16_t u16_x0)
{
    uint32_t y_1, y_2;
    int16_t iIdx;
    int16_t frac;

    u16_x0 = CLAMPS(u16_x0, 0, 4095);

    iIdx = u16_x0 >> 2; // 10 bits index
    frac = u16_x0 & 0x3;
    y_1 = depq1024Lut[iIdx];
    if (iIdx != (1<<10) - 1)
    {
        y_2 = depq1024Lut[iIdx + 1];
        y_1 += ((int64_t)(y_2 - y_1) * frac) >> 2; // interpolate using LSB
    }
    return y_1;
}

static int16_t FindNodeIdxL2PQS(uint32_t l)
{
    int16_t il = 0, num = DEF_L2PQ_LUT_NODES;// num = 2^n
    int16_t ic;
    // do a binary search
    while (num > 1)
    {
        num >>= 1;
        ic = il + num;
        if (l < l2pqLutX[ic]) {}
        else if (l > l2pqLutX[ic]) il = ic;
        else return ic;
    }

    return il;
}

uint16_t LToPQ12(uint32_t u32_x0)
{
    int16_t index;
    uint16_t r;

    index = FindNodeIdxL2PQS(u32_x0);

    r = (uint16_t)((((int64_t)(u32_x0 - l2pqLutX[index]) * l2pqLutA[index]) >> (DEF_L2PQ_LUT_X_SCALE2P + DEF_L2PQ_LUT_A_SCALE2P - DEF_L2PQ_LUT_B_SCALE2P)) + l2pqLutB[index]);
    return ((r + 0x8) >> 4);
}

uint16_t LToPQ15(uint32_t u32_x0)
{
    int16_t index;
    uint16_t r;

    index = FindNodeIdxL2PQS(u32_x0);

    r = (uint16_t)((((int64_t)(u32_x0 - l2pqLutX[index]) * l2pqLutA[index]) >> (DEF_L2PQ_LUT_X_SCALE2P + DEF_L2PQ_LUT_A_SCALE2P - DEF_L2PQ_LUT_B_SCALE2P)) + l2pqLutB[index]);
    return ((r + 0x8) >> 1);
}

////// kernel => control value xfer
#define CLR_K2C(k) (  ((k) == KClrYuv) ? CClrYuv :          \
                      ((k) == KClrRgb) ? CClrRgb :          \
                      ((k) == KClrRgba) ? CClrRgba : CClrIpt  )

#define CHRM_K2C(k) ( ((k) == KChrm420) ? CChrm420 :            \
                      ((k) == KChrm422) ? CChrm422 : CChrm444 )

#define DTP_K2C(k) (  ((k) == KDtpU16) ? CDtpU16 :            \
                      ((k) == KDtpU8)  ? CDtpU8  : CDtpF32 )

#define WEAV_K2C(k) ( ((k) == KWeavPlnr) ? CWeavPlnr :            \
                      ((k) == KWeavIntl) ? CWeavIntl : CWeavUyVy )


////// control => kernel value xfer
// kernel doesn't know Ipt I/O, metadata will be manipulated so that yuv will do the trick
#define CLR_C2K(c) (  ((c) == CClrYuv) ? KClrYuv :          \
                      ((c) == CClrRgb) ? KClrRgb :          \
                      ((c) == CClrRgba) ? KClrRgba : KClrYuv  )

#define CHRM_C2K(c) ( ((c) == CChrm420) ? KChrm420 :            \
                      ((c) == CChrm422) ? KChrm422 :  KChrm444 )

#define DTP_C2K(c) (  ((c) == CDtpU16) ? KDtpU16 :            \
                      ((c) == CDtpU8)  ? KDtpU8  :  KDtpF32 )

#define WEAV_C2K(c) ( ((c) == CWeavPlnr) ? KWeavPlnr :            \
                      ((c) == CWeavIntl) ? KWeavIntl :  KWeavUyVy )

// just in case of CEotfBtPower slip through
#define EOTF_C2K(c) (  ((c) == CEotfBt1886) ? KEotfBt1886 :             \
                       ((c) == CEotfPq   )  ? KEotfPq     : KEotfBt1886 )

#define LOC_C2K(c)  (  ((c) == CLocHost) ?  KLocHost : KLocDev  )

#if EN_SKIP_TMAP_MS
#define LUT_DIR_C2K(c)  (  ((c) == CTcLutDirFwd) ?  KTcLutDirFwd :            \
                           ((c) == CTcLutDirBwd) ?  KTcLutDirBwd : KTcLutDirPass)
#endif

# define CLR_M2K(m)   (KClr_t) (  ((m) == 1) ? KClrRgb : KClrYuv  )

# define CHRM_M2K(m)  (KChrm_t)(  ((m) == 0) ? KChrm420 :           \
                                  ((m) == 1) ? KChrm422 : KChrm444  )

#define WEAV_M2K(m)   (KWeav_t)(  ((m) == 0) ? KWeavPlnr :            \
                                  ((m) == 1) ? KWeavUyVy : KWeavPlnr  )

#if EN_GLOBAL_DIMMING
// production version Global Dimming behavior: always off if no mds presented
// to support possible application like testing GD without mds, define a macro
#define EN_LOCAL_GD_TEST  0
#if EN_LOCAL_GD_TEST
// so that GD can turn on and off without mds
#define LOCAL_GD_SETTING(lclGdSetting)   (lclGdSetting),
#else
// 0 for gdOn iff lclGdSetting && mds has level 4
#define LOCAL_GD_SETTING(lclGdSetting)   0,
#endif
#else
#define LOCAL_GD_SETTING(lclGdSetting)
#endif

// Matlab code always use ccm in mds as 'source' in TM regardless tcLutDir
#define FOLLOW_MATLAB_CCM_ALWAYS_AS_SOURCE_IN_TM   1

extern void Yuv2RgbOffsetRgbFxp(int16_t m33[3][3], uint32_t yOff, uint32_t uvOff, int32_t rgbOff[3]);

static int PxlColPitchFromK(KClr_t clr, KChrm_t chrm, KWeav_t weav, KDtp_t dtp)
{
    CClr_t  cClr  = CLR_K2C(clr);
    CChrm_t cChrm = CHRM_K2C(chrm);
    CWeav_t cWeav = WEAV_K2C(weav);
    CDtp_t  cDtp  = DTP_K2C(dtp);

    return GET_PXL_COL_PITCH(cClr, cChrm, cWeav, cDtp);
}

#if EN_MS_OPTION
// fram buffer for the 1st, 2nd and 3rd and 4th component
# define DEF_FRAME_BUFFER_COMP_NUM(imCompNum_, inDev_)  unsigned int imCompNum_ = 4
#elif EN_UP_DOWN_SAMPLE_OPTION
// fram buffer for the 1st, 2nd and 3rd component
# define DEF_FRAME_BUFFER_COMP_NUM(imCompNum_, inDev_)  unsigned int imCompNum_ = 3
#else
# define DEF_FRAME_BUFFER_COMP_NUM(imCompNum_, inDev_) // frmBufs not needed
#endif

// always in CPU: CUDA solution is just an enhancement,
// so trying to reduce the impact on CPU version, which demonstrate algorithm
static void RegTcLutMem(DmKsTMapFxp_t *pKsTMap, int sizeM1, HMmg_t hMmg)
{
    pKsTMap->tmLutISizeM1 = sizeM1;

}

HDmKsFxp_t InitDmKs(const HDmCfgFxp_t hDmCfg, HDmKsFxp_t hDmKs)
{
    HMmg_t hMmg;

    // frm buf
# if EN_UP_DOWN_SAMPLE_OPTION || EN_MS_OPTION || defined(_VDR_DM_CUDA_)
    const unsigned compSize = sizeof((*hDmKs->frmBuf0))*DM_MAX_IMG_SIZE;
    DEF_FRAME_BUFFER_COMP_NUM(compNum, hDmCfg->dmCtrl.Platform != CPlatformCpu);
# endif

    if (!hDmCfg || !hDmKs)
    {
        return 0;
    }

    hMmg = hDmCfg->hMmg;
    if (!hMmg)
    {
        return 0;
    }

    ////// register the memory require from kernel so control can alloc them
    // we can alloc memory according to the value in hDmCfg
    // to avoid memory re-allocation issue, we requestion maximum possible memory
    // use 16 aligned memory

    //// frame buffer
# if EN_UP_DOWN_SAMPLE_OPTION || EN_MS_OPTION || defined(_VDR_DM_CUDA_)
    hDmKs->rowPitchNum = DM_MAX_COL_NUM;

    assert(compNum == 3 || compNum == 4);
    RegMemReq(compSize*compNum, 16, hDmCfg->dmCtrl.Platform != CPlatformCpu, (void *)(&(hDmKs->frmBuf0)), MemAssignCbFunFc, hMmg);

# endif

    ////  TM lut buffer
    RegTcLutMem(&hDmKs->ksTMap,  TM1_LUT_MAX_SIZE - 1,  hMmg);


# if EN_MS_OPTION
    //// for MS
    // already allocated in heap, hook up
    hDmKs->ksMs.fltrLp[0] = &hDmKs->ksMs.fltrRowMem[0][0];
    hDmKs->ksMs.fltrLp[1] = &hDmKs->ksMs.fltrColMem[0][0];
# if !EN_EDGE_SIMPLE
    hDmKs->ksMs.fltrHp[0] = &hDmKs->ksMs.fltrRowMem[1][0];
    hDmKs->ksMs.fltrHp[1] = &hDmKs->ksMs.fltrColMem[1][0];
# endif

    if (hDmCfg->dmCtrl.Platform == CPlatformCpu)
    {

        // for lin - adm
        RegMemReq(compSize, 16, 0, (void *)(&(hDmKs->ksMs.linAdmBuf)),  MemAssignCbFunFc, hMmg);

#   if EN_AOI
        // to preserve adm content out side of AOI
        RegMemReq(compSize, 16, 0, (void *)(&(hDmKs->ksMs.tmpBuf)),  MemAssignCbFunFc, hMmg);
#   endif

        // transposed row pitch
        hDmKs->ksMs.tpRowPitchNum = DM_MAX_ROW_NUM;

# if !EN_EDGE_SIMPLE
        // for tp filtered result
        RegMemReq(compSize, 16, 0, (void *)(&(hDmKs->ksMs.tpRowBuf)),    MemAssignCbFunFc, hMmg);

        // edge
        RegMemReq(compSize, 16, 0, (void *)(&(hDmKs->ksMs.edgeBuf[0])),  MemAssignCbFunFc, hMmg);
        RegMemReq(compSize, 16, 0, (void *)(&(hDmKs->ksMs.edgeBuf[1])),  MemAssignCbFunFc, hMmg);
# else
        // two more row and column, for blured lin - adm
        RegMemReq(compSize + 2*(DM_MAX_ROW_NUM + DM_MAX_COL_NUM + 2)*sizeof(*hDmKs->frmBuf0) ,
                  16, 0, (void *)(&(hDmKs->ksMs.tpRowBuf)),    MemAssignCbFunFc, hMmg);
# endif
    }
# endif // EN_MS_OPTION

    return hDmKs;
}

HDmKsFxp_t GetHDmKs(HDmFxp_t hDm)
{
    return hDm->hDmKs;
}


# if !EN_IPT_PQ_ONLY_OPTION
static void GetRange(int inBits, CRng_t rng, CEotf_t eotf, uint16_t *rangeMin, uint16_t *range)
{
    if (rng == CRngNarrow)
    {
        *rangeMin = (unsigned short)( 16*(1<<(inBits - 8)) );
        *range =  (unsigned short)(235*(1<<(inBits - 8)) - (*rangeMin));
    }
    else if (rng == CRngSdi)
    {
        *rangeMin = (unsigned short)(1<<(inBits - 8));
        if (eotf == CEotfPq)
        {
            *range = (unsigned short)(1019*(1<< (inBits - 10)) - *rangeMin); // 1019*(1<< inBits)/1024
        }
        else
        {
            *range = (unsigned short)((1<< inBits) - 1 - 2 * (*rangeMin));
        }
    }
    else
    {
        *rangeMin = 0;
        *range = (unsigned short)((1<< inBits) - 1);
    }
}
#endif

static void GetIptNormParam(int bits, CRng_t rng, int32_t *v3Off, int32_t *scale)
{
    GetYuvRgbOffFxp(rng, bits, v3Off);
    if (rng == CRngNarrow)
    {
        *scale = (int32_t)(DLB_UINT_MAX(32)/((235-16)*(1<<(bits-8)))); // scale up 2^32
    }
    else
    {
        *scale = (int32_t)(DLB_UINT_MAX(32)/((1<<bits) - 1));
    }
}

# if DM_VER_HIGHER_THAN211
static void GetIptDnParam(int bits, CRng_t rng, int32_t *v3Off, int32_t *scale)
{
    GetYuvRgbOffFxp(rng, bits, v3Off);
    if (rng == CRngNarrow)
    {
        *scale = (int32_t)((235-16)*(1<<(bits-8)));
    }
    else
    {
        *scale = (int32_t)((1<<bits) - 1);
    }
}
#endif

static void SetTcSig(uint32_t min, uint32_t max, uint16_t diagSize, TcSigFxp_t *pTcSig)
{
    pTcSig->minPq = LToPQ12(min);
    pTcSig->maxPq = LToPQ12(max);
    pTcSig->diagSize = diagSize;
}


# if DM_FULL_SRC_SPEC
static void SrcSigEnv2TcCtrl(const SrcSigEnvFxp_t *pSrcSig, TcCtrlFxp_t *pTcCtrl)
{
    if ((pSrcSig->MinPq == 0) && (pSrcSig->MaxPq == 0))
    {
        //printf(" something is very wrong here \n");
        SetTcSig(pSrcSig->Min, pSrcSig->Max, pSrcSig->DiagSize, &pTcCtrl->tcSrcSig);
    }
    else   // use user's PQ value
    {
        pTcCtrl->tcSrcSig.minPq = pSrcSig->MinPq;
        pTcCtrl->tcSrcSig.maxPq = pSrcSig->MaxPq;
        pTcCtrl->tcSrcSig.diagSize = pSrcSig->DiagSize;
        //printf("pTcCtrl->tcSrcSig.minPq = %d\n",pTcCtrl->tcSrcSig.minPq);
        //printf(" pTcCtrl->tcSrcSig.maxPq = %d\n", pTcCtrl->tcSrcSig.maxPq);
    }
}
# endif

static void Tgt2TcCtrl(uint32_t tMin, uint32_t tMax, uint16_t diagSize, TcCtrlFxp_t *pTcCtrl)
{
    SetTcSig(tMin, tMax, diagSize, &pTcCtrl->tcTgtSig);
}

static void TgtSigEnv2TcCtrl(const TgtSigEnvFxp_t *pTgtSig, TcCtrlFxp_t *pTcCtrl)
{
    if ((pTgtSig->MinPq == 0) && (pTgtSig->MaxPq == 0))
    {
        Tgt2TcCtrl(pTgtSig->Min, pTgtSig->Max, pTgtSig->DiagSize, pTcCtrl);
    }
    else   // use user's PQ value
    {
        pTcCtrl->tcTgtSig.minPq = pTgtSig->MinPq;
        pTcCtrl->tcTgtSig.maxPq = pTgtSig->MaxPq;
        pTcCtrl->tcTgtSig.diagSize = pTgtSig->DiagSize;
    }
}

#if EN_GLOBAL_DIMMING
static void InitGdCtrlDr(const GdCtrlFxp_t *pGdCtrl, const TgtSigEnvFxp_t *pTgtSig, GdCtrlDrFxp_t *pGdCtrlDr)
{
    int32_t d1, d2;
# if DM_VER_LOWER_THAN212
    int32_t outBits = 15;
    GetRange(16,
             (pTgtSig->Clr != CClrRgb && pTgtSig->Clr != CClrRgba) ? CRngFull : pTgtSig->Rng,
             pTgtSig->Eotf,
             &pGdCtrlDr->otRngMin, &pGdCtrlDr->otRng);

    pGdCtrlDr->rangeOverOne = (pGdCtrlDr->otRng << (32 - outBits)) / DLB_UINT_MAX(16);

    if (pTgtSig->Clr == CClrYuv)
    {
#   if EN_KS_DM_IN
        if (pTgtSig->Rgb2YuvExt)
        {
            AssignM33I2I(pTgtSig->M33Rgb2Yuv, pGdCtrlDr->m33Rgb2Yuv);
            pGdCtrlDr->m33Rgb2YuvScale2P = pTgtSig->M33Rgb2YuvScale2P;
        }
        else
#   endif
            if (pTgtSig->Rng != CRngNarrow)
                pGdCtrlDr->m33Rgb2YuvScale2P = GetRgb2YuvM33Fxp(pTgtSig->YuvXferSpec, pGdCtrlDr->m33Rgb2Yuv);
            else
                pGdCtrlDr->m33Rgb2YuvScale2P = GetRgb2YuvM33NarrowFxp(pTgtSig->YuvXferSpec, outBits, pGdCtrlDr->m33Rgb2Yuv);

#   if EN_KS_DM_IN
        if (pTgtSig->Rgb2YuvOffExt)
            AssignV3I2I(pTgtSig->V3Rgb2YuvOff, pGdCtrlDr->v3Rgb2YuvOff);
        else
        {
#   endif
            int32_t offset[3];
            GetYuvRgbOffFxp(pTgtSig->Rng, 15, offset);
            pGdCtrlDr->v3Rgb2YuvOff[0] = (uint32_t)offset[0];
            pGdCtrlDr->v3Rgb2YuvOff[1] = (uint32_t)offset[1];
            pGdCtrlDr->v3Rgb2YuvOff[2] = (uint32_t)offset[2];
#   if EN_KS_DM_IN
        }
#   endif
    }
# else
    (void)pTgtSig;
#endif

    // cache the value
    d1 = LToPQ12(pTgtSig->Min);
    d2 = LToPQ12(pTgtSig->Max);
    pGdCtrlDr->fltrdMeanPqDft = ((d1 + d2 + 1) >> 1);  // the mean
#ifdef MSTAR_DRIVER_MOD
    pGdCtrlDr->fltrdStdPqDft  = ((((int64_t)(d2 - d1) * (1 << 24)) / (int32_t)(0x16A1)) + (1 << (12 - 1))) >> 12;  // unbiased std
#else
    pGdCtrlDr->fltrdStdPqDft  = ((((int64_t)(d2 - d1) * (1 << 24)) / (int32_t)(1.41421356 * (1 << 12))) + (1 << (12 - 1))) >> 12;  // unbiased std
#endif
    pGdCtrlDr->gdWeightMean = pGdCtrl->GdWeightMean;
    pGdCtrlDr->gdWeightStd  = pGdCtrl->GdWeightStd;

    pGdCtrlDr->gdWDynRngSqrt = pGdCtrl->GdWDynRngSqrt; // pow(10, pGdCtrl->gdWDynRngLog10/2);
    pGdCtrlDr->fltrdMeanUpBound = ((int64_t)pGdCtrl->GdWMax << 18) / pGdCtrlDr->gdWDynRngSqrt;
    d1 = (((int64_t)pGdCtrl->GdWMin * pGdCtrlDr->gdWDynRngSqrt) + ((uint32_t)1 << (18 - 1))) >> 18;
    d2 = ((int64_t)pGdCtrl->GdWMm << 18) / pGdCtrlDr->gdWDynRngSqrt;
    pGdCtrlDr->fltrdMeanLowBound = MAX2S(d1, d2);
    pGdCtrlDr->gdActiveLstFrm = 0;  // no gd for last frame
    pGdCtrlDr->gdUdPerFrmsTh = pGdCtrl->GdUdPerFrmsTh;
    pGdCtrlDr->gdUdFrms = 0;
    pGdCtrlDr->gdUdDltAnchTh = ((int64_t)pGdCtrl->GdUdDltTMaxTh << 18) / pGdCtrlDr->gdWDynRngSqrt;
}

// return 0 = if fltrdAnchorUsed will be the same
static int32_t GdDeriveEffTmm(uint16_t fltrdAnchorPq, GdCtrlDrFxp_t *pGdCtrlDr)
{
    uint32_t fltrdAnchor = PQ12ToL(fltrdAnchorPq);

    fltrdAnchor = CLAMPS(fltrdAnchor, pGdCtrlDr->fltrdMeanLowBound, pGdCtrlDr->fltrdMeanUpBound);

    if (!pGdCtrlDr->gdActiveLstFrm ||
        (((int64_t)pGdCtrlDr->fltrdAnchorUsed - fltrdAnchor >  (int64_t)pGdCtrlDr->gdUdDltAnchTh ||
          (int64_t)pGdCtrlDr->fltrdAnchorUsed - fltrdAnchor < -(int64_t)pGdCtrlDr->gdUdDltAnchTh) &&
         pGdCtrlDr->gdUdFrms >= pGdCtrlDr->gdUdPerFrmsTh))
    {
        pGdCtrlDr->tMinEff = ((int64_t)fltrdAnchor << 18) / pGdCtrlDr->gdWDynRngSqrt;
        pGdCtrlDr->tMaxEff = (((int64_t)fltrdAnchor * pGdCtrlDr->gdWDynRngSqrt) + ((uint32_t)1 << (18 - 1))) >> 18;

        pGdCtrlDr->fltrdAnchorUsed = fltrdAnchor;
        pGdCtrlDr->gdUdFrms = 0;
        return 1;
    }

    return 0;
}

// global diming to target
static void Gd2TcCtrl(const GdCtrlDrFxp_t *pGdCtrlDr, int diagSize, TcCtrlFxp_t *pTcCtrl)
{
    Tgt2TcCtrl(pGdCtrlDr->tMinEff, pGdCtrlDr->tMaxEff, diagSize, pTcCtrl);
}
#endif

static void TmCtrl2TcCtrl(const TmCtrlFxp_t *pTmCtrl, TcCtrlFxp_t *pTcCtrl, int32_t gSrc)
{
    // invertible only possible for graphic channel and EN_INVERTIBLE_TCM_STB
    pTcCtrl->bwdInvertible = gSrc && pTmCtrl->BwdInvertible && (pTcCtrl->tcLutDir == CTcLutDirBwd);
    pTcCtrl->rolloff = pTmCtrl->Rolloff;
    pTcCtrl->rolloffInv =  (((uint32_t)1<<QB02DATAIN)) / pTmCtrl->Rolloff;
# if DM_VER_HIGHER_THAN211
    pTcCtrl->keyWeight = pTmCtrl->KeyWeight;
    pTcCtrl->intensityVectorWeight = pTmCtrl->IntensityVectorWeight;
    pTcCtrl->chrmVectorWeight = pTmCtrl->ChrmVectorWeight;
    pTcCtrl->bpWeight = pTmCtrl->BpWeight;

    if (pTcCtrl->tcLutDir == CTcLutDirBwd && !pTcCtrl->bwdInvertible)
    {
        //pTcCtrl->keyWeight = (4096 * 4096)/pTmCtrl->KeyWeight; // KeyWeight scale in 12
        pTcCtrl->intensityVectorWeight = pTmCtrl->IntensityVectorWeightBwd;
        pTcCtrl->chrmVectorWeight = pTmCtrl->ChrmVectorWeightBwd;
    }

    pTcCtrl->chrmVectorWeight = pTmCtrl->ChrmVectorWeight;
# endif

    pTcCtrl->tMinBias =  pTmCtrl->TMinBias;
    pTcCtrl->tMaxBias =  pTmCtrl->TMaxBias;
    pTcCtrl->tMidBias =  pTmCtrl->TMidBias;

    pTcCtrl->dContrast =  pTmCtrl->DContrast;
    pTcCtrl->dBrightness =  pTmCtrl->DBrightness;

    if (pTcCtrl->tcLutDir == CTcLutDirBwd && !pTcCtrl->bwdInvertible)
    {
        //pTcCtrl->tMinBias =  -pTmCtrl->TMinBias;
        //pTcCtrl->tMaxBias =  -pTmCtrl->TMaxBias;
        pTcCtrl->tMidBias =  -pTmCtrl->TMidBias;

        //pTcCtrl->dContrast =  -pTmCtrl->DContrast;
        //pTcCtrl->dBrightness =  -pTmCtrl->DBrightness;
    }

    (void)gSrc;
}

//#if DM_VER_CTRL == 2
static void ClampLevel2(int32_t *level2)
{
    level2[TrimTypeSlope]   =       CLAMPS(level2[TrimTypeSlope],  -2048, 2047);        // -0.5, 0,5
    level2[TrimTypeOffset]  =       CLAMPS(level2[TrimTypeOffset], -2048, 2047);        // -0.5, 0.5
    level2[TrimTypePower]   =       CLAMPS(level2[TrimTypePower],  -2048, 2047);        // -0.5, 0.5
    level2[TrimTypeChromaWeight]  = CLAMPS(level2[TrimTypeChromaWeight], 0, 65535);      // 0, 1 (scale 16)
    level2[TrimTypeSatGain]       = CLAMPS(level2[TrimTypeSatGain], 2048, 4096 + 2047); // 0.5, 1.5
    level2[TrimTypeMsWeight]      = CLAMPS(level2[TrimTypeMsWeight],  0, 8191);         // 0, 2
}
//#endif

# if DM_FULL_SRC_SPEC || DM_SEC_INPUT
static void UpdateTcCtrlLevel(const TmCtrlFxp_t *pTmCtrl, TcCtrlFxp_t *pTcCtrl)
{
    int tt;

    /* level 2 */
    for (tt = 1; tt < TRIM_TYPE_DIM2; ++tt)
    {
        pTcCtrl->level2[tt]= pTmCtrl->Default2[tt] + pTmCtrl->ValueAdj2[tt];
    }
    if (pTcCtrl->tcLutDir == CTcLutDirBwd && !pTcCtrl->bwdInvertible)
    {
#   if  DM_VER_LOWER_THAN212
        pTcCtrl->level2[TrimTypeChromaWeight]= (pTmCtrl->Default2[TrimTypeChromaWeight] + pTmCtrl->ValueAdj2ChromaWeightBwd) << 4; // scale 16 in kernel
#   else
        pTcCtrl->level2[TrimTypeSatGain]= pTmCtrl->Default2[TrimTypeSatGain] + pTmCtrl->ValueAdj2SatGainBwd;
#   endif
    }

//# if DM_VER_CTRL == 2
    ClampLevel2(pTcCtrl->level2);
//# endif


    pTcCtrl->level2[TrimTypeMsWeight] *= 2; // scale 11 as in metadata

}
#endif

static int32_t CreateToneCurve(TcCtrlFxp_t *pTcCtrl, DmKsTMapFxp_t *pKsTMap, void *pKsTMap2, TcCtrlFxp_t *pTcCtrlUsed)
{

# if DM_VER_LOWER_THAN212
    val_norm_pair_t cParam1, cParam2, cParam3;
    displayPqTuning_t_ deviceTuning;
    srcMetadata_t_ srcMetadata;
    int32_t slopeOverRollOff;

    if (MemEqByte(pTcCtrlUsed, pTcCtrl, (int)((char *)&pTcCtrl->tCrush - (char *)&pTcCtrl->tcSrcSig)))
    {
        //printf("TMO didn't change, run it anyway\n");
        return 0;  // TC not changed
    }

    deviceTuning.tMinPQ             = pTcCtrl->tcTgtSig.minPq << (QB02DATAIN-12);
    deviceTuning.tMaxPQ             = pTcCtrl->tcTgtSig.maxPq << (QB02DATAIN-12);
    deviceTuning.tMinPQBias         = pTcCtrl->tMinBias << (QB02DATAIN-12);
    deviceTuning.tMidPQBias         = pTcCtrl->tMidBias << (QB02DATAIN-12);
    deviceTuning.tMaxPQBias         = pTcCtrl->tMaxBias << (QB02DATAIN-12);
    deviceTuning.tContrast          = pTcCtrl->dContrast << (QB02DATAIN-12);
    deviceTuning.tBrightness        = pTcCtrl->dBrightness << (QB02DATAIN-12);
    deviceTuning.tDiagonalInches    = pTcCtrl->tcTgtSig.diagSize;
    deviceTuning.tRolloff           = pTcCtrl->rolloff;
    deviceTuning.tRolloffInv        = pTcCtrl->rolloffInv;
    deviceTuning.tMode              = 0;
    if (pTcCtrl->tcLutDir == CTcLutDirPass)
    {
        deviceTuning.tMode            = DM_PASSTHROUGH_TC;
        srcMetadata.Crush             = pTcCtrl->tcSrcSig.crush << (QB02DATAIN-12);
        srcMetadata.Mid               = pTcCtrl->tcSrcSig.mid << (QB02DATAIN-12);
        srcMetadata.Clip              = pTcCtrl->tcSrcSig.clip << (QB02DATAIN-12);
        pKsTMap->chromaWeight         = 0;
    }
    else if (pTcCtrl->tcLutDir == CTcLutDirBwd)
    {
        deviceTuning.tMode            = DM_INVERSE_TC;
        srcMetadata.Crush             = pTcCtrl->tcTgtSig.crush << (QB02DATAIN-12);
        srcMetadata.Mid               = pTcCtrl->tcTgtSig.mid << (QB02DATAIN-12);
        srcMetadata.Clip              = pTcCtrl->tcTgtSig.clip << (QB02DATAIN-12);
    }
    else
    {
        deviceTuning.tMode            = DM_NORMAL_TC;
        srcMetadata.Crush               = pTcCtrl->tcSrcSig.crush << (QB02DATAIN-12);
        srcMetadata.Mid                 = pTcCtrl->tcSrcSig.mid << (QB02DATAIN-12);
        srcMetadata.Clip                = pTcCtrl->tcSrcSig.clip << (QB02DATAIN-12);
    }

    srcMetadata.SminPQ              = pTcCtrl->tcSrcSig.minPq << (QB02DATAIN-12);
    srcMetadata.SmaxPQ              = pTcCtrl->tcSrcSig.maxPq << (QB02DATAIN-12);
    srcMetadata.sTrimSlope          = pTcCtrl->level2[TrimTypeSlope] << (QTRIMS-12);
    srcMetadata.sTrimOffset         = pTcCtrl->level2[TrimTypeOffset] << (QTRIMS-12);
    srcMetadata.sTrimPower          = pTcCtrl->level2[TrimTypePower] << (QTRIMS-12);
    srcMetadata.Sdiagonalinches     = pTcCtrl->tcSrcSig.diagSize;

# if REDUCED_TC_LUT
    CreateToneCurve2pxx(
        &srcMetadata,
        &deviceTuning,
        &cParam1,&cParam2,&cParam3,&slopeOverRollOff,
        (int16_t *)pKsTMap->tmInternal515Lut, 0);
# else
    CreateToneCurve2pxx(
        &srcMetadata,
        &deviceTuning,
        &cParam1,&cParam2,&cParam3,&slopeOverRollOff,
        (int16_t *)pKsTMap->tmInternal515Lut, (int16_t *)pKsTMap->tmLutI);
# endif

# else
    val_norm_pair_t cParam1, cParam2, cParam3;
    displayPqTuning_t_ deviceTuning;
    srcMetadata_t_ srcMetadata;
    int32_t slopeOverRollOff;
    int16_t clampOut;

    if (MemEqByte(pTcCtrlUsed, pTcCtrl, (int)((char *)&pTcCtrl->tCrush - (char *)&pTcCtrl->tcSrcSig)))
    {
        //printf("TMO didn't change, run it anyway\n");
        return 0;  // TC not changed
    }

# if (DM_VER_CTRL == 2) && DM_VER_HIGHER_THAN211
    pTcCtrl->tmLutISizeM1 = pTcCtrl->tmLutSSizeM1 = pTcCtrl->smLutISizeM1 = pTcCtrl->smLutSSizeM1 = pKsTMap->tmLutISizeM1;
# endif

# if EN_SKIP_TMAP_MS
    pKsTMap->tcLutDir = LUT_DIR_C2K(pTcCtrl->tcLutDir);
# endif

    deviceTuning.tMinPQ             = pTcCtrl->tcTgtSig.minPq << (QB02DATAIN-12);
    deviceTuning.tMaxPQ             = pTcCtrl->tcTgtSig.maxPq << (QB02DATAIN-12);
    deviceTuning.tMinPQBias         = pTcCtrl->tMinBias << (QB02DATAIN-12);
    deviceTuning.tMidPQBias         = pTcCtrl->tMidBias << (QB02DATAIN-12);
    deviceTuning.tMaxPQBias         = pTcCtrl->tMaxBias << (QB02DATAIN-12);
    deviceTuning.tContrast          = pTcCtrl->dContrast << (QB02DATAIN-12);
    deviceTuning.tBrightness        = pTcCtrl->dBrightness << (QB02DATAIN-12);
    deviceTuning.tDiagonalInches    = pTcCtrl->tcTgtSig.diagSize;
    deviceTuning.tBrightPreserve    = pTcCtrl->bpWeight;
    deviceTuning.tRolloff           = pTcCtrl->rolloff;
    deviceTuning.tRolloffInv        = pTcCtrl->rolloffInv;
    if (pTcCtrl->tcLutDir == CTcLutDirPass)
    {
        deviceTuning.tMode            = DM_PASSTHROUGH_TC;
        srcMetadata.Crush             = pTcCtrl->tcSrcSig.crush << (QB02DATAIN-12);
        srcMetadata.Mid               = pTcCtrl->tcSrcSig.mid << (QB02DATAIN-12);
        srcMetadata.Clip              = pTcCtrl->tcSrcSig.clip << (QB02DATAIN-12);
        clampOut = 0;  // keep it identical
    }
    else if (pTcCtrl->tcLutDir == CTcLutDirBwd)
    {
        deviceTuning.tMode            = DM_INVERSE_TC;
        srcMetadata.Crush             = pTcCtrl->tcTgtSig.crush << (QB02DATAIN-12);
        srcMetadata.Mid               = pTcCtrl->tcTgtSig.mid << (QB02DATAIN-12);
        srcMetadata.Clip              = pTcCtrl->tcTgtSig.clip << (QB02DATAIN-12);
        clampOut = 1;
    }
    else
    {
        deviceTuning.tMode            = DM_NORMAL_TC;
        srcMetadata.Crush             = pTcCtrl->tcSrcSig.crush << (QB02DATAIN-12);
        srcMetadata.Mid               = pTcCtrl->tcSrcSig.mid << (QB02DATAIN-12);
        srcMetadata.Clip              = pTcCtrl->tcSrcSig.clip << (QB02DATAIN-12);
        clampOut = 1;
    }

    srcMetadata.KeyWeight           = pTcCtrl->keyWeight << (QLVL3MD-12);
    srcMetadata.SminPQ              = pTcCtrl->tcSrcSig.minPq << (QB02DATAIN-12);
    srcMetadata.SmaxPQ              = pTcCtrl->tcSrcSig.maxPq << (QB02DATAIN-12);
    srcMetadata.sTrimSlope          = pTcCtrl->level2[TrimTypeSlope] << (QTRIMS-12);
    srcMetadata.sTrimOffset         = pTcCtrl->level2[TrimTypeOffset] << (QTRIMS-12);
    srcMetadata.sTrimPower          = pTcCtrl->level2[TrimTypePower] << (QTRIMS-12);
    srcMetadata.ChromaWeight        = 0 << (QTRIMS-12);
    srcMetadata.SaturaGain          = pTcCtrl->level2[TrimTypeSatGain] << (QTRIMS-12);
    srcMetadata.Sdiagonalinches     = pTcCtrl->tcSrcSig.diagSize;
    srcMetadata.IntensityVWeight    = pTcCtrl->intensityVectorWeight << (QLVL3MD-12);
    srcMetadata.ChromaVWeight       = pTcCtrl->chrmVectorWeight << (QLVL3MD-12);

# if (DM_VER_CTRL == 2) && DM_VER_HIGHER_THAN211
# if REDUCED_TC_LUT
    CreateToneCurve3pxx(
        &srcMetadata,
        &deviceTuning,
        &cParam1,&cParam2,&cParam3,&slopeOverRollOff,
        pTcCtrl->tmLutI,
        pTcCtrl->tmLutS,
        pTcCtrl->smLutI,
        pTcCtrl->smLutS,
        &pTcCtrl->ratio,
        512, clampOut);

    MemCpyByte(&pKsTMap->tmInternal515Lut[1], pTcCtrl->tmLutI, sizeof(int16_t) * 512);
    pKsTMap->tmInternal515Lut[0] = pKsTMap->tmInternal515Lut[1];
    pKsTMap->tmInternal515Lut[513] = pKsTMap->tmInternal515Lut[512];
    pKsTMap->tmInternal515Lut[514] = pKsTMap->tmInternal515Lut[512];

# else
    CreateToneCurve3pxx(
        &srcMetadata,
        &deviceTuning,
        &cParam1,&cParam2,&cParam3,&slopeOverRollOff,
        &pTcCtrl->tmLutI[1],
        pTcCtrl->tmLutS,
        pTcCtrl->smLutI,
        pTcCtrl->smLutS,
        &pTcCtrl->ratio,
        512, clampOut);

    pTcCtrl->tmLutI[0] = pTcCtrl->tmLutI[1];
    pTcCtrl->tmLutI[513] = pTcCtrl->tmLutI[512];
    pTcCtrl->tmLutI[514] = pTcCtrl->tmLutI[512];

    {
        int32_t lratio;
        int16_t lutMaxValue, i;
        int16_t *toneCurveLUT = pTcCtrl->tmLutI;
        uint16_t *toneCurve4KLUT = pKsTMap->tmLutI;

        // intfilt(8,2,0.1)
#define INT_FILT_Q  16
#define INT8_FILT_C1  60523
#define INT8_FILT_C2  53827
#define INT8_FILT_C3  45833
#define INT8_FILT_C4  36929
#define INT8_FILT_C5  27505
#define INT8_FILT_C6  17949
#define INT8_FILT_C7  8651
#define INT8_FILT_C8  0
#define INT8_FILT_C9  -2272
#define INT8_FILT_C10 -3638
#define INT8_FILT_C11 -4225
#define INT8_FILT_C12 -4161
#define INT8_FILT_C13 -3577
#define INT8_FILT_C14 -2602
#define INT8_FILT_C15 -1366

        lutMaxValue = 0;
        for (i = 0; i < 515; i++)
        {
            if (pTcCtrl->tmLutI[i] > lutMaxValue)
                lutMaxValue = pTcCtrl->tmLutI[i];
        }

        for (i = 0; i < 512; i++)
        {
            //X0 = P0
            *toneCurve4KLUT++ = toneCurveLUT[1];

            //X1 =  C9*P0 + C1*P1 + C7*P2 + C15*P3
            lratio  = INT8_FILT_C9 * pTcCtrl->tmLutI[0];
            lratio += INT8_FILT_C1 * toneCurveLUT[1];
            lratio += INT8_FILT_C7 * toneCurveLUT[2];
            lratio += INT8_FILT_C15 * toneCurveLUT[3];
            lratio += (unsigned)0x1 << (INT_FILT_Q-1);
            *toneCurve4KLUT++ = CLAMPS(lratio >> (INT_FILT_Q),toneCurveLUT[1],lutMaxValue);

            //X2 =  C10*P0 + C2*P1 + C6*P2 + C14*P3
            lratio  = INT8_FILT_C10 * toneCurveLUT[0];
            lratio += INT8_FILT_C2 * toneCurveLUT[1];
            lratio += INT8_FILT_C6 * toneCurveLUT[2];
            lratio += INT8_FILT_C14 * toneCurveLUT[3];
            lratio += (unsigned)0x1 << (INT_FILT_Q-1);
            *toneCurve4KLUT++ = CLAMPS(lratio >> (INT_FILT_Q),toneCurveLUT[1],lutMaxValue);

            //X3 =  C11*P0 + C3*P1 + C5*P2 + C13*P3
            lratio  = INT8_FILT_C11 * toneCurveLUT[0];
            lratio += INT8_FILT_C3 * toneCurveLUT[1];
            lratio += INT8_FILT_C5 * toneCurveLUT[2];
            lratio += INT8_FILT_C13 * toneCurveLUT[3];
            lratio += (unsigned)0x1 << (INT_FILT_Q-1);
            *toneCurve4KLUT++ = CLAMPS(lratio >> (INT_FILT_Q),toneCurveLUT[1],lutMaxValue);


            //X4 =  C12 * (P0 + P3) + C4*(P1 + P2)
            lratio  = INT8_FILT_C12 * (toneCurveLUT[0] + toneCurveLUT[3]);
            lratio += INT8_FILT_C4 * (toneCurveLUT[1] + toneCurveLUT[2]);
            lratio += (unsigned)0x1 << (INT_FILT_Q-1);
            *toneCurve4KLUT++ = CLAMPS(lratio >> (INT_FILT_Q),toneCurveLUT[1],lutMaxValue);

            //X5 =  C11*P3 + C3*P2 + C5*P1 + C13*P0
            lratio  = INT8_FILT_C11 * toneCurveLUT[3];
            lratio += INT8_FILT_C3 * toneCurveLUT[2];
            lratio += INT8_FILT_C5 * toneCurveLUT[1];
            lratio += INT8_FILT_C13 * toneCurveLUT[0];
            lratio += (unsigned)0x1 << (INT_FILT_Q-1);
            *toneCurve4KLUT++ = CLAMPS(lratio >> (INT_FILT_Q),toneCurveLUT[1],lutMaxValue);


            //X6 =  C10*P3 + C2*P2 + C6*P1 + C14*P0
            lratio  = INT8_FILT_C10 * toneCurveLUT[3];
            lratio += INT8_FILT_C2 * toneCurveLUT[2];
            lratio += INT8_FILT_C6 * toneCurveLUT[1];
            lratio += INT8_FILT_C14 * toneCurveLUT[0];
            lratio += (unsigned)0x1 << (INT_FILT_Q-1);
            *toneCurve4KLUT++ = CLAMPS(lratio >> (INT_FILT_Q),toneCurveLUT[1],lutMaxValue);


            //X7 =  C9*P3 + C1*P2 + C7*P1 + C15*P0
            lratio  = INT8_FILT_C9 * toneCurveLUT[3];
            lratio += INT8_FILT_C1 * toneCurveLUT[2];
            lratio += INT8_FILT_C7 * toneCurveLUT[1];
            lratio += INT8_FILT_C15 * toneCurveLUT[0];
            lratio += (unsigned)0x1 << (INT_FILT_Q-1);
            *toneCurve4KLUT++ = CLAMPS(lratio >> (INT_FILT_Q),toneCurveLUT[1],lutMaxValue);

            toneCurveLUT++;
        }
    }
# endif
# else
    CreateToneCurve3pxx(
        &srcMetadata,
        &deviceTuning,
        &cParam1,&cParam2,&cParam3,&slopeOverRollOff,
        pKsTMap->tmLutI,
        pKsTMap->tmLutS,
        pKsTMap->smLutI,
        pKsTMap->smLutS,
        0,
        512, clampOut);
# endif
    /*
      {
      int i,r;
      BuildITCLut(pKsTMap->tmLutI, lutX, lutA, lutB);

      for (i=0; i<512; i++) {
        r = ITCLut(pKsTMap->tmLutI[i], lutX, lutA, lutB);
        printf("%d:%d\n", i<<3, r);
      }
      }
    */
# endif // # if (DM_VER_CTRL == 2)

    MemCpyByte(pTcCtrlUsed, pTcCtrl, (int)((char *)&pTcCtrl->tCrush - (char *)&pTcCtrl->tcSrcSig));

    return 1;
}

// build up.down sample exec
# if EN_UP_DOWN_SAMPLE_OPTION
#ifdef _USE_MPEG2_CHROMA_FILTER_
#define FilterUvRowUsHalfSize   3
static const int16_t FilterUvRowUsScale2p = 8;
static const int16_t FilterUvRowUs0_m[FilterUvRowUsHalfSize<<1] =
{
    3, -16, 67, 227, -32, 7
};
static const int16_t FilterUvRowUs1_m[FilterUvRowUsHalfSize<<1] =
{
    7, -32, 227, 67, -16, 3
};
#elif defined _USE_2_TAP_CHROMA_VERT_FILTER_
#define FilterUvRowUsHalfSize   1
static const int16_t FilterUvRowUsScale2p = 8;
static const int16_t FilterUvRowUs0_m[FilterUvRowUsHalfSize<<1] =
{
    64, 192
};
static const int16_t FilterUvRowUs1_m[FilterUvRowUsHalfSize<<1] =
{
    192, 64
};
#elif defined _USE_4_TAP_CHROMA_VERT_FILTER_
#define FilterUvRowUsHalfSize   2
static const int16_t FilterUvRowUsScale2p = 6;
static const int16_t FilterUvRowUs0_m[FilterUvRowUsHalfSize<<1] =
{
    -2, 16, 54, -4
};
static const int FilterUvRowUs1_m[FilterUvRowUsHalfSize<<1] =
{
    -4, 54, 16, -2
};
#else
#define FilterUvRowUsHalfSize   3
static const int16_t FilterUvRowUsScale2p = 8;
static const int16_t FilterUvRowUs0_m[FilterUvRowUsHalfSize<<1] =
{
    2, -12, 65, 222, -25, 4
};
static const int16_t FilterUvRowUs1_m[FilterUvRowUsHalfSize<<1] =
{
    4, -25, 222, 65, -12, 2
};
#endif

#ifdef _USE_MPEG2_CHROMA_FILTER_
#define FilterUvColUsHalfSize   3
static const int FilterUvColUsScale2p = 8;
static const int FilterUvColUs_m[FilterUvColUsHalfSize<<1] =
{
    21, -52, 159, 159, -52, 21
};
#elif defined _USE_4_TAP_CHROMA_HORI_FILTER_
#define FilterUvColUsHalfSize   2
static const int FilterUvColUsScale2p = 6;
static const int FilterUvColUs_m[FilterUvColUsHalfSize<<1] =
{
    -4, 36, 36, -4
};
#else
#define FilterUvColUsHalfSize   4
static const int FilterUvColUsScale2p = 12;
static const int16_t FilterUvColUs_m[FilterUvColUsHalfSize<<1] =
{
    22, 94, -524, 2456, 2456, -524, 94, 22
};
#endif

#ifdef _USE_MPEG2_CHROMA_FILTER_
//// UV col half
#define FilterUvColDsRadius      5 // total is 2*FilterUvColDsRadius + 1
static const int FilterUvColDsScale2p = 9;
static const int FilterUvColDs_m[(FilterUvColDsRadius<<1) + 1] =
{
    22, 0, -52, 0, 159, 256, 159, 0, -52, 0, 22
};
#else
#define FilterUvColDsRadius     7 // total is 2*FilterUvColDsRadius + 1
static const int FilterUvColDsScale2p = 12;
static const int16_t FilterUvColDs_m[(FilterUvColDsRadius<<1) + 1] =
{
    11, 0, 47, 0, -262, 0, 1228, 2048, 1228, 0, -262, 0, 47, 0, 11
};
#endif
#endif // # if EN_UP_DOWN_SAMPLE_OPTION

static void CommitUs(const SigEnvCom_t *pSigEnv, DmKsUsFxp_t *pKsUs)
{
# if EN_UP_DOWN_SAMPLE_OPTION
    int i;

    pKsUs->chrmIn = CHRM_C2K(pSigEnv->Chrm);
# endif

    pKsUs->minUs = 0;
    pKsUs->maxUs = (1 << pSigEnv->Bdp) - 1;

# if EN_UP_DOWN_SAMPLE_OPTION
    //// UV
    pKsUs->filterUvRowUsHalfSize = FilterUvRowUsHalfSize;
    pKsUs->filterUvRowUsScale2P = FilterUvRowUsScale2p;
    for (i = 0; i < 2*FilterUvRowUsHalfSize; ++i)
    {
        pKsUs->filterUvRowUs0_m[i] = FilterUvRowUs0_m[i];
        pKsUs->filterUvRowUs1_m[i] = FilterUvRowUs1_m[i];
    }

    pKsUs->filterUvColUsHalfSize = FilterUvColUsHalfSize;
    pKsUs->filterUvColUsScale2P = FilterUvColUsScale2p;
    for (i = 0; i < 2*FilterUvColUsHalfSize; ++i)
    {
        pKsUs->filterUvColUs_m[i] = FilterUvColUs_m[i];
    }
# endif
}

static void CommitUds(const DmCfgFxp_t *pDmCfg, HDmFxp_t hDm)
{
    DmKsDsFxp_t *pKsDs = &hDm->hDmKs->ksDs;
# if EN_UP_DOWN_SAMPLE_OPTION
    int i;

    pKsDs->chrmOut = CHRM_C2K(pDmCfg->tgtSigEnv.Chrm);
# endif

    pKsDs->minDs = 0;
    pKsDs->maxDs = (1 << pDmCfg->tgtSigEnv.Bdp) - 1;

# if EN_UP_DOWN_SAMPLE_OPTION
    //// UV
    pKsDs->filterUvColDsRadius = FilterUvColDsRadius;
    pKsDs->filterUvColDsScale2P = FilterUvColDsScale2p;
    for (i = 0; i <= 2*FilterUvColDsRadius; ++i)
    {
        pKsDs->filterUvColDs_m[i] = FilterUvColDs_m[i];
    }
# endif

    CommitUs((const SigEnvCom_t *)&pDmCfg->srcSigEnv, &hDm->hDmKs->ksUs);

    hDm->hDmKs->ksFrmFmtI.bdp = pDmCfg->srcSigEnv.Bdp;
    hDm->hDmKs->ksFrmFmtI.chrm = hDm->hDmKs->ksUs.chrmIn;
    hDm->hDmKs->ksFrmFmtI.clr = hDm->hDmKs->ksIMap.clr;
    hDm->hDmKs->ksFrmFmtO.bdp = pDmCfg->tgtSigEnv.Bdp;
    hDm->hDmKs->ksFrmFmtO.chrm = pKsDs->chrmOut;
    hDm->hDmKs->ksFrmFmtO.clr = hDm->hDmKs->ksOMap.clr;

}


static void SigEnv2KsFrmFmt(const SigEnvCom_t *pSigEnv, DmKsFrmFmt_t *pksFrmFmt)
{
    pksFrmFmt->rowNum = pSigEnv->RowNum;
    pksFrmFmt->colNum = pSigEnv->ColNum;

    pksFrmFmt->dtp = DTP_C2K(pSigEnv->Dtp);
    pksFrmFmt->weav = WEAV_C2K(pSigEnv->Weav);
    pksFrmFmt->loc = LOC_C2K(pSigEnv->Loc);

    pksFrmFmt->rowPitch = pSigEnv->RowPitch;
    pksFrmFmt->rowPitchC = pSigEnv->RowPitchC;
    pksFrmFmt->colPitch = GET_PXL_COL_PITCH(pSigEnv->Clr, pSigEnv->Chrm, pSigEnv->Weav, pSigEnv->Dtp);
}

int UpdateKsInFrameMemLayout(int rowPitch, int rowPitchC, HDmFxp_t hDm)
{
    if (hDm && hDm->hDmKs)
    {
        hDm->hDmKs->ksFrmFmtI.rowPitch  = rowPitch;
        hDm->hDmKs->ksFrmFmtI.rowPitchC = rowPitchC;
        return 0;
    }

    return -1;
}


static int CommitSrcSigEnv(const SrcSigEnvFxp_t *pSrcSigEnv, HDmFxp_t hDm)
{
    DmKsIMapFxp_t  *pKsIMap = &hDm->hDmKs->ksIMap;
    int32_t offset[3];

# if !EN_IPT_PQ_ONLY_OPTION
    CscCtrlFxp_t *pCscCtrl = &hDm->cscCtrl;
    int inBits;

    // logically, there shall be wp info in mds, or stipulating it is (1,1,1)
    pCscCtrl->sV3Wp[0] = pCscCtrl->sV3Wp[1] = pCscCtrl->sV3Wp[2] = 32767;

# if DM_FULL_SRC_SPEC
    inBits = pSrcSigEnv->Bdp;

    pKsIMap->clr = CLR_C2K(pSrcSigEnv->Clr);

    if (pSrcSigEnv->Clr != CClrIpt)
    {
        //// Yuv2Rgb m33 xfer
# if EN_KS_DM_IN
        if (pSrcSigEnv->Yuv2RgbExt)
        {
            AssignM33I2I(pSrcSigEnv->M33Yuv2Rgb, pKsIMap->m33Yuv2Rgb);
            pKsIMap->m33Yuv2RgbScale2P = pSrcSigEnv->M33Yuv2RgbScale2P;
        }
        else
# endif
            if (pSrcSigEnv->Rng != CRngNarrow)
                pKsIMap->m33Yuv2RgbScale2P = GetYuv2RgbM33Fxp(pSrcSigEnv->YuvXferSpec, pKsIMap->m33Yuv2Rgb);
            else
                pKsIMap->m33Yuv2RgbScale2P = GetYuv2RgbM33NarrowFxp(pSrcSigEnv->YuvXferSpec, pSrcSigEnv->Bdp, pKsIMap->m33Yuv2Rgb);


# if EN_KS_DM_IN
        if (pSrcSigEnv->Yuv2RgbOffExt)
            AssignV3I2I(pSrcSigEnv->V3Yuv2RgbOff, offset);
        else
# endif
            GetYuvRgbOffFxp(pSrcSigEnv->Rng, inBits, offset);
        offset[0] <<= 16; // upscale by 16 bits
        offset[1] <<= 16;
        Yuv2RgbOffsetRgbFxp(pKsIMap->m33Yuv2Rgb, offset[0], offset[1], pKsIMap->v3Yuv2RgbOffInRgb);
        //// eotfParam
        GetRange(inBits,
                 (pSrcSigEnv->Clr != CClrRgb && pSrcSigEnv->Clr != CClrRgba) ? CRngFull : pSrcSigEnv->Rng,
                 pSrcSigEnv->Eotf,
                 &pKsIMap->eotfParam.rangeMin, &pKsIMap->eotfParam.range);
        pKsIMap->eotfParam.rangeInv = DLB_UINT_MAX(32)/(pKsIMap->eotfParam.range << (16 - inBits)); // scale up 2^32
        pKsIMap->eotfParam.bdp = inBits;

        pKsIMap->eotfParam.eotf = EOTF_C2K(pSrcSigEnv->Eotf);

        if (pSrcSigEnv->Eotf == CEotfBt1886)
        {
#   if (REDUCED_COMPLEXITY == 0)
            pKsIMap->eotfParam.gamma = pSrcSigEnv->Gamma;
            pKsIMap->eotfParam.a = pSrcSigEnv->A;
            pKsIMap->eotfParam.b = pSrcSigEnv->B;
            pKsIMap->eotfParam.g = pSrcSigEnv->G;
#   else
            {
                uint16_t i;
                uint16_t x = 0;
                for (i = 0; i < DEF_G2L_LUT_SIZE; i++)
                {
                    pKsIMap->g2L[i] = DeGamma(pSrcSigEnv->A, pSrcSigEnv->B, pSrcSigEnv->Gamma, pSrcSigEnv->G, x);
                    x += (0x10000 / DEF_G2L_LUT_SIZE);
                }
            }
#   endif
        }
        else
        {
            //PQ=>L params are const
            //pKsIMap->eotfParam.gamma = 0;
        }

# if EN_KS_DM_IN
        if (pSrcSigEnv->Rgb2LmsM33Ext)
        {
            MemCpyByte(&pCscCtrl->sM33Rgb2Lms[0][0], &pSrcSigEnv->M33Rgb2Lms[0][0], 9*sizeof(pCscCtrl->sM33Rgb2Lms[0][0]));
            pCscCtrl->sM33Rgb2LmsScale2P = pSrcSigEnv->M33Rgb2LmsScale2P;
        }
        else if (pSrcSigEnv->Rgb2LmsRgbwExt)
        {
            pCscCtrl->sM33Rgb2LmsScale2P = GetRgb2LmsByPrimsM33Fxp(pSrcSigEnv->V8Rgbw[0], pSrcSigEnv->V8Rgbw[1], pSrcSigEnv->V8Rgbw[2], pSrcSigEnv->V8Rgbw[3],
                                           pSrcSigEnv->V8Rgbw[4], pSrcSigEnv->V8Rgbw[5], pSrcSigEnv->V8Rgbw[6], pSrcSigEnv->V8Rgbw[7], pCscCtrl->sM33Rgb2Lms);
        }
        else
# endif
            pCscCtrl->sM33Rgb2LmsScale2P = GetRgb2LmsByDefM33(pSrcSigEnv->RgbDef, pCscCtrl->sM33Rgb2Lms);

        pCscCtrl->sV3WpScale2P = 15;
# if DM_VER_HIGHER_THAN211
        if (pSrcSigEnv->WpExt)
        {
            MemCpyByte(&pCscCtrl->sV3Wp[0], &pSrcSigEnv->V3Wp[0], 3*sizeof(pCscCtrl->sV3Wp[0]));
            pCscCtrl->sV3WpScale2P = pSrcSigEnv->WpScale;
        }
        //else
# endif
        //GetRgb2LmsWpV3(pCscCtrl->sM33Rgb2Lms, pCscCtrl->sV3Wp);


//# if DM_VER_CTRL == 2
//  pKsIMap->m33Rgb2LmsScale2P = GetRgb2WpLmsM33Fxp(pCscCtrl->sM33Rgb2Lms, pCscCtrl->sM33Rgb2LmsScale2P, pCscCtrl->sV3Wp, pCscCtrl->sV3WpScale2P, pKsIMap->m33Rgb2Lms);
//# endif
//# if DM_VER_CTRL == 3
        pKsIMap->m33Rgb2LmsScale2P = GetRgb2LmsCtWpM33Fxp(pCscCtrl->sM33Rgb2Lms, pCscCtrl->sM33Rgb2LmsScale2P,
                                     pSrcSigEnv->CrossTalk, pCscCtrl->sV3Wp, pCscCtrl->sV3WpScale2P, pKsIMap->m33Rgb2Lms);
//# endif

        // L=>PQ: const
    }
    else
    {
        // hardwire short cut ipt input bypass data plan is always set up
#   if !EN_IPT_PQ_INPUT_SHORT_CUT
        int32_t iOffset[3], iScale;
        // fake it
        pKsIMap->clr = KClrYuv;

        // Ipt2Lms xfer, no range conversion
        pKsIMap->m33Yuv2RgbScale2P = GetIpt2LmsM33Fxp(pKsIMap->m33Yuv2Rgb);

        // get the ipt scale and offset, v3Yuv2RgbOff to v3Yuv2RgbOffInRgb
        GetIptNormParam(pSrcSigEnv->Bdp, pSrcSigEnv->Rng, iOffset, &iScale);
        Yuv2RgbOffsetRgbFxp(pKsIMap->m33Yuv2Rgb, iOffset[0], iOffset[1], pKsIMap->v3Yuv2RgbOffInRgb);
        // move scale to ipt pq here, the denormal stage
        pKsIMap->eotfParam.rangeMin = 0;
        pKsIMap->eotfParam.rangeInv = DLB_UINT_MAX(32)/(iScale << (16 - inBits)); //TODO: test this

        // ipt must be in pq
        pKsIMap->eotfParam.eotf = KEotfPq;
        // PQ=>L: const

        // stay in LMS: with incoming crosstalk and wp
        pKsIMap->m33Rgb2Lms[0][0] = pKsIMap->m33Rgb2Lms[1][1] = pKsIMap->m33Rgb2Lms[2][2] = 1;
        pKsIMap->m33Rgb2Lms[0][1] = pKsIMap->m33Rgb2Lms[0][2] =
                                        pKsIMap->m33Rgb2Lms[1][0] = pKsIMap->m33Rgb2Lms[1][2] =
                                                pKsIMap->m33Rgb2Lms[2][0] = pKsIMap->m33Rgb2Lms[2][1] = 0;
        pKsIMap->m33Rgb2LmsScale2P = 0;

        pCscCtrl->sV3WpScale2P = 15;
#   if DM_VER_HIGHER_THAN211
        if (pSrcSigEnv->WpExt)
        {
            MemCpyByte(&pCscCtrl->sV3Wp[0], &pSrcSigEnv->V3Wp[0], 3*sizeof(pCscCtrl->sV3Wp[0]));
        }
#   endif

        // L=>PQ const

#   endif //!EN_IPT_PQ_INPUT_SHORT_CUT
    }
# else
    (void)pSrcSigEnv;
    (void)inBits;
# endif // DM_FULL_SRC_SPEC

    // Lms=>Ipt
    pKsIMap->m33Lms2IptScale2P = GetLms2IptM33Fxp(pKsIMap->m33Lms2Ipt);

# endif // !EN_IPT_PQ_ONLY_OPTION

# if DM_FULL_SRC_SPEC && (EN_IPT_PQ_ONLY_OPTION || DM_VER_CTRL == 3)
    GetIptNormParam(pSrcSigEnv->Bdp, pSrcSigEnv->Rng, pKsIMap->v3IptOff, &pKsIMap->iptScale);
# endif

    return 0;
}

// the Dm3KsOMapFxp_t in ctrl layer
# define GET_DM3OMAP_PTR(hCtxt)          &hCtxt->dm3OMap
# define DECLARE_DM3OMAP_PTR(ptr)        Dm3OMapFxp_t  *ptr
# define DEFINE_DM3OMAP_PTR(hCtxt, ptr)  DECLARE_DM3OMAP_PTR(ptr) = GET_DM3OMAP_PTR(hCtxt)

int32_t CommitKsOMapDm2FromDm3(const HDmCfgFxp_t pDmCfg, HDmFxp_t hDm);

#if DM_VER_HIGHER_THAN211

static void KsOMapL2NLLut(const CEotf_t Eotf, const uint16_t Gamma, DECLARE_DM3OMAP_PTR(pKsOMap), uint32_t tMin, uint32_t tMax)
{
    if (Eotf == CEotfBt1886)
    {
        uint32_t gammaR;
        int64_t minOverMaxGmR;

        gammaR = (uint32_t)(((int64_t)1 << (31 + 14)) / Gamma); // scale in 31

        GenerateL2GLut(gammaR, pKsOMap->l2nlLutA, pKsOMap->l2nlLutB, pKsOMap->l2nlLutX);

        pKsOMap->l2nlLutAScale2P = 31;
        pKsOMap->l2nlLutBScale2P = 16;
        pKsOMap->l2nlLutXScale2P = 31;

        pKsOMap->preMapA = (int32_t)((((int64_t)1 << (31 + 18))) / tMax);
        pKsOMap->preMapB = 0;

        // calculate (tMin/tMax)^gmR
        minOverMaxGmR = ((int64_t)tMin << 31) / tMax; // scale in 31
        minOverMaxGmR = powTaylor((uint32_t)minOverMaxGmR, gammaR);

        // calculate 1/(1 - (tMin/tMax)^gmR)
        pKsOMap->postMapA = (int32_t)(((int64_t)1 << (31 + 16)) / (((int64_t)1 << 31) - minOverMaxGmR));
        // calculate (tMin/tMax)^gmR/(1 - (tMin/tMax)^gmR)
        pKsOMap->postMapB = -(int32_t)(((int64_t)pKsOMap->postMapA * minOverMaxGmR) >> 31);
    }
    else if (Eotf == CEotfPower)
    {
        uint32_t gammaR;
        int64_t minOverMax;

        gammaR = (uint32_t)(((int64_t)1 << (31 + 14)) / Gamma); // scale in 31

        GenerateL2GLut(gammaR, pKsOMap->l2nlLutA, pKsOMap->l2nlLutB, pKsOMap->l2nlLutX);

        pKsOMap->l2nlLutAScale2P = 31;
        pKsOMap->l2nlLutBScale2P = 16;
        pKsOMap->l2nlLutXScale2P = 31;

        // calculate tMin/tMax
        minOverMax = ((int64_t)tMin << 31) / tMax; // scale in 31

        // calculate (1/tMax)/(1 - (tMin/tMax))
        pKsOMap->preMapA = (int32_t)(((int64_t)1 << (31 + 31)) / (((((int64_t)1 << 31) - minOverMax) * tMax) >> 18));
        // calculate (tMin/tMax)/(1 - (tMin/tMax))
        pKsOMap->preMapB = -(int32_t)(((int64_t)pKsOMap->preMapA * tMin) >> 18);

        pKsOMap->postMapA = (int32_t)1 << 16;
        pKsOMap->postMapB = 0;
    }
    else
    {
        int32_t i;
        for (i = 0; i < DEF_L2PQ_LUT_NODES; i++)
        {
            pKsOMap->l2nlLutA[i] = l2pqLutA[i];
            pKsOMap->l2nlLutB[i] = l2pqLutB[i];
            pKsOMap->l2nlLutX[i] = l2pqLutX[i];
        }

        pKsOMap->l2nlLutAScale2P = DEF_L2G_LUT_A_SCALE2P;
        pKsOMap->l2nlLutBScale2P = DEF_L2G_LUT_B_SCALE2P;
        pKsOMap->l2nlLutXScale2P = DEF_L2G_LUT_X_SCALE2P;

        pKsOMap->preMapA = (int32_t)1 << 18;
        pKsOMap->preMapB = 0;

        pKsOMap->postMapA = (int32_t)1 << 16;
        pKsOMap->postMapB = 0;
    }
}

static void UpdateKsOMapTmm(const HDmCfgFxp_t pDmCfg,
# if EN_GLOBAL_DIMMING
                            int16_t gdActive,
#                           endif
                            HDmFxp_t hDm)
{
    DEFINE_DM3OMAP_PTR(hDm, pKsOMap);
    TgtSigEnvFxp_t *pTgtSigEnv = &pDmCfg->tgtSigEnv;
    uint32_t tMin, tMax;

# if EN_GLOBAL_DIMMING
    if (gdActive)
    {
        tMin = hDm->gdCtrlDr.tMinEff;
        tMax = hDm->gdCtrlDr.tMaxEff;
    }
    else
    {
# endif
        tMin = pTgtSigEnv->Min;
        tMax = pTgtSigEnv->Max;
# if EN_GLOBAL_DIMMING
    }
# endif

# if !EN_IPT_PQ_OUTPUT_SHORT_CUT
    if (pTgtSigEnv->Clr == CClrIpt)
    {
        // to avoid clamping in LMS during calculated ipt output
        tMin = 0;
        tMax = (uint32_t)10000L * (1 << 18);
    }
# endif

    // linear space range
    pKsOMap->oetfParam.min = tMin;
    pKsOMap->oetfParam.max = tMax;

    KsOMapL2NLLut(pTgtSigEnv->Eotf, pTgtSigEnv->Gamma, pKsOMap, tMin, tMax);
}


static int CommitKsOMap(const HDmCfgFxp_t pDmCfg, int32_t updateTmm, HDmFxp_t hDm)
{
    int j,k;
    const TgtSigEnvFxp_t *pTgtSigEnv = &pDmCfg->tgtSigEnv;
    DEFINE_DM3OMAP_PTR(hDm, pKsOMap);
    CscCtrlFxp_t *pCscCtrl = &hDm->cscCtrl;
    int outBits = pTgtSigEnv->Bdp;
    int32_t offset[3];

    pCscCtrl->tV3Wp[0] = pCscCtrl->tV3Wp[1] = pCscCtrl->tV3Wp[2] = 32767;
    pCscCtrl->tV3WpScale2P = 15;

    if (pTgtSigEnv->Clr != CClrIpt)
    {
        //// IPT=>LMS
        AssignM33I2I((const int16_t (*)[3])pCscCtrl->m33Ipt2Lms, pKsOMap->m33Ipt2Lms);
        pKsOMap->m33Ipt2LmsScale2P = pCscCtrl->m33Ipt2LmsScale2P;

        //// PQ=>L: const

        //// Lms=>RGB with wp and crosstalk
#   if EN_KS_DM_IN
        if (pTgtSigEnv->Lms2RgbM33Ext)
        {
            MemCpyByte(&pCscCtrl->tM33Lms2Rgb[0][0], &pTgtSigEnv->M33Lms2Rgb[0][0], 9*sizeof(pCscCtrl->tM33Lms2Rgb[0][0]));
            pCscCtrl->tM33Lms2RgbScale2P = pTgtSigEnv->M33Lms2RgbScale2P;
        }
        else if (pTgtSigEnv->Lms2RgbRgbwExt)
        {
            pCscCtrl->tM33Lms2RgbScale2P = GetLms2RgbByPrimsM33Fxp(pTgtSigEnv->V8Rgbw[0], pTgtSigEnv->V8Rgbw[1], pTgtSigEnv->V8Rgbw[2], pTgtSigEnv->V8Rgbw[3],
                                           pTgtSigEnv->V8Rgbw[4], pTgtSigEnv->V8Rgbw[5], pTgtSigEnv->V8Rgbw[6], pTgtSigEnv->V8Rgbw[7], pCscCtrl->tM33Lms2Rgb);
        }
        else
#   endif
            pCscCtrl->tM33Lms2RgbScale2P = GetLms2RgbByDefM33Fxp(pTgtSigEnv->RgbDef, pCscCtrl->tM33Lms2Rgb);



        if (pTgtSigEnv->WpExt)
        {
            MemCpyByte(&pCscCtrl->tV3Wp[0], &pTgtSigEnv->V3Wp[0], 3*sizeof(pCscCtrl->tV3Wp[0]));
            pCscCtrl->tV3WpScale2P = pTgtSigEnv->WpScale;
        }

        pKsOMap->m33Lms2RgbScale2P = GetLms2RgbCtWpM33Fxp(pCscCtrl->tM33Lms2Rgb, pCscCtrl->tM33Lms2RgbScale2P,
                                     pTgtSigEnv->CrossTalk, pCscCtrl->tV3Wp, pCscCtrl->tV3WpScale2P, pKsOMap->m33Lms2Rgb);

        //// oetfParam
        if (updateTmm)
        {
            // if called from CommitMds() and EN_GLOBAL_DIMMING == 1, updateTmm == 0
            // so the LOCAL_GD_SETTING(pDmCfg->gdCtrl.GdOn) is OK too
            UpdateKsOMapTmm(pDmCfg,
                            LOCAL_GD_SETTING(pDmCfg->gdCtrl.GdOn)
                            hDm);
        }

        // final output space
        GetRange(outBits,
                 (pTgtSigEnv->Clr != CClrRgb && pTgtSigEnv->Clr != CClrRgba) ? CRngFull : pTgtSigEnv->Rng,
                 pTgtSigEnv->Eotf,
                 &pKsOMap->oetfParam.rangeMin, &pKsOMap->oetfParam.range);
        // range conversion in 0.16
        pKsOMap->oetfParam.rangeOverOne = ((uint32_t)pKsOMap->oetfParam.range << 16) / ((1 << pTgtSigEnv->Bdp) - 1); // scale up by 2^16
        pKsOMap->oetfParam.bdp = outBits;

        pKsOMap->oetfParam.oetf = EOTF_C2K(pTgtSigEnv->Eotf);

        //// rgb=>yuv
        pKsOMap->clr = CLR_C2K(pTgtSigEnv->Clr);

# if EN_KS_DM_IN
        if (pTgtSigEnv->Rgb2YuvExt)
        {
            AssignM33I2I(pTgtSigEnv->M33Rgb2Yuv, pKsOMap->m33Rgb2Yuv);
            pKsOMap->m33Rgb2YuvScale2P = pTgtSigEnv->M33Rgb2YuvScale2P;
        }
        else
# endif
            if (pTgtSigEnv->Rng != CRngNarrow)
                pKsOMap->m33Rgb2YuvScale2P = GetRgb2YuvM33Fxp(pTgtSigEnv->YuvXferSpec, pKsOMap->m33Rgb2Yuv);
            else
                pKsOMap->m33Rgb2YuvScale2P = GetRgb2YuvM33NarrowFxp(pTgtSigEnv->YuvXferSpec, pTgtSigEnv->Bdp, pKsOMap->m33Rgb2Yuv);


# if EN_KS_DM_IN
        if (pTgtSigEnv->Rgb2YuvOffExt)
        {
            AssignV3I2I(pTgtSigEnv->V3Rgb2YuvOff, offset);
            pKsOMap->v3Rgb2YuvOff[0] = (uint32_t)offset[0] >> (15 - outBits); // config use scale 15
            pKsOMap->v3Rgb2YuvOff[1] = (uint32_t)offset[1] >> (15 - outBits);
            pKsOMap->v3Rgb2YuvOff[2] = (uint32_t)offset[2] >> (15 - outBits);
        }
        else
# endif
        {
            GetYuvRgbOffFxp(pTgtSigEnv->Rng, outBits, offset);
            pKsOMap->v3Rgb2YuvOff[0] = (uint32_t)offset[0];
            pKsOMap->v3Rgb2YuvOff[1] = (uint32_t)offset[1];
            pKsOMap->v3Rgb2YuvOff[2] = (uint32_t)offset[2];
        }
    }
    else
    {
# if EN_IPT_PQ_OUTPUT_SHORT_CUT
        pKsOMap->clr = KClrIpt;
# else

        //// use full data path to achieve the fact of scaling and offseting ipt pq input
        //// IPT=>LMS: const
        AssignM33I2I((const int16_t (*)[3])pCscCtrl->m33Ipt2Lms, pKsOMap->m33Ipt2Lms);
        pKsOMap->m33Ipt2LmsScale2P = pCscCtrl->m33Ipt2LmsScale2P;

        //// PQ=>L: const

        // let it stay in LMS
        pKsOMap->m33Lms2Rgb[0][0] = pKsOMap->m33Lms2Rgb[1][1] = pKsOMap->m33Lms2Rgb[2][2] = 1;
        pKsOMap->m33Lms2Rgb[0][1] = pKsOMap->m33Lms2Rgb[0][2] =
                                        pKsOMap->m33Lms2Rgb[1][0] = pKsOMap->m33Lms2Rgb[1][2] =
                                                pKsOMap->m33Lms2Rgb[2][0] = pKsOMap->m33Lms2Rgb[2][1] = 0;
        pKsOMap->m33Lms2RgbScale2P = 0;

        //pKsOMap->oetfParam.min = 0;
        //pKsOMap->oetfParam.max = 10000L * (1 << 18); // pq=>l result, shall be 10000
        // since UpdateKsOMapTmm will called again for global dimming, move the above
        // min, max setting to UpdateKsOMapTmm()
        if (updateTmm)
        {
            // if called from CommitMds() and EN_GLOBAL_DIMMING == 1, updateTmm == 0
            // so the LOCAL_GD_SETTING(pDmCfg->gdCtrl.GdOn) is OK too
            UpdateKsOMapTmm(pDmCfg,
                            LOCAL_GD_SETTING(pDmCfg->gdCtrl.GdOn)
                            hDm);
        }

        // L=>PQ const and

        // get the ipt scale and offset, v3Rgb2YuvOff: ipt scaled up by denormal
        GetIptDnParam(pTgtSigEnv->Bdp, pTgtSigEnv->Rng, pKsOMap->v3IptOff, &pKsOMap->iptScale);

        // move scale to ipt pq here, the denormal stage
        pKsOMap->oetfParam.rangeMin = 0;
        pKsOMap->oetfParam.rangeOverOne = pKsOMap->iptScale << (16 - outBits);

        pKsOMap->oetfParam.bdp = outBits;

        // we know it
        pKsOMap->oetfParam.oetf = KEotfPq;
        // and fake it
        pKsOMap->clr = KClrYuv;

        // let rgb2yuv achieve LMS=>IPT and offset
        pKsOMap->m33Rgb2YuvScale2P = GetLms2IptM33Fxp(pKsOMap->m33Rgb2Yuv);
        // borrow v3Rgb2YuvOff
        pKsOMap->v3Rgb2YuvOff[0] = pKsOMap->v3IptOff[0];
        pKsOMap->v3Rgb2YuvOff[1] = pKsOMap->v3IptOff[1];
        pKsOMap->v3Rgb2YuvOff[2] = pKsOMap->v3IptOff[2];
# endif
    }

    //// set it up anyway, debug code may use it too
    GetIptDnParam(pTgtSigEnv->Bdp, pTgtSigEnv->Rng, pKsOMap->v3IptOff, &pKsOMap->iptScale);

    CommitKsOMapDm2FromDm3(pDmCfg, hDm);

    return 0;
}
#endif // DM_VER_HIGHER_THAN211


#if DM_SOFTWARE_BYPASS
#define SET_IDENTICAL_M33(m33, val)                 \
  m33[0][0] = val; m33[0][1] = 0;   m33[0][2] = 0;  \
  m33[1][0] = 0;   m33[1][1] = val; m33[1][2] = 0;  \
  m33[2][0] = 0;   m33[2][1] = 0;   m33[2][2] = val;

// hack the kernel control to perform an effective bypass
static int CommitKsIMapBypass(KClr_t kClr, int32_t bdp, HDmFxp_t hDm)
{
    DmKsIMapFxp_t  *pKsIMap = &hDm->hDmKs->ksIMap;

    // we can set clr to rgb so that Yuv2Rgb stuff can be ignored.
    // to be safe, follow input color. Just in case kernel does
    // some clr space related checking before check chrm for upsampling op
    pKsIMap->clr = kClr;

    // stay in input color space
    SET_IDENTICAL_M33(pKsIMap->m33Yuv2Rgb, 32767);
    pKsIMap->m33Yuv2RgbScale2P = 15;
    pKsIMap->v3Yuv2RgbOffInRgb[0] = pKsIMap->v3Yuv2RgbOffInRgb[1] = pKsIMap->v3Yuv2RgbOffInRgb[2] = 0;

    // set to full range to simplify code and be safe(smaller PQ value behave better?)
    pKsIMap->eotfParam.rangeMin = 0;
    pKsIMap->eotfParam.range = (1 << bdp) - 1;
    pKsIMap->eotfParam.rangeInv = DLB_UINT_MAX(32)/(pKsIMap->eotfParam.range << (16 - bdp)); // scale up 2^32
    hDm->hDmKs->ksFrmFmtI.bdp = bdp;

    // set to pq to cancel the fixed L=>PQ mapping
    pKsIMap->eotfParam.eotf = KEotfPq;// PQ=>L: const

    // stay in input color space
    SET_IDENTICAL_M33(pKsIMap->m33Rgb2Lms, 1);
    pKsIMap->m33Rgb2LmsScale2P = 0;

    // L=>PQ const

    // stay in input color space
    SET_IDENTICAL_M33(pKsIMap->m33Lms2Ipt, 1);
    pKsIMap->m33Lms2IptScale2P = 0;

    return 0;
}

static int CommitDmLutBypass(DmKs2GmLutFxp_t *pKsGmLut)
{
# if USE_12BITS_IN_3D_LUT
# define UPSCALE_4BIT_TO_TBIT(v) (v != 16) ? (v << 8) :  0xfff;
#else
# define UPSCALE_4BIT_TO_TBIT(v) (v != 16) ? (v << 12) : 0xffff;
#endif

    uint16_t i, p, t;
    uint16_t *pIpt;

    // build a 4 bit(17 node) indexing => [0, 2^16(12) - 1] 3D LUT mapping.
    // using 17 nodes in case hardware implemented is fixed this value
    pKsGmLut->dimC1 = 17;
    pKsGmLut->dimC2 = 17;
    pKsGmLut->dimC3 = 17;

    pKsGmLut->valTp = (uint16_t)GmLutTypeIpt2Yuv; // no rgb=>yuv in OMap

    // input is simply normalized according to full range
    pKsGmLut->iMinC1 = 0;
    pKsGmLut->iMaxC1 = 32767;
    pKsGmLut->iMinC2 = 0;
    pKsGmLut->iMaxC2 = 32767;
    pKsGmLut->iMinC3 = 0;
    pKsGmLut->iMaxC3 = 32767;

    pKsGmLut->iDistC1Inv = (int32_t)((((uint32_t)1<<30) / (pKsGmLut->iMaxC1 - pKsGmLut->iMinC1)));
    pKsGmLut->iDistC2Inv = (int32_t)((((uint32_t)1<<30) / (pKsGmLut->iMaxC2 - pKsGmLut->iMinC2)));
    pKsGmLut->iDistC3Inv = (int32_t)((((uint32_t)1<<30) / (pKsGmLut->iMaxC3 - pKsGmLut->iMinC3)));

    // pre-calculated  value
    pKsGmLut->pitch = 17;
    pKsGmLut->slice = 17 * 17;

    // 3D LUT content
    pIpt = pKsGmLut->lutMap;
    for (t = 0; t < 17; ++t)   for (p = 0; p < 17; ++p)   for (i = 0; i < 17; ++i)
            {
                *pIpt++ = UPSCALE_4BIT_TO_TBIT(i)
                          *pIpt++ = UPSCALE_4BIT_TO_TBIT(p);
                *pIpt++ = UPSCALE_4BIT_TO_TBIT(t);
            }

    return 0;
}

static int CommitKsOMapBypass(KClr_t kClr, int32_t bdp, HDmFxp_t hDm)
{
    Dm2KsOMapFxp_t  *pKsOMap = &hDm->hDmKs->ksOMap;

    pKsOMap->gain = 4095;
    pKsOMap->offset = 1;
    pKsOMap->satGain = 4095;

    CommitDmLutBypass(&pKsOMap->ksGmLut);

    // final output space. likely no used cause 3D lut is in YUV, but anyway...
    pKsOMap->tRangeMin = 0;
    pKsOMap->tRange = (1 << bdp) - 1;
    pKsOMap->tRangeOverOne = (pKsOMap->tRange << (32 - bdp)) / DLB_UINT_MAX(16);
    pKsOMap->tRangeInv = DLB_UINT_MAX(32)/(pKsOMap->tRange<<(16 - 16));

    hDm->hDmKs->ksFrmFmtO.bdp = bdp;

    // for possible down sample checking
    pKsOMap->clr = kClr;

    //// setting bellow this point does not matter since the 3D lut output is set to yuv type already,
    // just give it some valid value
    SET_IDENTICAL_M33(pKsOMap->m33Rgb2Yuv, 1);
    pKsOMap->m33Rgb2YuvScale2P = 0;
    pKsOMap->v3Rgb2YuvOff[0] = 0;
    pKsOMap->v3Rgb2YuvOff[1] = 0;
    pKsOMap->v3Rgb2YuvOff[2] = 0;

    return 0;
}

// hack the kernel control to perform an effective bypass
static int CommitKsIOMapBypass(KClr_t kClrI, int32_t bdpI, int32_t bdpO, HDmFxp_t hDm)
{
    CommitKsIMapBypass(kClrI, bdpI, hDm);
    CommitKsOMapBypass(kClrI, bdpO, hDm);
    return 0;
}

#endif //DM_SOFTWARE_BYPASS

#if DM_VER_LOWER_THAN212

#define HACK_I16_DELTA_MAX_MIN_HARDWARE   0

static int CommitDmLut(const HGmLut_t hGmLut, DmKs2GmLutFxp_t *pKsGmLut)
{
    if (!hGmLut->LutMap) return 0;

    pKsGmLut->dimC1 = hGmLut->DimC1;
    pKsGmLut->dimC2 = hGmLut->DimC2;
    pKsGmLut->dimC3 = hGmLut->DimC3;

    //// implied
    pKsGmLut->valTp = 0;
    if ((hGmLut->Type == GmLutTypeRgb2Ipt) || (hGmLut->Type == GmLutTypeIpt2Ipt))
    {
        // ipt output
        pKsGmLut->valTp |= 0x1;
    }
    if ((hGmLut->Type == GmLutTypeIpt2Rgb) || (hGmLut->Type == GmLutTypeIpt2Ipt))
    {
        pKsGmLut->valTp |= 0x2;
    }

    //// derived float point value
    pKsGmLut->iMinC1 = hGmLut->IdxMinC1;
    pKsGmLut->iMaxC1 = hGmLut->IdxMaxC1;
    pKsGmLut->iMinC2 = hGmLut->IdxMinC2;
    pKsGmLut->iMaxC2 = hGmLut->IdxMaxC2;
    pKsGmLut->iMinC3 = hGmLut->IdxMinC3;
    pKsGmLut->iMaxC3 = hGmLut->IdxMaxC3;

# if HACK_I16_DELTA_MAX_MIN_HARDWARE
    if ((hGmLut->IdxMaxC2 - hGmLut->IdxMinC2) & 0x8000)
    {
        // int16_t fail case: it shall be uint16_t
        pKsGmLut->iMinC2 >>= 1;
        pKsGmLut->iMaxC2 >>= 1;
        pKsGmLut->iMinC3 >>= 1;
        pKsGmLut->iMaxC3 >>= 1;
    }
# endif

    pKsGmLut->iDistC1Inv = (int32_t)((((uint32_t)1<<30) / (pKsGmLut->iMaxC1 - pKsGmLut->iMinC1)));
    pKsGmLut->iDistC2Inv = (int32_t)((((uint32_t)1<<30) / (pKsGmLut->iMaxC2 - pKsGmLut->iMinC2)));
    pKsGmLut->iDistC3Inv = (int32_t)((((uint32_t)1<<30) / (pKsGmLut->iMaxC3 - pKsGmLut->iMinC3)));

    //// pre-calculated  value
    pKsGmLut->pitch = hGmLut->DimC3;
    pKsGmLut->slice = hGmLut->DimC2 * hGmLut->DimC3;

    return 0;
}

#if EN_GLOBAL_DIMMING
////// 1 D Lut look up
#define LUT1D(lut, LUT_SIZE_M1, LUT_SHIFT, LUT_MASK, x, y) \
  {int32_t iIdx, frac, y2; \
  if ((x) <= 0) y = lut[0]; \
  else { \
  iIdx = (x) >> (LUT_SHIFT);  \
  frac = (x) & LUT_MASK;  \
  y = lut[iIdx];          \
  if (iIdx < LUT_SIZE_M1) {   \
    y2 = lut[iIdx + 1];     \
    y = y + (((int32_t)(y2 - y) * frac) >> (LUT_SHIFT));  \
  } else {  \
    y = lut[LUT_SIZE_M1];   \
  }}}

#define LUT1D_32(lut, LUT_SIZE_M1, LUT_SHIFT, LUT_MASK, x, y) \
  {int32_t iIdx, frac; \
  int64_t y2; \
  if ((x) <= 0) y = lut[0]; \
  else { \
  iIdx = (x) >> (LUT_SHIFT);  \
  frac = (x) & LUT_MASK;  \
  y = lut[iIdx];          \
  if (iIdx < LUT_SIZE_M1) {   \
    y2 = lut[iIdx + 1];     \
    y = (int32_t)(y + (((int64_t)(y2 - y) * frac) >> (LUT_SHIFT)));  \
  } else {  \
    y = lut[LUT_SIZE_M1];   \
  }}}

#define DM_EOTF_LUT_SCALE   27
void GdBuildEotfCrrLut(const HDmCfgFxp_t pDmCfg, HDmFxp_t hDm)
{
    GdCtrlDrFxp_t *pGdCtrlDr = &hDm->gdCtrlDr;
    int32_t alpha;
    int16_t i, lutIndex;
    uint16_t tMaxEffPq = LToPQ12(pGdCtrlDr->tMaxEff);

    // Determine which LUT to interpolate between and alpha
    alpha = (((int32_t)tMaxEffPq - (int32_t)pDmCfg->gdCtrl.GdWMmPq) << 12) /
            ((int32_t)pDmCfg->gdCtrl.GdWMaxPq - (int32_t)pDmCfg->gdCtrl.GdWMmPq); // scale 12, 0~4096

//  lutIndex = alpha >> 10;
//  lutIndex = CLAMPS(lutIndex, 0, 2);
    alpha = CLAMPS(alpha, 0, 4095); // need to be within 12 bits
    if (alpha < 4096/2)
        lutIndex = 0;
    else if (alpha < 4095)
        lutIndex = 1;
    else
        lutIndex = 2;

    // look up alpha lut to get compensated alpha
    LUT1D_32(pDmCfg->hPq2GLut->eotfLutAlpha[lutIndex], EOTF_LUT_ALPHA_SIZE - 1, 8, 0xFF, alpha, alpha);

    // Interpolate pq to gamma luts
    for (i = 0; i < EOTF_LUT_SIZE; i++)
    {
        int64_t e = (int64_t)pDmCfg->hPq2GLut->eotfLuts[lutIndex][i] +
                    (((int64_t)alpha * (pDmCfg->hPq2GLut->eotfLuts[lutIndex+1][i] - pDmCfg->hPq2GLut->eotfLuts[lutIndex][i])) >> DM_EOTF_LUT_SCALE);
        e = e >> (DM_EOTF_LUT_SCALE - 16);
        pGdCtrlDr->eotfCrrLut[i] = (uint16_t)CLAMPS(e, 0, DLB_UINT_MAX(16));
    }
}

static void GdUpdateGmLut(const HDmCfgFxp_t pDmCfg, HDmFxp_t hDm, uint16_t *lutMap)
{
    const uint16_t *lutMapA = pDmCfg->hGmLutA->LutMap;
    const uint16_t *lutMapB = pDmCfg->hGmLutB->LutMap;

    GdCtrlDrFxp_t *pGdCtrlDr = &hDm->gdCtrlDr;
    uint16_t lutEntryNum = GetGmLutMapSize(pGdCtrlDr->hGmLut);
    uint16_t e;

    int32_t alpha = (int32_t)((((int32_t)pDmCfg->gdCtrl.GdWMaxPq - (int32_t)hDm->tcCtrl.tcTgtSig.maxPq) << 16) /
                              ((int32_t)pDmCfg->gdCtrl.GdWMaxPq - (int32_t)pDmCfg->gdCtrl.GdWMmPq)); // scale 16, 0~65536
    int32_t v3[3], v3Yuv[3], v, *pV;
    int16_t i;
    uint16_t Pq;

    pGdCtrlDr->hGmLut->DimC1 = pDmCfg->hGmLutA->DimC1;
    pGdCtrlDr->hGmLut->DimC2 = pDmCfg->hGmLutA->DimC2;
    pGdCtrlDr->hGmLut->DimC3 = pDmCfg->hGmLutA->DimC3;

    pGdCtrlDr->hGmLut->IdxMinC1 = pDmCfg->hGmLutA->IdxMinC1;
    pGdCtrlDr->hGmLut->IdxMaxC1 = pDmCfg->hGmLutA->IdxMaxC1;
    pGdCtrlDr->hGmLut->IdxMinC2 = pDmCfg->hGmLutA->IdxMinC2;
    pGdCtrlDr->hGmLut->IdxMaxC2 = pDmCfg->hGmLutA->IdxMaxC2;
    pGdCtrlDr->hGmLut->IdxMinC3 = pDmCfg->hGmLutA->IdxMinC3;
    pGdCtrlDr->hGmLut->IdxMaxC3 = pDmCfg->hGmLutA->IdxMaxC3;

    if (pDmCfg->hGmLutA->Type == GmLutTypeIpt2Rgb && pGdCtrlDr->hGmLut->Type == GmLutTypeIpt2Yuv)
        pV = &v3Yuv[0];
    else
        pV = &v3[0];

    alpha = CLAMPS(alpha, 0, DLB_UINT_MAX(16));
    pGdCtrlDr->gmLutBlendingAlpha = alpha;// just for debug purpose

    GdBuildEotfCrrLut(pDmCfg, hDm);

    while (lutEntryNum)
    {
        for (i = 0; i < 3; ++i)
        {
            // correction to pGdCtrlDr->tcTgtSig.maxPq
            Pq = (uint16_t)((int32_t)lutMapA[i] + (alpha * ((int32_t)lutMapB[i] - (int32_t)lutMapA[i]) >> 16)); // rgb pq

            // pq to target eotf
            LUT1D(pGdCtrlDr->eotfCrrLut, EOTF_CRR_LUT_SIZE - 1, 6, 0x3F, Pq, v);
            v3[i] = v;
        }

        //// denormal
        v3[0] = (((int32_t)v3[0] * pGdCtrlDr->rangeOverOne) >> 16) + pGdCtrlDr->otRngMin;
        v3[1] = (((int32_t)v3[1] * pGdCtrlDr->rangeOverOne) >> 16) + pGdCtrlDr->otRngMin;
        v3[2] = (((int32_t)v3[2] * pGdCtrlDr->rangeOverOne) >> 16) + pGdCtrlDr->otRngMin;

        // rgb=>yuv
        if (pDmCfg->hGmLutA->Type == GmLutTypeIpt2Rgb &&
            pGdCtrlDr->hGmLut->Type == GmLutTypeIpt2Yuv)
        {
            v3Yuv[0] = (pGdCtrlDr->m33Rgb2Yuv[0][0] * v3[0] + pGdCtrlDr->m33Rgb2Yuv[0][1] * v3[1] + pGdCtrlDr->m33Rgb2Yuv[0][2] * v3[2]) >> pGdCtrlDr->m33Rgb2YuvScale2P;
            v3Yuv[1] = (pGdCtrlDr->m33Rgb2Yuv[1][0] * v3[0] + pGdCtrlDr->m33Rgb2Yuv[1][1] * v3[1] + pGdCtrlDr->m33Rgb2Yuv[1][2] * v3[2]) >> pGdCtrlDr->m33Rgb2YuvScale2P;
            v3Yuv[2] = (pGdCtrlDr->m33Rgb2Yuv[2][0] * v3[0] + pGdCtrlDr->m33Rgb2Yuv[2][1] * v3[1] + pGdCtrlDr->m33Rgb2Yuv[2][2] * v3[2]) >> pGdCtrlDr->m33Rgb2YuvScale2P;

            v3Yuv[0] = (uint16_t)(v3Yuv[0] + (pGdCtrlDr->v3Rgb2YuvOff[0] << 1)); // assume m33Rgb2YuvScale2P = 15
            v3Yuv[1] = (uint16_t)(v3Yuv[1] + (pGdCtrlDr->v3Rgb2YuvOff[1] << 1));
            v3Yuv[2] = (uint16_t)(v3Yuv[2] + (pGdCtrlDr->v3Rgb2YuvOff[2] << 1));
        }

        // rebuild lut
        for (i = 0; i < 3; ++i)
        {
            e = (uint16_t)CLAMPS(pV[i], 0, 0xFFFF);

#     if USE_12BITS_IN_3D_LUT
            e = e >> 4;
#     elif USE_12BITS_MSB_IN_3D_LUT
            e = e & 0xfff0;
#     endif

            *lutMap++ = e;
        }

        lutMapA += 3;
        lutMapB += 3;
        lutEntryNum -= 3;
    }
}
#endif //EN_GLOBAL_DIMMING

void UpdateKsOMapTmm(const HDmCfgFxp_t pDmCfg,
#                    if EN_GLOBAL_DIMMING
                     int gdActive,
#                    endif
                     HDmFxp_t hDm)
{
# if HACK_I16_DELTA_MAX_MIN_HARDWARE
    HGmLut_t hGmLut;

# if EN_GLOBAL_DIMMING
    if (gdActive)
        hGmLut = hDm->gdCtrlDr.hGmLut;
    else
# endif
        hGmLut = pDmCfg->hGmLut;

    if ((hGmLut->IdxMaxC2 - hGmLut->IdxMinC2) & 0x8000)
    {
        // int16_t fail case: it shall be uint16_t
        int c;
        for (c = 0; c < 3; ++c)
        {
            hDm->hDmKs->ksIMap.m33Lms2Ipt[1][c] >>= 1;
            hDm->hDmKs->ksIMap.m33Lms2Ipt[2][c] >>= 1;
        }

        // IdxMax/MinC2/3 handle in CommitDmLut
    }
# endif

# if EN_GLOBAL_DIMMING
    if (gdActive)
    {
        GdUpdateGmLut(pDmCfg, hDm, hDm->hDmKs->ksOMap.ksGmLut.lutMap);
        CommitDmLut(hDm->gdCtrlDr.hGmLut, &hDm->hDmKs->ksOMap.ksGmLut);
        return;
    }
    else
# endif
        CommitDmLut(pDmCfg->hGmLut, &hDm->hDmKs->ksOMap.ksGmLut);
    //// control to kernel
    MemCpyByte(hDm->hDmKs->ksOMap.ksGmLut.lutMap, pDmCfg->hGmLut->LutMap,
               3*pDmCfg->hGmLut->DimC1*pDmCfg->hGmLut->DimC2*pDmCfg->hGmLut->DimC3*sizeof(*hDm->hDmKs->ksOMap.ksGmLut.lutMap));
}

static int CommitKsOMap(const HDmCfgFxp_t pDmCfg, int updateTmm, HDmFxp_t hDm)
{
    Dm2KsOMapFxp_t  *pKsOMap = &hDm->hDmKs->ksOMap;
    const TgtSigEnvFxp_t *pTgtSigEnv = &pDmCfg->tgtSigEnv;
    int32_t outBits = pTgtSigEnv->Bdp;
    int32_t offset[3];

    pKsOMap->gain = pDmCfg->tmCtrl.HGain;
    pKsOMap->offset = pDmCfg->tmCtrl.HOffset;

    if (updateTmm)
    {
        // if called from CommitMds() and EN_GLOBAL_DIMMING != 0, updateTmm == 0
        // so the LOCAL_GD_SETTING(pDmCfg->gdCtrl.GdOn) is OK too
        UpdateKsOMapTmm(pDmCfg,
                        LOCAL_GD_SETTING(pDmCfg->gdCtrl.GdOn)
                        hDm);
    }

    pKsOMap->bdp = outBits;

    GetRange(outBits,
             (pTgtSigEnv->Clr != CClrRgb && pTgtSigEnv->Clr != CClrRgba) ? CRngFull : pTgtSigEnv->Rng,
             pDmCfg->tgtSigEnv.Eotf,
             &pKsOMap->tRangeMin, &pKsOMap->tRange);
    pKsOMap->tRangeOverOne = (pKsOMap->tRange << (32 - outBits)) / DLB_UINT_MAX(16);
    pKsOMap->tRangeInv = DLB_UINT_MAX(32)/(pKsOMap->tRange<<(16 - 16));

    //// rgb=>yuv
    pKsOMap->clr = CLR_C2K(pDmCfg->tgtSigEnv.Clr);

# if EN_KS_DM_IN
    if (pTgtSigEnv->Rgb2YuvExt)
    {
        AssignM33I2I(pTgtSigEnv->M33Rgb2Yuv, pKsOMap->m33Rgb2Yuv);
        pKsOMap->m33Rgb2YuvScale2P = pTgtSigEnv->M33Rgb2YuvScale2P;
    }
    else
# endif
        if (pTgtSigEnv->Rng != CRngNarrow)
            pKsOMap->m33Rgb2YuvScale2P = GetRgb2YuvM33Fxp(pTgtSigEnv->YuvXferSpec, pKsOMap->m33Rgb2Yuv);
        else
            pKsOMap->m33Rgb2YuvScale2P = GetRgb2YuvM33NarrowFxp(pDmCfg->tgtSigEnv.YuvXferSpec, outBits, pKsOMap->m33Rgb2Yuv);

# if EN_KS_DM_IN
    if (pTgtSigEnv->Rgb2YuvOffExt)
    {
        AssignV3I2I(pTgtSigEnv->V3Rgb2YuvOff, offset);
        pKsOMap->v3Rgb2YuvOff[0] = (uint32_t)offset[0] >> (15 - outBits); // config use scale 15
        pKsOMap->v3Rgb2YuvOff[1] = (uint32_t)offset[1] >> (15 - outBits);
        pKsOMap->v3Rgb2YuvOff[2] = (uint32_t)offset[2] >> (15 - outBits);
    }
    else
# endif
    {
        GetYuvRgbOffFxp(pTgtSigEnv->Rng, outBits, offset);
        pKsOMap->v3Rgb2YuvOff[0] = (uint32_t)offset[0];
        pKsOMap->v3Rgb2YuvOff[1] = (uint32_t)offset[1];
        pKsOMap->v3Rgb2YuvOff[2] = (uint32_t)offset[2];
    }
    return 0;
}

#else //DM_VER_LOWER_THAN212


//// from info of DM3 TM/OMap stored in hDm->tcCtrl/hDm->oMapCtrl to build and commit a dm2 gmLut

// designed for 1024 lut size
void C_PQ2L(uint16_t u16_x0, uint16_t u16_x1, uint16_t u16_x2, uint32_t *u32_x0, uint32_t *u32_x1, uint32_t *u32_x2)
{
    uint32_t y_1, y_2;
    int16_t iIdx;
    int16_t frac;

    iIdx = u16_x0 >> 6; // 10 bits index
    frac = u16_x0 & 0x3f;
    y_1 = depq1024Lut[iIdx];
    if (iIdx != (1<<10) - 1)
    {
        y_2 = depq1024Lut[iIdx + 1];
        y_1 += ((int64_t)(y_2 - y_1) * frac) >> 6; // interpolate using LSB
    }
    *u32_x0 = y_1;

    iIdx = u16_x1 >> 6; // 10 bits index
    frac = u16_x1 & 0x3f;
    y_1 = depq1024Lut[iIdx];
    if (iIdx != (1<<10) - 1)
    {
        y_2 = depq1024Lut[iIdx + 1];
        y_1 += ((int64_t)(y_2 - y_1) * frac) >> 6; // interpolate using LSB
    }
    *u32_x1 = y_1;

    iIdx = u16_x2 >> 6; // 10 bits index
    frac = u16_x2 & 0x3f;
    y_1 = depq1024Lut[iIdx];
    if (iIdx != (1<<10) - 1)
    {
        y_2 = depq1024Lut[iIdx + 1];
        y_1 += ((int64_t)(y_2 - y_1) * frac) >> 6; // interpolate using LSB
    }
    *u32_x2 = y_1;
}

int16_t FindNodeIdxL2NL(const uint32_t *lutx, uint32_t l)
{
    int16_t il = 0, num = DEF_L2G_LUT_NODES;// num = 2^n
    int16_t ic;
    // do a binary search
    while (num > 1)
    {
        num >>= 1;
        ic = il + num;
        if (l < lutx[ic]) {}
        else if (l > lutx[ic]) il = ic;
        else return ic;
    }

    return il;
}

// loadable lut linear to none-linear
void C_L2NL(const Dm3OMapFxp_t *pKsOMap, uint32_t u32_x0, uint32_t u32_x1, uint32_t u32_x2, uint16_t *u16_x0, uint16_t *u16_x1, uint16_t *u16_x2)
{
    int16_t index;
    int32_t i32_x0, i32_x1, i32_x2;

    index = FindNodeIdxL2NL(pKsOMap->l2nlLutX, u32_x0);
    if (u32_x0 < pKsOMap->l2nlLutX[index])
        i32_x0 = pKsOMap->l2nlLutB[index];
    else
        i32_x0 = (int32_t)(((u32_x0 - (int64_t)pKsOMap->l2nlLutX[index]) * pKsOMap->l2nlLutA[index]) >> (pKsOMap->l2nlLutXScale2P + pKsOMap->l2nlLutAScale2P - pKsOMap->l2nlLutBScale2P)) + pKsOMap->l2nlLutB[index];
    *u16_x0 = (uint16_t)CLAMPS(i32_x0, 0, DLB_UINT_MAX(16));

    index = FindNodeIdxL2NL(pKsOMap->l2nlLutX, u32_x1);
    if (u32_x1 < pKsOMap->l2nlLutX[index])
        i32_x1 = pKsOMap->l2nlLutB[index];
    else
        i32_x1 = (int32_t)(((u32_x1 - (int64_t)pKsOMap->l2nlLutX[index]) * pKsOMap->l2nlLutA[index]) >> (pKsOMap->l2nlLutXScale2P + pKsOMap->l2nlLutAScale2P - pKsOMap->l2nlLutBScale2P)) + pKsOMap->l2nlLutB[index];
    *u16_x1 = (uint16_t)CLAMPS(i32_x1, 0, DLB_UINT_MAX(16));

    index = FindNodeIdxL2NL(pKsOMap->l2nlLutX, u32_x2);
    if (u32_x2 < pKsOMap->l2nlLutX[index])
        i32_x2 = pKsOMap->l2nlLutB[index];
    else
        i32_x2 = (int32_t)(((u32_x2 - (int64_t)pKsOMap->l2nlLutX[index]) * pKsOMap->l2nlLutA[index]) >> (pKsOMap->l2nlLutXScale2P + pKsOMap->l2nlLutAScale2P - pKsOMap->l2nlLutBScale2P)) + pKsOMap->l2nlLutB[index];
    *u16_x2 = (uint16_t)CLAMPS(i32_x2, 0, DLB_UINT_MAX(16));
}

static void C_OutputMapping(const Dm3OMapFxp_t *pKsOMap, int16_t *x0, int16_t *x1, int16_t *x2)
{
    uint16_t u16_x0, u16_x1, u16_x2;
    int16_t i16_x0, i16_x1, i16_x2;
    int32_t i32_x0, i32_x1, i32_x2;
    uint32_t u32_x0, u32_x1, u32_x2;
    int64_t i64_y0, i64_y1, i64_y2;
    uint16_t outBits = pKsOMap->oetfParam.bdp;

    // Input:  1.15 signed
    // Output: 0.16 unsigned

    i16_x0 = *x0;
    i16_x1 = *x1;
    i16_x2 = *x2;

    // IPT =>LMS x => y
    i32_x0 = ((int32_t)pKsOMap->m33Ipt2Lms[0][0] * i16_x0 + (int32_t)pKsOMap->m33Ipt2Lms[0][1] * i16_x1 + (int32_t)pKsOMap->m33Ipt2Lms[0][2] * i16_x2) >> (pKsOMap->m33Ipt2LmsScale2P - 1);
    i32_x1 = ((int32_t)pKsOMap->m33Ipt2Lms[1][0] * i16_x0 + (int32_t)pKsOMap->m33Ipt2Lms[1][1] * i16_x1 + (int32_t)pKsOMap->m33Ipt2Lms[1][2] * i16_x2) >> (pKsOMap->m33Ipt2LmsScale2P - 1);
    i32_x2 = ((int32_t)pKsOMap->m33Ipt2Lms[2][0] * i16_x0 + (int32_t)pKsOMap->m33Ipt2Lms[2][1] * i16_x1 + (int32_t)pKsOMap->m33Ipt2Lms[2][2] * i16_x2) >> (pKsOMap->m33Ipt2LmsScale2P - 1);
    u16_x0 = (uint16_t)CLAMPS(i32_x0, 0, DLB_UINT_MAX(16));
    u16_x1 = (uint16_t)CLAMPS(i32_x1, 0, DLB_UINT_MAX(16));
    u16_x2 = (uint16_t)CLAMPS(i32_x2, 0, DLB_UINT_MAX(16));

    // Input:  0.16 unsigned
    // Output: 14.18 signed

    ////// map to target space
    //// PQ=>L
    C_PQ2L(u16_x0, u16_x1, u16_x2, &u32_x0, &u32_x1, &u32_x2);

    //// LMS => RGB, y=>x
    i64_y0 = ((int64_t)pKsOMap->m33Lms2Rgb[0][0] * u32_x0 + (int64_t)pKsOMap->m33Lms2Rgb[0][1] * u32_x1 + (int64_t)pKsOMap->m33Lms2Rgb[0][2] * u32_x2) >> pKsOMap->m33Lms2RgbScale2P;
    i64_y1 = ((int64_t)pKsOMap->m33Lms2Rgb[1][0] * u32_x0 + (int64_t)pKsOMap->m33Lms2Rgb[1][1] * u32_x1 + (int64_t)pKsOMap->m33Lms2Rgb[1][2] * u32_x2) >> pKsOMap->m33Lms2RgbScale2P;
    i64_y2 = ((int64_t)pKsOMap->m33Lms2Rgb[2][0] * u32_x0 + (int64_t)pKsOMap->m33Lms2Rgb[2][1] * u32_x1 + (int64_t)pKsOMap->m33Lms2Rgb[2][2] * u32_x2) >> pKsOMap->m33Lms2RgbScale2P;

    // linear space clamping
    u32_x0 = (uint32_t)CLAMPS(i64_y0, pKsOMap->oetfParam.min, pKsOMap->oetfParam.max);
    u32_x1 = (uint32_t)CLAMPS(i64_y1, pKsOMap->oetfParam.min, pKsOMap->oetfParam.max);
    u32_x2 = (uint32_t)CLAMPS(i64_y2, pKsOMap->oetfParam.min, pKsOMap->oetfParam.max);

    // Input:  14.18 signed
    // Output: 0.12 unsigned

    // BT1886/Power or PQ
    i64_y0 = (((int64_t)u32_x0 * pKsOMap->preMapA) >> 18) + pKsOMap->preMapB;
    i64_y1 = (((int64_t)u32_x1 * pKsOMap->preMapA) >> 18) + pKsOMap->preMapB;
    i64_y2 = (((int64_t)u32_x2 * pKsOMap->preMapA) >> 18) + pKsOMap->preMapB;
    u32_x0 = (uint32_t)CLAMPS(i64_y0, 0, DLB_UINT_MAX(32));
    u32_x1 = (uint32_t)CLAMPS(i64_y1, 0, DLB_UINT_MAX(32));
    u32_x2 = (uint32_t)CLAMPS(i64_y2, 0, DLB_UINT_MAX(32));

    C_L2NL(pKsOMap, u32_x0, u32_x1, u32_x2, &u16_x0, &u16_x1, &u16_x2);

    i32_x0 = (((int64_t)u16_x0 * pKsOMap->postMapA) >> 16) + pKsOMap->postMapB;
    i32_x1 = (((int64_t)u16_x1 * pKsOMap->postMapA) >> 16) + pKsOMap->postMapB;
    i32_x2 = (((int64_t)u16_x2 * pKsOMap->postMapA) >> 16) + pKsOMap->postMapB;
    u16_x0 = (uint16_t)CLAMPS(i32_x0, 0, DLB_UINT_MAX(16));
    u16_x1 = (uint16_t)CLAMPS(i32_x1, 0, DLB_UINT_MAX(16));
    u16_x2 = (uint16_t)CLAMPS(i32_x2, 0, DLB_UINT_MAX(16));

    i16_x0 = u16_x0 >> (16 - outBits);
    i16_x1 = u16_x1 >> (16 - outBits);
    i16_x2 = u16_x2 >> (16 - outBits);

    //// de-normal
    i16_x0 = (((int32_t)i16_x0 * pKsOMap->oetfParam.rangeOverOne) >> 16) + pKsOMap->oetfParam.rangeMin;
    i16_x1 = (((int32_t)i16_x1 * pKsOMap->oetfParam.rangeOverOne) >> 16) + pKsOMap->oetfParam.rangeMin;
    i16_x2 = (((int32_t)i16_x2 * pKsOMap->oetfParam.rangeOverOne) >> 16) + pKsOMap->oetfParam.rangeMin;

    if (pKsOMap->clr== KClrYuv)
    {
        i32_x0 = (pKsOMap->m33Rgb2Yuv[0][0] * (int32_t)i16_x0 + pKsOMap->m33Rgb2Yuv[0][1] * (int32_t)i16_x1 + pKsOMap->m33Rgb2Yuv[0][2] * (int32_t)i16_x2) >> pKsOMap->m33Rgb2YuvScale2P;
        i32_x1 = (pKsOMap->m33Rgb2Yuv[1][0] * (int32_t)i16_x0 + pKsOMap->m33Rgb2Yuv[1][1] * (int32_t)i16_x1 + pKsOMap->m33Rgb2Yuv[1][2] * (int32_t)i16_x2) >> pKsOMap->m33Rgb2YuvScale2P;
        i32_x2 = (pKsOMap->m33Rgb2Yuv[2][0] * (int32_t)i16_x0 + pKsOMap->m33Rgb2Yuv[2][1] * (int32_t)i16_x1 + pKsOMap->m33Rgb2Yuv[2][2] * (int32_t)i16_x2) >> pKsOMap->m33Rgb2YuvScale2P;

        u16_x0 = (uint16_t)(i32_x0 + pKsOMap->v3Rgb2YuvOff[0]);
        u16_x1 = (uint16_t)(i32_x1 + pKsOMap->v3Rgb2YuvOff[1]);
        u16_x2 = (uint16_t)(i32_x2 + pKsOMap->v3Rgb2YuvOff[2]);
    }
    else if (pKsOMap->clr == KClrRgb)
    {
        u16_x0 = i16_x0;
        u16_x1 = i16_x1;
        u16_x2 = i16_x2;
    }

    *x0 = u16_x0;
    *x1 = u16_x1;
    *x2 = u16_x2;
}

# if DM_VER_HIGHER_THAN211
static int32_t SmIFwd(int32_t x, int32_t adm, const TcCtrlFxp_t *pTcCtrl)
{
    int32_t smIOrg = (pTcCtrl->level2[TrimTypeSatGain] * (adm - x + 32768)) >> 12;
    int32_t dltMinPq = pTcCtrl->tcTgtSig.minPq - pTcCtrl->tcSrcSig.minPq; // scale in 12

    if (dltMinPq <= 0)
    {
        x = smIOrg;
    }
    else
    {
        int32_t ltemp0 = adm >> 3;
        int64_t ltemp2;
        int32_t ltemp3 = ((uint32_t)1 << (30 - 1)) / (pTcCtrl->tcTgtSig.maxPq - pTcCtrl->tcTgtSig.minPq); // 0.5/(tMaxPq-tMinPq), scale 30-12=18
        int32_t ltemp4 = (int32_t)(4294967296 / 92.245708994065268); // scale in 32

        // Reduce saturation of darks when mapping to higher black level
        ltemp2 = 3 * (int64_t)dltMinPq * PQToL((((pTcCtrl->tcTgtSig.maxPq - ltemp0) * ltemp3) + (1 << 13)) >> 14); // scale in 18+12
        ltemp2 = ((ltemp2 * ltemp4) + ((int64_t)1 << (32 + 15 - 1))) >> (32 + 15); // scale in 15
        if (ltemp2 >= 32768)
            x = 0;
        else
            x = (int32_t)(((smIOrg * (32768 - ltemp2)) + (1 << 14)) >> 15);
    }

    x = MIN2S(x, 65535);

    return x;
}

static int32_t SmIBwd(int32_t x, int32_t adm, const TcCtrlFxp_t *pTcCtrl)
{
    x = (pTcCtrl->level2[TrimTypeSatGain] << 18) / (adm - x + 32768);
    x = CLAMPS(x, 0, 65535);

    return x;
}

static int32_t TmSFwd(int32_t x, const TcCtrlFxp_t *pTcCtrl)
{
    x = 32768 - (((int64_t)x * pTcCtrl->intensityVectorWeight * pTcCtrl->ratio) >> (12 + QB02DATAIN));
    x = CLAMPS(x, 0, 65535);

    return x;
}

static int32_t TmSBwd(int32_t x, const TcCtrlFxp_t *pTcCtrl)
{
    // invertible case shall already set intensityVectorWeight = 0
    x = 32768 + (((int64_t)x* pTcCtrl->intensityVectorWeight * pTcCtrl->ratio) >> (12 + QB02DATAIN));
    x = CLAMPS(x, 0, 65535);

    return x;
}

static int32_t SmSFwd(int32_t x, const TcCtrlFxp_t *pTcCtrl)
{
    x = 32768 - (((int64_t)x * pTcCtrl->chrmVectorWeight * pTcCtrl->ratio) >> (12 + QB02DATAIN));
    x = CLAMPS(x, 0, 65535);

    return x;
}

static int32_t SmSBwd(int32_t x, const TcCtrlFxp_t *pTcCtrl)
{
    // invertible case shall already set chrmVectorWeight = 0
    x = 32768 + (((int64_t)x * pTcCtrl->chrmVectorWeight * pTcCtrl->ratio) >> (12 + QB02DATAIN));
    x = CLAMPS(x, 0, 65535);

    return x;
}
#endif //# if DM_VER_HIGHER_THAN211

// inverse look up of a equal-space sampled lut, like those used in TM
static int16_t InvLUT1D(const int16_t *lut, int16_t lutSizeM1, int32_t adm)
{
    int16_t i0 = 0;
    int16_t i1 = lutSizeM1;
    int16_t i;

    do
    {
        i = (i0 + i1) >> 1;
        if (adm >= ((int32_t)lut[i] << 3))
            i0 = i;
        else
            i1 = i;
    }
    while (i1 > i0 + 1);

    return (lut[i1] != lut[i0]) ? ((((adm - ((int32_t)lut[i0] << 3)) << 15) / (lut[i1] - lut[i0])) >> 12) + (i0 << 6) : i1 << 6;
}

#define EN_DM_FLAT_OUTPUT   1

// for now, just take case of the first and last part clamping case
static int CacheAdmI(const DmKs2GmLutFxp_t *pKsGmLut, const TcCtrlFxp_t *pTcCtrl, int32_t *tmI, int32_t *smI)
{
    int16_t flatOut = pTcCtrl->tmLutI[0] == pTcCtrl->tmLutI[pTcCtrl->tmLutISizeM1];
    int16_t idx, ldx;
    int16_t i, iMin, iMax;

    if (flatOut)
    {
        // dm output constant case
#   if !EN_DM_FLAT_OUTPUT
        // the most possible input
        iMin = iMax = (pTcCtrl->tcLutDir != CTcLutDirBwd) ? pTcCtrl->tcSrcSig.mid : pTcCtrl->tcTgtSig.mid;
#   else
        return 1;
#   endif
    }
    else
    {
        // clamping at both ends
        for (ldx = 0; pTcCtrl->tmLutI[ldx] == pTcCtrl->tmLutI[ldx+1]; ++ldx);
        iMin = ldx << 3;

        for (ldx = pTcCtrl->tmLutISizeM1; pTcCtrl->tmLutI[ldx] == pTcCtrl->tmLutI[ldx-1]; --ldx);
        iMax = ldx << 3;
    }

    for (idx = 0; idx < GMLUT_MAX_DIM; ++idx)
    {
        tmI[idx] = pKsGmLut->iMinC1 + (idx * ((((int32_t)pKsGmLut->iMaxC1 - pKsGmLut->iMinC1) << 15) / 16) >> 15);

        // invert back to I
        if (flatOut)
        {
            // always back to mid point
            i = iMin << 3;
        }
        else
        {
            if (tmI[idx] <= (pTcCtrl->tmLutI[0] << 3))
            {
                i = iMin << 3;
            }
            else if (tmI[idx] >= (pTcCtrl->tmLutI[pTcCtrl->tmLutISizeM1] << 3))
            {
                i = iMax << 3;
            }
            else
            {
                i = InvLUT1D(pTcCtrl->tmLutI, pTcCtrl->tmLutISizeM1, tmI[idx]);
            }
        }

        if (pTcCtrl->tcLutDir == CTcLutDirFwd)
        {
            smI[idx] = SmIFwd(i, tmI[idx], pTcCtrl);
        }
        else if (pTcCtrl->tcLutDir == CTcLutDirBwd)
        {
            smI[idx] = SmIBwd(tmI[idx], i, pTcCtrl);
        }
        else
        {
            smI[idx] = 32767;
        }
    }

    return 0;
}

static int CommitGmLutFromDm3(int tBdp, const TcCtrlFxp_t *pTcCtrl, const Dm3OMapFxp_t *pDm3OMap, DmKs2GmLutFxp_t *pKsGmLut)
{
    uint16_t *pIpt = pKsGmLut->lutMap;
    uint32_t i, p, t,k,j;
    int32_t tmICh[GMLUT_MAX_DIM], smICh[GMLUT_MAX_DIM];

    // gmLut params
    pKsGmLut->dimC1 = GMLUT_MAX_DIM;
    pKsGmLut->dimC2 = GMLUT_MAX_DIM;
    pKsGmLut->dimC3 = GMLUT_MAX_DIM;

    if (pDm3OMap->clr == KClrYuv)
        pKsGmLut->valTp = (uint16_t)GmLutTypeIpt2Yuv;
    else
        pKsGmLut->valTp = (uint16_t)GmLutTypeIpt2Rgb;

    pKsGmLut->iMinC1 = LToPQ15(pDm3OMap->oetfParam.min);
    pKsGmLut->iMaxC1 = LToPQ15(pDm3OMap->oetfParam.max);
    pKsGmLut->iMinC2 = -16384;
    pKsGmLut->iMaxC2 = 16384;
    pKsGmLut->iMinC3 = -16384;
    pKsGmLut->iMaxC3 = 16384;
    pKsGmLut->iDistC1Inv = (int32_t)((((uint32_t)1<<30) / (pKsGmLut->iMaxC1 - pKsGmLut->iMinC1)));
    pKsGmLut->iDistC2Inv = (int32_t)((((uint32_t)1<<30) / (pKsGmLut->iMaxC2 - pKsGmLut->iMinC2)));
    pKsGmLut->iDistC3Inv = (int32_t)((((uint32_t)1<<30) / (pKsGmLut->iMaxC3 - pKsGmLut->iMinC3)));

    // pre-calculated  value
    pKsGmLut->pitch = pKsGmLut->dimC3;
    pKsGmLut->slice = pKsGmLut->dimC2 * pKsGmLut->dimC3;



//printf("pTcCtrl->tcLutDi = 0x%x\n",pTcCtrl->tcLutDi);
//printf("pTcCtrl->intensityVectorWeighti = 0x%x\n",pTcCtrl->intensityVectorWeighti);
    //// pre calculated the adm value and linear, x0, value
    if (CacheAdmI(pKsGmLut, pTcCtrl, tmICh, smICh))
    {
        // flat output case
        //// the dm3 adm
        int16_t x0  = pTcCtrl->tmLutI[0] << 3;
        int16_t x1 = 0;
        int16_t x2 = 0;

        C_OutputMapping(pDm3OMap, &x0, &x1, &x2);

        for (t = 0; t < GMLUT_MAX_DIM; ++t) for (p = 0; p < GMLUT_MAX_DIM; ++p) for (i = 0; i < GMLUT_MAX_DIM; ++i)
                {
#     if USE_12BITS_IN_3D_LUT
                    *pIpt++ = x0;
                    *pIpt++ = x1;
                    *pIpt++ = x2;
#     else
                    *pIpt++ = x0 << 4;
                    *pIpt++ = x1 << 4;
                    *pIpt++ = x2 << 4;
#     endif
                }
    }
    else
    {
        for (t = 0; t < GMLUT_MAX_DIM; ++t)
        {
            int32_t x2In = pKsGmLut->iMinC3 + t * 2048;
            if (x2In >= 32768) x2In = 32767;
            for (p = 0; p < GMLUT_MAX_DIM; ++p)
            {
                int32_t x1In = pKsGmLut->iMinC2 + p * 2048;
                int32_t tmS, smS;
                int32_t sat = ((int64_t)x1In * x1In + (int64_t)x2In * x2In) >> (15 + 0); // [0, 2]=>[0, 1]

                if (x1In >= 32768) x1In = 32767;
                sat = (sat >= 32768) ? 32767 : sat;

                if (pTcCtrl->tcLutDir == CTcLutDirFwd)
                {
                    tmS = TmSFwd(sat, pTcCtrl);
                    smS = SmSFwd(sat, pTcCtrl);
                }
                else if (pTcCtrl->tcLutDir == CTcLutDirBwd)
                {
                    tmS = TmSBwd(sat, pTcCtrl);
                    smS = SmSBwd(sat, pTcCtrl);
                }
                else
                {
                    tmS = 32767;
                    smS = 32767;
                }

                for (i = 0; i < GMLUT_MAX_DIM; ++i)
                {
                    //// the dm3 adm
                    int16_t x0  = (tmICh[i] * tmS) >> 15;
                    // get the ratio for P, T adj
                    const int32_t s = (smICh[i] * smS) >> 15;

                    int16_t x1 = (x1In * s) >> 15;
                    int16_t x2 = (x2In * s) >> 15;
                    x1 = CLAMPS(x1, -(1<<15), (int32_t)DLB_UINT_MAX(15));
                    x2 = CLAMPS(x2, -(1<<15), (int32_t)DLB_UINT_MAX(15));
                    x0 = CLAMPS(x0, 0, (int32_t)DLB_UINT_MAX(15));

                    C_OutputMapping(pDm3OMap, &x0, &x1, &x2);

#       if USE_12BITS_IN_3D_LUT
                    *pIpt++ = x0;
                    *pIpt++ = x1;
                    *pIpt++ = x2;
#       else
                    *pIpt++ = x0 << 4;
                    *pIpt++ = x1 << 4;
                    *pIpt++ = x2 << 4;
#       endif
                }
            }
        }
    }
    return 0;
}

int CommitKsOMapDm2FromDm3(const HDmCfgFxp_t pDmCfg, HDmFxp_t hDm)
{
    Dm2KsOMapFxp_t *pKsOMap = &hDm->hDmKs->ksOMap;
    DEFINE_DM3OMAP_PTR(hDm, pDm3OMap);
    TgtSigEnvFxp_t *pTgtSigEnv = &pDmCfg->tgtSigEnv;

    pKsOMap->gain = 4095;
    pKsOMap->offset = 1;

    pKsOMap->satGain = 4095;

    // use 12 bit in 3D lut always
    pDm3OMap->oetfParam.bdp = 12;
    GetRange(pDm3OMap->oetfParam.bdp,
             (pTgtSigEnv->Clr != CClrRgb && pTgtSigEnv->Clr != CClrRgba) ? CRngFull : pTgtSigEnv->Rng,
             pTgtSigEnv->Eotf,
             &pDm3OMap->oetfParam.rangeMin, &pDm3OMap->oetfParam.range);
    // range conversion in 0.16
    pDm3OMap->oetfParam.rangeOverOne = ((uint32_t)pDm3OMap->oetfParam.range << 16) / ((1 << pDm3OMap->oetfParam.bdp) - 1); // scale up by 2^16

    // if called from CommitMds() and EN_GLOBAL_DIMMING != 0, updateTmm == 0
    // so the LOCAL_GD_SETTING(pDmCfg->gdCtrl.GdOn) is OK too
    //static struct timespec NowTS;
    //static struct timespec previousTS;

    // getnstimeofday(&previousTS);
    CommitGmLutFromDm3(pTgtSigEnv->Bdp, &hDm->tcCtrl, pDm3OMap, &pKsOMap->ksGmLut);

    // getnstimeofday(&NowTS);
    //long diff = NowTS.tv_sec * 1000000 + NowTS.tv_nsec/1000 - previousTS.tv_sec * 1000000 - previousTS.tv_nsec/1000;
    //printf("3D LUT calculation time = %d micro second\n",diff);


    // a simple scale down from 16 bits to pTgtSigEnv->Bdp
# if USE_12BITS_IN_3D_LUT
//  pKsOMap->downScale = ((FloatComp_t)(1<<pTgtSigEnv->Bdp))/(1 << 12);
# else
//  pKsOMap->downScale = ((FloatComp_t)(1<<pTgtSigEnv->Bdp))/(1 << 16);
# endif

    pKsOMap->bdp = pTgtSigEnv->Bdp;

    // final output space
    pKsOMap->tRangeMin = pDm3OMap->oetfParam.rangeMin;
    pKsOMap->tRange = pDm3OMap->oetfParam.range;
    pKsOMap->tRangeOverOne = (pKsOMap->tRange << (32 - pKsOMap->bdp)) / DLB_UINT_MAX(16);

    //// rgb=>yuv
    pKsOMap->clr = CLR_C2K(pTgtSigEnv->Clr);

    AssignM33I2I((const int16_t (*)[3])pDm3OMap->m33Rgb2Yuv, pKsOMap->m33Rgb2Yuv);

    AssignV3I2I(pDm3OMap->v3Rgb2YuvOff, pKsOMap->v3Rgb2YuvOff);

    return 0;
}
#endif //DM_VER_LOWER_THAN212



# if EN_MS_OPTION
static const uint16_t msFilterScale = 12;
static const int16_t msFilterRow[5 + 1] =
{
    822,725,498,267,111,36
};
static const int16_t msFilterCol[2 + 1] =
{
    1650,1000,223
};
static const int16_t msFilterEdgeRow[5 + 1] =
{
    0,-725,-997,-800,-445,-180
};
static const int16_t msFilterEdgeCol[2 + 1] =
{
    0,-1000,-446
};

static void CommitMsCtrl(const MsCtrlFxp_t *pMsCtrl, HDmFxp_t hDm)
{
    DmKsMsFxp_t *pKsMs = &hDm->hDmKs->ksMs;
# if DM_VER_HIGHER_THAN211
    int idx;
    int16_t *pf;

    pKsMs->fltrScale = msFilterScale;

    pKsMs->fltrRadius[0] = 5;
    pKsMs->fltrRadius[1] = 2;

    pf = pKsMs->fltrLp[0];
    for (idx = 0; idx <= pKsMs->fltrRadius[0]; ++idx)
    {
        pf[idx] = msFilterRow[idx];
    }
    pf = pKsMs->fltrLp[1];
    for (idx = 0; idx <= pKsMs->fltrRadius[1]; ++idx)
    {
        pf[idx] = msFilterCol[idx];
    }

# if !EN_EDGE_SIMPLE
    pf = pKsMs->fltrHp[0];
    for (idx = 0; idx <= pKsMs->fltrRadius[0]; ++idx)
    {
        pf[idx] = msFilterEdgeRow[idx];
    }
    pf = pKsMs->fltrHp[1];
    for (idx = 0; idx <= pKsMs->fltrRadius[1]; ++idx)
    {
        pf[idx] = msFilterEdgeCol[idx];
    }
# endif
# endif

    pKsMs->msEdgeWeight = pMsCtrl->MsEdgeWeight;
    // pKsMs->msWeight move to the ugly part, to be consistent with mds case
    pKsMs->disMs = pMsCtrl->MsMethod == CMsMethodOff;
}
#endif // #if EN_MS_OPTION

static int CommitDmCtrl(const DmCtrlFxp_t *pDmCtrl, DmKsCtrl_t *pKsDmCtrl)
{
    pKsDmCtrl->mainIn = 1;
    (void)pDmCtrl;

# if EN_AOI
    pKsDmCtrl->aoiRow0 = pDmCtrl->AoiRow0;
    pKsDmCtrl->aoiRow1Plus1 = pDmCtrl->AoiRow1Plus1;

    pKsDmCtrl->aoiCol0 = pDmCtrl->AoiCol0;
    pKsDmCtrl->aoiCol1Plus1 = pDmCtrl->AoiCol1Plus1;
# endif

    //platform =  pDmCtrl->Platform;
    pKsDmCtrl->prf = pDmCtrl->Prf;

    return 0;
}

static void CommitKsCtrls(const TcCtrlFxp_t *pTcCtrl, DmKsFxp_t *pDmKs)
{
# if EN_MS_OPTION
    pDmKs->ksMs.msWeight = (uint16_t)pTcCtrl->level2[TrimTypeMsWeight];
    // to make -(msWeight - 1) into -msWeight in data plan, level2[TrimTypeMsWeight] has -1
# endif // EN_MS_OPTION


# if DM_VER_LOWER_THAN212
    pDmKs->ksOMap.satGain = (uint16_t)pTcCtrl->level2[TrimTypeSatGain];
    pDmKs->ksTMap.chromaWeight = (uint16_t)pTcCtrl->level2[TrimTypeChromaWeight];
# else
    pDmKs->ksOMap.satGain = 4096;
    pDmKs->ksTMap.chromaWeight = 0;
# endif
    (void)pDmKs;
    (void)pTcCtrl;
}

#if DM_FULL_SRC_SPEC ||  DM_SEC_INPUT
static void IntTcSigStaticCcm(TcSigFxp_t *pTcSig)
{
    pTcSig->crush = pTcSig->minPq;
    pTcSig->clip = pTcSig->maxPq;
    pTcSig->mid = (pTcSig->minPq + pTcSig->maxPq)/2;
}
#endif

#if EN_RUN_MODE
# define GET_BYPASS_SHIFT(inBits, outBits, bypassShift)                         \
    bypassShift = ((inBits) - (outBits))
#else
# define GET_BYPASS_SHIFT(inBits, outBits, bypassShift)
#endif


static void DeriveTcLutDir(TcCtrlFxp_t *pTcCtrl
# if EN_RUN_MODE
                           , int runMode
# endif
                          )
{
    // treat runMode != 0(not normal, in fact if runMode == 2, a sepatate data plan)
    // as an add on special case, disturb normal code as less as little as possible
# define PQ_RES   1 /*(1.0/(1<<DM_PQ_SCALE_2P)) // to allow for 1 quantization difference*/

    pTcCtrl->tcLutDir =
#   if EN_RUN_MODE
        (runMode == 1) ? CTcLutDirPass :
#   endif
#   if DM_SOFTWARE_BYPASS
        (runMode == 2) ? CTcLutDirPass :
#   endif
        (pTcCtrl->tcSrcSig.maxPq + PQ_RES <  pTcCtrl->tcTgtSig.maxPq         ) ? CTcLutDirBwd :
        (pTcCtrl->tcSrcSig.maxPq          >  pTcCtrl->tcTgtSig.maxPq + PQ_RES) ? CTcLutDirFwd :

        (pTcCtrl->tcSrcSig.minPq + PQ_RES >= pTcCtrl->tcTgtSig.minPq &&          // be conservative
         pTcCtrl->tcSrcSig.minPq          <= pTcCtrl->tcTgtSig.minPq + PQ_RES) ? CTcLutDirPass :

        (pTcCtrl->tcSrcSig.maxPq < pTcCtrl->tcTgtSig.maxPq) ? CTcLutDirBwd : CTcLutDirFwd;
}


int CommitDmCfg(const HDmCfgFxp_t pDmCfg, HDmKsFxp_t hDmKs, HDmFxp_t hDm)
{
    TcCtrlFxp_t *pTcCtrl = &hDm->tcCtrl;
    int32_t ret = 0;
# if DM_EXTERNAL_CCM
    TcSigFxp_t *pTcSrcSig;
# endif
    DEF_FRAME_BUFFER_COMP_NUM(compNum, pDmCfg->dmCtrl.Platform != CPlatformCpu);

    if (!hDm)
    {
        return -1;
    }

    // check if cfg is changed
    if (MemEqByte(pDmCfg, &hDm->dmCfg, sizeof(DmCfgFxp_t)))
    {
        //printf("no changes in CommitDmCfg, running anyway\n");
        return 0;  // cfg not changed
    }

    // invalidate cached Mds if any
    hDm->mdsExt.invalidateMds = 1;

    ret |= FLAG_CHANGE_CFG;

    hDm->hDmKs = hDmKs;

    ////// hook up the frame buffer
# if EN_UP_DOWN_SAMPLE_OPTION || EN_MS_OPTION || defined(_VDR_DM_CUDA_)
    if (compNum)
    {
        hDmKs->frmBuf1 = hDmKs->frmBuf0 + DM_MAX_IMG_SIZE;
        hDmKs->frmBuf2 = hDmKs->frmBuf1 + DM_MAX_IMG_SIZE;
#   if EN_MS_OPTION || defined(_VDR_DM_CUDA_)
        if (compNum == 4)
        {
            hDmKs->frmBuf3 = hDmKs->frmBuf2 + DM_MAX_IMG_SIZE;
        }
#   endif
    }


# endif

    // get the cfg ipt space
    if (pDmCfg->dmCtrl.Lms2IptExt)
    {
        int16_t rshift;

        MemCpyByte(&hDm->cscCtrl.m33Lms2Ipt[0][0], &pDmCfg->dmCtrl.M33Lms2Ipt[0][0], sizeof(hDm->cscCtrl.m33Lms2Ipt));

        hDm->cscCtrl.m33Lms2IptScale2P = pDmCfg->dmCtrl.M33Lms2IptScale2P;
        hDm->cscCtrl.m33Ipt2LmsScale2P = hDm->cscCtrl.m33Lms2IptScale2P;

        rshift = InvM33(pDmCfg->dmCtrl.M33Lms2Ipt, hDm->cscCtrl.m33Ipt2Lms, hDm->cscCtrl.m33Lms2IptScale2P, hDm->cscCtrl.m33Ipt2LmsScale2P);
        hDm->cscCtrl.m33Ipt2LmsScale2P -= rshift;
    }
    else
    {
        hDm->cscCtrl.m33Lms2IptScale2P = GetLms2IptM33Fxp(hDm->cscCtrl.m33Lms2Ipt);
        hDm->cscCtrl.m33Ipt2LmsScale2P = GetIpt2LmsM33Fxp(hDm->cscCtrl.m33Ipt2Lms);
    }
    CommitDmCtrl(&pDmCfg->dmCtrl, &hDmKs->ksDmCtrl);


    ////// ksFrmFmtI: simple direct conversion
    SigEnv2KsFrmFmt((const SigEnvCom_t *)&pDmCfg->srcSigEnv, &hDmKs->ksFrmFmtI);

    ////// source
    if (CommitSrcSigEnv(&pDmCfg->srcSigEnv, hDm))
    {
        return -3;
    }

    ////// tc lut
# if EN_GLOBAL_DIMMING
    // init one time
    InitGdCtrlDr(&pDmCfg->gdCtrl, &pDmCfg->tgtSigEnv, &hDm->gdCtrlDr);

    // since level4Gd related is heavy weighted, avoid it if mds must be present
# if EN_LOCAL_GD_TEST
    if (pDmCfg->gdCtrl.GdOn)
    {
        GdCtrlDrFxp_t *pGdCtrlDr = &hDm->gdCtrlDr;

        ++pGdCtrlDr->gdUdFrms;
        pGdCtrlDr->fltrdAnchorPqLstFrm = (pGdCtrlDr->gdWeightMean*pGdCtrlDr->fltrdMeanPqDft +
                                          pGdCtrlDr->gdWeightStd*pGdCtrlDr->fltrdStdPqDft) >> 12;
        if (GdDeriveEffTmm(pGdCtrlDr->fltrdAnchorPqLstFrm, pGdCtrlDr))
        {
            // global diming to TC
            Gd2TcCtrl(pGdCtrlDr, pDmCfg->tgtSigEnv.DiagSize, pTcCtrl);

            pGdCtrlDr->gdActiveLstFrm = 1;
            ret |= FLAG_CHANGE_GD;
        }
    }
    else
# endif
# endif
        TgtSigEnv2TcCtrl(&pDmCfg->tgtSigEnv, pTcCtrl); // target related

# if DM_FULL_SRC_SPEC
    SrcSigEnv2TcCtrl(&pDmCfg->srcSigEnv, pTcCtrl); // source related

    DeriveTcLutDir(pTcCtrl
# if EN_RUN_MODE
                   , pDmCfg->dmCtrl.RunMode
# endif
                  ); // must set the tcLutDir before all the Update* and TmCtrl2TcCtrl()

    TmCtrl2TcCtrl(&pDmCfg->tmCtrl, pTcCtrl, 0);  // tmCtrl related

//  UpdateTcCtrlDerived(pTcCtrl); // secondary derived staff update

    UpdateTcCtrlLevel(&pDmCfg->tmCtrl, pTcCtrl); // kind of no trim in case in CommitTrimFlt()

    // all ugly part go here
    CommitKsCtrls(pTcCtrl, hDmKs);

    // default source ccm to static
    IntTcSigStaticCcm(&pTcCtrl->tcSrcSig);
    // default target too
    IntTcSigStaticCcm(&pTcCtrl->tcTgtSig);

# if DM_EXTERNAL_CCM
    if (pDmCfg->dmCtrl.Igm && pDmCfg->dmCtrl.Ccm[1] > pDmCfg->dmCtrl.Ccm[0])
    {
        // shall always go to video source, because it is for video source only!
        // so in case of bwd mapping, it is currently not used
        pTcSrcSig = (FOLLOW_MATLAB_CCM_ALWAYS_AS_SOURCE_IN_TM && pTcCtrl->tcLutDir == CTcLutDirBwd) ?
                    &pTcCtrl->tcTgtSig : &pTcCtrl->tcSrcSig;

        pTcSrcSig->crush = pDmCfg->dmCtrl.Ccm[0];
        pTcSrcSig->clip  = pDmCfg->dmCtrl.Ccm[1];
        pTcSrcSig->mid   = pDmCfg->dmCtrl.Ccm[2];
    }
# endif

    if (CreateToneCurve(pTcCtrl, &hDmKs->ksTMap, (void *)0, &hDm->tcCtrlUsed))
        ret |= FLAG_CHANGE_TC;
# if REDUCED_TC_LUT
    {
        int i;
        hDmKs->ksTMap.tmLutMaxVal = 0;
        for (i = 0; i < 515; i++)
            if (hDmKs->ksTMap.tmInternal515Lut[i] > hDmKs->ksTMap.tmLutMaxVal)
                hDmKs->ksTMap.tmLutMaxVal = hDmKs->ksTMap.tmInternal515Lut[i];
    }
# endif

# endif // DM_FULL_SRC_SPEC


# if EN_MS_OPTION
    ////// blending
    CommitMsCtrl(&pDmCfg->msCtrl, hDm);
# if EN_RUN_MODE
    if (pDmCfg->dmCtrl.RunMode == 1
#   if DM_SOFTWARE_BYPASS
        || pDmCfg->dmCtrl.RunMode == 2
# endif
       )
    {
        // not normal: disable it
        hDmKs->ksMs.disMs = 1;
    }
    else
    {
    }
# endif
# endif

    ////// target
# if !EN_RUN_MODE
    if (CommitKsOMap(pDmCfg, 1, hDm))
    {
        return -4;
    }
# else
    // since CommitKsOMap() may perform some heavy lifting, avoid it during bypass
    // also, cluster all bypass related code here to overwrite the normal setting
    if (pDmCfg->dmCtrl.RunMode < 2)
    {
        // tcLut already taken care of
#   if EN_MS_OPTION
        hDmKs->ksMs.disMs = pDmCfg->dmCtrl.RunMode == 1;
#   endif
        if (CommitKsOMap(pDmCfg, 1, hDm))
        {
            return -4;
        }
    }
    else
    {
        // do not like it, but ...
        GET_BYPASS_SHIFT(pDmCfg->srcSigEnv.Bdp, pDmCfg->tgtSigEnv.Bdp, hDmKs->bypassShift);

# if DM_SOFTWARE_BYPASS
        // tcLut already taken care of
#   if EN_MS_OPTION
        hDmKs->ksMs.disMs = 1;
#   endif

        CommitKsIOMapBypass(CLR_C2K(pDmCfg->srcSigEnv.Clr), pDmCfg->srcSigEnv.Bdp, pDmCfg->tgtSigEnv.Bdp, hDm);
# endif
    }
# endif

    ////// ksFrmFmtO: simple direct conversion
    SigEnv2KsFrmFmt((const SigEnvCom_t *)&pDmCfg->tgtSigEnv, &hDmKs->ksFrmFmtO);

    ////// up/down. min/max
    CommitUds(pDmCfg, hDm);


    //// save the current committed cfg
    MemCpyByte(&hDm->dmCfg, pDmCfg, sizeof(*pDmCfg));

    return ret;
}


// bubble sort to ascending order according to pTrim->Trima[*][TrimTypeTMaxPq]
static void SortTrims(const Trim_t *pTrim, uint16_t *tnIdxa, uint16_t *tMaxPqa)
{
#define BUBBLE_UP(buf, i, t)  t = buf[i]; buf[i] = buf[i+1]; buf[i+1] = t

    uint16_t tn, c, tmp;
    int32_t cnt = 1;

    /// prepare an re-odering idx vector, make a copy of tMaxPq for sorting
    // Trima[0][0] is SMaxPq
    for (tn = 0; tn <= pTrim->TrimNum; ++tn)
    {
        tnIdxa[tn] = tn;
        tMaxPqa[tn] = pTrim->Trima[tn*pTrim->TrimTypeDim];
    }

    //// bubble sorting tMaxPqa
    // pTrim->tMaxPqa[0][0] is the SMaxPq pTrim->tMaxPqa[0][1...] is the default value
    for (tn = (uint16_t)pTrim->TrimNum; tn > 0 && cnt; --tn)   // compare to range
    {
        // each loop get max at tn for [0, tn]
        cnt = 0;
        for (c = 0; c < tn; ++c)   // candidate
        {
            if (tMaxPqa[c] >= tMaxPqa[c+1])
            {
                BUBBLE_UP(tMaxPqa, c, tmp);
                BUBBLE_UP(tnIdxa, c, tmp);
                cnt = 1;
            }
        }
    }
}

// must be called after commited srcSigEnv, tgtSigEnv and DmCtrl change
// Source related intermediate result is not saved since
// (1) that will updated by mdStream per frame
// (2) or one time set up
static int InterpolateTrim(uint16_t tMaxPq, const Trim_t *pTrim, int32_t *pTrimValue)
{

    uint16_t tnIdxa[TRIM_NUM_DIM];
    uint16_t tMaxPqa[TRIM_NUM_DIM];
    int32_t theTn = -1;
    int32_t tt;
    uint16_t *pTrim0, *pTrim1;

    SortTrims(pTrim, tnIdxa, tMaxPqa);

    // if no trim, trimNum == 0, the default, theTn = 0 anyway

    //  tMaxPq <= tMaxPqa[0]
    if (tMaxPq <= tMaxPqa[0])
    {
        theTn = tnIdxa[0];
    }
    //  pTrim->TrimNum >= tMaxPqa[pTrim->TrimNum],
    else if (tMaxPq >= tMaxPqa[pTrim->TrimNum])
    {
        theTn = tnIdxa[pTrim->TrimNum];
    }
    else
    {
        // tMaxPq in bound && pTrim->TrimNum > 0
        uint16_t idx1, idx2;
        for (idx2 = 1; tMaxPq >= tMaxPqa[idx2]; ++idx2);
        idx1 = idx2-1;

        assert(idx2 <= pTrim->TrimNum);

        if (tMaxPqa[idx1] == tMaxPqa[idx2])
        {
            // shall not happen, but let it be
            theTn = tnIdxa[idx1];
        }
        else
        {
            // interpolating
            idx1 = tnIdxa[idx1];
            idx2 = tnIdxa[idx2];
            pTrim1 = &pTrim->Trima[idx2*pTrim->TrimTypeDim];

            pTrim0 = &pTrim->Trima[idx1*pTrim->TrimTypeDim];
            for (tt = 1; tt < pTrim->TrimTypeDim; ++tt)
            {
                pTrimValue[tt] = ((int32_t)(pTrim1[tt] - pTrim0[tt]) << 15) / ((int32_t)pTrim1[0] - (int32_t)pTrim0[0]);
                pTrimValue[tt] = (pTrimValue[tt] * (tMaxPq - pTrim0[0])) >> 15;
                pTrimValue[tt] += pTrim0[tt];
            }
        }
    }

    if (theTn != -1)
    {
        // trim at theTn case
        pTrim0 = &pTrim->Trima[theTn*pTrim->TrimTypeDim];
        for (tt = 1; tt < pTrim->TrimTypeDim; ++tt)
        {
            pTrimValue[tt] = pTrim0[tt];
        }
    }

    pTrimValue[0] = tMaxPq;

    return 0;
}

static int CommitTrim(unsigned short tMaxPq, HMdsExt_t hMdsExt, DmCtxtFxp_t *pDmCtxt, TcCtrlFxp_t *pTcCtrl)
{
    TmCtrlFxp_t *pTmCtrl = &pDmCtxt->dmCfg.tmCtrl;
    TrimSet_t *pTrimSet = &hMdsExt->trimSets;
    Trim_t *pTrim;

    int32_t tt;
    int32_t trimAtTMaxPq[MAX2S(TRIM_TYPE_DIM2, TRIM_TYPE_DIM3)];


    /* level 2 at TrimSets[0] */
    pTrim = &pTrimSet->TrimSets[0];
    pTrim->Trima[0] = hMdsExt->source_max_PQ;
    InterpolateTrim(tMaxPq, pTrim, trimAtTMaxPq);

    for (tt = 1; tt < pTrim->TrimTypeDim - 1; ++tt)
    {
        // distribute the derived value convert back to true value
        pTcCtrl->level2[tt]= trimAtTMaxPq[tt] + pTmCtrl->CodeBias2[tt] + pTmCtrl->ValueAdj2[tt];
    }
    if (pTcCtrl->tcLutDir == CTcLutDirBwd && !pTcCtrl->bwdInvertible)
    {
#   if  DM_VER_LOWER_THAN212
        pTcCtrl->level2[TrimTypeChromaWeight] -= pTmCtrl->ValueAdj2[TrimTypeChromaWeight];
        pTcCtrl->level2[TrimTypeChromaWeight] += pTmCtrl->ValueAdj2ChromaWeightBwd;
#   else
        pTcCtrl->level2[TrimTypeSatGain] -= pTmCtrl->ValueAdj2[TrimTypeSatGain];
        pTcCtrl->level2[TrimTypeSatGain] += pTmCtrl->ValueAdj2SatGainBwd;
#   endif
    }

    pTcCtrl->level2[TrimTypeChromaWeight] = (trimAtTMaxPq[TrimTypeChromaWeight] +
                                            pTmCtrl->CodeBias2[TrimTypeChromaWeight] + pTmCtrl->ValueAdj2[TrimTypeChromaWeight]) << 4;

    pTcCtrl->level2[TrimTypeMsWeight] = trimAtTMaxPq[TrimTypeMsWeight] * 2 +
                                        pTmCtrl->CodeBias2[TrimTypeMsWeight] + pTmCtrl->ValueAdj2[TrimTypeMsWeight];

//# if DM_VER_CTRL == 2
    ClampLevel2(pTcCtrl->level2);
//# endif



    return 0;
}

// for now, only detect if no change, only TC change, cfg(everything) change
// smaller value=>less change
# if MSTAR_DRIVER_MOD
typedef enum MdsChg_t_ {MDS_CHG_NONE=0,  MDS_CHG_TC=1, MDS_CHG_CFG=2} MdsChg_t;
# else
typedef enum MdsChg_t_ {MDS_CHG_NONE,  MDS_CHG_TC, MDS_CHG_CFG} MdsChg_t;
# endif


// compare to cached one(mdsExtRef)
MdsChg_t CheckMds(const MdsExt_t *pMdsExt, const MdsExt_t *pMdsExtRef)
{
    const char *pSt = (const char *)pMdsExt;
    const char *pStRef = (const char *)pMdsExtRef;
    const int ccmOff = (int)((const char *)(&pMdsExt->min_PQ) - pSt);

    if (MemEqByte(pSt, pStRef, sizeof(MdsExt_t)))
    {
        return MDS_CHG_NONE;  // MDS not changed
    }


    if (!MemEqByte(pSt, pStRef, ccmOff))
    {
        return MDS_CHG_CFG; // CFG changed
    }

    return MDS_CHG_TC; // only TC related changed
}

void CommitMdsCsc(HMdsExt_t hMdsExt, DmCfgFxp_t *pDmCfg, CscCtrlFxp_t *pCscCtrl, DmKsIMapFxp_t *pKsIMap
#                if DM_VER_HIGHER_THAN211
                  , DECLARE_DM3OMAP_PTR(pKsOMap)
#                endif
                 )
{
# if !EN_IPT_PQ_ONLY_OPTION
    int32_t r, c;
# endif
    int32_t crossTalk;
    int16_t m33Lms2Ipt[3][3];
    int32_t m33Lms2IptScale2P;
    int16_t m33Ipt2Lms[3][3];
# if DM_VER_HIGHER_THAN211
    int32_t m33Ipt2LmsScale2P;
# endif


    //// get crosstalk and ipt space to use
    // local value
    crossTalk = pDmCfg->srcSigEnv.CrossTalk;
    MemCpyByte(m33Ipt2Lms, pCscCtrl->m33Ipt2Lms, sizeof(m33Ipt2Lms));
# if DM_VER_HIGHER_THAN211
    m33Ipt2LmsScale2P = pCscCtrl->m33Ipt2LmsScale2P;
# endif
    MemCpyByte(m33Lms2Ipt, pCscCtrl->m33Lms2Ipt, sizeof(m33Lms2Ipt));
    m33Lms2IptScale2P = pCscCtrl->m33Lms2IptScale2P;
# if MAP_2_INPUT_IPT
    if (hMdsExt->signal_color_space == 2)
    {
        int64_t scale;
        int16_t rshift;
        int64_t m33[3][3];

        // extract the crosstalk from inverse crossTalk matrix, the m33Rgb2Lms
        crossTalk = ((int32_t)(-hMdsExt->m33Rgb2WpLms[0][1]) << 15)/
                    ( hMdsExt->m33Rgb2WpLms[0][0]  - hMdsExt->m33Rgb2WpLms[0][1]);

        // the incoming ipt=>lms
        scale = DLB_UINT_MAX(32)/((int64_t)1<<hMdsExt->m33Yuv2RgbScale2P);
        if (hMdsExt->v3Yuv2Rgb[0])
        {
            // scale to remove narrow=>full upscaling
            int32_t rng;
            int32_t v3[3];
            GetIptDnParam(hMdsExt->signal_bit_depth, CRngNarrow, v3, &rng);
            scale *= rng;
            GetIptDnParam(hMdsExt->signal_bit_depth, CRngFull, v3, &rng);
            scale /= rng;
        }
        for (r = 0; r < 3; ++r)
        {
            for (c = 0; c < 3; ++c)
            {
                m33[r][c] = scale * hMdsExt->m33Yuv2Rgb[r][c];
            }
        }
        m33Ipt2LmsScale2P = Adjust64M33ScaleTo16M33(m33, 32, m33Ipt2Lms);

        m33Lms2IptScale2P = m33Ipt2LmsScale2P;
        rshift = InvM33(m33Ipt2Lms, m33Lms2Ipt, m33Ipt2LmsScale2P, m33Lms2IptScale2P);
        m33Lms2IptScale2P -= rshift;
    }
# endif

    //// input CSC
# if !EN_IPT_PQ_ONLY_OPTION

# if EN_IPT_PQ_INPUT_SHORT_CUT
    if (hMdsExt->signal_color_space != 2)
    {
# endif
        //// yuv=>rgb
        pKsIMap->m33Yuv2RgbScale2P = hMdsExt->m33Yuv2RgbScale2P;
        for (r = 0; r < 3; ++r)
        {
            for (c = 0; c < 3; ++c)
            {
                pKsIMap->m33Yuv2Rgb[r][c] = hMdsExt->m33Yuv2Rgb[r][c];
            }
        }

        // derive the offset in rgb domain with scale m33Yuv2RgbScale2P
        Yuv2RgbOffsetRgbFxp(pKsIMap->m33Yuv2Rgb, hMdsExt->v3Yuv2Rgb[0], hMdsExt->v3Yuv2Rgb[1],
                            pKsIMap->v3Yuv2RgbOffInRgb);

        //// rgb=>lms
        // use local crosstalk and ipt<=>lms
        pCscCtrl->sM33Rgb2LmsScale2P = hMdsExt->m33Rgb2WpLmsScale2P;
        for (r = 0; r < 3; ++r)
        {
            for (c = 0; c < 3; ++c)
            {
                pCscCtrl->sM33Rgb2Lms[r][c] = hMdsExt->m33Rgb2WpLms[r][c];
            }
        }

        // crossTalk: if ipt input, the input one, else local one
        if (hMdsExt->signal_color_space != 2)
        {
            pKsIMap->m33Rgb2LmsScale2P = GetRgb2LmsCtWpM33Fxp(pCscCtrl->sM33Rgb2Lms, pCscCtrl->sM33Rgb2LmsScale2P, crossTalk,
                                         pCscCtrl->sV3Wp, pCscCtrl->sV3WpScale2P, pKsIMap->m33Rgb2Lms);
        }
        else
        {
            // just in case user has input WP: it shall not be used in IPT input
            int32_t v3[3] = {32767, 32767, 32767};
            pKsIMap->m33Rgb2LmsScale2P = GetRgb2LmsCtWpM33Fxp(pCscCtrl->sM33Rgb2Lms, pCscCtrl->sM33Rgb2LmsScale2P, crossTalk,
                                         v3, 15, pKsIMap->m33Rgb2Lms);
        }

        //// signal_color_space
        pKsIMap->clr = CLR_M2K(hMdsExt->signal_color_space);

        //// signal_full_range_flag
        GetRange(hMdsExt->signal_bit_depth,
                 (hMdsExt->signal_full_range_flag == 1) ? CRngFull :
                 CRngNarrow,
                 (hMdsExt->signal_eotf == signal_eotf_VALUE_PQ) ? CEotfPq : CEotfBt1886,
                 &pKsIMap->eotfParam.rangeMin, &pKsIMap->eotfParam.range);
        pKsIMap->eotfParam.rangeInv = DLB_UINT_MAX(32)/(pKsIMap->eotfParam.range << (16 - hMdsExt->signal_bit_depth)); // scale up 2^32
        pKsIMap->eotfParam.bdp = hMdsExt->signal_bit_depth;

        //// eotf
        if (hMdsExt->signal_eotf == signal_eotf_VALUE_PQ)
        {
            pKsIMap->eotfParam.eotf = KEotfPq;
            //pKsIMap->eotfParam.gamma = 0; leave it alone
# if MSTAR_DRIVER_MOD
            uint16_t i;
            for (i = 0; i < MSTAR_DEGAMMA_LUT_SIZE; i++)
                pKsIMap->g2L[i] = g_au32DoViDegammaPqLut[i];
#endif
        }
        else
        {
            pKsIMap->eotfParam.eotf = KEotfBt1886;
#     if (REDUCED_COMPLEXITY == 0)
            pKsIMap->eotfParam.gamma = hMdsExt->signal_eotf;
            pKsIMap->eotfParam.a = hMdsExt->signal_eotf_param0;
            pKsIMap->eotfParam.b = hMdsExt->signal_eotf_param1;
            pKsIMap->eotfParam.g = hMdsExt->signal_eotf_param2;
#     else
            {
                uint16_t i;
# if MSTAR_DRIVER_MOD
                if ((hMdsExt->source_max_PQ >= 3695) && (hMdsExt->source_max_PQ <= 3698))   // 4000 nits, pulsar
                {
                    for (i = 0; i < MSTAR_DEGAMMA_LUT_SIZE; i++)
                        pKsIMap->g2L[i] = g_au32DoViDegammaPulsarLut[i];
                }
                else if ((hMdsExt->source_max_PQ >= 2080) && (hMdsExt->source_max_PQ <= 2083))   // 100 nits, sdr
                {
                    for (i = 0; i < MSTAR_DEGAMMA_LUT_SIZE; i++)
                        pKsIMap->g2L[i] = g_au32DoViDegammaSdrLut[i];
                }
                else   // others
                {
                    uint16_t x = 0;
                    for (i = 0; i < MSTAR_DEGAMMA_LUT_SIZE; i++)
                    {
                        pKsIMap->g2L[i] = DeGamma(hMdsExt->signal_eotf_param0, hMdsExt->signal_eotf_param1, hMdsExt->signal_eotf, hMdsExt->signal_eotf_param2, i);
                    }
                }
#else
                if ((hMdsExt->source_max_PQ >= 3695) && (hMdsExt->source_max_PQ <= 3698))   // 4000 nits, pulsar
                {
                    for (i = 0; i < DEF_G2L_LUT_SIZE; i++)
                        pKsIMap->g2L[i] = g2lPulsar256Lut[i];
                }
                else if ((hMdsExt->source_max_PQ >= 2080) && (hMdsExt->source_max_PQ <= 2083))   // 100 nits, sdr
                {
                    for (i = 0; i < DEF_G2L_LUT_SIZE; i++)
                        pKsIMap->g2L[i] = g2lSdr256Lut[i];
                }
                else   // others
                {
                    uint16_t x = 0;
                    for (i = 0; i < DEF_G2L_LUT_SIZE; i++)
                    {
                        pKsIMap->g2L[i] = DeGamma(hMdsExt->signal_eotf_param0, hMdsExt->signal_eotf_param1, hMdsExt->signal_eotf, hMdsExt->signal_eotf_param2, x);
                        x += (0x10000 / DEF_G2L_LUT_SIZE);
                    }
                }
#endif
            }
#     endif
        }

#   if !EN_IPT_PQ_INPUT_SHORT_CUT
        if (hMdsExt->signal_color_space == 2)
        {
            // fake to yuv: calculated bypass
            pKsIMap->clr = KClrYuv;
        }
#   endif

        // m33Lms2Ipt: if ipt input, the input ipt space, else local ipt
        AssignM33I2I((const int16_t (*)[3])m33Lms2Ipt, pKsIMap->m33Lms2Ipt);
        pKsIMap->m33Lms2IptScale2P = m33Lms2IptScale2P;

# if EN_IPT_PQ_INPUT_SHORT_CUT
    } // hMdsExt->signal_color_space != 2
    else
    {
        pKsIMap->clr = KClrIpt;
        // ipt scale offset is setup for all cases
    }
# endif

# endif //EN_IPT_PQ_ONLY_OPTION

    //// set up ipt scale offset anyway since some debug code need it
    // necessary only if EN_IPT_PQ_INPUT_SHORT_CUT && hMdsExt->signal_color_space == 2
# if (EN_IPT_PQ_ONLY_OPTION || DM_VER_CTRL == 3)
    GetIptNormParam(hMdsExt->signal_bit_depth,  (hMdsExt->v3Yuv2Rgb[0]) ? CRngNarrow : CRngFull,
                    pKsIMap->v3IptOff, &pKsIMap->iptScale);
    // just to make sure
    pKsIMap->v3IptOff[0] = hMdsExt->v3Yuv2Rgb[0] >> 16;
    pKsIMap->v3IptOff[1] = hMdsExt->v3Yuv2Rgb[1] >> 16;
    pKsIMap->v3IptOff[2] = hMdsExt->v3Yuv2Rgb[2] >> 16;
# endif // (EN_IPT_PQ_ONLY_OPTION || DM_VER_CTRL == 3)

# if MAP_2_INPUT_IPT
    //// graphic channel CSC

    //// output CSC
# if DM_VER_HIGHER_THAN211
# if EN_IPT_PQ_OUTPUT_SHORT_CUT
    if (pDmCfg->tgtSigEnv.Clr != CClrIpt)
    {
# endif
        AssignM33I2I((const int16_t (*)[3])m33Ipt2Lms, pKsOMap->m33Ipt2Lms);
        pKsOMap->m33Ipt2LmsScale2P = m33Ipt2LmsScale2P;
        pKsOMap->m33Lms2RgbScale2P = GetLms2RgbCtWpM33Fxp(pCscCtrl->tM33Lms2Rgb, pCscCtrl->tM33Lms2RgbScale2P, crossTalk, pCscCtrl->tV3Wp, pCscCtrl->tV3WpScale2P, pKsOMap->m33Lms2Rgb);
# if EN_IPT_PQ_OUTPUT_SHORT_CUT
    }
# endif
    //else done during CommitKsOMap(): use local LMS<=>IPT(as long as cancel out), crosstalk not used
# endif // DM_VER_HIGHER_THAN211
# endif //MAP_2_INPUT_IPT
}

#if EN_RUN_MODE
static int GetRunMode2Be(HDm_t hDm, HMdsExt_t hMdsExt)
{
    return (hMdsExt->lvl255RunModeAvail) ? hMdsExt->dm_run_mode : hDm->dmCfg.dmCtrl.RunMode;
}
#endif

int CommitMds(HMdsExt_t hMdsExt, HDmFxp_t hDm)
{
    DmCfgFxp_t *pDmCfg = &hDm->dmCfg;
    DmKsFxp_t *pDmKs = hDm->hDmKs;
    MdsChg_t mdsChg = CheckMds(hMdsExt, &hDm->mdsExt);
# if EN_RUN_MODE
    int runMode2Be;
# endif
    int32_t ret = 0;

    TcCtrlFxp_t *pTcCtrl = &hDm->tcCtrl;
    TcSigFxp_t *pTcSrcSig = &pTcCtrl->tcSrcSig;

# if EN_GLOBAL_DIMMING
    ++hDm->gdCtrlDr.gdUdFrms;
# endif

    if (mdsChg == MDS_CHG_NONE)
    {
        //printf("mds didn't change, run it anyway\n");
        return 0;
    }
    else
    {
        ret |= FLAG_CHANGE_MDS;
    }

# if EN_RUN_MODE
    runMode2Be = GetRunMode2Be(hDm, hMdsExt);
# endif

    if (mdsChg == MDS_CHG_CFG)
    {
        //// ksFrmFmtI
        pDmKs->ksFrmFmtI.dtp = KDtpU16; // it is the case so far
        pDmKs->ksFrmFmtI.weav = WEAV_M2K(hMdsExt->signal_chroma_format);

        pDmKs->ksFrmFmtI.colPitch =
            PxlColPitchFromK(CLR_M2K(hMdsExt->signal_color_space),
                             CHRM_M2K(hMdsExt->signal_chroma_format),
                             pDmKs->ksFrmFmtI.weav, pDmKs->ksFrmFmtI.dtp);

        //// Uds
        pDmKs->ksUs.minUs = 0;
        pDmKs->ksUs.maxUs = (int16_t)((1 << hMdsExt->signal_bit_depth) - 1);

#   if EN_UP_DOWN_SAMPLE_OPTION
        // signal_chroma_format: for MDS case, no simple intl mode
        pDmKs->ksUs.chrmIn = CHRM_M2K(hMdsExt->signal_chroma_format);
        pDmKs->ksFrmFmtI.chrm = pDmKs->ksUs.chrmIn; // for DM286
#   endif

        //// all CSC related stuff
        CommitMdsCsc(hMdsExt, pDmCfg, &hDm->cscCtrl, &pDmKs->ksIMap
#                if DM_VER_HIGHER_THAN211
                     , GET_DM3OMAP_PTR(hDm)
#                endif
                    );

        pDmKs->ksFrmFmtI.bdp = hMdsExt->signal_bit_depth; // for DM286

        ret |= FLAG_CHANGE_MDS_CFG;
    } // (mdsChg == MDS_CHG_CFG)

    //// set up only if needed
    ////// TC
# if EN_GLOBAL_DIMMING
# if EN_RUN_MODE
    if (runMode2Be != 2)   // add check to avoid heavy lifting of UpdateKsOMapTmm()
    {
# endif
        // if we are sure GdOn have higher priority than lvl4GdAvail, the follow code can be simplified
        // if (pDmCfg->gdCtrl.GdOn) {
        // if (hMdsExt->lvl4GdAvail) {
        //if (pDmCfg->gdCtrl.GdOn || hMdsExt->lvl4GdAvail) {
        if (pDmCfg->gdCtrl.GdOn && hMdsExt->lvl4GdAvail)
        {
            // gd is active
            uint32_t fltrdMeanPq;
            uint32_t fltrdStdPq;
            uint32_t fltrdAnchorPq;
            GdCtrlDrFxp_t *pGdCtrlDr = &hDm->gdCtrlDr;

            if (hMdsExt->lvl4GdAvail)
            {
                fltrdMeanPq = hMdsExt->filtered_mean_PQ;;
                fltrdStdPq  = hMdsExt->filtered_power_PQ;
            }
            else
            {
                fltrdMeanPq = pGdCtrlDr->fltrdMeanPqDft;
                fltrdStdPq  = pGdCtrlDr->fltrdStdPqDft;
            }
            fltrdAnchorPq = (pGdCtrlDr->gdWeightMean*fltrdMeanPq + pGdCtrlDr->gdWeightStd*fltrdStdPq) >> 12;

            if (
                (!pGdCtrlDr->gdActiveLstFrm ||
                 (int32_t)fltrdAnchorPq != pGdCtrlDr->fltrdAnchorPqLstFrm)
#       if DM_VER_LOWER_THAN212
                && pDmCfg->gdCtrl.GdCap
#       endif
            )
            {

                if (GdDeriveEffTmm(fltrdAnchorPq, pGdCtrlDr))   // tgt update: actual value used not changed
                {
                    // global dimming to TC
                    Gd2TcCtrl(pGdCtrlDr, pDmCfg->tgtSigEnv.DiagSize, pTcCtrl);

                    // update according to gd tMax, tMin
                    UpdateKsOMapTmm(pDmCfg, 1, hDm);
                    pGdCtrlDr->gdActiveLstFrm = 1;
                    pGdCtrlDr->fltrdAnchorPqLstFrm = fltrdAnchorPq;

                    ret |= FLAG_CHANGE_GD;
                }
            }
        }
        else
        {
            // gd is inactive
            if (hDm->gdCtrlDr.gdActiveLstFrm)
            {
                // tgt env to TC
                TgtSigEnv2TcCtrl(&pDmCfg->tgtSigEnv, pTcCtrl); // target related

                // update according to tgt tMax, tMin
                UpdateKsOMapTmm(pDmCfg, 0, hDm);
                hDm->gdCtrlDr.gdActiveLstFrm = 0;

                ret |= FLAG_CHANGE_GD;
            }
        }
# if EN_RUN_MODE
    }
# endif
# endif

# if !EN_AOI
    if (hMdsExt->lvl5AoiAvail
//#     if EN_AOI
//      || pDmCfg->dmCtrl.AoiOn
//#     endif
       )
    {
        // effectively disable trim when level5 present when AOI is not supported
        hMdsExt->trimSets.TrimSetNum = 0;
        hMdsExt->trimSets.TrimSets[0].TrimNum = 0;
    }
# endif

# if EN_AOI
    // even if lvl5AoiAvail = 0 or pDmCfg->dmCtrl.AoiOn = 0, the offsets are set up properly
    pDmKs->ksDmCtrl.aoiRow0       = hMdsExt->active_area_top_offset;
    pDmKs->ksDmCtrl.aoiRow1Plus1  = pDmCfg->srcSigEnv.RowNum - hMdsExt->active_area_bottom_offset;
    pDmKs->ksDmCtrl.aoiCol0       = hMdsExt->active_area_left_offset;
    pDmKs->ksDmCtrl.aoiCol1Plus1  = pDmCfg->srcSigEnv.ColNum - hMdsExt->active_area_right_offset;
# endif

    // no change tmCtrl

    // srcSigEnv: SrcSigEnv2TcCtrl
    pTcSrcSig->minPq = hMdsExt->source_min_PQ;
    pTcSrcSig->maxPq = hMdsExt->source_max_PQ;
    pTcSrcSig->diagSize = hMdsExt->source_diagonal;

    DeriveTcLutDir(pTcCtrl
# if EN_RUN_MODE
                   , runMode2Be
# endif
                  ); // must set the tcLutDir before all the Update* and TmCtrl2TcCtrl()

    TmCtrl2TcCtrl(&pDmCfg->tmCtrl, pTcCtrl, 0);  // tmCtrl related

//  UpdateTcCtrlDerived(pTcCtrl);

    // to ensure we have the right trim in Bwd TC mapping
    CommitTrim((pTcCtrl->tcLutDir == CTcLutDirBwd) ? hMdsExt->source_max_PQ : pTcCtrl->tcTgtSig.maxPq, hMdsExt, hDm, pTcCtrl);
    //  CommitTrim(pTcCtrl->tMaxPqDn, hMdsExt, hDm);

    // all ugly part go here
    CommitKsCtrls(pTcCtrl, pDmKs);

    if (pTcCtrl->tcLutDir != CTcLutDirBwd)
    {
        pTcSrcSig->crush = hMdsExt->min_PQ;
        pTcSrcSig->clip = hMdsExt->max_PQ;
        pTcSrcSig->mid = hMdsExt->mid_PQ;
    }
    else
    {
#   if FOLLOW_MATLAB_CCM_ALWAYS_AS_SOURCE_IN_TM
        // always use the given CCM
        pTcCtrl->tcTgtSig.crush = hMdsExt->min_PQ; //CreateToneCurve() uses source
        pTcCtrl->tcTgtSig.clip = hMdsExt->max_PQ;
        pTcCtrl->tcTgtSig.mid = hMdsExt->mid_PQ;
        /*  pTcCtrl->tcTgtSig.crush = hMdsExt->min_PQ;
            pTcCtrl->tcTgtSig.clip = hMdsExt->max_PQ;
            pTcCtrl->tcTgtSig.mid = hMdsExt->mid_PQ; */
#   else
        // use static one
        IntTcSigStaticCcm(&pTcCtrl->tcTgtSig);
#   endif
    }

    if (CreateToneCurve(pTcCtrl, &pDmKs->ksTMap, (void *)0, &hDm->tcCtrlUsed))
        ret |= FLAG_CHANGE_TC;

# if REDUCED_TC_LUT
    {
        int i;
        pDmKs->ksTMap.tmLutMaxVal = 0;
        for (i = 0; i < 515; i++)
            if (pDmKs->ksTMap.tmInternal515Lut[i] > pDmKs->ksTMap.tmLutMaxVal)
                pDmKs->ksTMap.tmLutMaxVal = pDmKs->ksTMap.tmInternal515Lut[i];
    }
# endif


# if EN_RUN_MODE
    if (runMode2Be == 1)
    {
        int32_t runModePrev = GetRunMode(hDm);// the previous run mode: init to RunMode == 0

        // to run bypass cvm
        // noCvm
#     if EN_MS_OPTION
        pDmKs->ksMs.disMs = 1;
#     endif

        if (runModePrev == 2)
        {
#     if DM_SOFTWARE_BYPASS
            CommitKsOMap(pDmCfg, EN_GLOBAL_DIMMING, hDm);
#     endif
        }
#   if (DM_VER_CTRL == 2) && DM_VER_HIGHER_THAN211
        else
        {
            CommitKsOMapDm2FromDm3(pDmCfg, hDm);
        }
#   endif
    }
    else if (runMode2Be == 2)
    {
        // to run bypass dm
        // do not like it, but ...
        GET_BYPASS_SHIFT(hMdsExt->signal_bit_depth, pDmCfg->tgtSigEnv.Bdp, pDmKs->bypassShift);

#   if DM_SOFTWARE_BYPASS
        // noCvm
#   if EN_MS_OPTION
        pDmKs->ksMs.disMs = 1;
#   endif

        CommitKsIOMapBypass(CLR_M2K(hMdsExt->signal_color_space), hMdsExt->signal_bit_depth, pDmCfg->tgtSigEnv.Bdp, hDm);
#   endif
    }
    else
    {
        // to run normal
        int runModePrev = GetRunMode(hDm);// the previous run mode: init to RunMode == 0
        if (runModePrev == 1)
        {
#     if EN_MS_OPTION
            // restore cvm
            pDmKs->ksMs.disMs = pDmCfg->msCtrl.MsMethod == CMsMethodOff;
#     endif
#     if (DM_VER_CTRL == 2) && DM_VER_HIGHER_THAN211
            CommitKsOMapDm2FromDm3(pDmCfg, hDm);
#     endif
        }
        else if (runModePrev == 2)
        {
#     if EN_MS_OPTION
            // restore cvm
            pDmKs->ksMs.disMs = pDmCfg->msCtrl.MsMethod == CMsMethodOff;
#     endif
#     if DM_SOFTWARE_BYPASS
            CommitKsOMap(pDmCfg, EN_GLOBAL_DIMMING == 0, hDm);
#     endif
        }
#   if (DM_VER_CTRL == 2) && DM_VER_HIGHER_THAN211
        else
        {
            CommitKsOMapDm2FromDm3(pDmCfg, hDm);
        }
# endif
    }
# elif (DM_VER_CTRL == 2) && DM_VER_HIGHER_THAN211
    CommitKsOMapDm2FromDm3(pDmCfg, hDm);
# endif

    MemCpyByte(&hDm->mdsExt, hMdsExt, sizeof(MdsExt_t));

    return ret;
}

#if EN_GLOBAL_DIMMING
int GetGdActiveFltrdAnchor(uint16_t *pFmPqDn, uint32_t *pFm, HDmFxp_t hDm)
{
    GdCtrlDrFxp_t *pGdCtrlDr;

    if (!hDm)
    {
        return -1;
    }

    pGdCtrlDr = &hDm->gdCtrlDr;

    if (!pGdCtrlDr->gdActiveLstFrm)
    {
        return -1;
    }

    if (pFm) *pFm = pGdCtrlDr->fltrdAnchorUsed;

    if (pFmPqDn)
    {
        *pFmPqDn = LToPQ12(pGdCtrlDr->fltrdAnchorUsed);
    }

    return 0;
}

// to obsolete
int GetGdActiveFltrdMean(uint16_t *pFmPqDn, uint32_t *pFm, HDmFxp_t hDm)
{
    return GetGdActiveFltrdAnchor(pFmPqDn, pFm, hDm);
}

int GetGdActiveTgtWindow(uint32_t *pWMin, uint32_t *pWMax, HDmFxp_t hDm)
{
    GdCtrlDrFxp_t *pGdCtrlDr;

    if (!hDm)
    {
        return -1;
    }

    pGdCtrlDr = &hDm->gdCtrlDr;

    if (!pGdCtrlDr->gdActiveLstFrm)
    {
        return -1;
    }

    if (pWMin) *pWMin = pGdCtrlDr->tMinEff;
    if (pWMax) *pWMax = pGdCtrlDr->tMaxEff;

    return 0;
}
#endif // EN_GLOBAL_DIMMING

int GetTcLutDir(HDmFxp_t hDm)
{
    return (hDm->tcCtrl.tcLutDir == CTcLutDirFwd) ?  1 :
           (hDm->tcCtrl.tcLutDir == CTcLutDirBwd) ? -1 : 0;
}

#if 0 //not MSTAR_DRIVER_MOD
// Set kernel structure (hardware interface)
// This function shows firmware and hardware interfacing, which may need to tweak to particular hardware
// It assumes hardware interface register map is same as DmKsFxp_t
void SetHardwareInterface(const HDmKs_t hKs, uint8_t *pRegStartAddr)
{
    uint8_t *p = pRegStartAddr;


    static DmKsFxpBack_t KsBack, *pKsBack;

    pKsBack = &KsBack;
    DM2xLoad3DLut(hKs, pKsBack);
    DM2xAdaptor(hKs, pKsBack);

    MemCpyByte(p, &pKsBack->ksFrmFmtI, sizeof(pKsBack->ksFrmFmtI));
    p += sizeof(pKsBack->ksFrmFmtI);

    MemCpyByte(p, &pKsBack->dmExec, sizeof(pKsBack->dmExec));
    p += sizeof(pKsBack->dmExec);

    MemCpyByte(p, &pKsBack->ksFrmFmtO, sizeof(pKsBack->ksFrmFmtO));
    p += sizeof(pKsBack->ksFrmFmtO);

}
#endif
