/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom
* without the express permission of Dolby Laboratories is prohibited.


* Copyright 2011 - 2013 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/


#ifndef _DLB_TRIMS_H_
#define _DLB_TRIMS_H_

#define MAX_TRIMS 16

typedef struct trimParams
{
    S32 TargetMax;
    S32 Slope;
    S32 Offset;
    S32 Power;
    S32 ChromaWeight;
    S32 SaturationGain;
} TRIM_PARAMS;


void CalcTrimValues(
    TRIM_PARAMS *dstTrim,
    TRIM_PARAMS MDTrims[MAX_TRIMS+1],
    TRIM_PARAMS *defaultTrim ,
    U16         numMDTrims,
    S32         SmaxPQ,S32 TmaxPQ);

#endif

