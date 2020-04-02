/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom
* without the express permission of Dolby Laboratories is prohibited.


* Copyright 2011 - 2015 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/


#ifndef _DLB_TONECURVE_H_
#define _DLB_TONECURVE_H_

typedef struct displayPqTuning_t_{
int32_t         tMinPQ;
int32_t         tMaxPQ;
int32_t         tMinPQBias;
int32_t         tMidPQBias;
int32_t         tMaxPQBias;
int32_t         tContrast;
int32_t         tBrightness;
int32_t         tDiagonalInches;
int32_t         tBrightPreserve;
int32_t         tRolloff;
int32_t         tRolloffInv;
uint16_t        tMode;
}displayPqTuning_t_;

typedef struct   srcMetadata_t_{
int32_t    Crush;
int32_t    Mid;
int32_t    Clip;
int32_t    SminPQ;
int32_t    SmaxPQ;
int32_t    KeyWeight ;
int32_t    sTrimSlope;
int32_t    sTrimOffset;
int32_t    sTrimPower;
int32_t    ChromaWeight;
int32_t    SaturaGain;
int32_t    Sdiagonalinches;
int32_t    IntensityVWeight;
int32_t    ChromaVWeight;

}srcMetadata_t_;


typedef struct val_norm_pair_t_{
int32_t         val;
uint16_t         norm;
}val_norm_pair_t;

typedef enum eToneCurveMode_{
    DM_NORMAL_TC   =0,
    DM_INVERSE_TC  =1,
    DM_PASSTHROUGH_TC =2,
    DM_SDRX_TC     =3
}eToneCurveMode;

void CalcMappedParams3pxx(  int32_t SMax,
                            srcMetadata_t_         *pSrcMetadata,
                            displayPqTuning_t_ *pDisplayTuning,
                            int32_t *pTMin,int32_t *pTMid,int32_t *pTMax,int32_t *pSMid,
                            int32_t *pSlope,int32_t *ps2t_ratio, int32_t *pscreen_ratio);

void CalcMappedParams2pxx(int32_t Crush,int32_t Mid,int32_t Clip,
                         displayPqTuning_t_    *pDisplayTuning,
                         int32_t SminPQ,int32_t SmaxPQ,
                         int32_t Sdiagonalinches,int32_t *TMin,int32_t *TMid,int32_t *TMax ,int32_t *SMid, int32_t *Slope);

void CalcCurveParams    (int32_t SMin,int32_t SMid,int32_t SMax ,int32_t TMin,int32_t TMid,int32_t TMax,int32_t Slope,
                         displayPqTuning_t_     *pDisplayTuning,
                         val_norm_pair_t        *pParam1,
                         val_norm_pair_t        *pParam2,
                         val_norm_pair_t        *pParam3,
                         int32_t                *pSoR);

void  GenerateToneCurveLut(int32_t              xMin,
                           int32_t              xMax,
                           val_norm_pair_t      *pParam1,
                           val_norm_pair_t      *pParam2,
                           val_norm_pair_t      *pParam3,
                           int32_t               SoR,
                           displayPqTuning_t_   *pDisplayTuning,
                           int32_t sTrimSlope , int32_t sTrimPower,int32_t sTrimOffset ,
                           int16_t *toneCurve515LUT,
                           int16_t *toneCurve4KLUT,
                           int16_t lutSize,
                           int16_t clampOut
                           );


#endif


