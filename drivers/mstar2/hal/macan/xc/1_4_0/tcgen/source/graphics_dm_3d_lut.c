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
#include "DolbyDisplayManagement.h"


//#include "DMFxPtQs.h"
//#include "lc_2x1d_luts.h"
//#include "graphics_dm_3d_lut.h"

#define PQ14_to_Linear_Use PQ14_to_Linear_11bLut //PQ14_to_Linear

void Gamma2LX(S32 Sgamma , U32  uSmax , U32 uSmin , U16 SRangeMin , U16 SRangeMax , U16 lut_size, U32 *pGamma2L )
{
    S32 iloop;
    S32  Ta , Tb  , log2T , lTgamma_n , TgammaS;
    S32 divFactor;
    U32 G2LX_Offset;

    calcTaTb(Sgamma ,  uSmax ,  uSmin , &Ta , &Tb);

    divFactor = 0x7FFFFFFF/(SRangeMax-SRangeMin);

    lTgamma_n   = unorm_s(Sgamma);
    TgammaS     = sshl_su(Sgamma,lTgamma_n);

    G2LX_Offset = 0;
    for(iloop = SRangeMin ; iloop < lut_size ; iloop++)
    {
        S32 X , ln1;
        S32 index;
        U32 Y ;
        U64 lltemp;


        index = (iloop - SRangeMin);
        lltemp = lmpy_ss(index,divFactor);
        X  = sshr_slu(lltemp, 31-QTB);


        /* Gamma to Linear */
        X = sadd_ss(X,Tb);
        log2T = fxLOG2Taylor(X,QLOG2OUT);
        log2T = ssub_ss(log2T,QTB << QLOG2OUT);     //log2(max(0,X+Tb))*2^26
        X     = smpy_ss(TgammaS,log2T);                   // Sgamma * log2(max(0,X+Tb))*2^(lTgamma_n+QGAMMA + QLOG2OUT-31)

#if 1   //costly but high precision         
        lltemp   = LfxPOW2Taylor(X,lTgamma_n+QGAMMA + QLOG2OUT-31);//,QINTERMEDIATE1); // result in Q32
        ln1      = unorm_ul(lltemp);
        lltemp   <<= (ln1);
        lltemp   >>= (32);
        Y        = (S32)lltemp;     // in Qln1
        lltemp   =  lmpy_ss(Y,Ta);
        ln1      =  ln1+QTA-QLINEAR;
        Y        =  sshr_ulu(lltemp,ln1);
#else
        Y        = fxPOW2Taylor(X,lTgamma_n+QGAMMA + QLOG2OUT-31,QINTERMEDIATE1);
        lltemp   =  lmpy_ss(Y,Ta);
        Y        =  sshr_ulu(lltemp,QINTERMEDIATE1+QTA-QLINEAR);
#endif

        if(0==index)     G2LX_Offset = Y;

        /* Linear to PQ */
        pGamma2L[iloop]     =  Y > 2621440000U ? 2621440000U : Y;        // last point in the LUt



    }// end of iloop


    for(iloop = 0 ; iloop < SRangeMin ; iloop++)
    {
        S32 X , ln1;
        S32 index;
        U32 Y ;
        S64 lY;
        U64 lltemp;


        index = (iloop - SRangeMin);
        index = uabs_ss(index);
        lltemp = lmpy_ss(index,divFactor);
        X  = sshr_slu(lltemp, 31-QTB);


        /* Gamma to Linear */
        X = sadd_ss(X,Tb);



        log2T = fxLOG2Taylor(X,QLOG2OUT);
        log2T = ssub_ss(log2T,QTB << QLOG2OUT);     //log2(max(0,X+Tb))*2^26
        X     = smpy_ss(TgammaS,log2T);                   // Sgamma * log2(max(0,X+Tb))*2^(lTgamma_n+QGAMMA + QLOG2OUT-31)

#if 1   //costly but high precision         
        lltemp   = LfxPOW2Taylor(X,lTgamma_n+QGAMMA + QLOG2OUT-31);//,QINTERMEDIATE1); // result in Q32
        ln1      = unorm_ul(lltemp);
        lltemp   <<= (ln1);
        lltemp   >>= (32);
        Y        = (S32)lltemp;     // in Qln1
        lltemp   =  lmpy_ss(Y,Ta);
        ln1      =  ln1+QTA-QLINEAR;
        Y        =  sshr_ulu(lltemp,ln1);
#else
        Y        = fxPOW2Taylor(X,lTgamma_n+QGAMMA + QLOG2OUT-31,QINTERMEDIATE1);
        lltemp   =  lmpy_ss(Y,Ta);
        Y        =  sshr_ulu(lltemp,QINTERMEDIATE1+QTA-QLINEAR);
#endif

        Y       =  Y > 2621440000U ? 2621440000U : Y;        // last point in the LUt

        lY = (S64)Y;
        lY = -lY + 2*G2LX_Offset;
        lY = smaxss(0,lY);

        pGamma2L[iloop] = (U32)lY;

    }// end of iloop




}/* end of Gamma2PQX */

void PQ2L_Lut(U16 lut_size, U32 *pPQ2L)
{
    U16 iloop;
    S32 divFactor;
    U64 lltemp;

    divFactor = 0x80000000/(lut_size-1);


    for(iloop=0 ; iloop<lut_size ; iloop++)
    {
        U16 inPQ;
        S32 iTemp;

        iTemp   = iloop << QPQIN;
        lltemp = lmpy_ss(iTemp,divFactor);
        inPQ = (U16)(lltemp >> 31);

        pPQ2L[iloop] =  PQ14_to_Linear_Use(inPQ)  ;
    }


}

