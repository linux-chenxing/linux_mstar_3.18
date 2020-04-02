/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom
* without the express permission of Dolby Laboratories is prohibited.


* Copyright 2011 - 2015 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/


#include "dolbyTypeToMstarType.h"
#include "fx_math.h"
#include "DolbyDisplayManagement.h"

void CreateToneCurve2pxx(
                     srcMetadata_t_     *pSrcMetadata,
                     displayPqTuning_t_ *pDisplayTuning,
                     val_norm_pair_t    *pParam1,
                     val_norm_pair_t    *pParam2,
                     val_norm_pair_t    *pParam3,
                     int32_t            *pSoR,
                     int16_t           *toneCurve515LUT,
                     int16_t           *toneCurveLUT)
{

    int32_t TMin,TMid, TMax,SMid,Slope;
    int32_t Crush, Mid, Clip;
    int32_t SminPQ, SmaxPQ, Sdiagonalinches;
    int32_t sTrimSlope , sTrimOffset ,  sTrimPower;
    int64_t temp64;
    int32_t TMidOffset;

    /* Limit Trim Values */
    Crush  = pSrcMetadata->Crush;
    Mid    = pSrcMetadata->Mid;
    Clip   = pSrcMetadata->Clip;
    SminPQ = pSrcMetadata->SminPQ;
    Sdiagonalinches = pSrcMetadata->Sdiagonalinches;
    SmaxPQ = pSrcMetadata->SmaxPQ;
    sTrimSlope  = sminss(TRIMSMAX,smaxss(pSrcMetadata->sTrimSlope,TRIMSMIN));
    sTrimPower  = sminss(TRIMSMAX,smaxss(pSrcMetadata->sTrimPower,TRIMSMIN));
    sTrimOffset = sminss(TRIMSMAX,smaxss(pSrcMetadata->sTrimOffset,TRIMSMIN));



    /* Mode = 0 , TC
       Mode = 1 , Inverse TC
       Mode = 2 , Bypass
       Mode = 3 , SDR TC  */

    if(DM_NORMAL_TC == pDisplayTuning->tMode)       /* Tone Curve */
    {
        #define INV_TMIDBIASMAX (68267) //round(2^12/0.06)
        temp64 = (int64_t)pDisplayTuning->tMidPQBias * INV_TMIDBIASMAX;
        temp64 = temp64 >> 12;
        temp64 = (int64_t)uabs_ls(temp64);
        TMidOffset = (int32_t)sminss(temp64,0x7FFFFFFF);
        Clip = sadd_ss(TMidOffset , smpy_ss((0x7FFFFFFF-TMidOffset),Clip));

        CalcMappedParams2pxx(Crush,Mid,Clip,
                        pDisplayTuning,
                        SminPQ,SmaxPQ,
                        Sdiagonalinches,
                        &TMin,&TMid,&TMax,&SMid,&Slope);

        CalcCurveParams(Crush, SMid,Clip , TMin,
                        TMid, TMax, Slope, pDisplayTuning,
                        pParam1,pParam2,pParam3,pSoR);


        if(toneCurveLUT == NULL)
            toneCurve515LUT = toneCurve515LUT+1;

        GenerateToneCurveLut(0,0x7fffffff,pParam1,pParam2,pParam3,*pSoR,pDisplayTuning,
                            sTrimSlope,sTrimPower,sTrimOffset,
                            toneCurve515LUT,toneCurveLUT, 512, 0);

        if(toneCurveLUT == NULL) {
          // duplicate edge points  only used for interpolation
          toneCurve515LUT = toneCurve515LUT-1;
          toneCurve515LUT[0] = toneCurve515LUT[1];
          toneCurve515LUT[513] = toneCurve515LUT[512];
          toneCurve515LUT[514] = toneCurve515LUT[512];
        }
    }
    else if(DM_INVERSE_TC==pDisplayTuning->tMode)    /* Inverse Tone Curve */
    {
        CalcMappedParams2pxx(Crush,Mid,Clip,
                        pDisplayTuning,
                        SminPQ,SmaxPQ,
                        Sdiagonalinches,
                        &TMin,&TMid,&TMax,&SMid,&Slope);

        CalcCurveParams(Crush, SMid, Clip, TMin,
                        TMid, TMax, Slope, pDisplayTuning,
                        pParam1,pParam2,pParam3,pSoR);


        if(toneCurveLUT == NULL)
            toneCurve515LUT = toneCurve515LUT+1;

        GenerateToneCurveLut(0,0x7fffffff,pParam1,pParam2,pParam3,*pSoR,pDisplayTuning,
                            sTrimSlope,sTrimPower,sTrimOffset,
                            toneCurve515LUT,toneCurveLUT, 512, 0);

        if(toneCurveLUT == NULL) {
          // duplicate edge points  only used for interpolation
          toneCurve515LUT = toneCurve515LUT-1;
          toneCurve515LUT[0] = toneCurve515LUT[1];
          toneCurve515LUT[513] = toneCurve515LUT[512];
          toneCurve515LUT[514] = toneCurve515LUT[512];
        }
    }
    else if(DM_SDRX_TC==pDisplayTuning->tMode)   /* SDR Tone Curve */
    {
    }
    else           /* bypass */
    {
        if(toneCurveLUT == NULL)
            toneCurve515LUT = toneCurve515LUT+1;

        GenerateToneCurveLut(0,0x7fffffff,pParam1,pParam2,pParam3,*pSoR,pDisplayTuning,
                            sTrimSlope,sTrimPower,sTrimOffset,
                            toneCurve515LUT,toneCurveLUT, 512, 0);

        if(toneCurveLUT == NULL) {
          // duplicate edge points  only used for interpolation
          toneCurve515LUT = toneCurve515LUT-1;
          toneCurve515LUT[0] = toneCurve515LUT[1];
          toneCurve515LUT[513] = toneCurve515LUT[512];
          toneCurve515LUT[514] = toneCurve515LUT[512];
        }
    }      /* end of if Mode */
}




