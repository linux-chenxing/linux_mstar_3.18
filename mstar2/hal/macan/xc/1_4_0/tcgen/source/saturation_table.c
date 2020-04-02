/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom
* without the express permission of Dolby Laboratories is prohibited.


* Copyright 2011 - 2013 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/


#include "fx_math.h"
#include "fixed_point_defines.h"
#include "sat_lut_cvdr.h"
#include "sat_lut_vdrinv.h"
#include "sat_lut_cmapinv.h"




void getSaturationTable(U16* sVDR , U16* sMapped , U32* sSaturation , U32 slength)
{

    U32 iloop;
    S32 stemp0 , stemp1, stemp2,stemp3,stemp4;
    S64 ltemp1;


    for (iloop=0 ; iloop < slength ; iloop++)
    {
        stemp0 = *sVDR++;
        stemp1 = *sMapped;

        stemp2 = sat_lut_cvdr[stemp0];
        stemp3 = sat_lut_cmapinv[stemp1];
        stemp4 = sat_lut_vdrinv[stemp0];


        ltemp1 = lmpy_ss(stemp2,stemp3);// 1.12 (u), 0.20(u) , intermediate output is 1.32(u)
        ltemp1 = ltemp1 & 0x1FFFFFFFF;      // To reprodce 1.31 intermediate
        stemp1 = sshr_slu(ltemp1,SATURATION_CVDR_SCALE+SATURATION_CINV_SCALE-19);//bring the output back to 1.19(u)
        stemp1 = stemp1 & 0xFFFFF; // to reproduce 1.19(u)

        ltemp1 = lmpy_ss(*sMapped++,stemp1);//   0.12(u), 1.19(u) , intermediate output is 1.31
        ltemp1 = ltemp1 & 0x7FFFFFFF;

        stemp1 = sshr_slu(ltemp1,12+19-19);    // bring the output back to 1.19(u) , Mapped(:,:,1) .* CVDR./CMapped
        stemp1 = stemp1 & 0xFFFFF; // to reproduce 1.19(u)

        ltemp1 = lmpy_ss(stemp1,stemp4);       // 1.19 , 9.15 intermediate 10.34
        ltemp1 = ltemp1 & 0xFFFFFFFFFFF;

        stemp1 = sshr_slu(ltemp1,19+SATURATION_VINV_SCALE-31); // output n Q31

        *sSaturation++ = smaxss(stemp1,1<<30);// max(0.5,S)
    }


}



