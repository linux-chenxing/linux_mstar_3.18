/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom
* without the express permission of Dolby Laboratories is prohibited.


* Copyright 2011 - 2015 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/
#include "fx_math.h"
#include "DolbyDisplayManagement.h"
#include "KCdmModCtrl.h"

extern const uint32_t depq1024Lut[1024];

uint32_t PQToL(uint16_t u16_x0)
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
    return y_1;
}

static int32_t ToneMapS(int32_t IntensityVectorWeight,int32_t s2tRatio, uint16_t tMode , int16_t *pTmapS, int16_t lutSize)
{
    int32_t iSign = (tMode==DM_NORMAL_TC)?1:-1;
    int32_t snorm;

    fx_assert(  QFTMS  <= 15 , " QFTMS must be <= 15");

    snorm = unorm_s(IntensityVectorWeight);
    IntensityVectorWeight = sshr_sss(IntensityVectorWeight,-(snorm-9));
    snorm = snorm + QLVL3MD-1; /* -1 ois for xVal 0 to 2 */

    if(DM_NORMAL_TC == tMode || DM_INVERSE_TC == tMode)
    {
        //max(0,1-S*DMParams.ChromaWeight*S2Tratio)
        int16_t iloop;
        int32_t ltemp1;
        int64_t lltemp1;

        for (iloop = 0 ; iloop < lutSize ; iloop++)
        {
            ltemp1  = IntensityVectorWeight * iloop; // result in Q31
            lltemp1 = lmpy_ss(ltemp1,s2tRatio); //Q31 + QB02DATAIN
            ltemp1  = sshr_slu(lltemp1, snorm+QB02DATAIN-QFTMS - 1);
            ltemp1 +=1;
            ltemp1 >>=1;
            ltemp1 = (1<<QFTMS)-1 - (ltemp1*iSign); // 1-X
            ltemp1 = sminss(8191,ltemp1) ;// Limit to Max 16bits
            *pTmapS++ = (uint16_t)smaxss(0,ltemp1);              // Limit to Min 16bits
        }
    }
    else
    {
        int16_t iloop;
        for (iloop = 0 ; iloop < lutSize ; iloop++)
        {
            *pTmapS++ = (uint16_t)((1<<QFTMS)-1);
        }
    }

    return 0;
}

static int32_t SaturationMapS(int32_t ChromaVectroWeight,int32_t s2tRatio, uint16_t tMode , int16_t *pSmapS, int16_t lutSize)
{
    int32_t iSign = (tMode==DM_NORMAL_TC)?1:-1;
    int32_t snorm;

    fx_assert(  QFTMS <= 15 , " QFTMS must be <= 15");

    snorm = unorm_s(ChromaVectroWeight);
    ChromaVectroWeight = sshr_sss(ChromaVectroWeight,-(snorm-9));
    snorm = snorm + QLVL3MD-1; /* -1 ois for xVal 0 to 2 */

    if(DM_NORMAL_TC == tMode || DM_INVERSE_TC == tMode)
    {
        //max(0,1-S*DMParams.SaturationWeight*S2Tratio)
        int16_t iloop;
        int32_t ltemp1;
        int64_t lltemp1;

        for (iloop = 0 ; iloop < lutSize ; iloop++)
        {
            ltemp1 = ChromaVectroWeight * iloop ; // result in unorm+9
            lltemp1 = lmpy_ss(ltemp1,s2tRatio); //Q31 + QB02DATAIN
            ltemp1  = sshr_slu(lltemp1, snorm+QB02DATAIN-QFTMS - 1);
            ltemp1 +=1;
            ltemp1 >>=1;
            ltemp1 = (1<<QFTMS)-1 - (ltemp1*iSign); // 1-X
            ltemp1 = sminss(8191,ltemp1) ;// Limit to Max 16bits
            *pSmapS++ = (int16_t)smaxss(0,ltemp1);              // Output allow up to 2.0
        }
    }
    else
    {
        int16_t iloop;
        for (iloop = 0 ; iloop < lutSize ; iloop++)
        {
            *pSmapS++ = (int16_t)((1 <<QFTMS)-1);
        }
    }

    return 0;
}

static int32_t  SaturationMapI(int32_t tMinPq,int32_t tMaxPq,int32_t sMinPq,int32_t satGain,const int16_t *pTmapI,uint16_t tMode,int16_t *pSmapI, int16_t lutSize)
{
    int32_t shift;

    shift = (lutSize == 256) ? 8 : 9;

    tMinPq >>= (QB02DATAIN-12);
    tMaxPq >>= (QB02DATAIN-12);
    sMinPq >>= (QB02DATAIN-12);

    if(DM_NORMAL_TC == tMode)
    {
        // Y = ToneMapI(I)-I+1;
        int16_t iloop , iVal;
        int32_t ltemp0, ltemp1;
        int32_t dltMinPq = tMinPq - sMinPq; // scale in 12
        int64_t ltemp2;
        int32_t ltemp3 = ((uint32_t)1 << (30 - 1)) / (tMaxPq - tMinPq); // 0.5/(tMaxPq-tMinPq), scale 30-12=18
        int32_t ltemp4 = (int32_t)(4294967296 / 92.245708994065268); // scale in 22

        for (iloop = 0 ; iloop < lutSize ; iloop++)
        {
            ltemp0 = pTmapI[iloop];
            iVal   = iloop<<(QTCLUT-shift);
            //iVal   = smaxss(xMin>>(QB02DATAIN-QTCLUT),sminss(xMax>>(QB02DATAIN-QTCLUT), iVal));
            ltemp1 = ltemp0 - iVal;
            ltemp1 += (1<<QTCLUT) ; //+1;
            ltemp1 = ((int64_t)ltemp1 * satGain) >> QTRIMS;

            if (dltMinPq > 0)
            {
                // Reduce saturation of darks when mapping to higher black level
                ltemp2 = 3 * (int64_t)dltMinPq * PQToL((((tMaxPq - ltemp0) * ltemp3) + (1 << 13)) >> 14); // scale in 18+12
                ltemp2 = ((ltemp2 * ltemp4) + ((int64_t)1 << (32 + 18 - 1))) >> (32 + 18); // scale in 12
                if (ltemp2 >= 4096)
                    ltemp1 = 0;
                else
                    ltemp1 = (int32_t)(((ltemp1 * (4096 - ltemp2)) + (1 << 11)) >> 12);
            }
            *pSmapI++  = (int16_t)sminss(8191,ltemp1);
        }

    }
    else if(DM_INVERSE_TC == tMode)
    {
        //Y = 1./(I-ToneMapI(I)+1);
        int16_t iloop,iVal;
        int32_t ltemp1;

        for (iloop = 0 ; iloop < lutSize ; iloop++)
        {
            ltemp1 = pTmapI[iloop];
            iVal   = iloop<<(QTCLUT-shift);
            //iVal   = smaxss(xMin>>(QB02DATAIN-QTCLUT),sminss(xMax>>(QB02DATAIN-QTCLUT), iVal));
            ltemp1 = iVal - ltemp1;
            ltemp1 += (1<<QTCLUT) ;
            ltemp1 = (0x1<<(2*QTCLUT))/ltemp1;
            ltemp1 = ((int64_t)ltemp1 * satGain) >> QTRIMS;
            ltemp1 = smaxss(0,ltemp1);
            *pSmapI++  = (int16_t)sminss(8191,ltemp1);
        }
    }
    else
    {
        int16_t iloop;
        for (iloop = 0 ; iloop < lutSize ; iloop++)
        {
            *pSmapI++ = (int16_t)((1<<QTCLUT)-1);
        }
    }

    return 0;
}

void CreateToneCurve3pxx(
    srcMetadata_t_         *pSrcMetadata,
    displayPqTuning_t_     *pDisplayTuning,
    val_norm_pair_t        *pParam1,
    val_norm_pair_t        *pParam2,
    val_norm_pair_t        *pParam3,
    int32_t                *pSoR,
    int16_t               *pTmapI,
    int16_t               *pTmapS,
    int16_t               *pSmapI,
    int16_t               *pSmapS,
    int32_t                *ratio,
    int16_t                lutSize,
    int16_t                clampOut
)
{
    int32_t TMin,TMid, TMax,SMid,Slope;
    int32_t s2t_ratio, screen_ratio;
    int32_t sTrimSlope , sTrimOffset,  sTrimPower;
    int64_t temp64;
    int32_t TMidOffset,SMax;
    //printk("pDisplayTuning->tMinPQ = %d\n",pDisplayTuning->tMinPQ);
    //printk("pDisplayTuning->tMaxPQ = %d\n",pDisplayTuning->tMaxPQ);
    //printk("pSrcMetadata->sMinPQ = %d\n",pSrcMetadata->SminPQ);
    //printk("pSrcMetadata->sMaxPQ = %d\n",pSrcMetadata->SmaxPQ);
    /* Limit Trim Values */
    sTrimSlope  = sminss(TRIMSMAX,smaxss(pSrcMetadata->sTrimSlope,TRIMSMIN));
    sTrimPower  = sminss(TRIMSMAX,smaxss(pSrcMetadata->sTrimPower,TRIMSMIN));
    sTrimOffset = sminss(TRIMSMAX,smaxss(pSrcMetadata->sTrimOffset,TRIMSMIN));

    if(DM_INVERSE_TC==pDisplayTuning->tMode)
    {
        CalcMappedParams3pxx(
            pSrcMetadata->Clip,
            pSrcMetadata,
            pDisplayTuning,
            &TMin,&TMid,&TMax,&SMid,&Slope,
            &s2t_ratio , &screen_ratio);

        CalcCurveParams(
            pSrcMetadata->Crush, SMid,pSrcMetadata->Clip,
            TMin,   TMid,TMax, Slope, pDisplayTuning,
            pParam1,pParam2,pParam3,pSoR);

        GenerateToneCurveLut(
            pSrcMetadata->Crush,pSrcMetadata->Clip,
            pParam1,pParam2,pParam3,*pSoR,
            pDisplayTuning,
            sTrimSlope,sTrimPower,sTrimOffset,
            pTmapI,NULL, lutSize, clampOut);

        if (ratio)
        {
            *ratio = s2t_ratio;
            return;
        }

        ToneMapS(
            pSrcMetadata->IntensityVWeight,s2t_ratio, pDisplayTuning->tMode , pTmapS, lutSize);

        SaturationMapS(
            pSrcMetadata->ChromaVWeight,s2t_ratio, pDisplayTuning->tMode , pSmapS, lutSize);

        SaturationMapI(
            pDisplayTuning->tMinPQ,pDisplayTuning->tMaxPQ,pSrcMetadata->SminPQ,pSrcMetadata->SaturaGain, pTmapI, pDisplayTuning->tMode, pSmapI, lutSize);
    }
    else
    {
#define INV_TMIDBIASMAX (68267) //round(2^12/0.06)
        temp64 = (int64_t)pDisplayTuning->tMidPQBias * INV_TMIDBIASMAX;
        temp64 = temp64 >> 12;
        temp64 = (int64_t)uabs_ls(temp64);
        TMidOffset = (int32_t)sminss(temp64,0x7FFFFFFF);
        SMax = sadd_ss(TMidOffset , smpy_ss((0x7FFFFFFF-TMidOffset),pSrcMetadata->Clip));

        CalcMappedParams3pxx(
            SMax,
            pSrcMetadata,
            pDisplayTuning,
            &TMin,&TMid,&TMax,&SMid,&Slope,
            &s2t_ratio , &screen_ratio);

        CalcCurveParams(
            pSrcMetadata->Crush, SMid,SMax,
            TMin,   TMid,TMax, Slope, pDisplayTuning,
            pParam1,pParam2,pParam3,pSoR);

        GenerateToneCurveLut(
            pSrcMetadata->Crush,SMax,
            pParam1,pParam2,pParam3,*pSoR,
            pDisplayTuning,
            sTrimSlope,sTrimPower,sTrimOffset,
            pTmapI,NULL, lutSize, clampOut);

        if (ratio)
        {
            *ratio = s2t_ratio;
            return;
        }

        ToneMapS(
            pSrcMetadata->IntensityVWeight,s2t_ratio, pDisplayTuning->tMode , pTmapS, lutSize);

        SaturationMapS(
            pSrcMetadata->ChromaVWeight,s2t_ratio, pDisplayTuning->tMode , pSmapS, lutSize);

        SaturationMapI(
            pDisplayTuning->tMinPQ,pDisplayTuning->tMaxPQ,pSrcMetadata->SminPQ,pSrcMetadata->SaturaGain, pTmapI, pDisplayTuning->tMode, pSmapI, lutSize);
    }
}