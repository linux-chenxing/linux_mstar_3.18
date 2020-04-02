/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws 
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.  
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom 
* without the express permission of Dolby Laboratories is prohibited.  


* Copyright 2011 - 2015 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/


#ifndef _DLB_DISPLAY_MGMT_H_
#define _DLB_DISPLAY_MGMT_H_

#include "DMFxPtQs.h"
#include "toneCurve.h"
#include "dolbyTypeToMstarType.h"


void CreateToneCurve2pxx(
                     srcMetadata_t_     *pSrcMetadata,
                     displayPqTuning_t_ *pDisplayTuning,
                     val_norm_pair_t    *pParam1,
                     val_norm_pair_t    *pParam2,
                     val_norm_pair_t    *pParam3,
                     int32_t            *pSoR,
                     int16_t           *toneCurve515LUT,
                     int16_t           *toneCurveLUT);



void CreateToneCurve3pxx(
                     srcMetadata_t_     *pSrcMetadata,
                     displayPqTuning_t_ *pDisplayTuning,
                     val_norm_pair_t    *pParam1,
                     val_norm_pair_t    *pParam2,
                     val_norm_pair_t    *pParam3,
                     int32_t            *pSoR,
                     int16_t           *pTmapI,
                     int16_t           *pTmapS,
                     int16_t           *pSmapI,
                     int16_t           *pSmapS,
                     int32_t            *ratio,
                     int16_t            lutSize,
                     int16_t            clampOut
                     );

#endif


