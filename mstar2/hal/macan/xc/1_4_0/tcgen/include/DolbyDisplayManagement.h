/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom
* without the express permission of Dolby Laboratories is prohibited.


* Copyright 2011 - 2013 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/


#ifndef _DLB_DISPLAY_MGMT_H_
#define _DLB_DISPLAY_MGMT_H_

#include "DMFxPtQs.h"
#include "trims.h"
#include "lc_2x1d_luts.h"
#include "graphics_dm_3d_lut.h"

typedef struct displayPqTuning_t_
{
    S32         tMinPQ;
    S32         tMaxPQ;
    S32         tMinPQBias;
    S32         tMidPQBias;
    S32         tMaxPQBias;
    S32         tContrast;
    S32         tBrightness;
    S32         tDiagonalInches;
    S32         tRolloff;
    S32         tRolloffInv;
    U16         tMode;
} displayPqTuning_t_;

typedef struct val_norm_pair_t_
{
    S32         val;
    U16         norm;
} val_norm_pair_t;

void CreateToneCurve(S32 Crush,S32 Mid,S32 Clip,
                     S32 SminPQ,S32 SmaxPQ,S32 Sdiagonalinches,
                     S32 sTrimSlope ,S32 sTrimOffset , S32 sTrimPower,
                     displayPqTuning_t_ *pTgtTuning,
                     val_norm_pair_t *cParam1,
                     val_norm_pair_t *cParam2,
                     val_norm_pair_t *cParam3,
                     S32 *slopeOverRollOff,
                     U16 *toneCurve515LUT,U16 *toneCurveLUT);

void DoVi_Calc_ToneMappingLut(S32 Crush,S32 Mid,S32 Clip,
                              S32 SminPQ,S32 SmaxPQ,S32 Sdiagonalinches,
                              S32 sTrimSlope ,S32 sTrimOffset , S32 sTrimPower,
                              S32 tMinPQ, S32 tMaxPQ, S32 tDiagonalInches,
                              S32 tMinPQBias, S32 tMidPQBias, S32 tMaxPQBias,
                              S32 tContrast, S32 tBrightness, S32 tRolloff,
                              U16 tMode, U16 *toneCurve515LUT);


#endif


