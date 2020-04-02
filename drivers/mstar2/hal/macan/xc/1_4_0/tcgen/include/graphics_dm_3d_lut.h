/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom
* without the express permission of Dolby Laboratories is prohibited.


* Copyright 2011 - 2013 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/


#ifndef _GRAPHICS_DM_3D_LUT_
#define _GRAPHICS_DM_3D_LUT_

#define GRAPHIC_3D_LUTSIZE 17
#define QRGB2LMS2RGB_COEFF 25

#define GRAPHICS_3D_LUT_PQ_OUTPUT 0

#if 1==GRAPHICS_3D_LUT_PQ_OUTPUT
typedef S16 lut3d_t ;
#else
typedef U32 lut3d_t ;
#endif

typedef enum
{
    EOTF_BT1886,
    EOTF_PQ
} SEOTF_T;

typedef struct dm_params
{
    SEOTF_T SEOTF;             /* 0=Gamma , 1=PQ */
    SEOTF_T TEOTF;             /* 0=Gamma , 1=PQ */
    S32  SRGB2LMSmat[3][3];
    S32  TLMS2RGBmat[3][3];
    U32  Sgamma;
    U16  SmaxPQ;
    U16  SminPQ;
    U32  Tgamma;
    U16  TmaxPQ;
    U16  TminPQ;
    U16  graphics_3d_lut_size;
    S32  Tdiagonalinches;
    S32  Sdiagonalinches;
} dm_params_t;


typedef struct tc_params
{
    S32 Crush;
    S32 Mid;
    S32 Clip;
    S32 RolloffInv;
    S32 Rolloff;
    U16 Mode;
    U16 tcLutOut[4096];
    U16 tcLutOut515[515];

} tc_params_t;


/* Fx to generate Gamma2LX as per ,
 *  Ta = (Smax.^(1/Sgamma)-Smin.^(1/Sgamma)).^Sgamma;
 *  Tb = Smin.^(1/Sgamma) / (Smax.^(1/Sgamma) - Smin.^(1/Sgamma));
 *  Y = BT18862LX(X,a,b,g);
 *  Sgamma       -  QGAMMA
 *  uSmax,uSmin  -  QLINEAR (0.14.18)
 *  SRangeMin    -  Lowest point on the Xaxis where the curve starts , below this point the data is replicated.
 *  SRangeMax    -  Highest point on the Xaxis where the curve ends , above this point the data is replicated.
 *  pGamma2PQLut -  Pointer to the generated LUT
 */
void Gamma2LX(S32 Sgamma , U32  uSmax , U32 uSmin , U16 SRangeMin , U16 SRangeMax , U16 lut_size, U32 *pGamma2L );

void PQ2L_Lut(U16 lut_size, U32 *pPQ2L);


#endif


