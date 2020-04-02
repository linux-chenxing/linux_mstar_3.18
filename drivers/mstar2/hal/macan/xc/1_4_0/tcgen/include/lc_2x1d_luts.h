/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom
* without the express permission of Dolby Laboratories is prohibited.


* Copyright 2011 - 2013 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/


#ifndef _DLB_LC_LUTS_
#define _DLB_LC_LUTS_

#include "lc_2x1d_lutsQ.h"

#define GAMMA2PQ_LOG2LUTSIZE    9
#define GAMMA2PQX_LOG2LUTSIZE   9
#define GAMMA2PQ_LUTSIZE        1<<(GAMMA2PQ_LOG2LUTSIZE)
#define GAMMA2PQX_LUTSIZE       1<<(GAMMA2PQX_LOG2LUTSIZE)
#define QTMAXTMIN               14
#define QGAMMA                  15
#define QDIFF                   (31-14)
#define QTA                     17
#define QTB                     25
#define QINTERMEDIATE1          20
#define QGAMMA2PQ               14
#define QPQ2GAMMA               12
#define QPQIN                   14



/* Fx to generate Gamma2PQ 512Pt LUT as per ,
 *  Ta = (Smax.^(1/Sgamma)-Smin.^(1/Sgamma)).^Sgamma;
 *  Tb = Smin.^(1/Sgamma) / (Smax.^(1/Sgamma) - Smin.^(1/Sgamma));
 *  Y = Ta*(max(0,X+Tb)).^Sgamma;
 *  Y = L2PQ(Y);
 *  Sgamma       -  QGAMMA
 *  uSmax,uSmin  -  QLINEAR (0.14.18)
 *  SRangeMin    -  Lowest point on the Xaxis where the curve starts , below this point the data is replicated.
 *  SRangeMax    -  Highest point on the Xaxis where the curve ends , above this point the data is replicated.
 *  pGamma2PQLut -  Pointer to the generated LUT
 */
void Gamma2PQ(S32 Sgamma , U32  uSmax , U32 uSmin , U16 SRangeMin , U16 SRangeMax ,  U16 *pGamma2PQLut);

/* Fx to generate Gamma2PQ 512Pt LUT as per ,
 *  Ta = (Smax.^(1/Sgamma)-Smin.^(1/Sgamma)).^Sgamma;
 *  Tb = Smin.^(1/Sgamma) / (Smax.^(1/Sgamma) - Smin.^(1/Sgamma));
 *  Y = BT18862PQX(X,a,b,g);
 *  Sgamma       -  QGAMMA
 *  uSmax,uSmin  -  QLINEAR (0.14.18)
 *  SRangeMin    -  Lowest point on the Xaxis where the curve starts , below this point the data is replicated.
 *  SRangeMax    -  Highest point on the Xaxis where the curve ends , above this point the data is replicated.
 *  pGamma2PQLut -  Pointer to the generated LUT
 */
void Gamma2PQX(S32 Sgamma , U32  uSmax , U32 uSmin , U16 SRangeMin , U16 SRangeMax ,  S16 *pGamma2PQLut);

/* Fx to generate Gamma2PQ 512Pt LUT as per ,
 *  Ta = (Smax.^(1/Sgamma)-Smin.^(1/Sgamma)).^Sgamma;
 *  Tb = Smin.^(1/Sgamma) / (Smax.^(1/Sgamma) - Smin.^(1/Sgamma));
 *  Y = PQ2L(X);
 *  Y = ((max(0,Y / Ta)).^(1/Sgamma) - Tb)
 *  Sgamma       -  QGAMMA
 *  uSmax,uSmin  -  QLINEAR (0.14.18)
 *  SRangeMin    -  Lowest point on the Xaxis where the curve starts , below this point the data is replicated.
 *  SRangeMax    -  Highest point on the Xaxis where the curve ends , above this point the data is replicated.
 *  pPQ2GammaLut -  Pointer to the generated LUT
 */
void PQ2Gamma(S32 Tgamma , U32  uTmax , U32 uTmin , U16 TRangeMin , U16 TRangeMax , U16 *pPQ2GammaLut);


/* Function to calculate Ta and Tb for GammaqL and PQ2L
*  Sgamma       -  QGAMMA
*  uSmax,uSmin  -  QLINEAR (0.14.18)
*/
void calcTaTb(S32 Sgamma , U32  uSmax , U32 uSmin , S32 *Ta , S32 *Tb);

/*  Function to convert 14bPQ input to 32bLinear in QLINEAR (0.14.18)
 */
U32  PQ14_to_Linear(U16 inPQ);

U32  PQ14_to_Linear_11bLut(U16 inPQ);

/*  Function to convert  32bLinear to 14bPQ  */
U16 Linear_to_PQ14(U32 uLinear);


#endif




